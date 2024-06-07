#include "BoilerZoneLogic.h"
#include "Managers/BoilerManagers/FirePlatformsManager.h"
#include "Managers/LogicManager/LogicManager.h"

BoilerZoneLogic::BoilerZoneLogic(int pZoneId, int pPlatEntryZoneId, int pPlatEntryMidId, int pPlatExitMidId, int pPlatExitZoneId, int pNextZoneId )
: zoneId(pZoneId)
, platEntryZoneId(pPlatEntryZoneId)
, platEntryMidId(pPlatEntryMidId)
, platExitMidId(pPlatExitMidId)
, platExitZoneId(pPlatExitZoneId)
, nextZoneId(pNextZoneId)
//, state(CLOSED)
{
}

void BoilerZoneLogic::initZone( bool initAsFirst )
{
	FirePlatformsManager *fpm = FirePlatformsManager::getFirePlatformsManager();

	fpm->startSequence( platExitMidId );
	fpm->startSequence( platExitZoneId );

	if( !initAsFirst )
	{
		// if it's not the first zone, block everything
		fpm->startSequence( platEntryZoneId );
		fpm->startSequence( platEntryMidId );
	}
}

void BoilerZoneLogic::resetZone( )
{
	bool platformFiring = false;
	FirePlatformsManager *fpm = FirePlatformsManager::getFirePlatformsManager();

	platformFiring = fpm->isPlatformFiring( platEntryZoneId );
	if( platformFiring )
		fpm->stopSequence( platEntryZoneId );
	
	platformFiring = fpm->isPlatformFiring( platEntryMidId );
	if( platformFiring )
		fpm->stopSequence( platEntryMidId );

	platformFiring = fpm->isPlatformFiring( platExitMidId );
	if( platformFiring )
		fpm->stopSequence( platExitMidId );

	platformFiring = fpm->isPlatformFiring( platExitZoneId );
	if( platformFiring )
		fpm->stopSequence( platExitZoneId );
}

void BoilerZoneLogic::openZone()
{
	FirePlatformsManager *fpm = FirePlatformsManager::getFirePlatformsManager();

	fpm->stopSequence( platEntryZoneId );
	fpm->stopSequence( platEntryMidId );
}

void BoilerZoneLogic::zoneEntered()
{
	bool platformFiring = false;
	FirePlatformsManager *fpm = FirePlatformsManager::getFirePlatformsManager();

	platformFiring = fpm->isPlatformFiring( platEntryZoneId );
	if( !platformFiring )
		fpm->startSequence( platEntryZoneId );
	
	platformFiring = fpm->isPlatformFiring( platEntryMidId );
	if( !platformFiring )
		fpm->startSequence( platEntryMidId );
}

void BoilerZoneLogic::miniBoilerKilled()
{
	FirePlatformsManager *fpm = FirePlatformsManager::getFirePlatformsManager();

	fpm->stopSequence( platExitMidId );
	fpm->stopSequence( platExitZoneId );
}

void BoilerZoneLogic::checkFiringPlatform( int zonePlatformId )
{
	bool platformFiring = false;

	FirePlatformsManager *fpm = FirePlatformsManager::getFirePlatformsManager();
	int platformId = getPlaformId( zonePlatformId );
	platformFiring = fpm->isPlatformFiring( platformId );
	if( platformFiring )
	{
		LogicManager::getLogicManager()->createUserEvent("on_player_fired");
	}
}

int BoilerZoneLogic::getPlaformId( int zonePlatformId )
{
	switch( zonePlatformId )
	{
		case 1:
			return platEntryZoneId;
			break;
		case 2:
			return platEntryMidId;
			break;
		case 3:
			return platExitMidId;
			break;
		case 4:
			return platExitZoneId;
			break;
		default:
			assert( !"This should not happend" );
			return -1;
			break;
	}
}