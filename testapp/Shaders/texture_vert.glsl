#version 130

in vec2 position;
in vec2 texCoords;
in vec4 color;


uniform mat4 mvpMatrix;


smooth out vec2 outTexCoords;
smooth out vec4 outColor;
void main()
{
	outTexCoords = texCoords;
	gl_Position = mvpMatrix * vec4(position, 0.0, 1.0);			
}

