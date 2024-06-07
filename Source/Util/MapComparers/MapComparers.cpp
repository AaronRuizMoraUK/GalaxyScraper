#include "MapComparers.h"
#include "GameState/OptionsState.h"

bool CompareResolution::operator()(const Resolution &r1, const Resolution &r2) const {

	if( r1.width != r2.width )
		return r1.width < r2.width;
	else if( r1.height != r2.height )
		return r1.height < r2.height;
	else if(r1.rate != r2.rate )
		return r1.rate < r2.rate;
	else
		return false;

	/*
	// Compare r1 and r2 and return true if r1 < r2
	if( r1.width < r2.width )
		return true;
	else if( r1.width > r2.width )
		return false;
	else if ( r1.height < r2.height )
		return true;
	else if ( r1.height > r2.height )
		return false;
	else if ( r1.rate < r2.rate )
		return true;
	else if ( r1.rate > r2.rate )
		return false;

	return false;
	*/
}
