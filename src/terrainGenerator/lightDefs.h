#pragma once

#include "glm/glm.hpp"
#include <vector>

constexpr auto ambientConstant = glm::vec3(0.3f, 0.3f, 0.3f);

struct LightData {
  // Positions in world space
  std::vector<glm::vec4> positions;
  std::vector<glm::vec3> colors;

  struct {
    std::vector<glm::vec3> colors;
    std::vector<float> intensities;
    std::vector<float> powers;
  } specularData;

  float reflectionStrength = 0.3f;
  int lightCount;
};

inline LightData initDefaultLightData() {
  LightData lightData;

  lightData.positions.push_back(glm::vec4(396.0f, 193.0f, -11.0f, 1.0f));
  lightData.colors.push_back(glm::vec3(1.0f, 1.0f, 1.0));
  lightData.specularData.colors.push_back(glm::vec3(0.04f));
  lightData.specularData.intensities.push_back(1.0f);
  lightData.specularData.powers.push_back(1000.0f);

  lightData.positions.push_back(glm::vec4(64.0f, 577.0f, 360.0f, 1.0f));
  lightData.colors.push_back(glm::vec3(1.0f, 1.0f, 1.0));
  lightData.specularData.colors.push_back(glm::vec3(0.04f));
  lightData.specularData.intensities.push_back(1.0f);
  lightData.specularData.powers.push_back(1000.0f);

  lightData.lightCount = int(lightData.positions.size());

  assert(lightData.positions.size() == lightData.lightCount);
  assert(lightData.colors.size() == lightData.lightCount);
  assert(lightData.specularData.colors.size() == lightData.lightCount);
  assert(lightData.specularData.intensities.size() == lightData.lightCount);
  assert(lightData.specularData.powers.size() == lightData.lightCount);

  lightData.reflectionStrength = 0.3f;

  return lightData;
}