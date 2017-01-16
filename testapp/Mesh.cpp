#include <GL/glew.h>
#include "Mesh.h"
#include "Shader.h"
#include "GameObject.h"
#include "GLError.h"
#include "Material.h"
#include <AssetSerializer.h>

#include "ForsythIndexOptimizer.h"

namespace Jasper
{
using namespace std;


Mesh::Mesh(const std::string& name) : Component(name),
    m_vertexBuffer(GLBuffer::BufferType::VERTEX),
    m_texCoordBuffer(GLBuffer::BufferType::VERTEX),
    m_normalBuffer(GLBuffer::BufferType::VERTEX),
    m_tangentBuffer(GLBuffer::BufferType::VERTEX),
    m_bitangentBuffer(GLBuffer::BufferType::VERTEX),
    m_indexBuffer(GLBuffer::BufferType::INDEX)
{
    //Initialize();
    //CalculateExtents();
    Color = {0.f, 0.f, 0.f, 0.f};
}


Mesh::~Mesh()
{
    Destroy();
}

void Mesh::OptimizeIndices()
{
    uint16_t* oldIndices = new uint16_t[Indices.size()];
    uint16_t* newIndices = new uint16_t[Indices.size()];
    uint indexSize = Indices.size();
    printf("Old Indices: \n");
    for (uint j = 0; j < indexSize; ++j) {
        uint16_t idx = (uint16_t)Indices[j];
        oldIndices[j] = idx;
        //printf("%d, ", idx);
    }
    uint cacheSize = 64;
    uint vertexCount = Positions.size();
    Forsyth::OptimizeFaces(oldIndices, indexSize, vertexCount, newIndices, cacheSize);
    Indices.clear();
    Indices.reserve(indexSize);
    printf("New Indices: \n");
    for (uint j = 0; j < indexSize; j++) {
        uint newIdx = (uint)newIndices[j];
        Indices.push_back(newIdx);
        //printf("%d, ", newIdx);
    }
    delete[] oldIndices;
    delete[] newIndices;
}

void Mesh::Initialize()
{
    GLERRORCHECK;
    //OptimizeIndices();

    //m_mesh->Initialize();
    // gather mesh data and create GL Buffers and such for future rendering...
    assert(m_material);
    //assert(m_mesh);

    // create a VAO first
    glGenVertexArrays(1, &m_vaoID);
    glBindVertexArray(m_vaoID);
    //GLERRORCHECK;

    m_elementCount = this->Indices.size();
    auto shader = m_material->GetShader();
    const int positionLocation = shader->PositionAttributeLocation();
    const int normalLocation = shader->NormalAttributeLocation();
    const int texLocation = shader->TexCoordAttributeLocation();
    const int tangentLocation = shader->TangentAttributeLocation();
    const int bitangentLocation = shader->BitangentAttributeLocation();
    const int colorLocation = shader->ColorsAttributeLocation();


    m_vertexBuffer.Create();
    m_vertexBuffer.Bind();
    m_vertexBuffer.Allocate(Positions.data(), Positions.size() * sizeof(Vector3));
    shader->SetAttributeArray(positionLocation, GL_FLOAT, (void*)0, 3, 0);
    m_indexBuffer.Create();
    m_indexBuffer.Bind();
    m_indexBuffer.Allocate(Indices.data(), Indices.size() * sizeof(GLuint));
    if (normalLocation > -1) {
        m_normalBuffer.Create();
        m_normalBuffer.Bind();
        m_normalBuffer.Allocate(Normals.data(), Normals.size() * sizeof(Vector3));
        shader->SetAttributeArray(normalLocation, GL_FLOAT, (void*)0, 3, 0);
    }
    if (texLocation > -1) {
        m_texCoordBuffer.Create();
        m_texCoordBuffer.Bind();
        m_texCoordBuffer.Allocate(TexCoords.data(), TexCoords.size() * sizeof(Vector2));
        shader->SetAttributeArray(texLocation, GL_FLOAT, (void*)0, 2, 0);
    }
    if (tangentLocation > -1) {
        m_tangentBuffer.Create();
        m_tangentBuffer.Bind();
        m_tangentBuffer.Allocate(Tangents.data(), Tangents.size() * sizeof(Vector4));
        shader->SetAttributeArray(tangentLocation, GL_FLOAT, (void*)0, 4, 0);
    }
    if (bitangentLocation > -1) {
        m_bitangentBuffer.Create();
        m_bitangentBuffer.Bind();
        m_bitangentBuffer.Allocate(Bitangents.data(), Bitangents.size() * sizeof(Vector3));
        shader->SetAttributeArray(bitangentLocation, GL_FLOAT, (void*)0, 3, 0);
    }
    if (this->Color != Vector4(0.f, 0.f, 0.f, 0.f)) {
        m_colorBuffer.Create();
        int cc = Positions.size();
        Vector4* colorData = new Vector4[cc];
        for (int i = 0; i < cc; ++i) {
            colorData[i] = Color;
        }
        m_colorBuffer.Bind();
        m_colorBuffer.Allocate(colorData, cc * sizeof(Vector4));
        delete[] colorData;
        shader->SetAttributeArray(colorLocation, GL_FLOAT, 0, 4, 0);
    }

    glBindVertexArray(0);
    GLERRORCHECK;
}

void Mesh::Serialize(std::ofstream& ofs) const
{
    using namespace AssetSerializer;
    Component::Serialize(ofs);
    auto mt = GetMeshType();
    ofs.write(ConstCharPtr(&mt), sizeof(mt));
    if (GetMeshType() == MeshType::Arbitrary) {
        SerializeMesh(ofs, this);
    }
}

void Mesh::Destroy()
{
    //Vertices.clear();
    Positions.clear();
    Normals.clear();
    TexCoords.clear();
    Colors.clear();
    Tangents.clear();
    Bitangents.clear();
    Indices.clear();

    m_vertexBuffer.Destroy();
    m_indexBuffer.Destroy();
    if (m_texCoordBuffer.IsCreated())
        m_texCoordBuffer.Destroy();
    if (m_normalBuffer.IsCreated())
        m_normalBuffer.Destroy();
    if (m_tangentBuffer.IsCreated())
        m_tangentBuffer.Destroy();
    if (m_bitangentBuffer.IsCreated())
        m_bitangentBuffer.Destroy();
    if (m_colorBuffer.IsCreated())
        m_colorBuffer.Destroy();
    //Vertices.clear();
}

void Mesh::FlipTextureCoords()
{
    for (auto& tc : TexCoords) {
        tc.y *= -1;
    }
}

void Mesh::CalculateFaceNormals()
{

    if (Normals.size() == 0) {
        Normals.resize(Positions.size());
    }

    for (unsigned int i = 0; i < Indices.size(); i += 3) {
        const unsigned index0 = Indices[i];
        const unsigned index1 = Indices[i + 1];
        const unsigned index2 = Indices[i + 2];

        const auto& v1 = Positions[index0];
        const auto& v2 = Positions[index1];
        const auto& v3 = Positions[index2];

        const auto& t1 = TexCoords[index0];
        const auto& t2 = TexCoords[index1];
        const auto& t3 = TexCoords[index2];



        const Vector3 edge1 = v2 - v1;
        const Vector3 edge2 = v3 - v1;

        const Vector2 uvedge1 = t2 - t1;
        const Vector2 uvedge2 = t3 - t1;

        Vector3 normal = Normalize(Cross(edge1, edge2));

        Normals[index0] += normal;
        Normals[index1] += normal;
        Normals[index2] += normal;

    }

    for (auto index : Indices) {
        auto& norm = Normals[index];
        norm = Normalize(norm);
    }

    printf("Calculated Face Normals\n");
}

void Mesh::CalculateTangentSpace()
{
    // Tangent Space calculations adapted from
    // Lengyel, Eric. Computing Tangent Space Basis Vectors for an Arbitrary Mesh.
    // Terathon Software 3D Graphics Library, 2001. http://www.terathon.com/code/tangent.html
    if (Tangents.size() == 0) {
        Tangents.resize(Positions.size());
    }

    if (Bitangents.size() == 0) {
        Bitangents.resize(Positions.size());
    }

    if (Normals.size() == 0) {
        Normals.resize(Positions.size());
    }

    for (unsigned int i = 0; i < Indices.size(); i += 3) {
        const unsigned index0 = Indices[i];
        const unsigned index1 = Indices[i + 1];
        const unsigned index2 = Indices[i + 2];

        const auto& v1 = Positions[index0];
        const auto& v2 = Positions[index1];
        const auto& v3 = Positions[index2];

        const auto& t1 = TexCoords[index0];
        const auto& t2 = TexCoords[index1];
        const auto& t3 = TexCoords[index2];

        const Vector3 edge1 = v2 - v1;
        const Vector3 edge2 = v3 - v1;

        const Vector2 uvedge1 = t2 - t1;
        const Vector2 uvedge2 = t3 - t1;

        float r = 1.0f / (uvedge1.x * uvedge2.y - uvedge1.y * uvedge2.x);
        Vector4 tangent   = Vector4((edge1 * uvedge2.y   - edge2 * uvedge1.y)*r, 1.f);
        Vector3 bitangent = (edge2 * uvedge1.x   - edge1 * uvedge2.x)*r;


        Tangents[index0] += tangent;
        Tangents[index1] += tangent;
        Tangents[index2] += tangent;

        Bitangents[index0] += bitangent;
        Bitangents[index1] += bitangent;
        Bitangents[index2] += bitangent;

    }

    for (auto index : Indices) {
        auto& norm = Normals[index];
        auto& tan = Tangents[index];
        const auto& bitan = Bitangents[index];

        norm = Normalize(norm);
        // Gram-Schmidt orthogonalize
        tan = { (tan.AsVector3() - norm * Dot(norm, tan.AsVector3())).Normalized(), 0.0f };
        tan.w = (Dot(Cross(norm, tan.AsVector3()), bitan) > 0.0f) ? 1.0f : -1.0f;
        tan = Normalize(tan);
    }

    printf("Calculated Tangent Space\n");

}

void Mesh::CalculateExtents()
{
    float xmin, ymin, zmin;
    xmin = ymin = zmin = 1000000.0f;
    float xmax, ymax, zmax;
    xmax = ymax = zmax = -1000000.0f;
    for (auto& vp : Positions) {
        //auto vp = v.Position;
        if (vp.x < xmin) xmin = vp.x;
        if (vp.y < ymin) ymin = vp.y;
        if (vp.z < zmin) zmin = vp.z;
        if (vp.x > xmax) xmax = vp.x;
        if (vp.y > ymax) ymax = vp.y;
        if (vp.z > zmax) zmax = vp.z;
    }
    const float x = (xmax - xmin) / 2.f;
    const float y = (ymax - ymin) / 2.f;
    const float z = (zmax - zmin) / 2.f;
    m_Extents = Vector3(x, y, z);
    m_minExtents = Vector3(xmin, ymin, zmin);
    m_maxExtents = Vector3(xmax, ymax, zmax);

}

} // namespace Jasper
