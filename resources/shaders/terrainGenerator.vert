#version 330 core

uniform mat4 modelToWorldMatrix;
uniform mat4 worldToViewMatrix;
uniform mat4 viewToClipMatrix;
uniform mat3 normalMatrix;

uniform vec4 worldLight;

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal; 
layout(location = 2) in vec2 texCoordIn;

smooth out vec3 eyePos;
smooth out vec3 eyeNormal;
smooth out vec3 eyeLight;
out vec2 texCoord;

void main() {
	vec4 viewPos = worldToViewMatrix*modelToWorldMatrix*position;
	gl_Position = viewToClipMatrix*viewPos;
	texCoord = texCoordIn;

	eyePos = viewPos.xyz;
	eyeNormal = normalize(normalMatrix*normal);
	eyeLight = (worldToViewMatrix*worldLight).xyz;
}