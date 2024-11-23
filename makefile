# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
LDFLAGS = -lssl -lcrypto -lcurl
INCLUDES = -Iinclude
DEBUG_CFLAGS = -g -DDEBUG

# Build Directories
BUILD_DIR = build
INSTALL_DIR = /opt/pmll
TEST_DIR = test_build
LOG_DIR = logs

# Source files
SOURCES = \
    unified_voice.c \
    pml_logic_loop.c \
    memory_silo.c \
    io_socket.c \
    persistence.c \
    cross_talk.c \
    custodian.c \
    main.c \
    vector_matrix.c \
    arc_agi_benchmark.c \
    pmll_np_solver.c \
    SAT_Compare.c \
    Pmll_NP_Solver.c \
    MiniSAT.c \
    generate_3sat_instance.c \
    SAT_Solver.c \
    API_Llama.c \
    API.c \
    silo_manager.c \
    logic_loop.c \
    free.c \
    json.c

# Object and Dependency files
OBJECTS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SOURCES))
DEPFILES = $(patsubst %.c, $(BUILD_DIR)/%.d, $(SOURCES))

# Targets
TARGETS = pmll arc_agi_benchmark pmll_np_solver sat_test api_llama api vector_matrix silo_manager logic_loop custodian free

# Default target: Build all
.PHONY: all
all: $(TARGETS)

# Define each binary and its dependencies
pmll: $(BUILD_DIR)/unified_voice.o $(BUILD_DIR)/pml_logic_loop.o $(BUILD_DIR)/memory_silo.o \
      $(BUILD_DIR)/io_socket.o $(BUILD_DIR)/persistence.o $(BUILD_DIR)/cross_talk.o \
      $(BUILD_DIR)/custodian.o $(BUILD_DIR)/main.o $(BUILD_DIR)/vector_matrix.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) -o $@ $^

arc_agi_benchmark: $(BUILD_DIR)/arc_agi_benchmark.o $(BUILD_DIR)/vector_matrix.o $(BUILD_DIR)/io_socket.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) -o $@ $^

pmll_np_solver: $(BUILD_DIR)/pmll_np_solver.o $(BUILD_DIR)/vector_matrix.o $(BUILD_DIR)/io_socket.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) -o $@ $^

sat_test: $(BUILD_DIR)/SAT_Compare.o $(BUILD_DIR)/Pmll_NP_Solver.o $(BUILD_DIR)/MiniSAT.o \
          $(BUILD_DIR)/generate_3sat_instance.o $(BUILD_DIR)/SAT_Solver.o $(BUILD_DIR)/vector_matrix.o \
          $(BUILD_DIR)/io_socket.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) -o $@ $^

api_llama: $(BUILD_DIR)/API_Llama.o $(BUILD_DIR)/vector_matrix.o $(BUILD_DIR)/io_socket.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) -o $@ $^

api: $(BUILD_DIR)/API.o $(BUILD_DIR)/vector_matrix.o $(BUILD_DIR)/io_socket.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) -o $@ $^

vector_matrix: $(BUILD_DIR)/vector_matrix.o $(BUILD_DIR)/io_socket.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) -o $@ $^

silo_manager: $(BUILD_DIR)/silo_manager.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) -o $@ $^

logic_loop: $(BUILD_DIR)/logic_loop.o $(BUILD_DIR)/pml_logic_loop.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) -o $@ $^

custodian: $(BUILD_DIR)/custodian.o $(BUILD_DIR)/io_socket.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) -o $@ $^

free: $(BUILD_DIR)/free.o $(BUILD_DIR)/json.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) -o $@ $^

# Compile Rules
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Dependency Generation
$(BUILD_DIR)/%.d: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) -M $(CFLAGS) $(INCLUDES) $< > $@

include $(wildcard $(DEPFILES))

# Orchestrate Health Check, Payload, and Binary Deployment
.PHONY: orchestrate
orchestrate: all
	@echo "Running orchestration..."
	bash Orchestrate.sh
	@echo "Orchestration completed."

# Test Build
.PHONY: test
test: clean
	@echo "Building test executables in $(TEST_DIR)..."
	@mkdir -p $(TEST_DIR)
	@$(MAKE) all CFLAGS="$(CFLAGS) -g -O0 -DTEST" LDFLAGS="$(LDFLAGS)" BUILD_DIR=$(TEST_DIR)

# Deployment
.PHONY: deploy
deploy: $(TARGETS)
	@sudo mkdir -p $(INSTALL_DIR)
	@sudo install -m 755 $(TARGETS) $(INSTALL_DIR)/
	@echo "Deployment complete to $(INSTALL_DIR)."

# Debug Build
.PHONY: debug
debug: CFLAGS += $(DEBUG_CFLAGS)
debug: all

# Clean Build Artifacts
.PHONY: clean
clean:
	@echo "Cleaning up..."
	-rm -f $(BUILD_DIR)/*.o $(TARGETS) $(DEPFILES) *.pid
	@echo "Clean complete."

# Help Target
.PHONY: help
help:
	@echo "Makefile commands:"
	@echo "  all                  Build all executables."
	@echo "  test                 Build test executables with debug flags."
	@echo "  orchestrate          Run orchestration script (Orchestrate.sh)."
	@echo "  deploy               Deploy binaries to $(INSTALL_DIR)."
	@echo "  debug                Build with debug flags."
	@echo "  clean                Remove all build artifacts."
	@echo "  help                 Show this help message."
