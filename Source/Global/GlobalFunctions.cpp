#include "GlobalVariables.h"
#include "VertexTypes.h"
//#include <stdio.h>
//#include <stdarg.h>
#include <string>
#include "Camera/Camera.h"
#include "Camera/OrthogonalCamera.h"
#include "Object/CollisionableObject/CollisionableObject.h"

namespace Global {

	// These are the real cameras
	// The pointer to them "camera" is changed in next 2 funtions
	// They are declarated here so nobody can see them, just can see "camera" (the pointer)
	Camera camera3D;
	OrthogonalCamera cameraOrthogonal;

	//-----------------------------------------------------------------------------
	// Name: drawLine()
	// Desc: Draw a line
	//-----------------------------------------------------------------------------
	void drawLine (const D3DXVECTOR3 & src, const D3DXVECTOR3 & dst, unsigned int color )
	{
		// Initialize vertices for axis
		CUSTOMVERTEX_COLOR g_points[] =
		{
			{ src.x, src.y, src.z, color, },
			{ dst.x, dst.y, dst.z, color, }
		};

		// Set the Vertex Declaration (FVF NOT USED)
		Global::device->SetVertexDeclaration( CUSTOMVERTEX_COLOR::getVertexDeclaration() );
		// Draw Lines with g_axis. User Primitive (UP)-> Stream is not needed
		Global::device->DrawPrimitiveUP(D3DPT_LINELIST, 1, g_points, sizeof(CUSTOMVERTEX_COLOR));
	}

	void drawDirection(float size){
		drawLine(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f,0.0f, 2.0f*size), 0xffffff00);
	}

	//-----------------------------------------------------------------------------
	// Name: drawAxis()
	// Desc: Draw the axis
	//-----------------------------------------------------------------------------
	void drawAxis(float size) {
		// Draw axis indicating source vertice, destiny vertice an color
		drawLine(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f*size,0.0f, 0.0f), 0xffff0000);
		drawLine(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 2.0f*size, 0.0f), 0xff00ff00);
		drawLine(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f,0.0f, 3.0f*size), 0xff0000ff);
	}

	//-----------------------------------------------------------------------------
	// Name: drawGridXZ()
	// Desc: Draw grids int the XZ plane
	//-----------------------------------------------------------------------------
	void drawGridXZ () {
		float size = 0.2f;
		int nsamples = 100;
		float length = size * nsamples;

		for (int i=-nsamples; i<nsamples; ++i) {
			float j = i * size;
			drawLine(D3DXVECTOR3(j, 0.0f, -length), D3DXVECTOR3(j, 0.0f, length), 0xff3f3f3f);
			drawLine(D3DXVECTOR3(-length, 0.0f, j), D3DXVECTOR3(length, 0.0f, j), 0xff3f3f3f);
		}
	}

	void anglesToVector(float yaw, float pitch, D3DXVECTOR3 &vector) {
		vector.x = sin(yaw)*cos(pitch);
		vector.y = -sin(pitch);
		vector.z = cos(yaw)*cos(pitch);
	}

	void vectorToAngles(const D3DXVECTOR3 &vector, float &yawOut, float &pitchOut) {
		yawOut = atan2 (vector.x, vector.z);
		pitchOut = -atan2 (vector.y, sqrt( pow(vector.x, 2) + pow(vector.z, 2)) );
	}

	void printCollisionableObjectPosition( const CollisionableObject *object ) {
		if( object == NULL || object->getCollisionObject() == NULL )
			return;

		D3DXMATRIX matrixCollision;
		DT_GetMatrixf( object->getCollisionObject(), matrixCollision);
		char msg2[ 1024 ];
		memset( msg2, 0x00, sizeof( msg2 ) );
		strcat_s( msg2, sizeof( msg2 ), "Solid Position\n" );
		for( int i=0; i<4; ++i ) {
			for( int j=0; j<4; ++j ) {
				char msg_line[128];
				memset( msg_line, 0x00, sizeof( msg_line ) );
				sprintf_s(msg_line, sizeof(msg_line), "%f ", matrixCollision(i,j));
				strcat_s( msg2, sizeof( msg2 ), msg_line );
			}
			strcat_s( msg2, sizeof( msg2 ), "\n" );
		}
		char msg_line2[128];
		memset( msg_line2, 0x00, sizeof( msg_line2 ) );
		sprintf_s(msg_line2, sizeof(msg_line2), "\nObject Position\n(%f,%f,%f)\n"
			, object->getPosition().x, object->getPosition().y, object->getPosition().z);
		strcat_s( msg2, sizeof( msg2 ), msg_line2 );
		OutputDebugString(msg2);
		MessageBox( NULL, msg2, "INF", MB_ICONERROR | MB_OK );
	}

	int print (int x, int y, int color, const char *msg, ...) {
		char txt[512];
		va_list ap;

		va_start (ap, msg);
		_vsnprintf_s (txt, sizeof(txt), 512, msg, ap);
		va_end(ap);

		RECT rect = {x,y,0,0};
		Global::font->DrawText(NULL, txt, -1, &rect, DT_CALCRECT, 0);
		Global::font->DrawText(NULL, txt, -1, &rect, DT_LEFT, color );

		return (rect.bottom - rect.top);
	}

	/**
	* Check if a key is pushed
	* 
	* @param vKey Key to check if is pushed
	* @return const bool & Key is pushed or not
	*/
	bool isPushed(int vKey) {
		// Get the key state and update the global parameter
		if( ::GetAsyncKeyState( vKey ) & 0x8000 )
			return true;
		else
			return false;
	}

	float distance(const D3DXVECTOR3 &origin, const D3DXVECTOR3 &target){
		return pow(origin.x-target.x,2) + pow(origin.y-target.y,2) + pow(origin.z-target.z,2);
	}

	float distanceSqrt(const D3DXVECTOR3 &origin, const D3DXVECTOR3 &target){
		return sqrt( distance(origin, target) );
	}

	void normalizeVector3(D3DXVECTOR3 &dst, const D3DXVECTOR3 &src) {
		dst = src / sqrt(src.x*src.x + src.y*src.y + src.z*src.z);
	}

	void fixMatrix(D3DXMATRIX &dst, const D3DXMATRIX &src) {
		D3DXVECTOR3 left(src(0,0), src(0,1), src(0,2));
		D3DXVECTOR3 up(src(1,0), src(1,1), src(1,2));
		D3DXVECTOR3 front(src(2,0), src(2,1), src(2,2));

		D3DXVec3Cross(&front, &left, &up);
		D3DXVec3Cross(&up, &front, &left);

		normalizeVector3(left, left);
		normalizeVector3(up, up);
		normalizeVector3(front, front);

		for(int i=0; i<4; ++i) {
			dst(0,i)=left[i];
			dst(1,i)=up[i];
			dst(2,i)=front[i];
			dst(3,i)=src(3,i);
		}
	}

	bool isFixedMatrix(const D3DXMATRIX &matrix) {
		D3DXVECTOR3 left(matrix(0,0), matrix(0,1), matrix(0,2));
		D3DXVECTOR3 up(matrix(1,0), matrix(1,1), matrix(1,2));
		D3DXVECTOR3 front(matrix(2,0), matrix(2,1), matrix(2,2));

		float normalLeft = sqrt(left.x*left.x + left.y*left.y + left.z*left.z);
		float normalUp = sqrt(up.x*up.x + up.y*up.y + up.z*up.z);
		float normalFront = sqrt(front.x*front.x + front.y*front.y + front.z*front.z);

		if (normalLeft!=1 || normalUp!=1 || normalFront!=1)
			return false;
		else
			return true;

		float fl = D3DXVec3Dot(&front, &left);
		float fu = D3DXVec3Dot(&front, &up);
		float lu = D3DXVec3Dot(&left, &up);

		if( fl!=0 || fu!=0 || lu!=0 )
			return false;
		else
			return true;
	}


	void normalize(float &dst, float src, float minNorm, float maxNorm, float min, float max) {
			dst = (maxNorm-minNorm)/(max-min) * (src-min) + minNorm;

			if(abs(dst)<0.0001)
				dst=0;
	}

	void normalize(D3DXVECTOR2 &dst, const D3DXVECTOR2 &src, float minNorm, float maxNorm, 
		float min, float max) {
			normalize(dst.x, src.x, minNorm, maxNorm, min, max);
			normalize(dst.y, src.y, minNorm, maxNorm, min, max);
	}

	float obtainRandom(float min, float max) {
		float randomNorm = (float)rand();
		normalize(randomNorm, randomNorm, min, max, 0.0f, RAND_MAX);
		return randomNorm;
	}

	// Status color must to be [0,1]
	void formatColorVector4ToDword(DWORD &color, const D3DXVECTOR4 &statusColor) {
		color = 0x00000000;

		float alphaF;
		normalize(alphaF, statusColor.w, 0.0f, 255.0f, 0.0f, 1.0f);
		DWORD alpha = (int)alphaF;
		alpha = alpha << 24;
		color = color | alpha;

		float redF;
		normalize(redF, statusColor.x, 0.0f, 255.0f, 0.0f, 1.0f);
		DWORD red = (int)redF;
		red = red << 16;
		color = color | red;

		float greenF;
		normalize(greenF, statusColor.y, 0.0f, 255.0f, 0.0f, 1.0f);
		DWORD green = (int)greenF;
		green = green << 8;
		color = color | green;

		float blueF;
		normalize(blueF, statusColor.z, 0.0f, 255.0f, 0.0f, 1.0f);
		DWORD blue = (int)blueF;
		color = color | blue;
	}

	void formatColorDWordToVector4(D3DXVECTOR4 &statusColor, DWORD color) {
		// Alpha
		unsigned int alphaD = color & 0xFF000000;
		unsigned int alphaMin = 0x00FFFFFF;
		unsigned int alphaMax = 0xFFFFFFFF;
		normalize(statusColor.w, alphaD, 0.0f, 1.0f, alphaMin, alphaMax);

		// X
		unsigned int xD = color & 0x00FF0000;
		unsigned int xMin = 0x0000FFFF;
		unsigned int xMax = 0x00FFFFFF;
		normalize(statusColor.x, xD, 0.0f, 1.0f, xMin, xMax);

		// Y
		unsigned int yD = color & 0x0000FF00;
		unsigned int yMin = 0x000000FF;
		unsigned int yMax = 0x0000FFFF;
		normalize(statusColor.y, yD, 0.0f, 1.0f, yMin, yMax);

		// Z
		unsigned int zD = color & 0x000000FF;
		unsigned int zMin = 0x00000000;
		unsigned int zMax = 0x000000FF;
		normalize(statusColor.z, zD, 0.0f, 1.0f, zMin, zMax);
	}

	float clamp(float value, float min, float max) {
		if ( value < min ) 
			return min;
		else if( value > max )
			return max;
		return value;
	}

	int clamp(int value, int min, int max) {
		if ( value < min ) 
			return min;
		else if( value > max )
			return max;
		return value;
	}

	void lerp(D3DXVECTOR3 &dst, const D3DXVECTOR3 &src0, const D3DXVECTOR3 &src1, float t) {
		dst = t*src0 + (1.0f-t)*src1;
	}

	void lerp(D3DXQUATERNION &dst, const D3DXQUATERNION &src0, const D3DXQUATERNION &src1, float t) {
		float dot = D3DXQuaternionDot(&src0, &src1);
		if( dot > 0.0f )
			dst = t*src0 + (1.0f-t)*src1;
		else
			dst = t*src0 - (1.0f-t)*src1;
	}

	void rotateVector3Quaternion( D3DXVECTOR3 &dst, const D3DXVECTOR3 &vector3, const D3DXQUATERNION &quaternion )
	{
		D3DXQUATERNION conjugated;
		D3DXQuaternionInverse(&conjugated, &quaternion);

		D3DXQUATERNION temp(conjugated* D3DXQUATERNION(vector3.x, vector3.y, vector3.z, 0.0f) * quaternion );

		dst.x = temp.x;
		dst.y = temp.y;
		dst.z = temp.z;
	}

	void activeCameraOrthogonal() {
		camera = &cameraOrthogonal;
	}

	void activeCamera3D() {
		camera = &camera3D;
	}

	void renderStateSetSolidFillMode() {
		device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	void renderStateSetWireFrameFillMode() {
		device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}

	void renderStateEnableZTest() {
		device->SetRenderState(D3DRS_ZENABLE, true);
		device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	}

	void renderStateDisableZTest() {
		device->SetRenderState(D3DRS_ZENABLE, false);
	}

	void renderStateEnableZWriting() {
		device->SetRenderState(D3DRS_ZWRITEENABLE, true);
	}

	void renderStateDisableZWriting() {
		device->SetRenderState(D3DRS_ZWRITEENABLE, false);
	}

	void renderStateEnableAlphaTest() {
		device->SetRenderState(D3DRS_ALPHATESTENABLE, true); 
		device->SetRenderState(D3DRS_ALPHAREF, 0x00000005);
		device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	}

	void renderStateDisableAlphaTest() {
		device->SetRenderState(D3DRS_ALPHATESTENABLE, false); 
	}

	void renderStateEnableAlphaBlending() {
		device->SetRenderState( D3DRS_ALPHABLENDENABLE , true );
		device->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA);
		device->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA);
	}

	void renderStateEnableAdditiveAlphaBlending() {
		device->SetRenderState( D3DRS_ALPHABLENDENABLE , true );
		device->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA);
		device->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_ONE);
	}

	void renderStateEnableSubtractiveAlphaBlending() {
		device->SetRenderState( D3DRS_ALPHABLENDENABLE , true );
		device->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_ZERO);
		device->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA);
	}

	void renderStateDisableAlphaBlending() {
		device->SetRenderState( D3DRS_ALPHABLENDENABLE , false );
	}

	void renderStateEnableCulling() {
		device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	}

	void renderStateDisableCulling() {
		device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	}

	void initRenderState( ) {
		// Disable Fixed Pipeline Lighting
		device->SetRenderState( D3DRS_LIGHTING, false );

		// Disable Z Test
		renderStateDisableZTest( );

		// Disable Z Writing
		renderStateDisableZWriting( );

		// Disable Z Test
		renderStateDisableZTest( );

		// Disable Alpha Blending
		renderStateDisableAlphaBlending( );

		// Back-facing triangles culling in D3DCULL_CCW mode
		renderStateEnableCulling( );
	}

	/*
	int getMouseBitMask(const std::string &key) {
		if(key=="MOUSE1")
			return VK_LBUTTON;
		else if(key=="MOUSE2")
			return VK_RBUTTON;
		else if(key=="MOUSE3")
			return VK_MBUTTON;

		return -1;
	}
	*/

	int getGamepadBitMask(const std::string &key) {
		if(key=="BUTTON0")
			return 0;
		else if(key=="BUTTON1")
			return 1;
		else if(key=="BUTTON2")
			return 2;
		else if(key=="BUTTON3")
			return 3;
		else if(key=="BUTTON4")
			return 4;
		else if(key=="BUTTON5")
			return 5;
		else if(key=="BUTTON6")
			return 6;
		else if(key=="BUTTON7")
			return 7;
		else if(key=="BUTTON8")
			return 8;
		else if(key=="BUTTON9")
			return 9;
		else if(key=="BUTTON10")
			return 10;
		else if(key=="BUTTON11")
			return 11;
		else if(key=="BUTTON12")
			return 12;
		else if(key=="BUTTON13")
			return 13;
		else if(key=="BUTTON14")
			return 14;
		else if(key=="BUTTON15")
			return 15;

		return -1;
	}

	int getXBOX360GamepadBitMask(const std::string &key) {
		if(key=="A")
			return XINPUT_GAMEPAD_A;
		else if(key=="B")
			return XINPUT_GAMEPAD_B;
		else if(key=="X")
			return XINPUT_GAMEPAD_X;
		else if(key=="Y")
			return XINPUT_GAMEPAD_Y;
		else if(key=="LEFT_SHOULDER")
			return XINPUT_GAMEPAD_LEFT_SHOULDER;
		else if(key=="RIGHT_SHOULDER")
			return XINPUT_GAMEPAD_RIGHT_SHOULDER;
		else if(key=="DPAD_UP")
			return XINPUT_GAMEPAD_DPAD_UP;
		else if(key=="DPAD_DOWN")
			return XINPUT_GAMEPAD_DPAD_DOWN;
		else if(key=="DPAD_LEFT")
			return XINPUT_GAMEPAD_DPAD_LEFT;
		else if(key=="DPAD_RIGHT")
			return XINPUT_GAMEPAD_DPAD_RIGHT;
		else if(key=="LEFT_TRIGGER")
			return VK_PAD_LTRIGGER;
		else if(key=="RIGHT_TRIGGER")
			return VK_PAD_RTRIGGER;
		else if(key=="START")
			return XINPUT_GAMEPAD_START;
		else if(key=="BACK")
			return XINPUT_GAMEPAD_BACK;

		return -1;
	}

	int getKeyboardBitMask(const std::string &key) {
		if(key.size()==1)
			return  key.c_str()[0];

		else if (key=="UP")
			return VK_UP;
		else if (key=="DOWN")
			return VK_DOWN;
		else if (key=="RIGHT")
			return VK_RIGHT ;
		else if (key=="LEFT")
			return VK_LEFT;

		else if(key=="MOUSE1")
			return VK_LBUTTON;
		else if(key=="MOUSE2")
			return VK_RBUTTON;
		else if(key=="MOUSE3")
			return VK_MBUTTON;

		else if (key=="ENTER")
			return VK_RETURN;
		else if (key=="SPACE")
			return VK_SPACE;
		else if (key=="ESCAPE")
			return VK_ESCAPE;
		else if (key=="TAB")
			return VK_TAB;
		else if (key=="BACK")
			return VK_BACK;
		else if (key=="CAPITAL")
			return VK_CAPITAL;

		else if (key=="SHIFT")
			return VK_SHIFT;
		else if (key=="CONTROL")
			return VK_CONTROL;
		else if (key=="ALT")
			return VK_MENU;
		else if (key=="LSHIFT")
			return VK_LSHIFT;
		else if (key=="RSHIFT")
			return VK_RSHIFT ;
		else if (key=="LCONTROL")
			return VK_LCONTROL;
		else if (key=="RCONTROL")
			return VK_RCONTROL ;

		else if (key=="INSERT")
			return VK_INSERT ;
		else if (key=="DELETE")
			return VK_DELETE;
		else if (key=="PRIOR")
			return VK_PRIOR;
		else if (key=="NEXT")
			return VK_NEXT;
		else if (key=="END")
			return VK_END;
		else if (key=="HOME")
			return VK_HOME;
		else if (key=="PRINT")
			return VK_PRINT;

		else if (key=="F1")
			return VK_F1;
		else if (key=="F2")
			return VK_F2;
		else if (key=="F3")
			return VK_F3;
		else if (key=="F4")
			return VK_F4;
		else if (key=="F5")
			return VK_F5;
		else if (key=="F6")
			return VK_F6;
		else if (key=="F7")
			return VK_F7;
		else if (key=="F8")
			return VK_F8;
		else if (key=="F9")
			return VK_F9;
		else if (key=="F10")
			return VK_F10;
		else if (key=="F11")
			return VK_F11;
		else if (key=="F12")
			return VK_F12;

		return -1;
	}

}
