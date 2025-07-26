#include "sokol_gfx.h"

#include "mesh.hpp"

Mesh::Mesh(std::vector<Vertex> v, std::vector<u16> i)
    : vertices(v), indices(i) {
  setupMesh();
}

Mesh::~Mesh() {}

void Mesh::draw() {
  sg_apply_bindings(&m_bind);
  sg_draw(0, indices.size() * 3, 1);
}

void Mesh::setupMesh() {
  sg_buffer_desc vbuf_desc = {};
  vbuf_desc.data.ptr = vertices.data();
  vbuf_desc.data.size = vertices.size() * sizeof(Vertex);

  sg_buffer_desc ibuf_desc = {};
  ibuf_desc.usage.index_buffer = true;
  ibuf_desc.data.ptr = indices.data();
  ibuf_desc.data.size = indices.size() * sizeof(u16);

  sg_buffer vbuf = sg_make_buffer(&vbuf_desc);
  sg_buffer ibuf = sg_make_buffer(&ibuf_desc);

  m_bind = {};
  m_bind.vertex_buffers[0] = vbuf;
  m_bind.index_buffer = ibuf;
}

// Mesh::~Mesh() {
//   vertices.clear();
//   indices.clear();
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
//   // if (indices.size() != 0) {
//     gl::drawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
//   // } else {
//   //   gl::drawArrays(GL_TRIANGLES, 0, vertices.size());
//   // }
//   glBindVertexArray(0);
//   gl::activeTexture(GL_TEXTURE0);
// }

// void Mesh::setupMesh() {
//   gl::genVertexArrays(1, &VAO);
//   gl::genBuffers(1, &VBO);
//   gl::genBuffers(1, &EBO);
//
//   glBindVertexArray(VAO);
//   gl::bindBuffer(GL_ARRAY_BUFFER, VBO);
//
//   gl::bufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
//                  vertices.data(), GL_STATIC_DRAW);
//
//   gl::bindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//   gl::bufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32),
//                  indices.data(), GL_STATIC_DRAW);
//
//   // Vertex
//   gl::enableVertexAttribArray(0);
//   gl::vertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
//                           (void *)offsetof(Vertex, pos));
//
//   // Vertex colors
//   gl::enableVertexAttribArray(1);
//   gl::vertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
//                           (void *)offsetof(Vertex, color));
//   // Vertex normals
//   gl::enableVertexAttribArray(2);
//   gl::vertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
//                           (void *)offsetof(Vertex, normal));
//   // Vertex texture coords
//   gl::enableVertexAttribArray(3);
//   gl::vertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
//                           (void *)offsetof(Vertex, tex_coords));
//
//   glBindVertexArray(0);
// }
