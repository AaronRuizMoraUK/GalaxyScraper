#include "FartParticles.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Factories/MeshFactory.h"
#include "Global/VertexTypes.h"
#include "Managers/PlanetManager.h"
#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"

using namespace PAPI;

const std::string FartParticles::type = "fart";
const std::string FartParticles::textureName = "particle_smoke.dds";

FartParticles::FartParticles():IParticles(), center(pVec(0.0f, 0.0f, 0.0f))
{
}


void FartParticles::update( float deltaTime )
{

	// Activate current group
	activateGroup();

	// Set the TimeStep
	particleContext.TimeStep(deltaTime);

	particleContext.Explosion(center, 2.0f, 1000.0f, 0.3f);

	pVec newSize = actualSize * 0.8f; 
	particleContext.TargetSize(actualSize, newSize);

	// Move particles
	particleContext.Move(true, false);

	// Kill older particles
	particleContext.KillOld(30.0f); //TODO - PARAMETRIZE TIME
	particleCurrentCount = particleContext.GetGroupCount();
 }

void FartParticles::generateSource()
{
	assert( parent && numQuads>0 && numQuads <= MAX_PARTICLES );

	// Create a group for these particles
	groupId = particleContext.GenParticleGroups(1, numQuads);

	// Activate the group
	activateGroup();

	float alpha = 0.0f;
	float delta = 0.0f;
	
	const D3DXVECTOR3 &parentPos = parent->getPosition();
	const D3DXVECTOR3 &parentUp = parent->getLocalUpVector();
	D3DXVECTOR3 vCenter = parentPos - parentUp;
	center = pVec(vCenter.x, vCenter.y, vCenter.z);
	// Define the behaviour of the particle

	PDDisc disc(pVec(parentPos.x, parentPos.y, parentPos.z), pVec(parentUp.x, parentUp.y, parentUp.z), 0.4f);
	
	// Set the size for this particles
	actualSize = pVec(0.5, 0.5, 0.5);
	particleContext.Size(actualSize);

	particleContext.TimeStep(1);
	// Adds new particles to the current particle group
	particleContext.Source(50, disc);

	particleCurrentCount = particleContext.GetGroupCount();
}
