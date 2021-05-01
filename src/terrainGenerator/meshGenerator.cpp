#include "meshGenerator.h"

#include "glm\gtc\matrix_transform.hpp"
#include "lightDefs.h"
#include "terrainDefs.h"
#include "textureGenerator.h"
#include <assert.h>

static void validateTerrainMeshTextureData(const unsigned char *pixelData, const int width, const int height,
                                           const int expectedWidth, const int expectedHeight) {
  assert(pixelData != nullptr);
  assert(width == expectedWidth);
  assert(height == expectedHeight);
}

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

static void calculateNormals(Mesh *mesh) {
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

static void calculateTangentVectors(Mesh *mesh) {
  for (size_t i = 0, size = mesh->indices.size(); i < size; i = i + 3) {
    auto &v1 = mesh->vertices[mesh->indices[i]];
    auto &v2 = mesh->vertices[mesh->indices[i + 1]];
    auto &v3 = mesh->vertices[mesh->indices[i + 2]];

    const auto edgeOne = glm::vec3(v2.position3f) - glm::vec3(v1.position3f);
    const auto edgeTwo = glm::vec3(v3.position3f) - glm::vec3(v1.position3f);
    const auto deltaUVOne = v2.textureCoordinate - v1.textureCoordinate;
    const auto deltaUVTwo = v3.textureCoordinate - v1.textureCoordinate;

    const auto fract = 1.0f / (deltaUVOne.x * deltaUVTwo.y - deltaUVTwo.x * deltaUVOne.y);

    glm::vec3 tangent(0.0f);
    glm::vec3 bitangent(0.0f);

    tangent.x = fract * (deltaUVTwo.y * edgeOne.x - deltaUVOne.y * edgeTwo.x);
    tangent.y = fract * (deltaUVTwo.y * edgeOne.y - deltaUVOne.y * edgeTwo.y);
    tangent.z = fract * (deltaUVTwo.y * edgeOne.z - deltaUVOne.y * edgeTwo.z);

    bitangent.x = fract * (-deltaUVTwo.x * edgeOne.x + deltaUVOne.x * edgeTwo.x);
    bitangent.y = fract * (-deltaUVTwo.x * edgeOne.y + deltaUVOne.x * edgeTwo.y);
    bitangent.z = fract * (-deltaUVTwo.x * edgeOne.z + deltaUVOne.x * edgeTwo.z);

    v1.tangent += tangent;
    v2.tangent += tangent;
    v3.tangent += tangent;

    v1.bitangent += bitangent;
    v2.bitangent += bitangent;
    v3.bitangent += bitangent;
  }

  for (size_t i = 0, size = mesh->vertices.size(); i < size; i++) {
    auto &v = mesh->vertices[i];
    const auto &T = v.tangent;
    const auto &B = v.bitangent;
    const auto &N = v.normal;
    v.tangent = normalize(T - glm::dot(T, N) * N);
    v.bitangent = glm::cross(N, T);
  }
}

static Mesh generateMeshHeightMapVertices(const int mapWidth, const int mapHeight, const NoiseMap &noiseMap) {
  Mesh heightMapMesh = {};

  // Assume height map texture is a multiple of 64 (so minimum is that it contains one patch)
  int numOfPatchesX = mapWidth / int(kPatchSize);
  int numOfPatchesZ = mapHeight / int(kPatchSize);

  int numOfPatches = numOfPatchesX * numOfPatchesZ;
  heightMapMesh.vertices.reserve(numOfPatches);
  heightMapMesh.indices.reserve(numOfPatches);

  for (size_t i = 0; i < numOfPatchesZ; ++i) {
    for (size_t j = 0; j < numOfPatchesX; ++j) {
      Vertex vertex = {};
      vertex.position2f = glm::vec2((j * kPatchSize) * 1.0f / mapWidth, (i * kPatchSize) * 1.0f / mapHeight);
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
                                      const std::vector<glm::vec3> &colors,
                                      const std::vector<float> &heights) {
  const auto noiseMap = generateNoiseMap(noiseMapData, useFalloffMap);

  Mesh terrainMesh = generateMeshHeightMapVertices(noiseMapData.width, noiseMapData.height, noiseMap);
  terrainMesh.modelTransformation = glm::identity<glm::mat4>();

  glGenBuffers(1, &terrainMesh.vboHandle);
  createVertexBufferObject(&terrainMesh.vboHandle, terrainMesh.vertices);

  glGenBuffers(1, &terrainMesh.iboHandle);
  createIndexBufferObject(&terrainMesh.iboHandle, terrainMesh.indices);

  glGenVertexArrays(1, &terrainMesh.vaoHandle);
  glBindVertexArray(terrainMesh.vaoHandle);

  glBindBuffer(GL_ARRAY_BUFFER, terrainMesh.vboHandle);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 15 * sizeof(float), 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainMesh.iboHandle);

  glBindVertexArray(0);

  glGenTextures(3, terrainMesh.textureHandles);
  createTexture2D(&terrainMesh.textureHandles[0], GL_CLAMP_TO_EDGE, GL_NEAREST, noiseMapData.width,
                  noiseMapData.height, GL_FLOAT, generateNoiseMapTexture(noiseMap).data());
  createTexture2D(&terrainMesh.textureHandles[1], GL_CLAMP_TO_EDGE, GL_NEAREST, noiseMapData.width,
                  noiseMapData.height, GL_FLOAT, generateFalloffMap(noiseMapData.width).data());

  std::vector<unsigned char *> terrainTexturesPixelData;

  const auto expectedTerrainTextureDimension = 1024; // Ensure it is 1024x1024 textures
  int terrainTextureWidth, terrainTextureHeight;
  unsigned char *terrainTexturePixelData = nullptr;

  loadTexture("sand.png", terrainTexturePath, &terrainTextureWidth, &terrainTextureHeight,
              &terrainTexturePixelData);
  validateTerrainMeshTextureData(terrainTexturePixelData, terrainTextureWidth, terrainTextureHeight,
                                 expectedTerrainTextureDimension, expectedTerrainTextureDimension);
  terrainTexturesPixelData.push_back(terrainTexturePixelData);

  loadTexture("sand.png", terrainTexturePath, &terrainTextureWidth, &terrainTextureHeight,
              &terrainTexturePixelData);
  validateTerrainMeshTextureData(terrainTexturePixelData, terrainTextureWidth, terrainTextureHeight,
                                 expectedTerrainTextureDimension, expectedTerrainTextureDimension);
  terrainTexturesPixelData.push_back(terrainTexturePixelData);

  loadTexture("grass.png", terrainTexturePath, &terrainTextureWidth, &terrainTextureHeight,
              &terrainTexturePixelData);
  validateTerrainMeshTextureData(terrainTexturePixelData, terrainTextureWidth, terrainTextureHeight,
                                 expectedTerrainTextureDimension, expectedTerrainTextureDimension);
  terrainTexturesPixelData.push_back(terrainTexturePixelData);

  loadTexture("rock.png", terrainTexturePath, &terrainTextureWidth, &terrainTextureHeight,
              &terrainTexturePixelData);
  validateTerrainMeshTextureData(terrainTexturePixelData, terrainTextureWidth, terrainTextureHeight,
                                 expectedTerrainTextureDimension, expectedTerrainTextureDimension);
  terrainTexturesPixelData.push_back(terrainTexturePixelData);

  loadTexture("mountain.png", terrainTexturePath, &terrainTextureWidth, &terrainTextureHeight,
              &terrainTexturePixelData);
  validateTerrainMeshTextureData(terrainTexturePixelData, terrainTextureWidth, terrainTextureHeight,
                                 expectedTerrainTextureDimension, expectedTerrainTextureDimension);
  terrainTexturesPixelData.push_back(terrainTexturePixelData);

  loadTexture("snow.png", terrainTexturePath, &terrainTextureWidth, &terrainTextureHeight,
              &terrainTexturePixelData);
  validateTerrainMeshTextureData(terrainTexturePixelData, terrainTextureWidth, terrainTextureHeight,
                                 expectedTerrainTextureDimension, expectedTerrainTextureDimension);
  terrainTexturesPixelData.push_back(terrainTexturePixelData);

  createTexture2DArray(&terrainMesh.textureHandles[2], GL_REPEAT, GL_NEAREST, terrainTextureWidth,
                       terrainTextureHeight, GL_UNSIGNED_BYTE, terrainTexturesPixelData);

  for (auto pixelData : terrainTexturesPixelData) {
    freeTexture(pixelData);
  }

  terrainTexturesPixelData.clear();

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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(float), 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxMesh.iboHandle);

  glBindVertexArray(0);

  std::vector<std::string> facesNames{
      "right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg",
  };

  glGenTextures(1, skyboxMesh.textureHandles);
  createCubeMapTexture(&skyboxMesh.textureHandles[0], facesNames);

  return skyboxMesh;
}

static Mesh generateLightMesh(const glm::vec4 lightPosition) {
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

  lightMesh.modelTransformation = glm::translate(glm::identity<glm::mat4>(), glm::vec3(lightPosition));
  lightMesh.modelTransformation = glm::scale(lightMesh.modelTransformation, glm::vec3(5.0f));

  glGenBuffers(1, &lightMesh.vboHandle);
  createVertexBufferObject(&lightMesh.vboHandle, lightMesh.vertices);

  glGenBuffers(1, &lightMesh.iboHandle);
  createIndexBufferObject(&lightMesh.iboHandle, lightMesh.indices);

  glGenVertexArrays(1, &lightMesh.vaoHandle);
  glBindVertexArray(lightMesh.vaoHandle);

  glBindBuffer(GL_ARRAY_BUFFER, lightMesh.vboHandle);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(float), 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightMesh.iboHandle);

  glBindVertexArray(0);

  return lightMesh;
}

static Mesh generateWaterMesh(const int mapWidth, const int mapHeight) {
  Mesh waterMesh{};
  // Assume height map texture is a multiple of 64 (so minimum is that it contains one patch)
  int numOfPatchesX = mapWidth / int(kPatchSize);
  int numOfPatchesZ = mapHeight / int(kPatchSize);

  Vertex v1 = {};
  const auto height = 0.2f;
  v1.position3f = glm::vec3(0.0f, height, 0.0f);
  v1.textureCoordinate = glm::vec2(0.0f, 0.0f);

  Vertex v2 = {};
  v2.position3f = glm::vec3(0.0f, height, numOfPatchesZ * kPatchSize);
  v2.textureCoordinate = glm::vec2(0.0f, 1.0f);

  Vertex v3 = {};
  v3.position3f = glm::vec3(numOfPatchesX * kPatchSize, height, 0.0f);
  v3.textureCoordinate = glm::vec2(1.0f, 0.0f);

  Vertex v4 = {};
  v4.position3f = glm::vec3(numOfPatchesX * kPatchSize, height, numOfPatchesZ * kPatchSize);
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

  calculateNormals(&waterMesh);
  calculateTangentVectors(&waterMesh);

  waterMesh.modelTransformation = glm::identity<glm::mat4>();

  glGenBuffers(1, &waterMesh.vboHandle);
  createVertexBufferObject(&waterMesh.vboHandle, waterMesh.vertices);

  glGenBuffers(1, &waterMesh.iboHandle);
  createIndexBufferObject(&waterMesh.iboHandle, waterMesh.indices);

  glGenVertexArrays(1, &waterMesh.vaoHandle);
  glBindVertexArray(waterMesh.vaoHandle);

  glBindBuffer(GL_ARRAY_BUFFER, waterMesh.vboHandle);

  // Position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(float), 0);

  // Normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (void *)(4 * sizeof(float)));

  // Tangent
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (void *)(7 * sizeof(float)));

  // Bitangent
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (void *)(10 * sizeof(float)));

  // Texture coordinates
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (void *)(13 * sizeof(float)));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterMesh.iboHandle);

  glBindVertexArray(0);

  glGenTextures(2, waterMesh.textureHandles);

  int dudvWidth, dudvHeight;
  unsigned char *dudvPixelData = nullptr;
  loadTexture("waterDuDv1.png", waterDuDvTexturePath, &dudvWidth, &dudvHeight, &dudvPixelData);
  assert(dudvPixelData != nullptr);
  createTexture2D(&waterMesh.textureHandles[0], GL_REPEAT, GL_LINEAR, dudvWidth, dudvHeight, GL_UNSIGNED_BYTE,
                  dudvPixelData);
  freeTexture(dudvPixelData);

  int normalMapWidth, normalMapHeight;
  unsigned char *normalMapPixelData = nullptr;
  loadTexture("waterNormalMap1.png", waterNormalMapTexturePath, &normalMapWidth, &normalMapHeight,
              &normalMapPixelData);
  assert(normalMapPixelData != nullptr);
  createTexture2D(&waterMesh.textureHandles[1], GL_REPEAT, GL_LINEAR, normalMapWidth, normalMapHeight,
                  GL_UNSIGNED_BYTE, normalMapPixelData);
  freeTexture(normalMapPixelData);

  return waterMesh;
}

MeshIdToMesh initSceneMeshes(const TerrainData &terrainData) {
  MeshIdToMesh meshIdToMesh;
  meshIdToMesh.reserve(4);

  meshIdToMesh.emplace(kSkyboxMeshId, generateSkyboxMesh());

  meshIdToMesh.emplace(kTerrainMeshId,
                       generateMeshFromHeightMap(terrainData.noiseMapData, terrainData.useFalloffMap,
                                                 terrainData.terrainProperties.colors,
                                                 terrainData.terrainProperties.heights));

  meshIdToMesh.emplace(kWaterMeshId,
                       generateWaterMesh(terrainData.noiseMapData.width, terrainData.noiseMapData.height));

  return meshIdToMesh;
}

std::vector<Mesh> initLightMeshes(const LightData &lightData) {
  std::vector<Mesh> lightMeshes;

  for (size_t i = 0; i < lightData.lightCount; ++i) {
    lightMeshes.push_back(generateLightMesh(lightData.positions[i]));
  }

  return lightMeshes;
}

void updateTerrainMeshTexture(Mesh *terrainMesh, const NoiseMapData &noiseMapData, const bool useFalloffMap,
                              const std::vector<glm::vec3> &colors, const std::vector<float> &heights) {
  const auto noiseMap = generateNoiseMap(noiseMapData, useFalloffMap);
  updateTexture2D(&terrainMesh->textureHandles[0], 0, 0, noiseMapData.width, noiseMapData.height, GL_FLOAT,
                  generateNoiseMapTexture(noiseMap).data());
}

void updateTerrainMeshWaterTextures(Mesh *waterMesh, const std::string mapIndex) {
  int dudvWidth, dudvHeight;
  unsigned char *dudvPixelData = nullptr;
  loadTexture("waterDuDv" + mapIndex + ".png", waterDuDvTexturePath, &dudvWidth, &dudvHeight, &dudvPixelData);
  assert(dudvPixelData != nullptr);
  createTexture2D(&waterMesh->textureHandles[0], GL_REPEAT, GL_LINEAR, dudvWidth, dudvHeight,
                  GL_UNSIGNED_BYTE, dudvPixelData);
  freeTexture(dudvPixelData);

  int normalMapWidth, normalMapHeight;
  unsigned char *normalMapPixelData = nullptr;
  loadTexture("waterNormalMap" + mapIndex + ".png", waterNormalMapTexturePath, &normalMapWidth,
              &normalMapHeight, &normalMapPixelData);
  assert(normalMapPixelData != nullptr);
  createTexture2D(&waterMesh->textureHandles[1], GL_REPEAT, GL_LINEAR, normalMapWidth, normalMapHeight,
                  GL_UNSIGNED_BYTE, normalMapPixelData);
  freeTexture(normalMapPixelData);
}