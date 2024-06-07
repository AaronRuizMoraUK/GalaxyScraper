#ifndef LOGIC_VOLUME_MANAGER_H
#define LOGIC_VOLUME_MANAGER_H

#include <vector>
#include "Logic/LogicVolume/LogicVolume.h"
#include "Expat/XMLParser.h"
#include "Util/MapComparers/MapComparers.h"

/**
* Manager of logic volumes.
*/

class LogicVolumeManager : public std::map<std::string, LogicVolume *, LessString>, CXMLParser
{
protected:
	std::string planetName;

public:
	LogicVolumeManager();
	~LogicVolumeManager() {
		OutputDebugString("Destroying LogicVolumeManager...\n");
		cleanUp();
	};
	void cleanUp();

	void load( const std::string &pPlanetName );

	void onStartElement (const std::string &elem, MKeyValue &atts);
	//void onEndElement (const std::string &elem);

	const LogicVolume *getElement(const std::string &elem) const;

	void update();
};

#endif //LOGIC_VOLUME_MANAGER_H
