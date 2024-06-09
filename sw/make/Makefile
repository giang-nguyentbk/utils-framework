# --------------------------------------------------
# Call Makefiles for init setups and build database
# --------------------------------------------------
include Makefile.config

# --------------------------------------------------
# Call Makefiles for modules of the project
# --------------------------------------------------
include $(SW_DIR)/threadLocal/Makefile
include $(SW_DIR)/eventLoop/Makefile
include $(SW_DIR)/itcPubSub/Makefile
include $(SW_DIR)/activeObject/Makefile




all: rootMakefile

rootMakefile:
	@echo "sw/make/Makefile: Compilation $(PROJ_NAME) done!!! [OK]"

clean:
	@echo
	@echo "  RMV \t\t $(BIN_DIR)"
	@$(SELF_RMV) $(BIN_DIR)