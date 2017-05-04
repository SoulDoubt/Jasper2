#version 330 core

in vec3 position;
in vec2 texCoords;
in vec3 normal;
in vec4 tangent;
in vec3 bitangent;                                            

uniform mat4 mvpMatrix;
uniform mat3 normalMatrix;
uniform mat4 modelMatrix;

out vec2 v_texCoords;
out vec3 v_normal;
out vec4 v_fragPosition;
out mat3 v_tbnMatrix;


                                        
void main()
{       	
	v_texCoords = texCoords;
	v_fragPosition = modelMatrix * vec4( position, 1.0 );
	vec3 tang = normalize(tangent.xyz - dot(tangent.xyz, normal) * normal);
	v_normal = normalMatrix * normal;

	vec3 n = normalize( ( modelMatrix * vec4(normal, 0.0 )).xyz );
	vec3 b = normalize( ( modelMatrix * vec4(bitangent, 0.0)).xyz );
	vec3 t = normalize( ( modelMatrix * vec4(tang, 1.0) ).xyz );
	v_tbnMatrix = mat3( t, b, n );

	gl_Position = mvpMatrix * vec4( position, 1.0f );
}
