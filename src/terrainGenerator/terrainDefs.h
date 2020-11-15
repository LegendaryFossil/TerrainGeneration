#pragma once

#include "falloffMapGenerator.h"
#include "glm/glm.hpp"
#include "noiseMapGenerator.h"
#include <string>

constexpr auto kPatchSize = 64.0f;
constexpr auto kTerrainCount = 5;

// Terrain
struct TerrainData {
  NoiseMapData noiseMapData = {}; // Noise map settings to alter terrain

  struct {
    std::vector<std::string> names;
    std::vector<glm::vec3> colors;
    std::vector<float> colorStrengths;
    std::vector<float> heights;
    std::vector<float> blends;
    std::vector<float> textureScalings;
  } terrainProperties;

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

  terrainData.terrainProperties.names.push_back("Water");
  terrainData.terrainProperties.colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
  terrainData.terrainProperties.colorStrengths.push_back(0.5f);
  terrainData.terrainProperties.heights.push_back(0.0f);
  terrainData.terrainProperties.blends.push_back(0.0f);
  terrainData.terrainProperties.textureScalings.push_back(50.0f);

  terrainData.terrainProperties.names.push_back("Sand");
  terrainData.terrainProperties.colors.push_back(glm::vec3(1.0f, 1.0f, 0.45f));
  terrainData.terrainProperties.heights.push_back(0.005f);
  terrainData.terrainProperties.colorStrengths.push_back(0.5f);
  terrainData.terrainProperties.blends.push_back(0.0f);
  terrainData.terrainProperties.textureScalings.push_back(50.0f);

  terrainData.terrainProperties.names.push_back("Grass");
  terrainData.terrainProperties.colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
  terrainData.terrainProperties.heights.push_back(0.021f);
  terrainData.terrainProperties.colorStrengths.push_back(0.5f);
  terrainData.terrainProperties.blends.push_back(0.0f);
  terrainData.terrainProperties.textureScalings.push_back(50.0f);

  terrainData.terrainProperties.names.push_back("Mountain");
  terrainData.terrainProperties.colors.push_back(glm::vec3(0.43f, 0.227f, 0.03f));
  terrainData.terrainProperties.heights.push_back(0.289f);
  terrainData.terrainProperties.colorStrengths.push_back(0.5f);
  terrainData.terrainProperties.blends.push_back(0.0f);
  terrainData.terrainProperties.textureScalings.push_back(50.0f);

  terrainData.terrainProperties.names.push_back("Snow");
  terrainData.terrainProperties.colors.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
  terrainData.terrainProperties.heights.push_back(0.763f);
  terrainData.terrainProperties.colorStrengths.push_back(0.5f);
  terrainData.terrainProperties.blends.push_back(0.0f);
  terrainData.terrainProperties.textureScalings.push_back(50.0f);

  assert(terrainData.terrainProperties.names.size() == kTerrainCount);
  assert(terrainData.terrainProperties.colors.size() == kTerrainCount);
  assert(terrainData.terrainProperties.colorStrengths.size() == kTerrainCount);
  assert(terrainData.terrainProperties.heights.size() == kTerrainCount);
  assert(terrainData.terrainProperties.blends.size() == kTerrainCount);
  assert(terrainData.terrainProperties.textureScalings.size() == kTerrainCount);

  return terrainData;
}