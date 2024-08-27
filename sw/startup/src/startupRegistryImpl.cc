#include <stringUtils.h>
#include <traceIf.h>

#include "util_framework_tpt_provider.h"
#include "startupRegistryImpl.h"
#include "preparationPhaseResponderImpl.h"

using namespace CommonUtils::V1::StringUtils;

namespace UtilsFramework
{
namespace Startup
{
namespace V1
{

IStartupRegistry& IStartupRegistry::getInstance()
{
    	return StartupRegistryImpl::getInstance();
}

StartupRegistryImpl& StartupRegistryImpl::getInstance()
{
	static StartupRegistryImpl instance;
    	return instance;
}

StartupRegistryImpl::~StartupRegistryImpl()
{
	m_startupModuleVec.clear();
	m_allocatorFuncVec.clear();
}

void StartupRegistryImpl::registerModuleAllocator(const UtilsFramework::Startup::V1::IStartupModule::StartupModuleAllocatorFunction& allocator)
{
	m_allocatorFuncVec.push_back(allocator);
}

bool StartupRegistryImpl::startAllModules(const std::chrono::seconds& startupTimeout)
{
	bool result = false;

	std::vector<std::shared_ptr<UtilsFramework::Startup::V1::PreparationPhaseResponderImpl>> responderVec;
	for(const auto& allocator : m_allocatorFuncVec)
	{
		auto startupModule = allocator();
		m_startupModuleVec.push_back(startupModule);

		auto responder = std::make_shared<UtilsFramework::Startup::V1::PreparationPhaseResponderImpl>
		(startupModule->getModuleName(), std::chrono::steady_clock::now() + startupTimeout);
		responderVec.push_back(responder);

		TPT_TRACE(TRACE_INFO, SSTR("Preparing initialization for \"", responder->getModuleName(), "\""));
		startupModule->prepare(responder);
	}

	for(const auto& resp : responderVec)
	{
		if(auto success = false; resp->wait(success))
		{
			if(success)
			{
				TPT_TRACE(TRACE_INFO, SSTR("Prepared initialization for \"", resp->getModuleName(), "\" successfully!"));
				result = true;
			}
			else
			{
				TPT_TRACE(TRACE_ERROR, SSTR("Failed to prepare initialization for \"", resp->getModuleName(), "\""));
			}
		}
		else
		{
			TPT_TRACE(TRACE_ERROR, SSTR("Timeout when trying to prepare initialization for \"", resp->getModuleName(), "\", timeout = ", startupTimeout.count(), " seconds!"));
		}
	}

	if(result)
	{
		for(const auto& mod : m_startupModuleVec)
		{
			TPT_TRACE(TRACE_INFO, SSTR("Starting up \"", mod->getModuleName(), "\"..."));
			mod->start();
		}
	}

	return result;
}

} // namespace V1

} // namespace Startup

} // namespace UtilsFramework