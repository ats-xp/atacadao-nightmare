#include "physics.hpp"

#include <ext/scalar_constants.hpp>

constexpr float EPS = glm::epsilon<float>();

bool rayAABB(const glm::vec3 &ray_origin, const glm::vec3 &ray_dir,
             const AABB &aabb, f32 &out_t_enter, f32 &out_t_exit,
             glm::vec3 &out_normal) {
  out_t_enter = -std::numeric_limits<float>::infinity();
  out_t_exit = std::numeric_limits<float>::infinity();
  glm::vec3 enter_normal(0.0f);

  for (int i = 0; i < 3; i++) {
    f32 origin = ray_origin[i];
    f32 dir = ray_dir[i];
    f32 min = aabb.min[i];
    f32 max = aabb.max[i];

    if (std::abs(dir) < EPS) {
      if (origin < min || origin > max)
        return false;
    } else {
      f32 t1 = (min - origin) / dir;
      f32 t2 = (max - origin) / dir;
      f32 sign_norm = 0.0f;

      if (t1 > t2) {
        std::swap(t1, t2);
        sign_norm = 1.0f;
      }

      if (t1 > out_t_enter) {
        out_t_enter = t1;
        enter_normal = glm::vec3(0.0f);
        enter_normal[i] = (sign_norm == 0.0f) ? -1.0f : 1.0f;
      }

      out_t_exit = std::min(out_t_exit, t2);
      if (out_t_enter > out_t_exit) {
        return false;
      }
    }
  }

  out_normal = enter_normal;
  return true;
}

TraceResult sweepAABBABB(const AABB &moving_box, const glm::vec3 &start_center,
                         const glm::vec3 &end_center, const AABB &static_box) {
  TraceResult result;
  glm::vec3 hsize = moving_box.getHalfSize();
  glm::vec3 expanded_min = static_box.min - hsize;
  glm::vec3 expanded_max = static_box.max + hsize;

  glm::vec3 ray_dir = end_center - start_center;
  glm::vec3 normal;
  f32 t_enter, t_exit;

  // Check if start inside
  if (start_center.x >= expanded_min.x && start_center.x <= expanded_max.x &&
      start_center.y >= expanded_min.y && start_center.y <= expanded_max.y &&
      start_center.z >= expanded_min.z && start_center.z <= expanded_max.z) {
    result.start_inside = true;
    result.hit = true;
    result.t = 0.0f;

    glm::vec3 static_center = static_box.getCenter();
    glm::vec3 dir =
        glm::normalize(start_center - static_center + glm::vec3(EPS));

    result.hit_normal = dir;
    result.end_pos = start_center;

    return result;
  }

  AABB aabb;
  aabb.min = expanded_min;
  aabb.max = expanded_max;

  if ((glm::length(ray_dir) < EPS) ||
      (!rayAABB(start_center, ray_dir, aabb, t_enter, t_exit, normal)) ||
      (t_enter < 0.0f || t_enter > 1.0f)) {
    result.hit = false;
    result.t = 1.0f;
    result.end_pos = end_center;
    return result;
  }

  result.hit = true;
  result.t = std::max(0.0f, t_enter);
  result.hit_normal = normal;
  result.end_pos = start_center + ray_dir * result.t;

  return result;
}

TraceResult sweepAABBPlane(const AABB &moving_box,
                           const glm::vec3 &start_center,
                           const glm::vec3 &end_center, const Plane &plane) {
  TraceResult result;
  glm::vec3 hsize = moving_box.getHalfSize();

  glm::vec3 support = start_center + glm::sign(plane.normal) * hsize;
  f32 start_dist = glm::dot(plane.normal, support) - plane.dist;

  support = end_center + glm::sign(plane.normal) * hsize;
  f32 end_dist = glm::dot(plane.normal, support) - plane.dist;

  if (start_dist < 0.0f && end_dist < 0.0f) {
    result.start_inside = true;
    result.hit = true;
    result.t = 0.0f;
    result.hit_normal = plane.normal;
    result.end_pos = start_center;
    return result;
  }

  f32 denom = start_dist - end_dist;
  if (std::abs(denom) < EPS) {
    result.hit = false;
    result.t = 1.0f;
    result.end_pos = end_center;
    return result;
  }

  f32 t = start_dist / denom;
  if (t >= 0.0f && t <= 1.0f) {
    result.hit = true;
    result.t = t;
    result.hit_normal = plane.normal;
    result.end_pos = start_center + (end_center - start_center) * t;
    return result;
  }

  result.hit = false;
  result.t = 1.0f;
  result.end_pos = end_center;

  return result;
}

bool pointInTriangle(const glm::vec3 &p, const glm::vec3 &a, const glm::vec3 &b,
                     const glm::vec3 &c) {
  glm::vec3 v0 = b - a;
  glm::vec3 v1 = c - a;
  glm::vec3 v2 = p - a;

  f32 d00 = glm::dot(v0, v0);
  f32 d01 = glm::dot(v0, v1);
  f32 d11 = glm::dot(v1, v1);
  f32 d20 = glm::dot(v2, v0);
  f32 d21 = glm::dot(v2, v1);

  f32 denom = d00 * d11 - d01 * d01;
  if (std::abs(denom) < EPS)
    return false;

  f32 v = (d11 * d20 - d01 * d21) / denom;
  f32 w = (d00 * d21 - d01 * d20) / denom;
  f32 u = 1.0f - v - w;

  return (u >= EPS && v >= EPS && w >= EPS);
}

TraceResult sweepAABBRamp(const AABB &moving_box, const glm::vec3 &start_center,
                          const glm::vec3 &end_center, const Ramp &ramp) {
  TraceResult tr =
      sweepAABBPlane(moving_box, start_center, end_center, ramp.plane);
  if (!tr.hit) {
    return tr;
  }

  glm::vec3 contact_point = tr.end_pos;
  f32 dist = glm::dot(ramp.plane.normal, contact_point) - ramp.plane.dist;
  contact_point -= ramp.plane.normal * dist;

  if (!pointInTriangle(contact_point, ramp.v0, ramp.v1, ramp.v2)) {
    tr.hit = false;
  }

  return tr;
}

glm::vec3 clipVelocity(const glm::vec3 &vel, const glm::vec3 &normal,
                       f32 overbounce) {
  f32 d = glm::dot(vel, normal);

  if (d > 0.0f) {
    return vel;
  }

  f32 backoff = d * overbounce;
  glm::vec3 clipped = vel - normal * backoff;

  for (int i = 0; i < 3; i++)
    if (std::abs(clipped[i]) < EPS)
      clipped[i] = 0.0f;

  return clipped;
}

MoveResult stepSlideMove(const AABB &moving_box, const glm::vec3 &start_center,
                         glm::vec3 velocity, f32 dt,
                         const std::vector<AABB> &static_boxes,
                         const std::vector<Ramp> &ramps, int max_iterations) {
  glm::vec3 origin = start_center;
  glm::vec3 remain = velocity * dt;
  glm::vec3 dest = origin + remain;
  glm::vec3 new_vel = velocity;

  for (const auto &b : static_boxes) {
    AABB mb;
    glm::vec3 hsize = moving_box.getHalfSize();
    mb.min = origin - hsize;
    mb.max = origin + hsize;

    if (mb.max.x < b.min.x || mb.min.x > b.max.x || mb.max.y < b.min.y ||
        mb.min.y > b.max.y || mb.max.z < b.min.z || mb.min.z > b.max.z) {
      resolvePenetrationBox(mb, origin, b);
    }
  }

  for (int i = 0; i < max_iterations; i++) {
    TraceResult earliest;
    earliest.t = 1.0f;
    earliest.hit = false;

    // Boxes
    for (const auto &b : static_boxes) {
      TraceResult result = sweepAABBABB(moving_box, origin, origin + remain, b);
      if (result.hit && result.t < earliest.t)
        earliest = result;
    }

    // Ramps
    for (const auto &r : ramps) {
      TraceResult tr = sweepAABBRamp(moving_box, origin, origin + remain, r);
      if (tr.hit && tr.t < earliest.t) {
        LogInfo("Ramp");
        earliest = tr;
      } else {
        LogError("Ramp");
      }
    }

    // Planes
    // for (const auto &p : planes) {
    //   TraceResult result =
    //       sweepAABBPlane(moving_box, origin, origin + remain, p);
    //
    //   if (result.hit && result.t < earliest.t)
    //     earliest = result;
    // }

    if (!earliest.hit) {
      // free move
      origin += remain;
      break;
    }

    f32 move_t = std::max(0.0f, earliest.t - EPS);
    origin += remain * move_t;

    f32 used = earliest.t;
    f32 remaining_fraction = 1.0f - used;
    remain *= remaining_fraction;

    new_vel = clipVelocity(new_vel, earliest.hit_normal, 1.01f);
    remain = clipVelocity(remain, earliest.hit_normal, 1.01f);

    if (glm::length(remain) < EPS)
      break;
  }

  MoveResult result;
  result.final_center = origin;
  result.final_velocity = new_vel;
  return result;
}

void resolvePenetrationBox(AABB &moving_box, glm::vec3 &center,
                           const AABB &static_box) {
  glm::vec3 overlap_min = glm::max(moving_box.min, static_box.min);
  glm::vec3 overlap_max = glm::min(moving_box.max, static_box.max);
  glm::vec3 overlaps = overlap_max - overlap_min;

  // Se algum overlap é negativo, não está penetrando
  if (overlaps.x <= 0 || overlaps.y <= 0 || overlaps.z <= 0)
    return;

  // Encontre o menor overlap
  if (overlaps.x <= overlaps.y && overlaps.x <= overlaps.z) {
    // Empurre o eixo
    f32 sign = (center.x > static_box.getCenter().x) ? 1.0f : -1.0f;
    center.x += sign * overlaps.x;
  } else if (overlaps.y <= overlaps.x && overlaps.y <= overlaps.z) {
    // Warning: Olhe se isto será afetado pelas coordenadas do OpenGL
    f32 sign = (center.y > static_box.getCenter().y) ? 1.0f : -1.0f;
    center.y += sign * overlaps.y;
  } else {
    f32 sign = (center.z > static_box.getCenter().z) ? 1.0f : -1.0f;
    center.z += sign * overlaps.z;
  }
}
