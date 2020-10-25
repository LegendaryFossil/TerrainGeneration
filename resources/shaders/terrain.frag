#version 430

uniform mat3 normalMatrix;
uniform mat4 modelToWorldMatrix;

uniform vec3 ambientConstant;

uniform sampler2D heightMapTexture;
uniform sampler2D colorMapTexture;
uniform sampler2D sceneTexture;
uniform sampler2D dudvTexture;

uniform float terrainGridPointSpacing;
uniform float heightMultiplier;
uniform float waterDistortionMoveFactor;

in vec2 uvTE;
in vec3 eyePositionTE;
in vec3 eyeLightTE;
in vec4 clipSpacePosTE;

out vec4 colorF;

const float distortionStrength = 0.02;

//Function to retrieve heights
float height(const float u, const float v) {
	return (texture(heightMapTexture, vec2(u, v)).r * heightMultiplier);
}

// R_F0 = Specular color
// E = toCamera vector
// N = normal
vec3 fresnel_schlick(const vec3 R_F0, const vec3 E, const vec3 N) {
	return R_F0 + (1.0 - R_F0) * pow(1.0 - max(0.0, dot(E,N)), 5.0);
}

void main() {
	// Compute normal 
	const float delta =  1.0 / (textureSize(heightMapTexture, 0).x);

	const float rightY = height(uvTE.s + delta, uvTE.t) - height(uvTE.s - delta, uvTE.t);
	const vec3 deltaX = vec3(2.0 * terrainGridPointSpacing, rightY, 0.0);

	const float forwardY = height(uvTE.s, uvTE.t + delta) - height(uvTE.s, uvTE.t - delta);
	const vec3 deltaZ = vec3 (0.0, forwardY, 2.0 * terrainGridPointSpacing);

	const vec3 viewNormal = normalize(normalMatrix * cross(deltaZ, deltaX));

	// Compute color
	const vec3 lightDirection = normalize(eyeLightTE-eyePositionTE);
	const vec3 viewDirection = normalize(-eyePositionTE);
	const vec3 halfWay = normalize(lightDirection + viewDirection);
	
	const vec3 colorMapValue = texture(colorMapTexture, uvTE).rgb;

	vec3 outputColor;
	if(colorMapValue == vec3(0.0, 0.0, 1.0)) {
		vec2 projectiveTextureCoord = (clipSpacePosTE.xy/clipSpacePosTE.w) / 2.0 + 0.5;
		
		vec2 distortion = (texture(dudvTexture, vec2(uvTE.x + waterDistortionMoveFactor, uvTE.y)).rg * 2.0 - 1.0) * distortionStrength;
		distortion += (texture(dudvTexture, vec2(-uvTE.x + waterDistortionMoveFactor, uvTE.y + waterDistortionMoveFactor)).rg * 2.0 - 1.0) * distortionStrength;

		projectiveTextureCoord += distortion;
		projectiveTextureCoord.x = clamp(projectiveTextureCoord.x, 0.001, 0.999);
		projectiveTextureCoord.y = clamp(projectiveTextureCoord.y, 0.001, 0.999);

		const vec3 reflectColorValue = texture(sceneTexture, vec2(projectiveTextureCoord.x, 1.0-projectiveTextureCoord.y)).rgb;
		
		outputColor = mix(colorMapValue, reflectColorValue, clamp(1.3 * fresnel_schlick(vec3(0.5), viewDirection, viewNormal), 0.0, 1.0));
	}
	else {
		const vec3 diffuseConstant = colorMapValue;
		const vec3 diffuseReflection = diffuseConstant * clamp(dot(lightDirection, viewNormal), 0.0f, 1.0f);
		outputColor =  ambientConstant + diffuseReflection;
	}

	colorF = vec4(outputColor, 1.0f);
}