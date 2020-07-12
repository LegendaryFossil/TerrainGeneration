#pragma once

// TODO: Make this inline with C++17 support, otherwise including this header
// in multiple translation units allocates new memory for each unit
constexpr auto ufAmbientConstantName = "ambientConstant";
constexpr auto ufColorMapTextureName = "colorMapTexture";
constexpr auto ufHeightMapTextureName = "heightMapTexture";
constexpr auto ufHeightMultiplierName = "heightMultiplier";
constexpr auto ufModelToWorldMatrixName = "modelToWorldMatrix";
constexpr auto ufNormalMatrix = "normalMatrix";
constexpr auto ufPatchSizeName = "patchSize";
constexpr auto ufPixelsPerTriangleName = "pixelsPerTriangle";
constexpr auto ufTerrainGridPointSpacingName = "terrainGridPointSpacing";
constexpr auto ufUseNoiseMapTextureName = "useNoiseMapTexture";
constexpr auto ufViewportSizeName = "viewportSize";
constexpr auto ufViewToClipMatrixName = "viewToClipMatrix";
constexpr auto ufWorldLightName = "worldLight";
constexpr auto ufWorldToViewMatrixName = "worldToViewMatrix";