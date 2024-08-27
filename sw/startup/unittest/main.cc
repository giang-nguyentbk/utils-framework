#include <iostream>
#include <chrono>
#include "startupRegistryIf.h"

using namespace std::chrono_literals;
using namespace UtilsFramework::Startup::V1;

int main()
{
	auto res = IStartupRegistry::getInstance().startAllModules(30s);

	if(!res)
	{
		std::cout << "ERROR: Failed to startup!\n";
		return EXIT_FAILURE;
	}

	std::cout << "INFO: Startup all module successfully!\n";
	return EXIT_SUCCESS;
}