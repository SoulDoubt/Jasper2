#include "AssetSerializer.h"
#include "Mesh.h"
#include "Cube.h"
#include "Quad.h"
#include "Triangle.h"
#include "Sphere.h"
#include "Material.h"
#include "Shader.h"
#include "Scene.h"
#include "BasicShader.h"
#include "TextureShader.h"
#include "GuiShader.h"
#include "LitShader.h"
#include "FontShader.h"
#include "SkyboxShader.h"
#include "SkyboxRenderer.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "SphereCollider.h"
#include "PlaneCollider.h"
#include "Scriptable.h"
#include <memory>


namespace Jasper
{
namespace AssetSerializer
{

using namespace std;

void SerializeMesh(ofstream& ofs, const Mesh* mesh) {
	// each mesh will write the following to the stream
	// 1) Count of Positions
	// 2) Position Data
	// 3) COunt of Normals
	// 4) Normal Data
	// and so on in that fashion

	/*const string name = mesh->GetName();
	const size_t namesz = name.size();
	ofs.write(ConstCharPtr(&namesz), sizeof(namesz));
	ofs.write(ConstCharPtr(name.data()), namesz);*/

	const int posc = mesh->Positions.size();
	const int texc = mesh->TexCoords.size();
	const int norc = mesh->Normals.size();
	const int tanc = mesh->Tangents.size();
	const int bitanc = mesh->Bitangents.size();
	const int indc = mesh->Indices.size();
	// write positions
	ofs.write(ConstCharPtr(&posc), sizeof(posc));
	ofs.write(ConstCharPtr(&(mesh->Positions[0])), sizeof(Vector3) * posc);
	cout << mesh->Positions[0].ToString() << endl;
	cout << mesh->Positions[1].ToString() << endl;
	// write normals
	ofs.write(ConstCharPtr(&norc), sizeof(norc));
	ofs.write(ConstCharPtr(&(mesh->Normals[0])), sizeof(Vector3) * norc);
	// write tex coords
	ofs.write(ConstCharPtr(&texc), sizeof(texc));
	ofs.write(ConstCharPtr(&(mesh->TexCoords[0])), sizeof(Vector2) * texc);
	// write tangents
	ofs.write(ConstCharPtr(&tanc), sizeof(tanc));
	ofs.write(ConstCharPtr(&(mesh->Tangents[0])), sizeof(Vector4) * tanc);
	// write bitans
	ofs.write(ConstCharPtr(&bitanc), sizeof(bitanc));
	ofs.write(ConstCharPtr(&(mesh->Bitangents[0])), sizeof(Vector3) * bitanc);

	ofs.write(ConstCharPtr(&indc), sizeof(indc));
	ofs.write(ConstCharPtr(&(mesh->Indices[0])), sizeof(uint) * indc);
	
	std::streampos pos = ofs.tellp();
	printf("%010x \n", pos);


}

void ConstructMesh(std::ifstream& ifs, Scene* scene) {
	// first deserialize the component that the mesh represents.	
	size_t namesize;
	ifs.read(CharPtr(&namesize), sizeof(namesize));
	char* componentname = new char[namesize + 1];
	ifs.read(componentname, namesize);
	componentname[namesize] = '\0';
	string componentName = string(componentname);
	delete[] componentname;
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
		Mesh* m = scene->GetMeshCache().CreateInstance<Mesh>(componentName);
		int pc;
		ifs.read(CharPtr(&pc), sizeof(pc));
		m->Positions.reserve(pc);
		for (int i = 0; i < pc; ++i) {
			Vector3 p;
			ifs.read(CharPtr(p.AsFloatPtr()), sizeof(p));
			m->Positions.push_back(p);
		}

		int nc;
		ifs.read(CharPtr(&nc), sizeof(nc));
		m->Positions.reserve(nc);
		for (int i = 0; i < nc; ++i) {
			Vector3 n;
			ifs.read(CharPtr(n.AsFloatPtr()), sizeof(n));
			m->Normals.push_back(n);
		}

		int tc;
		ifs.read(CharPtr(&tc), sizeof(tc));
		for (int i = 0; i < tc; ++i) {
			Vector2 t;
			ifs.read(CharPtr(t.AsFloatPtr()), sizeof(t));
			m->TexCoords.push_back(t);
		}

		int tanc;
		ifs.read(CharPtr(&tanc), sizeof(tanc));
		for (int i = 0; i < tanc; ++i) {
			Vector4 tan;
			ifs.read(CharPtr(tan.AsFloatPtr()), sizeof(tan));
			m->Tangents.push_back(tan);
		}

		int bc;
		ifs.read(CharPtr(&bc), sizeof(bc));
		for (int i = 0; i < bc; ++i) {
			Vector3 b;
			ifs.read(CharPtr(b.AsFloatPtr()), sizeof(b));
			m->Bitangents.push_back(b);
		}

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
		existing = scene->GetMeshCache().GetResourceByName(componentName);
		if (existing) return;
		auto cube = scene->GetMeshCache().CreateInstance<Cube>(componentName, dimensions, isCubeMap);
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
		existing = scene->GetMeshCache().GetResourceByName(componentName);
		if (existing) return;
		scene->GetMeshCache().CreateInstance<Quad>(componentName, size, repeatu, repeatv, align);
	}
	break;
	}
}


void SerializeMaterial(std::ofstream& ofs, const Material* mat) {
	using namespace AssetSerializer;

	const string name = mat->GetName();
	const size_t nameSize = name.size();
	ofs.write(ConstCharPtr(&nameSize), sizeof(nameSize));
	ofs.write(name.c_str(), nameSize * sizeof(char));
	ofs.write(ConstCharPtr(mat->Ambient.AsFloatPtr()), sizeof(Vector3));
	ofs.write(ConstCharPtr(mat->Diffuse.AsFloatPtr()), sizeof(Vector3));
	ofs.write(ConstCharPtr(mat->Specular.AsFloatPtr()), sizeof(Vector3));
	ofs.write(ConstCharPtr(&(mat->Shine)), sizeof(float));

	const string shaderName = mat->GetShaderName();
	const size_t shaderNameLength = shaderName.size();
	ofs.write(ConstCharPtr(&shaderNameLength), sizeof(shaderNameLength));
	ofs.write(shaderName.c_str(), sizeof(char) * shaderNameLength);

	const bool hasDiffuseMap = mat->GetTextureDiffuseMap() != nullptr;
	ofs.write(ConstCharPtr(&hasDiffuseMap), sizeof(hasDiffuseMap));
	if (hasDiffuseMap) {
		auto dm = mat->GetTextureDiffuseMap();
		const string diffuseMapFile = dm->GetFilename();
		const size_t dlen = diffuseMapFile.size();
		ofs.write(ConstCharPtr(&dlen), sizeof(dlen));
		ofs.write(ConstCharPtr(diffuseMapFile.data()), sizeof(char) * dlen);
	}

	const bool hasNormalMap = mat->GetTextureNormalMap() != nullptr;
	const bool hasSpecularMap = mat->GetTextureSpecularMap() != nullptr;

	ofs.write(ConstCharPtr(&hasNormalMap), sizeof(hasNormalMap));
	if (hasNormalMap) {
		auto nm = mat->GetTextureNormalMap();
		const string normalMapFile = nm->GetFilename();
		const size_t norlen = normalMapFile.size();
		ofs.write(ConstCharPtr(&norlen), sizeof(norlen));
		ofs.write(normalMapFile.data(), sizeof(char) * norlen);
	}

	ofs.write(ConstCharPtr(&hasSpecularMap), sizeof(hasSpecularMap));
	if (hasSpecularMap) {
		const auto sm = mat->GetTextureSpecularMap();
		const string specMapFile = sm->GetFilename();
		const size_t speclen = specMapFile.size();
		ofs.write(ConstCharPtr(&speclen), sizeof(speclen));
		ofs.write(specMapFile.c_str(), sizeof(char) * speclen);
	}

	if (auto cubemap = mat->GetCubemapTexture()) {
		const bool isCubeMap = true;
		ofs.write(ConstCharPtr(&isCubeMap), sizeof(isCubeMap));
		const size_t filecount = cubemap->GetFileNames().size();
		ofs.write(ConstCharPtr(&filecount), sizeof(filecount));
		for (const auto& file : cubemap->GetFileNames()) {
			const auto fs = file.size();
			ofs.write(ConstCharPtr(&fs), sizeof(fs));
			ofs.write(file.data(), fs);
		}
	}
	else {
		bool isCubeMap = false;
		ofs.write(CharPtr(&isCubeMap), sizeof(isCubeMap));
	}
}

void ConstructMaterial(std::ifstream& ifs, Scene* scene) {
	// name size then name...
	size_t namelen;
	ifs.read(CharPtr(&namelen), sizeof(namelen));
	char* namebuff = new char[namelen + 1];
	ifs.read(namebuff, namelen);
	namebuff[namelen] = '\0';
	string matName = string(namebuff);
	delete[] namebuff;
	printf("Deserialized Material Name: %s\n", matName.c_str());


	Vector3 ambient, diffuse, specular;
	float shine;
	ifs.read(CharPtr(&ambient), sizeof(ambient));
	ifs.read(CharPtr(&diffuse), sizeof(diffuse));
	ifs.read(CharPtr(&specular), sizeof(specular));
	ifs.read(CharPtr(&shine), sizeof(shine));

	size_t sns;
	ifs.read(CharPtr(&sns), sizeof(sns));
	char* snbuff = new char[sns + 1];
	ifs.read(snbuff, sns);
	snbuff[sns] = '\0';
	string shaderName = string(snbuff);
	delete[] snbuff;

	bool hasDiffuseMap;
	string diffuseMapPath = "";
	ifs.read(CharPtr(&hasDiffuseMap), sizeof(hasDiffuseMap));
	if (hasDiffuseMap) {
		size_t dms;
		ifs.read(CharPtr(&dms), sizeof(dms));
		char* dmbuff = new char[dms + 1];
		ifs.read(dmbuff, dms);
		dmbuff[dms] = '\0';
		diffuseMapPath = string(dmbuff);
		delete[] dmbuff;
		printf("Deserialized Diffuse Map Path: %s\n", diffuseMapPath.c_str());
	}

	// read the boolean for normal map
	// if true, the next bytes are the normal map file path
	bool hasNormalMap;
	string normalMapPath = "";
	ifs.read(CharPtr(&hasNormalMap), sizeof(hasNormalMap));
	if (hasNormalMap) {
		size_t nms;
		ifs.read(CharPtr(&nms), sizeof(nms));
		char* nmbuff = new char[nms + 1];
		ifs.read(nmbuff, nms);
		nmbuff[nms] = '\0';
		normalMapPath = string(nmbuff);
		delete[] nmbuff;
		printf("Deserialized Normal Map Path: %s\n", normalMapPath.c_str());
	}

	bool hasSpecMap;
	string specMapPath = "";
	ifs.read(CharPtr(&hasSpecMap), sizeof(hasSpecMap));
	if (hasSpecMap) {
		size_t sms;
		ifs.read(CharPtr(&sms), sizeof(sms));
		char* smbuff = new char[sms + 1];
		ifs.read(smbuff, sms);
		smbuff[sms] = '\0';
		specMapPath = string(smbuff);
		delete[] smbuff;
		printf("Deserialized Specular Map Path: %s\n", specMapPath.c_str());
	}

	bool iscubemap = false;
	ifs.read(CharPtr(&iscubemap), sizeof(iscubemap));
	vector<string> filenames;
	if (iscubemap) {
		size_t filecount = 0;
		ifs.read(CharPtr(&filecount), sizeof(filecount));
		filenames.reserve(filecount);
		for (size_t i = 0; i < filecount; ++i) {
			size_t fs = 0;
			ifs.read(CharPtr(&fs), sizeof(fs));
			char* fbuff = new char[fs + 1];
			ifs.read(fbuff, fs);
			fbuff[fs] = '\0';
			filenames.emplace_back(string(fbuff));
			delete[] fbuff;
		}
	}

	Material* cachedMaterial = scene->GetMaterialByName(matName);
	if (cachedMaterial) {
		printf("Material already in cache: %s\n", matName.c_str());
		return;
	}

	Shader* mShader = scene->GetShaderByName(shaderName);
	if (!mShader) {
		printf("Shader %s not found in cache, unable to load material %s\n", shaderName.c_str(), matName.c_str());
		return;
	}

	Material* mm = scene->GetMaterialCache().CreateInstance<Material>(mShader, matName);
	mm->Ambient = ambient;
	mm->Diffuse = diffuse;
	mm->Specular = specular;
	mm->Shine = shine;
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

}

}
