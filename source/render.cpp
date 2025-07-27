#include "render.hpp"

Render::~Render() {
  sg_destroy_shader(m_shader);
  sg_destroy_pipeline(m_pipe);
}

void Render::init(sg_pipeline_desc &pip_desc, const sg_shader_desc *shd_desc) {
  m_shader = sg_make_shader(shd_desc);

  pip_desc.shader = m_shader;

  m_pipe = sg_make_pipeline(&pip_desc);
}

void Render::use() { sg_apply_pipeline(m_pipe); }

RenderBuffer::RenderBuffer(std::vector<Vertex> vertices,
                           std::vector<u16> indices,
                           std::vector<Texture> textures) {
  sg_buffer_desc vbuf_desc = {};
  vbuf_desc.data.ptr = vertices.data();
  vbuf_desc.data.size = vertices.size() * sizeof(Vertex);

  sg_buffer_desc ibuf_desc = {};
  ibuf_desc.usage.index_buffer = true;
  ibuf_desc.data.ptr = indices.data();
  ibuf_desc.data.size = indices.size() * sizeof(u16);

  m_bind = {};
  m_bind.vertex_buffers[0] = sg_make_buffer(&vbuf_desc);
  m_bind.index_buffer = sg_make_buffer(&ibuf_desc);
}

RenderBuffer ::~RenderBuffer() { destroy(); }
