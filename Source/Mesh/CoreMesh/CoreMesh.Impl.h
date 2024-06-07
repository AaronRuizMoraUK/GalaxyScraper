#ifndef COREMESHIMPL_H
#define COREMESHIMPL_H

#include <cassert>
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"

template<class VertexType, class IndexType>
CoreMesh<VertexType,IndexType>::CoreMesh(int pNumTotalVertices, int pNumTotalTriangles) 
: IMesh(pNumTotalVertices, pNumTotalTriangles)
, vertexBuffer( NULL )
, indexBuffer( NULL )
, systemVertices( NULL )
, systemIndexes( NULL )
{
	allocate(numTotalVertices, numTotalTriangles);
}

template<class VertexType, class IndexType>
CoreMesh<VertexType,IndexType>::~CoreMesh()
{
	OutputDebugString("Destroying CoreMesh...\n");

	freeVRAM( );
	freeSystemMemory( );
}

template<class VertexType, class IndexType>
void CoreMesh<VertexType,IndexType>::constructBoundingSphere()
{
	const D3DXVECTOR3 & minPoint = boundingBox.minPoint;
	const D3DXVECTOR3 & maxPoint = boundingBox.maxPoint;

	// Get the center of the Sphere
	D3DXVECTOR3 & center = boundingSphere.center;
	center.x = (minPoint.x+maxPoint.x)/2.0f;
	center.y = (minPoint.y+maxPoint.y)/2.0f;
	center.z = (minPoint.z+maxPoint.z)/2.0f;

	// Get the radius (max distance to vertices)
	D3DXVECTOR3 systemVertexPosition (systemVertices[0].x, systemVertices[0].y, systemVertices[0].z);
	float distanceCenterPoint = Global::distance(center, systemVertexPosition); 
	for(int i=1; i<numTotalVertices; ++i) {
		D3DXVECTOR3 systemVertexPositionTemp (systemVertices[i].x, systemVertices[i].y, systemVertices[i].z);
		float distanceCenterPointTemp = Global::distance(center, systemVertexPositionTemp);

		if (distanceCenterPointTemp>distanceCenterPoint)
			distanceCenterPoint=distanceCenterPointTemp;
	}

	boundingSphere.radius=sqrt(distanceCenterPoint);
}

template<class VertexType, class IndexType>
void CoreMesh<VertexType,IndexType>::render(int firstVertex, int numTriangles) const {
	assert( vertexBuffer && indexBuffer );

	// Set index buffer in device
	Global::device->SetIndices( indexBuffer );

	// Activate Stream and Vertex Declaration (FVF NOT USED)
	activateStream( 0 );
	Global::device->SetVertexDeclaration( VertexType::getVertexDeclaration() );

	int numTrianglesToRender = numTriangles;
	if(numTriangles==-1)
		numTrianglesToRender=numTotalTriangles;

	// Draw
	HRESULT hr = Global::device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST
		, 0
		, 0
		, numTotalVertices
		, firstVertex
		, numTrianglesToRender);

	assert( hr == D3D_OK );
}

template<class VertexType, class IndexType>
void CoreMesh<VertexType,IndexType>::renderStrip(int index) const {
	render(stripVector[index]->firstVertex, stripVector[index]->numTriangles);
}

template<class VertexType, class IndexType>
void CoreMesh<VertexType,IndexType>::renderStripTriangles(int index, int numTriangles) const {
	assert( numTriangles <= stripVector[index]->numTriangles );
	assert( numTriangles > 0 );
	render(stripVector[index]->firstVertex, numTriangles);
}

template<class VertexType, class IndexType>
void CoreMesh<VertexType,IndexType>::allocate( int pNumTotalVertices, int pNumTotalTriangles ) {
	numTotalVertices = pNumTotalVertices;
	numTotalTriangles = pNumTotalTriangles;

	freeSystemMemory( ); 

	systemVertices = new VertexType[ numTotalVertices ];
	systemIndexes = new IndexType[ 3*numTotalTriangles ];
}

template<class VertexType, class IndexType>
void CoreMesh<VertexType,IndexType>::freeSystemMemory( ) {
	if( systemVertices )
		delete [] systemVertices, systemVertices = NULL;
	if( systemIndexes )
		delete [] systemIndexes, systemIndexes = NULL;
}

template<class VertexType, class IndexType>
void CoreMesh<VertexType,IndexType>::loadToVRAM( ) {
	// Free vertex and index buffer
	freeVRAM();

	// Create the vertex buffer.
	if( FAILED( Global::device->CreateVertexBuffer( numTotalVertices*sizeof(VertexType),
		D3DUSAGE_WRITEONLY,
		0, // FVF NOT USED
		D3DPOOL_DEFAULT, &vertexBuffer, NULL ) ) )
	{
		return;
	}

	// TODO WARNING, if IndexType is short -> D3DFMT_INDEX16
	// if it is int or long -> D3DFMT_INDEX32

	// Create the index buffer.
	if( FAILED( Global::device->CreateIndexBuffer( 3*numTotalTriangles*sizeof(IndexType),
		D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, 
		&indexBuffer, NULL ) ) )
	{
		return;
	}

	assert( systemVertices );

	// Fill the vertex buffer.
	VertexType* pVertices;
	if( FAILED( vertexBuffer->Lock( 0,		// Fill from start of the buffer
		numTotalVertices*sizeof(VertexType),// Size of the data to load
		(void**)&pVertices,					// Returned index data
		0 ) ) )								// Send default flags to the lock
	{
		return;
	}
	memcpy( pVertices, systemVertices, numTotalVertices*sizeof(VertexType) );
	vertexBuffer->Unlock();

	assert( systemIndexes );

	// Fill the index buffer.
	IndexType* pIndices;
	if( FAILED( indexBuffer->Lock( 0,			// Fill from start of the buffer
		3*numTotalTriangles*sizeof(IndexType),	// Size of the data to load
		(void**)&pIndices,						// Returned index data
		0 ) ) )									// Send default flags to the lock
	{
		return;
	}
	memcpy( pIndices, systemIndexes, 3*numTotalTriangles*sizeof(IndexType) );
	indexBuffer->Unlock();
}

template<class VertexType, class IndexType>
void CoreMesh<VertexType,IndexType>::freeVRAM( ) {
	if( vertexBuffer )
		vertexBuffer->Release( ), vertexBuffer = NULL;
	if( indexBuffer )
		indexBuffer->Release( ), indexBuffer = NULL;
}

template<class VertexType, class IndexType>
void * CoreMesh<VertexType,IndexType>::getRawVertices( ) const { 
	assert( systemVertices );
	return systemVertices;
}

template<class VertexType, class IndexType>
void * CoreMesh<VertexType,IndexType>::getRawIndexes() const {
	assert( systemIndexes );
	return systemIndexes;
}

template<class VertexType, class IndexType>
void CoreMesh<VertexType,IndexType>::activateStream( int streamNumber ) const {
	Global::device->SetStreamSource( streamNumber, vertexBuffer, 0, sizeof(VertexType) );
}

#endif //COREMESHIMPL_H