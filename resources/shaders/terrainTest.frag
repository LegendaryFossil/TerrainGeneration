#version 430

uniform mat3 normalMatrix;

uniform vec3 ambientConstant;

uniform sampler2D heightMapTexture;
uniform sampler2D colorMapTexture;

uniform float terrainGridPointSpacing;
uniform float heightMultiplier;

in vec2 uvTE;
in vec3 eyePosition;
in vec3 eyeLight;

out vec4 colorF;

//Function to retrieve heights
float height(const float u, const float v) {
	return (texture(heightMapTexture, vec2(u, v)).r * heightMultiplier);
}

void main() {
	// Compute normal 
	const float delta =  1.0 / (textureSize(heightMapTexture, 0).x);

	const float rightY = height(uvTE.s + delta, uvTE.t) - height(uvTE.s - delta, uvTE.t);
	const vec3 deltaX = vec3(2.0 * terrainGridPointSpacing, rightY, 0.0);

	const float forwardY = height(uvTE.s, uvTE.t + delta) - height(uvTE.s, uvTE.t - delta);
	const vec3 deltaZ = vec3 (0.0, forwardY, 2.0 * terrainGridPointSpacing);

	const vec3 normal = normalize(normalMatrix * cross(deltaZ, deltaX));

	// Compute color
	const vec3 lightDirection = normalize(eyeLight-eyePosition);
	const vec3 viewDirection = normalize(-eyePosition);
	const vec3 halfWay = normalize(lightDirection + viewDirection);
	
	const vec3 diffuseConstant = texture(colorMapTexture, uvTE).rgb;
	const vec3 diffuseReflection = diffuseConstant * clamp(dot(lightDirection, normal), 0.0f, 1.0f);
	const vec3 blinnPhongOutput =  ambientConstant + diffuseReflection;

	colorF = vec4(blinnPhongOutput, 1.0f);
}