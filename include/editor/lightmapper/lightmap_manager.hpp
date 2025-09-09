#pragma once

#include <vector>
#include <string>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm.hpp>

typedef uint16_t u16;
typedef uint32_t u32;

struct aiNode;
struct aiMesh;
struct aiScene;

struct Vertex {
  glm::vec3 pos;
  glm::vec3 normal;
  glm::vec2 tex_coords;
};

namespace LM {

struct ObjectMesh {
  std::vector<Vertex> vertices;
  std::vector<u16> indices;

  ObjectMesh(std::vector<Vertex> v, std::vector<u16> i);
  ~ObjectMesh();

  GLuint vao, vbo, ibo;
};

struct Object {
  std::vector<ObjectMesh> meshes;

  Object();
  ~Object();

  int load(const char *path);

  void processNode(aiNode *node, const aiScene *scene);
  ObjectMesh processMesh(aiMesh *mesh, const aiScene *scene);
};

} // namespace LM
