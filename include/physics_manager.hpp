#pragma once

#include "base.h"
#undef S
#include <PxPhysicsAPI.h>

class PhysicsManager {
public:
  physx::PxDefaultAllocator m_allocator;
  physx::PxDefaultErrorCallback m_error_callback;
  physx::PxFoundation *m_foundation = nullptr;
  physx::PxPhysics *m_physics = nullptr;
  physx::PxDefaultCpuDispatcher *m_dispatcher = nullptr;
  physx::PxScene *m_scene = nullptr;
  physx::PxMaterial *m_material = nullptr;

  physx::PxControllerManager *m_control = nullptr;

  PhysicsManager() = default;
  ~PhysicsManager();

  void init();
  void step(f32 dt);
};

class PhysicsGameObject {
public:
  PhysicsGameObject() = default;
  ~PhysicsGameObject();

  template <typename T>
  static physx::PxController *create(PhysicsManager &mgr, const f32 &height,
                                     const f32 &radius,
                                     const physx::PxExtendedVec3 &pos) {
    T desc;
    desc.height = height;
    desc.radius = radius;
    desc.position = pos;
    desc.material = mgr.m_material;
    desc.contactOffset = desc.radius * 0.1f;
    desc.stepOffset = desc.height * 0.25f;
    desc.slopeLimit = cosf(physx::PxPi / 4);
    desc.nonWalkableMode =
        physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING;

    return mgr.m_control->createController(desc);
  }

  static bool step(physx::PxController *ctrl, f32 dt, physx::PxVec3 &vel) {
    physx::PxControllerFilters filters;
    physx::PxVec3 disp = vel * dt;
    physx::PxControllerCollisionFlags flags =
        ctrl->move(disp, 0.001f, dt, filters);

    if (flags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN) {
      if (vel.y < 0.0f) {
        vel.y = -64.0f;
      }

      return true;
    }

    return false;
  }

  static void updateGravity(f32 dt, physx::PxVec3 &vel) {
    vel.y -= 640.0f * dt;
  }
};
