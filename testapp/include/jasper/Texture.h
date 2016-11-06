#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "Common.h"
#include <string>

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

    virtual unsigned TextureID() const {
        return m_textureID;
    }

    virtual void Bind();
    virtual void Release();

private:

    unsigned m_textureID;
};

class CubemapTexture : public Texture
{

public:

    bool Load(const std::string& posx,
              const std::string& negx,
              const std::string& posy,
              const std::string& negy,
              const std::string& posz,
              const std::string& negz);

    virtual unsigned TextureID() const override {
        return m_cubemapID;
    }

    ~CubemapTexture();

    virtual void Bind()override;
    virtual void Release() override;

private:

    unsigned m_cubemapID;



};

}
#endif
