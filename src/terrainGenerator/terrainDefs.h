#pragma once

#include <array>

#include "glm/glm.hpp"

const int TERRAIN_NAME_SIZE = 50;

struct TerrainType {
  std::array<char, TERRAIN_NAME_SIZE> name = {};
  float height;
  glm::vec3 color;
};