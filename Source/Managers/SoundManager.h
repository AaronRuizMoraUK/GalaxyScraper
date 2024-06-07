#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <map>
#include <string>
#include "Util/MapComparers/MapComparers.h"
#include "Bass/bass.h"
#include "XMLParser/XMLParser.h"

#define FADE_TIME 2500

typedef std::map<std::string, HSAMPLE, LessString> SamplesMap;
typedef std::map<std::string, HSTREAM, LessString> StreamsMap;

/**
* Manager of sounds.
*
* It is a singleton.
*/

class SoundManager : public CXMLParser
{
protected:

	std::string pathToData;

	HSTREAM *oldStream;
	HSTREAM *currentStream;

	StreamsMap streams;
	SamplesMap samples;

	int streamVolume;
	int sampleVolume;

public:
	~SoundManager() {
		OutputDebugString("Destroying SoundManager...\n");
		BASS_Free();
	};

	static SoundManager * getSoundManager();


	void onStartElement (const std::string &elem, MKeyValue &atts);
	//void onEndElement (const std::string &elem);

	void init();
	void playStream(const char *streamName);
	void stopCurrentStream();
	void playSample(const char *sampleName);
	HSAMPLE playStopableSample(const char *sampleName);
	void stopSample(DWORD sampleHandler);
	
	void setStreamVolume( int pStreamVolume ) { streamVolume = pStreamVolume; };
	int getStreamVolume() { return streamVolume; };
	void setSampleVolume( int pSampleVolume ) { sampleVolume = pSampleVolume; };
	int getSampleVolume() { return sampleVolume; };

	void upStreamVolume();
	void downStreamVolume();
	void upSampleVolume();
	void downSampleVolume();

private:

	void threatError();
	
	static SoundManager * soundManager;
	SoundManager();
};


//--------------- FLAGS FOR ENABLING 3D IN -------------------//
//-------- BASS_StreamCreateFile AND BASS_SampleLoad ---------//
//----------------- CREATING FUNCTIONS -----------------------//

/* BASS_SAMPLE_MONO | BASS_DEVICE_3D */

//------------------------------------------------------------//


//---------------- EXAMPLE OF UPDATE SOUND 3D ----------------//
	// Set the sound source position
	/*float radius=20;
	delta+=deltaTime;
	BASS_3DVECTOR musicStreamPosition(radius*cos(delta/100), 0, radius*sin(delta/100));
	BASS_3DVECTOR musicStreamOrientation(0, 0, 1);
	BASS_3DVECTOR speed(0, 0, 0);
	BASS_ChannelSet3DPosition( musicStream, &musicStreamPosition, &musicStreamOrientation, &speed);

	// Set the earing 3D position
	const D3DXVECTOR3 &cameraPosition = Global::camera->getPosition();
	BASS_3DVECTOR earingPosition(cameraPosition.x, cameraPosition.y, cameraPosition.z);
	const D3DXVECTOR3 &cameraLocalFront = Global::camera->getLocalFrontVector();
	BASS_3DVECTOR earingFront(cameraLocalFront.x, cameraLocalFront.y, cameraLocalFront.z);
	const D3DXVECTOR3 &cameraLocalUp = Global::camera->getLocalUpVector();
	BASS_3DVECTOR earingUp(cameraLocalUp.x, cameraLocalUp.y, cameraLocalUp.z);
	BASS_Set3DPosition(&earingPosition, &speed, &earingFront, &earingUp);

	// Apply 3D sound
	BASS_Apply3D();*/
//------------------------------------------------------------//


#endif //SOUND_MANAGER_H