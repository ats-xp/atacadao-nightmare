#include "player.hpp"

Player::Player(glm::vec3 pos) : m_pos(pos) {
  // m_cube = std::make_shared<Shape>();
}

Player::~Player() {

}

void Player::update(Input &inp) {
  //
  // if (inp.up) 

  // m_cube->setPos(m_pos);
  // m_pos += m_vel;
}

void Player::render(Camera &cam) {
  // m_cube->draw(cam);
}
