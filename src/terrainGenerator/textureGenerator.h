#pragma once

#include <vector>

#include "glm/glm.hpp"

#include "noiseMapGenerator.h"

struct TerrainType;

std::vector<glm::vec3> generateNoiseMapTexture(const NoiseMap &noiseMap);

std::vector<glm::vec3> generateColorMapTexture(const NoiseMap &noiseMap,
                                               const std::vector<TerrainType> &terrainTypes);