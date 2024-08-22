#include <unistd.h>
#include <cstring>

#include <stringUtils.h>
#include <traceIf.h>

#include "timerManagerImpl.h"
#include "timerManagerSyscallWrapper.h"
#include "timerSubscriberIf.h"
#include "eventLoopIf.h"
#include "threadLocalIf.h"
#include "util_framework_tpt_provider.h"

using namespace UtilsFramework::EventLoop::V1;
using namespace UtilsFramework::ThreadLocal::V1;
using namespace CommonUtils::V1::StringUtils;

namespace UtilsFramework
{
namespace Timer
{
namespace V1
{

ITimerManager& ITimerManager::getThreadLocalInstance()
{
	return TimerManagerImpl::getThreadLocalInstance();
}

TimerManagerImpl& TimerManagerImpl::getThreadLocalInstance()
{
	return IThreadLocal<TimerManagerImpl>::get();
}

void TimerManagerImpl::reset()
{
	IThreadLocal<TimerManagerImpl>::reset();
}

TimerManagerImpl::TimerManagerImpl()
	: m_threadId(std::this_thread::get_id()),
	  m_timerFd(-1),
	  m_syscallWrapper(std::make_shared<TimerManagerSyscallWrapper>())
{
}

TimerManagerImpl::~TimerManagerImpl()
{
	if(m_timerFd != -1)
	{
		repossessTimerFd(m_timerFd);
		close(m_timerFd);
		(void)IEventLoop::getThreadLocalInstance().removeFdHandler(m_timerFd);
		TPT_TRACE(TRACE_INFO, SSTR("TimerManagerImpl detructs successfully!"));
	}
}

ITimerManager::ReturnCode TimerManagerImpl::startTimer(const std::chrono::milliseconds& timeout, ITimerSubscriber *subscriber, uint32_t userId)
{
	TimerObject tmoObj;
	tmoObj.userId = userId;
	tmoObj.subscriber = subscriber;
	tmoObj.isPeriodical = false;

	TPT_TRACE(TRACE_INFO, SSTR("Starting a timer, timeout = ", timeout.count(), "ms, userId = ", userId));
	return launchNewTimer(tmoObj, timeout);
}

ITimerManager::ReturnCode TimerManagerImpl::startPeriodicalTimer(const std::chrono::milliseconds& interval, ITimerSubscriber *subscriber, uint32_t userId)
{
	TimerObject tmoObj;
	tmoObj.userId = userId;
	tmoObj.subscriber = subscriber;
	tmoObj.isPeriodical = false;
	tmoObj.periodicalInterval = interval;

	TPT_TRACE(TRACE_INFO, SSTR("Starting a periodical timer, interval = ", interval.count(), "ms, userId = ", userId));
	return launchNewTimer(tmoObj, interval);
}

ITimerManager::ReturnCode TimerManagerImpl::cancelTimer(ITimerSubscriber *subscriber, uint32_t userId)
{
	if(std::this_thread::get_id() != m_threadId)
	{
		return ITimerManager::ReturnCode::NOT_THREAD_LOCAL;
	}

	for(auto iter = m_activeTimers.begin(); iter != m_activeTimers.end(); ++iter)
	{
		if(iter->second.subscriber == subscriber && iter->second.userId == userId)
		{
			// Found one timer in the map, erase it
			TPT_TRACE(TRACE_INFO, SSTR("Cancelling a timer, userId = ", userId));
			auto next = m_activeTimers.erase(iter);
			if(next == m_activeTimers.begin())
			{
				(void)setTimerFd(m_timerFd);
			}

			return ITimerManager::ReturnCode::NORMAL;
		}
	}

	TPT_TRACE(TRACE_ABN, SSTR("Failed to cancel a timer (NOT_FOUND), userId = ", userId));
	return ITimerManager::ReturnCode::NOT_FOUND;
}

ITimerManager::ReturnCode TimerManagerImpl::launchNewTimer(const TimerObject& tmoObj, const std::chrono::milliseconds& timeout)
{
	if(std::this_thread::get_id() != m_threadId)
	{
		return ITimerManager::ReturnCode::NOT_THREAD_LOCAL;
	}

	for(auto iter = m_activeTimers.begin(); iter != m_activeTimers.end(); ++iter)
	{
		if(iter->second.subscriber == tmoObj.subscriber && iter->second.userId == tmoObj.userId)
		{
			TPT_TRACE(TRACE_ABN, SSTR("Launching a new timer failed (ALREADY_EXISTS), timeout = ", timeout.count(), "ms, userId = ", tmoObj.userId));
			return ITimerManager::ReturnCode::ALREADY_EXISTS;
		}
	}

	if(m_timerFd == -1)
	{
		m_timerFd = createTimerFd();
		if(m_timerFd == -1)
		{
			TPT_TRACE(TRACE_ERROR, SSTR("Launching a new timer failed, could not create timer fd!"));
			return ITimerManager::ReturnCode::INTERNAL_FAULT;
		}
	}

	std::chrono::time_point<std::chrono::steady_clock> expiredDate = std::chrono::steady_clock::now() + timeout;

	auto insertedIter = m_activeTimers.emplace(expiredDate, tmoObj);
	if(insertedIter == m_activeTimers.begin())
	{
		if(!setTimerFd(m_timerFd))
		{
			m_activeTimers.erase(expiredDate);
			TPT_TRACE(TRACE_ERROR, SSTR("Launching a new timer failed, could not pre-start the first timer!"));
			return ITimerManager::ReturnCode::INTERNAL_FAULT;
		}
	}

	TPT_TRACE(TRACE_INFO, SSTR("Launching a new timer successfully, timeout = ", timeout.count(), "ms, userId = ", tmoObj.userId));
	return ITimerManager::ReturnCode::NORMAL;
}

int TimerManagerImpl::createTimerFd()
{
	int fd = m_syscallWrapper->timerFdCreate(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
	if(fd != -1)
	{
		repossessTimerFd(fd);

		auto handler = std::bind(&TimerManagerImpl::onTimerExpired, this);
		if(IEventLoop::getThreadLocalInstance().addFdHandler(fd, IEventLoop::FdEventIn, handler) != IEventLoop::ReturnCode::NORMAL)
		{
			close(fd);
			fd = -1;
		}
	}
	else
	{
		TPT_TRACE(TRACE_ERROR, SSTR("Failed to create a new timer FD!"));
	}

	return fd;
}

bool TimerManagerImpl::setTimerFd(int fd)
{
	auto iter = m_activeTimers.begin();
	if(iter != m_activeTimers.end())
	{
		auto expiredDate = iter->first;
		auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(expiredDate);
		auto nanoSeconds = std::chrono::time_point_cast<std::chrono::nanoseconds>(expiredDate) - std::chrono::time_point_cast<std::chrono::nanoseconds>(seconds);

		struct itimerspec its;
		std::memset(&its, 0, sizeof(struct itimerspec));
		its.it_value.tv_sec = seconds.time_since_epoch().count();
		its.it_value.tv_nsec = nanoSeconds.count();

		if(m_syscallWrapper->timerFdSetTime(fd, TFD_TIMER_ABSTIME, &its, nullptr) == -1)
		{
			TPT_TRACE(TRACE_ERROR, SSTR("Failed to set time for timer FD = ", fd, "!"));
			return false;
		}
	}
	else
	{
		TPT_TRACE(TRACE_INFO, SSTR("No timers available in TimerManagerImpl!"));
		repossessTimerFd(fd);
	}

	return true;
}

void TimerManagerImpl::repossessTimerFd(int fd)
{
	struct itimerspec its;
	std::memset(&its, 0, sizeof(struct itimerspec));
	(void)m_syscallWrapper->timerFdSetTime(fd, TFD_TIMER_ABSTIME, &its, nullptr);
}

void TimerManagerImpl::onTimerExpired()
{
	uint64_t trigger; // Not used actually, just used to trigger read/write synchronization
	ssize_t ret = read(m_timerFd, &trigger, sizeof(uint64_t));
	if(ret == -1)
	{
		/* If errno is EAGAIN, that's ok meaning that no timer actually has expired,
		Maybe an already expired timer has been cancelled by another FD event in the same epoll event batch. */
		if(errno != EAGAIN)
		{
			TPT_TRACE(TRACE_ERROR, SSTR("Failed to read(), errno = ", errno, "!"));
		}
		return;
	}

	auto iter = m_activeTimers.begin();
	if(iter != m_activeTimers.end())
	{
		ITimerSubscriber* subscriber = iter->second.subscriber;
		uint32_t userId = iter->second.userId;

		if(iter->second.isPeriodical)
		{
			TimerObject tmoObj = iter->second;
			std::chrono::time_point<std::chrono::steady_clock> newExpiredTime = std::chrono::steady_clock::now() + tmoObj.periodicalInterval;
			m_activeTimers.erase(iter);
			m_activeTimers.emplace(newExpiredTime, tmoObj);
		}
		else
		{
			m_activeTimers.erase(iter);
		}

		(void)setTimerFd(m_timerFd);
		handleTimeout(subscriber, userId);
	}
	else
	{
		TPT_TRACE(TRACE_ERROR, SSTR("Timer FD has been triggered but no expired timer found!"));
	}
}

void TimerManagerImpl::handleTimeout(ITimerSubscriber* subscriber, uint32_t userId)
{
	subscriber->handleTimerExpired(userId);
}

} // namespace V1

} // namespace ActiveObject

} // namespace UtilsFramework
