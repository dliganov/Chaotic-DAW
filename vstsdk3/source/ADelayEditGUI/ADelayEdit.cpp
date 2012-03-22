//-------------------------------------------------------------------------------------------------------
// VST Plug-Ins SDK
//-
// Example ADelay (VST 2.0)
// Simple Delay plugin with Editor using VSTGUI (Mono->Stereo)
//-
// © 2003, Steinberg Media Technologies, All Rights Reserved
//-------------------------------------------------------------------------------------------------------

#ifndef __ADELAYEDIT_H
#include "ADelayEdit.hpp"
#endif

#ifndef __ADEditor
#include "ADEditor.hpp"
#endif

#include <string.h>

extern bool oome;

//-----------------------------------------------------------------------------
ADelayEdit::ADelayEdit (audioMasterCallback audioMaster)
 : ADelay (audioMaster)
{
	setUniqueID ('ADlE');
	editor = new ADEditor (this);
	if (!editor)
		oome = true;
}

//-----------------------------------------------------------------------------
ADelayEdit::~ADelayEdit ()
{
	// the editor gets deleted by the
	// AudioEffect base class
}

//-----------------------------------------------------------------------------
void ADelayEdit::setParameter (long index, float value)
{
	ADelay::setParameter (index, value);

	if (editor)
		((AEffGUIEditor*)editor)->setParameter (index, value);
}

//------------------------------------------------------------------------
bool ADelayEdit::getEffectName (char* name)
{
	strcpy (name, "DelayEditGUI");
	return true;
}

//------------------------------------------------------------------------
bool ADelayEdit::getProductString (char* text)
{
	strcpy (text, "DelayEditGUI");
	return true;
}

