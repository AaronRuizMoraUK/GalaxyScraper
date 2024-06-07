#ifndef PARTICLE_H
#define PARTICLE_H

#include "Directx/Directx.h"
#include <vector>

class Particle
{
public:

public:

	Particle();

	~Particle() {
		// OutputDebugString("Destroying Particle...\n");
	};


	void setPosition(const D3DXVECTOR3 &pPosition) {position = pPosition;};
	void setSize(float pSize) {size = pSize;};
	void setSpeed(const D3DXVECTOR3 &pSpeed) {speed = pSpeed;};
	void setAcceleration(const D3DXVECTOR3 &pAcceleration) {acceleration = pAcceleration;};
	void update(float deltaTime = 1.0f/60.0f);

	float getSize() const {return size;};
	const D3DXVECTOR3 &getPosition() const {return position;};
	float getTime() const {return time;};
	bool isSentenced() const {return time > lifeTime;};
	void setLifeTime(float pTime) {lifeTime = pTime;};
	const D3DXVECTOR3 &getSpeed() const {return speed;};
	
	void setInitialPhase(float pInitialPhase) {initialPhase = pInitialPhase;};
	float getInitialPhase() const {return initialPhase;};
	void setColor(D3DXVECTOR4 pColor) {color = pColor; };

	const D3DXVECTOR4 &getColor() {return color;};


protected:
	float time;
	float lifeTime;
	bool mustBeKilled;
	D3DXVECTOR3 position;
	float size;
	D3DXVECTOR3	speed;
	D3DXVECTOR3 acceleration;
	float initialPhase;
	D3DXVECTOR4 color;
};

class Particles : public std::vector<Particle *>
{
public:
	// Default constructor
	Particles(){};

	// Destructor
	~Particles() {
		// OutputDebugString("Destroying Particles...\n");
		clear();
	};

	// Override clear function
	void clear() {
		while( !empty() ) {
			Particle * particle = (*this)[size()-1];
			if(particle)
				delete particle, particle=NULL;
			pop_back();
		}
	};

	const D3DXVECTOR3 &getPosition(int index) const
	{
		Particle * particle = (*this)[index];
		return particle->getPosition();
	}

	float getSize(int index) const
	{
		Particle * particle = (*this)[index];
		return particle->getSize();
	}

	Particles::iterator Particles::killParticle(Particles::iterator &particleIterator)
	{
		Particle *particle =(*particleIterator);

		if(particle)
			delete particle, particle=NULL;
		return erase(particleIterator);
	}

	void killSentenced()
	{
		iterator it = begin();
		iterator it_end = end();

		while(it!=it_end) {
			if ((*it)->isSentenced()) {
				it = killParticle(it);
				it_end = end();
			}
		}
	}

	const D3DXVECTOR4 &getColor(int index) const
	{
		Particle * particle = (*this)[index];
		return particle->getColor();
	}

	void setColor(int index, D3DXVECTOR4 color)
	{
		Particle * particle = (*this)[index];
		return particle->setColor(color);
	}
};

#endif //PARTICLE_H
