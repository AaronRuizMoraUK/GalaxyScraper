#ifndef PLAYER_BURNED_PARTICLES_H
#define PLAYER_BURNED_PARTICLES_H

#include "Particles/OwnParticles/ParticleEmitter.h"
#include "Util/Timeout/Timeout.h"

class PlayerBurnedParticles  : public ParticleEmitter
{ 
public:
	const static std::string type; 
	const static std::string textureName;

public:

	PlayerBurnedParticles ();

	~PlayerBurnedParticles () {
		OutputDebugString("Destroying PlayerBurnedParticles ...\n");
	};

	void generate();

	void update(float deltaTime=(1.0f/60.0f) );

	const std::string & getType() const { return type; };
	const std::string & getTextureName() const { return textureName; };


protected:
	Timeout timeout;
	Timeout timeoutFinish;

protected:
	void createParticles();
};

#endif //PLAYER_BURNED_PARTICLES_H
