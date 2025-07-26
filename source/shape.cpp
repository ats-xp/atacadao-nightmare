#include "shape.hpp"

#include "shape.glsl.h"

#include "base.h"

Shape::Shape(glm::vec3 pos) {
  m_model = glm::mat4(1.0f);

  f32 vertices[1024];
  u16 indices[1024];

  sshape_buffer_t buf = {};
  buf.vertices.buffer = SSHAPE_RANGE(vertices);
  buf.indices.buffer = SSHAPE_RANGE(indices);

  buf = create(buf);

  assert(buf.valid);

  m_elm = sshape_element_range(&buf);

  sg_buffer_desc vdesc = sshape_vertex_buffer_desc(&buf);
  sg_buffer_desc idesc = sshape_index_buffer_desc(&buf);

  m_bind.vertex_buffers[0] = sg_make_buffer(&vdesc);
  m_bind.index_buffer = sg_make_buffer(&idesc);

  m_shader = sg_make_shader(shape_shader_desc(sg_query_backend()));

  sg_pipeline_desc desc = {};
  desc.shader = m_shader;
  desc.layout.buffers[0] = sshape_vertex_buffer_layout_state();
  desc.layout.attrs[ATTR_shape_apos] = sshape_position_vertex_attr_state();
  desc.layout.attrs[ATTR_shape_acolor] = sshape_color_vertex_attr_state();
  desc.index_type = SG_INDEXTYPE_UINT16;
  desc.cull_mode = SG_CULLMODE_BACK;
  desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
  desc.depth.write_enabled = true;

  m_pipe = sg_make_pipeline(&desc);
}

Shape::~Shape() {
  // sg_destroy_buffer(m_bind.index_buffer);
  // sg_destroy_buffer(m_bind.vertex_buffers[0]);

  // sg_destroy_shader(m_shader);
  // sg_destroy_pipeline(m_pipe);
}

void Shape::draw(glm::mat4 pv) {
  sg_apply_pipeline(m_pipe);
  sg_apply_bindings(&m_bind);

  m_model = glm::mat4(1.0f);
  m_model = glm::translate(m_model, glm::vec3(0.0f, 0.0f, 0.0f));
  rotate(m_rot += 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));

  vs_params_shape_t vs_params = {};
  vs_params.mvp = pv * m_model;
  sg_apply_uniforms(UB_vs_params_shape, SG_RANGE_REF(vs_params));

  sg_draw(m_elm.base_element, m_elm.num_elements, 1);
}
