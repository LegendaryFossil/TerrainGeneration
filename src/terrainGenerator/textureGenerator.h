#pragma once

#include <vector>

#include "glm/glm.hpp"

#include "noiseMapGenerator.h"

#include "GL/glew.h"

struct TerrainType;

void createTexture2D(GLuint textureHandle, GLenum wrapMode, GLenum filterMode, const int width, const int height,
                     const void *pixels);
void updateTexture2D(GLuint textureHandle, const int offsetX, const int offsetY, const int width, const int height,
                     const void *pixelData);

std::vector<glm::vec3> generateNoiseMapTexture(const NoiseMap &noiseMap);

std::vector<glm::vec3> generateColorMapTexture(const NoiseMap &noiseMap, const std::vector<TerrainType> &terrainTypes);