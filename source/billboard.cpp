/*
 *
 * TODO Adicionar mais opções para personilização
 * TODO Alterar a nomenclatura de algumas variaveis temp/membros
 *
 */
#include "billboard.hpp"

// tmp
#include "billboard.glsl.h"

#include "stb_image.h"

Billboard::Billboard(const char *texture) {
  Texture t;
  std::vector<Vertex> vtx;
  std::vector<u16> ind = {0, 1, 2, 0, 2, 3};
  std::vector<Texture> tex;

  stbi_set_flip_vertically_on_load(false);

  vtx.push_back(Vertex(glm::vec3(-0.5f, 0.5f, 0.0f),
                       glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
  vtx.push_back(Vertex(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                       glm::vec2(1.0f, 0.0f)));
  vtx.push_back(Vertex(glm::vec3(0.5f, -0.5f, 0.0f),
                       glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)));
  vtx.push_back(Vertex(glm::vec3(-0.5f, -0.5f, 0.0f),
                       glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)));

  t.load(texture);
  t.attrib(SG_FILTER_NEAREST, SG_FILTER_NEAREST, SG_WRAP_CLAMP_TO_EDGE,
           SG_WRAP_CLAMP_TO_EDGE);
  tex.push_back(t);

  m_mesh = new Mesh(vtx, ind, tex);
}

Billboard::~Billboard() {}

void Billboard::draw(Camera &cam) {
  vs_billboard_params_t vs_params = {};
  vs_params.proj = cam.getProjectionMatrix();
  vs_params.view = cam.getViewMatrix();
  vs_params.center = m_position;

  m_mesh->bind(IMG_tex, SMP_smp);

  sg_apply_uniforms(UB_vs_billboard_params, SG_RANGE_REF(vs_params));

  m_mesh->draw();
}
