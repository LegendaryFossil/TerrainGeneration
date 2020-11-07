#pragma once

#include "glm/glm.hpp"

struct GLFWwindow;

struct WindowData {
  GLFWwindow *window = nullptr;
  int width = 1920;
  int height = 1280;
  glm::dvec2 center = glm::vec2(width / 2.0f, height / 2.0f);
};