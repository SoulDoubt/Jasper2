#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Texture.h"
#include <GL/glew.h>
#include "GLError.h"

namespace Jasper
{

using namespace std;

Texture::Texture()
{
}


Texture::~Texture()
{
    glDeleteTextures(1, &m_textureID);
}

bool Texture::Load(std::string filename)
{
    int x, y, comp;

    GLenum force_linear = GL_SRGB_ALPHA;

    auto image = stbi_load(filename.c_str(), &x, &y, &comp, STBI_rgb_alpha);
    if (image) {
        printf("Loaded texture from: %s\n", filename.c_str());

        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, force_linear, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        GLERRORCHECK;
        stbi_image_free(image);
        return true;
    } else {
        printf("Failed to load texture from %s", filename.c_str());
        return false;
    }
}

bool Texture::Load(const unsigned char * data, int width, int height, GLenum format)
{
    if (data != nullptr) {
        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        GLERRORCHECK;

        return true;
    } else {
        return false;
    }
}

void Texture::Bind()
{
    glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void Texture::Release()
{
    glBindTexture(GL_TEXTURE_2D, m_textureID);
}



bool CubemapTexture::Load(const std::string & posx, const std::string & negx, const std::string & posy, const std::string & negy, const std::string & posz, const std::string & negz)
{
    string files[6] = { posx, negx, posy, negy, posz, negz };
    glGenTextures(1, &m_cubemapID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapID);

    for (int i = 0; i < 6; ++i) {
        int x, y, comp;
        auto image = stbi_load(files[i].c_str(), &x, &y, &comp, STBI_rgb_alpha);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB_ALPHA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        delete image;
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return true;

}

CubemapTexture::~CubemapTexture()
{
    if (m_cubemapID != 0)
        glDeleteTextures(1, &m_cubemapID);
}

void CubemapTexture::Bind()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapID);
}

void CubemapTexture::Release()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

}
