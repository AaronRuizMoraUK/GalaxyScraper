#ifndef LOGIC_VOLUME_H
#define LOGIC_VOLUME_H

#include <Windows.h>
#include <string>
#include "Object/Object.h"

/**
* Interface of a Logic Volume
*/

class LogicVolume
{
protected:
	bool checkForPlayer;
	bool checkIsInside;
	bool inside;
	std::string volumeName;

public:
	LogicVolume(const std::string &pVolumeName, bool pCheckForPlayer, bool pCheckIsInside = false);
	virtual ~LogicVolume() 
	{
		OutputDebugString("Destroying LogicVolume...\n");
	};

	void recalc(const std::string &planetName);

	virtual bool checkObjectInsideVolume(const Object &obj) const = 0;
	bool recalcForPlayer() const { return checkForPlayer; };
	
	virtual void setParentObject(const Object *pParentObject) = 0;
	virtual void updatePositions( ) = 0;

private:
	virtual bool checkInside() const = 0;

};

#endif //LOGICVOLUME_H