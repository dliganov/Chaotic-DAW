//-------------------------------------------------------------------------------------------------------
// VST Plug-Ins SDK
// -
// Example VstXSynth (VST 2.0)
// A simple 2 oscillators test 'synth',
// Each oscillator has waveform, frequency, and volume
//
// *very* basic monophonic 'synth' example. you should not attempt to use this
// example 'algorithm' to start a serious virtual instrument; it is intended to demonstrate
// how VstEvents ('MIDI') are handled, but not how a virtual analog synth works.
// there are numerous much better examples on the web which show how to deal with
// bandlimited waveforms etc.
// -
// © 2003, Steinberg Media Technologies, All Rights Reserved
//-------------------------------------------------------------------------------------------------------

#include <stdio.h>

#ifndef __vstxsynth__
#include "vstxsynth.h"
#endif

#ifndef __gmnames__
#include "gmnames.h"
#endif

//-----------------------------------------------------------------------------------------
// VstXSynthProgram
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
VstXSynthProgram::VstXSynthProgram ()
{
	// Default Program Values
	fWaveform1 = 0.f;	// saw
	fFreq1     =.0f;
	fVolume1   = .33f;

	fWaveform2 = 1.f;	// pulse
	fFreq2     = .05f;	// slightly higher
	fVolume2   = .33f;
	
	fVolume    = .9f;
	strcpy (name, "Basic");
}

//-----------------------------------------------------------------------------------------
// VstXSynth
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
VstXSynth::VstXSynth (audioMasterCallback audioMaster)
	: AudioEffectX (audioMaster, kNumPrograms, kNumParams)
{
	// initialize programs
	programs = new VstXSynthProgram[kNumPrograms];
	for (long i = 0; i < 16; i++)
		channelPrograms[i] = i;

	if (programs)
		setProgram (0);
	
	if (audioMaster)
	{
		setNumInputs (0);				// no inputs
		setNumOutputs (kNumOutputs);	// 2 outputs, 1 for each oscillator
		canProcessReplacing ();
		hasVu (false);
		hasClip (false);
		isSynth ();
		setUniqueID ('VxS2');			// <<<! *must* change this!!!!
	}
	initProcess ();
	suspend ();
}

//-----------------------------------------------------------------------------------------
VstXSynth::~VstXSynth ()
{
	if (programs)
		delete[] programs;
}

//-----------------------------------------------------------------------------------------
void VstXSynth::setProgram (long program)
{
	if (program < 0 || program >= kNumPrograms)
		return;
	
	VstXSynthProgram *ap = &programs[program];
	curProgram = program;
	
	fWaveform1 = ap->fWaveform1;
	fFreq1     = ap->fFreq1;
	fVolume1   = ap->fVolume1;

	fWaveform2 = ap->fWaveform2;
	fFreq2     = ap->fFreq2;
	fVolume2   = ap->fVolume2;
	
	fVolume    = ap->fVolume;
}

//-----------------------------------------------------------------------------------------
void VstXSynth::setProgramName (char *name)
{
	strcpy (programs[curProgram].name, name);
}

//-----------------------------------------------------------------------------------------
void VstXSynth::getProgramName (char *name)
{
	strcpy (name, programs[curProgram].name);
}

//-----------------------------------------------------------------------------------------
void VstXSynth::getParameterLabel (long index, char *label)
{
	switch (index)
	{
		case kWaveform1:
		case kWaveform2:
			strcpy (label, "Shape");
			break;

		case kFreq1:
		case kFreq2:
			strcpy (label, "Hz");
			break;

		case kVolume1:
		case kVolume2:
		case kVolume:
			strcpy (label, "dB");
			break;
	}
}

//-----------------------------------------------------------------------------------------
void VstXSynth::getParameterDisplay (long index, char *text)
{
	text[0] = 0;
	switch (index)
	{
		case kWaveform1:
			if (fWaveform1 < .5)
				strcpy (text, "Sawtooth");
			else
				strcpy (text, "Pulse   ");
			break;

		case kFreq1:		float2string (fFreq1, text);	break;
		case kVolume1:		dB2string (fVolume1, text);		break;
		
		case kWaveform2:
			if (fWaveform2 < .5)
				strcpy (text, "Sawtooth");
			else
				strcpy (text, "Pulse");
			break;
		case kFreq2:		float2string (fFreq2, text);	break;
		case kVolume2:		dB2string (fVolume2, text);		break;
		case kVolume:		dB2string (fVolume, text);		break;
	}
}

//-----------------------------------------------------------------------------------------
void VstXSynth::getParameterName (long index, char *label)
{
	switch (index)
	{
		case kWaveform1:	strcpy (label, "Wave 1");	break;
		case kFreq1:		strcpy (label, "Freq 1");	break;
		case kVolume1:		strcpy (label, "Levl 1");	break;
		case kWaveform2:	strcpy (label, "Wave 2");	break;
		case kFreq2:		strcpy (label, "Freq 2");	break;
		case kVolume2:		strcpy (label, "Levl 2");	break;
		case kVolume:		strcpy (label, "Volume");	break;
	}
}

//-----------------------------------------------------------------------------------------
void VstXSynth::setParameter (long index, float value)
{
	VstXSynthProgram *ap = &programs[curProgram];
	switch (index)
	{
		case kWaveform1:	fWaveform1	= ap->fWaveform1	= value;	break;
		case kFreq1:		fFreq1 		= ap->fFreq1		= value;	break;
		case kVolume1:		fVolume1	= ap->fVolume1		= value;	break;
		case kWaveform2:	fWaveform2	= ap->fWaveform2	= value;	break;
		case kFreq2:		fFreq2		= ap->fFreq2		= value;	break;
		case kVolume2:		fVolume2	= ap->fVolume2		= value;	break;
		case kVolume:		fVolume		= ap->fVolume		= value;	break;
	}
}

//-----------------------------------------------------------------------------------------
float VstXSynth::getParameter (long index)
{
	float value = 0;
	switch (index)
	{
		case kWaveform1:	value = fWaveform1;	break;
		case kFreq1:		value = fFreq1;		break;
		case kVolume1:		value = fVolume1;	break;
		case kWaveform2:	value = fWaveform2;	break;
		case kFreq2:		value = fFreq2;		break;
		case kVolume2:		value = fVolume2;	break;
		case kVolume:		value = fVolume;	break;
	}
	return value;
}

//-----------------------------------------------------------------------------------------
bool VstXSynth::getOutputProperties (long index, VstPinProperties* properties)
{
	if (index < kNumOutputs)
	{
		sprintf (properties->label, "Vstx %1d", index + 1);
		properties->flags = kVstPinIsActive;
		if (index < 2)
			properties->flags |= kVstPinIsStereo;	// make channel 1+2 stereo
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------------------
bool VstXSynth::getProgramNameIndexed (long category, long index, char* text)
{
	if (index < kNumPrograms)
	{
		strcpy (text, programs[index].name);
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------------------
bool VstXSynth::copyProgram (long destination)
{
	if (destination < kNumPrograms)
	{
		programs[destination] = programs[curProgram];
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------------------
bool VstXSynth::getEffectName (char* name)
{
	strcpy (name, "VstXSynth");
	return true;
}

//-----------------------------------------------------------------------------------------
bool VstXSynth::getVendorString (char* text)
{
	strcpy (text, "Steinberg Media Technologies");
	return true;
}

//-----------------------------------------------------------------------------------------
bool VstXSynth::getProductString (char* text)
{
	strcpy (text, "Vst Test Synth");
	return true;
}

//-----------------------------------------------------------------------------------------
long VstXSynth::canDo (char* text)
{
	if (!strcmp (text, "receiveVstEvents"))
		return 1;
	if (!strcmp (text, "receiveVstMidiEvent"))
		return 1;
	if (!strcmp (text, "midiProgramNames"))
		return 1;
	return -1;	// explicitly can't do; 0 => don't know
}

// midi program names:
// as an example, GM names are used here. in fact, VstXSynth doesn't even support
// multi-timbral operation so it's really just for demonstration.
// a 'real' instrument would have a number of voices which use the
// programs[channelProgram[channel]] parameters when it receives
// a note on message.

//------------------------------------------------------------------------
long VstXSynth::getMidiProgramName (long channel, MidiProgramName* mpn)
{
	long prg = mpn->thisProgramIndex;
	if (prg < 0 || prg >= 128)
		return 0;
	fillProgram (channel, prg, mpn);
	if (channel == 9)
		return 1;
	return 128L;
}

//------------------------------------------------------------------------
long VstXSynth::getCurrentMidiProgram (long channel, MidiProgramName* mpn)
{
	if (channel < 0 || channel >= 16 || !mpn)
		return -1;
	long prg = channelPrograms[channel];
	mpn->thisProgramIndex = prg;
	fillProgram (channel, prg, mpn);
	return prg;
}

//------------------------------------------------------------------------
void VstXSynth::fillProgram (long channel, long prg, MidiProgramName* mpn)
{
	mpn->midiBankMsb =
	mpn->midiBankLsb = -1;
	mpn->reserved = 0;
	mpn->flags = 0;

	if (channel == 9)	// drums
	{
		strcpy (mpn->name, "Standard");
		mpn->midiProgram = 0;
		mpn->parentCategoryIndex = 0;
	}
	else
	{
		strcpy (mpn->name, GmNames[prg]);
		mpn->midiProgram = (char)prg;
		mpn->parentCategoryIndex = -1;	// for now

		for (long i = 0; i < kNumGmCategories; i++)
		{
			if (prg >= GmCategoriesFirstIndices[i] && prg < GmCategoriesFirstIndices[i + 1])
			{
				mpn->parentCategoryIndex = i;
				break;
			}
		}
	}
}

//------------------------------------------------------------------------
long VstXSynth::getMidiProgramCategory (long channel, MidiProgramCategory* cat)
{
	cat->parentCategoryIndex = -1;	// -1:no parent category
	cat->flags = 0;					// reserved, none defined yet, zero.
	long category = cat->thisCategoryIndex;
	if (channel == 9)
	{
		strcpy (cat->name, "Drums");
		return 1;
	}
	if (category >= 0 && category < kNumGmCategories)
		strcpy (cat->name, GmCategories[category]);
	else
		cat->name[0] = 0;
	return kNumGmCategories;
}

//------------------------------------------------------------------------
bool VstXSynth::hasMidiProgramsChanged (long channel)
{
	return false;	// updateDisplay ()
}

//------------------------------------------------------------------------
bool VstXSynth::getMidiKeyName (long channel, MidiKeyName* key)
								// struct will be filled with information for 'thisProgramIndex' and 'thisKeyNumber'
								// if keyName is "" the standard name of the key will be displayed.
								// if false is returned, no MidiKeyNames defined for 'thisProgramIndex'.
{
	// key->thisProgramIndex;		// >= 0. fill struct for this program index.
	// key->thisKeyNumber;			// 0 - 127. fill struct for this key number.
	key->keyName[0] = 0;
	key->reserved = 0;				// zero
	key->flags = 0;					// reserved, none defined yet, zero.
	return false;
}


