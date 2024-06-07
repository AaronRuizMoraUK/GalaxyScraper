#ifndef DESINTEGRATE_BALL_PARTICLES_H
#define DESINTEGRATE_BALL_PARTICLES_H

#include "Particles/OwnParticles/ParticleEmitter.h"
#include "Util/Timeout/Timeout.h"

class DesintegrateBallParticles  : public ParticleEmitter
{ 
public:
	const static std::string type; 
	const static std::string textureName;

public:

	DesintegrateBallParticles ();

	~DesintegrateBallParticles () {
		OutputDebugString("Destroying DesintegrateBallParticles ...\n");
	};

	void generate();

	void update(float deltaTime=(1.0f/60.0f) );

	const std::string & getType() const { return type; };
	const std::string & getTextureName() const { return textureName; };
};

#endif //DESINTEGRATE_BALL_PARTICLES_H
