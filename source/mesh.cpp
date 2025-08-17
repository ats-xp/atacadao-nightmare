/*
 *
 * TODO Criar prevenções á dados relacionados na pilha de Texturas
 *
 */

#include "sokol_fetch.h"
#include "sokol_gfx.h"
#include "stb_image.h"

#include "mesh.hpp"

// tmp name??
std::vector<Texture> texture_pool;
u8 io_texture_buffer[256 * 1024];

static struct {
  std::map<std::string, Texture> hash;

  sg_filter min_filter : SG_FILTER_LINEAR;
  sg_filter max_filter : SG_FILTER_LINEAR;

  sg_wrap wrap_u : SG_WRAP_REPEAT;
  sg_wrap wrap_v : SG_WRAP_REPEAT;

} texture_info;

Texture loadTexture(const char *filename, TextureType tex_type) {
  Texture t = {};
  std::string f = std::string(filename);
  t.path = f.substr(f.find_last_of("/") + 1).c_str();

  t.type = tex_type;

  int w, h, nch;
  stbi_uc *data = stbi_load(filename, &w, &h, &nch, 4);

  if (data) {
    LogInfo("Loading texture: %s", filename);

    sg_image_desc desc = {};
    desc.type = SG_IMAGETYPE_2D;
    desc.width = w;
    desc.height = h;
    desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    desc.data.subimage[0][0].ptr = data;
    desc.data.subimage[0][0].size = w * h * 4;

    t.img = sg_make_image(&desc);

    // loaded = true;
  } else {
    LogError("Texture loading error %s file: %s", stbi_failure_reason(),
             filename);
  }

  sg_sampler_desc desc = {};
  desc.min_filter = texture_info.min_filter;
  desc.mag_filter = texture_info.max_filter;
  desc.wrap_u = texture_info.wrap_u;
  desc.wrap_v = texture_info.wrap_v;
  t.smp = sg_make_sampler(&desc);

  stbi_image_free(data);

  return t;
}

void initTextures() { addTexture("assets/default.png"); }

void setTextureFilter(sg_filter min, sg_filter max) {
  texture_info.min_filter = min;
  texture_info.max_filter = max;
}

void setTextureWrap(sg_wrap u, sg_wrap v) {
  texture_info.wrap_u = u;
  texture_info.wrap_v = v;
}

void setTextureVerticalFlip(bool flip) {
  stbi_set_flip_vertically_on_load(flip);
}

void addTexture(const char *path) {
  Texture tex = loadTexture(path);

  std::string id = getTextureIDFromPath(path);
  texture_info.hash.insert({id, tex});
}

const std::string getTextureIDFromPath(const std::string &path) {
  std::string id = path;
  id = id.substr(id.find_last_of("/") + 1);
  id = id.substr(0, id.find_first_of(".png"));
  // id = id.substr(0, id.find_first_of(".bmp"));

  // LogInfo("antes: %s depois: %s", path.c_str(), id.c_str());

  return id;
}

Texture &getTextureFromID(const std::string &id) {
  if (!texture_info.hash.contains(id)) {
    LogError("Texture ID not founded in hash map: %s", id.c_str());
    return texture_info.hash.at("default");
  }
  return texture_info.hash.at(id);
}

bool isTexture(const std::string &id) { 
  return texture_info.hash.contains(id);
}

void destroyTexture(std::string id) {
  Texture &t = texture_info.hash.at(id);
  t.destroy();
  texture_info.hash.erase(id);
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<u16> indices,
           std::vector<std::string> textures_path)
    : m_vertices(vertices), m_indices(indices), m_textures_path(textures_path) {
  assert(m_vertices.size() != 0);

  sg_buffer_desc vbuf_desc = {};
  vbuf_desc.data.ptr = m_vertices.data();
  vbuf_desc.data.size = m_vertices.size() * sizeof(Vertex);

  m_vbo = sg_make_buffer(&vbuf_desc);

  sg_buffer_desc ibuf_desc = {};
  ibuf_desc.usage.index_buffer = true;
  ibuf_desc.data.ptr = m_indices.data();
  ibuf_desc.data.size = m_indices.size() * sizeof(u16);

  m_ebo = sg_make_buffer(&ibuf_desc);
}

Mesh::Mesh(const Mesh &other)
    : m_vertices(other.m_vertices), m_indices(other.m_indices),
      m_textures_path(other.m_textures_path), m_vbo(other.m_vbo),
      m_ebo(other.m_ebo) {}

Mesh &Mesh::operator=(const Mesh &other) {
  Mesh tmp(other);
  std::swap(m_vertices, tmp.m_vertices);
  std::swap(m_indices, tmp.m_indices);
  std::swap(m_textures_path, tmp.m_textures_path);
  m_vbo = other.m_vbo;
  m_ebo = other.m_ebo;
  return *this;
}

Mesh::Mesh(Mesh &&other) noexcept
    : m_vertices(other.m_vertices), m_indices(other.m_indices),
      m_textures_path(other.m_textures_path), m_vbo(other.m_vbo),
      m_ebo(other.m_ebo) {}

Mesh &Mesh::operator=(Mesh &&other) noexcept {
  Mesh tmp(other);
  std::swap(m_vertices, tmp.m_vertices);
  std::swap(m_indices, tmp.m_indices);
  std::swap(m_textures_path, tmp.m_textures_path);
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

void Mesh::bind(u16 img, u16 smp) {
  sg_bindings bind = {};
  bind.vertex_buffers[0] = m_vbo;
  bind.index_buffer = m_ebo;

  Texture &t = getTextureFromID(getTextureIDFromPath(m_textures_path[0]));
  bind.images[img] = t.img;
  bind.samplers[smp] = t.smp;

  sg_apply_bindings(&bind);
}
