#ifndef FINISH_JUMP_PARTICLES_H
#define FINISH_JUMP_PARTICLES_H

#include "Particles/OwnParticles/ParticleEmitter.h"

class FinishJumpParticles : public ParticleEmitter
{ 
public:
	const static std::string type; 
	const static std::string textureName;

public:

	FinishJumpParticles();

	~FinishJumpParticles() {
		OutputDebugString("Destroying FinishJumpParticles...\n");
	};

	void generate();

	void update(float deltaTime=(1.0f/60.0f) );

	const std::string & getType() const { return type; };
	const std::string & getTextureName() const { return textureName; };

protected:
	float radius_a;
};

#endif //FINISH_JUMP_PARTICLES_H
