#include <Windows.h>
#include "TestROAState.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Game/Game.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "Managers/DataLocatorManager.h"
#include "PlayerControllers/KeyFramed/PlayerKeyFramedROA.h"

std::string TestROAState::name = "test_roa_state";

TestROAState::TestROAState( )
: IGameState()
, nextAnimationResource( NULL ) 
, previousAnimationResource( NULL ) 
{
}

TestROAState::~TestROAState()
{
	OutputDebugString("Destroying TestROAState...\n");

	// Player is already destroy

	// Destroy animations resources
	if(nextAnimationResource)
		delete nextAnimationResource, nextAnimationResource = NULL;
	if(previousAnimationResource)
		delete previousAnimationResource, previousAnimationResource = NULL;
};

void TestROAState::update( float deltaTime ) {
	// Update camera 
	Global::camera->update(deltaTime);

	Player *player = Player::getPlayer();

	updatePlayerController( deltaTime );

	player->updateAnimation(deltaTime);

	if( Global::isPushed( 'Q' ) ) {
		xmlParseFile( "data/xmls/prueba.xml" );
	}
}

void TestROAState::onStartElement (const std::string &elem, MKeyValue &atts) {
	if( elem == "video" ) {
		atts.put( "fullscreen", 26 );

		std::ofstream output_file;
		output_file.open("data/xmls/prueba.xml");
		output_file << "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n";
		output_file << "<config>\n";
		atts.writeSingle(output_file, elem.c_str() );
		output_file << "</config>\n";
		output_file.close();
	}
}

void TestROAState::updatePlayerController( float deltaTime ) {
	Player *player = Player::getPlayer();

	IPlayerController *playerController = player->getPlayerController();

	if( playerController != NULL ) {

		// If animation is finished, set menu player controller to NULL
		if( playerController->isFinished() )
			player->setPlayerController(NULL);
		// If animation is not finished, update it
		else
			player->update(deltaTime);
	}
	else {
		// Check player pushed UP
		if( Global::ioStatus.walk.becomesPressedUp() || Global::ioStatus.walk.becomesPressedRight() ) {
			D3DXQUATERNION rotationQuaternion;
			D3DXQuaternionRotationMatrix(&rotationQuaternion, &player->getTransformMatrix());

			PlayerKeyFramedROA *nextAnimation = new PlayerKeyFramedROA( nextAnimationResource, player->getPosition(), rotationQuaternion);
			//nextAnimation->setTimeScale(0.5f);

			player->setPlayerController(nextAnimation);
			player->update(deltaTime);
		}
		// Check player pushed DOWN
		else if( Global::ioStatus.walk.becomesPressedDown() || Global::ioStatus.walk.becomesPressedLeft() ) {
			D3DXQUATERNION rotationQuaternion;
			D3DXQuaternionRotationMatrix(&rotationQuaternion, &player->getTransformMatrix());
			
			PlayerKeyFramedROA *previousAnimation = new PlayerKeyFramedROA( previousAnimationResource, player->getPosition(), rotationQuaternion);
			//previousAnimation->setTimeScale(0.5f);

			player->setPlayerController(previousAnimation);
			player->update(deltaTime);
		}
	}
}

void TestROAState::preRender( ) {
	// Activate the camera in the device.
	// For normal execution it is not necesary, but for
	// calling any Global draw function it is.
	Global::camera->activate();

	// --- Debug draws --- //
	// Initiate the wold matrix
	Global::world.initiate();
	Global::world.activate();

	// Draw Grid XZ
	Global::drawGridXZ();

	// Draw axis
	Global::drawAxis();

	// Draw Back Side Frustum
	//Global::backSideFrustum.render();
	// ------------------ //

	// SET WIREFRAME FILLMODE
	//Global::renderStateSetWireframeFillMode();
}

void TestROAState::renderWorld3D( ) {
	Global::renderStateEnableZTest( );
	Global::renderStateEnableZWriting( );

	Player::getPlayer()->render( );

	Global::renderStateDisableZWriting( );
	Global::renderStateDisableZTest( );
}

void TestROAState::renderWorld2D( ) {
}

void TestROAState::renderPostProcessed2D( ) {
}

void TestROAState::renderGameUI( ) {
}

void TestROAState::postRender( ) {
	// Print FPS
	Global::print(0,0, 0xffff0000, "Frames per second %0.2f", Game::fps);
}

void TestROAState::renderFrame( ) {
	Global::renderTextureQuad.render( );
}

void TestROAState::onLoad( ) {
	D3DXVECTOR3 cameraInitialPosition(0,10,10);
	D3DXVECTOR3 cameraLookAt( D3DXVECTOR3(0,0,0) );
	Global::camera->setUpVector( D3DXVECTOR3(0,1,0) );
	Global::camera->lookAt( cameraInitialPosition, cameraLookAt );
	Global::camera->loadFPSRotationCameraController();

	D3DXMATRIX playerTransform;
	D3DXMatrixIdentity( &playerTransform );
	//D3DXMatrixRotationY( &playerTransform, D3DX_PI/2.0f );

	Player *player = Player::getPlayer();
	player->setTransformMatrix( playerTransform );
	player->setPosition( D3DXVECTOR3(0,0,0) );
	player->setPlayerController( NULL );
}

void TestROAState::onUnload( ) {
}

void TestROAState::loadObjects( ) {
	// Animation Resources
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToRoa = dataLocatorManager->getPath("roa");

	nextAnimationResource = new RigidAnimationResource();
	std::string filename = pathToRoa + "init_play2.xml";
	bool ok = nextAnimationResource->initFromFile(filename.c_str());
	assert(ok);

	previousAnimationResource = new RigidAnimationResource();
	filename = pathToRoa + "menu_previous.xml";
	ok = previousAnimationResource->initFromFile(filename.c_str());
	assert(ok);
}
