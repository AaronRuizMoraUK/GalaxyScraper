#include "StaticObjectManager.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Factories/MeshFactory.h"
#include "Managers/DataLocatorManager.h"
#include "Managers/PlanetManager.h"
#include <fstream>
#include <cassert>

StaticObjectManager * StaticObjectManager::staticObjectManager = NULL;

StaticObjectManager * StaticObjectManager::getStaticObjectManager() {
	if(staticObjectManager == NULL)
		staticObjectManager = new StaticObjectManager();

	return staticObjectManager;
}

StaticObjectManager::StaticObjectManager()
: currentParsed(NULL)
, boiler_boiler(NULL)
{
	init();
}

void StaticObjectManager::init()
{
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToXml = dataLocatorManager->getPath("xml");

	std::string xmlFile = pathToXml + "static_objects.xml";
	bool ok = xmlParseFile(xmlFile);
	assert(ok);
}

void StaticObjectManager::onEndElement (const std::string &elem)
{
	if( elem == "static_object" )
	{
		push_back(currentParsed);
		currentParsed = NULL;
	}
}

void StaticObjectManager::onStartElement (const std::string &elem, MKeyValue &atts) 
{
	if( currentParsed ) {
		createElement( elem, atts );
		return;
	}
	if( elem == "static_object" )
	{
		currentParsed = new CollisionableObject();
	}
}

void StaticObjectManager::createElement(const std::string &elem, MKeyValue &atts) 
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

		currentParsed->setTransformMatrix(matrix);

		std::string planet_name = atts["planet"];
		if( planet_name != "" ) {
			const Planet *planet = PlanetManager::getPlanetManager()->getPlanet( planet_name );
			const D3DXVECTOR3 &planet_center = planet->getCenter();
			currentParsed->setPosition( currentParsed->getPosition() + planet_center );
		}
	}
	else if( elem == "mesh" )
	{
		std::string coreMesh = atts["name"];
		
		// Create Cores
		IMesh * mesh = MeshFactory::getMeshFactory()->createMesh(coreMesh);
		assert(mesh);
		currentParsed->setMesh(mesh);

		// If it is the boiler it has collisions and save a pointer
		if( coreMesh == "boiler_boiler.mesh" ) {
			// Create collision object
			ShapeHandle *collisionShapeHandle = 
				CollisionShapeFactory::getCollisionShapeFactory()->createCollisionShape("boiler_boiler"
				, currentParsed->getMesh(), BOILER_SHAPE, true);
			assert(collisionShapeHandle);
			DT_ObjectHandle collisionObjectHandle = DT_CreateObject(currentParsed, collisionShapeHandle->shapeHandle);
			currentParsed->setCollisionShape(collisionShapeHandle);
			currentParsed->setCollisionObject(collisionObjectHandle);
			// Do know the setTransformMatrix for update also the collision position in world
			currentParsed->setTransformMatrix( currentParsed->getTransformMatrix() );

			boiler_boiler = currentParsed;
		}
	}
	else if( elem == "material" )
	{
		std::string texture = atts["name"];
		std::string shader = atts["shader"];
		std::string technique = atts["technique"];
		currentParsed->SETMATERIAL(texture, shader, technique);
 	}
}

void StaticObjectManager::render() const {
	const_iterator i = begin();

#ifdef DEBUG
	int staticElementsDrawn = 0;

	while(i != end()) {
#else
	// If not debug, not print the last element -> spatial sphere
	assert( !empty() );
	const_iterator itEnd = --end();
	while(i != itEnd) {
#endif

		const Object *obj = (*i);

		const IMesh * mesh = obj->getMesh();
		const D3DXVECTOR3 &center = mesh->getBoundingSphereCenter();
		float radius = mesh->getBoundingSphereRadius();

		D3DXVECTOR3 centerInstance = center + obj->getPosition();
		if( Global::frustum.isBSphereVisible(centerInstance, radius) &&
			Global::backSideFrustum.isBSphereVisible(centerInstance, radius) ) 
		{
#ifdef DEBUG
			staticElementsDrawn++;
#endif
			obj->render();
		}
		++i;
	}

#ifdef DEBUG
	Global::print(0,40, 0xffff0000, "Static objects drawn: %d", staticElementsDrawn);
#endif 
}

void StaticObjectManager::cleanUp() {
	while(!empty()) {
		CollisionableObject * collisionableObject= at(size()-1);
		if(collisionableObject)
			delete collisionableObject, collisionableObject=NULL;
		pop_back();
	}
	boiler_boiler = NULL;
}

void StaticObjectManager::reloadShader() {
	iterator it = begin();
	while( it != end() ) {
		(*it)->reloadShader();
		++it;
	}
}
