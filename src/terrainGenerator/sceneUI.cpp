#include "sceneUi.h"

#include "glm\gtc\type_ptr.hpp"
#include "imGui/imgui.h"
#include "imGui/imgui_impl_glfw.h"
#include "imGui/imgui_impl_opengl3.h"
#include "meshGenerator.h"
#include "sceneShaders.h"
#include "shaderLoader.h"
#include "terrainDefs.h"
#include "uniformDefs.h"
#include <string>

void initUI(GLFWwindow *window, const std::string &glslVersion) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glslVersion.c_str());

  ImGui::StyleColorsDark();
}

void destroyUI() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void handleUIInput(SceneSettings *sceneSettings, TerrainData *terrainData, MeshIdToMesh *meshIdToMesh) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  if (sceneSettings->showImGuiDemo)
    ImGui::ShowDemoWindow(&sceneSettings->showImGuiDemo);

  ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

  // View settings
  ImGui::Text("Render mode");
  if (ImGui::Button("Noise Map")) {
    sceneSettings->renderMode = SceneSettings::RENDER_MODE::NOISE_MAP;
  }
  ImGui::SameLine();
  if (ImGui::Button("Color Map")) {
    sceneSettings->renderMode = SceneSettings::RENDER_MODE::COLOR_MAP;
  }
  ImGui::SameLine();
  if (ImGui::Button("Falloff Map")) {
    sceneSettings->renderMode = SceneSettings::RENDER_MODE::FALLOFF_MAP;
  }
  ImGui::SameLine();
  if (ImGui::Button("Mesh")) {
    sceneSettings->renderMode = SceneSettings::RENDER_MODE::MESH;
  }
  ImGui::SameLine();
  if (ImGui::Button("Wireframe Mesh")) {
    sceneSettings->renderMode = SceneSettings::RENDER_MODE::WIREFRAME;
  }

  static int e = 0;
  if (ImGui::RadioButton("Camera control", &e, 0)) {
    sceneSettings->controlMode = SceneSettings::CONTROL_MODE::CAMERA;
  }
  if (ImGui::RadioButton("Light control", &e, 1)) {
    sceneSettings->controlMode = SceneSettings::CONTROL_MODE::LIGHT;
  }

  ImGui::Text("Terrain settings");
  ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
  if (ImGui::TreeNode("Noise Map settings")) {
    if (ImGui::SliderFloat("Scale", &terrainData->noiseMapData.scale, 1.0f, 10.0f) ||
        ImGui::SliderInt("Octaves", &terrainData->noiseMapData.octaves, 1, 6) ||
        ImGui::SliderFloat("Persistance", &terrainData->noiseMapData.persistance, 0.0f, 1.0f) ||
        ImGui::SliderFloat("Lacunarity", &terrainData->noiseMapData.lacunarity, 2.0f, 4.0f) ||
        ImGui::SliderInt("Seed", &terrainData->noiseMapData.seed, 1, 100) ||
        ImGui::SliderFloat("Octave offset X", &terrainData->noiseMapData.octaveOffset.x, 0.0f, 2000.0f) ||
        ImGui::SliderFloat("Octave offset Y", &terrainData->noiseMapData.octaveOffset.y, 0.0f, 2000.0f)) {
      updateTerrainMeshTexture(&meshIdToMesh->at(kTerrainMeshId), terrainData->noiseMapData,
                               terrainData->useFalloffMap, terrainData->terrainTypes);
    }

    ImGui::TreePop();
  }

  if (ImGui::Checkbox("Use falloff map", &terrainData->useFalloffMap)) {
    updateTerrainMeshTexture(&meshIdToMesh->at(kTerrainMeshId), terrainData->noiseMapData,
                             terrainData->useFalloffMap, terrainData->terrainTypes);
  }

  if (ImGui::SliderFloat("Terrain grid spacing", &terrainData->gridPointSpacing, 1.0f, 10.0f)) {
    // Do nothing, just update the variable
  }
  if (ImGui::SliderFloat("Height multiplier", &terrainData->heightMultiplier, 0.0f, 100.0f)) {
    // Do nothing, just update the variable
  }

  if (ImGui::SliderFloat("Water distortion speed", &terrainData->waterDistortionSpeed, 0.0f, 1.0f)) {
    // Do nothing, just update the variable
  }

  if (ImGui::SliderInt("Pixels per triangle", &terrainData->pixelsPerTriangle, 1, 30)) {
    // Do nothing, just update the variable
  }

  ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
  if (ImGui::TreeNode("Terrain type settings")) {
    for (size_t i = 0; i < terrainData->terrainTypes.size(); i++) {
      ImGui::PushID(int(i));
      if (ImGui::TreeNode(terrainData->terrainTypes[i].name.data())) {
        if (ImGui::SliderFloat("Height", &terrainData->terrainTypes[i].height, 0.0f, 1.0f) ||
            ImGui::ColorEdit3("Color", glm::value_ptr(terrainData->terrainTypes[i].color),
                              ImGuiColorEditFlags_NoInputs)) {
          updateTerrainMeshTexture(&meshIdToMesh->at(kTerrainMeshId), terrainData->noiseMapData,
                                   terrainData->useFalloffMap, terrainData->terrainTypes);
        }
        ImGui::TreePop();
      }
      ImGui::PopID();
    }

    ImGui::TreePop();
  }

  ImGui::End();
}

void renderUI() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}