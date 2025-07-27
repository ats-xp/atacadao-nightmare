#include "sokol_gfx.h"

#include "mesh.hpp"

#include "render.hpp"

#include "default.glsl.h"

Mesh::Mesh(std::vector<Vertex> v, std::vector<u16> i, std::vector<Texture> t)
    : m_vertices(v), m_indices(i), m_textures(t) {
  m_bind = {};
  m_buf = nullptr;
}

Mesh::~Mesh() {}

void Mesh::begin() {
  m_buf = new RenderBuffer(m_vertices, m_indices, m_textures);
  m_buf->setImage(IMG_tex, SMP_smp, m_textures[0]);

  for (size_t i = 0; i < m_textures.size(); i++) {
    if (m_textures[i].id == 0) {
      delete m_buf;
      // m_buf->destroy();
      m_trash = true;
      return;
    }
  }

  m_buf->use();
}

void Mesh::end() {
  if (m_trash)
    return;
  delete m_buf;
  m_buf = nullptr;
}

void Mesh::draw(Camera &cam) {
  if (m_trash)
    return;
  // RenderBuffer buf(m_vertices, m_indices, m_textures);
  // buf.setImage(IMG_tex, SMP_smp, m_textures[0]);

  // Não apaga esta linha se quiser tudo funcionando
  // ;)
  //
  // TODO
  // Teste para observar como estamos descartando
  // as texturas mal carregadas
  // for (size_t i = 0; i < m_textures.size(); i++) {
  //   if (m_textures[i].id == 0) {
  //     buf.destroy();
  //     return;
  //   }
  // }

  // buf.use();

  // m_model = glm::mat4(1.0f);
  // m_model = glm::translate(m_model, m_pos);
  // m_model = glm::scale(m_model, m_scale);
  //
  // vs_params_t vs_params = {};
  // vs_params.mvp = cam.getMatrix() * m_model;
  // sg_apply_uniforms(UB_vs_params, SG_RANGE_REF(vs_params));

  sg_draw(0, m_indices.size() * 3, 1);
}

void Mesh::destroy() {
  for (auto t : m_textures) {
    sg_image img = {};
    img.id = t.id;
    sg_destroy_sampler(t.smp);
    sg_destroy_image(img);
  }

  m_textures.clear();
  m_indices.clear();
  m_vertices.clear();
}

// Mesh::~Mesh() {
//   m_vertices.clear();
//   m_indices.clear();
//   textures.clear();
//
//   GL(glDeleteBuffers(1, &EBO));
//   GL(glDeleteBuffers(1, &VBO));
//   GL(glDeleteBuffers(1, &VAO));
// }
//
// void Mesh::draw(Shader &shd) {
//   shd.use();
//
//   u32 i;
//   u32 diffuse_num = 1;
//   u32 specular_num = 1;
//   u32 normal_num = 1;
//   u32 height_num = 1;
//
//   for (i = 0; i < textures.size(); i++) {
//     GL(glActiveTexture(GL_TEXTURE0 + i));
//
//     const char *type_name[4] = {
//         "texture_diffuse",
//         "texture_specular",
//         "texture_normal",
//         "texture_height",
//     };
//
//     u32 num = 0;
//     TextureType type = textures[i].type;
//
//     if (type == DIFFUSE) {
//       num = diffuse_num++;
//     } else if (type == SPECULAR) {
//       num = specular_num++;
//     } else if (type == NORMAL) {
//       num = normal_num++;
//     } else if (type == HEIGHT) {
//       num = height_num++;
//     }
//
//     char name[32];
//     sprintf(name, "%s%d", type_name[type], num);
//
//     GL(glUniform1i(glGetUniformLocation(shd.id, name), i));
//     GL(glBindTexture(GL_TEXTURE_2D, textures[i].id));
//   }
//
//   glBindVertexArray(VAO);
//   // if (m_indices.size() != 0) {
//     gl::drawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
//   // } else {
//   //   gl::drawArrays(GL_TRIANGLES, 0, m_vertices.size());
//   // }
//   glBindVertexArray(0);
//   gl::activeTexture(GL_TEXTURE0);
// }
