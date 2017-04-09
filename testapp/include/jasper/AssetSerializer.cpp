#include "AssetSerializer.h"
#include "Mesh.h"
#include "Cube.h"
#include "Quad.h"
#include "Triangle.h"
#include "Sphere.h"
#include "Material.h"
#include "Shader.h"
#include "Scene.h"
#include "SkyboxRenderer.h"
#include "PhysicsCollider.h"
#include "Scriptable.h"
#include <AnimationSystem.h>
#include <memory>


namespace Jasper
{
namespace AssetSerializer
{

using namespace std;

void WriteString(std::ofstream& ofs, const std::string& s) {
	size_t sz = s.size();
	if (sz == 0) {
		int x = 0;
	}
	ofs.write(ConstCharPtr(&sz), sizeof(sz));
	ofs.write(ConstCharPtr(s.data()), sz);
}

std::string ReadString(std::ifstream& ifs) {
	size_t sz = 0;
	ifs.read(CharPtr(&sz), sizeof(sz));
	if (sz == 0) {
		int x = 0;
	}

	char* buff = new char[sz + 1];
	ifs.read(buff, sz);
	buff[sz] = '\0';
	std::string str = std::string(buff);
	delete[] buff;
	return str;
}

void WriteInt(std::ofstream& ofs, int i) {
	ofs.write(ConstCharPtr(&i), sizeof(i));
}

int ReadInt(std::ifstream& ifs) {
	int i = 0;
	ifs.read(CharPtr(&i), sizeof(i));
	return i;
}

void WriteUint(std::ofstream& ofs, uint i) {
	ofs.write(ConstCharPtr(&i), sizeof(i));
}



void WriteFloat(std::ofstream& ofs, float f) {
	ofs.write(ConstCharPtr(&f), sizeof(f));
}

uint ReadUint(std::ifstream& ifs) {
	uint i = 0;
	ifs.read(CharPtr(&i), sizeof(i));
	return i;
}
bool ReadFloat(std::ifstream& ifs) {
	float f = 0;
	ifs.read(CharPtr(&f), sizeof(f));
	return f;
}

void WriteBool(std::ofstream& ofs, bool b) {
	ofs.write(ConstCharPtr(&b), sizeof(b));
}

bool ReadBool(std::ifstream& ifs) {
	bool b;
	ifs.read(CharPtr(&b), sizeof(b));
	return b;
}

void WriteTransform(std::ofstream& ofs, const Transform& t) {
	ofs.write(ConstCharPtr(&t), sizeof(t));
}

Transform ReadTransform(std::ifstream& ifs) {
	Transform t;
	ifs.read(CharPtr(&t), sizeof(t));
	return t;
}

void WriteSize_t(std::ofstream& ofs, size_t s) {
	ofs.write(ConstCharPtr(&s), sizeof(s));
}

size_t ReadSize_t(std::ifstream& ifs) {
	size_t s = 0;
	ifs.read(CharPtr(&s), sizeof(s));
	return s;
}

void WriteVector2(std::ofstream& ofs, const Vector2& vec) {
	ofs.write(ConstCharPtr(&vec), sizeof(vec));
}

Vector2 ReadVector2(std::ifstream& ifs) {
	Vector2 v;
	ifs.read(CharPtr(&v), sizeof(v));
	return v;
}

void WriteVector3(std::ofstream& ofs, const Vector3& vec) {
	ofs.write(ConstCharPtr(&vec), sizeof(vec));
}

Vector3 ReadVector3(std::ifstream& ifs) {
	Vector3 v;
	ifs.read(CharPtr(&v), sizeof(v));
	return v;
}

Vector4 ReadVector4(std::ifstream& ifs) {
	Vector4 v;
	ifs.read(CharPtr(&v), sizeof(v));
	return v;
}

void WriteVector4(std::ofstream& ofs, const Vector4& vec) {
	ofs.write(ConstCharPtr(&vec), sizeof(vec));
}

void WriteMatrix4(std::ofstream& ofs, const Matrix4& m) {
	ofs.write(ConstCharPtr(&m), sizeof(m));
}

Matrix4 ReadMatrix4(std::ifstream& ifs) {
	Matrix4 m;
	ifs.read(CharPtr(&m), sizeof(m));
	return m;
}

void WriteMatrix3(std::ofstream& ofs, const Matrix3& m) {
	ofs.write(ConstCharPtr(&m), sizeof(m));
}

Matrix3 ReadMatrix3(std::ifstream& ifs) {
	Matrix3 m;
	ifs.read(CharPtr(&m), sizeof(m));
	return m;
}

void WriteQuaternion(std::ofstream& ofs, const Quaternion& q) {
	ofs.write(ConstCharPtr(&q), sizeof(q));
}

Quaternion ReadQuaternion(std::ifstream& ifs) {
	Quaternion q;
	ifs.read(CharPtr(&q), sizeof(q));
	return q;
}

void SerializeMesh(ofstream& ofs, const Mesh* mesh) {
	// each mesh will write the following to the stream
	// 1) Count of Positions
	// 2) Position Data
	// 3) COunt of Normals
	// 4) Normal Data
	// and so on in that fashion

	// write mesh name
	WriteString(ofs, mesh->GetName());

	/*const int posc = mesh->Positions.size();
	const int texc = mesh->TexCoords.size();
	const int norc = mesh->Normals.size();
	const int tanc = meddash->Tangents.size();
	const int bitanc = mesh->Bitangents.size();
	const int indc = mesh->Indices.size();
	const int weightc = mesh->BoneWeights.size();*/
	// write positions
	/*ofs.write(ConstCharPtr(&posc), sizeof(posc));
	ofs.write(ConstCharPtr(&(mesh->Positions[0])), sizeof(Vector3) * posc);*/
	WriteVector(ofs, mesh->Positions);
	// write normals
	WriteVector(ofs, mesh->Normals);
	// write tex coords
	WriteVector(ofs, mesh->TexCoords);
	// write tangents
	WriteVector(ofs, mesh->Tangents);
	// write bitans
	WriteVector(ofs, mesh->Bitangents);
	// write bone weights
	WriteVector(ofs, mesh->BoneWeights);
	// write indices
	WriteVector(ofs, mesh->Indices);


	//std::streampos pos = ofs.tellp();

	//printf("%010x \n", pos);


}

void ConstructMesh(std::ifstream& ifs, Scene* scene) {
	// first deserialize the component that the mesh represents.	
	string meshname = ReadString(ifs);
	// now let's do the base mesh stuff	
	ComponentType comptype;
	ifs.read(CharPtr(&comptype), sizeof(comptype));
	assert(comptype == ComponentType::Mesh);
	MeshType mt;
	ifs.read(CharPtr(&mt), sizeof(mt));

	Mesh* existing = nullptr;


	switch (mt) {
	case MeshType::Arbitrary:
	{
		Mesh* m = scene->GetMeshCache().CreateInstance<Mesh>(meshname);
		ReadVector(ifs, m->Positions);
		//int pc;
		/*ifs.read(CharPtr(&pc), sizeof(pc));
		m->Positions.reserve(pc);
		for (int i = 0; i < pc; ++i) {
			Vector3 p;
			ifs.read(CharPtr(p.AsFloatPtr()), sizeof(p));
			m->Positions.push_back(p);
		}*/

		ReadVector(ifs, m->Normals);

		/*int nc;
		ifs.read(CharPtr(&nc), sizeof(nc));
		m->Positions.reserve(nc);
		for (int i = 0; i < nc; ++i) {
			Vector3 n;
			ifs.read(CharPtr(n.AsFloatPtr()), sizeof(n));
			m->Normals.push_back(n);
		}*/

		ReadVector(ifs, m->TexCoords);

		/*int tc;
		ifs.read(CharPtr(&tc), sizeof(tc));
		for (int i = 0; i < tc; ++i) {
			Vector2 t;
			ifs.read(CharPtr(t.AsFloatPtr()), sizeof(t));
			m->TexCoords.push_back(t);
		}*/

		ReadVector(ifs, m->Tangents);

		/*int tanc;
		ifs.read(CharPtr(&tanc), sizeof(tanc));
		for (int i = 0; i < tanc; ++i) {
			Vector4 tan;
			ifs.read(CharPtr(tan.AsFloatPtr()), sizeof(tan));
			m->Tangents.push_back(tan);
		}*/

		ReadVector(ifs, m->Bitangents);

		ReadVector(ifs, m->BoneWeights);

		/*int bc;
		ifs.read(CharPtr(&bc), sizeof(bc));
		for (int i = 0; i < bc; ++i) {
			Vector3 b;
			ifs.read(CharPtr(b.AsFloatPtr()), sizeof(b));
			m->Bitangents.push_back(b);
		}*/

		int indc;
		ifs.read(CharPtr(&indc), sizeof(indc));
		for (int i = 0; i < indc; ++i) {
			uint idx;
			ifs.read(CharPtr(&idx), sizeof(idx));
			m->Indices.push_back(idx);
		}

		std::streampos pos = ifs.tellg();
		std::cout << pos << std::endl;
		int next = ifs.peek();
		std::cout << next << std::endl;

		//	ifs.
	}
	break;
	case MeshType::Cube:
	{
		float repeatu, repeatv;
		Vector3 dimensions;
		// deserialize cube-specific stuff						
		ifs.read(CharPtr(dimensions.AsFloatPtr()), sizeof(dimensions));
		bool isCubeMap;
		ifs.read(CharPtr(&isCubeMap), sizeof(isCubeMap));
		ifs.read(CharPtr(&repeatu), sizeof(repeatu));
		ifs.read(CharPtr(&repeatv), sizeof(repeatv));
		// construct a cube with the given properties in the scene's mesh cache
		existing = scene->GetMeshCache().GetResourceByName(meshname);
		if (existing) return;
		auto cube = scene->GetMeshCache().CreateInstance<Cube>(meshname, dimensions, isCubeMap);
		cube->SetTextureRepeat(repeatu, repeatv);
	}
	break;
	case MeshType::Quad:
	{
		float repeatu, repeatv;
		Vector2 size;
		ifs.read(CharPtr(size.AsFloatPtr()), sizeof(size));
		Quad::AxisAlignment align;
		ifs.read(CharPtr(&align), sizeof(align));
		ifs.read(CharPtr(&repeatu), sizeof(repeatu));
		ifs.read(CharPtr(&repeatv), sizeof(repeatv));
		existing = scene->GetMeshCache().GetResourceByName(meshname);
		if (existing) return;
		scene->GetMeshCache().CreateInstance<Quad>(meshname, size, repeatu, repeatv, align);
	}
	break;
	}
}


void SerializeMaterial(std::ofstream& ofs, const Material* mat) {
	using namespace AssetSerializer;

	WriteString(ofs, mat->GetName());
	WriteUint(ofs, mat->Flags);
	//ofs.write(ConstCharPtr(mat->Ambient.AsFloatPtr()), sizeof(Vector3));
	WriteVector3(ofs, mat->Ambient);
	//ofs.write(ConstCharPtr(mat->Diffuse.AsFloatPtr()), sizeof(Vector3));
	WriteVector3(ofs, mat->Diffuse);
	//ofs.write(ConstCharPtr(mat->Specular.AsFloatPtr()), sizeof(Vector3));
	WriteVector3(ofs, mat->Specular);
	//ofs.write(ConstCharPtr(&(mat->Shine)), sizeof(float));
	WriteFloat(ofs, mat->Shine);

	//const string shaderName = mat->GetShaderName();
	//const size_t shaderNameLength = shaderName.size();
	//ofs.write(ConstCharPtr(&shaderNameLength), sizeof(shaderNameLength));
	//ofs.write(shaderName.c_str(), sizeof(char) * shaderNameLength);	

	const bool hasDiffuseMap = mat->GetTextureDiffuseMap() != nullptr;
	const bool hasNormalMap = mat->GetTextureNormalMap() != nullptr;
	const bool hasSpecularMap = mat->GetTextureSpecularMap() != nullptr;
	WriteBool(ofs, hasDiffuseMap);
	if (hasDiffuseMap) {
		WriteString(ofs, mat->GetTextureDiffuseMap()->GetFilename());
	}

	WriteBool(ofs, hasNormalMap);
	if (hasNormalMap) {
		WriteString(ofs, mat->GetTextureNormalMap()->GetFilename());
	}

	WriteBool(ofs, hasSpecularMap);
	if (hasSpecularMap) {
		WriteString(ofs, mat->GetTextureSpecularMap()->GetFilename());
	}

	if (auto cubemap = mat->GetCubemapTexture()) {
		const bool isCubeMap = true;
		WriteBool(ofs, isCubeMap);
		const size_t filecount = cubemap->GetFileNames().size();
		WriteSize_t(ofs, filecount);
		for (const auto& file : cubemap->GetFileNames()) {
			WriteString(ofs, file);
		}
	}
	else {
		const bool isCubeMap = false;
		WriteBool(ofs, isCubeMap);
	}

}

void ConstructMaterial(std::ifstream& ifs, Scene* scene) {
	// name size then name...
	auto matName = ReadString(ifs);
	printf("Deserialized Material Name: %s\n", matName.c_str());
	uint flags = ReadUint(ifs);

	auto ambient = ReadVector3(ifs);
	auto diffuse = ReadVector3(ifs);
	auto specular = ReadVector3(ifs);
	float shine = ReadFloat(ifs);

	string diffuseMapPath;
	string normalMapPath;
	string specMapPath;
	bool hasDiffuseMap = ReadBool(ifs);
	if (hasDiffuseMap) {
		diffuseMapPath = ReadString(ifs);
		printf("Deserialized Diffuse Map Path: %s\n", diffuseMapPath.c_str());
	}

	bool hasNormalMap = ReadBool(ifs);

	// read the boolean for normal map
	// if true, the next bytes are the normal map file path

	if (hasNormalMap) {
		normalMapPath = ReadString(ifs);
		printf("Deserialized Normal Map Path: %s\n", normalMapPath.c_str());
	}

	bool hasSpecMap = ReadBool(ifs);
	if (hasSpecMap) {
		specMapPath = ReadString(ifs);
		printf("Deserialized Specular Map Path: %s\n", specMapPath.c_str());
	}

	bool iscubemap = ReadBool(ifs);
	vector<string> filenames;
	if (iscubemap) {
		size_t filecount = ReadSize_t(ifs);
		filenames.reserve(filecount);
		for (size_t i = 0; i < filecount; ++i) {
			string fs = ReadString(ifs);
			filenames.push_back(fs);
		}
	}

	Material* cachedMaterial = scene->GetMaterialByName(matName);
	if (cachedMaterial) {
		printf("Material already in cache: %s\n", matName.c_str());
		return;
	}

	/*Shader* mShader = scene->GetShaderByName(shaderName);
	if (!mShader) {
		printf("Shader %s not found in cache, unable to load material %s\n", shaderName.c_str(), matName.c_str());
		return;
	}*/

	Material* mm = scene->GetMaterialCache().CreateInstance<Material>(matName);
	mm->Ambient = ambient;
	mm->Diffuse = diffuse;
	mm->Specular = specular;
	mm->Shine = shine;
	mm->Flags = flags;
	if (hasDiffuseMap) {
		mm->SetTextureDiffuse(diffuseMapPath);
	}
	if (hasNormalMap) {
		mm->SetTextureNormalMap(normalMapPath);
	}
	if (hasSpecMap) {
		mm->SetTextureSpecularMap(specMapPath);
	}
	if (iscubemap) {
		assert(filenames.size() == 6);
		mm->SetCubemapTextures(filenames[0], filenames[1], filenames[2], filenames[3], filenames[4], filenames[5]);
	}

}

void SerializeShader(std::ofstream& ofs, const Shader* shader) {
	// really all we need to store is the type enum and the name
	ShaderClassType ty = shader->GetShaderClassType();
	ofs.write(CharPtr(&ty), sizeof(ty));
	size_t namelen = shader->GetName().size();
	ofs.write(CharPtr(&namelen), sizeof(namelen));
	ofs.write(shader->GetName().c_str(), sizeof(char) * namelen);
}


void ConstructShader(std::ifstream& ifs, Scene* scene) {
	ShaderClassType typ;
	ifs.read(CharPtr(&typ), sizeof(typ));
	size_t namelen;
	ifs.read(CharPtr(&namelen), sizeof(namelen));
	char* namebuff = new char[namelen + 1];
	ifs.read(namebuff, namelen);
	namebuff[namelen] = '\0';
	string name = string(namebuff);
	delete[] namebuff;
	printf("Deserializing shader name: %s\n", name.c_str());

	Shader* s = scene->GetShaderByName(name);
	if (s) {
		printf("Shader already in tha cache: %s\n", name.c_str());
		return;
	}

	switch (typ) {
	case ShaderClassType::BasicShader:
		scene->GetShaderCache().CreateInstance<BasicShader>();
		break;
	case (ShaderClassType::FontShader):
		scene->GetShaderCache().CreateInstance<FontShader>();
		break;
	case ShaderClassType::GuiShader:
		scene->GetShaderCache().CreateInstance<GuiShader>();
		break;
	case ShaderClassType::LitShader:
		scene->GetShaderCache().CreateInstance<LitShader>();
		break;
	case ShaderClassType::SkyboxShader:
		scene->GetShaderCache().CreateInstance<SkyboxShader>(name);
		break;
	case ShaderClassType::TextureShader:
		scene->GetShaderCache().CreateInstance<TextureShader>();
		break;
	}
}

void ConstructMeshRenderer(std::ifstream& ifs, string name, GameObject* go, Scene* scene) {
	size_t meshnamesize, matnamesize;

	ifs.read(CharPtr(&meshnamesize), sizeof(meshnamesize));
	char* meshnamebuff = new char[meshnamesize + 1];
	ifs.read(meshnamebuff, meshnamesize);
	meshnamebuff[meshnamesize] = '\0';
	string meshname = string(meshnamebuff);
	delete[] meshnamebuff;

	ifs.read(CharPtr(&matnamesize), sizeof(matnamesize));
	char* matnamebuff = new char[matnamesize + 1];
	ifs.read(matnamebuff, matnamesize);
	matnamebuff[matnamesize] = '\0';
	string matname = string(matnamebuff);
	delete[] matnamebuff;

	// now that we have the material and mesh names we can get them from the cache
	auto mesh = scene->GetMeshCache().GetResourceByName(meshname);
	auto material = scene->GetMaterialCache().GetResourceByName(matname);
	if (!mesh) {
		printf("Mesh referenced in MeshRenderer constuction from file is noy in the cache.\n");
	}
	if (!material) {
		printf("Material referenced in MeshRenderer constuction from file is not in the cache.\n");
	}

	go->AttachNewComponent<MeshRenderer>(name, mesh, material);
}

void ConstructSkyboxRenderer(std::ifstream& ifs, string name, GameObject* go, Scene* scene) {
	size_t meshnamesize, matnamesize;

	ifs.read(CharPtr(&meshnamesize), sizeof(meshnamesize));
	char* meshnamebuff = new char[meshnamesize + 1];
	ifs.read(meshnamebuff, meshnamesize);
	meshnamebuff[meshnamesize] = '\0';
	string meshname = string(meshnamebuff);
	delete[] meshnamebuff;

	ifs.read(CharPtr(&matnamesize), sizeof(matnamesize));
	char* matnamebuff = new char[matnamesize + 1];
	ifs.read(matnamebuff, matnamesize);
	matnamebuff[matnamesize] = '\0';
	string matname = string(matnamebuff);
	delete[] matnamebuff;

	// now that we have the material and mesh names we can get them from the cache
	auto mesh = scene->GetMeshCache().GetResourceByName(meshname);
	auto material = scene->GetMaterialCache().GetResourceByName(matname);
	if (!mesh) {
		printf("Mesh referenced in MeshRenderer constuction from file is noy in the cache.\n");
	}
	if (!material) {
		printf("Material referenced in MeshRenderer constuction from file is not in the cache.\n");
	}

	go->AttachNewComponent<SkyboxRenderer>(name, mesh, material);
}

void ConstructPhysicsCollider(std::ifstream& ifs, string name, GameObject* go, Scene* scene) {

	const auto world = scene->GetPhysicsWorld();
	PHYSICS_COLLIDER_TYPE type;
	ifs.read(CharPtr(&type), sizeof(type));
	float mass, restitution, friction;
	ifs.read(CharPtr(&mass), sizeof(mass));
	ifs.read(CharPtr(&restitution), sizeof(restitution));
	ifs.read(CharPtr(&friction), sizeof(friction));
	Vector3 halfextents;
	ifs.read(CharPtr(halfextents.AsFloatPtr()), sizeof(halfextents));
	PhysicsCollider* collider;
	switch (type) {
	case PHYSICS_COLLIDER_TYPE::Box:
		collider = go->AttachNewComponent<BoxCollider>(name, halfextents, world);
		collider->Mass = mass;
		collider->Restitution = restitution;
		collider->Friction = friction;
		break;
	case PHYSICS_COLLIDER_TYPE::Capsule:
		collider = go->AttachNewComponent<CapsuleCollider>(name, halfextents, world);
		collider->Mass = mass;
		collider->Restitution = restitution;
		collider->Friction = friction;
		break;
	case PHYSICS_COLLIDER_TYPE::Plane:
		Vector3 normal;
		float constant;
		ifs.read(CharPtr(normal.AsFloatPtr()), sizeof(normal));
		ifs.read(CharPtr(&constant), sizeof(constant));
		collider = go->AttachNewComponent<PlaneCollider>(name, normal, constant, world);
		collider->Mass = mass;
		collider->Restitution = restitution;
		collider->Friction = friction;
		break;
	}

}



void ConstructScriptComponent(std::ifstream& ifs, std::string name, GameObject* go, Scene* scene) {
	ScriptType ty;
	ifs.read(CharPtr(&ty), sizeof(ty));
	ScriptComponent* script = nullptr;
	switch (ty) {
	case ScriptType::RotateAboutPoint:
		script = go->AttachNewComponent<RotateAboutPointScript>(name);
		script->Deserialize(ifs);
		break;
	}
}

void SerializeSkeleton(std::ofstream & ofs, const Skeleton * skeleton)
{
	int cc = static_cast<int>(skeleton->Bones.size());
	AssetSerializer::WriteString(ofs, skeleton->GetName());
	AssetSerializer::WriteInt(ofs, cc);
	for (int i = 0; i < cc; ++i) {
		auto& bnn = skeleton->Bones[i];
		AssetSerializer::WriteInt(ofs, bnn->Id);
		AssetSerializer::WriteInt(ofs, bnn->ParentID);
		AssetSerializer::WriteTransform(ofs, bnn->NodeTransform);
		AssetSerializer::WriteTransform(ofs, bnn->BoneOffsetTransform);
		AssetSerializer::WriteString(ofs, bnn->Name);
	}
}

Skeleton* ConstructSkeleton(std::ifstream & ifs, Scene * scene)
{
	string name = ReadString(ifs);
	int boneCount = ReadInt(ifs);

	auto sk = scene->GetSkeletonCache().GetResourceByName(name);
	if (sk) {
		printf("Skeleton %s is already in the cache.\n", name.c_str());
	}
	else {
		sk = scene->GetSkeletonCache().CreateInstance<Skeleton>(name);
	}

	sk->Bones.clear();
	sk->Bones.reserve(boneCount);
	for (int i = 0; i < boneCount; i++) {
		auto bd = make_unique<BoneData>();
		int id = AssetSerializer::ReadInt(ifs);
		int parentid = AssetSerializer::ReadInt(ifs);
		Transform nodet = AssetSerializer::ReadTransform(ifs);
		Transform offsett = AssetSerializer::ReadTransform(ifs);
		string nnn = AssetSerializer::ReadString(ifs);
		bd->BoneOffsetTransform = offsett;
		bd->NodeTransform = nodet;
		bd->Id = id;		
		bd->ParentID = parentid;
		bd->Name = nnn;
		sk->Bones.emplace_back(move(bd));
	}
	for (const auto& b : sk->Bones) {
		b->skeleton = sk;
		if (b->ParentID == -1) {
			b->Parent = nullptr;
			sk->RootBone = b.get();
		}
		else {
			auto parentBone = sk->Bones[b->ParentID].get();
			b->Parent = parentBone;
			parentBone->Children.push_back(b->Id);
		}
		sk->m_boneMap[b->Name] = b->Id;
	}
	return sk;
}

}

}
