#include "Timeout.h"

void Timeout::init(float pTimeToElapse)
{
	timeToElapse = pTimeToElapse;
	elapsedTime = 0.0f;
}

void Timeout::update(float deltaTime)
{
	elapsedTime += deltaTime;
}

bool Timeout::isFinished() const
{
	return ( elapsedTime >= timeToElapse );
}

void Timeout::restart()
{
	elapsedTime = 0.0f;
}
