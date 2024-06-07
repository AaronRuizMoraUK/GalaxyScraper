#ifndef UI_NUMBER_RENDER_H
#define UI_NUMBER_RENDER_H

#include "IStringRender.h"

/**
* This class make possible print numbers
* for the User Interface in planet gamepley
*/

class UINumberRender : public IStringRender
{
protected:
	const static std::string textureName;
	const static std::string techniqueName;

	const static D3DXVECTOR2 letterTexture;

public:
	UINumberRender( );
	~UINumberRender( );

	void print( float x, float y, float letter_width, float letter_height, const char *msg, ... );

protected:
	const std::string & getTextureName() const { return textureName; };
	const std::string & getTechnicName() const { return techniqueName; };

private:
	bool obtainTextureCoodinatesOf( const char letter, D3DXVECTOR2 &outTextureCoordinates ) const;
};

#endif // UI_NUMBER_RENDER_H
