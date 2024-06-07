#ifndef PLAYER_HIT_PARTICLES_H
#define PLAYER_HIT_PARTICLES_H

#include "Particles/OwnParticles/ParticleEmitter.h"
#include "Util/Timeout/Timeout.h"

class PlayerHitParticles  : public ParticleEmitter
{ 
public:
	const static std::string type; 
	const static std::string textureName;

public:

	PlayerHitParticles ();

	~PlayerHitParticles () {
		OutputDebugString("Destroying PlayerHitParticles ...\n");
	};

	void generate();

	void update(float deltaTime=(1.0f/60.0f) );

	const std::string & getType() const { return type; };
	const std::string & getTextureName() const { return textureName; };
};

#endif //PLAYER_HIT_PARTICLES_H
