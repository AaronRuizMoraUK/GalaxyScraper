#include "PlayerRunParticles.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"

const std::string PlayerRunParticles::type = "playerRun";
const std::string PlayerRunParticles::textureName = "particle_humillo2.dds";

PlayerRunParticles::PlayerRunParticles():ParticleEmitter(50)
{
}


void PlayerRunParticles::update( float deltaTime )
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

			/*
			const D3DXVECTOR3 &pos = p->getPosition();
			const D3DXVECTOR3 &playerFront = Player::getPlayer()->getLocalFrontVector();
			D3DXVECTOR3 offset;

			float x, y, z;

		
			x = Global::obtainRandom(-0.5f, 0.5f) * deltaTime * 3.0f;
			y = 0.2f;
			z = Global::obtainRandom(-0.8f, -0.5f) * deltaTime;
			
			offset = D3DXVECTOR3(x, 0.0f, z);
			p->setPosition(pos + offset);
			*/

//			float tempSize = sin(p->getTime() * 2.0f + D3DX_PI / 2.0f) * 0.3f;

			float tempSize = p->getInitialPhase() + 0.3f * sin(6.0f * p->getTime() +  D3DX_PI / 2.0f);
			
			if (tempSize > 0.0f)
				p->setSize(tempSize);
			else
				p->setSize(0.0f);

			/*
			const D3DXVECTOR3 &particlePos = p->getPosition();
			D3DXVECTOR3 pos(particlePos.x + Global::obtainRandom(-0.06f,0.06f), particlePos.y, particlePos.z);
			p->setPosition(pos);
			*/
			
			++it;
		}
		++i;
	}

	timeout.update(deltaTime);

	if (timeout.isFinished())
	{
		createParticles();
		timeout.restart();
	}

	if (player->getCurrentCycle() != "jog")
	{
		timeout.init(100.0f);
	}

	numQuads = particles.size();
}

void PlayerRunParticles::createParticles()
{

	if ((particles.size() + 1) > 50)
		return;

	for (int i = 0; i < 1; i++)
	{
		const D3DXVECTOR3 &playerFront = Player::getPlayer()->getLocalFrontVector();
		const D3DXVECTOR3 &playerPos = Player::getPlayer()->getPosition();
		const D3DXVECTOR3 &playerTop = Player::getPlayer()->getLocalUpVector();
		const D3DXVECTOR3 &playerLeft = Player::getPlayer()->getLocalLeftVector();

		Particle *p = new Particle();
		D3DXVECTOR3 pos(playerPos - playerFront * 0.5f + playerLeft * Global::obtainRandom(-0.2f, 0.2f) + playerTop * 0.5f);
		p->setPosition(pos);
		p->setSize(0.0f);
		p->setInitialPhase(Global::obtainRandom(0.1f, 0.2f));
		p->setLifeTime(0.2f);
		particles.push_back(p);
	}

	numQuads = particles.size();
}
void PlayerRunParticles::generate()
{
	createParticles();
	timeout.init(0.1f);
}
