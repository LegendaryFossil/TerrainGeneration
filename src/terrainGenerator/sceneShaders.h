#pragma once

#include <string>
#include <unordered_map>

#include "GL/glew.h"

struct WindowData;
struct SceneData;

constexpr auto kSkyboxProgramObjectName = "skybox";
constexpr auto kLightShaderProgramObjectName = "light";
constexpr auto kTerrainGeneratorProgramObjectName = "terrainGenerator";
constexpr auto kTerrainGeneratorDebugProgramObjectName = "terrainGeneratorDebug";
constexpr auto kWaterProgramObjectName = "water";
constexpr auto kWaterDebugProgramObjectName = "waterDebug";

using SceneProgramObjects = std::unordered_map<std::string, GLuint>;

SceneProgramObjects initSceneShaders(const WindowData &windowData, const SceneData &sceneData);