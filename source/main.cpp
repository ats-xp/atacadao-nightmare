#include <memory>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "sokol_app.h"
#include "sokol_fetch.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "sokol_log.h"
#include "sokol_memtrack.h"
#include "sokol_shape.h"
#include "sokol_time.h"

#include "fontstash.h"
#include "sokol_fontstash.h"
#include "sokol_gl.h"

#include "base.h"

#include "camera.hpp"
#include "game.hpp"
#include "menu.hpp"
#include "player.hpp"
#include "shape.hpp"

static struct {
  /*
   * FIXME:
   * Classe State instancia junto com a classe
   * compartilhada
   */
  std::unique_ptr<State> st;

  Input input;

  u64 last_time;
  f64 delta_time;
} state;

static void init() {
  sg_desc desc = {};
  desc.environment = sglue_environment();
  desc.logger.func = slog_func;
  desc.allocator.alloc_fn = smemtrack_alloc,
  desc.allocator.free_fn = smemtrack_free, sg_setup(&desc);

  sgl_desc_t sgl_desc = {};
  sgl_desc.logger.func = slog_func;
  sgl_setup(&sgl_desc);

  sfetch_desc_t fdesc = {};
  fdesc.max_requests = 1;
  fdesc.num_channels = 1;
  fdesc.num_lanes = 1;
  sfetch_setup(&fdesc);

  stm_setup();

  state.last_time = 0;

  state.st = std::make_unique<Menu>();
  sapp_lock_mouse(true);
}

static void frame() {
  state.delta_time = stm_ms(stm_laptime(&state.last_time));

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

  state.st->update(static_cast<float>(state.delta_time), state.input);

  sg_pass_action action = {};
  action.colors[0].load_action = SG_LOADACTION_CLEAR;
  action.colors[0].clear_value = {0.0f, 0.0f, 0.0f, 0.0f};

  sg_pass pass = {};
  pass.action = action;
  pass.swapchain = sglue_swapchain();

  sg_begin_pass(&pass);

  state.st->render();

  sg_end_pass();
  sg_commit();
}

static void handleInput(const sapp_event *e) {
  Input &inp = state.input;

  state.st->handleEvent(e);

  if (e->type == SAPP_EVENTTYPE_KEY_DOWN || e->type == SAPP_EVENTTYPE_KEY_UP) {
    bool btn_down = e->type == SAPP_EVENTTYPE_KEY_DOWN;

    if (e->key_code == SAPP_KEYCODE_Z)
      inp.action = btn_down;

    if (e->key_code == SAPP_KEYCODE_W)
      inp.up = btn_down;
    else if (e->key_code == SAPP_KEYCODE_S)
      inp.down = btn_down;

    if (e->key_code == SAPP_KEYCODE_A)
      inp.left = btn_down;
    else if (e->key_code == SAPP_KEYCODE_D)
      inp.right = btn_down;

    if (e->key_repeat)
      return;
  }
}

static void cleanup() {
  state.st.reset();

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
  desc.sample_count = 2;
  desc.gl_major_version = 3;
  desc.gl_minor_version = 3;

  return desc;
}
