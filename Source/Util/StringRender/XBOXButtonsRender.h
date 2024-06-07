#ifndef XBOX_BUTTONS_RENDER_H
#define XBOX_BUTTONS_RENDER_H

#include "IStringRender.h"

/**
* This class make possible print XBOX 360 Buttons
* for the Options in main menu
*/

class XBOXButtonsRender : public IStringRender
{
protected:
	const static std::string textureName;
	const static std::string techniqueName;

	const static D3DXVECTOR2 letterTexture;

public:
	XBOXButtonsRender( );
	~XBOXButtonsRender( );

	void print( float x, float y, float letter_width, float letter_height, const char *msg, ... );
	static const std::string & getKeyString( const std::string &key );
	static const std::string & getKeyStringFromVK( WORD virtualKey );

protected:
	const std::string & getTextureName() const { return textureName; };
	const std::string & getTechnicName() const { return techniqueName; };

private:
	bool obtainTextureCoodinatesOf( const char letter, D3DXVECTOR2 &outTextureCoordinates ) const;
};

#endif // XBOX_BUTTONS_RENDER_H
