#ifndef PLANET_MANAGER_H
#define PLANET_MANAGER_H

#include <map>
#include "Object/Planet/Planet.h"
#include "Util/MapComparers/MapComparers.h"
#include "Expat/XMLParser.h"

/**
* Manager of planets.
*
* It is a singleton.
*/

class PlanetManager : public std::map<std::string, Planet *, LessString>, public CXMLParser
{
protected:
	Planet *currentParsed;
	std::string currentParsedPlanetName;

	std::string currentPlanetName;
	Planet *currentPlanet;

public:
	~PlanetManager() {
		OutputDebugString("Destroying PlanetManager...\n");
		cleanUp();
	};
	void cleanUp();

	static PlanetManager * getPlanetManager();

//	void read(const char *fileName);
	void render() const;

	void onStartElement (const std::string &elem, MKeyValue &atts);
	void onEndElement (const std::string &elem);

	const std::string & getCurrentPlanetName() const {return currentPlanetName;};
	const Planet * getCurrentPlanet() const {return currentPlanet;};

	const Planet * getPlanet( const std::string &name) const;

	void setCurrentPlanet( const std::string &name ) {
		const_iterator it = find( name );
		if( it == end() ) {
			assert( !"Planet does not exist in PlanetManager" );
			return;
		}

		// Change the shader technique of old current planet to basic_tech
		currentPlanet->setTechnique( "basic_tech" );

		currentPlanetName = it->first;
		currentPlanet = it->second;

		// Change the shader technique of new current planet to normal_tech
		currentPlanet->setTechnique( "normal_tech" );
	};

	void reloadShader();

private:
	void init();
	void createElement(const std::string &elem, MKeyValue &atts);
	static PlanetManager * planetManager;
	PlanetManager();
	/*
	: currentPlanetName(""), currentPlanet(NULL) {
		read("./data/planet_instances.txt");
	};
	*/

	//IMesh * readMesh(const char *filename);
};

#endif //PLANET_MANAGER_H
