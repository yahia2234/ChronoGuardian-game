#!/bin/bash

# Chrono Guardian - Build Script
# This script builds the project after dependencies are installed

echo "=== Chrono Guardian Build Script ==="
echo ""

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "Error: CMakeLists.txt not found. Please run this script from the project root."
    exit 1
fi

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
fi

# Navigate to build directory
cd build

# Run CMake
echo "Running CMake..."
cmake ..

if [ $? -ne 0 ]; then
    echo ""
    echo "CMake failed! Make sure all dependencies are installed:"
    echo "  brew install cmake glfw glew glm"
    exit 1
fi

# Build the project
echo ""
echo "Building project..."
make

if [ $? -ne 0 ]; then
    echo ""
    echo "Build failed! Check the error messages above."
    exit 1
fi

echo ""
echo "=== Build Successful! ==="
echo ""
echo "To run the game:"
echo "  cd build"
echo "  ./ChronoGuardian"
echo ""
echo "Controls:"
echo "  WASD - Move"
echo "  Mouse - Look around"
echo "  Right Mouse Button - Toggle camera"
echo "  R - Restart level"
echo "  ESC - Quit"
echo ""
