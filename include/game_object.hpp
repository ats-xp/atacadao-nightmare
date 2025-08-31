#pragma once

#include "physics_manager.hpp"

#include "billboard.hpp"
#include "camera.hpp"
#include "physx_utils.hpp"

class GameObjectInterface {
  public:
  virtual void update(f32 dt) = 0;
  virtual void draw(Camera &cam) = 0;
};

class GameObject : public GameObjectInterface {
  protected:
  Billboard *m_sprite;

  public:
  glm::vec3 m_pos;

  GameObject() {}
  virtual ~GameObject() {}

  void update(f32 dt) override {}
  void draw(Camera &cam) override {}
};
