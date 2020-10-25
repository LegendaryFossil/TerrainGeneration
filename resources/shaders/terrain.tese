#version 430 core

layout(quads, fractional_even_spacing, cw) in;

uniform sampler2D heightMapTexture;

uniform float heightMultiplier;
uniform float terrainGridPointSpacing;
uniform float patchSize;

uniform vec4 worldLight;
uniform vec4 horizontalClipPlane;

uniform mat4 modelToWorldMatrix;
uniform mat4 worldToViewMatrix;
uniform mat4 viewToClipMatrix;

in vec2 positionTC[];

out vec2 uvTE; // Texture coordinates
out vec3 eyePositionTE; // Vertex position as seen from camera
out vec3 eyeLightTE; // Light as seen from the camera
out vec4 clipSpacePosTE; // Clip space vertex

void main(){
	ivec2 textureSize = textureSize(heightMapTexture, 0);
	vec2 div = textureSize * 1.0 / patchSize;
	
	// Compute texture coordinates, gl_TessCoord holds normalized coordinates [0, 1] for quads 
	uvTE = positionTC[0].xy + gl_TessCoord.st / div;
	
	// Compute vertex positions [0, 1] -> [0, textureSize * terrainGridPointSpacing]
	// TerrainGridPointSpacing adds the scaling "effect"
	vec4 vertex;
	vertex.xz = uvTE.st * textureSize * terrainGridPointSpacing;
	vertex.y = texture(heightMapTexture, uvTE).r * heightMultiplier * terrainGridPointSpacing;
	vertex.w = 1.0;

	vec4 worldPosition = modelToWorldMatrix * vertex;
	gl_ClipDistance[1] = dot(worldPosition, horizontalClipPlane);

	vec4 viewPosition = worldToViewMatrix * worldPosition;
		
	eyePositionTE = viewPosition.xyz;
	eyeLightTE = (worldToViewMatrix*worldLight).xyz;

	clipSpacePosTE = viewToClipMatrix * viewPosition;
	gl_Position = clipSpacePosTE;
}

