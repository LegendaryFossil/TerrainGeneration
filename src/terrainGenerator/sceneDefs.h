#pragma once

#include "camera.h"
#include "lightDefs.h"
#include "meshGenerator.h"
#include "terrainDefs.h"

struct SceneData {
  LightData lightData = {};
  TerrainData terrainData = {};
  Camera fpsCamera = Camera(glm::vec3(214.0f, 58.0f, 615.0f), glm::vec3(1.0f, glm::half_pi<float>(), -1.81f));
  ViewFrustumData viewFrustumData = {};
  MeshIdToMesh meshIdToMesh = {};

  struct SkyBoxData {
    float skyboxRotation = 0.0f;
    float skyboxRotationSpeed = 1.5f;
    float debugScale = 0.040f;
  } skyboxData;


  struct FrameBufferObject {
    unsigned int fboHandle;
    unsigned int fboTexture;
    unsigned int rboHandle;

    unsigned int width;
    unsigned int height;
  } frameBufferObject;
};