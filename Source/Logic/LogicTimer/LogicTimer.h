#ifndef LOGIC_TIMER_H
#define LOGIC_TIMER_H

#include <Windows.h>
#include <string>

/**
* Logic Timer
*/

class LogicTimer
{
protected:
	float timeout;
	float currentTime;

	std::string scriptName;

	//bool active;
	//unsigned long birth;

public:
	LogicTimer(const std::string &pScriptName, float pTimeout);
	~LogicTimer() {
		OutputDebugString("Destroying LogicTimer...\n");
	};

	float getCurrentTimeLeft() const { return (timeout - currentTime); };
	const std::string &getScriptName() const { return scriptName; };

	bool recalc( float deltaTime=(1.0f/60.0f) );

	//unsigned long getBirth() const { return birth; };
	//float getTimeout() const { return timeout; };
	//float getCurrentTime() const { return currentTime; };
	//bool isActive() const { return active; };

	//void setBirth(unsigned long pBirth) { birth = pBirth; };
	//void setTimeout(float pTimeout) { timeout = pTimeout; };
	//void setScriptName(const std::string &pScriptName) { scriptName = pScriptName; };
	//void activate() { active = true; };
	//void desactivate() { active = false; };
};

#endif //LOGIC_TIMER_H
