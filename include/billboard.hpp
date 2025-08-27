#pragma once

#include "mesh.hpp"

#define MUL_X 32
#define MUL_Y 56
#define MUL_Z 32

class Billboard {
  Mesh *m_mesh{nullptr};

public:
  glm::vec3 m_pos{0.0f, 0.0f, 0.0f};

  Billboard(const char *texture);
  ~Billboard();

  void draw(Camera &cam);

  constexpr void setPosition(const glm::vec3 &pos) { m_pos = pos; }
};
