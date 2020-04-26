#version 430 core

layout(quads, fractional_even_spacing, cw) in;

uniform sampler2D heightMapTexture;
uniform float heightMultiplier;

// Units between two consecutive grid points 
uniform float terrainGridPointSpacing;

uniform mat4 modelToWorldMatrix;
uniform mat4 worldToViewMatrix;
uniform mat4 viewToClipMatrix;

uniform vec4 worldLight;

// Vertices XZ position from the tessellation control shader
in vec2 positionTC[];

// Output texture coordinates for the fragment shader
out vec2 uvTE;

// Vertex position and light seen from the camera
out vec3 eyePosition;
out vec3 eyeLight;

float getCubicValue(const float value) {
  return value * value * value;
}

void main(){
	ivec2 tSize = textureSize(heightMapTexture, 0);
	vec2 div = tSize * 1.0 / 64.0;
	
	// Compute texture coordinates
	uvTE = positionTC[0].xy + gl_TessCoord.st / div;
	
	// Compute pos (scale x and z) [0..1] -> [0..tSize * gridSpacing]
	vec4 res;
	res.xz = uvTE.st * tSize * terrainGridPointSpacing;
	
	// Get height for the Y coordinate
	res.y = getCubicValue(texture(heightMapTexture, uvTE).r) * heightMultiplier;
	res.w = 1.0;

	vec4 viewPosition = worldToViewMatrix * modelToWorldMatrix * res;
		
	eyePosition = viewPosition.xyz;
	eyeLight = (worldToViewMatrix*worldLight).xyz; // Move this to fragment shader?

	// Transform the vertices as usual
	gl_Position = viewToClipMatrix * viewPosition;
}

