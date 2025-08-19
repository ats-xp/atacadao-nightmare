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

void initTexturePool();
Texture loadTexture(const char *filename, TextureType tex_type = DIFFUSE);
void addTexture(const char *path);
void addTextureOnThread(const char *path);

void setTextureFilter(sg_filter min, sg_filter max);
void setTextureWrap(sg_wrap u, sg_wrap v);
void setTextureVerticalFlip(bool flip);

const std::string getTextureIDFromPath(const std::string &path);
Texture &getTextureFromID(const std::string &id);
const u16 &getTextureRequests(void);

bool isTexture(const std::string &id);
void destroyTexture(std::string id); // Warning: inutil

class Mesh {
public:
  std::vector<Vertex> m_vertices;
  std::vector<u16> m_indices;
  std::vector<Texture> m_textures;
  std::vector<std::string> m_textures_path;

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
