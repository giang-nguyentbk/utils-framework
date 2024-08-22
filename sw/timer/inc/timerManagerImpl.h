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
#include <memory>
#include <map>
#include <thread>
#include <functional>
#include <chrono>

#include "timerManagerIf.h"
#include "timerManagerSyscallWrapper.h"
#include "timerSubscriberIf.h"

namespace UtilsFramework
{
namespace Timer
{
namespace V1
{

class TimerManagerImpl : public ITimerManager
{
public:
	static TimerManagerImpl& getThreadLocalInstance();
	static void reset();

	ReturnCode startTimer(const std::chrono::milliseconds& timeout, ITimerSubscriber *subscriber, uint32_t userId = 0) override;
	ReturnCode startPeriodicalTimer(const std::chrono::milliseconds& interval, ITimerSubscriber *subscriber, uint32_t userId = 0) override;
	ReturnCode cancelTimer(ITimerSubscriber *subscriber, uint32_t userId = 0) override;

	TimerManagerImpl(const TimerManagerImpl&) = delete;
	TimerManagerImpl(TimerManagerImpl&&) = delete;
	TimerManagerImpl& operator=(const TimerManagerImpl&) = delete;
	TimerManagerImpl& operator=(TimerManagerImpl&&) = delete;

	TimerManagerImpl();
	virtual ~TimerManagerImpl();
private:
	struct TimerObject
	{
		ITimerSubscriber *subscriber;
		uint32_t userId;
		bool isPeriodical;
		std::chrono::milliseconds periodicalInterval;
	};

	ReturnCode launchNewTimer(const TimerObject& tmoObj, const std::chrono::milliseconds& timeout);
	int createTimerFd();
	bool setTimerFd(int fd);
	void repossessTimerFd(int fd);
	void onTimerExpired();
	void handleTimeout(ITimerSubscriber* subscriber, uint32_t userId);

private:
	std::thread::id m_threadId;
	int m_timerFd;
	std::shared_ptr<TimerManagerSyscallWrapper> m_syscallWrapper;
	/* 
	+ A multimap contains all timerObjects of this thread, manage them via only one timerfd.
	+ All timerObjects in this multimap are sorted by expiration time.
	+ Some timerObjects have exactly same timeout, so using multimap instead of map.
	*/
	std::multimap<std::chrono::time_point<std::chrono::steady_clock>, TimerObject> m_activeTimers;

}; // class TimerManagerImpl

} // namespace V1

} // namespace ActiveObject

} // namespace UtilsFramework