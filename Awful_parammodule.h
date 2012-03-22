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
    bool        to_be_deleted;
    bool        temp;
    bool        internalModule;

    Preset*     NativePresets;
    Preset*     NP_last;
    long        NumNativePresets;
    long        NumPresets;
    Preset*     CurrentPreset;
    char        preset_path[MAX_PATH_STRING];
    char        current_preset_name[MAX_NAME_STRING]; // we use this string to identify current present on scanning

    Paramcell*  first_pcell;
    Paramcell*  last_pcell;

    Parameter*  first_param;
    Parameter*  last_param;
    int         lastparamidx;

    long        uniqueID;

    char        originalname[MAX_NAME_STRING];
    char        name[MAX_NAME_STRING];
    char        path[MAX_PATH_STRING];

    Scope       scope;

    PluginCommonWindow* wnd; // Internal reference to the window, to delete it first

    ParamWindow* paramWnd;

    ParamModule();
    virtual ~ParamModule();
    virtual void    SetName(char* name)
        {
            if (NULL != name)
            {
                memset(this->name, 0, MAX_NAME_STRING * sizeof(char));
                strncpy(this->name, name, min(MAX_NAME_STRING - 1, strlen(name)));
            }
        }
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

private:
    bool    m_ParamLocked;
};


#endif
