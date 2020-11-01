#include "meshGenerator.h"

#include "glm\gtc\matrix_transform.hpp"
#include "lightDefs.h"
#include "terrainDefs.h"
#include "textureGenerator.h"
#include <assert.h>

static void createVertexBufferObject(GLuint *vboHandle, const std::vector<Vertex> &vertices) {
  glBindBuffer(GL_ARRAY_BUFFER, *vboHandle);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void createIndexBufferObject(GLuint *iboHandle, const std::vector<uint32_t> &indices) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *iboHandle);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

static void calculateMeshNormals(Mesh *mesh) {
  for (size_t i = 0, size = mesh->indices.size(); i < size; i = i + 3) {
    auto &v1 = mesh->vertices[mesh->indices[i]];
    auto &v2 = mesh->vertices[mesh->indices[i + 1]];
    auto &v3 = mesh->vertices[mesh->indices[i + 2]];

    const auto edgeOne = glm::vec3(v2.position3f) - glm::vec3(v1.position3f);
    const auto edgeTwo = glm::vec3(v3.position3f) - glm::vec3(v1.position3f);
    const auto faceNormal = glm::cross(edgeOne, edgeTwo);
    v1.normal += faceNormal;
    v2.normal += faceNormal;
    v3.normal += faceNormal;
  }

  for (size_t i = 0, size = mesh->vertices.size(); i < size; i++) {
    auto &v = mesh->vertices[i];
    v.normal = glm::normalize(v.normal);
  }
}

static Mesh generateMeshHeightMapVertices(const int mapWidth, const int mapHeight, const NoiseMap &noiseMap) {
  Mesh heightMapMesh = {};

  // Assume height map texture is a multiple of 64 (so minimum is that it contains one patch)
  int numOfPatchesX = mapWidth / int(PATCH_SIZE);
  int numOfPatchesZ = mapHeight / int(PATCH_SIZE);

  int numOfPatches = numOfPatchesX * numOfPatchesZ;
  heightMapMesh.vertices.reserve(numOfPatches);
  heightMapMesh.indices.reserve(numOfPatches);

  for (size_t i = 0; i < numOfPatchesZ; ++i) {
    for (size_t j = 0; j < numOfPatchesX; ++j) {
      Vertex vertex = {};
      vertex.position2f = glm::vec2((j * PATCH_SIZE) * 1.0f / mapWidth, (i * PATCH_SIZE) * 1.0f / mapHeight);
      heightMapMesh.vertices.push_back(vertex);
    }
  }

  for (size_t i = 0; i < numOfPatches; i++) {
    heightMapMesh.indices.push_back(int(i));
  }

  assert(heightMapMesh.vertices.size() == numOfPatches);
  assert(heightMapMesh.indices.size() == numOfPatches);

  return heightMapMesh;
}

static Mesh generateMeshFromHeightMap(const NoiseMapData &noiseMapData, const bool useFalloffMap,
                                      const std::vector<TerrainType> &terrainTypes) {
  const auto noiseMap = generateNoiseMap(noiseMapData, useFalloffMap);

  Mesh terrainMesh = generateMeshHeightMapVertices(noiseMapData.width, noiseMapData.height, noiseMap);
  // calculateMeshNormals(&terrainMesh);
  terrainMesh.modelTransformation = glm::identity<glm::mat4>();

  glGenBuffers(1, &terrainMesh.vboHandle);
  createVertexBufferObject(&terrainMesh.vboHandle, terrainMesh.vertices);

  glGenBuffers(1, &terrainMesh.iboHandle);
  createIndexBufferObject(&terrainMesh.iboHandle, terrainMesh.indices);

  glGenVertexArrays(1, &terrainMesh.vaoHandle);
  glBindVertexArray(terrainMesh.vaoHandle);

  glBindBuffer(GL_ARRAY_BUFFER, terrainMesh.vboHandle);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainMesh.iboHandle);

  glBindVertexArray(0);

  glGenTextures(4, terrainMesh.textureHandles);
  createTexture2D(&terrainMesh.textureHandles[0], GL_CLAMP_TO_EDGE, GL_NEAREST, noiseMapData.width,
                  noiseMapData.height, GL_FLOAT, generateNoiseMapTexture(noiseMap).data());
  createTexture2D(&terrainMesh.textureHandles[1], GL_CLAMP_TO_EDGE, GL_NEAREST, noiseMapData.width,
                  noiseMapData.height, GL_FLOAT, generateColorMapTexture(noiseMap, terrainTypes).data());
  createTexture2D(&terrainMesh.textureHandles[2], GL_CLAMP_TO_EDGE, GL_NEAREST, noiseMapData.width,
                  noiseMapData.height, GL_FLOAT, generateFalloffMap(noiseMapData.width).data());

  int dudvWidth, dudvHeight;
  unsigned char *dudvPixelData = nullptr;
  loadTexture("waterDuDv1.png", waterDuDvTexturePath, &dudvWidth, &dudvHeight, &dudvPixelData);
  assert(dudvPixelData != nullptr);
  createTexture2D(&terrainMesh.textureHandles[3], GL_REPEAT, GL_NEAREST, dudvWidth, dudvHeight,
                  GL_UNSIGNED_BYTE, dudvPixelData);
  freeTexture(dudvPixelData);

  int normalMapWidth, normalMapHeight;
  unsigned char *normalMapPixelData = nullptr;
  loadTexture("waterNormalMap1.png", waterNormalMapTexturePath, &normalMapWidth, &normalMapHeight,
              &normalMapPixelData);
  assert(normalMapPixelData != nullptr);
  createTexture2D(&terrainMesh.textureHandles[4], GL_REPEAT, GL_NEAREST, normalMapWidth, normalMapHeight,
                  GL_UNSIGNED_BYTE, normalMapPixelData);
  freeTexture(normalMapPixelData);

  return terrainMesh;
}

static Mesh generateSkyboxMesh() {
  Mesh skyboxMesh = {};

  // 0-3
  skyboxMesh.vertices.push_back({.position3f = {-1.0f, 1.0f, -1.0f}});
  skyboxMesh.vertices.push_back({.position3f = {-1.0f, -1.0f, -1.0f}});
  skyboxMesh.vertices.push_back({.position3f = {1.0f, -1.0f, -1.0f}});
  skyboxMesh.vertices.push_back({.position3f = {1.0f, 1.0f, -1.0f}});

  // 4-7
  skyboxMesh.vertices.push_back({.position3f = {-1.0f, -1.0f, 1.0f}});
  skyboxMesh.vertices.push_back({.position3f = {-1.0f, 1.0f, 1.0f}});
  skyboxMesh.vertices.push_back({.position3f = {1.0f, -1.0f, 1.0f}});
  skyboxMesh.vertices.push_back({.position3f = {1.0f, 1.0f, 1.0f}});

  // Back
  skyboxMesh.indices.push_back(0);
  skyboxMesh.indices.push_back(1);
  skyboxMesh.indices.push_back(2);
  skyboxMesh.indices.push_back(2);
  skyboxMesh.indices.push_back(3);
  skyboxMesh.indices.push_back(0);

  // Left
  skyboxMesh.indices.push_back(4);
  skyboxMesh.indices.push_back(1);
  skyboxMesh.indices.push_back(0);
  skyboxMesh.indices.push_back(0);
  skyboxMesh.indices.push_back(5);
  skyboxMesh.indices.push_back(4);

  // Right
  skyboxMesh.indices.push_back(2);
  skyboxMesh.indices.push_back(6);
  skyboxMesh.indices.push_back(7);
  skyboxMesh.indices.push_back(7);
  skyboxMesh.indices.push_back(3);
  skyboxMesh.indices.push_back(2);

  // Front
  skyboxMesh.indices.push_back(4);
  skyboxMesh.indices.push_back(5);
  skyboxMesh.indices.push_back(7);
  skyboxMesh.indices.push_back(7);
  skyboxMesh.indices.push_back(6);
  skyboxMesh.indices.push_back(4);

  // Top
  skyboxMesh.indices.push_back(0);
  skyboxMesh.indices.push_back(3);
  skyboxMesh.indices.push_back(7);
  skyboxMesh.indices.push_back(7);
  skyboxMesh.indices.push_back(5);
  skyboxMesh.indices.push_back(0);

  // Bottom
  skyboxMesh.indices.push_back(1);
  skyboxMesh.indices.push_back(4);
  skyboxMesh.indices.push_back(2);
  skyboxMesh.indices.push_back(2);
  skyboxMesh.indices.push_back(4);
  skyboxMesh.indices.push_back(6);

  skyboxMesh.modelTransformation = glm::identity<glm::mat4>();

  glGenBuffers(1, &skyboxMesh.vboHandle);
  createVertexBufferObject(&skyboxMesh.vboHandle, skyboxMesh.vertices);

  glGenBuffers(1, &skyboxMesh.iboHandle);
  createIndexBufferObject(&skyboxMesh.iboHandle, skyboxMesh.indices);

  glGenVertexArrays(1, &skyboxMesh.vaoHandle);
  glBindVertexArray(skyboxMesh.vaoHandle);

  glBindBuffer(GL_ARRAY_BUFFER, skyboxMesh.vboHandle);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxMesh.iboHandle);

  glBindVertexArray(0);

  std::vector<std::string> facesNames{
      "right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg",
  };

  glGenTextures(1, skyboxMesh.textureHandles);
  createCubeMapTexture(&skyboxMesh.textureHandles[0], facesNames);

  return skyboxMesh;
}

static Mesh generateLightMesh(const LightData &lightData) {
  Mesh lightMesh = {};

  // 0-3
  lightMesh.vertices.push_back({.position3f = {-1.0f, 1.0f, -1.0f}});
  lightMesh.vertices.push_back({.position3f = {-1.0f, -1.0f, -1.0f}});
  lightMesh.vertices.push_back({.position3f = {1.0f, -1.0f, -1.0f}});
  lightMesh.vertices.push_back({.position3f = {1.0f, 1.0f, -1.0f}});

  // 4-7
  lightMesh.vertices.push_back({.position3f = {-1.0f, -1.0f, 1.0f}});
  lightMesh.vertices.push_back({.position3f = {-1.0f, 1.0f, 1.0f}});
  lightMesh.vertices.push_back({.position3f = {1.0f, -1.0f, 1.0f}});
  lightMesh.vertices.push_back({.position3f = {1.0f, 1.0f, 1.0f}});

  // Back
  lightMesh.indices.push_back(0);
  lightMesh.indices.push_back(1);
  lightMesh.indices.push_back(2);
  lightMesh.indices.push_back(2);
  lightMesh.indices.push_back(3);
  lightMesh.indices.push_back(0);

  // Left
  lightMesh.indices.push_back(4);
  lightMesh.indices.push_back(1);
  lightMesh.indices.push_back(0);
  lightMesh.indices.push_back(0);
  lightMesh.indices.push_back(5);
  lightMesh.indices.push_back(4);

  // Right
  lightMesh.indices.push_back(2);
  lightMesh.indices.push_back(6);
  lightMesh.indices.push_back(7);
  lightMesh.indices.push_back(7);
  lightMesh.indices.push_back(3);
  lightMesh.indices.push_back(2);

  // Front
  lightMesh.indices.push_back(4);
  lightMesh.indices.push_back(5);
  lightMesh.indices.push_back(7);
  lightMesh.indices.push_back(7);
  lightMesh.indices.push_back(6);
  lightMesh.indices.push_back(4);

  // Top
  lightMesh.indices.push_back(0);
  lightMesh.indices.push_back(3);
  lightMesh.indices.push_back(7);
  lightMesh.indices.push_back(7);
  lightMesh.indices.push_back(5);
  lightMesh.indices.push_back(0);

  // Bottom
  lightMesh.indices.push_back(1);
  lightMesh.indices.push_back(4);
  lightMesh.indices.push_back(2);
  lightMesh.indices.push_back(2);
  lightMesh.indices.push_back(4);
  lightMesh.indices.push_back(6);

  lightMesh.modelTransformation =
      glm::translate(glm::identity<glm::mat4>(), glm::vec3(lightData.worldLightPosition));
  lightMesh.modelTransformation = glm::scale(lightMesh.modelTransformation, glm::vec3(5.0f));

  glGenBuffers(1, &lightMesh.vboHandle);
  createVertexBufferObject(&lightMesh.vboHandle, lightMesh.vertices);

  glGenBuffers(1, &lightMesh.iboHandle);
  createIndexBufferObject(&lightMesh.iboHandle, lightMesh.indices);

  glGenVertexArrays(1, &lightMesh.vaoHandle);
  glBindVertexArray(lightMesh.vaoHandle);

  glBindBuffer(GL_ARRAY_BUFFER, lightMesh.vboHandle);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightMesh.iboHandle);

  glBindVertexArray(0);

  return lightMesh;
}

static Mesh generateWaterMesh(const int mapWidth, const int mapHeight) {
  Mesh waterMesh{};
  // Assume height map texture is a multiple of 64 (so minimum is that it contains one patch)
  int numOfPatchesX = mapWidth / int(PATCH_SIZE);
  int numOfPatchesZ = mapHeight / int(PATCH_SIZE);

  Vertex v1 = {};
  v1.position3f = glm::vec3(0.0f, 0.0f, 0.0f);
  v1.textureCoordinate = glm::vec2(0.0f, 0.0f);

  Vertex v2 = {};
  v2.position3f = glm::vec3(0.0f, 0.0f, numOfPatchesZ * PATCH_SIZE);
  v2.textureCoordinate = glm::vec2(1.0f, 0.0f);

  Vertex v3 = {};
  v3.position3f = glm::vec3(numOfPatchesX * PATCH_SIZE, 0.0f, 0.0f);
  v2.textureCoordinate = glm::vec2(0.0f, 1.0f);

  Vertex v4 = {};
  v4.position3f = glm::vec3(numOfPatchesX * PATCH_SIZE, 0.0f, numOfPatchesZ * PATCH_SIZE);
  v4.textureCoordinate = glm::vec2(1.0f, 1.0f);

  waterMesh.vertices.push_back(v1);
  waterMesh.vertices.push_back(v2);
  waterMesh.vertices.push_back(v3);
  waterMesh.vertices.push_back(v4);

  waterMesh.indices.push_back(0);
  waterMesh.indices.push_back(1);
  waterMesh.indices.push_back(2);

  waterMesh.indices.push_back(3);
  waterMesh.indices.push_back(2);
  waterMesh.indices.push_back(1);

  waterMesh.modelTransformation = glm::identity<glm::mat4>();

  glGenBuffers(1, &waterMesh.vboHandle);
  createVertexBufferObject(&waterMesh.vboHandle, waterMesh.vertices);

  glGenBuffers(1, &waterMesh.iboHandle);
  createIndexBufferObject(&waterMesh.iboHandle, waterMesh.indices);

  glGenVertexArrays(1, &waterMesh.vaoHandle);
  glBindVertexArray(waterMesh.vaoHandle);

  glBindBuffer(GL_ARRAY_BUFFER, waterMesh.vboHandle);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(7 * sizeof(float)));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterMesh.iboHandle);

  glBindVertexArray(0);

  return waterMesh;
}

MeshIdToMesh initSceneMeshes(const TerrainData &terrainData, const LightData &lightData) {
  MeshIdToMesh meshIdToMesh;
  meshIdToMesh.reserve(4);

  meshIdToMesh.emplace(kSkyboxMeshId, generateSkyboxMesh());

  meshIdToMesh.emplace(kTerrainMeshId,
                       generateMeshFromHeightMap(terrainData.noiseMapData, terrainData.useFalloffMap,
                                                 terrainData.terrainTypes));

  meshIdToMesh.emplace(kLightMeshId, generateLightMesh(lightData));

  meshIdToMesh.emplace(kWaterMeshId,
                       generateWaterMesh(terrainData.noiseMapData.width, terrainData.noiseMapData.height));

  return meshIdToMesh;
}

void updateTerrainMeshTexture(Mesh *terrainMesh, const NoiseMapData &noiseMapData, const bool useFalloffMap,
                              const std::vector<TerrainType> &terrainTypes) {
  const auto noiseMap = generateNoiseMap(noiseMapData, useFalloffMap);
  updateTexture2D(&terrainMesh->textureHandles[0], 0, 0, noiseMapData.width, noiseMapData.height, GL_FLOAT,
                  generateNoiseMapTexture(noiseMap).data());
  updateTexture2D(&terrainMesh->textureHandles[1], 0, 0, noiseMapData.width, noiseMapData.height, GL_FLOAT,
                  generateColorMapTexture(noiseMap, terrainTypes).data());
}

void updateTerrainMeshWaterTextures(Mesh *terrainMesh, const std::string mapIndex) {
  int dudvWidth, dudvHeight;
  unsigned char *dudvPixelData = nullptr;
  loadTexture("waterDuDv" + mapIndex + ".png", waterDuDvTexturePath, &dudvWidth, &dudvHeight, &dudvPixelData);
  assert(dudvPixelData != nullptr);
  createTexture2D(&terrainMesh->textureHandles[3], GL_REPEAT, GL_NEAREST, dudvWidth, dudvHeight,
                  GL_UNSIGNED_BYTE, dudvPixelData);
  freeTexture(dudvPixelData);

  int normalMapWidth, normalMapHeight;
  unsigned char *normalMapPixelData = nullptr;
  loadTexture("waterNormalMap" + mapIndex + ".png", waterNormalMapTexturePath, &normalMapWidth,
              &normalMapHeight, &normalMapPixelData);
  assert(normalMapPixelData != nullptr);
  createTexture2D(&terrainMesh->textureHandles[4], GL_REPEAT, GL_NEAREST, normalMapWidth, normalMapHeight,
                  GL_UNSIGNED_BYTE, normalMapPixelData);
  freeTexture(normalMapPixelData);
}