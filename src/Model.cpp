#include "Model.h"
#include <iostream>

Model::Model(const char *path) { loadModel(path); }

Model::~Model() {}

void Model::draw(Shader *shader) const {
  for (const auto &mesh : meshes) {
    mesh->draw();
  }
}

void Model::loadModel(const std::string &path) {
  Assimp::Importer importer;
  const aiScene *scene = importer.ReadFile(
      path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals |
                aiProcess_CalcTangentSpace);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
    return;
  }

  directory = path.substr(0, path.find_last_of('/'));

  processNode(scene->mRootNode, scene);

  std::cout << "Loaded model: " << path << " (" << meshes.size() << " meshes)"
            << std::endl;
}

void Model::processNode(aiNode *node, const aiScene *scene) {
  // Process all the node's meshes
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    meshes.push_back(std::unique_ptr<Mesh>(processMesh(mesh, scene)));
  }

  // Recursively process child nodes
  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }
}

Mesh *Model::processMesh(aiMesh *mesh, const aiScene *scene) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  // Process vertices
  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;

    // Position
    vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y,
                                mesh->mVertices[i].z);

    // Normals
    if (mesh->HasNormals()) {
      vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y,
                                mesh->mNormals[i].z);
    } else {
      vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
    }

    // Texture coordinates
    if (mesh->mTextureCoords[0]) {
      vertex.texCoord =
          glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
    } else {
      vertex.texCoord = glm::vec2(0.0f, 0.0f);
    }

    vertices.push_back(vertex);
  }

  // Process indices
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  // Process materials/textures (optional for now)
  // Can be extended later to load textures from materials

  return new Mesh(vertices, indices);
}

std::vector<Texture *> Model::loadMaterialTextures(aiMaterial *mat,
                                                   aiTextureType type) {
  std::vector<Texture *> textures;

  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);

    std::string filename = directory + "/" + std::string(str.C_Str());

    // Check if texture was already loaded
    bool skip = false;
    for (const auto &loadedTex : this->textures) {
      if (loadedTex->path == filename) {
        textures.push_back(loadedTex.get());
        skip = true;
        break;
      }
    }

    if (!skip) {
      Texture *texture = new Texture(filename.c_str());
      this->textures.push_back(std::unique_ptr<Texture>(texture));
      textures.push_back(texture);
    }
  }

  return textures;
}
