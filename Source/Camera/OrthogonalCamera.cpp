#include "OrthogonalCamera.h"

/**
* Default constructor. Initiates parameters needed.
*/
OrthogonalCamera::OrthogonalCamera () 
: ICamera()
{
	D3DXMatrixOrthoOffCenterRH(&matProj, -1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
}
