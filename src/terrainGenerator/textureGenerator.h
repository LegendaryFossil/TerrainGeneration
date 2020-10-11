#pragma once

#include <vector>
#include <string>

#include "glm/glm.hpp"

#include "GL/glew.h"
#include "noiseMapGenerator.h"

struct TerrainData;
struct TerrainType;

void loadTexture(const std::string &textureName, int *width, int *height, unsigned char **pixelData);

void createTexture2D(GLuint *texHandle, GLenum wrapMode, GLenum filterMode, const int width,
                     const int height, GLenum dataType,
                     const void *pixels);
void updateTexture2D(GLuint *texHandle, const int offsetX, const int offsetY, const int width,
                     const int height, GLenum dataType,
                     const void *pixels);

std::vector<glm::vec3> generateNoiseMapTexture(const NoiseMap &noiseMap);

std::vector<glm::vec3> generateColorMapTexture(const NoiseMap &noiseMap, const std::vector<TerrainType> &terrainTypes);