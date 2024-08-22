#include <sys/eventfd.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <string.h>

#include "activeObjectThread.h"

namespace UtilsFramework::ActiveObject::implementation
{

using namespace UtilsFramework::EventLoop::V1;

ActiveObjectThread::ActiveObjectThread(const std::string& name)
    :   m_name(name),
        m_eventFd(-1)
{
    // Should add a trace point here in the future for debugging
}

ActiveObjectThread::~ActiveObjectThread()
{
    if(m_thread.joinable())
    {
	if(m_thread.get_id() == std::this_thread::get_id())
	{
		/* In case AO termination came from AO Thread itself -> detach AO thread from main thread and stop eventLoop */
		m_thread.detach();
		ActiveObjectThread::stopEventLoop(m_eventFd);
	} else
	{
		/* If AO termination came from main thread -> schedule an event for AO thread to stop its eventLoop */
		scheduleFunction(std::bind(&ActiveObjectThread::stopEventLoop, m_eventFd));
		m_thread.join();
	}
    }

    if(m_eventFd != -1)
    {
	close(m_eventFd);
    }
}

bool ActiveObjectThread::start(bool isFifo, const AOFunc& initFunc)
{
	/* Create an event fd to synchronize with AO Thread.
	*  When main thread schedule an event/task for AO thread, it will notify AO Thread by writing to this fd */
	m_eventFd = eventfd(0, EFD_CLOEXEC | EFD_SEMAPHORE);
	if(m_eventFd == -1)
	{
		return false;
	}

	/* Give the AO Thread this m_eventFd, ask it to monitor on this fd. If any scheduled event has been enqueued, main thread will notify it via this fd. */
	m_thread = std::thread(&ActiveObjectThread::mainFunction, m_name, m_eventFd, std::bind(&ActiveObjectThread::handleFdEvent, this), isFifo, initFunc);

	return true;
}

void ActiveObjectThread::mainFunction(const std::string& name, int eventFd, \
                    const IEventLoop::CallbackFunc& fdHandler, \
                    bool isFifo, const AOFunc& initFunc)
{
	prctl(PR_SET_NAME, name.c_str(), 0, 0, 0);

	IEventLoop& eventLoop = IEventLoop::getThreadLocalInstance();
	if(eventLoop.addFdHandler(eventFd, IEventLoop::FdEventIn, fdHandler) != IEventLoop::ReturnCode::NORMAL)
	{
		return;
	}

	if(isFifo)
	{
		int policy;
		struct sched_param param;
		pthread_getschedparam(pthread_self(), &policy, &param);
		pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
	}

	if(initFunc)
	{
		initFunc();
	}

	eventLoop.run();
}

void ActiveObjectThread::stopEventLoop(int eventFd)
{
	IEventLoop& eventLoop = IEventLoop::getThreadLocalInstance();
	if(eventFd != -1)
	{
		eventLoop.removeFdHandler(eventFd);
	}

	eventLoop.stop();
}

void ActiveObjectThread::scheduleFunction(const AOFunc& func)
{
	/* Enqueue this task to AO Thread task queue */
	enqueueFunction(func);

	/* Notify AO Thread via m_eventFd */
	uint64_t one = 1;
	ssize_t len = ::write(m_eventFd, &one, sizeof(one));
	if(len == -1)
	{
		// Print ERROR
	}
}

void ActiveObjectThread::enqueueFunction(const AOFunc& func)
{
	/* We will lock this mutex and unlock it right when exiting this function.
	*  So if any other main threads that also owns this AO Thread will not be able to enqueue at a same time. 
	*  To avoid race condition or collision */
	std::lock_guard<std::mutex> lock(m_mutex);

	m_funcQueue.push_back(func);
}

ActiveObjectThread::AOFunc ActiveObjectThread::dequeueFunction()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	AOFunc func;
	auto func_it = m_funcQueue.begin();
	if(func_it != m_funcQueue.end())
	{
		func = *func_it;
		m_funcQueue.erase(func_it);
	}

	return func;
}

void ActiveObjectThread::handleFdEvent()
{
	uint64_t sem_count;
	ssize_t len = ::read(m_eventFd, &sem_count, sizeof(sem_count));
	if(len == -1)
	{
		return;
	}

	auto func = dequeueFunction();
	if(func)
	{
		func();
	}
}

} // namespace UtilsFramework::ActiveObject::implementation