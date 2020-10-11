#include "sceneRendering.h"

#include "lightDefs.h"
#include "sceneDefs.h"
#include "shaderLoader.h"
#include "uniformDefs.h"
#include "windowDefs.h"

static void renderTerrainImpl(const Mesh &terrainMesh, const LightData &lightData,
                              const glm::mat4 &viewMatrix, const glm::mat4 &viewToClipMatrix,
                              const GLuint terrainGeneratorProgramObject) {
  glBindVertexArray(terrainMesh.vaoHandle);
  glUseProgram(terrainGeneratorProgramObject);
  {
    setUniform(terrainGeneratorProgramObject, ufModelToWorldMatrixName, terrainMesh.modelTransformation);
    setUniform(terrainGeneratorProgramObject, ufWorldToViewMatrixName, viewMatrix);
    setUniform(terrainGeneratorProgramObject, ufNormalMatrix,
               glm::transpose(glm::inverse(glm::mat3(viewMatrix * terrainMesh.modelTransformation))));
    setUniform(terrainGeneratorProgramObject, ufViewToClipMatrixName, viewToClipMatrix);
    setUniform(terrainGeneratorProgramObject, ufWorldLightName, lightData.worldLightPosition);

    glDrawElements(GL_PATCHES, GLsizei(terrainMesh.indices.size()), GL_UNSIGNED_INT, (void *)0);
  }
  glUseProgram(0);
}

static void renderReflectionTexture(const Mesh &terrainMesh, const LightData &lightData,
                                    const GLuint fboHandle, Camera *camera, const glm::mat4 &viewToClipMatrix,
                                    const GLuint terrainGeneratorProgramObject) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[0]); // Height map
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[1]); // Color map

  glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_CLIP_DISTANCE1);

  // Move camera under water (twice distance from water to camera)
  // This assumes no model transformation affects the terrain (i.e. identity matrix transformation)
  // and that water height is always at y = 0.0
  const auto waterPositionY = 0.0f;
  auto cameraPosition = camera->cameraPosition();
  const auto distanceToMoveY = 2.0f * (camera->cameraPosition().y - waterPositionY);
  cameraPosition.y -= distanceToMoveY;
  camera->setCameraPosition(cameraPosition);
  camera->invertPitch();

  // Render to texture
  renderTerrainImpl(terrainMesh, lightData, camera->createViewMatrix(), viewToClipMatrix,
                    terrainGeneratorProgramObject);

  // Change camera back to original state
  cameraPosition.y += distanceToMoveY;
  camera->setCameraPosition(cameraPosition);
  camera->invertPitch();

  glDisable(GL_CLIP_DISTANCE1);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

void renderTerrain(SceneData *sceneData, const double frameTime, const glm::mat4 &viewToClipMatrix,
                   const bool isWireFrame, const GLuint terrainGeneratorProgramObject) {
  const auto &terrainMesh = sceneData->meshIdToMesh.at(kTerrainMeshId);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[0]); // Height map
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[1]); // Color map

  // Render reflection of scene to texture
  renderReflectionTexture(terrainMesh, sceneData->lightData, sceneData->sceneFrameBuffer.fboHandle,
                          &sceneData->fpsCamera, viewToClipMatrix, terrainGeneratorProgramObject);

  // Render scene
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, sceneData->sceneFrameBuffer.fboTexture);
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, terrainMesh.textureHandles[3]);

  sceneData->terrainData.waterDistortionMoveFactor +=
      sceneData->terrainData.waterDistortionSpeed * float(frameTime);
  sceneData->terrainData.waterDistortionMoveFactor =
      std::fmod(sceneData->terrainData.waterDistortionMoveFactor, 1.0f);

  setUniform(terrainGeneratorProgramObject, ufWaterDistortionMoveFactorName,
             sceneData->terrainData.waterDistortionMoveFactor);

  if (isWireFrame) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  renderTerrainImpl(terrainMesh, sceneData->lightData, sceneData->fpsCamera.createViewMatrix(),
                    viewToClipMatrix, terrainGeneratorProgramObject);
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