#ifndef CAMERA_H
#define CAMERA_H

#include "ICamera.h"
#include "AnimationControllers/KeyFramed/RigidAnimationResource.h"

class ICameraController;

/**
* 3D Camera of the scene
*/

// TODO by XML
// - position
// - zFar
// - controller to use
// - aspect ratio

class Camera : public ICamera {

protected:
	// Saving initial parameters
	D3DXVECTOR3 initialPosition;
	D3DXVECTOR3 initialTarget;
	D3DXVECTOR3 initialUpVector; // before doing the lookAt call
								 // when willing to regenerate initial position
								 // ensure that upVector is initialUpVector

	// Parameters of Projection Matrix
	float fov; // Field of View for camera (angle in radians)

	// Camera controller
	ICameraController * cameraController;

	// Rigid Animation Resource for Camera keyframed ROA
	RigidAnimationResource *rigidAnimationResource;

public:
	Camera ();
	~Camera();

	void loadDummyCameraController();
	void loadFollowPlayerCameraController();
	void loadFPSRotationCameraController();
	void loadSSPlayerCameraController();
	void loadSSPlayerCameraController(float targetSpeed, float positionSpeed, float distance);
	void loadFollowPointsCameraController(const std::vector<D3DXVECTOR3> &wp);
	void loadTargetPointCameraController(const D3DXVECTOR3 &endPosition, const D3DXVECTOR3 &endTarget);
	void loadKeyFramedCameraController(const std::string &rigidAnimROAResource, bool sync=true);

	/**
	* Initiate the camera
	*/
	void initiate();

	void setCameraToInitialPosition();

	/**
	* Set where camera is looking at. This is position and target.
	* The rest of parameter are set from position and target.
	* 
	* @param pPosition Camera position
	* @param pTarget Camera target
	*/
	void lookAt(const D3DXVECTOR3 &pPosition, const D3DXVECTOR3 &pTarget);

	/**
	* Update the camera with the camera controller
	*/
	void update(float deltaTime=(1.0f/60.0f));

	/**
	* Activate camera in the Device
	*/
	void activate() const;

	ICameraController *getCameraController() {return cameraController;};

	void destroyCameraController();

private:
	void calculateUpVector();

};

#endif //CAMERA_H
