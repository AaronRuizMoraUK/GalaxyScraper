#ifndef PAD_BUTTONS_RENDER_H
#define PAD_BUTTONS_RENDER_H

#include "IStringRender.h"

/**
* This class make possible print XBOX 360 Buttons
* for the Options in main menu
*/

class PadButtonsRender : public IStringRender
{
protected:
	const static std::string textureName;
	const static std::string techniqueName;

	const static D3DXVECTOR2 letterTexture;

public:
	PadButtonsRender( );
	~PadButtonsRender( );

	void print( float x, float y, float letter_width, float letter_height, const char *msg, ... );

protected:
	const std::string & getTextureName() const { return textureName; };
	const std::string & getTechnicName() const { return techniqueName; };

private:
	bool obtainTextureCoodinatesOf( int number, D3DXVECTOR2 &outTextureCoordinates ) const;
};

#endif // PAD_BUTTONS_RENDER_H
