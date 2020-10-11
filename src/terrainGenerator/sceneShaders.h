#pragma once

#include <string>
#include <unordered_map>

#include "GL/glew.h"

struct WindowData;
struct SceneData;

constexpr auto kTerrainGeneratorProgramObjectName = "terrainGenerator";
constexpr auto kTerrainGeneratorDebugProgramObjectName = "terrainGeneratorDebug";
constexpr auto kWaterShaderProgramObjectName = "waterShader";
constexpr auto kQuadShaderProgramObjectName = "quad";

using SceneProgramObjects = std::unordered_map<std::string, GLuint>;

SceneProgramObjects initSceneShaders(const WindowData &windowData, const SceneData &sceneData);