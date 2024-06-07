#ifndef PLAYER_H
#define PLAYER_H

#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"
#include "AIControllers/AISlots/Slot.h"
#include "PlayerControllers/IPlayerController.h"
#include "XMLParser/XMLParser.h"
#include "Util/SpatialPartitioner/PolarAnglesPartitioner.h"

/**
* This is the player. It is an AnimatedObject with a player
* controller and other attributes.
*
* It is a singleton.
*/

class Player : public AnimatedObject, public CXMLParser
{
protected:
	IPlayerController *playerController;

	float frontKickAngleEffect;
	float frontKickDistanceEffect;

	Slots slots;

	// Grid in Space
	PAGridIndex spaceGrid;

	D3DXMATRIX initialTransformMatrix;

public:
	~Player();
	static Player * getPlayer();

	void update(float deltaTime=(1.0f/60.0f));

	void onStartElement (const std::string &elem, MKeyValue &atts);
	//void onEndElement (const std::string &elem);

	IPlayerController * getPlayerController( ) const { return playerController; };
	void setPlayerController( IPlayerController *pPlayerController ) {
		if(playerController)
			delete playerController, playerController=NULL;
		playerController = pPlayerController;
	};

	void setPlayerToInitPosition();
	void hitRecieved(const D3DXVECTOR3 &newFront);

	float getFrontKickAngleEffect() const { return frontKickAngleEffect; };
	float getFrontKickDistanceEffect() const { return frontKickDistanceEffect; };

	const PAGridIndex & getSpatialGrid() const { return spaceGrid; };
	void setSpatialGrid(const PAGridIndex &pSpaceGrid) {
		assert( spaceGrid.size() == 2 && pSpaceGrid.size() == 2 );
		spaceGrid[0]=pSpaceGrid[0];
		spaceGrid[1]=pSpaceGrid[1];
	};

	unsigned int getNumSlots(void) const { return slots.size(); };
	
	Slot* getFreeSlot(unsigned int maxSlots);
	unsigned int getSlotIndex(Slot *s) const;
	Slot* getSlotByIndex(unsigned int i) const;
	
	bool isPlayerKicking();
private:
	Player();
	static Player *player;

	IPlayerController *controllerFactory(const std::string &controller);
	IAnimationController *animationControllerFactory(const std::string &animController);
};

#endif //PLAYER_H