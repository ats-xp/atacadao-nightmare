#include "game.hpp"

#include "sokol_app.h"

#include "default.glsl.h"

Game::Game() {
  m_cam = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 6.0f));
  m_cam->setViewport(sapp_widthf(), sapp_heightf());

  sg_pipeline_desc desc = {};
  desc.layout.buffers[0].stride = sizeof(Vertex);
  desc.layout.attrs[ATTR_default_apos].format = SG_VERTEXFORMAT_FLOAT3;
  desc.layout.attrs[ATTR_default_acolor].format = SG_VERTEXFORMAT_FLOAT4;
  desc.layout.attrs[ATTR_default_anormal].format = SG_VERTEXFORMAT_FLOAT3;
  desc.layout.attrs[ATTR_default_atex_coords].format = SG_VERTEXFORMAT_FLOAT2;
  desc.index_type = SG_INDEXTYPE_UINT16;
  desc.cull_mode = SG_CULLMODE_FRONT;
  desc.depth.write_enabled = true;
  desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
  m_render.init(desc, default_shader_desc(sg_backend()));

  m_player = std::make_shared<Player>(glm::vec3(0.0f));

  glm::vec3 pos[10] = {
      glm::vec3(0.0f, 0.0f, 0.0f),   glm::vec3(10.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 10.0f, 0.0f),  glm::vec3(0.0f, 0.0f, 10.0f),
      glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, -10.0f, 0.0f),
      glm::vec3(-10.0f, 0.0f, 0.0f)};

  for (int i = 0; i < 7; i++) {
    mdls.push_back(new Model("assets/models/Beat/Beat.obj"));
    mdls[i]->setPos(pos[i]);
  }

  LogInfo("Game created");
}

Game::~Game() {
  for (size_t i = 0; i < mdls.size(); i++) {
    delete mdls[i];
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

  mdls[0]->resetRotation();
  mdls[0]->setRotationY(rot += 0.2f);

  // m_player->update(inp);
}

void Game::render() {
  m_render.use();

  for (Model *m : mdls) {
    m->draw(*m_cam);
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
