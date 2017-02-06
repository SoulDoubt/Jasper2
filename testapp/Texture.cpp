#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Texture.h"
#include <GL/glew.h>
#include "GLError.h"

#include <fstream>

#define STB_DXT_IMPLEMENTATION
#include "stb_dxt.h"

#define STBGL_COMPRESSED_RGBA_S3TC_DXT5   0x83F3

namespace Jasper
{

typedef uint8_t uint8;

using namespace std;

static void stbgl__convert(uint8 *p, uint8 *q, int n, int input_desc, uint8 *end);
static void stbgl__compress(uint8 *p, uint8 *rgba, int w, int h, int output_desc, uint8 *end);

// delete the pointer returned by this
static uint8* CompressTexture(uint8* image_data, int w, int h, int comp, int* size)
{
    uint8* rgba = nullptr;
    if (comp == 3) {
        // convert the rgb to rgba for compression`
        rgba = new uint8[w*h*4];
        if (!rgba) return 0;
        uint8* end_rgba = rgba+w*h*4;
        stbgl__convert(rgba, image_data, w * h, GL_RGBA, end_rgba);
    }
    
    // try to compress the image...
    int osize = 16; // we're going to turn alpha on in the compressor
    uint8* out = new uint8[(w+3)*(h+3)/16*osize]; // enough storage for the s3tc data
    if (!out) return 0;
    uint8* end_out = out + ((w+3)*(h+3))/16*osize;

    stbgl__compress(out, rgba ? rgba : image_data, w, h, STBGL_COMPRESSED_RGBA_S3TC_DXT5, end_out);
    *size = ((w+3)&~3)*((h+3)&~3)/16*osize;
    delete[] rgba;
    return out;
}


Texture::Texture()
{
}


Texture::~Texture()
{
    glDeleteTextures(1, &m_textureID);
}

const unsigned int FOURCC_DXT1 = 0x31545844; //(MAKEFOURCC('D','X','T','1'))
const unsigned int FOURCC_DXT3 = 0x33545844; //(MAKEFOURCC('D','X','T','3'))
const unsigned int FOURCC_DXT5 = 0x35545844; //(MAKEFOURCC('D','X','T','5'))

void Texture::LoadDDS(const std::string& filename, bool linear)
{
    m_filename = filename;
    std::ifstream input( filename, std::ios::binary );
    // copies all data into buffer
    std::vector<char> buffer((
            std::istreambuf_iterator<char>(input)), 
            (std::istreambuf_iterator<char>()));
    
    unsigned char* data = (unsigned char*)buffer.data();
            
    unsigned int height = *(unsigned int*)&(data[12]);
    unsigned int width = *(unsigned int*)&(data[16]);
    unsigned int mipMapCount = *(unsigned int*)&(data[28]);
    unsigned int fourCC = *(unsigned int*)&(data[84]);
    unsigned int format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;

    switch(fourCC)
    {
        case FOURCC_DXT1: format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; break;
        case FOURCC_DXT3: format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; break;
        case FOURCC_DXT5: format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; break;
    }

    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 124 + 4;

    for(unsigned int level = 0; level < mipMapCount && (width || height); ++level)
    {
        unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
        //if(size == 0) break; // neccessary?
        glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, &data[offset]);

        offset += size;
        width = std::max((uint)1, width/(uint)2);
        height = std::max((uint)1, height/(uint)2);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

bool Texture::LoadSRGBA(std::string filename)
{    
    
    m_filename = filename;
    
    
    int w, h, comp;
    
    int formatCount;    
    glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, & formatCount);
    int* formatArray = new int[formatCount];
    glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, formatArray);
    for (int i = 0; i < formatCount; ++i){
        int format = formatArray[i];
        printf("Supported Compressed Texture Format: %#04X\n", format);
    }
    

    GLenum force_linear = GL_SRGB_ALPHA;

    uint8* rgba = nullptr;
    uint8* image = stbi_load(filename.c_str(), &w, &h, &comp, STBI_rgb_alpha);
    if (image) {
        printf("Loaded texture from: %s\n", filename.c_str());
        //int compressedSize = 0;
        //uint8* compressed = CompressTexture(image, w, h, comp, &compressedSize);

        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        //glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, compressedSize , compressed);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        GLERRORCHECK;
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        GLERRORCHECK;
        //delete[] compressed;
        
        stbi_image_free(image);
        return true;
    } else {
        printf("Failed to load texture from %s", filename.c_str());
        return false;
    }
}

bool Texture::LoadSRGBA(const unsigned char * data, int width, int height, GLenum format)
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
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


// STB COMPRESSION



static void stbgl__compress(uint8 *p, uint8 *rgba, int w, int h, int output_desc, uint8 *end)
{
    int i,j,y,y2;
    int alpha = (output_desc == STBGL_COMPRESSED_RGBA_S3TC_DXT5);
    for (j=0; j < w; j += 4) {
        int x=4;
        for (i=0; i < h; i += 4) {
            uint8 block[16*4];
            if (i+3 >= w) x = w-i;
            for (y=0; y < 4; ++y) {
                if (j+y >= h) break;
                memcpy(block+y*16, rgba + w*4*(j+y) + i*4, x*4);
            }
            if (x < 4) {
                switch (x) {
                case 0:
                    assert(0);
                case 1:
                    for (y2=0; y2 < y; ++y2) {
                        memcpy(block+y2*16+1*4, block+y2*16+0*4, 4);
                        memcpy(block+y2*16+2*4, block+y2*16+0*4, 8);
                    }
                    break;
                case 2:
                    for (y2=0; y2 < y; ++y2)
                        memcpy(block+y2*16+2*4, block+y2*16+0*4, 8);
                    break;
                case 3:
                    for (y2=0; y2 < y; ++y2)
                        memcpy(block+y2*16+3*4, block+y2*16+1*4, 4);
                    break;
                }
            }
            y2 = 0;
            for(; y<4; ++y,++y2)
                memcpy(block+y*16, block+y2*16, 4*4);
            stb_compress_dxt_block(p, block, alpha, 10);
            p += alpha ? 16 : 8;
        }
    }
    assert(p <= end);
}

static void stbgl__convert(uint8 *p, uint8 *q, int n, int input_desc, uint8 *end)
{
    int i;
    switch (input_desc) {
    case GL_RED:
    case GL_LUMINANCE:
        for (i=0; i < n; ++i,p+=4) p[0] = p[1] = p[2] = q[0], p[3]=255, q+=1;
        break;
    case GL_ALPHA:
        for (i=0; i < n; ++i,p+=4) p[0] = p[1] = p[2] = 0, p[3] = q[0], q+=1;
        break;
    case GL_LUMINANCE_ALPHA:
        for (i=0; i < n; ++i,p+=4) p[0] = p[1] = p[2] = q[0], p[3]=q[1], q+=2;
        break;
    case GL_RGB:
        for (i=0; i < n; ++i,p+=4) p[0]=q[0],p[1]=q[1],p[2]=q[2],p[3]=255,q+=3;
        break;
    case GL_RGBA:
        memcpy(p, q, n*4);
        break;
    case GL_INTENSITY:
        for (i=0; i < n; ++i,p+=4) p[0] = p[1] = p[2] = p[3] = q[0], q+=1;
        break;
    }
    assert(p <= end);
}

}
