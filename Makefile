CC       := gcc
CFLAGS   := -std=c11 -Wall -Wextra -Wpedantic -Iinclude $(shell pkg-config --cflags libxml-2.0)
LDFLAGS  := $(shell pkg-config --libs libxml-2.0)

SRC_DIR  := src
INC_DIR  := include
BIN_DIR  := bin
OBJ_DIR  := obj

TARGET   := $(BIN_DIR)/karf
SRCS     := $(wildcard $(SRC_DIR)/*.c)
OBJS     := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: all clean directories

all: directories $(TARGET)

directories:
	@mkdir -p $(BIN_DIR) $(OBJ_DIR)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@chmod +x $@
	@echo "Build successful: $@"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Cleaned build artifacts."
