//-------------------------------------------------------------------------------------------------------
// VST Plug-Ins SDK
// Example AGain (VST 1.0)
// Stereo plugin which applies a Gain [-oo, 0dB]
// © 2003, Steinberg Media Technologies, All Rights Reserved
//-------------------------------------------------------------------------------------------------------

#ifndef __AGAIN_H
#include "AGain.hpp"
#endif

//-------------------------------------------------------------------------------------------------------
AGain::AGain (audioMasterCallback audioMaster)
	: AudioEffectX (audioMaster, 1, 1)	// 1 program, 1 parameter only
{
	fGain = 1.;				// default to 0 dB
	setNumInputs (2);		// stereo in
	setNumOutputs (2);		// stereo out
	setUniqueID ('Gain');	// identify
	canMono ();				// makes sense to feed both inputs with the same signal
	canProcessReplacing ();	// supports both accumulating and replacing output
	strcpy (programName, "Default");	// default program name
}

//-------------------------------------------------------------------------------------------------------
AGain::~AGain ()
{
	// nothing to do here
}

//-------------------------------------------------------------------------------------------------------
void AGain::setProgramName (char *name)
{
	strcpy (programName, name);
}

//-----------------------------------------------------------------------------------------
void AGain::getProgramName (char *name)
{
	strcpy (name, programName);
}

//-----------------------------------------------------------------------------------------
void AGain::setParameter (long index, float value)
{
	fGain = value;
}

//-----------------------------------------------------------------------------------------
float AGain::getParameter (long index)
{
	return fGain;
}

//-----------------------------------------------------------------------------------------
void AGain::getParameterName (long index, char *label)
{
	strcpy (label, "Gain");
}

//-----------------------------------------------------------------------------------------
void AGain::getParameterDisplay (long index, char *text)
{
	dB2string (fGain, text);
}

//-----------------------------------------------------------------------------------------
void AGain::getParameterLabel(long index, char *label)
{
	strcpy (label, "dB");
}

//------------------------------------------------------------------------
bool AGain::getEffectName (char* name)
{
	strcpy (name, "Gain");
	return true;
}

//------------------------------------------------------------------------
bool AGain::getProductString (char* text)
{
	strcpy (text, "Gain");
	return true;
}

//------------------------------------------------------------------------
bool AGain::getVendorString (char* text)
{
	strcpy (text, "Steinberg Media Technologies");
	return true;
}

//-----------------------------------------------------------------------------------------
void AGain::process (float **inputs, float **outputs, long sampleFrames)
{
    float *in1  =  inputs[0];
    float *in2  =  inputs[1];
    float *out1 = outputs[0];
    float *out2 = outputs[1];

    while (--sampleFrames >= 0)
    {
        (*out1++) += (*in1++) * fGain;    // accumulating
        (*out2++) += (*in2++) * fGain;
    }
}

//-----------------------------------------------------------------------------------------
void AGain::processReplacing (float **inputs, float **outputs, long sampleFrames)
{
    float *in1  =  inputs[0];
    float *in2  =  inputs[1];
    float *out1 = outputs[0];
    float *out2 = outputs[1];

    while (--sampleFrames >= 0)
    {
        (*out1++) = (*in1++) * fGain;    // replacing
        (*out2++) = (*in2++) * fGain;
    }
}
