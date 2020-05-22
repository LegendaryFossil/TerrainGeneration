#include "meshGenerator.h"

#include <assert.h>

#include "glm\gtc\matrix_transform.hpp"

#define PATCHSIZE 64

static void calculateMeshNormals(Mesh* mesh) {
  for (size_t i = 0, size = mesh->indices.size(); i < size; i = i + 3) {
    auto& v1 = mesh->vertices[mesh->indices[i]];
    auto& v2 = mesh->vertices[mesh->indices[i + 1]];
    auto& v3 = mesh->vertices[mesh->indices[i + 2]];

    const auto edgeOne = glm::vec3(v2.position3f) - glm::vec3(v1.position3f);
    const auto edgeTwo = glm::vec3(v3.position3f) - glm::vec3(v1.position3f);
    const auto faceNormal = glm::cross(edgeOne, edgeTwo);
    v1.normal += faceNormal;
    v2.normal += faceNormal;
    v3.normal += faceNormal;
  }

  for (size_t i = 0, size = mesh->vertices.size(); i < size; i++) {
    auto& v = mesh->vertices[i];
    v.normal = glm::normalize(v.normal);
  }
}

static void calculateMeshVertices(Mesh* mesh, const NoiseMap& noiseMap) {
  const auto mapHeight = int(noiseMap.size());
  const auto mapWidth = int(noiseMap.front().size());

  int patchSize = 1;
  // We're assuming that the heightmap is a multiple of 64 and contains at least one patch
  int nopX = (mapWidth / PATCHSIZE);
  int nopZ = (mapHeight / PATCHSIZE);
  int numPatches = nopX * nopZ;

  const auto numOfIndices = numPatches * patchSize;
  const auto numOfVertices = numOfIndices;

  mesh->vertices.reserve(numOfVertices);
  mesh->indices.reserve(numOfIndices);

  for (size_t i = 0; i < nopX; ++i) {
    for (size_t j = 0; j < nopZ; ++j) {
      Vertex vertex = {};
      vertex.position2f = glm::vec2((i * PATCHSIZE) * 1.0f / mapWidth, (j * PATCHSIZE) * 1.0f / mapHeight);
      mesh->vertices.push_back(vertex);
    }
  }

  for (size_t i = 0; i < numOfIndices; i++) {
    mesh->indices.push_back(int(i));
  }

  assert(mesh->vertices.size() == numOfVertices);
  assert(mesh->indices.size() == numOfIndices);
  //assert(numOfIndices % 3 == 0);
}

Mesh generateMeshFromHeightMap(const NoiseMap& noiseMap) {
  Mesh terrainMesh = {};
  calculateMeshVertices(&terrainMesh, noiseMap);
  //calculateMeshNormals(&terrainMesh);
  terrainMesh.modelTransformation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -100.0f));

  return terrainMesh;
}