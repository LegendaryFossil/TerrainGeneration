#version 430

uniform sampler2D sceneTexture;

in vec2 texCoordV;
in vec4 clipSpacePositionV;

out vec4 outputColor;

void main() {
	vec2 ndc = (clipSpacePositionV.xy / clipSpacePositionV.w) / 2.0 + 0.5;
	vec2 reflectCoord = vec2(ndc.x, ndc.y);

	const vec4 reflectColor = texture(sceneTexture, reflectCoord);
	const vec4 waterColor = vec4(1.0, 0.0, 1.0, 1.0);
	outputColor = waterColor;
}