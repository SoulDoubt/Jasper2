#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "vector.h"
#include "Texture.h"
#include "Shader.h"
#include <memory>

namespace Jasper
{

class Material
{

private:

	Shader* m_shader;
	std::unique_ptr<Texture> m_texture;
	std::unique_ptr<CubemapTexture> m_cubemap;
	std::unique_ptr<Texture> m_normalMap;
	std::unique_ptr<Texture> m_specularMap;

	std::string m_name;

	std::string m_diffuseTextureFileName;

	std::string m_shaderName;

	void BindTextures() {
		bool hasNormalMap = this->GetTextureNormalMap();
		auto shader = this->GetShader();
		if (hasNormalMap) {
			int dl = glGetUniformLocation(shader->ProgramID(), "colorMap");
			int nl = glGetUniformLocation(shader->ProgramID(), "normalMap");
			if (dl > -1) {
				glUniform1i(dl, 0);
			}
			if (nl > -1) {
				glUniform1i(nl, 1);
			}
			glActiveTexture(GL_TEXTURE0 + 1);
			m_normalMap->Bind();

		}
		if (this->GetTextureSpecularMap()) {
			int sl = glGetUniformLocation(shader->ProgramID(), "specularMap");
			if (sl > -1) {
				glUniform1i(sl, 2);
			}
			glActiveTexture(GL_TEXTURE0 + 2);
			m_specularMap->Bind();
		}
		if (this->GetTextureDiffuseMap()) {
			glActiveTexture(GL_TEXTURE0 + 0);
			m_texture->Bind();
		}
	}

	void ReleaseTextures() {
		glActiveTexture(GL_TEXTURE0);
		if (m_texture) m_texture->Release();
		glActiveTexture(GL_TEXTURE0 + 1);
		if (m_normalMap) m_normalMap->Release();
		glActiveTexture(GL_TEXTURE0 + 2);
		if (m_specularMap) m_specularMap->Release();
	}

public:
	//explicit Material(Shader* shader);
	Material(Shader* shader, const std::string& name);
	~Material();
	// onto a material we will attach a shader...
	// a mesh will have a material assigned
	// meshes handle their various geometry buffers

	/* GLSL Format
	struct material{
		vec3 ka;
		vec3 kd;
		vec3 ks;
		float ns;
	};
	*/

	bool m_isBound = false;

	Vector3 Ambient;
	Vector3 Diffuse;
	Vector3 Specular;
	float Shine;

	bool IsBound() const {
		return m_isBound;
	}

	Texture* GetTextureDiffuseMap() {
		return m_texture.get();
	}

	const Texture* GetTextureDiffuseMap() const {
		return m_texture.get();
	}

	Texture* GetTextureNormalMap() {
		return m_normalMap.get();
	}

	const Texture* GetTextureNormalMap() const {
		return m_normalMap.get();
	}

	Texture* GetTextureSpecularMap() {
		return m_specularMap.get();
	}

	const Texture* GetTextureSpecularMap() const {
		return m_specularMap.get();
	}

	void SetTextureDiffuse(std::string filename) {
		auto tex = std::make_unique<Texture>();
		tex->Load(filename);
		m_texture = std::move(tex);
		m_diffuseTextureFileName = filename;
	}

	void SetTextureDiffuse(std::unique_ptr<Texture> tex) {
		m_texture = std::move(tex);
	}

	void SetTextureNormalMap(std::string filename) {
		auto tex = std::make_unique<Texture>();
		tex->Load(filename);
		m_normalMap = std::move(tex);
	}

	void SetTextureNormalMap(std::unique_ptr<Texture> tex) {
		m_normalMap = std::move(tex);
	}

	void SetTextureSpecularMap(std::string filename) {
		auto tex = std::make_unique<Texture>();
		tex->Load(filename);
		m_specularMap = std::move(tex);
	}

	void SetTextureSpecularMap(std::unique_ptr<Texture> tex) {
		m_specularMap = std::move(tex);
	}

	CubemapTexture* GetCubemapTexture() {
		return m_cubemap.get();
	}

	const CubemapTexture* GetCubemapTexture() const {
		return m_cubemap.get();
	}

	void SetCubemapTextures(const std::string& posx,
		const std::string& negx, const std::string& posy,
		const std::string& negy, const std::string& posz,
		const std::string& negz) {
		auto cm = std::make_unique<CubemapTexture>();
		cm->Load(posx, negx, posy, negy, posz, negz);
		m_cubemap = std::move(cm);
	}

	Shader* GetShader() {
		return m_shader;
	}

	std::string DiffuseTextureFilename() const {
		return m_diffuseTextureFileName;
	}

	void Bind() {
		m_shader->Bind();
		this->BindTextures();
		m_isBound = true;
	}

	void Release() {
		m_shader->Release();
		ReleaseTextures();
		m_isBound = false;
	}

	std::string GetName() {
		return m_name;
	}

	std::string GetName() const {
		return m_name;
	}

	void SetName(std::string name) {
		m_name = std::move(name);
	}

	bool ShowGui();
	void Serialize(std::ofstream& ofs) const;

	std::string GetShaderName() {
		return m_shaderName;
	}

	std::string GetShaderName() const {
		return m_shaderName;
	}



	/*void SetShader(Shader* shader) {
		m_shader = shader;
		m_shader->SetBoundMaterial(this);
	}*/

};
}
#endif // _MATERIAL_H_
