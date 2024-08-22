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
#include <chrono>

#include "startupModuleIf.h"

namespace UtilsFramework
{
namespace Startup
{
namespace V1
{

class StartupRegistryImpl;

class IStartupRegistry
{
public:
	static IStartupRegistry& getThreadLocalInstance();

	IStartupRegistry(const IStartupRegistry& other) = delete;
	IStartupRegistry(IStartupRegistry&& other) = delete;
	IStartupRegistry& operator=(const IStartupRegistry& other) = delete;
	IStartupRegistry& operator=(IStartupRegistry&& other) = delete;

	virtual void registerModuleAllocator(const UtilsFramework::Startup::V1::IStartupModule::StartupModuleAllocatorFunction& allocator) = 0;
	virtual bool startAllModules(const std::chrono::seconds& startupTimeout) = 0;

protected:
	IStartupRegistry() = default;
	virtual ~IStartupRegistry() = default;

}; // class IStartupRegistry

} // namespace V1

} // namespace Startup

} // namespace UtilsFramework