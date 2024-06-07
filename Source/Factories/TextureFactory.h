#ifndef TEXTURE_FACTORY_H
#define TEXTURE_FACTORY_H

#include <map>
#include <string>
#include "Directx/Directx.h"
#include "Util/MapComparers/MapComparers.h"

typedef std::map<std::string, Texture, LessString> TexturesMap;

/**
* Factory of textures.
*
* This is a singleton.
*/

class TextureFactory
{
protected:
	TexturesMap texturesMap;

public:
	~TextureFactory() {
		OutputDebugString("Destroying TextureFactory...\n");

		// Destroy texture map
		TexturesMap::const_iterator it = texturesMap.begin();
		while(it !=  texturesMap.end()) {	
			Texture texture = (it->second);
			if(texture)
				texture->Release(), texture=NULL;
			++it;
		}
		texturesMap.clear();
	};

	static TextureFactory * getTextureFactory();

	Texture createTexture(const std::string &filename);

private:
	static TextureFactory * textureFactory;
	TextureFactory() {};
};

#endif //TEXTURE_FACTORY_H
