#include "player.hpp"
#include "bullet.hpp"

#include <SDL2/SDL.h>

#include "physx_utils.hpp"

#include "shape.glsl.h"

constexpr int bullet_cooldown = 20;

Player::Player(const glm::vec3 &pos) {
  m_bullets.reserve(16);
  m_bullet_count = bullet_cooldown;

  m_pos = pos;
  m_vel = physx::PxVec3(0.0f);
  m_speed = 320.0f;
}

Player::~Player() {
  if (m_control) {
    m_control->release();
    m_control = nullptr;
  }
}

void Player::update(f32 dt) {
  const u8 *keys = SDL_GetKeyboardState(nullptr);
  glm::vec3 input(0.0f);

  if (keys[SDL_SCANCODE_Z] && m_bullet_count-- <= 0) {
    glm::vec3 pos = m_pos;
    pos.z += m_cam_front.z * 8.0f;
    glm::vec3 vel(m_cam_front);
    vel *= 15.0f;
    m_bullets.push_back(new Bullet(pos, vel));

    m_bullet_count = bullet_cooldown;
  }

  if (keys[SDL_SCANCODE_W]) {
    input.x += m_cam_front.x;
    input.z += m_cam_front.z;
  } else if (keys[SDL_SCANCODE_S]) {
    input.x -= m_cam_front.x;
    input.z -= m_cam_front.z;
  }

  if (keys[SDL_SCANCODE_A]) {
    input.x -= m_cam_right.x;
    input.z -= m_cam_right.z;
  } else if (keys[SDL_SCANCODE_D]) {
    input.x += m_cam_right.x;
    input.z += m_cam_right.z;
  }

  if (glm::length(input) > 0.0f) {
    input = glm::normalize(input);
  }

  input *= m_speed;

  m_vel.x = input.x;
  m_vel.z = input.z;

  bool ground = PhysicsGameObject::step(m_control, dt, m_vel);
  if (!ground)
    PhysicsGameObject::updateGravity(dt, m_vel);

  if (keys[SDL_SCANCODE_SPACE] && ground) {
    m_vel.y = 320.0f;
    ground = false;
  }

  m_pos = pxToGlmExtendedVec3(m_control->getPosition());

  // Transform trans;
  // trans.position = m_pos;

  for (size_t i = 0; i < m_bullets.size(); i++) {
    Bullet *b = m_bullets.at(i);
    b->update(dt);

    if (b->checkDestruction(*this)) {
      delete b;
      m_bullets.erase(m_bullets.begin() + i);
      // LogError("morreu");
    }
  }
}

void Player::draw(Camera &cam) {
  cam.setPosition(m_pos);
  m_cam_front = cam.getFront();
  m_cam_right = cam.getRight();

  for (Bullet *b : m_bullets) {
    b->draw(cam);
  }
}

void Player::drawDebug(Camera &cam) {
  // physx::PxTransform pose = m_collider->getGlobalPose();
  // glm::mat4 model = pxToGlmMat4(pose);
  //
  // vs_params_shape_t vs_params = {};
  // vs_params.mvp = cam.getMatrix() * model;
  //
  // m_collider_shape->bind();
  // sg_apply_uniforms(UB_vs_params_shape, SG_RANGE_REF(vs_params));
  // m_collider_shape->draw(cam);
}
