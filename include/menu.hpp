#pragma once

#include "state.hpp"

#include "fontstash.h"

class Menu : public State {
  FONScontext *m_font_ctx;
  int m_font_daydream;

public:
  Menu();
  ~Menu();

  void update(f32 dt) override;
  void render() override;
};
