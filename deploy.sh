#!/bin/bash

# Configuration
INSTALL_DIR="/opt/pmll"
DEPLOY_DIR="path/to/binaries"
LOG_DIR="./logs"
CONSENT_PAYLOAD='{
    "subject": "Request for Consent",
    "body": "We are deploying PMLL components. Do you consent to participate? Respond AGREE or DENY.",
    "action_required": true
}'

# Ensure logs directory exists
mkdir -p $LOG_DIR

# Silos discovery
discover_silos() {
    echo "Discovering silos..."
    # Replace with dynamic discovery logic
    SILOS=()
    for i in {1..300}; do
        SILOS+=("silo$i.example.com")
    done
    echo "Discovered ${#SILOS[@]} silos."
}

# Function to Send Consent Request
send_consent_request() {
    local silo=$1
    echo "Sending consent request to $silo..."
    RESPONSE=$(curl -s -X POST "http://$silo/consent" \
        -H "Content-Type: application/json" \
        -d "$CONSENT_PAYLOAD" 2>/dev/null)

    if [ $? -eq 0 ]; then
        echo "[$silo] Response: $RESPONSE" >> "$LOG_DIR/consent_responses.log"
    else
        echo "[$silo] Failed to send request." >> "$LOG_DIR/consent_responses.log"
    fi
}

# Function to Deploy to a Single Silo
deploy_to_silo() {
    local silo=$1
    echo "Deploying to $silo..."
    
    # Ensure the installation directory exists on the remote silo
    ssh $silo "mkdir -p $INSTALL_DIR" || { echo "Failed to create directory on $silo"; return 1; }
    
    # Copy binaries to the remote silo
    scp $DEPLOY_DIR/* $silo:$INSTALL_DIR/ || { echo "Failed to copy to $silo"; return 1; }
    
    # Start services on the remote silo
    ssh $silo "cd $INSTALL_DIR && nohup ./silo_manager &> silo_manager.log & nohup ./api &> api.log & nohup ./logic_loop &> logic_loop.log &" || { echo "Failed to start services on $silo"; return 1; }
    
    echo "Deployment successful on $silo"
}

# Discover Silos
discover_silos

# Consent and Deployment
for silo in "${SILOS[@]}"; do
    # Send consent request
    send_consent_request $silo &

    # Deploy if consent is granted
    if grep -q "AGREE" "$LOG_DIR/consent_responses.log"; then
        deploy_to_silo $silo &
    else
        echo "Consent not received for $silo. Skipping deployment."
    fi

    # Wait a moment before the next iteration to avoid overloading
    sleep 0.1
done

wait

echo "Deployment process completed. Check $LOG_DIR for logs."

