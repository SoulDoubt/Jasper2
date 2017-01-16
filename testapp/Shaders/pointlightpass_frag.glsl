#version 330 core

struct point_light{
	vec3 Color;
	vec3 Position;
	float AmbientIntensity;
	float DiffuseIntensity;	
	
	float ConstAtten;
	float LinearAtten;
	float ExpAtten;
	float Radius;
};

struct material{
	vec3 ka;
	vec3 kd;
	vec3 ks;
	float ns;
};

in vec2 outTexCoords;

uniform sampler2D positionTexture;
uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D texCoordTexture;
uniform sampler2D specularTexture;

uniform vec3 cameraPosition;

uniform directional_light dlight0;
uniform material material0;

out vec4 fragColor;

//vec4 CalcDirectionalLight(vec3 normal, vec3 specular, vec3 diffuseColor, vec3 position){
	// vec4 specular_color, diffuse_color;	
	// vec3 dlight_direction = dlight0.Direction;
	// float diffuse_factor = max(dot(-dlight_direction, normal), 0.0);
	// if (diffuse_factor > 0) {
	// 	vec3 diff = dlight0.Color * dlight0.DiffuseIntensity * diffuse_factor;
	// 	diffuse_color = vec4(diff, 1.0f);
	// 	// vec3 vert_to_eye = normalize(position - cameraPosition);
	// 	// vec3 reflection = normalize(reflect(-dlight_direction, normal));
	// 	// float specular_factor = max(dot(reflection, vert_to_eye), 0.0);
	// 	// if (specular_factor > 0){
	// 	// 	specular_factor = pow(specular_factor, 2);
	// 	// 	vec3 spec = dlight0.Color * specular_factor * specular; 
	// 	// 	specular_color = vec4(spec, 1.0f);
	// 	// }
	// }
	// return diffuse_color;// + specular_color;
//}

void main(){

	vec3 world_position = texture(positionTexture, outTexCoords).xyz;
	vec4 diffuse_color = texture(diffuseTexture, outTexCoords);
	vec3 normal = texture(normalTexture, outTexCoords).xyz;
	vec3 specular = texture(specularTexture, outTexCoords).xyz;

	vec3 ambient = material0.ka * dlight0.AmbientIntensity * diffuse_color.rgb;

	vec3 light_direction = dlight0.Direction;
	float d = clamp(max(dot(normal, -light_direction), 0.0), 0, 1);
	vec3 ma = material0.ka * material0.kd;
	vec4 dc = diffuse_color * d * dlight0.DiffuseIntensity;// + vec4(ma, 1.0);
	//diffuse_color = vec4(dlight0.Color, 1.0) * diffuse_color * vec4(material0.kd, 1.0) * diffuse;

	

	vec3 camera_to_position = normalize(world_position - cameraPosition);
	vec3 reflection = normalize(reflect(light_direction, normal));
	float specular_factor = clamp(dot(reflection, -camera_to_position), 0, 1);
	specular_factor = pow(specular_factor, material0.ns);
	vec4 sc = vec4(dlight0.Color * specular_factor * specular, 1.0);

	fragColor = dc + sc;// vec4(ambient, 1.0); //diffuse_color + specular_final;
	//fragColor = CalcDirectionalLight(normal, specular, diffuse_color.xyz, world_position);

	float gamma = 2.2;
	fragColor.rgb = pow(fragColor.rgb, vec3(1.0/gamma));

	//fragColor = vec4(normal, 1.0);
	//fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}