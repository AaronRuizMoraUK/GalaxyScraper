#include "Planet.h"
#include "Global/GlobalFunctions.h"
#include <fstream>

Planet::Planet() : Object(), radius(0), name(""),
heightMapHeight(0), heightMapWidth(0), heightMap(NULL) 
{
}

Planet::~Planet() {
	OutputDebugString("Destroying Planet...\n");

	// Destroy heightMap
	if(heightMap!=NULL) {
		for(int i=0; i<heightMapHeight; ++i)
			delete [] heightMap[i], heightMap[i] = NULL;
		delete [] heightMap;
		heightMap = NULL;
	}
}

void Planet::getPolarAngles(const D3DXVECTOR3 &point, float &alphaOut, float &deltaOut) const {
	// Obtain the direction vector
	D3DXVECTOR3 direction = point - position;
	Global::normalizeVector3(direction, direction);

	// Obtain in Planet Local Coordinates from Global Coordinates of the direction
	D3DXMATRIX inverseTransform;
	D3DXMatrixInverse(&inverseTransform, NULL, &transformMatrix);

	D3DXVec3TransformNormal(&direction, &direction, &inverseTransform);

	// Set alpha and delta angles from local point
	deltaOut = asin( direction.y );
	alphaOut = atan2( direction.x, direction.z );
	if(alphaOut<0)
		alphaOut += 2.0f*D3DX_PI;
}

float Planet::getHeight(float alpha, float delta) const {

	if(heightMap==NULL)
		return 0.0f;

	// Normalize alpha and delta angles to height map matrix dimensions
	float coordDelta = 0.0f;
	float coordAlpha = 0.0f;
	Global::normalize(coordDelta, delta, 0.0f, heightMapHeight-1.0f, -D3DX_PI/2.0f, D3DX_PI/2.0f);
	Global::normalize(coordAlpha, alpha, 0.0f, heightMapWidth-1.0f, 0.0f, 2.0f*D3DX_PI);

	// Obtain the coordinates to access to heightmap matrix
	float intPart;
	float fracPart;

	int coorY;
	fracPart = modf(coordDelta, &intPart);
	if(fracPart>=0.5f)
		coorY = intPart+1;
	else
		coorY = intPart;

	int coorX;
	fracPart = modf(coordAlpha, &intPart);
	if(fracPart>=0.5f)
		coorX = intPart+1;
	else
		coorX = intPart;

	// Return Height
	return heightMap[coorY][coorX];
}

bool Planet::constructHeightMap(const std::string &filename) {
	// Destroy heightMap if exists
	if(heightMap!=NULL) {
		for(int i=0; i<heightMapHeight; ++i)
			delete [] heightMap[i], heightMap[i] = NULL;
		delete [] heightMap;
		heightMap = NULL;
	}

	// Open file
	std::ifstream myFile;
	myFile.open(filename.c_str(), std::ios::in | std::ios::binary);

	if (myFile.is_open()) {

		// Read height
		if ( !myFile.read((char *)&heightMapHeight, 2) )
			return false;

		// Read Width
		if ( !myFile.read((char *)&heightMapWidth, 2) )
			return false;

		// Check dimensions
		if( heightMapHeight<=1 || heightMapWidth<=1 )
			return false;

		// Reserve memory
		heightMap = new float *[heightMapHeight];
		for(int i=0; i<heightMapHeight; ++i)
			heightMap[i] = new float[heightMapWidth];

		// Read heights the texture
		for(int y=0; y<heightMapHeight; ++y) {
			for(int x=0; x<heightMapWidth; ++x) {
				if( !myFile.read((char *)&(heightMap[y][x]), 4) )
					return false;
			}
		}

		// Close file
		myFile.close();

		return true;
	}
	else
		return false;
}
