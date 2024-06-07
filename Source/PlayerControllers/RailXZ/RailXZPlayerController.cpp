#include "RailXZPlayerController.h"
#include "Global/GlobalFunctions.h"

void RailXZPlayerController::update(AnimatedObject &object, float deltaTime) {

	// Delta vector
	D3DXVECTOR3 delta(0, 0, 0);

	// Check Controllers
	if (Global::isPushed('W'))
		delta += D3DXVECTOR3(-1,0,0);
	if (Global::isPushed('S'))
		delta += D3DXVECTOR3(1,0,0);
	if (Global::isPushed('A'))
		delta += D3DXVECTOR3(0,0,1);
	if (Global::isPushed('D'))
		delta += D3DXVECTOR3(0,0,-1);

	// Update delta vector
	delta *= speed*deltaTime;

	const D3DXVECTOR3 &position = object.getPosition();
	delta.x+=position.x;
	delta.y+=position.y;
	delta.z+=position.z;

	object.setPosition(delta);
};