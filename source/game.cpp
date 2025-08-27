/*
 *
 * TODO: Criar uma Classe Entity/GameObject
 *
 */

#include "game.hpp"

#include "sokol_app.h"
#include "sokol_fetch.h"
#include "sokol_log.h"

#include "sokol_fontstash.h"
#include "sokol_gl.h"

#include "physx_utils.hpp"

#include "billboard.glsl.h"
#include "default.glsl.h"
#include "shape.glsl.h"

#include <filesystem>
extern std::filesystem::path g_game_root;

Game::Game() {
  {
    sfons_desc_t desc = {};
    f32 dpi = sapp_dpi_scale();
    desc.width = 512 * dpi;
    desc.height = 512 * dpi;
    m_font_ctx = sfons_create(&desc);

    std::string path = g_game_root / "assets/fonts/daydream/Daydream.ttf";

    m_font_normal =
        fonsAddFont(m_font_ctx, "normal",
                    path.c_str());
  }

  {
    sfetch_desc_t desc = {};
    desc.max_requests = 128;
    desc.num_channels = 1;
    desc.num_lanes = 1;
    desc.logger.func = slog_func;
    sfetch_setup(&desc);
  }

  initPhysX();
  initPipeline();
  initTexturePool();
  initTextures();

  m_player = new Player(glm::vec3(52, 16, 157));
  m_map.init("assets/maps/test/test.obj");
  m_map.setupPhysics(m_physics, m_scene);

  m_boards[0] = new Billboard("assets/tree.png");
  m_boards[1] = new Billboard("assets/bayo.png");

  m_cam.init(glm::vec3(-12.861005, 1.293806, 0.921208));
  m_cam.setViewport(sapp_widthf(), sapp_heightf());
  m_cam.setDistance(0.01f, 3000.0f);

  m_player->initPhysics(m_physics, m_control_mgr);

  physx::PxU32 nb_actors =
      m_scene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC);
  m_actors.resize(nb_actors);
  m_scene->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC,
                     reinterpret_cast<physx::PxActor **>(m_actors.data()),
                     nb_actors);

  LogInfo("Game created");
}

Game::~Game() {
  for (Billboard *b : m_boards) {
    delete b;
  }

  delete m_player;
  m_map.destroy();

  shutdownPhysX();

  sfons_destroy(m_font_ctx);
  sfetch_shutdown();

  LogInfo("Game deleted");
}
glm::vec3 vel(0.0f);
glm::vec3 vel2(0.0f);

void Game::update(f32 dt) {
  sfetch_dowork();
  stepSimulation(dt);

  vel.y -= 35.0f * dt;
  vel2.y -= 35.0f * dt;

  {
    physx::PxControllerFilters filters;
    physx::PxController *cnt = m_control_mgr->getController(0);

    physx::PxVec3 disp = glmToPxVec3(vel * dt);
    physx::PxControllerCollisionFlags flags =
        cnt->move(disp, 0.001f, dt, filters);

    m_boards[0]->m_pos = pxToGlmExtendedVec3(cnt->getPosition());
  }

  {
    physx::PxControllerFilters filters;
    physx::PxController *cnt = m_control_mgr->getController(1);

    physx::PxVec3 disp = glmToPxVec3(vel2 * dt);
    physx::PxControllerCollisionFlags flags =
        cnt->move(disp, 0.001f, dt, filters);

    m_boards[1]->m_pos = pxToGlmExtendedVec3(cnt->getPosition());
  }

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
    m_player->draw(m_cam);
    m_map.draw(m_cam);
  }

  // All 3D-Debug Render
  // m_render_sp.use();
  // {
  //   for (physx::PxRigidActor *actor : m_actors) {
  //     physx::PxU32 nb_shapes = actor->getNbShapes();
  //     std::vector<physx::PxShape *> shapes(nb_shapes);
  //     actor->getShapes(shapes.data(), nb_shapes);
  //
  //     for (physx::PxShape *shape : shapes) {
  //       physx::PxTransform local_pose = shape->getLocalPose();
  //       physx::PxTransform global_pose = actor->getGlobalPose() * local_pose;
  //
  //       glm::mat4 model = pxToGlmMat4(global_pose);
  //       vs_params_shape_t vs_params = {};
  //       vs_params.mvp = m_cam.getMatrix() * model;
  //
  //       Shape sp(glm::vec3(0.0f));
  //       sp.bind();
  //       sg_apply_uniforms(UB_vs_params_shape, SG_RANGE_REF(vs_params));
  //       sp.draw(m_cam);
  //     }
  //   }
  // }

  m_render_bb.use();
  for (Billboard *b : m_boards) {
    b->draw(m_cam);
  }

  sgl_draw();
}

void Game::handleEvent(const sapp_event *e) {
  if (e->type == SAPP_EVENTTYPE_MOUSE_MOVE) {
    m_mouse_x += e->mouse_dx;
    m_mouse_y += e->mouse_dy;
    m_cam.updateMouse(m_mouse_x, m_mouse_y);
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

  m_control_mgr = PxCreateControllerManager(*m_scene);

  {
    physx::PxTransformT<double> t(physx::PxExtendedVec3(240, 16, 264));
    physx::PxCapsuleControllerDesc desc;
    desc.height = 56.0f - 2 * 16;
    desc.radius = 32.0f / 2;
    desc.material = m_material;
    desc.position = t.p;
    desc.contactOffset = desc.radius * 0.1f;
    desc.stepOffset = desc.height * 0.25f;
    desc.slopeLimit = cosf(physx::PxPi / 4);
    desc.nonWalkableMode =
        physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING;

    m_control_mgr->createController(desc);
  }

  {
    physx::PxTransformT<double> t(physx::PxExtendedVec3(-72, 224, 1472));
    physx::PxCapsuleControllerDesc desc;
    desc.height = 56.0f - 2 * 16;
    desc.radius = 32.0f / 2;
    desc.material = m_material;
    desc.position = t.p;
    desc.contactOffset = desc.radius * 0.1f;
    desc.stepOffset = desc.height * 0.25f;
    desc.slopeLimit = cosf(physx::PxPi / 4);
    desc.nonWalkableMode =
        physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING;

    m_control_mgr->createController(desc);
  }
}

void Game::stepSimulation(f32 dt) {
  m_scene->simulate(dt);
  m_scene->fetchResults(true);
}

void Game::shutdownPhysX() {
  if (m_control_mgr) {
    m_control_mgr->release();
    m_control_mgr = nullptr;
  }

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
    desc.depth.compare = SG_COMPAREFUNC_LESS;
    desc.depth.write_enabled = true;
    m_render_bb.init(desc, billboard_shader_desc(sg_query_backend()));
  }

  {
    sg_pipeline_desc desc = {};

    desc.layout.buffers[0] = sshape_vertex_buffer_layout_state();
    desc.layout.attrs[ATTR_shape_apos] = sshape_position_vertex_attr_state();
    desc.layout.attrs[ATTR_shape_acolor] = sshape_color_vertex_attr_state();
    desc.primitive_type = SG_PRIMITIVETYPE_LINE_STRIP;
    desc.index_type = SG_INDEXTYPE_UINT16;
    desc.cull_mode = SG_CULLMODE_FRONT;
    desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
    desc.depth.write_enabled = true;

    m_render_sp.init(desc, shape_shader_desc(sg_query_backend()));
  }
}

void Game::initTextures() {
  setTextureVerticalFlip(true);
  setTextureWrap(SG_WRAP_REPEAT, SG_WRAP_REPEAT);
  setTextureFilter(SG_FILTER_NEAREST, SG_FILTER_NEAREST);

  addTextureOnThread("assets/models/Beat/Beat.png");

  // addTextureOnThread("assets/models/deku_tree/leaves.png");
  // addTextureOnThread("assets/models/deku_tree/road.png");
  // addTextureOnThread("assets/models/deku_tree/treestop.png");
  // addTextureOnThread("assets/models/deku_tree/treesbottom.png");
  // addTextureOnThread("assets/models/deku_tree/vines.png");
  // addTextureOnThread("assets/models/deku_tree/Road end.png");
  // addTextureOnThread("assets/models/deku_tree/mustache and eyebrows.png");
  // addTextureOnThread("assets/models/deku_tree/deku tree skin.png");
  // addTextureOnThread("assets/models/deku_tree/ground.png");
  // addTextureOnThread("assets/models/deku_tree/Wall.png");

  addTextureOnThread("assets/maps/textures/test/grass.png");
  addTextureOnThread("assets/maps/textures/test/wall.png");
  addTextureOnThread("assets/maps/textures/test/floor.png");
  addTextureOnThread("assets/maps/textures/test/tenman.png");
  addTextureOnThread("assets/maps/textures/test/bayo2.png");
  addTextureOnThread("assets/maps/textures/test/rocks.png");

  setTextureVerticalFlip(false);
  setTextureWrap(SG_WRAP_CLAMP_TO_EDGE, SG_WRAP_CLAMP_TO_EDGE);
  setTextureFilter(SG_FILTER_NEAREST, SG_FILTER_NEAREST);

  addTextureOnThread("assets/textures/tree.png");
  addTextureOnThread("assets/textures/bayo.png");
}
