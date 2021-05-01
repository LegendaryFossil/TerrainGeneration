#include "sceneShaders.h"

#include <vector>

#include "camera.h"
#include "sceneDefs.h"
#include "shaderLoader.h"
#include "terrainDefs.h"
#include "uniformDefs.h"
#include "windowDefs.h"

SceneProgramObjects initSceneShaders(const WindowData &windowData, const SceneData &sceneData) {
  SceneProgramObjects programObjects;

  // Skybox shader
  std::vector<GLuint> skyboxShaderObjects;
  skyboxShaderObjects.push_back(compileShader("skybox.vert", GL_VERTEX_SHADER));
  skyboxShaderObjects.push_back(compileShader("skybox.frag", GL_FRAGMENT_SHADER));
  const auto skyboxProgramObject =
      programObjects.emplace(kSkyboxProgramObjectName, createProgramObject(skyboxShaderObjects))
          .first->second;
  setUniform(skyboxProgramObject, ufSkyboxTextureName, 0);

  // Light shader
  std::vector<GLuint> lightShaderObjects;
  lightShaderObjects.push_back(compileShader("light.vert", GL_VERTEX_SHADER));
  lightShaderObjects.push_back(compileShader("light.frag", GL_FRAGMENT_SHADER));
  const auto lightProgramObject =
      programObjects.emplace(kLightShaderProgramObjectName, createProgramObject(lightShaderObjects))
          .first->second;
  setUniform(lightProgramObject, ufSceneTextureName, 0);

  // Terrain generator shader
  std::vector<GLuint> terrainGeneratorShaderObjects;
  terrainGeneratorShaderObjects.push_back(compileShader("terrain.vert", GL_VERTEX_SHADER));
  terrainGeneratorShaderObjects.push_back(compileShader("terrain.tesc", GL_TESS_CONTROL_SHADER));
  terrainGeneratorShaderObjects.push_back(compileShader("terrain.tese", GL_TESS_EVALUATION_SHADER));
  terrainGeneratorShaderObjects.push_back(compileShader("terrain.frag", GL_FRAGMENT_SHADER));
  const auto terrainGeneratorProgramObject =
      programObjects
          .emplace(kTerrainGeneratorProgramObjectName, createProgramObject(terrainGeneratorShaderObjects))
          .first->second;

  setUniform(terrainGeneratorProgramObject, ufHeightMapTextureName, 0);
  setUniform(terrainGeneratorProgramObject, ufFalloffMapTextureName, 1);

  setUniform(terrainGeneratorProgramObject, ufPatchSizeName, kPatchSize);
  setUniform(terrainGeneratorProgramObject, ufTerrainGridPointSpacingName,
             sceneData.terrainData.gridPointSpacing);
  setUniform(terrainGeneratorProgramObject, ufHeightMultiplierName, sceneData.terrainData.heightMultiplier);
  setUniform(terrainGeneratorProgramObject, ufPixelsPerTriangleName, sceneData.terrainData.pixelsPerTriangle);

  setUniform(terrainGeneratorProgramObject, ufViewportSizeName,
             glm::vec2(windowData.width, windowData.height));
  setUniform(terrainGeneratorProgramObject, ufHorizontalClipPlane, glm ::vec4(0.0f, 1.0f, 0.0f, -0.35f));

  setUniform(terrainGeneratorProgramObject, ufLightCount, sceneData.lightData.lightCount);
  setUniform(terrainGeneratorProgramObject, ufLightColorsName, sceneData.lightData.colors);
  setUniform(terrainGeneratorProgramObject, ufWorldLightPositionsName, sceneData.lightData.positions);

  setUniform(terrainGeneratorProgramObject, ufAmbientConstantName, ambientConstant);

  setUniform(terrainGeneratorProgramObject, ufTerrainTextures, 2);
  setUniform(terrainGeneratorProgramObject, ufTerrainCount, sceneData.terrainData.terrainCount);
  setUniform(terrainGeneratorProgramObject, ufTerrainTextureScalings,
             sceneData.terrainData.terrainProperties.textureScalings);
  setUniform(terrainGeneratorProgramObject, ufTerrainColors, sceneData.terrainData.terrainProperties.colors);
  setUniform(terrainGeneratorProgramObject, ufTerrainColorStrengths,
             sceneData.terrainData.terrainProperties.colorStrengths);
  setUniform(terrainGeneratorProgramObject, ufTerrainHeights,
             sceneData.terrainData.terrainProperties.heights);
  setUniform(terrainGeneratorProgramObject, ufTerrainBlends, sceneData.terrainData.terrainProperties.blends);

  // Terrain noise/falloff map shader
  std::vector<GLuint> terrainGeneratorDebugShaderObjects;
  terrainGeneratorDebugShaderObjects.push_back(compileShader("terrain.vert", GL_VERTEX_SHADER));
  terrainGeneratorDebugShaderObjects.push_back(compileShader("terrainDebug.tesc", GL_TESS_CONTROL_SHADER));
  terrainGeneratorDebugShaderObjects.push_back(compileShader("terrainDebug.tese", GL_TESS_EVALUATION_SHADER));
  terrainGeneratorDebugShaderObjects.push_back(compileShader("terrainDebug.frag", GL_FRAGMENT_SHADER));
  const auto terrainGeneratorDebugProgramObject =
      programObjects
          .emplace(kTerrainGeneratorDebugProgramObjectName,
                   createProgramObject(terrainGeneratorDebugShaderObjects))
          .first->second;

  setUniform(terrainGeneratorDebugProgramObject, ufDebugSettings, glm::vec3(1.0f, 0.0f, 0.0f));
  setUniform(terrainGeneratorDebugProgramObject, ufHeightMapTextureName, 0);
  setUniform(terrainGeneratorDebugProgramObject, ufFalloffMapTextureName, 1);

  // Water shader
  std::vector<GLuint> waterShaderObjects;
  waterShaderObjects.push_back(compileShader("water.vert", GL_VERTEX_SHADER));
  waterShaderObjects.push_back(compileShader("water.frag", GL_FRAGMENT_SHADER));
  const auto waterProgramObject =
      programObjects.emplace(kWaterProgramObjectName, createProgramObject(waterShaderObjects)).first->second;

  setUniform(waterProgramObject, ufTerrainGridPointSpacingName, sceneData.terrainData.gridPointSpacing);

  setUniform(waterProgramObject, ufDuDvTextureName, 0);
  setUniform(waterProgramObject, ufNormalMapTextureName, 1);
  setUniform(waterProgramObject, ufSceneTextureName, 2);

  setUniform(waterProgramObject, ufWaterDistortionMoveFactorName,
             sceneData.waterData.waterDistortionMoveFactor);
  setUniform(waterProgramObject, ufWaterColor,
             sceneData.terrainData.terrainProperties.colors[0]); // [0] = Water

  setUniform(waterProgramObject, ufLightCount, sceneData.lightData.lightCount);
  setUniform(waterProgramObject, ufWorldLightPositionsName, sceneData.lightData.positions);
  setUniform(waterProgramObject, ufLightColorsName, sceneData.lightData.colors);
  setUniform(waterProgramObject, ufSpecularLightColorsName, sceneData.lightData.specularData.colors);
  setUniform(waterProgramObject, ufSpecularLightIntensitiesName,
             sceneData.lightData.specularData.intensities);
  setUniform(waterProgramObject, ufSpecularPowers, sceneData.lightData.specularData.powers);
  setUniform(waterProgramObject, ufReflectionStrength, sceneData.lightData.reflectionStrength);
  setUniform(waterProgramObject, ufWorldCameraPosition, sceneData.fpsCamera.cameraPosition());

  // Water debug shader
  std::vector<GLuint> waterDebugShaderObjects;
  waterDebugShaderObjects.push_back(compileShader("waterDebug.vert", GL_VERTEX_SHADER));
  waterDebugShaderObjects.push_back(compileShader("waterDebug.frag", GL_FRAGMENT_SHADER));
  const auto waterDebugProgramObject =
      programObjects.emplace(kWaterDebugProgramObjectName, createProgramObject(waterDebugShaderObjects)).first->second;

  setUniform(waterDebugProgramObject, ufWaterColor,
             sceneData.terrainData.terrainProperties.colors[0]); // [0] = Water

  return programObjects;
}