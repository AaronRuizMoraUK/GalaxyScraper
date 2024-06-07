#include "PlayerHitParticles.h"

const std::string PlayerHitParticles::type = "playerHit";
const std::string PlayerHitParticles::textureName = "particle_player_hit.dds";

PlayerHitParticles::PlayerHitParticles():ParticleEmitter(1)
{
}


void PlayerHitParticles::update( float deltaTime )
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

void PlayerHitParticles::generate()
	{

	Particle *p = new Particle();
	p->setSize(1.0f);
	p->setLifeTime(0.15f);
	p->setPosition(D3DXVECTOR3(0.0f, 1.5f, 0.0f));
	particles.push_back(p);
}
