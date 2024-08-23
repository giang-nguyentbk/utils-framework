#pragma once

#include <memory>
#include <string>
#include "startupRegistrarIf.h"

class ModuleAbcStartup : public UtilsFramework::Startup::V1::IStartupModule
{
public:
	ModuleAbcStartup() = default;
	virtual ~ModuleAbcStartup() = default;

	void prepare(std::shared_ptr<IPreparationPhaseResponder> responder) override;
	void start() override;
	std::string getModuleName() const override;

private:
	DECLARE_STARTUP_MODULE

};