#pragma once

#include <iostream>
#include <fstream>
#include <Transform.h>
#include <vector>

namespace Jasper
{

class Material;
class Mesh;
class Shader;
class Scene;
class GameObject;
class Skeleton;


namespace AssetSerializer
{

void SerializeMesh(std::ofstream& ofs, const Mesh* mesh);
void ConstructMesh(std::ifstream& ifs, Scene* scene);
void SerializeMaterial(std::ofstream& ofs, const Material* mat);
void ConstructMaterial(std::ifstream& ifs, Scene* scene);
void SerializeShader(std::ofstream&, const Shader* shader);
void ConstructShader(std::ifstream& ifs, Scene* scene);
void ConstructMeshRenderer(std::ifstream& ifs, std::string name, GameObject* go, Scene* scene);
void ConstructSkyboxRenderer(std::ifstream& ifs, std::string name, GameObject* go, Scene* scene);
void ConstructPhysicsCollider(std::ifstream& ifs, std::string name, GameObject* go, Scene* scene);
void ConstructScriptComponent(std::ifstream& ifs, std::string name, GameObject* go, Scene* scene);

void SerializeSkeleton(std::ofstream& ofs, const Skeleton* skeleton);


template<typename T>
constexpr char* CharPtr(T* val)
{
    return reinterpret_cast<char*>(val);
}
//
template<typename T>
constexpr const char* ConstCharPtr(T* val){
    return reinterpret_cast<const char*>(val);
}

inline void WriteString(std::ofstream& ofs, const std::string& s) {
	size_t sz = s.size();
	ofs.write(ConstCharPtr(&sz), sizeof(sz));
	ofs.write(ConstCharPtr(s.c_str()), sz);
}

inline std::string ReadString(std::ifstream& ifs) {
	size_t sz;
	ifs.read(CharPtr(&sz), sizeof(sz));
	char* buff = new char[sz + 1];
	ifs.read(buff, sz);
	buff[sz] = '\0';
	std::string str = std::string(buff);
	delete[] buff;
	return str;
}

inline void WriteInt(std::ofstream& ofs, int i) {
	ofs.write(ConstCharPtr(&i), sizeof(i));
}

inline int ReadInt(std::ifstream& ifs) {
	int i = 0;
	ifs.read(CharPtr(&i), sizeof(i));
	return i;
}

inline void WriteFloat(std::ofstream& ofs, float f) {
	ofs.write(ConstCharPtr(&f), sizeof(f));
}

inline bool ReadFloat(std::ifstream& ifs) {
	float f = 0;
	ifs.read(CharPtr(&f), sizeof(f));
	return f;
}

inline void WriteBool(std::ofstream& ofs, bool b) {
	ofs.write(ConstCharPtr(&b), sizeof(b));
}

inline bool ReadBool(std::ifstream& ifs) {
	bool b;
	ifs.read(CharPtr(&b), sizeof(b));
	return b;
}

inline void WriteTransform(std::ofstream& ofs, const Transform& t) {
	ofs.write(ConstCharPtr(&t), sizeof(t));
}

inline Transform ReadTransform(std::ifstream ifs) {
	Transform t;
	ifs.read(CharPtr(&t), sizeof(t));
	return t;
}

inline void WriteSize_t(std::ofstream& ofs, size_t s) {
	ofs.write(ConstCharPtr(&s), sizeof(s));
}

inline size_t ReadSize_t(std::ifstream& ifs) {
	size_t s = 0;
	ifs.read(CharPtr(&s), sizeof(s));
	return s;
}

inline void WriteVector2(std::ofstream& ofs, const Vector2& vec) {
	ofs.write(ConstCharPtr(&vec), sizeof(vec));
}

inline void WriteVector3(std::ofstream& ofs, const Vector3& vec) {
	ofs.write(ConstCharPtr(&vec), sizeof(vec));
}

inline void WriteVector4(std::ofstream& ofs, const Vector4& vec) {
	ofs.write(ConstCharPtr(&vec), sizeof(vec));
}

template<typename T>
void WriteVector(std::ofstream& ofs, const std::vector<T>& vec) {
	size_t sz = vec.size();
	WriteSize_t(ofs, sz);
	ofs.write(ConstCharPtr(&(vec[0])), sizeof(T) * sz);
}

template<typename T>
void ReadVector(std::ifstream& ifs, std::vector<T>& vec) {
	size_t sz = ReadSize_t(ifs);		
	vec.reserve(sz);
	for (int i = 0; i < sz; i++) {
		T t;
		ifs.read(CharPtr(&t), sizeof(t));
		vec.emplace_back(t);
	}	
}

}

}
