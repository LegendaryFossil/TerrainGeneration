#version 430 core

uniform mat4 modelToWorldMatrix;
uniform mat4 worldToViewMatrix;
uniform mat4 viewToClipMatrix;

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoordIn;

out vec2 texCoordV;

void main() {
	gl_Position = viewToClipMatrix * worldToViewMatrix * modelToWorldMatrix * vec4(position, 0.0f, 1.0f);
	texCoordV = texCoordIn;
}