ROOT_DIR 	:= $(shell git rev-parse --show-toplevel)
SW_DIR		:= $(ROOT_DIR)/sw
BIN_DIR		:= ./bin

TARGET 		= timerManagerTest
OBJ_FILES	:= $(BIN_DIR)/timerManagerTest.o

SDK_SYSROOT_DIR		:= $(SDKSYSROOT)
SDK_USR_DIR		:= $(SDK_SYSROOT_DIR)/usr
SDK_LIB_DIR		:= $(SDK_USR_DIR)/lib
SDK_INC_DIR		:= $(SDK_USR_DIR)/include

CXX		= g++
RMV		= rm -rf
CPPFLAGS 	= -c -g -Wall -Werror -Wextra

INC_PATH	+= \
		-I$(SDK_INC_DIR)

all: $(OBJ_FILES) $(BIN_DIR)/$(TARGET)

$(OBJ_FILES): $(SW_DIR)/timer/unittest/timerManagerTest.cc
	@mkdir -p $(@D)
	@echo "  CXX \t\t $@"
	@$(CXX) $(INC_PATH) $(CPPFLAGS) $^ -o $@

$(BIN_DIR)/$(TARGET): $(OBJ_FILES)
	@echo "  LINKING \t $@"
	@$(CXX) $^ -L$(SDK_LIB_DIR) -leventloopa -ltimera -ltraceifa -o $@

run:
	@$(BIN_DIR)/$(TARGET)

clean:
	$(RMV) $(BIN_DIR)