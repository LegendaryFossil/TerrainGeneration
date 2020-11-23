#version 430 core

// Output control points
layout(vertices = 1) out;

uniform sampler2D heightMapTexture;
uniform float heightMultiplier;

uniform float terrainGridPointSpacing;
uniform vec2 viewportSize;
uniform int pixelsPerTriangle;
uniform float patchSize;

uniform mat4 modelToWorldMatrix;
uniform mat4 worldToViewMatrix;
uniform mat4 viewToClipMatrix;

in vec2 positionV[];

out vec2 positionTC[];

// Number of invocations correspond to number of output control points
#define ID gl_InvocationID

float height(const float u, const float v) {
	return texture(heightMapTexture, vec2(u, v)).r * heightMultiplier;
}

// Args in clip space coordinates
float screenSphereDiameterPixels(const vec4 p1, const vec4 p2) {
	// Edge midpoint
	const vec4 clipSpaceP1 = (p1+p2) * 0.5;
	// Midpoint displaced by edge distance
	const vec4 clipSpaceP2 = vec4(clipSpaceP1.x, clipSpaceP1.y + distance(p1, p2), clipSpaceP1.z, clipSpaceP1.w);

	// Perspective division to transform points to NDC
	const vec4 ndcP1 =  clipSpaceP1 / clipSpaceP1.w;
	const vec4 ndcP2 =  clipSpaceP2 / clipSpaceP2.w;
	
	// Calculate length in pixels
	const vec2 ndcP1ToP2 = ndcP2.xy - ndcP1.xy;
	const float sphereRadiusPixels = length(ndcP1ToP2 * viewportSize * 0.5);
	return clamp(sphereRadiusPixels / pixelsPerTriangle, 1.0, patchSize);
}

const float eps = 0.0001;
bool patchEdgeInFrustum(const vec4 p1, const vec4 p2) {
	if((p1.x >= (-p1.w - eps) || p2.x >= (-p2.w - eps)) &&
		(p1.x <= (p1.w + eps) || p2.x <= (p2.w + eps)) &&
		(p1.z >= (-p1.w - eps) || p2.z >= (-p2.w - eps)) &&
		(p1.z <= (p1.w + eps) || p2.z <= (p2.w + eps))) {
		return true;
	}
	return false;
}

void main() {
	// Pass through the position
	positionTC[ID] = positionV[ID];

	const vec2 patchLowerLeftCorner = positionV[ID];
	
	const ivec2 textureSize = textureSize(heightMapTexture, 0);
	const float patchDiv = patchSize / textureSize.x;

	vec2 patchCornersTexCoord[4]; 
	patchCornersTexCoord[0] = patchLowerLeftCorner;
	patchCornersTexCoord[1] = vec2(patchLowerLeftCorner.x, patchLowerLeftCorner.y + patchDiv);
	patchCornersTexCoord[2] = vec2(patchLowerLeftCorner.x + patchDiv, patchLowerLeftCorner.y);
	patchCornersTexCoord[3] = vec2(patchLowerLeftCorner.x + patchDiv, patchLowerLeftCorner.y + patchDiv);

	vec2 patchCorners[4];
	for(int i = 0; i < 4; ++i) {
		patchCorners[i] = patchCornersTexCoord[i] * textureSize * terrainGridPointSpacing;
	}

	const mat4 mvp = viewToClipMatrix * worldToViewMatrix * modelToWorldMatrix;

	vec4 clipSpacePatchCorners[4];
	clipSpacePatchCorners[0] = mvp * vec4(patchCorners[0].x, height(patchCornersTexCoord[0].x, patchCornersTexCoord[0].y), patchCorners[0].y, 1.0f);
	clipSpacePatchCorners[1] = mvp * vec4(patchCorners[1].x, height(patchCornersTexCoord[1].x, patchCornersTexCoord[1].y), patchCorners[1].y, 1.0f);
	clipSpacePatchCorners[2] = mvp * vec4(patchCorners[2].x, height(patchCornersTexCoord[2].x, patchCornersTexCoord[2].y), patchCorners[2].y, 1.0f);
	clipSpacePatchCorners[3] = mvp * vec4(patchCorners[3].x, height(patchCornersTexCoord[3].x, patchCornersTexCoord[3].y), patchCorners[3].y, 1.0f);
		
	vec4 outerLevel = vec4(0.0, 0.0, 0.0, 0.0);
	vec2 innerLevel = vec2(0.0, 0.0);

	if(patchEdgeInFrustum(clipSpacePatchCorners[ID], clipSpacePatchCorners[ID + 1]) ||
		patchEdgeInFrustum(clipSpacePatchCorners[ID], clipSpacePatchCorners[ID + 2]) ||
		patchEdgeInFrustum(clipSpacePatchCorners[ID + 2], clipSpacePatchCorners[ID + 3]) ||
		patchEdgeInFrustum(clipSpacePatchCorners[ID + 3], clipSpacePatchCorners[ID + 1])) {
			// Define tessellation levels 
			outerLevel.x = screenSphereDiameterPixels(clipSpacePatchCorners[ID], clipSpacePatchCorners[ID + 1]);
			outerLevel.y = screenSphereDiameterPixels(clipSpacePatchCorners[ID], clipSpacePatchCorners[ID + 2]);
			outerLevel.z = screenSphereDiameterPixels(clipSpacePatchCorners[ID + 2], clipSpacePatchCorners[ID + 3]);
			outerLevel.w = screenSphereDiameterPixels(clipSpacePatchCorners[ID + 3], clipSpacePatchCorners[ID + 1]);
			innerLevel.x = max(outerLevel.y, outerLevel.w);
			innerLevel.y = max(outerLevel.x, outerLevel.z);
		}

		gl_TessLevelOuter[0] = outerLevel.x;
		gl_TessLevelOuter[1] = outerLevel.y;
		gl_TessLevelOuter[2] = outerLevel.z;
		gl_TessLevelOuter[3] = outerLevel.w;
		gl_TessLevelInner[0] = innerLevel.x;
		gl_TessLevelInner[1] = innerLevel.y;
}