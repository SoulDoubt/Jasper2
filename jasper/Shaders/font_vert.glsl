#version 130

in vec3 position;
in vec2 texCoords;
in vec4 color;

uniform mat4 mvpMatrix;

smooth out vec2 tex_coords;
out vec4 vert_color;

void main(){
	gl_Position = mvpMatrix * vec4(position, 1.0);
	tex_coords = texCoords;
	vert_color = color;
}
