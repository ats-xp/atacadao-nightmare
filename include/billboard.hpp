#pragma once

#include "mesh.hpp"

class Billboard {
  Mesh *m_mesh{nullptr};

public:
  glm::vec3 m_pos{0.0f, 0.0f, 0.0f};

  Billboard(const char *texture);
  ~Billboard();

  void draw(Camera &cam);

  constexpr void setPosition(const glm::vec3 &pos) { m_pos = pos; }
};
