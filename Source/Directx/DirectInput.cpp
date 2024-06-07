#include "DirectInput.h"

HRESULT createDeviceInputCreator(DeviceInputCreator &deviceInputCreator) {
	HRESULT hr;

	if( FAILED ( hr = DirectInput8Create( GetModuleHandle(NULL), 
		DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&deviceInputCreator, NULL ) ) )
		return hr;

	return S_OK;
}
