#pragma once

#include <PxPhysicsAPI.h>

#include "model.hpp"

struct Map {
private:
  Model *model;

public:
  void init(const char *path);
  void destroy();

  void draw(Camera &cam);

  void setupPhysics(physx::PxPhysics *physics, physx::PxScene *scene);
};
