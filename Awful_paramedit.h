#ifndef PARAMEDIT_H
#define PARAMEDIT_H

#include "awful.h"
#include "awful_elements.h"
#include "awful_audio.h"
#include "awful_controls.h"
#include "Awful_objects.h"


class PEdit : public Object
{
public:
    PEdit*      prev;
    PEdit*      next;

    char        hint[255];

    int         go;
    Element*    element;
    Pattern*    pt;
    bool        active;
    bool        bypass;
    bool        highlighted;
    bool        highlightable;
    bool        visible;
    bool        donotactivate;

    int         x;
    int         y;
    int         xs;
    int         ys;
    int         edx;
    int         edy;

    float       scale;
    ParamType   type;

    int         value;
    float       fvalue;

    Parameter*  param;
    Panel*      panel;
    Instrument* instr;

    DrawArea*   drawarea;

    PEdit();
    ~PEdit();
    virtual PEdit* Clone(Element* el) { return NULL; }
    bool    IsPointed(int mx, int my);
    virtual void    ProcessKey(unsigned int key, unsigned int flags) {}
    virtual void    ProcessChar(char character) {}
    virtual void    PlaceCursor() {}
	virtual void	UpdateValFromString() {}
	virtual void    toRight() {}
	virtual void    toLeft() {}
	virtual void    Activate();
    virtual void    Activate(int x, int y, int w, int h, int x1r, int y1r, int x2r, int y2r);
    void    Deactivate();
    virtual void    Leave();
    void    AddParam(Parameter* param, float* ptrval);
    virtual void    UpdateFromParam();
    virtual void    UpdateStringFromVal();
	virtual void    Reinit();
	virtual void    Hide() {}
	virtual void    Show() {}
    virtual void    Click(int mouse_x, int mouse_y, bool dbclick, unsigned flags) {}
    virtual void    SetHint(char *hint) { strcpy(this->hint, hint); };
};

class Envelope : public PEdit
{
public:
    CmdType     ctype;

    EnvPnt*     p_first;
    EnvPnt*     p_last;

    EnvPnt*     susPoint;

    int         hgt;    // in pixels
    float       len;    // in ticks
    float       scale;
    float       ticks;
    long        frame_length;
    float       last_value;
    float       prev_value;
    int         aaCount;
    float       aaBaseValue;
    float       cf1;
    float       cf2;
    bool        folded;
    Toggle*     fold;
    bool        timebased;
    Toggle*     timetg;
    bool        sustainable;
    bool        newbuff;   // flag indicating freshness of the buffer (to avoid double use)
	long		last_buffframe;
	long		last_buffframe_end;
    float       buff[MAX_BUFF_SIZE];
    float       buffoutval[MAX_BUFF_SIZE];

    // Dedicated list clamps for various instruments' and FX params
    Envelope*   group_prev;
    Envelope*   group_next;

    Envelope(CmdType ct);
    ~Envelope();
    Envelope* Clone(Element* el);
    Envelope* Clone();
    EnvPnt* LocatePoint(EnvPnt* p);
    void    RelocatePoint(EnvPnt* p);
    EnvPnt* AddPoint(float x, float y_norm, bool big = true);
    void    RemovePoint(EnvPnt* p);
    void    DeletePoint(EnvPnt* p);
    void    Clean();
    void    CalcTime();
    virtual bool    ProcessBuffer1(long curr_frame, long buffframe, long num_frames, int step, long start_frame, Trigger* tgenv = NULL, Trigger* tgi = NULL);
    void    StopBuffer();
    void    UpdateLastValue();
    float   GetValue(tframe frame);
    float   GetValue(float tick);
    virtual float   GetScale(Loc loc);
    void    DeleteRange(float x_curr, float x_start, bool start_except);
    void    Drag(int mouse_x, int mouse_y, unsigned flags);
    bool    IsMouseInside(int mouse_x, int mouse_y);
    void    Check(int mouse_x, int mouse_y);
    void    Click(int mouse_x, int mouse_y, unsigned flags);
    void    SetSustainable(bool sust);
    void    SetSustainPoint(EnvPnt* pnt);
    bool    IsOutOfBounds(long frame_phase);
    long    GetFramesRemaining(long frame_phase);
    void    InitAAEnv(bool offenv);
    void    Scale(float s);
    void    SetLength(float newlen);
    void    Simplify();
    XmlElement* Save(char* name);
    void Load(XmlElement* xmlEnvNode);
};

class Percent : public PEdit
{
public:
    char        digits[5];
    int         digit_width;
    int         curPos;
    int         maxPos;

    Percent(int val, int cpos, Element* ow, ParamType pt);
	void	ProcessChar(char character);
	void    ProcessKey(unsigned int key, unsigned int flags);
	void    toLeft();
    void    toRight();
    void    PlaceCursor();
	void	UpdateStringFromVal();
    void    UpdateValFromString();
	void    PackZeros();
    void    SetVal(int value);
    void    AdjustValue(int delta);
};

class Digits : public PEdit
{
public:
    char        digits[3];
    int         num_digits;
    int         digit_width;

    int         curPos;

    Digits(int val, int cpos, Element* ow);
	void ProcessChar(char character);
	void ProcessKey(unsigned int key, unsigned int flags);
    void PlaceCursor();
	void UpdateStringFromVal();
	void UpdateValFromString();
	void toLeft();
    void toRight();
};

class DigitStr : public PEdit
{
public:
    char        digits[3];
    int         num_digits;
    int         digit_width;
    int         curPos;
    int         oldval;
    bool        bright;
    Send*       send;
    Trk*        trk;
    bool        queued;
    Mixcell*    mcell;
    MixChannel* mchanout;
    MixChannel* mchanowner;
    Image*      bground;

    DigitStr*   mix_prev;
    DigitStr*   mix_next;

    DigitStr(const char* str);
	void SetString(const char* str);
	void DoUpdate();
	void ProcessChar(char character);
	void ProcessKey(unsigned int key, unsigned int flags);
    void PlaceCursor();
	void toLeft();
    void toRight();
    void Activate();
    void Activate(int x, int y, int w, int h, int x1r, int y1r, int x2r, int y2r);
    void Leave();
    void Click(int mouse_x, int mouse_y, bool dbclick, unsigned flags);
};

class Semitones : public PEdit
{
public:
    char        str[4];

    Semitones(int kolichestvo, Element* owna);
    void ProcessKey(unsigned int key, unsigned int flags);
    void UpdateStringFromVal();
    void SetValue(int val);
};

/*
class FVal : public PEdit
{
public:
    char        str[4];

    FVal(float fval, Element* owna);
    void ProcessKey(unsigned int key, unsigned int flags);
    void UpdateStringFromVal();
};

class Hz : public PEdit
{
public:
    Hz(float hzval, Element* owna);
};
*/

class Note : public PEdit
{
public:
    int     pixwidth;
    bool    relative;
    char    lastchar;
    float   freq;

    Note(short val, Element* ow);
	void ProcessKey(unsigned int key, unsigned int flags);
	void ProcessChar(char character);
	void Kreview(int key);
	void PlaceCursor();
	void SetValue(int newvalue);
	void SetRelative(bool rel);
	void Show();
	void Hide();
};

class TString : public PEdit
{
public:
    char        string[MAX_NAME_STRING];

    int         curPos;
    int         maxPos;
    int         cposx;
    int         maxlen;

    bool        space_allowed;

    TString(char* name, bool spc_allowed, Element* ow);
    TString(char* name, bool spc_allowed, Panel* pn);
    TString(char* name, bool spc_allowed, Instrument* inst);
    TString(char* name, bool spc_allowed);
    ~TString();

	void	ProcessChar(char character);
	void    ProcessKey(unsigned int key, unsigned int flags);
	void    toLeft();
    void    toRight();
    void    PlaceCursor();
	void    PackZeros();
	void    SetString(char* name);
	void    CapFirst();
};
#endif
