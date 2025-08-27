#pragma once

 struct Input {
  bool action;
  bool jump;
  bool b;

  bool up;
  bool down;
  bool left;
  bool right;

  bool inc, dec; // debug
};

extern Input inp;
