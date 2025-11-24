# Installation Guide for Chrono Guardian

## Installing Dependencies on macOS

Since CMake is not currently installed, here's how to set up your development environment:

### Option 1: Using Homebrew (Recommended)

1. **Install Homebrew** (if not already installed):
```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

2. **Install all required dependencies**:
```bash
brew install cmake glfw glew glm
```

### Option 2: Manual Installation

Download and install each component:
- **CMake**: https://cmake.org/download/
- **GLFW**: https://www.glfw.org/download.html
- **GLEW**: http://glew.sourceforge.net/
- **GLM**: https://github.com/g-truc/glm/releases

## Building the Project

Once dependencies are installed:

```bash
cd "/Users/yahia/Desktop/acl proj"
mkdir -p build
cd build
cmake ..
make
```

## Running the Game

```bash
./ChronoGuardian
```

## Troubleshooting

### "command not found: cmake"
- Install CMake using Homebrew or download from cmake.org
- Make sure CMake is in your PATH

### "Cannot find GLFW/GLEW/GLM"
- Install missing libraries via Homebrew
- Or specify library paths in CMakeLists.txt

### OpenGL version errors
- Ensure your Mac supports OpenGL 3.3+
- Most Macs from 2012+ support this

### Compilation errors
- Make sure you have Xcode Command Line Tools: `xcode-select --install`
- Check that all .h and .cpp files are present

## Quick Start (After Installation)

```bash
# One-time setup
brew install cmake glfw glew glm

# Build
cd "/Users/yahia/Desktop/acl proj"
mkdir build && cd build && cmake .. && make

# Run
./ChronoGuardian
```
