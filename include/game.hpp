#pragma once

#include <array>

#include "state.hpp"

#include "base.h"
#undef S
#include <PxPhysicsAPI.h>

#include "fontstash.h"

#include "billboard.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "player.hpp"
#include "render.hpp"
#include "shape.hpp"
#include "map.hpp"

class Game : public State {
  Render m_render;
  Render m_render_bb; // billboard
  Render m_render_sp; // Shape

  Player *m_player;
  std::array<Billboard *, 2> m_boards;

  Map m_map;

  Camera m_cam;

  FONScontext *m_font_ctx;
  int m_font_normal;

  f32 m_mouse_x = 0;
  f32 m_mouse_y = 0;

  physx::PxDefaultAllocator m_allocator;
  physx::PxDefaultErrorCallback m_error_callback;
  physx::PxFoundation *m_foundation = nullptr;
  physx::PxPhysics *m_physics = nullptr;
  physx::PxDefaultCpuDispatcher *m_dispatcher = nullptr;
  physx::PxScene *m_scene = nullptr;
  physx::PxMaterial *m_material = nullptr;

  physx::PxControllerManager *m_control_mgr;

  std::vector<physx::PxRigidActor *> m_actors;

  void initPhysX();
  void stepSimulation(f32 dt);
  void shutdownPhysX();

  void initPipeline();
  void initTextures();

public:
  Game();
  ~Game();

  void update(f32 dt) override;
  void render() override;
  void handleEvent(const sapp_event *e) override;
};
