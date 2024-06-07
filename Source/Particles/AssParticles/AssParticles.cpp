#include "AssParticles.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Factories/MeshFactory.h"
#include "Global/VertexTypes.h"
#include "Managers/PlanetManager.h"
#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"

using namespace PAPI;

const std::string AssParticles::type = "ass";
const std::string AssParticles::textureName = "particle_oloroso.dds";


AssParticles::AssParticles():ParticleEmitter(50), radius(0.0f)
{
}


void AssParticles::update( float deltaTime )
{
	Particles::iterator it = particles.begin();
	Particles::iterator it_end = particles.end();

	int i = 0;
	while(it!=it_end) 
	{
		if ((*it)->isSentenced()) 
		{
			it = particles.killParticle(it);
			it_end = particles.end();
		}
		else
		{

			Particle *p = (*it);
			p->update(deltaTime);

			const D3DXVECTOR3 &parentTop = parent->getLocalUpVector();

			float x, y, z;

			x = parentTop.x * Global::obtainRandom(1.0f, 1.2f);
			y = parentTop.y * Global::obtainRandom(0.5f, 1.8f);
			z = parentTop.z * Global::obtainRandom(1.0f, 1.2f);

			D3DXVECTOR3 offset(x,y,z);
			p->setPosition(p->getPosition() - offset * deltaTime);
			p->setSize(p->getSize()-deltaTime*0.3f);

			timeout.update(deltaTime);

			++it;
		}
		++i;
	}

	if (timeout.isFinished())
	{
		generate();
		timeout.restart();
	}

	numQuads = particles.size();
}

void AssParticles::generate()
{
	if (particles.size() + 5 > 50)
		return;

	for (int i = 0; i < 1; ++i)
	{
		Particle *p = new Particle();
		float x, y, z;

		x = 0.2f * cos(Global::obtainRandom(0.0f, 2.0f * D3DX_PI));
		z = 0.2f * sin(Global::obtainRandom(0.0f, 2.0f * D3DX_PI));
		y = Global::obtainRandom(0.0f, 1.0f);

		D3DXVECTOR3 pos(x, y, z);

		p->setPosition(pos);

		p->setSpeed(D3DXVECTOR3(Global::obtainRandom(-0.5f, 0.5f), Global::obtainRandom(-0.5f, 0.5f), Global::obtainRandom(-0.5f, 0.5f)));

		p->setSize(1.0f);
		// p->setInitialPhase(p->getSize());

		p->setLifeTime(2.5f);

		particles.push_back(p);

		radius = 0.05f;
	}


	numQuads = particles.size();

	timeout.init(0.20f);
}
