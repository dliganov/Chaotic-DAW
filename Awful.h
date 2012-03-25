//#if !defined(AFX_AWFUL_H__722DB30A_BAF1_4CAF_A33F_F6E145D206E2__INCLUDED_)
//#define AFX_AWFUL_H__722DB30A_BAF1_4CAF_A33F_F6E145D206E2__INCLUDED_
#ifndef _AWFUL_H_INCLUDED_
#define _AWFUL_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define _WIN32_WINNT 0x0500


//Find out wich platform we build for
#if (defined(_WIN32) || defined(_WIN64) || defined(_WIN32_WINNT))
  #define       USE_WIN32 1
#else
  #ifdef LINUX
    #define     USE_LINUX 1
  #else
    #define     USE_MAC 1
  #endif
#endif

//Set various defines
#define SPLASH_SCREEN TRUE
#define START_SCAN_VST TRUE

#define USE_JUCE_AUDIO

#define USE_JUCE

#define USE_OLD_JUCE

#define RELEASEBUILD TRUE


#include "sndfile.h"
#include <windows.h>
#include <portaudio.h>
#include "math.h"
#include "Awful_logger.h"
#include "winreg.h"
#include "winuser.h"
#ifdef USE_OLD_JUCE
#include "juce_amalgamated.h"
#else
#include "juce_amalgamated_NewestMerged.h"
#endif

#include "Awful_JuceComponents.h"
#include "Awful_renderer.h"

////#include "awful_sf2.h"


////////////
/// Global constants
// Grid X and Y size in cells:
#define GRID_X_SIZE       (158) 
#define GRID_Y_SIZE       (65)  

#define MAX_PREVIEW_ELEMENTS        (50)
#define MAX_GRID_ELEMENTS           (1000)
#define MAX_PATTERNS                (1000)
#define MAX_FILENAME_LENGTH         (30)
#define MAX_NAME_STRING             (256)
#define MAX_PATH_STRING             (260)
#define MAX_ALIAS_STRING            (10)

#ifndef USER_TIMER_MINIMUM
#define USER_TIMER_MINIMUM			(0)
#endif

#define CURSOR_TIMER                (1)
#define CURSOR1_TIMER               (2)
#define PLAYBACK_TIMER              (3)
#define DBCLICK_TIMER               (USER_TIMER_MINIMUM + 4)
#define HINT_TIMER                  (USER_TIMER_MINIMUM + 5)
#define IDLE_EFFECT_TIMER           (USER_TIMER_MINIMUM + 6)
#define SCROLL_TIMER                (USER_TIMER_MINIMUM + 7)

#define MAX_SAMPLENAME_LENGTH       (100)
#define MAX_PATTERNNAME_LENGTH      (100)
#define MAX_SAMPLES                 (100)
#define MAX_PARAMS                  (100)
#define MAX_TRACKS                  (1000)
#define MAX_PERCENT_INTEGER_DIGITS  (3)
#define MAX_PERCENT_FLOAT_DIGITS    (3)
#define MAX_SCOPES                  (1)
#define MAX_BUFF_SIZE               (16896) // this max buffer is equal to max samples
#define MAX_PARAM_NAME              (100)
#define MAX_PRESET_NAME             (256)
#define DECLICK_COUNT               (128)    // should not be greater than 256 or too small
#define RAMP_COUNT                  (128)
#define DEFAULT_BUFFER_SIZE         (2048)

#define NUM_MIXCHANNELS             (32)
#define NUM_MIXER_COLUMNS           (1)
#define NUM_MIXER_ROWS              (1)
#define NUM_MASTER_CELLS            (1)
#define NUM_PATTERN_TRACKS          (120)
#define NUM_PIANOROLL_LINES         (120)

#define ENVPOINT_TOUCH_RADIUS       (5)
#define LOCAL_PLUGIN_FOLDER  ("Plugins\\")
#define USER_SAVED_PRESET_PATH         ("Presets\\")
#define SAMPLES_PATH                   ("Samples\\")
#define PROJECTS_PATH                  ("Projects\\")
#define RENDERING_DEFAULT_FOLDER       ("Rendered\\")
#define VST_EXT_PATH_1                 ("C:\\Program Files\\Steinberg\\VstPlugins\\")
#define VST_EXT_PATH_2                 ("C:\\Program Files\\VstPlugins\\")
#define PLUGIN_LIST_FILENAME           ("vst_fxlist.lst")
#define MITEM_HEIGHT                (18)
#define DIVIDER_HEIGHT              (10)
#define PCSLIDERHEIGHT              (21)

#define BrwEntryHeight              (14)

#define CLEANUP_MESSAGE             (0x8001)

#define StepSeqLineHeight           (15)

#define ON							(1)
#define OFF							(0)

#define PI_F						(3.14159265f)

// for 6 dB max
//#define VolRange                  (1.41253754f)
//#define InvVolRange               (0.70794578f)

// for 3 dB max
//#define VolRange                  (1.18850222f)
//#define InvVolRange               (0.84139514f)

// for 4 dB max
//#define VolRange                  (1.25892541f)
//#define InvVolRange               (0.79432823f)

// for 6 dB max
#define     VolRange                    (1.41421356f)
#define     InvVolRange                 (0.70710678f)

#define     LATENCY_MSEC                (2000)
#define     SAMPLE_RATE                 (44100)
//#define     FRAMES_PER_BUFFER           (paFramesPerBufferUnspecified)

#define     MAX_TXTBUFF_LEN     250

#define	    Note_Font_Height    8
#define     SlideHeight         6
#define     SmpHeight           6
#define     EnvHeading          9

#define	    Sample_Font_Height   11.0
#define	    Params_Font_Height   10.0

#define     HINT_TIMER_INTERVAL (300) //0.3 second

#define     WT_SIZE             32768

//This macros makes fourCC code
#define MAKE_FOURCC(a,b,c,d) \
     ((((long)a) << 24) | (((long)b) << 16) | (((long)c) << 8) | (((long)d) << 0))

class Playback;

class Object;
class Eff;
class Gain;
class Send;
class CFilter;
class CFilter3;
class XDelay;
class CReverb;
class CChorus;

class Element;
class Instance;
class Gennote;
class Samplent;
class Slide;
class Muter;
class Txt;
class Reverse;
class SlideNote;
class Pattern;
class Command;
class Break;
class Vibrate;
class Repeat;
class Transpose;
class Frequencer;

class Semitones;
class Note;
class PEdit;
class Percent;
class Digits;
class DigitStr;
class Envelope;
class TString;
class ValueString;

class Control;
class ScrollBard;
class SliderBase;
class Butt;
class ButtFix;
class Knob;
class SliderBase;
class SliderHVol;
class SliderHPan;
class SliderParam;
class OutPin;
class InPin;
class InPin_Bus;
class MenuItem;
class Menu;
class Numba;
class Aux;

class Panel;
class CtrlPanel;
class Browser;
class InstrPanel;
class Mixcell;
class Mixer;

class ParamSet;
class Parameter;
class Preset;
class Vol;
class Pan;
class Len;
class BoolParam;
class Level;
class FrequencyParameter;

class Toggle;
class Event;

class Instrument;
class Sample;
class Gen;
class Sine;
class Osc;
class Saw;
class Synth;

class Trk;
class Lane;
class Pianoroll;
class StaticArea;

class VSTGenerator;

/* VST Specific classes declaration */
class VSTCollection;
class CVSTPlugin;
class VSTEffect;
/************************************/
class CPluginList;
class Renderer;
class AwfulMidiWrapper;
class AwfulMidiInputCallback;

class VU;
class Paramcell;

// Structs
class Trigger;
struct PreviewSlot;
struct TrkBunch;

class AwfulWindow;
class ChildWindow;
class ConfigWindow;
class RenderWindow;
class SampleWindow;
class SynthWindow;

class SynthComponent;

class Cursor;
class Mouse;

class UndoManaga;
class MixChannel;
class ProjectData;
class ParamModule;
class ScanThread;

class LogParam;

class ProjectData
{
public:
    bool    newproj;
    char    projname[MAX_NAME_STRING];
    //char    projpath[MAX_PATH_STRING];
    String  projpath;
    File*   file;

    void Init();
    void SetName(String name);
};

typedef short int16;        //----int16
typedef long tframe;

/*
enum input_flags
{
    mouse_left  = 1,
    mouse_right = 2,
    kbd_shift   = 4,
    kbd_ctrl    = 8,
    kbd_alt     = 16
};*/

enum keys
{
    // ASCII set. Should be supported everywhere
    key_backspace      = 8,
    key_tab            = 9,
    key_clear          = 12,
    key_return         = 13,
    key_pause          = 19,
    key_escape         = 27,

    // Keypad 
    key_delete         = 127,
    key_kp0            = 256,
    key_kp1            = 257,
    key_kp2            = 258,
    key_kp3            = 259,
    key_kp4            = 260,
    key_kp5            = 261,
    key_kp6            = 262,
    key_kp7            = 263,
    key_kp8            = 264,
    key_kp9            = 265,
    key_kp_period      = 266,
    key_kp_divide      = 267,
    key_kp_multiply    = 268,
    key_kp_minus       = 269,
    key_kp_plus        = 270,
    key_kp_enter       = 271,
    key_kp_equals      = 272,

    // Arrow-keys and stuff
    key_up             = 273,
    key_down           = 274,
    key_right          = 275,
    key_left           = 276,
    key_insert         = 277,
    key_home           = 278,
    key_end            = 279,
    key_page_up        = 280,
    key_page_down      = 281,

    // Functional keys. You'd better avoid using
    // f11...f15 in your applications if you want 
    // the applications to be portable
    key_f1             = 282,
    key_f2             = 283,
    key_f3             = 284,
    key_f4             = 285,
    key_f5             = 286,
    key_f6             = 287,
    key_f7             = 288,
    key_f8             = 289,
    key_f9             = 290,
    key_f10            = 291,
    key_f11            = 292,
    key_f12            = 293,
    key_f13            = 294,
    key_f14            = 295,
    key_f15            = 296,

    // The possibility of using these keys is 
    // very restricted. Actually it's guaranteed 
    // only in win32_api and win32_sdl implementations
    key_numlock        = 300,
    key_capslock       = 301,
    key_scrollock      = 302,

    // Phew!
    end_of_key_codes
};

typedef enum
{
    CMode_TxtDefault,
    CMode_ElemEdit,
    CMode_ElemParamEdit,
    CMode_JustParamEdit,
    CMode_FastMode,
    CMode_FastBrushMode,
    CMode_NotePlacing,
    CMode_Sliding,
    CMODE_UNKNOWN = 0xFFFF
}CursMode;

typedef enum
{
    CType_UnderVert,
    CType_Vertical,
    CType_Underline,
    CType_InstanceNote,
    CType_SlideNote,
    CType_Quad,
    CType_None
}CursType;

typedef enum MouseMode
{
    MOUSE_ON_GRID                   = 0x1,	        // Normal arrow mode
    MOUSE_GRID_X1	                = 0x2,          // Resizing left grid border
    MOUSE_GRID_X2                   = 0x4,          // Resizing right grid border
    MOUSE_GRID_Y1                   = 0x8,	        // Resizing upper grid border
    MOUSE_GRID_Y2                   = 0x10,	        // Resizing lower grid border (between grid and Aux).
    MOUSE_SELECTING                 = 0x20,
    MOUSE_RESIZE                    = 0x40,
    MOUSE_ENVELOPING                = 0x80,
    MOUSE_CONTROLLING               = 0x100,
    MOUSE_AUXING                    = 0x200,
    MOUSE_LINING                    = 0x400,
    MOUSE_INSTRUMENTING             = 0x800,
    MOUSE_UPPER_TRACK_EDGE          = 0x1000,
    MOUSE_LOWER_TRACK_EDGE          = 0x2000,
    MOUSE_BROWSING                  = 0x4000,
    MOUSE_ON_GENBROWSER_EDGE        = 0x8000,
    MOUSE_DRAG_N_DROPPING           = 0x10000,
    MOUSE_MIXING                    = 0x20000,
    MOUSE_TRACKING                  = 0x40000,
    MOUSE_LANEAUXING                = 0x80000,
    MOUSE_BRUSHING                  = 0x100000,
    MOUSE_LOWAUXEDGE                = 0x200000,
    MOUSE_DELETING                  = 0x400000,
    MOUSE_AUXAUXING                 = 0x800000,
    MOUSE_AUXMIXING                 = 0x1000000,
    MOUSE_BPMING                    = 0x2000000,
    MOUSE_GRID_X22                  = 0x4000000,
    MOUSE_SLIDING                   = 0x8000000,
    MOUSE_LOOPING                   = 0x10000000,
    MOUSE_ON_MIXBROWSER_EDGE        = 0x20000000
}MouseMode;

typedef enum BorderType
{
    BorderType_GRIDX1,
    BorderType_GRIDX2,
    BorderType_GRIDY1,
    BorderType_GRIDY2,
    BorderType_GRIDX22
}BorderType;

typedef enum ParamType
{
    Param_Pan,
    Param_Vol,
    Param_Pitch,
    Param_Len,
    Param_Note,
    Param_Semitones,
    Param_Env,
    Param_TString,
    Param_FX,
    Param_Frequency,
    Param_Bool,
    Param_Default,
    Param_Default_Bipolar
}ParamType;

typedef enum InstrType
{
    Instr_Sample,
    Instr_Generator,
    Instr_Synth,
    Instr_SoundFont,
    Instr_VSTPlugin
}InstrType;

typedef enum ElType
{
    El_Pattern,
    El_TextString,
    El_Samplent,
    El_Gennote,
    El_Command,
    El_Mute,
    El_Break,
    El_SlideNote,
    El_Slider,
    El_Reverse,
    El_Repeat,
    El_Vibrate,
    El_Transpose,
    EL_UNKNOWN = 0xFFFF
}ElType;

typedef enum Symbol
{
    Symbol_Mute,
    Symbol_Break,
    Symbol_Slider,
    Symbol_Repeat,
    Symbol_Reflect,
    Symbol_Vibrate,
    Symbol_Volume,
    Symbol_Panning,
    Symbol_Local,
    Symbol_Track,
    Symbol_Command,
    Symbol_Transpose,
    Symbol_Bookmark,
    SYMBOL_UNKNOWN = 0xFFFF
}Symbol;

typedef enum EffType
{
    EffType_Default = 1,
    EffType_Gain,
    EffType_Send,
    EffType_Filter,
    EffType_CFilter,
    EffType_CFilter2,
    EffType_CFilter3,
    EffType_Equalizer1,
    EffType_Equalizer3,
    EffType_GraphicEQ,
    EffType_XDelay,
    EffType_Reverb,
    EffType_Tremolo,
    EffType_Compressor,
    EffType_Chorus,
    EffType_Flanger,
    EffType_Phaser,
    EffType_WahWah,
    EffType_BitCrusher,
    EffType_Distortion,
    EffType_Synth1,
    EffType_Stereo,
    EffType_VSTPlugin = 100
}EffType;

typedef struct AliasRecord
{
    EffType        type;
    char            alias[MAX_NAME_STRING];

    AliasRecord*    prev;
    AliasRecord*    next;
}AliasRecord;

typedef enum
{
    PState_Ready,
    PState_Playing,
    PState_Stopped,
    PState_Inactive,
    PState_Free
}PrevState;

typedef enum
{
    TgState_Initial,
    TgState_Sustain,
    TgState_Release,
    TgState_SoftFinish,
    TgState_Finished
}TgState;

typedef enum
{
    Loc_UNKNOWN,
    Loc_MainGrid,
    Loc_SmallGrid,
    Loc_StaticGrid,
    Loc_Other
}Loc;

typedef enum CmdType
{
    Cmd_Default,
    Cmd_Vol,
    Cmd_Pan,
    Cmd_Mute,
    Cmd_Unmute,
    Cmd_Solo,
    Cmd_Unsolo,
    Cmd_Bypass,
    Cmd_Unbypass,
    Cmd_Len,
    Cmd_Offs,
    Cmd_Envelope,
    Cmd_VolEnv,
    Cmd_PanEnv,
    Cmd_LocVolEnv,
    Cmd_LocPanEnv,
    Cmd_PitchEnv,
    Cmd_ParamEnv
}CmdType;

typedef enum EnvType
{
    Env_Volume,
    Env_Panning,
    Env_Pitch,
    Env_Special
}EnvType;

typedef enum ResizeElem
{
    Resize_GridElem,
    Resize_AuxPattRange,
    Resize_Envelope,
    Resize_Waveform,
    Resize_Lane
}ResizeElem;

typedef enum ToggleType
{
    Toggle_Default,
    Toggle_Mute,
    Toggle_Mute1, // Small size
    Toggle_Solo,
    Toggle_Solo1, // Small size
    Toggle_AutoPatt,
    Toggle_MixBypass,
    Toggle_Window,
    Toggle_EnvFold,
    Toggle_EnvTime,
    Toggle_AutoSwitch,
    Toggle_RelSwitch,
    Toggle_EffFold,
    Toggle_EffBypass,
    Toggle_EffWindow
}ToggleType;

typedef enum NumbaType
{
    Numba_BPM,
    Numba_TPB,
    Numba_BPB,
    Numba_Octave,
    Numba_relOctave
}NumbaType;

typedef enum SelMode
{
    Sel_Usual,
    Sel_Tracks,
    Sel_Pattern,
    Sel_Fragment
}SelMode;

typedef enum BrushMode
{
    Brush_Default,
    Brush_Pattern,
    Brush_Envelope
}BrushMode;

typedef enum EnvAction
{
    ENVUSUAL,       // Usual hovering
    ENVPOINTING,    // Mouse on point (moving or hovering)
    ENVSUSTAINING,  // Sustain position changing
    ENVDELETING,    // Points deletion
    ENVDRAWMOVE,    // Freehand draw
    ENVLINEMOVE     // Freehand line draw
}EnvAction;

typedef enum DragType
{
    Drag_Instrument_Alias,
    Drag_MixCell_Content,
    Drag_MixCell_Indeks,
    Drag_MixChannel_Indeks,
    Drag_Instrument_File,
    Drag_Effect_File,
    Drag_Command,
    Drag_MixChannel_Effect
}DragType;

typedef enum
{
    LEFT_X,
    RIGHT_X,
    TOP_Y,
    BOTTOM_Y
}RectEdge;

typedef enum
{
    Ctrl_Slider,
    Ctrl_Knob,
    Ctrl_Snob,
    Ctrl_Button,
    Ctrl_ButtFix,
    Ctrl_HScrollBar,
    Ctrl_VScrollBar,
    Ctrl_Toggle,
    Ctrl_Fold,
    Ctrl_InPin_Point,
    Ctrl_OutPin_Point,
    Ctrl_InPin_Bus,
    Ctrl_Menu,
    Ctrl_MenuItem,
    Ctrl_MenuDivider
}ControlType;

typedef enum
{
    Panel_Main,
    Panel_Instruments,
    Panel_Mixer,
    Panel_MixCell,
    Panel_Browser,
    Panel_Window,
    Panel_Aux,
    Panel_List,
    Panel_Static,
    Panel_Number,
    Panel_MixChannel
}PanelType;

typedef enum
{
    ParamEdit_Loop,
    ParamEdit_Open,
    ParamEdit_Block,
    ParamEdit_BlockLeft,
    ParamEdit_BlockRight
}ParamEditMode;

typedef enum LenType
{
    Len_Percent,
    Len_Ticks,
    Len_Seconds
}LenType;

typedef enum
{
    Patt_Grid,
    Patt_Pianoroll,
    Patt_StepSeq
}PattType;

typedef struct DragData
{
    DragType    drag_type;
    void*       drag_stuff;
    int         instr_len;
    float       instr_vol;
    float       instr_pan;
    int         drag_len;
    int         drag_index;
    int         dragcount;
    bool        tsop;
}DragData;

typedef struct Area
{
    int     x1;
    int     y1;
    int     x2;
    int     y2;
}Area;

typedef struct Scope
{
    Pattern*        pt;
    Instrument*     instr;
    bool            for_track;
    Trk*            trkdata;
    Mixcell*        mixcell;
    bool            local;
    Eff*            eff;
}Scope;

typedef struct Vibe
{
    float   freq;
    float   range;
}Vibe;

typedef union comm_val
{
    float   n_vol;
    float   n_len;
    float   n_offs;
    float   n_pan;
    Vibe    vib;
}comm_val;

typedef enum VStrType
{
    VStr_Hz,
    VStr_fHz,
    VStr_fHz1,
    VStr_kHz,
    VStr_Percent,
    VStr_beat,
    VStr_second,
    VStr_msec,
    VStr_msec2,
    VStr_dB,
    VStr_oct,
    VStr_String,
    VStr_Integer,
    VStr_Default,
    VStr_Beats,
    VStr_Semitones,
    VStr_DryWet
}VStrType;

typedef union VStrVal
{
    float   hz;
    float   n_val;
    int     percent;
    int     integer;
    float	dB;
    float	ms;
    float	sec;
    float	semitones;
    float	beats;
    char    str[100];
}VStr_val;

typedef struct EnvPnt
{
    float   x;
    float   y_norm;
    float   cff;
    bool    deleted;
    bool    suspoint;
    bool    big;
    EnvPnt* next;
    EnvPnt* prev;
}EnvPnt;

typedef union len_val
{
    int     percent;
    float   ticks;
    int     sec;
    int     beats;
    int     times;
    long    frames;
}len_val;

typedef enum FType
{
    FTYPE_UNKNOWN           = 0x1,
    FType_Directory         = 0x2,
    FType_System            = 0x4,
	FType_Wave              = 0x8,
    FType_MP3               = 0x10,
    FType_OGG               = 0x20,
    FType_SoundFont         = 0x40,
    FType_DXiPlugins        = 0x80,
    FType_VST               = 0x100,
    FType_Native            = 0x200,
    FType_Delimiter         = 0x400,
	FType_Projects          = 0x800,
    FType_LevelDirectory    = 0x1000,
    FType_DiskSelector      = 0x2000,
    FType_DiskDrive         = 0x4000
}FType;

typedef enum BEType
{
    BEType_Common,
    BEType_Delimiter
}BEType;

typedef enum EType
{
    Etype_Usual,
    EType_Subtitle,
    ETYPE_UNKNOWN
}EType;

typedef enum DirType
{
    Dir_Nodir,
    Dir_Usual,
    Dir_Level,
    Dir_Selector,
    Dir_Drive,
    Dir_Files,
    Dir_VST
}DirType;

typedef enum MItemType
{
    MItem_Divider,
    MItem_CreateAutomation,
    MItem_MountPianoRoll,
    MItem_UnmountPianoRoll,
    MItem_DrawPicture,
    MItem_Load,
    MItem_Delete,
    MItem_ShowVolumeLane,
    MItem_ShowPanLane,
    MItem_HideVolumeLane,
    MItem_HidePanLane,
    MItem_AddElement,
    MItem_ShowDefinition,
    MItem_DeleteDefinition,
    MItem_DeletePreset,
    MItem_DeleteProject,
    MItem_OpenProject,
    MItem_ReScanFast,
    MItem_ReScanFull,
    MItem_File,
    MItem_Edit,
    MItem_View,
    MItem_Help,
    MItem_New,
    MItem_Open,
    MItem_Save,
    MItem_SaveAs,
    MItem_SaveNewVersion,
    MItem_Render,
    MItem_Exit,
    MItem_Custom,

    MItem_CreateUsualPattern,
    MItem_CreatePianorollPattern,
    MItem_CreatePianorollPatternForInstrument,
    MItem_BindInstrumentToCurrentPattern,

    MItem_CreateEnvelope,
    MItem_CreateCommand,
    MItem_ResetControl,

    MItem_EditAutoPattern,
    MItem_DeleteInstrument,
    MItem_CloneInstrument,
    MItem_BindInstrument,
    MItem_DedicatePianoRoll,

    MItem_HideAll,
    MItem_ShowAll,
    MItem_FullScreen,

    MItem_Undo,
    MItem_Redo,
    MItem_LoadInstrumentPlugin,
    MItem_LoadEffectPlugin,
    MItem_LoadProject,
    MItem_LoadSample,
    MItem_InsertTrack,
    MItem_DeleteTrack,
    MItem_CleanSong,
    MItem_Preferences,

    MItem_Contents,
    MItem_HotKeys,
    MItem_About,

    MItem_None,
    MItem_Step16,
    MItem_Step13,
    MItem_Step14,
    MItem_Step12,
    MItem_Step,
    MItem_Beat,
    MItem_Bar,

    MItem_Cut,
    MItem_Copy,
    MItem_Paste,
    MItem_SelectAll,
    MItem_Unbunch,
    MItem_Unbind,

    MItem_LoadAsInstrument,

    MItem_CleanMixCell,
    MItem_DeleteEffect,
    MItem_SetMixCellToGain,
    MItem_SetMixCellToSend,
    MItem_SetMixCellToEQ1,
    MItem_SetMixCellToEQ3,
    MItem_SetMixCellToGraphicEQ,
    MItem_SetMixCellToBlank,
    MItem_SetMixCellToDelay,
    MItem_SetMixCellToTremolo,
    MItem_SetMixCellToFilter,
    MItem_SetMixCellToCompressor,
    MItem_SetMixCellToReverb,
    MItem_SetMixCellToChorus,
    MItem_SetMixCellToFlanger,
    MItem_SetMixCellToPhaser,
    MItem_SetMixCellToWahWah,
    MItem_SetMixCellToDistortion,
    MItem_SetMixCellToBitCrusher,
    MItem_SetMixCellToStereoizer,
    MItem_SetMixCellToFilter2,
    MItem_SetMixCellToFilter3,
    MItem_SavePreset,
    MItem_LoadPreset,
    MItem_ViewParams
}MItemType;

typedef struct FileData
{
	char		name[MAX_NAME_STRING];
	long		size;
    FType       ftype;
    DirType     dirtype;

    DWORD       attrs;
    int         list_index;
    char        path[MAX_PATH_STRING];
}FileData;

typedef struct PresetData
{
    char    name[MAX_NAME_STRING];
    long    index;
    int     list_index;
    Preset* pPreset;
}PresetData;

typedef struct BrwEntry
{
    char        rame[MAX_NAME_STRING];
    void*       entry;
    BEType      type;

    BrwEntry*   prev;
    BrwEntry*   next;
}BrwEntry;

typedef enum BrwType
{
    Brw_Generators,
    Brw_Effects,
    Brw_None
}BrowserType;

typedef struct Entry
{
    char        name[MAX_PARAM_NAME];
    EType       type;
    int         list_index;

    Entry*      prev;
    Entry*      next;
    /* !!!Be careful: it is an union, so you can't have all the fileds enabled at the same time */
    union
    {
        int    int_val; /* e.g. device index */
        float  fval; /* e.g. sampling rate */
    }val;
}Entry;

typedef struct Buff
{
    float       data_buff[MAX_BUFF_SIZE*2];
    float       pan_buff[MAX_BUFF_SIZE];
}Buff;

typedef enum TrkType
{
    Trk_Usual,
    Trk_Pianoroll,
    Trk_Bunched,
    Trk_Buncho,
    Trk_Bunchito
}TrkType;

typedef enum BrwMode
{
    Browse_Files,
    Browse_Params,
    Browse_Presets,
    Browse_Projects,
    Browse_Plugins,
    Browse_Native,
    Browse_Samples
}BrwMode;

typedef struct Master
{
    ParamSet*   params;
    Trigger*    efftrig;
    Command*    envelopes;
    float       freq_mult;

    float       rampCounterV;
    float       cfsV;

}Master;

typedef enum Interpol_method
{
    Interpol_linear = 1,
    Interpol_3dHermit,
    Interpol_6dHermit,
    Interpol_64Sinc,
    Interpol_128Sinc,
    Interpol_256Sinc
}Interpol_method;

typedef void (*CLICK_HANDLER_FUNC)(Object*, Object*);

typedef enum EFFECT_CATEGORY_T
{
    EffCategory_Generator,
    EffCategory_Synth,
    EffCategory_Effect,
    EffCategory_Invalid
}EFFECT_CATEGORY_T;

typedef struct EffListEntry_t
{
    char              name[MAX_NAME_STRING];
    char              path[MAX_PATH_STRING];
    EFFECT_CATEGORY_T category;
    EffType           type;
    EffListEntry_t*   Prev;
    EffListEntry_t*   Next;
}EffListEntry_t;

typedef struct EffPresetHeader_t
{
    char      magic_string[32];
    EffType   effect_type;
    long      uniqueID;
    char      name[MAX_NAME_STRING];
    char      fx_name[MAX_NAME_STRING];
}EffPresetHeader_t;

typedef enum MenuType
{
    Menu_Context,
    Menu_Main,
    Menu_Quant,
}MenuType;

typedef enum ParamcellType
{
    PCType_BrwSlider,
    PCType_MixSlider,
    PCType_MixRadio,
    PCType_MixToggle,
    PCType_Custom
}ParamCellType;

class FXState
{
public:
    int         t_mutecount;
    bool        t_reversed;
    bool        t_break;
    float       t_freq_mult;
    Trigger*    freqtrig;

    FXState()
    {
        t_mutecount = 0;
        t_reversed = false;
        t_break = false;
        freqtrig = NULL;
        t_freq_mult = 1;
    }
};

///////////////////////////////////
// Frequency multipliers 
const double freq_mul_table[13] = {1, 1.059463094, 1.122462048, 1.189207115, 1.25992105, 1.334839854, 1.414213562, 1.498307077, 1.587401052, 1.68179283, 1.781797436, 1.887748625, 2};
const char note_table[][12] = {"C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-"};
const char WavExt[4] = "wav";
const char VstExt[4] = "dll";
const char ProjExt[4] = "cmm";

///////////////////////////////////
// Wavetables
extern float   wt_sine[];
extern float   wt_cosine[];
extern float   wt_saw[];
extern float   wt_triangle[];

///////////////////////
// Cursor variables
extern int          CursX;
extern int          CursY;
extern int          CursorStart;
extern int          CursorLen;
extern int          CLength;

extern Cursor       C;
extern Mouse        M;

extern CtrlPanel*   CP;

////////////////////////////////////
// Grid position relative to the window in pixels:
extern int          MainY1;
extern int          MainY2;
extern int          MainX1;
extern int          MainX2;

extern int          GridX1;
extern int          GridX2;
extern int          GridY1;
extern int          GridY2;

extern int          AuxX1;
extern int          AuxX2;
extern int          AuxY1;
extern int          AuxY2;

extern int          WindWidth;			// window X size
extern int          WindHeight;			// window Y size
extern int		    NavHeight;
extern int          AuxHeight;

extern int          CursX;
extern int          CursY;
extern int          CursX0;

extern float        CTick;			// Current text cursor X position in ticks
extern int          CLine;			// Current text cursor Y position in tracks

extern int          OffsLine;		// first visible track on the grid
extern float        OffsTick;		// first visible tick (tick offset) on the grid

// Baked selection rectangle coordinates. -1 means there's no selection rectangle
extern int          SelX1; 
extern int          SelY1;
extern int          SelX2;
extern int          SelY2;

extern int          LooX1; 
extern int          LooX2;

extern int          baseNote;

extern Instrument*      instr[100];
extern Element*			firstElem;
extern Element*         lastElem;
extern bool				Playing; // Global flag indicating whether playback is going on
extern bool             Rendering; // Global flag indicating whether rendering is going on
extern Master		    mAster;
extern float            fSampleRate; // global variable holding current sample rate

extern Pattern*         field;
extern Control*         firstCtrl;
extern Control*         lastCtrl;
extern Mixer*			mix;
extern int				CtrlPanelHeight;
extern int              CtrlPanelWidth;
extern int              MixMasterHeight;
extern int              MixCellWidth;
extern int              MixerWidth;
extern int              MixCellBasicHeight;
extern int              MixCellGap;
extern int              MixCellBasicHeight;

extern int	            currPlayX;	    // —четчик проигранных фреймов
extern double           currPlayX_f;

extern float            framesPerPixel;	    // —четчик проигранных фреймов

extern HWND             hWnd;   // —охранение дискриптора окна

extern float            ring[1024];
extern int              ringPos;
extern int              ringSize;

extern Element*         global_eff_elements;

extern int              num_instrs;
extern float            one_divided_per_sample_rate;
extern float            one_divided_per_frames_per_tick;      // 1 divided on seconds_in_tick (== ticks per second)

extern Trigger*         first_active_pattern_trigger;
extern Trigger*         last_active_pattern_trigger;

extern Command*         first_active_command;
extern Command*         last_active_command;

extern Parameter*       first_rec_param;
extern Parameter*       last_rec_param;

extern Eff*             first_eff;
extern Eff*             last_eff;

extern int              GenBrowserWidth;
extern int              GenBrowserHeight;
extern Instrument*      current_instr; //Pointer to the currently highlighted instrument on instrument panel

extern int              st_tickWidth;

extern float			tickWidth;
extern int              lineHeight;

extern float		    tickWidthBack;

extern Trk*             first_trkdata;
extern Trk*             last_trkdata;
extern Trk*             top_trk;
extern Trk*             bottom_trk;

//Generator's browser. This one is displayed in the most left side of the main window
extern Browser*         genBrw;
//FX browser. This one holds the most right position.
extern Browser*         mixBrw;

extern ScrollBard*      main_bar;
extern ScrollBard*      main_v_bar;

//Pointer to the first instrument in the linked list of all currently loaded instruments in the instrument panel
//Together with last_instr it makes linked list of all instruments
extern Instrument*      first_instr;
extern Instrument*      last_instr;

extern PEdit*           firstPE;
extern PEdit*           lastPE;

extern bool             static_visible;
extern Instrument*      overriding_instr;

extern int              curr_min;
extern int              curr_sec;
extern int              curr_msec;
extern int              total_min;
extern int              total_sec;
extern int              CtrlAdvance;
extern int              TranspAdvance;

extern int              InstrCellWidth;
extern int              InstrPanelWidth;
extern int              InstrPanelHeight;
extern int              InstrFoldedHeight;
extern int              InstrUnfoldedHeight;

extern InstrPanel*      IP;
extern Pianoroll*       proll_first;
extern Pianoroll*       proll_last;

extern int              NavHeight;
extern int              LinerHeight;
extern int              AuxHeight;
extern int              AuxKeysHeight;

extern int              keys_offset;
extern int              keys_width;

extern int              brw_heading;

extern int              GenBrowserWidth;
extern int              GenBrowserHeight;

extern int              EffBrowserPos;
extern int              EffBrowserWidth;
extern int              EffBrowserCurrentWidth;
extern int              EffBrowserHeight;

extern HDC              hDC;
extern HGLRC            hRC;

extern float            seconds_in_tick;
extern Element*         delList[];
extern int              delCount;
extern AliasRecord*     first_alias_record;

extern int              Octave;
extern int              relOctave;
extern float            beats_per_minute;
extern float            beats_per_minute_temp;
extern int				beats_per_bar;
extern int				ticks_per_beat;
extern float            frames_per_tick;

extern Panel*           firstPanel;
extern Panel*           lastPanel;

extern Aux*             gAux;

extern Playback*        pbMain;
extern Playback*        pbAux;


//Pointer to list of all found plugins of any kind (including internal FXes or generators)
//The list keeps plugin DLL path, name, type...
extern CPluginList*     pPluginList;

//Pointer to a kind of VST plugin factory. This class is responsible for instantinating of VST plugin objects
extern VSTCollection*   pVSTCollector;
//This global parameter holds sample buffer lenght used by renderer and input/output devices
extern int              gBuffLen;
extern HWND             gHwnd;

//Global pointer to instance of Rendering module. This module provides set of interfaces to configure rendering parameters
//Once started, renderer will request chunks of samples by invoking callback routine provided by main app.
extern Renderer*        pRenderer;

extern AwfulMidiWrapper *pMidiHost;

extern int              GridXS1;
extern int              GridXS2;
extern int              GridYS1;
extern int              GridYS2;

extern float            quantsize;
extern float            voltable[200];
extern int              vtmax;
extern StaticArea*      st;

extern int              StX1;
extern int              StX2;

extern int              InstrAliasOffset;

extern int              refresh_rate;
extern int              refresh_counter;

extern Trigger*         first_active_samplent_trigger;
extern Trigger*         last_active_samplent_trigger;

extern Trigger*         first_active_longnote_trigger;
extern Trigger*         last_active_longnote_trigger;

extern int              GenBrowserGap;

extern Trigger*         first_active_command_trigger;
extern Trigger*         last_active_command_trigger;

extern Trigger*         first_active_effect_trigger;
extern Trigger*         last_active_effect_trigger;

extern Trigger*         first_global_active_trigger;
extern Trigger*         last_global_active_trigger;

extern Instrument*      InstrSolo;
extern Trk*             TrkSolo;
extern Mixcell*         MixcellSolo;
extern MixChannel*      MixChannelSolo;

extern bool             scrolling;
extern Butt*            scrollbt;

extern int              curr_bar;
extern int              curr_beat;
extern float            curr_step;

extern bool             auto_advance;
extern float            wt_coeff;

extern bool             mixbrowse;
extern int              MixCenterWidth;
extern int              DefaultMixCenterWidth;

extern MainComponent*   MC;

extern int              CtrlPanelXRange;

extern int              MixX;
extern int              MixY;
extern int              MixW;
extern int              MixH;

extern int              mixX;
extern int              mixY;
extern int              mixW;
extern int              mixH;

extern int              MixChanWidth;

extern int              keyX;
extern int              keyY;
extern int              keyW;
extern int              keyH;

extern VU*              firstVU;
extern VU*              lastVU;
extern unsigned int     RenderInterpolationMethod;
extern unsigned int     WorkingInterpolationMethod;

//This variable contains full path to the program's working directory
extern char *           szWorkingDirectory;

extern bool             Sel_Active;
extern bool             Loo_Active;

extern int              Num_Selected;

extern int              AuxRX1;
extern int              AuxRX2;
extern int              AuxRY1;
extern int              AuxRY2;

extern AwfulWindow*     MainWnd;
extern ConfigWindow*    ConfigWnd;
extern RenderWindow*    RenderWnd;
extern SampleWindow*     SmpWnd;
extern Playback*        first_active_playback;
extern Playback*        last_active_playback;
extern Pattern*         auxPatternSet;
extern int              bottomIncr;
extern int              numFieldLines;
extern Event*           ev0;
extern bool             skip_input;
extern bool             post_menu_update;
extern UndoManaga*      uM;

extern int              PattX1;
extern int              PattX2;
extern int              PattY1;
extern int              PattY2;

extern int              PtDrawX1;
extern int              PtDrawX2;
extern int              PtDrawY1;
extern int              PtDrawY2;

extern int              LAuxH;

extern float            wt_angletoindex;

extern int              InstrCenterOffset;
extern ProjectData      PrjData;

extern int              PianorollLineHeight;
extern bool             Recording;
extern int              mixHeading;

extern Parameter*       firstParam;
extern Parameter*       lastParam;

extern tframe           lastFrame;

extern bool             followPos;
extern bool             firsttime_plugs_scanned;

extern bool             out_from_dialog;

extern HANDLE           hProcessMutex;

extern bool             MakePatternsFat;

extern long             instrIndex;
extern long             effIndex;

extern bool             ProjectLoadingInProgress;

extern bool             JustClickedPattern;
extern bool             RememberLengths;

extern bool             ChangesHappened;

extern bool             AutoBindPatterns;
extern bool             PatternsOverlapping;
extern bool             RescanPluginsAutomatically;

extern String           userName;

extern int              rVer;
extern int              rDay;
extern int              rMonth;
extern int              rYear;

extern File*           lastsessions[10];
extern int             numLastSessions;

extern Sample*         barsample;
extern Sample*         beatsample;

extern bool            metronome;


//*************************************************************************************************
//                       ADD EXTERNAL FUNCTIONS DECLARATIONS HERE
//*************************************************************************************************
Instance*                   CreateElement_Note(Instrument* instr, bool add, bool preventundo = false);
extern void                 GetLastElementEndFrame(long *pLastFrame);
extern void                 PortAudio_SetBufferSize(float BufSize);
extern void                 SetSampleRate(unsigned int uiSampleRate);
extern void                 SetBPM(float bpm);
extern Element*             NextElementToDelete(Element* eltodel);
extern void                 HardDelete(Element* el, bool preventundo);
extern void                 SoftDelete(Element* el, bool preventundo);
extern void                 SoftUnDelete(Element *el);
extern PEdit*               CheckPEditHighlights(int mx, int my);
extern Element*             CheckElementsHighlights(int mx, int my);
extern void                 SetMouseImage(unsigned flags);
extern bool                 CursModeCorrespondsToCursorMode();
extern void                 Selection_UpdateFrameBounds();
extern bool                 Selection_ToggleElements();
extern void                 Selection_UpdateCoords();
extern void                 Looping_UpdateCoords();
extern void                 Looping_Reset();
extern void                 PixelToFrame();
extern float                Div100(float percent);
extern float                Calc_SlideMultiplier(unsigned long frame_length, int semitones);
extern float                CalcSampleFreqIncrement(Sample* sample, int semitones);
extern void                 Vanish(Element* el);
extern Instance*            CreateElement_Instance_byChar(char character);
extern SlideNote*           CreateElement_SlideNote(bool add);
extern Muter*               CreateElement_Muter();
extern Break*               CreateElement_Break();
extern void                 AddNewElement(Element* data, bool noUM);
extern bool			        DeleteElement(Element* el, bool flush, bool preventundo);
extern long				    ParamString2Num(char* string);
extern void				    ParamNum2String(long num, char* string);
extern int                  Line2Y(int track, Loc loc);
extern int                  Tick2X(float tick, Loc loc);
extern float                X2Tick(int x, Loc loc);
extern int                  Y2Line(int y, Loc loc);
extern float				CalcFreqRatio(int semitones);
extern float                Calc_PercentPan(float pan_percent);
extern float				Calc_FloatVol(float vol_percent);
extern long					Tick2Frame(float tick);
extern void				    Num2String(long num, char* string);
extern Slide*				CreateElement_Slide(int semitones);
extern Reverse*             CreateElement_Reverse();
extern Pattern*				CreateElement_Pattern(int x1, int x2, int y1, int y2);
extern Vibrate*				CreateElement_Vibrate();
extern Command*             CreateElement_Command(Scope* scope, CmdType type);
extern Command*             CreateElement_Command(Scope* scope, bool preventundo);
extern Repeat*              CreateElement_Repeater();
extern void                 CreateVolumeEnvelope(Scope scp);
extern void                 CreatePanningEnvelope(Scope scp);
extern Pattern*             CreateElement_Pattern(char* pname, float tk1, float tk2, int tr1, int tr2);
extern Pattern*             CreateElement_Pattern(float tk1, float tk2, int tr1, int tr2, PattType pattype);
extern void                 AddNewControl(Control* ctrl, Panel* owner);
extern void                 RemoveControlCommon(Control* ctrl);
extern Txt*			        CreateElement_TextString();
extern void                 UpdateElementsVisibility();
extern void                 Relocate_Element(Element* el);
extern void                 Add_PEdit(PEdit* pe);
extern void                 Remove_PEdit(PEdit* pe);
extern void                 Enqueue_MixCell(Mixcell* mc);
extern void                 Dequeue_MixCell(Mixcell* mc);
extern AliasRecord*         GetAliasRecordFromString(char* str);
extern void                 LowerCase(char* data);
extern void                 Add_Panel(Panel* p);
extern void                 Add_VU(VU* vu);
extern void                 Remove_VU(VU* vu);
extern Sample*              Add_Sample(const char* path, const char* name, const char* alias, bool temp = false);
extern VSTGenerator*        Add_VSTGenerator(const char* path, const char* name, const char* alias);
extern VSTGenerator*        Add_VSTGenerator(VSTGenerator* vst, char* alias);
extern Synth*               Add_Synth(const char* alias);
extern void                 AddEff(Eff* eff);
extern void                 RemoveEff(Eff* eff);
extern Transpose*           CreateElement_Transpose(int semitones);
extern void                 UpdateAllElements(Pattern* pt, bool tonly = false);
extern Pattern*             CheckPosForPatterns(float tick, int trkline, Pattern* base);
extern void                 Process_Key_Default(unsigned key, unsigned flags);
extern void                 UpdateScrollbarOutput(ScrollBard* sb);
extern Element*				CheckCursorVisibility();
extern void                 PlaceCursor();
extern void                 UpdatePerBPM();
extern void                 UpdateTime(Loc loc);
extern void                 DecreaseScale(bool mouse);
extern void                 IncreaseScale(bool mouse);
extern void                 SetScale(float scale);
extern void                 UpdatePerScale();
extern void                 Grid2Main();
extern void                 CleanupElements();
extern void                 CleanupEffects();
extern void                 CleanupAll();
extern void                 SwitchInputMode(CursMode mode);
extern void                 PosUp();
extern void                 PosDown();
extern void                 MainPos2AuxPos();
extern void                 AuxPos2MainPos();
extern void                 Process_AutoAdvance();
extern void                 ChangeCurrentInstrument(Instrument* instr);
extern void                 MinimizeWindow();
extern void                 MaximizeWindow();
extern void                 FullScreenON();
extern void                 FullScreenOFF();
extern void                 ToggleFullScreen();
extern void                 ToggleConfigWindow();
extern void                 ToggleRenderWindow();
extern void                 ToFront();
extern void                 Grid_PutInstanceSpecific(Pattern* patt, Instrument *instr, int note, float vol,    tframe start_frame, tframe end_frame);
extern Element*             IsElemExists(float tick, int trknum, Pattern* pt);
extern bool                 CheckDeletionSkip(Element* eltocheck, Element* eltodel);
extern void                 R(int r);
extern bool                 ResetHighlights(bool* redrawmain, bool* redrawaux);
extern void                 Process_KeyState();
extern void                 DragNDrop_PlaceInstrument();
extern void                 Queue_Instr_Deleted(Instrument* i);
extern void                 DeleteMenu(Menu* menu);
extern void                 UpdateNavBarsData();
extern void                 AddRecordingParam(Parameter* param);
extern void                 RemoveRecordingParam(Parameter* param);
extern Command*             CreateCommandFromControl(Control* ctrl);
extern Command*             CreateCommandFromParam(Parameter* param);
extern void                 AddGlobalParam(Parameter* param);
extern void                 RemoveGlobalParam(Parameter* param);
extern void                 CreateElement_EnvelopeCommon(Parameter* param);
extern void                 DoSavePreset(Instrument* instr);
extern void                 DoSavePreset(Eff* eff);
extern void                 DoLoadPreset(Instrument* instr);
extern void                 DoLoadPreset(Eff* eff);
extern void                 UpdateQuants();
extern void                 UpdateAuxNavBarOnly();
extern void                 RescanPlugins(bool full, bool brwupdate = true);
extern void                 AddOriginalPattern(Pattern* pattern);
extern void                 RemoveOriginalPattern(Pattern* pattern);
extern Eff*                 GetEffByIndex(int index);
extern Pattern*             GetPatternByIndex(int index);
extern Parameter*           GetParamByGlobalIndex(int index);
extern void                 LoadElementsFromNode(XmlElement* xmlMainNode, Pattern* pttarget);
extern void                 LoadProject(File* f);
extern void                 UpdateScaledImages();
extern Instrument*          GetInstrumentByIndex(int index);
extern void                 AdjustTick();
extern void                 ChangesIndicate();
extern bool                 CheckSave();
extern void                 AdjustClipboardOffsets();
extern Pattern*             CreateElement_DerivedPattern(Pattern* ptmain, float tk1, float tk2, int tr1, int tr2);
extern void                 ExitFromApp();
extern void                 SaveSettings();
extern void                 ShowHintIfApplicable();
extern FILE*                ReadPluginsFromFile();
extern void                 UpdateControlHint(Control* ct, int mouse_x, int mouse_y);

#endif
//#endif // !defined(AFX_AWFUL_H__722DB30A_BAF1_4CAF_A33F_F6E145D206E2__INCLUDED_)
