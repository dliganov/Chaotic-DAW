//=================================================================================================
//                             Include Section
//=================================================================================================

#include "awful_params.h"
#include "awful_parammodule.h"
#include <direct.h>
#include <errno.h>

//=================================================================================================
//                                  ParamModule Class Implementation
//=================================================================================================

ParamModule::ParamModule()
{
    to_be_deleted = false;
    temp = false;
    internalModule = true;

    wnd = NULL;
    paramWnd = NULL;

    memset(&scope, 0, sizeof(Scope));
    strcpy(current_preset_name, "Untitled");

    first_param = last_param = NULL;
    first_pcell = last_pcell = NULL;
    lastparamidx = 0;
    this->NativePresets = NULL;
    this->NP_last = NULL;

    this->children = NULL;
    this->parent = NULL;
    this->prev_sibling = NULL;
    this->next_sibling = NULL;

    uniqueID = -1;
    this->CurrentPreset = NULL;
    m_ParamLocked = false;
}

ParamModule::~ParamModule()
{
    if(wnd != NULL)
        MainWnd->DeleteChild(wnd);
    // this->DeleteWindowIfAny(); // Need to do this first of all to avoid asliders receiving messages from already deleted params

    // Delete all paramcells
    Paramcell* pcellnext;
    Paramcell* pcell = first_pcell;
    while(pcell != NULL)
    {
        pcellnext = pcell->next;
        delete pcell;
        pcell = pcellnext;
    }

	// Delete all params
    Parameter* param = this->first_param;
    Parameter* paramnext;
    while (param != NULL)
    {
        paramnext = param->next;
        RemoveParam(param);
        param = paramnext;
    }

    //Kill all children ("Kill All Human"(c) Bender Bending Rodriguez )
    ParamModule* pKid = this->children;
    ParamModule* pTmpKid;
    while (pKid != NULL)
    {
        pTmpKid = pKid;
        pKid = pKid->next_sibling;
        delete pTmpKid;
    }

    DeletePresets();
	DeleteControls();
}

void ParamModule::DeleteControls()
{
	Control* ctrlnext;
	Control* ctrl = firstCtrl;
	while(ctrl != NULL)
	{
		ctrlnext = ctrl->next;
		if(ctrl->scope != NULL && (ctrl->scope == &this->scope))
		{
			ctrl->Deactivate();
			RemoveControlCommon(ctrl);
		}
		ctrl = ctrlnext;
	}
}

void ParamModule::DeletePresets()
{
    // Delete all presets
    Preset *preset, *preset1;
    preset = this->NativePresets;
    while(preset != NULL)
    {
        RemovePreset(preset);
        preset1 = preset;
        preset = preset->next;
        delete preset1;
    }

    this->NativePresets = NULL;
    this->NP_last = NULL;

    NumNativePresets = 0;
    NumPresets = 0;
}

void ParamModule::ScanForPresets()
{
    WIN32_FIND_DATA   founddata                  = {0};
    HANDLE            shandle                    = INVALID_HANDLE_VALUE;
    char              temp_path[MAX_PATH_STRING] = {0};
    char              filename[MAX_PATH_STRING]  = {0};
    Preset           *pPreset                    = NULL;

    if (this->preset_path[0] == 0)
    {
        return;
    }

    /* File with user-saved preset is named in this way: <FX name>'_'<preset name>'.cpf' */
    /* cpf - Chaotic Preset File */

    sprintf(temp_path,"%s%s",this->preset_path, "*.cxml\0");
    shandle = FindFirstFile(temp_path, &founddata);

    if (shandle != INVALID_HANDLE_VALUE)
    {
        do
        {
           sprintf(filename, "%s%s",this->preset_path, founddata.cFileName);

            String  sFilePath(filename);
            File myFile(sFilePath);
            XmlDocument myPreset(myFile);

            XmlElement* xmlMainNode = myPreset.getDocumentElement();

            if(xmlMainNode != NULL)
            {
                /* Sanity check */
                if(xmlMainNode->hasTagName(T("ChaoticPreset"))/* && xmlMainNode->hasAttribute(T("FormatVersion"))*/)
                {
                    pPreset = new Preset((Object*)this);
                    pPreset->index = this->NumNativePresets;
                    XmlElement* xmlHeader = xmlMainNode->getChildByName(T("Module"));

                    if (xmlHeader != NULL)
                    {
                        String Str1 = xmlHeader->getStringAttribute(T("Preset"));
                        Str1.copyToBuffer(pPreset->name, min(Str1.length(), 255));
                        pPreset->native = true;
                        strcpy(pPreset->path, filename);
                        this->AddPreset(pPreset);
                        if(strcmp(pPreset->name, current_preset_name) == 0)
                        {
                            CurrentPreset = pPreset;
                        }
                    }
                }
            }
        }while(FindNextFile(shandle, &founddata));
        FindClose(shandle);
    }
}

void ParamModule::AddParam(Parameter* param)
{
    AddGlobalParam(param);

    param->index = lastparamidx++;
    param->module = this;
    param->scope = &scope;
    param->setEnvDirect(false);
    if(first_param == NULL && last_param == NULL)
    {
        param->prev = NULL;
        param->next = NULL;
        first_param = param;
    }
    else
    {
        last_param->next = param;
        param->prev = last_param;
        param->next = NULL;
    }
    last_param = param;
}

void ParamModule::AddParamcell(Paramcell* pcell)
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

void ParamModule::RemoveParamcell(Paramcell* pcell)
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

Paramcell* ParamModule::AddParamWithParamcell(Parameter* param)
{
    AddParam(param);

    Paramcell* pcell;
    if(param->type == Param_Bool)
    {
        pcell = new Paramcell(PCType_MixToggle, param, NULL, &scope);
    }
    else
    {
        pcell = new Paramcell(PCType_MixSlider, param, NULL, &scope);
    }
    AddParamcell(pcell);
	return pcell;
}

void ParamModule::AddParamToParamcell(Parameter* param, Paramcell* pcell)
{
    AddParam(param);
    pcell->AddParamEntry(param);
}

void ParamModule::AddPreset(Preset* preset)
{
    if(NativePresets == NULL && NP_last == NULL)
    {
        preset->next = NULL;
        preset->prev = NULL;
        NativePresets = preset;
    }
    else
    {
        NP_last->next = preset;
        preset->next = NULL;
        preset->prev = NP_last;
    }
    NP_last = preset;
    ++(this->NumNativePresets);
    ++(this->NumPresets);
}

void ParamModule::RemoveParam(Parameter* param)
{
    RemoveGlobalParam(param);

	if((param == first_param)&&(param == last_param))
	{
		first_param = NULL;
		last_param = NULL;
	}
	else if(param == first_param)
	{
		first_param = param->next;
		first_param->prev = NULL;
	}
	else if(param == last_param)
	{
		last_param = param->prev;
		last_param->next = NULL;
	}
	else
	{
		if(param->prev != NULL)
		{
		    param->prev->next = param->next;
        }
		if(param->next != NULL)
		{
		    param->next->prev = param->prev;
        }
	}

    delete param;
}

void ParamModule::RemovePreset(Preset* preset)
{
    if (preset->native == true)
    {
        if((preset == NativePresets)&&(preset == NP_last))
        {
            NativePresets = NULL;
            NP_last = NULL;
        }
        else if(preset == NativePresets)
        {
            NativePresets = preset->next;
            NativePresets->prev = NULL;
        }
        else if(preset == NP_last)
        {
            NP_last = preset->prev;
            NP_last->next = NULL;
        }
        else
        {
            if(preset->prev != NULL)
            {
                preset->prev->next = preset->next;
            }
            if(preset->next != NULL)
            {
                preset->next->prev = preset->prev;
            }
        }
        --(this->NumNativePresets);
    }
    --(this->NumPresets);
}

/*==================================================================================================
BRIEF:
    Dumps parameters data of the effect and all its children into XML. The method branches a
    child-node from the parent node passed in, and fills it up with its parameters, name and so on.
    This method provides recursive tree traversing approach.

PARAMETERS:
[in]
    xmlParentNode - Reference to a parent node (where to branch from)
    pPresetName   - name of the preset, which should be used in the child-node (optional)

[out]
    none

OUTPUT:
    State of the effect (this) is added into parent xml node

==================================================================================================*/
void ParamModule::SaveStateData(XmlElement & xmlParentNode, char* pPresetName, bool global)
{
    XmlElement  *xmlStateNode = new XmlElement(T("Module"));

    xmlStateNode->setAttribute(T("Name"), this->name);
    xmlStateNode->setAttribute(T("ID"), uniqueID);
    //xmlEffectHeader->setAttribute(T("Type"), this->type);

    if (pPresetName != NULL)
    {
        xmlStateNode->setAttribute(T("Preset"), pPresetName);
    }
    else if (this->CurrentPreset != NULL)
    {
        xmlStateNode->setAttribute(T("Preset"), this->CurrentPreset->name);
    }
    else
    {
        xmlStateNode->setAttribute(T("Preset"), "default");
    }

    Parameter* pParam = this->first_param;
    while (pParam != NULL)
    {
        if(pParam->IsPresetable())
        {
            XmlElement * xmlParam = (global == true ? pParam->Save() : pParam->Save4Preset());

            xmlStateNode->addChildElement(xmlParam);
        }
        pParam = pParam->next;
    }

    //Now let the child-class a chance to save anything it wants as a custom tag
    this->SaveCustomStateData(*xmlStateNode);

    ParamModule* kid = this->children;
    while(kid != NULL)
    {
        kid->SaveStateData(*xmlStateNode, NULL, global);
        kid = kid->next_sibling;
    }

    xmlParentNode.addChildElement(xmlStateNode);
}

/*==================================================================================================
BRIEF:
    Loads parameters of the effect with values from XML node passed in.

PARAMETERS:
[in]
    xmlStateNode - Reference to a state node (where to get parameter values)

[out]
    none

OUTPUT:
    none

==================================================================================================*/
void ParamModule::RestoreStateData(XmlElement & xmlStateNode, bool global)
{
    ParamModule* pChildEffect = NULL;
    Parameter    Param;
    Parameter*   pParam;
    char         szName[MAX_NAME_STRING];
    XmlElement*  xmlChildNode = xmlStateNode.getFirstChildElement();

    //traverse XML tree and load parameters
    //When we meet "module" sub-node, pass it down to a proper child
    while (xmlChildNode != NULL)
    {
        // "Parameter" node we handle right here, it's ours to process
        if(xmlChildNode->hasTagName(T("Parameter")) == true)
        {
            //int idx = xmlChildNode->getIntAttribute(T("index"));
            //pParam = this->GetParamByIndex(idx);

			String name = xmlChildNode->getStringAttribute(T("name"));
            name.copyToBuffer(szName, min(name.length(), MAX_NAME_STRING));
            pParam = this->GetParamByName(szName);

			if(pParam != NULL)
			{
			    if(global)
                    pParam->Load(xmlChildNode);
                else
                    pParam->Load4Preset(xmlChildNode);
			}
        }
        //This sub-node describes a child-effect's state, so pass it down to a child object
        else if (xmlChildNode->hasTagName(T("Module")) == true)
        {
            //Extract module name from XML and convert it to C-style string
            String StrName = xmlChildNode->getStringAttribute(T("name"));
            memset(szName, 0, MAX_NAME_STRING * sizeof(char));
            StrName.copyToBuffer(szName, min(StrName.length(), MAX_NAME_STRING));

            //Go through the children list and find a kid with equal name
            pChildEffect = this->FindChild(szName);

            //If we don't find proper child, just skip the node silently and go furhter
            if (NULL != pChildEffect)
            {
                //Let the child to do its part of the job (delegation is the key to success ;) )
                pChildEffect->RestoreStateData(*xmlChildNode, global);
            }
        }
        //if sub-node has custom tag, we need to give a child-class a chance to process it
        else
        {
            this->RestoreCustomStateData(*xmlChildNode);
        }

        xmlChildNode = xmlChildNode->getNextElement();
    }
}

/*==================================================================================================
BRIEF:
    Saves state of the effect in XML document (preset file) and adds the preset into the list of
    available resets, which the effect holds.

PARAMETERS:
[in]
    preset_name  - Name which should be used for the preset being created

[out]
    none

OUTPUT:
    A new preset-file is created in file system and corresponding preset item is added into 
    effect's preset list
==================================================================================================*/
void ParamModule::SavePresetAs(char * preset_name)
{
    Preset           *pPreset                   = NULL;
    char              path[MAX_PATH_STRING]     = {0};
    char              cur_path[MAX_PATH_STRING] = {0};
    char              working_dir[MAX_PATH_STRING] = {};
    int               preset_index              = 0;
    char*             pCurPos = NULL;
    int               result = 0;
    int               length;
    int               drive = _getdrive();

    //Get current working directory
    _getdcwd(drive, working_dir, MAX_PATH_STRING - 1);

    //save the preset path into path variable
    sprintf_s(path, MAX_PATH_STRING - 1, "%s", preset_path);

    //let's save the length of the working directory, we gonna use it later
    length = strlen(working_dir);

    //concatenate working dir and preset path, now we have an absolute path to preset location 
    //we ignore leading '.' in the path variable, hence copying from an address of the second symbol
    strcat_s(working_dir, MAX_PATH_STRING - 1, &(path[1]));

    //sanity check, if we can't create a dir with this name, then it does already exist 
    //or requires additional parent dirs to be created first
    //result = _mkdir(working_dir);
    result = GetFileAttributes((LPCSTR)working_dir);

    if (result == INVALID_FILE_ATTRIBUTES)
    {
        //set initial position in the absolute path to the end of working directory
        //we gonna go through the rest of the path and check whether the path exists
        pCurPos = &(working_dir[length + 1]);

        //find next/next durectory to examine
        while ((pCurPos = strstr(pCurPos, "\\")) != NULL)
        {
            //length of current processing directory
            length = pCurPos - working_dir;
            strncpy_s(cur_path, MAX_PATH_STRING -1, working_dir, length);

            //try to create the dir
            _mkdir(cur_path);
            memset(cur_path, 0, length);
            ++pCurPos;
        }
    }

    sprintf_s(path, MAX_PATH_STRING - 1, "%s%s%s", preset_path, preset_name, ".cxml\0");

    XmlElement  xmlPresetMain(T("ChaoticPreset"));
    //xmlPresetMain.setAttribute(T("FormatVersion"), MAIN_PROJECT_VERSION);
    this->SaveStateData(xmlPresetMain, preset_name);

    String  sFilePath(path);
    File    myFile(sFilePath);
    xmlPresetMain.writeToFile(myFile, String::empty);

    pPreset = new Preset((Object*)this);
    /* Num is 0-based so its value is equal to the index of the next preset */
    pPreset->index = this->NumNativePresets;
    pPreset->native = true;
    strncpy_s(pPreset->path, MAX_PATH_STRING - 1, path, MAX_PATH_STRING - 1);
    strncpy_s(pPreset->name, MAX_PATH_STRING - 1, preset_name, MAX_PATH_STRING - 1);
    this->AddPreset(pPreset);
    this->CurrentPreset = pPreset;
    strcpy(current_preset_name, pPreset->name);
}

long ParamModule::GetPresetIndex(char* name)
{
    long ret_val = -1;

    if (name != NULL)
    {
        Preset* pPreset = this->NativePresets;

        while (pPreset != NULL)
        {
            if(_stricmp(pPreset->name, name) == 0)
            {
                ret_val = pPreset->index;
                break;
            }
            pPreset = pPreset->next;
        }
    }
    return ret_val;
}

bool ParamModule::SetPresetByName(char * name)
{
    bool ret_val = false;

    if (name != NULL)
    {
        Preset* pPreset = NULL;

        pPreset = this->NativePresets;

        while (pPreset != NULL)
        {
            if(_stricmp(pPreset->name, name) == 0)
            {
                ret_val = this->SetPreset(pPreset);
                break;
            }
            pPreset = pPreset->next;
        }
    }

    return ret_val;
}

bool ParamModule::KillPreset(Preset* pPreset)
{
    RemovePreset(pPreset);
    String  sFilePath(pPreset->path);
    File myFile(sFilePath);
    return myFile.deleteFile();
}

bool ParamModule::SetPreset(Preset* pPreset)
{
    bool ret_val   = false;

    ForceDeactivate();

    String  sFilePath(pPreset->path);

    File myFile(sFilePath);
    XmlDocument myPreset(myFile);

    XmlElement* xmlMainNode = myPreset.getDocumentElement();

    if(NULL == xmlMainNode)
    {
        AlertWindow::showMessageBox(AlertWindow::WarningIcon,
                                        T("Error"),
                                        T("Can't open preset file"),
                                        T("OK"));
    }
    else
    {
        if(xmlMainNode->hasTagName(T("ChaoticPreset"))/* && xmlMainNode->hasAttribute(T("FormatVersion"))*/)
        {
            /*
            if (MAIN_PROJECT_VERSION != xmlMainNode->getIntAttribute(T("FormatVersion")))
            {
                AlertWindow::showMessageBox(AlertWindow::WarningIcon,
                                                T(""),
                                                T("Format version doesn't match. Preset could be loaded incorrectly."),
                                                T("OK"));
            }*/

            try
            {
                //First child is always a master plugin/effect
                XmlElement* xmlMasterHeader = xmlMainNode->getFirstChildElement();

                //Check whether the preset is for right plugin
                if(this->uniqueID != xmlMasterHeader->getDoubleAttribute(T("ID")))
                {
                    AlertWindow::showMessageBox(AlertWindow::WarningIcon,
                                                T("Error"),
                                                T("Preset and plugin doesn't match"),
                                                T("OK"));
                }
                else
                {
                    //Start recursive traversing of XML tree and loading of the preset
                    this->RestoreStateData(*xmlMasterHeader);
                    strcpy(current_preset_name, pPreset->name);
                    ret_val = true;
                }
            }
            catch(...)
            {
                AlertWindow::showMessageBox(AlertWindow::WarningIcon,
                                                T("Error"),
                                                T("Cannot parse preset file"),
                                                T("OK"));
            }
        }
        else
        {
            AlertWindow::showMessageBox(AlertWindow::WarningIcon,
                                            T("Error"),
                                            T("Wrong preset format"),
                                            T("OK"));
        }
    }

    return ret_val;
}

Parameter* ParamModule::GetParamByName(char *param_name)
{
    Parameter * pParam = NULL;

    if(param_name[0] != '\0')
    {
        pParam = this->first_param;

        while((pParam != NULL) && (_stricmp(pParam->name, param_name) != 0))
        {
            pParam = pParam->next;
        }
    }

    return pParam;
}

Parameter* ParamModule::GetParamByIndex(int index)
{
    Parameter * pParam = first_param;
    while(pParam != NULL)
    {
        if(pParam->index == index)
            return pParam;
        pParam = pParam->next;
    }

    return pParam;
}

void ParamModule::DisableParamCells()
{
    Paramcell* pcell = first_pcell;
    while(pcell != NULL)
    {
        pcell->Disable();
        pcell = pcell->next;
    }
}

void ParamModule::EnableParamCells()
{
    Paramcell* pcell = first_pcell;
    while(pcell != NULL)
    {
        if(pcell->visible == true)
        {
            pcell->Enable();
        }
        pcell = pcell->next;
    }
}


/*==================================================================================================
BRIEF:
    This member function is used to add child-effect into master-effect.
    Any Chaotic Effect can be a composition of various effects connected to each other, hence
    producing a new complex effect

PARAMETERS:
[in]
    pEffect - pointer to an Eff object which should be added into children list

[out]
    none

OUTPUT:
    A new child is addded into linked list of all children
==================================================================================================*/
void ParamModule::AddChild(ParamModule* pEffect)
{
    //Link a new kid with other children in the list
    pEffect->next_sibling = this->children;

	if(this->children != NULL)
		this->children->prev_sibling = pEffect;

    //Nullify its back-link
    pEffect->prev_sibling = NULL;

    //re-direct pointer to the children list, now it looks to the new kid
    this->children = pEffect;

    //And the last but not least, set parent link of the effect being added
    pEffect->parent = this;

}

/*==================================================================================================
BRIEF:
    Helper method allowing to remove reference to a child-effect from the children list of 
    master-effect

PARAMETERS:
[in]
    message - pointer to an Eff object, which reference should be removed from the children list

[out]
    none

OUTPUT:
    Referenced effect is removed from the children list of master-effect
==================================================================================================*/
void ParamModule::RemoveChild(ParamModule* pEffect)
{
    ParamModule* pCurrentEffect = this->children;

    //traverse through the list of all children
    while (pCurrentEffect != NULL)
    {
        //found a match
        if (pCurrentEffect == pEffect)
        {
            //Not the last one in the list
            if (pCurrentEffect->next_sibling != NULL)
            {
                pCurrentEffect->next_sibling->prev_sibling = pCurrentEffect->prev_sibling;
            }

            //this is not the first kid in the list
            if (pCurrentEffect->prev_sibling != NULL)
            {
                pCurrentEffect->prev_sibling->next_sibling = pCurrentEffect->next_sibling;
            }
            else //the first kid is a head of the list, we need to update the head then
            {
                this->children = pCurrentEffect->next_sibling;
            }

            //erase parent link, we don't own the effect any more
            pCurrentEffect->parent = NULL;

            //Break is used to stop traversing.
            //We could use bool flag and check it in while() condition but it would be slower
            break;
        }
        pCurrentEffect = pCurrentEffect->next_sibling;
    }
}


/*==================================================================================================
BRIEF:
    Goes through the children list and looks for the kid with specified name.

PARAMETERS:
[in]
    name - Name of the effect to look for

[out]
    none

OUTPUT:
    Returns pointer to a child effect (if found one)

==================================================================================================*/
ParamModule* ParamModule::FindChild(char* name)
{
    ParamModule* pRetEffect = this->children;

    while (pRetEffect != NULL)
    {
        if (_stricmp(pRetEffect->name, name) == 0)
        {
            break;
        }

        pRetEffect = pRetEffect->next_sibling;
    }

    return pRetEffect;
}

void ParamModule::ToggleParamWindow()
{
    if(paramWnd == NULL)
    {
        paramWnd = MainWnd->CreateParamWindow(&scope);
        wnd = paramWnd;
        paramWnd->Show();
    }
    else
    {
        if(paramWnd->isVisible())
        {
            paramWnd->Hide();
        }
        else
        {
            paramWnd->Show();
        }
    }
}

