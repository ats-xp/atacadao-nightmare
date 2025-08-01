#include "game.hpp"

#include "sokol_app.h"

#include "billboard.glsl.h"
#include "default.glsl.h"

Game::Game() {
  m_cam = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 6.0f));
  m_cam->setViewport(sapp_widthf(), sapp_heightf());

  initPipeline();

  m_player = std::make_shared<Player>(glm::vec3(0.0f));

  // glm::vec3 pos[10] = {
  //     glm::vec3(0.0f, 0.0f, 0.0f),   glm::vec3(10.0f, 0.0f, 0.0f),
  //     glm::vec3(0.0f, 10.0f, 0.0f),  glm::vec3(0.0f, 0.0f, 10.0f),
  //     glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, -10.0f, 0.0f),
  //     glm::vec3(-10.0f, 0.0f, 0.0f)};
  //
  // for (int i = 0; i < 7; i++) {
  //   addModelStore(m_mdl_store, new Model("assets/models/Beat/Beat.obj"),
  //                 Transform(pos[i]));
  // }

  Transform trans;
  trans.position = glm::vec3(0.0f);
  addModelStore(m_mdl_store,
                new Model("assets/models/deku_tree/greatdekutree.obj"), trans);

  m_boards[0] = new Billboard("assets/tree.png");
  m_boards[0]->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

  m_boards[1] = new Billboard("assets/bayo.png");
  m_boards[1]->setPosition(glm::vec3(-4.0f, 0.0f, -1.0f));

  LogInfo("Game created");
}

Game::~Game() {
  for (Billboard *b : m_boards) {
    delete b;
  }

  for (Model *m : m_mdl_store.models) {
    delete m;
  }

  LogInfo("Game deleted");
}

void Game::update(f32 dt, Input &inp) {
  f32 speed = 5.0f * dt;
  if (inp.up)
    m_cam->move(CameraDirection::UP, speed);
  else if (inp.down)
    m_cam->move(CameraDirection::DOWN, speed);

  if (inp.left)
    m_cam->move(CameraDirection::LEFT, speed);
  else if (inp.right)
    m_cam->move(CameraDirection::RIGHT, speed);

  updateModelStore(m_mdl_store);

  // m_player->update(inp);
}

void Game::render() {
  m_render.use();

  drawModelStore(m_mdl_store, *m_cam);

  m_render_bb.use();
  for (Billboard *b : m_boards) {
    b->draw(*m_cam);
  }

  // m_player->render(*m_cam);
}

void Game::handleEvent(const sapp_event *e) {
  if (e->type == SAPP_EVENTTYPE_MOUSE_MOVE) {
    m_mouse_x += e->mouse_dx;
    m_mouse_y += e->mouse_dy;
    m_cam->updateMouse(m_mouse_x, m_mouse_y);
  }

  if (e->type == SAPP_EVENTTYPE_FOCUSED) {
    sapp_lock_mouse(true);
  }
}

void Game::initPipeline() {
  {
    sg_pipeline_desc desc = {};
    desc.layout.buffers[0].stride = sizeof(Vertex);
    desc.layout.attrs[ATTR_default_apos].format = SG_VERTEXFORMAT_FLOAT3;
    desc.layout.attrs[ATTR_default_anormal].format = SG_VERTEXFORMAT_FLOAT3;
    desc.layout.attrs[ATTR_default_atex_coords].format = SG_VERTEXFORMAT_FLOAT2;
    desc.index_type = SG_INDEXTYPE_UINT16;
    desc.cull_mode = SG_CULLMODE_FRONT;
    desc.depth.write_enabled = true;
    desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
    m_render.init(desc, default_shader_desc(sg_backend()));
  }

  {
    sg_pipeline_desc desc = {};

    sg_blend_state blend_state = {};
    blend_state.enabled = true;
    blend_state.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
    blend_state.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    blend_state.src_factor_alpha = SG_BLENDFACTOR_ZERO;
    blend_state.dst_factor_alpha = SG_BLENDFACTOR_ONE;

    desc.colors[0].blend = blend_state;

    desc.layout.buffers[0].stride = sizeof(Vertex);
    desc.layout.attrs[ATTR_default_apos].format = SG_VERTEXFORMAT_FLOAT3;
    desc.layout.attrs[ATTR_default_anormal].format = SG_VERTEXFORMAT_FLOAT3;
    desc.layout.attrs[ATTR_default_atex_coords].format = SG_VERTEXFORMAT_FLOAT2;
    desc.index_type = SG_INDEXTYPE_UINT16;
    desc.cull_mode = SG_CULLMODE_BACK;
    desc.depth.write_enabled = true;
    desc.depth.compare = SG_COMPAREFUNC_LESS;
    m_render_bb.init(desc, billboard_shader_desc(sg_backend()));
  }
}
