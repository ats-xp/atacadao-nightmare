#pragma once

#include <string>
#include <vector>

#include <glm.hpp>

#include "base.h"
#include "sokol_gfx.h"

#include "camera.hpp"

class Shader;
class RenderBuffer;

enum TextureType : u8 {
  DIFFUSE = 0,
  SPECULAR,
  NORMAL,
  HEIGHT,
};

struct Vertex {
  glm::vec3 pos;
  glm::vec4 color;
  glm::vec3 normal;
  glm::vec2 tex_coords;
};

struct Texture {
  u32 id;
  sg_sampler smp;
  TextureType type;
  std::string path;
};

class Mesh {
  std::vector<Vertex> m_vertices;
  std::vector<u16> m_indices;
  std::vector<Texture> m_textures;

  sg_pipeline m_pipe;
  sg_shader m_shader;
  sg_bindings m_bind;

  glm::vec3 m_pos;
  glm::vec3 m_scale;
  glm::vec3 m_rotation;

  glm::mat4 m_model;

  // tmp??
  RenderBuffer *m_buf;
  bool m_trash;

public:
  Mesh(std::vector<Vertex> v, std::vector<u16> i, std::vector<Texture> t);
  ~Mesh();

  void begin();
  void end();
  void draw(Camera &cam);
  void destroy();

  inline void setPos(glm::vec3 pos) { m_pos = pos; }
  inline void setScale(glm::vec3 scale) { m_scale = scale; }

  // TODO
  // inline void setRotationX(glm::vec3 rot) { m_rotation = rot; }
};
