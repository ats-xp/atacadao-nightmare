#pragma once

#include <glm.hpp>
#include <memory>

#include "camera.hpp"
#include "model.hpp"
#include "shape.hpp"

#undef S
#include <PxPhysicsAPI.h>

class Player {
public:
  Model *m_model;
  // physx::PxRigidDynamic *m_collider;
  Shape *m_collider_shape;
  bool is_ground;

  physx::PxController *m_control;

  glm::vec3 m_cam_front;
  glm::vec3 m_cam_right;

  glm::vec3 m_pos;
  glm::vec3 m_vel;
  f32 m_speed;

  Player(const glm::vec3 &pos);
  ~Player();

  void update(f32 dt);
  void draw(Camera &cam);
  void drawDebug(Camera &cam);

  void initPhysics(physx::PxPhysics *physics, physx::PxControllerManager *mgr);
  bool isOnGround(physx::PxScene *scene);
  bool snapToGround(physx::PxScene *scene, f32 snap_distance);
  void jump(f32 strength);

  constexpr void setPos(const glm::vec3 &pos) { m_pos = pos; }
};
