#pragma once

#include "lightDefs.h"
#include "terrainDefs.h"
#include "camera.h"
#include "meshGenerator.h"

struct SceneData {
  LightData lightData = {};
  TerrainData terrainData = {};
  Camera fpsCamera = Camera(glm::vec3(214.0f, 58.0f, 615.0f), glm::vec3(1.0f, glm::half_pi<float>(), -1.81f));
  ViewFrustumData viewFrustumData = {};
  MeshIdToMesh meshIdToMesh = {};

  struct FrameBufferObject {
    unsigned int fboHandle;
    unsigned int fboTexture;
    unsigned int rboHandle;

    unsigned int width;
    unsigned int height;
  } frameBufferObject;
};