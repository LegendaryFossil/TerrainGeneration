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
      programObjects.emplace(kSkyboxProgramObjectName, createProgramObject(skyboxShaderObjects)).first->second;
  setUniform(skyboxProgramObject, ufSkyboxTextureName, 0);

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

  setUniform(terrainGeneratorProgramObject, ufAmbientConstantName, sceneData.lightData.ambientConstant);
  setUniform(terrainGeneratorProgramObject, ufHeightMapTextureName, 0);
  setUniform(terrainGeneratorProgramObject, ufColorMapTextureName, 1);
  setUniform(terrainGeneratorProgramObject, ufFalloffMapTextureName, 2);
  setUniform(terrainGeneratorProgramObject, ufSceneTextureName, 3);
  setUniform(terrainGeneratorProgramObject, ufDuDvTextureName, 4);
  setUniform(terrainGeneratorProgramObject, ufPatchSizeName, PATCH_SIZE);
  setUniform(terrainGeneratorProgramObject, ufTerrainGridPointSpacingName,
             sceneData.terrainData.gridPointSpacing);
  setUniform(terrainGeneratorProgramObject, ufHeightMultiplierName, sceneData.terrainData.heightMultiplier);
  setUniform(terrainGeneratorProgramObject, ufPixelsPerTriangleName, sceneData.terrainData.pixelsPerTriangle);
  setUniform(terrainGeneratorProgramObject, ufViewportSizeName,
             glm::vec2(windowData.width, windowData.height));
  setUniform(terrainGeneratorProgramObject, ufWorldLightName, sceneData.lightData.worldLightPosition);
  setUniform(terrainGeneratorProgramObject, ufWorldCameraPos, sceneData.fpsCamera.cameraPosition());
  setUniform(terrainGeneratorProgramObject, ufHorizontalClipPlane, glm ::vec4(0.0f, 1.0f, 0.0f, 0.412f));

  // Terrain noise/color/falloff map shader
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
  setUniform(terrainGeneratorDebugProgramObject, ufColorMapTextureName, 1);
  setUniform(terrainGeneratorDebugProgramObject, ufFalloffMapTextureName, 2);

  // Water shader
  std::vector<GLuint> waterShaderObjects;
  waterShaderObjects.push_back(compileShader("water.vert", GL_VERTEX_SHADER));
  waterShaderObjects.push_back(compileShader("water.frag", GL_FRAGMENT_SHADER));
  const auto waterProgramObject =
      programObjects.emplace(kWaterProgramObjectName, createProgramObject(waterShaderObjects)).first->second;

  setUniform(waterProgramObject, ufSceneTextureName, 0);

  // Quad shader
  std::vector<GLuint> quadShaderObjects; // deleeeeeeeete
  quadShaderObjects.push_back(compileShader("quad.vert", GL_VERTEX_SHADER));
  quadShaderObjects.push_back(compileShader("quad.frag", GL_FRAGMENT_SHADER));
  const auto quadProgramObject =
      programObjects.emplace(kQuadShaderProgramObjectName, createProgramObject(quadShaderObjects))
          .first->second;
  setUniform(quadProgramObject, ufSceneTextureName, 0);

  return programObjects;
}