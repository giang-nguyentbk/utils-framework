ITCPUBSUB_DIR		:= $(SW_DIR)/itcPubSub
ITCPUBSUB_SRC_DIR	:= $(ITCPUBSUB_DIR)/src

ITCPUBSUB_CXXFLAGS	:= -lpthread -L$(LIB_DIR) -leventloop -L$(SDK_LIB_DIR) -litc
ITCPUBSUB_LIBAR	:= libitcpubsuba.a # Static libary
ITCPUBSUB_LIBSO	:= libitcpubsub.so # Dynamic libary

ITCPUBSUB_SRCS	=
ITCPUBSUB_SRCS	+= itcPubSubImpl.cc

ITCPUBSUB_OBJS	:= $(ITCPUBSUB_SRCS:%.cc=$(OBJ_DIR)/%.o)

ITCPUBSUB_INCS	:= \
		-I$(ITCPUBSUB_DIR)/if \
		-I$(ITCPUBSUB_DIR)/inc \
		-I$(SW_DIR)/threadLocal/if \
		-I$(SW_DIR)/eventLoop/if \
		-I$(SDK_INC_DIR)

all: $(ITCPUBSUB_OBJS) $(LIB_DIR)/$(ITCPUBSUB_LIBAR) $(LIB_DIR)/$(ITCPUBSUB_LIBSO) install-header-files-itcpubsubif

# Build target 1 objects
$(OBJ_DIR)/%.o: $(ITCPUBSUB_SRC_DIR)/%.cc
	@mkdir -p $(@D)
	@cd $(<D)
	@echo "  CXX \t\t $@"
	@$(SELF_CXX) $(SELF_LDFLAGS) $(SELF_CFLAGS) $(ITCPUBSUB_CXXFLAGS) $(ITCPUBSUB_INCS) -o $@ $^

$(LIB_DIR)/$(ITCPUBSUB_LIBAR): $(ITCPUBSUB_OBJS)
	@mkdir -p $(@D)
	@cd $(<D)
	@echo "  AR \t\t $@"
	@$(SELF_AR) $(SELF_ARFLAGS) $@ $^

$(LIB_DIR)/$(ITCPUBSUB_LIBSO): $(ITCPUBSUB_OBJS)
	@mkdir -p $(@D)
	@cd $(<D)
	@echo "  CXXLD \t $@"
	@$(SELF_CXX) $(SELF_SOFLAGS) -o $@ $^

install-header-files-itcpubsubif:
	@mkdir -p $(INC_DIR)
	@echo "  CPY \t\t $(ITCPUBSUB_DIR)/if"
	@$(SELF_CPY) $(ITCPUBSUB_DIR)/if/*.h $(INC_DIR)

clean-itcpubsubif:
	$(SELF_RMV) $(ITCPUBSUB_OBJS) $(LIB_DIR)/$(ITCPUBSUB_LIBAR) $(LIB_DIR)/$(ITCPUBSUB_LIBSO)