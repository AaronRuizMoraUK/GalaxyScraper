#ifndef INIT_JUMP_PARTICLES_H
#define INIT_JUMP_PARTICLES_H

#include "Particles/IParticles.h"

class InitJumpParticles : public IParticles
{
public:
	const static std::string type;
	const static std::string textureName;

public:

	InitJumpParticles();

	~InitJumpParticles() {
		OutputDebugString("Destroying InitJumpParticles...\n");
	};

	void generateSource();

	void update(float deltaTime=(1.0f/60.0f) );

	const std::string & getType() const { return type; };
	const std::string & getTextureName() const { return textureName; };
};

#endif //INIT_JUMP_PARTICLES_H
