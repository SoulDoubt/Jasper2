#pragma once

#include <iostream>
#include <fstream>
#include <Transform.h>
#include <vector>
#include <Component.h>



namespace Jasper
{

class Material;
class Mesh;
class Shader;
class Scene;
class GameObject;
class Skeleton;
class AnimationComponent;
class SkeletonComponent;
class Animation;

namespace AssetSerializer
{

std::vector<std::string> GetFileNamesByExtension(const std::string& path, const std::string& extension);

void WriteString(std::ofstream& ofs, const std::string& s);

std::string ReadString(std::ifstream& ifs);

void WriteInt(std::ofstream& ofs, int i);

int ReadInt(std::ifstream& ifs);

void WriteUint(std::ofstream& ofs, uint i);

void WriteFloat(std::ofstream& ofs, float f);

uint ReadUint(std::ifstream& ifs);

bool ReadFloat(std::ifstream& ifs);

void WriteBool(std::ofstream& ofs, bool b);

bool ReadBool(std::ifstream& ifs);

void WriteTransform(std::ofstream& ofs, const Transform& t);

Transform ReadTransform(std::ifstream& ifs);

void WriteSize_t(std::ofstream& ofs, size_t s);

size_t ReadSize_t(std::ifstream& ifs);

void WriteVector2(std::ofstream& ofs, const Vector2& vec);

Vector2 ReadVector2(std::ifstream& ifs);

void WriteVector3(std::ofstream& ofs, const Vector3& vec);

Vector3 ReadVector3(std::ifstream& ifs);

Vector4 ReadVector4(std::ifstream& ifs);

void WriteVector4(std::ofstream& ofs, const Vector4& vec);

void WriteMatrix4(std::ofstream& ofs, const Matrix4& m);

Matrix4 ReadMatrix4(std::ifstream& ifs);

void WriteMatrix3(std::ofstream& ofs, const Matrix3& m);

Matrix3 ReadMatrix3(std::ifstream& ifs);
void WriteQuaternion(std::ofstream& ofs, const Quaternion& q);
Quaternion ReadQuaternion(std::ifstream& ifs);

// Resources
void SerializeMesh(std::ofstream& ofs, const Mesh* mesh);
void DeserializeMesh(std::ifstream& ifs, Mesh* mesh);
void ConstructMesh(std::ifstream& ifs, Scene* scene);
void SerializeMaterial(std::ofstream& ofs, const Material* mat);
void DeserializeMaterial(std::ifstream& ifs, Material* mat);
void ConstructMaterial(std::ifstream& ifs, Scene* scene);
void SerializeShader(std::ofstream&, const Shader* shader);
void ConstructShader(std::ifstream& ifs, Scene* scene);
void SerializeSkeleton(std::ofstream& ofs, const Skeleton* skeleton);
void DeserializeSkeleton(std::ifstream& ifs, Skeleton* skeleton);
Skeleton* ConstructSkeleton(std::ifstream& ifs, Scene* scene);

// Components
void WriteBaseComponentData(std::ofstream& ofs, const Component* cmp);
void ReadBaseComponentData(std::ifstream& ifs, std::string* name, ComponentType* cmpType);
void DeserializeAndAttachMeshRendererComponent(std::ifstream& ifs, std::string name, GameObject* go, Scene* scene);
void DeserializeAndAttachSkyboxRendererComponent(std::ifstream& ifs, std::string name, GameObject* go, Scene* scene);
void DeserializeAndAttachPhysicsColliderComponent(std::ifstream& ifs, std::string name, GameObject* go, Scene* scene);
void ConstructScriptComponent(std::ifstream& ifs, std::string name, GameObject* go, Scene* scene);


void SerializeAnimationComponent(std::ofstream& ofs, const AnimationComponent* anim);
void DeserializeAndAttachAnimationComponent(std::ifstream& ifs, std::string name, GameObject* go);

void SerializeGameObjectAsAsset(std::ofstream& ofs, const GameObject* go);



void SerializeSkeletonComponent(std::ofstream& ofs, const SkeletonComponent* sk);
void DeserializeAndAttachSkeletonComponent(std::ifstream& ifs, GameObject* go);

void SerializeAnimation(std::ofstream& ofs, const Animation* anim);
void DeserializeAnimation(std::ifstream& ifs, Animation* anim);

template<typename T>
constexpr char* CharPtr(T* val)
{
	return reinterpret_cast<char*>(val);
}
//
template<typename T>
constexpr const char* ConstCharPtr(T* val) {
	return reinterpret_cast<const char*>(val);
}

template<typename T>
void WriteVector(std::ofstream& ofs, const std::vector<T>& vec) {
	size_t sz = vec.size();
	WriteSize_t(ofs, sz);
	size_t soft = sizeof(T);
	size_t bytes = soft * sz;
	if (sz > 0) {
		ofs.write(ConstCharPtr(&(vec[0])), soft * sz);
	}
}

template<typename T>
void ReadVector(std::ifstream& ifs, std::vector<T>& vec) {
	size_t sz = ReadSize_t(ifs);
	vec.reserve(sz);
	size_t bytes_read = 0;
	for (int i = 0; i < sz; ++i) {
		T t;
		size_t ss = sizeof(T);
		if (!ifs.eof()) {
			ifs.read(CharPtr(&t), ss);
			bytes_read += ss;
			vec.push_back(t);
		}
		else {
			printf("EOF Reached deserializing vector.\n");
		}
	}	
	printf("%d\n", bytes_read);
}

}

}
