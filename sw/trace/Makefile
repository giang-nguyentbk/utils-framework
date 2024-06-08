TRACE_DIR	:= $(SW_DIR)/trace

TRACE_LIBAR	:= libtracea.a # Static libary
TRACE_LIBSO	:= libtrace.so # Dynamic libary

TRACE_SRCS	=
TRACE_SRCS	+= $(TRACE_DIR)/src/traceIf.c

TRACE_OBJS	=
TRACE_OBJS	+= $(OBJ_DIR)/traceIf.o

TRACE_INCS	:= \
		-I$(TRACE_DIR)/if

all: $(TRACE_OBJS) $(LIB_DIR)/$(TRACE_LIBAR) $(LIB_DIR)/$(TRACE_LIBSO) install-header-files

# Build target 1 objects
$(TRACE_OBJS): $(TRACE_DIR)/src/traceIf.c
	@mkdir -p $(@D)
	@cd $(<D)
	@echo "  CC \t\t $@"
	@$(UF_CC) $^ $(UF_LDFLAGS) $(UF_CFLAGS) $(TRACE_INCS) -o $@

$(LIB_DIR)/$(TRACE_LIBAR): $(TRACE_OBJS)
	@mkdir -p $(@D)
	@cd $(<D)
	@echo "  AR \t\t $@"
	@$(UF_AR) $(UF_ARFLAGS) $@ $^

$(LIB_DIR)/$(TRACE_LIBSO): $(TRACE_OBJS)
	@mkdir -p $(INC_DIR)
	@cd $(<D)
	@echo "  CCLD \t\t $@"
	@$(UF_CC) $(UF_SOFLAGS) -o $(LIB_DIR)/$(TRACE_LIBSO) $^

install-header-files:
	@mkdir -p $(@D)
	@echo
	@echo "Installing header files into SDK..."
	@$(UF_CPY) $(TRACE_DIR)/if/*.h $(INC_DIR)/

clean-trace:
	$(UF_RMV) $(TRACE_OBJS) $(LIB_DIR)/$(TRACE_LIBAR) $(LIB_DIR)/$(TRACE_LIBSO)