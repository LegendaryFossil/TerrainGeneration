#version 430

uniform sampler2D heightMapTexture;
uniform sampler2D colorMapTexture;
uniform sampler2D falloffMapTexture;

uniform vec3 debugSettings;

in vec2 uvTE;

out vec4 colorF;

void main() {

if(debugSettings.x == 1) {
	colorF = texture(heightMapTexture, uvTE);
} else if (debugSettings.y == 1) {
	colorF = texture(colorMapTexture, uvTE);
} else if (debugSettings.z == 1) {
	colorF = texture(falloffMapTexture, uvTE);
}

}