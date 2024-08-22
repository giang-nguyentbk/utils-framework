/*
*        ________________           ________                                                    ______  
* ____  ___  /___(_)__  /_______    ___  __/____________ _______ ___________      _________________  /__
* _  / / /  __/_  /__  /__  ___/    __  /_ __  ___/  __ `/_  __ `__ \  _ \_ | /| / /  __ \_  ___/_  //_/
* / /_/ // /_ _  / _  / _(__  )     _  __/ _  /   / /_/ /_  / / / / /  __/_ |/ |/ // /_/ /  /   _  ,<   
* \__,_/ \__/ /_/  /_/  /____/      /_/    /_/    \__,_/ /_/ /_/ /_/\___/____/|__/ \____//_/    /_/|_|  
*                                                                                                       
*/

#pragma once

#include <future>
#include <chrono>
#include <string>

#include "startupModuleIf.h"

namespace UtilsFramework
{
namespace Startup
{
namespace V1
{

class PreparationPhaseResponderImpl : public IStartupModule::IPreparationPhaseResponder
{
public:
	explicit PreparationPhaseResponderImpl(const std::string& moduleName, const std::chrono::time_point<std::chrono::steady_clock>& timePoint);
	virtual ~PreparationPhaseResponderImpl() = default;

	PreparationPhaseResponderImpl(const PreparationPhaseResponderImpl& other) = delete;
	PreparationPhaseResponderImpl(PreparationPhaseResponderImpl&& other) = delete;
	PreparationPhaseResponderImpl& operator=(const PreparationPhaseResponderImpl& other) = delete;
	PreparationPhaseResponderImpl& operator=(PreparationPhaseResponderImpl&& other) = delete;

	void ready(bool success) override;
	bool wait(bool& success);
	const std::string& getModuleName() const
	{
		return m_moduleName;
	};

private:
	std::string m_moduleName;
	std::chrono::time_point<std::chrono::steady_clock> m_timePoint;
	std::promise<bool> m_promise;
	std::future<bool> m_future;

}; // class PreparationPhaseResponderImpl

} // namespace V1

} // namespace Startup

} // namespace UtilsFramework