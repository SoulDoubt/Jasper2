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
    //m_texCoordBuffer(GLBuffer::BufferType::VERTEX),
    //m_normalBuffer(GLBuffer::BufferType::VERTEX),
    //m_tangentBuffer(GLBuffer::BufferType::VERTEX),
    //m_bitangentBuffer(GLBuffer::BufferType::VERTEX),
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

void Mesh::InitializeForRendering(Shader* shader)
{
    
    glGenVertexArrays(1, &m_vaoID);
    glBindVertexArray(m_vaoID);
    //GLERRORCHECK;

    m_elementCount = this->Indices.size();
    const int positionLocation = shader->PositionAttributeLocation();
    const int normalLocation = shader->NormalAttributeLocation();
    const int texLocation = shader->TexCoordAttributeLocation();
    const int tangentLocation = shader->TangentAttributeLocation();
    const int bitangentLocation = shader->BitangentAttributeLocation();
    const int colorLocation = shader->ColorsAttributeLocation();

    if (m_vertexFormat == VERTEX_FORMAT::None) {
        if (m_material->Flags & Material::MATERIAL_FLAGS::HAS_NORMAL_MAP) {
            // normal mapped materials imply that we will need UVs and Tangent Space
            SetVertexFormat(VERTEX_FORMAT::Vertex_PNUTB);
        } else if (m_material->Flags & Material::MATERIAL_FLAGS::HAS_COLOR_MAP) {
            // we still need UVs
            SetVertexFormat(VERTEX_FORMAT::Vertex_PNU);
        } else if (m_material->Flags & Material::MATERIAL_FLAGS::USE_MATERIAL_COLOR) {
            // dont' need UVs or vertex color
            SetVertexFormat(VERTEX_FORMAT::Vertex_PN);
        } else if (m_material->Flags & Material::MATERIAL_FLAGS::USE_VERTEX_COLORS) {
            SetVertexFormat(VERTEX_FORMAT::Vertex_PCN);
        }
    }
    
    

    m_vertexBuffer.Create();
    m_indexBuffer.Create();
    
    
    m_indexBuffer.Bind();
    m_indexBuffer.Allocate(Indices.data(), Indices.size() * sizeof(uint));

    switch(m_vertexFormat) {

    case VERTEX_FORMAT::Vertex_P: {
        Vertex_P* verts = new Vertex_P[Positions.size()];
        for (uint i = 0; i < Positions.size(); ++i) {
            Vertex_P v;
            v.Position = Positions[i];
            verts[i] = v;
        }
        m_vertexBuffer.Bind();
        m_vertexBuffer.Allocate(verts, Positions.size() * sizeof(Vertex_P));
        shader->SetAttributeArray(positionLocation, GL_FLOAT, (void*)offsetof(Vertex_P, Position), 3, sizeof(Vertex_P));
        delete[] verts;
    }
    break;

    case VERTEX_FORMAT::Vertex_PN: {
        assert(Positions.size() == Normals.size());
        Vertex_PN* verts = new Vertex_PN[Positions.size()];
        for (uint i = 0; i < Positions.size(); ++i) {
            Vertex_PN v;
            v.Position = Positions[i];
            v.Normal = Normals[i];
            verts[i] = v;
        }
        m_vertexBuffer.Bind();
        m_vertexBuffer.Allocate(verts, Positions.size() * sizeof(Vertex_PN));
        shader->SetAttributeArray(positionLocation, GL_FLOAT, (void*)offsetof(Vertex_PN, Position), 3, sizeof(Vertex_PN));
        shader->SetAttributeArray(normalLocation, GL_FLOAT, (void*)offsetof(Vertex_PN, Normal), 3, sizeof(Vertex_PN));
        delete[] verts;
    }
    break;
    case VERTEX_FORMAT::Vertex_PNU: {
        assert(Positions.size() == Normals.size() && Positions.size() == TexCoords.size());
        Vertex_PNU* verts = new Vertex_PNU[Positions.size()];
        for (uint i = 0; i < Positions.size(); i++) {
            Vertex_PNU v;
            v.Position = Positions[i];
            v.Normal = Normals[i];
            v.TexCoords = TexCoords[i];
            verts[i] = v;
        }
        m_vertexBuffer.Bind();
        m_vertexBuffer.Allocate(verts, Positions.size() * sizeof(Vertex_PNU));
        shader->SetAttributeArray(positionLocation, GL_FLOAT, (void*)offsetof(Vertex_PNU, Position), 3, sizeof(Vertex_PNU));
        shader->SetAttributeArray(normalLocation, GL_FLOAT, (void*)offsetof(Vertex_PNU, Normal), 3, sizeof(Vertex_PNU));
        shader->SetAttributeArray(texLocation, GL_FLOAT, (void*)offsetof(Vertex_PNU, TexCoords), 2, sizeof(Vertex_PNU));
        delete[] verts;
    }
    break;
    case VERTEX_FORMAT::Vertex_PCN: {
        assert(Positions.size() == Colors.size() && Positions.size() == Normals.size());
        Vertex_PCN* verts = new Vertex_PCN[Positions.size()];
        for (uint i = 0; i < Positions.size(); i++) {
            Vertex_PCN v;
            v.Position = Positions[i];
            v.Color = Colors[i];
            v.Normal = Normals[i];
            verts[i] = v;
        }
        m_vertexBuffer.Bind();
        m_vertexBuffer.Allocate(verts, Positions.size() * sizeof(Vertex_PCN));
        shader->SetAttributeArray(positionLocation, GL_FLOAT, (void*)offsetof(Vertex_PCN, Position), 3, sizeof(Vertex_PCN));
        shader->SetAttributeArray(colorLocation, GL_FLOAT, (void*)offsetof(Vertex_PCN, Color), 3, sizeof(Vertex_PCN));
        shader->SetAttributeArray(normalLocation, GL_FLOAT, (void*)offsetof(Vertex_PCN, Normal), 3, sizeof(Vertex_PCN));
        delete[] verts;
    }
    break;
    case VERTEX_FORMAT::Vertex_PNUTB: {
        //assert(Positions.size() == Colors.size() == Normals.size());
        Vertex_PNUTB* verts = new Vertex_PNUTB[Positions.size()];
        for (uint i = 0; i < Positions.size(); ++i) {
            Vertex_PNUTB v;
            v.Position = Positions[i];
            v.Normal = Normals[i];
            v.TexCoords = TexCoords[i];
            v.Tangent = Tangents[i];
            v.Bitangent = Bitangents[i];
            verts[i] = v;
        }
        
        if (HasBones()){
            for (const BoneData& b : Bones){
                for (const VertexBoneWeight& w : b.Weights){
                    Vertex_PNUTB& v = verts[w.Index];
                    v.BonesAffecting.push_back(b.Name);
                }
            }
            printf("Bone Data for mesh %s : \n", this->GetName().data());
            int max_bones = 0;
            int vertex_id = 0;
            for (uint i = 0; i < Positions.size(); ++i){
                auto& vv = verts[i];
                if (vv.BonesAffecting.size() > max_bones){
                    max_bones = vv.BonesAffecting.size();
                    vertex_id = i;
                }
            }
            printf("Maximum bones affecting a vertex is: %d\n", max_bones);
            auto& maxvert = verts[vertex_id];
            for(const string& bonename : maxvert.BonesAffecting){
                printf("%s\n", bonename.data());
            }
        }

        
        
        m_vertexBuffer.Bind();
        m_vertexBuffer.Allocate(verts, Positions.size() * sizeof(Vertex_PNUTB));
        shader->SetAttributeArray(positionLocation, GL_FLOAT, (void*)offsetof(Vertex_PNUTB, Position), 3, sizeof(Vertex_PNUTB));
        shader->SetAttributeArray(normalLocation, GL_FLOAT, (void*)offsetof(Vertex_PNUTB, Normal), 3, sizeof(Vertex_PNUTB));
        shader->SetAttributeArray(texLocation, GL_FLOAT, (void*)offsetof(Vertex_PNUTB, TexCoords), 2, sizeof(Vertex_PNUTB));
        shader->SetAttributeArray(tangentLocation, GL_FLOAT, (void*)offsetof(Vertex_PNUTB, Tangent), 4, sizeof(Vertex_PNUTB));
        shader->SetAttributeArray(bitangentLocation, GL_FLOAT, (void*)offsetof(Vertex_PNUTB, Bitangent), 3, sizeof(Vertex_PNUTB));
        delete[] verts;
    }
    break;
    case VERTEX_FORMAT::Vertex_PCNUTB: {
        //assert(Positions.size() == Colors.size() == Normals.size());
        Vertex_PCNUTB* verts = new Vertex_PCNUTB[Positions.size()];
        for (uint i = 0; i < Positions.size(); i++) {
            Vertex_PCNUTB v;
            v.Position = Positions[i];
            v.Color = Colors[i];
            v.Normal = Normals[i];
            v.TexCoords = TexCoords[i];
            v.Tangent = Tangents[i];
            v.Bitangent = Bitangents[i];
            verts[i] = v;
        }
        m_vertexBuffer.Bind();
        m_vertexBuffer.Allocate(verts, Positions.size() * sizeof(Vertex_PCNUTB));
        shader->SetAttributeArray(positionLocation, GL_FLOAT, (void*)offsetof(Vertex_PCNUTB, Position), 3, sizeof(Vertex_PCNUTB));
        shader->SetAttributeArray(colorLocation, GL_FLOAT, (void*)offsetof(Vertex_PCNUTB, Color), 3, sizeof(Vertex_PCNUTB));
        shader->SetAttributeArray(normalLocation, GL_FLOAT, (void*)offsetof(Vertex_PCNUTB, Normal), 3, sizeof(Vertex_PCNUTB));
        shader->SetAttributeArray(texLocation, GL_FLOAT, (void*)offsetof(Vertex_PCNUTB, TexCoords), 2, sizeof(Vertex_PCNUTB));
        shader->SetAttributeArray(tangentLocation, GL_FLOAT, (void*)offsetof(Vertex_PCNUTB, Tangent), 4, sizeof(Vertex_PCNUTB));
        shader->SetAttributeArray(bitangentLocation, GL_FLOAT, (void*)offsetof(Vertex_PCNUTB, Bitangent), 3, sizeof(Vertex_PCNUTB));
        delete[] verts;
    }
    break;
    }

//    if (normalLocation > -1) {
//        m_normalBuffer.Create();
//        m_normalBuffer.Bind();
//        m_normalBuffer.Allocate(Normals.data(), Normals.size() * sizeof(Vector3));
//        shader->SetAttributeArray(normalLocation, GL_FLOAT, (void*)0, 3, 0);
//    }
//    if (texLocation > -1) {
//        m_texCoordBuffer.Create();
//        m_texCoordBuffer.Bind();
//        m_texCoordBuffer.Allocate(TexCoords.data(), TexCoords.size() * sizeof(Vector2));
//        shader->SetAttributeArray(texLocation, GL_FLOAT, (void*)0, 2, 0);
//    }
//    if (tangentLocation > -1) {
//        m_tangentBuffer.Create();
//        m_tangentBuffer.Bind();
//        m_tangentBuffer.Allocate(Tangents.data(), Tangents.size() * sizeof(Vector4));
//        shader->SetAttributeArray(tangentLocation, GL_FLOAT, (void*)0, 4, 0);
//    }
//    if (bitangentLocation > -1) {
//        m_bitangentBuffer.Create();
//        m_bitangentBuffer.Bind();
//        m_bitangentBuffer.Allocate(Bitangents.data(), Bitangents.size() * sizeof(Vector3));
//        shader->SetAttributeArray(bitangentLocation, GL_FLOAT, (void*)0, 3, 0);
//    }
//    if (this->Color != Vector4(0.f, 0.f, 0.f, 0.f)) {
//        m_colorBuffer.Create();
//        int cc = Positions.size();
//        Vector4* colorData = new Vector4[cc];
//        for (int i = 0; i < cc; ++i) {
//            colorData[i] = Color;
//        }
//        m_colorBuffer.Bind();
//        m_colorBuffer.Allocate(colorData, cc * sizeof(Vector4));
//        delete[] colorData;
//        shader->SetAttributeArray(colorLocation, GL_FLOAT, 0, 4, 0);
//    }

    glBindVertexArray(0);
    GLERRORCHECK;
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
//    if (m_texCoordBuffer.IsCreated())
//        m_texCoordBuffer.Destroy();
//    if (m_normalBuffer.IsCreated())
//        m_normalBuffer.Destroy();
//    if (m_tangentBuffer.IsCreated())
//        m_tangentBuffer.Destroy();
//    if (m_bitangentBuffer.IsCreated())
//        m_bitangentBuffer.Destroy();
//    if (m_colorBuffer.IsCreated())
//        m_colorBuffer.Destroy();
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

        //const Vector2 uvedge1 = t2 - t1;
        //const Vector2 uvedge2 = t3 - t1;

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
        //Vector3 tangent   = (edge1 * uvedge2.y   - edge2 * uvedge1.y)*r;
        //tangent = Normalize(tangent - Dot(tangent, normal) * normal);
        Vector3 bitangent = (edge2 * uvedge1.x   - edge1 * uvedge2.x)*r;
        //Vector4 outTan = Vector4(tangent, 1.0);


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

void Mesh::AddTriangle(const Tri& t)
{
    Indices.push_back(t.a);
    Indices.push_back(t.b);
    Indices.push_back(t.c);
}

} // namespace Jasper
