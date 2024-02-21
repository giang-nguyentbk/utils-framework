#pragma once

#include <memory>

#include "activeObjectAPI.h"
#include "activeObjectThread.h"

namespace CommonAPIs
{
namespace ActiveObject 
{
namespace V1 
{
using namespace CommonAPIs::ActiveObject::implementation;

class ActiveObjectImpl : public ActiveObjectAPI
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

    void executeFunction(const std::function<void()>& func) override;

    bool createThread(const std::string& name, const SchedulingPolicy& schedPolicy, const std::function<void()>& initFunc);

private:
    std::shared_ptr<ActiveObjectThread> m_aoThread;
};

} // namespace V1

} // namespace ActiveObject

} // namespace CommonAPIs