#include "MouthParticles.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Factories/MeshFactory.h"
#include "Global/VertexTypes.h"
#include "Managers/PlanetManager.h"
#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"

using namespace PAPI;

const std::string MouthParticles::type = "mouth";
const std::string MouthParticles::textureName = "particle_boca.dds";


MouthParticles::MouthParticles():ParticleEmitter(40), radius(0.0f)
{
}


void MouthParticles::update( float deltaTime )
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

			float x, y, z;
			

			x = p->getSpeed().x * sin(p->getTime() * 2.0f);
			y = -5.0f + p->getSpeed().y * sin(p->getTime() * 2.0f);
			z = p->getSpeed().z * sin(p->getTime() * 2.0f);

			D3DXVECTOR3 offset(x, y, z);
			p->setPosition(offset);


			/*

			float offset = 2.0f * D3DX_PI / 10.0f;

			float x, y, z;

			x = radius * cos(p->getInitialPhase());
			z = radius * sin(p->getInitialPhase());
			y = 0.0f;

			D3DXVECTOR3 pos(x, 0.2f, z);

			p->setPosition(pos);
			p->setSize(p->getSize() + 0.015f * sin(p->getTime() * 8.0f));

			*/

			++it;
		}
		++i;
	}

	numQuads = particles.size();
}

void MouthParticles::generate()
{
	float offset = 2.0f * D3DX_PI / (float) 10.0f;

	for (int j = 1; j < 5; ++j)
	{
		for (int i = 0; i < 10; ++i)
		{
			Particle *p = new Particle();
			float x, y, z;

			x = j * 0.2f * cos(i * offset);
			z = j * 0.3f * sin(i * offset);
			y = 0.0f;

			D3DXVECTOR3 pos(x, y, z);

			p->setPosition(pos);

			p->setSpeed(D3DXVECTOR3(Global::obtainRandom(-0.5f, 0.5f), Global::obtainRandom(-0.5f, 0.5f), Global::obtainRandom(-0.5f, 0.5f)));

			p->setSize(0.5f);
			// p->setInitialPhase(p->getSize());

			p->setInitialPhase(offset * i);
			p->setLifeTime(70.7f);

			particles.push_back(p);

			radius = 0.2f * j;
		}
	}

	numQuads = particles.size();
}
