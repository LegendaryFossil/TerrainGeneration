#pragma once

#include "falloffMapGenerator.h"
#include "glm/glm.hpp"
#include "noiseMapGenerator.h"
#include <string>

constexpr auto PATCH_SIZE = 64.0f;

constexpr auto kWaterIndex = 0;
constexpr auto kShallowWaterIndex = 1;
constexpr auto kSandIndex = 2;
constexpr auto kGrassIndex = 3;
// constexpr auto kRockIndex = 4;
constexpr auto kMountainIndex = 4;
constexpr auto kSnowIndex = 5;

struct TerrainProperty {
  std::string name;
  glm::vec3 color;
  float height;
};

// Terrain
struct TerrainData {
  NoiseMapData noiseMapData = {}; // Noise map settings to alter terrain
  std::vector<TerrainProperty> terrainProperties;
  float gridPointSpacing = 1.0f; // Terrain scaling
  float heightMultiplier = 65.0f;

  float waterDistortionMoveFactor = 0.0f;
  float waterDistortionSpeed = 0.05f;

  int pixelsPerTriangle = 20; // How many pixels for triangle in patch edge for dynamic LOD

  bool useFalloffMap = true;
};

inline TerrainData getDefaultTerrainData() {
  const int mapSize = 512;
  // Only allow power of two
  assert(mapSize && !(mapSize & (mapSize - 1)));

  TerrainData terrainData = {};

  terrainData.noiseMapData.width = terrainData.noiseMapData.height = mapSize;
  terrainData.noiseMapData.scale = 1.0f;
  terrainData.noiseMapData.octaves = 4;
  terrainData.noiseMapData.persistance = 0.366f;
  terrainData.noiseMapData.lacunarity = 2.0f;
  terrainData.noiseMapData.seed = 1;
  terrainData.noiseMapData.octaveOffset = glm::vec2(0.0f, 444.0f);

  terrainData.terrainProperties.reserve(3);
  terrainData.terrainProperties.push_back({"Water", glm::vec3(0.0f, 0.0f, 1.0f), 0.0f});
  terrainData.terrainProperties.push_back({"Shallow water", glm::vec3(0.42f, 0.61f, 0.94f), 0.020f});
  terrainData.terrainProperties.push_back({"Sand", glm::vec3(1.0f, 1.0f, 0.45f), 0.067f});
  terrainData.terrainProperties.push_back({"Grass", glm::vec3(0.0f, 1.0f, 0.0f), 0.392f});
  terrainData.terrainProperties.push_back({"Mountain", glm::vec3(0.43f, 0.227f, 0.03f), 0.686f});
  terrainData.terrainProperties.push_back({"Snow", glm::vec3(1.0f, 1.0f, 1.0f), 1.0f});

  return terrainData;
}