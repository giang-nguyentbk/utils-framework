#include <unistd.h>
#include <string.h>

#include "util_framework_tpt_provider.h"
#include <traceIf.h>

#include "threadLocalIf.h"
#include "eventLoopIf.h"
#include "eventLoopImpl.h"

using namespace UtilsFramework::ThreadLocal::V1;

namespace UtilsFramework
{
namespace EventLoop
{
namespace V1
{

IEventLoop& IEventLoop::getInstance()
{
    	return EventLoopImpl::getInstance();
}

EventLoopImpl& EventLoopImpl::getInstance()
{
    	return IThreadLocal<EventLoopImpl>::get();
}

void EventLoopImpl::reset()
{
	TPT_TRACE(TRACE_INFO, "Resetting Event Loop...");
    	IThreadLocal<EventLoopImpl>::reset();
}

EventLoopImpl::EventLoopImpl() :
	m_epfd(-1),
        m_threadId(std::this_thread::get_id()),
        m_syscallWrapper(std::make_shared<EventLoopSyscallWrapper>()),
        m_isRunning(false)
{
}

EventLoopImpl::~EventLoopImpl()
{
	TPT_TRACE(TRACE_INFO, "~EventLoopImpl is called!");
	if(m_epfd != -1)
	{
		TPT_TRACE(TRACE_INFO, "~EventLoopImpl - close m_epfd!");
		close(m_epfd);
	}
}

IEventLoop::ReturnCode EventLoopImpl::addFdHandler(int fd, uint32_t eventMask, const CallbackFunc& callback)
{
	// Check if current thread is thread local which owns this Event Loop instance
	if(m_threadId != std::this_thread::get_id())
	{
		TPT_TRACE(TRACE_ERROR, "addFdHandler - Not a thread local!");
		return IEventLoop::ReturnCode::NOT_THREAD_LOCAL;
	}

	// Find in the map the respective fd
	auto fd_it = m_fdHandlers.find(fd);
	if(fd_it != m_fdHandlers.end())
	{
		TPT_TRACE(TRACE_ABN, "addFdHandler - FD %d handler already exists!", fd_it->first);
		return IEventLoop::ReturnCode::ALREADY_EXISTS;
	}

	// Convert local events to epoll events
	uint32_t epollEvents = convertToEpollEvents(eventMask);
	if(0 == epollEvents)
	{
		TPT_TRACE(TRACE_ERROR, "addFdHandler - Failed to convertToEpollEvents()!");
		return IEventLoop::ReturnCode::INVALID_ARG;
	}

	// If epoll FD instance hasn't been created, create it
	if(-1 == m_epfd)
	{
		// Use EPOLL_CLOEXEC flag to avoid potential race condition in multithreaded application
		TPT_TRACE(TRACE_INFO, "addFdHandler - m_epfd hasn't been created yet, create it!");
		m_epfd = m_syscallWrapper->epoll_create1(EPOLL_CLOEXEC);
		if(-1 == m_epfd)
		{
			TPT_TRACE(TRACE_ERROR, "addFdHandler - Failed to epoll_create() m_epfd!");
		return IEventLoop::ReturnCode::INTERNAL_FAULT;
		}
	}

	// If everything ok, create a pointer to a FdHandler object
	auto fdHandler = std::make_shared<FdHandler>();
	fdHandler->fd = fd;
	fdHandler->epollEvents = epollEvents;
	fdHandler->callback = callback;

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
		TPT_TRACE(TRACE_ERROR, "addFdHandler - Failed to epoll_ctl() with EPOLL_CTL_ADD for FD %d", fd);
		return IEventLoop::ReturnCode::INTERNAL_FAULT;
	}

	// Also add to our map for self management
	m_fdHandlers.emplace(fd, fdHandler);

	TPT_TRACE(TRACE_INFO, "addFdHandler - Added FD %d handler successfully!", fd);
	return IEventLoop::ReturnCode::NORMAL;
}

IEventLoop::ReturnCode EventLoopImpl::updateFdEvents(int fd, uint32_t eventMask)
{
	// Check if current thread is thread local which owns this Event Loop instance
	if(m_threadId != std::this_thread::get_id())
	{
		TPT_TRACE(TRACE_ERROR, "updateFdEvents - Not a thread local!");
		return IEventLoop::ReturnCode::NOT_THREAD_LOCAL;
	}

	// Find in the map the respective fd
	auto fd_it = m_fdHandlers.find(fd);
	if(fd_it == m_fdHandlers.end())
	{
		TPT_TRACE(TRACE_ERROR, "updateFdEvents - FD %d not found!", fd);
		return IEventLoop::ReturnCode::NOT_FOUND;
	}

	// Convert local events to epoll events
	uint32_t epollEvents = convertToEpollEvents(eventMask);
	if(0 == epollEvents)
	{
		TPT_TRACE(TRACE_ERROR, "updateFdEvents - Failed to convertToEpollEvents()!");
		return IEventLoop::ReturnCode::INVALID_ARG;
	}

	// Create an epoll event struct to ask epoll instance for eventMask changes on respective FD in the interest list.
	struct epoll_event epEvent;
	memset(&epEvent, 0, sizeof(struct epoll_event));
	epEvent.events = epollEvents;
	fd_it->second->epollEvents = epollEvents;
	epEvent.data.ptr = fd_it->second.get();
	if(m_syscallWrapper->epoll_ctl(m_epfd, EPOLL_CTL_MOD, fd, &epEvent) == -1)
	{
		TPT_TRACE(TRACE_ERROR, "updateFdEvents - Failed to epoll_ctl() with EPOLL_CTL_MOD for FD %d", fd);
		return IEventLoop::ReturnCode::INTERNAL_FAULT;
	}

	TPT_TRACE(TRACE_INFO, "updateFdEvents - Modified FD %d handler successfully!", fd);
	return IEventLoop::ReturnCode::NORMAL;
}

IEventLoop::ReturnCode EventLoopImpl::removeFdHandler(int fd)
{
	// Check if current thread is thread local which owns this Event Loop instance
	if(m_threadId != std::this_thread::get_id())
	{
		TPT_TRACE(TRACE_ERROR, "removeFdHandler - Not a thread local!");
		return IEventLoop::ReturnCode::NOT_THREAD_LOCAL;
	}

	// Find in the map the respective fd
	auto fd_it = m_fdHandlers.find(fd);
	if(fd_it == m_fdHandlers.end())
	{
		TPT_TRACE(TRACE_ERROR, "removeFdHandler - FD %d not found!", fd);
		return IEventLoop::ReturnCode::NOT_FOUND;
	}

	// Request epoll instance to delete the fd from the interest list.
	(void) m_syscallWrapper->epoll_ctl(m_epfd, EPOLL_CTL_DEL, fd, nullptr);

	/*  Sometime, there could a not yet handled event for the removed FD in current batch
	*   Therefore, we will temporarily keep the corresponding FdHandler shared_ptr "alive" in our m_removedFdHandlers
	*   until all events in current batch have been carried out.
	*   But anyway, we set the event mask to 0 to avoid those event's callbacks being really executed.
	*/
	fd_it->second->epollEvents = 0;
	m_removedFdHandlers.emplace(fd, fd_it->second);

	m_fdHandlers.erase(fd_it);

	TPT_TRACE(TRACE_INFO, "removeFdHandler - Removed FD %d handler successfully!", fd);
	return IEventLoop::ReturnCode::NORMAL;
}

IEventLoop::ReturnCode EventLoopImpl::run()
{
	// Check if current thread is thread local which owns this Event Loop instance
	if(m_threadId != std::this_thread::get_id())
	{
		TPT_TRACE(TRACE_ERROR, "run - Not a thread local!");
		return IEventLoop::ReturnCode::NOT_THREAD_LOCAL;
	}

	TPT_TRACE(TRACE_INFO, "run - Starting Event Loop...");
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
			TPT_TRACE(TRACE_INFO, "run - Current batch: num events: %d", eventCount);
			for(int i = 0; i < eventCount; ++i)
			{
				handleEpollEvent(events[i]);
			}	
		} else if (eventCount == -1 && errno != EINTR)
		{
			TPT_TRACE(TRACE_ERROR, "run - Failed to epoll_wait()");
			return IEventLoop::ReturnCode::INTERNAL_FAULT;
		}
	}

	return IEventLoop::ReturnCode::NORMAL;
}

IEventLoop::ReturnCode EventLoopImpl::stop()
{
	// Check if current thread is thread local which owns this Event Loop instance
	if(m_threadId != std::this_thread::get_id())
	{
		TPT_TRACE(TRACE_ERROR, "stop - Not a thread local!");
		return IEventLoop::ReturnCode::NOT_THREAD_LOCAL;
	}

	m_isRunning = false;

	TPT_TRACE(TRACE_INFO, "stop - Exiting Event Loop...");
	return IEventLoop::ReturnCode::NORMAL;
}

void EventLoopImpl::handleEpollEvent(const struct epoll_event& event)
{
	FdHandler* fdHandler = static_cast<FdHandler*>(event.data.ptr);

	uint32_t eventMask = convertToLocalEvents(event.events & fdHandler->epollEvents);
	// For removed FDs, eventMask = 0, so those FD's callbacks will be skipped
	TPT_TRACE(TRACE_INFO, "handleEpollEvent - event = %d, eventMask = %d", +event.events, +eventMask);
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

	TPT_TRACE(TRACE_INFO, "convertToEpollEvents - localEvents = %d, epollEvents = %d", +localEvents, +epollEvents);
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

	TPT_TRACE(TRACE_INFO, "convertToLocalEvents - epollEvents = %d, localEvents = %d", +epollEvents, +localEvents);
	return localEvents;
}

void EventLoopImpl::dispatchEvent(const CallbackFunc& callback, int fd, uint32_t eventMask)
{
	TPT_TRACE(TRACE_INFO, "dispatchEvent - Invoking callback for fd  %d", fd);
    	callback(fd, eventMask);
}

void EventLoopImpl::executeScheduledEvents()
{
	auto it = m_scheduledEvents.begin();

	TPT_TRACE(TRACE_INFO, "executeScheduledEvents - m_scheduledEvents size = %d", m_scheduledEvents.size());
	while(it != m_scheduledEvents.end())
	{
		auto eventHandler = *it;
		m_scheduledEvents.erase(it);

		TPT_TRACE(TRACE_INFO, "executeScheduledEvents - Invoking eventHandler()!");
		eventHandler();

		// Note that: be careful with an infinite loop of executeScheduledEvents(), which means somehow eventHandler()
		// above repeatedly add another event(s) to this m_scheduledEvents list again and again.
		it = m_scheduledEvents.begin();
	}
}

IEventLoop::ReturnCode EventLoopImpl::scheduleEvent(const EventHandlerFunc& eventHandler)
{
	// Check if current thread is thread local which owns this Event Loop instance
	if(m_threadId != std::this_thread::get_id())
	{
		TPT_TRACE(TRACE_ERROR, "scheduleEvent - Not a thread local!");
		return IEventLoop::ReturnCode::NOT_THREAD_LOCAL;
	}

	m_scheduledEvents.push_back(eventHandler);

	TPT_TRACE(TRACE_INFO, "scheduleEvent - Scheduled a new eventHandler to m_scheduledEvents successfully!");
	return IEventLoop::ReturnCode::NORMAL;
}

} // namespace V1

} // namespace EventLoop

} // namespace UtilsFramework