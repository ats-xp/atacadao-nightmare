#pragma once

#include "state.hpp"

class Menu : public State {
public:
  Menu();
  ~Menu();

  void update() override;
  void render() override;
  void handleInput(Input &inp) override;
};
