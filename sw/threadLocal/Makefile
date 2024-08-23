THREADLOCAL_DIR	:= $(SW_DIR)/threadLocal

all: install-header-files-threadlocalif

install-header-files-threadlocalif:
	@mkdir -p $(INC_DIR)
	@echo "  COPY \t\t $(THREADLOCAL_DIR)/if"
	@$(SELF_CPY) $(THREADLOCAL_DIR)/if/*.h $(INC_DIR)

clean-threadlocalif:
	@echo "  RMV \t\t $(BIN_DIR)/threadlocalif"
	@$(SELF_RMV) $(INC_DIR)/threadLocalIf.h