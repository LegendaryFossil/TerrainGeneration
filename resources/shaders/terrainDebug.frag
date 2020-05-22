#version 430

uniform sampler2D heightMapTexture;
uniform sampler2D colorMapTexture;

uniform int useNoiseMapTexture;

in vec2 uvTE;

out vec4 colorF;

void main() {

if(useNoiseMapTexture == 1) {
	colorF = texture(heightMapTexture, uvTE);
} else {
	colorF = texture(colorMapTexture, uvTE);
}

}