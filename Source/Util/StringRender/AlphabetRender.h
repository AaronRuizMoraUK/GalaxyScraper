#ifndef ALPHABET_RENDER_H
#define ALPHABET_RENDER_H

#include "IStringRender.h"

/**
* This class make possible print with a
* a texture with alphabet
*/

class AlphabetRender : public IStringRender
{
protected:
	const static std::string textureName;
	const static std::string techniqueName;

	const static D3DXVECTOR2 letterTexture;

public:
	AlphabetRender( );
	~AlphabetRender( );

	void print( float x, float y, float letter_width, float letter_height, const char *msg, ... );

protected:
	const std::string & getTextureName() const { return textureName; };
	const std::string & getTechnicName() const { return techniqueName; };

private:
	bool obtainTextureCoodinatesOf( const char letter, D3DXVECTOR2 &outTextureCoordinates ) const;
};

#endif // ALPHABET_RENDER_H
