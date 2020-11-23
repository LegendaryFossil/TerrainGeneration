#pragma once

#include "camera.h"
#include "lightDefs.h"
#include "meshGenerator.h"
#include "terrainDefs.h"

struct SceneData {
  LightData lightData = {};
  TerrainData terrainData = {};
  Camera fpsCamera =
      Camera(glm::vec3(207.0f, 75.0f, 640.0f), glm::vec3(1.0f, 1.57f, -1.7f));
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