#pragma once

#include "Common.h"
#include "Component.h"
#include "vector.h"
#include "GLBuffer.h"
#include "Texture.h"
#include "Transform.h"

#include <vector>
#include <memory>
#include "Mesh.h"

namespace Jasper
{

class Material;

struct Vertex {
    Vector3 Position;
    Vector3 Normal;
    Vector2 TexCoords;
    Vector4 Color;
    Vector4 Tangent;
    Vector3 Bitangent;

    Vertex(float x, float y, float z, float u, float v) {
        Position = { x, y, z };
        TexCoords = { u, v };
        Normal = { 0.0f, 0.0f, 0.0f };
        Color = { 1.0f, 1.0f, 1.0f, 1.0f };
        Tangent = {0.f, 0.f, 0.f, 1.f};
        Bitangent = {0.f, 0.f, 0.f};
    }

    Vertex(const Vector3& position, const Vector3& normal, float u, float v) {
        Position = position;
        Normal = normal;
        Color = { 1.0f, 1.0f, 1.0f, 1.0f };
        TexCoords.x = u;
        TexCoords.y = v;
        Tangent = {0.f, 0.f, 0.f, 1.f};
        Bitangent = {0.f, 0.f, 0.f};
    }

    Vertex(const Vector3& position) {
        Position = position;
        TexCoords = { 0.0f, 1.0f };
        Normal = { 0.0f, 0.0f, 0.0f };
        Color = { 1.0f, 1.0f, 1.0f, 1.0f };
        Tangent = {0.f, 0.f, 0.f, 1.f};
        Bitangent = {0.f, 0.f, 0.f};
    }

    Vertex(const Vector3& position, const Vector3& normal, const Vector4& color, const Vector2& texCoords) {
        Position = position;
        Normal = normal;
        Color = color;
        TexCoords = texCoords;
        Tangent = {0.f, 0.f, 0.f, 1.f};
        Bitangent = {0.f, 0.f, 0.f};
    }

    Vertex() {
        Position = Vector3();
        Normal = Vector3();
        Color = Vector4();
        TexCoords = Vector2();
        Tangent = Vector4();
        Bitangent = Vector3();
    }

};

struct Tri {
    uint a, b, c;

    Tri(float x, float y, float z) : a(x), b(y), c(z) {}
};

enum class MeshType
{
    Arbitrary,
    Cube,
    Sphere,
    Quad,
    Triangle
};


class Mesh : public Component
{
    friend class Model;
private:
    uint m_vaoID;
    int m_elementCount;
    GLBuffer m_vertexBuffer;
    GLBuffer m_texCoordBuffer;
    GLBuffer m_normalBuffer;
    GLBuffer m_tangentBuffer;
    GLBuffer m_bitangentBuffer;
    GLBuffer m_indexBuffer;
    GLBuffer m_colorBuffer;

    public:

    void SetMaterial(Material* m){
        m_material = m;
    }
    
    int ElementCount() const {
        return m_elementCount;
    }

    int VaoID() const {
        return m_vaoID;
    }

    GLBuffer& VertexBuffer() {
        return m_vertexBuffer;
    }

    GLBuffer& TexCoordBuffer() {
        return m_texCoordBuffer;
    }

    GLBuffer& NormalBuffer() {
        return m_normalBuffer;
    }

    GLBuffer& TangentBuffer() {
        return m_tangentBuffer;
    }

    GLBuffer& BitangentBuffer() {
        return m_bitangentBuffer;
    }

    GLBuffer& IndexBuffer() {
        return m_indexBuffer;
    }

    GLBuffer& ColorBuffer() {
        return m_colorBuffer;
    }


    struct VertexBoneWeight {
        uint Index;
        float Weight;
    };

    struct BoneData {

        std::string Name;
        std::vector<VertexBoneWeight> Weights;
        BoneData* Parent;
        Transform BoneTransform;
    };

    std::vector<BoneData> Bones;
    
    explicit Mesh(const std::string& name);
    virtual ~Mesh();

    void Initialize() override;
    void Destroy() override;

    void FlipTextureCoords();

    ComponentType GetComponentType() const override {
        return ComponentType::Mesh;
    }

    virtual MeshType GetMeshType() const {
        return MeshType::Arbitrary;
    }

    void Serialize(std::ofstream& ofs) const override;

    std::vector<Vector3> Positions;
    std::vector<Vector3> Normals;
    std::vector<Vector2> TexCoords;
    std::vector<Vector3> Colors;
    std::vector<Vector4> Tangents;
    std::vector<Vector3> Bitangents;

    Vector4 Color;


    void AddVertex(const Vertex& vertex);

    Vector3 GetHalfExtents() const {
        return m_Extents;
    }

    Vector3 GetMaxExtents() const {
        return m_maxExtents;
    }

    Vector3 GetMinExtents() const {
        return m_minExtents;
    }

    Vector3 GetOrigin() const {
        return m_origin;
    }

    void SetReverseWinding(bool r) {
        m_reverseWinding = r;
    }
    void ReverseWinding();

    void UnloadClientData() {
        //Vertices.clear();
        Positions.clear();
        Normals.clear();
        TexCoords.clear();
        Tangents.clear();
        Bitangents.clear();
        Indices.clear();
    }

    unsigned GetTriCount() const {
        return (unsigned)Indices.size() / 3;
    }

    unsigned GetVertCount() const {
        return (unsigned)Positions.size();
    }

    int IncrementRendererCount() {
        renderer_count++;
        return renderer_count;
    }

    int DecrementRendererCount() {
        renderer_count--;
        return renderer_count;
    }

    int GetRendererCount() {
        return renderer_count;
    }

    //std::vector<Vertex> Vertices;
    std::vector<uint> Indices;

    void CalculateTangentSpace();
    void CalculateFaceNormals();
    void CalculateExtents();
    
    Material* GetMaterial() const {
        return m_material;
    }
    
    void OptimizeIndices();
    
    void AddTriangle(const Tri& t);

protected:

    int renderer_count = 0;


    

    bool m_reverseWinding = false;

    Vector3 m_Extents;
    Vector3 m_minExtents;
    Vector3 m_maxExtents;
    Vector3 m_origin;

    Material* m_material = nullptr;

};

inline void Mesh::AddVertex(const Vertex& vertex)
{
    //Vertices.push_back(vertex);
    TexCoords.push_back(vertex.TexCoords);
    Normals.push_back(vertex.Normal);
    Tangents.push_back(vertex.Tangent);
    Bitangents.push_back(vertex.Bitangent);
    Positions.push_back(vertex.Position);


}


inline void Mesh::ReverseWinding()
{
    for (unsigned i = 0; i < Indices.size(); i += 3) {
        unsigned i0 = Indices[i];
        unsigned i1 = Indices[i + 1];
        unsigned i2 = Indices[i + 2];
        Indices[i] = i2;
        Indices[i + 1] = i1;
        Indices[i + 2] = i0;
    }
}

} // namespace Jasper
