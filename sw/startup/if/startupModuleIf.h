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

namespace UtilsFramework
{
namespace Startup
{
namespace V1
{

/*
	Divide into 2 phases:
		+ Preparation phase: All modules should prepare their initialization first asynchronously.
		+ Startup phase: All modules will do their startup process asynchronously.
*/

class IStartupModule
{
public:
	using StartupModuleAllocatorFunction = std::function<std::shared_ptr<IStartupModule>()>;

	class IPreparationPhaseResponder
	{
	public:
		virtual void ready(bool success) = 0;

		// Delete copy/move constructor and assignment
		IPreparationPhaseResponder(const IPreparationPhaseResponder& other) = delete;
		IPreparationPhaseResponder& operator=(const IPreparationPhaseResponder& other) = delete;
		IPreparationPhaseResponder(IPreparationPhaseResponder&& other) = delete;
		IPreparationPhaseResponder& operator=(IPreparationPhaseResponder&& other) = delete;

	protected:
		IPreparationPhaseResponder() = default;
		virtual ~IPreparationPhaseResponder() = default;
	};

	virtual void prepare(std::shared_ptr<IPreparationPhaseResponder> responder) = 0;
	virtual void start() = 0;
	virtual std::string getModuleName() const = 0;

	IStartupModule(const IStartupModule& other) = delete;
	IStartupModule(IStartupModule&& other) = delete;
	IStartupModule& operator=(const IStartupModule& other) = delete;
	IStartupModule& operator=(IStartupModule&& other) = delete;

protected:
	IStartupModule() = default;
	virtual ~IStartupModule() = default;

}; // class IStartupModule

} // namespace V1

} // namespace Startup

} // namespace UtilsFramework