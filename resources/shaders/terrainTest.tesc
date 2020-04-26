#version 430 core

// number of output control points (and tess control shader invocations) per patch
layout(vertices = 1) out;

// XZ position from the vertex shader
in vec2 positionV[];

// XZ position for the tessellation evaluator shader
out vec2 positionTC[];

void main() {
	// Pass through the position
	positionTC[gl_InvocationID] = positionV[gl_InvocationID];
	
	// Define tessellation levels 
	gl_TessLevelOuter[0] = 64.0;
	gl_TessLevelOuter[1] = 64.0;
	gl_TessLevelOuter[2] = 64.0;
	gl_TessLevelOuter[3] = 64.0;
	gl_TessLevelInner[0] = 64.0;
	gl_TessLevelInner[1] = 64.0;
}