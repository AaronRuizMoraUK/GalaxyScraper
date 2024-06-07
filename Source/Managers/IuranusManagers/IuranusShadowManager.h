#ifndef IURANUS_SHADOW_MANAGER_H
#define IURANUS_SHADOW_MANAGER_H

#include <Windows.h>
#include "Object/Object.h"

class IntelligentObject;

/**
* Manager of shadows for iuranus.
*
* It is a singleton.
*/

typedef struct ShadowInformationStruct {
	float front_x, front_y, front_z, distance_to_planet;
	float left_x, left_y, left_z, particle_size;
} ShadowInformation;

class IuranusShadowManager
{
public:
	static const int MAX_SHADOWS_AT_TIME = 50;
	static const int MAX_TIMES = 4;

	static const float IURANUS_ENEMIES_PARTICLE_SIZE;
	static const float PLAYER_PARTICLE_SIZE;

protected:
	Object enemieShadows;
	Object playerShadow;

	ShadowInformation iuranusEnemiesShadowInformation[MAX_TIMES][MAX_SHADOWS_AT_TIME];
	int numEnemiesShadowInformation[MAX_TIMES];

public:
	~IuranusShadowManager() {
		OutputDebugString("Destroying IuranusShadowManager...\n");
	};

	void cleanIuranusEnemiesShadowInformation( ) {
		memset(&iuranusEnemiesShadowInformation[0][0], 0, MAX_TIMES*MAX_SHADOWS_AT_TIME*sizeof(ShadowInformation) );
		memset(&numEnemiesShadowInformation[0], 0, MAX_TIMES * sizeof(int) );
	}

	static IuranusShadowManager * getIuranusShadowManager();

	void updateEnemyShadow( const IntelligentObject *enemy, int index );

	void render();

	void reloadShader();

private:
	static IuranusShadowManager * iuranusShadowManager;
	IuranusShadowManager();

	void init( );
	void render( const Object &object, const ShadowInformation *shadowInfs, int numShadowInformations ) const;
};

#endif //IURANUS_SHADOW_MANAGER_H
