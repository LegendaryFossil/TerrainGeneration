#pragma once

#include <vector>

#include "GL/glew.h"
#include "glm/glm.hpp"

#include "noiseMapGenerator.h"

struct Vertex {
  glm::vec4 position;
  glm::vec3 normal;
  glm::vec2 textureCoordinate;
};

struct Mesh {
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;

  glm::mat4 modelTransformation;

  GLuint vaoHandle; // Vertex array object
  GLuint vboHandle; // Vertex buffer object
  GLuint iboHandle; // Index buffer object
  GLuint textureHandle;
};

Mesh generateMeshFromHeightMap(const NoiseMap &noiseMap);
Mesh generateQuadMesh();
