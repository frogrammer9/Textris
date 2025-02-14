# Compiler
CC = clang

# Compiler flags
CFLAGS = -Wall -Wextra -O0 -g 

# Build directory
BUILD_DIR = build

# Source directory
SRC_DIR = src

# Source files (find all .c files in the directory)
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Object files (replace .c with .o and place them in the 'build' directory)
OBJS = $(patsubst src/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Output executable
TARGET = Textris

# Default rule: Build the program
all: clean $(TARGET)

# Link all object files into the final executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# Compile each .c file into an object file inside 'build' directory
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up compiled files and build directory
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

run: all
	./$(TARGET)

# Phony targets (not actual files)
.PHONY: all clean run

