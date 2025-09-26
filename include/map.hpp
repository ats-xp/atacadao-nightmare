#pragma once

#include "model.hpp"
#include "physics_manager.hpp"

#include <sstream>

struct TBFace {
  glm::vec3 v[3];
  std::string texture;
  f32 shift_x, shift_y;
  f32 rotation;
  f32 scale_x, scale_y;
};

struct TBBrush {
  std::vector<TBFace> faces;
};

struct TBEntity {
  std::vector<std::pair<std::string, std::string>> properties;
  std::vector<TBBrush> brushes;

  std::string getProperty(std::string_view name) {
    for (auto &p : properties) {
      if (p.first == name)
        return p.second;
    }

    return "";
  }
};

struct TBMap {
  std::vector<TBEntity> entities;

  const TBEntity &getEntity(std::string_view name) {
    for (const auto &ent : entities) {
      for (size_t i = 0; i < ent.properties.size(); i++) {
        auto &p = ent.properties.at(i);

        if (p.first != "classname")
          continue;

        if (p.second == name)
          return ent;
      }
    }

    LogError("TB Entity Name not found");
    return entities[0];
  }

  std::vector<TBEntity> getEntityList(std::string_view name) {
    std::vector<TBEntity> list;

    for (const auto &ent : entities) {
      for (size_t i = 0; i < ent.properties.size(); i++) {
        auto &p = ent.properties.at(i);

        if (p.first != "classname")
          continue;

        if (p.second == name)
          list.push_back(ent);
      }
    }

    if (list.empty())
      LogError("TB Entity List Name not found");
    return list;
  }
};

struct Map {
private:

public:
  TBMap map = {};
  Model *model = nullptr;

  void init(const char *path);
  void destroy();

  void draw(Camera &cam);

  void setupPhysics(PhysicsManager &mgr);

  inline glm::vec3 getAttributeVec3(std::string_view name) {
    for (const auto &ent : map.entities) {
      for (size_t i = 0; i < ent.properties.size(); i++) {
        auto &p = ent.properties.at(i);

        if (p.second == name) {
          auto &p2 = ent.properties.at(i + 1);

          if (p2.first == "origin") {
            return parserStringToVec3(p2.second);
          }
        }
      }
    }

    return glm::vec3(0.0f);
  }

  static inline glm::vec3 parserStringToVec3(const std::string &str) {
    std::stringstream ss(str);
    glm::vec3 v;

    // É necessario inverter as posições
    // Y e Z, pois Quake costuma usar o eixo
    // Z para altura e Y para profundidade.
    ss >> v.x >> v.z >> v.y;

    // Também é preciso inverter o sinal
    // da posição Z, pois a Quake-Engine
    // não trabalha com este eixo da mesma
    // forma que o OpenGL.
    v.z = -v.z;

    return v;
  }
};
