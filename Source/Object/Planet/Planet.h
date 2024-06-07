#ifndef PLANET_H
#define PLANET_H

#include <string>
#include "Object/Object.h"

/**
* A planet in scene. It is an Object with radius, name and heigthmap.
*/

class Planet : public Object
{
protected:
	float radius;
	std::string name;

	short heightMapHeight;
	short heightMapWidth;
	float **heightMap;

public:
	Planet();
	~Planet();

	const D3DXVECTOR3 & getCenter() const {return getPosition();};
	float getRadius() const {return radius;};
	const std::string & getName() const {return name;};

	void setCenter(D3DXVECTOR3 &pCenter) { setPosition(pCenter); };
	void setRadius(float pRadius) {radius=pRadius;};
	void setName(const std::string &pName) {name=pName;};
	
	void getPolarAngles(const D3DXVECTOR3 &point, float &alphaOut, float &deltaOut) const;
	float getHeight(float alpha, float delta) const;
	bool constructHeightMap(const std::string &filename);
};

#endif //PLANET_H
