#include "awful.h"
#include "awful_elements.h"
#include "awful_paramedit.h"
#include "awful_controls.h"
#include "awful_audio.h"
#include "awful_effects.h"
#include "awful_instruments.h"
#include "VSTCollection.h"
#include "awful_panels.h"
#include "awful_utils_common.h"
#include "awful_events_triggers.h"
#ifdef USE_OLD_JUCE
#include "juce_amalgamated.h"
#else
#include "juce_amalgamated_NewestMerged.h"
#endif
#include "awful_cursorandmouse.h"

Control::Control()
{
    x = y = -1;
    width = height = 0;

    param = NULL;
    panel = NULL;
    prev = NULL;
    next = NULL;
    pn_prev = NULL;
    pn_next = NULL;
    prm_prev = NULL;
    prm_next = NULL;
    lst_prev = NULL;
    lst_next = NULL;
    patt = NULL;
    drawarea = NULL;
    elem = NULL;
    active = true;
    anchored = false;
    anchorcount = 0;
    coverable = true;

    scope = NULL;
    instr = NULL;

    memset(this->hint, 0, sizeof(this->hint));
    memset(this->title, 0, sizeof(this->title));
    memset(this->valstring, 0, sizeof(this->valstring));
    this->SetTitle("");
    this->SetHint("");
    this->enabled = true;
    this->on_click_func = NULL;
}

Control::~Control()
{
	if(drawarea != NULL)
		MC->RemoveDrawArea(drawarea);
}

bool Control::MouseCheck(int mouse_x, int mouse_y)
{
    if(drawarea != NULL)
    {
        if((mouse_x >= drawarea->ax)&&(mouse_x <= (drawarea->ax + drawarea->aw))&&
            (mouse_y >= drawarea->ay)&&(mouse_y <= drawarea->ay + drawarea->ah))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else 
    {
        if((mouse_x >= x)&&(mouse_x <= (x + width))&&(mouse_y >= y)&&(mouse_y <= y + height))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

void Control::AddParam(Parameter* p)
{
    param = p;
}

void Control::Deactivate()
{
    active = false;

	if(drawarea != NULL)
	{
		drawarea->Disable();
	}
}

void Control::Activate()
{
	active = true;

	if(drawarea != NULL)
	{
	    drawarea->Enable();
	}
}

void Control::Activate(int x, int y, int w, int h, int x1r, int y1r, int x2r, int y2r)
{
	if(drawarea != NULL)
	{
        drawarea->Enable();
        drawarea->SetBounds(x, y, w, h, x1r, y1r, x2r, y2r);
        active = drawarea->isEnabled();
	}
}

void Control::Activate(int x, int y, int w, int h)
{
	if(drawarea != NULL)
	{
        drawarea->Enable();
        drawarea->SetBounds(x, y, w, h);
        active = drawarea->isEnabled();
	}
}

void Control::UpToDrawarea()
{
	if(drawarea->isEnabled())
	{
	    Activate();
        x = drawarea->ax;
        y = drawarea->ay;
        width = drawarea->aw;
        height = drawarea->ah;
	}
    else
    {
        Deactivate();
    }
}

bool Control::isVolumeSlider()
{
    return(type == Ctrl_Slider && (((SliderBase*)this)->sltype == Slider_Default ||
                                    ((SliderBase*)this)->sltype == Slider_InstrVol));
}

bool Control::isPanningSlider()
{
    return(type == Ctrl_Slider && (((SliderBase*)this)->sltype == Slider_InstrPan));
}

ScrollBard::ScrollBard()
{
    type = Ctrl_HScrollBar;
    barpixoffs = actual_barpixoffs = 0;
    barpixlen = 0;
    pixlen = 0;
}

ScrollBard::ScrollBard(ControlType bartype, float totallen, float visiblelen, int tolstota)
{
    type = bartype;
    tolschina = tolstota;

    active = false;
    full_len = totallen;
    visible_len = visiblelen;
    offset = actual_offset = 0;
    fixed = false;
    discrete = false;
    barpixoffs = actual_barpixoffs = 0;
    barpixlen = 0;
    pixlen = 0;
}

void ScrollBard::AddDrawarea()
{
    drawarea = new DrawArea((void*)this, Draw_ScrollBar);
    MC->AddDrawArea(drawarea);
}

bool ScrollBard::MouseCheck(int mouse_x, int mouse_y)
{
    if(type == Ctrl_HScrollBar)
    {
        if(mouse_x >= x1 && mouse_x <= x2 && mouse_y >= y && mouse_y <= (y + height))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else if(type == Ctrl_VScrollBar)
    {
        if(mouse_y >= y1 && mouse_y <= y2 && mouse_x >= x && mouse_x <= (x + width))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
	return false;
}

void ScrollBard::TweakByWheel(int delta, int mouse_x, int mouse_y)
{
    int cdelta;
    if(type == Ctrl_HScrollBar)
    {
        cdelta = delta;
    }
    else if(type == Ctrl_VScrollBar)
    {
        cdelta = -delta;
    }

    //SetPixDelta(delta, mouse_x, mouse_y);
    SetDelta((float)cdelta);

/*
    if(discrete == false && (barpixoffs + barpixlen + cdelta) > pixlen - 2)
    {
        cdelta = pixlen - (barpixoffs + barpixlen) - 2;
    }

    if(discrete == false)
    {
        if((barpixoffs + cdelta) < 0)
        {
            barpixoffs = 0;
            offset = 0;
        }
        else
        {
            float dtf = (float)cdelta/cf;
            offset += dtf;
            if((barpixoffs + barpixlen + cdelta) == pixlen - 2)
            {
                offset = full_len - visible_len;
            }
        }
    }
    else
    {
        if((barpixoffs + cdelta) < 0)
        {
            barpixoffs = 0;
            offset = 0;
        }
        else
        {
			offset += cdelta;
			if(offset < 0 || (int)offset == 0)
			{
				offset = 0;
			}
			else if(offset + visible_len > full_len)
			{
				offset = full_len - visible_len;
			}
		}
    }

    actual_offset = offset;
    UpdateScrollbarOutput(this);
*/
}

void ScrollBard::SetDelta(float delta)
{
    SetOffset(offset + delta);
}

void ScrollBard::SetOffset(float offs)
{
    offset = offs;
    if(offset < 0)
    {
        offset = 0;
    }
    else if(offset + visible_len > full_len && 
                                               this != main_bar && 
                                               this != aux_panel->h_sbar)
    {
        offset = full_len - visible_len;
        if(offset < 0)
            offset = 0; // This can happen with main bar where visible len can be larger than full len
    }

    if(discrete == true)
    {
        actual_offset = (float)(int)offset;

        float barlen = float(barpixlen)/cf;
        if(actual_offset + barlen > full_len && full_len >= barlen)
        {
            actual_offset = offset = full_len - barlen;
        }
    }

    actual_offset = offset;

    UpdateScrollbarOutput(this);
}

void ScrollBard::SetPixDelta(int delta, int mouse_x, int mouse_y)
{
    int pixdelta = delta;
    if(type == Ctrl_HScrollBar)
    {
        if((barpixoffs + barpixlen + pixdelta) > pixlen - 2)
        {
            pixdelta = pixlen - (barpixoffs + barpixlen) - 2;
        }
        else if(barpixoffs + pixdelta < 0)
        {
            pixdelta = 0 - barpixoffs;
        }

		float dtf = (float)pixdelta/cf;
        SetDelta(dtf);

        if(discrete == false)
        {
            /*
			float dtf = (float)pixdelta/cf;
			offset += dtf;
			if(offset < 0)
			{
				offset = 0;
			}
            */

            if(fixed == false)
            {
            /*
                if(barpixoffs + barpixlen + pixdelta == pixlen - 2)
                {
                    offset = actual_offset = full_len - visible_len;
                    UpdateScrollbarOutput(this);
                }
            */
            }
            else if(offset > full_len)
            {
                offset = actual_offset = full_len - barpixlen/cf;
                UpdateScrollbarOutput(this);
            }
        }
        else
        {
            //offset = (barpixoffs + pixdelta)/(pixlen/full_len);
        }

        xs += pixdelta;

        //actual_offset = offset;
        //UpdateScrollbarOutput(this);
    }
    else if(type == Ctrl_VScrollBar)
    {
        int step = (int)(pixlen/full_len);
        int pixdelta = delta;
        float offscut = 0;

        float dtf = (float)pixdelta/cf;
        //SetDelta(dtf);
        offset += dtf;
        if(offset + visible_len > full_len)
        {
            offscut = (offset + visible_len) - full_len;
            offset = full_len - visible_len;
        }
        else if(offset < 0)
        {
            offscut = offset;
            offset = 0;
        }
        ys += (pixdelta - RoundFloat(offscut*cf));

        if(discrete == false)
        {
            actual_offset = offset;
        }
        else if(discrete == true)
        {
            actual_offset = (float)(int)offset;

            float barlen = float(barpixlen)/cf;
            if(actual_offset + barlen > full_len)
            {
                actual_offset = offset = full_len - barlen;
                ys -= (pixdelta - RoundFloat(offscut*cf));
            }
        }
        UpdateScrollbarOutput(this);
    }
}

void ScrollBard::MouseClick(int mouse_x, int mouse_y)
{
    if(type == Ctrl_HScrollBar)
    {
        if(mouse_x < x1 + barpixoffs || mouse_x > x1 + barpixoffs + barpixlen)
        {
            int pixdelta = mouse_x - (x1 + barpixoffs + barpixlen/2);
            if((barpixoffs + barpixlen + pixdelta) > pixlen - 2)
            {
                pixdelta = pixlen - (barpixoffs + barpixlen) - 2;
            }

            float dtf = (float)pixdelta/cf;
            SetDelta(dtf);
        }
    }
    else if(type == Ctrl_VScrollBar)
    {
        if(mouse_y < y1 + barpixoffs || mouse_y > y1 + barpixoffs + barpixlen)
        {
            int pixdelta = mouse_y - (y1 + barpixoffs + barpixlen/2);
            if(discrete == false)
            {
                if((barpixoffs + barpixlen + pixdelta) > pixlen - 2)
                {
                    pixdelta = pixlen - (barpixoffs + barpixlen) - 2;
                }
            }

            if(aux_panel->workPt->ptype == Patt_Pianoroll && this == aux_panel->v_sbar && aux_panel->offset_correction == true)
            {
                aux_panel->offset_correction = false;
                aux_panel->bottomincr = 0;
                //sb->actual_offset = (float)(int)sb->actual_offset + 1;
            }
            float dtf = (float)pixdelta/cf;
            SetDelta(dtf);
        }
    }

    UpdateScrollbarOutput(this);
}

SliderBase::SliderBase()
{
    type = Ctrl_Slider;
    sltype = Slider_Default;
    height = length = 73;
    width = 14;
    anchorval = 1;

    active = true;
    panel = NULL;
    scope = NULL;

    drawarea = NULL;
}

SliderBase::SliderBase(Panel* ow, Scope* sc)
{
    type = Ctrl_Slider;
    sltype = Slider_Default;
    height = length = 80;
    width = 14;
    anchorval = 1;

    active = true;
    panel = ow;
    scope = sc;

    drawarea = new DrawArea((void*)this, Draw_VertSlider);
    MC->AddDrawArea(drawarea);

    vu = new VU(VU_Big);
}

/*
bool SliderBase::MouseCheck(int mouse_x, int mouse_y)
{
    if((mouse_x >= x)&&(mouse_x <= (x + width))&&(mouse_y >= y - height)&&(mouse_y <= y))
    {
        return true;
    }
    else
    {
        return false;
    }
    return false;
}
*/

void SliderBase::UpdateValFromPos()
{
    if(param != NULL)
    {
        if(anchored == true && pos == anchorpos)
        {
            param->SetDirectValueFromControl(anchorval);
        }
        else
        {
            param->SetValueFromControl((float)pos/height);
        }
    }
}

void SliderBase::UpdatePosFromVal()
{
    if(param != NULL)
    {
        pos = RoundFloat((float)height*((param->val - param->offset)/param->range));
        if(drawarea != NULL)
        {
            drawarea->Change();
        }
    }
}

void SliderBase::SetAnchor(float val)
{
    anchored = true;
    if(param != NULL)
    {
        anchorval = val;
        anchorpos = RoundFloat((float)height*((val - param->offset)/param->range));
    }
}

void SliderBase::TweakByWheel(int delta, int mouse_x, int mouse_y)
{
    pos += delta;
    if(pos < 0)
        pos = 0;
    if(pos > length)
        pos = length;

    UpdateValFromPos();

    if(panel != NULL)
    {
        panel->HandleTweak(this, xs, ys);
    }

    UpdateAnchorSign();
    if(isVolumeSlider() || isPanningSlider())
        UpdateControlHint(this, mouse_x, mouse_y);
}

void SliderBase::TweakByMouseMove(int mouse_x, int mouse_y)
{
    if(anchorcount == 0)
    {
        int p_xs = xs, p_ys = ys;
        /*
        int dx = 0, dy = 0;
        dy = (mouse_y - ys);
        if((pos - dy) > height)
        {
            dy = pos - height;
        }
        else if((pos - dy) < 0)
        {
            dy = pos;
        }
        pos -= dy;
        ys += dy;
       */

        pos = y - mouse_y;
        if(pos > height)
        {
            pos = height;
        }
        else if(pos < 0)
        {
            pos = 0;
        }

        if(anchored)
        {
            bool newsign = pos > anchorpos ? true : false;
            if(newsign != anchorsign)
            {
                pos = anchorpos;
                anchorsign = newsign;
                anchorcount = 0;
            }
        }

        UpdateValFromPos();

        if(panel != NULL)
        {
            panel->HandleTweak(this, p_xs, p_ys);
        }
    }
    else
    {
        int dx = 0, dy = 0;
        int p_xs = xs, p_ys = ys;

        dy = (mouse_y - ys);
        
        if((pos - dy) > height)
        {
            dy = pos - height;
        }
        else if((pos - dy) < 0)
        {
            dy = pos;
        }
        ys += dy;

        anchorcount--;

        if(anchorcount > 0)
        {
            Desktop::setMousePosition(anchormx, anchormy);
        }
    }
}

void SliderBase::MouseClick(int mouse_x, int mouse_y)
{
    pos = y - mouse_y - 1;
    if(pos < 0)
    {
        pos = 0;
    }
    else if(pos > length)
    {
        pos = length;
    }

    this->UpdateValFromPos();

    if(anchored)
    {
        anchorcount = 0;
        UpdateAnchorSign();
    }
}

void SliderBase::UpdateAnchorSign()
{
    anchorsign = pos > anchorpos ? true : false;
}

SliderZoom::SliderZoom(Panel* ow, Scope* sc)
{
    type = Ctrl_Slider;
    sltype = Slider_Zoom;
    length = 70;
    width = length + 1;
    height = 21;
    pos = 0;
    step = RoundFloat(float(length)/18);

    active = true;
    panel = ow;
    scope = sc;

    drawarea = new DrawArea((void*)this, Draw_ZoomSlider);
    MC->AddDrawArea(drawarea);

    vu = NULL;
}

void SliderZoom::UpdateValFromPos()
{
    if(param != NULL)
    {
        param->SetValueFromControl((float)pos/height);
    }
}

void SliderZoom::UpdatePosFromVal()
{
    if(param != NULL)
    {
        pos = RoundFloat((float)height*((param->val - param->offset)/param->range));
        if(drawarea != NULL)
        {
            drawarea->Change();
        }
    }
}

void SliderZoom::TweakByWheel(int delta, int mouse_x, int mouse_y)
{
    pos += delta*3;
    if(pos < 3)
        pos = 3;
    if(pos > length)
        pos = length;

    UpdateValFromPos();

    if(panel != NULL)
    {
        panel->HandleTweak(this, xs, ys);
    }

    if(drawarea != NULL)
    {
        drawarea->Change();
    }
}

void SliderZoom::TweakByMouseMove(int mouse_x, int mouse_y)
{
    int dx = 0, dy = 0;
    int p_xs = xs, p_ys = ys;

    dx = (mouse_x - xs);
    
    if((pos + dx) > length)
    {
        dx = length - pos;
    }
    else if((pos + dx) < step)
    {
        dx = step - pos;
    }

    pos += dx;

    UpdateValFromPos();
    xs += dx;

    if(panel != NULL)
    {
        panel->HandleTweak(this, p_xs, p_ys);
    }

    if(drawarea != NULL)
    {
        drawarea->Change();
    }
}

void SliderZoom::MouseClick(int mouse_x, int mouse_y)
{
    pos = mouse_x - x;
    if(pos < step)
    {
        pos = step;
    }
    else if(pos > length)
    {
        pos = length;
    }

    if(panel != NULL)
    {
        panel->HandleTweak(this, xs, ys);
    }

    if(drawarea != NULL)
    {
        drawarea->Change();
    }
}

float SliderZoom::GetTickWidth()
{
    int los = (pos/5)*5;

    if(los <= step)
        return 0.5f;
    else if(los <= step*2)
        return 0.75f;
    else if(los <= step*3)
        return 1.f;
    else if(los <= step*4)
        return 1.5f;
    else if(los <= step*5)
        return 2.f;
    else if(los <= step*6)
        return 3.f;
    else if(los <= step*7)
        return 4.f;
    else if(los <= step*8)
        return 5.f;
    else if(los <= step*9)
        return 6.f;
    else if(los <= step*10)
        return 8.f;
    else if(los <= step*11)
        return 10.f;
    else if(los <= step*12)
        return 12.f;
    else if(los <= step*13)
        return 16.f;
    else if(los <= step*14)
        return 20.f;
    else if(los <= step*15)
        return 24.f;
    else if(los <= step*16)
        return 28.f;
    else
        return 32.f;
}

void SliderZoom::SetTickWidth(float tW)
{
    if(tW <= 0.5f)
        pos = step;
    else if(tW <= 0.75f)
        pos = step*2;
    else if(tW <= 1.f)
        pos = step*3;
    else if(tW <= 1.5f)
        pos = step*4;
    else if(tW <= 2.f)
        pos = step*5;
    else if(tW <= 3.f)
        pos = step*6;
    else if(tW <= 4.f)
        pos = step*7;
    else if(tW <= 5.f)
        pos = step*8;
    else if(tW <= 6.f)
        pos = step*9;
    else if(tW <= 8.f)
        pos = step*10;
    else if(tW <= 10.f)
        pos = step*11;
    else if(tW <= 12.f)
        pos = step*12;
    else if(tW <= 16.f)
        pos = step*13;
    else if(tW <= 20.f)
        pos = step*14;
    else if(tW <= 24.f)
        pos = step*15;
    else if(tW <= 28.f)
        pos = step*16;
    else if(tW < 32.f)
        pos = step*17;
    else
    {
        pos = length;
    }
    drawarea->Change();
}

SliderHVol::SliderHVol(Panel* ow, Scope* sc)
{
    type = Ctrl_Slider;
    sltype = Slider_InstrVol;
    length = width = 62;

    active = true;
    panel = ow;
    scope = sc;

    drawarea = new DrawArea((void*)this, Draw_SliderHVol);
    MC->AddDrawArea(drawarea);
}

SliderHVol::SliderHVol(int l, Panel* ow, Scope* sc)
{
    type = Ctrl_Slider;
    sltype = Slider_InstrVol;
    length = width = l;

    active = true;
    panel = ow;
    scope = sc;

    drawarea = new DrawArea((void*)this, Draw_SliderHVol);
    MC->AddDrawArea(drawarea);
}

void SliderHVol::UpdateValFromPos()
{
    if(param != NULL)
    {
        if(anchored == true && pos == anchorpos)
        {
            param->SetDirectValueFromControl(anchorval);
        }
        else
        {
            param->SetValueFromControl((float)pos/width);
        }
    }
}

void SliderHVol::UpdatePosFromVal()
{
    if(param != NULL)
    {
        pos = RoundFloat((float)width*((param->val - param->offset)/param->range));
        if(drawarea != NULL)
        {
            drawarea->Change();
        }
    }
}

void SliderHVol::SetAnchor(float val)
{
    anchored = true;
    if(param != NULL)
    {
        anchorpos = RoundFloat((float)width*((val - param->offset)/param->range));
    }
}

void SliderHVol::TweakByMouseMove(int mouse_x, int mouse_y)
{
    if(anchorcount == 0)
    {
        int p_xs = xs, p_ys = ys;
/*
        int dx = 0, dy = 0;
        dx = (mouse_x - xs);
        if((pos + dx) > width)
        {
            dx = width - pos;
        }
        else if((pos + dx) < 0)
        {
            dx = 0 - pos;
        }

        pos += dx;
        xs += dx;
*/

        pos = mouse_x - x;
        if(pos > width)
        {
            pos = width;
        }
        else if(pos < 0)
        {
            pos = 0;
        }

        if(anchored)
        {
            bool newsign = pos > anchorpos ? true : false;
            if(newsign != anchorsign)
            {
                pos = anchorpos;
                anchorsign = newsign;
                anchorcount = 0;
            }
        }

        UpdateValFromPos();

        if(panel != NULL)
        {
            panel->HandleTweak(this, p_xs, p_ys);
        }
    }
    else
    {
        int dx = 0, dy = 0;
        int p_xs = xs, p_ys = ys;

        dx = (mouse_x - xs);
        
        if((pos + dx) > width)
        {
            dx = width - pos;
        }
        else if((pos + dx) < 0)
        {
            dx = 0 - pos;
        }

        xs += dx;

        anchorcount--;
    }
}

void SliderHVol::MouseClick(int mouse_x, int mouse_y)
{
    pos = mouse_x - x - 1;
    if(pos < 0)
    {
        pos = 0;
    }
    else if(pos > length)
    {
        pos = length;
    }

    this->UpdateValFromPos();

    if(anchored)
    {
        anchorcount = 0;
        UpdateAnchorSign();
    }
}

void SliderHVol::UpdateAnchorSign()
{
    anchorsign = pos > anchorpos ? true : false;
}

SliderHPan::SliderHPan(Panel* ow, Scope* sc)
{
    type = Ctrl_Slider;
    sltype = Slider_InstrPan;
    length = width = 62;
    anchorval = 0;

    active = true;
    panel = ow;
    scope = sc;

    drawarea = new DrawArea((void*)this, Draw_SliderHPan);
    MC->AddDrawArea(drawarea);
}

SliderHPan::SliderHPan(int l, Panel* ow, Scope* sc)
{
    type = Ctrl_Slider;
    sltype = Slider_InstrPan;
    length = width = l;
    anchorval = 0;

    active = true;
    panel = ow;
    scope = sc;

    drawarea = new DrawArea((void*)this, Draw_SliderHPan);
    MC->AddDrawArea(drawarea);
}

void SliderHPan::UpdateValFromPos()
{
    if(param != NULL)
    {
        param->SetValueFromControl((float)pos/width);
    }
}

void SliderHPan::UpdatePosFromVal()
{
    if(param != NULL)
    {
        pos = RoundFloat((float)width*((param->val - param->offset)/param->range));
        if(drawarea != NULL)
        {
            drawarea->Change();
        }
    }
}

void SliderHPan::SetAnchor(float val)
{
    anchored = true;
    if(param != NULL)
    {
        anchorpos = RoundFloat((float)width*((val - param->offset)/param->range));
    }
}

void SliderHPan::TweakByMouseMove(int mouse_x, int mouse_y)
{
    if(anchorcount == 0)
    {
        int p_xs = xs, p_ys = ys;
/*
        int dx = 0, dy = 0;
        dx = (mouse_x - xs);
        if((pos + dx) > width)
        {
            dx = width - pos;
        }
        else if((pos + dx) < 0)
        {
            dx = 0 - pos;
        }

        pos += dx;
        xs += dx;
*/

        pos = mouse_x - x;
        if(pos > width)
        {
            pos = width;
        }
        else if(pos < 0)
        {
            pos = 0;
        }

        if(anchored)
        {
            bool newsign = pos > anchorpos ? true : false;
            if(newsign != anchorsign)
            {
                pos = anchorpos;
                anchorsign = newsign;
                anchorcount = 0;
            }
        }

        UpdateValFromPos();

        if(panel != NULL)
        {
            panel->HandleTweak(this, p_xs, p_ys);
        }
    }
    else
    {
        int dx = 0, dy = 0;
        int p_xs = xs, p_ys = ys;

        dx = (mouse_x - xs);
        
        if((pos + dx) > width)
        {
            dx = width - pos;
        }
        else if((pos + dx) < 0)
        {
            dx = 0 - pos;
        }

        xs += dx;

        anchorcount--;
    }
}

void SliderHPan::MouseClick(int mouse_x, int mouse_y)
{
    pos = mouse_x - x;
    if(pos < 0)
    {
        pos = 0;
    }
    else if(pos > length)
    {
        pos = length;
    }

    this->UpdateValFromPos();

    if(anchored)
    {
        anchorcount = 0;
        UpdateAnchorSign();
    }
}

void SliderHPan::UpdateAnchorSign()
{
    anchorsign = pos > anchorpos ? true : false;
}

SliderParam::SliderParam(int len, Panel* ow, Scope* sc, Paramcell* pd)
{
    type = Ctrl_Slider;
    sltype = Slider_Param;
    length = width = len;
    pdata = pd;
    interval = 0;
    active = true;
    panel = ow;
    scope = sc;

    if(pdata == NULL)
    {
        drawarea = new DrawArea((void*)this, Draw_Param);
        MC->AddDrawArea(drawarea);
    }
}

void SliderParam::AddParam(Parameter* p)
{
    Control::AddParam(p);
    if(param->interval > 0)
        interval = param->interval;
}

void SliderParam::UpdateValFromPos()
{
    if(param != NULL)
    {
        float value = (float)pos/width;
        //if(interval > 0)
        //    value = float(int(value))*interval;
        param->SetValueFromControl(value);
    }
}

void SliderParam::UpdatePosFromVal()
{
    if(param != NULL)
    {
        pos = RoundFloat((float)width*((param->val - param->offset)/param->range));
    }

    if(pdata != NULL)
    {
        pdata->drawarea->Change();
    }
}

void SliderParam::SetAnchor(float val)
{
    anchored = true;
    if(param != NULL)
    {
        anchorpos = RoundFloat((float)width*((val - param->offset)/param->range));
    }
}

void SliderParam::TweakByWheel(int delta, int mouse_x, int mouse_y)
{
    pos += delta;
    if(pos < 0)
        pos = 0;
    if(pos > length)
        pos = length;

    UpdateValFromPos();

    if(panel != NULL)
    {
        panel->HandleTweak(this, xs, ys);
    }

    UpdateAnchorSign();
}

void SliderParam::TweakByMouseMove(int mouse_x, int mouse_y)
{
    if(anchorcount == 0)
    {
        int p_xs = xs, p_ys = ys;
/*
        int dx = 0, dy = 0;
        dx = (mouse_x - xs);
        if((pos + dx) > width)
        {
            dx = width - pos;
        }
        else if((pos + dx) < 0)
        {
            dx = 0 - pos;
        }

        pos += dx;
        xs += dx;
*/

        pos = mouse_x - x;
        if(pos > width)
        {
            pos = width;
        }
        else if(pos < 0)
        {
            pos = 0;
        }

        if(anchored)
        {
            bool newsign = pos > anchorpos ? true : false;
            if(newsign != anchorsign)
            {
                pos = anchorpos;
                anchorsign = newsign;
                anchorcount = 0;
            }
        }

        UpdateValFromPos();

        if(panel != NULL)
        {
            panel->HandleTweak(this, p_xs, p_ys);
        }
    }
    else
    {
        int dx = 0, dy = 0;
        int p_xs = xs, p_ys = ys;

        dx = (mouse_x - xs);
        
        if((pos + dx) > width)
        {
            dx = width - pos;
        }
        else if((pos + dx) < 0)
        {
            dx = 0 - pos;
        }

        xs += dx;

        anchorcount--;
    }
}

void SliderParam::MouseClick(int mouse_x, int mouse_y)
{
    pos = mouse_x - x;
    if(pos < 0)
    {
        pos = 0;
    }
    else if(pos > length)
    {
        pos = length;
    }

    this->UpdateValFromPos();

    if(anchored)
    {
        anchorcount = 0;
        UpdateAnchorSign();
    }
}

void SliderParam::UpdateAnchorSign()
{
    anchorsign = pos > anchorpos ? true : false;
}

bool SliderParam::MouseCheck(int mouse_x, int mouse_y)
{
    if(pdata == NULL)
    {
        return Control::MouseCheck(mouse_x, mouse_y);
    }
    else
    {
        if((mouse_x >= pdata->drawarea->ax)&&(mouse_x <= (pdata->drawarea->ax + pdata->drawarea->aw))&&
            (mouse_y >= pdata->drawarea->ay)&&(mouse_y <= pdata->drawarea->ay + pdata->drawarea->ah))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

Knob::Knob(int r, Parameter* par, Panel* ow, Scope* sc, KnobType kt)
{
    type = Ctrl_Knob;
    ktype = kt;

    rad = r;
    x = -200;
    y = -200;
    param = par;
    panel = ow;
    scope = sc;

    drawarea = new DrawArea((void*)this, Draw_Knob);
    MC->AddDrawArea(drawarea);
}

void Knob::UpdateValFromPos()
{
    if(param != NULL)
    {
        param->SetValueFromControl((1 - float((pos + PI*0.25)/(PI*1.5))));
    }

	if(panel != NULL)
	{
		panel->Update();
	}
}

void Knob::UpdatePosFromVal()
{
    if(param != NULL)
    {
        pos = float((1 - (param->val - param->offset)/param->range)*(PI*1.5) - PI*0.25);
    }
    else
    {
        pos = 0;
    }

    if(drawarea != NULL)
    {
        drawarea->Change();
    }

    UpdateAnchorSign();
}

void Knob::SetAnchor(float val)
{
    anchored = true;
    if(param != NULL)
    {
        anchorpos = float((1 - (val - param->offset)/param->range)*(PI*1.5) - PI*0.25);
    }
    else
    {
        anchorpos = 0;
    }
}

void Knob::TweakByWheel(int delta, int mouse_x, int mouse_y)
{
    pos -= (float)((PI/32)*delta);

    if(pos < 0 - (float)PI*0.25f)
    {
        pos = 0 - (float)PI*0.25f;
    }
    else if(pos > (float)PI*1.25f)
    {
        pos = (float)PI*1.25f;
    }

    /*if(anchored)
    {
        if(abs(pos - anchorpos) < 0.1)
        {
            pos = anchorpos;
        }
    }*/

    UpdateValFromPos();
    if(panel != NULL)
    {
        panel->HandleTweak(this, xs, ys);
    }

    UpdateAnchorSign();
}

void Knob::TweakByMouseMove(int mouse_x, int mouse_y)
{
    if(anchorcount == 0)
    {
        int dy = mouse_y - ys;
        pos += (float)dy/25;
        
        if(pos < 0 - PI*0.25)
        {
           pos = (float)(0 - PI*0.25);
        }
        else if(pos > PI + PI*0.25)
        {
           pos = (float)(PI*1.25f);
        }

        if(anchored)
        {
            bool newsign = pos > anchorpos ? true : false;
            if(newsign != anchorsign)
            {
                pos = anchorpos;
                anchorsign = newsign;
                anchorcount = 0;
            }
        }

        UpdateValFromPos();
        if(panel != NULL)
        {
            panel->HandleTweak(this, xs, ys);
        }
        ys = mouse_y;
    }
    else
    {
        ys = mouse_y;
        anchorcount--;
    }
}

void Knob::UpdateAnchorSign()
{
    if(pos > anchorpos)
    {
        anchorsign = true;
    }
    else if(pos < anchorpos)
    {
        anchorsign = false;
    }
}

Butt::Butt(bool state)
{
    type = Ctrl_Button;
    pressed = state;
    imgup = imgdn = NULL;

    freeset = true;
    needs_refresh = false;
    drawarea = new DrawArea((void*)this, Draw_Button);
    MC->AddDrawArea(drawarea);
}

Butt::~Butt()
{
}

void Butt::Press()
{
    pressed = true;
    Refresh();
    this->OnClick(NULL);
}

void Butt::Release()
{
    Refresh();
    pressed = false;
}

void Butt::SetImages(Image* imup, Image* imdn)
{
    imgup = imup;
    imgdn = imdn;
}

void Butt::Refresh()
{
    needs_refresh = true;
}

void Butt::MouseClick(int mouse_x, int mouse_y)
{
    Press();
    if(panel != NULL)
    {
        panel->HandleButtDown(this);
    }
}

ButtFix::ButtFix(bool state) : Butt(state)
{
    type = Ctrl_ButtFix;
    pn = NULL;
    imgup = imgdn = NULL;

    needs_refresh = false;
}

ButtFix::ButtFix(bool state, Panel* panel) : Butt(state)
{
    type = Ctrl_ButtFix;
    pn = panel;
    imgup = imgdn = NULL;

    needs_refresh = false;
}

void ButtFix::Press()
{
    if(pressed == false)
    {
        pressed = true;
    }
    else
    {
        pressed = false;
    }
    Refresh();
}

void ButtFix::Release()
{
    pressed = false;
    Refresh();
}

void ButtFix::MouseClick(int mouse_x, int mouse_y)
{
    if(panel != NULL)
    {
        panel->HandleButtDown(this);
    }
    this->OnClick(NULL);
}

Toggle::Toggle(Panel* ow)
{
    type = Ctrl_Toggle;
    tgtype = Toggle_Default;

    state = NULL;

    panel = ow;
    mixcell = NULL;
    trk = NULL;
    instr = NULL;
    imgup = imgdn = NULL;
}

Toggle::Toggle(bool* st, Scope* scp)
{
    type = Ctrl_Toggle;
    tgtype = Toggle_Default;

    state = st;

    panel = NULL;
    mixcell = NULL;
    trk = NULL;
    instr = NULL;
    scope = scp;
}

Toggle::Toggle(bool* st, Scope* scp, ToggleType tt)
{
    type = Ctrl_Toggle;
    tgtype = tt;

    state = st;

    panel = NULL;
    mixcell = NULL;
    trk = NULL;
    instr = NULL;
    imgup = imgdn = NULL;
    scope = scp;

    if(tgtype == Toggle_EffBypass)
    {
        drawarea = new DrawArea((void*)this, Draw_Toggle);
        MC->AddDrawArea(drawarea);
    }
}

Toggle::Toggle(ToggleType tt, bool* st)
{
    type = Ctrl_Toggle;
    tgtype = tt;

    state = st;

    panel = NULL;
    mixcell = NULL;
    trk = NULL;
    instr = NULL;
    imgup = imgdn = NULL;

    if(tgtype != Toggle_EnvFold && tgtype != Toggle_EnvTime)
    {
        drawarea = new DrawArea((void*)this, Draw_Toggle);
        MC->AddDrawArea(drawarea);
    }
}

Toggle::Toggle(Instrument* i, ToggleType tt)
{
    type = Ctrl_Toggle;
    tgtype = tt;

    if(tt == Toggle_Mute || tt == Toggle_Mute1)
    {
        state = &i->params->muted;
    }
    else if(tt == Toggle_Solo || tt == Toggle_Solo1)
    {
        state = &i->params->solo;
    }

    drawarea = new DrawArea((void*)this, Draw_Toggle);
    MC->AddDrawArea(drawarea);

    panel = NULL;
    mixcell = NULL;
    trk = NULL;
    instr = i;
    imgup = imgdn = NULL;
    scope = &i->scope;
}

Toggle::Toggle(Mixcell* mc, ToggleType tt)
{
    type = Ctrl_Toggle;
    tgtype = tt;

    if(tt == Toggle_Mute || tt == Toggle_Mute1)
    {
        state = &mc->params->muted;
    }
    else
        if(tt == Toggle_Solo || tt == Toggle_Solo1)
    {
        state = &mc->params->solo;
    }
    else if(tt == Toggle_MixBypass)
    {
        state = &mc->bypass;
    }
    drawarea = new DrawArea((void*)this, Draw_Toggle);
    MC->AddDrawArea(drawarea);

    panel = (Panel*)mc;
    mixcell = mc;
    trk = NULL;
    instr = NULL;
    imgup = imgdn = NULL;
    scope = &mc->scope;
}

Toggle::Toggle(Trk* t, ToggleType tt)
{
    type = Ctrl_Toggle;
    tgtype = tt;

    if(tt == Toggle_Mute || tt == Toggle_Mute1)
    {
        state = &t->params->muted;
    }
    else if(tt == Toggle_Solo || tt == Toggle_Solo1)
    {
        state = &t->params->solo;
    }
    drawarea = new DrawArea((void*)this, Draw_Toggle);
    MC->AddDrawArea(drawarea);

    panel = NULL;
    mixcell = NULL;
    trk = t;
    instr = NULL;
    imgup = imgdn = NULL;
    scope = &t->scope;
}

void Toggle::PerClick()
{
    this->OnClick(owner);

    if(tgtype == Toggle_Solo)
    {
        if(instr != NULL)
        {
            if(instr == solo_Instr)
            {
                solo_Instr = NULL;
            }
            else
            {
                if(solo_Instr != NULL)
                {
                    solo_Instr->solo->MouseClick();
                }
                solo_Instr = instr;
            }
        }
        else if(trk != NULL)
        {
            if(trk == solo_Trk)
            {
                solo_Trk = NULL;
            }
            else
            {
                if(solo_Trk != NULL)
                {
                    solo_Trk->solo->MouseClick();
                }
                solo_Trk = trk;
            }
        }
        else if(mixcell != NULL)
        {
            if(mixcell->auxcell)
            {
                MixChannel* mchan = mixcell->mchan;
                if(mchan == solo_MixChannel)
                {
                    solo_MixChannel = NULL;
                }
                else
                {
                    if(solo_MixChannel != NULL)
                    {
                        solo_MixChannel->mc_main->solo_toggle->MouseClick();
                    }
                    solo_MixChannel = mchan;
                }
            }
            else
            {
                if(mixcell == solo_Mixcell)
                {
                    solo_Mixcell = NULL;
                }
                else
                {
                    if(solo_Mixcell != NULL)
                    {
                        solo_Mixcell->solo_toggle->MouseClick();
                    }
                    solo_Mixcell = mixcell;
                }
            }
        }
    }
    else if(tgtype == Toggle_EnvFold || tgtype == Toggle_EnvTime)
    {
        if(M.loc == Loc_MainGrid)
        {
            R(Refresh_GridContent);
        }
        else if(M.loc == Loc_SmallGrid)
        {
            R(Refresh_AuxContent);
        }
    }
    else if(tgtype == Toggle_EffFold)
    {
        R(Refresh_AuxHighlights);
    }
    else if(tgtype == Toggle_AutoPatt)
    {
        IP->EditInstrumentAutopattern(instr);
    }
    else if(panel == IP)
    {
        R(Refresh_InstrPanel);
    }

	if(drawarea != NULL)
    {
		drawarea->Change();
	}

    if(panel != NULL)
    {
        panel->HandleToggle(this);
    }

	// Place param updating here to prevent exceptions on deletion of this control (may occur on browser controls
	// refreshing in Browse_Params mode)
	if(param != NULL)
    {
        param->UpdateBindings();
    }

    ChangesIndicate();
}

void Toggle::MouseClick(int mouse_x, int mouse_y)
{
    if(state != NULL)
    {
       *state = !(*state);
    }
    PerClick();
}

void Toggle::SetImages(Image* imup, Image* imdn)
{
    imgup = imup;
    imgdn = imdn;
}

ToggleParam::ToggleParam(bool* st, Scope* scp, Paramcell* pd) : Toggle(st, scp)
{
    pdata = pd;
}

void ToggleParam::AddParam(Parameter* prm)
{
    if(prm->type == Param_Bool)
    {
        param = prm;
        state = &((BoolParam*)param)->outval;
    }
}

void ToggleParam::MouseClick(int mouse_x, int mouse_y)
{
    if(pdata != NULL)
    {
        // Block setting to off for radiobuttons
        if(!(pdata->pctype == PCType_MixRadio && *state == true))
        {
            Toggle::MouseClick();
        }
    }
}

bool ToggleParam::MouseCheck(int mouse_x, int mouse_y)
{
    if((mouse_x >= pdata->drawarea->ax)&&(mouse_x <= (pdata->drawarea->ax + pdata->drawarea->aw))&&
       (mouse_y >= pdata->drawarea->ay)&&(mouse_y <= pdata->drawarea->ay + pdata->drawarea->ah))
    {
        return Control::MouseCheck(mouse_x, mouse_y);
    }
    else
    {
        return false;
    }
}

void ToggleParam::Release()
{
    if(state != NULL && *state == true)
    {
       *state = false;
        PerClick();
    }
}

void ToggleParam::UpdatePosFromVal()
{
    if(pdata != NULL)
    {
        if(pdata->pctype == PCType_MixRadio && *state == true)
        {
            pdata->SetCurrentItem(this);
        }
        pdata->drawarea->Change();
    }
}

bool OutPin::MouseCheck(int mouse_x, int mouse_y)
{
    if((mouse_x >= x - 2)&&(mouse_x <= (x + width + 2))&&
       (mouse_y >= y - 2)&&(mouse_y <= y + height + 2))
    {
        xconn = xc;
        yconn = yc;
        point_begin = true;
        return true;
    }
    else if((dst != NULL)&&((mouse_x >= dst->xc - 8 + xc_offs)&&
                             (mouse_x <= (dst->xc + 8 + xc_offs))&&
                             (mouse_y >= dst->yc - 8 + yc_offs)&&
                             (mouse_y <= dst->yc + 8 + yc_offs)))
    {
        xconn = dst->xc + xc_offs;
        yconn = dst->yc + yc_offs;
        point_begin = false;
        return true;
    }
    else
    {
        return false;
    }
}

ValueString::ValueString(VStrType vtype)
{
    vstrtype = vtype;
    val.n_val = 0;

    fract_size = 2;
    memset(label, 0, sizeof(label));
}

void ValueString::SetValue(float newval)
{
    switch(vstrtype)
    {
        case VStr_kHz:
        case VStr_Hz:
        case VStr_fHz:
        case VStr_fHz1:
            val.hz = newval;
            break;
        case VStr_dB:
            val.dB = newval;
            break;
        case VStr_msec:
            val.ms = newval;
            break;
        case VStr_msec2:
            val.ms = newval;
            break;
        case VStr_second:
            val.sec = newval;
            break;
        case VStr_Semitones:
            val.semitones = newval;
            break;
        case VStr_Beats:
            val.beats = newval;
            break;
        default:
            val.n_val = newval;
            break;
    }

    UpdateString();
}

void ValueString::SetValue(int newval)
{
    switch(vstrtype)
    {
        case VStr_Percent:
            val.percent = newval;
            break;
        case VStr_Integer:
            val.integer = newval;
            break;
        default:
            val.n_val = (float)newval;
            break;
    }

    UpdateString();
}

void ValueString::SetValue(char* strval)
{
    switch(vstrtype)
    {
        case VStr_String:
            strcpy(val.str, strval);
            break;
    }

    UpdateString();
}

void ValueString::UpdateString()
{
    String str;
    switch(vstrtype)
    {
        case VStr_String:
            str = String(val.str);
            break;
        case VStr_Hz:
            str = String::formatted(T("%.0f Hz"), val.hz);
            break;
        case VStr_fHz:
            str = String::formatted(T("%.2f Hz"), val.hz);
            break;
        case VStr_fHz1:
            str = String::formatted(T("%.1f Hz"), val.hz);
            break;
        case VStr_kHz:
            str = String::formatted(T("%.2f kHz"), val.hz);
            break;
        case VStr_Percent:
            str = String::formatted(T("%d %s"), val.percent, "%");
            break;
        case VStr_Integer:
            str = String::formatted(T("%d"), val.integer);
            break;
        case VStr_dB:
            if(val.n_val > 0)
			    str = String::formatted(T("%.1f dB"), val.n_val);
            else
			    str = String::formatted(T("%.1f dB"), val.n_val);
            break;
        case VStr_msec:
			str = String::formatted(T("%.1f ms"), val.n_val);
            break;
        case VStr_msec2:
			str = String::formatted(T("%.2f ms"), val.n_val);
            break;
        case VStr_second:
			str = String::formatted(T("%.1f sec"), val.n_val);
            break;
        case VStr_oct:
			str = String::formatted(T("%.2f oct"), val.n_val);
            break;
        case VStr_Semitones:
			str = String::formatted(T("%.2f st"), val.n_val);
            break;
        case VStr_Beats:
			str = String::formatted(T("%.1f beats"), val.n_val);
            break;
        case VStr_DryWet:
			str = String::formatted(T("%.0f/%.0f"), (1 - val.n_val)*100, val.n_val*100);
            break;
        case VStr_Default:
            //g.setFont(*ti);
            if(label[0] != '\0')
            {
                str = String(label);
            }
            else
            {
                str = String::formatted(T("%.2f"), val.n_val);
            }
            break;
    }

    str.copyToBuffer(outstr, 100);
}

MenuItem::MenuItem(MItemType itype, Menu* m)
{
    type = Ctrl_MenuItem;
    itemtype = itype;
    menu = NULL;
    submenu = m;
    doable = true;

    iprev = NULL;
    inext = NULL;
}

bool MenuItem::MouseCheck(int mouse_x, int mouse_y)
{
    if(menu->active == true)
    {
        if((mouse_x >= x)&&(mouse_x <= (x + width))&&(mouse_y >= y)&&(mouse_y <= (y + height)))
        {
            if(menu == CP->MainMenu && M.menu_active == true && 
               M.contextmenu != submenu &&
               M.contextmenu != menu && 
               M.contextmenu->mtype == Menu_Main)
            {
				M.contextmenu->drawarea->DisableRedraw(MC->listen);
				M.contextmenu->active = false;

				M.contextmenu = submenu;
				if(submenu != NULL)
				{
					M.contextmenu->active = true;
					if(M.contextmenu->dir == true)
					{
						M.contextmenu->x = x;
						M.contextmenu->y = y + height;
					}
					else
					{
						M.contextmenu->x = x + width + 6;
						M.contextmenu->y = y;
					}

					MenuItem* mi = submenu->first_item;
					while(mi != NULL)
					{
						mi->active = true;
						mi = mi->inext;
					}

					M.contextmenu->drawarea->EnableWithBounds(M.contextmenu->x,
															  M.contextmenu->y,
															  M.contextmenu->width,
															  M.contextmenu->height);
				}
				else
				{
					M.menu_active = false;
				}
			}
            return true;
        }
        else
        {
            return false;
        }
    }
	return false;
}

void MenuItem::MouseClick(int mouse_x, int mouse_y)
{
    MenuItem* mitem = (MenuItem*)M.active_ctrl;
    if(mitem->submenu != NULL)
    {
        M.menu_active = true;
        M.contextmenu = mitem->submenu;
        M.contextmenu->active = true;
        if(M.contextmenu->dir == true)
        {
            M.contextmenu->x = mitem->x;
            M.contextmenu->y = mitem->y + mitem->height;
        }
        else
        {
            M.contextmenu->x = mitem->x + mitem->width + 6;
            M.contextmenu->y = mitem->y;
        }
    
        MenuItem* mi = mitem->submenu->first_item;
        while(mi != NULL)
        {
            mi->active = true;
            if(mi->itemtype == MItem_Custom && strlen(mi->title) == 0)
                mi->active = false;
            mi = mi->inext;
        }
    
        mitem->submenu->drawarea->EnableWithBounds(mitem->submenu->x, 
                                                   mitem->submenu->y, 
                                                   mitem->submenu->width + 1, 
                                                   mitem->submenu->height + 1);
    }
    else if(mitem->itemtype == MItem_Render)
    {
        ToggleRenderWindow();
    }
}

Menu::Menu(bool direction, MenuType mt)
{
    type = Ctrl_Menu;
    mtype = mt;
    dir = direction;

    width = 200;
    height = 5;
    num_items = 0;

    first_item = NULL;
    last_item = NULL;
    main_item = NULL;

    activectrl = NULL;
    module = NULL;
    activepresetdata = NULL;
    activefiledata = NULL;
    browser = NULL;
    curreff = NULL;
    mchan = NULL;

    drawarea = new DrawArea((void*)this, Draw_Menu);
    MC->AddDrawArea(drawarea);
}

Menu::Menu(int xc, int yc, MenuType mt)
{
    type = Ctrl_Menu;
    mtype = mt;
    dir = false;

    x = xc;
    y = yc;

    width = 180;
    height = 5;
    num_items = 0;

	first_item = NULL;
	last_item = NULL;
    main_item = NULL;

    activectrl = NULL;
    module = NULL;
    activepresetdata = NULL;
    activefiledata = NULL;
    browser = NULL;
    curreff = NULL;
    mchan = NULL;

    drawarea = new DrawArea((void*)this, Draw_Menu);
    MC->AddDrawArea(drawarea);
}

void Menu::SetMainItem(MItemType itype)
{
    MenuItem* mi = first_item;
    while(mi != NULL)
    {
        if(mi->itemtype == itype)
        {
            main_item = mi;
            break;
        }
        mi = mi->inext;
    }
}

void Menu::AddItem(MItemType itype, Menu* menu)
{
    num_items++;
    if(itype == MItem_Divider)
    {
        height += DIVIDER_HEIGHT;
    }
    else
    {
        height += MITEM_HEIGHT;
    }

    MenuItem* item = new MenuItem(itype, menu);
    item->menu = this;

    if(first_item == NULL && last_item == NULL)
    {
        item->iprev = NULL;
        item->inext = NULL;
        first_item = item;
    }
    else
    {
        last_item->inext = item;
        item->iprev = last_item;
        item->inext = NULL;
    }
    last_item = item;
    AddNewControl(item, NULL);
}

void Menu::AddItem(MItemType itype, Menu* menu, char* tt)
{
    num_items++;
    if(itype == MItem_Divider)
    {
        height += DIVIDER_HEIGHT;
    }
    else
    {
        height += MITEM_HEIGHT;
    }

    MenuItem* item = new MenuItem(itype, menu);
    item->menu = this;
    item->SetTitle(tt);

    if(first_item == NULL && last_item == NULL)
    {
        item->iprev = NULL;
        item->inext = NULL;
        first_item = item;
    }
    else
    {
        last_item->inext = item;
        item->iprev = last_item;
        item->inext = NULL;
    }
    last_item = item;
    AddNewControl(item, NULL);
}

DropMenu::DropMenu(bool direction, MenuType mt) : Menu(direction, mt)
{
}

void DropMenu::UpdateBounds(bool opened)
{
    if(opened)
    {
        drawarea->Enable();
        drawarea->SetBounds(x - 1, y - 1, width + 3, height + 3);
    }
    else
    {
        drawarea->Disable();
        if(panel == NULL)
        {
            drawarea->SetBounds(x - 1, y - 1, width + 3, height + 3);
        }
        else
        {
            if(this == aux_panel->qMenu)
            {
                drawarea->SetBounds(x - 1, y - 1, width + 3, height + 3,
                                    panel->x, 
                                    panel->y, 
                                    panel->x + panel->width - 1 - MixCenterWidth - 20,
                                    panel->y + panel->height);
            }
            else
            {
                drawarea->SetBounds(x - 1, y - 1, width + 3, height + 3,
                                    panel->x, 
                                    panel->y, 
                                    panel->x + panel->width - 1,
                                    panel->y + panel->height - 1);
            }
        }
    }
}

Paramcell::Paramcell(ParamcellType type, Parameter* prm, Panel* pn, Scope* scp)
{
    pctype = type;
    ctrl_first = ctrl_last = NULL;
    memset(title, 0, 100);
    scope = scp;
    tgp_current = NULL;
    toggle = NULL;
    slider = NULL;
    cloned = NULL;
    orig = NULL;
    visible = true;
    panel = pn;
    param = prm;
    if(param != NULL)
    {
        vstring = param->vstring;
    }

    drawarea = new DrawArea((void*)this, Draw_Param);
    MC->AddDrawArea(drawarea);
    if(pctype == PCType_MixSlider)
    {
        int len = 80;
        if(pn == NULL) // Effect cells
        {
            len = 90;
        }
        slider = new SliderParam(len, pn, scope, this);
        prm->AddControl(slider);
        AddNewControl(slider, pn);
        h = 22;
        h1 = 24;
    }
    else if(pctype == PCType_MixToggle)
    {
        toggle = new ToggleParam(NULL, scope, this);
        prm->AddControl(toggle);
        AddNewControl(toggle, pn);
        h = 14;
        h1 = 20;
    }
    else if(pctype == PCType_MixRadio)
    {
        h = 4;
        h1 = 10;
    }
    drawarea->UnChange();
}

Paramcell::~Paramcell()
{
    MC->RemoveDrawArea(drawarea);

    if(pctype == PCType_MixSlider)
    {
        RemoveControlCommon(slider);
    }
    else if(pctype == PCType_MixToggle)
    {
        RemoveControlCommon(toggle);
    }
    if(pctype == PCType_MixRadio)
    {
        Control* ctnext;
        Control* ct = ctrl_first;
        while(ct != NULL)
        {
            ctnext = ct->lst_next;
            RemoveControlCommon(ct);

            ct = ctnext;
        }
    }

    if(orig != NULL)
    {
        orig->cloned = NULL;
    }
}

Paramcell* Paramcell::Clone(Panel* pn)
{
    Paramcell* clone = new Paramcell(pctype, param, pn, scope);
    clone->SetTitle(title);
    clone->visible = visible;
    if(pctype == PCType_MixRadio)
    {
        Control* ct = ctrl_first;
        while(ct != NULL)
        {
            clone->AddParamEntry(ct->param);
            ct = ct->lst_next;
        }
    }
    cloned = clone;
    cloned->orig = this;
    return clone;
}

void Paramcell::Disable()
{
    drawarea->Disable();
    if(slider != NULL)
    {
        slider->Deactivate();
    }
    if(toggle != NULL)
    {
        toggle->Deactivate();
    }
    Control* ct = ctrl_first;
    while(ct != NULL)
    {
        ct->Deactivate();
        ct = ct->lst_next;
    }
}

void Paramcell::Enable()
{
    drawarea->Enable();
    if(slider != NULL)
    {
        slider->Activate();
    }
    if(toggle != NULL)
    {
        toggle->Activate();
    }
    Control* ct = ctrl_first;
    while(ct != NULL)
    {
        ct->Activate();
        ct = ct->lst_next;
    }
}

void Paramcell::AddParamEntry(Parameter* prm)
{
    if(pctype == PCType_MixRadio)
    {
        prm->grouped = true;
        ToggleParam* tgp = new ToggleParam(NULL, scope, this);
        tgp->SetTitle(prm->GetName());
        prm->AddControl(tgp);
        AddNewControl(tgp, panel);

        if(ctrl_first == NULL && ctrl_last == NULL)
        {
            tgp->lst_prev = NULL;
            tgp->lst_next = NULL;
            ctrl_first = tgp;
        }
        else
        {
            ctrl_last->lst_next = tgp;
            tgp->lst_prev = ctrl_last;
            tgp->lst_next = NULL;
        }
        ctrl_last = tgp;
        h += 11;
        h1 += 11;
    }
}

void Paramcell::SetCurrentItem(ToggleParam * tgp_new)
{
    if(tgp_new->pdata == this && tgp_current != tgp_new)
    {
		if(tgp_current != NULL)
			tgp_current->Release();
        tgp_current = tgp_new;
	}
}

void Paramcell::SetCurrentItemByTitle(char* title)
{
    ToggleParam* tgp;
    Control* ct = ctrl_first;
    while(ct != NULL)
    {
        if(strcmp(ct->title, title) == 0)
        {
            tgp = (ToggleParam*)ct;
            if(tgp != tgp_current && *(tgp->state) == false)
            {
                tgp->MouseClick();
            }
            break;
        }
        ct = ct->lst_next;
    }
}

ToggleParam* Paramcell::GetCurrentItem()
{
    return tgp_current;
}

void Paramcell::SetVisible(bool vis)
{
    visible = vis;
    if(vis == false)
    {
        Disable();
    }
    else if(drawarea->isEnabled())
    {
        Enable();
    }

    if(cloned != NULL)
        cloned->SetVisible(vis);
}

