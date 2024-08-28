TIMER_DIR		:= $(SW_DIR)/timer
TIMER_SRC_DIR		:= $(TIMER_DIR)/src

TIMER_CXXFLAGS		:= -lpthread -L$(SDK_LIB_DIR) -ltraceif
# TIMER_LIBAR		:= libtimera.a # Static libary
TIMER_LIBSO		:= libtimer.so # Dynamic libary

TIMER_SRCS		=
TIMER_SRCS		+= timerManagerImpl.cc

TIMER_OBJS		:= $(TIMER_SRCS:%.cc=$(OBJ_DIR)/%.o)

TIMER_INCS		:= \
			-I$(TIMER_DIR)/if \
			-I$(TIMER_DIR)/inc \
			-I$(SW_DIR)/threadLocal/if \
			-I$(SW_DIR)/eventLoop/if \
			-I$(SW_DIR)/common \
			-I$(SDK_INC_DIR)

# all: $(TIMER_OBJS) $(LIB_DIR)/$(TIMER_LIBAR) $(LIB_DIR)/$(TIMER_LIBSO) install-header-files-timerif
all: $(TIMER_OBJS) $(LIB_DIR)/$(TIMER_LIBSO) install-header-files-timerif

# Build target 1 objects
$(OBJ_DIR)/%.o: $(TIMER_SRC_DIR)/%.cc
	@mkdir -p $(@D)
	@cd $(<D)
	@echo "  CXX \t\t $@"
	@$(SELF_CXX) $(SELF_LDFLAGS) $(SELF_CFLAGS) $(TIMER_INCS) $^ -o $@

##### CANNOT BUILD STATIC LIBRARY FOR LEVEL-2 LIBRARY OR UPWARD #####
##### TO DO THIS, COMBINE THIS LIBRARY'S OBJECTS WITH ALL OTHER LEVEL-1 OBJECT BY YOURSELF WITH 'AR' COMMAND #####
##### OR JUST USING GNU-LIBTOOL OR SIMILARITIES INSTEAD #####
# $(LIB_DIR)/$(TIMER_LIBAR): $(TIMER_OBJS)
# 	@mkdir -p $(@D)
# 	@cd $(<D)
# 	@echo "  AR \t\t $@"
# 	@$(SELF_AR) $(SELF_ARFLAGS) $@ $^

$(LIB_DIR)/$(TIMER_LIBSO): $(TIMER_OBJS)
	@mkdir -p $(@D)
	@cd $(<D)
	@echo "  CXXLD \t $@"
	@$(SELF_CXX) $(SELF_SOFLAGS) $^ $(TIMER_CXXFLAGS) -o $@

install-header-files-timerif:
	@mkdir -p $(INC_DIR)
	@echo "  COPY \t\t $(TIMER_DIR)/if"
	@$(SELF_CPY) $(TIMER_DIR)/if/*.h $(INC_DIR)

clean-timerif:
	@echo "  RMV \t\t $(BIN_DIR)/timerif"
	@$(SELF_RMV) $(TIMER_OBJS) $(LIB_DIR)/$(TIMER_LIBSO)
	@$(SELF_RMV) $(INC_DIR)/timerManagerIf.h
	@$(SELF_RMV) $(INC_DIR)/timerSubscriberIf.h