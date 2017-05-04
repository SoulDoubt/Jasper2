#version 330 core

struct point_light{
	vec3  Color;
	vec3  Position;
	float AmbientIntensity;
	float Attenuation;	
	float DiffuseIntensity;
	
	float ConeAngle;
	vec3  ConeDirection;
	
	float Radius;
};

in vec2 outTexCoords;

uniform sampler2D positionTexture;
uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D specularTexture;
uniform sampler2D finalTexture;
uniform vec2 screenSize;

uniform vec3 cameraPosition;

uniform point_light plight0;

out vec4 fragColor;

const float gamma = 2.2;

vec2 CalcTexCoord()
{
   return gl_FragCoord.xy / screenSize;
}

float saturate(float val){
	return clamp(val, 0.0, 1.0);
}

float luminance(const in vec3 color)
{
    return dot(color, vec3(0.299, 0.587, 0.114));
}

vec4 CalculatePointLight(point_light plight, vec4 color, vec3 normal, vec4 specular, vec3 fragPosition){

	vec3 L = plight.Position - fragPosition;
	float D = length(L);	

	vec3 view = normalize(cameraPosition - fragPosition);

	vec3 halfvec = normalize(L + view);

	vec4 diffuse = vec4(0,0,0,1);
	vec4 ambient = vec4(plight0.Color, 1) * plight0.AmbientIntensity * color;
	vec4 specular_color = vec4(0,0,0,1);

	float lambert = saturate(dot(normal, normalize(L)));
	//if (lambert > 0){

		diffuse = vec4(plight0.Color, 1) * plight.DiffuseIntensity * color * lambert;		
		

		vec3 reflection = normalize(reflect(-L, normal));	
		float spec_angle = max(dot(reflection, view), 0.0);
		float sf = pow(spec_angle, specular.a * 255);
	
	   	specular_color = vec4(plight.Color * luminance(specular.rgb), 1.0) * sf;
   //}
   //return vec4(saturate(specular.a), saturate(specular.a), saturate(specular.a), 1.0);
	return ambient + diffuse + specular_color;//;ambient +  diffuse;// + vec4(specular_color, 1.0);
}

void main(){
	vec2 texCoords = CalcTexCoord();
	vec3 world_position = texture(positionTexture, texCoords).xyz;
	vec4 diffuse_col = texture(diffuseTexture, texCoords);
	vec3 normal = texture(normalTexture, texCoords).xyz;
	vec4 specular = texture(specularTexture, texCoords);

	vec4 lighting = CalculatePointLight(plight0, diffuse_col, normal, specular, world_position);
	//lighting.rgb = pow(lighting.rgb, vec3(1.0/gamma));
	fragColor = lighting;

}