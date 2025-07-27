#pragma once

#include "sokol_gfx.h"

#include "mesh.hpp"

struct Render {
  Render() = default;
  ~Render();

  void init(sg_pipeline_desc &pip_desc, const sg_shader_desc *shd_desc);
  void use();

private:
  sg_pipeline m_pipe;
  sg_shader m_shader;
};

class RenderBuffer {
  sg_bindings m_bind;

public:
  RenderBuffer(std::vector<Vertex> v, std::vector<u16> i,
               std::vector<Texture> t);
  ~RenderBuffer();

  inline void use() {
    sg_apply_bindings(&m_bind);
  }

  inline void setImage(u32 img, u32 smp, Texture &t) {
    m_bind.images[img].id = t.id;
    m_bind.samplers[smp] = t.smp;
  }

  inline void destroy() {
    sg_destroy_buffer(m_bind.index_buffer);
    sg_destroy_buffer(m_bind.vertex_buffers[0]);
  }
};
