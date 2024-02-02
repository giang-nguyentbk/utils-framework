#pragma once

#include <unordered_map>
#include <thread>
#include <memory>
#include <vector>
#include <set>

#include "eventLoopAPI.h"
#include "eventLoopSyscallWrapper.h"

namespace CommonAPIs
{
namespace EventLoop
{
namespace V1
{

class EventLoopImpl : public EventLoopAPI
{
public:
    static EventLoopImpl& getInstance();
    static void reset();

    ReturnCode addFdHandler(int fd, uint32_t eventMask, const CallBackFunc& callback) override;
    ReturnCode updateFdEvents(int fd, uint32_t eventMask) override;
    ReturnCode removeFdHandler(int fd) override;
    ReturnCode run() override;
    ReturnCode stop() override;
    ReturnCode scheduleEvent(const EventHandlerFunc& eventHandler) override;

    EventLoopImpl();
    virtual ~EventLoopImpl();

    // First prevent copy/move construtors
    EventLoopImpl(const EventLoopImpl&)               = delete;
    EventLoopImpl(EventLoopImpl&&)                    = delete;
    EventLoopImpl& operator=(const EventLoopImpl&)    = delete;
    EventLoopImpl& operator=(EventLoopImpl&&)         = delete;

private:
    void handEpollEvent(const struct epoll_event& event);
    void dispatchEvent(const CallBackFunc& callback, int fd, uint32_t eventMask);
    void executeScheduledEvents();

    /*! @brief Because our local events are:
    *           + FdEventIn     = 0x001
    *           + FdEventOut    = 0x002
    * While epol events defined in epoll.h are:
    *           + EPOLLIN       = 0x001
    *           + EPOLLOUT      = 0x040
    * 
    * So we need to convert between them.
    */
    uint32_t convertToEpollEvents(uint32_t localEvents);
    uint32_t convertToLocalEvents(uint32_t epollEvents);

    int m_epfd;
    std::thread::id m_threadId;
    std::shared_ptr<EventLoopSyscallWrapper> m_syscallWrapper;
    bool m_isRunning;

    struct FdHandler
    {
        int fd = -1;
        uint32_t epollEvents = 0;
        CallBackFunc callback;
    };

    using FdHandlerMap = std::unordered_map<int /*fd*/, std::shared_ptr<FdHandler> /*fdHandler*/>;
    FdHandlerMap m_fdHandlers;
    FdHandlerMap m_removedFdHandlers;

    std::vector<EventHandlerFunc> m_scheduledEvents;
    
}; // class EventLoopImpl

} // namespace V1

} // namespace EventLoop

} // namespace CommonAPIs