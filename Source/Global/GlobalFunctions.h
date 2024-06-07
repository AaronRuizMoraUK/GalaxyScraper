#ifndef GLOBAL_FUNCTIONS_H
#define GLOBAL_FUNCTIONS_H

#include "Directx/Directx.h"
#include <string>

//#define DEBUG

/**
* Several global necessary functions.
*/

class CollisionableObject;

namespace Global {

	//-----------------------------------------------------------------------------
	// Global Functions
	//-----------------------------------------------------------------------------
	void drawLine (const D3DXVECTOR3 & src, const D3DXVECTOR3 & dst, unsigned int color );
	void drawDirection(float size=1.0f);
	void drawAxis(float size=1.0f);
	void drawGridXZ();

	int print (int x, int y, int color, const char *msg, ...);
	float obtainRandom(float min, float max);

	void printCollisionableObjectPosition( const CollisionableObject *object );

	void anglesToVector(float yaw, float pitch, D3DXVECTOR3 &vector);
	void vectorToAngles(const D3DXVECTOR3 &vector, float &yawOut, float &pitchOut);

	bool isPushed(int vKey);

	bool isFixedMatrix(const D3DXMATRIX &matrix);
	void fixMatrix(D3DXMATRIX &dst, const D3DXMATRIX &src);

	float distance(const D3DXVECTOR3 &origin, const D3DXVECTOR3 &target);
	float distanceSqrt(const D3DXVECTOR3 &origin, const D3DXVECTOR3 &target);

	void normalizeVector3(D3DXVECTOR3 &dst, const D3DXVECTOR3 &src);
	void normalize(float &dst, float src, float minNorm, float maxNorm, float min, float max);
	void normalize(D3DXVECTOR2 &dst, const D3DXVECTOR2 &src, float minNorm, float maxNorm,
		float min, float max);

	float clamp(float value, float min, float max);
	int clamp(int value, int min, int max);

	void formatColorVector4ToDword(DWORD &color, const D3DXVECTOR4 &statusColor);
	void formatColorDWordToVector4(D3DXVECTOR4 &statusColor, DWORD color);

	void lerp(D3DXVECTOR3 &dst, const D3DXVECTOR3 &src0, const D3DXVECTOR3 &src1, float t);
	void lerp(D3DXQUATERNION &dst, const D3DXQUATERNION &src0, const D3DXQUATERNION &src1, float t);
	void rotateVector3Quaternion( D3DXVECTOR3 &dst, const D3DXVECTOR3 &vector3,
		const D3DXQUATERNION &quaternion );

	void activeCameraOrthogonal();
	void activeCamera3D();

	void renderStateSetSolidFillMode();
	void renderStateSetWireframeFillMode();

	void renderStateEnableZTest();
	void renderStateDisableZTest();

	void renderStateEnableZWriting();
	void renderStateDisableZWriting();

	void renderStateEnableAlphaTest();
	void renderStateDisableAlphaTest();

	void renderStateEnableAlphaBlending();
	void renderStateEnableAdditiveAlphaBlending();
	void renderStateEnableSubtractiveAlphaBlending();
	void renderStateDisableAlphaBlending();

	void renderStateEnableCulling();
	void renderStateDisableCulling();

	void initRenderState( );

	int getKeyboardBitMask(const std::string &key);
	//int getMouseBitMask(const std::string &key);
	int getGamepadBitMask(const std::string &key);
	int getXBOX360GamepadBitMask(const std::string &key);
}

#endif //GLOBAL_FUNCTIONS_H
