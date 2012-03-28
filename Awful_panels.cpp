#include "awful.h"
#include "awful_elements.h"
#include "awful_paramedit.h"
#include "awful_controls.h"
#include "awful_audio.h"
#include "awful_effects.h"
#include "awful_instruments.h"
#include "awful_panels.h"
#include "awful_utils_common.h"
#include "VSTCollection.h"
#include "awful_events_triggers.h"
#include "awful_audiohost.h"
#include "awful_renderer.h"
#ifdef USE_OLD_JUCE
#include "juce_amalgamated.h"
#else
#include "juce_amalgamated_NewestMerged.h"
#endif
#include "awful_preview.h"
#include "awful_cursorandmouse.h"
#include "awful_parammodule.h"
#include "awful_undoredo.h"

void OnBypassClick(Object* owner, Object* self);
void EffFoldClick(Object* owner, Object* self);
void EffBypassClick(Object* owner, Object* self);

Panel::Panel()
{
	first_ctrl = NULL;
	last_ctrl = NULL;

    prev = NULL;
    next = NULL;

    active = true;
    locked = false;
}

Panel::~Panel()
{
    Control *ctrl, *ctrl1;

    ctrl = this->first_ctrl;
    while (ctrl != NULL)
    {
        ctrl1 = ctrl;
        ctrl = ctrl->next;
        this->RemoveControl(ctrl1);
        RemoveControlCommon(ctrl1);
    }
}

void Panel::Update()
{
}

void Panel::Update(PEdit* pe)
{
}

void Panel::AddControl(Control* ct)
{
    if((first_ctrl == NULL)&&(last_ctrl == NULL))
    {
        ct->pn_prev = NULL;
        ct->pn_next = NULL;
        first_ctrl = ct;
        last_ctrl = first_ctrl;
    }
    else
    {
        last_ctrl->pn_next = ct;
        ct->pn_prev = last_ctrl;
        ct->pn_next = NULL;
        last_ctrl = ct;
    }
    ct->panel = this;
}

void Panel::RemoveControl(Control* ctrl)
{
    if(ctrl != NULL && ctrl->panel == this)
    {
        if((ctrl == first_ctrl)&&(ctrl == last_ctrl))
        {
            first_ctrl = NULL;
            last_ctrl = NULL;
        }
        else if(ctrl == first_ctrl)
        {
            first_ctrl = ctrl->pn_next;
            first_ctrl->pn_prev = NULL;
        }
        else if(ctrl == last_ctrl)
        {
            last_ctrl = ctrl->pn_prev;
            last_ctrl->pn_next = NULL;
        }
        else
        {
            if(ctrl->pn_prev != NULL)
            {
                ctrl->pn_prev->pn_next = ctrl->pn_next;
            }
            if(ctrl->pn_next != NULL)
            {
                ctrl->pn_next->pn_prev = ctrl->pn_prev;
            }
        }

        ctrl->panel = NULL;
    }
}

void Panel::HandleButtUp(Butt* bt)
{
    R(Refresh_Buttons);
}

void Panel::HandleButtDown(Butt* bt)
{
    bt->Refresh();
    R(Refresh_Buttons);
}

void Panel::HandleToggle(Toggle* tg)
{

}

CtrlPanel::CtrlPanel()
{
    type = Panel_Main;
    memset(&scope, 0, sizeof(Scope));

    main_bar = new ScrollBard(Ctrl_HScrollBar, 0, 0, NavHeight - 5);
    main_bar->Activate();
    main_bar->fixed = true;
    AddNewControl(main_bar, NULL);

    main_v_bar = new ScrollBard(Ctrl_VScrollBar, 0, 0, 17);
    main_v_bar->discrete = true;
    main_v_bar->Activate();
    main_v_bar->AddDrawarea();
    AddNewControl(main_v_bar, NULL);

    up = new Butt(false);
    down = new Butt(false);

    AddNewControl(up, this);
    AddNewControl(down, this);

    MVol = new SliderBase(this, &scope);
    MVol->height = MVol->length = 48;
    MVol->vu->vtype = VU_BigMain;
    //MVol->SetHint("Master volume control\0");

    slzoom = new SliderZoom(this, NULL);
    //slzoom->SetHint("Zoom slider\0");
    slzoom->SetTickWidth(tickWidth);

    //MPan = new Knob(50, NULL, this, &scope);
    //MPan->SetHint("Master Panning Control\0");

    mAster.params->vol->AddControl(MVol);
    mAster.params->vol->scope = &scope;
    mAster.params->pan->scope = &scope;
    MVol->SetAnchor(1);
    //mix->m_cell.vol_slider->SetAnchor(1);
    mAster.params->vol->SetNormalValue(1.0f);

    play_butt = new ButtFix(false);
    play_butt->SetHint("Play song");
    stop_butt = new Butt(false);
    stop_butt->SetHint("Stop playback");
    Record = new ButtFix(false);
    Record->SetHint("Record mode");

    Go2Home = new Butt(false);
    Go2Home->SetHint("Go to the beginning of the song");
    FFwd = new Butt(false);
    FFwd->SetHint("Go one beat forward");
    Rewind = new Butt(false);
    Rewind->SetHint("Go one beat backward");
    Go2End = new Butt(false);
    Go2End->SetHint("Go to the end of the song");

    Config = new Butt(false);
    Render = new Butt(false);

    IncrScale = new Butt(false);
    DecrScale = new Butt(false);
    //DefaultScaleMax = new Butt(false);
    //DefaultScaleMin = new Butt(false);

    IncrOctave = new Butt(false);
    DecrOctave = new Butt(false);

    Back = new Butt(false);
    Forth = new Butt(false);
    Back->coverable = false;
    Forth->coverable = false;

    UsualMode = new ButtFix(false);
    NoteMode = new ButtFix(false);
    MetroMode = new ButtFix(false);
    SlideMode = new ButtFix(false);
    FastMode = new ButtFix(false);
    BrushMode = new ButtFix(false);
    CurrentMode = UsualMode;

    BtFollowPlayback = new ButtFix(false);
    BtFollowPlayback->coverable = false;

    Waves = new ButtFix(false);
    Spectrum = new ButtFix(false);
    TextBar = new ScrollBard();

    autoadv = new Toggle(&auto_advance, NULL);

    copymode = true;
    pattmode = new Toggle(&copymode, NULL);

    bpmer = new Numba(&beats_per_minute, Numba_BPM);
    tpber = new Numba(&ticks_per_beat, Numba_TPB);
    bpber = new Numba(&beats_per_bar, Numba_BPB);
    octaver = new Numba(&Octave, Numba_Octave);

    tmarea = new DrawArea(NULL, Draw_Timer);
    MC->AddDrawArea(tmarea);
    tmarea->Change();

    minimize = new Butt(false);
    maximize = new Butt(false);
    close = new Butt(false);

    ShowTrackControls = new ButtFix(false);
    AddNewControl(ShowTrackControls, this);

    AccGenBrowsa = new Butt(false);
    AccGenBrowsa->SetHint("Show browser");
    AccInstrPanel = new Butt(false);
    AccInstrPanel->SetHint("Hide browser");
    AccGenBrowsa->coverable = false;
    AccInstrPanel->coverable = false;
    AddNewControl(AccGenBrowsa, this);
    AddNewControl(AccInstrPanel, this);

    AccBasicGrid = new Butt(false);
    AccBasicMixer = new Butt(false);
    AccBasicPatt = new Butt(false);
    AddNewControl(AccBasicGrid, this);
    AddNewControl(AccBasicMixer, this);
    AddNewControl(AccBasicPatt, this);

    AddNewControl(minimize, this);
    AddNewControl(maximize, this);
    AddNewControl(close, this);

    AddNewControl(MVol, this);
    AddNewControl(slzoom, this);
    AddNewControl(play_butt, this);
    AddNewControl(stop_butt, this);

    AddNewControl(Record, this);
    AddNewControl(Go2Home, this);
    AddNewControl(Go2End, this);
    AddNewControl(Rewind, this);
    AddNewControl(FFwd, this);

    AddNewControl(Waves, this);
    AddNewControl(Spectrum, this);
    AddNewControl(TextBar, this);
    AddNewControl(NoteMode, this);
    AddNewControl(MetroMode, this);
    AddNewControl(SlideMode, this);
    AddNewControl(UsualMode, this);
    AddNewControl(FastMode, this);
    AddNewControl(BrushMode, this);
    AddNewControl(BtFollowPlayback, this);

    AddNewControl(IncrScale, this);
    AddNewControl(DecrScale, this);
    //AddNewControl(DefaultScaleMax, this);
    //AddNewControl(DefaultScaleMin, this);

    AddNewControl(IncrOctave, this);
    AddNewControl(DecrOctave, this);

    AddNewControl(autoadv, this);
    AddNewControl(pattmode, this);

    AddNewControl(Config, this);
    AddNewControl(Render, this);

    AddNewControl(Back, this);
    AddNewControl(Forth, this);

    HotKeys = new Butt(false);
    AddNewControl(HotKeys, this);

    HKWnd = MainWnd->CreateHotKeysWindow();
    AboutWnd = NULL;

    view_mixer = new ButtFix(false);
    AddNewControl(view_mixer, this);

    lastauxmode = AuxMode_Undefined;

    MainMenu = new Menu(false, Menu_Main);

    FileMenu = new Menu(true, Menu_Main);
    FileMenu->AddItem(MItem_New, NULL);
    FileMenu->AddItem(MItem_Open, NULL);
    FileMenu->AddItem(MItem_Divider, NULL);
    FileMenu->AddItem(MItem_Save, NULL);
    FileMenu->AddItem(MItem_SaveAs, NULL);
    FileMenu->AddItem(MItem_Divider, NULL);
    FileMenu->AddItem(MItem_LoadProject, NULL);
    FileMenu->AddItem(MItem_LoadInstrumentPlugin, NULL);
    FileMenu->AddItem(MItem_LoadEffectPlugin, NULL);
    FileMenu->AddItem(MItem_LoadSample, NULL);
    FileMenu->AddItem(MItem_Divider, NULL);
    FileMenu->AddItem(MItem_Render, NULL);
    FileMenu->AddItem(MItem_Divider, NULL);
    FileMenu->AddItem(MItem_Exit, NULL);
    FileMenu->AddItem(MItem_Divider, NULL);
    FileMenu->AddItem(MItem_Custom, NULL);
    FileMenu->AddItem(MItem_Custom, NULL);
    FileMenu->AddItem(MItem_Custom, NULL);
    FileMenu->AddItem(MItem_Custom, NULL);
    FileMenu->AddItem(MItem_Custom, NULL);
    FileMenu->AddItem(MItem_Custom, NULL);
    FileMenu->AddItem(MItem_Custom, NULL);
    FileMenu->AddItem(MItem_Custom, NULL);
    FileMenu->AddItem(MItem_Custom, NULL);
    FileMenu->AddItem(MItem_Custom, NULL);

    EditMenu = new Menu(true, Menu_Main);
    EditMenu->AddItem(MItem_Undo, NULL);
    EditMenu->AddItem(MItem_Redo, NULL);
    EditMenu->AddItem(MItem_Divider, NULL);
    //EditMenu->AddItem(MItem_InsertTrack, NULL);
    //EditMenu->AddItem(MItem_DeleteTrack, NULL);
    //EditMenu->AddItem(MItem_Divider, NULL);
    EditMenu->AddItem(MItem_CleanSong, NULL);
    EditMenu->AddItem(MItem_Divider, NULL);
    EditMenu->AddItem(MItem_Preferences, NULL);

    ViewMenu = new Menu(true, Menu_Main);
    //ViewMenu->AddItem(MItem_HideAll, NULL);
    //ViewMenu->AddItem(MItem_ShowAll, NULL);
    //ViewMenu->AddItem(MItem_Divider, NULL);
    //ViewMenu->AddItem(MItem_FullScreen, NULL);

    HelpMenu = new Menu(true, Menu_Main);
    //HelpMenu->AddItem(MItem_Contents, NULL);
    HelpMenu->AddItem(MItem_HotKeys, NULL);
    HelpMenu->AddItem(MItem_Divider, NULL);
    HelpMenu->AddItem(MItem_About, NULL);

    MainMenu->AddItem(MItem_File, FileMenu, "File");
    MainMenu->AddItem(MItem_Edit, EditMenu, "Edit");
    //MainMenu->AddItem(MItem_View, ViewMenu, "View");
    MainMenu->AddItem(MItem_Help, HelpMenu, "Help");
    MainMenu->AddItem(MItem_Render, NULL, "Render");

    AddNewControl(MainMenu, this);

    qMenu = new DropMenu(true, Menu_Quant);
    qMenu->AddItem(MItem_Divider, NULL, ".....");
    qMenu->AddItem(MItem_None, NULL, "(none)");
    qMenu->AddItem(MItem_Step16, NULL, "1/6 tick");
    qMenu->AddItem(MItem_Step13, NULL, "1/3 tick");
    qMenu->AddItem(MItem_Step14, NULL, "1/4 tick");
    qMenu->AddItem(MItem_Step12, NULL, "1/2 tick");
    qMenu->AddItem(MItem_Step, NULL, "Tick");
    qMenu->AddItem(MItem_Beat, NULL, "Beat");
    qMenu->AddItem(MItem_Bar, NULL, "Bar");

    qMenu->SetMainItem(MItem_Beat);
    qMenu->active = true;

    AddNewControl(qMenu, this);
}

void CtrlPanel::CheckVisibility(int xc)
{
    Control* ct = first_ctrl;
    while(ct != NULL)
    {
        if(ct->x - 1 > xc)
        {
            ct->Deactivate();
        }
        else
        {
            if(ct->x + ct->width > xc && !(ct == qMenu && ct == M.active_menu))
            {
                ct->width = xc - ct->x - 1;
            }
            ct->Activate();
        }
        ct = ct->pn_next;
    }

    octaver->CheckVisibility(xc);
    bpmer->CheckVisibility(xc);

    xcc = xc;
}

void CtrlPanel::CheckControlVisibility(Control* ct)
{
    if(ct->x > xcc)
    {
        ct->Deactivate();
    }
    else
    {
        if(ct->x + ct->width > xcc && !(ct == qMenu && ct == M.active_menu))
        {
            ct->width = xcc - ct->x;
        }
        ct->Activate();
    }
}

void CtrlPanel::HandleToggle(Toggle* tg)
{
    if(tg == pattmode)
    {
        //copymode = !copymode;
    }
}

void CtrlPanel::HandleButtDown(Butt* bt)
{
    if(bt == AccBasicMixer)
    {
        MainX1 = InstrPanelWidth + InstrCenterOffset;
        if(MainX2 < MainX1 + 25)
        {
            MainX2 = MainX1 + 25;
        }
        Grid2Main();
        MainX2 = GridX1;
        Grid2Main();
        MixerWidth = WindWidth - MainX2 - 1;

        R(Refresh_All);
    }
    else if(bt == AccBasicGrid)
    {
        MainX1 = InstrPanelWidth + InstrCenterOffset;
        if(MainX2 < MainX1 + 25)
        {
            MainX2 = MainX1 + 25;
        }
        MixerWidth = MixCenterWidth;
        MainX2 = WindWidth - MixerWidth - 1;
        aux_panel->volpan_height = 124;
        MainY2 = WindHeight - aux_panel->volpan_height;
        Grid2Main();

        if(aux_panel->auxmode == AuxMode_Mixer)
        {
            CP->HandleButtDown(CP->view_mixer);
        }

        if(aux_panel->auxmode == AuxMode_Pattern)
        {
            if(aux_panel->last_vpmode == AuxMode_Vols)
            {
                aux_panel->HandleButtDown(aux_panel->Vols);
            }
            else if(aux_panel->last_vpmode == AuxMode_Pans)
            {
                aux_panel->HandleButtDown(aux_panel->Pans);
            }
        }

        R(Refresh_All);
    }
    else if(bt == AccBasicPatt)
    {
        MainX1 = InstrPanelWidth + InstrCenterOffset;
        if(MainX2 < MainX1 + 25)
        {
            MainX2 = MainX1 + 25;
        }

        MixerWidth = MixCenterWidth;
        MainX2 = WindWidth - MixerWidth - 1;
        Grid2Main();

        if(aux_panel->auxmode != AuxMode_Pattern)
        {
            if(aux_panel->workPt->ptype == Patt_Pianoroll)
            {
                aux_panel->HandleButtDown(aux_panel->PtPianorol);
            }
            else if(aux_panel->workPt->ptype == Patt_StepSeq)
            {
                aux_panel->HandleButtDown(aux_panel->PtStepseq);
            }
            else
            {
                aux_panel->HandleButtDown(aux_panel->PtUsual);
            }
            
            aux_panel->auxmode = AuxMode_Pattern;
        }

        if(aux_panel->PattExpand->pressed == false)
        {
            aux_panel->HandleButtDown(aux_panel->PattExpand);
        }

        R(Refresh_All);
    }
    else if(bt == play_butt)
    {
        if(aux_panel->playing == true)
        {
            aux_panel->playing = false;
            aux_panel->playbt->Release();
        }

        PlayMain();
        if(bt->pressed == false)
        {
            bt->pressed = true;
        }
        else
        {
            bt->pressed = false;
        }
    }
    else if(bt == stop_butt)
    {
        StopMain();
        play_butt->pressed = false;
        aux_panel->playbt->pressed = false;

        play_butt->Refresh();
        if(aux_panel->auxmode == AuxMode_Pattern)
        {
            aux_panel->playbt->Refresh();
        }
    }
    else if(bt == Go2Home)
    {
        PosToHome();
    }
    else if(bt == Go2End)
    {
        PosToEnd();
    }
    else if(bt == FFwd)
    {
        double newtick = pbkMain->currTick + ticks_per_beat*beats_per_bar;
        pbkMain->SetCurrTick(newtick);

        UpdatePerBaseFrame(pbkMain);
        UpdateTime(Loc_MainGrid);

        float delta = 0;
        while(pbkMain->currTick > main_bar->offset + main_bar->visible_len + delta)
        {
            delta += ticks_per_beat*beats_per_bar;
        }

        if(delta > 0)
        {
            main_bar->SetDelta(delta);
        }
    }
    else if(bt == Rewind)
    {
        double newtick = pbkMain->currTick - ticks_per_beat*beats_per_bar;
        if(newtick < 0)
        {
            newtick = 0;
        }
        pbkMain->SetCurrTick(newtick);
        UpdatePerBaseFrame(pbkMain);
        UpdateTime(Loc_MainGrid);

        float delta = 0;
        while(pbkMain->currTick < main_bar->offset + delta)
        {
            delta -= ticks_per_beat*beats_per_bar;
        }

        if(delta < 0)
        {
            main_bar->SetDelta(delta);
        }
    }
    else if(bt == IncrScale)
    {
        IncreaseScale(false);
    }
    else if(bt == DecrScale)
    {
        DecreaseScale(false);
    }
    else if(bt == DefaultScaleMax)
    {
        SetScale(14);
    }
    else if(bt == DefaultScaleMin)
    {
        SetScale(2);
    }
    else if(bt == Back)
    {
        main_bar->offset -= 100.0f/tickWidth;
        if(main_bar->offset <= 0)
        {
            main_bar->offset = 0;
        }

        main_bar->actual_offset = main_bar->offset;
        UpdateScrollbarOutput(main_bar);

        if(scrolling == false)
        {
            scrolling = true;
            scrollbt = Back;
            SetTimer(hWnd, SCROLL_TIMER, 400, NULL);
        }
    }
    else if(bt == Forth)
    {
        main_bar->offset += 100.0f/tickWidth;
        main_bar->actual_offset = main_bar->offset;
        UpdateScrollbarOutput(main_bar);

        if(scrolling == false)
        {
            scrolling = true;
            scrollbt = Forth;
            SetTimer(hWnd, SCROLL_TIMER, 400, NULL);
        }
    }
    else if(bt == Config)
    {
        ToggleConfigWindow();
    }
    else if(bt == Render)
    {
        ToggleRenderWindow();
    }
    else if(bt == AccGenBrowsa)
    {
        MainX1 = GenBrowserWidth + InstrPanelWidth + GenBrowserGap - 5 + InstrCenterOffset;
        if(MainX2 < MainX1 + 25)
        {
            MainX2 = MainX1 + 25;
        }
        Grid2Main();

        R(Refresh_All);
    }
    else if(bt == AccInstrPanel)
    {
        MainX1 = InstrPanelWidth + InstrCenterOffset;
        if(MainX2 < MainX1 + 25)
        {
            MainX2 = MainX1 + 25;
        }
        Grid2Main();

        R(Refresh_All);
    }
    else if(bt == UsualMode)
    {
        SwitchInputMode(CMode_TxtDefault);
    }
    else if(bt == NoteMode)
    {
        if(bt->pressed == false)
        {
            SwitchInputMode(CMode_NotePlacing);
        }
        else
        {
            SwitchInputMode(CMode_TxtDefault);
        }
    }
    else if(bt == MetroMode)
    {
        if(bt->pressed == false)
        {
            bt->Press();
            Metronome_ON = true;
        }
        else
        {
            bt->Release();
            Metronome_ON = false;
            barsample->ForceDeactivate();
            beatsample->ForceDeactivate();
        }
    }
    else if(bt == SlideMode)
    {
        SwitchInputMode(CMode_Sliding);
    }
    else if(bt == FastMode)
    {
        SwitchInputMode(CMode_FastMode);
    }
    else if(bt == BrushMode)
    {
        SwitchInputMode(CMode_FastBrushMode);
    }
    else if(bt == BtFollowPlayback)
    {
        if(bt->pressed == true)
        {
            followPos = false;
            bt->Release();
        }
        else
        {
            followPos = true;
            bt->Press();

            if(!(Playing || aux_panel->playing))
            {
                float currtick = Frame2Tick(pbkMain->currFrame_tsync);
                if((currtick < main_bar->offset) ||
                    ((currtick > main_bar->offset + (main_bar->visible_len - main_bar->visible_len/9)) && (main_bar->barpixoffs + main_bar->barpixlen < main_bar->pixlen - 2)))
                {
                    main_bar->SetDelta(currtick - main_bar->offset - main_bar->visible_len + main_bar->visible_len/2);
                }
            }
        }
        //Grid2Main();
    }
    else if (bt == Record)
    {
        if(bt->pressed == false)
        {
            bt->Press();
            if(aux_panel->playing == true)
            {
                // Cancel aux playback by simulating keypress
                //gAux->HandleButtDown(gAux->playbt);
            }
            Recording = true;
        }
        else
        {
            bt->Release();
            StopRecording();
        }
    }
    else if(bt == ShowTrackControls)
    {
        if(bt->pressed == false)
        {
            bt->Press();

            mixBrw->Disable();
            mixBrw->CurrButt->pressed = false;
            mixBrw->CurrButt = (ButtFix*)bt;

            mixbrowse = false;
            mixBrw->sbar->active = false;

            Grid2Main();

            R(Refresh_All);
        }
    }
    else if(bt == mixBrw->HideTrackControls)
    {
        if(bt->pressed == false)
        {
            bt->pressed = true;

            mixBrw->Disable();
            mixBrw->CurrButt->pressed = false;
            mixBrw->CurrButt = (ButtFix*)bt;

            mixbrowse = false;

            Grid2Main();

            R(Refresh_All);
        }
    }
    else if(bt == HotKeys)
    {
        if(HKWnd->Showing() == false)
            HKWnd->Show();
        else
            HKWnd->Hide();
    }
    else if(bt == view_mixer)
    {
        if(bt->pressed == false)
        {
            bt->Press();
            aux_panel->DisableAuxStuff();
            aux_panel->DisablePatternStuff();
            aux_panel->EnableMixStuff();
            lastauxmode = aux_panel->auxmode;
            aux_panel->auxmode = AuxMode_Mixer;

            if(mixBrw->CurrButt != mixBrw->HideTrackControls)
            {
                mixbrowse = true;
                Grid2Main();
                if(mixBrw->brwmode == Browse_Plugins && mixBrw->current_num_entries == 0)
                {
                    // Not updated yet
                    mixBrw->Update();
                }
            }
        }
        else if(bt->pressed == true)
        {
            bt->Release();
            aux_panel->DisableMixStuff();
            mixbrowse = false;
            aux_panel->auxmode = lastauxmode;
            aux_panel->EnableAuxStuff();
            switch(lastauxmode)
            {
                case AuxMode_Vols:
                    //gAux->HandleButtDown(gAux->Vols);
                    break;
                case AuxMode_Pans:
                    //gAux->HandleButtDown(gAux->Pans);
                    break;
                case AuxMode_Pattern:
                    aux_panel->EnablePatternStuff();
                    //gAux->HandleButtDown(gAux->CurrPt);
                    break;
            }
            Grid2Main();
        }
        R(Refresh_All);
    }
    else if(bt == up)
    {
        main_v_bar->SetDelta(-2.f);
    }
    else if(bt == down)
    {
        main_v_bar->SetDelta(2.f);
    }

    bt->Refresh();
    R(Refresh_Buttons);
}

void CtrlPanel::HandleButtUp(Butt* bt)
{
	// Buttons can become inactive
	if(bt->active == true)
	{
		if(bt == minimize)
		{
			MinimizeWindow();
		}
		else if(bt == maximize)
		{
			ToggleFullScreen();
		}
		else if(bt == close)
		{
		    if(!CheckSave())
            {
                ExitFromApp();
            }
		}

		if(scrolling == true)
		{
			scrolling = false;
			KillTimer(hWnd, SCROLL_TIMER);
		}
	}

    bt->Refresh();
	R(Refresh_Buttons);
}

void CtrlPanel::HandleTweak(Control* ct, int mouse_x, int mouse_y)
{
    if(ct == slzoom)
    {
        float scale = slzoom->GetTickWidth();
        SetScale(scale);
    }
}

void CtrlPanel::PosToHome()
{
    pbkMain->SetCurrFrame(0);
    UpdateTime(Loc_MainGrid);
    UpdatePerBaseFrame(pbkMain);
    main_bar->SetDelta(-main_bar->offset);
}

void CtrlPanel::PosToEnd()
{
    tframe lastframe;
    GetLastElementEndFrame(&lastframe);
    float tick = Frame2Tick(lastframe);
    pbkMain->SetCurrFrame(lastframe);
    UpdateTime(Loc_MainGrid);
    UpdatePerBaseFrame(pbkMain);
    float newoffs = tick - main_bar->visible_len/2;
    if(newoffs < 0)
        newoffs = 0;
    main_bar->SetDelta(newoffs - main_bar->offset);
    if(Playing)
    {
        PlayMain(); // Toggle play to stop
        play_butt->Release();
    }
}

void CtrlPanel::UpdateSessionItems()
{
    FileMenu->height = 5;
    MenuItem* mi = FileMenu->first_item;
    while(mi->itemtype != MItem_Custom)
    {
        if(mi->itemtype == MItem_Divider)
            FileMenu->height += DIVIDER_HEIGHT;
        else
            FileMenu->height += MITEM_HEIGHT;

        mi = mi->inext;
    }

    for(int si = 0; si < numLastSessions; si++)
    {
        if(lastsessions[si] != NULL)
        {
            strcpy(mi->title, lastsessions[si]->getFileName());
            FileMenu->height += MITEM_HEIGHT;
        }
        mi = mi->inext;
    }

    if(numLastSessions == 0)
        FileMenu->height -= DIVIDER_HEIGHT;
}

Browser::Browser(const char* dirpath, BrwType t)
{
    type = Panel_Browser;
    btype = t;

    memset(path, 0, MAX_PATH_STRING);
    strcpy(path, dirpath);
    currpath = path;

    memset(samplespath, 0, MAX_PATH_STRING);
    strcpy(samplespath, SAMPLES_PATH);

    memset(projectspath, 0, MAX_PATH_STRING);
    strcpy(projectspath, PROJECTS_PATH);

    memset(rootpath, 0, MAX_PATH_STRING);
    strcpy(rootpath, dirpath);

    memset(lastdir, 0, MAX_NAME_STRING);

    current_fd = NULL;
    current_index = -1;
    entry_first = NULL;
    entry_last = NULL;
    main_offs = 0;
    num_visible = 0;
    current_num_entries = 0;

    prevInstr = NULL;

    sbar = new ScrollBard(Ctrl_VScrollBar, 0, 0, 17);
    AddNewControl(sbar, this);

    if(btype == Brw_Effects)
    {
        HideTrackControls = new ButtFix(false);
        AddNewControl(HideTrackControls, this);
    }
    else
        HideTrackControls = NULL;

    ShowFiles = new ButtFix(btype == Brw_Generators);
    ShowProjects = new ButtFix(false);
    ShowExternalPlugs = new ButtFix(btype == Brw_Effects);
    ShowParams = new ButtFix(false);
    ShowPresets = new ButtFix(false);
    AddNewControl(ShowFiles, this);
    AddNewControl(ShowProjects, this);
    AddNewControl(ShowExternalPlugs, this);
    AddNewControl(ShowParams, this);
    AddNewControl(ShowPresets, this);
    ShowSamples = new ButtFix(false);
    AddNewControl(ShowSamples, this);
    if(btype == Brw_Effects)
    {
        CurrButt = ShowExternalPlugs;
    }
    else
        CurrButt = ShowFiles;

    if(t == Brw_Generators)
        ShowFiles->freeset = ShowSamples->freeset = ShowProjects->freeset = ShowExternalPlugs->freeset = ShowParams->freeset = ShowPresets->freeset = false;

    sortstr = new TString("", true);
    sortstr->panel = this;
    sortstr->highlightable = false;
    Add_PEdit(sortstr);

    SetCurrentListIndex(3);
    brwmode = btype == Brw_Generators ? Browse_Files : Browse_Plugins;
    dirtype = Dir_Files;

    if(!(btype == Brw_Effects && mixbrowse == false))
		Update();
}

Browser::~Browser()
{
    delete sbar;
    free(path);
}

void Browser::Clean()
{
    BrwEntry*   be  = NULL;
    FileData*   fd  = NULL;
    Paramcell*  pd  = NULL;
    PresetData* prd = NULL;

	if(entry_first != NULL)
	{
		be = entry_first;
		while(be != NULL)
		{
		    if(brwmode == Browse_Files)
            {
                fd = (FileData*)(be->entry);
		        delete fd;
            }
            else if(brwmode == Browse_Params)
            {
                pd = (Paramcell*)(be->entry);
                delete pd;
            }
            else if (brwmode == Browse_Presets)
            {
                prd = (PresetData*)(be->entry);
                delete prd;
            }

			if(be->next != NULL)
			{
				be = be->next;
                delete be->prev;
			}
			else
			{
				delete be;
				be = NULL;
			}
		}
		entry_first = NULL;
		entry_last = NULL;
    }

    current_index = -1;
    current_fd = NULL;
    current_prd = NULL;
}

void Browser::Update()
{
    current_num_entries = 0;
    main_offs = 0;
    if(brwmode == Browse_Files || 
       brwmode == Browse_Plugins || 
       brwmode == Browse_Native ||
       brwmode == Browse_Samples ||
	   brwmode == Browse_Projects)
    {
        entry_height = 14;
        UpdateFileData();
    }
    else if(brwmode == Browse_Params)
    {
        entry_height = 24;
        UpdateParamsData();
    }
    else if (brwmode == Browse_Presets)
    {
        entry_height = 14;
        UpdatePresetData();
    }

    if(this->btype == Brw_Generators)
    {
        R(Refresh_GenBrowser);
    }
    else if(this->btype == Brw_Effects)
    {
        R(Refresh_MixCenter);
    }

    strcpy(sortlow, sortstr->string);
    ToLowerCase(sortlow);
}

void Browser::AddEntry(BrwEntry* be)
{
    if(entry_first == NULL && entry_last == NULL)
    {
        be->prev = NULL;
        be->next = NULL;
        entry_first = be;
    }
    else
    {
        entry_last->next = be;
        be->prev = entry_last;
        be->next = NULL;
    }
    entry_last = be;
    current_num_entries++;
}

void Browser::AddDelimiter()
{
    files = new FileData;
    strcpy(files->name, "=============================");
    files->ftype = FType_Delimiter;
    files->dirtype = Dir_Nodir;

    BrwEntry* be = new BrwEntry;
    be->entry = (void*)files;
    AddEntry(be);
}

void Browser::AddDelimiter(const char* str)
{
    BrwEntry* be = new BrwEntry;
    be->entry = NULL;
    strcpy(be->rame, str);
    be->type = BEType_Delimiter;
    AddEntry(be);
}

void Browser::ScanDirForSamples(char *path)
{
    WIN32_FIND_DATA founddata = {0};
    char temp_path[MAX_PATH_STRING];
    FileData* filedata;
    char exttmp[MAX_PATH_STRING];
    char* ext;// = (char*)malloc(MAX_PATH_STRING);
    BrwEntry* be;

    strcpy(temp_path, path);
    strcat(temp_path, "*.*");

    HANDLE fhandle = FindFirstFile(temp_path, &founddata);
    bool with_ext;
    if(fhandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            if(strcmp(founddata.cFileName, ".") != 0 && !(founddata.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
            {
                filedata = new FileData;
                strcpy(filedata->name, founddata.cFileName);
                strcpy(filedata->path, currpath);
                strcat(filedata->path, filedata->name);
                filedata->size = (founddata.nFileSizeHigh*(MAXDWORD)) + founddata.nFileSizeLow;
                filedata->attrs = founddata.dwFileAttributes;

                with_ext = false;
                ext = &filedata->name[strlen(filedata->name)];
                while(*ext != 0x2E && ext != filedata->name)
                {
                    *ext--;
                }
                if(ext != filedata->name)
                {
                    with_ext = true;
                    ext++;
                }

                if(founddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    char dir_path[MAX_PATH_STRING];
                    if(strcmp(founddata.cFileName, ".") != 0 && 
                       strcmp(founddata.cFileName, "..") != 0)
                    {
                        sprintf(dir_path, "%s%s%s", path, founddata.cFileName, "\\");
                        ScanDirForSamples(dir_path);
                    }
                }
                else if(with_ext == true)
                {
                    filedata->dirtype = Dir_Nodir;
                    strcpy(exttmp, ext);
                    ToLowerCase(exttmp);
                    if(strcmp(exttmp, WavExt) == 0)
                    {
                        filedata->ftype = FType_Wave;

                        // indexes will be defined in drawing routine
                        //filedata->list_index = cur_index++;
                        filedata->list_index = -1;
                        be = new BrwEntry;
                        be->entry = (void*)filedata;

                        AddEntry(be);
                    }
                }
            }
        }while(FindNextFile(fhandle, &founddata));
        FindClose(fhandle);
    }
}

void Browser::UpdateFileData()
{
    WIN32_FIND_DATA founddata = {0};
    FileData* filedata;
    int cur_index = 0;

    Clean();

    /*
    char* currDir = NULL;
    {
        //First, lets check how long is our current working path
        int length = ::GetCurrentDirectory(0, NULL);
        if (length != 0)
        {
            currDir = (char*)malloc(length);
            ::GetCurrentDirectory(length, currDir);
        }
    }
    */

    SetCurrentDirectory(szWorkingDirectory);

    if(strcmp(path, rootpath) == 0)
    {
        at_root = true;
    }
    else
    {
        at_root = false;
    }

    BrwEntry* be;
    char* ext;// = (char*)malloc(MAX_PATH_STRING);
    char exttmp[MAX_PATH_STRING];
    char temp_path[MAX_PATH_STRING];

    if(brwmode == Browse_Files)
    {
        if(strcmp(path, "--") == 0)
        {
            long drv = GetLogicalDrives();
            long check = 1;
            int num = 1;
            char letter = 0x41;
			char lstr[4] = {0, ':', '\\', 0};
            while(num < 32)
            {
                if(check & drv)
                {
                    filedata = new FileData;
					lstr[0] = letter;
                    strcpy(filedata->name, lstr);
                    filedata->ftype = FType_DiskDrive;

                    be = new BrwEntry;
                    be->entry = (void*)filedata;
                    AddEntry(be);
                }
                letter++;
                check *= 2;
                num++;
            }
        }
        else
        {
            currpath = path;
            strcpy(temp_path, currpath);
            strcat(temp_path, "*.*");

            filedata = new FileData;
            strcpy(filedata->name, "--");
            filedata->ftype = FType_DiskSelector;

            be = new BrwEntry;
            be->entry = (void*)filedata;
            AddEntry(be);

            char strfind[MAX_PATH_STRING];
            strcpy(strfind, temp_path);
            char* cstr = strfind;
            char* kstr = cstr;
            cstr = strchr(cstr, '\\');
            while(cstr != NULL)
            {
               *cstr = NULL;
                filedata = new FileData;
                strcpy(filedata->name, kstr);
                filedata->ftype = FType_LevelDirectory;

                be = new BrwEntry;
                be->entry = (void*)filedata;
                AddEntry(be);

                cstr++;
                kstr = cstr;
                cstr = strchr(cstr, '\\');
            }

            //AddDelimiter();

            HANDLE fhandle = FindFirstFile(temp_path, &founddata);
            bool with_ext;
            if(fhandle != INVALID_HANDLE_VALUE)
            {
                do
                {
                    if(strcmp(founddata.cFileName, ".") != 0 && 
                        strcmp(founddata.cFileName, "..") != 0 && 
                                !(founddata.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
                    {
                        filedata = new FileData;
                        strcpy(filedata->name, founddata.cFileName);;
                        strcpy(filedata->path, currpath);
                        strcat(filedata->path, filedata->name);
                        filedata->size = (founddata.nFileSizeHigh*(MAXDWORD)) + founddata.nFileSizeLow;
                        filedata->attrs = founddata.dwFileAttributes;

                        with_ext = false;
                        ext = &filedata->name[strlen(filedata->name)];
                        while(*ext != 0x2E && ext != filedata->name)
                        {
                            *ext--;
                        }
                        if(ext != filedata->name)
                        {
                            with_ext = true;
                            ext++;
                        }

                        if(founddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                        {
                            filedata->ftype = FType_Directory;
                            filedata->dirtype = Dir_Usual;
                        }
                        else if(with_ext == true)
                        {
                            filedata->dirtype = Dir_Nodir;
                            strcpy(exttmp, ext);
                            ToLowerCase(exttmp);
                            if(strcmp(exttmp, WavExt) == 0)
                            {
                                filedata->ftype = FType_Wave;
                            }
                            else if (strcmp(exttmp, VstExt) == 0)
                            {
                                filedata->ftype = FType_VST;
                            }
                            else if (strcmp(exttmp, ProjExt) == 0)
                            {
                                filedata->ftype = FType_Projects;
                            }
                            else
                            {
                                filedata->ftype = FTYPE_UNKNOWN;
                            }
                        }
                        else
                        {
                            filedata->ftype = FTYPE_UNKNOWN;
                        }

                        // indexes will be defined in drawing routine
                        //filedata->list_index = cur_index++;
                        filedata->list_index = -1;
                        be = new BrwEntry;
                        be->entry = (void*)filedata;
                        AddEntry(be);
                    }
                }while(FindNextFile(fhandle, &founddata));
                FindClose(fhandle);
            }
        }
    }
    else if(brwmode == Browse_Samples)
    {
        currpath = samplespath;
        ScanDirForSamples(currpath);
    }
    else if(brwmode == Browse_Projects)
    {
        currpath = projectspath;
        strcpy(temp_path, currpath);
        strcat(temp_path, "\\*.*");

        HANDLE fhandle = FindFirstFile(temp_path, &founddata);
        bool with_ext;
        if (fhandle != INVALID_HANDLE_VALUE)
        {
            do
            {
                if(strcmp(founddata.cFileName, ".") != 0 && !(founddata.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
                {
                    filedata = new FileData;
                    strcpy(filedata->name, founddata.cFileName);
                    strcpy(filedata->path, currpath);
                    strcat(filedata->path, filedata->name);
                    filedata->size = (founddata.nFileSizeHigh*(MAXDWORD)) + founddata.nFileSizeLow;
                    filedata->attrs = founddata.dwFileAttributes;

                    with_ext = false;
                    ext = &filedata->name[strlen(filedata->name)];
                    while(*ext != 0x2E && ext != filedata->name)
                    {
                        *ext--;
                    }
                    if(ext != filedata->name)
                    {
                        with_ext = true;
                        ext++;
                    }

                    if(!(founddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && with_ext == true)
                    {
                        filedata->dirtype = Dir_Nodir;
                        strcpy(exttmp, ext);
                        ToLowerCase(exttmp);
                        if(strcmp(exttmp, ProjExt) == 0)
                        {
                            filedata->ftype = FType_Projects;

                            // Remove extension to display only names in list
                           *(ext - 1) = 0;

                            // indexes will be defined in drawing routine
                            //filedata->list_index = cur_index++;
                            filedata->list_index = -1;
                            be = new BrwEntry;
                            be->entry = (void*)filedata;

                            AddEntry(be);
                        }
                    }
                }
            }while(FindNextFile(fhandle, &founddata));
            FindClose(fhandle);
        }
    }
    else if(brwmode == Browse_Plugins)
    {
        File vstfile(PLUGIN_LIST_FILENAME);
        bool vstfileexists = vstfile.exists();
        if(firsttime_plugs_scanned == false)
        {
            firsttime_plugs_scanned = true;
            if(RescanPluginsAutomatically == true || vstfileexists == false)
            {
                out_from_dialog = true;
                RescanPlugins(false, false);
            }
            else if(vstfileexists)
            {
                ReadPluginsFromFile();
            }
        }

        EffListEntry_t *pEntry = pPluginList->pFirst;
        while (pEntry != NULL)
        {
            if( ((pEntry->category == EffCategory_Generator)&& (btype == Brw_Generators)) ||
                ((pEntry->category == EffCategory_Effect)&& (btype == Brw_Effects)) )
            {
                filedata = new FileData;
                if(pEntry->type == EffType_VSTPlugin) // temp hack
                {
                    filedata->ftype = FType_VST;
                }
                else
                {
                    filedata->ftype = FType_Native;
                }

                filedata->dirtype = Dir_Nodir;
                strcpy(filedata->name, pEntry->name);
                strcpy(filedata->path, pEntry->path);
                filedata->size = 0;
                filedata->attrs = 0;
                //filedata->list_index = ++cur_index;
                filedata->list_index = -1;

                be = new BrwEntry;
                be->entry = (void*)filedata;

                AddEntry(be);
            }
            pEntry = pEntry->Next;
        }
        // Update with VST plugs
    }

    //if(currDir != NULL)
    //   ::SetCurrentDirectory(currDir);
}

void Browser::GrabParamsFromModule(ParamModule* pmodule, char* name)
{
    Parameter* param = pmodule->first_param;
    while(param != NULL)
    {
        if(param->type == Param_Bool)
        {/*
            pc = new Paramcell(PCType_MixToggle, param, this, scope);
            be = new BrwEntry;
            be->entry = (void*)pc;
            AddEntry(be);
         */
        }
        else
        {
            Paramcell* pc = new Paramcell(PCType_MixSlider, param, this, &pmodule->scope);
            char ttl[MAX_NAME_STRING];
            strcpy(ttl, name);
            if(strlen(name) > 0)
                strcat(ttl, ".");
            strcat(ttl, param->name);
            pc->SetTitle(ttl);
            BrwEntry* be = new BrwEntry;
            be->entry = (void*)pc;
            AddEntry(be);
        }
        param = param->next;
    }
}

void Browser::ProcessModuleParams(ParamModule * pmodule, char* name)
{
    ParamModule* module = pmodule;
    GrabParamsFromModule(module, name);

    char cname[MAX_NAME_STRING];
    module = module->children;
    // Rewind to the end to add kids in proper order
    while(module != NULL && module->next_sibling != NULL)
        module = module->next_sibling;
    while(module != NULL)
    {
        strcpy(cname, name);
        if(strlen(name) > 0)
        {
            strcat(cname, ".");
        }
        strcat(cname, module->name);
        ProcessModuleParams(module, cname);
        module = module->prev_sibling;
    }
}

void Browser::UpdateParamsData()
{
    ParamModule* module = NULL;
    Parameter* param = NULL;
    Paramcell* pc = NULL;;
    BrwEntry*  be = NULL;;
    Scope*     scope;

    Clean();
    if (this->btype == Brw_Generators)
    {
        if(current_instr != NULL)
        {
            module = current_instr;
            if(current_instr->type == Instr_VSTPlugin)
            {
                module = ((VSTGenerator*)(current_instr))->pEff;
            }
            scope = &current_instr->scope;
        }
    }
    else if (this->btype == Brw_Effects)
    {
        if(aux_panel->current_eff != NULL)
        {
            module = aux_panel->current_eff;
            scope = &aux_panel->current_eff->scope;
        }

        /*
        if(mix->current_mixcell->effect != NULL)
        {
            eff = mix->current_mixcell->effect;
            scope = &mix->current_mixcell->scope;
        }
        */
    }

    // Grab from module, if any
    if(module != NULL)
    {
        if(module->first_pcell != NULL)
        {
            Paramcell* pcnew;
            pc = module->first_pcell;
            while(pc != NULL)
            {
                pcnew = pc->Clone(this);
				if(pcnew->param != NULL)
					pcnew->SetTitle(pcnew->param->name);
                be = new BrwEntry;
                be->entry = (void*)pcnew;
                AddEntry(be);

                pc = pc->next;
            }
        }
        else
        {
            char name[MAX_NAME_STRING];
            strcpy(name, "");
            ProcessModuleParams(module, name);
        }
    }
}

void Browser::UpdatePresetData()
{
    ParamModule* module = NULL;
    PresetData* prd = NULL;;
    BrwEntry*  be = NULL;;
    Preset* pPreset = NULL;;

    Clean();

    if (this->btype == Brw_Generators)
    {
        if(current_instr != NULL && current_instr->type != Instr_Sample)
        {
            if(current_instr->type == Instr_VSTPlugin)
            {
                module = ((VSTGenerator*)(current_instr))->pEff;
            }
            else
            {
				module = current_instr;
                // Rescan presets list since it can be changed outside of this module
                module->DeletePresets();
                module->ScanForPresets();
            }
        }
    }
    else if (this->btype == Brw_Effects)
    {
        if(aux_panel->current_eff != NULL)
        {
            module = aux_panel->current_eff;
			if(aux_panel->current_eff->type != EffType_VSTPlugin)
			{
                // Rescan presets list since it can be changed outside of this module
				module->DeletePresets();
				module->ScanForPresets();
			}
		}

        /*
        if(mix->current_mixcell->effect != NULL)
        {
            eff = mix->current_mixcell->effect;
        }
        */
    }

    if(module != NULL)
    {
        int cur_index = 0;
        /* Let's load Native first */
        pPreset = module->NativePresets;
        while(pPreset != NULL)
        {
            prd = new PresetData;
            be = new BrwEntry;

            prd->index = pPreset->index;
            prd->pPreset = pPreset;
            prd->list_index = -1;
            strcpy(prd->name, pPreset->name);

            be->type = BEType_Common;
            be->entry = (void*)prd;
            AddEntry(be);

            if (pPreset == module->CurrentPreset)
            {
                this->current_prd = prd;
            }

            pPreset = pPreset->next;
        }

        UpdateCurrentHighlight();
    }
}
void Browser::UpdateCurrentHighlight()
{
    switch(this->brwmode)
    {
        case Browse_Params:
        {
        }break;
        case Browse_Presets:
        {
            Eff* eff = NULL;
            if(this->btype == Brw_Effects)
            {
				eff = aux_panel->current_eff;
                //eff = mix->current_mixcell->effect;
            }
            else if(this->btype == Brw_Generators)
            {
                if(current_instr != NULL && current_instr->type == Instr_VSTPlugin)
                    eff = ((VSTGenerator*)current_instr)->pEff;
            }

            if(eff != NULL)
            {
                PresetData* prd;
                BrwEntry* be = entry_first;
                while(be != NULL)
                {
                    prd = (PresetData*)be->entry;
                    if (prd->pPreset == eff->CurrentPreset)
                    {
                        this->current_index = prd->list_index;
                        break;
                    }

                    be = be->next;
                }
            }
        }break;
        /*
        case Browse_Files:
        {
            if (this->btype == Brw_Effects)
            {
                BrwEntry* be = entry_first;
                while(be != NULL)
                {
                    if( (((FileData*)(be->entry))->ftype == FType_VST) &&
                        (0 == _stricmp(((FileData*)(be->entry))->path, mix->current_mixcell->effect->path)) )
                    {
                        current_index = ((FileData*)(be->entry))->list_index;
                        break;
                    }
                    be = be->next;
                }
            }
            else if (this->btype == Brw_Generators)
            {
                BrwEntry* be = entry_first;
                while(be != NULL)
                {
                    if( (((FileData*)(be->entry))->ftype == FType_VST) &&
                        (current_instr->type == Instr_VSTPlugin) &&
                        (0 == _stricmp(((FileData*)(be->entry))->path, ((VSTGenerator*)(current_instr))->pEff->path)) )
                    {
                        current_index = ((FileData*)(be->entry))->list_index;
                        break;
                    }
                    be = be->next;
                }
            }
        }break;
        */

        default:
            break;
    }

    if (this->btype == Brw_Generators)
    {
        R(Refresh_GenBrowser);
    }
    else if (this->btype == Brw_Effects)
    {
        R(Refresh_MixCenter);
    }
}

void Browser::SetCurrentFileDataByIndex(int index)
{
    BrwEntry* be = entry_first;
    while(be != NULL)
    {
        if(((FileData*)(be->entry))->list_index == index)
        {
            current_fd = (FileData*)(be->entry);
            current_index = index;
            break;
        }
        be = be->next;
    }
}

void Browser::SetCurrentFileDataByName(char* name)
{
    BrwEntry* be = entry_first;
    while(be != NULL)
    {
        if(strcmp(((FileData*)(be->entry))->name, name) == 0 && ((FileData*)(be->entry))->ftype != FType_LevelDirectory)
        {
            current_fd = (FileData*)(be->entry);
            current_index = -1;
            break;
        }
        be = be->next;
    }
}

void Browser::SetCurrentPresetDataByIndex(int index)
{
    BrwEntry* be = entry_first;

    while(be != NULL)
    {
		if(be->type == BEType_Common)
		{
			if(((PresetData*)(be->entry))->list_index == index)
			{
				current_prd = (PresetData*)(be->entry);
				current_index = index;
				break;
			}
		}
        be = be->next;
    }
}

FileData* Browser::GetFileDataByIndex(int index)
{
    BrwEntry* be = entry_first;
    while(be != NULL)
    {
        if(((FileData*)(be->entry))->list_index == index)
        {
            return ((FileData*)(be->entry));
        }
        be = be->next;
    }
    return NULL;
}

PresetData* Browser::GetPresetDataByIndex(int index)
{
    BrwEntry* be = entry_first;
    while(be != NULL)
    {
		if(be->type == BEType_Common)
		{
			if(((PresetData*)(be->entry))->list_index == index)
			{
				return ((PresetData*)(be->entry));
			}
		}
        be = be->next;
    }
    return NULL;
}

void Browser::SetCurrentListIndex(int index)
{
    current_index = index;
}

void Browser::ActivateEntry(FileData* fd)
{
    if(fd->ftype == FType_Directory)
    {
        ResetSortString();

        main_offs = 0;
        if(strcmp(fd->name, "..") == 0) // going to the parent directory
        {
            // Find the latest directory in path
            char* sc;
            char* last_sc;
            sc = last_sc = path;
            while(sc != NULL)
            {
                sc = strchr(sc, '\\');
                if(sc != NULL)
                {
                    last_sc = sc;
                    sc++;
                }
            }

			last_sc--;
			while(*last_sc != 0x5C)
			{
				last_sc--;
			}

            if(last_sc != path)
            {
                last_sc++;
               *last_sc = 0;
				UpdateFileData();

                if(strlen(lastdir) != 0)
                {
                    SetCurrentFileDataByName(lastdir);
                }
                else
                {
                    SetCurrentListIndex(fd->list_index - 1);
                }

                last_sc--;
                last_sc--;
                while(*last_sc != 0x5C && last_sc != path)
                {
                    last_sc--;
                }
				last_sc++;
                strcpy(lastdir, last_sc);
				lastdir[strlen(lastdir) - 1] = 0;
            }
        }
        else
        {
            char* sc;
			char* last_sc;
            sc = last_sc = path;
            while(sc != NULL)
            {
                sc = strchr(sc, '\\');
                if(sc != NULL)
                {
                    last_sc = sc;
                    sc++;
                }
            }

            if(last_sc != path)
            {
                strcpy(lastdir, fd->name);
                last_sc++;
               *last_sc = 0;
                strcat(path, fd->name);
                strcat(path, "\\");
				UpdateFileData();
                SetCurrentFileDataByName("..");
           }
        }
    }
    else if(fd->ftype == FType_LevelDirectory)
    {
        char* sc = strstr(path, fd->name);
        if(sc != NULL)
        {
            sc = strchr(sc, '\\');
            if(sc != NULL)
            {
               *sc = 0;
                strcat(path, "\\");
                UpdateFileData();
                SetCurrentFileDataByName("..");
            }
        }
    }
    else if(fd->ftype == FType_DiskDrive)
    {
        strcpy(path, fd->name);
        UpdateFileData();
    }
    else if(fd->ftype == FType_DiskSelector)
    {
        strcpy(path, "--");
        UpdateFileData();
    }
    else if ((btype == Brw_Generators)&&(fd->ftype == FType_VST || 
                                         fd->ftype == FType_Wave || 
                                         fd->ftype == FType_Native))
    {
        IP->AddInstrumentFromBrowser(fd, true);
        M.lastclick = LastClicked_Instrument;
    }
    else if ((btype == Brw_Generators)&&(fd->ftype == FType_Projects))
    {
        LoadProject(&(File(fd->path)));
    }
    else if ((btype == Brw_Effects)&&(fd->ftype == FType_VST || fd->ftype == FType_Native))
    {
        //mix->AddEffectFromBrowser(fd, mix->current_mixcell);
    }
}

void Browser::ActivateEntry(PresetData* prd)
{
    if(prd != NULL)
    {
        ParamModule *module = (ParamModule*)(prd->pPreset->owner);
        module->SetPresetByName(prd->name);
    }
}

void Browser::ResetSortString()
{
    sortstr->SetString("");
    sortlow[0] = 0;
}

void Browser::HandleButtDown(Butt* butt)
{
    if(butt != CurrButt)
    {
        CurrButt->Release();
        butt->Press();
        CurrButt = (ButtFix*)butt;

        ResetSortString();

        if(this == mixBrw)
        {
            if(mixbrowse == false)
            {
                mixbrowse = true;
                Grid2Main();

                R(Refresh_All);
            }
            else
            {
                R(Refresh_MixCenter);
            }
            sbar->active = true;
        }
        else if(this == genBrw)
        {
            R(Refresh_GenBrowser);
        }

        Clean();

        if(butt == ShowParams)
        {
            brwmode = Browse_Params;
        }
        else if(butt == ShowPresets)
        {
            brwmode = Browse_Presets;
        }
        else if(butt == ShowFiles)
        {
            brwmode = Browse_Files;
        }
        else if(butt == ShowProjects)
        {
            brwmode = Browse_Projects;
        }
        else if(butt == ShowNativePlugs)
        {
            brwmode = Browse_Native;
        }
        else if(butt == ShowExternalPlugs)
        {
            brwmode = Browse_Plugins;
        }
        else if(butt == ShowSamples)
        {
            brwmode = Browse_Samples;
        }

        Update();
    }

    R(Refresh_Buttons);
}

void Browser::HandleButtUp(Butt* bt)
{
    bt->Refresh();

    R(Refresh_Buttons);
}

void Browser::PreviewSample(FileData* fdi)
{
    if(fdi->ftype == FType_Wave)
    {
        char fullpath[MAX_PATH_STRING];
        strcpy(fullpath, fdi->path);
        char alias[MAX_ALIAS_STRING];
        char name[MAX_NAME_STRING];
        strcpy(name, fdi->name);
        ToLowerCase(name);
        GetOriginalInstrumentAlias(name, alias);
        prevInstr = (Instrument*)Add_Sample(fullpath, fdi->name, alias, true);
		if(prevInstr != NULL)
		{
			// Play preview then
			Preview_Add(NULL, prevInstr, -1, -1, NULL, NULL, NULL);
		}
    }
}

void Browser::MouseClick(int brwindex, bool dbclick, bool down, bool left)
{
    if(down)
    {
        if(brwmode == Browse_Files || 
           brwmode == Browse_Plugins ||
           brwmode == Browse_Samples ||
           brwmode == Browse_Projects)
        {
            FileData* fd = GetFileDataByIndex(brwindex);
            if(fd != NULL && fd->ftype != FType_Delimiter)
            {
                if(brwmode == Browse_Files && (fd->ftype == FType_DiskDrive ||
                                                fd->ftype == FType_DiskSelector ||
                                                fd->ftype == FType_LevelDirectory))
                {
                    ActivateEntry(fd);
                }
                else if(dbclick == true && current_index == brwindex)     // double click
                {
                    ActivateEntry(fd);
                }
                else
                {
                    SetCurrentFileDataByIndex(brwindex);
                    if(brwmode == Browse_Samples && left)
                    {
                        PreviewSample(fd);
                    }
                }
            }
        }
        else if (brwmode == Browse_Presets)
        {
            SetCurrentPresetDataByIndex(brwindex);
            PresetData* prd = GetPresetDataByIndex(brwindex);
            ActivateEntry(prd);

    /*      if(dbclick == true && current_index == M.brwindex)
            {
                PresetData* prd = GetPresetDataByIndex(brwindex);
                ActivateEntry(prd);
            }
            else
            {
                SetCurrentPresetDataByIndex(brwindex);
            }
    */
        }

        if(btype == Brw_Generators)
        {
            R(Refresh_GenBrowser);
        }
        else if(btype == Brw_Effects)
        {
            R(Refresh_MixCenter);
        }
    }
}

bool Browser::ItemIsVisibleWithSorting(char * itemname)
{
    char itemlow[MAX_NAME_STRING];
    strcpy(itemlow, itemname);
    ToLowerCase(itemlow);
    if(strstr(itemlow, sortlow) == itemlow)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Browser::SetViewMask(unsigned int vmask)
{
    // Defines mask filter for supported files
    viewmask = vmask;
}

void Browser::Enable()
{
    active = true;

    sbar->Activate();
    sortstr->Activate();
    ShowFiles->Activate();
    ShowParams->Activate();
    ShowPresets->Activate();
    ShowExternalPlugs->Activate();
    ShowProjects->Activate();
    ShowSamples->Activate();

    if(brwmode == Browse_Params)
    {
        Paramcell* pd;
        BrwEntry* be = entry_first;
        while(be != NULL)
        {
            pd = (Paramcell*)be->entry;
            pd->Enable();

            be = be->next;
        }
    }
}

void Browser::Disable()
{
    active = false;

    sbar->Deactivate();
    sortstr->Deactivate();
    sortstr->SetString("");
    if(C.curParam == sortstr)
    {
        C.ExitToCurrentMode();
    }

    if(btype != Brw_Effects)
    {
        ShowFiles->Deactivate();
        ShowParams->Deactivate();
        ShowPresets->Deactivate();
        ShowExternalPlugs->Deactivate();
        ShowProjects->Deactivate();
        ShowSamples->Deactivate();
    }

    if(brwmode == Browse_Params)
    {
        Paramcell* pd;
        BrwEntry* be = entry_first;
        while(be != NULL)
        {
            pd = (Paramcell*)be->entry;
            pd->Disable();

            be = be->next;
        }
    }
}

Mixcell::Mixcell()
{
    type = Panel_MixCell;
    auxcell = false;
    memset(&scope, 0, sizeof(Scope));
    scope.mixcell = this;
    in_first = NULL;
    in_last = NULL;
    in_prev = NULL;
    in_next = NULL;
    working_prev = NULL;
    working_next = NULL;
    check_next = NULL;
    outcell = NULL;
    vol_slider = NULL;
    pan_slider = NULL;
    level_slider = NULL;
    num_inputs = 0;
    num_active_inputs = 0;
    active_inputs_count = 0;
    buff_new = true;
    queued = false;
    workqueued = false;
    working = false;
    mutecount = 0;
    col = 0;
    row = 0;
    VULC = 0;
    VURC = 0;
    height = MixCellBasicHeight;
    master = false;
    grouper = false;
    memset(&indexstr, 0, 4);
    mixstr = NULL;
    effect = NULL;
    folded = true;
    fold_toggle = new Toggle(&folded, &scope);
    AddNewControl(fold_toggle, this);
    this->bypass = false;
    this->bypass_toggle = new Toggle(this, Toggle_MixBypass);
    this->bypass_toggle->owner = (Object*)this;
    this->bypass_toggle->SetOnClickHandler(&OnBypassClick);
    AddNewControl(this->bypass_toggle, (Panel*)this);
    drawarea = new DrawArea((void*)this, Draw_Mixcell);
    MC->AddDrawArea(drawarea);
}

void Mixcell::UpdateIndexStr()
{
    if(auxcell)
    {
        strcpy(indexstr, mchan->indexstr);
    }
    else if(grouper == false)
    {
        if(col < 10)
        {
            indexstr[0] = col + 0x30;
        }
        else
        {
            indexstr[0] = col - 10 + 0x41;
        }
        if(row < 10)
        {
            indexstr[1] = row + 0x30;
        }
        else
        {
            indexstr[1] = row - 10 + 0x41;
        }
        indexstr[2] = 0;
    }
    else
    {
        strcpy(indexstr, "bus");
        if(col < 10)
        {
            indexstr[3] = col + 0x30;
        }
        else
        {
            indexstr[3] = col - 10 + 0x41;
        }
        indexstr[4] = 0;
    }
}

void Mixcell::Disable()
{
    if(active == true)
    {
        active = false;
        drawarea->Disable();

		if(mixstr != NULL)
			mixstr->Deactivate();

        Control* ctrl = first_ctrl;
        while(ctrl != NULL)
        {
            // Here deactivate only those controls, that belong directly to the mixcell
            if(ctrl->scope != NULL && ctrl->scope->eff == NULL)
                ctrl->Deactivate();
            ctrl = ctrl->pn_next;
        }

		if(vol_slider != NULL)
            vol_slider->vu->drawarea->Disable();

        inpin->Deactivate();
        outpin->Deactivate();

        if(grouper == false)
        {
            if(effect != NULL)
            {
                effect->Disable();
            }
        }
    }
}

void Mixcell::Enable()
{
    if(active == false)
    {
        active = true;
        drawarea->Enable();

        mixstr->Activate();

        Control* ctrl = first_ctrl;
        while(ctrl != NULL)
        {
            if(ctrl->scope != NULL && ctrl->scope->eff == NULL)
                ctrl->Activate();
            ctrl = ctrl->pn_next;
        }

		if(vol_slider != NULL)
            vol_slider->vu->drawarea->Enable();

        inpin->Activate();
        outpin->Activate();

        if(grouper == false)
        {
            if(effect != NULL)
            {
                effect->Enable();
            }
    		else
            {
                bypass_toggle->Deactivate();
                fold_toggle->Deactivate();
            }
        }
    }
}

void Mixcell::HandleToggle(Toggle* tg)
{
    if(tg == fold_toggle)
    {
        if(folded == true)
        {
            effect->Disable();
        }
        else
        {
            effect->Enable();
        }

        R(Refresh_Mixer);
    }
	else if(tg == bypass_toggle)
	{
		if(bypass == false && effect != NULL)
		{
			effect->Reset();
		}
	}
}

InstrPanel::InstrPanel()
{
    main_offs = 0;
    sbar = new ScrollBard(Ctrl_VScrollBar, 0, 0, 18);
    AddNewControl(sbar, this);

    sbgodown = false;
/*
    fold_all = new Butt(false);
    fold_all->coverable = false;
    AddNewControl(fold_all, this);
    expand_all = new Butt(false);
    expand_all->coverable = false;
    AddNewControl(expand_all, this);
*/

    shut_all_windows = new Butt(false);
    shut_all_windows->coverable = false;
    AddNewControl(shut_all_windows, this);

    edit_autopattern = new Butt(false);
    edit_autopattern->coverable = false;
    AddNewControl(edit_autopattern, this);
}

void InstrPanel::EditInstrumentAutopattern(Instrument * instr)
{
    if(aux_panel->auxmode != AuxMode_Pattern)
    {
        aux_panel->EnablePatternStuff();
    }

    // Place cursor at the previously edited pattern
    if(C.loc == Loc_SmallGrid && aux_panel->workPt != aux_panel->blankPt)
    {
        aux_panel->workPt->Activate();
        C.loc = Loc_MainGrid;
        C.patt = field_pattern;
        CLine = aux_panel->workPt->track_line;
        CTick = aux_panel->workPt->start_tick;
    }

    if(instr->autoPatt == NULL)
    {
        instr->CreateAutoPattern();
    }
    aux_panel->EditPattern(instr->autoPatt);
    UpdateElementsVisibility();
    R(Refresh_Aux);
}

void InstrPanel::HandleButtDown(Butt* bt)
{
    if(bt == fold_all)
    {
        main_offs = 0;
        Instrument* i = first_instr;
        while(i != NULL)
        {
            i->folded = true;
            i = i->next;
        }
        R(Refresh_InstrPanel);
    }
    else if(bt == expand_all)
    {
        main_offs = 0;
        Instrument* i = first_instr;
        while(i != NULL)
        {
            i->folded = false;
            i = i->next;
        }
        R(Refresh_InstrPanel);
    }
    else if(bt == edit_autopattern)
    {
        if(current_instr != NULL)
        {
            EditInstrumentAutopattern(current_instr);
        }
    }
    else if(bt->instr != NULL)
    {
		bt->instr->instr_drawarea->Change();
    }

    bt->Refresh();

    R(Refresh_Buttons);
}

void InstrPanel::HandleButtUp(Butt* bt)
{
    bt->Refresh();
	if(bt->instr != NULL)
    {
		bt->instr->instr_drawarea->Change();
    }

    R(Refresh_Buttons);
}

void InstrPanel::Click(int mouse_x, int mouse_y, bool dbclick, unsigned flags)
{
    if(!(flags & kbd_ctrl) && 
        !(M.mmode & MOUSE_CONTROLLING && M.active_ctrl != M.active_instr->pEditorButton) &&
         !(M.active_paramedit != NULL))
    {
        ChangeCurrentInstrument(M.active_instr);

        if(M.active_instr != NULL && 
            mouse_x > M.active_instr->x + 3 && mouse_y > M.active_instr->y + 18 &&
            mouse_x < M.active_instr->x + 3 + 27 && mouse_y < M.active_instr->y + 39)
        {
            Preview_Add(NULL, M.active_instr, -1, -1, NULL, NULL, NULL);
        }
        else if(flags == 0 && dbclick == true && M.active_ctrl == NULL)
        {
            if(M.active_instr->type == Instr_VSTPlugin)
                M.active_instr->ToggleParamWindow();
        }

        if(flags & kbd_shift && flags & kbd_alt && 
            M.active_instr != NULL && M.active_instr->type == Instr_Sample)
        {
            Sample* sm = (Sample*)M.active_instr;
            sm->DumpData();
        }

        if(flags & kbd_shift && Num_Selected > 0)
        {
            // If any instance is selected - change its instrument
            Instance* ii;
            Element* newel;
            Element* el = firstElem;
            while(el != NULL)
            {
                if(el->selected == true && 
                    el->IsPresent() && 
                    el->IsInstance() &&
                    el->patt->ibound == NULL)
                {
                    ii = (Instance*)el;
                    if(ii->instr != M.active_instr)
                    {
                        el = el->next;
                        newel = ReassignInstrument(ii, M.active_instr);
                        newel->Select();
                        continue;
                    }
                }
                el = el->next;
            }

            UpdateScaledImages();

            R(Refresh_GridContent);
            if(M.selloc == Loc_SmallGrid)
            {
                R(Refresh_AuxContent);
            }
        }
    }
    else if((flags & kbd_ctrl) && M.active_ctrl == NULL)
    {
        Preview_Add(NULL, M.active_instr, -1, -1, NULL, NULL, NULL);
    }
}

int InstrPanel::GetFullLHeight()
{
    int fulllen = 20;
    int instroffs;
    Instrument* i = first_instr;
    while(i != NULL)
    {
        if(i->temp == false)
        {
            if(i->folded == true)
            {
                instroffs = InstrFoldedHeight;
            }
            else
            {
                instroffs = InstrUnfoldedHeight;
            }

            fulllen += instroffs;
            fulllen += 1;
        }
        i = i->next;
    }

    return fulllen;
}

void InstrPanel::CheckGoUp()
{
	float visible_len = (float)(InstrPanelHeight - brw_heading);
	int fulllen = GetFullLHeight();
    if/*(fulllen <= sbar->visible_len)*/(fulllen <= visible_len)
    {
        //sbar->offset = sbar->actual_offset = 0;
        //main_offs = -(int)(sbar->offset);
        main_offs = 0;
    }
    else if/*(sbar->offset + sbar->visible_len > fulllen)*/(main_offs + visible_len > fulllen)
    {
        //sbar->offset = sbar->actual_offset = fulllen - sbar->visible_len;
        //main_offs = -(int)(sbar->offset);
        main_offs = (int)(fulllen - visible_len);
    }
}

void InstrPanel::GoDown()
{
	float visible_len = (float)(InstrPanelHeight - brw_heading);
    int fulllen = GetFullLHeight();
    if/*(fulllen > sbar->visible_len)*/(fulllen > visible_len)
    {
        //sbar->offset = sbar->actual_offset = fulllen - sbar->visible_len;
        //main_offs = -(int)(sbar->offset);
        main_offs = -(int)(fulllen - visible_len);
    }
}

void InstrPanel::CloneInstrument(Instrument* i)
{
    Instrument* ni = i->Clone();
    if(i != last_instr->prev)
    {
        M.dropinstr1 = i;
        M.dropinstr2 = i->next;

        DragNDrop_PlaceInstrument();

        M.dropinstr1 = M.dropinstr2 = NULL;
    }
	else
		UpdateStepSequencers();
}

Instrument* InstrPanel::GetInstrByIndex(int index)
{
    Instrument* instr = first_instr;
    while(instr != NULL)
    {
        if(instr->index == index)
        {
            return instr;
        }
        instr = instr->next;
    }
    return NULL;
}

void InstrPanel::UpdateIndices()
{
    int idx = 0;
    Instrument* instr = first_instr;
    while(instr != NULL)
    {
        instr->index = idx++;
        instr = instr->next;
    }
}

//////////////////////////////////////////////////////////////////////
// This method completely removes an instrument including all possible references
void InstrPanel::RemoveInstrument(Instrument* i)
{
    WaitForSingleObject(hAudioProcessMutex, INFINITE);
    if(current_instr == i)
    {
        current_instr = i->next;
        if(current_instr == NULL)
        {
            current_instr = i->prev;
        }

		// If we delete the current sample then reassign sample window.
		if(i->type == Instr_Sample && SmpWnd->sample == i)
		{
			if(current_instr != NULL && current_instr->type == Instr_Sample)
			{
				SmpWnd->SetSample((Sample*)current_instr);
				current_instr->pEditorButton->pressed = i->pEditorButton->pressed;
			}
			else
			{
				SmpWnd->SetSample(NULL);
				SmpWnd->Hide();
			}
		}

        if(genBrw->brwmode == Browse_Presets || genBrw->brwmode == Browse_Params)
        {
            genBrw->Update();
        }
    }

    if(aux_panel->workPt == i->autoPatt)
    {
        aux_panel->HandleButtDown(aux_panel->PtPianorol);
    }

    if((i == first_instr)&&(i == last_instr))
    {
        first_instr = NULL;
        last_instr = NULL;
    }
    else if(i == first_instr)
    {
        first_instr = i->next;
        first_instr->prev = NULL;
    }
    else if(i == last_instr)
    {
        last_instr = i->prev;
        last_instr->next = NULL;
    }
    else
    {
        if(i->prev != NULL)
        {
            i->prev->next = i->next;
        }
        if(i->next != NULL)
        {
            i->next->prev = i->prev;
        }
    }

    num_instrs--;

    UpdateIndices();

    if(i->temp == false)
        ChangesIndicate();

    i->DereferenceElements();
    delete i;

    UpdateStepSequencers();
    UpdateScaledImages();

    CheckGoUp();

    ReleaseMutex(hAudioProcessMutex);
}

Instrument* InstrPanel::AddInstrumentFromBrowser(FileData * fdi, bool bottom)
{
    Instrument* ni = NULL;
    if(fdi->ftype == FType_Wave)
    {
        char fullpath[MAX_PATH_STRING];
        strcpy(fullpath, fdi->path);
        char alias[MAX_ALIAS_STRING];
        char name[MAX_NAME_STRING];
        strcpy(name, fdi->name);
        ToLowerCase(name);
        GetOriginalInstrumentAlias(name, alias);
        ni = (Instrument*)Add_Sample(fullpath, fdi->name, alias);

        if(bottom)
            IP->GoDown();
    }
    else if(fdi->ftype == FType_VST)
    {
        char fullpath[MAX_PATH_STRING] = {0};
        char alias[MAX_ALIAS_STRING]   = {0};
        char name[MAX_NAME_STRING]     = {0};
        long ret_val = -1;

        strcpy(fullpath, fdi->path);

        strcpy(name, fdi->name);
        ToLowerCase(name);
        GetOriginalInstrumentAlias(name, alias);
        VSTGenerator* vstgen = Add_VSTGenerator(fullpath, fdi->name, alias);
        ni = (Instrument*)vstgen;
        if(vstgen == NULL)
        {
            AlertWindow::showMessageBox(AlertWindow::InfoIcon,
                                        T("Can't load FX"),
                                        T("Can't load this DLL as an instrument."),
                                        T("OK"));
        }
        else
        {
            if(bottom)
                IP->GoDown();
        }
    }
    else if(fdi->ftype == FType_Native)
    {
        char alias[MAX_ALIAS_STRING] = {0};
        if(strcmp(fdi->name, "Chaotic Synthesizer") == 0)
        {
            GetOriginalInstrumentAlias("synthesizer", alias);
            ni = Add_Synth(alias);
        }

        if(bottom && ni != NULL)
            IP->GoDown();
    }

    if(ni != NULL)
    {
        ChangeCurrentInstrument(ni);

        UpdateStepSequencers();

        R(Refresh_InstrPanel);
        if(aux_panel->auxmode == AuxMode_Pattern && aux_panel->workPt->ptype == Patt_StepSeq)
        {
            R(Refresh_Aux);
        }
    }

    return ni;
}

void EffFoldClick(Object* owner, Object* self)
{
	Toggle* tg = (Toggle*)self;
    MixChannel* mchan = tg->scope->eff->mixcell->mchan;
    mchan->drawarea->Change();
	if(*(tg->state) == false)
		tg->scope->eff->Enable();
	else
		tg->scope->eff->Disable();
}

void EffBypassClick(Object* owner, Object* self)
{
    Toggle* tg = (Toggle*)self;
    if(*(tg->state) == true)
    {
        //tg->scope->eff->Reset();
    }
}

void MixChannelScrollClick(Object* owner, Object* self)
{
	Butt* bt = (Butt*)self;
    MixChannel* mchan = (MixChannel*)bt->panel;
    mchan->drawarea->Change();

    if(bt == mchan->kup)
    {
        int offs = 0;
        Eff* eff = mchan->first_eff;
        while(eff != NULL)
        {
            if(eff->y + eff->height >= mchan->ry1 - 1)
            {
                mchan->voffs = offs;
                break;
            }
            else
            {
                offs += eff->height;
            }
            eff = eff->cnext;
        }
    }
    else if(bt == mchan->down)
    {
        int offs = 0;
        Eff* eff = mchan->first_eff;
        while(eff != NULL)
        {
            if(eff->y > mchan->ry1)
            {
                mchan->voffs = offs;
                break;
            }
            else
            {
                offs += eff->height;
            }
            eff = eff->cnext;
        }
    }
}

MixChannel::MixChannel()
{
    type = Panel_MixChannel;
    send = master = false;
    memset(&indexstr, 0, 5);
    visible = false;
    col = 0;
    voffs = 0;
    cheight = 0;
    ry1 = ry2 = 0;
    mutecount = 0;
    mc_first = mc_last = NULL;
    first_eff = last_eff = NULL;
    rampCounterV = 0;
    cfsV = 0;
    incount = 0;

    mc_main = new Mixcell();
    mc_main->auxcell = true;
    mc_main->mchan = this;
    mc_main->vol_slider = new SliderBase(mc_main, &mc_main->scope);
    mc_main->vol_slider->SetHint("Channel level");
    mc_main->pan_slider = new SliderHPan(42, mc_main, &mc_main->scope);
    mc_main->pan_slider->SetHint("Channel pannning");
    mc_main->params = new ParamSet(100, 50, &mc_main->scope);
    AddGlobalParam(mc_main->params->vol);
    AddGlobalParam(mc_main->params->pan);
    mc_main->params->vol->AddControl(mc_main->vol_slider);
    mc_main->params->pan->AddControl(mc_main->pan_slider);
    mc_main->params->vol->SetNormalValue(1);
    mc_main->params->pan->SetNormalValue(0);
    mc_main->vol_slider->SetAnchor(1);
    mc_main->pan_slider->SetAnchor(0);
    mc_main->mute_toggle = new Toggle(mc_main, Toggle_Mute);
    mc_main->mute_toggle->SetImages(NULL, img_mute1off_c);
    mc_main->mute_toggle->SetHint("Mute channel");
    mc_main->solo_toggle = new Toggle(mc_main, Toggle_Solo);
    mc_main->solo_toggle->SetImages(NULL, img_solo1off_c);
    mc_main->solo_toggle->SetHint("Solo channel");
    mc_main->inpin = new InPin_Bus;
    mc_main->inpin->owna = mc_main;
    mc_main->outpin = new OutPin;
    mc_main->outpin->owna = mc_main;
    mc_main->mixstr = new TString("", false);
    mc_main->mixstr->panel = mc_main;
    mc_main->mixstr->highlightable = false;
    mc_main->grouper = true;
    mc_main->UpdateIndexStr();
    mc_main->Disable();

    params = mc_main->params;

    AddNewControl(mc_main->mute_toggle, this);
    AddNewControl(mc_main->solo_toggle, this);
    AddNewControl(mc_main->vol_slider, this);
    AddNewControl(mc_main->pan_slider, this);

    p_vu = mc_main->vol_slider->vu;

    amount1 = new Parameter(0.0f, 0.0f, 1.0f, Param_Default);
    amount1->SetName("send1.amount");
    AddGlobalParam(amount1);
    r_amount1 = new Knob(8, amount1, mc_main, &mc_main->scope, Knob_Large);
    r_amount1->SetHint("Send amount");
    amount1->AddControl(r_amount1);
    AddNewControl(r_amount1, this);

    amount2 = new Parameter(0.0f, 0.0f, 1.0f, Param_Default);
    amount2->SetName("send2.amount");
    AddGlobalParam(amount2);
    r_amount2 = new Knob(8, amount2, mc_main, &mc_main->scope, Knob_Large);
    r_amount2->SetHint("Send amount");
    amount2->AddControl(r_amount2);
    AddNewControl(r_amount2, this);

    amount3 = new Parameter(0.0f, 0.0f, 1.0f, Param_Default);
    amount3->SetName("send3.amount");
    AddGlobalParam(amount3);
    r_amount3 = new Knob(8, amount3, mc_main, &mc_main->scope, Knob_Large);
    r_amount3->SetHint("Send amount");
    amount3->AddControl(r_amount3);
    AddNewControl(r_amount3, this);

    amount1->scope = amount2->scope = amount3->scope = &mc_main->scope;

    kup = new Butt(false);
    kup->freeset = false;
    kup->SetImages(img_btinsup1, img_btinsup2);
    down = new Butt(false);
    down->freeset = false;
    down->SetImages(img_btinsdown1, img_btinsdown2);
    kup->SetOnClickHandler(&MixChannelScrollClick);
    down->SetOnClickHandler(&MixChannelScrollClick);
    AddNewControl(kup, this);
    AddNewControl(down, this);

    routestr = new DigitStr("--");
    routestr->num_digits = 2;
    routestr->mchanowner = this;
    routestr->highlightable = false;
    routestr->SetHint("Output channel");
    Add_PEdit(routestr);

    drawarea = new DrawArea((void*)this, Draw_MixChannel);
    MC->AddDrawArea(drawarea);
}

MixChannel::~MixChannel()
{
    
}

void MixChannel::Disable()
{
    visible = false;
    drawarea->Disable();
    Control* ctrl = first_ctrl;
    while(ctrl != NULL)
    {
        ctrl->Deactivate();
        ctrl = ctrl->pn_next;
    }

    mc_main->Disable();

    Eff* eff = first_eff;
    while(eff != NULL)
    {
        eff->fold_toggle->Deactivate();
        eff->bypass_toggle->Deactivate();
        eff->wnd_toggle->Deactivate();
        if(eff->type == EffType_VSTPlugin)
            ((VSTEffect*)eff)->pEditButton->Deactivate();
        eff->Disable();
        eff = eff->cnext;
    }

    routestr->Deactivate();
}

void MixChannel::Enable()
{
    visible = true;
    drawarea->Enable();
    Control* ctrl = first_ctrl;
    while(ctrl != NULL)
    {
        if(ctrl->scope != NULL && 
            (ctrl->scope->eff == NULL || ctrl == ctrl->scope->eff->fold_toggle))
        {
            ctrl->Activate();
        }
        ctrl = ctrl->pn_next;
    }

	mc_main->Enable();

    Eff* eff = first_eff;
    while(eff != NULL)
    {
        eff->fold_toggle->Activate();
        eff->bypass_toggle->Activate();
        eff->wnd_toggle->Activate();
        if(eff->type == EffType_VSTPlugin)
            ((VSTEffect*)eff)->pEditButton->Activate();
        eff->Enable();
        eff = eff->cnext;
    }

    routestr->Activate();
}

void MixChannel::UpdateIndexStr(int n)
{
    col = n;
    if(col < 100)
    {
        ParamNum2String(col + 1, indexstr);
    }
    else if(col < 104)
    {
        strcpy(indexstr, "Send");
        char snum[2];
        Num2String(col - 100, snum);
        strcat(indexstr, snum);
    }
    else
    {
        strcpy(indexstr, "M");
    }
}

void MixChannel::AddEffect(Eff* eff)
{
    if(first_eff == NULL && last_eff == NULL)
    {
        eff->cprev = NULL;
        eff->cnext = NULL;
        first_eff = eff;
    }
    else
    {
        last_eff->cnext = eff;
        eff->cprev = last_eff;
        eff->cnext = NULL;
    }
    last_eff = eff;

    eff->SetNewMixcell(mc_main);
}

void MixChannel::RemoveEffect(Eff* eff)
{
    if((eff == first_eff)&&(eff == last_eff))
    {
        first_eff = NULL;
        last_eff = NULL;
    }
    else if(eff == first_eff)
    {
        first_eff = eff->cnext;
        first_eff->cprev = NULL;
    }
    else if(eff == last_eff)
    {
        last_eff = eff->cprev;
        last_eff->cnext = NULL;
    }
    else
    {
        if(eff->cprev != NULL)
        {
            eff->cprev->cnext = eff->cnext;
        }
        if(eff->cnext != NULL)
        {
            eff->cnext->cprev = eff->cprev;
        }
    }

    if(first_eff == NULL && last_eff == NULL)
    {
        voffs = 0;
    }

    //RemoveControl(eff->fold_toggle);
    //if(eff->type == EffType_VSTPlugin)
    //{
    //    RemoveControl(((VSTEffect*)eff)->pEditButton);
    //}
    eff->SetNewMixcell(NULL);
}

void MixChannel::Process(int num_frames, float* outbuff)
{
    if(first_eff != NULL)
    {
        Eff* eff = first_eff;
        while(eff != NULL)
        {
            eff->Process(in_buff, out_buff, num_frames);
            if(eff->bypass == false)
            {
                // Copy output back to input for the next effect to process
                if(eff->muteCount > 0)
                {
                    tframe tc = 0;
                    float aa;
                    while(tc < num_frames)
                    {
                        aa = float(DECLICK_COUNT - eff->muteCount)/DECLICK_COUNT;
                        in_buff[tc*2] = in_buff[tc*2]*(1.f - aa) + out_buff[tc*2]*aa;
                        in_buff[tc*2 + 1] = in_buff[tc*2 + 1]*(1.f - aa) + out_buff[tc*2 + 1]*aa;
                        tc++;
                        if(eff->muteCount > 0)
                            eff->muteCount--;
                    }
                }
                else
                    memcpy(in_buff, out_buff, sizeof(float)*num_frames*2);
            }
            else
            {
                if(eff->muteCount < DECLICK_COUNT)
                {
                    tframe tc = 0;
                    float aa;
                    while(tc < num_frames && eff->muteCount < DECLICK_COUNT)
                    {
                        aa = float(DECLICK_COUNT - eff->muteCount)/DECLICK_COUNT;
                        in_buff[tc*2] = in_buff[tc*2]*(1.f - aa) + out_buff[tc*2]*aa;
                        in_buff[tc*2 + 1] = in_buff[tc*2 + 1]*(1.f - aa) + out_buff[tc*2 + 1]*aa;
                        tc++;
                        eff->muteCount++;
                    }
                }
            }

            eff = eff->cnext;
        }
    }
    else
    {
        memcpy(out_buff, in_buff, sizeof(float)*num_frames*2);
    }

    if(outbuff != NULL)
    {
        float panv, volv, volL, volR;
        bool off = false;
        {
            if(mc_main->params->muted == true || (Solo_MixChannel != NULL && Solo_MixChannel != this))
            {
                off = true;
            }

            bool fill;
            if(off == false || mutecount < DECLICK_COUNT)
                fill = true;
            else
                fill = false;

            if(fill)
            {
                Envelope* venv = NULL;
                Envelope* penv = NULL;
                if(mc_main->params->vol->envelopes != NULL)
                {
                    venv = (Envelope*)((Command*)mc_main->params->vol->envelopes->el)->paramedit;
                }
                if(mc_main->params->pan->envelopes != NULL)
                {
                    penv = (Envelope*)((Command*)mc_main->params->pan->envelopes->el)->paramedit;
                }

                volv = params->vol->outval;
                if(params->vol->lastval == -1)
                {
                    params->vol->SetLastVal(params->vol->outval);
                }
                else if(params->vol->lastval != params->vol->outval)
                {
                    if(rampCounterV == 0)
                    {
                        cfsV = float(params->vol->outval - params->vol->lastval)/RAMP_COUNT;
                        volv = params->vol->lastval;
                        rampCounterV = RAMP_COUNT;
                    }
                    else
                    {
                        volv = params->vol->lastval + (RAMP_COUNT - rampCounterV)*cfsV;
                    }
                }
                else if(rampCounterV > 0) // (params->vol->lastval == params->vol->outval)
                {
                    rampCounterV = 0;
                    cfsV = 0;
                }

                panv = params->pan->outval;
                PanLinearRule(panv, &volL, &volR);

                float aa = 1;
                float lMax, rMax, outL, outR;
                lMax = rMax = 0;
                float mL, mR;
                long tc = 0;
                for(long cc = 0; cc < num_frames; cc++)
                {
                    if(venv != NULL && rampCounterV == 0)
                    {
                        volv = venv->buffoutval[cc];
                    }

                    if(rampCounterV > 0)
                    {
                        volv += cfsV;
                        rampCounterV--;
                        if(rampCounterV == 0)
                        {
                            params->vol->SetLastVal(params->vol->outval);
                        }
                    }

                    if(penv != NULL)
                    {
                        panv = penv->buffoutval[cc];
                        PanLinearRule(panv, &volL, &volR);
                    }

                    if(off == true)
                    {
                        // Fadeout case
                        if(mutecount < DECLICK_COUNT)
                        {
                            aa = float(DECLICK_COUNT - mutecount)/DECLICK_COUNT;
                            mutecount++;
                        }
                        else
                            aa = 0;
                    }
                    else
                    {
                        // Fadein case
                        if(mutecount > 0)
                        {
                            aa = float(DECLICK_COUNT - mutecount)/DECLICK_COUNT;
                            mutecount--;
                        }
                        else if(mutecount == 0)
                            aa = 1;
                    }

    				if(volL > volR)
    					mR = (volL - volR)*in_buff[tc + 1];
    				else
    					mR = 0;

    				if(volR > volL)
    					mL = (volR - volL)*in_buff[tc];
    				else
    					mL = 0;

    				outL = in_buff[tc]*volL + mR;
                    outR = in_buff[tc + 1]*volR + mL;

                    outL *= volv*aa;
                    outR *= volv*aa;

                    if(outL > lMax)
                        lMax = outL;
                    if(outR > rMax)
                        rMax = outR;

                    outbuff[tc++] += outL;
                    outbuff[tc++] += outR;
                }

                if(p_vu->drawarea->isEnabled())
                {
                    p_vu->SetLR(lMax, rMax);
                }
            }
            else
            {
                //memset(out_buff, 0, num_frames*sizeof(float)*2);
            }
        }
    }
}

void MixChannel::DoSend(float * sendbuff, float amount, int num_frames)
{
    if(sendbuff != NULL)
    {
        float outL, outR;
        int i = 0;
        int fc = 0;
        while(i < num_frames)
        {
            outL = out_buff[fc];
            outR = out_buff[fc + 1];

            sendbuff[fc++] += outL*amount;
            sendbuff[fc++] += outR*amount;

            i++;
        }
    }
}

void MixChannel::HandleButtUp(Butt* bt)
{
    bt->Refresh();

    R(Refresh_Buttons);
}

void MixChannel::Save(XmlElement * xmlChanNode)
{
    xmlChanNode->setAttribute(T("Index"), indexstr);

    xmlChanNode->addChildElement(params->vol->Save());
    xmlChanNode->addChildElement(params->pan->Save());

    xmlChanNode->addChildElement(amount1->Save());
    xmlChanNode->addChildElement(amount2->Save());
    xmlChanNode->addChildElement(amount3->Save());

    xmlChanNode->setAttribute(T("Mute"), params->muted ? 1 : 0);
    xmlChanNode->setAttribute(T("Solo"), params->solo ? 1 : 0);

    xmlChanNode->setAttribute(T("VOffset"), voffs);

    xmlChanNode->setAttribute(T("OutMixChannel"), String(routestr->digits));

    Eff* eff = first_eff;
    while(eff != NULL)
    {
        XmlElement* xmlEff = new XmlElement(T("Effect"));
        eff->Save(xmlEff);
        xmlChanNode->addChildElement(xmlEff);

        eff = eff->cnext;
    }

    /*
    Parameter* pParam = this->first_param;
    while (pParam != NULL)
    {
        if(pParam->IsPresetable())
        {
            XmlElement * xmlParam = pParam->Save();
            xmlChanNode->addChildElement(xmlParam);
        }
        pParam = pParam->next;
    }
    */
}

void MixChannel::Load(XmlElement * xmlNode)
{
    voffs = xmlNode->getIntAttribute(T("VOffset"));

    bool bval = xmlNode->getBoolAttribute(T("Mute"));
    params->muted = bval;
    bval = xmlNode->getBoolAttribute(T("Solo"));
    params->solo = bval;
    if(params->solo)
    {
        Solo_MixChannel = this;
    }

    String mchan = xmlNode->getStringAttribute(T("OutMixChannel"));
    if(mchan.length() > 0)
    {
        routestr->SetString((const char*)mchan);
    }

    XmlElement* xmlParam = NULL;
    forEachXmlChildElementWithTagName(*xmlNode, xmlParam, T("Parameter"))
    {
        String sname = xmlParam->getStringAttribute(T("name"));
        if(sname == T("Vol"))
        {
            params->vol->Load(xmlParam);
        }
        else if(sname == T("Pan"))
        {
            params->pan->Load(xmlParam);
        }
        else if(sname == T("send1.amount"))
        {
            amount1->Load(xmlParam);
        }
        else if(sname == T("send2.amount"))
        {
            amount2->Load(xmlParam);
        }
        else if(sname == T("send3.amount"))
        {
            amount3->Load(xmlParam);
        }
    }
}

Aux::Aux()
{
    type = Panel_Aux;

    InitBlank(Patt_StepSeq);

    workPt = blankPt;

    keywidth = 60;
    prev_mouse_x = -1;
    prev_mouse_y = -1;
    //lineHeight = PianorollLineHeight;
    //tickWidth = tickWidthBack = 12;
    lineHeight = 15;
    tickWidth = tickWidthBack = 16;

    locked = false;
    eux = 80;
    bottomincr = 0;
    offset_correction = false;
    key_height = 100;
    patt_height = 388;
    mix_height = 399;
    volpan_height = 128;
    last_mixcenterwidth = MixCenterWidth;

    AuxHeight = patt_height;
    auxmode = AuxMode_Pattern;
    last_ptype = Patt_StepSeq;
    last_vpmode = AuxMode_Vols;
    ltype = Lane_Vol;

    reloctaver = new Numba(&relOctave, Numba_relOctave);

    v_sbar = new ScrollBard(Ctrl_VScrollBar, 0, 0, 17);
    v_sbar->discrete = true;
    v_sbar->Activate();
    v_sbar->AddDrawarea();

    h_sbar = new ScrollBard(Ctrl_HScrollBar, 0, 0, 17);
    h_sbar->fixed = true;
    h_sbar->Activate();
    h_sbar->AddDrawarea();

    OffsTick = 0;
    note_offset = 73;
    OffsLine = NUM_PIANOROLL_LINES - note_offset - 1;

    AddNewControl(v_sbar, this);
    AddNewControl(h_sbar, this);

    ResetScrollbars();

    Vols = new ButtFix(false);
    Pans = new ButtFix(false);
    PattExpand = new ButtFix(false);
    AddNewControl(Vols, this);
    AddNewControl(Pans, this);
    AddNewControl(PattExpand, this);

    PtUsual = new ButtFix(false);
    PtPianorol = new ButtFix(false);
    PtStepseq = new ButtFix(true);
    PtAuto = new ButtFix(false);
    PtAuto->coverable = false;
    AddNewControl(PtUsual, this);
    AddNewControl(PtPianorol, this);
    AddNewControl(PtStepseq, this);
    AddNewControl(PtAuto, this);
    CurrPt = PtStepseq;

    PR8 = new ButtFix(false);
    PR10 = new ButtFix(true);
    PR12 = new ButtFix(false);
    AddNewControl(PR8, this);
    AddNewControl(PR10, this);
    AddNewControl(PR12, this);
    CurrPR = PR10;

    playbt = new ButtFix(false);
    playbt->SetHint("Play pattern");
    stopbt = new Butt(false);
    stopbt->SetHint("Stop pattern");
    AddNewControl(playbt, this);
    AddNewControl(stopbt, this);

    Back = new Butt(false);
    Forth = new Butt(false);
    Up = new Butt(false);
    Down = new Butt(false);
    AddNewControl(Back, this);
    AddNewControl(Forth, this);
    AddNewControl(Up, this);
    AddNewControl(Down, this);
    Vols1 = new ButtFix(true);
    Pans1 = new ButtFix(false);
    Pitch1 = new ButtFix(false);
    AddNewControl(Vols1, this);
    AddNewControl(Pans1, this);
    AddNewControl(Pitch1, this);

    revsmp = new ButtFix(false);
    AddNewControl(revsmp, this);

    IncrScale = new Butt(false);
    DecrScale = new Butt(false);
    AddNewControl(IncrScale, this);
    AddNewControl(DecrScale, this);

    slzoom = new SliderZoom(this, NULL);
    slzoom->SetHint("Zoom slider\0");
    slzoom->SetTickWidth(tickWidth);
    AddNewControl(slzoom, this);

    //DefaultScaleMax = new Butt(false);
    //DefaultScaleMin = new Butt(false);
    //AddNewControl(DefaultScaleMax, this);
    //AddNewControl(DefaultScaleMin, this);

    qMenu = new DropMenu(false, Menu_Quant);
    qMenu->AddItem(MItem_Divider, NULL, ".....");
    qMenu->AddItem(MItem_None, NULL, "(none)");
    qMenu->AddItem(MItem_Step16, NULL, "1/6 tick");
    qMenu->AddItem(MItem_Step13, NULL, "1/3 tick");
    qMenu->AddItem(MItem_Step14, NULL, "1/4 tick");
    qMenu->AddItem(MItem_Step12, NULL, "1/2 tick");
    qMenu->AddItem(MItem_Step, NULL, "Tick");
    qMenu->AddItem(MItem_Beat, NULL, "Beat");
    qMenu->AddItem(MItem_Bar, NULL, "Bar");

    qMenu->SetMainItem(MItem_Step);
    qMenu->active = true;
    AddNewControl(qMenu, this);
    quantsize = 1;

    auto_switch = false;
    autoswitch = new Toggle(Toggle_AutoSwitch, &auto_switch);
    AddNewControl(autoswitch, this);

    relative_oct = true;
    relswitch = new Toggle(Toggle_RelSwitch, &relative_oct);
    AddNewControl(relswitch, this);

    curr_play_x = 0;
    curr_play_x_f = 0;

    ev_count_down = 0;
    queued_ev = NULL;
    playing = false;

    InitMixer();

    prev_mouse_x = -1;
    prev_mouse_y = -1;
}

void Aux::InitMixer()
{
    hoffs = 0;

    for(int mc = 0; mc < NUM_MIXCHANNELS; mc++)
    {
        mchan[mc].UpdateIndexStr(mc);
        mchan[mc].mc_main->col = mc;
        mchan[mc].mc_main->UpdateIndexStr();
        mchan[mc].routestr->mchanout = &masterchan;
    }

    sbarH = 22;
    mix_sbar = new ScrollBard(Ctrl_HScrollBar, 0, 0, sbarH);
    mix_sbar->Activate();
    AddNewControl(mix_sbar, this);

    sendchan[0].UpdateIndexStr(101);
    sendchan[0].send = true;
    sendchan[1].UpdateIndexStr(102);
    sendchan[1].send = true;
    sendchan[2].UpdateIndexStr(103);
    sendchan[2].send = true;
    sendchan[3].UpdateIndexStr(104);
    sendchan[3].send = true;

    masterchan.UpdateIndexStr(256);
    masterchan.master = true;
    mAster.params->vol->AddControl(masterchan.mc_main->vol_slider);
    current_eff = NULL;
    masterchan.routestr->mchanout = NULL;

    this->hMixMutex = CreateMutex(NULL, FALSE, NULL);
}

void Aux::CleanBuffers(int num_frames)
{
    for(int mc = 0; mc < NUM_MIXCHANNELS; mc++)
    {
        if(mc < 3)
        {
            memset(sendchan[mc].in_buff, 0, sizeof(float)*num_frames*2);
        }
        memset(mchan[mc].in_buff, 0, sizeof(float)*num_frames*2);
    }
    memset(masterchan.in_buff, 0, sizeof(float)*num_frames*2);
}

void Aux::Mix(int num_frames)
{
    WaitForSingleObject(hMixMutex, INFINITE);
    for(int mc = 0; mc < NUM_MIXCHANNELS; mc++)
    {
        mchan[mc].workcount = mchan[mc].incount;
        mchan[mc].processed = false;
    }

    bool incomplete;
    do
    {
        incomplete = false;
        for(int mc = 0; mc < NUM_MIXCHANNELS; mc++)
        {
            if(mchan[mc].workcount > 0)
                incomplete = true;
            else if(mchan[mc].processed == false)
            {
                mchan[mc].Process(num_frames, mchan[mc].routestr->mchanout->in_buff);
                mchan[mc].processed = true;
                if(mchan[mc].routestr->mchanout != &masterchan)
				{
                    mchan[mc].routestr->mchanout->workcount--;
					jassert(mchan[mc].routestr->mchanout->workcount >= 0);
				}

                if(mchan[mc].amount1->outval > 0)
                    mchan[mc].DoSend(sendchan[0].in_buff, mchan[mc].amount1->outval, num_frames);
                if(mchan[mc].amount2->outval > 0)
                    mchan[mc].DoSend(sendchan[1].in_buff, mchan[mc].amount2->outval, num_frames);
                if(mchan[mc].amount3->outval > 0)
                    mchan[mc].DoSend(sendchan[2].in_buff, mchan[mc].amount3->outval, num_frames);
            }
        }
    }while(incomplete == true);

    sendchan[0].Process(num_frames, masterchan.in_buff);
    sendchan[1].Process(num_frames, masterchan.in_buff);
    sendchan[2].Process(num_frames, masterchan.in_buff);
    masterchan.Process(num_frames, NULL);
    ReleaseMutex(hMixMutex);
}

MixChannel* Aux::GetMixChannelByIndexStr(char * indexstr)
{
    MixChannel* mc = NULL;
    char* snd;
    if((snd = strstr(indexstr, "Send")) != NULL)
    {
        snd += 4;
        int sndnum = ParamString2Num(snd);
        if(sndnum > 0 && sndnum <= 3)
        {
            mc = &sendchan[sndnum - 1];
        }
    }
    else if(strstr(indexstr, "M") != NULL)
    {
        mc = &masterchan;
    }
    else
    {
        int channum = ParamString2Num(indexstr);
        if(channum > 0 && channum <= NUM_MIXCHANNELS)
        {
            mc = &mchan[channum - 1];
        }
    }

    return mc;
}

MixChannel* Aux::CheckFXString(DigitStr * mixstr)
{
    WaitForSingleObject(hMixMutex, INFINITE);
    int num;
    if(mixstr->digits[0] == '-' || mixstr->digits[1] == '-')
    {
        num = 256;
    }
    else
    {
        num = ParamString2Num(mixstr->digits);
    }

    if(num <= 32 && num > 0)
    {
        mixstr->mchanout = &mchan[num - 1];
    }
    else
    {
        mixstr->mchanout = &masterchan;
    }

	if(mixstr->mchanowner != NULL && mixstr->mchanout != &masterchan)
	{
		bool looped = false;
		MixChannel* tmc = mixstr->mchanout;
		while(tmc != NULL && tmc != &masterchan)
		{
			if(tmc == mixstr->mchanowner)
			{
				looped = true;
				break;
			}
			tmc = tmc->routestr->mchanout;
		}

		if(looped)
			mixstr->mchanout = &masterchan;
	}

    if(mixstr->instr != NULL)
    {
        mixstr->instr->fx_channel = mixstr->mchanout;
    }
    else if(mixstr->trk != NULL)
    {
        mixstr->trk->mchan = mixstr->mchanout;
    }

    for(int mc = 0; mc < NUM_MIXCHANNELS; mc++)
    {
        mchan[mc].incount = 0;
    }

    for(int mc = 0; mc < NUM_MIXCHANNELS; mc++)
    {
        if(mchan[mc].routestr->mchanout != NULL)
        {
            mchan[mc].routestr->mchanout->incount++;
        }
    }
    ReleaseMutex(hMixMutex);

    return mixstr->mchanout;
}

void Aux::Play()
{
    if(!isBlank() && Playing == false && pbkAux->playPatt == workPt)
    {
        if(playing == false)
        {
            //Adjust playback
            if(pbkAux->currFrame < pbkAux->rng_start_frame || pbkAux->currFrame > pbkAux->rng_end_frame)
            {
                pbkAux->SetCurrFrame(pbkAux->rng_start_frame);
                UpdatePerBaseFrame(pbkAux);
            }
            pbkAux->UpdateQueuedEv();
            pbkAux->SetActive();
			if(workPt->OrigPt->autopatt == false)
			{
				pbkMain->SetInactive();
			}

            // Soft preinit envelopes (without param initialization)
            if(pbkAux->playPatt->autopatt == false)
            {
                PreInitEnvelopes(pbkMain->currFrame, pbkAux->playPatt, field_pattern->first_ev, true, false);
                PreInitSamples(pbkMain->currFrame, pbkAux->playPatt, field_pattern->first_ev);
            }
            else
            {
                PreInitEnvelopes(pbkAux->currFrame, pbkAux->playPatt, pbkAux->playPatt->first_ev, true, false);
                PreInitSamples(pbkAux->currFrame, pbkAux->playPatt, pbkAux->playPatt->first_ev);
            }

            /*
            if(Recording == true)
            {
                StopRecording();
            }*/

            playing = true;
        }
        else
        {
            playing = false;
            if(Recording == true)
            {
                FinishParamsRecording();
            }

            MC->poso->stopTimer();
            pbkAux->SetInactive();
            pbkAux->SetCurrFrame((long)pbkAux->currFrame_tsync);
			if(workPt->OrigPt->autopatt == false)
			{
				//pbMain->pactive = false;
                pbkMain->SetCurrFrame((long)pbkMain->currFrame_tsync);
			}

            ResetProcessing(false);
            ResetUnstableElements();
            CleanupAll();
        }
    }
}

void Aux::Stop()
{
    bool resetmix = false;
    if(Playing == true)
    {
        Playing = false;
    }
    else if(playing == false) // if both main and aux playbacks are off
    {
        resetmix = true;
    }
    playing = false;

    MC->poso->stopTimer();

    pbkAux->SetInactive();
    if(workPt->OrigPt->autopatt == false)
    {
        pbkMain->SetInactive();
    }

    if(Recording == true)
    {
        StopRecording();
    }

    Preview_StopAll();
    ResetProcessing(resetmix);
    CleanupAll();
    if(workPt != blankPt)
    {
        pbkAux->SetCurrFrame(pbkAux->rng_start_frame);
        UpdatePerBaseFrame(pbkAux);
        pbkAux->UpdateQueuedEv();
    }
    UpdateTime(Loc_MainGrid);

	if(pbkAux->playPatt != NULL)
	{
	    /* Let's comment it out and see which way is better
		// Hard preinit envelopes as the global position can be bigger than zero
		if(pbAux->playPatt->autopatt == false)
			PreInitEnvelopes(pbMain->currFrame, field, field->first_ev, true, true);
		else
			PreInitEnvelopes(pbAux->currFrame, pbAux->playPatt, pbAux->playPatt->first_ev, true, true);
		*/
	}
}

void Aux::ResetScrollbars()
{
    if(workPt->ptype == Patt_Pianoroll)
    {
        v_sbar->actual_offset = v_sbar->offset = (float)(NUM_PIANOROLL_LINES - note_offset - 1);
        OffsLine = (NUM_PIANOROLL_LINES - note_offset - 1);
    }
    else
    {
        v_sbar->actual_offset = v_sbar->offset = 0;
        OffsLine = 0;
    }
}

void Aux::HandleButtUp(Butt* bt)
{
    if(bt == playbt)
    {
        if(workPt != blankPt && Playing == false)
        {
        }
        else
        {
            bt->pressed = false;
        }
    }

    if(scrolling == true)
    {
        scrolling = false;
        KillTimer(hWnd, SCROLL_TIMER);
    }

    bt->Refresh();

    MC->refresh |= Refresh_Buttons;
}

/*==================================================================================================
BRIEF: For any pressed button checks if we should do some other action depending on current conditions.

PARAMETERS:
[IN]
    bt - button that is pressed.
[OUT]
N/A

OUTPUT: Target mode button or NULL if need to avoid any action.
==================================================================================================*/
Butt* Aux::CheckEditModeVariance(Butt* bt)
{
    // No mode change allowed if editing something
    if(auxmode == AuxMode_Pattern && workPt != blankPt && !workPt->autopatt && bt != PtAuto)
    {
        return NULL;
    }
    else
    {
        // If cursor is at small grid, return it to main grid and check for an element
        if(C.loc != Loc_MainGrid)
        {
            C.loc = Loc_MainGrid;
            C.patt = field_pattern;
            CLine = C.fieldposy;
            CTick = C.fieldposx;

            Element* el = IsElemExists(C.fieldposx, C.fieldposy, field_pattern);
            if(el != NULL)
            {
                el->Activate();
            }
        }

        if(C.curElem != NULL && C.curElem->type == El_Pattern)
        {
            Pattern* pt = (Pattern*)C.curElem;
            switch(pt->ptype)
            {
                case Patt_Grid:
                    bt = PtUsual;
                    break;
                case Patt_Pianoroll:
                    bt = PtPianorol;
                    break;
                case Patt_StepSeq:
                    bt = PtStepseq;
                    break;
            }
        }

        return bt;
    }
}

/*==================================================================================================
BRIEF: Converts pattern type, performing corresponding actions on elements.

PARAMETERS:
[IN]
    patt - Original pattern (not derived).
    ptype - new type for the pattern family.
[OUT]
N/A

OUTPUT: N/A.
==================================================================================================*/
void Aux::ConvertPatternType(Pattern* patt, PattType ptype)
{
    //patt->ibound = NULL;
    Pattern* pt = patt->der_first;
    while(pt != NULL)
    {
        //pt->ibound = NULL;
        if(ptype == Patt_Pianoroll)
        {
            pt->offs_line = pt->last_pianooffs;
        }
        else if(patt->ptype == Patt_Pianoroll)
        {
            pt->last_pianooffs = pt->offs_line;
        }

        if(ptype == Patt_StepSeq || ptype == Patt_Grid)
        {
            pt->offs_line = 0;
        }

        pt = pt->der_next;
    }

    if(ptype == Patt_Grid)
    {
        if(patt->ptype == Patt_Pianoroll)   // If switching from pianoroll to grid, then adjust note places
        {
            SlideNote* sn;
            bool line_taken = false;
            int trkline = 0;
            Instance* ii;
            Instrument* i  = first_instr;
            while(i != NULL)
            {
                Element* el = patt->first_elem;
                while(el != NULL)
                {
                    if(el->IsInstance())
                    {
                        ii = (Instance*)el;
                        if(ii->instr == i)
                        {
                            line_taken = true;
                            ii->SetTrackLine(trkline);
                            sn = ii->first_slide;
                            while(sn != NULL)
                            {
                                sn->SetTrackLine(trkline);
                                sn = sn->s_next;
                            }
                        }
                    }
                    else if(el->type == El_SlideNote)
                    {
                        sn = (SlideNote*)el;
                        if(sn->parent == NULL)
                        {
                            sn->SetTrackLine(trkline);
                        }
                    }
                    el = el->patt_next;
                }

                if(line_taken)
                {
                    trkline++;
                    line_taken = false;
                }
                i = i->next;
            }
        }
    }
    else if(ptype == Patt_StepSeq)
    {
        PopulatePatternWithInstruments(patt);

        int trkline = 0;
        SlideNote* sn;
        Instance* ii;
        Instrument* i  = first_instr;
        while(i != NULL)
        {
            Element* el = patt->first_elem;
            while(el != NULL)
            {
                if(el->IsInstance())
                {
                    ii = (Instance*)el;
                    if(ii->instr == i)
                    {
                        ii->SetTrackLine(trkline);
                        if(patt->ptype == Patt_Pianoroll)   // Replace slides only if switching from pianoroll mode
                        {
                            sn = ii->first_slide;
                            while(sn != NULL)
                            {
                                sn->SetTrackLine(trkline);
                                sn = sn->s_next;
                            }
                        }
                    }
                }
                else if(el->type == El_SlideNote && patt->ptype == Patt_Pianoroll) // Replace slides only if switching from pianoroll mode
                {
                    sn = (SlideNote*)el;
                    if(sn->parent == NULL)
                    {
                        sn->SetTrackLine(trkline);
                    }
                }
                el = el->patt_next;
            }

            trkline++;
            i = i->next;
        }
    }
    else if(ptype == Patt_Pianoroll)
    {
        SlideNote* sn;
        Instance* ii;
        Element* el = patt->first_elem;
        while(el != NULL)
        {
            if(el->IsInstance())
            {
                ii = (Instance*)el;
                ii->SetTrackLine(NUM_PIANOROLL_LINES - 1 - ii->ed_note->value);
                sn = ii->first_slide;
                while(sn != NULL)
                {
                    sn->SetTrackLine(NUM_PIANOROLL_LINES - 1 - sn->ed_note->value);
                    sn = sn->s_next;
                }
            }
            else if(el->type == El_SlideNote)
            {
                sn = (SlideNote*)el;
                if(sn->parent == NULL)
                {
                    sn->SetTrackLine(NUM_PIANOROLL_LINES - 1 - sn->ed_note->value);
                }
            }
            el = el->patt_next;
        }
    }

    patt->ptype = ptype;
	pt = patt->der_first;
	while(pt != NULL)
	{
		pt->ptype = ptype;
		pt = pt->der_next;
	}

    patt->UpdateScaledImage();

    ChangesIndicate();
}

void Aux::DeactivatePRHeight()
{
	PR8->Refresh();
	PR10->Refresh();
	PR12->Refresh();
    PR8->Deactivate(); PR10->Deactivate(); PR12->Deactivate();
    //MC->listen->repaint(PtPianorol->x + PtPianorol->width + 4 , PtPianorol->y - 1, 66, 22);
    //R(Refresh_Aux);
}

void Aux::ActivatePRHeight()
{
    PR8->Activate(); PR10->Activate(); PR12->Activate();
	PR8->Refresh();
	PR10->Refresh();
	PR12->Refresh();
    //MC->listen->repaint(PtPianorol->x + PtPianorol->width + 4 , PtPianorol->y - 1, 66, 22);
    //R(Refresh_Aux);
}

void Aux::HandleButtDown(Butt* bt)
{
    if(bt == Vols || bt == Pans)
    {
        DisablePatternStuff();
        DeactivatePRHeight();

        // unlock if required
        if(locked == true)
        {
            PattExpand->Release();
            locked = false;
        }
        PattExpand->Deactivate();

        Vols->pressed = false;
        Pans->pressed = false;
        bt->pressed = true;
        if(auxmode == AuxMode_Pattern && aux_panel->workPt != aux_panel->blankPt)
        {
            //Switch cursor to pattern name and reset
            C.loc = Loc_MainGrid;
            C.patt = field_pattern;
            workPt->Activate();
        }
        auxmode = AuxMode_Undefined;
        CurrPt->pressed = false;
        AuxHeight = volpan_height;
        if(AuxHeight > WindHeight - MainY1 - LinerHeight - 1)
        {
            AuxHeight = WindHeight - MainY1 - LinerHeight - 1;
            volpan_height = AuxHeight;
        }

        if(bt == Vols)
        {
            auxmode = last_vpmode = AuxMode_Vols;
        }
        else if(bt == Pans)
        {
            auxmode = last_vpmode = AuxMode_Pans;
        }
        MainY2 = WindHeight - AuxHeight;
        Grid2Main();

        R(Refresh_All);
    }
    else if(bt == PattExpand)
    {
        if(bt->pressed == false)
        {
            bt->pressed = true;
            AuxHeight = WindHeight - MainY1 - LinerHeight - 1;
            locked = true;
            Vols->pressed = false;
            Pans->pressed = false;
        }
        else
        {
            bt->pressed = false;
            AuxHeight = patt_height;
            locked = false;
            offset_correction = false;
            bottomincr = 0;
        }

        MainY2 = WindHeight - AuxHeight;
        Grid2Main();

        R(Refresh_All);
    }
    else if(bt == PtAuto)
    {
        if(current_instr != NULL && bt->pressed == false)
        {
            EnablePatternStuff();

            // Place cursor at the previously edited pattern
            if(C.loc == Loc_SmallGrid && workPt != blankPt)
            {
                workPt->Activate();
                C.loc = Loc_MainGrid;
                C.patt = field_pattern;
                CLine = workPt->track_line;
                CTick = workPt->start_tick;
            }

            CurrPt->Release();
            bt->Press();
            CurrPt = (ButtFix*)bt;
            if(current_instr->autoPatt == NULL)
            {
                current_instr->CreateAutoPattern();
            }
            EditPattern(current_instr->autoPatt);
        }
    }
    else if(bt == PR8 || bt == PR10 || bt == PR12)
    {
        if(bt->pressed == false)
        {
            CurrPR->Release();
            bt->Press();
            CurrPR = (ButtFix*)bt;

            if(bt == PR8)
            {
                PianorollLineHeight = 8;
            }
            else if(bt == PR10)
            {
                PianorollLineHeight = 10;
            }
            else if(bt == PR12)
            {
                PianorollLineHeight = 12;
            }
            lineHeight = PianorollLineHeight;

            J_RefreshAuxGridImage();

            R(Refresh_Aux);
        }
    }
    else if(bt == PtUsual || bt == PtStepseq || bt == PtPianorol)
    {
        if(bt->pressed == false)
        {
            EnablePatternStuff();
            if(bt == PtPianorol)
            {
                ActivatePRHeight();
            }
            else if(CurrPt == PtPianorol)
            {
                DeactivatePRHeight();
            }

            CurrPt->Release();
            bt->Press();
            CurrPt = (ButtFix*)bt;

            UpdateBlankType();
            if(workPt->autopatt)
            {
                AuxReset();
            }
            else if(workPt->ptype != blankPt->ptype)
            {
                ConvertPatternType(workPt->OrigPt, blankPt->ptype);
            }
            else if(C.curElem != NULL && C.curElem->type == El_Pattern)
            {
                Pattern* pt = (Pattern*)C.curElem;
                if(pt->ptype == blankPt->ptype)
                    workPt = pt;
            }

            EditPattern(workPt);
            ResetScrollbars();
        }
    }
    else if(bt == playbt)
    {
        if(workPt != blankPt)
        {
            if(Playing == true)
            {
                Playing = false;
                CP->play_butt->Release();
            }

            if(bt->pressed == true)
            {
                bt->pressed = false;
            }
            else
            {
                bt->pressed = true;
            }
            Play();
        }
        else
        {
            bt->pressed = true;
        }
    }
    else if(bt == stopbt)
    {
        Stop();
        playbt->Release();
        CP->play_butt->Release();
    }
    else if(bt == Back)
    {
        h_sbar->SetDelta(-100.0f/tickWidth);

        if(scrolling == false)
        {
            scrolling = true;
            scrollbt = Back;
            SetTimer(hWnd, SCROLL_TIMER, 400, NULL);
        }
        R(Refresh_Aux);
    }
    else if(bt == Forth)
    {
        h_sbar->SetDelta(100.0f/tickWidth);

        if(scrolling == false)
        {
            scrolling = true;
            scrollbt = Forth;
            SetTimer(hWnd, SCROLL_TIMER, 400, NULL);
        }
        R(Refresh_Aux);
    }
    else if(bt == Up)
    {
        if(workPt->ptype == Patt_Pianoroll)
        {
            note_offset += 1;
            if(note_offset > NUM_PIANOROLL_LINES - 1)
            {
                note_offset = NUM_PIANOROLL_LINES - 1;
            }
            else if(note_offset < (v_sbar->visible_len))
            {
                note_offset = (int)(v_sbar->visible_len);
        
                v_sbar->offset = v_sbar->full_len - v_sbar->visible_len;
                bottomincr = lineHeight - ((patty2 - eux) - (MainY2 + 36))%lineHeight;
                offset_correction = true;
            }
            v_sbar->actual_offset = v_sbar->offset = (float)(NUM_PIANOROLL_LINES - 1 - note_offset);
        }
        else
        {
            if(v_sbar->full_len > v_sbar->visible_len)
            {
                //v_sbar->TweakByWheel(1, M.mouse_x, M.mouse_y);
                v_sbar->SetDelta(-1);
            }
        }

        if(scrolling == false)
        {
            scrolling = true;
            scrollbt = Up;
            SetTimer(hWnd, SCROLL_TIMER, 400, NULL);
        }

        R(Refresh_Aux);
    }
    else if(bt == Down)
    {
        if(workPt->ptype == Patt_Pianoroll)
        {
            note_offset -= 1;
            if(note_offset > NUM_PIANOROLL_LINES - 1)
            {
                note_offset = NUM_PIANOROLL_LINES - 1;
            }
            else if(note_offset < (v_sbar->visible_len))
            {
                note_offset = (int)(v_sbar->visible_len);
        
                v_sbar->offset = v_sbar->full_len - v_sbar->visible_len;
                bottomincr = lineHeight - ((patty2 - eux) - (MainY2 + 36))%lineHeight;
                offset_correction = true;
            }
            v_sbar->actual_offset = v_sbar->offset = (float)(NUM_PIANOROLL_LINES - 1 - note_offset);
        }
        else
        {
            if(v_sbar->full_len > v_sbar->visible_len)
            {
                v_sbar->SetDelta(1);
                //v_sbar->TweakByWheel(-1, M.mouse_x, M.mouse_y);
            }
        }

        if(scrolling == false)
        {
            scrolling = true;
            scrollbt = Down;
            SetTimer(hWnd, SCROLL_TIMER, 400, NULL);
        }

        R(Refresh_Aux);
    }
    else if(bt == Vols1)
    {
        if(bt->pressed == false)
        {
            bt->Press();
            Pans1->Release();
            Pitch1->Release();
            ltype = Lane_Vol;
			if(MC->listen->comboCC != NULL)
                MC->listen->comboCC->setSelectedId(Lane_Vol);

            R(Refresh_SubAux);
        }
    }
    else if(bt == Pans1)
    {
        if(bt->pressed == false)
        {
            bt->Press();
            Vols1->Release();
            Pitch1->Release();
            ltype = Lane_Pan;
			if(MC->listen->comboCC != NULL)
				MC->listen->comboCC->setSelectedId(Lane_Pan);

            R(Refresh_SubAux);
        }
    }
    else if(bt == Pitch1)
    {
        if(bt->pressed == false)
        {
            bt->Press();
            Vols1->Release();
            Pans1->Release();
            ltype = Lane_Pitch;
			if(MC->listen->comboCC != NULL)
				MC->listen->comboCC->setSelectedId(Lane_Pitch);
            if(aux_panel->workPt->OrigPt->pitch == NULL)
                aux_panel->workPt->OrigPt->AddEnvelope(Param_Pitch);

            R(Refresh_SubAux);
        }
    }
    else if(bt == IncrScale)
    {
        IncreaseScale(false);
    }
    else if(bt == DecrScale)
    {
        DecreaseScale(false);
    }
    else if(bt == revsmp)
    {
        if(revsmp->pressed == true)
        {
            revsmp->pressed = false;
        }
        else
        {
            revsmp->pressed = true;
        }
    }
    else if(bt == mixBrw->ShowFiles || 
		    bt == mixBrw->ShowParams || 
		    bt == mixBrw->ShowPresets || 
			bt == mixBrw->ShowProjects ||
			bt == mixBrw->ShowSamples ||
			bt == mixBrw->ShowNativePlugs ||
			bt == mixBrw->ShowExternalPlugs)
    {
        mixBrw->Enable();
        mixBrw->HandleButtDown(bt);

        Grid2Main();

        R(Refresh_MixCenter);
    }


    bt->Refresh();

    R(Refresh_Buttons);
}

void Aux::UpdateBlankType()
{
    if(CurrPt == PtUsual)
    {
        blankPt->ptype = blankPt->OrigPt->ptype = Patt_Grid;
        blankPt->offs_line = 0;
        last_ptype = Patt_Grid;
    }
    else if(CurrPt == PtPianorol)
    {
        blankPt->ptype = blankPt->OrigPt->ptype = Patt_Pianoroll;
        blankPt->offs_line = NUM_PIANOROLL_LINES - 74;
        last_ptype = Patt_Pianoroll;
    }
    else if(CurrPt == PtStepseq)
    {
        blankPt->ptype = blankPt->OrigPt->ptype = Patt_StepSeq;
        blankPt->offs_line = 0;
        last_ptype = Patt_StepSeq;
        PopulatePatternWithInstruments(blankPt->OrigPt);
    }
}

void Aux::EditPattern(Pattern* pt, bool dbclick)
{
    if(auxmode == AuxMode_Mixer)
    {
        CP->HandleButtDown(CP->view_mixer);
    }

    // Enable/disable linesize buttons
    if(pt->ptype == Patt_Pianoroll)
    {
        ActivatePRHeight();
    }
    else if(workPt->ptype == Patt_Pianoroll)
    {
        DeactivatePRHeight();
    }

    workPt = pt;
    CurrPt->Release();
    if(pt->autopatt == false)
    {
        if(pt->ptype == Patt_Grid)
        {
            CurrPt = PtUsual;
        }
        else if(pt->ptype == Patt_Pianoroll)
        {
            CurrPt = PtPianorol;
        }
        else if(pt->ptype == Patt_StepSeq)
        {
            CurrPt = PtStepseq;
        }
    }
    else
    {
        CurrPt = PtAuto;
    }
    CurrPt->Press();

    UpdateBlankType();

    OffsLine = pt->offs_line;
    v_sbar->actual_offset = v_sbar->offset = (float)OffsLine;
	switch(pt->ptype)
    {
        case Patt_Grid:
            tickWidth = 12;
            lineHeight = 12;

            OffsTick = 0;
            break;
        case Patt_Pianoroll:
            tickWidth = 12;
            lineHeight = PianorollLineHeight;

            OffsTick = 0;
            note_offset = NUM_PIANOROLL_LINES - OffsLine - 1;
            break;
        case Patt_StepSeq:
            lineHeight = 15;
            tickWidth = 16;

            OffsTick = 0;
            break;
    }

    if(pt != blankPt && pt->OrigPt->tick_width > 0)
        tickWidth = pt->OrigPt->tick_width;

    slzoom->SetTickWidth(tickWidth);
    tickWidthBack = tickWidth;

    // If the pattern belongs to some instrument, then make that instrument current
    if(pt->ibound != NULL)
    {
        ChangeCurrentInstrument(pt->ibound);
    }

    if(ltype == Lane_Pitch && pt->OrigPt->pitch == NULL)
    {
        pt->OrigPt->AddEnvelope(Param_Pitch);
    }

    UpdateAuxNavBarOnly();

    if(!isPatternMode())
    {
        EnablePatternStuff();
        SwitchToPatternMode();

        R(Refresh_All);
    }
    else
    {
        R(Refresh_GridContent);
        R(Refresh_Aux);
        R(Refresh_Buttons);
    }

    if(dbclick)
    {
        HandleButtDown(PattExpand);
    }

    Grid2Main();
    UpdatePerScale();
    AuxPos2MainPos();
    aux_Pattern = workPt;
    if(C.loc == Loc_SmallGrid)
    {
		C.ExitToCurrentMode();
        C.PatternUpdate();
    }
}

void Aux::SwitchToPatternMode()
{
    PattExpand->Activate();
    if(locked == true)
    {
        AuxHeight = WindHeight - MainY1 - LinerHeight - 1;
    }
    else
    {
        AuxHeight = patt_height;
        if(AuxHeight > WindHeight - MainY1 - LinerHeight - 1)
        {
            AuxHeight = WindHeight - MainY1 - LinerHeight - 1;
            patt_height = AuxHeight;
        }
    }
    Vols->Release();
    Pans->Release();
    auxmode = AuxMode_Pattern;
    MainY2 = WindHeight - AuxHeight;
}

bool Aux::RescanPatternBounds()
{
    float ticklength = workPt->tick_length;
    float endtick = workPt->start_tick + workPt->tick_length;
    float elend;

	if(workPt->ptype != Patt_StepSeq && workPt->touchresized == false)
	{
		workPt->tick_length = float(beats_per_bar*ticks_per_beat);
		//workPt->num_lines = 1;
		workPt->end_tick = workPt->start_tick + workPt->tick_length;

		Element* el = workPt->OrigPt->first_elem;
		while(el != NULL)
		{
			if(el->IsPresent())
			{
				elend = el->start_tick + el->tick_length;
				if(elend > workPt->tick_length)
				{
					if((elend == (int)elend) && (int)elend%(int)(beats_per_bar*ticks_per_beat) == 0)
					{
						workPt->tick_length = (float)elend;
					}
					else
					{
						workPt->tick_length = float(int(elend/(beats_per_bar*ticks_per_beat) + 1)*(beats_per_bar*ticks_per_beat));
					}
					workPt->end_tick = workPt->start_tick + workPt->tick_length;
				}
				else if(el->type == El_Command)
				{
					Command* cmd = (Command*)el;
					if(cmd->IsEnvelope())
					{
						Envelope* env = (Envelope*)cmd->paramedit;
						if(cmd->start_tick + env->len > workPt->tick_length)
						{
							workPt->tick_length = float(int((cmd->start_tick + env->len)/(beats_per_bar*ticks_per_beat) + 1)*(beats_per_bar*ticks_per_beat));
							workPt->end_tick = workPt->start_tick + workPt->tick_length;
						}
					}
				}
			}
			el = el->patt_next;
		}
	}

    if((workPt->OrigPt != NULL && workPt->OrigPt->autopatt == false))
    {
		workPt->OrigPt->UpdateScaledImage();
		R(Refresh_GridContent);
	}

    if(ticklength != workPt->tick_length)
    {
        undoMan->DoNewAction(Action_Resize, (void*)workPt, ticklength, workPt->tick_length, 0, 0);
        workPt->touchresized = false;
        workPt->Update();
        Pattern* pt = workPt->OrigPt->der_first;
        while(pt != NULL)
        {
            if(pt != workPt && pt->touchresized == false)
            {
                ticklength = pt->tick_length;
                pt->SetEndTick(pt->StartTick() + workPt->tick_length);

                undoMan->DoNewAction(Action_Resize, (void*)pt, ticklength, pt->tick_length, 0, 0);
            }
            pt = pt->der_next;
        }

        //pbAux->SetRanges(workPt->actual_start_frame, workPt->actual_end_frame);
        pbkAux->AlignRangeToPattern();

        UpdateNavBarsData();

        R(Refresh_AuxGrid);
        if(workPt->visible)
        {
            R(Refresh_GridContent);
        }
        return true;
    }
    else
    {
        return false;
    }
}

void Aux::UpdatePerScale()
{
	Selection_UpdateCoords();
    Looping_UpdateCoords();
	frames_per_pixel = seconds_in_tick*fSampleRate/tickWidth;
    if(workPt != blankPt)
    {
        if(Playing == true)
        {
            curr_play_x_f = (pbkMain->currFrame - workPt->frame)/frames_per_pixel;
        }
        else
        {
            curr_play_x_f = (pbkAux->currFrame - workPt->frame)/frames_per_pixel;
        }
        curr_play_x = RoundDouble(curr_play_x_f);
    }

    // Assign new scale to the edited pattern
    workPt->OrigPt->tick_width = tickWidth;

    J_RefreshAuxGridImage();
}

void Aux::AdjustTick()
{
    tickWidth = tickWidthBack;
    float pixstep = float(tickWidth*ticks_per_beat);
    bool good = float(pixstep - (int)pixstep) == 0;
    if(!good)
    {
        tickWidth = float(RoundFloat(pixstep))/ticks_per_beat;
    }
}

void Aux::SetScale(float scale)
{
    tickWidthBack = scale;
    AdjustTick();

    //flen = (float)(GridXS2 - GridXS1)/(float)tickWidth;

    /*
    if(C.loc == Loc_SmallGrid)
    {
        gAux->OffsTick = CTick - 0.5f*flen;
    }

    if(OffsTick < 0)
    {
        OffsTick = 0;
    }
    */

    UpdatePerScale();

    R(Refresh_AuxGrid);
    R(Refresh_AuxScale);
    R(Refresh_SubAux);
}

void Aux::IncreaseScale(bool mouse)
{
    if(tickWidthBack < 32)
    {
        tickWidth = tickWidthBack;

        float flen = (float)(GridXS2 - GridXS1)/(float)tickWidth;
        float zm = (M.current_tick - OffsTick)/flen;
        //float zc = (CTick - OffsTick)/flen;

        if(tickWidth < 1)
            tickWidth += 0.25f;
        else if(tickWidth < 2)
            tickWidth += 0.5f;
        else if(tickWidth < 6)
            tickWidth++;
        else if(tickWidth < 12)
            tickWidth += 2;
        else
            tickWidth += 4;

        if(tickWidth > 32)
            tickWidth = 32;

        slzoom->SetTickWidth(tickWidth);

        tickWidthBack = tickWidth;
        AdjustTick();

        flen = (float)(GridXS2 - GridXS1)/(float)tickWidth;
        if(mouse == true)
        {
            OffsTick = M.current_tick - zm*flen;
        }

        if(OffsTick < 0)
        {
            OffsTick = 0;
        }

        UpdatePerScale();

        h_sbar->SetOffset(OffsTick);
        //h_sbar->drawarea->Change();

        //R(Refresh_AuxGrid);
        //R(Refresh_AuxScale);
        //R(Refresh_Auxaux);
    }
}

void Aux::DecreaseScale(bool mouse)
{
    if(tickWidthBack > 0.5f)
    {
        tickWidth = tickWidthBack;

        float flen = (float)(GridXS2 - GridXS1)/(float)tickWidth;
        float zm = (M.current_tick - OffsTick)/flen;
        //float zc = (CTick - OffsTick)/flen;

        if(tickWidth > 12)
            tickWidth -= 4;
        else if(tickWidth > 6)
            tickWidth -= 2;
        else if(tickWidth > 2)
        {
            tickWidth--;
            if(tickWidth == 0)
                tickWidth = 1;
        }
        else if(tickWidth > 1.f)
        {
            tickWidth -= 0.5f;
            if(tickWidth == 0)
                tickWidth = 0.5f;
        }
        else if(tickWidth > 0.5f)
        {
            tickWidth -= 0.25f;
            if(tickWidth < 0.5f)
                tickWidth = 0.5f;
        }

        slzoom->SetTickWidth(tickWidth);

        tickWidthBack = tickWidth;
        AdjustTick();

        flen = (float)(GridXS2 - GridXS1)/(float)tickWidth;
        if(mouse == true)
        {
            OffsTick = M.current_tick - zm*flen;
        }

        if(OffsTick < 0)
        {
            OffsTick = 0;
        }

        UpdatePerScale();

        h_sbar->SetOffset(OffsTick);

        //R(Refresh_AuxGrid);
        //R(Refresh_AuxScale);
        //R(Refresh_Auxaux);
    }
}

void Aux::CreateNew()
{
    C.patt = field_pattern;
    char* pname = GetOriginalPatternName();
    workPt->name->SetString(pname);
    workPt->OrigPt->name->SetString(pname);
    GetPatternNameImage(workPt->OrigPt);
    delete pname;

    AddOriginalPattern(workPt->OrigPt);
    if(MakePatternsFat && C.fieldposy > 0)
        workPt->is_fat = true;
    else
        workPt->is_fat = false;

    AddNewElement(workPt, false);
    workPt->start_tick = C.fieldposx;
    workPt->end_tick = C.fieldposx + float(int(M.snappedTick/(beats_per_bar*ticks_per_beat) + 1)*(beats_per_bar*ticks_per_beat));
    workPt->track_line = C.fieldposy;
    /* // What the fuck is this shit???
    workPt->track_line_end = C.fieldposy + M.mouseline;
    workPt->num_lines = workPt->track_line_end - workPt->track_line + 1;
    */
    workPt->folded = true;
    workPt->last_edited_param = workPt->name;
    workPt->Update();
    aux_Pattern = workPt;
    pbkAux->AlignRangeToPattern();

    InitBlank(workPt->ptype);
    AuxPos2MainPos();
    C.patt = aux_panel->workPt;
    C.loc = Loc_SmallGrid;

    if(auxmode == AuxMode_Pattern)
    {
        R(Refresh_Aux);
    }
}

void Aux::InitBlank(PattType ptype)
{
    Pattern* ptmain = new Pattern("blankPt", 0, 0, 0, 0, true);
    blankPt = new Pattern("blankPt", 0.0f, 0.0f, 0, 0, false);
    blankPt->ptype = ptmain->ptype = ptype;
    blankPt->tick_length = ptmain->tick_length = 0;
    blankPt->folded = ptmain->folded = true;
    ptmain->AddDerivedPattern(blankPt);
    if(ptype == Patt_StepSeq)
    {
        // Pattern-original should be referenced to instruments
        PopulatePatternWithInstruments(ptmain);
    }
    else if(ptype == Patt_Pianoroll)
    {
        blankPt->offs_line = NUM_PIANOROLL_LINES - 74;
    }
    blankPt->last_pianooffs = blankPt->offs_line;
    
    ptmain->patt = blankPt->patt = field_pattern;

    if(ltype == Lane_Pitch)
    {
        ptmain->AddEnvelope(Param_Pitch);
    }
}

void Aux::PopulatePatternWithInstruments(Pattern* pt)
{
    // Reset instrument definitions
    Trk* trk = pt->first_trkdata;
    while(trk != pt->bottom_trk)
    {
        trk->defined_instr = NULL;
        trk = trk->next;
    }

    Instrument* i = first_instr;
    Trk* t = pt->first_trkdata;
    while(i != NULL && t != pt->bottom_trk)
    {
        if(i->to_be_deleted == false)
        {
            t->defined_instr = i;
            t = t->next;
        }
        i = i->next;
    }
}

void Aux::AuxReset()
{
    if(workPt != blankPt)
    {
        if(playing == true)
        {
            playbt->Release();
            playing = false;
        }

        if(C.curElem == workPt || C.loc == Loc_SmallGrid || workPt->autopatt)
        {
            if(C.curElem != NULL)
                C.curElem->Leave();
            C.ExitToCurrentMode();
            C.loc = Loc_MainGrid;
            C.patt = field_pattern;
            if(workPt->OrigPt->autopatt == false)
            {
                CLine = workPt->track_line;
                CTick = workPt->start_tick;
            }
        }

        workPt = blankPt;
        if(auxmode == AuxMode_Pattern)
        {
            EditPattern(workPt);
        }
        aux_Pattern = workPt;
        if(blankPt->ptype == Patt_Pianoroll)
        {
            lineHeight = PianorollLineHeight;
        }
        else if(blankPt->ptype == Patt_StepSeq)
        {
            lineHeight = 15;
        }
        else
        {
            lineHeight = 12;
        }
        UpdatePerScale();

        if(M.looloc == Loc_SmallGrid)
            Looping_Reset();
    }

    R(Refresh_Aux);
}

bool Aux::CheckIfMouseOnKeys(int mouse_x, int mouse_y, int* pianokey_num, float * vol)
{
    if((mouse_x < (pattx1 + keywidth))&&(mouse_x < GridXS2)&&(mouse_y > GridYS1 + 1)&&(mouse_y <= GridYS2))
    {
        if(aux_panel->workPt->ptype == Patt_Pianoroll)
        {
            int keypix = OffsLine*lineHeight + (mouse_y - GridYS1) + bottomincr;
            int keypix1 = (NUM_PIANOROLL_LINES)*lineHeight - keypix;
            int oct = keypix1/(12*lineHeight);
            int keypix3 = keypix1%(12*lineHeight);
            float real_key_width = (float)keywidth;
            float delta_width = (float)(mouse_x - pattx1);

            if((mouse_x - pattx1) > keywidth*0.66666666f)
            {
                int th2 = lineHeight/2;
                if(keypix3 <= (lineHeight + th2))
                {
                   *pianokey_num = 0;
                }
                else if(keypix3 <= (lineHeight*3 + th2))
                {
                   *pianokey_num = 2;
                }
                else if(keypix3 <= (lineHeight*5))
                {
                   *pianokey_num = 4;
                }
                else if(keypix3 <= (lineHeight*6 + th2))
                {
                   *pianokey_num = 5;
                }
                else if(keypix3 <= (lineHeight*8 + th2))
                {
                   *pianokey_num = 7;
                }
                else if(keypix3 <= (lineHeight*10 + th2))
                {
                   *pianokey_num = 9;
                }
                else
                {
                   *pianokey_num = 11;
                }
            }
            else
            {
               *pianokey_num = (keypix3 - 2)/lineHeight;
                real_key_width = keywidth*0.66666666f;
            }

           *pianokey_num += oct*12;
            //M.key_num = NUM_PIANOROLL_LINES - M.current_line;
           *vol = 1.0f;/*/(real_key_width - 3)*delta_width;*/
        }
        else
        {
           *pianokey_num = M.current_line;
        }

        return true;
    }
    else
    {
        return false;
    }
}

void Aux::EnablePatternStuff()
{
    qMenu->Activate();
    
    v_sbar->Activate();
    h_sbar->Activate();
    
    playbt->Activate();
    stopbt->Activate();
    
    Back->Activate();
    Forth->Activate();
    
    Up->Activate();
    Down->Activate();

    PattExpand->Activate();
    //DefaultScaleMin->Activate();
    //DefaultScaleMax->Activate();
    DecrScale->Activate();
    IncrScale->Activate();
    slzoom->Activate();
    Vols1->Activate();
    Pans1->Activate();
    Pitch1->Activate();
}

void Aux::DisablePatternStuff()
{
    qMenu->Deactivate();
    
    v_sbar->Deactivate();
    h_sbar->Deactivate();
    
    playbt->Deactivate();
    stopbt->Deactivate();
    
    Back->Deactivate();
    Forth->Deactivate();
    
    Up->Deactivate();
    Down->Deactivate();

    PattExpand->Deactivate();
    //DefaultScaleMin->Deactivate();
    //DefaultScaleMax->Deactivate();
    DecrScale->Deactivate();
    IncrScale->Deactivate();
    slzoom->Deactivate();
    Vols1->Deactivate();
    Pans1->Deactivate();
    Pitch1->Deactivate();
}

void Aux::EnableAuxStuff()
{
    Vols->Activate();
    Pans->Activate();
    
    PtUsual->Activate();
    PtPianorol->Activate();
    PtStepseq->Activate();

    if(workPt->ptype == Patt_Pianoroll && isPatternMode())
    {
        ActivatePRHeight();
    }
}

void Aux::DisableAuxStuff()
{
    Vols->Deactivate();
    Pans->Deactivate();
    
    PtUsual->Deactivate();
    PtPianorol->Deactivate();
    PtStepseq->Deactivate();

    DeactivatePRHeight();
}

void Aux::DisableMixStuff()
{
    AuxHeight = last_height;
    MainY2 = WindHeight - AuxHeight;
    //MainX2 = WindWidth - 176 - 1;
    last_mixcenterwidth = MixCenterWidth;
    MixCenterWidth = DefaultMixCenterWidth;
	MainX2 = WindWidth - MixCenterWidth - 1;
    Grid2Main();

    mix_sbar->Deactivate();

    mixBrw->HideTrackControls->Deactivate();
    mixBrw->ShowExternalPlugs->Deactivate();
    mixBrw->ShowParams->Deactivate();
    mixBrw->ShowPresets->Deactivate();
	mixBrw->Disable();

    for(int mc = 0; mc < NUM_MIXCHANNELS; mc++)
    {
        if(mc < 4)
        {
            sendchan[mc].Disable();
            sendchan[mc].Disable();
        }
        mchan[mc].Disable();
        mchan[mc].Disable();
    }
    masterchan.Disable();
}

void Aux::EnableMixStuff()
{
    last_height = AuxHeight;
    AuxHeight = mix_height;
    if(AuxHeight > WindHeight - MainY1 - LinerHeight - 1)
    {
        AuxHeight = WindHeight - MainY1 - LinerHeight - 1;
        mix_height = AuxHeight;
    }
    MainY2 = WindHeight - AuxHeight;
    MixCenterWidth = last_mixcenterwidth;
	MainX2 = WindWidth - MixCenterWidth - 1;
    Grid2Main();

    mix_sbar->Activate();

    mixBrw->HideTrackControls->Activate();
    mixBrw->ShowExternalPlugs->Activate();
    mixBrw->ShowParams->Activate();
    mixBrw->ShowPresets->Activate();
	mixBrw->Enable();

    for(int mc = 0; mc < NUM_MIXCHANNELS; mc++)
    {
        mchan[mc].Enable();
    }
    sendchan[0].Enable();
    sendchan[1].Enable();
    sendchan[2].Enable();
    sendchan[3].Enable();

    masterchan.Enable();
}

void Aux::PlaceNote(Instrument *instr, int note, float vol, unsigned long start_frame, unsigned long end_frame)
{
    Grid_PutInstanceSpecific(workPt, 
                             instr, 
                             note, 
                             vol, 
                             start_frame - workPt->frame,
                             end_frame - workPt->frame);

    R(Refresh_AuxContent);
}

void Aux::ClickKeys()
{
    if(workPt->ptype == Patt_Pianoroll)
    {
        Mixcell* mc = NULL;
		Pattern* pt = NULL;
		Trk* trk = NULL;
        if(workPt != blankPt)
        {
			pt = workPt;
			trk = workPt->field_trkdata;
			/*
			if(mix->trkfxcell[workPt->field_trkdata->trknum] != &mix->m_cell)
			{
				mc = mix->trkfxcell[workPt->field_trkdata->trknum];
			}*/
			
			// If the pattern is bound, then change current instrument accordingly
			/*
			if(workPt->ibound != NULL && workPt->ibound != current_instr)
            {
                ChangeCurrentInstrument(workPt->ibound);
            }*/
        }
        M.pianokey_pressed = true;
        M.pianokey_slot = Preview_Add(NULL, current_instr, -1, M.pianokey_num, pt, trk, mc);

        R(Refresh_PianoKeys);
    }
    else if(workPt->ptype == Patt_StepSeq && M.current_trk->defined_instr != NULL)
    {
        Mixcell* mc = NULL;
		Pattern* pt = NULL;
		Trk* trk = NULL;
        if(workPt != blankPt)
        {
			pt = workPt;
			trk = workPt->field_trkdata;
			/*
			if(mix->trkfxcell[workPt->field_trkdata->trknum] != &mix->m_cell)
			{
				mc = mix->trkfxcell[workPt->field_trkdata->trknum];
			}
			*/
		}

        ChangeCurrentInstrument(M.current_trk->defined_instr);
        if(C.loc == Loc_SmallGrid)
        {
            C.ExitToDefaultMode();
            C.SetPos(CTick, M.current_trk->start_line);
        }
        Preview_Add(NULL, current_instr, -1, Octave*12, pt, trk, mc);

        R(Refresh_InstrPanel);
        R(Refresh_Aux);
    }
}

void Aux::HandleTweak(Control* ct, int mouse_x, int mouse_y)
{
    if(ct == slzoom)
    {
        float scale = slzoom->GetTickWidth();
        SetScale(scale);
    }
}

Eff* Aux::AddEffectByType(EffType etype, MixChannel* mchan)
{
    Eff* pEff;
    switch(etype)
    {
        case EffType_Gain:
            //pEff = new Gain(mchan->mc_main, NULL);
            break;
        case EffType_Send:
            //pEff = new Send(mchan->mc_main, NULL);
            break;
        case EffType_Filter:
            pEff = new Filter(mchan->mc_main, NULL);
            break;
        case EffType_CFilter:
            pEff = new CFilter(mchan->mc_main, NULL);
            break;
        case EffType_Equalizer1:
            pEff = new EQ1(mchan->mc_main, NULL);
            break;
        case EffType_Equalizer3:
            pEff = new EQ3(mchan->mc_main, NULL);
            break;
        case EffType_GraphicEQ:
            pEff = new GraphicEQ(mchan->mc_main, NULL);
            break;
        case EffType_XDelay:
            pEff = new XDelay(mchan->mc_main, NULL);
            break;
        case EffType_Reverb:
            pEff = new CReverb(mchan->mc_main, NULL);
            break;
        case EffType_Tremolo:
            pEff = new CTremolo(mchan->mc_main, NULL);
            break;
        case EffType_Compressor:
            pEff = new Compressor(mchan->mc_main, NULL);
            break;
        case EffType_Chorus:
            pEff = new CChorus(mchan->mc_main, NULL);
            break;
        case EffType_Flanger:
            pEff = new CFlanger(mchan->mc_main, NULL);
            break;
        case EffType_Phaser:
            pEff = new CPhaser(mchan->mc_main, NULL);
            break;
        case EffType_WahWah:
            pEff = new CWahWah(mchan->mc_main, NULL);
            break;
        case EffType_Distortion:
            pEff = new CDistort(mchan->mc_main, NULL);
            break;
        case EffType_BitCrusher:
            pEff = new CBitCrusher(mchan->mc_main, NULL);
            break;
        case EffType_Stereo:
            pEff = new CStereo(mchan->mc_main, NULL);
            break;
        case EffType_CFilter2:
            pEff = new CFilter2(mchan->mc_main, NULL);
            break;
        case EffType_CFilter3:
            pEff = new CFilter3(mchan->mc_main, NULL);
            break;
        case EffType_Default:
            pEff = new BlankEffect(mchan->mc_main, NULL);
            break;
        default:
            break;
    }

    if(pEff != NULL)
    {
        WaitForSingleObject(hMixMutex, INFINITE);
        AddEff(pEff);
        mchan->AddEffect(pEff);
        SetCurrentEffect(pEff);
        ReleaseMutex(hMixMutex);

        R(Refresh_Aux);
        R(Refresh_AuxHighlights);
    }

    return pEff;
}

Eff* Aux::AddVSTEffectByPath(const char * fullpath, MixChannel * mchan)
{
    VSTEffect* pEffect = NULL;

    AliasRecord ar;
    ar.type = EffType_VSTPlugin;
    strcpy(ar.alias, "plugins");

    pEffect = new VSTEffect(mchan->mc_main, &ar, (char*)fullpath);

    if( pEffect->pPlug != NULL )
    {
        if (pEffect->pPlug->nAllocatedInbufs == 0)
        {
            delete pEffect;
            pEffect = NULL;
            AlertWindow::showMessageBox(AlertWindow::InfoIcon,
                                        T(""),
                                        T("This plugin can't be used as an effect."),
                                        T("OK"));
        }
        else
        {
            WaitForSingleObject(hMixMutex, INFINITE);
            AddEff((Eff*)pEffect);
            mchan->AddEffect((Eff*)pEffect);
            current_eff = pEffect;
            ReleaseMutex(hMixMutex);

            mchan->drawarea->Change();
            R(Refresh_Aux);
            R(Refresh_AuxHighlights);
        }
    }
    else
    {
        delete pEffect;
        pEffect = NULL;
    }

    return pEffect;
}

Eff* Aux::AddEffectFromBrowser(FileData * fdi, MixChannel* mchan)
{
    char fullpath[MAX_PATH_STRING] = {0};

    strcpy(fullpath, fdi->path);

    char alias[MAX_ALIAS_STRING] = {0};
    char *pAlias = NULL;

    /* If effect is external plugin */
    if ((pAlias = strstr(fullpath, "internal://")) == NULL)
    {
        return AddVSTEffectByPath(fullpath, mchan);
    }
    else /* it is internal FX */
    {
        AliasRecord* ar = NULL;
        Eff* pEff = NULL;
        pAlias += strlen("internal://");
        ar = GetAliasRecordFromString(pAlias);

        switch(ar->type)
        {
            case EffType_Gain:
                pEff = new Gain(mchan->mc_main, ar);
                break;
            case EffType_Filter:
                pEff = new Filter(mchan->mc_main, ar);
                break;
            case EffType_CFilter:
                pEff = new CFilter(mchan->mc_main, ar);
                break;
            case EffType_CFilter3:
                pEff = new CFilter3(mchan->mc_main, ar);
                break;
            case EffType_Equalizer1:
                pEff = new EQ1(mchan->mc_main, ar);
                break;
            case EffType_Equalizer3:
                pEff = new EQ3(mchan->mc_main, ar);
                break;
            case EffType_GraphicEQ:
                pEff = new GraphicEQ(mchan->mc_main, ar);
                break;
            case EffType_XDelay:
                pEff = new XDelay(mchan->mc_main, ar);
                break;
            case EffType_Reverb:
                pEff = new CReverb(mchan->mc_main, ar);
                break;
            case EffType_Tremolo:
                pEff = new CTremolo(mchan->mc_main, ar);
                break;
            case EffType_Compressor:
                pEff = new Compressor(mchan->mc_main, ar);
                break;
            case EffType_Chorus:
                pEff = new CChorus(mchan->mc_main, ar);
                break;
            case EffType_Flanger:
                pEff = new CFlanger(mchan->mc_main, ar);
                break;
            case EffType_Phaser:
                pEff = new CPhaser(mchan->mc_main, ar);
                break;
            case EffType_WahWah:
                pEff = new CWahWah(mchan->mc_main, ar);
                break;
            case EffType_Distortion:
                pEff = new CDistort(mchan->mc_main, ar);
                break;
            case EffType_BitCrusher:
                pEff = new CBitCrusher(mchan->mc_main, ar);
                break;
            case EffType_Stereo:
                pEff = new CStereo(mchan->mc_main, ar);
                break;
            case EffType_Default:
                pEff = new BlankEffect(mchan->mc_main, ar);
                break;
            default:
                break;
        }

        if(pEff != NULL)
        {
            WaitForSingleObject(hMixMutex, INFINITE);
            AddEff(pEff);
            mchan->AddEffect(pEff);
            SetCurrentEffect(pEff);
            ReleaseMutex(hMixMutex);

            mchan->drawarea->Change();
            R(Refresh_Aux);
            R(Refresh_AuxHighlights);
        }

        return pEff;
    }
}

void Aux::SetCurrentEffect(Eff * eff)
{
    if(current_eff != eff)
    {
        current_eff = eff;
        if((mixBrw->brwmode == Browse_Params) || (mixBrw->brwmode == Browse_Presets))
        {
            mixBrw->Update();
        }

        R(Refresh_AuxHighlights);
    }
}

bool Aux::isBlank()
{
    return (workPt == blankPt);
}

bool Aux::isVolsPansMode()
{
    return (auxmode == AuxMode_Vols || auxmode == AuxMode_Pans);
}

bool Aux::isPatternMode()
{
    return (auxmode == AuxMode_Pattern);
}

bool Aux::isPlaying()
{
    return playing;
}

void Aux::UpdateDrawInfo()
{
    h_sbar->visible_len = float(GridXS2 - GridXS1)/tickWidth;
    h_sbar->offset = h_sbar->actual_offset = OffsTick;

    v_sbar->active = true;
    v_sbar->visible_len = (float)(GridYS2 - GridYS1)/(float)lineHeight;

    int flen = 0;
    if(workPt->ptype == Patt_Pianoroll)
    {
        v_sbar->full_len = NUM_PIANOROLL_LINES;
    }
    else if(workPt->ptype != Patt_StepSeq)
    {
        Trk* trk = workPt->OrigPt->first_trkdata;
        while(trk != workPt->OrigPt->bottom_trk)
        {
            flen++;
            if(trk->vol_lane.visible == true)
            {
                flen += trk->vol_lane.height;
            }
            if(trk->pan_lane.visible == true)
            {
                flen += trk->pan_lane.height;
            }
            trk = trk->next;
        }
        v_sbar->full_len = (float)flen;
    }
    else if(workPt->ptype == Patt_StepSeq)
    {
        Trk* trk = workPt->OrigPt->first_trkdata;
        while(trk->defined_instr != NULL)
        {
            flen++;
            if(trk->vol_lane.visible == true)
            {
                flen += trk->vol_lane.height;
            }
            if(trk->pan_lane.visible == true)
            {
                flen += trk->pan_lane.height;
            }
            trk = trk->next;
        }
        if(flen < 100)
        {
            flen = 100;
        }
        v_sbar->full_len = (float)flen;
    }

    if(workPt->ptype == Patt_Pianoroll)
    {
        if(note_offset <= (v_sbar->visible_len))
        {
            note_offset = (int)(v_sbar->visible_len);

            v_sbar->actual_offset = v_sbar->offset = v_sbar->full_len - v_sbar->visible_len;
            bottomincr = lineHeight - (GridYS2 - GridYS1)%lineHeight;
            offset_correction = true;
        }
        else if(offset_correction == true)
        {
            offset_correction = false;
            note_offset = (int)(v_sbar->visible_len) + 1;
            v_sbar->actual_offset = v_sbar->offset = (float)(NUM_PIANOROLL_LINES - note_offset);
            bottomincr = 0;
        }
    }
    else if(v_sbar->full_len > v_sbar->visible_len)
    {
        if((flen - OffsLine - 1) <= (v_sbar->visible_len))
        {
            v_sbar->actual_offset = v_sbar->offset = v_sbar->full_len - v_sbar->visible_len;
            bottomincr = lineHeight - (GridYS2 - GridYS1)%lineHeight;
			if(bottomincr == lineHeight)
			{
				bottomincr = 0;
			}
            offset_correction = true;
        }
        else if(offset_correction == true)
        {
            v_sbar->actual_offset = v_sbar->offset = (float)OffsLine;
    		bottomincr = 0;
            offset_correction = false;
    	}
    }
    else
    {
        v_sbar->active = false;
    }

    if(workPt->ptype == Patt_Pianoroll)
    {
        OffsLine = NUM_PIANOROLL_LINES - note_offset - 1;
    }
    else
    {
        OffsLine = (int)v_sbar->actual_offset;
    }

    ptdrawx1 = pattx1 + keywidth;
    ptdrawx2 = AuxX2 - 1;
    ptdrawy1 = GridYS1 - bottomincr;
    ptdrawy2 = GridYS2;
}

void Aux::DisableStepVUs()
{
    Instrument* i = first_instr;
    while(i != NULL)
    {
        i->stepvu->drawarea->Disable();
        i = i->next;
    }
}

bool Aux::CheckIfMouseOnBinder(int mouse_x, int mouse_y)
{
    if(auxmode == AuxMode_Pattern && workPt->ptype == Patt_Pianoroll &&
       mouse_x >= bindX && mouse_x <= bindX + keywidth &&
       mouse_y >= bindY && mouse_y <= bindY + 16)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void ChangeAuxLane(LaneType lt)
{
    aux_panel->ltype = lt;
    if(lt == Lane_Vol)
    {
        aux_panel->HandleButtDown(aux_panel->Vols1);
    }
    else if(lt == Lane_Pan)
    {
        aux_panel->HandleButtDown(aux_panel->Pans1);
    }
    else if(lt == Lane_Pitch)
    {
        aux_panel->Vols1->Release();
        aux_panel->Pans1->Release();
        aux_panel->workPt->OrigPt->AddEnvelope(Param_Pitch);

        R(Refresh_Buttons);
        R(Refresh_Aux);
    }

    MC->listen->CommonInputActions();
}

Numba::Numba(void* v)
{
    type = Panel_Number;

    width = 35;
    height = 30;
    kval = v;
    num = NULL;
    fnum = NULL;
    img = NULL;

    btup = new Butt(false);
    btdn = new Butt(false);

    AddNewControl(btup, this);
    AddNewControl(btdn, this);

    drawarea = new DrawArea((void*)this, Draw_Numba);
    MC->AddDrawArea(drawarea);
}

Numba::Numba(void* v, NumbaType nt)
{
    type = Panel_Number;

    width = 35;
    height = 30;

    kval = v;
    num = NULL;
    fnum = NULL;
    img = NULL;

    btup = new Butt(false);
    btdn = new Butt(false);

    AddNewControl(btup, this);
    AddNewControl(btdn, this);

    drawarea = new DrawArea((void*)this, Draw_Numba);
    MC->AddDrawArea(drawarea);

	ntype = nt;
    switch(nt)
    {
        case Numba_BPM:
        {
            strcpy(str, "BPM");
            fnum = &beats_per_minute;
        }break;
        case Numba_TPB:
        {
            strcpy(str, "TPB");
            num = &ticks_per_beat;
        }break;
        case Numba_BPB:
        {
            strcpy(str, "BPB");
            num = &beats_per_bar;
        }break;
        case Numba_Octave:
        {
            strcpy(str, "Oct.");
            num = &Octave;
        }break;
        case Numba_relOctave:
        {
            strcpy(str, "Rel.");
            num = &relOctave;
        }break;
    }
}

void Numba::HandleButtUp(Butt* bt)
{
    if(bt == btup)
    {
        if(num != NULL)
            *num = *num + 1;
        if(fnum != NULL)
            *fnum = *fnum + 1;
    }
    else if(bt == btdn)
    {
        if(num != NULL)
            *num = *num - 1;
        if(fnum != NULL)
            *fnum = *fnum - 1;
    }

    if(ntype == Numba_BPM)
    {
        UpdatePerBPM();

        //R(Refresh_GridContent);
        //if(gAux->auxmode == AuxMode_Pattern)
        //{
        //    R(Refresh_AuxContent);
        //}
    }
    else if(ntype == Numba_TPB)
    {
        if(*num < 1)
        {
           *num = 1;
        }
        else if(*num > 12)
        {
           *num = 12;
        }

        UpdateQuants();
        AdjustTick();
        aux_panel->AdjustTick();
        UpdatePerBPM();

        R(Refresh_Grid);
        if(aux_panel->isVolsPansMode())
        {
            R(Refresh_Aux);
        }
        else if(aux_panel->isPatternMode())
        {
            R(Refresh_AuxScale);
            R(Refresh_AuxGrid);
            R(Refresh_SubAux);
        }
    }
    else if(ntype == Numba_BPB)
    {
        if(*num < 1)
        {
           *num = 1;
        }
        else if(*num > 24)
        {
           *num = 24;
        }

        UpdateQuants();
        UpdatePerBPM();

        R(Refresh_Grid);
        if(aux_panel->isVolsPansMode())
        {
            R(Refresh_Aux);
        }
        else if(aux_panel->isPatternMode())
        {
            R(Refresh_AuxScale);
            R(Refresh_AuxGrid);
            R(Refresh_SubAux);
        }
    }
    else if(ntype == Numba_Octave)
    {
        if(*num < 1)
        {
           *num = 1;
        }
        else if(*num > 8)
        {
           *num = 8;
        }
    }
    else if(ntype == Numba_relOctave)
    {
        if(*num > 4)
        {
           *num = 4;
        }
        else if(*num < -4)
        {
           *num = -4;
        }
    }

    drawarea->Change();

    bt->Refresh();

    R(Refresh_Buttons);
}

void Numba::CheckVisibility(int xc)
{
    Control* ct = first_ctrl;
    while(ct != NULL)
    {
        if(ct->x > xc)
        {
            ct->active = false;
        }
        else
        {
            if(ct->x + ct->width > xc)
            {
                ct->width = xc - ct->x - 1;
            }
            ct->active = true;
        }
        ct = ct->pn_next;
    }

    if(x > xc)
    {
        x = y = - 1;
    }
    else if((x + width) > xc)
    {
        width = xc - x - 1;
    }
}

void OnBypassClick(Object* owner, Object* self)
{
//    Toggle* tt = (Toggle*) self;
//    Mixcell* mc = (Mixcell*) (tt->owner);

    //mc->effect->SavePresetAs("Test preset");
}

StaticArea::StaticArea()
{
    type = Panel_Static;

    num_ticks = 200;
    tick_offset = 0;

    patt = new Pattern("static", 0, -1, -1, -1, false);
    patt->patt = NULL;
    patt->tick_length = num_ticks;
    patt->end_tick = patt->start_tick + num_ticks;

    pixwidth = 320;

    sbar = new ScrollBard(Ctrl_HScrollBar, 0, 0, 16);
    sbar->active = true;

    AddNewControl(sbar, this);
}

void StaticArea::HandleButtDown(Butt* bt)
{

}
