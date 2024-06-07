#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

#include "Directx/Directx.h"
#include "World/World.h"
#include "Camera/ICamera.h"
#include "Frustum/Frustum.h"
#include "Frustum/BackSideFrustum.h"
#include "IO/IOStatus.h"
#include "Factories/MeshFactory.h"
#include "Factories/TextureFactory.h"
#include "Object/Object.h"
#include "Util/RenderTexture/RenderTexture.h"
#include "Util/Fade/Fade.h"

/**
* Several global necessary variables.
*/

namespace Global {

	//-----------------------------------------------------------------------------
	// Global variables about Device
	//-----------------------------------------------------------------------------
	extern DeviceCreator	deviceCreator;	// Used to create the Device
	extern Device			device;			// Our rendering device
#ifdef D3DX9_SUPPORTED
	extern LPD3DXFONT		font;			// For printing
#endif

	//-----------------------------------------------------------------------------
	// Global variables
	//-----------------------------------------------------------------------------
	extern World world;
	extern ICamera *camera; // Pointer to real cameras, declarated in GlobalFunctions.cpp
	extern Frustum frustum;
	extern BackSideFrustum backSideFrustum;

	extern IOStatus ioStatus;

	extern RenderTexture renderTexture; // Initialized in function initGlobals in Game.cpp
	extern Object renderTextureQuad;	// Initialized in function initGlobals in Game.cpp
	extern bool enabledRenderTexture;
	extern bool takeScreenshot;

	extern D3DXVECTOR3 lightPosition;
	extern float time;

	extern bool collisionEnabled;
	extern bool IAEnabled;
	extern bool animationEnabled;
	extern bool logicEnabled;

	extern bool youLose;
	extern bool youLoseTimeOut; // Player lose by time out
	extern bool youWin;
	extern bool alreadyFallen;
	extern bool playerFlickering;
	extern bool takePlayerControlOut;
	extern bool takePlayerControlOutUpdatingAnimation;
	extern bool samplesEnabled;
	extern bool playerInsideAssVolume;
	extern bool playerDeepingInAss;
	extern bool initIuranus;

	extern bool exitGame;

	// Quad UI Object
	// Initialized in function initGlobals in Game.cpp
	extern Object winQuadUI;
	extern Object loseQuadUI;
	extern Object initQuadUI;

	extern bool showIuranusPlanetLogicVariables;
	extern bool showStatistic;
	extern float renderTime;

	extern Fade fade;

	extern bool cameraVibrationEnabled;
}

#endif //GLOBAL_VARIABLES_H
