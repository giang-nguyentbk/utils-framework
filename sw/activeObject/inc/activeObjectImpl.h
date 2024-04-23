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

#include "activeObjectIf.h"
#include "activeObjectThread.h"

namespace UtilsFramework
{
namespace ActiveObject 
{
namespace V1 
{
using namespace UtilsFramework::ActiveObject::implementation;

class ActiveObjectImpl : public IActiveObject
{
public:
    // Default constructor and default virtual destructor
    ActiveObjectImpl() = default;
    ~ActiveObjectImpl() = default;

    // First avoid copy/move constructors
    ActiveObjectImpl(const ActiveObjectImpl&)               = delete;
    ActiveObjectImpl(ActiveObjectImpl&&)                    = delete;
    ActiveObjectImpl& operator=(const ActiveObjectImpl&)    = delete;
    ActiveObjectImpl& operator=(ActiveObjectImpl&&)         = delete;

    bool createThread(const std::string& name, const SchedulingPolicy& schedPolicy, const std::function<void()>& initFunc);

    void executeFunction(const std::function<void()>& func) override;

private:
    std::shared_ptr<ActiveObjectThread> m_aoThread;
};

} // namespace V1

} // namespace ActiveObject

} // namespace UtilsFramework