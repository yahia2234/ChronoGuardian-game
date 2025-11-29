# Player Model Import Guide

## 📥 **Step 1: Download a Free Model**

### **Recommended Source: Sketchfab**
Visit: https://sketchfab.com/search?features=downloadable&licenses=7c23a1ba438d4306920229c12afcb5f2&q=low+poly+character&sort_by=-likeCount&type=models

**Filters to use:**
- ✅ **Downloadable**: Yes
- ✅ **License**: CC0 (Public Domain)
- ✅ **Search**: "low poly character" or "robot" or "knight"

### **Good Free Models to Try:**
1. **KayKit Character Pack** (itch.io) - CC0, game-ready
   - https://kaylousberg.itch.io/kaykit-adventurers
   
2. **Quaternius Models** (Free, CC0)
   - https://quaternius.com/packs/ultimatecharacters.html

3. **Mixamo Characters** (Free with Adobe account)
   - https://www.mixamo.com/

## 📁 **Step 2: File Placement**

### **Directory Structure:**
```
ChronoGuardian-game/
├── assets/
│   └── models/
│       └── player.obj          ← Put your model here
│           player.mtl           ← Material file (if included)
│           player_texture.png  ← Texture (if included)
```

### **Supported Formats:**
- ✅ **OBJ** (recommended - simple, widely supported)
- ✅ **FBX** (Autodesk format)
- ✅ **GLTF/GLB** (modern format)

**Tip**: When downloading from Sketchfab, choose **"Autoconverted format (glTF)"** or **"Original format (OBJ)"**

## 🔧 **Step 3: Integrate into Player.cpp**

### **Current Player Code Location:**
`/Users/omar/Desktop/GUC/Chron0/ChronoGuardian-game/src/Player.cpp`

### **Modification Needed:**

**Add at top:**
```cpp
#include "Model.h"
```

**In Player constructor (around line 8-14):**
```cpp
// OPTION 1: Replace procedural mesh with model
model.reset(new Model("assets/models/player.obj"));

// OPTION 2: Keep both (model + orbiting fragments)
model.reset(new Model("assets/models/player.obj"));
// Keep existing fragment code...
```

**In Player::draw() method:**
```cpp
void Player::draw(Shader* shader) {
    shader->use();
    shader->setMat4("model", transform.getModelMatrix());
    shader->setVec3("objectColor", glm::vec3(1.0f, 0.8f, 0.3f)); // Gold color
    
    // Draw imported model instead of procedural sphere
    if (model) {
        model->draw(shader);
    }
    
    // Still draw orbiting fragments
    for (auto& frag : fragments) {
        // ... existing fragment drawing code
    }
}
```

## 🎨 **Step 4: Scale & Position**

Models might be too big/small. Adjust in Player constructor:
```cpp
transform.scale = glm::vec3(0.5f); // Make smaller
// or
transform.scale = glm::vec3(2.0f); // Make bigger
```

## ✅ **Quick Test Checklist**

1. ✅ Download model (OBJ recommended)
2. ✅ Place in `assets/models/player.obj`
3. ✅ Modify `Player.cpp` to use Model class
4. ✅ Rebuild: `cd build && make`
5. ✅ Run: `./ChronoGuardian`

## 🚀 **Want Me to Do It?**

I can:
1. Download a specific free model for you
2. Modify Player.cpp to use it
3. Set proper scale/position
4. Build and test

Just say "yes" and I'll integrate a cool player model!
