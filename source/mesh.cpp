#include "sokol_fetch.h"
#include "sokol_gfx.h"
#include "stb_image.h"

#include "mesh.hpp"

#include "asset_manager.hpp"

#include <xatlas.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<u16> indices,
           std::vector<std::string> textures_path)
    : m_vertices(vertices), m_indices(indices), m_textures_path(textures_path) {
  assert(m_vertices.size() != 0);

  xatlas::Atlas *atlas = xatlas::Create();
  xatlas::MeshDecl desc;
  desc.vertexCount = m_vertices.size();

  desc.vertexPositionData = &m_vertices[0].pos;
  desc.vertexPositionStride = sizeof(Vertex);

  desc.indexCount = m_indices.size();
  desc.indexData = m_indices.data();
  desc.indexFormat = xatlas::IndexFormat::UInt16;

  xatlas::AddMesh(atlas, desc);
  xatlas::Generate(atlas);

  const xatlas::Mesh &out_mesh = atlas->meshes[0];
  for (u32 i = 0; i < out_mesh.indexCount; i++) {
    u16 v = out_mesh.indexArray[i];
    const xatlas::Vertex &vx = out_mesh.vertexArray[v];

    m_vertices[m_indices[i]].lightmap_coords = m_vertices[m_indices[i]].tex_coords;
    // m_vertices[m_indices[i]].lightmap_coords.y = vx.uv[1] / atlas->height;
  }

  xatlas::Destroy(atlas);

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

  Texture &t = AssetManager::getTextureFromID(AssetManager::getTextureIDFromPath(m_textures_path[0]));

  bind.images[img] = t.img;
  bind.samplers[smp] = t.smp;

  sg_apply_bindings(&bind);
}
