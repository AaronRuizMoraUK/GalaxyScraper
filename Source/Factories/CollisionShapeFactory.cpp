#include "CollisionShapeFactory.h"

CollisionShapeFactory * CollisionShapeFactory::collisionShapeFactory = NULL;

CollisionShapeFactory * CollisionShapeFactory::getCollisionShapeFactory() {
	if(collisionShapeFactory == NULL)
		collisionShapeFactory = new CollisionShapeFactory();

	return collisionShapeFactory;
}

TypeShape CollisionShapeFactory::getTypeShape( const std::string &type ) {
	if( type == "box" )
		return BOX_SHAPE;
	else if ( type == "cylinder" ) 
		return CYLINDER_SHAPE;
	else if ( type == "sphere" ) 
		return SPHERE_SHAPE;
	else if ( type == "cone" ) 
		return CONE_SHAPE;
	else {
		assert( !"Type of Collision Shape unknown" );
		return BOX_SHAPE;
	}
}

ShapeHandle * CollisionShapeFactory::createCollisionShape(const std::string &filename, const IMesh *mesh, TypeShape typeShape, bool fixY) {
	assert( mesh != NULL );
	CollisionShapeMap::const_iterator it = collisionShapeMap.find(filename);

	if(it != collisionShapeMap.end())
		return it->second;
	else {
		ShapeHandle *shape = NULL;

		if( typeShape==ARACHNID_SHAPE )
			shape = createCylinderShape(mesh, fixY, 1.2f);
		else if( typeShape==PLAYER_SHAPE )
			shape = createCylinderShape(mesh, fixY, 1.2f);
		else if( typeShape==LEVER_SHAPE )
			shape = createBoxShape(mesh, fixY, 2.0f);
		else if( typeShape==DOOR_SHAPE )
			shape = createBoxShape(mesh, fixY, 2.0f);
		else if( typeShape==BOILER_SHAPE )
			shape = createCylinderShape(mesh, fixY, 0.7f);
		else if( typeShape==FIREBALL_SHAPE )
			shape = createSphereShape(mesh, fixY, 1.0f);
		else if( typeShape==CYLINDER_SHAPE )
			shape = createCylinderShape(mesh, fixY);
		else if ( typeShape==SPHERE_SHAPE )
			shape = createSphereShape(mesh, fixY);
		else if ( typeShape==BOX_SHAPE )
			shape = createBoxShape(mesh, fixY);
		else if ( typeShape==CONE_SHAPE )
			shape = createConeShape(mesh, fixY);
		else {
			assert( !"Trying to create a collision shape that does not exist" );
			return NULL;
		}

		if(shape==NULL)
			return NULL;

		collisionShapeMap[filename]=shape;
		return shape;
	}
}

ShapeHandle * CollisionShapeFactory::createCylinderShape(const IMesh *mesh, bool fixY, float bias) {

	const D3DXVECTOR3 & minLocalPoint = mesh->getMinLocalPoint();
	const D3DXVECTOR3 & maxLocalPoint = mesh->getMaxLocalPoint();

	// Get radius
	float radiusX = (maxLocalPoint.x - minLocalPoint.x)/2.0f;
	float radiusZ = (maxLocalPoint.z - minLocalPoint.z)/2.0f;
	float radius = max(radiusX, radiusZ) * bias; // With bias adjust we get a best radius for gameplay

	// Get Height
	float height = maxLocalPoint.y - minLocalPoint.y;

	// Create sphere shape
	ShapeHandle *collisionShapeHandle = new ShapeHandle();
	collisionShapeHandle->shapeHandle = DT_NewCylinder(radius, height);

	if( fixY )
		collisionShapeHandle->collisionFixY = height/2.0f;

	return collisionShapeHandle;
}

ShapeHandle * CollisionShapeFactory::createSphereShape(const IMesh *mesh, bool fixY, float bias) {
	// Get radius
	float radius = mesh->getBoundingSphereRadius();

	// Create spherical shape
	ShapeHandle *collisionShapeHandle = new ShapeHandle();
	collisionShapeHandle->shapeHandle = DT_NewSphere(radius*bias); // With bias adjust we get a best radius for gameplay

	if( fixY )
		collisionShapeHandle->collisionFixY = radius;

	return collisionShapeHandle;
}

ShapeHandle * CollisionShapeFactory::createBoxShape(const IMesh *mesh, bool fixY, float bias) {

	const D3DXVECTOR3 & minLocalPoint = mesh->getMinLocalPoint();
	const D3DXVECTOR3 & maxLocalPoint = mesh->getMaxLocalPoint();

	float x = maxLocalPoint.x - minLocalPoint.x; 
	float y = maxLocalPoint.y - minLocalPoint.y;
	float z = maxLocalPoint.z - minLocalPoint.z;

	// Create box shape
	ShapeHandle *collisionShapeHandle = new ShapeHandle();
	collisionShapeHandle->shapeHandle = DT_NewBox(x*bias, y*bias, z*bias); // With bias adjust we get a best radius for gameplay

	if( fixY )
		collisionShapeHandle->collisionFixY = y;

	return collisionShapeHandle;
}

ShapeHandle * CollisionShapeFactory::createConeShape(const IMesh *mesh, bool fixY, float bias) {

	const D3DXVECTOR3 & minLocalPoint = mesh->getMinLocalPoint();
	const D3DXVECTOR3 & maxLocalPoint = mesh->getMaxLocalPoint();

	// Get radius
	float radiusX = (maxLocalPoint.x - minLocalPoint.x)/2.0f;
	float radiusZ = (maxLocalPoint.z - minLocalPoint.z)/2.0f;
	float radius = max(radiusX, radiusZ);

	// Get Height
	float height = maxLocalPoint.y - minLocalPoint.y;

	// Create cone shape
	ShapeHandle *collisionShapeHandle = new ShapeHandle();
	collisionShapeHandle->shapeHandle = DT_NewCone(radius, height);

	if( fixY )
		collisionShapeHandle->collisionFixY = height/2.0f;

	return collisionShapeHandle;
}

/*
// -- PROVISIONAL COLISIONS --
#include "SOLID.h"

DT_ShapeHandle playerCapsuleCollisionHandle = NULL;
DT_ShapeHandle mouthSphereCollisionHandle = NULL;
DT_ObjectHandle playerCapsuleObjectHandle = NULL;
DT_ObjectHandle mouthSphereObjectHandle = NULL;
DT_SceneHandle sceneHandle = NULL;
DT_RespTableHandle respTableHandle = NULL;

DT_ShapeHandle object1ShapeHandle = NULL;
DT_ObjectHandle object1ObjectHandle = NULL;
DT_ShapeHandle object2ShapeHandle = NULL;
DT_ObjectHandle object2ObjectHandle = NULL;

// Callback Function
DT_Bool collisionDetected(void *clientData, void *clientObject1, void *clientObject2, 
						  const DT_CollData *collData)
{
	char msg[ 128 ];
	sprintf_s(msg, sizeof(msg), "Player-Ass Collision\n");
	OutputDebugString(msg);

	return DT_CONTINUE;
}

DT_Bool collisionStaticDetected(void *clientData, void *clientObject1, void *clientObject2, 
						  const DT_CollData *collData)
{
	char msg[ 128 ];
	sprintf_s(msg, sizeof(msg), "Between statics Collision\n");
	OutputDebugString(msg);

	return DT_CONTINUE;
}
// -----------------

// -- PROVISIONAL COLISIONS --
	// SOLID
	// Player Shape
	// Calculate height, width, large
	const D3DXVECTOR3 & playerMinLocalPoint = player->getMesh()->getMinLocalPoint();
	const D3DXVECTOR3 & playerMaxLocalPoint = player->getMesh()->getMaxLocalPoint();
	const float playerCenterX = (playerMaxLocalPoint.x + playerMinLocalPoint.x)/2.0f;
	const float playerCenterZ = (playerMaxLocalPoint.z + playerMinLocalPoint.z)/2.0f;
	const D3DXVECTOR3 playerMinCenterPoint(playerCenterX, playerMinLocalPoint.y, playerCenterZ);
	const D3DXVECTOR3 playerMaxCenterPoint(playerCenterX, playerMaxLocalPoint.y, playerCenterZ);
	playerCapsuleCollisionHandle = DT_NewLineSegment(playerMinCenterPoint, playerMaxCenterPoint);

	// Capsule radius for the margin
	const float playerWidth = playerMaxLocalPoint.x - playerMinLocalPoint.x;
	const float playerLarge = playerMaxLocalPoint.z - playerMinLocalPoint.z;
	const float playerCapsuleRadius = max(playerWidth, playerLarge);

	// Player Object
	playerCapsuleObjectHandle = DT_CreateObject(player, playerCapsuleCollisionHandle);
	DT_SetMargin(playerCapsuleObjectHandle, playerCapsuleRadius);
	const D3DXMATRIX &playerTransform = player->getTransformMatrix();
	DT_SetMatrixf(playerCapsuleObjectHandle, playerTransform);

	// Mouth Shape
	IntelligentObject * mouth = iuranusPlanetElementsManager->find("ass")->second;
	const float &mouthSphereRadius = mouth->getMesh()->getBoundingSphereRadius();
	mouthSphereCollisionHandle = DT_NewBox(mouthSphereRadius, mouthSphereRadius+5.0f, mouthSphereRadius);

	// Mouth Object
	mouthSphereObjectHandle = DT_CreateObject(mouth, mouthSphereCollisionHandle);
	const D3DXMATRIX &mouthTransform = mouth->getTransformMatrix();
	DT_SetMatrixf(mouthSphereObjectHandle, mouthTransform);

	// TWO OBJECTS IN THE SAME POSITION
	IntelligentObject * mouthTrue = iuranusPlanetElementsManager->find("mouth")->second;
	const D3DXMATRIX &mouthTrueTransform = mouthTrue->getTransformMatrix();
	object1ShapeHandle = DT_NewBox(10,10,10);
	object1ObjectHandle = DT_CreateObject(mouthTrue, object1ShapeHandle);
	DT_SetMatrixf(object1ObjectHandle, mouthTrueTransform);

	object2ShapeHandle = DT_NewBox(10,10,10);
	object2ObjectHandle = DT_CreateObject(mouthTrue, object1ShapeHandle);
	DT_SetMatrixf(object2ObjectHandle, mouthTrueTransform);

	// Scene
	sceneHandle = DT_CreateScene();
	DT_AddObject(sceneHandle, playerCapsuleObjectHandle);
	DT_AddObject(sceneHandle, mouthSphereObjectHandle);
	DT_AddObject(sceneHandle, object1ObjectHandle);
	DT_AddObject(sceneHandle, object2ObjectHandle);

	// Response Table
	respTableHandle = DT_CreateRespTable();

	// Response Class
	DT_ResponseClass responseClass = DT_GenResponseClass(respTableHandle);
	DT_SetResponseClass(respTableHandle, playerCapsuleObjectHandle, responseClass);
	DT_SetResponseClass(respTableHandle, mouthSphereObjectHandle, responseClass);

	//DT_AddClassResponse(respTableHandle, responseClass, &collisionDetected, DT_DEPTH_RESPONSE, NULL);

	// Response Class 2 (Static objects)
	DT_ResponseClass responseClassStatics = DT_GenResponseClass(respTableHandle);
	DT_SetResponseClass(respTableHandle, object1ObjectHandle, responseClassStatics);
	DT_SetResponseClass(respTableHandle, object2ObjectHandle, responseClassStatics);

	//DT_RemovePairResponse(respTableHandle, responseClass, responseClassStatics, &collisionStaticDetected);
	//DT_AddPairResponse(respTableHandle, responseClass, responseClassStatics, &collisionStaticDetected, DT_DEPTH_RESPONSE, NULL);
	// -----------------

// -- PROVISIONAL COLISIONS --
	if( respTableHandle != NULL )
		DT_DestroyRespTable(respTableHandle), respTableHandle=NULL;

	if( sceneHandle != NULL )
		DT_DestroyScene(sceneHandle), sceneHandle=NULL;

	if( playerCapsuleObjectHandle != NULL )
		DT_DestroyObject(playerCapsuleObjectHandle), playerCapsuleObjectHandle=NULL;

	if( mouthSphereObjectHandle != NULL )
		DT_DestroyObject(mouthSphereObjectHandle), mouthSphereObjectHandle=NULL;

	if( playerCapsuleCollisionHandle != NULL )
		DT_DeleteShape(playerCapsuleCollisionHandle), playerCapsuleCollisionHandle=NULL;

	if( mouthSphereCollisionHandle != NULL )
		DT_DeleteShape(mouthSphereCollisionHandle), mouthSphereCollisionHandle=NULL;

	if( object1ObjectHandle != NULL )
		DT_DestroyObject(object1ObjectHandle), object1ObjectHandle=NULL;

	if( object2ObjectHandle != NULL )
		DT_DestroyObject(object2ObjectHandle), object2ObjectHandle=NULL;

	if( object1ShapeHandle != NULL )
		DT_DeleteShape(object1ShapeHandle), object1ShapeHandle=NULL;

	if( object2ShapeHandle != NULL )
		DT_DeleteShape(object2ShapeHandle), object2ShapeHandle=NULL;
	// -----------------

		// -- PROVISIONAL COLISIONS --
		// Update Player Collisions
		const D3DXMATRIX &playerTransform = player->getTransformMatrix();
		DT_SetMatrixf(playerCapsuleObjectHandle, playerTransform);

		// Do collision test
		DT_Test(sceneHandle, respTableHandle);
		// -----------------
*/
