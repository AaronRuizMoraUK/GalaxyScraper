#ifndef PLANET_ATMOSPHERE_MANAGER_H
#define PLANET_ATMOSPHERE_MANAGER_H

#include <map>
#include "Object/Object.h"
#include "Mesh/IMesh.h"
#include "Util/MapComparers/MapComparers.h"
#include "XMLParser/XMLParser.h"

//#define ATMPH_QUAD_SCALE 1.25f
//#define ATMPH_QUAD_VERTEX_PER_AXIS 3

/**
* Manager of planet atmospheres.
*
* It is a singleton.
*/

class PlanetAtmosphereManager : public std::map<std::string, Object *, LessString>, public CXMLParser
{
protected:
	Object *currentParsed;
	std::string currentParsedPlanetName;

	float atmosphereQuadScale;
	int atmosphereQuadVertexPerAxis;

public:
	~PlanetAtmosphereManager() {
		OutputDebugString("Destroying PlanetAtmosphereManager...\n");
		cleanUp();
	};
	void cleanUp();

	static PlanetAtmosphereManager * getPlanetAtmosphereManager();

	void onStartElement (const std::string &elem, MKeyValue &atts);
	void onEndElement (const std::string &elem);

	void update();

//	void read(const char *fileName);
	void render() const;

	void reloadShader();

private:
	void init();
	void assignMesh();
	void createElement(const std::string &elem, MKeyValue &atts);
	
	static PlanetAtmosphereManager * planetAtmosphereManager;
	PlanetAtmosphereManager();
	/*
	{
		read("./data/planet_atmosphere_instances.txt");
	};
	*/

	void updateTransformMatrix(const std::string &name, Object *object);

};

#endif //PLANET_ATMOSPHERE_MANAGER_H