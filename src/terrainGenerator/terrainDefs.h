#pragma once

#include <string>

#include "glm/glm.hpp"

struct TerrainType {
  std::string name;
  glm::vec3 color;
  float height;
};