#version 430 core

// Output control points
layout(vertices = 1) out;

in vec2 positionV[];

out vec2 positionTC[];

void main() {
	// Pass through the position
	positionTC[gl_InvocationID] = positionV[gl_InvocationID];

	gl_TessLevelOuter[0] = 64.0;
	gl_TessLevelOuter[1] = 64.0;
	gl_TessLevelOuter[2] = 64.0;
	gl_TessLevelOuter[3] = 64.0;
	gl_TessLevelInner[0] = 64.0;
	gl_TessLevelInner[1] = 64.0;
}