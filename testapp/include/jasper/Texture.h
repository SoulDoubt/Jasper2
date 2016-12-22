#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "Common.h"
#include <string>
#include <vector>

namespace Jasper
{

class Texture
{
public:

    enum class WrapMode
    {
        REPEAT
    };

    Texture();
    ~Texture();

    bool Load(std::string filename);
    bool Load(const unsigned char* data, int width, int height, uint format);
    
    std::string GetFilename() const{
        return m_filename;
    }

    virtual unsigned TextureID() const {
        return m_textureID;
    }

    virtual void Bind();
    virtual void Release();

protected:
	int m_previouslyBound;

private:

    unsigned m_textureID;
    std::string m_filename;
};

class CubemapTexture : public Texture
{

public:

    bool Load(std::string posx,
              std::string negx,
              std::string posy,
              std::string negy,
              std::string posz,
              std::string negz);

    virtual unsigned TextureID() const override {
        return m_cubemapID;
    }

    ~CubemapTexture();

    virtual void Bind()override;
    virtual void Release() override;

	const std::vector<std::string> GetFileNames() const {
		return m_fileNames;
	}

private:

    unsigned m_cubemapID;
	std::vector<std::string> m_fileNames;
	



};

}
#endif
