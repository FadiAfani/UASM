BUILD = build
SRC = src
BIN = bin
CC = g++-13
CLFAGS = -g -std=c++20 

OBJS = $(SRCS:$(SRC)/%.cpp=$(BUILD)/%.o)
SRCS = $(wildcard $(SRC)/*.cpp)


$(BUILD)/%.o: $(SRC)/%.cpp
	$(CC) $(CLFAGS) -c $< -o $@

all: $(OBJS)
	$(CC) -o $(BIN)/uasm $^ $(CFLAGS)

clear_bin:
	rm -rf $(BIN)/*

clear_build:
	rm -rf $(BUILD)/*
