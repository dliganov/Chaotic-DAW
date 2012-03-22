#ifndef _VST_COLLECTION_
#define _VST_COLLECTION_

/*==================================================================================================
                                                                               
    Module Name:  VSTCollection.h

    General Description: VSTCollection class declaration. This class represents abstract element
                         which behaves as a black box having input stream, output stream and
						 set of parameters/settings which can be manipulated by client app.
						 Changing  of available characteristics of the plugin instance will
						 influence output stream.

====================================================================================================
                               Chaotic Systems Confidential Restricted
                        (c) Copyright Chaotic Systems 2008 All Rights Reserved


Revision History:
                         Modification
Author                       Date          Major Changes
----------------------   ------------      -------------
Alexander Veprik         07/23/2008        Initial version

==================================================================================================*/

//=================================================================================================
//                             Include Section
//=================================================================================================
#include "windows.h"
#include "CVSTHost.h"
//#include "Awful.h"
#include "Awful_effects.h"
#include "Awful_objects.h"
#include "awful_jucewindows.h"

//=================================================================================================
//                             Definition Section
//=================================================================================================
#define VST_MAX_NAME_LENGTH         (100)
#define VST_MAX_PARAM_LABEL_LENGTH  (30)
#define VST_MAX_PARAM_VALUE_LENGTH  (60)
#define IDLE_TIMER                  (USER_TIMER_MINIMUM + 6)
#define IDLE_TIMER_INTERVAL         (100)
#define WM_VST_PLUGEDITOR_CLOSED    (WM_USER + 1)

//=================================================================================================
//***************************** Class Declaration Section *****************************************
//=================================================================================================

class CChaoticEffect : public CEffect, public Object
{
public:
	CChaoticEffect(CVSTHost *pHost) : CEffect(pHost), Object() { this->owner = NULL; };
    virtual ~CChaoticEffect() {};
    //long EffProcessEvents(VstEvents * ptr);
};

//-------------------------------------------------------------------------------------------------
//                             CChaoticVSTHost Class Declaration
//-------------------------------------------------------------------------------------------------
class CChaoticVSTHost : public CVSTHost
{
public:
    CChaoticVSTHost();
    bool OnSetParameterAutomated(int nEffect, long index, float value);
    bool OnUpdateDisplay(int nEffect);
    void SetSampleRate(float fSampleRate);
    void SetTempo(float fBPM);
//    virtual bool OnOpenFileSelector (int nEffect, VstFileSelect *ptr);
//    virtual bool OnCloseFileSelector (int nEffect, VstFileSelect *ptr);
    virtual CEffect * CreateEffect() 
    {
        return new CChaoticEffect(this);
    }
    virtual long OnIdle(int nEffect=-1);
    virtual bool OnGetVendorString(char *text) { strcpy(text, "Chaotic Systems"); return true; }
    virtual long OnGetHostVendorVersion() { return 1; }
    virtual bool OnGetProductString(char *text) { strcpy(text, "Chaotic Sound Producer"); return true; }
    virtual bool OnCanDo(const char *ptr);
    virtual long OnAudioMasterCallback(int nEffect, long opcode, long index, long value, void *ptr, float opt);
};

//When build with JUCE, we use juce's windows
#ifndef USE_JUCE
//-------------------------------------------------------------------------------------------------
//                             VST Editor Window Class Declaration
//-------------------------------------------------------------------------------------------------
class CVSTEffWnd: public Object
{
public:
    CVSTEffWnd(CChaoticEffect* pEffect, void* ParentWnd, CVSTPlugin* pPlug);
    ~CVSTEffWnd(){};
    void SetTitle();
    void SetEffect(CChaoticEffect* pEffect){ this->pEffect = pEffect; };
    void ShowWindow(){ ::ShowWindow(this->hWndHandle, SW_SHOWNORMAL); };
    void UpdateWindow(){ ::UpdateWindow(this->hWndHandle); };
    void SetFocus(){ ::SetFocus(this->hWndHandle); };
    void Close(){ ::DestroyWindow(this->hWndHandle); };
    CVSTPlugin* pPlugin;
    void* hWndHandle; //window handle
    unsigned short width;
    unsigned short height;
    static LRESULT CALLBACK VSTWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
    CChaoticEffect *pEffect;
    void* ParentHWND;
};
#endif // USE_JUCE

//-------------------------------------------------------------------------------------------------
//                             VST Parameters Window Class Declaration
//-------------------------------------------------------------------------------------------------
class CVSTEffParamWnd: public Object
{
    CVSTEffParamWnd();
    ~CVSTEffParamWnd();
};

//-------------------------------------------------------------------------------------------------
//                             VST Plugin Class Declaration
//-------------------------------------------------------------------------------------------------
class CVSTPlugin : public Object
{
    public:
        CVSTPlugin(Object* owner, int index, CChaoticVSTHost* pHost, void* ParentWindow);
        ~CVSTPlugin();
        /* Inherited from Eff parent class */
        void                    ProcessData(float* in_buff, float* out_buff, int num_frames);
        void                    ProcessEvents(VstEvents *pEvents);
        bool                    ShowEditor();
        bool                    CloseEditor();
        void                    Open();
        void                    Close();
        void                    SetParam(long index, float Value);
        float                   GetParam(long index);
        unsigned int            GetNumParams();
        void                    GetDisplayValue(long index, char** ppValDisp);
        void                    GetParamName(long index, char** ppName);
        void                    GetParamLabel(long index, char **ppLabel);
        void                    SetReplacing(bool fReplace) { this->isReplacing = fReplace; };
        bool                    HasEditor() { return hasEditor; };
        bool                    UsesChunks() { return usesChunks; };
        void                    GetDisplayName(char *name, unsigned int length);
        long                    GetNumPresets();
        void                    GetProgramName(char *pName);
        long                    GetProgram();
        void                    SetProgram(long index);
        void                    Idle();
        void                    EditIdle();
        void                    SetBPM(float bpm);
        void                    SetSampleRate(float fSampleRate);
        void                    SetBufferSize(unsigned int uiBufferSize);
        void                    TurnOffProcessing();
        void                    TurnOnProcessing();
//        void                    GetPresetName(long index);
//        long                    GetPresetIndex(char* name);
//        bool                    SetPresetByName(char* name);
//        bool                    SetPresetByIndex(long index);
//        void                    RenamePreset(long index, char* new_name);
//        void                    RenamePreset(char* old_name, char* new_name);
//        void                    SavePresetAs(char *name);
        void                    GetErrorText();
        bool                    isWindowActive;
        bool                    isGenerator;
        HANDLE                  hMutex;
        CChaoticEffect         *pEffect;
        int                     eff_index;
        CChaoticVSTHost*        pHost;
        int nAllocatedInbufs;
        int nAllocatedOutbufs;
        Scope*                  scope;
#ifndef USE_JUCE
        CVSTEffWnd *pWnd;
#else
        PluginEditWindow *pWnd;
#endif
        int                     dispatch( const int opcode,
                                          const int index,
                                          const int value,
                                          void* const ptr,
                                          float opt )
        {
            return this->pEffect->EffDispatch(opcode, index, value, ptr, opt);
        }

    private:
        void*    ParentHWND;
        float **inBufs;
        float **outBufs;
        bool isReplacing;
        bool hasEditor;
        bool usesChunks;

};

//-------------------------------------------------------------------------------------------------
//                             VST Collection Class Declaration
//-------------------------------------------------------------------------------------------------
class VSTCollection
{
public:
    VSTCollection(void* MainWindowHandle);
    ~VSTCollection();
    CVSTPlugin* LoadPlugin(Object * owner, char* path);
    CChaoticVSTHost* pMainHost;
    void        RemovePlugin(CVSTPlugin *pPlug);
    bool        CheckPlugin(char *path, bool *isGenerator, char* name);
    void        SetBufferSize(unsigned int uiBufferSize)
                {
                    this->pMainHost->SetBlockSize(uiBufferSize);
                }

    void        SetBPM(float bpm) { this->pMainHost->SetTempo(bpm);}
    void        SetSampleRate(float fSampleRate){this->pMainHost->SetSampleRate(fSampleRate);}

    void AcquireSema();
    void ReleaseSema();

protected:

    void* ParentHWND;
    HANDLE hMutex;
};
#endif // _VST_COLLECTION_
