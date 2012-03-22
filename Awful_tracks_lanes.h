#ifndef TRACKS_LANES_H
#define TRACKS_LANES_H

#include "awful.h"

typedef struct TrkBunch
{
    int start_track;
    int end_track;

    Trk* trk_start;
    Trk* trk_end;

    int startpix;
    int endpix;

    ParamSet*  params;

    TrkBunch*   prev;
    TrkBunch*   next;
}TrkBunch;

class Lane
{
public:
    LaneType    type;

    int         lnum;
    Toggle*     fold;
    bool        visible;
    bool        folded;
    int         height;
    int         y;
    int         base_line;

    Lane*       next;

    Lane();
    Lane(bool vis, int hgt, LaneType lt);
    bool CheckMouse(int mouse_x, int mouse_y);
};

class Trk
{
public:
    TrkType     trktype;
    int         trknum;
    // all lines occupied by the track
    int         start_line;
    int         end_line;
    // only text lines (excepting vol/pan lanes)
    int         trk_start_line;
    int         trk_end_line;
    bool        active;
    ParamSet*   params;
    TrkBunch*   trkbunch;
    bool        buncho;
    bool        bunched;
    bool        bunchito;
    Command*    envelopes;
    Instrument* defined_instr;
    Pianoroll*  proll;
    Lane        vol_lane;
    Lane        pan_lane;
    SliderHVol* vol;
    SliderHPan* pan;
    Toggle*     mute;
    Toggle*     solo;
    Scope       scope;
    FXState     fxstate;
    Mixcell*    mcell;
    MixChannel* mchan;

    Trk*        next;
    Trk*        prev;

    Trk();
    ~Trk();
    void Init(int track_num, bool controls);
    void UpdateLaneBases();
};

class Pianoroll
{
public:
    int     trknum;
    int     start_line;
    int     end_line;
    int     proll_width;
    int     keypixwidth;

    int     startpix;
    int     endpix;

    int         start_note;
    ScrollBard*  sbar;
    Toggle*     scale;
    bool        scaled;
    Trk*        trk;

    bool        view;
    Pianoroll*   next;
    Pianoroll(Pattern* pt, int trknum, int pixwidth);
};

extern void             Bunch_Tracks(int y1, int y2);
extern int              GetActualTrack(int trk_num);
extern TrkBunch*        CheckIfTrackBunched(int trk_num, Pattern* pt);
extern Trk*             GetTrkDataForTrkNum(int trknum, Pattern* pt);
extern int				GetTrkNumForLine(int line, Pattern* pt);
extern Trk*             GetTrkDataForLine(int line, Pattern* pt);
extern void             DeleteTrack();
extern void             InsertTrack();
extern void             AdvanceTracks(Trk* trkdata, int num, Pattern* pt);
extern void             Remove_Bunch(TrkBunch* tb, Pattern* pt);
extern void             Add_Bunch(int start, int end, ParamSet* params, Pattern* pattern);
extern void             ResetTrackControls();

#endif //TRACKS_LANES_H
