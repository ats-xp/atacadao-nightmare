#pragma once

#include <memory>
#include <glm.hpp>

#include "camera.hpp"
#include "shape.hpp"
#include "input.hpp"
#include "model.hpp"
#include "physics.hpp"

class Player {
public:
  Model *m_model;

  glm::vec3 m_cam_front;
  glm::vec3 m_cam_right;

  glm::vec3 m_pos;
  glm::vec3 m_vel;
  f32 m_speed;

  Shape *m_collider_shape;
  AABB m_collider;

  Player(const glm::vec3 &pos);
  ~Player();

  void input(Input &inp);
  void update(f32 dt);
  void draw(Camera &cam);
  void drawDebug(Camera &cam);

  constexpr void setPos(const glm::vec3 &pos) {m_pos = pos;}
};
