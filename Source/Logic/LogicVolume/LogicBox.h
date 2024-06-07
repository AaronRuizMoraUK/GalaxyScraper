#ifndef LOGIC_BOX_H
#define LOGIC_BOX_H

#include "Directx/Directx.h"
#include "Logic/LogicVolume/LogicVolume.h"
#include <string>

/**
* Logic Box Volume
*/

class LogicBox : public LogicVolume
{
protected:
	D3DXVECTOR3 pMin;
	D3DXVECTOR3 pMax;

	const Object *parentObject;
	D3DXVECTOR3 pMinLocal;
	D3DXVECTOR3 pMaxLocal;
	
public:
	LogicBox(const std::string &pVolumeName, bool pCheckForPlayer, bool pCheckIsInside, const D3DXVECTOR3 &pPMin, const D3DXVECTOR3 &pPMax);
	~LogicBox() {
		OutputDebugString("Destroying LogicBox...\n");
	};

	const D3DXVECTOR3 &getPMin() const { return pMin; };
	void setPMin(const D3DXVECTOR3 &pPMin) { pMin = pPMin; };
	const D3DXVECTOR3 &getPMax() const { return pMax; };
	void setPMax(const D3DXVECTOR3 &pPMax) { pMax = pPMax; };

	bool checkObjectInsideVolume(const Object &obj) const;

	void setParentObject(const Object *pParentObject) 
	{
		parentObject = pParentObject;
	};

	void updatePositions( ) 
	{
		assert(parentObject);
		if(parentObject) {
			const D3DXMATRIX &parentTransformMatrix = parentObject->getTransformMatrix();
			D3DXVec3TransformCoord(&pMin, &pMinLocal, &parentTransformMatrix);
			D3DXVec3TransformCoord(&pMax, &pMaxLocal, &parentTransformMatrix);
			float temp;
			if( pMin.x > pMax.x )
			{
				temp = pMin.x;
				pMin.x = pMax.x;
				pMax.x = temp;
			}
			if( pMin.y > pMax.y )
			{
				temp = pMin.y;
				pMin.y = pMax.y;
				pMax.y = temp;
			}
			if( pMin.z > pMax.z )
			{
				temp = pMin.z;
				pMin.z = pMax.z;
				pMax.z = temp;
			}
		}
	};

private:
	bool checkInside() const;
};

#endif // LOGIC_BOX_H