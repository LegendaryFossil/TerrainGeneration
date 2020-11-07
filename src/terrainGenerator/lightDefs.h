#pragma once

#include "glm/glm.hpp"

struct LightData {
  glm::vec4 worldLightPosition = glm::vec4(64.0f, 577.0f, 360.0f, 1.0f);
  const glm::vec3 ambientConstant = glm::vec3(0.3f, 0.3f, 0.3f);

  struct{
    glm::vec3 reflection = glm::vec3(0.7f, 0.7f, 0.7f);
    float intensity = 0.3f;
    float shineDamper = 20.0f;
  } specularData;
};