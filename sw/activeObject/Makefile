ACTIVEOBJECT_DIR	:= $(SW_DIR)/activeObject
ACTIVEOBJECT_SRC_DIR	:= $(ACTIVEOBJECT_DIR)/src

ACTIVEOBJECT_CXXFLAGS	:= -lpthread -L$(LIB_DIR) -leventloop
ACTIVEOBJECT_LIBAR	:= libactiveobjecta.a # Static libary
ACTIVEOBJECT_LIBSO	:= libactiveobject.so # Dynamic libary

ACTIVEOBJECT_SRCS	=
ACTIVEOBJECT_SRCS	+= activeObjectImpl.cc
ACTIVEOBJECT_SRCS	+= activeObjectThread.cc

ACTIVEOBJECT_OBJS	:= $(ACTIVEOBJECT_SRCS:%.cc=$(OBJ_DIR)/%.o)

ACTIVEOBJECT_INCS	:= \
		-I$(ACTIVEOBJECT_DIR)/if \
		-I$(ACTIVEOBJECT_DIR)/inc \
		-I$(SW_DIR)/threadLocal/if \
		-I$(SW_DIR)/eventLoop/if \
		-I$(SDK_INC_DIR)

all: $(ACTIVEOBJECT_OBJS) $(LIB_DIR)/$(ACTIVEOBJECT_LIBAR) $(LIB_DIR)/$(ACTIVEOBJECT_LIBSO) install-header-files-activeobjectif

# Build target 1 objects
$(OBJ_DIR)/%.o: $(ACTIVEOBJECT_SRC_DIR)/%.cc
	@mkdir -p $(@D)
	@cd $(<D)
	@echo "  CXX \t\t $@"
	@$(SELF_CXX) $(SELF_LDFLAGS) $(SELF_CFLAGS) $(ACTIVEOBJECT_CXXFLAGS) $(ACTIVEOBJECT_INCS) -o $@ $^

$(LIB_DIR)/$(ACTIVEOBJECT_LIBAR): $(ACTIVEOBJECT_OBJS)
	@mkdir -p $(@D)
	@cd $(<D)
	@echo "  AR \t\t $@"
	@$(SELF_AR) $(SELF_ARFLAGS) $@ $^

$(LIB_DIR)/$(ACTIVEOBJECT_LIBSO): $(ACTIVEOBJECT_OBJS)
	@mkdir -p $(@D)
	@cd $(<D)
	@echo "  CXXLD \t $@"
	@$(SELF_CXX) $(SELF_SOFLAGS) -o $@ $^

install-header-files-activeobjectif:
	@mkdir -p $(INC_DIR)
	@echo "  CPY \t\t $(ACTIVEOBJECT_DIR)/if"
	@$(SELF_CPY) $(ACTIVEOBJECT_DIR)/if/*.h $(INC_DIR)

clean-activeobjectif:
	$(SELF_RMV) $(ACTIVEOBJECT_OBJS) $(LIB_DIR)/$(ACTIVEOBJECT_LIBAR) $(LIB_DIR)/$(ACTIVEOBJECT_LIBSO)