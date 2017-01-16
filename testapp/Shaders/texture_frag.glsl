#version 130

out vec4 fcolor;
smooth in vec2 outTexCoords;
smooth in vec4 outColor;

uniform sampler2D colorMap;

void main(){	
	fcolor = outColor * texture(colorMap, outTexCoords);
}
