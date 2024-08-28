STARTUP_DIR		:= $(SW_DIR)/startup
STARTUP_SRC_DIR		:= $(STARTUP_DIR)/src

STARTUP_CXXFLAGS	:= -lpthread -L$(LIB_DIR) -L$(SDK_LIB_DIR) -ltraceif
# STARTUP_LIBAR		:= libstartupa.a # Static libary
STARTUP_LIBSO		:= libstartup.so # Dynamic libary

STARTUP_SRCS		=
STARTUP_SRCS		+= preparationPhaseResponderImpl.cc
STARTUP_SRCS		+= startupRegistrarIf.cc
STARTUP_SRCS		+= startupRegistryImpl.cc

STARTUP_OBJS		:= $(STARTUP_SRCS:%.cc=$(OBJ_DIR)/%.o)

STARTUP_INCS		:= \
			-I$(STARTUP_DIR)/if \
			-I$(STARTUP_DIR)/inc \
			-I$(SW_DIR)/threadLocal/if \
			-I$(SW_DIR)/common \
			-I$(SDK_INC_DIR)

# all: $(STARTUP_OBJS) $(LIB_DIR)/$(STARTUP_LIBAR) $(LIB_DIR)/$(STARTUP_LIBSO) install-header-files-startupif
all: $(STARTUP_OBJS) $(LIB_DIR)/$(STARTUP_LIBSO) install-header-files-startupif

# Build target 1 objects
$(OBJ_DIR)/%.o: $(STARTUP_SRC_DIR)/%.cc
	@mkdir -p $(@D)
	@cd $(<D)
	@echo "  CXX \t\t $@"
	@$(SELF_CXX) $(SELF_LDFLAGS) $(SELF_CFLAGS) $(STARTUP_INCS) $^ -o $@

##### CANNOT BUILD STATIC LIBRARY FOR LEVEL-2 LIBRARY OR UPWARD #####
##### TO DO THIS, COMBINE THIS LIBRARY'S OBJECTS WITH ALL OTHER LEVEL-1 OBJECT BY YOURSELF WITH 'AR' COMMAND #####
##### OR JUST USING GNU-LIBTOOL OR SIMILARITIES INSTEAD #####
# $(LIB_DIR)/$(STARTUP_LIBAR): $(STARTUP_OBJS)
# 	@mkdir -p $(@D)
# 	@cd $(<D)
# 	@echo "  AR \t\t $@"
# 	@$(SELF_AR) $(SELF_ARFLAGS) $@ $^

$(LIB_DIR)/$(STARTUP_LIBSO): $(STARTUP_OBJS)
	@mkdir -p $(@D)
	@cd $(<D)
	@echo "  CXXLD \t $@"
	@$(SELF_CXX) $(SELF_SOFLAGS) $^ $(STARTUP_CXXFLAGS) -o $@

install-header-files-startupif:
	@mkdir -p $(INC_DIR)
	@echo "  COPY \t\t $(STARTUP_DIR)/if"
	@$(SELF_CPY) $(STARTUP_DIR)/if/*.h $(INC_DIR)

clean-startupif:
	@echo "  RMV \t\t $(BIN_DIR)/startupif"
	@$(SELF_RMV) $(STARTUP_OBJS) $(LIB_DIR)/$(STARTUP_LIBSO)
	@$(SELF_RMV) $(INC_DIR)/startupModuleIf.h
	@$(SELF_RMV) $(INC_DIR)/startupRegistrarIf.h
	@$(SELF_RMV) $(INC_DIR)/startupRegistryIf.h