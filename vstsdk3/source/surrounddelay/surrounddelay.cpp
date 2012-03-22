//-------------------------------------------------------------------------------------------------------
// VST Plug-Ins SDK
//-
// Example SurroundDelay (VST 2.3)
// Simple Surround Delay plugin with Editor using VSTGUI
//-
// © 2003, Steinberg Media Technologies, All Rights Reserved
//-------------------------------------------------------------------------------------------------------

#ifndef __surrounddelay
#include "surrounddelay.hpp"
#endif

#ifndef __ADEditor
#include "ADEditor.hpp"
#endif

#include <string.h>
#include <stdio.h>

extern bool oome;

//-----------------------------------------------------------------------------
SurroundDelay::SurroundDelay (audioMasterCallback audioMaster)
 : ADelay (audioMaster), plugInput (0), plugOutput (0)
{
	
	// The first buffer is allocated in ADelay's constructor
	for (int i = 1; i < MAX_CHANNELS; i++)
	{
		sBuffers[i] = new float[size];
	}
	
	setNumInputs (MAX_CHANNELS);
	setNumOutputs (MAX_CHANNELS);

	// We initialize the arrangements to default values.
	// Nevertheless, the host should modify them via
	// appropriate calls to setSpeakerArrangement.
	allocateArrangement (&plugInput, MAX_CHANNELS);
	plugInput->type = kSpeakerArr51;

	allocateArrangement (&plugOutput, MAX_CHANNELS);
	plugOutput->type = kSpeakerArr51;

	setUniqueID ('SDlE');
	editor = new ADEditor (this);
	
	if (!editor)
		oome = true;

	resume ();
}

//-----------------------------------------------------------------------------
SurroundDelay::~SurroundDelay ()
{
	sBuffers[0] = 0;
	// We let ~ADelay delete "buffer"...
	for (int i = 1; i < MAX_CHANNELS; i++)
	{
		if (sBuffers[i])
		{
			delete[] sBuffers[i];
		}
		sBuffers[i] = 0;
	}
	
	deallocateArrangement (&plugInput);
	deallocateArrangement (&plugOutput);
}

//------------------------------------------------------------------------
void SurroundDelay::resume ()
{
	memset (buffer, 0, size * sizeof (float));
	sBuffers[0] = buffer;

	for (int i = 1; i < MAX_CHANNELS; i++)
	{
		memset (sBuffers[i], 0, size * sizeof (float));
	}
}

//------------------------------------------------------------------------
long SurroundDelay::vendorSpecific (long lArg1, long lArg2, void* ptrArg, float floatArg)
{
	// This is a simple way of implementing the mouse's wheel...
	if (editor && lArg1 == 'stCA' && lArg2 == 'Whee')
		return editor->onWheel (floatArg) == true ? 1 : 0;
	else
		return AudioEffectX::vendorSpecific (lArg1, lArg2, ptrArg, floatArg);
}

//------------------------------------------------------------------------
bool SurroundDelay::getSpeakerArrangement (VstSpeakerArrangement** pluginInput, VstSpeakerArrangement** pluginOutput)
{
	*pluginInput  = plugInput;
	*pluginOutput = plugOutput;
	return true;
}

//------------------------------------------------------------------------
bool SurroundDelay::setSpeakerArrangement (VstSpeakerArrangement* pluginInput,
									 VstSpeakerArrangement* pluginOutput)
{
	if (!pluginOutput || !pluginInput)
		return false;

	bool result = true;
	
	// This plug-in can act on any speaker arrangement,
	// provided that there are the same number of inputs/outputs.
	if (pluginInput->numChannels > MAX_CHANNELS)
	{
		// This plug-in can't have so many channels. So we answer
		// false, and we set the input arrangement with the maximum
		// number of channels possible
		result = false;
		allocateArrangement (&plugInput, MAX_CHANNELS);
		plugInput->type = kSpeakerArr51;
	}
	else
	{
		matchArrangement (&plugInput, pluginInput);
	}
	
	if (pluginOutput->numChannels != plugInput->numChannels)
	{
		// This plug-in only deals with symetric IO configurations...
		result = false;
		matchArrangement (&plugOutput, plugInput);
	}
	else
	{
		matchArrangement (&plugOutput, pluginOutput);
	}

	return result;
}

//------------------------------------------------------------------------
void SurroundDelay::process (float **inputs, float **outputs, long sampleframes)
{
	// Actually, the idea should be to deal with the process according to the
	// speaker arrangement provided. It's up to you to do that...

	float* inputs2[1];
	float* outputs2[2];

	outputs2[1] = NULL;

	long cursorTemp = cursor;

	for (int i = 0; i < plugInput->numChannels; i++)
	{
		// ADelay's process changes this internal
		// variable. We need to restore it before processing
		// each channel...
		cursor = cursorTemp;
		
		// ADelay's process uses "buffer", so we have to
		// allocate one buffer for each channel (see SurroundDelay's
		// constructor), and make "buffer" point to the right buffer
		// before processing each channel
		buffer = sBuffers[i];
		
		inputs2[0] = inputs[i];
		outputs2[0] = outputs[i];
		
		// We use ADelay to process each channel
		ADelay::process (inputs2, outputs2, sampleframes);
	}

	// We restore here "buffer"'s initial value
	buffer = sBuffers[0];
	
}

//------------------------------------------------------------------------
void SurroundDelay::processReplacing (float **inputs, float **outputs, long sampleframes)
{
	float* inputs2[1];
	float* outputs2[2];

	outputs2[1] = NULL;

	long cursorTemp = cursor;

	for (int i = 0; i < plugInput->numChannels; i++)
	{
		cursor = cursorTemp;
		
		buffer = sBuffers[i];

		inputs2[0] = inputs[i];
		outputs2[0] = outputs[i];
		
		ADelay::processReplacing (inputs2, outputs2, sampleframes);
	}

	buffer = sBuffers[0];
}

//-----------------------------------------------------------------------------
void SurroundDelay::setParameter (long index, float value)
{
	ADelay::setParameter (index, value);

	if (editor)
		((AEffGUIEditor*)editor)->setParameter (index, value);
}
