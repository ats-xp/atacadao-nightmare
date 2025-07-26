#pragma once

#include <memory>

#include "base.h"

#include "input.hpp"

enum StateId : u8 {
  OFF = 0,
  MENU,
  GAME,
  STATE_IDS,
};

class StateInterface {
public:
  virtual void update() = 0;
  virtual void render() = 0;
  virtual void handleInput(Input &inp) = 0;
};

class State : public StateInterface {

public:
  static bool m_change;
  u8 m_next{StateId::OFF};

  State() { LogInfo("State created"); }

  virtual ~State() { LogInfo("State deleted"); }

  void update() override {}
  void render() override {}
  void handleInput(Input &inp) override {};

  inline void setNext(u8 n) { m_next = n; };

  inline const u8 &getNext() { return m_next; };
};
