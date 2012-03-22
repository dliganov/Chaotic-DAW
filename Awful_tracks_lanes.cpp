#include "awful.h"
#include "awful_audio.h"
#include "awful_instruments.h"
#include "awful_elements.h"
#include "awful_paramedit.h"
#include "awful_controls.h"
#include "awful_panels.h"
#include "awful_utils_common.h"
#include "awful_effects.h"
#include "awful_events_triggers.h"
#include "awful_tracks_lanes.h"
#include "awful_cursorandmouse.h"

extern void             Remove_Bunch(TrkBunch* tb, Pattern* pt);
extern void             Add_Bunch(int start, int end, ParamSet* params, Pattern* pattern);
extern void             AdvanceTracks(Trk* trkdata, int num, Pattern* pt);
extern void             DeleteTrack();
extern void             InsertTrack();
extern void             Bunch_Tracks(int y1, int y2);
extern int              GetActualTrack(int trk_num);
extern TrkBunch*        CheckIfTrackBunched(int trk_num, Pattern* pt);
extern Trk*             GetTrkDataForTrkNum(int trknum, Pattern* pt);
extern int              GetTrkNumForLine(int line, Pattern* pt);
extern Trk*             GetTrkDataForLine(int line, Pattern* pt);
extern void             ResetTrackControls();

Trk::Trk()
{
}

Trk::~Trk()
{
    delete params;
    if(vol != NULL && pan != NULL && mute != NULL && solo != NULL)
    {
        RemoveControlCommon(vol);
        RemoveControlCommon(pan);
        RemoveControlCommon(mute);
        RemoveControlCommon(solo);
    }
}

void Trk::Init(int track_num, bool controls)
{
    prev = NULL;
    next = NULL;
    trknum = track_num;
    active = false;
    bunched = buncho = bunchito = false;
    trktype = Trk_Usual;
    start_line = track_num;
    end_line = track_num;
    trk_start_line = track_num;
    trk_end_line = track_num;
    proll = NULL;
    trkbunch = NULL;
    defined_instr = NULL;
    params = new ParamSet(100, 50, &scope);
    envelopes = NULL;
    mcell = NULL;
    mchan = NULL;
    memset(&scope, 0, sizeof(Scope));
    scope.for_track = true;
    scope.trkdata = this;
    vol = NULL;
    pan = NULL;
    mute = NULL;
    solo = NULL;

    if(controls == true)
    {
        vol = new SliderHVol(59, NULL, &scope);
        pan = new SliderHPan(40, NULL, &scope);
        //pan = new Knob(6, NULL, NULL, &scope, Knob_Small);

        pan->SetHint("Track panning\0");
        vol->SetHint("Track volume\0");

    	params->vol->AddControl(vol);
        params->pan->AddControl(pan);
        params->vol->SetNormalValue(1.0f);
        params->pan->SetNormalValue(0.0f);

        vol->SetAnchor(1);
        pan->SetAnchor(0);

        mute = new Toggle(this, Toggle_Mute);
        solo = new Toggle(this, Toggle_Solo);

        mute->SetHint("Mute track\0");
        solo->SetHint("Solo track\0");

        AddNewControl(pan, mix);
        AddNewControl(vol, mix);
        AddNewControl(mute, mix);
        AddNewControl(solo, mix);
    }

    vol_lane = Lane(false, 3, Lane_Vol);
    pan_lane = Lane(false, 3, Lane_Pan);

    UpdateLaneBases();
}

void Trk::UpdateLaneBases()
{
    vol_lane.base_line = trk_end_line;
    pan_lane.base_line = trk_end_line;
    if(vol_lane.visible == true)
    {
        pan_lane.base_line = vol_lane.base_line + vol_lane.height;
    }
}

Lane::Lane()
{
    visible = false;
    height = 3;
}

Lane::Lane(bool vis, int hgt, LaneType lt)
{
    visible = vis;
    height = hgt;
    type = lt;
}

bool Lane::CheckMouse(int mouse_x, int mouse_y)
{
    if((mouse_y - y) <= 3 && (mouse_y - y) >= -3)
    {
        M.mmode |= MOUSE_RESIZE;
        M.edge = BOTTOM_Y;
        M.resize_object = Resize_Lane;
        M.active_lane = type;
        return true;
    }
    else if(mouse_y <= y && mouse_y > (y - height*(M.loc != Loc_SmallGrid ? lineHeight : gAux->lineHeight)))
    {
        M.mmode |= MOUSE_LANEAUXING;
        M.active_lane = type;
        return true;
    }
    return false;
}

void ResetTrackControls()
{
    Trk* trk = first_trkdata;
    while(trk != bottom_trk)
    {
        //trkfxstr[trk->trknum]->active = false;
        //trkfxstr[trk->trknum]->drawarea->Disable();
        trk->mute->active = false;
        trk->mute->drawarea->Disable();
        trk->solo->active = false;
        trk->solo->drawarea->Disable();
        trk->vol->active = false;
        trk->vol->drawarea->Disable();
        trk->pan->active = false;
        trk->pan->drawarea->Disable();
        trk = trk->next;
    }
}

TrkBunch* CheckIfTrackBunched(int trk_num, Pattern* pt)
{
    TrkBunch* tb = pt->bunch_first;
    while(tb != NULL)
    {
        if((trk_num >= tb->start_track)&&(trk_num <= tb->end_track))
        {
            return tb;
        }
        tb = tb->next;
    }
    return NULL;
}

int GetActualTrack(int trk_num)
{
    TrkBunch* tb = CheckIfTrackBunched(trk_num, field);
    if(tb != NULL)
    {
        return tb->start_track;
    }
    return trk_num;
}

void InsertTrack()
{
    Element* elem = firstElem;
	Element* elemnext;
    while(elem != NULL)
    {
        elemnext = elem->next;
        if(elem->patt == field)
        {
            if(elem->trknum == 999)
            {
                elemnext = NextElementToDelete(elem);
                DeleteElement(elem, true, true);
            }
            else if(elem->trknum >= C.trknum && elem->patt == field)
            {
                elem->Move(0, 1);
            }
        }
        elem = elemnext;
    }

    Trk* trk = last_trkdata;
    if(trk->bunchito == true)
    {
        trk->bunchito = false;
        trk->prev->bunchito = true;
        trk->trkbunch->end_track = trk->prev->trknum;
        trk->trkbunch->trk_end = trk->prev;
    }

    if(trk->prev->buncho == true)
    {
        Remove_Bunch(trk->trkbunch, field);
    }

	trk->trknum = C.trknum;
    trk->start_line = C.trk->start_line;
    trk->end_line = trk->start_line;
    trk->trk_start_line = trk->start_line;
    trk->trk_end_line = trk->start_line;
    trk->buncho = false;
    trk->bunched = C.trk->bunched;
    trk->trkbunch = C.trk->trkbunch;
	trk->vol_lane.visible = false;
    trk->vol_lane.height = 3;
    trk->pan_lane.visible = false;
    trk->pan_lane.height = 3;
	trk->params->vol->SetNormalValue(1);
	trk->params->pan->SetNormalValue(0);

    trk->UpdateLaneBases();

    trk = C.trk;
    while(trk != last_trkdata)
    {
        trk->trknum++;
        trk->start_line++;
        trk->end_line++;
        trk->trk_start_line++;
        trk->trk_end_line++;
        
        if(trk->trkbunch != NULL)
        {
            if(trk->buncho == true)
            {
                trk->trkbunch->start_track++;
                trk->trkbunch->startpix += lineHeight;
            }
        
            if(trk->bunchito == true)
            {
                trk->trkbunch->end_track++;
                trk->trkbunch->endpix += lineHeight;
            }
        }
        
        if(trk->proll != NULL)
        {
            trk->proll->start_line++;
            trk->proll->end_line++;
            trk->proll->startpix += lineHeight;
            trk->proll->endpix += lineHeight;
        }
        trk->UpdateLaneBases();
        trk = trk->next;
    }

	trk->prev->next = trk->next;
    trk->next->prev = trk->prev;
    last_trkdata = trk->prev;
    C.trk->prev->next = trk;
    trk->prev = C.trk->prev;
    trk->next = C.trk;
    C.trk->prev = trk;
	if(C.trk == first_trkdata)
	{
		first_trkdata = trk;
	}
    C.trk = trk;

    field->first_trkdata = first_trkdata;
    field->last_trkdata = last_trkdata;

    UpdateAllElements(field);
}

void DeleteTrack()
{
    Element* elem = firstElem;
	Element* elemnext; 
    while(elem != NULL)
    {
		elemnext = elem->next;
        if(elem->patt == field)
        {
            if(elem->trknum == C.trknum)
            {
                elemnext = NextElementToDelete(elem);
                DeleteElement(elem, true, true);
            }
            else if(elem->trknum > C.trknum)
            {
                elem->Move(0, -1);
            }
        }
        elem = elemnext;
    }

    Trk* trk = C.trk;
    if(trk->buncho == true)
    {
        if(trk->next->bunchito == true)
        {
            Remove_Bunch(trk->trkbunch, field);
        }
        else
        {
            trk->next->buncho = true;
            trk->next->bunched = false;
            trk->trkbunch->start_track = trk->next->trknum;
            trk->trkbunch->trk_start = trk->next;
        }
    }
    else if(trk->bunchito == true)
    {
        trk->prev->bunchito = true;
        trk->prev->trkbunch = trk->trkbunch;
        trk->trkbunch->end_track--;
        trk->trkbunch->trk_end = trk->prev;
        trk->trkbunch->endpix -= lineHeight;
    }

    trk->start_line = last_trkdata->start_line;
    trk->vol->active = trk->pan->active = trk->mute->active = trk->solo->active = false;
    trk->end_line = trk->start_line;
    trk->trk_start_line = trk->start_line;
    trk->trk_end_line = trk->start_line;
    trk->buncho = false;
    trk->bunched = false;
    trk->bunchito = false;
    trk->trkbunch = NULL;
	trk->vol_lane.visible = false;
    trk->vol_lane.height = 3;
    trk->pan_lane.visible = false;
    trk->pan_lane.height = 3;
	trk->params->Reset();
    trk->vol->drawarea->UnChange();
    trk->pan->drawarea->UnChange();
    trk->UpdateLaneBases();
	trk->trknum = 999;

    trk = last_trkdata;
    while(trk != C.trk)
    {
        trk->trknum--;
        trk->start_line--;
        trk->end_line--;
        trk->trk_start_line--;
        trk->trk_end_line--;
        if(trk->trkbunch != NULL)
        {
            if(trk->buncho == true)
            {
                trk->trkbunch->start_track--;
                trk->trkbunch->startpix -= lineHeight;
            }
            if(trk->bunchito == true)
            {
                trk->trkbunch->end_track--;
                trk->trkbunch->endpix -= lineHeight;
            }
        }
        
        trk->UpdateLaneBases();
        trk = trk->prev;
    }

    C.trk = trk->next;
	trk->prev->next = trk->next;
    trk->next->prev = trk->prev;

    last_trkdata->next->prev = trk;
    trk->next = last_trkdata->next;
    last_trkdata->next = trk;
    trk->prev = last_trkdata;
    last_trkdata = trk;

	if(trk == first_trkdata)
	{
		first_trkdata = C.trk;
	}

    field->first_trkdata = first_trkdata;
    field->last_trkdata = last_trkdata;

    UpdateAllElements(field);
}

int GetTrkNumForLine(int line, Pattern* pt)
{
    Trk* td = pt->first_trkdata;
    while(td != NULL)
    {
        if(line >= td->start_line && line <= td->end_line)
        {
            return td->trknum;
        }
        td = td->next;
    }
    return 0xFFFF;
}

Trk* GetTrkDataForLine(int line, Pattern* pt)
{
	if(pt != NULL)
	{
		Trk* td = pt->first_trkdata;
		while(td != NULL)
		{
			if(line >= td->start_line && line <= td->end_line)
			{
				return td;
			}
			td = td->next;
		}
	}
    return NULL;
}

Trk* GetTrkDataForTrkNum(int trknum, Pattern* pt)
{
    Trk* td = pt->first_trkdata;
    while(td != NULL)
    {
        if(trknum == td->trknum)
        {
            return td;
        }
        td = td->next;
    }
    return NULL;
}

void Add_Bunch(int start, int end, ParamSet* params, Pattern* pt)
{
    TrkBunch* nb = (TrkBunch*)malloc(sizeof(TrkBunch));
    Trk* trk;
    trk = GetTrkDataForTrkNum(start, pt);
    trk->buncho = true;
    trk->trkbunch = nb;
    nb->trk_start = trk;
    int startpix = trk->start_line*lineHeight;

    while(trk->trknum <= end)
    {
        trk->bunched = true;
        trk->trkbunch = nb;
        if(trk->trknum == end)
        {
            trk->bunchito = true;
        }
        trk = trk->next;
    }

    trk = trk->prev;
    nb->trk_end = trk;
    int endpix = trk->end_line*lineHeight + lineHeight;
    nb->start_track = start;
    nb->end_track = end;
    nb->startpix = startpix;
    nb->endpix = endpix;

    nb->params = params;
    nb->next = NULL;

    if(pt->bunch_first == NULL && pt->bunch_last == NULL)
    {
        nb->prev = NULL;
        nb->next = NULL;
        pt->bunch_first = nb;
    }
    else
    {
        pt->bunch_last->next = nb;
        nb->prev = pt->bunch_last;
        nb->next = NULL;
    }
    pt->bunch_last = nb;
}

void Remove_Bunch(TrkBunch* tb, Pattern* pt)
{
    Trk* trk;
    trk = tb->trk_start;
    trk->buncho = false;
    trk->trkbunch = NULL;
    while(trk->trknum <= tb->trk_end->trknum)
    {
        trk->bunched = false;
        trk->trkbunch = NULL;
        if(trk->trknum == tb->end_track)
        {
            trk->bunchito = false;
        }
        trk = trk->next;
    }

    if((tb == pt->bunch_first)&&(tb == pt->bunch_last))
    {
        pt->bunch_first = NULL;
        pt->bunch_last = NULL;
    }
    else if(tb == pt->bunch_first)
    {
        pt->bunch_first = tb->next;
        pt->bunch_first->prev = NULL;
    }
    else if(tb == pt->bunch_last)
    {
        pt->bunch_last = tb->prev;
        pt->bunch_last->next = NULL;
    }
    else
    {
        if(tb->prev != NULL)
        {
            tb->prev->next = tb->next;
        }
        if(tb->next != NULL)
        {
            tb->next->prev = tb->prev;
        }
    }
}

void Bunch_Tracks(int y1, int y2)
{
    int start = GetTrkNumForLine(Y2Line(y1 + GridY1, Loc_MainGrid) - M.patt->track_line, M.patt);
    int end = GetTrkNumForLine(Y2Line(y2 + GridY1 - 1, Loc_MainGrid) - M.patt->track_line, M.patt);

    if(end > start && start >= 0 && end > 0)
    {
        for(int ic = start; ic <= end; ic++)
        {
            if(CheckIfTrackBunched(ic, M.patt) != NULL)
            {
                return;
            }
        }

        Trk* td = GetTrkDataForTrkNum(start, M.patt);
        ParamSet* params = td->params;
        Add_Bunch(start, end, params, M.patt);
        UpdateAllElements(M.patt);
    }
}

void AdvanceTracks(Trk* trkdata, int num, Pattern* pt)
{
    if(pt == field)
    {
        Element* elem = firstElem;
        while(elem != NULL)
        {
            if(elem->patt == field && elem->trknum >= trkdata->trknum)
            {
                if(elem->type == El_Pattern)
                {
					if(elem->trknum == trkdata->trknum)
					{
						elem = elem->next;
						continue;
					}
                    // pattern-specific
                   ((Pattern*)elem)->track_line_end += num;
                }
                elem->track_line += num;
            }
            elem = elem->next;
        }

        Trk* trk = trkdata;
        while(trk != NULL)
        {
            trk->start_line += num;
            trk->end_line += num;
            trk->trk_start_line += num;
            trk->trk_end_line += num;

            if(trk->trkbunch != NULL && trk->buncho == true)
            {
                trk->trkbunch->startpix += num*lineHeight;
                trk->trkbunch->endpix += num*lineHeight;
            }

            if(trk->proll != NULL)
            {
                trk->proll->start_line += num;
                trk->proll->end_line += num;
                trk->proll->startpix += num*lineHeight;
                trk->proll->endpix += num*lineHeight;
            }
            trk->UpdateLaneBases();
            trk = trk->next;
        }
    }
    else
    {
        Trk* trk = trkdata;
        while(trk != NULL)
        {
            trk->start_line += num;
            trk->end_line += num;
            trk->trk_start_line += num;
            trk->trk_end_line += num;

            if(trk->trkbunch != NULL && trk->buncho == true)
            {
                trk->trkbunch->startpix += num*lineHeight;
                trk->trkbunch->endpix += num*lineHeight;
            }

            if(trk->proll != NULL)
            {
                trk->proll->start_line += num;
                trk->proll->end_line += num;
                trk->proll->startpix += num*lineHeight;
                trk->proll->endpix += num*lineHeight;
            }
            trk->UpdateLaneBases();
            trk = trk->next;
        }

        Element* elem = pt->first_elem;
        while(elem != NULL)
        {
            if(elem->trknum >= trkdata->trknum)
            {
                elem->track_line += num;
            }
            elem = elem->patt_next;
        }
    }
}

void ShowVolLane(Trk* trk, Pattern* pt)
{
    trk->vol_lane.visible = true;
    trk->end_line += trk->vol_lane.height;
    trk->UpdateLaneBases();
    pt->track_line_end += trk->vol_lane.height;
    pt->num_lines += trk->vol_lane.height;
    AdvanceTracks(trk->next, trk->vol_lane.height, pt);
}

void HideVolLane(Trk* trk, Pattern* pt)
{
    trk->vol_lane.visible = false;
    trk->end_line -= trk->vol_lane.height;
    trk->UpdateLaneBases();
    pt->track_line_end -= trk->vol_lane.height;
    pt->num_lines -= trk->vol_lane.height;
    AdvanceTracks(trk->next, -trk->vol_lane.height, pt);
}

void ShowPanLane(Trk* trk, Pattern* pt)
{
    trk->pan_lane.visible = true;
    trk->end_line += trk->pan_lane.height;
    trk->UpdateLaneBases();
    pt->track_line_end += trk->vol_lane.height;
    pt->num_lines += trk->vol_lane.height;
    AdvanceTracks(trk->next, trk->pan_lane.height, pt);
}

void HidePanLane(Trk* trk, Pattern* pt)
{
    trk->pan_lane.visible = false;
    trk->end_line -= trk->pan_lane.height;
    trk->UpdateLaneBases();
    pt->track_line_end -= trk->pan_lane.height;
    pt->num_lines -= trk->pan_lane.height;
    AdvanceTracks(trk->next, -trk->pan_lane.height, pt);
}

