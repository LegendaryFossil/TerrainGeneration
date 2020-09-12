#version 430 core

layout(quads, fractional_even_spacing, cw) in;

uniform sampler2D heightMapTexture;
uniform float heightMultiplier;

uniform float terrainGridPointSpacing;
uniform float patchSize;

uniform mat4 modelToWorldMatrix;
uniform mat4 worldToViewMatrix;
uniform mat4 viewToClipMatrix;

uniform vec4 worldLight;

// Vertex position and light seen from the camera
out vec3 eyePosition;
out vec3 eyeLight;

in vec2 positionTC[];

out vec2 uvTE;

float getCurveValue(const float value) {
  return (1.8234 * value * value - 0.3233f * value - 0.0258);
}

void main(){
	ivec2 textureSize = textureSize(heightMapTexture, 0);
	vec2 div = textureSize * 1.0 / patchSize;
	
	// Compute texture coordinates, gl_TessCoord holds normalized coordinates [0, 1] for quads 
	uvTE = positionTC[0].xy + gl_TessCoord.st / div;
	
	// Compute vertex positions [0, 1] -> [0, textureSize * terrainGridPointSpacing]
	// TerrainGridPointSpacing adds the scaling "effect"
	vec4 res;
	res.xz = uvTE.st * textureSize * terrainGridPointSpacing;
	res.y = getCurveValue(texture(heightMapTexture, uvTE).r) * heightMultiplier;
	res.w = 1.0;

	vec4 viewPosition = worldToViewMatrix * modelToWorldMatrix * res;
		
	eyePosition = viewPosition.xyz;
	eyeLight = (worldToViewMatrix*worldLight).xyz; // Move this to fragment shader?

	gl_Position = viewToClipMatrix * viewPosition;
}

