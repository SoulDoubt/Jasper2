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
    m_filename = filename;
    int x, y, comp;

    GLenum force_linear = GL_SRGB_ALPHA;

    auto image = stbi_load(filename.c_str(), &x, &y, &comp, STBI_rgb_alpha);
    if (image) {
        printf("Loaded texture from: %s\n", filename.c_str());

		int format, type;
		GLenum texture_target = GL_TEXTURE_2D;
		glGetInternalformativ(texture_target, GL_RGBA8, GL_TEXTURE_IMAGE_FORMAT, 1, &format);
		glGetInternalformativ(texture_target, GL_RGBA8, GL_TEXTURE_IMAGE_TYPE, 1, &type);

        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, force_linear, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &m_previouslyBound);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void Texture::Release()
{
    glBindTexture(GL_TEXTURE_2D, m_previouslyBound);

}



bool CubemapTexture::Load(std::string posx, std::string negx, std::string posy, std::string negy, std::string posz, std::string negz)
{
	m_fileNames.emplace_back(move(posx));
	m_fileNames.emplace_back(move(negx));
	m_fileNames.emplace_back(move(posy));
	m_fileNames.emplace_back(move(negy));
	m_fileNames.emplace_back(move(posz));
	m_fileNames.emplace_back(move(negz));
    //string files[6] = { posx, negx, posy, negy, posz, negz };
    glGenTextures(1, &m_cubemapID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapID);

	int i = 0;
    for (const auto& file: m_fileNames) {
        int x, y, comp;
        auto image = stbi_load(file.c_str(), &x, &y, &comp, STBI_rgb_alpha);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB_ALPHA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        delete image;
		i++;
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
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &m_previouslyBound);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapID);
}

void CubemapTexture::Release()
{	
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_previouslyBound);
}

}
