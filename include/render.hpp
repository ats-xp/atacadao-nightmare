#pragma once

#include "sokol_gfx.h"

struct VBO {
  void create(const void *vertices, int vsize);
  void destroy();
  sg_buffer &getBuffer();

private:
  sg_buffer buffer = {};
};

struct EBO {
  void create(const void *indices, int isize);
  void destroy();
  sg_buffer &getBuffer();

private:
  sg_buffer buffer = {};
};

struct VAO {
  void create(sg_pipeline_desc &pip_desc, const sg_shader_desc *shd_desc);
  void bindBuffers(VBO &vbo, EBO &ebo);
  void bindTexture(int slot, const sg_image &img, const sg_sampler &smp);
  void use();
  void destroy();

private:
  sg_pipeline pipe = {};
  sg_shader shader = {};
};
