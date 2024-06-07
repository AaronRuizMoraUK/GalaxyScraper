#include "PlanetManager.h"
#include "Global/GlobalVariables.h"
#include "Factories/MeshFactory.h"
#include "Managers/DataLocatorManager.h"
#include <fstream>
#include <cassert>

PlanetManager * PlanetManager::planetManager = NULL;

PlanetManager * PlanetManager::getPlanetManager() {
	if(planetManager == NULL)
		planetManager = new PlanetManager();

	return planetManager;
}

PlanetManager::PlanetManager()
: currentParsed(NULL)
, currentParsedPlanetName("")
, currentPlanetName("")
, currentPlanet(NULL)
{
	init();
}

void PlanetManager::init()
{
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToXml = dataLocatorManager->getPath("xml");

	std::string xmlFile = pathToXml + "planets.xml";
	bool ok = xmlParseFile(xmlFile);
	assert(ok);
}

void PlanetManager::onEndElement (const std::string &elem)
{
	if( elem == "planet" )
	{
		(*this)[currentParsedPlanetName] = currentParsed;
		currentParsed = NULL;
	}
}

void PlanetManager::onStartElement (const std::string &elem, MKeyValue &atts) 
{
	if( currentParsed ) {
		createElement( elem, atts );
		return;
	}
	if( elem == "planets" )
	{
		currentPlanetName = atts["initial_planet"];
	}
	else if( elem == "planet" )
	{
		currentParsed = new Planet();
		currentParsedPlanetName = atts["name"];
		currentParsed->setName(currentParsedPlanetName);

		// If this planet is the initial_planet, set as current planet
		if( currentParsedPlanetName == currentPlanetName )
			currentPlanet = currentParsed;
	}
}

void PlanetManager::createElement(const std::string &elem, MKeyValue &atts) 
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

		float radius = atts.getFloat( "radius", 0.0f );

		currentParsed->setRadius(radius);
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
	else if( elem == "heightmap" ) {
		std::string heightmap = atts["name"];

		// Set heightmap to the planet
		DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
		const std::string &pathToHeightMaps = dataLocatorManager->getPath("heightmap");

		currentParsed->constructHeightMap( pathToHeightMaps + heightmap );
	}
}

/*
void PlanetManager::read(const char *fileName) {
	std::ifstream is (fileName);
	assert( is.is_open() );

	int nPlanets;
	is >> nPlanets;

	std::string currentPlanetString;
	is >> currentPlanetString;

	for (int i=0; i<nPlanets; ++i) {
		Planet *planet = new Planet();

		std::string name;
		is >> name;

		planet->setName(name);

		D3DXMATRIX transformMatrix;
		D3DXMatrixIdentity(&transformMatrix);
		for (int row=0; row<4; ++row) {
			for (int col=0; col<3; ++col) {
				float data;
				is >> data;
				transformMatrix(row,col)=data;
			}
		}
		planet->setTransformMatrix(transformMatrix);

		float radius;
		is >> radius;

		planet->setRadius(radius);

		std::string coreMesh;
		is >> coreMesh;

		// Create Cores
		IMesh * mesh = MeshFactory::getMeshFactory()->createMesh(coreMesh);
		assert(mesh);
		planet->setMesh(mesh);

		std::string texture;
		is >> texture;
		std::string shader;
		is >> shader;
		std::string technique;
		is >> technique;

		// Create Material
		planet->SETMATERIAL(texture, shader, technique);

		// TODO Get CoreMesh From CoreMeshVector or Factory
		// include a CoreName in txt, so we can read and search

		// TODO Get Materials From MaterialVector (Manager) or Factory

		// Insert the planet in the list
		(*this)[name] = planet;

		if(currentPlanetString==name) {
			currentPlanet = planet;
			currentPlanetName = name;
			// TODO
			currentPlanet->constructHeightMap("./data/iuranus_surface_heightmap.bin");
		}
	}
	
	is.close();
}
*/

void PlanetManager::render() const {
	const_iterator i = begin();

	while(i!=end()) {
		// Do not render the iuranus planet, because all planet now is a cal3D mesh
		const Planet *obj = i->second;
		if(  currentPlanetName != "iuranus" || obj->getName() != "iuranus" ) {

			const IMesh * mesh = obj->getMesh();
			const D3DXVECTOR3 &center = mesh->getBoundingSphereCenter();
			float radius = mesh->getBoundingSphereRadius();

			D3DXVECTOR3 centerInstance = center + obj->getPosition();
			if( Global::frustum.isBSphereVisible(centerInstance, radius) &&
				Global::backSideFrustum.isBSphereVisible(centerInstance, radius) ) {
				obj->render();
			}
		}
		++i;
	}
}

const Planet * PlanetManager::getPlanet( const std::string &name) const {
	const_iterator it = begin();
	while(it != end())
	{	
		if( (it->first) == name )
			return (it->second);
		++it;
	}

	assert( !"Planet not exists in PlanetManger" );
	return NULL;
}

void PlanetManager::cleanUp() {
	iterator it = begin();
	while(it != end())
	{	
		Planet *planet = (it->second);
		if(planet)
			delete planet, planet=NULL;
		++it;
	}
	clear();
}

void PlanetManager::reloadShader() {
	iterator it = begin();
	while( it != end() ) {
		(it->second)->reloadShader();
		++it;
	}
}
