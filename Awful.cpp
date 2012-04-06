//////////////////////////////////////////////////////////
// Awful.cpp 
#include "stdafx.h"
//#include "resource.h"
#include <windows.h>

#include "awful.h"
#include "awful_audio.h"
#include "awful_audiohost.h"
#include "awful_instruments.h"
#include "awful_elements.h"
#include "awful_params.h"
#include "awful_paramedit.h"
#include "awful_controls.h"
#include "awful_panels.h"
#include "awful_graphJuce.h"
#include "awful_utils_common.h"
#include "awful_effects.h"
#include "awful_events_triggers.h"
#include "awful_tracks_lanes.h"
#include "VSTCollection.h"
#include <direct.h>
#include "shlwapi.h"
#include "awful_renderer.h"
#include "Awful_JuceComponents.h"
#ifdef USE_OLD_JUCE
#include "juce_amalgamated.h"
#else
#include "juce_amalgamated_NewestMerged.h"
#endif
#include "Awful_midi.h"
#include "Awful_JuceWindows.h"
#include "Awful_preview.h"
#include "awful_cursorandmouse.h"
#include "awful_undoredo.h"

#include "rosic/math/rosic_PrimeNumbers.h"
#include "rosic/delaylines/rosic_IntegerDelayLine.h"
#include "rosic/filters/rosic_DampingFilter.h"
#include "rosic/filters/rosic_LowpassHighpass.h"
#include "rosic/filters/rosic_WhiteToPinkFilter.h"

#include "Data sources/metronome_waves.h"

//#include "awful_sf2.h"

///////////////////////////////////////
// Functions declarations
///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////


////
//// Creation group
extern void                 CreateElement_EnvelopeCommon(Parameter* param);
extern Command*             CreateElement_Command(Scope* scope, CmdType type);
extern Command*             CreateElement_Command(Scope* scope, bool preventundo);
extern Vibrate*				CreateElement_Vibrate();
extern NoteInstance*            CreateElement_Instance_byChar(char character);
extern SlideNote*           CreateElement_SlideNote(bool add);
extern Muter*               CreateElement_Muter();
extern Break*               CreateElement_Break();
extern Slide*				CreateElement_Slide(int semitones);
extern Reverse*             CreateElement_Reverse();
extern Repeat*              CreateElement_Repeater();
extern Pattern*				CreateElement_Pattern(int x1, int x2, int y1, int y2);
extern Pattern*             CreateElement_Pattern(char* pname, float tk1, float tk2, int tr1, int tr2);
extern Transpose*           CreateElement_Transpose(int semitones);
extern Txt*			        CreateElement_TextString();
extern NoteInstance*            CreateElement_Note(Instrument* instr, bool add, bool preventundo);
extern Element*             CreateElement_CommonSymbol(ElemType eltype, bool add, bool preventundo);


////
//// Editing group
void                        SetPosX(double newX, Loc loc);
void                        SetPatternContext(int mouse_x, int mouse_y);
void				        Vanish_CleanUp();
void				        ToggleTextModeToNoteMode();
void                        ProcessColLane(int mouse_x, int mouse_y, ParamType paramtype, Loc loc, Trk* trk, Pattern* pt, int lx1, int ly1, int lx2, int ly2, bool deflt);
void                        Selection_UpdateTicks();
void                        Selection_UpdateCoords();
bool                        Selection_ToggleElements();
void                        ChangeCurrentInstrument(Instrument* instr);
void                        Process_AutoAdvance();
void                        Process_Resize(int mouse_x, int mouse_y, unsigned flags, Loc loc);
void                        AuxPos2MainPos();
void                        MainPos2AuxPos();
void                        PosUp();
void                        PosDown();
void                        SwitchInputMode(CursMode mode);
void                        Grid2Main();
void                        Grid_PutInstanceSpecific(Pattern* patt, Instrument *instr, int note, float vol,  tframe   start_frame, tframe end_frame);
void                        UpdateScaledImages();
extern void                 UpdateTime(Loc loc);
void                        UpdatePerBPM();
void                        Process_Key_Default(unsigned key, unsigned flags);
void                        Process_KeyState();
extern void                 Dequeue_MixCell(Mixcell* mc);
extern void                 Enqueue_MixCell(Mixcell* mc);
void                        UpdateScrollbarOutput(ScrollBard* sb);
void                        ShowVolLane(Trk* trk, Pattern* pt);
void                        ShowPanLane(Trk* trk, Pattern* pt);
void                        HideVolLane(Trk* trk, Pattern* pt);
void                        HidePanLane(Trk* trk, Pattern* pt);
void                        MoveBorders(int mouse_x, int mouse_y);
void                        SetMouseImage(unsigned flags);
void                        DeleteMenu(Menu* menu);
void                        DoSavePreset(Instrument* instr);
void                        DoSavePreset(Eff* eff);
void                        DoLoadPreset(Instrument* instr);
void                        DoLoadPreset(Eff* eff);

////
//// Elements and params manipulations group
extern Pattern*             GetPatternByIndex(int index);
extern bool                 ResetHighlights(bool* redrawmain, bool* redrawaux);
extern void                 UpdateElementsVisibility();
void                        MovePickedUpElements(int mx, int my, bool shift);
void                        ClonePickedToNew(float dtick, int dtrack);
bool                        PickUpElements();
void                        DropPickedElements();
extern void                 Relocate_Element(Element* el);
void                        LocateElement(Element* el);
extern void                 UpdateAllElements(Pattern* pt, bool tonly);
void                        CleanupElements();
void                        CleanupEffects();
void                        CleanupAll();
void                        UpdateNavBarsData();
void                        UpdateAuxNavBarOnly();
void                        UpdateParamIndex();
void                        UpdatePatternIndex();
void                        UpdateEffIndex();
Eff*						GetEffByIndex(int index);
void                        UpdateInstrumentIndices();
void                        UpdateEffIndices();
void                        UpdatePatternIndices();
void                        UpdateParamIndices();

////
//// Adders and removers group
extern void                 AddSamplent(Samplent* s);
extern void                 AddCommand(Command* c);
extern void                 AddNewElement(Element* data);
extern void                 HardDelete(Element* el, bool preventundo);
extern void                 SoftDelete(Element* el, bool preventundo);
extern void                 SoftUnDelete(Element *el);
extern bool                 DeleteElement(Element* el, bool flush, bool preventundo);
extern Element*             NextElementToDelete(Element* eltodel);
extern void                 Delete_Slide(SlideNote* sl);
extern void                 AddNewControl(Control* ctrl, Panel* owner);
extern void                 RemoveControlCommon(Control* ctrl);
extern void                 Add_VU(VU* vu);
extern void                 Remove_VU(VU* vu);
extern void				    AddEff(Eff* eff);
extern void				    RemoveEff(Eff* eff);
extern void                 Add_PEdit(PEdit* pe);
extern void                 Remove_PEdit(PEdit* pe);
void                        Add_Panel(Panel* p);
void                        AddGlobalParam(Parameter* param);
void                        RemoveGlobalParam(Parameter* param);


////
//// Checkers group
bool                        CursModeCorrespondsToCursorMode();
bool                        CheckDeletionSkip(Element* eltocheck, Element* eltodel);
Element*                    CheckElementsHighlights(int mx, int my);
PEdit*                      CheckPEditHighlights(int mx, int my);
Pattern*                    CheckPosForPatterns(float tick, int trkline);
Element*					CheckCursorVisibility();

////
//// Scaling functions group
void                        DecreaseScale(bool mouse);
void                        IncreaseScale(bool mouse);
void                        SetScale(float scale);
void                        UpdatePerScale();

////
//// Instruments initialisation group
void                        Load_Default_Instruments();
void                        ScanDirForVST(char *path, char mode, FILE* fhandle, ScanThread* thread);
static void                 Init_InternalPlugins();
static void                 Init_ScanForVST(ScanThread* thread);
void                        ClearVSTListFile();

////
//// System group
void                ReleaseDataOnExit();


//// Libsndfile library
SNDFILE*            sf_open(const char *path, int mode, SF_INFO *sfinfo);
sf_count_t          sf_readf_float(SNDFILE *sndfile, float *ptr, sf_count_t frames);


//This is a piece of shit
VSTCollection  *pVSTCollector;
CPluginList    *pModulesList;
char            splash_string[MAX_NAME_STRING];
Renderer       *pRenderer;
RNDR_CONFIG_DATA_T renderConfig;

//This global parameter holds sample buffer lenght used by renderer and input/output devices
int             gBuffLen;
AwfulMidiWrapper *pMidiHost;


//Crappy shitty hack to get slider rendered when Plugin Editor window is in focus and 
//we are changing parameters - hence slider needs to be updated and rendered in out-of-focus window
HWND gHwnd;
#if (SPLASH_SCREEN == TRUE)
HWND splash_hwnd;
#endif
//////////////////////////////////////
////////////////////////////////////////
// G l o b a l   s t u f f 

// Cursor types
HCURSOR     VSCursor;			// vertical resize
HCURSOR     HSCursor;			// horizontal resize
HCURSOR     ArrowCursor;		// plain arrow

// Raw selection rectangle coordinates
int         SX1;
int         SY1;
int         SX2;
int         SY2;

// Baked selection rectangle coordinates. -1 means absence of selection rectangle
int         SelX1; 
int         SelY1;
int         SelX2;
int         SelY2;

int         LX1;
int         LX2;

int         LooX1; 
int         LooX2;

// Start and end tick of selection
float       SelTick1;
float       SelTick2;

// Start and end tick of looping
float       LooTick1;
float       LooTick2;

// Selection is active
bool        Sel_Active;

// Looping is active
bool        Looping_Active;

// Number of selected items
int         Num_Selected;

// Number of buffered items (cut/copied)
int         Num_Buffered;

// The upper left corner of buffered stuff
float       BuffTick;
int         BuffLine;

// Mix browser works in browsing mode (opposing to track controls mode)
bool        mixbrowse;

// Whethter mixcenter (track controls and mixbrowser) is visible
bool        mixcentervisible;

// Main coordinates (represent the outer line of the main grid)
int         MainX1;
int         MainX2;
int         MainY1;
int         MainY2;

// Main grid inner coordinates
int         GridX1;
int         GridX2;
int         GridY1;
int         GridY2;
int         numFieldLines;

// Main grid quant size
float       quantsize;

// Main grid line offset
int         OffsLine;

int         bottomIncr;

// Main grid tick offset
float       OffsTick;

// Main grid tick width
float		tickWidth;
float		tickWidthBack; // Additional variable used for proper zooming

// Main grid line height
int         lineHeight;

int         st_tickWidth;


// Aux grid inner coordinates
int         GridXS1;
int         GridXS2;
int         GridYS1;
int         GridYS2;
int         numAuxLines;

// Main Aux coordinates
int         AuxX1;
int         AuxX2;
int         AuxY1;
int         AuxY2;

// Aux mixer coord

int         mixHeading;

int         MixChanWidth;

int         PattX1;
int         PattX2;
int         PattY1;
int         PattY2;

int         PtDrawX1;
int         PtDrawX2;
int         PtDrawY1;
int         PtDrawY2;

int         LAuxH;

// Aux vols/pans mode workarea coordinates
int         AuxRX1;
int         AuxRX2;
int         AuxRY1;
int         AuxRY2;
int         AuxHeight;
int         AuxKeysHeight;

// Mixer whole area coordinates
int         MixX;
int         MixY;
int         MixW;
int         MixH;

int         mixX;
int         mixY;
int         mixW;
int         mixH;

// Mixer vertical scrolling offset
int         MixVOffs;
int         MixCenterWidth;
int         DefaultMixCenterWidth;
// Mixer width including mixbrowser area
int         MixerWidth;
// Mixer master area height
int         MixMasterHeight;

// Mixcell sizes
int         MixCellWidth;
int         MixCellBasicHeight;
int         MixCellGap;

// Piano keys coordinates (in pianoroll mode)
int         keyX;
int         keyY;
int         keyW;
int         keyH;

// x-range of control panel
int         CtrlPanelXRange;
// Control panel area sizes
int         CtrlPanelHeight;
int         CtrlPanelWidth;
int         CtrlAdvance;
int         TranspAdvance;

// Static grid x-bounds (postponed)
int         StX1;
int         StX2;

// Browser heading height
int         brw_heading;

// Navigation bar (main bar) height
int         NavHeight;
// Playback scale bar height
int         LinerHeight;

// Instruments panel sizes
int         InstrCellWidth;
int         InstrPanelWidth;
int         InstrPanelHeight;
int         InstrFoldedHeight;
int         InstrUnfoldedHeight;
int         InstrAliasOffset;
int         InstrCenterOffset;

// Gen browser sizes
int         GenBrowserWidth;
int         GenBrowserHeight;
int         GenBrowserGap;

// Time info
int         curr_min;
int         curr_sec;
int         curr_msec;
int         total_min;
int         total_sec;

// Position info
int         curr_bar;
int         curr_beat;
float       curr_step;

// Key cursor global data struct
Cursor      C;

// Cursor global data
int         CursX;
int         CursY;
int         CursX0;
float       CTick;
int         CLine;

// Mouse global data struct
Mouse       M;

// Main panels
CtrlPanel*      CP;
InstrPanel*     IP;
Aux*            aux_panel;

Browser*        genBrw;
Browser*        mixBrw;
StaticArea*     st;
ScrollBard*     main_bar;
ScrollBard*     main_v_bar;

// Master audio data
Master          mAster;

// JUCE base component
MainComponent*  MC;

// JUCE windows
AwfulWindow*    MainWnd;
ConfigWindow*   ConfigWnd;
RenderWindow*   RenderWnd;
SampleWindow*   SmpWnd;

// Main window width and height
int			    WindWidth;
int			    WindHeight;

unsigned int    RenderInterpolationMethod;
unsigned int    WorkingInterpolationMethod;

//Global variable initialized at program start-up
char *          szWorkingDirectory;

// Just moved some stuff
bool            JustMoved;
// Double click timed out flag
bool            DBClickTimeout = true;
// Whether static grid is visible
bool            static_visible;

bool            followPos;

bool            scrolling;
Butt*           scrollbt;

// Instruments list and other relative data
Instrument*     first_instr;
Instrument*     last_instr;
Instrument*     first_instr_del;
Instrument*     last_instr_del;
int             num_instrs;
Instrument*     current_instr;
Instrument*     overriding_instr;

// Timing related global vars
float           beats_per_minute;
float           beats_per_minute_temp;
int             beats_per_bar;
int             ticks_per_beat;
float           frames_per_tick;

// Seconds per one tick
float           seconds_in_tick;

// Some auxiliary stuff
float           one_divided_per_frames_per_tick;      // 1 divided on seconds_in_tick (== ticks per second)
float           one_divided_per_sample_rate;

// Playback x coord integer and float
int	            currPlayX;
double          currPlayX_f;

float           framesPerPixel;

int             refresh_rate;
int             refresh_counter;

// Metronome flag
bool            Metronome_ON;

// Main playback
bool            Playing;
// Rendering is in progress
bool            Rendering;
// Recording is ON
bool            Recording;
// Base octave
int             Octave;
// Relative octave
int             relOctave;

// The base note (C-5 commonly)
int             baseNote;
float           fSampleRate;
// Number of items in deletion stack
int             delCount;
// The deletion stack
Element*        delList[100];

// Grids are represented by patterns. Field is the main grid.
Pattern*        field_pattern;

// Playback pattern to set for Aux.
Pattern*        aux_Pattern;

// Original patterns list
Pattern*        first_base_pattern;
Pattern*        last_base_pattern;

// All elements list
Element*        firstElem;
Element*        lastElem;

// Active commands list
Trigger*        first_active_command_trigger;
Trigger*        last_active_command_trigger;

// Global active triggers list
Trigger*        first_global_active_trigger;
Trigger*        last_global_active_trigger;

// Effects list
Eff*            first_eff;
Eff*            last_eff;

// Effects list
Parameter*      first_rec_param;
Parameter*      last_rec_param;

// Active patterns list
Trigger*        first_active_pattern_trigger;
Trigger*        last_active_pattern_trigger;

// Active additional playbacks list
Playback*       first_active_playback;
Playback*       last_active_playback;

// PEdit list
PEdit*          firstPE;
PEdit*          lastPE;

// Controls list
Control*        firstCtrl;
Control*        lastCtrl;

// Params list
Parameter*      firstParam;
Parameter*      lastParam;

// Volume meters list
VU*             firstVU;
VU*             lastVU;

// Panels list
Panel*          firstPanel;
Panel*          lastPanel;

// Main grid tracks data list and auxilliary data
Trk*            first_trkdata;
Trk*            last_trkdata;
Trk*            top_trk;
Trk*            bottom_trk;

// Alias records list
AliasRecord*    first_alias_record;
AliasRecord*    last_alias_record;

// Bunches list
TrkBunch*       bunch_first;
TrkBunch*       bunch_last;

// Pianokeys data (postponed)
int             keys_offset;
int             keys_width;

// data buffer for analysis
float           ring[1024];
int             ringPos;
int             ringSize;

// Keyboard mappings array
unsigned        keymap[256];

// Main window handle
HWND            hWnd;

// This shit is needed for displaying Hint content
unsigned int    HintTimer;
Control*        MouseControl;

// Different precalculated wavetables
float           wt_sine[WT_SIZE];
float           wt_cosine[WT_SIZE];
float           wt_saw[WT_SIZE];
float           wt_triangle[WT_SIZE];

float           wt_coeff;

float           wt_angletoindex;

// Solo data
Instrument*     solo_Instr;
Trk*            solo_Trk;
Mixcell*        solo_Mixcell;
MixChannel*     solo_MixChannel;

// Various flags
bool            skip_input = false;
bool            out_from_dialog = false;
bool            auto_advance;
bool            post_menu_update;
bool            firsttime_plugs_scanned;

// Playback data for both grids
Playback*       pbkMain;
Playback*       pbkAux;

// Zero event for preview purposes
Event*          ev0;
UndoManagerC*   undoMan;

long            paramIndex;
long            pattIndex;
long            instrIndex;
long            effIndex;

int             PianorollLineHeight;

tframe          lastFrame;

char lastchar[2] = {0, 0};

// Basic project info data
ProjectData PrjData;

// Various mouse cursor shapes
MouseCursor*     cursCopy;
MouseCursor*     cursClone;
MouseCursor*     cursSlide;
MouseCursor*     cursBrush;
MouseCursor*     cursSelect;

HANDLE          hAudioProcessMutex;

bool            MakePatternsFat;
bool            JustClickedPattern;
bool            RememberLengths;
bool            AutoBindPatterns;
bool            PatternsOverlapping;
bool            RescanPluginsAutomatically;

bool            ProjectLoadingInProgress;

bool            ChangesHappened;

int             rVer = 101;
int             rDay = 7, rMonth = 7, rYear = 2010;
String          userName;

bool            bDebug;

File*           lastsessions[10];
int             numLastSessions;

Sample*         barsample = NULL;
Sample*         beatsample = NULL;

// In release build, we retrieve the target path a bit differently
bool            obtainReleasePathDir = false;


void MakeCoolVUFallDownEffectYo()
{
    CP->MVol->vu->SetLR(Random::getSystemRandom().nextFloat()*0.5f + 0.5f, 
                        Random::getSystemRandom().nextFloat()*0.5f + 0.5f);

    Instrument* i = first_instr;
    while(i != NULL)
    {
        i->vu->SetLR(Random::getSystemRandom().nextFloat()*0.5f + Random::getSystemRandom().nextFloat()*0.5f, 
                        Random::getSystemRandom().nextFloat()*0.5f + Random::getSystemRandom().nextFloat()*0.5f);
        i = i->next;
    }
}

void ChangesIndicate()
{
    if(!ChangesHappened)
    {
        R(Refresh_All);
    }

    ChangesHappened = true;
    if(!ProjectLoadingInProgress)
    {
        MainWnd->UpdateTitle();
    }
}

void ResetChangesIndicate()
{
    ChangesHappened = false;
}


void ProjectData::Init()
{
    newproj = true;
    strcpy(projname, "Untitled");
    projpath;
    //memset(projpath, 0, MAX_PATH_STRING);
    file = NULL;
}

void ProjectData::SetName(String name)
{
    if(name.length()<= MAX_NAME_STRING)
    {
        name.copyToBuffer(projname, MAX_NAME_STRING);
    }
}

void CleanElements()
{
    AlertWindow w (T("Warning"),
                   T("Do you really want to clean all existing elements (no undo)?"),
                   AlertWindow::WarningIcon);
    w.setSize(122, 55);
    w.addButton (T("Yes"), 1, KeyPress (KeyPress::returnKey, 0, 0));
    w.addButton (T("No"), 0, KeyPress (KeyPress::returnKey, 0, 0));
    int result = w.runModalLoop();
    if(result == 1) // if they picked 'OK'
    {
        if(Playing == true)
        {
            StopMain(true);
            CP->play_butt->Release();
        }
        else if(aux_panel->playing == true)
        {
            aux_panel->Stop();
            aux_panel->playbt->Release();
        }

        if(C.mode == CMode_NotePlacing)
        {
            ToggleTextModeToNoteMode();
        }
        Element* elnext;
        Element* el = firstElem;
        while(el != NULL)
        {
            elnext = NextElementToDelete(el);
            DeleteElement(el, true, true);
            el = elnext;
        }

        R(Refresh_All);
    }
    else
    {
        return;
    }
}

void CleanProject()
{
    WaitForSingleObject(hAudioProcessMutex, INFINITE);

    Selection_Reset();
    Looping_Reset();

    if(Playing == true)
    {
        StopMain(true);
        CP->play_butt->Release();
    }
    else if(aux_panel->playing == true)
    {
        aux_panel->Stop();
        aux_panel->playbt->Release();
    }
    else
    {
        // Just stop any sounding
        StopMain(true);
    }

    aux_panel->AuxReset();
    pbkAux->SetPlayPatt(aux_Pattern); // force setting to blank pattern

    if(C.mode == CMode_NotePlacing)
    {
        ToggleTextModeToNoteMode();
    }

    Element* elnext;
    Element* el = firstElem;
    while(el != NULL)
    {
        elnext = NextElementToDelete(el);
        DeleteElement(el, true, true);
        el = elnext;
    }

    Instrument* inext;
    Instrument* i = first_instr;
    while(i != NULL)
    {
        inext = i->next;
        IP->RemoveInstrument(i);
        i = inext;
    }

    aux_panel->current_eff = NULL;
    if(mixBrw->brwmode == Browse_Presets || mixBrw->brwmode == Browse_Params)
    {
        mixBrw->Update();
    }

    Eff* effnext;
    Eff* eff = first_eff;
    while(eff != NULL)
    {
        effnext = eff->next;
        RemoveEff(eff);
        eff = effnext;
    }

    for(int mc = 0; mc < NUM_MIXCHANNELS; mc++)
    {
        aux_panel->mchan[mc].routestr->SetString("--");
        aux_panel->mchan[mc].mc_main->params->Reset();
        aux_panel->mchan[mc].amount1->Reset();
        aux_panel->mchan[mc].amount2->Reset();
        aux_panel->mchan[mc].amount3->Reset();
    }

    solo_Instr = NULL;
    solo_Trk = NULL;
    solo_Mixcell = NULL;
    solo_MixChannel = NULL;

    //UpdateParamIndex();
    //UpdatePatternIndex();
    //UpdateEffIndex();
    //UpdateInstrumentIndex();

    baseNote = 60;
    relOctave = 0;
    beats_per_minute = 120;       // BPM
    ticks_per_beat = 4;           // TPB =)
    beats_per_bar = 4;           // BPB =)))
    Octave = 5;
    lastFrame = 0;

    UpdatePerBPM();
    UpdateQuants();

    // Position cursor, playback pos and mainbar
    SetPosX(0 - OffsTick*tickWidth, Loc_MainGrid);
    SetScale(tickWidthBack);
    aux_panel->SetScale(aux_panel->tickWidthBack);

    OffsLine = 0;
    C.ExitToDefaultMode();
    C.SetPattern(field_pattern, Loc_MainGrid);
    C.SetPos(8, 4);
    main_bar->SetOffset(0);
    main_v_bar->SetOffset(0);
    aux_panel->mix_sbar->SetOffset(0);

    PrjData.projpath = "";
    PrjData.SetName("Untitled");
    ResetChangesIndicate();
    MainWnd->UpdateTitle();

    ReleaseMutex(hAudioProcessMutex);
}

void ActualizeStepSeqPositions()
{
    Element* el = firstElem;
    while(el != NULL)
    {
        if(el->IsPresent() && el->IsInstance() && el->patt->ptype == Patt_StepSeq)
        {
            el->SetTrackLine(((NoteInstance*)el)->instr->index);
        }

        el = el->next;
    }
}

void LoadElementsFromNode(XmlElement* xmlMainNode, Pattern* pttarget)
{
    XmlElement* xmlChild = NULL;
    forEachXmlChildElementWithTagName(*xmlMainNode, xmlChild, T("Element"))
    {
        ElemType type = (ElemType)xmlChild->getIntAttribute(T("Type"));
        float stick = (float)xmlChild->getDoubleAttribute(T("StartTick"));
        float etick = (float)xmlChild->getDoubleAttribute(T("EndTick"));
        float ticklength = (float)xmlChild->getDoubleAttribute(T("TickLength"));
        int trackline = xmlChild->getIntAttribute(T("TrackLine"));
        int pattindex = xmlChild->getIntAttribute(T("PattIndex"));
        Pattern* patt;
        if(pttarget == NULL)
        {
            patt = GetPatternByIndex(pattindex);
        }
        else
        {
            patt = pttarget;
        }

        if(patt != NULL)
        {
            Element* el = NULL;
            C.SaveState();
            C.SetPos(stick, trackline);
            C.SetPattern(patt, patt == field_pattern ? Loc_MainGrid : Loc_SmallGrid);
            switch(type)
            {
                case El_TextString:
                {
                    el = CreateElement_TextString();
                }break;
                case El_GenNote:
                case El_Samplent:
                {
                    int index = (int)xmlChild->getIntAttribute(T("InstrIndex"));
                    Instrument* instr = GetInstrumentByIndex(index);
                    if(instr != NULL)
                    {
                        el = CreateElement_Note(instr, true, true);
                    }
                }break;
                case El_Command:
                {
                    el = CreateElement_Command(NULL, true);
                }break;
                case El_Mute:
                case El_Break:
                case El_SlideNote:
                case El_Slider:
                case El_Reverse:
                case El_Vibrate:
                case El_Transpose:
                {
                    el = CreateElement_CommonSymbol(type, true, true);
                }break;
            }

            if(el != NULL)
            {
                el->Load(xmlChild);
                el->Update();
            }
            C.RestoreState();
        }
    }
}


void ResetIndices()
{
    Parameter* param = firstParam;
    while(param != NULL)
    {
        param->globalindex = -1;
        param = param->gnext;
    }

    Pattern* patt = first_base_pattern;
    while(patt != NULL)
    {
        patt->baseindex = -1;
        patt = patt->base_next;
    }

    Instrument* instr = first_instr;
    while(instr != NULL)
    {
        instr->index = -1;
        instr = instr->next;
    }

    Eff* eff = first_eff;
    while(eff != NULL)
    {
        eff->index = -1;
        eff = eff->next;
    }
}

void SortLastSessions(File* newEntry)
{
    if(newEntry != NULL)
    {
        File* got;
        File* put = NULL;
        bool replaced = false;
        for(int si = 0; si < numLastSessions; si++)
        {
            if(*lastsessions[si] != *newEntry)
            {
                got = lastsessions[si];
                lastsessions[si] = put;
                put = got;
            }
            else
            {
                lastsessions[0] = lastsessions[si];
    			if(put != NULL)
    				lastsessions[si] = put;
                replaced = true;
                break;
            }
        }

        if(replaced == false)
        {
    		lastsessions[0] = new File(*newEntry);
			if(numLastSessions < 10)
			{
    			if(numLastSessions > 0)
    				lastsessions[numLastSessions] = put;
				numLastSessions++;
			}
        }
    }

    CP->UpdateSessionItems();
}

class LoadThread;
bool LoadProjectData(File chosenFile, LoadThread* thread);

//==============================================================================
class LoadThread  : public ThreadWithProgressWindow
{
public:
    LoadThread(File f)
        : ThreadWithProgressWindow (T("Loading saved project..."),
                                    false,
                                    false)
    {
        setStatusMessage (T("Loading saved project..."));
        projectfile = f;
    }

    ~LoadThread()
    {
    }

    void run()
    {
        //const MessageManagerLock mmlock;
        LoadProjectData(projectfile, this);
    }

    File projectfile;
};

bool LoadProjectData(File chosenFile, LoadThread* thread)
{
    ProjectLoadingInProgress = true;

    ResetIndices();

    bool retval = false;
    XmlDocument myProject(chosenFile);
    XmlElement* xmlMainNode = myProject.getDocumentElement();
    if(NULL == xmlMainNode)
    {
        AlertWindow::showMessageBox(AlertWindow::WarningIcon,
                                     T("Error"),
                                     T("Can't open project file"),
                                     T("OK"));
    }
    else
    {
        if(xmlMainNode->hasTagName(T("ChaoticProject")))
        {
            beats_per_minute = beats_per_minute_temp = (float)xmlMainNode->getDoubleAttribute(T("BPM"));
            ticks_per_beat = xmlMainNode->getIntAttribute(T("TPB"));
            beats_per_bar = xmlMainNode->getIntAttribute(T("BPB"));
            Octave = xmlMainNode->getIntAttribute(T("Octave"));

            UpdatePerBPM();
            UpdateQuants();

            // Load and set master volume
            XmlElement* xmlChild = NULL;
            forEachXmlChildElementWithTagName(*xmlMainNode, xmlChild, T("Parameter"))
            {
                String sname = xmlChild->getStringAttribute(T("name"));
                if(sname == T("MasterVolume"))
                {
                    mAster.params->vol->Load(xmlChild);
                }
            }

            // Load all instruments
            forEachXmlChildElementWithTagName(*xmlMainNode, xmlChild, T("Instrument"))
            {
                InstrType itype = (InstrType)xmlChild->getIntAttribute(T("InstrType"));
                String iname = xmlChild->getStringAttribute(T("InstrName"));
                String ipath = xmlChild->getStringAttribute(T("InstrPath"));
                String ialias = xmlChild->getStringAttribute(T("InstrAlias"));

                File f(ipath);
                if(thread != NULL)
                    thread->setStatusMessage(T("Loading instrument: ") + iname);

                Instrument* instr = NULL;
                if(itype == Instr_Sample)
                {
                    instr = AddSample((const char*)ipath, (const char*)iname, (const char*)ialias);
                }
                else if(itype == Instr_VSTPlugin)
                {
                    instr = AddVSTGenerator((const char*)ipath, (const char*)iname, (const char*)ialias);
                }
                else 
                {
                    instr = AddInternalGenerator((const char*)ipath, (const char*)ialias);
                }

                if(instr != NULL)
                {
                    instr->Load(xmlChild);
                }
            }

            // Populate step-sequencer if it's turned on
            if(aux_panel->workPt->basePattern->ptype == Patt_StepSeq)
            {
                aux_panel->PopulatePatternWithInstruments(aux_panel->workPt->basePattern);
            }

            // Load all effects into mixchannels
            forEachXmlChildElementWithTagName(*xmlMainNode, xmlChild, T("MixChannel"))
            {
                char indexstr[15];
                MixChannel* mchan = NULL;
                xmlChild->getStringAttribute(T("Index")).copyToBuffer(indexstr, 15);
                mchan = aux_panel->GetMixChannelByIndexStr(indexstr);

                if(mchan != NULL)
                {
                    mchan->Load(xmlChild);

                    forEachXmlChildElementWithTagName(*xmlChild, xmlChildEff, T("Effect"))
                    {
                        ModuleSubType etype = (ModuleSubType)xmlChildEff->getIntAttribute(T("EffType"));
                        String ename = xmlChildEff->getStringAttribute(T("EffName"));
                        String epath = xmlChildEff->getStringAttribute(T("EffPath"));

                        if(etype == ModSubtype_VSTPlugin)
                        {
                            File f(epath);
                            if(thread != NULL)
                                thread->setStatusMessage(T("Loading effect: ") + f.getFileName());
                        }
                        else
                        {
                            if(thread != NULL)
                                thread->setStatusMessage(T("Loading effect: ") + ename);
                        }

                        Eff* eff = NULL;
                        if(etype != ModSubtype_VSTPlugin)
                        {
                            eff = aux_panel->AddEffectByType(etype, mchan);
                        }
                        else
                        {
                            eff = aux_panel->AddVSTEffectByPath((const char*)epath, mchan);
                        }

                        if(eff != NULL)
                        {
                            eff->Load(xmlChildEff);
                        }
                    }
                }
            }

            // Load all patterns
            if(thread != NULL)
                thread->setStatusMessage(T("Loading patterns..."));
            forEachXmlChildElementWithTagName(*xmlMainNode, xmlChild, T("Pattern"))
            {
                int origindex = xmlChild->getIntAttribute(T("PattIndex"));
                bool isauto = xmlChild->getBoolAttribute(T("IsAuto"));
                if(isauto)
                {
                    Instrument* instr = IP->GetInstrByIndex(xmlChild->getIntAttribute(T("InstrOwnerIndex"), -1));
                    if(instr != NULL)
                    {
                        instr->CreateAutoPattern();
                        if(instr->autoPatt != NULL)
                            instr->autoPatt->baseindex = origindex;
                    }
                }
                else
                {
                    char pattname[MAX_NAME_STRING];
                    xmlChild->getStringAttribute(T("Name")).copyToBuffer(pattname, MAX_NAME_STRING);
                    Pattern* patt = new Pattern(pattname, -1, -1, -1, -1, true);

                    PattType pattype = (PattType)xmlChild->getIntAttribute(T("PattType"));
                    patt->tick_width = (float)xmlChild->getDoubleAttribute(T("TickWidth"));

                    GetPatternNameImage(patt);
                    patt->ptype = pattype;
                    if(patt->ptype == Patt_StepSeq)
                    {
                        aux_panel->PopulatePatternWithInstruments(patt);
                    }
                    patt->baseindex = origindex;
                    
                    int ibidx = xmlChild->getIntAttribute(T("InstrBoundIndex"), -1);
                    if(ibidx != -1)
                    {
                        Instrument* ibound = IP->GetInstrByIndex(ibidx);
                        if(ibound != NULL)
                            patt->ibound = ibound;
                    }

                    AddOriginalPattern(patt);

                    XmlElement* xmlDerived = NULL;
                    forEachXmlChildElementWithTagName(*xmlChild, xmlDerived, T("Derived"))
                    {
                        float stick = (float)xmlDerived->getDoubleAttribute(T("StartTick"));
                        float etick = (float)xmlDerived->getDoubleAttribute(T("EndTick"));
                        float ticklength = (float)xmlDerived->getDoubleAttribute(T("TickLength"));
                        int trackline = xmlDerived->getIntAttribute(T("TrackLine"));
                        Element* el = NULL;
                        Pattern* ptder = new Pattern(pattname, stick, etick, trackline, trackline, false);
                        ptder->Load(xmlDerived);
                        ptder->ptype = pattype;
                        patt->AddDerivedPattern(ptder);
                        AddNewElement(ptder, true);
                    }
                }
            }

            // And load all elements, of course
            if(thread != NULL)
                thread->setStatusMessage(T("Loading elements..."));

            LoadElementsFromNode(xmlMainNode, NULL);

            // Update images for originators
            UpdateScaledImages();
            OffsTick = 0;

            ChangeCurrentInstrument(first_instr);
        }
    }

    PrjData.newproj = false;
    PrjData.SetName(chosenFile.getFileNameWithoutExtension());
    PrjData.projpath = chosenFile.getFullPathName();

    // Now reset all indices
    UpdateInstrumentIndices();
    UpdateEffIndices();
    UpdatePatternIndices();
    UpdateParamIndices();

    // If some instrument wasn't loaded, then need to adjust line positions in step sequencers
    ActualizeStepSeqPositions();

    if(aux_panel->isPatternMode())
        aux_panel->HandleButtDown(aux_panel->Vols);

    undoMan->WipeEntireHistory();

    ProjectLoadingInProgress = false;

    MakeCoolVUFallDownEffectYo();

    return retval;
}

void SaveProjectData(File chosenFile)
{
    XmlElement  xmlProjectMain(T("ChaoticProject"));
    xmlProjectMain.setAttribute(T("CMMVersion"), rVer);

    //XmlElement  *xmlProjectHeader = new XmlElement(T("Module"));
    //xmlProjectHeader->setAttribute(T("Name"), this->name);

    //xmlProjectMain.setAttribute(T("ProjectName"), PrjData.projname);

    xmlProjectMain.setAttribute(T("BPM"), beats_per_minute);
    xmlProjectMain.setAttribute(T("TPB"), ticks_per_beat);
    xmlProjectMain.setAttribute(T("BPB"), beats_per_bar);
    xmlProjectMain.setAttribute(T("Octave"), Octave);

    XmlElement* mXML = mAster.params->vol->Save();
    xmlProjectMain.addChildElement(mXML);

    //////////////////////////////////
    // Save instruments
    Instrument* i = first_instr;
    while(i != NULL)
    {
        XmlElement* xmlInstr = new XmlElement(T("Instrument"));
        i->Save(xmlInstr);

        xmlProjectMain.addChildElement(xmlInstr);

        i = i->next;
    }

    //////////////////////////////////
    // Save mixchannels and effects
    MixChannel* mchan;
    for(int mc = 0; mc < NUM_MIXCHANNELS; mc++)
    {
        XmlElement* xmlMChan = new XmlElement(T("MixChannel"));
        mchan = &aux_panel->mchan[mc];
        mchan->Save(xmlMChan);

        xmlProjectMain.addChildElement(xmlMChan);
    }

    for(int sc = 0; sc < 3; sc++)
    {
        XmlElement* xmlMChan = new XmlElement(T("MixChannel"));
        mchan = &aux_panel->sendchan[sc];
        mchan->Save(xmlMChan);

        xmlProjectMain.addChildElement(xmlMChan);
    }

    XmlElement* xmlMChan = new XmlElement(T("MixChannel"));
    mchan = &aux_panel->masterchan;
    mchan->Save(xmlMChan);

    xmlProjectMain.addChildElement(xmlMChan);

    // Save patterns
    Pattern* pt = first_base_pattern;
    while(pt != NULL)
    {
        if(pt->IsPresent() && (pt->IsAnyDerivedPresent() || pt->autopatt))
        {
            XmlElement* xmlPatt = new XmlElement(T("Pattern"));
            xmlPatt->setAttribute(T("Name"), pt->name->string);
            xmlPatt->setAttribute(T("PattType"), pt->ptype);
            xmlPatt->setAttribute(T("PattIndex"), pt->baseindex);
            xmlPatt->setAttribute(T("IsAuto"), pt->autopatt);
            xmlPatt->setAttribute(T("TickWidth"), pt->tick_width);
            if(pt->instr_owner != NULL)
                xmlPatt->setAttribute(T("InstrOwnerIndex"), pt->instr_owner->index);
            if(pt->ibound != NULL)
                xmlPatt->setAttribute(T("InstrBoundIndex"), pt->ibound->index);

            if(!pt->autopatt)
            {
                Pattern* ptd = pt->der_first;
                while(ptd != NULL)
                {
                    if(ptd->IsPresent())
                    {
                        XmlElement* xmlDer = new XmlElement(T("Derived"));
                        ptd->Save(xmlDer);
                        xmlPatt->addChildElement(xmlDer);
                    }

                    ptd = ptd->der_next;
                }
            }

            xmlProjectMain.addChildElement(xmlPatt);
        }

        pt = pt->base_next;
    }

    // Save elements
    Element* el = firstElem;
    while(el != NULL)
    {
        if(el->IsPresent() && el->type != El_Pattern)
        {
            XmlElement* xmlElem = new XmlElement(T("Element"));
            el->Save(xmlElem);
            xmlProjectMain.addChildElement(xmlElem);
        }
        el = el->next;
    }

    xmlProjectMain.writeToFile(chosenFile.withFileExtension(ProjExt), String::empty);

    PrjData.SetName(chosenFile.getFileNameWithoutExtension());
    ResetChangesIndicate();
    MainWnd->UpdateTitle();
}

bool SaveProject(bool as)
{
    SetCurrentDirectory(szWorkingDirectory);

    if(as == true || PrjData.newproj == true)
    {
        String str = T(".\\Projects");
        str += "\\";
        str += PrjData.projname;

        FileChooser fc(T("Save project as..."),
                       File(str),
                       "*.cmm",
                       true);
        
        if(fc.browseForFileToSave (true))
        {
            File chosenFile = fc.getResult();
            PrjData.newproj = false;
            PrjData.projpath = chosenFile.getFullPathName();

            SaveProjectData(chosenFile);
            chosenFile = chosenFile.withFileExtension("cmm");
            SortLastSessions(&chosenFile);

            R(Refresh_All);
            if(genBrw->brwmode == Browse_Projects)
            {
                genBrw->Update();
            }

            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        File chosenFile = File(PrjData.projpath);
        SaveProjectData(chosenFile);
        SortLastSessions(&chosenFile);

        R(Refresh_All);
        if(genBrw->brwmode == Browse_Projects)
        {
            genBrw->Update();
        }

        return false;
    }
}

// returns true if need to skip saving
bool CheckSave()
{
    if(ChangesHappened)
    {
        AlertWindow w (T("Question"),
                       T("Save changes to the current project?"),
                       AlertWindow::QuestionIcon);
        w.setSize(122, 55);
        w.addButton (T("Yes"), 2, KeyPress (KeyPress::returnKey, 0, 0));
        w.addButton (T("No"), 1, KeyPress (KeyPress::returnKey, 0, 0));
        w.addButton (T("Cancel"), 0, KeyPress (KeyPress::returnKey, 0, 0));
        int result = w.runModalLoop();
        if(result == 2) // if they picked 'Yes'
        {
            return SaveProject(PrjData.newproj == true);
        }
        else if(result == 1)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        return false;
    }
}

void LoadProject(File* f)
{
    if(!CheckSave())
    {
        if(f == NULL)
        {
            SetCurrentDirectory(szWorkingDirectory);

            FileChooser fc (T("Choose a file to open"),
                            File(T(".\\Projects")),
                            "*.cmm",
                            true);
            
            if(fc.browseForFileToOpen())
            {
                CleanProject();

                File chosenFile = fc.getResult();
                LoadThread lthread(chosenFile);
                lthread.runThread();
                //LoadProjectData(chosenFile);
                SortLastSessions(f);
            }
        }
        else
        {
            CleanProject();

            LoadThread lthread(*f);
            lthread.runThread();
            //LoadProjectData(*f, NULL);
            SortLastSessions(f);
        }

        ResetChangesIndicate();
        MainWnd->UpdateTitle();

        R(Refresh_All);
    }
}

void NewProject()
{
    if(!CheckSave())
    {
        CleanProject();
        PrjData.newproj = true;

        R(Refresh_All);
    }
}

void SaveSettings()
{
    XmlElement xmlSettings(T("ChaoticSettings"));
    //xmlSettings.setAttribute(T("MakePatternsFat"), (int)MakePatternsFat);
    xmlSettings.setAttribute(T("RememberLengths"), (int)RememberLengths);
    xmlSettings.setAttribute(T("AutoBindPatterns"), (int)AutoBindPatterns);
    xmlSettings.setAttribute(T("PatternsOverlapping"), (int)PatternsOverlapping);
    xmlSettings.setAttribute(T("RescanPluginsAutomatically"), (int)RescanPluginsAutomatically);
    xmlSettings.setAttribute(T("BufferSize"), gBuffLen);
    xmlSettings.setAttribute(T("Interpolation"), (int)WorkingInterpolationMethod);

    XmlElement* xmlRenderSettings = new XmlElement(T("RenderSettings"));
    xmlRenderSettings->setAttribute(T("Format"), (int)renderConfig.format);
    xmlRenderSettings->setAttribute(T("Quality"), (int)renderConfig.quality);
    xmlRenderSettings->setAttribute(T("Q1"), renderConfig.q1);
    xmlRenderSettings->setAttribute(T("Q2"), renderConfig.q2);
    xmlRenderSettings->setAttribute(T("InBuffSize"), (int)renderConfig.inbuff_len);
    xmlRenderSettings->setAttribute(T("OutDir"), renderConfig.output_dir);
    xmlRenderSettings->setAttribute(T("Interpolation"), (int)RenderInterpolationMethod);

    xmlSettings.addChildElement(xmlRenderSettings);

    XmlElement* xmlLastSession;
    for(int si = 0; si < numLastSessions; si++)
    {
        xmlLastSession = new XmlElement(T("Session") + String(si));
        xmlLastSession->setAttribute(T("FilePath"), lastsessions[si]->getFullPathName());
        xmlSettings.addChildElement(xmlLastSession);
    }

#ifdef USE_JUCE_AUDIO
    XmlElement* xmlAudio = audioDeviceManager->createStateXml();
    if(xmlAudio != NULL)
       xmlSettings.addChildElement(xmlAudio);
#endif

    String  sFilePath(".\\settings.xml");
    File    sFile(sFilePath);
    xmlSettings.writeToFile(sFile, String::empty);
}

void ExitFromApp()
{
    SaveSettings();
    JUCEApplication::quit();
}

//==============================================================================
class ScanThread  : public ThreadWithProgressWindow
{
public:
    ScanThread()
        : ThreadWithProgressWindow (T("Scanning for new plugins..."),
                                    false,
                                    false)
    {
        setStatusMessage (T("Scanning for new plugins..."));
    }

    ~ScanThread()
    {
    }

    void run()
    {
        //setProgress (-1.0); // setting a value beyond the range 0 -> 1 will show a spinning bar..
        //setStatusMessage (T("Preparing to do some stuff..."));
        //wait (2000);

        Init_ScanForVST(this);

        //const int thingsToDo = 10;

            // must check this as often as possible, because this is
            // how we know if the user's pressed 'cancel'
            //if (threadShouldExit())
                //break;

            // this will update the progress bar on the dialog box
            //setProgress (i / (double) thingsToDo);
    }
};

void PlaceCursor()
{
    C.PlaceCursor();
}

void R(int r)
{
    MC->refresh |= r;
}

NoteInstance* CreateElement_Note(Instrument* instr, bool add, bool preventundo)
{
    NoteInstance* ii = instr->CreateNoteInstance();

    if(add)
    {
        if(C.patt == aux_panel->workPt && aux_panel->workPt->autopatt == true && aux_panel->relative_oct)
        {
            ii->ed_note->SetRelative(true);
        }
        AddNewElement(ii, preventundo);
    }
    return ii;
}

NoteInstance* CreateElement_Instance_byChar(char character)
{
    Instrument* instr = C.patt->ibound != NULL ? C.patt->ibound : current_instr;
    C.last_char = character;
	NoteInstance* ii = CreateElement_Note(instr, true);

	ii->ed_note->visible = true;
    ii->Switch2Param(ii->ed_note);
    ii->ed_note->ProcessChar(character);
    ii->ed_note->Kreview(character);
    C.last_note = ii->ed_note->value;
	return ii;
}

NoteInstance* CreateElement_Instance(bool preview)
{
    NoteInstance* ii = NULL;
    Instrument* instr = C.patt->ibound != NULL ? C.patt->ibound : current_instr;
    if(instr != current_instr)
    {
        ChangeCurrentInstrument(instr);
    }

    if(instr == NULL)
    {
        instr = first_instr;
        if(instr != NULL)
        {
            // kakaya-to nezdorovaya hueta
            Preview_Add(NULL, instr, -1,  -1, NULL, NULL, NULL);
        }
        return NULL;
    }
    else
    {
        ii = CreateElement_Note(instr, true);
        ii->loc_vol->SetNormalValue(instr->last_vol);
        ii->loc_pan->SetNormalValue(instr->last_pan);

        ii->ed_note->ProcessChar(C.last_char);
        if(preview)
            ii->ed_note->Kreview(-1);
    }

    return ii;
}

NoteInstance* CreateElement_Instance_special(Pattern* patt, Instrument *instr, int note, float vol, float starttick, float endtick, int line)
{
    Pattern* currPatt = C.patt;
    float currTick = CTick;
    int currLine = CLine;

    CTick = starttick;
    CLine = line;
    C.patt = patt;

    NoteInstance* ii = CreateElement_Note(instr, false);

    ii->end_tick = endtick;
    ii->ed_note->SetValue(note);
    ii->loc_vol->SetNormalValue(vol);
    AddNewElement(ii, false);

    C.patt = currPatt;
    CTick = currTick;
    CLine = currLine;

    return ii;
}

extern SlideNote* CreateElement_SlideNote(bool add)
{
    bool rel = false;
    if(C.patt == aux_panel->workPt && aux_panel->workPt->autopatt == true && aux_panel->relative_oct)
    {
        rel = true;
    }

    SlideNote* sl = new SlideNote(C.last_note, NULL);
    sl->ed_note->SetRelative(rel);
    if(M.qsize > 0)
    {
        sl->tick_length = M.qsize;
        sl->end_tick = sl->start_tick + sl->tick_length;
    }
    else
    {
        sl->tick_length = 0.1f;
        sl->end_tick = sl->start_tick + sl->tick_length;
    }

    if(add)
    {
        AddNewElement(sl, false);
    }

    return sl;
}

Txt* CreateElement_TextString()
{
    Txt* t = new Txt;
    AddNewElement(t, false);
    return t;
}

Pattern* CreateElement_PianorollPattern(int x1, int x2, int y1, int y2)
{
    char* pname = GetOriginalPatternName();
    Pattern* pt = new Pattern(pname, (int)x1 + GridX1, (int)x2 + GridX1, (int)y1 + GridY1, (int)y2 + GridY1 - 1, true);
    pt->ptype = Patt_Pianoroll;
    pt->folded = true;

    AddNewElement(pt, false);
    pt->AddSelectedElements();

    delete pname;
    return pt;
}

Pattern* CreateElement_PianorollPattern(float x1, float x2, int tr1, int tr2)
{
    char* pname = GetOriginalPatternName();
    Pattern* pt = new Pattern(pname, x1, x2, tr1, tr2, true);
    delete pname;

    pt->ptype = Patt_Pianoroll;
    pt->folded = true;
    //pt->proll = new Pianoroll(pt, 0xFFFF, 30);

    AddNewElement(pt, false);
    return pt;
}

Pattern* CreateElement_Pattern(int x1, int x2, int y1, int y2)
{
    char* pname = GetOriginalPatternName();
    Pattern* ptmain = new Pattern(pname, (int)x1 + GridX1, (int)x2 + GridX1, (int)y1 + GridY1, (int)y2 + GridY1 - 1, true);
    AddOriginalPattern(ptmain);
    Pattern* pt = new Pattern(pname, (int)x1 + GridX1, (int)x2 + GridX1, (int)y1 + GridY1, (int)y2 + GridY1 - 1, false);
    ptmain->AddDerivedPattern(pt);
    ptmain->AddSelectedElements();
    GetPatternNameImage(ptmain);
    AddNewElement(pt, false);
    delete pname;
    return pt;
}

Pattern* CreateElement_Pattern(char* pname, float tk1, float tk2, int tr1, int tr2)
{
    Pattern* ptmain = new Pattern(pname, tk1, tk2, tr1, tr2, true);
    AddOriginalPattern(ptmain);
    Pattern* pt = new Pattern(pname, tk1, tk2, tr1, tr2, false);
    ptmain->AddDerivedPattern(pt);

    AddNewElement(pt, false);
    return pt;
}

Pattern* CreateElement_DerivedPattern(Pattern* ptmain, float tk1, float tk2, int tr1, int tr2)
{
    Pattern* pt = new Pattern(ptmain->name->string, tk1, tk2, tr1, tr2, false);
    pt->ptype = ptmain->ptype;
    if(pt->ptype == Patt_Pianoroll)
    {
        pt->offs_line = NUM_PIANOROLL_LINES - 74;
    }
    pt->last_pianooffs = pt->offs_line;
	pt->Update();

    ptmain->AddDerivedPattern(pt);

    AddNewElement(pt, false);
    return pt;
}

extern Pattern* CreateElement_Pattern(float tk1, float tk2, int tr1, int tr2, PattType pattype = Patt_Grid)
{
    char* pname = GetOriginalPatternName();
    Pattern* ptmain = new Pattern(pname, tk1, tk2, tr1, tr2, true);
    AddOriginalPattern(ptmain);
    GetPatternNameImage(ptmain);
    Pattern* pt = new Pattern(pname, tk1, tk2, tr1, tr2, false);
    ptmain->AddDerivedPattern(pt);
    ptmain->ptype = pt->ptype = pattype;
    if(pattype == Patt_StepSeq)
    {
        // Pattern-original should be referenced to instruments
        aux_panel->PopulatePatternWithInstruments(ptmain);
    }
    else if(pattype == Patt_Pianoroll)
    {
        pt->offs_line = NUM_PIANOROLL_LINES - 74;
    }
    pt->last_pianooffs = pt->offs_line;

    AddNewElement(pt, false);
    delete pname;
    return pt;
}

Slide* CreateElement_Slide(int semitones)
{
    Slide* sl = new Slide(semitones);
    AddNewElement(sl, false);
    sl->CalcSlide();
    return sl;
}

Transpose* CreateElement_Transpose(int semitones)
{
    Transpose* tr = new Transpose(semitones);
    AddNewElement(tr, false);
    return tr;
}

Muter* CreateElement_Muter()
{
    Muter* m = new Muter;
    AddNewElement(m, false);
    return m;
}

Break* CreateElement_Break()
{
    Break* b = new Break;
    AddNewElement(b, false);
    return b;
}

Repeat* CreateElement_Repeater()
{
    Repeat* r = new Repeat();
    AddNewElement(r, false);
    return r;
}

Reverse* CreateElement_Reverse()
{
    Reverse* r = new Reverse();
    AddNewElement(r, false);
    return r;
}

Vibrate* CreateElement_Vibrate()
{
    Vibrate* v = new Vibrate;
    AddNewElement(v, false);
    return v;
}

Element* CreateElement_CommonSymbol(ElemType eltype, bool add, bool preventundo)
{
    Element* el = NULL;
    switch(eltype)
    {
        case El_Mute:
        {
            el = new Muter;
        }break;
        case El_Break:
        {
            el = new Break;
        }break;
        case El_SlideNote:
        {
            el = new SlideNote(baseNote, NULL);
        }break;
        case El_Slider:
        {
            el = new Slide(0);
        }break;
        case El_Reverse:
        {
            el = new Reverse;
        }break;
        case El_Vibrate:
        {
            el = new Vibrate;
        }break;
        case El_Transpose:
        {
            el = new Transpose(0);
        }break;
    }

    if(el != NULL && add)
    {
        AddNewElement(el, preventundo);
    }

	return el;
}

Command* CreateElement_Command(Scope* scope, bool preventundo)
{
    Command* c = new Command(scope);
    AddNewElement(c, preventundo);
    return c;
}

Command* CreateElement_Command(Scope* scope, CmdType type)
{
    Command* c = new Command(scope, type);
    AddNewElement(c, false);
    return c;
}

int GetBottomElementLine()
{
    Element* pElem = firstElem;
    int line = 1;

    while(pElem != NULL)
    {
        if (pElem->IsPresent() && pElem->patt == field_pattern && pElem->track_line > line)
        {
            line = pElem->track_line;
        }
        pElem = pElem->next;
    }

    return line;
}

void UpdateNavBarsData()
{
    Element* pElem = firstElem;
    long maxFrame = 0;
    float ftick = 0;
    GetLastElementEndFrame(&maxFrame);
    ftick = Frame2Tick(maxFrame);
    lastFrame = maxFrame;

    /*
    float tsec = (float)lastFrame/fSampleRate;
    total_min = int(tsec/60);
    total_sec = int(tsec)%60;

    CP->tmarea->Change(); 
    */

    float btick;
    if(C.loc == Loc_MainGrid)
    {
        btick = CTick;
    }
    else
    {
        btick = C.fieldposx;
    }

    float oldlen = main_bar->full_len;
    float newlen = jmax(ftick, btick) + 12;
    main_bar->full_len = newlen;
    if(oldlen != newlen)
    {
        R(Refresh_MainBar);
    }

    oldlen = main_v_bar->full_len;
	int dpixlen = (main_v_bar->pixlen - main_v_bar->barpixlen);
	float cf;
	if(dpixlen <= 0)
		cf = 1;
    else 
		cf = float(main_v_bar->pixlen)/(main_v_bar->pixlen - main_v_bar->barpixlen);
    newlen = float(GetBottomElementLine())*cf + 1;
    main_v_bar->full_len = newlen;
    if(oldlen != newlen)
    {
        R(Refresh_MainVBar);
    }

    pElem = aux_panel->workPt->first_elem;
    maxFrame = 0;
    ftick = 0;
    while(pElem != NULL)
    {
        if (pElem->IsPresent() && pElem->end_frame > maxFrame)
        {
            maxFrame = pElem->end_frame;
        }
        pElem = pElem->patt_next;
    }
    ftick = Frame2Tick(maxFrame);

    btick = 0;
    if(C.loc == Loc_SmallGrid)
    {
        btick = CTick;
    }
    if(aux_panel->workPt->tick_length > btick)
    {
        btick = aux_panel->workPt->tick_length;
    }

    oldlen = aux_panel->h_sbar->full_len;
    newlen = jmax(ftick, btick) + 12;
    aux_panel->h_sbar->full_len = newlen;
}

void UpdateAuxNavBarOnly()
{
    Element* pElem = aux_panel->workPt->first_elem;
    tframe maxFrame = 0;
    float ftick = 0;

    while (pElem != NULL)
    {
        if (pElem->IsPresent() && pElem->end_frame > maxFrame)
        {
            maxFrame = pElem->end_frame;
        }
        pElem = pElem->patt_next;
    }
    ftick = Frame2Tick(maxFrame);

    float btick = 0;
    if(C.loc == Loc_SmallGrid)
    {
        btick = CTick;
    }
    if(aux_panel->workPt->tick_length > btick)
    {
        btick = aux_panel->workPt->tick_length;
    }

    float oldlen = aux_panel->h_sbar->full_len;
    float newlen = jmax(ftick, btick) + 12;
    aux_panel->h_sbar->full_len = newlen;
}

void GetLastElementEndFrame(long *pLastFrame)
{
    Element* pElem = firstElem;
    long ullMaxFrame = 0;

    if (pLastFrame != NULL)
    {
        while (pElem != NULL)
        {
            if (pElem->IsPresent() && pElem->end_frame > ullMaxFrame)
            {
                ullMaxFrame = pElem->end_frame;
            }
            pElem = pElem->next;
        }
        *pLastFrame = ullMaxFrame;
    }
}

void AddNewElement(Element* el, bool preventundo)
{
    if(firstElem == NULL && lastElem == NULL)
    {
        el->prev = NULL;
        el->next = NULL;
        firstElem = el;
    }
    else
    {
        lastElem->next = el;
        el->prev = lastElem;
        el->next = NULL;
    }
    lastElem = el;

    field_pattern->first_elem = firstElem;
    field_pattern->last_elem = lastElem;

    if(el->type != El_Pattern && (C.patt != field_pattern))
    {
        if(C.patt->basePattern != NULL)
            C.patt->basePattern->AddElement(el);
        else
            C.patt->AddElement(el);
    }
    else
    {
        el->Update();
    }

    if((el->type == El_TextString)||
       (el->type == El_Samplent)||
       (el->type == El_GenNote)||
       (el->type == El_SlideNote)||
       (el->type == El_Pattern)||
       (el->type == El_Slider)||
       (el->type == El_Mute)||
       (el->type == El_Break)||
       (el->type == El_Reverse)||
       (el->type == El_Repeat)||
       (el->type == El_Vibrate)||
       (el->type == El_Transpose)||
       (el->type == El_Command))
    {
        MakeTriggersForElement(el, el->patt);
    }

    UpdateNavBarsData();

    if(!preventundo)
        undoMan->DoNewAction(Action_Add, (void*)el, NULL, NULL);

    if(Playing)
    {
        WaitForSingleObject(hAudioProcessMutex, INFINITE);
        pbkMain->UpdateQueuedEv();
        ReleaseMutex(hAudioProcessMutex);
    }
    else if(aux_panel->isPlaying())
    {
        WaitForSingleObject(hAudioProcessMutex, INFINITE);
        pbkAux->UpdateQueuedEv();
        ReleaseMutex(hAudioProcessMutex);
    }
}

void Add_trkdata(Trk* td)
{
    if(first_trkdata == NULL && last_trkdata == NULL)
    {
        td->prev = NULL;
        td->next = NULL;
        first_trkdata = td;
    }
    else
    {
        last_trkdata->next = td;
        td->prev = last_trkdata;
        td->next = NULL;
    }
    last_trkdata = td;

    field_pattern->first_trkdata = first_trkdata;
    field_pattern->last_trkdata = last_trkdata;
}

void AddNewControl(Control* ctrl, Panel* owner)
{
    ctrl->panel = owner;
    if(owner != NULL)
    {
        owner->AddControl(ctrl);
    }

    if(firstCtrl == NULL && lastCtrl == NULL)
    {
        ctrl->prev = NULL;
        ctrl->next = NULL;
        firstCtrl = ctrl;
    }
    else
    {
        lastCtrl->next = ctrl;
        ctrl->prev = lastCtrl;
        ctrl->next = NULL;
    }
    lastCtrl = ctrl;
}

Parameter* GetParamByGlobalIndex(int index)
{
    Parameter* param = firstParam;
    while(param != NULL)
    {
        if(param->globalindex == index)
        {
            return param;
        }
        param = param->gnext;
    }

    return NULL;
}

void UpdateParamIndex()
{
    paramIndex = 0;
    Parameter* param = firstParam;
    while(param != NULL)
    {
        if(param->globalindex >= paramIndex)
            paramIndex = param->globalindex + 1;
        param = param->gnext;
    }
}

void UpdateParamIndices()
{
    int index = 0;
    Parameter* param = firstParam;
    while(param != NULL)
    {
        param->globalindex = index++;
        param = param->gnext;
    }
}

void AddGlobalParam(Parameter* param)
{
    if(firstParam == NULL && lastParam == NULL)
    {
        param->gprev = NULL;
        param->gnext = NULL;
        firstParam = param;
    }
    else
    {
        lastParam->gnext = param;
        param->gprev = lastParam;
        param->gnext = NULL;
    }
    lastParam = param;

    if(ProjectLoadingInProgress == false)
        UpdateParamIndices();
    //param->gindex = paramIndex;
    //paramIndex++;
}

void RemoveGlobalParam(Parameter* param)
{
    if((param == firstParam)&&(param == lastParam))
    {
    	firstParam = NULL;
    	lastParam = NULL;
    }
    else if(param == firstParam)
    {
    	firstParam = param->gnext;
    	firstParam->gprev = NULL;
    }
    else if(param == lastParam)
    {
    	lastParam = param->gprev;
    	lastParam->gnext = NULL;
    }
    else
    {
    	if(param->gprev != NULL)
    	{
    		param->gprev->gnext = param->gnext;
    	}
    	if(param->gnext != NULL)
    	{
    		param->gnext->gprev = param->gprev;
    	}
    }

    UpdateParamIndices();
}

void RemoveControlCommon(Control* ctrl)
{
	if(ctrl->type == Ctrl_Toggle && ((Toggle*)ctrl)->tgtype == Toggle_EffFold)
		int a = 1;

	if(ctrl == M.active_ctrl)
    {
        M.active_ctrl = NULL;
        M.mmode &= ~MOUSE_CONTROLLING;
    }

    if(ctrl != NULL)
	{
        if(ctrl->param != NULL)
        {
            ctrl->param->RemoveControl(ctrl);
        }

        if(ctrl->panel != NULL)
        {
            ctrl->panel->RemoveControl(ctrl);
        }

		if((ctrl == firstCtrl)&&(ctrl == lastCtrl))
		{
			firstCtrl = NULL;
			lastCtrl = NULL;
		}
		else if(ctrl == firstCtrl)
		{
			firstCtrl = ctrl->next;
			firstCtrl->prev = NULL;
		}
		else if(ctrl == lastCtrl)
		{
			lastCtrl = ctrl->prev;
			lastCtrl->next = NULL;
		}
		else
		{
			if(ctrl->prev != NULL)
			{
				ctrl->prev->next = ctrl->next;
			}
			if(ctrl->next != NULL)
			{
				ctrl->next->prev = ctrl->prev;
			}
		}

        delete ctrl;
	}
}

void Add_VU(VU* vu)
{
    if(firstVU == NULL && lastVU == NULL)
    {
        vu->prev = NULL;
        vu->next = NULL;
        firstVU = vu;
    }
    else
    {
        lastVU->next = vu;
        vu->prev = lastVU;
        vu->next = NULL;
    }
    lastVU = vu;
}

void Remove_VU(VU* vu)
{
	if(vu != NULL)
	{
		if(vu == firstVU && vu == lastVU)
		{
			firstVU = NULL;
			lastVU = NULL;
		}
		else if(vu == firstVU)
		{
			firstVU = vu->next;
			firstVU->prev = NULL;
		}
		else if(vu == lastVU)
		{
			lastVU = vu->prev;
			lastVU->next = NULL;
		}
		else
		{
			if(vu->prev != NULL)
			{
				vu->prev->next = vu->next;
			}
			if(vu->next != NULL)
			{
				vu->next->prev = vu->prev;
			}
		}
	}
}

void Add_Panel(Panel* p)
{
    if(firstPanel == NULL && lastPanel == NULL)
    {
        p->prev = NULL;
        p->next = NULL;
        firstPanel = p;
    }
    else
    {
        lastPanel->next = p;
        p->prev = lastPanel;
        p->next = NULL;
    }
    lastPanel = p;
}

Eff* GetEffByIndex(int index)
{
    Eff* eff = first_eff;
    while(eff != NULL)
    {
        if(eff->index == index)
        {
            return eff;
        }
        eff = eff->next;
    }

    return NULL;
}

void UpdateEffIndex()
{
    effIndex = 0;
    Eff* eff = first_eff;
    while(eff != NULL)
    {
        if(eff->index >= effIndex)
            effIndex = eff->index + 1;
        eff = eff->next;
    }
}

void UpdateEffIndices()
{
    int index = 0;
    Eff* eff = first_eff;
    while(eff != NULL)
    {
        eff->index = index++;
        eff = eff->next;
    }
}

void AddEff(Eff* eff)
{
    if(first_eff == NULL && last_eff == NULL)
    {
        eff->prev = NULL;
        eff->next = NULL;
        first_eff = eff;
    }
    else
    {
        last_eff->next = eff;
        eff->prev = last_eff;
        eff->next = NULL;
    }
    last_eff = eff;

    if(ProjectLoadingInProgress == false)
        UpdateEffIndices();
    //eff->gindex = effIndex;
    //effIndex++;

    ChangesIndicate();
}

void RemoveEff(Eff* eff)
{
	if((eff == first_eff)&&(eff == last_eff))
	{
		first_eff = NULL;
		last_eff = NULL;
	}
	else if(eff == first_eff)
	{
		first_eff = eff->next;
		first_eff->prev = NULL;
	}
	else if(eff == last_eff)
	{
		last_eff = eff->prev;
		last_eff->next = NULL;
	}
	else
	{
		if(eff->prev != NULL)
		{
		    eff->prev->next = eff->next;
        }
		if(eff->next != NULL)
		{
		    eff->next->prev = eff->prev;
        }
	}

    if(eff->mixcell != NULL && eff->mixcell->mchan != NULL)
    {
        eff->mixcell->mchan->RemoveEffect(eff);
    }

    delete eff;

    UpdateEffIndices();

    ChangesIndicate();
}

void AddRecordingParam(Parameter* param)
{
    if(first_rec_param == NULL && last_rec_param == NULL)
    {
        param->rec_prev = NULL;
        param->rec_next = NULL;
        first_rec_param = param;
    }
    else
    {
        last_rec_param->rec_next = param;
        param->rec_prev = last_rec_param;
        param->rec_next = NULL;
    }
    last_rec_param = param;
}

void RemoveRecordingParam(Parameter* param)
{
	if((param == first_rec_param)&&(param == last_rec_param))
	{
		first_rec_param = NULL;
		last_rec_param = NULL;
	}
	else if(param == first_rec_param)
	{
		first_rec_param = param->rec_next;
		first_rec_param->rec_prev = NULL;
	}
	else if(param == last_rec_param)
	{
		last_rec_param = param->rec_prev;
		last_rec_param->rec_next = NULL;
	}
	else
	{
		if(param->rec_prev != NULL)
		{
		    param->rec_prev->rec_next = param->rec_next;
        }
		if(param->rec_next != NULL)
		{
		    param->rec_next->rec_prev = param->rec_prev;
        }
	}
}

void UpdateScaledImages()
{
    Pattern* pto = first_base_pattern;
    while(pto != NULL)
    {
        if(!pto->autopatt)
            pto->UpdateScaledImage();
        pto = pto->base_next;
    }
}

void UpdatePatternIndex()
{
    pattIndex = 1;
    Pattern* patt = first_base_pattern;
    while(patt != NULL)
    {
        if(patt->baseindex >= pattIndex)
            pattIndex = patt->baseindex + 1;
        patt = patt->base_next;
    }
}

void UpdatePatternIndices()
{
    int index = 1;
    Pattern* patt = first_base_pattern;
    while(patt != NULL)
    {
        patt->baseindex = index++;
        patt = patt->base_next;
    }
}

Pattern* GetPatternByIndex(int index)
{
    if(index == 0)
        return field_pattern;

    Pattern* patt = first_base_pattern;
    while(patt != NULL)
    {
        if(patt->baseindex == index)
        {
            return patt;
        }
        patt = patt->base_next;
    }

    return NULL;
}

void AddOriginalPattern(Pattern* pattern)
{
    if((first_base_pattern == NULL)&&(last_base_pattern == NULL))
    {
        pattern->base_prev = NULL;
        pattern->base_next = NULL;
        first_base_pattern = pattern;
    }
    else
    {
        last_base_pattern->base_next = pattern;
        pattern->base_prev = last_base_pattern;
        pattern->base_next = NULL;
    }
    last_base_pattern = pattern;

    if(ProjectLoadingInProgress == false)
    {
        UpdatePatternIndices();
    }
}

void RemoveBasePattern(Pattern* pattern)
{
	if(pattern != NULL)
	{
		if((pattern == first_base_pattern)&&(pattern == last_base_pattern))
		{
			first_base_pattern = NULL;
			last_base_pattern = NULL;
		}
		else if(pattern == first_base_pattern)
		{
			first_base_pattern = pattern->base_next;
			first_base_pattern->base_prev = NULL;
		}
		else if(pattern == last_base_pattern)
		{
			last_base_pattern = pattern->base_prev;
			last_base_pattern->base_next = NULL;
		}
		else
		{
			if(pattern->base_prev != NULL)
			{
				pattern->base_prev->base_next = pattern->base_next;
			}
			if(pattern->base_next != NULL)
			{
				pattern->base_next->base_prev = pattern->base_prev;
			}
		}
        delete pattern;

        UpdatePatternIndices();
	}
}

void Add_PEdit(PEdit* pe)
{
    if((firstPE == NULL)&&(lastPE == NULL))
    {
        pe->prev = NULL;
        pe->next = NULL;
        firstPE = pe;
    }
    else
    {
        lastPE->next = (PEdit*)pe;
        pe->prev = lastPE;
        pe->next = NULL;
    }
    lastPE = pe;
}

void Remove_PEdit(PEdit* pe)
{
	if(pe != NULL)
	{
		if((pe == firstPE)&&(pe == lastPE))
		{
			firstPE = NULL;
			lastPE = NULL;
		}
		else if(pe == firstPE)
		{
			firstPE = pe->next;
			firstPE->prev = NULL;
		}
		else if(pe == lastPE)
		{
			lastPE = pe->prev;
			lastPE->next = NULL;
		}
		else
		{
			if(pe->prev != NULL)
			{
				pe->prev->next = pe->next;
			}
			if(pe->next != NULL)
			{
				pe->next->prev = pe->prev;
			}
		}
        delete pe;
	}
}

void Add_AliasRecord(ModuleSubType type, const char* alias)
{
    AliasRecord* ar = new AliasRecord;
    strcpy(ar->alias, alias);
    ar->type = type;

    if(first_alias_record == NULL && last_alias_record == NULL)
    {
        ar->prev = NULL;
        ar->next = NULL;
        first_alias_record = ar;
    }
    else
    {
        last_alias_record->next = ar;
        ar->prev = last_alias_record;
        ar->next = NULL;
    }
    last_alias_record = ar;
}

Element* NextElementToDelete(Element* eltodel)
{
    Element* elnext = eltodel->next;
    while(elnext != NULL && CheckDeletionSkip(elnext, eltodel) == true)
    {
        elnext = elnext->next;
    }
    return elnext;
}

////////////////////////
// Checks whether eltocheck will be deleted per deletion of eltodel
bool CheckDeletionSkip(Element* eltocheck, Element* eltodel)
{
    if(eltocheck == eltodel)
    {
        return true;
    }
    else if(eltodel->type == El_Pattern)
    {
        Pattern* pt = (Pattern*)eltodel;
        if((eltocheck->patt == pt->basePattern || eltocheck->patt == pt) && pt->IsLastChild() == true)
        {
            return true;
        }
    }
    else if(eltocheck->type == El_SlideNote && ((SlideNote*)eltocheck)->parent == eltodel)
    {
        return true;
    }
    else if(eltocheck->type == El_Pattern)
    {
        Pattern* pt = (Pattern*)eltocheck;
        if(eltodel->IsInstance() && pt->parent_trigger != NULL && pt->parent_trigger->el == eltodel)
        {
            return true;
        }
    }

    return false;
}

bool IsElementDeletable(Element* el)
{
    if(el->type == El_Command)
    {
        Command* cmd = (Command*)el;
        if(cmd->cmdtype == Cmd_LocVolEnv || cmd->cmdtype == Cmd_LocPanEnv)
        {
            return false;
        }
    }
    return true;
}

void HardDelete(Element* el, bool preventundo)
{
    if(el == M.active_elem)
    {
        M.active_elem = NULL;
    }

    if(el == C.curElem)
    {
        if(CP->CurrentMode != CP->NoteMode)
        {
           C.ExitToDefaultMode();
        }
        else
        {
           C.ExitToCurrentMode();
        }
    }

    if(el->tg_first != NULL)
    {
        Del_Element_Triggers(el);
    }

    if(el->type != El_Pattern && el->patt != field_pattern)
    {
        el->patt->RemoveElement(el);
    }

    if(el->type == El_SlideNote)
    {
        SlideNote* sl = (SlideNote*)el;
        if(sl->parent != NULL)
            sl->parent->RemoveSlidenote(sl);
    }
    else if(el->IsInstance())
    {
        if(CP->CurrentMode == CP->SlideMode && el == C.slideparent)
        {
            C.ExitToDefaultMode();
        }

        NoteInstance* ii = (NoteInstance*)el;
        SlideNote* sl = ii->first_slide;
        if(sl != NULL)
        {
            while(sl->s_next != NULL)
            {
                sl = sl->s_next;
                DeleteElement((Element*)(sl->s_prev), true, preventundo);
            }
            DeleteElement((Element*)sl, true, preventundo);
        }
    }
    else if(el->type == El_Pattern)
    {
        Pattern* pt = (Pattern*)el;
        pt->DisablePlayback(); // Care should be taken if this pattern's playback is globally listed, to unlist it before deletion.

        if(pt == aux_panel->workPt)
        {
            if(C.loc == Loc_SmallGrid)
            {
                C.ExitToDefaultMode();
                C.SetPattern(field_pattern, Loc_MainGrid);
                C.SetPos(aux_panel->workPt->start_tick, aux_panel->workPt->track_line);
            }
            aux_panel->AuxReset();
            C.PosUpdate();
        }
        else if(C.patt == pt)
        {
            C.ExitToDefaultMode();
            C.SetPattern(field_pattern, Loc_MainGrid);
            C.SetPos(aux_panel->workPt->start_tick, aux_panel->workPt->track_line);
        }

        pt->basePattern->RemoveDerivedPattern(pt);
		// Delete if no childs remaining
        if(pt->basePattern->der_first == NULL) // Autopattern's will never be deleted here (they are deleted together with instrument)
        {
            pt->basePattern->DeleteAllElements(true, preventundo);
            RemoveBasePattern(pt->basePattern);
        }
        else if(pt->tg_internal_first != NULL)
        {
            Del_Pattern_Internal_Triggers(pt);
        }
    }

    if((el == firstElem)&&(el == lastElem))
    {
        firstElem = NULL;
        lastElem = NULL;
    }
    else if(el == firstElem)
    {
        firstElem = el->next;
        firstElem->prev = NULL;
    }
    else if(el == lastElem)
    {
        lastElem = el->prev;
        lastElem->next = NULL;
    }
    else
    {
        if(el->prev != NULL)
        {
            el->prev->next = el->next;
        }
        if(el->next != NULL)
        {
            el->next->prev = el->prev;
        }
    }
    field_pattern->first_elem = firstElem;
    field_pattern->last_elem = lastElem;

    delete el;
}

void SoftUnDelete(Element *el)
{
    el->deleted = false;
    el->UnDelete();

    /*
    if(el->type == El_Samplent || el->type == El_Gennote)
    {
        Instance* ii = (Instance*)el;
        SlideNote* sl = ii->first_slide;
        if(sl != NULL)
        {
            while(sl != NULL)
            {
                SoftUnDelete(sl);
                sl = sl->s_next;
            }
        }
    }
    else if(el->type == El_Pattern)
    {
        Pattern* pt = (Pattern*)el;

        if(pt->IsLastChild() == true)
        {
            Element* el = pt->OrigPt->first_elem;
            while(el != NULL)
            {
                SoftUnDelete(el);
                el = el->patt_next;
            }
        }
    }
    */
}

void SoftDelete(Element *el, bool preventundo)
{
    el->deleted = true;
    el->picked = false;
    el->Deselect();
    el->highlighted = false;

    if(el == M.active_elem)
    {
        M.active_elem = NULL;
    }
    
    if(el == C.curElem)
    {
        C.ExitToCurrentMode();
    }

    el->Delete();

    if(el->type == El_Samplent || el->type == El_GenNote)
    {
        if(CP->CurrentMode == CP->SlideMode && el == C.slideparent)
        {
            C.ExitToDefaultMode();
        }

        NoteInstance* ii = (NoteInstance*)el;
        SlideNote* sl = ii->first_slide;
        if(sl != NULL)
        {
            while(sl->s_next != NULL)
            {
                sl = sl->s_next;
                DeleteElement((Element*)(sl->s_prev), false, preventundo);
            }
            DeleteElement((Element*)sl, false, preventundo);
        }
    }
    else if(el->type == El_Command)
    {
        Command* cmd = (Command*)el;
        if(cmd->IsEnvelope() && cmd->param != NULL && cmd->param->isRecording())
        {
            cmd->param->FinishRecording();
        }
    }
    else if(el->type == El_Pattern)
    {
        Pattern* pt = (Pattern*)el;
        pt->DisablePlayback();
        if(pt == aux_panel->workPt)
        {
            if(C.loc == Loc_SmallGrid)
            {
                C.ExitToDefaultMode();
                C.SetPattern(field_pattern, Loc_MainGrid);
                C.SetPos(aux_panel->workPt->start_tick, aux_panel->workPt->track_line);
            }
            aux_panel->AuxReset();
            C.PosUpdate();
        }
        else if(C.patt == pt)
        {
            C.ExitToDefaultMode();
            C.SetPattern(field_pattern, Loc_MainGrid);
            C.SetPos(aux_panel->workPt->start_tick, aux_panel->workPt->track_line);
        }

        if(pt->IsLastChild() == true)
        {
            pt->basePattern->DeleteAllElements(false, preventundo);
        }
    }
}

void CutElement(Element* el)
{
    if(IsElementDeletable(el) == true)
    {
        el->MarkAsCopied();
        undoMan->DoNewAction(Action_Delete, (void*)el, NULL, NULL);
    }
}

bool DeleteElement(Element* el, bool hard, bool preventundo)
{
    bool harddeleted = false;
    if(IsElementDeletable(el) == true)
    {
        if(hard == true)
        {
            /*
            if(Playing == true || gAux->playing == true || el->IsPlaying() ||
               (el->type == El_Pattern && Preview_StopPerPattern(el)))
            {
                el->to_be_deleted = true;
                SoftDelete(el, true);
            }
            else*/ // Do forced deletion with trigger deactivation
            {
                HardDelete(el, preventundo);
                harddeleted = true;
            }
        }
        else if(!preventundo)
        {
            undoMan->DoNewAction(Action_Delete, (void*)el, NULL, NULL);
        }
        else
        {
            SoftDelete(el, preventundo);
        }
    }

    UpdateNavBarsData();

    return harddeleted;
}

/*
void Add_SoundFont(const char* path, const char* name, const char* alias)
{
    sfBankID bank_id = LoadSF2Bank((char*)path);
    SoundFont* sf = new SoundFont(bank_id);

    strcpy(sf->sf_file_path, path);
    strcpy(sf->sf_name, name);

    sf->alias = new TString((char*)alias, false, NULL);
    sf->alias->instr = sf;
    Add_PEdit(sf->alias);

    instr[num_instrs] = (Instrument*)sf;
    if(num_instrs == 0)
    {
        current_instr = (Instrument*)sf;
    }
    num_instrs++;
}
*/

void Queue_Instr_Deleted(Instrument* i)
{
    i->QueueDeletion();

    if(first_instr_del == NULL && last_instr_del == NULL)
    {
        i->del_prev = NULL;
        i->del_next = NULL;
        first_instr_del = i;
    }
    else
    {
        last_instr_del->del_next = i;
        i->del_prev = last_instr_del;
        i->del_next = NULL;
    }
    last_instr_del = i;
}

void Dequeue_Instr_Deleted(Instrument* i)
{
    if((i == first_instr_del)&&(i == last_instr_del))
    {
    	first_instr_del = NULL;
    	last_instr_del = NULL;
    }
    else if(i == first_instr_del)
    {
    	first_instr_del = i->del_next;
    	first_instr_del->del_prev = NULL;
    }
    else if(i == last_instr)
    {
    	last_instr_del = i->del_prev;
    	last_instr_del->del_next = NULL;
    }
    else
    {
    	if(i->del_prev != NULL)
    	{
    	    i->del_prev->del_next = i->del_next;
        }
    	if(i->del_next != NULL)
    	{
    	    i->del_next->del_prev = i->del_prev;
        }
    }
}

void CleanupInstruments()
{
    Instrument* instrnext;
    Instrument* instr = first_instr_del;
    while(instr != NULL)
    {
        instrnext = instr->del_next;
        if(instr->DereferenceElements() == true)
        {
            Dequeue_Instr_Deleted(instr);
            delete instr;
        }
        instr = instrnext;
    }
}

void CleanupEffects()
{
    Eff* eff_d;
    Eff* eff = first_eff;
    while(eff != NULL)
    {
        if(eff->to_be_deleted == true)
        {
            eff_d = eff;
            eff = eff->next;
            RemoveEff(eff_d);
            continue;
        }
        eff = eff->next;
    }
}

void CleanupElements()
{
    Element* elnext;
    Element* el = firstElem;
    while(el != NULL)
    {
        if(el->to_be_deleted == true)
        {
            elnext = NextElementToDelete(el);
            DeleteElement(el, true, true);
            el = elnext;
			continue;
        }
        el = el->next;
    }
}

void CleanupAll()
{
    CleanupElements();
    CleanupEffects();
    CleanupEvents(field_pattern);
    CleanupInstruments();

    Element* el = firstElem;
    while(el != NULL)
    {
        if(el->type == El_Pattern)
        {
            Pattern* pt = (Pattern*)el;
            if(pt->basePattern != NULL && pt->basePattern->autopatt == true)
            {
                CleanupEvents(pt);
            }
        }
        el = el->next;
    }
}

void LocateElement(Element* el)
{
    Trigger* tg = el->tg_first;
    while(tg != NULL)
    {
        Locate_Trigger(tg);
        tg = tg->el_next;
    }
}

void Relocate_Element(Element* el)
{
    Trigger* tg = el->tg_first;
    while(tg != NULL)
    {
        RemoveTriggerFromEvent(tg);
        tg = tg->el_next;
    }
    LocateElement(el);
}

void UpdateAllElements(Pattern* pt, bool tonly)
{
    if(pt == field_pattern)
    {
        Element* el = firstElem;
        while(el != NULL)
        {
			if(el->IsPresent())
			{
				el->Update(tonly);
			}
			el = el->next;
        }

        Instrument* i = first_instr;
        while(i != NULL)
        {
            if(i->autoPatt != NULL)
            {
                i->autoPatt->Update(tonly);
            }
            i = i->next;
        }
    }
    else
    {
        Element* el = pt->first_elem;
        while(el != NULL)
        {
			if(el->IsPresent())
			{
				el->Update(tonly);
			}
            el = el->patt_next;
        }
    }
}

void CheckPosForNote()
{
    Element* el = firstElem;
    while(el != NULL)
    {
/*      if((el->type == El_Samplent)&&(el->start_tick == CTick)&&(el->start_line == CLine))
        {
            // TODO: switch to note editing
        }
        */
        el = el->next;
    }
}

bool CursModeCorrespondsToCursorMode()
{
    Butt* btmode;
    switch(C.mode)
    {
        case CMode_TxtDefault:
            btmode = CP->UsualMode;
            break;
        case CMode_NotePlacing:
            btmode = CP->NoteMode;
            break;
        case CMode_Sliding:
            btmode = CP->SlideMode;
            break;
        case CMode_FastMode:
            btmode = CP->FastMode;
            break;
        case CMode_FastBrushMode:
            btmode = CP->BrushMode;
            break;
		default:
			btmode = NULL;
			break;
    }

    if(btmode == CP->CurrentMode)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void SwitchInputMode(CursMode mode)
{
    CP->CurrentMode->Refresh();

    bool jpe_override = false;
    if(C.mode == CMode_JustParamEdit)
    {
        jpe_override = true;
    }

    switch(mode)
    {
        case CMode_TxtDefault:
        {
            CP->CurrentMode->pressed = false;
            CP->UsualMode->pressed = true;
            CP->CurrentMode = CP->UsualMode;

            if(C.mode == CMode_ElemParamEdit)
            {
                if(C.curParam->type == Param_Note)
                {
    				if(C.curElem->type == El_SlideNote)
    				{
    					C.mode = CMode_TxtDefault;
    				}
    				else if(C.curElem->type == El_Samplent || C.curElem->type == El_GenNote)
    				{
    					NoteInstance* ii = (NoteInstance*)C.curElem;
                        if(ii->active_param != NULL)
                        {
    					    ii->ProcessKey(key_escape, 0);
                        }
                        else
                        {
                            C.mode = CMode_TxtDefault;
                        }
    				}
                }
                else if(C.curParam->type == Param_TString)
                {
                    // Do not change cursor mode in pattern name case
                }
			}
			else
			{
				C.mode = CMode_TxtDefault;
			}
        }break;
        case CMode_NotePlacing:
        {
            if(current_instr != NULL)
            {
                CP->CurrentMode->pressed = false;
                CP->NoteMode->pressed = true;
                CP->CurrentMode = CP->NoteMode;

                if(C.mode == CMode_ElemEdit && 
                  (C.curElem->type == El_Samplent || C.curElem->type == El_GenNote))
                {
                    NoteInstance* ii = (NoteInstance*)C.curElem;
                    ii->ProcessKey(key_escape, 0);
                }
                else
                {
                    C.mode = CMode_NotePlacing;
                }
            }
        }break;
        case CMode_Sliding:
        {
            CP->CurrentMode->Release();
            CP->CurrentMode = CP->SlideMode;
            CP->CurrentMode->Press();

            C.ExitToSlidingMode(NULL, 60);
        }break;
        case CMode_FastMode:
        {
            CP->CurrentMode->pressed = false;
            CP->FastMode->pressed = true;
            CP->CurrentMode = CP->FastMode;
        }break;
        case CMode_FastBrushMode:
        {
            CP->CurrentMode->pressed = false;
            CP->BrushMode->pressed = true;
            CP->CurrentMode = CP->BrushMode;
            C.mode = CMode_FastBrushMode;
		}break;
        default:
        {
            if(CP->CurrentMode == CP->UsualMode)
            {
                C.mode = CMode_TxtDefault;
            }
            else if(CP->CurrentMode == CP->NoteMode)
            {
                C.mode = CMode_NotePlacing;
            }
            else if(CP->CurrentMode == CP->SlideMode)
            {
                C.mode = CMode_Sliding;
            }
            else if(CP->CurrentMode == CP->FastMode)
            {
                C.mode = CMode_FastMode;
            }
            else if(CP->CurrentMode == CP->BrushMode)
            {
                C.mode = CMode_FastBrushMode;
            }
        }break;
    }

    if(jpe_override)
    {
        C.mode = CMode_JustParamEdit;
    }

    CP->CurrentMode->Refresh();
    R(Refresh_Buttons);

    if(C.loc == Loc_MainGrid)
    {
        R(Refresh_GridContent);
    }
    else if(C.loc == Loc_SmallGrid)
    {
        R(Refresh_AuxContent);
    }
}

void ToggleFastMode()
{
    if(current_instr == NULL)
    {
        CP->FastMode->pressed = false;
    }
    else
    {
        if(CP->FastMode->pressed == false)
        {
            CP->FastMode->pressed = true;
        }
        else
        {
            CP->FastMode->pressed = false;

            if(CP->BrushMode->pressed == true)
            {
                CP->BrushMode->pressed = false;
            }
        }
    }
}

void ToggleTextModeToNoteMode()
{
    if(C.mode == CMode_TxtDefault)
    {
        SwitchInputMode(CMode_NotePlacing);
    }
    else
    {
        SwitchInputMode(CMode_TxtDefault);
    }
}

Element* IsElemExists(float tick, int trknum, Pattern* pt)
{
    Element* el = firstElem;
    while(el != NULL)
    {
        if(el->IsPresent() && el->patt == pt && el->start_tick == tick && el->track_line == trknum)
        {
            return el;
        }
        el = el->next;
    }

    return NULL;
}

/*==================================================================================================
BRIEF: This function processes lane with value columns (volumes, pannings, etc.) depending on
certain mouse position.

PARAMETERS:
[IN]
    mouse_x, mouse_y - mouse coordinates.
    paramtype - type of data to process.
    loc - grid location.
    buffframe - global buffering offset.
    pt - pattern filter.
    lx1, int ly1, int lx2, int ly2 - area limits.

[OUT]
N/A

OUTPUT: N/A
==================================================================================================*/
void ProcessColLane(int mouse_x, int mouse_y, ParamType paramtype, Loc loc, Trk* trk, Pattern* pt, int lx1, int ly1, int lx2, int ly2, bool deflt)
{
    int vh, auxed;
    float nv;
    Samplent*   smpdata;
    SlideNote*  sldata;
    GenNote*    gndata;
    Pattern*    pdata;
    bool        go;

    Trk* actual_trk;
    if(trk != NULL && trk->trkbunch != NULL)
    {
        actual_trk = trk->trkbunch->trk_start;
    }
    else
    {
        actual_trk = trk;
    }

    int ay = mouse_y - ly1;
    if((M.prev_x == -1) && (M.prev_y == -1))
    {
        M.prev_x = mouse_x;
        M.prev_y = ay;
        if(M.RMB == true)
        {
            M.mousefix = true;
        }
    }

    vh = ly2 - ly1;
    bool selectedonly = false;
    if(Num_Selected > 0 && M.selloc == loc)
    {
        selectedonly = true;
    }

    Element* el = firstElem;
    while(el != NULL)
    {
        if(loc == Loc_SmallGrid)
        {
            auxed = GridXS1 + el->auxauxed;
        }
        else if(loc == Loc_MainGrid)
        {
            auxed = GridX1 + el->gridauxed;
        }
        else
        {
            auxed = StX1 + el->gridauxed;
        }

        if(el->IsPresent() &&
           (pt == NULL || el->patt == pt)&& 
           (!selectedonly|| el->selected == true)&&
           (trk == NULL || el->field_trkdata == actual_trk)&&
           (auxed >= lx1 && auxed <= lx2))
        {
            go = false;
            if((M.RMB == false || M.prev_x == mouse_x) && abs(mouse_x - auxed) <= 3)
            {
                nv = (float)(vh - ay)/(float)vh;
                if(nv > 1) 
                {
                    nv = 1;
                }
                if(nv < 0)
                {
                    nv = 0;
                }

                go = true;
            }
            else if((auxed >= M.prev_x && auxed <= mouse_x)||(auxed >= mouse_x && auxed <= M.prev_x))
            {
                nv = (1.0f - (Interpolate_Line((double)M.prev_x, (float)M.prev_y, (double)mouse_x, (float)ay, (double)auxed)/(float)vh));
                if(nv > 1.0f)
                {
                    nv = 1.0f;
                }
                if(nv < 0)
                {
                    nv = 0;
                }

                go = true;
            }

			if(go)
			{
				if(paramtype == Param_Vol)
				{
				    if(deflt)
                    {
                        nv = 1.0f/VolRange;
                    }

                    Vol* locvol;
					if(el->type == El_Samplent)
					{
						smpdata = (Samplent*)el;
						locvol = smpdata->loc_vol;
					}
					if(el->type == El_GenNote)
					{
						gndata = (GenNote*)el;
						locvol = gndata->loc_vol;
					}
					else if(el->type == El_SlideNote)
					{
						sldata = (SlideNote*)el;
						locvol = sldata->loc_vol;
					}
					else if(el->type == El_Pattern)
					{
						pdata = (Pattern*)el;
						locvol = pdata->vol_local;
					}

                    //locvol->SetNormalValue(nv*VolRange);
                    undoMan->DoNewAction(Action_ParamChange, (void*)locvol, locvol->val, nv*VolRange, 0, 0);
                    if(el->IsInstance())
                    {
                        NoteInstance* ii = (NoteInstance*)el;
                        ii->instr->SetLastVol(locvol->val);
                    }
				}
				else if(paramtype == Param_Pan)
				{
				    if(deflt)
                    {
                        nv = 0.5f;
                    }

                    Pan* locpan;
					if(el->type == El_Samplent)
					{
						smpdata = (Samplent*)el;
						locpan = smpdata->loc_pan;
					}
					if(el->type == El_GenNote)
					{
						gndata = (GenNote*)el;
						locpan = gndata->loc_pan;
					}
					else if(el->type == El_SlideNote)
					{
						sldata = (SlideNote*)el;
						locpan = sldata->loc_pan;
					}
					else if(el->type == El_Pattern)
					{
						pdata = (Pattern*)el;
						locpan = pdata->pan_local;
					}

					//locpan->SetNormalValue(1 - nv*2);
                    undoMan->DoNewAction(Action_ParamChange, (void*)locpan, locpan->val, nv*2 - 1, 0, 0);
                    if(el->IsInstance())
                    {
                        NoteInstance* ii = (NoteInstance*)el;
                        ii->instr->SetLastPan(locpan->val);
                    }
				}
			}
        }
        el = el->next;
    }

    if(M.mousefix == false)
    {
        M.prev_x = mouse_x;
        M.prev_y = ay;
    }
}

void DeletePointedElement(int mprevx, int mprevy, int mx, int my)
{
    if(mprevx == -1 || mprevy == -1)
    {
        Element* el = firstElem;
        while(el != NULL)
        {
            if(el->IsPointed(mx, my) == true)
            {
                DeleteElement(el, false, false);
                break;
            }
            el = el->next;
        }
    }
    else
    {
        Element* el = firstElem;
        Element* elemnext;
		while(el != NULL)
        {
            if(el->IsPresent() == true && el->visible && 
               CheckPlaneCrossing(el->abs_area.x1 - GridXS1, 
                                  el->abs_area.y1 - GridYS1, 
                                  el->abs_area.x2 - GridXS1, 
                                  el->abs_area.y2 - GridYS1, 
                                  mprevx - GridXS1, 
                                  mprevy - GridYS1, 
                                  mx - GridXS1, 
                                  my - GridYS1) == true)
            {
                elemnext = NextElementToDelete(el);
                DeleteElement(el, false, false);
                el = elemnext;
                continue;
            }
            el = el->next;
        }
    }
}

void DeletePointedAuxElement(int mprevx, int mprevy, int mx, int my)
{
    if(mprevx == -1 || mprevy == -1)
    {
        Element* el = aux_panel->workPt->basePattern->first_elem;
        while(el != NULL)
        {
            if(el->IsPointed(mx, my) == true)
            {
                DeleteElement(el, false, false);
                break;
            }
            el = el->patt_next;
        }
    }
    else
    {
        Element* el = aux_panel->workPt->basePattern->first_elem;
        Element* elemnext;
		while(el != NULL)
        {
            if(el->IsPresent() == true && el->auxvisible &&
               CheckPlaneCrossing(el->abs_area.x1 - GridXS1, 
                                  el->abs_area.y1 - GridYS1, 
                                  el->abs_area.x2 - GridXS1, 
                                  el->abs_area.y2 - GridYS1, 
                                  mprevx - GridXS1, 
                                  mprevy - GridYS1, 
                                  mx - GridXS1, 
                                  my - GridYS1) == true)
            {
                elemnext = aux_panel->workPt->basePattern->NextElementToDelete(el);
                DeleteElement(el, false, false);
                el = elemnext;
                continue;
            }
            el = el->patt_next;
        }
    }
}

bool ResetHighlights(bool* redrawmain, bool* redrawaux)
{
	bool changed = false;
    Element* el = firstElem;
    while(el != NULL)
    {
        if(el->IsPresent() && el->highlighted == true)
        {
            el->highlighted = false;
            if(el->patt == field_pattern)
                *redrawmain = true; 
            else
                *redrawaux = true;

			changed = true;
        }
        el = el->next;
    }
	return changed;
}

Element* CheckHighlightsByEvents(Event* event, int mx, int my)
{
    bool pointed;
    Element* pel = NULL;
    Element* el = NULL;
    Trigger* tg;
    Event* ev = event;
    while(ev != NULL)
    {
        tg = ev->tg_first;
        while(tg != NULL)
        {
            if(tg->el->IsPresent() && 
               tg->activator == true && 
               (tg->el->auxvisible || tg->el->visible))
            {
                el = tg->el;
                pointed = el->IsPointed(mx, my);
                if(pointed == true && el->IsPresent() && 
                    !(!el->IsCoveringEffect() && pel != NULL && pel->IsCoveringEffect() && pel->IsPointed(mx, my)))
                {
                    if(pel != NULL && pel->highlighted == true)
                    {
                        pel->highlighted = false;
                    }
                    pel = el;
                }
                else if(el->highlighted == true /*&& pointed == false*/)
                {
                    // Reset previously highlighted element and refresh
                    el->highlighted = false;
                    M.active_elem = NULL;

                    if(el->patt == field_pattern)
                    {
                        R(Refresh_GridContent);
                    }
                    else
                    {
                        R(Refresh_AuxContent);
                    }
                }
            }
			else if(tg->activator == true && tg->el->highlighted == true)
			{
				tg->el->highlighted = false;
			}
            tg = tg->ev_next;
        }
        ev = ev->next;
    }
	return pel;
}

Element* CheckElementsHighlights(int mx, int my)
{
    Element* pel = NULL;
    Element* el = NULL;
    bool    skip;

    if(aux_panel->isPatternMode() && aux_panel->workPt->autopatt == true)
    {
        pel = CheckHighlightsByEvents(aux_panel->workPt->first_ev, mx, my);
    }

    if(pel == NULL)
    {
        pel = CheckHighlightsByEvents(field_pattern->first_ev, mx, my);
    }

    if(pel != NULL)
    {
        M.active_elem = pel; // this pointer needs constant refreshing
        skip = false;

        // An envelope can cover elements, so let's check for this
        if(M.mmode & MOUSE_ENVELOPING)
        {
            if(pel->type != El_Command)
            {
                // skip all non commands
                skip = true;
                M.active_elem = NULL;
            }
            else if(((Command*)pel)->IsEnvelope() == false)
            {
                // skip all non envelopes
                skip = true;
                M.active_elem = NULL;
            }
            else
            {
                if(pel->StartTick() < M.active_env->element->StartTick())
                {
                    skip = true;
                    M.active_elem = NULL;
                }
                else
                {
                    skip = false;
                    M.mmode &= ~MOUSE_ENVELOPING;
                    M.active_env = NULL;
                    if(M.mmode & MOUSE_RESIZE && M.resize_object == Resize_Envelope)
                        M.mmode &= ~MOUSE_RESIZE;
                }
            }
        }

        if(skip == false && pel->type == El_Pattern)
        {
            Pattern* pt = (Pattern*)pel;
            if((my <= pel->abs_area.y2)&&(pt->is_fat && my >= pel->abs_area.y2 - 6 || 
                !pt->is_fat && my >= pel->abs_area.y2 - 3))
            {
                M.patternbrush = true;
            }
        }

        if(pel->highlighted == false && skip == false)
        {
            pel->highlighted = true;
        
            if(M.loc == Loc_MainGrid)
            {
                R(Refresh_GridContent);
            }
            else if(M.loc == Loc_SmallGrid)
            {
                R(Refresh_AuxContent);
            }
            else
            {
                R(Refresh_GridContent);
                R(Refresh_AuxContent);
            }
        }
        else if(pel->highlighted == true && skip == true)
        {
            M.active_elem = NULL;
            pel->highlighted = false;
        
            if(M.loc == Loc_MainGrid)
            {
                R(Refresh_GridContent);
            }
            else if(M.loc == Loc_SmallGrid)
            {
                R(Refresh_AuxContent);
            }
            else
            {
                R(Refresh_GridContent);
                R(Refresh_AuxContent);
            }
        }
    }

    return pel;
}

PEdit* CheckPEditHighlights(int mx, int my)
{
    PEdit* pe = firstPE;
    PEdit* pe_r = NULL;

    while(pe != NULL)
    {
        pe->highlighted = false;
        if(pe->active == true && pe->IsPointed(mx, my) == true)
        {
            pe->highlighted = true;
            pe_r = pe;
        }
        pe = pe->next;
    }

    return pe_r;
}

void UpdateScrollbarOutput(ScrollBard* sb)
{
    if(sb == main_bar)
    {
        OffsTick = sb->actual_offset;
        C.PosUpdate();
        Selection_UpdateCoords();
        Looping_UpdateCoords();

        if(followPos && (Playing || aux_panel->playing) && 
            (pbkMain->currTick > sb->actual_offset + sb->visible_len ||
             pbkMain->currTick < sb->actual_offset))
        {
            // Cancel follow
            CP->HandleButtDown(CP->BtFollowPlayback);
        }

        R(Refresh_MainBar);
        R(Refresh_Grid);
        if(aux_panel->isVolsPansMode())
        {
            R(Refresh_Aux);
        }
    }
    else if(sb == main_v_bar)
    {
        OffsLine = (int)sb->actual_offset;

        R(Refresh_MainVBar);
        R(Refresh_Grid);
    }
    else if(sb == genBrw->sbar)
    {
        genBrw->main_offs = -sb->actual_offset;
        R(Refresh_GenBrowser);
    }
    else if(sb == mixBrw->sbar)
    {
        mixBrw->main_offs = -(sb->actual_offset);
        R(Refresh_MixCenter);
    }
    else if(sb == IP->sbar)
    {
        /*
        IP->main_offs = -RoundFloat(sb->actual_offset);
        if(IP->main_offs > 0)
        {
            IP->main_offs = 0;
        }*/

        R(Refresh_InstrPanel);
    }
    else if(sb == aux_panel->h_sbar)
    {
        aux_panel->OffsTick = aux_panel->h_sbar->actual_offset;
        C.PosUpdate();
        Selection_UpdateCoords();
        Looping_UpdateCoords();

        sb->drawarea->Change();

        R(Refresh_AuxGrid);
        R(Refresh_AuxScale);
        R(Refresh_SubAux);
    }
    else if(sb == aux_panel->v_sbar)
    {
        aux_panel->OffsLine = (int)aux_panel->v_sbar->actual_offset;
        if(aux_panel->workPt->ptype == Patt_Pianoroll)
        {
            aux_panel->note_offset = NUM_PIANOROLL_LINES - aux_panel->OffsLine - 1;
            if(aux_panel->note_offset < (int)aux_panel->v_sbar->visible_len)
            {
            	aux_panel->note_offset = (int)aux_panel->v_sbar->visible_len;
            }
        }

        aux_panel->workPt->offs_line = aux_panel->OffsLine;

        aux_panel->UpdateDrawInfo();

        aux_panel->DisableStepVUs();

        C.PosUpdate();

        sb->drawarea->Change();

        R(Refresh_AuxGrid);
        R(Refresh_PianoKeys);
    }
    else if(sb == st->sbar)
    {
        st->tick_offset = st->sbar->actual_offset;
		C.PosUpdate();
    }
    else if(sb == aux_panel->mix_sbar)
    {
        aux_panel->hoffs = int(aux_panel->mix_sbar->actual_offset);
		if(aux_panel->hoffs + aux_panel->mix_sbar->visible_len > aux_panel->mix_sbar->full_len)
			int s = 1;

		R(Refresh_Aux);
    }

    UpdateElementsVisibility();
}

bool PickUpElements(int mx, int my, unsigned flags)
{
    bool picked_any = false;
    bool drop_other = false;

    Element* el = firstElem;
    while(el != NULL)
    {
        if(el->IsPresent() && el->highlighted == true && 
           !(el->patt->ptype == Patt_StepSeq && el->selected == false && 
             !(flags & kbd_ctrl || flags & kbd_shift))) // In step sequencer, an element can be picked only if it's selected or with Shift or Ctrl held
        {
            el->picked = true;
            el->xs = mx;
            el->ys = my;

            picked_any = true;
            if(el->selected == false && !(flags & kbd_ctrl))
            {
                drop_other = true;
            }
        }
        el = el->next;
    }

    if(picked_any == true)
    {
        el = firstElem;
        while(el != NULL)
        {
            if(el->IsPresent() && el->selected == true)
            {
                if(drop_other == false)
                {
                    el->picked = true;
                    el->xs = mx;
                    el->ys = my;
                }
                else if(drop_other == true)
                {
                    el->Deselect();
                    el->picked = false;
                }
            }
            el = el->next;
        }
    }

    if(drop_other)
    {
        // Do not reset loop selection if we're working on another grid
		//if(!(M.selmode == Sel_Fragment /* && M.selloc == Loc_MainGrid && M.loc == Loc_SmallGrid */))
		//	Selection_Reset();

		if(M.loc == Loc_SmallGrid)
			R(Refresh_SubAux);
		else if(aux_panel->isVolsPansMode())
			R(Refresh_Aux);
	}

    if(picked_any)
    {
        M.movetick = M.snappedTick;
        M.moveline = M.snappedLine;
    }

    return picked_any;
}

void DropPickedElements()
{
    Element* el = firstElem;
    while(el != NULL)
    {
        if(el->picked == true)
        {
            el->picked = false;
        }
        el = el->next;
    }
}

void ClonePickedToNew(float dtick, int dtrack, float tW, int tH, bool ptcopy)
{
    Element* el = firstElem;
    while(el != NULL)
    {
        el->skip_cloning = false;
        if(el->type == El_SlideNote)
        {
            SlideNote* sn = (SlideNote*)el;
            if(sn->parent != NULL && sn->parent->picked == true)
            {
                el->skip_cloning = true;
            }
        }
        el = el->next;
    }

    Element* nel;
    el = firstElem;
    Element* lel = lastElem;
    while(el != NULL)
    {
        if(el->picked == true)
        {
            if(el->skip_cloning == false)
            {
                C.SaveState();
                if(M.pickloc == Loc_MainGrid)
                {
                    C.patt = field_pattern;
                }
                else if(M.pickloc == Loc_SmallGrid)
                {
                    C.patt = aux_panel->workPt;
                }

                if(el->type == El_Pattern && ptcopy == true)
                {
                    nel = ((Pattern*)el)->Copy(true);
                   ((Pattern*)nel)->basePattern->UpdateScaledImage();
                }
                else
                {
                    nel = el->Clone(true);
                }

                if(el->type == El_Samplent || el->type == El_GenNote)
                {
                    CloneSlideNotes((NoteInstance*)el, (NoteInstance*)nel, true);
                }

                nel->xs = el->xs;
                nel->ys = el->ys;
                nel->picked = true;
                nel->selected = el->selected;
                nel->highlighted = el->highlighted;
                nel->selloc = el->selloc;
                if(el->type == El_SlideNote && ((SlideNote*)el)->parent != NULL)
                {
                    ((SlideNote*)el)->parent->AddSlideNote((SlideNote*)nel);
                }

                undoMan->DoNewAction(Action_Move, (void*)nel, dtick, 0, dtrack, 0);
                //nel->Move(dtick, dtrack);
                nel->xs += int(dtick*tW);
                nel->ys += dtrack*tH;

                C.RestoreState();
            }

            el->highlighted = false;
            el->picked = false;
            el->Deselect();
        }

        if(el == lel)
        {
        	break;
        }
        else
        {
            el = el->next;
        }
    }
}

void CheckElementMoveBlocking(Element* el, float* dtick, int* dtrack)
{
    if(el->patt != aux_panel->workPt->basePattern)
    {
        // Move blocking conditions
        if(el->patt == field_pattern)
        {
            if(el->start_tick >= 0 && el->start_tick + *dtick < 0)
            {
               *dtick = -(el->start_tick);
            }
        }

        if(el->type == El_Pattern && ((Pattern*)el)->is_fat &&
            el->track_line >= 1 && el->track_line + *dtrack < 1)
        {
           *dtrack = -(el->track_line) + 1;
        }
        else if(el->track_line >= 0 && el->track_line + *dtrack < 0)
        {
           *dtrack = -(el->track_line);
        }
        else if(el->track_line + *dtrack > (field_pattern->num_lines - 1))
        {
           *dtrack = field_pattern->num_lines - 1 - el->track_line;
        }
    }
    else if(el->patt == aux_panel->workPt->basePattern)
    {
        // Move blocking conditions
        if(el->start_tick >= 0 && el->start_tick + *dtick < 0)
        {
           *dtick = -(el->start_tick);
        }
    
        if(el->track_line >= 0 && el->track_line + *dtrack < 0)
        {
            *dtrack = -(el->track_line);
        }
        else
        {
            if(aux_panel->workPt->ptype == Patt_Pianoroll)
            {
                if(el->track_line + *dtrack > NUM_PIANOROLL_LINES - 1)
                {
                   *dtrack = NUM_PIANOROLL_LINES - 1 - el->track_line;
                }
            }
            else if(aux_panel->workPt->ptype == Patt_StepSeq)
            {
                Trk* ntrk = GetTrkDataForLine(el->track_line + *dtrack, aux_panel->workPt->basePattern);
                if(ntrk == NULL || ntrk->defined_instr == NULL)
                {
                   *dtrack = 0;
                }
            }
            else if(el->track_line + *dtrack > aux_panel->workPt->basePattern->num_lines - 1)
            {
               *dtrack = aux_panel->workPt->basePattern->num_lines - 1 - el->track_line;
            }
        }
    }

    if(el->type == El_SlideNote && ((SlideNote*)el)->parent != NULL && 
                                   ((SlideNote*)el)->parent->picked == false)
    {
        SlideNote* sl = (SlideNote*)el;
        if((sl->start_tick + *dtick) < sl->parent->start_tick)
        {
           *dtick = sl->parent->start_tick - sl->start_tick;
            //sl->start_tick = sl->parent->start_tick;
        }
    }
}

void MovePickedUpElements(int mx, int my, unsigned flags)
{
    int dtrack, odtrack;
    float dtick, odtick;
    bool changed = false;
    int tH;
    float tW;

    if(M.pickloc == Loc_SmallGrid)
    {
        tW = aux_panel->tickWidth;
        tH = aux_panel->lineHeight;
    }
    else if(M.pickloc == Loc_MainGrid)
    {
        tW = tickWidth;
        tH = lineHeight;
    }
    else
    {
        tW = tickWidth;
        tH = lineHeight;
    }

    float gw = (float)(GridX2 - GridX1)/tickWidth;
    //int gh = (int)(GridY2 - GridY1)/lineHeight;
    float gw1 = (float)(GridXS2 - GridXS1)/aux_panel->tickWidth;
    int gh1 = (int)(GridYS2 - GridYS1)/aux_panel->lineHeight;
    float gw3 = (float)(StX2 - StX1)/st_tickWidth;
    float s_tick;
    int s_track;

    dtick = odtick = M.dragTick - M.movetick;
    dtrack = odtrack = M.dragLine - M.moveline;
    Element* el = firstElem;
    while(el != NULL)
    {
        if(el->IsPresent() && el->picked == true)
        {
            CheckElementMoveBlocking(el, &dtick, &dtrack);
        }
        el = el->next;
    }

    if(dtick != 0 || dtrack != 0)
    {
        el = firstElem;
        while(el != NULL)
        {
            if(el->IsPresent() && el->picked == true)
            {
                s_tick = el->start_tick;
                s_track = el->track_line;
                if(JustMoved == false)
                {
                    if(flags & kbd_ctrl && flags & kbd_shift)
                    {
                        ClonePickedToNew(0, 0, tW, tH, true);
                    }
                    else if(flags & kbd_shift || flags & kbd_ctrl)
                    {
                        ClonePickedToNew(0, 0, tW, tH, false);
                    }
                    else
                    {
                        JustMoved = true;
                    }
                }

                // If we move VST plugin note, then stop this note to avoid infinite sounding
                if(el->IsInstance())
                {
                    NoteInstance* ii = (NoteInstance*)el;
                    if(ii->instr->subtype == ModSubtype_VSTPlugin)
                    {
                        ((VSTGenerator*)(ii->instr))->PostNoteOFF(ii->ed_note->value, 0);
                    }
                }

                if(JustMoved == true)
                {
                    if(!(el->type == El_SlideNote && 
                        ((SlideNote*)el)->parent != NULL &&
                        ((SlideNote*)el)->parent->picked == true))
                    {
                        undoMan->DoNewAction(Action_Move, (void*)el, dtick, 0, dtrack, 0);
                        //el->Move(dtick, dtrack);
                        el->xs += int(dtick*tW);
                        el->ys += dtrack*tH;
                    }
                }
                else
                {
                    JustMoved = true;
                }

                // Hear preview if on pianoroll
                if(el->patt == aux_panel->workPt->basePattern && aux_panel->workPt->ptype == Patt_Pianoroll && 
                  ((el->highlighted == true && el->selected == false)||(M.prepianorolling == true)) && !(s_tick == el->start_tick && s_track == el->track_line))
                {
                    if(el->IsInstance())
                    {
                        Preview_FinishAll();
                        NoteInstance* ii = (NoteInstance*)el;
                        //ii->ed_note->ProcessChar(&C.last_char);
                        ii->ed_note->Kreview(-1);
                    }
                    else if(el->type == El_SlideNote)
                    {
    					SlideNote* sn = (SlideNote*)el;
                        Preview_FinishAll();
    					if(sn->parent != NULL)
    					{
    						NoteInstance* ii = (NoteInstance*)(sn->parent);
						    Preview_Add(ii, NULL, -1, sn->ed_note->value, M.patt, NULL, NULL, true, sn->ed_note->relative);
    					}
    					else
    					{
    						Preview_Add(NULL, current_instr, -1, sn->ed_note->value, M.patt, NULL, NULL, true, sn->ed_note->relative);
    					}
                    }
                }
            }
            el = el->next;
        }
    }

    if(M.loc == Loc_SmallGrid && aux_panel->workPt->ptype == Patt_StepSeq)
    {
        Element* el = firstElem;
        while(el != NULL)
        {
            if(el->IsPresent() && el->picked == true && el->IsInstance())
            {
                NoteInstance* ii = (NoteInstance*)el;
                if(ii->instr != ii->trkdata->defined_instr)
                {
                    el->Deselect();
                    el->picked = false;
                    Element* nel = ReassignInstrument((NoteInstance*)el, el->trkdata->defined_instr);
                    nel->Select();
                    nel->picked = true;
                }
            }
            el = el->next;
        }
    }

    M.movetick = M.dragTick;
    M.moveline = M.dragLine;

    if(M.loc == Loc_MainGrid)
    {
        pbkMain->UpdateQueuedEv();
    }
    else if(M.loc == Loc_SmallGrid)
    {
        pbkAux->UpdateQueuedEv();
    }

    // In case we move edited pattern with looping set
    Looping_UpdateCoords();

    UpdateNavBarsData();
}

Command* CreateCommandFromParam(Parameter* param)
{
    CmdType type;
    switch(param->type)
    {
        case Param_Vol:
            type = Cmd_VolEnv;
            break;
        case Param_Pan:
            type = Cmd_PanEnv;
            break;
        case Param_Default:
        default:
            type = Cmd_ParamEnv;
            break;
    }

    Command* cmd = new Command(param->scope, type, param);

    return cmd;
}

Command* CreateCommandFromControl(Control* ctrl)
{
    CmdType type;
    if(ctrl->type == Ctrl_Toggle)
    {
        switch(((Toggle*)ctrl)->tgtype)
        {
            case Toggle_Mute:
                type = Cmd_Mute;
                break;
            case Toggle_Solo:
                type = Cmd_Solo;
                break;
            case Toggle_MixBypass:
                type = Cmd_Bypass;
                break;
        }
    }
    else
    {
        switch(ctrl->param->type)
        {
            case Param_Vol:
                type = Cmd_VolEnv;
                break;
            case Param_Pan:
                type = Cmd_PanEnv;
                break;
            case Param_Default:
            default:
                type = Cmd_ParamEnv;
                break;
        }
    }

    Command* cmd = new Command(ctrl->scope, type, ctrl->param);

    return cmd;
}

String GetDbString(float value)
{
    if(value == 0)
    {
        return "-INF dB";
    }
    else
    {
        return String::formatted(T("%.2f %s"), amp2dB(value), T("dB"));
    }
}

String GetPanString(float value)
{
    if(value == 0)
    {
        return "Center";
    }
    else
    {
        int pval = abs(int(value*100));
        String str = String(pval);
        if(value < 0)
            str += "L";
        else
            str += "R";
        return str;
    }
}

String GetPercentString(float value)
{
    String str = String(int(value*100));
        str += "%";
    return str;
}

void UpdateControlHint(Control* ct, int mouse_x, int mouse_y)
{
    if(ct->isVolumeSlider())
    {
        String str = GetDbString(ct->param->outval);

        MC->EnableHint(str, mouse_x + 12, mouse_y + 12);
    }
    if(ct->isPanningSlider())
    {
        String str = GetPanString(ct->param->outval);

        MC->EnableHint(str, mouse_x + 12, mouse_y + 12);
    }
    if(ct->type == Ctrl_Knob)
    {
        String str = GetPercentString(ct->param->outval);

        MC->EnableHint(str, mouse_x + 12, mouse_y + 12);
    }
}

void MoveControls(int mouse_x, int mouse_y, unsigned flags)
{
    Toggle* tg = NULL;
    if(M.active_ctrl->type == Ctrl_Toggle)
    {
        tg = (Toggle*)M.active_ctrl;
    }
        
    if(M.active_ctrl->type == Ctrl_Slider ||
       M.active_ctrl->type == Ctrl_Knob ||
      (M.active_ctrl->type == Ctrl_Toggle && (tg->tgtype == Toggle_Mute ||
                                              tg->tgtype == Toggle_Solo ||
                                              tg->tgtype == Toggle_MixBypass)))
    {
        if(flags & kbd_ctrl)
        {
            if(M.active_ctrl->scope != NULL)
            {
                M.mmode &= ~MOUSE_CONTROLLING;
                M.mmode |= MOUSE_DRAG_N_DROPPING;

                M.active_command = CreateCommandFromControl(M.active_ctrl);
                M.drag_data.drag_type = Drag_Command;
                M.drag_data.drag_stuff = (void*)M.active_command;
            }
        }
        else
        {
            Control* ct = (Control*)M.active_ctrl;
            ct->TweakByMouseMove(mouse_x, mouse_y);

            if(ct->isVolumeSlider() || ct->isPanningSlider() || ct->type == Ctrl_Knob)
                UpdateControlHint(ct, mouse_x, mouse_y);
        }
    }
    else if(M.active_ctrl->type == Ctrl_HScrollBar)
    {
        ScrollBard* sb = (ScrollBard*)M.active_ctrl;
        int pixdelta = mouse_x - sb->xs;
        sb->SetPixDelta(pixdelta, mouse_x, mouse_y);
    }
    else if(M.active_ctrl->type == Ctrl_VScrollBar)
    {
        ScrollBard* sb = (ScrollBard*)M.active_ctrl;
        int step = (int)(sb->pixlen/sb->full_len);
		int pixdelta = mouse_y - sb->ys;
        sb->SetPixDelta(pixdelta, mouse_x, mouse_y);
    }
}

void MoveBorders(int mx, int my)
{
    if(M.mmode & MOUSE_GRID_X1)
    {
        if(mx < GenBrowserWidth + InstrPanelWidth + GenBrowserGap - 5 + InstrCenterOffset)
        {
            if(abs(mx - (InstrPanelWidth + GenBrowserGap - 5)) < 25)
            {
                MainX1 = InstrPanelWidth + InstrCenterOffset;
            }
            else
            {
                MainX1 = mx;
                if(MainX1 + 25 >= MainX2)
                {
                    MainX2 = MainX1 + 25;
                }
            }
        }
        else
		{
            MainX1 = GenBrowserWidth + InstrPanelWidth + GenBrowserGap - 5 + InstrCenterOffset;
            if(MainX1 + 25 >= MainX2)
            {
                MainX2 = MainX1 + 25;
            }
		}
    }
    else if(M.mmode & MOUSE_ON_GENBROWSER_EDGE && M.btype == Brw_Generators)
    {
        if(abs(mx) < 25)
            MainX1 = InstrPanelWidth + InstrCenterOffset;
        else if(mx < GenBrowserWidth)
            MainX1 = mx + InstrPanelWidth - 5;
        else
            MainX1 = GenBrowserWidth + InstrPanelWidth + GenBrowserGap - 5 + InstrCenterOffset;
    }
    else if(M.mmode & MOUSE_ON_MIXBROWSER_EDGE && M.btype == Brw_Effects)
    {
        if(mx > WindWidth - 65)
            MixCenterWidth = 0;
        else if(mx > WindWidth - DefaultMixCenterWidth)
            MixCenterWidth = WindWidth - mx;
        else
            MixCenterWidth = DefaultMixCenterWidth;

        MainX2 = WindWidth - MixCenterWidth - 1;
    }
    else if(M.mmode & MOUSE_GRID_X2)
    {
        if(mx > GridX1)
            MainX2 = mx;
        else
        {
            if(mx > 28)
            {
                MainX2 = GridX1 = mx;
                MainX1 = GridX1 - 25;
            }
            else
            {
                MainX2 = GridX1 = 28;
                MainX1 = GridX1 - 25;
            }
        }

        MixerWidth = WindWidth - MainX2 - 1;
    }
    else if(M.mmode & MOUSE_GRID_X22)
    {
        if(mx > GridX1 + (MixCenterWidth+4))
            MainX2 = mx - (MixCenterWidth+4);
        else
        {
            if(mx > 28 + MixCenterWidth)
            {
                MainX2 = GridX1 = mx - (MixCenterWidth+4);
                MainX1 = GridX1 - 25;
            }
            else
            {
                MainX2 = GridX1 = 28;
                MainX1 = GridX1 - 25;
            }
        }

        MixerWidth = WindWidth - MainX2 - 1;
    }
    else if(M.mmode & MOUSE_GRID_Y1)
    {
        if(my + 4 < (CtrlPanelHeight + 1 + NavHeight + 2))
            MainY1 = my + 4;
        else
            MainY1 = (CtrlPanelHeight + 1 + NavHeight + 2);

        if(aux_panel->locked == true)
        {
            AuxHeight = WindHeight - MainY1 - LinerHeight - 1;
            MainY2 = WindHeight - AuxHeight;
            GridY2 = MainY2 - 1;
        }
        else if(WindHeight - MainY1 - LinerHeight - 1 < AuxHeight)
        {
            AuxHeight = WindHeight - MainY1 - LinerHeight - 1;
            MainY2 = WindHeight - AuxHeight;
        }
    }
    else if(M.mmode & MOUSE_GRID_Y2)
    {
        if(my > MainY1 + LinerHeight + 1)
        {
            MainY2 = my;
            if(aux_panel->isVolsPansMode())
            {
                if(MainY2 > WindHeight - 7)
                    MainY2 = WindHeight - 7;
            }
            else
            {
                if(MainY2 > WindHeight - 38)
                    MainY2 = WindHeight - 38;
            }

            AuxHeight = WindHeight - MainY2;

            if(aux_panel->auxmode == AuxMode_Mixer && MainY2 > WindHeight - 159)
            {
                MainY2 = WindHeight - 159;
            }

            if(aux_panel->auxmode == AuxMode_Mixer && 
                mixBrw->sbar->full_len < mixBrw->sbar->offset + mixBrw->sbar->visible_len)
            {
                mixBrw->main_offs += (mixBrw->sbar->offset + mixBrw->sbar->visible_len - mixBrw->sbar->full_len);
				if(mixBrw->main_offs > 0)
					mixBrw->main_offs = 0;
            }
        }
        else
        {
            MainY2 = MainY1 + LinerHeight + 1;
            AuxHeight = WindHeight - MainY2;
        }

        if(aux_panel->locked == true)
        {
            aux_panel->PattExpand->Release();
            aux_panel->locked = false;
        }

        if(aux_panel->auxmode == AuxMode_Pattern)
        {
            aux_panel->patt_height = AuxHeight;
        }
        else if(aux_panel->auxmode == AuxMode_Vols || aux_panel->auxmode == AuxMode_Pans)
        {
            aux_panel->volpan_height = AuxHeight;
        }
        else if(aux_panel->auxmode == AuxMode_Mixer)
        {
            aux_panel->mix_height = AuxHeight;
        }
    }
    else if(M.mmode & MOUSE_LOWAUXEDGE)
    {
        int mcy = my;
        if(mcy < GridYS1)
        {
            mcy = GridYS1;
        }

        aux_panel->eux = aux_panel->patty2 - mcy;
        if(aux_panel->eux < 5)
        {
            aux_panel->eux = 5;
        }
    }
	/*
    else if(M.mmode & MOUSE_ON_STATIC_EDGE)
    {
        st->pixwidth = mx - (MainX1 + 23);
        if(st->pixwidth < 3)
        {
            st->pixwidth = 3;
        }

        if(st->pixwidth >= GridX2 - MainX1 - 23)
        {
            st->pixwidth = GridX2 - MainX1 - 23;
        }
    }
	*/

    Grid2Main();
    //MC->pos->UpdateBounds();
    //MC->redraw_all = true;
    //MC->repaint();
    // update scrollbars
    UpdateElementsVisibility();
}

Pattern* CheckPosForPatterns(float tick, int trkline, Pattern* base)
{
    Pattern* pt;
    Element* el = firstElem;
    while(el != NULL)
    {
		if(el->type == El_Pattern && el->base == base)
		{
			pt = (Pattern*)el;
            if(pt->folded == false)
            {
    			if(tick >= pt->start_tick && tick < pt->end_tick && trkline >= pt->track_line && trkline <= pt->track_line_end)
    			{   
    				return pt;
    			}
            }
		}
		el = el->next;
    }
	return base;
}

void Grid2Main()
{
    GridX1 = MainX1 + 25;

    if(CP == NULL /*|| mixbrowse*/)
    {
        GridX2 = MainX2 - 17;
    }
    else
    {
        GridX2 = MainX2 + MixCenterWidth - 17;
    }

    GridY1 = MainY1 + LinerHeight + 1;
    GridY2 = MainY2;
    AuxX1 = MainX1;
    AuxX2 = MainX2 + 1;
    AuxY1 = MainY2 + 1;
    AuxY2 = WindHeight - 2;
    AuxRX1 = AuxX1 + 24;
    AuxRX2 = AuxX2 - 2;
    AuxRY1 = AuxY1 + 2;
    AuxRY2 = AuxY2 - 2;
    AuxKeysHeight = AuxRY2 - AuxRY1;

    PattX1 = AuxX1 + 3;
    PattX2 = AuxX2;
    
    PattY1 = AuxY1;
    PattY2 = AuxY2;

    if(aux_panel != NULL)
    {
        if(aux_panel->auxmode == AuxMode_Pattern)
        {
            AuxX2 = MainX2 + 21;

            aux_panel->pattx1 = AuxX1 + 3;
            aux_panel->pattx2 = AuxX2;
            
            aux_panel->patty1 = AuxY1;
            aux_panel->patty2 = AuxY2;

            GridXS1 = aux_panel->pattx1 + aux_panel->keywidth;;
            GridYS1 = aux_panel->patty1 + 33;
            
            GridXS2 = aux_panel->pattx2 - 20;
            GridYS2 = (GridYS1 >= AuxY2 - aux_panel->eux) ? GridYS1 : AuxY2 - aux_panel->eux;
        }

        mixX = AuxX1;
        //mixX = AuxX1 - InstrPanelWidth;
        mixY = AuxY1 + 2;
        
        //mixW = MainX2 + MixCenterWidth + 1 - mixX;
        mixW = MainX2 + 1 - mixX;
        mixH = WindHeight - mixY;
    }

    if(lineHeight > 0)
        numFieldLines = (GridY2 - GridY1)/lineHeight;

    if(aux_panel != NULL && aux_panel->lineHeight > 0)
        numAuxLines = (GridYS2 - GridYS1)/aux_panel->lineHeight;

    InstrPanelHeight = WindHeight - MainY1;
    GenBrowserHeight = WindHeight - MainY1;

    if(field_pattern != NULL)
    {
        C.PosUpdate();
    }

    if(MainWnd != NULL && !ProjectLoadingInProgress)
    {
        MainWnd->setResizeLimits(GridX1 + MixCenterWidth + 7, GridY1 + AuxHeight + LinerHeight + 8, 32768, 32768);
    }
}

void MainPos2AuxPos()
{
    if(aux_panel->workPt->autopatt == false)
    {
        pbkMain->SetCurrFrame(pbkAux->currFrame);
        currPlayX_f = (double)pbkMain->currFrame/framesPerPixel;
        currPlayX = RoundDouble(currPlayX_f);
    }
}

void AuxPos2MainPos()
{
    if(aux_panel->workPt->autopatt == false)
    {
        pbkAux->SetCurrFrame(pbkMain->currFrame);
        if(aux_panel->workPt != aux_panel->blankPt)
        {
            aux_panel->curr_play_x_f = (double)(pbkAux->currFrame - aux_panel->workPt->frame)/aux_panel->frames_per_pixel;
            aux_panel->curr_play_x = RoundDouble(aux_panel->curr_play_x_f);
        }
    }
}

void SetMouseImage(unsigned flags)
{
    if(M.mmode & MOUSE_RESIZE && !(M.mmode & MOUSE_SLIDING))
    {
        if(M.edge == LEFT_X || M.edge == RIGHT_X)
        {
            MC->listen->setMouseCursor(MouseCursor(MouseCursor::LeftRightResizeCursor));
            //SetCursor(HSCursor);
        }
        else if(M.edge == TOP_Y || M.edge == BOTTOM_Y)
        {
            //SetCursor(VSCursor);
            MC->listen->setMouseCursor(MouseCursor(MouseCursor::UpDownResizeCursor));
        }
        else
        {
            MC->listen->setMouseCursor(MouseCursor(MouseCursor::NormalCursor));
            //SetCursor(ArrowCursor);
        }
    }
    else if(M.mmode & MOUSE_ON_GENBROWSER_EDGE || 
            M.mmode & MOUSE_ON_MIXBROWSER_EDGE || 
            M.mmode & MOUSE_GRID_X1 || 
            M.mmode & MOUSE_GRID_X2 ||
            M.mmode & MOUSE_GRID_X22)
    {
        //SetCursor(HSCursor);
        MC->listen->setMouseCursor(MouseCursor(MouseCursor::LeftRightResizeCursor));
    }
    else if(M.mmode & MOUSE_GRID_Y1 || M.mmode & MOUSE_GRID_Y2 || M.mmode & MOUSE_UPPER_TRACK_EDGE || 
            M.mmode & MOUSE_LOWER_TRACK_EDGE || M.mmode & MOUSE_LOWAUXEDGE)
    {
        //SetCursor(VSCursor);
        MC->listen->setMouseCursor(MouseCursor(MouseCursor::UpDownResizeCursor));
    }
    else if(M.mmode & MOUSE_ON_GRID && !(M.mmode & MOUSE_ENVELOPING) && (flags & kbd_alt || M.patternbrush) && !(flags & kbd_ctrl))
    {
        MC->listen->setMouseCursor(*cursBrush);
    }
    else if(M.mmode & MOUSE_ON_GRID && !(M.mmode & MOUSE_ENVELOPING) && flags & kbd_shift && M.active_elem == NULL)
    {
        MC->listen->setMouseCursor(*cursSlide);
    }
    else if(M.mmode & MOUSE_ON_GRID && flags & kbd_ctrl && flags & kbd_shift && M.active_elem != NULL)
    {
        if(M.active_elem->type == El_Pattern)
            MC->listen->setMouseCursor(*cursCopy);
        else
            MC->listen->setMouseCursor(*cursClone);
    }
    else if(M.mmode & MOUSE_ON_GRID && M.loc == Loc_SmallGrid && 
            (flags & kbd_shift || flags & kbd_ctrl) && M.active_elem != NULL)
    {
        MC->listen->setMouseCursor(*cursClone);
    }
    else if(M.mmode & MOUSE_ON_GRID && M.loc == Loc_MainGrid && 
            (flags & kbd_shift || flags & kbd_ctrl) && M.active_elem != NULL)
    {
        MC->listen->setMouseCursor(*cursClone);
    }
/*    else if(M.mmode & MOUSE_ON_GRID && flags & kbd_ctrl)
    {
        //MC->listen->setMouseCursor(*cursSelect);
    }*/
    else
    {
        MC->listen->setMouseCursor(MouseCursor(MouseCursor::NormalCursor));
        //SetCursor(ArrowCursor);
    }

    if(M.LMB && ((M.mmode & MOUSE_BPMING && M.active_ctrl == NULL) || 
                    M.active_paramedit != NULL && M.active_paramedit->type == Param_FX))
    {
        MC->listen->setMouseCursor(MouseCursor(MouseCursor::NoCursor));
        //SetCursor(NULL);
    }
}


////// / // /   /
/////////
///////////
///////
/////
///////
/////
////
///////
///////////
//////////////
///////////////

unsigned TranslateKey(unsigned keycode)
{
    return (keycode > 255) ? 0 : keymap[keycode];
}

unsigned TranslateWKey(unsigned keycode)
{
    return (keycode > 255) ? 0 : keymap[keycode];
}

void UpdateElementsVisibility()
{
    Element* el = firstElem;
    while(el != NULL)
    {
        if(el->IsPresent())
        {
            el->CheckVisibility();

            if(el->visible || el->auxvisible)
            {
                el->Show();
            }
            else
            {
                el->Hide();
            }
        }

        el = el->next;
    }
}

void DeleteMenu(Menu* menu)
{
    MenuItem* mitem = menu->first_item;
    while(mitem->inext != NULL)
    {
        mitem = mitem->inext;
        RemoveControlCommon(mitem->iprev);
    }
    RemoveControlCommon(mitem);

    if(menu->drawarea != NULL)
    {
        MC->listen->repaint(menu->drawarea->ax,
                            menu->drawarea->ay,
                            menu->drawarea->aw,
                            menu->drawarea->ah);
        MC->RemoveDrawArea(menu->drawarea);
    }
    delete menu;
}

bool IsPasteAllowed(Element* el)
{
    if(el->to_be_deleted == true)
    {
        return false;
    }
    // Not allowed pasting patterns to patterns and only samples, gennotes and slidenotes are
    // allowed to paste to pianoroll
    if(!(el->type == El_Pattern && C.patt != field_pattern) &&
        !(el->type != El_Samplent && el->type != El_GenNote && el->type != El_SlideNote &&
         C.patt->ptype == Patt_Pianoroll))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void PasteElements(bool mousepos)
{
    if((mousepos && M.loc == Loc_UNKNOWN) || (mousepos && C.loc == Loc_UNKNOWN))
    {
        // Can paste only to grid
        return;
    }

    C.SaveState();
    if(mousepos)
    {
        C.SetPattern(M.patt, M.loc);
    }
    Element* nel;
    Element* el = firstElem;
    Element* lel = lastElem;
    int dLine;
    while(el != NULL)
    {
        el->Deselect();
        if(el->is_copied)
        {
            if(IsPasteAllowed(el))
            {
                if(M.patt == aux_panel->workPt &&  aux_panel->workPt == aux_panel->blankPt)
                {
                    aux_panel->CreateNew();
                }
                nel = el->Clone(true);
                if(mousepos)
                {
                    nel->start_tick = M.snappedTick;
                    nel->end_tick = nel->start_tick + nel->tick_length;
                    nel->track_line = M.snappedLine;
                }
                else
                {
                    nel->start_tick = CTick;
                    nel->track_line = CLine;
                }

                jassert(C.patt->basePattern != NULL);
                if(nel->track_line > C.patt->basePattern->num_lines - 1)
                {
                    nel->track_line = C.patt->basePattern->num_lines - 1;
                }

                if(el->patt->ptype == Patt_Pianoroll && M.patt->ptype != Patt_Pianoroll)
                {
                    nel->Move(el->start_tick - BuffTick, 0);
                }
                else if(M.patt->ptype == Patt_Pianoroll)
                {
                    if(nel->type == El_SlideNote)
                    {
                        SlideNote* sn = (SlideNote*)nel;
                        SlideNote* sn0 = (SlideNote*)el;
                        sn->track_line = NUM_PIANOROLL_LINES - sn0->ed_note->value - 1;
                        sn->Move(sn0->start_tick - BuffTick, 0);
                    }
                    else if(nel->IsInstance())
                    {
                        NoteInstance* ii = (NoteInstance*)nel;
                        NoteInstance* ii0 = (NoteInstance*)el;
                        ii->track_line = NUM_PIANOROLL_LINES - ii0->ed_note->value - 1;
                        ii->Move(el->start_tick - BuffTick, 0);
                        if(M.patt->ibound != NULL)
                        {
                            nel = ReassignInstrument(ii, M.patt->ibound);
                        }
                    }
                }
                else
                {
                    dLine = el->track_line - BuffLine;
                    if(M.patt->ptype == Patt_StepSeq)
                    {
                        if(nel->track_line + dLine > num_instrs - 1)
                        {
                            dLine = 0;
                            nel->track_line = num_instrs - 1;
                        }
                    }
                    else
                    {
                        if(nel->track_line + dLine > C.patt->basePattern->num_lines - 1)
                        {
                            dLine = 0;
                            nel->track_line = C.patt->basePattern->num_lines - 1;
                        }
                    }
                    nel->Move(el->start_tick - BuffTick, dLine);

                    if(M.patt->ptype == Patt_StepSeq && nel->IsInstance())
                    {
                        NoteInstance* ii = (NoteInstance*)nel;
                        if(ii->trkdata->defined_instr != NULL && ii->instr != ii->trkdata->defined_instr)
                        {
                            nel = ReassignInstrument(ii, ii->trkdata->defined_instr);
                        }
                    }
                }
                nel->Select();
                Num_Selected++;
            }
        }

        if(el == lel)
        {
            break;
        }
        else
        {
            el = el->next;
        }
    }

    if(M.patt == aux_panel->workPt && aux_panel->workPt != aux_panel->blankPt)
    {
        aux_panel->RescanPatternBounds();
        pbkAux->AlignRangeToPattern();
    }

    C.RestoreState();

    if(M.loc == Loc_SmallGrid && !aux_panel->isBlank())
    {
        aux_panel->workPt->basePattern->UpdateScaledImage();
        R(Refresh_GridContent);
    }

    if(M.loc == Loc_MainGrid)
    {
        R(Refresh_GridContent);
        R(Refresh_Aux);
    }
    else if(M.loc == Loc_SmallGrid)
    {
        R(Refresh_AuxGrid);
        R(Refresh_GridContent);
    }
}

void CleanUpClipboard()
{
    Element* elnext;
    Element* el = firstElem;
    while(el != NULL)
    {
        elnext = el->next;
        if(el->is_copied == true)
        {
            el->is_copied = false;
        }
        el = elnext;
    }
    Num_Buffered = 0;
}

void AdjustClipboardOffsets()
{
    BuffTick = -1;
    BuffLine = -1;
    Element* elnext;
    Element* el = firstElem;
    while(el != NULL)
    {
        elnext = el->next;
        if(el->IsPresent() && el->is_copied == true)
        {
            if(BuffTick == -1 || el->start_tick < BuffTick)
            {
                BuffTick = el->start_tick;
            }

            if(BuffLine == -1 || el->track_line < BuffLine)
            {
                BuffLine = el->track_line;
            }
        }
        el = elnext;
    }
}

void CopySelectedElements()
{
    if(Num_Selected > 0)
    {
        CleanUpClipboard();
        BuffTick = -1;
        BuffLine = -1;
        Element* elnext;
        Element* el = firstElem;
        while(el != NULL)
        {
            elnext = el->next;
            if(el->IsPresent() && el->selected == true)
            {
                el->is_copied = true;

                if(BuffTick == -1 || el->start_tick < BuffTick)
                {
                    BuffTick = el->start_tick;
                }

                if(el->type == El_Pattern && ((Pattern*)el)->is_fat == true)
                {
                    if(BuffLine == -1 || el->track_line - 1 < BuffLine)
                    {
                        BuffLine = el->track_line - 1;
                    }
                }
                else if(BuffLine == -1 || el->track_line < BuffLine)
                {
                    BuffLine = el->track_line;
                }
            }
            el = elnext;
        }
    }
}

void CutSelectedElements()
{
    CleanUpClipboard();
    if(Num_Selected > 0)
    {
        BuffTick = -1;
        BuffLine = -1;
        Element* elnext;
        Element* el = firstElem;
        while(el != NULL)
        {
            elnext = el->next;
            if(el->IsPresent() && el->selected == true)
            {
                CutElement(el);
                el->Deselect();

                if(BuffTick == -1 || el->start_tick < BuffTick)
                {
                    BuffTick = el->start_tick;
                }

                if(el->type == El_Pattern && ((Pattern*)el)->is_fat == true)
                {
                    if(BuffLine == -1 || el->track_line - 1 < BuffLine)
                    {
                        BuffLine = el->track_line - 1;
                    }
                }
                else if(BuffLine == -1 || el->track_line < BuffLine)
                {
                    BuffLine = el->track_line;
                }
            }
            el = elnext;
        }

        Selection_ToggleElements();
        UpdateNavBarsData();

        if(M.patt == aux_panel->workPt && aux_panel->workPt != aux_panel->blankPt)
        {
            aux_panel->RescanPatternBounds();
        }

        if(M.loc == Loc_SmallGrid && !aux_panel->isBlank())
        {
            aux_panel->workPt->basePattern->UpdateScaledImage();
            R(Refresh_GridContent);
        }

        if(M.selloc == Loc_MainGrid)
        {
            R(Refresh_GridContent);
            R(Refresh_Aux);
        }
        else if(M.selloc == Loc_SmallGrid)
        {
            R(Refresh_AuxContent);
            R(Refresh_SubAux);
        }
    }
}

void SelectAll()
{
    Num_Selected = 0;
    Element* el = firstElem;
    while(el != NULL)
    {
        if(el->IsPresent() && el->patt->IsElementTypeDisplayable(el->type))
        {
            el->Deselect();
            if((M.loc == Loc_MainGrid && el->patt == field_pattern) || 
               (M.loc == Loc_SmallGrid && el->patt == aux_panel->workPt->basePattern))
            {
                el->Select();
                Num_Selected++;
            }
        }
        el = el->next;
    }
    
    R(Refresh_GridContent);
    if(aux_panel->isVolsPansMode())
    {
        R(Refresh_Aux);
    }

    if(aux_panel->auxmode == AuxMode_Pattern)
    {
        R(Refresh_AuxContent);
        R(Refresh_SubAux);
    }
}

void SetMenuItemEffect(MItemType itype, MixChannel* mchan)
{
    if(mchan != NULL)
    {
        switch(itype)
        {
            case MItem_SetMixCellToSend:
                aux_panel->AddEffectByType(ModSubtype_Send, mchan);
                break;
            case MItem_SetMixCellToGain:
                aux_panel->AddEffectByType(ModSubtype_Gain, mchan);
                break;
            case MItem_SetMixCellToEQ1:
                aux_panel->AddEffectByType(ModSubtype_Equalizer1, mchan);
                break;
            case MItem_SetMixCellToEQ3:
                aux_panel->AddEffectByType(ModSubtype_Equalizer3, mchan);
                break;
            case MItem_SetMixCellToGraphicEQ:
                aux_panel->AddEffectByType(ModSubtype_GraphicEQ, mchan);
                break;
            case MItem_SetMixCellToBlank:
                aux_panel->AddEffectByType(ModSubtype_Default, mchan);
                break;
            case MItem_SetMixCellToDelay:
                aux_panel->AddEffectByType(ModSubtype_XDelay, mchan);
                break;
            case MItem_SetMixCellToFilter:
                aux_panel->AddEffectByType(ModSubtype_CFilter3, mchan);
                break;
            case MItem_SetMixCellToTremolo:
                aux_panel->AddEffectByType(ModSubtype_Tremolo, mchan);
                break;
            case MItem_SetMixCellToCompressor:
                aux_panel->AddEffectByType(ModSubtype_Compressor, mchan);
                break;
            case MItem_SetMixCellToReverb:
                aux_panel->AddEffectByType(ModSubtype_Reverb, mchan);
                break;
            case MItem_SetMixCellToChorus:
                aux_panel->AddEffectByType(ModSubtype_Chorus, mchan);
                break;
            case MItem_SetMixCellToFlanger:
                aux_panel->AddEffectByType(ModSubtype_Flanger, mchan);
                break;
            case MItem_SetMixCellToPhaser:
                aux_panel->AddEffectByType(ModSubtype_Phaser, mchan);
                break;
            case MItem_SetMixCellToWahWah:
                aux_panel->AddEffectByType(ModSubtype_WahWah, mchan);
                break;
            case MItem_SetMixCellToDistortion:
                aux_panel->AddEffectByType(ModSubtype_Distortion, mchan);
                break;
            case MItem_SetMixCellToBitCrusher:
                aux_panel->AddEffectByType(ModSubtype_BitCrusher, mchan);
                break;
            case MItem_SetMixCellToStereoizer:
                aux_panel->AddEffectByType(ModSubtype_Stereo, mchan);
                break;
            case MItem_SetMixCellToFilter2:
                aux_panel->AddEffectByType(ModSubtype_CFilter2, mchan);
                break;
            case MItem_SetMixCellToFilter3:
                aux_panel->AddEffectByType(ModSubtype_CFilter3, mchan);
                break;
        }

        R(Refresh_Aux);
    }
}

void CreateElement_EnvelopeCommon(Parameter* param)
{
    Command* cmdenv = CreateCommandFromParam(param);
    AddNewElement(cmdenv, false);
}

void DoSavePreset(Instrument* instr)
{
    AlertWindow w (T("Save preset"),
                   T("Enter new preset name:"),
                   AlertWindow::QuestionIcon);

    w.setSize(132, 55);
    char name[MAX_NAME_STRING];
    if(strlen(instr->current_preset_name) == 0)
    {
        strcpy(name, "preset1");
    }
    else
    {
        strcpy(name, instr->current_preset_name);
    }

    w.addTextEditor (T("PresetName"), name, T(""));

    w.addButton (T("OK"), 1, KeyPress (KeyPress::returnKey, 0, 0));
    w.addButton (T("Cancel"), 0, KeyPress (KeyPress::escapeKey, 0, 0));

    if(w.runModalLoop() != 0) // is they picked 'OK'
    {
        String nmstr = w.getTextEditorContents(T("PresetName"));
        char name[25];
        nmstr.copyToBuffer(name, 25);
        instr->SavePresetAs(name);
        if(genBrw->brwmode == Browse_Presets && instr->subtype != ModSubtype_VSTPlugin)
        {
            genBrw->Update();
        }
    }
}

void DoSavePreset(Eff* eff)
{
    AlertWindow w (T("Save preset"),
                   T("Enter new preset name:"),
                   AlertWindow::QuestionIcon);

    w.setSize(132, 55);
    w.addTextEditor (T("PresetName"), T("preset1"), T(""));

    w.addButton (T("OK"), 1, KeyPress (KeyPress::returnKey, 0, 0));
    w.addButton (T("Cancel"), 0, KeyPress (KeyPress::escapeKey, 0, 0));

    if(w.runModalLoop() != 0) // is they picked 'OK'
    {
        String nmstr = w.getTextEditorContents(T("PresetName"));
        char name[25];
        nmstr.copyToBuffer(name, 25);
        eff->SavePresetAs(name);
        if(mixBrw->brwmode == Browse_Presets && 
           eff->subtype != ModSubtype_VSTPlugin)
        {
            //eff->DeletePresets();
            //eff->ScanForPresets(true);
            mixBrw->Update();
        }
    }
}

void DoLoadPreset(Instrument* instr)
{
    if(current_instr != instr)
    {
        ChangeCurrentInstrument(instr);
    }
    genBrw->HandleButtDown(genBrw->ShowPresets);
    CP->HandleButtDown(CP->AccGenBrowsa);
}

void DoLoadPreset(Eff* eff)
{
    if(CP->view_mixer->pressed == false)
    {
        aux_panel->last_mixcenterwidth = DefaultMixCenterWidth;
        CP->HandleButtDown(CP->view_mixer);
    }
    else
    {
        MixCenterWidth = DefaultMixCenterWidth;
    	MainX2 = WindWidth - MixCenterWidth - 1;
        Grid2Main();
        R(Refresh_Aux);
    }

    if(aux_panel->current_eff != eff)
    {
        aux_panel->SetCurrentEffect(eff);
    }

    if(mixBrw->CurrButt != mixBrw->ShowPresets)
    {
        mixBrw->HandleButtDown(mixBrw->ShowPresets);
    }
}

void RescanPlugins(bool full, bool brwupdate)
{
    //Stop_PortAudio();
    delete pModulesList;
    pModulesList = new CPluginList(NULL);
    Init_InternalPlugins();
    if(full)
    {
        ClearVSTListFile();
    }

    ScanThread PlugScan;
    PlugScan.runThread();

    if(brwupdate)
    {
        genBrw->Clean();
        genBrw->Update();
        mixBrw->Clean();
        mixBrw->Update();
    }
    //Start_PortAudio();
}

void DeleteEffect(Eff* eff)
{
    WaitForSingleObject(hAudioProcessMutex, INFINITE);
    if(aux_panel->current_eff == eff && 
        (mixBrw->brwmode == Browse_Params || mixBrw->brwmode == Browse_Presets))
    {
        mixBrw->Clean();
        R(Refresh_MixCenter);
    }

    if(eff == aux_panel->current_eff)
    {
        aux_panel->current_eff = NULL;
    }

    RemoveEff(eff);

    ReleaseMutex(hAudioProcessMutex);

    R(Refresh_GridContent);
}

void ActivateMenuItem(MenuItem* mitem, MItemType itype, Menu* menu)
{
    Pattern* pt;
    switch(itype)
    {
        case MItem_ShowVolumeLane:
            ShowVolLane(menu->trk, field_pattern);
            break;
        case MItem_ShowPanLane:
            ShowPanLane(menu->trk, field_pattern);
            break;
        case MItem_HideVolumeLane:
            HideVolLane(menu->trk, field_pattern);
            break;
        case MItem_HidePanLane:
            HidePanLane(menu->trk, field_pattern);
            break;
        case MItem_DeleteDefinition:
            menu->trk->defined_instr = NULL;
            break;
        case MItem_CreateUsualPattern:
            pt = CreateElement_Pattern(menu->tick, menu->tick + 10, menu->line + 1, menu->line + 10);
            pt->Switch2Param(pt->name);
            break;
        case MItem_CreatePianorollPattern:
            pt = CreateElement_PianorollPattern(menu->tick, menu->tick + 10, menu->line + 1, menu->line + 10);
            pt->Switch2Param(pt->name);
            break;
        case MItem_DeleteInstrument:
            IP->RemoveInstrument(menu->instr);
            if(aux_panel->isPatternMode())
            {
                if(aux_panel->workPt->ptype == Patt_StepSeq)
                {
                    R(Refresh_Aux);
                }
                else
                {
                    R(Refresh_AuxGrid);
                }
            }
            R(Refresh_GridContent);

            R(Refresh_InstrPanel);
            break;
        case MItem_CloneInstrument:
            IP->CloneInstrument(menu->instr);
            R(Refresh_InstrPanel);
            break;
        case MItem_CreatePianorollPatternForInstrument:
            pt = CreateElement_Pattern(CTick, CTick + ticks_per_beat*beats_per_bar, CLine, CLine, Patt_Pianoroll);
            if(CLine == 0)
                pt->is_fat = false;
            BindPatternToInstrument(pt, menu->instr);

            aux_panel->EditPattern(pt);
            R(Refresh_GridContent);
            break;
        case MItem_BindInstrumentToCurrentPattern:
            BindPatternToInstrument(aux_panel->workPt, menu->instr);
            R(Refresh_GridContent);
            break;
        case MItem_CleanMixCell:
        case MItem_SetMixCellToSend:
        case MItem_SetMixCellToGain:
        case MItem_SetMixCellToEQ1:
        case MItem_SetMixCellToEQ3:
        case MItem_SetMixCellToGraphicEQ:
        case MItem_SetMixCellToBlank:
        case MItem_SetMixCellToDelay:
        case MItem_SetMixCellToFilter:
        case MItem_SetMixCellToTremolo:
        case MItem_SetMixCellToCompressor:
        case MItem_SetMixCellToReverb:
        case MItem_SetMixCellToChorus:
        case MItem_SetMixCellToFlanger:
        case MItem_SetMixCellToPhaser:
        case MItem_SetMixCellToWahWah:
        case MItem_SetMixCellToDistortion:
        case MItem_SetMixCellToBitCrusher:
        case MItem_SetMixCellToFilter2:
        case MItem_SetMixCellToFilter3:
        case MItem_SetMixCellToStereoizer:
            SetMenuItemEffect(itype, menu->mchan);
            break;
        case MItem_DeleteEffect:
            if(menu->curreff != NULL && menu->mchan != NULL)
            {
                DeleteEffect(menu->curreff);
                menu->mchan->drawarea->Change();
            }
            break;
        case MItem_SavePreset:
            if(menu->curreff != NULL)/*(M.active_mixcell != NULL && M.active_mixcell->effect != NULL)*/
            {
                DoSavePreset(menu->curreff);
            }
            break;
        case MItem_LoadPreset:
        {
			if(menu->curreff != NULL)
            {
                DoLoadPreset(menu->curreff);
            }
            else if(menu->instr != NULL)
            {
                DoLoadPreset(menu->instr);
            }
            R(Refresh_MixHighlights);
        }break;
        case MItem_ViewParams:
        {
            if(menu->curreff != NULL && menu->mchan != NULL)
            {
                aux_panel->current_eff = menu->curreff;
				if(mixBrw->CurrButt != mixBrw->ShowParams)
				{
					mixBrw->HandleButtDown(mixBrw->ShowParams);
				}
				else
				{
					mixBrw->Update();
				}
            }
            else if(M.mmode & MOUSE_INSTRUMENTING)
            {
                ChangeCurrentInstrument(menu->instr);
                genBrw->HandleButtDown(genBrw->ShowParams);
                CP->HandleButtDown(CP->AccGenBrowsa);
            }
        }break;
        case MItem_EditAutoPattern:
        {
            if(menu->instr != NULL)
            {
                // Place cursor at the previously edited pattern
                if(C.loc == Loc_SmallGrid && aux_panel->workPt != aux_panel->blankPt)
                {
                    aux_panel->workPt->Activate();
                    C.loc = Loc_MainGrid;
                    C.patt = field_pattern;
                    CLine = aux_panel->workPt->track_line;
                    CTick = aux_panel->workPt->start_tick;
                }

                if(menu->instr->autoPatt == NULL)
                {
                    menu->instr->CreateAutoPattern();
                }
                aux_panel->EditPattern(menu->instr->autoPatt);
                UpdateElementsVisibility();
                R(Refresh_Aux);
            }
        }break;
        case MItem_ResetControl:
        {
            if(menu->activectrl->type == Ctrl_Slider || menu->activectrl->type == Ctrl_Knob)
            {
                Parameter* param = menu->activectrl->param;
                if(param->type == Param_Vol)
                {
                    param->SetNormalValue(1);
                    param->SetInitialValue(1);
                }
                else if(param->type == Param_Pan)
                {
                    param->SetNormalValue(0);
                    param->SetInitialValue(0);
                }
                else 
                {
                    param->SetNormalValue(param->defaultval);
                    param->SetInitialValue(param->defaultval);
                }
            }
        }break;
        case MItem_CreateEnvelope:
        {
            if(menu->activectrl->scope != NULL && menu->activectrl->param->scope != NULL)
            {
                if(C.loc == Loc_SmallGrid && aux_panel->isBlank())
                {
                    aux_panel->CreateNew();
                }

                CreateElement_EnvelopeCommon(menu->activectrl->param);

                if(C.loc == Loc_SmallGrid)
                {
                    aux_panel->RescanPatternBounds();
                }

                /*
                M.mmode &= ~MOUSE_CONTROLLING;
                M.mmode |= MOUSE_DRAG_N_DROPPING;

                M.active_command = CreateCommandFromControl(menu->activectrl);
                M.drag_data.drag_type = Drag_Command;
                M.drag_data.drag_stuff = (void*)M.active_command;
                M.holding = true;
                MC->listen->updateMouseCursor();
                */
            }
        }break;
        case MItem_CreateCommand:
        {
            if(menu->activectrl->scope != NULL)
            {
                if(C.loc == Loc_SmallGrid && aux_panel->isBlank())
                {
                    aux_panel->CreateNew();
                }

                Command* cmd = CreateCommandFromControl(menu->activectrl);
				AddNewElement(cmd, false);

                if(C.loc == Loc_SmallGrid)
                {
                    aux_panel->RescanPatternBounds();
                }

            }
        }break;
        case MItem_ReScanFull:
            RescanPlugins(true);
            //ScanWnd->InitFull();
            //ScanWnd->Show();
            break;
        case MItem_ReScanFast:
            RescanPlugins(false);
            //ScanWnd->InitFast();
            //ScanWnd->Show();
            break;
        case MItem_SelectAll:
        {
            SelectAll();
        }break;
        case MItem_Cut:
        {
            CutSelectedElements();
        }break;
        case MItem_Copy:
            CopySelectedElements();
            break;
        case MItem_Paste:
            PasteElements(true);
            break;
        case MItem_BindInstrument:
        {
            if(aux_panel->workPt != aux_panel->blankPt && aux_panel->workPt->ptype != Patt_Pianoroll)
            {
                aux_panel->AuxReset();
            }

            if(aux_panel->workPt->ptype != Patt_Pianoroll)
            {
                aux_panel->HandleButtDown(aux_panel->PtPianorol);
            }

            if(aux_panel->workPt == aux_panel->blankPt)
            {
                aux_panel->CreateNew();
            }

            BindPatternToInstrument(aux_panel->workPt, M.active_instr);

            R(Refresh_GridContent);
        }break;
        case MItem_Unbunch:
        {
            Remove_Bunch(M.track_bunch, M.patt);

            if(M.loc == Loc_MainGrid)
            {
                R(Refresh_Grid);
                if(mixbrowse == false)
                    R(Refresh_MixCenter);
            }
            else
                R(Refresh_Aux);
        }break;
        case MItem_Undo:
            undoMan->UndoAction();
            break;
        case MItem_Redo:
            undoMan->RedoAction();
            break;
        case MItem_Delete:
            break;
        case MItem_Save:
        {
            M.DeleteContextMenu();
            SaveProject(false);
            out_from_dialog = true;
        }break;
        case MItem_SaveAs:
        {
            M.DeleteContextMenu();
            SaveProject(true);
            out_from_dialog = true;
        }break;
        case MItem_Open:
        {
            M.DeleteContextMenu();
            LoadProject(0);
            out_from_dialog = true;
        }break;
        case MItem_New:
        {
            M.DeleteContextMenu();
            NewProject();
            out_from_dialog = true;
        }break;
        case MItem_Render:
        {
            M.DeleteContextMenu();
            ToggleRenderWindow();
            out_from_dialog = true;
        }break;
        case MItem_Preferences:
        {
            M.DeleteContextMenu();
            ToggleConfigWindow();
            out_from_dialog = true;
        }break;
        case MItem_LoadAsInstrument:
        {
            IP->AddInstrumentFromBrowser(menu->activefiledata, true);
            //menu->browser->Update();
        }break;
        case MItem_DeletePreset:
        {
            menu->module->KillPreset(menu->activepresetdata->pPreset);
            menu->browser->Update();
        }break;
        case MItem_DeleteProject:
        {
            File f(menu->activefiledata->path);
            f.deleteFile();
            menu->browser->Update();
        }break;
        case MItem_OpenProject:
        {
            File f(menu->activefiledata->path);
            LoadProject(&f);
        }break;
        case MItem_HotKeys:
        {
            if(CP->HKWnd->Showing() == false)
                CP->HKWnd->Show();
            else
                CP->HKWnd->Hide();
        }break;
        case MItem_About:
        {
            if(CP->AboutWnd == NULL)
                CP->AboutWnd = MainWnd->CreateAboutWindow();
            if(CP->AboutWnd->Showing() == false)
                CP->AboutWnd->Show();
            else
                CP->AboutWnd->Hide();
        }break;
        case MItem_Exit:
            ExitFromApp();
            break;
        case MItem_CleanSong:
            M.DeleteContextMenu();
            CleanElements();
            out_from_dialog = true;
            break;
        case MItem_LoadInstrumentPlugin:
            CP->HandleButtDown(CP->AccGenBrowsa);
            genBrw->HandleButtDown(genBrw->ShowExternalPlugs);
            R(Refresh_All);
            break;
        case MItem_LoadEffectPlugin:
            if(aux_panel->auxmode != AuxMode_Mixer)
            {
                CP->HandleButtDown(CP->view_mixer);
            }
            CP->HandleButtDown(mixBrw->ShowExternalPlugs);
            R(Refresh_All);
            break;
        case MItem_LoadSample:
            CP->HandleButtDown(CP->AccGenBrowsa);
            genBrw->HandleButtDown(genBrw->ShowSamples);
            R(Refresh_All);
            break;
        case MItem_LoadProject:
            CP->HandleButtDown(CP->AccGenBrowsa);
            genBrw->HandleButtDown(genBrw->ShowProjects);
            R(Refresh_All);
            break;
        case MItem_FullScreen:
            MainWnd->setFullScreen(!MainWnd->isFullScreen());
            break;
        case MItem_Unbind:
		{
            Pattern* pt = aux_panel->workPt;
            pt->basePattern->ibound = NULL;
            Pattern* ptc = pt->basePattern->der_first;
            while(ptc != NULL)
            {
                ptc->ibound = NULL;
                ptc = ptc->der_next;
            }

            R(Refresh_GridContent);
            R(Refresh_Aux);
		}break;
        case MItem_Custom:
        {
            if(mitem != NULL)
            {
                for(int si = 0; si < numLastSessions; si++)
                {
                    if(strcmp(lastsessions[si]->getFileName(), mitem->title) == 0)
                    {
                        M.DeleteContextMenu();
                        LoadProject(lastsessions[si]);
                        //out_from_dialog = true;
                    }
                }
            }
        }break;
    }
}

extern void UpdateTime(Loc loc)
{
    long frame;
    double tick;
    if(loc == Loc_SmallGrid)
    {
        frame = pbkAux->currFrame;
        tick = pbkAux->currTick;
    }
    else
    {
        frame = pbkMain->currFrame;
        tick = pbkMain->currTick;
    }

    float fsec = (float)frame/fSampleRate;
    curr_min = int(fsec/60);
    curr_msec = int(fsec*1000)%1000;
    curr_sec = int(fsec)%60;

    curr_bar = int(tick/(ticks_per_beat*beats_per_bar));
    curr_beat = int(tick/ticks_per_beat)%ticks_per_beat;
    curr_step = float(tick - int(tick)/ticks_per_beat*ticks_per_beat);

    /*
    float tsec = (float)lastFrame/fSampleRate;
    total_min = int(tsec/60);
    total_sec = int(tsec)%60;
    */

    CP->tmarea->Change();
}

void PixelToFrame()
{
    currPlayX_f = pbkMain->currFrame/framesPerPixel;
    currPlayX = (int)currPlayX_f;
}

void UpdatePerScale()
{
    Selection_UpdateCoords();
    Looping_UpdateCoords();
    C.PosUpdate();

    framesPerPixel = seconds_in_tick*fSampleRate/tickWidth;
    currPlayX_f = pbkMain->currFrame/framesPerPixel;
	currPlayX = (int)currPlayX_f;

    UpdateScaledImages();
    J_RefreshGridImage();
}

void UpdateFrames()
{
    UpdateEventsFrames(field_pattern);
    Instrument* i = first_instr;
    while(i != NULL)
    {
        if(i->autoPatt != NULL)
            UpdateEventsFrames(i->autoPatt);

        i = i->next;
    }
}

void UpdatePerBPM()
{
    float tickoldMain = (float)pbkMain->currTick;
    float tickoldAux = (float)pbkAux->currTick;

    if(beats_per_minute < 10)
        beats_per_minute = 10;
    else if(beats_per_minute > 999)
        beats_per_minute = 999;

    SetBPM(beats_per_minute);
    seconds_in_tick = 1.0f/(beats_per_minute/60.f*ticks_per_beat);
    frames_per_tick = seconds_in_tick*fSampleRate;

    one_divided_per_frames_per_tick = 1/(seconds_in_tick*fSampleRate);

    framesPerPixel = seconds_in_tick*fSampleRate/tickWidth;
    aux_panel->frames_per_pixel = seconds_in_tick*fSampleRate/aux_panel->tickWidth;

    UpdateFrames();
    UpdateAllElements(field_pattern, true);

    UpdateScaledImages();

    UpdateTime(Loc_MainGrid);

    J_RefreshGridImage();
    J_RefreshAuxGridImage();

    pbkMain->SetCurrTick(tickoldMain);
    pbkAux->AlignRangeToPattern();
    pbkAux->SetCurrTick(tickoldAux);

    Selection_UpdateCoords();
    Looping_UpdateCoords();
}

void SetScale(float scale)
{
    tickWidthBack = scale;
    AdjustTick();
    //float flen = (float)(GridX2 - GridX1)/(float)tickWidth;

    /*
    if(changeoffs)
    {
        if(C.loc == Loc_SmallGrid)
        {
            OffsTick = C.fieldposx - 0.5f*flen;
        }
        else
        {
            OffsTick = CTick - 0.5f*flen;
        }

        if(OffsTick < 0)
        {
            OffsTick = 0;
        }
    }
    */

    UpdatePerScale();

    R(Refresh_Grid);
    if(aux_panel->isVolsPansMode())
    	R(Refresh_Aux);
}

void AdjustTick()
{
    tickWidth = tickWidthBack;
    float pixstep = float(tickWidth*ticks_per_beat);
    bool good = float(pixstep - (int)pixstep) == 0;
    if(!good)
    {
        tickWidth = float(RoundFloat(pixstep))/ticks_per_beat;
    }
}

void IncreaseScale(bool mouse)
{
    if(tickWidthBack < 32)
    {
        tickWidth = tickWidthBack;

        float flen = (float)(GridX2 - GridX1)/(float)tickWidth;
        float zm = (M.current_tick - OffsTick)/flen;

        if(tickWidth < 1)
            tickWidth += 0.25f;
        else if(tickWidth < 2)
            tickWidth += 0.5f;
        else if(tickWidth < 6)
            tickWidth++;
        else if(tickWidth < 12)
            tickWidth += 2;
        else
            tickWidth += 4;

        if(tickWidth > 32)
            tickWidth = 32;

        CP->slzoom->SetTickWidth(tickWidth);

        tickWidthBack = tickWidth;
        AdjustTick();

        flen = (float)(GridX2 - GridX1)/(float)tickWidth;
        if(mouse == true)
        {
            OffsTick = M.current_tick - zm*flen;
        }
        /*
        else
        {
            if(C.loc == Loc_SmallGrid)
            {
                OffsTick = C.fieldposx - 0.5f*flen;
            }
            else
            {
                OffsTick = CTick - 0.5f*flen;
            }
        }*/

        if(OffsTick < 0)
        {
            OffsTick = 0;
        }

        UpdatePerScale();

        R(Refresh_Grid);
        if(aux_panel->isVolsPansMode())
        {
            R(Refresh_Aux);
        }
    }
}

void DecreaseScale(bool mouse)
{
    if(tickWidthBack > 0.5f)
    {
        tickWidth = tickWidthBack;

        float flen = (float)(GridX2 - GridX1)/(float)tickWidth;
        float zm = (M.current_tick - OffsTick)/flen;

        if(tickWidth > 12)
            tickWidth -= 4;
        else if(tickWidth > 6)
            tickWidth -= 2;
        else if(tickWidth > 2)
        {
            tickWidth--;
            if(tickWidth == 0)
                tickWidth = 1;
        }
        else if(tickWidth > 1.f)
        {
            tickWidth -= 0.5f;
            if(tickWidth == 0)
                tickWidth = 0.5f;
        }
        else if(tickWidth > 0.5f)
        {
            tickWidth -= 0.25f;
            if(tickWidth < 0.5f)
                tickWidth = 0.5f;
        }

        CP->slzoom->SetTickWidth(tickWidth);

        tickWidthBack = tickWidth;
        AdjustTick();

        flen = (float)(GridX2 - GridX1)/(float)tickWidth;
        if(mouse == true)
        {
            OffsTick = M.current_tick - zm*flen;
        }
        /*
        else
        {
            if(C.loc == Loc_SmallGrid)
            {
                OffsTick = C.fieldposx - 0.5f*flen;
            }
            else
            {
                OffsTick = CTick - 0.5f*flen;
            }
        }*/

        if(OffsTick < 0)
        {
            OffsTick = 0;
        }

        UpdatePerScale();

        R(Refresh_Grid);
        if(aux_panel->isVolsPansMode())
        {
            R(Refresh_Aux);
        }
    }
}

void DragNDrop_Process(int mouse_x, int mouse_y)
{
    if((M.mmode & MOUSE_AUXMIXING && M.drag_data.drag_type == Drag_MixChannel_Effect) ||
       (M.mmode & MOUSE_BROWSING && M.drag_data.drag_type == Drag_Effect_File)||
       (M.mmode & MOUSE_INSTRUMENTING && M.drag_data.drag_type == Drag_Instrument_Alias))
    {
        // Highlight corruption workaround
        if(aux_panel->auxmode == AuxMode_Mixer)
        {
            R(Refresh_AuxHighlights);
        }

        M.active_dropmixchannel = NULL;
        MixChannel* oadmc = M.active_dropmixchannel;
        MixChannel* mchan = NULL;
        mchan = &aux_panel->masterchan;
        if(mchan->visible && mouse_x >= mchan->x && mouse_x <= (mchan->x + mchan->width) && 
                             mouse_y >= mchan->y && mouse_y <= (mchan->y + mchan->height))
        {
            M.active_dropmixchannel = mchan;
        }
        else
        {
            for(int mc = 0; mc < NUM_MIXCHANNELS; mc++)
            {
                mchan = &aux_panel->mchan[mc];
                if(mchan->visible)
                {
                    if(mouse_x >= mchan->x && mouse_x <= (mchan->x + mchan->width) && 
                       mouse_y >= (mchan->y - 10) && mouse_y <= (mchan->y + mchan->height))
                    {
                        M.active_dropmixchannel = mchan;
                        break;
                    }
                }
            }

            if(M.active_dropmixchannel == NULL)
            {
                for(int mc = 0; mc < 3; mc++)
                {
                    mchan = &aux_panel->sendchan[mc];
                    if(mchan->visible)
                    {
                        if(mouse_x >= mchan->x && mouse_x <= (mchan->x + mchan->width) && 
                           mouse_y >= (mchan->y - 10) && mouse_y <= (mchan->y + mchan->height))
                        {
                            M.active_dropmixchannel = mchan;
                            break;
                        }
                    }
                }
            }
        }
        
        if((M.mmode & MOUSE_AUXMIXING && M.drag_data.drag_type == Drag_MixChannel_Effect) ||
           (M.mmode & MOUSE_BROWSING && M.drag_data.drag_type == Drag_Effect_File))
        {
            Eff* oadeff1 = M.dropeff1;
            Eff* oadeff2 = M.dropeff2;
            M.dropeff1 = M.dropeff2 = NULL;
            if(M.active_dropmixchannel != NULL && mouse_y > mixY && mouse_y <= M.active_dropmixchannel->ry2)
            {
                Eff* eff = M.active_dropmixchannel->first_eff;
                while(eff != NULL)
                {
                    if(eff->visible)
                    {
                        if(mouse_y >= eff->y + eff->height/2 && 
                           (eff->cnext == NULL || mouse_y <= eff->cnext->y + eff->cnext->height/2))
                        {
                            M.dropeff1 = eff;
                        }
                        
						if(mouse_y <= eff->y + eff->height/2 && 
							(eff->cprev == NULL || mouse_y >= eff->cprev->y + eff->cprev->height/2))
                        {
                            M.dropeff2 = eff;
                            if(M.dropeff2 == M.dropeff1)
                            {
                                M.dropeff2 = M.dropeff2->cnext;
                            }
                        }
                    }

                    if(M.dropeff1 != NULL && M.dropeff2 != NULL)
                    {
                        break;
                    }

                    eff = eff->cnext;
                }
            }

            if((aux_panel->auxmode == AuxMode_Mixer) && (oadeff1 != M.dropeff1 || oadeff2 != M.dropeff2))
            {
                R(Refresh_AuxHighlights);
            }
        }
    }

    if(M.drag_data.drag_type == Drag_Instrument_File ||
       (M.mmode & MOUSE_INSTRUMENTING && M.drag_data.drag_type == Drag_Instrument_Alias))
    {
        Instrument* oadinstr1 = M.dropinstr1;
        Instrument* oadinstr2 = M.dropinstr2;
        M.dropinstr1 = M.dropinstr2 = NULL;
        if(mouse_y > IP->y + brw_heading + 1 && 
           mouse_y < IP->y + brw_heading + 1 + InstrPanelHeight)
        {
            Instrument* i = first_instr;
            while(i != NULL)
            {
                if(i->visible && (mouse_x >= i->x && mouse_x <= i->x + i->width)&&
                                 (mouse_y >= i->y + InstrUnfoldedHeight/2 && mouse_y <= i->y + InstrUnfoldedHeight/2 + i->height))
                {
                    M.dropinstr1 = i;
                }

                if(i->visible && (mouse_x >= i->x && mouse_x <= i->x + i->width)&&
                                 (mouse_y >= i->y - InstrUnfoldedHeight/2 && mouse_y <= i->y - InstrUnfoldedHeight/2 + i->height))
                {
                    M.dropinstr2 = i;
                    if(M.dropinstr2 == M.dropinstr1)
                    {
                        M.dropinstr2 = M.dropinstr2->next;
                    }
                }

                if(M.dropinstr1 != NULL && M.dropinstr2 != NULL)
                {
                    break;
                }

                i = i->next;
            }
        }

        if(oadinstr2 != M.dropinstr2 || oadinstr1 != M.dropinstr1)
        {
            R(Refresh_InstrPanel);
        }
    }

    if(aux_panel->auxmode == AuxMode_Mixer && 
        (M.drag_data.drag_type == Drag_MixChannel_Index || M.drag_data.drag_type == Drag_Command))
    {
        // Highlight corruption workaround
        R(Refresh_AuxHighlights);
    }

}

bool DragNDrop_Check(int mouse_x, int mouse_y)
{
    if(!(M.mmode & MOUSE_CONTROLLING) && M.mmode & MOUSE_INSTRUMENTING)
    {
        if(M.active_instr != NULL)
        {
            if(M.drag_data.dragcount < 1)
            {
                M.drag_data.dragcount++;
            }
            else
            {
                M.drag_data.dragcount = 0;
                M.drag_data.drag_type = Drag_Instrument_Alias;
                M.drag_data.drag_stuff = (void*)M.active_instr->alias;
                Instrument* instr = M.active_instr;
                if(instr != NULL)
                {
                    M.drag_data.instr_len = instr->GetPixelNoteLength(tickWidth, false);
                    M.mmode |= MOUSE_DRAG_N_DROPPING;
                }
            }
        }
    }
    else if(!(M.mmode & MOUSE_CONTROLLING)&&((M.mmode & MOUSE_BROWSING) == MOUSE_BROWSING))
    {
        if(M.btype == Brw_Generators)
        {
            if(genBrw->brwmode == Browse_Files || 
               genBrw->brwmode == Browse_Plugins || 
               genBrw->brwmode == Browse_Samples)
            {
                //if(M.drag_data.tsop == false)
                {
                    M.drag_data.tsop = true;
                    M.drag_data.drag_index = M.brwindex;
                }

                if(M.drag_data.dragcount < 1)
                {
                    M.drag_data.dragcount++;
                }
                else if(M.drag_data.drag_index >= 0)
                {
                    M.drag_data.dragcount = 0;
                    FileData* fd = genBrw->GetFileDataByIndex(M.drag_data.drag_index);
                    if(fd != NULL && (fd->ftype == FType_Wave || 
									  fd->ftype == FType_VST ||
									  fd->ftype == FType_Native))
                    {
                        M.drag_data.drag_type = Drag_Instrument_File;
                        M.drag_data.drag_stuff = (void*)fd;
                        M.mmode |= MOUSE_DRAG_N_DROPPING;
                    }
                }
            }
        }
        else if(M.btype == Brw_Effects)
        {
            if(mixBrw->brwmode == Browse_Plugins)
            {
                if(M.drag_data.tsop == false)
                {
                    M.drag_data.tsop = true;
                    M.drag_data.drag_index = M.brwindex;
                }

                if(M.drag_data.dragcount < 1)
                {
                    M.drag_data.dragcount++;
                }
                else
                {
    				M.drag_data.dragcount = 0;
    				FileData* fd = mixBrw->GetFileDataByIndex(M.drag_data.drag_index);

                    if(M.drag_data.drag_index >= 0 && fd != NULL)
                    {
    					M.drag_data.drag_type = Drag_Effect_File;
    					M.drag_data.drag_stuff = (void*)fd;
    					M.mmode |= MOUSE_DRAG_N_DROPPING;
    				}
                }
            }
        }
    }
    else if(!(M.mmode & MOUSE_CONTROLLING) && (M.mmode & MOUSE_AUXMIXING) && M.active_mixchannel != NULL)
    {
        if(M.auxcellindexing == true)
        {
            M.drag_data.drag_type = Drag_MixChannel_Index;
            M.drag_data.drag_stuff = (void*)M.active_mixchannel;
            M.mmode |= MOUSE_DRAG_N_DROPPING;
        }
        else if(M.active_effect != NULL)
        {
            if(M.drag_data.dragcount < 1)
            {
                M.drag_data.dragcount++;
            }
            else
            {
                M.drag_data.dragcount = 0;
                M.drag_data.drag_type = Drag_MixChannel_Effect;
                M.drag_data.drag_stuff = (void*)M.active_effect;
                //M.drag_data.drag_len = instr_font.width(M.active_mixcell->mixstr->string);
                M.mmode |= MOUSE_DRAG_N_DROPPING;
            }
        }
    }

    return ((M.mmode & MOUSE_DRAG_N_DROPPING) != 0);
}

void DragNDrop_PlaceEffect(Eff* eff)
{
    jassert(eff != M.dropeff1 && eff != M.dropeff2);
    jassert(M.active_dropmixchannel->first_eff != NULL && M.active_dropmixchannel->last_eff != NULL);

	if(eff->cprev != NULL)
    {
		eff->cprev->cnext = eff->cnext;
        if(eff == M.active_dropmixchannel->last_eff)
        {
            M.active_dropmixchannel->last_eff = eff->cprev;
        }
    }

	if(eff->cnext != NULL)
    {
		eff->cnext->cprev = eff->cprev;
        if(eff == M.active_dropmixchannel->first_eff)
        {
            M.active_dropmixchannel->first_eff = eff->cnext;
        }
    }

    if(M.dropeff1 != NULL)
    {
        M.dropeff1->cnext = eff;
        eff->cprev = M.dropeff1;

        if(M.dropeff1 == M.active_dropmixchannel->last_eff)
        {
            M.active_dropmixchannel->last_eff = eff;
        }
    }
    else
    {
        eff->cprev = NULL;
    }

    if(M.dropeff2 != NULL)
    {
        M.dropeff2->cprev = eff;
        eff->cnext = M.dropeff2;

        if(M.dropeff2 == M.active_dropmixchannel->first_eff)
        {
            M.active_dropmixchannel->first_eff = eff;
        }
    }
    else
    {
        eff->cnext = NULL;
    }
}

void DragNDrop_PlaceInstrument()
{
	if(current_instr->prev != NULL)
    {
		current_instr->prev->next = current_instr->next;
        if(current_instr == last_instr)
        {
            last_instr = current_instr->prev;
        }
    }

	if(current_instr->next != NULL)
    {
		current_instr->next->prev = current_instr->prev;
        if(current_instr == first_instr)
        {
            first_instr = current_instr->next;
        }
    }

    if(M.dropinstr1 != NULL)
    {
        M.dropinstr1->next = current_instr;
        current_instr->prev = M.dropinstr1;

        if(M.dropinstr1 == last_instr)
        {
            last_instr = current_instr;
        }
    }
    else
    {
        current_instr->prev = NULL;
    }

    if(M.dropinstr2 != NULL)
    {
        M.dropinstr2->prev = current_instr;
        current_instr->next = M.dropinstr2;

        if(M.dropinstr2 == first_instr)
        {
            first_instr = current_instr;
        }
    }
    else
    {
        current_instr->next = NULL;
    }

    IP->UpdateIndices();
    UpdateStepSequencers();
    UpdateScaledImages();

    R(Refresh_InstrPanel);
    if(aux_panel->auxmode == AuxMode_Pattern && aux_panel->workPt->ptype == Patt_StepSeq)
    {
        R(Refresh_AuxGrid);
        R(Refresh_PianoKeys);
    }
    R(Refresh_GridContent);
}

void DragNDrop_Drop(int mouse_x, int mouse_y, unsigned int flags)
{
    M.mmode &= ~MOUSE_DRAG_N_DROPPING;
    M.drag_data.tsop = false;
    if(M.drag_data.drag_type == Drag_Instrument_Alias)
    {
        TString* al = (TString*)M.drag_data.drag_stuff;
        if(M.at_definer == true)
        {
            M.current_trk->defined_instr = al->instr;
        }
        else if(M.loc == Loc_MainGrid)
        {
            if(M.snappedTick > 0 && M.snappedLine < field_pattern->num_lines)
            {
                M.patt = field_pattern;
                C.SetPattern(field_pattern, Loc_MainGrid);
                C.SetPos(M.snappedTick, M.snappedLine);
                al->instr->ResizeTouch(false);
                C.curElem = CreateElement_Note(al->instr, true);
                C.curElem->Switch2Param(NULL);

                R(Refresh_GridContent);
                if(aux_panel->auxmode == AuxMode_Vols || 
                   aux_panel->auxmode == AuxMode_Pans)
                {
                    R(Refresh_Aux);
                }
            }
        }
        else if(M.loc == Loc_SmallGrid && aux_panel->workPt->ptype != Patt_StepSeq)
        {
            if(aux_panel->workPt->ibound == NULL || aux_panel->workPt->ibound == al->instr)
            {
                if(aux_panel->isBlank())
                {
                    aux_panel->CreateNew();
                }
                M.patt = aux_panel->workPt;
                C.SetPattern(aux_panel->workPt, Loc_SmallGrid);
                C.SetPos(M.snappedTick, M.snappedLine);
                al->instr->ResizeTouch(false);
                C.curElem = CreateElement_Note(al->instr, true);
                C.curElem->Switch2Param(NULL);

                aux_panel->RescanPatternBounds();
            }

            R(Refresh_GridContent);
        }
        else if(M.loc == Loc_UNKNOWN)
        {
            if(aux_panel->CheckIfMouseOnBinder(mouse_x, mouse_y))
            {
                if(aux_panel->workPt == aux_panel->blankPt)
                {
                    aux_panel->CreateNew();
                }
                BindPatternToInstrument(aux_panel->workPt, al->instr);

                R(Refresh_GridContent);
            }
            else if(M.active_dropmixchannel != NULL)
            {
                MixChannel* mchan = (MixChannel*)M.active_dropmixchannel;
                DigitStr* pfx = al->instr->dfxstr;
                strcpy(pfx->digits, mchan->indexstr);
                aux_panel->CheckFXString(pfx);
                pfx->drawarea->Change();
                M.active_dropmixchannel = NULL;
                R(Refresh_AuxHighlights);
            }
            else if((M.dropinstr1 != NULL || M.dropinstr2 != NULL) && 
                     M.dropinstr1 != current_instr && M.dropinstr2 != current_instr)
            {
                if(flags & kbd_ctrl)
                {
                    Instrument* ni = current_instr->Clone();
                    ChangeCurrentInstrument(ni);
                    DragNDrop_PlaceInstrument();
                }
                else
                    DragNDrop_PlaceInstrument();
            }
        }
    }
    else if(M.drag_data.drag_type == Drag_Instrument_File)
    {
        if(M.IsMouseOnInstrPane(mouse_x, mouse_y) == true || 
           M.IsMouseOnMainGrid(mouse_x, mouse_y) == true)
        {
            FileData* fd = (FileData*)M.drag_data.drag_stuff;
            Instrument* i = IP->AddInstrumentFromBrowser(fd, false);
            M.lastclick = LastClicked_Instrument;
            if(i != NULL)
            {
                if(M.dropinstr2 != NULL && 
                   M.dropinstr1 != current_instr && 
                   M.dropinstr2 != current_instr)
                {
                    DragNDrop_PlaceInstrument();
                }
                else
                {
                    IP->GoDown();
                }
            }
            else
            {
                R(Refresh_InstrPanel);
            }
        }
    }
    else if(M.drag_data.drag_type == Drag_MixChannel_Index)
    {
        if(M.active_paramfx != NULL)
        {
            MixChannel* mchan = (MixChannel*)M.drag_data.drag_stuff;
            DigitStr* pfx = (DigitStr*)M.active_paramfx;
            //DigitStr* pfx = (DigitStr*)M.active_instr->dfxstr;
            strcpy(pfx->digits, mchan->indexstr);
            if(pfx->panel != NULL)
            {
                pfx->panel->Update();
            }
            if(pfx->instr != NULL || pfx->trk != NULL)
            {
                aux_panel->CheckFXString(pfx);
                if(pfx->instr != NULL)
                {
                    ChangeCurrentInstrument(pfx->instr);
                }
            }
            pfx->drawarea->Change();
        }
    }
    if(M.drag_data.drag_type == Drag_Effect_File)
    {
        if(M.active_dropmixchannel != NULL)
        {
            WaitForSingleObject(aux_panel->hMixMutex, INFINITE);
            FileData* fd = (FileData*)M.drag_data.drag_stuff;
            Eff* eff = aux_panel->AddEffectFromBrowser(fd, M.active_dropmixchannel);
            M.lastclick = LastClicked_Effect;
            if(eff != NULL)
            {
                DragNDrop_PlaceEffect(eff);
            }
            M.active_dropmixchannel = NULL;
            ReleaseMutex(aux_panel->hMixMutex);

            R(Refresh_Aux);
            R(Refresh_AuxHighlights);
        }
    }
    else if(M.drag_data.drag_type == Drag_MixChannel_Effect)
    {
        if(M.active_dropmixchannel != NULL)
        {
            Eff* eff = (Eff*)M.drag_data.drag_stuff;
            if(mouse_y > mixY &&
               mouse_y <= M.active_dropmixchannel->ry2 &&
               eff != M.dropeff1 && eff != M.dropeff2)
            {
                WaitForSingleObject(aux_panel->hMixMutex, INFINITE);
                if(flags & kbd_ctrl)
                {
                    Eff* neff = eff->Clone(M.active_dropmixchannel->mc_main);
                    neff->folded = eff->folded;
                    neff->bypass = eff->bypass;
                    eff = neff;
                    AddEff(neff);
                }
                else
                {
                    M.active_mixchannel->RemoveEffect(eff);
                    eff->cprev = eff->cnext = NULL;
                }
                M.active_dropmixchannel->AddEffect(eff);
    			DragNDrop_PlaceEffect(eff);
                ReleaseMutex(aux_panel->hMixMutex);

                R(Refresh_Aux);
                R(Refresh_AuxHighlights);
                R(Refresh_GridContent);
            }
        }
    }
    else if(M.drag_data.drag_type == Drag_Command)
    {
        Command* cmd = (Command*)M.drag_data.drag_stuff;
        if(M.loc == Loc_MainGrid || (M.loc == Loc_SmallGrid && aux_panel->workPt->ptype == Patt_Grid))
        {
            if(M.loc == Loc_MainGrid)
            {
    			if(M.snappedTick >= 0 && M.snappedLine < field_pattern->num_lines)
    			{
    				cmd->patt = field_pattern;
    				cmd->start_tick = M.snappedTick;
    				cmd->track_line = M.snappedLine;
    				cmd->Update();

    				C.patt = field_pattern;
    				C.loc = Loc_MainGrid;
    			}
            }
            else if(M.loc == Loc_SmallGrid && aux_panel->workPt->ptype == Patt_Grid)
            {
                if(aux_panel->workPt == aux_panel->blankPt)
                {
                    aux_panel->CreateNew();
                }
                M.patt = aux_panel->workPt;
                cmd->patt = aux_panel->workPt->basePattern;
                cmd->start_tick = M.snappedTick;
                cmd->track_line = M.snappedLine;
                cmd->Update();

                C.patt = aux_panel->workPt;
                C.loc = Loc_SmallGrid;
            }

			C.mode = CMode_ElemEdit;
			C.curElem = cmd;
			AddNewElement(cmd, false);
			if(M.loc == Loc_MainGrid)
			{
				R(Refresh_GridContent);
			}
			else if(M.loc == Loc_SmallGrid)
			{
				aux_panel->RescanPatternBounds();
				R(Refresh_AuxContent);
			}
        }
    }

    M.holding = false;
    M.dropinstr1 = M.dropinstr2 = NULL;
    M.dropeff1 = M.dropeff2 = NULL;
    M.active_dropmixchannel = NULL;
}

void CheckStepSeqPos()
{
    if(C.loc == Loc_SmallGrid && aux_panel->workPt->ptype == Patt_StepSeq)
    {
        Element* el = aux_panel->workPt->basePattern->IsElemExists(CTick, CLine);
        if(el != NULL)
        {
            el->Activate();
			R(Refresh_AuxContent);
        }
    }
}

void ChangeCurrentInstrument(Instrument* instr)
{
    if(instr != NULL && instr != current_instr)
    {
        bool currvisible = false; // cancelled to avoid odd behaviour
        if(current_instr != NULL)
        {
            currvisible = current_instr->SwitchWindowOFFIfNeeded();
            //current_instr->alias->active = false;
            current_instr->instr_drawarea->Change();
        }
        current_instr = instr;
        //current_instr->alias->active = true;
        current_instr->instr_drawarea->Change();

        if(genBrw->brwmode == Browse_Presets || 
           genBrw->brwmode == Browse_Params)
        {
            genBrw->Update();
        }

        R(Refresh_Aux);
        R(Refresh_InstrPanel);
        if(C.mode == CMode_NotePlacing)
        {
            R(Refresh_KeyCursor);
        }
    }
}

void Process_TrackEdges(int mouse_x, int mouse_y)
{
    if(M.mmode & MOUSE_UPPER_TRACK_EDGE)
    {
        if(M.prbunch == false)  // bunch edge
        {
            // find new line for the bunch
            int nl = Y2Line(mouse_y, Loc_MainGrid) - M.patt->track_line;
            if(nl >= M.track_bunch->trk_end->start_line)
            {
                nl = M.track_bunch->trk_end->start_line - 1;
            }
            else if(nl < 0)
            {
                nl = 0;
            }

            Trk* ntrk = GetTrkDataForLine(nl, M.patt);
            while(ntrk->trk_start_line < M.track_bunch->trk_start->trk_start_line)
            {
                if((ntrk->buncho || ntrk->bunched) && ntrk->trkbunch != M.track_bunch)
                    return;
                ntrk = ntrk->next;
            }

            // Reset current bunch entries except bunchito
            M.track_bunch->trk_start->buncho = false;
            M.track_bunch->trk_start->trkbunch = NULL;
            int ti = M.track_bunch->start_track;
            Trk* trk = M.track_bunch->trk_start;
            while(trk != M.track_bunch->trk_end)
            {
                trk->bunched = false;
                trk->trkbunch = NULL;
                trk = trk->next;
            }

            // init new bunch entries except bunchito
            M.track_bunch->trk_start = GetTrkDataForLine(nl, M.patt);
            M.track_bunch->trk_start->trkbunch = M.track_bunch;
            M.track_bunch->trk_start->buncho = true;
            M.track_bunch->start_track = M.track_bunch->trk_start->trknum;
            M.track_bunch->startpix = nl*lineHeight;

            trk = M.track_bunch->trk_start;
            trk = trk->next;
            while(trk!= M.track_bunch->trk_end)
            {
                trk->bunched = true;
                trk->trkbunch = M.track_bunch;
                if(trk->pan_lane.visible)
                {
                    HidePanLane(trk, M.patt);
                }
                if(trk->vol_lane.visible)
                {
                    HideVolLane(trk, M.patt);
                }
                trk = trk->next;
            }
            M.track_bunch->endpix = M.track_bunch->trk_end->trk_end_line*lineHeight + lineHeight;
        }
        UpdateAllElements(M.patt);
    }
    else if(M.mmode & MOUSE_LOWER_TRACK_EDGE)
    {
        if(M.prbunch == false)  // bunch edge
        {
            int nl = Y2Line(mouse_y, Loc_MainGrid) - M.patt->track_line;
            if(nl <= M.track_bunch->trk_start->end_line)
            {
                nl = M.track_bunch->trk_start->end_line + 1;
            }

            Trk* ntrk = GetTrkDataForLine(nl, M.patt);
            while(ntrk->trk_start_line > M.track_bunch->trk_end->trk_end_line)
            {
                if((ntrk->buncho || ntrk->bunched) && ntrk->trkbunch != M.track_bunch)
                    return;
                ntrk = ntrk->prev;
            }

            M.track_bunch->trk_end->bunchito = false;
            M.track_bunch->trk_end->trkbunch = NULL;

            Trk* trk = M.track_bunch->trk_end;
            while(trk != M.track_bunch->trk_start)
            {
                trk->bunched = false;
                trk->trkbunch = NULL;
                trk = trk->prev;
            }
    
            M.track_bunch->trk_end = GetTrkDataForLine(nl, M.patt);
            M.track_bunch->end_track = M.track_bunch->trk_end->trknum;

            M.track_bunch->endpix = nl*lineHeight + lineHeight;
            M.track_bunch->trk_end->bunchito = true;
            M.track_bunch->trk_end->trkbunch = M.track_bunch;

            trk = M.track_bunch->trk_end;
            while(trk != M.track_bunch->trk_start)
            {
                trk->bunched = true;
                trk->trkbunch = M.track_bunch;
                if(trk->pan_lane.visible)
                {
                    HidePanLane(trk, M.patt);
                }
                if(trk->vol_lane.visible)
                {
                    HideVolLane(trk, M.patt);
                }
                trk = trk->prev;
            }
            M.track_bunch->startpix = M.track_bunch->trk_start->trk_start_line*lineHeight;
        }
        UpdateAllElements(M.patt);
    }
}

void Grid_ProcessBrush()
{
    if(M.lpx != M.snappedTick && (M.loc == M.brushloc))
    {
        float step = M.qsize;
        if(step <= 0) step = 0.1f;
        if(M.brushmode == Brush_Pattern || M.brushmode == Brush_Envelope)
        {
            M.snappedTick = M.brushbasetick + (float)(int)((M.snappedTick - M.brushbasetick)/M.brushwidth)*M.brushwidth;
            step = M.brushwidth;
        }

        float xtick = M.lpx;
        while(1)
        {
            if((M.loc == Loc_MainGrid && IsElemExists(xtick, M.lpy, field_pattern) == NULL)||
               (M.loc == Loc_SmallGrid && aux_panel->workPt->basePattern->IsElemExists(xtick, M.lpy) == NULL))
            {
                Element* el;
                if(M.active_elem != NULL)
                {
                    //M.loc == Loc_MainGrid && C.curElem != NULL && C.curElem->type == El_Pattern
                    C.SaveState();
                    CTick = xtick;
                    CLine = M.lpy;
                    C.SetPattern(M.patt, M.loc);
                    if(M.brushmode == Brush_Pattern)
                    {
                        Pattern* pt = (Pattern*)M.active_elem;
                        el = (Element*)pt->Clone(true, CTick, CLine);
                    }
                    else
                    {
                        Pattern* pt = (Pattern*)C.curElem;
                        el = M.active_elem->Clone(true);
                        el->Move(CTick - el->start_tick, 0);
                    }
                    M.active_elem->highlighted = false;
                    M.active_elem = el;
                    M.active_elem->highlighted = true;
                    C.RestoreState();
                }
                else
                {
                    CTick = xtick;
                    CLine = M.lpy;
                    if(CP->NoteMode->pressed == true)
                    {
                        el = CreateElement_Instance_byChar(C.last_char);
                    }
                    else
                    {
                        el = CreateElement_Instance(false);
                    }

                    C.ExitToCurrentMode();
                    el->Activate();
                }

                if(M.loc == Loc_SmallGrid && el != NULL)
                {
                    // stepseq specific
                    if(aux_panel->workPt->ptype == Patt_StepSeq)
                    {
                        NoteInstance* ii = (NoteInstance*)el;
                        el->last_edited_param = ii->ed_note; //Switch2Param(ii->ed_note);
                    }
                    else
                    {
                        aux_panel->RescanPatternBounds();
                    }
                }
            }

            if(M.snappedTick > M.lpx)
            {
                xtick += step;
                if(xtick > M.snappedTick)
                    break;
            }
            else
            {
                xtick -= step;
                if(xtick < M.snappedTick)
                    break;
            }
        }
        M.lpx = M.snappedTick;
    }
}

/*==================================================================================================
BRIEF: This function puts instance of current instrument to current cursor position on the grid 
under mouse (M.loc variable defines grid type) if there no already an element exists in that
position. It also initiates brushing, if we're under brush conditions or if we're on step sequencer.

PARAMETERS:
[IN]
    mouse_x, mouse_y - mouse coordinates (auxilliary).

[OUT]
N/A

OUTPUT: N/A
==================================================================================================*/
void Grid_PutInstance(int mouse_x, int mouse_y, unsigned flags, bool slide)
{
    bool creatednew = false;
    if(M.loc == Loc_SmallGrid)
    {
        // Change current instrument to the one defined at stepsequencer line
        if(aux_panel->workPt->ptype == Patt_StepSeq)
        {
            ChangeCurrentInstrument(M.current_trk->defined_instr);
        }

        // Create a new pattern if Aux grid is blank
        if(aux_panel->workPt == aux_panel->blankPt)
        {
            aux_panel->CreateNew();
            creatednew = true;
            M.patt = aux_panel->workPt;
        }
    }

    Element* auxelem;
    bool auxelemexists = false;
    if(M.loc == Loc_SmallGrid)
    {
        auxelem = aux_panel->workPt->basePattern->IsElemExists(M.snappedTick, M.snappedLine);
        auxelemexists = (auxelem != NULL && auxelem->IsInstance());
    }

    // If there're no already an element on target position, then create it there
    if((M.loc == Loc_MainGrid && IsElemExists(M.snappedTick, M.snappedLine, field_pattern) == NULL)||
       (M.loc == Loc_SmallGrid && !auxelemexists))
    {
        Element* el = NULL;

        if(slide)
        {
            el = (Element*)C.PlaceSlideNote(-1, flags);
        }
        else
        {
            NoteInstance* ii = CreateElement_Instance(true);

            if(CP->NoteMode->pressed == true)
            {
                ii->ed_note->Show();
                ii->Switch2Param(ii->ed_note);
            }
            el = (Element*)ii;
        }

        // adjust Aux pattern bounds if it's not a stepseq
        if(M.loc == Loc_SmallGrid)
        {
            aux_panel->RescanPatternBounds();
        }

        // stepseq specific
        if(el != NULL && M.loc == Loc_SmallGrid && aux_panel->workPt->ptype == Patt_StepSeq)
        {
            NoteInstance* ii = (NoteInstance*)el;
            el->last_edited_param = ii->ed_note; //Switch2Param(ii->ed_note);
        }

        M.bypass_element_activating = true;

        if(el != NULL)
        {
            //if(el->type == El_SlideNote)
            //{
            //    el->Switch2Param(NULL);
            //   ((SlideNote*)el)->ed_note->Hide();
            //}
            //else
            {
                el->Activate();
            }

            if(el->type != El_SlideNote &&
                    (CP->BrushMode->pressed == true || 
                     flags & kbd_alt ||
                     aux_panel->workPt->ptype == Patt_StepSeq))
            {
                // starts brushing if in brush mode
                M.mmode |= MOUSE_BRUSHING;
                if(el->type == El_Pattern)
                {
                    M.brushmode = Brush_Pattern;
                    M.brushbasetick = CTick;
                    M.brushwidth = el->tick_length;
                }
                else
                {
                    M.brushmode = Brush_Default;
                }
                M.lpx = M.snappedTick;
                M.lpy = M.snappedLine;
                M.brushloc = M.loc;
            }
            else if(el->type == El_SlideNote)
            {
                M.mmode |= MOUSE_SLIDING;
                M.mmode |= MOUSE_RESIZE;
                M.resize_object = Resize_GridElem;
                M.edge = RIGHT_X;
                M.resize_elem = el;
                M.movetick = M.snappedTick;
                M.moveline = M.snappedLine;
                M.pickloc = M.loc;
				M.resize_loc = M.loc;
            }
            else if(M.loc == Loc_SmallGrid && aux_panel->workPt->ptype == Patt_Pianoroll)
            {
                // pianorol or fast mode specific
                el->x = el->StartTickX();
                el->y = el->TrackLine2Y();
                el->picked = true;
                M.pickedup = true;
                M.pickloc = M.loc;
                M.movetick = M.snappedTick;
                M.moveline = M.snappedLine;
                M.prepianorolling = true;
                //M.selloc = Loc_SmallGrid;
                M.active_elem = el;
                el->xs = mouse_x;
                el->ys = mouse_y;

                // Bind pianorol if needed
                if(AutoBindPatterns && creatednew)
                {
                    BindPatternToInstrument(aux_panel->workPt, current_instr);
                }
            }

			// Since we don't call RescanPatternBounds for step sequencer, then update it's scaled image here
            if(M.loc == Loc_SmallGrid && aux_panel->workPt->ptype == Patt_StepSeq && !aux_panel->isBlank())
            {
                aux_panel->workPt->basePattern->UpdateScaledImage();
                R(Refresh_GridContent);
            }
        }
    }
    else if(auxelemexists && aux_panel->workPt->ptype == Patt_StepSeq)
    {
        NoteInstance* ii = (NoteInstance*)auxelem;
        auxelem->last_edited_param = ii->ed_note;
        auxelem->Activate();
        ii->ed_note->ProcessChar('~');
        ii->ed_note->Kreview(-1);

        // In step sequencer we always start brushing, even if there's already an element
        M.mmode |= MOUSE_BRUSHING;
        M.brushmode = Brush_Default;
        M.lpx = M.snappedTick;
        M.lpy = M.snappedLine;
        M.brushloc = M.loc;
    }
}

void Grid_PutInstanceSpecific(Pattern* patt, Instrument *instr, int note, float vol, tframe start_frame, tframe end_frame)
{
    if(patt != aux_panel->blankPt)
    {
        NoteInstance* i = NULL;
        float starttick = Frame2Tick(start_frame);
        float endtick = Frame2Tick(end_frame);
        int line = 10;

        /* In the piano roll, all notes must be in track lines correlating with piano keys*/
        if (patt->ptype == Patt_Pianoroll)
        {
            line = NUM_PIANOROLL_LINES - note;
        }
        else if (patt->ptype == Patt_Grid)
        {
            line = Random::getSystemRandom().nextInt(8);;
        }

        i = CreateElement_Instance_special(patt, instr, note, vol, starttick, endtick, line);

        if(i != NULL && patt == aux_panel->workPt)
        {
    		if(patt->ptype == Patt_StepSeq)
            {
                i->last_edited_param = i->ed_note;
            }
            else if(patt->ptype == Patt_Grid)
            {
                i->ed_note->Show();
            }
        }
    }
}

void Grid_MouseClickDown(int mouse_x, int mouse_y, unsigned flags)
{
    DropPickedElements();
    //if(M.loc == Loc_SmallGrid && gAux->workPt->ptype == Patt_StepSeq)
    //{
    //    M.pickedup = false;
    //}
    //else
    {
        M.pickedup = PickUpElements(mouse_x, mouse_y, flags);
        if(M.pickedup)
        {
            M.pickloc = M.loc;
        }
    }

    if((M.pickedup == false && !(flags & kbd_ctrl)) || flags & kbd_alt || M.patternbrush)
    {
        // Do not reset loop selection if we're working on another grid
        if(!(M.selmode == Sel_Fragment && M.selloc == Loc_MainGrid && M.loc == Loc_SmallGrid))
        {
            Selection_Reset();
        }
        Selection_ToggleElements();

        // Tracks stuff postponed
        /*
        if(mixbrowse == false && M.loc == Loc_MainGrid)
        {
            R(Refresh_MixCenter);
        }
        */
    
        if(M.pickedup == false && 
           (CP->FastMode->pressed == true || 
            CP->BrushMode->pressed == true || 
            flags & kbd_alt ||
            M.patternbrush ||
            flags & kbd_shift || // For slidenotes
           (M.loc == Loc_SmallGrid &&
           (aux_panel->workPt->ptype == Patt_Pianoroll || aux_panel->workPt->ptype == Patt_StepSeq))))
        {
            bool slide = false;
            if(flags & kbd_shift && !(M.loc == Loc_SmallGrid && aux_panel->workPt->ptype == Patt_StepSeq))
            {
                slide = true;
                // preserve cursor pos if sliding (postponed)
                //C.SaveState();
            }

            C.PosAndCursor2Mouse(mouse_x, mouse_y);
            Grid_PutInstance(mouse_x, mouse_y, flags, slide);

            /*
            if(slide)
            {
                C.ExitToDefaultMode();
                C.RestoreState();
            }*/

            if(M.loc == Loc_MainGrid)
            {
                R(Refresh_GridContent);
                if(aux_panel->isVolsPansMode())
                {
                    R(Refresh_Aux);
                }
            }
            else if(M.loc == Loc_SmallGrid)
            {
                R(Refresh_AuxGrid);
                R(Refresh_SubAux);
                R(Refresh_GridContent);
            }
        }
        else
        {
            if((flags & kbd_alt || M.patternbrush) && !(flags & kbd_ctrl) && M.pickedup == true)
            {
                // starts brushing if under brush conditions
                M.mmode |= MOUSE_BRUSHING;
                M.brushbasetick = M.active_elem->start_tick;
                M.lpx = M.brushbasetick;
                M.lpy = M.snappedLine;
                Element* el = M.active_elem;
                if(M.active_elem->type == El_Pattern)
                {
                    M.lpy = M.active_elem->track_line;
                    M.brushmode = Brush_Pattern;
                    M.brushwidth = M.active_elem->tick_length;
                }
                else if(el->type == El_Command && ((Command*)el)->IsEnvelope())
                {
                    M.lpy = M.active_elem->track_line;
                    M.brushmode = Brush_Envelope;
                    M.brushwidth = M.active_elem->tick_length;
                }
                else
                {
                    M.brushmode = Brush_Default;
                }
                M.brushloc = M.loc;
            }
            /*  // Moved to mouse click up
            else  // Cancel editing
            {
                C.ExitPerClickInAnotherGridPlace();
                C.PosAndCursor2Mouse(mouse_x, mouse_y);

                // Reset Aux patt mode if needed
                if(C.loc == Loc_MainGrid && 
    				gAux->workPt != gAux->blankPt &&
    			   !gAux->workPt->autopatt)
                {
                    gAux->AuxReset();
                    if(gAux->auto_switch == true)
                    {
                        gAux->HandleButtDown(gAux->Vols);
                    }
                }
                UpdateNavBarsData();
            }
            */

            R(Refresh_GridContent);
            R(Refresh_KeyCursor);
            if(C.loc == Loc_MainGrid)
            {
                R(Refresh_MainBar);
            }
            else
            {
                R(Refresh_AuxGrid);
            }
        }
    }
    else if(Num_Selected <= 1 && 
                (flags & kbd_ctrl || (M.loc == Loc_SmallGrid && aux_panel->workPt->ptype == Patt_Pianoroll)))
    {
        if(M.active_elem != NULL)
        {
            if(M.active_elem->type == El_Samplent || M.active_elem->type == El_GenNote)
            {
                NoteInstance* ii = (NoteInstance*)M.active_elem;
                ii->ed_note->Kreview(-1);
                if(M.loc == Loc_SmallGrid && aux_panel->workPt->ptype == Patt_Pianoroll)
                    M.prepianorolling = true;
            }
            else if(M.active_elem->type == El_SlideNote)
            {
                SlideNote* sn = (SlideNote*)M.active_elem;
                if(sn->parent != NULL)
                {
                    NoteInstance* ii = (NoteInstance*)(sn->parent);
                    Preview_Add(ii, NULL, -1, sn->ed_note->value, M.patt, NULL, NULL, true, sn->ed_note->relative);
                    if(M.loc == Loc_SmallGrid && aux_panel->workPt->ptype == Patt_Pianoroll)
                        M.prepianorolling = true;
                }
                else
                {
                    Preview_Add(NULL, current_instr, -1,  sn->ed_note->value, M.patt, NULL, NULL, true, sn->ed_note->relative);
                }
            }
        }
    }
}

void Grid_MouseDrag(int mouse_x, int mouse_y, unsigned flags)
{
    if(M.mmode & MOUSE_LANEAUXING)
    {
        if(M.loc == Loc_MainGrid)
        {
            int hgt, ylane;
            ParamType partype = (M.active_lane == Lane_Vol) ? Param_Vol : Param_Pan;
            if(M.active_lane == Lane_Vol)
            {
                hgt = M.current_trk->vol_lane.height*lineHeight;
                ylane = M.current_trk->vol_lane.y - hgt;
            }
            else if(M.active_lane == Lane_Pan)
            {
                hgt = M.current_trk->pan_lane.height*lineHeight;
                ylane = M.current_trk->pan_lane.y - hgt;
            }

            ProcessColLane(mouse_x, mouse_y, 
                           partype, 
                           M.loc, M.current_trk, NULL, 
                           GridX1, ylane, GridX2, ylane + hgt,
                           (flags & kbd_alt) != 0);

            R(Refresh_GridContent);
            if(aux_panel->auxmode == AuxMode_Pans || 
               aux_panel->auxmode == AuxMode_Vols)
            {
                R(Refresh_Aux);
            }
        }
        else if(M.loc == Loc_SmallGrid)
        {
            int hgt, ylane;
            ParamType partype = (M.active_lane == Lane_Vol) ? Param_Vol : Param_Pan;

            if(M.active_lane == Lane_Vol)
            {
                hgt = M.current_trk->vol_lane.height*lineHeight;
                ylane = M.current_trk->vol_lane.y - hgt;
            }
            else if(M.active_lane == Lane_Pan)
            {
                hgt = M.current_trk->pan_lane.height*lineHeight;
                ylane = M.current_trk->pan_lane.y - hgt;
            }

            ProcessColLane(mouse_x, mouse_y, 
                           partype, 
                           M.loc, M.current_trk, NULL, 
                           GridXS1, ylane, GridXS2, ylane + hgt,
                           (flags & kbd_alt) != 0);

            R(Refresh_AuxContent);
            R(Refresh_SubAux);
        }
    }
    else if(M.mmode & MOUSE_RESIZE && M.RMB == false)
    {
        Process_Resize(mouse_x, mouse_y, flags, M.loc);
        if(M.loc == Loc_MainGrid)
        {
            if(M.resize_object == Resize_Lane)
            {
                R(Refresh_Grid);
                if(mixbrowse == false)
                {
                    R(Refresh_MixCenter);
                }
            }
            else
            {
                R(Refresh_GridContent);
                if(aux_panel->auxmode == AuxMode_Vols || 
                   aux_panel->auxmode == AuxMode_Pans)
                {
                    R(Refresh_Aux);
                }
            }
        }
        else if(M.loc == Loc_SmallGrid)
        {
            if(M.resize_object == Resize_Lane || M.resize_object == Resize_AuxPattRange)
            {
                R(Refresh_Aux);
            }
            else
            {
                R(Refresh_AuxContent);
                R(Refresh_SubAux);
            }
        }
    }
    else if(M.mmode & MOUSE_ENVELOPING)
    {
        M.active_env->Drag(mouse_x, mouse_y, flags);

        ChangesIndicate();

        if(M.loc == Loc_MainGrid)
        {
            R(Refresh_GridContent);
        }
        else
        {
            R(Refresh_AuxContent);
        }
    }
    else if(M.mmode & MOUSE_DELETING)
    {
        if(M.delloc == M.loc)
        {
            if(M.delloc == Loc_MainGrid)
            {
                DeletePointedElement(M.prev_x, M.prev_y, mouse_x, mouse_y);

                R(Refresh_GridContent);
                if(aux_panel->isVolsPansMode())
                {
                    R(Refresh_Aux);
                }
            }
            else if(M.delloc == Loc_SmallGrid)
            {
                DeletePointedAuxElement(M.prev_x, M.prev_y, mouse_x, mouse_y);

                aux_panel->RescanPatternBounds();

                R(Refresh_AuxContent);
                R(Refresh_SubAux);
            }

            M.prev_x = mouse_x;
            M.prev_y = mouse_y;
        }
    }
    else if(M.mmode & MOUSE_UPPER_TRACK_EDGE || M.mmode & MOUSE_LOWER_TRACK_EDGE)
    {
        Process_TrackEdges(mouse_x, mouse_y);
    
        if(M.loc == Loc_MainGrid)
        {
            R(Refresh_Grid);
            if(mixbrowse == false)
            {
                R(Refresh_MixCenter);
            }
        }
        else
        {
            R(Refresh_Aux);
        }
    }
    else if(M.mmode & MOUSE_BRUSHING)
    {
        Grid_ProcessBrush();

        if(M.loc == Loc_SmallGrid && !aux_panel->isBlank())
        {
            aux_panel->workPt->basePattern->UpdateScaledImage();
            R(Refresh_GridContent);
        }

        if(M.loc == Loc_MainGrid)
        {
            R(Refresh_GridContent);
        }
    }

    if(M.RMB == true && !(flags & kbd_ctrl) && !(flags & kbd_alt) && !(M.mmode & MOUSE_DELETING) && !(M.mmode & MOUSE_SELECTING))
    {
        Selection_ToggleElements();
        M.delloc = M.loc;
        M.mmode |= MOUSE_DELETING;
        M.mmode &= ~MOUSE_RESIZE; // To avoid cursor pic fixing
    }
}

void Selection_UpdateTicks()
{
    if(M.selloc == Loc_MainGrid || M.selloc == Loc_Other)
    {
        SelTick1 = X2Tick(SelX1 + GridX1, M.selloc);
        SelTick2 = X2Tick(SelX2 + GridX1, M.selloc);
        SelTick2 = Frame2Tick(Tick2Frame(SelTick2) - 1);

        if(M.mmode & MOUSE_LINING)
        {
			pbkMain->SetRanges(Tick2Frame(SelTick1), Tick2Frame(SelTick2));
            pbkMain->SetLooped(true);
        }
    }
    else if(M.selloc == Loc_SmallGrid)
    {
        SelTick1 = X2Tick(SelX1 + GridXS1, Loc_SmallGrid);
        SelTick2 = X2Tick(SelX2 + GridXS1, Loc_SmallGrid);
        SelTick2 = Frame2Tick(Tick2Frame(SelTick2) - 1);

        if(M.mmode & MOUSE_LINING)
        {
            pbkAux->SetRanges(Tick2Frame(SelTick1) + aux_panel->workPt->frame, Tick2Frame(SelTick2) + aux_panel->workPt->frame);
            pbkAux->SetLooped(true);
        }
    }
}

void Selection_UpdateCoords()
{
	if(Sel_Active == true)
	{
		if(M.selloc == Loc_MainGrid || M.selloc == Loc_Other)
		{
			SelX1 = Tick2X(SelTick1, Loc_MainGrid) - GridX1;
			SelX2 = Tick2X(SelTick2, Loc_MainGrid) - GridX1 - 1;

			if(M.selmode == Sel_Fragment)
			{
				pbkMain->SetRanges(Tick2Frame(SelTick1), Tick2Frame(SelTick2));
			}
		}
		else if(M.selloc == Loc_SmallGrid)
		{
			SelX1 = Tick2X(SelTick1, Loc_SmallGrid) - GridXS1;
			SelX2 = Tick2X(SelTick2, Loc_SmallGrid) - GridXS1 - 1;

			if(M.selmode == Sel_Fragment)
			{
				pbkAux->SetRanges(Tick2Frame(SelTick1) + aux_panel->workPt->frame, Tick2Frame(SelTick2) + aux_panel->workPt->frame);
			}
		}
		else if(M.selloc == Loc_StaticGrid)
		{
			SelX1 = Tick2X(SelTick1, Loc_StaticGrid) - GridX1;
			SelX2 = Tick2X(SelTick2, Loc_StaticGrid) - GridX1 - 1;
		}
	}
}

void Selection_Promote(int mouse_x, int mouse_y, unsigned flags)
{
    int k;

    if(M.selloc == Loc_MainGrid)
    {
        if(M.selmode != Sel_Tracks)
            SX2 = mouse_x - GridX1;
        SY2 = mouse_y - GridY1;
    }
    else if(M.selloc == Loc_SmallGrid)
    {
        SX2 = mouse_x - GridXS1;
        SY2 = mouse_y - GridYS1;
    }

    if(SX1 < SX2)
    {
        SelX1 = SX1;
        SelX2 = SX2;
    }
    else
    {
        SelX1 = SX2;
        SelX2 = SX1;
    }

    if(SY1 < SY2)
    {
        SelY1 = SY1;
        SelY2 = SY2;
    }
    else
    {
        SelY1 = SY2;
        SelY2 = SY1;
    }

    if(M.selloc == Loc_MainGrid)
    {
        if(M.selmode != Sel_Tracks)
        {
            if(SelX1 < 0)
            {
                SelX1 = 0;
            }
            if(SelX1 > GridX2 - GridX1)
            {
                SelX1 = GridX2 - GridX1;
            }
            if(SelX2 < 0)
            {
                SelX2 = 0;
            }
            if(SelX2 > GridX2 - GridX1)
            {
                SelX2 = GridX2 - GridX1;
            }
        }

        if(SelY1 < 0)
        {
            SelY1 = 0;
        }
        if(SelY1 > GridY2 - GridY1)
        {
            SelY1 = GridY2 - GridY1;	// - lineHeight/2;
        }
        if(SelY2 < 0)
        {
            SelY2 = 0;
        }
        if(SelY2 > GridY2 - GridY1)
        {
            SelY2 = GridY2 - GridY1;// - lineHeight/2;
        }

        if(M.selmode == Sel_Fragment)
        {
            SelY1 = 0;
            SelY2 = lineHeight*field_pattern->num_lines;
        }

        if(M.selmode == Sel_Pattern)
        {
            k = (SelX1 + RoundFloat(OffsTick*tickWidth))%int(tickWidth);
            SelX1 = SelX1 - k;

            k = (SelX2 + RoundFloat(OffsTick*tickWidth))%int(tickWidth);
            SelX2 = SelX2 - k;
        }
        else if(M.selmode == Sel_Fragment)
        {
            // Align to quantization
            if(M.qsize > 0)
            {
                k = (SelX1 + RoundFloat(OffsTick*tickWidth))%int(tickWidth*M.qsize);
                SelX1 = SelX1 - k;
                k = (SelX2 + RoundFloat(OffsTick*tickWidth))%int(tickWidth*M.qsize);
                SelX2 = SelX2 - k;
            }
        }

        if(M.selmode == Sel_Tracks || M.selmode == Sel_Pattern)
        {
            k = SelY1 % lineHeight;
            if(k > lineHeight/2)
            {
                SelY1 = SelY1 + (lineHeight - k);
            }
            else
            {
                SelY1 = SelY1 - k;
            }

            k = SelY2 % lineHeight;
            if(k > lineHeight/2)
            {
                SelY2 = SelY2 + (lineHeight - k);
            }
            else
            {
                SelY2 = SelY2 - k;
            }
        }
    }
    else if(M.selloc == Loc_SmallGrid)
    {
        if(SelX1 < 0)
        {
            SelX1 = 0;
        }
        if(SelX1 > GridXS2 - GridXS1)
        {
            SelX1 = (GridXS2 - GridXS1);
        }
        if(SelX2 < 0)
        {
            SelX2 = 0;
        }
        if(SelX2 > GridXS2 - GridXS1)
        {
            SelX2 = (GridXS2 - GridXS1);
        }

        if(SelY1 < 0)
        {
            SelY1 = 0;
        }
        if(SelY1 > GridYS2 - GridYS1)
        {
            SelY1 = (GridYS2 - GridYS1);// - Aux->trackH/2;
        }
        if(SelY2 < 0)
        {
            SelY2 = 0;
        }
        if(SelY2 > GridYS2 - GridYS1)
        {
            SelY2 = (GridYS2 - GridYS1);// - Aux->trackH/2;
        }

        if(M.selmode == Sel_Fragment)
        {
            SelY1 = 0;
            SelY2 = lineHeight*aux_panel->workPt->basePattern->num_lines;
        }

        if(M.selmode == Sel_Pattern)
        {
            k = (SelX1 + RoundFloat(aux_panel->OffsTick*aux_panel->tickWidth))%int(aux_panel->tickWidth);
            SelX1 = SelX1 - k;

            k = (SelX2 + RoundFloat(aux_panel->OffsTick*aux_panel->tickWidth))%int(aux_panel->tickWidth);
            SelX2 = SelX2 - k;
        }
        else if(M.selmode == Sel_Fragment)
        {
            // Align to quantization
            if(M.qsize > 0)
            {
                k = (SelX1 + RoundFloat(aux_panel->OffsTick*aux_panel->tickWidth))%int(aux_panel->tickWidth*M.qsize);
                SelX1 = SelX1 - k;
                k = (SelX2 + RoundFloat(aux_panel->OffsTick*aux_panel->tickWidth))%int(aux_panel->tickWidth*M.qsize);
                SelX2 = SelX2 - k;
            }
        }

        if(M.selmode == Sel_Tracks || M.selmode == Sel_Pattern)
        {
            k = SelY1 % aux_panel->lineHeight;
            if(k > aux_panel->lineHeight/2)
            {
                SelY1 = SelY1 + (aux_panel->lineHeight - k);
            }
            else
            {
                SelY1 = SelY1 - k;
            }

            k = SelY2 % aux_panel->lineHeight;
            if(k > aux_panel->lineHeight/2)
            {
                SelY2 = SelY2 + (aux_panel->lineHeight - k);
            }
            else
            {
                SelY2 = SelY2 - k;
            }
        }
    }

    Selection_UpdateTicks();
    SelX2 -= 1; // Digital decrement
    SelY2 -= 1; // Digital decrement
}

void Selection_Drag(int mouse_x, int mouse_y, unsigned flags)
{
    Selection_Promote(mouse_x, mouse_y, flags);
    if(abs(SX2 - SX1) > 4 || abs(SY2 - SY1) > 4)
    {
        M.active_elem = NULL;
    }
    
    bool toggled = false;
//    if(/*M.pickedup == false && */M.selmode != Sel_Tracks)
    {
        toggled = Selection_ToggleElements();
    }
    
    R(Refresh_Selection);
}

void Selection_Stop(unsigned flags)
{
    if(M.selmode == Sel_Tracks)
    {
        //Bunch_Tracks(SelY1, SelY2);

        R(Refresh_Grid);
        if(mixbrowse == false)
        {
            R(Refresh_MixCenter);
        }

        Selection_Reset();
        //Selection_ToggleElements();
    }
    else if(M.selmode != Sel_Fragment)
    {
        Selection_Reset();
    }

    R(Refresh_Selection);
}

void Selection_Start(int mouse_x, int mouse_y, unsigned flags)
{
    // Cleanup previous selection stuff if any
    if(Sel_Active == true)
    {
        Selection_Reset();
    }
    if(Num_Selected > 0 && M.selloc != M.loc)
    {
        Selection_ToggleElements();
    }

    Sel_Active = true;
    M.selstarted = true;
    M.skip_menu = true;
    if(M.mmode & MOUSE_ON_GRID)
    {
        if(flags & kbd_alt && !(flags & kbd_shift) && M.loc == Loc_MainGrid)
        {
            M.selmode = Sel_Tracks;
        }
        else if(flags & kbd_shift)
        {
            M.selmode = Sel_Pattern;
        }
        else
        {
            M.selmode = Sel_Usual;
        }
    
        if(M.loc == Loc_MainGrid)
        {
            SX1 = mouse_x - GridX1;
            SY1 = mouse_y - GridY1;
            M.selloc = Loc_MainGrid;
        }
        else if(M.loc == Loc_SmallGrid)
        {
            SX1 = mouse_x - GridXS1;
            SY1 = mouse_y - GridYS1;
            M.selloc = Loc_SmallGrid;
        }
        else if(M.loc == Loc_StaticGrid)
        {
            SX1 = mouse_x - StX1;
            SY1 = mouse_y - GridY1;
            M.selloc = Loc_StaticGrid;
        }
    }
    else if(M.mmode & MOUSE_LINING)
    {
        M.selloc = M.lineloc;
        M.selmode = Sel_Fragment;
        if(M.selloc == Loc_MainGrid)
        {
            SX1 = mouse_x - GridX1;
            SY1 = 0;
            SY2 = lineHeight*field_pattern->num_lines;
        }
        else
        {
            SX1 = mouse_x - GridXS1;
            SY1 = 0;
            SY2 = aux_panel->lineHeight*aux_panel->workPt->basePattern->num_lines;
        }
    }
    else if(M.mmode & MOUSE_TRACKING)
    {
        SX1 = 0;
        SX2 = 65535;
        tframe lastframe;
        GetLastElementEndFrame(&lastframe);
        float ltick = Frame2Tick(lastframe);
        SX2 = int(ltick*tickWidth);
        if(SX2 < GridX2 - GridX1) SX2 = GridX2 - GridX1;

        SY1 = mouse_y - GridY1;
        M.selloc = Loc_MainGrid;
        M.selmode = Sel_Tracks;
    }
}

void Selection_Reset()
{
	if(Sel_Active == true)
	{
		Sel_Active = false;
		SelX1 = SelX2 = SelY1 = SelY2 = -1;
		if(M.selmode == Sel_Fragment)
		{
			if(M.selloc == Loc_MainGrid)
			{
				if(pbkMain->looped == true)
				{
					pbkMain->ResetLooped();
				}
				pbkMain->AlignRangeToPattern();
			}
			else if(M.selloc == Loc_SmallGrid)
			{
				pbkAux->AlignRangeToPattern();
			}
		}

        // Use direct selection refresh since M.selloc can be changed before common R(Refresh_Selection);
        if(M.selloc == Loc_SmallGrid)
		    R(Refresh_SelAux);
        else if(M.selloc == Loc_MainGrid)
		    R(Refresh_SelMain);
	}
}

// Returns true if changed something
bool Selection_ToggleElements()
{
    Num_Selected = 0;
    bool changed = false;
    bool was;
    Element* el = firstElem;
    while(el != NULL)
    {
        if(el->IsPresent())
        {
            was = el->selected;
            if(M.selmode == Sel_Tracks)
            {
                int line1, line2;
                line1 = Y2Line(SelY1 + GridY1, Loc_MainGrid);
                line2 = Y2Line(SelY2 + GridY1, Loc_MainGrid);
                if(Sel_Active && 
                   ((el->track_line >= line1 && el->track_line <= line2) ||
                    (el->type == El_Pattern && ((Pattern*)el)->is_fat &&
                        el->track_line - 1 >= line1 && el->track_line - 1 <= line2)))
                {
                    el->Select();
                    Num_Selected++;
                    changed = true;
                    MC->listen->repaint(el->abs_area.x1, el->abs_area.y1, el->abs_area.x2 - el->abs_area.x1 + 1,  el->abs_area.y2 - el->abs_area.y1 + 1);
                }
                else if(was)
                {
                    el->Deselect();
                    changed = true;
                    MC->listen->repaint(el->abs_area.x1, el->abs_area.y1, el->abs_area.x2 - el->abs_area.x1 + 1,  el->abs_area.y2 - el->abs_area.y1 + 1);
                }
            }
            else
            {
                if(Sel_Active && (M.mmode & MOUSE_SELECTING) && el->CheckSelected())
                {
                    Num_Selected++;
                    changed = true;
                    MC->listen->repaint(el->abs_area.x1, el->abs_area.y1, el->abs_area.x2 - el->abs_area.x1 + 1,  el->abs_area.y2 - el->abs_area.y1 + 1);
                }
                else if(was)
                {
                    el->Deselect();
                    changed = true;
                    MC->listen->repaint(el->abs_area.x1, el->abs_area.y1, el->abs_area.x2 - el->abs_area.x1 + 1,  el->abs_area.y2 - el->abs_area.y1 + 1);
                }
            }
        }
        el = el->next;
    }

    if(changed == true)
    {
        if(M.selloc == Loc_MainGrid)
        {
            R(Refresh_GridContent);
            if(aux_panel->auxmode == AuxMode_Vols || 
               aux_panel->auxmode == AuxMode_Pans)
            {
                R(Refresh_Aux);
            }
        }
        else if(M.selloc == Loc_SmallGrid)
        {
            R(Refresh_AuxContent);
            R(Refresh_SubAux);
        }
    }

    return changed;
}

void Selection_Check(int mouse_x, int mouse_y, unsigned flags)
{
    // Check for start of selection
    if(M.selstarted == false && JustMoved == false && 
        !(M.pickedup || M.mmode & MOUSE_SELECTING || M.mmode & MOUSE_DELETING || M.mmode & MOUSE_RESIZE))
    {
        if(M.mmode & MOUSE_ON_GRID)
        {
            if(!(flags & kbd_shift || flags & kbd_alt) && (M.RMB == true || M.LMB == true) && 
               !(M.mmode & MOUSE_ENVELOPING) && 
               !(M.mmode & MOUSE_LANEAUXING) &&
               !(M.mmode & MOUSE_GRID_Y2) &&
               !(M.mmode & MOUSE_AUXING && aux_panel->isVolsPansMode()) &&
                 ((M.loc == Loc_MainGrid && !(M.mmode & MOUSE_AUXING)) || (flags & kbd_ctrl))) // On small grid the selection is only via control
            {
                skip_input = false;
                M.PosUpdate(mouse_x, mouse_y, flags);
                Selection_Start(mouse_x, mouse_y, flags);
            }
        }
        else if (M.mmode & MOUSE_LINING)
        {
            /*
            if(M.RMB == true)
            {
                skip_input = false;
                M.PosUpdate(mouse_x, mouse_y, flags);
                Selection_Start(mouse_x, mouse_y, flags);
            }
            */
        }
        else if(M.mmode & MOUSE_TRACKING)
        {
            if(M.RMB == true)
            {
                skip_input = false;
                M.PosUpdate(mouse_x, mouse_y, flags);
                Selection_Start(mouse_x, mouse_y, flags);
            }
        }
    }
}

void Looping_UpdateCoords()
{
	if(Looping_Active == true)
	{
		if(M.looloc == Loc_MainGrid || M.looloc == Loc_Other)
		{
			LooX1 = Tick2X(LooTick1, Loc_MainGrid) - GridX1;
			LooX2 = Tick2X(LooTick2, Loc_MainGrid) - GridX1 - 1;

			pbkMain->SetRanges(Tick2Frame(LooTick1), Tick2Frame(LooTick2));
		}
		else if(M.looloc == Loc_SmallGrid)
		{
			LooX1 = Tick2X(LooTick1, Loc_SmallGrid) - GridXS1;
			LooX2 = Tick2X(LooTick2, Loc_SmallGrid) - GridXS1 - 1;

			pbkAux->SetRanges(Tick2Frame(LooTick1) + aux_panel->workPt->frame, Tick2Frame(LooTick2) + aux_panel->workPt->frame);
		}
		else if(M.looloc == Loc_StaticGrid)
		{
			LooX1 = Tick2X(LooTick1, Loc_StaticGrid) - GridX1;
			LooX2 = Tick2X(LooTick2, Loc_StaticGrid) - GridX1 - 1;
		}
	}
}

void Looping_UpdateTicks()
{
    if(M.looloc == Loc_MainGrid || M.looloc == Loc_Other)
    {
        LooTick1 = X2Tick(LooX1 + GridX1, Loc_MainGrid);
        LooTick2 = X2Tick(LooX2 + GridX1, Loc_MainGrid);
        //LooTick2 = Frame2Tick(Tick2Frame(LooTick2) - 1);

        if(LooTick1 != LooTick2 && M.mmode & MOUSE_LINING)
        {
            pbkMain->SetRanges(Tick2Frame(LooTick1), Tick2Frame(LooTick2));
            pbkMain->SetLooped(true);
        }
        else
        {
            pbkMain->SetLooped(false);
        }
    }
    else if(M.looloc == Loc_SmallGrid)
    {
        LooTick1 = X2Tick(LooX1 + GridXS1, Loc_SmallGrid);
        LooTick2 = X2Tick(LooX2 + GridXS1, Loc_SmallGrid);
        //LooTick2 = Frame2Tick(Tick2Frame(LooTick2) - 1);

        if(LooTick1 != LooTick2 && M.mmode & MOUSE_LINING)
        {
            pbkAux->SetRanges(aux_panel->workPt->frame + Tick2Frame(LooTick1), aux_panel->workPt->frame + Tick2Frame(LooTick2));
            pbkAux->SetLooped(true);
        }
        else
        {
            pbkAux->SetLooped(false);
        }
    }
}

void Looping_Drag(int mouse_x, int mouse_y, unsigned flags)
{
    int k;

    if(M.looloc == Loc_MainGrid)
    {
        LX2 = mouse_x - GridX1;
    }
    else if(M.looloc == Loc_SmallGrid)
    {
        LX2 = mouse_x - GridXS1;
    }

    if(LX1 < LX2)
    {
        LooX1 = LX1;
        LooX2 = LX2;
    }
    else
    {
        LooX1 = LX2;
        LooX2 = LX1;
    }

    if(M.looloc == Loc_MainGrid)
    {
        if(LooX1 < 0)
        {
            LooX1 = 0;
        }
        if(LooX1 > GridX2 - GridX1)
        {
            LooX1 = GridX2 - GridX1;
        }
        if(LooX2 < 0)
        {
            LooX2 = 0;
        }
        if(LooX2 > GridX2 - GridX1)
        {
            LooX2 = GridX2 - GridX1;
        }


        // Align to quantization
        if(quantsize > 0)
        {
            k = (LooX1 + RoundFloat(OffsTick*tickWidth))%int(tickWidth*quantsize);
            LooX1 = LooX1 - k;
            k = (LooX2 + RoundFloat(OffsTick*tickWidth))%int(tickWidth*quantsize);
            LooX2 = LooX2 - k;
        }

    }
    else if(M.looloc == Loc_SmallGrid)
    {
        if(LooX1 < 0)
        {
            LooX1 = 0;
        }
        if(LooX1 > GridXS2 - GridXS1)
        {
            LooX1 = (GridXS2 - GridXS1);
        }
        if(LooX2 < 0)
        {
            LooX2 = 0;
        }
        if(LooX2 > GridXS2 - GridXS1)
        {
            LooX2 = (GridXS2 - GridXS1);
        }

        // Align to quantization
        if(aux_panel->quantsize > 0)
        {
            k = (LooX1 + RoundFloat(aux_panel->OffsTick*aux_panel->tickWidth))%int(aux_panel->tickWidth*aux_panel->quantsize);
            LooX1 = LooX1 - k;
            k = (LooX2 + RoundFloat(aux_panel->OffsTick*aux_panel->tickWidth))%int(aux_panel->tickWidth*aux_panel->quantsize);
            LooX2 = LooX2 - k;
        }

    }

    Looping_UpdateTicks();
    LooX2 -= 1; // Digital decrement

    R(Refresh_Selection);
}

void Looping_Reset()
{
	if(Looping_Active == true)
	{
		Looping_Active = false;
		LooX1 = LooX2 = -1;
		if(M.looloc == Loc_MainGrid)
		{
			if(pbkMain->looped == true)
			{
				pbkMain->ResetLooped();
			}
			pbkMain->AlignRangeToPattern();
		}
		else if(M.looloc == Loc_SmallGrid)
		{
			pbkAux->AlignRangeToPattern();
		}

        // Use direct selection refresh since M.selloc can be changed before common R(Refresh_Selection);
        if(M.looloc == Loc_SmallGrid)
		    R(Refresh_SelAux);
        else if(M.looloc == Loc_MainGrid)
		    R(Refresh_SelMain);
	}
}


void Looping_Check(int mouse_x, int mouse_y, unsigned flags)
{
    // Check for start of selection
    if(!(M.mmode & MOUSE_SELECTING || M.mmode & MOUSE_DELETING || 
        M.mmode & MOUSE_RESIZE || M.mmode & MOUSE_LOOPING))
    {
        if(M.mmode & MOUSE_LINING && M.RMB == true)
        {
            Looping_Reset();

            skip_input = false;
            M.PosUpdate(mouse_x, mouse_y, flags);

            Looping_Active = true;
            M.loostarted = true;
            if(M.lineloc == Loc_MainGrid)
            {
                LX1 = mouse_x - GridX1;
                M.looloc = Loc_MainGrid;
            }
            else
            {
                LX1 = mouse_x - GridXS1;
                M.looloc = Loc_SmallGrid;
            }
        }
    }
}

void UpdateQuants()
{
    quantsize = MapItem2Quant(CP->qMenu->main_item->itemtype);
    aux_panel->quantsize = MapItem2Quant(aux_panel->qMenu->main_item->itemtype);
}

bool Menu_MouseClick(int mouse_x, int mouse_y)
{
    bool skip = false;
    if(M.quanting == true)
    {
        if(M.active_ctrl != NULL)
        {
            MenuItem* mi = (MenuItem*)M.active_ctrl;
            if(M.active_ctrl->type == Ctrl_MenuItem)
            {
                M.active_menu->SetMainItem(((MenuItem*)M.active_ctrl)->itemtype);
                float qsize = MapItem2Quant(((MenuItem*)M.active_ctrl)->itemtype);
                if(M.active_menu == CP->qMenu)
                {
                    quantsize = qsize;
                    R(Refresh_Grid);
                }
                else
                {
                    aux_panel->quantsize = qsize;
                    R(Refresh_Aux);
                }
                M.quanting = false;
                M.menu_active = false;
                M.active_ctrl = NULL;
        
                MenuItem* mi = M.active_menu->first_item;
                while(mi != NULL)
                {
                    mi->active = false;
                    mi = mi->inext;
                }
                M.active_menu = NULL;
                M.mmode &= ~MOUSE_CONTROLLING;
            }
        }
    }
    else if(M.contextmenu != NULL)
    {
        if(M.mmode & MOUSE_CONTROLLING && M.active_ctrl->type == Ctrl_MenuItem)
        {
            MenuItem* mi = (MenuItem*)M.active_ctrl;
            if(mi->doable == true)
            {
                ActivateMenuItem(mi, mi->itemtype, M.contextmenu);
                skip_input = true;
                M.LMB = false;
            }
            else
            {
                return skip;
            }
        }

        M.DeleteContextMenu();
        post_menu_update = true;
    }

    return skip;
}

void Process_AutoAdvance()
{
    float qsize = C.qsize;

    if(auto_advance)
    {
        C.ExitToCurrentMode();
        CTick += 1.0f*qsize;
        if(!(C.loc == Loc_SmallGrid && aux_panel->workPt->ptype == Patt_StepSeq))
        {
            CLine++;
        }
        C.PosUpdate();
        CheckStepSeqPos();
    }
}

void ResizeSelectedElems(float dtick, Element* excludeel, Loc loc, unsigned flags)
{
    Element* el = firstElem;
    while(el != NULL)
    {
        if(el->IsPresent() && el->selected == true && el != excludeel && el->patt == excludeel->patt)
        {
            if(M.edge == RIGHT_X)
            {
                int t1 = 0;
                int t2 = 0;

                if(el->type == El_Samplent)
    			{
                    if(el->type == El_Samplent)
                    {
                        t1 = (el->IsResizeTouched() == false) ? 0 : 1;
                    }

                    if(flags & kbd_alt)
                    {
                        // touchresized data for undo
                        t2 = 0;

                        el->ResizeTouch(false);
                       ((NoteInstance*)el)->instr->ResizeTouch(false);
                        //newlen = 0;
                    }
                    else
                    {
                        // touchresized data for undo
                        t2 = 1;

                        el->ResizeTouch(true);
                       ((NoteInstance*)el)->instr->ResizeTouch(true);
                    }
    			}
    //            else
    //            if(el->type != El_Samplent)
    			{
                    if(el->end_tick + dtick > el->start_tick)
                    {
                        el->end_tick += dtick;
                    }
                    else
                    {
                        if(loc == Loc_MainGrid)
                        {
                            el->end_tick = el->start_tick + quantsize;
                        }
                        else if(loc == Loc_SmallGrid)
                        {
                            el->end_tick = el->start_tick + aux_panel->quantsize;
                        }
                    }
                }
    			if(el->end_tick - el->start_tick != el->tick_length)
    			{
    				undoMan->DoNewAction(Action_Resize, (void*)el, el->tick_length, el->end_tick - el->start_tick, t1, t2);
    			}

                if(el->type == El_Pattern)
                {
    			    Pattern* pt = (Pattern*)el;
                    pt->basePattern->UpdateScaledImage();
                    pt->ResizeTouch(true);
    			}
            }
            else if(M.edge == TOP_Y)
            {
    			if(el->type == El_Pattern)
    			{
    			    Pattern* pt = (Pattern*)el;
                    bool oldfat = pt->is_fat;
                    if(dtick == 0)
                    {
                        pt->is_fat = false;
                    }
                    else if(dtick == 1)
                    {
                        pt->is_fat = true;
                    }
                }
            }
        }
        el = el->next;
    }
}

void Process_Resize(int mouse_x, int mouse_y, unsigned flags, Loc loc)
{
    if(M.resize_object == Resize_Lane)
    {
        if(M.patt == field_pattern || (M.patt != field_pattern && M.current_trk != NULL))
        {
            if(M.active_lane == Lane_Vol)
            {
                int h;
                int dt = M.current_trk->vol_lane.height;
                if(loc == Loc_SmallGrid)
                {
                    h = Y2Line(mouse_y, Loc_SmallGrid) - M.current_trk->vol_lane.base_line;
                }
                else
                {
                    h = Y2Line(mouse_y, Loc_MainGrid) - M.current_trk->vol_lane.base_line - M.patt->track_line;
                }
                if(h > 1)
                {
                    M.current_trk->vol_lane.height = h;
                    dt = h - dt;
                    M.current_trk->end_line += dt;
                    M.current_trk->UpdateLaneBases();
                    M.patt->track_line_end += dt;
                    M.patt->num_lines += dt;
                    AdvanceTracks(M.current_trk->next, dt, M.patt);
                    if(C.trknum > M.current_trk->trknum)
                    {
                        CLine += dt;
                    }
                }
            }
            else if(M.active_lane == Lane_Pan)
            {
                int dt = M.current_trk->pan_lane.height;
                int h;
                if(loc == Loc_SmallGrid)
                {
                    h = Y2Line(mouse_y, Loc_SmallGrid) - M.current_trk->pan_lane.base_line;
                }
                else
                {
                    h = Y2Line(mouse_y, Loc_MainGrid) - M.current_trk->pan_lane.base_line - M.patt->track_line;
                }

                if(h > 1)
                {
                    M.current_trk->pan_lane.height = h;
                    dt = h - dt;
                    M.current_trk->end_line += dt;
                    M.current_trk->UpdateLaneBases();
                    M.patt->track_line_end += dt;
                    M.patt->num_lines += dt;
                    AdvanceTracks(M.current_trk->next, dt, M.patt);
                    if(C.trknum > M.current_trk->trknum)
                    {
                        CLine += dt;
                    }
                }
            }
        }
        else // for patterns on grid
        {
            if(M.active_lane == Lane_Vol)
            {
                int dt = M.patt->vol_lane_main.height;
                int h = Y2Line(mouse_y, Loc_MainGrid) - M.patt->track_line - M.patt->num_lines;
                if(h > 1)
                {
                    M.patt->vol_lane_main.height = h;
                    dt = h - dt;
                    M.patt->track_line_end += dt;
					//M.pt->num_lines += dt;
                    M.patt->UpdateLaneBases();
                }
                if(C.trknum > M.current_trk->trknum)
                {
                    CLine += dt;
                }
            }
            else if(M.active_lane == Lane_Pan)
            {
                int dt = M.patt->pan_lane_main.height;
				int h = Y2Line(mouse_y, Loc_MainGrid) - M.patt->track_line - M.patt->num_lines - (M.patt->vol_lane_main.visible == true ? M.patt->vol_lane_main.height : 0);
                if(h > 1)
                {
                    M.patt->pan_lane_main.height = h;
                    dt = h - dt;
                    M.patt->track_line_end += dt;
					//M.pt->num_lines += dt;
                    M.patt->UpdateLaneBases();

                    if(C.trknum > M.current_trk->trknum)
                    {
                        CLine += dt;
                    }
                }
            }
        }
        C.PosUpdate();
    }
    else if(M.resize_object == Resize_GridElem)
    {
        if(M.edge == RIGHT_X && M.loc == M.resize_loc)
        {
            int t1 = 0;
            int t2 = 0;
            Element* el = M.resize_elem;
            float ptick;
            if(loc == Loc_SmallGrid)
            {
                ptick = X2Tick(mouse_x, Loc_SmallGrid);
            }
            else
            {
                ptick = X2Tick(mouse_x, Loc_MainGrid);
            }

            float newlen;
            if(el->type == El_Samplent)
            {
                t1 = (el->IsResizeTouched() == false) ? 0 : 1;

                if(flags & kbd_alt)
                {
                    t2 = 0;

                    el->ResizeTouch(false);
                   ((NoteInstance*)el)->instr->ResizeTouch(false);
                    newlen = 0;
                }
                else
                {
                    t2 = 1;

                    if(el->IsResizeTouched() == false)
                    {
                        el->ResizeTouch(true);
                       ((NoteInstance*)el)->instr->ResizeTouch(true);
                        el->tick_length = M.snappedTick - el->start_tick;
                        el->end_tick = el->start_tick + el->tick_length;
                    }
                }
            }

            {
                // Resizing method kindly proposed by Didier Dambrin :)
                float diff = ptick - el->end_tick;
                float dtick;
                if(M.qsize > 0)
                {
                    dtick = int((diff + (diff >= 0 ? 0.5f*M.qsize : -0.5f*M.qsize))/M.qsize)*M.qsize;
                }
                else
                {
                    dtick = M.snappedTick - el->end_tick;
                }
                newlen = el->end_tick - el->start_tick + dtick;
            }

            if(M.qsize > 0 && newlen < M.qsize)
            {
                newlen = M.qsize;
            }
            else if(M.qsize < 0 && newlen < 0.1f)
            {
                newlen = 0.1f;
            }

            float dlen = newlen - el->tick_length;

            if(el->tick_length != newlen)
            {
                undoMan->DoNewAction(Action_Resize, (void*)el, el->tick_length, newlen, t1, t2);

    			if(el->IsInstance())
    			{
    				NoteInstance* ii = (NoteInstance*)el;
    				ii->instr->SetLastLength(el->tick_length);
                    if(!(current_instr == ii->instr))
                    {
                        ChangeCurrentInstrument(ii->instr);
                    }
    			}
    			else if(el->type == El_SlideNote)
    			{
    				C.lastslidelength = el->tick_length;
    			}

    			if(el->type == El_Pattern)
    			{
    			    Pattern* pt = (Pattern*)el;
                    pt->basePattern->UpdateScaledImage();
                    pt->ResizeTouch(true);
    			}

                ResizeSelectedElems(dlen, el, M.loc, flags);

                // Do rescan bounds after every element is resized
                if(loc == Loc_SmallGrid)
                {
                    aux_panel->RescanPatternBounds();
                }

                M.movetick = M.snappedTick;
            }
        }
        else if(M.edge == TOP_Y)
        {
            Element* el = M.resize_elem;
			if(el->type == El_Pattern)
			{
			    Pattern* pt = (Pattern*)el;
                bool oldfat = pt->is_fat;
                if((mouse_y - el->abs_area.y1) > 7)
                {
                    pt->is_fat = false;
                    MakePatternsFat = false;
                }
                else if((mouse_y - el->abs_area.y1) < -7 && pt->track_line > 0)
                {
                    pt->is_fat = true;
                    MakePatternsFat = true;
                }

                if(pt->is_fat != oldfat)
                {
                    float dt = pt->is_fat ? 1.f : 0.f;
                    ResizeSelectedElems(dt, el, M.loc, flags);

                    ChangesIndicate();

					//pt->OrigPt->UpdateScaledImage();
                    R(Refresh_GridContent);
                }
            }
        }
    }
    else if(M.resize_object == Resize_AuxPattRange)
    {
        Pattern* pt = aux_panel->workPt;
        float ptick = X2Tick(mouse_x, Loc_SmallGrid);
        float oldtick = pt->tick_length;
        float newtick = oldtick;
        if(aux_panel->quantsize > 0)
            newtick += int((ptick - newtick)/aux_panel->quantsize)*aux_panel->quantsize;
        else
            newtick += (ptick - newtick);
        if(newtick <= 0)
        {
            newtick = aux_panel->quantsize > 0 ? aux_panel->quantsize : 0.01f;
        }
        else if(newtick > aux_panel->h_sbar->visible_len + aux_panel->h_sbar->offset)
        {
            newtick = aux_panel->h_sbar->visible_len + aux_panel->h_sbar->offset;
        }

        undoMan->DoNewAction(Action_Resize, (void*)pt, oldtick, newtick, 0, 0);

        pt->ResizeTouch(true);
        pt->Update();
        pt->basePattern->UpdateScaledImage();

        // it is done in undo manager
        //    pbAux->RangesToPattern();
        
        R(Refresh_GridContent);
    }
    else if(M.resize_object == Resize_Envelope)
    {
        if(M.edge == BOTTOM_Y)
        {
            int phgt = mouse_y - M.active_env->y;
            if(phgt < 8)
                phgt = 8;
            M.active_env->hgt = phgt;
        }
        else if(M.edge == RIGHT_X)
        {
            int plen = mouse_x - M.active_env->x;
            float len = float(RoundFloat(plen/M.active_env->scale));
            if(len < ticks_per_beat)
                len = float(ticks_per_beat);
            M.active_env->SetLength(len);

            if(M.loc == Loc_SmallGrid)
            {
                aux_panel->RescanPatternBounds();
            }
        }

        ChangesIndicate();
    }
    else if(M.resize_object == Resize_Waveform)
    {
        M.active_samplent->wave_height_pix = M.active_samplent->y - mouse_y;
        if(M.active_samplent->wave_height_pix < lineHeight)
        {
            M.active_samplent->wave_height_pix = lineHeight;
        }
    }
}

void Lining_ProcessWheel(Loc loc, int delta)
{
    ResetUnstableElements();
    if(loc == Loc_MainGrid)
    {
        currPlayX_f += delta;
        if(currPlayX_f < 0)
        {
            currPlayX_f = 0;
        }

        currPlayX = RoundDouble(currPlayX_f);
        pbkMain->SetCurrFrame((long)(currPlayX_f*framesPerPixel));
        UpdateTime(Loc_MainGrid);
        AuxPos2MainPos();
    }
    else if(loc == Loc_SmallGrid && aux_panel->workPt != aux_panel->blankPt)
    {
        aux_panel->curr_play_x_f += delta;
        if(aux_panel->curr_play_x_f < 0)
        {
            aux_panel->curr_play_x_f = 0;
        }

        aux_panel->curr_play_x = RoundDouble(aux_panel->curr_play_x_f);
        pbkAux->SetCurrFrame((long)(aux_panel->curr_play_x_f*aux_panel->frames_per_pixel) + aux_panel->workPt->frame);
        UpdateTime(Loc_SmallGrid);
        MainPos2AuxPos();
    }
}

void SetPosX(double newX, Loc loc)
{
    if(loc == Loc_MainGrid)
    {
        currPlayX_f = (double)newX + (float)(OffsTick*tickWidth);
        currPlayX = int(currPlayX_f);
        pbkMain->SetCurrFrame((long)(currPlayX_f*framesPerPixel));
        UpdateTime(Loc_MainGrid);
        AuxPos2MainPos();

        PreInitEnvelopes(pbkMain->currFrame, field_pattern, field_pattern->first_ev, Playing);

        if(Playing)
            PreInitSamples(pbkMain->currFrame, field_pattern, field_pattern->first_ev);

        // Switch to main playback if set pos outside the playing pattern
        if(aux_panel->playing && 
            (pbkMain->currFrame < pbkAux->rng_start_frame || pbkMain->currFrame > pbkAux->rng_end_frame))
        {
            CP->HandleButtDown(CP->play_butt);
        }
    }
    else if(loc == Loc_SmallGrid)
    {
        aux_panel->curr_play_x_f = (float)newX + (float)(aux_panel->OffsTick*aux_panel->tickWidth);
        aux_panel->curr_play_x = int(aux_panel->curr_play_x_f);
        pbkAux->SetCurrFrame((long)((aux_panel->curr_play_x*aux_panel->frames_per_pixel) + aux_panel->workPt->frame));
        UpdateTime(Loc_SmallGrid);
        MainPos2AuxPos();

        if(pbkAux->playPatt->autopatt == false)
        {
            if(Playing)
                PreInitEnvelopes(pbkMain->currFrame, field_pattern, field_pattern->first_ev, Playing, false);
            else
                PreInitEnvelopes(pbkMain->currFrame, pbkAux->playPatt, field_pattern->first_ev, aux_panel->playing, false);

            if(Playing)
                PreInitSamples(pbkMain->currFrame, field_pattern, field_pattern->first_ev);
            else if(aux_panel->playing)
                PreInitSamples(pbkMain->currFrame, pbkAux->playPatt, field_pattern->first_ev);
        }
        else
        {
            PreInitEnvelopes(pbkAux->currFrame, pbkAux->playPatt, pbkAux->playPatt->first_ev, aux_panel->playing, false);

            if(aux_panel->playing)
                PreInitSamples(pbkAux->currFrame, pbkAux->playPatt, pbkAux->playPatt->first_ev);
        }
    }
}

void Lining_ProcessMouse(Loc loc, int mouse_x)
{
    // Prevent lining when border resize is in process
    if(!(M.mmode & MOUSE_GRID_X1 || M.mmode & MOUSE_GRID_X2 || M.mmode & MOUSE_GRID_Y1 || M.mmode & MOUSE_GRID_Y2))
    {
        ResetUnstableElements();
        if(loc == Loc_MainGrid)
        {
            int newX = mouse_x + 1 - GridX1;
            if(newX > (GridX2 - GridX1))
            {
                newX = GridX2 - GridX1;
            }
            else if(newX < 0)
            {
                newX = 0;
            }

            SetPosX((double)newX, loc);
        }
        else if(loc == Loc_SmallGrid)
        {
            if(aux_panel->workPt != aux_panel->blankPt)
            {
                int newX = mouse_x + 1 - GridXS1;
                if(newX > (GridXS2 - GridXS1))
                {
                    newX = GridXS2 - GridXS1;
                }
                else if(newX < 0)
                {
                    newX = 0;
                }

                SetPosX((double)newX, loc);
            }
        }

        if(MC->poso->isTimerRunning())
        {
            MC->poso->stopTimer();
        }
    }
}

void MuteSelectedPatterns()
{
    Pattern* pt;
    if(Num_Selected > 0)
    {
        Element* el = firstElem;
        while(el != NULL)
        {
            if(el->IsPresent() && el->selected == true && el->type == El_Pattern)
            {
                pt = (Pattern*)el;
                pt->muted = !pt->muted;
            }
            el = el->next;
        }

        R(Refresh_GridContent);
    }
    else if(M.active_elem != NULL && M.active_elem->type == El_Pattern)
    {
        pt = (Pattern*)M.active_elem;
        pt->muted = !pt->muted;

        R(Refresh_GridContent);
    }
}

void MakePatternUnique(Pattern* pt)
{
    if(!pt->isTheOnlyPresentChild()) // If it's the only child present, then it's already unique
    {
        Pattern* npt = pt->Copy(true);
        npt->basePattern->UpdateScaledImage();
        DeleteElement(pt, false, false);
    }
}

void UniqueSelectedPatterns()
{
    Pattern* pt;
    if(Num_Selected > 0)
    {
        Element* el = firstElem;
        while(el != NULL)
        {
            if(el->IsPresent() && el->selected == true && el->type == El_Pattern)
            {
                pt = (Pattern*)el;
                MakePatternUnique(pt);
            }
            el = el->next;
        }

        R(Refresh_GridContent);
    }
    else if(M.active_elem != NULL && M.active_elem->type == El_Pattern)
    {
        pt = (Pattern*)M.active_elem;
        MakePatternUnique(pt);

        R(Refresh_GridContent);
    }
}

void CreateContextMenu(int mouse_x, int mouse_y)
{
    PopupMenu m;
    /*
    m.addItem (1, T("Normal item"));
    m.addItem (2, T("Disabled item"), false);
    m.addItem (3, T("Ticked item"), true, true);
    m.addColouredItem (4, T("Coloured item"), Colours::green);
    m.addSeparator();
    
    PopupMenu tabsMenu;
    tabsMenu.addItem (1001, T("Show tabs at the top"), true, NULL);
    tabsMenu.addItem (1002, T("Show tabs at the bottom"), true, NULL);
    tabsMenu.addItem (1003, T("Show tabs at the left"), true, NULL);
    tabsMenu.addItem (1004, T("Show tabs at the right"), true, NULL);
    m.addSubMenu (T("Tab position"), tabsMenu);
    
    int result = m.showAt(155, 155, 0, 155, 0, 20);
    */
    bool retval = false;
    Menu* menu = NULL;
    if(M.mmode == MOUSE_TRACKING || M.mmode == MOUSE_INSTRUMENTING)
    {
        menu = new Menu(mouse_x, mouse_y, Menu_Context);
        menu->tick = M.snappedTick;
        menu->line = M.snappedLine;
        /*
        if(M.mmode == MOUSE_TRACKING)
        {
            menu->trknum = M.current_track_num;
            menu->trk = GetTrkDataForTrkNum(menu->trknum, field);
    
            if(menu->trk->vol_lane.visible == false)
            {
                m.addItem(MItem_ShowVolumeLane, T("Show volume lane"));
            }
            else
            {
                m.addItem(MItem_HideVolumeLane, T("Hide volume lane"));
            }
    
            if(menu->trk->pan_lane.visible == false)
            {
                m.addItem(MItem_ShowPanLane, T("Show panning lane"));
            }
            else
            {
                m.addItem(MItem_HidePanLane, T("Hide panning lane"));
            }
        }
        else */
        if(M.mmode == MOUSE_INSTRUMENTING)
        {
            menu->instr = M.active_instr;
            if(M.active_instr != NULL)
            {
                m.addItem(MItem_CloneInstrument, T("Clone instrument"));
                m.addItem(MItem_DeleteInstrument, T("Delete instrument"));
                m.addItem(MItem_CreatePianorollPatternForInstrument, T("Create pianoroll at cursor pos."), C.patt == field_pattern);
                m.addItem(MItem_BindInstrumentToCurrentPattern, T("Bind to current pattern"), (!aux_panel->isBlank() && aux_panel->workPt->ptype == Patt_Pianoroll));
                //m.addSeparator();
                if(M.active_instr->subtype != ModSubtype_Sample)
                {
                    m.addItem(MItem_LoadPreset, T("View presets"));
                    m.addItem(MItem_ViewParams, T("View params"));
                }
                m.addItem(MItem_EditAutoPattern, T("Edit autopattern"));
            }
        }
    }
    else if(M.mmode & MOUSE_ON_GRID)
    {
        /* Grid menu cancelled
        if(!(M.mmode & MOUSE_ENVELOPING) && !(M.loc == Loc_SmallGrid && gAux->workPt->ptype == Patt_StepSeq))
        {
            menu = new Menu(mouse_x, mouse_y, Menu_Context);
            menu->tick = M.mousetick;
            menu->line = M.mouseline;

            if(Num_Selected > 0)
            {
                m.addItem (MItem_Cut, T("Cut"));
                m.addItem (MItem_Copy, T("Copy"));
            }

            Element* el = firstElem;
            while(el != NULL)
            {
                if(el->is_copied || el->is_cut)
                {
                    m.addItem (MItem_Paste, T("Paste"));
                    break;
                }
                el = el->next;
            }

            m.addSeparator();
            m.addItem (MItem_SelectAll, T("Select all"));
            if(M.track_bunch != NULL)
            {
                m.addItem (MItem_Unbunch, T("Unbunch tracks"));
            }
        }
        */
    }
    else if(M.mmode & MOUSE_AUXMIXING && M.active_mixchannel != NULL && 
         !(M.mmode & MOUSE_DRAG_N_DROPPING) && M.active_ctrl == NULL)
    {
        menu = new Menu(mouse_x, mouse_y, Menu_Context);
        menu->curreff = M.active_effect;
        menu->mchan = M.active_mixchannel;

        //m.addItem(MItem_SetMixCellToGain, T("Add Gain"));
        //m.addItem(MItem_SetMixCellToSend, T("Add Send"));
        if(M.active_effect != NULL)
        {
            menu->curreff = M.active_effect;
            m.addItem(MItem_LoadPreset, T("Load preset"));
            m.addItem(MItem_SavePreset, T("Save preset"));
            m.addItem(MItem_ViewParams, T("View params"));
            m.addItem(MItem_DeleteEffect, T("Delete effect"));

            PopupMenu mex;
            PopupMenu mexvst;
            int eindex = 100;
            ModListEntry *pEntry = pModulesList->pFirst;
            while (pEntry != NULL)
            {
                if(pEntry->modtype == ModuleType_Effect)
                {
                    if(pEntry->subtype != ModSubtype_VSTPlugin)
                        mex.addItem(eindex, pEntry->name);
                    else
                        mexvst.addItem(eindex, pEntry->name);

                    eindex++;
                }
                pEntry = pEntry->Next;
            }

            mex.addSubMenu(T("External plugins"), mexvst);

            m.addSubMenu(T("Replace"), mex);

            m.addSeparator();
        }
        else if(M.active_effect == NULL)
        {
            m.addItem(MItem_SetMixCellToEQ1, T("Add 1-band equalizer"));
            m.addItem(MItem_SetMixCellToEQ3, T("Add 3-band equalizer"));
            m.addItem(MItem_SetMixCellToGraphicEQ, T("Add Graphic equalizer"));
            m.addItem(MItem_SetMixCellToFilter, T("Add Filter"));
            m.addItem(MItem_SetMixCellToCompressor, T("Add Compressor"));
            m.addItem(MItem_SetMixCellToReverb, T("Add Reverb"));
            m.addItem(MItem_SetMixCellToChorus, T("Add Chorus"));
            m.addItem(MItem_SetMixCellToFlanger, T("Add Flanger"));
            m.addItem(MItem_SetMixCellToPhaser, T("Add Phaser"));
            m.addItem(MItem_SetMixCellToWahWah, T("Add WahWah"));
            m.addItem(MItem_SetMixCellToDistortion, T("Add Distortion"));
            m.addItem(MItem_SetMixCellToBitCrusher, T("Add BitCrusher"));
            m.addItem(MItem_SetMixCellToDelay, T("Add Delay"));

            PopupMenu mex;
            int eindex = 100;
            ModListEntry *pEntry = pModulesList->pFirst;
            while (pEntry != NULL)
            {
                if(pEntry->modtype == ModuleType_Effect)
                {
                    if(pEntry->subtype == ModSubtype_VSTPlugin)
                        mex.addItem(eindex, pEntry->name);
                    eindex++;
                }
                pEntry = pEntry->Next;
            }
            m.addSubMenu(T("External plugins"), mex);
            //m.addItem(MItem_SetMixCellToStereoizer, T("Add Stereoizer"));
            //m.addItem(MItem_SetMixCellToFilter2, T("Add Filter2"));
            //m.addItem(MItem_SetMixCellToFilter3, T("Add Filter3"));
        }
    }
    else if(M.mmode & MOUSE_BROWSING)
    {
        Browser* brw = M.btype == Brw_Generators ? genBrw : mixBrw;
        if(brw->brwmode == Browse_Plugins)
        {
            menu = new Menu(mouse_x, mouse_y, Menu_Context);

            m.addItem(MItem_ReScanFast, T("Fast rescan"));
            m.addItem(MItem_ReScanFull, T("Full rescan (unsafe)"));
        }
        else if(brw->brwmode == Browse_Samples)
        {
		    brw->MouseClick(M.brwindex, false, true, false);
            MC->ForceRefresh(); // Force browser refresh to indicate entry

            menu = new Menu(mouse_x, mouse_y, Menu_Context);
            menu->browser = brw;
            FileData* fd = brw->GetFileDataByIndex(M.brwindex);
            menu->activefiledata = fd;

            m.addItem(MItem_LoadAsInstrument, T("Load as instrument"));
        }
        else if(brw->brwmode == Browse_Presets && brw->entry_first != NULL)
        {
			PresetData* prd = brw->GetPresetDataByIndex(M.brwindex);
			if(prd != NULL)
			{
				ParamModule* module = (ParamModule*)(prd->pPreset->owner);
				if(module->internalModule == true)
				{
					menu = new Menu(mouse_x, mouse_y, Menu_Context);
                    menu->browser = brw;
					menu->module = module;
					menu->activepresetdata = prd;

					m.addItem(MItem_DeletePreset, T("Delete preset"));
				}
			}
        }
        else if(brw->brwmode == Browse_Projects && brw->entry_first != NULL)
        {
			FileData* fd = brw->GetFileDataByIndex(M.brwindex);
			if(fd != NULL)
			{
			    brw->MouseClick(M.brwindex, false, true);
                MC->listen->CommonInputActions(); // Force browser refresh to indicate entry

				menu = new Menu(mouse_x, mouse_y, Menu_Context);
				menu->browser = brw;
				menu->activefiledata = fd;

				m.addItem(MItem_OpenProject, T("Open project"));
				m.addItem(MItem_DeleteProject, T("Delete project"));
			}
        }
    }
    else if(aux_panel->CheckIfMouseOnBinder(mouse_x, mouse_y) && aux_panel->workPt->ibound != NULL)
    {
		menu = new Menu(mouse_x, mouse_y, Menu_Context);

		m.addItem(MItem_Unbind, T("Unbind pianoroll"));
    }
    
	if(M.mmode & MOUSE_CONTROLLING && M.active_ctrl != NULL)
    {
        bool separate = false;
        if(menu != NULL)
            separate = true;
        else
            separate = false;

        menu = new Menu(mouse_x, mouse_y, Menu_Context);
        menu->activectrl = M.active_ctrl;

        if(M.active_ctrl->type == Ctrl_Slider || M.active_ctrl->type == Ctrl_Knob)
        {
            bool skip = false;
            if(M.active_ctrl->type == Ctrl_Slider)
			{
				SliderBase* sb = (SliderBase*)M.active_ctrl;
				if(sb->sltype == Slider_Zoom)
				{
					skip = true;
				}
			}

			if(!skip)
			{
                if(separate)
                {
                    m.addSeparator();
                }
                //if(M.active_ctrl->param->type == Param_Vol || M.active_ctrl->param->type == Param_Pan)
                {
                    m.addItem(MItem_ResetControl, T("Reset"));
                }

                m.addItem(MItem_CreateEnvelope, T("Create envelope at cursor pos."), C.patt->ptype == Patt_Grid);
            }
        }
        else if(M.active_ctrl->type == Ctrl_Toggle)
        {
            Toggle* tg = (Toggle*)M.active_ctrl;
            if(tg->tgtype == Toggle_Mute || tg->tgtype == Toggle_Solo)
            {
                m.addItem(MItem_CreateCommand, T("Create command at cursor"), C.patt->ptype == Patt_Grid);
            }
        }
    }

    if(menu != NULL)
    {
        //skip_after_menu = true;
        post_menu_update = true;
        int result = m.showAt(MC->getScreenX() + mouse_x, MC->getScreenY() + mouse_y, 0, 160, 0, 20);
        if(result != 0)
        {
            if(result >= 100) // VST effects
            {
                int eindex = 100;
                ModListEntry *pEntry = pModulesList->pFirst;
                while (pEntry != NULL)
                {
                    if(pEntry->modtype == ModuleType_Effect)
                    {
                        if(eindex == result)
                        {
                            FileData* filedata = new FileData;
                            filedata->ftype = FType_VST;

                            filedata->dirtype = Dir_Nodir;
                            strcpy(filedata->name, pEntry->name);
                            strcpy(filedata->path, pEntry->path);
                            filedata->size = 0;
                            filedata->attrs = 0;
                            //filedata->list_index = ++cur_index;
                            filedata->list_index = -1;
                            if(menu->curreff == NULL)
                            {
                                WaitForSingleObject(aux_panel->hMixMutex, INFINITE);
                                aux_panel->AddEffectFromBrowser(filedata, menu->mchan);
                                ReleaseMutex(aux_panel->hMixMutex);
                            }
                            else
                            {
                                M.dropeff1 = menu->curreff->cprev;
                                M.dropeff2 = menu->curreff->cnext;
                                M.active_dropmixchannel = menu->mchan;
                                DeleteEffect(menu->curreff);
                                WaitForSingleObject(aux_panel->hMixMutex, INFINITE);
                                Eff* eff = aux_panel->AddEffectFromBrowser(filedata, menu->mchan);
                                if(eff != NULL)
                                {
                                    DragNDrop_PlaceEffect(eff);
                                }
                                ReleaseMutex(aux_panel->hMixMutex);
                            }
                            break;
                        }
                        eindex++;
                    }
                    pEntry = pEntry->Next;
                }
            }
            else
            {
                MItemType it = (MItemType)result;
                ActivateMenuItem(NULL, it, menu);
            }
        }

        delete menu;
        menu = NULL;

        R(Refresh_AuxHighlights);
    }
}

void ShowHintIfApplicable()
{
    if(M.LMB || M.RMB)
    {
        MC->DisableHint();
        return;
    }

    String str;
    if(M.active_ctrl != NULL /*&& M.active_ctrl != M.lasthintctrl*/)
    {
        M.lasthintctrl = M.active_ctrl;

        if(M.active_ctrl->isVolumeSlider())
        {
            str = GetDbString(M.active_ctrl->param->outval);
        }
        else if(M.active_ctrl->isPanningSlider())
        {
            str = GetPanString(M.active_ctrl->param->outval);
        }
        else if(M.active_ctrl->type == Ctrl_Knob)
        {
            str = GetPercentString(M.active_ctrl->param->outval);
        }
        else
        {
            if(M.active_ctrl == CP->BtFollowPlayback)
            {
                str = String("Follow playback position");
            }
            else if(M.active_ctrl == CP->view_mixer)
            {
                str = String("Toggle mixer");
            }
            else if(M.active_ctrl == aux_panel->PtPianorol)
            {
                str = String("Switch to pianoroll");
            }
            else if(M.active_ctrl == aux_panel->PtStepseq)
            {
                str = String("Switch to step sequencer");
            }
            else if(M.active_ctrl == aux_panel->PtUsual)
            {
                str = String("Switch to text grid");
            }
            else if(M.active_ctrl == aux_panel->Vols)
            {
                str = String("Switch to velocity mode");
            }
            else if(M.active_ctrl == aux_panel->Pans)
            {
                str = String("Switch to panning mode");
            }
            else if(M.active_ctrl == aux_panel->slzoom || M.active_ctrl == aux_panel->DecrScale || M.active_ctrl == aux_panel->IncrScale)
            {
                str = String("Pattern area zooming");
            }
            else if(M.active_ctrl == CP->slzoom  || M.active_ctrl == CP->IncrScale || M.active_ctrl == CP->DecrScale)
            {
                str = String("Main area zooming");
            }
            else if(M.active_ctrl == genBrw->ShowFiles)
            {
                str = String("Browse files");
            }
            else if(M.active_ctrl == genBrw->ShowProjects)
            {
                str = String("Browse projects only");
            }
            else if(M.active_ctrl == genBrw->ShowSamples)
            {
                str = String("Samples only");
            }
            else if(M.active_ctrl == genBrw->ShowExternalPlugs || M.active_ctrl == mixBrw->ShowExternalPlugs)
            {
                str = String("Plugins only");
            }
            else if(M.active_ctrl == genBrw->ShowPresets || M.active_ctrl == mixBrw->ShowPresets)
            {
                if(M.active_ctrl == genBrw->ShowPresets)
                    str = String("Presets of the current instrument");
                else
                    str = String("Presets of the current effect");
            }
            else if(M.active_ctrl == genBrw->ShowParams || M.active_ctrl == mixBrw->ShowParams)
            {
                if(M.active_ctrl == genBrw->ShowParams)
                    str = String("Params of the current instrument");
                else
                    str = String("Params of the current effect");
            }
            else if(M.active_ctrl == mixBrw->HideTrackControls)
            {
                str = String("Hide browser");
            }
            else if(M.active_ctrl == CP->MVol)
            {
                str = String("Master volume");
            }
            else if(M.active_ctrl == CP->NoteMode)
            {
                str = String("Keyboard-sequencing mode");
            }
            else if(M.active_ctrl == CP->MetroMode)
            {
                str = String("Metronome ON/OFF");
            }
            else if(strlen(M.active_ctrl->hint) > 0)
            {
                str = String(M.active_ctrl->hint);
            }
        }
    }
    else if(M.active_paramedit != NULL)
    {
        if(strlen(M.active_paramedit->hint) > 0)
        {
            str = String(M.active_paramedit->hint);
        }
    }


    int w = (int)rox->getStringWidth(str) + 8;
    int h = (int)rox->getHeight();

    int xhint, yhint;
    xhint = M.mouse_x + 12;
    if(xhint > WindWidth - w)
        xhint = WindWidth - w - 1;

    yhint = M.mouse_y + 12;
    if(yhint > WindHeight - h)
        yhint = WindHeight - h - 1;

    if(str.length() > 0)
    {
        MC->EnableHint(str, xhint, yhint);

        MC->hint->stopTimer();
        MC->hint->startTimer(4444);
    }
}

void Process_MouseWheel(int delta, int mouse_x, int mouse_y, unsigned flags)
{
    if(M.mmode & MOUSE_LINING)
    {
        //Lining_ProcessWheel(M.lineloc, delta);
    }

    if(M.mmode & MOUSE_BPMING)
    {
        if(M.bpmbig)
        {
            beats_per_minute += delta;
        }
        else
        {
            beats_per_minute += (float)delta/100;
        }
        UpdatePerBPM();

        CP->bpmer->drawarea->Change();
        R(Refresh_GridContent);
        if(aux_panel->auxmode == AuxMode_Pattern)
        {
            R(Refresh_AuxContent);
        }
    }

    if(M.mmode & MOUSE_AUXMIXING && M.active_mixchannel != NULL && M.active_mixchannel->cheight > 0 &&
        mouse_y > M.active_mixchannel->ry1 && mouse_y < M.active_mixchannel->ry2)
    {
        M.active_mixchannel->voffs -= delta*22;
        if(M.active_mixchannel->voffs < 0)
        {
            M.active_mixchannel->voffs = 0;
        }
        else if(M.active_mixchannel->voffs > M.active_mixchannel->cheight - 4)
        {
            M.active_mixchannel->voffs = M.active_mixchannel->cheight - 4;
        }
        M.active_mixchannel->drawarea->Change();
    }
    else if(M.mmode & MOUSE_CONTROLLING && !(M.mmode & MOUSE_INSTRUMENTING)&&
            M.active_ctrl != NULL && 
            M.active_ctrl->type != Ctrl_Toggle &&
            M.active_ctrl->type != Ctrl_InPin_Point &&
            M.active_ctrl->type != Ctrl_OutPin_Point)
    {
        if(M.active_ctrl == IP->sbar)
        {
            M.active_ctrl->TweakByWheel(delta*IP->sbar->pixlen/num_instrs, mouse_x, mouse_y);
        }
        else if(M.active_ctrl == genBrw->sbar)
        {
            genBrw->sbar->TweakByWheel(delta*genBrw->sbar->pixlen/genBrw->current_num_entries*2, mouse_x, mouse_y);
        }
        else if(M.active_ctrl == mixBrw->sbar)
        {
            mixBrw->sbar->TweakByWheel(delta*mixBrw->sbar->pixlen/mixBrw->current_num_entries*2, mouse_x, mouse_y);
        }
        else if(M.active_ctrl == aux_panel->v_sbar)
        {
            if(aux_panel->v_sbar->full_len > aux_panel->v_sbar->visible_len)
            {
                aux_panel->v_sbar->TweakByWheel(delta*2, mouse_x, mouse_y);
            }
        }
        else if(M.active_ctrl == main_v_bar)
        {
            main_v_bar->TweakByWheel(delta*2, mouse_x, mouse_y);
        }
        else if(M.active_ctrl == CP->slzoom || 
                M.active_ctrl == CP->IncrScale || 
                M.active_ctrl == CP->DecrScale)
        {
            if(delta > 0)
            {
                while(delta > 0)
                {
                    IncreaseScale(false);

                    delta--;
                }
            }
            else
            {
                while(delta < 0)
                {
                    DecreaseScale(false);
                    delta++;
                }
            }
        }
        else if(M.active_ctrl == aux_panel->slzoom || 
                M.active_ctrl == aux_panel->IncrScale || 
                M.active_ctrl == aux_panel->DecrScale)
        {
            if(delta > 0)
            {
                while(delta > 0)
                {
                    aux_panel->IncreaseScale(false);
                    delta--;
                }
            }
            else
            {
                while(delta < 0)
                {
                    aux_panel->DecreaseScale(false);
                    delta++;
                }
            }
        }
        else
        {
            M.active_ctrl->TweakByWheel(delta, mouse_x, mouse_y);
        }
    }
    else if(M.mmode & MOUSE_INSTRUMENTING)
    {
        /*
        if(IP->sbar->active == true)
        {
            IP->sbar->TweakByWheel(delta*IP->sbar->pixlen/num_instrs, mouse_x, mouse_y);
        }*/
        if(delta < 0 && (IP->GetFullLHeight() + IP->main_offs) < (float)(InstrPanelHeight - brw_heading))
        {
        }
        else
        {
            IP->main_offs += delta*19;
            if(IP->main_offs > 0)
            {
                IP->main_offs = 0;
            }
        }

        R(Refresh_InstrPanel);

    }
    else if(M.mmode & MOUSE_BROWSING)
    {
        Browser* brw;
        if(M.btype == Brw_Generators)
        {
            brw = genBrw;
        }
        else if(M.btype == Brw_Effects)
        {
            brw = mixBrw;
        }

        if(brw->sbar->active == true)
        {
            brw->sbar->TweakByWheel(delta*brw->sbar->pixlen/brw->current_num_entries*2, mouse_x, mouse_y);
        }

        if(M.btype == Brw_Generators)
        {
            R(Refresh_GenBrowser);
        }
        else if(M.btype == Brw_Effects)
        {
            R(Refresh_MixCenter);
        }
    }
    else if(M.loc == Loc_MainGrid || M.mmode & MOUSE_TRACKING)
    {
        if(flags & kbd_ctrl)
        {
            if(delta > 0)
            {
                while(delta > 0)
                {
                    IncreaseScale(true);

                    delta--;
                }
            }
            else
            {
                while(delta < 0)
                {
                    DecreaseScale(true);
                    delta++;
                }
            }
        }
        else
        {
            /*
            OffsLine -= delta*2;
            if(OffsLine < 0)
            {
                OffsLine = 0;
            }
            else if(OffsLine > field->num_lines)
            {
                OffsLine = field->num_lines;
            }
            */

            main_v_bar->SetDelta(float(-delta*2));
        }

        if(M.loc == Loc_MainGrid)
        {
            R(Refresh_Grid);
            if(mixbrowse == false)
            {
                R(Refresh_MixCenter);
            }

            if(aux_panel->isVolsPansMode())
            {
                R(Refresh_Aux);
            }
        }
        else if(M.loc == Loc_SmallGrid)
        {
            R(Refresh_Aux);
        }
    }
    else if(M.loc == Loc_SmallGrid || M.on_keys == true)
    {
        if(flags & kbd_ctrl)
        {
            if(delta > 0)
            {
                while(delta > 0)
                {
                    aux_panel->IncreaseScale(true);
                    delta--;
                }
            }
            else
            {
                while(delta < 0)
                {
                    aux_panel->DecreaseScale(true);
                    delta++;
                }
            }
            R(Refresh_AuxGrid);
        }
        else
        {
            if(aux_panel->v_sbar->full_len > aux_panel->v_sbar->visible_len)
            {
                aux_panel->v_sbar->TweakByWheel(delta*2, mouse_x, mouse_y);
            }

            /*
            if(gAux->workPt->ptype == Patt_Pianoroll)
            {
                gAux->note_offset += delta*2;
                if(gAux->note_offset > NUM_PIANOROLL_LINES - 1)
                {
                    gAux->note_offset = NUM_PIANOROLL_LINES - 1;
                }
                else if(gAux->note_offset < (gAux->v_sbar->visible_len))
                {
                    gAux->note_offset = (int)(gAux->v_sbar->visible_len);

                    gAux->v_sbar->offset = gAux->v_sbar->full_len - gAux->v_sbar->visible_len;
                    gAux->bottomincr = gAux->lineHeight - ((gAux->patty2 - gAux->eux) - (MainY2 + 36))%gAux->lineHeight;
                    gAux->offset_correction = true;
                }
                gAux->v_sbar->actual_offset = gAux->v_sbar->offset = (float)(NUM_PIANOROLL_LINES - gAux->note_offset - 1);
                gAux->OffsLine = (int)gAux->v_sbar->actual_offset;

                gAux->workPt->offs_line = gAux->OffsLine;
            }
            else
            {
                if(gAux->v_sbar->full_len > gAux->v_sbar->visible_len)
                {
                    gAux->v_sbar->TweakByWheel(delta*2, mouse_x, mouse_y);
                }
            }*/
        }
    }

    M.PosUpdate(mouse_x, mouse_y, flags);
}

bool CanClickOntoGrid()
{
    if((M.mmode & MOUSE_ON_GRID)&&
        !(M.mmode & MOUSE_LANEAUXING)&&
        !(M.mmode & MOUSE_RESIZE || M.mmode & MOUSE_LOWAUXEDGE)&&
        !(M.loc == Loc_SmallGrid && aux_panel->workPt->ptype == Patt_StepSeq && M.current_trk->defined_instr == NULL)&&
        !(M.mmode & MOUSE_UPPER_TRACK_EDGE || M.mmode & MOUSE_LOWER_TRACK_EDGE)&&
        !(M.mmode & MOUSE_GRID_X1 || M.mmode & MOUSE_GRID_X2 || M.mmode & MOUSE_GRID_Y1 || M.mmode & MOUSE_GRID_Y2))
        return true;
    else
        return false;
}

void Process_LeftButtDown(int mouse_x, int mouse_y, bool dbclick, unsigned flags)
{
    M.LMB = true;
    M.bypass_element_activating = false;
    M.lastclick = LastClicked_Nothing;

	if(MC->hint->isVisible())
    {
        MC->DisableHint();
    }

    if(M.menu_active == true)
    {
        Menu_MouseClick(mouse_x, mouse_y);
    }
    else if(dbclick == true && JustClickedPattern == true)
    {
        aux_panel->EditPattern(aux_panel->workPt, dbclick);
        skip_input = true;
    }
    JustClickedPattern = false;

    {
        if(post_menu_update == true)
        {
            post_menu_update = false;
            M.PosUpdate(mouse_x, mouse_y, flags, true);
        }

        if(skip_input == false)
        {
            if(M.holding == true)
            {
                DragNDrop_Drop(mouse_x, mouse_y, flags);
            }
            else
            {
                if(M.on_keys == true)
                {
                    aux_panel->ClickKeys();
                }

                if(M.mmode & MOUSE_INSTRUMENTING)
                {
                    M.lastclick = LastClicked_Instrument;
                    IP->Click(mouse_x, mouse_y, dbclick, flags);
                }
                else if(M.mmode & MOUSE_AUXMIXING)
                {
                    if(M.active_effect != NULL)
                        M.lastclick = LastClicked_Effect;

                    /*
                    if(!(M.mmode & MOUSE_CONTROLLING && M.active_ctrl->type == Ctrl_Toggle && 
                         (((Toggle*)M.active_ctrl)->tgtype == Toggle_EffFold || 
                          ((Toggle*)M.active_ctrl)->tgtype == Toggle_EffBypass)))*/
                    {
                        if(M.active_effect != NULL)
                        {
                            aux_panel->SetCurrentEffect(M.active_effect);
                        }
                        //else if(gAux->current_eff != NULL)
                        //{
                        //    gAux->current_eff = NULL;
                        //    R(Refresh_AuxHighlights);
                        //}

                        if(!(M.mmode & MOUSE_CONTROLLING) && M.active_effect != NULL && dbclick /*&& M.active_effect->type != EffType_VSTPlugin*/)
                        {
                            M.active_effect->ToggleParamWindow();
                        }
                    }
                }

                if(M.active_paramedit != NULL)
                {
                    M.active_paramedit->Click(mouse_x, mouse_y, dbclick, flags);
                }

                if((M.mmode & MOUSE_CONTROLLING)&&(M.active_ctrl->enabled == true)&& !(flags & kbd_ctrl))
                {
					Control* ctrl = M.active_ctrl;
                    M.active_ctrl->MouseClick(mouse_x, mouse_y);
					if(M.active_ctrl != NULL && M.active_ctrl->type == Ctrl_Knob && dbclick)
                    {
						aux_panel->mix_sbar->SetOffset(aux_panel->mix_sbar->full_len);
						aux_panel->mix_sbar->SetDelta(-5);
                    }
                }
        		else if(M.mmode & MOUSE_ON_GRID)
                {
                    if(M.mmode & MOUSE_ENVELOPING && !(M.mmode & MOUSE_RESIZE))
                    {
                        M.active_env->Click(mouse_x, mouse_y, flags);

                        ChangesIndicate();

    					if(M.loc == Loc_MainGrid)
    						R(Refresh_GridContent);
    					else if(M.loc == Loc_SmallGrid)
    						R(Refresh_AuxContent);
                    }   // if mouse is on grid and doesn't do anything
                    else if(CanClickOntoGrid())
                    {
                        Grid_MouseClickDown(mouse_x, mouse_y, flags);
                    }
                }
                else if(M.mmode & MOUSE_LINING && !M.RMB)
                {
                    Lining_ProcessMouse(M.lineloc, mouse_x);
                }
                else if((M.mmode & MOUSE_BROWSING) == MOUSE_BROWSING)
                {
                    Browser* b;
                    if(M.btype == Brw_Generators)
                    {
                        b = genBrw;
                    }
                    else if(M.btype == Brw_Effects)
                    {
                        b = mixBrw;
                    }

                    b->MouseClick(M.brwindex, dbclick, true);
                }
                else if(M.mmode & MOUSE_AUXING && (aux_panel->auxmode == AuxMode_Vols || aux_panel->auxmode == AuxMode_Pans))
                {
                    ParamType partype = (aux_panel->auxmode == AuxMode_Vols) ? Param_Vol : Param_Pan;
                    ProcessColLane(mouse_x, mouse_y, partype, Loc_MainGrid, NULL, NULL, GridX1, AuxRY1, GridX2, AuxRY2, (flags & kbd_alt) != 0);

                    R(Refresh_Aux);
                }
                else if(M.mmode & MOUSE_AUXAUXING && !M.IsOnEdge())
                {
                    switch(aux_panel->ltype)
                    {
                        case Lane_Vol:
                            ProcessColLane(mouse_x, mouse_y, Param_Vol, Loc_SmallGrid, NULL, NULL, GridXS1, GridYS2 + 4, GridXS2, WindHeight - 4, (flags & kbd_alt) != 0);
                            break;
                        case Lane_Pan:
                            ProcessColLane(mouse_x, mouse_y, Param_Pan, Loc_SmallGrid, NULL, NULL, GridXS1, GridYS2 + 4, GridXS2, WindHeight - 4, (flags & kbd_alt) != 0);
                            break;
                        case Lane_Pitch:
                        {
                            Envelope* env = (Envelope*)aux_panel->workPt->basePattern->pitch->paramedit;
                            env->Click(mouse_x, mouse_y, flags);
                        }break;
                    }

                    R(Refresh_SubAux);
                }

                UpdateElementsVisibility();
            }
        }
    }

    if(M.active_ctrl != NULL && 
        (M.active_ctrl->isVolumeSlider() || M.active_ctrl->isPanningSlider() || M.active_ctrl->type == Ctrl_Knob))
        UpdateControlHint(M.active_ctrl, mouse_x, mouse_y);

    if(skip_input == true && out_from_dialog == true)
    {
        // this is the case, when some top dialog were shown from our native menu. need to clear these
        // flags to avoid them stalling
        skip_input = false;
        out_from_dialog = false;
    }

    M.PosUpdate(mouse_x, mouse_y, flags);
}

void Process_LeftButtUp(int mouse_x, int mouse_y, bool dbclick, unsigned int flags)
{
    M.LMB = false;
    M.dragcount = 0;
    M.pickedup = false;

    if(skip_input == false)
    {
        M.prev_x = -1;
        M.prev_y = -1;
        aux_panel->prev_mouse_x = -1;
        aux_panel->prev_mouse_y = -1;
        M.mousefix = false;

        DropPickedElements();

        if(M.mmode & MOUSE_BRUSHING)
        {
            //M.mmode &= ~MOUSE_BRUSHING;
        }

        if(M.mmode & MOUSE_BPMING)
        {
            beats_per_minute = beats_per_minute_temp;
            CP->bpmer->fnum = &beats_per_minute;
            if(!(M.mmode & MOUSE_CONTROLLING))
                Desktop::setMousePosition(mouse_x, CtrlPanelHeight - 12);
            UpdatePerBPM();
            R(Refresh_GridContent);
            if(aux_panel->isPatternMode())
            {
                R(Refresh_AuxContent);
            }

            SetCursor(ArrowCursor);
        }

        if(JustMoved == false)
        {
            // If can click onto grid and no selection was made
            if(CanClickOntoGrid() && ((SelX2 - SelX1 <= 6 && SelY2 - SelY1 <= 6)) &&
                                      !(M.mmode & MOUSE_BRUSHING || M.mmode & MOUSE_ENVELOPING))
            {
                // And if not on step sequencer
                if(!(M.loc == Loc_SmallGrid && aux_panel->workPt->ptype == Patt_StepSeq))
                {
                    C.ExitPerClickInAnotherGridPlace();
                    C.PosAndCursor2Mouse(mouse_x, mouse_y);

                    // Reset Aux patt mode if needed
                    if(C.loc == Loc_MainGrid && 
                        !(M.active_elem != NULL && M.active_elem->type == El_Pattern) &&
        				aux_panel->workPt != aux_panel->blankPt &&
        			   !aux_panel->workPt->autopatt)
                    {
                        aux_panel->AuxReset();
                        if(aux_panel->auto_switch == true)
                        {
                            aux_panel->HandleButtDown(aux_panel->Vols);
                        }
                    }
                    UpdateNavBarsData();

                    R(Refresh_GridContent);
                    R(Refresh_KeyCursor);
                    if(C.loc == Loc_MainGrid)
                    {
                        R(Refresh_MainBar);
                    }
                    else
                    {
                        R(Refresh_AuxGrid);
                    }
                }
            }

			if(/*M.pickedup == false &&*/ M.mmode & MOUSE_SELECTING)
            {
                Selection_Stop(flags);
            }


            if(M.active_paramedit != NULL && M.active_paramedit->type == Param_FX)
            {
                MC->listen->setMouseCursor(MouseCursor(MouseCursor::NormalCursor));
                if(M.active_paramedit->donotactivate == true)
                    Desktop::setMousePosition(M.active_paramedit->x + 5, M.active_paramedit->y - 5);
            }

            if(M.mmode & MOUSE_DRAG_N_DROPPING)
            {
                DragNDrop_Drop(mouse_x, mouse_y, flags);
            }
            else if(M.mmode & MOUSE_CONTROLLING)
            {
                if(M.active_ctrl->type == Ctrl_Button)
                {
                    Butt* bt;
                    bt = (Butt*)M.active_ctrl;
                    bt->Release();
                    if(bt->panel != NULL)
                    {
                        bt->panel->HandleButtUp(bt);
                    }
                }
                else if(M.active_ctrl->type == Ctrl_ButtFix)
                {
                    ButtFix* bt = (ButtFix*)M.active_ctrl;
                    if(bt->panel != NULL)
                    {
                        bt->panel->HandleButtUp(bt);
                    }
                }
                /*
                else if(M.active_ctrl->type == Ctrl_OutPin_Point)
                {
                    OutPin* op = (OutPin*)M.active_ctrl;
                    if(op->connected == false)
                    {
                        op->xconn = op->xc;
                        op->yconn = op->yc;
                        op->xc_offs = op->yc_offs = 0;

                        if(op->owna->outcell != NULL)
                        {
                            op->owna->Disconnect();
                        }
                        op->dst = NULL;
                    }

                    R(Refresh_MixHighlights);
                }
                */
            }
            else if(M.bypass_element_activating == false && 
                    !(M.mmode & MOUSE_RESIZE)&&
                    !(M.mmode & MOUSE_BRUSHING && (M.brushbasetick != M.lpx)))
            {
                if((M.pianokey_pressed == true))
                {
                    if(aux_panel->workPt->ptype == Patt_Pianoroll)
                    {
                        M.pianokey_pressed = false;
                    }

                    R(Refresh_PianoKeys);
                }

                if(M.active_elem != NULL)
                {
                    M.ClickActiveElement(mouse_x, mouse_y, dbclick, flags);
                }
                else
                {
                    if(M.active_paramedit != NULL && M.active_paramedit->donotactivate == false)
                    {
                        C.ExitToDefaultMode();
                        M.active_paramedit->Activate();
                        C.mode = CMode_JustParamEdit;
                        C.SetPattern(field_pattern, Loc_Other);
                        C.curParam = M.active_paramedit;

                        R(Refresh_KeyCursor);
                    }
                }
            }
        }
        else
        {
            JustMoved = false;
        }

        Preview_MouseRelease();
        if(genBrw->prevInstr != NULL)
        {
            IP->RemoveInstrument(genBrw->prevInstr);
            genBrw->prevInstr = NULL;
        }

        M.PosUpdate(mouse_x, mouse_y, flags);
    }

    if(undoMan->started == true)
    {
        undoMan->StartNewGroup();
    }

	if(skip_input == true)
    {
        skip_input = false;
    }
}

void Process_RightButtDown(int mouse_x, int mouse_y, unsigned flags)
{
    M.RMB = true;
    M.skip_menu = false;

	if(MC->hint->isVisible())
		MC->DisableHint();

    if(M.menu_active == true)
    {
        Menu_MouseClick(mouse_x, mouse_y);
    }
    else
    {
        if(post_menu_update == true)
        {
            post_menu_update = false;
            M.PosUpdate(mouse_x, mouse_y, flags, true);
        }

        if(skip_input == false)
        {
            if(M.mmode & MOUSE_ON_GRID)
            {
                //Selection_Reset();
                //if(Num_Selected == 0)
                {
                    if(M.active_elem != NULL && M.pickedup == false)
                    {
                        Element* el = M.active_elem;
                        DeleteElement(el, false, false);
                        if(M.loc == Loc_SmallGrid)
                        {
                            aux_panel->RescanPatternBounds();
                        }
                        M.active_elem = NULL;
                        M.skip_menu = true;

                        R(Refresh_Buttons);
                        if(M.loc == Loc_MainGrid)
                        {
                            R(Refresh_GridContent);
                            if(aux_panel->isVolsPansMode())
                            {
                                R(Refresh_Aux);
                            }
                        }
                        else if(M.loc == Loc_SmallGrid)
                        {
                            R(Refresh_AuxContent);
                            R(Refresh_SubAux);
                        }
                    }
                }
            }
            else if((M.mmode & MOUSE_BROWSING) == MOUSE_BROWSING)
            {
                Browser* b;
                if(M.btype == Brw_Generators)
                {
                    b = genBrw;

                    R(Refresh_GenBrowser);
                }
                else if(M.btype == Brw_Effects)
                {
                    b = mixBrw;

                    R(Refresh_MixCenter);
                }

                b->MouseClick(M.brwindex, false, false);
            }
            else if(M.mmode == MOUSE_INSTRUMENTING && M.active_instr != NULL)
            {
                ChangeCurrentInstrument(M.active_instr);
            }

            if(M.mmode & MOUSE_LINING && Sel_Active == true && M.selmode == Sel_Fragment)
            {
                Selection_Reset();
            }

            if(M.mmode & MOUSE_LINING && Looping_Active == true)
            {
                Looping_Reset();
            }

            if(M.mmode & MOUSE_ENVELOPING)
            {
                M.active_env->Click(mouse_x, mouse_y, flags);

                if(M.mmode & MOUSE_ON_GRID)
                {
                    if(M.loc == Loc_MainGrid)
                        R(Refresh_GridContent);
                    else if(M.loc == Loc_SmallGrid)
                        R(Refresh_AuxContent);
                }
                else if(M.mmode & MOUSE_AUXAUXING)
                {
                    R(Refresh_SubAux);
                }
            }

            if(M.active_paramedit != NULL && M.active_paramedit->type == Param_FX)
            {
                DigitStr* pfx = (DigitStr*)M.active_paramedit;
                strcpy(pfx->digits, "--");
                if(pfx->panel != NULL)
                {
                    pfx->panel->Update();
                }
                if(pfx->instr != NULL || pfx->trk != NULL)
                {
                    aux_panel->CheckFXString(pfx);
                }
                pfx->drawarea->Change();
                M.skip_menu = true;
            }
        }
    }

    if(skip_input == true && out_from_dialog == true)
    {
        // this is the case, when some top dialog were shown from our native menu. need to clear these
        // flags to avoid them stalling
        skip_input = false;
        out_from_dialog = false;
    }

    M.PosUpdate(mouse_x, mouse_y, flags);
}

void Process_RightButtUp(int mouse_x, int mouse_y, unsigned int flags)
{
    M.RMB = false;
    M.dragcount = 0;
    M.pickedup = false;

    if(skip_input == false)
    {
        M.prev_x = -1;
        M.prev_y = -1;
        aux_panel->prev_mouse_x = -1;
        aux_panel->prev_mouse_y = -1;
        M.mousefix = false;

        DropPickedElements();

        if(M.pickedup == false && M.mmode & MOUSE_SELECTING)
        {
            Selection_Stop(flags);
        }
        else if(M.mmode & MOUSE_ENVELOPING)
        {
            M.skip_menu = true;
        }
        else if(M.mmode & MOUSE_CONTROLLING)
        {
            // Workaround for the case, when two mouse buttons are clicked voer some panel button and then
            // right mouse button is released. This will lead to panel button's being unpressed because
            // Juce strangely won't handle left mouse button release
            if(M.active_ctrl->type == Ctrl_Button)
            {
                Butt* bt;
                bt = (Butt*)M.active_ctrl;
                if(bt->pressed)
                {
                    bt->Release();
                    if(bt->panel != NULL)
                    {
                        bt->panel->HandleButtUp(bt);
                    }
                }
            }
            else if(M.active_ctrl->type == Ctrl_ButtFix)
            {
                ButtFix* bt = (ButtFix*)M.active_ctrl;
                if(bt->pressed)
                {
                    if(bt->panel != NULL)
                    {
                        bt->panel->HandleButtUp(bt);
                    }
                }
            }
        }

        Preview_MouseRelease();

        M.PosUpdate(mouse_x, mouse_y, flags);
    }

    if(undoMan->started == true)
    {
        undoMan->StartNewGroup();
    }

    if(M.pickedup == false && M.skip_menu == false && skip_input == false)
    {
        CreateContextMenu(mouse_x, mouse_y);
    }
    else if(skip_input == true)
    {
        skip_input = false;
    }
}

void Process_MouseMove(int mouse_x, int mouse_y, unsigned flags, bool left, bool right)
{
    M.hint_active = false;
    M.LMB = left;
    M.RMB = right;

    if(M.LMB == false && M.RMB == false && M.holding == false)
    {
		if((M.mouse_x != mouse_x || M.mouse_y != mouse_y))
        {
			if(MC->hint->isVisible())
			{
			    bool skip = false;
				if(M.active_ctrl != NULL)
				{
					if(M.active_ctrl->isVolumeSlider() || M.active_ctrl->isPanningSlider() || M.active_ctrl->type == Ctrl_Knob)
					{
						skip = true;
					}
				}

                if(!skip)
				{
    				MC->hint->stopTimer();
    				MC->DisableHint();
    			}
			}
			if(M.active_ctrl != NULL || M.active_paramedit != NULL)
			{
				MC->hint->stopTimer();
				if(M.active_ctrl != NULL && 
                    (M.active_ctrl->isVolumeSlider() || 
                     M.active_ctrl->isPanningSlider() || 
                     M.active_ctrl->type == Ctrl_Knob))
				{
					MC->hint->startTimer(4);
				}
				else
					MC->hint->startTimer(444);
			}
	    }
    }
    else
    {
        if(M.LMB == true || M.RMB == true || M.holding == true)
        {
            if(M.holding == true)
            {
                R(Refresh_MouseCursor);
            }

            M.dragcount++;
            if(M.dragcount > 1)
            {
                M.skip_menu = true;
            }

            if(M.IsOnEdge())
            {
                MoveBorders(mouse_x, mouse_y);
                /*
                if(M.mmode & MOUSE_GRID_Y2)
                {
                    R(Refresh_Aux);
                    R(Refresh_Grid);
                    R(Refresh_MixCenter);
                }
                else if(M.mmode & MOUSE_GRID_X1)
                {
                    R(Refresh_GenBrowser);
                    R(Refresh_InstrPanel);
                    R(Refresh_Grid);
                    R(Refresh_Aux);
                }
                else*/
                {
                    R(Refresh_All);
                }
            }
            else if(M.mmode & MOUSE_DRAG_N_DROPPING)
            {
                DragNDrop_Process(mouse_x, mouse_y);
            }

            if(M.mmode & MOUSE_BPMING)
            {
                if(M.active_ctrl == NULL)
                {
                    int dt = mouse_y - CP->bpmer->ys;
                    if(M.bpmbig)
                    {
                        beats_per_minute_temp -= dt;
                    }
                    else
                    {
                        beats_per_minute_temp -= (float)dt/100;
                    }
                    if(beats_per_minute_temp < 10)
                        beats_per_minute_temp = 10;
                    else if(beats_per_minute_temp > 999)
                        beats_per_minute_temp = 999;

                    CP->bpmer->ys += dt;
                    //UpdatePerBPM();

                    CP->bpmer->drawarea->Change();

                    if(mouse_y < 11)
                    {
                        Desktop::setMousePosition(mouse_x, CtrlPanelHeight + 20);
                        CP->bpmer->xs = mouse_x;
                        CP->bpmer->ys = CtrlPanelHeight + 20;
                    }
                    else if(mouse_y > CtrlPanelHeight + 20)
                    {
                        Desktop::setMousePosition(mouse_x, 11);
                        CP->bpmer->xs = mouse_x;
                        CP->bpmer->ys = 11;
                    }

                    /*
                    R(Refresh_GridContent);
                    if(gAux->isPatternMode())
                    {
                        R(Refresh_AuxContent);
                    }*/
                }
            }
            else if(M.mmode & MOUSE_CONTROLLING && M.RMB == false)
            {
                MoveControls(mouse_x, mouse_y, flags);
            }
            else if(M.active_paramedit != NULL && M.active_paramedit->type == Param_FX)
            {
                DigitStr* dstr = (DigitStr*)M.active_paramedit;
                dstr->donotactivate = true;
                int numcell = ParamString2Num(dstr->digits);
                if(numcell == 100)
                    numcell = 0;

                numcell = dstr->oldval -(mouse_y - dstr->ys)/4;
                if(numcell < 1)
                    numcell = 100;
                else if(numcell > NUM_MIXCHANNELS)
                    numcell = NUM_MIXCHANNELS;
                ParamNum2String(numcell, dstr->digits);
                dstr->SetString(dstr->digits);
                ChangesIndicate();
            }
            else if(M.mmode & MOUSE_LINING && !M.RMB)
            {
                Lining_ProcessMouse(M.lineloc, mouse_x);
            }
            else if(M.mmode & MOUSE_ON_GRID)
            {
                Grid_MouseDrag(mouse_x, mouse_y, flags);
            }
			else if(M.mmode & MOUSE_AUXING)
            {
                if((aux_panel->auxmode == AuxMode_Vols || aux_panel->auxmode == AuxMode_Pans) &&
                    !M.pickedup &&
                    !(M.mmode & MOUSE_SELECTING))
                {
                    ParamType partype = (aux_panel->auxmode == AuxMode_Vols) ? Param_Vol : Param_Pan;
                    ProcessColLane(mouse_x, mouse_y, partype, Loc_MainGrid, NULL, NULL, GridX1, AuxRY1, GridX2, AuxRY2, (flags & kbd_alt) != 0);

                    R(Refresh_Aux);
                }
                else if(aux_panel->auxmode == AuxMode_Pattern)
                {
                    if(!M.RMB && !(M.mmode & MOUSE_AUXAUXING))
                    {
                        int keynum;
                        float vol;
                        M.on_keys = aux_panel->CheckIfMouseOnKeys(mouse_x, mouse_y, &keynum, &vol);
                        if(M.on_keys && keynum != M.pianokey_num)
                        {
                            M.pianokey_num = keynum;
                            Preview_ReleaseAll();
                            aux_panel->ClickKeys();
                        }
                    }

                    if(M.mmode & MOUSE_AUXAUXING)
                    {
                        switch(aux_panel->ltype)
                        {
                            case Lane_Vol:
                                ProcessColLane(mouse_x, mouse_y, Param_Vol, Loc_SmallGrid, NULL, NULL, GridXS1, GridYS2 + 4, GridXS2, WindHeight - 4, (flags & kbd_alt) != 0);
                                break;
                            case Lane_Pan:
                                ProcessColLane(mouse_x, mouse_y, Param_Pan, Loc_SmallGrid, NULL, NULL, GridXS1, GridYS2 + 4, GridXS2, WindHeight - 4, (flags & kbd_alt) != 0);
                                break;
                            case Lane_Pitch:
                            {
                                Envelope* env = (Envelope*)aux_panel->workPt->basePattern->pitch->paramedit;
                                M.active_env = env;
                                M.active_env->Drag(mouse_x, mouse_y, flags);
                            }break;
                        }

                        R(Refresh_SubAux);
                    }
                }
            }

            Selection_Check(mouse_x, mouse_y, flags);
            Looping_Check(mouse_x, mouse_y, flags);

            if(M.pickedup == true && 
               !(M.mmode & MOUSE_BRUSHING) && 
               !(M.mmode & MOUSE_SLIDING) /*&&
			    !(M.loc == Loc_SmallGrid && flags & kbd_ctrl)*/)
            {
                MovePickedUpElements(mouse_x, mouse_y, flags);

                if(M.pickloc == Loc_SmallGrid && !aux_panel->isBlank())
                {
                    aux_panel->workPt->basePattern->UpdateScaledImage();
                    R(Refresh_GridContent);
                }

                if(M.pickloc == Loc_MainGrid)
                {
                    R(Refresh_GridContent);
                    if(aux_panel->isVolsPansMode())
                    {
                        R(Refresh_Aux);
                    }
                }
                else if(M.pickloc == Loc_SmallGrid)
                {
                    R(Refresh_AuxContent);
                    R(Refresh_SubAux);

                    if(aux_panel->workPt->ptype != Patt_StepSeq)
                    {
                        bool changed = aux_panel->RescanPatternBounds();
                        if(changed)
                        {
                            R(Refresh_Aux);
                            R(Refresh_GridContent);
                        }
                    }
                }
            }

            if(!(M.mmode & MOUSE_DRAG_N_DROPPING) && !(M.RMB) && 
                !(M.active_paramedit != NULL && M.active_paramedit->type == Param_FX))
            {
                DragNDrop_Check(mouse_x, mouse_y);
            }

            if(M.selstarted == true /*&& M.dragcount > 1*/)
            {
                M.selstarted = false;
                M.mmode |= MOUSE_SELECTING;
            }

            if(M.loostarted == true /*&& M.dragcount > 1*/)
            {
                M.loostarted = false;
                M.mmode |= MOUSE_LOOPING;
            }


            if((M.RMB == true || M.LMB == true) && M.mmode & MOUSE_SELECTING)
            {
                Selection_Drag(mouse_x, mouse_y, flags);
            }

            if((M.RMB == true || M.LMB == true) && M.mmode & MOUSE_LOOPING)
            {
                Looping_Drag(mouse_x, mouse_y, flags);
            }
        }
    }

    M.PosUpdate(mouse_x, mouse_y, flags);
}

void Process_MouseLeave(int mouse_x, int mouse_y, unsigned flags)
{
    M.LMB = M.RMB = false;
    M.PosUpdate(mouse_x, mouse_y, flags);
}

void Process_MouseEnter(int mouse_x, int mouse_y, unsigned flags)
{
    M.LMB = M.RMB = false;
    M.PosUpdate(mouse_x, mouse_y, flags);
}

void Process_KeyDown(unsigned key, unsigned flags)
{
    if(C.mode == CMode_TxtDefault || C.mode == CMode_NotePlacing || C.mode == CMode_Sliding)
    {
        Process_Key_Default(key, flags);
    }
    else
    {
        if(C.mode == CMode_ElemEdit && !(C.patt->ptype == Patt_Pianoroll))
        {
            C.curElem->ProcessKey(key, flags);
            if(C.loc == Loc_MainGrid)
            {
                R(Refresh_GridContent);
                if(aux_panel->auxmode == AuxMode_Vols || aux_panel->auxmode == AuxMode_Pans)
                {
                    R(Refresh_Aux);
                }
            }
            else if(C.loc == Loc_SmallGrid)
            {
                R(Refresh_AuxContent);
            }
        }
        else if(C.mode == CMode_ElemParamEdit)
        {
            C.curParam->ProcessKey(key, flags);

            if(C.curParam != NULL)
            {
                if(C.curParam->go == 1)
                {
                    PEdit* old = C.curParam;
                    C.curParam = C.curParam->next;

                    while((C.curParam != NULL)&&(C.curParam->bypass == true))
                    {
                        C.curParam = C.curParam->next;
                    }

                    if(C.curParam == NULL)
                    {
                        if(C.curElem->pe_mode == ParamEdit_Open)
                        {
                            old->Deactivate();
                            C.mode = CMode_ElemEdit;
                        }
                        else if(C.curElem->pe_mode == ParamEdit_Block)
                        {
                            C.curParam = old;
                            C.curParam->go = 0;
                        }
                    }
                    else if(C.curParam != old)
                    {
                        old->Deactivate();
                        C.curParam->Activate();
                        C.curParam->toLeft();
                    }
                    else
                    {
                        C.curParam = old;
                        C.curParam->go = 0;
                    }
                }
                else if(C.curParam->go == -1)
                {
                    PEdit* old = C.curParam;
                    C.curParam = C.curParam->prev;

                    while((C.curParam != NULL)&&(C.curParam->bypass == true))
                    {
                        C.curParam = C.curParam->prev;
                    }

                    if(C.curParam == NULL)
                    {
                        if(C.curElem->pe_mode == ParamEdit_Open)
                        {
                            old->Deactivate();
                            C.mode = CMode_ElemEdit;
                        }
                        else if(C.curElem->pe_mode == ParamEdit_Block)
                        {
                            C.curParam = old;
                            C.curParam->go = 0;
                        }
                    }
                    else if(C.curParam != old)
                    {
                        old->Deactivate();
                        C.curParam->Activate();
                        C.curParam->toRight();
                    }
                    else
                    {
                        C.curParam = old;
                        C.curParam->go = 0;
                    }
                }
            }

            if(C.loc == Loc_MainGrid)
            {
                R(Refresh_GridContent);
            }
            else if(C.loc == Loc_SmallGrid)
            {
                R(Refresh_AuxContent);
            }
        }
        else if(C.mode == CMode_JustParamEdit)
        {
            C.curParam->ProcessKey(key, flags);
        }
        else
        {
            Process_Key_Default(key, flags);
        }
    }

    CheckStepSeqPos();

    if(undoMan->started == true)
    {
        undoMan->StartNewGroup();
    }

    M.PosUpdate(M.mouse_x, M.mouse_y, flags);
}

void Process_Key_Default(unsigned key, unsigned flags)
{
    float qsize = C.qsize;
    switch(key)
    {
    	case key_escape:
    	{
            ToggleTextModeToNoteMode();
        }break;
		case key_left:
        {
			if(flags && kbd_shift)
			{
                C.AdvanceView(-float(ticks_per_beat), 0);
            }
            else
            {
                C.AdvanceTick(-1.0f);
            }
        }break;
		case key_right:
        {
            if(flags && kbd_shift)
            {
                C.AdvanceView(float(ticks_per_beat), 0);
            }
            else
            {
                C.AdvanceTick(1.0f);
            }
        }break;
		case key_up:
        {
            if(flags && kbd_shift)
            {
                C.AdvanceView(0, -2);
            }
            else
            {
                C.AdvanceLine(-1);
            }
        }break;
		case key_down:
        {
            if(flags && kbd_shift)
            {
                C.AdvanceView(0, 2);
            }
            else
            {
                C.AdvanceLine(1);
            }
        }break;
        case key_tab:
        {
            if(flags && kbd_shift)
            {
                C.AdvanceTick(-float(beats_per_bar));
            }
            else
            {
                C.AdvanceTick(float(beats_per_bar));
            }
        }break;
		case key_return:
		{
            C.AdvanceLine(1);
		}break;
        case key_f1:
        {
            if(!(C.loc == Loc_SmallGrid && aux_panel->workPt->ptype == Patt_Pianoroll))
            {
                if(flags & kbd_shift)
                {
                    int trknum;
                    Trk* trk;
                    if(C.trk->trkbunch != NULL)
                    {
                        trknum = C.trk->trkbunch->end_track;
                        trk = C.trk->trkbunch->trk_end;
                    }
                    else
                    {
                        trknum = C.trknum;
                        trk = C.trk;
                    }

                    if(trk->vol_lane.visible == false)
                    {
                        ShowVolLane(trk, C.patt->basePattern);
                    }
                    else
                    {
                        HideVolLane(trk, C.patt->basePattern);
                    }

                    if(C.loc == Loc_MainGrid)
                    {
                        R(Refresh_Grid);
                        R(Refresh_MixCenter);
                    }
                    else
                    {
                        R(Refresh_Aux);
                    }
                }
            }
        }break;
        case key_f2:
        {
            if(!(C.loc == Loc_SmallGrid && aux_panel->workPt->ptype == Patt_Pianoroll))
            {
                if(flags & kbd_shift)
                {
                    int trknum;
                    Trk* trk;
                    if(C.trk->trkbunch != NULL)
                    {
                        trknum = C.trk->trkbunch->end_track;
                        trk = C.trk->trkbunch->trk_end;
                    }
                    else
                    {
                        trknum = C.trknum;
                        trk = C.trk;
                    }

                    if(trk->pan_lane.visible == false)
                    {
                        ShowPanLane(trk, C.patt->basePattern);
                    }
                    else
                    {
                        HidePanLane(trk, C.patt->basePattern);
                    }

                    if(C.loc == Loc_MainGrid)
                    {
                        R(Refresh_Grid);
                        R(Refresh_MixCenter);
                    }
                    else
                    {
                        R(Refresh_Aux);
                    }
                }
            }
        }break;
        case key_f3:
        {
            if(!(flags & kbd_shift || flags & kbd_ctrl))
            {
                CP->HandleButtDown(CP->view_mixer);
            }

            if(flags & kbd_shift && C.patt != field_pattern)
            {
                if(C.patt->vol_lane_main.visible == false)
                {
                    C.patt->vol_lane_main.visible = true;
                    C.patt->track_line_end += C.patt->vol_lane_main.height;
                }
                else
                {
                    C.patt->vol_lane_main.visible = false;
                    C.patt->track_line_end -= C.patt->vol_lane_main.height;
                }
            }
        }break;
        case key_f4:
        {
            if(flags & kbd_shift && C.patt != field_pattern)
            {
                if(C.patt->pan_lane_main.visible == false)
                {
                    C.patt->pan_lane_main.visible = true;
                    C.patt->track_line_end += C.patt->pan_lane_main.height;
                }
                else
                {
                    C.patt->pan_lane_main.visible = false;
                    C.patt->track_line_end -= C.patt->pan_lane_main.height;
                }
            }
        }break;
        case key_delete:
        {
            if(M.lastclick == LastClicked_Nothing)
            {
                Element* el_d;
                Element* el = firstElem;
                while(el != NULL)
                {
                    if((el->selected == true) && el->IsPresent())
                    {
                        el_d = el;
                        el = NextElementToDelete(el_d);
                        DeleteElement(el_d, false, false);
                        continue;
                    }
                    el = el->next;
                }

                if(M.selloc == Loc_MainGrid)
                {
                    R(Refresh_GridContent);
                    if(aux_panel->isVolsPansMode())
                    {
                        R(Refresh_Aux);
                    }
                }
                else if(M.selloc == Loc_SmallGrid)
                {
                    aux_panel->RescanPatternBounds();
                }
            }
            else if(M.lastclick == LastClicked_Instrument && current_instr != NULL)
            {
                IP->RemoveInstrument(current_instr);
                R(Refresh_InstrPanel);
                if(aux_panel->isPatternMode())
                {
                    if(aux_panel->workPt->ptype == Patt_StepSeq)
                    {
                        R(Refresh_Aux);
                    }
                    else
                        R(Refresh_AuxGrid);
                }
                R(Refresh_GridContent);
            }
            else if(M.lastclick == LastClicked_Effect && aux_panel->current_eff != NULL)
            {
                aux_panel->current_eff->mixcell->mchan->drawarea->Change();
                DeleteEffect(aux_panel->current_eff);
                if(aux_panel->auxmode == AuxMode_Mixer)
                {
                    R(Refresh_Aux);
                }
            }
        }break;
        /*
        case key_page_up:
        {
            if(Octave < 8)
            {
                Octave++;
            }
        }break;
        case key_page_down:
        {
            if(Octave > 0)
            {
                Octave--;
            }
        }break;*/
		case key_page_down:
        {
            if(C.loc == Loc_MainGrid)
            {
                C.AdvanceView(0, numFieldLines);
            }
            else if(C.loc == Loc_SmallGrid)
            {
                aux_panel->v_sbar->TweakByWheel(-numAuxLines, M.mouse_x, M.mouse_y);
            }
        }break;
        case key_page_up:
        {
            if(C.loc == Loc_MainGrid)
            {
                C.AdvanceView(0, -numFieldLines);
            }
            else if(C.loc == Loc_SmallGrid)
            {
                aux_panel->v_sbar->TweakByWheel(numAuxLines, M.mouse_x, M.mouse_y);
            }
        }break;
        case key_f10:
        {
#if(RELEASEBUILD == FALSE)
            if(flags & kbd_shift && flags & kbd_ctrl && flags & kbd_alt)
            {
                ChildWindow* lwnd = MainWnd->CreateLicenseWindow();
                lwnd->Show();
            }
#endif
        }break;
    }

    R(Refresh_KeyCursor);
}

void Process_KeyState()
{
    Preview_ReleaseAll();
}

void Process_Char(char character, unsigned flags)
{
    /*
    if(character == 0x21)
    {
        if(gAux->playing == true)
        {
            gAux->HandleButtDown(gAux->playbt);
        }
        CP->HandleButtDown(CP->play_butt);
    }
    else*/ if(character == 0x20 && (C.curElem == NULL || C.curElem->type != El_TextString))
    {
        if(C.loc == Loc_SmallGrid)
        {
            if(Playing == true)
            {
                CP->HandleButtDown(CP->play_butt);
            }
            aux_panel->HandleButtDown(aux_panel->playbt);
        }
        else
        {
            if(aux_panel->playing == true)
            {
                aux_panel->HandleButtDown(aux_panel->playbt);
            }
            CP->HandleButtDown(CP->play_butt);
        }
    }
    else if(flags & kbd_ctrl)
    {
        if(character == 'a' || character == 'A')
        {
            SelectAll();
        }
        else if(character == 'c' || character == 'C')
        {
            CopySelectedElements();
        }
        else if(character == 'x' || character == 'X')
        {
            CutSelectedElements();
        }
        else if(character == 'v' || character == 'V')
        {
            PasteElements(true);
        }
        else if(character == 'z' || character == 'Z')
        {
            undoMan->UndoAction();
            UpdateNavBarsData(); // For elements adding/deleting cases

            UpdateScaledImages();
            R(Refresh_GridContent);
        }
        else if(character == 'y' || character == 'Y')
        {
            undoMan->RedoAction();
            UpdateNavBarsData(); // For elements adding/deleting cases

            UpdateScaledImages();
            R(Refresh_GridContent);
        }
        else if(character == 'd' || character == 'D')
        {
            MuteSelectedPatterns();
            ChangesIndicate();
        }
        else if(character == 'q' || character == 'Q')
        {
            UniqueSelectedPatterns();
            ChangesIndicate();
        }
        else if(character == 'o' || character == 'O')
        {
            M.DeleteContextMenu();
            LoadProject(0);
        }
        else if(character == 's' || character == 'S')
        {
            M.DeleteContextMenu();
            if(flags & kbd_shift)
                SaveProject(true);
            else
                SaveProject(false);
        }
    }
    else if(C.loc == Loc_SmallGrid && aux_panel->workPt->ptype == Patt_Pianoroll)
    {
        int note = MapKeyToNote(character, false);
        int pnum = Preview_Add(NULL, current_instr, character, note, C.patt, NULL, NULL, false, false);
        if(pnum != 0xFFFF)
        {
            PrevSlot[pnum].keybound_pianokey = true;
            //C.pianokey_pressed = true;
            //C.pianokey_num = note;
            //C.pianokey_keybound = character;
            R(Refresh_PianoKeys);
        }
    }
    else
    {
        if(C.mode == CMode_ElemEdit)
        {
            C.curElem->ProcessChar(character);
            ChangesIndicate();
            if(C.loc == Loc_SmallGrid && aux_panel->workPt->ptype == Patt_Grid)
            {
                aux_panel->RescanPatternBounds();
            }

            if(C.loc == Loc_MainGrid)
            {
                R(Refresh_GridContent);
            }
            else if(C.loc == Loc_SmallGrid)
            {
                R(Refresh_AuxContent);
            }
        }
        else if((C.mode == CMode_ElemParamEdit)||(C.mode == CMode_JustParamEdit))
        {
            jassert(C.curParam != NULL);
            C.curParam->ProcessChar(character);
            ChangesIndicate();
            if(C.curParam != NULL && C.curParam->type == Param_Note && MapKeyToNote(character, false) != -1000)
            {
                ((Note*)C.curParam)->Kreview(character);
            }
        }
        else if(C.mode == CMode_TxtDefault || 
                C.mode == CMode_NotePlacing ||
                C.mode == CMode_FastBrushMode ||
				C.mode == CMode_Sliding)
        {
            if(C.loc == Loc_SmallGrid)
            {
                /*
                if(Aux->workpt->ptype == Patt_Pianoroll)
                {
                    Instance* ii = CreateInstanceByChar(character);
                    ii->Switch2Param(NULL);

                    if(Aux->revsmp->pressed == true && ii->type == El_Samplent)
                    {
                       ((Samplent*)ii)->revB = true;
                    }
                    
                    UpdateQueuedEvPos(true);
                }
                else */

                if(aux_panel->workPt == aux_panel->blankPt)
                {
                    aux_panel->CreateNew();
                }
            }

            if(C.loc == Loc_MainGrid && C.patt == field_pattern && character == 0x2B || character == 0x5F)
            {
                if(character == 0x2B)
                    InsertTrack();
                else if(character == 0x5F)
                    DeleteTrack();

                if(C.loc == Loc_MainGrid)
                {
                    R(Refresh_GridContent);
                    R(Refresh_MixCenter);
                }
                else if(C.loc == Loc_SmallGrid)
                {
                    R(Refresh_AuxContent);
                    R(Refresh_GridContent);
                }
            }
            else if(C.mode == CMode_NotePlacing || (C.loc == Loc_SmallGrid && aux_panel->workPt->ptype == Patt_StepSeq))
            {
                if(MapKeyToNote(character, false) != -1000)
                {
                    CreateElement_Instance_byChar(character);
                }

                if(C.loc == Loc_SmallGrid)
                {
                    C.patt->basePattern->UpdateScaledImage();
                    R(Refresh_GridContent);
                }
            }
            else if(C.mode == CMode_Sliding)
            {
                C.curElem = C.PlaceSlideNote(character, flags);
            }
            else if(C.mode == CMode_TxtDefault || C.mode == CMode_FastBrushMode)
            {
                Txt* t = CreateElement_TextString();
                C.mode = CMode_ElemEdit;
                C.curElem = t;
                t->ProcessChar(character);
            }

            if(C.loc == Loc_SmallGrid && aux_panel->workPt->ptype != Patt_StepSeq)
            {
                aux_panel->RescanPatternBounds();
            }

            if(C.loc == Loc_MainGrid)
            {
                R(Refresh_GridContent);
            }
            else if(C.loc == Loc_SmallGrid)
            {
                R(Refresh_AuxContent);
            }
        }
    }

    //Preview_ReleaseAll();

    if(undoMan->started == true)
    {
        undoMan->StartNewGroup();
    }
}

void Process_WndResize(int wx, int wh)
{
    WindWidth = wx;
    WindHeight = wh;
    
    InstrPanelHeight = WindHeight - 1 - MainY1;
    GenBrowserHeight = WindHeight - 1 - MainY1;
    
	if(aux_panel != NULL && aux_panel->locked == true)
        AuxHeight = WindHeight - MainY1 - LinerHeight - 1;

	if(WindHeight - AuxHeight < GridY1)
		AuxHeight = WindHeight - GridY1;
	else
		MainY2 = WindHeight - AuxHeight;

	MainX2 = WindWidth - MixCenterWidth - 1;
    Grid2Main();
}

////////////////////////////
// Initialize sample data and add some samples
void Load_Default_Instruments()
{
    num_instrs = 0;

    AddSample("Samples\\Kicked.wav", "808.wav", NULL);
    AddSample("Samples\\Jazzy Hat.wav", "Jazzy Hat.wav", NULL);
    AddSample("Samples\\Closed Hat.wav", "Closed Hat.wav", NULL);
    AddSample("Samples\\snar_04m1.wav", "snar_04m1.wav", NULL);
    AddSample("Samples\\Crash 2.wav", "Crash 2.wav", NULL);

    LoadMetronomeSamples();

    ChangeCurrentInstrument(first_instr);

    aux_panel->PopulatePatternWithInstruments(aux_panel->workPt->basePattern);

	/*
	Add_SoundFont("basic.sf2", "basic.sf2", "sf");
    Add_SoundFont("synthgms.sf2", "synthgms.sf2", "sf1");
	*/
}

void Init_Tables()
{
    // Wavetables for basic oscillators
    wt_coeff = WT_SIZE/fSampleRate;

    wt_angletoindex = float(PI*2/WT_SIZE);

    float trianglestep = 1.0f/(WT_SIZE/4);
    float triangleval = 0;

    //float sawstep1 = 2.0f/(WT_SIZE/32);
    //float sawstep2 = 2.0f/(WT_SIZE/32*31);
    float sawstep = 2.0f/(WT_SIZE);
    float sawval = 0;

    float anglestep = wt_angletoindex;
    float angle = 0;

    for(int i = 0; i < WT_SIZE; i++)
    {
        wt_sine[i] = sin(angle);
        wt_cosine[i] = cos(angle);
        angle += anglestep;

        wt_saw[i] = sawval;
        sawval += sawstep;
        if(sawval >= 1)
        {
            sawval = -1;
        }
        //else if(sawval <= -1)
        //{
        //    sawstep = sawstep1;
        //}

        wt_triangle[i] = triangleval;
        triangleval += trianglestep;
        if(triangleval >= 1)
        {
            trianglestep = -trianglestep;
        }
        else if(triangleval <= -1)
        {
            trianglestep = -trianglestep;
        }
    }
}

void Init_InternalPlugins()
{
    // Generators
    pModulesList->AddEntry(ModuleType_Instrument, ModSubtype_Synth1, "Chaotic Synthesizer", "internal://synth1");
    pModulesList->AddEntry(ModuleType_Instrument, ModSubtype_Sinenoise, "Chaotic Sinenoise", "internal://sinenoise");

    // Effects
    pModulesList->AddEntry(ModuleType_Effect, ModSubtype_Equalizer1, "Chaotic 1-band Equalizer", "internal://eq1");
    pModulesList->AddEntry(ModuleType_Effect, ModSubtype_Equalizer3, "Chaotic 3-band Equalizer", "internal://eq3");
    pModulesList->AddEntry(ModuleType_Effect, ModSubtype_GraphicEQ, "Chaotic Graphic Equalizer", "internal://eqg");
    pModulesList->AddEntry(ModuleType_Effect, ModSubtype_XDelay, "Chaotic Delay", "internal://delay");
    pModulesList->AddEntry(ModuleType_Effect, ModSubtype_CFilter, "Chaotic Filter", "internal://filter");
    pModulesList->AddEntry(ModuleType_Effect, ModSubtype_Compressor, "Chaotic Compressor", "internal://comp");
    pModulesList->AddEntry(ModuleType_Effect, ModSubtype_Reverb, "Chaotic Reverb", "internal://reverb");
    pModulesList->AddEntry(ModuleType_Effect, ModSubtype_Chorus, "Chaotic Chorus", "internal://chorus");
    pModulesList->AddEntry(ModuleType_Effect, ModSubtype_Flanger, "Chaotic Flanger", "internal://flanger");
    pModulesList->AddEntry(ModuleType_Effect, ModSubtype_Phaser, "Chaotic Phaser", "internal://phaser");
    pModulesList->AddEntry(ModuleType_Effect, ModSubtype_WahWah, "Chaotic WahWah", "internal://wahwah");
    pModulesList->AddEntry(ModuleType_Effect, ModSubtype_Distortion, "Chaotic Distortion", "internal://distortion");
    pModulesList->AddEntry(ModuleType_Effect, ModSubtype_BitCrusher, "Chaotic BitCrusher", "internal://bitcrusher");
}

void GetStartupDir()
{
    //ExtractFilePath(Application->ExeName);
    //GetFullPathName(szWorkingDirectory, );
    char name[2222];
    GetModuleFileName(NULL, name, 2222);
    szWorkingDirectory = (char*)malloc(strlen(name));

    char* sc;
    char* last_sc;
    sc = last_sc = name;
    while(sc != NULL)
    {
        sc = strchr(sc, '\\');
        if(sc != NULL)
        {
            last_sc = sc;
            sc++;
        }
    }

    if(last_sc != name)
    {
        last_sc++;
       *last_sc = 0;
    }

    strcpy(szWorkingDirectory, name);
}

void Init_WorkData()
{

#ifdef _DEBUG
    bDebug = true;
#else
    bDebug = false;
#endif

    // Base lists
    firstCtrl = lastCtrl = NULL;
    firstParam = lastParam = NULL;
    firstVU = lastVU = NULL;
    firstPanel = lastPanel = NULL;
    first_trkdata = last_trkdata = NULL;
    first_alias_record = last_alias_record = NULL;
    firstElem = lastElem = NULL;
    bunch_first = bunch_last = NULL;
    first_active_command_trigger = last_active_command_trigger = NULL;
    first_global_active_trigger = last_global_active_trigger = NULL;
    first_eff = last_eff = NULL;
    first_rec_param = last_rec_param = NULL;
    first_instr = last_instr = NULL;
    first_instr_del = last_instr_del = NULL;
    first_active_pattern_trigger = last_active_pattern_trigger = NULL;
    first_active_playback = first_active_playback = NULL;
    first_base_pattern = last_base_pattern = NULL;

    current_instr = NULL;
    overriding_instr = NULL;

    solo_Instr = NULL;
    solo_Trk = NULL;
    solo_Mixcell = NULL;
    solo_MixChannel = NULL;

    paramIndex = 0;
    pattIndex = 1;
    instrIndex = 0;
    effIndex = 0;

    PianorollLineHeight = 10;
    delCount = 0;
    fSampleRate = SAMPLE_RATE;
    baseNote = 60;
    relOctave = 0;
    beats_per_minute = 120;       // BPM
    ticks_per_beat = 4;           // TPB =)
    beats_per_bar = 4;           // BPB =)))
    Octave = 5;
    lastFrame = 0;

    hAudioProcessMutex = CreateMutex(NULL, FALSE, NULL);

    ev0 = new Event;
    ev0->frame = 0;
    ev0->vick = 0;
    ev0->patt = NULL;
    ev0->prev = NULL;
    ev0->next = NULL;

    //////////////////////////////////////
    // Main field init
    field_pattern = new Pattern(NULL, 0, -1, -1, -1, false);
    field_pattern->start_tick = 0;
    field_pattern->end_tick = -1;
    field_pattern->trknum = 0;
    field_pattern->track_line = 0;
    field_pattern->track_line_end = 999;
    field_pattern->num_lines = 1000;
    field_pattern->folded = false;
    field_pattern->basePattern = field_pattern;
    field_pattern->CalcTiming();
    field_pattern->bunch_first = bunch_first;
    field_pattern->bunch_last = bunch_last;
    field_pattern->der_first = field_pattern->der_last = field_pattern;

    Trk* td;
    for(int tc = -1; tc <= 1000; tc++)
    {
        td = new Trk();
        td->Init(tc, true);
        Add_trkdata(td);
    }

    first_trkdata->vol->active = false;
    last_trkdata->vol->active = false;
    first_trkdata->pan->active = false;
    last_trkdata->pan->active = false;
    top_trk = first_trkdata;
    bottom_trk = last_trkdata;
    first_trkdata = first_trkdata->next;
    last_trkdata = last_trkdata->prev;

    field_pattern->first_trkdata = first_trkdata;
    field_pattern->last_trkdata = last_trkdata;
    //st->patt->first_trkdata = first_trkdata;
    //st->patt->last_trkdata = last_trkdata;

    //pbMain = new Playback();
    //pbMain->SetPlayPatt(field);
    //field->pbk = pbMain;
    field_pattern->AddPlayback();
    pbkMain = field_pattern->pbk;

    /* -- Postponed     */
    st = new StaticArea();
    st->patt->basePattern = st->patt;
    st->patt->bunch_first = bunch_first;
    st->patt->bunch_last = bunch_last;

    //////////////////////////////////////
    // Instruments panel init
    IP = new InstrPanel;

    //////////////////////////////////////
    // Various settings
    gBuffLen = DEFAULT_BUFFER_SIZE;
    pRenderer = new Renderer();
    RenderInterpolationMethod = Interpol_6dHermit;
    WorkingInterpolationMethod = Interpol_linear;

    // Init rendering configuration
    //memset(&renderConfig, 0, sizeof(RNDR_CONFIG_DATA_T));
    strcpy(renderConfig.file_name, ".\\test.wav");
    renderConfig.format = RNDR_FORMAT_WAVE;
    renderConfig.inbuff_len = 2048;
    renderConfig.quality = RNDR_QUALITY_32BIT;
    String str(T(".\\Rendered\\"));
    renderConfig.output_dir = str;

    renderConfig.q1 = 2;
    renderConfig.q2 = 5;

    pRenderer->SetConfig(&renderConfig);

    if(obtainReleasePathDir)
    {
        GetStartupDir();
    }
    else
    {
        //First, lets check how long is our working path
        int length = ::GetCurrentDirectory(0, NULL);
        szWorkingDirectory = NULL;
        if (length != 0)
        {
            szWorkingDirectory = (char*)malloc(length + 1);
        }

        if (szWorkingDirectory != NULL)
        {
            ::GetCurrentDirectory(length, szWorkingDirectory);
        }
        strcat(szWorkingDirectory, "\\");
    }

    if(szWorkingDirectory[0] >= 0x61)
        szWorkingDirectory[0] -= 0x20;
    SetCurrentDirectory(szWorkingDirectory);

    // Create required subdirectories if they don't exist
    File frendered(String((const char*)szWorkingDirectory) + T("Rendered"));
    File fprojects(String((const char*)szWorkingDirectory) + T("Projects"));
    File fplugins(String((const char*)szWorkingDirectory) + T("Plugins"));
    File fpresets(String((const char*)szWorkingDirectory) + T("Presets"));
    File fsamples(String((const char*)szWorkingDirectory) + T("Samples"));

    if(!frendered.exists())
        frendered.createDirectory();
    if(!fprojects.exists())
        fprojects.createDirectory();
    if(!fplugins.exists())
        fplugins.createDirectory();
    if(!fpresets.exists())
        fpresets.createDirectory();
    if(!fsamples.exists())
        fsamples.createDirectory();

    C.SetPattern(field_pattern, Loc_MainGrid);
    C.SetPos(8, 4);

    //////////////////////////////////////////
    // Master parameters initialisation
    memset(&mAster, 0, sizeof(Master));
    mAster.params = new ParamSet(100, 50, NULL); // scope for global params will be initialized later, in CtrlPanel
    mAster.params->vol->SetName("MasterVolume");
    mAster.params->vol->setEnvDirect(false);
    mAster.rampCounterV = 0;
    mAster.cfsV = 0;

    AddGlobalParam(mAster.params->vol);
    AddGlobalParam(mAster.params->pan);

    /////////////////////
    // Aux
    aux_panel = new Aux();
    aux_Pattern = NULL;
    pbkAux = new Playback();
    pbkAux->SetLooped(true);
    AuxPos2MainPos();

    MainY2 = WindHeight - AuxHeight;

    // Control panel
    CP = new CtrlPanel;

    genBrw = new Browser(szWorkingDirectory, Brw_Generators);
    genBrw->SetViewMask(FTYPE_UNKNOWN | FType_VST | FType_Native | FType_Wave | FType_Projects);

    mixBrw = new Browser(szWorkingDirectory, Brw_Effects);
    // Assign mixer buttons row to control panel for correct handling
    mixBrw->ShowFiles->panel = mixBrw->ShowParams->panel = mixBrw->ShowPresets->panel = mixBrw->ShowExternalPlugs->panel = mixBrw->HideTrackControls->panel = aux_panel;
    mixBrw->CurrButt = mixBrw->ShowExternalPlugs;
    //mixBrw->CurrButt->pressed = false;
    mixBrw->SetViewMask(FTYPE_UNKNOWN | FType_VST | FType_Native);

    SwitchInputMode(CMode_TxtDefault);
    // Initialize arrow cursors
    VSCursor = LoadCursor(NULL, IDC_SIZENS);
    HSCursor = LoadCursor(NULL, IDC_SIZEWE);
    ArrowCursor = LoadCursor(NULL, IDC_ARROW);
    one_divided_per_sample_rate = 1.0f/fSampleRate;
    UpdatePerBPM();
    UpdateTime(Loc_MainGrid);
    static_visible = false;
    followPos = false;
    scrolling = false;
    scrollbt = NULL;

    refresh_rate = 1500;
    refresh_counter = 0;
    auto_advance = false;

    Preview_Init();

    currPlayX = 0;
    currPlayX_f = 0;

    Metronome_ON = false;
    Playing = false;
    Rendering = false;
    Recording = false;
    firsttime_plugs_scanned = false;

    MakePatternsFat = true;
    JustClickedPattern = false;
    RememberLengths = true;
    AutoBindPatterns = false;
    PatternsOverlapping = true;
    RescanPluginsAutomatically = false;

    ProjectLoadingInProgress = false;

    numLastSessions = 0;

    undoMan = new UndoManagerC();

    M.PosUpdate(0, 0, 0);

    PrjData.Init();

    Grid2Main();

    UpdateNavBarsData();

    MakeCoolVUFallDownEffectYo();
}

//// Here interface graphical coordinates are initialized
//////////////////////////////////
void Init_Coords()
{
    tickWidth = tickWidthBack = 3;
    lineHeight = 14;
    st_tickWidth = 8;
    if(MainWnd != NULL)
    {
        WindWidth = MainWnd->getWidth();
        WindHeight = MainWnd->getHeight();
    }
    else
    {
        WindWidth = 1100;
        WindHeight = 700;
    }

    InstrPanelWidth = 163;
    InstrCellWidth = 171;
    InstrAliasOffset = 104;
    InstrCenterOffset = -1;

	AuxHeight = 130;
    NavHeight = 28;
    LinerHeight = 12;

    mixbrowse = false;
    mixcentervisible = true;

    mixHeading = 10;
    MixChanWidth = 97;
    //TrackControlsWidth = 157;
    MixCenterWidth = DefaultMixCenterWidth = 176;
    MixerWidth = MixCenterWidth;
    MixCellWidth = 115;
    MixCellBasicHeight = 34;

    MixCellGap = 18;

    MixMasterHeight = 102;
    CtrlPanelHeight = 53;
    TranspAdvance = 5;
    CtrlAdvance = 249;
    //CtrlPanelWidth = WindWidth - MixerWidth - 9;

    MainX1 = InstrPanelWidth + InstrCenterOffset;
    MainX2 = WindWidth - MixCenterWidth - 1;

	MainY1 = (CtrlPanelHeight + 1 + NavHeight + 2);		// Grid upper coordinate
	MainY2 = WindHeight - AuxHeight;

    InstrFoldedHeight = 16;
    InstrUnfoldedHeight = 33;

    GenBrowserWidth = 162;
    GenBrowserGap = 9;

    brw_heading = 27;

    SelX1 = -1;
    SelY1 = -1;
    SelX2 = -1;
    SelY2 = -1;

    Sel_Active = false;
    Num_Selected = 0;
    Num_Buffered = 0;

    BuffTick = 0;
    BuffLine = 0;

    ///////////////////////////
	// Initialize text cursor position
    OffsTick = 0;
    OffsLine = 0;
    bottomIncr = 0;

    quantsize = 4;

    keys_offset = 0;
    keys_width = 12;
}

void Init_Keymap()
{
    memset(keymap, 0, sizeof(keymap));

    keymap[VK_BACK]       = key_backspace;
    keymap[VK_TAB]        = key_tab;
    keymap[VK_CLEAR]      = key_clear;
    keymap[VK_RETURN]     = key_return;
    keymap[VK_ESCAPE]     = key_escape;

    keymap[VK_PAUSE]      = key_pause;
    keymap[VK_CLEAR]      = key_clear;
    
    keymap[VK_NUMPAD0]    = key_kp0;
    keymap[VK_NUMPAD1]    = key_kp1;
    keymap[VK_NUMPAD2]    = key_kp2;
    keymap[VK_NUMPAD3]    = key_kp3;
    keymap[VK_NUMPAD4]    = key_kp4;
    keymap[VK_NUMPAD5]    = key_kp5;
    keymap[VK_NUMPAD6]    = key_kp6;
    keymap[VK_NUMPAD7]    = key_kp7;
    keymap[VK_NUMPAD8]    = key_kp8;
    keymap[VK_NUMPAD9]    = key_kp9;
    keymap[VK_DECIMAL]    = key_kp_period;
    keymap[VK_DIVIDE]     = key_kp_divide;
    keymap[VK_MULTIPLY]   = key_kp_multiply;
    keymap[VK_SUBTRACT]   = key_kp_minus;
    keymap[VK_ADD]        = key_kp_plus;
    
    keymap[VK_UP]         = key_up;
    keymap[VK_DOWN]       = key_down;
    keymap[VK_RIGHT]      = key_right;
    keymap[VK_LEFT]       = key_left;
    keymap[VK_INSERT]     = key_insert;
    keymap[VK_DELETE]     = key_delete;
    keymap[VK_HOME]       = key_home;
    keymap[VK_END]        = key_end;
    keymap[VK_PRIOR]      = key_page_up;
    keymap[VK_NEXT]       = key_page_down;
    
    keymap[VK_F1]         = key_f1;
    keymap[VK_F2]         = key_f2;
    keymap[VK_F3]         = key_f3;
    keymap[VK_F4]         = key_f4;
    keymap[VK_F5]         = key_f5;
    keymap[VK_F6]         = key_f6;
    keymap[VK_F7]         = key_f7;
    keymap[VK_F8]         = key_f8;
    keymap[VK_F9]         = key_f9;
    keymap[VK_F10]        = key_f10;
    keymap[VK_F11]        = key_f11;
    keymap[VK_F12]        = key_f12;
    keymap[VK_F13]        = key_f13;
    keymap[VK_F14]        = key_f14;
    keymap[VK_F15]        = key_f15;
    
    keymap[VK_NUMLOCK]    = key_numlock;
    keymap[VK_CAPITAL]    = key_capslock;
    keymap[VK_SCROLL]     = key_scrollock;
}

void ClearVSTListFile()
{
    char   list_path[MAX_PATH_STRING] = {0};

    /* Check whether file-list of already scanned plugins exists */
    sprintf(list_path, "%s%s", ".\\", PLUGIN_LIST_FILENAME);

    File f(list_path);
    if(f.existsAsFile())
    {
        f.deleteFile();
    }
}

void ScanDirForVST(char *path, XmlElement* xmlList, ScanThread* thread)
{
#if (START_SCAN_VST == TRUE)
    WIN32_FIND_DATA   founddata                          = {0};
    HANDLE            shandle                            = INVALID_HANDLE_VALUE;
    bool              isGen                              = false;
    char              filename[MAX_PATH_STRING]          = {0};
    char              name[MAX_NAME_STRING]              = {0};
    ModListEntry     *pEntry                             = NULL;
    char              temp_path[MAX_PATH_STRING*5]         = {0};
    //XmlElement*       xmlPlug;

    sprintf(temp_path, "%s%s", path, "*.dll\0");
    shandle = FindFirstFile(temp_path, &founddata);
    if(shandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            sprintf(filename, "%s%s", path, founddata.cFileName);
            memset(name, 0, MAX_NAME_STRING* sizeof(char));

            if (NULL != pModulesList->SearchEntryByPath(filename))
            {
                continue;
            }

            thread->setStatusMessage("Scanning: " + String(strrchr(filename, '\\') + 1));
            if (pVSTCollector->CheckPlugin(filename, &isGen, name) == true)
            {
                pEntry = new ModListEntry;

                pEntry->subtype = ModSubtype_VSTPlugin;
                strcpy(pEntry->path, filename);

                if ((name != NULL) && (name[0] != 0))
                {
                    strcpy(pEntry->name, name);
                }
                else
                {
                    char * pName = NULL;
                    pName = strrchr(filename, '\\');
                    strcpy(pEntry->name, pName + 1);
                }

                if(true == isGen)
                {
                    pEntry->modtype = ModuleType_Instrument;
                }
                else
                {
                    pEntry->modtype = ModuleType_Effect;
                }

            }
            else
            {
                char * pName = NULL;
                pEntry = new ModListEntry;

                pEntry->subtype = ModSubtype_VSTPlugin;
                strcpy(pEntry->path, filename);
                pName = strrchr(filename, '\\');

                if (pName != NULL)
                {
                    strcpy(pEntry->name, pName + 1);
                }
                else
                {
                    strcpy(pEntry->name, "stub");
                }

                pEntry->modtype = ModuleType_Invalid;
            }

            pModulesList->AddEntry(pEntry);

            /*
            xmlPlug = new XmlElement(T("Plugin"));
            xmlPlug->setAttribute(T("Name"), String(pEntry->name));;
            xmlPlug->setAttribute(T("Filepath"), String(pEntry->path));;
            xmlPlug->setAttribute(T("Modtype"), int(pEntry->modtype));;
            xmlPlug->setAttribute(T("Subtype"), int(pEntry->subtype));;
            xmlList->addChildElement(xmlPlug);*/

        }while (FindNextFile(shandle, &founddata));
        FindClose(shandle);
    }

    sprintf(temp_path, "%s%s", path, "*.*\0");
    shandle = FindFirstFile(temp_path, &founddata);
    if(shandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            if(founddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if(strcmp(founddata.cFileName, ".") != 0 && 
                   strcmp(founddata.cFileName, "..") != 0)
                {
                    sprintf(temp_path, "%s%s%s", path, founddata.cFileName, "\\");
                    ScanDirForVST(temp_path, xmlList, thread);
                }
            }
        }while (FindNextFile(shandle, &founddata));
        FindClose(shandle);
    }
#endif 
}

void ReadPluginsFromFile()
{
    ModListEntry   *pListEntry = NULL;
    XmlElement*     xmlChild = NULL;
    File            xmlFile(T(".\\vst_fxlist.xml"));
    XmlDocument     xmlDoc(xmlFile);
    XmlElement*     xmlPlugList = xmlDoc.getDocumentElement();

    if(xmlPlugList != NULL)
    {
        forEachXmlChildElementWithTagName(*xmlPlugList, xmlChild, T("Plugin"))
        {
            pListEntry = new ModListEntry;
            xmlChild->getStringAttribute(T("Name")).copyToBuffer(pListEntry->name, MAX_NAME_STRING);
            xmlChild->getStringAttribute(T("Filepath")).copyToBuffer(pListEntry->path, MAX_PATH_STRING);
            pListEntry->modtype = ParamModule::GetModuleTypeFromString(xmlChild->getStringAttribute(T("Modtype")));
            pListEntry->subtype = ParamModule::GetModuleSubTypeFromString(xmlChild->getStringAttribute(T("Subtype")));

            pModulesList->AddEntry(pListEntry);
        }
    }
}

void Init_ScanForVST(ScanThread* thread)
{
#if(START_SCAN_VST == TRUE)
    if (pModulesList != NULL)
    {
        char            temp_path[MAX_PATH_STRING]         = {0};
        ModListEntry   *pEntry                             = NULL;
        ModListEntry   *pListEntry                         = NULL;
        ModListEntry    ListEntry                          = {0};
        bool            NeedUpdateList                     = false;
        char           *env_path                           = NULL;
        char            RegPath[MAX_PATH_STRING]           = {0};
        DWORD           rPathSize                          = MAX_PATH_STRING - 1;
        DWORD           rType                              = REG_SZ;
        HKEY            hResult                            = 0;

        {
            ReadPluginsFromFile();

            // scan local plugin folder
            //_getdcwd(drive, working_directory, MAX_PATH_STRING - 1 );
            //sprintf_s(temp_path, MAX_PATH_STRING, "%s%s", szWorkingDirectory, LOCAL_PLUGIN_FOLDER);
            sprintf_s(temp_path, MAX_PATH_STRING, "%s", LOCAL_PLUGIN_FOLDER);
            ScanDirForVST(temp_path, NULL, thread);

            // Check whether environment has VST_PATH variable set
            env_path = getenv( "VST_PATH" );
            if (env_path != NULL)
            {
                memset(temp_path, 0, sizeof(temp_path));
                sprintf(temp_path,"%s%s", env_path, "\\");

                if ( (_stricmp(temp_path, VST_EXT_PATH_1) != 0) &&
                     (_stricmp(temp_path, VST_EXT_PATH_2) != 0) )
                {
                    ScanDirForVST(temp_path, NULL, thread);
                }
            }

            // Scan special Steinberg's folder for plugins 
            //SHGetValue(HKEY_LOCAL_MACHINE, "Software\\VST","VSTPluginsPath",&rType,&RegPath, &rPathSize);
            //RegGetValue(HKEY_LOCAL_MACHINE, "Software\\VST", "VSTPluginsPath",RRF_RT_ANY,NULL, &RegPath, &rPathSize);
            RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\VST", &hResult);

            if (hResult != 0)
            {
                RegQueryValueEx(hResult, "VSTPluginsPath", NULL, NULL, (LPBYTE)&RegPath, &rPathSize);
                RegCloseKey(hResult);
            }

            if ( (rPathSize != 0) && (RegPath[0] != 0) )
            {
                memset(temp_path, 0, sizeof(temp_path));
                sprintf(temp_path,"%s%s", RegPath, "\\");

                if ( (_stricmp(temp_path, VST_EXT_PATH_1) != 0) &&
                     (_stricmp(temp_path, VST_EXT_PATH_2) != 0) )
                {
                    ScanDirForVST(temp_path, NULL, thread);
                }
            }

            // Last place to check for VST
            memset(temp_path, 0, sizeof(temp_path));
            strcpy(temp_path, VST_EXT_PATH_1);
            ScanDirForVST(temp_path, NULL, thread);

            memset(temp_path, 0, sizeof(temp_path));
            strcpy(temp_path, VST_EXT_PATH_2);
            ScanDirForVST(temp_path, NULL, thread);

            _chdir(szWorkingDirectory);

            // Phase 2: go through the list and remove old plugins which don't exist
            pEntry = pModulesList->pFirst;

            while(pEntry != NULL)
            {
                pListEntry = pEntry;
                pEntry = pEntry->Next;
                if ((strstr(pListEntry->path, "internal://") == NULL) && File(pListEntry->path).existsAsFile() == FALSE)
                {
                    pModulesList->RemoveEntry(pListEntry);
                    NeedUpdateList = true;
                }
            }

            File xmlFile(T(".\\vst_fxlist.xml"));
            // now re-write list file by new list if needed
            if (xmlFile.existsAsFile() == false || NeedUpdateList == true)
            {
                XmlElement* xmlPlugList = new XmlElement(T("ChaoticPluginsList"));

                pEntry = pModulesList->pFirst;
                while(pEntry != NULL)
                {
                    if (strstr(pEntry->path, "internal://") == NULL)
                    {
                        XmlElement* xmlPlug = new XmlElement(T("Plugin"));
                        xmlPlug->setAttribute(T("Name"), String(pEntry->name));;
                        xmlPlug->setAttribute(T("Filepath"), String(pEntry->path));;
                        xmlPlug->setAttribute(T("Modtype"), ParamModule::GetModuleTypeString(pEntry->modtype));;
                        xmlPlug->setAttribute(T("Subtype"), ParamModule::GetModuleSubTypeString(pEntry->subtype));;
                        xmlPlugList->addChildElement(xmlPlug);
                    }
                    pEntry = pEntry->Next;
                }

                xmlFile.deleteFile();
                xmlPlugList->writeToFile(xmlFile, String::empty);
            }
        }
    }
#endif
}

void ReleaseDataOnExit()
{
#ifdef USE_JUCE_AUDIO
    audioDeviceManager->removeAudioCallback(audioCallBack);
    delete audioDeviceManager;
    delete audioCallBack;
#else
    PortAudio_Deinit();
#endif

    aux_panel->current_eff = NULL;
    Eff* pEffect = first_eff;
    Eff* pTempEff = NULL;
    while (pEffect != NULL)
    {
        pTempEff = pEffect;
        pEffect = pEffect->next;
        delete pTempEff;
    }

    Instrument* pInstr = first_instr;
    Instrument* pTmpInstr = NULL;
    while (pInstr != NULL)
    {
        pTmpInstr = pInstr;
        pInstr = pTmpInstr->next;
        delete pTmpInstr;
    }

    // Erase remembered session files
    for(int si = 0; si < numLastSessions; si++)
    {
        delete lastsessions[si];
    }

    delete pVSTCollector;
    delete pModulesList;

    if (szWorkingDirectory != NULL)
    {
        free(szWorkingDirectory);
    }

    if(MainWnd != NULL)
        delete MainWnd;
}

#if (SPLASH_SCREEN == TRUE)
LRESULT CALLBACK SplashWndProc(HWND    hWnd,
                               UINT    message,
                               WPARAM  wParam,
                               LPARAM  lParam)
{
    PAINTSTRUCT ps = {0};
    HDC hdc, bdc;
    RECT rt = {0};
    HBITMAP  hbitmap;
    BITMAPINFO bmi = {0};
    switch (message)
    {
        case WM_PAINT:
            hdc = GetDC(hWnd);
            GetClientRect(hWnd, &rt);
            DrawText(hdc, splash_string, strlen(splash_string), &rt, DT_TOP | DT_CENTER| DT_SINGLELINE );
            ReleaseDC(hWnd, hdc);
            //EndPaint(hWnd, NULL);
            break;
         case WM_ERASEBKGND:
            hdc = GetDC(hWnd);
            bdc = CreateCompatibleDC(hdc);
            hbitmap = (HBITMAP)LoadImage(NULL, ".\\splash.bmp", IMAGE_BITMAP, 529, 226, LR_LOADFROMFILE);
            SelectObject(bdc, hbitmap);
            BitBlt(hdc, 0,0,529,226,bdc,0,0,SRCCOPY);
            ReleaseDC(hWnd, hdc);
            ReleaseDC(hWnd, bdc);
            DeleteObject(hbitmap);
            return 1;
            break;
         default:
            return (DefWindowProc(hWnd, message, wParam, lParam));
    }

    return (0);
}

void SendStatusToSplash(char* text, HWND hWnd)
{
    memset(splash_string, 0, strlen(splash_string));
    strcpy(splash_string, text);
    SendMessage(hWnd, WM_PAINT, 0, 0);
}
#endif

/*
LRESULT CALLBACK GLWndProc(HWND    hWnd,
                           UINT    message,
                           WPARAM  wParam,
                           LPARAM  lParam)
{
    RECT    Screen;        // используется позднее для размеров окна
    GLuint  PixelFormat;
    unsigned key;

    int sx, sy;
    int delta;

    //WINDOWPLACEMENT *lpwndpl;

	//PAINTSTRUCT ps;
	//HDC hdc;s
    switch (message)        // Тип сообщения
    {
        case WM_CREATE:
        {
            static  PIXELFORMATDESCRIPTOR pfd =
            {
                    sizeof(PIXELFORMATDESCRIPTOR),  // Размер этой структуры
            1,                              // Номер версии (?)
            PFD_DRAW_TO_WINDOW |            // Формат для Окна
            PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,             // Формат для OpenGL
											// Формат для двойного буфера
            PFD_TYPE_RGBA,                  // Требуется RGBA формат
            32,                             // Выбор 32 бит глубины цвета
            0, 0, 0, 0, 0, 0,               // Игнорирование цветовых битов (?)
            0,                              // нет буфера прозрачности
            0,                              // Сдвиговый бит игнорируется (?)
            0,                              // Нет буфера аккумуляции
            0, 0, 0, 0,                     // Биты аккумуляции игнорируются (?)
            0,                              // 16 битный Z-буфер (буфер глубины) 
            4,                              // Есть буфер траффарета
            0,                              // Нет вспомогательных буферов (?)
            PFD_MAIN_PLANE,                 // Главный слой рисования
            0,                              // Резерв (?)
            0, 0, 0                         // Маски слоя игнорируются (?)
            };

            hDC = GetDC(hWnd);      // Получить контекст устройства для окна
            PixelFormat = ChoosePixelFormat(hDC, &pfd);
                    // Найти ближайшее совпадение для нашего формата пикселов
            if(!PixelFormat)
            {
                MessageBox(0, "Can't Find A Suitable PixelFormat.", "Error", MB_OK | MB_ICONERROR);
                PostQuitMessage(0);
                        // Это сообщение говорит, что программа должна завершится
                break;  // Предтовращение повтора кода
            }

            if(!SetPixelFormat(hDC,PixelFormat,&pfd))
            {
                MessageBox(0, "Can't Set The PixelFormat.", "Error", MB_OK | MB_ICONERROR);
                PostQuitMessage(0);
                break;
            }

            hRC = wglCreateContext(hDC);
            if(!hRC)
            {
                MessageBox(0, "Can't Create A GL Rendering Context.", "Error", MB_OK | MB_ICONERROR);
                PostQuitMessage(0);
                break;
            }

            if(!wglMakeCurrent(hDC, hRC))
            {
                MessageBox(0, "Can't activate GLRC.","Error", MB_OK | MB_ICONERROR);
                PostQuitMessage(0);
                break;
            }

            GetClientRect(hWnd, &Screen);
            GL_Init(Screen.right, Screen.bottom);

            // Force redraw
            InvalidateRect(hWnd, NULL, true);
		}break;
        case WM_PAINT:
			DrawGL();
            SwapBuffers(hDC);
            ValidateRect(hWnd, NULL);
			//EndPaint(hWnd, NULL);
			break;
        case WM_CLOSE:
            ChangeDisplaySettings(NULL, 0);
            
            wglMakeCurrent(hDC, NULL);
            wglDeleteContext(hRC);
            ReleaseDC(hWnd, hDC);
            return (DefWindowProc(hWnd, message, wParam, lParam));
            //PostQuitMessage(0);
            break;
        case WM_DESTROY:
            ReleaseDataOnExit();
            PostQuitMessage(0);
            break;
        case WM_LBUTTONDOWN:
            Process_LeftButtDown(LOWORD(lParam), HIWORD(lParam), !DBClickTimeout, flags);
            if(DBClickTimeout == true)
            {
                SetTimer(hWnd, DBCLICK_TIMER, 400, NULL);
                DBClickTimeout = false;
            }
            else
            {
                DBClickTimeout = true;
            }

            InvalidateRect(hWnd, NULL, false);
            break;
        case WM_LBUTTONUP:
            Process_LeftButtUp(LOWORD(lParam), HIWORD(lParam), flags);
            Vanish_CleanUp();
            InvalidateRect(hWnd, NULL, false);
            break;
        case WM_RBUTTONDOWN:
            Process_RightButtDown(LOWORD(lParam), HIWORD(lParam), flags);
            Vanish_CleanUp();
            InvalidateRect(hWnd, NULL, false);
            break;
        case WM_RBUTTONUP:
            Process_RightButtUp(LOWORD(lParam), HIWORD(lParam), flags);
            Vanish_CleanUp();
            InvalidateRect(hWnd, NULL, false);
            break;
        case WM_MOUSEMOVE:
            Process_MouseMove(LOWORD(lParam), HIWORD(lParam), flags);
            InvalidateRect(hWnd, NULL, false);
            break;
        case WM_MOUSEWHEEL:
            delta = (int)(HIWORD(wParam));
			delta /= 120;

            Process_MouseWheel(delta, M.mouse_x, M.mouse_y, flags);
            InvalidateRect(hWnd, NULL, false);
            break;
        case WM_CHAR:
            if(CharSkip == false)
            {
                Process_Char(wParam);
                Vanish_CleanUp();
                InvalidateRect(hWnd, NULL, false);
            }
            break;
        case WM_KEYDOWN:
            keys[wParam] = TRUE;
            switch(wParam)
            {
                case VK_CONTROL:
                    flags |= kbd_ctrl;
                    break;

                case VK_SHIFT:
                    flags |= kbd_shift;
                    break;

                case VK_MENU:
                    flags |= kbd_alt;
                    break;

                default:
                    key = TranslateKey(wParam);
                    if(key != 0)
                    {
                        CharSkip = true;
                        Process_KeyDown(key, flags);

                        Vanish_CleanUp();
                        InvalidateRect(hWnd, NULL, false);
                    }
                    else
                    {
                        CharSkip = false;
                    }
                    break;
            }
            break;
        case WM_KEYUP:
            keys[wParam] = FALSE;
            switch(wParam) 
            {
                case VK_CONTROL:
                    flags &= ~kbd_ctrl;
                    break;

                case VK_SHIFT:
                    flags &= ~kbd_shift;
                    break;
            }
            //InvalidateRect(hWnd, NULL, false);
            break;
        case WM_SIZE:
            sx = LOWORD(lParam);
            sy = HIWORD(lParam);
            Process_WndResize(sx, sy);

            ResizeGL(sx, sy);
            //InvalidateRect(hWnd, NULL, false);
            break;
        case WM_ACTIVATE:
            if( (M.LMB == true) &&
                (LOWORD(wParam) == WA_INACTIVE) )
            {
                Process_LeftButtUp(0, 0, 0);
            }
			return (DefWindowProc(hWnd, message, wParam, lParam));
            break;
        case WM_TIMER:
        {
            switch(wParam)
            {
                case DBCLICK_TIMER:
                {
                    DBClickTimeout = true;
                    KillTimer(hWnd, DBCLICK_TIMER);
                }break;

                case HINT_TIMER:
                {
                    KillTimer(hWnd, HINT_TIMER);
                    HintTimer = 0;
                    if((M.active_ctrl != NULL) && (M.active_ctrl == MouseControl))
                    {
                        M.hint_active = true;
                        InvalidateRect(hWnd, NULL, false);
                    }
                }break;

                case SCROLL_TIMER:
                {
                    if(scrolling && scrollbt != NULL && scrollbt->panel != NULL)
                    {
                        scrollbt->panel->HandleButtDown(scrollbt);
                        SetTimer(hWnd, SCROLL_TIMER, 100, NULL);
                    }
                    else
                    {
                        KillTimer(hWnd, SCROLL_TIMER);
                    }
                }break;
            }
        }break;
        case CLEANUP_MESSAGE:
        {
			Element* el_d;
            Element* el = firstElem;
            while(el != NULL)
            {
                if(el->to_be_deleted == true)
                {
					el_d = el;
                    while(el != NULL)
                    {
                        if(el->to_be_deleted == true)
                            el = el->next;
						else
							break;
                    }

                    DeleteElementCommon(el_d);
					continue;
                }
                el = el->next;
            }
            need_cleanup = false;
            InvalidateRect(hWnd, NULL, false);
        }break;
        case WM_VST_PLUGEDITOR_CLOSED:
        {
            Instrument *pInstr = first_instr;

            while (pInstr != NULL)
            {
                if (pInstr->type == Instr_Plugin)
                {
                    Generator *pGen = (Generator*)pInstr;
                    CVSTPlugin* pSentPlug = (CVSTPlugin*)lParam;

                    if (pGen->pEff->pPlug == pSentPlug)
                    {
                        pGen->window->Click();
                        //while ()
                        break;
                    }
                }

                pInstr = pInstr->next;
            }
        }break;
        default:
        return (DefWindowProc(hWnd, message, wParam, lParam));
    }

    //TrackMouseEvent();
    return (0);
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, 
                LPSTR lpCmdLine,int nCmdShow)
{
    hInst = hInstance;

    MSG             msg;    // Структура сообщения Windows
    WNDCLASS        wc;		// Структура класса Windows для установки типа окна

    // Register Main Window Class
    wc.style                =  CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    wc.lpfnWndProc          = (WNDPROC)GLWndProc;
    wc.cbClsExtra           =  0;
    wc.cbWndExtra           =  0;
    wc.hInstance            =  hInstance;
    wc.hIcon                =  LoadIcon(0, IDI_APPLICATION);
    wc.hCursor              =  LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground        =  NULL;
    wc.lpszMenuName         =  NULL;
    wc.lpszClassName        = "OpenGL WinClass";

    if(!RegisterClass(&wc))
    {
        MessageBox(0,"Failed To Register The Window Class.","Error",MB_OK|MB_ICONERROR);
        return FALSE;
    }

    // Create Main Window
    hWnd = CreateWindow("OpenGL WinClass",
                        "Chaotic - Project1", // Заголовок вверху окна
                         WS_POPUP | WS_SYSMENU |
                         WS_CLIPCHILDREN |
                         WS_CLIPSIBLINGS |
                         WS_CAPTION |
                         WS_MINIMIZEBOX |
                         WS_OVERLAPPED |
                         WS_THICKFRAME | WS_MAXIMIZEBOX,
                         100, 50,					// Позиция окна на экране
                         1100, 700,					// Ширина и высота окна
                         NULL, 
                         NULL,
                         hInstance,
                         NULL);

    if(!hWnd)
    {
        MessageBox(0, "Window Creation Error.", "Error", MB_OK|MB_ICONERROR); 
        return FALSE;
    }
    else
    {
        gHwnd = hWnd;
    }

//    DEVMODE dmScreenSettings;                       // Режим работы

//    memset(&dmScreenSettings, 0, sizeof(DEVMODE));          // Очистка для хранения установок
//    dmScreenSettings.dmSize = sizeof(DEVMODE);              // Размер структуры Devmode
//    dmScreenSettings.dmPelsWidth    = 800;                  // Ширина экрана
//    dmScreenSettings.dmPelsHeight   = 600;                  // Высота экрана
//    dmScreenSettings.dmFields       = DM_PELSWIDTH | DM_PELSHEIGHT; // Режим Пиксела
//    ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

    HWND splash_hwnd = 0;
    DEVMODE dmScreenSettings = {0};

#if (SPLASH_SCREEN == TRUE)
    WNDCLASS        splash_wc;

    // Register Splash Screen Window
    splash_wc.style                =  CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    splash_wc.lpfnWndProc          = (WNDPROC)SplashWndProc;
    splash_wc.cbClsExtra           =  0;
    splash_wc.cbWndExtra           =  0;
    splash_wc.hInstance            =  NULL;
    splash_wc.hIcon                =  NULL;
    splash_wc.hCursor              =  NULL;
    splash_wc.hbrBackground        =  (HBRUSH)(COLOR_WINDOWFRAME);
    splash_wc.lpszMenuName         =  NULL;
    splash_wc.lpszClassName        = "Splash WinClass";

    if(!RegisterClass(&splash_wc))
    {
        MessageBox(0,"Failed To Register The Splash Class.","Error", MB_OK|MB_ICONERROR);
        return FALSE;
    }

    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmScreenSettings);

    // Create the Splash Screen window in the middle of the display

    splash_hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
                                 "Splash WinClass",
                                 "Sound Producer From The Space",
                                 WS_POPUP| WS_OVERLAPPED,
                                 (dmScreenSettings.dmPelsWidth - 300)/2,
                                 (dmScreenSettings.dmPelsHeight - 450)/2,
                                 300, 450,
                                 hWnd,
                                 NULL,
                                 hInstance,
                                 NULL);

    if (!splash_hwnd)
    {
        return FALSE;
    }

    // Show Splash screen
    ShowWindow(splash_hwnd, SW_SHOWNORMAL);
    UpdateWindow(splash_hwnd);
    SetFocus(splash_hwnd);
#endif // SPLASH_SCREEN == TRUE

//-------------------------------------------------------------------------------------------------
//   Now here is the section of any start-up initialization. Put any initialization here so that
//   splash screen will be displayed while program is initializing any necessary staff.
//-------------------------------------------------------------------------------------------------

    //Initialize PortAudio library and start streaming
    SendStatusToSplash("Initializing audio engine...", splash_hwnd);
    PaError err = Init_PortAudio();

    SendStatusToSplash("Initializing key maps...", splash_hwnd);
    Init_Keymap();
    SendStatusToSplash("Loading coords...", splash_hwnd);
    Init_Coords();
    SendStatusToSplash("Initializing internal panels...", splash_hwnd);
    Init_WorkData();

    Init_Wavetables();

    CursorUpdate();
    PlaceCursor();

    SendStatusToSplash("Initializing VST engine...", splash_hwnd);
    // Initialize VST host module
    pVSTCollector = new VSTCollection(hWnd);
    // Create list for all registered plugins and internal FX
    pPluginList = new CPluginList(NULL);

    // initialize all internal FX (generators, effects) and add them to the list
    SendStatusToSplash("Loading internal FX ...", splash_hwnd);
    Init_Aliases();
    // Scan VST folder for all plugins and add them to the list
    SendStatusToSplash("Scaning for external VST plug-ins...", splash_hwnd);
    Init_ScanForVST();

    SendStatusToSplash("Starting audio stream routing...", splash_hwnd);
    Start_PortAudio();
    // display Main window begind splash screen
    ShowWindow(hWnd, SW_SHOWMAXIMIZED);
    UpdateWindow(hWnd);

//-------------------------------------------------------------------------------------------------
//    End of start-up initialization section
//-------------------------------------------------------------------------------------------------

#if (SPLASH_SCREEN == TRUE)
    // Destroy splash screen
    DestroyWindow(splash_hwnd);
#endif

    // Set focus to Main Window
    SetFocus(hWnd);
    while (1)
    {
        while(GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return msg.wParam;
        if (keys[VK_ESCAPE]) SendMessage(hWnd, WM_CLOSE, 0, 0);    // Если ESC - выйти
    }

    KillGLWindow();
}
*/


#if (SPLASH_SCREEN == TRUE)
static void MakeASplash()
{
    DEVMODE dmScreenSettings = {0};
    WNDCLASS        splash_wc;

    /* Register Splash Screen Window */
    splash_wc.style                =  CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    splash_wc.lpfnWndProc          = (WNDPROC)SplashWndProc;
    splash_wc.cbClsExtra           =  0;
    splash_wc.cbWndExtra           =  0;
    splash_wc.hInstance            =  NULL;
    splash_wc.hIcon                =  NULL;
    splash_wc.hCursor              =  NULL;
    splash_wc.hbrBackground        =  (HBRUSH)(COLOR_WINDOWFRAME);
    splash_wc.lpszMenuName         =  NULL;
    splash_wc.lpszClassName        = "Splash WinClass";

    if(!RegisterClass(&splash_wc))
    {
        MessageBox(0,"Failed To Register The Splash Class.","Error", MB_OK|MB_ICONERROR);
        return;
    }

    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmScreenSettings);

    /* Create the Splash Screen window in the middle of the display */

    splash_hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
                                 "Splash WinClass",
                                 "Sound Producer From The Space",
                                 WS_POPUP| WS_OVERLAPPED,
                                 (dmScreenSettings.dmPelsWidth - 529)/2,
                                 (dmScreenSettings.dmPelsHeight - 226)/2,
                                 529, 226,
                                 hWnd,
                                 NULL,
                                 NULL,
                                 NULL);

    if (!splash_hwnd)
    {
        return;
    }

    /* Show Splash screen */
    ShowWindow(splash_hwnd, SW_SHOWNORMAL);
    UpdateWindow(splash_hwnd);
    SetFocus(splash_hwnd);
}
#endif /* SPLASH_SCREEN == TRUE */

//==============================================================================
/** This is the application object that is started up when Juce starts. It handles
    the initialisation and shutdown of the whole application.
*/
class AwfulApplication : public JUCEApplication
{
    /* Important! NEVER embed objects directly inside your JUCEApplication class! Use
       ONLY pointers to objects, which you should create during the initialise() method
       (NOT in the constructor!) and delete in the shutdown() method (NOT in the
       destructor!)

       This is because the application object gets created before Juce has been properly
       initialised, so any embedded objects would also get constructed too soon.
   */
    AwfulWindow*    awfulWindow;
    ConfigWindow*   configWindow;
    RenderWindow*   renderWindow;
    SampleWindow*    sampleWindow;

public:
    //==============================================================================
    AwfulApplication()
        : awfulWindow (0)
    {
        // NEVER do anything in here that could involve any Juce function being called
        // - leave all your startup tasks until the initialise() method.
    }

    ~AwfulApplication()
    {
        // Your shutdown() method should already have done all the things necessary to
        // clean up this app object, so you should never need to put anything in
        // the destructor.

        // Making any Juce calls in here could be very dangerous...
    }

    //==============================================================================
    void initialise (const String& commandLine)
    {
#if (SPLASH_SCREEN == TRUE)
       // MakeASplash();
#endif
        // main window
        awfulWindow = new AwfulWindow();
        awfulWindow->setVisible(true);
        awfulWindow->setFullScreen(true);
        awfulWindow->setResizable(true, false);

        hWnd = (HWND)awfulWindow->getWindowHandle();
        gHwnd = hWnd;
        MainWnd = awfulWindow;

        //xmlPlugList = NULL;
        //xmlPlugList = new XmlElement(T("Chaotic plugins list"));
        /*
        sprintf(list_path_xml, "%s%s", ".\\", PLUGIN_LIST_FILENAME_XML);

        xmlPlugList = new XmlElement(T("Chaotic plugins list"));
        xmlFile = File(list_path_xml);

        if(xmlFile.existsAsFile() == false)
        {
            xmlFile.create();
        }*/


        Init_Fonts();

        Init_Images();

        Init_Keymap();
        Init_Coords();
        Init_WorkData();
        Init_Tables();

        AssignLoadedImages();

        // Load settings from file, if it exists
        XmlElement* xmlAudio = NULL;
        File sFile(T(".\\settings.xml"));
        if(sFile.existsAsFile())
        {
            XmlDocument sDoc(sFile);
            XmlElement* xmlSettings = sDoc.getDocumentElement();
            //MakePatternsFat = xmlSettings->getBoolAttribute(T("MakePatternsFat"), MakePatternsFat);
            RememberLengths = xmlSettings->getBoolAttribute(T("RememberLengths"), RememberLengths);
            AutoBindPatterns = xmlSettings->getBoolAttribute(T("AutoBindPatterns"), AutoBindPatterns);
            PatternsOverlapping = xmlSettings->getBoolAttribute(T("PatternsOverlapping"), PatternsOverlapping);
            RescanPluginsAutomatically = xmlSettings->getBoolAttribute(T("RescanPluginsAutomatically"), RescanPluginsAutomatically);
            WorkingInterpolationMethod = xmlSettings->getIntAttribute(T("Interpolation"), WorkingInterpolationMethod);
            gBuffLen = xmlSettings->getIntAttribute(T("BufferSize"), gBuffLen);

            XmlElement* xmlRender = xmlSettings->getChildByName(T("RenderSettings"));
            if(xmlRender != NULL)
            {
                // Read renderer settings
                renderConfig.format = (RNDR_FORMAT_T)xmlRender->getIntAttribute(T("Format"), renderConfig.format);
                renderConfig.quality = xmlRender->getIntAttribute(T("Quality"), renderConfig.quality);
                renderConfig.q1 = xmlRender->getIntAttribute(T("Q1"), renderConfig.q1);
                renderConfig.q2 = xmlRender->getIntAttribute(T("Q2"), renderConfig.q2);
                renderConfig.inbuff_len = xmlRender->getIntAttribute(T("InBuffLen"), renderConfig.inbuff_len);
                renderConfig.output_dir = xmlRender->getStringAttribute(T("OutDir"), renderConfig.output_dir);
                RenderInterpolationMethod = xmlRender->getIntAttribute(T("Interpolation"), RenderInterpolationMethod);
                pRenderer->SetConfig(&renderConfig);
            }

            numLastSessions = 0;
            String elname = "Session" + String(numLastSessions);
            XmlElement* xmlSession = NULL;
            xmlSession = xmlSettings->getChildByName(elname);
            while(xmlSession != NULL)
            {
                lastsessions[numLastSessions] = new File(xmlSession->getStringAttribute(T("FilePath"), T("")));
                numLastSessions++;
                elname = "Session" + String(numLastSessions);
                xmlSession = xmlSettings->getChildByName(elname);
            }

            xmlAudio = xmlSettings->getChildByName(T("DEVICESETUP"));
        }

        SortLastSessions(NULL);

        C.PlaceCursor();

        cursCopy = new MouseCursor(*img_curscopy, 0, 0);
        cursClone = new MouseCursor(*img_cursclone, 0, 0);
        cursSlide = new MouseCursor(*img_cursslide, 0, 0);
        cursBrush = new MouseCursor(*img_cursbrush, 0, 0);
        cursSelect = new MouseCursor(*img_cursselect, 0, 0);

        pMidiHost = new AwfulMidiWrapper();
        //pMidiHost->OpenMidiSource(0);
        midiCallBack = new AwfulMidiInputCallback(pMidiHost);

        /* Initialize VST host module */
        pVSTCollector = new VSTCollection(hWnd);
        /* Create list for all registered plugins and internal FX */
        pModulesList = new CPluginList(NULL);

        Init_InternalPlugins();
        
        Load_Default_Instruments();

        ResetChangesIndicate();

        MainWnd->UpdateTitle();

        // Now configure and start audio device
#ifdef USE_JUCE_AUDIO
        audioDeviceManager = new CAudioDeviceManager;
        audioDeviceManager->initialise (0, // number of input channels
                                        2, // number of output channels
                                        xmlAudio,
                                        true  // select default device on failure
                                        );
        audioCallBack = new AudioCallback;

        audioDeviceManager->addAudioCallback(audioCallBack);
        audioDeviceManager->addMidiInputCallback("", midiCallBack);

        if(xmlAudio == NULL)
        {
            CAudioDeviceManager::AudioDeviceSetup setup;
            audioDeviceManager->getAudioDeviceSetup(setup);
            setup.bufferSize = gBuffLen;
            audioDeviceManager->setAudioDeviceSetup(setup, false);
        }
#else
        PaError err = PortAudio_Init();
#endif

        configWindow = new ConfigWindow();
        renderWindow = new RenderWindow();
        sampleWindow = new SampleWindow();

        awfulWindow->AddChild(configWindow);
        awfulWindow->AddChild(renderWindow);
        awfulWindow->AddChild(sampleWindow);

        ConfigWnd = configWindow;
        RenderWnd = renderWindow;
        SmpWnd = sampleWindow;

        MC->listen->UpdatePosData();
        MC->listen->UpdateRects();
        MC->resized();

        LookAndFeel& lkf = awfulWindow->getLookAndFeel();
        lkf.setUpDownImages(img_closew1, img_closew2);

#if (SPLASH_SCREEN == TRUE)
        /* Destroy splash screen */
        //  DestroyWindow(splash_hwnd);
#endif

        // Reset changes flag
        ResetChangesIndicate();

        String arg = getCommandLineParameters();
        if(arg.length() > 0)
        {
            // Remove quotes first, as they can prevent from loading
            char* fn = (char*)malloc(arg.length());
            arg.copyToBuffer(fn, arg.length());
    		char* k = fn;
            char* a = strchr(fn, '\"');
    		while(a != NULL)
    		{
    			if(a == fn)
    				k += 1;
    		   *a = 0;
    			a = strchr(k, '\"');
    		}

            File f(k);
            if(f.exists())
            {
                LoadProject(&f);
            }
        }

        /*  ..and now return, which will fall into to the main event
            dispatch loop, and this will run until something calls
            JUCEAppliction::quit().
        */
    }

    void shutdown()
    {
        // clear up..
        ReleaseDataOnExit();
    }

    //==============================================================================
    const String getApplicationName()
    {
        return T("Chaotic Music Maker");
    }

    const String getApplicationVersion()
    {
        return T("1.00");
    }

    bool moreThanOneInstanceAllowed()
    {
        return true;
    }

    void anotherInstanceStarted(const String& commandLine)
    {
        AlertWindow w (T("Restriction warning"),
                       commandLine,
                       AlertWindow::WarningIcon);
        w.setSize(122, 55);
        w.addButton (T("OK"), 1, KeyPress (KeyPress::returnKey, 0, 0));
        int result = w.runModalLoop();

        File f(commandLine);
        if(f.exists())
        {
            LoadProject(&f);
        }
    }

    void systemRequestedQuit()
    {
        CheckSave();
        SaveSettings();
    }
};

//==============================================================================
// This macro creates the application's main() function..
START_JUCE_APPLICATION(AwfulApplication)
;
