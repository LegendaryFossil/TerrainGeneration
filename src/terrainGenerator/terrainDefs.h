#pragma once

#include <string>

#include "falloffMapGenerator.h"
#include "glm/glm.hpp"
#include "noiseMapGenerator.h"

constexpr auto PATCH_SIZE = 64.0f;

struct TerrainType {
  std::string name;
  glm::vec3 color;
  float height;
};

// Terrain
struct TerrainData {
  NoiseMapData noiseMapData = {}; // Noise map settings to alter terrain
  std::vector<TerrainType> terrainTypes;
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

  terrainData.terrainTypes.reserve(3);
  terrainData.terrainTypes.push_back({"Water", glm::vec3(0.0f, 0.0f, 1.0f), 0.0f});
  terrainData.terrainTypes.push_back({"Shallow water", glm::vec3(0.05f, 0.4f, 1.0f), 0.036f});
  terrainData.terrainTypes.push_back({"Sand", glm::vec3(1.0f, 1.0f, 0.45f), 0.067f});
  terrainData.terrainTypes.push_back({"Land", glm::vec3(0.0f, 1.0f, 0.0f), 0.392f});
  terrainData.terrainTypes.push_back({"Mountain", glm::vec3(0.43f, 0.227f, 0.03f), 0.686f});
  terrainData.terrainTypes.push_back({"Snow", glm::vec3(1.0f, 1.0f, 1.0f), 1.0f});

  return terrainData;
}