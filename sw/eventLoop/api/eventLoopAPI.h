#pragma once

#include <cstdint>
#include <functional>

namespace CommonAPIs
{
namespace EventLoop
{
namespace V1
{
/*! @brief Event Loop is basically a infinite loop.
* + Responsible for managing a list of File Descriptors.
* + Continuously wait for some epoll_event using epoll_wait() system call.
* + Handle those epoll_event by calling callback functions on correct FDs.
*
* Let's imagine a simple example where every people (processes) has their own phone numbers (FDs),
* someone (another process) wants to send a message (epoll_event) to you via your FD.
* That simply shows you what File Descriptor is and how it works.
*
* Example usage:
* 
* </code>
*   using namespace CommonAPIs::EventLoop::V1;
*
*   int main()
*   {
*       ... 
*   
*       EventLoopAPI::ReturnCode result;
*       EventLoopAPI& eventLoop = EventLoopAPI::getInstance();
*   
*       // Add some FDs to your event loop, similarly to the way you add some phone numbers
*       // and what to do when someone calls to those phone numbers.
*       result = eventLoop.addFdHandler(yourFd, EventLoopAPI::FdEventIn, yourCallbackFunc);
*   
*       ...
*   
*       result = eventLoop.run();
*    
*       return 0;
*   }
* </code>    
*
*   @note Note that: some others like ItcPubSub and TimerManager also use APIs of EventLoop and they already added
*   appropriate FDs and CallbackFunc on your behalf. This means, for example, your thread in the first place create
*   and run an eventLoop without adding any FdHandler. But later on, your thread involves ItcPubSub and
*   TimerManager and performs some APIs from them, so they helped you add FdHandlers to your EventLoop at that time. */
class EventLoopAPI
{
public:
    /*! @brief Define return/result codes when using EventLoop */
    enum class ReturnCode
    {
        NORMAL,             /*!< No error */
        ALREADY_EXISTS,     /*!< A FD handler already exists */
        NOT_FOUND,          /*!< A FD handler was not found */
        INVALID_ARG,        /*!< The callback was called with invalid arguments */
        NOT_THREAD_LOCAL,   /*!< The EventLoopAPI instance called was not the thread-local instance */
        INTERNAL_FAULT      /*!< An internal error */
    };

    // First prevent end users from copy/move construtors
    EventLoopAPI(const EventLoopAPI&)               = delete;
    EventLoopAPI(EventLoopAPI&&)                    = delete;
    EventLoopAPI& operator=(const EventLoopAPI&)    = delete;
    EventLoopAPI& operator=(EventLoopAPI&&)         = delete;

    /*! @brief  Create a local thread instance of EventLoopAPI for current thread.
    *   @return An address of the local thread instance of EventLoopAPI */
    static EventLoopAPI& getInstance();

    /* Supported FD event type. Used in the event mask */
    static constexpr uint32_t FdEventIn     = 0x001;    /*!< The FD is readable */
    static constexpr uint32_t FdEventOut    = 0x002;    /*!< The FD is writeable */

    /*! @brief Callback function signature used for handling an event on a FD.
    *   @param[in] fd The FD, a positive integer.
    *   @param[in] eventMask A bit-mask used for differentiating a FD readable only or writeable only or both rights. */
    using CallbackFunc = std::function<void(int fd, uint32_t eventMask)>;

    /*! @brief Add an FD handler function to a FD with respective access rights via eventMask.
    *   @param[in] fd The FD, a positive integer.
    *   @param[in] eventMask A bit-mask used for differentiating a FD readable only or writeable only or both rights.
    *   @note Note that: eventMask must be different from 0, otherwise an ReturnCode INVALID_ARG will be returned.
    *   @param[in] callback Function to be invoked when event occurs on a FD.
    *   @return NORMAL if successfully added, otherwise possible ReturnCode will be NOT_FOUND, INVALID_ARG,
    *   NOT_THREAD_LOCAL, INTERNAL_FAULT. */
    virtual ReturnCode addFdHandler(int fd, uint32_t eventMask, const CallbackFunc& callback) = 0;

    /*! @brief Update a FD handler with new eventMask (read/write/both)
    *   @param[in] fd The FD, a positive integer.
    *   @param[in] eventMask A bit-mask used for differentiating a FD readable only or writeable only or both rights.
    *   @note Note that: eventMask must be different from 0, otherwise an ReturnCode INVALID_ARG will be returned.
    *   @return NORMAL if successfully added, otherwise possible ReturnCode will be NOT_FOUND, INVALID_ARG,
    *   NOT_THREAD_LOCAL, INTERNAL_FAULT. */
    virtual ReturnCode updateFdEvents(int fd, uint32_t eventMask) = 0;

    /*! @brief Remove a FD handler on a FD.
    *   @param[in] fd The FD, a positive integer.
    *   @return NORMAL if successfully added, otherwise possible ReturnCode will be NOT_FOUND, NOT_THREAD_LOCAL */
    virtual ReturnCode removeFdHandler(int fd) = 0;

    /*! @brief Run the EventLoop. Wait for events on all added FDs. Will not return until there is no active FDs
    *   or EventLoopAPI::stop() has been called. 
    *   @return NORMAL if successfully no active FDs or EventLoopAPI::stop() has been called,
    *   otherwise possible ReturnCode will be NOT_THREAD_LOCAL, INTERNAL_FAULT */
    virtual ReturnCode run() = 0;

    /*! @brief Run the EventLoop. Wait for events on all added FDs. Will not return until there is no active FDs
    *   or EventLoopAPI::stop() has been called.
    *   @return NORMAL if successfully stopped, otherwise possible ReturnCode will be NOT_THREAD_LOCAL */
    virtual ReturnCode stop()
    {
        return ReturnCode::NORMAL;
    }

/****************************************************-SPECIAL-USE-*****************************************************/

    /*! @brief Callback function signature used for handling a scheduled event. */
    using EventHandlerFunc = std::function<void()>;

    /*! @brief Schedule an asynchronous event for current thread. The input callback will be called before any
    *   FD event handlers are called. Scheduled event will be more prioritized than external events.
    *   @param[in] eventHandler Function to be invoked when event occurs on a FD.
    *   @return NORMAL if successfully added, otherwise possible ReturnCode will be NOT_FOUND, INVALID_ARG,
    *   NOT_THREAD_LOCAL, INTERNAL_FAULT. */
    virtual ReturnCode scheduleEvent(const EventHandlerFunc& eventHandler) = 0;

/****************************************************-SPECIAL-USE-*****************************************************/

protected:
    EventLoopAPI()  = default;
    ~EventLoopAPI() = default;

}; // class EventLoopAPI

} // namespace V1

} // namespace EventLoop

} // namespace CommonAPIs