EVENTLOOP_DIR		:= $(SW_DIR)/eventLoop
EVENTLOOP_SRC_DIR	:= $(EVENTLOOP_DIR)/src

EVENTLOOP_CXXFLAGS	:= -lpthread -L$(SDK_LIB_DIR) -ltraceif
EVENTLOOP_LIBAR	:= libeventloopa.a # Static libary
EVENTLOOP_LIBSO	:= libeventloop.so # Dynamic libary

EVENTLOOP_SRCS	=
EVENTLOOP_SRCS	+= eventLoopImpl.cc

EVENTLOOP_OBJS	:= $(EVENTLOOP_SRCS:%.cc=$(OBJ_DIR)/%.o)

EVENTLOOP_INCS	:= \
		-I$(EVENTLOOP_DIR)/if \
		-I$(EVENTLOOP_DIR)/inc \
		-I$(SW_DIR)/threadLocal/if \
		-I$(SDK_INC_DIR)

all: $(EVENTLOOP_OBJS) $(LIB_DIR)/$(EVENTLOOP_LIBAR) $(LIB_DIR)/$(EVENTLOOP_LIBSO) install-header-files-eventloopif

# Build target 1 objects
$(OBJ_DIR)/%.o: $(EVENTLOOP_SRC_DIR)/%.cc
	@mkdir -p $(@D)
	@cd $(<D)
	@echo "  CXX \t\t $@"
	@$(SELF_CXX) $(SELF_LDFLAGS) $(SELF_CFLAGS) $(EVENTLOOP_CXXFLAGS) $(EVENTLOOP_INCS) -o $@ $^

$(LIB_DIR)/$(EVENTLOOP_LIBAR): $(EVENTLOOP_OBJS)
	@mkdir -p $(@D)
	@cd $(<D)
	@echo "  AR \t\t $@"
	@$(SELF_AR) $(SELF_ARFLAGS) $@ $^

$(LIB_DIR)/$(EVENTLOOP_LIBSO): $(EVENTLOOP_OBJS)
	@mkdir -p $(@D)
	@cd $(<D)
	@echo "  CXXLD \t $@"
	@$(SELF_CXX) $(SELF_SOFLAGS) -o $@ $^

install-header-files-eventloopif:
	@mkdir -p $(INC_DIR)
	@echo "  CPY \t\t $(EVENTLOOP_DIR)/if"
	@$(SELF_CPY) $(EVENTLOOP_DIR)/if/*.h $(INC_DIR)

clean-eventloopif:
	$(SELF_RMV) $(EVENTLOOP_OBJS) $(LIB_DIR)/$(EVENTLOOP_LIBAR) $(LIB_DIR)/$(EVENTLOOP_LIBSO)