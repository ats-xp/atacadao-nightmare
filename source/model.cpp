#include "model.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "stb_image.h"

#include "default.glsl.h"

Model::Model(const char *path) : m_scale(1.0f, 1.0f, 1.0f) {
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

  LogInfo("Model created");
}

Model::~Model() {
  m_textures_loaded.clear();

  for (auto m : m_meshes) {
    m.destroy();
  }
  m_meshes.clear();

  LogInfo("Model destroyed");
}

void Model::draw(Camera &cam) {
  m_model = glm::mat4(1.0f);
  m_model = glm::translate(m_model, m_pos);
  m_model = glm::scale(m_model, m_scale);

  for (size_t i = 0; i < m_meshes.size(); i++) {
    auto &m = m_meshes[i];
    m.begin();

    vs_params_t vs_params = {};
    vs_params.mvp = cam.getMatrix() * m_model;
    sg_apply_uniforms(UB_vs_params, SG_RANGE_REF(vs_params));

    m.draw(cam);

    m.end();
  }
}

void Model::processNode(aiNode *node, const aiScene *scene) {
  u32 i;
  for (i = 0; i < node->mNumMeshes; i++) {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    m_meshes.push_back(processMesh(mesh, scene));
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

    vertex.color = {1.0f, 1.0f, 1.0f, 1.0f};

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

    for (u32 j = 0; j < m_textures_loaded.size(); j++) {
      Texture &t = m_textures_loaded.at(j);

      if (strcmp(t.path.data(), str.C_Str()) == 0) {
        textures.push_back(t);
        skip = true;
        break;
      }
    }

    if (!skip) {
      sg_sampler_desc desc = {};
      Texture tex = {};

      desc.min_filter = SG_FILTER_LINEAR;
      desc.mag_filter = SG_FILTER_LINEAR;
      tex.smp = sg_make_sampler(&desc);

      tex.id = textureFromFile(str.C_Str(), m_directory.c_str());

      tex.type = (TextureType)tex_type;
      tex.path = str.C_Str();

      textures.push_back(tex);
      m_textures_loaded.push_back(tex);
    }
  }

  return textures;
}

u32 Model::textureFromFile(const char *path, const char *dir) {
  const char *slash = "/";
  char filename[512];

  strcpy(filename, dir);
  strcat(filename, slash);
  strcat(filename, path);

  sg_image img = {};

  int w, h, nch;
  stbi_set_flip_vertically_on_load(true);
  stbi_uc *data = stbi_load(filename, &w, &h, &nch, 4);

  if (data) {
    LogInfo("Loading texture: %s", filename);

    sg_image_desc desc = {};
    desc.type = SG_IMAGETYPE_2D;
    desc.width = w;
    desc.height = h;
    desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    desc.data.subimage[0][0].ptr = data;
    desc.data.subimage[0][0].size = w * h * 4;

    img = sg_make_image(&desc);
  } else {
    LogError("Texture loading error %s file: %s", stbi_failure_reason(),
             filename);
    img.id = 0;
  }

  stbi_image_free(data);

  return img.id;
}

void Model::setPos(glm::vec3 pos) {
  m_pos = pos;
  // for (Mesh m : m_meshes) {
  //   m.setPos(pos);
  // }
}

void Model::setScale(glm::vec3 scale) {
  m_scale = scale;
  // for (Mesh m : m_meshes) {
  //   m.setScale(scale);
  // }
}

void Model::setRotation(glm::vec3 rot) {
  // TODO
  // for (Mesh m : m_meshes) {
  //   m.setRotation(rot);
  // }
}
