#include "game.hpp"
#include "asset_manager.hpp"
#include "dummy.hpp"

#include "sdl2_glue.hpp"

#include "sokol_fetch.h"
#include "sokol_log.h"

#include "sokol_fontstash.h"
#include "sokol_gl.h"

#include <fstream>

Game::Game() {
  {
    sfons_desc_t desc = {};
    f32 dpi = 1;
    desc.width = 512 * dpi;
    desc.height = 512 * dpi;
    m_font_ctx = sfons_create(&desc);

    std::string path =
        AssetManager::getPath("assets/fonts/daydream/Daydream.ttf");
    m_font_normal = fonsAddFont(m_font_ctx, "normal", path.c_str());
  }

  {
    sfetch_desc_t desc = {};
    desc.max_requests = 128;
    desc.num_channels = 1;
    desc.num_lanes = 4;
    desc.logger.func = slog_func;
    sfetch_setup(&desc);
  }

  initTextures();

  m_physics.init();

  m_map.init("assets/maps/test/test.obj");
  m_map.setupPhysics(m_physics);

  m_cam.init(glm::vec3(-12.861005, 1.293806, 0.921208));
  m_cam.setViewport(sdl2_width(), sdl2_height());
  m_cam.setDistance(0.01f, 3000.0f);

  TBEntity ent = m_map.map.getEntity("info_player_spawn");
  m_player = new Player(Map::parserStringToVec3(ent.getProperty("origin")));
  m_player->m_control = PhysicsGameObject::create<physx::PxBoxControllerDesc>(
      // m_physics, 56.0f - 2 * 16, 32.0f / 2,
      m_physics, 32.0f, 56.0f, 32.0f, glmToPxExtendedVec3(m_player->m_pos));

  std::vector<TBEntity> entities = m_map.map.getEntityList("info_enemy_spawn");

  for (auto &ent : entities) {
    DummyObject *d =
        new DummyObject("assets/textures/ishowspeed.png", m_physics);

    physx::PxExtendedVec3 v =
        glmToPxExtendedVec3(Map::parserStringToVec3(ent.getProperty("origin")));

    d->m_control->setPosition(v);
    m_objects.push_back(d);
  }

  LogInfo("Game created");
}

Game::~Game() {
  delete m_player;
  m_map.destroy();

  sfons_destroy(m_font_ctx);
  sfetch_shutdown();

  LogInfo("Game deleted");
}

void Game::update(f32 dt) {
  sfetch_dowork();
  m_physics.step(dt);

  m_player->update(dt);

  for (auto o : m_objects) {
    o->chaseTarget(*m_player);
    o->update(dt);
  }
}

void Game::render() {
  {
    sgl_defaults();
    sgl_matrix_mode_projection();
    sgl_ortho(0.0f, sdl2_width(), sdl2_height(), 0.0f, -1.0f, 1.0f);

    f32 dpi = 1;
    f32 sx = 50 * dpi, sy = 50 * dpi;
    f32 dx = sx, dy = sy;

    fonsSetFont(m_font_ctx, m_font_normal);
    fonsSetSize(m_font_ctx, 30.0f);
    fonsSetColor(m_font_ctx, sfons_rgba(255, 0, 0, 128));
    dx = fonsDrawText(m_font_ctx, dx, dy, "Hello My Friends", NULL);

    sfons_flush(m_font_ctx);
  }

  // All 3D Render
  m_player->draw(m_cam);
  m_map.draw(m_cam);

  for (auto o : m_objects) {
    o->draw(m_cam);
  }

  sgl_draw();
}

void Game::handleEvent(const SDL_Event *e) {
  if (e->type == SDL_MOUSEMOTION) {
    m_mouse_x += e->motion.xrel;
    m_mouse_y += e->motion.yrel;
    m_cam.updateMouse(m_mouse_x, m_mouse_y);
  }
}

void Game::initTextures() {
  AssetManager::setTextureVerticalFlip(true);
  AssetManager::setTextureWrap(SG_WRAP_REPEAT, SG_WRAP_REPEAT);
  AssetManager::setTextureFilter(SG_FILTER_NEAREST, SG_FILTER_NEAREST);

  std::ifstream file(AssetManager::getPath("load_textures.txt"));
  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
      AssetManager::addTextureOnThread(line.c_str());
    }
  }

  AssetManager::setTextureVerticalFlip(false);
  AssetManager::setTextureWrap(SG_WRAP_CLAMP_TO_EDGE, SG_WRAP_CLAMP_TO_EDGE);
  AssetManager::setTextureFilter(SG_FILTER_NEAREST, SG_FILTER_NEAREST);

  // addTextureOnThread("assets/textures/tree.png");
  AssetManager::addTextureOnThread("assets/textures/bayo.png");
  AssetManager::addTextureOnThread("assets/textures/ishowspeed.png");
  AssetManager::addTextureOnThread("assets/textures/bullet.png");
  AssetManager::addTextureOnThread("assets/textures/anim.png");
}
