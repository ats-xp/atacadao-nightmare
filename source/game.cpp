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

  initPhysX();

  physx::PxU32 nb_actors =
      m_scene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC);
  m_actors.resize(nb_actors);
  m_scene->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC,
                     reinterpret_cast<physx::PxActor **>(m_actors.data()),
                     nb_actors);

  m_cam = std::make_shared<Camera>(glm::vec3(-12.861005, 1.293806, 0.921208));
  m_cam->setViewport(sapp_widthf(), sapp_heightf());

  initPipeline();

  m_player = std::make_shared<Player>(glm::vec3(-4.0f, 0.0f, 4.0f));

  m_boards[0] = new Billboard("assets/tree.png");
  m_boards[1] = new Billboard("assets/bayo.png");

  Transform trans;
  addModelStore(m_mdl_store,
                new Model("assets/models/deku_tree/greatdekutree.obj"), trans);

  LogInfo("Game created");
}

Game::~Game() {
  for (Billboard *b : m_boards) {
    delete b;
  }

  shutdownPhysX();

  sfons_destroy(m_font_ctx);

  sfetch_shutdown();

  LogInfo("Game deleted");
}

void Game::update(f32 dt, Input &inp) {
  sfetch_dowork();
  stepSimulation(dt);

  {
    physx::PxRigidDynamic *box = m_actors[0]->is<physx::PxRigidDynamic>();
    physx::PxTransform pose = box->getGlobalPose();
    m_boards[0]->m_pos.x = pose.p.x;
    m_boards[0]->m_pos.y = pose.p.y - 1.0f;
    m_boards[0]->m_pos.z = pose.p.z;
  }

  {
    physx::PxRigidDynamic *box = m_actors[1]->is<physx::PxRigidDynamic>();
    physx::PxTransform pose = box->getGlobalPose();
    m_boards[1]->m_pos.x = pose.p.x;
    m_boards[1]->m_pos.y = pose.p.y - 1.0f;
    m_boards[1]->m_pos.z = pose.p.z;
    box->setLinearVelocity(physx::PxVec3(1.0f, box->getLinearVelocity().y, 0.0f));
  }

  m_player->input(inp);
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

    drawModelStore(m_mdl_store, *m_cam);
  }

  // All 3D-Debug Render
  m_render_sp.use();
  {
    m_player->drawDebug(*m_cam);
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

void Game::initPhysX() {
  m_foundation =
      PxCreateFoundation(PX_PHYSICS_VERSION, m_allocator, m_error_callback);
  if (!m_foundation) {
    LogError("PxFoundation Failed");
    abort();
  }

  physx::PxTolerancesScale scale;
  bool record_memory_alloc = true;

  m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, scale,
                              record_memory_alloc);
  if (!m_physics) {
    LogError("PxPhysics Failed");
    abort();
  }

  physx::PxSceneDesc scene_desc(m_physics->getTolerancesScale());
  scene_desc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);

  m_dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
  scene_desc.cpuDispatcher = m_dispatcher;
  scene_desc.filterShader = physx::PxDefaultSimulationFilterShader;

  m_scene = m_physics->createScene(scene_desc);
  if (!m_scene) {
    LogError("PxScene Failed");
    abort();
  }

  m_material = m_physics->createMaterial(0.5f, 0.5f, 0.6f);
  if (!m_material) {
    LogError("PxMaterial Failed");
    abort();
  }

  physx::PxRigidStatic *ground =
      physx::PxCreatePlane(*m_physics, physx::PxPlane(0, 1, 0, 0), *m_material);
  m_scene->addActor(*ground);

  {
    physx::PxTransform t(physx::PxVec3(0.0f, 0.0f, 0.0f));
    physx::PxBoxGeometry geom(physx::PxVec3(0.5f, 0.5f, 0.5f));

    f32 density = 10.0f;

    physx::PxRigidDynamic *box =
        physx::PxCreateDynamic(*m_physics, t, geom, *m_material, density);
    box->setAngularDamping(0.5f);
    // box->setLinearVelocity(physx::PxVec3(1.0f, 0.0f, 0.0f));
    m_scene->addActor(*box);
  }

  {
    physx::PxTransform t(physx::PxVec3(-4.0f, 0.0f, -1.0f));
    physx::PxBoxGeometry geom(physx::PxVec3(0.5f, 0.5f, 0.5f));

    f32 density = 10.0f;

    physx::PxRigidDynamic *box =
        physx::PxCreateDynamic(*m_physics, t, geom, *m_material, density);
    box->setAngularDamping(0.5f);
    m_scene->addActor(*box);
  }
}

void Game::stepSimulation(f32 dt) {
  m_scene->simulate(dt);
  m_scene->fetchResults(true);
}

void Game::shutdownPhysX() {
  if (m_scene) {
    m_scene->release();
    m_scene = nullptr;
  }

  if (m_dispatcher) {
    m_dispatcher->release();
    m_dispatcher = nullptr;
  }

  if (m_physics) {
    m_physics->release();
    m_physics = nullptr;
  }

  if (m_foundation) {
    m_foundation->release();
    m_foundation = nullptr;
  }
}

void Game::initPipeline() {
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
