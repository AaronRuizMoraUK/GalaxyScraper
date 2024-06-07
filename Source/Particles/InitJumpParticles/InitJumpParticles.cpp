#include "InitJumpParticles.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Factories/MeshFactory.h"
#include "Global/VertexTypes.h"
#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"

using namespace PAPI;

const std::string InitJumpParticles::type = "initJump";
const std::string InitJumpParticles::textureName = "particle_dust.dds";

InitJumpParticles::InitJumpParticles():IParticles()
{
}


void InitJumpParticles::update( float deltaTime )
{
	// Activate the group
	activateGroup();

	particleCurrentCount = particleContext.GetGroupCount();
	if( isEmpty() )
		return;

	// Set the TimeStep
	particleContext.TimeStep(1.0f);

	// Destroy old particles
	particleContext.KillOld(10.0f); //TODO - PARAMETRIZE TIME
	particleCurrentCount = particleContext.GetGroupCount();
	if( isEmpty() )
		return;

	if( isParentAnimated )
		((AnimatedObject *)parent)->updateAnimation( deltaTime );

	// Move particles
	particleContext.Move(true, false);
 }

void InitJumpParticles::generateSource()
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
	PDDisc disc(pVec(parentPos.x, parentPos.y, parentPos.z), pVec(parentUp.x, parentUp.y, parentUp.z), 0.4f);
	
	// Set the size for this particles
	particleContext.Size(pVec(0.1f, 0.1f, 0.1f));

	// Adds new particles to the current particle group
	particleContext.Source(numQuads, disc);

	particleCurrentCount = numQuads;
}
