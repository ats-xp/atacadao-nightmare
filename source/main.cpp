#include <memory>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "sdl2_glue.hpp"

#include "sokol_fetch.h"
#include "sokol_gfx.h"
#include "sokol_log.h"
#include "sokol_memtrack.h"
#include "sokol_shape.h"
#include "sokol_time.h"

#include "fontstash.h"
#include "sokol_fontstash.h"
#include "sokol_gl.h"

#include "base.h"

#include "game.hpp"
// #include "input.hpp"
#include "menu.hpp"

#include "asset_manager.hpp"

// Input inp;

static struct {
  /*
   * FIXME:
   * Classe State instancia junto com a classe
   * compartilhada
   */
  std::unique_ptr<State> st;

  u64 last_time;
  f64 delta_time;
  f64 frame_accum;
  int frame_count;
  int fps;

  SDL_Window *win;
  SDL_GLContext *gl_ctx;
} state;

static void init() {
  {
    sdl2_desc_t desc = {};
    desc.title = "Project AN FPS:";
    desc.width = 640;
    desc.height = 480;
    sdl2_init(&desc);
  }

  {
    sg_desc desc = {};
    desc.environment = sdl2_environment();
    desc.logger.func = slog_func;

    desc.pipeline_pool_size = 256;
    desc.buffer_pool_size = 256;
    desc.shader_pool_size = 256;

    desc.allocator.alloc_fn = smemtrack_alloc,
    desc.allocator.free_fn = smemtrack_free;
    sg_setup(&desc);

    SDL_SetRelativeMouseMode(SDL_TRUE);
  }

  {
    sgl_desc_t desc = {};
    desc.logger.func = slog_func;
    sgl_setup(&desc);
  }

  stm_setup();

  AssetManager::init();

  state.last_time = stm_now();
  state.st = std::make_unique<Game>();
}

static void frame() {
  u64 now = stm_now();
  state.delta_time = stm_sec(stm_diff(now, state.last_time));
  state.last_time = now;

  state.frame_accum += state.delta_time;
  state.frame_count++;

  char title[30];
  sprintf(title, "Project AN FPS: %d", state.fps);
  SDL_SetWindowTitle(state.win, title);

  u8 next_st = state.st->getNext();
  if (StateId::OFF != next_st) {
    state.st.reset();
    // memset(&inp, 0, sizeof(Input));

    switch (next_st) {
    case StateId::MENU:
      state.st = std::make_unique<Menu>();
      break;
    case StateId::GAME:
      state.st = std::make_unique<Game>();
      break;
    }
  }

  state.st->update(static_cast<float>(state.delta_time));

  sg_pass_action action = {};
  action.colors[0].load_action = SG_LOADACTION_CLEAR;
  action.colors[0].clear_value = {0.0f, 0.0f, 0.0f, 0.0f};

  sg_pass pass = {};
  pass.action = action;
  pass.swapchain = sdl2_swapchain();

  sg_begin_pass(&pass);

  state.st->render();

  sg_end_pass();
  sg_commit();
  SDL_GL_SwapWindow(state.win);

  if (state.frame_accum >= 1.0f) {
    state.fps = state.frame_count;
    state.frame_count = 0;
    state.frame_accum = 0.0f;
  }
}

// static void handleInput(const sapp_event *e) {
// #define SET_KEY(btn, key)                                                      \
//   if (e->key_code == (key))                                                    \
//     inp.btn = btn_down;
//
//   // Input &inp = state.input;
//
//   state.st->handleEvent(e);
//
//   if (e->type == SAPP_EVENTTYPE_KEY_DOWN || e->type == SAPP_EVENTTYPE_KEY_UP) {
//     bool btn_down = e->type == SAPP_EVENTTYPE_KEY_DOWN;
//
//     SET_KEY(action, SAPP_KEYCODE_Z);
//     SET_KEY(b, SAPP_KEYCODE_B);
//
//     SET_KEY(jump, SAPP_KEYCODE_SPACE);
//
//     SET_KEY(up, SAPP_KEYCODE_W);
//     SET_KEY(down, SAPP_KEYCODE_S);
//     SET_KEY(left, SAPP_KEYCODE_A);
//     SET_KEY(right, SAPP_KEYCODE_D);
//
//     if (e->key_repeat)
//       return;
//
//     // if (e->key_code == SAPP_KEYCODE_ESCAPE)
//       // sapp_request_quit();
//   }
// }

static void cleanup() {
  state.st.reset();

  sgl_shutdown();
  sg_shutdown();

  SDL_GL_DeleteContext(state.gl_ctx);
  SDL_DestroyWindow(state.win);
  SDL_Quit();
}

int main() {
  init();

  bool quit = false;
  state.win = sdl2_window();
  state.gl_ctx = sdl2_gl_ctx();

  if (!sg_isvalid()) {
    SDL_Log("Error: Failed to initialize sokol_gfx");
    cleanup();
    return -1;
  }

  while (!quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        quit = true;
      }

      state.st->handleEvent(&event);
    }

    frame();
  }

  cleanup();

  return 0;
}
