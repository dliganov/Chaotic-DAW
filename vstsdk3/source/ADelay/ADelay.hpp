//-------------------------------------------------------------------------------------------------------
// VST Plug-Ins SDK
//-
// Example ADelay (VST 2.0)
// Simple Delay plugin (Mono->Stereo)
//-
// © 2003, Steinberg Media Technologies, All Rights Reserved
//-------------------------------------------------------------------------------------------------------

#ifndef __ADelay__
#define __ADelay__

#ifndef __audioeffectx__
#include "audioeffectx.h"
#endif

enum
{
	// Global
	kNumPrograms = 16,

	// Parameters Tags
	kDelay = 0,
	kFeedBack,
	kOut,

	kNumParams
};

class ADelay;

//------------------------------------------------------------------------
class ADelayProgram
{
friend class ADelay;
public:
	ADelayProgram ();
	~ADelayProgram () {}

private:	
	float fDelay;
	float fFeedBack;
	float fOut;
	char name[24];
};

//------------------------------------------------------------------------
class ADelay : public AudioEffectX
{
public:
	ADelay (audioMasterCallback audioMaster);
	~ADelay ();

	virtual void process (float **inputs, float **outputs, long sampleframes);
	virtual void processReplacing (float **inputs, float **outputs, long sampleFrames);

	virtual void setProgram (long program);
	virtual void setProgramName (char *name);
	virtual void getProgramName (char *name);
	virtual bool getProgramNameIndexed (long category, long index, char* text);
	
	virtual void setParameter (long index, float value);
	virtual float getParameter (long index);
	virtual void getParameterLabel (long index, char *label);
	virtual void getParameterDisplay (long index, char *text);
	virtual void getParameterName (long index, char *text);

	virtual void resume ();

	virtual bool getEffectName (char* name);
	virtual bool getVendorString (char* text);
	virtual bool getProductString (char* text);
	virtual long getVendorVersion () { return 1000; }
	
	virtual VstPlugCategory getPlugCategory () { return kPlugCategEffect; }

protected:
	void setDelay (float delay);

	ADelayProgram *programs;
	
	float *buffer;
	float fDelay, fFeedBack, fOut;
	
	
	long delay;
	long size;
	long cursor;
};

#endif
