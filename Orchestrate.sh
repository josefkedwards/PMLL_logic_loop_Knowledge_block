#!/bin/bash

# Define component configurations
VECTOR_MATRIX="./vector_matrix"
MEMORY_SILO="./memory_silo"
KNOWLEDGE="./knowledge"
IO_SOCKET="./io_socket"
PML_LOGIC_LOOP="./pml_logic_loop"
UNIFIED_VOICE="./unified_voice"
CROSS_TALK="./cross_talk"
PERSISTENCE="./persistence"
FREE="./free"
LOG_DIR="./logs"
PORT_BASE=8080

# Log files for orchestration tasks
CONSENT_LOG_FILE="$LOG_DIR/consent_responses.log"
HEALTH_LOG_FILE="$LOG_DIR/health_checks.log"
SYNC_LOG_FILE="$LOG_DIR/data_sync.log"
BUILD_LOG_FILE="$LOG_DIR/build.log"

# Ensure logs directory exists with proper permissions
if [ ! -d "$LOG_DIR" ]; then
    mkdir -p $LOG_DIR || { echo "ERROR: Failed to create logs directory."; exit 1; }
    chmod 755 $LOG_DIR
fi

# Logging utility
log() {
    echo "$(date '+%Y-%m-%d %H:%M:%S') - $1" | tee -a "$LOG_DIR/orchestra.log"
}

# Compile a component
compile_component() {
    local component=$1
    local source_files=$2

    log "Compiling $component..."
    gcc -o "$component" $source_files -lcurl &>> "$BUILD_LOG_FILE"
    if [ $? -ne 0 ]; then
        log "ERROR: Compilation failed for $component. Check $BUILD_LOG_FILE for details."
        exit 1
    fi
    log "Compilation successful for $component."
}

# Validate environment
validate_environment() {
    local missing_components=()
    for component in "$IO_SOCKET" "$CROSS_TALK" "$PML_LOGIC_LOOP" "$FREE"; do
        if [ ! -f "$component" ]; then
            missing_components+=("$component")
        fi
    done

    if [ ${#missing_components[@]} -ne 0 ]; then
        log "ERROR: Missing executables: ${missing_components[*]}. Ensure they are compiled before running."
        exit 1
    fi
}

# Execute free.c tasks
execute_free() {
    log "Executing free.c tasks..."
    $FREE > "$LOG_DIR/free_log.txt" 2>&1
    if [ $? -ne 0 ]; then
        log "ERROR: free.c execution failed. Check free_log.txt for details."
    else
        log "free.c execution completed successfully."
    fi
}

# Start a component
start_component() {
    local component=$1
    local port=$2

    if [ "$component" == "$CROSS_TALK" ]; then
        log "Starting $component with direct execution..."
        $component > "$LOG_DIR/${component}_log.txt" 2>&1 &
    else
        log "Starting $component on port $port..."
        $component $port > "$LOG_DIR/${component}_log.txt" 2>&1 &
    fi

    local pid=$!
    echo $pid
}

# Check if a component is running
check_pid() {
    local pid=$1
    if kill -0 $pid 2>/dev/null; then
        return 0
    else
        return 1
    fi
}

# Kill all running components
cleanup() {
    log "Cleaning up all running processes..."
    for pid in "${PIDS[@]}"; do
        if check_pid $pid; then
            log "Stopping process $pid..."
            kill $pid
        fi
    done
    log "Cleanup complete."
}

# Trap exit signal to ensure cleanup
trap cleanup EXIT

# Main orchestration logic
log "Starting orchestration..."

# Compile all components
compile_component "$IO_SOCKET" "io_socket.c"
compile_component "$CROSS_TALK" "cross_talk.c io_socket.c"
compile_component "$PML_LOGIC_LOOP" "logic_loop.c io_socket.c memory_silo.c"
compile_component "$FREE" "free.c json.c"

# Validate environment
validate_environment

# Start components
PIDS=()
COMPONENTS=($VECTOR_MATRIX $MEMORY_SILO $KNOWLEDGE $IO_SOCKET $PML_LOGIC_LOOP $UNIFIED_VOICE $CROSS_TALK $PERSISTENCE)
PORT=$PORT_BASE

for component in "${COMPONENTS[@]}"; do
    if [ -f "$component" ]; then
        pid=$(start_component $component $PORT)
        PIDS+=($pid)
        log "$component started with PID $pid on port $PORT"
        PORT=$((PORT + 1))
    else
        log "Skipping $component - executable not found."
    fi
done

# Monitor components and silos
log "All components started successfully. Monitoring processes and silos..."

while true; do
    sleep 5
    for i in "${!PIDS[@]}"; do
        if ! check_pid "${PIDS[$i]}"; then
            log "Process ${PIDS[$i]} (Component: ${COMPONENTS[$i]}) has stopped unexpectedly."
            cleanup
            exit 1
        fi
    done

    # Execute periodic free.c tasks
    execute_free
done
