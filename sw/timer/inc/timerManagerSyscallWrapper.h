/*
*        ________________           ________                                                    ______  
* ____  ___  /___(_)__  /_______    ___  __/____________ _______ ___________      _________________  /__
* _  / / /  __/_  /__  /__  ___/    __  /_ __  ___/  __ `/_  __ `__ \  _ \_ | /| / /  __ \_  ___/_  //_/
* / /_/ // /_ _  / _  / _(__  )     _  __/ _  /   / /_/ /_  / / / / /  __/_ |/ |/ // /_/ /  /   _  ,<   
* \__,_/ \__/ /_/  /_/  /____/      /_/    /_/    \__,_/ /_/ /_/ /_/\___/____/|__/ \____//_/    /_/|_|  
*                                                                                                       
*/

#pragma once

#include <sys/timerfd.h>

namespace UtilsFramework
{
namespace Timer
{
namespace V1
{

class TimerManagerSyscallWrapper
{
public:
	TimerManagerSyscallWrapper() = default;
	virtual ~TimerManagerSyscallWrapper() = default;

	TimerManagerSyscallWrapper(const TimerManagerSyscallWrapper&) = delete;
	TimerManagerSyscallWrapper(TimerManagerSyscallWrapper&&) = delete;
	TimerManagerSyscallWrapper& operator=(const TimerManagerSyscallWrapper&) = delete;
	TimerManagerSyscallWrapper& operator=(TimerManagerSyscallWrapper&&) = delete;

	virtual int timerFdCreate(int clockType, int flags)
	{
		return ::timerfd_create(clockType, flags);
	}

	virtual int timerFdSetTime(int fd, int flags, const struct itimerspec *new_value, struct itimerspec *old_value)
	{
		return ::timerfd_settime(fd, flags, new_value, old_value);
	}

}; // class TimerManagerSyscallWrapper

} // namespace V1

} // namespace ActiveObject

} // namespace UtilsFramework