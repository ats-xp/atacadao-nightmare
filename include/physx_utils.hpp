#pragma once

#undef S // because of base.h
#include <PxPhysicsAPI.h>

#include <glm.hpp>
#include <gtc/quaternion.hpp>
#include <gtc/matrix_transform.hpp>

constexpr glm::vec3 pxToGlmVec3(const physx::PxVec3 &vec) {
  glm::vec3 v;
  v.x = vec.x;
  v.y = vec.y;
  v.z = vec.z;
  return v;
}


inline physx::PxVec3 glmToPxVec3(const glm::vec3 &vec) {
  physx::PxVec3 v;
  v.x = vec.x;
  v.y = vec.y;
  v.z = vec.z;
  return v;
}

inline glm::mat4 pxToGlmMat4(const physx::PxTransform &t)  {
  glm::quat q(t.q.w, t.q.x, t.q.y, t.q.z);
  glm::mat4 rot = glm::mat4_cast(q);
  glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(t.p.x, t.p.y, t.p.z));
  return trans * rot;
}
