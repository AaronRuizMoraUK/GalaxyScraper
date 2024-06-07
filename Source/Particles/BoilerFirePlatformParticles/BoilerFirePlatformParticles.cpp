#include "BoilerFirePlatformParticles.h"
#include "Object/CollisionableObject/AnimatedObject/IntelligentObject/IntelligentObject.h"
#include "AIControllers/FirePlatformBehaviour/FirePlatformBehaviour.h"

using namespace PAPI;

const std::string BoilerFirePlatformParticles::type = "boiler_fire_platform";
const std::string BoilerFirePlatformParticles::textureName = "particle_boca.dds";


BoilerFirePlatformParticles::BoilerFirePlatformParticles()
: ParticleEmitter(50)
, radius(0.0f)
{
	timeout.init(0.33f);
}


void BoilerFirePlatformParticles::update( float deltaTime )
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
			const D3DXVECTOR3 &offset = p->getSpeed();


			float an = p->getTime() * 8.0f;

			y = pos.y + 2.3f;

			x = pos.y * 0.33f * sin(an + Global::obtainRandom(0.0f, 4.0f) * y) + offset.x;
			z = pos.y * 0.33f * sin(an + Global::obtainRandom(0.0f, 4.0f) * y) + offset.z;


			float alpha = 1.0f - y * 0.25f;
			float r = 1.0f;
			float b = 0.0f;
			float g = 0.5f * y;
			p->setColor(D3DXVECTOR4(r, g, b, alpha));

			if (alpha < 0.0f)
				p->update(100.0f);

			y =  pos.y + deltaTime * 4.0f;

			p->setPosition(D3DXVECTOR3(x, y, z));

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

void BoilerFirePlatformParticles::generate()
{
	if (particles.size() + 1 > 25)
		return;

	float width = 1.5f;

	D3DXVECTOR3 pos;

	for (int j = 0; j < 1; ++j)
	{
		for (int i = 0; i < 1; ++i)
		{
			Particle *p = new Particle();
			D3DXVECTOR3 offset;

			switch (j)
			{
			case 0:
				offset = D3DXVECTOR3(0, 0, 0);
				break;
			case 1:
				offset = D3DXVECTOR3(0, 0, width);
				break;
			case 2:
				offset = D3DXVECTOR3(width, 0, 0);
				break;
			case 3:
				offset = D3DXVECTOR3(0, 0, -width);
				break;
			case 4:
				offset = D3DXVECTOR3(-width, 0, 0);
				break;
			}

			float x, y, z;

			y = Global::obtainRandom(0.0f, 2.0f);
			x = Global::obtainRandom(-y * 0.33f, y * 0.33f);
			z = Global::obtainRandom(-y * 0.33f, y * 0.33f);

			float alpha;

			alpha = 1.0f - y * 0.43f;
			float r = 1.0f;
			float b = 0.0f;
			float g = 0.5f * y;
			// p->setSpeed(D3DXVECTOR3(Global::obtainRandom(-0.5f, 0.5f), Global::obtainRandom(-0.5f, 0.5f), Global::obtainRandom(-0.5f, 0.5f)));
			// 		p->setSpeed(D3DXVECTOR3(Global::obtainRandom(-0.2f, 0.2f), 0.1f, Global::obtainRandom(-0.2f, 0.2f)));
			p->setColor(D3DXVECTOR4(r, g, b, alpha));
			//		p->setSize(1.0f);
			p->setSpeed(offset);

			y -= 2.3f;

			p->setPosition(D3DXVECTOR3(x, y, z) + offset);
			p->setLifeTime(2.0f);
			particles.push_back(p);
			radius = 0.05f;
		}

	}
	numQuads = particles.size();

	timeout.restart();
}
