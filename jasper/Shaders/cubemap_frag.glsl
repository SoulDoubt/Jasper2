#version 130

smooth in vec3 outTexCoords;
uniform samplerCube cubeMap;
out vec4 fragColor;
const float gamma = 2.2;

void main(){
	fragColor = texture(cubeMap, outTexCoords);

	fragColor.rgb = pow(fragColor.rgb, vec3(1.0/gamma));
}

