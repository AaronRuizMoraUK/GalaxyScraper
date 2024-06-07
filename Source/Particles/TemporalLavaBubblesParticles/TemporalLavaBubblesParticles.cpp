#include "TemporalLavaBubblesParticles.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"

const std::string TemporalLavaBubblesParticles::type = "temporal_lava_bubbles";
const std::string TemporalLavaBubblesParticles::textureName = "particle_lava.dds";

TemporalLavaBubblesParticles::TemporalLavaBubblesParticles()
: ParticleEmitter(50)
, radius_a(0.3f)
{
	timeoutFinish.init(1.5f);
}


void TemporalLavaBubblesParticles::update( float deltaTime )
{
	Particles::iterator it = particles.begin();
	Particles::iterator it_end = particles.end();

	int i = 0;

	timeoutFinish.update(deltaTime);

	while(it!=it_end) 
	{

		if (timeoutFinish.isFinished() || (*it)->isSentenced()) 
		{
			it = particles.killParticle(it);
			it_end = particles.end();
		}
		else
		{
			Particle *p = (*it);
			p->update(deltaTime);

			float x, z, y;

			x = radius_a * cos(p->getInitialPhase());
			z = radius_a * sin(p->getInitialPhase());
			y = 1.5f + p->getTime() * 0.2f;

			if( y > 1.5 + 0.73f ) 
				radius_a += deltaTime;

			D3DXVECTOR3 pos(x, y, z);

			p->setPosition(pos);
			p->setSize(p->getSize() - deltaTime);

			if (p->getSize() < 0.0f)
				p->setSize(0.0f);

			++it;

		}
		++i;
	}

	timeout.update(deltaTime);

	if (timeout.isFinished())
	{
		createParticles();
		timeout.init(Global::obtainRandom(0.2f,0.5f));
		timeout.restart();
	}


	numQuads = particles.size();
}

void TemporalLavaBubblesParticles::createParticles()
{

	if (particles.size() + 8 > 9)
		return;

	float offset = 2.0f * D3DX_PI / 8.0f;

	for (int i = 0; i < 8; ++i)
	{
		Particle *p = new Particle();
		float x, y, z;

		x = radius_a * cos(i * offset);
		z = radius_a * sin(i * offset);
		y = 1.5f;

		D3DXVECTOR3 pos(x, y, z);

		p->setPosition(pos);

	//	p->setSize(0.5f);
		p->setSize(Global::obtainRandom(0.3f, 1.0f));
		// p->setInitialPhase(p->getSize());

		p->setInitialPhase(offset * i);
		p->setLifeTime(Global::obtainRandom(0.7f, 1.3f));

		particles.push_back(p);

		// radius_a = 0.3f;
	}

	numQuads = particles.size();
}
void TemporalLavaBubblesParticles::generate()
{

	createParticles();
	timeout.init(Global::obtainRandom(0.2f,0.5f));
}
