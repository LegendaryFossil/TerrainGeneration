#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "noiseMapGenerator.h"
#include "utils.h"
#include <string>
#include <vector>

struct TerrainData;
struct TerrainProperty;

const std::string texturePath(getExePath() + "/resources/textures/");
const std::string skyboxTexturePath(getExePath() + "/resources/textures/skybox/");
const std::string waterDuDvTexturePath(getExePath() + "/resources/textures/waterDuDv/");
const std::string waterNormalMapTexturePath(getExePath() + "/resources/textures/waterNormalMap/");

void loadTexture(const std::string &textureName, const std::string &texturePath, int *width, int *height,
                 unsigned char **pixelData);
void freeTexture(unsigned char *pixelData);

void createCubeMapTexture(GLuint *texHandle, const std::vector<std::string> &facesNames);

void createTexture2D(GLuint *texHandle, GLenum wrapMode, GLenum filterMode, const int width, const int height,
                     GLenum dataType, const void *pixels);
void updateTexture2D(GLuint *texHandle, const int offsetX, const int offsetY, const int width,
                     const int height, GLenum dataType, const void *pixels);

std::vector<glm::vec3> generateNoiseMapTexture(const NoiseMap &noiseMap);

std::vector<glm::vec3> generateColorMapTexture(const NoiseMap &noiseMap,
                                               const std::vector<TerrainProperty> &terrainProperties);