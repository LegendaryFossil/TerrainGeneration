#version 430

uniform mat3 normalMatrix;
uniform mat4 modelToWorldMatrix;

uniform float specularLightIntensity;
uniform float shineDamper;
uniform vec3 specularLightReflection;
uniform vec3 ambientConstant;

uniform sampler2D heightMapTexture;
uniform sampler2D colorMapTexture;
uniform sampler2D sceneTexture;
uniform sampler2D dudvTexture;
uniform sampler2D normalMapTexture;

uniform float terrainGridPointSpacing;
uniform float heightMultiplier;
uniform float waterDistortionMoveFactor;

in vec2 uvTE;
in vec3 viewPositionTE;
in vec3 viewLightPositionTE;
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

vec3 getReflectionColor(const vec4 clipSpacePosition, const vec2 distortionTextureCoordinates, 
				  const vec3 colorMapValue, const vec3 viewDirection, const vec3 waterNormal) {
	const vec2 totalDistortion = (texture(dudvTexture, distortionTextureCoordinates).rg * 2.0 - 1.0) * distortionStrength;

	vec2 projectiveTextureCoord = (clipSpacePosition.xy/clipSpacePosition.w) / 2.0 + 0.5;
	projectiveTextureCoord += totalDistortion;
	projectiveTextureCoord.x = clamp(projectiveTextureCoord.x, 0.001, 0.999);
	projectiveTextureCoord.y = clamp(projectiveTextureCoord.y, 0.001, 0.999);

	const vec3 reflectionColor = texture(sceneTexture, vec2(projectiveTextureCoord.x, 1.0-projectiveTextureCoord.y)).rgb;
	return mix(colorMapValue, reflectionColor, clamp(1.3 * fresnel_schlick(vec3(0.5), viewDirection, waterNormal), 0.0, 1.0));
}

vec3 getSpecularReflection(const vec2 distortionTextureCoordinates, const vec3 viewDirection, 
						const vec3 lightDirection, const vec3 waterNormal) {
	const vec3 halfWay = normalize(lightDirection + viewDirection);
	return specularLightIntensity * max(0.0, pow(dot(waterNormal, halfWay), shineDamper)) * specularLightReflection;
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
	const vec3 lightDirection = normalize(viewLightPositionTE-viewPositionTE);
	const vec3 viewDirection = normalize(-viewPositionTE);
	const vec3 colorMapValue = texture(colorMapTexture, uvTE).rgb;

	vec3 outputColor;
	if(colorMapValue == vec3(0.0, 0.0, 1.0)) {
		vec2 distortionTexCoord = texture(dudvTexture, vec2(uvTE.x + waterDistortionMoveFactor, uvTE.y)).rg * 0.1;
		distortionTexCoord = uvTE + vec2(distortionTexCoord.x, distortionTexCoord.y + waterDistortionMoveFactor);

		// Reinterval normal so that it always points in the positive y-axis but can point in the positive or
		// negative direction in the xz-plane
		const vec3 normalMapValue = texture(normalMapTexture, distortionTexCoord).rgb;
		const vec3 waterNormal = normalize(vec3(normalMapValue.r * 2.0 - 1.0, normalMapValue.b * 3.0, normalMapValue.g * 2.0 - 1.0));

		const vec3 reflectionColor = getReflectionColor(clipSpacePosTE, distortionTexCoord, colorMapValue, 
		viewDirection, waterNormal);
		const vec3 specularReflection = getSpecularReflection(distortionTexCoord, viewDirection, lightDirection, waterNormal);
		
		outputColor = reflectionColor + specularReflection;
	}
	else {
		const vec3 diffuseConstant = colorMapValue;
		const vec3 diffuseReflection = diffuseConstant * clamp(dot(lightDirection, viewNormal), 0.0f, 1.0f);
		outputColor =  ambientConstant + diffuseReflection;
	}

	colorF = vec4(outputColor, 1.0f);
}