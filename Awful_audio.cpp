#include "stdafx.h"
#include "resource.h"

#include "awful.h"
#include "awful_audio.h"
#include "awful_instruments.h"
#include "awful_elements.h"
#include "awful_paramedit.h"
#include "awful_controls.h"
#include "awful_panels.h"
#include "awful_effects.h"
#include "VSTCollection.h"
#include "awful_events_triggers.h"
#include "awful_tracks_lanes.h"
#include "awful_audiohost.h"
#include "awful_renderer.h"
#include "awful_utils_common.h"
#include "awful_preview.h"
#include "awful_cursorandmouse.h"

extern void         OutsyncTriggers();
extern void         ResetProcessing(bool resetmix);
extern void         Place_Note(Instrument *instr, int note, float vol, unsigned long start_frame, unsigned long end_frame);
extern void         UpdatePerBaseFrame(Playback* pb);
extern void         ProcessSymbols4Gennote(Trigger* tg, long curr_frame, float* freq_active, bool* skip_and_continue, bool* skip);
extern PaError      PortAudio_Init();
extern PaError      PortAudio_Deinit();
extern PaError      PortAudio_Start();
extern void			PlayMain();
extern void         StopMain(bool forceresetmix);
void                MixPreviewData(float* dataL, float* dataR);
bool                IsCommandApplicable(Command* cmd, NoteInstance* instr_instance);
extern inline float MixPans(float src_pan, float dst_pan);
inline void		    GetMonoSampleData(Sample* sample, double cursor_pos, float* dataLR);
inline void			GeStereoSampleData(Sample* sample, double cursor_pos, float* dataL, float* dataR);
inline double       Sinc(double a);
inline double       SincWindowedBlackman(double a, double b, unsigned int num);
inline float        Sinc_Interpolate(float* Yi, double dX, unsigned int num);
extern void         ResetUnstableElements();
extern void         ResetPlacedEnvelopes();

/* Main callback function called by portAudio when it needs to fill sample buffer with next chunk of data */
static int          PortAudio_StreamCallback(const void  *inputBuffer, void  *outputBuffer,
                                                    unsigned long   framesPerBuffer, const PaStreamCallbackTimeInfo*  timeInfo,
                                                    PaStreamCallbackFlags   statusFlags, void  *userData);

/* Function provided to Renderer module as callback filling in sample buffer */
static RNDR_StreamCode Render_Callback(void *sample_buff, unsigned long frameCount);

void                CommonAudioCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer);


long                curr_frame;

PaStream           *Stream;					// PortAudio stream
//Parameters of active selected output audio device
PaStreamParameters  outputParameters;
//Parameters of active selected input audio device
PaStreamParameters  inputParameters;
PaTime              CurrentTime = 0;

float               databuff[MAX_BUFF_SIZE*2];

// Global mute triggers for antialiasing
bool                GlobalMute = false;
bool                MixMute = false;

//Sample buffer for sinc interpolation
static float Yi[256];

//==============================================================================
// this wraps the actual audio device
CAudioDeviceManager* audioDeviceManager = NULL;
AudioCallback*      audioCallBack = NULL;
AwfulMidiInputCallback* midiCallBack = NULL;

void AudioCallback::audioDeviceIOCallback(const float** inputChannelData, int totalNumInputChannels, float** outputChannelData, int totalNumOutputChannels, int numSamples)
{
    //Random rnd(1);
    if(totalNumOutputChannels == 2)
    {
		CommonAudioCallback(NULL, databuff, numSamples);
		for(int fc = 0; fc < numSamples; fc++)
		{
			outputChannelData[0][fc] = databuff[fc*2];
			outputChannelData[1][fc] = databuff[fc*2 + 1];
/*
			if(Playing)
			{
				outputChannelData[0][fc] = rnd.nextFloat();
				outputChannelData[1][fc] = rnd.nextFloat();
			}
			else
			{
				outputChannelData[0][fc] = 0;
				outputChannelData[1][fc] = 0;
			}
*/
		}
    }
}

void AudioCallback::audioDeviceAboutToStart(AudioIODevice * device)
{
}

void AudioCallback::audioDeviceStopped()
{
}

//
///
///////
//////////////
//////////////////
//////////////////
/////////
///////////
//////////////////////////
///////////////////
//////////
////////////8/8/8/8
//////////////*******
/////////////////////// // -9982
///////////////////////////////////
void SetSampleRate(unsigned int uiSampleRate)
{
    pVSTCollector->SetSampleRate((float)uiSampleRate);

    Eff* eff = first_eff;

    while(eff != NULL)
    {
        eff->SetSampleRate((float)uiSampleRate);
        eff = eff->next;
    }

    Instrument* instr = first_instr;

    while(instr != NULL)
    {
        instr->SetSampleRate((float)uiSampleRate);
        instr = instr->next;
    }
}
void SetBPM(float bpm)
{
    if (pVSTCollector != NULL)
    {
        pVSTCollector->SetBPM(bpm);
    }
}

bool Render_Start()
{
    long ullLastFrame;
    GetLastElementEndFrame(&ullLastFrame);

    if(ullLastFrame > 0 && Playing == false)
    {
        if(Rendering == false)
        {
            CP->PosToHome();

#ifdef USE_JUCE_AUDIO
            audioDeviceManager->removeAudioCallback(audioCallBack);
#else
			PortAudio_Stop();
#endif

			PlayMain();
			Rendering = true;
			pRenderer->SetConfig(&renderConfig);
			pRenderer->SetAudioLength(ullLastFrame);
			pRenderer->Open();
			pRenderer->Start(&Render_Callback);
		}
		else
		{
			pbkMain->UpdateQueuedEv();
			pRenderer->SetAudioLength(ullLastFrame);
			pRenderer->Resume();
		}
        return true;
	}
    else
        return false;
}

void Render_Stop()
{
    if (Rendering == true)
    {
        Rendering = false;
        pRenderer->Stop();
        pRenderer->Close();

#ifdef USE_JUCE_AUDIO
        audioDeviceManager->addAudioCallback(audioCallBack);
#else
        PortAudio_Start();
#endif
    }
    StopMain();
}

// Resets parameters being recorded to envelopes
void FinishParamsRecording()
{
    if(Recording == true)
    {
        Parameter* paramnext;
        Parameter* param = first_rec_param;
        while(param != NULL)
        {
            paramnext = param->rec_next;
            param->FinishRecording();
            param = paramnext;
        }
    }
}

// Completely stops params recording
void StopRecording()
{
    FinishParamsRecording();
    Recording = false;
    CP->Record->Release();
}

void PlayMain()
{
    if(Playing == false)  // Go playing
    {
        // "Playing selected"
        if(pbkMain->looped == true)
        {
            if(pbkMain->currFrame < pbkMain->rng_start_frame || pbkMain->currFrame > pbkMain->rng_end_frame)
            {
                pbkMain->SetCurrFrame(pbkMain->rng_start_frame);
                UpdatePerBaseFrame(pbkMain);
            }
        }
        pbkMain->UpdateQueuedEv();
        pbkMain->SetActive();
		if(aux_panel->workPt->basePattern->autopatt == false)
		{
            pbkAux->SetInactive();
        }

        // Seamless envelopes support
        PreInitEnvelopes(pbkMain->currFrame, field_pattern, field_pattern->first_ev, true);
        PreInitSamples(pbkMain->currFrame, field_pattern, field_pattern->first_ev);

        Playing = true;
    }
    else  // Pause playing
    {
        Playing = false;
        if(Recording == true)
        {
            FinishParamsRecording();
        }

        MC->poso->stopTimer();
        pbkMain->SetInactive();
        pbkAux->SetInactive();

        // Align current frame to the last *seen* pos, not the last played.
        pbkMain->SetCurrFrame((long)pbkMain->currFrame_tsync);
		if(aux_panel->workPt->basePattern->autopatt == false)
		{
            pbkAux->SetCurrFrame((long)pbkAux->currFrame_tsync);
        }

        ResetProcessing(false);
        ResetUnstableElements();
        CleanupAll();
    }
}

void StopMain(bool forceresetmix)
{
    bool resetmix = forceresetmix;
    if(resetmix == false && Playing == false && aux_panel->playing == false)
    {
        resetmix = true;
    }

    if(Playing == true || !aux_panel->isPlaying())
    {
        Playing = false;
        MC->poso->stopTimer();
        pbkMain->pactive = false;
        pbkMain->SetCurrFrame(0);
        currPlayX = 0;
        currPlayX_f = 0;

        if(Recording == true)
        {
            StopRecording();
        }

        Preview_StopAll();
        ResetProcessing(resetmix);
        CleanupAll();
        UpdateTime(Loc_MainGrid);
        AuxPos2MainPos();
    }
    else if(aux_panel->isPlaying())
    {
        aux_panel->Stop();
    }

    // Seamless envelopes support
    PreInitEnvelopes(pbkMain->currFrame, field_pattern, field_pattern->first_ev, true);
}

void ResetPlacedEnvelopes()
{
    Command* cmd;
    Trigger* tg = first_active_command_trigger;
    while(tg != NULL)
    {
        cmd = (Command*)tg->el;
        if(cmd->cmdtype != Cmd_LocVolEnv && cmd->cmdtype != Cmd_LocPanEnv)
        {
            tg->Deactivate();
		}
        tg = tg->loc_act_next;
    }
}

// This function takes care of elements that can produce undesired effects per position change
void ResetUnstableElements()
{
    // Remove active effect-elements
    Trigger* tg = first_global_active_trigger;
    Trigger* tgnext;
    while(tg != NULL)
    {
        tgnext = tg->act_next;
        if(((tg->el->type == El_GenNote) && ((NoteInstance*)tg->el)->instr->subtype == ModSubtype_VSTPlugin)||
              tg->el->type == El_Mute ||
              tg->el->type == El_Vibrate ||
              tg->el->type == El_Slider ||
              tg->el->type == El_Transpose ||
              tg->el->type == El_SlideNote ||
              tg->el->type == El_Command) //  Envelopes will be preinited when playback restarts
        {
            tg->toberemoved = true;
        }
        tg = tgnext;
    }

    // Block instance triggers to prevent unneeded effects
    OutsyncTriggers();
}

void OutsyncTriggers()
{
    Trigger* tg = first_global_active_trigger;
    Trigger* tgnext;
    while(tg != NULL)
    {
        tgnext = tg->act_next;
        if(tg->el->IsInstance())
        {
            tg->outsync = true;
        }
        tg = tgnext;
    }
}

void ResetProcessing(bool resetmix)
{
    GlobalMute = true;
    if(resetmix)
    {
        MixMute = true;
		field_pattern->Reset();
		Trk* trk = first_trkdata;
		while(trk != NULL)
		{
			trk->fxstate.t_break = false;
			trk->fxstate.t_reversed = false;
			trk->fxstate.t_mutecount = 0;
			trk->envelopes = NULL;
			trk->fxstate.freqtrig = NULL;
			trk->fxstate.t_freq_mult = 1;

            trk = trk->next;
        }
        mAster.efftrig = NULL;
        mAster.envelopes = NULL;
        mAster.freq_mult = 1;

        Instrument* i = first_instr;
        while(i != NULL)
        {
            i->Reset();
            i = i->next;
        }
    }
}

void Place_Note(Instrument *instr, int note, float vol, unsigned long start_frame, unsigned long end_frame)
{
    aux_panel->PlaceNote(instr, note, vol, start_frame, end_frame);
}

void MidiToHost_AddNoteOn(Instrument* instr, int note, int vol)
{
    float notevol;
    if(vol <= 100)
        notevol = (float)vol/100;
    else
        notevol = float(vol - 100)/27.f*(VolRange - 1) + 1;

    // Let's handle recording stuff
    Pattern* pt = NULL;
    if(Recording)
    {
        if(Playing)
        {
            pt = field_pattern;
            if(!aux_panel->isBlank())
            {
                pt = aux_panel->workPt;
                C.SetPattern(pt, Loc_MainGrid);
            }
            else if(C.patt == field_pattern)
            {
                C.SetPos(Frame2Tick(pbkMain->currFrame), CLine);
                pt = CreateElement_Pattern(CTick, CTick + (float)ticks_per_beat, CLine, CLine, Patt_Pianoroll);
                C.SetPattern(pt, Loc_MainGrid);

                // Remove it from field
                C.type = CType_None;
                R(Refresh_KeyCursor);
                MC->ForceRefresh();
            }
            else
                pt = C.patt;

            //Place_Note(((Instance*)(PrevSlot[ic].trig.el))->instr,
            //        note, PrevSlot[ic].trig.vol_val, PrevSlot[ic].start_frame, pbMain->currFrame - pbAux->currFrame);

            if(instr->subtype == ModSubtype_Sample)
            {
                // If this is a sample, then record it immediately, since it's released by itself, without
                // waiting for user to release the key on MIDI keyboard
                tframe frame1 = pbkMain->currFrame - C.patt->StartFrame();
                Grid_PutInstanceSpecific(C.patt, 
                                         instr, 
                                         note + 1, 
                                         notevol, 
                                         frame1,
                                         frame1 + 1);

                float newlength = Frame2Tick(frame1 + 4444);
                if(newlength > C.patt->tick_length)
                    C.patt->SetTickLength(newlength);

                R(Refresh_Grid);
                MC->poso->ScheduleRefresh();
                //MC->listen->CommonInputActions();
            }
        }
        else if(aux_panel->playing)
        {
            if(aux_panel->isBlank())
            {
                aux_panel->CreateNew();
            }
            pt = aux_panel->workPt;
        }
    }

    // Then preview the note
    int num = Preview_Add(NULL, instr, -1, note, pt, NULL, NULL);

    // Remember the frame and the volume, to use them after note is released
    if(num != 0xFFFF)
    {
        PrevSlot[num].ii->loc_vol->SetNormalValue(notevol);
        PrevSlot[num].start_frame = pbkMain->currFrame;
    }

    //if(Recording)
    //{
     //   Place_Note(instr, note, vol, pbMain->currFrame, pbMain->currFrame + 44100);
     //   R(Refresh_AuxGrid);
     //   MC->listen->CommonInputActions();
    //}
}

void MidiToHost_AddNoteOff(Instrument* instr, int note)
{
    Preview_Release(instr, note);
}

extern inline float MixPans(float src_pan, float dst_pan)
{
    return src_pan*(1 - abs(dst_pan)) + dst_pan;
}

inline bool IsCommandApplicable(Command* cmd, NoteInstance* instance)
{
    if(cmd->scope->local == true)
    {
        if(cmd->patt == instance->patt)
        {
            if(cmd->scope->instr == instance->instr)
            {
                return true;
            }
        }
    }
    else if(cmd->scope->for_track == true)
    {
		if(cmd->field_trknum == instance->field_trknum)
        {
            if(cmd->scope->instr == instance->instr)
            {
                return true;
            }
        }
    }
    else if(cmd->scope->pt != NULL)
    {
        if(cmd->scope->pt == instance->patt)
        {
            if(cmd->scope->instr == instance->instr)
            {
                return true;
            }
        }
    }
    else
    {
        if(cmd->scope->instr == instance->instr)
        {
            return true;
        }
    }

    return false;
}

void StopPlacedEnvelopes()
{
    Trigger* tg = first_active_command_trigger;
    while(tg != NULL)
    {
        if(tg->el->IsPresent())
        {
           ((Envelope*)((Command*)tg->el)->paramedit)->newbuff = false;;
        }
        tg = tg->loc_act_next;
    }
}

void UpdatePerBaseFrame(Playback* pb)
{
    if(pb == pbkMain)
    {
        currPlayX_f = pbkMain->currFrame/framesPerPixel;
        currPlayX = (int)currPlayX_f;
        AuxPos2MainPos();
    }
    else if(pb == pbkAux)
    {
        aux_panel->curr_play_x_f = (pbkAux->currFrame - aux_panel->workPt->frame)/aux_panel->frames_per_pixel;
        aux_panel->curr_play_x = (int)aux_panel->curr_play_x_f;
        MainPos2AuxPos();
    }
}

INLINE float Gaussian_Interpol(float* Yi, float dX)
{
    float ret_val = 0;

//    double a = -1.0/12.0*Yi[2]-1.0/120.0*Yi[0]+1.0/24.0*Yi[1]+1.0/120.0*Yi[5]+1.0/12.0*Yi[3]-1.0/24.0*Yi[4];
//    double b = 13.0/12.0*Yi[2]+1.0/8.0*Yi[0]-7.0/12.0*Yi[1]-1.0/12.0*Yi[5]-Yi[3]+11.0/24.0*Yi[4];
//    double c = -59.0/12.0*Yi[2]-17.0/24.0*Yi[0]+71.0/24.0*Yi[1]+7.0/24.0*Yi[5]+49.0/12.0*Yi[3]-41.0/24.0*Yi[4];
//    double d = 107.0/12.0*Yi[2]+15.0/8.0*Yi[0]-77.0/12.0*Yi[1]-5.0/12.0*Yi[5]-13.0/2.0*Yi[3]+61.0/24.0*Yi[4];
//    double e = -137.0/60.0*Yi[0]+5.0*Yi[1]-5.0*Yi[2]+10.0/3.0*Yi[3]-5.0/4.0*Yi[4]+1.0/5.0*Yi[5];

    dX += 2;
    //ret_val = ((((a*dX + b)*dX + c)*dX + d)*dX + e)*dX + Yi[0];
    ret_val = (float)((((((-1.0/12.0*Yi[2]-1.0/120.0*Yi[0]+1.0/24.0*Yi[1]+1.0/120.0*Yi[5]+1.0/12.0*Yi[3]-1.0/24.0*Yi[4])*dX + \
					   (13.0/12.0*Yi[2]+1.0/8.0*Yi[0]-7.0/12.0*Yi[1]-1.0/12.0*Yi[5]-Yi[3]+11.0/24.0*Yi[4]))*dX + \
					   (-59.0/12.0*Yi[2]-17.0/24.0*Yi[0]+71.0/24.0*Yi[1]+7.0/24.0*Yi[5]+49.0/12.0*Yi[3]-41.0/24.0*Yi[4]))*dX + \
					   (107.0/12.0*Yi[2]+15.0/8.0*Yi[0]-77.0/12.0*Yi[1]-5.0/12.0*Yi[5]-13.0/2.0*Yi[3]+61.0/24.0*Yi[4]))*dX + \
					   (-137.0/60.0*Yi[0]+5.0*Yi[1]-5.0*Yi[2]+10.0/3.0*Yi[3]-5.0/4.0*Yi[4]+1.0/5.0*Yi[5]))*dX + Yi[0]);

    return ret_val;
}

INLINE float Sinc_Interpolate(float* Yi, double dX, unsigned int num)
{
    double ret_val = 0;
    unsigned char mid = num/2;
    int j = (int)(0 - mid);
    unsigned int i = 0;

    for (; i < num; ++i)
    {
        ret_val += Yi[i]*SincWindowedBlackman((double)(j - dX), (double) (i - dX), num);
        ++j;
    }

    return (float)ret_val;
}

INLINE double Sinc(double a)
{
    return sin(PI*a)/(PI*a);
}

INLINE double SincWindowedBlackman(double a, double b, unsigned int num)
{
    return Sinc(a)*(0.42 - 0.5*cos((2.f*PI*b)/((float)num)) + 0.08*cos((4.f*PI*b)/((float)num)));
}

INLINE void GetMonoSampleData(Sample* sample, double cursor_pos, float* dataLR)
{
    float x1 = (float)(int)cursor_pos;
    float x2 = x1 + 1;

    float y1 = sample->sample_data[(int)x1];
    float y2;
    float  Y[6];

    if((int)x2 == sample->sample_info.frames)
    {
        y2 = 0;
    }
    else
    {
        y2 = sample->sample_data[(int)x2];
    }

    double x3 = cursor_pos;

    if (x1 == x3)
    {
        *dataLR = sample->sample_data[(int)x3];
    }
    else
    {
        unsigned int method = WorkingInterpolationMethod;
        if(Rendering == true)
            method = RenderInterpolationMethod;

        switch (method)
        {
            case Interpol_linear:
            {
               *dataLR = Interpolate_Line(x1, y1, x2, y2, x3);
            }
            break;

            case Interpol_3dHermit:
            {
                float dX = (float)cursor_pos - x1;
                float xm1 = x1 > 0 ? sample->sample_data[(int)(x1-1)] : 0;
                float _x0  = sample->sample_data[(int)x1];
                float _x1  = (x1+1) <= (sample->sample_info.frames -1) ? sample->sample_data[(int)(x1 + 1)] : sample->sample_data[(int)x1];
                float _x2  = (x1+2) <= (sample->sample_info.frames -1) ? sample->sample_data[(int)(x1 + 2)] : sample->sample_data[(int)x1];
//                float a = (3 * (_x0-_x1) - xm1 + _x2) / 2;
//                float b = 2*_x1 + xm1 - (5*_x0 + _x2) / 2;
//                float c = (_x1 - xm1) / 2;
                *dataLR = (((((3 * (_x0-_x1) - xm1 + _x2) / 2) * dX) +\
                          (2*_x1 + xm1 - (5*_x0 + _x2) / 2)) * dX + ((_x1 - xm1) / 2)) * dX + _x0;
            }
            break;

            case Interpol_6dHermit:
            {
                float dX = (float)cursor_pos - x1;
                int i = 0;
                unsigned int pX = (x1 > 1)? (unsigned int)x1-2: 0;

                for (i = 0; i<6;++i)
                {
                    if ((pX + i)< (sample->sample_info.frames -1))
                    {
                        Y[i] = sample->sample_data[(int)pX+i];
                    }
                    else
                    {
                        Y[i] = 0;
                    }
                }
                *dataLR = Gaussian_Interpol(Y, dX);
            }
            break;

            case Interpol_64Sinc:
            case Interpol_128Sinc:
            case Interpol_256Sinc:
            {
                double dX = cursor_pos - x1;
                unsigned int num = 64;
                if (method == Interpol_128Sinc)
                {
                    num = 128;
                }
                else if (method == Interpol_256Sinc)
                {
                    num = 256;
                }

                int uiLBorder;

                uiLBorder = int(x1 - num/2);

                for (unsigned int i = 0; i < num; ++i)
                {
                    if (((uiLBorder + i) >= 0) && ((uiLBorder + i) < sample->sample_info.frames))
                    {
                        Yi[i] = sample->sample_data[(int)(uiLBorder + i)];
                    }
                    else
                    {
                        Yi[i] = 0;
                    }
                }

                *dataLR = Sinc_Interpolate(Yi, dX, num);
            }
            break;

            default:
            {
            }
            break;
        }
    }
}

INLINE void GeStereoSampleData(Sample* sample, double cursor_pos, float* dataL, float* dataR)
{
    double x1, x2, x3;
	float y1, y2;
    float  Y[6];

    // process left
    x1 = (float)((int)cursor_pos);
    x2 = x1 + 1;
    x3 = cursor_pos;

    y1 = sample->sample_data[(int)x1*2];

    if((int)x2 >= sample->sample_info.frames)
    {
        y2 = 0;
    }
    else
    {
        y2 = sample->sample_data[(int)x2*2];
    }

    /* If current pos matches to sample point, don't need to interpolate (make CPU overhead) */
    if (x3 == x1)
    {
        *dataL = sample->sample_data[(int)x3*2];
    }
    else
    {
        unsigned int method = WorkingInterpolationMethod;
        if(Rendering == true)
            method = RenderInterpolationMethod;

        switch (method)
        {
            case Interpol_linear:
            {
                *dataL = Interpolate_Line(x1, y1, x2, y2, x3);
            }
            break;

            case Interpol_3dHermit:
            {
                float dX = (float)(cursor_pos - x1);
                float xm1 = x1 > 0 ? sample->sample_data[(int)(x1-1)*2] : 0;
                float _x0  = sample->sample_data[(int)x1*2];
                float _x1  = (x1+1) <= (sample->sample_info.frames -1) ? sample->sample_data[(int)(x1 + 1)*2] : sample->sample_data[(int)x1*2];
                float _x2  = (x1+2) <= (sample->sample_info.frames -1) ? sample->sample_data[(int)(x1 + 2)*2] : sample->sample_data[(int)x1*2];
//                float a = (3 * (_x0-_x1) - xm1 + _x2) / 2;
//                float b = 2*_x1 + xm1 - (5*_x0 + _x2) / 2;
//                float c = (_x1 - xm1) / 2;

                *dataL = (((((3 * (_x0-_x1) - xm1 + _x2) / 2) * dX) +\
                         (2*_x1 + xm1 - (5*_x0 + _x2) / 2)) * dX + ((_x1 - xm1) / 2)) * dX + _x0;
            }
            break;

            case Interpol_6dHermit:
            {
                float dX = (float)(cursor_pos - x1);
                int i = 0;
                unsigned int pX = (x1 > 1)? (unsigned int)x1-2: 0;

                for (i = 0; i<6;++i)
                {
                    if ((pX + i) < (sample->sample_info.frames -1))
                    {
                        Y[i] = sample->sample_data[(int)(pX+i)*2];
                    }
                    else
                    {
                        Y[i] = 0;
                    }
                }
                *dataL = Gaussian_Interpol(Y, dX);
            }
            break;

            case Interpol_64Sinc:
            case Interpol_128Sinc:
            case Interpol_256Sinc:
            {
                double dX = (cursor_pos - x1);
                unsigned int num = 64;
                if (method == Interpol_128Sinc)
                {
                    num = 128;
                }
                else if (method == Interpol_256Sinc)
                {
                    num = 256;
                }

                unsigned int uiLBorder;

                uiLBorder = int(x1 - num/2);

                for (unsigned int i = 0; i<num; ++i)
                {
                    if (((uiLBorder + i) >= 0) && ((uiLBorder + i) < sample->sample_info.frames))
                    {
                        Yi[i] = sample->sample_data[(int)(uiLBorder + i)*2];
                    }
                    else
                    {
                        Yi[i] = 0;
                    }
                }
                *dataL = Sinc_Interpolate(Yi,dX,num);
            }
            break;

            default:
            {
            }
            break;
        }
    }

    // process right
    y1 = sample->sample_data[(int)x1*2 + 1];

    if(((int)x2 + 1) >= sample->sample_info.frames)
    {
        y2 = 0;
    }
    else
    {
        y2 = sample->sample_data[(int)x2*2 + 1];
    }

    /* If current pos matches to sample point, don't need to interpolate (make CPU overhead) */
    if (x3 == x1)
    {
        *dataR = sample->sample_data[(int)x3*2 + 1];
    }
    else
    {
        unsigned int method = WorkingInterpolationMethod;
        if(Rendering == true)
            method = RenderInterpolationMethod;

        switch (method)
        {
            case Interpol_linear:
            {
                *dataR = Interpolate_Line(x1, y1, x2, y2, x3);
            }
            break;

            case Interpol_3dHermit:
            {
                float dX = (float)(cursor_pos - x1);
                float xm1 = x1 > 0 ? sample->sample_data[(int)(x1-1)*2 + 1] : 0;
                float _x0  = sample->sample_data[(int)x1*2 + 1];
                float _x1  = (x1+1) <= (sample->sample_info.frames -1) ? sample->sample_data[(int)(x1 + 1)*2 + 1] : sample->sample_data[(int)x1*2 + 1];
                float _x2  = (x1+2) <= (sample->sample_info.frames -1) ? sample->sample_data[(int)(x1 + 2)*2 + 1] : sample->sample_data[(int)x1*2 + 1];
//                float a = (3 * (_x0-_x1) - xm1 + _x2) / 2;
//                float b = 2*_x1 + xm1 - (5*_x0 + _x2) / 2;
//                float c = (_x1 - xm1) / 2;

               *dataR = (((((3 * (_x0-_x1) - xm1 + _x2) / 2) * dX) +\
                        (2*_x1 + xm1 - (5*_x0 + _x2) / 2)) * dX + ((_x1 - xm1) / 2)) * dX + _x0;
            }
            break;

            case Interpol_6dHermit:
            {
                float dX = (float)(cursor_pos - x1);
                int i = 0;
                unsigned int pX = (x1 > 1)? (unsigned int)x1-2: 0;

                for (i = 0; i<6;++i)
                {
                    if ((pX + i) < (sample->sample_info.frames -1))
                    {
                        Y[i] = sample->sample_data[(int)(pX+i)*2 + 1];
                    }
                    else
                    {
                        Y[i] = 0;
                    }
                }
                *dataR = Gaussian_Interpol(Y, dX);
            }
            break;

            case Interpol_64Sinc:
            case Interpol_128Sinc:
            case Interpol_256Sinc:
            {
                double dX = (cursor_pos - x1);
                unsigned int num = 64;
                if (method == Interpol_128Sinc)
                {
                    num = 128;
                }
                else if (method == Interpol_256Sinc)
                {
                    num = 256;
                }

                unsigned int uiLBorder;

                uiLBorder = int(x1 - num/2);

                for (unsigned int i = 0; i<num; ++i)
                {
                    if (((uiLBorder + i) >= 0) && ((uiLBorder + i) < sample->sample_info.frames))
                    {
                        Yi[i] = sample->sample_data[(int)(uiLBorder + i)*2 + 1];
                    }
                    else
                    {
                        Yi[i] = 0;
                    }
                }
                *dataR = Sinc_Interpolate(Yi, dX, num);
            }
            break;

            default:
            {
            }
            break;
        }
    }
}

void PortAudio_UpdateDevices(int out_device_idx, int in_device_idx)
{
    inputParameters.device = (PaDeviceIndex) in_device_idx;
    inputParameters.channelCount = 2;
    inputParameters.hostApiSpecificStreamInfo = NULL;
    inputParameters.sampleFormat = paFloat32;

    if (in_device_idx != -1)
    {
        inputParameters.suggestedLatency =
            (Pa_GetDeviceInfo(inputParameters.device))->defaultLowInputLatency;
    }
    else
    {
        inputParameters.suggestedLatency = 0;
    }

    outputParameters.device = (PaDeviceIndex) out_device_idx;
    outputParameters.channelCount = 2;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    outputParameters.sampleFormat = paFloat32;

    if (out_device_idx != -1)
    {
        outputParameters.suggestedLatency =
            (Pa_GetDeviceInfo(outputParameters.device))->defaultLowOutputLatency;
    }
    else
    {
        outputParameters.suggestedLatency = 0;
    }
}

PaError PortAudio_Start()
{
    PaError err = 0;
    PaStreamParameters *pInput  = NULL;
    PaStreamParameters *pOutput = NULL;

    if (outputParameters.device != -1)
    {
        pOutput = &outputParameters;

        if (inputParameters.device != -1)
        {
            pInput = &inputParameters;
        }

        /* Open an audio I/O stream. */
        err = Pa_OpenStream(&Stream,
                             pInput,
                             pOutput,
                             fSampleRate,
                             gBuffLen,
                             paNoFlag,
                             PortAudio_StreamCallback,
                             NULL);

        if(err == 0)
        {
            err = Pa_StartStream(Stream);
        }
        else
        {
            switch(err)
            {
                case paInvalidSampleRate:
                    MessageBox(0,"Selected sample rate is not supported by output device you have chosen",
                        "Config error", MB_OK|MB_ICONERROR);
                break;
                case paBadIODeviceCombination:
                    MessageBox(0,"Invalid combination of I/O devices",
                        "Config error", MB_OK|MB_ICONERROR);
                break;

                default:
                break;
            }
        }
    }
    else
    {
        err = paInvalidDevice;
    }

    return err;
}

PaError PortAudio_Stop()
{
    PaError err;

    err = Pa_AbortStream(Stream);
    err = Pa_CloseStream(Stream);

    return err;
}

PaError PortAudio_Init()
{
    PaError  err;

    // Initialize Port Audio
    err = Pa_Initialize();
    if(err == paNoError)
    {
        PortAudio_UpdateDevices((int)Pa_GetDefaultOutputDevice(), -1);
        err = PortAudio_Start();
    }
    return err;
}

PaError PortAudio_Deinit()
{
    PaError err;
    PortAudio_Stop();
    err = Pa_Terminate();

    return err;
}

void PortAudio_SetBufferSize(float BufSize)
{
#ifndef USE_JUCE_AUDIO
    if(PortAudio_Stop() == paNoError)
    {
        gBuffLen = (int)BufSize;
        pVSTCollector->SetBufferSize(BufSize);

        Eff* eff = first_eff;
        while(eff != NULL)
        {
            eff->SetBufferSize((unsigned int)BufSize);
            eff = eff->next;
        }

        Instrument* instr = first_instr;
        while(instr != NULL)
        {
            instr->SetBufferSize((unsigned int)BufSize);
            instr = instr->next;
        }

		/*
        memset(mix->m_cell.in_buff, 0, (MAX_BUFF_SIZE*2)*sizeof(float));
        for (int i = 0; i < NUM_MASTER_CELLS; ++i)
        {
            memset(mix->o_cell[i].in_buff, 0, (MAX_BUFF_SIZE*2)*sizeof(float));
        }*/

        PortAudio_Start();
    }
#endif
}

void JuceAudio_SetBufferSize(float BufSize)
{
#ifdef USE_JUCE_AUDIO
    //audioDeviceManager->stopDevice();
    gBuffLen = (int)BufSize;
    pVSTCollector->SetBufferSize(gBuffLen);

    Eff* eff = first_eff;
    while(eff != NULL)
    {
        eff->SetBufferSize(gBuffLen);
        eff = eff->next;
    }

    Instrument* instr = first_instr;
    while(instr != NULL)
    {
        instr->SetBufferSize((unsigned int)BufSize);
        instr = instr->next;
    }

    CAudioDeviceManager::AudioDeviceSetup setup;
    audioDeviceManager->getAudioDeviceSetup(setup);
    setup.bufferSize = int(BufSize);
    audioDeviceManager->setAudioDeviceSetup(setup, true);
#endif
}

RNDR_StreamCode Render_Callback(void *sample_buff, unsigned long frameCount)
{
    RNDR_StreamCode ret_val = RNDR_Continue;

    if (Playing != TRUE)
    {
        ret_val = RNDR_Complete;
    }
    else
    {
        //PortAudio_StreamCallback(NULL, sample_buff, frameCount, NULL, 0, NULL);
        CommonAudioCallback(NULL, sample_buff, frameCount);
    }

    return ret_val;
}

void CommonAudioCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer)
{
    WaitForSingleObject(hAudioProcessMutex, INFINITE);

    // Cast data passed through stream to our structure.
    float          *out = (float*)outputBuffer;
    long            bc, fpb;
    const float    *in = (float*)inputBuffer;

    fpb = (long)framesPerBuffer;

    long baroffs = 0;
    long beatoffs = 0;
    // If metronome is enabled, check if we need to activate beat- or bar-sample
    if(Metronome_ON && (Playing == true || aux_panel->playing == true))
    {
        tframe frame = Playing == true ? pbkMain->currFrame : pbkAux->currFrame - pbkAux->playPatt->StartFrame();
        long fpbeat = long(frames_per_tick*ticks_per_beat);
        long fpbar = fpbeat*beats_per_bar;
        if(frame == 0 ||(frame/fpbar != (frame + fpb)/fpbar))
        {
            baroffs = (frame + fpb)/fpbar*fpbar - frame;
            barsample->ActivateTrigger(barsample->prevInst->tg_first);
        }
        else if(frame/fpbeat != (frame + fpb)/fpbeat)
        {
            beatoffs = (frame + fpb)/fpbeat*fpbeat - frame;
            beatsample->ActivateTrigger(beatsample->prevInst->tg_first);
        }
    }

    // Cleanup mixer buffers here
    aux_panel->CleanBuffers(fpb);

    jassert(NumPrevs >= 0);
    //if(NumPrevs > 0)
    {
        Preview_CheckStates(fpb);
    }

    if(aux_Pattern != pbkAux->playPatt)
    {
        pbkAux->SetPlayPatt(aux_Pattern);
        if(aux_panel->playing == true)
        {
            pbkAux->ResetPos();
            UpdatePerBaseFrame(pbkAux);
        }
    }

    if(Playing == true)
    {
        if(pbkMain->tsync_block == false)
            MC->poso->initTimer();
        UpdateTime(Loc_MainGrid);
    }
    else if(aux_panel->playing == true)
    {
        if(pbkAux->tsync_block == false)
            MC->poso->initTimer();
        UpdateTime(Loc_SmallGrid);
    }
    
    Instrument* instr;
    bool        changed = true;
    long        nc2 = 0;
    long        framestotick, nextframestotick;
    framestotick = fpb;
    if(Playing == true)
    {
        pbkMain->GetSmallestCountDown(&framestotick);
    }
    else if(aux_panel->playing == true)
    {
        pbkAux->GetSmallestCountDown(&framestotick);
    }
    
    Playback* pb = first_active_playback;
    while(pb != NULL)
    {
        pb->GetSmallestCountDown(&framestotick);
        pb = pb->next;
    }

    long    nc = 0;
    while(nc < fpb)
    {
        curr_frame = framestotick;
        nextframestotick = fpb - nc - framestotick;

        // Playback can go either on Main field or on Aux field, but not on both
        if(Playing == true)     // Main field is playing
        {
            pbkMain->TickFrame(nc, framestotick, fpb);
            pbkMain->GetSmallestCountDown(&nextframestotick);

            // Synchronize current frame in Aux, if not editing autopattern
            if(aux_panel->workPt->autopatt == false)
            {
                pbkAux->currFrame = pbkMain->currFrame;
            }

            // Stop on the last frame (disabled currently)
            //if(pbMain->currFrame >= lastFrame)
            //{
            //    const MessageManagerLock mmLock;
            //    CP->HandleButtDown(CP->play_butt);
            //    MC->listen->CommonInputActions();
            //}
        }
        else if(aux_panel->playing == true)  // Aux field is playing
        {
            pbkAux->TickFrame(nc, framestotick, fpb);
            pbkAux->GetSmallestCountDown(&nextframestotick);

            // Synchronize current frame in Main, if not editing autopattern
            if(aux_panel->workPt->autopatt == false)
            {
                pbkMain->currFrame = pbkAux->currFrame;
            }
        }

        pb = first_active_playback;
        while(pb != NULL)
        {
            pb->TickFrame(nc, framestotick, fpb);
            if(pb->dworking == false)
                DeactivatePlayback(pb);
            else
                pb->GetSmallestCountDown(&nextframestotick);
            pb = pb->next;
        }

        // Main instruments processing loop
        instr = first_instr;
        while(instr != NULL)
        {
            instr->GenerateData(framestotick, nc);
            instr = instr->next;
        }

        StopPlacedEnvelopes();

        nc += framestotick;
        framestotick = nextframestotick;
    }

    if(Metronome_ON && (Playing == true || aux_panel->playing == true))
    {
        barsample->GenerateData(fpb - baroffs, baroffs);
        beatsample->GenerateData(fpb - beatoffs, beatoffs);
    }

    // Active triggers cleanup
    Trigger* tg = first_global_active_trigger;
    Trigger* tgnext;
    while(tg != NULL)
    {
        tgnext = tg->act_next;
        if(tg->CheckRemoval())
        {
            if(tg->el->IsInstance() && // Smoothly finish playing notes
               (tg->tgstate == TgState_Sustain || tg->tgstate == TgState_Release))
            {
                tg->SoftFinish();
            }
            else
            {
                tg->Deactivate();
            }
        }
        tg = tgnext;
    }

    int fadenum = fpb >= 100 ? 100 : fpb;
    if(GlobalMute)
    {
        // Force active triggers cleanup, when need to fade out
        tg = first_global_active_trigger;
        while(tg != NULL)
        {
            if(MixMute)
                tg->Deactivate();
            else
            {
                if(tg->el->IsInstance())
                    tg->SoftFinish();
                else
                    tg->Deactivate();
            }
            tg = tg->act_next;
        }

        // Force deactivating active playbacks as well, if total muting
        if(MixMute)
        {
            pb = first_active_playback;
            while(pb != NULL)
            {
                DeactivatePlayback(pb);
                pb = pb->next;
            }
        }
        GlobalMute = false;
    }

    aux_panel->Mix(fpb);

    bool mutemixing = false;
    if(MixMute)
    {
        mutemixing = true;
        Eff* eff = first_eff;
        while(eff != NULL)
        {
            eff->Reset();
            eff = eff->next;
        }

        Instrument* instr = first_instr;
        while(instr != NULL)
        {
            if(instr->instrtype == Instr_VSTPlugin)
            {
               ((VSTGenerator*)instr)->pEff->Reset();
            }
            instr = instr->next;
        }
    }

    Envelope* mvenv = NULL;
    Trigger* tgenv = mAster.params->vol->envelopes;
    if(tgenv != NULL && mAster.params->vol->envaffect)
    {
        mvenv = (Envelope*)((Command*)tgenv->el)->paramedit;
    }

    float vol = mAster.params->vol->outval;
    if(mAster.params->vol->lastval == -1)
    {
        mAster.params->vol->SetLastVal(mAster.params->vol->outval);
    }
    else if(mAster.params->vol->lastval != mAster.params->vol->outval)
    {
        if(mAster.rampCounterV == 0)
        {
            mAster.cfsV = float(mAster.params->vol->outval - mAster.params->vol->lastval)/RAMP_COUNT;
            vol = mAster.params->vol->lastval;
            mAster.rampCounterV = RAMP_COUNT;
        }
        else
        {
            vol = mAster.params->vol->lastval + (RAMP_COUNT - mAster.rampCounterV)*mAster.cfsV;
        }
    }
    else if(mAster.rampCounterV > 0) // (params->vol->lastval == params->vol->outval)
    {
        mAster.rampCounterV = 0;
        mAster.cfsV = 0;
    }

    float outL, outR, lMax, rMax;
    lMax = rMax = 0;
    bc = 0;
    if(in == NULL)
    {
        for(nc = 0; nc < fpb; nc++)
        {
            if(mvenv != NULL && nc >= mvenv->last_buffframe && mAster.rampCounterV == 0)
            {
                vol = mvenv->buffoutval[nc];
                if(nc == mvenv->last_buffframe_end - 1)
                {
                    mAster.params->vol->SetValueFromEnvelope(mvenv->buff[nc], mvenv);
                    mAster.params->vol->SetLastVal(mAster.params->vol->outval);
                }
            }

            if(mAster.rampCounterV > 0)
            {
                vol += mAster.cfsV;
                mAster.rampCounterV--;
                if(mAster.rampCounterV == 0)
                {
                    mAster.params->vol->SetLastVal(mAster.params->vol->outval);
                }
            }
            outL = aux_panel->masterchan.in_buff[bc++]*vol;
            outR = aux_panel->masterchan.in_buff[bc++]*vol;

            if(outL > lMax)
                lMax = outL;
            if(outR > rMax)
                rMax = outR;

           *out++ = outL;
           *out++ = outR;
        }

        CP->MVol->vu->SetLR(lMax, rMax);
        if(aux_panel->masterchan.p_vu->drawarea->isEnabled())
        {
            aux_panel->masterchan.p_vu->SetLR(lMax, rMax);
        }
    }

    if(mutemixing)
        MixMute = false;

    ReleaseMutex(hAudioProcessMutex);
}

/* This routine will be called by the PortAudio engine when audio is needed.
** It may be called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int PortAudio_StreamCallback(const void   *inputBuffer,
                                          void   *outputBuffer,
                                          unsigned long    framesPerBuffer,
                                          const PaStreamCallbackTimeInfo*   timeInfo,
                                          PaStreamCallbackFlags    statusFlags,
                                          void   *userData)
{
    CommonAudioCallback(inputBuffer, outputBuffer, framesPerBuffer);

    return paContinue;
}

#define Params

/*
SoundFont::SoundFont(sfBankID bankid)
{
    bank_id = bankid;
    type = Instr_SoundFont;
}*/

