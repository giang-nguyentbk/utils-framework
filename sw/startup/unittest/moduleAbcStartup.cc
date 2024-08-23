#include "moduleAbcStartup.h"
#include "startupRegistrarIf.h"

using namespace UtilsFramework::Startup::V1;

static const std::string MODULE_ABC_STARTUP_NAME {"ModuleAbcStartup"};

REGISTER_STARTUP_MODULE(ModuleAbcStartup);

void ModuleAbcStartup::prepare(std::shared_ptr<IPreparationPhaseResponder> responder)
{
	// Do something to prepare initialization for this module
	responder->ready(true);
}

void ModuleAbcStartup::start()
{
	// Do something to start this module
}

std::string ModuleAbcStartup::getModuleName() const
{
	return MODULE_ABC_STARTUP_NAME;
}