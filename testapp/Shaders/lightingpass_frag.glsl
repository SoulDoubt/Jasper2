#version 330 core

struct directional_light{
	vec3 Color;
	vec3 Direction;
	float AmbientIntensity;
	float DiffuseIntensity;	
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

const float gamma = 2.2;

float luminance(const in vec3 color)
{
    return dot(color, vec3(0.299, 0.587, 0.114));
}

void main(){

	vec3 world_position = texture(positionTexture, outTexCoords).xyz;
	vec4 diffuse_color = texture(diffuseTexture, outTexCoords);
	vec3 normal = texture(normalTexture, outTexCoords).xyz;
	vec3 specular = texture(specularTexture, outTexCoords).xyz;	

	vec3 light_direction = normalize(-dlight0.Direction); // need vector pointing towards light
	vec3 view_direction = normalize(world_position - cameraPosition);
	vec3 half_direction = normalize(light_direction + view_direction);

	vec3 ambient = material0.ka * dlight0.AmbientIntensity * diffuse_color.rgb;

	
	float d = clamp(max(dot(normal, light_direction), 0.0), 0, 1);
	vec3 ma = material0.ka * material0.kd;
	vec4 dc = diffuse_color * d * dlight0.DiffuseIntensity;// + vec4(ma, 1.0);

	
	vec3 reflection = normalize(reflect(-light_direction, normal));
	float specular_factor = clamp(dot(reflection, -view_direction), 0, 1);
	specular_factor = pow(specular_factor, material0.ns);
	vec4 sc = vec4(dlight0.Color * dlight0.DiffuseIntensity * specular_factor * specular, 1.0);

	fragColor = dc + sc + vec4(ambient, 1.0);// vec4(ambient, 1.0); //diffuse_color + specular_final;
	//fragColor = CalcDirectionalLight(normal, specular, diffuse_color.xyz, world_position);

	
	fragColor.rgb = pow(fragColor.rgb, vec3(1.0/gamma));

	//fragColor = vec4(normal, 1.0);
	//fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}