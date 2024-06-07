#include "LogicVolumeManager.h"
#include "Logic/LogicVolume/LogicBox.h"
#include "Logic/LogicVolume/LogicSphere.h"
#include "Managers/DataLocatorManager.h"
#include "Managers/PlanetManager.h"
#include <cassert>

LogicVolumeManager::LogicVolumeManager()
: planetName( "" )
{
}

void LogicVolumeManager::load( const std::string &pPlanetName ) {
	assert( pPlanetName != "" );
	planetName = pPlanetName;

	cleanUp();

	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToXml = dataLocatorManager->getPath("xml");

	std::string xmlFile = pathToXml + planetName + "/" + planetName + "_logic_volumes.xml";
	bool ok = xmlParseFile(xmlFile);
	assert(ok);
}

void LogicVolumeManager::update()
{
	iterator it = begin();
	while(it != end())
	{
		if( (it->second)->recalcForPlayer() )
			(it->second)->recalc(planetName);
		++it;
	}
}

void LogicVolumeManager::onStartElement (const std::string &elem, MKeyValue &atts) 
{
	if (elem == "volume") 
	{
		std::string type = atts["type"];
		std::string volumeName = atts["name"];

		int tmpCheck = atts.getInt( "check_for_player", 0 );
		bool checkForPlayer = (tmpCheck == 1) ? true : false;

		int tmpInside = atts.getInt("check_is_inside", 0 );
		bool checkIsInside = (tmpInside == 1) ? true : false;

		if( type == "box" )
		{
			std::string position=atts["transform"];
			D3DXMATRIX matrix;
			D3DXMatrixIdentity(&matrix);
			sscanf_s(position.c_str(),"%f %f %f %f %f %f %f %f %f %f %f %f",
				&matrix(0,0), &matrix(0,1), &matrix(0,2),
				&matrix(1,0), &matrix(1,1), &matrix(1,2),
				&matrix(2,0), &matrix(2,1), &matrix(2,2),
				&matrix(3,0), &matrix(3,1), &matrix(3,2));

			float width = atts.getFloat("width", 0.0f);
			float height = atts.getFloat("height", 0.0f);
			float length = atts.getFloat("length", 0.0f);

			D3DXVECTOR3 pMin(-width/2.0f, 0.0f, -length/2.0f);
			D3DXVECTOR3 pMax(width/2.0f, height, length/2.0f);
	
			D3DXVec3TransformCoord(&pMin, &pMin, &matrix);
			D3DXVec3TransformCoord(&pMax, &pMax, &matrix);
			
			LogicVolume *logicBox = new LogicBox(volumeName, checkForPlayer, checkIsInside, pMin, pMax);

			const Planet *planet = PlanetManager::getPlanetManager( )->getPlanet( planetName );
			logicBox->setParentObject( planet ); // The parent is the current planet
			logicBox->updatePositions( ); // This updates its positions too

			(*this)[volumeName] = logicBox;
		}
		else if ( type == "sphere" )
		{
			D3DXVECTOR3 center(0,0,0);
			sscanf_s( atts["center"].c_str(), "%f %f %f", &center.x, &center.y, &center.z );
			float radius = atts.getFloat( "radius", 0.0f );
		
			LogicVolume *logicSphere = new LogicSphere(volumeName, checkForPlayer, checkIsInside, center, radius);

			const Planet *currentPlanet = PlanetManager::getPlanetManager( )->getPlanet( planetName );
			logicSphere->setParentObject( currentPlanet ); // The parent is the current planet
			logicSphere->updatePositions( ); // This updates its positions too

			(*this)[volumeName] = logicSphere;
		}
	}
}

void LogicVolumeManager::cleanUp() {

	iterator it = begin();
	while(it != end())
	{	
		LogicVolume *logicVolume = (it->second);
		if(logicVolume)
			delete logicVolume, logicVolume=NULL;
		++it;
	}
	clear();
}

const LogicVolume * LogicVolumeManager::getElement(const std::string &elem) const
{
	const_iterator it = find(elem);
	if( it == end() ) {
		assert( !"Element not fount in LogicVolumeManager" );
		return NULL;
	}

	return it->second;
}
