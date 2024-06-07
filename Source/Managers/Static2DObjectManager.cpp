#include "Static2DObjectManager.h"
#include "Global/GlobalVariables.h"
#include "Factories/MeshFactory.h"
#include "Managers/DataLocatorManager.h"
#include "Managers/PlanetManager.h"
#include "Source/Game.h"
#include <cassert>

Static2DObjectManager * Static2DObjectManager::static2DObjectManager = NULL;

Static2DObjectManager * Static2DObjectManager::getStatic2DObjectManager() {
	if(static2DObjectManager == NULL)
		static2DObjectManager = new Static2DObjectManager();

	return static2DObjectManager;
}

Static2DObjectManager::Static2DObjectManager()
: currentParsed(NULL)
, currentParsedElement( 0 )
{
	init();
}

void Static2DObjectManager::init()
{
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToXml = dataLocatorManager->getPath("xml");

	std::string xmlFile = pathToXml + "static_2d_objects.xml";
	bool ok = xmlParseFile(xmlFile);
	assert(ok);
}

void Static2DObjectManager::onEndElement (const std::string &elem)
{
	if( elem == "static_2d_object" )
	{
		// USED DRAWN VARIABLE HERE TO KNOW IF GODRAY MUST BE RENDERED OR NOT
		currentParsed->setDrawn( true );

		(*this)[currentParsedElement] = currentParsed;
		//push_back(currentParsed);
		currentParsed = NULL;
		currentParsedElement = 0;
	}
}

void Static2DObjectManager::onStartElement (const std::string &elem, MKeyValue &atts) 
{
	if( currentParsed ) {
		createElement( elem, atts );
		return;
	}
	if( elem == "static_2d_object" )
	{
		currentParsed = new Object();
		currentParsedElement = atts.getInt( "id", 0 );
		assert( currentParsedElement != 0 );
	}
}

void Static2DObjectManager::createElement(const std::string &elem, MKeyValue &atts) 
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
	}
	else if( elem == "material" )
	{
		std::string texture = atts["name"];
		std::string shader = atts["shader"];
		std::string technique = atts["technique"];
		currentParsed->SETMATERIAL(texture, shader, technique);
 	}
}

void Static2DObjectManager::render() const {
	const std::string &quality = Game::configOptions.quality;
	if( quality == "minimum" )
		return;

	const_iterator i = begin();

	while(i != end() ) {
		const Object *obj = i->second;

		// IF OBJECT MUST BE RENDERED
		if( obj->getDrawn() ) {
			const IMesh * mesh = obj->getMesh();
			const D3DXVECTOR3 &center = mesh->getBoundingSphereCenter();
			float radius = mesh->getBoundingSphereRadius();

			D3DXVECTOR3 centerInstance = center + obj->getPosition();
			if( Global::frustum.isBSphereVisible(centerInstance, radius) &&
				Global::backSideFrustum.isBSphereVisible(centerInstance, radius) ) 
			{
				obj->render();
			}
		}
		++i;
	}
}

void Static2DObjectManager::activeGodray( int id, bool enabled ) {
	Object *godray = getStatic2DObject( id );
	if( godray )
		godray->setDrawn( enabled );
}

Object * Static2DObjectManager::getStatic2DObject( int id ) {
	iterator it = find( id );

	if( it == end( ) ) {
		assert( !"Object not found in Static2DObjectManager" );
		return NULL;
	}

	return it->second;
}

void Static2DObjectManager::cleanUp() {
	iterator it = begin();
	while(it != end())
	{	
		Object * object = (it->second);
		if(object)
			delete object, object=NULL;
		++it;
	}
	clear();
}

void Static2DObjectManager::reloadShader() {
	iterator it = begin();
	while( it != end() ) {
		it->second->reloadShader();
		++it;
	}
}
