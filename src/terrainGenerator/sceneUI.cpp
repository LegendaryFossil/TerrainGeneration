#include "sceneUi.h"

#include "glm\gtc\type_ptr.hpp"
#include "imGui/imgui.h"
#include "imGui/imgui_impl_glfw.h"
#include "imGui/imgui_impl_opengl3.h"
#include "lightDefs.h"
#include "meshGenerator.h"
#include "sceneShaders.h"
#include "shaderLoader.h"
#include "terrainDefs.h"
#include "uniformDefs.h"
#include "utils.h"
#include <string>

const std::string fontPath(getExePath() + "/resources/fonts/");

void initUI(GLFWwindow *window, const std::string &glslVersion) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glslVersion.c_str());

  const auto font = std::string(fontPath + "Montserrat-Regular.ttf");
  ImGui::GetIO().Fonts->AddFontFromFileTTF(font.c_str(), 25.0f, nullptr, nullptr);

  ImGui::StyleColorsDark();
}

void destroyUI() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void handleUIInput(SceneSettings *sceneSettings, TerrainData *terrainData, SceneData::WaterData *waterData,
                   LightData *lightData, SceneData::SkyBoxData *skyboxData, MeshIdToMesh *meshIdToMesh) {
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

  ImGui::NewLine();
  ImGui::Text("Scene control");
  if (ImGui::BeginCombo("", sceneSettings->sceneControlNames[int(sceneSettings->controlMode)].c_str())) {
    for (int i = 0; i < sceneSettings->sceneControlNames.size(); ++i) {
      const auto isSelected = int(sceneSettings->controlMode) == i;
      if (ImGui::Selectable(sceneSettings->sceneControlNames[i].c_str(), isSelected)) {
        sceneSettings->controlMode = SceneSettings::CONTROL_MODE(i);
      }

      if (isSelected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }

  ImGui::NewLine();
  ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
  if (ImGui::TreeNode("Light settings")) {
    for (size_t i = 0; i < lightData->lightCount; ++i) {
      ImGui::PushID(int(i));
      const std::string name("Light " + std::to_string(i));
      if (ImGui::TreeNode(name.data())) {
        if (ImGui::ColorEdit3("Light color", glm::value_ptr(lightData->colors[i]),
                              ImGuiColorEditFlags_NoInputs)) {
          // Do nothing, just update the variable
        }
        if (ImGui::ColorEdit3("Specular Color", glm::value_ptr(lightData->specularData.colors[i]),
                              ImGuiColorEditFlags_NoInputs)) {
          // Do nothing, just update the variable
        }
        if (ImGui::SliderFloat("Specular intensity", &lightData->specularData.intensities[i], 0.0f, 1.0f)) {
          // Do nothing, just update the variable
        }
        if (ImGui::SliderFloat("Specular power", &lightData->specularData.powers[i], 2.0f, 2000.0f)) {
          // Do nothing, just update the variable
        }

        ImGui::TreePop();
      }
      ImGui::PopID();
    }

    ImGui::TreePop();
  }

  if (ImGui::SliderFloat("Reflection Strength", &lightData->reflectionStrength, 0.0f, 1.0f)) {
    // Do nothing, just update the variable
  }

  ImGui::NewLine();
  ImGui::Text("Water settings");
  if (ImGui::SliderFloat("Water distortion speed", &waterData->waterDistortionSpeed, 0.0f, 0.1f)) {
    // Do nothing, just update the variable
  }

  if (ImGui::BeginCombo("Water Distortion map", sceneSettings->currentWaterDuDv.c_str())) {
    for (int i = 0; i < sceneSettings->waterDuDvs.size(); ++i) {
      const auto isSelected = sceneSettings->currentWaterDuDv == sceneSettings->waterDuDvs[i];
      if (ImGui::Selectable(sceneSettings->waterDuDvs[i].c_str(), isSelected)) {
        sceneSettings->currentWaterDuDv = sceneSettings->waterDuDvs[i];
        updateTerrainMeshWaterTextures(&meshIdToMesh->at(kTerrainMeshId), sceneSettings->currentWaterDuDv);
      }

      if (isSelected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }

  ImGui::NewLine();
  ImGui::Text("Skybox settings");
  if (ImGui::SliderFloat("Skybox Rotation Speed", &skyboxData->skyboxRotationSpeed, 0.0f, 10.0f)) {
    // Do nothing, just update the variable
  }

  ImGui::NewLine();
  ImGui::Text("Terrain settings");
  ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
  if (ImGui::TreeNode("Noise Map settings")) {
    if (ImGui::SliderInt("Octaves", &terrainData->noiseMapData.octaves, 1, 6) ||
        ImGui::SliderFloat("Persistance", &terrainData->noiseMapData.persistance, 0.0f, 1.0f) ||
        ImGui::SliderFloat("Lacunarity", &terrainData->noiseMapData.lacunarity, 2.0f, 4.0f) ||
        ImGui::SliderInt("Seed", &terrainData->noiseMapData.seed, 1, 100) ||
        ImGui::SliderFloat("Octave offset X", &terrainData->noiseMapData.octaveOffset.x, 0.0f, 2000.0f) ||
        ImGui::SliderFloat("Octave offset Y", &terrainData->noiseMapData.octaveOffset.y, 0.0f, 2000.0f) ||
        ImGui::SliderFloat("Scale", &terrainData->noiseMapData.scale, 1.0f, 10.0f)) {
      updateTerrainMeshTexture(&meshIdToMesh->at(kTerrainMeshId), terrainData->noiseMapData,
                               terrainData->useFalloffMap, terrainData->terrainProperties.colors,
                               terrainData->terrainProperties.heights);
    }

    ImGui::TreePop();
  }

  ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
  if (ImGui::TreeNode("Terrain type settings")) {
    for (size_t i = 0; i < terrainData->terrainCount; ++i) {
      ImGui::PushID(int(i));
      if (ImGui::TreeNode(terrainData->terrainProperties.names[i].data())) {
        if (ImGui::SliderFloat("Height", &terrainData->terrainProperties.heights[i], 0.0f, 1.0f) ||
            ImGui::ColorEdit3("Color", glm::value_ptr(terrainData->terrainProperties.colors[i]),
                              ImGuiColorEditFlags_NoInputs)) {
          updateTerrainMeshTexture(&meshIdToMesh->at(kTerrainMeshId), terrainData->noiseMapData,
                                   terrainData->useFalloffMap, terrainData->terrainProperties.colors,
                                   terrainData->terrainProperties.heights);
        }

        if (ImGui::SliderFloat("Color strength", &terrainData->terrainProperties.colorStrengths[i], 0.0f,
                               1.0f)) {
          // Do nothing, just update the variable
        }

        if (ImGui::SliderFloat("Blend", &terrainData->terrainProperties.blends[i], 0.0f, 1.0f)) {
          // Do nothing, just update the variable
        }

        if (ImGui::SliderFloat("Texture scaling", &terrainData->terrainProperties.textureScalings[i], 20.0f,
                               100.0f)) {
          // Do nothing, just update the variable
        }

        ImGui::TreePop();
      }
      ImGui::PopID();
    }

    ImGui::TreePop();
  }

  if (ImGui::Checkbox("Use falloff map", &terrainData->useFalloffMap)) {
    updateTerrainMeshTexture(&meshIdToMesh->at(kTerrainMeshId), terrainData->noiseMapData,
                             terrainData->useFalloffMap, terrainData->terrainProperties.colors,
                             terrainData->terrainProperties.heights);
  }

  if (ImGui::SliderFloat("Terrain grid spacing", &terrainData->gridPointSpacing, 1.0f, 10.0f)) {
    // Do nothing, just update the variable
  }
  if (ImGui::SliderFloat("Height multiplier", &terrainData->heightMultiplier, 0.0f, 1000.0f)) {
    // Do nothing, just update the variable
  }
  if (ImGui::SliderInt("Pixels per triangle", &terrainData->pixelsPerTriangle, 1, 30)) {
    // Do nothing, just update the variable
  }

  ImGui::NewLine();
  if (ImGui::Button("Reset terrain settings")) {
    sceneSettings->renderMode = SceneSettings::RENDER_MODE::MESH;
    *terrainData = initDefaultTerrainData();
    updateTerrainMeshTexture(&meshIdToMesh->at(kTerrainMeshId), terrainData->noiseMapData,
                             terrainData->useFalloffMap, terrainData->terrainProperties.colors,
                             terrainData->terrainProperties.heights);
  }

  ImGui::End();
}

void renderUI() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}