#ifndef BOILER_ZONE_LOGIC_H
#define BOILER_ZONE_LOGIC_H

#include <Windows.h>
#include <map>

/**
* Boiler zona logic. This makes the gameplay of
* Boiler zone.
*/

/*
const enum BoilerZoneState
{
	OPENED, CLOSED
};
*/

class BoilerZoneLogic
{
protected:

	int zoneId;
	int platEntryZoneId;
	int platEntryMidId;
	int platExitMidId;
	int platExitZoneId;
	int nextZoneId;
	
//	BoilerZoneState state;

public:

	BoilerZoneLogic(int pZoneId, int pPlatEntryZoneId, 
					int pPlatEntryMidId, int pPlatExitMidId, 
					int pPlatExitZoneId, int pNextZoneId );
	~BoilerZoneLogic() 
	{
		OutputDebugString("Destroying BoilerZoneLogic...\n");
	};

	/*
	void setZoneId( int pZoneId ) { zoneId = pZoneId; };
	void setPlatEntryZoneId( int pPlatEntryZoneId ) { platEntryZoneId = pPlatEntryZoneId; };
	void setPlatEntryMidId( int pPlatEntryMidId ) { platEntryMidId = pPlatEntryMidId; };
	void setPlatExitMidId( int pPlatExitMidId ) { platExitMidId = pPlatExitMidId; };
	void setPlatExitZoneId( int pPlatExitZoneId ) { platExitZoneId = pPlatExitZoneId; };
	*/

	int getNextZoneId() { return nextZoneId; };

	void initZone( bool initAsFirst );
	void resetZone( );
	void openZone();
	void zoneEntered();
	void miniBoilerKilled();
	void checkFiringPlatform( int zonePlatformId );

private:
	int getPlaformId( int zonePlatformId );
};

#endif //BOILER_PLANET_LOGIC_H
