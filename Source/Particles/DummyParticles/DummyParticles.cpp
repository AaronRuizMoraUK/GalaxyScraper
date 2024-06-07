#include "DummyParticles.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Factories/MeshFactory.h"
#include "Global/VertexTypes.h"
#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"

using namespace PAPI;

const std::string DummyParticles::type = "dummy";
const std::string DummyParticles::textureName = "particle_flare.dds";

DummyParticles::DummyParticles():IParticles()
{
}


void DummyParticles::update( float deltaTime )
{
//	D3DXVECTOR3 pos = parent->getPosition();
//	D3DXVECTOR3 up = parent->getLocalUpVector();

	/*
	 particleContext.Velocity(pVec(1,1,1));
    particleContext.Size(1.5);


	particleContext.OrbitPoint(pVec(0, 0, 0), 0.05f, 1.5f);

	 particleContext.Gravitate(1.0f, 0.0f);
//    particleContext.Gravity(GravityVec);

    particleContext.OrbitPoint(pVec(0.0f, 0.0f, 0.0f), 0);

	particleContext.Jet(PDDisc(pVec(0,0,0), pVec(0,1,0), 4, 4), PDDisc(pVec(0,0,0), pVec(-1,-1,-1), 4, 4));
	*/

	// Activate the group
	activateGroup();

	particleCurrentCount = particleContext.GetGroupCount();
	if( isEmpty() )
		return;

	// Set the TimeStep
	particleContext.TimeStep(1.0f);

	// Destroy old particles
	particleContext.KillOld(100.0f); //TODO - PARAMETRIZE TIME
	particleCurrentCount = particleContext.GetGroupCount();
	if( isEmpty() )
		return;

	if( isParentAnimated )
		((AnimatedObject *)parent)->updateAnimation( deltaTime );

	// Move particles
	particleContext.Move(true, false);
 }

void DummyParticles::generateSource()
{
	assert( parent && numQuads>0 && numQuads <= MAX_PARTICLES );

	// Create a group for these particles
	groupId = particleContext.GenParticleGroups(1, numQuads);

	// Activate the group
	activateGroup();

	// Define the behaviour of the particle

	const D3DXVECTOR3 &parentPos = parent->getPosition();
	const D3DXVECTOR3 &parentUp = parent->getLocalUpVector();
	//PDPoint center(pVec(parentPos.x, parentPos.y, parentPos.z));
	PDDisc disc(pVec(parentPos.x, parentPos.y, parentPos.z), pVec(parentUp.x, parentUp.y, parentUp.z), 2);
	
	// Set the size for this particles
	particleContext.Size(pVec(1, 1, 1));

	// Adds new particles to the current particle group
	particleContext.Source(numQuads, disc);

	particleCurrentCount = numQuads;
}
