#ifndef GAME_H
#define GAME_H

#include <Windows.h>
#include "XMLParser/XMLParser.h"
#include "GameState/OptionsState.h"

/**
* Game application with windows gestion
*/

class Game : public CXMLParser
{

protected:
	// Delta Time
	float deltaTime;

	// Variable to calculate FPS
	static float numFrames;

	static bool isGameInitiated;

public:
	// Frames per second
	static float fps;

	// Config Options
	static Options configOptions;
	static Resolutions allowedResolutions;

private:
	// Game Speed
	static bool normalGameSpeed;
	static bool stopGameSpeed ;
	static bool verySlowGameSpeed;
	static bool slowGameSpeed;
	static bool fastGameSpeed;

public:
	Game();
	~Game();

	void onStartElement (const std::string &elem, MKeyValue &atts);
	//void onEndElement (const std::string &elem);

	bool initiate();
	void run();
	void clean();

	static void reloadShaders();

private:
	// Window class and handler
	WNDCLASSEX wc;
	HWND hWnd;
	bool isClassRegistered;

	bool createWindow();
	bool initDevice();
	bool initGlobals();

	bool loadEverything();
	void destroyEverything();

	friend LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
};

#endif // GAME_H
