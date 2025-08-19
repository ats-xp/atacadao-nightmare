#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "base.h"

enum CameraDirection : u8 {
  UP = 0,
  DOWN,
  LEFT,
  RIGHT,
};

struct Camera {
  glm::vec3 m_pos;
  glm::vec3 m_front;
  glm::vec3 m_up;
  glm::vec3 m_world_up;
  glm::vec3 m_right;

  glm::vec2 m_last_mouse;

  f32 m_win_w;
  f32 m_win_h;

  f32 m_fov;
  f32 m_near;
  f32 m_far;

  f32 m_yaw;
  f32 m_pitch;

  f32 m_sensivity;

public:
  Camera(glm::vec3 pos = glm::vec3(0.0f));

  Camera(Camera &other);
  Camera &operator=(Camera &other);

  Camera(const Camera &&other);
  Camera &operator=(const Camera &&other);

  ~Camera() = default;

  void init(glm::vec3 pos = glm::vec3(0.0f));

  void updateVectors();
  void updateMouse(f32 x, f32 y);

  void move(CameraDirection d, f32 speed);

  inline void setPosition(glm::vec3 pos) { m_pos = pos; };

  inline void setViewport(int w, int h) {
    m_win_w = static_cast<f32>(w);
    m_win_h = static_cast<f32>(h);
  }

  inline void setDistance(float near, float far) {
    m_near = near;
    m_far = far;
  }

  inline glm::vec3 &getPosition() { return m_pos; };

  inline glm::mat4 getProjectionMatrix() const {
    return glm::perspectiveRH_NO(glm::radians(m_fov), m_win_w / m_win_h, m_near,
                                 m_far);
  }

  inline glm::mat4 getViewMatrix() const {
    return glm::lookAt(m_pos, m_pos + m_front, m_up);
  }

  inline glm::mat4 getMatrix() {
    updateVectors();
    return getProjectionMatrix() * getViewMatrix();
  }

  constexpr glm::vec3 getFront() const {
    return m_front;
  }

  constexpr glm::vec3 getRight() const {
    return m_right;
  }
};
