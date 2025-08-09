/*
 *
 * TODO: Refatorar a Detecção e Resolução de colisões do jogador
 * TODO: Criar uma Classe Entity/GameObject
 *
 */

#include "game.hpp"

#include "sokol_app.h"
#include "sokol_fetch.h"
#include "sokol_log.h"

#include "sokol_fontstash.h"
#include "sokol_gl.h"

#include "billboard.glsl.h"
#include "default.glsl.h"
#include "shape.glsl.h"

Game::Game() {
  {
    sfons_desc_t desc = {};
    f32 dpi = sapp_dpi_scale();
    desc.width = 512 * dpi;
    desc.height = 512 * dpi;
    m_font_ctx = sfons_create(&desc);

    m_font_normal =
        fonsAddFont(m_font_ctx, "normal", "assets/fonts/daydream/Daydream.ttf");
  }

  {
    sfetch_desc_t desc = {};
    desc.max_requests = 128;
    desc.num_channels = 2;
    desc.num_lanes = 1;
    desc.logger.func = slog_func;
    sfetch_setup(&desc);
  }

  m_cam = std::make_shared<Camera>(glm::vec3(-12.861005, 1.293806, 0.921208));
  m_cam->setViewport(sapp_widthf(), sapp_heightf());

  initPipeline();

  {
    for (int i = 0; i < 2; i++) {
      glm::vec3 pos[2] = {
          glm::vec3(0.0f),
          glm::vec3(-4.0f, 0.0f, -1.0f),
      };

      m_colliders_shape.push_back(new Shape(pos[i], glm::vec3(1.0f)));

      AABB c;
      c.min = (pos[i] - 1.0f);
      c.max = (pos[i] + 1.0f);
      m_colliders.push_back(c);
    }
  }

  {
    Ramp ramp;

    glm::vec3 offset(2.0f, -1.0f, 2.0f);

    ramp.v0 = glm::vec3(-1.0f, 0.0f, 0.0f);
    ramp.v1 = glm::vec3(1.0f, 0.0f, 0.0f);
    ramp.v2 = glm::vec3(1.0f, 1.0f, 2.0f);

    ramp.v0 += offset;
    ramp.v1 += offset;
    ramp.v2 += offset;

    glm::vec3 center = (ramp.v0 + ramp.v1 + ramp.v2) / 3.0f;

    f32 scale = 4.0f;
    ramp.v0 = center + (ramp.v0 - center) * scale;
    ramp.v1 = center + (ramp.v1 - center) * scale;
    ramp.v2 = center + (ramp.v2 - center) * scale;

    glm::vec3 e1 = ramp.v1 - ramp.v0;
    glm::vec3 e2 = ramp.v2 - ramp.v0;
    glm::vec3 n = glm::normalize(glm::cross(e1, e2));

    ramp.plane.normal = n;
    ramp.plane.dist = glm::dot(n, ramp.v0);

    m_ramps.push_back(ramp);
    m_ramps_shape.push_back(new Shape(n, glm::vec3(1.0f), ShapeType::PLANE));
  }

  m_player = std::make_shared<Player>(glm::vec3(-4.0f, 0.0f, 4.0f));

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

  sfons_destroy(m_font_ctx);

  sfetch_shutdown();

  LogInfo("Game deleted");
}

void Game::update(f32 dt, Input &inp) {
  sfetch_dowork();

  m_player->input(inp);

  MoveResult r = stepSlideMove(m_player->m_collider, m_player->m_pos,
                               m_player->m_vel, dt, m_colliders, m_ramps);

  m_player->m_pos = r.final_center;
  m_player->m_vel = r.final_velocity;

  m_player->update(dt);
}

void Game::render() {
  {
    sgl_defaults();
    sgl_matrix_mode_projection();
    sgl_ortho(0.0f, sapp_widthf(), sapp_heightf(), 0.0f, -1.0f, 1.0f);

    f32 dpi = sapp_dpi_scale();
    f32 sx = 50 * dpi, sy = 50 * dpi;
    f32 dx = sx, dy = sy;

    fonsSetFont(m_font_ctx, m_font_normal);
    fonsSetSize(m_font_ctx, 30.0f);
    fonsSetColor(m_font_ctx, sfons_rgba(255, 0, 0, 128));
    dx = fonsDrawText(m_font_ctx, dx, dy, "Hello My Friends", NULL);

    sfons_flush(m_font_ctx);
  }

  // All 3D Render
  m_render.use();
  {
    m_player->draw(*m_cam);
  }

  // All 3D-Debug Render
  m_render_sp.use();
  {
    m_player->drawDebug(*m_cam);

    for (size_t i = 0; i < m_colliders_shape.size(); i++) {
      auto &s = m_colliders_shape.at(i);
      auto &c = m_colliders.at(i);

      glm::mat4 m_model = glm::mat4(1.0f);

      glm::vec3 size = c.getSize();
      glm::vec3 center = c.getCenter();

      m_model = glm::translate(m_model, center);
      m_model = glm::scale(m_model, size);

      s->bind();

      vs_params_shape_t vs_params = {};
      vs_params.mvp = m_cam->getMatrix() * m_model;
      sg_apply_uniforms(UB_vs_params_shape, SG_RANGE_REF(vs_params));
      s->draw(*m_cam);
    }

    for (size_t i = 0; i < m_ramps_shape.size(); i++) {
      auto &s = m_ramps_shape.at(i);
      auto &r = m_ramps.at(i);

      glm::vec3 center = (r.v0 + r.v1 + r.v2) / 3.0f;

      // glm::vec3 min = glm::min(glm::min(r.v0, r.v1), r.v2);
      // glm::vec3 max = glm::max(glm::max(r.v0, r.v1), r.v2);

      glm::vec3 size = glm::vec3(2.0f);

      glm::mat4 m_model = glm::mat4(1.0f);
      m_model = glm::translate(m_model, center);
      m_model = glm::scale(m_model, size);

      s->bind();

      vs_params_shape_t vs_params = {};
      vs_params.mvp = m_cam->getMatrix() * m_model;
      sg_apply_uniforms(UB_vs_params_shape, SG_RANGE_REF(vs_params));
      s->draw(*m_cam);
    }
  }

  m_render_bb.use();
  for (Billboard *b : m_boards) {
    b->draw(*m_cam);
  }

  sgl_draw();
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
    m_render.init(desc, default_shader_desc(sg_query_backend()));
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

    desc.layout.attrs[ATTR_billboard_apos].format = SG_VERTEXFORMAT_FLOAT3;
    desc.layout.attrs[ATTR_billboard_apos].offset = 0;

    desc.layout.attrs[ATTR_billboard_atex_coords].format =
        SG_VERTEXFORMAT_FLOAT2;
    desc.layout.attrs[ATTR_billboard_atex_coords].offset =
        offsetof(Vertex, tex_coords);

    // desc.layout.attrs[ATTR_default_anormal].format = SG_VERTEXFORMAT_FLOAT3;

    desc.index_type = SG_INDEXTYPE_UINT16;
    desc.cull_mode = SG_CULLMODE_BACK;
    desc.depth.write_enabled = true;
    desc.depth.compare = SG_COMPAREFUNC_LESS;
    m_render_bb.init(desc, billboard_shader_desc(sg_query_backend()));
  }

  {
    sg_pipeline_desc desc = {};

    desc.layout.buffers[0] = sshape_vertex_buffer_layout_state();
    desc.layout.attrs[ATTR_shape_apos] = sshape_position_vertex_attr_state();
    desc.layout.attrs[ATTR_shape_acolor] = sshape_color_vertex_attr_state();
    desc.primitive_type = SG_PRIMITIVETYPE_LINE_STRIP;
    desc.index_type = SG_INDEXTYPE_UINT16;
    desc.cull_mode = SG_CULLMODE_BACK;
    desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
    desc.depth.write_enabled = true;

    m_render_sp.init(desc, shape_shader_desc(sg_query_backend()));
  }
}
