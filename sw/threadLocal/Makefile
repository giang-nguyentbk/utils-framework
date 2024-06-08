THREADLOCAL_DIR	:= $(SW_DIR)/threadLocal

all: install-header-files-threadlocalif

install-header-files-threadlocalif:
	@mkdir -p $(INC_DIR)
	@echo "  CPY \t\t $(THREADLOCAL_DIR)/if"
	@$(SELF_CPY) $(THREADLOCAL_DIR)/if/*.h $(INC_DIR)