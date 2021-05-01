#version 430

uniform vec3 waterColor;

out vec4 outputColor;

void main() {
	outputColor = vec4(waterColor, 1.0);
}