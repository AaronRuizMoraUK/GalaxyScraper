#include "FinishJumpParticles.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Factories/MeshFactory.h"
#include "Global/VertexTypes.h"
#include "Managers/PlanetManager.h"
#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"

using namespace PAPI;

const std::string FinishJumpParticles::type = "finishJump";
const std::string FinishJumpParticles::textureName = "particle_humillo2.dds";


FinishJumpParticles::FinishJumpParticles():ParticleEmitter(10), radius_a(0.3f)
{
}


void FinishJumpParticles::update( float deltaTime )
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

			float x, z;

			x = radius_a * cos(p->getInitialPhase());
			z = radius_a * sin(p->getInitialPhase());
			
			D3DXVECTOR3 pos(x, 0.2f, z);

			p->setPosition(pos);
			p->setSize(p->getSize() - deltaTime);

			if (p->getSize() < 0.0f)
				p->setSize(0.0f);

			++it;
		}
		++i;
	}
	
	radius_a+=deltaTime;

	numQuads = particles.size();
}

void FinishJumpParticles::generate()
{
	float offset = 2.0f * D3DX_PI / 10.0f;

	for (int i = 0; i < 10; ++i)
	{
		Particle *p = new Particle();
		float x, y, z;

		x = radius_a * cos(i * offset);
		z = radius_a * sin(i * offset);
		y = 0.0f;

		D3DXVECTOR3 pos(x, y, z);

		p->setPosition(pos);

	//	p->setSize(0.5f);
		p->setSize(Global::obtainRandom(0.3f, 1.0f));
		// p->setInitialPhase(p->getSize());

		p->setInitialPhase(offset * i);
		p->setLifeTime(0.7f);

		particles.push_back(p);

		// radius_a = 0.3f;
	}

	numQuads = 10;
}
