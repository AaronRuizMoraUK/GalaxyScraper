#include "FirePlatformBaseParticles.h"
#include "Object/CollisionableObject/AnimatedObject/IntelligentObject/IntelligentObject.h"
#include "AIControllers/FirePlatformBehaviour/FirePlatformBehaviour.h"

using namespace PAPI;

const std::string FirePlatformBaseParticles::type = "fire_platform_base";
const std::string FirePlatformBaseParticles::textureName = "particle_boca.dds";


FirePlatformBaseParticles::FirePlatformBaseParticles()
: ParticleEmitter(50)
, radius(0.0f)
{
}


void FirePlatformBaseParticles::update( float deltaTime )
{
	Particles::iterator it = particles.begin();
	Particles::iterator it_end = particles.end();

	timeout.update(deltaTime);

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
			
			p->setColor(p->getColor() + D3DXVECTOR4(0.0f, deltaTime * 3.0f, 0.0f, 0.0f));
			p->setSize(p->getSize() - deltaTime);

			if (p->getSize() < 0.0f)
				p->setSize(0.0f);
			++it;

		}
		++i;
	}

	if (timeout.isFinished())
	{
		generate();
		timeout.restart();
	}

	IntelligentObject *platform = (IntelligentObject *) parent;
	FirePlatformBehaviour *fpb = (FirePlatformBehaviour *) platform->getAIController();
	int currentAIState = fpb->getCurrentAIState();

	if ( currentAIState == STOPED )
	{
		it = particles.begin();
		it_end = particles.end();


		while(it!=it_end) 
		{

			(*it)->update(1000.0f);
			++it;
		}
	}

	numQuads = particles.size();
}

void FirePlatformBaseParticles::createParticles()
{
	if (particles.size() + 10 > 50)
		return ;
		

	float offset = 2.0f * D3DX_PI / 10.0f;
	radius = 1.5f;

	for (int i = 0; i < 10; ++i)
	{
		Particle *p = new Particle();
		float x, y, z;

		radius = Global::obtainRandom(1.0f, 1.5f);

		x = radius * cos(i * offset);
		z = radius * sin(i * offset);
		y = -2.3f;

		D3DXVECTOR3 pos(x, y, z);

		p->setPosition(pos);
		p->setColor(D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

		p->setSize(Global::obtainRandom(0.3f, 1.0f));
		p->setInitialPhase(offset * i);
		p->setLifeTime(Global::obtainRandom(0.7f, 1.3f));

		particles.push_back(p);
	}

	numQuads = particles.size();
}

void FirePlatformBaseParticles::generate()
{

	createParticles();
	timeout.init(Global::obtainRandom(0.2f,0.5f));
}
