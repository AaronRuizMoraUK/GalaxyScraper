#include "IParticles.h"
#include "Object/Object.h"
#include "Global/GlobalVariables.h"
#include "Global/VertexTypes.h"
#include "Mesh/IMesh.h"

PAPI::ParticleContext_t IParticles::particleContext;

void IParticles::render( )
{
	if ( isEmpty() )
		return;

	// Activate the group
	activateGroup( );

	Global::world.update(identity);

	// Calculate World View Projection matrix
	const D3DXMATRIX &matView = Global::camera->getMatView();
	const D3DXMATRIX &matProj = Global::camera->getMatProj();
	D3DXMATRIX matWorlViewProj;
	D3DXMatrixMultiply(&matWorlViewProj, &matView, &matProj);
	D3DXMatrixMultiply(&matWorlViewProj, &(Global::world.getMatWorld()), &matWorlViewProj);

	material->setMatWorldViewProj(matWorlViewProj);

//	material->activateParticle( );
	material->activate();

	UINT cPasses, iPass;
	const Effect &shader = material->getShader();
	shader->Begin( &cPasses, 0 ); // How many passes has the technique?
	for( iPass = 0; iPass < cPasses; ++iPass ) { // For each pass
		shader->BeginPass( iPass );	// Begin pass

		// Do the real rendering of geometry
		mesh->renderStripTriangles(0, particleCurrentCount * 2);

		shader->EndPass( );	// End Pass
	}
	shader->End( );
}

void IParticles::activateGroup() {
	particleContext.CurrentGroup( groupId );
}

void IParticles::setParent(Object *pParent, bool isAnimated) {
	assert( pParent );
	parent = pParent;
	isParentAnimated = isAnimated;
}

void IParticles::reloadShader() {
	if(mesh != NULL && material != NULL)
		material->reloadShader();
}
