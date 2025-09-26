#include "sdl2_glue.hpp"

#include "base.h"

#include <assert.h>

static int _sample_count;
static bool _no_depth_buffer;
static int _major_version;
static int _minor_version;
static SDL_Window *_window;
static SDL_GLContext *_gl_ctx;

#define _sdl2_def(val, def) (((val) == 0) ? (def) : (val))

void sdl2_init(const sdl2_desc_t *desc) {
  assert(desc);
  assert(desc->width > 0);
  assert(desc->height > 0);
  assert(desc->title);
  sdl2_desc_t desc_def = *desc;
  desc_def.sample_count = _sdl2_def(desc_def.sample_count, 1);
  desc_def.version_major = _sdl2_def(desc_def.version_major, 4);
  desc_def.version_minor = _sdl2_def(desc_def.version_minor, 1);
  _sample_count = desc_def.sample_count;
  _no_depth_buffer = desc_def.no_depth_buffer;
  _major_version = desc_def.version_major;
  _minor_version = desc_def.version_minor;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
    LogError("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, desc_def.version_major);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, desc_def.version_minor);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  if (!desc_def.no_depth_buffer) {
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  }

  _window = SDL_CreateWindow(
      desc_def.title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      desc_def.width, desc_def.height,
      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (!_window) {
    LogError("Window could not be created! SDL Error: %s\n", SDL_GetError());
  }

  _gl_ctx = (SDL_GLContext*)SDL_GL_CreateContext(_window);
  if (!_gl_ctx) {
    LogError("OpenGL context could not be created! SDL Error: %s\n",
             SDL_GetError());
  }

  if (SDL_GL_SetSwapInterval(1) < 0) {
    LogInfo("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
  }

  SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
  // SDL_ShowCursor(false);
}

SDL_Window *sdl2_window(void) { return _window; }
SDL_GLContext *sdl2_gl_ctx(void) { return _gl_ctx; }

int sdl2_width(void) {
  int width, height;
  SDL_GetWindowSize(_window, &width, &height);
  return width;
}

int sdl2_height(void) {
  int width, height;
  SDL_GetWindowSize(_window, &width, &height);
  return height;
}

sg_environment sdl2_environment(void) {
  sg_environment env = {};
  env.defaults.color_format = SG_PIXELFORMAT_RGBA8;
  env.defaults.depth_format =
      _no_depth_buffer ? SG_PIXELFORMAT_NONE : SG_PIXELFORMAT_DEPTH_STENCIL;
  env.defaults.sample_count = _sample_count;

  return env;
}

sg_swapchain sdl2_swapchain(void) {
  int width, height;
  SDL_GetWindowSize(_window, &width, &height);
  sg_swapchain swap = {};
  swap.width = width;
  swap.height = height;
  swap.sample_count = _sample_count;
  swap.color_format = SG_PIXELFORMAT_RGBA8;
  swap.depth_format =
      _no_depth_buffer ? SG_PIXELFORMAT_NONE : SG_PIXELFORMAT_DEPTH_STENCIL;
  swap.gl.framebuffer = 0;

  return swap;
}
