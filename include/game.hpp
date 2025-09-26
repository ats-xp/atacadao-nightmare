#pragma once

#include "state.hpp"

#include "fontstash.h"

#include "billboard.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "player.hpp"
#include "render.hpp"
#include "shape.hpp"
#include "map.hpp"
#include "physics_manager.hpp"

class DummyObject;

class Game : public State {
  Player *m_player;
  std::vector<DummyObject*> m_objects;

  Map m_map;

  Camera m_cam;

  FONScontext *m_font_ctx;
  int m_font_normal;

  f32 m_mouse_x = 0;
  f32 m_mouse_y = 0;

  PhysicsManager m_physics;

  void initTextures();

public:
  Game();
  ~Game();

  void update(f32 dt) override;
  void render() override;
  void handleEvent(const SDL_Event *e) override;
};
