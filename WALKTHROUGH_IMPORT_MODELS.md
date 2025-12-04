# How to Import 3D Models in Chrono Guardian

This project uses the **Assimp** library to load 3D models (GLTF, OBJ, FBX, etc.). The `Model` class (`include/Model.h`, `src/Model.cpp`) handles the loading and processing of these models.

## Step 1: Add Your Model File
1.  Place your model file (e.g., `player.gltf`, `enemy.obj`) in the `assets/models/` directory.
2.  Ensure any associated textures are also in the correct relative path (usually the same directory or a `textures` subdirectory).

## Step 2: Use the `Model` Class
You can use the `Model` class to load and draw your 3D asset.

### Option A: Using the `Model` class directly in a custom class (Recommended for Player)

1.  **Include the header** in your class file (e.g., `Player.h`):
    ```cpp
    #include "Model.h"
    ```

2.  **Add a member variable** to store the model:
    ```cpp
    class Player : public GameObject {
    private:
        std::unique_ptr<Model> playerModel;
        // ...
    };
    ```

3.  **Initialize the model** in your constructor (e.g., `Player.cpp`):
    ```cpp
    Player::Player() {
        // ... other initialization ...
        
        // Load the model
        // Note: Path is relative to the executable (build directory)
        playerModel = std::make_unique<Model>("assets/models/player.gltf");
        
        // You might need to adjust scale/rotation if the model isn't perfectly aligned
        transform.scale = glm::vec3(0.5f); 
    }
    ```

4.  **Draw the model** in your `draw` function:
    ```cpp
    void Player::draw(Shader* shader) {
        if (playerModel) {
            glm::mat4 model = transform.getModelMatrix();
            shader->setMat4("model", model);
            
            // Set default material properties if the model doesn't have them
            shader->setVec3("objectColor", glm::vec3(1.0f)); 
            
            playerModel->draw(shader);
        } else {
            // Fallback to default mesh if model failed to load
            // ...
        }
    }
    ```

### Option B: Extending `GameObject` to support Models

Currently, `GameObject` is designed for single meshes. To support complex models with multiple meshes (which `Model` handles), you can add a `Model` pointer to `GameObject` or create a subclass.

**Example Subclass:**

```cpp
class ModelGameObject : public GameObject {
public:
    std::unique_ptr<Model> modelAsset;

    ModelGameObject(const std::string& path, GameObjectType type) : GameObject(type) {
        modelAsset = std::make_unique<Model>(path.c_str());
    }

    void draw(Shader* shader) override {
        if (!isActive || !modelAsset) return;

        glm::mat4 model = transform.getModelMatrix();
        shader->setMat4("model", model);
        
        // Calculate normal matrix
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
        shader->setMat3("normalMatrix", normalMatrix);

        // Draw all meshes in the model
        modelAsset->draw(shader);
    }
};
```

## Step 3: Build and Run
1.  Run `./build.sh` to compile your changes.
2.  The build script automatically copies the `assets` folder to the `build` directory.
3.  Run `./ChronoGuardian` to see your model in game!

## Troubleshooting
*   **Model not showing?** Check the console output. Assimp will print errors if it can't find the file.
*   **Model too big/small?** Adjust `transform.scale` in your class constructor.
*   **Model facing wrong way?** Adjust `transform.rotation` or export your model with Z-forward (OpenGL standard).
*   **Textures missing?** Ensure texture paths in your model file are relative or absolute paths that exist. GLTF usually embeds them or keeps them relative.

