#ifndef PREVIEW_H
#define PREVIEW_H

#include "awful.h"
#include "awful_elements.h"
#include "awful_instruments.h"
#include "awful_panels.h"


extern PreviewSlot  PrevSlot[MAX_PREVIEW_ELEMENTS];
extern HANDLE       hPreviewMutex;

typedef struct PreviewSlot
{
    Instance*           ii;
    PrevState           state;
    Trigger             trigger;
    bool                sustainable;    // Indicates immediate release after trigger activation. Useful shit.
    bool                mouse_preview;
    bool                keybound_pianokey;
    long                start_frame;
    int                 key;
    int                 note;
}PreviewSlot;


extern unsigned int        NumPrevs;

extern void                 Preview_Init();
extern void                 Preview_StopAll();
extern void                 Preview_MouseRelease();
extern void                 Preview_FinishAll();
extern void                 Preview_ReleaseAll();
extern int                  Preview_Add(Instance* ii, Instrument* i, int key, int note, Pattern* pt, Trk* trk, Mixcell* mcell, bool noauto = false, bool setrelative = false);
extern void                 Preview_Release(Instrument* i, int note);
extern void                 Preview_Release(int key);
extern inline void          Preview_SetNoteVolume(unsigned int note, float volume);
extern inline void          Preview_SetVolume(float volume);
extern bool                 Preview_StopPerPattern(Element* el);
extern void                 Preview_ReleaseData(unsigned int ic);
extern void                 Preview_CheckStates(long num_frames);
extern void                 Preview_ForceCleanForInstrument(Instrument* instr);

#endif
