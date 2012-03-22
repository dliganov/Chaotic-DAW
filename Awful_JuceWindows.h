#ifndef JUCEWINDOWS_H
#define JUCEWINDOWS_H

#include "Awful.h"

class VSTParamWindow;
class ParamWindow;
class SampleWindow;
class SynthWindow;
class EnvelopeComponent;
class SampleWave;
class ScanWindow;

extern AwfulWindow*     MainWnd;
extern ConfigWindow*    ConfigWnd;
extern SampleWindow*     SmpWnd;


extern void ToggleInstrWindow();


class SavePresetButton   : public AButton
{
public:

    SavePresetButton (const String& name) throw()
        : AButton (name)
    {
        imgup = img_savepreset1;
        imgdn = img_savepreset2;
    }

    ~SavePresetButton()
    {
    }

    void paintButton (Graphics& g, bool isMouseOverButton, bool isButtonDown)
    {
        AButton::paintButton(g, isMouseOverButton, isButtonDown);

        if(isButtonDown)
        {
            if(imgdn != NULL)
            {
                g.drawImageAt(imgdn, 1, 1, false);
            }
        }
        else
        {
            if(imgup != NULL)
            {
                g.drawImageAt(imgup, 1, 1, false);
            }
        }
    }

    juce_UseDebuggingNewOperator

private:
    Colour colour;
    Path normalShape, toggledShape;
    Image* imgup;
	Image* imgdn;

    SavePresetButton (const SavePresetButton&);
    const SavePresetButton& operator= (const SavePresetButton&);
};

class LoadPresetButton   : public AButton
{
public:

    LoadPresetButton (const String& name) throw()
        : AButton (name)
    {
        imgup = img_showpresets1;
        imgdn = img_showpresets2;
    }

    ~LoadPresetButton()
    {
    }

    void paintButton (Graphics& g, bool isMouseOverButton, bool isButtonDown)
    {
        AButton::paintButton(g, isMouseOverButton, isButtonDown);

        if(isButtonDown)
        {
            if(imgdn != NULL)
            {
                g.drawImageAt(imgdn, 1, 1, false);
            }
        }
        else
        {
            if(imgup != NULL)
            {
                g.drawImageAt(imgup, 1, 1, false);
            }
        }
    }

    juce_UseDebuggingNewOperator

private:
    Colour colour;
    Path normalShape, toggledShape;
    Image* imgup;
    Image* imgdn;

    LoadPresetButton (const LoadPresetButton&);
    const LoadPresetButton& operator= (const LoadPresetButton&);
};

class AwfulWindow  : public DocumentWindow
{
public:
    
    bool tofronted;
    //==============================================================================
    AwfulWindow();
    ~AwfulWindow();
    void AwfulWindow::UpdateTitle();
    void mouseDoubleClick (const MouseEvent& e);
    void closeButtonPressed();
    void mouseDown(const MouseEvent& e);
    bool keyPressed(const KeyPress& key);
    bool keyStateChanged(const bool isKeyDown);
    void activeWindowStatusChanged();
    void parentHierarchyChanged();
    void broughtToFront();
    void AddChild(ChildWindow* cw);
    void RemoveChild(ChildWindow* cw);
    void DeleteChild(ChildWindow* cw);
    void ToFront();
    void paint(Graphics& g);
    void drawBorder (Graphics& g, int w, int h, const BorderSize& border, ResizableWindow&);
    void MinimizeChilds();
    void MaximizeChilds();
    void minimisationStateChanged (bool isNowMinimised);
    VSTParamWindow* CreateVSTParamWindow(VSTEffect* vsteff, Scope* scope);
    ParamWindow* CreateParamWindow(Scope* scope);
    SynthWindow* CreateSynthWindow(Synth* syn);
    ChildWindow* CreateAboutWindow();
#if(RELEASEBUILD == FALSE)
    ChildWindow* CreateLicenseWindow();
#endif
    ChildWindow* CreateHotKeysWindow();
    ScanWindow* CreatePlugScanWindow();

private:
    ChildWindow*    first_child;
    ChildWindow*    last_child;
};

class ChildWindow      : public DocumentWindow
{
public:
    ChildWindow(const char* name);
    ~ChildWindow();
    int getDesktopWindowStyleFlags() const;
    bool Showing();
    void Show();
    void Hide();
    void paint(Graphics& g);
    void parentHierarchyChanged();
    void broughtToFront();
    void mouseDown();
    void closeButtonPressed();
    void lookAndFeelChanged();
    void drawChildWindowBorder (Graphics& g, int w, int h,
                                 const BorderSize& border, ResizableWindow&);

    ChildWindow* prev;
    ChildWindow* next;

    AwfulWindow* parent;
    bool         bypassfront;

    Image*       wndtitle1;
    Image*       wndtitle2;
    Image*       wndtitle3;

private:
    bool visible;
};

class PluginCommonWindow      : public ChildWindow,
                          public ButtonListener
{
public:
    Scope*      scope;

    LoadPresetButton* loadpresetbt;
    SavePresetButton* savepresetbt;

    PluginCommonWindow();
    ~PluginCommonWindow();
    void resized();
    void buttonClicked(Button * button);
    void closeButtonPressed();
};

class ParamWindow      : public PluginCommonWindow
{
public:

    ParamWindow(Scope* sc);
    ~ParamWindow();
};

class VSTParamWindow      : public PluginCommonWindow
{
public:
    Paramcell*  first_pcell;
    Paramcell*  last_pcell;

    VSTParamWindow(VSTEffect* vsteff, Scope* sc);
    ~VSTParamWindow();
};


class CComponent : public Component,
                       public ASliderListener,
                       public ButtonListener
{
public:
    virtual ASlider* PlaceSliderWithLabel(char* txt, Parameter* param, int x, int y, int w = 83, int h = 22);
    virtual AToggleButton* PlaceToggleWithLabel(char* txt, BoolParam* bparam, int tgtype, int group, int x, int y, int w, int h);
    virtual ALabel* PlaceSmallLabel(char* txt, int x, int y, Colour& clr);
    virtual ALabel* PlaceBigLabel(char* txt, int x, int y, Colour& clr);
    virtual ALabel* PlaceTxtLabel(char* txt, int x, int y, Colour& clr);
    virtual ALabel* PlaceTxtLabel1(char* txt, int x, int y, Colour& clr);
    void sliderValueChanged(ASlider* slider) {};
    void buttonClicked(Button* button) {};
};

class HotKeysWindow  : public ChildWindow
{
public:

    HotKeysWindow();
    ~HotKeysWindow();
};

class HotKeysComponent : public CComponent
{
public:
    AGroupComponent* group;
    Image* hotkeysimg;

    HotKeysComponent();
    ~HotKeysComponent() {};
    void buttonClicked(Button* button);
    void paint(Graphics& g);
};

class AboutComponent : public CComponent
{
public:
    Image* keysimg;

    AboutComponent();
    ~AboutComponent() {};
    void buttonClicked(Button* button);
    void paint(Graphics& g);
};

class LicenseComponent : public CComponent,
                                public AComboBoxListener,
                                public TextEditorListener
{
public:
    ATextButton*    generateButton;
    TextEditor*     userName;
    TextEditor*     userEmail;

    LicenseComponent();
    ~LicenseComponent() {};
    void buttonClicked(Button* button);
    void paint(Graphics& g);
    void comboBoxChanged (AComboBox* comboBoxThatHasChanged) {};
    void textEditorReturnKeyPressed (TextEditor& editor) {};
    void textEditorTextChanged (TextEditor& editor) {};
    void textEditorEscapeKeyPressed (TextEditor& editor) {};
    void textEditorFocusLost (TextEditor& editor) {};
};

class ParamComponent : public CComponent
{
public:
    Scope*      scope;
    Parameter*  firstparam;

    Eff*            pEff;
    Instrument*     instr;

    ParamComponent(Scope* sc);
    ~ParamComponent() {};
    void buttonClicked(Button* button);
    void sliderValueChanged(ASlider* slider);
};

class EnvelopeComponent : public Component
{
public:
    Envelope*   env;
    EnvPnt*     active_pnt;
    EnvAction   env_action;
    float       env_xstart;
    bool        v_only;
    bool        drawscale;
    bool        drawnumbers;
    int         x;
    int         y;

    EnvelopeComponent();
    EnvelopeComponent(Envelope*       nenv);
    ~EnvelopeComponent();
    void paint(Graphics& g);
    void mouseMove(const MouseEvent &);
    void mouseExit(const MouseEvent &);
    void mouseDrag(const MouseEvent &);
    void mouseDown(const MouseEvent &);
    void setEnvelope(Envelope*    nenv);
    //void mouseDown(const MouseEvent &);
};

class SynthComponent : public CComponent
{
public:
    Synth*              synth;

    AGroupComponent*    osc1_group;
    ASlider*            osc1_oct;
    ASlider*            osc1_detune;
    ASlider*            osc1_level;
    ASlider*            osc1_width;
    AToggleButton*      osc1_saw;
    AToggleButton*      osc1_sine;
    AToggleButton*      osc1_tri;
    AToggleButton*      osc1_pulse;
    AToggleButton*      osc1_noise;

    AGroupComponent*    osc2_group;
    ASlider*            osc2_oct;
    ASlider*            osc2_detune;
    ASlider*            osc2_level;
    ASlider*            osc2_width;
    AToggleButton*      osc2_saw;
    AToggleButton*      osc2_sine;
    AToggleButton*      osc2_tri;
    AToggleButton*      osc2_pulse;
    AToggleButton*      osc2_noise;

    AGroupComponent*    osc3_group;
    ASlider*            osc3_oct;
    ASlider*            osc3_detune;
    ASlider*            osc3_level;
    ASlider*            osc3_width;
    AToggleButton*      osc3_saw;
    AToggleButton*      osc3_sine;
    AToggleButton*      osc3_tri;
    AToggleButton*      osc3_pulse;
    AToggleButton*      osc3_noise;

    AToggleButton*      osc1_fat;
    AToggleButton*      osc2_fat;
    AToggleButton*      osc3_fat;

    ASlider*            noise_level;
    AToggleButton*      noise_type;

    AGroupComponent*    filt1_group;
    ASlider*            filt1_freq;
    ASlider*            filt1_res;
    AToggleButton*      filt1_LP;
    AToggleButton*      filt1_HP;
    AToggleButton*      filt1_BP;
    AToggleButton*      filt1_x2;
    AToggleButton*      filt1_osc1;
    AToggleButton*      filt1_osc2;
    AToggleButton*      filt1_osc3;

    AGroupComponent*    filt2_group;
    ASlider*            filt2_freq;
    ASlider*            filt2_res;
    AToggleButton*      filt2_LP;
    AToggleButton*      filt2_HP;
    AToggleButton*      filt2_BP;
    AToggleButton*      filt2_x2;
    AToggleButton*      filt2_osc1;
    AToggleButton*      filt2_osc2;
    AToggleButton*      filt2_osc3;

    EnvelopeComponent*  env1;
    EnvelopeComponent*  env2;
    EnvelopeComponent*  env3;
    EnvelopeComponent*  env4;
    EnvelopeComponent*  env5;
    EnvelopeComponent*  PitchVol;

    AToggleButton*      envVol1_1;
    AToggleButton*      envVol1_2;
    AToggleButton*      envVol1_3;
    AToggleButton*      envVol1_4;
    AToggleButton*      envVol1_5;

    AToggleButton*      envVol2_1;
    AToggleButton*      envVol2_2;
    AToggleButton*      envVol2_3;
    AToggleButton*      envVol2_4;
    AToggleButton*      envVol2_5;

    AToggleButton*      envVol3_1;
    AToggleButton*      envVol3_2;
    AToggleButton*      envVol3_3;
    AToggleButton*      envVol3_4;
    AToggleButton*      envVol3_5;

    AToggleButton*      envflt1_1;
    AToggleButton*      envflt1_2;
    AToggleButton*      envflt1_3;
    AToggleButton*      envflt1_4;
    AToggleButton*      envflt1_5;

    AToggleButton*      envflt2_1;
    AToggleButton*      envflt2_2;
    AToggleButton*      envflt2_3;
    AToggleButton*      envflt2_4;
    AToggleButton*      envflt2_5;

    ASlider*            lfo_vol1Amp;
    ASlider*            lfo_vol1Rate;
    ASlider*            lfo_vol2Amp;
    ASlider*            lfo_vol2Rate;
    ASlider*            lfo_vol3Amp;
    ASlider*            lfo_vol3Rate;

    ASlider*            lfo_pitch1Amp;
    ASlider*            lfo_pitch1Rate;
    ASlider*            lfo_pitch2Amp;
    ASlider*            lfo_pitch2Rate;
    ASlider*            lfo_pitch3Amp;
    ASlider*            lfo_pitch3Rate;

    ASlider*            lfo_flt1freqLevel;
    ASlider*            lfo_flt1freqRate;
    ASlider*            lfo_flt2freqLevel;
    ASlider*            lfo_flt2freqRate;

    ASlider*            fm_osc12;
    ASlider*            fm_osc23;
    ASlider*            fm_osc13;

    ASlider*            rm_osc12;
    ASlider*            rm_osc23;
    ASlider*            rm_osc13;

    AToggleButton*      chorus_on;
    AToggleButton*      delay_on;
    AToggleButton*      reverb_on;

    ASlider*            delay_level;
    ASlider*            delay_delay;
    ASlider*            delay_feedback;
    ASlider*            delay_lowcut;

    ASlider*            chorus_drywet;
    ASlider*            chorus_depth;
    ASlider*            chorus_delay;

    ASlider*            reverb_drywet;
    ASlider*            reverb_decay;
    ASlider*            reverb_lowcut;

    ATextButton*        bt_reset;

    ASlider*            envscale;


    SynthComponent(Synth* syn);
    virtual ~SynthComponent();
    void buttonClicked(Button* button);
    void sliderValueChanged(ASlider* slider);
    void resized();
    void PlaceOsc1(int x, int y);
    void PlaceOsc2(int x, int y);
    void PlaceOsc3(int x, int y);
    void PlaceFlt1(int x, int y);
    void PlaceFlt2(int x, int y);
    void PlaceLFOVol1(int x, int y);
    void PlaceLFOVol2(int x, int y);
    void PlaceLFOVol3(int x, int y);
    void PlaceLFOPitch1(int x, int y);
    void PlaceLFOPitch2(int x, int y);
    void PlaceLFOPitch3(int x, int y);
    void PlaceLFOFlt1Freq(int x, int y);
    void PlaceLFOFlt2Freq(int x, int y);
    void PlaceFMRM(int x, int y);
    void PlaceStuff(int x, int y);
    void PlaceDelay(int x, int y);
    void PlaceChorus(int x, int y);
    void PlaceReverb(int x, int y);
    void PlaceEnv1Butt(int x, int y);
    void PlaceEnv2Butt(int x, int y);
    void PlaceEnv3Butt(int x, int y);
    void PlaceEnv4Butt(int x, int y);
    void PlaceEnv5Butt(int x, int y);
    void UpdateEnvButts();
    virtual ASlider* PlaceSliderWithLabel(char* txt, Parameter* param, int x, int y, int w = 83, int h = 22);
    void paint(Graphics &g);
};

class SynthWindow  : public PluginCommonWindow
{
public:

    SynthWindow(Synth* syn);
    ~SynthWindow();
};

class VSTParamComponent : public Component,
                                  public ButtonListener,
                                  public ASliderListener
{
public:
    Paramcell*  first_pcell;
    Paramcell*  last_pcell;
    Scope*      scope;

    Eff*        pEff;

    VSTParamComponent(VSTEffect* vsteff, Scope* sc);
    ~VSTParamComponent() {};
    void AddParamcell(Paramcell* pcell);
    void RemoveParamcell(Paramcell* pcell);
    void buttonClicked(Button* button) {};
    void sliderValueChanged(ASlider* slider);
};

class SampleWindow  : public ChildWindow
{
public:
    Sample*     sample;

    SampleWindow();
    ~SampleWindow();
    void SetSample(Sample* smp);
    void moved();
    void closeButtonPressed();
};

class Looper : public Component
{
public:
    bool    leftside;
    ComponentDragger dragger;
    SampleWave* wave;

    Looper(bool lside, SampleWave* wave);
    void setWave(SampleWave * wv);
    void mouseDrag(const MouseEvent &);
    void mouseDown(const MouseEvent &);
    void mouseEnter(const MouseEvent &);
    void mouseExit(const MouseEvent &);
    void paint(Graphics& g);
};

class SampleWave : public Component
{
public:
    Sample* sample;
    bool    looped;
    long    loopstart;
    long    loopend;
    Looper* left;
    Looper* right;
    double  wratio;
    double  wratio1;
    ComponentBoundsConstrainer*  c1;
    ComponentBoundsConstrainer*  c2;

    SampleWave(Sample* smp);
    void SetSample(Sample* smp);
    void SetLoopPoints(long start, long end);
    void ConstrainLooper(Looper* looper);
    void Loopers();
    void mouseMove(const MouseEvent &);
    void mouseExit(const MouseEvent &);
    void mouseDrag(const MouseEvent &);
    void mouseDown(const MouseEvent &);
    void mouseUp(const MouseEvent &);
    void paint(Graphics& g);
    void resized();
};

class SampleComponent : public CComponent
{
public:
    Sample*     sample;
    SampleWave* wave;
    AToggleButton*  tgnorm;
    AToggleButton*  tgNoLoop;
    AToggleButton*  tgFwdLoop;
    AToggleButton*  tgPPongLoop;
    AToggleButton*  tgSustain;
    //Button*     butt_normalize;
    AToggleButton*  tgdelayON;
    ASlider*        delayDryWet;
    ASlider*        delayDelay;
    ASlider*        delayFeedback;
    ASlider*        delayHCut;
    ASlider*        delayLCut;
    EnvelopeComponent*  env;
    ASlider*        slTime;
    ATextButton*    btAlign;

    SampleComponent();
    void    buttonClicked(Button* buttonThatWasClicked);
	void    sliderValueChanged(ASlider* slider);
    void    SetSample(Sample* smp);
    void    resized();
    void    PlaceDelay(int x, int y);
private:

};

class ConfigWindow  : public ChildWindow
{
    public:
    //==============================================================================
    ConfigWindow();
    ~ConfigWindow();
};

class RenderWindow  : public ChildWindow
{
    public:
    //==============================================================================
    RenderWindow();
    void ResetToStart();
    RenderComponent* RC;
    ~RenderWindow();
};
class VSTPluginComponent;

class PluginEditWindow : public PluginCommonWindow
{
	public:
		PluginEditWindow(void * ParentWindow, CVSTPlugin * pPlug);
		~PluginEditWindow();
		void ShowWindow();
		void UpdateWindow() {};
		void SetFocus();
		void Close();
		void SetTitle();
		void* ParentHandle;
		CVSTPlugin* pPlugin;
		void PluginEditWindow::closeButtonPressed();
        int getBorderSize() const
        {
            return 2;
        }
};

#endif
