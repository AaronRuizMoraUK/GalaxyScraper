#ifndef PARTICLES_MANAGER_H
#define PARTICLES_MANAGER_H

#include <list>
#include "Particles/OwnParticles/ParticleEmitter.h"
#include "Util/MapComparers/MapComparers.h"


class Object;

/**
* Manager of particles.
*
* It is a singleton.
*/

class ParticlesManager : public std::list<ParticleEmitter *>
{	
public:
	~ParticlesManager() {
		OutputDebugString("Destroying ParticleManager...\n");
		cleanUp();
	};

	void cleanUp();

	static ParticlesManager * getParticlesManager();

	void update( float deltaTime=(1.0f/60.0f) ); 
	void render() const;

	void addParticle(const std::string &type, Object *parent, bool createPivot, bool pIsLocal = true);

	Object *createPivotFromObject(Object *pParent);

	void reloadShader();

	void removeParticles(Object *enemy);

private:
	static ParticlesManager * particlesManager;

	ParticlesManager::iterator killParticle(ParticlesManager::iterator &particleIterator);

	ParticlesManager() {};
};

#endif //PARTICLES_MANAGER_H