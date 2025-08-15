/*
 *
 * TODO Otimizar a 'load texture'
 * TODO Carregar as texturas de forma assincrona'
 * TODO Aprimorar a Model Store
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

static void fecthTextureCallback(const sfetch_response_t *response);

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

  u32 flags =
      aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenNormals;

  Assimp::Importer imp;

  LogInfo("Processing model: %s", path);

  const aiScene *scene = imp.ReadFile(path, flags);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    LogError("Assimp failed: %s", imp.GetErrorString());
    return;
  }

  std::string p = std::string(path);
  p = p.substr(0, p.find_last_of("/"));
  m_directory = p.c_str();

  processNode(scene->mRootNode, scene);
}

void Model::draw(Camera &cam) {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, m_transformition.position);
  // model = glm::scale(model, m_scale);
  // model = glm::rotate(model, glm::radians(m_rotation), m_axis);

  vs_params_t vs_params = {};
  vs_params.mvp = cam.getMatrix() * model;

  // 11.807526  -- class
  // 9.103988   -- struct
  // 7.926278 -- alocando/limpando buffer
  // 1.646011 -- buffer alocado uma vez
  // u64 start = stm_now();

  for (Mesh &m : m_meshes) {
    if (m.m_textures.size() == 0 || !m.m_textures[0].loaded) {
      continue;
    }

    m.bind(IMG_tex, SMP_smp);

    sg_apply_uniforms(UB_vs_params, SG_RANGE_REF(vs_params));

    m.draw();
  }

  // u64 elapsed = stm_since(start);
  // LogInfo("%lf", stm_ms(elapsed));
  // abort();
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

  std::vector<Texture> diffuse_maps =
      loadMaterialTextures(material, aiTextureType_DIFFUSE, DIFFUSE);
  textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

  std::vector<Texture> specular_maps =
      loadMaterialTextures(material, aiTextureType_SPECULAR, SPECULAR);
  textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());

  std::vector<Texture> normal_maps =
      loadMaterialTextures(material, aiTextureType_HEIGHT, NORMAL);
  textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());

  std::vector<Texture> height_maps =
      loadMaterialTextures(material, aiTextureType_AMBIENT, HEIGHT);
  textures.insert(textures.end(), height_maps.begin(), height_maps.end());

  return Mesh(vertices, indices, textures);
}

std::vector<Texture>
Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, u8 tex_type) {
  std::vector<Texture> textures;

  for (u32 i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);

    bool skip = false;

    for (Texture &t : texture_pool) {
      if (strcmp(t.path.data(), str.C_Str()) == 0) {
        skip = true;
        textures.push_back(t);
        break;
      }
    }

    // for (size_t j = 0; j < m_textures_loaded.size(); j++) {
    //   Texture &t = m_textures_loaded.at(j);
    //
    //   if (strcmp(t.path.data(), str.C_Str()) == 0) {
    //     textures.push_back(t);
    //     skip = true;
    //     break;
    //   }
    // }

    if (!skip) {
      std::string filename = getTexturePath(str.C_Str());
      Texture t;

      // sfetch_request_t desc = {};
      // desc.path = filename.c_str();
      // desc.callback = fn;
      // desc.buffer = SFETCH_RANGE(io_texture_buffer);
      // sfetch_send(&desc);
      //
      // for (Texture &t : texture_pool) {
      //   if (strcmp(t.path.data(), str.C_Str()) == 0 && t.loaded) {
      //     textures.push_back(t);
      //   }
      // }

      Texture tex;
      stbi_set_flip_vertically_on_load(true);
      // tex.attrib(SG_FILTER_NEAREST, SG_FILTER_NEAREST, SG_WRAP_CLAMP_TO_EDGE, SG_WRAP_CLAMP_TO_EDGE);
      tex.load(filename.c_str(), (TextureType)tex_type);
      tex.attrib();

      if (tex.loaded) {
        texture_pool.push_back(tex);
        textures.push_back(tex);
      }

      // m_textures_loaded.push_back(tex);
    }
  }

  return textures;
}

void addModelStore(ModelStore &store, Model *model, const Transform &trans) {
  model->setPos(trans.position);

  store.models.push_back(model);
  store.transforms.push_back(trans);
}

void updateModelStore(ModelStore &store) {
  size_t i;
  for (i = 0; i < store.models.size(); i++) {
    Model *m = store.models.at(i);
    Transform &t = store.transforms.at(i);

    // m->m_collider.setPosition(t.position);
    m->setTransformitions(t);
  }
}

void drawModelStore(ModelStore &store, Camera &cam) {
  size_t i;
  for (i = 0; i < store.models.size(); i++) {
    Model *m = store.models.at(i);
    m->draw(cam);
  }
}

static void fecthTextureCallback(const sfetch_response_t *response) {
  if (response->fetched) {

    Texture tex;
    stbi_set_flip_vertically_on_load(true);
    tex.load(response->path);
    tex.attrib();

    if (tex.loaded) {
      texture_pool.push_back(tex);
    }
  }
}
