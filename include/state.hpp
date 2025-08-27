#pragma once

#include <memory>

#include "base.h"

#include "sokol_app.h"

enum StateId : u8 {
  OFF = 0,
  MENU,
  GAME,
  STATE_IDS,
};

class StateInterface {
public:
  virtual void update(f32 dt) = 0;
  virtual void render() = 0;
  virtual void handleEvent(const sapp_event *e) = 0;
};

class State : public StateInterface {

public:
  static bool m_change;
  u8 m_next{StateId::OFF};

  State() { LogInfo("State created"); }

  virtual ~State() { LogInfo("State deleted"); }

  void update(f32 dt) override {}
  void render() override {}
  void handleEvent(const sapp_event *e) override {};

  inline void setNext(u8 n) { m_next = n; };

  inline const u8 &getNext() { return m_next; };
};
