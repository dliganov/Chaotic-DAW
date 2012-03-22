//-------------------------------------------------------------------------------------------------------
// VST Plug-Ins SDK
//-
// Example ADelay (VST 2.0)
// Simple Delay plugin (Mono->Stereo)
//-
// © 2003, Steinberg Media Technologies, All Rights Reserved
//-------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>

#ifndef __ADelay__
#include "ADelay.hpp"
#endif

#ifndef __AEffEditor__
#include "AEffEditor.hpp"
#endif

//----------------------------------------------------------------------------- 
ADelayProgram::ADelayProgram ()
{
	// default Program Values
	fDelay = 0.5;
	fFeedBack = 0.5;
	fOut = 0.75;

	strcpy (name, "Init");
}

//-----------------------------------------------------------------------------
ADelay::ADelay (audioMasterCallback audioMaster)
	: AudioEffectX (audioMaster, kNumPrograms, kNumParams)
{
	// init
	size = 44100;
	cursor = 0;
	delay = 0;
	buffer = new float[size];
	
	programs = new ADelayProgram[numPrograms];
	fDelay = fFeedBack = fOut = 0;

	if (programs)
		setProgram (0);

	setNumInputs (1);	// mono input
	setNumOutputs (2);	// stereo output

	hasVu ();
	canProcessReplacing ();
	setUniqueID ('ADly');

	resume ();		// flush buffer
}

//------------------------------------------------------------------------
ADelay::~ADelay ()
{
	if (buffer)
		delete[] buffer;
	if (programs)
		delete[] programs;
}

//------------------------------------------------------------------------
void ADelay::setProgram (long program)
{
	ADelayProgram* ap = &programs[program];

	curProgram = program;
	setParameter (kDelay, ap->fDelay);	
	setParameter (kFeedBack, ap->fFeedBack);
	setParameter (kOut, ap->fOut);
}

//------------------------------------------------------------------------
void ADelay::setDelay (float fdelay)
{
	fDelay = fdelay;
	programs[curProgram].fDelay = fdelay;
	cursor = 0;
	delay = (long)(fdelay * (float)(size - 1));
}

//------------------------------------------------------------------------
void ADelay::setProgramName (char *name)
{
	strcpy (programs[curProgram].name, name);
}

//------------------------------------------------------------------------
void ADelay::getProgramName (char *name)
{
	if (!strcmp (programs[curProgram].name, "Init"))
		sprintf (name, "%s %d", programs[curProgram].name, curProgram + 1);
	else
		strcpy (name, programs[curProgram].name);
}

//-----------------------------------------------------------------------------------------
bool ADelay::getProgramNameIndexed (long category, long index, char* text)
{
	if (index < kNumPrograms)
	{
		strcpy (text, programs[index].name);
		return true;
	}
	return false;
}

//------------------------------------------------------------------------
void ADelay::resume ()
{
	memset (buffer, 0, size * sizeof (float));
	AudioEffectX::resume ();
}

//------------------------------------------------------------------------
void ADelay::setParameter (long index, float value)
{
	ADelayProgram* ap = &programs[curProgram];

	switch (index)
	{
		case kDelay :    setDelay (value);					break;
		case kFeedBack : fFeedBack = ap->fFeedBack = value; break;
		case kOut :      fOut = ap->fOut = value;			break;
	}
	if (editor)
		editor->postUpdate ();
}

//------------------------------------------------------------------------
float ADelay::getParameter (long index)
{
	float v = 0;

	switch (index)
	{
		case kDelay :    v = fDelay;	break;
		case kFeedBack : v = fFeedBack; break;
		case kOut :      v = fOut;		break;
	}
	return v;
}

//------------------------------------------------------------------------
void ADelay::getParameterName (long index, char *label)
{
	switch (index)
	{
		case kDelay :    strcpy (label, "Delay");		break;
		case kFeedBack : strcpy (label, "FeedBack");	break;
		case kOut :      strcpy (label, "Volume");		break;
	}
}

//------------------------------------------------------------------------
void ADelay::getParameterDisplay (long index, char *text)
{
	switch (index)
	{
		case kDelay :    long2string (delay, text);			break;
		case kFeedBack : float2string (fFeedBack, text);	break;
		case kOut :      dB2string (fOut, text);			break;
	}
}

//------------------------------------------------------------------------
void ADelay::getParameterLabel (long index, char *label)
{
	switch (index)
	{
		case kDelay :    strcpy (label, "samples");	break;
		case kFeedBack : strcpy (label, "amount");	break;
		case kOut :      strcpy (label, "dB");		break;
	}
}

//------------------------------------------------------------------------
bool ADelay::getEffectName (char* name)
{
	strcpy (name, "Delay");
	return true;
}

//------------------------------------------------------------------------
bool ADelay::getProductString (char* text)
{
	strcpy (text, "Delay");
	return true;
}

//------------------------------------------------------------------------
bool ADelay::getVendorString (char* text)
{
	strcpy (text, "Steinberg Media Technologies");
	return true;
}

//------------------------------------------------------------------------
void ADelay::process (float** inputs, float** outputs, long sampleFrames)
{
	float* in = inputs[0];
	float* out1 = outputs[0];
	float* out2 = outputs[1];

	while (--sampleFrames >= 0)
	{
		float x = *in++;
		float y = buffer[cursor];
		buffer[cursor++] = x + y * fFeedBack;
		if (cursor >= delay)
			cursor = 0;
		(*out1++) += y;
		if (out2)
			(*out2++) += y;
	}
}

//---------------------------------------------------------------------------
void ADelay::processReplacing (float** inputs, float** outputs, long sampleFrames)
{
	float* in = inputs[0];
	float* out1 = outputs[0];
	float* out2 = outputs[1];

	while (--sampleFrames >= 0)
	{
		float x = *in++;
		float y = buffer[cursor];
		buffer[cursor++] = x + y * fFeedBack;
		if (cursor >= delay)
			cursor = 0;
		*out1++ = y;
		if (out2)
			*out2++ = y;
	}
}
