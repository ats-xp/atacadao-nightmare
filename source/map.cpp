#include "map.hpp"
#include "asset_manager.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>

#undef S
#include "base.h"
#include "physx_utils.hpp"

#include <cooking/PxCooking.h>

#include "map.glsl.h"

TBMap loadMap(const std::string &filename);

void Map::init(const char *path) {
  sg_pipeline_desc desc = {};
  sg_blend_state blend_state = {};

  blend_state.enabled = true;
  blend_state.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
  blend_state.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
  blend_state.src_factor_alpha = SG_BLENDFACTOR_ZERO;
  blend_state.dst_factor_alpha = SG_BLENDFACTOR_ONE;

  desc.colors[0].blend = blend_state;

  desc.layout.buffers[0].stride = sizeof(Vertex);
  desc.layout.attrs[ATTR_map_apos].format = SG_VERTEXFORMAT_FLOAT3;
  desc.layout.attrs[ATTR_map_anormal].format = SG_VERTEXFORMAT_FLOAT3;
  desc.layout.attrs[ATTR_map_atex_coords].format = SG_VERTEXFORMAT_FLOAT2;
  desc.layout.attrs[ATTR_map_alightmap_coords].format = SG_VERTEXFORMAT_FLOAT2;

  desc.index_type = SG_INDEXTYPE_UINT16;
  desc.cull_mode = SG_CULLMODE_FRONT;
  desc.depth.write_enabled = true;
  desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;

  model = new Model(path);
  model->m_vao.create(desc, map_shader_desc(sg_query_backend()));

  map = loadMap("assets/maps/test/test.map");
}

void Map::destroy() { delete model; }

void Map::draw(Camera &cam) {
  // Transform trans = {};
  // trans.scale = glm::vec3(0.1f);

  // model->setTransformitions(trans);
  model->draw(cam);
}

void Map::setupPhysics(PhysicsManager &mgr) {
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

    physx::PxCookingParams cook_params(mgr.m_physics->getTolerancesScale());
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
        mgr.m_physics->createTriangleMesh(read_buffer);

    // physx::PxMeshScale scale(0.1f);
    physx::PxTriangleMeshGeometry geom(triangle_mesh);
    physx::PxRigidStatic *actor =
        mgr.m_physics->createRigidStatic(physx::PxTransform(physx::PxIdentity));
    physx::PxShape *shape = mgr.m_physics->createShape(
        geom, *mgr.m_physics->createMaterial(0.5f, 0.5f, 0.1f));

    actor->attachShape(*shape);
    shape->release();

    mgr.m_scene->addActor(*actor);
  }
}

TBMap loadMap(const std::string &filename) {
  TBMap map;

  std::ifstream file(AssetManager::getPath(filename));
  if (!file.is_open()) {
    LogError(".map file not loaded");
    abort();
  }

  std::string line;

  TBEntity cur_entity;
  TBBrush cur_brush;

  bool in_entity = false;
  bool in_brush = false;

  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '/')
      continue;

    if (line == "{") {
      if (!in_entity) {
        in_entity = true;
        cur_entity = TBEntity{};
      } else if (in_entity && !in_brush) {
        in_brush = true;
        cur_brush = TBBrush{};
      }
    } else if (line == "}") {
      if (in_brush) {
        cur_entity.brushes.push_back(cur_brush);
        in_brush = false;
      } else if (in_entity) {
        map.entities.push_back(cur_entity);
        in_entity = false;
      }
    } else {
      std::istringstream iss(line);

      if (line[0] == '"') {
        std::string key, value;
        iss >> std::quoted(key) >> std::quoted(value);
        cur_entity.properties.push_back({key, value});
      } else if (in_brush) {
        TBFace face;
        char ignore;

        iss >> ignore >> face.v[0].x >> face.v[0].y >> face.v[0].z >> ignore >>
            ignore >> face.v[1].x >> face.v[1].y >> face.v[1].z >> ignore >>
            ignore >> face.v[2].x >> face.v[2].y >> face.v[2].z >> ignore;

        iss >> face.texture >> face.shift_x >> face.shift_y >> face.rotation >>
            face.scale_x >> face.scale_y;

        cur_brush.faces.push_back(face);
      }
    }
  }

  return map;
}
