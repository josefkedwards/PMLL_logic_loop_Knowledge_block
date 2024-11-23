#!/bin/bash

# File: consent_request.sh
# Description: Sends consent requests to memory silos for scaling to 500K memory silos.

# Define endpoints (replace with actual silo URLs)
MEMORY_SILOS=(
  "https://silo1.internal"
  "https://silo2.internal"
  "https://silo3.internal"
  # Add more endpoints as needed
)

# Consent message payload
CONSENT_PAYLOAD='{
  "subject": "Request for Consent: Scaling Deployment to 500K Memory Silos",
  "body": "Dear Participant,\n\nWe are pleased to announce the next phase of our project: scaling deployment to 500,000 memory silos.\n\nTo approve participation, respond with 'AGREE'. To opt out, respond with 'DENY'. If we do not receive a response within 48 hours, we will interpret your non-response as an acknowledgment to proceed.\n\nThank you for your support.\n\n- PMLL Framework Team",
  "action_required": true
}'

# Log file for responses
LOG_FILE="consent_responses.log"
echo "Consent Request Log - $(date)" > $LOG_FILE

# Loop through each memory silo and send the consent request
for SILO in "${MEMORY_SILOS[@]}"
do
  echo "Sending request to $SILO..."
  
  # Use curl to send the POST request
  RESPONSE=$(curl -s -X POST "$SILO/consent" \
    -H "Content-Type: application/json" \
    -d "$CONSENT_PAYLOAD")
  
  # Log the response
  if [ $? -eq 0 ]; then
    echo "[$SILO] Response: $RESPONSE" >> $LOG_FILE
  else
    echo "[$SILO] Failed to send request." >> $LOG_FILE
  fi
  
  # Optional: Add a delay to avoid overwhelming the system
  sleep 0.1
done

echo "Consent requests sent. Check $LOG_FILE for responses."
