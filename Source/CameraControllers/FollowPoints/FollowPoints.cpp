#include "FollowPoints.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"

std::string FollowPoints::controllerType = "follow_points_camera_controller";

FollowPoints::FollowPoints(const WayPoints &pWayPoint)
: ICameraController()
, state(SEEKWP)
, wayPoints(pWayPoint)
, wayPointId(0)
, distanceNextWP(2.0f)
, currentSpeed(0.0f)
, currentPosition(0.0f, 0.0f, 0.0f)
{
}

void FollowPoints::update( Camera &camera, float deltaTime ){
	
	switch (state) {
		case SEEKWP:
			if( wayPointId!=wayPoints.size()-1 && Global::distanceSqrt(camera.getPosition(), wayPoints[wayPointId])< distanceNextWP)
				state=NEXTWP;
			else
				follow(camera, wayPoints[wayPointId], 1);

			break;

		case NEXTWP:
			wayPointId = (wayPointId+1)%wayPoints.size();
			state=SEEKWP;
			break;

		default:
			assert( !"This should not happend" );
			return;
			break;
	}
}

void FollowPoints::follow (Camera &camera, const D3DXVECTOR3 &aim, float speed) {
	float yaw = camera.getYaw();
	float pitch = camera.getPitch();
	D3DXVECTOR3 position (camera.getPosition());

	//D3DXVECTOR3 eYawV(sin(eYaw), 0, cos(eYaw));
	D3DXVECTOR3 eYawV;
	Global::anglesToVector(yaw, pitch, eYawV);
	//D3DXVECTOR3 temp5(ePos.x+eYawV.x*2, ePos.y+eYawV.y*2, ePos.z+eYawV.z*2);
	//drawLine(ePos, temp5, 0xffffff00);

	// Calculate alpha

	// vector normalized

	D3DXVECTOR3 v(aim.x-position.x, aim.y-position.y, aim.z-position.z);
	Global::normalizeVector3(v,v);

	float alpha = acos(v.x*eYawV.x + v.y*eYawV.y + v.z*eYawV.z);

	// Calculate normal of plane
	D3DXVECTOR3 normal(sin(yaw-D3DX_PI/2), 0, cos(yaw-D3DX_PI/2));
	float dComponent = - (normal.x*position.x + normal.y*position.y + normal.z*position.z);

	// Check where the aim is with plane ecuation
	float sign = normal.x*aim.x + normal.y*aim.y + normal.z*aim.z + dComponent;

	float deltaG = 0.02f;
	float gir=0; // Important to initialize to 0

	if (alpha > deltaG)
		gir=deltaG;
	else if (alpha < deltaG)
		gir=alpha;

	if (sign < 0) {
		yaw+=gir;
		//camera.setYaw(yaw);
	}
	else if (sign > 0) {
		yaw-=gir;
		//camera.setYaw(yaw);
	}

	// Enemy follow player
	D3DXVECTOR3 vector;
	Global::anglesToVector(yaw, pitch, vector);
	position+=vector*0.04f*speed;
	camera.lookAt(position, position+vector);

	currentPosition = position;
}

bool FollowPoints::isFinished() { 
	if ( currentPosition == wayPoints[ wayPoints.size()-1 ] )
		return true;

	return false;
}
