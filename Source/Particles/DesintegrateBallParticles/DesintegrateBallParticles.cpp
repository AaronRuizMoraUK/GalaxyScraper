#include "DesintegrateBallParticles.h"
#include "Object/Object.h"
#include "Global/VertexTypes.h"
#include "Global/Definitions.h"

const std::string DesintegrateBallParticles::type = "desintegrate_ball";
const std::string DesintegrateBallParticles::textureName = "particle_fire.dds";

DesintegrateBallParticles::DesintegrateBallParticles():ParticleEmitter(49)
{
}


void DesintegrateBallParticles::update( float deltaTime )
{
	/*
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
	*/
}

void DesintegrateBallParticles::generate()
	{
		int segments = 6;
		float radius = 0.5f;
		float offset = 2.0f * D3DX_PI / segments;

		for (float alpha = 0; alpha < 2.0f * D3DX_PI; alpha+=offset)
		{
			for (float omega = 0; omega < 2.0f * D3DX_PI; omega+=offset)
			{
				float sin_alpha = sin(alpha);
				float sin_omega = sin(omega);
				float cos_alpha = cos(alpha);
				float cos_omega = cos(omega);

				D3DXVECTOR3 pos(radius * cos_omega * cos_alpha, radius * cos_omega * sin_alpha, radius * sin_omega);

				Particle *p = new Particle();

				p->setPosition(pos);
				p->setLifeTime(2.5f);
				p->setSize(0.75f);
				p->setColor(D3DXVECTOR4(1.0f, 1.5f, 1.0f, 0.5f));

				particles.push_back(p);
			}
		}

		numQuads = particles.size();
}
