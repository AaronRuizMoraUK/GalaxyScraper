#ifndef TIMEOUT_NUMBER_RENDER_H
#define TIMEOUT_NUMBER_RENDER_H

#include "IStringRender.h"

/**
* This class make possible print numbers
* for the time out in planet gameplay
*/

class TimeOutNumberRender : public IStringRender
{
protected:
	const static std::string textureName;
	const static std::string techniqueName;

	const static D3DXVECTOR2 letterTexture;

private:
	std::string lastLetter;
	float show_time;
	float last_global_time;

public:
	TimeOutNumberRender( );
	~TimeOutNumberRender( );

	void print( float x, float y, float letter_width, float letter_height, const char *msg, ... );
	void printClosing( float x, float y, float letter_width, float letter_height, const char *msg, ... );

protected:
	const std::string & getTextureName() const { return textureName; };
	const std::string & getTechnicName() const { return techniqueName; };

private:
	bool obtainTextureCoodinatesOf( const char letter, D3DXVECTOR2 &outTextureCoordinates ) const;
};

#endif // TIMEOUT_NUMBER_RENDER_H
