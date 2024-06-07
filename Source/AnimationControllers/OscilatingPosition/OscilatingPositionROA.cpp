#include "OscilatingPositionROA.h"

std::string OscilatingPositionROA::controllerType = "oscilating_position_roa";

OscilatingPositionROA::OscilatingPositionROA( const D3DXVECTOR3 &pInitialPosition,
					  const D3DXQUATERNION &pInitialRotation, const D3DXVECTOR3 &pDirection, 
					  float pAmplitude, float pFrequency )
					  : IAnimationController()
					  , currentTime(0)
					  , position(pInitialPosition)
					  , rotation(pInitialRotation)
					  , initialPosition(pInitialPosition)
					  , direction(pDirection)
					  , amplitude(pAmplitude)
					  , frequency(pFrequency)
{
}

void OscilatingPositionROA::update(AnimatedObject &object, float deltaTime )
{
	currentTime += deltaTime;
	float nextStepVariation = amplitude*sin( frequency*currentTime );

	position = initialPosition + nextStepVariation*direction;
	object.setPosition(position); // TODO
}
