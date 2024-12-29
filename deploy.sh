#!/bin/bash
# File: Orchestrate_Deploy.sh
# Description: Integrates consent collection, health checks, payload distribution, binary deployment, and advanced PMLL/ARLL/EFLL logic.

# Determine the directory of this script
SCRIPT_DIR="${SCRIPT_DIR:-$(dirname "$0")}"

# Configuration
INSTALL_DIR="/opt/pmll"
DEPLOY_DIR="$SCRIPT_DIR/binaries"
LOG_DIR="$SCRIPT_DIR/logs"
SSH_OPTIONS="-o BatchMode=yes -o ConnectTimeout=5"
CONSENT_PAYLOAD='{
    "subject": "Request for Consent",
    "body": "We are deploying PMLL components. Do you consent to participate? Respond AGREE or DENY.",
    "action_required": true
}'
ARLL_RETRIES=3
ARLL_DELAY=5  # Retry delay in seconds

# Ensure directories exist before using them
mkdir -p "$LOG_DIR" "$DEPLOY_DIR"

# Logging utility
log() {
    echo "$(date '+%Y-%m-%d %H:%M:%S') - $1" | tee -a "$LOG_DIR/orchestra_deploy.log"
}

# PMLL (Payload Management Logic Loop)
run_pmll_logic() {
    local silo=$1
    log "Running PMLL logic for $silo..."
    ssh $SSH_OPTIONS $silo "$INSTALL_DIR/pmll_logic" &> "$LOG_DIR/pmll_$silo.log"
    if [ $? -eq 0 ]; then
        log "PMLL logic completed successfully for $silo."
        return 0
    else
        log "ERROR: PMLL logic failed for $silo. Check $LOG_DIR/pmll_$silo.log for details."
        return 1
    fi
}

# ARLL (Availability & Retry Logic Loop)
run_arll_logic() {
    local silo=$1
    local attempt=1
    while [ $attempt -le $ARLL_RETRIES ]; do
        log "Running ARLL logic for $silo (Attempt $attempt)..."
        ssh $SSH_OPTIONS $silo "$INSTALL_DIR/arll_logic" &> "$LOG_DIR/arll_$silo.log"
        if [ $? -eq 0 ]; then
            log "ARLL logic completed successfully for $silo."
            return 0
        else
            log "ARLL logic failed for $silo on attempt $attempt."
        fi
        sleep $ARLL_DELAY
        attempt=$((attempt + 1))
    done
    log "ERROR: ARLL logic failed for $silo after $ARLL_RETRIES attempts."
    return 1
}

# EFLL (Error & Failure Logic Loop)
flag_efll() {
    local silo=$1
    log "Flagging EFLL issue for $silo..."
    ssh $SSH_OPTIONS $silo "$INSTALL_DIR/efll_flag" &> "$LOG_DIR/efll_$silo.log"
    if [ $? -eq 0 ]; then
        log "EFLL issue flagged successfully for $silo."
        return 0
    else
        log "ERROR: Failed to flag EFLL issue for $silo. Check $LOG_DIR/efll_$silo.log for details."
        return 1
    fi
}

# Silos discovery
discover_silos() {
    log "Discovering silos..."
    SILOS=()
    for i in {1..300}; do
        SILOS+=("silo$i.example.com")
    done
    log "Discovered ${#SILOS[@]} silos."
}

# Consent requests
send_consent_request() {
    local silo=$1
    log "Sending consent request to $silo..."
    RESPONSE=$(curl -s -X POST "http://$silo/consent" \
        -H "Content-Type: application/json" \
        -d "$CONSENT_PAYLOAD" 2>/dev/null)

    if [ $? -eq 0 ]; then
        echo "[$silo] Response: $RESPONSE" >> "$LOG_DIR/consent_responses.log"
        if [[ "$RESPONSE" == "AGREE" ]]; then
            return 0
        else
            return 1
        fi
    else
        echo "[$silo] Failed to send request." >> "$LOG_DIR/consent_responses.log"
        return 1
    fi
}

# Health check
health_check() {
    local silo=$1
    local retries=3
    local delay=5
    local attempt=1
    while [ $attempt -le $retries ]; do
        RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" "https://$silo/health")
        if [ "$RESPONSE" == "200" ]; then
            return 0
        fi
        log "Health check failed for $silo (Attempt $attempt/$retries)"
        sleep $delay
        attempt=$((attempt + 1))
    done
    log "ERROR: Health check failed for $silo after $retries attempts."
    return 1
}

# Deploy to silo
deploy_to_silo() {
    local silo=$1
    log "Deploying to $silo..."

    # Ensure the installation directory exists
    ssh $SSH_OPTIONS $silo "mkdir -p $INSTALL_DIR && chmod -R 755 $INSTALL_DIR"
    if [ $? -ne 0 ]; then
        log "ERROR: Failed to create directory on $silo."
        return 1
    fi

    # Transfer binaries
    scp -q $DEPLOY_DIR/* $silo:$INSTALL_DIR/
    if [ $? -ne 0 ]; then
        log "ERROR: Failed to copy binaries to $silo."
        return 1
    fi

    # Start services
    ssh $SSH_OPTIONS $silo "cd $INSTALL_DIR && nohup ./silo_manager &> silo_manager.log & nohup ./api &> api.log & nohup ./logic_loop &> logic_loop.log &"
    if [ $? -ne 0 ]; then
        log "ERROR: Failed to start services on $silo."
        return 1
    fi

    log "Deployment successful on $silo."
    return 0
}

# Main orchestration flow
main() {
    discover_silos

    CONSENT_GRANTED_SILOS=()
    for silo in "${SILOS[@]}"; do
        send_consent_request $silo
        if [ $? -eq 0 ]; then
            CONSENT_GRANTED_SILOS+=("$silo")
        else
            log "Consent not received or failed for $silo. Skipping deployment."
        fi
    done

    for silo in "${CONSENT_GRANTED_SILOS[@]}"; do
        if health_check "$silo"; then
            deploy_to_silo $silo &&
            run_pmll_logic $silo &&
            run_arll_logic $silo || flag_efll $silo &
            sleep 0.1
        else
            log "Skipping $silo due to failed health check."
        fi
    done

    wait
    log "Deployment process completed. Check $LOG_DIR for logs."
}

# Execute the orchestration
main

