#ifndef CONTROLS_H
#define CONTROLS_H

#include "awful.h"
#include "awful_audio.h"
#include "awful_effects.h"
#include "awful_params.h"
#include "awful_instruments.h"

typedef enum KnobType
{
    Knob_Small,
    Knob_Large,
    Knob_XLarge,
    Knob_ZLarge,
    Knob_Default
}KnobType;

class Control : public Object
{
public:
    ControlType     type;
    char            hint[255];
    char            valstring[111];
    char            title[255];
    bool            enabled;

    int             x;
    int             y;
	int		        width;
	int		        height;
    int             xs;
    int             ys;
    int             ysl;

    bool            active;
    bool            pattern;
    bool            coverable;

    Parameter*      param;
    Panel*          panel;
    Pattern*        patt;
    Instrument*     instr;
    Element*        elem;

    Scope*          scope;

    bool            anchored;
    bool            anchorsign;
    int             anchorcount;
    long            anchormx;
    long            anchormy;

    DrawArea*       drawarea;

    Control*        next;
    Control*        prev;

    Control*        pn_next;
    Control*        pn_prev;

    Control*        prm_next;
    Control*        prm_prev;

    Control*        lst_next;
    Control*        lst_prev;

    Control();
    virtual ~Control();
    virtual bool    MouseCheck(int mouse_x, int mouse_y);
    virtual void    MouseClick(int mouse_x = 0, int mouse_y = 0) {};
    virtual void    TweakByWheel(int delta, int mouse_x, int mouse_y) {};
    virtual void    TweakByMouseMove(int mouse_x, int mouse_y) {};
    virtual void    Reset() {};
    virtual void    AddParam(Parameter* p);
    virtual void    UpdatePosFromVal() {};
    virtual void    UpdateValFromPos() {};
    virtual void    SetHint(char *hint) { strcpy(this->hint, hint); };
    virtual void    SetTitle(char *title) { strcpy(this->title, title); };
    void            SetOnClickHandler(CLICK_HANDLER_FUNC handler) { this->on_click_func = handler; };
    void            OnClick(Object* owner = NULL)
                    {
                        if (this->on_click_func != NULL)
                        {
                            this->on_click_func(owner, (Object*)this);
                        }
                    };
    virtual void    SetAnchor(float val) {};
    virtual void    Deactivate();
    virtual void    Activate();
    virtual void    Activate(int x, int y, int w, int h, int x1r, int y1r, int x2r, int y2r);
    virtual void    Activate(int x, int y, int w, int h);
    void            UpToDrawarea();
    bool            isVolumeSlider();
    bool            isPanningSlider();

private:
    CLICK_HANDLER_FUNC on_click_func;
};

typedef enum SliderType
{
    Slider_Default,
    Slider_Param,
    Slider_Volume,
    Slider_InstrVol,
    Slider_InstrPan,
    Slider_Zoom
}SliderType;

class SliderBase : public Control
{
public:
    int         pos;
    int         step;
    int         anchorpos;
    float       anchorval;
	int         scale;
    int         length;
    SliderType  sltype;

    VU*         vu;

	SliderBase();
	SliderBase(Panel* ow, Scope* sc);
    virtual void    UpdateValFromPos();
    virtual void    TweakByWheel(int delta, int mouse_x, int mouse_y);
    virtual void    TweakByMouseMove(int mouse_x, int mouse_y);
    //virtual bool    MouseCheck(int mouse_x, int mouse_y);
    virtual void    UpdatePosFromVal();
    virtual void    MouseClick(int mouse_x, int mouse_y);
    virtual void    SetAnchor(float val);
    virtual void    UpdateAnchorSign();
};

class SliderZoom : public SliderBase
{
public:

	SliderZoom(Panel* ow, Scope* sc);
    virtual void    UpdateValFromPos();
    virtual void    TweakByWheel(int delta, int mouse_x, int mouse_y);
    virtual void    TweakByMouseMove(int mouse_x, int mouse_y);
    virtual void    UpdatePosFromVal();
    virtual void    MouseClick(int mouse_x, int mouse_y);
            void    SetTickWidth(float tW);
            float   GetTickWidth();
};

class SliderHVol : public SliderBase
{
public:

	SliderHVol(Panel* ow, Scope* sc);
	SliderHVol(int l, Panel* ow, Scope* sc);
    void    UpdateValFromPos();
    void    TweakByMouseMove(int mouse_x, int mouse_y);
    void    UpdatePosFromVal();
    void    MouseClick(int mouse_x, int mouse_y);
    void    SetAnchor(float val);
    void    UpdateAnchorSign();
};

class SliderHPan : public SliderBase
{
public:

	SliderHPan(Panel* ow, Scope* sc);
	SliderHPan(int l, Panel* ow, Scope* sc);
    void    UpdateValFromPos();
    void    TweakByMouseMove(int mouse_x, int mouse_y);
    void    UpdatePosFromVal();
    void    MouseClick(int mouse_x, int mouse_y);
    void    SetAnchor(float val);
    void    UpdateAnchorSign();
};

class SliderParam : public SliderBase
{
public:
    Paramcell*  pdata;
    float       interval;

	SliderParam(int len, Panel* ow, Scope* sc, Paramcell* pd);
    virtual void    UpdateValFromPos();
    virtual void    TweakByWheel(int delta, int mouse_x, int mouse_y);
    virtual void    TweakByMouseMove(int mouse_x, int mouse_y);
    virtual void    UpdatePosFromVal();
    virtual void    MouseClick(int mouse_x, int mouse_y);
    virtual void    SetAnchor(float val);
    virtual void    UpdateAnchorSign();
    virtual bool    MouseCheck(int mouse_x, int mouse_y);
    virtual void    AddParam(Parameter* p);
};

class Knob : public Control
{
public:
    KnobType    ktype;

	int     scale;
    int     rad;
    float   pos;    // angle in radians
    float   anchorpos;

    float   range;
    float   offset;

	Knob(int r, Parameter* par, Panel* ow, Scope* sc, KnobType kt);
    void    UpdateValFromPos();
    void    UpdatePosFromVal();
    void    TweakByWheel(int delta, int mouse_x, int mouse_y);
    void    TweakByMouseMove(int mouse_x, int mouse_y);
    void    SetAnchor(float val);
    void    UpdateAnchorSign();
};

class ScrollBard : public Control
{
public:
	float       visible_len;	// Normalized
	float       full_len;
    float       offset;
    float       actual_offset;
    float       cf;

    int         pixlen;
    int			tolschina;
    int         barpixoffs;
    int         actual_barpixoffs;
    int         barpixlen;

    bool        rect;

    int			x1;
    int			x2;
    int			y1;
    int			y2;

    bool        fixed;
    bool        discrete;

    ScrollBard();
    ScrollBard(ControlType bartype, float totallen, float visiblelen, int tolstota);
    bool    MouseCheck(int mouse_x, int mouse_y);
    void    MouseClick(int mouse_x, int mouse_y);
    void    TweakByWheel(int delta, int mouse_x, int mouse_y);
    void    SetPixDelta(int delta, int mouse_x, int mouse_y);
    void    SetDelta(float delta);
    void    SetOffset(float offs);
    void    AddDrawarea();
};

class Butt : public Control
{
public:
	bool    pressed;

	bool    needs_refresh;
    bool    freeset;

    Image*  imgup;
    Image*  imgdn;

	Butt(bool state);
	~Butt();
    virtual void SetImages(Image* imup, Image* imdn);
    virtual void Press();
    virtual void Release();
    virtual void MouseClick(int mouse_x, int mouse_y);
    void Refresh();
};

class ButtFix : public Butt
{
public:
    Panel*    pn;

	ButtFix(bool state);
	ButtFix(bool state, Panel* panel);
	ButtFix(bool state, Image* img1, Image* img2);
    void MouseClick(int mouse_x, int mouse_y);
    void Press();
    void Release();
};

class Toggle : public Control
{
public:
	bool*       state;
    ToggleType  tgtype;

    Trk*        trk;
    Mixcell*    mixcell;

    Image*  imgup;
    Image*  imgdn;

    Toggle(Panel* ow);
    Toggle(bool* st, Scope* scp);
    Toggle(bool* st, Scope* scp, ToggleType tt);
    Toggle(ToggleType tt, bool* st);
    Toggle(Instrument* i, ToggleType tt);
    Toggle(Mixcell* mc, ToggleType tt);
    Toggle(Trk* t, ToggleType tt);
    void MouseClick(int mouse_x = 0, int mouse_y = 0);
    void PerClick();
    virtual void SetImages(Image* imup, Image* imdn);
};

class ToggleParam    : public Toggle
{
public:
    Paramcell*  pdata;

    ToggleParam(bool* st, Scope* scp, Paramcell* pd);
    void AddParam(Parameter* prm);
    void MouseClick(int mouse_x = 0, int mouse_y = 0);
    void Release();
    void UpdatePosFromVal();
	bool MouseCheck(int mouse_x, int mouse_y);
};

class ValueString : public Control
{
public: 
    VStrType    vstrtype;
    VStrVal     val;
    int         fract_size;
    int         scaler;
    char        label[100];
    char        outstr[100];

	ValueString(VStrType vtype);
    void SetValue(float newval);
    void SetValue(int newval);
    void SetValue(char* val);
    void UpdateString();
};

class MenuItem : public Control
{
public:
    MItemType   itemtype;
    bool        doable;
    Menu*       menu;
    Menu*       submenu;

    MenuItem*   iprev;
    MenuItem*   inext;

    MenuItem(MItemType itype, Menu* menu);
    bool    MouseCheck(int mouse_x, int mouse_y);
    void    MouseClick(int mouse_x, int mouse_y);
};

class Menu : public Control
{
public:
    MenuItem*   first_item;
    MenuItem*   last_item;
    MenuItem*   main_item;

    float       tick;
    int         line;
    int         trknum;
    int         num_items;
    Trk*        trk;
    bool        dir;
    MenuType    mtype;
    Control*    activectrl;
    Browser*    browser;
    ParamModule* module;
    PresetData* activepresetdata;
    FileData*   activefiledata;
    Eff*        curreff;
    Instrument* instr;
    MixChannel* mchan;

    Menu(bool direction, MenuType mt);
    Menu(int mouse_x, int mouse_y, MenuType mt);
    void    AddItem(MItemType itype, Menu* m);
    void    AddItem(MItemType itype, Menu* m, char* title);
    void    SetMainItem(MItemType itype);
};

class DropMenu : public Menu
{
public:

    DropMenu(bool direction, MenuType mt);
    void    UpdateBounds(bool opened);
};

class InPin : public Control
{
public:
    int xc;
    int yc;

    Mixcell*    owna;

    InPin()
    {
        type = Ctrl_InPin_Point;
        owna = NULL;
    }
};

class InPin_Bus : public InPin
{
public:

    InPin_Bus()
    {
        type = Ctrl_InPin_Bus;
        owna = NULL;
    }
};

class OutPin : public Control
{
public:
	int xc;
    int yc;
    int xc_offs;
    int yc_offs;
    int xconn;
    int yconn;

    bool offset;
    bool point_begin;
    bool connected;

    Mixcell*    owna;
    InPin*      dst;

    OutPin()
    {
        type = Ctrl_OutPin_Point;
        owna = NULL;
        dst = NULL;
        offset = false;
        xc_offs = 0;
        yc_offs = 0;
        connected = false;
    }
    bool    MouseCheck(int mouse_x, int mouse_y);
};

class Paramcell
{
public:
    ParamcellType   pctype;
    char            title[100];
    int             x;
    int             y;
    int             w;
    int             h;
    int             h1;
    bool            visible;
    ValueString*    vstring;
    SliderParam*    slider;
    Parameter*      param;
    Toggle*         toggle;
    DrawArea*       drawarea;
    Panel*          panel;
    Scope*          scope;
    ToggleParam*    tgp_current;
    Control*        ctrl_first;
    Control*        ctrl_last;
    Paramcell*      cloned; // Pointer to the cell, cloned from this cell
    Paramcell*      orig; // Pointer to the originator cell, if any
    Paramcell*      prev;
    Paramcell*      next;

    Paramcell(ParamcellType type, Parameter* prm, Panel* pn, Scope* scp);
    ~Paramcell();
    Paramcell* Clone(Panel* pn);
    void SetTitle(char* title) { strcpy(this->title, title); };
    void AddParamEntry(Parameter* prm);
    void SetCurrentItem(ToggleParam*    tgp_new);
    void SetCurrentItemByTitle(char* title);
    ToggleParam* GetCurrentItem();
    void Disable();
    void Enable();
    void SetVisible(bool vis);
};

#endif
