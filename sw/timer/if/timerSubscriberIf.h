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

/* Interface class used by user's classes to define what should be performed when their timer expired 
   Users need to create their own timerObject classes that inherit this interface, give instances of
   that timerObject classes to ITimerManager then. */

namespace UtilsFramework
{
namespace Timer
{
namespace V1
{

class ITimerSubscriber
{
public:
	virtual void handleTimerExpired(uint32_t userId) = 0;

	virtual ~ITimerSubscriber() = default;

protected:
	ITimerSubscriber() = default;

	ITimerSubscriber(const ITimerSubscriber&) = default;
	ITimerSubscriber(ITimerSubscriber&&) = default;
	ITimerSubscriber& operator=(const ITimerSubscriber&) = default;
	ITimerSubscriber& operator=(ITimerSubscriber&&) = default;

}; // class ITimerSubscriber

} // namespace V1

} // namespace ActiveObject

} // namespace UtilsFramework