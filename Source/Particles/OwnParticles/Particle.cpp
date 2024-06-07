#include "Particle.h"

Particle::Particle() : time(0.0f), 
lifeTime(0.0f), 
position(0.0f, 0.0f, 0.0f), 
size(1.0f), 
speed(0.0f, 0.0f, 0.0f), 
acceleration(0.0f, 0.0f, 0.0f),
initialPhase(0.0f),
color(1.0f, 1.0f, 1.0f, 1.0f)
{
}

void Particle::update(float deltaTime)
{
	 time+=deltaTime;
}
