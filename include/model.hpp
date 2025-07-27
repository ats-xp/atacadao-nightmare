#pragma once

#include "mesh.hpp"

#include <assimp/scene.h>

class Model {
  std::vector<Mesh> m_meshes;
  std::vector<Texture> m_textures_loaded;

  std::string m_directory;

  void processNode(aiNode *node, const aiScene *scene);
  Mesh processMesh(aiMesh *mesh, const aiScene *scene);

  std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                            u8 tex_type);
  u32 textureFromFile(const char *path, const char *dir);

  glm::mat4 m_model;

  glm::vec3 m_pos;
  glm::vec3 m_scale;
  glm::vec3 m_rotation;

public:
  Model(const char *path);
  ~Model();

  void draw(Camera &cam);

  void setPos(glm::vec3 pos);
  void setScale(glm::vec3 scale);
  void setRotation(glm::vec3 rot);
};
