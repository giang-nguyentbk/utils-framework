#include "preparationPhaseResponderImpl.h"

namespace UtilsFramework
{
namespace Startup
{
namespace V1
{

PreparationPhaseResponderImpl::PreparationPhaseResponderImpl(const std::string& moduleName, const std::chrono::time_point<std::chrono::steady_clock>& timePoint)
	: m_moduleName(moduleName),
	  m_timePoint(timePoint),
	  m_future(m_promise.get_future())
{
}

void PreparationPhaseResponderImpl::ready(bool success)
{
	if(m_future.valid())
	{
		m_promise.set_value(success);
	}
}

bool PreparationPhaseResponderImpl::wait(bool& success)
{
	if(m_future.valid() && m_future.wait_until(m_timePoint) == std::future_status::ready)
	{
		success = m_future.get();
		return true;
	}

	return false; // Could be timeout
}

} // namespace V1

} // namespace Startup

} // namespace UtilsFramework