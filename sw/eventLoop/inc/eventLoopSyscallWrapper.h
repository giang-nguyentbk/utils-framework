/*
*        ________________           ________                                                    ______  
* ____  ___  /___(_)__  /_______    ___  __/____________ _______ ___________      _________________  /__
* _  / / /  __/_  /__  /__  ___/    __  /_ __  ___/  __ `/_  __ `__ \  _ \_ | /| / /  __ \_  ___/_  //_/
* / /_/ // /_ _  / _  / _(__  )     _  __/ _  /   / /_/ /_  / / / / /  __/_ |/ |/ // /_/ /  /   _  ,<   
* \__,_/ \__/ /_/  /_/  /____/      /_/    /_/    \__,_/ /_/ /_/ /_/\___/____/|__/ \____//_/    /_/|_|  
*                                                                                                       
*/

#pragma once

#include <sys/epoll.h>
#include <iostream>

class EventLoopSyscallWrapper
{
public:
	EventLoopSyscallWrapper() = default;
	virtual ~EventLoopSyscallWrapper() = default;

	// First prevent copy/move construtors
	EventLoopSyscallWrapper(const EventLoopSyscallWrapper&)               = delete;
	EventLoopSyscallWrapper(EventLoopSyscallWrapper&&)                    = delete;
	EventLoopSyscallWrapper& operator=(const EventLoopSyscallWrapper&)    = delete;
	EventLoopSyscallWrapper& operator=(EventLoopSyscallWrapper&&)         = delete;

	virtual int epoll_create1(int flags)
	{
		return ::epoll_create1(flags);
	}

	virtual int epoll_ctl(int epfd, int op, int fd, struct epoll_event* event)
	{
		return ::epoll_ctl(epfd, op, fd, event);
	}

	virtual int epoll_wait(int epfd, struct epoll_event* event, int maxEvents, int timeout)
	{
		return ::epoll_wait(epfd, event, maxEvents, timeout);
	}

}; // class EventLoopSyscallWrapper