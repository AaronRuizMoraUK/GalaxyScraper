#include "World.h"
#include "Global/GlobalVariables.h"

World::World() {
	// Initiate World Matrix
	D3DXMatrixIdentity(&identityMatWorld);
	initiate( );
}

/**
* Set the world matrix to the initiate values.
*/
void World::initiate() {
	currentMatWorld = identityMatWorld;
}

/**
* Update the world matrix to the position and
* rotation that the parameter indicates
* 
* @param matWorld Position and rotation to the world matrix
*/
void World::update(const D3DXMATRIX &matWorld) {
	currentMatWorld = matWorld;
}

/**
* Update the world matrix to the position and
* rotation that the parameter indicates with a scaled factor
* 
* @param matWorld Position and rotation to the world matrix
* @param scaledFactors Scaled factors (in x, y and z) for scaling
*                      the world matrix
*/
void World::updateScale(const D3DXMATRIX &matWorld, const D3DXVECTOR3 &scaledFactors){
	// Obtain the matrix with scale factors
	D3DXMATRIX scaledMatrix;
	D3DXMatrixScaling(&scaledMatrix, scaledFactors.x, scaledFactors.y, scaledFactors.z);

	// Apply the scale matrix to the world matrix
	D3DXMatrixMultiply(&currentMatWorld, &scaledMatrix, &matWorld);
}

/**
* Activate the world matrix of the device with current world matrix
*/
void World::activate(){
	// Set World Matrix to the Device
	Global::device->SetTransform( D3DTS_WORLD, &currentMatWorld );
}
