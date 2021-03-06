#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "noiseMapGenerator.h"
#include <unordered_map>
#include <vector>

constexpr auto MAX_TEXTURES = 10;

struct LightData;
struct TerrainData;
struct TerrainProperty;

struct Vertex {
  union {
    glm::vec2 position2f;
    glm::vec3 position3f;
    glm::vec4 position4f;
  };

  glm::vec3 normal;
  glm::vec3 tangent;
  glm::vec3 bitangent;
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

constexpr auto kSkyboxMeshId = "skybox";
constexpr auto kTerrainMeshId = "terrain";
constexpr auto kWaterMeshId = "water";

using MeshIdToMesh = std::unordered_map<std::string, Mesh>;

MeshIdToMesh initSceneMeshes(const TerrainData &terrainData);
std::vector<Mesh> initLightMeshes(const LightData &lightData);

void updateTerrainMeshTexture(Mesh *terrainMesh, const NoiseMapData &noiseMapData, const bool useFalloffMap,
                              const std::vector<glm::vec3> &colors, const std::vector<float> &heights);
void updateTerrainMeshWaterTextures(Mesh *terrainMesh, const std::string mapIndex);