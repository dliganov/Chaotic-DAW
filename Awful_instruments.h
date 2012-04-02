#ifndef INSTRUMENTS_H
#define INSTRUMENTS_H

#include "awful.h"
#include "awful_objects.h"
#include "awful_controls.h"
#include "awful_effects.h"
#include "aeffectx.h"
#include "rosic/rosic.h"
#include "awful_parammodule.h"

#define BUFF_CHUNK_SIZE                  (64)

extern bool                 SwitchCurrentInstrumentWindowOFFIfNeeded(Instrument* newinstr);
extern void                 ShowInstrumentWindow(Instrument* i);
extern Instrument*          GetInstrumentByIndex(int index);
extern void                 UpdateInstrumentIndex();
extern void                 UpdateInstrumentIndices();
extern void                 AddInstrument(Instrument* i);
extern void                 LoadMetronomeSamples();
extern Sample*              AddSample(const char* path, const char* name, const char* alias, bool temp = false);
extern CGenerator*          AddInternalGenerator(const char* path, const char* alias);
extern VSTGenerator*        AddVSTGenerator(const char* path, const char* name, const char* alias);
extern VSTGenerator*        AddVSTGenerator(VSTGenerator* vst, char* alias);


typedef enum LoopType
{
    LoopType_NoLoop,
    LoopType_ForwardLoop,
    LoopType_PingPongLoop
}LoopType;

class Instrument: public ParamModule
{
public:
    InstrType   type;

    // Basic parameters set (volume, panning, etc)
    ParamSet*   params;

    // GUI button
    ButtFix*    pEditorButton;

    // Basic parameters controls
    SliderHVol* mvol;
    SliderHPan* mpan;
    Toggle*     mute;
    Toggle*     solo;

    // FX two-digit string
    DigitStr*   dfxstr;

    // Text alias
    TString*    alias;
    Image*      alias_image;

    // VU-meters
    VU*         vu;
    VU*         stepvu;

    // Target mix-channel
    MixChannel* fx_channel;
    Trk*        ptrk;

    // Auto-pattern
    Pattern*    autoPatt;

    // Note instance for instrument preview (pre-listen)
    Instance*   prevInst;

    DrawArea*   instr_drawarea;

    float       data_buff[MAX_BUFF_SIZE*2];     // Initial data
    float       in_buff[MAX_BUFF_SIZE*2];       // Data after separate DSP
    float       out_buff[MAX_BUFF_SIZE*2];      // Output after postprocessing

    // Volume and panning envelopes (common for the whole instrument)
    Envelope*   envVol;
    Envelope*   envPan;

    // Remember last parameters for the notes of this instrument
    float       last_note_length;
    float       last_vol;
    float       last_pan;

    // Queues
    // Note triggers queue
    Trigger*    tg_first;
    Trigger*    tg_last;

    // List of all instruments
    Instrument* prev;
    Instrument* next;

    // List of all instruments queued to be deleted
    Instrument* del_prev;
    Instrument* del_next;

    bool        dereferenced;

    int         rampCount;

    // Helpers for data generation
    Instance*   ii;
    Envelope*   venv1;
    Envelope*   venv2;
    Envelope*   venv3;

    Envelope*   penv1;
    Envelope*   penv2;
    Envelope*   penv3;

    MixChannel* mchan;
    float       volbase;    // Base environmental volume
    long        venvphase;
    long        end_frame;  // last frame to fill
    float       pan0, pan1, pan2, pan3, pan4, pan5;
    bool        fill;
    bool        skip;
    float       rampCounterV;
    float       cfsV;
    float       rampCounterP;
    float       cfsP;
    float       locPan;

    Instrument();
    virtual ~Instrument();
    void QueueDeletion();
    virtual void Reset();
    void ForceDeactivate();
    void Update();
    virtual Instrument* Clone();
    void ParamEditUpdate(PEdit* pe);
    void SetAlias(char* al);
    virtual void CreateAutoPattern();
    void DereferenceBoundPattern(Pattern* pt);
    bool DereferenceElements();
    virtual void Disable();
    virtual void Enable();
    virtual void ActivateTrigger(Trigger* tg);
    virtual void DeactivateTrigger(Trigger* tg);
    virtual void GenerateData(long num_frames = 0, long buffframe = 0);
    virtual void StaticInit(Trigger* tg, long num_frames);
    virtual void PreProcessTrigger(Trigger* tg, bool* skip, bool* fill, long num_frames, long buffframe = 0);
    virtual long ProcessTrigger(Trigger* tg, long num_frames = 0, long buffframe = 0) {return 0;};
    virtual long WorkTrigger(Trigger* tg, long num_frames, long remaining, long buffframe, long mixbuffframe);
    virtual void PostProcessTrigger(Trigger* tg, long num_frames = 0, long buffframe = 0, long mixbuffframe = 0, long remaining = 0);
    virtual void ApplyDSP(Trigger* tg, long buffframe, long num_frames = 0);
    virtual void FillMixChannel(long num_frames, long buffframe, long mixbuffframe);
    virtual void DeClick(Trigger* tg, long num_frames = 0, long buffframe = 0, long mixbuffframe = 0, long remaining = 0);
    virtual void Mute(Trigger* tg) {};
    void    SetLastLength(float lastlength);
    void    SetLastVol(float lastvol);
    void    SetLastPan(float lastpan);
    void    FlowTriggers(Trigger* tgfrom, Trigger* tgto);
    virtual void Save(XmlElement* instrNode);
    virtual void Load(XmlElement* instrNode);
    virtual void CopyDataToClonedInstrument(Instrument* instr);
};

class Sample : public Instrument
{
public:

    // Basic sample data
    SF_INFO     sample_info;
    float*      sample_data;
    float       timelen;

    float       rateUp;
    float       rateDown;

    // Normalizing related
    bool        normalized;
    float       norm_factor;

    // Looping related
    LoopType    looptype;
    long        lp_start;
    long        lp_end;

    bool        touchresized;

    // Additional effects and parameters
    BoolParam*  delayOn;
    XDelay*     delay;

    Sample(float* data, char* pth, char* nm, SF_INFO sfinfo);
   virtual ~Sample();
    void ActivateTrigger(Trigger * tg);
    long ProcessTrigger(Trigger* tg, long num_frames = 0, long buffframe = 0);
    virtual bool CheckBounds(Samplent* samplent, Trigger* tg, long num_frames);
    void UpdateNormalizeFactor();
    void ToggleNormalize();
    void ApplyDSP(Trigger* tg, long buffframe, long num_frames);
    void SetLoopEnd(long end);
    void SetLoopStart(long start);
    void SetLoopPoints(long start, long end);
    void Save(XmlElement* instrNode);
    void Load(XmlElement* instrNode);
    void CopyDataToClonedInstrument(Instrument * instr);
    void DumpData();
};

class CGenerator : public Instrument
{
public:

    CGenerator();
    virtual ~CGenerator();
    void ActivateTrigger(Trigger* tg);
    virtual void CheckBounds(Gennote* gnote, Trigger* tg, long num_frames);
};


class SineNoise : public CGenerator
{
public:

    SineNoise();
    virtual ~SineNoise();
};


/*
class SoundFont : public CGenerator
{
public:

    sfBankID    bank_id;
    char        sf_file_path[MAX_PATH_STRING];
    char        sf_name[MAX_NAME_STRING];

    SoundFont(sfBankID bankid);
};
*/

class VSTGenerator: public CGenerator
{
public:
    VSTEffect* pEff;
    VSTGenerator(char* fullpath, char* alias);
    VSTGenerator(VSTGenerator* vst, char* alias);
    virtual ~VSTGenerator();
    void GenerateData(long num_frames = 0, long mixbuffframe = 0);
    void VSTProcess(long num_frames, long buffframe);
    void AddNoteEvent(int note, long num_frames, long frame_phase, long total_frames, float volume);
    void PostNoteON(int note, float vol);
    void PostNoteOFF(int note, int velocity);
    void StopAllNotes();
    void PostProcessTrigger(Trigger* tg = NULL, long num_frames = 0, long buffframe = 0, long mixbuffframe = 0, long remaining = 0);
    void DeactivateTrigger(Trigger* tg);
    long ProcessTrigger(Trigger* tg, long num_frames = 0, long buffframe = 0);
    void CheckBounds(Gennote* gnote, Trigger* tg, long num_frames);
    void FlowTriggers(Trigger* tgfrom, Trigger* tgto) {}; // stub to avoid any action here
    void Save(XmlElement* instrNode);
    void Load(XmlElement* instrNode);
    void ToggleParamWindow();
    void Reset();

private:
    VstMidiEvent    MidiEvents[800];
    long            NumMidiEvents;
    int             muteCount;
};

class Osc
{
public:
    float   phase_shift;
    float   finetune;
    int     note_shift;

    Osc();
    virtual void GetData(double wt_pos, float freq, long frame_phase, float* od1, float* od2) {}
};

class Saw : public Osc
{
public:
    void GetData(double wt_pos, float freq, long frame_phase, float* od1, float* od2);
};

class Sine : public Osc
{
public:
    void GetData(double wt_pos, float freq, long frame_phase, float* od1, float* od2);
};

class Triangle : public Osc
{
public:
    void GetData(double wt_pos, float freq, long frame_phase, float* od1, float* od2);
};

class Synth : public CGenerator
{
    typedef struct sVoice
    {
        double		sec_phase1;
        double		sec_phase2;
        double		sec_phase3;
        float       envVal1;
        float       envVal2;
        float       envVal3;
        float       envVal4;
        float       envVal5;
        EnvPnt*     ep1;
        EnvPnt*     ep2;
        EnvPnt*     ep3;
        EnvPnt*     ep4;
        EnvPnt*     ep5;
        double		env_phase1;
        double		env_phase2;
        double		env_phase3;
        double		env_phase4;
        double		env_phase5;
    };

public:
    int         numUsedVoices;
    int         maxVoices;
    int         freeVoice;

    Parameter*  osc1Level;
    Parameter*  osc1Octave;
    Parameter*  osc1Detune;
    Parameter*  osc1Width;
    BoolParam*  osc1Sine;
    BoolParam*  osc1Saw;
    BoolParam*  osc1Tri;
    BoolParam*  osc1Pulse;
    BoolParam*  osc1Noise;
    float       osc1fmulStatic;
    float       osc1fmulLFO;
    float       osc1fmulFM;

    Parameter*  osc2Level;
    Parameter*  osc2Octave;
    Parameter*  osc2Detune;
    Parameter*  osc2Width;
    BoolParam*  osc2Sine;
    BoolParam*  osc2Saw;
    BoolParam*  osc2Tri;
    BoolParam*  osc2Pulse;
    BoolParam*  osc2Noise;
    float       osc2fmulStatic;
    float       osc2fmulLFO;
    float       osc2fmulFM;

    Parameter*  osc3Level;
    Parameter*  osc3Octave;
    Parameter*  osc3Detune;
    Parameter*  osc3Width;
    BoolParam*  osc3Sine;
    BoolParam*  osc3Saw;
    BoolParam*  osc3Tri;
    BoolParam*  osc3Pulse;
    BoolParam*  osc3Noise;
    float       osc3fmulStatic;
    float       osc3fmulLFO;
    float       osc3fmulFM;

    BoolParam*  osc1Fat;
    BoolParam*  osc2Fat;
    BoolParam*  osc3Fat;

    Parameter*  noiseLevel;

    Parameter*  osc1FM2Level;
    Parameter*  osc2FM3Level;
    Parameter*  osc1FM3Level;

    Parameter*  osc1RM2Level;
    Parameter*  osc2RM3Level;
    Parameter*  osc1RM3Level;

    Parameter*  LFOAmp_Vol1;
    Parameter*  LFOAmp_Vol2;
    Parameter*  LFOAmp_Vol3;
    Parameter*  LFOAmp_Pitch1;
    Parameter*  LFOAmp_Pitch2;
    Parameter*  LFOAmp_Pitch3;
    Parameter*  LFOAmp_Flt1Freq;
    Parameter*  LFOAmp_Flt2Freq;

    Parameter*  LFORate_Vol1;
    Parameter*  LFORate_Vol2;
    Parameter*  LFORate_Vol3;
    Parameter*  LFORate_Pitch1;
    Parameter*  LFORate_Pitch2;
    Parameter*  LFORate_Pitch3;
    Parameter*  LFORate_Flt1Freq;
    Parameter*  LFORate_Flt2Freq;

    CFilter3*   filt1[12];
    BoolParam*  filt1Osc1;
    BoolParam*  filt1Osc2;
    BoolParam*  filt1Osc3;

    CFilter3*   filt2[12];
    BoolParam*  filt2Osc1;
    BoolParam*  filt2Osc2;
    BoolParam*  filt2Osc3;

    CChorus*    chorus;
    XDelay*     delay;
    CReverb*    reverb;

    BoolParam*  chorusON;
    BoolParam*  delayON;
    BoolParam*  reverbON;

    sVoice      voice[12];
    float       data_buff2[MAX_BUFF_SIZE*2];
    float       data_buff3[MAX_BUFF_SIZE*2];

    SynthWindow*    synthWin;
    SynthComponent* SynthComp;

    float       fltbuff1[MAX_BUFF_SIZE*2];
    float       fltbuff2[MAX_BUFF_SIZE*2];
    float       fltbuff1_out[MAX_BUFF_SIZE*2];
    float       fltbuff2_out[MAX_BUFF_SIZE*2];

    float       fltlfo1[MAX_BUFF_SIZE];
    float       fltlfo2[MAX_BUFF_SIZE];

    // Buffers where parsed envelope values are stored
    float       ebuff1[MAX_BUFF_SIZE];
    float       ebuff2[MAX_BUFF_SIZE];
    float       ebuff3[MAX_BUFF_SIZE];
    float       ebuff4[MAX_BUFF_SIZE];
    float       ebuff5[MAX_BUFF_SIZE];

    float       fp;
    float       rate;

    float*      p_osc1envVal;
    float*      p_osc2envVal;
    float*      p_osc3envVal;
    float*      p_flt1envVal;
    float*      p_flt2envVal;

    Envelope*   env1;
    Envelope*   env2;
    Envelope*   env3;
    Envelope*   env4;
    Envelope*   env5;

    float       envV1;
    float       envV2;
    float       envV3;
    float       envV4;
    float       envV5;

    float       lenvV1;
    float       lenvV2;
    float       lenvV3;
    float       lenvV4;
    float       lenvV5;

    Trigger*    tgramp;

    Random*     rnd;

    Synth();
    virtual ~Synth();
    void ActivateTrigger(Trigger * tg);
    void DeactivateTrigger(Trigger* tg);
    long ProcessTrigger(Trigger* tg, long num_frames = 0, long buffframe = 0);
    void FillMixChannel(long num_frames, long buffframe, long mixbuffframe);
    void ApplyDSP(Trigger* tg, long buffframe, long num_frames);
    void UpdateOsc1Mul();
    void UpdateOsc2Mul();
    void UpdateOsc3Mul();
    void ParamUpdate(Parameter* param);
    void CheckBounds(Gennote* gnote, Trigger* tg, long num_frames);
    void SaveCustomStateData(XmlElement & xmlParentNode);
    void RestoreCustomStateData(XmlElement & xmlStateNode);
    int  MapEnvPointerToNum(float* eV);
    float* MapNumToEnvPointer(int num);
    float* MapNumToEBuffPointer(int num);
    void ResetValues();
    bool IsTriggerAtTheEnd(Trigger* tg);
    void Save(XmlElement* instrNode);
    void Load(XmlElement* instrNode);
    void CreateModuleWindow();
    Envelope* CreateEnvelope();
    void CopyDataToClonedInstrument(Instrument * instr);
};

#endif
