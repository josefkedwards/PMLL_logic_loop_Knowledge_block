# Compiler and flags
CC = gcc
NVCC = nvcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -O2 -Iinclude
CUDAFLAGS = -arch=sm_60 -O2
LDFLAGS = -lssl -lcrypto -lcurl -lpthread
DEBUG_CFLAGS = -g -DDEBUG

# Directories
SRC_DIR = ./src
BUILD_DIR = ./build
INCLUDE_DIR = ./include
BINARIES_DIR = ./binaries
LOG_DIR = ./logs
INSTALL_DIR ?= /opt/pmll  # Allow overriding the install directory

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
CUDA_SOURCES = $(wildcard $(SRC_DIR)/*.cu)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES)) \
          $(patsubst $(SRC_DIR)/%.cu, $(BUILD_DIR)/%.o, $(CUDA_SOURCES))

# Target binaries
TARGETS = pmll arc_agi_benchmark pmll_np_solver sat_test api_llama api \
          vector_matrix silo_manager logic_loop custodian free gpu_program bugwatcher

# Versioning
VERSION = 1.0.0
LDFLAGS += -DVERSION=\"$(VERSION)\"

# Default target: Build all binaries
.PHONY: all
all: prepare_dirs $(TARGETS)

# Parallel build (using 4 jobs)
.PHONY: parallel
parallel:
	$(MAKE) -j4 all

# --- Rules for each binary ---
pmll: $(BUILD_DIR)/unified_voice.o $(BUILD_DIR)/pml_logic_loop.o $(BUILD_DIR)/memory_silo.o \
      $(BUILD_DIR)/io_socket.o $(BUILD_DIR)/persistence.o $(BUILD_DIR)/cross_talk.o \
      $(BUILD_DIR)/custodian.o $(BUILD_DIR)/main.o $(BUILD_DIR)/vector_matrix.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINARIES_DIR)/$@ $^

arc_agi_benchmark: $(BUILD_DIR)/arc_agi_benchmark.o $(BUILD_DIR)/vector_matrix.o $(BUILD_DIR)/io_socket.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINARIES_DIR)/$@ $^

pmll_np_solver: $(BUILD_DIR)/pmll_np_solver.o $(BUILD_DIR)/vector_matrix.o $(BUILD_DIR)/io_socket.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINARIES_DIR)/$@ $^

sat_test: $(BUILD_DIR)/SAT_Compare.o $(BUILD_DIR)/Pmll_NP_Solver.o $(BUILD_DIR)/MiniSAT.o \
          $(BUILD_DIR)/generate_3sat_instance.o $(BUILD_DIR)/SAT_Solver.o $(BUILD_DIR)/vector_matrix.o \
          $(BUILD_DIR)/io_socket.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINARIES_DIR)/$@ $^

api_llama: $(BUILD_DIR)/API_Llama.o $(BUILD_DIR)/vector_matrix.o $(BUILD_DIR)/io_socket.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINARIES_DIR)/$@ $^

api: $(BUILD_DIR)/API.o $(BUILD_DIR)/vector_matrix.o $(BUILD_DIR)/io_socket.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINARIES_DIR)/$@ $^

vector_matrix: $(BUILD_DIR)/vector_matrix.o $(BUILD_DIR)/io_socket.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINARIES_DIR)/$@ $^

silo_manager: $(BUILD_DIR)/silo_manager.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINARIES_DIR)/$@ $^

logic_loop: $(BUILD_DIR)/logic_loop.o $(BUILD_DIR)/pml_logic_loop.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINARIES_DIR)/$@ $^

custodian: $(BUILD_DIR)/custodian.o $(BUILD_DIR)/io_socket.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINARIES_DIR)/$@ $^

free: $(BUILD_DIR)/free.o $(BUILD_DIR)/json.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINARIES_DIR)/$@ $^

gpu_program: $(BUILD_DIR)/GPU.o
	$(NVCC) $(CUDAFLAGS) -I$(INCLUDE_DIR) -o $(BINARIES_DIR)/$@ $^

# BugWatcher (Ensure BugWatcher.c exists in src/)
bugwatcher: $(BUILD_DIR)/BugWatcher.o 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINARIES_DIR)/$@ $^


# --- Compilation rules ---
# Compile C source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Compile CUDA source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cu
	@mkdir -p $(BUILD_DIR)
	$(NVCC) $(CUDAFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# --- Utility targets ---
# Prepare necessary directories
.PHONY: prepare_dirs
prepare_dirs:
	@mkdir -p $(BUILD_DIR) $(BINARIES_DIR) $(LOG_DIR)

# Clean build artifacts
.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR) $(BINARIES_DIR) $(LOG_DIR)
	@echo "Clean complete."

# Orchestrate Health Check, Payload, and Binary Deployment
.PHONY: orchestrate
orchestrate: all
	@echo "Running orchestration..."
	bash Orchestrate.sh > $(LOG_DIR)/orchestration.log 2>&1
	@echo "Orchestration completed."

# Deployment to system directory
.PHONY: deploy
deploy: all
	@echo "Deploying binaries to $(INSTALL_DIR)..."
	@sudo mkdir -p $(INSTALL_DIR) || { echo "Failed to create $(INSTALL_DIR). Check permissions."; exit 1; }
	@sudo install -m 755 $(BINARIES_DIR)/* $(INSTALL_DIR)/ || { echo "Failed to install binaries. Check permissions."; exit 1; }
	@echo "Deployment complete."

# Debug build
.PHONY: debug
debug: CFLAGS += $(DEBUG_CFLAGS)
debug: CUDAFLAGS += -G -lineinfo
debug: all

# Static analysis with clang-tidy
.PHONY: static_analysis
static_analysis:
	@echo "Running static analysis..."
	clang-tidy $(SOURCES) -- $(CFLAGS) -I$(INCLUDE_DIR) || { echo "Static analysis found issues."; exit 1; }
	@echo "Static analysis completed."

# Install Dependencies
.PHONY: deps
deps:
	@echo "Installing necessary dependencies..."
	sudo apt-get update && sudo apt-get install -y libssl-dev libcrypto++-dev libboost-all-dev curl || { echo "Failed to install dependencies."; exit 1; }
	@echo "Dependencies installed successfully."

# Run Tests (assuming run_tests.c exists)
.PHONY: test
test: all
	@echo "Running tests..."
	$(CC) $(CFLAGS) $(SRC_DIR)/run_tests.c -o $(BINARIES_DIR)/run_tests $(LDFLAGS)
	$(BINARIES_DIR)/run_tests || { echo "Tests failed."; exit 1; }
	@echo "Tests completed."

# Help
.PHONY: help
help:
	@echo "Makefile commands:"
	@echo "  all                  - Build all binaries."
	@echo "  clean                - Remove all build artifacts."
	@echo "  orchestrate          - Run orchestration script (Orchestrate.sh)."
	@echo "  deploy               - Deploy binaries to $(INSTALL_DIR)."
	@echo "  debug                - Build with debugging enabled."
	@echo "  deps                 - Install necessary dependencies."
	@echo "  test                 - Run automated tests."
	@echo "  parallel             - Build in parallel."
	@echo "  static_analysis      - Run static code analysis."
