CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2 -std=c11 -Isrc

BIN = sezar
TEST_BIN = test_sezar

SRC_DIR = src
TEST_DIR = tests

SRCS = $(SRC_DIR)/sezar.c $(SRC_DIR)/cli.c
TEST_SRCS = $(SRC_DIR)/sezar.c $(TEST_DIR)/test_sezar.c
HEADERS = $(SRC_DIR)/sezar.h

.PHONY: all test clean run

all: $(BIN)

$(BIN): $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) -o $(BIN) $(SRCS)

$(TEST_BIN): $(TEST_SRCS) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TEST_BIN) $(TEST_SRCS)

test: $(TEST_BIN)
	./$(TEST_BIN)

run: $(BIN)
	./$(BIN)

clean:
	rm -f $(BIN) $(TEST_BIN)
