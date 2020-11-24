#pragma once

// TODO: Make this inline with C++17 support, otherwise including this header
// in multiple translation units allocates new memory for each unit
constexpr auto ufSkyboxTextureName = "skybox";
constexpr auto ufSceneTextureName = "sceneTexture";
constexpr auto ufDuDvTextureName = "dudvTexture";
constexpr auto ufNormalMapTextureName = "normalMapTexture";
constexpr auto ufFalloffMapTextureName = "falloffMapTexture";
constexpr auto ufColorMapTextureName = "colorMapTexture";
constexpr auto ufHeightMapTextureName = "heightMapTexture";

constexpr auto ufModelToWorldMatrixName = "modelToWorldMatrix";
constexpr auto ufWorldToViewMatrixName = "worldToViewMatrix";
constexpr auto ufViewToClipMatrixName = "viewToClipMatrix";
constexpr auto ufNormalMatrix = "normalMatrix";
constexpr auto ufViewportSizeName = "viewportSize";
constexpr auto ufHorizontalClipPlane = "horizontalClipPlane";

constexpr auto ufPatchSizeName = "patchSize";
constexpr auto ufPixelsPerTriangleName = "pixelsPerTriangle";
constexpr auto ufTerrainGridPointSpacingName = "terrainGridPointSpacing";
constexpr auto ufHeightMultiplierName = "heightMultiplier";

constexpr auto ufLightCount = "lightCount";
constexpr auto ufWorldLightPositionsName = "worldLightPositions";
constexpr auto ufLightColorsName = "lightColors";
constexpr auto ufSpecularLightColorsName = "specularLightColors";
constexpr auto ufSpecularLightIntensitiesName = "specularLightIntensities";
constexpr auto ufSpecularPowers = "specularPowers";

constexpr auto ufReflectionStrength = "reflectionStrength";
constexpr auto ufAmbientConstantName = "ambientConstant";

constexpr auto ufTerrainCount = "terrainCount";
constexpr auto ufTerrainColors = "terrainColors";
constexpr auto ufTerrainColorStrengths = "terrainColorStrengths";
constexpr auto ufTerrainHeights = "terrainHeights";
constexpr auto ufTerrainBlends = "terrainBlends";
constexpr auto ufTerrainTextures = "terrainTextures";
constexpr auto ufTerrainTextureScalings = "terrainTextureScalings";

constexpr auto ufWaterDistortionMoveFactorName = "waterDistortionMoveFactor";
constexpr auto ufWaterColor = "waterColor";

constexpr auto ufWorldCameraPosition = "worldCameraPosition";

constexpr auto ufDebugSettings = "debugSettings";