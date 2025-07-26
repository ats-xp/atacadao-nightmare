#include "game.hpp"

Game::Game() { LogInfo("Game created"); }

Game::~Game() { LogInfo("Game deleted"); }

void Game::update() {}

void Game::render() {}

void Game::handleInput(Input &inp) {
  if (inp.up) LogInfo("ola");
};
