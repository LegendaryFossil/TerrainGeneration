#include "meshGenerator.h"

#include <assert.h>

#include "glm\gtc\matrix_transform.hpp"

static void calculateMeshNormals(Mesh *mesh) {
  for (size_t i = 0, size = mesh->indices.size(); i < size; i = i + 3) {
    auto &v1 = mesh->vertices[mesh->indices[i]];
    auto &v2 = mesh->vertices[mesh->indices[i + 1]];
    auto &v3 = mesh->vertices[mesh->indices[i + 2]];

    const auto edgeOne = glm::vec3(v2.position) - glm::vec3(v1.position);
    const auto edgeTwo = glm::vec3(v3.position) - glm::vec3(v1.position);
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
  const auto topLeftX = (mapWidth - 1) / -2.0f;
  const auto topLeftY = (mapHeight - 1) / 2.0f;

  const auto numOfVertices = mapWidth * mapHeight;
  const auto numOfIndices = (mapWidth - 1) * (mapHeight - 1) * 6;

  mesh->vertices.reserve(numOfVertices);
  mesh->indices.reserve(numOfIndices);

  for (size_t y = 0; y < mapHeight; ++y) {
    for (size_t x = 0; x < mapWidth; ++x) {
      if (x < (mapWidth - 1) && y < (mapHeight - 1)) {
        const auto vertexIndex = int(mesh->vertices.size());
        mesh->indices.push_back(vertexIndex);
        mesh->indices.push_back(vertexIndex + mapWidth + 1);
        mesh->indices.push_back(vertexIndex + mapWidth);
        mesh->indices.push_back(vertexIndex + mapWidth + 1);
        mesh->indices.push_back(vertexIndex);
        mesh->indices.push_back(vertexIndex + 1);
      }

      Vertex vertex = {};
      vertex.position =
          glm::vec4(topLeftX + x, noiseMap[y][x], topLeftY - y,
                    1.0f); // try changing order of x,y array index if you get bug, also remember
                           // to switch y with noisemap value when rotating plane
      vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
      vertex.textureCoordinate = glm::vec2(x / float(mapWidth), y / float(mapHeight));
      mesh->vertices.push_back(vertex);
    }
  }

  assert(mesh->vertices.size() == numOfVertices);
  assert(mesh->indices.size() == numOfIndices);
  assert(numOfIndices % 3 == 0);
}

Mesh generateQuadMesh() {
  Mesh quadMesh = {};

  Vertex v1 = {};
  v1.position = glm::vec4(-2.5f, -2.5f, 0.0f, 1.0f);
  v1.textureCoordinate = glm::vec2(0.0f, 0.0f);

  Vertex v2 = {};
  v2.position = glm::vec4(2.5f, -2.5f, 0.0f, 1.0f);
  v2.textureCoordinate = glm::vec2(1.0f, 0.0f);

  Vertex v3 = {};
  v3.position = glm::vec4(-2.5f, 2.5f, 0.0f, 1.0f);
  v3.textureCoordinate = glm::vec2(0.0f, 1.0f);

  Vertex v4 = {};
  v4.position = glm::vec4(2.5f, 2.5f, 0.0f, 1.0f);
  v4.textureCoordinate = glm::vec2(1.0f, 1.0f);

  quadMesh.vertices.reserve(4);
  quadMesh.indices.reserve(6);

  quadMesh.vertices.push_back(v1);
  quadMesh.vertices.push_back(v2);
  quadMesh.vertices.push_back(v3);
  quadMesh.vertices.push_back(v4);

  quadMesh.indices.push_back(0);
  quadMesh.indices.push_back(1);
  quadMesh.indices.push_back(2);
  quadMesh.indices.push_back(3);
  quadMesh.indices.push_back(2);
  quadMesh.indices.push_back(1);

  calculateMeshNormals(&quadMesh);

  quadMesh.modelTransformation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -6.0f));

  return quadMesh;
}

Mesh generateMeshFromHeightMap(const NoiseMap &noiseMap) {
  Mesh terrainMesh = {};
  calculateMeshVertices(&terrainMesh, noiseMap);
  calculateMeshNormals(&terrainMesh);
  terrainMesh.modelTransformation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -100.0f));

  return terrainMesh;
}