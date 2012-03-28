#include "awful_elements.h"
#include "awful.h"
#include "awful_audio.h"
#include "awful_controls.h"
#include "awful_instruments.h"
#include "awful_panels.h"
#include "awful_utils_common.h"
#include "awful_events_triggers.h"
#include "VSTCollection.h"
#include "awful_tracks_lanes.h"
#include "awful_cursorandmouse.h"
#include "awful_undoredo.h"

//#include "math.h"


extern Element* ReassignInstrument(Instance* ii, Instrument* instr);
extern void     BindPatternToInstrument(Pattern* pt, Instrument* instr);
extern void     UpdateStepSequencers();

Element::Element()
{
    if(C.patt == NULL || C.patt == field_pattern)
    {
        patt = C.patt;
    }
    else
    {
        if(C.patt->OrigPt != NULL)
        {
            patt = C.patt->OrigPt;
        }
        else
        {
            patt = C.patt;
        }
    }

    if(C.patt != NULL)
    {
        base = field_pattern;
    }
    else
    {
        base = NULL;
    }

    active_param = NULL;
    last_edited_param = NULL;
    default_param = NULL;
    first_param = NULL;
    last_param = NULL;

    track_line = 0;
    trkdata = NULL;
    field_trkdata = NULL;
    trknum = field_trknum = -1;
    
    prev = NULL;
    next = NULL;
    patt_prev = NULL;
    patt_next = NULL;
    pr_prev = NULL;
    pr_next = NULL;

    active = false;
    highlighted = false;
    selected = false;
    picked = false;
    visible = false;
    auxvisible = false;
    displayable = true;

    is_copied = false;
    gridauxed = auxauxed = 0xFFFF;
    to_be_deleted = false;
    deleted = false;

	if(patt != NULL)
	{
		start_tick = CTick;
        track_line = CLine;

		end_tick = -1;
		end_frame = -1;
	}

    pe_mode = ParamEdit_Open;

    tg_first = tg_last = NULL;
}

Element::~Element()
{
    undoMan->WipeElementFromHistory(this);
}

Element* Element::Clone(bool add)
{
    return NULL;
}

void Element::Globalize()
{
    if(trkdata != NULL)
    {
        if(trkdata->trkbunch == NULL)
        {
            field_trkdata = trkdata;
        }
        else
        {
            field_trkdata = trkdata->trkbunch->trk_start;
        }
        field_trknum = field_trkdata->trknum;
    }
}

void Element::UpdateTrackDataFromLine()
{
	if(track_line < 0)
	{
		track_line = 0;
	}
    trknum = GetTrkNumForLine(track_line, patt);
    trkdata = GetTrkDataForTrkNum(trknum, patt);
}

void Element::Update(bool tonly)
{
    if(tonly)
        CalcTiming();
    else
    {
        UpdateTrackDataFromLine();
        CalcTiming();
        Globalize();
        track_params = field_trkdata->params;
        UpdateNote();
        CheckVisibility();
        Relocate_Element(this);
    }
}

void Element::CalcTiming()
{
    frame = Tick2Frame(start_tick);
    end_frame = Tick2Frame(end_tick);
    tick_length = end_tick - start_tick;
    //frame_length = Tick2Frame(tick_length);
    frame_length = end_frame - frame - 1;
}

void Element::Leave()
{
    last_edited_param = active_param;
    if(active_param != NULL)
        active_param->Deactivate();
}

void Element::PlaceCursor()
{
	if(visible || auxvisible)
	{
        if(patt->ptype != Patt_StepSeq)
        {
    		C.type = CType_UnderVert;
    		CursX0 = x;
    		CursX = cx;
    		CursY = y;
        }
        else
        {
    		C.type = CType_Quad;
    		CursX0 = x;
    		CursX = x;
    		CursY = y;
        }
	}
	else
	{
		C.type = CType_None;
	}
}

void Element::SetAbsArea(int x1, int y1, int x2, int y2)
{
    abs_area.x1 = x1;
    abs_area.y1 = y1;
    abs_area.x2 = x2;
    abs_area.y2 = y2;
}

void Element::Activate()
{
    CTick = start_tick;
    CLine = track_line;
    if(default_param != NULL)
    {
        Switch2Param(default_param);
    }
    else
    {
        Switch2Param(last_edited_param);
    }
}

void Element::AddParamedit(PEdit* param)
{
    if((first_param == NULL)&&(last_param == NULL))
	{
        param->prev = NULL;
        param->next = NULL;
		first_param = param;
	}
    else
    {
        last_param->next = param;
        param->prev = last_param;
        param->next = NULL;
    }

    last_param = param;
}

void Element::RemoveParamedit(PEdit* param)
{
    if((param == first_param)&&(param == last_param))
    {
        first_param = NULL;
        last_param = NULL;
    }
    else if(param == first_param)
    {
        first_param = param->next;
        first_param->prev = NULL;
    }
    else if(param == last_param)
    {
        last_param = param->prev;
        last_param->next = NULL;
    }
    else
    {
        if(param->prev != NULL)
        {
            param->prev->next = param->next;
        }
        if(param->next != NULL)
        {
            param->next->prev = param->prev;
        }
    }

    delete param;
}

void Element::Switch2Param(PEdit* param)
{
	last_edited_param = active_param;

    if(active_param != NULL)
    {
        active_param->Deactivate();
    }

    if(param != NULL)
    {
        param->Activate();
        C.curParam = param;
        C.mode = CMode_ElemParamEdit;
    }
    else
    {
        C.mode = CMode_ElemEdit;
    }
    C.curElem = this;

    active_param = param;
}

bool Element::IsPointed(int mx, int my)
{
    if(IsPresent() == true)
    {
        if(M.loc == Loc_SmallGrid && auxvisible && patt == aux_panel->workPt->OrigPt)
        {
            if((mx >= abs_area.x1)&&(mx <= abs_area.x2)&&(my >= abs_area.y1)&&(my <= abs_area.y2))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else if(M.loc == Loc_MainGrid && visible && !(patt == aux_panel->workPt->OrigPt && aux_panel->auxmode == AuxMode_Pattern)
                && base == field_pattern && patt->folded == false)
        {
            if((mx >= abs_area.x1)&&(mx <= abs_area.x2)&&(my >= abs_area.y1)&&(my <= abs_area.y2))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }

    return false;
}

bool Element::CheckSelected()
{
    bool retval = false;
    Deselect();

    if(visible || auxvisible)
    {
        if(M.selloc == Loc_MainGrid)
        {
            if(patt == field_pattern) // && base == field)
            {
                if(CheckPlaneCrossing(abs_area.x1 - GridX1, abs_area.y1 - GridY1, abs_area.x2 - GridX1, abs_area.y2 - GridY1, SelX1, SelY1, SelX2, SelY2))
                {
                    Select();
                    selloc = Loc_MainGrid;
                    retval = true;
                }
            }
        }
        else if(M.selloc == Loc_StaticGrid && base == st->patt)
        {
            if(patt->folded == false)
            {
    			if(CheckPlaneCrossing(abs_area.x1 - StX1, abs_area.y1 - GridY1, abs_area.x2 - StX1, abs_area.y2 - GridY1, SelX1, SelY1, SelX2, SelY2))
                {
                    Select();
                    selloc = Loc_StaticGrid;
                    retval = true;
                }
            }
        }
    	else if(M.selloc == Loc_SmallGrid && patt == aux_panel->workPt->OrigPt)
        {
            if(CheckPlaneCrossing(abs_area.x1 - GridXS1, abs_area.y1 - GridYS1, abs_area.x2 - GridXS1, abs_area.y2 - GridYS1, SelX1, SelY1, SelX2, SelY2))
            {
                Select();
                selloc = Loc_SmallGrid;
                retval = true;
            }
        }
    }

    return retval;
}

void Element::CheckVisibility()
{
	visible = auxvisible = false;
	if(trkdata != NULL && displayable == true)
	{
		if(patt == field_pattern)
		{
    		if(patt->folded == false)
    		{
        		int x1 = StartTickX();
        		int x2 = EndTickX();

        		int y1 = TrackLine2Y() - lineHeight;
        		int y2 = EndLine2Y();

				if(CheckPlaneCrossing(x1, y1, x2, y2, GridX1, GridY1, GridX2, GridY2))
				{
					visible = true;
				}
    		}
		}
		else if(patt == aux_panel->workPt->OrigPt && aux_panel->auxmode == AuxMode_Pattern)
		{
		    int x1, x2, y1, y2;
		    if(patt->ptype == Patt_StepSeq)
            {
    			x1 = s_StartTickX();
    			x2 = x1 + 16;
    	        
    			y1 = s_TrackLine2Y() - lineHeight;
    			y2 = s_EndLine2Y();
            }
            else
            {
    			x1 = s_StartTickX();
    			x2 = s_EndTickX();
    	        
    			y1 = s_TrackLine2Y() - lineHeight;
    			y2 = s_EndLine2Y();
            }

			if(CheckPlaneCrossing(x1, y1, x2, y2, GridXS1, GridYS1, GridXS2, GridYS2))
			{
				auxvisible = true;
			}
		}
	}
}

void Element::Move(float dtick, int dtrack)
{
    float adtick = dtick;
    int adtrack = dtrack;

    start_tick += dtick;
    end_tick = start_tick + tick_length;
    track_line += dtrack;

    Update();
}

int Element::Track()
{
    if((patt != NULL)&&(patt != base))
    {
        return patt->Track();
    }
    else
    {
        return trknum;
    }
}

float Element::StartTick()
{
    return start_tick;
}

float Element::EndTick()
{
    return end_tick;
}

int Element::StartTickX()
{
    return Tick2X(start_tick, Loc_MainGrid);
}

int Element::EndTickX()
{
    return Tick2X(end_tick, Loc_MainGrid);
}

int Element::TrackLine2Y()
{
    //int line = trkdata->start_line;
    return Line2Y(patt->track_line + track_line, Loc_MainGrid);
}

int Element::EndLine2Y()
{
    //int line = trkdata->start_line;
    return Line2Y(patt->track_line + track_line, Loc_MainGrid);
}

int Element::s_StartTickX()
{
    return Tick2X(start_tick, Loc_SmallGrid);
}

int Element::s_EndTickX()
{
    return Tick2X(end_tick, Loc_SmallGrid);
}

int Element::st_StartTickX()
{
    return Tick2X(start_tick, Loc_StaticGrid);
}

int Element::st_EndTickX()
{
    return Tick2X(end_tick, Loc_StaticGrid);
}

int Element::s_TrackLine2Y()
{
    //int line = trkdata->start_line;
    return Line2Y(track_line, Loc_SmallGrid);
}

int Element::s_EndLine2Y()
{
    //int line = trkdata->start_line;
    return Line2Y(track_line, Loc_SmallGrid);
}

long Element::StartFrame()
{
    return frame;
}

long Element::EndFrame()
{
	if(patt == field_pattern)
	{
		return end_frame;
	}
	else
	{
		long pef = patt->EndFrame();
		long pp = patt->StartFrame() + end_frame;
		if(pp > pef)
		{
			return pef;
		}
		else
		{
			return pp;
		}
	}
}

void Element::ProcessKey(unsigned int key, unsigned int flags)
{
    Process_Key_Default(key, flags);
}

void Element::AddTrigger(Trigger* tg)
{
    if((tg_first == NULL)&&(tg_last == NULL))
    {
        tg->el_prev = NULL;
        tg->el_next = NULL;
        tg_first = tg;
    }
    else
    {
        tg_last->el_next = tg;
        tg->el_prev = tg_last;
        tg->el_next = NULL;
    }
    tg_last = tg;
}

void Element::RemoveTrigger(Trigger* tg)
{
	if((tg == tg_first)&&(tg == tg_last))
	{
		tg_first = NULL;
		tg_last = NULL;
	}
	else if(tg == tg_first)
	{
		tg_first = tg->el_next;
		tg_first->el_prev = NULL;
	}
	else if(tg == tg_last)
	{
		tg_last = tg->el_prev;
		tg_last->el_next = NULL;
	}
	else
	{
		tg->el_prev->el_next = tg->el_next;
		tg->el_next->el_prev = tg->el_prev;
	}
}

void Element::ForceResetTriggers()
{
    Trigger* tg = tg_first;
    while(tg != NULL)
    {
        tg->wt_pos = 0;
        tg->signal = 1;
        tg->frame_phase = 0;
        tg->tgworking = false;
        tg->globallisted = false;
        tg->tgsactive = false;
        if(tg->apatt_instance != NULL && tg->apatt_instance->pbk != NULL)
        {
            tg->apatt_instance->pbk->dworking = false;
        }

        tg = tg->el_next;
    }
}

bool Element::IsPlaying()
{
    Trigger* tgs;
    Trigger* tg = tg_first;
    while(tg != NULL)
    {
        // Check per pattern triggers
        if(tg->tgworking == true || tg->globallisted == true)
        {
            return true;
        }
        else
        {
            // Check sliding triggers
            tgs = tg->first_tgslide;
            while(tgs != NULL)
            {
                if(tgs->tgworking == true || tg->globallisted == true)
                {
                    return true;
                }
                tgs = tgs->group_next;
            }

            // Check autoinstance if exists
            if(tg->apatt_instance != NULL && tg->apatt_instance->IsPlaying() == true)
            {
                return true;
            }
        }
        tg = tg->el_next;
    }
    return false;
}

bool Element::IsPresent()
{
    if(to_be_deleted == true || deleted == true || (patt != NULL && patt->to_be_deleted == true))
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool Element::IsCoveringEffect()
{
    switch(type)
    {
        case El_Command:
        case El_Mute:
        case El_Break:
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

void Element::GetRealCoordinates(Loc loc)
{
    if(loc == Loc_MainGrid)
    {
        x = StartTickX();
        y = TrackLine2Y() - bottomIncr;
    }
    else if(loc == Loc_SmallGrid)
    {
        x = s_StartTickX();
        y = s_TrackLine2Y() - aux_panel->bottomincr;
    }
    else if(loc == Loc_StaticGrid)
    {
        x = st_StartTickX();
        y = TrackLine2Y() - bottomIncr;
    }
}

bool Element::IsInstance()
{
    if(type == El_Samplent || type == El_GenNote)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Element::Delete()
{
}

void Element::UnDelete()
{
}

void Element::DeactivateAllTriggers()
{
    Trigger* tg = tg_first;
    while(tg != NULL)
    {
        if(tg->tgworking)
        {
            tg->Deactivate();
        }
        tg = tg->el_next;
    }
}

bool Element::IsDisplayableOnPatternType(PattType ptype)
{
    if(ptype == Patt_Grid)
    {
        return true;
    }
    else if(ptype == Patt_Pianoroll)
    {
        if(type == El_Samplent || type == El_GenNote || type == El_SlideNote)
        {
            return true;
        }
    }
    else if(ptype == Patt_StepSeq)
    {
        if(type == El_Samplent || type == El_GenNote)
        {
            return true;
        }
    }

    return false;
}

void Element::SetTrackLine(int trkline)
{
    track_line = trkline;
    UpdateTrackDataFromLine();
}

void Element::SetEndTick(float endtick)
{
    end_tick = endtick;
    Update();
}

void Element::SetTickLength(float ticklength)
{
    // We calc. timing basing from end tick, not the tick length, so...
    end_tick = start_tick + ticklength;
    CalcTiming();
}

void Element::Save(XmlElement * xmlNode)
{
    xmlNode->setAttribute(T("Type"), int(type));
    xmlNode->setAttribute(T("StartTick"), start_tick);
    xmlNode->setAttribute(T("EndTick"), end_tick);
    xmlNode->setAttribute(T("TickLength"), tick_length);
    xmlNode->setAttribute(T("TrackLine"), track_line);

    if(type != El_Pattern)
    {
        xmlNode->setAttribute(T("PattIndex"), patt->origindex);
    }
}

void Element::Load(XmlElement * xmlNode)
{
    start_tick = (float)xmlNode->getDoubleAttribute(T("StartTick"));
    end_tick = (float)xmlNode->getDoubleAttribute(T("EndTick"));
    tick_length = (float)xmlNode->getDoubleAttribute(T("TickLength"));
}

void Element::Select()
{
    selected = true;
}

void Element::Deselect()
{
    selected = false;
}

void Element::MarkAsCopied()
{
    is_copied = true;
}

Loc Element::GetElemLoc()
{
    if(patt == field_pattern)
        return Loc_MainGrid;
    else if(patt == aux_panel->workPt->OrigPt)
        return Loc_SmallGrid;
    else
        return Loc_Other;
}

Txt::Txt()
{
    type = El_TextString;
    memset(buff, 0, 250);
    curPos = 0;
    bookmark = false;
}

Txt* Txt::Clone(bool add)
{
    Txt* clone = CreateElement_TextString();
    clone->AddString(buff);
    clone->curPos = curPos;
    clone->bookmark = bookmark;

    clone->start_tick = start_tick;
    clone->end_tick = end_tick;
    clone->track_line = track_line;
    clone->trknum = trknum;
    clone->tick_length = tick_length;

    return clone;
}

void Txt::AddString(char* character)
{
    int ic, len;
    len = (int)strlen(character);
    for(ic = 0; ic < len; ic++)
    {
        if(curPos == 200)
            break;
        buff[curPos] = character[ic];
        curPos++;
    }
}

void Txt::ParseString()
{
    if(bookmark == false)
    {
        Symbol sym = GetSymbolFromString(buff);
        if(sym == Symbol_Mute)
        {
            DeleteElement(this, false, false);

            Muter* m = CreateElement_Muter();
            
            C.mode = CMode_ElemEdit;
            C.curElem = m;
        }
        else if(sym == Symbol_Break)
        {
            DeleteElement(this, false, false);

            Break* bk = CreateElement_Break();
            C.mode = CMode_ElemEdit;
            C.curElem = bk;
        }
        else if(sym == Symbol_Slider)
        {
            DeleteElement(this, false, false);

            Slide* sl = CreateElement_Slide(+1);
            C.mode = CMode_ElemEdit;
            C.curElem = sl;
        }
        else if(sym == Symbol_Reflect)
        {
            DeleteElement(this, false, false);

            Reverse* r = CreateElement_Reverse();
            C.mode = CMode_ElemEdit;
            C.curElem = r;
        }
        /* // Canceled
        else if(sym == Symbol_Repeat)
        {
            Vanish(this);

            Repeat* rp = CreateRepeat();
            C.mode = CMode_ElemEdit;
            C.curElem = rp;
        }
        */
        else if(sym == Symbol_Vibrate)
        {
            DeleteElement(this, false, false);

            Vibrate* vb = CreateElement_Vibrate();

            C.mode = CMode_ElemEdit;
            C.curElem = vb;
        }
        else if(sym == Symbol_Transpose)
        {
            DeleteElement(this, false, false);

            Scope scp;
            memset(&scp, 0, sizeof(Scope));
            scp.for_track = true;
            Transpose* c = CreateElement_Transpose(1);

            C.mode = CMode_ElemEdit;
            C.curElem = c;
        }
/*
        else if(sym == Symbol_Command)
        {
            Vanish(this);

            Scope scp;
            memset(&scp, 0, sizeof(Scope));
            Command* c = CreateCommand(scp);

            C.mode = CMode_ElemEdit;
            C.curElem = c;
        }
        else if(sym == Symbol_Volume)
        {
            Vanish(this);

            Scope scp;
            memset(&scp, 0, sizeof(Scope));
            Command* c = CreateCommand(scp, Cmd_Vol);

            C.mode = CMode_ElemEdit;
            C.curElem = c;
        }
        else if(sym == Symbol_Panning)
        {
            Vanish(this);

            Scope scp;
            memset(&scp, 0, sizeof(Scope));

            Command* c = CreateCommand(scp, Cmd_Pan);

            C.mode = CMode_ElemEdit;
            C.curElem = c;
        }
        else if(sym == Symbol_Local)
        {
            Vanish(this);

            Scope scp;
            memset(&scp, 0, sizeof(Scope));
            scp.local = true;
            Command* c = CreateCommand(scp);

            C.mode = CMode_ElemEdit;
            C.curElem = c;
        }
        else if(sym == Symbol_Track)
        {
            Vanish(this);

            Scope scp;
            memset(&scp, 0, sizeof(Scope));
            scp.for_track = true;
            Command* c = CreateCommand(scp);

            C.mode = CMode_ElemEdit;
            C.curElem = c;
        }
*/
        else if(sym == Symbol_Bookmark && patt == field_pattern)
        {
            bookmark = true;
        }
        else
        {
            // Check for sample
            Instrument* instr = CheckStringForInstrumentAlias(buff);

            if(instr != NULL)
            {
                if(!(aux_panel->auxmode == AuxMode_Pattern && aux_panel->workPt->OrigPt->autopatt))
                {
                    ChangeCurrentInstrument(instr);
                }

                DeleteElement(this, false, false);

				C.curElem = CreateElement_Note(instr, true);
                C.curElem->Activate();
            }
            else if(patt == field_pattern)
            {
                Pattern* p = CheckStringForPatternName(buff);
                if(p != NULL)
                {
                    DeleteElement(this, false, false);

                    //Pattern* ptn = p->Clone(true);
                    //// Reset some stuff to initial
                    //ptn->muted = false;
                    //ptn->SetEndTick(ptn->StartTick() + ticks_per_beat);
                    Pattern* ptn = CreateElement_DerivedPattern(p->OrigPt, CTick, CTick + p->tick_length, CLine, CLine);
                    if(MakePatternsFat && CLine > 0)
                        ptn->is_fat = true;
                    else
                        ptn->is_fat = false;

                    //float dtick = CTick - ptn->start_tick;
                    //int dtrack = CLine - ptn->trknum;
                    //ptn->Move(dtick, dtrack);

                    ptn->Switch2Param(ptn->name);
                    ptn->aliasModeName = false;
                }
            }
        }
    }
}

void Txt::ProcessChar(char character)
{
    char str[2] = {character, 0};
    AddString(str);
    ParseString();

    if(bookmark == true)
    {
        R(Refresh_MainBar);
    }
}

void Txt::ProcessKey(unsigned int key, unsigned int flags)
{
    if(key == key_backspace)
    {
        if(curPos > 0)
        {
            delChar();

            // Adjust cursor to initial position
            if(curPos == 0)
            {
                DeleteElement(this, false, false);
            }
            else
    		{
                ParseString();
    		}
        }
    }
    else if(key == key_up)
    {
        C.ExitToDefaultMode();
        C.AdvanceLine(-1);
    }
    else if(key == key_down)
    {
        C.ExitToDefaultMode();
        C.AdvanceLine(1);
    }
    else if(key == key_return)
    {
        C.ExitToDefaultMode();
        C.AdvanceLine(1);
    }
    else if(key == key_left)
    {
        C.ExitToDefaultMode();
        C.AdvanceTick(-1.0f);
    }
    else if(key == key_right)
    {
        C.ExitToDefaultMode();
        C.AdvanceTick(1.0f);
    }
    else if(key == key_escape)
    {
        C.ExitToDefaultMode();
    }
    else
    {
        Process_Key_Default(key, flags);
    }

    if(bookmark == true)
    {
        R(Refresh_MainBar);
    }
}

void Txt::delChar()
{
    curPos--;
    buff[curPos] = 0;
}

void Txt::Move(float dtick, int dtrack)
{
    Element::Move(dtick, dtrack);
    if(bookmark == true)
    {
        R(Refresh_MainBar);
    }
}

void Txt::Delete()
{
    if(bookmark == true)
    {
        R(Refresh_MainBar);
    }
}

void Txt::UnDelete()
{
    if(bookmark == true)
    {
        R(Refresh_MainBar);
    }
}

void Txt::Save(XmlElement * xmlNode)
{
    Element::Save(xmlNode);
    xmlNode->setAttribute(T("Text"), buff);
    xmlNode->setAttribute(T("Bookmark"), bookmark ? 1 : 0);
}

void Txt::Load(XmlElement * xmlNode)
{
    Element::Load(xmlNode);
    xmlNode->getStringAttribute(T("Text")).copyToBuffer(buff, 250);
    curPos = strlen(buff);
    bookmark = xmlNode->getBoolAttribute(T("Bookmark"));
}

Instance::Instance()
{
    instr = NULL;
    first_slide = NULL;
    instr_pattern_params = NULL;
    instr_track_params = NULL;
    attached_vol_env = NULL;
    attached_pan_env = NULL;
    mixcell = NULL;
    trkbuff = NULL;
    preview = false;

    loc_vol = new Vol(100);
    loc_pan = new Pan(50);

    ed_vol = new Percent(100, 3, this, Param_Vol);
    AddParamedit(ed_vol);
    loc_vol->AddParamedit(ed_vol);

    ed_pan = new Percent(50, 2, this, Param_Pan);
    AddParamedit(ed_pan);
    loc_pan->AddParamedit(ed_pan);

    ed_note = new Note(baseNote, this);
    AddParamedit(ed_note);
}

Instance::~Instance()
{
    delete loc_vol;
    delete loc_pan;
    delete ed_vol;
    delete ed_pan;
    delete ed_note;
}

void Instance::ProcessChar(char character)
{
/*  if(*character == 0x2E)  // dot transitions to command
    {
      Vanish(this);

        Scope st;
        memset(&st, 0, sizeof(Scope));
        st.instr = this->instr;
        Command* c = CreateCommand(st);

        C.mode = CMode_ElemEdit;
        C.curElem = c;
    }
    else if(*character == 0x5e)
    {
        SwitchVol();
    }
    else if(*character == 0x2a)
    {
        SwitchPan();
    }
    else  
    if(character == 0x2f)
    {
        CTick += 2;
        C.ExitToSlidingMode(this, ed_note->value);
    }
    else  */// probably bukva character, process by text editor
    {
        DeleteElement(this, false, false);

        Txt* t = new Txt;
        AddNewElement(t, false);

        C.mode = CMode_ElemEdit;
        C.curElem = t;

        t->AddString(instr->alias->string);
        t->ProcessChar(character);
    }
}

void Instance::Mute()
{
    Trigger* tg = tg_first;
    while(tg != NULL)
    {
        tg->Release();

        tg = tg->el_next;
    }
}

void Instance::ProcessKey(unsigned int key, unsigned int flags)
{
    if(key == key_escape)
    {
        SwitchNote();
    }
    else if(key == key_right)
    {
        if(active_param == ed_note)
        {
            C.ExitToNoteMode(instr, ed_note->value);
            C.AdvanceTick(1.0f);
        }
        else
        {
            if(ed_vol->visible == true)
            {
                Switch2Param(ed_vol);
            }
            else if(ed_pan->visible == true)
            {
                Switch2Param(ed_pan);
            }
            else
            {
                C.ExitToDefaultMode();
                C.AdvanceTick(1.0f);
            }
        }
    }
    else if(key == key_up)
    {
        if(active_param == ed_note)
        {
            C.ExitToNoteMode(instr, ed_note->value);
            C.AdvanceLine(-1);
        }
        else
        {
            C.ExitToDefaultMode();
            C.AdvanceLine(-1);
        }
    }
    else if((key == key_down)||(key == key_return))
    {
        if(active_param == ed_note)
        {
            C.ExitToNoteMode(instr, ed_note->value);
            C.AdvanceLine(1);
        }
        else
        {
            C.ExitToDefaultMode();
            C.AdvanceLine(1);
        }
    }
    else if(key == key_left)
    {
        if(active_param == ed_note)
        {
            C.ExitToNoteMode(instr, ed_note->value);
            C.AdvanceTick(-1.0f);
        }
        else
        {
            C.ExitToDefaultMode();
            C.AdvanceTick(-1.0f);
        }
    }
    else if(key == key_backspace)
    {
        if(active_param == ed_note)
        {
            DeleteElement(this, false, false);
        }
        else
        {
            DeleteElement(this, false, false);

            Txt* t = new Txt;
            AddNewElement(t, false);

            C.mode = CMode_ElemEdit;
            C.curElem = t;

            t->AddString(instr->alias->string);
            t->ProcessKey(key, flags);
        }
    }
    else
    {
        Process_Key_Default(key, flags);
    }
}

void Instance::SwitchVol()
{
    if(ed_vol->visible == false)
    {
        ed_vol->visible = true;
		ed_vol->bypass = false;
    }
    else
    {
        ed_vol->bypass = true;
        ed_vol->visible = false;
        if(active_param == ed_vol)
        {
            Switch2Param(NULL);
        }
    }
}

void Instance::SwitchPan()
{
    if(ed_pan->visible == false)
    {
        ed_pan->visible = true;
		ed_pan->bypass = false;
    }
    else
    {
        ed_pan->bypass = true;
        ed_pan->visible = false;
        if(active_param == ed_pan)
        {
            Switch2Param(NULL);
        }
    }
}

void Instance::SwitchNote()
{
    CP->CurrentMode->Refresh();
    if(ed_note->visible == false)
    {
        ed_note->Show();
		ed_note->bypass = false;
        Switch2Param(ed_note);
        CP->CurrentMode->pressed = false;
        CP->NoteMode->pressed = true;
        CP->CurrentMode = CP->NoteMode;
    }
    else
    {
        if(active_param == ed_note)
        {
            Switch2Param(last_edited_param);
            CP->CurrentMode->pressed = false;
            CP->UsualMode->pressed = true;
            CP->CurrentMode = CP->UsualMode;
        }
        else
        {
            Switch2Param(ed_note);
            CP->CurrentMode->pressed = false;
            CP->NoteMode->pressed = true;
            CP->CurrentMode = CP->NoteMode;
        }
    }
    CP->CurrentMode->Refresh();

    MC->refresh |= Refresh_Buttons;
}

void Instance::SwitchLen()
{
    if(ed_len->visible == false)
    {
        ed_len->visible = true;
		ed_len->bypass = false;
    }
    else
    {
        ed_len->bypass = true;
        ed_len->visible = false;
        if(active_param == ed_len)
        {
            Switch2Param(NULL);
        }
    }
}

void Instance::Activate()
{
    CTick = start_tick;
    CLine = track_line;
    if(CP->CurrentMode == CP->NoteMode || ed_note->visible == true || patt->ptype == Patt_StepSeq)
    {
		if(patt->ptype != Patt_StepSeq)
			ed_note->Show();
        Switch2Param(ed_note);
    }
    else
    {
        Switch2Param(last_edited_param);
    }

	C.last_note = ed_note->value;
    C.last_char = ed_note->lastchar;
}

/////////////////////////////////////////////
// Connect to the proper place in slides list. We have a linked list of slides for each instrument, and
// we need to keep their sequence according to their actual timing.
void Instance::AddSlideNote(SlideNote* sl)
{
	sl->parent = this;
	if(first_slide == NULL)
    {
        first_slide = sl;
        sl->s_next = NULL;
        sl->s_prev = NULL;
    }
    else
    {
        SlideNote* sd = first_slide;
        if(sl->start_tick <= sd->start_tick)
        {
            first_slide = sl;
            sl->s_next = sd;
            sl->s_prev = NULL;
            sd->s_prev = sl;
        }
        else
        {
            while(sd->s_next != NULL)
            {
                if(sl->start_tick <= sd->s_next->start_tick)
                {
                    sl->s_next = sd->s_next;
                    sd->s_next = sl;
                    sl->s_prev = sd;
                    sl->s_next->s_prev = sl;
                    break;
                }
                else
                {
                    sd = sd->s_next;
                }
            }

            if(sd->s_next == NULL)
            {
                sd->s_next = sl;
                sl->s_prev = sd;
                sl->s_next = NULL;
            }
        }
    }
    CalcSlideNotes();
    SpreadSlideNotes();
}

void Instance::RemoveSlidenote(SlideNote* sl)
{
    if(sl->parent == this)
    {
        if(sl == first_slide)
        {
            first_slide = sl->s_next;
        }
        else
        {
            sl->s_prev->s_next = sl->s_next;
            if(sl->s_next != NULL)
            {
                sl->s_next->s_prev = sl->s_prev;
            }
        }
        sl->parent = NULL;
        sl->s_next = sl->s_prev = NULL;
        CalcSlideNotes();
        SpreadSlideNotes();
    }
}

void Instance::SpreadSlideNotes()
{
    SlideNote* sn;
    Trigger* ptg = tg_first;
    Trigger* stg;
    Trigger** pptg;
    // loop through element's pattlocal
    while(ptg != NULL)
    {
        pptg = &ptg->first_tgslide;

        // loop through slidenotes
        sn = first_slide;
        if(sn != NULL)
        {
            stg = sn->tg_first;
            // find corresponding slidenote's pattlocal
            while(stg != NULL && (stg->patt != ptg->patt || stg->activator != true))
            {
                stg = stg->el_next;
            }
            // TODO: insurance
           *pptg = stg;
            stg->group_prev = NULL;
            stg->tgsparent = ptg;

            sn = sn->s_next;
            while(sn != NULL)
            {
                stg = sn->tg_first;
                // find corresponding slidenote's pattlocal
                while(stg != NULL && (stg->patt != ptg->patt || stg->activator != true))
                {
                    stg = stg->el_next;
                }
              (*pptg)->group_next = stg;
                stg->group_prev = *pptg;
                stg->tgsparent = ptg;
			    pptg = &((*pptg)->group_next);

                sn = sn->s_next;
            }
            stg->group_next = NULL;
        }
        else
        {
            ptg->first_tgslide = NULL;
        }

        ptg = ptg->el_next;
    }
}

SlideNote* Instance::GetLastSlide()
{
    SlideNote* ls = first_slide;

	if(first_slide != NULL)
    {
        while(ls->s_next != NULL)
            ls = ls->s_next;
    }
    return ls;
}

//// Rearranges slidenotes in list to correspond to their actual timing (in ticks)
void Instance::RearrangeSlideNotes()
{
    SlideNote*  s_current;
    SlideNote*  s_closer;
    SlideNote*  s_pick;
    SlideNote*  s_first;
    bool		first;

    first = true;
    s_first = first_slide;
    while((s_first != NULL)&&(s_first->s_next != NULL))
    {
        s_pick = s_first;
        s_closer = s_first;
        while(s_pick != NULL)
        {
            if(s_pick->start_tick < s_closer->start_tick)
            {
                s_closer = s_pick;
            }
            s_pick = s_pick->s_next;
        }

		// remove this slide from the list and connect to the brand new
		if(s_closer == s_first)
        {
            s_first = s_first->s_next;
        }
        else
        {
			s_closer->s_prev->s_next = s_closer->s_next;
			if(s_closer->s_next != NULL)
			{
				s_closer->s_next->s_prev = s_closer->s_prev;
			}
        }

        if(first == true)
        {
            first_slide = s_closer;
            first_slide->s_prev = NULL;
            s_current = first_slide;
            first = false;
        }
        else
        {
            s_current->s_next = s_closer;
            s_closer->s_prev = s_current;
            s_current = s_current->s_next;
        }
    }

    if(first == false)
    {
        s_current->s_next = s_first;
        s_first->s_prev = s_current;
    }

    CalcSlideNotes();
    SpreadSlideNotes();
}

void Instance::CalcSlideNotes()
{
    float	 base_vol = loc_vol->val;
    float    base_pan = loc_pan->val;
    int		 base_note = ed_note->value; 

    SlideNote*   sdata = first_slide;
    while(sdata != NULL)
    {
        if(sdata->IsPresent())
        {
            sdata->linear_increment = (float)(1.0f/(double)sdata->frame_length);
            sdata->vol_increment = (float)(100.0f*(sdata->loc_vol->val - base_vol)/(double)sdata->frame_length);
            sdata->pan_increment = (float)((sdata->loc_pan->val - base_pan)/(double)sdata->frame_length);
            sdata->fR = CalcFreqRatio(sdata->ed_note->value - base_note);

            if(sdata->s_next == NULL || sdata->s_next->frame > sdata->end_frame)
            {
                base_vol = sdata->loc_vol->val;
                base_pan = sdata->loc_pan->val;
            }
            else
            {
                base_vol = base_vol + (sdata->s_next->frame - sdata->frame)*sdata->vol_increment;
                base_pan = base_pan + (sdata->s_next->frame - sdata->frame)*sdata->pan_increment;
            }
            base_note = sdata->ed_note->value;
        }

        sdata = sdata->s_next;
    }
}

void Instance::Reset()
{
    active = false;

    loc_vol->Reinit();
    loc_pan->Reinit();
    loc_len->Reinit();

    ed_vol->Reinit();
    ed_pan->Reinit();
    ed_len->Reinit();
    ed_note->Reinit();
}

int Instance::TrackLine2Y()
{
    int line = trkdata->start_line;
    return Line2Y(patt->track_line + line, Loc_MainGrid);
}

int Instance::EndLine2Y()
{
    int line = trkdata->start_line;
    return Line2Y(patt->track_line + line, Loc_MainGrid);
}

void Instance::UpdateNote()
{
    if(preview == false && patt->ptype == Patt_Pianoroll)
    {
        ed_note->SetValue(NUM_PIANOROLL_LINES - track_line - 1);
    }

    if(ed_note->relative == false)
    {
        freq_ratio = 1.0f/CalcFreqRatio(ed_note->value - baseNote);
    }
    else
    {
        freq_ratio = 1.0f/CalcFreqRatio(ed_note->value);
    }
    freq = NoteToFreq(ed_note->value);
    CalcSlideNotes();
}

void Instance::Move(float dtick, int dtrack)
{
	start_tick += dtick;
	end_tick += dtick;
	track_line += dtrack; 

	SlideNote* sdata = first_slide;
    while(sdata != NULL)
    {
        sdata->Move(dtick, dtrack);
		jassert(sdata->start_tick >= start_tick);
        sdata = sdata->s_next;
    }

    Update();
}

void Instance::Save(XmlElement * xmlNode)
{
    Element::Save(xmlNode);
    xmlNode->setAttribute(T("InstrIndex"), instr->index);
    xmlNode->setAttribute(T("Volume"), loc_vol->val);
    xmlNode->setAttribute(T("Panning"), loc_pan->val);
    xmlNode->setAttribute(T("Note"), ed_note->value);
    xmlNode->setAttribute(T("NoteVisible"), int(ed_note->visible));
    xmlNode->setAttribute(T("NoteRelative"), int(ed_note->relative));
}

void Instance::Load(XmlElement * xmlNode)
{
    Element::Load(xmlNode);
    loc_vol->SetNormalValue((float)xmlNode->getDoubleAttribute(T("Volume")));
    loc_pan->SetNormalValue((float)xmlNode->getDoubleAttribute(T("Panning")));
    ed_note->SetValue(xmlNode->getIntAttribute(T("Note")));
    ed_note->visible = xmlNode->getBoolAttribute(T("NoteVisible"));
    ed_note->relative = xmlNode->getBoolAttribute(T("NoteRelative"));
}

Gennote::Gennote(Instrument* instr)
{
    type = El_GenNote;
    this->instr = instr;

    if(RememberLengths)
        loc_len = new Len(instr->last_note_length, Len_Ticks);
    else
        loc_len = new Len(ticks_per_beat, Len_Ticks);

    ed_len = new Percent(10, 3, this, Param_Len);
    AddParamedit(ed_len);
    loc_len->AddParamedit(ed_len);

    tick_length = loc_len->lval.ticks;
    end_tick = start_tick + tick_length;

    this->owner = (Object*)instr;
}

Gennote::~Gennote()
{
    delete loc_len;
    delete ed_len;
}

Gennote* Gennote::Clone(bool add)
{
    Gennote* clone;
    clone = (Gennote*)CreateElement_Note(instr, false);
	clone->ed_note->SetRelative(ed_note->relative);
    clone->ed_note->SetValue(ed_note->value);
    clone->ed_note->visible = ed_note->visible;
    clone->ed_vol->value = ed_vol->value;
    clone->ed_vol->visible = ed_vol->visible;
    clone->ed_pan->value = ed_pan->value;
    clone->ed_pan->visible = ed_pan->visible;
    clone->ed_len->value = ed_len->value;
    clone->ed_len->visible = ed_len->visible;
    clone->loc_vol->SetNormalValue(loc_vol->val);
    clone->loc_pan->SetNormalValue(loc_pan->val);
    clone->loc_len->SetNormalValue(loc_len->val);
    clone->start_tick = start_tick;
    clone->end_tick = end_tick;
    clone->track_line = track_line;
    clone->trknum = trknum;
    clone->tick_length = tick_length;

    clone->freq = freq;

    if(add)
    {
        AddNewElement(clone, false);
    }
    else
    {
        clone->Update();
    }

    return clone;
}

void Gennote::Update(bool tonly)
{
    if(tonly)
    {
        CalcTiming();
    }
    else
    {
        UpdateTrackDataFromLine();
        CalcTiming();
        Globalize();

        track_params = field_trkdata->params;

        UpdateNote();
        CalcSlideNotes();
        CheckVisibility();

        Relocate_Element(this);
    }
}

Samplent::Samplent(Sample* smp)
{
    //Common stuff
    type = El_Samplent;
    instr = smp;
    sample = smp;
    sample_frame_len = (long)sample->sample_info.frames;
    wave_visible = false;
    wave_height_pix = 60;
    revB = false;
    loc_len = new Len(100, Len_Percent);
    loc_offs = new Len(0, Len_Percent);
    ed_len = new Percent(100, 3, this, Param_Len);
    AddParamedit(ed_len);
    loc_len->AddParamedit(ed_len);
    ed_offs = new Percent(0, 3, this, Param_Len);
    AddParamedit(ed_offs);
    loc_offs->AddParamedit(ed_offs);
    if(RememberLengths)
    {
        touchresized = sample->touchresized;
        tick_length = sample->last_note_length;
    }
    else
    {
        touchresized = false;
    }
}

Samplent::~Samplent()
{
    delete loc_len;
    delete loc_offs;
    delete ed_len;
    delete ed_offs;
}

Samplent* Samplent::Clone(bool add)
{
    Samplent* clone ;
    clone = (Samplent*)CreateElement_Note(sample, false);
	clone->ed_note->SetRelative(ed_note->relative);
    clone->ed_note->SetValue(ed_note->value);
    clone->ed_note->visible = ed_note->visible;
    clone->ed_vol->value = ed_vol->value;
    clone->ed_vol->visible = ed_vol->visible;
    clone->ed_pan->value = ed_pan->value;
    clone->ed_pan->visible = ed_pan->visible;
    clone->ed_len->value = ed_len->value;
    clone->ed_len->visible = ed_len->visible;
    clone->ed_offs->value = ed_offs->value;
    clone->ed_offs->visible = ed_offs->visible;
    clone->loc_vol->SetNormalValue(loc_vol->val);
    clone->loc_pan->SetNormalValue(loc_pan->val);
    clone->loc_len->SetNormalValue(loc_len->val);
    clone->loc_offs->SetNormalValue(loc_offs->val);
    clone->freq_incr_base = freq_incr_base;
    clone->revB = revB;
    clone->start_tick = start_tick;
    clone->end_tick = end_tick;
    clone->track_line = track_line;
    clone->trknum = trknum;
    clone->tick_length = tick_length;
    clone->wave_visible = wave_visible;
    clone->trkdata = trkdata;
    clone->field_trkdata = field_trkdata;
    clone->first_slide = NULL;
    clone->touchresized = touchresized;
    if(add)
    {
        AddNewElement(clone, false);
    }
    else
    {
        clone->Update();
    }
    return clone;
}

void Samplent::Reset()
{
	active = false;

	loc_vol->Reinit();
    loc_pan->Reinit();
    loc_len->Reinit();

    ed_vol->Reinit();
    ed_pan->Reinit();
    ed_len->Reinit();
    ed_note->Reinit();

    UpdateNote();
}

void Samplent::ProcessChar(char character)
{
    if(patt == aux_panel->workPt && aux_panel->workPt->ptype == Patt_StepSeq)
    {
        ed_note->ProcessChar(character);
    }
    /*
    else if(*character == 0x2E)  // dot transitions to command
    {
        Vanish(this);

        Scope st;
        memset(&st, 0, sizeof(Scope));
        st.instr = this->sample;
        Command* c = CreateCommand(st);

        C.mode = CMode_ElemEdit;
        C.curElem = c;
    }
    else if(*character == 0x5e)
    {
        SwitchVol();
    }
    else if(*character == 0x2a)
    {
        SwitchPan();
    }
    else if(character == 0x2f)
    {
        C.ExitToSlidingMode(this, ed_note->value);
        CTick += 2;
    }
    else */
    if(character == 0x3C) // <
    {
        revB = true;
    }
    else if(character == 0x24) // !
    {
        /*
        if(wave_visible == false)
        {
            wave_visible = true;
        }
        else
        {
            wave_visible = false;
        }
        */
    }
    else if(character == 0x3E) // >
    {
        revB = false;
    }
    else  // probably bukva character, process by text editor
    {
        DeleteElement(this, false, false);

        Txt* t = new Txt;
        AddNewElement(t, false);

        C.mode = CMode_ElemEdit;
        C.curElem = t;
		C.curParam = NULL;

		t->AddString(sample->alias->string);
        t->ProcessChar(character);
    }
}

void Samplent::SwitchOffs()
{
    if(ed_offs->visible == false)
    {
        ed_offs->visible = true;
		ed_offs->bypass = false;
    }
    else
    {
        ed_offs->bypass = true;
        ed_offs->visible = false;
        if(active_param == ed_offs)
        {
            Switch2Param(NULL);
        }
    }
}

void Samplent::ProcessKey(unsigned key, unsigned int flags)
{
    if(key == key_escape)
    {
        SwitchNote();
    }
    else if(key == key_right)
    {
        if(active_param == ed_note)
        {
            C.ExitToNoteMode(sample, ed_note->value);
			C.AdvanceTick(1.0f);
        }
        else
        {
            if(ed_vol->visible == true)
            {
                Switch2Param(ed_vol);
            }
            else if(ed_pan->visible == true)
            {
                Switch2Param(ed_pan);
            }
            else if(ed_len->visible == true)
            {
                Switch2Param(ed_len);
            }
            else if(ed_offs->visible == true)
            {
                Switch2Param(ed_offs);
            }
            else
            {
                C.ExitToDefaultMode();
                C.AdvanceTick(1.0f);
            }
        }
    }
    else if(key == key_up)
    {
        if(active_param == ed_note)
        {
            C.ExitToNoteMode(sample, ed_note->value);
            C.AdvanceLine(-1);
        }
        else
        {
            C.ExitToDefaultMode();
            C.AdvanceLine(-1);
        }
    }
    else if((key == key_down)||(key == key_return))
    {
        if(active_param == ed_note)
        {
            C.ExitToNoteMode(sample, ed_note->value);
            C.AdvanceLine(1);
        }
        else
        {
            C.ExitToDefaultMode();
            C.AdvanceLine(1);
        }
    }
    else if(key == key_left)
    {
        if(active_param == ed_note)
        {
            C.ExitToNoteMode(sample, ed_note->value);
            C.AdvanceTick(-1.0f);
        }
        else
        {
            C.ExitToDefaultMode();
            C.AdvanceTick(-1.0f);
        }
    }
    else if(key == key_backspace)
    {
        if(active_param == ed_note)
        {
            DeleteElement(this, false, false);
        }
        else
        {
            Txt* t = new Txt;
            AddNewElement(t, false);

            C.mode = CMode_ElemEdit;
            C.curElem = t;

            t->AddString(sample->alias->string);
            t->ProcessKey(key, flags);
            DeleteElement(this, false, false);
        }
    }
    else
    {
        Process_Key_Default(key, flags);
    }
}

bool Samplent::IsOutOfBounds(double* cursor)
{
    if(*cursor > smp_right_frame || *cursor < smp_left_frame)
    {
	    return true;
    }
    else
    {
	    return false;
    }
}

bool Samplent::InitCursor(double* cursor)
{
    if(revB == false)
    {
       *cursor = smp_left_frame;
    }
    else if(revB == true)
    {
       *cursor = smp_right_frame;
    }

    if(patt->ranged && (patt != field_pattern)&&((frame + *cursor) >= patt->end_frame))
    {
        return false;
    }
    else
    {
        return true;
    }
}

void Samplent::UpdateNote()
{
    if(preview == false && patt->ptype == Patt_Pianoroll)
    {
        ed_note->SetValue(NUM_PIANOROLL_LINES - track_line - 1);
    }
    if(ed_note->relative == false)
    {
        freq_ratio = 1.0f/CalcFreqRatio(ed_note->value - baseNote);
        freq_incr_base = CalcSampleFreqIncrement(sample, ed_note->value - baseNote);
    }
    else
    {
        freq_ratio = 1.0f/CalcFreqRatio(ed_note->value);
        freq_incr_base = CalcSampleFreqIncrement(sample, ed_note->value);
    }

    if(touchresized == false)
    {
        tick_length = Frame2Tick(sample_frame_len)*sample->rateUp*freq_ratio;
    }
    end_tick = start_tick + tick_length;
    CalcSlideNotes();
}

void Samplent::UpdateSampleBounds()
{
    // TODO: length and offset global_envelopes. ParamSet params for samplent
    if(sample != NULL)
    {
        if(revB == false)
        {
            if(ed_offs->value != 0)
            {
                smp_left_frame = (long)((float)(sample_frame_len - 1)/100*ed_offs->value);
            }
			/*
            else if(sample->params->offs->val != 0)
            {
                smp_left_frame = (long)((sample_frame_len - 1) * sample->params->offs->val);
            }*/
            else
            {
                smp_left_frame = 0;
            }

            if(ed_len->value != 100)
            {
                smp_right_frame = (long)((float)(sample_frame_len - 1)/ 100 * ed_len->value);
            }
			/*
            else if(sample->params->len->val != 1)
            {
                smp_right_frame = (long)((sample_frame_len - 1) * sample->params->len->val);
            }*/
            else
            {
                smp_right_frame = (sample_frame_len - 1);
            }

            //smp_right_frame = long(smp_right_frame*ksize);
        }
        else // rev == true
        {
            if(ed_offs->value != 0)
            {
            	smp_right_frame = (sample_frame_len - 1)/100 * (1 - ed_offs->value);
            }
			/*
            else if(sample->params->offs->val != 0)
            {
            	smp_right_frame = (long)((sample_frame_len - 1) * (1 - sample->params->offs->val));
            }*/
            else
            {
            	smp_right_frame = (sample_frame_len - 1);
            }

            if(ed_len->value != 100)
            {
            	smp_left_frame = (sample_frame_len - 1) * (1 - ed_len->value);
            }
			/*
            else if(sample->params->len->val != 1)
            {
            	smp_left_frame = (long)((sample_frame_len - 1)*(1 - sample->params->len->val));
            }*/
            else
            {
            	smp_left_frame = 0;
            }

            //smp_left_frame = long(smp_left_frame*ksize);
        }

        if(patt->ranged == true)
        {
    		long p_offs = 0;
    		long p_cut = 0;
    		if((patt != NULL)&&(patt != field_pattern))
    		{
    			if(frame < 0)
    			{
    				p_offs = (long)(abs(frame)*sample->rateDown);
    			}
    			else
    			{
    				p_offs = 0;
    			}

    			if(patt->frame_length < end_frame)
    			{
    				p_cut = (long)(abs(end_frame - patt->frame_length)*(sample->rateDown));
    			}
    			else
    			{
    				p_cut = 0;
    			}
    		}

    		if(revB == false)
    		{
    			if(p_offs > smp_left_frame)
    			{
    				smp_left_frame = p_offs;
    			}

    			if(p_cut > ((sample_frame_len - 1) - smp_right_frame))
    			{
    				smp_right_frame = (sample_frame_len - 1) - p_cut;
    			}
    		}
    		else if(revB == true)
    		{
    			if(p_offs > ((sample_frame_len - 1) - smp_right_frame))
    			{
    				smp_right_frame = (sample_frame_len - 1) - p_offs;
    			}

    			if(p_cut > smp_left_frame)
    			{
    				smp_left_frame = p_cut;
    			}
    		}
        }
	}
}

void Samplent::Update(bool tonly)
{
    if(tonly)
    {
        UpdateNote();
        CalcTiming();
    }
    else
    {
        UpdateNote();
        UpdateTrackDataFromLine();
        CalcTiming();
        Globalize();
        track_params = field_trkdata->params;

        UpdateSampleBounds();
        CheckVisibility();

        Relocate_Element(this);
    }
}

void Samplent::Save(XmlElement * xmlNode)
{
    Instance::Save(xmlNode);
    xmlNode->setAttribute(T("TouchResized"), int(touchresized));
    xmlNode->setAttribute(T("Reversed"), int(revB));
}

void Samplent::Load(XmlElement * xmlNode)
{
    Instance::Load(xmlNode);
    touchresized = xmlNode->getBoolAttribute(T("TouchResized"));
    revB = xmlNode->getBoolAttribute(T("Reversed"));
}

void EffElement::Move(float dtick, int dtrack)
{
    if(dtrack != 0)
    {
        DeactivateAllTriggers();
    }
    Element::Move(dtick, dtrack);
}

SlideNote::SlideNote(int note_num, Instance* par) : Slide(0)
{
    type = El_SlideNote;

	parent = par;
    end_tick = start_tick/* + C.lastslidelength*/;

    loc_vol = new Vol(100);
    loc_pan = new Pan(50);

    // Vol
    //ed_vol = new Digits(100, 0, this);
    ed_vol = new Percent(100, 3, this, Param_Vol);
    AddParamedit(ed_vol);
    loc_vol->AddParamedit(ed_vol);

    // Pan
    //ed_pan = new Digits(50, 0, this);
    ed_pan = new Percent(50, 2, this, Param_Pan);
    AddParamedit(ed_pan);
    loc_pan->AddParamedit(ed_pan);

    // Note
    ed_note = new Note(note_num, this);
    AddParamedit(ed_note);
}

SlideNote* SlideNote::Clone(bool add)
{
    SlideNote* clone = CreateElement_SlideNote(false);

    clone->ed_note->SetValue(ed_note->value);
    clone->ed_note->visible = ed_note->visible;
    clone->loc_vol->SetNormalValue(loc_vol->val);
    clone->loc_pan->SetNormalValue(loc_pan->val);
    clone->start_tick = start_tick;
    clone->end_tick = end_tick;
    clone->track_line = track_line;
    clone->trknum = trknum;
    clone->tick_length = tick_length;
    clone->Switch2Param(clone->ed_note);
    if(add)
    {
        AddNewElement(clone, false);
    }
    else
    {
        clone->Update();
    }

    return clone;
}

void SlideNote::UpdateNote()
{
    if(patt->ptype == Patt_Pianoroll)
    {
        ed_note->SetValue(NUM_PIANOROLL_LINES - track_line - 1);
    }

    if(ed_note->relative == true)
    {
        fR = CalcFreqRatio(ed_note->value);
    }

	if(parent != NULL)
		parent->CalcSlideNotes();
}

void SlideNote::Activate()
{
    CTick = start_tick;
    CLine = track_line;
	if(last_edited_param == NULL)
	{
		Switch2Param(ed_note);
	}
	else
	{
		Switch2Param(last_edited_param);
	}
}

void SlideNote::ProcessKey(unsigned int key, unsigned int flags)
{
    float qsize = C.qsize;
    if(key == key_right)
    {
        //C.ExitToSlidingMode(parent, ed_note->value);
		C.ExitToDefaultMode();
		C.AdvanceTick(1);
    }
    else if(key == key_up)
    {
        //C.ExitToSlidingMode(parent, ed_note->value);
		C.ExitToDefaultMode();
		C.AdvanceLine(-1);
    }
    else if((key == key_down)||(key == key_return))
    {
        //C.ExitToSlidingMode(parent, ed_note->value);
		C.ExitToDefaultMode();
        C.AdvanceLine(1);
	}
    else if(key == key_left)
    {
        //C.ExitToSlidingMode(parent, ed_note->value);
		C.ExitToDefaultMode();
        C.AdvanceTick(-1);
	}
    else if(key == key_escape)
    {
        // Do nothing, hehehe
    }
    else if(key == key_backspace)
    {
        if(active_param == ed_note)
        {
    		C.ExitToSlidingMode(parent, ed_note->value);
            DeleteElement(this, false, false);
        }
    }
    else
    {
        Process_Key_Default(key, flags);
    }
}

void SlideNote::Move(float dtick, int dtrack)
{
    if(parent == NULL && dtrack != 0)
    {
        DeactivateAllTriggers();
    }

    track_line += dtrack;
    start_tick += dtick;
    end_tick = start_tick + tick_length;
	Update();
	if(parent != NULL && picked == true && parent->picked == false)
	{
		parent->RearrangeSlideNotes();
	}
}

void SlideNote::Update(bool tonly)
{
    if(tonly)
        CalcTiming();
    else
    {
        UpdateTrackDataFromLine();
        CalcTiming();
        Globalize();

        track_params = field_trkdata->params;

        UpdateNote();
        CheckVisibility();

        //mixcell = mix->trkfxcell[field_trknum];
        Relocate_Element(this);
    }
}

float SlideNote::GetMult(Trigger* tg, Trigger* tgi, long framephase, long num_frames)
{
    float m;
    //if(parent == NULL)
    {
        fR = tg->freq/tgi->freq;
        nN = (float)framephase/frame_length;
        m = (1 + nN*(fR - 1));

        if(framephase + num_frames >= frame_length) // if slide is finishing
        {
            float mfinal = (1 + float((framephase + num_frames)/(frame_length))*(fR - 1));  // get final multiplier
            tgi->freq_incr_base *= mfinal;
            tgi->freq *= mfinal;
        }
    }
    /*
    else
    {
        m = Slide::GetMult(tg, tgi, framephase, num_frames);
    }
    */

    return m;
}

float SlideNote::GetVolMult(Trigger* tg, Trigger* tgi, long framephase, long num_frames)
{
    nN = (float)framephase/(frame_length);
    float m = nN*(tg->vol_base - tgi->vol_base);

    if(framephase + num_frames >= frame_length)
    {
        float mfinal = float((framephase + num_frames)/(frame_length))*(tg->vol_base - tgi->vol_base);
        tgi->vol_base += m;
    }

    return m;
}

float SlideNote::GetPanMult(Trigger* tg, Trigger* tgi, long framephase, long num_frames)
{
    nN = (float)framephase/(frame_length);
    float m = nN*(tg->pan_base - tgi->pan_base);

    if(framephase + num_frames >= frame_length)
    {
        float mfinal = float((framephase + num_frames)/(frame_length))*(tg->pan_base - tgi->pan_base);
        tgi->pan_base += m;
    }

    return m;
}

float SlideNote::GetSignalIncr(Trigger* tg, Trigger* tgi, long framephase, long num_frames)
{
    return (float)(framephase + num_frames)/(frame_length);
}

void SlideNote::Delete()
{
    if(parent != NULL)
        parent->CalcSlideNotes();
}

void SlideNote::UnDelete()
{
    if(parent != NULL)
        parent->CalcSlideNotes();
}

void SlideNote::Save(XmlElement * xmlNode)
{
    Element::Save(xmlNode);

    xmlNode->setAttribute(T("Volume"), loc_vol->val);
    xmlNode->setAttribute(T("Panning"), loc_pan->val);
    xmlNode->setAttribute(T("Note"), ed_note->value);
    xmlNode->setAttribute(T("NoteVisible"), int(ed_note->visible));
}

void SlideNote::Load(XmlElement * xmlNode)
{
    Element::Load(xmlNode);

	loc_vol->SetNormalValue((float)xmlNode->getDoubleAttribute(T("Volume")));
    loc_pan->SetNormalValue((float)xmlNode->getDoubleAttribute(T("Panning")));
    ed_note->visible = xmlNode->getBoolAttribute(T("NoteVisible"));
    ed_note->SetValue(xmlNode->getIntAttribute(T("Note")));
}

Command::Command(Scope* sc)
{
    type = El_Command;
    cmdtype = Cmd_Default;
    com_num = 0;
    reset_command = false;
    queued = false;

    paramedit = NULL;
    param = NULL;
    toggle = NULL;

    scope = sc;
    target_params = NULL;
    eff_target = NULL;
}

Command::Command(Scope* sc, CmdType ct)
{
    type = El_Command;
    reset_command = false;
    queued = false;

    paramedit = NULL;
    param = NULL;
    toggle = NULL;

    scope = sc;
    target_params = NULL;
    eff_target = NULL;

    cmdtype = ct;
    com_num = MapCType2CNum(ct);
    ChangeParam();
}

Command::Command(Scope* sc, CmdType ct, Parameter* prm)
{
    type = El_Command;
    reset_command = false;
    queued = false;

    param = prm;
    paramedit = NULL;
    toggle = NULL;

    scope = sc;
    target_params = NULL;
    eff_target = NULL;

    cmdtype = ct;
    com_num = MapCType2CNum(ct);
    ChangeParam();
}

Command* Command::Clone(bool add)
{
    Command* clone = CreateElement_Command(scope, false);

    clone->com_num = com_num;
    clone->cmdtype = cmdtype;
    clone->reset_command = reset_command;
    clone->param_type = param_type;
    clone->val = val;
    clone->param = param;

    //if(clone->reset_command == false)
    //{
        //clone->InitCommParam(); // TODO: params copying
    //}
    if(paramedit != NULL)
    {
        clone->paramedit = paramedit->Clone(clone);
    }

    clone->start_tick = start_tick;
    clone->end_tick = end_tick;
    clone->track_line = track_line;
    clone->trknum = trknum;
    clone->tick_length = tick_length;

    return clone;
}

void Command::ProcessChar(char character)
{
    switch(character)
    {
        case 0x5e: // ^
            cmdtype = Cmd_Vol;
            com_num = MapCType2CNum(cmdtype);
            ChangeParam();
            break;
        case 0x2a: // *
            cmdtype = Cmd_Pan;
            com_num = MapCType2CNum(cmdtype);
            ChangeParam();
            break;
        case 0x23: // #
            cmdtype = Cmd_Mute;
            com_num = MapCType2CNum(cmdtype);
            ChangeParam();
            break;
//        case 0x2f: // /
//            SwitchEnvelope();
//            break;
        case 0x3d: // =
            if(scope->for_track == false)
            {
                scope->local = false;
                scope->for_track = true;
            }
            else
            {
                scope->for_track = false;
            }
            break;
        case 0x40: // @
            if(scope->local == false)
            {
                scope->for_track = false;
                scope->local = true;
            }
            else
            {
                scope->local = false;
            }
            break;
        case 0x5f: // _
            if(reset_command == false)
            {
                if(paramedit != NULL)
                {
                    delete paramedit;
                    paramedit = NULL;
                }
                reset_command = true;
            }
            else
            {
                reset_command = false;
            }
            break;
        case 0x24: // $
            // Add timing
            break;
    }
}

void Command::SwitchEnvelope()
{
    if(cmdtype == Cmd_Vol)
    {
        cmdtype = Cmd_VolEnv;
        com_num = MapCType2CNum(cmdtype);
        ChangeParam();
    }
    else if(cmdtype == Cmd_VolEnv)
    {
        cmdtype = Cmd_Vol;
        com_num = MapCType2CNum(cmdtype);
        ChangeParam();
    }
    else if(cmdtype == Cmd_Pan)
    {
        cmdtype = Cmd_PanEnv;
        com_num = MapCType2CNum(cmdtype);
        ChangeParam();
    }
    else if(cmdtype == Cmd_PanEnv)
    {
        cmdtype = Cmd_Pan;
        com_num = MapCType2CNum(cmdtype);
        ChangeParam();
    }
}

void Command::ProcessKey(unsigned int key, unsigned int flags)
{
    if(key == key_down)
    {
    /*
        com_num++;
        if(com_num > 6)
        {
            com_num = 1;
        }
        cmdtype = MapCNum2CType(com_num);
        ChangeParam();
    */
    }
    else if(key == key_up)
    {
    /*
        com_num--;
        if(com_num < 1)
        {
            com_num = 6;
        }
        cmdtype = MapCNum2CType(com_num);
        ChangeParam();
    */
    }
    else if(key == key_right)
    {
        if((reset_command == true)||(cmdtype == Cmd_Mute))
        {
            C.AdvanceLine(1);
            C.ExitToDefaultMode();
        }
        else
        {
            Switch2Param(paramedit);
        }
    }
	else if(key == key_return)
	{
        C.AdvanceLine(1);
        C.ExitToDefaultMode();
	}
    else
    {
        Process_Key_Default(key, flags);
    }
}

void Command::ChangeParam()
{
    if(reset_command == false)
    {
        if(paramedit != NULL)
        {
            Switch2Param(NULL);
            delete paramedit;
            paramedit = NULL;
        }
        InitCommParam();
    }
}

void Command::InitCommParam()
{
    switch(cmdtype)
    {
        case Cmd_Vol:
            InitVol();
            break;
        case Cmd_Pan:
            InitPan();
            break;
        case Cmd_VolEnv:
            InitVolEnv();
            break;
        case Cmd_PanEnv:
            InitPanEnv();
            break;
        case Cmd_LocVolEnv:
            InitLocVolEnv();
            break;
        case Cmd_LocPanEnv:
            InitLocPanEnv();
            break;
        case Cmd_ParamEnv:
            InitParamEnv();
            break;
        case Cmd_PitchEnv:
            InitPitchEnv();
            break;
        case Cmd_Mute:
            InitMute();
            break;
        case Cmd_Len:
            InitLen();
            break;
        case Cmd_Offs:
            InitOffs();
            break;
        default:
            Leave();
            break;
    }
}

void Command::InitMute()
{
    paramedit = NULL;
}

void Command::InitVol()
{
    Percent* vol = new Percent(100, 3, this, Param_Vol);
    paramedit = vol;
    val.n_vol = 1;
}

void Command::InitLen()
{
    Percent* len = new Percent(100, 3, this, Param_Len);
    paramedit = len;
    val.n_len = 1;
}

void Command::InitOffs()
{
    Percent* offs = new Percent(0, 3, this, Param_Len);
    offs->curPos = 0;
    offs->maxPos = 0;
    paramedit = offs;
    val.n_offs = 1;
}

void Command::InitPan()
{
    Percent* pan = new Percent(50, 2, this, Param_Pan);
    paramedit = pan;
    val.n_pan = 0;
}

void Command::InitVolEnv()
{
    Envelope* v_env = new Envelope(cmdtype);

    v_env->element = this;
    v_env->hgt = 40;
    v_env->len = float(ticks_per_beat*beats_per_bar);

    if(param == NULL)
    {
        v_env->AddPoint(0.0f, 0.9f);
        v_env->AddPoint(3.3f, 1.0f);
        v_env->AddPoint(8.5f, 0.4f);
    }
    else
    {
        v_env->AddPoint(0.0f, param->val*InvVolRange);
    }

    v_env->prev = NULL;
    v_env->next = NULL;

    paramedit = v_env;
}

void Command::InitPanEnv()
{
    Envelope* p_env = new Envelope(cmdtype);

    p_env->element = this;
    p_env->hgt = 40;
    p_env->len = float(ticks_per_beat*beats_per_bar);

    if(param == NULL)
    {
        p_env->AddPoint(0.0f, 0.9f);
        p_env->AddPoint(3.3f, 1.0f);
        p_env->AddPoint(8.5f, 0.4f);
    }
    else
    {
        p_env->AddPoint(0.0f, (2 - (param->val + 1))/2);
    }

    p_env->prev = NULL;
    p_env->next = NULL;

    paramedit = p_env;
}

void Command::InitLocVolEnv()
{
    Envelope* v_env = new Envelope(cmdtype);

    v_env->SetSustainable(true);
    v_env->element = this;
    v_env->hgt = 40;
    v_env->len = 3;

    if(scope->instr != NULL)
    {
        if(scope->instr->type == Instr_Sample)
        {
            v_env->AddPoint(0.0f, 1.0f/VolRange);
            v_env->SetSustainPoint(v_env->AddPoint(1.0f, 1.0f/VolRange));
        }
        else
        {
            v_env->AddPoint(0.0f, 1.0f/VolRange);
            v_env->SetSustainPoint(v_env->AddPoint(0.1f, 1.0f/VolRange));
            v_env->AddPoint(0.2f, 0.25f/VolRange);
            v_env->AddPoint(0.5f, 0.1f/VolRange);
            v_env->AddPoint(0.7f, 0.0f/VolRange);
        }
    }

    v_env->prev = NULL;
    v_env->next = NULL;

    paramedit = v_env;
}

void Command::InitLocPanEnv()
{
    Envelope* p_env = new Envelope(cmdtype);

    p_env->element = this;
    p_env->hgt = 40;
    p_env->len = 3;

    p_env->AddPoint(0.0f, 0.5f);
    p_env->AddPoint(1.3f, 0.5f);

    p_env->prev = NULL;
    p_env->next = NULL;

    paramedit = p_env;
}

void Command::InitParamEnv()
{
    Envelope* p_env = new Envelope(cmdtype);

    p_env->element = this;
    p_env->hgt = 40;
    p_env->len = float(ticks_per_beat*beats_per_bar);

    if(param == NULL)
    {
        p_env->AddPoint(0.0f, 0.9f);
        p_env->AddPoint(1.3f, 0.2f);
        p_env->AddPoint(7.5f, 0.9f);
    }
    else
    {
        p_env->AddPoint(0.0f, (param->val - param->offset)/param->range);
    }

    p_env->prev = NULL;
    p_env->next = NULL;

    paramedit = p_env;
}

void Command::InitPitchEnv()
{
    Envelope* p_env = new Envelope(cmdtype);

    p_env->element = this;
    p_env->hgt = 41;
    p_env->len = 15;

    p_env->AddPoint(0.0f, 0.5f);

    p_env->prev = NULL;
    p_env->next = NULL;

    paramedit = p_env;
}

void Command::Update(bool tonly)
{
    if(tonly)
    {
        if(IsEnvelope() && paramedit != NULL)
        {
            Envelope* env = (Envelope*)paramedit;
            env->CalcTime();
            tick_length = env->len;
            end_tick = start_tick + tick_length;
        }

        CalcTiming();
    }
    else
    {
        if(IsEnvelope() && paramedit != NULL)
        {
            Envelope* env = (Envelope*)paramedit;
            env->CalcTime();
            tick_length = env->len;
            end_tick = start_tick + tick_length;
        }

        UpdateTrackDataFromLine();
        CalcTiming();
        Globalize();

        track_params = field_trkdata->params;

        target_params = GetTargetParams();
        eff_target = GetTargetSym();

        switch(cmdtype)
        {
            case Cmd_Vol:
                val.n_vol = Div100(float(((Percent*)paramedit)->value));
                param = target_params->vol;
                break;
            case Cmd_Pan:
                val.n_vol = Calc_PercentPan((float)((Percent*)paramedit)->value);
                param = target_params->pan;
                break;
            case Cmd_Len:
                val.n_len = Div100(float(((Percent*)paramedit)->value));
                param = target_params->len;
                break;
            case Cmd_Offs:
                val.n_offs = Div100(float(((Percent*)paramedit)->value));
                param = target_params->offs;
                break;
            default:
                //Leave();
                break;
        }

        CheckVisibility();
        Relocate_Element(this);
    }
}

void Command::Reset()
{
    queued = false;
}

Trigger** Command::GetTargetSym()
{
    Trigger** target = NULL;
	if(scope != NULL)
	{
		if(scope->local == true)
		{
			target = &(patt->symtrig);
		}
		else if(scope->pt != NULL)
		{
			target = &(scope->pt->symtrig);
		}
		else if(scope->for_track == true)
		{
			target = &(field_trkdata->fxstate.freqtrig);
		}
		else    // Global scope
		{
			target = &(mAster.efftrig);
		}
	}
    return target;
}

ParamSet* Command::GetTargetParams()
{
    ParamSet* target = NULL;
	if(scope != NULL)
	{
		if(scope->local == true)
		{
			// local pattern
			target = patt->params;
		}
		else if(scope->pt != NULL)
		{
			// external pattern
			target = scope->pt->params;
		}
		else if(scope->mixcell != NULL)
		{
			target = scope->mixcell->params;
		}
		else if(scope->for_track == true)
		{
			if(scope->trkdata == NULL)
			{
				target = field_trkdata->params;
			}
			else
			{
				target = scope->trkdata->params;
			}
		}
		else    // Global scope
		{
			if(scope->instr != NULL)
			{
				target = scope->instr->params;
			}
			else
			{
				target = mAster.params;
			}
		}
	}
    return target;
}

void Command::ApplyInstant()
{
    if(cmdtype == Cmd_Mute)
    {
        if(target_params != NULL)
        {
            if(scope->instr != NULL)
            {
				scope->instr->mute->MouseClick();
            }
            else if(scope->trkdata != NULL)
            {
				scope->trkdata->mute->MouseClick();
            }
            else if(scope->mixcell != NULL)
            {
                scope->mixcell->mute_toggle->MouseClick();
            }
        }
    }
    else if(cmdtype == Cmd_Solo)
    {
        if(target_params != NULL)
        {
            if(scope->instr != NULL)
            {
                scope->instr->solo->MouseClick();
            }
            else if(scope->trkdata != NULL)
            {
                scope->trkdata->solo->MouseClick();
            }
            else if(scope->mixcell != NULL)
            {
                scope->mixcell->solo_toggle->MouseClick();
            }
        }
    }
    else if(cmdtype == Cmd_Bypass)
    {
        if(scope->mixcell != NULL)
        {
            scope->mixcell->bypass = !scope->mixcell->bypass;
        }
    }
    else if(cmdtype == Cmd_Vol)
    {
        if(reset_command == false)
        {
            target_params->vol->SetNormalValue(val.n_vol);
        }
        else
        {
            target_params->vol->Reset();
        }
    }
    else if(cmdtype == Cmd_Pan)
    {
        if(reset_command == false)
        {
            target_params->pan->SetNormalValue(val.n_pan);
        }
        else
        {
            target_params->pan->Reset();
        }
    }
    else if(cmdtype == Cmd_Len)
    {
        if(reset_command == false)
        {
            target_params->len->SetNormalValue(val.n_len);
        }
        else
        {
            target_params->len->Reset();
        }
    }
    else if(cmdtype == Cmd_Offs)
    {
        if(reset_command == false)
        {
            target_params->offs->SetNormalValue(val.n_offs);
        }
        else
        {
            target_params->offs->Reset();
        }
    }
}

void Command::Hide()
{
    if(IsEnvelope())
    {
        Envelope* env = (Envelope*)paramedit;
        env->fold->Deactivate();
        if(env->timetg != NULL)
        {
            env->timetg->Deactivate();
        }
    }
}

void Command::Show()
{
    if(IsEnvelope())
    {
        Envelope* env = (Envelope*)paramedit;
        env->fold->Activate();
        if(env->timetg != NULL)
        {
            env->timetg->Activate();
        }
    }
}

void Command::CheckVisibility()
{
    Element::CheckVisibility();

    if(trkdata != NULL && displayable == true)
    {
        if(IsEnvelope())
        {
            Envelope* env = (Envelope*)paramedit;
            if(base == field_pattern)
            {
                if(patt->folded == false)
                {
                    int x1 = StartTickX();
                    int x2 = EndTickX();
                    int y1 = TrackLine2Y() - lineHeight;
                    int y2 = EndLine2Y();
                    if(CheckPlaneCrossing(x1, 
                                          y1, 
                                          x1 + env->edx, 
                                          y2 + env->hgt, 
                                          GridX1, GridY1, GridX2, GridY2))
                    {
                        visible = true;
                    }
                }
            }
            else if(base == st->patt)
            {
                if(patt->folded == false)
                {
                    int x1 = st_StartTickX();
                    int x2 = st_EndTickX();
                    int y1 = TrackLine2Y() - lineHeight;
                    int y2 = EndLine2Y();
                    if(CheckPlaneCrossing(x1, 
                                          y1, 
                                          x1 + env->edx, 
                                          y2 + env->hgt, 
                                          StX1, GridY1, StX2, GridY2))
                    {
                        visible = true;
                    }
                }
            }

            if(patt == aux_panel->workPt->OrigPt && aux_panel->auxmode == AuxMode_Pattern)
            {
                int x1 = s_StartTickX();
                int x2 = s_EndTickX();
                int y1 = s_TrackLine2Y() - lineHeight;
                int y2 = s_EndLine2Y();
                if(CheckPlaneCrossing(x1, 
                                      y1, 
                                      x1 + env->edx, 
                                      y2 + env->hgt, 
                                      GridXS1, GridYS1, GridXS2, GridYS2))
                {
                    auxvisible = true;
                }
            }
        }
    }
}

void Command::Delete()
{
    if(IsEnvelope() == true)
    {
        Envelope* env = (Envelope*)paramedit;
		if(env->fold != NULL)
			env->fold->Deactivate();
		if(env->timetg != NULL)
			env->timetg->Deactivate();
    }
}

void Command::UnDelete()
{
    if(IsEnvelope() == true)
    {
        Envelope* env = (Envelope*)paramedit;
		if(env->fold != NULL)
			env->fold->Activate();
		if(env->timetg != NULL)
	        env->timetg->Activate();
    }
}

bool Command::IsEnvelope()
{
    if(cmdtype == Cmd_Envelope ||
       cmdtype == Cmd_LocVolEnv ||
       cmdtype == Cmd_LocPanEnv ||
       cmdtype == Cmd_VolEnv ||
       cmdtype == Cmd_PanEnv ||
       cmdtype == Cmd_ParamEnv ||
       cmdtype == Cmd_PitchEnv)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Command::Save(XmlElement * xmlNode)
{
    Element::Save(xmlNode);
    xmlNode->setAttribute(T("CmdType"), int(cmdtype));

    if(param != NULL)
    {
        xmlNode->setAttribute(T("ParamIndex"), param->globalindex);
    }

    if(scope != NULL)
    {
        if(scope->pt != NULL)
        {
            xmlNode->setAttribute(T("CmdPattIndex"), scope->pt->origindex);
        }

        if(scope->instr != NULL)
        {
            xmlNode->setAttribute(T("InstrIndex"), scope->instr->index);
        }

        if(scope->eff != NULL)
        {
            xmlNode->setAttribute(T("EffIndex"), scope->eff->index);
        }
		else if(scope->mixcell != NULL)
        {
            xmlNode->setAttribute(T("CellIndex"), scope->mixcell->indexstr);
        }
    }

    if(IsEnvelope())
    {
        Envelope* env = (Envelope*)paramedit;
        XmlElement* xmlenv = env->Save("Envelope");
        xmlNode->addChildElement(xmlenv);
    }
}

void Command::Load(XmlElement * xmlNode)
{
    Element::Load(xmlNode);
    cmdtype = (CmdType)xmlNode->getIntAttribute(T("CmdType"));

    int index;

	index = xmlNode->getIntAttribute(T("ParamIndex"), -1);
    if(index != -1)
    {
        param = GetParamByGlobalIndex(index);
    }

	index = xmlNode->getIntAttribute(T("CmdPattIndex"), -1);
    if(index != -1)
    {
        Pattern* patt = GetPatternByIndex(index);
        scope = &patt->scope;
    }
    else 
    {
		index = xmlNode->getIntAttribute(T("InstrIndex"), -1);
		if(index != -1)
		{
			Instrument* instr = GetInstrumentByIndex(index);
			scope = &instr->scope;
		}
		else
		{
			index = xmlNode->getIntAttribute(T("EffIndex"), -1);
			if(index != -1)
			{
				Eff* eff = GetEffByIndex(index);
				scope = &eff->scope;
			}
			else
			{
				index = xmlNode->getIntAttribute(T("CellIndex"), -1);
				if(index != -1)
				{
					char indexstr[15];
					xmlNode->getStringAttribute(T("CellIndex")).copyToBuffer(indexstr, 15);
					MixChannel* mchan = aux_panel->GetMixChannelByIndexStr(indexstr);
					if(mchan != NULL)
					{
						scope = &mchan->mc_main->scope;
					}
				}
			}
		}
    }

    if(IsEnvelope())
    {
        XmlElement* xmlenv = xmlNode->getChildByName(T("Envelope"));
        if(xmlenv != NULL)
        {
            Envelope* env = new Envelope(cmdtype);
            env->Load(xmlenv);
			paramedit = env;
            paramedit->element = this;
        }
    }
}

Muter::Muter()
{
    type = El_Mute;
    end_tick = start_tick + 1;
}

Muter* Muter::Clone(bool add)
{
    Muter* clone = CreateElement_Muter();
    clone->mute_param = mute_param;

    clone->start_tick = start_tick;
    clone->end_tick = end_tick;
    clone->track_line = track_line;
    clone->trknum = trknum;
    clone->tick_length = tick_length;

    return clone;
}

void Muter::ProcessKey(unsigned int key, unsigned int flags)
{
    switch(key)
    {
        case key_up:
        {
            C.AdvanceLine(-1);
            C.ExitToDefaultMode();
        }break;
		case key_return:
		case key_down:
		{
            C.AdvanceLine(1);
            C.ExitToDefaultMode();
		}break;
		case key_right:
		{
            C.AdvanceTick(1);
            C.ExitToDefaultMode();
		}break;
		case key_left:
		{
            C.AdvanceTick(-1);
            C.ExitToDefaultMode();
		}break;
        case key_backspace:
        {
            end_tick--;
        	if(end_tick <= start_tick)
        	{
                DeleteElement(this, false, false);
            }
            else
            {
                Update();
            }
        }break;
        default:
        {
            Process_Key_Default(key, flags);
        }
    }
}

void Muter::ProcessChar(char character)
{
    if(character == 0x23) // #
    {
        end_tick++;
        Update();
    }
}

Transpose::Transpose(int semitones)
{
    type = El_Transpose;
    end_tick = start_tick + 1;
    smt = new Semitones(semitones, this);
}

Transpose* Transpose::Clone(bool add)
{
    Transpose* clone = CreateElement_Transpose(smt->value);

    clone->start_tick = start_tick;
    clone->end_tick = end_tick;
    clone->track_line = track_line;

    clone->trknum = trknum;
    clone->tick_length = tick_length;

    return clone;
}

void Transpose::ProcessChar(char character)
{
    if(character == 0x25)
    {
        end_tick++;
        Update();
    }
}

void Transpose::UpdateNote()
{
    freq_mult = CalcFreqRatio(smt->value);
}

void Transpose::ProcessKey(unsigned int key, unsigned int flags)
{
    switch(key)
    {
        case key_down:
        case key_up:
            smt->ProcessKey(key, flags);
            Update();
            break;
		case key_return:
            C.ExitToDefaultMode();
            C.AdvanceLine(1);
		    break;
		case key_right:
            C.ExitToDefaultMode();
			C.SetPos(end_tick + 1, CLine);
		    break;
		case key_left:
            C.ExitToDefaultMode();
            C.AdvanceTick(-1);
		    break;
        case key_backspace:
            end_tick--;

        	if(end_tick <= start_tick)
        	{
                DeleteElement(this, false, false);
            }
            else
            {
                CalcTiming();
            }
            break;
        default:
        {
            Process_Key_Default(key, flags);
        }
    }
}

void Transpose::Save(XmlElement * xmlNode)
{
    Element::Save(xmlNode);

    xmlNode->setAttribute(T("Semitones"), smt->value);
}

void Transpose::Load(XmlElement * xmlNode)
{
    Element::Load(xmlNode);

    smt->SetValue(xmlNode->getIntAttribute(T("Semitones")));
}

Break::Break()
{
    type = El_Break;
}

Break* Break::Clone(bool add)
{
    Break* clone = CreateElement_Break();
    clone->break_param = break_param;

    clone->start_tick = start_tick;
    clone->end_tick = end_tick;
    clone->track_line = track_line;
    clone->trknum = trknum;
    clone->tick_length = tick_length;

    return clone;
}

void Break::ProcessKey(unsigned int key, unsigned int flags)
{
    switch(key)
    {
        case key_up:
        {
            C.AdvanceLine(-1);
            C.ExitToDefaultMode();
        }break;
		case key_return:
		case key_down:
		{
            C.AdvanceLine(1);
            C.ExitToDefaultMode();
		}break;
		case key_right:
		{
            C.SetPos(end_tick + 1, CLine);
            C.ExitToDefaultMode();
		}break;
		case key_left:
		{
            C.AdvanceTick(-1);
            C.ExitToDefaultMode();
		}break;
        case key_backspace:
        {
            end_tick--;

            if(end_tick <= start_tick)
            {
                DeleteElement(this, false, false);
            }
        }break;
        default:
        {
            Process_Key_Default(key, flags);
        }
    }
}

Frequencer::Frequencer()
{
    freq_prev = freq_next = NULL;
}

Slide::Slide(int semitones)
{
    type = El_Slider;

    end_tick = start_tick + 1;
    smt = new Semitones(semitones, this);
}

Slide* Slide::Clone(bool add)
{
    Slide* clone = CreateElement_Slide(smt->value);

    clone->start_tick = start_tick;
    clone->end_tick = end_tick;
    clone->track_line = track_line;

    clone->trknum = trknum;
    clone->tick_length = tick_length;

    return clone;
}

void Slide::CalcSlide()
{
    fR = CalcFreqRatio(smt->value);
}

float Slide::GetMult(Trigger * tg, Trigger * tgi, long framephase, long num_frames)
{
    nN = (float)framephase/(frame_length - 1);
    float m = (1 + nN*(fR - 1));

    if(framephase + num_frames >= frame_length)
    {
        float mfinal = (1 + float((framephase + num_frames)/(frame_length))*(fR - 1));
        tgi->freq_incr_base *= mfinal;
        tgi->freq *= mfinal;
    }

    return m;
}

void Slide::ProcessChar(char character)
{
    if(character == 0x2F)
    {
        end_tick++;
        Update();
    }
}

void Slide::Update(bool tonly)
{
    Element::Update(tonly);
	if(!tonly)
		CalcSlide();
}

void Slide::ProcessKey(unsigned int key, unsigned int flags)
{
    switch(key)
    {
        case key_down:
        case key_up:
            smt->ProcessKey(key, flags);
            break;
		case key_return:
            C.AdvanceLine(1);
            C.ExitToDefaultMode();
		    break;
		case key_right:
            C.SetPos(end_tick + 1, CLine);
            C.ExitToDefaultMode();
		    break;
		case key_left:
            C.AdvanceTick(-1);
            C.ExitToDefaultMode();
		    break;
        case key_backspace:
            end_tick--;

        	if(end_tick <= start_tick)
        	{
                DeleteElement(this, false, false);
            }
            else
            {
                CalcTiming();
            }
            break;
        default:
        {
            Process_Key_Default(key, flags);
        }
    }
    Update();
}

void Slide::Save(XmlElement * xmlNode)
{
    Element::Save(xmlNode);

    xmlNode->setAttribute(T("Semitones"), smt->value);
}

void Slide::Load(XmlElement * xmlNode)
{
    Element::Load(xmlNode);

    smt->SetValue(xmlNode->getIntAttribute(T("Semitones")));
}

Vibrate::Vibrate()
{
    type = El_Vibrate;
    end_tick = start_tick + 1;

    freq = 9.0f;
    range = 0.3f;
}

Vibrate::Vibrate(float freq, float range)
{
    type = El_Vibrate;
    end_tick = start_tick + 1;

    freq = freq;
    range = range;
}

Vibrate* Vibrate::Clone(bool add)
{
    Vibrate* clone = CreateElement_Vibrate();
    clone->freq = freq;
    clone->range = range;

    clone->start_tick = start_tick;
    clone->end_tick = end_tick;
    clone->track_line = track_line;
    clone->trknum = trknum;
    clone->tick_length = tick_length;

    return clone;
}

float Vibrate::GetMult(Trigger * tg, Trigger * tgi, long framephase, long num_frames)
{
    //float t = (float)framephase/fSampleRate;
    //t *= (float)PI;
    //t *= val.freq;
    //float s = sin(t);   

    double wt_pos = freq/2.f*framephase;
    float s = wt_sine[int(wt_pos*wt_coeff)&0x7FFF];

    s = 1 + s*(((float)freq_mul_table[1]- 1)*range);

	return s;
}

void Vibrate::ProcessChar(char character)
{
    if(character == 0x7e)
    {
        end_tick++;
        Update();
    }
}

void Vibrate::ProcessKey(unsigned int key, unsigned int flags)
{
    switch(key)
    {
        case key_down:
            freq -= 0.1f;
            break;
        case key_up:
            freq += 0.1f;
            break;
		case key_return:
            C.AdvanceLine(1);
            C.ExitToDefaultMode();
		    break;
		case key_right:
            C.SetPos(end_tick + 1, CLine);
            C.ExitToDefaultMode();
		    break;
		case key_left:
            C.AdvanceTick(-1);
            C.ExitToDefaultMode();
		    break;
        case key_backspace:
            end_tick--;

        	if(end_tick <= start_tick)
        	{
                DeleteElement(this, false, false);
            }
            else
            {
                CalcTiming();
            }
            break;
        default:
        {
            Process_Key_Default(key, flags);
        }
    }
    Update();
}

Reverse::Reverse()
{
    type = El_Reverse;
    value = true;
}

Reverse* Reverse::Clone(bool add)
{
    Reverse* clone = CreateElement_Reverse();
    clone->value = value;

    clone->start_tick = start_tick;
    clone->end_tick = end_tick;
    clone->track_line = track_line;
    clone->trknum = trknum;
    clone->tick_length = tick_length;

    return clone;
}

void Reverse::ProcessKey(unsigned int key, unsigned int flags)
{
    switch(key)
    {
        case key_up:
            C.AdvanceLine(-1);
            C.ExitToDefaultMode();
            break;
		case key_return:
		case key_down:
            C.AdvanceLine(1);
            C.ExitToDefaultMode();
		    break;
		case key_right:
            C.AdvanceTick(1);
            C.ExitToDefaultMode();
		    break;
		case key_left:
            C.AdvanceTick(-1);
            C.ExitToDefaultMode();
		    break;
        case key_backspace:
            end_tick--;

        	if(end_tick <= start_tick)
        	{
                DeleteElement(this, false, false);
            }
            break;
        default:
        {
            Process_Key_Default(key, flags);
        }
    }
}

void Reverse::ProcessChar(char character)
{
/*
    if(*character == 0x3C) // <
    {
        value = true;
    }
    else if(*character == 0x3E) // >
    {
        value = false;
    }*/
}

Repeat::Repeat()
{
    type = El_Repeat;
    value = true;
}

Repeat* Repeat::Clone(bool add)
{
    Repeat* clone = CreateElement_Repeater();
    clone->value = value;

    clone->start_tick = start_tick;
    clone->end_tick = end_tick;
    clone->track_line = track_line;
    clone->trknum = trknum;
    clone->tick_length = tick_length;

    return clone;
}

void Repeat::ProcessKey(unsigned int key, unsigned int flags)
{
    switch(key)
    {
        case key_up:
            C.AdvanceLine(-1);
            C.ExitToDefaultMode();
            break;
		case key_return:
		case key_down:
            C.AdvanceLine(1);
            C.ExitToDefaultMode();
		    break;
		case key_right:
            C.AdvanceTick(1);
            C.ExitToDefaultMode();
		    break;
		case key_left:
            C.AdvanceTick(-1);
            C.ExitToDefaultMode();
		    break;
        case key_backspace:
            end_tick--;

        	if(end_tick <= start_tick)
        	{
                DeleteElement(this, false, false);
            }
            break;
        default:
        {
            Process_Key_Default(key, flags);
        }
    }
}

void Repeat::ProcessChar(char character)
{
    // TODO: switching between repeat and reverse
/*
    if(*character == 0x3C) // <
    {
        value = true;
    }
    else if(*character == 0x3E) // >
    {
        value = false;
    }*/
}

Pattern::Pattern(char* nm, int x1, int x2, int y1, int y2, bool tracks)
{
    start_tick = X2Tick(x1, Loc_MainGrid);
    end_tick = X2Tick(x2, Loc_MainGrid);
    track_line = Y2Line(y1, Loc_MainGrid);
    track_line_end = Y2Line(y2, Loc_MainGrid);

    Init(nm, tracks);
}

Pattern::Pattern(char* nm, float tk1, float tk2, int tr1, int tr2, bool tracks)
{
    start_tick = tk1;
    end_tick = tk2;
    track_line = tr1;
    track_line_end = tr2;

    Init(nm, tracks);
}

void Pattern::Init(char* nm, bool tracks)
{
    type = El_Pattern;
    ptype = Patt_Grid;

    num_lines = 0;
    offs_line = 0;
    last_pianooffs = 0;
    tick_width = -1;
    origindex = 0;

    OrigPt = NULL;
    first_elem = NULL;
    last_elem = NULL;
    symtrig = NULL;
    tgenv = NULL;
    pitch = NULL;
    bunch_first = NULL;
    bunch_last = NULL;
    first_trkdata = top_trk = NULL;
    last_trkdata = bottom_trk = NULL;
    der_first = der_last = der_prev = der_next = NULL;
    der_num = 0;
    orig_prev = orig_next = NULL;
    tg_internal_first = tg_internal_last = NULL;
    parent_instance = NULL;
    parent_trigger = NULL;
    first_ev = last_ev = NULL;
    instr_owner = NULL;
    nmimg = NULL;
    limg = NULL;
    smallimg = NULL;
    pbk = NULL;
    ibound = NULL;

    // params
    params = new ParamSet(100, 50, &scope);
    loc_vol = new Vol(100);
    loc_pan = new Pan(50);
    // paramedits
    name = new TString(nm, false, this);
    name->pt = this;
    AddParamedit(name);
    default_param = name;
    ed_vol = new Percent(100, 3, this, Param_Vol);
    AddParamedit(ed_vol);
    loc_vol->AddParamedit(ed_vol);
    ed_pan = new Percent(50, 3, this, Param_Pan);
    AddParamedit(ed_pan);
    loc_pan->AddParamedit(ed_pan);
    ed_note = new Note(baseNote, this);
    AddParamedit(ed_note);
    pe_mode = ParamEdit_Block;

    ranged = false;
    folded = true;
    autopatt = false;
    touchresized = false;
    muted = false;
    is_fat = true;
    aliasModeName = true;
    //foldtoggle = new Toggle(&folded, NULL);
    //AddNewControl(foldtoggle, NULL);
    //showkeys = false;
    //keystoggle = new Toggle(&showkeys, NULL);
    //AddNewControl(keystoggle, NULL);
    //playing = false;
    //playtoggle = new Toggle(&playing, NULL);
    //AddNewControl(playtoggle, NULL);
    //playtoggle->pattern = true;
    //playtoggle->patt = this;

    vol_lane_main = Lane(false, 3, Lane_Vol);
    pan_lane_main = Lane(false, 3, Lane_Pan);
    UpdateLaneBases();

    memset(&scope, 0, sizeof(Scope));
    scope.pt = this;
    scope.local = true;

    if(tracks)
    {
        num_lines = NUM_PATTERN_TRACKS;

        Trk* td;
        for(int tc = -1; tc <= NUM_PATTERN_TRACKS; tc++)
        {
            td = new Trk();
            td->Init(tc, false);
            td->scope.pt = this;
            AddTrkData(td);
        }
        top_trk = first_trkdata;
        bottom_trk = last_trkdata;
        first_trkdata = first_trkdata->next;
        last_trkdata = last_trkdata->prev;
    }
}

Pattern::~Pattern()
{
    delete params;
    delete loc_vol;
    delete loc_pan;
	delete name;
	delete ed_vol;
	delete ed_pan;
	delete ed_note;

    Trk* trk = top_trk;
	Trk* trknext;
	while(trk != NULL)
	{
		trknext = trk->next;
		delete trk;
		trk = trknext;
	}

	// if this playback is not external
    if(pbk != NULL && pbk != pbkAux && pbk != pbkMain)
        delete pbk;
}

void Pattern::CopyElementsTo(Pattern * dst, bool add)
{
    C.SaveState();
    C.SetPattern(dst, Loc_SmallGrid);
    Element* el = OrigPt->first_elem;
    while(el != NULL)
    {
        el->skip_cloning = false;
        if(el->type == El_SlideNote)
        {
            SlideNote* sn = (SlideNote*)el;
            if(sn->parent != NULL)
            {
                el->skip_cloning = true;
            }
        }
        el = el->patt_next;
    }

    el = OrigPt->first_elem;
    Element* nel;
    while(el != NULL)
    {
        if(el->IsPresent() == true && el->skip_cloning == false)
		{
			nel = el->Clone(add);
            if(el->IsInstance())
            {
                CloneSlideNotes((Instance*)el, (Instance*)nel, false);
            }
		}
        el = el->patt_next;
    }
    C.RestoreState();
}

Pattern* Pattern::Copy(bool add)
{
    char* pname = GetOriginalPatternName();
    Pattern* ptmain = new Pattern(pname, 0, 0, 0, 0, true);
    GetPatternNameImage(ptmain);
    Pattern* copy = new Pattern(pname, start_tick, end_tick, track_line, track_line_end, false);
    delete pname;

    AddOriginalPattern(ptmain);
    ptmain->AddDerivedPattern(copy);

    Trk* trk = OrigPt->first_trkdata;
    Trk* trk1 = ptmain->first_trkdata;
    while(trk != OrigPt->bottom_trk)
    {
        trk1->defined_instr = trk->defined_instr;
        trk1 = trk1->next;
        trk = trk->next;
    }

    copy->ed_vol->visible = ed_vol->visible;
    copy->ed_pan->visible = ed_pan->visible;
    copy->ed_note->visible = ed_note->visible;
    copy->folded = folded;
    copy->symtrig = NULL;

    copy->loc_vol->SetNormalValue(loc_vol->val);
    copy->loc_pan->SetNormalValue(loc_pan->val);

    copy->patt = field_pattern;
    copy->ptype = ptmain->ptype = ptype;
    copy->last_edited_param = copy->name;
    copy->ibound = ibound;
    ptmain->ibound = ibound;
    copy->offs_line = offs_line;
    copy->last_pianooffs = last_pianooffs;
    copy->is_fat = is_fat;

    copy->Update();

    CopyElementsTo(copy, add);

    if(add)
    {
       AddNewElement(copy, false);
    }

    ptmain->autopatt = OrigPt->autopatt;

    return copy;
}

Pattern* Pattern::Clone(bool add, float new_tick, int new_trackline)
{
    Pattern* clone = new Pattern(name->string, 
                                 new_tick, 
                                 new_tick + tick_length, 
                                 new_trackline, 
                                 new_trackline + num_lines, 
                                 false);

    //clone->name = OrigPt->name;
    clone->ed_vol->visible = ed_vol->visible;
    clone->ed_pan->visible = ed_pan->visible;
    clone->ed_note->visible = ed_note->visible;
    clone->folded = folded;

    clone->loc_vol->SetNormalValue(loc_vol->val);
    clone->loc_pan->SetNormalValue(loc_pan->val);

    if(ptype == Patt_StepSeq)
    {
        Trk* t = first_trkdata;
        Trk* t1 = clone->first_trkdata;
        while(t != bottom_trk)
        {
            t1->defined_instr = t->defined_instr;

            t1 = t1->next;
            t = t->next;
        }
    }

    clone->first_elem = first_elem;
    clone->last_elem = last_elem;
    clone->ptype = ptype;
    clone->OrigPt = OrigPt;
    clone->last_edited_param = clone->name;
    clone->ibound = ibound;
    clone->offs_line = offs_line;
    clone->last_pianooffs = last_pianooffs;
    clone->muted = muted;
    clone->is_fat = is_fat;
    clone->touchresized = touchresized;

    clone->Update();

    OrigPt->AddDerivedPattern(clone);
    AddNewElement(clone, false);
    return clone;
}

Pattern* Pattern::Clone(bool add)
{
    Pattern* clone = new Pattern(name->string, start_tick, end_tick, track_line, track_line_end, false);

    //clone->name = OrigPt->name;
    clone->ed_vol->visible = ed_vol->visible;
    clone->ed_pan->visible = ed_pan->visible;
    clone->ed_note->visible = ed_note->visible;
    clone->loc_vol->SetNormalValue(loc_vol->val);
    clone->loc_pan->SetNormalValue(loc_pan->val);
    clone->folded = folded;

    if(ptype == Patt_StepSeq)
    {
        Trk* t = first_trkdata;
        Trk* t1 = clone->first_trkdata;
        while(t != bottom_trk)
        {
            t1->defined_instr = t->defined_instr;

            t1 = t1->next;
            t = t->next;
        }
    }

    clone->first_elem = first_elem;
    clone->last_elem = last_elem;
    clone->ptype = ptype;
    clone->OrigPt = OrigPt;
    clone->last_edited_param = clone->name;
    clone->ibound = ibound;
    clone->offs_line = offs_line;
    clone->last_pianooffs = last_pianooffs;
    clone->muted = muted;
    clone->is_fat = is_fat;
    clone->touchresized = touchresized;

    clone->Update();

    OrigPt->AddDerivedPattern(clone);
    AddNewElement(clone, false);
    return clone;
}

void Pattern::AddEnvelope(ParamType ptype)
{
    if(ptype == Param_Pitch)
    {
        if(pitch == NULL)
        {
            C.SaveState();
            C.SetPattern(this, Loc_SmallGrid);
            C.SetPos(0, 0);
            Command* c = new Command(&scope, Cmd_PitchEnv);
            C.RestoreState();
            AddNewElement(c, false);
            c->displayable = false;
            pitch = c;
        }
    }
}

void Pattern::ProcessChar(char character)
{
    switch(character)
    {
        case 0x5e: // ^
            SwitchVol();
            break;
        case 0x2a: // *
            SwitchPan();
            break;
    }
}

void Pattern::SwitchVol()
{
    if(ed_vol->visible == false)
    {
        ed_vol->visible = true;
        ed_vol->bypass = false;
    }
    else
    {
        ed_vol->bypass = true;
        ed_vol->visible = false;
        if(active_param == ed_vol)
        {
            Switch2Param(name);
        }
    }
}

void Pattern::SwitchPan()
{
    if(ed_pan->visible == false)
    {
        ed_pan->visible = true;
		ed_pan->bypass = false;
    }
    else
    {
        ed_pan->bypass = true;
        ed_pan->visible = false;
        if(active_param == ed_pan)
        {
            Switch2Param(name);
        }
    }
}

/*
void Pattern::SwitchVol()
{
    if(vol_visible == false)
    {
        vol_visible = true;
        if(ed_vol == NULL)
        {
            ed_vol = new Percent(loc_vol->n_val*100, 3, this);
            AddParamedit(ed_vol);
            loc_vol->AddParamedit(ed_vol);
        }
		else
		{
			ed_vol->bypass = false;
		}
    }
    else
    {
        ed_vol->bypass = true;
        vol_visible = false;
        if(active_param == ed_vol)
        {
            Switch2Param(name);
        }
    }
}

void Pattern::SwitchPan()
{
    if(pan_visible == false)
    {
        pan_visible = true;
        if(ed_pan == NULL)
        {
            ed_pan = new Percent(loc_pan->n_val.n*100, 3, this);
            AddParamedit(ed_pan);
            loc_pan->AddParamedit(ed_pan);
        }
		else
		{
			ed_pan->bypass = false;
		}
    }
    else
    {
        ed_pan->bypass = true;
        pan_visible = false;
        if(active_param == ed_pan)
        {
            Switch2Param(name);
        }
    }
}
*/

void Pattern::AddElement(Element* el)
{
    if((first_elem == NULL)&&(last_elem == NULL))
    {
        el->patt_prev = NULL;
        el->patt_next = NULL;
        first_elem = el;
        last_elem = first_elem;
    }
    else
    {
        last_elem->patt_next = el;
        el->patt_prev = last_elem;
        el->patt_next = NULL;
        last_elem = el;
    }
    el->patt = this;
    el->Update();
}

void Pattern::RemoveElement(Element* el)
{
	if((el == first_elem)&&(el == last_elem))
	{
		first_elem = NULL;
		last_elem = NULL;
	}
	else if(el == first_elem)
	{
		first_elem = el->patt_next;
		first_elem->patt_prev = NULL;
	}
	else if(el == last_elem)
	{
		last_elem = el->patt_prev;
		last_elem->patt_next = NULL;
	}
	else
	{
		el->patt_prev->patt_next = el->patt_next;
		el->patt_next->patt_prev = el->patt_prev;
	}
}

void Pattern::AddSelectedElements()
{
    Element* el = firstElem;

    while(el != NULL)
    {
        if((el->selected && el->selloc == Loc_MainGrid)&&(el->type != El_SlideNote)&&(el->type != El_Pattern))
        {
            el->Deselect();
            AddElement(el);
        }
        el = el->next;
    }

    // Adjust timing data for added elements
	if((first_elem != NULL)&&(last_elem != NULL))
	{
		first_elem->patt_prev = NULL;
		last_elem->patt_next = NULL;

        el = first_elem;
        while(el != NULL)
        {
            el->start_tick -= this->start_tick;
            el->end_tick -= this->start_tick;

            el->track_line -= this->track_line;
            el->Update();
            el = el->patt_next;
        }
	}
}

void Pattern::Move(float dtick, int dtrack)
{
    start_tick += dtick;
    end_tick = start_tick + tick_length;

    track_line += dtrack;
    track_line_end += dtrack;

    Update();

    if(this == aux_panel->workPt)
    {
        AuxPos2MainPos();

        pbkAux->AlignRangeToPattern();
        pbkAux->SetCurrFrame(pbkAux->currFrame += Tick2Frame(dtick));
        if(aux_panel->playing)
        {
            MainPos2AuxPos();
        }
	}
}

void Pattern::UpdateInternalTriggers()
{
    Trigger* tg = tg_internal_first;
    while(tg != NULL)
    {
		if(tg->el->IsPresent())
		{
			RemoveTriggerFromEvent(tg);
			Locate_Trigger(tg);
		}
        tg = tg->pt_next;
    }
}

void Pattern::Reset()
{
	active = false;
    params->Reset();
    symtrig = NULL;
    tgenv = NULL;
}

int Pattern::Track()
{
    return trknum;
}

/*
int Pattern::EndTrack()
{
    return end_track;
}*/

float Pattern::StartTick()
{
    return start_tick;
}

float Pattern::EndTick()
{
    return end_tick;
}

int Pattern::StartTickX()
{
    return Tick2X(this->StartTick(), Loc_MainGrid);
}

int Pattern::EndTickX()
{
    return Tick2X(this->EndTick(), Loc_MainGrid);
}

int Pattern::st_StartTickX()
{
    return Tick2X(this->StartTick(), Loc_StaticGrid);
}

int Pattern::st_EndTickX()
{
    return Tick2X(this->EndTick(), Loc_StaticGrid);
}

int Pattern::TrackLine2Y()
{
    int line = trkdata->start_line;
    return Line2Y(line, Loc_MainGrid);
}

int Pattern::EndLine2Y()
{
    return Line2Y(track_line_end, Loc_MainGrid);
}

long Pattern::StartFrame()
{
    return frame;
}

long Pattern::EndFrame()
{
    return end_frame;
}

void Pattern::CheckVisibility()
{
    auxvisible = false;
	if(trkdata != NULL && displayable == true)
	{
        int x1 = StartTickX();
        int x2 = EndTickX();
        int y1 = TrackLine2Y() - lineHeight*2;
        int y2 = EndLine2Y() - lineHeight;
        if(CheckPlaneCrossing(x1, y1, x2, y2, GridX1, GridY1, GridX2, GridY2))
        {
            visible = true;
        }
        else
        {
            visible = false;
        }
    }
}

void Pattern::ParameditUpdate(PEdit* pe)
{
    if(pe == name && OrigPt != NULL)
    {
        if(aliasModeName) // if renaming
        {
            OrigPt->name->SetString(name->string);
            GetPatternNameImage(OrigPt);
            Pattern* pt = OrigPt->der_first;
            while(pt != NULL)
            {
                if(pt != this)
                {
                    pt->name->SetString(name->string);
                }
                pt = pt->der_next;
            }
            OrigPt->UpdateScaledImage();
        }
        else // if aliasing
        {
            DeleteElement(this, false, false);

            Txt* t = new Txt;
            AddNewElement(t, false);

            C.mode = CMode_ElemEdit;
            C.curElem = t;

            t->AddString(name->string);
            t->ParseString();
        }

        R(Refresh_Aux);
        R(Refresh_GridContent);
    }
}

void Pattern::UpdateLaneBases()
{
    vol_lane_main.base_line = track_line_end;
    pan_lane_main.base_line = track_line_end;
    if(vol_lane_main.visible == true)
    {
        pan_lane_main.base_line = vol_lane_main.base_line + vol_lane_main.height;
    }
}

void Pattern::DeleteAllElements(bool flush, bool preventundo)
{
	Element* el = first_elem;
    Element* elemnext;
    while(el != NULL)
    {
        elemnext = el->patt_next;
        while(elemnext != NULL && CheckDeletionSkip(elemnext, el) == true)
        {
            elemnext = elemnext->patt_next;
        }
        DeleteElement(el, flush, preventundo);
        el = elemnext;
    }
}

void Pattern::AddTrkData(Trk* td)
{
    if(first_trkdata == NULL && last_trkdata == NULL)
    {
        td->prev = NULL;
        td->next = NULL;
        first_trkdata = td;
    }
    else
    {
        last_trkdata->next = td;
        td->prev = last_trkdata;
        td->next = NULL;
    }
    last_trkdata = td;
}

Element* Pattern::IsElemExists(float tick, int trknum)
{
    Element* el = first_elem;
    while(el != NULL)
    {
        if(el->IsPresent() && el->start_tick == tick && el->track_line == trknum)
        {
            return el;
        }
        el = el->patt_next;
    }
    return NULL;
}

void Pattern::Update(bool tonly)
{
    if(tonly)
    {
        CalcTiming();

        // If this is the main pattern
        if(OrigPt == NULL)
        {
            Element* el = first_elem;
            while(el != NULL)
            {
                el->Update();
                el = el->patt_next;
            }
        }
    }
    else
    {
        UpdateTrackDataFromLine();
        CalcTiming();
        Globalize();

        track_params = field_trkdata->params;

        UpdateNote();
        CheckVisibility();

        // If this is the main pattern
        if(OrigPt == NULL)
        {
            Element* el = first_elem;
            while(el != NULL)
            {
                el->Update();
                el = el->patt_next;
            }
        }

        Relocate_Element(this);
        UpdateInternalTriggers();
    }
}

void Pattern::AddDerivedPattern(Pattern * pt, bool skipadd)
{
    if(skipadd)
    {
        // skip adding
    }
    else
    {
        if((der_first == NULL)&&(der_last == NULL))
        {
            pt->der_prev = NULL;
            pt->der_next = NULL;
            der_first = pt;
        }
        else
        {
            der_last->der_next = pt;
            pt->der_prev = der_last;
            pt->der_next = NULL;
        }
        der_last = pt;
        der_num++;
    }

    pt->OrigPt = this;
    pt->ibound = ibound;

    Element* el = first_elem;
    while(el != NULL)
    {
        CreateElementTriggersPerPattern(pt, el, skipadd);
        el = el->patt_next;
    }

    Instance* ii;
    el = first_elem;
    while(el != NULL)
    {
        if(el->IsInstance())
        {
            ii = (Instance*)el;
            ii->SpreadSlideNotes();
        }
        el = el->patt_next;
    }
}

void Pattern::RemoveDerivedPattern(Pattern* pt)
{
    if(pt->OrigPt == this)
    {
        if((pt == der_first)&&(pt == der_last))
        {
            der_first = NULL;
            der_last = NULL;
        }
        else if(pt == der_first)
        {
            der_first = pt->der_next;
            der_first->der_prev = NULL;
        }
        else if(pt == der_last)
        {
            der_last = pt->der_prev;
            der_last->der_next = NULL;
        }
        else
        {
            pt->der_prev->der_next = pt->der_next;
            pt->der_next->der_prev = pt->der_prev;
        }
        der_num--;
    }
}

void Pattern::AddInternalTrigger(Trigger* tg)
{
    if((tg_internal_first == NULL)&&(tg_internal_last == NULL))
    {
        tg->pt_prev = NULL;
        tg->pt_next = NULL;
        tg_internal_first = tg;
    }
    else
    {
        tg_internal_last->pt_next = tg;
        tg->pt_prev = tg_internal_last;
        tg->pt_next = NULL;
    }
    tg_internal_last = tg;
}

void Pattern::RemoveInternalTrigger(Trigger* tg)
{
	if((tg == tg_internal_first)&&(tg == tg_internal_last))
	{
		tg_internal_first = NULL;
		tg_internal_last = NULL;
	}
	else if(tg == tg_internal_first)
	{
		tg_internal_first = tg->pt_next;
		tg_internal_first->pt_prev = NULL;
	}
	else if(tg == tg_internal_last)
	{
		tg_internal_last = tg->pt_prev;
		tg_internal_last->pt_next = NULL;
	}
	else
	{
		tg->pt_prev->pt_next = tg->pt_next;
		tg->pt_next->pt_prev = tg->pt_prev;
	}
}

bool Pattern::IsLastChild()
{
    if(OrigPt != NULL && OrigPt->der_first == this && OrigPt->der_last == this)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Pattern::isTheOnlyPresentChild()
{
    if(OrigPt != NULL)
    {
        Pattern* pt = OrigPt->der_first;
        while(pt != NULL)
        {
            if(pt != this && pt->IsPresent())
            {
                return false;
            }
            pt = pt->der_next;
        }
    }

    return true;
}

void Pattern::UpdateTrackDataFromLine()
{
    if(parent_trigger != NULL)
    {
        jassert(parent_trigger->trkdata != NULL);
        trkdata = parent_trigger->trkdata;
        trknum = parent_trigger->trkdata->trknum;
        track_line = parent_trigger->el->track_line;
    }
    else
    {
        if(track_line < 0)
        {
            track_line = 0;
        }
        trknum = GetTrkNumForLine(track_line, patt);
        trkdata = GetTrkDataForTrkNum(trknum, patt);
    }
	track_line_end = track_line;
}

void Pattern::AddPlayback()
{
    if(pbk != NULL)
        delete pbk;

    pbk = new Playback(this);
}

bool Pattern::IsPlaying()
{
    if(Element::IsPlaying() == true)
    {
        return true;
    }

    Trigger* tg = tg_internal_first;
    while(tg != NULL)
    {
        if(tg->tgworking == true || 
           tg->globallisted == true || 
           (tg->apatt_instance != NULL && tg->apatt_instance->IsPlaying() == true))
        {
            return true;
        }
        tg = tg->pt_next;
    }

    if(IsPlaybackActive() == true)
    {
        return true;
    }

    return false;
}

bool Pattern::IsPlaybackActive()
{
    if(pbk != NULL && (pbk->dworking == true || pbk->pactive == true))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Pattern::DisablePlayback()
{
    if(pbk != NULL)
    {
        pbk->dworking = false;
    }
}

void Pattern::EnqueueEnvelopeTrigger(Trigger* tg)
{
    if(tgenv != NULL)
    {
        tgenv->group_next = tg;
    }
    tg->group_prev = tgenv;
    tg->group_next = NULL;
    tgenv = tg;
}

void Pattern::DequeueEnvelopeTrigger(Trigger* tg)
{
    if(tgenv == tg)
    {
        tgenv = tg->group_prev;
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

Element* Pattern::NextElementToDelete(Element* eltodel)
{
    Element* elnext = eltodel->patt_next;
    while(elnext != NULL && CheckDeletionSkip(elnext, eltodel) == true)
    {
        elnext = elnext->patt_next;
    }
    return elnext;
}

bool Pattern::IsPresent()
{
    if(Element::IsPresent())
    {
        if(parent_trigger == NULL || parent_trigger->el->IsPresent())
            return true;
        else
            return false;
    }
    else
        return false;
}

bool Pattern::IsAnyDerivedPresent()
{
    Pattern* pt = der_first;
    while(pt != NULL)
    {
        if(pt->IsPresent())
        {
            return true;
        }
        pt = pt->der_next;
    }
    return false;
}

void Pattern::Save(XmlElement * xmlNode)
{
    Element::Save(xmlNode);
    xmlNode->setAttribute(T("Volume"), loc_vol->val);
    xmlNode->setAttribute(T("Panning"), loc_pan->val);
    xmlNode->setAttribute(T("OffsLine"), offs_line);
    xmlNode->setAttribute(T("LastPianoOffs"), last_pianooffs);
    xmlNode->setAttribute(T("Muted"), muted ? 1 : 0);
    xmlNode->setAttribute(T("TouchResized"), touchresized ? 1 : 0);
    xmlNode->setAttribute(T("IsFat"), is_fat);
}

void Pattern::Load(XmlElement * xmlNode)
{
    Element::Load(xmlNode);
    loc_vol->SetNormalValue((float)xmlNode->getDoubleAttribute(T("Volume")));
    loc_pan->SetNormalValue((float)xmlNode->getDoubleAttribute(T("Panning")));
    offs_line = xmlNode->getIntAttribute(T("OffsLine"));
    last_pianooffs = xmlNode->getIntAttribute(T("LastPianoOffs"));
    muted = xmlNode->getBoolAttribute(T("Muted"));
    touchresized = xmlNode->getBoolAttribute(T("TouchResized"));
    is_fat = xmlNode->getBoolAttribute(T("IsFat"), true);
}

void Pattern::UpdateScaledImage()
{
    float maxlength = 0;
    Pattern* patt = der_first;
    while(patt != NULL)
    {
        if(patt->tick_length > maxlength)
            maxlength = patt->tick_length;
        patt = patt->der_next;
    }

    int nmimglen = nmimg != NULL ? nmimg->getWidth() : 0;

    if(maxlength > 0)
    {
        float ltick = 0;
        int hgt = 22;
        if(is_fat == false)
        {
            hgt = 10;
        }
        int ahgt = 0;
        int nmin, nmax;
        if(ptype == Patt_Pianoroll)
        {
            nmin = NUM_PIANOROLL_LINES - 1;
            nmax = 0;
        }
        else
        {
            nmin = 0;
            nmax = 0;
            ltick = tick_length;
        }

        int val;
        Instance* ii;
        Element* elmin = NULL;
        Element* elmax = NULL;
        Element* el = first_elem;
        while(el != NULL)
        {
            if(el->IsPresent() && IsElementTypeDisplayable(el->type) && el->type != El_TextString)
            {
                if(ptype == Patt_Pianoroll || ptype == Patt_StepSeq)
                {
                    if(el->IsInstance())
                    {
                        ii = (Instance*)el;
                        if(ptype == Patt_Pianoroll)
                        {
                            val = ii->ed_note->value;
                        }
                        else if(ptype == Patt_StepSeq)
                        {
                            val = ii->instr->index;
                        }
                    }
                    else if(el->type == El_SlideNote)
                    {
                        SlideNote* sl = (SlideNote*)el;
                        val = sl->ed_note->value;
                    }
                }
                else
                {
                    val = el->track_line;
                }

                if(ptype == Patt_Pianoroll)
                {
                    if(elmin == NULL || val < nmin)
                    {
						elmin = el;
                        nmin = val;
                    }

                    if(elmax == NULL || val > nmax)
                    {
						elmax = el;
                        nmax = val;
                    }

                    if(el->end_tick > ltick)
                    {
                        ltick = el->end_tick;
                    }
                }
                else
                {
                    if(val > nmax)
                    {
                        nmax = val;
                    }

                    if(ptype == Patt_StepSeq)
                    {
    				    if(el->start_tick >= ltick)
                        {
                            ltick = el->start_tick + tickWidth*2;
                        }
                    }
                    else if(ptype == Patt_Grid)
                    {
                        if(el->end_tick > ltick)
                        {
                            ltick = el->end_tick;
                        }
                    }
				}
            }
            el = el->patt_next;
        }

        if(ptype == Patt_Pianoroll)
        {
            ahgt = nmax - nmin + 1;
        }
        else
        {
            ahgt = nmax  + 1;
        }

        if(limg != NULL)
        {
            delete limg;
            limg = NULL;
        }

        int ptpixheight = lineHeight*2 - 2;
        int ptpixlen = int(tickWidth*maxlength);
        int imgpixlen = ptpixlen;
        if(ptpixlen < nmimglen)
            imgpixlen = nmimglen;

        limg = new Image(Image::ARGB, imgpixlen, hgt + 7, true);
        Graphics imageContext(*(limg));
        int xi = 0;
        int yi = 0;
        if(ptype == Patt_StepSeq)
        {
            int gappy = 0;
            if(is_fat)
            {
                imageContext.setColour(Colour(0x5a2D415A));
                J_FillRect(imageContext, xi, yi, xi + ptpixlen - 1, yi + ptpixheight);

                imageContext.setColour(Colour(0xff4A608A));
                //J_LineRect(g, x, y - tH + 1, x + pixlen - 1, y - tH + ptheight + 1);
                J_VLine(imageContext, xi, yi, yi + ptpixheight);
                J_HLine(imageContext, yi, xi, xi + ptpixlen - gappy);
                J_HLine(imageContext, yi + ptpixheight, xi, xi + ptpixlen - gappy);

                imageContext.setColour(Colour(0x6f446888));
                J_HLine(imageContext, yi + 1, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x5f446888));
                J_HLine(imageContext, yi + 2, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x4f446888));
                J_HLine(imageContext, yi + 3, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x4a446888));
                J_HLine(imageContext, yi + 4, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x3f446888));
                J_HLine(imageContext, yi + 5, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x3a446888));
                J_HLine(imageContext, yi + 6, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x35446888));
                J_HLine(imageContext, yi + 7, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x30446888));
                J_HLine(imageContext, yi + 8, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x2a446888));
                J_HLine(imageContext, yi + 9, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x25446888));
                J_HLine(imageContext, yi + 10, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x20446888));
                J_HLine(imageContext, yi + 11, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x1a446888));
                J_HLine(imageContext, yi + 12, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x15446888));
                J_HLine(imageContext, yi + 13, xi + 1, xi + ptpixlen - gappy);
            }
            else
            {
                imageContext.setColour(Colour(0xdf2D415A));
                J_FillRect(imageContext, xi, yi, xi + ptpixlen - 1, yi + ptpixheight);

                imageContext.setColour(Colour(0xff4A608A));
                //J_LineRect(g, x, y - tH + 1, x + pixlen - 1, y - tH + ptheight + 1);
                J_VLine(imageContext, xi, yi, yi + ptpixheight);
                J_HLine(imageContext, yi, xi, xi + ptpixlen - gappy);
                J_HLine(imageContext, yi + ptpixheight, xi, xi + ptpixlen - gappy);

                imageContext.setColour(Colour(0x6f446888));
                J_HLine(imageContext, yi + 1, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x5f446888));
                J_HLine(imageContext, yi + 2, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x4f446888));
                J_HLine(imageContext, yi + 3, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x3f446888));
                J_HLine(imageContext, yi + 4, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x2f446888));
                J_HLine(imageContext, yi + 5, xi + 1, xi + ptpixlen - gappy);
            }
        }
        else if(ptype == Patt_Pianoroll)
        {
            int gappy = 0;
            if(is_fat)
            {
                imageContext.setColour(Colour(0x5a4A4A4A));
                J_FillRect(imageContext, xi, yi, xi + ptpixlen - 1, yi + ptpixheight);

                imageContext.setColour(Colour(0xff686868));
                //J_LineRect(g, x, y - tH + 1, x + pixlen - 1, y - tH + ptheight + 1);
                J_VLine(imageContext, xi, yi, yi + ptpixheight);
                J_HLine(imageContext, yi, xi, xi + ptpixlen - gappy);
                J_HLine(imageContext, yi + ptpixheight, xi, xi + ptpixlen - gappy);

                imageContext.setColour(Colour(0x5f787878));
                J_HLine(imageContext, yi + 1, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x4f787878));
                J_HLine(imageContext, yi + 2, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x3f787878));
                J_HLine(imageContext, yi + 3, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x3a787878));
                J_HLine(imageContext, yi + 4, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x35787878));
                J_HLine(imageContext, yi + 5, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x2f787878));
                J_HLine(imageContext, yi + 6, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x2a787878));
                J_HLine(imageContext, yi + 7, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x25787878));
                J_HLine(imageContext, yi + 8, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x20787878));
                J_HLine(imageContext, yi + 9, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x1f787878));
                J_HLine(imageContext, yi + 10, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x1a787878));
                J_HLine(imageContext, yi + 11, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x15787878));
                J_HLine(imageContext, yi + 12, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x0a787878));
                J_HLine(imageContext, yi + 13, xi + 1, xi + ptpixlen - gappy);
            }
            else
            {
                imageContext.setColour(Colour(0xdf4A4A4A));
                J_FillRect(imageContext, xi, yi, xi + ptpixlen - 1, yi + ptpixheight);

                imageContext.setColour(Colour(0xdf686868));
                //J_LineRect(g, x, y - tH + 1, x + pixlen - 1, y - tH + ptheight + 1);
                J_VLine(imageContext, xi, yi, yi + ptpixheight);
                J_HLine(imageContext, yi, xi, xi + ptpixlen - gappy);
                J_HLine(imageContext, yi + ptpixheight, xi, xi + ptpixlen - gappy);

                imageContext.setColour(Colour(0x6f787878));
                J_HLine(imageContext, yi + 1, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x5f787878));
                J_HLine(imageContext, yi + 2, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x4f787878));
                J_HLine(imageContext, yi + 3, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x3f787878));
                J_HLine(imageContext, yi + 4, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x2f787878));
                J_HLine(imageContext, yi + 5, xi + 1, xi + ptpixlen - gappy);
            }
        }
        else if(ptype == Patt_Grid)
        {
            int gappy = 0;
            if(is_fat)
            {
                imageContext.setColour(Colour(0x5a3A5A3A));
                J_FillRect(imageContext, xi, yi, xi + ptpixlen - 1, yi + ptpixheight);

                imageContext.setColour(Colour(0xff5A855A));
                //J_LineRect(g, x, y - tH + 1, x + pixlen - 1, y - tH + ptheight + 1);
                J_VLine(imageContext, xi, yi, yi + ptpixheight);
                J_HLine(imageContext, yi, xi, xi + ptpixlen - gappy);
                J_HLine(imageContext, yi + ptpixheight, xi, xi + ptpixlen - gappy);

                imageContext.setColour(Colour(0x5f5A855A));
                J_HLine(imageContext, yi + 1, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x4f5A855A));
                J_HLine(imageContext, yi + 2, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x3f5A855A));
                J_HLine(imageContext, yi + 3, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x3a5A855A));
                J_HLine(imageContext, yi + 4, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x355A855A));
                J_HLine(imageContext, yi + 5, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x2f5A855A));
                J_HLine(imageContext, yi + 6, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x2a5A855A));
                J_HLine(imageContext, yi + 7, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x255A855A));
                J_HLine(imageContext, yi + 8, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x205A855A));
                J_HLine(imageContext, yi + 9, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x1f5A855A));
                J_HLine(imageContext, yi + 10, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x1a5A855A));
                J_HLine(imageContext, yi + 11, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x155A855A));
                J_HLine(imageContext, yi + 12, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x0a5A855A));
                J_HLine(imageContext, yi + 13, xi + 1, xi + ptpixlen - gappy);
            }
            else
            {
                imageContext.setColour(Colour(0xdf3A5A3A));
                J_FillRect(imageContext, xi, yi, xi + ptpixlen - 1, yi + ptpixheight);

                imageContext.setColour(Colour(0xff5A855A));
                //J_LineRect(g, x, y - tH + 1, x + pixlen - 1, y - tH + ptheight + 1);
                J_VLine(imageContext, xi, yi, yi + ptpixheight);
                J_HLine(imageContext, yi, xi, xi + ptpixlen - gappy);
                J_HLine(imageContext, yi + ptpixheight, xi, xi + ptpixlen - gappy);

                imageContext.setColour(Colour(0x6f5A855A));
                J_HLine(imageContext, yi + 1, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x5f5A855A));
                J_HLine(imageContext, yi + 2, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x4f5A855A));
                J_HLine(imageContext, yi + 3, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x3f5A855A));
                J_HLine(imageContext, yi + 4, xi + 1, xi + ptpixlen - gappy);
                imageContext.setColour(Colour(0x2f5A855A));
                J_HLine(imageContext, yi + 5, xi + 1, xi + ptpixlen - gappy);
            }
        }

        if(ltick <= 0 || ahgt <= 0)
        {
            // skip then
        }
        else
        {
            int ascent = 0;
            if(ptype == Patt_Pianoroll)
            {
                ascent = (hgt - ahgt)/2;
            }
            int offset = nmin;
            float cf = (float)hgt/(ahgt);
			float cf2 = (float)hgt/(ahgt*2);

            //Colour col1 = Colour(0xdf5FBF5F); // for samplent
            Colour col1 = Colour(0xdf8F959A); // for samplent
            Colour col2 = Colour(0xff6F6FEF); // for gennote
            Colour col3 = Colour(0xff3FBFBF); // for steppy
            Colour col4 = Colour(0xdfFFFFFF); // common
            Colour colsl = Colour(0xafFFFF52); // for slide
            Colour col;
            int ycoord;
            float x1, x2;
            el = first_elem;
            while(el != NULL)
            {
                if(el->IsPresent() && IsElementTypeDisplayable(el->type) && el->type != El_TextString)
                {
                    if(ptype == Patt_Pianoroll || ptype == Patt_StepSeq)
                    {
                        if(el->IsInstance())
                        {
                            ii = (Instance*)el;
                            if(ptype == Patt_Pianoroll)
                            {
                                val = ii->ed_note->value;
                            }
                            else if(ptype == Patt_StepSeq)
                            {
                                val = ii->instr->index;
                            }
                        }
                        else if(el->type == El_SlideNote)
                        {
                            SlideNote* sl = (SlideNote*)el;
                            val = sl->ed_note->value;
                        }
                    }
                    else
                    {
                        val = el->track_line;
                    }

                    if(ptype == Patt_Pianoroll)
                    {
                        if(el->type == El_Samplent)
                        {
                            col = col1;
                        }
                        else if(el->type == El_GenNote)
                        {
                            col = col2;
                        }
                        else if(el->type == El_SlideNote)
                        {
                            col = colsl;
                        }
                        
                        if(ahgt <= hgt)
                        {
                            ycoord = hgt - (val - offset + ascent);
                        }
                        else
                            ycoord = int(hgt - (val - offset)*cf);
                    }
                    else if(ptype == Patt_StepSeq)
                    {
                        col = col3;
                        if(ahgt*2 <= hgt)
                        {
                            ycoord = int(val*2);
                        }
                        else
                            ycoord = int(val*cf2*2);
                    }
                    else if(ptype == Patt_Grid)
                    {
                        if(el->type == El_Samplent)
                        {
                            col = col1;
                        }
                        else if(el->type == El_GenNote)
                        {
                            col = col2;
                        }
                        else if(el->type == El_SlideNote)
                        {
                            col = colsl;
                        }
                        else
                            col = col4;

                        if(ahgt <= hgt)
                        {
                            ycoord = int(val);
                        }
                        else
                            ycoord = int(val*cf);
                    }

                    ycoord += 2;

                    x1 = el->start_tick*tickWidth;
                    if(el->end_tick > 0)
                    {
                        x2 = el->end_tick*tickWidth;
                        if(x2 - x1 < 1.f)
                        {
                            x2 = x1 + 1.f;
                        }
                    }
                    else
                        x2 = x1 + 1;

                    if(ptype == Patt_Pianoroll || ptype == Patt_Grid)
                    {
                        imageContext.setColour(col);
                        imageContext.drawHorizontalLine(ycoord, x1, x2);
                        imageContext.setColour(col.withAlpha(0.2f));
                        imageContext.drawHorizontalLine(ycoord - 1, x1, x2);
                        imageContext.drawHorizontalLine(ycoord + 1, x1, x2);
                    }
                    else
                    {
                        imageContext.setColour(col.withAlpha(0.7f));
                        imageContext.drawVerticalLine(int(x1), (float)ycoord, (float)ycoord + 2);
                        imageContext.setColour(col.withAlpha(0.5f));
                        imageContext.drawVerticalLine(int(x1) + 1, (float)ycoord, (float)ycoord + 2);
                        //imageContext.drawVerticalLine(int(x1) + 2, (float)ycoord, (float)ycoord + 2);
                        imageContext.setColour(col.withAlpha(0.7f));
                        imageContext.drawHorizontalLine(ycoord, x1, x1 + 2);
                        //imageContext.setColour(col);
                        //imageContext.drawVerticalLine(int(x1), (float)ycoord, (float)ycoord + 2);
                        //imageContext.drawHorizontalLine(ycoord, x1, x1 + 3);
                    }
                }
                el = el->patt_next;
            }
        }

        if(nmimg != NULL)
        {
            imageContext.setColour(Colours::white);
            imageContext.drawImageAt(nmimg, 1, ptpixheight - 12);
        }

        if(smallimg != NULL)
        {
            delete smallimg;
            smallimg = NULL;
        }

        hgt = 8;

        ptpixheight = lineHeight - 2;

        smallimg = new Image(Image::ARGB, imgpixlen, hgt + 7, true);
        Graphics imageContext1(*(smallimg));
        xi = 0;
        yi = 0;
        if(ptype == Patt_StepSeq)
        {
            int gappy = 0;
            {
                imageContext1.setColour(Colour(0x9f2D415A));
                J_FillRect(imageContext1, xi, yi, xi + ptpixlen - 1, yi + ptpixheight);

                imageContext1.setColour(Colour(0xff4A608A));
                //J_LineRect(g, x, y - tH + 1, x + pixlen - 1, y - tH + ptheight + 1);
                J_VLine(imageContext1, xi, yi, yi + ptpixheight);
                J_HLine(imageContext1, yi, xi, xi + ptpixlen - gappy);
                J_HLine(imageContext1, yi + ptpixheight, xi, xi + ptpixlen - gappy);

                imageContext1.setColour(Colour(0x6f446888));
                J_HLine(imageContext1, yi + 1, xi + 1, xi + ptpixlen - gappy);
                imageContext1.setColour(Colour(0x5f446888));
                J_HLine(imageContext1, yi + 2, xi + 1, xi + ptpixlen - gappy);
                imageContext1.setColour(Colour(0x4f446888));
                J_HLine(imageContext1, yi + 3, xi + 1, xi + ptpixlen - gappy);
                imageContext1.setColour(Colour(0x3f446888));
                J_HLine(imageContext1, yi + 4, xi + 1, xi + ptpixlen - gappy);
                imageContext1.setColour(Colour(0x2f446888));
                J_HLine(imageContext1, yi + 5, xi + 1, xi + ptpixlen - gappy);
            }
        }
        else if(ptype == Patt_Pianoroll)
        {
            int gappy = 0;
            {
                imageContext1.setColour(Colour(0x9f4A4A4A));
                J_FillRect(imageContext1, xi, yi, xi + ptpixlen - 1, yi + ptpixheight);

                imageContext1.setColour(Colour(0xff686868));
                //J_LineRect(g, x, y - tH + 1, x + pixlen - 1, y - tH + ptheight + 1);
                J_VLine(imageContext1, xi, yi, yi + ptpixheight);
                J_HLine(imageContext1, yi, xi, xi + ptpixlen - gappy);
                J_HLine(imageContext1, yi + ptpixheight, xi, xi + ptpixlen - gappy);

                imageContext1.setColour(Colour(0x6f787878));
                J_HLine(imageContext1, yi + 1, xi + 1, xi + ptpixlen - gappy);
                imageContext1.setColour(Colour(0x5f787878));
                J_HLine(imageContext1, yi + 2, xi + 1, xi + ptpixlen - gappy);
                imageContext1.setColour(Colour(0x4f787878));
                J_HLine(imageContext1, yi + 3, xi + 1, xi + ptpixlen - gappy);
                imageContext1.setColour(Colour(0x3f787878));
                J_HLine(imageContext1, yi + 4, xi + 1, xi + ptpixlen - gappy);
                imageContext1.setColour(Colour(0x2f787878));
                J_HLine(imageContext1, yi + 5, xi + 1, xi + ptpixlen - gappy);
            }
        }
        else if(ptype == Patt_Grid)
        {
            int gappy = 0;
            {
                imageContext1.setColour(Colour(0x9f3A5A3A));
                J_FillRect(imageContext1, xi, yi, xi + ptpixlen - 1, yi + ptpixheight);

                imageContext1.setColour(Colour(0xff5A855A));
                //J_LineRect(g, x, y - tH + 1, x + pixlen - 1, y - tH + ptheight + 1);
                J_VLine(imageContext1, xi, yi, yi + ptpixheight);
                J_HLine(imageContext1, yi, xi, xi + ptpixlen - gappy);
                J_HLine(imageContext1, yi + ptpixheight, xi, xi + ptpixlen - gappy);

                imageContext1.setColour(Colour(0x6f5A855A));
                J_HLine(imageContext1, yi + 1, xi + 1, xi + ptpixlen - gappy);
                imageContext1.setColour(Colour(0x5f5A855A));
                J_HLine(imageContext1, yi + 2, xi + 1, xi + ptpixlen - gappy);
                imageContext1.setColour(Colour(0x4f5A855A));
                J_HLine(imageContext1, yi + 3, xi + 1, xi + ptpixlen - gappy);
                imageContext1.setColour(Colour(0x3f5A855A));
                J_HLine(imageContext1, yi + 4, xi + 1, xi + ptpixlen - gappy);
                imageContext1.setColour(Colour(0x2f5A855A));
                J_HLine(imageContext1, yi + 5, xi + 1, xi + ptpixlen - gappy);
            }
        }

        if(ltick <= 0 || ahgt <= 0)
        {
            // skip then
        }
        else
        {
            int ascent = 0;
            if(ptype == Patt_Pianoroll)
            {
                ascent = (hgt - ahgt)/2;
            }
            int offset = nmin;
            float cf = (float)hgt/(ahgt);
			float cf2 = (float)hgt/(ahgt*2);

            //Colour col1 = Colour(0xBf5FBF5F); // for samplent
            Colour col1 = Colour(0xBf8F959A); // for samplent
            Colour col2 = Colour(0xDf6F6FEF); // for gennote
            Colour col3 = Colour(0xbf2F8F8F); // for steppy
            Colour col4 = Colour(0xafFFFFFF); // common
            Colour colsl = Colour(0x7fFFFF52); // for slide

            Colour col;
            int ycoord;
            float x1, x2;
            el = first_elem;
            while(el != NULL)
            {
                if(el->IsPresent() && IsElementTypeDisplayable(el->type) && el->type != El_TextString)
                {
                    if(ptype == Patt_Pianoroll || ptype == Patt_StepSeq)
                    {
                        if(el->IsInstance())
                        {
                            ii = (Instance*)el;
                            if(ptype == Patt_Pianoroll)
                            {
                                val = ii->ed_note->value;
                            }
                            else if(ptype == Patt_StepSeq)
                            {
                                val = ii->instr->index;
                            }
                        }
                        else if(el->type == El_SlideNote)
                        {
                            SlideNote* sl = (SlideNote*)el;
                            val = sl->ed_note->value;
                        }
                    }
                    else
                    {
                        val = el->track_line;
                    }

                    if(ptype == Patt_Pianoroll)
                    {
                        if(el->type == El_Samplent)
                        {
                            col = col1;
                        }
                        else if(el->type == El_GenNote)
                        {
                            col = col2;
                        }
                        else if(el->type == El_SlideNote)
                        {
                            col = colsl;
                        }
                        
                        if(ahgt <= hgt)
                        {
                            ycoord = hgt - (val - offset + ascent);
                        }
                        else
                            ycoord = int(hgt - (val - offset)*cf);
                    }
                    else if(ptype == Patt_StepSeq)
                    {
                        col = col3;
                        if(ahgt*2 <= hgt)
                        {
                            ycoord = int(val*2);
                        }
                        else
                            ycoord = int(val*cf2*2);
                    }
                    else if(ptype == Patt_Grid)
                    {
                        if(el->type == El_Samplent)
                        {
                            col = col1;
                        }
                        else if(el->type == El_GenNote)
                        {
                            col = col2;
                        }
                        else if(el->type == El_SlideNote)
                        {
                            col = colsl;
                        }
                        else
                            col = col4;

                        if(ahgt <= hgt)
                        {
                            ycoord = int(val);
                        }
                        else
                            ycoord = int(val*cf);
                    }

                    ycoord += 2;

                    x1 = el->start_tick*tickWidth;
                    if(el->end_tick > 0)
                    {
                        x2 = el->end_tick*tickWidth;
                        if(x2 - x1 < 1.f)
                        {
                            x2 = x1 + 1.f;
                        }
                    }
                    else
                        x2 = x1 + 1;

                    if(ptype == Patt_Pianoroll || ptype == Patt_Grid)
                    {
                        imageContext1.setColour(col);
                        imageContext1.drawHorizontalLine(ycoord, x1, x2);
                        imageContext1.setColour(col.withAlpha(0.1f));
                        imageContext1.drawHorizontalLine(ycoord - 1, x1, x2);
                        imageContext1.drawHorizontalLine(ycoord + 1, x1, x2);
                    }
                    else
                    {
                        imageContext1.setColour(col.withAlpha(0.7f));
                        imageContext1.drawVerticalLine(int(x1), (float)ycoord, (float)ycoord + 2);
                        imageContext1.setColour(col.withAlpha(0.5f));
                        imageContext1.drawVerticalLine(int(x1) + 1, (float)ycoord, (float)ycoord + 2);
                        //imageContext.drawVerticalLine(int(x1) + 2, (float)ycoord, (float)ycoord + 2);
                        imageContext1.setColour(col.withAlpha(0.7f));
                        imageContext1.drawHorizontalLine(ycoord, x1, x1 + 2);
                        //imageContext.setColour(col);
                        //imageContext.drawVerticalLine(int(x1), (float)ycoord, (float)ycoord + 2);
                        //imageContext.drawHorizontalLine(ycoord, x1, x1 + 3);
                    }
                }
                el = el->patt_next;
            }
        }

        if(nmimg != NULL)
        {
            imageContext1.setColour(Colours::white);
            imageContext1.drawImageAt(nmimg, 1, ptpixheight - 12);
        }
    }
}

bool Pattern::IsElementTypeDisplayable(ElemType eltype)
{
    switch(ptype)
    {
        case Patt_Pianoroll:
            return(eltype == El_GenNote || 
                   eltype == El_Samplent || 
                   eltype == El_SlideNote);
        case Patt_StepSeq:
            return(eltype == El_GenNote || 
                   eltype == El_Samplent);
        default:
            return true;
    }
}

void Pattern::MarkAsCopied()
{
    is_copied = true;
    Element* el = first_elem;
    while(el != NULL)
    {
        if(el->IsPresent())
        {
            el->MarkAsCopied();
        }
        el = el->patt_next;
    }
}

/*==================================================================================================
BRIEF: Changes instrument for a given instance. Caller should be aware that the given istance will be
deleted in this function and take care of it.

PARAMETERS:
[IN]
    ii - instance to change instrument for.
    instr - instrument.
[OUT]
N/A

OUTPUT: Newly created element
==================================================================================================*/
Element* ReassignInstrument(Instance* ii, Instrument* instr)
{
    Element* retel = NULL;
    C.SaveState();
    C.SetPattern(ii->patt, C.loc);
    if(instr->type == Instr_Sample)
    {
        Samplent* smp = (Samplent*)CreateElement_Note(instr, true);
        smp->ed_note->value = ii->ed_note->value;
        smp->ed_note->visible = ii->ed_note->visible;
        smp->ed_vol->value = ii->ed_vol->value;
        smp->ed_pan->value = ii->ed_pan->value;
        smp->loc_vol->SetNormalValue(ii->loc_vol->val);
        smp->loc_pan->SetNormalValue(ii->loc_pan->val);
        smp->start_tick = ii->start_tick;
        smp->track_line = ii->track_line;
        smp->Update();
        retel = smp;
    }
    else if(instr->type == Instr_Generator || instr->type == Instr_VSTPlugin)
    {
        Gennote* gn = (Gennote*)CreateElement_Note(instr, true);
        gn->ed_note->value = ii->ed_note->value;
        gn->ed_vol->value = ii->ed_vol->value;
        gn->ed_pan->value = ii->ed_pan->value;
        if(ii->type == El_GenNote)
            gn->ed_len->value = ii->ed_len->value;
        gn->loc_vol->SetNormalValue(ii->loc_vol->val);
        gn->loc_pan->SetNormalValue(ii->loc_pan->val);
        if(ii->type == El_GenNote)
            gn->loc_len->SetNormalValue(ii->loc_len->val);
        gn->start_tick = ii->start_tick;
        gn->end_tick = gn->start_tick + ii->tick_length;
        gn->track_line = ii->track_line;
        gn->Update();
        retel = gn;
    }
    CloneSlideNotes(ii, (Instance*)retel, false);
    DeleteElement(ii, false, false);
    C.RestoreState();
    return retel;
}

void BindPatternToInstrument(Pattern* pt, Instrument* instr)
{
    undoMan->DoNewAction(Action_Bind, (void*)pt, (void*)pt->ibound, (void*)instr);

    pt->OrigPt->ibound = instr;
    Pattern* ptc = pt->OrigPt->der_first;
    while(ptc != NULL)
    {
        ptc->ibound = instr;
        ptc = ptc->der_next;
    }

    //Pattern* cpt = C.patt;
    //C.patt = pt;
    Instance* ii;
    Element* el = pt->OrigPt->first_elem;
    while(el != NULL)
    {
        if(el->IsPresent() && (el->type == El_Samplent || el->type == El_GenNote))
        {
            ii = (Instance*)el;
            if(ii->instr != instr)
            {
                el = el->patt_next;
                ReassignInstrument(ii, instr);
                continue;
            }
        }
        el = el->patt_next;
    }
    //C.patt = cpt;

    pt->OrigPt->UpdateScaledImage();
}

void UpdateStepSeqPatt(Pattern* pt)
{
    if(pt->ptype == Patt_StepSeq)
    {
        // Define instrument for tracks
        aux_panel->PopulatePatternWithInstruments(pt->OrigPt);

        // Update instances
        Instance* ii;
        Trk* trk = pt->OrigPt->first_trkdata;
        while(trk != pt->OrigPt->bottom_trk)
        {
            if(trk->defined_instr != NULL)
            {
                Element* el = pt->OrigPt->first_elem;
                while(el != NULL)
                {
                    if(el->IsInstance())
                    {
                        ii = (Instance*)el;
                        if(ii->instr == trk->defined_instr)
                        {
                            ii->trkdata = trk;
                            ii->trknum = trk->trknum;
                            ii->track_line = trk->start_line;
                        }
                    }
                    el = el->patt_next;
                }
            }
            trk = trk->next;
        }
    }
}

void UpdateStepSequencers()
{
    // To prevent double update of the same original stepseq. pattern, we list them in linked list and perform
    // corresponding checks
    Pattern* ptfirst = NULL; // Pointer to the first pattern in list
    Pattern* ptlast = NULL;  // Pointer to the last pattern in list
    Pattern* ptcheck = NULL;  // Working pointer

    bool doupdate;
    Pattern* pt;
    Element* el = firstElem;
    while(el != NULL)
    {
        if(el->type == El_Pattern)
        {
            pt = (Pattern*)el;
            if(pt->ptype == Patt_StepSeq)
            {
                doupdate = true;
                ptcheck = ptfirst;
                while(ptcheck != NULL)
                {
                    if(ptcheck == pt->OrigPt)
                    {
                        doupdate = false;
                        break;
                    }
                    ptcheck = (Pattern*)ptcheck->next;
                }

                if(doupdate)
                {
                    UpdateStepSeqPatt(pt);
                    if(ptlast != NULL)
                    {
                        ptlast->next = pt->OrigPt;
                    }
                    else // first pattern
                    {
                        ptfirst = pt->OrigPt;
                    }
                    ptlast = pt->OrigPt;
                    ptlast->next = NULL;
                }
            }
        }
        el = el->next;
    }

    UpdateStepSeqPatt(aux_panel->blankPt);
}

void CloneSlideNotes(Instance* src, Instance* dst, bool select)
{
    SlideNote* sn;
    SlideNote* sn0 = src->first_slide;
    while(sn0 != NULL)
    {
        if(sn0->IsPresent())
        {
            sn = sn0->Clone(true);
            if(select)
            {
                sn->xs = sn0->xs;
                sn->ys = sn0->ys;
                sn->picked = true;
                sn->selected = sn0->selected;
                sn->highlighted = sn0->highlighted;
                sn->selloc = sn0->selloc;
            }
            dst->AddSlideNote(sn);
        }
        sn0 = sn0->s_next;
    }
}

