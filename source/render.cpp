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
