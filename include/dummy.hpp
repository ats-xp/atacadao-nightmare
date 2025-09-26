#pragma once

#include "game_object.hpp"

class Player;

class DummyObject : public GameObject {
  u32 m_hp = 5;

public:
  physx::PxController *m_control = nullptr;
  physx::PxVec3 m_vel = physx::PxVec3(0.0f);

  DummyObject(const char *texture, PhysicsManager &mgr);
  ~DummyObject();

  void update(f32 dt) final;
  void draw(Camera &cam) final;

  void chaseTarget(Player &p);
};
