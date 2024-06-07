#ifndef TIMEOUT_H
#define TIMEOUT_H

#include <Windows.h>

/**
* Time out utilitiy
*/

class Timeout
{

protected:
	float timeToElapse;
	float elapsedTime;

public:
	Timeout() : timeToElapse(0.0f), elapsedTime(0.0f) {};
	Timeout(float pTimeToElapse) { init(pTimeToElapse); };
	~Timeout()
	{
		OutputDebugString("Destroying Timeout...\n");
	};

	void init(float pTimeToElapse);
	void update(float deltaTime=(1.0f/60.0f));
	bool isFinished() const;
	void restart();

	float getElapsedTime() const { return elapsedTime; };
	float getTotalTime() const { return timeToElapse; };
};

#endif // TIMEOUT_H
