//=================================================================================================
//                             Include Section
//=================================================================================================

#include "awful.h"
#include "awful_effects.h"
#include "awful_params.h"
#include "awful_panels.h"
#include "awful_controls.h"
#include "awful_utils_common.h"
#include "awful_undoredo.h"
#include "VSTCollection.h"
#include <direct.h>
#include <errno.h>

//=================================================================================================
//                             Local functions declaration
//=================================================================================================
void VSTEffEditButtonOnClick(Object* owner, Object* self);
void EffWindowClick(Object* owner, Object* self);


//=================================================================================================
//                                  Eff Class Implementation
//=================================================================================================
Eff::Eff(Mixcell* mc)
{
    this->modtype = ModuleType_Effect;

    x = y = width = height = 1;
    folded = true;
    bypass = false;
    off = false;
    muteCount = 0;
    visible = false;
    wndvisible = false;
    prev = NULL;
    next = NULL;
    cprev = NULL;
    cnext = NULL;
    envelopes = NULL;
    index = -1;

    scope.eff = this;

    SetNewMixcell(mc);

    fold_toggle = new Toggle(&folded, &scope, Toggle_EffFold);
    AddNewControl(fold_toggle, NULL);
    fold_toggle->SetOnClickHandler(&EffFoldClick);
    fold_toggle->SetHint("Fold/unfold effect");

    bypass_toggle = new Toggle(&bypass, &scope, Toggle_EffBypass);
    AddNewControl(bypass_toggle, NULL);
    bypass_toggle->SetOnClickHandler(&EffBypassClick);
    bypass_toggle->SetHint("Turn effect on/off");

    wnd_toggle = new Toggle(&wndvisible, &scope, Toggle_EffWindow);
    AddNewControl(wnd_toggle, NULL);
    wnd_toggle->SetOnClickHandler(&EffWindowClick);
    wnd_toggle->SetHint("Show editor window");

    memset(this->name, 0, MAX_NAME_STRING*sizeof(char));
    memset(this->preset_path, 0, MAX_PATH_STRING*sizeof(char));
    memset(this->path, 0, MAX_PATH_STRING*sizeof(char));
}

Eff::~Eff()
{
    DereferenceElements();

    undoMan->WipeEffectFromHistory(this);
}

////////////////////////////////////////////////////////
// Take care of elements, related to this effect upon deletion
void Eff::DereferenceElements()
{
    // If some envelopes were extracted from this effect, disable them
    Element* elnext;
    Element* el = firstElem;
    while(el != NULL)
    {
        elnext = el->next;
        if(el->type == El_Command)
        {
            Command* cmd = (Command*)el;
            if(cmd->scope == &scope)
            {
                elnext = NextElementToDelete(el);
                DeleteElement(cmd, true, true);
            }
        }
        el = elnext;
    }
}

void Eff::Disable()
{
    DisableParamCells();
}

void Eff::Enable()
{
	/*
    if(mixcell != NULL && mixcell->folded == true)
    {
        
    }
    else */
	if(folded == false)
    {
        EnableParamCells();
    }
}

void Eff::QueueDeletion()
{
    to_be_deleted = true;
    Disable();
    DereferenceElements();
    fold_toggle->Deactivate();
    bypass_toggle->Deactivate();
}

int Eff::GetHeight()
{
    int h = 0;
    if(mixcell->folded == false)
    {
        Paramcell* pcell = first_pcell;
        while(pcell != NULL)
        {
            if(pcell->visible)
                h += pcell->h;
            pcell = pcell->next;
        }
    }
    return h;
}

void Eff::SetNewMixcell(Mixcell* ncell)
{
    mixcell = ncell;
    scope.mixcell = ncell;
}

void Eff::Process(float* in_buff, float* out_buff, int num_frames)
{
    if(envelopes == NULL && (bypass == false || muteCount < DECLICK_COUNT))
    {
        ProcessData(in_buff, out_buff, num_frames);
    }
    else if(envelopes != NULL)
    {
        Envelope* env;
        Parameter* param;
        Trigger* tgenv;
        long frames_to_process;
        long buffframe = 0;
        long frames_remaining = num_frames;
        while(frames_remaining > 0)
        {
            if(frames_remaining > 32)
            {
                frames_to_process = 32;
            }
            else
            {
                frames_to_process = frames_remaining;
            }

            tgenv = envelopes;
            while(tgenv != NULL)
            {
                env = (Envelope*)((Command*)tgenv->el)->paramedit;
				if(buffframe >= env->last_buffframe)
				{
					param = ((Command*)tgenv->el)->param;
					param->SetValueFromEnvelope(env->buffoutval[buffframe], env);
				}
                tgenv = tgenv->group_prev;
            }

            if(bypass == false || muteCount < DECLICK_COUNT)
            {
                ProcessData(&in_buff[buffframe*2], &out_buff[buffframe*2], frames_to_process);
            }

            frames_remaining -= frames_to_process;
            buffframe += frames_to_process;
        }
    }

    if(bypass == true && muteCount >= DECLICK_COUNT)
    {
        memcpy(out_buff, in_buff, num_frames*2*sizeof(float));
    }
}

void Eff::Save(XmlElement * xmlEff)
{
    xmlEff->setAttribute(T("EffIndex"), index);

    xmlEff->setAttribute(T("EffType"), int(subtype));
    xmlEff->setAttribute(T("EffName"), String(name));
    xmlEff->setAttribute(T("EffPath"), String(path));

    xmlEff->setAttribute(T("Folded"), folded ? 1 : 0);
    xmlEff->setAttribute(T("Bypass"), bypass ? 1 : 0);

    //if(type != EffType_VSTPlugin)
    {
        SaveStateData(*xmlEff, "Current", true);
    }
}

void Eff::Load(XmlElement * xmlEff)
{
    index = xmlEff->getIntAttribute(T("EffIndex"));

    folded = xmlEff->getBoolAttribute(T("Folded"));
    bypass = xmlEff->getBoolAttribute(T("Bypass"));

    //if(type != EffType_VSTPlugin)
    {
        XmlElement* stateNode = xmlEff->getChildByName(T("Module"));
        if(stateNode != NULL)
        {
            RestoreStateData(*stateNode, true);
        }
    }
}

//=================================================================================================
//                                          Blank effect
//=================================================================================================

BlankEffect::BlankEffect(Mixcell* mc, AliasRecord* ar) : Eff(mc)
{
    subtype = ModSubtype_Default;
    strcpy(this->name, "Blank effect");
    strcpy(this->preset_path, ".\\Presets\\Blank Effect\\");
    strcpy(this->path,"internal://blank");
    uniqueID = MAKE_FOURCC('B','L','N','K');

    arec = ar;

    // Create 1st slider param
    level = new Parameter(0.5f, 0.0f, 1.0f, Param_Default);
    level->SetName("blank percent");
    vstr_level = new ValueString(VStr_Percent);
    level->vstring = vstr_level;
    AddParamWithParamcell(level);

    // Create 2nd slider param
    level1 = new Parameter(220.0f, 0.0f, 3200.0f, Param_Default);
    level1->SetName("blank Hz");
    vstr_level1 = new ValueString(VStr_Hz);
    level1->vstring = vstr_level1;
    AddParamWithParamcell(level1);

    // Create 3rd slider param
    level2 = new Parameter(5000.0f, 0.0f, 10000.0f, Param_Default);
    level2->SetName("blank kHz");
    vstr_level2 = new ValueString(VStr_kHz);
    level2->vstring = vstr_level2;
    AddParamWithParamcell(level2);

    // Create 1st toggle
    d1 = new BoolParam(false);
    d1->SetName("Tg1");
    d1_state = new ValueString(VStr_String);
    d1->vstring = d1_state;
    AddParamWithParamcell(d1);

    // Create 2nd toggle
    d2 = new BoolParam(true);
    d2->SetName("Tg2");
    d2_state = new ValueString(VStr_String);
    d2->vstring = d2_state;
    AddParamWithParamcell(d2);

    // Create radiobuttons set
    radiocell = new Paramcell(PCType_MixRadio, NULL, mixcell, &scope);
    radiocell->SetTitle("Choose a value");
    AddParamcell(radiocell);
    r1 = new BoolParam(true);
    r1->SetName("Val1");
    r1_state = new ValueString(VStr_String);
    r1_state->SetValue("Value1");
    r1->vstring = r1_state;
    r2 = new BoolParam(false);
    r2->SetName("Val2");
    r2_state = new ValueString(VStr_String);
    r2_state->SetValue("Value2");
    r2->vstring = r2_state;
    r3 = new BoolParam(false);
    r3->SetName("Val3");
    r3_state = new ValueString(VStr_String);
    r3_state->SetValue("Value3");
    r3->vstring = r3_state;
    r4 = new BoolParam(false);
    r4->SetName("Val4");
    r4_state = new ValueString(VStr_String);
    r4_state->SetValue("Value4");
    r4->vstring = r4_state;
    AddParamToParamcell(r1, radiocell);
    AddParamToParamcell(r2, radiocell);
    AddParamToParamcell(r3, radiocell);
    AddParamToParamcell(r4, radiocell);

    ParamUpdate();
}

BlankEffect::~BlankEffect()
{
    // Delete all value strings
    delete vstr_level;
    delete vstr_level1;
    delete vstr_level2;
}

BlankEffect* BlankEffect::Clone(Mixcell* mc)
{
    BlankEffect* clone = new BlankEffect(mc, arec);
    clone->level->SetNormalValue(level->val);
    clone->ParamUpdate();

    return clone;
}

void BlankEffect::ParamUpdate(Parameter* param)
{
    vstr_level->SetValue(int(level->outval*100));
    vstr_level1->SetValue(int(level1->outval));
    vstr_level2->SetValue((level2->outval/1000));

    if(d1->outval == true)
    {
        d1_state->SetValue("State 1 ON");
    }
    else
    {
        d1_state->SetValue("State 1 OFF");
    }
    if(d2->outval == true)
    {
        d2_state->SetValue("State 2 ON");
    }
    else
    {
        d2_state->SetValue("State 2 OFF");
    }
}

void BlankEffect::ProcessData(float* in_buff, float* out_buff, int num_frames)
{
    memcpy(out_buff, in_buff, num_frames*sizeof(float)*2);

    float* out_p = out_buff;
    int i = 0;
    while(i < num_frames)
    {
       *out_p = level->outval*(*out_p++);
       *out_p = level->outval*(*out_p++);
        i++;
    }
}

//=================================================================================================
//                             Gain Class Implementation
//=================================================================================================

Gain::Gain(Mixcell* mc, AliasRecord* ar) : Eff(mc)
{
    subtype = ModSubtype_Gain;

    strcpy(this->name, "Gain");
    strcpy(this->preset_path, ".\\Data\\Presets\\Effects\\Native\\Gain\\");
    strcpy(this->path,"internal://gain");
    uniqueID = MAKE_FOURCC('G','A','I','N');

    arec = ar;

    level = new Parameter(1.0f, 0.0f, 2.0f, Param_Default);
    level->SetName("level");
    AddParam(level);

    r_level = new Knob(11, level, NULL, &scope, Knob_XLarge);
    level->AddControl(r_level);
    AddNewControl(r_level, NULL);

    v_level = new ValueString(VStr_Default);
    level->vstring = v_level;

    ParamUpdate();
}

Gain::~Gain()
{
    delete v_level;
}

Gain* Gain::Clone(Mixcell* mc)
{
    Gain* clone = new Gain(mc, arec);
    clone->level->SetNormalValue(level->outval);
    clone->ParamUpdate();

	return clone;
}

void Gain::ProcessData(float* in_buff, float* out_buff, int num_frames)
{
    memcpy(out_buff, in_buff, num_frames*sizeof(float)*2);

    float* out_p = out_buff;
    int i = 0;
    while(i < num_frames)
    {
       *out_p = level->outval*(*out_p++);
       *out_p = level->outval*(*out_p++);
        i++;
    }
}

void Gain::ParamUpdate(Parameter* param)
{
    v_level->SetValue(this->level->outval);
}

//=================================================================================================
//                             Filter Class Implementation
//=================================================================================================

Filter::Filter(Mixcell* mc, AliasRecord* ar) : Eff(mc)
{
    subtype = ModSubtype_Filter;
    strcpy(this->name, "Simple Filter");
    strcpy(this->preset_path, ".\\Presets\\Simple Filter\\");
    strcpy(this->path,"internal://lp");
    uniqueID = MAKE_FOURCC('F','L','T','R');

    arec = ar;

    cutoff = new Parameter(700.0f, 200.0f, 9800.0f, Param_Default);
    cutoff->SetName("cutoff");
    v_cutoff = new ValueString(VStr_kHz);
    cutoff->vstring = v_cutoff;
    AddParamWithParamcell(cutoff);

    resonance = new Parameter(5.5f, 1.0f, 10.0f, Param_Default);
    resonance->SetName("resonance");
    v_resonance = new ValueString(VStr_Percent);
    resonance->vstring = v_resonance;
    AddParamWithParamcell(resonance);

    // Filter work data
    lx1 = lx2 = ly1 = ly2 = 0.0f;
    rx1 = rx2 = ry1 = ry2 = 0.0f;
    Reset();

    ParamUpdate();
}

Filter* Filter::Clone(Mixcell* mc)
{
    Filter* clone = new Filter(mc, arec);
    clone->cutoff->SetNormalValue(cutoff->val);
    clone->resonance->SetNormalValue(resonance->val);
    clone->ParamUpdate();

	return clone;
}

void Filter::ParamUpdate(Parameter* param)
{
    v_cutoff->SetValue((float)(cutoff->outval/1000.0f));
    v_resonance->SetValue(int((resonance->outval - 1.0f)/10.0f*100));
    Reset();
}

void Filter::Reset()
{
    float alpha, omega, sn, cs;
    float a0, a1, a2, b0, b1, b2;

    omega = (float)(2.0f*PI*cutoff->outval/fSampleRate);
    sn = sin (omega); 
    cs = cos (omega);
    alpha = sn/resonance->outval;
    b0 = (1.0f - cs)/2.0f;
    b1 = 1.0f - cs;
    b2 = (1.0f - cs)/2.0f;
    a0 = 1.0f + alpha;
    a1 = -2.0f*cs;
    a2 = 1.0f - alpha;

    filtCoefTab[0] = b0/a0;
    filtCoefTab[1] = b1/a0;
    filtCoefTab[2] = b2/a0;
    filtCoefTab[3] = -a1/a0;
    filtCoefTab[4] = -a2/a0;
}

void Filter::ProcessData(float* in_buff, float* out_buff, int num_frames)
{
    float inL, inR, outL, outR, temp_y;
    int i;

    long fc = 0;
    for(i=0; i < num_frames; i++)
    {
        inL = in_buff[fc];
        inR = in_buff[fc + 1];

        outL = inL;
        outR = inR;

        // Left
        temp_y = filtCoefTab[0]*outL +
                 filtCoefTab[1]*lx1 +
                 filtCoefTab[2]*lx2 +
                 filtCoefTab[3]*ly1 +
                 filtCoefTab[4]*ly2;
        ly2 = ly1; 
        ly1 = temp_y; 
        lx2 = lx1; 
        lx1 = outL ; 
        outL = temp_y;

        // Right
        temp_y = filtCoefTab[0]*outR +
                 filtCoefTab[1]*rx1 +
                 filtCoefTab[2]*rx2 +
                 filtCoefTab[3]*ry1 +
                 filtCoefTab[4]*ry2;
        ry2 = ry1; 
        ry1 = temp_y; 
        rx2 = rx1; 
        rx1 = outR ; 
        outR = temp_y;

        out_buff[fc++] = outL;
        out_buff[fc++] = outR;
    };
}

CFilter3::CFilter3(Mixcell* mc, AliasRecord* ar) : Eff(mc)
{
    subtype = ModSubtype_CFilter3;
    strcpy(this->name, "Filter");
    strcpy(this->preset_path, ".\\Presets\\Filter\\");
    strcpy(this->path,"internal://filter3");
    uniqueID = MAKE_FOURCC('C','F','L','T');

    arec = ar;
    //dspCoreCFilter3.useTwoStages(false);
    f_master = false;
    f_next = NULL;

    dspCoreCFilter3.setMakeUp(100.f);
    dspCoreCFilter3.setDcOffset(0.f);
    dspCoreCFilter3.setOutputStage(4);

    cutoff = new FrequencyParameter(0.6f);
    cutoff->SetName("Cutoff");
    cutoff->AddValueString(VStr_Hz);
    AddParamWithParamcell(cutoff);

    resonance = new Parameter(0.f, 0.0f, 0.97f, Param_Default);
    resonance->SetName("Resonance");
    resonance->AddValueString(VStr_Percent);
    AddParamWithParamcell(resonance);

    //bandwidth = new Parameter(1.f, 0.25f, 3.75f, Param_Default);
    //bandwidth->SetName("Bandwidth");
    //bandwidth->AddValueString(VStr_oct);
    //AddParamWithParamcell(bandwidth);

    //Q = new Parameter(0.f, 0.f, 50.f, Param_Default);
    //Q->SetName("Q");
    //Q->AddValueString(VStr_Default);
    //AddParamWithParamcell(Q);

    // Create radiobuttons set
    ftype = new Paramcell(PCType_MixRadio, NULL, NULL, &scope);
    ftype->SetTitle("");
    AddParamcell(ftype);
    f1 = new BoolParam(false);
    f1->SetName("LP12");
    f1_state = new ValueString(VStr_String);
    f1_state->SetValue("Lowpass");
    f1->vstring = f1_state;
    f2 = new BoolParam(false);
    f2->SetName("HP12");
    f2_state = new ValueString(VStr_String);
    f2_state->SetValue("Highpass");
    f2->vstring = f2_state;
    f3 = new BoolParam(false);
    f3->SetName("BP");
    f3_state = new ValueString(VStr_String);
    f3_state->SetValue("Bandpass");
    f3->vstring = f3_state;
    AddParamToParamcell(f1, ftype);
    AddParamToParamcell(f2, ftype);
    AddParamToParamcell(f3, ftype);
    ftype->SetCurrentItemByTitle("LP12");

    x2 = new BoolParam(false);
    x2->SetName("Mode");
    x2->AddValueString(VStr_String);
    AddParamWithParamcell(x2);
    x2->vstring->SetValue("x2");

    //ParamUpdate(bandwidth);
    ParamUpdate(f1);
    ParamUpdate(f2);
    ParamUpdate(f3);
    ParamUpdate(cutoff);
    ParamUpdate(resonance);
    ParamUpdate(x2);
}

CFilter3* CFilter3::Clone(Mixcell* mc)
{
    CFilter3* clone = new CFilter3(mc, arec);
    clone->cutoff->SetNormalValue(cutoff->val);
    clone->resonance->SetNormalValue(resonance->val);
    //clone->bandwidth->SetNormalValue(bandwidth->val);
    clone->ftype->SetCurrentItemByTitle(ftype->tgp_current->title);
    clone->x2->SetBoolValue(x2->outval);
    clone->ParamUpdate();

	return clone;
}

void CFilter3::ParamUpdate(Parameter* param)
{
    if(param == f1)
	{
		BoolParam* bp = (BoolParam*)param;
		if(bp->outval == true)
		{
			dspCoreCFilter3.setMode(rosic::LadderFilterParameters::LOWPASS);
		}
    }
    else if(param == f2)
	{
		BoolParam* bp = (BoolParam*)param;
		if(bp->outval == true)
		{
			dspCoreCFilter3.setMode(rosic::LadderFilterParameters::HIGHPASS);
		}
	}
    else if(param == f3)
	{
		BoolParam* bp = (BoolParam*)param;
		if(bp->outval == true)
        {
            dspCoreCFilter3.setMode(rosic::LadderFilterParameters::BANDPASS_12_12);
        }
    }
    else if(param == cutoff)
	{
        dspCoreCFilter3.setCutoff((double)cutoff->outval);
        cutoff->vstring->SetValue((cutoff->outval));
    }
    else if(param == resonance)
	{
        dspCoreCFilter3.setResonance((double)resonance->outval);
        resonance->vstring->SetValue(int(resonance->outval/0.97f*100.0f)); // /66.0f*100.0f
    }
	else if(param == bandwidth)
	{
		//dspCoreCFilter2.setBandwidth(bandwidth->outval);
		//v_bandwidth->SetValue(bandwidth->outval);
	}
    else if(param == x2)
	{
	    if(x2->outval == true)
        {
            dspCoreCFilter3.setOutputStage(4);
        }
        else
        {
            dspCoreCFilter3.setOutputStage(2);
        }
    }

    if(f_master == true)
    {
        CFilter3* f = f_next;
        while(f != NULL)
        {
            if(param == f1 || param == f2 || param == f3)
            {
                f->dspCoreCFilter3.setMode(dspCoreCFilter3.getMode());
            }
            else if(param == cutoff)
            {
                f->dspCoreCFilter3.setCutoff(dspCoreCFilter3.getCutoff());
            }
            else if(param == resonance)
            {
                f->dspCoreCFilter3.setResonance(dspCoreCFilter3.getResonance());
            }
            else if(param == x2)
        	{
                f->dspCoreCFilter3.setOutputStage(dspCoreCFilter3.getOutputStage());
            }

            f = f->f_next;
        }
    }
}

void CFilter3::Reset()
{
	dspCoreCFilter3.reset();
}

void CFilter3::ProcessData(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;
    for(int i = 0; i < 2*num_frames; i += 2)
	{
		inOutL = (double) in_buff[i];
		inOutR = (double) in_buff[i + 1];
		dspCoreCFilter3.getSampleFrameStereo(&inOutL, &inOutR);
		out_buff[i]    = (float) inOutL;
		out_buff[i + 1]  = (float) inOutR;
	}
}

CFilter2::CFilter2(Mixcell* mc, AliasRecord* ar) : Eff(mc)
{
    subtype = ModSubtype_CFilter2;
    strcpy(this->name, "Filter");
    strcpy(this->preset_path, ".\\Presets\\Filter\\");
    strcpy(this->path,"internal://filter2");
    uniqueID = MAKE_FOURCC('C','F','L','T');

    arec = ar;
    dspCoreCFilter2.useTwoStages(false);

    //dspCoreCFilter2.setMakeUp(100.f);
    //dspCoreCFilter2.setDcOffset(0.f);

    cutoff = new FrequencyParameter(0.6f);
    cutoff->SetName("Cutoff");
    cutoff->AddValueString(VStr_Hz);
    AddParamWithParamcell(cutoff);

    resonance = new Parameter(0.f, 0.0f, 50.0f, Param_Default);
    resonance->SetName("Resonance");
    resonance->AddValueString(VStr_Default);
    AddParamWithParamcell(resonance);

    //bandwidth = new Parameter(1.f, 0.25f, 3.75f, Param_Default);
    //bandwidth->SetName("Bandwidth");
    //bandwidth->AddValueString(VStr_oct);
    //AddParamWithParamcell(bandwidth);

    //Q = new Parameter(0.f, 0.f, 50.f, Param_Default);
    //Q->SetName("Q");
    //Q->AddValueString(VStr_Default);
    //AddParamWithParamcell(Q);

    // Create radiobuttons set
    ftype = new Paramcell(PCType_MixRadio, NULL, NULL, &scope);
    ftype->SetTitle("");
    AddParamcell(ftype);
    f1 = new BoolParam(false);
    f1->SetName("LP12");
    f1_state = new ValueString(VStr_String);
    f1_state->SetValue("Lowpass");
    f1->vstring = f1_state;
    f2 = new BoolParam(false);
    f2->SetName("HP12");
    f2_state = new ValueString(VStr_String);
    f2_state->SetValue("Highpass");
    f2->vstring = f2_state;
    f3 = new BoolParam(false);
    f3->SetName("BP");
    f3_state = new ValueString(VStr_String);
    f3_state->SetValue("Bandpass");
    f3->vstring = f3_state;
    AddParamToParamcell(f1, ftype);
    AddParamToParamcell(f2, ftype);
    AddParamToParamcell(f3, ftype);
    ftype->SetCurrentItemByTitle("LP12");

    x2 = new BoolParam(false);
    x2->SetName("Mode");
    x2->AddValueString(VStr_String);
    AddParamWithParamcell(x2);
    x2->vstring->SetValue("x2");

    //ParamUpdate(bandwidth);
    ParamUpdate(f1);
    ParamUpdate(f2);
    ParamUpdate(f3);
    ParamUpdate(cutoff);
    ParamUpdate(resonance);
}

CFilter2* CFilter2::Clone(Mixcell* mc)
{
    CFilter2* clone = new CFilter2(mc, arec);
    clone->cutoff->SetNormalValue(cutoff->val);
    clone->resonance->SetNormalValue(resonance->val);
    clone->bandwidth->SetNormalValue(bandwidth->val);
    clone->ftype->SetCurrentItemByTitle(ftype->tgp_current->title);
    clone->x2->SetBoolValue(x2->outval);
    clone->ParamUpdate();

	return clone;
}

void CFilter2::ParamUpdate(Parameter* param)
{
    if(param == f1)
	{
		BoolParam* bp = (BoolParam*)param;
		if(bp->outval == true)
		{
			dspCoreCFilter2.setMode(rosic::FourPoleFilterParameters::LOWPASS_12);
		}
    }
    else if(param == f2)
	{
		BoolParam* bp = (BoolParam*)param;
		if(bp->outval == true)
		{
			dspCoreCFilter2.setMode(rosic::FourPoleFilterParameters::HIGHPASS_12);
		}
	}
    else if(param == f3)
	{
		BoolParam* bp = (BoolParam*)param;
		if(bp->outval == true)
        {
            dspCoreCFilter2.setMode(rosic::FourPoleFilterParameters::BANDPASS_RBJ);
        }
    }
    else if(param == cutoff)
	{
        dspCoreCFilter2.setFrequency((double)cutoff->outval);
        cutoff->vstring->SetValue((cutoff->outval));
    }
    else if(param == resonance)
	{
        dspCoreCFilter2.setQ((double)resonance->outval);
        resonance->vstring->SetValue((resonance->outval)); // /66.0f*100.0f
    }
    else if(param == x2)
	{
        dspCoreCFilter2.useTwoStages(x2->outval);
    }
	else if(param == bandwidth)
	{
		//dspCoreCFilter2.setBandwidth(bandwidth->outval);
		//v_bandwidth->SetValue(bandwidth->outval);
	}

    dspCoreCFilter2.updateFilterCoefficients();
}

void CFilter2::Reset()
{
	dspCoreCFilter2.reset();
}

void CFilter2::ProcessData(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;
    for(int i = 0; i < 2*num_frames; i += 2)
	{
		inOutL = (double) in_buff[i];
		inOutR = (double) in_buff[i + 1];
		dspCoreCFilter2.getSampleFrameStereo(&inOutL, &inOutR);
		out_buff[i]    = (float) inOutL;
		out_buff[i + 1]  = (float) inOutR;
	}
}

CFilter::CFilter(Mixcell* mc, AliasRecord* ar) : Eff(mc)
{
    subtype = ModSubtype_CFilter;

    strcpy(this->name, "Filter");
    strcpy(this->preset_path, ".\\Presets\\Filter\\");
    strcpy(this->path,"internal://filter");
    uniqueID = MAKE_FOURCC('C','F','L','T');
    arec = ar;

    //dspCoreCFilter.useTwoStages(true);
    f_master = false;
    f_next = NULL;

    cutoff = new FrequencyParameter(0.6f);
    cutoff->SetName("Cutoff");
    v_cutoff = new ValueString(VStr_Hz);
    cutoff->vstring = v_cutoff;
    cutpcell = AddParamWithParamcell(cutoff);

    resonance = new Parameter(0.f, 0.0f, 48.0f, Param_Default);
    resonance->SetName("Resonance");
    v_resonance = new ValueString(VStr_Percent);
    resonance->vstring = v_resonance;
    respcell = AddParamWithParamcell(resonance);

    bandwidth = new Parameter(1.f, 0.25f, 3.75f, Param_Default);
    bandwidth->SetName("Bandwidth");
    v_bandwidth = new ValueString(VStr_oct);
    bandwidth->vstring = v_bandwidth;
    bandpcell = AddParamWithParamcell(bandwidth);
    bandpcell->SetVisible(false);

    //Q = new Parameter(0.1f, 0.1f, 99.9f, Param_Default);
    //Q->SetName("Q");
    //Q->AddValueString(VStr_Default);
    //AddParamWithParamcell(Q);

    // Create radiobuttons set
    ftype = new Paramcell(PCType_MixRadio, NULL, NULL, &scope);
    ftype->SetTitle("");
    AddParamcell(ftype);
    f1 = new BoolParam(false);
    f1->SetName("LP12");
    f1_state = new ValueString(VStr_String);
    f1_state->SetValue("Lowpass");
    f1->vstring = f1_state;
    f2 = new BoolParam(false);
    f2->SetName("HP12");
    f2_state = new ValueString(VStr_String);
    f2_state->SetValue("Highpass");
    f2->vstring = f2_state;
    f3 = new BoolParam(false);
    f3->SetName("BP");
    f3_state = new ValueString(VStr_String);
    f3_state->SetValue("Bandpass");
    f3->vstring = f3_state;
    AddParamToParamcell(f1, ftype);
    AddParamToParamcell(f2, ftype);
    AddParamToParamcell(f3, ftype);
    ftype->SetCurrentItemByTitle("LP12");

    ParamUpdate(bandwidth);
    //ParamUpdate(Q);
    ParamUpdate(f1);
    ParamUpdate(f2);
    ParamUpdate(f3);
    ParamUpdate(cutoff);
    ParamUpdate(resonance);
}

CFilter* CFilter::Clone(Mixcell* mc)
{
    CFilter* clone = new CFilter(mc, arec);
    clone->cutoff->SetNormalValue(cutoff->val);
    clone->resonance->SetNormalValue(resonance->val);
    clone->bandwidth->SetNormalValue(bandwidth->val);
    clone->Q->SetNormalValue(Q->val);
    clone->ftype->SetCurrentItemByTitle(ftype->tgp_current->title);
    if(dspCoreCFilter.getMode() == rosic::TwoPoleFilter::BANDPASS)
    {
        clone->InsertBandwidth();
    }
    clone->ParamUpdate();

	return clone;
}

void CFilter::RemoveBandwidth()
{
    bandpcell->SetVisible(false);
    respcell->SetVisible(true);
    if(folded == true)
    {
        respcell->Disable();
    }

    if(mixBrw->brwmode == Browse_Params)
    {
        mixBrw->Update();
    }

	if(mixcell != NULL)
	{
		if(mixcell->mchan != NULL)
		{
			mixcell->mchan->drawarea->Change();
		}
		else
		{
			R(Refresh_Mixer);
		}
	}
}

void CFilter::InsertBandwidth()
{
    respcell->SetVisible(false);
    bandpcell->SetVisible(true);
    if(folded == true)
    {
        bandpcell->Disable();
    }
    if(mixBrw->brwmode == Browse_Params)
    {
        mixBrw->Update();
    }

	if(mixcell != NULL)
	{
		if(mixcell->mchan != NULL)
		{
			mixcell->mchan->drawarea->Change();
		}
		else
		{
			R(Refresh_Mixer);
		}
	}
}

void CFilter::ParamUpdate(Parameter* param)
{
    if(param == f1)
    {
        BoolParam* bp = (BoolParam*)param;
        if(bp->outval == true)
        {
            dspCoreCFilter.setMode(rosic::TwoPoleFilter::LOWPASS12);
        }
    }
    else if(param == f2)
	{
		BoolParam* bp = (BoolParam*)param;
		if(bp->outval == true)
		{
			dspCoreCFilter.setMode(rosic::TwoPoleFilter::HIGHPASS12);
		}
	}
    else if(param == f3)
	{
		BoolParam* bp = (BoolParam*)param;
		if(bp->outval == true)
        {
	        dspCoreCFilter.setMode(rosic::TwoPoleFilter::BANDPASS);
        }
    }
    else if(param == cutoff)
	{
        dspCoreCFilter.setFrequency((double)cutoff->outval);
        v_cutoff->SetValue((cutoff->outval));
    }
    else if(param == resonance)
	{
        dspCoreCFilter.setGain((double)resonance->outval);
        v_resonance->SetValue((int)(resonance->outval/48.0f*100.0f));
    }
	else if(param == bandwidth)
	{
		dspCoreCFilter.setBandwidth(bandwidth->outval);
		v_bandwidth->SetValue(bandwidth->outval);
	}
	else if(param == Q)
	{
		//dspCoreCFilter.setQ(Q->outval);
		//Q->vstring->SetValue(Q->outval);
	}

    if(f_master == true)
    {
        CFilter* f = f_next;
        while(f != NULL)
        {
            if(param == f1 || param == f2 || param == f3)
            {
                f->dspCoreCFilter.setMode(dspCoreCFilter.getMode());
            }
            else if(param == cutoff)
            {
                f->dspCoreCFilter.setFrequency(dspCoreCFilter.getFrequency());
            }
            else if(param == resonance)
            {
                f->dspCoreCFilter.setGain(dspCoreCFilter.getGain());
            }

            f = f->f_next;
        }
    }
}

void CFilter::Reset()
{
	dspCoreCFilter.reset();
}

void CFilter::ProcessData(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;
    for(int i = 0; i < 2*num_frames; i += 2)
	{
		inOutL = (double) in_buff[i];
		inOutR = (double) in_buff[i + 1];
		dspCoreCFilter.getSampleFrameStereo(&inOutL, &inOutR);
		out_buff[i]    = (float) inOutL;
		out_buff[i + 1]  = (float) inOutR;
	}
}

//=================================================================================================
//                             CChorus Class Implementation
//=================================================================================================

CChorus::CChorus(Mixcell* mc, AliasRecord* ar) : Eff(mc)
{
    subtype = ModSubtype_Chorus;
    strcpy(this->name, "Chorus");
    strcpy(this->preset_path, ".\\Presets\\Chorus\\");
    strcpy(this->path,"internal://chorus");
    uniqueID = MAKE_FOURCC('C','H','O','R');

    arec = ar;

    dspCoreChorus = new rosic::Chorus(65535);
    dspCoreChorus->setTempoSync(false);

    delay = new Parameter(5.f, 1.f, 49.0f, Param_Default);
    delay->SetName("Delay");
    v_delay = new ValueString(VStr_msec); 
    delay->vstring = v_delay;
    AddParamWithParamcell(delay);

    freq = new Parameter(2.f, 0.1f, 4.9f, Param_Default);
    freq->SetName("Mod. Freq");
    v_freq = new ValueString(VStr_fHz);
    freq->vstring = v_freq;
    AddParamWithParamcell(freq);

    depth = new Parameter(0.25f, 0, 1.5f, Param_Default);
    depth->SetName("Depth");
    v_depth = new ValueString(VStr_Semitones);
    depth->vstring = v_depth;
    AddParamWithParamcell(depth);

    drywet = new Parameter(0.5f, 0.0f, 1.f, Param_Default);
    drywet->SetName("DryWet");
    v_drywet = new ValueString(VStr_DryWet); 
    drywet->vstring = v_drywet;
    AddParamWithParamcell(drywet);

    ParamUpdate(drywet);
    ParamUpdate(delay);
    ParamUpdate(freq);
    ParamUpdate(depth);
}

CChorus* CChorus::Clone(Mixcell* mc)
{
    CChorus* clone = new CChorus(mc, arec);
    clone->drywet->SetNormalValue(drywet->val);
    clone->delay->SetNormalValue(delay->val);
    clone->freq->SetNormalValue(freq->val);
    clone->depth->SetNormalValue(depth->val);

	return clone;
}

void CChorus::ParamUpdate(Parameter* param)
{
    if(param == drywet)
	{
        dspCoreChorus->setDryWetRatio(drywet->outval);
        v_drywet->SetValue((drywet->outval));
    }
    else if(param == delay)
	{
        dspCoreChorus->setAverageDelayTime(delay->outval);
        v_delay->SetValue(delay->outval);
    }
	else if(param == freq)
	{
		dspCoreChorus->setCycleLength(1.f/freq->outval);
		v_freq->SetValue(freq->outval);
	}
	else if(param == depth)
	{
		dspCoreChorus->setDepth(depth->outval);
		v_depth->SetValue(depth->outval);
	}
}

void CChorus::Reset()
{
	//dspCoreChorus.reset();
}

void CChorus::ProcessData(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;
    for(int i = 0; i < 2*num_frames; i += 2)
	{
		inOutL = (double) in_buff[i];
		inOutR = (double) in_buff[i+1];
		dspCoreChorus->getSampleFrameStereo(&inOutL, &inOutR);
		out_buff[i]    = (float) inOutL;
		out_buff[i+1]  = (float) inOutR;
	}
}

//=================================================================================================
//                             CFlanger Class Implementation
//=================================================================================================

CFlanger::CFlanger(Mixcell* mc, AliasRecord* ar) : Eff(mc)
{
    subtype = ModSubtype_Flanger;
    strcpy(this->name, "Flanger");
    strcpy(this->preset_path, ".\\Presets\\Flanger\\");
    strcpy(this->path,"internal://flanger");
    uniqueID = MAKE_FOURCC('F','L','N','G');

    arec = ar;

    dspCoreFlanger.setTempoSync(false);
    //fmemory = (float*)malloc(65535);
    //dspCoreFlanger.setSharedMemoryAreaToUse(fmemory, 65535); -> not needed anymore - Flanger allocates its own memory now
    Reset();

    frequency = new FrequencyParameter(0.5f);
    frequency->SetReversed(true);
    frequency->SetName("Delay");
    v_freq = new ValueString(VStr_msec2); 
    frequency->vstring = v_freq;
    AddParamWithParamcell(frequency);

    modfreq = new Parameter(3.f, 0.1f, 9.9f, Param_Default);
    modfreq->SetName("Mod. Period");
    v_modfreq = new ValueString(VStr_second);
    modfreq->vstring = v_modfreq;
    AddParamWithParamcell(modfreq);

    feedback = new Parameter(0, -1.f, 2.f, Param_Default_Bipolar);
    feedback->SetName("Feedback");
    v_feedback = new ValueString(VStr_Default);
    feedback->vstring = v_feedback;
    feedback->SetSigned(true);
    AddParamWithParamcell(feedback);

    depth = new Parameter(24.f, 2.f, 46.f, Param_Default);
    depth->SetName("Depth");
    v_depth = new ValueString(VStr_Semitones);
    depth->vstring = v_depth;
    AddParamWithParamcell(depth);

/*
    invert = new BoolParam(false);
    invert->SetName("Invert wet signal");
    v_invert = new ValueString(VStr_String);
    invert->vstring = v_invert;
    AddParamWithParamcell(invert);
    v_invert->SetValue("Invert wet signal");
*/

    drywet = new Parameter(0.5f, 0.0f, 0.5f, Param_Default);
    drywet->SetName("DryWet");
    v_drywet = new ValueString(VStr_DryWet); 
    drywet->vstring = v_drywet;
    AddParamWithParamcell(drywet);

    ParamUpdate(drywet);
    ParamUpdate(frequency);
    ParamUpdate(modfreq);
    ParamUpdate(feedback);
    ParamUpdate(depth);
    //ParamUpdate(invert);
}

CFlanger* CFlanger::Clone(Mixcell* mc)
{
    CFlanger* clone = new CFlanger(mc, arec);
    clone->drywet->SetNormalValue(drywet->val);
    clone->frequency->SetNormalValue(frequency->val);
    clone->modfreq->SetNormalValue(modfreq->val);
    clone->depth->SetNormalValue(depth->val);

    return clone;
}

void CFlanger::ParamUpdate(Parameter* param)
{
    if(param == drywet)
	{
        dspCoreFlanger.setDryWetRatio(drywet->outval);
        v_drywet->SetValue((drywet->outval));
    }
    else if(param == frequency)
	{
        dspCoreFlanger.setFrequency(frequency->outval);
        v_freq->SetValue(1.f/frequency->outval*1000);
    }
	else if(param == modfreq)
	{
		dspCoreFlanger.setCycleLength(modfreq->outval);
		v_modfreq->SetValue(modfreq->outval);
	}
	else if(param == depth)
	{
		dspCoreFlanger.setDepth(depth->outval);
		v_depth->SetValue(depth->outval);
	}
	else if(param == feedback)
	{
		dspCoreFlanger.setFeedbackFactor(feedback->outval);
		v_feedback->SetValue(feedback->outval);
	}
	else if(param == invert)
	{
		dspCoreFlanger.setNegativePolarity(invert->outval);
	}
}

void CFlanger::Reset()
{
    dspCoreFlanger.reset();
}

void CFlanger::ProcessData(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;
    for(int i = 0; i < 2*num_frames; i += 2)
	{
		inOutL = (double) in_buff[i];
		inOutR = (double) in_buff[i+1];
		dspCoreFlanger.getSampleFrameStereo(&inOutL, &inOutR);
		out_buff[i]    = (float) inOutL;
		out_buff[i+1]  = (float) inOutR;
	}
}

//=================================================================================================
//                             CPhaser Class Implementation
//=================================================================================================

CPhaser::CPhaser(Mixcell* mc, AliasRecord* ar) : Eff(mc)
{
    subtype = ModSubtype_Phaser;
    strcpy(this->name, "Phaser");
    strcpy(this->preset_path, ".\\Presets\\Phaser\\");
    strcpy(this->path,"internal://phaser");
    uniqueID = MAKE_FOURCC('P','H','A','S');

    arec = ar;

    dspCorePhaser.setTempoSync(false);
    //fmemory = (float*)malloc(65535);
    //dspCorePhaser.setSharedMemoryAreaToUse(fmemory, 65535);
    Reset();

    frequency = new FrequencyParameter(0.5f);
    frequency->SetReversed(true);
    frequency->SetName("Delay");
    v_freq = new ValueString(VStr_msec2); 
    frequency->vstring = v_freq;
    AddParamWithParamcell(frequency);

    modfreq = new Parameter(3.f, 0.1f, 9.9f, Param_Default);
    modfreq->SetName("Mod. Period");
    v_modfreq = new ValueString(VStr_second);
    modfreq->vstring = v_modfreq;
    AddParamWithParamcell(modfreq);

    feedback = new Parameter(0, -1.f, 2.f, Param_Default_Bipolar);
    feedback->SetName("Feedback");
    v_feedback = new ValueString(VStr_Default);
    feedback->vstring = v_feedback;
    feedback->SetSigned(true);
    AddParamWithParamcell(feedback);

    depth = new Parameter(24.f, 2.f, 46.f, Param_Default);
    depth->SetName("Depth");
    v_depth = new ValueString(VStr_Semitones);
    depth->vstring = v_depth;
    AddParamWithParamcell(depth);

    numstages = new Parameter(4, 1, 23, Param_Default);
    numstages->SetName("Num. stages");
    v_numstages = new ValueString(VStr_Integer);
    numstages->vstring = v_numstages;
    AddParamWithParamcell(numstages);
    numstages->SetInterval(1);

    stereo = new Parameter(0, 0, 180, Param_Default);
    stereo->SetName("Stereo");
    v_stereo = new ValueString(VStr_Default);
    stereo->vstring = v_stereo;
    ////AddParamWithParamcell(stereo);
    stereo->SetInterval(1);

    drywet = new Parameter(0.5f, 0.0f, 0.5f, Param_Default);
    drywet->SetName("DryWet");
    v_drywet = new ValueString(VStr_DryWet); 
    drywet->vstring = v_drywet;
    AddParamWithParamcell(drywet);

    dspCorePhaser.setFilterMode(rosic::AllpassChain::FIRST_ORDER_ALLPASS);

    ParamUpdate(drywet);
    ParamUpdate(frequency);
    ParamUpdate(modfreq);
    ParamUpdate(feedback);
    ParamUpdate(depth);
    ParamUpdate(numstages);
    ParamUpdate(stereo);
}

CPhaser* CPhaser::Clone(Mixcell* mc)
{
    CPhaser* clone = new CPhaser(mc, arec);
    clone->drywet->SetNormalValue(drywet->val);
    clone->frequency->SetNormalValue(frequency->val);
    clone->modfreq->SetNormalValue(modfreq->val);
    clone->depth->SetNormalValue(depth->val);

    return clone;
}

void CPhaser::ParamUpdate(Parameter* param)
{
    if(param == drywet)
    {
        dspCorePhaser.setDryWetRatio(drywet->outval);
        v_drywet->SetValue((drywet->outval));
    }
    else if(param == frequency)
    {
        dspCorePhaser.setFrequency(frequency->outval);
        v_freq->SetValue(1.f/frequency->outval*1000);
    }
    else if(param == modfreq)
    {
        dspCorePhaser.setCycleLength(modfreq->outval);
        v_modfreq->SetValue(modfreq->outval);
    }
    else if(param == depth)
    {
        dspCorePhaser.setDepth(depth->outval);
        v_depth->SetValue(depth->outval);
    }
    else if(param == feedback)
    {
        dspCorePhaser.setFeedbackFactor(feedback->outval);
        v_feedback->SetValue(feedback->outval);
    }
    else if(param == numstages)
    {
        dspCorePhaser.setNumStages(int(numstages->outval));
        v_numstages->SetValue(int(numstages->outval));
    }
    else if(param == stereo)
    {
        dspCorePhaser.setStereoPhaseOffsetInDegrees((stereo->outval));
        v_stereo->SetValue((stereo->outval));
        dspCorePhaser.resetOscillatorPhases();
    }
}

void CPhaser::Reset()
{
    dspCorePhaser.reset();
}

void CPhaser::ProcessData(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;
    for(int i = 0; i < 2*num_frames; i += 2)
	{
        inOutL = (double) in_buff[i];
        inOutR = (double) in_buff[i+1];
        dspCorePhaser.getSampleFrameStereo(&inOutL, &inOutR);
        out_buff[i]    = (float) inOutL;
        out_buff[i+1]  = (float) inOutR;
	}
}

//=================================================================================================
//                             Equalizer1 Class Implementation
//=================================================================================================

EQ1::EQ1(Mixcell* mc, AliasRecord* ar) : Eff(mc)
{
    subtype = ModSubtype_Equalizer1;
    strcpy(this->name, "EQ1");
    strcpy(this->preset_path, ".\\Presets\\EQ1\\");
    strcpy(this->path,"internal://eq1");
    uniqueID = MAKE_FOURCC('E','Q','0','1');

    arec = ar;

    //frequency = new Parameter(1000.0f, 20.0f, 19980.0f, Param_Default);
	frequency = new FrequencyParameter(0.5);
    frequency->SetName("Frequency");
    v_frequency = new ValueString(VStr_Hz);
    frequency->vstring = v_frequency;
    AddParamWithParamcell(frequency);

    gain = new Parameter(0.f, -24.0f, 48.0f, Param_Default_Bipolar);
    gain->SetName("Gain");
    v_gain = new ValueString(VStr_dB); 
    gain->vstring = v_gain;
    AddParamWithParamcell(gain);

    bandwidth = new Parameter(1.f, 0.25f, 3.75f, Param_Default);
    bandwidth->SetName("Bandwidth");
    v_bandwidth = new ValueString(VStr_oct);
    bandwidth->vstring = v_bandwidth;
    AddParamWithParamcell(bandwidth);

    Reset();

    ParamUpdate(frequency);
    ParamUpdate(gain);
    ParamUpdate(bandwidth);
}

EQ1* EQ1::Clone(Mixcell* mc)
{
    EQ1* clone = new EQ1(mc, arec);
    clone->frequency->SetNormalValue(frequency->val);
    clone->gain->SetNormalValue(gain->val);
    clone->bandwidth->SetNormalValue(bandwidth->val);
    clone->ParamUpdate();
	return clone;
}

void EQ1::ParamUpdate(Parameter* param)
{
	if( param == frequency )
	{
		dspCoreEq1.setFrequency(frequency->outval);
		v_frequency->SetValue(         frequency->outval);
	}
	else if( param == gain )
	{
		dspCoreEq1.setGain(gain->outval);
		v_gain->SetValue(gain->outval);
	}
	else if( param == bandwidth )
	{
		dspCoreEq1.setBandwidth(bandwidth->outval);
		v_bandwidth->SetValue(bandwidth->outval);
	}
}

void EQ1::Reset()
{
	dspCoreEq1.reset();
}

void EQ1::ProcessData(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;
    for(int i=0; i<2*num_frames; i+=2)
	{
		inOutL = (double) in_buff[i];
		inOutR = (double) in_buff[i+1];
		dspCoreEq1.getSampleFrameStereo(&inOutL, &inOutR);
		out_buff[i]    = (float) inOutL;
		out_buff[i+1]  = (float) inOutR;
	}
}

//=================================================================================================
//                             GraphicEQ Class Implementation
//=================================================================================================

GraphicEQ::GraphicEQ(Mixcell* mc, AliasRecord* ar) : Eff(mc)
{
    subtype = ModSubtype_GraphicEQ;
    strcpy(this->name, "GraphicEQ");
    strcpy(this->preset_path, ".\\Presets\\GraphicEQ\\");
    strcpy(this->path,"internal://eqg");
    uniqueID = MAKE_FOURCC('G','R','E','Q');

    arec = ar;

    gain1 = new Parameter(0.f, -24.0f, 48.0f, Param_Default_Bipolar);
    gain1->SetName("8000 Hz");
    v_gain1 = new ValueString(VStr_dB); 
    gain1->vstring = v_gain1;
    gain1->SetSigned(true);
    AddParamWithParamcell(gain1);

    gain6 = new Parameter(0.f, -24.0f, 48.0f, Param_Default_Bipolar);
    gain6->SetName("6500 Hz");
    v_gain6 = new ValueString(VStr_dB); 
    gain6->vstring = v_gain6;
    gain6->SetSigned(true);
    AddParamWithParamcell(gain6);

    gain5 = new Parameter(0.f, -24.0f, 48.0f, Param_Default_Bipolar);
    gain5->SetName("3000 Hz");
    v_gain5 = new ValueString(VStr_dB); 
    gain5->vstring = v_gain5;
    gain5->SetSigned(true);
    AddParamWithParamcell(gain5);

    gain2 = new Parameter(0.f, -24.0f, 48.0f, Param_Default_Bipolar);
    gain2->SetName("900 Hz");
    v_gain2 = new ValueString(VStr_dB); 
    gain2->vstring = v_gain2;
    gain2->SetSigned(true);
    AddParamWithParamcell(gain2);

    gain3 = new Parameter(0.f, -24.0f, 48.0f, Param_Default_Bipolar);
    gain3->SetName("500 Hz");
    v_gain3 = new ValueString(VStr_dB); 
    gain3->vstring = v_gain3;
    gain3->SetSigned(true);
    AddParamWithParamcell(gain3);

    gain4 = new Parameter(0.f, -24.0f, 48.0f, Param_Default_Bipolar);
    gain4->SetName("150 Hz");
    v_gain4 = new ValueString(VStr_dB); 
    gain4->vstring = v_gain4;
    gain4->SetSigned(true);
    AddParamWithParamcell(gain4);

    gain9 = new Parameter(0.f, -24.0f, 48.0f, Param_Default_Bipolar);
    gain9->SetName("70 Hz");
    v_gain9 = new ValueString(VStr_dB); 
    gain9->vstring = v_gain9;
    gain9->SetSigned(true);
    AddParamWithParamcell(gain9);

    f1 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::HIGH_SHELF, 8000, 0, 1);
    f6 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::PEAK, 6500, 0, 1);
    f5 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::PEAK, 3000, 0, 1);
    f2 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::PEAK, 900, 0, 1);
    f3 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::PEAK, 500, 0, 1);
    f4 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::PEAK, 150, 0, 1);
    f9 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::LOW_SHELF, 70, 0, 1);

    Reset();

    ParamUpdate(gain1);
    ParamUpdate(gain6);
    ParamUpdate(gain5);
    ParamUpdate(gain2);
    ParamUpdate(gain3);
    ParamUpdate(gain4);
    ParamUpdate(gain9);
}

GraphicEQ* GraphicEQ::Clone(Mixcell* mc)
{
    GraphicEQ* clone = new GraphicEQ(mc, arec);
    clone->gain1->SetNormalValue(gain1->val);
    clone->gain6->SetNormalValue(gain6->val);
    clone->gain5->SetNormalValue(gain5->val);
    clone->gain2->SetNormalValue(gain2->val);
    clone->gain3->SetNormalValue(gain3->val);
    clone->gain4->SetNormalValue(gain4->val);
    clone->gain9->SetNormalValue(gain9->val);
	return clone;
}

void GraphicEQ::ParamUpdate(Parameter* param)
{
	if(param == gain5)
	{
		dspCoreEqualizer.setBandGain(f5, gain5->outval);
		v_gain5->SetValue(gain5->outval);
	}
	else if(param == gain6)
	{
		dspCoreEqualizer.setBandGain(f6, gain6->outval);
		v_gain6->SetValue(gain6->outval);
	}
	else if(param == gain1)
	{
		dspCoreEqualizer.setBandGain(f1, gain1->outval);
		v_gain1->SetValue(gain1->outval);
	}
	else if(param == gain2)
	{
		dspCoreEqualizer.setBandGain(f2, gain2->outval);
		v_gain2->SetValue(gain2->outval);
	}
	else if(param == gain3)
	{
		dspCoreEqualizer.setBandGain(f3, gain3->outval);
		v_gain3->SetValue(gain3->outval);
	}
	else if(param == gain4)
	{
		dspCoreEqualizer.setBandGain(f4, gain4->outval);
		v_gain4->SetValue(gain4->outval);
	}
	else if(param == gain9)
	{
		dspCoreEqualizer.setBandGain(f9, gain9->outval);
		v_gain9->SetValue(gain9->outval);
	}
}

void GraphicEQ::Reset()
{
	dspCoreEqualizer.reset();
}

void GraphicEQ::ProcessData(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;
    for(int i=0; i<2*num_frames; i+=2)
	{
		inOutL = (double) in_buff[i];
		inOutR = (double) in_buff[i+1];
		dspCoreEqualizer.getSampleFrameStereo(&inOutL, &inOutR);
		out_buff[i]    = (float) inOutL;
		out_buff[i+1]  = (float) inOutR;
	}
}

//=================================================================================================
//                             EQ3 Class Implementation
//=================================================================================================

EQ3::EQ3(Mixcell* mc, AliasRecord* ar) : Eff(mc)
{
    subtype = ModSubtype_Equalizer3;
    strcpy(this->name, "EQ3");
    strcpy(this->preset_path, ".\\Presets\\EQ3\\");
    strcpy(this->path,"internal://eq3");
    uniqueID = MAKE_FOURCC('E','Q','0','3');

    arec = ar;

    gain1 = new Parameter(0.f, -24.0f, 48.0f, Param_Default_Bipolar);
    gain1->SetName("High gain");
    v_gain1 = new ValueString(VStr_dB); 
    gain1->vstring = v_gain1;
    AddParamWithParamcell(gain1);

    gain2 = new Parameter(0.f, -24.0f, 48.0f, Param_Default_Bipolar);
    gain2->SetName("Center gain");
    v_gain2 = new ValueString(VStr_dB); 
    gain2->vstring = v_gain2;
    AddParamWithParamcell(gain2);

    gain3 = new Parameter(0.f, -24.0f, 48.0f, Param_Default_Bipolar);
    gain3->SetName("Low gain");
    v_gain3 = new ValueString(VStr_dB); 
    gain3->vstring = v_gain3;
    AddParamWithParamcell(gain3);

/*
	freq1 = new FrequencyParameter(0.5);
    freq1->SetName("High freq");
    v_freq1 = new ValueString(VStr_Hz);
    freq1->vstring = v_freq1;
    AddParamWithParamcell(freq1);
*/

	freq2 = new FrequencyParameter(0.6f);
    freq2->SetName("Center freq");
    v_freq2 = new ValueString(VStr_Hz);
    freq2->vstring = v_freq2;
    AddParamWithParamcell(freq2);

    bandwidth = new Parameter(1.5f, 0.25f, 3.75f, Param_Default);
    bandwidth->SetName("Bandwidth");
    v_bandwidth = new ValueString(VStr_oct);
    bandwidth->vstring = v_bandwidth;
    //AddParamWithParamcell(bandwidth);

/*
    freq3 = new FrequencyParameter(0.5);
    freq3->SetName("Low freq");
    v_freq3 = new ValueString(VStr_Hz);
    freq3->vstring = v_freq3;
    AddParamWithParamcell(freq3);
*/

    f1 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::HIGH_SHELF, 4000, 0);
    f2 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::PEAK, freq2->outval, 0, bandwidth->outval);
    f3 = dspCoreEqualizer.addBand(rosic::TwoPoleFilter::LOW_SHELF, 100, 0);

    Reset();

    //ParamUpdate(freq1);
    ParamUpdate(freq2);
    //ParamUpdate(freq3);
    ParamUpdate(gain1);
    ParamUpdate(gain2);
    ParamUpdate(gain3);
    ParamUpdate(bandwidth);
}

EQ3* EQ3::Clone(Mixcell* mc)
{
    EQ3* clone = new EQ3(mc, arec);
    //clone->freq1->SetNormalValue(gain1->val);
    clone->freq2->SetNormalValue(freq2->val);
    //clone->freq3->SetNormalValue(gain3->val);
    clone->gain1->SetNormalValue(gain1->val);
    clone->gain2->SetNormalValue(gain2->val);
    clone->gain3->SetNormalValue(gain3->val);
    clone->bandwidth->SetNormalValue(bandwidth->val);
	return clone;
}

void EQ3::ParamUpdate(Parameter* param)
{
	if(param == gain1)
	{
		dspCoreEqualizer.setBandGain(f1, gain1->outval);
		v_gain1->SetValue(gain1->outval);
	}
	else if(param == gain2)
	{
		dspCoreEqualizer.setBandGain(f2, gain2->outval);
		v_gain2->SetValue(gain2->outval);
	}
	else if(param == gain3)
	{
		dspCoreEqualizer.setBandGain(f3, gain3->outval);
		v_gain3->SetValue(gain3->outval);
	}
	else if(param == freq1)
	{
		dspCoreEqualizer.setBandFrequency(f1, freq1->outval);
		v_freq1->SetValue(freq1->outval);
	}
	else if(param == freq2)
	{
		dspCoreEqualizer.setBandFrequency(f2, freq2->outval);
		v_freq2->SetValue(freq2->outval);
	}
	else if(param == freq3)
	{
		dspCoreEqualizer.setBandFrequency(f3, freq3->outval);
		v_freq3->SetValue(freq3->outval);
	}
	else if(param == bandwidth)
	{
		dspCoreEqualizer.setBandBandwidth(f2, bandwidth->outval);
		v_bandwidth->SetValue(bandwidth->outval);
	}
}

void EQ3::Reset()
{
	dspCoreEqualizer.reset();
}

void EQ3::ProcessData(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;
    for(int i=0; i<2*num_frames; i+=2)
	{
		inOutL = (double) in_buff[i];
		inOutR = (double) in_buff[i+1];
		dspCoreEqualizer.getSampleFrameStereo(&inOutL, &inOutR);
		out_buff[i]    = (float) inOutL;
		out_buff[i+1]  = (float) inOutR;
	}
}

//=================================================================================================
//                             Tremolo Class Implementation
//=================================================================================================

CTremolo::CTremolo(Mixcell* mc, AliasRecord* ar) : Eff(mc)
{
    subtype = ModSubtype_Tremolo;
    strcpy(this->name, "Tremolo");
    strcpy(this->preset_path, ".\\Presets\\Tremolo\\");
    strcpy(this->path,"internal://tremolo");
    uniqueID = MAKE_FOURCC('T','R','E','M');

    arec = ar;

    speed= new Parameter(1.f, 0.1f, 2.25f, Param_Default);
    speed->SetName("Speed");
    v_speed = new ValueString(VStr_Default); 
    speed->vstring = v_speed;
    AddParamWithParamcell(speed);

    depth = new Parameter(0.5f, 0.f, 1.0f, Param_Default);
    depth->SetName("Depth");
    v_depth= new ValueString(VStr_Default);
    depth->vstring = v_depth;
    AddParamWithParamcell(depth);

    Reset();

    ParamUpdate(speed);
    ParamUpdate(depth);
}

CTremolo* CTremolo::Clone(Mixcell* mc)
{
    CTremolo* clone = new CTremolo(mc, arec);
    clone->speed->SetNormalValue(speed->val);
    clone->depth->SetNormalValue(depth->val);
    clone->ParamUpdate();
	return clone;
}

void CTremolo::ParamUpdate(Parameter* param)
{
    if(param == speed)
    {
        dspCoreCTremolo.setCycleLength(speed->outval);
        v_speed->SetValue(             speed->outval);
    }
    else if(param == depth)
    {
        dspCoreCTremolo.setDepth(depth->outval);
        v_depth->SetValue(       depth->outval);
    }
}

void CTremolo::Reset()
{
	dspCoreCTremolo.resetOscillatorPhases();
}

void CTremolo::ProcessData(float* in_buff, float* out_buff, int num_frames)
{
	dspCoreCTremolo.setTempoInBPM(beats_per_minute);
    double inOutL;
    double inOutR;
    for(int i=0; i<2*num_frames; i+=2)
	{
		inOutL = (double) in_buff[i];
		inOutR = (double) in_buff[i+1];
		dspCoreCTremolo.getSampleFrameStereo(&inOutL, &inOutR);
		out_buff[i]    = (float) inOutL;
		out_buff[i+1]  = (float) inOutR;
	}
}

//=================================================================================================
//                             XDelay Class Implementation
//=================================================================================================

XDelay::XDelay(Mixcell* mc, AliasRecord* ar) : Eff(mc), dspCorePingPongDelay()
{
    subtype = ModSubtype_XDelay;
    strcpy(this->name, "Delay");
    strcpy(this->preset_path, ".\\Presets\\Delay\\");
    strcpy(this->path,"internal://ppd");
    uniqueID = MAKE_FOURCC('P','P','D','L');
    arec = ar;

    dspCorePingPongDelay.setTrueStereoMode(true);

    dspCorePingPongDelay.setPingPongMode(true);
    dspCorePingPongDelay.setStereoSwap(true);
    ppmode = new BoolParam(true);
    ppmode->SetName("Mode");
    ppmode->AddValueString(VStr_String);
    AddParamWithParamcell(ppmode);
    ppmode->vstring->SetValue("ping-pong mode");

    delay = new Parameter(3, 0.5f, 22.5f, Param_Default);
    delay->SetName("Delay");
    delay->AddValueString(VStr_Default);
    AddParamWithParamcell(delay);

    ggain = new Parameter(1.f, 0.f, 1.f, Param_Default);
    ggain->SetName("Amount");
    ggain->AddValueString(VStr_Percent); 
    AddParamWithParamcell(ggain);

    feedback = new Parameter(55.f, 0.0f, 100.f, Param_Default);
    feedback->SetName("Feedback");
    feedback->AddValueString(VStr_Percent);
    AddParamWithParamcell(feedback);

    pan = new Parameter(0.0f, -1.0f, 2.f, Param_Default_Bipolar);
    pan->SetName("Pan");
    pan->AddValueString(VStr_Default);
    pan->SetSigned(true);
    AddParamWithParamcell(pan);

    //highCut = new FrequencyParameter(0.2f);
    //highCut->SetName("HighCut");
    //highCut->AddValueString(VStr_Hz);
    //AddParamWithParamcell(highCut);

    lowCut = new FrequencyParameter(0.9f);
    lowCut->SetName("LowCut");
    v_lowCut = new ValueString(VStr_Hz);
    lowCut->vstring = v_lowCut;
    AddParamWithParamcell(lowCut);

    drywet = new Parameter(40.f, 0.0f, 100.f, Param_Default);
    drywet->SetName("DryWet");
    drywet->AddValueString(VStr_DryWet);
    AddParamWithParamcell(drywet);

    Reset();

    ParamUpdate(ggain);
    ParamUpdate(delay);
    ParamUpdate(drywet);
    ParamUpdate(feedback);
    ParamUpdate(pan);
    //ParamUpdate(highCut);
    ParamUpdate(lowCut);
}

XDelay* XDelay::Clone(Mixcell* mc)
{
    XDelay* clone = new XDelay(mc, arec);

    clone->delay->SetNormalValue(delay->val);
    clone->drywet->SetNormalValue(drywet->val);
    clone->feedback->SetNormalValue(feedback->val);
    clone->pan->SetNormalValue(pan->val);
    //clone->highCut->SetNormalValue(highCut->val);
    clone->lowCut->SetNormalValue(lowCut->val);
    clone->ppmode->SetBoolValue(ppmode->outval);
    clone->ParamUpdate();

	return clone;
}

void XDelay::ParamUpdate(Parameter* param)
{
    if(param == ppmode)
    {
        dspCorePingPongDelay.setPingPongMode(ppmode->outval);
        dspCorePingPongDelay.setStereoSwap(ppmode->outval);
		dspCorePingPongDelay.setPan((ppmode->outval == true ? -1 : 1)*pan->outval);
    }
	else if( param == delay )
	{
		dspCorePingPongDelay.setDelayTime(delay->outval/ticks_per_beat);
		delay->vstring->SetValue(                delay->outval);
	}
	else if( param == ggain )
	{
		//dspCorePingPongDelay.setGlobalGainFactor(ggain->outval); // old, obsolete
		dspCorePingPongDelay.setWetLevel(amp2dB(ggain->outval));
		ggain->vstring->SetValue(int(100*ggain->outval));
	}
	else if( param == drywet )
	{
		dspCorePingPongDelay.setDryWetRatio((float)(0.01*drywet->outval));
		drywet->vstring->SetValue(                 (float)(0.01*drywet->outval));
	}
	else if( param == feedback )
	{
		dspCorePingPongDelay.setFeedbackInPercent(feedback->outval);
		feedback->vstring->SetValue(                     (int)feedback->outval);
	}
	else if( param == pan )
	{
		dspCorePingPongDelay.setPan((ppmode->outval == true ? -1 : 1)*pan->outval);
		pan->vstring->SetValue(            pan->outval);
	}
	else if( param == highCut )
	{
		dspCorePingPongDelay.setLowDamp(highCut->outval);
		highCut->vstring->SetValue(            highCut->outval);
	}
	else if( param == lowCut )
	{
		dspCorePingPongDelay.setHighDamp(lowCut->outval);
		lowCut->vstring->SetValue(            lowCut->outval);
	}
}

void XDelay::Reset()
{
	dspCorePingPongDelay.reset();
}

void XDelay::ProcessData(float* in_buff, float* out_buff, int num_frames)
{
	dspCorePingPongDelay.setTempoInBPM(beats_per_minute);
    double inOutL;
    double inOutR;
    for(int i=0; i<2*num_frames; i+=2)
	{
		inOutL = (double) in_buff[i];
		inOutR = (double) in_buff[i+1];
		dspCorePingPongDelay.getSampleFrameStereo(&inOutL, &inOutR);
		out_buff[i]    = (float) inOutL;
		out_buff[i+1]  = (float) inOutR;
	}
}

//=================================================================================================
//                             Reverb Class Implementation
//=================================================================================================

CReverb::CReverb(Mixcell* mc, AliasRecord* ar) : Eff(mc), dspCoreReverb()
{
    subtype = ModSubtype_Reverb;
    strcpy(this->name, "Reverb");
    strcpy(this->preset_path, ".\\Presets\\Reverb\\");
    strcpy(this->path,"internal://reverb");
    uniqueID = MAKE_FOURCC('R','E','V','R');

    arec = ar;

    preDelay  = new Parameter(0.0f, 0.0f, 200.0f, Param_Default);
    preDelay->SetName("PreDelay");
    preDelay->AddValueString(VStr_msec); 
    AddParamWithParamcell(preDelay);

    roomsize = new Parameter(100.0f, 4.0f, 96.0f, Param_Default);
    roomsize->SetName("Roomsize");
    roomsize->AddValueString(VStr_Integer);
    AddParamWithParamcell(roomsize);

    decay = new Parameter(6.f, 1.0f, 14.f, Param_Default);
    decay->SetName("Decay");
    decay->AddValueString(VStr_second);
    AddParamWithParamcell(decay);

    highCut = new FrequencyParameter(0.2f);
    highCut->SetName("HighCut");
    highCut->AddValueString(VStr_Hz);
    AddParamWithParamcell(highCut);

    lowCut = new FrequencyParameter(1.0f);
    lowCut->SetName("LowCut");
    lowCut->AddValueString(VStr_Hz);
    AddParamWithParamcell(lowCut);

    drywet = new Parameter(50.f, 0.0f, 100.f, Param_Default);
    drywet->SetName("DryWet");
    drywet->AddValueString(VStr_DryWet); 
    AddParamWithParamcell(drywet);

    lowscale = new LogParam(0.5f, 0.1f, 100);
    lowscale->SetName("LowDecScale");
    lowscale->AddValueString(VStr_Default); 
    AddParamWithParamcell(lowscale);

    highscale = new LogParam(0.24f, 0.1f, 100);
    highscale->SetName("HighDecScale");
    highscale->AddValueString(VStr_Default); 
    AddParamWithParamcell(highscale);

    ParamUpdate(drywet);
    ParamUpdate(roomsize);
    ParamUpdate(decay);
    ParamUpdate(lowCut);
    ParamUpdate(highCut);
    ParamUpdate(preDelay);
    ParamUpdate(highscale);
    ParamUpdate(lowscale);

    Reset();
}

CReverb* CReverb::Clone(Mixcell* mc)
{
    CReverb* clone = new CReverb(mc, arec);

    clone->roomsize->SetNormalValue(roomsize->val);
    clone->drywet->SetNormalValue(drywet->val);
    clone->decay->SetNormalValue(decay->val);
    clone->lowCut->SetNormalValue(lowCut->val);
    clone->highCut->SetNormalValue(highCut->val);
    clone->lowscale->SetNormalValue(lowscale->val);
    clone->highscale->SetNormalValue(highscale->val);
    //clone->ParamUpdate();

	return clone;
}

void CReverb::ParamUpdate(Parameter* param)
{
	if(param == roomsize)
	{
		dspCoreReverb.setReferenceDelayTime(roomsize->outval);
		roomsize->vstring->SetValue(RoundFloat(roomsize->outval));
	}
	else if(param == preDelay)
	{
		dspCoreReverb.setPreDelay(preDelay->outval);
		preDelay->vstring->SetValue(preDelay->outval);
	}
	else if(param == drywet)
	{
		dspCoreReverb.setDryWetRatio((float)(0.01*drywet->outval));
		drywet->vstring->SetValue((float)(0.01*drywet->outval));
	}
	else if(param == decay)
	{
		dspCoreReverb.setMidReverbTime(decay->outval);
		decay->vstring->SetValue(decay->outval);
	}
	else if(param == highCut)
	{
		dspCoreReverb.setWetHighpassCutoff(highCut->outval);
		highCut->vstring->SetValue(highCut->outval);
	}
	else if(param == lowCut)
	{
		dspCoreReverb.setWetLowpassCutoff(lowCut->outval);
		lowCut->vstring->SetValue(lowCut->outval);
	}
	else if(param == highscale)
	{
		dspCoreReverb.setHighReverbTimeScale(highscale->outval);
		highscale->vstring->SetValue(highscale->outval);
	}
	else if(param == lowscale)
	{
		dspCoreReverb.setLowReverbTimeScale(lowscale->outval);
		lowscale->vstring->SetValue(lowscale->outval);
	}
}

void CReverb::Reset()
{
	dspCoreReverb.reset();
}

void CReverb::ProcessData(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;
    for(int i=0; i<2*num_frames; i+=2)
	{
		inOutL = (double) in_buff[i];
		inOutR = (double) in_buff[i+1];
		dspCoreReverb.getSampleFrameStereo(&inOutL, &inOutR);
		out_buff[i]    = (float) inOutL;
		out_buff[i+1]  = (float) inOutR;
	}
}

//=================================================================================================
//                             Compressor Class Implementation
//=================================================================================================

Compressor::Compressor(Mixcell* mc, AliasRecord* ar) : Eff(mc)
{
    subtype = ModSubtype_Compressor;
    strcpy(this->name, "Compressor");
    strcpy(this->preset_path, ".\\Presets\\Compressor\\");
    strcpy(this->path,"internal://compressor");
    uniqueID = MAKE_FOURCC('C','O','M','P');

    arec = ar;

    mode = new BoolParam(false);
    mode->SetName("Mode");
    mode_state = new ValueString(VStr_String);
    mode->vstring = mode_state;
    AddParamWithParamcell(mode);
    mode_state->SetValue("Limiter mode");

    threshold = new Parameter(0.f, -60.0f, 60.0f, Param_Default);
    threshold->SetName("Threshold");
    v_threshold = new ValueString(VStr_dB); 
    threshold->vstring = v_threshold;
    AddParamWithParamcell(threshold);

    ratio = new Parameter(1.0f, 1.0f, 30.0f, Param_Default);
    ratio->SetName("Ratio");
    v_ratio = new ValueString(VStr_Default); 
    ratio->vstring = v_ratio;
    AddParamWithParamcell(ratio);

    knee = new Parameter(0.f, 0.0f, 48.0f, Param_Default);
    knee->SetName("Knee");
    knee->AddValueString(VStr_dB);
    AddParamWithParamcell(knee);

    attack = new Parameter(10.f, 1.f, 90.0f, Param_Default);
    attack->SetName("Attack");
    v_attack = new ValueString(VStr_msec); 
    attack->vstring = v_attack;
    AddParamWithParamcell(attack);

    release = new Parameter(100.f, 10.f, 900.0f, Param_Default);
    release->SetName("Release");
    v_release = new ValueString(VStr_msec); 
    release->vstring = v_release;
    AddParamWithParamcell(release);

    gain = new Parameter(0.f, 0.f, 30.0f, Param_Default);
    gain->SetName("Gain");
    v_gain = new ValueString(VStr_dB); 
    gain->vstring = v_gain;
    AddParamWithParamcell(gain);

/* // Unpredictable shit
    autogain = new BoolParam(false);
    autogain->SetName("Autogain");
    autogain_state = new ValueString(VStr_String);
    autogain->vstring = autogain_state;
    AddParamWithParamcell(autogain);
*/

    ParamUpdate(threshold);
    ParamUpdate(ratio);
    ParamUpdate(knee);
    ParamUpdate(gain);
    ParamUpdate(attack);
    ParamUpdate(release);
    ParamUpdate(mode);
    //ParamUpdate(autogain);
}

Compressor* Compressor::Clone(Mixcell* mc)
{
    Compressor* clone = new Compressor(mc, arec);
    clone->threshold->SetNormalValue(threshold->val);
    clone->ratio->SetNormalValue(ratio->val);
    clone->knee->SetNormalValue(knee->val);
    clone->gain->SetNormalValue(gain->val);
    clone->attack->SetNormalValue(attack->val);
    clone->release->SetNormalValue(release->val);
    clone->mode->SetNormalValue(mode->val);

    return clone;
}

void Compressor::ParamUpdate(Parameter* param)
{
    if(param == mode)
    {
        dspCoreComp.setLimiterMode(mode->outval);
    }
    else if(param == autogain)
    {
        if(autogain->outval == true)
        {
            autogain_state->SetValue("Autogain ON");
            dspCoreComp.setAutoGain(true);
        }
        else
        {
            autogain_state->SetValue("Autogain OFF");
            dspCoreComp.setAutoGain(false);
        }
    }
    else if(param == threshold)
    {
		dspCoreComp.setThreshold((double)threshold->outval);
		v_threshold->SetValue(threshold->outval);
    }
    else if(param == knee)
    {
		dspCoreComp.setThreshold((double)knee->outval);
		knee->vstring->SetValue(knee->outval);
    }
	else if(param == ratio)
	{
		dspCoreComp.setRatio((double)ratio->outval);
		v_ratio->SetValue(ratio->outval);
	}
	else if(param == gain)
	{
		dspCoreComp.setOutputGain((double)gain->outval);
		v_gain->SetValue(gain->outval);
	}
	else if(param == attack)
	{
		dspCoreComp.setAttackTime(attack->outval);
		v_attack->SetValue(attack->outval);
	}
	else if(param == release)
	{
		dspCoreComp.setReleaseTime(release->outval);
		v_release->SetValue(release->outval);
	}
}

void Compressor::Reset()
{
	dspCoreComp.reset();
}

void Compressor::ProcessData(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;
    for(int i=0; i<2*num_frames; i+=2)
	{
		inOutL = (double) in_buff[i];
		inOutR = (double) in_buff[i+1];
		dspCoreComp.getSampleFrameStereo(&inOutL, &inOutR);
		out_buff[i]    = (float) inOutL;
		out_buff[i+1]  = (float) inOutR;
	}
}

CWahWah::CWahWah(Mixcell* mc, AliasRecord* ar) : Eff(mc)
{
    subtype = ModSubtype_WahWah;
    strcpy(this->name, "WahWah");
    strcpy(this->preset_path, ".\\Presets\\WahWah\\");
    strcpy(this->path,"internal://wahwah");
    uniqueID = MAKE_FOURCC('W','A','H','W');

    arec = ar;

    frequency = new FrequencyParameter(0.5f);
    frequency->SetName("Freq.");
    frequency->AddValueString(VStr_Hz); 
    //AddParamWithParamcell(frequency);

    modfreq = new Parameter(1.25f, 0.1f, 4.9f, Param_Default);
    modfreq->SetName("ModFreq");
    modfreq->AddValueString(VStr_fHz1);
    AddParamWithParamcell(modfreq);

    depth = new Parameter(48.f, 2.f, 46.f, Param_Default);
    depth->SetName("Depth");
    depth->AddValueString(VStr_Semitones);
    AddParamWithParamcell(depth);

    drywet = new Parameter(0.75f, 0.0f, 1.f, Param_Default);
    drywet->SetName("DryWet");
    drywet->AddValueString(VStr_DryWet); 
    AddParamWithParamcell(drywet);

    Reset();

    ParamUpdate(drywet);
    ParamUpdate(frequency);
    ParamUpdate(modfreq);
    ParamUpdate(depth);
}

CWahWah* CWahWah::Clone(Mixcell* mc)
{
    CWahWah* clone = new CWahWah(mc, arec);
    clone->drywet->SetNormalValue(drywet->val);
    clone->frequency->SetNormalValue(frequency->val);
    clone->modfreq->SetNormalValue(modfreq->val);
    clone->depth->SetNormalValue(depth->val);
    return clone;
}

void CWahWah::ParamUpdate(Parameter* param)
{
    if(param == drywet)
    {
        dspCoreWah.setDryWetRatio(drywet->outval);
        drywet->vstring->SetValue(drywet->outval);
    }
    else if(param == frequency)
    {
        dspCoreWah.setFrequency(frequency->outval);
        frequency->vstring->SetValue(frequency->outval);
    }
    else if(param == modfreq)
    {
        dspCoreWah.setCycleLength(1.f/modfreq->outval);
        modfreq->vstring->SetValue(modfreq->outval);
    }
    else if(param == depth)
    {
        dspCoreWah.setDepth(depth->outval);
        depth->vstring->SetValue(depth->outval);
    }
}

void CWahWah::Reset()
{
	dspCoreWah.reset();
}

void CWahWah::ProcessData(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;
    for(int i = 0; i < 2*num_frames; i += 2)
    {
        inOutL = (double) in_buff[i];
        inOutR = (double) in_buff[i + 1];
        dspCoreWah.getSampleFrameStereo(&inOutL, &inOutR);
        out_buff[i]    = (float) inOutL;
        out_buff[i + 1]  = (float) inOutR;
    }
}

CDistort::CDistort(Mixcell* mc, AliasRecord* ar) : Eff(mc)
{
    subtype = ModSubtype_Distortion;
    strcpy(this->name, "Distortion");
    strcpy(this->preset_path, ".\\Presets\\Distortion\\");
    strcpy(this->path,"internal://distortion");
    uniqueID = MAKE_FOURCC('D','I','S','T');

    arec = ar;

    drive = new Parameter(32.0f, 0.0f, 48.f, Param_Default);
    drive->SetName("Drive");
    drive->AddValueString(VStr_dB); 
    AddParamWithParamcell(drive);

    postgain = new Parameter(0.0f, -48.0f, 48.f, Param_Default);
    postgain->SetName("PostGain");
    postgain->AddValueString(VStr_dB); 
    AddParamWithParamcell(postgain);

    slope = new Parameter(1.0f, -1.0f, 4.f, Param_Default);
    slope->SetName("Slope");
    slope->AddValueString(VStr_Default); 
    AddParamWithParamcell(slope);

    dspCoreDist.setOversampling(1);
    dspCoreDist.setAmount(10);
    dspCoreDist.setTransferFunction(3);

    Reset();

    ParamUpdate(drive);
    ParamUpdate(postgain);
    ParamUpdate(slope);
}

CDistort* CDistort::Clone(Mixcell* mc)
{
    CDistort* clone = new CDistort(mc, arec);
    clone->drive->SetNormalValue(drive->val);
    clone->postgain->SetNormalValue(postgain->val);
    return clone;
}

void CDistort::ParamUpdate(Parameter* param)
{
    if(param == drive)
    {
        dspCoreDist.setDrive(drive->outval);
        drive->vstring->SetValue(drive->outval);
    }
    else if(param == postgain)
    {
        dspCoreDist.setOutputLevel(postgain->outval);
        postgain->vstring->SetValue(postgain->outval);
    }
    else if(param == slope)
    {
        dspCoreDist.setPenticSlopeAtZero(slope->outval);
        slope->vstring->SetValue(slope->outval);
    }
}

void CDistort::Reset()
{
	dspCoreDist.reset();
}

void CDistort::ProcessData(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;
    for(int i = 0; i < 2*num_frames; i += 2)
	{
		inOutL = (double) in_buff[i];
		inOutR = (double) in_buff[i + 1];
		dspCoreDist.getSampleFrameStereo(&inOutL, &inOutR);
		out_buff[i]    = (float) inOutL;
		out_buff[i + 1]  = (float) inOutR;
	}
}

CBitCrusher::CBitCrusher(Mixcell* mc, AliasRecord* ar) : Eff(mc)
{
    subtype = ModSubtype_BitCrusher;
    strcpy(this->name, "BitCrusher");
    strcpy(this->preset_path, ".\\Presets\\BitCrusher\\");
    strcpy(this->path,"internal://bitcrusher");
    uniqueID = MAKE_FOURCC('B','I','T','C');

    arec = ar;

    decimation = new Parameter(1.0f, 1.0f, 127.f, Param_Default);
    decimation->SetName("Decimation");
    decimation->AddValueString(VStr_Integer); 
    AddParamWithParamcell(decimation);

    quantization = new Parameter(0.0f, 0.0f, 1.f, Param_Default);
    quantization->SetName("Quantization");
    quantization->AddValueString(VStr_Default); 
    AddParamWithParamcell(quantization);

    dspCoreBC.setAmount(1);

    ParamUpdate(decimation);
    ParamUpdate(quantization);
}

CBitCrusher* CBitCrusher::Clone(Mixcell* mc)
{
    CBitCrusher* clone = new CBitCrusher(mc, arec);
    clone->decimation->SetNormalValue(decimation->val);
    clone->quantization->SetNormalValue(quantization->val);
    return clone;
}

void CBitCrusher::ParamUpdate(Parameter* param)
{
    if(param == decimation)
    {
        dspCoreBC.setDecimationFactor(int(decimation->outval));
        decimation->vstring->SetValue(int(decimation->outval));
    }
    else if(param == quantization)
    {
        dspCoreBC.setQuantizationInterval(0.001f*pow(1000.0f, quantization->outval));
        quantization->vstring->SetValue(quantization->outval);
    }
}

void CBitCrusher::Reset()
{
	dspCoreBC.reset();
}

void CBitCrusher::ProcessData(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;
    for(int i = 0; i < 2*num_frames; i += 2)
	{
        inOutL = (double) in_buff[i];
        inOutR = (double) in_buff[i + 1];
        dspCoreBC.getSampleFrameStereo(&inOutL, &inOutR);
        out_buff[i]    = (float) inOutL;
        out_buff[i + 1]  = (float) inOutR;
    }
}

CStereo::CStereo(Mixcell* mc, AliasRecord* ar) : Eff(mc)
{
    subtype = ModSubtype_Stereo;
    strcpy(this->name, "Stereoizer");
    strcpy(this->preset_path, ".\\Presets\\Stereoizer\\");
    strcpy(this->path,"internal://stereoizer");
    uniqueID = MAKE_FOURCC('S','T','E','R');

    arec = ar;

    offset = new Parameter(10.0f, 1.0f, 99.f, Param_Default);
    offset->SetName("Offset");
    offset->AddValueString(VStr_Integer); 
    AddParamWithParamcell(offset);

    ParamUpdate(offset);
}

CStereo* CStereo::Clone(Mixcell* mc)
{
    CStereo* clone = new CStereo(mc, arec);
    clone->offset->SetNormalValue(offset->val);
    return clone;
}

void CStereo::ParamUpdate(Parameter* param)
{
    if(param == offset)
    {
        dspCoreStereo.delayLine.setDelayInMilliseconds(offset->outval);
        offset->vstring->SetValue(int(offset->outval));
    }
}

void CStereo::Reset()
{
	//dspCoreStereo.reset();
}

void CStereo::ProcessData(float* in_buff, float* out_buff, int num_frames)
{
    double inOutL;
    double inOutR;
    for(int i = 0; i < 2*num_frames; i += 2)
	{
		inOutL = (double) in_buff[i];
		inOutR = (double) in_buff[i + 1];
		dspCoreStereo.getSampleFrameStereo(&inOutL, &inOutR);
		out_buff[i]    = (float) inOutL;
		out_buff[i + 1]  = (float) inOutR;
	}
}
//=================================================================================================
//                             VSTEffect Class Implementation
//=================================================================================================

VSTEffect::~VSTEffect()
{
    if(VSTParamWnd != NULL)
    {
        MainWnd->DeleteChild(VSTParamWnd);
    }

    if (this->arec)
    {
        delete this->arec;
    }

    if (this->pPlug != NULL)
    {
        pVSTCollector->RemovePlugin(this->pPlug);
    }

    this->pPlug = NULL;
    this->pEditButton = NULL;
}

VSTEffect* VSTEffect::Clone(Mixcell* mc)
{
    VSTEffect* clone = NULL;

    clone = new VSTEffect(mc, arec, this->path);

    if(clone != NULL)
    {
        MemoryBlock m;
        GetStateInformation(m);
        clone->SetStateInformation(m.getData(), m.getSize());

/*
        // Set the current preset equal to its parent one
        if (this->CurrentPreset != NULL)
        {
            clone->SetPresetByName(this->CurrentPreset->name, this->CurrentPreset->native);
        }

        // Now go through the list of parameters and tune them up to the parent effect
        Parameter* pParam = this->first_param;
        Parameter* pCloneParam = clone->first_param;
        while((pParam != NULL) && (pCloneParam != NULL))
        {
            clone->ParamUpdate(pParam);
            pCloneParam->SetNormalValue(pParam->val);
            pParam = pParam->next;
            pCloneParam = pCloneParam->next;
        }
*/
    }

    return clone;
}

VSTEffect::VSTEffect(Mixcell * mc, AliasRecord * ar, char* path) : Eff(mc)
{
    internalModule = false;
    this->bLoading = true;
    this->subtype = ModSubtype_VSTPlugin;
    memset(this->preset_path, 0, MAX_PATH_STRING * sizeof(char));
    strcpy(this->preset_path, ".\\Data\\Presets\\");
    this->arec = new AliasRecord();
    this->arec->type = ar->type;
    strcpy(this->arec->alias, ar->alias);

    this->owner = (Object*)mc;
    this->NativePresets = NULL;
    this->NP_last = NULL;
    this->NumNativePresets = 0;
    this->NumPresets = 0;
    this->pEditButton = NULL;
    this->VSTParamWnd = NULL;
    /* While loading some VST plugins current working directory somehow gets changed */
    /* Thus we need to remember it and restore in the end of initialization hehehe   */
    char working_directory[MAX_PATH_STRING] = {0};
    int  drive                              = _getdrive();

    _getdcwd(drive, working_directory, MAX_PATH_STRING - 1 );


    /* Pass NULL if you want to see OpenFile dialog */
    pPlug = pVSTCollector->LoadPlugin(this, path);

    if (this->pPlug != NULL)
    {
        pPlug->scope = &scope;
        uniqueID = pPlug->pEffect->pEffect->uniqueID;

        this->pPlug->GetErrorText();

        if (path)
        {
            strcpy(this->path, path);
        }
        else
        {
            strcpy(this->path, this->pPlug->pEffect->sName);
        }

        /* 0 - means unlimited length */
        pPlug->GetDisplayName(this->name,0);

        this->pEditButton = new Butt(false);
        this->pEditButton->scope = &scope;
        this->pEditButton->SetTitle("Edit");
        this->pEditButton->SetHint("Show editor window");
        this->pEditButton->enabled = pPlug->HasEditor();
        this->pEditButton->SetOnClickHandler(&VSTEffEditButtonOnClick);
        this->pEditButton->owner = (Object*)(this);
 
        AddNewControl((Control*)(this->pEditButton), NULL);

        AddParameters();

        if (this->pPlug->isGenerator == true)
        {
            this->modtype = ModuleType_Instrument;
            strcat(this->preset_path, "Instruments\\VST\\");
        }
        else
        {
            this->modtype = ModuleType_Effect;
            strcat(this->preset_path, "Effects\\VST\\");
        }

        {
            UpdatePresets();

            /* Let's add user saved presets
            {
                WIN32_FIND_DATA   founddata                  = {0};
                HANDLE            shandle                    = INVALID_HANDLE_VALUE;
                char              temp_path[MAX_PATH_STRING] = {0};
                FILE             *fhandle                    = NULL;
                char              filename[MAX_PATH_STRING]  = {0};
                EffPresetHeader_t Header                     = {0};
                Preset           *pPreset                    = NULL;


                sprintf(temp_path,"%s%s%s",this->preset_path, this->name, "_*.cxml\0");

                shandle = FindFirstFile(temp_path, &founddata);

                if (shandle != INVALID_HANDLE_VALUE)
                {
                    do
                    {
                        sprintf(filename, "%s%s",this->preset_path, founddata.cFileName);

                        String  sFilePath(filename);
                        File myFile(sFilePath);
                        XmlDocument myPreset(myFile);

                        XmlElement* xmlMainNode = myPreset.getDocumentElement();

                        if (xmlMainNode != NULL)
                        {
                            // Sanity check
                            if (xmlMainNode->hasTagName(T("ChaoticPreset")) && xmlMainNode->hasAttribute(T("FormatVersion")))
                            {
                                pPreset = new Preset((Object*)this);
                                pPreset->index = this->NumUserPresets;
                                XmlElement* xmlHeader = xmlMainNode->getChildByName(T("Module"));

                                if (xmlHeader != NULL)
                                {
                                    String Str1 = xmlHeader->getStringAttribute(T("Preset"));
                                    Str1.copyToBuffer(pPreset->name, min(Str1.length(), 255));
                                    pPreset->native = false;
                                    strcpy(pPreset->path, filename);
                                    this->AddPreset(pPreset);
                                }
                            }
                        }
                    }while (FindNextFile(shandle, &founddata));
                    FindClose(shandle);
                }
            } */
        }
    }

    //  this->pPlug->pEffect->EffSuspend();
    /* Restore previously saved working directory */
    _chdir(working_directory);
    this->bLoading = false;
}

void VSTEffect::UpdatePresets()
{
    long    CurrentProgram          = this->pPlug->GetProgram();
    Preset* pPreset                 = NULL;
    long    num_presets             = this->pPlug->GetNumPresets();
    char    bzName[MAX_PRESET_NAME] = {0};

    DeletePresets();

    for (int idx = 0; idx < num_presets; ++idx)
    {
        memset(bzName, 0, MAX_PRESET_NAME * sizeof(char));

        //this->pPlug->SetProgram(index);
        this->pPlug->pEffect->EffGetProgramNameIndexed(0, idx, bzName);

        if(bzName[0] != '\0')
        {
            pPreset = new Preset((Object*)this);
            pPreset->index = idx;
            pPreset->native = true;
            strcpy(pPreset->name, bzName);
            if(idx == CurrentProgram)
            {
               CurrentPreset = pPreset;
            }
            this->AddPreset(pPreset);
        }
    }
}

void VSTEffect::AddParameters()
{
    /* Get all parameters and add them to list of parameters for the effect */
    int                     index       = 0;
    int                     NumParam    = pPlug->GetNumParams();
    char                   *pParamName  = NULL;
    char                   *pValDisp    = NULL;
    char                   *pParamLabel = NULL;
    Parameter              *pParam      = NULL;
    float                   fVal        = 0;
    VstParameterProperties *pParamProp  = NULL;

    for (index = 0; index < NumParam; ++index)
    {
        if ( pPlug->pEffect->EffCanBeAutomated(index) == 0 )
        {
            //Don't add parameters which are useless
            continue;
        }

        pPlug->GetParamName(index,&pParamName);
        pPlug->GetDisplayValue(index,&pValDisp);
        pPlug->GetParamLabel(index, &pParamLabel);

		if(strlen(pParamName) <= MAX_PARAM_NAME)
		{
			fVal = pPlug->GetParam(index);
			if(fVal > 1)
				fVal = 1;
			else if(fVal < 0)
				fVal = 0;

            if (1 == pPlug->pEffect->EffGetParameterProperties(index, pParamProp))
            {
                if ( (pParamProp->flags & kVstParameterIsSwitch) != 0 )
                {
                    pParam = new Parameter(fVal, 0, 1., Param_Default);
					pParam->SetInterval(1.f);
				}
                else if ( (pParamProp->flags & kVstParameterUsesFloatStep) != 0 )
                {
                    pParam = new Parameter(fVal, 0, 1., Param_Default);
					pParam->SetInterval(pParamProp->smallStepFloat);
                }
                else if ( ((pParamProp->flags & kVstParameterUsesIntStep) != 0) &&
                          ((pParamProp->flags & kVstParameterUsesIntegerMinMax) != 0) )
                {
                    float step = 1.0f/(pParamProp->maxInteger - pParamProp->minInteger);

                    pParam = new Parameter(fVal, 0, 1., Param_Default);
					pParam->SetInterval(step);
                }
                else
                {
                    pParam = new Parameter(fVal, 0, 1., Param_Default);
                }
            }
            else
            {
                pParam = new Parameter(fVal, 0, 1., Param_Default);
            }
            //copy only MAX_PARAM_NAME number of chars to prevent corruption
			strncpy(pParam->name, pParamName, MAX_PARAM_NAME);
			pParam->vstring = new ValueString(VStr_Default);

			strncpy(pParam->vstring->label, pValDisp, min(100-strlen(pParamLabel),strlen(pValDisp)));
			strcat(pParam->vstring->label, pParamLabel);

			pParam->vstring->SetValue(fVal);

			pParam->index = index; // for VST purpose
			this->AddParam(pParam);

			if (NULL != pParamName)
			{
				free(pParamName);
				pParamName = NULL;
			}

			if (NULL != pValDisp)
			{
				free(pValDisp);
				pValDisp = NULL;
			}

			if (NULL != pParamLabel)
			{
				free(pParamLabel);
				pParamLabel = NULL;
			}
		}
    }
}

void VSTEffect::ProcessData(float * in_buff,float * out_buff,int num_frames)
{
   // pVSTCollector->AcquireSema();
    if (this->pPlug != NULL)
    {
       // this->pPlug->pEffect->EffResume();
        this->pPlug->ProcessData(in_buff, out_buff, num_frames);
      //  this->pPlug->pEffect->EffSuspend();
    }

   // pVSTCollector->ReleaseSema();
}

void VSTEffect::ProcessEvents(VstEvents *pEvents)
{
    this->pPlug->ProcessEvents(pEvents);
}

void VSTEffect::UpdateVString(Parameter* param)
{
    char      *pValDisp    = NULL;
    char      *pParamLabel = NULL;

    this->pPlug->GetDisplayValue(param->index,&pValDisp);
    this->pPlug->GetParamLabel(param->index, &pParamLabel);
    //strcpy(pParamLabel, (const char*)String(pParamLabel).trim());
    memset(param->vstring->label, 0, sizeof(param->vstring->label));
    strncpy(param->vstring->label, pValDisp, min(100-strlen(pParamLabel),strlen(pValDisp)));
    strcat(param->vstring->label, pParamLabel);

    param->vstring->SetValue(param->val);

    if (NULL != pValDisp)
    {
        free(pValDisp);
        pValDisp = NULL;
    }

    if (NULL != pParamLabel)
    {
        free(pParamLabel);
        pParamLabel = NULL;
    }
}

void VSTEffect::ParamUpdate(Parameter* param)
{
    if ((param != NULL) && (this->GetParamLock() == false))
    {
        this->pPlug->SetParam(param->index, param->val);
        UpdateVString(param);
    }
}

bool VSTEffect::OnSetParameterAutomated(int nEffect,long index,float value)
{
    Parameter* param = this->first_param;
    RECT       rc = {0, 0, 1, 1};

    while (param != NULL)
    {
        if (param->index == index)
        {
            this->SetParamLock(true);
            param->SetValueFromControl(value);
            UpdateVString(param);
            this->SetParamLock(false);
            break;
        }
        param = param->next;
    }
    return false;
}

bool VSTEffect::OnUpdateDisplay()
{
    if (this->bLoading == true || ProjectLoadingInProgress == true)
    {
        //This effect or current project is loading
        return false;
    }

    // Plugin must limit program name to kVstMaxProgNameLen = 24
    char szProgName[100] = {0};
    unsigned long ulProgram = 0;
    Preset* pNewPreset = NULL;

    UpdatePresets();
    if ((this->modtype == ModuleType_Effect) && (aux_panel->current_eff == this))
    {
        if(mixbrowse && mixBrw->brwmode == Browse_Presets)
        {
            float oldoffs = mixBrw->main_offs;
            mixBrw->Update();
            mixBrw->main_offs = oldoffs;
        }
    }
    else if ( (current_instr->instrtype == Instr_VSTPlugin) &&
              (((VSTGenerator*)current_instr)->pEff == this) )
    {
        if(genBrw->brwmode == Browse_Presets)
        {
            float oldoffs = genBrw->main_offs;
            genBrw->Update();
            genBrw->UpdateCurrentHighlight();
            genBrw->main_offs = oldoffs;
        }
    }

    // Get current program from the plugin
    //this->pPlug->GetProgramName(szProgName);
    ulProgram = this->pPlug->GetProgram();
    // Update pointer to current active preset
    pNewPreset = this->GetPreset(ulProgram);
    if (this->CurrentPreset != pNewPreset)
    {
        this->CurrentPreset = pNewPreset;

        // Find out what browser to re-fresh and whether we really need to refresh anything
        if ((this->modtype == ModuleType_Effect) && (aux_panel->current_eff == this))
        {
            mixBrw->UpdateCurrentHighlight();
        }
        else if ( (current_instr->instrtype == Instr_VSTPlugin) &&
                  (((VSTGenerator*)current_instr)->pEff == this) )
        {
            genBrw->UpdateCurrentHighlight();
        }
    }

    MC->listen->CommonInputActions();

    return true;
}

long VSTEffect::GetNumPresets()
{
    return this->NumPresets;
}

void VSTEffect::GetPresetName(long index, char *ppName)
{
    if (((index >=0) && (index <= this->NumPresets)) && (ppName != NULL))
    {
        Preset* pPreset;
        /* If index in DLL's preset range then get it straight from plugin */
        if (index <= this->NumNativePresets)
        {
            pPreset = this->NativePresets;
        }

        while (pPreset != NULL)
        {
            if (pPreset->index == index)
            {
                strcpy(ppName, pPreset->name);
                break;
            }
            pPreset = pPreset->next;
        }
    }
}

long VSTEffect::GetPresetIndex(char* name)
{
    long ret_val = -1;

    if (name != NULL)
    {
        Preset* pPreset = this->NativePresets;

        while (pPreset != NULL)
        {
            if(_stricmp(pPreset->name, name) == 0)
            {
                ret_val = pPreset->index;
                break;
            }
            pPreset = pPreset->next;
        }
    }
    return ret_val;
}

bool VSTEffect::SetUserPreset(Preset* pPreset)
{
    return this->SetPreset(pPreset);
}

bool VSTEffect::SetPresetByName(char* name)
{
    bool ret_val = false;

    if (name != NULL)
    {
        Preset* pPreset = this->NativePresets;

        Parameter* pParam = NULL;

        while (pPreset != NULL)
        {
            if(_stricmp(pPreset->name, name) == 0)
            {
                this->pPlug->SetProgram(pPreset->index);
                ret_val = true;
                this->CurrentPreset = pPreset;

				float fVal;
                this->SetParamLock(true);
                pParam = this->first_param;
                while (pParam != NULL)
                {
					fVal = this->pPlug->GetParam(pParam->index);
					if(fVal < 0)
						fVal = 0;
					else if(fVal > 1)
						fVal = 1;
                    pParam->SetNormalValue(fVal);
                    pParam->SetInitialValue(fVal);

                    pParam = pParam->next;
                    //this->ParamUpdate(pParam);
                }

                this->SetParamLock(false);
                break;
            }
            pPreset = pPreset->next;
        }
    }
    return ret_val;
}

bool VSTEffect::SetPresetByIndex(long index)
{
    bool ret_val = false;

    if (index >=0)
    {
        if(index <= this->NumPresets)
        {
            this->pPlug->SetProgram(index);
            ret_val = true;
        }
    }
    return ret_val;
}

long VSTEffect::GetCurrentPreset()
{
    long ret_val = -1;

    ret_val = this->pPlug->pEffect->EffGetProgram();

    return ret_val;
}

void VSTEffect::SetPresetName(char* new_name)
{
    this->pPlug->pEffect->EffSetProgramName(new_name);
}

Preset* VSTEffect::GetPreset(long index)
{
    Preset* pPreset = NULL;

    if (index >= 0)
    {
        if (index <= this->NumNativePresets)
        {
            pPreset = this->NativePresets;
            while ((pPreset != NULL) && (pPreset->index != index))
            {
                pPreset = pPreset->next;
            }
        }
    }

    return pPreset;
}

Preset* VSTEffect::GetPreset(char* name)
{
    Preset* pPreset = NULL;
    bool    found   = false;

    if (name != NULL)
    {
        pPreset = this->NativePresets;
        while (pPreset != NULL)
        {
            if (strcmp(pPreset->name,name) == 0)
            {
                found = true;
                break;
            }
            pPreset = pPreset->next;
        }
    }

    if (found == false)
    {
        pPreset = NULL;
    }

    return pPreset;
}


void VSTEffect::RenamePreset(long index, char* new_name)
{
    Preset *pPreset = NULL;

    if (this->pPlug != NULL)
    {
        long idx = this->GetCurrentPreset();

        if (true == this->SetPresetByIndex(index))
        {
            this->SetPresetName(new_name);
        }
        this->SetPresetByIndex(idx);
    }

    pPreset = this->GetPreset(index);

    strcpy(pPreset->name, new_name);

}

void VSTEffect::RenamePreset(char* old_name, char* new_name)
{
    //TO DO: may be later, may be never
}

void VSTEffect::Reset()
{
    /* // Fucking Absynth fucking crashes fucking everything when the below code fucking works because of
    // conflict with StopAllNotes function in VSTGenerator on stopping. Probably unneeded.
    VstEvents myEvents;

    myEvents.numEvents = 1;
    myEvents.reserved = 0;
    myEvents.events[0] = (VstEvent*)malloc(sizeof(VstEvent));
    myEvents.events[1] = NULL;
    VstMidiEvent * pMidi = (VstMidiEvent *)(myEvents.events[0]);
    if (myEvents.events[0] != NULL)
    {
        pMidi->type = kVstMidiType;
        pMidi->byteSize = sizeof(VstMidiEvent);
        pMidi->deltaFrames = 0;
        pMidi->flags = 0;
        pMidi->noteLength = 0;
        pMidi->noteOffset = 0;
        pMidi->noteOffVelocity = 127;
        pMidi->midiData[0] = char(0xB0); // to avoid some fucking warnings
        pMidi->midiData[1] = 0x7B;
        pMidi->midiData[2] = 0x00;

        if( (this->pPlug != NULL) && (this->pPlug->pEffect != NULL) )
        {
            this->pPlug->pEffect->EffProcessEvents(&myEvents);
        }

        free(myEvents.events[0]);
    }*/
    this->pPlug->TurnOffProcessing();
    this->pPlug->TurnOnProcessing();
}

void VSTEffect::SetBPM(float bpm)
{
    this->pPlug->SetBPM(bpm);
}

void VSTEffect::SetBufferSize(unsigned int uiBufferSize)
{
    this->pPlug->SetBufferSize(uiBufferSize);
}

void VSTEffect::SetSampleRate(float fSampleRate)
{
    this->pPlug->SetSampleRate(fSampleRate);
}

void VSTEffect::ShowEditor(bool show)
{
    if (show == true)
    {
        this->pPlug->ShowEditor();
        if(scope.instr == NULL)
        {
            wnd = this->pPlug->pWnd;
        }
    }
    else
    {
        this->pPlug->CloseEditor();
        if(scope.instr == NULL)
        {
            wnd = NULL;
        }
    }
}

void VSTEffect::getChunkData (MemoryBlock& mb, bool isPreset, int maxSizeMB) const
{
    if (pPlug->UsesChunks())
    {
        void* data = 0;
        const int bytes = pPlug->pEffect->EffDispatch(effGetChunk, isPreset ? 1 : 0, 0, &data, 0.0f);

        if (data != 0 && bytes <= maxSizeMB * 1024 * 1024)
        {
            mb.setSize (bytes);
            mb.copyFrom (data, 0, bytes);
        }
    }
}

void VSTEffect::updateStoredProgramNames()
{
    if (GetNumPresets()> 0)
    {
        char nm [256];
        zerostruct (nm);

        // only do this if the plugin can't use indexed names..
        if (pPlug->pEffect->EffDispatch(effGetProgramNameIndexed, 0, -1, nm, 0) == 0)
        {
            const int oldProgram = GetCurrentPreset();
            MemoryBlock oldSettings;
            createTempParameterStore (oldSettings);

            for (int i = 0; i < GetNumPresets(); ++i)
            {
                SetPresetByIndex(i);
            }

            SetPresetByIndex(oldProgram);
            restoreFromTempParameterStore (oldSettings);
        }
    }
}

void VSTEffect::setChunkData (const char* data, int size, bool isPreset)
{
    if (size > 0 && pPlug->UsesChunks())
    {
        pPlug->pEffect->EffDispatch(effSetChunk, isPreset ? 1 : 0, size, (void*) data, 0.0f);

        //if (!isPreset)
        //    updateStoredProgramNames();
    }
}

const String VSTEffect::GetCurrentPresetName()
{
    char nm[MAX_PRESET_NAME];
    GetPresetName(GetCurrentPreset(), nm);
	
    return String(nm).trim();
}

bool VSTEffect::restoreProgramSettings (const fxProgram* const prog)
{
    if (prog->chunkMagic == 'CcnK' && prog->fxMagic == 'FxCk')
    {
        SetPresetName((char*)prog->prgName);

        for (int i = 0; i < prog->numParams; ++i)
        {
            this->pPlug->SetParam(i, prog->params[i]);

            Parameter* extparam =  GetParamByIndex(i);
            extparam->SetNormalValue(prog->params[i]);
        }

        return true;
    }

    return false;
}

bool VSTEffect::loadFromFXBFile (const void* const data, const int dataSize)
{
    if (dataSize < 28)
        return false;

    const fxSet* const set = (const fxSet*) data;

    if (((set->chunkMagic) != 'CcnK' && (set->chunkMagic) != 'KncC')
         ||  (set->version) > fxbVersionNum)
        return false;

    if ((set->fxMagic) == 'FxBk')
    {
        // bank of programs
        if ((set->numPrograms) >= 0)
        {
            const int oldProg = GetCurrentPreset();
            const int numParams = (((const fxProgram*) (set->programs))->numParams);
            const int progLen = sizeof (fxProgram) + (numParams - 1) * sizeof (float);

            for (int i = 0; i < (set->numPrograms); ++i)
            {
                if (i != oldProg)
                {
                    const fxProgram* const prog = (const fxProgram*) (((const char*) (set->programs)) + i * progLen);
                    if(((const char*) prog) - ((const char*) set) >= dataSize)
                        return false;

                    if((set->numPrograms) > 0)
                        SetPresetByIndex(i);

                    if(! restoreProgramSettings (prog))
                        return false;
                }
            }

            if(set->numPrograms > 0)
                SetPresetByIndex(oldProg);

            const fxProgram* const prog = (const fxProgram*) (((const char*) (set->programs)) + oldProg * progLen);
            if (((const char*) prog) - ((const char*) set) >= dataSize)
                return false;

            if (! restoreProgramSettings (prog))
                return false;
        }
    }
    else if (set->fxMagic == 'FxCk')
    {
        // single program
        const fxProgram* const prog = (const fxProgram*) data;

        if (prog->chunkMagic != 'CcnK')
            return false;

        SetPresetName((char*)prog->prgName);

        for (int i = 0; i <  (prog->numParams); ++i)
		{
            this->pPlug->SetParam(i, prog->params[i]);

            Parameter* extparam =  GetParamByIndex(i);
			if(extparam != NULL)
			{
				extparam->SetNormalValue(prog->params[i]);
			}
		}
	}
    else if ( (set->fxMagic) == 'FBCh' ||  (set->fxMagic) == 'hCBF')
    {
        // non-preset chunk
        const fxChunkSet* const cset = (const fxChunkSet*) data;

        if ( (cset->chunkSize) + sizeof (fxChunkSet) - 8 > (unsigned int) dataSize)
            return false;

        setChunkData (cset->chunk,  (cset->chunkSize), false);
    }
    else if ( (set->fxMagic) == 'FPCh' || (set->fxMagic) == 'hCPF')
    {
        // preset chunk
        const fxProgramSet* const cset = (const fxProgramSet*) data;

        if (cset->chunkSize + sizeof (fxProgramSet) - 8 > (unsigned int) dataSize)
            return false;

        setChunkData (cset->chunk, cset->chunkSize, true);

        SetPresetName((char*)cset->name);
    }
    else
    {
        return false;
    }

    return true;
}

void VSTEffect::createTempParameterStore (MemoryBlock& dest)
{
    dest.setSize (64 + 4 * this->pPlug->GetNumParams());
    dest.fillWith (0);

    GetCurrentPresetName().copyToBuffer ((char*) dest.getData(), 63);

    float* const p = (float*) (((char*) dest.getData()) + 64);
    for (unsigned i = 0; i < this->pPlug->GetNumParams(); ++i)
        p[i] = this->pPlug->GetParam(i);
}

void VSTEffect::restoreFromTempParameterStore (const MemoryBlock& m)
{
    SetPresetName((char*)m.getData());

    float* p = (float*) (((char*) m.getData()) + 64);
    for (unsigned i = 0; i < this->pPlug->GetNumParams(); ++i)
        this->pPlug->SetParam(i, p[i]);
}

void VSTEffect::setParamsInProgramBlock (fxProgram* const prog) throw()
{
    const int numParams = this->pPlug->GetNumParams();

    prog->chunkMagic = ('CcnK');
    prog->byteSize = 0;
    prog->fxMagic = ('FxCk');
    prog->version = (fxbVersionNum);
    prog->fxID = 0; //vst_swap (getUID());
    prog->fxVersion = 0; //vst_swap (getVersionNumber());
    prog->numParams = (numParams);

    GetCurrentPresetName().copyToBuffer (prog->prgName, sizeof (prog->prgName) - 1);

    for (int i = 0; i < numParams; ++i)
        prog->params[i] = this->pPlug->GetParam(i);
}

bool VSTEffect::saveToFXBFile(MemoryBlock& dest, bool isFXB, int maxSizeMB)
{
    const int numPrograms = pPlug->GetNumPresets();
    const int numParams = pPlug->GetNumParams();

    if (pPlug->UsesChunks())
    {
        if(isFXB)
        {
            MemoryBlock chunk;
            getChunkData (chunk, false, maxSizeMB);

            const int totalLen = sizeof (fxChunkSet) + chunk.getSize() - 8;
            dest.setSize (totalLen, true);

            fxChunkSet* const set = (fxChunkSet*) dest.getData();
            set->chunkMagic = ('CcnK');
            set->byteSize = 0;
            set->fxMagic = ('FBCh');
            set->version = (fxbVersionNum);
            set->fxID = 0; //vst_swap (getUID());
            set->fxVersion = 0;//vst_swap (getVersionNumber());
            set->numPrograms = (numPrograms);
            set->chunkSize = (chunk.getSize());

            chunk.copyTo (set->chunk, 0, chunk.getSize());
        }
        else
        {
            MemoryBlock chunk;
            getChunkData (chunk, true, maxSizeMB);

            const int totalLen = sizeof (fxProgramSet) + chunk.getSize() - 8;
            dest.setSize (totalLen, true);

            fxProgramSet* const set = (fxProgramSet*) dest.getData();
            set->chunkMagic = ('CcnK');
            set->byteSize = 0;
            set->fxMagic = ('FPCh');
            set->version = (fxbVersionNum);
            set->fxID = 0; //(getUID());
            set->fxVersion = 0; //(getVersionNumber());
            set->numPrograms = (numPrograms);
            set->chunkSize = (chunk.getSize());

            GetCurrentPresetName().copyToBuffer (set->name, sizeof (set->name) - 1);
            chunk.copyTo (set->chunk, 0, chunk.getSize());
        }
    }
    else
    {
        if (isFXB)
        {
            const int progLen = sizeof (fxProgram) + (numParams - 1) * sizeof (float);
            const int len = (sizeof (fxSet) - sizeof (fxProgram)) + progLen * jmax (1, numPrograms);
            dest.setSize (len, true);

            fxSet* const set = (fxSet*) dest.getData();
            set->chunkMagic = ('CcnK');
            set->byteSize = 0;
            set->fxMagic = ('FxBk');
            set->version = (fxbVersionNum);
            set->fxID = 0; //(getUID());
            set->fxVersion = 0; //(getVersionNumber());
            set->numPrograms = (numPrograms);

            const int oldProgram = GetCurrentPreset();
            MemoryBlock oldSettings;
            createTempParameterStore (oldSettings);

            setParamsInProgramBlock ((fxProgram*) (((char*) (set->programs)) + oldProgram * progLen));

            for (int i = 0; i < numPrograms; ++i)
            {
                if (i != oldProgram)
                {
                    SetPresetByIndex(i);
                    setParamsInProgramBlock ((fxProgram*) (((char*) (set->programs)) + i * progLen));
                }
            }

            SetPresetByIndex(oldProgram);
            restoreFromTempParameterStore (oldSettings);
        }
        else
        {
            const int totalLen = sizeof (fxProgram) + (numParams - 1) * sizeof (float);
            dest.setSize (totalLen, true);

            setParamsInProgramBlock ((fxProgram*) dest.getData());
        }
    }

    return true;
}

void VSTEffect::GetStateInformation (MemoryBlock& destData)
{
    saveToFXBFile (destData, false, defaultMaxSizeMB);
}

void VSTEffect::SetStateInformation (const void* data, int sizeInBytes)
{
    loadFromFXBFile (data, sizeInBytes);
}

void VSTEffect::Save(XmlElement * xmlEff)
{
    Eff::Save(xmlEff);

    XmlElement* state = new XmlElement("STATE");

    MemoryBlock m;
    GetStateInformation(m);
    state->addTextElement(m.toBase64Encoding());
    xmlEff->addChildElement(state);
}

void VSTEffect::Load(XmlElement * xmlEff)
{
    SetParamLock(true);
    Eff::Load(xmlEff);
    SetParamLock(false);

    const XmlElement* const state = xmlEff->getChildByName (T("STATE"));
    if(state != 0)
    {
        MemoryBlock m;
        m.fromBase64Encoding (state->getAllSubText());

        SetStateInformation (m.getData(), m.getSize());
    }
}

void VSTEffect::ToggleParamWindow()
{
    if(VSTParamWnd == NULL)
    {
        VSTParamWnd = MainWnd->CreateVSTParamWindow(this, &scope);
        VSTParamWnd->Show();
    }
    else
    {
        if(VSTParamWnd->isVisible())
        {
            VSTParamWnd->Hide();
        }
        else
        {
            VSTParamWnd->Show();
        }
    }
}

//=================================================================================================
//                             PluginList Class Implementation
//=================================================================================================
CPluginList::CPluginList(Object* owner)
{
    this->owner = owner;
    this->num_items = 0;
    this->pFirst = NULL;
    this->pLast = NULL;
}

CPluginList::~CPluginList()
{
    ModListEntry *pEntry, *pEntry1;

    pEntry = this->pFirst;

    while (pEntry != NULL)
    {
        pEntry1 = pEntry;
        pEntry = pEntry->Next;
        delete pEntry1;
    }
}

void CPluginList::AddEntry(ModListEntry* pEntry)
{
    if (pEntry != NULL)
    {
        if(this->pFirst == NULL && pLast == NULL)
        {
            pEntry->Prev = NULL;
            pEntry->Next = NULL;
            this->pFirst = pEntry;
        }
        else
        {
            pLast->Next = pEntry;
            pEntry->Prev = pLast;
            pEntry->Next = NULL;
        }

        pLast = pEntry;
        ++(this->num_items);
    }
}

void CPluginList::AddEntry(ModuleType mtype, ModuleSubType mstype, const char * name, const char * path)
{
    ModListEntry* pEntry = NULL;

    pEntry = new ModListEntry;
    memset(pEntry, 0, sizeof(ModListEntry));
    pEntry->modtype = mtype;
    pEntry->subtype = mstype;
    strcpy(pEntry->name, name);
    strcpy(pEntry->path, path);

    AddEntry(pEntry);
}

void CPluginList::RemoveEntry(ModListEntry * pEntry)
{
    if((pEntry == pFirst)&&(pEntry == pLast))
    {
        pFirst = NULL;
        pLast = NULL;
    }
    else if(pEntry == pFirst)
    {
        pFirst = pEntry->Next;
        pFirst->Prev = NULL;
    }
    else if(pEntry == pLast)
    {
        pLast = pEntry->Prev;
        pLast->Next = NULL;
    }
    else
    {
        if(pEntry->Prev != NULL)
        {
            pEntry->Prev->Next = pEntry->Next;
        }
        if(pEntry->Next != NULL)
        {
            pEntry->Next->Prev = pEntry->Prev;
        }
    }
}

ModListEntry* CPluginList::SearchEntryByPath(const char* path)
{
    ModListEntry* pEntry = this->pFirst;

    while (pEntry != NULL)
    {
        if (_stricmp(pEntry->path, path) == 0)
        {
            /* just break and return pointer to an entry found*/
            break;
        }
        pEntry = pEntry->Next;
    }

    return pEntry;
}

//=================================================================================================
//                          Static local functions - Handlers for Effect controls
//=================================================================================================


void VSTEffEditButtonOnClick(Object* owner, Object* self)
{
    Butt* pButton = (Butt*)self;
    VSTEffect* effect = (VSTEffect*)(self->owner);

    if(effect->pPlug->HasEditor())
    {
        if(effect->pPlug->isWindowActive == false)
        {
            effect->ShowEditor(true);
        }
        else
        {
            effect->ShowEditor(false);
        }
    }
    else
    {
        if(pButton->pressed == false)
        {
            if(effect->VSTParamWnd != NULL)
            {
                effect->VSTParamWnd->Hide();
            }
        }
        else
        {
            if(effect->VSTParamWnd == NULL)
            {
                effect->VSTParamWnd = MainWnd->CreateVSTParamWindow(effect, &effect->scope);
                effect->wnd = effect->VSTParamWnd;
                effect->VSTParamWnd->Show();
            }
            else
            {
                effect->VSTParamWnd->Show();
            }
        }
    }
}

void EffWindowClick(Object* owner, Object* self)
{
    Toggle* wtg = (Toggle*)self;
    Eff* eff = (Eff*)(wtg->scope->eff);

    if(wtg->state != NULL && eff != NULL)
    {
        if(*(wtg->state) == true)
        {
            if(eff->paramWnd != NULL)
            {
                eff->paramWnd->Hide();
            }
        }
        else
        {
            if(eff->paramWnd == NULL)
            {
                eff->paramWnd = MainWnd->CreateParamWindow(&eff->scope);
                eff->wnd = eff->paramWnd;
                eff->paramWnd->Show();
            }
            else
            {
                eff->paramWnd->Show();
            }
        }
    }
}

