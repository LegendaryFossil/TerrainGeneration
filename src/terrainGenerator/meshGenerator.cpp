#include "meshGenerator.h"

#include "glm\gtc\matrix_transform.hpp"
#include "terrainDefs.h"
#include <assert.h>

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

static void calculateMeshVertices(Mesh *mesh, const NoiseMap &noiseMap) {
  const auto mapHeight = int(noiseMap.size());
  const auto mapWidth = int(noiseMap.front().size());

  // Assume height map texture is a multiple of 64 (so minimum is that it contains one patch)
  int numOfPatchesX = mapWidth / int(PATCH_SIZE);
  int numOfPatchesZ = mapHeight / int(PATCH_SIZE);

  int numOfPatches = numOfPatchesX * numOfPatchesZ;
  mesh->vertices.reserve(numOfPatches);
  mesh->indices.reserve(numOfPatches);

  for (size_t i = 0; i < numOfPatchesZ; ++i) {
    for (size_t j = 0; j < numOfPatchesX; ++j) {
      Vertex vertex = {};
      vertex.position2f = glm::vec2((j * PATCH_SIZE) * 1.0f / mapWidth, (i * PATCH_SIZE) * 1.0f / mapHeight);
      mesh->vertices.push_back(vertex);
    }
  }

  for (size_t i = 0; i < numOfPatches; i++) {
    mesh->indices.push_back(int(i));
  }

  assert(mesh->vertices.size() == numOfPatches);
  assert(mesh->indices.size() == numOfPatches);
}

Mesh generateMeshFromHeightMap(const NoiseMap &noiseMap) {
  Mesh terrainMesh = {};
  calculateMeshVertices(&terrainMesh, noiseMap);
  // calculateMeshNormals(&terrainMesh);
  // terrainMesh.modelTransformation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -100.0f));

  return terrainMesh;
}

void createVertexBufferObject(GLuint *vboHandle, const std::vector<Vertex> &vertices) {
  glBindBuffer(GL_ARRAY_BUFFER, *vboHandle);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void createIndexBufferObject(GLuint *iboHandle, const std::vector<uint32_t> &indices) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *iboHandle);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}