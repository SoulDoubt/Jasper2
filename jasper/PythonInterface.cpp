#include <PythonInterface.h>


namespace Jasper {
namespace PythonInterface {

using namespace std;

string PyObjectToString(PyObject* po) {
	PyObject* pStrObj = PyUnicode_AsUTF8String(po);
	char* zStr = PyBytes_AsString(pStrObj);
	string ret = string(zStr);
	Py_DecRef(pStrObj);
	return ret;
}

vector<Vector3> ParsePyVerts(PyObject* vo) {
	vector<Vector3> verts;	
	int sz = PySequence_Length(vo);
	verts.reserve(sz);
	for (int i = 0; i < sz; ++i) {
		PyObject* pyvert = PySequence_GetItem(vo, i);
		float x = (float)PyFloat_AsDouble(PySequence_GetItem(pyvert, 0));
		float y = (float)PyFloat_AsDouble(PySequence_GetItem(pyvert, 1));
		float z = (float)PyFloat_AsDouble(PySequence_GetItem(pyvert, 2));
		Vector3 vv = { x, y, z };
		verts.push_back(vv);
		Py_DecRef(pyvert);
	}
	return verts;
}

vector<Vector3> ParsePyVector3Array(PyObject* vo) {
	vector<Vector3> vecs;
	int sz = PySequence_Length(vo);
	vecs.reserve(sz);
	for (int i = 0; i < sz; ++i) {
		PyObject* pyvert = PySequence_GetItem(vo, i);
		float x = (float)PyFloat_AsDouble(PySequence_GetItem(pyvert, 0));
		float y = (float)PyFloat_AsDouble(PySequence_GetItem(pyvert, 1));
		float z = (float)PyFloat_AsDouble(PySequence_GetItem(pyvert, 2));
		Vector3 vv = { x, y, z };
		vecs.push_back(vv);
		Py_DecRef(pyvert);
	}
	return vecs;
}

vector<Vector3> ParsePyNormals(PyObject* no) {
	vector<Vector3> norms;
	int sz = PySequence_Length(no);
	norms.reserve(sz);
	for (int i = 0; i < sz; ++i) {
		PyObject* pynorm = PySequence_GetItem(no, i);
		float x = (float)PyFloat_AsDouble(PySequence_GetItem(pynorm, 0));
		float y = (float)PyFloat_AsDouble(PySequence_GetItem(pynorm, 1));
		float z = (float)PyFloat_AsDouble(PySequence_GetItem(pynorm, 2));
		Vector3 vv = { x, y, z };
		norms.push_back(vv);
		Py_DecRef(pynorm);
	}
	return norms;
}

vector<Vector2> ParsePyVector2Array(PyObject* to) {
	vector<Vector2> uvs;
	int sz = PySequence_Length(to);
	uvs.reserve(sz);
	for (int i = 0; i < sz; ++i) {
		PyObject* pytc = PySequence_GetItem(to, i);
		float x = (float)PyFloat_AsDouble(PySequence_GetItem(pytc, 0));
		float y = (float)PyFloat_AsDouble(PySequence_GetItem(pytc, 1));		
		Vector2 vv = { x, y };
		uvs.push_back(vv);
		Py_DecRef(pytc);
	}
	return uvs;
}

std::vector<uint32_t> ParsePyIndices(PyObject * o)
{
	std::vector<uint32_t> result;
	int sz = PySequence_Length(o);
	for (int i = 0; i < sz; ++i) {		
		auto ido = PySequence_GetItem(o, i);
		if (PySequence_Check(ido)) {
			int iss = PySequence_Length(ido);
			assert(iss == 3);
			uint32_t idx0 = (uint32_t)_PyLong_AsInt(PySequence_GetItem(ido, 0));
			uint32_t idx1 = (uint32_t)_PyLong_AsInt(PySequence_GetItem(ido, 1));
			uint32_t idx2 = (uint32_t)_PyLong_AsInt(PySequence_GetItem(ido, 2));
			result.push_back(idx0);
			result.push_back(idx1);
			result.push_back(idx2);
		}
		else {
			uint32_t idx = (uint32_t)_PyLong_AsInt(ido);
			result.push_back(idx);
		}						
	}	
	return result;
}

void ParseColladaGeometries(PyObject* pyGeoms, std::vector<std::unique_ptr<ColladaMesh>>& colladaMeshes)
{
	int isSeq = PySequence_Check(pyGeoms);
	if (isSeq) {
		int sz = PySequence_Length(pyGeoms);
		for (int i = 0; i < sz; ++i) {

			PyObject* geom = PySequence_GetItem(pyGeoms, i);
			auto nameobj = PyObject_GetAttrString(geom, "id");
			string meshName = PyObjectToString(nameobj);
			auto colladaMesh = make_unique<ColladaMesh>();
			colladaMesh->Name = move(meshName);
			if (PyObject_HasAttrString(geom, "primitives")) {
				PyObject* primitives = PyObject_GetAttrString(geom, "primitives");
				int psz = PySequence_Length(primitives);
				for (int j = 0; j < psz; ++j) {
					PyObject* prim = PySequence_GetItem(primitives, j);
					PyObject* vertices = PyObject_GetAttrString(prim, "vertex");
					PyObject* normals = PyObject_GetAttrString(prim, "normal");
					PyObject* uvSet = PyObject_GetAttrString(prim, "texcoordset");
					colladaMesh->Positions = move(ParsePyVector3Array(vertices));
					colladaMesh->Normals = move(ParsePyVector3Array(normals));
					colladaMesh->TexCoords = move(ParsePyVector2Array(PySequence_GetItem(uvSet, 0)));
					auto vindex = PyObject_GetAttrString(prim, "vertex_index");
					auto nindex = PyObject_GetAttrString(prim, "normal_index");
					auto tindexs = PyObject_GetAttrString(prim, "texcoord_indexset");
					int tids = PySequence_Length(tindexs);
					if (tids > 0) {
						auto tid = PySequence_GetItem(tindexs, 0);
						colladaMesh->TexCoordIndices = move(PythonInterface::ParsePyIndices(tid));
						Py_DecRef(tid);
					}
					colladaMesh->PositionIndices = move(PythonInterface::ParsePyIndices(vindex));
					colladaMesh->NormalIndices = move(PythonInterface::ParsePyIndices(nindex));
					colladaMeshes.emplace_back(move(colladaMesh));
					Py_DecRef(tindexs);
					Py_DecRef(vindex);
					Py_DecRef(nindex);
					Py_DecRef(normals);
					Py_DecRef(uvSet);
					Py_DecRef(vertices);
					Py_DecRef(prim);
				}
				Py_DecRef(primitives);
			}
			Py_DecRef(geom);
		}
	}
}

void ParseColladaControllers(PyObject * cobj, std::vector<std::unique_ptr<ColladaMesh>>& colladaMeshes)
{
	int isSeq = PySequence_Check(cobj);
	if (isSeq) {
		int sz = PySequence_Length(cobj);
		for (int i = 0; i < sz; ++i) {
			PyObject* skin = PySequence_GetItem(cobj, i);			
			// get the name of the colladamesh that this skin applies to
			string meshName = PyObjectToString(PyObject_GetAttrString(PyObject_GetAttrString(skin, "geometry"), "id"));
			

			Py_DecRef(skin);
		}
	}
}


void PythonInterpreter::Initialize()
{
	Py_Initialize();
	SetupModulePaths();

}

void PythonInterpreter::ShutDown()
{
	Py_Finalize();
}

PyObject* PythonInterpreter::LoadModule(const std::string & name)
{
	auto pypath = Py_GetPath();
	wprintf(L"Python Path: %s\n", pypath);
	PyObject* moduleName = PyUnicode_FromString(name.c_str());
	PyObject* pyModule = PyImport_Import(moduleName);
	if (pyModule == nullptr)
	{
		PyErr_Print();
		Py_DecRef(moduleName);
		return nullptr;
	}
	Py_DecRef(moduleName);
	return pyModule;
}



void PythonInterpreter::SetupModulePaths()
{
	using namespace std::experimental::filesystem;
	
	path current = current_path();
	path parent = current.parent_path();
	path scripts_path = parent.append("scripts"s);
	
	if (exists(scripts_path)) {		
		PyRun_SimpleString("import sys");
		auto sp = scripts_path.string();
		std::replace(sp.begin(), sp.end(), '\\', '/');
		auto npath = string("sys.path.append(\"") + sp + string("\")");		
		PyRun_SimpleString(npath.c_str());		
	}
	
}

} // PythonInterface
} // Jasper
