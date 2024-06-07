#ifndef FART_PARTICLES_H
#define FART_PARTICLES_H

#include "Particles/IParticles.h"

class FartParticles : public IParticles
{

protected: 
	PAPI::pVec center;
	PAPI::pVec actualSize;

public:
	const static std::string type;
	const static std::string textureName;

public:

	FartParticles();

	~FartParticles() {
		OutputDebugString("Destroying FartParticles...\n");
	};

	void generateSource();

	void update(float deltaTime=(1.0f/60.0f) );

	const std::string & getType() const { return type; };
	const std::string & getTextureName() const { return textureName; };
};

#endif //FART_PARTICLES_H
