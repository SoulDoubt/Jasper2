#ifndef _JASPER_GBUFFER_H_
#define _JASPER_GBUFFER_H_


namespace Jasper
{


class GBuffer
{
public:

    static const uint GBUFFER_POSITION_TEXTURE_UNIT = 0;
    static const uint GBUFFER_DIFFUSE_TEXTURE_UNIT  = 1;
    static const uint GBUFFER_NORMAL_TEXTURE_UNIT   = 2;
    static const uint GBUFFER_SPECULAR_TEXTURE_UNIT = 3;
    static const uint GBUFFER_FINAL_TEXTURE_UNIT    = 4;
    static const uint GBUFFER_TEXTURE_TYPE_POSITION = 0;
    static const uint GBUFFER_TEXTURE_TYPE_DIFFUSE  = 1;
    static const uint GBUFFER_TEXTURE_TYPE_NORMAL   = 2;
    static const uint GBUFFER_TEXTURE_TYPE_SPECULAR = 3;
    static const uint GBUFFER_TEXTURE_TYPE_FINAL    = 4;
    static const uint GBUFFER_NUM_TEXTURES          = 5;


    GBuffer();

    ~GBuffer();

    bool Initialize(unsigned int WindowWidth, unsigned int WindowHeight);

    void BindForWriting();

    void BindForReading();
    
    void BindForGeometryPass();
    void BindForPointLightPass();
    void BindForStencilPass();
    void BindForDirectionalLightPass();
    void BindForFinalPass();

    void SetReadBuffer(uint textureType);

    uint gPosition, gDiffuse, gNormal, gSpecular, gFinal;
    
    uint ID() const {
        return m_fbo;
    }

private:

    uint m_fbo;
    uint m_textures[GBUFFER_NUM_TEXTURES];
    uint m_depthTexture;
};

}
#endif // _JASPER_GBUFFER_H_
