#ifndef MESH_FACTORY_H
#define MESH_FACTORY_H

#include <map>
#include <string>
#include "Mesh/IMesh.h"
#include "Util/MapComparers/MapComparers.h"
#include "cal3d/cal3d.h"

typedef std::map<std::string, IMesh *, LessString> MeshMap;

/**
* Factory of meshes.
*
* This is a singleton.
*/

class MeshFactory
{
protected:
	MeshMap meshMap;

public:
	static const int MAX_BONES = 40;
	static const int MAX_INFLUENCES = 3;

public:
	~MeshFactory(void) {
		OutputDebugString("Destroying MeshFactory...\n");

		// Destroy mesh map
		MeshMap::const_iterator it = meshMap.begin();
		while(it !=  meshMap.end()) {	
			IMesh *mesh = (it->second);
			if(mesh)
				delete mesh, mesh=NULL;
			++it;
		}
		meshMap.clear();
	};
	
	static MeshFactory * getMeshFactory();

	IMesh * createMesh(const std::string &filename);
	IMesh * createMesh(const std::string &name, CalCoreMesh *calMesh);
	
private:
	static MeshFactory * meshFactory;
	MeshFactory() {};

	IMesh * readNoNormalMesh(const char *filename);
	IMesh * readMesh(const char *filename);
	IMesh * convertCalCoreMeshToMesh(CalCoreMesh *calMesh);
	
	struct Triangle {
		unsigned short index[3];
	};

	void CalculateTangentArray(int vertexCount, const D3DXVECTOR3 *vertex, const D3DXVECTOR3 *normal,
						   const D3DXVECTOR2 *texcoord, int triangleCount, const Triangle *triangle, 
						   D3DXVECTOR3 *tangent);
};

#endif //MESH_FACTORY_H
