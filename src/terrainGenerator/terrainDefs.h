#pragma once

#include "falloffMapGenerator.h"
#include "glm/glm.hpp"
#include "noiseMapGenerator.h"
#include <string>

constexpr auto kPatchSize = 64.0f;

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
  float heightMultiplier = 103.0f;

  int pixelsPerTriangle = 10; // How many pixels for triangle in patch edge for dynamic LOD

  int terrainCount;

  bool useFalloffMap = true;
};

inline TerrainData initDefaultTerrainData() {
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
  terrainData.terrainProperties.colors.push_back(glm::vec3(0.02f, 0.33f, 0.49f));
  terrainData.terrainProperties.colorStrengths.push_back(0.495f);
  terrainData.terrainProperties.heights.push_back(0.0f);
  terrainData.terrainProperties.blends.push_back(0.015f);
  terrainData.terrainProperties.textureScalings.push_back(60.825f);

  terrainData.terrainProperties.names.push_back("Sand");
  terrainData.terrainProperties.colors.push_back(glm::vec3(0.9f, 0.9f, 0.059f));
  terrainData.terrainProperties.heights.push_back(0.005f);
  terrainData.terrainProperties.colorStrengths.push_back(0.320f);
  terrainData.terrainProperties.blends.push_back(0.015f);
  terrainData.terrainProperties.textureScalings.push_back(61.649f);

  terrainData.terrainProperties.names.push_back("Grass");
  terrainData.terrainProperties.colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
  terrainData.terrainProperties.heights.push_back(0.021f);
  terrainData.terrainProperties.colorStrengths.push_back(0.160f);
  terrainData.terrainProperties.blends.push_back(0.041f);
  terrainData.terrainProperties.textureScalings.push_back(56.289f);

  terrainData.terrainProperties.names.push_back("Rock");
  terrainData.terrainProperties.colors.push_back(glm::vec3(0.13f, 0.105f, 0.105f));
  terrainData.terrainProperties.heights.push_back(0.139f);
  terrainData.terrainProperties.colorStrengths.push_back(0.381f);
  terrainData.terrainProperties.blends.push_back(0.149f);
  terrainData.terrainProperties.textureScalings.push_back(25.0f);

  terrainData.terrainProperties.names.push_back("Mountain");
  terrainData.terrainProperties.colors.push_back(glm::vec3(0.113f, 0.105f, 0.098f));
  terrainData.terrainProperties.heights.push_back(0.325f);
  terrainData.terrainProperties.colorStrengths.push_back(0.572f);
  terrainData.terrainProperties.blends.push_back(0.191f);
  terrainData.terrainProperties.textureScalings.push_back(41.0f);

  terrainData.terrainProperties.names.push_back("Snow");
  terrainData.terrainProperties.colors.push_back(glm::vec3(0.66f, 0.66f, 0.66f));
  terrainData.terrainProperties.heights.push_back(0.881f);
  terrainData.terrainProperties.colorStrengths.push_back(0.361f);
  terrainData.terrainProperties.blends.push_back(0.088f);
  terrainData.terrainProperties.textureScalings.push_back(87.0f);

  terrainData.terrainCount = int(terrainData.terrainProperties.colors.size());

  assert(terrainData.terrainProperties.names.size() == terrainData.terrainCount);
  assert(terrainData.terrainProperties.colors.size() == terrainData.terrainCount);
  assert(terrainData.terrainProperties.colorStrengths.size() == terrainData.terrainCount);
  assert(terrainData.terrainProperties.heights.size() == terrainData.terrainCount);
  assert(terrainData.terrainProperties.blends.size() == terrainData.terrainCount);
  assert(terrainData.terrainProperties.textureScalings.size() == terrainData.terrainCount);

  return terrainData;
}