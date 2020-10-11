#pragma once

#include "GL/glew.h"

#include "meshGenerator.h"
#include "sceneShaders.h"

struct TerrainData;
struct GLFWwindow;

// Scene settings
struct SceneSettings {
  enum class RENDER_MODE { NOISE_MAP, COLOR_MAP, FALLOFF_MAP, MESH, WIREFRAME };
  enum class CONTROL_MODE { LIGHT, CAMERA };

  RENDER_MODE renderMode = RENDER_MODE::MESH;
  CONTROL_MODE controlMode = CONTROL_MODE::CAMERA;

  GLuint selectedProgramObject = GLuint(-1);
  bool useFalloffMap = false;
  bool showImGuiDemo = false;
};

void initUI(GLFWwindow *window, const std::string &glslVersion);
void destroyUI();
void handleSceneUiInput(SceneSettings *sceneSettings, TerrainData *terrainData, MeshIdToMesh *meshIdToMesh,
                        const SceneProgramObjects &sceneProgramObjects);
