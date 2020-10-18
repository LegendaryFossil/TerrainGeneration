#version 430 core

uniform mat4 modelToWorldMatrix;
uniform mat4 worldToViewMatrix;
uniform mat4 viewToClipMatrix;

layout(location = 0) in vec3 position;

out vec3 texCoordV;

void main() {
	const vec4 worldPosition = modelToWorldMatrix * vec4(position, 1.0f);

	texCoordV = worldPosition.xyz;

	gl_Position = viewToClipMatrix * worldToViewMatrix * worldPosition;
}