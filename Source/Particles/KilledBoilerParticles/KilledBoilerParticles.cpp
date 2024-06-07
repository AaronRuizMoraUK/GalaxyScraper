#include "KilledBoilerParticles.h"
#include "Object/CollisionableObject/AnimatedObject/IntelligentObject/IntelligentObject.h"
#include "AIControllers/FirePlatformBehaviour/FirePlatformBehaviour.h"

using namespace PAPI;

const std::string KilledBoilerParticles::type = "killed_boiler";
const std::string KilledBoilerParticles::textureName = "particle_smoke_boiler.dds";


KilledBoilerParticles::KilledBoilerParticles()
: ParticleEmitter(50)
, radius(0.0f)
{
	timeout.init(0.33f);
}


void KilledBoilerParticles::update( float deltaTime )
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

			const D3DXVECTOR3 &pos = p->getPosition();

			float an = p->getTime() * 8.0f;

			y =  pos.y + deltaTime * 4.0f;
			x = pos.y * 0.13f * sin(an + Global::obtainRandom(0.0f, 4.0f) * y);
			z = pos.y * 0.13f * sin(an + Global::obtainRandom(0.0f, 4.0f) * y);
			p->setPosition(D3DXVECTOR3(x, y, z));
			
			float alpha = 1.0f - y * 0.25f;
			float r = 0.5f * y;
			float b = 0.5f * y;
			float g = 0.5f * y;
			p->setColor(D3DXVECTOR4(r, g, b, alpha));

			if (alpha < 0.0f)
				p->update(100.0f);

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

	IntelligentObject *platform = (IntelligentObject *) parent;
	FirePlatformBehaviour *fpb = (FirePlatformBehaviour *) platform->getAIController();
	int currentAIState = fpb->getCurrentAIState();

	if ( currentAIState == STOPED )
	{
		timeout.init(100.0f);
	}

	numQuads = particles.size();
}

void KilledBoilerParticles::generate()
{
	if (particles.size() + 5 > 50)
		return;

	float width = 0.5f;

	D3DXVECTOR3 pos;

	for (int i = 0; i < 5; ++i)
	{
		Particle *p = new Particle();

		float x, y, z;

		y = Global::obtainRandom(0.0f, 2.0f);
		x = Global::obtainRandom(-y * 0.25f, y * 0.25f);
		z = Global::obtainRandom(-y * 0.25f, y * 0.25f);

		p->setPosition(D3DXVECTOR3(x, y , z));

		float alpha;

		alpha = 1.0f - y * 0.43f;
		float r = 1.0f;
		float b = 0.0f;
		float g = 0.5f * y;
		// p->setSpeed(D3DXVECTOR3(Global::obtainRandom(-0.5f, 0.5f), Global::obtainRandom(-0.5f, 0.5f), Global::obtainRandom(-0.5f, 0.5f)));
		// 		p->setSpeed(D3DXVECTOR3(Global::obtainRandom(-0.2f, 0.2f), 0.1f, Global::obtainRandom(-0.2f, 0.2f)));
		p->setColor(D3DXVECTOR4(r, g, b, alpha));
		//		p->setSize(1.0f);
		p->setInitialPhase(Global::obtainRandom(0.0f, 4.0f));

		p->setLifeTime(2.5f);
		particles.push_back(p);
		radius = 0.05f;
	}


	numQuads = particles.size();

	timeout.restart();


}
