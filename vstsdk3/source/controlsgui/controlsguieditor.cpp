//------------------------------------------------------------------------
//-
//- Project     : Use different Controls of VSTGUI
//- Filename    : controlsguieditor.cpp
//- Created by  : Yvan Grabit
//- Description :
//-
//- © 2000-1999, Steinberg Soft und Hardware GmbH, All Rights Reserved
//------------------------------------------------------------------------

#ifndef __controlsguieditor__
#include "controlsguieditor.h"
#endif

#ifndef __controlsgui__
#include "controlsgui.h"
#endif

#include <math.h>
#include <stdlib.h>	
#include <stdio.h>

enum
{
	// bitmaps
	kBackgroundBitmap = 10001,
	
	kSliderHBgBitmap,
	kSliderVBgBitmap,
	kSliderHandleBitmap,

	kSwitchHBitmap,
	kSwitchVBitmap,

	kOnOffBitmap,

	kKnobHandleBitmap,
	kKnobBgBitmap,

	kDigitBitmap,
	kRockerBitmap,

	kVuOnBitmap,
	kVuOffBitmap,

	kSplashBitmap,

	kMovieKnobBitmap,

	kMovieBitmap,

	// others
	kBackgroundW = 420,
	kBackgroundH = 210
};


#if MOTIF
#include "bmp001.xpm"
#include "bmp002.xpm"
#include "bmp003.xpm"
#include "bmp004.xpm"
#include "bmp005.xpm"
#include "bmp006.xpm"
#include "bmp007.xpm"
#include "bmp008.xpm"
#include "bmp009.xpm"
#include "bmp010.xpm"
#include "bmp011.xpm"
#include "bmp012.xpm"
#include "bmp013.xpm"
#include "bmp014.xpm"
#include "bmp015.xpm"
#include "bmp016.xpm"


CResTable xpmResources = {
	{kBackgroundBitmap   , bmp001},

	{kSliderHBgBitmap    , bmp002},
	{kSliderVBgBitmap    , bmp003},
	{kSliderHandleBitmap , bmp004},

	{kSwitchHBitmap      , bmp005},
	{kSwitchVBitmap      , bmp006},

	{kOnOffBitmap        , bmp007},
 
	{kKnobHandleBitmap   , bmp008},
	{kKnobBgBitmap       , bmp009},
 
	{kDigitBitmap        , bmp010},
	{kRockerBitmap       , bmp011},
	{kVuOnBitmap         , bmp012},
	{kVuOffBitmap        , bmp013},
	{kSplashBitmap       , bmp014},

	{kMovieKnobBitmap    , bmp015},
	{kMovieBitmap        , bmp016},

	{0, 0}
};
#endif

void stringConvert (float value, char* string);


//-----------------------------------------------------------------------------
// CLabel declaration
//-----------------------------------------------------------------------------
class CLabel : public CParamDisplay
{
public:
	CLabel (CRect &size, char *text);

	void draw (CDrawContext *pContext);

	void setLabel (char *text);
	bool onDrop (void **ptrItems, long nbItems, long type, CPoint &where);

protected:
	char label[256];
};

//-----------------------------------------------------------------------------
// CLabel implementation
//-----------------------------------------------------------------------------
CLabel::CLabel (CRect &size, char *text)
: CParamDisplay (size)
{
	strcpy (label, "");
	setLabel (text);
}

//------------------------------------------------------------------------
void CLabel::setLabel (char *text)
{
	if (text)
		strcpy (label, text);
	setDirty ();
}

//-----------------------------------------------------------------------------
bool CLabel::onDrop (void **ptrItems, long nbItems, long type, CPoint &where)
{
	if (nbItems > 0 && type == kDropFiles)
	{
		char text[1024];
		long pos = where.h - size.left;
		sprintf (text, "%d : %s at %d", nbItems, (char*)ptrItems[0], pos);
		setLabel (text);
	}
	return true;
}

//------------------------------------------------------------------------
void CLabel::draw (CDrawContext *pContext)
{
	pContext->setFillColor (backColor);
	pContext->fillRect (size);
	pContext->setFrameColor (fontColor);
	pContext->drawRect (size);

	pContext->setFont (fontID);
	pContext->setFontColor (fontColor);
	pContext->drawString (label, size, false, kCenterText);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ControlsguiEditor::ControlsguiEditor (AudioEffect *effect) 
	:	AEffGUIEditor (effect)
{
	frame = 0;
	oldTicks = 0;

	rect.left   = 0;
	rect.top    = 0;
	rect.right  = kBackgroundW;
	rect.bottom = kBackgroundH;

	// we decide in this plugin to open all bitmaps in the open function
}

//-----------------------------------------------------------------------------
ControlsguiEditor::~ControlsguiEditor ()
{}

//-----------------------------------------------------------------------------
long ControlsguiEditor::open (void *ptr)
{
	// always call this !!!
	AEffGUIEditor::open (ptr);

	// get version
	int version = getVstGuiVersion ();
	int verMaj = (version & 0xFF00) >> 16;
	int verMin = (version & 0x00FF);

	// init the background bitmap
	CBitmap *background = new CBitmap (kBackgroundBitmap);

	//--CFrame-----------------------------------------------
	CRect size (0, 0, background->getWidth () + 100, background->getHeight ());
	frame = new CFrame (size, ptr, this);
	frame->setBackground (background);
	background->forget ();

	CPoint point (0, 0);

	//--COnOffButton-----------------------------------------------
	CBitmap *onOffButton = new CBitmap (kOnOffBitmap);

	size (0, 0, onOffButton->getWidth (), onOffButton->getHeight () / 2);
	size.offset (20, 20);
	cOnOffButton = new COnOffButton (size, this, kOnOffTag, onOffButton);
	frame->addView (cOnOffButton);


	//--CKickButton-----------------------------------------------
 	size.offset (70, 0);
	point (0, 0);
	cKickButton = new CKickButton (size, this, kKickTag, onOffButton->getHeight() / 2, onOffButton, point);
	frame->addView (cKickButton);


	//--CKnob--------------------------------------
	CBitmap *knob   = new CBitmap (kKnobHandleBitmap);
	CBitmap *bgKnob = new CBitmap (kKnobBgBitmap);

 	size (0, 0, bgKnob->getWidth (), bgKnob->getHeight ());
	size.offset (140 + 15, 15);
	point (0, 0);
	cKnob = new CKnob (size, this, kKnobTag, bgKnob, knob, point);
	cKnob->setInsetValue (7);
	frame->addView (cKnob);
	knob->forget ();
	bgKnob->forget ();


	//--CMovieButton--------------------------------------
 	size (0, 0, onOffButton->getWidth (), onOffButton->getHeight () / 2);
	size.offset (210 + 20, 20);
	point (0, 0);
	cMovieButton = new CMovieButton (size, this, kMovieButtonTag, onOffButton->getHeight () / 2, onOffButton, point);
	frame->addView (cMovieButton);

	onOffButton->forget ();


	//--CAnimKnob--------------------------------------
	CBitmap *movieKnobBitmap = new CBitmap (kMovieKnobBitmap);

	size (0, 0, movieKnobBitmap->getWidth (), movieKnobBitmap->getHeight () / 7);
	size.offset (280 + 15, 15);
	point (0, 0);
	cAnimKnob = new CAnimKnob (size, this, kAnimKnobTag, 7, movieKnobBitmap->getHeight () / 7, movieKnobBitmap, point);
	frame->addView (cAnimKnob);
	
	movieKnobBitmap->forget ();


	//--COptionMenu--------------------------------------
	size (0, 0, 50, 14);
	size.offset (350 + 10, 30);

	long style = k3DIn | kMultipleCheckStyle;
	cOptionMenu = new COptionMenu (size, this, kOptionMenuTag, bgKnob, 0, style);
	if (cOptionMenu)
	{
		cOptionMenu->setFont (kNormalFont);
		cOptionMenu->setFontColor (kWhiteCColor);
		cOptionMenu->setBackColor (kRedCColor);
		cOptionMenu->setFrameColor (kWhiteCColor);
		cOptionMenu->setHoriAlign (kLeftText);
		int i;
		for (i = 0; i < 3; i++)
		{
			char txt[256];
			sprintf (txt, "Entry %d", i);
			cOptionMenu->addEntry (txt);
		}
		cOptionMenu->addEntry ("-");
		for (i = 3; i < 60; i++)
		{
			char txt[256];
			sprintf (txt, "Entry %d", i);
			cOptionMenu->addEntry (txt);
		}

		frame->addView (cOptionMenu);
	}


	//--CRockerSwitch--------------------------------------
	CBitmap *rocker = new CBitmap (kRockerBitmap);
 	size (0, 0, rocker->getWidth (), rocker->getHeight () / 3);
	size.offset (9, 70 + 29);
	point (0, 0);
	cRockerSwitch = new CRockerSwitch (size, this, kRockerSwitchTag, rocker->getHeight () / 3, rocker, point);
	frame->addView (cRockerSwitch);
	rocker->forget ();


	//--CHorizontalSwitch--------------------------------------
	CBitmap *switchHBitmap = new CBitmap (kSwitchHBitmap);
	size (0, 0, switchHBitmap->getWidth (), switchHBitmap->getHeight () / 4);
	size.offset (70 + 10, 70 + 30);
	point (0, 0);
	cHorizontalSwitch = new CHorizontalSwitch (size, this, kSwitchHTag, 4, switchHBitmap->getHeight () / 4, 4, switchHBitmap, point);
	frame->addView (cHorizontalSwitch);
	switchHBitmap->forget ();


	//--CVerticalSwitch--------------------------------------
	CBitmap *switchVBitmap = new CBitmap (kSwitchVBitmap);

	size (0, 0, switchVBitmap->getWidth (), switchVBitmap->getHeight () / 4);
	size.offset (140 + 30, 70 + 5);
	cVerticalSwitch = new CVerticalSwitch (size, this, kSwitchVTag, 4, switchVBitmap->getHeight () / 4, 4, switchVBitmap, point);
	frame->addView (cVerticalSwitch);
	switchVBitmap->forget ();


	//--CHorizontalSlider--------------------------------------
	CBitmap *sliderHBgBitmap = new CBitmap (kSliderHBgBitmap);
	CBitmap *sliderHandleBitmap = new CBitmap (kSliderHandleBitmap);

	size (0, 0, sliderHBgBitmap->getWidth (), sliderHBgBitmap->getHeight ());
	size.offset (10, 30);

	point (0, 0);
#if 1
	cHorizontalSlider = new CHorizontalSlider (size, this, kSliderHTag, size.left + 2, size.left + sliderHBgBitmap->getWidth () - sliderHandleBitmap->getWidth () - 1, sliderHandleBitmap, sliderHBgBitmap, point, kLeft);
	point (0, 2);
	cHorizontalSlider->setOffsetHandle (point);
#else
	CPoint handleOffset (2, 2);
	cHorizontalSlider = new CHorizontalSlider (size, this, kSliderHTag, handleOffset, size.width () - 2 * handleOffset.h, sliderHandleBitmap, sliderHBgBitmap, point, kLeft);
#endif
	cHorizontalSlider->setFreeClick (false);
	size.offset (0, -30 + 10);

	cHorizontalSlider2 = 0;
	style =  k3DIn | kCheckStyle;
	COptionMenu *cOptionMenu2 = new COptionMenu (size, this, kOptionMenuTag, bgKnob, 0, style);
	if (cOptionMenu2)
	{
		cOptionMenu2->setFont (kNormalFont);
		cOptionMenu2->setFontColor (kWhiteCColor);
		cOptionMenu2->setBackColor (kRedCColor);
		cOptionMenu2->setFrameColor (kWhiteCColor);
		cOptionMenu2->setHoriAlign (kLeftText);
		int i;
		for (i = 0; i < 3; i++)
		{
			char txt[256];
			sprintf (txt, "Entry %d", i);
			cOptionMenu2->addEntry (txt);
		}
	}

	// add this 2 control in a CViewContainer
	size (0, 0, 70, 45);
	size.offset (210, 70);
	cViewContainer = new CViewContainer (size, frame, background);
	cViewContainer->addView (cHorizontalSlider);
	cViewContainer->addView (cOptionMenu2);
	frame->addView (cViewContainer);

	sliderHBgBitmap->forget ();


	//--CVerticalSlider--------------------------------------
	CBitmap *sliderVBgBitmap = new CBitmap (kSliderVBgBitmap);

	size (0, 0, sliderVBgBitmap->getWidth (), sliderVBgBitmap->getHeight ());
	size.offset (280 + 30, 70 + 5);
#if 1
	point (0, 0);
	cVerticalSlider = new CVerticalSlider (size, this, kSliderVTag, size.top + 2, size.top + sliderVBgBitmap->getHeight () - sliderHandleBitmap->getHeight () - 1, sliderHandleBitmap, sliderVBgBitmap, point, kBottom);
	point (2, 0);
	cVerticalSlider->setOffsetHandle (point);
#else
	point (0, 0);
	CPoint handleOffset (2, 2);
	cVerticalSlider = new CVerticalSlider (size, this, kSliderVTag, handleOffset, 
		size.height () - 2 * handleOffset.v, sliderHandleBitmap, sliderVBgBitmap, point, kBottom);
#endif
	cVerticalSlider->setFreeClick (false);
	frame->addView (cVerticalSlider);

	sliderVBgBitmap->forget ();
	sliderHandleBitmap->forget ();


	//--CTextEdit--------------------------------------
	size (0, 0, 50, 12);
	size.offset (350 + 10, 70 + 30);
	cTextEdit = new CTextEdit (size, this, kTextEditTag, 0, 0, k3DIn);
	if (cTextEdit)
	{
		cTextEdit->setFont (kNormalFontVerySmall);
		cTextEdit->setFontColor (kWhiteCColor);
		cTextEdit->setBackColor (kBlackCColor);
		cTextEdit->setFrameColor (kWhiteCColor);
		cTextEdit->setHoriAlign (kCenterText);
		frame->addView (cTextEdit);
	}

	//--CSplashScreen--------------------------------------
	CBitmap *splashBitmap = new CBitmap (kSplashBitmap);

	size (0, 0, 70, 70);
	size.offset (0, 140);
	point (0, 0);
	CRect toDisplay (0, 0, splashBitmap->getWidth (), splashBitmap->getHeight ());
	toDisplay.offset (100, 50);

	cSplashScreen = new CSplashScreen (size, this, kAbout, splashBitmap, toDisplay, point);
	frame->addView (cSplashScreen);
	splashBitmap->forget ();


	//--CMovieBitmap--------------------------------------
  	CBitmap *movieBitmap = new CBitmap (kMovieBitmap);

	size (0, 0, movieBitmap->getWidth (), movieBitmap->getHeight () / 10);
	size.offset (70 + 15, 140 + 15);
	point (0, 0);	
	cMovieBitmap = new CMovieBitmap (size, this, kMovieBitmapTag, 10, movieBitmap->getHeight () / 10, movieBitmap, point);
	frame->addView (cMovieBitmap);


	//--CAutoAnimation--------------------------------------
	size (0, 0, movieBitmap->getWidth (), movieBitmap->getHeight () / 10);
	size.offset (140 + 15, 140 + 15);
	point (0, 0);
	cAutoAnimation = new CAutoAnimation (size, this, kAutoAnimationTag, 10, movieBitmap->getHeight () / 10, movieBitmap, point);
	frame->addView (cAutoAnimation);
	movieBitmap->forget ();


	//--CSpecialDigit--------------------------------------
	CBitmap *specialDigitBitmap = new CBitmap (kDigitBitmap);

 	size (0, 0, specialDigitBitmap->getWidth (), specialDigitBitmap->getHeight () / 10);
	size.offset (210 + 10, 140 + 30);

	long xpos[10];
	long ypos[10];
	for (long i = 0; i < 10; i++) 
	{
	  xpos[i] = size.left + i * (specialDigitBitmap->getWidth ());
	  ypos[i] = size.top;
	}
	cSpecialDigit = new CSpecialDigit (size, this, kDigitTag, 0, 7, xpos, ypos, specialDigitBitmap->getWidth (), specialDigitBitmap->getHeight () / 10 , specialDigitBitmap);
	frame->addView (cSpecialDigit);
	specialDigitBitmap->forget ();


	//--CParamDisplay--------------------------------------
	size (0, 0, 50, 15);
	size.offset (280 + 10, 140 + 30);
	cParamDisplay = new CParamDisplay (size);
	if (cParamDisplay)
	{
		cParamDisplay->setFont (kNormalFontSmall);
		cParamDisplay->setFontColor (kWhiteCColor);
		cParamDisplay->setBackColor (kBlackCColor);
		frame->addView (cParamDisplay);
	}


	//--CVuMeter--------------------------------------
	CBitmap* vuOnBitmap  = new CBitmap (kVuOnBitmap);
	CBitmap* vuOffBitmap = new CBitmap (kVuOffBitmap);

	size (0, 0, vuOnBitmap->getWidth (), vuOnBitmap->getHeight ());
	size.offset (350 + 30, 140 + 5);
	cVuMeter = new CVuMeter (size, vuOnBitmap, vuOffBitmap, 14);
	cVuMeter->setDecreaseStepValue (0.1f);
	frame->addView (cVuMeter);
	vuOnBitmap->forget ();
	vuOffBitmap->forget ();

	//--My controls---------------------------------
	//--CLabel--------------------------------------
	size (0, 0, 349, 14);
	size.offset (0, 140);
	cLabel = new CLabel (size, "Type a Key or Drop a file...");
	if (cLabel)
	{
		cLabel->setFont (kNormalFontSmall);
		cLabel->setFontColor (kWhiteCColor);
		cLabel->setBackColor (kGreyCColor);
		frame->addView (cLabel);
	}

	//--CLabel--------------------------------------
	size (0, 0, 65, 12);
	size.offset (1, 40);
	CLabel *cLabel2 = new CLabel (size, "FileSelector");
	if (cLabel2)
	{
		cLabel2->setFont (kNormalFontSmaller);
		cLabel2->setFontColor (kWhiteCColor);
		cLabel2->setBackColor (kGreyCColor);
		frame->addView (cLabel2);
	}

	// here we can call a initialize () function to initalize all controls values
	return true;
}

//-----------------------------------------------------------------------------
bool ControlsguiEditor::keysRequired ()
{
	if (frame && frame->getEditView ())
		return true;
	else
		return false;
}

//-----------------------------------------------------------------------------
long ControlsguiEditor::onKeyDown (VstKeyCode &keyCode)
{
	if (frame && cLabel && (keyCode.character >= 'a' && keyCode.character <= 'z'))
	{
		char val[64];
		char modifiers[32];
		strcpy (modifiers, "");
		if (keyCode.modifier & MODIFIER_SHIFT)
			strcpy (modifiers, "Shift+");
		if (keyCode.modifier & MODIFIER_ALTERNATE)
			strcat (modifiers, "Alt+");
		if (keyCode.modifier & MODIFIER_COMMAND)
			strcat (modifiers, "Cmd+");
		if (keyCode.modifier & MODIFIER_CONTROL)
			strcat (modifiers, "Ctrl+");

		sprintf (val, "onKeyDown : '%s%c'", modifiers, (char)(keyCode.character));
		cLabel->setLabel (val);
		return 1;
	}

	if (frame && (keyCode.virt == VKEY_UP || keyCode.virt == VKEY_DOWN))
	{
		CView *pView = frame->getCurrentView ();
		if (pView == cVerticalSlider || pView == cKnob || pView == cViewContainer)
		{
			CControl *control = (CControl*)pView;
			if (pView == cViewContainer)
			{
				pView = (CControl*)(cViewContainer->getCurrentView ());
				if (pView == cHorizontalSlider)
					control = (CControl*)pView;
				else
					return -1;
			}

			float inc;
			if (keyCode.virt == VKEY_UP)
				inc = 0.05f;
			else
				inc = -0.05f;
			float val = control->getValue () + inc;
			float min = control->getMin ();
			float max = control->getMax ();
			if (val > max)
				val = max;
			else if (val < min)
				val = min;
			control->setValue (val);

			return 1;
		}
	}
	return -1;
}

//-----------------------------------------------------------------------------
long ControlsguiEditor::onKeyUp (VstKeyCode &keyCode)
{
	if (cLabel && (keyCode.character >= 'a' && keyCode.character <= 'z'))
	{
		char val[64];
		char modifiers[32];
		strcpy (modifiers, "");
		if (keyCode.modifier & MODIFIER_SHIFT)
			strcpy (modifiers, "Shift+");
		if (keyCode.modifier & MODIFIER_ALTERNATE)
			strcat (modifiers, "Alt+");
		if (keyCode.modifier & MODIFIER_COMMAND)
			strcat (modifiers, "Cmd+");
		if (keyCode.modifier & MODIFIER_CONTROL)
			strcat (modifiers, "Ctrl+");

		sprintf (val, "onKeyUp : '%s%c'", modifiers, (char)(keyCode.character));
		cLabel->setLabel (val);
		return 1;
	}

	return -1; 
}

//-----------------------------------------------------------------------------
void ControlsguiEditor::resume ()
{
	// called when the plugin will be On
}

//-----------------------------------------------------------------------------
void ControlsguiEditor::suspend ()
{
	// called when the plugin will be Off
}

//-----------------------------------------------------------------------------
void ControlsguiEditor::close ()
{
	// don't forget to remove the frame !!
	if (frame)
		delete frame;
	frame = 0;

	// set to zero all pointer (security)
	cOnOffButton   = 0;
	cKickButton    = 0;
	cKnob          = 0;
	cMovieButton   = 0;
	cAutoAnimation = 0;
	cOptionMenu    = 0;

	cRockerSwitch     = 0;
	cHorizontalSwitch = 0;
	cVerticalSwitch   = 0;
	cHorizontalSlider = 0;
	cHorizontalSlider2 = 0;
	cVerticalSlider   = 0;
	cTextEdit         = 0;

	cSplashScreen = 0;
	cMovieBitmap  = 0;
	cAnimKnob     = 0;
	cSpecialDigit = 0;
	cParamDisplay = 0;
	cVuMeter      = 0;

	cViewContainer = 0;
	cLabel = 0;
}

//-----------------------------------------------------------------------------
void ControlsguiEditor::idle ()
{
	AEffGUIEditor::idle ();		// always call this to ensure update

	if (cAutoAnimation && cAutoAnimation->isWindowOpened ())
	{
		long newTicks = getTicks ();
		if (newTicks > oldTicks + 60)
		{
			cAutoAnimation->nextPixmap ();
			oldTicks = newTicks;
		}
	}
}

//-----------------------------------------------------------------------------
void ControlsguiEditor::setParameter (long index, float value)
{
	// called from the Aeffect to update the control's value

	// test if the plug is opened
	if (!frame)
		return;

	switch (index)
	{
	case kSliderHTag:
		if (cHorizontalSlider)
			cHorizontalSlider->setValue (effect->getParameter (index));
		if (cHorizontalSlider2)
			cHorizontalSlider2->setValue (effect->getParameter (index));
		break;

	case kSliderVTag:
		if (cVerticalSlider)
			cVerticalSlider->setValue (effect->getParameter (index));
 		break;

	case kKnobTag:
		if (cKnob)
			cKnob->setValue (effect->getParameter (index));
		if (cParamDisplay)
			cParamDisplay->setValue (effect->getParameter (index));
		if (cSpecialDigit)
			cSpecialDigit->setValue (1000000 * effect->getParameter (index));
		if (cVuMeter)
			cVuMeter->setValue (effect->getParameter (index));
		if (cAnimKnob)
			cAnimKnob->setValue (effect->getParameter (index));
		if (cMovieBitmap)
			cMovieBitmap->setValue (effect->getParameter (index));
		break;
	}
	
	// call this to be sure that the graphic will be updated
	postUpdate ();
}

//-----------------------------------------------------------------------------
void ControlsguiEditor::valueChanged (CDrawContext* context, CControl* control)
{
	// called when something changes in the UI (mouse, key..)
	switch (control->getTag ())
	{
	case kSliderVTag:
		// this function will called later the setParameter of ControlsguiEditor
		effect->setParameter (control->getTag (), control->getValue ());	
		effect->setParameter (kSliderHTag, control->getValue ());
		effect->setParameter (kKnobTag, control->getValue ());
		break;

	case kSliderHTag:
		effect->setParameter (control->getTag (), control->getValue ());	
		effect->setParameter (kSliderVTag, control->getValue ());
		effect->setParameter (kKnobTag, control->getValue ());
		break;

	case kKnobTag:
	{
		effect->setParameter (control->getTag (), control->getValue ());	
		effect->setParameter (kSliderVTag, control->getValue ());
		effect->setParameter (kSliderHTag, control->getValue ());
		char text[256];
		cTextEdit->getText (text);
		long v = context->getStringWidth (text);
	} break;

	case kAnimKnobTag:
		effect->setParameter (control->getTag (), control->getValue ());	
		effect->setParameter (kSliderVTag, control->getValue ());
		effect->setParameter (kSliderHTag, control->getValue ());
		effect->setParameter (kKnobTag, control->getValue ());
		break;


	// open file selector
	case kOnOffTag:
	{
		control->update (context);	

		AudioEffectX *effect = (AudioEffectX*)getEffect ();
		if (effect && control->getValue () > 0.5f)
		{
			if (effect->canHostDo ("openFileSelector"))
			{
				VstFileType aiffType ("AIFF File", "AIFF", "aif", "aiff", "audio/aiff", "audio/x-aiff");
				VstFileType aifcType ("AIFC File", "AIFC", "aif", "aifc", "audio/x-aifc");
				VstFileType waveType ("Wave File", ".WAV", "wav", "wav",  "audio/wav", "audio/x-wav");
				VstFileType sdIIType ("SoundDesigner II File", "Sd2f", "sd2", "sd2");

				VstFileSelect vstFileSelect;
				memset (&vstFileSelect, 0, sizeof (VstFileType));

				vstFileSelect.command     = kVstFileLoad;
				vstFileSelect.type        = kVstFileType;
				strcpy (vstFileSelect.title, "Test for open file selector");
				vstFileSelect.nbFileTypes = 1;
				vstFileSelect.fileTypes   = &waveType;
				vstFileSelect.returnPath  = new char[1024];
				//vstFileSelect.initialPath  = new char[1024];
				vstFileSelect.initialPath = 0;
				if (effect->openFileSelector (&vstFileSelect))
				{
					if (cLabel)
						cLabel->setLabel (vstFileSelect.returnPath);
					frame->setDropActive (true);
				}
				else
				{
					if (cLabel)
						cLabel->setLabel ("OpenFileSelector: canceled!!!!");
					frame->setDropActive (false);
				}
				delete []vstFileSelect.returnPath;
				if (vstFileSelect.initialPath)
					delete []vstFileSelect.initialPath;
			}
		}
		} break;
	
	default:
		control->update (context);	
	}
}

//-----------------------------------------------------------------------------
void stringConvert (float value, char* string)
{
	 sprintf (string, "p %.2f", value);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
