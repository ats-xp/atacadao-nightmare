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

#include "game.hpp"
#include "menu.hpp"

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
  f64 frame_accum;
  int frame_count;
  int fps;
} state;

static void init() {
  {
    sg_desc desc = {};
    desc.environment = sglue_environment();
    desc.logger.func = slog_func;
    desc.buffer_pool_size = 256;
    // desc.sampler_pool_size = 128;
    desc.allocator.alloc_fn = smemtrack_alloc,
    desc.allocator.free_fn = smemtrack_free;
    sg_setup(&desc);
  }

  {
    sgl_desc_t desc = {};
    desc.logger.func = slog_func;
    sgl_setup(&desc);
  }

  stm_setup();

  state.last_time = stm_now();

  state.st = std::make_unique<Menu>();
  sapp_lock_mouse(true);
}

static void frame() {
  u64 now = stm_now();
  state.delta_time = stm_sec(stm_diff(now, state.last_time));
  state.last_time = now;

  state.frame_accum += state.delta_time;
  state.frame_count++;

  char title[30];
  sprintf(title, "Project AN FPS: %d", state.fps);
  sapp_set_window_title(title);

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

  if (state.frame_accum >= 1.0f) {
    state.fps = state.frame_count;
    state.frame_count = 0;
    state.frame_accum = 0.0f;
  }
}

static void handleInput(const sapp_event *e) {
#define SET_KEY(btn, key)                                                      \
  if (e->key_code == (key))                                                    \
    inp.btn = btn_down;

  Input &inp = state.input;

  state.st->handleEvent(e);

  if (e->type == SAPP_EVENTTYPE_KEY_DOWN || e->type == SAPP_EVENTTYPE_KEY_UP) {
    bool btn_down = e->type == SAPP_EVENTTYPE_KEY_DOWN;

    SET_KEY(action, SAPP_KEYCODE_Z);

    SET_KEY(up, SAPP_KEYCODE_W);
    SET_KEY(down, SAPP_KEYCODE_S);
    SET_KEY(left, SAPP_KEYCODE_A);
    SET_KEY(right, SAPP_KEYCODE_D);

    SET_KEY(b, SAPP_KEYCODE_B);

    if (e->key_repeat)
      return;

    if (e->key_code == SAPP_KEYCODE_ESCAPE)
      sapp_request_quit();
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
  desc.logger.func = slog_func;

  // tmp = mul-size
  desc.width = 640;
  desc.height = 480;

  desc.window_title = "Atacado - Nightmare";
  desc.icon.sokol_default = true;

  desc.sample_count = 2;
  desc.swap_interval = 1;
  desc.high_dpi = true;

  desc.gl_major_version = 3;
  desc.gl_minor_version = 3;

  return desc;
}
