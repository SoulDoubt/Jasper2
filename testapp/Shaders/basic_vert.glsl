#version 130

in vec3 position;
in vec2 inTexCoords;
in vec4 vertColor;
in vec3 inNormal;

uniform mat4 mvpMatrix;
uniform mat3 normalMatrix;
uniform mat4 mvMatrix;

smooth out vec2 outTexCoords;
smooth out vec3 outNormal;
smooth out vec3 eyeSpaceVertPosition;

void main()
{
	gl_Position = mvpMatrix * vec4(position, 1.0);			
}

