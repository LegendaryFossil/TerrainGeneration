#version 330

uniform sampler2D terrainTexture;

in vec2 texCoord;

out vec4 outputColor;

void main() {
	outputColor = texture(terrainTexture, texCoord);
}