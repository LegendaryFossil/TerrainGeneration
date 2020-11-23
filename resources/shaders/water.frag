#version 430

uniform float waterDistortionMoveFactor;
uniform vec3 waterColor;

uniform int lightCount;
uniform vec3 lightColors[2];
uniform float specularLightIntensities[2];
uniform vec3 specularLightColors[2];
uniform float specularPowers[2];
uniform float reflectionStrength;

uniform sampler2D dudvTexture;
uniform sampler2D normalMapTexture;
uniform sampler2D sceneTexture;

in vec2 texCoordV;
in vec3 tangentLightPositionsV[2];;
in vec3 tangentPositionV;
in vec4 clipPositionV;

out vec4 outputColor;

const float distortionStrength = 0.02;

// Gamma correction
const vec3 gamma = vec3(2.2);

vec3 sRGBToLinear(const vec3 sRGB) {
	return pow(sRGB, gamma);
}

// R_F0 = Specular color
// E = toCamera vector
// N = normal
vec3 fresnel_schlick(const vec3 R_F0, const vec3 E, const vec3 N) {
	return R_F0 + (1.0 - R_F0) * pow(1.0 - max(0.0, dot(E,N)), 5.0);
}

float getBlinnPhongSpecular(const vec3 halfWay, const vec3 waterNormal) {
	float blinnPhongSpecular = 0.0;
	for(int i = 0; i < lightCount; ++i) {
		blinnPhongSpecular += pow(max(dot(waterNormal, halfWay), 0.0), specularPowers[i]) * (8.0 + specularPowers[i]) / 8.0;
	}

	return blinnPhongSpecular;
}

vec3 getReflectionColor(const vec4 clipSpacePosition, const vec2 distortionTextureCoordinates, const vec3 viewDirection, const vec3 waterNormal) {
	const vec2 totalDistortion = (texture(dudvTexture, distortionTextureCoordinates).rg * 2.0 - 1.0) * distortionStrength;

	vec2 projectiveTextureCoord = (clipSpacePosition.xy/clipSpacePosition.w) / 2.0 + 0.5;
	projectiveTextureCoord += totalDistortion;
	projectiveTextureCoord.x = clamp(projectiveTextureCoord.x, 0.001, 0.999);
	projectiveTextureCoord.y = clamp(projectiveTextureCoord.y, 0.001, 0.999);

	const vec3 reflectionColor = sRGBToLinear(texture(sceneTexture, vec2(projectiveTextureCoord.x, 1.0-projectiveTextureCoord.y)).rgb);
	const vec3 F = clamp(fresnel_schlick(vec3(0.04), viewDirection, waterNormal), 0.0, 1.0);
	return mix(sRGBToLinear(waterColor), reflectionColor, mix(F, vec3(1.0), reflectionStrength));
}


void main() {
	vec2 distortionTexCoord = texture(dudvTexture, vec2(texCoordV.x + waterDistortionMoveFactor, texCoordV.y)).rg * 0.1;
	distortionTexCoord = texCoordV + vec2(distortionTexCoord.x, distortionTexCoord.y + waterDistortionMoveFactor);

	// Reinterval normal so that it always points in the positive y-axis but can point in the positive or
	// negative direction in the xz-plane
	const vec3 normalMapValue = texture(normalMapTexture, distortionTexCoord).rgb;
	const vec3 waterNormal = normalize(vec3(normalMapValue.r * 2.0 - 1.0, normalMapValue.b * 3.0, normalMapValue.g * 2.0 - 1.0));

	const vec3 viewDirection = normalize(-tangentPositionV);
	vec3 reflectionColor = getReflectionColor(clipPositionV, distortionTexCoord, viewDirection, waterNormal);

	vec3 specularReflection = vec3(0.0);
	for(int i = 0; i < lightCount; ++i) {
		const vec3 lightDirection = normalize(tangentLightPositionsV[i] - tangentPositionV);
		const vec3 halfWay = normalize(lightDirection + viewDirection);
		float blinnPhongSpecular = getBlinnPhongSpecular(halfWay, waterNormal);
		specularReflection += specularLightIntensities[i] * fresnel_schlick(specularLightColors[i], lightDirection, halfWay) * lightColors[i] * blinnPhongSpecular;
	}
		
	vec3 finalColor = pow(reflectionColor + specularReflection, vec3(1.0/gamma));
	outputColor = vec4(finalColor, 1.0);
}