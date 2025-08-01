#pragma once

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
};

struct Vertex {
  glm::vec3 pos;
  glm::vec3 normal;
  glm::vec2 tex_coords;
};

struct Texture {
  std::string path;
  u32 id;
  sg_sampler smp;
  TextureType type;

  bool loaded;

  void load(const char *filename, TextureType tex_type = DIFFUSE);
  void attrib(const sg_filter &min_filter = SG_FILTER_LINEAR,
              const sg_filter &max_filter = SG_FILTER_LINEAR,
              const sg_wrap &wrap_s = SG_WRAP_REPEAT,
              const sg_wrap &wrap_t = SG_WRAP_REPEAT);
  void destroy();
};

class Mesh {
public:
  std::vector<Vertex> m_vertices;
  std::vector<u16> m_indices;
  std::vector<Texture> m_textures;

  sg_buffer m_vbo;
  sg_buffer m_ebo;

  Mesh(std::vector<Vertex> vertices, std::vector<u16> indices,
       std::vector<Texture> textures);
  Mesh(const Mesh &other);
  Mesh &operator=(const Mesh &other);

  Mesh(Mesh &&other) noexcept;
  Mesh &operator=(Mesh &&other) noexcept;

  ~Mesh();

  void draw();
  void destroy();

  void bind(u16 img, u16 smp);
};
