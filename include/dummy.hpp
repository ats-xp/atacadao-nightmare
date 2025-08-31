#pragma once

#include "game_object.hpp"

class DummyObject : public GameObject {
public:
  physx::PxController *m_control = nullptr;
  physx::PxVec3 m_vel = physx::PxVec3(0.0f);

  DummyObject(const char *texture, PhysicsManager &mgr) {
    m_sprite = new Billboard(texture);

    physx::PxExtendedVec3 pos(-40.0f, 16.0f, 157.0f);
    m_control = PhysicsGameObject::create<physx::PxCapsuleControllerDesc>(
        mgr, 56.0f - 2 * 16, 32.0f / 2, pos);
    m_control->setPosition(pos);
  }

  ~DummyObject() {}

  void update(f32 dt) final {
    bool ground = PhysicsGameObject::step(m_control, dt, m_vel);
    if (!ground)
      PhysicsGameObject::updateGravity(dt, m_vel);

    m_pos = pxToGlmExtendedVec3(m_control->getPosition());
    m_sprite->setPosition(m_pos);
  }
  void draw(Camera &cam) final { m_sprite->draw(cam); }
};
