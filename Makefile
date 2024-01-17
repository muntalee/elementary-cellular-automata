# Makefile for simulate

# Directories
SRC_DIR := src
INCLUDE_DIR := include
BIN_DIR := bin

# Files
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(SRC_FILES))
EXECUTABLE := $(BIN_DIR)/simulate

# Compiler and flags
CC := gcc
CFLAGS := -I$(INCLUDE_DIR) -Wall -std=c11 -pedantic $(shell sdl2-config --cflags) -O3 -g
LDFLAGS := $(shell sdl2-config --libs)

# Determine GLFLAG based on the operating system
OS_NAME := $(shell uname -o 2>/dev/null || uname -s)
ifeq ($(OS_NAME), Msys)
    GLFLAG := -lopengl32
else ifeq ($(OS_NAME), Darwin)
    GLFLAG := -framework OpenGL
else
    GLFLAG := -lGL
endif

# Targets and rules
.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ_FILES)
	$(CC) $^ -o $@ $(LDFLAGS) $(GLFLAG) -lm

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf $(BIN_DIR)/*.o $(EXECUTABLE)

