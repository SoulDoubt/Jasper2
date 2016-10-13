#ifndef _JASPER_SHADER_H_
#define _JASPER_SHADER_H_

#include "Common.h"
#include "matrix.h"
#include <GL/glew.h>
#include <vector>
#include "Lights.h"


/*

*/
namespace Jasper {

class Material;

struct DirectionalLightUniformLocations {
	bool isPopulated = false;
	int Color;	
	int Direction;	
	int AmbientIntensity;
	int DiffuseIntensity;
};


struct MaterialUniformLocations {
	bool isPopulated = false;
	int Ka;
	int Kd;
	int Ks;
	int Ns;
};

struct PointLightUniformLocations {
	bool isPopulated = false;
	int Color;
	int Position;
	int AmbientIntensity;
	int DiffuseIntensity;
	int ConstAttenuation;
	int LinearAtten;
	int ExpAtten;
	int Radius;
};

class Shader
{
public:
	enum ShaderType
	{
		VERTEX,
		FRAGMENT,
		GEOMETRY
	};

	Shader();
	Shader(std::string name);
	~Shader();
    
    std::string GetName() const { return m_name; }

	void AddShader(std::string filename, ShaderType t);
	void LinkShaderProgram();

	const GLuint ProgramID() const;

	bool Bind();
	void Release();

	int GetAttributeLocation(const char* name);
	void SetAttributeArray(const char* name, GLenum type, const void* values, int tupleSize, int stride);
	void SetAttributeArray(int location, GLenum num_type, void* offset, int num_components, int stride);

	virtual void Initialize();
	virtual void Destroy();

	// attribute lookups
	virtual uint PositionAttributeLocation();
	virtual uint NormalAttributeLocation();
	virtual uint TexCoordAttributeLocation();
	virtual uint ColorsAttributeLocation();
	virtual int TangentAttributeLocation();

	// 
	virtual void SetModelViewMatrix(const Matrix4& mvm);
	virtual void SetModelViewProjectionMatrix(const Matrix4& mvp);
	virtual void SetNormalMatrix(const Matrix3& normal);
	virtual void SetModelMatrix(const Matrix4& model);
	virtual void SetViewMatrix(const Matrix4& view);
	virtual void SetProjectionMatrix(const Matrix4& projection) {}

	virtual void GetDirectionalLightUniformLocations();
	virtual void SetDirectionalLightUniforms(const DirectionalLight* dl);	
	
	virtual void GetPointLightUniformLocations();
	virtual void SetPointLightUniforms(const PointLight* pl, const Vector3& eslp);

	virtual void GetMaterialUniformLocations();
	virtual void SetMaterialUniforms(const Material* m);

	virtual void SetTransformUniforms(const Transform & trans);

	void SetBoundMaterial(Material* m) {
		m_material = m;
	}

	virtual void SetCameraPosition(const Vector3& cp);



private:
	NON_COPYABLE(Shader);
	uint m_programID;
	std::vector<uint> m_shaders;
	GLboolean m_transpose = GL_FALSE;

	

protected:

	uint m_positionsAttribute = 0;
	uint m_normalsAttribute = 0;
	uint m_texCoordsAttribute = 0;
	uint m_colorsAttribute = 0;
	int m_tangentAttribute = -1;
	Material* m_material;
	std::string m_name;

	DirectionalLightUniformLocations m_dlus;
	PointLightUniformLocations m_plus;
	MaterialUniformLocations m_mus;

};

inline const uint Shader::ProgramID() const {
	return m_programID;
}





}

#endif // _SHADER_H_

