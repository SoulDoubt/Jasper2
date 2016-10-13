#version 330

in vec2 v_texCoords;

uniform sampler2D shadowMap;

out vec4 fragColor;

void main(){

	float depth = texture(shadowMap, v_texCoords).x;
	depth = 1.0 - (1.0 - depth) * 25.0;
	fragColor = vec4(depth);

}