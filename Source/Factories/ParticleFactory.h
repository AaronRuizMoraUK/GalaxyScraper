#ifndef PARTICLE_FACTORY_H
#define PARTICLE_FACTORY_H

#include <map>
#include "Mesh/IMesh.h"

typedef std::map<int, IMesh *> ParticleMeshMap;

/**
* Factory of particle mesh.
*
* This is a singleton.
*/

class ParticleFactory
{
protected:
	ParticleMeshMap particleMeshMap;

public:
	~ParticleFactory() {
		OutputDebugString("Destroying ParticleFactory...\n");

		// Destroy particle mesh map
		ParticleMeshMap::const_iterator it = particleMeshMap.begin();
		while(it !=  particleMeshMap.end()) {	
			IMesh *mesh = (it->second);
			if(mesh)
				delete mesh, mesh=NULL;
			++it;
		}
		particleMeshMap.clear();
	};
	
	static ParticleFactory * getParticleFactory();

	IMesh * createParticle(int numParticles);

private:
	static ParticleFactory * particleFactory;
	ParticleFactory() {};

	IMesh * createParticleMesh(int numParticles);
};

#endif //PARTICLE_FACTORY_H
