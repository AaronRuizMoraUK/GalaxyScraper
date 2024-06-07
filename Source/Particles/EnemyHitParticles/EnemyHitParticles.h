#ifndef ENEMY_HIT_PARTICLES
#define ENEMY_HIT_PARTICLES

#include "Particles/OwnParticles/ParticleEmitter.h"

class EnemyHitParticles  : public ParticleEmitter
{ 
public:
	const static std::string type; 
	const static std::string textureName;

	const static float RADIUS;
	const static int NUM_PARTICLES;
	const static float PARTICLE_SIZE;
	const static float SPEED;
	const static float PARTICLE_DURATION;

public:

	EnemyHitParticles ();

	~EnemyHitParticles () {
		OutputDebugString("Destroying EnemyHitParticles ...\n");
	};

	void generate();

	void update(float deltaTime=(1.0f/60.0f) );

	const std::string & getType() const { return type; };
	const std::string & getTextureName() const { return textureName; };
};

#endif //ENEMY_HIT_PARTICLES
