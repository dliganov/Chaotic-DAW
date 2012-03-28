#include "awful.h"
#include "awful_instruments.h"
#include "awful_controls.h"
#include "awful_params.h"
#include "awful_panels.h"
#include "VSTCollection.h"
#include "awful_effects.h"
#include "awful_utils_common.h"
#include "Awful_midi.h"
#include "Awful_JuceWindows.h"
#include "Awful_preview.h"
#include "awful_cursorandmouse.h"
#include "awful_undoredo.h"
#include "math.h"


void GenWindowToggleOnClick(Object* owner, Object* self);
void EditButtonClick(Object* owner, Object* self);

Instrument::Instrument()
{
    dereferenced = false;
    buff2 = buff3 = false;
    env_update = false;
    alias = NULL;
    alimg = NULL;
    envelopes = NULL;
    tg_first = tg_last = NULL;
    VLocEnv = NULL;
    PLocEnv = NULL;
    VEnv = NULL;
    PEnv = NULL;
    memset(name, 0, MAX_NAME_STRING);
    memset(path, 0, MAX_PATH_STRING);
    index = -1;

    scope.instr = this;

    last_length = 4;
    last_vol = 1;
    last_pan = 0;

    params = new ParamSet(100, 50, &scope);
    params->vol->SetNormalValue(1.0f);
    AddParam(params->vol);
    AddParam(params->pan);
    params->vol->setEnvDirect(true);
    params->pan->setEnvDirect(true);
    rampCounterV = 0;
    cfsV = 0;
    rampCounterP = 0;
    cfsP = 0;

    folded = false;
    fold = new Toggle(&folded, &scope);
    fold->panel = IP;
    layout_visible = true;
    layout = NULL;
    //layout = new Toggle(NULL, &layout_visible);
    //AddNewControl(layout, NULL);
    window_visible = false;
    window = new Toggle(this, Toggle_Window);
    mpan = new SliderHPan(NULL, &scope);
    mpan->SetHint("Instrument panning");
    params->pan->AddControl(mpan);
    mpan->SetAnchor(0);
    mvol = new SliderHVol(NULL, &scope);
    mvol->SetHint("Instrument volume");
    params->vol->AddControl(mvol);
    mvol->SetAnchor(1);
    mute = new Toggle(this, Toggle_Mute);
    mute->SetHint("Mute instrument");
    solo = new Toggle(this, Toggle_Solo);
    solo->SetHint("Solo instrument");
    AddNewControl(fold, IP);
    AddNewControl(mpan, NULL);
    AddNewControl(mvol, NULL);
    AddNewControl(mute, NULL);
    AddNewControl(solo, NULL);
    AddNewControl(window, NULL);

    autopt = new  Butt(false);
    autopt->SetOnClickHandler(&EditButtonClick);
    autopt->owner = (Object*)(this);
    autopt->instr = this;
    autopt->SetHint("Edit autopattern");
    AddNewControl(autopt, IP);

    this->pEditButton = new ButtFix(false);
    this->pEditButton->scope = &scope;
    this->pEditButton->instr = this;
    this->pEditButton->SetTitle("Edit");
    this->pEditButton->SetHint("Show editor window");
    this->pEditButton->SetOnClickHandler(&GenWindowToggleOnClick);
    this->pEditButton->owner = (Object*)(this);
    AddNewControl((Control*)(this->pEditButton), IP);

    solo->SetImages(NULL, img_solo1off);
    mute->SetImages(NULL, img_mute1off);
    //autopt->SetImages(NULL, img_autooff_s);
    //pEditButton->SetImages(NULL, img_btwnd_s);

    fxcell = &(mix->m_cell);
    fxchan = &(gAux->masterchan);

    dfxstr = new DigitStr("--");
    dfxstr->num_digits = 2;
    dfxstr->instr = this;
    dfxstr->highlightable = false;
    dfxstr->SetHint("Mixer channel number");
    Add_PEdit(dfxstr);

    alias = new TString((char*)"instrdefault", false, this);
    alias->highlightable = false;
    alias->SetHint("Instrument alias");
    Add_PEdit(alias);

    ptrk = new Trk();
    ptrk->Init(0, false);

    autoPatt = NULL;
    prevInst = NULL;

    vu = new VU(VU_Instr);
    vu->instr = this;
    vu->SetDecr(0.13f);
    //vu->SetCeil(0.2f);
    stepvu = new VU(VU_StepLine);
    stepvu->instr = this;

    rampCount = 512;

    instr_drawarea = new DrawArea((void*)this, Draw_Instrument);
    MC->AddDrawArea(instr_drawarea);
}

Instrument::~Instrument()
{
    Disable();

    MC->RemoveDrawArea(instr_drawarea);

    if(this == Solo_Instr)
        Solo_Instr = NULL;

    delete params;
    delete vu;
    delete stepvu;
    RemoveControlCommon(fold);
    RemoveControlCommon(window);
    RemoveControlCommon(mpan);
    RemoveControlCommon(mvol);
    RemoveControlCommon(mute);
    RemoveControlCommon(solo);
    RemoveControlCommon(autopt);
    RemoveControlCommon(pEditButton);

    Remove_PEdit(dfxstr);
    Remove_PEdit(alias);

    if(autoPatt != NULL)
    {
        RemoveOriginalPattern(autoPatt);
    }

    if(prevInst!= NULL)
    {
        delete prevInst;
    }

    //if(paramWin != NULL)
    {
    //    MainWnd->DeleteChild(paramWin);
    }

    undoMan->WipeInstrumentFromHistory(this);
}

void Instrument::QueueDeletion()
{
    to_be_deleted = true;
    Disable();
    DereferenceElements();
}

void Instrument::CopyDataToClonedInstrument(Instrument * instr)
{
    instr->params->vol->SetNormalValue(params->vol->val);
    instr->params->pan->SetNormalValue(params->pan->val);
    instr->params->muted = params->muted;
    instr->dfxstr->SetString(dfxstr->digits);
}

Instrument* Instrument::Clone()
{
    Instrument* instr = NULL;
    char nalias[MAX_ALIAS_STRING];
    GetOriginalInstrumentAlias(name, nalias);
    ToLowerCase(nalias);
    switch(type)
    {
        case Instr_Sample:
            // TODO: rework sample cloning to copy from internal memory
            instr = (Instrument*)Add_Sample(path, name, nalias);
            break;
        case Instr_VSTPlugin:
            instr = (Instrument*)Add_VSTGenerator((VSTGenerator*)this, nalias);
            break;
        case Instr_Generator:
            instr = (Instrument*)Add_Synth(nalias);
            break;
    }

    CopyDataToClonedInstrument(instr);

    ChangeCurrentInstrument(instr);

    /*
    if(autoPatt != NULL)
    {
        if(instr->autoPatt == NULL)
            instr->CreateAutoPattern();

        autoPatt->CopyElementsTo(instr->autoPatt, true);
    }
    */

    return instr;
}

bool Instrument::DereferenceElements()
{
    bool alldone = true;
    bool harddeleted = false;

    /// Clean preview
    Preview_ForceCleanForInstrument(this);

    /// Dereference and delete instances
    Instance* ii;
    Element* elnext;
    Element* el = firstElem;
    while(el != NULL)
    {
        elnext = el->next;
        if(el->IsInstance())
        {
            ii = (Instance*)el;
            if(ii->instr == this)
            {
				elnext = NextElementToDelete(ii);
                harddeleted = DeleteElement(ii, true, true);
                if(harddeleted == false)
                {
                    jassertfalse;
                    alldone = false;
                    /// Erase instrument reference to mark the element as obsolete since we do not currently
                    /// hardly delete active (playing) elements
                    ii->instr = NULL;
                    if(ii->type == El_Samplent)
                    {
                      ((Samplent*)ii)->sample = NULL;
                    }
                }
            }
        }
        else if(el->type == El_Command)
        {
            Command* cmd = (Command*)el;
            if(cmd->scope == &scope)
            {
                elnext = NextElementToDelete(el);
                harddeleted = DeleteElement(cmd, true, true);
                if(harddeleted == false)
                {
                    alldone = false;
                }
            }
        }
        else if(el->type == El_Pattern)
        {
            DereferencePattern((Pattern*)el);
        }

        el = elnext;
    }

    DereferencePattern(gAux->blankPt);

    dereferenced = alldone;

    return alldone;
}

void Instrument::DereferencePattern(Pattern* pt)
{
    if(pt->ibound != NULL && pt->ibound == this)
    {
        pt->ibound = NULL;
        if(pt->OrigPt != NULL && pt->OrigPt->ibound == this)
            pt->OrigPt->ibound = NULL;
    }
}

void Instrument::Reset()
{
}

void Instrument::ForceDeactivate()
{
    Trigger* tgnext;
    Trigger* tg = tg_first;
    while(tg != NULL)
    {
        tgnext = tg->loc_act_next;
        tg->Deactivate();
        tg = tgnext;
    }
}

void Instrument::ActivateTrigger(Trigger* tg)
{
    tg->frame_phase = 0;
    tg->sec_phase = 0;
    tg->outsync = false;
    tg->tworking = true;
    tg->muted = false;
    tg->broken = false;
    tg->auCount = 0;
	tg->frames_remaining = 0;
	tg->tgstate = TgState_Sustain;
    tg->signal = 1;

    if(tg_last == NULL)
    {
        tg->loc_act_prev = NULL;
        tg->loc_act_next = NULL;
        tg_first = tg;
    }
    else
    {
        tg_last->loc_act_next = tg;
        tg->loc_act_prev = tg_last;
        tg->loc_act_next = NULL;
    }
    tg_last = tg;
}

void Instrument::DeactivateTrigger(Trigger* tg)
{
    if(tg == tg_first && tg == tg_last)
    {
        tg_first = NULL;
        tg_last = NULL;
    }
    else if(tg == tg_first)
    {
        tg_first = tg->loc_act_next;
        tg_first->loc_act_prev = NULL;
    }
    else if(tg == tg_last)
    {
        tg_last = tg->loc_act_prev;
        tg_last->loc_act_next = NULL;
    }
    else
    {
        if(tg->loc_act_prev != NULL)
        {
            tg->loc_act_prev->loc_act_next = tg->loc_act_next;
        }
        if(tg->loc_act_next != NULL)
        {
            tg->loc_act_next->loc_act_prev = tg->loc_act_prev;
        }
    }
    tg->tworking = false;
}

void Instrument::Update()
{
    instr_drawarea->Change();
}

void Instrument::ParamEditUpdate(PEdit* pe)
{
    if(pe == alias)
    {
        GetInstrAliasImage(this);
        instr_drawarea->Change();

        R(Refresh_GridContent);
        R(Refresh_Aux);
    }
}

void Instrument::GenerateData(long num_frames, long mixbuffframe)
{
    long    frames_remaining;
    long    frames_to_process;
	long	actual;
    long    buffframe;
    long    mbframe;
    Trigger* tg;
    Trigger* tgnext;
    Trigger* tgenv;
    Envelope* env;
    Parameter* param;

    bool dofill = true;
    /*
    if(params->muted == false && (InstrSolo == NULL || InstrSolo == this))
    {
        dofill = true;
    }
    else
    {
        dofill = false;
    }*/

    memset(out_buff, 0, num_frames*sizeof(float)*2);

    frames_remaining = num_frames;
    buffframe = 0;
    mbframe = mixbuffframe;

    while(frames_remaining > 0)
    {
        if(frames_remaining > BUFF_CHUNK_SIZE)
        {
            frames_to_process = BUFF_CHUNK_SIZE;
        }
        else
        {
            frames_to_process = frames_remaining;
        }

        // Process envelopes for this instrument
        env_update = true;
        tgenv = envelopes;
        // Rewind to the very first, to provide correct envelopes overriding
        while(tgenv != NULL && tgenv->group_prev != NULL) tgenv = tgenv->group_prev;
        // Now process them all
        while(tgenv != NULL)
        {
            env = (Envelope*)((Command*)tgenv->el)->paramedit;
            if(env->newbuff && buffframe >= env->last_buffframe)
            {
                param = ((Command*)tgenv->el)->param;
                param->SetValueFromEnvelope(env->buff[mixbuffframe + buffframe], env);
            }
            tgenv = tgenv->group_next;
        }
        env_update = false;

        // Process triggers for the current chunk
        tg = tg_first;
        while(tg != NULL)
        {
            tgnext = tg->loc_act_next;

            // Clean inbuff to ensure obsolete data won't be used
            memset(in_buff, 0, num_frames*sizeof(float)*2);
            actual = WorkTrigger(tg, frames_to_process, frames_remaining, buffframe, mbframe);

            tg = tgnext;
        }

        frames_remaining -= frames_to_process;
        buffframe += frames_to_process;
        mbframe += frames_to_process;
    }

    //if(fill)
        FillMixChannel(num_frames, 0, mixbuffframe);
}

long Instrument::WorkTrigger(Trigger * tg, long num_frames, long remaining, long buffframe, long mixbuffframe)
{
    StaticInit(tg, num_frames);
    long actual_num_frames = ProcessTrigger(tg, num_frames, buffframe);

    if(actual_num_frames > 0)
    {
        PostProcessTrigger(tg, actual_num_frames, buffframe, mixbuffframe, remaining - actual_num_frames);
    }

    if(tg->lcount > 0)
    {
        for(int ic = 0; ic < actual_num_frames; ic++)
        {
            out_buff[(buffframe + ic)*2] += tg->auxbuff[int(rampCount - tg->lcount)*2];
            out_buff[(buffframe + ic)*2 + 1] += tg->auxbuff[int(rampCount - tg->lcount)*2 + 1];
            tg->lcount--;
            if(tg->lcount == 0)
                break;
        }
    }

    if(tg->tgstate == TgState_Finished)
    {
        tg->Deactivate();
    }

	return actual_num_frames;
}

// Autopattern creation
void Instrument::CreateAutoPattern()
{
    if(autoPatt != NULL)
    {
        delete autoPatt;
    }
    autoPatt = new Pattern("auto", 0.0f, 16.0f, 0, 0, true);
    autoPatt->instr_owner = this;
    autoPatt->OrigPt = autoPatt;
    autoPatt->autopatt = true;
    autoPatt->patt = autoPatt;
    autoPatt->trkdata = autoPatt->field_trkdata = ptrk;
    autoPatt->der_first = autoPatt->der_last = autoPatt;
    autoPatt->folded = true;
    autoPatt->pbk = pbkAux;
    autoPatt->Update();

    AddOriginalPattern(autoPatt);

    C.SaveState();

    C.SetPattern(autoPatt, Loc_SmallGrid);

    if(prevInst != NULL)
    {
        delete prevInst;
    }

    C.SetPos(0, 0);
    prevInst = CreateElement_Note(this, false);
    prevInst->patt = autoPatt;
    prevInst->Update();
    MakeTriggersForElement(prevInst, autoPatt);
	//DeleteElement(prevInst->tg_first->ai, true, true);
	prevInst->tg_first->ai = NULL;
    autoPatt->parent_trigger = prevInst->tg_first;
    prevInst->tg_first->vol_val = 1;
    prevInst->tg_first->pan_val = 0;

    /* Volume envelope postponed
    Command* cmdV = new Command(&scope, Cmd_LocVolEnv);
    cmdV->patt = autoPatt;
    cmdV->start_tick = 0;
    cmdV->track_line = 0;
    AddNewElement(cmdV, true);
    VLocEnv = cmdV;
    VEnv = (Envelope*)VLocEnv->paramedit;
    */

    /* Panning envelope postponed
    Command* cmdP = new Command(scope, Cmd_LocPanEnv);
    cmdP->patt = autoPatt;
    cmdP->start_tick = 0;
    cmdP->track_line = 5;
    AddNewElement(cmdP, true);
    PLocEnv = cmdP;
    PEnv = (Envelope*)PLocEnv->paramedit;
    */

    C.RestoreState();

    SpreadAutoPatterns();
}

void Instrument::SpreadAutoPatterns()
{
    Trigger* tg;
    Instance* ii;
    Event* ev = field->first_ev;
    while(ev != NULL)
    {
        tg = ev->tg_first;
        while(tg != NULL)
        {
            if(tg->el->IsInstance())
            {
                ii = (Instance*)tg->el;
                if(ii->instr == this && tg->ai == NULL)
                {
                    AddAutoInstance(tg, ii, true);
                }
            }
            tg = tg->ev_next;
        }
        ev = ev->next;
    }
}

void Instrument::PreProcessTrigger(Trigger* tg, bool* skip, bool* fill, long num_frames, long buffframe)
{
    // FXState is the sequencing-effects helper structure. Basically we use two FXStates - one for the track and
    // one for the patterns (used for autopatterns only)
    FXState*    fx1 = &tg->trkdata->fxstate; // Track stuff postponed
    FXState*    fx2 = NULL;
    Pattern*    ai;
    long        aiframe;
    Instance*   ii = (Instance*)tg->el;

    // fx2 stuff
    if(tg->ai != NULL)
    {
        ai = tg->ai;
        fx2 = &tg->ai->fxstate;
        aiframe = ai->parent_trigger->ev->frame;
    }
    else if(tg->patt->OrigPt->autopatt == true)
    {
        ai = tg->patt;
        fx2 = &tg->patt->fxstate;
        aiframe = 0;
    }

    if(tg->broken || ((fx1->t_break || (fx2 != NULL && fx2->t_break)) && tg->frame_phase > 0))
    {
        // Per break or mute we could leave filling for two or more times, if there're not enough num_frames
        // to cover ANTIALIASING_FRAMES number
        if(tg->broken == false)
        {
            tg->broken = true;
            tg->aaOUT = true;
            tg->aaCount = DECLICK_COUNT;
            tg->aaFilledCount = num_frames;
        }
        else
        {
            if(tg->aaFilledCount >= DECLICK_COUNT)
            {
               *fill = false;
               *skip = true;
                tg->tgstate = TgState_Finished;
            }
            else
            {
                tg->aaFilledCount += num_frames;
            }
        }
    }

    // Check conditions for muting
    if(ii->instr->params->muted == true || 
       /* tg->trkdata->params->muted == true || !(TrkSolo == NULL || TrkSolo == tg->trkdata)|| */
      !(Solo_Instr == NULL || Solo_Instr == ii->instr)||
       (fx1->t_mutecount > 0 || (fx2 != NULL && fx2->t_mutecount)))
    {
        // If note just begun then there's nothing to declick. Set aaFilledCount to full for immediate muting
        if(tg->frame_phase == 0)
        {
            tg->muted = true;
            tg->aaFilledCount = DECLICK_COUNT;
        }

        if(tg->muted == false)
        {
            // If note was already playig, then need to continue until DECLICK_COUNT number of frames are filled
            tg->muted = true;
            tg->aaOUT = true;
            tg->aaCount = DECLICK_COUNT;
            tg->aaFilledCount = num_frames;
        }
        else
        {
            if(tg->aaFilledCount >= DECLICK_COUNT)
            {
               *fill = false;
            }
            else
            {
                tg->aaFilledCount += num_frames;
            }
        }
    }
    else if(*fill == true)
    {
        if(tg->muted == true)
        {
            tg->muted = false;
            tg->aaIN = true;
            tg->aaCount = DECLICK_COUNT;
        }
    }

    // Do reverse only single time per whole instrument generation session
    if(buffframe == 0 && (fx1->t_reversed || (fx2 != NULL && fx2->t_reversed)))
    {
        tg->freq_incr_sgn *= -1;
    }

    float pv = 1; // Pitch value temporary workaround
/* Pitch is fucking postponed
    // Pitch processing. Pitch is a static envelope inside a pattern
    float pv = 1;
    if(tg->patt->OrigPt->pitch != NULL)
    {
        Envelope* env;
        env = (Envelope*)tg->patt->OrigPt->pitch->paramedit;
        env->ProcessBuffer1(tg->el->frame + tg->frame_phase,
						   0, 
                           num_frames, 
                           num_frames, 
                           tg->patt->OrigPt->pitch->frame);
        if(env->buffov[0] >= 0.5f)
        {
            pv = 1 + (env->buffov[0] - 0.5f)*0.828427124f;
        }
        else
        {
            pv = 1 - (0.5f - env->buffov[0])*0.585786437f;
        }
    }

    // Including autopattern's pitch
    if(tg->ai != NULL && tg->ai->OrigPt->pitch != NULL)
    {
        Envelope* env;
        env = (Envelope*)tg->ai->OrigPt->pitch->paramedit;
        env->ProcessBuffer1(tg->el->frame + tg->frame_phase, 
						   0,
                           num_frames, 
                           num_frames, 
                           tg->el->frame + tg->ai->OrigPt->pitch->frame);
        if(env->buffov[0] >= 0.5f)
        {
            pv *= (1 + (env->buffov[0] - 0.5f)*0.828427124f);
        }
        else
        {
            pv *= (1 - (0.5f - env->buffov[0])*0.585786437f);
        }
    }
*/
    tg->freq_incr_active *= pv*fx1->t_freq_mult;

    // External frequencers. Frequencers are effect-elements that affect note frequency e.g. vibratos,
    // slides, slidenotes, etc.
    SlideNote* sn;
    long fphase;
    Trigger* tgfreq;
    if(tg->outsync == false && ii->preview == false) // Block this for preview notes
    {
        tgfreq = fx1->freqtrig;
        while(tgfreq != NULL)
        {
            //if(tgfreq->ev->frame >= tg->ev->frame) //// commented as older frequencers do affect newer notes
            {
				// the phase, relative to the frequencer's start
                fphase = (long)(tg->ev->frame + tg->frame_phase - tgfreq->ev->frame);
				if(fphase > 0 && fphase <= tgfreq->el->frame_length) // to ensure it's the right case, when frequencer and note are in the single timeline
				{
					tg->freq_incr_active *= ((Frequencer*)tgfreq->el)->GetMult(tgfreq, tg, fphase, num_frames);
					if(tgfreq->ev->frame >= tg->ev->frame && tgfreq->el->type == El_SlideNote)
					{
						sn = (SlideNote*)tgfreq->el;
						tg->vol_val += sn->GetVolMult(tgfreq, tg, fphase, num_frames);
						tg->pan_val += sn->GetPanMult(tgfreq, tg, fphase, num_frames);
					}
				}
            }
            tgfreq = tgfreq->loc_act_prev;
        }
    }

    // Internal autopattern frequencers
    if(fx2 != NULL)
    {
        tg->freq_incr_active *= fx2->t_freq_mult;
        tgfreq = fx2->freqtrig;
        while(tgfreq != NULL)
        {
            //if(aiframe + tgfreq->ev->frame >= tg->ev->frame) //// commented as older frequencers do affect newer notes
            {
                fphase = (long)(tg->ev->frame + tg->frame_phase - (aiframe + tgfreq->ev->frame));
				if(fphase > 0 && fphase <= tgfreq->el->frame_length) // to ensure it's the right case, when frequencer and note are in the single timeline
				{
					tg->freq_incr_active *= ((Frequencer*)tgfreq->el)->GetMult(tgfreq, tg, fphase, num_frames);
					if(aiframe + tgfreq->ev->frame >= tg->ev->frame && tgfreq->el->type == El_SlideNote)
					{
						sn = (SlideNote*)tgfreq->el;
						tg->vol_val += sn->GetVolMult(tgfreq, tg, fphase, num_frames);
						tg->pan_val += sn->GetPanMult(tgfreq, tg, fphase, num_frames);
					}
				}
            }
            tgfreq = tgfreq->loc_act_prev;
        }
    }

    //float increment;
    //float decrement;
    //Trigger* tg_other;
    Trigger* tgs = tg->tgsactive;
    while(tgs != NULL)
    {
        sn = (SlideNote*)tgs->el;
        tg->freq_incr_active *= sn->GetMult(tgs, tg, tgs->frame_phase, num_frames);

        tg->vol_val += sn->GetVolMult(tgs, tg, tgs->frame_phase, num_frames);
        tg->pan_val += sn->GetPanMult(tgs, tg, tgs->frame_phase, num_frames);

        // Adjust signal levels (postponed)
        /*
        increment = sn->GetSignalIncr(tgs, tg, tgs->frame_phase)/tg->tgsactnum;
        decrement = increment/tg->tgsactnum;
        tgs->signal += increment;
        if(tgs->signal > 1)
        {
            tgs->signal = 1;
        }

        if(tg->signal > 0)
        {
            tg->signal -= decrement;
            if(tg->signal < 0)
            {
                tg->signal = 0;
            }
        }

        tg_other = tg->first_tgslide;
        while(tg_other != NULL)
        {
            if((tg_other != tgs)&&(tg_other->signal > 0))
            {
                tg_other->signal -= decrement;
                if(tg_other->signal < 0)
                {
                    tg_other->signal = 0;
                }
            }
            tg_other = tg_other->group_next;
        }*/

        tgs->frame_phase += num_frames;

        tgs = tgs->loc_act_prev;
    }
}

// Here initialized the stuff that does not change throughout the whole session
void Instrument::StaticInit(Trigger* tg, long num_frames)
{
    ii = (Instance*)tg->el;

    venvA = NULL;
    venv0 = NULL;
    venv1 = NULL;
    venv2 = NULL;
    venv3 = NULL;
    penv0 = NULL;
    penv2 = NULL;
    penv1 = NULL;
    penv3 = NULL;
    mcell = NULL;
    pan0 = pan1 = pan2 = pan3 = pan4 = 0;
    volbase = 0;
    venvphase = 0;
	end_frame = 0;

    //SetMixcell4Trigger(tg);
    //mcell = tg->mcell;

    //SetMixChannel4Trigger(tg);
    //mchan = tg->mchan;

/*
    if(VEnv != NULL)
    {
        venv0 = VEnv;
    }

    if(PEnv != NULL)
    {
        penv0 = PEnv;
    }
    if(venv0 != NULL)
    {
        if(tg->tgvolloc != NULL)
        {
            // We use separate phase variable for local volume because trigger's internal phase can go
            // too far in pre-buffering
            venvphase = tg->tgvolloc->frame_phase;
        }
        memset(venv0->buffov, 0, MAX_BUFF_SIZE*sizeof(float));
        venv0->ProcessBuffer1(tg->frame_phase, 0, num_frames, 1, 0, tg->tgvolloc, tg);
    }
*/
    if(penv0 != NULL && tg->frame_phase >= PLocEnv->frame)
    {
        // Postponed
        //penv0->ProcessBuffer(tg->frame_phase, 0, num_frames, 32, PLocEnv->frame, tg->tgpanloc, tg);
    }

    
    // Get envelope from this trigger's pattern (currently for autopatterns only).
    if(tg->patt->params->vol->envelopes != NULL)
    {
        venv2 = (Envelope*)((Command*)tg->patt->params->vol->envelopes->el)->paramedit;
        if(venv2->newbuff == false)
            venv2 = NULL;
    }

    /* // Tracks stuff postponed
    // If trigger is on field, then get third envelope from instace's track. If not - get 
    // third envelope from pattern's track.
    if(tg->patt == field)
    {
        if(ii->track_params->vol->envelopes != NULL)
        {
            venv3 = (Envelope*)((Command*)ii->track_params->vol->envelopes->el)->paramedit;
            if(venv3->newbuff == false)
                venv3 = NULL;
        }
    }
    else
    {
        if(tg->patt->track_params->vol->envelopes != NULL)
        {
            venv3 = (Envelope*)((Command*)tg->patt->track_params->vol->envelopes->el)->paramedit;
            if(venv3->newbuff == false)
                venv3 = NULL;
        }
    }
    */

    /// Init volume base. Volume base consist of volumes, that don't get changed during the whole fill session.
    /// it's then multiplied to dynamic tg->vol_val value in PostProcessTrigger.
    volbase = tg->patt->loc_vol->outval; // Pattern static params
    volbase *= InvVolRange;
    if(type == Instr_Generator)
    {
        volbase *= InvVolRange;
    }

    if(tg->patt->OrigPt->autopatt == true)
    {
        // Apply external autopattern stuff
        if(tg->patt->parent_trigger != NULL)
        {
            volbase *= tg->patt->parent_trigger->vol_val;
            volbase *= tg->patt->parent_trigger->patt->loc_vol->outval;
        }
    
        if(tg->patt->OrigPt->instr_owner != NULL)
        {
            volbase *= tg->patt->OrigPt->instr_owner->params->vol->outval;
        }
    }

    // Get envelope/value from this trigger's pattern.
    //if(venv2 == NULL)
    //{
    //    volbase *= tg->patt->params->vol->outval; // Pattern dynamic params
    //}

    /* Tracks stuff postponed
    // If trigger is on field, then get third envelope from instance's or pattern's track.
    if(venv3 == NULL)
    {
        if(tg->patt == field)
        {
            volbase *= ii->track_params->vol->outval;
        }
        else
        {
            volbase *= tg->patt->track_params->vol->outval;
        }
    }
    */

    ///// Panning envelopes
    if(tg->patt->params->pan->envelopes != NULL)
    {
        penv1 = (Envelope*)((Command*)tg->patt->params->pan->envelopes->el)->paramedit;
        if(penv1->newbuff == false)
            penv1 = NULL;
    }

    if(params->pan->envelopes != NULL)
    {
        penv2 = (Envelope*)((Command*)params->pan->envelopes->el)->paramedit;
        if(penv2->newbuff == false)
            penv2 = NULL;
    }

    /* Tracks stuff is postponed
    if(tg->patt == field)
    {
        if(ii->track_params->pan->envelopes != NULL)
        {
            penv3 = (Envelope*)((Command*)ii->track_params->pan->envelopes->el)->paramedit;
            if(penv3->newbuff == false)
                penv3 = NULL;
        }
    }
    else
    {
        if(tg->patt->track_params->pan->envelopes != NULL)
        {
            penv3 = (Envelope*)((Command*)tg->patt->track_params->pan->envelopes->el)->paramedit;
            if(penv3->newbuff == false)
                penv3 = NULL;
        }
    }
    */

    // Init pans
    //pan1 = tg->patt->params->pan->outval;     // Pattern's params
    pan1 = 0;
    pan2 = tg->patt->loc_pan->outval;     // Pattern's local panning
    pan3 = params->pan->outval;     // Instrument's panning

    /* Tracks stuff postponed
    if(tg->patt == field)
    {
        pan4 = ii->track_params->pan->outval;
    }
    else
    {
        pan4 = tg->patt->track_params->pan->outval;
    }
    */
}

void Instrument::DeClick(Trigger* tg, 
                                  long num_frames, 
                                  long buffframe, 
                                  long mixbuffframe, 
                                  long buff_remaining)
{
    long tc0;
    // Perform fadeout or fadein antialiasing
    float mul;
    if(tg->aaOUT)
    {
        int nc = 0;
        tc0 = buffframe*2;
        while(nc < num_frames)
        {
            if(tg->aaCount > 0)
            {
                mul = (float)tg->aaCount/DECLICK_COUNT;
                in_buff[tc0] *= mul;
                in_buff[tc0 + 1] *= mul;

                tc0++; tc0++;
                tg->aaCount--;
            }
            else
            {
                in_buff[tc0] = 0;
                in_buff[tc0 + 1] = 0;
                tc0++; tc0++;
            }
            nc++;
        }

        if(tg->aaCount == 0)
            tg->aaOUT = false;
    }
    else if(tg->aaIN)
    {
        int nc = 0;
        tc0 = buffframe*2;
        while(nc < num_frames && tg->aaCount > 0)
        {
            mul = (float)(DECLICK_COUNT - tg->aaCount)/DECLICK_COUNT;
            in_buff[tc0] *= mul;
            in_buff[tc0 + 1] *= mul;

            tc0++;
            tc0++;
            nc++;
            tg->aaCount--;
        }

        if(tg->aaCount == 0)
            tg->aaIN = false;
    }

    // Check if finished
    bool aaU = false;
    int aaStart;
	if(tg->tgstate == TgState_SoftFinish)
    {
        // Finished case
        aaU = true;
        if(tg->auCount == 0)
        {
            aaStart = end_frame - DECLICK_COUNT;
            if(aaStart < 0)
                aaStart = 0;
        }
        else
        {
            tg->tgstate = TgState_Finished;
            aaStart = 0;
        }
    }

    // Finish declick processing
    if(aaU == true)
    {
        jassert(aaStart >= 0);
        if(aaStart >= 0)
        {
            tc0 = buffframe*2 + aaStart*2;
            while(aaStart < num_frames)
            {
            if(tg->auCount < DECLICK_COUNT)
            {
                    tg->auCount++;
                    mul = float(DECLICK_COUNT - tg->auCount)/DECLICK_COUNT;
                    in_buff[tc0] *= mul;
                    in_buff[tc0 + 1] *= mul;

                    tc0++;
                    tc0++;
                }
                else
                {
                    in_buff[tc0] = 0;
                    in_buff[tc0 + 1] = 0;

                    tc0++;
                    tc0++;
                }
                aaStart++;
            }
	    }
	}
}

/*==================================================================================================
BRIEF: This function post-processes data generated per trigger. It applies all high-level params,
envelopes and fills corresponding mixcell.

PARAMETERS:
[IN]
    tg - trigger being processed.
    num_frames - number of frames to process.
    curr_frame - global timing frame number.
    buffframe - global buffering offset.
[OUT]
N/A

OUTPUT: N/A
==================================================================================================*/
void Instrument::PostProcessTrigger(Trigger* tg, 
                                        long num_frames, 
                                        long buffframe, 
                                        long mixbuffframe, 
                                        long buff_remaining)
{
    float       vol, pan;
    float       volL, volR;
    long        tc, tc0;
    float       envV;

    DeClick(tg, num_frames, buffframe, mixbuffframe, buff_remaining);

    vol = tg->vol_val*volbase;
    pan0 = tg->pan_val;
    int ai;
    tc = mixbuffframe*2;
    tc0 = buffframe*2;
    for(long cc = 0; cc < num_frames; cc++)
    {
/*      // Slidenote mixing postponed
        Trigger* stg = tg->first_tgslide;
        while(stg != NULL)
        {
            stg->mcell->in_buff[tc] += data_buff[tc0]*vol*volL*stg->signal;
            stg->mcell->in_buff[tc + 1] += data_buff[tc0 + 1]*vol*volR*stg->signal;
            stg = stg->group_next;
        }*/

        // pan0 is unchanged

        // then to pattern/env
        if(penv1 != NULL)
            pan1 = penv1->buffoutval[mixbuffframe + cc];

        // pan2 is unchanged

        // then to instrument/env
        if(penv2 != NULL)
            pan3 = penv2->buffoutval[mixbuffframe + cc];

        /* Track stuff postponed
        // then to track/env
        if(penv3 != NULL)
            pan4 = penv3->buffov[mixbuffframe + cc];
        */

        pan = (((pan0*(1 - c_abs(pan1)) + pan1)*(1 - c_abs(pan2)) + pan2)*(1 - c_abs(pan3)) + pan3);

        //tg->lastpan = pan;
        ////PanLinearRule(pan, &volL, &volR);
        //pan = 0;
        //volL = volR = 1;
        //if(pan > 0)
        //    volL -= pan;
        //else if(pan < 0)
        //    volR += pan;
        //PanConstantRule(pan, &volL, &volR);
        ai = int((PI_F*(pan + 1)/4)/wt_angletoindex);
        volL = wt_cosine[ai];
        volR = wt_sine[ai];

        envV = 1;
/*      if(venv0 != NULL)
        {
            envV *= venv0->buffov[buffframe + cc];
        }*/

        if(venv2 != NULL)
        {
            envV *= venv2->buffoutval[mixbuffframe + cc];
        }

        /* Track stuff postponed
        if(venv3 != NULL)
        {
            data_buff[tc0] *= venv3->buffov[mixbuffframe + cc];
            data_buff[tc0 + 1] *= venv3->buffov[mixbuffframe + cc];
            if(buff2)
            {
                data_buff2[tc0] *= venv3->buffov[mixbuffframe + cc];
                data_buff2[tc0 + 1] *= venv3->buffov[mixbuffframe + cc];
            }
            if(buff3)
            {
                data_buff3[tc0] *= venv3->buffov[mixbuffframe + cc];
                data_buff3[tc0 + 1] *= venv3->buffov[mixbuffframe + cc];
            }
        }
        */

        out_buff[tc0] += in_buff[tc0]*envV*vol*volL;
        out_buff[tc0 + 1] += in_buff[tc0 + 1]*envV*vol*volR;

        //float d = c_abs(out_buff[tc0] - tg->prevAAval);
        //if(d > 0.04f)
        //    int a = 1;
        //tg->prevAAval = out_buff[tc0];

        tc0++;
        tc0++;
    }
}

void Instrument::FillMixChannel(long num_frames, long buffframe, long mixbuffframe)
{
    // Get instrument envelope
    Envelope* venv = NULL;
    if(params->vol->envelopes != NULL && params->vol->envaffect)
    {
        venv = (Envelope*)((Command*)params->vol->envelopes->el)->paramedit;
        if(venv->newbuff == false)
        {
            venv = NULL;
        }
    }

    float vol = params->vol->outval;
    if(params->vol->lastval == -1)
    {
        params->vol->SetLastVal(params->vol->outval);
    }
    else if(params->vol->lastval != params->vol->outval)
    {
        if(rampCounterV == 0)
        {
            cfsV = float(params->vol->outval - params->vol->lastval)/RAMP_COUNT;
            vol = params->vol->lastval;
            rampCounterV = RAMP_COUNT;
        }
        else
        {
            vol = params->vol->lastval + (RAMP_COUNT - rampCounterV)*cfsV;
        }
    }
    else if(rampCounterV > 0) // (params->vol->lastval == params->vol->outval)
    {
        rampCounterV = 0;
        cfsV = 0;
    }

    float lMax, rMax, outL, outR;
    lMax = rMax = 0;
    long tc0 = buffframe*2;
    long tc = mixbuffframe*2;
    for(long cc = 0; cc < num_frames; cc++)
    {
        if(venv != NULL && rampCounterV == 0)
        {
            vol = venv->buffoutval[mixbuffframe + cc];
        }

        if(rampCounterV > 0)
        {
            vol += cfsV;
            rampCounterV--;
            if(rampCounterV == 0)
            {
                params->vol->SetLastVal(params->vol->outval);
            }
        }

        outL = out_buff[tc0++]*vol;
        outR = out_buff[tc0++]*vol;

        fxchan->in_buff[tc++] += outL;
        fxchan->in_buff[tc++] += outR;
        if(c_abs(outL) > lMax)
        {
            lMax = outL;
        }

        if(c_abs(outR) > rMax)
        {
            rMax = outR;
        }

        /*
        vucount--;
        if(vucount == 0)
        {
            vucount = 55;
            vu->SetLR(lMax, rMax);
            stepvu->SetLR(lMax, rMax);
            lMax = rMax = 0;
        }*/
    }

    lMax = pow(lMax, 0.4f);
    rMax = pow(rMax, 0.4f);
    vu->SetLR(lMax, rMax);
    stepvu->SetLR(lMax, rMax);
}

void Instrument::ApplyDSP(Trigger* tg, long buffframe, long num_frames)
{
    long tc0 = buffframe*2;
    for(long cc = 0; cc < num_frames; cc++)
    {
        in_buff[tc0] = data_buff[tc0];
        in_buff[tc0 + 1] = data_buff[tc0 + 1];
        if(buff2)
        {
            in_buff[tc0] += data_buff2[tc0];
            in_buff[tc0 + 1] += data_buff2[tc0 + 1];
        }
        if(buff3)
        {
            in_buff[tc0] += data_buff3[tc0];
            in_buff[tc0 + 1] += data_buff3[tc0 + 1];
        }

        tc0++;
        tc0++;
    }
}

void Instrument::Disable()
{
    zisible = false;
    instr_drawarea->Disable();
    vu->drawarea->Disable();
    stepvu->drawarea->Disable();
    mpan->Deactivate();
    mvol->Deactivate();
    solo->Deactivate();
    mute->Deactivate();
    window->Deactivate();
    dfxstr->Deactivate();
    alias->Deactivate();
}

void Instrument::Enable()
{
    zisible = true;
}

void Instrument::SetAlias(char * al)
{
    if(alias != NULL)
    {
        alias->SetString(al);
        GetInstrAliasImage(this);
    }
}

void Instrument::EnqueueParamEnvelopeTrigger(Trigger* tg)
{
    tg->tworking = true;
    if(envelopes != NULL)
    {
        envelopes->group_next = tg;
    }
    tg->group_prev = envelopes;
    tg->group_next = NULL;
    envelopes = tg;

    Parameter* param = ((Command*)tg->el)->param;
    {
        tg->prev_value = (param->val - param->offset)/param->range;
    }
    // New envelopes unblock the param ability to be changed by envelope
    param->UnblockEnvAffect();
}

void Instrument::DequeueParamEnvelope(Trigger* tg)
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

void Instrument::SetLastLength(float lastlength)
{
    last_length = lastlength;
}

void Instrument::SetLastPan(float lastpan)
{
    last_pan = lastpan;
}

void Instrument::SetLastVol(float lastvol)
{
    last_vol = lastvol;
}

void Instrument::FlowTriggers(Trigger* tgfrom, Trigger* tgto)
{
    memset(out_buff, 0, rampCount*sizeof(float)*2);
    memset(in_buff, 0, rampCount*sizeof(float)*2);
    memset(tgto->auxbuff, 0, rampCount*sizeof(float)*2);
    long actual = WorkTrigger(tgfrom, rampCount, rampCount, 0, 0);
    for(int ic = 0; ic < actual; ic++)
    {
        out_buff[ic*2] *= float(actual - ic)/actual;
        out_buff[ic*2 + 1] *= float(actual - ic)/actual;
    }

    memcpy(tgto->auxbuff, out_buff, actual*sizeof(float)*2);
    tgto->lcount = (float)rampCount;
}

void Instrument::Save(XmlElement * instrNode)
{
    instrNode->setAttribute(T("InstrIndex"), index);
    instrNode->setAttribute(T("InstrType"), int(type));
    instrNode->setAttribute(T("InstrName"), String(name));
    instrNode->setAttribute(T("InstrPath"), String(path));
    instrNode->setAttribute(T("InstrAlias"), String(alias->string));
    instrNode->setAttribute(T("MixChannel"), String(dfxstr->digits));

    instrNode->addChildElement(params->vol->Save());
    instrNode->addChildElement(params->pan->Save());

    instrNode->setAttribute(T("Mute"), int(params->muted));
    instrNode->setAttribute(T("Solo"), int(params->solo));

    /*
    if(autoPatt != NULL)
    {
        XmlElement* xmlPatt = new XmlElement(T("AutoPattern"));

        Element* el = autoPatt->first_elem;
        while(el != NULL)
        {
            if(el->IsPresent() && el->type != El_Pattern)
            {
                XmlElement* xmlElem = new XmlElement(T("Element"));
                el->Save(xmlElem);
                xmlPatt->addChildElement(xmlElem);
            }
            el = el->patt_next;
        }

        instrNode->addChildElement(xmlPatt);
    }
    */

    //SaveStateData(*instrNode, "Current");
}

void Instrument::Load(XmlElement * instrNode)
{
    index = instrNode->getIntAttribute(T("InstrIndex"), -1);

    String mchan = instrNode->getStringAttribute(T("MixChannel"));
    if(mchan.length() > 0)
    {
        dfxstr->SetString((const char*)mchan);
    }

    XmlElement* xmlParam = NULL;
    forEachXmlChildElementWithTagName(*instrNode, xmlParam, T("Parameter"))
    {
        int idx = xmlParam->getIntAttribute(T("index"), -1);
        if(idx == params->vol->index)
        {
            params->vol->Load(xmlParam);
        }
        else if(idx == params->pan->index)
        {
            params->pan->Load(xmlParam);
        }
    }

    bool mute = instrNode->getIntAttribute(T("Mute")) == 1;
    params->muted = mute;
    bool solo = instrNode->getIntAttribute(T("Solo")) == 1;
    params->solo = solo;
    if(solo)
    {
        Solo_Instr = this;
    }

    /*
    XmlElement* xmlPatt = instrNode->getChildByName(T("AutoPattern"));
    if(xmlPatt != NULL)
    {
        if(autoPatt == NULL)
        {
            CreateAutoPattern();
            //if(autoPatt != NULL)
            //{
            //    LoadElementsFromNode(xmlPatt, autoPatt);
            //}
        }
    }*/

    //XmlElement* stateNode = instrNode->getChildByName(T("Module"));
    //if(stateNode != NULL)
    //{
    //    RestoreStateData(*stateNode);
    //}
}

Sample::Sample(float* data, char* pth, char* nm, SF_INFO sfinfo)
{
    type = Instr_Sample;
    sample_data = data;
	if(pth != NULL)
		strcpy(path, pth);
	if(nm != NULL)
		strcpy(name, nm);
    strcpy(this->preset_path, "");
    info = sfinfo;
    rateDown = (float)info.samplerate/fSampleRate;
    rateUp = fSampleRate/info.samplerate;
    //smpwindow = NULL;
    last_length = 2.0f;
    normalized = false;
    touchresized = false;
    norm_factor = 1.0f;
    looptype = LoopType_NoLoop;
    SetLoopPoints(0, long(info.frames - 1));
    UpdateNormFactor();

    // Built-in delay for the sample
    //delay = new XDelay(NULL, NULL);
    //delay->scope.instr = this;
    //delayOn = new BoolParam(false);

    timelen = info.frames/fSampleRate;
    VEnv = new Envelope(Cmd_ParamEnv);
    VEnv->len = jmax(timelen, 2.f);
    VEnv->timebased = true;

    // Add some default breakpoints to sample envelope
    VEnv->SetSustainPoint(VEnv->AddPoint(0.0f, 1.0f));
    //VEnv->AddPoint(0.3f, 0.5f);
    //VEnv->AddPoint(0.6f, 0.0f);
}

Sample::~Sample()
{

}

void Sample::Save(XmlElement * instrNode)
{
    Instrument::Save(instrNode);

    instrNode->setAttribute(T("Normalized"), normalized ? 1 : 0);
    instrNode->setAttribute(T("LoopStart"), String(lp_start));
    instrNode->setAttribute(T("LoopEnd"), String(lp_end));
    instrNode->setAttribute(T("LoopType"), int(looptype));

    XmlElement * envX = VEnv->Save("SmpEnv");
    instrNode->addChildElement(envX);
}

void Sample::Load(XmlElement * instrNode)
{
    Instrument::Load(instrNode);

    bool norm = instrNode->getBoolAttribute(T("Normalized"));
    if(normalized != norm)
        ToggleNormalize();
    lp_start = (tframe)instrNode->getStringAttribute(T("LoopStart")).getLargeIntValue();
    lp_end = (tframe)instrNode->getStringAttribute(T("LoopEnd")).getLargeIntValue();
    looptype = (LoopType)instrNode->getIntAttribute(T("LoopType"));

    XmlElement * envX = instrNode->getChildByName(T("SmpEnv"));
	if(envX != NULL)
	{
		VEnv->Load(envX);
	}
}

void Sample::ActivateTrigger(Trigger* tg)
{
    Samplent* samplent = (Samplent*)tg->el;
    bool skip = !(samplent->InitCursor(&tg->wt_pos));
    if(skip == false)
    {
        tg->outsync = false;
        tg->tworking = true;
        tg->muted = false;
        tg->broken = false;
        tg->auCount = 0;
        tg->frames_remaining = 0;
        tg->tgstate = TgState_Sustain;
        tg->frame_phase = 0;
        tg->sec_phase = 0;
        tg->signal = 1;
        tg->vol_base = samplent->loc_vol->outval;
        tg->pan_base = samplent->loc_pan->outval;
        tg->freq_incr_base = samplent->freq_incr_base;
        tg->freq = samplent->ed_note->freq;
        tg->note_val = samplent->ed_note->value;
        tg->ep1 = VEnv->p_first;
        tg->envVal1 = tg->ep1->y_norm;
        tg->env_phase1 = 0;
        if(samplent->ed_note->relative == true)
        {
            jassert(tg->patt->parent_trigger != NULL);
            Instance* pi = (Instance*)tg->patt->parent_trigger->el;
            if(pi->freq_ratio > 0)
                tg->freq_incr_base /= pi->freq_ratio;
            tg->note_val += pi->ed_note->value;
            tg->freq = NoteToFreq(pi->ed_note->value + samplent->ed_note->value);
        }
        tg->freq_incr_sgn = !samplent->revB ? 1 : -1;

        Trigger* tgs = tg->first_tgslide;
        while(tgs != NULL)
        {
            tgs->signal = 0;
            tgs = tgs->group_next;
        }

        if(tg_last == NULL)
        {
            tg->loc_act_prev = NULL;
            tg->loc_act_next = NULL;
            tg_first = tg;
        }
        else
        {
            tg_last->loc_act_next = tg;
            tg->loc_act_prev = tg_last;
            tg->loc_act_next = NULL;
        }
        tg_last = tg;
    }
}

bool Sample::CheckBounds(Samplent* samplent, Trigger* tg, long num_frames)
{
    if(looptype == LoopType_NoLoop)
    {
        if(samplent->IsOutOfBounds(&tg->wt_pos) == true)
        {
            tg->tgstate = TgState_Finished;
            //end_frame = num_frames - ANTIALIASING_FRAMES;
            //if(end_frame < 0)
            end_frame = 0;
        }
        else if(tg->frame_phase >= samplent->frame_length)
        {
            tg->tgstate = TgState_SoftFinish;
        }
    }
    else
    {
        if(tg->tgstate == TgState_Sustain && samplent->preview == false && tg->frame_phase >= samplent->frame_length)
        {
            // Go to release state as note is over
            tg->Release();
        }
    }

    if(tg->tgstate == TgState_Release)
    {
        if(VEnv != NULL)
        {
            /*
            bool finished = false;
            if(tg->tgvolloc != NULL)
            {
                finished = VEnv->IsOutOfBounds(venvphase);
                end_frame = num_frames - (venvphase - VEnv->frame_length);
            }
            else
            {
                finished = VEnv->IsOutOfBounds(tg->frame_phase);
                end_frame = num_frames - (tg->frame_phase - VEnv->frame_length);
            }

            if(finished)
                tg->tgstate = TgState_Finished;*/

            if(tg->ep1->next == NULL)
            {
                tg->SoftFinish();
            }
            //if(end_frame < 0)
            end_frame = 0;
        }
        else
        {
            tg->SoftFinish();
        }
    }

    if(tg->tgstate == TgState_Finished)
    {
        return true;
    }

    return false;
}

long Sample::ProcessTrigger(Trigger * tg, long num_frames, long buffframe)
{
    Samplent* samplent = (Samplent*)tg->el;
    fill = true;
    skip = false;

    // Initial stuff
    tg->freq_incr_active = tg->freq_incr_base;
    tg->vol_val = tg->vol_base;
    tg->pan_val = tg->pan_base;
    PreProcessTrigger(tg, &skip, &fill, num_frames, buffframe);

    if(!skip)
    {
        long cc, tc0 = buffframe*2;
        float sd1, sd2;
        for(cc = 0; cc < num_frames; cc++)
        {
            if(samplent->IsOutOfBounds(&tg->wt_pos) == true)
                break;

            if(fill)
            {
                if(1 == samplent->sample->info.channels) // Mono sample
                {
                    GetMonoSampleData(samplent->sample, tg->wt_pos, &sd1);
                    in_buff[tc0++] = sd1*tg->envVal1;
                    in_buff[tc0++] = sd1*tg->envVal1;
                }
                else if(2 == samplent->sample->info.channels) // Stereo sample
                {
                    GeStereoSampleData(samplent->sample, tg->wt_pos, &sd1, &sd2);
                    in_buff[tc0++] = sd1*tg->envVal1;
                    in_buff[tc0++] = sd2*tg->envVal1;
                }
            }

            if(tg->tgstate == TgState_Sustain && VEnv->sustainable && tg->ep1->suspoint == true)
            {
                tg->env_phase1 = tg->ep1->x;
                tg->envVal1 = tg->ep1->y_norm;
                tg->envVal1 = GetVolOutput(tg->envVal1);
            }
            else
            {
                tg->env_phase1 += one_divided_per_sample_rate;
                while(tg->ep1->next != NULL && (tg->env_phase1 >= tg->ep1->next->x || tg->ep1->next->x == tg->ep1->x))
                {
                    tg->ep1 = tg->ep1->next;
                }
                if(tg->ep1->next != NULL && (tg->ep1->next->x > tg->ep1->x))
                {
                    tg->envVal1 = Interpolate_Line(tg->ep1->x, tg->ep1->y_norm, tg->ep1->next->x, tg->ep1->next->y_norm, tg->env_phase1); //tg->ep1->cf*one_divided_per_sample_rate;
                    tg->envVal1 = GetVolOutput(tg->envVal1);
                }
            }
            //ebuff1[buffframe + ic] = tg->envV1;

            tg->wt_pos += tg->freq_incr_sgn*tg->freq_incr_active;
            if(tg->freq_incr_sgn == 1 && tg->wt_pos >= lp_end)
            {
                if(looptype == LoopType_ForwardLoop)
                {
                    tg->wt_pos = lp_start;
                }
                else if(looptype == LoopType_PingPongLoop)
                {
                    tg->freq_incr_sgn = -1;
					tg->wt_pos = lp_end;
                }
            }
            else if(tg->freq_incr_sgn == -1 && tg->wt_pos <= lp_start)
            {
                if(looptype == LoopType_ForwardLoop)
                {
                    tg->wt_pos = lp_end;
                }
                else if(looptype == LoopType_PingPongLoop)
                {
                    tg->freq_incr_sgn = 1;
                    tg->wt_pos = lp_start;
                }
            }

            tg->frame_phase++;
        }

        CheckBounds(samplent, tg, cc);

        return cc;
    }
    else
    {
        return 0;
    }
}

void Sample::ShowWindow()
{
    //if(smpwindow == NULL)
    //{
        //smpwindow = new InstrWindow(sample_name);
    //}

    //smpwindow->Show();
}

void Sample::CloseWindow()
{
}

void Sample::UpdateNormFactor()
{
    float dmax = 0;
    float dcurr;
    long dc = 0;
    long total = long(info.frames*info.channels);
    while(dc < total)
    {
        dcurr = fabs(sample_data[dc]);
        if(dcurr > dmax)
        {
            dmax = dcurr;
        }
        dc++;
    }

    if(dmax > 0)
    {
        norm_factor = 1.0f/dmax;
    }
}

void Sample::ToggleNormalize()
{
    if(norm_factor != 0)
    {
        normalized = !normalized;
        float mult;
        if(normalized == true)
        {
            mult = norm_factor;
        }
        else
        {
            mult = 1.0f/norm_factor;
        }

        long dc = 0;
        long total = long(info.frames*info.channels);
        while(dc < total)
        {
            sample_data[dc] *= mult;
            dc++;
        }
    }
}

void Sample::ApplyDSP(Trigger* tg, long buffframe, long num_frames)
{
    if(delayOn->outval == true)
    {
        delay->ProcessData(&data_buff[buffframe*2], &in_buff[buffframe*2], num_frames);
    }
    else
    {
        memcpy(&in_buff[buffframe*2], &data_buff[buffframe*2], num_frames*(sizeof(float))*2);
    }
}

void Sample::SetLoopStart(long start)
{
    lp_start = start;
}

void Sample::SetLoopEnd(long end)
{
    lp_end = end;
}

void Sample::SetLoopPoints(long start, long end)
{
    lp_start = start;
    lp_end = end;
}

void Sample::CopyDataToClonedInstrument(Instrument * instr)
{
    Instrument::CopyDataToClonedInstrument(instr);

    Sample* nsample = (Sample*)instr;
    delete nsample->VEnv;
    nsample->VEnv = VEnv->Clone();

    if(normalized != nsample->normalized)
    {
        nsample->ToggleNormalize();
    }
    nsample->touchresized = touchresized;
    nsample->SetLoopPoints(lp_start, lp_end);
    nsample->looptype = looptype;
}

void Sample::DumpData()
{
    File f("sampledump.bin");
    f.create();
    FileOutputStream* os = f.createOutputStream();
    os->write(sample_data, int(info.frames*info.channels*sizeof(float)));
    delete os;
}

Gen::Gen()
{
    type = Instr_Generator;

    //solo->SetImages(NULL, img_solo1off_c);
    //mute->SetImages(NULL, img_mute1off_c);
    //autopt->SetImages(NULL, img_autooff_g);
    //pEditButton->SetImages(NULL, img_btwnd_g);
}

Gen::~Gen()
{
}

void Gen::ActivateTrigger(Trigger* tg)
{
    Gennote* gnote = (Gennote*)tg->el;

    tg->outsync = false;
    tg->tworking = true;
    tg->muted = false;
    tg->broken = false;
    tg->auCount = 0;
    tg->frames_remaining = 0;
    tg->tgstate = TgState_Sustain;
    tg->frame_phase = 0;
    tg->sec_phase = 0;
    tg->vol_base = gnote->loc_vol->outval;
    tg->pan_base = gnote->loc_pan->outval;
    tg->freq = gnote->freq;
    tg->note_val = gnote->ed_note->value;
    if(gnote->ed_note->relative == true)
    {
        Instance* pi = (Instance*)tg->patt->parent_trigger->el;
        if(pi->freq_ratio > 0)
            tg->freq /= pi->freq_ratio;
        tg->note_val += pi->ed_note->value;
        tg->freq = NoteToFreq(pi->ed_note->value + gnote->ed_note->value);
    }

    tg->wt_pos = 0;
    tg->signal = 1;
    tg->prevAAval = tg->prevAAval1 = 0;

    if(tg_last == NULL)
    {
        tg->loc_act_prev = NULL;
        tg->loc_act_next = NULL;
        tg_first = tg;
    }
    else
    {
        tg_last->loc_act_next = tg;
        tg->loc_act_prev = tg_last;
        tg->loc_act_next = NULL;
    }
    tg_last = tg;
}

void Gen::CheckBounds(Gennote* gnote, Trigger* tg, long num_frames)
{
	if(tg->tgstate == TgState_Sustain && gnote->preview == false && tg->frame_phase >= gnote->frame_length)
    {
        // Go to release state as note is over
        tg->Release();
    }

    // Finishing can happen both when envelope is out of bounds and when the note length is longer than envelope 
    // length and the note is finished (frame phase exceeds note frame length)
	if(tg->tgstate == TgState_Release && VEnv != NULL)
    {
        bool finished = false;
        if(tg->tgvolloc != NULL)
        {
			finished = VEnv->IsOutOfBounds(venvphase);
            end_frame = num_frames - (venvphase - VEnv->frame_length);
        }
		else
        {
			finished = VEnv->IsOutOfBounds(tg->frame_phase);
            end_frame = num_frames - (tg->frame_phase - VEnv->frame_length);
        }

        if(finished)
            tg->tgstate = TgState_Finished;

        if(end_frame < 0)
        {
            end_frame = 0;
        }
    }
}

Synth::Synth()
{
    SetName("Chaosynth");

    strcpy(this->preset_path, ".\\Presets\\Chaotic\\");
    strcpy(this->path,"internal://chaotic");
    uniqueID = MAKE_FOURCC('C','S','Y','N');

    fp = 0;
    rate = 0.0001f;
    numUsedVoices = 0;
    maxVoices = 5;

    osc1Fat = new BoolParam(false, "Osc1.3x");
    osc2Fat = new BoolParam(false, "Osc2.3x");
    osc3Fat = new BoolParam(false, "Osc3.3x");
    AddParam(osc1Fat);
    AddParam(osc2Fat);
    AddParam(osc3Fat);

    osc1Saw = new BoolParam(false, "Osc1.saw");
    osc1Sine = new BoolParam(true, "Osc1.sine");
    osc1Tri = new BoolParam(false, "Osc1.triangle");
    osc1Pulse = new BoolParam(false, "Osc1.pulse");
    osc1Noise = new BoolParam(false, "Osc1.noise");
    AddParam(osc1Saw);
    AddParam(osc1Sine);
    AddParam(osc1Tri);
    AddParam(osc1Pulse);
    AddParam(osc1Noise);
 
    osc2Saw = new BoolParam(false, "Osc2.saw");
    osc2Sine = new BoolParam(true, "Osc2.sine");
    osc2Tri = new BoolParam(false, "Osc2.triangle");
    osc2Pulse = new BoolParam(false, "Osc2.pulse");
    osc2Noise = new BoolParam(false, "Osc2.noise");
    AddParam(osc2Saw);
    AddParam(osc2Sine);
    AddParam(osc2Tri);
    AddParam(osc2Pulse);
    AddParam(osc2Noise);

    osc3Saw = new BoolParam(false, "Osc3.saw");
    osc3Sine = new BoolParam(true, "Osc3.sine");
    osc3Tri = new BoolParam(false, "Osc3.triangle");
    osc3Pulse = new BoolParam(false, "Osc3.pulse");
    osc3Noise = new BoolParam(false, "Osc3.noise");
    AddParam(osc3Saw);
    AddParam(osc3Sine);
    AddParam(osc3Tri);
    AddParam(osc3Pulse);
    AddParam(osc3Noise);

// 0.059463094 - single semitone
// 0.029731547 - single semitone
    osc1Level = new Parameter("Osc1.level", 1, 0, 1, Param_Default);
    osc1Level->AddValueString(VStr_Percent);
    osc1Detune = new Parameter("Osc1.detune", 0.0f, -0.029731547f, 0.029731547f*2, Param_Default_Bipolar);
    osc1Detune->AddValueString(VStr_Percent);
    osc1Octave = new Parameter("Osc1.octave", -2, -3, 6, Param_Default_Bipolar);
    osc1Octave->SetInterval(1);
    osc1Octave->AddValueString(VStr_Integer);
    osc1Width = new Parameter("Osc1.pwidth", 0.5f, 0.1f, 0.4f, Param_Default);
    osc1Width->AddValueString(VStr_Default);
    AddParam(osc1Level);
    AddParam(osc1Detune);
    AddParam(osc1Octave);
    AddParam(osc1Width);

    osc2Level = new Parameter("Osc2.level", 1, 0, 1, Param_Default);
    osc2Level->AddValueString(VStr_Percent);
    osc2Detune = new Parameter("Osc2.detune", 0.0f, -0.029731547f, 0.029731547f*2, Param_Default_Bipolar);
    osc2Detune->AddValueString(VStr_Percent);
    osc2Octave = new Parameter("Osc2.octave", -1, -3, 6, Param_Default_Bipolar);
    osc2Octave->SetInterval(1);
    osc2Octave->AddValueString(VStr_Integer);
    osc2Width = new Parameter("Osc2.pwidth", 0.5f, 0.1f, 0.4f, Param_Default);
    osc2Width->AddValueString(VStr_Default);
    AddParam(osc2Level);
    AddParam(osc2Detune);
    AddParam(osc2Octave);
    AddParam(osc2Width);

    osc3Level = new Parameter("Osc3.level", 1, 0, 1, Param_Default);
    osc3Level->AddValueString(VStr_Percent);
    osc3Detune = new Parameter("Osc3.detune", 0.0f, -0.029731547f, 0.029731547f*2, Param_Default_Bipolar);
    osc3Detune->AddValueString(VStr_Percent);
    osc3Octave = new Parameter("Osc3.octave", 0, -3, 6, Param_Default_Bipolar);
    osc3Octave->SetInterval(1);
    osc3Octave->AddValueString(VStr_Integer);
    osc3Width = new Parameter("Osc3.pwidth", 0.5f, 0.1f, 0.4f, Param_Default);
    osc3Width->AddValueString(VStr_Default);
    AddParam(osc3Level);
    AddParam(osc3Detune);
    AddParam(osc3Octave);
    AddParam(osc3Width);

    filt1Osc1 = new BoolParam(true, "Flt1.Osc1");
    filt1Osc2 = new BoolParam(false, "Flt1.Osc2");
    filt1Osc3 = new BoolParam(false, "Flt1.Osc3");
    AddParam(filt1Osc1);
    AddParam(filt1Osc2);
    AddParam(filt1Osc3);

    filt2Osc1 = new BoolParam(false, "Flt2.Osc1");
    filt2Osc2 = new BoolParam(true, "Flt2.Osc2");
    filt2Osc3 = new BoolParam(false, "Flt2.Osc3");
    AddParam(filt2Osc1);
    AddParam(filt2Osc2);
    AddParam(filt2Osc3);

    chorusON = new BoolParam(false, "Chorus ON");
    delayON = new BoolParam(false, "Delay ON");
    reverbON = new BoolParam(false, "Reverb ON");
    AddParam(chorusON);
    AddParam(delayON);
    AddParam(reverbON);

    LFOAmp_Vol1 = new Parameter("LFOAmp.Osc1.level", 0, 0, 1, Param_Default);
    LFOAmp_Vol1->AddValueString(VStr_Percent);
    LFORate_Vol1 = new Parameter("LFORate.Osc1.level", 3.0f, 0, 20, Param_Default);
    LFORate_Vol1->AddValueString(VStr_fHz1);
    LFOAmp_Vol2 = new Parameter("LFOAmp.Osc2.level", 0, 0, 1, Param_Default);
    LFOAmp_Vol2->AddValueString(VStr_Percent);
    LFORate_Vol2 = new Parameter("LFORate.Osc2.level", 3.0f, 0, 20, Param_Default);
    LFORate_Vol2->AddValueString(VStr_fHz1);
    LFOAmp_Vol3 = new Parameter("LFOAmp.Osc3.level", 0, 0, 1, Param_Default);
    LFOAmp_Vol3->AddValueString(VStr_Percent);
    LFORate_Vol3 = new Parameter("LFORate.Osc3.level", 3.0f, 0, 20, Param_Default);
    LFORate_Vol3->AddValueString(VStr_fHz1);
    AddParam(LFOAmp_Vol1);
    AddParam(LFORate_Vol1);
    AddParam(LFOAmp_Vol2);
    AddParam(LFORate_Vol2);
    AddParam(LFOAmp_Vol3);
    AddParam(LFORate_Vol3);

    LFOAmp_Pitch1 = new Parameter("LFOAmp.Osc1.pitch", 0, 0, 0.059463094f, Param_Default);
    LFOAmp_Pitch1->AddValueString(VStr_Percent);
    LFORate_Pitch1 = new Parameter("LFORate.Osc1.pitch", 3.0f, 0, 20, Param_Default);
    LFORate_Pitch1->AddValueString(VStr_fHz1);
    LFOAmp_Pitch2 = new Parameter("LFOAmp.Osc2.pitch", 0, 0, 0.059463094f, Param_Default);
    LFOAmp_Pitch2->AddValueString(VStr_Percent);
    LFORate_Pitch2 = new Parameter("LFORate.Osc2.pitch", 3.0f, 0, 20, Param_Default);
    LFORate_Pitch2->AddValueString(VStr_fHz1);
    LFOAmp_Pitch3 = new Parameter("LFOAmp.Osc3.pitch", 0, 0, 0.059463094f, Param_Default);
    LFOAmp_Pitch3->AddValueString(VStr_Percent);
    LFORate_Pitch3 = new Parameter("LFORate.Osc3.pitch", 3.0f, 0, 20, Param_Default);
    LFORate_Pitch3->AddValueString(VStr_fHz1);
    AddParam(LFOAmp_Pitch1);
    AddParam(LFORate_Pitch1);
    AddParam(LFOAmp_Pitch2);
    AddParam(LFORate_Pitch2);
    AddParam(LFOAmp_Pitch3);
    AddParam(LFORate_Pitch3);

    LFOAmp_Flt1Freq = new Parameter("LFOAmp.Flt1.freq", 0, 0, 1, Param_Default);
    LFOAmp_Flt1Freq->AddValueString(VStr_Percent);
    LFORate_Flt1Freq = new Parameter("LFORate.Flt1.freq", 3.0f, 0, 20, Param_Default);
    LFORate_Flt1Freq->AddValueString(VStr_fHz1);
    LFOAmp_Flt2Freq = new Parameter("LFOAmp.Flt2.freq", 0, 0, 1, Param_Default);
    LFOAmp_Flt2Freq->AddValueString(VStr_Percent);
    LFORate_Flt2Freq = new Parameter("LFORate.Flt2.freq", 3.0f, 0, 20, Param_Default);
    LFORate_Flt2Freq->AddValueString(VStr_fHz1);
    AddParam(LFOAmp_Flt1Freq);
    AddParam(LFORate_Flt1Freq);
    AddParam(LFOAmp_Flt2Freq);
    AddParam(LFORate_Flt2Freq);

    osc1FM2Level = new Parameter("Osc1.FM.Osc2", 0.0f, 0, 1.0f, Param_Default);
    osc1FM2Level->AddValueString(VStr_Percent);
    osc2FM3Level = new Parameter("Osc2.FM.Osc3", 0.0f, 0, 1.0f, Param_Default);
    osc2FM3Level->AddValueString(VStr_Percent);
    osc1FM3Level = new Parameter("Osc1.FM.Osc3", 0.0f, 0, 1.0f, Param_Default);
    osc1FM3Level->AddValueString(VStr_Percent);
    AddParam(osc1FM2Level);
    AddParam(osc2FM3Level);
    AddParam(osc1FM3Level);

    osc1RM2Level = new Parameter("Osc1.RM.Osc2", 0.0f, 0, 1.0f, Param_Default);
    osc1RM2Level->AddValueString(VStr_Percent);
    osc2RM3Level = new Parameter("Osc2.RM.Osc3", 0.0f, 0, 1.0f, Param_Default);
    osc2RM3Level->AddValueString(VStr_Percent);
    osc1RM3Level = new Parameter("Osc1.RM.Osc3", 0.0f, 0, 1.0f, Param_Default);
    osc1RM3Level->AddValueString(VStr_Percent);
    AddParam(osc1RM2Level);
    AddParam(osc2RM3Level);
    AddParam(osc1RM3Level);

    // Init filters
    for(int fc = 0; fc <= maxVoices; fc++)
    {
        filt1[fc] = new CFilter3(NULL, NULL);
        filt1[fc]->scope.instr = this;
        if(fc > 0)
        {
            filt1[fc - 1]->f_next = filt1[fc];
        }

        filt2[fc] = new CFilter3(NULL, NULL);
        filt2[fc]->scope.instr = this;
        if(fc > 0)
        {
            filt2[fc - 1]->f_next = filt2[fc];
        }

        if(fc == 0)
        {
            AddChild(filt1[fc]);
            AddChild(filt2[fc]);
        }
    }
    freeVoice = maxVoices;

    filt1[0]->f_master = true;
    filt2[0]->f_master = true;

    filt1[0]->SetName("Filter");
    filt2[0]->SetName("Filter2");

    // Init other FX
    chorus = new CChorus(NULL, NULL);
    chorus->scope.instr = this;
    delay = new XDelay(NULL, NULL);
    delay->scope.instr = this;
    delay->delay->SetInterval(0.25f);
    reverb = new CReverb(NULL, NULL);
    reverb->scope.instr = this;

    AddChild(chorus);
    AddChild(delay);
    AddChild(reverb);

    buff2 = buff3 = true;
    osc1fmulStatic = osc2fmulStatic = osc3fmulStatic = 1;

    synthWin = NULL;
    wnd = NULL;
    SynthComp = NULL;

    env1 = CreateEnvelope();
    env2 = CreateEnvelope();
    env3 = CreateEnvelope();
    env4 = CreateEnvelope();
    env5 = CreateEnvelope();

    envV1 = envV2 = envV3 = envV4 = envV5 = 1;

    p_osc1envVal = &envV1;
    p_osc2envVal = &envV2;
    p_osc3envVal = &envV3;

    p_flt1envVal = ebuff4;
    p_flt2envVal = ebuff5;

    rnd = new Random(0);

    Preset preset(NULL);
    strcpy(preset.name, "Square lead");
    strcpy(preset.path, ".\\Presets\\Chaotic\\Square lead.cxml");
    preset.native = true;
    File f(preset.path);
    if(f.exists())
    {
        SetPreset(&preset);
    }

    CreateModuleWindow();
}

Synth::~Synth()
{
}

Envelope* Synth::CreateEnvelope()
{
    Envelope* env = new Envelope(Cmd_ParamEnv);
    env->len = 3;
    env->timebased = true;

    env->AddPoint(0.0f, 1.0f);
    env->AddPoint(0.2f, 1.0f);
    env->AddPoint(0.3f, 0.5f);
    env->SetSustainPoint(env->AddPoint(0.6f, 0.0f));
    env->sustainable = true;

    return env;
}

void Synth::CreateModuleWindow()
{
    synthWin = MainWnd->CreateSynthWindow(this);
    wnd = synthWin;
    SynthComp = (SynthComponent*)synthWin->getContentComponent();
    SynthComp->UpdateEnvButts();
}

void Synth::Save(XmlElement * instrNode)
{
    Instrument::Save(instrNode);
    SaveStateData(*instrNode, "Current", true);
}

void Synth::Load(XmlElement * instrNode)
{
    Instrument::Load(instrNode);

    XmlElement* stateNode = instrNode->getChildByName(T("Module"));
    if(stateNode != NULL)
    {
        RestoreStateData(*stateNode, true);
    }
}

void Synth::ResetValues()
{
    osc1Fat->SetBoolValue(false);
    osc2Fat->SetBoolValue(false);
    osc3Fat->SetBoolValue(false);

    osc1Sine->SetBoolValue(true);
    osc1Saw->SetBoolValue(false);
    osc1Tri->SetBoolValue(false);
    osc1Pulse->SetBoolValue(false);
    osc1Noise->SetBoolValue(false);

    osc1Detune->SetNormalValue(0);
    osc1Octave->SetNormalValue(0);
    osc1Width->SetNormalValue(0.5f);

    osc2Sine->SetBoolValue(true);
    osc2Saw->SetBoolValue(false);
    osc2Tri->SetBoolValue(false);
    osc2Pulse->SetBoolValue(false);
    osc2Noise->SetBoolValue(false);

    osc2Detune->SetNormalValue(0);
    osc2Octave->SetNormalValue(0);
    osc2Width->SetNormalValue(0.5f);

    osc3Sine->SetBoolValue(true);
    osc3Saw->SetBoolValue(false);
    osc3Tri->SetBoolValue(false);
    osc3Pulse->SetBoolValue(false);
    osc3Noise->SetBoolValue(false);

    osc3Detune->SetNormalValue(0);
    osc3Octave->SetNormalValue(0);
    osc3Width->SetNormalValue(0.5f);

    osc1FM2Level->SetNormalValue(0);
    osc2FM3Level->SetNormalValue(0);
    osc1FM3Level->SetNormalValue(0);

    osc1RM2Level->SetNormalValue(0);
    osc2RM3Level->SetNormalValue(0);
    osc1RM3Level->SetNormalValue(0);

    LFOAmp_Vol1->SetNormalValue(0);
    LFOAmp_Vol2->SetNormalValue(0);
    LFOAmp_Vol3->SetNormalValue(0);
    LFORate_Vol1->SetNormalValue(3);
    LFORate_Vol2->SetNormalValue(3);
    LFORate_Vol3->SetNormalValue(3);

    LFOAmp_Pitch1->SetNormalValue(0);
    LFOAmp_Pitch2->SetNormalValue(0);
    LFOAmp_Pitch3->SetNormalValue(0);
    LFORate_Pitch1->SetNormalValue(3);
    LFORate_Pitch2->SetNormalValue(3);
    LFORate_Pitch3->SetNormalValue(3);

    LFOAmp_Flt1Freq->SetNormalValue(0);
    LFOAmp_Flt2Freq->SetNormalValue(0);
    LFORate_Flt1Freq->SetNormalValue(3);
    LFORate_Flt2Freq->SetNormalValue(3);

    filt1Osc1->SetBoolValue(true);
    filt1Osc2->SetBoolValue(false);
    filt1Osc3->SetBoolValue(false);

    filt2Osc1->SetBoolValue(false);
    filt2Osc2->SetBoolValue(false);
    filt2Osc3->SetBoolValue(false);

    filt1[0]->f1->SetBoolValue(true);
    filt1[0]->x2->SetBoolValue(false);
    filt1[0]->cutoff->SetNormalValue(1);
    filt1[0]->resonance->SetNormalValue(0);

    filt2[0]->f1->SetBoolValue(true);
    filt2[0]->x2->SetBoolValue(false);
    filt2[0]->cutoff->SetNormalValue(1);
    filt2[0]->resonance->SetNormalValue(0);

    chorusON->SetBoolValue(false);
    delayON->SetBoolValue(false);
    reverbON->SetBoolValue(false);

    chorus->drywet->Reset();
    chorus->depth->Reset();
    chorus->delay->Reset();

    delay->ggain->Reset();
    delay->feedback->Reset();
    delay->delay->Reset();

    reverb->drywet->Reset();
    reverb->decay->Reset();
    reverb->lowCut->Reset();

    p_osc1envVal = &envV1;
    p_osc2envVal = &envV2;
    p_osc3envVal = &envV3;

    p_flt1envVal = ebuff4;
    p_flt2envVal = ebuff5;

    if(SynthComp != NULL)
    {
        SynthComp->env1->env->Clean();
        SynthComp->env1->env->SetSustainPoint(SynthComp->env1->env->AddPoint(0, 1));
        SynthComp->env2->env->Clean();
        SynthComp->env2->env->SetSustainPoint(SynthComp->env2->env->AddPoint(0, 1));
        SynthComp->env3->env->Clean();
        SynthComp->env3->env->SetSustainPoint(SynthComp->env3->env->AddPoint(0, 1));
        SynthComp->env4->env->Clean();
        SynthComp->env4->env->SetSustainPoint(SynthComp->env4->env->AddPoint(0, 1));
        SynthComp->env5->env->Clean();
        SynthComp->env5->env->SetSustainPoint(SynthComp->env5->env->AddPoint(0, 1));

        SynthComp->UpdateEnvButts();

        SynthComp->repaint();
    }
}

void Synth::ActivateTrigger(Trigger* tg)
{
    int vnum = 0;
    if(numUsedVoices == maxVoices)
    {
		vnum = tg_first->voicenum;
        if(tg_first->frame_phase > 0)
        {
            FlowTriggers(tg_first, tg);
        }
        else if(tg_first->lcount > 0)
		{
            memcpy(tg->auxbuff, tg_first->auxbuff, (int)(tg_first->lcount*sizeof(float)*2));
			tg->lcount = tg_first->lcount;
			tg_first->lcount = 0;
		}

		if(tg_first != NULL)
            tg_first->Deactivate();
    }
    else
    {
        // Check and loop until first unused number is found
    	Trigger* tgc = tg_first;
    	while(tgc != NULL)
    	{
    		if(tgc->voicenum == vnum)
    		{
    			vnum++;
                tgc = tg_first;
                continue;
    		}
    		tgc = tgc->loc_act_next;
    	}
    }
    tg->voicenum = vnum;
    numUsedVoices++;
    jassert(numUsedVoices <= maxVoices + 1);
    Gen::ActivateTrigger(tg);

    tg->sec_phase1 = 0;
    tg->sec_phase2 = 0;
    tg->sec_phase3 = 0;

    tg->aaCount = DECLICK_COUNT;
    tg->aaIN = true;

    tg->ep1 = env1->p_first;
    tg->ep2 = env2->p_first;
    tg->ep3 = env3->p_first;
    tg->ep4 = env4->p_first;
    tg->ep5 = env5->p_first;
    tg->envVal1 = tg->ep1->y_norm;
    tg->envVal2 = tg->ep2->y_norm;
    tg->envVal3 = tg->ep3->y_norm;
    tg->envVal4 = tg->ep4->y_norm;
    tg->envVal5 = tg->ep5->y_norm;

    tg->env_phase1 = 0;
    tg->env_phase2 = 0;
    tg->env_phase3 = 0;
    tg->env_phase4 = 0;
    tg->env_phase5 = 0;

    //filt1[vnum]->Reset();
    //filt2[vnum]->Reset();
}

void Synth::DeactivateTrigger(Trigger* tg)
{
    // Reset on deactivate causes aliasing in filters
    //filt1[tg->voicenum]->Reset();
    //filt2[tg->voicenum]->Reset();
    numUsedVoices--;
    jassert(numUsedVoices >= 0);
    Instrument::DeactivateTrigger(tg);
}

long Synth::ProcessTrigger(Trigger * tg, long num_frames, long buffframe)
{
    fill = true;
    skip = false;

    tg->freq_incr_active = tg->freq;
    tg->vol_val = tg->vol_base;
    tg->pan_val = tg->pan_base;
    PreProcessTrigger(tg, &skip, &fill, num_frames, buffframe);

    if(!skip)
    {
        memset(fltbuff1, 0, num_frames*(sizeof(float))*2);
        memset(fltbuff2, 0, num_frames*(sizeof(float))*2);

        //memset(fltbuff1_out, 0, num_frames*(sizeof(float))*2);
        //memset(fltbuff2_out, 0, num_frames*(sizeof(float))*2);
        long tcf0 = 0;
        long tc0 = buffframe*2;
        bool f1work = filt1Osc1->outval || filt1Osc2->outval || filt1Osc3->outval;
        bool f2work = filt2Osc1->outval || filt2Osc2->outval || filt2Osc3->outval;
        //bool o1work = filt1Osc1->outval || filt2Osc1->outval;
        //bool o2work = filt1Osc2->outval || filt2Osc2->outval;
        //bool o3work = filt1Osc3->outval || filt2Osc3->outval;
        double inOutL1, inOutL2, inOutR1, inOutR2;
        inOutL1 = inOutL2 = inOutR1 = inOutR2 = 0;
        double f1freqnormval = filt1[0]->cutoff->val;
        double f2freqnormval = filt2[0]->cutoff->val;
        long ic = 0;
        int fqc = 0;

        int bc = buffframe*2;
        Gennote* gnote = (Gennote*)tg->el;
        float d1, d2, d3, v1, v2, v3;
        int vnum = tg->voicenum;
        //if(fill)
        {
            for(int ic = 0; ic < num_frames; ic++)
            {
                envV1 = tg->envVal1;
                envV2 = tg->envVal2;
                envV3 = tg->envVal3;
                envV4 = tg->envVal4;
                envV5 = tg->envVal5;
                d1 = 0;
                if(osc1Tri->outval)
                {
                    d1 = wt_triangle[int(tg->sec_phase1*WT_SIZE)&0x7FFF];
                    if(osc1Fat->outval == true)
                    {
                        double f1 = tg->sec_phase1 + 1.0/6.0;
                        if(f1 >= 1)
                            f1 -= 1;
                        double f2 = tg->sec_phase1 + 1.0/3.0;
                        if(f2 >= 1)
                            f2 -= 1;

                        d1 += wt_triangle[int(f1*WT_SIZE)&0x7FFF];
                        d1 += wt_triangle[int(f2*WT_SIZE)&0x7FFF];
                    }
                }
                else if(osc1Sine->outval)
                {
                    d1 = wt_sine[int(tg->sec_phase1*WT_SIZE)&0x7FFF];
                    if(osc1Fat->outval == true)
                    {
                        double f1 = tg->sec_phase1 + 1.0/6.0;
                        if(f1 >= 1)
                            f1 -= 1;
                        double f2 = tg->sec_phase1 + 1.0/3.0;
                        if(f2 >= 1)
                            f2 -= 1;

                        d1 += wt_sine[int(f1*WT_SIZE)&0x7FFF];
                        d1 += wt_sine[int(f2*WT_SIZE)&0x7FFF];
                    }
                }
                else if(osc1Saw->outval)
                {
                    d1 = float(tg->sec_phase1*2 - 1);
                    if(osc1Fat->outval == true)
                    {
                        double f1 = tg->sec_phase1 + 1.0/6.0;
                        if(f1 >= 1)
                            f1 -= 1;
                        double f2 = tg->sec_phase1 + 1.0/3.0;
                        if(f2 >= 1)
                            f2 -= 1;
                        d1 += float(f1*2 - 1);
                        d1 += float(f2*2 - 1);
                    }
                }
                else if(osc1Pulse->outval)
                {
                    d1 = tg->sec_phase1 < osc1Width->outval ? -1.f : 1.f;
                    if(osc1Fat->outval == true)
                    {
                        double f1 = tg->sec_phase1 + 1.0/6.0;
                        if(f1 >= 1)
                            f1 -= 1;
                        double f2 = tg->sec_phase1 + 1.0/3.0;
                        if(f2 >= 1)
                            f2 -= 1;
                        d1 += f1 < osc1Width->outval ? -1 : 1;
                        d1 += f2 < osc1Width->outval ? -1 : 1;
                    }
                }
                else if(osc1Noise->outval)
                {
                    d1 = rnd->nextFloat()*2 -1;
                }

                v1 = osc1Level->outval;

                v1 *= 1 - LFOAmp_Vol1->outval*(0.5f*(1 + wt_sine[int(LFORate_Vol1->outval*(tg->sec_phase)*WT_SIZE)&0x7FFF]));
                v1 *= *p_osc1envVal;

                data_buff[bc] = data_buff[bc + 1] = d1*(v1*v1 + (1 - pow((1 - v1), 0.666666666f)))*0.5f;
                osc1fmulLFO = 1 + LFOAmp_Pitch1->outval*wt_sine[int(tg->sec_phase*LFORate_Pitch1->outval*WT_SIZE)&0x7FFF];

                tg->sec_phase1 += one_divided_per_sample_rate*tg->freq_incr_active*osc1fmulStatic*osc1fmulLFO;
                if(tg->sec_phase1 >= 1)
                {
                    tg->sec_phase1 -= 1;
                }

                //////////
                // 2nd
                osc2fmulFM = osc1FM2Level->outval*d1*v1;

                d2 = 0;
                if(osc2Tri->outval)
                {
                    d2 = wt_triangle[int((tg->sec_phase2 + osc2fmulFM)*WT_SIZE)&0x7FFF];
                    if(osc2Fat->outval == true)
                    {
                        double f1 = (tg->sec_phase2 + osc2fmulFM) + 1.0/6.0;
                        if(f1 >= 1)
                            f1 -= 1;
                        double f2 = (tg->sec_phase2 + osc2fmulFM) + 1.0/3.0;
                        if(f2 >= 1)
                            f2 -= 1;

                        d2 += wt_triangle[int(f1*WT_SIZE)&0x7FFF];
                        d2 += wt_triangle[int(f2*WT_SIZE)&0x7FFF];
                    }
                }
                else if(osc2Sine->outval)
                {
                    //d1 = sin(tg->sec_phase2*2.0f*PI*osc2fmulStatic*osc2fmuDynamic);
                    //d1 = sin(tg->sec_phase2*2.0f*PI);
                    d2 = wt_sine[int((tg->sec_phase2 + osc2fmulFM)*WT_SIZE)&0x7FFF];
                    if(osc2Fat->outval == true)
                    {
                        double f1 = (tg->sec_phase2 + osc2fmulFM) + 1.0/6.0;
                        if(f1 >= 1)
                            f1 -= 1;
                        double f2 = (tg->sec_phase2 + osc2fmulFM) + 1.0/3.0;
                        if(f2 >= 1)
                            f2 -= 1;

                        d2 += wt_sine[int(f1*WT_SIZE)&0x7FFF];
                        d2 += wt_sine[int(f2*WT_SIZE)&0x7FFF];
                    }
                }
                else if(osc2Saw->outval)
                {
                    d2 = float(tg->sec_phase2 + osc2fmulFM)*2 - 1;
                    if(osc2Fat->outval == true)
                    {
                        double f1 = (tg->sec_phase2 + osc2fmulFM) + 1.0/6.0;
                        if(f1 >= 1)
                            f1 -= 1;
                        double f2 = (tg->sec_phase2 + osc2fmulFM) + 1.0/3.0;
                        if(f2 >= 1)
                            f2 -= 1;
                        d2 += float(f1*2 - 1);
                        d2 += float(f2*2 - 1);
                    }
                }
                else if(osc2Pulse->outval)
                {
                    d2 = (tg->sec_phase2 + osc2fmulFM) < osc2Width->outval ? -1.f : 1.f;
                    if(osc2Fat->outval == true)
                    {
                        double f1 = (tg->sec_phase2 + osc2fmulFM) + 1.0/6.0;
                        if(f1 >= 1)
                            f1 -= 1;
                        double f2 = (tg->sec_phase2 + osc2fmulFM) + 1.0/3.0;
                        if(f2 >= 1)
                            f2 -= 1;
                        d2 += f1 < osc2Width->outval ? -1 : 1;
                        d2 += f2 < osc2Width->outval ? -1 : 1;
                    }
                }
                else if(osc2Noise->outval)
                {
                    d2 = rnd->nextFloat()*2 - 1;
                }

                // Apply RM
                d2 *= 1.f - osc1RM2Level->outval*(1.f - d1*v1);
                // Apply Level
                v2 = osc2Level->outval;
                v2 *= 1 - LFOAmp_Vol2->outval*(0.5f*(1 + wt_sine[int(LFORate_Vol2->outval*(tg->sec_phase)*WT_SIZE)&0x7FFF]));
                v2 *= *p_osc2envVal;
                data_buff2[bc] = data_buff2[bc + 1] = d2*(v2*v2 + (1 - pow((1 - v2), 1.f/1.5f)))*0.5f;
                // Increase phase
                osc2fmulLFO = 1 + LFOAmp_Pitch2->outval*wt_sine[int(tg->sec_phase*LFORate_Pitch2->outval*WT_SIZE)&0x7FFF];
                tg->sec_phase2 += one_divided_per_sample_rate*tg->freq_incr_active*osc2fmulStatic*osc2fmulLFO;
                if(tg->sec_phase2 >= 1)
                {
                    tg->sec_phase2 -= 1;
                }

                //////////
                // 3rd
                osc3fmulFM = osc2FM3Level->outval*d2*v2;
                osc3fmulFM += osc1FM3Level->outval*d1*v1;

                d3 = 0;
                if(osc3Tri->outval)
                {
                    d3 = wt_triangle[int((tg->sec_phase3 + osc3fmulFM)*WT_SIZE)&0x7FFF];
                    if(osc3Fat->outval == true)
                    {
                        double f1 = (tg->sec_phase3 + osc3fmulFM) + 1.0/6.0;
                        if(f1 >= 1)
                            f1 -= 1;
                        double f2 = (tg->sec_phase3 + osc3fmulFM) + 1.0/3.0;
                        if(f2 >= 1)
                            f2 -= 1;

                        d3 += wt_triangle[int(f1*WT_SIZE)&0x7FFF];
                        d3 += wt_triangle[int(f2*WT_SIZE)&0x7FFF];
                    }
                }
                else if(osc3Sine->outval)
                {
                    //d1 = sin(tg->sec_phase2*2.0f*PI*osc2fmulStatic*osc2fmuDynamic);
                    //d1 = sin(tg->sec_phase2*2.0f*PI);
                    d3 = wt_sine[int((tg->sec_phase3 + osc3fmulFM)*WT_SIZE)&0x7FFF];
                    if(osc3Fat->outval == true)
                    {
                        double f1 = (tg->sec_phase3 + osc3fmulFM) + 1.0/6.0;
                        if(f1 >= 1)
                            f1 -= 1;
                        double f2 = (tg->sec_phase3 + osc3fmulFM) + 1.0/3.0;
                        if(f2 >= 1)
                            f2 -= 1;

                        d3 += wt_sine[int(f1*WT_SIZE)&0x7FFF];
                        d3 += wt_sine[int(f2*WT_SIZE)&0x7FFF];
                    }
                }
                else if(osc3Saw->outval)
                {
                    d3 = float(tg->sec_phase3 + osc3fmulFM)*2 - 1;
                    if(osc3Fat->outval == true)
                    {
                        double f1 = (tg->sec_phase3 + osc3fmulFM) + 1.0/6.0;
                        if(f1 >= 1)
                            f1 -= 1;
                        double f2 = (tg->sec_phase3 + osc3fmulFM) + 1.0/3.0;
                        if(f2 >= 1)
                            f2 -= 1;
                        d3 += float(f1*2 - 1);
                        d3 += float(f2*2 - 1);
                    }
                }
                else if(osc3Pulse->outval)
                {
                    d3 = (tg->sec_phase3 + osc3fmulFM) < osc3Width->outval ? -1.f : 1.f;
                    if(osc2Fat->outval == true)
                    {
                        double f1 = (tg->sec_phase3 + osc3fmulFM) + 1.0/6.0;
                        if(f1 >= 1)
                            f1 -= 1;
                        double f2 = (tg->sec_phase3 + osc3fmulFM) + 1.0/3.0;
                        if(f2 >= 1)
                            f2 -= 1;
                        d3 += f1 < osc3Width->outval ? -1 : 1;
                        d3 += f2 < osc3Width->outval ? -1 : 1;
                    }
                }
                else if(osc3Noise->outval)
                {
                    d3 = rnd->nextFloat()*2 - 1;
                }

                d3 *= 1 - osc1RM3Level->outval*(1 - d1*v1);
                d3 *= 1 - osc2RM3Level->outval*(1 - d2*v2);

                v3 = osc3Level->outval;
                v3 *= 1 - LFOAmp_Vol3->outval*(0.5f*(1 + wt_sine[int(LFORate_Vol3->outval*(tg->sec_phase)*WT_SIZE)&0x7FFF]));
                v3 *= *p_osc3envVal;
                data_buff3[bc] = data_buff3[bc + 1] = d3*(v3*v3 + (1 - pow((1 - v3), 0.666666666f)))*0.5f;
                osc3fmulLFO = 1;
                osc3fmulLFO = 1 + LFOAmp_Pitch3->outval*wt_sine[int(tg->sec_phase*LFORate_Pitch3->outval*WT_SIZE)&0x7FFF];
                tg->sec_phase3 += one_divided_per_sample_rate*tg->freq_incr_active*osc3fmulStatic*osc3fmulLFO;
                if(tg->sec_phase3 >= 1)
                {
                    tg->sec_phase3 -= 1;
                }

                bc += 2;
                tg->wt_pos += tg->freq_incr_active;

                tg->sec_phase += one_divided_per_sample_rate;

                if(tg->tgstate == TgState_Sustain && tg->ep1->suspoint)
                {
                    tg->env_phase1 = tg->ep1->x;
                    tg->envVal1 = tg->ep1->y_norm;
                }
                else
                {
                    tg->env_phase1 += one_divided_per_sample_rate;
                    while(tg->ep1->next != NULL && (tg->env_phase1 >= tg->ep1->next->x || tg->ep1->next->x == tg->ep1->x))
                    {
                        tg->ep1 = tg->ep1->next;
                    }
                    if(tg->ep1->next != NULL)
                    {
                        tg->envVal1 += tg->ep1->cff;
						if(tg->envVal1 > 1)
							tg->envVal1 = 1;
						else if(tg->envVal1 < 0)
							tg->envVal1 = 0;
                        //tg->envVal1 = (float)(tg->ep1->y_norm + (tg->env_phase1 - tg->ep1->x)*((tg->ep1->next->y_norm - tg->ep1->y_norm)/(tg->ep1->next->x - tg->ep1->x)));
                    }
                }
                ebuff1[buffframe + ic] = tg->envVal1;

                if(tg->tgstate == TgState_Sustain && tg->ep2->suspoint)
                {
                    tg->env_phase2 = tg->ep2->x;
                    tg->envVal2 = tg->ep2->y_norm;
                }
                else
                {
                    tg->env_phase2 += one_divided_per_sample_rate;
                    while(tg->ep2->next != NULL && (tg->env_phase2 >= tg->ep2->next->x || tg->ep2->next->x == tg->ep2->x))
                    {
                        tg->ep2 = tg->ep2->next;
                    }
                    if(tg->ep2->next != NULL)
                    {
                        tg->envVal2 += tg->ep2->cff;
						if(tg->envVal2 > 1)
							tg->envVal2 = 1;
						else if(tg->envVal2 < 0)
							tg->envVal2 = 0;
                        //tg->envVal2 = (float)(tg->ep2->y_norm + (tg->env_phase2 - tg->ep2->x)*((tg->ep2->next->y_norm - tg->ep2->y_norm)/(tg->ep2->next->x - tg->ep2->x)));
                    }
                }
                ebuff2[buffframe + ic] = tg->envVal2;

                if(tg->tgstate == TgState_Sustain && tg->ep3->suspoint)
                {
                    tg->env_phase3 = tg->ep3->x;
                    tg->envVal3 = tg->ep3->y_norm;
                }
                else
                {
                    tg->env_phase3 += one_divided_per_sample_rate;
                    while(tg->ep3->next != NULL && (tg->env_phase3 >= tg->ep3->next->x || tg->ep3->next->x == tg->ep3->x))
                    {
                        tg->ep3 = tg->ep3->next;
                    }
                    if(tg->ep3->next != NULL)
                    {
                        tg->envVal3 += tg->ep3->cff;
						if(tg->envVal3 > 1)
							tg->envVal3 = 1;
						else if(tg->envVal3 < 0)
							tg->envVal3 = 0;
                        //tg->envVal3 = (float)(tg->ep3->y_norm + (tg->env_phase3 - tg->ep3->x)*((tg->ep3->next->y_norm - tg->ep3->y_norm)/(tg->ep3->next->x - tg->ep3->x)));
                    }
                }
                ebuff3[buffframe + ic] = tg->envVal3;

                if(tg->tgstate == TgState_Sustain && tg->ep4->suspoint)
                {
                    tg->env_phase4 = tg->ep4->x;
                    tg->envVal4 = tg->ep4->y_norm;
                }
                else
                {
                    tg->env_phase4 += one_divided_per_sample_rate;
                    while(tg->ep4->next != NULL && (tg->env_phase4 >= tg->ep4->next->x || tg->ep4->next->x == tg->ep4->x))
                    {
                        tg->ep4 = tg->ep4->next;
                    }
                    if(tg->ep4->next != NULL)
                    {
                        tg->envVal4 += tg->ep4->cff;
						if(tg->envVal4 > 1)
							tg->envVal4 = 1;
						else if(tg->envVal4 < 0)
							tg->envVal4 = 0;
                        //tg->envVal4 = (float)(tg->ep4->y_norm + (tg->env_phase4 - tg->ep4->x)*((tg->ep4->next->y_norm - tg->ep4->y_norm)/(tg->ep4->next->x - tg->ep4->x)));
                    }
                }
                ebuff4[buffframe + ic] = tg->envVal4;

                if(tg->tgstate == TgState_Sustain && tg->ep5->suspoint)
                {
                    tg->env_phase5 = tg->ep5->x;
                    tg->envVal5 = tg->ep5->y_norm;
                }
                else
                {
                    tg->env_phase5 += one_divided_per_sample_rate;
                    while(tg->ep5->next != NULL && (tg->env_phase5 >= tg->ep5->next->x || tg->ep5->next->x == tg->ep5->x))
                    {
                        tg->ep5 = tg->ep5->next;
                    }
                    if(tg->ep5->next != NULL)
                    {
                        tg->envVal5 += tg->ep5->cff;
						if(tg->envVal5 > 1)
							tg->envVal5 = 1;
						else if(tg->envVal5 < 0)
							tg->envVal5 = 0;
                        //tg->envVal5 = (float)(tg->ep5->y_norm + (tg->env_phase5 - tg->ep5->x)*((tg->ep5->next->y_norm - tg->ep5->y_norm)/(tg->ep5->next->x - tg->ep5->x)));
                    }
                }
                ebuff5[buffframe + ic] = tg->envVal5;


                if(f1work)
                {
                    fltlfo1[buffframe + ic] = 1 + LFOAmp_Flt1Freq->outval*(wt_sine[int(LFORate_Flt1Freq->outval*(tg->sec_phase)*WT_SIZE)&0x7FFF]);

                    if(filt1Osc1->outval)
                    {
                        fltbuff1[tcf0] += data_buff[tc0];
                        fltbuff1[tcf0 + 1] += data_buff[tc0 + 1];
                    }

                    if(filt1Osc2->outval)
                    {
                        fltbuff1[tcf0] += data_buff2[tc0];
                        fltbuff1[tcf0 + 1] += data_buff2[tc0 + 1];
                    }

                    if(filt1Osc3->outval)
                    {
                        fltbuff1[tcf0] += data_buff3[tc0];
                        fltbuff1[tcf0 + 1] += data_buff3[tc0 + 1];
                    }

                    if(fqc == 0)
                    {
                        //flt1eV[buffframe + ic] = 1;  // debug stuff
                        filt1[vnum]->dspCoreCFilter3.setCutoff((20.0*pow(1000.0, (double)f1freqnormval*p_flt1envVal[buffframe + ic]*fltlfo1[buffframe + ic])));
                    }
                    inOutL1 = (double)fltbuff1[tcf0];
                    inOutR1 = (double)fltbuff1[tcf0 + 1];
                    filt1[vnum]->dspCoreCFilter3.getSampleFrameStereo(&inOutL1, &inOutR1);
                }

                if(f2work)
                {
                    fltlfo2[buffframe + ic] = 1 + LFOAmp_Flt2Freq->outval*(wt_sine[int(LFORate_Flt2Freq->outval*(tg->sec_phase)*WT_SIZE)&0x7FFF]);

                    if(filt2Osc1->outval)
                    {
                        fltbuff2[tcf0] += data_buff[tc0];
                        fltbuff2[tcf0 + 1] += data_buff[tc0 + 1];
                    }

                    if(filt2Osc2->outval)
                    {
                        fltbuff2[tcf0] += data_buff2[tc0];
                        fltbuff2[tcf0 + 1] += data_buff2[tc0 + 1];
                    }

                    if(filt2Osc3->outval)
                    {
                        fltbuff2[tcf0] += data_buff3[tc0];
                        fltbuff2[tcf0 + 1] += data_buff3[tc0 + 1];
                    }

                    if(fqc == 0)
                    {
                        //flt2eV[buffframe + ic] = 1;  // debug stuff
                        filt2[vnum]->dspCoreCFilter3.setCutoff((20.0*pow(1000.0, (double)f2freqnormval*p_flt2envVal[buffframe + ic]*fltlfo2[buffframe + ic])));
                    }
                    inOutL2 = (double)fltbuff2[tcf0];
                    inOutR2 = (double)fltbuff2[tcf0 + 1];
                    filt2[vnum]->dspCoreCFilter3.getSampleFrameStereo(&inOutL2, &inOutR2);
                }

				if(fill)
				{
					in_buff[tc0] = (float)inOutL1 + (float)inOutL2;
					in_buff[tc0 + 1] = (float)inOutR1 + (float)inOutR2;
				}
				else
				{
					in_buff[tc0] = 0;
					in_buff[tc0 + 1] = 0;
				}

                tc0++;
                tc0++;

                tcf0++;
                tcf0++;

                fqc++;
                if(fqc == 77)
                    fqc = 0;

                //if(tg->lcount > 0)
                //    tg->lcount--;
            }
        }
        /*
        else
        {
            tg->wt_pos += tg->freq_incr_active*num_frames;
            //while(tg->wt_pos >= 1.0f)
            //{
            //    tg->wt_pos -= 1.0f;
            //}
            tg->sec_phase += one_divided_per_sample_rate*num_frames;
            //while(tg->sec_phase >= 1.0f)
            //{
            //    tg->sec_phase -= 1.0f;
            //}
        }
        */

        tg->frame_phase += num_frames;
        CheckBounds(gnote, tg, num_frames);
    }

    return num_frames;
}
void Synth::FillMixChannel(long num_frames, long buffframe, long mixbuffframe)
{
    if(chorusON->outval == true)
    {
        chorus->ProcessData(&out_buff[buffframe*2], &out_buff[buffframe*2], num_frames);
    }

    if(delayON->outval == true)
    {
        delay->ProcessData(&out_buff[buffframe*2], &out_buff[buffframe*2], num_frames);
    }

    if(reverbON->outval == true)
    {
        reverb->ProcessData(&out_buff[buffframe*2], &out_buff[buffframe*2], num_frames);
    }

    Instrument::FillMixChannel(num_frames, buffframe, mixbuffframe);
}

// data_buff >> in_buff
void Synth::ApplyDSP(Trigger* tg, long buffframe, long num_frames)
{
    memset(fltbuff1, 0, num_frames*(sizeof(float))*2);
    memset(fltbuff2, 0, num_frames*(sizeof(float))*2);
    memset(fltbuff1_out, 0, num_frames*(sizeof(float))*2);
    memset(fltbuff2_out, 0, num_frames*(sizeof(float))*2);

    long tcf0 = 0;
    long tc0 = buffframe*2;
    bool f1work = filt1Osc1->outval || filt1Osc2->outval || filt1Osc3->outval;
    bool f2work = filt2Osc1->outval || filt2Osc2->outval || filt2Osc3->outval;
    double inOutL;
    double inOutR;
    double f1freqnormval = filt1[0]->cutoff->val;
    double f2freqnormval = filt2[0]->cutoff->val;
    long ic = 0;
    int fqc = 0;
    for(long cc = 0; cc < num_frames; cc++)
    {
        if(filt1Osc1->outval)
        {
            fltbuff1[tcf0] += data_buff[tc0];
            fltbuff1[tcf0 + 1] += data_buff[tc0 + 1];
        }

        if(filt1Osc2->outval)
        {
            fltbuff1[tcf0] += data_buff2[tc0];
            fltbuff1[tcf0 + 1] += data_buff2[tc0 + 1];
        }

        if(filt1Osc3->outval)
        {
            fltbuff1[tcf0] += data_buff3[tc0];
            fltbuff1[tcf0 + 1] += data_buff3[tc0 + 1];
        }

        if(f1work)
        {
            if(fqc == 0)
                filt1[tg->voicenum]->dspCoreCFilter3.setCutoff((20.0*pow(1000.0, (double)f1freqnormval*p_flt1envVal[buffframe + cc]*fltlfo1[buffframe + cc])));
            inOutL = (double)fltbuff1[tcf0];
            inOutR = (double)fltbuff1[tcf0 + 1];
            filt1[tg->voicenum]->dspCoreCFilter3.getSampleFrameStereo(&inOutL, &inOutR);
            fltbuff1_out[tcf0] = (float)inOutL;
            fltbuff1_out[tcf0 + 1] = (float)inOutR;

            in_buff[tc0] = fltbuff1_out[tcf0];
            in_buff[tc0 + 1] = fltbuff1_out[tcf0 + 1];
        }

        if(filt2Osc1->outval)
        {
            fltbuff2[tcf0] += data_buff[tc0];
            fltbuff2[tcf0 + 1] += data_buff[tc0 + 1];
        }

        if(filt2Osc2->outval)
        {
            fltbuff2[tcf0] += data_buff2[tc0];
            fltbuff2[tcf0 + 1] += data_buff2[tc0 + 1];
        }

        if(filt2Osc3->outval)
        {
            fltbuff2[tcf0] += data_buff3[tc0];
            fltbuff2[tcf0 + 1] += data_buff3[tc0 + 1];
        }

        if(f2work)
        {
            if(fqc == 0)
                filt2[tg->voicenum]->dspCoreCFilter3.setCutoff((20.0*pow(1000.0, (double)f2freqnormval*p_flt2envVal[buffframe + cc]*fltlfo2[buffframe + cc])));
            inOutL = (double)fltbuff2[tcf0];
            inOutR = (double)fltbuff2[tcf0 + 1];
            filt2[tg->voicenum]->dspCoreCFilter3.getSampleFrameStereo(&inOutL, &inOutR);
            fltbuff2_out[tcf0] = (float)inOutL;
            fltbuff2_out[tcf0 + 1] = (float)inOutR;

            in_buff[tc0] += fltbuff2_out[tcf0];
            in_buff[tc0 + 1] += fltbuff2_out[tcf0 + 1];
        }

        tc0++;
        tc0++;

        tcf0++;
        tcf0++;
    }
}

void Synth::UpdateOsc1Mul()
{
    osc1fmulStatic = pow(2, osc1Octave->outval)*(1 + osc1Detune->outval);
}

void Synth::UpdateOsc2Mul()
{
    osc2fmulStatic = pow(2, osc2Octave->outval)*(1 + osc2Detune->outval);
}

void Synth::UpdateOsc3Mul()
{
    osc3fmulStatic = pow(2, osc3Octave->outval)*(1 + osc3Detune->outval);
}

bool Synth::IsTriggerAtTheEnd(Trigger* tg)
{
    // Check all three oscillators whether they are sounding and still under envelope. If not, then finish.
    bool attheend = true;
    if(filt1Osc1->outval || filt2Osc1->outval)
    {
        if((p_osc1envVal == &envV1 && tg->ep1->next != NULL) ||
           (p_osc1envVal == &envV2 && tg->ep2->next != NULL) || 
           (p_osc1envVal == &envV3 && tg->ep3->next != NULL) ||
           (p_osc1envVal == &envV4 && tg->ep4->next != NULL) ||
           (p_osc1envVal == &envV5 && tg->ep5->next != NULL))
        {
            attheend = false;
        }
    }

    if(attheend && (filt1Osc2->outval || filt2Osc2->outval))
    {
        if((p_osc2envVal == &envV1 && tg->ep1->next != NULL) ||
           (p_osc2envVal == &envV2 && tg->ep2->next != NULL) || 
           (p_osc2envVal == &envV3 && tg->ep3->next != NULL) ||
           (p_osc2envVal == &envV4 && tg->ep4->next != NULL) ||
           (p_osc2envVal == &envV5 && tg->ep5->next != NULL))
        {
            attheend = false;
        }
    }

    if(attheend && (filt1Osc3->outval || filt2Osc3->outval))
    {
        if((p_osc3envVal == &envV1 && tg->ep1->next != NULL) ||
           (p_osc3envVal == &envV2 && tg->ep2->next != NULL) || 
           (p_osc3envVal == &envV3 && tg->ep3->next != NULL) ||
           (p_osc3envVal == &envV4 && tg->ep4->next != NULL) ||
           (p_osc3envVal == &envV5 && tg->ep5->next != NULL))
        {
            attheend = false;
        }
    }

    return attheend;
}

void Synth::CheckBounds(Gennote* gnote, Trigger* tg, long num_frames)
{
	int vnum = tg->voicenum;
	if(tg->tgstate == TgState_Sustain && gnote->preview == false && tg->frame_phase >= gnote->frame_length)
    {
        // Go to release state as note is over
        tg->Release();
    }

    // Finishing can happen both when envelope is out of bounds and when the note length is longer than envelope 
    // length and the note is finished (frame phase exceeds note frame length)
	if(tg->tgstate == TgState_Release)
    {

        if(IsTriggerAtTheEnd(tg))
        {
            tg->tgstate = TgState_SoftFinish;
            //if(end_frame < 0)
            {
                end_frame = 0;
            }
        }
    }
}

void Synth::ParamUpdate(Parameter* param)
{
    //if(!env_update)
    //{
    //    param->SetDirectValueFromControl((float)aslider->getValue(), true);
    //}

    if(param == delayON)
    {
        //if(delayON->outval == false)
        {
            delay->Reset();
        }
    }
    else if(param == reverbON)
    {
        //if(reverbON->outval == false)
        {
            reverb->Reset();
        }
    }
    else if(param == osc1Level)
    {
        osc1Level->vstring->SetValue(int(osc1Level->outval*100));
    }
    else if(param == osc1Octave)
    {
        osc1Octave->vstring->SetValue(int(osc1Octave->outval));
        UpdateOsc1Mul();
    }
    else if(param == osc1Detune)
    {
        osc1Detune->vstring->SetValue(int(osc1Detune->outval/0.029731547f*100));
        UpdateOsc1Mul();
    }
    else if(param == osc2Level)
    {
        osc2Level->vstring->SetValue(int(osc2Level->outval*100));
    }
    else if(param == osc2Octave)
    {
        osc2Octave->vstring->SetValue(int(osc2Octave->outval));
        UpdateOsc2Mul();
    }
    else if(param == osc2Detune)
    {
        osc2Detune->vstring->SetValue(int(osc2Detune->outval/0.029731547f*100));
        UpdateOsc2Mul();
    }
    else if(param == osc3Level)
    {
        osc3Level->vstring->SetValue(int(osc3Level->outval*100));
    }
    else if(param == osc3Octave)
    {
        osc3Octave->vstring->SetValue(int(osc3Octave->outval));
        UpdateOsc3Mul();
    }
    else if(param == osc3Detune)
    {
        osc3Detune->vstring->SetValue(int(osc3Detune->outval/0.029731547f*100));
        UpdateOsc3Mul();
    }
    else if(param == LFOAmp_Vol1)
    {
        LFOAmp_Vol1->vstring->SetValue(int(LFOAmp_Vol1->outval*100));
    }
    else if(param == LFOAmp_Vol2)
    {
        LFOAmp_Vol2->vstring->SetValue(int(LFOAmp_Vol2->outval*100));
    }
    else if(param == LFOAmp_Vol3)
    {
        LFOAmp_Vol3->vstring->SetValue(int(LFOAmp_Vol3->outval*100));
    }
    else if(param == LFOAmp_Pitch1)
    {
        LFOAmp_Pitch1->vstring->SetValue(int(LFOAmp_Pitch1->outval/0.059463094f*100));
    }
    else if(param == LFOAmp_Pitch2)
    {
        LFOAmp_Pitch2->vstring->SetValue(int(LFOAmp_Pitch2->outval/0.059463094f*100));
    }
    else if(param == LFOAmp_Pitch3)
    {
        LFOAmp_Pitch3->vstring->SetValue(int(LFOAmp_Pitch3->outval/0.059463094f*100));
    }
    else if(param == LFORate_Vol1)
    {
        LFORate_Vol1->vstring->SetValue(LFORate_Vol1->outval);
    }
    else if(param == LFORate_Vol2)
    {
        LFORate_Vol2->vstring->SetValue(LFORate_Vol2->outval);
    }
    else if(param == LFORate_Vol3)
    {
        LFORate_Vol3->vstring->SetValue(LFORate_Vol3->outval);
    }
    else if(param == LFORate_Pitch1)
    {
        LFORate_Pitch1->vstring->SetValue(LFORate_Pitch1->outval);
    }
    else if(param == LFORate_Pitch2)
    {
        LFORate_Pitch2->vstring->SetValue(LFORate_Pitch2->outval);
    }
    else if(param == LFORate_Pitch3)
    {
        LFORate_Pitch3->vstring->SetValue(LFORate_Pitch3->outval);
    }
    else if(param == LFOAmp_Flt1Freq)
    {
        LFOAmp_Flt1Freq->vstring->SetValue(int(LFOAmp_Flt1Freq->outval*100));
    }
    else if(param == LFOAmp_Flt2Freq)
    {
        LFOAmp_Flt2Freq->vstring->SetValue(int(LFOAmp_Flt2Freq->outval*100));
    }
    else if(param == LFORate_Flt1Freq)
    {
        LFORate_Flt1Freq->vstring->SetValue(LFORate_Flt1Freq->outval);
    }
    else if(param == LFORate_Flt2Freq)
    {
        LFORate_Flt2Freq->vstring->SetValue(LFORate_Flt2Freq->outval);
    }
    else if(param == osc1FM2Level)
    {
        osc1FM2Level->vstring->SetValue(int(osc1FM2Level->outval*100));
    }
    else if(param == osc2FM3Level)
    {
        osc2FM3Level->vstring->SetValue(int(osc2FM3Level->outval*100));
    }
    else if(param == osc1FM3Level)
    {
        osc1FM3Level->vstring->SetValue(int(osc1FM3Level->outval*100));
    }
    else if(param == osc1RM2Level)
    {
        osc1RM2Level->vstring->SetValue(int(osc1RM2Level->outval*100));
    }
    else if(param == osc2RM3Level)
    {
        osc2RM3Level->vstring->SetValue(int(osc2RM3Level->outval*100));
    }
    else if(param == osc1RM3Level)
    {
        osc1RM3Level->vstring->SetValue(int(osc1RM3Level->outval*100));
    }
    else if(param == osc1Width)
    {
        osc1Width->vstring->SetValue(osc1Width->outval);
    }
    else if(param == osc2Width)
    {
        osc2Width->vstring->SetValue(osc2Width->outval);
    }
    else if(param == osc3Width)
    {
        osc3Width->vstring->SetValue(osc3Width->outval);
    }
    /* // This stuff gets updated in the top of this function, when we call SetNormalVaule for the param.
    else if(param == filt1[0]->cutoff)
    {
        filt1[0]->ParamUpdate(filt1[0]->cutoff);
    }
    else if(param == filt2[0]->cutoff)
    {
        filt2[0]->ParamUpdate(filt2[0]->cutoff);
    }
    else if(param == filt1[0]->resonance)
    {
        filt1[0]->ParamUpdate(filt1[0]->resonance);
    }
    else if(param == filt2[0]->resonance)
    {
        filt2[0]->ParamUpdate(filt2[0]->resonance);
    }
    */

	//if(param->vstring != NULL)
	//{
	//	aslider->setText(String(param->vstring->outstr), false);
	//}
}

int Synth::MapEnvPointerToNum(float * eV)
{
    if(eV == &envV1)
        return 1;
    else if(eV == &envV2)
        return 2;
    else if(eV == &envV3)
        return 3;
    else if(eV == &envV4)
        return 4;
    else if(eV == &envV5)
        return 5;
    else if(eV == ebuff1)
        return 1;
    else if(eV == ebuff2)
        return 2;
    else if(eV == ebuff3)
        return 3;
    else if(eV == ebuff4)
        return 4;
    else if(eV == ebuff5)
        return 5;
	else
        return 0;
}

float* Synth::MapNumToEnvPointer(int num)
{
    switch(num)
    {
        case 1:
            return &envV1;
        case 2:
            return &envV2;
        case 3:
            return &envV3;
        case 4:
            return &envV4;
        case 5:
            return &envV5;
        default:
            return NULL;
    }
}

float* Synth::MapNumToEBuffPointer(int num)
{
    switch(num)
    {
        case 1:
            return ebuff1;
        case 2:
            return ebuff2;
        case 3:
            return ebuff3;
        case 4:
            return ebuff4;
        case 5:
            return ebuff5;
        default:
            return NULL;
    }
}

void Synth::SaveCustomStateData(XmlElement & xmlParentNode)
{
    XmlElement * xmlCustomData = new XmlElement(T("EnvIndex"));
    xmlCustomData->setAttribute(T("Osc1"), MapEnvPointerToNum(p_osc1envVal));
    xmlCustomData->setAttribute(T("Osc2"), MapEnvPointerToNum(p_osc2envVal));
    xmlCustomData->setAttribute(T("Osc3"), MapEnvPointerToNum(p_osc3envVal));
    xmlCustomData->setAttribute(T("Filt1"), MapEnvPointerToNum(p_flt1envVal));
    xmlCustomData->setAttribute(T("Filt2"), MapEnvPointerToNum(p_flt2envVal));

    xmlParentNode.addChildElement(xmlCustomData);

    xmlParentNode.addChildElement(env1->Save("Envelope1"));
    xmlParentNode.addChildElement(env2->Save("Envelope2"));
    xmlParentNode.addChildElement(env3->Save("Envelope3"));
    xmlParentNode.addChildElement(env4->Save("Envelope4"));
    xmlParentNode.addChildElement(env5->Save("Envelope5"));

    //xmlEnv1Data->setAttribute(T("Osc1"), MapEnvPointerToNum(p_osc1envVal));
}

void Synth::RestoreCustomStateData(XmlElement & xmlStateNode)
{
    int idx;
    if(xmlStateNode.hasTagName(T("EnvIndex")))
    {
        idx = xmlStateNode.getIntAttribute(T("Osc1"));
        p_osc1envVal = MapNumToEnvPointer(idx);
        idx = xmlStateNode.getIntAttribute(T("Osc2"));
        p_osc2envVal = MapNumToEnvPointer(idx);
        idx = xmlStateNode.getIntAttribute(T("Osc3"));
        p_osc3envVal = MapNumToEnvPointer(idx);
        idx = xmlStateNode.getIntAttribute(T("Filt1"));
        p_flt1envVal = MapNumToEBuffPointer(idx);
        idx = xmlStateNode.getIntAttribute(T("Filt2"));
        p_flt2envVal = MapNumToEBuffPointer(idx);

        if(SynthComp != NULL)
            SynthComp->UpdateEnvButts();
    }
    else if(xmlStateNode.hasTagName(T("Envelope1")))
    {
        env1->Load(&xmlStateNode);
    }
    else if(xmlStateNode.hasTagName(T("Envelope2")))
    {
        env2->Load(&xmlStateNode);
    }
    else if(xmlStateNode.hasTagName(T("Envelope3")))
    {
        env3->Load(&xmlStateNode);
    }
    else if(xmlStateNode.hasTagName(T("Envelope4")))
    {
        env4->Load(&xmlStateNode);
    }
    else if(xmlStateNode.hasTagName(T("Envelope5")))
    {
        env5->Load(&xmlStateNode);
    }

    if(SynthComp != NULL)
        SynthComp->repaint();
}

void Synth::CopyDataToClonedInstrument(Instrument * instr)
{
    Instrument::CopyDataToClonedInstrument(instr);

    Synth* nsynth = (Synth*)instr;

    XmlElement  xmlPreset(T("ClonePreset"));
    this->SaveStateData(xmlPreset, current_preset_name);

    XmlElement* xmlModule = xmlPreset.getChildByName(T("Module"));
    if(xmlModule != NULL)
        nsynth->RestoreStateData(*xmlModule);

    strcpy(nsynth->current_preset_name, current_preset_name);
    strcpy(nsynth->preset_path, preset_path);
}

Osc::Osc()
{
    phase_shift = 0;
    note_shift = 0;
    finetune = 0;
}

void Sine::GetData(double wt_pos, float freq, long frame_phase, float* od1, float* od2)
{
   *od1 = *od2 = wt_sine[int(wt_pos*wt_coeff*freq)&0x7FFF];
}

void Saw::GetData(double wt_pos, float freq, long frame_phase, float* od1, float* od2)
{
    *od1 = *od2 = wt_saw[int(wt_pos*wt_coeff*freq)&0x7FFF];
}

void Triangle::GetData(double wt_pos, float freq, long frame_phase, float* od1, float* od2)
{
    *od1 = *od2 = wt_triangle[int(wt_pos*wt_coeff*freq)&0x7FFF];
}

VSTGenerator::VSTGenerator(char* fullpath, char* alias)
{
    internalModule = false;
    AliasRecord ar;
    memset(&ar, 0, sizeof(AliasRecord));
    strcpy(ar.alias, alias);
    ar.type = EffType_VSTPlugin;
    this->type = Instr_VSTPlugin;
    this->NumMidiEvents = 0;
    this->pEff = new VSTEffect(NULL, &ar, fullpath);
    scope.eff = this->pEff;
    this->pEff->scope = scope;
    muteCount = 0;
    if (this->pEff != NULL)
    {
        /* Unable to initialize FX - nullify the pointer then */
        if (((VSTEffect*)(this->pEff))->pPlug == NULL)
        {
            this->pEff = NULL;
        }
    }
}

// Cloning helper method
VSTGenerator::VSTGenerator(VSTGenerator* vst, char* alias)
{
    internalModule = false;
    AliasRecord ar;
    memset(&ar, 0, sizeof(AliasRecord));
    strcpy(ar.alias, alias);
    ar.type = EffType_VSTPlugin;
    this->type = Instr_VSTPlugin;
    this->NumMidiEvents = 0;
    this->pEff = vst->pEff->Clone(NULL);
    scope.eff = this->pEff;
    this->pEff->scope = scope;
    muteCount = 0;
    if (this->pEff != NULL)
    {
        /* Unable to initialize FX - nullify the pointer then */
        if (((VSTEffect*)(this->pEff))->pPlug == NULL)
        {
            this->pEff = NULL;
        }
    }
}

VSTGenerator::~VSTGenerator()
{
    if (this->pEff != NULL)
    {
        delete pEff;
    }
}

void VSTGenerator::CheckBounds(Gennote * gnote, Trigger * tg, long num_frames)
{
    long loc_num_frames = num_frames;

    if(tg->frame_phase + loc_num_frames >= gnote->frame_length || 
        tg->tgstate == TgState_Release || 
        tg->tgstate == TgState_Finished ||
        tg->tgstate == TgState_SoftFinish)
    {
        if(gnote->preview == true)
        {
            if(tg->tgstate == TgState_Release || tg->tgstate == TgState_Finished)
            {
                tg->pslot->state = PState_Stopped;
            }
            else
            {
                /* Just a temp hack to continue sustaining */
                loc_num_frames = 0;
            }
        }
        else
        {
            tg->Deactivate();
        }
    }

    tg->frame_phase += loc_num_frames;
}

long VSTGenerator::ProcessTrigger(Trigger* tg, long num_frames, long buffframe)
{
    float vol = 0;
    long loc_num_frames = num_frames;
    Gennote* gnote = (Gennote*)tg->el;
    vol = gnote->loc_vol->val*tg->patt->loc_vol->val;

    if(gnote->frame_length - tg->frame_phase < loc_num_frames)
        loc_num_frames = gnote->frame_length - tg->frame_phase;

    AddNoteEvent(tg->note_val, loc_num_frames, tg->frame_phase, gnote->frame_length, vol);

    if(tg->frame_phase + loc_num_frames >= gnote->frame_length || 
        tg->tgstate == TgState_Release || 
        tg->tgstate == TgState_Finished ||
        tg->tgstate == TgState_SoftFinish)
    {
        if(gnote->preview == true)
        {
            if(tg->tgstate == TgState_Release || 
               tg->tgstate == TgState_Finished ||
               tg->tgstate == TgState_SoftFinish)
            {
                tg->pslot->state = PState_Stopped;
            }
            else
            {
                /* Just a temp hack to continue sustaining */
                loc_num_frames = 0;
            }
        }
        else
        {
            //tg->Deactivate();
        }
    }

    tg->frame_phase += loc_num_frames;

    return num_frames;
}

void VSTGenerator::PostProcessTrigger(Trigger* tg, long num_frames, long buffframe, long mixbuffframe, long remaining)
{
    float       pan;
    float       volL, volR;
    long        tc, tc0;
    int         ai;

    pan = params->pan->outval;
    // PanConstantRule(pan, &volL, &volR);
    // Calculate constant rule directly here to improve performance
    ai = int((PI_F*(pan + 1)/4)/wt_angletoindex);
    volL = wt_cosine[ai];
    volR = wt_sine[ai];

    Envelope* penv1 = NULL;
    if(params->pan->envelopes != NULL)
    {
        penv1 = (Envelope*)((Command*)params->pan->envelopes->el)->paramedit;
        if(penv1->newbuff == false)
            penv1 = NULL;
    }

    tc = mixbuffframe*2;
    tc0 = buffframe*2;
    for(long cc = 0; cc < num_frames; cc++)
    {
        if(penv1 != NULL)
        {
            pan = penv1->buffoutval[mixbuffframe + cc];
            // recalculate pan
            // PanConstantRule(pan, &volL, &volR);
            ai = int((PI_F*(pan + 1)/4)/wt_angletoindex);
            volL = wt_cosine[ai];
            volR = wt_sine[ai];
        }
    
        out_buff[tc0] = data_buff[tc0++]*volL;
        out_buff[tc0] = data_buff[tc0++]*volR;
    }
}

void VSTGenerator::DeactivateTrigger(Trigger* tg)
{
    Gennote* gnote = (Gennote*)tg->el;
    this->PostNoteOFF(gnote->ed_note->value, 127);
    ((Instrument*)this)->Instrument::DeactivateTrigger(tg);
}

void VSTGenerator::VSTProcess(long num_frames, long buffframe)
{
    /*
    struct VstEvents			// a block of events for the current audio block
    {
    	long numEvents;
    	long reserved;			// zero
    	VstEvent* events[2];	// variable
    };
    */
    if(this->pEff != NULL)
    {
        VstEvents* events = NULL;

        if(this->NumMidiEvents > 0)
        {
            long             EvSize = 0;
            unsigned short	 NumEvents = (unsigned short)this->NumMidiEvents;

            CEffect         *pEffect = 
                ((VSTEffect*)(this->pEff))->pPlug->pHost->GetAt(((VSTEffect*)(this->pEff))->pPlug->eff_index);

            EvSize = sizeof(long)+ sizeof(long) + (sizeof(VstEvent*)*NumEvents);
            events = (VstEvents*)malloc(EvSize);

            memset(events, 0, EvSize);
            events->numEvents = NumEvents;
            events->reserved = 0;

            for (int i = 0; i < NumEvents; ++i)
            {
                events->events[i] = (VstEvent*) &(this->MidiEvents[i]);
            }

            pEffect->EffProcessEvents(events);
        }

        ((VSTEffect*)(this->pEff))->ProcessData(NULL, &data_buff[buffframe*2], num_frames);

        this->NumMidiEvents = 0;

        // This freeing code was working here, but it causes Absynth 2.04 to crash, while other hosts work
        // well with it. Probably it's responsibility of a plugin to free the memory allocated for VstEvents
        // struct, although need to ensure later in this. Hahaha.
        //if(NULL != events)
        //{
        //    free(events);
        //}
    }
}

void VSTGenerator::GenerateData(long num_frames, long mixbuffframe)
{
    bool off;
    if(params->muted == false && (Solo_Instr == NULL || Solo_Instr == this))
    {
        off = false;
    }
    else
    {
        off = true;
    }
    fill = true;

    Trigger* tg = tg_first;
    Trigger* tgnext;
    while(tg != NULL)
    {
        tgnext = tg->loc_act_next;
        //if(tg->patt->IsPresent() && tg->el->IsPresent())
        {
            ProcessTrigger(tg, num_frames);
        }
        tg = tgnext;
    }

    memset(out_buff, 0, num_frames*sizeof(float)*2);
    memset(data_buff, 0, num_frames*sizeof(float)*2);

    if(envelopes == NULL)
    {
        VSTProcess(num_frames, 0);
    }
    else
    {
        Envelope* env;
        Parameter* param;
        Trigger* tgenv;
        long frames_to_process;
        long frames_remaining = num_frames;
        long buffframe = 0;
        while(frames_remaining > 0)
        {
            if(frames_remaining > BUFF_CHUNK_SIZE)
            {
                frames_to_process = BUFF_CHUNK_SIZE;
            }
            else
            {
                frames_to_process = frames_remaining;
            }

            tgenv = envelopes;
            while(tgenv != NULL)
            {
                env = (Envelope*)((Command*)tgenv->el)->paramedit;
                param = ((Command*)tgenv->el)->param;
                param->SetValueFromEnvelope(env->buff[mixbuffframe + buffframe], env);
                tgenv = tgenv->group_prev;
            }

            VSTProcess(frames_to_process, buffframe);

            frames_remaining -= frames_to_process;
            buffframe += frames_to_process;
        }
    }

    if(off == false)
    {
        if(muteCount > 0)
        {
            tframe tc = 0;
            float aa;
            while(tc < num_frames)
            {
                if(muteCount > 0)
                {
                    aa = float(DECLICK_COUNT - muteCount)/DECLICK_COUNT;
                    muteCount--;
                }
                data_buff[tc*2] *= aa;
                data_buff[tc*2 + 1] *= aa;
                tc++;
            }
        }
    }
    else
    {
        if(muteCount < DECLICK_COUNT)
        {
            tframe tc = 0;
            float aa;
            while(tc < num_frames)
            {
                if(muteCount < DECLICK_COUNT)
                {
                    aa = float(DECLICK_COUNT - muteCount)/DECLICK_COUNT;
                    muteCount++;
                }
                data_buff[tc*2] *= aa;
                data_buff[tc*2 + 1] *= aa;
                tc++;
            }
        }
        else
            fill = false;
    }

    if(fill)
    {
        PostProcessTrigger(NULL, num_frames, 0, mixbuffframe);
        FillMixChannel(num_frames, 0, mixbuffframe);
    }
}

void VSTGenerator::AddNoteEvent(int note, long num_frames, long frame_phase, long total_frames, float volume)
{
    VstMidiEvent* pEv = &(this->MidiEvents[this->NumMidiEvents]);
	unsigned char velocity = (unsigned char)((volume <= 1) ? volume * 100 : 100 + (volume - 1)/(VolRange - 1)*0x1B); //volume is the value from 0 to 1, if it's greater then it's gaining
/*
struct VstMidiEvent		// to be casted from a VstEvent
{
	long type;			// kVstMidiType
	long byteSize;		// 24
	long deltaFrames;	// sample frames related to the current block start sample position
	long flags;			// none defined yet

	long noteLength;	// (in sample frames) of entire note, if available, else 0
	long noteOffset;	// offset into note from note start if available, else 0

	char midiData[4];	// 1 thru 3 midi bytes; midiData[3] is reserved (zero)
	char detune;		// -64 to +63 cents; for scales other than 'well-tempered' ('microtuning')
	char noteOffVelocity;
	char reserved1;		// zero
	char reserved2;		// zero
};
*/
    memset(pEv, 0, sizeof(VstMidiEvent));
    pEv->type = kVstMidiType;
    pEv->byteSize = sizeof(VstMidiEvent);
    //pEv->deltaFrames = (num_frames < total_frames - frame_phase)? (num_frames - 1): (total_frames - frame_phase);
    //pEv->noteLength = total_frames - frame_phase;
    //pEv->noteOffset = frame_phase;

    pEv->deltaFrames = 0;
    pEv->noteLength = 0;
    pEv->noteOffset = 0;

    pEv->flags = 0;

    if (frame_phase == 0)
    {
        pEv->midiData[0] = (char)0x90;
        pEv->midiData[1] = note;
        pEv->midiData[2] = velocity;
        this->NumMidiEvents++;
    }
    //else if (frame_phase < total_frames)
    //{
    //    pEv->midiData[0] = (char)0xa0;
    //    pEv->midiData[1] = note;
    //    pEv->midiData[2] = velocity;
    //    this->NumMidiEvents++;
    //}
    else if(frame_phase > total_frames)
    {
        pEv->midiData[0] = (char)0x80;
        pEv->midiData[1] = note;
        pEv->midiData[2] = velocity;
        this->NumMidiEvents++;
    }
}

void VSTGenerator::PostNoteON(int note, float vol)
{
    VstMidiEvent* pEv = &(this->MidiEvents[this->NumMidiEvents]);
    unsigned char l_velocity = (unsigned char)((vol <= 1) ? vol * 127 : 100 + (vol - 1)/(VolRange - 1)*0x1B); //volume is the value from 0 to 1, if it's greater then its gaining

    memset(pEv, 0, sizeof(VstMidiEvent));
    pEv->type = kVstMidiType;
    pEv->byteSize = sizeof(VstMidiEvent);
    pEv->deltaFrames = 0;
    pEv->noteLength = 0;
    pEv->noteOffset = 0;
    pEv->flags = 0;

    pEv->midiData[0] = (char)0x90;
    pEv->midiData[1] = note;
    pEv->midiData[2] = l_velocity;
    this->NumMidiEvents++;
}

void VSTGenerator::PostNoteOFF(int note, int velocity)
{
    VstMidiEvent* pEv = &(this->MidiEvents[this->NumMidiEvents]);

    memset(pEv, 0, sizeof(VstMidiEvent));
    pEv->type = kVstMidiType;
    pEv->byteSize = sizeof(VstMidiEvent);
    pEv->deltaFrames = 0;
    pEv->noteLength = 0;
    pEv->noteOffset = 0;
    pEv->flags = 0;

    pEv->midiData[0] = (char)0x80;
    pEv->midiData[1] = note;
    pEv->midiData[2] = velocity;
    this->NumMidiEvents++;
}

void VSTGenerator::StopAllNotes()
{
    VstMidiEvent* pEv = &(this->MidiEvents[this->NumMidiEvents]);

    memset(pEv, 0, sizeof(VstMidiEvent));
    pEv->type = kVstMidiType;
    pEv->byteSize = sizeof(VstMidiEvent);
    pEv->deltaFrames = 0;
    pEv->noteLength = 0;
    pEv->noteOffset = 0;
    pEv->flags = 0;

    pEv->midiData[0] = (char)0xB0;
    pEv->midiData[1] = 0x7B;
    pEv->midiData[2] = 0x00;
    this->NumMidiEvents++;
}

void VSTGenerator::Save(XmlElement * instrNode)
{
    Instrument::Save(instrNode);

    //XmlElement* effModule = new XmlElement(T("EffModule"));
    //pEff->SaveStateData(*instrNode, "Current");
    XmlElement* effModule = new XmlElement(T("EffModule"));
    pEff->Save(effModule);
    instrNode->addChildElement(effModule);

/*
    XmlElement* state = new XmlElement ("STATE");

    MemoryBlock m;
    pEff->GetStateInformation(m);
    state->addTextElement (m.toBase64Encoding());
    instrNode->addChildElement (state);
    */
}

void VSTGenerator::Load(XmlElement * instrNode)
{
    Instrument::Load(instrNode);

    XmlElement* effModule = instrNode->getChildByName (T("EffModule"));
    if(effModule != NULL)
    {
       pEff->Load(effModule);
    }

    //XmlElement* stateNode = instrNode->getChildByName(T("Module"));
    //if(stateNode != NULL)
    //{
    //    pEff->RestoreStateData(*stateNode);
    //}

/*
    const XmlElement* const state = instrNode->getChildByName (T("STATE"));

    if(state != 0)
    {
        MemoryBlock m;
        m.fromBase64Encoding (state->getAllSubText());

        pEff->SetStateInformation (m.getData(), m.getSize());
    }
*/
}

/*
void VSTGenerator::CopyDataToClonedInstrument(Instrument * instr)
{
    Instrument::CopyDataToClonedInstrument(instr);

    VSTGenerator* nvst = (VSTGenerator*)instr;

    MemoryBlock m;
    pEff->GetStateInformation(m);
    nvst->pEff->SetStateInformation(m.getData(), m.getSize());
}
*/

void VSTGenerator::ToggleParamWindow()
{
    if(pEff->VSTParamWnd == NULL)
    {
        pEff->VSTParamWnd = MainWnd->CreateVSTParamWindow(pEff, &scope);
        pEff->VSTParamWnd->Show();
    }
    else
    {
        if(pEff->VSTParamWnd->isVisible())
        {
            pEff->VSTParamWnd->Hide();
        }
        else
        {
            pEff->VSTParamWnd->Show();
        }
    }
}

void VSTGenerator::Reset()
{
    StopAllNotes();
}

void ShowInstrumentWindow(Instrument* i)
{
    i->pEditButton->Press();
    i->instr_drawarea->Change();

    if(i->type == Instr_VSTPlugin)
    {
        VSTGenerator* pGen = (VSTGenerator*)i;
        if(pGen->pEff != NULL)
        {
            if(pGen->pEff->pPlug->HasEditor())
            {
                if(pGen->pEff->pPlug->isWindowActive == false)
                {
                    pGen->pEff->ShowEditor(true);
                    pGen->wnd = pGen->pEff->pPlug->pWnd;
                }
                else
                {
                    pGen->pEff->ShowEditor(false);
                    pGen->wnd = NULL;
                    i->pEditButton->Release();
                }
            }
            else
            {
                if(pGen->pEff->pPlug->isWindowActive == false)
                {
                    if(pGen->pEff->VSTParamWnd == NULL)
                    {
                        pGen->pEff->VSTParamWnd = MainWnd->CreateVSTParamWindow(pGen->pEff, &pGen->scope);
                        //pGen->wnd = pGen->pEff->VSTParamWnd;
                        pGen->pEff->VSTParamWnd->Show();
                    }
                    else
                    {
                        if(pGen->pEff->VSTParamWnd->isShowing() == false)
                        {
                            pGen->pEff->VSTParamWnd->Show();
                        }
                        else
                        {
                            pGen->pEff->VSTParamWnd->Hide();
                            i->pEditButton->Release();
                        }
                    }
                }
                else
                {
                    if(pGen->pEff->VSTParamWnd != NULL)
                    {
                        pGen->pEff->VSTParamWnd->Hide();
                        i->pEditButton->Release();
                    }
                }
            }
        }

        /* Just a temp hack to test midi keyboard */
        //pMidiHost->Attach((Instrument*)pGen, 1);
    }
    else if(i->type == Instr_Sample)
    {
		if(SmpWnd != NULL)
		{
			if(SmpWnd->sample == NULL || SmpWnd->sample != i)
			{
				SmpWnd->SetSample((Sample*)i);

				if(SmpWnd->isVisible() == false)
					SmpWnd->Show();
			}
			else
			{
				if(SmpWnd->isVisible() == false)
                {
					SmpWnd->Show();
                }
				else
                {
                    SmpWnd->Hide();
                    i->pEditButton->Release();
                }
			}
		}
    }
    else if(i->type == Instr_Generator)
    {
        Synth* syn = (Synth*)i;
        if(syn->synthWin == NULL)
            syn->CreateModuleWindow();

        if(syn->synthWin->Showing() == false)
        {
            syn->synthWin->Show();
        }
        else
        {
            syn->synthWin->Hide();
            i->pEditButton->Release();

            // This is a workaround shit because sometimes synthwindow becomes hidden somehow
            //if(syn->synthWin->isVisible() == false)
            //{
            //    syn->synthWin->setVisible(true);
            //}
            //syn->synthWin->toFront(true);
        }
    }
}

bool SwitchCurrentInstrumentWindowOFFIfNeeded(Instrument* newinstr)
{
    current_instr->pEditButton->Release();
    current_instr->instr_drawarea->Change();

	// Hide previous instrument's window
    if(current_instr->type == Instr_VSTPlugin)
    {
        VSTGenerator* pGen = (VSTGenerator*)current_instr;
        if(pGen->pEff->pPlug->HasEditor())
        {
            if(pGen->pEff->pPlug->isWindowActive == true)
            {
                pGen->pEff->ShowEditor(false);
                return true;
            }
        }
        else
        {
            if(pGen->pEff->VSTParamWnd != NULL && pGen->pEff->VSTParamWnd->isShowing())
            {
                pGen->pEff->VSTParamWnd->Hide();
                return true;
            }
        }
    }
    else if(current_instr->type == Instr_Sample)
    {
		if(SmpWnd != NULL && SmpWnd->Showing() == true)
		{
			//if(newinstr->type != Instr_Sample)
			{
				SmpWnd->Hide();
			}
			return true;
		}

    }
    else if(current_instr->type == Instr_Generator)
    {
        Synth* syn = (Synth*)current_instr;
        if(syn->synthWin == NULL)
            syn->CreateModuleWindow();

        {
            if(syn->synthWin->Showing() == true)
            {
                syn->synthWin->Hide();
                return true;
            }
        }
    }

    return false;
}

void GenWindowToggleOnClick(Object* owner, Object* self)
{
    Instrument* i = (Instrument*)self->owner;
    ShowInstrumentWindow(i);
}

void EditButtonClick(Object* owner, Object* self)
{
    Instrument* i = (Instrument*)self->owner;
    IP->EditInstrumentAutopattern(i);
}

