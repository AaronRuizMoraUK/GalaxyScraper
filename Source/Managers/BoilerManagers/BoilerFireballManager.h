#ifndef BOILER_FIREBALL_MANAGER_H
#define BOILER_FIREBALL_MANAGER_H

#include <map>
#include "Object/CollisionableObject/AnimatedObject/IntelligentObject/IntelligentObject.h"
#include "XMLParser/XMLParser.h"

/**
* Manager of Iuranus animated objects.
*
* It is a singleton.
*/

typedef std::vector<IntelligentObject *> FireballVector;

class BoilerFireballManager : public std::map<std::string, IntelligentObject *>, public CXMLParser
{
protected:
	IntelligentObject *currentParsed;
	std::string currentParsedElementName;
	FireballVector collisionableFireballs;



public:
	~BoilerFireballManager() {
		OutputDebugString("Destroying BoilerFireballManager...\n");
		cleanUp();
	};
	void cleanUp();

	static BoilerFireballManager * getBoilerFireballManager();

//	void read(const char *fileName);
	void render() const;
	void updateAnimation(float deltaTime=(1.0f/60.0f));
	void updateAI(float deltaTime=(1.0f/60.0f));

	void onStartElement (const std::string &elem, MKeyValue &atts);
	void onEndElement (const std::string &elem);

	const FireballVector &getCollisionableBalls() const {return collisionableFireballs;};

	IntelligentObject * getIntelligentObject( const std::string &name ) const;

	void reloadShader();

	void shootBall(const std::string &name);

	void renewParticles();

private:
	void init();
	void createElement(const std::string &elem, MKeyValue &atts);

	static BoilerFireballManager * boilerFireballManager;
	BoilerFireballManager();
};

#endif //BOILER_FIREBALL_MANAGER_H
