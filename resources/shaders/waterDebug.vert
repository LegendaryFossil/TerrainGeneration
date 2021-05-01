#version 430 core

uniform mat4 modelToWorldMatrix;
uniform mat4 worldToViewMatrix;
uniform mat4 viewToClipMatrix;

layout(location = 0) in vec3 position;

void main() {
	gl_Position = viewToClipMatrix * worldToViewMatrix * modelToWorldMatrix * vec4(position, 1.0);
}