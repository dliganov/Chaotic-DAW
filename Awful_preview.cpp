#include "stdafx.h"
#include "resource.h"

#include "awful_preview.h"
#include "awful_cursorandmouse.h"
#include "awful_utils_common.h"

extern void         Preview_Init();
extern bool         Preview_StopPerPattern(Element* el);
extern void         Preview_MouseRelease();
extern void         Preview_FinishAll();
extern void         Preview_ReleaseAll();
extern void         Preview_Release(Instrument* i, int note);
extern void         Preview_Release(int key);
extern int          Preview_Add(Instance* ii, Instrument* i, int key, int note, Pattern* pt, Trk* trk, Mixcell* mcell, bool noauto, bool setrelative);
int                 Preview_GetFreeSlot(int key);
extern void         Preview_ReleaseData(unsigned int ic);
extern void         Preview_CheckStates(long num_frames);
extern void         Preview_ForceCleanForInstrument(Instrument* instr);


PreviewSlot         PrevSlot[MAX_PREVIEW_ELEMENTS];
PreviewSlot         PrevMouse;
HANDLE              hPreviewMutex;

unsigned int        NumPrevs;

bool                Release = false;
bool                Finish = false;

void Preview_Init()
{
    for(int ic = 0; ic < MAX_PREVIEW_ELEMENTS; ic++)
    {
        PrevSlot[ic].ii = NULL;
        PrevSlot[ic].state = PState_Free;
		PrevSlot[ic].key = -1;
        PrevSlot[ic].keybound_pianokey = false;
    }
    NumPrevs = 0;
    hPreviewMutex = CreateMutex(NULL, FALSE, NULL);
}

bool Preview_StopPerPattern(Element* el)
{
    bool any_stopped = false;
    for (int ic = 0; ic < MAX_PREVIEW_ELEMENTS; ic++)
    {
        if(PrevSlot[ic].state != PState_Free && 
           PrevSlot[ic].state != PState_Inactive && 
           PrevSlot[ic].trig.patt == el)
        {
            PrevSlot[ic].state = PState_Stopped;
            any_stopped = true;
        }
    }
    return any_stopped;
}

void Preview_ForceCleanForInstrument(Instrument* instr)
{
	for(int ic = 0; ic < MAX_PREVIEW_ELEMENTS; ic++)
    {
        if(PrevSlot[ic].ii != NULL && PrevSlot[ic].ii->instr == instr)
        {
            Preview_ReleaseData(ic);
        }
    }
}

int Preview_GetFreeSlot(int key)
{
	if(key != -1)
	{
		for(int ic = 0; ic < MAX_PREVIEW_ELEMENTS; ic++)
		{
			if(PrevSlot[ic].key == key)
			{
				return 0xFFFF;
			}
		}
	}

	for(int ic = 0; ic < MAX_PREVIEW_ELEMENTS; ic++)
    {
        if(PrevSlot[ic].state == PState_Inactive)
        {
            Preview_ReleaseData(ic);
        }

        if(PrevSlot[ic].state == PState_Free)
        {
            return ic;
        }
    }
    return 0xFFFF;
}

extern int Preview_Add(Instance* ii, Instrument* i, int key, int note, Pattern* pt, Trk* trk, Mixcell* mcell, bool noauto, bool setrelative)
{
    if(ii == NULL && i == NULL)
    {
        return 0xFFFF;
    }
    else
    {
        int pnum = Preview_GetFreeSlot(key);
        if(pnum != 0xFFFF)
        {
            PreviewSlot* pslot = &PrevSlot[pnum];
            pslot->sustainable = true;
            pslot->mouse_preview = true;
            pslot->key = key;
            if(ii != NULL)
            {
                pslot->ii = (Instance*)ii->Clone(false);
            }
            else
            {
                pslot->ii = CreateNote(i, false);
				if(setrelative)
				{
					pslot->ii->ed_note->SetRelative(true);
				}
                if(i->type == Instr_Sample)
                {
                    // Avoid partial length for instrument-only preview
                   ((Samplent*)pslot->ii)->touchresized = false;
                }
            }
            pslot->ii->preview = true;

            if(note != -1)
            {
                pslot->ii->ed_note->SetValue(note);
            }

            // Apply pattern to the instance
            if(pt != NULL)
                pslot->ii->patt = pt->OrigPt;
            else
                pslot->ii->patt = field;

            pslot->ii->Update();

            // Apply other custom stuff
            if(trk != NULL)
                pslot->ii->field_trkdata = trk;
            else if(ii == NULL)
                pslot->ii->field_trkdata = i->ptrk;
			else if(ii != NULL)
                pslot->ii->field_trkdata = ii->field_trkdata;

            pslot->ii->track_params = pslot->ii->field_trkdata->params;

            memset(&pslot->trig, 0, sizeof(Trigger));
            Trigger* tg = &pslot->trig;
            tg->tgstate = TgState_Sustain;
            tg->el = pslot->ii;
            if(pt != NULL)
                tg->patt = pt;
            else
                tg->patt = field;

            tg->trkdata = pslot->ii->field_trkdata;

            // Pattern trackdata always overrides ii trackdata
			if(tg->patt->field_trkdata != NULL)
			{
				tg->trkdata = tg->patt->field_trkdata;
			}

            tg->pslot = pslot;
            ev0->AddTrigger(tg);

            if(mcell != NULL)
            {
                tg->mcell = ((Instance*)tg->el)->instr->fxcell;
            }

            // Add autoinstance without external bindings (for safety)
            if(!noauto)
			    AddAutoInstance(tg, pslot->ii, false);

            pslot->state = PState_Ready;
            NumPrevs++;
        }

        return pnum;
    }
}

void  Preview_Release(Instrument* i, int note)
{
	bool released = false;
    for(int ic = 0; ic < MAX_PREVIEW_ELEMENTS; ic++)
    {
        if((PrevSlot[ic].trig.tgstate != TgState_Release) && 
           (PrevSlot[ic].ii != NULL && PrevSlot[ic].ii->ed_note->value == note))
        {
            if(PrevSlot[ic].state == PState_Playing)
            {
                released = true;
                PrevSlot[ic].trig.Release();
            }
            else if(PrevSlot[ic].state == PState_Ready)
                PrevSlot[ic].sustainable = false;

            if(PrevSlot[ic].state == PState_Playing || PrevSlot[ic].state == PState_Ready)
            {
				// Record a note for generator here as it's released
                if(Recording && C.patt != field && PrevSlot[ic].ii->type != Instr_Sample)
                {
                    //Place_Note(((Instance*)(PrevSlot[ic].trig.el))->instr,
                    //        note, PrevSlot[ic].trig.vol_val, PrevSlot[ic].start_frame, pbMain->currFrame - pbAux->currFrame);

                    //if(PrevSlot[ic].state == PState_Ready)
                    //	jassertfalse;

                    tframe frame1 = PrevSlot[ic].start_frame - C.patt->StartFrame();
                    tframe frame2 = pbMain->currFrame - C.patt->StartFrame();

                    if(frame2 <= frame1)
                        frame2 = frame1 + 44;
                    Grid_PutInstanceSpecific(PrevSlot[ic].ii->patt, 
                                            ((Instance*)(PrevSlot[ic].trig.el))->instr, 
                                              PrevSlot[ic].ii->ed_note->value + 1, 
                                              PrevSlot[ic].ii->loc_vol->val, 
                                              frame1,
                                              frame2);

                    float newlength = Frame2Tick(frame2);
                    if(newlength > C.patt->tick_length)
                        C.patt->SetTickLength(newlength);

                    R(Refresh_Grid);
                    MC->poso->ScheduleRefresh();
                }
            }
        }
    }

	//if(!released)
	//	jassertfalse;
}

void  Preview_Release(int key)
{
    if(key != -1)
    {
        for(int ic = 0; ic < MAX_PREVIEW_ELEMENTS; ic++)
        {
            if((PrevSlot[ic].state == PState_Playing)&&
               (PrevSlot[ic].trig.tgstate != TgState_Release) && 
               (PrevSlot[ic].key == key))
            {
                PrevSlot[ic].trig.Release();
            }
        }
    }
}

inline void Preview_SetNoteVolume(unsigned int note, float volume)
{
    for(int ic = 0; ic < MAX_PREVIEW_ELEMENTS; ic++)
    {
        if(!(PrevSlot[ic].state == PState_Inactive || PrevSlot[ic].state == PState_Free) &&
            (PrevSlot[ic].trig.tgstate == TgState_Sustain) &&
            (PrevSlot[ic].ii->ed_note->value == note))
        {
            PrevSlot[ic].ii->ed_vol->fvalue = volume;
            PrevSlot[ic].ii->loc_vol->outval = volume;
            PrevSlot[ic].ii->loc_vol->val = volume;
        }
    }
}

inline void Preview_SetVolume(float volume)
{
    for(int ic = 0; ic < MAX_PREVIEW_ELEMENTS; ic++)
    {
        if(!(PrevSlot[ic].state == PState_Inactive || PrevSlot[ic].state == PState_Free) &&
            (PrevSlot[ic].trig.tgstate == TgState_Sustain))
        {
            PrevSlot[ic].ii->ed_vol->fvalue = volume;
            PrevSlot[ic].ii->loc_vol->outval = volume;
            PrevSlot[ic].ii->loc_vol->val = volume;
        }
    }
}

void Preview_ReleaseData(unsigned int ic)
{
    if(ic < MAX_PREVIEW_ELEMENTS)
    {
        PrevSlot[ic].trig.Deactivate();
        if(PrevSlot[ic].trig.ai != NULL)
        {
            if(PrevSlot[ic].trig.ai->IsPlaybackActive())
            {
                DeactivatePlayback(PrevSlot[ic].trig.ai->pbk);
            }
            Del_Pattern_Internal_Triggers(PrevSlot[ic].trig.ai, true);
            delete PrevSlot[ic].trig.ai;
            PrevSlot[ic].trig.ai = NULL;
        }

        delete PrevSlot[ic].ii;
        PrevSlot[ic].ii = NULL;
        PrevSlot[ic].state = PState_Free;
        PrevSlot[ic].trig.patt = NULL;
		PrevSlot[ic].key = -1;
    }
}

void Preview_MouseRelease()
{
    M.pianokey_pressed = false;
    M.pianokey_slot = 0xFFFF;

    Release = true;
    /*
    for(int ic = 0; ic < MAX_PREVIEW_ELEMENTS; ic++)
    {
        if(PrevSlot[ic].mouse_preview && PrevSlot[ic].state == PState_Playing)
        {
            PrevSlot[ic].trig.tgstate = TgState_Release;
        }
    }
    */
}

void Preview_FinishAll()
{
    Finish = true;
    /*
    for(int ic = 0; ic < MAX_PREVIEW_ELEMENTS; ic++)
    {
        if(PrevSlot[ic].mouse_preview && PrevSlot[ic].state == PState_Playing)
        {
            PrevSlot[ic].trig.tgstate = TgState_Finished;
        }
    }*/
}

void Preview_ReleaseAll()
{
    for(int ic = 0; ic < MAX_PREVIEW_ELEMENTS; ic++)
    {
        if(PrevSlot[ic].state == PState_Ready || PrevSlot[ic].state == PState_Playing)
        {
            PrevSlot[ic].sustainable = false;
            PrevSlot[ic].trig.Release();
        }
    }
}

extern void Preview_StopAll()
{
    for(int ic = 0; ic < MAX_PREVIEW_ELEMENTS; ic++)
    {
        if(PrevSlot[ic].state == PState_Ready || PrevSlot[ic].state == PState_Playing)
        {
            PrevSlot[ic].state = PState_Stopped;
        }
    }

    Instrument *p_cur_instr = first_instr;
    while (p_cur_instr != NULL)
    {
        if (p_cur_instr->type == Instr_VSTPlugin)
        {
            ((VSTGenerator*)(p_cur_instr))->StopAllNotes();
        }
        p_cur_instr = p_cur_instr->next;
    }
}

void Preview_CheckStates(long num_frames)
{
    bool doR = false, doF = false;
    if(Release)
        doR = true; // Use flags to ensure release and finish only from start of this function
    if(Finish)
        doF = true;

	if(NumPrevs > 0)
	{
		for (int ic = 0; ic < MAX_PREVIEW_ELEMENTS; ic++)
		{
			if(PrevSlot[ic].state == PState_Playing)
			{
				if(PrevSlot[ic].keybound_pianokey == false)
				{
					if(Release && doR)
						PrevSlot[ic].trig.Release();
					else if(Finish && doF)
						PrevSlot[ic].trig.SoftFinish();
				}
			}
			else if(PrevSlot[ic].state == PState_Ready)
			{
				Trigger* tg = &PrevSlot[ic].trig;
				Instrument* instr = ((Instance*)tg->el)->instr;

				if(tg->tworking == true)
				{
                    tg->Deactivate();
				}

                tg->Activate();
				if(PrevSlot[ic].sustainable == false || 
				   (Release && doR && PrevSlot[ic].ii->type != El_Samplent))
				{
					tg->Release();
				}
				if(tg->ai != NULL)
				{
					if(tg->ai->pbk->dworking == true)
					{
						DeactivatePlayback(tg->ai->pbk);
					}
					ActivatePlayback(tg->ai->pbk);
				}

				tg->pslot->state = PState_Playing;
			}
			else if(PrevSlot[ic].state == PState_Stopped)
			{
				Trigger* tg = &PrevSlot[ic].trig;
				if(tg->tworking)
                    tg->Deactivate();
				if(tg->ai == NULL || !tg->ai->IsPlaybackActive())
				{
					Instrument* instr = ((Instance*)tg->el)->instr;
					PrevSlot[ic].state = PState_Inactive;
					NumPrevs--;
				}
			}
		}
	}

    if(Release && doR)
        Release = false;
    if(Finish && doF)
        Finish = false;
}

