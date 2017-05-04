#version 130

in vec3 position;
in vec4 color;


uniform mat4 mvpMatrix;

out vec4 vcolor;

void main()
{
	gl_Position = mvpMatrix * vec4(position, 1.0);		
	vcolor = color;	
}

