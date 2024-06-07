#ifndef IURANUS_FIXED_PARTICLES_MANAGER_H
#define IURANUS_FIXED_PARTICLES_MANAGER_H

#include <list>
#include "Expat/XMLParser.h"
#include "Particles/OwnParticles/ParticleEmitter.h"

class AnimatedObject;

/**
* Iuranus Manager of fixed particles.
*
* It is a singleton.
*/

class IuranusFixedParticlesManager : public std::list<ParticleEmitter *>, public CXMLParser
{	

protected:
	// For reading from XML
	std::string currentType;
	AnimatedObject *currentParent;
	int currentNumQuads;

public:
	~IuranusFixedParticlesManager() {
		OutputDebugString("Destroying IuranusFixedParticlesManager...\n");
		cleanUp();
	};

	void cleanUp();

	static IuranusFixedParticlesManager * getIuranusFixedParticlesManager();

	void update( float deltaTime=(1.0f/60.0f) ); 
	void render() const;

	void reloadShader();

	void onStartElement (const std::string &elem, MKeyValue &atts);
	void onEndElement (const std::string &elem);

private:
	static IuranusFixedParticlesManager * iuranusFixedParticlesManager;
	IuranusFixedParticlesManager();

	void addParticle(const std::string &type, AnimatedObject *parent, int pNumQuads);
	IuranusFixedParticlesManager::iterator killParticle(IuranusFixedParticlesManager::iterator &particleIterator);

	void init();
};

#endif //IURANUS_FIXED_PARTICLES_MANAGER_H
