#ifndef PLANTS_HIT_PARTICLES_H
#define PLANTS_HIT_PARTICLES_H

#include "Particles/IParticles.h"

class PlantsHitParticles : public IParticles
{
public:
	const static std::string type;
	const static std::string textureName;

public:

	PlantsHitParticles();

	~PlantsHitParticles() {
		OutputDebugString("Destroying PlantsHitParticles...\n");
	};

	void generateSource();

	void update(float deltaTime=(1.0f/60.0f) );

	const std::string & getType() const { return type; };
	const std::string & getTextureName() const { return textureName; };
};

#endif //PLANTS_HIT_PARTICLES_H
