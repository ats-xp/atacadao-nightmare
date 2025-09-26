#pragma once

#include "game_object.hpp"

class Player;

class Bullet : public GameObject {
public:
  glm::vec3 m_vel;

  Bullet(const glm::vec3 &pos, const glm::vec3 &vel);
  virtual ~Bullet();

  void update(f32 dt) final;
  void draw(Camera &cam) final;

  bool checkDestruction(const Player &p);
};
