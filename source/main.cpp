#include <memory>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "sokol_app.h"
#include "sokol_fetch.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "sokol_log.h"
#include "sokol_shape.h"

#include "fontstash.h"
#include "sokol_fontstash.h"
#include "sokol_gl.h"

#include "base.h"

#include "game.hpp"
#include "menu.hpp"
#include "shape.hpp"

static struct {
  /*
   * FIXME:
   * Classe State instancia junto com a classe
   * compartilhada
   */
  std::unique_ptr<State> st;

  Input input;
} state;

std::shared_ptr<Shape> cube;

static void init() {
  sg_desc desc = {};
  desc.environment = sglue_environment();
  desc.logger.func = slog_func;
  sg_setup(&desc);

  sgl_desc_t sgl_desc = {};
  sgl_desc.logger.func = slog_func;
  sgl_setup(&sgl_desc);

  state.st = std::make_unique<Menu>();
  cube = std::make_shared<Shape>();
}

static void frame() {
  u8 next_st = state.st->getNext();
  if (StateId::OFF != next_st) {
    state.st.reset();

    switch (next_st) {
    case StateId::MENU:
      state.st = std::make_unique<Menu>();
      break;
    case StateId::GAME:
      state.st = std::make_unique<Game>();
      break;
    }
  }

  state.st->handleInput(state.input);

  sg_pass_action action = {};
  action.colors[0].load_action = SG_LOADACTION_CLEAR;
  action.colors[0].clear_value = {0.0f, 0.0f, 0.0f, 0.0f};

  sg_pass pass = {};
  pass.action = action;
  pass.swapchain = sglue_swapchain();

  glm::mat4 proj = glm::perspective(
      glm::radians(45.0f), sapp_widthf() / sapp_heightf(), 0.1f, 100.0f);
  glm::mat4 view =
      glm::lookAt(glm::vec3(0.0f, 0.0f, 6.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f));

  sg_begin_pass(&pass);

  cube->draw(proj * view);

  sg_end_pass();
  sg_commit();
}

static void handleInput(const sapp_event *e) {
  auto &inp = state.input;

  if (e->type == SAPP_EVENTTYPE_KEY_DOWN || e->type == SAPP_EVENTTYPE_KEY_UP) {
    bool btn_down = e->type == SAPP_EVENTTYPE_KEY_DOWN;

    if (e->key_code == SAPP_KEYCODE_Z)
      inp.action = btn_down;

    if (e->key_code == SAPP_KEYCODE_UP)
      inp.up = btn_down;

    if (e->key_repeat)
      return;
  }
}

static void cleanup() {
  sgl_shutdown();
  sg_shutdown();
}

sapp_desc sokol_main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  sapp_desc desc = {};
  desc.init_cb = init;
  desc.frame_cb = frame;
  desc.cleanup_cb = cleanup;
  desc.event_cb = handleInput;
  desc.width = 640;
  desc.height = 480;
  desc.window_title = "Atacado - Nightmare";
  desc.icon.sokol_default = true;
  desc.logger.func = slog_func;
  desc.sample_count = 4;
  desc.gl_major_version = 3;
  desc.gl_minor_version = 3;

  return desc;
}
