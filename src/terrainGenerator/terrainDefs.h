#pragma once

#include <string>

#include "glm/glm.hpp"
#include "noiseMapGenerator.h"
#include "falloffMapGenerator.h"

constexpr auto PATCH_SIZE = 64.0f;

struct TerrainType {
  std::string name;
  glm::vec3 color;
  float height;
};

// Terrain
struct TerrainData {
  NoiseMapData noiseMapData = {}; // Noise map settings to alter terrain
  NoiseMap noiseMap = {};
  FalloffMap falloffMap = {};
  std::vector<TerrainType> terrainTypes;
  float gridPointSpacing = 1.0f; // Terrain scaling
  float heightMultiplier = 65.0f;
  int pixelsPerTriangle = 20; // How many pixels for triangle in patch edge for dynamic LOD
};