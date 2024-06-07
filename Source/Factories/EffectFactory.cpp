#include "EffectFactory.h"
#include "Global/GlobalVariables.h"
#include "Managers/DataLocatorManager.h"
#include "Game/Game.h"

EffectFactory * EffectFactory::effectFactory = NULL;

EffectFactory * EffectFactory::getEffectFactory() {
	if(effectFactory == NULL)
		effectFactory = new EffectFactory();

	return effectFactory;
}

Effect EffectFactory::createEffect(const std::string &filename) 
{
	EffectsMap::const_iterator it = effectsMap.find(filename);

	if(it != effectsMap.end())
		return it->second;
	else {
		DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
		const std::string &pathToShader = dataLocatorManager->getPath("shader");
		
		std::string realFilename = filename;
		if( Game::configOptions.quality == "low" ||
			Game::configOptions.quality == "minimum" ) {
			// Change name of shader to load
			std::string::size_type pos = realFilename.find(".fx");
			realFilename.insert(pos, "_"+Game::configOptions.quality);
		}

		std::string pathFilename = pathToShader + realFilename;

		Effect effect = NULL;
		if( FAILED( loadEffect(pathFilename, effect) ) )
		{
			char msg[100];
			strcpy_s(msg, sizeof(msg), "Could not load effect ");
			strcat_s(msg, sizeof(msg), pathFilename.c_str());
			MessageBox(NULL, msg, "Galaxy Scraper.exe", MB_OK);
			assert( !"Could not load effect in EffectFactory" );

			return NULL;
		}

		effectsMap[filename]=effect;
		updatedEffectsMap[filename]=true; // Effect is updated
		return effect;
	}
}


HRESULT EffectFactory::loadEffect(const std::string &filename, Effect &effect) {
	if( effect )
		effect->Release( ), effect=NULL;

	HRESULT hr;
	LPD3DXBUFFER errorBuffer = NULL;
	hr = D3DXCreateEffectFromFile( 
		Global::device
		, filename.c_str()
		, NULL
		, NULL
		, (Game::configOptions.quality == "minimum") ? D3DXSHADER_USE_LEGACY_D3DX9_31_DLL : 0
		, NULL
		, &effect
		, &errorBuffer
		);
	if( FAILED( hr ) ) {
		const char *errMsg = "Unknow error compiling fx";
		if( errorBuffer ) 
			errMsg = (const char *) errorBuffer->GetBufferPointer( );
		OutputDebugString( "Error compiling FX file\n" );
		OutputDebugString( filename.c_str() );
		OutputDebugString( "\n" );
		OutputDebugString( errMsg );
		OutputDebugString( "\n" );
		MessageBox(NULL, errMsg, filename.c_str(), MB_OK);

		assert( !"Error compiling FX file in EffectFactory" );
		return hr;
	}

	if( errorBuffer )
		errorBuffer->Release( ), errorBuffer=NULL;

	return hr;
}

void EffectFactory::outdateUpdatedEffectsMap() {
	UpdatedEffectsMap::iterator it = updatedEffectsMap.begin();

	while(it != updatedEffectsMap.end()) {
		it->second = false;
		++it;
	}
}

Effect EffectFactory::reloadEffect(const std::string &filename) 
{
	EffectsMap::iterator itFX = effectsMap.find(filename);
	UpdatedEffectsMap::iterator itUpFX = updatedEffectsMap.find(filename);

	if( itFX != effectsMap.end() && itUpFX != updatedEffectsMap.end() ) {
		if(itUpFX->second == true)
			return itFX->second;
		else {
			DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
			const std::string &pathToShader = dataLocatorManager->getPath("shader");
		
			std::string realFilename = itFX->first;
			if( Game::configOptions.quality == "low" ||
				Game::configOptions.quality == "minimum" ) {
				// Change name of shader to load
				std::string::size_type pos = realFilename.find(".fx");
				realFilename.insert(pos, "_"+Game::configOptions.quality);
			}
			std::string pathFilename = pathToShader + realFilename;
			
			Effect effect = itFX->second;
			// Use D3DX to create a texture from a file based image
			if( FAILED( loadEffect(pathFilename, effect) ) )
			{
				char msg[100];
				strcpy_s(msg, sizeof(msg), "Could not load effect ");
				strcat_s(msg, sizeof(msg), pathFilename.c_str());
				MessageBox(NULL, msg, "Galaxy Scraper.exe", MB_OK);
				assert( !"Could not load effect in EffectFactory" );

				return NULL;
			}

			itFX->second=effect;
			itUpFX->second=true; // Effect is updated
			return itFX->second;
		}
	}
	else
		return NULL;
}
