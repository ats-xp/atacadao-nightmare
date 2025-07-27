#pragma once

#include "sokol_gfx.h"
#include "sokol_shape.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "base.h"

#include "camera.hpp"

//
// MAYBE:
// separar os formatos por classes
//
class ShapeInterface {
public:
  virtual sshape_buffer_t create(sshape_buffer_t &buf) = 0;
};

class Shape : public ShapeInterface {
  sg_pipeline m_pipe{};
  sg_bindings m_bind{};
  sg_shader m_shader;
  sshape_element_range_t m_elm;

  glm::mat4 m_model;

public:
  glm::vec3 m_pos;

  Shape(glm::vec3 pos = glm::vec3(0.0f));
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

  void draw(Camera &cam);

  inline void rotate(f32 degrees, glm::vec3 axis) {
    m_model = glm::rotate(m_model, glm::radians(degrees), axis);
  }

  inline void setPos(glm::vec3 pos) { m_pos = pos; }
};
