#ifndef LAVA_BUBBLES_PARTICLES_H
#define LAVA_BUBBLES_PARTICLES_H

#include "Particles/OwnParticles/ParticleEmitter.h"
#include "Util/Timeout/Timeout.h"

class LavaBubblesParticles  : public ParticleEmitter
{ 
public:
	const static std::string type; 
	const static std::string textureName;

public:

	LavaBubblesParticles ();

	~LavaBubblesParticles () {
		OutputDebugString("Destroying LavaBubblesParticles ...\n");
	};

	void generate();

	void update(float deltaTime=(1.0f/60.0f) );

	const std::string & getType() const { return type; };
	const std::string & getTextureName() const { return textureName; };

protected:
	Timeout timeout;

	float radius_a;

protected:
	void createParticles();
};

#endif //LAVA_BUBBLES_PARTICLES_H
