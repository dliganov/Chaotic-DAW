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

#ifndef __vstxsynth__
#define __vstxsynth__

#ifndef __audioeffectx__
#include "audioeffectx.h"
#endif

//------------------------------------------------------------------------------------------
enum
{
	// Global
	kNumPrograms = 128,
	kNumOutputs = 2,

	// Parameters Tags
	kWaveform1 = 0,
	kFreq1,
	kVolume1,

	kWaveform2,
	kFreq2,
	kVolume2,

	kVolume,
	
	kNumParams
};

//------------------------------------------------------------------------------------------
// VstXSynthProgram
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
class VstXSynthProgram
{
friend class VstXSynth;
public:
	VstXSynthProgram ();
	~VstXSynthProgram () {}

private:
	float fWaveform1;
	float fFreq1;
	float fVolume1;

	float fWaveform2;
	float fFreq2;
	float fVolume2;

	float fVolume;
	char name[24];
};

//------------------------------------------------------------------------------------------
// VstXSynth
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
class VstXSynth : public AudioEffectX
{
public:
	VstXSynth (audioMasterCallback audioMaster);
	~VstXSynth ();

	virtual void process (float **inputs, float **outputs, long sampleframes);
	virtual void processReplacing (float **inputs, float **outputs, long sampleframes);
	virtual long processEvents (VstEvents* events);

	virtual void setProgram (long program);
	virtual void setProgramName (char *name);
	virtual void getProgramName (char *name);
	virtual bool getProgramNameIndexed (long category, long index, char* text);
	virtual bool copyProgram (long destination);

	virtual void setParameter (long index, float value);
	virtual float getParameter (long index);
	virtual void getParameterLabel (long index, char *label);
	virtual void getParameterDisplay (long index, char *text);
	virtual void getParameterName (long index, char *text);
	
	virtual void setSampleRate (float sampleRate);
	virtual void setBlockSize (long blockSize);
	
	virtual void resume ();

	virtual bool getOutputProperties (long index, VstPinProperties* properties);
		
	virtual bool getEffectName (char* name);
	virtual bool getVendorString (char* text);
	virtual bool getProductString (char* text);
	virtual long getVendorVersion () { return 1000; }
	virtual long canDo (char* text);

	virtual long getMidiProgramName (long channel, MidiProgramName* midiProgramName);
	virtual long getCurrentMidiProgram (long channel, MidiProgramName* currentProgram);
	virtual long getMidiProgramCategory (long channel, MidiProgramCategory* category);
	virtual bool hasMidiProgramsChanged (long channel);
	virtual bool getMidiKeyName (long channel, MidiKeyName* keyName);

private:
	void initProcess ();
	void noteOn (long note, long velocity, long delta);
	void noteOff ();
	void fillProgram (long channel, long prg, MidiProgramName* mpn);

	float fWaveform1;
	float fFreq1;
	float fVolume1;
	float fWaveform2;
	float fFreq2;
	float fVolume2;

	float fVolume;	
	float fPhase1, fPhase2;
	float fScaler;

	VstXSynthProgram* programs;

	long channelPrograms[16];

	long currentNote;
	long currentVelocity;
	long currentDelta;
	bool noteIsOn;
};

#endif
