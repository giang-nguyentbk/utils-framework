/*
*        ________________           ________                                                    ______  
* ____  ___  /___(_)__  /_______    ___  __/____________ _______ ___________      _________________  /__
* _  / / /  __/_  /__  /__  ___/    __  /_ __  ___/  __ `/_  __ `__ \  _ \_ | /| / /  __ \_  ___/_  //_/
* / /_/ // /_ _  / _  / _(__  )     _  __/ _  /   / /_/ /_  / / / / /  __/_ |/ |/ // /_/ /  /   _  ,<   
* \__,_/ \__/ /_/  /_/  /____/      /_/    /_/    \__,_/ /_/ /_/ /_/\___/____/|__/ \____//_/    /_/|_|  
*                                                                                                       
*/

#pragma once

#include <vector>

#include "startupRegistryIf.h"

namespace UtilsFramework
{
namespace Startup
{
namespace V1
{

class StartupRegistryImpl : public IStartupRegistry
{
public:
	static StartupRegistryImpl& getThreadLocalInstance();
	static void reset();

	StartupRegistryImpl() = default;
	virtual ~StartupRegistryImpl();

	StartupRegistryImpl(const StartupRegistryImpl& other) = delete;
	StartupRegistryImpl(StartupRegistryImpl&& other) = delete;
	StartupRegistryImpl& operator=(const StartupRegistryImpl& other) = delete;
	StartupRegistryImpl& operator=(StartupRegistryImpl&& other) = delete;

	void registerModuleAllocator(const UtilsFramework::Startup::V1::IStartupModule::StartupModuleAllocatorFunction& allocator) override;
	bool startAllModules(const std::chrono::seconds& startupTimeout = 1min) override;

private:
	std::vector<UtilsFramework::Startup::V1::IStartupModule::StartupModuleAllocatorFunction> m_allocatorFuncVec;
	std::vector<std::shared_ptr<UtilsFramework::Startup::V1::IStartupModule>> m_startupModuleVec;

}; // class StartupRegistryImpl

} // namespace V1

} // namespace Startup

} // namespace UtilsFramework