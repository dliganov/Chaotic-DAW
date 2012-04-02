#ifndef CURSOR_H
#define CURSOR_H

#include "awful.h"


class Cursor
{
public:
    CursMode        mode;
    CursType        type;
    Loc             loc;
    float           qsize;
    Element*        curElem;
    PEdit*          curParam;
    Instance*       slideparent;
    Pattern*        patt; // Only derived pattern can be referred here. No originator.
    Pattern*        tmp_patt;
    Loc             tmp_loc;
    float           tmp_tick;
    int             tmp_line;
    int             trknum;
    Trk*            trk;
    Trk*            maintrk;
    int             last_note;
    char            last_char;
    float           fieldposx;
    int             fieldposy;
    float           staticposx;
    int             staticposy;
    float           lastslidelength;
    int             pianokey_num;
    int             pianokey_keybound;
    bool            pianokey_pressed;

    Cursor();
    void SetPattern(Pattern* pattern, Loc location);
    void SetPatternContext(int mouse_x, int mouse_y);
    void PlaceCursor();
    void AdvanceView(float dtick, int dline);
    void AdvanceTick(float dtick);
    void AdvanceLine(int dline);
    void SetPos(float tick, int line);
    void PosUpdate();
    void PatternUpdate();
    void SaveState();
    void RestoreState();
    Element* CheckVisibility();
    void ExitPerClickInAnotherGridPlace();
    void ExitToDefaultMode();
    void ExitToCurrentMode();
    void ExitToNoteMode(Instrument* instr, int note_num);
    void ExitToSlidingMode(Element* parent, int note_num);
    SlideNote* PlaceSlideNote(char character, unsigned flags);
    void PosAndCursor2Mouse(int mx, int my);
};


typedef enum LastClicked
{
    LastClicked_Nothing,
    LastClicked_Instrument,
    LastClicked_Grid,
    LastClicked_Effect
}LastClicked;

class Mouse
{
public:
    __int64     mmode;
    LastClicked lastclick;
    SelMode     selmode;
    BrwType     btype;
    int         brwindex;
    Command*    active_command;
    Control*    active_ctrl;
    Control*    lasthintctrl;
    Pattern*    patt;
    SlideNote*  active_slide;
    Instrument* active_instr;
    Instrument* dropinstr1;
    Instrument* dropinstr2;
    Eff*        dropeff1;
    Eff*        dropeff2;
    Envelope*   active_env;
    EnvPnt*     active_pnt;
    EnvAction   env_action;
    float       env_xstart;
    PEdit*      active_paramedit;
    FileData*   active_filedata;
    Element*    active_elem;
    Element*    resize_elem;
    Samplent*   active_samplent;
    Gennote*    active_longnote;
    Mixcell*    active_mixcell;
    Mixcell*    active_dropmixcell;
    MixChannel* active_mixchannel;
    bool        on_effects;
    Eff*        active_effect;
    MixChannel* active_dropmixchannel;
    bool        mixcellindexing;
    bool        auxcellindexing;
    DigitStr*   active_paramfx;
    Numba*      active_numba;
    Panel*      active_panel;
    Menu*       active_menu;
    bool        skip_menu;
    bool        patternbrush;
    ResizeElem  resize_object;
    Loc         resize_loc;
    bool        v_only;
    LaneType    active_lane;
    int         current_track_num;
    Trk*        current_trk;
    int			current_line;
    float		current_tick;
    int			current_pt_line;
    float		current_pt_tick;
    TrkBunch*   track_bunch;
    DragData    drag_data;
    int         dragcount;
    bool        selstarted;
    bool        loostarted;
    RectEdge    edge;
    bool        LMB;      // left mouse button is pressed (if true)
    bool        RMB;      // right mouse button is pressed (if true)
    bool        bypass_element_activating;
    bool        menu_active;
    bool        at_definer;
    Loc         loc;
    Loc         lineloc;
    Loc         delloc;
    Loc         brushloc;
    bool        quanting;
    bool        on_keys;
    int         pianokey_num;
    bool        pianokey_pressed;
    int         pianokey_slot;
    float       pianokey_vol;
    Menu*       contextmenu;
    bool        prbunch;
    int         mouse_x;
    int         mouse_y;
    int         prev_x;
    int         prev_y;
    float       snappedTick;
    int         snappedLine;
    float       dragTick;
    int         dragLine;
    float       movetick;
    int         moveline;
    float       lpx;
    int         lpy;
    bool        prepianorolling;
    bool        pickedup;
    Loc         pickloc;
    Loc         selloc;
    Loc         looloc;
    bool        hint_active;
    bool        mousefix;
    bool        bpmbig;
    float       brushbasetick;
    float       brushwidth;
    BrushMode   brushmode;
    float       adtick;
    float       adtrack;
    float       qsize;
    bool        holding;

    Mouse();
    void PosUpdate(int mx, int my, unsigned flags, bool reset = false);
    void CalcPosition(int mx, int my);
    void CalcTickAndLine(int mx, int my, float* tick, int* line, Loc cloc);
    void PosCheck(int mx, int my);
    void CheckGridStuff(int mx, int my);
    void CheckGridElements(int mx, int my);
    void CheckAuxMixerStuff(int mx, int my);
    void* IsMouseOnBunch(int mx, int my);
    bool IsMouseOnMixer(int mx, int my);
    bool IsMouseOnLiner(int mx, int my);
    bool IsMouseOnAux(int mx, int my);
    bool IsMouseOnInstrPane(int mx, int my);
    bool IsMouseOnStaticGrid(int mx, int my);
    bool IsMouseOnSmallGrid(int mx, int my);
    bool IsMouseOnMainGrid(int mx, int my);
    void ClickActiveElement(int mx, int my, bool dbclick, unsigned flags);
    bool IsOnEdge();
    void DeleteContextMenu();
};


#endif CURSOR_H
