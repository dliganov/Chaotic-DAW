#ifndef PARAMS_H
#define PARAMS_H

#include "awful.h"
#include "awful_audio.h"
#include "awful_effects.h"
#include "Awful_objects.h"
#include "awful_jucecomponents.h"

class Preset : public Object
{
public:
    Preset(Object* owner);
    char      name[MAX_PRESET_NAME];
    long      index;
    bool      native;
    char      path[MAX_PATH_STRING];
    Preset*   next;
    Preset*   prev;
};

class ParamSet
{
public:

    Vol*            vol;
    Pan*            pan;
    Len*            len;
    Len*            offs;

    BoolParam*      muteparam;
    BoolParam*      soloparam;

    bool            muted;
    bool            solo;

    ParamSet(int p_vol, int p_pan, Scope* sc);
    ParamSet(float nvol, float npan, Scope* sc);
    void Reset();
};

class Switch : public Object
{
public:
    bool    state;

    Switch(bool val);
    void SetValue(bool val);
    void Toggle();
    void AddToggle(Control* ct);
    void RemoveControl(Control* ct);
    virtual void AddParamedit(PEdit* pe);
    virtual void Reset();
    void UpdateBindings();
    virtual void UpdateFromParamedit();
};

class Parameter : public Object
{
public:
    int         globalindex;
    ParamType   type;
    char        name[MAX_PARAM_NAME];
    Parameter*  gprev;
    Parameter*  gnext;
    Parameter*  prev;
    Parameter*  next;
    Parameter*  rec_prev;
    Parameter*  rec_next;
    int     p_val;
    float   defaultval;
    float   initialval;
    float   val;
    float   outval;
    float   offset;
    float   range;
    float   interval;
    float   lastval;  // used for ramping
    int     p_base;
    int     index;
    bool    grouped;
    bool    reversed;
    bool    issigned;
    bool    aslider_updating;
    bool    recording;
    bool    envaffect;
    bool    envdirect;
    bool    envtweaked;
    bool    presetable;

    Scope*      scope;
    Control*    first_ctrl;
    Control*    last_ctrl;
    PEdit*      prmedit;
    ParamModule* module;
    Element*    element;
    ValueString*    vstring;
    Trigger*    envelopes;
    ASlider*    aslider;
    Envelope*   autoenv;
    Command*    cmdenv;
    EnvPnt*     lastrecpnt;

    tframe      lastsetframe;

	Parameter();
	Parameter(float nval, float offs, float rng);
	Parameter(float nval, float offs, float rng, ParamType ptype);
	Parameter(const char* name, float nval, float offs, float rng, ParamType ptype);
	Parameter(int p_value);
	Parameter(float nval);
	virtual ~Parameter();
    void ParamInit();
    virtual void    SetPercentValue(int val);
    virtual void    SetNormalValue(float val, bool asliderupdate = false);
    virtual void    SetValueFromControl(float ctrlval, bool asliderupdate = false);
    virtual void    SetDirectValueFromControl(float ctrlval, bool asliderupdate = false);
    virtual void    SetValueFromEnvelope(float envval, Envelope* env, bool asliderupdate = false);
    virtual void    UpdatePerNormalValue();
    virtual void    Normalize();
    virtual void    Unnormalize();
    void            AddControl(Control* ct);
    void            RemoveControl(Control* ct);
    virtual void    AddParamedit(PEdit* pe);
    virtual void    Reset();
    virtual void    UpdateBindings();
    virtual void    UpdateFromParamedit();
    virtual void    Reinit();
    virtual void    ResetToInitial();
    void            SetName(char *nm) { strcpy(this->name, nm); };
    char*           GetName() { return this->name; };
    void            EnqueueEnvelopeTrigger(Trigger* tg);
    void            DequeueEnvelopeTrigger(Trigger* tg);
    void            SetInterval(float newint);
    float           GetInterval();
    void            SetReversed(bool rev);
    bool            GetReversed();
    void            SetSigned(bool sgn);
    bool            GetSigned();
    void            AddValueString(VStrType vstrtype);
    void            HandleRecordingFromControl(float ctrlval);
    void            FinishRecording();
    void            BlockEnvAffect();
    void            UnblockEnvAffect();
    bool            isRecording();
    void            setEnvDirect(bool envdir);
    bool            getEnvDirect();
    void            SetInitialValue(float initial);
    void            SetPresetable(bool able);
    bool            IsPresetable();
    void            SetLastVal(float lval);
    XmlElement *    Save();
    void            Load(XmlElement* xmlParamNode);
    XmlElement *    Save4Preset();
    void            Load4Preset(XmlElement* xmlParamNode);
};

class BoolParam : public Parameter
{
public:
    bool     outval;
    AToggleButton*  atoggle;

	BoolParam(bool state);
	BoolParam(bool state, const char* name);
	void SetBoolValue(bool bval);
    void UpdateBindings();
};

class Len : public Parameter
{
public:
    LenType     len_type;
    len_val     lval;

	Len(int p_len, LenType ltype);
	Len(float v_len, LenType ltype);
    void Normalize();
};

class Vol : public Parameter
{
public:
	Vol(int p_vol);
	Vol(float n_vol);
    void SetNormalValue(float norm_val, bool asliderupdate = false);
    void Reinit();
};

class Level : public Parameter
{
public:
	Level(int p_level);
};

class Pan : public Parameter
{
public:

	Pan(int p_pan);
	Pan(float n_pan);
    void SetNormalValue(float norm_val, bool asliderupdate = false);
    void Normalize();
    void Unnormalize();
    void UpdatePan();
    void ParamEditUpdate();
    void UpdateFromParamedit();
};

class FrequencyParameter : public Parameter
{
public:
	FrequencyParameter(float val);
    void SetNormalValue(float norm_val, bool asliderupdate = false);
};

class LogParam : public Parameter
{
public:
    float offset1;
    float range1;

	LogParam(float val, float offs, float rng);
    void SetNormalValue(float norm_val, bool asliderupdate = false);
};

#endif
