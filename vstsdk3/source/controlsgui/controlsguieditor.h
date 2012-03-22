//------------------------------------------------------------------------
//-
//- Project     : Use different Controls of VSTGUI
//- Filename    : controlsguieditor.h
//- Created by  : Yvan Grabit
//- Description :
//- 
//- © 2000-1999, Steinberg Soft und Hardware GmbH, All Rights Reserved
//------------------------------------------------------------------------

#ifndef __controlsguieditor__
#define __controlsguieditor__

#ifndef __vstgui__
#include "vstgui.h"
#endif


class Test;
class CLabel;

//-----------------------------------------------------------------------------
class ControlsguiEditor : public AEffGUIEditor, public CControlListener
{
public:
	ControlsguiEditor (AudioEffect *effect);
	virtual ~ControlsguiEditor ();

	void suspend ();
	void resume ();
	bool keysRequired ();

protected:
	virtual long open (void *ptr);
	virtual void idle ();
	void setParameter (long index, float value);
	virtual void close ();
	
	// VST 2.1
	virtual long onKeyDown (VstKeyCode &keyCode);
	virtual long onKeyUp (VstKeyCode &keyCode);

private:
	void valueChanged (CDrawContext* context, CControl* control);

	COnOffButton      *cOnOffButton;
	CKickButton       *cKickButton;
	CKnob             *cKnob;
	CMovieButton      *cMovieButton;
	CAnimKnob         *cAnimKnob;
	COptionMenu       *cOptionMenu;

	CRockerSwitch     *cRockerSwitch;
	CHorizontalSwitch *cHorizontalSwitch;
	CVerticalSwitch   *cVerticalSwitch;
	CHorizontalSlider *cHorizontalSlider;
	CHorizontalSlider *cHorizontalSlider2;
	CVerticalSlider   *cVerticalSlider;
	CTextEdit         *cTextEdit;

	CSplashScreen     *cSplashScreen;
	CMovieBitmap      *cMovieBitmap;
	CAutoAnimation    *cAutoAnimation;
	CSpecialDigit     *cSpecialDigit;
	CParamDisplay     *cParamDisplay;
	CVuMeter          *cVuMeter;

	CViewContainer    *cViewContainer;

	// others
	CLabel            *cLabel;

	long              oldTicks;
};

#endif
