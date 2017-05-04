#version 330 core

in vec3 position;
in vec2 texCoords;

uniform mat4 mvpMatrix;

out vec2 outTexCoords;

void main(){
	gl_Position = mvpMatrix * vec4(position, 1.0);
	outTexCoords = texCoords;
}