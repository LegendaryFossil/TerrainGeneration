#pragma once

#include "GL/glew.h"
#include "glm\glm.hpp"
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

void renderTerrain(SceneData *sceneData, const WindowData &windowData, const double frameTime,
                   const glm::mat4 &viewToClipMatrix, const bool isWireFrame,
                   const SceneProgramObjects &sceneProgramObjects);

void renderQuad(const Mesh &quadMesh, const GLuint fboTexture, const glm::mat4 &viewMatrix,
                const glm::mat4 &viewToClipMatrix, const GLuint quadProgramObject);

void renderWater(const Mesh &waterMesh, const GLuint fboTexture, const glm::mat4 &viewMatrix,
                 const glm::mat4 &viewToClipMatrix, const GLuint waterProgramObject);

void renderTerrainReflectionTexture(const SceneData &sceneData, const glm::mat4 &viewMatrix,
                                        const glm::mat4 &viewToClipMatrix,
                                        const SceneProgramObjects &sceneProgramObjects);

void renderTerrain(const WindowData &windowData, const SceneData &sceneData, const glm::mat4 &viewMatrix,
                       const glm::mat4 &viewToClipMatrix, const bool isWireFrame,
                       const SceneProgramObjects &sceneProgramObjects);