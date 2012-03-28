#ifndef AUDIO_H
#define AUDIO_H

#include <portaudio.h>
#include "awful_paramedit.h"
#include "awful.h"
#include "awful_elements.h"
//#include "awful_audio.h"
#include "awful_renderer.h"
#include "awful_midi.h"
#include "awful_events_triggers.h"


class AudioCallback : public AudioIODeviceCallback
{
    //==============================================================================
    void audioDeviceIOCallback (const float** inputChannelData,
                                int totalNumInputChannels,
                                float** outputChannelData,
                                int totalNumOutputChannels,
                                int numSamples);
    void audioDeviceAboutToStart (AudioIODevice* device);
    void audioDeviceStopped();
};

//This is most current frame which is valid even during current callback processing
extern long                 curr_frame;

//Parameters of active selected output audio device
extern PaStreamParameters   outputParameters;
//Parameters of active selected input audio device
extern PaStreamParameters   inputParameters;
extern RNDR_CONFIG_DATA_T   renderConfig;

extern CAudioDeviceManager*  audioDeviceManager;
extern AudioCallback*       audioCallBack;
extern AwfulMidiInputCallback* midiCallBack;

typedef enum VUType
{
    VU_Instr,
    VU_Big,
    VU_BigMain,
    VU_StepLine
}VUType;

class VU
{
public:
    VU(VUType vt)
    {
        vtype = vt;
        VULC = VURC = 0;
        prev = next = NULL;
        instr = NULL;
        decr = .05f;
        ceil = 1.5f;

        drawarea = new DrawArea((void*)this, Draw_VU);
        MC->AddDrawArea(drawarea);

        Add_VU(this);
    }
    ~VU()
    {
        MC->RemoveDrawArea(drawarea);

        Remove_VU(this);
    }
    INLINE void Tick()
    {
        if(VULC > 0)
        {
            VULC -= decr;
            if(VULC < 0)
                VULC = 0;
            if(drawarea->isChanged() == false)
            {
                drawarea->Change();
            }
        }
        if(VURC > 0)
        {
            VURC -= decr;
            if(VURC < 0)
                VURC = 0;
            if(drawarea->isChanged() == false)
            {
                drawarea->Change();
            }
        }
    }
    float getL()
    {
        return VULC;
    }
    float getR()
    {
        return VURC;
    }
    void SetDecr(float d)
    {
        decr = d;
    }
    void SetCeil(float c)
    {
        ceil = c;
    }
    void SetLR(float L, float R)
    {
        if(L > VULC)
        {
            VULC = L;
            if(VULC > ceil)
            {
                VULC = ceil;
            }

            if(!drawarea->isChanged())
            {
                drawarea->Change();
            }
        }

        if(R > VURC)
        {
            VURC = R;
            if(VURC > ceil)
            {
                VURC = ceil;
            }

            if(!drawarea->isChanged())
            {
                drawarea->Change();
            }
        }
    }

    int         x;
    int         y;
    int         h;
    int         oh;
    float       decr;
    float       ceil;

    DrawArea*   drawarea;
    VUType      vtype;

    Instrument* instr;
    Scope*      scope;

    VU*         prev;
    VU*         next;

private:
    float       VULC;
    float       VURC;
};

extern void                 OutsyncTriggers();
extern void                 ResetUnstableElements();
extern void                 Place_Note(Instrument *instr, int note, float vol, unsigned long start_frame, unsigned long end_frame);
extern void                 UpdatePerBaseFrame(Playback* pb);
extern PaError              PortAudio_Init();
extern PaError              PortAudio_Deinit();
extern PaError              PortAudio_Start();
extern PaError              PortAudio_Stop();
extern void                 PortAudio_UpdateDevices(int out_device_idx, int in_device_idx);
extern void                 PlayMain();
extern void                 StopMain(bool forceresetmix = false);
extern ParamSet*            New_CustomTrackParams(int track_num);
extern ParamSet*            New_InstrPatternParams(Instrument* instr, Pattern* patt);
extern ParamSet*            New_InstrTrackParams(Instrument* instr, int track_num);
extern ParamSet*            New_CustomTrackParams(int track_num);
extern ParamSet*            Get_InstrPatternParams(Instrument* instr, Pattern* patt);
extern ParamSet*            Get_InstrTrackParams(Instrument* instr, int track_num);
extern inline float         MixPans(float src_pan, float dst_pan);
extern void                 ResetProcessing(bool resetmix);
extern void                 ProcessSymbols4Gennote(Trigger* tg, long curr_frame, float* freq_active, bool* skip_and_continue, bool* skip);
extern void                 ProcessSymbols4Samplent(Trigger* tg, long curr_frame, float* freq_incr_active, bool* skip_and_continue, bool* skip);
extern inline void          GetMonoSampleData(Sample* sample, double cursor_pos, float* dataLR);
extern inline void          GeStereoSampleData(Sample* sample, double cursor_pos, float* dataL, float* dataR);
extern void                 MidiToHost_AddNoteOn(Instrument* instr, int note, int vol);
extern void                 MidiToHost_AddNoteOff(Instrument* instr, int note);
extern void                 FinishParamsRecording();
extern void                 StopRecording();
extern void                 JuceAudio_SetBufferSize(float BufSize);

#endif
