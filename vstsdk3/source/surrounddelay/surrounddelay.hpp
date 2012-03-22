//-------------------------------------------------------------------------------------------------------
// VST Plug-Ins SDK
//-
// Example SurroundDelay (VST 2.3)
// Simple Surround Delay plugin with Editor using VSTGUI
//-
// © 2003, Steinberg Media Technologies, All Rights Reserved
//-------------------------------------------------------------------------------------------------------

#ifndef __surrounddelay
#define __surrounddelay

#ifndef __audioeffectx__
#include "audioeffectx.h"
#endif

#ifndef __ADelay__
#include "ADelay.hpp"
#endif

#define MAX_CHANNELS 6  // maximun number of channel

//------------------------------------------------------------------------
// SurroundDelay declaration
//------------------------------------------------------------------------
class SurroundDelay : public ADelay
{
public:
	SurroundDelay (audioMasterCallback audioMaster);
	~SurroundDelay ();

	virtual void process (float **inputs, float **outputs, long sampleframes);
	virtual void processReplacing (float **inputs, float **outputs, long sampleframes);

	void setParameter (long index, float value);

	virtual long vendorSpecific (long lArg1, long lArg2, void* ptrArg, float floatArg);
	

	// functions VST version 2
	virtual bool getVendorString (char* text) { if (text) strcpy (text, "Steinberg");return true; }
	virtual bool getProductString (char* text) { if (text) strcpy (text, "SDelay");return true; }
	virtual long getVendorVersion () { return 1000; }

	virtual VstPlugCategory getPlugCategory () { return kPlugSurroundFx; }
	
	virtual bool getSpeakerArrangement (VstSpeakerArrangement** pluginInput, VstSpeakerArrangement** pluginOutput);
	virtual bool setSpeakerArrangement (VstSpeakerArrangement* pluginInput, VstSpeakerArrangement* pluginOutput);

	virtual void resume ();

private:
	VstSpeakerArrangement* plugInput;
	VstSpeakerArrangement* plugOutput;

	float* sBuffers[MAX_CHANNELS];


};

#endif
