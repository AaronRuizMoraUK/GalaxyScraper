#ifndef COLLISION_SHAPE_FACTORY_H
#define COLLISION_SHAPE_FACTORY_H

#include <Windows.h>
#include <map>
#include <string>
#include "Mesh/IMesh.h"
#include "Util/MapComparers/MapComparers.h"
#include "Solid/SOLID.h"

// Type of shapes
typedef enum EnumTypeShape {PLAYER_SHAPE
						  , ARACHNID_SHAPE
						  , LEVER_SHAPE
						  , DOOR_SHAPE
						  , BOILER_SHAPE
						  , FIREBALL_SHAPE
						  , BOX_SHAPE
						  , CONE_SHAPE
						  , CYLINDER_SHAPE
						  , SPHERE_SHAPE
						  , POINT_SHAPE
						  , LINE_SEGMENT_SHAPE
						  , MINKOWSKI_SHAPE} TypeShape;

typedef struct ShapeHandleStruct {
	DT_ShapeHandle shapeHandle;
	float collisionFixY; // The shape's position is in its geometric center, but in some objects we want it in the base

	ShapeHandleStruct() :shapeHandle(NULL), collisionFixY(0.0f) {};
	~ShapeHandleStruct() {
		OutputDebugString("Destroying ShapeHandle...\n");

		if(shapeHandle!=NULL)
			DT_DeleteShape(shapeHandle) , shapeHandle=NULL;
	};
} ShapeHandle;

typedef std::map<std::string, ShapeHandle *, LessString> CollisionShapeMap;

/**
* Factory of collision shapes.
*
* This is a singleton.
*/

class CollisionShapeFactory
{
protected:
	CollisionShapeMap collisionShapeMap;

public:
	~CollisionShapeFactory() {
		OutputDebugString("Destroying CollisionShapeFactory...\n");

		// Destroy collision shape map
		CollisionShapeMap::const_iterator it = collisionShapeMap.begin();
		while(it !=  collisionShapeMap.end()) {	
			ShapeHandle *collisionShape = (it->second);
			if(collisionShape)
				delete collisionShape, collisionShape=NULL;
			++it;
		}
		collisionShapeMap.clear();
	};
	
	static CollisionShapeFactory * getCollisionShapeFactory();

	static TypeShape getTypeShape( const std::string &type );

	ShapeHandle * createCollisionShape(const std::string &filename, const IMesh *mesh, TypeShape typeShape, bool fixY);

private:
	static CollisionShapeFactory * collisionShapeFactory;
	CollisionShapeFactory() {};

	ShapeHandle *createCylinderShape(const IMesh *mesh, bool fixY, float bias=1.0f);
	ShapeHandle *createSphereShape(const IMesh *mesh, bool fixY, float bias=1.0f);
	ShapeHandle *createBoxShape(const IMesh *mesh, bool fixY, float bias=1.0f);
	ShapeHandle *createConeShape(const IMesh *mesh, bool fixY, float bias=1.0f);
};

#endif //COLLISION_SHAPE_FACTORY_H
