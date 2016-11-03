#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "vector.h"
#include "Texture.h"
#include "Shader.h"
#include <memory>

namespace Jasper {

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
    
    void BindTextures(){
        bool hasNormalMap = this->GetTextureNormalMap();
        auto shader  = this->GetShader();
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
            glBindTexture(GL_TEXTURE_2D, this->GetTextureNormalMap()->TextureID());
        } else {
            //glActiveTexture(GL_TEXTURE0 + 1);
            //glBindTexture(GL_TEXTURE_2D, 0);
        }

        if (this->GetTextureSpecularMap()) {
            int sl = glGetUniformLocation(shader->ProgramID(), "specularMap");
            if (sl > -1) {
                glUniform1i(sl, 2);
            }
            glActiveTexture(GL_TEXTURE0 + 2);
            glBindTexture(GL_TEXTURE_2D, this->GetTextureSpecularMap()->TextureID());
        }
        glActiveTexture(GL_TEXTURE0 + 0);
        uint texID = this->GetTexture2D()->TextureID();
        glBindTexture(GL_TEXTURE_2D, texID);
    }
        
    void ReleaseTextures(){
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);	
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0 + 2);
        glBindTexture(GL_TEXTURE_2D, 0);
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
    
    bool IsBound() const{
        return m_isBound;
    }

	Texture* GetTexture2D() {
		return m_texture.get();
	}

	Texture* GetTextureNormalMap() {
		return m_normalMap.get();
	}

	Texture* GetTextureSpecularMap() {
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
    
    void Bind(){
        m_shader->Bind();
        this->BindTextures();
        m_isBound = true;
    }
    
    void Release(){
        m_shader->Release();
        ReleaseTextures();
        m_isBound = false;
    }
    
    std::string GetName() { return m_name; }
    
        

	/*void SetShader(Shader* shader) {
		m_shader = shader;
		m_shader->SetBoundMaterial(this);
	}*/

};
}
#endif // _MATERIAL_H_