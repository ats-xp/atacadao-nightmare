#pragma once

#include "state.hpp"

class Menu : public State {
public:
  Menu();
  ~Menu();

  void update(f32 dt, Input &inp) override;
  void render() override;
};
