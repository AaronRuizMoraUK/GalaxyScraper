#ifndef FOLLOW_POINTS_H
#define FOLLOW_POINTS_H

#include "CameraControllers/ICameraController.h"
#include <vector>

/**
* Camera Animation Controller for making camera follows
* a secuence of points
*/

typedef std::vector<D3DXVECTOR3> WayPoints;

class FollowPoints: public ICameraController
{
protected:
	static std::string controllerType;

	enum FollowPointsStates {SEEKWP, NEXTWP};
	FollowPointsStates state;

	WayPoints wayPoints;
	unsigned int wayPointId;
	float distanceNextWP;

	// Current speed and position
	float currentSpeed;
	D3DXVECTOR3 currentPosition;

public:
    FollowPoints( const WayPoints &pWayPoint );
    ~FollowPoints() {
		OutputDebugString("Destroying FollowPoints controller...\n");
	};
    
    void update(Camera &camera, float deltaTime=(1.0f/60.0f));
    bool isFinished();

	const std::string & getControllerType () const { return controllerType; };

private:
	void follow (Camera &camera, const D3DXVECTOR3 &aim, float speed);
};

#endif // FOLLOW_POINTS_H
