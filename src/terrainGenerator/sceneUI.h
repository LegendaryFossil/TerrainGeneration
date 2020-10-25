#pragma once

#include "GL/glew.h"

#include "meshGenerator.h"
#include "sceneShaders.h"
#include "sceneDefs.h"

struct TerrainData;
struct GLFWwindow;

// Scene settings
struct SceneSettings {
  enum class RENDER_MODE { NOISE_MAP, COLOR_MAP, FALLOFF_MAP, MESH, WIREFRAME };
  enum class CONTROL_MODE { LIGHT, CAMERA };

  RENDER_MODE renderMode = RENDER_MODE::MESH;
  CONTROL_MODE controlMode = CONTROL_MODE::CAMERA;

  bool useFalloffMap = false;
  bool showImGuiDemo = false;
};

void initUI(GLFWwindow *window, const std::string &glslVersion);
void destroyUI();
void renderUI();
void handleUIInput(SceneSettings *sceneSettings, TerrainData *terrainData, SceneData::SkyBoxData *skyboxData,
                   MeshIdToMesh *meshIdToMesh);
