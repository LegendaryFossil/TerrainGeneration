#include "noiseMapGenerator.h"

#include <array>
#include <numeric>
#include <random>

#include "PerlinNoise.h"
#include "glm/glm.hpp"

NoiseMap generateNoiseMap(const NoiseMapData &noiseMapData) {
  assert(noiseMapData.width == noiseMapData.height);

  std::mt19937 rng(noiseMapData.seed);
  std::uniform_int_distribution<int> rngDist(-100000, 100000);
  std::vector<glm::vec2> octaveOffsets;
  octaveOffsets.reserve(noiseMapData.octaves);
  for (size_t i = 0, size = noiseMapData.octaves; i < size; ++i) {
    octaveOffsets.emplace_back(glm::vec2(rngDist(rng) + noiseMapData.octaveOffset.x,
                                         rngDist(rng) + noiseMapData.octaveOffset.y));
  }

  PerlinNoise perlinNoise;

  float maxNoiseHeight = std::numeric_limits<float>::min();
  float minNoiseHeight = std::numeric_limits<float>::max();

  std::vector<std::vector<float>> noiseMap;
  noiseMap.reserve(noiseMapData.height);

  const auto halfMapWidth = noiseMapData.width / 2;
  const auto halfMapHeight = halfMapWidth;

  for (int i = 0; i < noiseMapData.height; ++i) {
    std::vector<float> noiseValues;
    noiseValues.reserve(noiseMapData.width);
    for (int j = 0; j < noiseMapData.width; ++j) {
      float amplitude = 1.0f;
      float frequency = 1.0f;
      float noiseHeight = 0.0f;

      for (int octave = 0; octave < noiseMapData.octaves; ++octave) {
        // minus half map dimensions to scale in to the center instead of corner
        const auto sampleX =
            (j - halfMapWidth) / noiseMapData.scale * frequency + octaveOffsets[octave].x;
        const auto sampleY =
            (i - halfMapHeight) / noiseMapData.scale * frequency + octaveOffsets[octave].y;

        // Multiplying by two and subtracting one to reinterval it from [0,1] to [-1,1]
        const float perlinNoiseValue =
            (float(perlinNoise.noise(sampleX, sampleY, 0.0f)) * 2.0f) - 1.0f;

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

  const auto noiseHeightDiffInverse = 1.0f / (maxNoiseHeight - minNoiseHeight);
  for (size_t i = 0; i < noiseMapData.height; ++i) {
    for (size_t j = 0; j < noiseMapData.width; ++j) {
      noiseMap[i][j] = (noiseMap[i][j] - minNoiseHeight) * noiseHeightDiffInverse;
    }
  }

  return noiseMap;
}