#include <functional>
#include "activeObjectImpl.h"

using namespace UtilsFramework::ActiveObject::implementation;

namespace UtilsFramework
{

namespace ActiveObject
{

namespace V1
{

std::shared_ptr<IActiveObject> IActiveObject::create(const std::function<void()>& initFunc, const SchedulingPolicy& schedPolicy)
{
	return create("ActiveObjectThread", initFunc, schedPolicy);
}

std::shared_ptr<IActiveObject> IActiveObject::create(const std::string& name, const std::function<void()>& initFunc, const SchedulingPolicy& schedPolicy)
{
	auto ao = std::make_shared<ActiveObjectImpl>();
	if(!ao->createThread(name, schedPolicy, initFunc))
	{
		ao.reset(); // Reset shared_ptr to nullptr
	}

	return ao;
}

bool ActiveObjectImpl::createThread(const std::string& name, const SchedulingPolicy& schedPolicy, const std::function<void()>& initFunc)
{
	bool isFifo = (schedPolicy == SchedulingPolicy::Fifo);

	m_aoThread = std::make_shared<ActiveObjectThread>(name);

	return m_aoThread->start(isFifo, initFunc);
}

void ActiveObjectImpl::executeFunction(const std::function<void()>& func)
{
	m_aoThread->scheduleFunction(func);
}

} // namespace V1

} // namespace ActiveObject

} // namespace UtilsFramework