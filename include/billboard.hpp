#pragma once

#include "mesh.hpp"

class Billboard {
  glm::vec3 m_position{0.0f, 0.0f, 0.0f};
  Mesh *m_mesh{nullptr};

public:
  Billboard(const char *texture);
  ~Billboard();

  void draw(Camera &cam);

  constexpr void setPosition(const glm::vec3 &pos) { m_position = pos; }
};
