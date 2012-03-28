
#include "VSTCollection.h"
#include "awful_jucewindows.h"
#include "awful_preview.h"
#include "awful_cursorandmouse.h"
#include "awful_utils_common.h"
#include "Data sources/images.h"

#define MAIN_WINDOW_TITLE_BAR_HEIGHT 18
#define CHILD_WINDOW_TITLE_BAR_HEIGHT 20

Colour  bkgcolour = Colour(114, 114, 114);
Colour  btmcolour = Colour(152, 152, 152);
Colour  btm1colour = Colour(70, 70, 70);
Colour  upperlinecolour = Colour(210, 210, 210);
Colour  leftlinecolour = Colour(158, 158, 158);
Colour  rightedgecolour = Colour(64, 64, 64);
Colour  rightedgecolour1 = Colour(180, 180, 180);

AwfulWindow::AwfulWindow() 
    : DocumentWindow (T("CMM beta"),
                      Colour(50, 114, 99), 
                      DocumentWindow::allButtons,
                      true)
{
    first_child = last_child = NULL;

    // Create an instance of our main content component, and add it 
    // to our window.
    MainComponent* const contentComponent = new MainComponent();
    MC = contentComponent;
    setContentComponent (contentComponent, true, true);
    //centreWithSize(getWidth(), getHeight());
    setTitleBarHeight(0);
    //setTitleBarButtonsRequired(0, false);
    tofronted = false;
    //setWantsKeyboardFocus(true);
}

AwfulWindow::~AwfulWindow()
{
    // Delete all kids
	ChildWindow* chldnext;
    ChildWindow* chld = first_child;
    while(chld != NULL)
    {
		chldnext = chld->next;
        delete chld;
        chld = chldnext;
    }
    // (the content component will be deleted automatically, so no need to do it here)
}

void AwfulWindow::UpdateTitle()
{
    char name[MAX_NAME_STRING];
    strcpy(name, "Chaotic - ");
    strcat(name, PrjData.projname);
    if(!PrjData.projpath.isEmpty())
        strcat(name, ".cmm");

    if(ChangesHappened)
        strcat(name, "*");

    setName(name);
}

void AwfulWindow::minimisationStateChanged(bool isNowMinimised)
{
    if(isNowMinimised)
        MinimizeChilds();
    else
        MaximizeChilds();
}

VSTParamWindow* AwfulWindow::CreateVSTParamWindow(VSTEffect * vsteff, Scope* scope)
{
    VSTParamWindow* wnd = new VSTParamWindow(vsteff, scope);
    AddChild(wnd);
    return wnd;
}

ParamWindow* AwfulWindow::CreateParamWindow(Scope* scope)
{
    ParamWindow* wnd = new ParamWindow(scope);
    AddChild(wnd);
    return wnd;
}

SynthWindow* AwfulWindow::CreateSynthWindow(Synth* syn)
{
    SynthWindow* wnd = new SynthWindow(syn);
    AddChild(wnd);
    return wnd;
}

ChildWindow* AwfulWindow::CreateAboutWindow()
{
    ChildWindow* wnd = new ChildWindow("About Chaotic DAW");
    AddChild(wnd);

    AboutComponent* const contentComponent = new AboutComponent();
    wnd->setContentComponent(contentComponent, true, true);
    wnd->centreWithSize(wnd->getWidth(), wnd->getHeight());

    return wnd;
}

#if(RELEASEBUILD == FALSE)
ChildWindow* AwfulWindow::CreateLicenseWindow()
{
    ChildWindow* wnd = new ChildWindow("Licenser");
    AddChild(wnd);

    LicenseComponent* const contentComponent = new LicenseComponent();
    wnd->setContentComponent(contentComponent, true, true);
    wnd->centreWithSize(wnd->getWidth(), wnd->getHeight());

    return wnd;
}
#endif

ChildWindow* AwfulWindow::CreateHotKeysWindow()
{
    ChildWindow* wnd = new ChildWindow("Keyboard reference");
    AddChild(wnd);

    HotKeysComponent* const contentComponent = new HotKeysComponent();
    wnd->setContentComponent(contentComponent, true, true);
    wnd->centreWithSize(wnd->getWidth(), wnd->getHeight());

    return wnd;
}

void AwfulWindow::mouseDoubleClick (const MouseEvent& e)
{
    const int border = getBorderSize();

    if (e.x >= border
         && e.y >= border
         && e.x < getWidth() - border
         && e.y < border + getTitleBarHeight())
    {
        //getMaximiseButton()->triggerClick();
        ToggleFullScreen();
    }
}

//==============================================================================
void AwfulWindow::closeButtonPressed()
{
    // When the user presses the close button, we'll tell the app to quit. This 
    // window will be deleted by our HelloWorldApplication::shutdown() method
    // 
    if(!CheckSave())
    {
        SaveSettings();
        JUCEApplication::quit();
    }
}

void AwfulWindow::mouseDown(const MouseEvent& e)
{
    DocumentWindow::mouseDown(e);
    ToFront();
}

bool AwfulWindow::keyPressed(const KeyPress& key)
{
    unsigned int flags = 0;
    ModifierKeys mods = key.getModifiers();
    if(mods.isCtrlDown())
    {
        flags |= kbd_ctrl;
    }
    if(mods.isShiftDown())
    {
        flags |= kbd_shift;
    }
    if(mods.isAltDown())
    {
        flags |= kbd_alt;
    }

	char kcode = (char)key.getKeyCode();
	unsigned k = TranslateKey(kcode);
    juce_wchar c = key.getTextCharacter();
	char ch = (char)c;

	if(k != 0 && ((k != 256 && k != 127) || (ch == 0)))
    {
		Process_KeyDown(k, flags);
    }	
    else if(ch != 0)
    {
        Process_Char(ch, flags);
    }
    else if(kcode != 0)
    {
        Process_Char(kcode, flags);
    }

    Component* com = getCurrentlyFocusedComponent();

    Vanish_CleanUp();

	MC->listen->CommonInputActions();
	return true;
}

bool AwfulWindow::keyStateChanged(bool isKeyDown)
{
    for(int ic = 0; ic < MAX_PREVIEW_ELEMENTS; ic++)
    {
        // Release any preview slot with key not pressed except if it's not under mouse
        if(!(M.pianokey_pressed == true && M.pianokey_slot == ic))
        {
            if(KeyPress::isKeyCurrentlyDown(PrevSlot[ic].key) == false)
            {
                if((PrevSlot[ic].state == PState_Playing && PrevSlot[ic].trigger.tgstate == TgState_Sustain))
                {
                    PrevSlot[ic].trigger.Release();
                }
                else if(PrevSlot[ic].state == PState_Ready)
                {
                    PrevSlot[ic].state = PState_Stopped;
                }

                if(PrevSlot[ic].keybound_pianokey == true)
                {
                    PrevSlot[ic].keybound_pianokey = false;
                }
                PrevSlot[ic].key = -1;
            }
        }
    }

    if(aux_panel->auxmode == AuxMode_Pattern && aux_panel->workPt->ptype == Patt_Pianoroll)
    {
	    R(Refresh_PianoKeys);
	    MC->listen->CheckAndRedraw();
    }

	return true;
}

void AwfulWindow::activeWindowStatusChanged()
{
    DocumentWindow::activeWindowStatusChanged();
    bool active = isActiveWindow();
}

void AwfulWindow::parentHierarchyChanged()
{
    DocumentWindow::parentHierarchyChanged();
}

void AwfulWindow::broughtToFront()
{
    if(tofronted == false)
    {
        ToFront();
    }
    else
    {
        tofronted = false;
    }
}

void AwfulWindow::ToFront()
{
    ChildWindow* cw = first_child;
    while(cw != NULL)
    {
        if(cw->Showing())
        {
            cw->bypassfront = true;
            cw->toFront(false);
            //if(cw->isVisible() == false)
            //{
            //    cw->setVisible(true);
            //}
            cw->bypassfront = false;
        }
		cw = cw->next;
    }

	// Refresh mixer highlights as they will gone after focus regain
	if(aux_panel != NULL && aux_panel->auxmode == AuxMode_Mixer)
	{
		R(Refresh_AuxHighlights);
		MC->listen->CommonInputActions();
	}
}

void AwfulWindow::MinimizeChilds()
{
    ChildWindow* cw = first_child;
    while(cw != NULL)
    {
        if(cw->Showing())
        {
            cw->setVisible(false);
        }
		cw = cw->next;
    }
}

void AwfulWindow::MaximizeChilds()
{
    ChildWindow* cw = first_child;
    while(cw != NULL)
    {
        if(cw->Showing())
        {
            cw->bypassfront = true;
			cw->setVisible(true);
			cw->bypassfront = false;
        }
		cw = cw->next;
    }
}

void AwfulWindow::AddChild(ChildWindow* cw)
{
    cw->parent = this;
    if(first_child == NULL && last_child == NULL)
    {
        cw->prev = NULL;
        cw->next = NULL;
        first_child = cw;
    }
    else
    {
        last_child->next = cw;
        cw->prev = last_child;
        cw->next = NULL;
    }
    last_child = cw;
    //getContentComponent()->addChildComponent(cw);
}

void AwfulWindow::RemoveChild(ChildWindow* cw)
{
	if((cw == first_child)&&(cw == last_child))
	{
		first_child = NULL;
		last_child = NULL;
	}
	else if(cw == first_child)
	{
		first_child= cw->next;
		first_child->prev = NULL;
	}
	else if(cw == last_child)
	{
		last_child= cw->prev;
		last_child->next = NULL;
	}
	else
	{
		if(cw->prev != NULL)
		{
		    cw->prev->next = cw->next;
        }
		if(cw->next != NULL)
		{
		    cw->next->prev = cw->prev;
        }
	}
}

void AwfulWindow::DeleteChild(ChildWindow* cw)
{
    RemoveChild(cw);
    delete cw;
}

void AwfulWindow::drawBorder(Graphics& g, int w, int h, const BorderSize& border, ResizableWindow&)
{
    g.fillAll(Colour(30, 95, 103));

    int hh = MAIN_WINDOW_TITLE_BAR_HEIGHT + 3;

    //g.setColour(Colour(40, 104, 91));
    //g.drawRect (1, 1, w - 2, h - 2);
    //g.drawRect (2, 2, w - 4, h - 4);

    g.setColour (Colour (205, 205, 255));
    g.drawRect(border.getLeft() - 3,
               border.getTop() + hh - 3,
               w + 6 - border.getLeftAndRight(),
               h + 1 - border.getTopAndBottom() - hh + 4);

//    g.setColour (Colour (54, 140, 118));
    g.drawRect(border.getLeft() - 2,
               border.getTop() + hh - 3,
               w + 4 - border.getLeftAndRight(),
               h + 1 - border.getTopAndBottom() - hh + 4);

    g.setColour(bkgcolour);
    g.fillRect(0, 0, w, hh + 1);

    // Upper line
    //g.setColour(Colour(0xff3D9DB5));
    g.setColour(upperlinecolour);
    g.fillRect(1, 0, (w - 2), 1);
    // Upper fat line
    g.setColour(Colour(140, 140, 140));
    g.fillRect(1, 1, (w - 1), 2);

    // Left side line
    g.setColour(leftlinecolour);
    g.fillRect(1, 1, 1, hh - 3);

    g.setColour(rightedgecolour1);
    g.fillRect(w - 2, 1, w - 1, hh - 1);

    // Right side line 2 and bottom line 2
    g.setColour(bkgcolour);
    g.fillRect(w - 1, 1, w - 1, hh);

    g.setColour(btm1colour);
    g.fillRect(0, hh - 1, w, 1);

    g.setColour(Colour (16, 71, 61));
    g.drawRect(0, hh, w, h - hh);
}

void AwfulWindow::paint(Graphics& g)
{
    drawBorder(g, getWidth(), getHeight(), getBorderThickness(), *this);

    const int border = getBorderSize();

    g.setOrigin (border, border);
    g.reduceClipRegion (0, 0, getWidth() - border * 2, getTitleBarHeight());

    int titleSpaceX1 = 6;
    int titleSpaceX2 = getWidth() - 6;

    for (int i = 0; i < 3; ++i)
    {
        if (titleBarButtons[i] != 0)
        {
            if (positionTitleBarButtonsOnLeft)
                titleSpaceX1 = jmax (titleSpaceX1, titleBarButtons[i]->getRight() + (getWidth() - titleBarButtons[i]->getRight()) / 8);
            else
                titleSpaceX2 = jmin (titleSpaceX2, titleBarButtons[i]->getX() - (titleBarButtons[i]->getX() / 8));
        }
    }

    int w = getWidth() - border * 2;
    int h = getTitleBarHeight();
    int titleSpaceX = titleSpaceX1;
    int titleSpaceW = jmax(1, titleSpaceX2 - titleSpaceX1);
    Image* icon = titleBarIcon;
    bool drawTitleTextOnLeft = !drawTitleTextCentred;

    const bool isActive = isActiveWindow();

/*
    // Main background
    g.setColour(bkgcolour);
    g.fillRect(0, 0, w, h);

    g.setColour(btmcolour);
    g.fillRect(0, h - 2, w, 1);

    g.setColour(btm1colour);
    g.fillRect(0, h - 1, w, 1);
*/

    // Lower fat line
    g.setColour(Colour(134, 134, 134));
    g.fillRect(0, h - 2, w , 1);

    g.setFont (*ari);
    int textW = ari->getStringWidth (getName());
    int iconW = 0;
    int iconH = 0;
    if (icon != 0)
    {
        iconH = (int) ari->getHeight();
        iconW = icon->getWidth() * iconH / icon->getHeight() + 4;
    }

    textW = jmin (titleSpaceW, textW + iconW);
    int textX = drawTitleTextOnLeft ? titleSpaceX
                                    : jmax (titleSpaceX, (w - textW) / 2);

    if (textX + textW > titleSpaceX + titleSpaceW)
        textX = titleSpaceX + titleSpaceW - textW;

    if(icon != 0)
    {
        g.setOpacity (isActive ? 1.0f : 0.6f);
        g.drawImageWithin (icon, textX, (h - iconH) / 2, iconW, iconH,
                           RectanglePlacement::centred, false);
        textX += iconW;
        textW -= iconW;
    }

    g.setColour(Colour(0xffFFFFFF));
    g.setFont(*ari);
    g.drawSingleLineText(getName(), 3, 12);
}

VSTParamWindow::VSTParamWindow(VSTEffect* vsteff, Scope* sc) : PluginCommonWindow()
{
    setName(vsteff->name);
    scope = sc;
    savepresetbt->setVisible(false);
    //loadpresetbt->setBounds(getWidth() - 57, 2, 20, 19);

    VSTParamComponent* const contentComponent = new VSTParamComponent(vsteff, sc);
    setContentComponent(contentComponent, true, true);
    //centreWithSize(getWidth(), getHeight());
}

VSTParamWindow::~VSTParamWindow()
{
}

PluginCommonWindow::PluginCommonWindow() : ChildWindow("")
{
    loadpresetbt = new LoadPresetButton("");
    Component::addAndMakeVisible(loadpresetbt);
    loadpresetbt->addButtonListener(this);
    savepresetbt = new SavePresetButton("");
    Component::addAndMakeVisible(savepresetbt);
    savepresetbt->addButtonListener(this);
}

void PluginCommonWindow::closeButtonPressed()
{
    if(scope->instr != NULL)
    {
        scope->instr->pEditorButton->Release();
        scope->instr->instr_drawarea->Change();
    }
    ChildWindow::closeButtonPressed();
}

PluginCommonWindow::~PluginCommonWindow()
{
    delete loadpresetbt;
    delete savepresetbt;
}

void PluginCommonWindow::resized()
{
    DocumentWindow::resized();
    loadpresetbt->setBounds(getWidth() - 78, 2, 22 , 19);
    savepresetbt->setBounds(getWidth() - 57, 2, 20, 19);
}

void PluginCommonWindow::buttonClicked(Button * button)
{
    if(button == loadpresetbt)
    {
        if(scope != NULL)
        {
            if(scope->instr != NULL)
            {
                DoLoadPreset(scope->instr);
            }
            else if(scope->eff != NULL)
            {
                DoLoadPreset(scope->eff);
            }
            MC->listen->CommonInputActions();
        }
    }
    else if(button == savepresetbt)
    {
        if(scope != NULL)
        {
            if(scope->instr != NULL)
            {
                DoSavePreset(scope->instr);
            }
            else if(scope->eff != NULL)
            {
                DoSavePreset(scope->eff);
            }
            MC->listen->CommonInputActions();
        }
    }
}

ParamWindow::ParamWindow(Scope* sc) : PluginCommonWindow()
{
    scope = sc;
    ParamComponent* const contentComponent = new ParamComponent(sc);
    setContentComponent(contentComponent, true, true);
    setName(contentComponent->getName());
    centreWithSize(getWidth(), getHeight());
}

ParamWindow::~ParamWindow()
{
}

#if(RELEASEBUILD == FALSE)
#define PRIV_KEY "395e5a740a571602e702c52067c8823221f164b224378e0cad901a4fb0f4dab5,8f6be22219d9b7074186ecd10375457ed6268a222ce74c2ec38602c2632869e1"

LicenseComponent::LicenseComponent()
{
    setSize(293, 191);

    addAndMakeVisible(userName = new TextEditor(T("UserName")));
    addAndMakeVisible(userEmail = new TextEditor(T("UserEmail")));

    userName->setFont(*ins);
    userName->setColour(TextEditor::textColourId, Colour(244, 244, 244));
    userName->setColour(TextEditor::backgroundColourId, Colour(4, 53, 46));
    userName->setColour(TextEditor::focusedOutlineColourId, Colour((uint8)208, (uint8)228, (uint8)218, (uint8)192));
    userName->setColour(TextEditor::outlineColourId, Colour(36, 158, 133));
    userName->setColour(TextEditor::shadowColourId, Colour(19, 84, 72));
    userName->setIndents(4, 4);
    userName->setText(T("UserName"));

    userEmail->setFont(*ins);
    userEmail->setColour(TextEditor::textColourId, Colour(244, 244, 244));
    userEmail->setColour(TextEditor::backgroundColourId, Colour(4, 53, 46));
    userEmail->setColour(TextEditor::focusedOutlineColourId, Colour((uint8)208, (uint8)228, (uint8)218, (uint8)192));
    userEmail->setColour(TextEditor::outlineColourId, Colour(36, 158, 133));
    userEmail->setColour(TextEditor::shadowColourId, Colour(19, 84, 72));
    userEmail->setIndents(4, 4);
    userEmail->setText(T("UserEmail"));

    addAndMakeVisible(generateButton = new ATextButton(T("Generate!")));
    generateButton->addButtonListener(this);

    userName->setBounds(10, 11, 211, 20);
    userEmail->setBounds(10, 41, 211, 20);
    generateButton->setBounds(10, 71, 73, 33);
}

void LicenseComponent::buttonClicked(Button* button)
{
    if(button == generateButton)
    {
        XmlElement xmlLicense(T("CMMSingleUserLicenseData"));
        xmlLicense.setAttribute(T("UserName"), userName->getText());
        xmlLicense.setAttribute(T("UserEmail"), userEmail->getText());

        /*
        Time time = Time::getCurrentTime();
        xmlLicense.setAttribute(T("Day"), time.getDayOfMonth());
        xmlLicense.setAttribute(T("Month"), time.getMonth());
        xmlLicense.setAttribute(T("Year"), time.getYear());*/

        xmlLicense.setAttribute(T("CMMVersion"), (int)rVer);

        BitArray val;

        const String s(xmlLicense.createDocument (String::empty, true));
    	char buff[1024];
    	s.copyToBuffer(buff, s.length());
        const MemoryBlock mb(buff, s.length());

        val.loadFromMemoryBlock (mb);

    	RSAKey privkey(PRIV_KEY);

    	privkey.applyToValue(val);

    	File licfile(userName->getText() + T("/") + T("CMMSingleUserLicenseKey.license"));
    	licfile.deleteFile();
    	licfile.create();
    	licfile.appendText(val.toString (16));
}
}

void LicenseComponent::paint(Graphics & g)
{

}

#endif

HotKeysComponent::HotKeysComponent()
{
    setSize(375, 377);

    hotkeysimg = ImageFileFormat::loadFrom(images::hotkeys_png, images::hotkeys_pngSize);

    //addAndMakeVisible(group = new AGroupComponent(T("Group"), T("")));
    //group->setBounds(0, 1, getWidth(), getHeight() - 1);

    int x1 = 5;
    int y1 = 5;
    Colour clr = Colour(120, 185, 135);
    PlaceBigLabel("Left mouse button over grid = Start selection", 
                    x1, y1, clr);
    PlaceBigLabel("Alt + Left mouse button over grid = Brush with current instrument or element", 
                    x1, y1 + 12, clr);
    PlaceBigLabel("Shift + Left mouse button over grid = Draw slide-note", 
                    x1, y1 + 12 + 12, clr);
    PlaceBigLabel("Shift + drag element = Clone element", 
                    x1, y1 + 12 + 12 + 12, clr);
    PlaceBigLabel("Shift + Ctrl + drag pattern = Copy pattern to new one", 
                    x1, y1 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("Ctrl + Mouse wheel = Zoom at mouse position", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("Shift + Left mouse button on any instrument = reassign selected notes to this instrument", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12, clr);

    PlaceBigLabel("Ctrl + 'C' = Copy selected elements", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("Ctrl + 'X = Cut selected elements", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("Ctrl + 'V' = Paste elements at mouse position", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("Ctrl + 'Z' = Undo", 
					x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("Ctrl + 'Y' = Redo", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("Ctrl + 'A' = Select all", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("Ctrl + 'D' = Mute/Unmute pattern(s)", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("Ctrl + 'Q' = Make pattern(s) unique", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);

    PlaceBigLabel("'////' = Slide", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("'####' = Mute", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("'~~~~' = Vibrato", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("'%%%%' = Transpose", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("'|' = Break", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("'<' = Reverse", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("'>' = No reverse (only when cursor is on sample)", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("'{' = Place bookmark", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);

    PlaceBigLabel("Space = Start/stop playback on the main field_pattern", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("Esc = Switch note mode ON/OFF", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("Tab/Shift + Tab = Advance cursor forward/backward BPB times", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceBigLabel("F3 = Toggle mixer", 
                    x1, y1 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);
}

void HotKeysComponent::buttonClicked(Button* button)
{

}

void HotKeysComponent::paint(Graphics & g)
{
    g.saveState();
    g.reduceClipRegion(0, 4, getWidth() - 1, getHeight() - 5);
    g.drawImageAt(hotkeysimg, getWidth() - hotkeysimg->getWidth(), 2, false);
    g.restoreState();
}

AboutComponent::AboutComponent()
{
    setSize(393, 257);
    //AGroupComponent* group = new AGroupComponent(T("Group"), T(""));
    //addAndMakeVisible(group);
    //group->setBounds(0, 2, getWidth() - 1, getHeight() - 2);

    keysimg = ImageFileFormat::loadFrom(images::keysabout_png, images::keysabout_pngSize);

    int x1 = 3;
    int y1 = 6;
    Colour clr = Colour(120, 185, 135);
    PlaceTxtLabel1("Version 1.08", 
                    x1, y1, Colour(170, 225, 185));

    PlaceTxtLabel1("Visit program website at www.chaoticdaw.com", 
                    x1, y1 + 24, clr);

    PlaceTxtLabel1("", 
                    x1, y1 + 36, clr);

    PlaceTxtLabel1("DSP algorithms by Robin Schmidt (www.rs-met.com)", 
                    x1, y1 + 36 + 12, clr);

    PlaceTxtLabel1("VST and ASIO are registered trademarks of Steinberg GmbH", 
                    x1, y1 + 36 + 12 + 12 + 12, clr);

    PlaceTxtLabel1("Chaotic also uses the following additional libraries:", 
                    x1, y1 + 36 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceTxtLabel1("    - Libsndfile library © by Eric de Castro Lopo;", 
                    x1, y1 + 36 + 12 + 12 + 12 + 12 + 12 + 12, clr);
    PlaceTxtLabel1("    - JUCE library © by Raw Material Software", 
                    x1, y1 + 36 + 12 + 12 + 12 + 12 + 12 + 12 + 12, clr);

    HyperlinkButton* hbutt = new HyperlinkButton(T("www.rs-met.com"), URL(T("http://rs-met.com")));
    hbutt->setFont(*rox, false, Justification::left);
    addAndMakeVisible(hbutt);
    hbutt->setBounds(169, y1 + 50, 92, 15);

    hbutt = new HyperlinkButton(T("www.chaoticdaw.com"), URL(T("http://chaoticdaw.com")));
    hbutt->setFont(*rox, false, Justification::left);
    addAndMakeVisible(hbutt);
    hbutt->setBounds(236, y1 + 26, 112, 15);
}

void AboutComponent::buttonClicked(Button* button)
{

}

void AboutComponent::paint(Graphics & g)
{
    g.saveState();
    g.reduceClipRegion(0, 4, getWidth(), getHeight() - 5);
    g.drawImageAt(keysimg, 0, 2, false);
    g.restoreState();
}

ParamComponent::ParamComponent(Scope* sc)
{
    scope = sc;
    pEff = NULL;
    instr = NULL;

    Parameter* first_param = NULL;
    if(scope->instr != NULL)
    {
        instr = scope->instr;
        first_param = instr->first_param;
        setName(instr->name);
    }
    else if(scope->eff != NULL)
    {
        pEff = scope->eff;
        first_param = pEff->first_param;
        setName(pEff->name);
    }
    firstparam = first_param;

    if(first_param != NULL)
    {
        AGroupComponent* Group = new AGroupComponent(T("Group"), T(""));
        addAndMakeVisible(Group);

        int x = 2;
        int y = 2;
        int my = y;
        ASlider* slider;
        ALabel* label;
        //AGroupComponent* tgroup = NULL;
        bool tgroup = false;
        int gx, gy, gw, gh;
        int radgroup = 100;

        char paramname[MAX_PARAM_NAME];
        Parameter* param = first_param;
        while(param != NULL)
        {
            strcpy(paramname, param->name);

            if(param->type == Param_Bool)
            {
                if(tgroup == NULL)
                {
                    y += 0;
                    tgroup = true;
                    //tgroup = new AGroupComponent(T("Group"), T(""));
                    //addAndMakeVisible(tgroup);
                    gx = x + 1;
                    gy = y - 2;
                    gw = 140;
                    gh = 17;
                }
                else
                {
                    gh += 12;
                }

                BoolParam* bp = (BoolParam*)param;
                AToggleButton* tg = new AToggleButton(bp->vstring->outstr);
                addAndMakeVisible(tg);
                tg->setToggleState(bp->outval, false);
                if(bp->grouped == true)
                {
                    tg->setRadioGroupId(radgroup);
                }
                else if(y > 6) // if there's already a param above
                {
                    y += 4;
                }

                tg->setBounds(x + 3, y + 8, ins->getStringWidth(bp->vstring->outstr) + 45, 22);
                tg->setName(paramname);
                bp->atoggle = tg;
                tg->addButtonListener(this);

                y += 12;
            }
            else
            {
                // End of group of toggles
                if(tgroup == true)
                {
                    radgroup++;
                    y += 5;
                    gh += 10;
                    //tgroup->setBounds(gx, gy, gw, gh);
                    tgroup = false;
                }

                strcat(paramname, ":");
                addAndMakeVisible(slider = new ASlider(T("")));
                slider->setSigned(param->GetSigned());
                slider->setRange(param->offset, param->range + param->offset);
                slider->setPopupMenuEnabled (true);
                //pBufferSlider->setValue(Random::getSystemRandom().nextDouble() * 100, false, false);
                slider->setValue(param->val, false, false);

                slider->setSliderStyle(ASlider::LinearHorizontal);
                slider->setTextBoxStyle(ASlider::TextBoxPositioned, true, 150, 20);
                slider->setTextBoxIsEditable(false);

                String str1(param->name);
                str1.trim();
                str1.append(T(":"), 1);

                String str(param->vstring->outstr);
                str.trim();
                slider->setText(str, false);
                //slider->setTextColour(Colour(155, 255, 155));
                slider->setTextColour(Colour(170, 185, 195));
                slider->setTextBoxXY(ti->getStringWidth(str1) + 3, -2);
                slider->setBounds(x + 4, y + 5, 115, 22);
                slider->setParamIndex(param->index);
                slider->setParameter(param);
                slider->addListener(this);
                param->aslider = slider;

                addAndMakeVisible(label = new ALabel("", str1));
                label->setFont(*ti);
                label->setColour(ALabel::textColourId, Colour(140, 155, 162));
                label->setBounds(x + 4, y, ti->getStringWidth(str1) + 10, 17);

                y += 24;
            }

            if(y > my)
            {
                my = y;
            }

            if(0 && param->next != NULL)
            {
                x += 155;
                y = 10;
            }

            param = param->next;
        }

        if(tgroup)
        {
            y += 7;
            gh += 10;
            //tgroup->setBounds(gx, gy, gw, gh);
            tgroup = false;
        }
        else
        {
            y += 4;
        }

        setSize(x + 145, y + 6);
        Group->setBounds(0, 1, getWidth(), getHeight() - 1);
    }
	else
	{
		setSize(155, 25);
	}
}

void ParamComponent::buttonClicked(Button * button)
{
    Parameter* param = firstparam;
    while(param != NULL)
    {
        if(param->type == Param_Bool)
        {
            BoolParam* bp = (BoolParam*)param;
            if(bp->atoggle == button)
            {
                AToggleButton* atg = (AToggleButton*)button;
                bp->SetBoolValue(atg->getToggleState());
            }
        }
        param = param->next;
    }

    if(MC->refresh != 0)
    {
        MC->listen->CommonInputActions();
    }
}

void ParamComponent::sliderValueChanged(ASlider* slider)
{
    int idx = slider->getParamIndex();
    Parameter* param = pEff->first_param;
    while(param != NULL)
    {
        if(param->index == idx)
        {
            float val = (float)slider->getValue();
            if(param->val != val)
            {
                param->SetDirectValueFromControl(val, true);
            }
            slider->setText(String(param->vstring->outstr), false);
            break;
        }
        param = param->next;
    }
}

VSTParamComponent::VSTParamComponent(VSTEffect* vsteff, Scope* sc)
{
    first_pcell = last_pcell = NULL;
    scope = sc;
    pEff = vsteff;
    if(vsteff->first_param != NULL && vsteff->last_param != NULL)
    {
        AGroupComponent* Group = new AGroupComponent(T("Group"), T(""));
        addAndMakeVisible(Group);

        int x = 2;
        int y = 2;
        int cnt = 0;
        ASlider* slider;
        ALabel* label;
        //Paramcell* pcell;
        int num = vsteff->last_param->index + 1;
        int cnum = num;
        int hgt = cnum*25;
        int rnum = num/cnum;
        while(cnum > 0 && (cnum*25)/(rnum*155) > 0.6f)
        {
            rnum = num/cnum;
            cnum--;
        }

        char paramname[MAX_PARAM_NAME];
        Parameter* param = vsteff->first_param;
        while(param != NULL)
        {
            strcpy(paramname, param->name);
            strcat(paramname, ":");

            addAndMakeVisible(slider = new ASlider(T("BufferSlider")));
            slider->setRange(0, 1);
            slider->setValue(param->outval, false, false);

            slider->setSliderStyle(ASlider::LinearHorizontal);
            slider->setTextBoxStyle(ASlider::TextBoxPositioned, true, 150, 20);
            slider->setTextBoxIsEditable(false);
            slider->setPopupMenuEnabled(true);

            String str1(param->name);
            str1 = str1.trim();
            str1.append(T(":"), 1);

            String str(param->vstring->label);
            str = str.trim();
            slider->setText(str, false);
            //slider->setTextColour(Colour(155, 255, 155));
            slider->setTextColour(Colour(170, 185, 195));
            slider->setTextBoxXY(ti->getStringWidth(str1) + 3, -2);
            slider->setBounds(x + 4, y + 5, 125, 22);
            slider->setParamIndex(param->index);
            slider->setParameter(param);
            slider->addListener(this);
            param->aslider = slider;

            addAndMakeVisible(label = new ALabel("", str1));
            label->setFont(*ti);
            label->setColour(ALabel::textColourId, Colour(140, 155, 162));
            label->setBounds(x + 4, y, ti->getStringWidth(str1) + 10, 17);

            y += 24;
            cnt++;
            if(cnt == cnum && param->next != NULL)
            {
                cnt = 0;
                x += 130;
                y = 2;
            }

            param = param->next;
        }

        setSize(x + 136, 24*cnum + 12);
        Group->setBounds(0, 1, getWidth(), getHeight() - 1);
    }
	else
	{
		setSize(135, 25);
	}
}

void VSTParamComponent::sliderValueChanged(ASlider* slider)
{
    int idx = slider->getParamIndex();
    Parameter* param = pEff->first_param;
    while(param != NULL)
    {
        if(param->index == idx)
        {
            float val = (float)slider->getValue();
            if(param->val != val)
            {
                param->SetDirectValueFromControl(val, true);
            }
            slider->setText(String(param->vstring->label).trim(), false);
            break;
        }
        param = param->next;
    }
}

void VSTParamComponent::AddParamcell(Paramcell* pcell)
{
    if(first_pcell == NULL && last_pcell == NULL)
    {
        pcell->prev = NULL;
        pcell->next = NULL;
        first_pcell = pcell;
    }
    else
    {
        last_pcell->next = pcell;
        pcell->prev = last_pcell;
        pcell->next = NULL;
    }
    last_pcell = pcell;
}

void VSTParamComponent::RemoveParamcell(Paramcell* pcell)
{
	if((pcell == first_pcell)&&(pcell == last_pcell))
	{
		first_pcell = NULL;
		last_pcell = NULL;
	}
	else if(pcell == first_pcell)
	{
		first_pcell = pcell->next;
		first_pcell->prev = NULL;
	}
	else if(pcell == last_pcell)
	{
		last_pcell = pcell->prev;
		last_pcell->next = NULL;
	}
	else
	{
		if(pcell->prev != NULL)
		{
		    pcell->prev->next = pcell->next;
        }
		if(pcell->next != NULL)
		{
		    pcell->next->prev = pcell->prev;
        }
	}
}

ChildWindow::ChildWindow(const char* name)
    : DocumentWindow (T("Child"),
                      /*Colour(33, 86, 75),*/
                      Colour(64, 79, 83), 
                      DocumentWindow::allButtons,
                      true)
{
    prev = next = NULL;
    parent = NULL;
    bypassfront = false;

    setName(String(name));
    setTitleBarButtonsRequired(closeButton, false);
    setAlwaysOnTop(false);
    setVisible(false);
    setResizable(false, false);
    setTitleBarHeight(CHILD_WINDOW_TITLE_BAR_HEIGHT);
    setDropShadowEnabled(false);
    Hide();

    wndtitle1 = ImageFileFormat::loadFrom(images::wndtitle1_png, images::wndtitle1_pngSize);
    wndtitle2 = ImageFileFormat::loadFrom(images::wndtitle2_png, images::wndtitle2_pngSize);
    wndtitle3 = ImageFileFormat::loadFrom(images::wndtitle3_png, images::wndtitle3_pngSize);
}

ChildWindow::~ChildWindow()
{
    // (the content component will be deleted automatically, so no need to do it here)
}

int ChildWindow::getDesktopWindowStyleFlags() const
{
    int flags = 0;
    flags |= ComponentPeer::windowHasDropShadow;

    return flags;
}

bool ChildWindow::Showing()
{
	return visible;
}

void ChildWindow::Show()
{
    visible = true;
    setVisible(true);
}

void ChildWindow::Hide()
{
    visible = false;
    setVisible(false);
}

void ChildWindow::drawChildWindowBorder (Graphics& g, int w, int h,
                                             const BorderSize& border, ResizableWindow&)
{
    int hh = 23;

    //g.setColour(Colour(40, 104, 91));
    //g.drawRect (1, 1, w - 2, h - 2);
    //g.drawRect (2, 2, w - 4, h - 4);

    Colour  bkgcolour = Colour(114, 114, 114);
    Colour  upperlinecolour = Colour(190, 190, 190);
    Colour  leftlinecolour = Colour(158, 158, 158);
    Colour  btmcolour = Colour(152, 152, 152);
    Colour  btm1colour = Colour(70, 70, 70);
    Colour  rightedgecolour = Colour(64, 64, 64);
    Colour  rightedgecolour1 = Colour(180, 180, 180);

    g.setColour (Colour (51, 148, 122));
    g.setColour (Colour (112, 132, 137));
    g.drawRect (1,
                hh + 1,
                w - 2,
                h - hh - 2);

    /*
    g.setColour(bkgcolour);
    g.fillRect(0, 0, w, hh + 1);

    // Upper line
    //g.setColour(Colour(0xff3D9DB5));
    g.setColour(upperlinecolour);
    g.drawHorizontalLine(0, 0, (float)(w - 1));

    // Upper fat line
    g.setColour(Colour(140, 140, 140));
    g.fillRect(0, 1, (w - 1), 2);

    //g.setColour(Colour(152, 152, 152));
    //g.drawHorizontalLine(1, 2, (float)(w - 1));

    // Left side line and bottom line 1
    g.setColour(leftlinecolour);
    g.drawVerticalLine(0, 1.f, float(hh - 1));
    g.setColour(btmcolour);
    g.drawHorizontalLine(hh - 2, 0, (float)(w - 1));

    // Bottom fat line
    g.setColour(Colour(124, 124, 124));
    g.fillRect(1, hh - 4, (w - 3), 2);

    g.setColour(btm1colour);
    g.fillRect(0, hh - 1, w, 1);

    // Right side line 2 and bottom line 2
    g.setColour(rightedgecolour1);
    g.fillRect(w - 2, 1, w - 1, hh - 2);

    g.setColour(rightedgecolour);
    g.fillRect(w - 1, 0, w - 1, hh);*/

    g.setColour(Colour (16, 71, 61));
    g.setColour(Colour (51, 61, 65));
    g.drawRect(0, hh, w, h - hh);

    int xw = 0;
    g.drawImageAt(wndtitle1, 0, 0, false);
    xw += wndtitle1->getWidth();
    while(xw < w)
    {
        g.drawImageAt(wndtitle2, xw, 0, false);
        xw += wndtitle2->getWidth();
    }
    g.drawImageAt(wndtitle3, w - wndtitle3->getWidth(), 0, false);
}

void ChildWindow::paint(Graphics& g)
{
    //ResizableWindow::paint(g);
    getLookAndFeel().fillResizableWindowBackground (g, getWidth(), getHeight(),
                                                    getBorderThickness(), *this);

    if (! isFullScreen())
    {
        if(resizableBorder != 0)
        {
            drawChildWindowBorder(g, getWidth(), getHeight(), getBorderThickness(), *this);
        }
        else
        {
            drawChildWindowBorder(g, getWidth(), getHeight(), BorderSize(4), *this);
        }
    }

/*
    if(resizableBorder == 0 && getBorderSize() == 1)
    {
        g.setColour (getBackgroundColour().overlaidWith(Colour (0x80000000)));
        g.drawRect (0, 0, getWidth(), getHeight());
    }
*/

    const int border = getBorderSize();

    g.setOrigin (border, border);
    g.reduceClipRegion (0, 0, getWidth() - border * 2, getTitleBarHeight());

    int titleSpaceX1 = 6;
    int titleSpaceX2 = getWidth() - 6;

    for (int i = 0; i < 3; ++i)
    {
        if (titleBarButtons[i] != 0)
        {
            if (positionTitleBarButtonsOnLeft)
                titleSpaceX1 = jmax (titleSpaceX1, titleBarButtons[i]->getRight() + (getWidth() - titleBarButtons[i]->getRight()) / 8);
            else
                titleSpaceX2 = jmin (titleSpaceX2, titleBarButtons[i]->getX() - (titleBarButtons[i]->getX() / 8));
        }
    }

    int w = getWidth() - border * 2;
    int h = getTitleBarHeight();
    int titleSpaceX = titleSpaceX1;
    int titleSpaceW = jmax(1, titleSpaceX2 - titleSpaceX1);
    Image* icon = titleBarIcon;
    bool drawTitleTextOnLeft = !drawTitleTextCentred;

    const bool isActive = isActiveWindow();

/*
    // Main background
    g.setColour(bkgcolour);
    g.fillRect(0, 0, w, h);

    g.setColour(Colour(130, 130, 130));
    g.fillRect(0, h - 3, w - 2, 1);

    g.setColour(btmcolour);
    g.fillRect(0, h - 2, w, 1);

    g.setColour(btm1colour);
    g.fillRect(0, h - 1, w, 1);
*/

    g.setFont (h * 0.65f, Font::bold);

    int textW = ins->getStringWidth (getName());
    int iconW = 0;
    int iconH = 0;

    if (icon != 0)
    {
        iconH = (int) ins->getHeight();
        iconW = icon->getWidth() * iconH / icon->getHeight() + 4;
    }

    textW = jmin (titleSpaceW, textW + iconW);
    int textX = drawTitleTextOnLeft ? titleSpaceX
                                    : jmax (titleSpaceX, (w - textW) / 2);

    if (textX + textW > titleSpaceX + titleSpaceW)
        textX = titleSpaceX + titleSpaceW - textW;

    if(icon != 0)
    {
        g.setOpacity (isActive ? 1.0f : 0.6f);
        g.drawImageWithin (icon, textX, (h - iconH) / 2, iconW, iconH,
                           RectanglePlacement::centred, false);
        textX += iconW;
        textW -= iconW;
    }

    g.setColour(Colour(0xffFFFFFF));
    g.setFont(*ins);
    g.drawSingleLineText(getName(), 5, 13);
}

void ChildWindow::parentHierarchyChanged()
{
    DocumentWindow::parentHierarchyChanged();
}

void ChildWindow::broughtToFront()
{
    if(bypassfront == false)
    {
        if(parent != NULL)
        {
            parent->RemoveChild(this);
            parent->AddChild(this);
        }
    }
}

void ChildWindow::mouseDown()
{
    if(parent != NULL)
    {
        parent->RemoveChild(this);
        parent->AddChild(this);
    }
}

void ChildWindow::closeButtonPressed()
{
    Hide();
}

void ChildWindow::lookAndFeelChanged()
{
    int i;
#ifdef USE_OLD_JUCE
    for (i = 0; i < 3; ++i)
        deleteAndZero (titleBarButtons[i]);
#else
	for (i = numElementsInArray (titleBarButtons); --i >= 0;)
		titleBarButtons[i] = 0;
#endif

    if (! isUsingNativeTitleBar())
    {
        titleBarButtons[0] = ((requiredButtons & minimiseButton) != 0)
                                ? createDocumentWindowButton (minimiseButton) : 0;

        titleBarButtons[1] = ((requiredButtons & maximiseButton) != 0)
                                ? createDocumentWindowButton (maximiseButton) : 0;

        titleBarButtons[2] = ((requiredButtons & closeButton) != 0)
                                ? createDocumentWindowButton (closeButton) : 0;

        for (i = 0; i < 3; ++i)
        {
            if (titleBarButtons[i] != 0)
            {
                buttonListener.owner = this;
                titleBarButtons[i]->addButtonListener (&buttonListener);
                titleBarButtons[i]->setWantsKeyboardFocus (false);

                // (call the Component method directly to avoid the assertion in ResizableWindow)
                Component::addAndMakeVisible (titleBarButtons[i]);
            }
        }

        if (getCloseButton() != 0)
        {
            getCloseButton()->addShortcut (KeyPress (KeyPress::F4Key, ModifierKeys::altModifier, 0));
        }
    }

    activeWindowStatusChanged();

    ResizableWindow::lookAndFeelChanged();
}

EnvelopeComponent::EnvelopeComponent()
{
    x = 2;
    y = 2;

    v_only = false;
    drawscale = false;
    drawnumbers = false;

    env = new Envelope(Cmd_ParamEnv);
    env->len = 3;
    env->timebased = true;

    env->AddPoint(0.0f, 1.0f);
    env->AddPoint(0.2f, 1.0f);
    env->AddPoint(0.3f, 0.5f);
    env->SetSustainPoint(env->AddPoint(0.6f, 0.0f));
    env->sustainable = true;
}

EnvelopeComponent::EnvelopeComponent(Envelope* nenv)
{
    x = 2;
    y = 2;

    v_only = false;
    drawscale = false;
    drawnumbers = false;
    env = nenv;
}

EnvelopeComponent::~EnvelopeComponent()
{
}

void EnvelopeComponent::mouseExit(const MouseEvent &e)
{
    if(env_action != ENVUSUAL)
    {
        env_action = ENVUSUAL;
        repaint();
    }
}

void EnvelopeComponent::setEnvelope(Envelope* nenv)
{
    env = nenv;
    repaint();
}

void EnvelopeComponent::mouseDrag(const MouseEvent &e)
{
    if(env != NULL)
    {
        jassert(env_action >= ENVUSUAL && env_action <= ENVLINEMOVE);
        jassert(active_pnt == NULL || active_pnt->y_norm < 2);

        int mouse_x = e.getMouseDownX() + e.getDistanceFromDragStartX();
        int mouse_y = e.getMouseDownY() + e.getDistanceFromDragStartY();

        int py = y + env->hgt - mouse_y;
        if(py < 0)
            py = 0;
        if(py > env->hgt)
            py = env->hgt;

        float yval = (float)py/env->hgt;
        int px;
        if(v_only == true)
        {
            px = 0;
        }
        else
        {
            px = mouse_x - x;
        
            if(px < 0)
                px = 0;
            if(px > env->edx)
                px = env->edx;
        }
        float env_xcurr = (float)px/env->scale;
        
        if(env_action == ENVPOINTING)
        {
            float oldx = active_pnt->x;
            active_pnt->x = (float)px/env->scale;
            active_pnt->y_norm = yval;
            if(active_pnt != env->p_first)
            {
                if(active_pnt->prev != NULL && active_pnt->x < active_pnt->prev->x)
                {
                    active_pnt->x = active_pnt->prev->x;
                }
                else if(active_pnt->next != NULL && active_pnt->x > active_pnt->next->x)
                {
                    active_pnt->x = active_pnt->next->x;
                }
            }

            if(e.mods.isShiftDown() && active_pnt->next != NULL)
            {
                float dx = active_pnt->x - oldx;
                EnvPnt* ep = active_pnt->next;
                while(ep != NULL)
                {
                    ep->x += dx;
                    ep = ep->next;
                }
            }
			env->UpdateLastValue();
			
			//if(v_only == false)
            //{
            //    env->RelocatePoint(active_pnt);
            //}
        }
        else if(env_action == ENVDRAWMOVE)
        {
            env->DeleteRange(env_xcurr, env_xstart, true);
        
            if(px > 0)
            {
                env->AddPoint((float)px/env->scale, yval);
            }
            else
            {
                env->p_first->x = (float)px/env->scale;
                env->p_first->y_norm = (float)py/env->hgt;
            }
            env_xstart = env_xcurr;
        }
        else if(env_action == ENVDELETING)
        {
            if(px > 0)
            {
                env->DeleteRange(env_xcurr, env_xstart, false);
            }
            else
            {
                env->p_first->x = (float)px/env->scale;
                env->p_first->y_norm = (float)py/env->hgt;
            }
        }
        else if(env_action == ENVLINEMOVE)
        {
            env->DeleteRange(env_xcurr, env_xstart, true);
        
            if(px > 0)
            {
                env->AddPoint((float)px/env->scale, yval);
            }
            else
            {
                env->p_first->x = (float)px/env->scale;
                env->p_first->y_norm = (float)py/env->hgt;
            }
        }
        else if(env_action == ENVSUSTAINING)
        {
            EnvPnt* p = env->p_first;
            EnvPnt* pl = p;
            while(p != NULL)
            {
                if(abs(p->x - env_xcurr) < abs(pl->x - env_xcurr))
                {
                    pl = p;
                }
                p = p->next;
            }
            env->SetSustainPoint(pl);
        }
        env->CalcTime();

        repaint();
    }
}

void EnvelopeComponent::mouseMove(const MouseEvent &e)
{
    if(env != NULL)
    {
        active_pnt = NULL;
        EnvAction old_action = env_action;
        env_action = ENVUSUAL;

        int mouse_x = e.getMouseDownX();
        int mouse_y = e.getMouseDownY();

        v_only = false;
        EnvPnt* p = env->p_first;
        while(p != NULL)
        {
            if(fabs(mouse_x - (x + p->x*env->scale)) <= ENVPOINT_TOUCH_RADIUS &&
                abs(mouse_y - (y + env->hgt - p->y_norm*(float)env->hgt)) <= ENVPOINT_TOUCH_RADIUS)
            {
                if(p != active_pnt)
                {
                    repaint();
                }

                env_action = ENVPOINTING;
                active_pnt = p;

                if(p == env->p_first)
                    v_only = true;
                else
                    v_only = false;
                break;
            }
            else if(p == env->susPoint && abs(mouse_x - (x + p->x*env->scale)) <= 3)
            {
                if(old_action != ENVSUSTAINING)
                {
                    repaint();
                }
                env_action = ENVSUSTAINING;
            }
            p = p->next;
        }

        if(env_action != old_action)
        {
            repaint();
        }
    }
}

void EnvelopeComponent::mouseDown(const MouseEvent &e)
{
    if(env != NULL)
    {
        jassert(env_action >= ENVUSUAL && env_action <= ENVLINEMOVE);
        jassert(active_pnt == NULL || active_pnt->y_norm < 2);

        int mouse_x = e.getMouseDownX();
        int mouse_y = e.getMouseDownY();
        int px = mouse_x - x;
        int py = y + env->hgt - mouse_y;
        if(px >= 0)
        {
            if(e.mods.isLeftButtonDown() == true)
            {
                float yval = (float)py/env->hgt;

                if(e.mods.isCtrlDown())
                {
                    env_action = ENVDRAWMOVE;
                    env_xstart = (float)px/env->scale;
                }
                else if(env_action != ENVPOINTING && env_action != ENVSUSTAINING)
                {
                    active_pnt = env->AddPoint((float)px/env->scale, yval);
                    env_action = ENVPOINTING;
                    v_only = false;
                }
                env->CalcTime();
            }
            else if(e.mods.isRightButtonDown() == true)
            {
                if(!(e.mods.isCtrlDown()))
                {
                    if(env_action == ENVPOINTING)
                    {
                        if(active_pnt != env->p_first)
                        {
                            env->DeletePoint(active_pnt);
                            active_pnt = NULL;
                        }
                    }
                    env_action = ENVDELETING;
                    env_xstart = (float)px/env->scale;
                }
                else
                {
                    if(env_action == ENVPOINTING)
                    {
                    }
                    else
                    {
                        active_pnt = env->AddPoint((float)px/env->scale, (float)py/env->hgt);
                    }
                    env_action = ENVLINEMOVE;
                    env_xstart = (float)px/env->scale;
                }
            }

            repaint();
        }
    }
}

void EnvelopeComponent::paint(Graphics& g)
{
    //g.fillAll(Colour(33, 86, 75));
    int w = getWidth() - 4;
    int h = getHeight() - 6;

    g.setColour(Colour(28, 49, 62));
    g.fillRect(x, y, w, h + 1);
    g.setColour(Colour(74, 94, 104));
    g.drawRect(x, y, w, h + 1);

    if(env != NULL)
    {
        EnvPnt* p;
        int yb = y;
        env->x = x;
        env->y = yb;
        env->hgt = h;

        /*
        g.setColour(Colour(0x7f50B4A5));
        g.drawRect(x, y + 1, getWidth(), getHeight() - 7);
        g.setColour(Colour(0x3f50B4A5));
        g.drawRect(x, y + 2, getWidth(), getHeight() - 9);
        g.setColour(Colour(0x1f50B4A5));
        g.drawRect(x, y + 3, getWidth(), getHeight() - 11);
        g.setColour(Colour(0x0f50B4A5));
        g.drawRect(x, y + 4, getWidth(), getHeight() - 13);*/
        //J_HLine(g, y, x, x + getWidth());
        //J_HLine(g, y + getHeight() - 5, x, x + getWidth());

        float scale = getWidth()/env->len;
        int iscale = RoundFloat(scale);
        env->scale = scale;
        env->edx = RoundFloat(env->len*scale) - 5;

        if(drawscale)
        {
            if(env->timebased == true)
            {
                g.setFont(*ti);
                int tx = x + 1;
                int v = 0;
                while(tx < x + env->edx)
                {
                    if(drawnumbers)
                    {
                        g.setColour(Colour(0xafFFFFFF));
                        String fmtstr = String::formatted(T("%d"), v);
                        g.drawSingleLineText(fmtstr, tx + 2, yb + env->hgt - 1);
                    }
                    g.setColour(Colour(0xffFFAF5F));
                    J_VLine(g, tx, yb + env->hgt - 3, yb + env->hgt);
                    if(scale > 55)
                    {
                        for(int ic = 0; ic < 10; ic++)
                        {
                            g.setColour(Colour(0x8fFFFFFF));
                            J_VLine(g, tx + RoundFloat(scale/10*ic), yb + env->hgt - 1, yb + env->hgt);
                        }
                    }
                    tx += iscale;
                    v++;
                }
            }
        }

        //if(0)
        {
            g.setColour(Colour((uint8)165, (uint8)165, (uint8)165, (uint8)80));
            Path path;
            p = env->p_first;
            if(p != NULL)
			{
                path.startNewSubPath((float)(x + p->x*scale), (float)(yb + env->hgt));
                path.lineTo((float)(x + p->x*scale), (float)(yb + env->hgt - p->y_norm*(float)env->hgt));
			}
            while(p != NULL)
            {
                if(p->deleted == false)
                {
                    if(p->x <= env->len && (p->next == NULL || p->next->x <= env->len))
                    {
                        if(p->next != NULL)
                        {
                            path.lineTo((float)(x + p->next->x*scale), (float)(yb + env->hgt - p->next->y_norm*(float)env->hgt));
                        }
                        else
                        {
                            path.lineTo((float)(x + env->edx + 1), (float)(yb + env->hgt - p->y_norm*(float)env->hgt));
                        }
                    }
                    else
                    {
                        float px1 = p->x;
                        float py1 = p->y_norm;

                        float px2;
                        float py2;

                        if(p->next != NULL)
                        {
                            px2 = p->next->x;
                            py2 = p->next->y_norm;
                        }
                        else
                        {
                            px2 = p->x;
                            py2 = p->y_norm;
                        }

                        float py3 = Interpolate_Line(px1, py1, px2, py2, env->len);
                        path.lineTo((float)(x + env->edx + 1), (float)(yb + env->hgt - py3*(float)env->hgt));
                        break;
                    }
                }
                p = p->next;
            }
            path.lineTo((float)(x + env->edx + 1), (float)(yb + env->hgt));
            path.closeSubPath();
            g.fillPath(path);
        }

    /*
        g.setColour(Colour(0x5fFFFFFF));
        J_HLine(g, yb, x, x + env->edx);
        J_HLine(g, yb + env->hgt, x, x + env->edx);
        g.setColour(Colour(0x1fFFFFFF));
        J_HLine(g, yb + 1, x, x + env->edx);
        //J_Line(g, x, yb + env->hgt - 1, x + env->edx, yb + env->hgt - 1);
        g.setColour(Colour(0x0fFFFFFF));
        J_HLine(g, yb + 2, x, x + env->edx);
        //J_Line(g, x, yb + env->hgt - 2, x + env->edx, yb + env->hgt - 2);
        g.setColour(Colour(0x0aFFFFFF));
        J_HLine(g, yb + 3, x, x + env->edx);
        //J_Line(g, x, yb + env->hgt - 3, x + env->edx, yb + env->hgt - 3);
        g.setColour(Colour(0x05FFFFFF));
        J_HLine(g, yb + 4, x, x + env->edx);
        //J_Line(g, x, yb + env->hgt - 4, x + env->edx, yb + env->hgt - 4);
    */
        p = env->p_first;
        while(p != NULL)
        {
            if(p->x <= env->len)
            {
                if(env->sustainable && p == env->susPoint)
                {
                    g.setColour(Colour(0xffFFFF00));
                    J_VLine(g, int(x + p->x*scale) + 1, yb + 1, yb + env->hgt);
                    //J_FillRect(g, int(x + p->x*scale - 1) + 1, yb + 1, int(x + p->x*scale + 1) + 1, yb + 2);
                    //J_FillRect(g, int(x + p->x*scale - 2) + 1, yb + 1, int(x + p->x*scale + 2) + 1, yb + 1);

                    if(env_action == ENVSUSTAINING)
                    {
                        g.setColour(Colour(0x9fFFAF00));
                        J_VLine(g, int(x + p->x*scale), yb + 1, yb + env->hgt);
                        J_VLine(g, int(x + p->x*scale) + 2, yb + 1, yb + env->hgt);
                        //g.setColour(Colour(0x4fFFAF00));
                        //J_VLine(g, int(x + p->x*scale) - 1, yb + 1, yb + env->hgt);
                        //J_VLine(g, int(x + p->x*scale) + 3, yb + 1, yb + env->hgt);
                    }
                }

                g.setColour(Colour(0xdfFFFFFF));
                if(p != env->p_first)
                {
                    J_Line(g, x + p->x*scale, yb + env->hgt - p->y_norm*(float)env->hgt, 
                             x + p->prev->x*scale, yb + env->hgt - p->prev->y_norm*(float)env->hgt);
                }

                g.setColour(Colour(0xffFFFFFF));
                //J_Round(g, (int)(x + p->x*scale), (int)(yb  + env->hgt - p->y_norm*(float)env->hgt), 3);
                int xr = RoundFloat(x + p->x*scale);
                int yr = RoundFloat(yb  + env->hgt - p->y_norm*(float)env->hgt);
                J_VLine(g, xr, yr - 2, yr + 3);
                J_HLine(g, yr, xr - 2, xr + 3);
                J_HLine(g, yr - 1, xr - 1, xr + 2);
                J_HLine(g, yr + 1, xr - 1, xr + 2);

                if(env_action == ENVPOINTING && active_pnt == p)
                {
                    g.setColour(Colour(0xffFFAF5F));
                    J_Line(g, xr - 3, yr, xr, yr - 3);
                    J_Line(g, xr, yr - 3, xr + 3, yr);
                    J_Line(g, xr + 3, yr, xr, yr + 3);
                    J_Line(g, xr, yr + 3, xr - 3, yr);
                    g.setPixel(xr + 3, yr);
                    //J_Round(g, (int)(x + p->x*scale), (int)(yb + env->hgt -  p->y_norm*(float)env->hgt), 7);
                }
            }
            else // invisible point. draw last cut line and break
            {
                float px1 = p->prev->x;
                float px2 = p->x;

                float py1 = p->prev->y_norm;
                float py2 = p->y_norm;

                float py3 = Interpolate_Line(px1, py1, px2, py2, env->len);

                g.setColour(Colour(0xffFFFFFF));
                J_Line(g, (float)(x + px1*scale), (float)(yb + env->hgt - py1*(float)env->hgt),
                          (float)(x + env->edx), (float)(yb + env->hgt - py3*(float)env->hgt));
                break;
            }
            p = p->next;
        }
    }
}

AToggleButton* CComponent::PlaceToggleWithLabel(char * txt, BoolParam * bparam, int tgtype, int group, int x, int y, int w, int h)
{
    AToggleButton* tgbutt;
    addAndMakeVisible(tgbutt = new AToggleButton(txt));
    tgbutt->setType(tgtype);
    tgbutt->setRadioGroupId(group);
    tgbutt->setBounds(x, y, w, h);
    tgbutt->addButtonListener(this);
    if(bparam != NULL)
    {
        bparam->atoggle = tgbutt;
        tgbutt->setToggleState(bparam->outval, false);
    }
    return tgbutt;
}

ASlider* CComponent::PlaceSliderWithLabel(char* txt, Parameter* param, int x, int y, int w, int h)
{
    ASlider* slider;
    addAndMakeVisible(slider = new ASlider(txt));
    slider->setRange(0, 1);
    slider->setSliderStyle(ASlider::LinearHorizontal);
    slider->setTextBoxStyle(ASlider::TextBoxPositioned, true, 150, 20);
    slider->setTextBoxIsEditable(false);
    slider->setPopupMenuEnabled(true);

    String str1(txt);
    //slider->setTextColour(Colour(135, 235, 135));
    slider->setTextColour(Colour(160, 185, 195));
    slider->setTextBoxXY(ti->getStringWidth(str1) + 2, -2);
    slider->addListener(this);
    slider->setBounds(x, y, w, h);

    if(param != NULL)
    {
        param->aslider = slider;
        if(param->interval <= 0)
        {
            slider->setRange(param->offset, param->range + param->offset);
        }
        else
        {
            slider->setRange(param->offset, param->range + param->offset, param->interval);
        }
        slider->setValue(param->val, false, false);

        slider->setParamIndex(param->index);
        slider->setParameter(param);
        if(param->vstring == NULL)
        {
            slider->setText("", false);
        }
        else
        {
            slider->setText(param->vstring->outstr, false);
        }
    }

    ALabel* label;
    addAndMakeVisible(label = new ALabel(txt, str1));
    label->setInterceptsMouseClicks(false, false);
    label->setFont(*ti);
    label->setColour(ALabel::textColourId, Colour(140, 155, 162));
    label->setBounds(x, y - 5, ti->getStringWidth(txt) + 10, 17);

    return slider;
}

ALabel* CComponent::PlaceSmallLabel(char * txt, int x, int y, Colour& clr)
{
    String str1(txt);
    ALabel* label;
    addAndMakeVisible(label = new ALabel(str1, str1));
    label->setInterceptsMouseClicks(false, false);
    label->setFont(*ti);
    label->setColour(ALabel::textColourId, clr);
    label->setBounds(x, y, ti->getStringWidth(str1) + 10, 17);

    return label;
}

ALabel* CComponent::PlaceBigLabel(char * txt, int x, int y, Colour& clr)
{
    String str1(txt);
    ALabel* label;
    addAndMakeVisible(label = new ALabel(str1, str1));
    label->setInterceptsMouseClicks(false, false);
    label->setFont(*ins);
    label->setColour(ALabel::textColourId, clr);
    label->setBounds(x, y, ins->getStringWidth(str1) + 10, 17);

    return label;
}

ALabel* CComponent::PlaceTxtLabel(char * txt, int x, int y, Colour& clr)
{
    String str1(txt);
    ALabel* label;
    addAndMakeVisible(label = new ALabel(str1, str1));
    label->setInterceptsMouseClicks(false, false);
    label->setFont(*prj);
    label->setColour(ALabel::textColourId, clr);
    label->setBounds(x, y, prj->getStringWidth(str1) + 10, 17);

    return label;
}

ALabel* CComponent::PlaceTxtLabel1(char * txt, int x, int y, Colour& clr)
{
    String str1(txt);
    ALabel* label;
    addAndMakeVisible(label = new ALabel(str1, str1));
    label->setInterceptsMouseClicks(false, false);
    label->setFont(*rox);
    label->setColour(ALabel::textColourId, clr);
    label->setBounds(x, y, rox->getStringWidth(str1) + 10, 17);

    return label;
}

SynthComponent::SynthComponent(Synth* syn)     
{
    synth = syn;
    setSize(823, 365);

    int ys = 3;

    PlaceOsc1(0, ys);
    PlaceOsc2(135, ys);
    PlaceOsc3(270, ys);

    PlaceFlt1(214, ys + 177);
    PlaceFlt2(214, ys + 244);

    PlaceChorus(477, ys + 283);
    PlaceDelay(477 + 115, ys + 283);
    PlaceReverb(477 + 115 + 115, ys + 283);

    //noise_level = PlaceSliderWithLabel("Noise level", NULL, 270, 5);

    PlaceLFOVol1(0, ys + 98);
    PlaceLFOVol2(0, ys + 98 + 25);
    PlaceLFOVol3(0, ys + 98 + 25 + 25);
    PlaceLFOPitch1(0, ys + 102 + 25 + 25 + 25);
    PlaceLFOPitch2(0, ys + 102 + 25 + 25 + 25 + 25);
    PlaceLFOPitch3(0, ys + 102 + 25 + 25 + 25 + 25 + 25);
    PlaceLFOFlt1Freq(0, ys + 106 + 25 + 25 + 25 + 25 + 25 + 25);
    PlaceLFOFlt2Freq(0, ys + 106 + 25 + 25 + 25 + 25 + 25 + 25 + 25);

    PlaceFMRM(214, ys + 98);
    //PlaceStuff(214, 78 + 78 + 1);

    AGroupComponent* envgroup = new AGroupComponent(T("Env. group"), T(""));
    addAndMakeVisible(envgroup);
    envgroup->setBounds(407, ys, 415, 285);

    addAndMakeVisible(env1 = new EnvelopeComponent(synth->env1));
    env1->drawscale = true;
    env1->setBounds(435, ys + 7, 383, 55);
    PlaceEnv1Butt(408, ys + 6);
    addAndMakeVisible(env2 = new EnvelopeComponent(synth->env2));
    env2->drawscale = true;
    env2->setBounds(435, ys + 7 + 55, 383, 55);
    PlaceEnv2Butt(408, ys + 6 + 55);
    addAndMakeVisible(env3 = new EnvelopeComponent(synth->env3));
    env3->drawscale = true;
    env3->setBounds(435, ys + 7 + 55 + 55, 383, 55);
    PlaceEnv3Butt(408, ys + 6 + 55 + 55);
    addAndMakeVisible(env4 = new EnvelopeComponent(synth->env4));
    env4->drawscale = true;
    env4->setBounds(435, ys + 7 + 55 + 55 + 55, 383, 55);
    PlaceEnv4Butt(408, ys + 6 + 55 + 55 + 55);
    addAndMakeVisible(env5 = new EnvelopeComponent(synth->env5));
    env5->drawscale = true;
    env5->setBounds(435, ys + 7 + 55 + 55 + 55 + 55, 383, 55);
    PlaceEnv5Butt(408, ys + 6 + 55 + 55 + 55 + 55);

    addAndMakeVisible(bt_reset = new ATextButton(String::empty));
    bt_reset->setButtonText(T("Reset"));
    bt_reset->addButtonListener(this);
    bt_reset->setBounds(5, ys + 335, 45, 22);

    envscale = CComponent::PlaceSliderWithLabel("Scale:", NULL, 409, ys + 285, 68);
    envscale->setRange(3, 10);
    envscale->setValue(3, false, false);
    envscale->setText("3.00", false);

    //AGroupComponent* ggroup = new AGroupComponent(T("Env. group"), T(""));
    //addAndMakeVisible(envgroup);
    //envgroup->setBounds(0, 310, 477, 51);
}

SynthComponent::~SynthComponent()
{
    deleteAllChildren();
}

void SynthComponent::paint(Graphics &g)
{
    g.setColour(Colour(45, 54, 60));
    int ys = 3;
    int yy = ys + 312;
    int h = 1;
    while(h < 9)
    {
        if(h < 8)
            g.fillRect(3, yy, 474, h);
        else
        {
            g.fillRect(3, yy, 474, h + 4);
            break;
        }
        yy += h + 1;
        h ++;
    }
}

ASlider* SynthComponent::PlaceSliderWithLabel(char* txt, Parameter* param, int x, int y, int w, int h)
{
    synth->ParamUpdate(param); // Force ValueStrings update to feed the slider's textbox
    ASlider* asl = CComponent::PlaceSliderWithLabel(txt, param, x, y, w, h);
    return asl;
}

void SynthComponent::PlaceFMRM(int x, int y)
{
    AGroupComponent* fm_group = new AGroupComponent(T("FM and RM group"), T("FM and RM"));
    addAndMakeVisible(fm_group);
    fm_group->setBounds(x, y, 193, 81);

    fm_osc12 = PlaceSliderWithLabel("FM 1>>2:", synth->osc1FM2Level, x + 7, y + 9);
    fm_osc23 = PlaceSliderWithLabel("FM 2>>3:", synth->osc2FM3Level, x + 7, y + 31);
    fm_osc13 = PlaceSliderWithLabel("FM 1>>3:", synth->osc1FM3Level, x + 7, y + 53);

    rm_osc12 = PlaceSliderWithLabel("RM 1>>2:", synth->osc1RM2Level, x + 95, y + 9);
    rm_osc23 = PlaceSliderWithLabel("RM 2>>3:", synth->osc2RM3Level, x + 95, y + 31);
    rm_osc13 = PlaceSliderWithLabel("RM 1>>3:", synth->osc1RM3Level, x + 95, y + 53);
}

void SynthComponent::PlaceStuff(int x, int y)
{
    AGroupComponent* stuff_group = new AGroupComponent(T("FM group"), T("Tetris"));
    addAndMakeVisible(stuff_group);
    stuff_group->setBounds(x, y, 193, 135);
}

void SynthComponent::PlaceDelay(int x, int y)
{
    AGroupComponent* delay_group = new AGroupComponent(T("Delay"), T("Echo"));
    addAndMakeVisible(delay_group);
    delay_group->setBounds(x, y, 115, 78);

    delay_on = PlaceToggleWithLabel("", synth->delayON, 0, 0, x + 88, y + 13, 30, 22);

    delay_level = PlaceSliderWithLabel("Level", synth->delay->ggain, x + 7, y + 6);
    delay_delay = PlaceSliderWithLabel("Delay", synth->delay->delay, x + 7, y + 28);
    delay_feedback = PlaceSliderWithLabel("Feedback", synth->delay->feedback, x + 7, y + 50);
    //delay_lowcut = PlaceSliderWithLabel("LowCut", synth->osc1FM3Level, x + 7, y + 72);
}

void SynthComponent::PlaceChorus(int x, int y)
{
    AGroupComponent* chorus_group = new AGroupComponent(T("Chorus"), T("Chorus"));
    addAndMakeVisible(chorus_group);
    chorus_group->setBounds(x, y, 115, 78);

    chorus_on = PlaceToggleWithLabel("", synth->chorusON, 0, 0, x + 88, y + 13, 30, 22);

    chorus_depth = PlaceSliderWithLabel("Depth", synth->chorus->depth, x + 7, y + 6);
    chorus_delay = PlaceSliderWithLabel("Delay", synth->chorus->delay, x + 7, y + 28);
    chorus_drywet = PlaceSliderWithLabel("DryWet", synth->chorus->drywet, x + 7, y + 50);
}

void SynthComponent::PlaceReverb(int x, int y)
{
    AGroupComponent* rev_group = new AGroupComponent(T("Reverb"), T("Reverb"));
    addAndMakeVisible(rev_group);
    rev_group->setBounds(x, y, 115, 78);

    reverb_on = PlaceToggleWithLabel("", synth->reverbON, 0, 0, x + 88, y + 13, 30, 22);

    reverb_decay = PlaceSliderWithLabel("Decay", synth->reverb->decay, x + 7, y + 6);
    reverb_lowcut = PlaceSliderWithLabel("LowCut", synth->reverb->lowCut, x + 7, y + 28);
    reverb_drywet = PlaceSliderWithLabel("DryWet", synth->reverb->drywet, x + 7, y + 50);
}

void SynthComponent::PlaceOsc1(int x, int y)
{
    osc1_group = new AGroupComponent(T("Osc1 group"), T("Osc1"));
    addAndMakeVisible(osc1_group);
    osc1_group->setBounds(x, y, 137, 100);

    osc1_level = PlaceSliderWithLabel("Level:", synth->osc1Level, x + 5, y + 6);
    osc1_oct = PlaceSliderWithLabel("Octave:", synth->osc1Octave, x + 5, y + 28);
    osc1_oct->setSigned(true);
    osc1_detune = PlaceSliderWithLabel("Detune:", synth->osc1Detune, x + 5, y + 50);
    osc1_detune->setSigned(true);
    osc1_width = PlaceSliderWithLabel("Pulse width:", synth->osc1Width, x + 5, y + 72);

    osc1_saw = PlaceToggleWithLabel("Saw", synth->osc1Saw, 22, 1, x + 85, y + 12, 50, 22);
    osc1_sine = PlaceToggleWithLabel("Sine", synth->osc1Sine, 21, 1, x + 85, y + 12 + 12, 50, 22);
    osc1_tri = PlaceToggleWithLabel("Tri", synth->osc1Tri, 23, 1, x + 85, y + 12 + 12 + 12, 50, 22);
    osc1_pulse = PlaceToggleWithLabel("Pulse", synth->osc1Pulse, 26, 1, x + 85, y + 12 + 12 + 12 + 12, 50, 22);
    osc1_noise = PlaceToggleWithLabel("Noise", synth->osc1Noise, 27, 1, x + 85, y + 12 + 12 + 12 + 12 + 12, 50, 22);
    osc1_fat = PlaceToggleWithLabel("3x", synth->osc1Fat, 0, 0, x + 89, y + 12 + 12 + 12 + 12 + 12 + 19, 50, 22);
}

void SynthComponent::PlaceOsc2(int x, int y)
{
    osc2_group = new AGroupComponent(T("Osc2 group"), T("Osc2"));
    addAndMakeVisible(osc2_group);
    osc2_group->setBounds(x, y, 137, 100);

    osc2_level = PlaceSliderWithLabel("Level:", synth->osc2Level, x + 5, y + 6);
    osc2_oct = PlaceSliderWithLabel("Octave:", synth->osc2Octave, x + 5, y + 28);
    osc2_oct->setSigned(true);
    osc2_detune = PlaceSliderWithLabel("Detune:", synth->osc2Detune, x + 5, y + 50);
    osc2_detune->setSigned(true);
    osc2_width = PlaceSliderWithLabel("Pulse width:", synth->osc2Width, x + 5, y + 72);

    osc2_saw = PlaceToggleWithLabel("Saw", synth->osc2Saw, 22, 2, x + 85, y + 12, 50, 22);
    osc2_sine = PlaceToggleWithLabel("Sine", synth->osc2Sine, 21, 2, x + 85, y + 12 + 12, 50, 22);
    osc2_tri = PlaceToggleWithLabel("Tri", synth->osc2Tri, 23, 2, x + 85, y + 12 + 12 + 12, 50, 22);
    osc2_pulse = PlaceToggleWithLabel("Pulse", synth->osc2Pulse, 26, 2, x + 85, y + 12 + 12 + 12 + 12, 50, 22);
    osc2_noise = PlaceToggleWithLabel("Noise", synth->osc2Noise, 27, 2, x + 85, y + 12 + 12 + 12 + 12 + 12, 50, 22);
    osc2_fat = PlaceToggleWithLabel("3x", synth->osc2Fat, 0, 0, x + 89, y + 12 + 12 + 12 + 12 + 12 + 19, 50, 22);
}

void SynthComponent::PlaceOsc3(int x, int y)
{
    osc3_group = new AGroupComponent(T("Osc3 group"), T("Osc3"));
    addAndMakeVisible(osc3_group);
    osc3_group->setBounds(x, y, 137, 100);

    osc3_level = PlaceSliderWithLabel("Level:", synth->osc3Level, x + 5, y + 6);
    osc3_oct = PlaceSliderWithLabel("Octave:", synth->osc3Octave, x + 5, y + 28);
    osc3_oct->setSigned(true);
    osc3_detune = PlaceSliderWithLabel("Detune:", synth->osc3Detune, x + 5, y + 50);
    osc3_detune->setSigned(true);
    osc3_width = PlaceSliderWithLabel("Pulse width:", synth->osc3Width, x + 5, y + 72);

    osc3_saw = PlaceToggleWithLabel("Saw", synth->osc3Saw, 22, 5, x + 85, y + 12, 50, 22);
    osc3_sine = PlaceToggleWithLabel("Sine", synth->osc3Sine, 21, 5, x + 85, y + 12 + 12, 50, 22);
    osc3_tri = PlaceToggleWithLabel("Tri", synth->osc3Tri, 23, 5, x + 85, y + 12 + 12 + 12, 50, 22);
    osc3_pulse = PlaceToggleWithLabel("Pulse", synth->osc3Pulse, 26, 5, x + 85, y + 12 + 12 + 12 + 12, 50, 22);
    osc3_noise = PlaceToggleWithLabel("Noise", synth->osc3Noise, 27, 5, x + 85, y + 12 + 12 + 12 + 12 + 12, 50, 22);
    osc3_fat = PlaceToggleWithLabel("3x", synth->osc3Fat, 0, 0, x + 89, y + 12 + 12 + 12 + 12 + 12 + 19, 50, 22);
}

void SynthComponent::UpdateEnvButts()
{
    if(synth->p_osc1envVal == &synth->envV1)
        envVol1_1->setToggleState(true, false);
    else if(synth->p_osc1envVal == &synth->envV2)
        envVol1_2->setToggleState(true, false);
    else if(synth->p_osc1envVal == &synth->envV3)
        envVol1_3->setToggleState(true, false);
    else if(synth->p_osc1envVal == &synth->envV4)
        envVol1_4->setToggleState(true, false);
    else if(synth->p_osc1envVal == &synth->envV5)
        envVol1_5->setToggleState(true, false);

    if(synth->p_osc2envVal == &synth->envV1)
        envVol2_1->setToggleState(true, false);
    else if(synth->p_osc2envVal == &synth->envV2)
        envVol2_2->setToggleState(true, false);
    else if(synth->p_osc2envVal == &synth->envV3)
        envVol2_3->setToggleState(true, false);
    else if(synth->p_osc2envVal == &synth->envV4)
        envVol2_4->setToggleState(true, false);
    else if(synth->p_osc2envVal == &synth->envV5)
        envVol2_5->setToggleState(true, false);

    if(synth->p_osc3envVal == &synth->envV1)
        envVol3_1->setToggleState(true, false);
    else if(synth->p_osc3envVal == &synth->envV2)
        envVol3_2->setToggleState(true, false);
    else if(synth->p_osc3envVal == &synth->envV3)
        envVol3_3->setToggleState(true, false);
    else if(synth->p_osc3envVal == &synth->envV4)
        envVol3_4->setToggleState(true, false);
    else if(synth->p_osc3envVal == &synth->envV5)
        envVol3_5->setToggleState(true, false);

    if(synth->p_flt1envVal == synth->ebuff1)
        envflt1_1->setToggleState(true, false);
    else if(synth->p_flt1envVal == synth->ebuff2)
        envflt1_2->setToggleState(true, false);
    else if(synth->p_flt1envVal == synth->ebuff3)
        envflt1_3->setToggleState(true, false);
    else if(synth->p_flt1envVal == synth->ebuff4)
        envflt1_4->setToggleState(true, false);
    else if(synth->p_flt1envVal == synth->ebuff5)
        envflt1_5->setToggleState(true, false);

    if(synth->p_flt2envVal == synth->ebuff1)
        envflt2_1->setToggleState(true, false);
    else if(synth->p_flt2envVal == synth->ebuff2)
        envflt2_2->setToggleState(true, false);
    else if(synth->p_flt2envVal == synth->ebuff3)
        envflt2_3->setToggleState(true, false);
    else if(synth->p_flt2envVal == synth->ebuff4)
        envflt2_4->setToggleState(true, false);
    else if(synth->p_flt2envVal == synth->ebuff5)
        envflt2_5->setToggleState(true, false);
}

void SynthComponent::PlaceEnv1Butt(int x, int y)
{
    envVol1_1 = PlaceToggleWithLabel("O1", NULL, 3, 11, x, y + 6, 28, 22);
    envVol1_1->setToggleState(true, false);
    envVol2_1 = PlaceToggleWithLabel("O2", NULL, 3, 12, x, y + 6 + 9, 28, 22);
    envVol2_1->setToggleState(false, false);
    envVol3_1 = PlaceToggleWithLabel("O3", NULL, 3, 13, x, y + 6 + 9 + 9, 28, 22);
    envVol3_1->setToggleState(false, false);
    envflt1_1 = PlaceToggleWithLabel("F1", NULL, 3, 14, x, y + 6 + 9 + 9 + 9, 28, 22);
    envflt1_1->setToggleState(false, false);
    envflt2_1 = PlaceToggleWithLabel("F2", NULL, 3, 15, x, y + 6 + 9 + 9 + 9 + 9, 28, 22);
    envflt2_1->setToggleState(false, false);
}

void SynthComponent::PlaceEnv2Butt(int x, int y)
{
    envVol1_2 = PlaceToggleWithLabel("O1", NULL, 3, 11, x, y + 6, 28, 22);
    envVol1_2->setToggleState(false, false);
    envVol2_2 = PlaceToggleWithLabel("O2", NULL, 3, 12, x, y + 6 + 9, 28, 22);
    envVol2_2->setToggleState(true, false);
    envVol3_2 = PlaceToggleWithLabel("O3", NULL, 3, 13, x, y + 6 + 9 + 9, 28, 22);
    envVol3_2->setToggleState(false, false);
    envflt1_2 = PlaceToggleWithLabel("F1", NULL, 3, 14, x, y + 6 + 9 + 9 + 9, 28, 22);
    envflt1_2->setToggleState(false, false);
    envflt2_2 = PlaceToggleWithLabel("F2", NULL, 3, 15, x, y + 6 + 9 + 9 + 9 + 9, 28, 22);
    envflt2_2->setToggleState(false, false);
}

void SynthComponent::PlaceEnv3Butt(int x, int y)
{
    envVol1_3 = PlaceToggleWithLabel("O1", NULL, 3, 11, x, y + 6, 28, 22);
    envVol1_3->setToggleState(false, false);
    envVol2_3 = PlaceToggleWithLabel("O2", NULL, 3, 12, x, y + 6 + 9, 28, 22);
    envVol2_3->setToggleState(false, false);
    envVol3_3 = PlaceToggleWithLabel("O3", NULL, 3, 13, x, y + 6 + 9 + 9, 28, 22);
    envVol3_3->setToggleState(true, false);
    envflt1_3 = PlaceToggleWithLabel("F1", NULL, 3, 14, x, y + 6 + 9 + 9 + 9, 28, 22);
    envflt1_3->setToggleState(false, false);
    envflt2_3 = PlaceToggleWithLabel("F2", NULL, 3, 15, x, y + 6 + 9 + 9 + 9 + 9, 28, 22);
    envflt2_3->setToggleState(false, false);
}

void SynthComponent::PlaceEnv4Butt(int x, int y)
{
    envVol1_4 = PlaceToggleWithLabel("O1", NULL, 3, 11, x, y + 6, 28, 22);
    envVol1_4->setToggleState(false, false);
    envVol2_4 = PlaceToggleWithLabel("O2", NULL, 3, 12, x, y + 6 + 9, 28, 22);
    envVol2_4->setToggleState(false, false);
    envVol3_4 = PlaceToggleWithLabel("O3", NULL, 3, 13, x, y + 6 + 9 + 9, 28, 22);
    envVol3_4->setToggleState(false, false);
    envflt1_4 = PlaceToggleWithLabel("F1", NULL, 3, 14, x, y + 6 + 9 + 9 + 9, 28, 22);
    envflt1_4->setToggleState(true, false);
    envflt2_4 = PlaceToggleWithLabel("F2", NULL, 3, 15, x, y + 6 + 9 + 9 + 9 + 9, 28, 22);
    envflt2_4->setToggleState(false, false);
}

void SynthComponent::PlaceEnv5Butt(int x, int y)
{
    envVol1_5 = PlaceToggleWithLabel("O1", NULL, 3, 11, x, y + 6, 28, 22);
    envVol1_5->setToggleState(false, false);
    envVol2_5 = PlaceToggleWithLabel("O2", NULL, 3, 12, x, y + 6 + 9, 28, 22);
    envVol2_5->setToggleState(false, false);
    envVol3_5 = PlaceToggleWithLabel("O3", NULL, 3, 13, x, y + 6 + 9 + 9, 28, 22);
    envVol3_5->setToggleState(false, false);
    envflt1_5 = PlaceToggleWithLabel("F1", NULL, 3, 14, x, y + 6 + 9 + 9 + 9, 28, 22);
    envflt1_5->setToggleState(false, false);
    envflt2_5 = PlaceToggleWithLabel("F2", NULL, 3, 15, x, y + 6 + 9 + 9 + 9 + 9, 28, 22);
    envflt2_5->setToggleState(true, false);
}

void SynthComponent::PlaceFlt1(int x, int y)
{
    filt1_group = new AGroupComponent(T("Flt1 group"), T("Filter1"));
    addAndMakeVisible(filt1_group);
    filt1_group->setBounds(x, y, 193, 69);

    filt1_LP = PlaceToggleWithLabel("LP", synth->filt1[0]->f1, 0, 4, x + 152, y + 12, 50, 22);
    filt1_HP = PlaceToggleWithLabel("HP", synth->filt1[0]->f2, 0, 4, x + 152, y + 12 + 12, 50, 22);
    filt1_BP = PlaceToggleWithLabel("BP", synth->filt1[0]->f3, 0, 4, x + 152, y + 12 + 12 + 12, 50, 22);
    filt1_x2 = PlaceToggleWithLabel("x2", synth->filt1[0]->x2, 0, 0, x + 152, y + 12 + 12 + 12 + 15, 50, 22);

    filt1_osc1 = PlaceToggleWithLabel("Osc1", synth->filt1Osc1, 1, 0, x + 2, y + 8, 50, 22);
    filt1_osc2 = PlaceToggleWithLabel("Osc2", synth->filt1Osc2, 1, 0, x + 2, y + 8 + 12, 50, 22);
    filt1_osc3 = PlaceToggleWithLabel("Osc3", synth->filt1Osc3, 1, 0, x + 2, y + 8 + 12 + 12, 50, 22);

    filt1_freq = PlaceSliderWithLabel("Cutoff:", synth->filt1[0]->cutoff, x + 52, y + 6, 95);
    filt1_res = PlaceSliderWithLabel("Res:", synth->filt1[0]->resonance, x + 52, y + 28, 95);
}

void SynthComponent::PlaceFlt2(int x, int y)
{
    filt2_group = new AGroupComponent(T("Flt2 group"), T("Filter2"));
    addAndMakeVisible(filt2_group);
    filt2_group->setBounds(x, y, 193, 68);

    filt2_LP = PlaceToggleWithLabel("LP", synth->filt2[0]->f1, 0, 3, x + 152, y + 12, 50, 22);
    filt2_HP = PlaceToggleWithLabel("HP", synth->filt2[0]->f2, 0, 3, x + 152, y + 12 + 12, 50, 22);
    filt2_BP = PlaceToggleWithLabel("BP", synth->filt2[0]->f3, 0, 3, x + 152, y + 12 + 12 + 12, 50, 22);
    filt2_x2 = PlaceToggleWithLabel("x2", synth->filt2[0]->x2, 0, 0, x + 152, y + 12 + 12 + 12 + 15, 50, 22);

    filt2_osc1 = PlaceToggleWithLabel("Osc1", synth->filt2Osc1, 1, 0, x + 2, y + 8, 50, 22);
    filt2_osc2 = PlaceToggleWithLabel("Osc2", synth->filt2Osc2, 1, 0, x + 2, y + 8 + 12, 50, 22);
    filt2_osc3 = PlaceToggleWithLabel("Osc3", synth->filt2Osc3, 1, 0, x + 2, y + 8 + 12 + 12, 50, 22);

    filt2_freq = PlaceSliderWithLabel("Cutoff:", synth->filt2[0]->cutoff, x + 52, y + 6, 95);
    filt2_res = PlaceSliderWithLabel("Res:", synth->filt2[0]->resonance, x + 52, y + 28, 95);
}

void SynthComponent::PlaceLFOVol1(int x, int y)
{
    AGroupComponent* group = new AGroupComponent(T("LFO Osc1 Vol"), T(""));
    addAndMakeVisible(group);
    group->setBounds(x, y, 214, 31);

    lfo_vol1Amp = PlaceSliderWithLabel("Amp:", synth->LFOAmp_Vol1, x + 49, y + 5, 82);
    lfo_vol1Rate = PlaceSliderWithLabel("Rate:", synth->LFORate_Vol1, x + 132, y + 5, 77);

    PlaceSmallLabel("Osc1 level", x + 2, y - 2, Colour(140, 155, 162));
    PlaceBigLabel("LFO", x + 13, y + 13, Colour(140, 155, 162));
}

void SynthComponent::PlaceLFOVol2(int x, int y)
{
    AGroupComponent* group = new AGroupComponent(T("LFO Osc2 Vol"), T(""));
    addAndMakeVisible(group);
    group->setBounds(x, y, 214, 31);

    lfo_vol2Amp = PlaceSliderWithLabel("Amp:", synth->LFOAmp_Vol2, x + 49, y + 5, 82);
    lfo_vol2Rate = PlaceSliderWithLabel("Rate:", synth->LFORate_Vol2, x + 132, y + 5, 77);

    PlaceSmallLabel("Osc2 level", x + 2, y - 2, Colour(140, 155, 162));
    PlaceBigLabel("LFO", x + 13, y + 13, Colour(140, 155, 162));
}

void SynthComponent::PlaceLFOVol3(int x, int y)
{
    AGroupComponent* group = new AGroupComponent(T("LFO Osc3 Vol"), T(""));
    addAndMakeVisible(group);
    group->setBounds(x, y, 214, 31);

    lfo_vol3Amp = PlaceSliderWithLabel("Amp:", synth->LFOAmp_Vol3, x + 49, y + 5, 82);
    lfo_vol3Rate = PlaceSliderWithLabel("Rate:", synth->LFORate_Vol3, x + 132, y + 5, 77);

    PlaceSmallLabel("Osc3 level", x + 2, y - 2, Colour(140, 155, 162));
    PlaceBigLabel("LFO", x + 13, y + 13, Colour(140, 155, 162));
}

void SynthComponent::PlaceLFOPitch1(int x, int y)
{
    AGroupComponent* group = new AGroupComponent(T("LFO Pitch1 Vol"), T(""));
    addAndMakeVisible(group);
    group->setBounds(x, y, 214, 31);

    lfo_pitch1Amp = PlaceSliderWithLabel("Amp:", synth->LFOAmp_Pitch1, x + 49, y + 5, 82);
    lfo_pitch1Rate = PlaceSliderWithLabel("Rate:", synth->LFORate_Pitch1, x + 132, y + 5, 77);

    PlaceSmallLabel("Osc1 pitch", x + 2, y - 2, Colour(140, 155, 162));
    PlaceBigLabel("LFO", x + 13, y + 13, Colour(140, 155, 162));
}

void SynthComponent::PlaceLFOPitch2(int x, int y)
{
    AGroupComponent* group = new AGroupComponent(T("LFO Pitch2 Vol"), T(""));
    addAndMakeVisible(group);
    group->setBounds(x, y, 214, 31);

    lfo_pitch2Amp = PlaceSliderWithLabel("Amp:", synth->LFOAmp_Pitch2, x + 49, y + 5, 82);
    lfo_pitch2Rate = PlaceSliderWithLabel("Rate:", synth->LFORate_Pitch2, x + 132, y + 5, 77);

    PlaceSmallLabel("Osc2 pitch", x + 2, y - 2, Colour(140, 155, 162));
    PlaceBigLabel("LFO", x + 13, y + 13, Colour(140, 155, 162));
}

void SynthComponent::PlaceLFOPitch3(int x, int y)
{
    AGroupComponent* group = new AGroupComponent(T("LFO Pitch3 Vol"), T(""));
    addAndMakeVisible(group);
    group->setBounds(x, y, 214, 31);

    lfo_pitch3Amp = PlaceSliderWithLabel("Amp:", synth->LFOAmp_Pitch3, x + 49, y + 5, 82);
    lfo_pitch3Rate = PlaceSliderWithLabel("Rate:", synth->LFORate_Pitch3, x + 132, y + 5, 77);

    PlaceSmallLabel("Osc3 pitch", x + 2, y - 2, Colour(140, 155, 162));
    PlaceBigLabel("LFO", x + 13, y + 13, Colour(140, 155, 162));
}

void SynthComponent::PlaceLFOFlt1Freq(int x, int y)
{
    AGroupComponent* group = new AGroupComponent(T("LFO Flt1 Freq"), T(""));
    addAndMakeVisible(group);
    group->setBounds(x, y, 214, 31);

    lfo_flt1freqLevel = PlaceSliderWithLabel("Amp:", synth->LFOAmp_Flt1Freq, x + 49, y + 5, 82);
    lfo_flt1freqRate = PlaceSliderWithLabel("Rate:", synth->LFORate_Flt1Freq, x + 132, y + 5, 77);

    PlaceSmallLabel("Flt1 freq", x + 2, y - 2, Colour(140, 155, 162));
    PlaceBigLabel("LFO", x + 13, y + 13, Colour(140, 155, 162));
}

void SynthComponent::PlaceLFOFlt2Freq(int x, int y)
{
    AGroupComponent* group = new AGroupComponent(T("LFO Flt2 Freq"), T(""));
    addAndMakeVisible(group);
    group->setBounds(x, y, 214, 31);

    lfo_flt2freqLevel = PlaceSliderWithLabel("Amp:", synth->LFOAmp_Flt2Freq, x + 49, y + 5, 82);
    lfo_flt2freqRate = PlaceSliderWithLabel("Rate:", synth->LFORate_Flt2Freq, x + 132, y + 5, 77);

    PlaceSmallLabel("Flt2 freq", x + 2, y - 2, Colour(140, 155, 162));
    PlaceBigLabel("LFO", x + 13, y + 13, Colour(140, 155, 162));
}

void SynthComponent::buttonClicked(Button* buttonThatWasClicked)
{
    if(buttonThatWasClicked == osc1_saw || 
       buttonThatWasClicked == osc1_sine ||
       buttonThatWasClicked == osc1_tri ||
       buttonThatWasClicked == osc1_pulse ||
       buttonThatWasClicked == osc1_noise)
    {
        synth->osc1Saw->SetBoolValue(osc1_saw->getToggleState());
        synth->osc1Sine->SetBoolValue(osc1_sine->getToggleState());
        synth->osc1Tri->SetBoolValue(osc1_tri->getToggleState());
        synth->osc1Pulse->SetBoolValue(osc1_pulse->getToggleState());
        synth->osc1Noise->SetBoolValue(osc1_noise->getToggleState());
    }
    else if(buttonThatWasClicked == osc2_saw || 
            buttonThatWasClicked == osc2_sine ||
            buttonThatWasClicked == osc2_tri ||
            buttonThatWasClicked == osc2_pulse ||
            buttonThatWasClicked == osc2_noise)
    {
        synth->osc2Saw->SetBoolValue(osc2_saw->getToggleState());
        synth->osc2Sine->SetBoolValue(osc2_sine->getToggleState());
        synth->osc2Tri->SetBoolValue(osc2_tri->getToggleState());
        synth->osc2Pulse->SetBoolValue(osc2_pulse->getToggleState());
        synth->osc2Noise->SetBoolValue(osc2_noise->getToggleState());
    }
    else if(buttonThatWasClicked == osc3_saw || 
            buttonThatWasClicked == osc3_sine ||
            buttonThatWasClicked == osc3_tri ||
            buttonThatWasClicked == osc3_pulse ||
            buttonThatWasClicked == osc3_noise)
    {
        synth->osc3Saw->SetBoolValue(osc3_saw->getToggleState());
        synth->osc3Sine->SetBoolValue(osc3_sine->getToggleState());
        synth->osc3Tri->SetBoolValue(osc3_tri->getToggleState());
        synth->osc3Pulse->SetBoolValue(osc3_pulse->getToggleState());
        synth->osc3Noise->SetBoolValue(osc3_noise->getToggleState());
    }
    else if(buttonThatWasClicked == chorus_on)
    {
        synth->chorusON->SetBoolValue(chorus_on->getToggleState());
    }
    else if(buttonThatWasClicked == delay_on)
    {
        synth->delayON->SetBoolValue(delay_on->getToggleState());
    }
    else if(buttonThatWasClicked == reverb_on)
    {
        synth->reverbON->SetBoolValue(reverb_on->getToggleState());
    }
    else if(buttonThatWasClicked == osc1_fat)
    {
        synth->osc1Fat->SetBoolValue(osc1_fat->getToggleState());
    }
    else if(buttonThatWasClicked == osc2_fat)
    {
        synth->osc2Fat->SetBoolValue(osc2_fat->getToggleState());
    }
    else if(buttonThatWasClicked == osc3_fat)
    {
        synth->osc3Fat->SetBoolValue(osc3_fat->getToggleState());
    }
    else if(buttonThatWasClicked == filt1_LP)
    {
        synth->filt1[0]->f1->SetBoolValue(filt1_LP->getToggleState());
    }
    else if(buttonThatWasClicked == filt1_HP)
    {
        synth->filt1[0]->f2->SetBoolValue(filt1_HP->getToggleState());
    }
    else if(buttonThatWasClicked == filt1_BP)
    {
        synth->filt1[0]->f3->SetBoolValue(filt1_BP->getToggleState());
    }
    else if(buttonThatWasClicked == filt1_x2)
    {
        synth->filt1[0]->x2->SetBoolValue(filt1_x2->getToggleState());
    }
    else if(buttonThatWasClicked == filt2_LP)
    {
        synth->filt2[0]->f1->SetBoolValue(filt2_LP->getToggleState());
    }
    else if(buttonThatWasClicked == filt2_HP)
    {
        synth->filt2[0]->f2->SetBoolValue(filt2_HP->getToggleState());
    }
    else if(buttonThatWasClicked == filt2_BP)
    {
        synth->filt2[0]->f3->SetBoolValue(filt2_BP->getToggleState());
    }
    else if(buttonThatWasClicked == filt2_x2)
    {
        synth->filt2[0]->x2->SetBoolValue(filt2_x2->getToggleState());
    }
    else if(buttonThatWasClicked == filt1_osc1)
    {
        synth->filt1Osc1->SetBoolValue(filt1_osc1->getToggleState());
    }
    else if(buttonThatWasClicked == filt1_osc2)
    {
        synth->filt1Osc2->SetBoolValue(filt1_osc2->getToggleState());
    }
    else if(buttonThatWasClicked == filt1_osc3)
    {
        synth->filt1Osc3->SetBoolValue(filt1_osc3->getToggleState());
    }
    else if(buttonThatWasClicked == filt2_osc1)
    {
        synth->filt2Osc1->SetBoolValue(filt2_osc1->getToggleState());
    }
    else if(buttonThatWasClicked == filt2_osc2)
    {
        synth->filt2Osc2->SetBoolValue(filt2_osc2->getToggleState());
    }
    else if(buttonThatWasClicked == filt2_osc3)
    {
        synth->filt2Osc3->SetBoolValue(filt2_osc3->getToggleState());
    }
    else if(buttonThatWasClicked == envVol1_1)
    {
        synth->p_osc1envVal = &synth->envV1;
    }
    else if(buttonThatWasClicked == envVol1_2)
    {
        synth->p_osc1envVal = &synth->envV2;
    }
    else if(buttonThatWasClicked == envVol1_3)
    {
        synth->p_osc1envVal = &synth->envV3;
    }
    else if(buttonThatWasClicked == envVol1_4)
    {
        synth->p_osc1envVal = &synth->envV4;
    }
    else if(buttonThatWasClicked == envVol1_5)
    {
        synth->p_osc1envVal = &synth->envV5;
    }
    else if(buttonThatWasClicked == envVol2_1)
    {
        synth->p_osc2envVal = &synth->envV1;
    }
    else if(buttonThatWasClicked == envVol2_2)
    {
        synth->p_osc2envVal = &synth->envV2;
    }
    else if(buttonThatWasClicked == envVol2_3)
    {
        synth->p_osc2envVal = &synth->envV3;
    }
    else if(buttonThatWasClicked == envVol2_4)
    {
        synth->p_osc2envVal = &synth->envV4;
    }
    else if(buttonThatWasClicked == envVol2_5)
    {
        synth->p_osc2envVal = &synth->envV5;
    }
    else if(buttonThatWasClicked == envVol3_1)
    {
        synth->p_osc3envVal = &synth->envV1;
    }
    else if(buttonThatWasClicked == envVol3_2)
    {
        synth->p_osc3envVal = &synth->envV2;
    }
    else if(buttonThatWasClicked == envVol3_3)
    {
        synth->p_osc3envVal = &synth->envV3;
    }
    else if(buttonThatWasClicked == envVol3_4)
    {
        synth->p_osc3envVal = &synth->envV4;
    }
    else if(buttonThatWasClicked == envVol3_5)
    {
        synth->p_osc3envVal = &synth->envV5;
    }
    else if(buttonThatWasClicked == envflt1_1)
    {
        synth->p_flt1envVal = synth->ebuff1;
    }
    else if(buttonThatWasClicked == envflt1_2)
    {
        synth->p_flt1envVal = synth->ebuff2;
    }
    else if(buttonThatWasClicked == envflt1_3)
    {
        synth->p_flt1envVal = synth->ebuff3;
    }
    else if(buttonThatWasClicked == envflt1_4)
    {
        synth->p_flt1envVal = synth->ebuff4;
    }
    else if(buttonThatWasClicked == envflt1_5)
    {
        synth->p_flt1envVal = synth->ebuff5;
    }
    else if(buttonThatWasClicked == envflt2_1)
    {
        synth->p_flt2envVal = synth->ebuff1;
    }
    else if(buttonThatWasClicked == envflt2_2)
    {
        synth->p_flt2envVal = synth->ebuff2;
    }
    else if(buttonThatWasClicked == envflt2_3)
    {
        synth->p_flt2envVal = synth->ebuff3;
    }
    else if(buttonThatWasClicked == envflt2_4)
    {
        synth->p_flt2envVal = synth->ebuff4;
    }
    else if(buttonThatWasClicked == envflt2_5)
    {
        synth->p_flt2envVal = synth->ebuff5;
    }
    else if(buttonThatWasClicked == bt_reset)
    {
        synth->ResetValues();
    }
}

void SynthComponent::sliderValueChanged(ASlider* slider)
{
    if(slider == envscale)
    {
        env1->env->SetLength((float)slider->getValue());
        env1->repaint();
        env2->env->SetLength((float)slider->getValue());
        env2->repaint();
        env3->env->SetLength((float)slider->getValue());
        env3->repaint();
        env4->env->SetLength((float)slider->getValue());
        env4->repaint();
        env5->env->SetLength((float)slider->getValue());
        env5->repaint();

        envscale->setText(String::formatted(T("%.2f"), envscale->getValue()), false);
    }
    else
    {
        //synth->ParamUpdate(slider->getParameter(), slider);
        slider->getParameter()->SetDirectValueFromControl((float)slider->getValue(), true);
    }
    /*
    if(slider == osc1_level)
    {
        synth->ParamUpdate(synth->osc1Level, slider);
    }
    else if(slider == osc1_oct)
    {
        synth->ParamUpdate(synth->osc1Octave, slider);
    }
    else if(slider == osc1_detune)
    {
        synth->ParamUpdate(synth->osc1Detune, slider);
    }
    else if(slider == osc2_level)
    {
        synth->ParamUpdate(synth->osc2Level, slider);
    }
    else if(slider == osc2_oct)
    {
        synth->ParamUpdate(synth->osc2Octave, slider);
    }
    else if(slider == osc2_detune)
    {
        synth->ParamUpdate(synth->osc2Detune, slider);
    }
    else if(slider == osc3_level)
    {
        synth->ParamUpdate(synth->osc3Level, slider);
    }
    else if(slider == osc3_oct)
    {
        synth->ParamUpdate(synth->osc3Octave, slider);
    }
    else if(slider == osc3_detune)
    {
        synth->ParamUpdate(synth->osc3Detune, slider);
    }
    else if(slider == fm_osc12)
    {
        synth->ParamUpdate(synth->osc1FM2Level, slider);
    }
    else if(slider == fm_osc23)
    {
        synth->ParamUpdate(synth->osc2FM3Level, slider);
    }
    else if(slider == fm_osc13)
    {
        synth->ParamUpdate(synth->osc1FM3Level, slider);
    }
    else if(slider == rm_osc12)
    {
        synth->ParamUpdate(synth->osc1RM2Level, slider);
    }
    else if(slider == rm_osc23)
    {
        synth->ParamUpdate(synth->osc2RM3Level, slider);
    }
    else if(slider == rm_osc13)
    {
        synth->ParamUpdate(synth->osc1RM3Level, slider);
    }
    else if(slider == lfo_vol1Amp)
    {
        synth->ParamUpdate(synth->LFOAmp_Vol1, slider);
    }
    else if(slider == lfo_vol1Rate)
    {
        synth->ParamUpdate(synth->LFORate_Vol1, slider);
    }
    else if(slider == lfo_vol2Amp)
    {
        synth->ParamUpdate(synth->LFOAmp_Vol2, slider);
    }
    else if(slider == lfo_vol2Rate)
    {
        synth->ParamUpdate(synth->LFORate_Vol2, slider);
    }
    else if(slider == lfo_vol3Amp)
    {
        synth->ParamUpdate(synth->LFOAmp_Vol3, slider);
    }
    else if(slider == lfo_vol3Rate)
    {
        synth->ParamUpdate(synth->LFORate_Vol3, slider);
    }
    else if(slider == lfo_pitch1Amp)
    {
        synth->ParamUpdate(synth->LFOAmp_Pitch1, slider);
    }
    else if(slider == lfo_pitch1Rate)
    {
        synth->ParamUpdate(synth->LFORate_Pitch1, slider);
    }
    else if(slider == lfo_pitch2Amp)
    {
        synth->ParamUpdate(synth->LFOAmp_Pitch2, slider);
    }
    else if(slider == lfo_pitch2Rate)
    {
        synth->ParamUpdate(synth->LFORate_Pitch2, slider);
    }
    else if(slider == lfo_pitch3Amp)
    {
        synth->ParamUpdate(synth->LFOAmp_Pitch3, slider);
    }
    else if(slider == lfo_pitch3Rate)
    {
        synth->ParamUpdate(synth->LFORate_Pitch3, slider);
    }
    else if(slider == lfo_flt1freqLevel)
    {
        synth->ParamUpdate(synth->LFOAmp_Flt1Freq, slider);
    }
    else if(slider == lfo_flt1freqRate)
    {
        synth->ParamUpdate(synth->LFORate_Flt1Freq, slider);
    }
    else if(slider == lfo_flt2freqLevel)
    {
        synth->ParamUpdate(synth->LFOAmp_Flt2Freq, slider);
    }
    else if(slider == lfo_flt2freqRate)
    {
        synth->ParamUpdate(synth->LFORate_Flt2Freq, slider);
    }
    else if(slider == filt1_freq)
    {
        synth->ParamUpdate(synth->filt1[0]->cutoff, slider);
    }
    else if(slider == filt2_freq)
    {
        synth->ParamUpdate(synth->filt2[0]->cutoff, slider);
    }
    else if(slider == filt1_res)
    {
        synth->ParamUpdate(synth->filt1[0]->resonance, slider);
    }
    else if(slider == filt2_res)
    {
        synth->ParamUpdate(synth->filt2[0]->resonance, slider);
    }
    else if(slider == osc1_width)
    {
        synth->ParamUpdate(synth->osc1Width, slider);
    }
    else if(slider == osc2_width)
    {
        synth->ParamUpdate(synth->osc2Width, slider);
    }
    else if(slider == osc3_width)
    {
        synth->ParamUpdate(synth->osc3Width, slider);
    }
    else if(slider == envscale)
    {
        env1->env->SetLength((float)slider->getValue());
        env1->repaint();
        env2->env->SetLength((float)slider->getValue());
        env2->repaint();
        env3->env->SetLength((float)slider->getValue());
        env3->repaint();
        env4->env->SetLength((float)slider->getValue());
        env4->repaint();
        env5->env->SetLength((float)slider->getValue());
        env5->repaint();

        envscale->setText(String::formatted(T("%.2f"), envscale->getValue()), false);
    }
    else if(slider == chorus_drywet)
    {
        synth->ParamUpdate(synth->chorus->drywet, slider);
    }
    else if(slider == chorus_delay)
    {
        synth->ParamUpdate(synth->chorus->delay, slider);
    }
    else if(slider == chorus_depth)
    {
        synth->ParamUpdate(synth->chorus->depth, slider);
    }
    else if(slider == reverb_drywet)
    {
        synth->ParamUpdate(synth->reverb->drywet, slider);
    }
    else if(slider == reverb_decay)
    {
        synth->ParamUpdate(synth->reverb->decay, slider);
    }
    else if(slider == reverb_lowcut)
    {
        synth->ParamUpdate(synth->reverb->lowCut, slider);
    }
    else if(slider == delay_drywet)
    {
        synth->ParamUpdate(synth->delay->drywet, slider);
    }
    else if(slider == delay_delay)
    {
        synth->ParamUpdate(synth->delay->delay, slider);
    }
    else if(slider == delay_feedback)
    {
        synth->ParamUpdate(synth->delay->feedback, slider);
    }*/
}

void SynthComponent::resized()
{
}

SynthWindow::SynthWindow(Synth* syn)
{
    setName("Chaotic Synthesizer");
    scope = &syn->scope;
    SynthComponent* const cComponent = new SynthComponent(syn);
    setContentComponent(cComponent, true, true);
    centreWithSize(getWidth(), getHeight());
}

SynthWindow::~SynthWindow()
{
    // (the content component will be deleted automatically, so no need to do it here)
    //delete contentComponent;
}

SampleWindow::SampleWindow() : ChildWindow("Sample settings")
{
    sample = NULL;
    SampleComponent* const contentComponent = new SampleComponent();
    setContentComponent(contentComponent, true, true);
    centreWithSize(getWidth(), getHeight());
}

SampleWindow::~SampleWindow()
{
    // (the content component will be deleted automatically, so no need to do it here)
}

void SampleWindow::closeButtonPressed()
{
    if(sample != NULL)
    {
        sample->pEditorButton->Release();
        sample->instr_drawarea->Change();
    }
    ChildWindow::closeButtonPressed();
}

void SampleWindow::SetSample(Sample* smp)
{
    sample = smp;
    SampleComponent* scomp = (SampleComponent*)getContentComponent();
    scomp->SetSample(smp);
	if(smp != NULL)
	{
		setName(String(smp->name));
	}
    else
    {
        setName("");
    }
}

void SampleWindow::moved()
{
    //int a = 1;
}

SampleComponent::SampleComponent()      //: quitButton(0)
{
    sample = NULL;

    //AGroupComponent* group1 = new AGroupComponent(T("LFO Flt2 Freq"), T(""));
    //addAndMakeVisible(group1);
    //group1->setBounds(0, 0, 349, 349);

    addAndMakeVisible(wave = new SampleWave(sample));

    int tgx = 0;

    tgnorm = new AToggleButton(T("Normalize"));
    addAndMakeVisible(tgnorm);
    tgnorm->setRadioGroupId(0);
    tgnorm->setBounds(tgx, 171, 70, 22);
    tgnorm->addButtonListener(this);

    tgNoLoop = new AToggleButton(T("No loop"));
    addAndMakeVisible(tgNoLoop);
    tgNoLoop->setRadioGroupId(1);
    tgNoLoop->setBounds(tgx, 188, 70, 22);
    tgNoLoop->addButtonListener(this);

    tgFwdLoop = new AToggleButton(T("Forward loop"));
    addAndMakeVisible(tgFwdLoop);
    tgFwdLoop->setRadioGroupId(1);
    tgFwdLoop->setBounds(tgx, 188 + 12, 90, 22);
    tgFwdLoop->addButtonListener(this);

    tgPPongLoop = new AToggleButton(T("Ping Pong loop"));
    addAndMakeVisible(tgPPongLoop);
    tgPPongLoop->setRadioGroupId(1);
    tgPPongLoop->setBounds(tgx, 188 + 12 + 12, 110, 22);
    tgPPongLoop->addButtonListener(this);

    tgSustain = new AToggleButton(T("Sustain envelope"));
    addAndMakeVisible(tgSustain);
    tgSustain->setRadioGroupId(0);
    tgSustain->setBounds(tgx, 244, 120, 22);
    tgSustain->addButtonListener(this);

    //PlaceDelay(250, 167);
    addAndMakeVisible(env = new EnvelopeComponent(NULL));
    env->drawscale = true;
    //env->drawnumbers = true;
    env->setBounds(1, 257, 349, 94);

    slTime = PlaceSliderWithLabel("Env. Scale", NULL, 249, 233, 98);
    slTime->setText("", false);

    addAndMakeVisible(btAlign = new ATextButton(String::empty));
    btAlign->setButtonText(T("Align scale to sample"));
    btAlign->addButtonListener(this);
    btAlign->setBounds(214, 215, 133, 18);

    setSize(350, 350);
}

void SampleComponent::PlaceDelay(int x, int y)
{
    AGroupComponent* group = new AGroupComponent(T("Delay"), T(""));
    addAndMakeVisible(group);
    group->setBounds(x, y, 137, 80);

    delayDryWet = PlaceSliderWithLabel("DryWet", sample->delay->drywet, x + 7, y + 6);
    delayDelay = PlaceSliderWithLabel("Delay", sample->delay->delay, x + 7, y + 28);
    delayFeedback = PlaceSliderWithLabel("Feedback", sample->delay->feedback, x + 7, y + 50);
    //osc1_detune->setRange(-1, 1);

    addAndMakeVisible(tgdelayON = new AToggleButton(T("ON")));
    tgdelayON->setToggleState(sample->delayOn->outval, false);
    tgdelayON->setRadioGroupId(1);
    tgdelayON->setBounds(x + 86, y + 8, 50, 22);
    tgdelayON->addButtonListener(this);
}

void SampleComponent::buttonClicked(Button* buttonThatWasClicked)
{
    if(buttonThatWasClicked == tgnorm)
    {
        if(sample != NULL)
        {
            sample->ToggleNormalize();
            wave->repaint();
        }
    }
    else if(buttonThatWasClicked == btAlign)
    {
        env->env->len = sample->timelen;
        slTime->setValue(sample->timelen, false);
        slTime->setText(T(""), false);
        env->repaint();
	}
    else if(buttonThatWasClicked == tgdelayON)
    {
        if(sample != NULL)
        {
			sample->delayOn->SetBoolValue(tgdelayON->getToggleState());
		}
	}
    else if(buttonThatWasClicked == tgNoLoop)
    {
        tgSustain->setVisible(false);
        env->env->SetSustainable(false);
        env->repaint();
		if(sample != NULL)
		{
			sample->looptype = LoopType_NoLoop;
			wave->Loopers();
		}
    }
    else if(buttonThatWasClicked == tgFwdLoop)
    {
        tgSustain->setVisible(true);
        env->env->SetSustainable(tgSustain->getToggleState());
        env->repaint();
		if(sample != NULL)
		{
			sample->looptype = LoopType_ForwardLoop;
			wave->Loopers();
		}
    }
    else if(buttonThatWasClicked == tgPPongLoop)
    {
        tgSustain->setVisible(true);
        env->env->SetSustainable(tgSustain->getToggleState());
        env->repaint();
		if(sample != NULL)
		{
			sample->looptype = LoopType_PingPongLoop;
			wave->Loopers();
		}
    }
    else if(buttonThatWasClicked == tgSustain)
    {
        if(env != NULL)
            env->env->SetSustainable(tgSustain->getToggleState());
        repaint();
    }
}

void SampleComponent::sliderValueChanged(ASlider* slider)
{
    if(slider == slTime)
    {
        env->env->len = (float)slider->getValue();
        env->repaint();
        slider->setText("", false);
    }
}

void SampleComponent::resized()
{
    wave->setBounds(3, 6, getWidth() - 6, 160);
    //butt_normalize->setBounds(getWidth() - 60, 163, 55, 22);
}

void SampleComponent::SetSample(Sample* smp)
{
    sample = smp;
    wave->SetSample(smp);
    if(sample != NULL)
    {
        tgnorm->setToggleState(smp->normalized, false);
        if(smp->looptype == LoopType_NoLoop)
        {
            tgNoLoop->setToggleState(true, false);
            tgSustain->setVisible(false);
        }
        else if(smp->looptype == LoopType_ForwardLoop)
        {
            tgFwdLoop->setToggleState(true, false);
            tgSustain->setVisible(true);
        }
        else if(smp->looptype == LoopType_PingPongLoop)
        {
            tgPPongLoop->setToggleState(true, false);
            tgSustain->setVisible(true);
        }

        env->setEnvelope(smp->envVol);
        tgSustain->setToggleState(smp->envVol->sustainable, false);

        float max = 10;
        if(smp->timelen > max)
            max = smp->timelen;

        float min = 1;
        //if(sample->timelen < min)
        //    min = sample->timelen;

        slTime->setRange(min, max);
		slTime->setValue(smp->envVol->len);
        //if(sample->timelen > 2)
        //    slTime->setValue(sample->timelen);
        //else
        //    slTime->setValue(2);
        slTime->setText("", false);
    }
}

Looper::Looper(bool lside, SampleWave * wv)
{
    leftside = lside;
    wave = wv;
}

void Looper::setWave(SampleWave * wv)
{
    wave = wv;
}

void Looper::paint(Graphics & g)
{
    if(isMouseOverOrDragging())
        g.setColour(Colour(0xffFFFF00));
    else
        g.setColour(Colour(0xffFF2020));

    g.setFont(*ti);
    if(leftside)
    {
        g.drawVerticalLine(0, 0, float(getHeight()));
        g.drawHorizontalLine(0, 0, float(getWidth()));
        g.drawHorizontalLine(1, 0, float(getWidth()) - 1);
        g.drawHorizontalLine(2, 0, float(getWidth()) - 2);
        //g.drawHorizontalLine(getHeight() - 1, 0, float(getWidth()));
    }
    else
    {
        g.drawVerticalLine(getWidth() - 1, 0, float(getHeight()));
        //g.drawHorizontalLine(0, 0, float(getWidth()));
        g.drawHorizontalLine(getHeight() - 1, 0, float(getWidth()));
        g.drawHorizontalLine(getHeight() - 2, 1, float(getWidth()));
        g.drawHorizontalLine(getHeight() - 3, 2, float(getWidth()));
    }
}

void Looper::mouseDown(const MouseEvent &e)
{
    dragger.startDraggingComponent(this, 0);
}

void Looper::mouseEnter(const MouseEvent &e)
{
    repaint();
}

void Looper::mouseExit(const MouseEvent &e)
{
    repaint();
}

void Looper::mouseDrag(const MouseEvent &e)
{
    dragger.dragComponent(this, e);
    if(wave != NULL)
    {
        wave->ConstrainLooper(this);
    }
}

SampleWave::SampleWave(Sample* smp)
{
    addChildComponent(right = new Looper(false, this));
    addChildComponent(left = new Looper(true, this));
    setBufferedToImage(true);
    SetSample(smp);
}

void SampleWave::resized()
{
    left->setBounds(3, 0, 4, getHeight());
    right->setBounds(23, 0, 4, getHeight());
}

void SampleWave::Loopers()
{
    if(sample != NULL)
    {
        if(sample->looptype == LoopType_NoLoop)
        {
            left->setVisible(false);
            right->setVisible(false);
        }
        else
        {
            left->setVisible(true);
            right->setVisible(true);
        }
    }
}

void SampleWave::SetSample(Sample* smp)
{
    sample = smp;
	if(sample != NULL)
	{
		wratio = double(sample->sample_info.frames)/getWidth();
		wratio1 = double(sample->sample_info.frames - 1)/getWidth();
		SetLoopPoints(sample->lp_start, sample->lp_end);
        Loopers();
    }
    repaint();
}

void SampleWave::SetLoopPoints(long start, long end)
{
    loopstart = start;
    loopend = end;
    int xl = RoundDouble(loopstart/wratio1);
    int xr = RoundDouble(loopend/wratio1 - 4);
    left->setBounds(xl, left->getY(), left->getWidth(), left->getHeight());
    right->setBounds(xr, right->getY(), right->getWidth(), right->getHeight());
}

void SampleWave::ConstrainLooper(Looper * looper)
{
    if(looper == left)
    {
        int x = left->getX();
        int y = left->getY();
        if(x < 0)
            x = 0;
        else if(x > right->getX() - 1)
            x = right->getX() - 1;

        if(y != 0)
            y = 0;

        left->setBounds(x, y, left->getWidth(), left->getHeight());
        sample->SetLoopStart(long(x*wratio1));
    }
    else if(looper == right)
    {
        int x = right->getX();
        int y = right->getY();
        if(x < left->getX() + 1)
            x = left->getX() + 1;
        else if(x > getWidth() - 4)
            x = getWidth() - 4;

        if(y != 0)
            y = 0;

        right->setBounds(x, y, right->getWidth(), right->getHeight());
        sample->SetLoopEnd(RoundDoubleLong((x + 4)*wratio1));
    }
}

void SampleWave::mouseMove(const MouseEvent &e)
{
    
}

void SampleWave::mouseDrag(const MouseEvent &e)
{
    
}

void SampleWave::mouseDown(const MouseEvent &e)
{
    if(sample != NULL)
    {
        Preview_Add(NULL, sample, -1, -1, NULL, NULL, NULL);
    }
}

void SampleWave::mouseUp(const MouseEvent &e)
{
    if(sample != NULL)
    {
        Preview_MouseRelease();
    }
}

void SampleWave::mouseExit(const MouseEvent &e)
{
    
}

void SampleWave::paint(Graphics & g)
{
    g.fillAll(Colour(46, 62, 68));
    g.setColour(Colour(36, 44, 50));
    g.drawRect(0, 0, getWidth(), getHeight());

    if(sample != NULL)
    {
        if(sample != NULL && sample->sample_info.frames > 0)
        {
            int w = getWidth() - 1;
            wratio = double(sample->sample_info.frames)/w;
            long nframes = long(sample->sample_info.frames);
            float h2 = float(getHeight() - 1)/2;
            float* sdata = sample->sample_data;
            int step = sample->sample_info.channels;
            g.setColour(Colour(125, 125, 255));
            g.drawHorizontalLine((int)h2, 1, (float)w - 1);
            g.setColour(Colour(165, 185, 200));
            if(wratio > 1)
            {
                double fc = 0;
                int x = 1;
                long fc1, fc2;
                fc1 = fc2 = 0;
                float top, bottom;
                float tmpdata;
                while(x < w && (long)fc < nframes)
                {
                    fc += wratio;
                    if((long)fc >= nframes)
                    {
                        fc = nframes - 1;
                    }
                    fc1 = fc2;
                    fc2 = (long)fc;
                    top = bottom = sdata[fc1*step];
                    while(fc1 < fc2)
                    {
                        fc1++;
                        tmpdata = sdata[fc1*step];
                        if(tmpdata > top)
                        {
                            top = tmpdata;
                        }
                        else if(tmpdata < bottom)
                        {
                            bottom = tmpdata;
                        }
                    }

                    top = float(int(h2 - top*h2));
                    bottom = float(int(h2 - bottom*h2));
                    if(top != bottom)
                    {
                        g.drawVerticalLine(x, top, bottom);
                    }
                    else
                    {
                        g.setPixel(x, int(top));
                    }

                    x++;
                }
				int a = 1;
            }
            else
            {
                float xlen = float(1.0f/wratio);
                float x = 0;
                float y;
                long fc = 0;
                while(fc < nframes)
                {
                    y = h2 - sdata[fc*step]*h2;
                    g.drawHorizontalLine((int)y, x, x + xlen);
                    fc++;
                    x += xlen;
                }
            }
        }
    }
}

ConfigWindow::ConfigWindow() : ChildWindow("Preferences")
{
    ConfigComponent* const contentComponent = new ConfigComponent();
    setContentComponent (contentComponent, true, true);
    centreWithSize(getWidth(), getHeight());
}

ConfigWindow::~ConfigWindow()
{
    // (the content component will be deleted automatically, so no need to do it here)
}

RenderWindow::RenderWindow() : ChildWindow("Render")
{
    RenderComponent* const contentComponent = new RenderComponent();
    this->RC = contentComponent;
    setContentComponent (contentComponent, true, true);
    centreWithSize(getWidth(), getHeight());
}

void RenderWindow::ResetToStart()
{
    MessageManagerLock ml;
    this->RC->slider->setValue(0, false, false);
    this->RC->startButt->setButtonText(T("Start"));
}

RenderWindow::~RenderWindow()
{
    // (the content component will be deleted automatically, so no need to do it here)
}

static Array <VSTPluginComponent*> activeMyVSTWindows;

class VSTPluginComponent   : //public AudioProcessorEditor,
                            public Component,
                            public Timer
{
public:

    VSTPluginComponent (CVSTPlugin *pPlug)
        : //AudioProcessorEditor (&plugin_),
          _plugin(pPlug),
          isOpen (false),
          wasShowing (false),
          pluginRefusesToResize (false),
          pluginWantsKeys (false),
          alreadyInside (false),
          recursiveResize (false)
    {
#if JUCE_WIN32
        sizeCheckCount = 0;
        pluginHWND = 0;
#elif JUCE_LINUX
        pluginWindow = None;
        pluginProc = None;
#else
        pluginViewRef = 0;
#endif

        movementWatcher = new CompMovementWatcher (this);

        //activeVSTWindows.add (this);

        setSize (1, 1);
        setOpaque (true);
        //setVisible (true);
        //setBufferedToImage(true);
    }

    ~VSTPluginComponent()
    {
        deleteAndZero (movementWatcher);

        closePluginWindow();

        activeMyVSTWindows.removeValue (this);
        //plugin.editorBeingDeleted (this);
    }

    void componentMovedOrResized()
    {
        if (recursiveResize)
            return;

        Component* const topComp = getTopLevelComponent();

        if (topComp->getPeer() != 0)
        {
            int x = 0, y = 0;
            relativePositionToOtherComponent (topComp, x, y);

            recursiveResize = true;

#if JUCE_MAC
            if (pluginViewRef != 0)
            {
                HIRect r;
                r.origin.x = (float) x;
                r.origin.y = (float) y;
                r.size.width = (float) getWidth();
                r.size.height = (float) getHeight();
                HIViewSetFrame (pluginViewRef, &r);
            }
            else if (pluginWindowRef != 0)
            {
                Rect r;
                r.left = getScreenX();
                r.top = getScreenY();
                r.right = r.left + getWidth();
                r.bottom = r.top + getHeight();

                WindowGroupRef group = GetWindowGroup (pluginWindowRef);
                WindowGroupAttributes atts;
                GetWindowGroupAttributes (group, &atts);
                ChangeWindowGroupAttributes (group, 0, kWindowGroupAttrMoveTogether);

                SetWindowBounds (pluginWindowRef, kWindowContentRgn, &r);

                if ((atts & kWindowGroupAttrMoveTogether) != 0)
                    ChangeWindowGroupAttributes (group, kWindowGroupAttrMoveTogether, 0);
            }
            else
            {
                repaint();
            }
#elif JUCE_WIN32
            if (pluginHWND != 0)
                MoveWindow (pluginHWND, x, y, getWidth(), getHeight(), TRUE);
#elif JUCE_LINUX
            if (pluginWindow != 0)
            {
                XResizeWindow (display, pluginWindow, getWidth(), getHeight());
                XMoveWindow (display, pluginWindow, x, y);
                XMapRaised (display, pluginWindow);
            }
#endif

            recursiveResize = false;
        }
    }

    void componentVisibilityChanged()
    {
        const bool isShowingNow = isShowing();

        if (wasShowing != isShowingNow)
        {
            wasShowing = isShowingNow;

            if (isShowingNow)
                openPluginWindow();
            else
                closePluginWindow();
        }

        componentMovedOrResized();
    }

    void componentPeerChanged()
    {
        closePluginWindow();
        openPluginWindow();
    }

    bool keyStateChanged()
    {
        return pluginWantsKeys;
    }

    bool keyPressed (const KeyPress&)
    {
        return pluginWantsKeys;
    }

    void paint (Graphics& g)
    {
        if (isOpen)
        {
            ComponentPeer* const peer = getPeer();

            if (peer != 0)
            {
                //peer->addMaskedRegion (getScreenX() - peer->getScreenX(),
                //                       getScreenY() - peer->getScreenY(),
                //                       getWidth(), getHeight());

#if JUCE_MAC
                if (pluginViewRef == 0)
                {
                    ERect r;
                    r.left = getScreenX() - peer->getScreenX();
                    r.right = r.left + getWidth();
                    r.top = getScreenY() - peer->getScreenY();
                    r.bottom = r.top + getHeight();

                    dispatch (effEditDraw, 0, 0, &r, 0);
                }
#elif JUCE_LINUX
                if (pluginWindow != 0)
                {
                    const Rectangle clip (g.getClipBounds());

                    XEvent ev;
                    zerostruct (ev);
                    ev.xexpose.type = Expose;
                    ev.xexpose.display = display;
                    ev.xexpose.window = pluginWindow;
                    ev.xexpose.x = clip.getX();
                    ev.xexpose.y = clip.getY();
                    ev.xexpose.width = clip.getWidth();
                    ev.xexpose.height = clip.getHeight();

                    sendEventToChild (&ev);
                }
#endif
            }
        }
        else
        {
            g.fillAll (Colours::black);
        }
    }

    void timerCallback()
    {
#if JUCE_WIN32
        if (--sizeCheckCount <= 0)
        {
            sizeCheckCount = 10;

            checkPluginWindowSize();
        }
#endif

        try
        {
            static bool reentrant = false;

            if (! reentrant)
            {
                reentrant = true;
                //_plugin->EditIdle();
                dispatch (effEditIdle, 0, 0, 0, 0);
                reentrant = false;
            }
        }
        catch (...)
        {}
    }

    void mouseDown (const MouseEvent& e)
    {
#if JUCE_MAC
        if (! alreadyInside)
        {
            alreadyInside = true;
            toFront (true);
            dispatch (effEditMouse, e.x, e.y, 0, 0);
            alreadyInside = false;
        }
        else
        {
            PostEvent (::mouseDown, 0);
        }
#elif JUCE_LINUX

        if (pluginWindow == 0)
            return;

        toFront (true);

        XEvent ev;
        zerostruct (ev);
        ev.xbutton.display = display;
        ev.xbutton.type = ButtonPress;
        ev.xbutton.window = pluginWindow;
        ev.xbutton.root = RootWindow (display, DefaultScreen (display));
        ev.xbutton.time = CurrentTime;
        ev.xbutton.x = e.x;
        ev.xbutton.y = e.y;
        ev.xbutton.x_root = e.getScreenX();
        ev.xbutton.y_root = e.getScreenY();

        translateJuceToXButtonModifiers (e, ev);

        sendEventToChild (&ev);

#else
        (void) e;

        toFront (true);
#endif
    }

    void broughtToFront()
    {
        activeMyVSTWindows.removeValue (this);
        activeMyVSTWindows.add (this);

#if JUCE_MAC
        dispatch (effEditTop, 0, 0, 0, 0);
#endif
    }

private:
    CVSTPlugin * _plugin;
    bool isOpen, wasShowing, recursiveResize;
    bool pluginWantsKeys, pluginRefusesToResize, alreadyInside;

#if JUCE_WIN32
    HWND pluginHWND;
    void* originalWndProc;
    int sizeCheckCount;
#elif JUCE_MAC
    HIViewRef pluginViewRef;
    WindowRef pluginWindowRef;
#elif JUCE_LINUX
    Window pluginWindow;
    EventProcPtr pluginProc;
#endif

    void openPluginWindow()
    {
        if (isOpen || getWindowHandle() == 0)
            return;

        isOpen = true;

        ERect* rect = 0;
        //plugin.pEffect
        dispatch (effEditGetRect, 0, 0, &rect, 0);
        dispatch (effEditOpen, 0, 0, getWindowHandle(), 0);

        // do this before and after like in the steinberg example
        dispatch (effEditGetRect, 0, 0, &rect, 0);
        dispatch (effGetProgram, 0, 0, 0, 0); // also in steinberg code

        // Install keyboard hooks
        pluginWantsKeys = (dispatch (effKeysRequired, 0, 0, 0, 0) == 0);

#if JUCE_WIN32
        originalWndProc = 0;
        pluginHWND = GetWindow ((HWND) getWindowHandle(), GW_CHILD);

        if (pluginHWND == 0)
        {
            isOpen = false;
            setSize (300, 150);
            return;
        }

        #pragma warning (push)
        #pragma warning (disable: 4244)

        originalWndProc = (void*) GetWindowLongPtr (pluginHWND, GWL_WNDPROC);

        //if (! pluginWantsKeys)
        //    SetWindowLongPtr (pluginHWND, GWL_WNDPROC, (LONG_PTR) vstHookWndProc);

        #pragma warning (pop)

        int w, h;
        RECT r;
        GetWindowRect (pluginHWND, &r);
        w = r.right - r.left;
        h = r.bottom - r.top;

        if (rect != 0)
        {
            const int rw = rect->right - rect->left;
            const int rh = rect->bottom - rect->top;

            if ((rw > 50 && rh > 50 && rw < 2000 && rh < 2000 && rw != w && rh != h)
                || ((w == 0 && rw > 0) || (h == 0 && rh > 0)))
            {
                // very dodgy logic to decide which size is right.
                if (abs (rw - w) > 350 || abs (rh - h) > 350)
                {
                    SetWindowPos (pluginHWND, 0,
                                  0, 0, rw, rh,
                                  SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);

                    GetWindowRect (pluginHWND, &r);

                    w = r.right - r.left;
                    h = r.bottom - r.top;

                    pluginRefusesToResize = (w != rw) || (h != rh);

                    w = rw;
                    h = rh;
                }
            }
        }
#elif JUCE_MAC
        HIViewRef root = HIViewGetRoot ((WindowRef) getWindowHandle());
        HIViewFindByID (root, kHIViewWindowContentID, &root);
        pluginViewRef = HIViewGetFirstSubview (root);

        while (pluginViewRef != 0 && juce_isHIViewCreatedByJuce (pluginViewRef))
            pluginViewRef = HIViewGetNextView (pluginViewRef);

        pluginWindowRef = 0;

        if (pluginViewRef == 0)
        {
            WindowGroupRef ourGroup = GetWindowGroup ((WindowRef) getWindowHandle());
            //DebugPrintWindowGroup (ourGroup);
            //DebugPrintAllWindowGroups();

            GetIndexedWindow (ourGroup, 1,
                              kWindowGroupContentsVisible,
                              &pluginWindowRef);

            if (pluginWindowRef == (WindowRef) getWindowHandle()
                 || juce_isWindowCreatedByJuce (pluginWindowRef))
                pluginWindowRef = 0;
        }

        int w = 250, h = 150;

        if (rect != 0)
        {
            w = rect->right - rect->left;
            h = rect->bottom - rect->top;

            if (w == 0 || h == 0)
            {
                w = 250;
                h = 150;
            }
        }

#elif JUCE_LINUX
        pluginWindow = getChildWindow ((Window) getWindowHandle());

        if (pluginWindow != 0)
            pluginProc = (EventProcPtr) getPropertyFromXWindow (pluginWindow,
                                                                XInternAtom (display, "_XEventProc", False));

        int w = 250, h = 150;

        if (rect != 0)
        {
            w = rect->right - rect->left;
            h = rect->bottom - rect->top;

            if (w == 0 || h == 0)
            {
                w = 250;
                h = 150;
            }
        }

        if (pluginWindow != 0)
            XMapRaised (display, pluginWindow);
#endif

        // double-check it's not too tiny
        w = jmax (w, 32);
        h = jmax (h, 32);

        setSize (w, h);

#if JUCE_WIN32
        checkPluginWindowSize();
#endif

        startTimer (18 + JUCE_NAMESPACE::Random::getSystemRandom().nextInt (5));
        repaint();
    }

    void closePluginWindow()
    {
        if (isOpen)
        {
            isOpen = false;

            dispatch (effEditClose, 0, 0, 0, 0);

#if JUCE_WIN32
            #pragma warning (push)
            #pragma warning (disable: 4244)

            if (pluginHWND != 0 && IsWindow (pluginHWND))
                SetWindowLongPtr (pluginHWND, GWL_WNDPROC, (LONG_PTR) originalWndProc);

            #pragma warning (pop)

            stopTimer();

            if (pluginHWND != 0 && IsWindow (pluginHWND))
                DestroyWindow (pluginHWND);

            pluginHWND = 0;
#elif JUCE_MAC
            dispatch (effEditSleep, 0, 0, 0, 0);
            pluginViewRef = 0;
            stopTimer();
#elif JUCE_LINUX
            stopTimer();
            pluginWindow = 0;
            pluginProc = 0;
#endif
        }
    }

#if JUCE_WIN32
    void checkPluginWindowSize() throw()
    {
        RECT r;
        GetWindowRect (pluginHWND, &r);
        const int w = r.right - r.left;
        const int h = r.bottom - r.top;

        if (isShowing() && w > 0 && h > 0
             && (w != getWidth() || h != getHeight())
             && ! pluginRefusesToResize)
        {
            setSize (w, h);
            sizeCheckCount = 0;
        }
    }
#endif

    class CompMovementWatcher  : public ComponentMovementWatcher
    {
    public:
        CompMovementWatcher (VSTPluginComponent* const owner_)
            : ComponentMovementWatcher (owner_),
              owner (owner_)
        {
        }

        void componentMovedOrResized (bool /*wasMoved*/, bool /*wasResized*/)
        {
            owner->componentMovedOrResized();
        }

        void componentPeerChanged()
        {
            owner->componentPeerChanged();
        }

        void componentVisibilityChanged (Component&)
        {
            owner->componentVisibilityChanged();
        }

    private:
        VSTPluginComponent* const owner;
    };

    CompMovementWatcher* movementWatcher;

    int dispatch (const int opcode, const int index, const int value, void* const ptr, float opt)
    {
        return _plugin->dispatch (opcode, index, value, ptr, opt);
    }

    // hooks to get keyboard events from VST windows..
#if JUCE_WIN32
    static LRESULT CALLBACK vstHookWndProc (HWND hW, UINT message, WPARAM wParam, LPARAM lParam)
    {
        for (int i = activeMyVSTWindows.size(); --i >= 0;)
        {
            const VSTPluginComponent* const w = (const VSTPluginComponent*) activeMyVSTWindows.getUnchecked (i);

            if (w->pluginHWND == hW)
            {
                if (message == WM_CHAR
                    || message == WM_KEYDOWN
                    || message == WM_SYSKEYDOWN
                    || message == WM_KEYUP
                    || message == WM_SYSKEYUP
                    /*|| message == WM_APPCOMMAND*/)
                {
                    SendMessage ((HWND) w->getTopLevelComponent()->getWindowHandle(),
                                 message, wParam, lParam);
                }

                return CallWindowProc ((WNDPROC) (w->originalWndProc),
                                       (HWND) w->pluginHWND,
                                       message,
                                       wParam,
                                       lParam);
            }
        }

        return DefWindowProc (hW, message, wParam, lParam);
    }
#endif

#if JUCE_LINUX

    // overload mouse/keyboard events to forward them to the plugin's inner window..
    void sendEventToChild (XEvent* event)
    {
        if (pluginProc != 0)
        {
            // if the plugin publishes an event procedure, pass the event directly..
            pluginProc (event);
        }
        else if (pluginWindow != 0)
        {
            // if the plugin has a window, then send the event to the window so that
            // its message thread will pick it up..
            XSendEvent (display, pluginWindow, False, 0L, event);
            XFlush (display);
        }
    }

    void mouseEnter (const MouseEvent& e)
    {
        if (pluginWindow != 0)
        {
            XEvent ev;
            zerostruct (ev);
            ev.xcrossing.display = display;
            ev.xcrossing.type = EnterNotify;
            ev.xcrossing.window = pluginWindow;
            ev.xcrossing.root = RootWindow (display, DefaultScreen (display));
            ev.xcrossing.time = CurrentTime;
            ev.xcrossing.x = e.x;
            ev.xcrossing.y = e.y;
            ev.xcrossing.x_root = e.getScreenX();
            ev.xcrossing.y_root = e.getScreenY();
            ev.xcrossing.mode = NotifyNormal; // NotifyGrab, NotifyUngrab
            ev.xcrossing.detail = NotifyAncestor; // NotifyVirtual, NotifyInferior, NotifyNonlinear,NotifyNonlinearVirtual

            translateJuceToXCrossingModifiers (e, ev);

            sendEventToChild (&ev);
        }
    }

    void mouseExit (const MouseEvent& e)
    {
        if (pluginWindow != 0)
        {
            XEvent ev;
            zerostruct (ev);
            ev.xcrossing.display = display;
            ev.xcrossing.type = LeaveNotify;
            ev.xcrossing.window = pluginWindow;
            ev.xcrossing.root = RootWindow (display, DefaultScreen (display));
            ev.xcrossing.time = CurrentTime;
            ev.xcrossing.x = e.x;
            ev.xcrossing.y = e.y;
            ev.xcrossing.x_root = e.getScreenX();
            ev.xcrossing.y_root = e.getScreenY();
            ev.xcrossing.mode = NotifyNormal; // NotifyGrab, NotifyUngrab
            ev.xcrossing.detail = NotifyAncestor; // NotifyVirtual, NotifyInferior, NotifyNonlinear,NotifyNonlinearVirtual
            ev.xcrossing.focus = hasKeyboardFocus (true); // TODO - yes ?

            translateJuceToXCrossingModifiers (e, ev);

            sendEventToChild (&ev);
        }
    }

    void mouseMove (const MouseEvent& e)
    {
        if (pluginWindow != 0)
        {
            XEvent ev;
            zerostruct (ev);
            ev.xmotion.display = display;
            ev.xmotion.type = MotionNotify;
            ev.xmotion.window = pluginWindow;
            ev.xmotion.root = RootWindow (display, DefaultScreen (display));
            ev.xmotion.time = CurrentTime;
            ev.xmotion.is_hint = NotifyNormal;
            ev.xmotion.x = e.x;
            ev.xmotion.y = e.y;
            ev.xmotion.x_root = e.getScreenX();
            ev.xmotion.y_root = e.getScreenY();

            sendEventToChild (&ev);
        }
    }

    void mouseDrag (const MouseEvent& e)
    {
        if (pluginWindow != 0)
        {
            XEvent ev;
            zerostruct (ev);
            ev.xmotion.display = display;
            ev.xmotion.type = MotionNotify;
            ev.xmotion.window = pluginWindow;
            ev.xmotion.root = RootWindow (display, DefaultScreen (display));
            ev.xmotion.time = CurrentTime;
            ev.xmotion.x = e.x ;
            ev.xmotion.y = e.y;
            ev.xmotion.x_root = e.getScreenX();
            ev.xmotion.y_root = e.getScreenY();
            ev.xmotion.is_hint = NotifyNormal;

            translateJuceToXMotionModifiers (e, ev);
            sendEventToChild (&ev);
        }
    }

    void mouseUp (const MouseEvent& e)
    {
        if (pluginWindow != 0)
        {
            XEvent ev;
            zerostruct (ev);
            ev.xbutton.display = display;
            ev.xbutton.type = ButtonRelease;
            ev.xbutton.window = pluginWindow;
            ev.xbutton.root = RootWindow (display, DefaultScreen (display));
            ev.xbutton.time = CurrentTime;
            ev.xbutton.x = e.x;
            ev.xbutton.y = e.y;
            ev.xbutton.x_root = e.getScreenX();
            ev.xbutton.y_root = e.getScreenY();

            translateJuceToXButtonModifiers (e, ev);
            sendEventToChild (&ev);
        }
    }

    void mouseWheelMove (const MouseEvent& e,
                         float incrementX,
                         float incrementY)
    {
        if (pluginWindow != 0)
        {
            XEvent ev;
            zerostruct (ev);
            ev.xbutton.display = display;
            ev.xbutton.type = ButtonPress;
            ev.xbutton.window = pluginWindow;
            ev.xbutton.root = RootWindow (display, DefaultScreen (display));
            ev.xbutton.time = CurrentTime;
            ev.xbutton.x = e.x;
            ev.xbutton.y = e.y;
            ev.xbutton.x_root = e.getScreenX();
            ev.xbutton.y_root = e.getScreenY();

            translateJuceToXMouseWheelModifiers (e, incrementY, ev);
            sendEventToChild (&ev);

            // TODO - put a usleep here ?

            ev.xbutton.type = ButtonRelease;
            sendEventToChild (&ev);
        }
    }
#endif
};


PluginEditWindow::PluginEditWindow(void * ParentWindow, CVSTPlugin * pPlug)
                            : PluginCommonWindow()
{
    //this->setTitleBarButtonsRequired((DocumentWindow::minimiseButton | DocumentWindow::closeButton),
    //                                  false);
    VSTPluginComponent* uiComp = new VSTPluginComponent(pPlug);
    pPlugin = pPlug;
	scope = pPlug->scope;
    savepresetbt->setVisible(false);
    //loadpresetbt->setBounds(getWidth() - 57, 2, 20, 19);
    
	ParentHandle = ParentWindow;
    setSize(300,400);
    setContentComponent (uiComp, true, true);

    //char name[256];
    //pPlug->GetDisplayName(name, 0);
    SetTitle();

    setCentreRelative(0.5f,0.5f);
    MainWnd->AddChild(this);
}

void PluginEditWindow::closeButtonPressed()
{
    this->Hide();
    this->pPlugin->isWindowActive = false;

    PluginCommonWindow::closeButtonPressed();
}

void PluginEditWindow::ShowWindow()
{
	this->Show();
}

void PluginEditWindow::Close()
{
	this->Hide();
}

void PluginEditWindow::SetFocus()
{
    VSTPluginComponent* uiComp = (VSTPluginComponent*)this->getContentComponent();
    uiComp->broughtToFront();
}

void PluginEditWindow::SetTitle()
{
    char name[31] = {0};
    char *prog = NULL;

    this->pPlugin->GetDisplayName(name,30);
    //this->pPlugin->GetProgramName(prog); // temporarily postpone preset name

    String strName(name);

    //strName += ": ";
    //strName += prog;

    this->setName(strName);
}

PluginEditWindow::~PluginEditWindow()
{
    MainWnd->RemoveChild(this);
    //MainWnd->removeChildComponent(this);
}

void ToggleInstrWindow()
{
    if(SmpWnd->Showing() == false)
    {
        SmpWnd->Show();
    }
    else
    {
        //SmpWnd->Hide();
        SmpWnd->toFront(true);
    }
}

void MinimizeWindow()
{
    MainWnd->setMinimised(true);
}

void MaximizeWindow()
{
    MainWnd->setMinimised(false);
}

void FullScreenON()
{
    MainWnd->setFullScreen(true);
    MainWnd->setTitleBarHeight(0);
}

void FullScreenOFF()
{
    MainWnd->setFullScreen(false);
    MainWnd->setTitleBarHeight(MAIN_WINDOW_TITLE_BAR_HEIGHT);
}

void ToggleFullScreen()
{
    // Delete popup menu to avoid corruption
    if(M.contextmenu != NULL)
    {
        M.DeleteContextMenu();
    }

    if(MainWnd->isFullScreen() == true)
    {
        FullScreenOFF();
    }
    else if(MainWnd->isFullScreen() == false)
    {
        FullScreenON();
    }
}

void ToggleConfigWindow()
{
    if(ConfigWnd->Showing() == true)
    {
        ConfigWnd->Hide();
    }
    else
    {
        ConfigWnd->Show();
    }
}

void ToggleRenderWindow()
{
    if(RenderWnd->Showing() == true)
    {
        RenderWnd->Hide();
    }
    else
    {
        RenderWnd->Show();
    }
}

void ToBehind()
{
    if(ConfigWnd != NULL)
    {
        if(ConfigWnd->Showing())
        {
        }
    }
}

void ToFront()
{
    MainWnd->ToFront();
    MainWnd->tofronted = true;
}

