#ifndef BOILER_PLANET_LOGIC_H
#define BOILER_PLANET_LOGIC_H

#include "Logic/PlanetLogic/IPlanetLogic.h"
#include "Logic/PlanetLogic/BoilerPlanetLogic/BoilerZoneLogic.h"
#include "Expat/XMLParser.h"

/**
* Boiler planet logic. This makes the gameplay of
* Boiler planet.
*
* It is a singleton.
*/

typedef std::map<int, BoilerZoneLogic *> BoilerZones;

class BoilerPlanetLogic : public IPlanetLogic, public CXMLParser
{
public:
	bool alreadyEnteredBossZone;
	
protected:

	BoilerZones boilerZones;
	int numZones;
	int firstZoneId;
	int firedRetries;
	int fireDeaths;
	
private:
	// Only for xml
	int currentParsedElementId;
	BoilerZoneLogic *currentBoilerZone;

public:
	~BoilerPlanetLogic() {
		OutputDebugString("Destroying BoilerPlanetLogic...\n");
		cleanUp();
	};

	static BoilerPlanetLogic * getBoilerPlanetLogic();

	void onStartElement (const std::string &elem, MKeyValue &atts);
	void onEndElement (const std::string &elem);

	int getFiredRetries( ) const { return firedRetries; };
	int getFireDeaths( ) const { return fireDeaths; };
	int getFiredRetriesLeft( ) const;
	
	void init();
	void resetCounters() {};
	void checkVictoryAchieved() {};

	void resetLogic();
	void initZone( int zoneId );
	void zoneEntered( int zoneId );
	void miniBoilerKilledAtZone( int zoneId );
	void openNextZoneTo( int zoneId );
	void checkPlatformFiring( int zoneId, int zonePlatformId );
	void updatePlayerFiredRetries();
	void resetFiredDeathsCount();

private:
	static BoilerPlanetLogic * boilerPlanetLogic;
	BoilerPlanetLogic();

	void cleanUp();
};

#endif //BOILER_PLANET_LOGIC_H
