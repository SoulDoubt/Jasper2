#version 330 core

in vec2 v_texCoords;
in vec3 v_normal;
in vec4 v_fragPosition;
in mat3 v_tbnMatrix;

uniform sampler2D colorMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;

layout (location = 0) out vec4 WorldPosOut;   
layout (location = 1) out vec4 DiffuseOut;     
layout (location = 2) out vec4 NormalOut;     
layout (location = 3) out vec4 SpecularOut; 
layout (location = 4) out vec4 FinalOut; 

uniform bool has_diffuse_map;
uniform bool has_normal_map;
uniform bool has_specular_map;

struct material{
	vec3 ka;
	vec3 kd;
	vec3 ks;
	float ns;
};

uniform  material material0;

void main(){

	vec3 normal;
	if (has_normal_map){
		vec3 fn = texture( normalMap, v_texCoords ).xyz;
		fn = fn * 2.0 - 1.0;
		vec3 fragNormal = v_tbnMatrix * fn;				
		normal = normalize(fragNormal);
	}
	else {
		normal = normalize(v_normal); 	
	}

	vec4 specular;
	if (has_specular_map){
		specular = texture( specularMap, v_texCoords );
	}
	else{
		specular = vec4(material0.ks, material0.ns);
	}

	vec4 color;
	if (has_diffuse_map){
		color = texture(colorMap, v_texCoords);
	}
	else {
		color = vec4(material0.kd, 1.0);
	}
		
	WorldPosOut = v_fragPosition;
	DiffuseOut  = color;
	NormalOut   = vec4(normal, 0.0);	
	SpecularOut = specular;
	
}


