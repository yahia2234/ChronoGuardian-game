# Models & Textures Guide

## Overview
Chrono Guardian now supports importing 3D models and textures using **Assimp**.

## Supported Model Formats
- ✅ **OBJ** (Wavefront)
- ✅ **FBX** (Autodesk)
- ✅ **GLTF/GLB** (GL Transmission Format)
- ✅ **DAE** (Collada)
- ✅ **3DS** (3D Studio)
- ✅ **BLEND** (Blender)
- ✅ **And 40+ more formats!**

## Supported Texture Formats
- ✅ **PNG** (recommended - supports transparency)
- ✅ **JPG/JPEG**
- ✅ **BMP**
- ✅ **TGA**

## Directory Structure
```
assets/
├── models/
│   ├── player.obj
│   ├── wall.fbx
│   └── coin.gltf
└── textures/
    ├── brick.png
    ├── stone.jpg
    └── metal.png
```

## Usage Example

### Loading a Model
```cpp
#include "Model.h"

// In your initialization code
Model* playerModel = new Model("assets/models/player.obj");

// In your render loop
playerModel->draw(shader);
```

### Loading a Texture
```cpp
#include "Texture.h"

// Load texture
Texture* brickTexture = new Texture("assets/textures/brick.png");

// Bind before drawing
brickTexture->bind(0); // Bind to texture slot 0
// ... draw your object ...
brickTexture->unbind();
```

## Free Model Resources
- **[Sketchfab](https://sketchfab.com/feed)** - Downloadable 3D models (many free)
- **[TurboSquid Free](https://www.turbosquid.com/Search/3D-Models/free)** - Free 3D models
- **[Free3D](https://free3d.com/)** - Free 3D models
- **[Poly Haven](https://polyhaven.com/models)** - CC0 models

## Free Texture Resources
- **[Poly Haven](https://polyhaven.com/textures)** - CC0 PBR textures
- **[AmbientCG](https://ambientcg.com/)** - Free PBR textures
- **[Textures.com](https://www.textures.com/)** - 15 free credits/day
- **[OpenGameArt](https://opengameart.org/)** - Free game textures

## Tips
1. **Optimize models**: Keep poly count reasonable (< 10k triangles for game objects)
2. **Texture size**: Use power-of-2 dimensions (256x256, 512x512, 1024x1024)
3. **Format**: OBJ is simplest, GLTF is modern standard
4. **Attribution**: Check licenses for free resources

## Current Status
✅ Assimp installed (v6.0.2)
✅ Model loading class implemented
✅ Texture loading working
✅ Ready to import models and textures!
