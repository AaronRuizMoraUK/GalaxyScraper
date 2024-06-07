#ifndef BOILER_FIRE_PARTICLES_H
#define BOILER_FIRE_PARTICLES_H

#include "Particles/OwnParticles/ParticleEmitter.h"
#include "Util/Timeout/Timeout.h"


class BoilerFireParticles : public ParticleEmitter
{ 
public:
	const static std::string type; 
	const static std::string textureName;

public:

	BoilerFireParticles();

	~BoilerFireParticles() {
		OutputDebugString("Destroying BoilerFireParticles...\n");
	};

	void generate();

	void update(float deltaTime=(1.0f/60.0f) );

	const std::string & getType() const { return type; };
	const std::string & getTextureName() const { return textureName; };

protected:
	float radius;
	Timeout timeout;
};

#endif //BOILER_FIRE_PARTICLES_H
