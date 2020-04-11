#version 330 core

uniform mat4 modelToWorldMatrix;
uniform mat4 worldToViewMatrix;
uniform mat4 viewToClipMatrix;
uniform mat3 normalMatrix;

layout(location = 0) in vec4 position;
layout(location = 2) in vec2 texCoordIn;

out vec2 texCoord;

void main() {
	vec4 viewPos = worldToViewMatrix*modelToWorldMatrix*position;
	gl_Position = viewToClipMatrix*viewPos;
	texCoord = texCoordIn;
}