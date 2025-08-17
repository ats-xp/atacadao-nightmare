#include "player.hpp"

#include "shape.glsl.h"

#include "physx_utils.hpp"

Player::Player(const glm::vec3 &pos) {
  Transform trans;
  trans.position = pos;

  m_pos = pos;
  m_vel = glm::vec3(0.0f);
  m_speed = 3.0f;

  m_model = new Model("assets/models/Beat/Beat.obj");
  m_model->setTransformitions(trans);

  m_collider_shape = new Shape(m_pos);
}

Player::~Player() {
  m_collider->release();
  m_collider = nullptr;

  delete m_model;
}

void Player::input(Input &inp) {
  m_vel = glm::vec3(0.0f);

  if (inp.up) {
    m_vel.x = m_cam_front.x;
    m_vel.z = m_cam_front.z;
  } else if (inp.down) {
    m_vel.x = -m_cam_front.x;
    m_vel.z = -m_cam_front.z;
  }

  if (inp.left) {
    m_vel.x = -m_cam_right.x;
    m_vel.z = -m_cam_right.z;
  } else if (inp.right) {
    m_vel.x = m_cam_right.x;
    m_vel.z = m_cam_right.z;
  }

  if (inp.action && is_ground) {
    jump(8.0f);
  }
}

void Player::update(f32 dt) {
  m_vel.x *= m_speed;
  m_vel.z *= m_speed;

  physx::PxTransform pose = m_collider->getGlobalPose();
  m_pos = pxToGlmVec3(pose.p);

  m_vel.y = m_collider->getLinearVelocity().y;
  m_collider->setLinearVelocity(glmToPxVec3(m_vel));

  Transform trans;
  trans.position = m_pos;

  // m_model->setTransformitions(trans);
}

void Player::draw(Camera &cam) {
  m_model->draw(cam);
  cam.setPosition(m_pos);
  m_cam_front = cam.getFront();
  m_cam_right = cam.getRight();
}

void Player::drawDebug(Camera &cam) {
  physx::PxTransform pose = m_collider->getGlobalPose();
  glm::mat4 model = pxToGlmMat4(pose);

  vs_params_shape_t vs_params = {};
  vs_params.mvp = cam.getMatrix() * model;

  m_collider_shape->bind();
  sg_apply_uniforms(UB_vs_params_shape, SG_RANGE_REF(vs_params));
  m_collider_shape->draw(cam);
}

void Player::initPhysics(physx::PxPhysics *physics, physx::PxMaterial *material,
                         physx::PxScene *scene) {
  physx::PxTransform t(glmToPxVec3(m_pos));
  physx::PxBoxGeometry geom(physx::PxVec3(0.5f, 0.5f, 0.5f));

  f32 density = 20.0f;

  m_collider = physx::PxCreateDynamic(*physics, t, geom, *material, density);

  m_collider->setRigidDynamicLockFlag(
      physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
  m_collider->setRigidDynamicLockFlag(
      physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
  m_collider->setRigidDynamicLockFlag(
      physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);

  scene->addActor(*m_collider);
}

bool Player::isOnGround(physx::PxScene *scene) {
  f32 half_height =  0.5f;
  physx::PxTransform pose = m_collider->getGlobalPose();
  physx::PxVec3 dir(0.0f, -1.0f, 0.0f);
  physx::PxVec3 origin = pose.p - physx::PxVec3(0.0f, half_height - 0.05f, 0.0f);
  f32 max_dist = 0.1f;

  physx::PxRaycastBuffer hit;
  physx::PxQueryFilterData filter(physx::PxQueryFlag::eSTATIC);

  bool status = scene->raycast(origin, dir, max_dist, hit,
                               physx::PxHitFlag::eDEFAULT, filter);
  return status && hit.hasBlock;
}

void Player::jump(f32 strength) {
  physx::PxVec3 v = m_collider->getLinearVelocity();
  v.y = strength;
  m_collider->setLinearVelocity(v);
}
