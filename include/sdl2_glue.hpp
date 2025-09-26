#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "sokol_gfx.h"

typedef struct sdl2_desc_t {
    int width;
    int height;
    int sample_count;
    bool no_depth_buffer;
    const char* title;
    int version_major;
    int version_minor;
} sdl2_desc_t;

void sdl2_init(const sdl2_desc_t* desc);
SDL_Window* sdl2_window(void);
SDL_GLContext *sdl2_gl_ctx(void);
int sdl2_width(void);
int sdl2_height(void);
sg_environment sdl2_environment(void);
sg_swapchain sdl2_swapchain(void);
