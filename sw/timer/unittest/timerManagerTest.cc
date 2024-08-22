#include <iostream>
#include <type_traits>
#include <chrono>
#include <unistd.h>
#include <csignal>
#include <cstdlib>

#include <eventLoopIf.h>
#include <threadLocalIf.h>

#include <timerSubscriberIf.h>
#include <timerManagerIf.h>

using namespace UtilsFramework::Timer::V1;
using namespace UtilsFramework::EventLoop::V1;
using namespace UtilsFramework::ThreadLocal::V1;

void my_handler(int s){
	(void)s;
	IEventLoop::getThreadLocalInstance().stop();
}

enum class MySignalE
{
	SETUP_REQ = 1,
	ACTIVATE_REQ,
	DEACTIVATE_REQ,
	RELEASE_REQ
};

template<typename E>
constexpr auto toUnderlyingType(E e) -> typename std::underlying_type<E>::type 
{
   return static_cast<typename std::underlying_type<E>::type>(e);
}

class MyTimer : public ITimerSubscriber
{
	void handleTimerExpired(uint32_t userId)
	{
		switch (userId)
		{
		case toUnderlyingType(MySignalE::SETUP_REQ):
			std::cout << "Starting SETUP_REQ!" << std::endl;
			break;
		case toUnderlyingType(MySignalE::ACTIVATE_REQ):
			std::cout << "Starting ACTIVATE_REQ!" << std::endl;
			break;
		case toUnderlyingType(MySignalE::DEACTIVATE_REQ):
			std::cout << "Starting DEACTIVATE_REQ!" << std::endl;
			break;
		case toUnderlyingType(MySignalE::RELEASE_REQ):
			std::cout << "Starting RELEASE_REQ!" << std::endl;
			break;
		default:
			std::cout << "Unknown userId " << userId << "!" << std::endl;
			break;
		}		
	}
};

int main()
{
	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = my_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);

	MyTimer m_signalTimer;

	std::cout << "Start delay timer for SETUP_REQ!\n";
	ITimerManager::getThreadLocalInstance().startTimer(std::chrono::milliseconds(500), &m_signalTimer, toUnderlyingType(MySignalE::SETUP_REQ));

	std::cout << "Start delay timer for ACTIVATE_REQ!\n";
	ITimerManager::getThreadLocalInstance().startPeriodicalTimer(std::chrono::milliseconds(1000), &m_signalTimer, toUnderlyingType(MySignalE::ACTIVATE_REQ));

	std::cout << "Start delay timer for DEACTIVATE_REQ!\n";
	ITimerManager::getThreadLocalInstance().startTimer(std::chrono::milliseconds(2000), &m_signalTimer, toUnderlyingType(MySignalE::DEACTIVATE_REQ));

	std::cout << "Start delay timer for RELEASE_REQ!\n";
	ITimerManager::getThreadLocalInstance().startTimer(std::chrono::milliseconds(500), &m_signalTimer, toUnderlyingType(MySignalE::RELEASE_REQ));

	IEventLoop::getThreadLocalInstance().run();

	ITimerManager::getThreadLocalInstance().cancelTimer(&m_signalTimer, toUnderlyingType(MySignalE::SETUP_REQ));
	ITimerManager::getThreadLocalInstance().cancelTimer(&m_signalTimer, toUnderlyingType(MySignalE::ACTIVATE_REQ));
	ITimerManager::getThreadLocalInstance().cancelTimer(&m_signalTimer, toUnderlyingType(MySignalE::DEACTIVATE_REQ));
	ITimerManager::getThreadLocalInstance().cancelTimer(&m_signalTimer, toUnderlyingType(MySignalE::RELEASE_REQ));

	return 0;
}