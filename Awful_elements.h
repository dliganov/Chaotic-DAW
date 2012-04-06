#ifndef ELEMENTS_H
#define ELEMENTS_H

#include "awful_paramedit.h"
#include "awful_audio.h"
#include "awful.h"
#include "Awful_objects.h"
#include "awful_tracks_lanes.h"


extern Element* ReassignInstrument(NoteInstance* ii, Instrument* instr);
extern void     BindPatternToInstrument(Pattern* pt, Instrument* instr);
extern void     CloneSlideNotes(NoteInstance* src, NoteInstance* dst, bool select);
extern void     UpdateStepSequencers();


class Element : public Object
{
public:
    ElemType          type;

    float           start_tick;
    float           end_tick;
    float			tick_length;
    int             pix_length;

    int             trknum;
    int             num_lines;
    int             track_line;

    Trk*            field_trkdata;
    Trk*            trkdata;

    long            frame;
    long            end_frame;
    long            frame_length;

    int             field_trknum;

    ParamSet*       track_params;

    Pattern*        patt;

    PEdit*      first_param;
    PEdit*      last_param;
    PEdit*      active_param;
    PEdit*      last_edited_param;
    PEdit*      default_param;
    ParamEditMode   pe_mode;

    Area            abs_area;

    int             gridauxed;
    int             auxauxed;
    int             xs;
    int             ys;
    int             cx;
    int             x;
    int             y;

    Pattern*        base;

    bool            active;
    bool            selected;
    bool            skip_cloning;
    Loc             selloc;
    bool            highlighted;
    bool            picked;
    bool            visible;
    bool            auxvisible;
    bool            displayable;

    bool            deleted;
    bool            to_be_deleted;
    bool            is_copied;

    Trigger*        tg_first;
    Trigger*        tg_last;

    Element*        next;
    Element*        prev;

    Element*        patt_next;
    Element*        patt_prev;

    Element*        pr_next;
    Element*        pr_prev;

    bool            resizetouched;


    Element();
	virtual ~Element();
    virtual Element*    Clone(bool add);
    virtual void    ProcessKey(unsigned int key, unsigned int flags);
    virtual void    ProcessChar(char character) {}
	virtual void    PlaceCursor();
    virtual void    SetAbsArea(int x1, int x2, int x3, int x4);
    virtual void    Activate();
    virtual void    Leave();
    virtual void    Reset() {}
    virtual void    Switch2Base() {}
    virtual void    Move(float dtick, int dtrack);
    virtual void	UpdateNote() {}
    void    CalcTiming();
    bool    IsPointed(int mx, int my);
    bool    CheckSelected();
    void    AddParamedit(PEdit* param);
    void    RemoveParamedit(PEdit* param);
    void    Switch2Param(PEdit* param);
    virtual int     Track();
    //virtual int     EndTrack();
    virtual float   StartTick();
    virtual float   EndTick();
	virtual int     StartTickX();
    virtual int     EndTickX();
    virtual int     TrackLine2Y();
    virtual int     EndLine2Y();
	virtual int     s_StartTickX();
    virtual int     s_EndTickX();
    virtual int     st_StartTickX();
    virtual int     st_EndTickX();
    virtual int     s_TrackLine2Y();
    virtual int     s_EndLine2Y();
    virtual long    StartFrame();
    virtual long    EndFrame();
    virtual void    Globalize();
    virtual void    Update(bool tonly = false);
    virtual void    CheckVisibility();
    virtual void    UpdateTrackDataFromLine();
    void    AddTrigger(Trigger* tg);
    void    RemoveTrigger(Trigger* tg);
    void    ForceResetTriggers();
    virtual bool    IsPlaying();
    virtual void    Show()   {}
    virtual void    Hide()  {}
    virtual bool    IsPresent();
    virtual void    GetRealCoordinates(Loc loc);
    bool            IsInstance();
    virtual void    Delete();
    virtual void    UnDelete();
    void            DeactivateAllTriggers();
    bool            IsDisplayableOnPatternType(PattType ptype);
    void            SetTrackLine(int trkline);
    void            SetEndTick(float endtick);
    void            SetTickLength(float ticklength);
    bool            IsCoveringEffect();
    virtual void    Save(XmlElement* xmlNode);
    virtual void    Load(XmlElement* xmlNode);
    void            Select();
    void            Deselect();
    virtual void    MarkAsCopied();
    Loc             GetElemLoc();
    void            ResizeTouch(bool touch);
    bool            IsResizeTouched();
};

class NoteInstance : public Element
{
public:
    Instrument*     instr;

    Percent*        ed_vol;
    Percent*        ed_pan;
    Note*           ed_note;
    Percent*        ed_len;

    Vol*            loc_vol;
    Pan*            loc_pan;
    Len*            loc_len;

    ParamSet*       instr_track_params;
    ParamSet*       instr_pattern_params;

    Envelope*       attached_vol_env;
    Envelope*       attached_pan_env;

    SlideNote*      first_slide;
    Mixcell*        mixcell;

    Trk*            trkbuff;

    bool            preview;

    float           freq;
    float           freq_ratio;

    NoteInstance();
    virtual ~NoteInstance();
    void    SwitchVol();
    void    SwitchPan();
    void    SwitchNote();
    void    SwitchLen();
	void	Activate();
    void	AddSlideNote(SlideNote* sl);
    void    RearrangeSlideNotes();
    void    SpreadSlideNotes();
	SlideNote*  GetLastSlide();
	virtual void	CalcSlideNotes();
    void	ProcessChar(char character);
    void	ProcessKey(unsigned int key, unsigned int flags);
    virtual void	Reset();
    int     TrackLine2Y();
    int     EndLine2Y();
    virtual void	UpdateNote();
    void    RemoveSlidenote(SlideNote* sn);
    void    Move(float dtick, int dtrack);
    void    Save(XmlElement* xmlNode);
    void    Load(XmlElement* xmlNode);
    void    Mute();
};

class GenNote : public NoteInstance
{
public:

    GenNote(Instrument* instr);
    virtual ~GenNote();
    GenNote* Clone(bool add);
    void    Update(bool tonly = false);
};

class Samplent : public NoteInstance
{
public:
	Sample*         sample;
    long            sample_frame_len;
    bool            wave_visible;
    int             wave_height_pix;
    // paramedits
    Percent*        ed_offs;
    // params
    Len*            loc_offs;
    bool            revB;
    long            smp_left_frame;
    long            smp_right_frame;
    float           freq_incr_base;

    Samplent(Sample* sample);
    virtual ~Samplent();
    Samplent*   Clone(bool add);
    void    ProcessKey(unsigned int key, unsigned int flags);
    void    ProcessChar(char character);
    void    UpdateNote();
    bool    IsOutOfBounds(double* cursor);
    void    Reset();
    void    UpdateSampleBounds();
    bool    InitCursor(double* cursor);
    void    Update(bool tonly = false);
    void    SwitchOffs();
    void    Save(XmlElement* xmlNode);
    void    Load(XmlElement* xmlNode);
};

class EffElement : public Element
{
    void    Move(float dtick, int dtrack);
};

class Frequencer : public EffElement
{
public:
    Frequencer* freq_prev;
    Frequencer* freq_next;

    Frequencer();
    virtual float GetMult(Trigger* tg, Trigger* tgi, long framephase, long num_frames) {return 1;}

};

class Slide : public Frequencer
{
public:
    // Initial params
    Semitones*      smt;

    // Active params
    float           nN;
    float           fR;

    float           linear_increment;
    float           pan_increment;
    float           vol_increment;

	Slide(int semitones);
    Slide* Clone(bool add);
	void ProcessKey(unsigned int key, unsigned int flags);
	void ProcessChar(char character);
    void CalcSlide();
    float GetMult(Trigger* tg, Trigger* tgi, long framephase, long num_frames);
    void Update(bool tonly = false);
    void Save(XmlElement* xmlNode);
    void Load(XmlElement* xmlNode);
};

class Vibrate : public Frequencer
{
public:
    float       freq;
    float       range;

	Vibrate();
    Vibrate(float freq, float range);
    Vibrate* Clone(bool add);
	void ProcessKey(unsigned int key, unsigned int flags);
	void ProcessChar(char character);
    float GetMult(Trigger* tg, Trigger* tgi, long framephase, long num_frames);
};

class SlideNote : public Slide
{
public:
	NoteInstance*       parent;

    Note*           ed_note;
    //Digits*         ed_vol;
    //Digits*         ed_pan;
    Percent*        ed_vol;
    Percent*        ed_pan;
    //Percent*        ed_len;

    Vol*            loc_vol;
    Pan*            loc_pan;
    //Len*            loc_len;

    Mixcell*        mixcell;

    SlideNote*      s_next;
    SlideNote*      s_prev;

    SlideNote(int note_num, NoteInstance* par);
    SlideNote* Clone(bool add);
	void	UpdateNote();
	void	ProcessKey(unsigned int key, unsigned int flags);
	void	Activate();
    void    Move(float dtick, int dtrack);
    void    Update(bool tonly = false);
    float   GetMult(Trigger* tg, Trigger* tgi, long framephase, long num_frames);
    float   GetVolMult(Trigger* tg, Trigger* tgi, long framephase, long num_frames);
    float   GetPanMult(Trigger* tg, Trigger* tgi, long framephase, long num_frames);
    float   GetSignalIncr(Trigger* tg, Trigger* tgi, long framephase, long num_frames);
    void    Delete();
    void    UnDelete();
    void    Save(XmlElement* xmlNode);
    void    Load(XmlElement* xmlNode);
};

class Muter : public EffElement
{
public:
	int mute_param;

	Muter();
    Muter*  Clone(bool add);
	void    ProcessKey(unsigned int key, unsigned int flags);
	void    ProcessChar(char character);
};

class Transpose : public EffElement
{
public:
    Semitones*      smt;
    float           freq_mult;

	Transpose(int semitones);
    Transpose* Clone(bool add);
	void ProcessKey(unsigned int key, unsigned int flags);
	void ProcessChar(char character);
    void UpdateNote();
    void Save(XmlElement* xmlNode);
    void Load(XmlElement* xmlNode);
};

class Break : public EffElement
{
public:
	int break_param;

	Break();
    Break* Clone(bool add);
	void ProcessKey(unsigned int key, unsigned int flags);
};

class Txt : public Element
{
public:
	char    buff[250];
    int     curPos;
    bool    bookmark;

    Txt();
    Txt* Clone(bool add);
	void ProcessKey(unsigned int key, unsigned int flags);
	void ProcessChar(char character);
	void ParseString();
	void AddString(char* character);
	void delChar();
    void Move(float dtick, int dtrack);
    void Delete();
    void UnDelete();
    void Save(XmlElement* xmlNode);
    void Load(XmlElement* xmlNode);
};

class Command : public Element
{
public:
    int         com_num;
    CmdType     cmdtype;
    bool        reset_command;
    bool        queued;

    Scope*      scope;

    Parameter*  param;
    Toggle*     toggle;
    PEdit*      paramedit;
    ParamType   param_type;

    comm_val    val;

    ParamSet*   target_params;
    Trigger**   eff_target;

    long        process_frame_first;
    long        process_frame_last;

    Command(Scope* sc_t);
    Command(Scope* sc, CmdType ct);
    Command(Scope* sc, CmdType ct, Parameter* prm);
    Command* Clone(bool add);
    void    ProcessChar(char character);
    void    ProcessKey(unsigned int key, unsigned int flags);
    void    InitCommParam();
    void    InitVol();
    void    InitPan();
    void    InitVolEnv();
    void    InitPanEnv();
    void    InitLocVolEnv();
    void    InitLocPanEnv();
    void    InitMute();
    void    InitVibrate();
    void	InitSlide();
    void	InitLen();
    void	InitOffs();
    void	ChangeParam();
    void    InitParamEnv();
    void    InitPitchEnv();
    void    SwitchEnvelope();
    void    Update(bool tonly = false);
    void	Reset();
    ParamSet* GetTargetParams();
    Trigger** GetTargetSym();
    void    ApplyInstant();
    void    Show();
    void    Hide();
    void    CheckVisibility();
    void    Delete();
    void    UnDelete();
    bool    IsEnvelope();
    void    Save(XmlElement* xmlNode);
    void    Load(XmlElement* xmlNode);
};

class Reverse : public EffElement
{
public:
    bool   value;

    Reverse();
    Reverse* Clone(bool add);
    void ProcessKey(unsigned int key, unsigned int flags);
	void ProcessChar(char character);
};

class Repeat : public EffElement
{
public:
    bool   value;

    Repeat();
    Repeat* Clone(bool add);
    void ProcessKey(unsigned int key, unsigned int flags);
	void ProcessChar(char character);
};

class Pattern : public Element
{
public:
    TrkBunch*   bunch_first;
    TrkBunch*   bunch_last;
    Trk*        first_trkdata;
    Trk*        last_trkdata;
    Trk*        top_trk;
    Trk*        bottom_trk;
    Element*    first_elem;
    Element*    last_elem;
    Pattern*    der_prev;
    Pattern*    der_next;
    Pattern*    der_first;
    Pattern*    der_last;
    int         der_num;
    Trigger*    tg_internal_first;
    Trigger*    tg_internal_last;
    Event*      first_ev;
    Event*      last_ev;

    float       tick_width;
    TString*    name;
    bool        aliasModeName; // Indicates whether we are renaming pattern or typing an alias
    Image*      nmimg;
    int         track_line_end;
    int         total_lines;

    Image*      limg;
    Image*      smallimg;

    bool        is_fat;

    ParamSet*   params;
    bool        ranged;
    Percent*    ed_vol;
    Percent*    ed_pan;
    Note*       ed_note;

    Vol*        vol_local;
    Pan*        pan_local;

    PattType    ptype;
    bool        autopatt;
    bool        folded;
    bool        muted;
    Instrument* instr_owner;
    NoteInstance*   parent_instance;
    Trigger*    parent_trigger;
    Trigger*    symtrig;
    Trigger*    tgenv;
    Command*    pitch;
    Lane        vol_lane_main;
    Lane        pan_lane_main;
    Pattern*    basePattern;
    int         baseindex;
    Pattern*    base_prev;
    Pattern*    base_next;
    Playback*   pbk;
    Instrument* ibound;
    FXState     fxstate;
    Scope       scope;

    int         offs_line;
    int         last_pianooffs;


    Pattern(char* nm, int x1, int x2, int y1, int y2, bool tracks);
    Pattern(char* nm, float tk1, float tk2, int tr1, int tr2, bool tracks);
    ~Pattern();
    Pattern* Clone(bool add, float new_tick, int new_trackline);
    Pattern* Clone(bool add);
    Pattern* Copy(bool add);
    void Init(char* nm, bool tracks);
    void Save(XmlElement* xmlNode);
    void Load(XmlElement* xmlNode);
    void AddEnvelope(ParamType ptype);
    void AddElement(Element* el);
    void RemoveElement(Element* el);
	void AddSelectedElements();
    void CopyElementsTo(Pattern* dst, bool add);
	void DeleteAllElements(bool flush, bool preventundo);
    void Move(float dtick, int dtrack);
	void Reset();
    int Track();
    //int EndTrack();
    float StartTick();
    float EndTick();
	int StartTickX();
    int EndTickX();
    int st_StartTickX();
    int st_EndTickX();
    int TrackLine2Y();
    int EndLine2Y();
    long StartFrame();
    long EndFrame();
    void ProcessChar(char character);
    void SwitchVol();
    void SwitchPan();
    void CheckVisibility();
    void Update(bool tonly = false);
    void ParameditUpdate(PEdit* pe);
    void ToggleLane(int line_num, LaneType type);
    void AddLane(Lane* lane);
    Lane* GetLane(int line_num, LaneType type);
    int LineOffs(int line_num);
    int LineNum(int offs);
    void UpdateLaneBases();
    void ToggleFullView();
    void AddTrkData(Trk * td);
    Element* IsElemExists(float tick, int trknum);
    void AddDerivedPattern(Pattern* child, bool skipadd = false);
    void RemoveDerivedPattern(Pattern* child);
    void AddInternalTrigger(Trigger* tg);
    void RemoveInternalTrigger(Trigger* tg);
    void UpdateInternalTriggers();
    bool IsLastChild();
    void SpreadNewName();
    void UpdateTrackDataFromLine();
    void AddPlayback();
    bool IsPlaying();
    bool IsPlaybackActive();
    void DisablePlayback();
    void EnqueueEnvelopeTrigger(Trigger* tg);
    void DequeueEnvelopeTrigger(Trigger* tg);
    Element* NextElementToDelete(Element* eltodel);
    bool IsPresent();
    bool IsAnyDerivedPresent();
    void UpdateScaledImage();
    bool IsElementTypeDisplayable(ElemType eltype);
    void MarkAsCopied();
    bool isTheOnlyPresentChild();
};

#endif
