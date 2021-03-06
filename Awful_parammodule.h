#ifndef PARAMMODULE_H
#define PARAMMODULE_H

#include "awful.h"
#include "Awful_params.h"
#include "Awful_objects.h"
#include "awful_jucewindows.h"

class PluginCommonWindow;

class ParamModule : public Object
{
public:
    ModuleType      modtype;
    ModuleSubType   subtype;

    bool        to_be_deleted;
    bool        temp;
    bool        internalModule;

    Preset*     NativePresets;
    Preset*     NP_last;
    long        NumNativePresets;
    long        NumPresets;
    Preset*     CurrentPreset;

    Paramcell*  first_pcell;
    Paramcell*  last_pcell;

    Parameter*  first_param;
    Parameter*  last_param;
    int         lastparamidx;

    long        uniqueID;

    char        originalname[MAX_NAME_STRING];
    char        name[MAX_NAME_STRING];
    char        path[MAX_PATH_STRING];
    char        preset_path[MAX_PATH_STRING];
    char        current_preset_name[MAX_NAME_STRING]; // we use this string to identify current present on scanning

    Scope       scope;

    int         x;
    int         y;
    int         width;
    int         height;

    bool        visible;

    int         index;

    PluginCommonWindow* wnd; // Internal reference to the window

    ParamWindow* paramWnd;

    Trigger*    envelopes;


    ParamModule();
    virtual ~ParamModule();
    virtual void SetName(char* name);
    virtual void SetPath(char* path);
    virtual void SetPresetPath(char* presetpath);
//=================================================================================================
//                This is our approach to hierarchical effect composition

    //Linked list of all children Effects. All children connected to each other by sibling pointers
    ParamModule*    children;
    //Pointer to parent Effect
    ParamModule*    parent;

    //these are helper pointers which are used for traversing list of children effects
    ParamModule*    next_sibling;
    ParamModule*    prev_sibling;
    //Should be used to add a child into children list
    virtual void    AddChild(ParamModule* pEffect);
    //Should be used to remove an effect from children list
    virtual void    RemoveChild(ParamModule* pEffect);
    //Use it for children look up
    ParamModule*    FindChild(char* name);
//=================================================================================================
    virtual void    SaveStateData(XmlElement & xmlParentNode, char* pPresetName = NULL, bool global = false);
    virtual void    RestoreStateData(XmlElement & xmlStateNode, bool global = false);
    virtual void    SaveCustomStateData(XmlElement & xmlParentNode) {};
    virtual void    RestoreCustomStateData(XmlElement & xmlStateNode) {};
    virtual void    ParamUpdate(Parameter* param = NULL) {};
    virtual void    AddParam(Parameter* param);
    virtual void    RemoveParam(Parameter* param);
    Paramcell*      AddParamWithParamcell(Parameter* param);
            void    AddParamToParamcell(Parameter* param, Paramcell* pcell);
    virtual void    AddParamcell(Paramcell* pcell);
    virtual void    RemoveParamcell(Paramcell* pcell);
    virtual void    AddPreset(Preset* preset);
    virtual void    RemovePreset(Preset* preset);
    virtual void    GetParamValue(long index) {};
    virtual long    GetNumPresets() { return 0; };
    virtual void    GetPresetName(long index, char **ppName)    {};
    virtual long    GetPresetIndex(char* name);
    virtual bool    SetPresetByName(char* name);
    virtual bool    SetPresetByIndex(long index) { return true; };
    virtual void    RenamePreset(long index, char* new_name) {};
    virtual void    RenamePreset(char* old_name, char* new_name) {};
    virtual void    SavePresetAs(char *preset_name);
    virtual long    GetNumStoredPresets() { return 0; };
    virtual long    GetNumNativePresets() { return 0; };
    virtual void    ScanForPresets();
    virtual void    DeletePresets();
    virtual bool    SetPreset(Preset* pPreset);
    virtual bool    KillPreset(Preset* pPreset);
    virtual Preset* GetPreset(long index) { return NULL; };
    virtual Preset* GetPreset(char* name) { return NULL; };
    virtual Parameter* GetParamByName(char *param_name);
    virtual Parameter* GetParamByIndex(int index);
    virtual void    DisableParamCells();
    virtual void    EnableParamCells();
    virtual void    SetParamLock(bool lock) { m_ParamLocked = lock; };
    virtual bool    GetParamLock() { return m_ParamLocked; };
            void    DeleteControls();
    virtual void    ForceDeactivate() {};
    virtual void    ToggleParamWindow();
    void EnqueueParamEnvelopeTrigger(Trigger* tg);
    void DequeueParamEnvelopeTrigger(Trigger* tg);
    static String  GetModuleTypeString(ModuleType mtype);
    static String  GetModuleSubTypeString(ModuleSubType subtype);
    static ModuleType  GetModuleTypeFromString(String mtype);
    static ModuleSubType  GetModuleSubTypeFromString(String subtype);
    virtual void CreateModuleWindow() {};
    virtual void SetSampleRate(float fSampleRate) {};
    virtual void SetBufferSize(unsigned int uiBufferSize) {};

private:
    bool    m_ParamLocked;
};


#endif
