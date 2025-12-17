<p align="center">
  <img src="https://img.shields.io/badge/OpenGL-3.3-blue?style=for-the-badge&logo=opengl" alt="OpenGL 3.3"/>
  <img src="https://img.shields.io/badge/C++-17-00599C?style=for-the-badge&logo=cplusplus" alt="C++17"/>
  <img src="https://img.shields.io/badge/Platform-macOS%20|%20Linux%20|%20Windows-lightgrey?style=for-the-badge" alt="Platform"/>
  <img src="https://img.shields.io/badge/License-MIT-green?style=for-the-badge" alt="License"/>
</p>

# 🤖 Chrono Guardian

> A 3D puzzle-platformer game featuring a hovering robot navigating through ancient chambers filled with deadly traps and mystical artifacts.

<p align="center">
  <strong>Navigate treacherous obstacles • Collect mystical artifacts • Master time and space</strong>
</p>

---

## ✨ Features

### 🎮 Core Gameplay
- **Dual Camera System** — Seamlessly toggle between first-person and third-person perspectives
- **Physics-Based Movement** — Smooth hovering mechanics with realistic collision detection
- **Dynamic Obstacles** — Swinging pendulums, falling stalactites, erupting geysers, and crumbling floors
- **Collectible System** — Gather crystals and gemstones to unlock progression

### 🎨 Graphics & Rendering
- **Blinn-Phong Shading** — Realistic lighting with specular highlights
- **Multi-Light Support** — Up to 10 dynamic light sources with distance attenuation
- **Particle Effects** — GPU-accelerated sparks and impact particles
- **Procedural Geometry** — Fully procedural mesh generation (spheres, cubes, cylinders)
- **Custom GLSL Shaders** — Hand-crafted vertex and fragment shaders

### 🔊 Audio System
- **Procedural Sound Generation** — All sounds generated programmatically using OpenAL
- **7 Unique Sound Effects** — Movement, collisions, pickups, and level completion
- **Spatial Audio** — Immersive 3D audio positioning
- **Zero External Dependencies** — No audio files required

### 🏛️ Two Unique Levels

| Level 1: Futuristic Chamber | Level 2: Organic Cavern |
|:---------------------------:|:-----------------------:|
| Polished stone environment | Dark cave with torches |
| 3 Swinging pendulums | 5 Falling stalactites |
| 4 Crumbling floor tiles | 3 Erupting geysers |
| Energy Crystal collectible | Glowing Gemstone collectible |
| Force Field Door exit | Ancient Pedestal altar |

---

## 🎯 Gameplay Demo

```
╔════════════════════════════════════════════════════════════════╗
║  LEVEL 1                              LEVEL 2                  ║
║  ┌────────────────────┐               ┌────────────────────┐   ║
║  │  ◇ ◇ ◇  Pendulums  │               │  ▼ ▼ ▼  Stalactites│   ║
║  │    ↓ ↓ ↓           │   ──────►     │    ↓ ↓ ↓           │   ║
║  │  ░░░░  Crumbling   │               │  ~ ~ ~  Geysers    │   ║
║  │    💎  Crystal     │               │    💎  Gemstone    │   ║
║  │  [===] Force Field │               │  [▄▄▄] Pedestal    │   ║
║  └────────────────────┘               └────────────────────┘   ║
╚════════════════════════════════════════════════════════════════╝
```

---

## 🚀 Quick Start

### Prerequisites

<details>
<summary><b>macOS</b> (Recommended)</summary>

```bash
brew install glfw glew glm cmake
```
> ℹ️ OpenAL is built into macOS — no additional installation needed!

</details>

<details>
<summary><b>Ubuntu/Debian</b></summary>

```bash
sudo apt-get install libglfw3-dev libglew-dev libglm-dev cmake build-essential libopenal-dev
```

</details>

<details>
<summary><b>Windows</b></summary>

1. Install [CMake](https://cmake.org/download/)
2. Install [GLFW](https://www.glfw.org/download.html)
3. Install [GLEW](http://glew.sourceforge.net/)
4. Install [GLM](https://github.com/g-truc/glm/releases)
5. Install [OpenAL SDK](https://www.openal.org/downloads/)

</details>

### Build & Run

```bash
# Clone the repository
git clone https://github.com/yourusername/chrono-guardian.git
cd chrono-guardian

# Build the project
mkdir build && cd build
cmake ..
make

# Run the game
./ChronoGuardian
```

---

## 🎮 Controls

| Key | Action |
|:---:|:------:|
| `W` `A` `S` `D` | Move Forward / Left / Backward / Right |
| `Mouse` | Look Around |
| `Right Click` | Toggle First/Third Person Camera |
| `R` | Restart Current Level |
| `ESC` | Quit Game |

---

## 🏗️ Project Architecture

```
chrono-guardian/
├── 📂 src/                    # Source files (17 files, ~3,800 lines)
│   ├── main.cpp               # Entry point
│   ├── Game.cpp               # Main game loop & state management
│   ├── Player.cpp             # Player character with orbiting fragments
│   ├── Level1.cpp             # Futuristic Chamber implementation
│   ├── Level2.cpp             # Organic Cavern implementation
│   ├── Camera.cpp             # Dual camera system
│   ├── Physics.cpp            # AABB & sphere collision detection
│   └── ...
├── 📂 include/                # Header files (16 files)
├── 📂 shaders/                # GLSL shader programs (12 files)
│   ├── vertex.glsl            # Vertex transformations
│   ├── fragment.glsl          # Blinn-Phong lighting
│   ├── particle_*.glsl        # Particle system rendering
│   └── *_screen_*.glsl        # UI screen shaders
├── 📂 assets/
│   ├── 📂 models/             # 3D models (.glb format)
│   └── 📂 sounds/             # Audio assets
└── CMakeLists.txt             # Build configuration
```

---

## 🔧 Technical Highlights

### Graphics Pipeline
```cpp
// Blinn-Phong Shading with Multiple Dynamic Lights
vec3 lighting = ambient;
for (int i = 0; i < numLights; i++) {
    float attenuation = 1.0 / (1.0 + 0.09*dist + 0.032*dist*dist);
    lighting += (diffuse + specular) * attenuation;
}
```

### Physics System
- **AABB Collision** — Axis-Aligned Bounding Box for walls and rectangular objects
- **Sphere Collision** — For player and round objects with penetration resolution
- **Trigger Zones** — Non-physical interaction areas for collectibles and exits

### Procedural Audio Generation
```cpp
// All 7 sound effects generated using sine waves and noise synthesis
AudioManager::generateTone(frequency, duration, type);
```

---

## 📊 Performance

| Metric | Value |
|:------:|:-----:|
| Target FPS | 60 |
| Audio Latency | < 10ms |
| Memory Footprint | ~50MB |
| OpenGL Version | 3.3 Core |

---

## 🎓 Course Project

This game was developed as a final project for **DMET 502: Computer Graphics** at the German University in Cairo (GUC), Winter 2025.

### Requirements Met
- ✅ First-person and third-person camera navigation
- ✅ Multiple 3D obstacles with collision detection
- ✅ Textured 3D models with procedural geometry
- ✅ Animations for every player-object interaction
- ✅ Two distinct levels with unique mechanics
- ✅ Multiple dynamic light sources with varying intensities

---

## 📄 License

This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

- **OpenGL** — Graphics rendering
- **GLFW** — Window and input management  
- **GLEW** — OpenGL extension loading
- **GLM** — Mathematics library
- **OpenAL** — Audio processing
- **stb_image** — Texture loading

---

<p align="center">
  <sub>Built with ❤️ and OpenGL</sub>
</p>
