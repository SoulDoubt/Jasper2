#version 330

in vec3 position;
in vec2 texCoords;


//uniform mat4 viewMatrix;
//uniform mat4 modelMatrix;
uniform mat4 mvpMatrix;

smooth out vec2 v_texCoords;

//out vec3 v_lightPosition;

void main()
{
	v_texCoords = texCoords;
	gl_Position = mvpMatrix * vec4( position, 1.0f );
}