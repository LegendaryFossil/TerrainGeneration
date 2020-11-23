#version 430 core

layout(quads, fractional_even_spacing, cw) in;

uniform sampler2D heightMapTexture;

uniform float heightMultiplier;
uniform float terrainGridPointSpacing;
uniform float patchSize;

uniform int lightCount;
uniform vec4 worldLightPositions[2];

uniform vec4 horizontalClipPlane;

uniform mat4 modelToWorldMatrix;
uniform mat4 worldToViewMatrix;
uniform mat4 viewToClipMatrix;

in vec2 positionTC[];

out vec2 uvTE; // Texture coordinates
out vec3 worldPositionTE; // World vertex position
out vec3 viewPositionTE; // Vertex position as seen from camera
out vec3 viewLightPositionsTE[2]; // Lights as seen from the camera

void main(){
	ivec2 texSize = textureSize(heightMapTexture, 0);
	vec2 div = texSize * 1.0 / patchSize;
	
	// Compute texture coordinates, gl_TessCoord holds normalized coordinates [0, 1] for quads 
	uvTE = positionTC[0].xy + gl_TessCoord.st / div;
	
	// Compute vertex positions [0, 1] -> [0, texSize * terrainGridPointSpacing]
	// terrainGridPointSpacing adds the scaling
	vec4 vertex;
	vertex.xz = uvTE.st * texSize * terrainGridPointSpacing;
	vertex.y = texture(heightMapTexture, uvTE).r * heightMultiplier * terrainGridPointSpacing;
	vertex.w = 1.0;

	vec4 worldPosition = modelToWorldMatrix * vertex;
	worldPositionTE = worldPosition.xyz;
	gl_ClipDistance[0] = dot(vertex, horizontalClipPlane);

	vec4 viewPosition = worldToViewMatrix * worldPosition;
		
	viewPositionTE = viewPosition.xyz;

	for(int i = 0; i < lightCount; ++i) {
		viewLightPositionsTE[i] = (worldToViewMatrix*worldLightPositions[i]).xyz;
	}

	gl_Position = viewToClipMatrix * viewPosition;
}

