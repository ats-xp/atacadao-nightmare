#include "player.hpp"

#include "shape.glsl.h"

#include "input.hpp"
#include "physx_utils.hpp"

Player::Player(const glm::vec3 &pos) {
  // Transform trans;
  // trans.position = pos;
  // trans.scale = glm::vec3(32 / 1.8f);

  m_pos = pos;
  m_vel = physx::PxVec3(0.0f);
  m_speed = 240.0f;
}

Player::~Player() {
  if (m_control) {
    m_control->release();
    m_control = nullptr;
  }
}

void Player::update(f32 dt) {
  glm::vec3 input(0.0f);

  if (inp.up) {
    input += m_cam_front;
  } else if (inp.down) {
    input -= m_cam_front;
  }

  if (inp.left) {
    input -= m_cam_right;
  } else if (inp.right) {
    input += m_cam_right;
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

  if (inp.jump && ground) {
    m_vel.y = 320.0f;
    ground = false;
  }

  m_pos = pxToGlmExtendedVec3(m_control->getPosition());

  // Transform trans;
  // trans.position = m_pos;

  // m_model->setTransformitions(trans);
}

void Player::draw(Camera &cam) {
  // m_model->draw(cam);
  cam.setPosition(m_pos);
  m_cam_front = cam.getFront();
  m_cam_right = cam.getRight();
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
