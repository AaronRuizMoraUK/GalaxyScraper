#include "ParticleEmitter.h"
#include "Object/Object.h"
#include "Global/GlobalVariables.h"
#include "Global/VertexTypes.h"
#include "Mesh/IMesh.h"

ParticleEmitter::ParticleEmitter(int pNumQuads) : parent(NULL)
, isParentAnimated(false)
, particleCurrentCount(0)
, numQuads(pNumQuads)
, mesh(NULL)
, material(NULL)
, isParentPivot(false)
, isLocal(true)
{
	D3DXMatrixIdentity(&identity);
}

ParticleEmitter::~ParticleEmitter()
{

	
	OutputDebugString("Destroying ParticleEmitter...\n");

	if (isParentPivot)
	{
		// DELETE PARENT
		if( parent ) 
			delete parent, parent = NULL;
	}
}



void ParticleEmitter::render( )
{
	if ( isEmpty() )
		return;

	// Activate the group
	Global::world.update(identity);

	// Calculate World View Projection matrix
	const D3DXMATRIX &matView = Global::camera->getMatView();
	const D3DXMATRIX &matProj = Global::camera->getMatProj();
	D3DXMATRIX matWorlViewProj;
	D3DXMatrixMultiply(&matWorlViewProj, &matView, &matProj);
	D3DXMatrixMultiply(&matWorlViewProj, &(Global::world.getMatWorld()), &matWorlViewProj);

	material->setMatWorldViewProj(matWorlViewProj);

	material->activateParticle( *this );
	material->activate();

	UINT cPasses, iPass;
	const Effect &shader = material->getShader();
	shader->Begin( &cPasses, 0 ); // How many passes has the technique?
	for( iPass = 0; iPass < cPasses; ++iPass ) { // For each pass
		shader->BeginPass( iPass );	// Begin pass

		// Do the real rendering of geometry
		mesh->renderStripTriangles(0, numQuads * 2);

		shader->EndPass( );	// End Pass
	}
	shader->End( );
}

void ParticleEmitter::setParent(Object *pParent, bool isAnimated) {
	assert( pParent );
	assert( isParentPivot == false);
	parent = pParent;
	isParentAnimated = isAnimated;
}

void ParticleEmitter::reloadShader() {
	if(mesh != NULL && material != NULL)
		material->reloadShader();
}

/*
void ParticleEmitter::GeneratePoint(float numParticles)
{
for (int i = 0; i < numParticles; ++i)
{
Particle *p = new Particle();
particles.push_back(p);
}
}

void ParticleEmitter::GenerateDisc(float radius, float numParticles)
{
float offset = 2.0f * D3DX_PI / (float) numParticles;

for (int i = 0; i < numParticles; ++i)
{
Particle *p = new Particle();
float x, y, z;

x = radius * cos(i * offset);
z = radius * sin(i * offset);
y = 0.0f;

D3DXVECTOR3 pos(x, y, z);

p->setPosition(pos);

p->setInitialTime(offset * i);

particles.push_back(p);
}
}

void ParticleEmitter::OrbitPoint(float deltaTime)
{
float offset = 2.0f * D3DX_PI / (float) particles.size();

}
*/

