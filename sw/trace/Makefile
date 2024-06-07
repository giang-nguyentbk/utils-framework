ROOT_DIR 	:= $(shell git rev-parse --show-toplevel)
BIN_DIR		:= $(ROOT_DIR)/sw/bin
TRACELIB_DIR 	:= $(BIN_DIR)/libs/trace
TRACEOBJ_DIR 	:= $(BIN_DIR)/objs/trace
TRACE_DIR 	:= $(ROOT_DIR)/sw/trace

TRACE_CC	:= gcc
TRACE_AR	:= ar
TRACE_LDFLAGS	:= -fPIC
TRACE_CFLAGS	:= -c -g -Wall -Werror -Wextra
TRACE_ARFLAGS	:= -rcs
TRACE_SOFLAGS	:= -shared

TRACE_LIBAR	:= libtracea.a # Static libary
TRACE_LIBSO	:= libtrace.so # Dynamic libary

TRACE_SRCS	:= src/traceIf.c
TRACE_OBJS	:= $(TRACEOBJ_DIR)/traceIf.o
TRACE_INCS	:= -I$(TRACE_DIR)/if

all: $(TRACE_OBJS) $(TRACELIB_DIR)/$(TRACE_LIBAR) $(TRACELIB_DIR)/$(TRACE_LIBSO)

# Build target 1 objects
$(TRACE_OBJS): $(TRACE_DIR)/src/traceIf.c
	@mkdir -p $(@D)
	@cd $(<D)
	@echo "  CC \t\t $@"
	@$(TRACE_CC) $^ $(TRACE_LDFLAGS) $(TRACE_CFLAGS) $(TRACE_INCS) -o $@

$(TRACELIB_DIR)/$(TRACE_LIBAR): $(TRACE_OBJS)
	@mkdir -p $(@D)
	@cd $(<D)
	@echo "  AR \t\t $@"
	@$(TRACE_AR) $(TRACE_ARFLAGS) $@ $^

$(TRACELIB_DIR)/$(TRACE_LIBSO): $(TRACE_OBJS)
	@mkdir -p $(@D)
	@cd $(<D)
	@echo "  CCLD \t\t $@"
	@$(TRACE_CC) $(TRACE_SOFLAGS) -o $(TRACELIB_DIR)/$(TRACE_LIBSO) $^

clean:
	rm -rf $(BIN_DIR)