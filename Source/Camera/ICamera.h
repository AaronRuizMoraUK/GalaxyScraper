#ifndef ICAMERA_H
#define ICAMERA_H

#include <string>
#include "Directx/Directx.h"
#include <vector>

class ICameraController;

/**
* Camera interface
*/

class ICamera {

protected:
	// View Matrix
	D3DXMATRIX matView;

	// Projection Matrix
	D3DXMATRIX matProj;

	// Parameters of View Matrix
	D3DXVECTOR3 position; // Position of the camera in the world
	D3DXVECTOR3 target; // Target point of the camera in the world
	D3DXVECTOR3 upVector; // Vector looking up in the world

	D3DXVECTOR3 localUpVector; // Local camera up vector
	D3DXVECTOR3 localLeftVector; // Local camera left vector
	D3DXVECTOR3 localFrontVector; // Local camera front vector

	float yaw, pitch; // Axis Angles

	float zFar; // Profundity of vision

public:
	ICamera ()
		: position(0,0,0)
		, target(0,0,0)
		, upVector(0,1,0)
		, localUpVector(0,1,0)
		, localLeftVector(1,0,0)
		, localFrontVector(0,0,1)
		, yaw(0.0f)
		, pitch(0.0f)
		, zFar(0.0f)
	{
		D3DXMatrixIdentity( &matView );
		D3DXMatrixIdentity( &matProj );
	};

	virtual ~ICamera(){
		OutputDebugString("Destroying ICamera...\n");
	};

	/**
	* Get camera local front vector
	* 
	* @return const D3DXVECTOR3 & Camera local front vector
	*/
	const D3DXMATRIX & getMatView() const {
		return matView;
	};

	/**
	* Get camera local front vector
	* 
	* @return const D3DXVECTOR3 & Camera local front vector
	*/
	const D3DXMATRIX & getMatProj() const {
		return matProj;
	};

	/**
	* Get camera position
	* 
	* @return const D3DXVECTOR3 & Camera Position
	*/
	const D3DXVECTOR3 & getPosition() const {
		return position;
	};

	/**
	* Get camera target
	* 
	* @return const D3DXVECTOR3 & Camera target
	*/
	const D3DXVECTOR3 & getTarget() const {
		return target;
	};

	/**
	* Get camera local up vector
	* 
	* @return const D3DXVECTOR3 & Camera local up vector
	*/
	const D3DXVECTOR3 & getLocalUpVector() const {
		return localUpVector;
	};

	/**
	* Get camera local left vector
	* 
	* @return const D3DXVECTOR3 & Camera local left vector
	*/
	const D3DXVECTOR3 & getLocalLeftVector() const {
		return localLeftVector;
	};

	/**
	* Get camera local front vector
	* 
	* @return const D3DXVECTOR3 & Camera local front vector
	*/
	const D3DXVECTOR3 & getLocalFrontVector() const {
		return localFrontVector;
	};

	const D3DXVECTOR3 & getUpVector() const {
		return upVector;
	};

	void setUpVector(const D3DXVECTOR3 &pUpVector) {
		upVector = pUpVector;
	};

	const float & getYaw() const {
		return yaw;
	};

	const float & getPitch() const {
		return pitch;
	};

	const float & getZFar() const {
		return zFar;
	};

	/////////////////////////////////////////////////////////////////
	// ---------------------------------------------------- Functions
	/////////////////////////////////////////////////////////////////

	virtual void loadDummyCameraController() {};
	virtual void loadFollowPlayerCameraController() {};
	virtual void loadFPSRotationCameraController() {};
	virtual void loadSSPlayerCameraController() {};
	virtual void loadSSPlayerCameraController(float targetSpeed, float positionSpeed, float distance) {};
	virtual void loadFollowPointsCameraController(const std::vector<D3DXVECTOR3> &wp) {};
	virtual void loadTargetPointCameraController(const D3DXVECTOR3 &endPosition, const D3DXVECTOR3 &endTarget) {};
	virtual void loadKeyFramedCameraController(const std::string &rigidAnimROAResource, bool sync=true) {};

	/**
	* Initiate the camera
	*/
	virtual void initiate() {};

	virtual void setCameraToInitialPosition() {};

	/**
	* Set where camera is looking at. This is position and target.
	* The rest of parameter are set from position and target.
	* 
	* @param pPosition Camera position
	* @param pTarget Camera target
	*/
	virtual void lookAt(const D3DXVECTOR3 &pPosition, const D3DXVECTOR3 &pTarget) {};

	/**
	* Update the camera with the camera controller
	*/
	virtual void update(float deltaTime=(1.0f/60.0f)) {};

	/**
	* Activate camera in the Device
	*/
	virtual void activate() const {};

	virtual ICameraController *getCameraController() {return NULL;};
	virtual void destroyCameraController() {};
};

#endif //ICAMERA_H
