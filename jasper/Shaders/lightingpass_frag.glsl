#version 330 core

struct directional_light{
	vec3 Color;
	vec3 Direction;
	float AmbientIntensity;
	float DiffuseIntensity;	
};


in vec2 outTexCoords;

uniform sampler2D positionTexture;
uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D texCoordTexture;
uniform sampler2D specularTexture;

uniform vec3 cameraPosition;

uniform directional_light dlight0;

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
	vec4 specular = texture(specularTexture, outTexCoords);

	vec3 light_direction = normalize(-dlight0.Direction); // need vector pointing towards light
	vec3 view_direction = normalize(world_position - cameraPosition);
	vec3 half_direction = normalize(light_direction + view_direction);

	vec4 ambient = dlight0.AmbientIntensity * diffuse_color;

	vec4 dc = vec4(0,0,0,1);
	vec4 sc = vec4(0,0,0,1);
	float d = clamp(max(dot(normal, light_direction), 0.0), 0, 1);
	if (d > 0){	
		dc = diffuse_color * d * dlight0.DiffuseIntensity;// + vec4(ma, 1.0);
	
		vec3 reflection = normalize(reflect(-light_direction, normal));
		float specular_factor = clamp(dot(reflection, -view_direction), 0, 1);
		specular_factor = pow(specular_factor, specular.a * 255);
		sc = vec4(dlight0.Color * luminance(specular.rgb) * specular_factor, 1.0);
	}

	fragColor = ambient + dc + sc;// + ambient;// vec4(ambient, 1.0); //diffuse_color + specular_final;
	//fragColor = CalcDirectionalLight(normal, specular, diffuse_color.xyz, world_position);

	
	fragColor.rgb = pow(fragColor.rgb, vec3(1.0/gamma));

	//fragColor = vec4(normal, 1.0);
	//fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}