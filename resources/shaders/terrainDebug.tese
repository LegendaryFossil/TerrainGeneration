#version 430 core

layout(quads, fractional_even_spacing, cw) in;

uniform sampler2D heightMapTexture;

uniform mat4 modelToWorldMatrix;
uniform mat4 worldToViewMatrix;
uniform mat4 viewToClipMatrix;

// Vertices XZ position from the tessellation control shader
in vec2 positionTC[];

// Output texture coordinates for the fragment shader
out vec2 uvTE;

void main(){
	ivec2 tSize = textureSize(heightMapTexture, 0);
	vec2 div = tSize * 1.0 / 64.0;
	
	// Compute texture coordinates
	uvTE = positionTC[0].xy + gl_TessCoord.st / div;
	
	// Compute pos (scale x and z) [0..1] -> [0..tSize * gridSpacing]
	vec4 res;
	res.xz = uvTE.st * tSize;
	
	// Get height for the Y coordinate
	res.y = 1.0;
	res.w = 1.0;

	vec4 viewPosition = worldToViewMatrix * modelToWorldMatrix * res;

	// Transform the vertices as usual
	gl_Position = viewToClipMatrix * viewPosition;
}

