#include "falloffMapGenerator.h"

#include <array>
#include <memory>

FalloffMap generateFalloffMap(const int mapSize) {
  FalloffMap falloffMap;
  falloffMap.reserve(size_t(mapSize) * size_t(mapSize));

  float b = 2.2f;

  for (size_t i = 0; i < mapSize; ++i) {
    for (size_t j = 0; j < mapSize; ++j) {
      const auto x = i / float(mapSize) * 2.0f - 1.0f;
      const auto y = j / float(mapSize) * 2.0f - 1.0f;
      const auto maxValue = std::max(std::fabs(x), std::fabs(y));

      const auto maxValuePow3 = maxValue * maxValue * maxValue;
      const auto temp = b - b * maxValue;
      const auto tempPow3 = temp * temp * temp;

      falloffMap.push_back(glm::vec3(maxValuePow3 / (maxValuePow3 + tempPow3)));
    }
  }

  return falloffMap;
}