#ifndef ORTHOGONAL_CAMERA_H
#define ORTHOGONAL_CAMERA_H

#include "ICamera.h"

/**
* Orthogonal Camera of the scene for UI
*/

class OrthogonalCamera : public ICamera {

public:
	OrthogonalCamera ();
	~OrthogonalCamera() {
		OutputDebugString("Destroying OrthogonalCamera...\n");
	};

};

#endif //ORTHOGONAL_CAMERA_H
