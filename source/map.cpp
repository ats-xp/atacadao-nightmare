#include "map.hpp"

#undef S
#include "base.h"
#include "physx_utils.hpp"

#include <cooking/PxCooking.h>

void Map::init(const char *path) { model = new Model(path); }

void Map::destroy() { delete model; }

void Map::draw(Camera &cam) {
  // Transform trans = {};
  // trans.scale = glm::vec3(0.1f);

  // model->setTransformitions(trans);
  model->draw(cam);
}

void Map::setupPhysics(physx::PxPhysics *physics, physx::PxScene *scene) {
  for (auto &m : model->getMeshes()) {
    physx::PxTriangleMeshDesc desc = {};
    desc.points.count = static_cast<physx::PxU32>(m.m_vertices.size());
    desc.points.stride = sizeof(Vertex);
    desc.points.data = &m.m_vertices[0].pos;

    desc.triangles.count = static_cast<physx::PxU32>(m.m_indices.size());
    desc.triangles.stride = 3 * sizeof(u16);
    desc.triangles.data = m.m_indices.data();
    desc.flags |= physx::PxMeshFlag::e16_BIT_INDICES;

    physx::PxDefaultMemoryOutputStream write_buffer;
    physx::PxTriangleMeshCookingResult::Enum result;

    physx::PxCookingParams cook_params(physics->getTolerancesScale());
    cook_params.meshPreprocessParams =
        physx::PxMeshPreprocessingFlag::eWELD_VERTICES;
    cook_params.midphaseDesc = physx::PxMeshMidPhase::eBVH33;

    if (!PxCookTriangleMesh(cook_params, desc, write_buffer, &result)) {
      LogError("Failed to cookin MAP! %d", (int)result);
      return;
    }

    physx::PxDefaultMemoryInputData read_buffer(write_buffer.getData(),
                                                write_buffer.getSize());
    physx::PxTriangleMesh *triangle_mesh =
        physics->createTriangleMesh(read_buffer);

    // physx::PxMeshScale scale(0.1f);
    physx::PxTriangleMeshGeometry geom(triangle_mesh);
    physx::PxRigidStatic *actor =
        physics->createRigidStatic(physx::PxTransform(physx::PxIdentity));
    physx::PxShape *shape =
        physics->createShape(geom, *physics->createMaterial(0.5f, 0.5f, 0.1f));

    actor->attachShape(*shape);
    shape->release();

    scene->addActor(*actor);
  }
}
