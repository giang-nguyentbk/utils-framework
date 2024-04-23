ROOT_DIR 	:= $(shell git rev-parse --show-toplevel)
SW_DIR		:= $(ROOT_DIR)/sw
BIN_DIR		:= ./bin

TARGET 		= eventLoopTest

CXX		= g++
RMV		= rm -rf
CPPFLAGS 	= -c -lpthread

SRC_FILES	+= \
		src/eventLoopImpl.cc \
		unittest/eventLoopTest.cc

OBJ_FILES	:= $(SRC_FILES:%.cc=$(BIN_DIR)/%.o)

INC_PATH	+= \
		-I$(SW_DIR)/eventLoop/if \
		-I$(SW_DIR)/eventLoop/inc \
		-I$(SW_DIR)/threadLocal/if

all: $(OBJ_FILES) $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/%.o : $(SW_DIR)/eventLoop/%.cc
	@mkdir -p $(@D)
	@echo "  CXX  $@"
	@$(CXX) $(CPPFLAGS) $(INC_PATH) -o $@ $<

$(BIN_DIR)/$(TARGET): $(OBJ_FILES)
	@echo "  LINKING  $@"
	@$(CXX) $(INC_PATH) -o $@ $^

run:
	@$(BIN_DIR)/$(TARGET)

clean:
	$(RMV) $(BIN_DIR)