#version 430 core

uniform mat4 modelToWorldMatrix;
uniform mat4 worldToViewMatrix;
uniform mat4 viewToClipMatrix;
uniform mat3 normalMatrix;

uniform vec3 worldCameraPosition;

uniform int lightCount;
uniform vec4 worldLightPositions[2];

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec3 bitangent;
layout(location = 4) in vec2 texCoord;

out vec2 texCoordV;
out vec3 tangentLightPositionsV[2];
out vec3 tangentCameraPositionV;
out vec3 tangentPositionV;
out vec4 clipPositionV;

mat3 createTBNMatrix(const vec3 T, const vec3 B, const vec3 N) {
   vec3 viewT = normalize(normalMatrix * T);
   vec3 viewB = normalize(normalMatrix * B);
   vec3 viewN = normalize(normalMatrix * N);
   return mat3(viewT, viewB, viewN);
}

void main() {
	const mat3 invTBNMatrix = transpose(createTBNMatrix(tangent, bitangent, normal));

	for(int i = 0; i < lightCount; ++i) {
		vec4 viewLightPosition = worldToViewMatrix * worldLightPositions[i];
		tangentLightPositionsV[i] =  invTBNMatrix * viewLightPosition.xyz;
	}

	vec3 viewCameraPosition = (worldToViewMatrix * vec4(worldCameraPosition, 1.0)).xyz;
	tangentCameraPositionV = invTBNMatrix * viewCameraPosition;

	vec4 viewPosition = worldToViewMatrix * modelToWorldMatrix * vec4(position, 1.0);
	tangentPositionV = invTBNMatrix * viewPosition.xyz;

	clipPositionV = viewToClipMatrix * viewPosition;
	texCoordV = texCoord;

	gl_Position = clipPositionV;
}