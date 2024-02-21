#pragma once

#include <string>
#include <thread>
#include <mutex>
#include <functional>
#include <vector>

#include "eventLoopAPI.h"

namespace CommonAPIs::ActiveObject::implementation
{

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

    using AOFunction = std::function<void()>;

    bool start(bool isFifo, const AOFunction& initFunc);
    void scheduleFunction(const AOFunction& func);

private:
    static void mainFunction(const std::string& name, int eventFd, \
                    const CommonAPIs::EventLoop::V1::EventLoopAPI::CallbackFunc& fdHandler, \
                    bool isFifo, const AOFunction& initFunc);

    static void stopEventLoop(int eventFd);
    void enqueueFunction(const AOFunction& func);
    AOFunction dequeueFunction();
    void handleFdEvent();

    std::string m_name;
    std::thread m_thread;
    int m_eventFd;
    std::mutex m_mutex;
    std::vector<AOFunction> m_funcQueue;
};

} // namespace CommonAPIs::ActiveObject::implementation