/*
 *
 * TODO Adicionar mais opções para personilização
 * TODO Alterar a nomenclatura de algumas variaveis temp/membros
 *
 */
#include "billboard.hpp"

#include "asset_manager.hpp"

// tmp
#include "billboard.glsl.h"

Billboard::Billboard(const char *texture) {
  Texture t;
  std::vector<Vertex> vtx;
  std::vector<u16> ind = {0, 1, 2, 0, 2, 3};
  std::vector<std::string> tex_id;

  // glm::vec3 mul(MUL_X, MUL_Y, MUL_Z);

  vtx.push_back(Vertex(glm::vec3(-0.5f, 0.5f, 0.0f),
                       glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
  vtx.push_back(Vertex(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                       glm::vec2(1.0f, 0.0f)));
  vtx.push_back(Vertex(glm::vec3(0.5f, -0.5f, 0.0f),
                       glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)));
  vtx.push_back(Vertex(glm::vec3(-0.5f, -0.5f, 0.0f),
                       glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)));

  tex_id.push_back(texture);

  m_mesh = new Mesh(vtx, ind, tex_id);

  sg_pipeline_desc desc = {};

  sg_blend_state blend_state = {};
  blend_state.enabled = true;
  blend_state.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
  blend_state.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
  blend_state.src_factor_alpha = SG_BLENDFACTOR_ZERO;
  blend_state.dst_factor_alpha = SG_BLENDFACTOR_ONE;

  desc.colors[0].blend = blend_state;

  desc.layout.buffers[0].stride = sizeof(Vertex);

  desc.layout.attrs[ATTR_billboard_apos].format = SG_VERTEXFORMAT_FLOAT3;
  desc.layout.attrs[ATTR_billboard_apos].offset = 0;

  desc.layout.attrs[ATTR_billboard_atex_coords].format = SG_VERTEXFORMAT_FLOAT2;
  desc.layout.attrs[ATTR_billboard_atex_coords].offset =
      offsetof(Vertex, tex_coords);

  // desc.layout.attrs[ATTR_default_anormal].format = SG_VERTEXFORMAT_FLOAT3;

  desc.index_type = SG_INDEXTYPE_UINT16;
  desc.cull_mode = SG_CULLMODE_BACK;
  desc.depth.compare = SG_COMPAREFUNC_LESS;
  desc.depth.write_enabled = true;

  m_vao.create(desc, billboard_shader_desc(sg_query_backend()));
}

Billboard::~Billboard() {
  m_mesh->destroy();
  m_vao.destroy();
}

void Billboard::draw(Camera &cam) {
  vs_billboard_params_t vs_params = {};
  vs_params.proj = cam.getProjectionMatrix();
  vs_params.view = cam.getViewMatrix();
  vs_params.center = m_pos;
  vs_params.size = glm::vec2(64, 64);

  sg_range range = SG_RANGE_REF(vs_params);
  Texture t = AssetManager::getTextureFromID(
      AssetManager::getTextureIDFromPath(m_mesh->m_textures_path[0]));

  sg_bindings bind = {};
  bind.vertex_buffers[0] = m_mesh->m_vbo.getBuffer();
  bind.index_buffer = m_mesh->m_ebo.getBuffer();
  bind.images[0] = t.img;
  bind.samplers[0] = t.smp;

  m_vao.use();
  m_mesh->draw(bind, 0, &range);
}
