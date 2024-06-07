#ifndef RAILXZ_PLAYER_CONTROLLER_H
#define RAILXZ_PLAYER_CONTROLLER_H

#include "PlayerControllers/IPlayerController.h"

class RailXZPlayerController : public IPlayerController
{
protected:
	// Speed for the camera controller
	float speed;

public:
	RailXZPlayerController() : IPlayerController(), speed(5.0f) {};
	~RailXZPlayerController() {
		OutputDebugString("Destroying RailXZ player controller...\n");
	};

	void update(AnimatedObject &object, float deltaTime=(1.0f/60.0f));
};

#endif //RAILXZ_PLAYER_CONTROLLER_H