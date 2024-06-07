#ifndef OPTIONS_STATE_H
#define OPTIONS_STATE_H

#include <Windows.h>
#include "GameState/IGameState.h"
#include "Object/Object.h"
#include "Util/StringRender/AlphabetRender.h"
#include "Util/StringRender/XBOXButtonsRender.h"
#include "Util/StringRender/PadButtonsRender.h"
#include "Expat/XMLParser.h"
#include "Util/MapComparers/MapComparers.h"
#include "Util/Fade/Fade.h"
#include <set>
#include <sstream>

/**
* Menu options state
*
* Resolution: Saved in xml when you exit options, applied when you restart the game
* Quality: Saved in xml and applied when you exit options
* Music: Saved in xml when you exit options, applied directly when you changes its value
* SFX: Saved in xml when you exit options, applied directly when you changes its value
* Button: Saved in xml and applied directly when you changes its value
*/

typedef struct ResolutionStruct {
	int width;
	int height;
	int rate;

	ResolutionStruct()
	: width(800)
	, height(600)
	, rate(60)
	{
	};

	ResolutionStruct(int pWidth, int pHeight, int pRate)
	: width(pWidth)
	, height(pHeight)
	, rate(pRate)
	{
	};

	bool operator==( const ResolutionStruct &othr ) {
		if( width == othr.width &&
			height == othr.height &&
			rate == othr.rate )
			return true;
		else
			return false;
	}

	bool operator!=( const ResolutionStruct &othr ) {
		return !(*this == othr);
	}

} Resolution;

typedef std::set<Resolution, CompareResolution> Resolutions;

typedef struct optionsStruct {
	bool fullScreen;
	bool vSync;
	Resolution resolution;
	float aspectRatio;

	std::string quality;
	int music;
	int sfx;

	optionsStruct()
	: fullScreen(false)
	, vSync(false)
	, resolution()
	, aspectRatio(4.0f/3.0f)
	, quality("high")
	, music(100)
	, sfx(100)
	{
	};

	bool operator==( const optionsStruct &othr ) {
		if( resolution == othr.resolution &&
			quality == othr.quality &&
			music == othr.music &&
			sfx == othr.sfx )
			return true;
		else
			return false;
	}

	bool operator!=( const optionsStruct &othr ) {
		return !(*this == othr);
	}

} Options;

class OptionsState : public IGameState, public CXMLParser
{
protected:
	static std::string name;

	bool enter_options;
	bool exit_options;
	bool warning_changes;

	Object arrow;
	Object selectionQuad;
	AlphabetRender alphabetRender;
	XBOXButtonsRender xboxButtonsRender;
	PadButtonsRender padButtonsRender;

	// Buttons Fade
	Fade pressButtonFade;
	bool pressAButtonNow;

	// Flowers pictures
	Object flowerUp;
	Object flowerDown;

	// Flowers Changes pictures
	Object flowerChangesUp;
	Object flowerChangesDown;

	// Positions of the arrow
	const static D3DXVECTOR3 onExitPosition;
	static D3DXVECTOR3 onResolutionPosition;
	static D3DXVECTOR3 onQualityPosition;
	static D3DXVECTOR3 onMusicPosition;
	static D3DXVECTOR3 onSFXPosition;
	static D3DXVECTOR3 onButtonPosition;

	// Option position
	typedef enum OptionPositionEnum {EXIT, RESOLUTION, QUALITY, MUSIC, SFX, BUTTONS, NUM_OPTION_POSITIONS} OptionPosition;
	OptionPosition optionPosition;
	Resolutions::const_iterator resolutionsIt;

	// Quality
	typedef enum QualityEnum{MINIMUM, LOW, MEDIUM, HIGH, NUM_QUALITIES} Quality;
	Quality quality;

	// Selection in Buttons option position
	typedef enum ButtonsOptionPositionEnum {JUMP_KEY, JUMP_PAD, JUMP_360,
										    KICK_KEY, KICK_PAD, KICK_360,  
										    WALK_KEY, WALK_PAD, WALK_360,
											PAUSE_KEY, PAUSE_PAD, PAUSE_360,
										    NUM_BUTTONS_OPTION_POSITIONS} ButtonsOptionPosition;
	ButtonsOptionPosition buttonsOptionPosition;

	bool isChangingSetting; // This bool means if player is trying to change any setting

	// This is used to check what options have been changed by you
	Options currentOptions;

private:
	// Used to write in xml
	std::ostringstream outputStringStream;

public:
	OptionsState();
	~OptionsState()
	{
		OutputDebugString("Destroying OptionsState...\n");
	};

	void update( float deltaTime=(1.0f/60.0f) );
	void onLoad( );
	void onUnload( );
	const std::string & getName( ) const { return name; };

	void onStartElement (const std::string &elem, MKeyValue &atts);
	//void onEndElement (const std::string &elem);

protected:
	void loadObjects( );
	void reloadShader( );

	void preRender( );
	void renderWorld3D( );
	void renderWorld2D( );
	void renderPostProcessed2D( );
	void renderGameUI( );
	void postRender( );

	void renderFrame( );

private:
	void updateSetting( );
	void updateButtonsSettings( );

	void updateResolution( );
	void updateQuality( );
	void updateMusic( );
	void updateSFX( );
	void udpatePositionInButtons( );

	void applyChanges( );

	void changeQuality( );

	const std::string &getQualityString( Quality pQuality ) const;
	Quality getQuality( const std::string &pQuality ) const;

	void printfKeyKeyboard( float y, const char *msg, ... );
};

#endif // OPTIONS_STATE_H
