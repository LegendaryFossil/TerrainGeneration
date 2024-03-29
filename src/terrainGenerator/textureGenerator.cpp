#include "textureGenerator.h"

#include "GL/glew.h"
#include "glm\gtc\type_ptr.hpp"
#include "stb_image.h"
#include "terrainDefs.h"

void loadTexture(const std::string &textureName, const std::string &texturePath, int *width, int *height,
                 unsigned char **pixelData) {
  int numOfColorChannels;
  *pixelData =
      stbi_load(std::string(texturePath + textureName).c_str(), width, height, &numOfColorChannels, 0);
}

void freeTexture(unsigned char *pixelData) { stbi_image_free(pixelData); }

void createTexture2D(GLuint *texHandle, GLenum wrapMode, GLenum filterMode, const int width, const int height,
                     GLenum dataType, const void *pixelData) {
  glBindTexture(GL_TEXTURE_2D, *texHandle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, dataType, pixelData);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void createTexture2DArray(GLuint *texHandle, GLenum wrapMode, GLenum filterMode, const int width,
                          const int height, GLenum dataType,
                          const std::vector<unsigned char *> &terrainTextures) {
  glBindTexture(GL_TEXTURE_2D_ARRAY, *texHandle);

  glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGB8, width, height, GLsizei(terrainTextures.size()));
  for (size_t i = 0; i < terrainTextures.size(); ++i) {
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, GLint(i), width, height, 1, GL_RGB, dataType, terrainTextures[i]);
  }

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, wrapMode);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, wrapMode);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, filterMode);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, filterMode);

  glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void createCubeMapTexture(GLuint *texHandle, const std::vector<std::string> &facesNames) {
  glBindTexture(GL_TEXTURE_CUBE_MAP, *texHandle);

  for (size_t i = 0; i < facesNames.size(); i++) {
    int faceWidth, faceHeight;
    unsigned char *facePixelData = nullptr;
    loadTexture(facesNames[i], skyboxTexturePath, &faceWidth, &faceHeight, &facePixelData);
    assert(facePixelData != nullptr);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + GLenum(i), 0, GL_RGB, faceWidth, faceHeight, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, facePixelData);
    freeTexture(facePixelData);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void updateTexture2D(GLuint *texHandle, const int offsetX, const int offsetY, const int width,
                     const int height, GLenum dataType, const void *pixelData) {
  glBindTexture(GL_TEXTURE_2D, *texHandle);
  glTexSubImage2D(GL_TEXTURE_2D, 0, offsetX, offsetY, width, height, GL_RGB, dataType, pixelData);
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