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
extern int          Preview_Add(NoteInstance* ii, Instrument* i, int key, int note, Pattern* pt, Trk* trk, Mixcell* mcell, bool noauto, bool setrelative);
int                 Preview_GetFreeSlot(int key);
extern void         Preview_ReleaseData(unsigned int ic);
extern void         Preview_CheckStates(long num_frames);
extern void         Preview_ForceCleanForInstrument(Instrument* instr);


PreviewSlot         PrevSlot[MAX_PREVIEW_ELEMENTS];
PreviewSlot         PrevMouse;
HANDLE              hPreviewMutex;

unsigned int        NumPrevs;

bool                preview_mouse_release = false;
bool                preview_finish_all = false;

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
           PrevSlot[ic].trigger.patt == el)
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

extern int Preview_Add(NoteInstance* ii, Instrument* i, int key, int note, Pattern* pt, Trk* trk, Mixcell* mcell, bool noauto, bool setrelative)
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
                pslot->ii = (NoteInstance*)ii->Clone(false);
            }
            else
            {
                pslot->ii = CreateElement_Note(i, false);
                if(setrelative)
                {
                    pslot->ii->ed_note->SetRelative(true);
                }

                 // Avoid partial length for instrument-only preview
                pslot->ii->ResizeTouch(false);
            }
            pslot->ii->preview = true;

            if(note != -1)
            {
                pslot->ii->ed_note->SetValue(note);
            }

            // Apply pattern to the instance
            if(pt != NULL)
                pslot->ii->patt = pt->basePattern;
            else
                pslot->ii->patt = field_pattern;

            pslot->ii->Update();

            // Apply other custom stuff
            if(trk != NULL)
                pslot->ii->field_trkdata = trk;
            else if(ii == NULL)
                pslot->ii->field_trkdata = i->ptrk;
            else if(ii != NULL)
                pslot->ii->field_trkdata = ii->field_trkdata;

            pslot->ii->track_params = pslot->ii->field_trkdata->params;

            memset(&pslot->trigger, 0, sizeof(Trigger));
            Trigger* tg = &pslot->trigger;
            tg->tgstate = TgState_Sustain;
            tg->el = pslot->ii;
            if(pt != NULL)
                tg->patt = pt;
            else
                tg->patt = field_pattern;

            tg->trkdata = pslot->ii->field_trkdata;

            // Pattern trackdata always overrides ii trackdata
            if(tg->patt->field_trkdata != NULL)
            {
                tg->trkdata = tg->patt->field_trkdata;
            }

            tg->pslot = pslot;
            ev0->AddTrigger(tg);

            // Add autoinstance without external bindings (for safety)
            if(!noauto)
                AddAutopatternInstance(tg, pslot->ii, false);

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
        if((PrevSlot[ic].trigger.tgstate != TgState_Release) && 
           (PrevSlot[ic].ii != NULL && PrevSlot[ic].ii->ed_note->value == note))
        {
            if(PrevSlot[ic].state == PState_Playing)
            {
                released = true;
                PrevSlot[ic].trigger.Release();
            }
            else if(PrevSlot[ic].state == PState_Ready)
                PrevSlot[ic].sustainable = false;

            if(PrevSlot[ic].state == PState_Playing || PrevSlot[ic].state == PState_Ready)
            {
				// Record a note for generator here as it's released
                if(Recording && C.patt != field_pattern && PrevSlot[ic].ii->type != Instr_Sample)
                {
                    //Place_Note(((Instance*)(PrevSlot[ic].trig.el))->instr,
                    //        note, PrevSlot[ic].trig.vol_val, PrevSlot[ic].start_frame, pbMain->currFrame - pbAux->currFrame);

                    //if(PrevSlot[ic].state == PState_Ready)
                    //	jassertfalse;

                    tframe frame1 = PrevSlot[ic].start_frame - C.patt->StartFrame();
                    tframe frame2 = pbkMain->currFrame - C.patt->StartFrame();

                    if(frame2 <= frame1)
                        frame2 = frame1 + 44;
                    Grid_PutInstanceSpecific(PrevSlot[ic].ii->patt, 
                                            ((NoteInstance*)(PrevSlot[ic].trigger.el))->instr, 
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
               (PrevSlot[ic].trigger.tgstate != TgState_Release) && 
               (PrevSlot[ic].key == key))
            {
                PrevSlot[ic].trigger.Release();
            }
        }
    }
}

inline void Preview_SetNoteVolume(unsigned int note, float volume)
{
    for(int ic = 0; ic < MAX_PREVIEW_ELEMENTS; ic++)
    {
        if(!(PrevSlot[ic].state == PState_Inactive || PrevSlot[ic].state == PState_Free) &&
            (PrevSlot[ic].trigger.tgstate == TgState_Sustain) &&
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
            (PrevSlot[ic].trigger.tgstate == TgState_Sustain))
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
        PrevSlot[ic].trigger.Deactivate();
        if(PrevSlot[ic].trigger.apatt_instance != NULL)
        {
            if(PrevSlot[ic].trigger.apatt_instance->IsPlaybackActive())
            {
                DeactivatePlayback(PrevSlot[ic].trigger.apatt_instance->pbk);
            }
            Del_Pattern_Internal_Triggers(PrevSlot[ic].trigger.apatt_instance, true);
            delete PrevSlot[ic].trigger.apatt_instance;
            PrevSlot[ic].trigger.apatt_instance = NULL;
        }

        delete PrevSlot[ic].ii;
        PrevSlot[ic].ii = NULL;
        PrevSlot[ic].state = PState_Free;
        PrevSlot[ic].trigger.patt = NULL;
        PrevSlot[ic].key = -1;
    }
}

void Preview_MouseRelease()
{
    M.pianokey_pressed = false;
    M.pianokey_slot = 0xFFFF;

    preview_mouse_release = true;
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
    preview_finish_all = true;
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
            PrevSlot[ic].trigger.Release();
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
        if (p_cur_instr->subtype == ModSubtype_VSTPlugin)
        {
            ((VSTGenerator*)(p_cur_instr))->StopAllNotes();
        }
        p_cur_instr = p_cur_instr->next;
    }
}

void Preview_CheckStates(long num_frames)
{
    bool doR = false, doF = false;
    if(preview_mouse_release)
        doR = true; // Use flags to ensure release and finish only from start of this function
    if(preview_finish_all)
        doF = true;

    if(NumPrevs > 0)
    {
        for (int ic = 0; ic < MAX_PREVIEW_ELEMENTS; ic++)
        {
            if(PrevSlot[ic].state == PState_Playing)
            {
                if(PrevSlot[ic].keybound_pianokey == false)
                {
                    if(preview_mouse_release && doR)
                        PrevSlot[ic].trigger.Release();
                    else if(preview_finish_all && doF)
                        PrevSlot[ic].trigger.SoftFinish();
                }
            }
            else if(PrevSlot[ic].state == PState_Ready)
            {
                Trigger* tg = &PrevSlot[ic].trigger;
                Instrument* instr = ((NoteInstance*)tg->el)->instr;

                if(tg->tgworking == true)
                {
                    tg->Deactivate();
                }

                tg->Activate();
                if(PrevSlot[ic].sustainable == false || 
                   (preview_mouse_release && doR && PrevSlot[ic].ii->type != El_Samplent))
                {
                    tg->Release();
                }
                if(tg->apatt_instance != NULL)
                {
                    if(tg->apatt_instance->pbk->dworking == true)
                    {
                        DeactivatePlayback(tg->apatt_instance->pbk);
                    }
                    ActivatePlayback(tg->apatt_instance->pbk);
                }

                tg->pslot->state = PState_Playing;
            }
            else if(PrevSlot[ic].state == PState_Stopped)
            {
                Trigger* tg = &PrevSlot[ic].trigger;
                if(tg->tgworking)
                    tg->Deactivate();
                if(tg->apatt_instance == NULL || !tg->apatt_instance->IsPlaybackActive())
                {
                    Instrument* instr = ((NoteInstance*)tg->el)->instr;
                    PrevSlot[ic].state = PState_Inactive;
                    NumPrevs--;
                }
            }
        }
    }

    if(preview_mouse_release && doR)
        preview_mouse_release = false;
    if(preview_finish_all && doF)
        preview_finish_all = false;
}

