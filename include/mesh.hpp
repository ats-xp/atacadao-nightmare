#pragma once

#include <map>
#include <string>
#include <vector>

#include <glm.hpp>

#include "base.h"
#include "sokol_gfx.h"

#include "camera.hpp"

class Shader;
struct RenderBuffer;

enum TextureType : u8 {
  DIFFUSE = 0,
  SPECULAR,
  NORMAL,
  HEIGHT,
};

struct Transform {
  glm::vec3 position;
  glm::vec3 scale;
  glm::vec3 rotation;

  constexpr void setPosition(const glm::vec3 &offset) { position = offset; }

  constexpr void move(const glm::vec3 &offset) { position += offset; }
  constexpr void moveX(const f32 &x) { position.x += x; }
  constexpr void moveY(const f32 &y) { position.y += y; }
  constexpr void moveZ(const f32 &z) { position.z += z; }
};

struct Vertex {
  glm::vec3 pos;
  glm::vec3 normal;
  glm::vec2 tex_coords;
  glm::vec2 lightmap_coords;
};

struct Texture {
  std::string path;
  sg_image img;
  sg_sampler smp;
  TextureType type;

  inline void destroy() {
    sg_destroy_sampler(smp);
    sg_destroy_image(img);
  }
};

class Mesh {
public:
  std::vector<Vertex> m_vertices;
  std::vector<u16> m_indices;
  std::vector<Texture> m_textures;
  std::vector<std::string> m_textures_path;

  union {
    std::vector<f32> m_lightmap;
  };

  sg_buffer m_vbo;
  sg_buffer m_ebo;

  Mesh(std::vector<Vertex> vertices, std::vector<u16> indices,
       std::vector<std::string> textures_path);
  Mesh(const Mesh &other);
  Mesh &operator=(const Mesh &other);

  Mesh(Mesh &&other) noexcept;
  Mesh &operator=(Mesh &&other) noexcept;

  ~Mesh();

  void draw();
  void destroy();

  void bind(u16 img, u16 smp);
};
