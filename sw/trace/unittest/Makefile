ROOT_DIR := $(shell git rev-parse --show-toplevel)
TARGET = traceIfTest
BIN = ./bin

CC = gcc
CCFLAGS = -g -Wall -Werror -Wextra -lpthread

INC_DIR =
INC_DIR += -I ../if/

all: create_bin $(BIN)/$(TARGET)

create_bin:
	@mkdir -p $(BIN)

$(BIN)/${TARGET}: $(BIN)/traceIf.o $(BIN)/traceIfTest.o 
	$(CC) $(CCFLAGS) -o $@ $^

$(BIN)/traceIf.o: $(ROOT_DIR)/sw/trace/src/traceIf.c
	$(CC) -c $(CCFLAGS) -o $@ $(INC_DIR) $(SRC_DIR) $<

$(BIN)/traceIfTest.o: traceIfTest.c
	$(CC) -c $(CCFLAGS) -o $@ $(INC_DIR) $(SRC_DIR) $<

run:
	$(BIN)/$(TARGET)

clean:
	rm -rf $(BIN)