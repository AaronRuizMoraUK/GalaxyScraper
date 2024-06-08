#include "SoundManager.h"
#include "Object/Planet/Planet.h"
#include "Managers/DataLocatorManager.h"
#include "Global/GlobalFunctions.h"
#include <cassert>

SoundManager * SoundManager::soundManager = NULL;

SoundManager * SoundManager::getSoundManager() 
{
	if(soundManager == NULL)
		soundManager = new SoundManager();

	return soundManager;
}

SoundManager::SoundManager() 
: currentStream(NULL)
, oldStream(NULL)
, streamVolume(100)
, sampleVolume(100)
{
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	pathToData = dataLocatorManager->getPath("sound");

	// Try to initiate Bass, if not, initiate Bass without sound 
	if(!BASS_Init(1, 44100, BASS_DEVICE_3D, 0, NULL))
		BASS_Init(0, 44100, 0, 0, NULL);
	init();
};

void SoundManager::init()
{
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToXml = dataLocatorManager->getPath("xml");

	std::string xmlFile = pathToXml + "sounds.xml";
	bool ok = xmlParseFile(xmlFile);
	assert(ok);
}

void SoundManager::onStartElement (const std::string &elem, MKeyValue &atts) 
{
	if (elem == "ambient") 
	{
		std::string name = atts["name"];
		std::string file = atts["file"];
		std::string filePath = pathToData + "ambient/" + file;

		HSTREAM hStream = BASS_StreamCreateFile(FALSE, filePath.c_str(), 0, 0, BASS_SAMPLE_LOOP);
		// TODO Set current stream by a parameter by xml
		if( hStream == 0 ) threatError();
		else streams[name] = hStream;
	}
	else if (elem == "sample") 
	{
		std::string name = atts["name"];
		std::string file = atts["file"];
		std::string filePath = pathToData + "samples/" + file;

		HSAMPLE hSample = BASS_SampleLoad(FALSE, filePath.c_str(), 0, 0, 5, 0);
		if(hSample == 0)
			threatError();
		else
			samples[name] = hSample;
	}
}

void SoundManager::playStream(const char *streamName)
{
	if( streamVolume >0 ) 
	{
		StreamsMap::iterator it = streams.find(streamName);
		if(it == streams.end())
			return;
		
		if( currentStream != NULL )	// something is already played
		{
			oldStream = currentStream;
			currentStream = &(it->second);

			BOOL ok;
			ok = BASS_ChannelSlideAttribute(*oldStream, BASS_ATTRIB_VOL, 0.0f, FADE_TIME);
			if( ok != TRUE )
				threatError();

			// correct new stream volume to indicated one
			BASS_ChannelSetAttribute(*currentStream, BASS_ATTRIB_VOL, toFloat(streamVolume));

			ok = BASS_ChannelSlideAttribute(*currentStream, BASS_ATTRIB_VOL, toFloat(streamVolume), FADE_TIME);
			if( ok != TRUE )
				threatError();
		}
		else						// nothing else is played
		{
			currentStream = &(it->second);	

			// correct new stream volume to indicated one
			BASS_ChannelSetAttribute(*currentStream, BASS_ATTRIB_VOL, toFloat(streamVolume));

			BOOL ok = BASS_ChannelSlideAttribute(*currentStream, BASS_ATTRIB_VOL, toFloat(streamVolume), FADE_TIME);
			if( ok != TRUE )
				threatError();
		}

		BASS_ChannelPlay(*currentStream, TRUE);
	}
}

void SoundManager::stopCurrentStream()
{
	if(currentStream) {
		BOOL ok = BASS_ChannelStop(*currentStream);
		if( ok != TRUE )
			threatError();
		currentStream = NULL;
	}
}

void SoundManager::playSample(const char *sampleName)
{
	if( sampleVolume >0 ) 
	{
		SamplesMap::iterator it = samples.find(sampleName);
		if(it == samples.end())
			return;

		HCHANNEL channel = BASS_SampleGetChannel( it->second, FALSE );
		if( channel != NULL )
		{
			BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, toFloat(sampleVolume));
			BASS_ChannelPlay(channel, FALSE);
		}
		else if ( BASS_ErrorGetCode() != BASS_ERROR_NOCHAN )
			threatError();
	}
}

HCHANNEL SoundManager::playStopableSample(const char *sampleName)
{
	if( sampleVolume >0 ) 
	{
		SamplesMap::iterator it = samples.find(sampleName);
		if(it == samples.end())
			return NULL;

		HCHANNEL channel = BASS_SampleGetChannel( it->second, FALSE );
		if( channel != NULL )
		{
			BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, toFloat(sampleVolume));
			BASS_ChannelPlay(channel, FALSE);
			return channel;
		}
		else if ( BASS_ErrorGetCode() != BASS_ERROR_NOCHAN )
			threatError();
	}
	return NULL;
}

void SoundManager::stopSample(DWORD sampleHandler)
{
	if( sampleHandler != NULL )
	{
		BASS_ChannelStop(sampleHandler);
	}
}

void SoundManager::upStreamVolume()
{
	if( streamVolume < 100 ) 
	{
		streamVolume += 5;
		streamVolume=Global::clamp(streamVolume, 0, 100);

		if( currentStream != NULL ) 
		{
			BASS_ChannelSetAttribute(*currentStream, BASS_ATTRIB_VOL, toFloat(streamVolume));
		}

		if( oldStream != NULL ) 
		{
			BASS_ChannelSetAttribute(*oldStream, BASS_ATTRIB_VOL, toFloat(streamVolume));
		}
	}
}

void SoundManager::downStreamVolume()
{
	if( streamVolume > 0 ) 
	{
		streamVolume -= 5;
		streamVolume = Global::clamp(streamVolume, 0, 100);

		if( currentStream != NULL ) 
		{
			BASS_ChannelSetAttribute(*currentStream, BASS_ATTRIB_VOL, toFloat(streamVolume));
		}

		if( oldStream != NULL ) 
		{
			BASS_ChannelSetAttribute(*oldStream, BASS_ATTRIB_VOL, toFloat(streamVolume));
		}
	}
}

void SoundManager::upSampleVolume()
{
	if( sampleVolume < 100 ) 
	{
		sampleVolume += 5;
		sampleVolume = Global::clamp( sampleVolume, 0, 100 );
	}
}

void SoundManager::downSampleVolume()
{
	if( sampleVolume > 0 ) 
	{
		sampleVolume -= 5;
		sampleVolume = Global::clamp( sampleVolume, 0, 100 );
	}
}

float SoundManager::toFloat(int volume)
{
	return static_cast<float>(volume) / 100.0f;
}

void SoundManager::threatError()
{
	int errCode = BASS_ErrorGetCode();
	if( errCode == BASS_OK )
		return;

	switch(errCode)
	{
	case BASS_ERROR_MEM:	// memory error
		OutputDebugString("BASS ERROR: memory error\n");
		break;
	case BASS_ERROR_FILEOPEN:	// can't open the file
		OutputDebugString("BASS ERROR: can't open the file\n");
		break;
	case BASS_ERROR_DRIVER:	// can't find a free/valid driver
		OutputDebugString("BASS ERROR: can't find a free/valid driver\n");
		break;
	case BASS_ERROR_BUFLOST:	// the sample buffer was lost
		OutputDebugString("BASS ERROR: the sample buffer was lost\n");
		break;
	case BASS_ERROR_HANDLE:	// invalid handle
		OutputDebugString("BASS ERROR: invalid handle\n");
		break;
	case BASS_ERROR_FORMAT:	// unsupported sample format
		OutputDebugString("BASS ERROR: unsupported sample format\n");
		break;
	case BASS_ERROR_POSITION:	// invalid playback position
		OutputDebugString("BASS ERROR: invalid playback position\n");
		break;
	case BASS_ERROR_INIT:	// BASS_Init has not been successfully called
		OutputDebugString("BASS ERROR: BASS_Init has not been successfully called\n");
		break;
	case BASS_ERROR_START:	// BASS_Start has not been successfully called
		OutputDebugString("BASS ERROR: BASS_Start has not been successfully called\n");
		break;
	case BASS_ERROR_SSL:	// SSL/HTTPS support isn't available
		OutputDebugString("BASS ERROR: SSL/HTTPS support isn't available\n");
		break;
	case BASS_ERROR_ALREADY:	// already initialized/paused/whatever
		OutputDebugString("BASS ERROR: already initialized/paused/whatever\n");
		break;
	case BASS_ERROR_NOTAUDIO:	// file does not contain audio
		OutputDebugString("BASS ERROR: file does not contain audio\n");
		break;
	case BASS_ERROR_NOCHAN:	// can't get a free channel
		OutputDebugString("BASS ERROR: can't get a free channel\n");
		break;
	case BASS_ERROR_ILLTYPE:	// an illegal type was specified
		OutputDebugString("BASS ERROR: an illegal type was specified\n");
		break;
	case BASS_ERROR_ILLPARAM:	// an illegal parameter was specified
		OutputDebugString("BASS ERROR: an illegal parameter was specified\n");
		break;
	case BASS_ERROR_NO3D:	// no 3D support
		OutputDebugString("BASS ERROR: no 3D support\n");
		break;
	case BASS_ERROR_NOEAX:	// no EAX support
		OutputDebugString("BASS ERROR: no EAX support\n");
		break;
	case BASS_ERROR_DEVICE:	// illegal device number
		OutputDebugString("BASS ERROR: illegal device number\n");
		break;
	case BASS_ERROR_NOPLAY:	// not playing
		OutputDebugString("BASS ERROR: not playing\n");
		break;
	case BASS_ERROR_FREQ:	// illegal sample rate
		OutputDebugString("BASS ERROR: illegal sample rate\n");
		break;
	case BASS_ERROR_NOTFILE:	// the stream is not a file stream
		OutputDebugString("BASS ERROR: the stream is not a file stream\n");
		break;
	case BASS_ERROR_NOHW:	// no hardware voices available
		OutputDebugString("BASS ERROR: no hardware voices available\n");
		break;
	case BASS_ERROR_EMPTY:	// the MOD music has no sequence data
		OutputDebugString("BASS ERROR: the MOD music has no sequence data\n");
		break;
	case BASS_ERROR_NONET:	// no internet connection could be opened
		OutputDebugString("BASS ERROR: no internet connection could be opened\n");
		break;
	case BASS_ERROR_CREATE:	// couldn't create the file
		OutputDebugString("BASS ERROR: couldn't create the file\n");
		break;
	case BASS_ERROR_NOFX:	// effects are not available
		OutputDebugString("BASS ERROR: effects are not available\n");
		break;
	case BASS_ERROR_NOTAVAIL:	// requested data is not available
		OutputDebugString("BASS ERROR: requested data is not available\n");
		break;
	case BASS_ERROR_DECODE:	// the channel is a decoding channel
		OutputDebugString("BASS ERROR: the channel is a decoding channel\n");
		break;
	case BASS_ERROR_DX:	// a sufficient DirectX version is not installed
		OutputDebugString("BASS ERROR: a sufficient DirectX version is not installed\n");
		break;
	case BASS_ERROR_TIMEOUT:	// connection timedout
		OutputDebugString("BASS ERROR: connection timedout\n");
		break;
	case BASS_ERROR_FILEFORM:	// unsupported file format
		OutputDebugString("BASS ERROR: unsupported file format\n");
		break;
	case BASS_ERROR_SPEAKER:	// unavailable speaker
		OutputDebugString("BASS ERROR: unavailable speaker\n");
		break;
	case BASS_ERROR_VERSION:	// invalid BASS version (used by add-ons)
		OutputDebugString("BASS ERROR: invalid BASS version (used by add-ons)\n");
		break;
	case BASS_ERROR_CODEC:	// codec is not available/supported
		OutputDebugString("BASS ERROR: codec is not available/supported\n");
		break;
	case BASS_ERROR_ENDED:	// the channel/file has ended
		OutputDebugString("BASS ERROR: the channel/file has ended\n");
		break;
	case BASS_ERROR_BUSY:	// the device is busy
		OutputDebugString("BASS ERROR: the device is busyd\n");
		break;
	case BASS_ERROR_UNSTREAMABLE:	// unstreamable file
		OutputDebugString("BASS ERROR: unstreamable file\n");
		break;
	default:	// some other mystery error
		OutputDebugString("BASS ERROR: some other mystery error\n");
		break;
	}

	assert( !"Bass Error" );
}
