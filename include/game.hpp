#pragma once

#include "state.hpp"

class Game : public State {
public:
  Game();
  ~Game();

  void update() override;
  void render() override;
  void handleInput(Input &inp) override;
};
