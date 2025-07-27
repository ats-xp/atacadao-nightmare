#pragma once

#include <memory>
#include <glm.hpp>

#include "camera.hpp"
#include "shape.hpp"
#include "input.hpp"

class Player {
  glm::vec3 m_pos;
  glm::vec3 m_vel;

public:
  std::shared_ptr<Shape> m_cube;

  Player(glm::vec3 pos);
  ~Player();

  void update(Input &inp);
  void render(Camera &cam);

  inline void setPos(glm::vec3 pos) {m_pos = pos;}
};
