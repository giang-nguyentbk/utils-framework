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
#include <functional>
#include <chrono>

#include "timerSubscriberIf.h"


namespace UtilsFramework
{
namespace Timer
{
namespace V1
{

class ITimerManager
{
public:
	enum class ReturnCode
	{
		NORMAL,             /*!< No error */
		ALREADY_EXISTS,     /*!< A FD handler already exists */
		NOT_FOUND,          /*!< A FD handler was not found */
		INVALID_ARG,        /*!< The callback was called with invalid arguments */
		NOT_THREAD_LOCAL,   /*!< The IEventLoop instance called was not the thread-local instance */
		INTERNAL_FAULT      /*!< An internal error */
	};

	static ITimerManager& getThreadLocalInstance();

	virtual ReturnCode startTimer(const std::chrono::milliseconds& timeout, ITimerSubscriber* subscriber, uint32_t userId = 0) = 0;
	virtual ReturnCode startPeriodicalTimer(const std::chrono::milliseconds& interval, ITimerSubscriber* subscriber, uint32_t userId = 0) = 0;
	virtual ReturnCode cancelTimer(ITimerSubscriber* subscriber, uint32_t userId = 0) = 0;

	ITimerManager(const ITimerManager&) = delete;
	ITimerManager(ITimerManager&&) = delete;
	ITimerManager& operator=(const ITimerManager&) = delete;
	ITimerManager& operator=(ITimerManager&&) = delete;

protected:
	ITimerManager() = default;
	virtual ~ITimerManager() = default;

}; // class ITimerManager

} // namespace V1

} // namespace ActiveObject

} // namespace UtilsFramework