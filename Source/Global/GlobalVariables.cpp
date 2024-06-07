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

namespace Global {

	//-----------------------------------------------------------------------------
	// Global variables about Device
	//-----------------------------------------------------------------------------
	DeviceCreator	deviceCreator	= NULL;	// Used to create the Device
	Device			device			= NULL;	// Our rendering device
	LPD3DXFONT		font			= NULL;	// For printing

	//-----------------------------------------------------------------------------
	// Global variables
	//-----------------------------------------------------------------------------
	World world;
	ICamera *camera = NULL;
	Frustum frustum;
	BackSideFrustum backSideFrustum;

	IOStatus ioStatus;

	RenderTexture renderTexture; // Initialized in function initGlobals in Game.cpp
	Object renderTextureQuad;	 // Initialized in function initGlobals in Game.cpp
	bool enabledRenderTexture = RenderToSurfaceSupported;

	D3DXVECTOR3 lightPosition(60.0f,85.0f,50.0f);
	float time = 0.0f;

	bool collisionEnabled = true;
	bool IAEnabled = true;
	bool animationEnabled = true;
	bool logicEnabled = true;

	bool youLose = false;
	bool youLoseTimeOut = false; // Player lose by time out
	bool youWin = false;
	bool alreadyFallen = false;
	bool playerFlickering = false;
	bool takePlayerControlOut = false;
	bool takePlayerControlOutUpdatingAnimation = false;
	bool samplesEnabled = true;
	bool playerInsideAssVolume = false;
	bool playerDeepingInAss = false;
	bool initIuranus = false;

	bool exitGame = false;

	// Quad UI Object
	Object winQuadUI;
	Object loseQuadUI;
	Object initQuadUI;

	bool showIuranusPlanetLogicVariables = false;
	bool showStatistic = false;
	float renderTime = 0.0f;

	Fade fade;

	bool cameraVibrationEnabled = false;
}