#include "player.hpp"

#include "shape.glsl.h"

Player::Player(const glm::vec3 &pos) {
  Transform trans;
  trans.position = pos;

  m_pos = pos;
  m_vel = glm::vec3(0.0f);
  m_speed = 3.0f;

  m_model = new Model("assets/models/Beat/Beat.obj");
  m_model->setTransformitions(trans);
}

Player::~Player() {
  delete m_model;
}

void Player::input(Input &inp) {
  // m_vel = glm::vec3(0.0f);
  m_vel.x = 0.0f;
  m_vel.z = 0.0f;

  if (inp.up) {
    m_vel.x = m_cam_front.x;
    m_vel.z = m_cam_front.z;
  }
  else if (inp.down) {
    m_vel.x = -m_cam_front.x;
    m_vel.z = -m_cam_front.z;
  }

  if (inp.left) {
    m_vel.x = -m_cam_right.x;
    m_vel.z = -m_cam_right.z;
  }
  else if (inp.right)  {
    m_vel.x = m_cam_right.x;
    m_vel.z = m_cam_right.z;
  }
}

void Player::update(f32 dt) {
  glm::vec3 horz_vel(m_vel.x, 0.0f, m_vel.z);
  m_pos += horz_vel * m_speed * dt;

  m_pos.y += m_vel.y * dt;

  Transform trans;
  trans.position = m_pos;

  // m_model->setTransformitions(trans);
}

void Player::draw(Camera &cam) {
  m_model->draw(cam);
  cam.setPosition(m_pos);
  m_cam_front = cam.getFront();
  m_cam_right = cam.getRight();
}

void Player::drawDebug(Camera &cam) {

}
