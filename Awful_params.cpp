
#include "awful.h"
#include "awful_params.h"
#include "awful_audio.h"
#include "awful_effects.h"
#include "awful_utils_common.h"
#include "awful_undoredo.h"
#include "awful_cursorandmouse.h"

ParamSet::ParamSet(int p_vol, int p_pan, Scope* sc)
{
	vol = new Vol(p_vol);
    vol->SetName("Vol");
    vol->scope = sc;
    pan = new Pan(p_pan);
    pan->SetName("Pan");
    pan->scope = sc;

    vol->SetPresetable(false);
    pan->SetPresetable(false);

    muted = false;
    solo = false;
}

ParamSet:: ParamSet(float nvol, float npan, Scope* sc)
{
	vol = new Vol(nvol);
    vol->SetName("Vol");
    vol->scope = sc;
    pan = new Pan(npan);
    pan->SetName("Pan");
    pan->scope = sc;

    vol->SetPresetable(false);
    pan->SetPresetable(false);

    muted = false;
    solo = false;
}

void ParamSet::Reset()
{
    vol->Reset();
    pan->Reset();
    muted = false;
    solo = false;
}

Parameter::Parameter()
{
	ParamInit();
}

Parameter::Parameter(float nval, float offs, float rng, ParamType ptype)
{
    ParamInit();
    type = ptype;
    val = outval = defaultval = initialval = nval;
    offset = offs;
    range = rng;
}

Parameter::Parameter(const char* name, float nval, float offs, float rng, ParamType ptype)
{
    ParamInit();
    type = ptype;
    val = outval = defaultval = initialval = nval;
    offset = offs;
    range = rng;
    SetName((char*)name);
}

Parameter::Parameter(float nval)
{
	ParamInit();
    val = outval = defaultval = initialval = nval;
}

Parameter::Parameter(int p_value)
{
    ParamInit();
    p_val = p_base = p_value;
    Normalize();
    range = outval;
}

void Parameter::ParamInit()
{
    prev = next = NULL;
    gprev = gnext = NULL;
    vstring = NULL;
    envelopes = NULL;
    module = NULL;
    element = NULL;
    first_ctrl = NULL;
    last_ctrl = NULL;
    prmedit = NULL;
    aslider = NULL;
    autoenv = NULL;
    cmdenv = NULL;
    lastrecpnt = NULL;
    scope = NULL;
    vstring = NULL;
    type = Param_Default;
    memset(name, 0, MAX_PARAM_NAME);

    val = outval = defaultval = initialval = 0;
    index = 0;
    offset = 0;
    range = 1;
    p_base = 0;
    interval = -1;
    lastval = -1;
    grouped = false;
    reversed = false;
    issigned = false;
    aslider_updating = false;
    recording = false;
    envaffect = true;
    envdirect = true;
    envtweaked = false;
    presetable = true;

    index = 0;
}

Parameter::~Parameter()
{
    Command* cmd;
    Element* el = firstElem;
    while(el != NULL)
    {
        if(el->type == El_Command)
        {
            cmd = (Command*)el;
            if(cmd->param == this)
            {
                cmd->param = NULL;
            }
        }
        el = el->next;
    }

    if(recording == true)
    {
        FinishRecording();
    }

    undoMan->WipeParameterFromHistory(this);
}

void Parameter::ResetToInitial()
{
    SetNormalValue(initialval);
}

void Parameter::SetInitialValue(float initial)
{
    initialval = initial;
    envtweaked = false;
}

void Parameter::SetPercentValue(int p_value)
{
    p_val = p_value;
    Normalize();
    UpdateBindings();
}

void Parameter::SetNormalValue(float norm_val, bool asliderupdate)
{
    aslider_updating = asliderupdate;
    val = norm_val;
    outval = val;

    if(interval > 0)
    {
        outval = float(RoundFloat(outval/interval))*interval;
        val = outval;
    }

    UpdatePerNormalValue();
}

void Parameter::UpdatePerNormalValue()
{
    Unnormalize();
    UpdateBindings();
}

void Parameter::FinishRecording()
{
    if(recording == true)
    {
        recording = false;
        //autoenv->Simplify();
        autoenv = NULL;
        cmdenv = NULL;
        RemoveRecordingParam(this);
    }
}

void Parameter::HandleRecordingFromControl(float ctrlval)
{
    if(Recording == true && Playing && autoenv == NULL)
    {
        // First time envelope creation
        C.SaveState();
        int cline = C.loc == Loc_MainGrid ? CLine : C.fieldposy;
        /*
        // Advance line if it's already busy by a recording param (cancelled)
        Parameter* otherparam = first_rec_param;
        while(otherparam != NULL && cline < 999)
        {
            if(otherparam->cmdenv->track_line == cline)
            {
                cline++;
                otherparam = first_rec_param;
                continue;
            }
            otherparam = otherparam->rec_next;
        }*/
        C.SetPattern(field_pattern, Loc_MainGrid);
        C.SetPos(float(Frame2Tick(pbkMain->currFrame_tsync)), cline);

        cmdenv = CreateCommandFromParam(this);

        autoenv = (Envelope*)cmdenv->paramedit;
        //autoenv->p_first->y_norm = ctrlval;
        autoenv->p_first->big = false;
        autoenv->folded = true;
        AddNewElement(cmdenv, true);

        autoenv->SetLength(Frame2Tick(pbkMain->currFrame_tsync) - cmdenv->start_tick);

        lastrecpnt = autoenv->p_last;
        C.RestoreState();

        if(recording == false)
        {
            recording = true;
            AddRecordingParam(this);
        }

        R(Refresh_GridContent);
    }
}

void Parameter::SetValueFromControl(float ctrlval, bool asliderupdate)
{
    SetNormalValue(ctrlval*range + offset, asliderupdate);
    SetInitialValue(val);
    BlockEnvAffect();  // Block this param update from currently working envelopes
    HandleRecordingFromControl(ctrlval);

    ChangesIndicate();
}

// The only difference between the above function and below is that below sets passed value directly, without
// actualizing it with range and offset
void Parameter::SetDirectValueFromControl(float ctrlval, bool asliderupdate)
{
    SetNormalValue(ctrlval, asliderupdate);
    SetInitialValue(ctrlval);
    BlockEnvAffect();  // Block this param update from currently working envelopes
    HandleRecordingFromControl(ctrlval);

    ChangesIndicate();
}

void Parameter::SetValueFromEnvelope(float envval, Envelope * env, bool asliderupdate)
{
    if(envaffect == true && env != autoenv)
    {
        envtweaked = true;

        // Check for shit
        if(envval < 0 || envval > 1)
            envval = 0;

        SetNormalValue(envval*range + offset, asliderupdate);

        //SetLastVal(outval);
    }
}

void Parameter::SetLastVal(float lval)
{
    lastval = lval;
}

void Parameter::Normalize()
{
    SetNormalValue(Div100((float)p_val));
}

void Parameter::Unnormalize()
{
    p_val = RoundFloat(val*100);
}

void Parameter::Reset()
{
    SetNormalValue(defaultval, false);
}

void Parameter::AddParamedit(PEdit* pe)
{
    prmedit = pe;
    pe->AddParam(this, &val);
}

void Parameter::AddControl(Control* ct)
{
    if((first_ctrl == NULL)&&(last_ctrl == NULL))
    {
        ct->prm_prev = NULL;
        ct->prm_next = NULL;
        first_ctrl = ct;
    }
    else
    {
        last_ctrl->prm_next = ct;
        ct->prm_prev = last_ctrl;
        ct->prm_next = NULL;
    }
    last_ctrl = ct;

    ct->AddParam(this);
    ct->UpdatePosFromVal();
}

void Parameter::RemoveControl(Control* ctrl)
{
	if((ctrl == first_ctrl)&&(ctrl == last_ctrl))
	{
		first_ctrl = NULL;
		last_ctrl = NULL;
	}
	else if(ctrl == first_ctrl)
	{
		first_ctrl = ctrl->prm_next;
		first_ctrl->prm_prev = NULL;
	}
	else if(ctrl == last_ctrl)
	{
		last_ctrl = ctrl->prm_prev;
		last_ctrl->prm_next = NULL;
	}
	else
	{
		if(ctrl->prev != NULL)
		{
		    ctrl->prev->prm_next = ctrl->prm_next;
        }
		if(ctrl->prm_next != NULL)
		{
		    ctrl->prm_next->prm_prev = ctrl->prm_prev;
        }
	}
}

void Parameter::UpdateBindings()
{
    Control* ctrl = first_ctrl;
    while(ctrl != NULL)
    {
        ctrl->UpdatePosFromVal();
        ctrl = ctrl->prm_next;
    }

    if(aslider != NULL)
    {
        aslider->postParamMessage(this);
        
        if(aslider_updating)
        {
            aslider_updating = false;
        }
    }

    if(prmedit != NULL)
    {
        prmedit->UpdateFromParam();
    }

    if(module != NULL && module->to_be_deleted == false)
    {
        module->ParamUpdate(this);
    }

    if(element != NULL)
    {
        element->Update();
    }
}

void Parameter::UpdateFromParamedit()
{
    if(prmedit != NULL)
    {
        p_val = prmedit->value;
        Normalize(); 
        outval = val;
    }
}

void Parameter::Reinit()
{
    outval = val;
}

void Parameter::EnqueueEnvelopeTrigger(Trigger* tg)
{
    tg->tgworking = true;
    if(envelopes != NULL)
    {
        envelopes->group_next = tg;
    }
    tg->group_prev = envelopes;
    tg->group_next = NULL;
    envelopes = tg;

    // Set initial envelope data value as tg->prev_value
    if(type == Param_Pan)
    {
		tg->prev_value = 1 - (val - offset)/range;
    }
    else
    {
		tg->prev_value = (val - offset)/range;
    }

    // New envelopes unblock the param ability to be changed by envelope
    UnblockEnvAffect();
}

void Parameter::DequeueEnvelopeTrigger(Trigger* tg)
{
    if(envelopes == tg)
    {
        envelopes = tg->group_prev;
    }

    if(tg->group_prev != NULL)
    {
        tg->group_prev->group_next = tg->group_next;
    }
    if(tg->group_next != NULL)
    {
        tg->group_next->group_prev = tg->group_prev;
    }
    tg->group_prev = NULL;
    tg->group_next = NULL;
}

void Parameter::SetInterval(float newint)
{
    interval = newint;
}

float Parameter::GetInterval()
{
    return interval;
}

void Parameter::SetReversed(bool rev)
{
    reversed = rev;
}

bool Parameter::GetReversed()
{
    return reversed;
}

void Parameter::SetSigned(bool sgn)
{
    issigned = sgn;
}

bool Parameter::GetSigned()
{
    return issigned;
}

void Parameter::AddValueString(VStrType vstrtype)
{
    vstring = new ValueString(vstrtype);
}

void Parameter::BlockEnvAffect()
{
    envaffect = false;
}

void Parameter::UnblockEnvAffect()
{
    envaffect = true;
}

bool Parameter::isRecording()
{
    return recording;
}

void Parameter::setEnvDirect(bool envdir)
{
    envdirect = envdir;
}

bool Parameter::getEnvDirect()
{
    return envdirect;
}

void Parameter::SetPresetable(bool able)
{
    presetable = able;
}

bool Parameter::IsPresetable()
{
    return presetable;
}

// Dedicated saving methods that avoid saving and loading global index
XmlElement* Parameter::Save4Preset()
{
    XmlElement * xmlParam = new XmlElement(T("Parameter"));
    xmlParam->setAttribute(T("name"), name);
    xmlParam->setAttribute(T("index"), index);
    xmlParam->setAttribute(T("val"), initialval);

    if(type == Param_Bool)
    {
        BoolParam* bp = (BoolParam*)this;
        xmlParam->setAttribute(T("bval"), bp->outval ? 1 : 0);
    }

    return xmlParam;
}

void Parameter::Load4Preset(XmlElement* xmlNode)
{
	if(type == Param_Bool)
	{
		BoolParam* bp = (BoolParam*)this;
		bp->SetBoolValue(xmlNode->getBoolAttribute(T("bval"), bp->outval));
	}
	else
	{
		float fval = (float)xmlNode->getDoubleAttribute(T("val"), initialval);
		SetNormalValue(fval);
		SetInitialValue(fval);
	}
}

XmlElement* Parameter::Save()
{
    XmlElement * xmlParam = new XmlElement(T("Parameter"));
    xmlParam->setAttribute(T("GlobalIndex"), globalindex);
    xmlParam->setAttribute(T("name"), name);
    xmlParam->setAttribute(T("index"), index);
    xmlParam->setAttribute(T("val"), initialval);

    if(type == Param_Bool)
    {
        BoolParam* bp = (BoolParam*)this;
        xmlParam->setAttribute(T("bval"), bp->outval ? 1 : 0);
    }

    return xmlParam;
}

void Parameter::Load(XmlElement* xmlNode)
{
    globalindex = xmlNode->getIntAttribute(T("GlobalIndex"), globalindex);
    /*
    index = xmlNode->getIntAttribute(T("index"), index);
    xmlNode->getStringAttribute(T("name"), T("default")).copyToBuffer(name, MAX_PARAM_NAME);
	*/

	//char xname[MAX_PARAM_NAME];
	//xmlNode->getStringAttribute(T("name"), T("default")).copyToBuffer(xname, MAX_PARAM_NAME);
	//if(strcmp(name, xname) == 0)
	{
		if(type == Param_Bool)
		{
			BoolParam* bp = (BoolParam*)this;
			bp->SetBoolValue(xmlNode->getBoolAttribute(T("bval"), bp->outval));
		}
		else
		{
			float fval = (float)xmlNode->getDoubleAttribute(T("val"), initialval);
			SetNormalValue(fval);
			SetInitialValue(fval);
		}
	}
}

BoolParam::BoolParam(bool value)
{
    type = Param_Bool;
    outval = value;
    atoggle = NULL;
}

BoolParam::BoolParam(bool value, const char* name)
{
    type = Param_Bool;
    SetName((char*)name);
    outval = value;
    atoggle = NULL;
}

void BoolParam::SetBoolValue(bool bval)
{
    outval = bval;

    UpdateBindings();
}

void BoolParam::UpdateBindings()
{
    Parameter::UpdateBindings();
    if(atoggle != NULL)
    {
        const MessageManagerLock mmLock;
        atoggle->setToggleState(outval, false);
    }
}

Vol::Vol(int p_vol)
{
    type = Param_Vol;
    offset = 0;
    range = VolRange;
    p_val = p_base = p_vol;
    Normalize();
    defaultval = val;
    initialval = val;
}

Vol::Vol(float n_val)
{
    type = Param_Vol;
    offset = 0;
    range = VolRange;
    defaultval = n_val;
    initialval = n_val;
    SetNormalValue(n_val);
}

void Vol::SetNormalValue(float norm_val, bool asliderupdate)
{
    aslider_updating = asliderupdate;
    val = norm_val;
    outval = GetVolOutput(val);

    UpdatePerNormalValue();
}

void Vol::Reinit()
{
    SetNormalValue(val);
}

Pan::Pan(int p_pan)
{
    type = Param_Pan;
    offset = -1;
    range = 2;

    p_val = p_base = p_pan;
    Normalize();
    outval = val;
}

Pan::Pan(float n_pan)
{
    type = Param_Pan;
    offset = -1;
    range = 2;

    val = defaultval = initialval = n_pan;
    outval = val;
}

void Pan::Normalize()
{
    SetNormalValue(float(p_val/50) - 1);
}

void Pan::SetNormalValue(float norm_val, bool asliderupdate)
{
    aslider_updating = asliderupdate;
    val = norm_val;
    outval = val;

    UpdatePerNormalValue();
}

void Pan::Unnormalize()
{
    p_val = int((val + 1)*50);
}

void Pan::UpdateFromParamedit()
{
    if(prmedit != NULL)
    {
        p_val = prmedit->value;
        Normalize();
        outval = val;
    }
}

FrequencyParameter::FrequencyParameter(float val) : Parameter(val, 0.0f, 1.0f, Param_Default)
{
	SetNormalValue(val);
}

void FrequencyParameter::SetNormalValue(float norm_val, bool asliderupdate)
{
    aslider_updating = asliderupdate;
    val = norm_val;
    if(reversed == false)
    {
        outval = (float)(20.0*pow(1000.0, (double)val));
    }
    else
    {
        outval = (float)(20.0*pow(1000.0, (double)(1 - val)));
    }

    UpdatePerNormalValue();
}

LogParam::LogParam(float val, float offs, float rng) : Parameter(val, 0.0f, 1.0f, Param_Default)
{
    offset1 = offs;
    range1 = rng;
	SetNormalValue(val);
}

void LogParam::SetNormalValue(float norm_val, bool asliderupdate)
{
    aslider_updating = asliderupdate;
    val = norm_val;
    if(reversed == false)
    {
        outval = (float)((double)offset1*pow((double)range1, (double)val));
    }
    else
    {
        outval = (float)((double)offset1*pow((double)range1, (double)(1 - val)));
    }

    UpdatePerNormalValue();
}

Len::Len(int p_len, LenType ltype) : Parameter(p_len)
{
    type = Param_Len;
    offset = 0;
    range = 1;

    len_type = ltype;
    if(len_type == Len_Percent)
    {
        lval.percent = p_val;
    }
    else if(len_type == Len_Ticks)
    {
        lval.ticks = (float)p_val;
    }
}

Len::Len(float v_len, LenType ltype) : Parameter(v_len)
{
    type = Param_Len;
    offset = 0;
    range = 1;

    len_type = ltype;
    if(len_type == Len_Percent)
    {
        lval.percent = p_val;
    }
    else if(len_type == Len_Ticks)
    {
        lval.ticks = val;
    }
}

void Len::Normalize()
{
    if(len_type == Len_Percent)
    {
       SetNormalValue(val = Div100((float)lval.percent));
    }
}

Level::Level(int p_level) : Parameter(p_level)
{
}

Preset::Preset(Object *owner)
{
    this->owner = owner;
    memset(this->name, 0, (MAX_PRESET_NAME*sizeof(char)));
}
