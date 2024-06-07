#ifndef IMESH_H
#define IMESH_H

#include <vector>
#include "Directx/Directx.h"

typedef struct BoundingBoxStruct {
	D3DXVECTOR3 minPoint;
	D3DXVECTOR3 maxPoint;
} BoundingBox;

typedef struct BoundingSphereStruct {
	D3DXVECTOR3 center;
	float radius;
} BoundingSphere;

typedef struct TStripStruct {
	int firstVertex;
	int numTriangles;
} TStrip;
typedef std::vector<TStrip *> StripVector;

/**
* Interface of a mesh for objects. This has number of total vertex, number of total
* triangles, bounding box and bounding sphere
* material vector.
*/

class IMesh
{
protected:
	int numTotalVertices;
	int numTotalTriangles;
	StripVector stripVector;

	BoundingBox boundingBox;
	BoundingSphere boundingSphere;

public:
	IMesh() {};
	IMesh(int pNumTotalVertices, int pNumTotalTriangles);

	virtual ~IMesh() {
		OutputDebugString("Destroying Mesh...\n");

		// Destroy strip vector
		while(!stripVector.empty()) {
			unsigned int size = stripVector.size()-1;
			TStrip * strip= stripVector[size];
			if(strip)
				delete strip, strip=NULL;
			stripVector.pop_back();
		}
	};

	virtual void render(int firstVertex=0, int numTriangles=-1) const = 0;
	virtual void renderStrip(int index) const = 0;
	virtual void renderStripTriangles(int index, int numTriangles) const = 0;
	virtual void loadToVRAM( ) = 0;
	virtual void freeSystemMemory( ) = 0;
	virtual void *getRawVertices( ) const = 0;
	virtual void *getRawIndexes( ) const = 0;

	int getNumTotalVertices( ) const { return numTotalVertices; };
	int getNumTotalTriangles( ) const { return numTotalTriangles; };
	const D3DXVECTOR3 & getMinLocalPoint( ) const { return boundingBox.minPoint; };
	const D3DXVECTOR3 & getMaxLocalPoint( ) const { return boundingBox.maxPoint; };
	const D3DXVECTOR3 & getBoundingSphereCenter( ) const { return boundingSphere.center; };
	float getBoundingSphereRadius( ) const { return boundingSphere.radius; };

	void constructBoundingBox(const D3DXVECTOR3 &min, const D3DXVECTOR3 &max);
	virtual void constructBoundingSphere() = 0;
	void fixBoundingBoxAndSphereForCal3D( );

	void drawBoundingBox() const;
	void drawAxisBoundingSphere() const;
};

typedef std::vector<IMesh *> MeshVector;

#endif //IMESH_H