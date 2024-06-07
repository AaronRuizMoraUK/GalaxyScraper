#ifndef COLLISIONABLE_OBJECT_H
#define COLLISIONABLE_OBJECT_H

#include "Object/Object.h"
#include "SOLID.h"
#include "Factories/CollisionShapeFactory.h"

/**
* A collisionable object in scene. This has a Solid shape and
* an object.
*/

class CollisionableObject : public Object
{
protected:
	ShapeHandle *collisionShapeHandle;
	DT_ObjectHandle collisionObjectHandle;

public:
	// Default constructor
	CollisionableObject();
	// Override copy constructor
	CollisionableObject(const CollisionableObject &source);
	// Destructor
	virtual ~CollisionableObject() {
		OutputDebugString("Destroying CollisionableObject...\n");
		clear();
	};
	// Override Operator '='
	virtual void operator=(const CollisionableObject &source) {
		Object::operator=(source);
		clear();
		copy(source);
	};

	const DT_ShapeHandle getCollisionShape() const {
		if(collisionShapeHandle!=NULL)
			return collisionShapeHandle->shapeHandle;
		return NULL;
	};
	const DT_ObjectHandle getCollisionObject() const {return collisionObjectHandle;};
	void setCollisionShape(ShapeHandle *shapeHandle) {collisionShapeHandle=shapeHandle;};

	void setCollisionObject(DT_ObjectHandle objectHandle) {
		if( collisionObjectHandle != NULL )
			DT_DestroyObject(collisionObjectHandle), collisionObjectHandle=NULL;
		collisionObjectHandle=objectHandle;
	};

	// These functions firts act in Object field and then act in colision field
	void setTransformMatrix(const D3DXMATRIX &pTransformMatrix);
	void setPosition(const D3DXVECTOR3 &pPosition);
	void setAngles(float yaw, float pitch);

private:
	void setCollisionPosition();
	void setCollisionTransformMatrix();

	void clear();
	void copy(const CollisionableObject &source);
};

#endif //COLLISIONABLE_OBJECT_H
