#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "vector.h"
#include "Texture.h"
#include "Shader.h"
#include <memory>

namespace Jasper
{

    
inline bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}
    
class Material
{

public:

    enum MATERIAL_FLAGS {
        HAS_COLOR_MAP      = 0x0001,
        HAS_NORMAL_MAP     = 0x0002,
        HAS_SPECULAR_MAP   = 0x0004,
        USE_MATERIAL_COLOR = 0x0008,
        USE_VERTEX_COLORS  = 0x0010,
        IS_TRANSPARENT     = 0x0020,
        IS_CUBEMAP         = 0x0040
    };
    
    uint Flags = 0x0000;

    private:
    

    //Shader* m_shader;
    std::unique_ptr<Texture> m_texture;
    int m_diffuseTextureLocation = -1;
    std::unique_ptr<CubemapTexture> m_cubemap;
    int m_cubemapTextureLocation = -1;
    std::unique_ptr<Texture> m_normalMap;
    int m_normalMapTextureLocation = -1;
    std::unique_ptr<Texture> m_specularMap;
    int m_specularMapTextureLocation = -1;

    std::string m_name;

    std::string m_diffuseTextureFileName;


    void BindTextures(Shader* shader) {
        const Shader::TextureUnitLocations locs = shader->GetTextureUniformLocations();
        
        if (Flags & HAS_NORMAL_MAP) {                                    
            glUniform1i(locs.Maps.has_normal_map, 1);            
            glUniform1i(locs.Maps.normalmap, 1);            
            glActiveTexture(GL_TEXTURE0 + 1);
            m_normalMap->Bind();
        } else {            
            glUniform1i(locs.Maps.has_normal_map, 0);
            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        if (Flags & HAS_SPECULAR_MAP) {            
            glUniform1i(locs.Maps.has_specular_mao, 1);            
            glUniform1i(locs.Maps.specularmap, 2);            
            glActiveTexture(GL_TEXTURE0 + 2);
            m_specularMap->Bind();
        } else {            
            glUniform1i(locs.Maps.has_specular_mao, 0);
            glActiveTexture(GL_TEXTURE0 + 2);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        if (Flags & HAS_COLOR_MAP) {
            glUniform1i(locs.Maps.has_color_map, 1);
            glUniform1i(locs.Maps.colormap, 0);            
            glActiveTexture(GL_TEXTURE0 + 0);
            m_texture->Bind();
        } else {
            glUniform1i(locs.Maps.has_color_map, 0);
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    void ReleaseTextures() {
        /*glActiveTexture(GL_TEXTURE0);
        if (m_texture) m_texture->Release();
        glActiveTexture(GL_TEXTURE0 + 1);
        if (m_normalMap) m_normalMap->Release();
        glActiveTexture(GL_TEXTURE0 + 2);
        if (m_specularMap) m_specularMap->Release();*/
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0 + 2);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

public:
    //explicit Material(Shader* shader);
    //Material(Shader* shader, const std::string& name);
    Material(const std::string& name);
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

    bool IsTransparent = false;

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
        if (ends_with(filename, ".dds")){
            tex->LoadDDS(filename, true);
        } else {
            tex->LoadSRGBA(filename);
        }                
        m_texture = std::move(tex);
        m_diffuseTextureFileName = filename;
        Flags |= HAS_COLOR_MAP;
    }

    void SetTextureDiffuse(std::unique_ptr<Texture> tex) {
        m_texture = std::move(tex);
        Flags |= HAS_COLOR_MAP;
    }

    void SetTextureNormalMap(std::string filename) {
        auto tex = std::make_unique<Texture>();
        tex->Load(filename);
        m_normalMap = std::move(tex);
        Flags |= HAS_NORMAL_MAP;
    }

    void SetTextureNormalMap(std::unique_ptr<Texture> tex) {
        m_normalMap = std::move(tex);
        Flags |= HAS_NORMAL_MAP;
    }

    void SetTextureSpecularMap(std::string filename) {
        auto tex = std::make_unique<Texture>();
        tex->Load(filename);
        m_specularMap = std::move(tex);
        Flags |= HAS_SPECULAR_MAP;
    }

    void SetTextureSpecularMap(std::unique_ptr<Texture> tex) {
        m_specularMap = std::move(tex);
        Flags |= HAS_SPECULAR_MAP;
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
        Flags |= IS_CUBEMAP;
    }

//	Shader* GetShader() {
//		return m_shader;
//	}

//    void SetShader(Shader* shader){
//        m_shader = shader;
//    }

    std::string DiffuseTextureFilename() const {
        return m_diffuseTextureFileName;
    }

//	void Bind() {
//		//m_shader->Bind();
//		this->BindTextures();
//		m_isBound = true;
//	}

    void Bind(Shader* shader) {
        //shader->Bind();
        this->BindTextures(shader);
        m_isBound = true;
    }

    void Release() {
        //shader->Release();
        ReleaseTextures();
        m_isBound = false;
    }

//	void Release() {
//		m_shader->Release();
//		ReleaseTextures();
//		m_isBound = false;
//	}

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

//	std::string GetShaderName() {
//		return m_shaderName;
//	}
//
//	std::string GetShaderName() const {
//		return m_shaderName;
//	}



    /*void SetShader(Shader* shader) {
    	m_shader = shader;
    	m_shader->SetBoundMaterial(this);
    }*/

};
}
#endif // _MATERIAL_H_
