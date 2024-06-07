#include "EnemyFoolishedParticles.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Factories/MeshFactory.h"
#include "Global/VertexTypes.h"
#include "Managers/PlanetManager.h"
#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"
#include "AIControllers/ArachnidBehaviour/ArachnidBehaviour.h"

using namespace PAPI;

const std::string EnemyFoolishedParticles::type = "enemyFoolished";
const std::string EnemyFoolishedParticles::textureName = "particle_star.dds";

EnemyFoolishedParticles::EnemyFoolishedParticles():ParticleEmitter(6)
{
}


void EnemyFoolishedParticles::update( float deltaTime )
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
			float offset = 2.0f * D3DX_PI / (float) 6;

			Particle *p = (*it);
			p->update(deltaTime);

			float x, y, z;

			x = 0.5f * cos(2.5f * i * offset * p->getTime() + p->getInitialPhase());
			z = 0.5f * sin(2.5f * i * offset * p->getTime() + p->getInitialPhase());
			y = 1.0f;

			D3DXVECTOR3 pos(x, y, z);

			p->setPosition(pos);

			++it;
		}
		++i;
	}

	numQuads = particles.size();

	if (numQuads != 6 && numQuads != 0)
		assert(!"Aqui falla algo");
}

void EnemyFoolishedParticles::generate()
{
	float offset = 2.0f * D3DX_PI / (float) 6;

	for (int i = 0; i < 6; ++i)
	{
		Particle *p = new Particle();
		float x, y, z;

		x = 0.5f * cos(i * offset);
		z = 0.5f * sin(i * offset);
		y = 1.0f;

		D3DXVECTOR3 pos(x, y, z);

		p->setPosition(pos);

		p->setSize(0.2f);

		p->setInitialPhase(offset * i);
		p->setLifeTime(1.2f);

		particles.push_back(p);
	}

	numQuads = 6;
}
