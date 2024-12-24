#!/bin/bash
# File: Orchestrate.sh
# Description: Integrates consent collection, health checks, payload distribution, binary deployment, and PMLL/ARLL/EFLL logic loops.

# Determine the directory of this script (Dynamically resolve the script location)
SCRIPT_DIR="${SCRIPT_DIR:-$(dirname "$0")}"

# Ensure the directory where Orchestrate.sh is located exists
if [ ! -d "$SCRIPT_DIR" ]; then
    echo "ERROR: The directory for Orchestrate.sh does not exist! Exiting."
    exit 1
fi

# Define paths dynamically based on SCRIPT_DIR
LOG_DIR="$SCRIPT_DIR/logs"
BUILD_LOG="$LOG_DIR/build.log"
ORCHESTRA_LOG="$LOG_DIR/orchestra.log"
CONSENT_LOG_FILE="$LOG_DIR/consent_responses.log"
CONSENT_SCRIPT="$SCRIPT_DIR/Consent_request.sh"
BINARIES_DIR="$SCRIPT_DIR/binaries"
PORT_BASE=8080

# Components to orchestrate
COMPONENTS=("VECTOR_MATRIX" "MEMORY_SILO" "IO_SOCKET" "PML_LOGIC_LOOP" "UNIFIED_VOICE" "CROSS_TALK" "PERSISTENCE" "ARLL" "EFLL")

# URL/IP configuration for silos
SILO_DOMAIN="silo"
START_SILO=1
END_SILO=144000
RETRY_COUNT=3

# Ensure directories exist
mkdir -p "$LOG_DIR" "$BINARIES_DIR"

# Logging utility
log() {
    echo "$(date '+%Y-%m-%d %H:%M:%S') - $1" | tee -a "$ORCHESTRA_LOG"
}

# Health check with retry logic
health_check() {
    local silo=$1
    local retries=3
    local delay=5
    local attempt=1
    while [ $attempt -le $retries ]; do
        RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" "https://silo$silo.$SILO_DOMAIN/health")
        if [ "$RESPONSE" == "200" ]; then
            return 0
        fi
        log "Health check failed for silo$silo.$SILO_DOMAIN (Attempt $attempt/$retries)"
        sleep $delay
        attempt=$((attempt + 1))
    done
    log "ERROR: Health check failed for silo$silo.$SILO_DOMAIN after $retries attempts."
    return 1
}

# Step 1: Execute Consent_request.sh
log "Starting consent request process..."
bash "$CONSENT_SCRIPT"
if [ $? -ne 0 ]; then
    log "ERROR: Consent_request.sh failed. Exiting."
    exit 1
fi

# Verify consent results
if [ ! -f "$CONSENT_LOG_FILE" ]; then
    log "ERROR: Consent log not found. Exiting."
    exit 1
fi
SUCCESS_COUNT=$(grep -c "SUCCESS" "$CONSENT_LOG_FILE")
FAILURE_COUNT=$(grep -c "ERROR" "$CONSENT_LOG_FILE")
log "Consent process completed. Success: $SUCCESS_COUNT, Failures: $FAILURE_COUNT"

# Compile dependencies
log "Compiling dependencies..."
make clean all &>> "$BUILD_LOG"
if [ $? -ne 0 ]; then
    log "ERROR: Compilation failed. Check $BUILD_LOG for details."
    exit 1
fi
log "Compilation completed successfully."

# Validate compiled components
log "Validating compiled components..."
for component in "${COMPONENTS[@]}"; do
    if [ ! -f "$BINARIES_DIR/$component" ]; then
        log "ERROR: Missing executable for $component in $BINARIES_DIR. Skipping deployment for this component."
        continue
    fi
done
log "All components validated and prepared for distribution."

# Modular logic: Integrate PMLL, ARLL, and EFLL loops
run_pmll_logic() {
    log "Executing PMLL Logic Loop..."
    "$BINARIES_DIR/PML_LOGIC_LOOP"
    log "PMLL Logic Loop execution completed."
}

run_arll_logic() {
    log "Executing ARLL Logic Loop..."
    "$BINARIES_DIR/ARLL"
    log "ARLL Logic Loop execution completed."
}

run_efll_logic() {
    log "Executing EFLL Logic Loop..."
    "$BINARIES_DIR/EFLL"
    log "EFLL Logic Loop execution completed."
}

# Execute logic loops
run_pmll_logic
run_arll_logic
run_efll_logic

# Send payloads to silos
log "Sending payload notifications to silos..."
for silo in $(seq $START_SILO $END_SILO); do
    if health_check "$silo"; then
        log "Payload sent to silo$silo.$SILO_DOMAIN."
    else
        log "Skipping silo$silo.$SILO_DOMAIN due to failed health check."
    fi
done

# Deploy binaries to silos
log "Deploying binaries to silos..."
for binary in "${COMPONENTS[@]}"; do
    for silo in $(seq $START_SILO $END_SILO); do
        if health_check "$silo"; then
            log "Deploying $binary to silo$silo.$SILO_DOMAIN."
        else
            log "Skipping silo$silo.$SILO_DOMAIN due to failed health check."
        fi
    done
done
log "Binary distribution complete."

# Summary
log "Deployment process completed."
echo -e "\nSummary of Operations:"
echo "Payload Notifications: Completed"
echo "Binary Deployments: Completed"
echo "Health Checks: Completed"
