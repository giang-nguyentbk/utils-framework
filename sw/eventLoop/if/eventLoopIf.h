/*
*        ________________           ________                                                    ______  
* ____  ___  /___(_)__  /_______    ___  __/____________ _______ ___________      _________________  /__
* _  / / /  __/_  /__  /__  ___/    __  /_ __  ___/  __ `/_  __ `__ \  _ \_ | /| / /  __ \_  ___/_  //_/
* / /_/ // /_ _  / _  / _(__  )     _  __/ _  /   / /_/ /_  / / / / /  __/_ |/ |/ // /_/ /  /   _  ,<   
* \__,_/ \__/ /_/  /_/  /____/      /_/    /_/    \__,_/ /_/ /_/ /_/\___/____/|__/ \____//_/    /_/|_|  
*                                                                                                       
*/

#pragma once

#include <cstdint>
#include <functional>

namespace UtilsFramework
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
*   using namespace UtilsFramework::EventLoop::V1;
*
*   int main()
*   {
*       ... 
*   
*       IEventLoop::ReturnCode result;
*       IEventLoop& eventLoop = IEventLoop::getInstance();
*   
*       // Add some FDs to your event loop, similarly to the way you add some phone numbers
*       // and what to do when someone calls to those phone numbers.
*       result = eventLoop.addFdHandler(yourFd, IEventLoop::FdEventIn, yourCallbackFunc);
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
class IEventLoop
{
public:
    /*! @brief Define return/result codes when using EventLoop */
    enum class ReturnCode
    {
        NORMAL,             /*!< No error */
        ALREADY_EXISTS,     /*!< A FD handler already exists */
        NOT_FOUND,          /*!< A FD handler was not found */
        INVALID_ARG,        /*!< The callback was called with invalid arguments */
        NOT_THREAD_LOCAL,   /*!< The IEventLoop instance called was not the thread-local instance */
        INTERNAL_FAULT      /*!< An internal error */
    };

    // First prevent end users from copy/move construtors
    IEventLoop(const IEventLoop&)               = delete; // Copy constructor
    IEventLoop(IEventLoop&&)                    = delete; // Move constructor
    IEventLoop& operator=(const IEventLoop&)    = delete; // Copy assignment constructor
    IEventLoop& operator=(IEventLoop&&)         = delete; // Move assignment constructor

    static IEventLoop& getInstance();

    /* Supported FD event type. Used in the event mask */
    static constexpr uint32_t FdEventIn     = 0x001;    /*!< The FD is readable */
    static constexpr uint32_t FdEventOut    = 0x002;    /*!< The FD is writeable */

    using CallbackFunc = std::function<void(int fd, uint32_t eventMask)>;
    virtual ReturnCode addFdHandler(int fd, uint32_t eventMask, const CallbackFunc& callback) = 0;
    virtual ReturnCode updateFdEvents(int fd, uint32_t eventMask) = 0;
    virtual ReturnCode removeFdHandler(int fd) = 0;
    virtual ReturnCode run() = 0;
    virtual ReturnCode stop()
    {
        return ReturnCode::NORMAL;
    }

/****************************************************-SPECIAL-USE-*****************************************************/

    /*! @brief Callback function signature used for handling a own scheduled events. */
    using EventHandlerFunc = std::function<void()>;
    virtual ReturnCode scheduleEvent(const EventHandlerFunc& eventHandler) = 0;

/****************************************************-SPECIAL-USE-*****************************************************/

protected:
    IEventLoop()  = default;
    ~IEventLoop() = default;

}; // class IEventLoop

} // namespace V1

} // namespace EventLoop

} // namespace UtilsFramework