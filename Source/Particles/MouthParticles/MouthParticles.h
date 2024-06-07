#ifndef MOUTH_PARTICLES_H
#define MOUTH_PARTICLES_H

#include "Particles/OwnParticles/ParticleEmitter.h"

class MouthParticles : public ParticleEmitter
{ 
public:
	const static std::string type; 
	const static std::string textureName;

public:

	MouthParticles();

	~MouthParticles() {
		OutputDebugString("Destroying MouthParticles...\n");
	};

	void generate();

	void update(float deltaTime=(1.0f/60.0f) );

	const std::string & getType() const { return type; };
	const std::string & getTextureName() const { return textureName; };

protected:
	float radius;
};

#endif //MOUTH_PARTICLES_H
