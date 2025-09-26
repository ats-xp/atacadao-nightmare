#include "menu.hpp"
#include "asset_manager.hpp"
#include "input.hpp"

#include "sdl2_glue.hpp"

#include "sokol_gfx.h"

#include "sokol_fontstash.h"
#include "sokol_gl.h"

Menu::Menu() {
  {
    std::string path = AssetManager::getPath("assets/fonts/daydream/Daydream.ttf");

    sfons_desc_t desc = {};
    desc.width = 512;
    desc.height = 512;
    m_font_ctx = sfons_create(&desc);
    m_font_daydream = fonsAddFont(m_font_ctx, "daydream",
                                  path.c_str());
  }

  LogInfo("Menu created");
}

Menu::~Menu() {
  sfons_destroy(m_font_ctx);

  LogInfo("Menu deleted");
}

void Menu::update(f32 dt) {
  (void)dt;

  const u8 *keys = SDL_GetKeyboardState(nullptr);

  if (keys[SDL_SCANCODE_Z])
    setNext(StateId::GAME);
};

void Menu::render() {
  {
    f32 dpi = 1;
    f32 sx = 50 * dpi, sy = 50 * dpi;
    f32 dx = sx;
    f32 dy = sy;
    // f32 lh = 0;

    fonsClearState(m_font_ctx);

    sgl_defaults();
    sgl_matrix_mode_projection();
    sgl_ortho(0.0f, sdl2_width(), sdl2_height(), 0.0f, -1.0f, 1.0f);

    // fonsVertMetrics(m_font_ctx, nullptr, nullptr, &lh);
    fonsSetSize(m_font_ctx, 32 * dpi);
    fonsSetFont(m_font_ctx, m_font_daydream);

    fonsSetAlign(m_font_ctx, FONS_ALIGN_LEFT | FONS_ALIGN_BASELINE);
    // dx = sx;
    // dy += lh;
    dx = ((f32)sdl2_width() - (21 * 4)) / 2;
    dy = ((f32)sdl2_height() - (21 * 4)) / 2;
    fonsDrawText(m_font_ctx, dx, dy, "MENU", nullptr);

    sfons_flush(m_font_ctx);
  }

  sgl_draw();
}
