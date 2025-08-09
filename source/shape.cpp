#include "shape.hpp"

#include "shape.glsl.h"

#include "base.h"

Shape::Shape(const glm::vec3 &pos, const glm::vec3 &size)  {
  m_trans.position = pos;

  f32 vertices[1024];
  u16 indices[1024];

  sshape_buffer_t buf = {};
  buf.vertices.buffer = SSHAPE_RANGE(vertices);
  buf.indices.buffer = SSHAPE_RANGE(indices);

  // buf = create(buf);
  sshape_box_t b = {};
  b.width = size.x;
  b.height = size.y;
  b.depth = size.z;
  b.tiles = 1;
  b.random_colors = true;
  buf = sshape_build_box(&buf, &b);

  assert(buf.valid);

  m_elm = sshape_element_range(&buf);

  sg_buffer_desc vdesc = sshape_vertex_buffer_desc(&buf);
  sg_buffer_desc idesc = sshape_index_buffer_desc(&buf);

  m_vbo = sg_make_buffer(&vdesc);
  m_ebo = sg_make_buffer(&idesc);
}

Shape::~Shape() {
}

void Shape::draw(Camera &cam) {
  // bind ...
  sg_draw(m_elm.base_element, m_elm.num_elements, 1);
}
