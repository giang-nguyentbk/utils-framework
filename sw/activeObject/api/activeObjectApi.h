#pragma once

#include <memory>
#include <functional>
#include <string>

namespace CommonFwks
{
namespace ActiveObject 
{
namespace V1 
{
/*! @brief Active Objects (AO): Primarily used for managing internal tasks and encapsulating behavior within an object.
* The focus is on providing a high-level abstraction for asynchronous task execution with its own thread of control.
* Please use this CommonFwks::ActiveObject::V1::ActiveObjectApi along with CommonFwks::EventLoop::V1::EventLoopApi and
* CommonFwks::ItcPubSub::V1::ItcPubSubApi.
*
* Example usage:
*
* </code>
*   // some your code ...
*   std::shared_ptr<ActiveObjectApi> ao = ActiveObjectApi::create();
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

class ActiveObjectApi
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
    *   @param[in] initFunc A initialization function which is done before any other things are handled. This param is
    * optional. For example, you can simply can ActiveObjectApi::create() or utilize lamda expressions to pass initFunc
    * to AO like:
    * 
    * </code>  
    *   ActiveObjectApi::create([var1, var2, obj1]() 
    *   {
    *       // do something here for initialization, for example create AO's mailboxes, doMonitorSomething(),...
    *   });
    * </code>
    * 
    * You can also pass some variables in current context of main thread to lamda via Capture clause.
    * Note that: no param in () and no return for lamda 
    *   @param[in] schedPolicy 
    * */
    static std::shared_ptr<ActiveObjectApi> create(const std::function<void()>& initFunc = nullptr, \
                                            const SchedulingPolicy& schedPolicy = SchedulingPolicy::Default);


    static std::shared_ptr<ActiveObjectApi> create(const std::string& name, \
                                            const std::function<void()>& initFunc = nullptr, \
                                            const SchedulingPolicy& schedPolicy = SchedulingPolicy::Default);

    virtual void executeFunction(const std::function<void()>& func = nullptr);

    // To avoid user doing copy/move operations
    ActiveObjectApi(const ActiveObjectApi&) = delete;
    ActiveObjectApi(ActiveObjectApi&&) = delete;
    ActiveObjectApi& operator=(const ActiveObjectApi&) = delete;
    ActiveObjectApi& operator=(ActiveObjectApi&&) = delete;

protected:
    // Only backend implementation (behind the scene) can define constructor and destructor.
    ActiveObjectApi() = default;
    ~ActiveObjectApi() = default;

}; // class ActiveObjectIf

} // namespace V1

} // namespace ActiveObject

} // namespace CommonFwks