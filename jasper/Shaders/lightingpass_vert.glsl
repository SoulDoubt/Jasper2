#version 330 core

in vec3 position;
in vec2 texCoords;

out vec2 outTexCoords;

void main(){
	gl_Position = vec4(position, 1.0);
	outTexCoords = texCoords;
}


