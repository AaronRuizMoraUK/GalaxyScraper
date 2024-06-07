#include "BoilerPlanetLogic.h"
#include "Managers/DataLocatorManager.h"
#include "Managers/LogicManager/LogicManager.h"
#include "Managers/BoilerManagers/CheckpointsManager.h"
#include <cassert>

BoilerPlanetLogic * BoilerPlanetLogic::boilerPlanetLogic = NULL;

BoilerPlanetLogic * BoilerPlanetLogic::getBoilerPlanetLogic() 
{
	if(boilerPlanetLogic == NULL)
		boilerPlanetLogic = new BoilerPlanetLogic();

	return boilerPlanetLogic;
}

BoilerPlanetLogic::BoilerPlanetLogic() 
: IPlanetLogic()
, currentBoilerZone(NULL)
, currentParsedElementId(0)
, numZones(0)
, firstZoneId(0)
, firedRetries(0)
, fireDeaths(0)
, alreadyEnteredBossZone(false)
{
	init();
};

int BoilerPlanetLogic::getFiredRetriesLeft( ) const { 
	assert( firedRetries >= fireDeaths );
	return firedRetries - fireDeaths; 
}

void BoilerPlanetLogic::init()
{
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToXml = dataLocatorManager->getPath("xml");

	std::string xmlFile = pathToXml + "/boiler/boiler_logic_parameters.xml";
	bool ok = xmlParseFile(xmlFile);
	assert(ok);
}

void BoilerPlanetLogic::onStartElement (const std::string &elem, MKeyValue &atts) 
{
	if( elem == "logic" )
	{
		firedRetries = atts.getInt( "fired_retries", 0);
		numZones = atts.getInt( "num_zones", 0 );
		firstZoneId = atts.getInt( "first_zone_id", 0 );
		assert( numZones >= 0 && firstZoneId >= 0 );
	}
	else if( elem == "zone_logic" )
	{
		int zoneId = atts.getInt( "zone_id", 0 );
		assert( zoneId >= 0 );
		currentParsedElementId = zoneId;

		int plat1 = atts.getInt( "platform_enter_zone", 0 );
		int plat2 = atts.getInt( "platform_enter_mid", 0 );
		int plat3 = atts.getInt( "platform_exit_mid", 0 );
		int plat4 = atts.getInt( "platform_exit_zone", 0 );
		int nextZoneId = atts.getInt( "next_zone_id", 0 );
		assert( nextZoneId >= 0 );

		currentBoilerZone = new BoilerZoneLogic(zoneId, plat1, plat2, plat3, plat4, nextZoneId);
	}
}

void BoilerPlanetLogic::onEndElement (const std::string &elem)
{
	if( elem == "zone_logic" )
	{
		boilerZones[currentParsedElementId] = currentBoilerZone;
		currentBoilerZone = NULL;
	}
}

void BoilerPlanetLogic::initZone( int zoneId )
{
	BoilerZones::iterator it = boilerZones.find( zoneId );
	if( it == boilerZones.end() )
	{
		assert( !"[initZone]::Zone Id not existing in BoilerPlanetLogic" );
		return;
	}

	BoilerZoneLogic *boilerZoneLogic = it->second;
	
	bool isTheFirstZone = (zoneId == firstZoneId);
	boilerZoneLogic->initZone( isTheFirstZone );
}

void BoilerPlanetLogic::resetLogic()
{
	BoilerZones::iterator it = boilerZones.begin();
	while(it != boilerZones.end())
	{	
		BoilerZoneLogic *boilerZoneLogic = (it->second);
		boilerZoneLogic->resetZone();
		++it;
	}

	alreadyEnteredBossZone = false;
	fireDeaths = 0;
}

void BoilerPlanetLogic::zoneEntered( int zoneId )
{
	BoilerZones::iterator it = boilerZones.find( zoneId );
	if( it == boilerZones.end() )
	{
		assert( !"[zoneEntered]::Zone id not existing in BoilerPlanetLogic" );
		return;
	}

	BoilerZoneLogic *boilerZoneLogic = it->second;
	boilerZoneLogic->zoneEntered();
}

void BoilerPlanetLogic::miniBoilerKilledAtZone( int zoneId )
{
	BoilerZones::iterator it = boilerZones.find( zoneId );
	if( it == boilerZones.end() )
	{
		assert( !"[miniBoilerKilledAtZone]::Zone id not existing in BoilerPlanetLogic" );
		return;
	}

	BoilerZoneLogic *boilerZoneLogic = it->second;
	boilerZoneLogic->miniBoilerKilled();
}

void BoilerPlanetLogic::openNextZoneTo( int zoneId )
{
	BoilerZones::iterator it = boilerZones.find( zoneId );
	if( it == boilerZones.end() )
	{
		assert( !"[openNextZoneTo]::Zone id not existing in BoilerPlanetLogic" );
		return;
	}

	BoilerZoneLogic *boilerZoneLogic = it->second;
	
	int nextZoneId = boilerZoneLogic->getNextZoneId();
	if( nextZoneId <= numZones )
	{
		it = boilerZones.find( nextZoneId );
		if( it == boilerZones.end() )
		{
			assert( !"[openNextZoneTo]::Next zone id not existing in BoilerPlanetLogic" );
			return;
		}	
		boilerZoneLogic = it->second;
		boilerZoneLogic->openZone();
	}
}

void BoilerPlanetLogic::checkPlatformFiring( int zoneId, int zonePlatformId )
{
	BoilerZones::iterator it = boilerZones.find( zoneId );
	if( it == boilerZones.end() )
	{
		assert( !"[checkPlatformFiring]::Zone id not existing in BoilerPlanetLogic" );
		return;
	}

	BoilerZoneLogic *boilerZoneLogic = it->second;
	boilerZoneLogic->checkFiringPlatform( zonePlatformId );
}

void BoilerPlanetLogic::updatePlayerFiredRetries()
{
	fireDeaths++;
	if( fireDeaths == firedRetries )
	{
		fireDeaths=0;
		LogicManager::getLogicManager()->createUserEvent("boiler_player_death");		
	}
	else
	{
		LogicManager::getLogicManager()->createUserEvent("boiler_player_flicker");
		CheckpointsManager *cpm = CheckpointsManager::getCheckpointsManager();
		cpm->placePlayerLastCheckpoint();
	}
}

void BoilerPlanetLogic::resetFiredDeathsCount()
{
	fireDeaths = 0;
}


void BoilerPlanetLogic::cleanUp()
{
	BoilerZones::iterator it = boilerZones.begin();
	while(it != boilerZones.end())
	{	
		BoilerZoneLogic *boilerZoneLogic = (it->second);
		if(boilerZoneLogic)
			delete boilerZoneLogic, boilerZoneLogic=NULL;
		++it;
	}
	boilerZones.clear();
}

