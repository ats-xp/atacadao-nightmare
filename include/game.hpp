#pragma once

#include <array>

#include "state.hpp"

#include "base.h"

#include "fontstash.h"

#include "billboard.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "player.hpp"
#include "render.hpp"
#include "shape.hpp"

struct Collider {
  Shape *shape;
  AABB box;
};

class Game : public State {
  Render m_render;
  Render m_render_bb; // billboard
  Render m_render_sp; // Shape

  ModelStore m_mdl_store;
  f32 rot = 0.0f;

  std::array<Billboard *, 2> m_boards;

  std::shared_ptr<Camera> m_cam;
  std::shared_ptr<Player> m_player;

  std::vector<AABB> m_colliders;
  std::vector<Shape *> m_colliders_shape;

  FONScontext *m_font_ctx;
  int m_font_normal;

  f32 m_mouse_x = 0;
  f32 m_mouse_y = 0;

  void initPipeline();

public:
  Game();
  ~Game();

  void update(f32 dt, Input &inp) override;
  void render() override;
  void handleEvent(const sapp_event *e) override;
};
