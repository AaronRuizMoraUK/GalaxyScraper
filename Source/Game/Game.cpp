#include "Game.h"
#include "resource.h"
#include "Global/GlobalVariables.h"
#include "Global/VertexTypes.h"
#include "GameState/IGameState.h"
#include "Managers/SoundManager.h"
#include "Managers/IuranusManagers/IuranusEnemyManager.h"
#include "Managers/IuranusManagers/IuranusPlanetElementsManager.h"
#include "Managers/BoilerManagers/BoilerEnemyManager.h"
#include "Managers/PlanetManager.h"
#include "Managers/StaticObjectManager.h"
#include "Managers/Static2DObjectManager.h"
#include "Managers/IuranusManagers/IuranusAnimatedObjectManager.h"
#include "Managers/IuranusManagers/IuranusShadowManager.h"
#include "Managers/PlanetAtmosphereManager.h"
#include "Managers/LogicManager/LogicManager.h"
#include "Managers/CollisionManager/IuranusCollisionManager.h"
#include "Managers/ParticlesManager.h"
#include "Managers/IuranusManagers/IuranusFixedParticlesManager.h"
#include "Managers/CollisionManager/BoilerCollisionManager.h"
#include "Managers/BoilerManagers/BoilerFixedParticlesManager.h"
#include "Managers/BoilerManagers/BoilerAnimatedObjectManager.h"
#include "Managers/BoilerManagers/BoilerFireballManager.h"
#include "Managers/BoilerManagers/FirePlatformsManager.h"
#include "Managers/BoilerManagers/LeversManager.h"
#include "Managers/BoilerManagers/CheckpointsManager.h"
#include "Managers/BoilerManagers/BoilerStaticCollionsManager.h"
#include "Logic/LuaVM/LuaVM.h"
#include "Managers/BoilerManagers/CanonsManager.h"
#include "Logic/PlanetLogic/IuranusPlanetLogic/IuranusPlanetLogic.h"
#include "Logic/PlanetLogic/BoilerPlanetLogic/BoilerPlanetLogic.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "Factories/MeshFactory.h"
#include "Factories/TextureFactory.h"
#include "Factories/EffectFactory.h"
#include "Factories/AnimationStateGraphFactory.h"
#include "Factories/CalCoreModelFactory.h"
#include "Factories/QuadFactory.h"
#include "Factories/ParticleFactory.h"
#include "Factories/CollisionShapeFactory.h"
#include "Managers/DataLocatorManager.h"
#include "Util/SpatialPartitioner/PolarAnglesPartitioner.h"
#include <cassert>

Options Game::configOptions;
Resolutions Game::allowedResolutions;

float Game::numFrames = 0.0f;
float Game::fps = 0.0f;
bool Game::normalGameSpeed = true;
bool Game::stopGameSpeed = false;
bool Game::verySlowGameSpeed = false;
bool Game::slowGameSpeed = false;
bool Game::fastGameSpeed = false;

bool Game::isGameInitiated = false;

Game::Game()
: deltaTime( 0.0f )
, isClassRegistered(false)
{
	// Hide mouse cursor
	ShowCursor(FALSE);
}

Game::~Game() {
	OutputDebugString("Destroying Game...\n");

	// Unregister the window class
	if( isClassRegistered )
		UnregisterClass( "Game", wc.hInstance );

	// Show mouse cursor
	ShowCursor(TRUE);
}

void Game::onStartElement (const std::string &elem, MKeyValue &atts) {
	if( elem=="video" ) {
		configOptions.fullScreen = atts.getInt("fullscreen", 1) ? true : false;
		configOptions.vSync = atts.getInt("vsync", 1) ? true : false;
		configOptions.resolution.width = atts.getInt("width", 800);
		configOptions.resolution.height = atts.getInt("height", 600);
		configOptions.resolution.rate = atts.getInt("rate", -1);
		if( configOptions.resolution.height != 0 )
			configOptions.aspectRatio = configOptions.resolution.width / (float)configOptions.resolution.height;
	}
	else if( elem=="options" ) {
		configOptions.quality = atts["quality"];
		if( configOptions.quality != "minimum" &&
			configOptions.quality != "low" &&
			configOptions.quality != "medium" &&
			configOptions.quality != "high" ) {
				configOptions.quality = "high";
		}
		if( configOptions.quality == "high" )
			Global::enabledRenderTexture = true;
		else
			Global::enabledRenderTexture = false;

		configOptions.music = atts.getInt("music", 100);
		configOptions.music = Global::clamp( configOptions.music, 0, 100 );
		configOptions.sfx = atts.getInt("sfx", 100);
		configOptions.sfx = Global::clamp( configOptions.sfx, 0, 100 );
	}
}

bool Game::initiate() {
	// Read properpies of game

	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToXml = dataLocatorManager->getPath("xml");

	std::string xmlFile = pathToXml + "config.xml";

	if( !xmlParseFile(xmlFile) ) {
		isGameInitiated = false;
		return isGameInitiated;
	}

	// Create window
	if( !createWindow() ) {
		isGameInitiated = false;
		return isGameInitiated;
	}

	// Init Device
	if( !initDevice() ) {
		isGameInitiated = false;
		return isGameInitiated;
	}

	// Show loading screen
	IGameState::showLoadingScreen( );

	// Init Game
	if( !initGlobals() ) {
		isGameInitiated = false;
		return isGameInitiated;
	}

	isGameInitiated = true;
	return isGameInitiated;
}

void Game::run() {

	// Enter the message loop
	MSG msg;
	ZeroMemory( &msg, sizeof(msg) );

	// Delta time variables
	unsigned long time1=0;
	unsigned long time2=0;
	timeBeginPeriod(1);
	time1 = timeGetTime(); // Get first time

	while( true )
	{
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) ) {
			// If message is QUIT
			if(msg.message==WM_QUIT)
				break;

			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else {
			// Update IO Status
			Global::ioStatus.update( deltaTime );

			// Update all scene
			IGameState::currentState->update( deltaTime );

			if(Global::exitGame)
				break;

			// Draw the scene
			IGameState::currentState->render( );

			// Get time to calculate delta time
			time2  = timeGetTime();
			deltaTime=((float)(time2-time1))/1000.0f;
			time1 = time2;

			// Change Game speed
			if(!normalGameSpeed) {
				if(stopGameSpeed)
					deltaTime=0.0f;
				if(verySlowGameSpeed)
					deltaTime/=20.0f;
				if(slowGameSpeed)
					deltaTime/=5.0f;
				if(fastGameSpeed)
					deltaTime*=3.0f;
			}

			// Update global time varible for shaders
			Global::time += deltaTime;

			// Increment the number of renders for FPS
			numFrames++;
		}
	}
}

/**
* Releases and destroy all previously initialized objects
*/
void Game::clean() {
	OutputDebugString("Begin Game Clean...\n");

	destroyEverything();

	if( CUSTOMVERTEX_TEXTURE_DECLARATION != NULL )
		CUSTOMVERTEX_TEXTURE_DECLARATION->Release(), CUSTOMVERTEX_TEXTURE_DECLARATION=NULL;

	if( CUSTOMVERTEX_NORMAL_TEXTURE_DECLARATION != NULL )
		CUSTOMVERTEX_NORMAL_TEXTURE_DECLARATION->Release(), CUSTOMVERTEX_NORMAL_TEXTURE_DECLARATION=NULL;

	if( CUSTOMVERTEX_NORMAL_TAN_TEXTURE_DECLARATION != NULL )
		CUSTOMVERTEX_NORMAL_TAN_TEXTURE_DECLARATION->Release(), CUSTOMVERTEX_NORMAL_TAN_TEXTURE_DECLARATION=NULL;

	if( CUSTOMVERTEX_COLOR_DECLARATION != NULL )
		CUSTOMVERTEX_COLOR_DECLARATION->Release(), CUSTOMVERTEX_COLOR_DECLARATION=NULL;

	if( CUSTOMVERTEX_TEXTURE_LIGHTMAP_DECLARATION != NULL )
		CUSTOMVERTEX_TEXTURE_LIGHTMAP_DECLARATION->Release(), CUSTOMVERTEX_TEXTURE_LIGHTMAP_DECLARATION=NULL;

	if( CUSTOMVERTEX_CAL3D_NORMAL_TAN_TEXTURE_DECLARATION != NULL )
		CUSTOMVERTEX_CAL3D_NORMAL_TAN_TEXTURE_DECLARATION->Release(), CUSTOMVERTEX_CAL3D_NORMAL_TAN_TEXTURE_DECLARATION=NULL;

	if( Global::font != NULL )
		Global::font->Release(), Global::font=NULL;

	Global::renderTexture.destroy();

	if( Global::device != NULL )
		Global::device->Release(), Global::device=NULL;

	if( Global::deviceCreator != NULL )
		Global::deviceCreator->Release(), Global::deviceCreator=NULL;
}

/**
* The window's message handler
*
* @param hWnd Handler of the windows
* @param msg Message to be processed
* @param wParam Information about the msg
* @param lParam Information about the msg
* @return LRESULT Return value of the processing of the message
*/
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
	case WM_CHAR:
		{
			char chCharCode = (char) wParam;

			/*
			// ESCAPE was pressed, so the application close
			if(chCharCode==VK_ESCAPE) {
			PostMessage(hWnd, WM_CLOSE, NULL, NULL); // Post a WM_CLOSE message
			}
			*/

			// CHANGE CAMERA CONTROLLER
			if(chCharCode=='c' || chCharCode=='C') {
				static bool sphereController = true;
				if(sphereController) {
					Global::camera->loadFPSRotationCameraController();
					sphereController=false;
				}
				else {
					Global::camera->loadSSPlayerCameraController();
					sphereController=true;
				}
			}

			// CHANGE GAME SPEED
			if(chCharCode=='3') {
				Game::normalGameSpeed = true;
				Game::stopGameSpeed = Game::verySlowGameSpeed = Game::slowGameSpeed = Game::fastGameSpeed = false;
			}
			else if(chCharCode=='0') {
				Game::stopGameSpeed = true;
				Game::verySlowGameSpeed = Game::slowGameSpeed = Game::normalGameSpeed = Game::fastGameSpeed = false;
			}
			else if(chCharCode=='1') {
				Game::verySlowGameSpeed = true;
				Game::stopGameSpeed = Game::slowGameSpeed = Game::normalGameSpeed = Game::fastGameSpeed = false;
			}
			else if(chCharCode=='2') {
				Game::slowGameSpeed = true;
				Game::stopGameSpeed = Game::verySlowGameSpeed = Game::normalGameSpeed = Game::fastGameSpeed = false;
			}
			else if(chCharCode=='4') {
				Game::fastGameSpeed = true;
				Game::stopGameSpeed = Game::verySlowGameSpeed = Game::slowGameSpeed = Game::normalGameSpeed = false;
			}

			// GO TO BOILER
			if(chCharCode=='6')
			{
				LogicManager *logicManager = LogicManager::getLogicManager();
				logicManager->createUserEvent("on_iuranus_victory_achieved");
			}

			// TEST VOLUME CONTROLS
			if( chCharCode == 'H' )
			{
				SoundManager *soundManager = SoundManager::getSoundManager();
				soundManager->upStreamVolume();
			}
			else if( chCharCode == 'Y' )
			{
				SoundManager *soundManager = SoundManager::getSoundManager();
				soundManager->downStreamVolume();
			}
			else if( chCharCode == 'y' )
			{
				SoundManager *soundManager = SoundManager::getSoundManager();
				soundManager->setSampleVolume(10);
				//soundManager->upSampleVolume();
			}
			else if( chCharCode == 'h' )
			{
				SoundManager *soundManager = SoundManager::getSoundManager();
				soundManager->setSampleVolume(90);
				//soundManager->downSampleVolume();
			}

			// SHOW IURANUS PLANET LOGIC VARIABLES
			if(chCharCode=='p' || chCharCode=='P')
				Global::showIuranusPlanetLogicVariables = !Global::showIuranusPlanetLogicVariables;

			// SHOW STATISTIC
			if(chCharCode=='t' || chCharCode=='T')
				Global::showStatistic = !Global::showStatistic;

			// ENABLE/DISABLE RENDER TEXTURE
			if(chCharCode=='g' || chCharCode=='G')
				Global::enabledRenderTexture = !Global::enabledRenderTexture;

			// TAKE SCREENSHOT
			if( (chCharCode=='b' || chCharCode=='B') && Global::enabledRenderTexture )
				Global::takeScreenshot = true;

			// Enable/Disable collisions
			if(chCharCode=='z')
				Global::collisionEnabled = !Global::collisionEnabled;

			// Enable/Disable IAs
			if(chCharCode=='Z')
				Global::IAEnabled = !Global::IAEnabled;

			// Enable/Disable animations
			if(chCharCode=='x')
				Global::animationEnabled = !Global::animationEnabled;

			// Enable/Disable logic
			if(chCharCode=='X')
				Global::logicEnabled = !Global::logicEnabled;

			// RELOAD SHADER
			if(chCharCode=='r' || chCharCode=='R')
				Game::reloadShaders();

			//char msg[ 128 ];
			//sprintf_s(msg, sizeof(msg), "KEY value as INT: %d\n", chCharCode);
			//OutputDebugString(msg);
		} break;
	case WM_TIMER:
		{
			// Number of frames per second
			Game::fps=Game::numFrames;
			Game::numFrames=0;
		} break;
		/*case WM_CLOSE:
		{
		//When user close the application
		int exit = MessageBox(hWnd, "Exit Galaxy Scraper?", "Exit", MB_YESNO | MB_ICONQUESTION);

		if( exit==IDYES )
		// We return the tipical windows value, so call WM_DESTROY
		return ( DefWindowProc( hWnd, msg, wParam, lParam ) );
		else
		return(0); // Do nothing, so, we go back to the game
		} break;*/
	case WM_DESTROY:
		{
			PostQuitMessage( 0 );
			return 0;
		}break;
	}

	// Process any message that we did not take care of
	return DefWindowProc( hWnd, msg, wParam, lParam );
}

void Game::reloadShaders() {
	if( isGameInitiated ) {
		// Outdate all effects in the effect factory
		EffectFactory::getEffectFactory()->outdateUpdatedEffectsMap();

		// Player
		Player::getPlayer()->reloadShader();

		// Fade
		Global::fade.reloadShader();

		// RenderTextureQuad
		Global::renderTextureQuad.reloadShader();

		// Quad UI Object
		Global::winQuadUI.reloadShader();
		Global::loseQuadUI.reloadShader();
		Global::initQuadUI.reloadShader();

		// Game State
		IGameState::reloadShaders();

		// Animated Objects Manager
		IuranusAnimatedObjectManager::getIuranusAnimatedObjectManager()->reloadShader();
		BoilerAnimatedObjectManager::getBoilerAnimatedObjectManager()->reloadShader();

		// Enemy Manager
		IuranusEnemyManager::getIuranusEnemyManager()->reloadShader();
		BoilerEnemyManager::getBoilerEnemyManager()->reloadShader();
		BoilerFireballManager::getBoilerFireballManager()->reloadShader();

		// Shadows
		IuranusShadowManager::getIuranusShadowManager()->reloadShader();

		// Particles Manager
		ParticlesManager::getParticlesManager()->reloadShader();
		IuranusFixedParticlesManager::getIuranusFixedParticlesManager()->reloadShader();
		BoilerFixedParticlesManager::getBoilerFixedParticlesManager()->reloadShader();

		// Planet Atmosphere Manager
		PlanetAtmosphereManager::getPlanetAtmosphereManager()->reloadShader();

		// Planet Elements Manager
		IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->reloadShader();
		LeversManager::getLeversManager()->reloadShader();
		CanonsManager::getCanonsManager()->reloadShader();

		// Planet Manager
		PlanetManager::getPlanetManager()->reloadShader();

		// Static Objects Manager
		StaticObjectManager::getStaticObjectManager()->reloadShader();
			
		// Static 2D Objects Manager
		Static2DObjectManager::getStatic2DObjectManager()->reloadShader();

		// Boiler StaticCollisionsManager
		BoilerStaticCollisionsManager::getBoilerStaticCollisionsManager()->reloadShader();
	}
}

bool Game::createWindow( )
{
	static const char className[] = "Game";

	// Register the window class
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		className, NULL };
	wc = wcex;
	wc.hIcon = LoadIcon(wc.hInstance, (LPCTSTR)IDI_ICON);
	//wc.hIconSm = LoadIcon(wc.hInstance, (LPCTSTR)IDI_ICON_SMALL);
	if(!RegisterClassEx( &wc ))
		return false;

	// At this point the window class has been registered, so at the end must be unregistered
	isClassRegistered=true;

	// -----------------------------------------------------------------------------------------

	// Create the deviceCreator object
	if( NULL == ( Global::deviceCreator = createDeviceCreator() ) )
		return false;

	// Check if D3DFMT_X8R8G8B8 BackBufferFormat is supported and get allowed resolutions
	allowedResolutions.insert( Resolution(800, 600, -1) );
	allowedResolutions.insert( Resolution(1024, 768, -1) );
	UINT numberModeCount = Global::deviceCreator->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8);
	char msg[ 128 ];
	for(UINT i=0; i<numberModeCount; ++i) {
		D3DDISPLAYMODE displayMode;

		if( FAILED ( Global::deviceCreator->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, i, &displayMode) ) ) {
			//sprintf_s(msg, sizeof(msg), "Not supported: %d\n", i);
			//OutputDebugString(msg);
			continue;
		}
		else {
			sprintf_s(msg, sizeof(msg), "i: %d Width: %d Height: %d RefreshRate: %d\n", i,
				displayMode.Width, displayMode.Height, displayMode.RefreshRate);
			OutputDebugString(msg);

			Resolution newResolution(displayMode.Width, displayMode.Height, displayMode.RefreshRate);
			if( newResolution.width >=800 && newResolution.width <=1920 &&
				(newResolution.rate == 60 || newResolution.rate == 70 ||
				newResolution.rate == 75 || newResolution.rate == 85) ) {

					allowedResolutions.insert( newResolution );
			}
		}
	}

	if( configOptions.fullScreen ) {
		// If resolution in config.xml is not in allowedResolutions, put 800x600:-1
		Resolutions::const_iterator res_it = allowedResolutions.find( configOptions.resolution );
		if( res_it == allowedResolutions.end() ) {
			configOptions.resolution = Resolution(800, 600, -1);
			configOptions.aspectRatio = configOptions.resolution.width / (float)configOptions.resolution.height;
		}
	}

	// -----------------------------------------------------------------------------------------

	// Create the application's window
	if( !(hWnd = CreateWindow(className, className,
		(configOptions.fullScreen) ? WS_POPUP : WS_OVERLAPPEDWINDOW, 0, 0, configOptions.resolution.width, configOptions.resolution.height,
		GetDesktopWindow(), NULL, wc.hInstance, NULL)) )
		return false;

	// Show the window
	ShowWindow( hWnd, SW_SHOWDEFAULT );
	UpdateWindow( hWnd );

	return true;
}

bool Game::initDevice()
{
	HRESULT hr;

	// Set up the structure used to create the Device with z-buffer
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.BackBufferCount = 1;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; // D3DSWAPEFFECT_DISCARD D3DSWAPEFFECT_COPY
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8; // D3DFMT_D24S8 D3DFMT_D16
	// Antialliasing. OFF=D3DMULTISAMPLE_NONE 4x=D3DMULTISAMPLE_4_SAMPLES
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	// VSync. OFF=D3DPRESENT_INTERVAL_IMMEDIATE ON=D3DPRESENT_INTERVAL_ONE
	d3dpp.PresentationInterval = configOptions.vSync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
	// Full Screen
	d3dpp.Windowed = !configOptions.fullScreen;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8; // D3DFMT_A8R8G8B8 D3DFMT_UNKNOWN D3DFMT_X8R8G8B8
	if(configOptions.fullScreen) {
		d3dpp.BackBufferWidth = configOptions.resolution.width;
		d3dpp.BackBufferHeight = configOptions.resolution.height;
		if( configOptions.resolution.rate != -1 )
			d3dpp.FullScreen_RefreshRateInHz = configOptions.resolution.rate;
	}

	static bool DEBUG_NVPERFHUD = false;
	if( !DEBUG_NVPERFHUD ) {
		// Create the device with hardware vertex processing
		if( FAILED( Global::deviceCreator->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&d3dpp, &Global::device ) ) )
		{
			// If failed, try to create the device with software vertex processing
			if( FAILED( Global::deviceCreator->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&d3dpp, &Global::device ) ) ) {
				return false;
			}
			else {
				// If the graphic card does not support D3DCREATE_HARDWARE_VERTEXPROCESSING is
				// because does not have Hardware T&L, that means the graphic card is very old, so
				// set the quality to minimum
				configOptions.quality = "minimum";
				Global::enabledRenderTexture = false;
			}
		}
		assert( Global::device || !"Device not created" );
	}
	else {

		// Set default settings
		UINT AdapterToUse=D3DADAPTER_DEFAULT;
		D3DDEVTYPE DeviceType=D3DDEVTYPE_HAL;

#if SHIPPING_VERSION
		// When building a shipping version, disable NVPerfHUD (opt-out)
#else
		// Look for 'NVIDIA NVPerfHUD' adapter
		// If it is present, override default settings
		for (UINT Adapter=0;Adapter<Global::deviceCreator->GetAdapterCount();Adapter++)
		{
			D3DADAPTER_IDENTIFIER9 Identifier;
			HRESULT Res;
			Res = Global::deviceCreator->GetAdapterIdentifier(Adapter,0,&Identifier);
			if (strstr(Identifier.Description,"NVPerfHUD") != 0)
			{
				AdapterToUse=Adapter;
				DeviceType=D3DDEVTYPE_REF;
				break;
			}
		}
#endif

		if (FAILED(Global::deviceCreator->CreateDevice( AdapterToUse, DeviceType, hWnd,
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&d3dpp, &Global::device) ) )
		{
			return false;
		}
	}

	// INIT RENDER STATE
	Global::initRenderState( );

	// FOR PRINTING
	D3DXFONT_DESC fnt;
	memset(&fnt, 0, sizeof(D3DXFONT_DESC));
	fnt.Width = 7;
	fnt.Height = 12;
	fnt.Italic = true;
	strcpy_s (fnt.FaceName, sizeof(fnt.FaceName), "arial");
	D3DXCreateFontIndirect (Global::device, &fnt, &Global::font);

	// Create Vertex declarations
	hr = Global::device->CreateVertexDeclaration( CUSTOMVERTEX_TEXTURE_ELEMENT, &CUSTOMVERTEX_TEXTURE_DECLARATION);
	assert(hr==D3D_OK);
	hr = Global::device->CreateVertexDeclaration( CUSTOMVERTEX_NORMAL_TEXTURE_ELEMENT, &CUSTOMVERTEX_NORMAL_TEXTURE_DECLARATION);
	assert(hr==D3D_OK);
	hr = Global::device->CreateVertexDeclaration( CUSTOMVERTEX_NORMAL_TAN_TEXTURE_ELEMENT, &CUSTOMVERTEX_NORMAL_TAN_TEXTURE_DECLARATION);
	assert(hr==D3D_OK);
	hr = Global::device->CreateVertexDeclaration( CUSTOMVERTEX_COLOR_ELEMENT, &CUSTOMVERTEX_COLOR_DECLARATION);
	assert(hr==D3D_OK);
	hr = Global::device->CreateVertexDeclaration( CUSTOMVERTEX_TEXTURE_LIGHTMAP_ELEMENT, &CUSTOMVERTEX_TEXTURE_LIGHTMAP_DECLARATION);
	assert(hr==D3D_OK);
	hr = Global::device->CreateVertexDeclaration( CUSTOMVERTEX_CAL3D_NORMAL_TAN_TEXTURE_ELEMENT, &CUSTOMVERTEX_CAL3D_NORMAL_TAN_TEXTURE_DECLARATION);
	assert(hr==D3D_OK);

	return true;
}

bool Game::initGlobals() {
	// Set a timer to go off 1 times a second, so calculate in MsgProc function the FPS
	SetTimer( hWnd, 0, 1000, NULL );

	// CAL3D INITIATE
	CalLoader::setLoadingMode(LOADER_ROTATE_X_AXIS); // Rotate -90 degree in X axis

	// Init IO Status
	Global::ioStatus.initiate( hWnd );

	// Load Everything
	if( !loadEverything() )
		return false;

	// Init game states
	IGameState::getState("menu_state")->getFocus();
	// IGameState::getState("init_boiler_state")->getFocus();
	//IGameState::getState("test_boiler_planet_state")->getFocus();
	//IGameState::getState("test_boiler_state")->getFocus();
	//IGameState::getState("test_roa_state")->getFocus();

	return true;
}

bool Game::loadEverything() {

	// Load Everything Common

	// Initializing Sound
	SoundManager *soundManager = SoundManager::getSoundManager();
	soundManager->setStreamVolume( configOptions.music );
	soundManager->setSampleVolume( configOptions.sfx );

	// Initializing LUA
	LuaVM::getLuaVM();

	// Initializing planets
	PlanetManager::getPlanetManager();

	// Initializing Static Objects
	StaticObjectManager::getStaticObjectManager();

	// Initializing Static 2D Objects
	Static2DObjectManager::getStatic2DObjectManager();

	// Initializing planet atmospheres
	PlanetAtmosphereManager::getPlanetAtmosphereManager();

	// Initializing the Polar Angles Partitioner
	PolarAnglesPartitioner::getPolarAnglesPartitioner();

	// Initializing Logic
	LogicManager::getLogicManager();

	// Initializing Temporal Particles
	ParticlesManager::getParticlesManager();

	// Initializing Player
	Player::getPlayer();

	// Initializing camera
	Global::activeCamera3D();
	Global::camera->initiate();

	// Init Render to Texture
	Global::renderTexture.create( configOptions.resolution.width, configOptions.resolution.height );

	// Init Fade object to make fades in/out
	Global::fade.create( );

	// RENDER TEXTURE QUAD
	D3DXMATRIX renderTextureQuadtransformMatrix;
	D3DXMatrixIdentity(&renderTextureQuadtransformMatrix);
	Global::renderTextureQuad.setTransformMatrix(renderTextureQuadtransformMatrix);

	Global::renderTextureQuad.SETMATERIAL(Global::renderTexture.getTexture(), "shader.fx", "dummy2_tech");

	IMesh * renderTextureMesh = QuadFactory::getQuadFactory()->createQuad(1.0f, 1.0f);
	assert(renderTextureMesh);
	Global::renderTextureQuad.setMesh(renderTextureMesh);

	// WIN QUAD UI
	D3DXMATRIX winQuadUItransformMatrix;
	D3DXMatrixIdentity(&winQuadUItransformMatrix);
	Global::winQuadUI.setTransformMatrix(winQuadUItransformMatrix);

	Global::winQuadUI.SETMATERIAL("win.dds", "shader.fx", "dummy_tech");

	IMesh * winMesh = QuadFactory::getQuadFactory()->createQuad(1.0f, 1.0f);
	assert(winMesh);
	Global::winQuadUI.setMesh(winMesh);

	// LOSE QUAD UI
	D3DXMATRIX loseQuadUItransformMatrix;
	D3DXMatrixIdentity(&loseQuadUItransformMatrix);
	Global::loseQuadUI.setTransformMatrix(loseQuadUItransformMatrix);

	Global::loseQuadUI.SETMATERIAL("lose.dds", "shader.fx", "dummy_tech");

	IMesh * loseMesh = QuadFactory::getQuadFactory()->createQuad(1.0f, 1.0f);
	assert(loseMesh);
	Global::loseQuadUI.setMesh(loseMesh);

	// ------------------------------------------------------

	// Load Everything about Iuranus

	// Initializing Iuranus Logic
	IuranusPlanetLogic::getIuranusPlanetLogic();

	// Initializing Iuranus Collisions
	IuranusCollisionManager::getIuranusCollisionManager();

	// Initializing Iuranus fixed particles
	IuranusFixedParticlesManager::getIuranusFixedParticlesManager();

	// Initializing Iuranus Animated Objects
	IuranusAnimatedObjectManager::getIuranusAnimatedObjectManager();

	// Initializing Iuranus Elements Objects
	IuranusPlanetElementsManager::getIuranusPlanetElementsManager();

	// Initializing Iuranus enemies
	IuranusEnemyManager::getIuranusEnemyManager();

	// Initializing Iuranus shadows
	IuranusShadowManager::getIuranusShadowManager();

	// ------------------------------------------------------

	// Load everything about Boiler

	// Initializing Boiler Logic
	BoilerPlanetLogic::getBoilerPlanetLogic();

	// Initializing Boiler Collisions
	BoilerCollisionManager::getBoilerCollisionManager();

	// Initializing Boiler fixed particles
	BoilerFixedParticlesManager::getBoilerFixedParticlesManager();

	// TODO Initializing Boiler Animated Objects?
	BoilerAnimatedObjectManager::getBoilerAnimatedObjectManager();

	// TODO Initializing Boiler Elements Objects?
	BoilerFireballManager::getBoilerFireballManager();

	// TODO Initializing Boiler enemies?
	BoilerEnemyManager::getBoilerEnemyManager();

	// Initializing Fire Platforms Manager
	FirePlatformsManager::getFirePlatformsManager();

	// Initializing Lever Manager
	LeversManager::getLeversManager();
	CanonsManager::getCanonsManager();

	// Initializing Checkpoints Manager
	CheckpointsManager::getCheckpointsManager();
	
	// Initializing BoilerCollision Manager
	BoilerStaticCollisionsManager::getBoilerStaticCollisionsManager();

	// ------------------------------------------------------

	// Load verything about game states
	IGameState::load( );

	return true;
}

void Game::destroyEverything() {

	SoundManager *soundManager = SoundManager::getSoundManager();
	if(soundManager!=NULL)
		delete soundManager, soundManager=NULL;

	IuranusShadowManager * iuranusShadowManager = IuranusShadowManager::getIuranusShadowManager();
	if(iuranusShadowManager!=NULL)
		delete iuranusShadowManager, iuranusShadowManager=NULL;

	IuranusEnemyManager *iuranusEnemyManager = IuranusEnemyManager::getIuranusEnemyManager();
	if(iuranusEnemyManager!=NULL)
		delete iuranusEnemyManager, iuranusEnemyManager=NULL;

	BoilerEnemyManager *boilerEnemyManager = BoilerEnemyManager::getBoilerEnemyManager();
	if( boilerEnemyManager != NULL )
		delete boilerEnemyManager, boilerEnemyManager=NULL;

	IuranusPlanetElementsManager *iuranusPlanetElementsManager = IuranusPlanetElementsManager::getIuranusPlanetElementsManager();
	if(iuranusPlanetElementsManager!=NULL)
		delete iuranusPlanetElementsManager, iuranusPlanetElementsManager=NULL;

	PlanetManager *planetManager = PlanetManager::getPlanetManager();
	if(planetManager!=NULL)
		delete planetManager, planetManager=NULL;

	StaticObjectManager *staticObjectManager = StaticObjectManager::getStaticObjectManager();
	if(staticObjectManager!=NULL)
		delete staticObjectManager, staticObjectManager=NULL;

	Static2DObjectManager *static2DObjectManager = Static2DObjectManager::getStatic2DObjectManager();
	if(static2DObjectManager!=NULL)
		delete staticObjectManager, static2DObjectManager=NULL;

	IuranusAnimatedObjectManager *iuranusAnimatedObjectManager = IuranusAnimatedObjectManager::getIuranusAnimatedObjectManager();
	if(iuranusAnimatedObjectManager!=NULL)
		delete iuranusAnimatedObjectManager, iuranusAnimatedObjectManager=NULL;

	PlanetAtmosphereManager *planetAtmosphereManager = PlanetAtmosphereManager::getPlanetAtmosphereManager();
	if(planetAtmosphereManager!=NULL)
		delete planetAtmosphereManager, planetAtmosphereManager=NULL;

	LogicManager *logicManager = LogicManager::getLogicManager();
	if(logicManager != NULL)
		delete logicManager, logicManager = NULL;

	IuranusCollisionManager *iuranusCollisionManager = IuranusCollisionManager::getIuranusCollisionManager();
	if(iuranusCollisionManager != NULL)
		delete iuranusCollisionManager, iuranusCollisionManager = NULL;

	BoilerCollisionManager *boilerCollisionManager = BoilerCollisionManager::getBoilerCollisionManager();
	if(boilerCollisionManager != NULL)
		delete boilerCollisionManager, boilerCollisionManager = NULL;

	BoilerStaticCollisionsManager *boilerStaticCollisionsManager = BoilerStaticCollisionsManager::getBoilerStaticCollisionsManager();
	if(boilerStaticCollisionsManager != NULL)
		delete boilerStaticCollisionsManager, boilerStaticCollisionsManager = NULL;

	ParticlesManager *particlesManager = ParticlesManager::getParticlesManager();
	if (particlesManager != NULL)
		delete particlesManager, particlesManager = NULL;

	IuranusFixedParticlesManager *iuranusFixedParticlesManager = IuranusFixedParticlesManager::getIuranusFixedParticlesManager();
	if (iuranusFixedParticlesManager != NULL)
		delete iuranusFixedParticlesManager, iuranusFixedParticlesManager = NULL;

	BoilerFixedParticlesManager *boilerFixedParticlesManager = BoilerFixedParticlesManager::getBoilerFixedParticlesManager();
	if (boilerFixedParticlesManager != NULL)
		delete boilerFixedParticlesManager, boilerFixedParticlesManager = NULL;

	BoilerAnimatedObjectManager *boilerAnimatedObjectManager = BoilerAnimatedObjectManager::getBoilerAnimatedObjectManager();
	if(boilerAnimatedObjectManager!=NULL)
		delete boilerAnimatedObjectManager, boilerAnimatedObjectManager=NULL;

	BoilerFireballManager *boilerFireballManager = BoilerFireballManager::getBoilerFireballManager();
	if(boilerFireballManager!=NULL)
		delete boilerFireballManager, boilerFireballManager=NULL;

	FirePlatformsManager *firePlatformsManager = FirePlatformsManager::getFirePlatformsManager();
	if(firePlatformsManager!=NULL)
		delete firePlatformsManager, firePlatformsManager=NULL;

	LeversManager *leversManager = LeversManager::getLeversManager();
	if(leversManager!=NULL)
		delete leversManager, leversManager=NULL;

	CanonsManager *canonsManager = CanonsManager::getCanonsManager();
	if(canonsManager!=NULL)
		delete canonsManager, canonsManager=NULL;

	LuaVM *luaVM = LuaVM::getLuaVM();
	if( luaVM != NULL)
		delete luaVM, luaVM=NULL;

	IuranusPlanetLogic *iuranusPlanetLogic = IuranusPlanetLogic::getIuranusPlanetLogic();
	if( iuranusPlanetLogic!=NULL )
		delete iuranusPlanetLogic, iuranusPlanetLogic=NULL;

	BoilerPlanetLogic *boilerPlanetLogic = BoilerPlanetLogic::getBoilerPlanetLogic();
	if( boilerPlanetLogic!=NULL )
		delete boilerPlanetLogic, boilerPlanetLogic=NULL;

	CheckpointsManager *checkpointsManager = CheckpointsManager::getCheckpointsManager();
	if( checkpointsManager!=NULL )
		delete checkpointsManager, checkpointsManager=NULL;

	Player *player = Player::getPlayer();
	if( player != NULL )
		delete player, player=NULL;

	MeshFactory *meshFactory = MeshFactory::getMeshFactory();
	if(meshFactory!=NULL)
		delete meshFactory, meshFactory=NULL;

	TextureFactory *textureFactory = TextureFactory::getTextureFactory();
	if(textureFactory!=NULL)
		delete textureFactory, textureFactory=NULL;

	EffectFactory *effectFactory = EffectFactory::getEffectFactory();
	if(effectFactory!=NULL)
		delete effectFactory, effectFactory=NULL;

	AnimationStateGraphFactory *animationStateGraphFactory = AnimationStateGraphFactory::getAnimationStateGraphFactory();
	if(animationStateGraphFactory!=NULL)
		delete animationStateGraphFactory, animationStateGraphFactory=NULL;

	CalCoreModelFactory *calCoreModelFactory = CalCoreModelFactory::getCalCoreModelFactory();
	if(calCoreModelFactory!=NULL)
		delete calCoreModelFactory, calCoreModelFactory=NULL;

	QuadFactory *quadFactory = QuadFactory::getQuadFactory();
	if(quadFactory!=NULL)
		delete quadFactory, quadFactory=NULL;

	ParticleFactory *particleFactory = ParticleFactory::getParticleFactory();
	if(particleFactory!=NULL)
		delete particleFactory, particleFactory=NULL;

	CollisionShapeFactory *collisionShapeFactory = CollisionShapeFactory::getCollisionShapeFactory();
	if(collisionShapeFactory!=NULL)
		delete collisionShapeFactory, collisionShapeFactory=NULL;

	PolarAnglesPartitioner *spatialPartitioner = PolarAnglesPartitioner::getPolarAnglesPartitioner();
	if(spatialPartitioner!=NULL)
		delete spatialPartitioner, spatialPartitioner=NULL;

	if( Global::camera!=NULL )
		Global::camera = NULL; // Set the current camera to NULL, delete not necessary because it is pointing a not dinamic memory variable

	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	if(dataLocatorManager != NULL)
		delete dataLocatorManager, dataLocatorManager = NULL;

	// NO GLOBAL VARIABLE CAN CALL A SINGLENTON IN ITS DESTROY
}
