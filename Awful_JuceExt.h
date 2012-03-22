#ifndef JUCEEXT_H
#define JUCEEXT_H

class CtrlPanel;
class Browser;
class Aux;
class juce::Graphics;
class juce::Font;
class juce::Button;
class Parameter;


enum input_flags
{
    mouse_left  = 1,
    mouse_right = 2,
    kbd_shift   = 4,
    kbd_ctrl    = 8,
    kbd_alt     = 16
};

typedef enum LaneType
{
    Lane_Vol = 1,
    Lane_Pan,
    Lane_Pitch,
    Lane_Usual
}LaneType;

typedef enum RefreshType
{
    Refresh_All             = 0x1,
    Refresh_Grid            = 0x2,
    Refresh_MainBar         = 0x4,
    Refresh_Mixer           = 0x8,
    Refresh_Aux             = 0x10,
    Refresh_GenBrowser      = 0x20,
    Refresh_MixCenter       = 0x40,
    Refresh_MainVBar        = 0x80,
    Refresh_Buttons         = 0x100,
    Refresh_Highlight       = 0x200,
    Refresh_InstrPanel      = 0x400,
    Refresh_KeyCursor       = 0x800,
    Refresh_MouseCursor     = 0x1000,
    Refresh_GridContent     = 0x2000,
    Refresh_AuxContent      = 0x4000,
    Refresh_Selection       = 0x8000,
    Refresh_MixHighlights   = 0x10000,
    Refresh_MainMenu        = 0x20000,
    Refresh_PianoKeys       = 0x40000,
    Refresh_GridLanes       = 0x80000,
    Refresh_AuxLanes        = 0x100000,
    Refresh_UnderPanel      = 0x200000,
    Refresh_Auxaux          = 0x400000,
    Refresh_AuxHighlights   = 0x800000,
    Refresh_SelMain         = 0x1000000,
    Refresh_SelAux          = 0x2000000,
    Refresh_AuxGrid         = 0x4000000,
    Refresh_AuxScale        = 0x8000000
}RefreshType;

typedef enum DrawObjType
{
    Draw_VertSlider,
    Draw_ZoomSlider,
    Draw_SliderHVol,
    Draw_SliderHPan,
    Draw_VU,
    Draw_Knob,
    Draw_Param,
    Draw_Timer,
    Draw_Menu,
    Draw_DigitStr,
    Draw_TString,
    Draw_Mixcell,
    Draw_Instrument,
    Draw_Highlight,
    Draw_Panel,
    Draw_Toggle,
    Draw_Numba,
    Draw_Button,
    Draw_MixChannel,
    Draw_ScrollBar
}DrawObjType;

typedef enum AuxMode
{
    AuxMode_Vols,
    AuxMode_Pans,
    AuxMode_Pattern,
    AuxMode_Mixer,
    AuxMode_Undefined
}AuxMode;


extern int		GridX1;
extern int		GridX2;
extern int		GridY1;
extern int		GridY2;
extern int		GridXS1;
extern int		GridXS2;
extern int		GridYS1;
extern int		GridYS2;
extern int		MainY1;
extern int		MainY2;
extern int		MainX1;
extern int		MainX2;

extern int      CursX;
extern int      CursY;
extern int      CursX0;

extern int      AuxX1;
extern int      AuxX2;
extern int      AuxY1;
extern int      AuxY2;

extern int		currPlayX;
extern float	OffsTick;
extern float	tickWidth;

extern bool         mixbrowse;
extern CtrlPanel*   CP;
extern Browser*     mixBrw;
extern Aux*         gAux;

extern int          CtrlPanelXRange;
extern int		    NavHeight;

extern int          MixCenterWidth;
extern int          InstrPanelWidth;
extern int          GenBrowserGap;
extern int          GenBrowserWidth;
extern int          GenBrowserHeight;
extern int          InstrPanelHeight;

extern int              keyX;
extern int              keyY;
extern int              keyW;
extern int              keyH;

extern int			    WindWidth;			// window X size
extern int			    WindHeight;			// window Y size

extern juce::Font*    	    ins;
extern juce::Font*    	    ari;
extern juce::Font*    	    ti;
extern juce::Font*    	    bignum;
extern juce::Font*    	    prj;
extern juce::Font*    	    rox;
extern juce::Font*    	    bld;
extern juce::Font*    	    fix;


extern void             ChangeAuxLane(LaneType lt);
extern void             Vanish_CleanUp();
extern void             Process_WndResize(int wx, int wh);
extern void             Process_MouseWheel(int delta, int mouse_x, int mouse_y, unsigned flags);
extern void             Process_RightButtDown(int mouse_x, int mouse_y, unsigned flags);
extern void             Process_RightButtUp(int mouse_x, int mouse_y, unsigned int flags);
extern void             Process_MouseMove(int mouse_x, int mouse_y, unsigned flags, bool left, bool right);
extern void             Process_LeftButtUp(int mouse_x, int mouse_y, bool dbclick, unsigned int flags);
extern void             Process_LeftButtDown(int mouse_x, int mouse_y, bool dbclick, unsigned flags);
extern unsigned int     TranslateKey(unsigned keycode);
extern void             Process_KeyDown(unsigned key, unsigned flags);
extern void             Process_Char(char character, unsigned flags);
extern int				RoundFloat(float val);
extern bool             AuxCheckPosData(int* posX);
extern void             J_MainScale(juce::Graphics& g);
extern void             J_Aux(juce::Graphics& g);
extern void             AuxCheck();
extern void             PlaceCursor();
extern void             J_MixCenter(juce::Graphics& g);
extern void             CheckButtonsRepaint();
extern void             CheckHighlightsRefresh();
extern bool             CheckPlaneCrossing(int x1, int y1, int x2, int y2, int a1, int b1, int a2, int b2);
extern void             CheckTempDrawareas();
extern void             RefreshSeparates();
extern void             Selection_Reset();
extern juce::Button*    createDocumentWindowButton (int buttonType);
extern void             ToFront();
extern bool             J_IsUnder();
extern void             J_UnderPanel(juce::Graphics& g);
extern void             J_UnderPanelRefresh();

extern void             SetUpRenderer();
extern bool             Render_Start();
extern void             Render_Stop();
extern void             Process_MouseLeave(int mouse_x, int mouse_y, unsigned flags);
extern void             Process_MouseEnter(int mouse_x, int mouse_y, unsigned flags);
extern void             CreateEnvelopeCommon(Parameter* param);

#endif
