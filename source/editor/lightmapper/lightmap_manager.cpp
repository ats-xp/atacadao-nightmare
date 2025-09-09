#include "lightmap_manager.hpp"

#include <stdio.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#define LIGHTMAPPER_IMPLEMENTATION
#include "lightmapper.h"

#include <xatlas.h>

#include "camera.hpp"

static struct {
  GLuint program;
  GLint u_lightmap;
  GLint u_projection;
  GLint u_view;
  GLint u_model;

  GLuint lightmap;
  int w, h;
} scene;

Camera cam(glm::vec3(0.0f));

static LM::Object obj;

static void error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

static int init();
static void drawScene(float *view, float *projection);
static GLuint loadShader(GLenum type, const char *source);
static GLuint loadProgram(const char *vp, const char *fp,
                          const char **attributes, int attributeCount);

static int bake(const char *output) {
  lm_context *ctx = lmCreate(
      16,               // hemisphere resolution (power of two, max=512)
      0.01f, 1000.0f,  // zNear, zFar of hemisphere cameras
      1.0f, 1.0f, 1.0f, // background color (white for ambient occlusion)
      2,
      0.01f, // lightmap interpolation threshold (small differences are
             // interpolated rather than sampled) check
             // debug_interpolation.tga for an overview of sampled (red) vs
             // interpolated (green) pixels.
      0.0f); // modifier for camera-to-surface distance for hemisphere
             // rendering. tweak this to trade-off between interpolated
             // normals quality and other artifacts (see declaration).

  if (!ctx) {
    fprintf(stderr, "Error: Could not initialize lightmapper.\n");
    return 0;
  }

  int c = 3;
  int count = 0;

  printf("\nMeshes Total: %zu\n", obj.meshes.size());

  for (auto &m : obj.meshes) {
    int w = scene.w, h = scene.h;
    float *data = (float *)calloc(w * h * c, sizeof(float));
    lmSetTargetLightmap(ctx, data, w, h, c);

    xatlas::Atlas *atlas = xatlas::Create();

    xatlas::MeshDecl desc;
    desc.vertexCount = m.vertices.size();
    desc.vertexPositionData = &m.vertices[0].pos;
    desc.vertexPositionStride = sizeof(Vertex);

    desc.indexCount = m.indices.size();
    desc.indexData = m.indices.data();
    desc.indexFormat = xatlas::IndexFormat::UInt16;

    xatlas::AddMeshError err = xatlas::AddMesh(atlas, desc);
    xatlas::Generate(atlas);

    if ((int)err != 0) {
      printf("Nao foi\n");
      return 0;
    }

    const xatlas::Mesh &out_mesh = atlas->meshes[0];

    // for (u32 i = 0; i < out_mesh.indexCount; i++) {
    //   u16 v = out_mesh.indexArray[i];
    //   const xatlas::Vertex &vx = out_mesh.vertexArray[v];
    //
    //   m.vertices[m.indices[i]].tex_coords.x = vx.uv[0] / atlas->width;
    //   m.vertices[m.indices[i]].tex_coords.y = vx.uv[1] / atlas->height;

      lmSetGeometry(
          ctx, NULL, // no transformation in this example
          LM_FLOAT, (unsigned char *)m.vertices.data() + offsetof(Vertex, pos),
          sizeof(Vertex), LM_NONE, NULL, 0, LM_FLOAT,
          (unsigned char *)m.vertices.data() + offsetof(Vertex, tex_coords),
          sizeof(Vertex), m.indices.size(), LM_UNSIGNED_SHORT,
          m.indices.data());
    // }

    xatlas::Destroy(atlas);

    int vp[4];
    float view[16], projection[16];
    double lastUpdateTime = 0.0;
    while (lmBegin(ctx, vp, view, projection)) {
      // render to lightmapper framebuffer
      glViewport(vp[0], vp[1], vp[2], vp[3]);
      drawScene(view, projection);

      // display progress every second (printf is expensive)
      double time = glfwGetTime();
      if (time - lastUpdateTime > 1.0) {
        lastUpdateTime = time;
        printf("\r%6.2f%%\n", lmProgress(ctx) * 100.0f);
        fflush(stdout);
      }

      lmEnd(ctx);
    }
    // printf("\rFinished baking %d triangles.\n", scene->indexCount / 3);

    // postprocess texture
    float *temp = static_cast<float *>(calloc(w * h * c, sizeof(float)));
    for (int i = 0; i < 16; i++) {
      lmImageDilate(data, temp, w, h, c);
      lmImageDilate(temp, data, w, h, c);
    }
    lmImageSmooth(data, temp, w, h, c);
    lmImageDilate(temp, data, w, h, c);
    lmImagePower(data, w, h, c, 1.0f / 2.2f,
                 0x7); // gamma correct color channels
    free(temp);

    // save result to a file

    std::string filename = output;
    filename = filename.substr(0, filename.find_last_of("."));
    filename += std::to_string(count) + ".tga";
    if (lmImageSaveTGAf(filename.c_str(), data, w, h, c, 1.0f))
      printf("Saved %s\n", filename.c_str());

    // upload result
    glBindTexture(GL_TEXTURE_2D, scene.lightmap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_FLOAT, data);
    free(data);

    count++;
  }

  lmDestroy(ctx);

  return 1;
}

int main(int argc, char **argv) {
  if (argc <= 1) {
    printf("lightmapper input.obj output.tga\n");
    return -1;
  }

  glfwSetErrorCallback(error_callback);

  if (!glfwInit()) {
    fprintf(stderr, "Could not initialize GLFW.\n");
    return -1;
  }

  // glfwWindowHint(GLFW_RED_BITS, 8);
  // glfwWindowHint(GLFW_GREEN_BITS, 8);
  // glfwWindowHint(GLFW_BLUE_BITS, 8);
  // glfwWindowHint(GLFW_ALPHA_BITS, 8);
  // glfwWindowHint(GLFW_DEPTH_BITS, 32);
  // glfwWindowHint(GLFW_STENCIL_BITS, GLFW_DONT_CARE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  // glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  // glfwWindowHint(GLFW_SAMPLES, 4);

  GLFWwindow *window = glfwCreateWindow(600, 400, "cu", nullptr, nullptr);
  if (!window) {
    fprintf(stderr, "Could not create window.\n");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glfwSwapInterval(1);

  printf("%s\n", argv[1]);
  if (!obj.load(argv[1])) {
    fprintf(stderr, "Error loading obj file\n");
    return 0;
  }

  if (!init()) {
    fprintf(stderr, "Could not initialize scene.\n");
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
  }

  // printf("Ambient Occlusion Baking Example.\n");

  int w, h;
  glfwGetFramebufferSize(window, &w, &h);

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    // mainLoop(window, &scene);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
      bake(argv[2]);

    cam.setViewport(w, h);
    cam.updateVectors();

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
      cam.move(CameraDirection::UP, 1.0f);
    } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
      cam.move(CameraDirection::DOWN, 1.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
      cam.move(CameraDirection::LEFT, 1.0f);
    } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
      cam.move(CameraDirection::RIGHT, 1.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
      cam.m_yaw--;
    else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
      cam.m_yaw++;

    glViewport(0, 0, w, h);

    // draw to screen with a blueish sky
    glClearColor(0.6f, 0.8f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 6.0f));

    glUseProgram(scene.program);
    glUniform1i(scene.u_lightmap, 0);
    glUniformMatrix4fv(scene.u_projection, 1, GL_FALSE,
                       glm::value_ptr(cam.getProjectionMatrix()));
    glUniformMatrix4fv(scene.u_view, 1, GL_FALSE,
                       glm::value_ptr(cam.getViewMatrix()));
    glUniformMatrix4fv(scene.u_model, 1, GL_FALSE, glm::value_ptr(model));

    glBindTexture(GL_TEXTURE_2D, scene.lightmap);

    for (auto m : obj.meshes) {
      glBindVertexArray(m.vao);
      glDrawElements(GL_TRIANGLES, m.indices.size(), GL_UNSIGNED_SHORT, 0);
    }

    glfwSwapBuffers(window);
  }

  glDeleteTextures(1, &scene.lightmap);
  glDeleteProgram(scene.program);

  glfwDestroyWindow(window);
  glfwTerminate();
  return 1;
}

static void drawScene(float *view, float *projection) {
  glEnable(GL_DEPTH_TEST);

  glm::mat4 model(1.0f);

  glUseProgram(scene.program);
  glUniform1i(scene.u_lightmap, 0);
  glUniformMatrix4fv(scene.u_projection, 1, GL_FALSE, projection);
  glUniformMatrix4fv(scene.u_view, 1, GL_FALSE, view);
  glUniformMatrix4fv(scene.u_model, 1, GL_FALSE, glm::value_ptr(model));

  glBindTexture(GL_TEXTURE_2D, scene.lightmap);

  for (auto m : obj.meshes) {
    glBindVertexArray(m.vao);
    glDrawElements(GL_TRIANGLES, m.indices.size(), GL_UNSIGNED_SHORT, 0);
  }
}

static int init() {
  // create lightmap texture
  scene.w = 64;
  scene.h = 64;
  glGenTextures(1, &scene.lightmap);
  glBindTexture(GL_TEXTURE_2D, scene.lightmap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  unsigned char emissive[] = {0, 0, 0, 255};
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE,
               emissive);

  // load shader
  const char *vp =
      "#version 150 core\n"
      "in vec3 a_position;\n"
      "in vec2 a_texcoord;\n"
      "uniform mat4 u_view;\n"
      "uniform mat4 u_projection;\n"
      "uniform mat4 u_model;\n"
      "out vec2 v_texcoord;\n"

      "void main()\n"
      "{\n"
      "gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);\n"
      "v_texcoord = a_texcoord;\n"
      "}\n";

  const char *fp = "#version 150 core\n"
                   "in vec2 v_texcoord;\n"
                   "uniform sampler2D u_lightmap;\n"
                   "out vec4 o_color;\n"

                   "void main()\n"
                   "{\n"
                   "o_color = vec4(texture(u_lightmap, v_texcoord).rgb, "
                   "gl_FrontFacing ? 1.0 : 0.0);\n"
                   "}\n";

  const char *attribs[] = {"a_position", "a_texcoord"};

  scene.program = loadProgram(vp, fp, attribs, 2);
  if (!scene.program) {
    fprintf(stderr, "Error loading shader\n");
    return 0;
  }
  scene.u_model = glGetUniformLocation(scene.program, "u_model");
  scene.u_view = glGetUniformLocation(scene.program, "u_view");
  scene.u_projection = glGetUniformLocation(scene.program, "u_projection");
  scene.u_lightmap = glGetUniformLocation(scene.program, "u_lightmap");

  return 1;
}

static GLuint loadShader(GLenum type, const char *source) {
  GLuint shader = glCreateShader(type);
  if (shader == 0) {
    fprintf(stderr, "Could not create shader!\n");
    return 0;
  }
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);
  GLint compiled;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    fprintf(stderr, "Could not compile shader!\n");
    GLint infoLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
    if (infoLen) {
      char *infoLog = (char *)malloc(infoLen);
      glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
      fprintf(stderr, "%s\n", infoLog);
      free(infoLog);
    }
    glDeleteShader(shader);
    return 0;
  }
  return shader;
}

static GLuint loadProgram(const char *vp, const char *fp,
                          const char **attributes, int attributeCount) {
  GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vp);
  if (!vertexShader)
    return 0;
  GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fp);
  if (!fragmentShader) {
    glDeleteShader(vertexShader);
    return 0;
  }

  GLuint program = glCreateProgram();
  if (program == 0) {
    fprintf(stderr, "Could not create program!\n");
    return 0;
  }
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);

  for (int i = 0; i < attributeCount; i++)
    glBindAttribLocation(program, i, attributes[i]);

  glLinkProgram(program);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  GLint linked;
  glGetProgramiv(program, GL_LINK_STATUS, &linked);
  if (!linked) {
    fprintf(stderr, "Could not link program!\n");
    GLint infoLen = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
    if (infoLen) {
      char *infoLog = (char *)malloc(sizeof(char) * infoLen);
      glGetProgramInfoLog(program, infoLen, NULL, infoLog);
      fprintf(stderr, "%s\n", infoLog);
      free(infoLog);
    }
    glDeleteProgram(program);
    return 0;
  }
  return program;
}

LM::ObjectMesh::ObjectMesh(std::vector<Vertex> v, std::vector<u16> i) {
  vertices = v;
  indices = i;

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               vertices.data(), GL_STATIC_DRAW);

  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u16),
               indices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, pos));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, tex_coords));
}

LM::ObjectMesh::~ObjectMesh() {
  // glDeleteVertexArrays(1, &vao);
  // glDeleteBuffers(1, &vbo);
  // glDeleteBuffers(1, &ibo);
}

LM::Object::Object() {}

LM::Object::~Object() {}

int LM::Object::load(const char *path) {
  u32 flags = aiProcess_Triangulate | aiProcess_CalcTangentSpace |
              aiProcess_GenNormals;
              // | aiProcess_OptimizeGraph | aiProcess_OptimizeMeshes;

  Assimp::Importer imp;
  printf("Processing model for lightmap: %s", path);

  const aiScene *scene = imp.ReadFile(path, flags);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    printf("Assimp failed: %s", imp.GetErrorString());
    return 0;
  }

  processNode(scene->mRootNode, scene);
  return 1;
}

void LM::Object::processNode(aiNode *node, const aiScene *scene) {
  u32 i;
  for (i = 0; i < node->mNumMeshes; i++) {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    LM::ObjectMesh m = processMesh(mesh, scene);
    meshes.push_back(m);
  }

  for (i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }
}

LM::ObjectMesh LM::Object::processMesh(aiMesh *mesh, const aiScene *scene) {
  u32 i;
  std::vector<Vertex> vertices;
  std::vector<u16> indices;
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

  return LM::ObjectMesh(vertices, indices);
}
