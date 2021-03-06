#include "noiseMapGenerator.h"

#include "FastNoise/FastNoise.h"
#include "falloffMapGenerator.h"
#include <array>
#include <memory>
#include <numeric>
#include <random>

/*
NoiseMap generateNoiseMapSIMD(const NoiseMapData &noiseMapData) {
  std::unique_ptr<FastNoiseSIMD> myNoise(FastNoiseSIMD::NewFastNoiseSIMD(noiseMapData.seed));

  myNoise.get()->SetFractalOctaves(noiseMapData.octaves);
  myNoise.get()->SetFractalLacunarity(noiseMapData.lacunarity);
  myNoise.get()->SetFractalGain(noiseMapData.persistance);

  const auto perlinFractal =
      myNoise.get()->GetPerlinFractalSet(0, 0, 0, noiseMapData.width, noiseMapData.height, 1, noiseMapData.scale);

  std::vector<std::vector<float>> noiseMap;
  noiseMap.reserve(noiseMapData.height);

  float maxNoiseHeight = std::numeric_limits<float>::min();
  float minNoiseHeight = std::numeric_limits<float>::max();

  int index = 0;
  for (int i = 0; i < noiseMapData.height; ++i) {
    std::vector<float> noiseValues;
    noiseValues.reserve(noiseMapData.width);
    for (int j = 0; j < noiseMapData.width; ++j) {
      const auto noiseHeight = perlinFractal[index++];

      if (noiseHeight > maxNoiseHeight)
        maxNoiseHeight = noiseHeight;
      else if (noiseHeight < minNoiseHeight)
        minNoiseHeight = noiseHeight;

      noiseValues.push_back(noiseHeight);
    }
    noiseMap.push_back(noiseValues);
  }

  FastNoiseSIMD::FreeNoiseSet(perlinFractal);

  const auto noiseHeightDiffInverse = 1.0f / (maxNoiseHeight - minNoiseHeight);
  for (size_t i = 0; i < noiseMapData.height; ++i) {
    for (size_t j = 0; j < noiseMapData.width; ++j) {
      noiseMap[i][j] = (noiseMap[i][j] - minNoiseHeight) * noiseHeightDiffInverse;
    }
  }

  return noiseMap;
}*/

static float getCurveValue(const float value) {
  return ((-0.635179f * value * value * value * value) + (2.35243f * value * value) + (-0.72331f * value) + -0.000937f);
}

NoiseMap generateNoiseMap(const NoiseMapData &noiseMapData, const bool useFalloffMap) {
  assert(noiseMapData.width == noiseMapData.height);

  FastNoise fastNoise(noiseMapData.seed);
  fastNoise.SetNoiseType(FastNoise::Perlin);

  std::vector<std::vector<float>> noiseMap;
  noiseMap.reserve(noiseMapData.height);

  const auto halfMapWidth = noiseMapData.width / 2;
  const auto halfMapHeight = halfMapWidth;

  float maxNoiseHeight = std::numeric_limits<float>::min();
  float minNoiseHeight = std::numeric_limits<float>::max();

  for (int i = 0; i < noiseMapData.height; ++i) {
    std::vector<float> noiseValues;
    noiseValues.reserve(noiseMapData.width);
    for (int j = 0; j < noiseMapData.width; ++j) {
      float amplitude = 1.0f;
      float frequency = 1.0f;
      float noiseHeight = 0.0f;

      for (int octave = 0; octave < noiseMapData.octaves; ++octave) {
        // Minus half map dimensions to scale in to the center instead of corner
        const auto sampleX = (j - halfMapWidth + noiseMapData.octaveOffset.x) / noiseMapData.scale * frequency;
        const auto sampleY = (i - halfMapHeight - noiseMapData.octaveOffset.y) / noiseMapData.scale * frequency;

        const auto perlinNoiseValue = fastNoise.GetNoise(sampleX, sampleY);
        noiseHeight += perlinNoiseValue * amplitude;

        amplitude *= noiseMapData.persistance;
        frequency *= noiseMapData.lacunarity;
      }

      if (noiseHeight > maxNoiseHeight)
        maxNoiseHeight = noiseHeight;
      else if (noiseHeight < minNoiseHeight)
        minNoiseHeight = noiseHeight;

      noiseValues.push_back(noiseHeight);
    }
    noiseMap.push_back(noiseValues);
  }

  FalloffMap falloffMap;
  if (useFalloffMap) {
    falloffMap = generateFalloffMap(noiseMapData.width);
  }

  const auto noiseHeightDiffInverse = 1.0f / (maxNoiseHeight - minNoiseHeight);
  for (size_t i = 0; i < noiseMapData.height; ++i) {
    for (size_t j = 0; j < noiseMapData.width; ++j) {
      noiseMap[i][j] = (noiseMap[i][j] - minNoiseHeight) * noiseHeightDiffInverse;

      if (useFalloffMap) {
        noiseMap[i][j] = glm::clamp(noiseMap[i][j] - falloffMap[noiseMapData.width * j + i].x, 0.0f, 1.0f);
      }

      noiseMap[i][j] = glm::clamp(getCurveValue(noiseMap[i][j]), 0.0f, 1.0f);
    }
  }

  return noiseMap;
}