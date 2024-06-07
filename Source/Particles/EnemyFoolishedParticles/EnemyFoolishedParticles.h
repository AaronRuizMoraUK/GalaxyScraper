#ifndef ENEMY_FOOLISHED_PARTICLES_H
#define ENEMY_FOOLISHED_PARTICLES_H

#include "Particles/OwnParticles/ParticleEmitter.h"

class EnemyFoolishedParticles : public ParticleEmitter
{ 
public:
	const static std::string type; 
	const static std::string textureName;

public:

	EnemyFoolishedParticles();

	~EnemyFoolishedParticles() {
		OutputDebugString("Destroying FinishJumpParticles...\n");
	};

	void generate();

	void update(float deltaTime=(1.0f/60.0f) );

	const std::string & getType() const { return type; };
	const std::string & getTextureName() const { return textureName; };
};

#endif //ENEMY_FOOLISHED_PARTICLES_H
