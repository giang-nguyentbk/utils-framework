#include "startupRegistryIf.h"
#include "startupRegistrarIf.h"

namespace UtilsFramework
{
namespace Startup
{
namespace V1
{

IStartupRegistrar::IStartupRegistrar(const IStartupModule::StartupModuleAllocatorFunction& allocator)
{
	UtilsFramework::Startup::V1::IStartupRegistry::getThreadLocalInstance().registerModuleAllocator(allocator);
}

} // namespace V1

} // namespace Startup

} // namespace UtilsFramework