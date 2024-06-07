#include "PlanetAtmosphereManager.h"
#include "Factories/QuadFactory.h"
#include "Managers/PlanetManager.h"
#include "Object/Planet/Planet.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Managers/DataLocatorManager.h"
#include <fstream>
#include <cassert>

PlanetAtmosphereManager * PlanetAtmosphereManager::planetAtmosphereManager = NULL;

PlanetAtmosphereManager * PlanetAtmosphereManager::getPlanetAtmosphereManager() {
	if(planetAtmosphereManager == NULL)
		planetAtmosphereManager = new PlanetAtmosphereManager();

	return planetAtmosphereManager;
}

PlanetAtmosphereManager::PlanetAtmosphereManager()
: currentParsed(NULL)
, currentParsedPlanetName("")
, atmosphereQuadScale(0.0f)
, atmosphereQuadVertexPerAxis(0)
{
	init();
}

void PlanetAtmosphereManager::init()
{
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToXml = dataLocatorManager->getPath("xml");

	std::string xmlFile = pathToXml + "planets_atmosphere.xml";
	bool ok = xmlParseFile(xmlFile);
	assert(ok);
}

void PlanetAtmosphereManager::assignMesh()
{
	// Obtain the planet radius
	const Planet *planet = PlanetManager::getPlanetManager()->getPlanet(currentParsedPlanetName);
	float planetRadius = planet->getRadius();

	// Create Cores
	IMesh * mesh = QuadFactory::getQuadFactory()->createQuad( planetRadius*atmosphereQuadScale, 
															  planetRadius*atmosphereQuadScale,
															  atmosphereQuadVertexPerAxis);
	assert(mesh);
	currentParsed->setMesh(mesh);
}

void PlanetAtmosphereManager::onEndElement (const std::string &elem)
{
	if( elem == "planet_atmosphere" )
	{
		assignMesh();
		(*this)[currentParsedPlanetName] = currentParsed;
		currentParsed = NULL;
	}
}

void PlanetAtmosphereManager::onStartElement (const std::string &elem, MKeyValue &atts) 
{
	if( currentParsed ) 
	{
		createElement( elem, atts );
		return;
	}

	if( elem == "planet_atmosphere" )
	{
		currentParsed = new Object();
		currentParsedPlanetName = atts["planet_name"];

		D3DXMATRIX transformMatrix;
		D3DXMatrixIdentity(&transformMatrix);
		currentParsed->setTransformMatrix(transformMatrix);
	}
}

void PlanetAtmosphereManager::createElement(const std::string &elem, MKeyValue &atts) 
{
	if( elem == "material" )
	{
		std::string texture = atts["name"];
		std::string shader = atts["shader"];
		std::string technique = atts["technique"];
		currentParsed->SETMATERIAL(texture, shader, technique);
 	}
	else if( elem == "quad" ) {
		atmosphereQuadScale = atts.getFloat( "atmph_quad_scale", 1.25f );
		atmosphereQuadVertexPerAxis = atts.getInt( "atmph_quad_vertex_per_axis", 3 );
		assert( atmosphereQuadVertexPerAxis >= 2 );
	}
}


/*
void PlanetAtmosphereManager::read(const char *fileName) {
	std::ifstream is (fileName);
	assert( is.is_open() );

	int nAtmosphere;
	is >> nAtmosphere;

	for (int i=0; i<nAtmosphere; ++i) {
		Object *object = new Object();

		std::string planetName;
		is >> planetName;

		D3DXMATRIX transformMatrix;
		D3DXMatrixIdentity(&transformMatrix);
		object->setTransformMatrix(transformMatrix);

		std::string texture;
		is >> texture;
		std::string shader;
		is >> shader;
		std::string technique;
		is >> technique;

		// Create Material
		object->SETMATERIAL(texture, shader, technique);

		// Obtain the planet radius
		PlanetManager * planetManager = PlanetManager::getPlanetManager();
		PlanetManager::const_iterator it = planetManager->find(planetName);
		// If planet does not exist delete object and continue with next atmosphere
		if( it == planetManager->end() ){
			delete object;
			continue;
		}
		float planetRadius = (it->second)->getRadius();

		// Create Cores
		IMesh * mesh = QuadFactory::getQuadFactory()->createQuad( planetRadius*ATMPH_QUAD_SCALE, planetRadius*ATMPH_QUAD_SCALE,
			ATMPH_QUAD_VERTEX_PER_AXIS);
		assert(mesh);
		object->setMesh(mesh);

		(*this)[planetName] = object;
	}
	
	is.close();
}
*/

void PlanetAtmosphereManager::update() {
	iterator i = begin();

	while(i!=end()) {
		updateTransformMatrix(i->first, i->second); // Update transform of atmosphere
		++i;
	}
}

void PlanetAtmosphereManager::render() const {

	const std::string &currenPlanetName = PlanetManager::getPlanetManager()->getCurrentPlanetName();

	// Draw all atmoshpheres lees current planet
	const_iterator i = begin();
	const_iterator iCurrentPlanet = end();

	while(i!=end()) {
		if( (i->first) != currenPlanetName ) {
			const Object *obj = i->second;

			const IMesh * mesh = obj->getMesh();
			const D3DXVECTOR3 &center = mesh->getBoundingSphereCenter();
			float radius = mesh->getBoundingSphereRadius();

			D3DXVECTOR3 centerInstance = center + obj->getPosition();
			if( Global::frustum.isBSphereVisible(centerInstance, radius) &&
				Global::backSideFrustum.isBSphereVisible(centerInstance, radius) ) {
				obj->render();
			}
		}
		else {
			iCurrentPlanet = i;
		}
		++i;
	}

	// Draw atmosphere of current planet
	if( iCurrentPlanet == end() )
		return;

	const Object *obj = iCurrentPlanet->second;

	const IMesh * mesh = obj->getMesh();
	const D3DXVECTOR3 &center = mesh->getBoundingSphereCenter();
	float radius = mesh->getBoundingSphereRadius();

	D3DXVECTOR3 centerInstance = center + obj->getPosition();
	if( Global::frustum.isBSphereVisible(centerInstance, radius) &&
		Global::backSideFrustum.isBSphereVisible(centerInstance, radius) ) {

		Global::renderStateDisableZTest(); // Disable Z Test
		obj->render();
		Global::renderStateEnableZTest(); // Enable Z Test

	}
}

void PlanetAtmosphereManager::updateTransformMatrix(const std::string &name, Object *object) {

	D3DXMATRIX transform;
	D3DXMatrixIdentity(&transform);

	// Planet center and radius
	PlanetManager *planetManager = PlanetManager::getPlanetManager();
	const Planet *planet = planetManager->getPlanet(name);
	const D3DXVECTOR3 &center = planet->getCenter();
	float radius = planet->getRadius();

	// Direction
	const D3DXVECTOR3 &cameraPosition = Global::camera->getPosition();
	D3DXVECTOR3 localFront( cameraPosition - center );
	Global::normalizeVector3(localFront, localFront);

	// Position
	D3DXVECTOR3 position(center);
	if( name !=  planetManager->getCurrentPlanetName() )
		position += radius*localFront;
	else
		position += (radius/2.0f)*localFront;

	// Rotation
	const D3DXVECTOR3 &cameraLocalUp = Global::camera->getLocalUpVector();
	D3DXVECTOR3 localLeft;
	D3DXVec3Cross(&localLeft, &cameraLocalUp, &localFront);
	Global::normalizeVector3(localLeft, localLeft);
	D3DXVECTOR3 localUp;
	D3DXVec3Cross(&localUp, &localFront, &localLeft);

	// Update matWorld
	transform(0,0) = localLeft.x;
	transform(0,1) = localLeft.y;
	transform(0,2) = localLeft.z;
	transform(1,0) = localUp.x;
	transform(1,1) = localUp.y;
	transform(1,2) = localUp.z;
	transform(2,0) = localFront.x;
	transform(2,1) = localFront.y;
	transform(2,2) = localFront.z;

	object->setTransformMatrix(transform);
	object->setPosition(position);
}

void PlanetAtmosphereManager::cleanUp() {
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

void PlanetAtmosphereManager::reloadShader() {
	iterator it = begin();
	while( it != end() ) {
		(it->second)->reloadShader();
		++it;
	}
}
