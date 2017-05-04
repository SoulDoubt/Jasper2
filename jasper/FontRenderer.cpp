
#include "FontRenderer.h"

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"


#include "Texture.h"
#include <GL/glew.h>
#include <fstream>
#include "GLError.h"


using namespace std;

namespace Jasper
{

using uchar8 = unsigned char;

int* AlphaBitmapToColors(uchar8* alphaBitmap, int width, int height)
{
    int* outBitmap = new int[width * height];
    int i = (width * height) - 1;
    int j = 0;
    for (; i >= 0; i--) {
        //for (int i = 0; i < width * height; i++){
        uchar8 tval = alphaBitmap[i];
        int ival = (tval << 24) | (tval << 16) | (tval << 8) | (tval << 0);
        outBitmap[i] = ival;
        ++j;
    }
    return outBitmap;
}


FontRenderer::FontRenderer()
    : m_vertexBuffer(GLBuffer::BufferType::VERTEX),
      m_indexBuffer(GLBuffer::BufferType::INDEX)
{
    m_vertexBuffer.SetUsage(GLBuffer::Usage::DynamicDraw);
    m_indexBuffer.SetUsage(GLBuffer::Usage::DynamicDraw);
    m_shader = make_unique<FontShader>();
}


FontRenderer::~FontRenderer()
{
    if (m_vertexBuffer.IsCreated())
        m_vertexBuffer.Destroy();
    if (m_indexBuffer.IsCreated())
        m_indexBuffer.Destroy();
}



void FontRenderer::RenderText(const string& text, float x, float y)
{
    Vector4 color = { m_color.x, m_color.y, m_color.z, 1.0f };
    std::vector<Vertex> verts;
    verts.reserve(text.length() * 4);

    for (char c : text) {
        if (c >= 32 && c < 128) {
            stbtt_aligned_quad q;
            stbtt_GetPackedQuad(m_packData, 512, 512, c, &x, &y, &q, 0);
            Vertex v0 = Vertex(q.x1, q.y1, 0.f, q.s1, q.t1);
            v0.Color = color;
            Vertex v1 = Vertex(q.x1, q.y0, 0.f, q.s1, q.t0);
            v1.Color = color;
            Vertex v2 = Vertex(q.x0, q.y0, 0.f, q.s0, q.t0);
            v2.Color = color;
            Vertex v3 = Vertex(q.x0, q.y1, 0.f, q.s0, q.t1);
            v3.Color = color;
            verts.push_back(v0);
            verts.push_back(v1);
            verts.push_back(v2);
            verts.push_back(v3);
        }
    }

    m_shader->Bind();
    glBindVertexArray(m_vao);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, m_texID);
    m_shader->SetModelViewProjectionMatrix(m_matrix);
    m_vertexBuffer.Bind();
    m_vertexBuffer.Allocate(verts.data(), verts.size() * sizeof(Vertex));
    glDrawArrays(GL_QUADS, 0, verts.size());
    glBindTexture(GL_TEXTURE_2D, 0);
    m_shader->Release();
    glBindVertexArray(0);
}

std::unique_ptr<Texture> FontRenderer::GetTextureAtlas()
{

    std::ifstream file("../fonts/Roboto-Medium.ttf", std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (file.read(buffer.data(), size)) {
        int h = 512;
        int w = 512;
        file.close();

        uchar8* tempBitmap = new uchar8[w * h];
        stbtt_pack_context ctx;
        stbtt_PackBegin(&ctx, tempBitmap, w, h, 0, 1, nullptr);
        stbtt_PackFontRange(&ctx, (uchar8*)buffer.data(), 0, 16.0f, 0, 128, m_packData);
        stbtt_PackEnd(&ctx);
        buffer.clear();
        auto tex = std::make_unique<Texture>();
        tex->Load(tempBitmap, w, h, GL_ALPHA);
        delete[] tempBitmap;
        return std::move(tex);
    }
}


void FontRenderer::Initialize()
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    m_texture = GetTextureAtlas();

    m_texID = m_texture->TextureID();
    int positionLocation = m_shader->PositionAttributeLocation();
    int texLocation = m_shader->TexCoordAttributeLocation();
    int colorLocation = m_shader->ColorsAttributeLocation();
    m_vertexBuffer.Create();
    m_vertexBuffer.Bind();

    size_t positionOffset = offsetof(Vertex, Position);
    size_t texOffset = offsetof(Vertex, TexCoords);
    size_t colorOffset = offsetof(Vertex, Color);

    m_shader->SetAttributeArray(positionLocation, GL_FLOAT, (void*)positionOffset, 3, sizeof(Vertex));
    m_shader->SetAttributeArray(texLocation, GL_FLOAT, (void*)texOffset, 2, sizeof(Vertex));
    m_shader->SetAttributeArray(colorLocation, GL_FLOAT, (void*)colorOffset, 4, sizeof(Vertex));
    m_vertexBuffer.Release();
    m_shader->Release();
    glBindVertexArray(0);

}
}
