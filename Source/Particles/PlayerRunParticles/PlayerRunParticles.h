#ifndef PLAYER_RUN_PARTICLES_H
#define PLAYER_RUN_PARTICLES_H

#include "Particles/OwnParticles/ParticleEmitter.h"
#include "Util/Timeout/Timeout.h"

class PlayerRunParticles  : public ParticleEmitter
{ 
public:
	const static std::string type; 
	const static std::string textureName;

public:

	PlayerRunParticles ();

	~PlayerRunParticles () {
		OutputDebugString("Destroying PlayerRunParticles ...\n");
	};

	void generate();

	void update(float deltaTime=(1.0f/60.0f) );

	const std::string & getType() const { return type; };
	const std::string & getTextureName() const { return textureName; };


protected:
	Timeout timeout;

protected:
	void createParticles();
};

#endif //PLAYER_RUN_PARTICLES_H
