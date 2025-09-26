#pragma once

#include "base.h"

class SpriteAnimationManager {
  public:

  SpriteAnimationManager();
  ~SpriteAnimationManager() {}

  void update();
};

class SpriteAnimator {
  u32 m_sheet_w;
  u32 m_sheet_h;

  public:

  SpriteAnimator(u32 sheet_w, u32 sheet_h);
  ~SpriteAnimator() {};
};
