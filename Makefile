# Makefile for Smart EV Charging Network Operations Platform

CXX = g++
CXXFLAGS = -std=c++98 -Wall -pedantic
SRC_DIR = src
DB_DIR = db
DATA_DIR = data
OBJ_DIR = obj
EXEC = evmanager

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Directories that must exist
REQUIRED_DIRS = $(DB_DIR) $(DATA_DIR) $(OBJ_DIR)

# Default target
.PHONY: all
all: create_dirs build

# Create required directories
.PHONY: create_dirs
create_dirs:
	@mkdir -p $(REQUIRED_DIRS)
	@echo "Created required directories: $(REQUIRED_DIRS)"

# Compile object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo "Compiled: $<"

# Link executable
$(EXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@
	@echo "Executable created: $(EXEC)"

# Build target
.PHONY: build
build: create_dirs $(EXEC)

# Run target
.PHONY: run
run: build
	@echo "Starting Smart EV Charging Network Operations Platform..."
	@./$(EXEC)

# Clean target
.PHONY: clean
clean:
	@rm -rf $(OBJ_DIR) $(EXEC)
	@echo "Cleaned: object files and executable"

# Deep clean (includes directories and generated files)
.PHONY: clean-all
clean-all: clean
	@rm -rf $(DATA_DIR)/* $(DB_DIR)/*
	@echo "Deep cleaned: data and db directories cleared"

# Rebuild target
.PHONY: rebuild
rebuild: clean build

# Help target
.PHONY: help
help:
	@echo "Smart EV Charging Network Operations - Makefile Targets"
	@echo "========================================================"
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all          - Create directories and build the project (default)"
	@echo "  build        - Build the project (compile and link)"
	@echo "  run          - Build and run the executable"
	@echo "  rebuild      - Clean and rebuild from scratch"
	@echo "  clean        - Remove object files and executable"
	@echo "  clean-all    - Remove object files, executable, and generated data"
	@echo "  help         - Display this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make         - Build the project"
	@echo "  make run     - Build and run the application"
	@echo "  make rebuild - Clean and rebuild"
	@echo "  make clean   - Remove build artifacts"
	@echo ""

# Print variables for debugging
.PHONY: info
info:
	@echo "Project Configuration:"
	@echo "====================="
	@echo "Compiler: $(CXX)"
	@echo "Flags: $(CXXFLAGS)"
	@echo "Source directory: $(SRC_DIR)"
	@echo "Object directory: $(OBJ_DIR)"
	@echo "Database directory: $(DB_DIR)"
	@echo "Data directory: $(DATA_DIR)"
	@echo "Executable: $(EXEC)"
	@echo "Sources: $(SOURCES)"
	@echo ""

.PHONY: status
status:
	@echo "Build Status:"
	@echo "============="
	@if [ -f $(EXEC) ]; then \
		echo "✓ Executable exists: $(EXEC)"; \
	else \
		echo "✗ Executable not built"; \
	fi
	@if [ -d $(DB_DIR) ]; then \
		echo "✓ Database directory exists: $(DB_DIR)"; \
		ls -la $(DB_DIR) | tail -n +2 | sed 's/^/  /'; \
	else \
		echo "✗ Database directory missing"; \
	fi
	@if [ -d $(DATA_DIR) ]; then \
		echo "✓ Data directory exists: $(DATA_DIR)"; \
		ls -la $(DATA_DIR) | tail -n +2 | sed 's/^/  /'; \
	else \
		echo "✗ Data directory missing"; \
	fi
	@echo ""
