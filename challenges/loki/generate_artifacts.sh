#!/bin/bash

# generate_artifacts.sh - Build and extract artifacts for Loki challenge
# This script ensures clean builds with no caching for both debug and production versions

set -e  # Exit on any error

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Set up all path variables relative to script location
CHALLENGE_DIR="$SCRIPT_DIR/challenge"
ARTIFACTS_DIR="$CHALLENGE_DIR/artifacts"
SRC_DIR="$CHALLENGE_DIR/src"
LOG_FILE="$CHALLENGE_DIR/loki.log"

echo "=== Loki Artifact Generation Script ==="
echo "Working directory: $SCRIPT_DIR"
echo "Challenge directory: $CHALLENGE_DIR"
echo "Artifacts directory: $ARTIFACTS_DIR"
echo ""
echo "Cleaning up existing handouts and artifacts..."

# Remove all existing handouts to prevent caching issues
echo "Removing existing handouts..."
podman images --format "table {{.Repository}} {{.Tag}} {{.ID}}" | grep "loki-handout" | awk '{print $3}' | xargs -r podman rmi -f >/dev/null 2>&1 || true

# Clean artifacts directory
echo "Cleaning artifacts directory..."
rm -rf "$ARTIFACTS_DIR"/* 2>/dev/null || true

# echo ""
# echo "=== Building DEBUG version ==="
# echo "Building debug container with --no-cache..."
# cd "$CHALLENGE_DIR"
# echo "  (filtering output, only showing errors...)"
# if ! podman build -f Dockerfile.debug --no-cache -t loki-handout-debug . >/dev/null 2>&1; then
#     echo "Debug build failed, showing full output:"
#     podman build -f Dockerfile.debug --no-cache -t loki-handout-debug-retry . 2>&1
#     exit 1
# fi
# echo "  Debug build completed successfully"
# 
# echo "Extracting debug artifacts..."
# mkdir -p "$ARTIFACTS_DIR"
# if ! CONTAINER_ID=$(podman create loki-handout-debug 2>/dev/null); then
#     echo "Failed to create debug container"
#     exit 1
# fi
# if ! podman cp "$CONTAINER_ID:/handout.tar.gz" "$ARTIFACTS_DIR/debug-handout.tar.gz"; then
#     echo "Failed to extract debug handout"
#     podman rm "$CONTAINER_ID" 2>/dev/null
#     exit 1
# fi
# podman rm "$CONTAINER_ID" >/dev/null 2>&1
# cd "$ARTIFACTS_DIR" && tar -xzf debug-handout.tar.gz >/dev/null && mv handout/reverseme handout/challenge . && rm -rf handout debug-handout.tar.gz && cd ..
# 
# echo "Renaming debug artifacts..."
# if [ -f "$ARTIFACTS_DIR/reverseme" ]; then
#     mv "$ARTIFACTS_DIR/reverseme" "$ARTIFACTS_DIR/reverseme-debug"
# fi
# if [ -f "$ARTIFACTS_DIR/challenge" ]; then
#     mv "$ARTIFACTS_DIR/challenge" "$ARTIFACTS_DIR/challenge-debug"
# fi

# Create artifacts directory for production build
mkdir -p "$ARTIFACTS_DIR"

echo ""
echo "=== Checking/Building base image ==="
if ! podman images | grep -q "loki-build-base"; then
    echo "Building base image with build tools..."
    if ! podman build -f "$CHALLENGE_DIR/Dockerfile.base" -t loki-build-base "$CHALLENGE_DIR" >> "$LOG_FILE" 2>&1; then
        echo "Base image build failed, check $LOG_FILE for details"
        exit 1
    fi
    echo "  Base image built successfully"
else
    echo "  Using existing loki-build-base image"
fi

echo ""
echo "=== Building PRODUCTION version ==="
echo "Building production container..."
echo "  (writing detailed output to $LOG_FILE...)"
if ! podman build -f "$CHALLENGE_DIR/Dockerfile" -t loki-handout-production "$CHALLENGE_DIR" >> "$LOG_FILE" 2>&1; then
    echo "Production build failed, check $LOG_FILE for details"
    exit 1
fi
echo "  Production build completed successfully"

echo "Extracting production artifacts..."
if ! CONTAINER_ID=$(podman create loki-handout-production 2>/dev/null); then
    echo "Failed to create production container"
    exit 1
fi
if ! podman cp "$CONTAINER_ID:/handout.tar.gz" "$ARTIFACTS_DIR/production-handout.tar.gz"; then
    echo "Failed to extract production handout"
    podman rm "$CONTAINER_ID" 2>/dev/null
    exit 1
fi
podman rm "$CONTAINER_ID" >/dev/null 2>&1
cd "$ARTIFACTS_DIR" && tar -xzf production-handout.tar.gz >/dev/null && mv handout/reverseme handout/challenge . && rm -rf handout production-handout.tar.gz && cd "$SCRIPT_DIR"

echo "Renaming production artifacts..."
if [ -f "$ARTIFACTS_DIR/reverseme" ]; then
    mv "$ARTIFACTS_DIR/reverseme" "$ARTIFACTS_DIR/reverseme-production"
fi
if [ -f "$ARTIFACTS_DIR/challenge" ]; then
    mv "$ARTIFACTS_DIR/challenge" "$ARTIFACTS_DIR/challenge-production"
fi

echo ""
echo "=== Building TEST version ==="
echo "Building test binary with TEST_FLAG..."
echo "  (writing detailed output to $LOG_FILE...)"
# Build test version using the base image
CONTAINER_ID=$(podman run -d loki-build-base sleep 3600)
podman cp "$SRC_DIR/." $CONTAINER_ID:/src/ >> "$LOG_FILE" 2>&1
echo "Building test binary..." | tee -a "$LOG_FILE"
if podman exec $CONTAINER_ID bash -c "cd /src && make test && python3 update_checksum.py reverseme-test" >> "$LOG_FILE" 2>&1; then
    podman cp $CONTAINER_ID:/src/reverseme-test "$ARTIFACTS_DIR/reverseme-test" >> "$LOG_FILE" 2>&1
    echo "  Test build completed successfully"
else
    echo "  Test build failed, check $LOG_FILE"
fi
podman stop $CONTAINER_ID >> "$LOG_FILE" 2>&1
podman rm $CONTAINER_ID >> "$LOG_FILE" 2>&1

echo ""
echo "=== Running TEST version ==="
if [ -f "$ARTIFACTS_DIR/reverseme-test" ]; then
    echo "Testing flag assembly..."
    echo "Running test binary..." | tee -a "$LOG_FILE"
    podman run --rm -v "$ARTIFACTS_DIR:/artifacts:ro" ubuntu:22.04 /artifacts/reverseme-test | tee -a "$LOG_FILE"
else
    echo "Error: Test binary not found"
fi

echo ""
echo "=== Artifact Generation Complete ==="
echo "Generated artifacts:"
ls -la "$ARTIFACTS_DIR/"

echo ""
echo "Debug artifacts:"
ls -la "$ARTIFACTS_DIR"/*-debug 2>/dev/null || echo "No debug artifacts found"

echo ""
echo "Production artifacts:"
ls -la "$ARTIFACTS_DIR"/*-production 2>/dev/null || echo "No production artifacts found"

echo ""
echo "Test artifact:"
ls -la "$ARTIFACTS_DIR"/*-test 2>/dev/null || echo "No test artifact found"

echo ""
echo "=== Verifying XOR obfuscation in production binary ==="
if [ -f "$ARTIFACTS_DIR/reverseme-production" ]; then
    echo "Checking for XOR obfuscation in production reverseme binary..."
    # Run objdump in a podman container to disassemble the binary
    podman run --rm -v "$ARTIFACTS_DIR:/artifacts:ro" ubuntu:22.04 bash -c "
        apt-get update -qq > /dev/null 2>&1 && apt-get install -y -qq binutils > /dev/null 2>&1
        echo 'Looking for XOR instructions in check_system_resources function:'
        objdump -d /artifacts/reverseme-production | grep -A 20 -B 5 'xor.*0x5a' || echo 'No XOR with 0x5a found'
    "
else
    echo "Warning: Production reverseme binary not found"
fi

echo ""
echo "All artifacts generated successfully!"