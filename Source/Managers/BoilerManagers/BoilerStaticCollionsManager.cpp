#include "BoilerStaticCollionsManager.h"
#include "Managers/DataLocatorManager.h"
#include "Managers/PlanetManager.h"
#include "Factories/MeshFactory.h"
#include "Global/GlobalVariables.h"
#include <cassert>

BoilerStaticCollisionsManager * BoilerStaticCollisionsManager::boilerStaticCollisionsManager = NULL;

BoilerStaticCollisionsManager * BoilerStaticCollisionsManager::getBoilerStaticCollisionsManager() {
	if(boilerStaticCollisionsManager == NULL)
		boilerStaticCollisionsManager = new BoilerStaticCollisionsManager();

	return boilerStaticCollisionsManager;
}
 
BoilerStaticCollisionsManager::BoilerStaticCollisionsManager()
: currentParsed( NULL )
, currentParsedCollisionName( "" )
, currentParsedCollisionType( "" )
, currentParsedFixY( false )
{
	init();
}

void BoilerStaticCollisionsManager::init()
{
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToXml = dataLocatorManager->getPath("xml");

	std::string xmlFile = pathToXml + "/boiler/boiler_static_collisions.xml";
	bool ok = xmlParseFile(xmlFile);
	assert(ok);
}

void BoilerStaticCollisionsManager::onEndElement (const std::string &elem)
{
	if( elem == "static_collision" )
	{		
		// Get type shape
		TypeShape type_shape = CollisionShapeFactory::getTypeShape( currentParsedCollisionType );

		// Create collision object
		ShapeHandle *collisionShapeHandle = 
			CollisionShapeFactory::getCollisionShapeFactory()->createCollisionShape(currentParsedCollisionName
			, currentParsed->getMesh(), type_shape, currentParsedFixY);
		assert(collisionShapeHandle);
		DT_ObjectHandle collisionObjectHandle = DT_CreateObject(currentParsed, collisionShapeHandle->shapeHandle);
		currentParsed->setCollisionShape(collisionShapeHandle);
		currentParsed->setCollisionObject(collisionObjectHandle);
		// Do know the setTransformMatrix for update also the collision position in world
		currentParsed->setTransformMatrix( currentParsed->getTransformMatrix() );

		push_back(currentParsed);
		currentParsed = NULL;
		currentParsedCollisionName = "";
		currentParsedCollisionType = "";
		currentParsedFixY = false;
	}
}

void BoilerStaticCollisionsManager::onStartElement (const std::string &elem, MKeyValue &atts) 
{
	if( currentParsed ) {
		createElement( elem, atts );
		return;
	}

	if( elem == "static_collision" )
	{
		currentParsed = new CollisionableObject();
		currentParsedCollisionName = atts["name"];
		currentParsedCollisionType = atts["type"];
		currentParsedFixY = atts.getBool( "fix_y", false );
	}
}

void BoilerStaticCollisionsManager::createElement(const std::string &elem, MKeyValue &atts) 
{
	if( elem == "localization" )
	{
		std::string position=atts["transform"];
		D3DXMATRIX matrix;
		D3DXMatrixIdentity(&matrix);
		sscanf_s(position.c_str(),"%f %f %f %f %f %f %f %f %f %f %f %f",
			&matrix(0,0), &matrix(0,1), &matrix(0,2),
			&matrix(1,0), &matrix(1,1), &matrix(1,2),
			&matrix(2,0), &matrix(2,1), &matrix(2,2),
			&matrix(3,0), &matrix(3,1), &matrix(3,2));

		const Planet *currentPlanet = PlanetManager::getPlanetManager()->getPlanet("boiler");
		currentParsed->setParentObject( currentPlanet ); // The parent is the current planet
		currentParsed->setLocalTransformMatrix(matrix); // This is the local transform matrix, it never changes
		currentParsed->updateTransformMatrix( ); // This updates its transform matrix too
	}
	else if( elem == "mesh" )
	{
		std::string name = atts["name"];

		// Create Cores
		IMesh * mesh = MeshFactory::getMeshFactory()->createMesh(name);
		assert(mesh);
		currentParsed->setMesh(mesh);
	}
	else if( elem == "material" )
	{
		std::string texture = atts["texture"];
		std::string shader = atts["shader"];
		std::string technique = atts["technique"];
		currentParsed->SETMATERIAL(texture, shader, technique);
 	}
}

void BoilerStaticCollisionsManager::render( ) const {
	if( true )
		return;

	const_iterator i = begin();

	while(i != end()) {
		const Object *obj = (*i);

		const IMesh * mesh = obj->getMesh();
		const D3DXVECTOR3 &center = mesh->getBoundingSphereCenter();
		float radius = mesh->getBoundingSphereRadius();

		D3DXVECTOR3 centerInstance = center + obj->getPosition();
		if( Global::frustum.isBSphereVisible(centerInstance, radius) &&
			Global::backSideFrustum.isBSphereVisible(centerInstance, radius) ) 
		{
			obj->render();
		}

		++i;
	}
}

void BoilerStaticCollisionsManager::cleanUp() 
{
	iterator it = begin();
	while(it != end())
	{	
		CollisionableObject *collisionableObject = *it;
		if(collisionableObject)
			delete collisionableObject, collisionableObject=NULL;
		++it;
	}
	clear();
}

void BoilerStaticCollisionsManager::reloadShader() 
{
	iterator it = begin();
	while( it != end() ) 
	{
		(*it)->reloadShader();
		++it;
	}
}
