#include "bullet.hpp"
#include "player.hpp"

Bullet::Bullet(const glm::vec3 &pos, const glm::vec3 &vel) {
  m_sprite = new Billboard("bullet");
  m_pos = pos;
  m_vel = vel;
}

Bullet::~Bullet() { delete m_sprite; }

void Bullet::update(f32 dt) {
  m_pos += m_vel;
  m_sprite->setPosition(m_pos);
}

void Bullet::draw(Camera &cam) { m_sprite->draw(cam); }

bool Bullet::checkDestruction(const Player &p) {
  f32 xx = p.m_pos.x - m_pos.x;
  f32 zz = p.m_pos.z - m_pos.z;

  f32 a = std::atan2(zz, xx);
  f64 dist = sqrtf(xx * xx + zz * zz);

  if (dist > 600) {
    return true;
  }

  return false;
}
