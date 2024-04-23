#include <iostream>
#include <sys/eventfd.h>
#include "eventLoopIf.h"

using namespace UtilsFramework::EventLoop::V1;

int main()
{
	IEventLoop& eventLoop = IEventLoop::getInstance();

	int fd = eventfd(0, EFD_CLOEXEC);
	uint32_t eventMask = IEventLoop::FdEventIn;
	IEventLoop::CallbackFunc callback = [](int _fd, uint32_t _eventMask) -> void
	{
		std::cout << "\tDEBUG: Call back for " << _fd << " is called with eventMask = " << +_eventMask << std::endl;
	};


	IEventLoop::ReturnCode rc = eventLoop.addFdHandler(fd, eventMask, callback);
	if(rc != IEventLoop::ReturnCode::NORMAL)
	{
		std::cout << "[FAILED] - IEventLoop.addFdHandler()" << std::endl;
		return -1;
	} else
	{
		std::cout << "[PASSED] - IEventLoop.addFdHandler()" << std::endl;
	}


	// eventMask |= IEventLoop::FdEventOut; // Never do this otherwise it will cause deadlock
	rc = eventLoop.updateFdEvents(fd, eventMask);
	if(rc != IEventLoop::ReturnCode::NORMAL)
	{
		std::cout << "[FAILED] - IEventLoop.updateFdEvents()" << std::endl;
		return -1;
	} else
	{
		std::cout << "[PASSED] - IEventLoop.updateFdEvents()" << std::endl;
	}


	rc = eventLoop.run();
	if(rc != IEventLoop::ReturnCode::NORMAL)
	{
		std::cout << "[FAILED] - IEventLoop.run()" << std::endl;
		return -1;
	} else
	{
		std::cout << "[PASSED] - IEventLoop.run()" << std::endl;
	}


	IEventLoop::EventHandlerFunc evtFunc = []() -> void
	{
		std::cout << "\tDEBUG: Executing EventHandlerFunc()!" << std::endl;
	};
	rc = eventLoop.scheduleEvent(evtFunc);
	if(rc != IEventLoop::ReturnCode::NORMAL)
	{
		std::cout << "[FAILED] - IEventLoop.scheduleEvent()" << std::endl;
		return -1;
	} else
	{
		std::cout << "[PASSED] - IEventLoop.scheduleEvent()" << std::endl;
	}


	rc = eventLoop.removeFdHandler(fd);
	if(rc != IEventLoop::ReturnCode::NORMAL)
	{
		std::cout << "[FAILED] - IEventLoop.removeFdHandler()" << std::endl;
		return -1;
	} else
	{
		std::cout << "[PASSED] - IEventLoop.removeFdHandler()" << std::endl;
	}

	rc = eventLoop.stop();
	if(rc != IEventLoop::ReturnCode::NORMAL)
	{
		std::cout << "[FAILED] - IEventLoop.stop()" << std::endl;
		return -1;
	} else
	{
		std::cout << "[PASSED] - IEventLoop.stop()" << std::endl;
	}

	return 0;
}
