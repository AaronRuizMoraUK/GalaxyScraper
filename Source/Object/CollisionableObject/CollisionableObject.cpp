#include "CollisionableObject.h"

// Default constructor
CollisionableObject::CollisionableObject()
: Object()
, collisionShapeHandle(NULL)
, collisionObjectHandle(NULL)
{
}

// Override copy constructor
CollisionableObject::CollisionableObject(const CollisionableObject &source)
: Object(source)
, collisionShapeHandle(NULL)
, collisionObjectHandle(NULL)
{
	copy(source);
}

void CollisionableObject::clear() {
	// CollisionObject does not destroy its Collision shape, CollisionShapeFactory does!!.

	// Destroy the collision object because each instance has its own collision object
	if( collisionObjectHandle != NULL )
		DT_DestroyObject(collisionObjectHandle), collisionObjectHandle=NULL;
}

void CollisionableObject::copy(const CollisionableObject &source) {
	if( source.collisionShapeHandle!=NULL && source.collisionObjectHandle!=NULL ) {
		// The collision shape is the same
		collisionShapeHandle = source.collisionShapeHandle;

		// The collision object is a new instance
		DT_ObjectHandle collisionObjectHandleTmp = DT_CreateObject(this, collisionShapeHandle->shapeHandle);
		setCollisionObject( collisionObjectHandleTmp );

		assert(collisionShapeHandle!=NULL && collisionObjectHandle!=NULL);
		setTransformMatrix(source.transformMatrix);
	}
	else {
		collisionShapeHandle = NULL;
		collisionObjectHandle = NULL;
	}

}

void CollisionableObject::setTransformMatrix(const D3DXMATRIX &pTransformMatrix) {
	Object::setTransformMatrix(pTransformMatrix);

	setCollisionTransformMatrix();
}

void CollisionableObject::setPosition(const D3DXVECTOR3 &pPosition) {
	Object::setPosition(pPosition);

	setCollisionPosition();
}

void CollisionableObject::setAngles(float yaw, float pitch) {
	Object::setAngles(yaw, pitch);

	setCollisionTransformMatrix();
}

void CollisionableObject::setCollisionPosition() {
	if( collisionObjectHandle!=NULL ) {
		D3DXVECTOR3 collisionPosition = position + collisionShapeHandle->collisionFixY*localUp;
		DT_SetPosition(collisionObjectHandle, collisionPosition);
	}
}

void CollisionableObject::setCollisionTransformMatrix() {
	if( collisionObjectHandle!=NULL ) {
		DT_SetMatrixf(collisionObjectHandle, transformMatrix);
		setCollisionPosition();
	}
}
