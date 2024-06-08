#include "TextureFactory.h"
#include "Global/GlobalVariables.h"
#include "Managers/DataLocatorManager.h"

#ifndef D3DX9_SUPPORTED
#include "DDSTextureLoader9.h"
#endif

TextureFactory * TextureFactory::textureFactory = NULL;

TextureFactory * TextureFactory::getTextureFactory() {
	if(textureFactory == NULL)
		textureFactory = new TextureFactory();

	return textureFactory;
}

Texture TextureFactory::createTexture(const std::string &filename) {
	TexturesMap::const_iterator it = texturesMap.find(filename);

	if(it != texturesMap.end())
		return it->second;
	else {
		DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
		const std::string &pathToShader = dataLocatorManager->getPath("texture");
		
		std::string pathFilename = pathToShader + filename;

		Texture texture = NULL;
		assert(Global::device || !"Device not created");
#ifdef D3DX9_SUPPORTED
		/*
		// Use D3DX to create a texture from a file based image
		if( FAILED( D3DXCreateTextureFromFile( Global::device,
			pathFilename.c_str(), &texture ) ) )
		{
			//char msg[100];
			//strcpy_s(msg, sizeof(msg), "Could not find texture ");
			//strcat_s(msg, sizeof(msg), pathFilename.c_str());
			//MessageBox(NULL, msg, "Galaxy Scraper.exe", MB_OK);
			assert( !"Could not find texture in TextureFactory" );

			return NULL;
		}
		*/

		if( FAILED( D3DXCreateTextureFromFileEx(Global::device, pathFilename.c_str(),
			D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN,
			D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &texture) ) )
		{
			//char msg[100];
			//strcpy_s(msg, sizeof(msg), "Could not find texture ");
			//strcat_s(msg, sizeof(msg), pathFilename.c_str());
			//MessageBox(NULL, msg, "Galaxy Scraper.exe", MB_OK);
			assert( !"Could not find texture in TextureFactory" );

			return NULL;
		}
#else
		const std::wstring pathFilenameW(pathFilename.begin(), pathFilename.end());

		if ( FAILED( DirectX::CreateDDSTextureFromFileEx(Global::device, pathFilenameW.c_str(),
			 0, D3DPOOL_DEFAULT, true, &texture)))
		{
			//char msg[100];
			//strcpy_s(msg, sizeof(msg), "Could not find texture ");
			//strcat_s(msg, sizeof(msg), pathFilename.c_str());
			//MessageBox(NULL, msg, "Galaxy Scraper.exe", MB_OK);
			assert(!"Could not find texture in TextureFactory");

			return NULL;
		}
#endif

		texturesMap[filename]=texture;
		return texture;
	}
}
