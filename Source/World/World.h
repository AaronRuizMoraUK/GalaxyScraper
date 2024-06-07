#ifndef WORLD_H
#define WORLD_H

#include "Directx/Directx.h"

/**
* World of the scene
*/

class World
{

protected:
	// Identity World Matrix
	D3DXMATRIX identityMatWorld;

	// Current World Matrix
	D3DXMATRIX currentMatWorld;

public:

	World();
	~World() {
		OutputDebugString("Destroying World...\n");
	};

	const D3DXMATRIX & getMatWorld() const {return currentMatWorld;};

	void initiate();

	void update(const D3DXMATRIX &matWorld);
	void updateScale(const D3DXMATRIX &matWorld, const D3DXVECTOR3 &scaledFactors);

	void activate();
};

#endif //WORLD_H