#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "noiseMapGenerator.h"
#include "terrainDefs.h"
#include <unordered_map>
#include <vector>

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

  glm::mat4 modelTransformation = glm::mat4(1.0f);

  GLuint vaoHandle; // Vertex array object
  GLuint vboHandle; // Vertex buffer object
  GLuint iboHandle; // Index buffer object
  GLuint textureHandles[MAX_TEXTURES];
};

constexpr auto kTerrainMeshId = "terrain";
constexpr auto kWaterMeshId = "water";
constexpr auto kQuadMeshId = "quad";

using MeshIdToMesh = std::unordered_map<std::string, Mesh>;

MeshIdToMesh initSceneMeshes(const TerrainData &terrainData);

void updateTerrainMeshTexture(Mesh *terrainMesh, const NoiseMapData &noiseMapData, const FalloffMap &falloffMap,
                              const std::vector<TerrainType> &terrainTypes);