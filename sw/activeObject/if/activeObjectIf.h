/*
*        ________________           ________                                                    ______  
* ____  ___  /___(_)__  /_______    ___  __/____________ _______ ___________      _________________  /__
* _  / / /  __/_  /__  /__  ___/    __  /_ __  ___/  __ `/_  __ `__ \  _ \_ | /| / /  __ \_  ___/_  //_/
* / /_/ // /_ _  / _  / _(__  )     _  __/ _  /   / /_/ /_  / / / / /  __/_ |/ |/ // /_/ /  /   _  ,<   
* \__,_/ \__/ /_/  /_/  /____/      /_/    /_/    \__,_/ /_/ /_/ /_/\___/____/|__/ \____//_/    /_/|_|  
*                                                                                                       
*/

#pragma once

#include <memory>
#include <functional>
#include <string>

namespace UtilsFramework
{
namespace ActiveObject 
{
namespace V1 
{
/*! @brief Active Objects (AO): Primarily used for managing internal tasks and encapsulating behavior within an object.
* The focus is on providing a high-level abstraction for asynchronous task execution with its own thread of control.
* Please use this UtilsFramework::ActiveObject::V1::IActiveObject along with UtilsFramework::EventLoop::V1::IEventLoop and
* UtilsFramework::ItcPubSub::V1::ItcPubSubApi.
*
* Example usage:
*
* </code>
*   // some your code ...
*   std::shared_ptr<IActiveObject> ao = IActiveObject::create();
*   if(!ao)
*   {
*         // print some error;
*   }
*
*   // Assume that you already had a function to be passed to AO, YourClass::functionAbc()
*   // And you want to pass an argument such as a bool "flag" to AO as well
*   YourClass *obj = ...
*   auto function = std::bind(&YourClass::functionAbc, obj, flag);
*   ao->executeFunction();
* </code>
*
* Note that: in asynchronous context, the function that is passed to AO must have void return type.
*/

class IActiveObject
{
public:
    /*! @brief Define Linux supported scheduling policies, see man7 pages for more information. Normally, we can simply
    * use Default priority for all AO threads. Only use Fifo priority for exceptional cases that need real-time
    * execution,... */
    enum class SchedulingPolicy
    {
        Default,    // Respectively default scheduling priority, mostly used in all cases.
        Fifo        // Respectively SCHED_FIFO.
    };

    /*! @brief Creates a new Active Object for current calling thread.
    *   @param[in] initFunc A optional initialization function which is done before any other things are handled.
    * For example, you can simply can IActiveObject::create() or utilize lamda expressions to pass initFunc to AO.
    * Usage:
    * 
    * </code>  
    *   IActiveObject::create([var1, var2, obj1]() 
    *   {
    *       // do something here for initialization, for example create AO's mailboxes, doMonitorSomething(),...
    *   });
    * </code>
    * 
    * You can also give lamda access to variables in current context of main thread via Capture clause.
    * Note that: no param in () and no return for lamda 
    *   @param[in] schedPolicy Define Scheduling Policy for the AO. Scheduling::Default is selected by default.
    *   @return Return a shared pointer to the created AO instance. If the creation fails for any reason, the returned
    * shared pointer will be nullptr. You need to double check the return pointer before using. Once use_count
    * of shared pointer becomes zero, the AO thread will automatically terminated after all scheduled functions
    * in the event queue have been carried out. */
    static std::shared_ptr<IActiveObject> create(const std::function<void()>& initFunc = nullptr, \
                                            const SchedulingPolicy& schedPolicy = SchedulingPolicy::Default);

    static std::shared_ptr<IActiveObject> create(const std::string& name, \
                                            const std::function<void()>& initFunc = nullptr, \
                                            const SchedulingPolicy& schedPolicy = SchedulingPolicy::Default);

    virtual void executeFunction(const std::function<void()>& func = nullptr);

    // To avoid user doing copy/move operations
    IActiveObject(const IActiveObject&) = delete;
    IActiveObject(IActiveObject&&) = delete;
    IActiveObject& operator=(const IActiveObject&) = delete;
    IActiveObject& operator=(IActiveObject&&) = delete;

protected:
    // Only backend implementation (behind the scene) can define constructor and destructor.
    IActiveObject() = default;
    ~IActiveObject() = default;

}; // class IActiveObject

} // namespace V1

} // namespace ActiveObject

} // namespace UtilsFramework