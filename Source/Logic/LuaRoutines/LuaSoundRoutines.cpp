#include "LuaSoundRoutines.h"
#include <Windows.h>
#include "Managers/SoundManager.h"
#include "Global/GlobalVariables.h"
#include <string>

namespace LuaSoundRoutines {

	// LUA ROUTINES

	// play_ambient
	// input:	std::string ambientName
	static int play_ambient(lua_State *ls)
	{
		int num = lua_gettop(ls);

		if( num == 1 ) {
			std::string ambientName = lua_tostring(ls,1);
			
			SoundManager *soundManager = SoundManager::getSoundManager();
			soundManager->playStream(ambientName.c_str());
		}
		else {
			assert( !"Number of parameters incorrect in play_ambient in LuaSoundRoutines" );
		}
		
		return 0;
	}

	static int play_sample(lua_State *ls)
	{
		int num = lua_gettop(ls);

		if( num == 1 ) {
			std::string sampleName = lua_tostring(ls,1);
			
			if(Global::samplesEnabled)
			{
				SoundManager *soundManager = SoundManager::getSoundManager();
				soundManager->playSample(sampleName.c_str());
			}
		}
		else {
			assert( !"Number of parameters incorrect in play_sample in LuaSoundRoutines" );
		}

		return 0;
	}

	static int stop_ambient(lua_State *ls)
	{		
		SoundManager *soundManager = SoundManager::getSoundManager();
		soundManager->stopCurrentStream();

		return 0;
	}

	static int enable_samples(lua_State *ls)
	{
		int num = lua_gettop(ls);

		if(num == 1)
			Global::samplesEnabled = lua_toboolean(ls, 1) ? true : false;
		else {
			assert( !"Number of parameters incorrect in enable_samples in LuaSoundRoutines" );
		}

		return 0;
	}

	// LUA REGISTER FUNCTION
	void registerLuaSoundRoutines(lua_State *ls) 
	{
		lua_register(ls, "play_ambient", &play_ambient);
		lua_register(ls, "play_sample", &play_sample);
		lua_register(ls, "stop_ambient", &stop_ambient);
		lua_register(ls, "enable_samples", &enable_samples);
	}
}
