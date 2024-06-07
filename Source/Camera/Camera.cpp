#include "Camera.h"
#include "CameraControllers/ICameraController.h"
#include "CameraControllers/Dummy/DummyCameraController.h"
#include "CameraControllers/FPSRotation/FPSRotationCameraController.h"
#include "CameraControllers/SSPlayer/SSPlayerCameraController.h"
#include "CameraControllers/KeyFramed/CameraKeyFramedROA.h"
#include "CameraControllers/FollowPoints/FollowPoints.h"
#include "CameraControllers/TargetPoint/TargetPoint.h"
#include "CameraControllers/FollowPlayer/FollowPlayerCameraController.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "Managers/DataLocatorManager.h"
#include "Managers/PlanetManager.h"
#include "Game/Game.h"

/**
* Default constructor. Initiates parameters needed.
*/
Camera::Camera () 
: ICamera()
, fov(D3DX_PI/4)
, initialPosition(0,0,31)
, initialTarget(0,0,16)
, initialUpVector(0,1,0)
, cameraController(NULL)
, rigidAnimationResource(NULL)
{
	// TODO
	position = D3DXVECTOR3(0,0,31);
	target = D3DXVECTOR3(0,0,16);
	//upVector = D3DXVECTOR3(0,1,0);
	zFar = 3500.0f;
}

void Camera::initiate() {
	// Calculate up vector
	// TODO THIS IS ONLY FOR SSPlayerCameraController
	// When this will be by XML, change true for string=="SSPlayerCameraController"
	//if(true)
		//calculateUpVector();

	// Obtain Player position -> Target
	Player *player = Player::getPlayer();
	const D3DXVECTOR3 &playerPosition = player->getPosition();
	target = playerPosition;

	// Initiate View Matrix and its parameters
	lookAt(position, target);

	// Initiate Projection Matrix in RH (Right Hand)
	D3DXMatrixPerspectiveFovRH( &matProj, fov, Game::configOptions.aspectRatio, 1.0f, zFar );

	// Initiate camera controller TODO
	if(cameraController)
		delete cameraController, cameraController=NULL;
	cameraController = new SSPlayerCameraController();
	//cameraController = new FPSRotationCameraController();
}

/**
* Default destructor.
*/
Camera::~Camera() {
	OutputDebugString("Destroying Camera...\n");

	// If camera is destroied, his controller is destroied
	if(cameraController)
		delete cameraController, cameraController=NULL;

	if(rigidAnimationResource)
		delete rigidAnimationResource, rigidAnimationResource=NULL;
}

/**
* Set where camera is looking at. This is position and target.
* The rest of parameter are set from position and target.
* 
* @param pPosition Camera position
* @param pTarget Camera target
*/
void Camera::lookAt(const D3DXVECTOR3 &pPosition, const D3DXVECTOR3 &pTarget) {
	position=pPosition;
	target=pTarget;

	// Update View Matrix in RH (Right Hand)
	D3DXMatrixLookAtRH( &matView, &position, &target, &upVector);

	// Update camera local axis from View Matrix
	localLeftVector = D3DXVECTOR3( -matView(0,0), -matView(1,0), -matView(2,0));
	localUpVector = D3DXVECTOR3(  matView(0,1),  matView(1,1),  matView(2,1));
	localFrontVector = D3DXVECTOR3( -matView(0,2), -matView(1,2), -matView(2,2));

	// Update yaw and pitch for localFrontVector
	Global::vectorToAngles(localFrontVector, yaw, pitch);
}

/**
* Update the camera with the camera controller
*/
void Camera::update(float deltaTime) {
	if(cameraController)
		cameraController->update(*this, deltaTime);
}

/**
* Activate camera in the Device
*/
void Camera::activate() const {
	// Set View Matrix to the Device
	D3DMATRIX view;
	memcpy(view.m, matView.m, sizeof(view.m));
	Global::device->SetTransform( D3DTS_VIEW, &view );

	// Set Projection Matrix to the Device
	D3DMATRIX proj;
	memcpy(proj.m, matView.m, sizeof(proj.m));
	Global::device->SetTransform( D3DTS_PROJECTION, &proj);
}

void Camera::loadDummyCameraController() {
	if(cameraController)
		delete cameraController, cameraController=NULL;

	cameraController = new DummyCameraController();
}

void Camera::loadFPSRotationCameraController() {
	if(cameraController)
		delete cameraController, cameraController=NULL;

	cameraController = new FPSRotationCameraController();
}

void Camera::loadFollowPlayerCameraController() {
	if(cameraController)
		delete cameraController, cameraController=NULL;

	//calculateUpVector();
	cameraController = new FollowPlayerCameraController();
}

void Camera::loadSSPlayerCameraController() {
	if(cameraController)
		delete cameraController, cameraController=NULL;

	//calculateUpVector();
	cameraController = new SSPlayerCameraController();
}

void Camera::loadSSPlayerCameraController(float targetSpeed, float positionSpeed, float distance) {
	if(cameraController)
		delete cameraController, cameraController=NULL;

	//calculateUpVector();
	cameraController = new SSPlayerCameraController(targetSpeed, positionSpeed, distance);
}

void Camera::loadFollowPointsCameraController(const std::vector<D3DXVECTOR3> &wp) {
	if(cameraController)
		delete cameraController, cameraController=NULL;

	assert(!wp.empty());
	cameraController = new FollowPoints(wp);
}

void Camera::loadTargetPointCameraController(const D3DXVECTOR3 &endPosition, const D3DXVECTOR3 &endTarget) {
	if(cameraController)
		delete cameraController, cameraController=NULL;

	cameraController = new TargetPoint(endPosition, endTarget);
}

void Camera::loadKeyFramedCameraController(const std::string &rigidAnimROAResource, bool sync) {
	if(cameraController)
		delete cameraController, cameraController=NULL;

	assert(rigidAnimROAResource!="");
	if(rigidAnimationResource)
		delete rigidAnimationResource, rigidAnimationResource=NULL;

	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToRoa = dataLocatorManager->getPath("roa");
	std::string filename = pathToRoa + rigidAnimROAResource;

	rigidAnimationResource = new RigidAnimationResource();
	bool ok = rigidAnimationResource->initFromFile(filename.c_str());
	assert(ok);

	if( sync ) {
		D3DXMATRIX transformMatrix;
		D3DXMatrixIdentity( &transformMatrix );
		transformMatrix(0,0) = localLeftVector.x;
		transformMatrix(0,1) = localLeftVector.y;
		transformMatrix(0,2) = localLeftVector.z;
		transformMatrix(1,0) = localUpVector.x;
		transformMatrix(1,1) = localUpVector.y;
		transformMatrix(1,2) = localUpVector.z;
		transformMatrix(2,0) = localFrontVector.x;
		transformMatrix(2,1) = localFrontVector.y;
		transformMatrix(2,2) = localFrontVector.z;
		transformMatrix(3,0) = position.x;
		transformMatrix(3,1) = position.y;
		transformMatrix(3,2) = position.z;

		D3DXQUATERNION rotationQuaternion;
		D3DXQuaternionRotationMatrix(&rotationQuaternion, &transformMatrix);
		D3DXQuaternionNormalize(&rotationQuaternion, &rotationQuaternion);

		cameraController = new CameraKeyFramedROA(rigidAnimationResource, position, rotationQuaternion);
	}
	else {
		cameraController = new CameraKeyFramedROA(rigidAnimationResource);
	}
}

void Camera::calculateUpVector() {
	// Obtain Player position
	Player *player = Player::getPlayer();
	const D3DXVECTOR3 &playerPosition = player->getPosition();

	// Calculate the upVector
	D3DXVECTOR3 cameraPlayerDirection = playerPosition - position;
	Global::normalizeVector3(cameraPlayerDirection, cameraPlayerDirection);
	D3DXVECTOR3 leftVector;
	const D3DXVECTOR3 up(0, 1, 0);
	D3DXVec3Cross(&leftVector, &up, &cameraPlayerDirection);
	Global::normalizeVector3(leftVector, leftVector);
	D3DXVec3Cross(&upVector, &cameraPlayerDirection, &leftVector);
}

void Camera::setCameraToInitialPosition() {
	upVector = initialUpVector;
	lookAt( initialPosition, initialTarget );
}

void Camera::destroyCameraController() {
	if(cameraController)
		delete cameraController, cameraController=NULL;
}