#version 430 core

uniform mat4 modelToWorldMatrix;
uniform mat4 worldToViewMatrix;
uniform mat4 viewToClipMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoordIn;

out vec2 texCoordV;
out vec4 clipSpacePositionV;

void main() {
	vec4 clipSpacePositionV = viewToClipMatrix * worldToViewMatrix * modelToWorldMatrix * vec4(position, 1.0f);
	texCoordV = texCoordIn;
	gl_Position = clipSpacePositionV;
}