#pragma once

#include <glm.hpp>
#include <memory>

#include "camera.hpp"
#include "input.hpp"
#include "model.hpp"
#include "shape.hpp"

#undef S
#include <PxPhysicsAPI.h>

class Player {
public:
  Model *m_model;
  physx::PxRigidDynamic *m_collider;
  Shape *m_collider_shape;
  bool is_ground;

  glm::vec3 m_cam_front;
  glm::vec3 m_cam_right;

  glm::vec3 m_pos;
  glm::vec3 m_vel;
  f32 m_speed;

  Player(const glm::vec3 &pos);
  ~Player();

  void input(Input &inp);
  void update(f32 dt);
  void draw(Camera &cam);
  void drawDebug(Camera &cam);

  void initPhysics(physx::PxPhysics *physics, physx::PxMaterial *material,
                   physx::PxScene *scene);
  bool isOnGround(physx::PxScene *scene);
  void jump(f32 strength);

  constexpr void setPos(const glm::vec3 &pos) { m_pos = pos; }
};
