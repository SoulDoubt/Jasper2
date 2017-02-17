
#version 330

in vec3 position;
in vec2 texCoords;
in vec4 color;
in vec3 normal;
in vec4 tangent;
in vec3 bitangent;
in ivec4 boneIds;
in vec4 boneWeights;

uniform mat4[64] boneTransforms;


uniform mat4 mvMatrix;
uniform mat4 mvpMatrix;
uniform mat3 normalMatrix;
uniform mat4 modelMatrix;

uniform vec3 lightPosition;
uniform vec3 cameraPosition;

smooth out vec2 v_texCoords;
smooth out vec3 v_normal;
smooth out vec4 v_vertColor;
smooth out vec4 v_fragPosition;
//out vec3 v_lightDirection;
out mat3 v_tbnMatrix;
//out vec3 v_lightPosition;


void main()
{
	mat4 boneTransform;
	//if (boneIds.x > -1){
		boneTransform = boneTransforms[boneIds.x] * boneWeights.x;
	//}
	//if (boneIds.y > -1){
		boneTransform += boneTransforms[boneIds.y] * boneWeights.y;
	//}
	//if (boneIds.z > -1){
		boneTransform     += boneTransforms[boneIds.z] * boneWeights.z;
	//}
	//if (boneIds.w > -1){
		boneTransform     += boneTransforms[boneIds.w] * boneWeights.w;
	//}

	vec4 posb = boneTransform * vec4( position, 1.0 );
	vec4 normalb = boneTransform * vec4( normal, 1.0 );

	v_vertColor = color;
	v_texCoords = texCoords;
	v_fragPosition = modelMatrix * posb;
	v_normal = normalMatrix * normalb.xyz;

	vec3 n = normalize( ( modelMatrix * vec4(normal, 0.0 )).xyz );
	vec3 b = normalize( ( modelMatrix * vec4(bitangent, 0.0)).xyz );
	vec3 t = normalize( ( modelMatrix * tangent ).xyz );
	v_tbnMatrix = mat3( t, b, n );
	
	gl_Position = mvpMatrix * posb;
}
