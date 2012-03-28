#include "awful_paramedit.h"
#include "awful_panels.h"
#include "awful_utils_common.h"
#include "awful_preview.h"
#include "awful_cursorandmouse.h"

PEdit::PEdit()
{
    visible = false;
    active = false;
    bypass = true;
    highlighted = false;
    highlightable = true;
    donotactivate = false;
    go = 0;
    prev = NULL;
    next = NULL;
    type = Param_Default;

    element = NULL;
    param = NULL;
    panel = NULL;
    instr = NULL;
    pt = NULL;
    drawarea = NULL;

    this->SetHint("");

    edx = -1;
    edy = -1;
}

PEdit::~PEdit()
{
    Deactivate();
    if(C.curParam == this)
    {
        C.ExitToCurrentMode();
    }
}

bool PEdit::IsPointed(int mx, int my)
{
    if((mx >= x)&&(mx <= (x + edx))&&(my <= y)&&(my >= (y - edy)))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void PEdit::AddParam(Parameter* prm, float* ptrval)
{
    param = prm;
}

void PEdit::Deactivate()
{
	active = false;
    if(element != NULL)
    {
        element->active_param = NULL;
    }

    if(drawarea != NULL)
    {
        drawarea->Disable();
    }
}

void PEdit::Leave()
{
    if(drawarea != NULL)
    {
        drawarea->Change();
    }
}

void PEdit::Activate()
{
	go = 0;
	active = true;
    bypass = false;

    if(element != NULL)
    {
        element->active_param = this;
    }

    if(drawarea != NULL)
    {
        drawarea->Change();
    }
}

void PEdit::Activate(int x, int y, int w, int h, int x1r, int y1r, int x2r, int y2r)
{
	if(drawarea != NULL)
	{
        drawarea->Enable();
        drawarea->SetBounds(x, y, w, h, x1r, y1r, x2r, y2r);
        active = drawarea->isEnabled();
	}
}

void PEdit::UpdateFromParam()
{
    if(param != NULL)
    {
        if(param->type == Param_Pan)
        {
            fvalue = (param->val + 1)*50;
            value = (int)fvalue;
            UpdateStringFromVal();
        }
        else
        {
            fvalue = (param->val*100);
            value = (int)fvalue;
            UpdateStringFromVal();
        }
    }
}

void PEdit::Reinit()
{
    fvalue = (float)value;
}

void PEdit::UpdateStringFromVal()
{
}

Digits::Digits(int val, int cpos, Element* ow)
{
    value = val;
	UpdateStringFromVal();

	curPos = cpos;
    element = ow;
    active = true;
}

void Digits::ProcessChar(char character)
{
    if(character >= 0x30 && character <= 0x39)
    {
        digits[curPos] = character;
		UpdateValFromString();

        if(curPos == 0)
            curPos = 1;
        else if(curPos == 1)
            curPos = 0;
        element->Update();
    }
    else if(character == 0x2d)
    {
        value = 100;
        UpdateStringFromVal();
        element->Update();
    }
    else
    {
        element->ProcessChar(character);
    }
}

void Digits::UpdateStringFromVal()
{
	ParamNum2String(value, digits);
}

void Digits::UpdateValFromString()
{
    char* c = digits;
	value = ParamString2Num(digits);
    fvalue = (float)value;
    while(*c && (value != 100))
    {
        if(*c == 0x2D)
        {
           *c = 0x30;
        }
        c++;
    }
}

void Digits::ProcessKey(unsigned int key, unsigned int flags)
{
    switch(key)
	{
        case key_down:
            value -= 1;
            if(value < 0)
                value = 0;

            fvalue = (float)value;
            UpdateStringFromVal();
            element->Update();
            break;
        case key_up:
            value += 1;
            if(value > 100)
                value = 100;

            fvalue = (float)value;
            UpdateStringFromVal();
            element->Update();
            break;
        case key_left:
            curPos -= 1;
            if(curPos < 0)
            {
                curPos = 0;
                go = -1;
            }
            break;
        case key_right:
            curPos += 1;
            if(curPos > 1)
            {
                curPos = 1;
                go = 1;
            }
            break;
        case key_tab:
            if(flags & kbd_shift)
                go = -1;
            else
                go = 1;
            break;
        default:
            element->ProcessKey(key, flags);
	}
}

void Digits::toLeft()
{
    curPos = 0;
}

void Digits::toRight()
{
    curPos = 1;
}

void Digits::PlaceCursor()
{
    C.type = CType_Underline;
	if(element != NULL)
	{
	    
	}
	else
	{
		CursX0 = element->x;
		CursX = element->cx;
		CursY = y;
	}
}

DigitStr::DigitStr(const char* str)
{
    strcpy(digits, str);
    active = false;
    element = NULL;
    panel = NULL;
    send = NULL;
    trk = NULL;
    mix_prev = mix_next = NULL;
    curPos = 0;
    type = Param_FX;
    queued = false;
    mcell = NULL;
    mchanout = NULL;
    mchanowner = NULL;
    bground = NULL;

    drawarea = new DrawArea((void*)this, Draw_DigitStr);
    MC->AddDrawArea(drawarea);
}

void DigitStr::SetString(const char* str)
{
    strcpy(digits, str);
    DoUpdate();
}

void DigitStr::DoUpdate()
{
    if(panel != NULL)
    {
        panel->Update();
    }

    //if(instr != NULL || trk != NULL)
    {
        aux_panel->CheckFXString(this);
    }

    drawarea->Change();
}

void DigitStr::ProcessChar(char character)
{
    if((curPos == 0 && ((character >= 0x30 && character <= 0x39)||
                         (character >= 0x41 && character <= 0x40 + NUM_MIXER_COLUMNS - 10)||
                         (character >= 0x61 && character <= 0x60 + NUM_MIXER_COLUMNS - 10)||
                         (character == 0x2D))) || 
       (curPos == 1 && ((character >= 0x30 && character <= 0x39)||
                         (character >= 0x41 && character <= 0x40 + NUM_MIXER_ROWS - 10)||
                         (character >= 0x61 && character <= 0x60 + NUM_MIXER_ROWS - 10)||
                         (character == 0x2D))))
    {
        ToUpperCase(&character);
        digits[curPos] = character;

        if(curPos == 0)
            curPos = 1;
        else if(curPos == 1)
            curPos = 0;
    }

    DoUpdate();
}

void DigitStr::ProcessKey(unsigned int key, unsigned int flags)
{
    switch(key)
	{
        case key_down:
            value -= 1;
            if(value < 0)
                value = 0;
            break;
        case key_up:
            value += 1;
            if(value > 100)
                value = 100;
            break;
        case key_left:
            curPos -= 1;
            if(curPos < 0)
            {
                curPos = 0;
                go = -1;
            }
            break;
        case key_right:
            curPos += 1;
            if(curPos > 1)
            {
                curPos = 1;
                go = 1;
            }
            break;
        case key_tab:
            if(flags & kbd_shift)
                go = -1;
            else
                go = 1;
            break;
        case key_delete:
            digits[0] = '-';
            digits[1] = '-';
            curPos = 0;
            break;
        default:
            break;
	}

    DoUpdate();
}

void DigitStr::toLeft()
{
    curPos = 0;
}

void DigitStr::toRight()
{
    curPos = 1;
}

void DigitStr::PlaceCursor()
{
    C.type = CType_None;
}

void DigitStr::Activate()
{
    go = 0;
    bypass = false;

    if(element != NULL)
    {
        element->active_param = this;
    }

    if(drawarea != NULL)
    {
        drawarea->Change();
    }

    if(send != NULL)
    {
        MC->refresh |= Refresh_Mixer;
    }

    curPos = 0;
}

void DigitStr::Activate(int x, int y, int w, int h, int x1r, int y1r, int x2r, int y2r)
{
	if(drawarea != NULL)
	{
        drawarea->Enable();
        drawarea->SetBounds(x, y, w, h, x1r, y1r, x2r, y2r);
        active = drawarea->isEnabled();

        drawarea->Change();

    	go = 0;
        bypass = false;

        if(element != NULL)
        {
            element->active_param = this;
        }

        if(send != NULL)
        {
            MC->refresh |= Refresh_Mixer;
        }
	}
}

void DigitStr::Leave()
{
    if(drawarea != NULL)
    {
        drawarea->Change();
    }

    if(send != NULL)
    {
        MC->refresh |= Refresh_Mixer;
    }
}

void DigitStr::Click(int mouse_x, int mouse_y, bool dbclick, unsigned flags)
{
    xs = mouse_x;
    ys = mouse_y;
    donotactivate = false;
    oldval = ParamString2Num(digits);
    if(oldval == 100)
        oldval = 0;
    if(dbclick == true)
    {
        if(aux_panel->auxmode != AuxMode_Mixer)
        {
            CP->HandleButtDown(CP->view_mixer);
        }

        if(mchanout != NULL && mchanout != &aux_panel->masterchan)
        {
            aux_panel->mix_sbar->full_len = float(NUM_MIXCHANNELS*(MixChanWidth + 1) + 14 + 3*(MixChanWidth + 1));
            aux_panel->mix_sbar->visible_len = float(mixW - MixChanWidth - 10 - 1);

            aux_panel->mix_sbar->offset = float(mchanout->col*MixChanWidth) - (aux_panel->mix_sbar->visible_len/2 - MixChanWidth/2);
            if(aux_panel->mix_sbar->offset < 0)
            {
                aux_panel->mix_sbar->offset = 0;
            }
            else if(aux_panel->mix_sbar->offset + aux_panel->mix_sbar->visible_len > aux_panel->mix_sbar->full_len)
            {
                aux_panel->mix_sbar->offset = aux_panel->mix_sbar->full_len - aux_panel->mix_sbar->visible_len;
            }
            aux_panel->mix_sbar->actual_offset = aux_panel->mix_sbar->offset;
            UpdateScrollbarOutput(aux_panel->mix_sbar);
        }

        // Old mixer stuff
        /*
        if(MainX2 > WindWidth - WindWidth/2 + 25)
        {
            MixerWidth = WindWidth/2 + 25;
            MainX2 = WindWidth - MixerWidth - 1;
            MixW = WindWidth - (MainX2 + MixCenterWidth+ 12) + 1;
            Grid2Main();
            R(Refresh_All);
        }

        if(mcell != NULL && mcell != &mix->m_cell)
        {
            mix->current_mixcell = mcell;
            R(Refresh_MixHighlights);

            int xoffs = mcell->col*MixCellWidth;
            if(xoffs - mix->horiz_bar->actual_offset > MixW - MixCellWidth || 
               xoffs - mix->horiz_bar->actual_offset < 0)
            {
                mix->horiz_bar->offset = mix->horiz_bar->actual_offset = (float)(xoffs - MixW/2 + MixCellWidth/2);
                if(mix->horiz_bar->offset < 0)
                    mix->horiz_bar->offset = mix->horiz_bar->actual_offset = 0;
                R(Refresh_Mixer);
            }
        }
        */
    }
}

Percent::Percent(int val, int cpos, Element* ow, ParamType pt)
{
    value = val;
    fvalue = (float)val;
    memset(digits, 0, 6);
	UpdateStringFromVal();
    type = pt;

    digit_width = 5;
	curPos = cpos;
    maxPos = cpos;
    element = ow;
}

void Percent::ProcessChar(char character)
{
    if((character >= 0x30)&&(character <= 0x39))
    {
        if(curPos < 3)
        {
            digits[curPos] = character;
			if(curPos == maxPos)
            {
				maxPos++;
            }
			curPos++;

            UpdateValFromString();
        }
    }
    else if(character == 0x2d)
    {
        SetVal(100);
    }
    else
    {
        element->ProcessChar(character);
    }
}

void Percent::ProcessKey(unsigned int key, unsigned int flags)
{
    switch(key)
	{
        case key_down:
            AdjustValue(-1);
            curPos = maxPos;
            break;
        case key_up:
            AdjustValue(+1);
            curPos = maxPos;
            break;
        case key_left:
            curPos -= 1;
            if(curPos < 0)
            {
                curPos = 0;
                go = -1;
            }
            break;
        case key_right:
            curPos += 1;
            if(curPos > maxPos)
            {
                curPos = maxPos;
                go = 1;
            }
            break;
        case key_backspace:
            if(curPos > 0)
            {
                digits[curPos-1] = 0;
                curPos--;
                maxPos--;

                if(curPos < maxPos)
                {
    				PackZeros();
                }
				UpdateValFromString();
			}
            break;
	}
    element->Update();
}

void Percent::PackZeros()
{
    int i = 0, grab = 0;
	char *c = digits;

	while(i <= maxPos)
	{
		if(*c == 0)
			grab++;

	   *c = *(c + grab);
		c++;
		i++;
	}
}

void Percent::toLeft()
{
    curPos = 0;
}

void Percent::toRight()
{
    curPos = maxPos;
}

void Percent::PlaceCursor()
{
    C.type = CType_Vertical;
    CursX = x + digit_width*curPos - (1 && curPos);
    CursY = y;
}

void Percent::UpdateStringFromVal()
{
	Num2String(value, digits);
    int sl = strlen(digits);
    digits[sl] = 0;

    maxPos = sl;
}

void Percent::UpdateValFromString()
{
	value = (int)ParamString2Num(digits);
    fvalue = (float)value;
	if(value > 100)
	{
        SetVal(100);
	}
    else
    {
        if(param != NULL)
        {
            param->UpdateFromParamedit();
        }

        if(element != NULL)
        {
            element->Update();
        }
    }
}

void Percent::SetVal(int val)
{
    value = val;
    fvalue = (float)val;
    UpdateStringFromVal();
    if(element != NULL)
    {
        element->Update();
    }

	if(param != NULL)
    {
        param->UpdateFromParamedit();
    }
}

void Percent::AdjustValue(int delta)
{
    value += delta;
    UpdateStringFromVal();
    if(value < 0)
    {
        SetVal(0);
    }
    if(value > 100)
    {
        SetVal(100);
    }
    
	if(param != NULL)
    {
        param->UpdateFromParamedit();
    }
    fvalue = (float)value;
}

Note::Note(short val, Element* ow)
{
    type = Param_Note;
    SetValue(val);
    element = ow;
    relative = false;
	lastchar = 'q';
}

void Note::PlaceCursor()
{
    if(element->visible || element->auxvisible)
    {
        C.type = CType_Underline;
        CursX0 = x;
        CursX = x + pixwidth;
        CursY = y;
    }
    else
    {
        C.type = CType_None;
    }
}

void Note::ProcessKey(unsigned int key, unsigned int flags)
{
    element->ProcessKey(key, flags);
}

void Note::ProcessChar(char character)
{
    if(character == 0x2F)  // slash key enables sliding
    {
        element->ProcessChar(character);

        if(C.loc == Loc_MainGrid)
        {
            R(Refresh_GridContent);
        }
        else if(C.loc == Loc_SmallGrid)
        {
            R(Refresh_AuxContent);
        }
    }
    else    // handle note-bound key
    {
        int oldval = value;
        int n = MapKeyToNote(character, relative);
        if(n != -1000)
        {
            SetValue(n);
            element->UpdateNote();
        }

        lastchar = character;
        //Process_AutoAdvance();

        if(value != oldval)
        {
            if(C.loc == Loc_MainGrid)
            {
                R(Refresh_GridContent);
            }
            else if(C.loc == Loc_SmallGrid)
            {
                R(Refresh_AuxContent);
            }
        }
    }
}

void Note::Kreview(int key)
{
    Instance* ii = NULL;
    if(element->type == El_Samplent || 
       element->type == El_GenNote)
    {
        ii = (Instance*)element;
    }
    else if(element->type == El_SlideNote)
    {
        ii = (Instance*)((SlideNote*)element)->parent;
    }

    if(ii != NULL)
    {
        Preview_Add(ii, NULL, key, value, C.patt, NULL, NULL, false, relative);
        C.last_char = lastchar;
        C.last_note = value;
    }
	else
	{
        Preview_Add(NULL, current_instr, key, value, C.patt, NULL, NULL, true, relative);
        C.last_char = lastchar;
        C.last_note = value;
	}
}

void Note::SetValue(int newvalue)
{
    value = newvalue;
    freq = NoteToFreq(newvalue);
}

void Note::SetRelative(bool rel)
{
    if(relative == false && rel == true)
    {
        value -= baseNote;
    }
    else if(relative == true && rel == false)
    {
        value += baseNote;
    }
    freq = NoteToFreq(value);
    relative = rel;
}

void Note::Hide()
{
    visible = false;
}

void Note::Show()
{
    visible = true;
}

Semitones::Semitones(int kolichestvo, Element* owna)
{
    element = owna;
    type = Param_Semitones;
    value = kolichestvo;
    UpdateStringFromVal();
}

void Semitones::ProcessKey(unsigned int key, unsigned int flags)
{
    if(key == key_up)
    {
        value++;

        if(value == 0)
            value = 1;
        else if(value > 48)
            value = 48;
        UpdateStringFromVal();
    }
    else if(key == key_down)
    {
        value--;

        if(value == 0)
            value = -1;
        else if(value < -48)
            value = -48;
        UpdateStringFromVal();
    }
}

void Semitones::UpdateStringFromVal()
{
    char  cs[3];
	ParamNum2String(abs(value), cs);

    if(value > 0)
    {
        str[0] = '+';
    }
    else if(value < 0)
    {
        str[0] = '-';
    }

    char* f = str; 
    f++;
    memcpy(f, cs, 3);
    str[3] = 0;
}

void Semitones::SetValue(int val)
{
    value = val;
    UpdateStringFromVal();
}

void EnvTimeClick(Object* owner, Object* self)
{
	Toggle* tg = (Toggle*)self;
    Envelope* env = (Envelope*)owner;
	if(env->timebased == true)
	{
		env->Scale(0.125);
	}
	else
	{
		env->Scale(8);
	}
}

Envelope::Envelope(CmdType ct)
{
    type = Param_Env;
    ctype = ct;
    p_first = NULL;
    p_last = NULL;
    susPoint = NULL;
    memset(buff, 0, MAX_BUFF_SIZE*sizeof(float));
    last_value = prev_value = 0;
    aaBaseValue = 0;
    aaCount = 0;
    ticks = 0;
    len = 15;
    hgt = 40;
    folded = false;
    group_prev = group_next = NULL;
    fold = new Toggle(Toggle_EnvFold, &folded);

    AddNewControl(fold, NULL);
    if(ct == Cmd_LocVolEnv || ct == Cmd_LocPanEnv)
    {
        timebased = true;
        timetg = new Toggle(Toggle_EnvTime, &timebased);
        timetg->owner = (Object*)this;
        timetg->SetOnClickHandler(&EnvTimeClick);
        AddNewControl(timetg, NULL);
    }
    else
    {
        timebased = false;
        timetg = NULL;
    }
    SetSustainable(false);
    newbuff = false;
}

Envelope::~Envelope()
{
    RemoveControlCommon(fold);
    if(timetg != NULL)
        RemoveControlCommon(timetg);
}

Envelope* Envelope::Clone(Element* el)
{
    Envelope* clone = Clone();
    clone->element = el;

    return clone;
}

Envelope* Envelope::Clone()
{
    Envelope* clone = new Envelope(ctype);
    clone->hgt = hgt;
    clone->len = len;
    clone->folded = folded;
    clone->timebased = timebased;
    clone->sustainable = sustainable;

    EnvPnt* epc;
    EnvPnt* ep = p_first;
    while(ep != NULL)
    {
        if(ep->deleted == false)
        {
            epc = clone->AddPoint(ep->x, ep->y_norm, ep->big);
            if(ep == susPoint)
            {
                clone->SetSustainPoint(epc);
            }
        }
        ep = ep->next;
    }

    return clone;
}

EnvPnt* Envelope::LocatePoint(EnvPnt* p)
{
    if(p->x < p_first->x)
    {
        p->next = p_first;
        p_first->prev = p;
        p_first = p; 
        p->prev = NULL;
    }
    else
    {
        EnvPnt* cp = p_first;
        while(cp->next != NULL)
        {
            if(p->x < cp->next->x)
                break;

            cp = cp->next;
        }

        p->next = cp->next;

        if(cp->next != NULL)
        {
            cp->next->prev = p;
        }
        else if(cp->next == NULL)
        {
            p_last = p;
        }

        cp->next = p;
        p->prev = cp;
    }

    CalcTime();
    return p;
}

void Envelope::CalcTime()
{
    if(p_last != NULL && p_first != NULL)
    {
        ticks = p_last->x - p_first->x;
        if(timebased == false)
        {
            frame_length = long(ticks/one_divided_per_frames_per_tick);
        }
        else
        {
            frame_length = long(ticks/one_divided_per_sample_rate);
        }
    }
}

void Envelope::RelocatePoint(EnvPnt* p)
{
    if((p->next == NULL || p->x <= p->next->x) && 
       (p->prev == NULL || p->x >= p->prev->x))
    {
        // no need to relocate
    }
    else
    {
        RemovePoint(p);
        LocatePoint(p);
    }
    UpdateLastValue();
}

void Envelope::RemovePoint(EnvPnt* p)
{
	if((p == p_first)&&(p == p_last))
	{
		p_first = p_last = NULL;
	}
	else if(p == p_first)
	{
		p_first = p->next;
		p_first->prev = NULL;
	}
	else if(p == p_last)
	{
		p_last = p->prev;
		p_last->next = NULL;
	}
	else
	{
		p->prev->next = p->next;
		p->next->prev = p->prev;
	}

    p->prev = p->next = NULL;
}

void Envelope::DeletePoint(EnvPnt* p)
{
    // TODO: cleanup deletion
    if(p == susPoint)
    {
        if(p->next != NULL)
        {
            SetSustainPoint(p->next);
        }
        else
        {
            SetSustainPoint(p->prev);
        }
    }

    RemovePoint(p);

    p->deleted = true;
    //delete p;

    CalcTime();
    UpdateLastValue();
}

EnvPnt* Envelope::AddPoint(float x, float y_norm, bool big)
{
    EnvPnt* rp;
    EnvPnt* p = new EnvPnt;
    p->x = x;
    p->y_norm = y_norm;
    p->deleted = false;
    p->suspoint = false;
    p->big = big;

    rp = p;
    if(p_last == NULL)
    {
        p->prev = NULL;
        p->next = NULL;
        p_first = p;
        p_last = p;
    }
    else
    {
        rp = LocatePoint(p);
    }

    UpdateLastValue();
    return rp;
}

float Envelope::GetValue(float tick)
{
    EnvPnt* ep = p_first;
    float val = ep->y_norm;
    if(ep->next != NULL)
    {
        float xval = tick;
        while(xval > ep->next->x)
        {
            ep = ep->next;
            if(ep->next == NULL)
            {
                val = last_value;
                break;
            }
        }

        if(ep->next != NULL && xval <= ep->next->x)
        {
            if(xval > ep->x && xval < ep->next->x)
            {
                val = Interpolate_Line(ep->x, ep->y_norm, ep->next->x, ep->next->y_norm, xval);
            }
            else if(xval == ep->x)
            {
                val = ep->y_norm;
            }
            else if(xval == ep->next->x)
            {
                val = ep->next->y_norm;
            }
        }
    }
    return val;
}

float Envelope::GetValue(tframe frame)
{
    float xval;
    if(timebased == false)
    {
        xval = one_divided_per_frames_per_tick*frame;
    }
    else
    {
        xval = one_divided_per_sample_rate*frame;
    }

    return GetValue(xval);
}

bool Envelope::ProcessBuffer1(long curr_frame, 
                                long buffframe,
                                long num_frames, 
                                int step, 
                                long start_frame, 
                                Trigger* tgenv, 
                                Trigger* tgi)
{
	last_buffframe = buffframe;
    Command* cmd = (Command*)element;

    //float* single_buffer;
    //float* single_buffer_ov;
    //single_buffer = buff;
    //single_buffer_ov = buffov;
/*
    Command* cmd1 = NULL;
    if(cmd->param != NULL)
    {
        cmd1 = (Command*)cmd->param->envelopes->el;
    }

    if(cmd == cmd1 || cmd1 == NULL)
    {
        single_buffer = buff;
        single_buffer_ov = buffov;
    }
    else if(cmd1 != NULL)
    {
        single_buffer = ((Envelope*)cmd1->paramedit)->buff;
        single_buffer_ov = ((Envelope*)cmd1->paramedit)->buffov;
    }
*/

    float fstep;
    float xval;
    long fPhase;
    long fPhaseCeiling;
    if(tgenv != NULL)
    {
        fPhase = tgenv->frame_phase;
        fPhaseCeiling = tgenv->frame_phase + num_frames;
    }
    else
    {
        fPhase = long(curr_frame - start_frame);
    }

    if(timebased == false)
    {
        fstep = one_divided_per_frames_per_tick*step;
        xval = one_divided_per_frames_per_tick*fPhase;
    }
    else
    {
        fstep = one_divided_per_sample_rate*step;
        xval = one_divided_per_sample_rate*fPhase;
    }

    EnvPnt* ep = p_first;
    float val = ep->y_norm;
    float useval = 1;
    int cntdown = 1;
    long i = 0;
    while(i < num_frames && i < MAX_BUFF_SIZE && xval < len)
    {
        if(ep->next != NULL)
        {
            cntdown--;
            if(cntdown == 0)
            {
                while(xval > ep->next->x)
                {
                    ep = ep->next;
                    if(ep->next == NULL)
                    {
                        val = last_value;
                        break;
                    }
                }

                if(ep->next != NULL && xval <= ep->next->x)
                {
                    if(xval > ep->x && xval < ep->next->x)
                    {
                        val = Interpolate_Line(ep->x, ep->y_norm, ep->next->x, ep->next->y_norm, xval);
                    }
                    else if(xval == ep->x)
                    {
                        val = ep->y_norm;
                    }
                    else if(xval == ep->next->x)
                    {
                        val = ep->next->y_norm;
                    }
                }

                cntdown = step;
                xval += fstep;
                if(tgenv != NULL)
                {
                    tgenv->frame_phase += step;
                    if(tgi != NULL)
                    {
                        if(sustainable == true && tgi->tgstate == TgState_Sustain && susPoint != NULL && xval > susPoint->x)
                        {
                            xval = susPoint->x;
                            if(timebased == false)
                            {
                                tgenv->frame_phase = long(xval/one_divided_per_frames_per_tick);
                            }
                            else
                            {
                                tgenv->frame_phase = long(xval/one_divided_per_sample_rate);
                            }
                        }
                    }
                }
            }
        }

        useval = val;

        if(ctype == Cmd_VolEnv || ctype == Cmd_PanEnv || ctype == Cmd_ParamEnv)
        {
            if(tgenv != NULL)
            {
                if(/*curr_frame == start_frame && i == 0 && */tgenv->prev_value == -123.0f)
                {
                    tgenv->prev_value = val;
                }
                else
                {
                    if(tgenv->aaCount > 0)
                    {
                        tgenv->cf1 = (float)tgenv->aaCount/DECLICK_COUNT;
                        tgenv->cf2 = 1.0f - tgenv->cf1;
                        useval = tgenv->aaBaseVal*tgenv->cf1 + val*tgenv->cf2;
                        tgenv->aaCount--;
                    }
                    else if(fabs(val - tgenv->prev_value) > 0.01f)
                    {
                        useval = tgenv->prev_value;
                        tgenv->aaBaseVal = tgenv->prev_value;
                        tgenv->aaCount = DECLICK_COUNT;
                        tgenv->cf1 = 1;
                        tgenv->cf2 = 0;
                    }
                    tgenv->prev_value = useval;
                }
            }
            else
            {
                if(curr_frame == start_frame && i == 0)
                {
                    tgi->prev_value = val;
                }
                else
                {
                    if(aaCount > 0)
                    {
                        cf1 = (float)aaCount/DECLICK_COUNT;
                        cf2 = 1.0f - cf1;
                        useval = aaBaseValue*cf1 + val*cf2;
                        aaCount--;
                    }
                    else if(fabs(val - tgi->prev_value) > 0.15f)
                    {
                        useval = tgi->prev_value;
                        aaBaseValue = tgi->prev_value;
                        aaCount = DECLICK_COUNT;
                        cf1 = 1;
                        cf2 = 0;
                    }
                    tgi->prev_value = useval;
                }
            }
        }

        // Check for shit
        if(val < 0 || val > 1)
            val = 0;
        if(useval < 0 || useval > 1)
            useval = 0;

        buff[buffframe + i] = val;
        if(ctype == Cmd_VolEnv || ctype == Cmd_LocVolEnv)
        {
            buffoutval[buffframe + i] = GetVolOutput(useval*VolRange);
        }
        else if(ctype == Cmd_PanEnv || ctype == Cmd_LocPanEnv)
        {
            buffoutval[buffframe + i] = (useval*2) - 1;
        }
        else
        {
            buffoutval[buffframe + i] = useval;
        }

        i++;
    }	
    last_buffframe_end = buffframe + i;

    // Update the parameter directly from here if required
    if(num_frames > 0 && cmd->param != NULL && cmd->param->envdirect)
    {
        param = cmd->param;
        param->SetValueFromEnvelope(useval, this);
        param->SetLastVal(param->outval);
    }

    // Trigger phase can overcome number of frames, but it shouldn't
    if(tgenv != NULL && tgenv->frame_phase > fPhaseCeiling)
    {
        tgenv->frame_phase = fPhaseCeiling;
    }

    newbuff = true;
    return true;
}

void Envelope::UpdateLastValue()
{
    if(p_last != NULL)
    {
        if(p_last->x <= len)
            last_value = p_last->y_norm;
        else
        {
            EnvPnt* ep = p_first;
            while(ep != NULL && ep->next != NULL)
            {
                if(ep->next->x > len)
                {
                    last_value = Interpolate_Line(ep->x, ep->y_norm, ep->next->x, ep->next->y_norm, len);
                    break;
                }
                ep = ep->next;
            }
        }
    }
    else
    {
        if(ctype == Cmd_VolEnv || ctype == Cmd_LocVolEnv)
        {
            last_value = 1;
        }
        else if(ctype == Cmd_PanEnv || ctype == Cmd_LocPanEnv)
        {
            last_value = 0;
        }
    }

    EnvPnt* ep = p_first;
    while(ep != NULL)
    {
        if(ep->next != NULL)
        {
            ep->cff = (ep->next->y_norm - ep->y_norm)/(ep->next->x - ep->x)*one_divided_per_sample_rate;
        }
        else
        {
            ep->cff = 0;
        }
        ep = ep->next;
    }
}

float Envelope::GetScale(Loc loc)
{
    switch(ctype)
    {
        case Cmd_LocVolEnv:
        case Cmd_LocPanEnv:
            if(timebased)
                return 96.0f;
        default:
            if(loc == Loc_MainGrid)
            {
                return (float)tickWidth;
            }
            else if(loc == Loc_SmallGrid)
            {
                return (float)aux_panel->tickWidth;
            }
            else if(loc == Loc_StaticGrid)
            {
                return (float)st_tickWidth;
            }
            else
            {
                return (float)tickWidth;
            }
    }
}

void Envelope::DeleteRange(float x_curr, float x_start, bool start_except)
{
    if(x_curr >= x_start)
    {
        EnvPnt* epd;
        EnvPnt* ep = p_first->next;
        while(ep != NULL)
        {
            if(ep->x >= x_start && ep->x <= x_curr && !(start_except && ep->x == x_start))
            {
                epd = ep;
                ep = ep->next;
                DeletePoint(epd);
                continue;
            }
            ep = ep->next;
        }
    }
    else if(x_curr < x_start)
    {
        EnvPnt* epd;
        EnvPnt* ep = p_first->next;
        while(ep != NULL)
        {
            if(ep->x < x_start && ep->x >= x_curr)
            {
                epd = ep;
                ep = ep->next;
                DeletePoint(epd);
                continue;
            }
            ep = ep->next;
        }
    }
}

void Envelope::Drag(int mouse_x, int mouse_y, unsigned flags)
{
    int py = y + hgt - mouse_y;
    if(py < 0)
        py = 0;
    if(py > hgt)
        py = hgt;

    float yval = (float)py/hgt;
    if(flags & kbd_alt)
    {
        switch(ctype)
        {
            case Cmd_VolEnv:
            case Cmd_LocVolEnv:
                yval = 1.0f/VolRange;
                break;
            case Cmd_PanEnv:
            case Cmd_LocPanEnv:
            case Cmd_PitchEnv:
                yval = 0.5f;
                break;
        }
    }

    int px;
    if(M.v_only == true)
    {
        px = 0;
    }
    else
    {
        px = mouse_x - x;
    
        if(px < 0)
            px = 0;
        if(px > edx)
            px = edx;
    }
    float env_xcurr = (float)px/scale;
    
    if(M.env_action == ENVPOINTING)
    {
        float oldx = M.active_pnt->x;
        M.active_pnt->x = (float)px/scale;
        M.active_pnt->y_norm = yval;
        if(M.active_pnt != p_first)
        {
            if(M.active_pnt->prev != NULL && M.active_pnt->x < M.active_pnt->prev->x)
            {
                M.active_pnt->x = M.active_pnt->prev->x;
            }
            else if(M.active_pnt->next != NULL && M.active_pnt->x > M.active_pnt->next->x)
            {
                M.active_pnt->x = M.active_pnt->next->x;
            }
        }

        if(flags & kbd_shift && M.active_pnt->next != NULL)
        {
            float dx = M.active_pnt->x - oldx;
            EnvPnt* ep = M.active_pnt->next;
            while(ep != NULL)
            {
                ep->x += dx;
                ep = ep->next;
            }
        }
        UpdateLastValue();

        //if(M.v_only == false)
        //{
        //    env->RelocatePoint(M.active_pnt);
        //}
    }
    else if(M.env_action == ENVDRAWMOVE)
    {
        DeleteRange(env_xcurr, M.env_xstart, true);
    
        if(px > 0)
        {
            AddPoint((float)px/scale, yval);
        }
        else
        {
            p_first->x = (float)px/scale;
            p_first->y_norm = (float)py/hgt;
        }
        M.env_xstart = env_xcurr;
    }
    else if(M.env_action == ENVDELETING)
    {
        if(px > 0)
        {
            DeleteRange(env_xcurr, M.env_xstart, false);
        }
        else
        {
            p_first->x = (float)px/scale;
            p_first->y_norm = (float)py/hgt;
        }
    }
    else if(M.env_action == ENVLINEMOVE)
    {
        DeleteRange(env_xcurr, M.env_xstart, true);
    
        if(px > 0)
        {
            AddPoint((float)px/scale, yval);
        }
        else
        {
            p_first->x = (float)px/scale;
            p_first->y_norm = (float)py/hgt;
        }
    }
    else if(M.env_action == ENVSUSTAINING)
    {
        EnvPnt* p = p_first;
        EnvPnt* pl = p;
        while(p != NULL)
        {
            if(abs(p->x - env_xcurr) < abs(pl->x - env_xcurr))
            {
                pl = p;
            }
            p = p->next;
        }
        SetSustainPoint(pl);
    }
    CalcTime();
    element->Update();
}

bool Envelope::IsMouseInside(int mouse_x, int mouse_y)
{
    if((mouse_x >= x)&&(mouse_x <= (x + edx))&&(mouse_y >= y - 12)&&(mouse_y <= (y + hgt)))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Envelope::Check(int mouse_x, int mouse_y)
{
    bool waspointing = false;
    bool wassustaining = false;
    if(M.env_action == ENVPOINTING)
    {
        waspointing = true;
    }
    if(M.env_action == ENVSUSTAINING)
    {
        wassustaining = true;
    }

    M.v_only = false;
    M.active_pnt = NULL;
    M.env_action = ENVUSUAL;

    if(folded == false)
    {
        if(IsMouseInside(mouse_x, mouse_y))
        {
            if(M.mmode & MOUSE_RESIZE && M.resize_object == Resize_Envelope)
                M.mmode &= ~MOUSE_RESIZE;

            M.mmode |= MOUSE_ENVELOPING;
            M.env_action = ENVUSUAL;
            M.active_env = this;

            EnvPnt* p = p_first;
            while(p != NULL)
            {
                if(fabs(mouse_x - (x + p->x*scale)) <= ENVPOINT_TOUCH_RADIUS &&
                    abs(mouse_y - (y + hgt - p->y_norm*(float)hgt)) <= ENVPOINT_TOUCH_RADIUS)
                {
                    M.env_action = ENVPOINTING;
                    M.active_pnt = p;

                    if(M.loc == Loc_MainGrid)
                    {
                        R(Refresh_GridContent);
                    }
                    else if(M.loc == Loc_SmallGrid)
                    {
                        R(Refresh_AuxContent);
                    }
                    else if(M.mmode & MOUSE_AUXAUXING)
                    {
                        R(Refresh_SubAux);
                    }

                    if(p == p_first)
                        M.v_only = true;
                    else
                        M.v_only = false;
                    break;
                }
                else if(p == susPoint && abs(M.mouse_x - (x + p->x*scale)) <= 3)
                {
                    M.env_action = ENVSUSTAINING;

                    if(M.loc == Loc_MainGrid)
                    {
                        R(Refresh_GridContent);
                    }
                    else if(M.loc == Loc_SmallGrid)
                    {
                        R(Refresh_AuxContent);
                    }
                    else if(M.mmode & MOUSE_AUXAUXING)
                    {
                        R(Refresh_SubAux);
                    }
                }
                p = p->next;
            }
        }

        if(M.active_pnt == NULL)
        {
        	if((abs(mouse_x - (x + edx)) <= 3)&&(mouse_y >= y - 12)&&(mouse_y <= (y + hgt)))
            {
                M.mmode |= MOUSE_RESIZE;
                M.resize_object = Resize_Envelope;
                M.edge = RIGHT_X;
                M.active_env = this;
            }
            else if((abs(mouse_y - (y + hgt)) <= 2)&&(mouse_x >= x)&&(mouse_x <= x + edx))
            {
                M.mmode |= MOUSE_RESIZE;
                M.resize_object = Resize_Envelope;
                M.edge = BOTTOM_Y;
                M.active_env = this;
            }
        }
    }
    else
    {
    	if((abs(mouse_x - (x + edx)) <= 3)&&(mouse_y <= y)&&(mouse_y >= (y - 12)))
        {
            M.mmode |= MOUSE_RESIZE;
            M.resize_object = Resize_Envelope;
            M.edge = RIGHT_X;
            M.active_env = this;
        }
    }

    if((waspointing == true || wassustaining == true) && M.env_action == ENVUSUAL)
    {
        if(M.loc == Loc_MainGrid)
        {
            R(Refresh_GridContent);
        }
        else if(M.loc == Loc_SmallGrid)
        {
            R(Refresh_AuxContent);
        }
        else if(M.mmode & MOUSE_AUXAUXING)
        {
            R(Refresh_SubAux);
        }
    }
}

void Envelope::Click(int mouse_x, int mouse_y, unsigned flags)
{
    if(M.LMB == true && mouse_y >= y)
    {
        int px = mouse_x - x;
        int py = y + hgt - mouse_y;
        float yval = (float)py/hgt;

        if(flags & kbd_alt)
        {
            switch(ctype)
            {
                case Cmd_VolEnv:
                case Cmd_LocVolEnv:
                    yval = 1.0f/VolRange;
                    break;
                case Cmd_PanEnv:
                case Cmd_LocPanEnv:
                case Cmd_PitchEnv:
                    yval = 0.5f;
                    break;
            }
        }

        if(flags & kbd_ctrl)
        {
            M.env_action = ENVDRAWMOVE;
            M.env_xstart = (float)px/scale;
        }
        else if(M.env_action != ENVPOINTING && M.env_action != ENVSUSTAINING)
        {
            M.active_pnt = AddPoint((float)px/scale, yval);
            M.env_action = ENVPOINTING;
            M.v_only = false;
        }
        else if(M.env_action == ENVPOINTING)
        {
            M.active_pnt->big = true;
        }
        CalcTime();
        element->Update();
    }
    else if(M.RMB == true)
    {
        int px = mouse_x - x;
        int py = y + hgt - mouse_y;

        if(!(flags && kbd_ctrl))
        {
            if(M.env_action & ENVPOINTING)
            {
                if(M.active_pnt != p_first)
                {
                    DeletePoint(M.active_pnt);
                }
            }
            M.env_action = ENVDELETING;
            M.env_xstart = (float)px/scale;
        }
        else
        {
            if(M.env_action & ENVPOINTING)
            {
            }
            else
            {
                M.active_pnt = AddPoint((float)px/scale, (float)py/hgt);
            }
            M.env_action = ENVLINEMOVE;
            M.env_xstart = (float)px/scale;
        }
    }
}

void Envelope::SetSustainable(bool sust)
{
    sustainable = sust;
}

void Envelope::SetSustainPoint(EnvPnt * pnt)
{
    if(susPoint != NULL)
    {
        susPoint->suspoint = false;
    }
    susPoint = pnt;
    if(susPoint != NULL)
    {
        susPoint->suspoint = true;
    }
}

// Returns number of frames remaining between envelope length and passed trigger's phase
long Envelope::GetFramesRemaining(long frame_phase)
{
    return frame_length - frame_phase;
}

bool Envelope::IsOutOfBounds(long frame_phase)
{
    if(frame_phase < 0 || frame_phase > frame_length)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Envelope::Clean()
{
    EnvPnt* pnext;
    EnvPnt* p = p_first;
    while(p != NULL)
    {
        pnext = p->next;
        DeletePoint(p);
        p = pnext;
    }

    SetSustainPoint(NULL);
}

void Envelope::InitAAEnv(bool offenv)
{
    Clean();
    timebased = true;
    sustainable = false;
    if(offenv)
    {
        AddPoint(0, 1/VolRange);
        AddPoint(0.001f, 0);
        AddPoint(1.0f, 0);
    }
    else
    {
        AddPoint(0, 0);
        AddPoint(0.001f, 1/VolRange);
        AddPoint(1.0f, 1/VolRange);
    }
}

void Envelope::Scale(float s)
{
    len *= s;
    EnvPnt* p = p_first;
    while(p != NULL)
    {
        p->x *= s;
        p = p->next;
    }
	CalcTime();
}

void Envelope::SetLength(float newlen)
{
    len = newlen;
    UpdateLastValue();
    if(element != NULL)
        element->Update();
}

void Envelope::Simplify()
{
    float llen;
    EnvPnt* ep = p_first->next;
    while(ep != NULL && ep->next != NULL)
    {
        llen = sqrt(pow((ep->next->x - ep->x), 2) + pow((ep->next->y_norm - ep->y_norm), 2));
        if(llen < 0.5f)
        {
            ep->y_norm = Interpolate_Line(ep->x, ep->y_norm, ep->next->x, ep->next->y_norm, ep->x + (ep->next->x - ep->x)/2.f);
            ep->x = ep->x + (ep->next->x - ep->x)/2.f;
            RemovePoint(ep->next);
        }

        ep = ep->next;
    }
}

XmlElement* Envelope::Save(char* name)
{
    XmlElement * xmlEnvData = new XmlElement(name);
    xmlEnvData->setAttribute(T("Len"), len);
    xmlEnvData->setAttribute(T("Hgt"), hgt);
    xmlEnvData->setAttribute(T("sustainable"), sustainable ? 1 : 0);
    xmlEnvData->setAttribute(T("folded"), folded ? 1 : 0);
    XmlElement * xmlEnvPoint;
    EnvPnt* ep = p_first;
    while(ep != NULL)
    {
        xmlEnvPoint = new XmlElement(T("EnvPoint"));
        xmlEnvPoint->setAttribute(T("xval"), (double)ep->x);
        xmlEnvPoint->setAttribute(T("yval"), (double)ep->y_norm);
        xmlEnvPoint->setAttribute(T("sustainpoint"), ep->suspoint);
        xmlEnvPoint->setAttribute(T("big"), ep->big ? 1 : 0);

        xmlEnvData->addChildElement(xmlEnvPoint);

        ep = ep->next;
    }

    return xmlEnvData;
}

void Envelope::Load(XmlElement * xmlEnvNode)
{
    len = (float)xmlEnvNode->getDoubleAttribute(T("Len"), len);
    hgt = xmlEnvNode->getIntAttribute(T("Hgt"), hgt);
    sustainable = xmlEnvNode->getBoolAttribute(T("sustainable"), true);
    folded = xmlEnvNode->getBoolAttribute(T("folded"), false);
    Clean();
    XmlElement*  xmlChildNode = xmlEnvNode->getFirstChildElement();
    while(xmlChildNode != NULL)
    {
        if(xmlChildNode->hasTagName(T("EnvPoint")) == true)
        {
            EnvPnt* ep = new EnvPnt;
            ep->x = (float)xmlChildNode->getDoubleAttribute(T("xval"));
            ep->y_norm = (float)xmlChildNode->getDoubleAttribute(T("yval"));
            ep->suspoint = xmlChildNode->getBoolAttribute(T("sustainpoint"));
            ep->big = xmlChildNode->getBoolAttribute(T("big"));

            ep->deleted = false;

            if(p_last == NULL)
            {
                ep->prev = NULL;
                ep->next = NULL;
                p_first = ep;
                p_last = ep;
            }
            else
            {
                LocatePoint(ep);
            }

            if(ep->suspoint == true)
            {
                SetSustainPoint(ep);
            }
        }

        xmlChildNode = xmlChildNode->getNextElement();
    }

    UpdateLastValue();
}

TString::TString(char* name, bool spc_allowed, Element* ow)
{
    type = Param_TString;
    memset(string, 0, MAX_NAME_STRING);

	if(name != NULL)
    {
		strcpy(string, name);
    }

    if(name == NULL)
        curPos = 0;
    else
        curPos = strlen(name);

    active = true;
    maxPos = curPos;
    maxlen = 100;
    space_allowed = spc_allowed;
    element = ow;
	panel = NULL;
    instr = NULL;

    drawarea = new DrawArea((void*)this, Draw_TString);
    MC->AddDrawArea(drawarea);
}

TString::TString(char* name, bool spc_allowed, Panel* pn)
{
    type = Param_TString;
    memset(string, 0, MAX_NAME_STRING);

	if(name != NULL)
    {
		strcpy(string, name);
    }

    if(name == NULL)
        curPos = 0;
    else
        curPos = strlen(name);

    active = true;
    maxPos = curPos;
    maxlen = 100;
    space_allowed = spc_allowed;
    element = NULL;
	panel = pn;
    instr = NULL;

    drawarea = new DrawArea((void*)this, Draw_TString);
    MC->AddDrawArea(drawarea);
}


TString::TString(char* name, bool spc_allowed, Instrument* inst)
{
    type = Param_TString;
    memset(string, 0, MAX_NAME_STRING);

	if(name != NULL)
    {
		strcpy(string, name);
    }

    if(name == NULL)
        curPos = 0;
    else
        curPos = strlen(name);

    active = true;
    maxPos = curPos;
    maxlen = 100;
    space_allowed = spc_allowed;
    element = NULL;
	panel = NULL;
    instr = inst;

    drawarea = new DrawArea((void*)this, Draw_TString);
    MC->AddDrawArea(drawarea);
}

TString::TString(char* name, bool spc_allowed)
{
    type = Param_TString;
    memset(string, 0, MAX_NAME_STRING);

	if(name != NULL)
    {
		strcpy(string, name);
    }

    if(name == NULL)
        curPos = 0;
    else
        curPos = strlen(name);

    active = true;
    maxPos = curPos;
    maxlen = 100;
    space_allowed = spc_allowed;
    element = NULL;
	panel = NULL;
    instr = NULL;

    drawarea = new DrawArea((void*)this, Draw_TString);
    MC->AddDrawArea(drawarea);
}

TString::~TString()
{
    Deactivate();
    if(C.curParam == this)
    {
        C.ExitToCurrentMode();
    }
}

void TString::ProcessChar(char character)
{
    if(active)
    {
        // Esli eto bukva
        if((curPos < MAX_NAME_STRING)&&(((character >= 0x41)&&(character <= 0x5A))||   // ABCD...
           ((character >= 0x61)&&(character <= 0x7A))||(character == 0x21)||   // abcd...
           ((character >= 0x30)&&(character <= 0x39))||   // 12345...
           (character == 0x5F)||    // Underline
           ((space_allowed == true)&&(character == 0x20))))   // space
        {
            if(curPos != maxPos || maxPos < maxlen)
            {
                string[curPos] = character;

        		if(curPos == maxPos)
        		{
        			maxPos++;
        		}
        		curPos++;

                if(pt != NULL)
                {
                    pt->ParameditUpdate(this);
                }

                if(element != NULL)
                {
                    element->Update();
                }

                if(panel != NULL)
                {
                    panel->Update();
                }

                if(instr != NULL)
                {
                    instr->ParamEditUpdate(this);
                }
            }
        }
        else if(element != NULL)
        {
            element->ProcessChar(character);
        }
    }

    ChangesIndicate();
}

void TString::ProcessKey(unsigned int key, unsigned int flags)
{
    switch(key)
	{
        case key_left:
            curPos -= 1;
            if(curPos < 0)
            {
                curPos = 0;
                go = -1;
            }break;
        case key_right:
            curPos += 1;
            if(curPos > maxPos)
            {
                curPos = maxPos;
                go = 1;
            }break;
        case key_backspace:
            if(curPos > 0)
            {
                string[curPos - 1] = 0;
                curPos--;
                maxPos--;
                if(curPos < maxPos)
				    PackZeros();
			}break;
        case key_delete:
            if(element != NULL)
            {
                element->ProcessKey(key, flags);
            }break;
	}

    if(pt != NULL)
    {
        pt->ParameditUpdate(this);
    }

    if(element != NULL)
    {
        element->Update();
    }

    if(panel != NULL)
    {
        panel->Update();
    }

    if(instr != NULL)
    {
        instr->ParamEditUpdate(this);
    }

    ChangesIndicate();
}

void TString::PackZeros()
{
    int i = 0, grab = 0;
	char *c = string;

	while(i <= maxPos)
	{
		if(*c == 0)
			grab++;

	   *c = *(c + grab);
		c++;
		i++;
	}
}

void TString::toLeft()
{
    curPos = 0;
}

void TString::toRight()
{
    curPos = maxPos;
}

void TString::PlaceCursor()
{
    if(active && (element == NULL || (element->visible || element->auxvisible)))
    {
        C.type = CType_Vertical;
        CursX = x + cposx; // - (1 && curPos);
        CursY = y;
        if(element == NULL && CursX > (x + edx))
        {
            C.type = CType_None;
        }
        CursX0 = CursX;
    }
    else
    {
        C.type = CType_None;
    }
}

void TString::SetString(char* name)
{
    memset(string, 0, MAX_NAME_STRING);
    if(name != NULL)
    {
        strcpy(string, name);
        curPos = strlen(name);
    }
    else
    {
        curPos = 0;
    }
}

void TString::CapFirst()
{
    if(strlen(string) > 0 && ((string[0] >= 0x61)&&(string[0] <= 0x7A)))
    {
        string[0] -= 0x20;
    }
}

