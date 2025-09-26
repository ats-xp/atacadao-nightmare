#pragma once

#include <glm.hpp>
#include <memory>

#include "camera.hpp"
#include "model.hpp"
#include "shape.hpp"

#include "physics_manager.hpp"

#include "game_object.hpp"

class Bullet;

class Player : public GameObject {
public:
  // Model *m_model;
  std::vector<Bullet *> m_bullets;
  u32 m_bullet_count;

  physx::PxController *m_control;

  glm::vec3 m_cam_front;
  glm::vec3 m_cam_right;

  glm::vec3 m_pos;
  physx::PxVec3 m_vel;
  // glm::vec3 m_vel;

  f32 m_speed;

  Player(const glm::vec3 &pos);
  ~Player();

  void update(f32 dt) final;
  void draw(Camera &cam) final;
  void drawDebug(Camera &cam);
};
