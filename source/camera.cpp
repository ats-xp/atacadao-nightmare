#include "camera.hpp"

Camera::Camera(glm::vec3 pos) : m_pos(pos) {
  m_front = {0.0f, 0.0f, -1.0f};
  m_world_up = {0.0f, 1.0f, 0.0f};

  m_last_mouse = {0.0f, 0.0f};

  m_fov = 45.0f;
  m_near = 0.1f;
  m_far = 100.0f;

  m_yaw = -90.0f;
  m_pitch = 0.0f;

  m_sensivity = 0.06f;

  updateVectors();
}

Camera::Camera(Camera &other) {
  m_pos = other.m_pos;
  m_front = other.m_front;
  m_up = other.m_up;
  m_world_up = other.m_world_up;

  m_win_w = other.m_win_w;
  m_win_h = other.m_win_h;

  m_fov = other.m_fov;
  m_near = other.m_near;
  m_far = other.m_far;

  m_yaw = other.m_yaw;
  m_pitch = other.m_pitch;

  m_sensivity = other.m_sensivity;
}

Camera &Camera::operator=(Camera &other) {
  m_pos = other.m_pos;
  m_front = other.m_front;
  m_up = other.m_up;
  m_world_up = other.m_world_up;

  m_win_w = other.m_win_w;
  m_win_h = other.m_win_h;

  m_fov = other.m_fov;
  m_near = other.m_near;
  m_far = other.m_far;

  m_yaw = other.m_yaw;
  m_pitch = other.m_pitch;

  m_sensivity = other.m_sensivity;
  return *this;
}

Camera::Camera(const Camera &&other) {
  m_pos = other.m_pos;
  m_front = other.m_front;
  m_up = other.m_up;
  m_world_up = other.m_world_up;

  m_win_w = other.m_win_w;
  m_win_h = other.m_win_h;

  m_fov = other.m_fov;
  m_near = other.m_near;
  m_far = other.m_far;

  m_yaw = other.m_yaw;
  m_pitch = other.m_pitch;

  m_sensivity = other.m_sensivity;
}

Camera &Camera::operator=(const Camera &&other) {
  m_pos = other.m_pos;
  m_front = other.m_front;
  m_up = other.m_up;
  m_world_up = other.m_world_up;

  m_win_w = other.m_win_w;
  m_win_h = other.m_win_h;

  m_fov = other.m_fov;
  m_near = other.m_near;
  m_far = other.m_far;

  m_yaw = other.m_yaw;
  m_pitch = other.m_pitch;

  m_sensivity = other.m_sensivity;
  return *this;
}

void Camera::updateVectors() {
  m_front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
  m_front.y = sin(glm::radians(m_pitch));
  m_front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
  m_front = glm::normalize(m_front);

  m_right = glm::normalize(glm::cross(m_front, m_world_up));
  m_up = glm::normalize(glm::cross(m_right, m_front));
}

void Camera::updateMouse(f32 x, f32 y) {
  f32 offset_x = x - m_last_mouse.x;
  f32 offset_y = m_last_mouse.y - y;
  m_last_mouse.x = x;
  m_last_mouse.y = y;

  offset_x *= m_sensivity;
  offset_y *= m_sensivity;

  m_yaw += offset_x;
  m_pitch += offset_y;

  if (m_pitch > 89.0f)
    m_pitch = 89.0f;
  if (m_pitch < -89.0f)
    m_pitch = -89.0f;

  updateVectors();
}

void Camera::move(CameraDirection d, f32 speed) {
  if (d == UP) {
    m_pos += m_front * speed;
  } else if (d == DOWN) {
    m_pos -= m_front * speed;
  }

  if (d == LEFT) {
    m_pos -= m_right * speed;
  } else if (d == RIGHT) {
    m_pos += m_right * speed;
  }
}
