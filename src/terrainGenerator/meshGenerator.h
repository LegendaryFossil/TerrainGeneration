#pragma once

#include <vector>

#include "GL/glew.h"
#include "glm/glm.hpp"

#include "noiseMapGenerator.h"

constexpr auto MAX_TEXTURES = 10;

struct Vertex {
  union {
    glm::vec2 position2f;
    glm::vec3 position3f;
    glm::vec4 position4f;
  };
  
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
  GLuint textureHandles[MAX_TEXTURES];
};

Mesh generateMeshFromHeightMap(const NoiseMap &noiseMap);
Mesh generateQuadMesh();
