#include "render.hpp"

void VAO::create(sg_pipeline_desc &pip_desc, const sg_shader_desc *shd_desc) {
  shader = sg_make_shader(shd_desc);
  pip_desc.shader = shader;
  pipe = sg_make_pipeline(&pip_desc);
}

void VAO::use() {
  sg_apply_pipeline(pipe);
}

void VAO::destroy() {
  sg_destroy_shader(shader);
  sg_destroy_pipeline(pipe);
}

void VBO::create(const void *vertices, int vsize) {
  sg_buffer_desc vbuf_desc = {};
  vbuf_desc.data.ptr = vertices;
  vbuf_desc.data.size = vsize;
  buffer = sg_make_buffer(&vbuf_desc);
}

void VBO::destroy() { sg_destroy_buffer(buffer); }
sg_buffer &VBO::getBuffer() { return buffer; }

void EBO::create(const void *indices, int isize) {
  sg_buffer_desc ibuf_desc = {};
  ibuf_desc.usage.index_buffer = true;
  ibuf_desc.data.ptr = indices;
  ibuf_desc.data.size = isize;
  buffer = sg_make_buffer(&ibuf_desc);
}

void EBO::destroy() { sg_destroy_buffer(buffer); }
sg_buffer &EBO::getBuffer() { return buffer; }
