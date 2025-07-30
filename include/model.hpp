#pragma once

#include "mesh.hpp"

#include <assimp/scene.h>

class Model {
  std::string m_directory;

  std::vector<Mesh> m_meshes;
  std::vector<Texture> m_textures_loaded;

  glm::vec3 m_pos;
  glm::vec3 m_scale;
  glm::vec3 m_axis;
  f32 m_rotation;

  Transform m_transformition;

  void processNode(aiNode *node, const aiScene *scene);
  Mesh processMesh(aiMesh *mesh, const aiScene *scene);

  std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                            u8 tex_type);

  constexpr std::string getTexturePath(const char *path) {
    return std::string(m_directory + "/" + path);
  }

public:
  Model(const char *path);

  Model(const Model &other);
  Model &operator=(const Model &other);

  Model(Model &&other);
  Model &operator=(Model &&other);

  ~Model();

  void init(const char *path);
  void draw(Camera &cam);

  constexpr void resetRotation() { m_axis = glm::vec3(0.0f, 0.0f, 0.0f); }

  constexpr void setPos(const glm::vec3 &pos) { m_pos = pos; }

  constexpr void setScale(const glm::vec3 &scale) { m_scale = scale; }

  constexpr void setRotation(const f32 &angle, const glm::vec3 &axis) {
    m_rotation = angle;
    m_axis = axis;
  }

  constexpr void setRotationX(const f32 &angle) {
    m_rotation = angle;
    m_axis.x = 1.0f;
  }

  constexpr void setRotationY(const f32 &angle) {
    m_rotation = angle;
    m_axis.y = 1.0f;
  }

  constexpr void setRotationZ(const f32 &angle) {
    m_rotation = angle;
    m_axis.z = 1.0f;
  }

  constexpr void setTransformitions(const Transform &trans) {
    m_transformition = trans;
  }
};

struct ModelStore {
  std::vector<Model> models;
  std::vector<Transform> transforms;
};

void initModelStore(ModelStore &store, const Model &model,
                    const Transform &trans);
