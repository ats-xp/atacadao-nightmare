#pragma once

#include "sokol_gfx.h"

#include "mesh.hpp"

struct Render {
  ~Render();

  void init(sg_pipeline_desc &pip_desc, const sg_shader_desc *shd_desc);
  void use();

private:
  sg_pipeline m_pipe;
  sg_shader m_shader;
};
