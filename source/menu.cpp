#include "menu.hpp"

#include "input.hpp"

#include "sokol_gfx.h"

#include "sokol_fontstash.h"
#include "sokol_gl.h"

#include <filesystem>
extern std::filesystem::path g_game_root;

Menu::Menu() {
  {
    std::string path = g_game_root / "assets/fonts/daydream/Daydream.ttf";

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
  if (inp.action)
    setNext(StateId::GAME);
};

void Menu::render() {
  {
    f32 dpi = sapp_dpi_scale();
    f32 sx = 50 * dpi, sy = 50 * dpi;
    f32 dx = sx;
    f32 dy = sy;
    f32 lh = 0;

    fonsClearState(m_font_ctx);

    sgl_defaults();
    sgl_matrix_mode_projection();
    sgl_ortho(0.0f, sapp_widthf(), sapp_heightf(), 0.0f, -1.0f, 1.0f);

    // fonsVertMetrics(m_font_ctx, nullptr, nullptr, &lh);
    fonsSetSize(m_font_ctx, 32 * dpi);
    fonsSetFont(m_font_ctx, m_font_daydream);

    fonsSetAlign(m_font_ctx, FONS_ALIGN_LEFT | FONS_ALIGN_BASELINE);
    // dx = sx;
    // dy += lh;
    dx = (sapp_widthf() - (21 * 4)) / 2;
    dy = (sapp_heightf() - (21 * 4)) / 2;
    fonsDrawText(m_font_ctx, dx, dy, "MENU", nullptr);

    sfons_flush(m_font_ctx);
  }

  sgl_draw();
}
