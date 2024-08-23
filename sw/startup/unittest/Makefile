# SDKSYSROOT is an env variable which should be exported by doing "source <path-to-SDK>/SDK-***/sysroot/env.sh
# which is automatically done by running atbuild-sdk.sh"
SDK_SYSROOT_DIR		:= $(SDKSYSROOT)
SDK_USR_DIR		:= $(SDK_SYSROOT_DIR)/usr
SDK_LIB_DIR		:= $(SDK_USR_DIR)/lib
SDK_INC_DIR		:= $(SDK_USR_DIR)/include

ROOT_DIR 	:= $(shell git rev-parse --show-toplevel)
TARGET 		:= startupTest
BIN_DIR 	:= $(ROOT_DIR)/sw/startup/unittest/bin

CFLAGS 		:= -c -g -Wall -Wextra
CXX 		:= g++

INCLUDE_DIR 	:= \
		-I$(ROOT_DIR)/sw/startup/if \
		-I$(ROOT_DIR)/sw/startup/inc \
		-I$(ROOT_DIR)/sw/startup/unittest \
		-I$(ROOT_DIR)/sw/common \
		-I$(SDK_INC_DIR)

MAIN		:= $(ROOT_DIR)/sw/startup/unittest/main.cc
TEST 		:= $(ROOT_DIR)/sw/startup/unittest/moduleAbcStartup.cc

OBJECTS 	=
OBJECTS 	+= $(BIN_DIR)/main.o
OBJECTS 	+= $(BIN_DIR)/moduleAbcStartup.o

all: create_bin $(OBJECTS) $(BIN_DIR)/$(TARGET)

create_bin:
	@mkdir -p $(BIN_DIR)

$(BIN_DIR)/main.o: $(MAIN)
	@echo "  CXX \t\t $@"
	@$(CXX) $(CFLAGS) $^ $(INCLUDE_DIR) -o $@

$(BIN_DIR)/moduleAbcStartup.o: $(TEST)
	@echo "  CXX \t\t $@"
	@$(CXX) $(CFLAGS) $^ $(INCLUDE_DIR) -o $@

$(BIN_DIR)/$(TARGET): $(OBJECTS)
	@echo "  CXXLD \t $@"
	@$(CXX) $^ -L$(SDK_LIB_DIR) -lstartup -o $@

run:
	@$(BIN_DIR)/$(TARGET)

val:
	sudo valgrind --leak-check=yes --leak-check=full --show-leak-kinds=all $(BIN_DIR)/$(TARGET)

clean:
	rm -rf $(BIN_DIR)