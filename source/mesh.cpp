#include "sokol_gfx.h"
#include "stb_image.h"

#include "mesh.hpp"

void Texture::load(const char *filename, TextureType tex_type) {
  std::string f = std::string(filename);
  path = f.substr(f.find_last_of("/") + 1).c_str();

  type = tex_type;

  int w, h, nch;
  stbi_set_flip_vertically_on_load(true);
  stbi_uc *data = stbi_load(filename, &w, &h, &nch, 4);

  sg_image img = {};

  if (data) {
    LogInfo("Loading texture: %s", filename);

    sg_image_desc desc = {};
    desc.type = SG_IMAGETYPE_2D;
    desc.width = w;
    desc.height = h;
    desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    desc.data.subimage[0][0].ptr = data;
    desc.data.subimage[0][0].size = w * h * 4;

    img = sg_make_image(&desc);

    loaded = true;
  } else {
    img.id = 0;
    LogError("Texture loading error %s file: %s", stbi_failure_reason(),
             filename);
  }

  id = img.id;

  sg_sampler_desc desc = {};
  desc.min_filter = SG_FILTER_NEAREST;
  desc.mag_filter = SG_FILTER_NEAREST;
  smp = sg_make_sampler(&desc);

  stbi_image_free(data);
}

void Texture::destroy() {
  sg_image img = {};
  img.id = id;
  sg_destroy_sampler(smp);
  sg_destroy_image(img);
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<u16> indices,
           std::vector<Texture> textures)
    : m_vertices(vertices), m_indices(indices), m_textures(textures) {
  sg_buffer_desc vbuf_desc = {};
  vbuf_desc.data.ptr = m_vertices.data();
  vbuf_desc.data.size = m_vertices.size() * sizeof(Vertex);

  sg_buffer_desc ibuf_desc = {};
  ibuf_desc.usage.index_buffer = true;
  ibuf_desc.data.ptr = m_indices.data();
  ibuf_desc.data.size = m_indices.size() * sizeof(u16);

  m_vbo = sg_make_buffer(&vbuf_desc);
  m_ebo = sg_make_buffer(&ibuf_desc);
}

Mesh::Mesh(const Mesh &other)
    : m_vertices(other.m_vertices), m_indices(other.m_indices),
      m_textures(other.m_textures), m_vbo(other.m_vbo), m_ebo(other.m_ebo) {}

Mesh &Mesh::operator=(const Mesh &other) {
  Mesh tmp(other);
  std::swap(m_vertices, tmp.m_vertices);
  std::swap(m_indices, tmp.m_indices);
  std::swap(m_textures, tmp.m_textures);
  m_vbo = other.m_vbo;
  m_ebo = other.m_ebo;
  return *this;
}

Mesh::Mesh(Mesh &&other) noexcept
    : m_vertices(other.m_vertices), m_indices(other.m_indices),
      m_textures(other.m_textures), m_vbo(other.m_vbo), m_ebo(other.m_ebo) {}

Mesh &Mesh::operator=(Mesh &&other) noexcept {
  Mesh tmp(other);
  std::swap(m_vertices, tmp.m_vertices);
  std::swap(m_indices, tmp.m_indices);
  std::swap(m_textures, tmp.m_textures);
  m_vbo = other.m_vbo;
  m_ebo = other.m_ebo;
  return *this;
}

Mesh::~Mesh() {}

void Mesh::draw() { sg_draw(0, m_indices.size() * 3, 1); }

void Mesh::destroy() {
  sg_destroy_buffer(m_ebo);
  sg_destroy_buffer(m_vbo);

  for (Texture &t : m_textures) {
    t.destroy();
  }
}
