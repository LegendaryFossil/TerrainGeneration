#version 430 core

layout(quads, fractional_even_spacing, cw) in;

uniform sampler2D heightMapTexture;

uniform mat4 modelToWorldMatrix;
uniform mat4 worldToViewMatrix;
uniform mat4 viewToClipMatrix;

in vec2 positionTC[];

out vec2 uvTE;

void main(){
	ivec2 texSize = textureSize(heightMapTexture, 0);
	vec2 div = texSize * 1.0 / 64.0;
	
		
	// Compute texture coordinates, gl_TessCoord holds normalized coordinates [0, 1] for quads 
	uvTE = positionTC[0].xy + gl_TessCoord.st / div;
	
	// Compute vertex positions [0, 1] -> [0, texSize]
	vec4 res;
	res.xz = uvTE.st * texSize;
	res.y = 1.0;
	res.w = 1.0;

	vec4 viewPosition = worldToViewMatrix * modelToWorldMatrix * res;
	gl_Position = viewToClipMatrix * viewPosition;
}

