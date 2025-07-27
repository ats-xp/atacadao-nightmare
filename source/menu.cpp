#include "menu.hpp"

Menu::Menu() { LogInfo("Menu created"); }

Menu::~Menu() { LogInfo("Menu deleted"); }

void Menu::update(f32 dt, Input &inp) {
  if (inp.action) setNext(StateId::GAME);
};

void Menu::render() {}

