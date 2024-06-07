#include "PlayerBurnedParticles.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"

const std::string PlayerBurnedParticles::type = "player_burned";
const std::string PlayerBurnedParticles::textureName = "particle_humillo.dds";

PlayerBurnedParticles::PlayerBurnedParticles():ParticleEmitter(50)
{
}


void PlayerBurnedParticles::update( float deltaTime )
{
	Particles::iterator it = particles.begin();
	Particles::iterator it_end = particles.end();

	int i = 0;

	Player *player = Player::getPlayer();

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

			p->setColor(p->getColor() + D3DXVECTOR4(deltaTime*3.0f, deltaTime*3.0f, deltaTime*3.0f, -deltaTime));

			p->setPosition(p->getPosition() + D3DXVECTOR3(p->getInitialPhase() * deltaTime, 0, -deltaTime*2.0f));

			++it;
		}
		++i;
	}

	timeout.update(deltaTime);
	timeoutFinish.update(deltaTime);


	if (timeout.isFinished())
	{
		createParticles();
		timeout.restart();
	}

	if (timeoutFinish.isFinished())
	{
		it = particles.begin();
		it_end = particles.end();

		while(it!=it_end) 
		{
			(*it)->update(100.0f);
			++it;
		}
	}


	numQuads = particles.size();
}

void PlayerBurnedParticles::createParticles()
{

	if ((particles.size() + 5) > 50)
		return;

	for (int i = 0; i < 1; i++)
	{
		Particle *p = new Particle();
		D3DXVECTOR3 pos(0,0,0.1f * i);

		p->setPosition(pos);
		p->setColor(D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
		p->setSize(0.3f);
		p->setLifeTime(0.4f);

		p->setInitialPhase(Global::obtainRandom(-0.2f, 0.2f));

		particles.push_back(p);
	}

	numQuads = particles.size();
}
void PlayerBurnedParticles::generate()
{
	createParticles();
	timeout.init(0.2f);
	timeoutFinish.init(0.9f);
}
