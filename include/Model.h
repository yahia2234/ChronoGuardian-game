#ifndef MODEL_H
#define MODEL_H

#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>

#include "Mesh.h"
#include "Texture.h"
#include <memory>
#include <string>
#include <vector>

class Model {
public:
  std::vector<std::unique_ptr<Mesh>> meshes;
  std::vector<std::unique_ptr<Texture>> textures;
  std::string directory;

  Model(const char *path);
  ~Model();

  void draw(class Shader *shader) const;

private:
  void loadModel(const std::string &path);
  void processNode(aiNode *node, const aiScene *scene);
  Mesh *processMesh(aiMesh *mesh, const aiScene *scene);
  std::vector<Texture *> loadMaterialTextures(aiMaterial *mat,
                                              aiTextureType type);
};

#endif
