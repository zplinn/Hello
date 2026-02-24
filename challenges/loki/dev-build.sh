#!/bin/bash

# Development build script for fast iterative testing
# This script uses a persistent container with mounted source for quick rebuilds

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== Loki Development Build Script ===${NC}"

# Check if base image exists, build if not
if ! podman images | grep -q "loki-build-base"; then
    echo -e "${YELLOW}Base image not found. Building loki-build-base...${NC}"
    podman build -f challenge/Dockerfile.base -t loki-build-base challenge/
    echo -e "${GREEN}Base image built successfully${NC}"
else
    echo -e "${GREEN}Using existing loki-build-base image${NC}"
fi

# Check for existing dev container
if podman ps -a | grep -q "loki-dev"; then
    echo -e "${YELLOW}Found existing loki-dev container${NC}"
    read -p "Remove existing container? (y/n) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        podman stop loki-dev 2>/dev/null || true
        podman rm loki-dev 2>/dev/null || true
    else
        echo "Using existing container"
        CONTAINER_ID="loki-dev"
    fi
fi

# Start persistent container if not already running
if [ -z "$CONTAINER_ID" ] || [ "$CONTAINER_ID" != "loki-dev" ]; then
    echo -e "${YELLOW}Starting persistent development container...${NC}"
    podman run -d \
        --name loki-dev \
        -v "$SCRIPT_DIR/challenge/src:/src:Z" \
        -v "$SCRIPT_DIR/challenge/artifacts:/artifacts:Z" \
        -v "$SCRIPT_DIR/challenge/build:/build:Z" \
        loki-build-base \
        sleep infinity
    CONTAINER_ID="loki-dev"
    echo -e "${GREEN}Container started: $CONTAINER_ID${NC}"
fi

# Function to build binaries
build_target() {
    local target=$1
    local output_name=$2
    echo -e "${YELLOW}Building $target...${NC}"
    
    # Clean and build
    podman exec loki-dev bash -c "cd /src && make clean >/dev/null 2>&1"
    
    if podman exec loki-dev bash -c "cd /src && make $target"; then
        # Run post-build script if needed
        if [ "$target" == "all" ] || [ "$target" == "reverseme" ] || [ "$target" == "test" ]; then
            echo "Running post-build script..."
            podman exec loki-dev bash -c "cd /src && python3 update_checksum.py ${output_name:-reverseme}"
        fi
        
        # Copy artifacts
        if [ "$target" == "all" ]; then
            podman exec loki-dev bash -c "cp /src/reverseme /src/challenge /artifacts/ 2>/dev/null"
            echo -e "${GREEN}Production binaries copied to artifacts/${NC}"
        elif [ "$target" == "test" ]; then
            podman exec loki-dev bash -c "cp /src/reverseme-test /artifacts/ 2>/dev/null"
            echo -e "${GREEN}Test binary copied to artifacts/${NC}"
        elif [ "$target" == "reverseme" ]; then
            podman exec loki-dev bash -c "cp /src/reverseme /artifacts/ 2>/dev/null"
            echo -e "${GREEN}Reverseme binary copied to artifacts/${NC}"
        elif [ "$target" == "challenge" ]; then
            podman exec loki-dev bash -c "cp /src/challenge /artifacts/ 2>/dev/null"
            echo -e "${GREEN}Challenge binary copied to artifacts/${NC}"
        fi
    else
        echo -e "${RED}Build failed for $target${NC}"
        return 1
    fi
}

# Main menu
while true; do
    echo
    echo -e "${GREEN}=== Build Options ===${NC}"
    echo "1) Build production (all targets)"
    echo "2) Build test version (with TEST_FLAG)"
    echo "3) Build reverseme only"
    echo "4) Build challenge only"
    echo "5) Run test binary"
    echo "6) Enter container shell"
    echo "7) Clean all build artifacts"
    echo "8) Rebuild base image"
    echo "9) Exit (keep container running)"
    echo "0) Exit and stop container"
    
    read -p "Select option: " option
    
    case $option in
        1)
            build_target "all"
            ;;
        2)
            build_target "test" "reverseme-test"
            ;;
        3)
            build_target "reverseme"
            ;;
        4)
            build_target "challenge"
            ;;
        5)
            if [ -f "challenge/artifacts/reverseme-test" ]; then
                echo -e "${YELLOW}Running test binary...${NC}"
                podman exec loki-dev /artifacts/reverseme-test
            else
                echo -e "${RED}Test binary not found. Build it first (option 2)${NC}"
            fi
            ;;
        6)
            echo -e "${YELLOW}Entering container shell. Type 'exit' to return.${NC}"
            podman exec -it loki-dev bash
            ;;
        7)
            echo -e "${YELLOW}Cleaning build artifacts...${NC}"
            podman exec loki-dev bash -c "cd /src && make clean"
            podman exec loki-dev bash -c "rm -f /artifacts/*"
            echo -e "${GREEN}Cleaned${NC}"
            ;;
        8)
            echo -e "${YELLOW}Rebuilding base image...${NC}"
            podman build --no-cache -f challenge/Dockerfile.base -t loki-build-base challenge/
            echo -e "${RED}Container needs to be restarted to use new base image${NC}"
            ;;
        9)
            echo -e "${GREEN}Exiting. Container $CONTAINER_ID is still running.${NC}"
            echo "To stop it later: podman stop loki-dev && podman rm loki-dev"
            exit 0
            ;;
        0)
            echo -e "${YELLOW}Stopping container...${NC}"
            podman stop loki-dev
            podman rm loki-dev
            echo -e "${GREEN}Container stopped and removed${NC}"
            exit 0
            ;;
        *)
            echo -e "${RED}Invalid option${NC}"
            ;;
    esac
done