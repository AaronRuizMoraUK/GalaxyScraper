#include "EnemyHitParticles.h"

const std::string EnemyHitParticles::type = "enemyHit";
const std::string EnemyHitParticles::textureName = "particle_enemy_hit.dds";


EnemyHitParticles::EnemyHitParticles():ParticleEmitter(1)
{
}


void EnemyHitParticles::update( float deltaTime )
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

			++it;
		}
		++i;
	}

	numQuads = particles.size();
}

void EnemyHitParticles::generate()
	{

	Particle *p = new Particle();
	p->setSize(1.0f);
	p->setLifeTime(0.15f);
	p->setPosition(D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	particles.push_back(p);
}
