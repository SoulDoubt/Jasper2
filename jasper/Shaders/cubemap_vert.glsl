#version 130

in vec3 position;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

smooth out vec3 outTexCoords;

void main(){

	outTexCoords = position;
	gl_Position = projectionMatrix * viewMatrix * vec4(position, 1.0);
}
