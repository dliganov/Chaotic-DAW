
#include "awful.h"
#include "awful_cursorandmouse.h"
#include "awful_elements.h"
#include "awful_panels.h"
#include "awful_preview.h"

Cursor::Cursor()
{
    type = CType_UnderVert;
    mode = CMODE_UNKNOWN;
    last_note = 60;
    last_char = 'q';
    patt = NULL;
    curElem = NULL;
    curParam = NULL;
    slideparent = NULL;
    loc = Loc_UNKNOWN;
    trknum = 0;
    trk = maintrk = NULL;
    fieldposx = 0;
	fieldposy = 0;
	staticposx = 0;
	staticposy = 0;
    lastslidelength = 4;
    pianokey_num = 0;
    pianokey_pressed = false;
    pianokey_keybound = -1;
}

void Cursor::SaveState()
{
    tmp_line = CLine;
    tmp_tick = CTick;
    tmp_patt = patt;
    tmp_loc = loc;
}

void Cursor::RestoreState()
{
    SetPattern(tmp_patt, tmp_loc);
    SetPos(tmp_tick, tmp_line);
}

void Cursor::SetPattern(Pattern* pattern, Loc location)
{
    patt = pattern;
    loc = location;
}

void Cursor::SetPatternContext(int mouse_x, int mouse_y)
{
    Pattern* pt;
    Element* el = firstElem;
    float tx = X2Tick(mouse_x, loc);
    int ty = Y2Line(mouse_y, loc);
    while(el != NULL)
    {
        if(el->type == El_Pattern)
        {
            pt = (Pattern*)el;
            if(pt->folded == false)
            {
                if(tx >= pt->start_tick && tx <= pt->end_tick && ty >= pt->track_line && ty <= pt->track_line_end)
                {
                    patt = (Pattern*)el;
                }
            }
        }
		el = el->next;
    }
}

void Cursor::PlaceCursor()
{
    if(patt == aux_panel->workPt && aux_panel->auxmode != AuxMode_Pattern)
    {
        type = CType_None;
    }
    else if(patt != field_pattern && aux_panel->workPt != patt)
    {
        type = CType_None;
    }
    else if(patt == aux_panel->workPt && aux_panel->workPt->ptype == Patt_StepSeq)
    {
        if(curElem != NULL)
        {
            //curElem->PlaceCursor();
            type = CType_None;
        }
        else
        {
            type = CType_Quad;
        }
    }
    else if(patt == aux_panel->workPt && aux_panel->workPt->ptype == Patt_Pianoroll)
    {
        if(curElem != NULL)
        {
            curElem->PlaceCursor();
        }
        type = CType_None;
    }
    else
    {
        if(mode == CMode_ElemParamEdit || mode == CMode_JustParamEdit)
        {
            curParam->PlaceCursor();
        }
        else if(mode == CMode_ElemEdit)
        {
            curElem->PlaceCursor();
        }
        else if(mode == CMode_NotePlacing)
        {
            type = CType_InstanceNote;
        }
        else if(mode == CMode_Sliding)
        {
            type = CType_SlideNote;
        }
        else
        {
            type = CType_UnderVert;
        }
    }
}

void Cursor::AdvanceView(float dtick, int dline)
{
    if(loc == Loc_MainGrid)
    {
        if(dtick != 0)
        {
            OffsTick += dtick;
            if(OffsTick < 0)
                OffsTick = 0;
        }

        if(dline != 0)
        {
            OffsLine += dline;
            if(OffsLine < 0)
                OffsLine = 0;
            else if(OffsLine >= patt->basePattern->num_lines)
            {
                OffsLine -= dline;
            }
        }

        UpdateElementsVisibility();

        R(Refresh_Grid);
        if(mixbrowse == false)
        {
            R(Refresh_MixCenter);
        }

        if(aux_panel->auxmode == AuxMode_Vols ||
           aux_panel->auxmode == AuxMode_Pans)
        {
            R(Refresh_Aux);
        }
    }
    else if(loc == Loc_SmallGrid)
    {
        if(dtick != 0)
        {
            aux_panel->OffsTick += dtick;
            if(aux_panel->OffsTick < 0)
                aux_panel->OffsTick = 0;
        }

        /*
        if(dline != 0)
        {
            gAux->OffsLine += dline;
            if(gAux->OffsLine < 0)
                gAux->OffsLine = 0;
            else if(gAux->OffsLine >= patt->OrigPt->num_lines)
            {
                gAux->OffsLine -= dline;
            }
        }
        */

        R(Refresh_Aux);
    }
    PosUpdate();
}

void Cursor::AdvanceTick(float dtick)
{
    if(qsize > 0)
    {
        CTick += dtick*qsize;
    }
    else
    {
        CursX += (int)dtick;
        CTick = X2Tick(CursX, loc);
    }

    if(CTick < 0)
    {
        CTick = 0;
    }
    PosUpdate();
}

void Cursor::AdvanceLine(int dline)
{
    if(dline < 0)
    {
        if(CLine > 0)
        {
            if(CLine == trk->trk_start_line)
            {
                if(C.trk->prev != NULL)
                {
                    CLine = trk->prev->trk_end_line;
                }
            }
            else
            {
                CLine--;
            }
        }
    }
    else if(dline > 0)
    {
        if(CLine == trk->trk_end_line)
        {
            if(C.trk->next != bottom_trk && !(loc == Loc_SmallGrid && aux_panel->workPt->ptype == Patt_StepSeq && trk->next->defined_instr == NULL))
            {
                CLine = trk->next->trk_start_line;
            }
        }
        else
        {
            CLine++;
        }
    }

    PosUpdate();

    // Definarium
    if(trk != NULL && loc == Loc_SmallGrid && aux_panel->workPt->ptype == Patt_StepSeq && trk->defined_instr != NULL)
    {
        ChangeCurrentInstrument(trk->defined_instr);
    }
}

void Cursor::SetPos(float tick, int line)
{
    CTick = tick;
    CLine = line;

    PosUpdate();
}

void Cursor::PatternUpdate()
{
    if(loc == Loc_MainGrid)
    {
        patt = field_pattern;
    }
    else if(loc == Loc_SmallGrid)
    {
        patt = aux_panel->workPt;
    }
}

void Cursor::PosUpdate()
{
    trk = GetTrkDataForLine(CLine, patt->basePattern);
    if(trk != NULL)
    {
		if(CLine > trk->trk_end_line)
			CLine = trk->trk_end_line;
		else if(CLine < trk->trk_start_line)
			CLine = trk->trk_start_line;

		trknum = trk->trknum;

        if(trk->trkbunch != NULL)
        {
            trk = trk->trkbunch->trk_start;
            trknum = trk->trknum;
        }
    }

    Element* existel = NULL;
    if(loc == Loc_MainGrid)
    {
        CursX = Tick2X(CTick + patt->start_tick, loc);
        CursY = Line2Y(CLine + patt->track_line, loc);
        existel = IsElemExists(CTick, CLine, field_pattern);
        fieldposx = CTick;
        fieldposy = CLine;
        maintrk = trk;
    }
    else if(loc == Loc_SmallGrid)
    {
        CursX = Tick2X(CTick, loc);
        CursY = Line2Y(CLine, loc) - aux_panel->bottomincr;
        existel = aux_panel->workPt->basePattern->IsElemExists(CTick, CLine);
    }
    CursX0 = CursX;

    qsize = loc == Loc_SmallGrid ? aux_panel->quantsize : quantsize;

    // Activate an element in position if any
    if(curElem == NULL && existel != NULL && (type == CType_Vertical || type == CType_UnderVert || type == CType_InstanceNote))
    {
        //existel->Activate();
    }
}

Element* Cursor::CheckVisibility()
{
	Element* hiding_element = NULL;
	Element* el = firstElem;
	float fpx = (float)(CTick + patt->start_tick);
	float fpy = (float)(CLine + patt->track_line);

    if(mode == CMode_JustParamEdit)
    {
        return NULL;
    }
    else if(loc == Loc_SmallGrid)
    {
        if((CursX < GridXS1)||(CursX > GridXS2)||(CursY < GridYS1)||(CursY > GridYS2 + aux_panel->lineHeight))
        {
            return aux_panel->workPt;
        }
        else
        {
            return NULL;
        }
    }
    else if(loc == Loc_StaticGrid && 
            (static_visible == false ||
            ((CursX < StX1)||(CursX  > StX2)||(CursY <= GridY1)||(CursY > GridY2 + lineHeight))))
    {
        return st->patt;
    }
    else if(loc == Loc_MainGrid && 
           ((CursX < GridX1)||(CursX > GridX2)||(CursY <= GridY1)||(CursY > GridY2 + lineHeight)))
    {
        {
            return field_pattern;
        }
    }

    Pattern* pt;
    while(el != NULL)
    {
        if((mode == CMode_TxtDefault)&&(el->type == El_Pattern))
        {
            pt = (Pattern*)el;
            if(pt->folded == false)
            {
                if(fpx >= pt->start_tick && fpx <= pt->end_tick && fpy >= pt->track_line && fpy <= pt->track_line_end)
                {
    				if(pt != patt)
    				{
    					hiding_element = pt;
    				}
                }
            }
        }
		el = el->next;
    }
	return hiding_element;
}

void Cursor::ExitPerClickInAnotherGridPlace()
{
    if(mode == CMode_ElemParamEdit)
    {
        if(curParam->type == Param_Note &&
           !(loc == Loc_SmallGrid && aux_panel->workPt->ptype == Patt_StepSeq)&&
           (CursModeCorrespondsToCursorMode()))
        {
            //if(curElem->type == El_SlideNote)
            //{
            //    ExitToSlidingMode(((SlideNote*)curElem)->parent, ((SlideNote*)curElem)->ed_note->value);
            //}
            //else
            {
                ExitToNoteMode(((Samplent*)curElem)->sample, ((Note*)curParam)->value);
            }
        }
        else
        {
            ExitToCurrentMode();
        }
    }
    else if(mode == CMode_JustParamEdit)
    {
        ExitToCurrentMode();
    }
	else if(mode == CMode_ElemEdit)
	{
        //if(curElem->type == El_SlideNote)
        //{
		//	ExitToSlidingMode(((SlideNote*)curElem)->parent, ((SlideNote*)curElem)->ed_note->value);
        //}
        //else
        {
		    ExitToCurrentMode();
        }
	}
}

void Cursor::ExitToDefaultMode()
{
    if(curElem != NULL)
    {
        SetPos(curElem->start_tick, curElem->track_line);
        curElem->Leave();
    }

    if(curParam != NULL)
    {
        curParam->Leave();
    }

    SwitchInputMode(CMode_TxtDefault);
    // Force TxtDefault mode
    mode = CMode_TxtDefault;
    curElem = NULL;
    curParam = NULL;
}

void Cursor::ExitToCurrentMode()
{
    if(curElem != NULL)
    {
        SetPos(curElem->start_tick, curElem->track_line);
        curElem->Leave();
    }

    if(curParam != NULL)
    {
        curParam->Leave();;
    }

    if(loc == Loc_Other)
    {
        loc = Loc_MainGrid;
    }
    mode = CMODE_UNKNOWN;
    SwitchInputMode(mode);
    curElem = NULL;
    curParam = NULL;

    PosUpdate();
}

void Cursor::ExitToNoteMode(Instrument* instr, int note_num)
{
    jassert(curElem != NULL);
    if(loc == Loc_MainGrid)
    {
        if(!(aux_panel->auxmode == AuxMode_Pattern && aux_panel->workPt->basePattern->autopatt))
        {
            ChangeCurrentInstrument(instr);
        }
        SetPos(curElem->start_tick, curElem->track_line);
        curElem->Leave();

        mode = CMode_NotePlacing;
        type = CType_InstanceNote;
    	curElem = NULL;
    	curParam = NULL;

        last_note = note_num;
    }
    else if(loc == Loc_SmallGrid)
    {
        if(!(aux_panel->auxmode == AuxMode_Pattern && aux_panel->workPt->basePattern->autopatt))
        {
            ChangeCurrentInstrument(instr);
        }
        SetPos(curElem->start_tick, curElem->track_line);
        curElem->Leave();

        mode = CMode_NotePlacing;
        type = CType_InstanceNote;
        curElem = NULL;
        curParam = NULL;
    }

    PosUpdate();
}

void Cursor::ExitToSlidingMode(Element* parent, int note_num)
{
    if(!(loc == Loc_Other))
    {
        if(curElem != NULL)
            curElem->Leave();

        mode = CMode_Sliding;
        type = CType_SlideNote;
        curElem = NULL;
        curParam = NULL;

        if(parent != NULL && 
           (parent->type == El_GenNote || parent->type == El_Samplent))
        {
            slideparent = (NoteInstance*)parent;
        }
    	else
        {
            slideparent = NULL;
        }
        last_note = note_num;
        PosUpdate();
        CP->CurrentMode->Release();
        CP->CurrentMode = CP->SlideMode;
        CP->CurrentMode->Press();
        R(Refresh_Buttons);
    }
}

SlideNote* Cursor::PlaceSlideNote(char character, unsigned flags)
{
    SlideNote* sl = NULL;
    sl = CreateElement_SlideNote(true);
/*    if(flags & kbd_ctrl && C.curElem != NULL)
    {
        if(C.curElem->IsInstance())
        {
            Instance* ii = (Instance*)C.curElem;
            ii->AddSlideNote(sl);
        }
    }*/

    if(!(loc == Loc_SmallGrid && aux_panel->workPt->ptype == Patt_Pianoroll))
    {
        sl->ed_note->Show();
        sl->Switch2Param(sl->ed_note);
        sl->ed_note->ProcessChar(character);
        sl->ed_note->Kreview(character);
    }
    else
    {
        sl->ed_note->Hide();
        sl->Switch2Param(NULL);
        sl->ed_note->Kreview(character);
    }

    return sl;
}

void Cursor::PosAndCursor2Mouse(int mouse_x, int mouse_y)
{
	if(M.snappedTick >= 0 && M.snappedLine < field_pattern->num_lines)
	{
		if(M.loc == Loc_MainGrid)
		{
            if(loc != Loc_MainGrid && CP->CurrentMode == CP->SlideMode && slideparent != NULL)
            {
                SwitchInputMode(CMode_TxtDefault);
            }

            SetPattern(field_pattern, Loc_MainGrid);
            SetPatternContext(mouse_x, mouse_y);
		}
		else if(M.loc == Loc_SmallGrid)
		{
			if(loc != Loc_SmallGrid && CP->CurrentMode == CP->SlideMode && slideparent != NULL)
			{
				SwitchInputMode(CMode_TxtDefault);
			}

            SetPattern(aux_panel->workPt, Loc_SmallGrid);
        }
        else if(M.loc == Loc_StaticGrid)
        {
            SetPattern(st->patt, Loc_StaticGrid);
        }

        SetPos(M.snappedTick, M.snappedLine);
    }
}

Mouse::Mouse()
{
    mmode = 0;
    selmode = Sel_Usual;
    btype = Brw_None;
    brwindex = 0;
    active_command = NULL;
    active_ctrl = NULL;
    lasthintctrl = NULL;
    patt = NULL;
    active_slide = NULL;
    active_instr = NULL;
    dropinstr1 = NULL;
    dropinstr2 = NULL;
    dropeff1 = NULL;
    dropeff2 = NULL;
    active_env = NULL;
    active_pnt = NULL;
    env_xstart = 0;
    active_paramedit = NULL;
    active_filedata = NULL;
    active_elem = NULL;
    resize_elem = NULL;
    active_samplent = NULL;
    active_longnote = NULL;
    active_mixcell = NULL;
    active_dropmixcell = NULL;
    active_mixchannel = NULL;
    active_dropmixchannel = NULL;
    on_effects = false;
    mixcellindexing = false;
    auxcellindexing = false;
    active_paramfx = NULL;
    active_numba = NULL;
    active_panel = NULL;
    active_menu = NULL;
    skip_menu = false;
    v_only = false;
    current_track_num = 0;
    current_trk = NULL;
    current_line = 0;
    current_tick = 0;
    current_pt_line = 0;
    current_pt_tick = 0;
    track_bunch = NULL;
    dragcount = 0;
    selstarted = false;
    loostarted = false;
    LMB = false;    // left mouse button is pressed (if true)
    RMB = false;    // right mouse button is pressed (if true)
    bypass_element_activating = false;
    menu_active = false;
    at_definer = false;
    loc = Loc_UNKNOWN;
    lineloc = Loc_UNKNOWN;
    pickloc = Loc_UNKNOWN;
    selloc = Loc_UNKNOWN;
    quanting = false;
    on_keys = false;
    pianokey_num = 0;
    pianokey_pressed = false;
    pianokey_vol = 0;
    pianokey_slot = 0xFFFF;
    contextmenu = NULL;
    prbunch = false;
    mouse_x = 0;
    mouse_y = 0;
    prev_x = -1;
    prev_y = -1;
    snappedTick = 0;
    snappedLine = 0;
    movetick = 0;
    moveline = 0;
    current_tick = 0;
    lpx = 0;
    lpy = 0;
    prepianorolling = false;
    pickedup = false;
    hint_active = false;
    mousefix = false;
    bpmbig = false;
    env_action = ENVUSUAL;
    brushbasetick = 0;
    brushwidth = 0;
    brushmode = Brush_Default;
    adtick = -1;
    adtrack = -1;
    holding = false;
}

void Mouse::PosUpdate(int mx, int my, unsigned flags, bool reset)
{
    mouse_x = mx;
    mouse_y = my;

    //if(skip_after_menu == false)
    {
        CalcPosition(mx, my);
    }

    if((RMB == false && LMB == false && holding == false) || reset)
    {
        PosCheck(mx, my);
    }

    UpdateElementsVisibility();
    SetMouseImage(flags);

    C.PosUpdate();
}

void Mouse::CalcTickAndLine(int mx, int my, float * tick, int * line, Loc cloc)
{
    float currtick = X2Tick(mx, cloc);
    float tk;
    if(cloc == Loc_MainGrid)
    {
        tk = currtick - patt->start_tick;
    }
    else
    {
        tk = currtick;
    }
    *tick = (float)(int)tk;

    float q = (cloc == Loc_MainGrid) ? quantsize : aux_panel->quantsize;
    if(q < 1)
    {
        if(q > 0)
        {
            *tick += (int((tk - *tick)/q))*q;
        }
        else
        {
            *tick = tk;
        }
    }
    else if(q > 1)
    {
        int pos = (int)*tick;
        while(pos%(int)q)
        {
            pos--;
        }
        *tick = (float)pos;
    }

    if(cloc == Loc_MainGrid)
    {
        *line = Y2Line(my, Loc_MainGrid) - patt->track_line;
    }
    else if(cloc == Loc_SmallGrid)
    {
        *line = Y2Line(my + aux_panel->bottomincr, Loc_SmallGrid);
    }
}

void Mouse::CalcPosition(int mx, int my)
{
    loc = Loc_UNKNOWN;
    patt = field_pattern;

    if(IsMouseOnMainGrid(mx, my) == true)
    {
        loc = Loc_MainGrid;
        //M.patt = CheckPosForPatterns(X2Tick(mouse_x, Loc_MainGrid), Y2Line(mouse_y, Loc_MainGrid), field);
        patt = field_pattern;
    }
    else if(IsMouseOnSmallGrid(mx, my) == true)
    {
        loc = Loc_SmallGrid;
        patt = aux_panel->workPt;
        current_line = Y2Line(my + aux_panel->bottomincr, Loc_SmallGrid);
        current_trk = GetTrkDataForLine(current_line, aux_panel->workPt->basePattern);
    }

    if(loc == Loc_MainGrid || loc == Loc_SmallGrid)
    {
        current_tick = X2Tick(mx, loc);
        qsize = (loc == Loc_MainGrid) ? quantsize : aux_panel->quantsize;
        CalcTickAndLine(mx, my, &snappedTick, &snappedLine, loc);
    }

    if(pickedup)
    {
        CalcTickAndLine(mx, my, &dragTick, &dragLine, pickloc);
    }
}

void Mouse::PosCheck(int mx, int my)
{
    if(M.mmode & MOUSE_AUXMIXING && M.auxcellindexing)
    {
        R(Refresh_AuxHighlights);
    }

    if(M.menu_active == false)
    {
        M.mmode = 0;
        M.on_keys = false;
        M.at_definer = false;
        M.active_instr = NULL;
        M.active_mixcell = NULL;
        M.active_mixchannel = NULL;
        M.active_effect = NULL;
        M.active_numba = NULL;
        M.active_panel = NULL;
        M.active_elem = NULL;
        M.btype = Brw_None;
        M.prepianorolling = false;
        M.patternbrush = false;

        bool skip_controls = false;

        Panel* p = firstPanel;
        while(p != NULL)
        {
            if(p->active == true)
            {
                if(p->CheckMouse(mx, my) == true)
                {
                    M.active_panel = p;
                    break;
                }
            }
            p = p->next;
        }

        if(IsMouseOnAux(mx,my) == true)
        {
            M.mmode |= MOUSE_AUXING;
            M.active_panel = aux_panel;

            if(aux_panel->auxmode == AuxMode_Pattern)
            {
                M.on_keys = aux_panel->CheckIfMouseOnKeys(mx, my, &M.pianokey_num, &(M.pianokey_vol));

                if(M.on_keys == true)
                {
                    M.current_line = Y2Line(my + aux_panel->bottomincr, Loc_SmallGrid);
                    M.current_trk = GetTrkDataForLine(M.current_line, aux_panel->workPt->basePattern);
                }

                if(mx >= GridXS1 && mx <= GridXS2 && 
                   my > (GridYS2 - 4) && my < (GridYS2 + 4))
                {
                    M.mmode |= MOUSE_LOWAUXEDGE;
                }

                if(mx >= GridXS1 && mx <= GridXS2 && 
                   my > (aux_panel->patty2 - aux_panel->eux + 1) && my < aux_panel->patty2)
                {
                    M.mmode |= MOUSE_AUXAUXING;
                }

                if(aux_panel->ltype == Lane_Pitch)
                {
                    Envelope* env = (Envelope*)aux_panel->workPt->basePattern->pitch->paramedit;
                    env->Check(mx, my);
                }
            }
        }

        if(M.loc == Loc_MainGrid || M.loc == Loc_SmallGrid || M.loc == Loc_StaticGrid)
        {
            M.mmode |= MOUSE_ON_GRID;
        }

        if(M.mmode & MOUSE_ON_GRID)
        {
            CheckGridStuff(mx, my);
        }
        else if(mx > MainX1 && mx < (MainX1 + 25) && my > GridY1 && my < GridY2)
        {
            M.mmode |= MOUSE_TRACKING;
            M.current_line = Y2Line(my, Loc_MainGrid);
            M.current_track_num = GetTrkNumForLine(M.current_line, M.patt);

            if(M.current_track_num >= 0 && M.current_track_num <= 999)
            {
                M.current_trk = GetTrkDataForLine(M.current_line, M.patt);
            }
        }

        if(static_visible == true && abs(mx - (GridX1 - 2)) <= 3 && my > MainY1 && my < MainY2)
        {
            //M.mmode |= MOUSE_ON_STATIC_EDGE;
        }
        else if(mx <= MainX1 + 2 && mx >= MainX1 - 6 && my >= MainY1)
        {
            M.mmode |= MOUSE_GRID_X1;
            skip_controls = true;
        }
        /*
        else if(mx <= MainX2 + 6 && mx >= MainX2 - 4 && ((mixbrowse == false && my >= MainY1) || (mixbrowse == true && my > 0)) && my < MainY2)
        {
            M.mmode |= MOUSE_GRID_X2;
            skip_controls = true;
        }
        else if(mx <= MainX2 + MixCenterWidth+4 + 4 && mx >= MainX2 + MixCenterWidth+4 - 4 && (my >= 0 && my < WindHeight))
        {
            M.mmode |= MOUSE_GRID_X22;
            skip_controls = true;
        }
        */
        else if(my <= MainY1 - 2 && my >= MainY1 - 5 && 
                ((mixbrowse == false && mx < MainX2 + MixCenterWidth) ||(mixbrowse == true && mx < MainX2)))
        {
            M.mmode |= MOUSE_GRID_Y1;
            skip_controls = true;
        }
        else if(my <= MainY2 + 4 && my >= MainY2 - 1 && mx > MainX1 && mx < MainX2 + MixCenterWidth)
        {
            M.mmode |= MOUSE_GRID_Y2;
            skip_controls = true;
        }
        else if(mx > MainX1 - InstrPanelWidth - 7 && mx < MainX1 - InstrPanelWidth - 1 && my >= MainY1)
        {
            M.mmode |= MOUSE_ON_GENBROWSER_EDGE;
            M.btype = Brw_Generators;
            skip_controls = true;
        }
        else if(genBrw->active == true && (mx > genBrw->x &&
                                       mx < genBrw->x + genBrw->width &&
                                       my > genBrw->y &&
                                       my < genBrw->y + genBrw->height))
        {
            M.mmode |= MOUSE_BROWSING;
            M.active_panel = genBrw;

            M.btype = Brw_Generators;
            M.brwindex = (my - genBrw->y - (int)genBrw->main_offs)/BrwEntryHeight;
        }
        else if(mixbrowse && mx >= WindWidth - MixCenterWidth - 4)
        {
            if(abs(mx - mixBrw->x) < 5 && my > MainY1)
            {
                M.mmode |= MOUSE_ON_MIXBROWSER_EDGE;
                M.btype = Brw_Effects;
                skip_controls = true;
            }
            else if(mx > mixBrw->x &&
                             mx < mixBrw->x + mixBrw->width &&
                             my > mixBrw->y &&
                             my < mixBrw->y + mixBrw->height)
            {
                M.mmode |= MOUSE_BROWSING;
                M.active_panel = mixBrw;

                M.btype = Brw_Effects;
                M.brwindex = (my - mixBrw->y - (int)mixBrw->main_offs)/BrwEntryHeight;
            }
        }
		/*
        else if(mix->visible == true && IsMouseOnMixer(mx, my) == true)
        {
            M.mmode |= MOUSE_MIXING;
            M.active_panel = mix;

            CheckMixerStuff(mx, my);
        }
		*/
        else if(aux_panel->auxmode == AuxMode_Mixer && M.mmode & MOUSE_AUXING)
        {
            M.mmode |= MOUSE_AUXMIXING;

            CheckAuxMixerStuff(mx, my);
        }

        if(M.active_panel == NULL &&
           mx >= CP->bpmer->x && mx <= CP->bpmer->x + 54 &&
           my >= CP->bpmer->y && my <= CP->bpmer->y + CP->bpmer->height)
        {
            M.mmode |= MOUSE_BPMING;
            beats_per_minute_temp = beats_per_minute;
            CP->bpmer->fnum = &beats_per_minute_temp;

            CP->bpmer->xs = mx;
            CP->bpmer->ys = my;
            int intwidth = beats_per_minute_temp > 99 ? int(CP->bpmer->width/2.5) : int(CP->bpmer->width/3.5);
            if(mx <= CP->bpmer->x + intwidth)
            {
                M.bpmbig = true;
            }
            else
            {
                M.bpmbig = false;
            }
        }

        if(IsMouseOnLiner(mx, my) == true)
        {
            M.mmode |= MOUSE_LINING;
            M.lineloc = Loc_MainGrid;
        }
        else if(aux_panel->auxmode == AuxMode_Pattern && 
                mx >= GridXS1 && mx <= GridXS2 && my >= AuxY1 + 21 && my <= AuxY1 + 33)
        {
            M.mmode |= MOUSE_LINING;
            M.lineloc = Loc_SmallGrid;
        }
        else
        {
            M.lineloc = Loc_UNKNOWN;
        }


        if(M.mmode & MOUSE_ON_GRID)
        {
            Element* el = CheckElementsHighlights(mx, my);

            // Resolve resize and highlight conflicts
            if(el != NULL && M.mmode & MOUSE_RESIZE)
            {
                if(M.resize_object == Resize_GridElem && M.resize_elem != el &&
                    M.edge == RIGHT_X && M.resize_elem->end_tick > el->start_tick)
                {
                    M.mmode &= ~MOUSE_RESIZE;
                }
                else if(M.active_elem != NULL)
                {
                    // If resize should stay, then reset any highlights
                    M.active_elem->highlighted = false;
                    if(M.active_elem->patt == field_pattern)
                    {
                        R(Refresh_GridContent);
                    }
                    else
                    {
                        R(Refresh_AuxContent);
                    }
                    M.active_elem = NULL;
                }
            }
        }
        else
        {
            bool rMain = false;
            bool rAux = false;
            if(ResetHighlights(&rMain, &rAux)) // If changed some highlights
			{
				if(rMain)
				{
					R(Refresh_GridContent);
				}

				if(rAux)
				{
					R(Refresh_AuxContent);
				}
			}
        }

        M.active_paramedit = CheckPEditHighlights(mx, my);

        if(skip_controls == false)
        {
            // check controls
            M.active_ctrl = NULL;
            Control* ct = firstCtrl;
            while(ct != NULL)
            {
				if(ct == aux_panel->playbt)
					int a = 1;
				if(ct->active == true && (ct->panel == NULL || 
                                           M.active_panel == NULL || 
                                           ct->panel == M.active_panel || 
                                          (M.active_panel->type == Panel_Mixer && ct->panel->type == Panel_MixCell) ||
                                          (M.active_panel->type == Panel_Aux && ct->panel->type == Panel_Number) ||
										  (M.active_panel->type == Panel_Aux && ct->panel->type == Panel_MixChannel)))
                {
					if(ct->MouseCheck(mx, my) == true)
                    {
                        M.mmode |= MOUSE_CONTROLLING;
                        M.active_ctrl = ct;
                        ct->xs = mx;
                        ct->ys = my;
                        //char mmode[155];
                        //sprintf(mmode, "Control: %d, mx: %d, my: %d, x: %d, y: %d, dx: %d, dy: %d, dw: %d, dh: %d", ct->type, mx, my, 
                        //                            ct->x, ct->y, ct->drawarea->ax, ct->drawarea->ay,
                        //                            ct->drawarea->aw, ct->drawarea->ah);
                        //LogData(mmode);
                        break;
                    }
                }
                ct = ct->next;
            }

            // Reset hint viewer if took off the control
            if(M.active_ctrl == NULL)
            {
                M.lasthintctrl = NULL;
            }

            if(M.active_ctrl == CP->qMenu || M.active_ctrl == aux_panel->qMenu)
            {
                M.active_menu = (Menu*)M.active_ctrl;
                //M.active_ctrl = ((Menu*)M.active_ctrl)->first_item;
                M.quanting = true;
                M.menu_active = true;

                Menu* mn = M.active_menu;
                int yh = 15*mn->num_items + 13;
                if(mn == CP->qMenu)
                {
                	mn->drawarea->EnableWithBounds(mn->x - 1, mn->y, 59, yh);
                }
                else if(mn == aux_panel->qMenu)
                {
                	mn->drawarea->EnableWithBounds(mn->x - 1, mn->y - yh, 59, yh + 20);
                }

                M.active_menu->first_item->active = false;
                MenuItem* mi = M.active_menu->first_item->inext;
                while(mi != NULL)
                {
                    mi->active = true;
                    mi = mi->inext;
                }
            }
        }

        if(my > IP->y + brw_heading + 1 && 
           my < IP->y + brw_heading + 1 + InstrPanelHeight)
        {
            Instrument* i = first_instr;
            while(i != NULL)
            {
                if(i->visible && (mx >= i->x && mx <= i->x + i->width - 9)&&
                                 (my >= i->y + 2 && my <= i->y + 2 + i->height))
                {
                    M.mmode |= MOUSE_INSTRUMENTING;
                    M.active_instr = i;
                    break;
                }
                i = i->next;
            }
        }
    }
    else
    {
        if(M.quanting == true && M.active_menu->MouseCheck(mx, my) == false)
        {
            M.quanting = false;
            M.menu_active = false;
        
            M.mmode &= ~MOUSE_CONTROLLING;
            M.active_ctrl = NULL;
            MenuItem* mi = M.active_menu->first_item;
            while(mi != NULL)
            {
                mi->active = false;
                mi = mi->inext;
            }
            M.active_menu = NULL;
        }
        else
        {
            M.mmode &= ~MOUSE_CONTROLLING;
            M.active_ctrl = NULL;
            Control* ct = firstCtrl;
            while(ct != NULL)
            {
                if(ct->type == Ctrl_MenuItem && ct->active == true && ((MenuItem*)ct)->itemtype != MItem_Divider && ct->MouseCheck(mx, my) == true)
                {
                    M.mmode |= MOUSE_CONTROLLING;
                    M.active_ctrl = ct;
                    ct->xs = mx;
                    ct->ys = my;
                    break;
                }
                ct = ct->next;
            }
        }
    }
}

bool Mouse::IsMouseOnMainGrid(int mx, int my)
{
    if((mx > GridX1)&&(mx < GridX2)&&(my > GridY1)&&(my < GridY2))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Mouse::IsMouseOnSmallGrid(int mx,  int my)
{
    if(aux_panel->auxmode == AuxMode_Pattern && mx >= GridXS1 && mx <= GridXS2 && my >= GridYS1 && my <= GridYS2)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Mouse::IsMouseOnStaticGrid(int mx, int my)
{
    if(static_visible == true && mx > MainX1 && mx < (GridX1 - 2) && my > GridY1 && my < GridY2)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Mouse::IsMouseOnInstrPane(int mx, int my)
{
	if((mx > (MainX1 - 1 - InstrPanelWidth))&&(mx < (MainX1 - 2))&&
       (mx > MainY1)&&(my < (MainY1 + InstrPanelHeight)))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Mouse::IsMouseOnAux(int mx, int my)
{
    if((mx > MainX1)&&(mx < MainX2 + 3 + 136 + 30 + 2)&&(my > GridY2 + 2)&&(my < WindHeight - 2))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Mouse::IsMouseOnLiner(int mx, int my)
{
    if((mx > GridX1)&&(mx < GridX2)&&(my >= MainY1 - 1)&&(my < MainY1 + LinerHeight))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Mouse::IsMouseOnMixer(int mx, int my)
{
    int xb = MainX2 + MixCenterWidth + 2;
    if(mx > xb && mx < WindWidth && my > 27 && my < WindHeight)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void* Mouse::IsMouseOnBunch(int mx, int my)
{
	M.track_bunch = NULL;
    int gmy = my - GridY1 + OffsLine*lineHeight;
    TrkBunch* tb = M.patt->bunch_first;
    int pixoffs = M.patt->track_line*lineHeight;
    while(tb != NULL)
    {
        if(gmy > (pixoffs + tb->startpix - 3) &&  gmy < (pixoffs + tb->endpix + 3))
        {
            M.track_bunch = tb;
        }

        if(gmy > (pixoffs + tb->startpix - 3) && gmy < (pixoffs + tb->startpix + 3))
        {
            M.mmode |= MOUSE_UPPER_TRACK_EDGE;
            M.track_bunch = tb;
            M.prbunch = false;
            return tb;
        }
        else if(tb->trk_end->vol_lane.visible == false && tb->trk_end->pan_lane.visible == false && gmy > (pixoffs + tb->endpix - 3) && gmy < (pixoffs + tb->endpix + 3))
        {
            M.mmode |= MOUSE_LOWER_TRACK_EDGE;
            M.track_bunch = tb;
            M.prbunch = false;
            return tb;
        }
        tb = tb->next;
    }

    return NULL;
}

void Mouse::CheckAuxMixerStuff(int mx, int my)
{
    MixChannel* mchan = NULL;
    mchan = &aux_panel->masterchan;
    if(mchan->visible && mx >= mchan->x && mx <= (mchan->x + mchan->width) && 
                         my >= mchan->y && my <= (mchan->y + mchan->height))
    {
        M.active_mixchannel = mchan;
    }
    else
    {
        for(int mc = 0; mc < NUM_MIXCHANNELS; mc++)
        {
            mchan = &aux_panel->mchan[mc];
            if(mchan->visible)
            {
                if(mx >= mchan->x && mx <= (mchan->x + mchan->width) && 
                   my >= (mchan->y - 10) && my <= (mchan->y + mchan->height))
                {
                    M.active_mixchannel = mchan;
                    if(mx > (mchan->x + 78) && mx < (mchan->x + 92) && my < mchan->y && my > mchan->y - 9)
                    {
                        M.auxcellindexing = true;
                    }
                    else
                    {
                        M.auxcellindexing = false;
                    }
                    break;
                }
            }
        }

        for(int mc = 0; mc < 3; mc++)
        {
            mchan = &aux_panel->sendchan[mc];
            if(mchan->visible)
            {
                if(mx >= mchan->x && mx <= (mchan->x + mchan->width) && 
                   my >= (mchan->y - 10) && my <= (mchan->y + mchan->height))
                {
                    M.active_mixchannel = mchan;
                }
            }
        }
    }

    mchan = M.active_mixchannel;
    if(mchan != NULL && (my > mchan->ry1 && my < mchan->ry2))
    {
        Eff* eff = mchan->first_eff;
        while(eff != NULL)
        {
            if(mx >= eff->x && mx <= (eff->x + eff->width) && 
               my >= eff->y && my <= (eff->y + eff->height))
            {
                M.active_effect = eff;
                break;
            }
            eff = eff->cnext;
        }

        /* // check for near mouse effects in usual mode - maybe later maybe never
        Eff* oadeff1 = M.dropeff1;
        Eff* oadeff2 = M.dropeff2;
        M.dropeff1 = M.dropeff2 = NULL;
        if(mchan != NULL && mouse_y > mixY && mouse_y <= mchan->ry2)
        {
            Eff* eff = mchan->first_eff;
            while(eff != NULL)
            {
                if(eff->visible)
                {
                    if(mouse_y >= eff->y + eff->h/2 && 
                       (eff->cnext == NULL || mouse_y <= eff->cnext->y + eff->cnext->h/2))
                    {
                        M.dropeff1 = eff;
                    }
                    
					if(mouse_y <= eff->y + eff->h/2 && 
						(eff->cprev == NULL || mouse_y >= eff->cprev->y + eff->cprev->h/2))
                    {
                        M.dropeff2 = eff;
                        if(M.dropeff2 == M.dropeff1)
                        {
                            M.dropeff2 = M.dropeff2->cnext;
                        }
                    }
                }

                if(M.dropeff1 != NULL && M.dropeff2 != NULL)
                {
                    break;
                }

                eff = eff->cnext;
            }
        }*/
    }

    if(M.auxcellindexing)
    {
        R(Refresh_AuxHighlights);
    }
}

void Mouse::CheckGridElements(int mx, int my)
{
    //Samplent* samplent;
    Element* el;
    int lH;
    if(loc == Loc_MainGrid)
    {
        el = firstElem;
        lH = lineHeight;
    }
    else if(loc == Loc_SmallGrid)
    {
        el = aux_panel->workPt->basePattern->first_elem;
        lH = aux_panel->lineHeight;
    }

    // Not allowed resizing elements on stepsequencer currently and in brush mode
    if(!(M.loc == Loc_SmallGrid && aux_panel->workPt->ptype == Patt_StepSeq) && CP->CurrentMode != CP->BrushMode)
    {
        // Check envelopes first as they hide everything
        while(el != NULL)
        {
            if(el->IsPresent() && (el->visible || el->auxvisible))
            {
                if(el->type == El_Command)
                {
                    Command* scomm = (Command*)el;
                    if(scomm->IsEnvelope())
                    {
                        Envelope* env = (Envelope*)scomm->paramedit;
                        if(env->IsMouseInside(mx, my))
                        {
                            if((M.mmode & MOUSE_ENVELOPING && 
                                M.active_env->element->StartTick() > scomm->StartTick()))
                            {
                                // Skip this envelope as it's already covered by another
                            }
                            else
                            {
                                env->Check(mx, my);
                            }
                        }
                    }
                }
            }
            el = el->next;
        }

        if(!(M.mmode & MOUSE_ENVELOPING))
        {
            if(loc == Loc_MainGrid)
            {
                el = firstElem;
                lH = lineHeight;
            }
            else if(loc == Loc_SmallGrid)
            {
                el = aux_panel->workPt->basePattern->first_elem;
                lH = aux_panel->lineHeight;
            }

            while(el != NULL)
            {
                if(el->IsPresent() && 
                    ((loc == Loc_MainGrid && el->visible) || (loc == Loc_SmallGrid && el->auxvisible)))
                {
                    /*if(el->type == El_Samplent)
                    {
                        samplent = (Samplent*)el;
                        if(samplent->wave_visible == true)
                        {
                            if(mx >= samplent->x && mx < samplent->x + samplent->pix_length && abs(my - (samplent->y - samplent->wave_height_pix - lH)) <= 3)
                            {
                                M.mmode |= MOUSE_RESIZE;
                                M.resize_object = Resize_Waveform;
                                M.edge = TOP_Y;
                                M.active_samplent = samplent;
                            }
                        }
                    }
                    else*/ 
                    if(el->type == El_Pattern ||
                       el->type == El_GenNote ||
                       el->type == El_Samplent ||
                       el->type == El_SlideNote ||
                       el->type == El_Mute ||
                       el->type == El_Slider ||
                       el->type == El_Vibrate ||
                       el->type == El_Transpose)
                    {
                        if(M.current_line == el->track_line || (el->type == El_Pattern))
                        {
                            bool resize1 = false;
                            bool resize2 = false;
                            if(abs(mx - Tick2X(el->end_tick, loc)) < 2 && 
                                    (M.current_line == el->track_line || 
                                    (el->type == El_Pattern && ((Pattern*)el)->is_fat &&  M.current_line == el->track_line - 1)))
                            {
                                resize1 = true;
                            }
                            else if(el->type == El_Pattern)
                            {
                                if((mx >= el->abs_area.x1)&&(mx <= el->abs_area.x2)&&
                                   abs(my - el->abs_area.y1) < 2)
                                {
                                    resize2 = true;
                                }
                            }

        					if(resize1 == true)
        					{
        						M.mmode |= MOUSE_RESIZE;
        						M.resize_object = Resize_GridElem;
                                M.resize_loc = M.loc;
        						M.edge = RIGHT_X;
        						M.resize_elem = el;
        						M.movetick = M.snappedTick;
        						M.moveline = M.snappedLine;
        						M.pickloc = M.loc;
        						break;
        					}
                            else if(resize2 == true)
        					{
        						M.mmode |= MOUSE_RESIZE;
        						M.resize_object = Resize_GridElem;
                                M.resize_loc = M.loc;
        						M.edge = TOP_Y;
        						M.resize_elem = el;
        						M.movetick = M.snappedTick;
        						M.moveline = M.snappedLine;
        						M.pickloc = M.loc;
        						break;
                            }
                        }
                    }
                }
                el = el->next;
            }
        }
    }

    if(loc == Loc_SmallGrid && !(M.mmode & MOUSE_ENVELOPING) && !(M.mmode & MOUSE_RESIZE))
    {
        if(abs(mx - Tick2X(aux_panel->workPt->tick_length, loc)) < 2)
        {
            M.mmode |= MOUSE_RESIZE;
            M.resize_object = Resize_AuxPattRange;
            M.edge = RIGHT_X;
            M.resize_loc = M.loc;
        }
    }

    // Lanes check
    if(!(M.mmode & MOUSE_RESIZE))
    {
        int tc;
        Trk* trk;
        int last;
        Pattern* patt;
        if(loc == Loc_MainGrid)
        {
            tc = OffsLine;
            last = (GridY2 - GridY1)/lineHeight + OffsLine;
            patt = field_pattern;
        }
        else if(loc == Loc_SmallGrid)
        {
            tc = aux_panel->OffsLine;
            last = (GridYS2 - GridYS1)/aux_panel->lineHeight + aux_panel->OffsLine;
            patt = aux_panel->workPt->basePattern;
        }

        while(tc <= last)
        {
            trk = GetTrkDataForLine(tc, patt);
			if(trk != NULL)
			{
                M.active_lane = Lane_Usual;
                if(trk->vol_lane.visible == true)
                {
                    if(trk->vol_lane.CheckMouse(mx, my) == true)
                    {
                        M.current_trk = trk;
                        break;
                    }
                }

				if(M.active_lane == Lane_Usual && trk->pan_lane.visible == true)
				{
					if(trk->pan_lane.CheckMouse(mx, my) == true)
					{
						M.current_trk = trk;
						break;
					}
				}
				tc++;
			}
			else
			{
				break;
			}
        }

        /* On-grid patterns
        {
            int tc = 0;
            int last = M.patt->num_lines;
            Trk* trk = M.patt->first_trkdata;
            while(trk != M.patt->bottom_trk)
            {
                M.active_lane = Lane_Usual;
                if(trk->vol_lane.visible == true)
                {
                    if(trk->vol_lane.CheckMouse(mouse_x, mouse_y) == true)
                    {
                        M.current_trk = trk;
                        break;
                    }
                }

                if(M.active_lane == Lane_Usual && trk->pan_lane.visible == true)
                {
                    if(trk->pan_lane.CheckMouse(mouse_x, mouse_y) == true)
                    {
                        M.current_trk = trk;
                        break;
                    }
                }
                trk = trk->next;
            }

            if(M.active_lane == Lane_Usual && M.patt->vol_lane_main.visible)
            {
                if(M.patt->vol_lane_main.CheckMouse(mouse_x, mouse_y) == true)
                {
                    M.current_trk = NULL;
                }
            }
            
            if(M.active_lane == Lane_Usual && M.patt->pan_lane_main.visible)
            {
                if(M.patt->pan_lane_main.CheckMouse(mouse_x, mouse_y) == true)
                {
                    M.current_trk = NULL;
                }
            }
        }*/
    }
}

void Mouse::CheckGridStuff(int mx, int my)
{
    if(M.loc == Loc_MainGrid)
    {
        M.current_line = Y2Line(my, Loc_MainGrid);
        M.current_tick = X2Tick(mx, Loc_MainGrid);
        M.current_track_num = GetTrkNumForLine(M.current_line, M.patt);
    
        if(M.current_track_num >= 0 && M.current_track_num <= 999)
        {
            M.current_trk = GetTrkDataForLine(M.current_line, M.patt);
        }
    }
    else if(M.loc == Loc_SmallGrid)
    {
        M.current_line = Y2Line(my + aux_panel->bottomincr, Loc_SmallGrid);
        M.current_tick = X2Tick(mx, Loc_SmallGrid);
        M.current_trk = GetTrkDataForLine(M.current_line, aux_panel->workPt->basePattern);
    }

    if((M.loc == Loc_MainGrid || M.loc == Loc_SmallGrid))
    {
        CheckGridElements(mx, my);
    }

    if(M.patt != NULL)
    {
        IsMouseOnBunch(mx, my);
    }
}

void Mouse::ClickActiveElement(int mx, int my, bool dbclick, unsigned flags)
{
    Element* el = active_elem;
    C.PosAndCursor2Mouse(mx, my);

    if(el->IsInstance())
    {
        NoteInstance* ii = (NoteInstance*)el;
        ii->instr->SetLastLength(ii->tick_length);
        ii->instr->SetLastVol(ii->loc_vol->val);
        ii->instr->SetLastPan(ii->loc_pan->val);
        if(!(current_instr == ii->instr))
        {
            ChangeCurrentInstrument(ii->instr);
        }

        if(CP->CurrentMode != CP->SlideMode && CP->CurrentMode != CP->BrushMode && 
           ii->ed_note->visible == true)
        {
            C.ExitToDefaultMode();
            SwitchInputMode(CMode_NotePlacing);
        }

        C.slideparent = ii;
    }

    if(el->type == El_Pattern && dbclick == false && Sel_Active && M.selloc == Loc_SmallGrid && M.selmode == Sel_Fragment)
        Selection_Reset();

    if(!(flags & kbd_ctrl))
        Selection_ToggleElements();
    /*
    else if(el->type == El_SlideNote)
    {
        SlideNote* sn = (SlideNote*)el;
        CP->CurrentMode->Refresh();
        CP->CurrentMode->pressed = false;
        CP->SlideMode->pressed = true;
        CP->CurrentMode = CP->SlideMode;
        CP->CurrentMode->Refresh();
        C.mode = CMode_Sliding;
        C.slideparent = sn->parent;
        C.lastslidelength = sn->tick_length;

        R(Refresh_Buttons);
    }
    */
    el->highlighted = false;
    el->Activate();

    // Exclusively for Alex V
    if(flags & kbd_ctrl)
    {
        el->Select();
		if(loc == Loc_MainGrid && aux_panel->isVolsPansMode())
			R(Refresh_Aux);
		else if(loc == Loc_SmallGrid)
			R(Refresh_SubAux);
    }

    if(el->type == El_Pattern && dbclick == false)
    {
		Pattern* pt = (Pattern*)el;
        pt->aliasModeName = true;

        if(aux_panel->playing)
        {
            ResetProcessing(false);
            ResetUnstableElements();
        }

		if(Looping_Active == true && M.looloc == Loc_SmallGrid)
        {
            Looping_Reset();
        }

        aux_panel->EditPattern((Pattern*)el, false);
        JustClickedPattern = true;
    }
}

bool Mouse::IsOnEdge()
{
    if(M.mmode & MOUSE_GRID_X1 || M.mmode & MOUSE_GRID_X2 || M.mmode & MOUSE_GRID_X22 || M.mmode & MOUSE_GRID_Y1 ||
       M.mmode & MOUSE_GRID_Y2 || M.mmode & MOUSE_LOWAUXEDGE ||
      ((M.mmode & MOUSE_ON_GENBROWSER_EDGE) == MOUSE_ON_GENBROWSER_EDGE)||
      ((M.mmode & MOUSE_ON_MIXBROWSER_EDGE) == MOUSE_ON_MIXBROWSER_EDGE))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Mouse::DeleteContextMenu()
{
    if(M.contextmenu != NULL)
    {
        if(M.contextmenu->mtype != Menu_Main)
        {
            DeleteMenu(M.contextmenu);
        }
        else
        {
            M.contextmenu->active = false;
            MenuItem* mi = M.contextmenu->first_item;
            while(mi != NULL)
            {
                mi->active = false;
                mi = mi->inext;
            }

            if(M.contextmenu->drawarea != NULL)
            {
                M.contextmenu->drawarea->DisableRedraw(MC->listen);
            }
        }
        M.contextmenu = NULL;
        M.menu_active = false;
    }
}

