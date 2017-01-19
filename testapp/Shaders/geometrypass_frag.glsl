#version 330 core

in vec2 v_texCoords;
in vec3 v_normal;
in vec4 v_fragPosition;
in mat3 v_tbnMatrix;

uniform sampler2D colorMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;

layout (location = 0) out vec3 WorldPosOut;   
layout (location = 1) out vec3 DiffuseOut;     
layout (location = 2) out vec3 NormalOut;     
layout (location = 3) out vec3 TexCoordOut; 
layout (location = 4) out vec3 SpecularOut; 

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

	vec3 materialSpecular;
	if (has_specular_map){
		materialSpecular = texture( specularMap, v_texCoords ).xyz;
	}
	else{
		materialSpecular = material0.ks;
	}
	
	vec4 map_color = texture(colorMap, v_texCoords);
	
	if (map_color == vec4(0,0,0,0)){
		map_color = vec4(material0.kd, 1.0);
	}
	
	//map_color = vec4(1.0, 0.0, 0.0, 1.0);
	WorldPosOut = v_fragPosition.xyz;
	DiffuseOut  = map_color.xyz;
	NormalOut   = normal;
	TexCoordOut = vec3(v_texCoords, 0.0);
	SpecularOut = materialSpecular;
	
}


