#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <tuple>

#include <vector.h>
#include <matrix.h>
#include <quaternion.h>
#include <AnimationSystem.h>

#include <experimental/filesystem>

namespace Jasper {
namespace PythonInterface {

std::string PyObjectToString(PyObject* po);

std::vector<Vector3> ParsePyVerts(PyObject* vo);

std::vector<Vector3> ParsePyNormals(PyObject* vo);

std::vector<Vector2> ParsePyTexCoords(PyObject* vo);

std::vector<uint32_t> ParsePyIndices(PyObject* o);

void ParseColladaGeometries(PyObject* gobj, std::vector<std::unique_ptr<ColladaMesh>>& colladaMeshes);
void ParseColladaControllers(PyObject* cobj, std::vector<std::unique_ptr<ColladaMesh>>& colladaMeshes);

class PythonInterpreter {

public:

	static PythonInterpreter& GetInstance() {
		static PythonInterpreter instance;
		return instance;
	}

	~PythonInterpreter() {
		ShutDown();
	}

	void Initialize();
	void ShutDown();

	PyObject* LoadModule(const std::string& name);

	template<typename... T>
	PyObject* ExecuteFunction(PyObject* mod, const std::string& funcName, std::tuple<T...> args);

private:
	void SetupModulePaths();
	PythonInterpreter() {
		Initialize();
	}
	PythonInterpreter(const PythonInterpreter& o);
	void operator=(const PythonInterpreter& o);

};

template<int N, typename... Ts> using NthTypeOf =
typename std::tuple_element<N, std::tuple<Ts...>>::type;

template<typename... T>
inline PyObject* PythonInterpreter::ExecuteFunction(PyObject* mod, const std::string& funcName, std::tuple<T...> args) {
	PyObject* func = PyObject_GetAttrString(mod, funcName.c_str());

	auto argsize = std::tuple_size<decltype(args)>::value;
	PyObject* pyargs = PyTuple_New(argsize);

	for (int i = 0; i < argsize; ++i) {
		auto argi = std::get<0>(args);
		// figure out what to do for non string args...
		PyObject* strarg = PyUnicode_FromString(static_cast<const char*>(argi));
		PyTuple_SetItem(pyargs, i, strarg);
		//Py_DecRef(strarg);
	}
	if (func && PyCallable_Check(func)) {
		//printf("Good To Go!\n");		
		PyObject* result = PyObject_CallObject(func, pyargs);
		Py_DecRef(func);		
		Py_DecRef(pyargs);
		return result;
	}
	Py_DecRef(func);
	return nullptr;
}


} // PythonInterface
} // Jasper