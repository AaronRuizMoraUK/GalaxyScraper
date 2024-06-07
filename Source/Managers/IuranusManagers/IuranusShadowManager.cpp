#include "IuranusShadowManager.h"
#include "Factories/ParticleFactory.h"
#include "Managers/PlanetManager.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "Object/CollisionableObject/AnimatedObject/IntelligentObject/IntelligentObject.h"
#include "Game/Game.h"

IuranusShadowManager * IuranusShadowManager::iuranusShadowManager = NULL;

const float IuranusShadowManager::IURANUS_ENEMIES_PARTICLE_SIZE = 0.5f;
const float IuranusShadowManager::PLAYER_PARTICLE_SIZE = 0.7f;

IuranusShadowManager * IuranusShadowManager::getIuranusShadowManager() {
	if(iuranusShadowManager == NULL)
		iuranusShadowManager = new IuranusShadowManager();

	return iuranusShadowManager;
}

IuranusShadowManager::IuranusShadowManager()
{
	cleanIuranusEnemiesShadowInformation( );

	init( );
}

void IuranusShadowManager::init( ) {
	// Enemies shadows object
	D3DXMATRIX enemyShadowstransformMatrix;
	D3DXMatrixIdentity( &enemyShadowstransformMatrix );
	enemieShadows.setTransformMatrix( enemyShadowstransformMatrix );

	enemieShadows.SETMATERIAL("shadow.dds", "shader.fx", "shadow_tech");

	IMesh *enemieShadowsMesh = ParticleFactory::getParticleFactory()->createParticle(MAX_SHADOWS_AT_TIME);	
	assert(enemieShadowsMesh);
	enemieShadows.setMesh(enemieShadowsMesh);

	// Player shadow object
	D3DXMATRIX playerShadowtransformMatrix;
	D3DXMatrixIdentity( &playerShadowtransformMatrix );
	playerShadow.setTransformMatrix( playerShadowtransformMatrix );

	playerShadow.SETMATERIAL("shadow.dds", "shader.fx", "shadow_tech");

	IMesh *playerShadowMesh = ParticleFactory::getParticleFactory()->createParticle(1);	
	assert(playerShadowMesh);
	playerShadow.setMesh(playerShadowMesh);
}

void IuranusShadowManager::updateEnemyShadow( const IntelligentObject *enemy, int index ) {
	const std::string &quality = Game::configOptions.quality;
	if( quality == "low" || quality == "minimum" )
		return;


	for( int i=0; i<MAX_TIMES; ++i ) {
		if( numEnemiesShadowInformation[i] > MAX_SHADOWS_AT_TIME ) {
			assert( !"Too many shadows" );
			return;
		}
	}

	int i = index / MAX_SHADOWS_AT_TIME;
	int j = index % MAX_SHADOWS_AT_TIME;

	assert( i < MAX_TIMES && j < MAX_SHADOWS_AT_TIME );

	// Increment number of shadows in i render
	numEnemiesShadowInformation[i]++;

	// Fill enemy shadow information
	const D3DXVECTOR3 &enemyFront = enemy->getLocalFrontVector();
	const D3DXVECTOR3 &enemyLeft = enemy->getLocalLeftVector();

	iuranusEnemiesShadowInformation[i][j].front_x = enemyFront.x;
	iuranusEnemiesShadowInformation[i][j].front_y = enemyFront.y;
	iuranusEnemiesShadowInformation[i][j].front_z = enemyFront.z;

	const Planet *planet = PlanetManager::getPlanetManager()->getCurrentPlanet( );
	float alpha = enemy->getAlpha( );
	float delta = enemy->getDelta( );
	iuranusEnemiesShadowInformation[i][j].distance_to_planet = planet->getHeight(alpha, delta) + 0.1f;
	//assert( iuranusEnemiesShadowInformation[i][j].distance_to_planet > 5.0f );

	iuranusEnemiesShadowInformation[i][j].left_x = enemyLeft.x;
	iuranusEnemiesShadowInformation[i][j].left_y = enemyLeft.y;
	iuranusEnemiesShadowInformation[i][j].left_z = enemyLeft.z;

	iuranusEnemiesShadowInformation[i][j].particle_size = IURANUS_ENEMIES_PARTICLE_SIZE;
}

void IuranusShadowManager::render() {
	const std::string &quality = Game::configOptions.quality;
	if( quality == "low" || quality == "minimum" )
		return;

	// Player
	ShadowInformation playerShadowInformation;
	Player *player = Player::getPlayer();
	const D3DXVECTOR3 &playerFront = player->getLocalFrontVector();
	const D3DXVECTOR3 &playerLeft = player->getLocalLeftVector();

	playerShadowInformation.front_x = playerFront.x;
	playerShadowInformation.front_y = playerFront.y;
	playerShadowInformation.front_z = playerFront.z;

	const Planet *planet = PlanetManager::getPlanetManager()->getCurrentPlanet( );
	float alpha = player->getAlpha( );
	float delta = player->getDelta( );
	playerShadowInformation.distance_to_planet = planet->getHeight(alpha, delta) + 0.1f;

	playerShadowInformation.left_x = playerLeft.x;
	playerShadowInformation.left_y = playerLeft.y;
	playerShadowInformation.left_z = playerLeft.z;

	playerShadowInformation.particle_size = PLAYER_PARTICLE_SIZE;

	render( playerShadow, &playerShadowInformation, 1 );

	// Enemies. Iuranus enemies shadows information are updates in the update function
	// of Iuranus enemy manager.
	for( int i=0; i<MAX_TIMES; ++i)
		render( enemieShadows, iuranusEnemiesShadowInformation[i], numEnemiesShadowInformation[i] );

	// Clean all iuranusEnemiesShadowInformation for next frame
	cleanIuranusEnemiesShadowInformation();
}

void IuranusShadowManager::render( const Object &object, const ShadowInformation *shadowInfs, int numShadowInformations ) const {
	if( numShadowInformations < 1 )
		return;

	Global::world.update( object.getTransformMatrix() );

	// Calculate World View Projection matrix
	const D3DXMATRIX &matView = Global::camera->getMatView();
	const D3DXMATRIX &matProj = Global::camera->getMatProj();
	D3DXMATRIX matWorlViewProj;
	D3DXMatrixMultiply(&matWorlViewProj, &matView, &matProj);
	D3DXMatrixMultiply(&matWorlViewProj, &(Global::world.getMatWorld()), &matWorlViewProj);

	const MaterialVector &materialVector = object.getMaterialVector( );
	for(unsigned int i=0; i<materialVector.size(); ++i) {
		materialVector[i]->setMatWorldViewProj(matWorlViewProj);
		materialVector[i]->activateShadow( shadowInfs, numShadowInformations );
		materialVector[i]->activate();

		UINT cPasses, iPass;
		const Effect &shader = materialVector[i]->getShader();
		shader->Begin( &cPasses, 0 ); // How many passes has the technique?
		for( iPass = 0; iPass < cPasses; ++iPass ) { // For each pass
			shader->BeginPass( iPass );	// Begin pass

			// Do the real rendering of geometry
			object.getMesh()->renderStripTriangles(0, numShadowInformations * 2);
 
			shader->EndPass( );	// End Pass
		}
		shader->End( );
	}
}

void IuranusShadowManager::reloadShader() {
	enemieShadows.reloadShader( );
	playerShadow.reloadShader( );
}
