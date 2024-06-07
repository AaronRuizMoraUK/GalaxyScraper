#ifndef KILLED_BOILER_PARTICLES_H
#define KILLED_BOILER_PARTICLES_H

#include "Particles/OwnParticles/ParticleEmitter.h"
#include "Util/Timeout/Timeout.h"


class KilledBoilerParticles : public ParticleEmitter
{ 
public:
	const static std::string type; 
	const static std::string textureName;

public:

	KilledBoilerParticles();

	~KilledBoilerParticles() {
		OutputDebugString("Destroying KilledBoilerParticles...\n");
	};

	void generate();

	void update(float deltaTime=(1.0f/60.0f) );

	const std::string & getType() const { return type; };
	const std::string & getTextureName() const { return textureName; };

protected:
	float radius;
	Timeout timeout;
};

#endif //KILLED_BOILER_PARTICLES_H
