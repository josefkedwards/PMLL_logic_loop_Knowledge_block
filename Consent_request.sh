#!/bin/bash
# File: consent_request.sh
# Description: Sends consent requests to internal and external memory silos for scaling to 500K memory silos.

# Consent message payload
CONSENT_PAYLOAD='{
  "subject": "Request for Consent: Scaling Deployment to 500K Memory Silos",
  "body": "Dear Participant,\\n\\nWe are pleased to announce the next phase of our project: scaling deployment to 500,000 memory silos.\\n\\nTo approve participation, respond with \\'AGREE\\'. To opt out, respond with \\'DENY\\'. If we do not receive a response within 48 hours, we will interpret your non-response as an acknowledgment to proceed.\\n\\nThank you for your support.\\n\\n- PMLL Framework Team",
  "action_required": true
}'

# Log files for responses
INTERNAL_LOG_FILE="./logs/internal_consent_responses.log"
EXTERNAL_LOG_FILE="./logs/external_consent_responses.log"

# Initialize log files
echo "Consent Request Log - $(date)" > "$INTERNAL_LOG_FILE"
echo "Consent Request Log - $(date)" > "$EXTERNAL_LOG_FILE"

# Function to dynamically discover endpoints
discover_endpoints() {
  # Example 1: Generate predictable endpoints
  for i in {1..7500}; do
    INTERNAL_SILOS+=("https://silo$i.internal")
  done

  for i in {1..144000}; do
    EXTERNAL_SILOS+=("https://silo$i.external")
  done

  # Print discovered endpoints for debugging
  echo "Discovered Internal Silos:"
  printf "%s\n" "${INTERNAL_SILOS[@]}" > ./logs/internal_endpoints.log
  echo "Discovered External Silos:"
  printf "%s\n" "${EXTERNAL_SILOS[@]}" > ./logs/external_endpoints.log
}

# Function to send consent requests
send_request() {
  local SILO="$1"
  local LOG_FILE="$2"

  echo "Sending request to $SILO..."

  # Use curl to send the POST request
  RESPONSE=$(curl -s -X POST "$SILO/consent" \
    -H "Content-Type: application/json" \
    -d "$CONSENT_PAYLOAD")
  
  # Log the response
  if [ $? -eq 0 ]; then
    echo "[$SILO] Response: $RESPONSE" >> "$LOG_FILE"
  else
    echo "[$SILO] Failed to send request." >> "$LOG_FILE"
  fi

  # Optional: Add a delay to avoid overwhelming the system
  sleep 0.1
}

# Discover endpoints dynamically
INTERNAL_SILOS=()
EXTERNAL_SILOS=()
discover_endpoints

# Loop through internal silos and send consent requests
for SILO in "${INTERNAL_SILOS[@]}"; do
  send_request "$SILO" "$INTERNAL_LOG_FILE" &
done

# Loop through external silos and send consent requests
for SILO in "${EXTERNAL_SILOS[@]}"; do
  send_request "$SILO" "$EXTERNAL_LOG_FILE" &
done

# Wait for all background jobs to finish
wait

echo "Consent requests sent. Check $INTERNAL_LOG_FILE and $EXTERNAL_LOG_FILE for responses."

