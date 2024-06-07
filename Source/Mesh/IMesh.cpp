#include "IMesh.h"
#include "Global/GlobalFunctions.h"

IMesh::IMesh(int pNumTotalVertices, int pNumTotalTriangles)
:numTotalVertices(pNumTotalVertices)
,numTotalTriangles(pNumTotalTriangles)
{
	D3DXVECTOR3 zero(0,0,0);
	boundingBox.minPoint = zero;
	boundingBox.maxPoint = zero;
	boundingSphere.center = zero;
	boundingSphere.radius = 0;

	// TODO Not Initialice this way
	TStrip *strip = new TStrip();
	strip->firstVertex=0;
	strip->numTriangles=numTotalTriangles;
	stripVector.push_back(strip);
}

void IMesh::constructBoundingBox(const D3DXVECTOR3 &min, const D3DXVECTOR3 &max)
{
	boundingBox.minPoint = min;
	boundingBox.maxPoint = max;
}

void IMesh::fixBoundingBoxAndSphereForCal3D( ) {
	// Fix Bounding Box and Sphere, so rotate center -90º in X (For Cal3d objects)
	D3DXMATRIX fixCal3dMatrix;
	D3DXMatrixIdentity( &fixCal3dMatrix );
	D3DXMatrixRotationX( &fixCal3dMatrix, -D3DX_PI/2.0f);

	D3DXVec3TransformCoord( &boundingBox.minPoint, &boundingBox.minPoint, &fixCal3dMatrix );
	D3DXVec3TransformCoord( &boundingBox.maxPoint, &boundingBox.maxPoint, &fixCal3dMatrix );
	D3DXVec3TransformCoord( &boundingSphere.center, &boundingSphere.center, &fixCal3dMatrix );
}

void IMesh::drawBoundingBox() const {
	const D3DXVECTOR3 & minPoint = boundingBox.minPoint;
	const D3DXVECTOR3 & maxPoint = boundingBox.maxPoint;

	D3DXVECTOR3 zero(minPoint);
	D3DXVECTOR3 one(minPoint.x, minPoint.y, maxPoint.z);
	D3DXVECTOR3 two(minPoint.x, maxPoint.y, minPoint.z);
	D3DXVECTOR3 three(minPoint.x, maxPoint.y, maxPoint.z);
	D3DXVECTOR3 four(maxPoint.x, minPoint.y, minPoint.z);
	D3DXVECTOR3 five(maxPoint.x, minPoint.y, maxPoint.z);
	D3DXVECTOR3 six(maxPoint.x, maxPoint.y, minPoint.z);
	D3DXVECTOR3 seven(maxPoint);

	Global::drawLine(zero,one,0xffff0000);
	Global::drawLine(one,three,0xffff0000);
	Global::drawLine(three,two,0xffff0000);
	Global::drawLine(two,zero,0xffff0000);
	Global::drawLine(four,five,0xffff0000);
	Global::drawLine(five,seven,0xffff0000);
	Global::drawLine(seven,six,0xffff0000);
	Global::drawLine(six,four,0xffff0000);
	Global::drawLine(zero,four,0xffff0000);
	Global::drawLine(two,six,0xffff0000);
	Global::drawLine(three,seven,0xffff0000);
	Global::drawLine(one,five,0xffff0000);
}

void IMesh::drawAxisBoundingSphere() const {
	const D3DXVECTOR3 & center = boundingSphere.center;
	const float & radius = boundingSphere.radius;

	D3DXVECTOR3 firstPoint(center.x-radius, center.y, center.z);
	D3DXVECTOR3 secondPoint(center.x+radius, center.y, center.z);
	Global::drawLine(firstPoint, secondPoint,0xffffffff);

	D3DXVECTOR3 thirdPoint(center.x, center.y-radius, center.z);
	D3DXVECTOR3 fourthPoint(center.x, center.y+radius, center.z);
	Global::drawLine(thirdPoint, fourthPoint,0xffffffff);

	D3DXVECTOR3 fifthPoint(center.x, center.y, center.z-radius);
	D3DXVECTOR3 sixthPoint(center.x, center.y, center.z+radius);
	Global::drawLine(fifthPoint, sixthPoint,0xffffffff);
}
