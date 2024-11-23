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

# Log files for orchestration tasks
CONSENT_LOG_FILE="$LOG_DIR/consent_responses.log"
HEALTH_LOG_FILE="$LOG_DIR/health_checks.log"
SYNC_LOG_FILE="$LOG_DIR/data_sync.log"

# Ensure logs directory exists
mkdir -p $LOG_DIR

# Logging utility
log() {
    echo "$(date '+%Y-%m-%d %H:%M:%S') - $1" | tee -a "$LOG_DIR/orchestra.log"
}

# Compile cross_talk executable if not found
compile_cross_talk() {
    if [ ! -f "$CROSS_TALK" ]; then
        log "cross_talk executable not found. Attempting to compile..."
        gcc -o cross_talk cross_talk.c -lcurl
        if [ $? -eq 0 ]; then
            log "cross_talk compiled successfully."
        else
            log "ERROR: Failed to compile cross_talk."
            exit 1
        fi
    fi
}

# Start a component
start_component() {
    local component=$1
    local port=$2

    if [ "$component" == "$CROSS_TALK" ]; then
        log "Starting $component with custom input..."
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

# Dynamic silo discovery
discover_endpoints() {
    log "Discovering internal and external silos..."

    # Internal silos
    for i in {1..7500}; do
        INTERNAL_SILOS+=("https://silo$i.internal")
    done

    # External silos
    for i in {1..144000}; do
        EXTERNAL_SILOS+=("https://silo$i.external")
    done

    # Log discovered silos
    log "Discovered ${#INTERNAL_SILOS[@]} internal silos."
    log "Discovered ${#EXTERNAL_SILOS[@]} external silos."
}

# Consent request
send_consent_request() {
    local SILO="$1"
    local LOG_FILE="$CONSENT_LOG_FILE"

    log "Sending consent request to $SILO..."
    RESPONSE=$(curl -s -X POST "$SILO/consent" \
        -H "Content-Type: application/json" \
        -d '{
            "subject": "Request for Consent",
            "body": "Do you agree to participate? Respond AGREE or DENY.",
            "action_required": true
        }')

    if [ $? -eq 0 ]; then
        echo "[$SILO] Response: $RESPONSE" >> "$LOG_FILE"
    else
        echo "[$SILO] Failed to send request." >> "$LOG_FILE"
    fi
}

# Initiate cross-talk logic
initiate_cross_talk() {
    local llama_input="Hello, LLaMA!"
    log "Initiating cross-talk process..."
    RESPONSE=$($CROSS_TALK "$llama_input")
    if [ $? -eq 0 ]; then
        log "Cross-talk completed successfully. Response: $RESPONSE"
    else
        log "Cross-talk failed. Check cross_talk.log for details."
    fi
}

# Main orchestration logic
log "Starting orchestration..."

# Compile cross_talk if necessary
compile_cross_talk

PIDS=()
COMPONENTS=($VECTOR_MATRIX $MEMORY_SILO $KNOWLEDGE $IO_SOCKET $PML_LOGIC_LOOP $UNIFIED_VOICE $CROSS_TALK $PERSISTENCE)
PORT=$PORT_BASE

for component in "${COMPONENTS[@]}"; do
    pid=$(start_component $component $PORT)
    PIDS+=($pid)
    log "$component started with PID $pid on port $PORT"
    PORT=$((PORT + 1))
done

# Discover silos
INTERNAL_SILOS=()
EXTERNAL_SILOS=()
discover_endpoints

# Send consent requests to all silos
log "Sending consent requests to silos..."
for SILO in "${INTERNAL_SILOS[@]}"; do
    send_consent_request "$SILO" &
done
for SILO in "${EXTERNAL_SILOS[@]}"; do
    send_consent_request "$SILO" &
done
wait
log "Consent requests completed."

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

    # Trigger periodic cross-talk
    initiate_cross_talk
done
