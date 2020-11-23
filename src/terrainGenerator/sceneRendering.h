#pragma once

#include "GL/glew.h"
#include "glm\glm.hpp"
//#include "sceneDefs.h"
#include "sceneShaders.h"

struct Mesh;
struct WindowData;
struct SceneData;

void renderNoiseMap(const Mesh &terrainMesh, const glm::mat4 &viewMatrix, const glm::mat4 &viewToClipMatrix,
                    const GLuint terrainGeneratorDebugProgramObject);

void renderColorMap(const Mesh &terrainMesh, const glm::mat4 &viewMatrix, const glm::mat4 &viewToClipMatrix,
                    const GLuint terrainGeneratorDebugProgramObject);

void renderFalloffMap(const Mesh &terrainMesh, const glm::mat4 &viewMatrix, const glm::mat4 &viewToClipMatrix,
                      const GLuint terrainGeneratorDebugProgramObject);

void renderLight(const std::vector<Mesh> &lightMeshes, const glm::mat4 &viewMatrix, const glm::mat4 &viewToClipMatrix,
                 const GLuint lightProgramObject);

void renderWater(const Mesh &waterMesh, const SceneData &sceneData, const glm::mat4 &viewMatrix,
                 const glm::mat4 &viewToClipMatrix, const GLuint waterProgramObject);

void renderTerrainReflectionTexture(const SceneData &sceneData, const glm::mat4 &viewMatrix,
                                    const glm::mat4 &viewToClipMatrix,
                                    const SceneProgramObjects &sceneProgramObjects);

void renderTerrain(const WindowData &windowData, const SceneData &sceneData, const glm::mat4 &viewMatrix,
                   const glm::mat4 &viewToClipMatrix, const bool isWireFrame,
                   const SceneProgramObjects &sceneProgramObjects);