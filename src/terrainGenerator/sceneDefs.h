#pragma once

#include "camera.h"
#include "lightDefs.h"
#include "meshGenerator.h"
#include "terrainDefs.h"

struct SceneData {
  LightData lightData = {};
  TerrainData terrainData = {};
  Camera fpsCamera = Camera(glm::vec3(188.0f, 83.0f, 655.0f), glm::vec3(1.0f, glm::half_pi<float>(), -1.66f));
  ViewFrustumData viewFrustumData = {};
  MeshIdToMesh meshIdToMesh = {};
  
  // For debugging purposes
  std::vector<Mesh> lightMeshes = {};

  struct SkyBoxData {
    float skyboxRotation = 0.0f;
    float skyboxRotationSpeed = 1.5f;
  } skyboxData;

  struct WaterData {
    float waterDistortionMoveFactor = 0.0f;
    float waterDistortionSpeed = 0.05f;
  } waterData;

  struct FrameBufferObject {
    unsigned int fboHandle;
    unsigned int fboTexture;
    unsigned int rboHandle;

    unsigned int width;
    unsigned int height;
  } frameBufferObject;
};