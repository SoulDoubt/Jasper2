#pragma once

#include "Common.h"
#include "Component.h"
#include "vector.h"
#include "GLBuffer.h"
#include "Texture.h"
#include "Transform.h"

#include <vector>
#include <memory>
#include <unordered_map>

class aiNode;

namespace Jasper
{

class Material;
class Shader;

struct Vertex {
	Vector3 Position;
	Vector4 Color;
	Vector3 Normal;
	Vector2 TexCoords;
	Vector4 Tangent;
	Vector3 Bitangent;

	Vertex(float x, float y, float z, float u, float v) {
		Position = { x, y, z };
		TexCoords = { u, v };
		Normal = { 0.0f, 0.0f, 0.0f };
		Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		Tangent = { 0.f, 0.f, 0.f, 1.f };
		Bitangent = { 0.f, 0.f, 0.f };
	}

	Vertex(const Vector3& position, const Vector3& normal, float u, float v) {
		Position = position;
		Normal = normal;
		Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		TexCoords.x = u;
		TexCoords.y = v;
		Tangent = { 0.f, 0.f, 0.f, 1.f };
		Bitangent = { 0.f, 0.f, 0.f };
	}

	Vertex(const Vector3& position) {
		Position = position;
		TexCoords = { 0.0f, 1.0f };
		Normal = { 0.0f, 0.0f, 0.0f };
		Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		Tangent = { 0.f, 0.f, 0.f, 1.f };
		Bitangent = { 0.f, 0.f, 0.f };
	}

	Vertex(const Vector3& position, const Vector3& normal, const Vector4& color, const Vector2& texCoords) {
		Position = position;
		Normal = normal;
		Color = color;
		TexCoords = texCoords;
		Tangent = { 0.f, 0.f, 0.f, 1.f };
		Bitangent = { 0.f, 0.f, 0.f };
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

struct Vertex_P {
	Vector3 Position;

	Vertex_P() : Position(0.f) {}
	Vertex_P(const Vector3& position) : Position(position) {}
};

struct Vertex_PCNUTB {
	Vector3 Position;
	Vector4 Color;
	Vector3 Normal;
	Vector2 TexCoords;
	Vector4 Tangent;
	Vector3 Bitangent;

	Vertex_PCNUTB(float x, float y, float z, float u, float v) {
		Position = { x, y, z };
		TexCoords = { u, v };
		Normal = { 0.0f, 0.0f, 0.0f };
		Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		Tangent = { 0.f, 0.f, 0.f, 1.f };
		Bitangent = { 0.f, 0.f, 0.f };
	}

	Vertex_PCNUTB(const Vector3& position, const Vector3& normal, float u, float v) {
		Position = position;
		Normal = normal;
		Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		TexCoords.x = u;
		TexCoords.y = v;
		Tangent = { 0.f, 0.f, 0.f, 1.f };
		Bitangent = { 0.f, 0.f, 0.f };
	}

	Vertex_PCNUTB(const Vector3& position) {
		Position = position;
		TexCoords = { 0.0f, 1.0f };
		Normal = { 0.0f, 0.0f, 0.0f };
		Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		Tangent = { 0.f, 0.f, 0.f, 1.f };
		Bitangent = { 0.f, 0.f, 0.f };
	}

	Vertex_PCNUTB(const Vector3& position, const Vector3& normal, const Vector4& color, const Vector2& texCoords) {
		Position = position;
		Normal = normal;
		Color = color;
		TexCoords = texCoords;
		Tangent = { 0.f, 0.f, 0.f, 1.f };
		Bitangent = { 0.f, 0.f, 0.f };
	}

	Vertex_PCNUTB() {
		Position = Vector3();
		Normal = Vector3();
		Color = Vector4();
		TexCoords = Vector2();
		Tangent = Vector4();
		Bitangent = Vector3();
	}

};

struct Vertex_PNUTB {
	Vector3 Position;
	Vector3 Normal;
	Vector2 TexCoords;
	Vector4 Tangent;
	Vector3 Bitangent;

	Vertex_PNUTB(float x, float y, float z, float u, float v) {
		Position = { x, y, z };
		TexCoords = { u, v };
		Normal = { 0.0f, 0.0f, 0.0f };
		Tangent = { 0.f, 0.f, 0.f, 1.f };
		Bitangent = { 0.f, 0.f, 0.f };
	}

	Vertex_PNUTB(const Vector3& position, const Vector3& normal, float u, float v) {
		Position = position;
		Normal = normal;
		TexCoords.x = u;
		TexCoords.y = v;
		Tangent = { 0.f, 0.f, 0.f, 1.f };
		Bitangent = { 0.f, 0.f, 0.f };
	}

	Vertex_PNUTB(const Vector3& position) {
		Position = position;
		TexCoords = { 0.0f, 1.0f };
		Normal = { 0.0f, 0.0f, 0.0f };
		Tangent = { 0.f, 0.f, 0.f, 1.f };
		Bitangent = { 0.f, 0.f, 0.f };
	}

	Vertex_PNUTB(const Vector3& position, const Vector3& normal, const Vector2& texCoords) {
		Position = position;
		Normal = normal;
		TexCoords = texCoords;
		Tangent = { 0.f, 0.f, 0.f, 1.f };
		Bitangent = { 0.f, 0.f, 0.f };
	}

	Vertex_PNUTB() {
		Position = Vector3();
		Normal = Vector3();
		TexCoords = Vector2();
		Tangent = Vector4();
		Bitangent = Vector3();
	}

};

struct Vertex_PNUTB_ANIM {
	Vector3 Position;
	Vector3 Normal;
	Vector2 TexCoords;
	Vector4 Tangent;
	Vector3 Bitangent;

	int Bones[4] = { -1,-1,-1,-1 };
	float Weights[4] = { 0.f, 0.f, 0.f, 0.f };

	int GetNextAvailableBoneIndex() {
		for (int i = 0; i < 4; ++i) {
			if (Bones[i] == -1) {
				return i;
			}
		}
		return 4;
	}

	Vertex_PNUTB_ANIM(float x, float y, float z, float u, float v) {
		Position = { x, y, z };
		TexCoords = { u, v };
		Normal = { 0.0f, 0.0f, 0.0f };
		Tangent = { 0.f, 0.f, 0.f, 1.f };
		Bitangent = { 0.f, 0.f, 0.f };
	}

	Vertex_PNUTB_ANIM(const Vector3& position, const Vector3& normal, float u, float v) {
		Position = position;
		Normal = normal;
		TexCoords.x = u;
		TexCoords.y = v;
		Tangent = { 0.f, 0.f, 0.f, 1.f };
		Bitangent = { 0.f, 0.f, 0.f };
	}

	Vertex_PNUTB_ANIM(const Vector3& position) {
		Position = position;
		TexCoords = { 0.0f, 1.0f };
		Normal = { 0.0f, 0.0f, 0.0f };
		Tangent = { 0.f, 0.f, 0.f, 1.f };
		Bitangent = { 0.f, 0.f, 0.f };
	}

	Vertex_PNUTB_ANIM(const Vector3& position, const Vector3& normal, const Vector2& texCoords) {
		Position = position;
		Normal = normal;
		TexCoords = texCoords;
		Tangent = { 0.f, 0.f, 0.f, 1.f };
		Bitangent = { 0.f, 0.f, 0.f };
	}

	Vertex_PNUTB_ANIM() {
		Position = Vector3();
		Normal = Vector3();
		TexCoords = Vector2();
		Tangent = Vector4();
		Bitangent = Vector3();
	}

};

struct Vertex_PNU {
	Vector3 Position;
	Vector3 Normal;
	Vector2 TexCoords;

	Vertex_PNU(float x, float y, float z, float u, float v) {
		Position = { x, y, z };
		TexCoords = { u, v };
		Normal = { 0.0f, 0.0f, 0.0f };
	}

	Vertex_PNU(const Vector3& position, const Vector3& normal, float u, float v) {
		Position = position;
		Normal = normal;
		TexCoords.x = u;
		TexCoords.y = v;
	}

	Vertex_PNU(const Vector3& position) {
		Position = position;
		TexCoords = { 0.0f, 1.0f };
		Normal = { 0.0f, 0.0f, 0.0f };
	}

	Vertex_PNU(const Vector3& position, const Vector3& normal, const Vector2& texCoords) {
		Position = position;
		Normal = normal;
		TexCoords = texCoords;
	}

	Vertex_PNU() {
		Position = Vector3();
		Normal = Vector3();
		TexCoords = Vector2();
	}

};

struct Vertex_PNU_ANIM {
	Vector3 Position;
	Vector3 Normal;
	Vector2 TexCoords;

	int Bones[4] = { -1,-1,-1,-1 };
	float Weights[4] = { 0.f, 0.f, 0.f, 0.f };

	int GetNextAvailableBoneIndex() {
		for (int i = 0; i < 4; ++i) {
			if (Bones[i] == -1) {
				return i;
			}
		}
		return 4;
	}

	Vertex_PNU_ANIM(float x, float y, float z, float u, float v) {
		Position = { x, y, z };
		TexCoords = { u, v };
		Normal = { 0.0f, 0.0f, 0.0f };
	}

	Vertex_PNU_ANIM(const Vector3& position, const Vector3& normal, float u, float v) {
		Position = position;
		Normal = normal;
		TexCoords.x = u;
		TexCoords.y = v;
	}

	Vertex_PNU_ANIM(const Vector3& position) {
		Position = position;
		TexCoords = { 0.0f, 1.0f };
		Normal = { 0.0f, 0.0f, 0.0f };
	}

	Vertex_PNU_ANIM(const Vector3& position, const Vector3& normal, const Vector2& texCoords) {
		Position = position;
		Normal = normal;
		TexCoords = texCoords;
	}

	Vertex_PNU_ANIM() {
		Position = Vector3();
		Normal = Vector3();
		TexCoords = Vector2();
	}

};

struct Vertex_PCN {

	Vector3 Position;
	Vector4 Color;
	Vector3 Normal;


	Vertex_PCN() : Position(0.f), Color(1.f), Normal(0.f) {

	}

	Vertex_PCN(const Vector3& position, const Vector3& normal, const Vector4& color) : Position(position), Color(color), Normal(normal) {}
};

struct Vertex_PN {
	Vector3 Position;
	Vector3 Normal;

	Vertex_PN() : Position(0.f), Normal(0.f) {}
	Vertex_PN(const Vector3& p, const Vector3& n) : Position(p), Normal(n) {}
};

struct Tri {
	uint32_t a, b, c;
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

class Skeleton;
class BoneData;
//class VertexBoneWeight;

struct VertexBoneWeight {
	int   BoneID;
	uint  Index;
	float Weight;

	VertexBoneWeight(int boneID, uint vertIndex, float weight)
		: BoneID(boneID), Index(vertIndex), Weight(weight) {}

	VertexBoneWeight() : BoneID(-1), Index(-1), Weight(0.f) {}
};

inline bool operator<(const VertexBoneWeight& a, const VertexBoneWeight& b)
{
	return a.Index < b.Index;
}

inline bool operator==(const VertexBoneWeight& a, const VertexBoneWeight& b)
{
	return a.Index == b.Index;
}



class Mesh //: public Component
{
public:
	enum class VERTEX_FORMAT
	{
		None,
		Vertex_PCNUTB,
		Vertex_PNUTB,
		Vertex_PNU,
		Vertex_PCN,
		Vertex_PN,
		Vertex_P,
		Vertex_PNUTB_ANIM,
		Vertex_PNU_ANIM
	};

private:
	uint m_vaoID;
	uint m_elementCount;
	VERTEX_FORMAT m_vertexFormat = VERTEX_FORMAT::None;

	GLBuffer m_vertexBuffer;
	GLBuffer m_indexBuffer;


public:

	void SetVertexFormat(VERTEX_FORMAT format) {
		m_vertexFormat = format;
	}

	void SetMaterial(Material* m) {
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

	bool HasBones() const {
		return Bones.size() > 0;
	}

	GLBuffer& IndexBuffer() {
		return m_indexBuffer;
	}

	std::vector<int> Bones;



	int m_numBones = 0;

	explicit Mesh(const std::string& name);
	virtual ~Mesh();

	virtual void Initialize();
	virtual void Destroy();

	void FlipTextureCoords();


	virtual MeshType GetMeshType() const {
		return MeshType::Arbitrary;
	}

	virtual void Serialize(std::ofstream& ofs) const;

	std::vector<Vector3>		  Positions;
	std::vector<Vector3>		  Normals;
	std::vector<Vector2>		  TexCoords;
	std::vector<Vector4>		  Colors;
	std::vector<Vector4>		  Tangents;
	std::vector<Vector3>		  Bitangents;
	std::vector<uint>			  Indices;
	std::vector<VertexBoneWeight> BoneWeights;

	Vector4 Color;


	void AddVertex(const Vertex& vertex);
	void AddVertex_PCNUTB(const Vertex_PCNUTB& vertex);
	void AddVertex_PNUTB(const Vertex_PNUTB& vertex);
	void AddVertex_PNU(const Vertex_PNU& vertex);
	void AddVertex_PCN(const Vertex_PCN& vertex);

	Vector3 HalfExtents() const {
		return m_Extents;
	}

	Vector3 MaxExtents() const {
		return m_maxExtents;
	}

	Vector3 MinExtents() const {
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
		Colors.clear();
		BoneWeights.clear();
	}

	unsigned GetTriCount() const {
		return static_cast<unsigned>(Indices.size()) / 3;
	}

	unsigned GetVertCount() const {
		return static_cast<unsigned>(Positions.size());
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


	void CalculateTangentSpace();
	void CalculateFaceNormals();
	void CalculateExtents();

	Material* GetMaterial() const {
		return m_material;
	}

	void OptimizeIndices();

	void AddTriangle(const Tri& t);

	void InitializeForRendering(Shader* shader);

	void SetSkeleton(Skeleton* sk) {
		m_skeleton = sk;
	}

	Skeleton* GetSkeleton() const {
		return m_skeleton;
	}

	const std::string& GetName() const {
		return m_name;
	}

	void SetName(const std::string& name) {
		m_name = name;
	}

protected:

	std::string m_name;

	int renderer_count = 0;

	bool m_reverseWinding = false;

	Vector3 m_Extents;
	Vector3 m_minExtents;
	Vector3 m_maxExtents;
	Vector3 m_origin;

	Material* m_material = nullptr;

	Skeleton* m_skeleton;

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

inline void Mesh::AddVertex_PCNUTB(const Vertex_PCNUTB& vertex)
{
	TexCoords.push_back(vertex.TexCoords);
	Normals.push_back(vertex.Normal);
	Tangents.push_back(vertex.Tangent);
	Bitangents.push_back(vertex.Bitangent);
	Positions.push_back(vertex.Position);
	Colors.push_back(vertex.Color);
}

inline void Mesh::AddVertex_PNUTB(const Vertex_PNUTB& vertex)
{
	TexCoords.push_back(vertex.TexCoords);
	Normals.push_back(vertex.Normal);
	Tangents.push_back(vertex.Tangent);
	Bitangents.push_back(vertex.Bitangent);
	Positions.push_back(vertex.Position);
}

inline void Mesh::AddVertex_PNU(const Vertex_PNU& vertex)
{
	Positions.push_back(vertex.Position);
	TexCoords.push_back(vertex.TexCoords);
	Normals.push_back(vertex.Normal);
}

inline void Mesh::AddVertex_PCN(const Vertex_PCN& vertex)
{
	Positions.push_back(vertex.Position);
	Colors.push_back(vertex.Color);
	Normals.push_back(vertex.Normal);
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

class MeshComponent : public Component {
public:

	ComponentType GetComponentType() const override {
		return ComponentType::Mesh;
	}

	MeshComponent(const std::string& name, Mesh* m) 
		: Component(name), m_mesh(m)
	{
	}

	Mesh* GetMesh() {
		return m_mesh;
	}

	const Mesh* GetMesh() const {
		return m_mesh;
	}

	void SetMesh(Mesh* m) {
		m_mesh = m;
	}

	void Serialize(std::ofstream& ofs) const override;

private:
	Mesh* m_mesh;
};

} // namespace Jasper
