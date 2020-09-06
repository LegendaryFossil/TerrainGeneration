#include "textureGenerator.h"

#include "terrainDefs.h"

#include "GL/glew.h"
/*
static float sampleHeight(const int u, const int v, const int mapSize, const TerrainData &terrainData) {
  auto tempU = glm::clamp(u, 0, mapSize - 1);
  auto tempV = glm::clamp(v, 0, mapSize - 1);

  const auto noiseMapHeight = terrainData.noiseMap[tempV][tempU];
  return (noiseMapHeight * terrainData.heightMultiplier / terrainData.maxHeightMultiplier);
}

struct PatchCorners {
  glm::vec3 c0;
  glm::vec3 c1;
  glm::vec3 c2;
  glm::vec3 c3;
};

static PatchCorners getPatchCorners(const int patchIndexX, const int patchIndexZ, const TerrainData &terrainData,
                                    const NoiseMap &noiseMap) {
  const auto mapSize = terrainData.noiseMapData.width;

  glm::vec3 corner0(0.0f, sampleHeight(patchIndexX * PATCH_SIZE, patchIndexZ * PATCH_SIZE, mapSize, terrainData), 0.0f);

  glm::vec3 corner1(0.0f, sampleHeight(patchIndexX * PATCH_SIZE, (patchIndexZ + 1) * PATCH_SIZE, mapSize, terrainData),
                    terrainData.gridPointSpacing * PATCH_SIZE);

  glm::vec3 corner2(terrainData.gridPointSpacing * PATCH_SIZE,
                    sampleHeight((patchIndexX + 1) * PATCH_SIZE, patchIndexZ * PATCH_SIZE, mapSize, terrainData), 0.0f);

  glm::vec3 corner3(terrainData.gridPointSpacing * PATCH_SIZE,
                    sampleHeight((patchIndexX + 1) * PATCH_SIZE, (patchIndexZ + 1) * PATCH_SIZE, mapSize, terrainData),
                    terrainData.gridPointSpacing * PATCH_SIZE);

  return {corner0, corner1, corner2, corner3};
}

static glm::vec3 calcPatchAvgNormal(const int i, const int j, const TerrainData &terrainData) {
  const auto patchCorners = getPatchCorners(i, j, terrainData, terrainData.noiseMap);

  /*
  c1----------c3
  |            |
  |            |
  |            |
  |            |
  c0----------c2
  */
/*
  const auto c1ToC0 = patchCorners.c1 - patchCorners.c0;
  const auto c2ToC0 = patchCorners.c2 - patchCorners.c0;
  const auto c3ToC1 = patchCorners.c3 - patchCorners.c1;
  const auto c3ToC2 = patchCorners.c3 - patchCorners.c2;

  glm::vec3 n0 = glm::normalize(glm::cross(c1ToC0, c2ToC0));
  glm::vec3 n1 = glm::normalize(glm::cross(c1ToC0, c3ToC1));
  glm::vec3 n2 = glm::normalize(glm::cross(c3ToC2, c2ToC0));
  glm::vec3 n3 = glm::normalize(glm::cross(c3ToC2, c3ToC1));

  return glm::normalize(n0 + n1 + n2 + n3);
}*/

void createTexture2D(GLuint *texHandle, GLenum wrapMode, GLenum filterMode, const int width, const int height,
                     const void *pixelData) {
  glBindTexture(GL_TEXTURE_2D, *texHandle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, pixelData);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void updateTexture2D(GLuint *texHandle, const int offsetX, const int offsetY, const int width, const int height,
                     const void *pixelData) {
  glBindTexture(GL_TEXTURE_2D, *texHandle);
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
/*
std::vector<glm::vec3> generateRoughnessTexture(const TerrainData &terrainData) {
  const auto mapSize = terrainData.noiseMapData.width;

  int numOfPatchesX = mapSize / int(PATCH_SIZE);
  int numOfPatchesZ = numOfPatchesX;

  for (size_t i = 0; i < numOfPatchesZ; ++i) {
    for (size_t j = 0; j < numOfPatchesX; ++j) {
      const auto patchAverageNormal = calcPatchAvgNormal(j, i, terrainData);

      // Calculate normal at each point in patch
      float minNormalDiff = 1.0f;
      for (size_t k = 0; k < PATCH_SIZE; ++k) {
        for (size_t l = 0; l < PATCH_SIZE; ++l) {
          // Method based on 'Fast Heightfield Normal Calculation' from Game Programming Gems 3
          // See https://stackoverflow.com/questions/49640250/calculate-normals-from-heightmap
          // http://index-of.co.uk/Game-Development/Programming/Game%20Programming%20Gems%203.pdf

          const auto up = sampleHeight(j * PATCH_SIZE + k, i * PATCH_SIZE + l + 1, mapSize, terrainData);
          const auto down = sampleHeight(j * PATCH_SIZE + k, i * PATCH_SIZE + l - 1, mapSize, terrainData);
          const auto right = sampleHeight(j * PATCH_SIZE + k + 1, i * PATCH_SIZE + l, mapSize, terrainData);
          const auto left = sampleHeight(j * PATCH_SIZE + k - 1, i * PATCH_SIZE + l, mapSize, terrainData);

          const auto leftToRight = right - left;
          const auto downToUp = up - down;

          const auto normal =
              glm::normalize(glm::vec3(-2.0f * terrainData.gridPointSpacing * downToUp,
                                       4.0f * terrainData.gridPointSpacing * terrainData.gridPointSpacing,
                                       -2.0f * terrainData.gridPointSpacing * leftToRight));

          const auto diff = std::abs(glm::dot(patchAverageNormal, normal));
          if (diff < minNormalDiff) {
            minNormalDiff = diff;
          }
        }
      }
    }
  }

  return {};
}*/