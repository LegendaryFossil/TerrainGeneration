#version 430

uniform sampler2D sceneTexture;

in vec2 texCoordV;

out vec4 outputColor;

void main() {
	const vec3 test = texture(sceneTexture, texCoordV).rgb;
	outputColor = vec4(test, 1.0f); //vec4(texture(sceneTexture, texCoordV).rgb, 1.0f);
}