//-------------------------------------------------------------------------------------------------------
// VST Plug-Ins SDK
//-
// Example SurroundDelay (VST 2.3)
// Simple Surround Delay plugin with Editor using VSTGUI
//-
// © 2003, Steinberg Media Technologies, All Rights Reserved
//-------------------------------------------------------------------------------------------------------

#ifndef __surrounddelay
#include "surrounddelay.hpp"	// *change*
#endif

#define DEBUG 0

bool oome = false;

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
	if (!audioMaster (0, audioMasterVersion, 0, 0, 0, 0))
	{
		#if DEBUG
		DebugStr("\pVstPlugIn: old vst version");
		#endif
		return 0;  // old version
	}

	// Create the AudioEffect
	SurroundDelay* effect = new SurroundDelay (audioMaster);	// *change*
	if (!effect)
		return 0;
	
	// Check if no problem in constructor of ADelayEdit
	if (oome)
	{
		delete effect;
		return 0;
	}
	return effect->getAeffect ();
}

#if MAC
#pragma export off
#endif

//------------------------------------------------------------------------
#if WIN32
#include <windows.h>
void* hInstance;
BOOL WINAPI DllMain (HINSTANCE hInst, DWORD dwReason, LPVOID lpvReserved)
{
	hInstance = hInst;
	return 1;
}
#endif

