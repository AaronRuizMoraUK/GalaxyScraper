#ifndef CHECKPOINTS_MANAGER_H
#define CHECKPOINTS_MANAGER_H

#include <Windows.h>
#include <map>
#include "Directx/Directx.h"
#include "Expat/XMLParser.h"

/**
* Manager of boiler checkpoints.
* Not Reload Shader, no update, no render.
*
* It is a singleton.
*/

class CheckpointsManager : public std::map<int, D3DXVECTOR3>, public CXMLParser
{
protected:
	D3DXVECTOR3 currentParsed;
	int currentParsedElementId;

	int lastCheckPointAchievedId;
	int initialCheckpoint;
	
public:
	~CheckpointsManager() {
		OutputDebugString("Destroying CheckpointsManager...\n");
		cleanUp();
	};
	void cleanUp();

	static CheckpointsManager * getCheckpointsManager();
	
	void onStartElement (const std::string &elem, MKeyValue &atts);
	void onEndElement (const std::string &elem);

	void playerCheckpointAchieved(int checkpointId);
	void placePlayerLastCheckpoint();
	void placePlayerInitialCheckpoint();

	int getLastCheckpointAchievedId() { return lastCheckPointAchievedId; };
	
	void reset();

private:
	void init();
	void getCheckpointPos( D3DXVECTOR3 &out, int checkpointId );

	static CheckpointsManager * checkpointsManager;
	CheckpointsManager();
};

#endif //CHECKPOINTS_MANAGER_H
