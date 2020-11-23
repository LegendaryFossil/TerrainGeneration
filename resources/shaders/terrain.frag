#version 430

uniform mat3 normalMatrix;
uniform mat4 modelToWorldMatrix;

/*
[0] = Water
[1] = Sand
[2] = Grass
[3] = Rocks
[4] = Mountain
[5] = Snow
*/
uniform int terrainCount;
uniform vec3 terrainColors[6];
uniform float terrainColorStrengths[6];
uniform float terrainHeights[6];
uniform float terrainBlends[6];
uniform float terrainTextureScalings[6];

uniform int lightCount;
uniform vec3 lightColors[2];
uniform vec3 ambientConstant;

uniform sampler2D heightMapTexture;
uniform sampler2D colorMapTexture;
uniform sampler2DArray terrainTextures;

uniform float terrainGridPointSpacing;
uniform float heightMultiplier;

in vec2 uvTE;
in vec3 worldPositionTE;
in vec3 viewPositionTE;
in vec3 viewLightPositionsTE[2];

out vec4 colorF;

// Gamma correction
const vec3 gamma = vec3(2.2);

vec3 sRGBToLinear(const vec3 sRGB) {
	return pow(sRGB, gamma);
}

// When generating the noise map the min height is always clamped to 0.0
// and max height is 1.0 * heightMultiplier * terrainGridPointSpacing
const float minHeight = 0.0;
const float maxHeight = 1.0 * heightMultiplier * terrainGridPointSpacing;

float height(const float u, const float v) {
	return (texture(heightMapTexture, vec2(u, v)).r * heightMultiplier);
}

vec3 triPlanarTextureWeight(const vec3 worldNormal) {
	// Use world normal as weights and take absolute value as we are not interested direction   
	vec3 weights = abs( worldNormal );

	// Convert from [-1, 1] to [weightEps, 1], make sure there is always a weight larger than 0
	const float weightEps = 0.00001;
	weights = normalize(max(weights, weightEps));
		
	// Force weights to sum to 1
	weights /= (weights.x + weights.y + weights.z);
	
	return weights;
}

vec3 getTerrainTextureColor(const vec3 worldPosition, const vec3 textureWeights) {
	const float eps = 0.0001;

	float heightPercent = smoothstep(minHeight, maxHeight, worldPosition.y);
	vec3 color = vec3(0.0);
	for(int i = 0; i < terrainCount; ++i) {
		float drawStrength = smoothstep(-terrainBlends[i]/2.0 - eps, terrainBlends[i]/2.0, heightPercent - terrainHeights[i]);
		
		vec3 tintColor = terrainColors[i] * terrainColorStrengths[i];

		const vec3 scaledWorldPos = worldPosition / terrainTextureScalings[i];
		vec3 xProjection = sRGBToLinear(texture(terrainTextures, vec3(scaledWorldPos.yz, i)).rgb);
		vec3 yProjection = sRGBToLinear(texture(terrainTextures, vec3(scaledWorldPos.xz, i)).rgb);
		vec3 zProjection = sRGBToLinear(texture(terrainTextures, vec3(scaledWorldPos.xy, i)).rgb);
		vec3 textureColor = textureWeights.x * xProjection + textureWeights.y * yProjection + textureWeights.z * zProjection;
		textureColor *= (1.0-terrainColorStrengths[i]);

		color = color * (1.0 - drawStrength) + (tintColor + textureColor) * drawStrength;
	}

	return color;
}

void main() {
	// We use a plane as our water mesh so any fragment at 
	// the bottom of the terrain can be discarded
	if(worldPositionTE.y < 0.0001) {
		discard;
	}

	// Compute normal 
	const float delta =  1.0 / (textureSize(heightMapTexture, 0).x);
	
	const float rightY = height(uvTE.s + delta, uvTE.t) - height(uvTE.s - delta, uvTE.t);
	const vec3 deltaX = vec3(2.0 * terrainGridPointSpacing, rightY, 0.0);
	
	const float forwardY = height(uvTE.s, uvTE.t + delta) - height(uvTE.s, uvTE.t - delta);
	const vec3 deltaZ = vec3 (0.0, forwardY, 2.0 * terrainGridPointSpacing);

	const vec3 modelNormal = normalize(cross(deltaZ, deltaX));
	const vec3 worldNormal = mat3(modelToWorldMatrix) * modelNormal;
	const vec3 viewNormal = normalMatrix * modelNormal;
	
	vec3 diffuseConstant = getTerrainTextureColor(worldPositionTE, triPlanarTextureWeight(worldNormal));

	vec3 diffuseReflection = vec3(0.0);
	for(int i = 0; i < lightCount; ++i) {
		const vec3 lightDirection = normalize(viewLightPositionsTE[i]-viewPositionTE);
		diffuseReflection += diffuseConstant * clamp(dot(lightDirection, viewNormal), 0.0f, 1.0f) * lightColors[i];
	}

	// Convert back to sRGB before outputting to framebuffer
	vec3 outputColor = pow(ambientConstant * diffuseConstant + diffuseReflection, vec3(1.0/gamma));
	colorF = vec4(outputColor, 1.0f);
}