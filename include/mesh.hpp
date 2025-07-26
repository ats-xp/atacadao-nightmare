#pragma once

#include <string>
#include <vector>

#include "base.h"
// #include "gl.hpp"
#include <glm.hpp>

class Shader;

enum TextureType : u8 {
  DIFFUSE = 0,
  SPECULAR,
  NORMAL,
  HEIGHT,
};

struct Vertex {
  glm::vec3 pos;
  glm::vec4 color;
  // glm::vec3 normal;
  // glm::vec2 tex_coords;
};

// extern std::vector<Vertex> vtx_cube;
// extern std::vector<u32> ind_cube;

struct Texture {
  u32 id;
  TextureType type;
  std::string path;
};

class Mesh {
public:
  std::vector<Vertex> vertices;
  std::vector<u16> indices;
  // std::vector<Texture> textures;

  // u32 VAO, VBO, EBO;
  sg_bindings m_bind;

  Mesh(std::vector<Vertex> v, std::vector<u16> i);
  // Mesh(std::vector<Vertex> v, std::vector<u32> i, std::vector<Texture> t);
  ~Mesh();

  void draw();

  static Mesh *createCube() {
    std::vector<Vertex> vtx = {
        Vertex({-1.0, -1.0, -1.0}, {1.0, 0.0, 0.0, 1.0}),
        Vertex({1.0, -1.0, -1.0}, {1.0, 0.0, 0.0, 1.0}),
        Vertex({1.0, 1.0, -1.0}, {1.0, 0.0, 0.0, 1.0}),
        Vertex({-1.0, 1.0, -1.0}, {1.0, 0.0, 0.0, 1.0}),

        Vertex({-1.0, -1.0, 1.0}, {0.0, 1.0, 0.0, 1.0}),
        Vertex({1.0, -1.0, 1.0}, {0.0, 1.0, 0.0, 1.0}),
        Vertex({1.0, 1.0, 1.0}, {0.0, 1.0, 0.0, 1.0}),
        Vertex({-1.0, 1.0, 1.0}, {0.0, 1.0, 0.0, 1.0}),

        Vertex({-1.0, -1.0, -1.0}, {0.0, 0.0, 1.0, 1.0}),
        Vertex({-1.0, 1.0, -1.0}, {0.0, 0.0, 1.0, 1.0}),
        Vertex({-1.0, 1.0, 1.0}, {0.0, 0.0, 1.0, 1.0}),
        Vertex({-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0, 1.0}),

        Vertex({1.0, -1.0, -1.0}, {1.0, 0.5, 0.0, 1.0}),
        Vertex({1.0, 1.0, -1.0}, {1.0, 0.5, 0.0, 1.0}),
        Vertex({1.0, 1.0, 1.0}, {1.0, 0.5, 0.0, 1.0}),
        Vertex({1.0, -1.0, 1.0}, {1.0, 0.5, 0.0, 1.0}),

        Vertex({-1.0, -1.0, -1.0}, {0.0, 0.5, 1.0, 1.0}),
        Vertex({-1.0, -1.0, 1.0}, {0.0, 0.5, 1.0, 1.0}),
        Vertex({1.0, -1.0, 1.0}, {0.0, 0.5, 1.0, 1.0}),
        Vertex({1.0, -1.0, -1.0}, {0.0, 0.5, 1.0, 1.0}),

        Vertex({-1.0, 1.0, -1.0}, {1.0, 0.0, 0.5, 1.0}),
        Vertex({-1.0, 1.0, 1.0}, {1.0, 0.0, 0.5, 1.0}),
        Vertex({1.0, 1.0, 1.0}, {1.0, 0.0, 0.5, 1.0}),
        Vertex({1.0, 1.0, -1.0}, {1.0, 0.0, 0.5, 1.0}),
    };

    std::vector<u16> ind = {0, 1, 3, 3, 1, 2, 1, 5, 2, 2, 5, 6,
                            5, 4, 6, 6, 4, 7, 4, 0, 7, 7, 0, 3,
                            3, 2, 7, 7, 2, 6, 4, 5, 0, 0, 5, 1};

    return new Mesh(vtx, ind);
  }

private:
  void setupMesh();
};
