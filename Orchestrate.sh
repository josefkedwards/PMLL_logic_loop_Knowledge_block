#!/bin/bash

# Define configurations
VECTOR_MATRIX="./vector_matrix"
MEMORY_SILO="./memory_silo"
KNOWLEDGE="./knowledge"
IO_SOCKET="./io_socket"
PML_LOGIC_LOOP="./pml_logic_loop"
UNIFIED_VOICE="./unified_voice"
CROSS_TALK="./cross_talk"
PERSISTENCE="./persistence"
LOG_DIR="./logs"
PORT_BASE=8080

# Ensure logs directory exists
mkdir -p $LOG_DIR

# Logging utility
log() {
    echo "$(date '+%Y-%m-%d %H:%M:%S') - $1" | tee -a "$LOG_DIR/orchestra.log"
}

# Start a component
start_component() {
    local component=$1
    local port=$2

    log "Starting $component on port $port..."
    $component $port > "$LOG_DIR/${component}_log.txt" 2>&1 &
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

# Start all components
PIDS=()
COMPONENTS=($VECTOR_MATRIX $MEMORY_SILO $KNOWLEDGE $IO_SOCKET $PML_LOGIC_LOOP $UNIFIED_VOICE $CROSS_TALK $PERSISTENCE)
PORT=$PORT_BASE

for component in "${COMPONENTS[@]}"; do
    pid=$(start_component $component $PORT)
    PIDS+=($pid)
    log "$component started with PID $pid on port $PORT"
    PORT=$((PORT + 1))
done

# Monitor the components
log "All components started successfully. Monitoring processes..."

while true; do
    sleep 5
    for i in "${!PIDS[@]}"; do
        if ! check_pid "${PIDS[$i]}"; then
            log "Process ${PIDS[$i]} (Component: ${COMPONENTS[$i]}) has stopped unexpectedly."
            cleanup
            exit 1
        fi
    done
done

