#include "Object.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Global/VertexTypes.h"

Object::Object() : mesh(NULL),
position(0,0,0),
localFront(1,0,0),
localLeft(0,1,0),
localUp(0,0,1),
parentObject(NULL),
drawn(true)
{
	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&localTransformMatrix);
	updateVariablesFromTransformMatrix();
}

void Object::render() const {
	Global::world.update(transformMatrix);

	// Calculate World View Projection matrix
	const D3DXMATRIX &matView = Global::camera->getMatView();
	const D3DXMATRIX &matProj = Global::camera->getMatProj();
	D3DXMATRIX matWorlViewProj;
	D3DXMatrixMultiply(&matWorlViewProj, &matView, &matProj);
	D3DXMatrixMultiply(&matWorlViewProj, &(Global::world.getMatWorld()), &matWorlViewProj);

	for(unsigned int i=0; i<materialVector.size(); ++i) {
		materialVector[i]->setMatWorldViewProj(matWorlViewProj);
		materialVector[i]->activate();

		UINT cPasses, iPass;
		const Effect &shader = materialVector[i]->getShader();
		shader->Begin( &cPasses, 0 ); // How many passes has the technique?
		for( iPass = 0; iPass < cPasses; ++iPass ) { // For each pass
			shader->BeginPass( iPass );	// Begin pass
		
			// Do the real rendering of geometry
			mesh->renderStrip(i);
 
			shader->EndPass( );	// End Pass
		}
		shader->End( );
	}

	// DRAW NORMALS AND TANGENTS. NOTE: Comment line "mesh->freeSystemMemory();" in MeshFactory.cpp
	// and "planetAtmosphereManager->render();" for not rendering atmospheres in main.cpp
	/*Global::world.activate();
	for(int i=0; i<mesh->getNumTotalVertices(); ++i) {
		const CUSTOMVERTEX_NORMAL_TAN_TEXTURE &verts = ((CUSTOMVERTEX_NORMAL_TAN_TEXTURE *) mesh->getRawVertices())[i];
		D3DXVECTOR3 src (verts.x, verts.y, verts.z);
		D3DXVECTOR3 dst (verts.x + 2*verts.nx,
							verts.y + 2*verts.ny, 
							verts.z + 2*verts.nz);
		Global::drawLine(src, dst, 0xFF777777);
		D3DXVECTOR3 dstTan (verts.x + 2*verts.tx,
							verts.y + 2*verts.ty, 
							verts.z + 2*verts.tz);
		Global::drawLine(src, dstTan, 0xFF777777);
	}*/
	
	// DRAW BOUNDING BOX, AXIS BOUNDING SPHERE, AXIS AND DIRECTION
	/*Global::world.activate();
	mesh->drawBoundingBox();
	mesh->drawAxisBoundingSphere();
	Global::drawAxis();
	Global::drawDirection();*/
}

void Object::setPosition(const D3DXVECTOR3 &pPosition) {
	position = pPosition;
	transformMatrix(3,0)=position.x;
	transformMatrix(3,1)=position.y;
	transformMatrix(3,2)=position.z;
}

float Object::getYaw() const {
	float yaw, pitch;
	Global::vectorToAngles(localFront, yaw, pitch);
	return yaw;
}

float Object::getPitch() const{
	float yaw, pitch;
	Global::vectorToAngles(localFront, yaw, pitch);
	return pitch;
}

void Object::setAngles(float yaw, float pitch) {
	D3DXMatrixRotationX(&transformMatrix, pitch);
	D3DXMatrixRotationY(&transformMatrix, yaw);
	updateVariablesFromTransformMatrix();
}

void Object::updateVariablesFromTransformMatrix() {
	localLeft.x = transformMatrix(0,0);
	localLeft.y = transformMatrix(0,1);
	localLeft.z = transformMatrix(0,2);
	localUp.x = transformMatrix(1,0);
	localUp.y = transformMatrix(1,1);
	localUp.z = transformMatrix(1,2);
	localFront.x = transformMatrix(2,0);
	localFront.y = transformMatrix(2,1);
	localFront.z = transformMatrix(2,2);
	position.x = transformMatrix(3,0);
	position.y = transformMatrix(3,1);
	position.z = transformMatrix(3,2);
}

void Object::reloadShader() {
	if(mesh != NULL) {
		MaterialVector::iterator it = materialVector.begin();

		while( it != materialVector.end() ) {
			(*it)->reloadShader();
			++it;
		}
	}
}