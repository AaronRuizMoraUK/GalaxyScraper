#ifndef EFFECT_FACTORY_H
#define EFFECT_FACTORY_H

#include <map>
#include <string>
#include "Directx/Directx.h"
#include "Util/MapComparers/MapComparers.h"

typedef std::map<std::string, Effect, LessString> EffectsMap;
typedef std::map<std::string, bool, LessString> UpdatedEffectsMap;

/**
* Factory of effects (shaders).
*
* This is a singleton.
*/

class EffectFactory
{
protected:
	EffectsMap effectsMap;
	UpdatedEffectsMap updatedEffectsMap;

public:
	~EffectFactory(void) {
		OutputDebugString("Destroying EffectFactory...\n");

		// Destroy effect map
		EffectsMap::const_iterator it = effectsMap.begin();
		while(it !=  effectsMap.end()) {	
			Effect effect = (it->second);
			if(effect)
				effect->Release(), effect=NULL;
			++it;
		}
		effectsMap.clear();
	};

	static EffectFactory * getEffectFactory();

	Effect createEffect(const std::string &filename);

	void outdateUpdatedEffectsMap();
	Effect reloadEffect(const std::string &filename);

private:
	static EffectFactory * effectFactory;
	EffectFactory() {};

	HRESULT loadEffect(const std::string &filename, Effect &effect);
};

#endif //EFFECT_FACTORY_H
