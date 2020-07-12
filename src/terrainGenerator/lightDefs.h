#pragma once

#include "glm/glm.hpp"

struct LightData {
  glm::vec4 worldLightPosition = glm::vec4(64.0f, 245.0f, 30.0f, 1.0f);
  const glm::vec3 ambientConstant = glm::vec3(0.3f, 0.3f, 0.3f);
};