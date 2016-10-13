#version 130

smooth in vec3 outTexCoords;
uniform samplerCube cubeMap;
out vec4 fragColor;

void main(){
	fragColor = texture(cubeMap, outTexCoords);
}

