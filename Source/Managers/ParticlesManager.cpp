#include "ParticlesManager.h"
#include "Particles/LavaBubblesParticles/LavaBubblesParticles.h"
#include "Particles/TemporalLavaBubblesParticles/TemporalLavaBubblesParticles.h"
#include "Particles/DummyParticles/DummyParticles.h"
#include "Particles/InitJumpParticles/InitJumpParticles.h"
#include "Particles/FinishJumpParticles/FinishJumpParticles.h"
#include "Particles/PlayerHitParticles/PlayerHitParticles.h"
#include "Particles/PlayerRunParticles/PlayerRunParticles.h"
#include "Particles/EnemyHitParticles/EnemyHitParticles.h"
#include "Particles/EnemyFoolishedParticles/EnemyFoolishedParticles.h"
#include "Particles/PlantsHitParticles/PlantsHitParticles.h"
#include "Particles/KilledBoilerParticles/KilledBoilerParticles.h"
#include "Particles/DesintegrateBallParticles/DesintegrateBallParticles.h"
#include "Particles/DesintegrateBallParticles/DesintegrateBallParticles.h"
#include "Particles/FirePlatformParticles/FirePlatformParticles.h"
#include "Particles/FirePlatformBaseParticles/FirePlatformBaseParticles.h"
#include "Particles/PlayerBurnedParticles/PlayerBurnedParticles.h"
#include "Particles/BoilerFireParticles/BoilerFireParticles.h"
#include "Particles/BoilerFirePlatformParticles/BoilerFirePlatformParticles.h"
#include "Factories/ParticleFactory.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Object/Object.h"
#include <cassert>

ParticlesManager * ParticlesManager::particlesManager = NULL;

ParticlesManager * ParticlesManager::getParticlesManager() {
	if(particlesManager == NULL)
		particlesManager = new ParticlesManager();

	return particlesManager;
}

void ParticlesManager::addParticle(const std::string &type, Object *parent, bool createPivot, bool pIsLocal)
{
	assert(parent);
	ParticleEmitter *p = NULL;

	/*
	if (type == DummyParticles::type)
		p = new DummyParticles();
	else if(type == InitJumpParticles::type)
		p = new InitJumpParticles();
	else if(type == FinishJumpParticles::type)
		p = new FinishJumpParticles();
	else if(type == PlayerHitParticles::type)
		p = new PlayerHitParticles();
	else if(type == EnemyHitParticles::type)
		p = new EnemyHitParticles();
	else if(type == EnemyFoolishedParticles::type)
		p = new EnemyFoolishedParticles();
	else if(type == PlantsHitParticles::type)
		p = new PlantsHitParticles();
	else if(type == PlayerRunParticles::type)
		p = new PlayerRunParticles();
	else if(type == FartParticles::type)
		p = new FartParticles();
	else {
		assert(!"Undefined particle type");
		return;
	}
	*/
	if (type == EnemyFoolishedParticles::type)
		p = new EnemyFoolishedParticles();
	else if (type == FinishJumpParticles::type)
		p = new FinishJumpParticles();
	else if (type == PlayerRunParticles::type)
		p = new PlayerRunParticles();
	else if (type == PlayerHitParticles::type)
		p = new PlayerHitParticles();
	else if (type == PlayerBurnedParticles::type)
		p = new PlayerBurnedParticles();
	else if (type == EnemyHitParticles::type)
		p = new EnemyHitParticles();
	else if (type == FirePlatformParticles::type)
		p = new FirePlatformParticles();
	else if (type == FirePlatformBaseParticles::type)
		p = new FirePlatformBaseParticles();
	else if (type == BoilerFireParticles::type)
		p = new BoilerFireParticles();
	else if (type == BoilerFirePlatformParticles::type)
		p = new BoilerFirePlatformParticles();
	else if (type == KilledBoilerParticles::type)
		p = new KilledBoilerParticles();
	else if (type == LavaBubblesParticles::type)
		p = new LavaBubblesParticles();
	else if (type == TemporalLavaBubblesParticles::type)
		p = new TemporalLavaBubblesParticles();
	else {
		// assert(!"Undefined particle type");
		return;
	}
	

	assert( p );

	if (createPivot)
	{
		Object *o = createPivotFromObject(parent);
		p->setParent(o);
		p->setIsParentPivot(true);
	}
	else
	{
		p->setParent(parent);
		p->setIsParentPivot(false);
	}

	p->isLocal = pIsLocal;
	

	IMesh *mesh = ParticleFactory::getParticleFactory()->createParticle(p->getNumQuads());	
	assert(mesh);
	p->setMesh(mesh);

	p->SETMATERIAL(p->getTextureName(), "shader.fx", "particle_tech");

	p->generate();

	push_back(p);
}

void ParticlesManager::removeParticles(Object *enemy)
{
	iterator it = begin();

	while(it!=end()) {
		// TODO Hacer algo para que las particulas que queden fuera del frustum no se vean
		if ((*it)->getParent() == enemy)
		{
			it = killParticle(it);
		}
		else
		{
			++it;	
		}
	}
}

void ParticlesManager::render() const {
	const_iterator it = begin();

	while(it!=end()) {
		// TODO Hacer algo para que las particulas que queden fuera del frustum no se vean
		(*it)->render();
		++it;
	}
}

ParticlesManager::iterator ParticlesManager::killParticle(ParticlesManager::iterator &particleIterator)
{
	ParticleEmitter *particle =(*particleIterator);

	if(particle)
		delete particle, particle=NULL;
	return erase(particleIterator);
}

void ParticlesManager::update(float deltaTime) {
	iterator it = begin();
	iterator it_end = end();

	while(it!=it_end) {
		if ((*it)->isEmpty()) {
			it = killParticle(it);
			it_end = end();
		}
		else
		{
			(*it)->update(deltaTime);
			++it;
		}
	}

	//char msg[ 128 ];
	//sprintf_s(msg, sizeof(msg), "Number of particles: %d\n", size());
	//OutputDebugString(msg);
}


void ParticlesManager::cleanUp() 
{
	while(!empty()) {
		ParticleEmitter * particle = back();

		if(particle)
			delete particle, particle=NULL;
		pop_back();
	}

}

void ParticlesManager::reloadShader() {
	iterator it = begin();
	while( it != end() ) {
		(*it)->reloadShader();
		++it;
	}
}

Object * ParticlesManager::createPivotFromObject(Object *pParent)
{
	Object * pivot = new Object();
	pivot->setTransformMatrix(pParent->getTransformMatrix());
	return pivot;
}