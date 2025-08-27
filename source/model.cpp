/*
 *
 * TODO Melhorar as funções de rotação
 *
 */
#include "model.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "sokol_fetch.h"
#include "sokol_time.h"
#include "stb_image.h"

#include "default.glsl.h"

#include <filesystem>

extern std::filesystem::path g_game_root;

Model::Model(const char *path) { init(path); }

Model::Model(const Model &other) : m_meshes(other.m_meshes) {
  // Warning: por enquanto não estou passando os demais dados, lembre-se disso.
}

Model &Model::operator=(const Model &other) {
  Model tmp(other);
  std::swap(m_meshes, tmp.m_meshes);
  return *this;
}

Model::Model(Model &&other) : m_meshes(other.m_meshes) {
  // Warning: por enquanto não estou passando os demais dados, lembre-se disso.
}

Model &Model::operator=(Model &&other) {
  Model m(other);
  std::swap(m_meshes, m.m_meshes);
  return *this;
}

Model::~Model() {
  for (auto m : m_meshes) {
    m.destroy();
  }
}

void Model::init(const char *path) {
  m_pos = glm::vec3(0.0f, 0.0f, 0.0f);
  m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
  m_axis = glm::vec3(0.0f, 1.0f, 0.0f);
  m_rotation = 0.0f;

  m_transformition.position = glm::vec3(0.0f);
  m_transformition.scale = glm::vec3(1.0f);

  u32 flags =
      aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenNormals;

  Assimp::Importer imp;

  std::string path_root = g_game_root / path;
  LogInfo("Processing model: %s", path_root.c_str());

  const aiScene *scene = imp.ReadFile(path_root.c_str(), flags);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    LogError("Assimp failed: %s", imp.GetErrorString());
    return;
  }

  std::string p = path_root;
  p = p.substr(0, p.find_last_of("/"));
  m_directory = p.c_str();

  processNode(scene->mRootNode, scene);
}

void Model::draw(Camera &cam) {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, m_transformition.position);
  model = glm::scale(model, m_transformition.scale);

  vs_params_t vs_params = {};
  vs_params.mvp = cam.getMatrix() * model;

  for (Mesh &m : m_meshes) {
    m.bind(IMG_tex, SMP_smp);
    sg_apply_uniforms(UB_vs_params, SG_RANGE_REF(vs_params));
    m.draw();
  }
}

void Model::processNode(aiNode *node, const aiScene *scene) {
  u32 i;
  for (i = 0; i < node->mNumMeshes; i++) {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    Mesh m = processMesh(mesh, scene);
    m_meshes.push_back(std::move(m));
  }

  for (i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
  u32 i;
  std::vector<Vertex> vertices;
  std::vector<u16> indices;
  std::vector<Texture> textures;
  std::vector<std::string> textures_id;

  for (i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;
    glm::vec3 vector;

    // Vertices
    vector.x = mesh->mVertices[i].x;
    vector.y = mesh->mVertices[i].y;
    vector.z = mesh->mVertices[i].z;
    vertex.pos = vector;

    // Normals
    if (mesh->mNormals != NULL && mesh->mNumVertices > 0) {
      vector.x = mesh->mVertices[i].x;
      vector.y = mesh->mVertices[i].y;
      vector.z = mesh->mVertices[i].z;
      vertex.normal = vector;
    }

    // Textures Coords
    if (mesh->mTextureCoords[0]) {
      glm::vec2 vec;
      vec.x = mesh->mTextureCoords[0][i].x;
      vec.y = mesh->mTextureCoords[0][i].y;
      vertex.tex_coords = vec;
    } else {
      vertex.tex_coords = {0.0f, 0.0f};
    }

    vertices.push_back(vertex);
  }

  for (i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];

    for (u32 j = 0; j < face.mNumIndices; j++) {
      u32 ind = face.mIndices[j];
      indices.push_back(ind);
    }
  }

  aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

  std::vector<std::string> diffuse_maps =
      loadMaterialTextures(material, aiTextureType_DIFFUSE, DIFFUSE);
  textures_id.insert(textures_id.end(), diffuse_maps.begin(),
                     diffuse_maps.end());

  // std::vector<Texture> specular_maps =
  //     loadMaterialTextures(material, aiTextureType_SPECULAR, SPECULAR);
  // textures.insert(textures.end(), specular_maps.begin(),
  // specular_maps.end());
  //
  // std::vector<Texture> normal_maps =
  //     loadMaterialTextures(material, aiTextureType_HEIGHT, NORMAL);
  // textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());
  //
  // std::vector<Texture> height_maps =
  //     loadMaterialTextures(material, aiTextureType_AMBIENT, HEIGHT);
  // textures.insert(textures.end(), height_maps.begin(), height_maps.end());

  return Mesh(vertices, indices, textures_id);
}

std::vector<std::string>
Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, u8 tex_type) {
  std::vector<std::string> ids;

  for (u32 i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);

    // Texture &tex = getTextureFromID(getTextureIDFromPath(str.C_Str()));
    std::string path = getTextureIDFromPath(str.C_Str());

    ids.push_back(path);
  }

  return ids;
}
