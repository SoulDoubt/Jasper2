#version 330

in vec3 v_normal;
in vec3 v_eyeSpaceVertPosition;
in vec3 v_eyeSpaceLightPosition;
in mat3 v_tbnMatrix;
in vec2 v_texCoords;
in vec3 v_cameraPosition;

out vec4 fcolor;

uniform sampler2D colorMap;
uniform sampler2D normalMap;

const vec3 lightPos = vec3(1.0,1.0,1.0);
const vec4 specColor = vec4(1.0, 1.0, 1.0, 1.0);

vec4 diffuseColor = vec4(0.0, 0.0, 0.0, 1.0);

void main() {

  diffuseColor = texture(colorMap, v_texCoords);

  vec3 normal = normalize(v_normal); 
  vec3 lightDir = normalize(v_eyeSpaceLightPosition - v_eyeSpaceVertPosition);

  float lambertian = max(dot(lightDir,normal), 0.0);
  float specular = 0.0;

  if(lambertian > 0.0) {

    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 viewDir = normalize(v_eyeSpaceVertPosition - v_cameraPosition);

    float specAngle = max(dot(reflectDir, viewDir), 0.0);
    specular = pow(specAngle, 4.0);

    // the exponent controls the shininess (try mode 2)
    specular = pow(specAngle, 16.0);

    // according to the rendering equation we would need to multiply
    // with the the "lambertian", but this has little visual effect
    specular *= lambertian;

    // switch to mode 4 to turn off the specular component
    //if(mode == 4) specular *= 0.0;

  }
  vec3 lighting = lambertian * diffuseColor.xyz + specular * specColor.xyz;

  fcolor = vec4(lighting, 1.0);
}
