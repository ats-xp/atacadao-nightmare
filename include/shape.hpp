#pragma once

#include "sokol_gfx.h"
#include "sokol_shape.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "base.h"

#include "camera.hpp"
#include "mesh.hpp"

enum ShapeType : u8 {
  BOX = 0,
  PLANE,
};

//
// MAYBE:
// separar os formatos por classes
//
class ShapeInterface {
public:
  virtual sshape_buffer_t create(sshape_buffer_t &buf) = 0;
};

class Shape : public ShapeInterface {
  sshape_element_range_t m_elm;

  glm::mat4 m_model;

  sg_buffer m_vbo, m_ebo;

public:
  Transform m_trans;
  ShapeType m_type;

  Shape(const glm::vec3 &pos, const glm::vec3 &size, const ShapeType &type = BOX);
  ~Shape();

  sshape_buffer_t create(sshape_buffer_t &buf) override {
    sshape_box_t b = {};
    b.width = 2.0f;
    b.height = 2.0f;
    b.depth = 2.0f;
    b.tiles = 1;
    b.random_colors = true;
    return sshape_build_box(&buf, &b);
  };

  void bind() {
    sg_bindings bind = {};

    bind.vertex_buffers[0] = m_vbo;
    bind.index_buffer = m_ebo;

    sg_apply_bindings(&bind);
  }

  void draw(Camera &cam);
};
