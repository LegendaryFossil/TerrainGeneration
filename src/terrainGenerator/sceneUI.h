#pragma once

#include "GL/glew.h"
#include "meshGenerator.h"
#include "sceneDefs.h"
#include "sceneShaders.h"
#include <array>
#include <string>

struct TerrainData;
struct LightData;
struct GLFWwindow;

// Scene settings
struct SceneSettings {
  enum class RENDER_MODE { NOISE_MAP, COLOR_MAP, FALLOFF_MAP, MESH, WIREFRAME };
  enum class CONTROL_MODE { LIGHT_1, LIGHT_2, CAMERA };
  std::array<std::string, 3> sceneControlNames{"Light 1", "Light 2", "Camera"};

  std::array<std::string, 7> waterDuDvs{"1", "2", "3", "4", "5", "6", "7"};

  RENDER_MODE renderMode = RENDER_MODE::MESH;

  CONTROL_MODE controlMode = CONTROL_MODE::CAMERA;
  std::string currentWaterDuDv = waterDuDvs[0];

  bool useFalloffMap = false;
  bool showSettings = false;
  bool showImGuiDemo = false;
};

void initUI(GLFWwindow *window, const std::string &glslVersion);
void destroyUI();
void renderUI();
void handleUIInput(SceneSettings *sceneSettings, TerrainData *terrainData, SceneData::WaterData *waterData,
                   LightData *lightData, SceneData::SkyBoxData *skyboxData, MeshIdToMesh *meshIdToMesh);
