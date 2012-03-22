//------------------------------------------------------------------------
//-
//- Project     : Use different Controls of VSTGUI
//- Filename    : controlsgui.cpp
//- Created by  : Yvan Grabit
//- Description :
//-
//- © 2000-1999, Steinberg Soft und Hardware GmbH, All Rights Reserved
//------------------------------------------------------------------------

#ifndef __controlsgui__
#include "controlsgui.h"
#endif

#ifndef __controlsguieditor__
#include "controlsguieditor.h"
#endif

#include <stdio.h>
#include <string.h>

// global

extern bool oome;


//-----------------------------------------------------------------------------
class Program
{
public:
	Program () {}

	void setValue (float val) { value = val; }

protected:
	friend class Controlsgui;

	float value;
	char name[24];
};



//-----------------------------------------------------------------------------
Controlsgui::Controlsgui (audioMasterCallback audioMaster) :
  AudioEffectX (audioMaster, 4, kNumParams)
{
	setNumInputs (2);
	setNumOutputs (2);
	hasVu (false);
	canMono (true);
	canProcessReplacing ();
	setUniqueID (CCONST ('C', 'o', 'n', '9'));

	// init programs
	programs = new Program[numPrograms];
	if (programs)
		for (int i = 0; i < numPrograms; i++)
		{
			Program *prg = &programs[i];
			prg->setValue ((float)i / (float)(numPrograms - 1));
			sprintf (prg->name, "Prog %d", i + 1);
		}

	fSliderValue = 0.f;

	editor = new ControlsguiEditor (this);
}

//-----------------------------------------------------------------------------
Controlsgui::~Controlsgui ()
{
	if (programs)
		delete[] programs;
}

//------------------------------------------------------------------------
long Controlsgui::vendorSpecific (long lArg1, long lArg2, void* ptrArg, float floatArg)
{
	if (editor && lArg1 == 'stCA' && lArg2 == 'Whee')
		return editor->onWheel (floatArg) == true ? 1 : 0;
	else
		return AudioEffectX::vendorSpecific (lArg1, lArg2, ptrArg, floatArg);
}

//-----------------------------------------------------------------------------
void Controlsgui::resume ()
{
	if (editor)
		((ControlsguiEditor*)editor)->resume ();
}

//-----------------------------------------------------------------------------
void Controlsgui::suspend ()
{
	if (editor)
		((ControlsguiEditor*)editor)->suspend ();
}

//-----------------------------------------------------------------------------
bool Controlsgui::keysRequired ()
{
	if (editor)
		return ((ControlsguiEditor*)editor)->keysRequired ();
	else
		return false;
}

//-----------------------------------------------------------------------------
bool Controlsgui::string2parameter (long index, char* text)
{
	bool v = false;

	
	switch (index)
	{
		case kSliderVTag :
		case kSliderHTag :
		case kKnobTag :
			if (text)
			{
				sscanf (text, "%f", &fSliderValue);
				if (fSliderValue < 0.f)
					fSliderValue = 0.f;
				else if (fSliderValue > 1.f)
					fSliderValue = 1.f;

				Program* ap = programs + curProgram;
				ap->setValue (fSliderValue);
			}
			v = true;
		break;

		default:
			v = false;
	}

	return v;
}

//-----------------------------------------------------------------------------
void Controlsgui::setBlockSize (long size)
{
	AudioEffect::setBlockSize (size);
}

//-----------------------------------------------------------------------------
void Controlsgui::setProgram (long program)
{
	curProgram = program;
	Program* ap = programs + curProgram;
	
	setParameter (kKnobTag, ap->value);
}

//-----------------------------------------------------------------------------
void Controlsgui::setProgramName (char *name)
{
	strcpy (programs[curProgram].name, name);
}

//-----------------------------------------------------------------------------
void Controlsgui::getProgramName (char *name)
{
	strcpy (name, programs[curProgram].name);
}

//-------------------------------------------------------------------------
bool Controlsgui::getProgramNameIndexed (long category, long index, char* text)
{
	if (index >= 0 && index < numPrograms)
	{
		if (!strcmp (programs[index].name, "Init"))
			sprintf (text, "%s %d", programs[index].name, index + 1);
		else
			strcpy (text, programs[index].name);

		return true;
	}
	return false;
}
//-----------------------------------------------------------------------------
void Controlsgui::setParameter (long index, float value)
{
	Program* ap = programs + curProgram;

	switch (index)
	{
			case kSliderVTag :
			case kSliderHTag :
			case kKnobTag :
				ap->value = fSliderValue = value;
				break;
	}
	if (editor)
		((AEffGUIEditor*)editor)->setParameter (index, value);
}

//-----------------------------------------------------------------------------
float Controlsgui::getParameter (long index)
{
	float v = 0;

	switch (index)
	{
	case kSliderVTag :
	case kSliderHTag :
	case kKnobTag :
		v = fSliderValue;
		break;
	}
	return v;
}

//-----------------------------------------------------------------------------
void Controlsgui::getParameterDisplay (long index, char *text)
{
	switch (index)
	{
	case kSliderHTag :
	case kSliderVTag :
	case kKnobTag :
		sprintf (text, "%.6f", fSliderValue);
		break;

	default :
		strcpy (text, "0");
	}
}

//-----------------------------------------------------------------------------
void Controlsgui::getParameterLabel (long index, char *text)
{
	switch (index)
	{
	case kSliderHTag :
	case kSliderVTag :
	case kKnobTag :
		strcpy (text, "float");
		break;

	default :
		strcpy (text, "-");
	}
}

//-----------------------------------------------------------------------------
void Controlsgui::getParameterName (long index, char *text)
{
	switch (index)
	{
	case kSliderHTag :
		strcpy (text, "SliderBase H");
		break;
	case kSliderVTag :
		sprintf (text, "SliderBase V");
		break;
	case kKnobTag :
		sprintf (text, "Knob");
		break;

	default :
		strcpy (text, "-");
	}
}

//-----------------------------------------------------------------------------
void Controlsgui::process (float **inputs, float **outputs, long sampleFrames)
{
	outputs = outputs;
}

//-----------------------------------------------------------------------------
void Controlsgui::processReplacing (float **inputs, float **outputs, long sampleFrames)
{
	float *in1 = inputs[0];
	float *in2 = inputs[1];
	float *out1 = outputs[0];
	float *out2 = outputs[1];
	
#if 0
	VstTimeInfo *timeInfo = getTimeInfo (kVstPpqPosValid|kVstTempoValid|kVstBarsValid | kVstTimeSigValid|kVstCyclePosValid|kVstSmpteValid);
	if (timeInfo)
	{
		fprintf (stderr, "timeInfo\n");
		fprintf (stderr, "\tsamplePos     %f\n", timeInfo->samplePos);
		fprintf (stderr, "\tsampleRate    %f\n", timeInfo->sampleRate);
		fprintf (stderr, "\tnanoSeconds   %f\n", timeInfo->nanoSeconds);
		fprintf (stderr, "\tppqPos        %f\n", timeInfo->ppqPos);
		fprintf (stderr, "\ttempo         %f\n",  timeInfo->tempo);
		fprintf (stderr, "\tcycleStartPos %f\n", timeInfo->cycleStartPos);
		fprintf (stderr, "\tcycleEndPos   %f\n", timeInfo->cycleEndPos);
		fprintf (stderr, "\ttimeSigNumerator   %d\n", timeInfo->timeSigNumerator);
		fprintf (stderr, "\ttimeSigDenominator %d\n", timeInfo->timeSigDenominator);
		fprintf (stderr, "\tbarStartPos        %f\n", timeInfo->barStartPos);
		fprintf (stderr, "\tsmpteOffset        %d\n", timeInfo->smpteOffset);
		fprintf (stderr, "\tsmpteFrameRate     %d\n", timeInfo->smpteFrameRate);
		fprintf (stderr, "\tsamplesToNextClock %d\n", timeInfo->samplesToNextClock);
	}


	if(out1 != in1)
		memcpy(out1, in1, sampleFrames * 4);
	if(out2 != in2)
		memcpy(out2, in2, sampleFrames * 4);
#else
	if (out1 != in1)
		memcpy (out1, in1, sampleFrames * 4);
	if (out2 != in2)
		memcpy (out2, in2, sampleFrames * 4);
#endif
}
