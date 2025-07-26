#include "menu.hpp"

Menu::Menu() { LogInfo("Menu created"); }

Menu::~Menu() { LogInfo("Menu deleted"); }

void Menu::update() {}

void Menu::render() {}

void Menu::handleInput(Input &inp) {
  if (inp.action) setNext(StateId::GAME);
};
