#version 430

uniform sampler2D heightMapTexture;
uniform sampler2D falloffMapTexture;

uniform vec3 debugSettings;

in vec2 uvTE;

out vec4 colorF;

void main() {
colorF = vec4(0.0);

if(debugSettings.x == 1) {
	colorF = texture(heightMapTexture, uvTE);
} else if (debugSettings.y == 1) {
	colorF = texture(falloffMapTexture, uvTE);
}

}