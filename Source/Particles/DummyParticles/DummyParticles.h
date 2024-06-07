#ifndef DUMMY_PARTICLES_H
#define DUMMY_PARTICLES_H

#include "Particles/IParticles.h"

class DummyParticles : public IParticles
{
public:
	const static std::string type;
	const static std::string textureName;

public:

	DummyParticles();

	~DummyParticles() {
		OutputDebugString("Destroying DummyParticles...\n");
	};

	void generateSource();

	void update(float deltaTime=(1.0f/60.0f) );

	const std::string & getType() const { return type; };
	const std::string & getTextureName() const { return textureName; };
};

#endif //DUMMYPARTICLES_H
