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
  glUseProgram(skyboxProgramObject);
  {

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxMesh.textureHandles[0]);

    setUniform(skyboxProgramObject, ufModelToWorldMatrixName, skyboxMesh.modelTransformation);
    setUniform(skyboxProgramObject, ufWorldToViewMatrixName, glm::mat4(glm::mat3(viewMatrix)));
    setUniform(skyboxProgramObject, ufViewToClipMatrixName, viewToClipMatrix);

    glDrawElements(GL_TRIANGLES, GLsizei(skyboxMesh.indices.size()), GL_UNSIGNED_INT, (void *)0);
  }
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
  const auto &skyboxMesh = sceneData.meshIdToMesh.at(kSkyboxMeshId);

  glBindVertexArray(terrainMesh.vaoHandle);
  const auto terrainGeneratorProgramObject = sceneProgramObjects.at(kTerrainGeneratorProgramObjectName);
  glUseProgram(terrainGeneratorProgramObject);
  {
    glViewport(0, 0, frameBufferWidth, frameBufferHeight);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[0]); // Height map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[1]); // Color map

    setUniform(terrainGeneratorProgramObject, ufModelToWorldMatrixName, terrainMesh.modelTransformation);
    setUniform(terrainGeneratorProgramObject, ufWorldToViewMatrixName, viewMatrix);
    setUniform(terrainGeneratorProgramObject, ufNormalMatrix,
               glm::transpose(glm::inverse(glm::mat3(viewMatrix * terrainMesh.modelTransformation))));
    setUniform(terrainGeneratorProgramObject, ufViewToClipMatrixName, viewToClipMatrix);
    setUniform(terrainGeneratorProgramObject, ufWorldLightName, sceneData.lightData.worldLightPosition);
    setUniform(terrainGeneratorProgramObject, ufViewportSizeName,
               glm::vec2(frameBufferWidth, frameBufferHeight));
    setUniform(terrainGeneratorProgramObject, ufWaterDistortionMoveFactorName,
               sceneData.terrainData.waterDistortionMoveFactor);
    setUniform(terrainGeneratorProgramObject, ufTerrainGridPointSpacingName,
               sceneData.terrainData.gridPointSpacing);
    setUniform(terrainGeneratorProgramObject, ufHeightMultiplierName, sceneData.terrainData.heightMultiplier);
    setUniform(terrainGeneratorProgramObject, ufPixelsPerTriangleName,
               sceneData.terrainData.pixelsPerTriangle);

    glDrawElements(GL_PATCHES, GLsizei(terrainMesh.indices.size()), GL_UNSIGNED_INT, (void *)0);
  }
  glUseProgram(0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // Skybox
  renderSkybox(skyboxMesh, viewMatrix, viewToClipMatrix, sceneProgramObjects.at(kSkyboxProgramObjectName));
}

static void renderMaps(const Mesh &terrainMesh, const glm::mat4 &viewMatrix,
                       const glm::mat4 &viewToClipMatrix, const GLuint terrainGeneratorDebugProgramObject) {
  glBindVertexArray(terrainMesh.vaoHandle);
  glUseProgram(terrainGeneratorDebugProgramObject);
  {
    setUniform(terrainGeneratorDebugProgramObject, ufModelToWorldMatrixName, terrainMesh.modelTransformation);
    setUniform(terrainGeneratorDebugProgramObject, ufWorldToViewMatrixName, viewMatrix);
    setUniform(terrainGeneratorDebugProgramObject, ufNormalMatrix,
               glm::transpose(glm::inverse(glm::mat3(viewMatrix * terrainMesh.modelTransformation))));
    setUniform(terrainGeneratorDebugProgramObject, ufViewToClipMatrixName, viewToClipMatrix);

    glDrawElements(GL_PATCHES, GLsizei(terrainMesh.indices.size()), GL_UNSIGNED_INT, (void *)0);
  }
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

void renderQuad(const Mesh &quadMesh, const GLuint fboTexture, const glm::mat4 &viewMatrix,
                const glm::mat4 &viewToClipMatrix, const GLuint quadProgramObject) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, fboTexture);

  glBindVertexArray(quadMesh.vaoHandle);
  glUseProgram(quadProgramObject);
  {
    setUniform(quadProgramObject, ufModelToWorldMatrixName, quadMesh.modelTransformation);

    setUniform(quadProgramObject, ufWorldToViewMatrixName, viewMatrix);

    setUniform(quadProgramObject, ufViewToClipMatrixName, viewToClipMatrix);

    glDrawElements(GL_TRIANGLES, GLsizei(quadMesh.indices.size()), GL_UNSIGNED_INT, (void *)0);
  }
  glUseProgram(0);
}

void renderWater(const Mesh &waterMesh, const GLuint fboTexture, const glm::mat4 &viewMatrix,
                 const glm::mat4 &viewToClipMatrix, const GLuint waterProgramObject) {
  glBindVertexArray(waterMesh.vaoHandle);
  glUseProgram(waterProgramObject);
  {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fboTexture); // Scene texture

    setUniform(waterProgramObject, ufModelToWorldMatrixName, waterMesh.modelTransformation);
    setUniform(waterProgramObject, ufWorldToViewMatrixName, viewMatrix);
    setUniform(waterProgramObject, ufViewToClipMatrixName, viewToClipMatrix);

    glDrawElements(GL_TRIANGLES, GLsizei(waterMesh.indices.size()), GL_UNSIGNED_INT, (void *)0);
  }
  glUseProgram(0);
}

void renderTerrainReflectionTexture(const SceneData &sceneData, const glm::mat4 &viewMatrix,
                                    const glm::mat4 &viewToClipMatrix,
                                    const SceneProgramObjects &sceneProgramObjects) {
  glBindFramebuffer(GL_FRAMEBUFFER, sceneData.frameBufferObject.fboHandle);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_CLIP_DISTANCE1);

  // Render to texture
  renderSceneImpl(sceneData, sceneData.frameBufferObject.width, sceneData.frameBufferObject.height,
                  viewMatrix, viewToClipMatrix, false, sceneProgramObjects);

  glDisable(GL_CLIP_DISTANCE1);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderTerrain(const WindowData &windowData, const SceneData &sceneData, const glm::mat4 &viewMatrix,
                   const glm::mat4 &viewToClipMatrix, const bool isWireFrame,
                   const SceneProgramObjects &sceneProgramObjects) {
  const auto &terrainMesh = sceneData.meshIdToMesh.at(kTerrainMeshId);

  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, sceneData.frameBufferObject.fboTexture);
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[3]); // Dudv map

  renderSceneImpl(sceneData, windowData.width, windowData.height, viewMatrix, viewToClipMatrix, isWireFrame,
                  sceneProgramObjects);
}