#include "dummy.hpp"

#include "player.hpp"
#include "bullet.hpp"

DummyObject::DummyObject(const char *texture, PhysicsManager &mgr) {
  m_sprite = new Billboard(texture);

  physx::PxExtendedVec3 pos(-40.0f, 16.0f, 157.0f);
  m_control = PhysicsGameObject::create<physx::PxBoxControllerDesc>(
      mgr, 32.0f, 56.0f, 32.0f, pos);
  m_control->setPosition(pos);
}

DummyObject::~DummyObject() {
  delete m_sprite;
}

void DummyObject::update(f32 dt) {
  bool ground = PhysicsGameObject::step(m_control, dt, m_vel);
  if (!ground)
    PhysicsGameObject::updateGravity(dt, m_vel);

  m_pos = pxToGlmExtendedVec3(m_control->getPosition());
  m_sprite->setPosition(m_pos);
}

void DummyObject::draw(Camera &cam) { m_sprite->draw(cam); }

void DummyObject::chaseTarget(Player &p) {
  f32 xx = p.m_pos.x - m_pos.x;
  f32 zz = p.m_pos.z - m_pos.z;

  f32 a = std::atan2(zz, xx);
  f64 dist = sqrtf(xx * xx + zz * zz);

  if (dist >= 160) {
    m_vel.x = physx::PxCos(a) * 220.0f;
    m_vel.z = physx::PxSin(a) * 220.0f;
  } else {
    m_vel.x = 0.0f;
    m_vel.z = 0.0f;
  }
}
