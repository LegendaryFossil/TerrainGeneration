#pragma once

// TODO: Make this inline with C++17 support, otherwise including this header
// in multiple translation units allocates new memory for each unit
constexpr auto ufAmbientConstantName = "ambientConstant";
constexpr auto ufSkyboxTextureName = "skybox";
constexpr auto ufSceneTextureName = "sceneTexture";
constexpr auto ufDuDvTextureName = "dudvTexture";
constexpr auto ufNormalMapTextureName = "normalMapTexture";
constexpr auto ufFalloffMapTextureName = "falloffMapTexture";
constexpr auto ufColorMapTextureName = "colorMapTexture";
constexpr auto ufHeightMapTextureName = "heightMapTexture";
constexpr auto ufHeightMultiplierName = "heightMultiplier";
constexpr auto ufModelToWorldMatrixName = "modelToWorldMatrix";
constexpr auto ufNormalMatrix = "normalMatrix";
constexpr auto ufPatchSizeName = "patchSize";
constexpr auto ufPixelsPerTriangleName = "pixelsPerTriangle";
constexpr auto ufTerrainGridPointSpacingName = "terrainGridPointSpacing";
constexpr auto ufWaterDistortionMoveFactorName = "waterDistortionMoveFactor";
constexpr auto ufViewportSizeName = "viewportSize";
constexpr auto ufViewToClipMatrixName = "viewToClipMatrix";
constexpr auto ufWorldToViewMatrixName = "worldToViewMatrix";
constexpr auto ufHorizontalClipPlane = "horizontalClipPlane";
constexpr auto ufWorldLightPositionName = "worldLightPosition";
constexpr auto ufSpecularLightReflectionName = "specularLightReflection";
constexpr auto ufSpecularLightIntensityName = "specularLightIntensity";
constexpr auto ufShineDamper = "shineDamper";
constexpr auto ufDebugSettings = "debugSettings";