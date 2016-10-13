#version 130

out vec4 fcolor;
smooth in vec2 outTexCoords;
smooth in vec3 outNormal;
smooth in vec3 eyeSpaceVertPosition;

uniform sampler2D colorMap;
uniform int isTextured;
uniform vec3 cameraPosition;

struct d_light{
	vec3 Color;
	vec3 Position;
	float AmbientIntensity;
	float DiffuseIntensity;
	vec3 Direction;
	
	float ConstAtten;
	float LinearAtten;
	float Exp;
};

struct material{
	vec3 ka;
	vec3 kd;
	vec3 ks;
	float ns;
};

uniform material material0;

uniform d_light light0;

void main()
{
	vec4 diffuse_color = vec4(0,0,0,0);
	vec4 specular_color = vec4(0,0,0,0);
	vec3 light_direction = eyeSpaceVertPosition - light0.Position;
	float dist_to_light = length(light_direction);
	light_direction = normalize(light_direction);
	
	vec4 ambient_color = vec4(light0.Color, 1.0f) * vec4(material0.ka, 1.0f) * light0.AmbientIntensity;
	
	float diffuse_factor = dot(normalize(outNormal), -light_direction);	
	if (diffuse_factor > 0){
		diffuse_color = vec4(light0.Color, 1.0f) * light0.DiffuseIntensity * diffuse_factor;
	}
	
	vec3 vert_to_eye = normalize(cameraPosition - eyeSpaceVertPosition);
	vec3 reflection = normalize(reflect(light_direction, outNormal));
	float specular_factor = dot(vert_to_eye, reflection);	
	if (specular_factor > 0){
		specular_color = vec4(light0.Color, 1.0f) * vec4(material0.ks, 1.0f) * material0.ns * specular_factor;
	}
	
	if (isTextured == 1){
		fcolor = texture(colorMap, outTexCoords)* (diffuse_color + ambient_color + specular_color);			
	} else{
		fcolor = vec4(material0.kd, 1.0f) * (diffuse_color + ambient_color + specular_color);	
	}
}