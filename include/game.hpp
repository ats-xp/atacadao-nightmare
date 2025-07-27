#pragma once

#include "state.hpp"

#include "sokol_memtrack.h"

#include "camera.hpp"
#include "player.hpp"
#include "model.hpp"
#include "render.hpp"

class Game : public State {
  std::shared_ptr<Camera> m_cam;
  std::shared_ptr<Player> m_player;

  std::vector<std::shared_ptr<Model>> mdls;

  Render m_render;

  smemtrack_info_t m_info;

  f32 m_mouse_x = 0;
  f32 m_mouse_y = 0;

public:
  Game();
  ~Game();

  void update(f32 dt, Input &inp) override;
  void render() override;
  void handleEvent(const sapp_event *e) override;
};
