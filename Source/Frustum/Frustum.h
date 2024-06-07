#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "Directx/Directx.h"

/**
* Generic Frustum to avoid drawing anything that is
* outside of the vision of camera
*/

class Frustum {
protected:
	float frustum[6][4];

public:	
	Frustum() {};
	~Frustum() {
		OutputDebugString("Destroying Frustum...\n");
	};

	void update();
	bool isBSphereVisible(const D3DXVECTOR3 &center, float radius) const;
};

#endif //FRUSTUM_H
