//-------------------------------------------------------------------------------------------------------
// VST Plug-Ins SDK
// -
// Example Offline (VST 2.3)
// -
// © 2003, Steinberg Media Technologies, All Rights Reserved
//-------------------------------------------------------------------------------------------------------

#include <stddef.h>
#include "OfflineEffect.hpp"

//-----------------------------------------------------------------------------

#if MAC
#pragma export on
#endif

//------------------------------------------------------------------------
// Prototype of the export function main
//------------------------------------------------------------------------
#if BEOS
#define main main_plugin
extern "C" __declspec(dllexport) AEffect *main_plugin (audioMasterCallback audioMaster);

#elif MACX
#define main main_macho
extern "C" AEffect *main_macho (audioMasterCallback audioMaster);

#else
AEffect *main (audioMasterCallback audioMaster);
#endif

//------------------------------------------------------------------------
AEffect *main (audioMasterCallback audioMaster)
{
	// Get VST Version
	if (!audioMaster(0, audioMasterVersion, 0, 0, 0, 0))
		return 0;  // old version

	AudioEffect* effect = new OfflineEffect (audioMaster);
	return effect->getAeffect ();
}

#if MAC
#pragma export off
#endif

//-----------------------------------------------------------------------------

#if WIN32
#include <windows.h>
HINSTANCE hInstance;
BOOL WINAPI DllMain (HINSTANCE hInst, DWORD dwReason, LPVOID lpvReserved)
{
	hInstance = hInst;
	return 1;
}
#endif
