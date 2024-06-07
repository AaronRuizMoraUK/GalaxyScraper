#ifndef SPHERE_PATROLMAN_H
#define SPHERE_PATROLMAN_H

#include <vector>
#include "AIControllers/IAIController.h"

typedef std::vector<D3DXVECTOR3 *> WayPointVector;
const enum SpherePatrolmanState {SEEKWP, NEXTWP, FOLLOWPLAYER, FIGHT, RESETWP};

/**
* Artificial intelligent controller that implement
* a patrolman in a spherical surface
*/

class SpherePatrolman : public IAIController
{
protected:
	static std::string controllerType;

	WayPointVector vectorWP;
	int numberWP;
	float distanceNextWP;
	float distancePlayer;
	float distancePlayerFight;
	unsigned short wayPointId;
	SpherePatrolmanState state;
	
	// Current speed
	float currentSpeed;

public:
	SpherePatrolman(int pNumberWP);
	~SpherePatrolman() {
		OutputDebugString("Destroying SpherePatrolman controller...\n");

		// Destroy way points
		while(!vectorWP.empty()) {
			unsigned int size = vectorWP.size()-1;
			D3DXVECTOR3 * wp = vectorWP[size];
			if(wp)
				delete wp, wp=NULL;
			vectorWP.pop_back();
		}
	};

	const std::string & getControllerType () const { return controllerType; };

	void update(AnimatedObject &object, float deltaTime=(1.0f/60.0f));
	int getCurrentAIState() const { return state; };
	void setCurrentAIState(int pState) { state = (SpherePatrolmanState)pState; };
	void drawWP() const;

private:
	void follow (Object &object, const D3DXVECTOR3 &aim, float speed, float deltaTime=(1.0f/60.0f));
	void directObject (Object &object, const D3DXVECTOR3 &aim, float deltaTime=(1.0f/60.0f));
	void moveObject (Object &object, float speed, float deltaTime=(1.0f/60.0f));
	void fight (Object &object, const D3DXVECTOR3 &aim, float deltaTime=1.0f/60.0f);
};

#endif //SPHERE_PATROLMAN_H
