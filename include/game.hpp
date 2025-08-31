#pragma once

#include <array>

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

class Game : public State {
  Render m_render;
  Render m_render_bb; // billboard
  Render m_render_sp; // Shape

  Player *m_player;
  std::vector<GameObject*> m_objects;

  Map m_map;

  Camera m_cam;

  FONScontext *m_font_ctx;
  int m_font_normal;

  f32 m_mouse_x = 0;
  f32 m_mouse_y = 0;

  PhysicsManager m_physics;

  void initPipeline();
  void initTextures();

public:
  Game();
  ~Game();

  void update(f32 dt) override;
  void render() override;
  void handleEvent(const sapp_event *e) override;
};
