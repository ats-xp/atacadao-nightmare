#pragma once

#include <glm.hpp>

#include "base.h"

struct AABB {
  glm::vec3 min, max;

  constexpr glm::vec3 getCenter() const { return (min + max) * 0.5f; };
  constexpr glm::vec3 getSize() const { return (max - min); };
  constexpr glm::vec3 getHalfSize() const { return (max - min) * 0.5f; };
};

struct Plane {
  glm::vec3 normal;
  f32 dist;
};

struct TraceResult {
  glm::vec3 hit_normal;
  glm::vec3 end_pos;
  f32 t = 1.0f;
  bool hit;
  bool start_inside;
};

struct MoveResult {
  glm::vec3 final_center;
  glm::vec3 final_velocity;
  bool hit;
};

bool rayAABB(const glm::vec3 &ray_origin, const glm::vec3 &ray_dir,
             const AABB &aabb, f32 &out_t_enter, f32 &out_t_exit,
             glm::vec3 &out_normal);

TraceResult sweepAABBABB(const AABB &moving_box, const glm::vec3 &start_center,
                         const glm::vec3 &end_center, const AABB &static_box);

glm::vec3 clipVelocity(const glm::vec3 &vel, const glm::vec3 &normal,
                       f32 overbounce = 1.0f);

MoveResult stepSlideMove(const AABB &moving_box, const glm::vec3 &start_center,
                         glm::vec3 velocity, f32 dt,
                         const std::vector<AABB> &static_boxes,
                         const std::vector<Plane> &planes,
                         int max_iterations = 4);

void resolvePenetrationBox(AABB &moving_box, glm::vec3 &center,
                           const AABB &static_box);
