#version 430

uniform samplerCube skybox;

in vec3 texCoordV;

out vec4 outputColor;

void main() {
	outputColor = texture(skybox, texCoordV);
}