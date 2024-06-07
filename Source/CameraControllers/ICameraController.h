#ifndef ICAMERA_CONTROLLER_H
#define ICAMERA_CONTROLLER_H

#include "Camera/Camera.h"

/**
* Interface of Camera Controller
*/

class ICameraController {

public:
	ICameraController(){};
	virtual ~ICameraController(){
		OutputDebugString("Destroying camera controller...\n");
	};

	/**
	* Update the camera with a delta time.
	* 
	* @param camera Camera
	* @param deltaTime Delta time for update
	*/
	virtual void update(Camera &camera, float deltaTime=(1.0f/60.0f)) = 0;

	virtual const std::string & getControllerType () const = 0;

	virtual bool isFinished() {return false;};
	virtual void restart( const D3DXVECTOR3 &initialPosition, const D3DXQUATERNION &initialRotation ) {};

};

#endif //ICAMERA_CONTROLLER_H
