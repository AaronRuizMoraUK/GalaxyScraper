#ifndef BOILER_FIXED_PARTICLES_MANAGER_H
#define BOILER_FIXED_PARTICLES_MANAGER_H

#include <list>
#include "Particles/OwnParticles/ParticleEmitter.h"
#include "Expat/XMLParser.h"

class AnimatedObject;

/**
* Boiler Manager of fixed particles.
*
* It is a singleton.
*/

class BoilerFixedParticlesManager : public std::list<ParticleEmitter *>, public CXMLParser
{	

protected:
	// For reading from XML
	std::string currentType;
	AnimatedObject *currentParent;
	int currentNumQuads;

public:
	~BoilerFixedParticlesManager() {
		OutputDebugString("Destroying BoilerFixedParticlesManager...\n");
		cleanUp();
	};

	void cleanUp();

	static BoilerFixedParticlesManager * getBoilerFixedParticlesManager();

	void update( float deltaTime=(1.0f/60.0f) ); 
	void render() const;

	void reloadShader();

	void onStartElement (const std::string &elem, MKeyValue &atts);
	void onEndElement (const std::string &elem);

private:
	static BoilerFixedParticlesManager * boilerFixedParticlesManager;
	BoilerFixedParticlesManager();

	void addParticle(const std::string &type, AnimatedObject *parent, int pNumQuads);
	BoilerFixedParticlesManager::iterator killParticle(BoilerFixedParticlesManager::iterator &particleIterator);

	void init();
};

#endif //BOILER_FIXED_PARTICLES_MANAGER_H
