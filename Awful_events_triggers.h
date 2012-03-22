#ifndef EVENTS_TRIGGERS_H
#define EVENTS_TRIGGERS_H

#include "awful.h"

class Trigger
{
public:
    Event*      ev;
    Element*    el;
    Pattern*    patt;
    bool        activator;  // Whether this trigger activates or deactivates something
    bool        muted;
    bool        broken;     // Interrupted by break
    int         voicenum;

    // Antialiasing suite
    float       auxbuff[1024];
    bool        aaIN;
    bool        aaOUT;
    int         aaFilledCount;
    int         aaCount;
    int         auCount;
    float       prev_value;
    float       prevAAval;
    float       prevAAval1;
    float       aaBaseVal;
    float       cf1;
    float       cf2;
    long        frames_remaining;

    Trigger*    tgact;  // reference to the activator if this is a deactivator, otherwise NULL
    bool        outsync;   // When this trigger is outsync'ed with playback (per lining for example)
    bool        tworking;
    bool        toberemoved;
    bool        globallisted;
    Pattern*    ai;     // Autopattern instance for this trigga
    Trk*        trkdata;
    double      wt_pos;     // Wavetable position integer
    long		frame_phase;

    double		sec_phase;
    double		sec_phase1;
    double		sec_phase2;
    double		sec_phase3;
    float       envVal1;
    float       envVal2;
    float       envVal3;
    float       envVal4;
    float       envVal5;
    float       lcount;
    EnvPnt*     ep1;
    EnvPnt*     ep2;
    EnvPnt*     ep3;
    EnvPnt*     ep4;
    EnvPnt*     ep5;
    double		env_phase1;
    double		env_phase2;
    double		env_phase3;
    double		env_phase4;
    double		env_phase5;

    TgState     tgstate;
    float       vol_val;
    float       pan_val;
    float       vol_base;
    float       pan_base;
    int         note_val;
    double      freq_incr_base;
    double      freq_incr_active;
    int         freq_incr_sgn;
    float       freq;   // Constant part of the frequency
    float       signal;
    bool        rev;
    bool        skip;
    Mixcell*    mcell;
    MixChannel* mchan;
    PreviewSlot*    pslot;
    Trigger*    first_tgslide;
    Trigger*    tgsactive;
    Trigger*    tgsparent;
    Trigger*    tgvolloc;   // Local volume trigger reference
    int         tgsactnum;
    //////////////////////////////////////
    // Clamps
    //////////////////////////////////////
    Trigger*    act_prev; // Global active list
    Trigger*    act_next;
    Trigger*    loc_act_prev; // Local active list
    Trigger*    loc_act_next;
    Trigger*    ev_prev; // Event internal list
    Trigger*    ev_next;
    Trigger*    el_prev; // Element clones list
    Trigger*    el_next;
    Trigger*    pt_prev; // Pattern internal list
    Trigger*    pt_next;
    Trigger*    group_prev; // For group lists (envelopes, slidenotes, etc.)
    Trigger*    group_next;

    Trigger();
    void Initialize();
    void Activate();
    void Deactivate();
    void Release();
    void SoftFinish();
    bool IsInstant();
    bool IsSymbol();
    bool IsOutOfPlayback();
    bool CheckRemoval();
};

class Event
{
public:
    long   frame;
    long   framecount;
    float       vick;
    Pattern*    patt;

    Event*      next;
    Event*      prev;

    Trigger*    tg_first;
    Trigger*    tg_last;

    bool        queued;
    bool        to_be_deleted;

    Event();
    void AddTrigger(Trigger* c);
    bool RemoveTrigger(Trigger* c);
};

class Playback
{
public:
    Event*      queued_ev;
    Event*      first_ev;
    long        ev_count_down;
    bool        kooped;
    long        rng_start_frame;
    long        rng_end_frame;
    Pattern*    playPatt;
    long        currFrame;
    double      currFrame_tsync; // Time-synced value of current frame
    bool        tsync_block;
    double      currTick;
    bool        at_start;
    bool        dworking;
    bool        pactive;

    Playback*   prev;
    Playback*   next;


    Playback();
    Playback(Pattern* ppt);
    void SetPlayPatt(Pattern*    pPt);
    void SetActive();
    void SetInactive();
    void UnqueueAll();
    void UpdateQueuedEv();
    void TickFrame(long nc, long dcount, long fpb);
    void SetRanges(long start, long end);
    void SetLooped(bool loop);
    void SetCurrFrame(long frame);
    void SetCurrTick(double tick);
    void SetCurrFrameNOR(long frame);
    void SetFrameToTick();
    void RangesToPattern();
    bool IsAtStart();
    void ResetLooped();
    void ResetPos();
    void GetSmallestCountDown(long* count);
    long Requeue(bool change);
};


extern bool             IsInstantTrigger(Trigger* tg);
extern bool             IsSymbolTrigger(Trigger* tg);
extern bool             IsTriggerOutOfPlayback(Trigger* tg);
extern void             GlobalAddActiveTrigger(Trigger* tg);
extern void             GlobalRemoveActiveTrigger(Trigger* tg);
extern void             CleanupEvents(Pattern* pt);
extern void             UpdateEventsFrames(Pattern* pt);
extern void             RemoveTriggerFromEvent(Trigger* c);
extern void             CreateElementTriggersPerPattern(Pattern* pt, Element* el, bool skipaddtoelement = false);
extern void             Locate_Trigger(Trigger* tg);
extern void             Del_Element_Triggers(Element* el);
extern void             Del_Pattern_Internal_Triggers(Pattern* pt, bool skipelremove = false);
extern void             MakeTriggersForElement(Element* el, Pattern* ptmain);
extern void             RemoveTriggerFromEvent(Trigger* c);
extern void             DeactivateTriggerCommon(Trigger* tg);
extern void             ActivateCommandTrigger(Trigger* tg);
extern void             DeactivateCommandTrigger(Trigger* tg);
extern void             DeactivateSymbolTrigger(Trigger* efftg);
extern void             ActivateSymbolTrigger(Trigger* efftg);
extern void             DeactivatePlayback(Playback* pbk);
extern void             ActivatePlayback(Playback* pbk);
extern void             DisableAllPlaybacks();
extern void             AddAutoInstance(Trigger* tg, Instance* ii, bool add);
extern void             SetMixcell4Trigger(Trigger* tg);
extern void             SetMixChannel4Trigger(Trigger* tg);
extern void             PreInitEnvelopes(tframe frame, Pattern* pt, Event* firstev, bool activate_env, bool paraminit = true);
extern void             PreInitSamples(tframe frame, Pattern* pt, Event* firstev);

#endif //EVENTS_TRIGGERS_H
