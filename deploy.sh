#!/bin/bash

SILOS=("silo1.example.com" "silo2.example.com" "silo3.example.com" ... "silo300.example.com")
INSTALL_DIR="/opt/pmll"
DEPLOY_DIR="path/to/binaries"

deploy_to_silo() {
    local silo=$1
    echo "Deploying to $silo..."
    
    # Copy binaries to the silo
    scp $DEPLOY_DIR/* $silo:$INSTALL_DIR/ || { echo "Failed to copy to $silo"; return 1; }
    
    # Start services on the silo
    ssh $silo "cd $INSTALL_DIR && ./silo_manager & ./api & ./logic_loop &" || { echo "Failed to start services on $silo"; return 1; }
    
    echo "Deployment successful on $silo"
}

for silo in "${SILOS[@]}"; do
    deploy_to_silo $silo &
done

# Wait for all deployments to complete
wait
echo "Deployment to all silos completed!"
