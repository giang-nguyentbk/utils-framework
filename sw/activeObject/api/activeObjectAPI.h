#pragma once

#include <memory>
#include <functional>
#include <string>

namespace CommonAPIs
{
namespace ActiveObject 
{
namespace V1 
{
/*! @brief Active Objects (AO): Primarily used for managing internal tasks and encapsulating behavior within an object.
* The focus is on providing a high-level abstraction for asynchronous task execution with its own thread of control.
* Please use this CommonAPIs::ActiveObject::V1::ActiveObjectAPI along with CommonAPIs::EventLoop::V1::EventLoopApi and
* CommonAPIs::ItcPubSub::V1::ItcPubSubApi.
*
* Example usage:
*
* </code>
*   // some your code ...
*   std::shared_ptr<ActiveObjectAPI> ao = ActiveObjectAPI::create();
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

class ActiveObjectAPI
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
    * For example, you can simply can ActiveObjectAPI::create() or utilize lamda expressions to pass initFunc to AO.
    * Usage:
    * 
    * </code>  
    *   ActiveObjectAPI::create([var1, var2, obj1]() 
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
    static std::shared_ptr<ActiveObjectAPI> create(const std::function<void()>& initFunc = nullptr, \
                                            const SchedulingPolicy& schedPolicy = SchedulingPolicy::Default);

    /*! @brief Creates a new Active Object for current calling thread. Same as above except for giving the name
    * of master thread for AO thread or AO's itc mailbox naming which is easier for debugging.
    *   @param[in] name The name of master thread given to AO thread for naming, for example, AO_module1 where
    * "module1" is the name of master thread (thread naming by prctrl syscall).
    *   @param[in] initFunc A optional initialization function which is done before any other things are handled.
    *   @param[in] schedPolicy Define Scheduling Policy for the AO. Scheduling::Default is selected by default.
    *   @return Return a shared pointer to the created AO instance. If the creation fails for any reason, the returned
    * shared pointer will be nullptr. You need to double check the return pointer before using. Once use_count
    * of shared pointer becomes zero, the AO thread will automatically terminated after all scheduled functions
    * in the event queue have been carried out. */
    static std::shared_ptr<ActiveObjectAPI> create(const std::string& name, \
                                            const std::function<void()>& initFunc = nullptr, \
                                            const SchedulingPolicy& schedPolicy = SchedulingPolicy::Default);

    /*! @brief Executes asynchronously a function which is given by master thread in the context of AO thread.
    *   @param[in] func The function to be executed.
    */
    virtual void executeFunction(const std::function<void()>& func = nullptr);

    // To avoid user doing copy/move operations
    ActiveObjectAPI(const ActiveObjectAPI&) = delete;
    ActiveObjectAPI(ActiveObjectAPI&&) = delete;
    ActiveObjectAPI& operator=(const ActiveObjectAPI&) = delete;
    ActiveObjectAPI& operator=(ActiveObjectAPI&&) = delete;

protected:
    // Only backend implementation (behind the scene) can define constructor and destructor.
    ActiveObjectAPI() = default;
    ~ActiveObjectAPI() = default;

}; // class ActiveObjectAPI

} // namespace V1

} // namespace ActiveObject

} // namespace CommonAPIs