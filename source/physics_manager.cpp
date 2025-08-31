#include "physics_manager.hpp"

PhysicsManager::~PhysicsManager() {
  if (m_control) {
    m_control->release();
    m_control = nullptr;
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

void PhysicsManager::init() {
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
  scene_desc.gravity = physx::PxVec3(0.0f, -15.0f, 0.0f);

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

  m_control = PxCreateControllerManager(*m_scene);
}

void PhysicsManager::step(f32 dt) {
  m_scene->simulate(dt);
  m_scene->fetchResults(true);
}
