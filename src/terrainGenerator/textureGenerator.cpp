#include "textureGenerator.h"

#include "terrainDefs.h"

#include "GL/glew.h"

void createTexture2D(GLuint textureHandle, GLenum wrapMode, GLenum filterMode, const int width, const int height,
                     const void *pixelData) {
  glBindTexture(GL_TEXTURE_2D, textureHandle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, pixelData);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void updateTexture2D(GLuint textureHandle, const int offsetX, const int offsetY, const int width, const int height,
                     const void *pixelData) {
  glBindTexture(GL_TEXTURE_2D, textureHandle);
  glTexSubImage2D(GL_TEXTURE_2D, 0, offsetX, offsetY, width, height, GL_RGB, GL_FLOAT, pixelData);
}

std::vector<glm::vec3> generateNoiseMapTexture(const NoiseMap &noiseMap) {
  const auto black = glm::vec3(0.0f);
  const auto white = glm::vec3(1.0f);

  const auto mapHeight = noiseMap.size();
  const auto mapWidth = noiseMap.front().size();

  std::vector<glm::vec3> noiseMapTextureData;
  noiseMapTextureData.reserve(size_t(mapWidth) * size_t(mapHeight));
  for (size_t i = 0; i < mapHeight; ++i) {
    for (size_t j = 0; j < mapWidth; ++j) {
      noiseMapTextureData.push_back(glm::mix(black, white, noiseMap[i][j]));
    }
  }

  return noiseMapTextureData;
}

std::vector<glm::vec3> generateColorMapTexture(const NoiseMap &noiseMap, const std::vector<TerrainType> &terrainTypes) {
  const auto mapHeight = noiseMap.size();
  const auto mapWidth = noiseMap.front().size();

  std::vector<glm::vec3> colorMapTextureData;
  colorMapTextureData.reserve(mapHeight * mapWidth);
  for (size_t i = 0, i_size = mapHeight; i < i_size; ++i) {
    for (size_t j = 0, j_size = mapWidth; j < j_size; ++j) {
      const float height = noiseMap[i][j];
      colorMapTextureData.push_back(glm::vec3(0.0f));
      for (const auto &terrainType : terrainTypes) {
        if (height <= terrainType.height) {
          colorMapTextureData.back() = terrainType.color;
          break;
        }
      }
    }
  }

  return colorMapTextureData;
}