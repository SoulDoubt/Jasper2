#version 330

in vec3 position;
in vec2 texCoords;
in vec4 color;
in vec3 normal;
in vec4 tangent;

uniform mat4 mvMatrix, mvpMatrix;
uniform mat4 viewMatrix;
uniform mat3 normalMatrix;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;

out vec3 v_normal;
out vec3 v_eyeSpaceVertPosition;
out vec3 v_eyeSpaceLightPosition;
out mat3 v_tbnMatrix;
out vec2 v_texCoords;
out vec3 v_cameraPosition;

void main(){

	v_texCoords = texCoords;

    gl_Position = mvpMatrix * vec4( position, 1.0 );
    vec4 vertPos4 = mvMatrix * vec4( position, 1.0 );
    v_eyeSpaceVertPosition = vec3( vertPos4 ) / vertPos4.w;

    vec3 n = normalize( normalMatrix * normal );
	vec3 t = normalize( normalMatrix * tangent.xyz );
	vec3 b = normalize( normalMatrix * ( cross( normal, tangent.xyz ) * tangent.w ) );
	mat3 tbn = transpose( mat3( t, b, n ) );
	
	v_tbnMatrix = tbn;
    v_normal = n;

    v_eyeSpaceLightPosition = (viewMatrix * vec4(lightPosition, 1.0)).xyz;
    v_cameraPosition = cameraPosition ;//(viewMatrix * vec4(v_cameraPosition, 1.0)).xyz;

}
