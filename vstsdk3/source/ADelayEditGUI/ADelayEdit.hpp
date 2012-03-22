//-------------------------------------------------------------------------------------------------------
// VST Plug-Ins SDK
//-
// Example ADelay (VST 2.0)
// Simple Delay plugin with Editor using VSTGUI (Mono->Stereo)
//-
// © 2003, Steinberg Media Technologies, All Rights Reserved
//-------------------------------------------------------------------------------------------------------

#ifndef __ADELAYEDIT_H
#define __ADELAYEDIT_H

#ifndef __ADelay__
#include "ADelay.hpp"
#endif

//-------------------------------------------------------------------------------------------------------
class ADelayEdit : public ADelay
{
public:
	ADelayEdit (audioMasterCallback audioMaster);
	~ADelayEdit ();

	virtual void setParameter (long index, float value);

	virtual bool getEffectName (char* name);
	virtual bool getProductString (char* text);
	virtual long getVendorVersion () { return 1000; }
};

#endif
