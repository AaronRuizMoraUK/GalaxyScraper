#ifndef LOGIC_SPHERE_H
#define LOGIC_SPHERE_H

#include "Directx/Directx.h"
#include "Logic/LogicVolume/LogicVolume.h"
#include <string>

/**
* Logic Sphere Volume
*/

class LogicSphere : public LogicVolume
{
protected:
	D3DXVECTOR3 center;
	float radius;
		
	const Object *parentObject;
	D3DXVECTOR3 centerLocal;

public:

	LogicSphere(const std::string &pVolumeName,  bool pCheckForPlayer, bool pCheckIsInside, const D3DXVECTOR3 &pCenter, float pRadius);
	~LogicSphere() 
	{
		OutputDebugString("Destroying LogicSphere...\n");
	};

	const D3DXVECTOR3 &getCenter() const { return center; };
	void setCenter(const D3DXVECTOR3 &pCenter) { center = pCenter; };
	float getRadius() const { return radius; };
	void setRadius(float pRadius) { radius = pRadius; };

	bool checkObjectInsideVolume(const Object &obj) const;

	void setParentObject(const Object *pParentObject) {
		parentObject = pParentObject;
	};

	void updatePositions( ) {
		assert(parentObject);
		if(parentObject) {
			const D3DXMATRIX &parentTransformMatrix = parentObject->getTransformMatrix();
			D3DXVec3TransformCoord(&center, &centerLocal, &parentTransformMatrix);
		}
	};

private:
	bool checkInside() const;
};

#endif //LOGIC_SPHERE_H
