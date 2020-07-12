#pragma once

#include <vector>

#include "glm/glm.hpp"

#include "GL/glew.h"

struct TerrainType;
using NoiseMap = std::vector<std::vector<float>>;

void createTexture2D(GLuint *texHandle, GLenum wrapMode, GLenum filterMode, const int width, const int height,
                     const void *pixels);
void updateTexture2D(GLuint *texHandle, const int offsetX, const int offsetY, const int width, const int height,
                     const void *pixelData);

std::vector<glm::vec3> generateNoiseMapTexture(const NoiseMap &noiseMap);

std::vector<glm::vec3> generateColorMapTexture(const NoiseMap &noiseMap, const std::vector<TerrainType> &terrainTypes);