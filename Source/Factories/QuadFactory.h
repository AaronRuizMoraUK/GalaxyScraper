#ifndef QUAD_FACTORY_H
#define QUAD_FACTORY_H

#include <map>
#include <vector>
#include <string>
#include "Mesh/IMesh.h"
#include "Util/MapComparers/MapComparers.h"

typedef std::map<std::vector<float>, IMesh *, CompareFloatVector> QuadMeshMap;

/**
* Factory of quads.
*
* This is a singleton.
*/

class QuadFactory
{
protected:
	QuadMeshMap quadMeshMap;

public:
	~QuadFactory() {
		OutputDebugString("Destroying QuadFactory...\n");

		// Destroy quad mesh map
		QuadMeshMap::const_iterator it = quadMeshMap.begin();
		while(it !=  quadMeshMap.end()) {	
			IMesh *mesh = (it->second);
			if(mesh)
				delete mesh, mesh=NULL;
			++it;
		}
		quadMeshMap.clear();
	};
	
	static QuadFactory * getQuadFactory();

	IMesh * createQuad(float radiusSideX, float radiusSideY, int numQuadVertexPerAxis=2);

private:
	static QuadFactory * quadFactory;
	QuadFactory() {};

	IMesh * createQuadMesh(float radiusSideX, float radiusSideY, unsigned int numQuadVertexPerAxis);
};

#endif //QUAD_FACTORY_H
