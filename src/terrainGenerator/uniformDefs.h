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
constexpr auto ufWaterDistortionMoveFactorName = "waterDistortionMoveFactor";
constexpr auto ufHeightMultiplierName = "heightMultiplier";

constexpr auto ufWorldLightPositionName = "worldLightPosition";
constexpr auto ufAmbientConstantName = "ambientConstant";
constexpr auto ufSpecularLightReflectionName = "specularLightReflection";
constexpr auto ufSpecularLightIntensityName = "specularLightIntensity";
constexpr auto ufShineDamper = "shineDamper";

constexpr auto ufGrassTextureName = "grassTexture";
constexpr auto ufSandTextureName = "sandTexture";

constexpr auto ufWater = "water";
constexpr auto ufSand = "sand";
constexpr auto ufGrass = "grass";
constexpr auto ufRock = "rock";
constexpr auto ufMountain = "mountain";
constexpr auto ufSnow = "snow";

constexpr auto ufTerrainCount = "terrainCount";
constexpr auto ufTerrainColors = "terrainColors";
constexpr auto ufTerrainColorStrengths = "terrainColorStrengths";
constexpr auto ufTerrainHeights = "terrainHeights";
constexpr auto ufTerrainBlends = "terrainBlends";
constexpr auto ufTerrainTextures = "terrainTextures";
constexpr auto ufTerrainTextureScalings = "terrainTextureScalings";

constexpr auto ufDebugScale = "debugScale";

constexpr auto ufDebugSettings = "debugSettings";