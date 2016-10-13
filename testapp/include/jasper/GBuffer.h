#ifndef _JASPER_GBUFFER_H_
#define _JASPER_GBUFFER_H_

class GBuffer {
public:

	enum class TextureType {
		Position,
		Diffuse,
		Normal,
		TexCoord,
		Tangent,
		Bitangent,
		NUM_TEXTURES
	};

	explicit GBuffer(int width, int height);
	~GBuffer();

	void Initialize();
	void Destroy();

private:
	unsigned int m_fboID;
	unsigned int m_textureIDs[6];
	unsigned int m_depthTextureID;
	int m_height, m_width;
};

#endif // _JASPER_GBUFFER_H_


