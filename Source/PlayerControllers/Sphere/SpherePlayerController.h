#ifndef SPHERE_PLAYER_CONTROLLER_H
#define SPHERE_PLAYER_CONTROLLER_H

#include "PlayerControllers/IPlayerController.h"

// TODO by XML
// - rotationSpeed

/**
* Sphere Player Controller. This controller checks the gamepad
* and changes the player animation state.
*/

class SpherePlayerController : public IPlayerController
{
protected:
	float currentAngle;
	//float rotationSpeed;

	bool firstMomentInJump;
	float angleInFirstMomentInJump;

public:
	SpherePlayerController(float pCurrentAngle);

	~SpherePlayerController(void) {
		OutputDebugString("Destroying Sphere player controller...\n");
	};

	void update(AnimatedObject &object, float deltaTime=(1.0f/60.0f));

private:
	void rotatePlayer(AnimatedObject &object, const D3DXVECTOR2 &direction, float rotationSpeed, float deltaTime=(1.0f/60.0f));
	void movePlayerJumping(AnimatedObject &object, const D3DXVECTOR2 &direction, float deltaTime=(1.0f/60.0f));
};

#endif //SPHERE_PLAYER_CONTROLLER_H
