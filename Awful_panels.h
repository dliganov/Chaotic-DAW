#ifndef PANELS_H
#define PANELS_H

#include "awful.h"
#include "awful_audio.h"
#include "awful_effects.h"
#include "awful_params.h"
#include "awful_instruments.h"
#include "awful_graphjuce.h"


extern void ChangeAuxLane(LaneType lt);
extern void EffFoldClick(Object* owner, Object* self);
extern void EffBypassClick(Object* owner, Object* self);

class Panel : public Object
{
public:
    Control*    first_ctrl;
    Control*    last_ctrl;

    PanelType   type;
    bool        active;
    bool        locked;

    int         x;
    int         y;
    int         width;
    int         height;

    int         x1r;
    int         y1r;
    int         x2r;
    int         y2r;

    int         num_visible;

    DrawArea*   drawarea;

    Panel*      prev;
    Panel*      next;

    Panel();
    virtual ~Panel();
    void            AddControl(Control * ct);
    void            RemoveControl(Control * ctrl);
    virtual void    HandleTweak(Control* ct, int mouse_x, int mouse_y) {}
    virtual bool    CheckPos4Control(int mouse_x, int mouse_y) {return false;}
    virtual bool    CheckMouse(int mouse_x, int mouse_y) {return false;}
    virtual void    Update(PEdit* pe);
    virtual void    Update();
    virtual void    HandleButtUp(Butt* bt);
    virtual void    HandleButtDown(Butt* bt);
    virtual void    HandleToggle(Toggle* tg);
    virtual void    Click(int mouse_x, int mouse_y, bool dbclick, unsigned flags) {};
};

class CtrlPanel : public Panel
{
public:
    SliderBase*     MVol;
    Knob*           MPan;

    SliderZoom*     slzoom;

    ButtFix*        play_butt;
    Butt*           stop_butt;
    ButtFix*        Record;

    Butt*           Go2Home;
    Butt*           Go2End;
    Butt*           FFwd;
    Butt*           Rewind;

    Butt*           Config;
    Butt*           Render;

    Butt*           IncrScale;
    Butt*           DecrScale;
    Butt*           DefaultScaleMax;
    Butt*           DefaultScaleMin;

    Butt*           Back;
    Butt*           Forth;

    Butt*           IncrOctave;
    Butt*           DecrOctave;

    ButtFix*        Waves;
    ButtFix*        Spectrum;

    ButtFix*        MetroMode;
    ButtFix*        NoteMode;
    ButtFix*        SlideMode;
    ButtFix*        UsualMode;
    ButtFix*        FastMode;
    ButtFix*        BrushMode;
    // Current mode button
    ButtFix*        CurrentMode;

    ButtFix*        BtFollowPlayback;

    DropMenu*       qMenu;

    ScrollBard*     TextBar;
    Scope           scope;

    Toggle*         autoadv;

    // pattern mode toggles
    Toggle*         pattmode;
    bool            copymode;

    MenuItem*       File;
    MenuItem*       Edit;
    MenuItem*       Help;

    Menu*           MainMenu;
    Menu*           FileMenu;
    Menu*           EditMenu;
    Menu*           ViewMenu;
    Menu*           HelpMenu;

    Numba*          bpmer;
    Numba*          tpber;
    Numba*          bpber;
    Numba*          octaver;

    Butt*           AccGenBrowsa;
    Butt*           AccInstrPanel;

    Butt*           minimize;
    Butt*           maximize;
    Butt*           close;

    int             xcc;

    int             tmx;
    int             tmy;
    int             tmw;
    int             tmh;

    DrawArea*       tmarea;

    Butt*           AccBasicGrid;
    Butt*           AccBasicPatt;
    Butt*           AccBasicMixer;
    ButtFix*        ShowTrackControls;

    Butt*           HotKeys;
    ChildWindow*    HKWnd;
    ChildWindow*    AboutWnd;

    Butt*           view_mixer;
    AuxMode         lastauxmode;

    Butt*           up;
    Butt*           down;

    CtrlPanel();
    void HandleToggle(Toggle* tg);
    void HandleButtDown(Butt* bt);
    void HandleButtUp(Butt* bt);
    void CheckVisibility(int xc);
    void CheckControlVisibility(Control* ct);
    void HandleTweak(Control* ct, int mouse_x, int mouse_y);
    void PosToHome();
    void PosToEnd();
    void UpdateSessionItems();
};

class Browser : public Panel
{
public:
    BrwType     btype;
    char        path[MAX_NAME_STRING];
    char        samplespath[MAX_NAME_STRING];
    char        projectspath[MAX_NAME_STRING];
    char        rootpath[MAX_NAME_STRING];
    char*       currpath;
    bool        at_root;
    char        lastdir[MAX_NAME_STRING];
    BrwEntry*   entry_first;
    BrwEntry*   entry_last;
    FileData*   current_fd;
    PresetData* current_prd;
    FileData*   files;
    FileData*   plugins;
    float       main_offs;
    int         entry_height;   // for use in scrolling (e.g. mousewheel)
    int         current_index;
    int         current_num_entries;

    ButtFix*    ShowFiles;
    ButtFix*    ShowProjects;
    ButtFix*    ShowExternalPlugs;
    ButtFix*    ShowNativePlugs;
    ButtFix*    ShowSamples;
    ButtFix*    ShowParams;
    ButtFix*    ShowPresets;
    ButtFix*    HideTrackControls;
    ButtFix*    CurrButt;

    ScrollBard* sbar;
    BrwMode     brwmode;
    DirType     dirtype;

    unsigned int    viewmask;

    TString*    sortstr;
    char        sortlow[MAX_NAME_STRING];

    Instrument* prevInstr;

    Browser(const char* dirpath, BrwType t);
    virtual ~Browser();
    void AddEntry(BrwEntry* be);
    void AddDelimiter();
    void AddDelimiter(const char* dstr);
    void SetCurrentFileDataByIndex(int index);
    void SetCurrentFileDataByName(char* name);
    void SetCurrentPresetDataByIndex(int index);
    void SetCurrentListIndex(int index);
    void ActivateEntry(FileData* fd);
    void ActivateEntry(PresetData* prd);
    FileData* GetFileDataByIndex(int index);
    PresetData* GetPresetDataByIndex(int index);
    void Update();
    void Clean();
    void UpdateParamsData();
    void UpdatePresetData();
    void UpdateFileData();
    void HandleButtDown(Butt* bt);
    void HandleButtUp(Butt* bt);
    void MouseClick(int brwindex, bool dbclick, bool down, bool left = true);
	void UpdateCurrentHighlight();
    void SetViewMask(unsigned int vmask);
    void Enable();
    void Disable();
    bool ItemIsVisibleWithSorting(char* itemname);
    void ResetSortString();
    void PreviewSample(FileData* fdi);
    void GrabParamsFromModule(ParamModule* pmodule, char* name);
    void ProcessModuleParams(ParamModule* pmodule, char* name);
    void ScanDirForSamples(char *path);
};

class Mixcell : public Panel
{
public:
    bool        auxcell;

    int         x;
    int         y;
    int         x1;
    int         y1;

    Level*      lev;
    SliderParam* level_slider;
    SliderBase*  vol_slider;
    SliderHPan*  pan_slider;
    InPin*      inpin;
    OutPin*     outpin;
    TString*    mixstr;
    char        indexstr[5];

    Eff*        effect;
    Scope       scope;

    Mixcell*    outcell;
    Mixcell*    in_first;
    Mixcell*    in_last;
    Mixcell*    in_prev;
    Mixcell*    in_next;
    Mixcell*    working_prev;
    Mixcell*    working_next;
    Mixcell*    initial_prev;
    Mixcell*    initial_next;

    Mixcell*    check_next;

    Toggle*     solo_toggle;
    Toggle*     mute_toggle;

    ParamSet*   params;

    Toggle*     fold_toggle;
    bool        folded;

    Toggle*     bypass_toggle;
    bool        bypass;

    int         num_inputs;
    int         active_inputs_count;
    int         num_active_inputs;

    float       in_buff[MAX_BUFF_SIZE*2];
    float       out_buff[MAX_BUFF_SIZE*2];
    float       auxbuff[256];

    bool        visible;
    bool        buff_new;
    bool        queued;
    bool        workqueued;
    bool        working;

    int         col;
    int         row;

    int         height;

    float       VULC;
    float       VURC;

    // distinguishing flags
    bool        master;
    bool        grouper;

    // antialiasing helper
    bool        wasmuted;
    int         mutecount;

    MixChannel* mchan;

    Mixcell();
    void UpdateIndexStr();
    void Enable();
    void Disable();
    void HandleToggle(Toggle* tg);
};

class InstrPanel : public Panel
{
public:
    int             rx1;
    int             ry1;
    int             rx2;
    int             ry2;
    int             main_offs;
    ScrollBard*     sbar;
    bool            sbgodown;
    Butt*           fold_all;
    Butt*           expand_all;
    Butt*           shut_all_windows;
    Butt*           edit_autopattern;

    InstrPanel();
    void HandleButtDown(Butt* bt);
    void HandleButtUp(Butt* bt);
    void Click(int mouse_x, int mouse_y, bool dbclick, unsigned flags);
    void GoDown();
    void CheckGoUp();
    int GetFullLHeight();
    Instrument* AddInstrumentFromBrowser(FileData* fdi, bool bottom);
    void RemoveInstrument(Instrument* i);
    void CloneInstrument(Instrument* i);
    Instrument* GetInstrByIndex(int index);
	void UpdateIndices();
	void EditInstrumentAutopattern(Instrument* instr);
};

class SendSingle
{
    Parameter*  amount;
    Knob*       r_amount;

    SendSingle();
    ~SendSingle();
};

class MixChannel : public Panel
{
public:
    bool        visible;
    char        indexstr[15];

    Mixcell*    mc_first;
    Mixcell*    mc_last;
    Mixcell*    mc_main;

    ParamSet*   params;

    int         mutecount;

    bool        send;
    bool        master;

    int         col;
    int         voffs;
    int         cheight; // content height

    // visible area y-coordinates
    int         ry1;
    int         ry2;

    Eff*        first_eff;
    Eff*        last_eff;

    Butt*       kup;
    Butt*       down;

    Parameter*  amount1;
    Knob*       r_amount1;
    Parameter*  amount2;
    Knob*       r_amount2;
    Parameter*  amount3;
    Knob*       r_amount3;

    DigitStr*   routestr;

    float       in_buff[MAX_BUFF_SIZE*2];
    float       out_buff[MAX_BUFF_SIZE*2];

    VU*         p_vu;

    float       rampCounterV;
    float       cfsV;

    int         incount;
    int         workcount;
    bool        processed;


    MixChannel();
    ~MixChannel();
    void Disable();
    void Enable();
    void AddEffect(Eff* eff);
    void RemoveEffect(Eff* eff);
    void UpdateIndexStr(int n);
    void Process(int num_frames, float* outbuff);
    void HandleButtUp(Butt* bt);
    void DoSend(float * sendbuff, float amount, int num_frames);
    void Save(XmlElement* xmlChanNode);
    void Load(XmlElement* xmlChanNode);
};

class Aux : public Panel
{
public:
    int         note_offset;
    int         prev_mouse_x;
    int         prev_mouse_y;
    int         keywidth;
    int         key_height;
    int         patt_height;
    int         mix_height;
    int         volpan_height;
    int         last_height;
    int         last_mixcenterwidth;
    AuxMode     auxmode;
    PattType    last_ptype;
    AuxMode     last_vpmode;
    float       tickWidth;
    float       tickWidthBack;
    int         lineHeight;
    // Aux tick offset
    float       OffsTick;
    // Aux line offset
    int         OffsLine;
    // Aux pattern area
    int         pattx1;
    int         pattx2;
    int         patty1;
    int         patty2;
    // Aux pattern grid drawing coordinates (they do not correspond to grid coordinates GridXS1 GridYS1 and so on).
    int         ptdrawx1;
    int         ptdrawx2;
    int         ptdrawy1;
    int         ptdrawy2;
    // *** Magic variables. Your should be a very experienced sorcerer to deal with them.
    int         eux;
    int         aeux;
    int         bottomincr;
    bool        offset_correction;
    // ************************
    int         curr_play_x;
    double      curr_play_x_f;
    float       frames_per_pixel;
    // playback frame absolute
    bool        playing;
    long        ev_count_down;
    Event*      queued_ev;
    // Controls
    float       quantsize;
    DropMenu*   qMenu;
    Pattern*    blankPt;
    Pattern*    workPt;
    Butt*       Back;
    Butt*       Forth;
    Butt*       Up;
    Butt*       Down;
    ButtFix*    playbt;
    Butt*       stopbt;
    ButtFix*    Thick;
    ButtFix*    Thin;
    ScrollBard* v_sbar;
    ScrollBard* h_sbar;
    Butt*       IncrScale;
    Butt*       DecrScale;
    Butt*       DefaultScaleMax;
    Butt*       DefaultScaleMin;
    ButtFix*    Vols1;
    ButtFix*    Pans1;
    ButtFix*    Pitch1;
    LaneType    ltype;
    ButtFix*    revsmp;
    ButtFix*    Vols;
    ButtFix*    Pans;
    ButtFix*    PtUsual;
    ButtFix*    PtPianorol;
    ButtFix*    PtStepseq;
    ButtFix*    PtAuto;
    ButtFix*    PtMix;
    ButtFix*    CurrPt;
    ButtFix*    PR8;
    ButtFix*    PR10;
    ButtFix*    PR12;
    ButtFix*    CurrPR;
    ButtFix*    PattExpand;
    bool        auto_switch;
    Toggle*     autoswitch;
    bool        relative_oct;
    Toggle*     relswitch;
    Numba*      reloctaver;
    int         bindX;
    int         bindY;
    SliderZoom* slzoom;
    // Mixing stuff
    int         hoffs;
    HANDLE      hMixMutex;
    MixChannel  mchan[NUM_MIXCHANNELS];
    MixChannel  sendchan[4];
    MixChannel  masterchan;
    ScrollBard* mix_sbar;
    int         sbarH;
    Eff*        current_eff;

    Aux();
    void Play();
    void Stop();
    void ResetScrollbars();
	void HandleButtDown(Butt* bt);
    void UpdateBlankType();
	void HandleButtUp(Butt* bt);
    bool RescanPatternBounds();
    void UpdatePerScale();
    void SetScale(float scale);
    void DecreaseScale(bool mouse);
    void IncreaseScale(bool mouse);
    void CreateNew();
    void InitBlank(PattType ptype);
    void AuxReset();
    bool CheckIfMouseOnKeys(int mouse_x, int mouse_y, int* pianokey_num, float* vol);
    bool CheckIfMouseOnBinder(int mouse_x, int mouse_y);
    void ClickKeys();
    void Disable();
    void Enable();
    void DisableMixStuff();
    void EnableMixStuff();
    void DisablePatternStuff();
    void EnablePatternStuff();
    void DisableAuxStuff();
    void EnableAuxStuff();
    void PlaceNote(Instrument *instr, int note, float vol, unsigned long start_frame, unsigned long end_frame);
    void EditPattern(Pattern* pt, bool dbclick = false);
    Butt* CheckEditModeVariance(Butt* bt);
    void PopulatePatternWithInstruments(Pattern* pt);
    void SwitchToPatternMode();
    void ConvertPatternType(Pattern* patt, PattType ptype);
	void HandleTweak(Control* ct, int mouse_x, int mouse_y);
	void InitMixer();
    void CleanBuffers(int num_frames);
    void Mix(int num_frames);
    Eff* AddVSTEffectByPath(const char* fullpath, MixChannel* mchan);
    Eff* AddEffectFromBrowser(FileData * fdi, MixChannel* mchan);
    Eff* AddEffectByType(ModuleSubType etype, MixChannel* mchan);
    MixChannel* CheckFXString(DigitStr * mixstr);
    MixChannel* GetMixChannelByIndexStr(char * indexstr);
    void SetCurrentEffect(Eff* eff);
    void DeactivatePRHeight();
    void ActivatePRHeight();
    bool isBlank();
	bool isVolsPansMode();
    bool isPatternMode();
    bool isPlaying();
    void UpdateDrawInfo();
    void DisableStepVUs();
    void AdjustTick();
};

class Numba : public Panel
{
public:
    void*       kval;

    Butt*       btup;
    Butt*       btdn;

    int         xs;
    int         ys;

    Image*      img;
    int*        num;
    float*      fnum;

    char        str[MAX_ALIAS_STRING];

    NumbaType   ntype;

    Numba(void* v);
    Numba(void* v, NumbaType nt);
    void HandleButtUp(Butt* bt);
    void CheckVisibility(int xc);
};

class StaticArea : public Panel
{
public:
    float       num_ticks;
    float       tick_offset;

    int         pixwidth;
    int         actwidth;

    ScrollBard* sbar;

    Pattern*    patt;

    StaticArea();
    void HandleButtDown(Butt* bt);
};

#endif
