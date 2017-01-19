#ifndef _JASPER_GBUFFER_H_
#define _JASPER_GBUFFER_H_

//#define GBUFFER_POSITION_TEXTURE_UNIT 0
//#define GBUFFER_DIFFUSE_TEXTURE_UNIT  1
//#define GBUFFER_NORMAL_TEXTURE_UNIT   2
//#define GBUFFER_TEXCOORD_TEXTURE_UNIT 3
//#define GBUFFER_TEXTURE_TYPE_POSITION 0
//#define GBUFFER_TEXTURE_TYPE_DIFFUSE  1
//#define GBUFFER_TEXTURE_TYPE_NORMAL   2
//#define GBUFFER_TEXTURE_TYPE_TEXCOORD 3
//#define GBUFFER_TEXTURE_TYPE_SPECULAR 4
//#define GBUFFER_NUM_TEXTURES          5

namespace Jasper
{


class GBuffer
{
public:

    static const uint GBUFFER_POSITION_TEXTURE_UNIT = 0;
    static const uint GBUFFER_DIFFUSE_TEXTURE_UNIT  = 1;
    static const uint GBUFFER_NORMAL_TEXTURE_UNIT   = 2;
    static const uint GBUFFER_TEXCOORD_TEXTURE_UNIT = 3;
    static const uint GBUFFER_TEXTURE_TYPE_POSITION = 0;
    static const uint GBUFFER_TEXTURE_TYPE_DIFFUSE  = 1;
    static const uint GBUFFER_TEXTURE_TYPE_NORMAL   = 2;
    static const uint GBUFFER_TEXTURE_TYPE_TEXCOORD = 3;
    static const uint GBUFFER_TEXTURE_TYPE_SPECULAR = 4;
    static const uint GBUFFER_NUM_TEXTURES          = 5;


    GBuffer();

    ~GBuffer();

    bool Initialize(unsigned int WindowWidth, unsigned int WindowHeight);

    void BindForWriting();

    void BindForReading();

    void SetReadBuffer(uint textureType);

    uint gPosition, gDiffuse, gNormal, gTexCoords, gSpecular;

private:

    uint m_fbo;
    uint m_textures[GBUFFER_NUM_TEXTURES];
    uint m_depthTexture;
};

}
#endif // _JASPER_GBUFFER_H_
