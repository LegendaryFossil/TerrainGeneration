#version 330

uniform sampler2D terrainTexture;
uniform int useMapTexture;

uniform vec3 ambientConstant;

in vec2 texCoord;

smooth in vec3 eyePos;
smooth in vec3 eyeNormal;
smooth in vec3 eyeLight;

out vec4 outputColor;

void main() {
	if(useMapTexture == 1) {
		vec3 f_eyeNormal = normalize(eyeNormal);
		vec3 f_lightDirection = normalize(eyeLight-eyePos);
		vec3 f_viewDirection = normalize(-eyePos);

		vec3 halfWay = normalize(f_lightDirection+f_viewDirection);
	
		vec3 ambientReflection = ambientConstant;
		vec4 textureColor = texture(terrainTexture, texCoord);
		vec3 diffuseConstant = textureColor.rgb;
		vec3 diffuseReflection = diffuseConstant*clamp(dot(f_lightDirection, f_eyeNormal), 0.0f, 1.0f);

		vec3 blinnPhongOutput =  ambientReflection + diffuseReflection;
		outputColor = vec4(blinnPhongOutput, 1.0f);
	}
	else {
		outputColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
	}
}