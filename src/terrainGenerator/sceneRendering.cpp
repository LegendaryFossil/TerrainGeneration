#include "sceneRendering.h"

#include "lightDefs.h"
#include "sceneDefs.h"
#include "shaderLoader.h"
#include "uniformDefs.h"
#include "windowDefs.h"

static void renderSkybox(const Mesh &skyboxMesh, const glm::mat4 &viewMatrix,
                         const glm::mat4 &viewToClipMatrix, const GLuint skyboxProgramObject) {
  glDepthFunc(GL_LEQUAL);

  glBindVertexArray(skyboxMesh.vaoHandle);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxMesh.textureHandles[0]);

  setUniform(skyboxProgramObject, ufModelToWorldMatrixName, skyboxMesh.modelTransformation);
  setUniform(skyboxProgramObject, ufWorldToViewMatrixName, glm::mat4(glm::mat3(viewMatrix)));
  setUniform(skyboxProgramObject, ufViewToClipMatrixName, viewToClipMatrix);

  validateProgramObject(skyboxProgramObject);
  glUseProgram(skyboxProgramObject);
  glDrawElements(GL_TRIANGLES, GLsizei(skyboxMesh.indices.size()), GL_UNSIGNED_INT, (void *)0);
  glUseProgram(0);

  glDepthFunc(GL_LESS);
}

static void renderSceneImpl(const SceneData &sceneData, const unsigned int frameBufferWidth,
                            const unsigned int frameBufferHeight, const glm::mat4 &viewMatrix,
                            const glm::mat4 &viewToClipMatrix, const bool isWireFrame,
                            const SceneProgramObjects &sceneProgramObjects) {
  if (isWireFrame) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  const auto &terrainMesh = sceneData.meshIdToMesh.at(kTerrainMeshId);

  glBindVertexArray(terrainMesh.vaoHandle);
  const auto terrainGeneratorProgramObject = sceneProgramObjects.at(kTerrainGeneratorProgramObjectName);

  glViewport(0, 0, frameBufferWidth, frameBufferHeight);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[0]); // Height map
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[1]); // Color map

  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D_ARRAY, terrainMesh.textureHandles[3]);
  setUniform(terrainGeneratorProgramObject, ufTerrainTextureScalings,
             sceneData.terrainData.terrainProperties.textureScalings);

  setUniform(terrainGeneratorProgramObject, ufModelToWorldMatrixName, terrainMesh.modelTransformation);
  setUniform(terrainGeneratorProgramObject, ufWorldToViewMatrixName, viewMatrix);
  setUniform(terrainGeneratorProgramObject, ufNormalMatrix,
             glm::transpose(glm::inverse(glm::mat3(viewMatrix * terrainMesh.modelTransformation))));
  setUniform(terrainGeneratorProgramObject, ufViewToClipMatrixName, viewToClipMatrix);
  setUniform(terrainGeneratorProgramObject, ufViewportSizeName,
             glm::vec2(frameBufferWidth, frameBufferHeight));
  setUniform(terrainGeneratorProgramObject, ufTerrainGridPointSpacingName,
             sceneData.terrainData.gridPointSpacing);
  setUniform(terrainGeneratorProgramObject, ufHeightMultiplierName, sceneData.terrainData.heightMultiplier);
  setUniform(terrainGeneratorProgramObject, ufPixelsPerTriangleName, sceneData.terrainData.pixelsPerTriangle);
  setUniform(terrainGeneratorProgramObject, ufTerrainColors, sceneData.terrainData.terrainProperties.colors);
  setUniform(terrainGeneratorProgramObject, ufTerrainColorStrengths,
             sceneData.terrainData.terrainProperties.colorStrengths);
  setUniform(terrainGeneratorProgramObject, ufTerrainHeights,
             sceneData.terrainData.terrainProperties.heights);
  setUniform(terrainGeneratorProgramObject, ufTerrainBlends, sceneData.terrainData.terrainProperties.blends);

  setUniform(terrainGeneratorProgramObject, ufWorldLightPositionsName, sceneData.lightData.positions);
  setUniform(terrainGeneratorProgramObject, ufLightColorsName, sceneData.lightData.colors);

  validateProgramObject(terrainGeneratorProgramObject);
  glUseProgram(terrainGeneratorProgramObject);
  glDrawElements(GL_PATCHES, GLsizei(terrainMesh.indices.size()), GL_UNSIGNED_INT, (void *)0);
  glUseProgram(0);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // Skybox
  const auto skyboxViewMatrix =
      glm::rotate(viewMatrix, glm::radians(sceneData.skyboxData.skyboxRotation), glm::vec3(0.0f, 1.0f, 0.0f));
  renderSkybox(sceneData.meshIdToMesh.at(kSkyboxMeshId), skyboxViewMatrix, viewToClipMatrix,
               sceneProgramObjects.at(kSkyboxProgramObjectName));
}

static void renderMaps(const Mesh &terrainMesh, const glm::mat4 &viewMatrix,
                       const glm::mat4 &viewToClipMatrix, const GLuint terrainGeneratorDebugProgramObject) {
  glBindVertexArray(terrainMesh.vaoHandle);

  setUniform(terrainGeneratorDebugProgramObject, ufModelToWorldMatrixName, terrainMesh.modelTransformation);
  setUniform(terrainGeneratorDebugProgramObject, ufWorldToViewMatrixName, viewMatrix);
  setUniform(terrainGeneratorDebugProgramObject, ufNormalMatrix,
             glm::transpose(glm::inverse(glm::mat3(viewMatrix * terrainMesh.modelTransformation))));
  setUniform(terrainGeneratorDebugProgramObject, ufViewToClipMatrixName, viewToClipMatrix);

  validateProgramObject(terrainGeneratorDebugProgramObject);
  glUseProgram(terrainGeneratorDebugProgramObject);
  glDrawElements(GL_PATCHES, GLsizei(terrainMesh.indices.size()), GL_UNSIGNED_INT, (void *)0);
  glUseProgram(0);
}

void renderNoiseMap(const Mesh &terrainMesh, const glm::mat4 &viewMatrix, const glm::mat4 &viewToClipMatrix,
                    const GLuint terrainGeneratorDebugProgramObject) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[0]);
  setUniform(terrainGeneratorDebugProgramObject, ufDebugSettings, glm::vec3(1.0f, 0.0f, 0.0f));
  renderMaps(terrainMesh, viewMatrix, viewToClipMatrix, terrainGeneratorDebugProgramObject);
}

void renderColorMap(const Mesh &terrainMesh, const glm::mat4 &viewMatrix, const glm::mat4 &viewToClipMatrix,
                    const GLuint terrainGeneratorDebugProgramObject) {
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[1]);
  setUniform(terrainGeneratorDebugProgramObject, ufDebugSettings, glm::vec3(0.0f, 1.0f, 0.0f));
  renderMaps(terrainMesh, viewMatrix, viewToClipMatrix, terrainGeneratorDebugProgramObject);
}

void renderFalloffMap(const Mesh &terrainMesh, const glm::mat4 &viewMatrix, const glm::mat4 &viewToClipMatrix,
                      const GLuint terrainGeneratorDebugProgramObject) {
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[2]);
  setUniform(terrainGeneratorDebugProgramObject, ufDebugSettings, glm::vec3(0.0f, 0.0f, 1.0f));
  renderMaps(terrainMesh, viewMatrix, viewToClipMatrix, terrainGeneratorDebugProgramObject);
}

void renderLight(const std::vector<Mesh> &lightMeshes, const unsigned int frameBufferWidth,
                 const unsigned int frameBufferHeight, const glm::mat4 &viewMatrix,
                 const glm::mat4 &viewToClipMatrix, const GLuint lightProgramObject) {
  glViewport(0, 0, frameBufferWidth, frameBufferHeight);

  setUniform(lightProgramObject, ufWorldToViewMatrixName, viewMatrix);
  setUniform(lightProgramObject, ufViewToClipMatrixName, viewToClipMatrix);

  for (const auto &lightMesh : lightMeshes) {
    glBindVertexArray(lightMesh.vaoHandle);
    setUniform(lightProgramObject, ufModelToWorldMatrixName, lightMesh.modelTransformation);
    validateProgramObject(lightProgramObject);
    glUseProgram(lightProgramObject);
    glDrawElements(GL_TRIANGLES, GLsizei(lightMesh.indices.size()), GL_UNSIGNED_INT, (void *)0);
    glUseProgram(0);
  }
}

void renderWater(const Mesh &waterMesh, const SceneData &sceneData, const unsigned int frameBufferWidth,
                 const unsigned int frameBufferHeight, const glm::mat4 &viewMatrix,
                 const glm::mat4 &viewToClipMatrix, const GLuint waterProgramObject) {
  glViewport(0, 0, frameBufferWidth, frameBufferHeight);

  glBindVertexArray(waterMesh.vaoHandle);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, waterMesh.textureHandles[0]); // Dudv map
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, waterMesh.textureHandles[1]); // Normal map
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, sceneData.frameBufferObject.fboTexture);

  setUniform(waterProgramObject, ufModelToWorldMatrixName, waterMesh.modelTransformation);
  setUniform(waterProgramObject, ufWorldToViewMatrixName, viewMatrix);
  setUniform(waterProgramObject, ufViewToClipMatrixName, viewToClipMatrix);
  setUniform(waterProgramObject, ufNormalMatrix,
             glm::transpose(glm::inverse(glm::mat3(viewMatrix * waterMesh.modelTransformation))));
  setUniform(waterProgramObject, ufWaterDistortionMoveFactorName,
             sceneData.waterData.waterDistortionMoveFactor);
  setUniform(waterProgramObject, ufWaterColor,
             sceneData.terrainData.terrainProperties.colors[0]); // [0] = Water;

  setUniform(waterProgramObject, ufWorldCameraPosition, sceneData.fpsCamera.cameraPosition());

  setUniform(waterProgramObject, ufWorldLightPositionsName, sceneData.lightData.positions);
  setUniform(waterProgramObject, ufLightColorsName, sceneData.lightData.colors);
  setUniform(waterProgramObject, ufSpecularLightColorsName, sceneData.lightData.specularData.colors);
  setUniform(waterProgramObject, ufSpecularLightIntensitiesName,
             sceneData.lightData.specularData.intensities);
  setUniform(waterProgramObject, ufSpecularPowers, sceneData.lightData.specularData.powers);
  setUniform(waterProgramObject, ufReflectionStrength, sceneData.lightData.reflectionStrength);

  validateProgramObject(waterProgramObject);
  glUseProgram(waterProgramObject);
  glDrawElements(GL_TRIANGLES, GLsizei(waterMesh.indices.size()), GL_UNSIGNED_INT, (void *)0);
  glUseProgram(0);
}

void renderTerrainReflectionTexture(const SceneData &sceneData, const glm::mat4 &viewMatrix,
                                    const glm::mat4 &viewToClipMatrix,
                                    const SceneProgramObjects &sceneProgramObjects) {
  glBindFramebuffer(GL_FRAMEBUFFER, sceneData.frameBufferObject.fboHandle);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_CLIP_DISTANCE0);

  // Render to texture
  renderSceneImpl(sceneData, sceneData.frameBufferObject.width, sceneData.frameBufferObject.height,
                  viewMatrix, viewToClipMatrix, false, sceneProgramObjects);

  glDisable(GL_CLIP_DISTANCE0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderTerrain(const WindowData &windowData, const SceneData &sceneData, const glm::mat4 &viewMatrix,
                   const glm::mat4 &viewToClipMatrix, const bool isWireFrame,
                   const SceneProgramObjects &sceneProgramObjects) {
  const auto &terrainMesh = sceneData.meshIdToMesh.at(kTerrainMeshId);

  const auto terrainGeneratorProgramObject = sceneProgramObjects.at(kTerrainGeneratorProgramObjectName);

  renderSceneImpl(sceneData, windowData.width, windowData.height, viewMatrix, viewToClipMatrix, isWireFrame,
                  sceneProgramObjects);
}