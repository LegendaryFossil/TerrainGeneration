#pragma once
#include <vector>

#include "glm/glm.hpp"

struct NoiseMapData {
  int width, height;
  float scale;
  int octaves;
  float persistance;
  float lacunarity;
  int seed;
  glm::vec2 octaveOffset;
};

using NoiseMap = std::vector<std::vector<float>>;

NoiseMap generateNoiseMap(const NoiseMapData &noiseMapData);