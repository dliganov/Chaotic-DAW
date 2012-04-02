#ifndef EFFECTS_H
#define EFFECTS_H

#include "rosic/rosic.h"
#include "awful.h"
#include "awful_audio.h"
#include "awful_jucewindows.h"
#include "Awful_objects.h"
#include "VSTCollection.h"
#include "Awful_params.h"
#include "awful_instruments.h"
#include "awful_parammodule.h"

const int defaultMaxSizeMB = 64;
const int fxbVersionNum = 1;


// copied from Juce for working with VST programs
struct fxProgram
{
    long chunkMagic;        // 'CcnK'
    long byteSize;          // of this chunk, excl. magic + byteSize
    long fxMagic;           // 'FxCk'
    long version;
    long fxID;              // fx unique id
    long fxVersion;
    long numParams;
    char prgName[28];
    float params[1];        // variable no. of parameters
};

struct fxSet
{
    long chunkMagic;        // 'CcnK'
    long byteSize;          // of this chunk, excl. magic + byteSize
    long fxMagic;           // 'FxBk'
    long version;
    long fxID;              // fx unique id
    long fxVersion;
    long numPrograms;
    char future[128];
    fxProgram programs[1];  // variable no. of programs
};

struct fxChunkSet
{
    long chunkMagic;        // 'CcnK'
    long byteSize;          // of this chunk, excl. magic + byteSize
    long fxMagic;           // 'FxCh', 'FPCh', or 'FBCh'
    long version;
    long fxID;              // fx unique id
    long fxVersion;
    long numPrograms;
    char future[128];
    long chunkSize;
    char chunk[8];          // variable
};

struct fxProgramSet
{
    long chunkMagic;        // 'CcnK'
    long byteSize;          // of this chunk, excl. magic + byteSize
    long fxMagic;           // 'FxCh', 'FPCh', or 'FBCh'
    long version;
    long fxID;              // fx unique id
    long fxVersion;
    long numPrograms;
    char name[28];
    long chunkSize;
    char chunk[8];          // variable
};

class Eff : public ParamModule
{
public:
    Eff(Mixcell* mc);
    virtual ~Eff();

    bool        folded;
    Toggle*     fold_toggle;

    bool        bypass;
    Toggle*     bypass_toggle;
    bool        off;
    int         muteCount;

    bool        wndvisible;
    Toggle*     wnd_toggle;

    ModuleSubType     type;
    Mixcell*    mixcell;

    AliasRecord*    arec;

    ModuleType category;
    Eff*        prev;
    Eff*        next;
    Eff*        cprev;
    Eff*        cnext;

    virtual void    Process(float* in_buff, float* out_buff, int num_frames);
    virtual void    ProcessData(float* in_buff, float* out_buff, int num_frames) {};
    virtual Eff*    Clone(Mixcell* mc) { return NULL; };
    virtual void    Disable();
    virtual void    Enable();
    virtual void    QueueDeletion();
    virtual int     GetHeight();
    virtual void    Reset() { }
            void    DereferenceElements();
    virtual void    SetNewMixcell(Mixcell* ncell);
    virtual void    SetBPM(float bpm) {};
    virtual void    SetBufferSize(unsigned int uiBufferSize) {};
    virtual void    SetSampleRate(float fSampleRate) {};
    virtual void    Save(XmlElement* xmlEff);
    virtual void    Load(XmlElement* xmlEff);
};

class VSTEffect : public Eff
{
public:
    Butt*   pEditButton;
    CVSTPlugin* pPlug;

    VSTParamWindow* VSTParamWnd;

    VSTEffect(Mixcell* mc, AliasRecord* ar, char* path = NULL);
    virtual ~VSTEffect();
    VSTEffect* Clone(Mixcell* mc);
    void    ProcessData(float* in_buff, float* out_buff, int num_frames);
    void    ProcessEvents(VstEvents *pEvents);
    long    GetNumPresets();
    void    GetPresetName(long index, char *ppName);
    long    GetPresetIndex(char* name);
    bool    SetPresetByName(char* name);
    bool    SetPresetByIndex(long index);
    void    RenamePreset(long index, char* new_name);
    void    RenamePreset(char* old_name, char* new_name);
//    void    SavePresetAs(char *preset_name);
    long    GetNumStoredPresets(){ return 0; };
    long    GetNumNativePresets(){ return 0; };
    long    GetCurrentPreset();
    void    SetPresetName(char *new_name);
    Preset* GetPreset(long index);
    Preset* GetPreset(char* name);
    bool    SetUserPreset(Preset* pPreset);

    void    AddParameters();
    void    UpdatePresets();
    void    ParamUpdate(Parameter* param = NULL);
    void    UpdateVString(Parameter* param);
    bool    OnSetParameterAutomated(int nEffect, long index, float value);
    bool    OnUpdateDisplay();
    void    Reset();
    void    SetBPM(float bpm);
    void    SetBufferSize(unsigned int uiBufferSize);
    void    SetSampleRate(float fSampleRate);
    void    ShowEditor(bool show);

    const String GetCurrentPresetName();
    void    setChunkData(const char* data, int size, bool isPreset);
    void    getChunkData(MemoryBlock& mb, bool isPreset, int maxSizeMB) const;
    void    SetStateInformation (const void* data, int sizeInBytes);
    void    GetStateInformation (MemoryBlock& destData);
    bool    saveToFXBFile (MemoryBlock& dest, bool isFXB, int maxSizeMB);
    bool    loadFromFXBFile (const void* const data, const int dataSize);
    void    setParamsInProgramBlock (fxProgram* const prog);
    void    restoreFromTempParameterStore (const MemoryBlock& m);
    void    createTempParameterStore (MemoryBlock& dest);
    bool    restoreProgramSettings (const fxProgram* const prog);
    void    updateStoredProgramNames();
    void    ToggleParamWindow();

    void    Save(XmlElement* xmlEff);
    void    Load(XmlElement* xmlEff);


private:
    bool    bLoading;
    void   *m_WndHandle;
};

class BlankEffect : public Eff
{
public:
    Parameter*      level;
    ValueString*    vstr_level;

    Parameter*      level1;
    ValueString*    vstr_level1;

    Parameter*      level2;
    ValueString*    vstr_level2;

    BoolParam*      d1;
    ValueString*    d1_state;
    BoolParam*      d2;
    ValueString*    d2_state;

    Paramcell*      radiocell;
    BoolParam*      r1;
    ValueString*    r1_state;
    BoolParam*      r2;
    ValueString*    r2_state;
    BoolParam*      r3;
    ValueString*    r3_state;
    BoolParam*      r4;
    ValueString*    r4_state;

    BlankEffect(Mixcell* mc, AliasRecord* ar);
    virtual ~BlankEffect();
    void ProcessData(float* in_buff, float* out_buff, int num_frames);
    void ParamUpdate(Parameter* param = NULL);
    BlankEffect* Clone(Mixcell* mc);
};

class Gain : public Eff
{
public:
    Parameter* level;
    Knob* r_level;
    ValueString* v_level;

    Gain(Mixcell* mc, AliasRecord* ar);
    virtual ~Gain();
    void ProcessData(float* in_buff, float* out_buff, int num_frames);
    void ParamUpdate(Parameter* param = NULL);
    Gain* Clone(Mixcell* mc);
};

class Filter : public Eff
{
public:
    ValueString*    v_cutoff;
    ValueString*    v_resonance;

    // Filter stuff
    Parameter*  cutoff;
    Parameter*  resonance;
    float   filtCoefTab[5];
    float   lx1, lx2, ly1, ly2; // Left sample history
    float   rx1, rx2, ry1, ry2; // Right sample history

    Filter(Mixcell* mc, AliasRecord* ar);
    void ProcessData(float* in_buff, float* out_buff, int num_frames);
    void Reset();
    void ParamUpdate(Parameter* param = NULL);
    Filter* Clone(Mixcell* mc);
};

class CFilter3 : public Eff
{
public:
    Parameter*      cutoff;
    Parameter*      resonance;
    Parameter*      bandwidth;

    BoolParam*      x2;

    Paramcell*      ftype;
    BoolParam*      f1;
    ValueString*    f1_state;
    BoolParam*      f2;
    ValueString*    f2_state;
    BoolParam*      f3;
    ValueString*    f3_state;

    bool            f_master;
    CFilter3*       f_next;

    CFilter3(Mixcell* mc, AliasRecord* ar);
    void ProcessData(float* in_buff, float* out_buff, int num_frames);
    void Reset();
    void ParamUpdate(Parameter* param = NULL);
    CFilter3* Clone(Mixcell* mc);

    rosic::LadderFilter dspCoreCFilter3;
};

class CFilter2 : public Eff
{
public:
    Parameter*      cutoff;
    Parameter*      resonance;
    Parameter*      bandwidth;
    Parameter*      Q;

    BoolParam*      x2;

    Paramcell*      ftype;
    BoolParam*      f1;
    ValueString*    f1_state;
    BoolParam*      f2;
    ValueString*    f2_state;
    BoolParam*      f3;
    ValueString*    f3_state;

    CFilter2(Mixcell* mc, AliasRecord* ar);
    void ProcessData(float* in_buff, float* out_buff, int num_frames);
    void Reset();
    void ParamUpdate(Parameter* param = NULL);
    CFilter2* Clone(Mixcell* mc);

    rosic::FourPoleFilter dspCoreCFilter2;
};

class CFilter : public Eff
{
public:
    ValueString*    v_cutoff;
    ValueString*    v_resonance;
    ValueString*    v_bandwidth;

    Parameter*      cutoff;
    Parameter*      resonance;
    Parameter*      bandwidth;
    Parameter*      Q;
    Paramcell*      bandpcell;
    Paramcell*      respcell;
    Paramcell*      cutpcell;

    Paramcell*      ftype;
    BoolParam*      f1;
    ValueString*    f1_state;
    BoolParam*      f2;
    ValueString*    f2_state;
    BoolParam*      f3;
    ValueString*    f3_state;

    bool            f_master;
    CFilter*        f_next;

    CFilter(Mixcell* mc, AliasRecord* ar);
    void ProcessData(float* in_buff, float* out_buff, int num_frames);
    void Reset();
    void ParamUpdate(Parameter* param = NULL);
    CFilter* Clone(Mixcell* mc);
    void InsertBandwidth();
    void RemoveBandwidth();

    rosic::TwoPoleFilter dspCoreCFilter;
};

class CChorus : public Eff
{
public:
    ValueString*    v_delay;
    ValueString*    v_freq;
    ValueString*    v_depth;
    ValueString*    v_drywet;

    Parameter*      delay;
    Parameter*      freq;
    Parameter*      depth;
    Parameter*      drywet;

    Paramcell*      ftype;
    BoolParam*      f1;
    ValueString*    f1_state;
    BoolParam*      f2;
    ValueString*    f2_state;
    BoolParam*      f3;
    ValueString*    f3_state;

    CChorus(Mixcell* mc, AliasRecord* ar);
    void ProcessData(float* in_buff, float* out_buff, int num_frames);
    void Reset();
    void ParamUpdate(Parameter* param = NULL);
    CChorus* Clone(Mixcell* mc);

    rosic::Chorus* dspCoreChorus;
};

class CFlanger : public Eff
{
public:
    ValueString*    v_freq;
    ValueString*    v_modfreq;
    ValueString*    v_depth;
    ValueString*    v_drywet;
    ValueString*    v_feedback;

    Parameter*      frequency;
    Parameter*      feedback;
    Parameter*      modfreq;
    Parameter*      depth;
    Parameter*      drywet;

    BoolParam*      invert;
    ValueString*    v_invert;

    float*          fmemory;

    CFlanger(Mixcell* mc, AliasRecord* ar);
    void ProcessData(float* in_buff, float* out_buff, int num_frames);
    void Reset();
    void ParamUpdate(Parameter* param = NULL);
    CFlanger* Clone(Mixcell* mc);

    rosic::Flanger dspCoreFlanger;
};

class CPhaser : public Eff
{
public:
    ValueString*    v_freq;
    ValueString*    v_modfreq;
    ValueString*    v_depth;
    ValueString*    v_drywet;
    ValueString*    v_feedback;
    ValueString*    v_numstages;
    ValueString*    v_stereo;

    Parameter*      frequency;
    Parameter*      feedback;
    Parameter*      modfreq;
    Parameter*      depth;
    Parameter*      drywet;
    Parameter*      numstages;
    Parameter*      stereo;

    float*          fmemory;

    CPhaser(Mixcell* mc, AliasRecord* ar);
    void ProcessData(float* in_buff, float* out_buff, int num_frames);
    void Reset();
    void ParamUpdate(Parameter* param = NULL);
    CPhaser* Clone(Mixcell* mc);

    rosic::Phaser dspCorePhaser;
};

class EQ1 : public Eff
{
public:
    ValueString *v_frequency, *v_gain, *v_bandwidth;

    FrequencyParameter *frequency;	
	Parameter *gain, *bandwidth;

    EQ1(Mixcell* mc, AliasRecord* ar);
    void ProcessData(float* in_buff, float* out_buff, int num_frames);
    void Reset();
    void ParamUpdate(Parameter* param = NULL);
    EQ1* Clone(Mixcell* mc);

	rosic::TwoPoleFilter dspCoreEq1;
};

class GraphicEQ : public Eff
{
public:
    ValueString *v_gain1, *v_gain2, *v_gain3, *v_gain4, *v_gain5, *v_gain6, *v_gain7, *v_gain8, *v_gain9;
    Parameter *gain1, *gain2, *gain3, *gain4, *gain5, *gain6, *gain7, *gain8, *gain9;
    int f1, f2, f3, f4, f5, f6, f7, f8, f9;

    GraphicEQ(Mixcell* mc, AliasRecord* ar);
    void ProcessData(float* in_buff, float* out_buff, int num_frames);
    void Reset();
    void ParamUpdate(Parameter* param = NULL);
    GraphicEQ* Clone(Mixcell* mc);

	rosic::Equalizer dspCoreEqualizer;
};

class EQ3 : public Eff
{
public:
    ValueString *v_gain1, *v_gain2, *v_gain3, *v_freq1, *v_freq2, *v_freq3, *v_bandwidth;
    Parameter *gain1, *gain2, *gain3, *bandwidth;
    FrequencyParameter *freq1, *freq2, *freq3;

    int f1, f2, f3;

    EQ3(Mixcell* mc, AliasRecord* ar);
    void ProcessData(float* in_buff, float* out_buff, int num_frames);
    void Reset();
    void ParamUpdate(Parameter* param = NULL);
    EQ3* Clone(Mixcell* mc);

	rosic::Equalizer dspCoreEqualizer;
};

class XDelay : public Eff
{
public:
    ValueString *v_delay, *v_drywet, *v_feedback, *v_highCut, *v_lowCut, *v_pan;

    // user parameters:
    FrequencyParameter *highCut, *lowCut;
    Parameter *delay, *drywet, *ggain, *feedback, *pan;

    BoolParam*      ppmode;

    XDelay(Mixcell* mc, AliasRecord* ar);
    void ProcessData(float* in_buff, float* out_buff, int num_frames);
    void Reset();
    void ParamUpdate(Parameter* param = NULL);
    XDelay* Clone(Mixcell* mc);

	rosic::PingPongEcho dspCorePingPongDelay;
};

class CReverb : public Eff
{
public:

    // user parameters:
    FrequencyParameter *highCut, *lowCut;
    Parameter *roomsize, *preDelay, *drywet, *decay;
    LogParam *lowscale, *highscale;

    CReverb(Mixcell* mc, AliasRecord* ar);
    void ProcessData(float* in_buff, float* out_buff, int num_frames);
    void Reset();
    void ParamUpdate(Parameter* param = NULL);
    CReverb* Clone(Mixcell* mc);

	rosic::Reverb dspCoreReverb;
};

class CTremolo : public Eff
{
public:
    ValueString* v_speed;
    ValueString* v_depth;
    Parameter*  speed;
    Parameter*  depth;

    CTremolo(Mixcell* mc, AliasRecord* ar);
    void ProcessData(float* in_buff, float* out_buff, int num_frames);
    void Reset();
    void ParamUpdate(Parameter* param = NULL);
    CTremolo* Clone(Mixcell* mc);

	rosic::Tremolo dspCoreCTremolo;
};

class Compressor : public Eff
{
public:
    ValueString *v_threshold, *v_ratio, *v_attack, *v_release, *v_gain;
    Parameter *threshold, *knee, *ratio, *attack, *release, *gain;

    BoolParam*      mode;
    ValueString*    mode_state;
    BoolParam*      autogain;
    ValueString*    autogain_state;

    Compressor(Mixcell* mc, AliasRecord* ar);
    void ProcessData(float* in_buff, float* out_buff, int num_frames);
    void Reset();
    void ParamUpdate(Parameter* param = NULL);
    Compressor* Clone(Mixcell* mc);

	rosic::Compressor dspCoreComp;
};

class CWahWah : public Eff
{
public:
    Parameter*      frequency;
    Parameter*      modfreq;
    Parameter*      depth;
    Parameter*      drywet;
    Parameter*      bandwidth;

    CWahWah(Mixcell* mc, AliasRecord* ar);
    void ProcessData(float* in_buff, float* out_buff, int num_frames);
    void Reset();
    void ParamUpdate(Parameter* param = NULL);
    CWahWah* Clone(Mixcell* mc);

    rosic::WahWah dspCoreWah;
};

class CDistort : public Eff
{
public:
    Parameter*      drive;
    Parameter*      postgain;
    Parameter*      slope;

    CDistort(Mixcell* mc, AliasRecord* ar);
    void ProcessData(float* in_buff, float* out_buff, int num_frames);
    void Reset();
    void ParamUpdate(Parameter* param = NULL);
    CDistort* Clone(Mixcell* mc);

    rosic::WaveShaper dspCoreDist;
};

class CBitCrusher : public Eff
{
public:
    Parameter*      decimation;
    Parameter*      quantization;

    CBitCrusher(Mixcell* mc, AliasRecord* ar);
    void ProcessData(float* in_buff, float* out_buff, int num_frames);
    void Reset();
    void ParamUpdate(Parameter* param = NULL);
    CBitCrusher* Clone(Mixcell* mc);

    rosic::BitCrusher dspCoreBC;
};

class CStereo : public Eff
{
public:
    Parameter*      offset;

    CStereo(Mixcell* mc, AliasRecord* ar);
    void ProcessData(float* in_buff, float* out_buff, int num_frames);
    void Reset();
    void ParamUpdate(Parameter* param = NULL);
    CStereo* Clone(Mixcell* mc);

    rosic::CombStereoizer dspCoreStereo;
};

class CPluginList : public Object
{
public:
    CPluginList(Object* owner);
    virtual ~CPluginList();
    ModListEntry*   pFirst;
    ModListEntry*   pLast;
    unsigned long     num_items;
    void              AddEntry(ModListEntry* pEntry);
    void              AddEntry(ModuleType mtype, ModuleSubType mstype, const char* name, const char* path);
    void              RemoveEntry(ModListEntry* pEntry);
    ModListEntry*   SearchEntryByPath(const char* path);
};

#endif
