#include "player.hpp"

#include "shape.glsl.h"

#include "input.hpp"
#include "physx_utils.hpp"

struct IgnoreControllerFilter : public physx::PxQueryFilterCallback {
  physx::PxController *controller;

  IgnoreControllerFilter(physx::PxController *ctrl) : controller(ctrl) {}

  physx::PxQueryHitType::Enum preFilter(const physx::PxFilterData &filter_data,
                                        const physx::PxShape *shape,
                                        const physx::PxRigidActor *actor,
                                        physx::PxHitFlags &) override {
    (void)filter_data;
    (void)shape;

    if (actor == controller->getActor()) {
      LogInfo("foi??");
      return physx::PxQueryHitType::eNONE;
    }

    return physx::PxQueryHitType::eBLOCK;
  }

  physx::PxQueryHitType::Enum
  postFilter(const physx::PxFilterData &, const physx::PxQueryHit &,
             const physx::PxShape *shape,
             const physx::PxRigidActor *actor) override {
    (void)shape;
    (void)actor;
    return physx::PxQueryHitType::eBLOCK;
  }
};

Player::Player(const glm::vec3 &pos) {
  Transform trans;
  trans.position = pos;
  trans.scale = glm::vec3(32 / 1.8f);

  m_pos = pos;
  m_vel = glm::vec3(0.0f);
  m_speed = 240.0f;

  m_model = new Model("assets/models/Beat/Beat.obj");
  m_model->setTransformitions(trans);
}

Player::~Player() {
  m_control->release();
  m_control = nullptr;
  delete m_model;
}

void Player::update(f32 dt) {
  glm::vec3 input(0.0f);

  is_ground = m_vel.y == 0.0f;

  if (inp.up) {
    input += m_cam_front;
  } else if (inp.down) {
    input -= m_cam_front;
  }

  if (inp.left) {
    input -= m_cam_right;
  } else if (inp.right) {
    input += m_cam_right;
  }

  if (glm::length(input) > 0.0f) {
    input = glm::normalize(input);
  }

  input *= m_speed;

  m_vel.x = input.x;
  m_vel.z = input.z;

  physx::PxControllerFilters filters;
  physx::PxVec3 disp = glmToPxVec3(m_vel * dt);
  physx::PxControllerCollisionFlags flags =
      m_control->move(disp, 0.001f, dt, filters);

  if (flags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN) {
    if (m_vel.y < 0.0f) {
      m_vel.y = -64.0f;
    }

    is_ground = true;
  }

  if (inp.jump && is_ground) {
    m_vel.y = 320.0f;
    is_ground = false;
  }

  if (!is_ground) {
    m_vel.y -= 640.0f * dt;
  }

  m_pos = pxToGlmExtendedVec3(m_control->getPosition());

  // Transform trans;
  // trans.position = m_pos;

  // m_model->setTransformitions(trans);
}

void Player::draw(Camera &cam) {
  // m_model->draw(cam);
  cam.setPosition(m_pos);
  m_cam_front = cam.getFront();
  m_cam_right = cam.getRight();
}

void Player::drawDebug(Camera &cam) {
  // physx::PxTransform pose = m_collider->getGlobalPose();
  // glm::mat4 model = pxToGlmMat4(pose);
  //
  // vs_params_shape_t vs_params = {};
  // vs_params.mvp = cam.getMatrix() * model;
  //
  // m_collider_shape->bind();
  // sg_apply_uniforms(UB_vs_params_shape, SG_RANGE_REF(vs_params));
  // m_collider_shape->draw(cam);
}

void Player::initPhysics(physx::PxPhysics *physics,
                         physx::PxControllerManager *mgr) {
  physx::PxExtendedVec3 v(glmToPxExtendedVec3(m_pos));

  physx::PxU32 mat_nb = physics->getNbMaterials();
  LogInfo(FMT_I32, mat_nb);
  std::vector<physx::PxMaterial *> materials(mat_nb);
  physics->getMaterials(
      reinterpret_cast<physx::PxMaterial **>(materials.data()), mat_nb);

  physx::PxCapsuleControllerDesc desc;
  desc.height = 56.0f - 2 * 16;
  desc.radius = 32.0f / 2;
  desc.position = v;
  desc.material = materials[0];
  desc.contactOffset = desc.radius * 0.1f;
  desc.stepOffset = desc.height * 0.25f;
  desc.slopeLimit = cosf(physx::PxPi / 4);
  desc.nonWalkableMode =
      physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING;

  m_control = mgr->createController(desc);
}

bool Player::isOnGround(physx::PxScene *scene) {
  f32 tolerance = 0.1f;

  physx::PxRaycastBuffer hit;
  physx::PxVec3 origin(physx::toVec3(m_control->getPosition()));
  f32 w = 32.0f / 2;
  f32 h = 32.0f;
  f32 bottom = origin.y - (56.0f * 0.5f + ((w * w + h * h) / (2 * h)));

  origin.y = bottom + tolerance;

  physx::PxVec3 dir(0.0f, -1.0f, 0.0f);

  physx::PxControllerState state;
  m_control->getState(state);

  // physx::PxQueryFilterData filter;
  IgnoreControllerFilter filter(m_control);

  if (state.collisionFlags &
      physx::PxControllerCollisionFlag::eCOLLISION_DOWN) {
    return true;
  }

  // return false;

  bool ray = scene->raycast(origin, dir, tolerance * 2, hit,
                            physx::PxHitFlag::eDEFAULT,
                            physx::PxQueryFilterData(), &filter);

  LogError("OLHA: " FMT_U8, ray);

  return ray;
}

void Player::jump(f32 strength) {
  // physx::PxVec3 v = m_collider->getLinearVelocity();
  // v.y = strength;
  // m_collider->setLinearVelocity(v);
}
