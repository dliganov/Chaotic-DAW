//-------------------------------------------------------------------------------------------------------
// VST Plug-Ins SDK
//-
// Example ADelay (VST 2.0)
// Simple Delay plugin with Editor using VSTGUI (Mono->Stereo)
//-
// © 2003, Steinberg Media Technologies, All Rights Reserved
//-------------------------------------------------------------------------------------------------------

#ifndef __ADEditor
#define __ADEditor


// include VSTGUI
#ifndef __vstgui__
#include "vstgui.h"
#endif


//-----------------------------------------------------------------------------
class ADEditor : public AEffGUIEditor, public CControlListener
{
public:
	ADEditor (AudioEffect *effect);
	virtual ~ADEditor ();

public:
	virtual long open (void *ptr);
	virtual void close ();

	virtual void setParameter (long index, float value);
	virtual void valueChanged (CDrawContext* context, CControl* control);

private:
	// Controls
	CVerticalSlider *delayFader;
	CVerticalSlider *feedbackFader;
	CVerticalSlider *volumeFader;

	CParamDisplay *delayDisplay;
	CParamDisplay *feedbackDisplay;
	CParamDisplay *volumeDisplay;

	// Bitmap
	CBitmap *hBackground;

	// Others
	bool bOpened;
};

#endif
