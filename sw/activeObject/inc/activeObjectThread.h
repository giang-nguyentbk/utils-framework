/*
*        ________________           ________                                                    ______  
* ____  ___  /___(_)__  /_______    ___  __/____________ _______ ___________      _________________  /__
* _  / / /  __/_  /__  /__  ___/    __  /_ __  ___/  __ `/_  __ `__ \  _ \_ | /| / /  __ \_  ___/_  //_/
* / /_/ // /_ _  / _  / _(__  )     _  __/ _  /   / /_/ /_  / / / / /  __/_ |/ |/ // /_/ /  /   _  ,<   
* \__,_/ \__/ /_/  /_/  /____/      /_/    /_/    \__,_/ /_/ /_/ /_/\___/____/|__/ \____//_/    /_/|_|  
*                                                                                                       
*/

#pragma once

#include <string>
#include <thread>
#include <mutex>
#include <functional>
#include <vector>

#include "eventLoopIf.h"

namespace UtilsFramework::ActiveObject::implementation
{

using namespace UtilsFramework::EventLoop::V1;

class ActiveObjectThread
{
public:
    // By using explicit modifier, only this type of constructor is accepted
    explicit ActiveObjectThread(const std::string& name);
    ~ActiveObjectThread();

    // First avoid copy/move constructors
    ActiveObjectThread(const ActiveObjectThread&)               = delete;
    ActiveObjectThread(ActiveObjectThread&&)                    = delete;
    ActiveObjectThread& operator=(const ActiveObjectThread&)    = delete;
    ActiveObjectThread& operator=(ActiveObjectThread&&)         = delete;

    using AOFunc = std::function<void()>;

    bool start(bool isFifo, const AOFunc& initFunc);
    void scheduleFunction(const AOFunc& func);

private:
    static void mainFunction(const std::string& name, int eventFd, \
                    const IEventLoop::CallbackFunc& fdHandler, \
                    bool isFifo, const AOFunc& initFunc);

    static void stopEventLoop(int eventFd);
    void enqueueFunction(const AOFunc& func);
    AOFunc dequeueFunction();
    void handleFdEvent();

    std::string m_name;
    std::thread m_thread;
    int m_eventFd;
    std::mutex m_mutex;
    std::vector<AOFunc> m_funcQueue;
};

} // namespace UtilsFramework::ActiveObject::implementation