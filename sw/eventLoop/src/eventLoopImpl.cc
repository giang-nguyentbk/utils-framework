#include <unistd.h>
#include <string.h>

#include "threadLocalAPI.h"
#include "eventLoopAPI.h"
#include "eventLoopImpl.h"

using namespace CommonAPIs::ThreadLocal::V1;

namespace CommonAPIs
{
namespace EventLoop
{
namespace V1
{

EventLoopAPI& EventLoopAPI::getInstance()
{
    return EventLoopImpl::getInstance();
}

EventLoopImpl& EventLoopImpl::getInstance()
{
    return ThreadLocalLinuxAPI<EventLoopImpl>::get();
}

void EventLoopImpl::reset()
{
    ThreadLocalLinuxAPI<EventLoopImpl>::reset();
}

EventLoopImpl::EventLoopImpl()
    :   m_epfd(-1),
        m_threadId(std::this_thread::get_id()),
        m_syscallWrapper(std::make_shared<EventLoopSyscallWrapper>()),
        m_isRunning(false)
{
}

EventLoopImpl::~EventLoopImpl()
{
    if(-1 != m_epfd)
    {
        close(m_epfd);
    }
}

EventLoopAPI::ReturnCode EventLoopImpl::addFdHandler(int fd, uint32_t eventMask, const CallBackFunc& callback)
{
    // Check if current thread is thread local which owns this Event Loop instance
    if(m_threadId != std::this_thread::get_id())
    {
        return EventLoopAPI::ReturnCode::NOT_THREAD_LOCAL;
    }

    // Find in the map the respective fd
    auto fd_it = m_fdHandlers.find(fd);
    if(fd_it != m_fdHandlers.end())
    {
        return EventLoopAPI::ReturnCode::ALREADY_EXISTS;
    }

    // Convert local events to epoll events
    uint32_t epollEvents = convertToEpollEvents(eventMask);
    if(0 == epollEvents)
    {
        return EventLoopAPI::ReturnCode::INVALID_ARG;
    }

    // If epoll FD instance hasn't been created, create it
    if(-1 == m_epfd)
    {
        // Use EPOLL_CLOEXEC flag to avoid potential race condition in multithreaded application
        m_epfd = m_syscallWrapper->epoll_create1(EPOLL_CLOEXEC);
        if(-1 == m_epfd)
        {
            return EventLoopAPI::ReturnCode::INTERNAL_FAULT;
        }
    }

    // If everything ok, create a pointer to a FdHandler object
    auto fdHandler = std::make_shared<FdHandler>();
    fdHandler.fd = fd;
    fdHandler.epollEvents = epollEvents;
    fdHandler.callback = callback;

    // Then create a standard struct epoll_event used by epoll
    /*  Definition from <sys/epoll.h>
    *   struct epoll_event {
    *       uint32_t        events; // Epoll events
    *       epoll_data_t    data;   // User data variable, here is our FdHandler objects
    *   };
    * 
    *   union epoll_data {
    *       void*           ptr;
    *       int             fd;
    *       uint32_t        padding_32;
    *       uint64_t        padding_64;
    *   };
    * 
    *   typedef union epoll_data epoll_data_t;
    */
    struct epoll_event epEvent;
    memset(&epEvent, 0, sizeof(struct epoll_event));
    epEvent.events = epollEvents;
    epEvent.data.ptr = fdHandler.get();

    // Add a FD to the interest list of epoll instance which is referred by m_epfd
    if(m_syscallWrapper->epoll_ctl(m_epfd, EPOLL_CTL_ADD, fd, &epEvent) == -1)
    {
        return EventLoopAPI::ReturnCode::INTERNAL_FAULT;
    }

    // Also add to our map for self management
    m_fdHandlers.emplace(fd, fdHandler);

    return EventLoopAPI::ReturnCode::NORMAL;
}

EventLoopAPI::ReturnCode EventLoopImpl::updateFdEvents(int fd, uint32_t eventMask)
{
    // Check if current thread is thread local which owns this Event Loop instance
    if(m_threadId != std::this_thread::get_id())
    {
        return EventLoopAPI::ReturnCode::NOT_THREAD_LOCAL;
    }

    // Find in the map the respective fd
    auto fd_it = m_fdHandlers.find(fd);
    if(fd_it == m_fdHandlers.end())
    {
        return EventLoopAPI::ReturnCode::NOT_FOUND;
    }

    // Convert local events to epoll events
    uint32_t epollEvents = convertToEpollEvents(eventMask);
    if(0 == epollEvents)
    {
        return EventLoopAPI::ReturnCode::INVALID_ARG;
    }

    // Create an epoll event struct to ask epoll instance for eventMask changes on respective FD in the interest list.
    struct epoll_event epEvent;
    memset(&epEvent, 0, sizeof(struct epoll_event));
    epEvent.events = epollEvents;
    epEvent.data.ptr = fd_it->second.get();
    if(m_syscallWrapper->epoll_ctl(m_epfd, EPOLL_CTL_MOD, fd, &epEvent) == -1)
    {
        return EventLoopAPI::ReturnCode::INTERNAL_FAULT;
    }

    // Also update eventMask on respective FD in our map for self management
    fd_it->second->epollEvents = epollEvents;

    return EventLoopAPI::ReturnCode::NORMAL;
}

EventLoopAPI::ReturnCode EventLoopImpl::removeFdHandler(int fd)
{
    // Check if current thread is thread local which owns this Event Loop instance
    if(m_threadId != std::this_thread::get_id())
    {
        return EventLoopAPI::ReturnCode::NOT_THREAD_LOCAL;
    }

    // Find in the map the respective fd
    auto fd_it = m_fdHandlers.find(fd);
    if(fd_it == m_fdHandlers.end())
    {
        return EventLoopAPI::ReturnCode::NOT_FOUND;
    }

    // Request epoll instance to delete the fd from the interest list.
    (void) m_syscallWrapper->epoll_ctl(m_epfd, EPOLL_CTL_DEL, fd, nullptr)

    /*  Sometime, there could a not yet handled event for the removed FD in current batch
    *   Therefore, we will temporarily keep the corresponding FdHandler shared_ptr "alive" in our m_removedFdHandlers
    *   until all events in current batch have been carried out.
    *   But anyway, we set the event mask to 0 to avoid those event's callbacks being really executed.
    */
    fd_it->second->epollEvents = 0;
    m_removedFdHandlers.emplace(fd, it->second);

    m_fdHandlers.erase(fd_it);

    return EventLoopAPI::ReturnCode::NORMAL;
}

EventLoopAPI::ReturnCode EventLoopImpl::run()
{
    // Check if current thread is thread local which owns this Event Loop instance
    if(m_threadId != std::this_thread::get_id())
    {
        return EventLoopAPI::ReturnCode::NOT_THREAD_LOCAL;
    }

    m_isRunning = true;

    const int maxEvents = 3;
    struct epoll_event events[maxEvents];

    while(m_isRunning && !m_fdHandlers.empty())
    {
        // Clear all almost deleted FD Handlers in the previous event batch
        m_removedFdHandlers.clear();

        // Wait infinitely until receive an enough number (maxEvents) of events for all FDs in the interest list
        // or epoll_wait() is unblocked due to any reason such as another thread has added a new FD
        // to the interest list,...
        int eventCount = m_syscallWrapper->epoll_wait(m_epfd, events, maxEvents, -1);

        if(eventCount > 0)
        {
            for(int i = 0; i < eventCount; ++i)
            {
                handleEpollEvent(events[i]);
            }
        } else if (eventCount == -1 && errno != EINTR)
        {
            return EventLoopAPI::ReturnCode::INTERNAL_FAULT;
        }
        
    }

    return EventLoopAPI::ReturnCode::NORMAL;
}

EventLoopAPI::ReturnCode EventLoopImpl::stop()
{
    // Check if current thread is thread local which owns this Event Loop instance
    if(m_threadId != std::this_thread::get_id())
    {
        return EventLoopAPI::ReturnCode::NOT_THREAD_LOCAL;
    }

    m_isRunning = false;

    return EventLoopAPI::ReturnCode::NORMAL;
}

void handEpollEvent(const struct epoll_event& event)
{
    FdHandler* fdHandler = static_cast<FdHandler*>(event.data.ptr);

    uint32_t eventMask = convertToLocalEvents(event.events & fdHandler->epollEvents);
    // For removed FDs, eventMask = 0, so those FD's callbacks will be skipped
    if(eventMask)
    {
        dispatchEvent(fdHandler->callback, fdHandler->fd, eventMask);

        executeScheduledEvents();
    }
}

uint32_t EventLoopImpl::convertToEpollEvents(uint32_t localEvents)
{
    uint32_t epollEvents = 0;

    if(localEvents & FdEventIn)
    {
        epollEvents |= EPOLLIN;
    } 
    
    if(localEvents & FdEventOut)
    {
        epollEvents |= EPOLLOUT;
    }

    return epollEvents;
}

uint32_t EventLoopImpl::convertToLocalEvents(uint32_t epollEvents)
{
    uint32_t localEvents = 0;

    if(epollEvents & EPOLLIN)
    {
        localEvents |= FdEventIn;
    } 
    
    if(epollEvents & EPOLLOUT)
    {
        localEvents |= FdEventOut;
    }

    return localEvents;
}

void EventLoopImpl::dispatchEvent(const CallbackFunc& callback, int fd, uint32_t eventMask)
{
    callback(fd, eventMask);
}

EventLoopAPI::ReturnCode EventLoopImpl::scheduleEvent(const EventHandlerFunc& eventHandler)
{
    // Check if current thread is thread local which owns this Event Loop instance
    if(m_threadId != std::this_thread::get_id())
    {
        return EventLoopAPI::ReturnCode::NOT_THREAD_LOCAL;
    }

    m_scheduledEvents.push_back(eventHandler);

    return EventLoopAPI::ReturnCode::NORMAL;
}

} // namespace V1

} // namespace EventLoop

} // namespace CommonAPIs