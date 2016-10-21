#include "AssetSerializer.h"
#include "Mesh.h"



namespace Jasper {
namespace AssetSerializer{
    
using namespace std;

void SerializeMesh(ofstream& ofs, Mesh* mesh){
    // each mesh will write the following to the stream
    // 1) Count of Positions
    // 2) Position Data
    // 3) COunt of Normals
    // 4) Normal Data
    // and so on in that fashion
    int posc = mesh->Positions.size();
    int texc = mesh->TexCoords.size();
    int norc = mesh->Normals.size();
    int tanc = mesh->Tangents.size();
    int bitanc = mesh->Bitangents.size();
    // write positions
    ofs.write(reinterpret_cast<char*>(&posc), sizeof(posc));
    ofs.write(reinterpret_cast<char*>(&(mesh->Positions[0])), sizeof(Vector3) * posc);
    cout << mesh->Positions[0].ToString() << endl;
    cout << mesh->Positions[1].ToString() << endl;
    // write normals
    ofs.write(reinterpret_cast<char*>(&norc), sizeof(norc));
    ofs.write(reinterpret_cast<char*>(&(mesh->Normals[0])), sizeof(Vector3) * norc);
    // write tex coords
    ofs.write(reinterpret_cast<char*>(&texc), sizeof(texc));
    ofs.write(reinterpret_cast<char*>(&(mesh->TexCoords[0])), sizeof(Vector2) * texc);
    // write tangents
    ofs.write(reinterpret_cast<char*>(&tanc), sizeof(tanc));
    ofs.write(reinterpret_cast<char*>(&(mesh->Tangents[0])), sizeof(Vector4) * tanc);
    // write bitans
    ofs.write(reinterpret_cast<char*>(&bitanc), sizeof(bitanc));
    ofs.write(reinterpret_cast<char*>(&(mesh->Bitangents[0])), sizeof(Vector3) * bitanc);
        
}

void SerializeMaterial(std::ofstream& ofs, Material* mat){
    
}


}
}
