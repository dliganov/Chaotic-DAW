

#include "awful.h"
#include "awful_audio.h"
#include "awful_instruments.h"
#include "awful_elements.h"
#include "awful_paramedit.h"
#include "awful_controls.h"
#include "awful_panels.h"
#include "awful_events_triggers.h"
#include "awful_utils_common.h"
#include "awful_preview.h"

void                    PreInitEnvelopes(tframe frame, Pattern* pt, Event* firstev, bool activate_env, bool paraminit);
void                    ProcessPlacedEnvelopes(Pattern* pt, long buffframe, long num_frames, long curr_frame);
extern void             SetMixcell4Trigger(Trigger* tg);
extern void             SetMixChannel4Trigger(Trigger* tg);
extern void             AddAutopatternInstance(Trigger* tg, NoteInstance* ii, bool add);
extern void             DisableAllPlaybacks();
extern void             GlobalAddActiveTrigger(Trigger* tg);
extern void             GlobalRemoveActiveTrigger(Trigger* tg);
extern void             DeactivatePlayback(Playback* pbk);
extern void             ActivatePlayback(Playback* pbk);
extern void             ActivateCommandTrigger(Trigger* tg);
extern void             DeactivateCommandTrigger(Trigger* tg);
extern void             DeactivateSymbolTrigger(Trigger* efftg);
extern void             ActivateSymbolTrigger(Trigger* efftg);
extern void             UpdateEventsFrames(Pattern* pt);
extern void             RemoveTriggerFromEvent(Trigger* c);
extern void             MakeTriggersForElement(Element* el, Pattern* ptmain);
extern void             RemoveTriggerFromEvent(Trigger* c);
extern void             Locate_Trigger(Trigger* tg);
extern void             Del_Element_Triggers(Element* el);
extern bool             IsTriggerOutOfPlayback(Trigger* tg);
extern bool             IsSymbolTrigger(Trigger* tg);
extern bool             IsInstantTrigger(Trigger* tg);

Event::Event()
{
	next = NULL;
	prev = NULL;
	tg_first = NULL;
	tg_last = NULL;

    to_be_deleted = false;
}

void Event::AddTrigger(Trigger* tg)
{
    tg->ev = this;

    if(tg_first == NULL && tg_last == NULL)
    {
        tg->ev_prev = NULL;
        tg->ev_next = NULL;
        tg_first = tg;
    }
    else
    {
        tg_last->ev_next = tg;
        tg->ev_prev = tg_last;
        tg->ev_next = NULL;
    }
    tg_last = tg;
}

bool Event::RemoveTrigger(Trigger* tg)
{
    bool no_more = false;
    if(tg == tg_first && tg == tg_last)
    {
        no_more = true;
		tg_first = NULL;
		tg_last = NULL;
    }
    else if(tg == tg_first)
    {
        tg_first = tg->ev_next;
        tg_first->ev_prev = NULL;
    }
    else if(tg == tg_last)
    {
        tg_last = tg->ev_prev;
        tg_last->ev_next = NULL;
    }
    else
    {
        tg->ev_prev->ev_next = tg->ev_next;
        tg->ev_next->ev_prev = tg->ev_prev;
    }

    return no_more;
}

// Looks like for both below functions we consider all element's and pattern's triggers are deactivated and 
// not playing, but if we gonna support forced deletion, need to check and perform deactivation here, eh?
void Del_Element_Triggers(Element* el)
{
    Trigger* tg = el->tg_first;
    Trigger* tgnext;
    while(tg != NULL)
    {
        tg->Deactivate();
        RemoveTriggerFromEvent(tg);
        tg->patt->RemoveInternalTrigger(tg);
        if(tg->apatt_instance != NULL)
        {
            // Take care of autopattern playback, if any. Pattern doesn't care about this, when being deleted.
            if(tg->apatt_instance->IsPlaybackActive())
            {
                DeactivatePlayback(tg->apatt_instance->pbk);
            }

            DeleteElement(tg->apatt_instance, true, true);
        }
        tgnext = tg->el_next;
        delete tg;
        tg = tgnext;
    }
    el->tg_first = el->tg_last = NULL;
}

void Del_Pattern_Internal_Triggers(Pattern* pt, bool skipelremove)
{
    Trigger* tg = pt->tg_internal_first;
    Trigger* tgnext;
    while(tg != NULL)
    {
        tgnext = tg->pt_next;
        tg->Deactivate();
        RemoveTriggerFromEvent(tg);
        if(!skipelremove)
            tg->el->RemoveTrigger(tg);
        if(tg->apatt_instance != NULL)
        {
            // Take care of autopattern playback, if any. Pattern doesn't care about this, when being deleted.
            if(tg->apatt_instance->IsPlaybackActive())
            {
                DeactivatePlayback(tg->apatt_instance->pbk);
            }

            DeleteElement(tg->apatt_instance, true, true);
        }
        delete tg;
        tg = tgnext;
    }
    pt->tg_internal_first = pt->tg_internal_last = NULL;
}

long GetGlobalFrame4Trigger(Trigger* tg)
{
    // Get global pattern data for this trigger
    if(tg->activator == true)
    {
        //return tg->el->start_frame + tg->patt->start_frame;
        return Tick2Frame(tg->patt->start_tick + tg->el->start_tick);
    }
    else
    {
        if((tg->el->type == El_SlideNote)||
           (tg->el->type == El_Slider)||
           (tg->el->type == El_Mute)||
           (tg->el->type == El_Vibrate)||
           (tg->el->type == El_Transpose)||
           (tg->el->type == El_Command)||
		   (tg->el->type == El_GenNote))
        {
            return Tick2Frame(tg->patt->start_tick + tg->el->end_tick);
        }
        else
        {
            jassertfalse;
			return -1;
        }
    }
}

void SetMixcell4Trigger(Trigger* tg)
{
	/*
    if(tg->trkdata == NULL)
    {
        tg->mcell = &mix->m_cell;
    }
    else
    {
        tg->mcell = tg->trkdata->mcell;
    }

    if((tg->el->IsInstance()) && mix->trkfxcell[tg->trkdata->trknum] == &mix->m_cell)
    {
        tg->mcell = ((Instance*)tg->el)->instr->fxcell;
    }
	*/
}

void SetMixChannel4Trigger(Trigger* tg)
{
    if(tg->trkdata == NULL)
    {
        tg->mchan = &aux_panel->masterchan;
    }
    else
    {
        tg->mchan = tg->trkdata->mchan;
    }

    if((tg->el->IsInstance()) && (tg->mchan == NULL || tg->mchan == &aux_panel->masterchan))
    {
        tg->mchan = ((NoteInstance*)tg->el)->instr->fx_channel;
    }
}

void Locate_Trigger(Trigger* tg)
{
    long frame = GetGlobalFrame4Trigger(tg);
    float tick = tg->activator ? tg->patt->start_tick + tg->el->start_tick : tg->patt->start_tick + tg->el->end_tick;

    Pattern* evpatt;
    Event** firstEv;
    Event** lastEv;
    if(tg->patt->basePattern== NULL || tg->patt->basePattern->autopatt == false)
    {
        firstEv = &field_pattern->first_ev;
        lastEv = &field_pattern->last_ev;
        evpatt = field_pattern;
    }
    else
    {
        firstEv = &tg->patt->first_ev;
        lastEv = &tg->patt->last_ev;
        evpatt = tg->patt;
    }

    // Determine the target global trackdata
    if(tg->patt == field_pattern)
    {
        tg->trkdata = tg->el->field_trkdata;
    }
    else
    {
        tg->trkdata = tg->patt->field_trkdata;
    }

    //SetMixcell4Trigger(tg);

    if((*firstEv == NULL)&&(*lastEv == NULL))
    {
        Event* ev = new Event;
        ev->AddTrigger(tg);

        ev->frame = frame;
        ev->vick = tick;
        ev->patt = evpatt;

        ev->prev = NULL;
        ev->next = NULL;

       *firstEv = ev;
       *lastEv = ev;
    }
    else
    {
        Event* ev = *firstEv;
        while(ev != NULL)
        {
            if(frame < ev->frame)
            {
                Event* n_ev = new Event;
                n_ev->AddTrigger(tg);

                n_ev->frame = frame;
                n_ev->vick = tick;
                n_ev->patt = evpatt;
                if(ev == *firstEv)
                {
                   *firstEv = n_ev;
                }
                else
                {
                    ev->prev->next = n_ev;
                }
                n_ev->next = ev;
                n_ev->prev = ev->prev;
                ev->prev = n_ev;

                break;
            }
            else if(frame == ev->frame)
            {
				//jassert(ev->vick == tick);
                ev->AddTrigger(tg);
                if(ev->to_be_deleted == true)
                {
                    ev->to_be_deleted = false;
                }
                break;
            }
            else if(ev == *lastEv)
            {
                Event* n_ev = new Event;
                n_ev->AddTrigger(tg);

                n_ev->frame = frame;
                n_ev->vick = tick;
                n_ev->patt = evpatt;

                n_ev->next = NULL;
                n_ev->prev = ev;
                ev->next = n_ev;
               *lastEv = n_ev;
                break;
            }
            else
            {
                ev = ev->next;
            }
        }
    }
}

void AddAutopatternInstance(Trigger* tg, NoteInstance* ii, bool add)
{
    if(ii->instr->autoPatt != NULL)
    {
        tg->apatt_instance = new Pattern("autoinstance", ii->instr->autoPatt->StartTick(), 
                             ii->instr->autoPatt->EndTick(), 
                             ii->track_line, 
                             ii->track_line, 
                             false);
		tg->apatt_instance->patt = tg->patt;
        tg->apatt_instance->displayable = false;
        tg->apatt_instance->parent_trigger = tg;
        tg->apatt_instance->folded = true;
        tg->apatt_instance->Update();
        ii->instr->autoPatt->AddDerivedPattern(tg->apatt_instance, add == false);
        tg->apatt_instance->AddPlayback();

        if(add)
        {
            AddNewElement(tg->apatt_instance, true);
        }
    }
}

/*==================================================================================================
BRIEF: This function creates and locates triggers for an element per single child pattern. Every 
editable and playable pattern does not actually contain any element, but only triggers of elements.

Also, we add an auto-pattern instance here, if applicable.

PARAMETERS:
[IN]
    el - element to create triggers for.
    pt - pattern to create element's triggers at.

[OUT]
N/A

OUTPUT: N/A
==================================================================================================*/
void CreateElementTriggersPerPattern(Pattern* pt, Element* el, bool skipaddtoelement)
{
    // if this is not autopattern inside autopattern (excluding original pattern case when pt == pt->OrigPt)
    if(el->IsInstance() && pt != pt->basePattern && pt->patt != NULL && pt->patt->basePattern != NULL &&
							pt->patt->basePattern->autopatt == true)
	{
		return;
	}
	else
	{
		Trigger* tg_start = new Trigger;
		tg_start->activator = true;
		tg_start->el = el;
		tg_start->patt = pt;

        if(!skipaddtoelement)
            el->AddTrigger(tg_start);
        pt->AddInternalTrigger(tg_start);

        Locate_Trigger(tg_start);
        if(((el->type == El_GenNote) && ((NoteInstance*)el)->instr->instrtype == Instr_VSTPlugin)||
             (el->type == El_Mute)||
             (el->type == El_SlideNote)||
             (el->type == El_Slider)||
             (el->type == El_Vibrate)||
             (el->type == El_Transpose))
        {
            Trigger* tg_end = new Trigger;
            tg_end->el = el;
            tg_end->patt = pt;
            tg_end->tgact = tg_start;

            if(!skipaddtoelement)
                el->AddTrigger(tg_end);
            pt->AddInternalTrigger(tg_end);

            Locate_Trigger(tg_end);
        }

        if(el->IsInstance() && pt->basePattern->autopatt == false)
        {
            AddAutopatternInstance(tg_start, (NoteInstance*)el, true);
        }
    }
}

void MakeTriggersForElement(Element* el, Pattern* ptmain)
{
    Pattern* pt = ptmain->der_first;
    while(pt != NULL)
    {
        CreateElementTriggersPerPattern(pt, el);
        pt = pt->der_next;
    }
}

void UpdateEventsFrames(Pattern* pt)
{
    Event* ev = pt->first_ev;
    while(ev != NULL)
    {
        {
            ev->frame = Tick2Frame(ev->vick);
        }

        ev = ev->next;
    }
}

void CleanupEvents(Pattern* pt)
{
    Event* ev_d;
    Event* ev = pt->first_ev;
    while(ev != NULL)
    {
		if(ev->to_be_deleted == true)
		{
			// Do not delete yet if queued or if pattern's playback is still active
			if(ev->queued == false && !(ev->patt->pbk != NULL && ev->patt->pbk->dworking == true))
			{
				if(ev == pt->first_ev && ev == pt->last_ev)
				{
					pt->first_ev = pt->last_ev = NULL;
				}
				else if (ev == pt->first_ev)
				{
					pt->first_ev = ev->next;
					pt->first_ev->prev = NULL;
				}
				else if(ev == pt->last_ev)
				{
					pt->last_ev = ev->prev;
					pt->last_ev->next = NULL;
				}
				else
				{
					ev->prev->next = ev->next;
					ev->next->prev = ev->prev;
				}

				ev_d = ev;
				ev = ev->next;
				delete ev_d;
				continue;
			}
		}
        ev = ev->next;
    }
}

void RemoveTriggerFromEvent(Trigger* tg)
{
    Event** firstEv;
    Event** lastEv;
    if(tg->patt->basePattern== NULL || tg->patt->basePattern->autopatt == false)
    {
        firstEv = &field_pattern->first_ev;
        lastEv = &field_pattern->last_ev;
    }
    else
    {
        firstEv = &tg->patt->first_ev;
        lastEv = &tg->patt->last_ev;
    }

    Event* ev = tg->ev;
    bool last = ev->RemoveTrigger(tg);
    if(last == true)
    {
        if(Playing == true || aux_panel->playing == true || ev->queued == true || 
            (ev->patt->pbk != NULL && ev->patt->pbk->dworking == true))
        {
            ev->to_be_deleted = true;
        }
        else if(ev->queued == false)
        {
            if(ev == *firstEv && ev == *lastEv)
            {
               *firstEv = *lastEv = NULL;
            }
            else if (ev == *firstEv)
            {
               *firstEv = ev->next;
              (*firstEv)->prev = NULL;
            }
            else if(ev == *lastEv)
            {
               *lastEv = ev->prev;
              (*lastEv)->next = NULL;
            }
            else
            {
                ev->prev->next = ev->next;
                ev->next->prev = ev->prev;
            }
            jassert(ev->queued == false);
            delete ev;
        }
    }
}

void GlobalAddActiveTrigger(Trigger* tg)
{
    Trigger* tgg = first_global_active_trigger;
    while(tgg != NULL)
    {
        jassert(tgg != tg);

        tgg = tgg->act_next;
    }

    tg->listed_globally = true;
    if(last_global_active_trigger == NULL)
    {
        tg->act_prev = NULL;
        tg->act_next = NULL;
        first_global_active_trigger = tg;
    }
    else
    {
        last_global_active_trigger->act_next = tg;
        tg->act_prev = last_global_active_trigger;
        tg->act_next = NULL;
    }
    last_global_active_trigger = tg;
}

void GlobalRemoveActiveTrigger(Trigger* tg)
{
    if(tg == first_global_active_trigger && tg == last_global_active_trigger)
    {
        first_global_active_trigger= NULL;
        last_global_active_trigger= NULL;
    }
    else if(tg == first_global_active_trigger)
    {
        first_global_active_trigger= tg->act_next;
        first_global_active_trigger->act_prev = NULL;
    }
    else if(tg == last_global_active_trigger)
    {
        last_global_active_trigger= tg->act_prev;
        last_global_active_trigger->act_next = NULL;
    }
    else
    {
        if(tg->act_prev != NULL)
        {
            tg->act_prev->act_next = tg->act_next;
        }
        if(tg->act_next != NULL)
        {
            tg->act_next->act_prev = tg->act_prev;
        }
    }
    tg->listed_globally = false;
}

void ActivateCommandTrigger(Trigger* tg)
{
    tg->tgworking = true;
    tg->frame_phase = 0;
    if(last_active_command_trigger == NULL)
    {
        tg->loc_act_prev = NULL;
        tg->loc_act_next = NULL;
        first_active_command_trigger = tg;
    }
    else
    {
        last_active_command_trigger->loc_act_next = tg;
        tg->loc_act_prev = last_active_command_trigger;
        tg->loc_act_next = NULL;
    }
    last_active_command_trigger = tg;
}

void DeactivateCommandTrigger(Trigger* tg)
{
    if(tg == first_active_command_trigger && tg == last_active_command_trigger)
    {
        first_active_command_trigger = NULL;
        last_active_command_trigger = NULL;
    }
    else if(tg == first_active_command_trigger)
    {
        first_active_command_trigger = tg->loc_act_next;
        first_active_command_trigger->loc_act_prev = NULL;
    }
    else if(tg == last_active_command_trigger)
    {
        last_active_command_trigger = tg->loc_act_prev;
        last_active_command_trigger->loc_act_next = NULL;
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
    tg->tgworking = false;
}

// Activates effect-element's trigger
inline void ActivateSymbolTrigger(Trigger* stg)
{
    stg->tgworking = true;
    stg->frame_phase = 0;
    FXState* fxstate;
    if(stg->patt->basePattern->autopatt)
    {
        fxstate = &stg->patt->fxstate;
    }
    else
    {
        fxstate = &stg->trkdata->fxstate;
    }

	jassert(fxstate->t_mutecount >= 0);
    switch(stg->el->type)
    {
        case El_Break:
        {
            fxstate->t_break = true;
        }break;
        case El_Reverse:
        {
            fxstate->t_reversed = true;
        }break;
        case El_Transpose:
        {
            fxstate->t_freq_mult *= ((Transpose*)stg->el)->freq_mult;
        }break;
        case El_Mute:
        {
            fxstate->t_mutecount++;
        }break;
        case El_Slider:
        case El_Vibrate:
        {
            if(fxstate->freqtrig != NULL)
            {
                fxstate->freqtrig->loc_act_next = stg;
            }
            stg->loc_act_prev = fxstate->freqtrig;
            stg->loc_act_next = NULL;
            fxstate->freqtrig = stg;
        }break;
        case El_SlideNote:
        {
            SlideNote* sn = (SlideNote*)stg->el;
            stg->vol_base = sn->loc_vol->outval;
            stg->pan_base = sn->loc_pan->outval;
            stg->freq = sn->ed_note->freq;
            if(sn->ed_note->relative == true)
            {
                NoteInstance* pi = (NoteInstance*)stg->patt->parent_trigger->el;
                stg->freq = NoteToFreq(pi->ed_note->value + sn->ed_note->value);
            }

            if(stg->tgsparent != NULL)
            {
                stg->tgsparent->tgsactnum++;

                if(stg->tgsparent->tgsactive != NULL)
                {
                    stg->tgsparent->tgsactive->loc_act_next = stg;
                }
                stg->loc_act_prev = stg->tgsparent->tgsactive;
                stg->loc_act_next = NULL;
                stg->tgsparent->tgsactive = stg;
            }
            else
            {
                if(fxstate->freqtrig != NULL)
                {
                    fxstate->freqtrig->loc_act_next = stg;
                }
                stg->loc_act_prev = fxstate->freqtrig;
                stg->loc_act_next = NULL;
                fxstate->freqtrig = stg;
            }
        }break;
    }
}

inline void DeactivateSymbolTrigger(Trigger* stg)
{
    FXState* fxstate;
    if(stg->patt->basePattern->autopatt)
    {
        fxstate = &stg->patt->fxstate;
    }
    else
    {
        fxstate = &stg->trkdata->fxstate;
    }
    switch(stg->el->type)
    {
        case El_Break:
        {
            fxstate->t_break = false;
        }break;
        case El_Reverse:
        {
            fxstate->t_reversed = false;
        }break;
        case El_Transpose:
        {
            fxstate->t_freq_mult /= ((Transpose*)stg->el)->freq_mult;
        }break;
        case El_Mute:
        {
            fxstate->t_mutecount--;
        }break;
        case El_Slider:
        case El_Vibrate:
        {
            if(fxstate->freqtrig == stg)
            {
                fxstate->freqtrig = stg->loc_act_prev;
            }
            
            if(stg->loc_act_prev != NULL)
            {
                stg->loc_act_prev->loc_act_next = stg->loc_act_next;
                stg->loc_act_prev = NULL;
            }
            if(stg->loc_act_next != NULL)
            {
                stg->loc_act_next->loc_act_prev = stg->loc_act_prev;
                stg->loc_act_next = NULL;
            }
        }break;
        case El_SlideNote:
        {
            if(stg->tgsparent != NULL)
            {
                stg->tgsparent->tgsactnum--;

                if(stg->tgsparent->tgsactive == stg)
                {
                    stg->tgsparent->tgsactive = stg->loc_act_prev;
                }

                if(stg->loc_act_prev != NULL)
                {
                    stg->loc_act_prev->loc_act_next = stg->loc_act_next;
                    stg->loc_act_prev = NULL;
                }
                if(stg->loc_act_next != NULL)
                {
                    stg->loc_act_next->loc_act_prev = stg->loc_act_prev;
                    stg->loc_act_next = NULL;
                }
            }
            else
            {
                if(fxstate->freqtrig == stg)
                {
                    fxstate->freqtrig = stg->loc_act_prev;
                }
                
                if(stg->loc_act_prev != NULL)
                {
                    stg->loc_act_prev->loc_act_next = stg->loc_act_next;
                    stg->loc_act_prev = NULL;
                }
                if(stg->loc_act_next != NULL)
                {
                    stg->loc_act_next->loc_act_prev = stg->loc_act_prev;
                    stg->loc_act_next = NULL;
                }
            }
        }break;
    }

    stg->tgworking = false;
}

void DeactivatePatternTrigger(Trigger* ptg)
{
    if(ptg == first_active_pattern_trigger && ptg == last_active_pattern_trigger)
    {
        first_active_pattern_trigger = NULL;
        last_active_pattern_trigger = NULL;
    }
    else if(ptg == first_active_pattern_trigger)
    {
        first_active_pattern_trigger = ptg->loc_act_next;
        first_active_pattern_trigger->loc_act_prev = NULL;
    }
    else if(ptg == last_active_pattern_trigger)
    {
        last_active_pattern_trigger = ptg->loc_act_prev;
        last_active_pattern_trigger->loc_act_next = NULL;
    }
    else
    {
        if(ptg->loc_act_prev != NULL)
        {
            ptg->loc_act_prev->loc_act_next = ptg->loc_act_next;
        }
        if(ptg->loc_act_next != NULL)
        {
            ptg->loc_act_next->loc_act_prev = ptg->loc_act_prev;
        }
    }
    ptg->tgworking = false;
}

void ActivatePatternTrigger(Trigger* ptg)
{
    ptg->tgworking = true;
    ptg->frame_phase = 0;
    if(last_active_pattern_trigger == NULL)
    {
        ptg->loc_act_prev = NULL;
        ptg->loc_act_next = NULL;
        first_active_pattern_trigger = ptg;
    }
    else
    {
        last_active_pattern_trigger->loc_act_next = ptg;
        ptg->loc_act_prev = last_active_pattern_trigger;
        ptg->loc_act_next = NULL;
    }
    last_active_pattern_trigger = ptg;
}

Trigger::Trigger()
{
    Initialize();
}

void Trigger::Initialize()
{
    tgworking = activator = rev = skip = muted = broken = toberemoved = listed_globally = outsync = false;
    loc_act_prev = loc_act_next = ev_prev = ev_next = el_prev = el_next = group_prev = group_next = act_prev = act_next = NULL;
    aaIN = aaOUT = false;
    pslot = NULL;
    mcell = NULL;
    trkdata = NULL;
    pt_prev = pt_next = NULL;
    ev = NULL;
    el = NULL;
    patt = NULL;

    tgsparent = tgsactive = first_tgslide = NULL;
    tgsactnum = 0;

    voicenum = 0;

    mchan = NULL;
    mcell = NULL;

    freq_incr_sgn = 1;

    vol_val = pan_val =  0;
    frame_phase = 0;
    freq_incr_base = freq_incr_active = 0;
    wt_pos = 0;

    apatt_instance = NULL;

    lcount = 0;

    tgstate = TgState_Initial;
}

void Trigger::Activate()
{
    // reset remove flag
    toberemoved = false;
    if(tgworking == false && listed_globally == false)
    {
        switch(el->type)
        {
            case El_Samplent:
            case El_GenNote:
               ((NoteInstance*)el)->instr->ActivateTrigger(this);
                break;
            case El_Command:
            {
                Command* cmd = (Command*)el;
                if((cmd->cmdtype == Cmd_VolEnv || 
                    cmd->cmdtype == Cmd_PanEnv || 
                    cmd->cmdtype == Cmd_ParamEnv) && cmd->param != NULL) // long commands
                {
                    prev_value = -123.0f; // Undefined value for further redefinition
                    if(cmd->cmdtype == Cmd_ParamEnv)
                    {
                        if(cmd->scope->instr != NULL)
                        {
                            cmd->scope->instr->EnqueueParamEnvelopeTrigger(this);
                        }
                        else if(cmd->scope->eff != NULL)
                        {
                            cmd->scope->eff->EnqueueParamEnvelopeTrigger(this);
                        }
                        else
                        {
                            cmd->param->EnqueueEnvelopeTrigger(this);
                        }
                    }
                    else
                    {
                        cmd->param->EnqueueEnvelopeTrigger(this);
                    }
                }
                /*
                else if(cmd->cmdtype == Cmd_LocVolEnv || 
                        cmd->cmdtype == Cmd_LocPanEnv) // long commands
                {
                    tg->prev_value = -123.0f; // Undefined value for further redefinition
                    if(tg->patt->parent_trigger != NULL)
                    {
                        tg->patt->EnqueueEnvelopeTrigger(tg);
                    }
                }
                */
                ActivateCommandTrigger(this);
            }break;
            case El_Transpose:
            case El_Mute:
            case El_Break:
            case El_SlideNote:
            case El_Slider:
            case El_Reverse:
            case El_Repeat:
            case El_Vibrate:
                ActivateSymbolTrigger(this);
                break;
            case El_Pattern:
                ActivatePatternTrigger(this);
                break;
        }
        GlobalAddActiveTrigger(this);
    }
}

void Trigger::Deactivate()
{
    if(listed_globally)
    {
        GlobalRemoveActiveTrigger(this);
    }

    if(tgworking)
    {
        switch(el->type)
        {
            case El_Samplent:
            case El_GenNote:
                if(((NoteInstance*)el)->preview == true)
                {
                    pslot->state = PState_Stopped;
                }

                if(((NoteInstance*)el)->instr != NULL)
                    ((NoteInstance*)el)->instr->DeactivateTrigger(this);
                break;
            case El_Command:
            {
                Command* cmd = (Command*)el;
                if((cmd->cmdtype == Cmd_VolEnv || 
                    cmd->cmdtype == Cmd_PanEnv || 
                    cmd->cmdtype == Cmd_ParamEnv) && cmd->param != NULL) // long commands
                {
                    if(cmd->cmdtype == Cmd_ParamEnv)
                    {
                        if(cmd->scope->instr != NULL)
                        {
                            cmd->scope->instr->DequeueParamEnvelopeTrigger(this);
                        }
                        else if(cmd->scope->eff != NULL)
                        {
                            cmd->scope->eff->DequeueParamEnvelopeTrigger(this);
                        }
                        else
                        {
                            cmd->param->DequeueEnvelopeTrigger(this);
                        }
                    }
                    else
                    {
                        cmd->param->DequeueEnvelopeTrigger(this);
                    }
                }
                /*
                else if(cmd->cmdtype == Cmd_LocVolEnv || 
                        cmd->cmdtype == Cmd_LocPanEnv) // long commands
                {
                    if(tg->patt->parent_trigger != NULL)
                    {
                        tg->patt->DequeueEnvelopeTrigger(tg);
                    }
                }
                */
                DeactivateCommandTrigger(this);
    		}break;
            case El_Mute:
            case El_Break:
            case El_SlideNote:
            case El_Slider:
            case El_Reverse:
            case El_Repeat:
            case El_Vibrate:
            case El_Transpose:
                DeactivateSymbolTrigger(this);
                break;
            case El_Pattern:
                DeactivatePatternTrigger(this);
                break;
        }
    }
}

void Trigger::Release()
{
    tgstate = TgState_Release;
}

void Trigger::SoftFinish()
{
    tgstate = TgState_SoftFinish;
}

bool Trigger::IsInstant()
{
    switch(el->type)
    {
        case El_Break:
        case El_Reverse:
        case El_Repeat:
            return true;
        default:
            return false;
    }
}

bool Trigger::IsSymbol()
{
    switch(el->type)
    {
        case El_Mute:
        case El_Break:
        case El_SlideNote:
        case El_Slider:
        case El_Reverse:
        case El_Repeat:
        case El_Vibrate:
        case El_Transpose:
            return true;
        default:
            return false;
    }
}

bool Trigger::IsOutOfPlayback()
{
    if(ev->patt->pbk->currFrame < ev->frame || 
       ev->patt->pbk->currFrame > ev->frame + el->frame_length)
        return true;
    else
        return false;
}

bool Trigger::CheckRemoval()
{
    if(toberemoved == true || !el->IsPresent() || IsInstant() || (IsSymbol() && IsOutOfPlayback()))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void ProcessPlacedEnvelopes(Pattern* pt, long buffframe, long num_frames, long curr_frame)
{
    Command* cmd;
    Envelope* env;
    Trigger* tg = first_active_command_trigger;
    Trigger* tgnext;
    while(tg != NULL)
    {
        tgnext = tg->loc_act_next;
        if(tg->el->IsPresent() && (tg->ev->patt == pt || tg->patt == pt))
        {
            cmd = (Command*)tg->el;
            if(curr_frame < tg->ev->frame)
            {
                tg->Deactivate();
            }
            else if(cmd->cmdtype != Cmd_LocVolEnv && cmd->cmdtype != Cmd_LocPanEnv /* && cmd->cmdtype != Cmd_ParamEnv*/)
            {
                env = (Envelope*)cmd->paramedit;
                tg->frame_phase = (curr_frame - tg->ev->frame);
                if(tg->frame_phase > env->frame_length)
                {
                    tg->Deactivate();
                }
                else
                {
                    env->ProcessBuffer1(curr_frame, buffframe, num_frames, 1, tg->ev->frame, tg, NULL);
                }
            }
        }
        tg = tgnext;
    }
}

void PreInitSamples(tframe frame, Pattern* pt, Event* firstev)
{
    Trigger* tg;
    Event* ev = firstev;
    while(ev != NULL)
    {
        tg = ev->tg_first;
        while(tg != NULL)
        {
            if((pt == field_pattern || tg->patt == pt) && 
                tg->el->IsPresent() && tg->patt->IsPresent() && 
                tg->el->IsInstance() && 
                tg->activator)
            {
                Samplent* sm = (Samplent*)tg->el;
                tframe smend = (tframe)(tg->ev->frame + sm->frame_length);
                if(tg->ev->frame < frame && smend > frame)
                {
                    tg->Activate();
                    tg->frame_phase = frame - tg->ev->frame;
                    tg->wt_pos = tg->freq_incr_base*tg->frame_phase;
                    tg->env_phase1 = one_divided_per_sample_rate*tg->frame_phase;
                }
            }
            tg = tg->ev_next;
        }
        ev = ev->next;
    }
}

void PreInitEnvelopes(tframe frame, Pattern* pt, Event* firstev, bool activate_env, bool paraminit)
{
    if(paraminit)
    {
        Parameter* param = firstParam;
        while(param != NULL)
        {
    		param->lastsetframe = 0;
            if(param->envtweaked)
            {
                param->ResetToInitial();
            }
            param = param->gnext;
        }
    }

    Command* cmd;
    Trigger* tg;
    Envelope* env;
    Event* ev = firstev;
    while(ev != NULL)
    {
        tg = ev->tg_first;
        while(tg != NULL)
        {
            if((pt == field_pattern || tg->patt == pt) && 
                tg->el->IsPresent() && tg->patt->IsPresent() && 
                tg->el->type == El_Command && 
                tg->activator)
            {
                cmd = (Command*)tg->el;
                if(cmd->IsEnvelope() && cmd->param != NULL)
                {
                    cmd->param->UnblockEnvAffect(); // to allow env. value set
                    env = (Envelope*)cmd->paramedit;
                    tframe envend = tframe(tg->ev->frame + env->len*frames_per_tick);
                    if(tg->ev->frame <= frame && envend >= frame)
                    {
                        cmd->param->SetValueFromEnvelope(env->GetValue(frame - tg->ev->frame), env);
                        if(activate_env)
                        {
                            tg->Activate();
                            tg->frame_phase = frame - tg->ev->frame;
                        }
                        cmd->param->lastsetframe = frame;
                    }
                    else if(envend < frame && 
                            envend > cmd->param->lastsetframe)
                    {
                        cmd->param->SetValueFromEnvelope(env->last_value, env);
                        cmd->param->lastsetframe = envend;
                    }
                }
            }
            tg = tg->ev_next;
        }
        ev = ev->next;
    }
}

void ProcessTriggers(Event* queued_ev, Pattern* pt, bool deactonly = false)
{
    Command*    cmd;
    Trigger*    tg;

    tg = queued_ev->tg_first;
    while(tg != NULL)
    {
        if(((pt == field_pattern && !tg->patt->muted) || tg->patt == pt) && tg->patt->IsPresent() && tg->el->IsPresent())
        {
            if(tg->patt != field_pattern && tg->activator && 
                   (queued_ev->vick < tg->patt->start_tick || queued_ev->vick >= tg->patt->end_tick))
            {
                // skip out-of-pattern activators
            }
            else if(!(deactonly && tg->activator)) // skip activators if required
            {
                if(tg->el->IsInstance())
                {
                    if(tg->activator == true)
                    {
                        NoteInstance* ii = (NoteInstance*)tg->el;
                        if(tg->tgworking == true)
                        {
                            ii->instr->FlowTriggers(tg, tg);
                            if(tg->tgworking == true) // it could change
                                tg->Deactivate();
                        }
                        tg->Activate();

                        if(tg->apatt_instance != NULL)
                        {
                            if(tg->apatt_instance->pbk->dworking == true)
                            {
                                DeactivatePlayback(tg->apatt_instance->pbk);
                            }
                            ActivatePlayback(tg->apatt_instance->pbk);
                        }
                    }
                    else
                    {
                        if(tg->tgact->tgworking == true)
                        {
                            tg->tgact->Deactivate();
                        }
                    }
                }
                else if(tg->el->type == El_Command)
                {
                    cmd = (Command*)tg->el;
                    if(((cmd->cmdtype == Cmd_VolEnv || 
                        cmd->cmdtype == Cmd_PanEnv || 
                        cmd->cmdtype == Cmd_ParamEnv) && cmd->param != NULL) ||
                       (cmd->cmdtype == Cmd_LocVolEnv || cmd->cmdtype == Cmd_LocPanEnv))
                    {
                        if(tg->activator == true)
                        {
                            if(tg->tgworking == true)
                            {
                                tg->Deactivate();
                            }
                            tg->Activate();
                        }
                        else
                        {
                            if(tg->tgact->tgworking == true)
                            {
                                tg->tgact->Deactivate();
                            }
                        }
                    }
                    else // instant commands
                    {
                        cmd->ApplyInstant();
                    }
                }
                else if(tg->el->type == El_Pattern)
                {
                    // Do nothing here
                }
                else if(tg->el->type == El_Mute ||
                        tg->el->type == El_Break ||
                        tg->el->type == El_SlideNote ||
                        tg->el->type == El_Slider ||
                        tg->el->type == El_Reverse ||
                        tg->el->type == El_Repeat ||
                        tg->el->type == El_Vibrate ||
                        tg->el->type == El_Transpose)
                {
                    // symbol elements
                    if(tg->activator == true)
                    {
                        if(tg->tgworking == true)
                        {
                            tg->Deactivate();
                        }
                        tg->Activate();
                    }
                    else
                    {
                        if(tg->tgact->tgworking == true)
                        {
                            tg->tgact->Deactivate();
                        }
                    }
                }
            }
        }

        tg = tg->ev_next;
    }
}

Playback::Playback()
{
    looped = false;
    currFrame = 0;
    currTick = 0;
    rng_start_frame = rng_end_frame = 0;
    ev_count_down = -1;
    queued_ev = NULL;
    first_ev = NULL;
    playPatt = NULL;
    prev = next = NULL;
    at_start = true;
    dworking = false;
    pactive = false;
    tsync_block = false;
}

Playback::Playback(Pattern* ppt)
{
    looped = false;
    currFrame = 0;
    currTick = 0;
    rng_start_frame = rng_end_frame = 0;
    ev_count_down = -1;
    queued_ev = NULL;
    first_ev = NULL;
    prev = next = NULL;
    at_start = true;
    dworking = false;
    pactive = false;
    tsync_block = false;
    SetPlayPatt(ppt);
}

void Playback::SetPlayPatt(Pattern*    pPt)
{
    playPatt = pPt;
    AlignRangeToPattern();
    UpdateQueuedEv();
}

void Playback::ResetPos()
{
    currFrame = rng_start_frame;
    UpdateQueuedEv();
}

void Playback::UnqueueAll()
{
    Event* ev;
    if(playPatt->basePattern == NULL || playPatt->basePattern->autopatt == false)
        ev = field_pattern->first_ev;
    else
        ev = playPatt->first_ev;

    while(ev != NULL)
    {
        ev->queued = false;
        ev = ev->next;
    }
}

/*==================================================================================================
BRIEF: This function initialises queued event and frame countdown according to current playback 
frame (currBaseFrame).

PARAMETERS:
[IN]
N/A
[OUT]
N/A

OUTPUT: N/A
==================================================================================================*/
void Playback::UpdateQueuedEv()
{
    if(playPatt != NULL)
    {
        UnqueueAll();

        Event* ev;
        if(playPatt->basePattern == NULL || playPatt->basePattern->autopatt == false)
            ev = field_pattern->first_ev;
        else
            ev = playPatt->first_ev;

        while(ev != NULL)
        {
            if(ev->to_be_deleted == false && currFrame <= ev->frame)
            {
                ev_count_down = long(ev->frame - currFrame);
                queued_ev = ev;
                queued_ev->queued = true;
                queued_ev->framecount = ev_count_down;
                break;
            }
            ev = ev->next;
        }

        if(ev == NULL)
        {
            queued_ev = NULL;
            if(looped == false)
            {
                ev_count_down = -1;
            }
            else
            {
                ev_count_down = rng_end_frame - currFrame;
            }
        }
    }
}

/*==================================================================================================
BRIEF: Ticks dcount number of frames for this playback. Calling function is responsible to provide 
dcount number less or equal than number of frames in currently queued event. After ev_count_down 
becomes zero, we process currently queued event, requeue event and process internal envelopes. 
If the playback is ranged, then we also check for wrapping.

PARAMETERS:
[IN]
    nc - start frame in buffer
    dcount - number of frames to tick
    fpb - frames per buffer
[OUT]
N/A

OUTPUT: N/A
==================================================================================================*/
void Playback::TickFrame(long nc, long dcount, long fpb)
{
    if(playPatt != NULL)
    {
        if(nc == 0 || ev_count_down == 0)
        {
            // Process currently queued event's triggers and requeue event
            if(ev_count_down == 0 && queued_ev != NULL)
            {
                ProcessTriggers(queued_ev, playPatt);
                Requeue(true);
            }

            long num_frames;
            if(ev_count_down > (fpb - nc) || ev_count_down < 0)
            {
                num_frames = fpb - nc;
            }
            else
            {
                num_frames = (long)ev_count_down;
            }
        
            if(num_frames > 0)
            {
                ProcessPlacedEnvelopes(playPatt, nc, num_frames, currFrame);
            }
        }

        if(ev_count_down > 0)
        {
            ev_count_down -= dcount;
            if(queued_ev != NULL)
                queued_ev->framecount = ev_count_down;
        }
    }
    currFrame += dcount;
    currTick = currFrame*one_divided_per_frames_per_tick;

    // Wrap if needed
    if(rng_end_frame > 0 && currFrame >= rng_end_frame)
    {
        if(looped == false)
        {
            queued_ev = NULL;
            ev_count_down = -1;
            if(dworking == true)
                dworking = false;
        }
        else
        {
            // Which cases is this for?
            if(ev_count_down == 0 && queued_ev != NULL && queued_ev->frame == rng_end_frame)
                ProcessTriggers(queued_ev, playPatt, true);

            SetCurrFrameNOR(rng_start_frame);
        }
    }
}

long Playback::Requeue(bool change)
{
    long evcnt;
    Event* qev = queued_ev;
    while(qev != NULL && (qev->to_be_deleted == true || 
                         (evcnt = qev->frame - currFrame) <= 0))
    {
        qev = qev->next;
    }
    
    if(qev != NULL)
    {
        if(looped == true && (rng_end_frame - currFrame) < evcnt)
        {
            evcnt = rng_end_frame - currFrame;
        }
    }
    else
    {
        if(looped == true)
        {
            evcnt = rng_end_frame - currFrame;
        }
        else
        {
            evcnt = -1;
        }
    }

    if(change)
    {
        ev_count_down = evcnt;
        if(queued_ev != NULL)
        {
            queued_ev->queued = false;
        }
        queued_ev = qev;
        if(queued_ev != NULL)
        {
            queued_ev->queued = true;
            queued_ev->framecount = ev_count_down;
        }
    }

    return evcnt;
}

void Playback::GetSmallestCountDown(long* count)
{
    long newcount;
    if(ev_count_down != 0)
    {
        newcount = ev_count_down;
    }
    else
    {
        newcount = Requeue(false);
        if(queued_ev != NULL)
        {
            Trigger* tg = queued_ev->tg_first;
            while(tg != NULL)
            {
                if(tg->apatt_instance != NULL && tg->apatt_instance->first_ev != NULL)
                {
					//jassert(tg->ai->first_ev->frame >= 0);
					if(tg->apatt_instance->first_ev->frame > 0 && tg->apatt_instance->first_ev->frame < newcount)
						newcount = tg->apatt_instance->first_ev->frame;
                }
                tg = tg->ev_next;
            }
        }
    }

    if(newcount > 0 && newcount < *count)
    {
        *count = newcount;
    }
}

void Playback::SetRanges(long start, long end)
{
    rng_start_frame = start;
    rng_end_frame = end;
}

void Playback::SetLooped(bool loop)
{
    looped = loop;
}

void Playback::SetCurrFrame(long frame)
{
    tsync_block = false;
    currFrame = frame;
    currFrame_tsync = (double)currFrame;
    currTick = currFrame*one_divided_per_frames_per_tick;
    UpdateQueuedEv();
}

void Playback::SetCurrTick(double tick)
{
    if(tick >= 0)
    {
        currTick = tick;
        SetFrameToTick();
    }
}

// Alternative SetCurrFrame method that doesn't change timesynced value to keep it free running and defends it
// with tsync_block flag
void Playback::SetCurrFrameNOR(long frame)
{
    tsync_block = true;
    currFrame = frame;
    currTick = currFrame*one_divided_per_frames_per_tick;
    UpdateQueuedEv();
}

void Playback::SetFrameToTick()
{
    currFrame = Tick2Frame((float)currTick);
    currFrame_tsync = currFrame_tsync;
    UpdateQueuedEv();
}

void Playback::AlignRangeToPattern()
{
    if(playPatt != NULL)
    {
        SetRanges(playPatt->frame, playPatt->end_frame);
        if(pactive && currFrame < rng_start_frame)
        {
            SetCurrFrame(rng_start_frame);
        }
    }
}

void Playback::ResetLooped()
{
    looped = false;
}

void Playback::SetActive()
{
    pactive = true;
}

void Playback::SetInactive()
{
    pactive = false;
}

void DeactivatePlayback(Playback* pbk)
{
    if(pbk == first_active_playback && pbk == last_active_playback)
    {
        first_active_playback = NULL;
        last_active_playback = NULL;
    }
    else if(pbk == first_active_playback)
    {
        first_active_playback = pbk->next;
        first_active_playback->prev = NULL;
    }
    else if(pbk == last_active_playback)
    {
        last_active_playback = pbk->prev;
        last_active_playback->next = NULL;
    }
    else
    {
        if(pbk->prev != NULL)
        {
            pbk->prev->next = pbk->next;
        }
        if(pbk->next != NULL)
        {
            pbk->next->prev = pbk->prev;
        }
    }
    pbk->pactive = false;
    pbk->dworking = false;
}

void ActivatePlayback(Playback* pbk)
{
    if(pbk->playPatt != NULL && pbk->playPatt->to_be_deleted == false)
    {
        pbk->ResetPos();
        pbk->pactive = true;
        pbk->dworking = true;
        if(last_active_playback == NULL)
        {
            pbk->prev = NULL;
            pbk->next = NULL;
            first_active_playback = pbk;
        }
        else
        {
            last_active_playback->next = pbk;
            pbk->prev = last_active_playback;
            pbk->next = NULL;
        }
        last_active_playback = pbk;
    }
}

void DisableAllPlaybacks()
{
    Playback* pbk = first_active_playback;
    while(pbk != NULL)
    {
        pbk->dworking = false;
        pbk = pbk->next;
    }
}

