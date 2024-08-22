/*
*        ________________           ________                                                    ______  
* ____  ___  /___(_)__  /_______    ___  __/____________ _______ ___________      _________________  /__
* _  / / /  __/_  /__  /__  ___/    __  /_ __  ___/  __ `/_  __ `__ \  _ \_ | /| / /  __ \_  ___/_  //_/
* / /_/ // /_ _  / _  / _(__  )     _  __/ _  /   / /_/ /_  / / / / /  __/_ |/ |/ // /_/ /  /   _  ,<   
* \__,_/ \__/ /_/  /_/  /____/      /_/    /_/    \__,_/ /_/ /_/ /_/\___/____/|__/ \____//_/    /_/|_|  
*                                                                                                       
*/

#pragma once

#include <string>
#include <memory>
#include <functional>

#include "startupModuleIf.h"

#define DECLARE_STARTUP_MODULE \
	static std::shared_ptr<UtilsFramework::Startup::V1::IStartupModule> createModule(); \
	static UtilsFramework::Startup::V1::IStartupRegistrar startupRegistrar;

#define REGISTER_STARTUP_MODULE(ModuleClassName) \
	std::shared_ptr<UtilsFramework::Startup::V1::IStartupModule> ModuleClassName::createModule() \
	{ \
		return std::make_shared<ModuleClassName>(); \
	} \
	UtilsFramework::Startup::V1::IStartupRegistrar ModuleClassName::startupRegistrar(ModuleClassName::createModule);

namespace UtilsFramework
{
namespace Startup
{
namespace V1
{

class IStartupRegistrar
{
public:
	explicit IStartupRegistrar(const IStartupModule::StartupModuleAllocatorFunction& allocator);
	virtual ~IStartupRegistrar() = default;

	IStartupRegistrar(const IStartupRegistrar& other) = delete;
	IStartupRegistrar(IStartupRegistrar&& other) = delete;
	IStartupRegistrar& operator=(const IStartupRegistrar& other) = delete;
	IStartupRegistrar& operator=(IStartupRegistrar&& other) = delete;

}; // class IStartupRegistrar

} // namespace V1

} // namespace Startup

} // namespace UtilsFramework