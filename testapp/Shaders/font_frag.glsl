#version 130

smooth in vec2 tex_coords;
in vec4 vert_color;
uniform sampler2D colorMap;
out vec4 fcolor;

void main(){
	//vec4 font_color = texture(colorMap, tex_coords);	
	fcolor = vec4(vert_color.rgb, texture(colorMap, tex_coords).a);
}
