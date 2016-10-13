#version 330

in vec3 position;
in vec2 texCoords;
in vec4 color;
in vec3 normal;

uniform mat4 mvpMatrix;
uniform mat3 normalMatrix;
uniform mat4 mvMatrix;

smooth out vec2 outTexCoords;
smooth out vec3 outNormal;
smooth out vec3 eyeSpaceVertPosition;

void main()
{
	outTexCoords = texCoords;
	gl_Position = mvpMatrix * vec4(position, 1.0);			
}

