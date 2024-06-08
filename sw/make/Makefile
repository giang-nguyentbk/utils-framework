ROOT_DIR := $(shell git rev-parse --show-toplevel)

# --------------------------------------------------
# Call Makefiles for init setups and build database
# --------------------------------------------------
include Makefile.config

# --------------------------------------------------
# Call Makefiles for modules of the project
# --------------------------------------------------
include $(SW_DIR)/trace/Makefile




all: rootMakefile

rootMakefile:
	@echo "sw/make/Makefile: Compilation $(PROJ_NAME) done!!! [OK]"

clean:
	$(UF_RMV) $(BIN_DIR)