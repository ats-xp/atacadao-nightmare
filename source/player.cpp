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

  glm::vec3 p = m_pos;
  p -= 1.0f;
  m_collider.min = p;
  p = m_pos;
  p += 1.0f;
  m_collider.max = p;

  m_collider_shape = new Shape(m_pos, glm::vec3(1.0f));
}

Player::~Player() {
  delete m_model;
}

void Player::input(Input &inp) {
  m_vel = glm::vec3(0.0f);

  if (inp.up)
    m_vel = m_cam_front;
  else if (inp.down)
    m_vel = -m_cam_front;

  if (inp.left)
    m_vel = -m_cam_right;
  else if (inp.right)
    m_vel = m_cam_right;
}

void Player::update(f32 dt) {
  m_pos += m_vel * m_speed * dt;

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
  m_collider_shape->bind();

  glm::vec3 offset(0.0f, 0.0f, 0.0f);
  glm::vec3 min = (m_pos + offset) - m_collider.getHalfSize();
  glm::vec3 max = (m_pos + offset) + m_collider.getHalfSize();

  glm::vec3 size = max - min;
  glm::vec3 center = (max + min) * .5f;

  glm::mat4 m_model = glm::translate(glm::mat4(1.0f), center) *
                      glm::scale(glm::mat4(1.0f), size);

  vs_params_shape_t vs_params = {};
  vs_params.mvp = cam.getMatrix() * m_model;
  sg_apply_uniforms(UB_vs_params_shape, SG_RANGE_REF(vs_params));
  m_collider_shape->draw(cam);
}
