/*==================================================================================================
                                                                               
    Module Name:  VSTCollection.cpp

    General Description: VSTCollection class implementation. This class represents abstract element
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
#include "VSTCollection.h"
#include "awful_effects.h"
#include "awful_logger.h"

//=================================================================================================
//                             Global/Static Variable Section
//=================================================================================================

//=================================================================================================
//                             Local functions declaration
//=================================================================================================


//=================================================================================================
//                             CChaoticVSTHost Class Implementation
//=================================================================================================

CChaoticVSTHost::CChaoticVSTHost()
{
    lBlockSize = gBuffLen;
    vstTimeInfo.samplePos = 0.0;
    vstTimeInfo.sampleRate = fSampleRate;
    vstTimeInfo.nanoSeconds = 0.0;
    vstTimeInfo.ppqPos = 0.0;
    vstTimeInfo.tempo = beats_per_minute;
    vstTimeInfo.barStartPos = 0.0;
    vstTimeInfo.cycleStartPos = 0.0;
    vstTimeInfo.cycleEndPos = 0.0;
    vstTimeInfo.timeSigNumerator = 4;
    vstTimeInfo.timeSigDenominator = 4;
    vstTimeInfo.smpteOffset = 0;
    vstTimeInfo.smpteFrameRate = 1;
    vstTimeInfo.samplesToNextClock = 0;
    vstTimeInfo.flags = kVstTempoValid | kVstTimeSigValid;
}

void CChaoticVSTHost::SetSampleRate(float fSampleRate)
{
    this->fSampleRate = fSampleRate;
    this->vstTimeInfo.sampleRate = fSampleRate;
}

void CChaoticVSTHost::SetTempo(float fBPM)
{
    this->vstTimeInfo.tempo = fBPM;
}

bool CChaoticVSTHost::OnSetParameterAutomated(int nEffect,long index,float value)
{
    CChaoticEffect *pEffect = (CChaoticEffect*)this->pHost->GetAt(nEffect);

	if (pEffect)
	{
		if (pEffect->owner != NULL)
		{
		 ((VSTEffect*)(pEffect->owner))->OnSetParameterAutomated(nEffect, index, value);
		}
	}

    return true;
    //return CVSTHost::OnSetParameterAutomated(nEffect, index, value);
}
bool CChaoticVSTHost::OnUpdateDisplay(int nEffect)
{
    CChaoticEffect *pEffect = (CChaoticEffect*)this->pHost->GetAt(nEffect);

    if (pEffect)
    {
        if (pEffect->owner != NULL)
        {
            ((VSTEffect*)(pEffect->owner))->OnUpdateDisplay();
        }
    }

    return true;
}

long CChaoticVSTHost::OnIdle(int nEffect)
{
#if (defined (USE_WIN32))
    MSG msg;

    while ((::PeekMessage(&msg, NULL, WM_TIMER, WM_TIMER, PM_REMOVE)) ||
          (::PeekMessage(&msg, NULL, WM_PAINT, WM_PAINT, PM_REMOVE)) ||
          (::PeekMessage(&msg, NULL, WM_NCPAINT, WM_NCPAINT, PM_REMOVE)))
    {
//        if (!::PreTranslateMessage(&msg))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }
#elif (defined (USE_LINUX))
//TODO: Implement this for linux
#elif (defined (USE_MAC))
//TODO: Implement this for MAC
#endif

    return 0;
}

bool CChaoticVSTHost::OnCanDo(const char *ptr)
{
if (!strcmp(ptr, "supplyIdle"))
  return true;
return CVSTHost::OnCanDo(ptr);
}

/*****************************************************************************/
/* OnAudioMasterCallback : redefine the callback a bit                       */
/*****************************************************************************/

long CChaoticVSTHost::OnAudioMasterCallback
    (
    int nEffect,
    long opcode,
    long index,
    long value,
    void *ptr,
    float opt
    )
{
return CVSTHost::OnAudioMasterCallback(nEffect, opcode, index, value, ptr, opt);
}

//=================================================================================================
//                             VST Plugin Class Implementation
//=================================================================================================
CVSTPlugin::CVSTPlugin(Object * owner, int index, CChaoticVSTHost* pHost, void* ParentWindow)
{
    ERect *pRC = NULL;

    this->eff_index = index;
    this->pHost = pHost;
    this->ParentHWND = ParentWindow;
//    this->type = EffType_VSTPlugin;
    this->isReplacing = true;
    this->hasEditor = false;
    this->isWindowActive = false;
    this->isGenerator = false;
    this->pWnd = NULL;

    this->inBufs = NULL;
    this->outBufs = NULL;
    this->nAllocatedInbufs = 0;
    this->nAllocatedOutbufs = 0;
    this->pEffect = (CChaoticEffect*)this->pHost->GetAt(this->eff_index);


    this->pEffect->owner = owner;

	this->pEffect->EffEditGetRect(&pRC);

	if (pRC)
	{
		this->hasEditor = true;
		// no need for releaseing of prc since it points to a memory allocated by plugin
	}

    this->usesChunks = this->pEffect->EffUsesChunks();

	int i, j;  /* loop counter                      */
	if (this->pEffect->pEffect->numInputs) /* allocate input pointers           */
	{
		//We need to allocate input and output buffer arrays on plugin initialization
		this->inBufs = new float *[this->pEffect->pEffect->numInputs];
		for (i = 0; i < this->pEffect->pEffect->numInputs; ++i)
		{
			inBufs[i] = new float[24000];
			for (j = 0; j < 24000; ++j)
			{
				inBufs[i][j] = 0;
			}
		}
		long catg = this->pEffect->EffGetPlugCategory();

		if (catg == kPlugCategGenerator || catg == kPlugCategSynth)
		{
			this->isGenerator = true;
		}
	}
    else
    {
        this->isGenerator = true;
    }

	nAllocatedInbufs = this->pEffect->pEffect->numInputs;

	if (this->pEffect->pEffect->numOutputs)                /* allocate output pointers          */
	{
		int nAlloc;
		if (this->pEffect->pEffect->numOutputs < 2)
			nAlloc = 2;
		else
			nAlloc = this->pEffect->pEffect->numOutputs;
		outBufs = new float *[nAlloc];
                                        /* and the buffers pointed to        */
		for (i = 0; i < this->pEffect->pEffect->numOutputs; i++)
		{
		// for this sample project, I've assumed a maximum buffer size
		// of 1/4 second at 96KHz - presumably, this is MUCH too much
		// and should be tweaked to more reasonable values, since it
		// requires a machine with lots of main memory this way...
		// user configurability would be nice, of course.
			outBufs[i] = new float[24000];
			for (j = 0; j < 24000; j++)
			outBufs[i][j] = 0.f;
		}
		for (; i < nAlloc; ++i)               /* if less than 2, fill up           */
			outBufs[i] = outBufs[0];            /* by replicating buffer 0 pointer   */
		nAllocatedOutbufs = nAlloc;
	}
}

CVSTPlugin::~CVSTPlugin()
{
	int i;
    WaitForSingleObject(this->hMutex,INFINITE);
	if (this->inBufs != NULL)
	{
		for (i = 0; i < this->nAllocatedInbufs; ++i)
		{
			if(this->inBufs[i] != NULL)
			{
				delete((void*)this->inBufs[i]);
				this->inBufs[i] = NULL;
			}
		}
		delete((void *) this->inBufs);
		this->inBufs = NULL;
	}

	for (i = 0; i < this->pEffect->pEffect->numOutputs; ++i)
	{
		if (this->outBufs[i] != NULL)
		{
			delete((void*)this->outBufs[i]);
			this->outBufs[i] = NULL;
		}
	}

	for (; i < this->nAllocatedOutbufs; ++i)
	{
		this->outBufs[i] = NULL;
	}

	delete((void *) this->outBufs);
	this->outBufs = NULL;
	this->pHost->RemoveAt(this->eff_index);

    //if (this->pWnd != NULL)
    //{
    //    delete this->pWnd;
    //}

	ReleaseMutex(this->hMutex);
}


bool CVSTPlugin::ShowEditor()
{
	bool ret_val = false;

	if (this->hasEditor == true)
	{
		if (this->isWindowActive == true)
		{
			this->pWnd->ShowWindow();
			this->pWnd->UpdateWindow();
			this->pWnd->SetFocus();
		}
		else
		{
			//if (this->pWnd != NULL)
			//{
			//	delete (this->pWnd);
			//}

			if (this->pWnd == NULL)
			{
#ifdef USE_JUCE
                this->pWnd = new PluginEditWindow(this->ParentHWND, this);
#else
    			this->pWnd = new CVSTEffWnd(this->pEffect, this->ParentHWND, this);
#endif
			}
			this->pWnd->ShowWindow();
			this->pWnd->UpdateWindow();
			this->pWnd->SetFocus();
			this->isWindowActive = true;
		}
		ret_val = true;
	}

	return ret_val;
}

bool CVSTPlugin::CloseEditor()
{
	this->pEffect->EffEditClose();

	if (this->pWnd != NULL)
	{
		this->pWnd->Close();
	}
    // don't delete it to preserve coordinates of the window
	//delete (this->pWnd);
    //this->pWnd = NULL;

	this->isWindowActive = false;
	return true;
}

void CVSTPlugin::Open()
{
    this->pEffect->EffOpen();
}

void CVSTPlugin::SetSampleRate(float SampleRate)
{
    this->pEffect->EffStopProcess();
    this->pEffect->EffSuspend();

    this->pEffect->EffSetSampleRate(SampleRate);

    this->pEffect->EffResume();
    this->pEffect->EffStartProcess();

}

void CVSTPlugin::Close()
{
    if (this->pEffect != NULL)
    {
        this->pEffect->EffClose();
    }
}

void CVSTPlugin::Idle()
{
    //WaitForSingleObject(this->hMutex,INFINITE);
    this->pEffect->EffIdle();
    //ReleaseMutex(this->hMutex);
}

void CVSTPlugin::EditIdle()
{
    //WaitForSingleObject(this->hMutex,INFINITE);
    this->pEffect->EffEditIdle();
    //ReleaseMutex(this->hMutex);
}
/*==================================================================================================

FUNCTION: ProcessData

DESCRIPTION: This is a wrapper for standart VST ProcessData function.
             input and outpud flows passed in has the following format:
             frame[0], frame[1],...,frame[buff_size-1]
             Where frame is: float(L), float(R) pair.

             Currently process function supports only mono and stereo data to in and out.
             All extra channels will be ignored or filled in by duplication of first two channels.

ARGUMENTS PASSED:
   in_buff   - pointer to the data to be processed (in internal program format)
   out_buff  - pointer to the processed data (in internal program format)
   buff_size - num of data frames (pair of L & R channel value) in incoming stream buffer

RETURN VALUE:

PRE-CONDITIONS:
   None.

POST-CONDITIONS:
   None.

==================================================================================================*/
void CVSTPlugin::ProcessData(float* in_buff, float* out_buff, int buff_size)
{
	int i,j;
    WaitForSingleObject(this->hMutex,INFINITE);
	//If effect has only one input then mix both L and R to one mono channel
    if (in_buff != NULL)
	{
		if (this->nAllocatedInbufs == 1)
		{
			for (j = 0; j < buff_size; ++j)
			{
				this->inBufs[0][j] = (in_buff[j*2] + in_buff[j*2+1])/2;
			}
		}
		// if effect has more than one input
		else if (this->nAllocatedInbufs > 1)
		{
			for (j = 0; j < buff_size; ++j)
			{
				//fill in only first two channels
				this->inBufs[0][j] = in_buff[j*2];
				this->inBufs[1][j] = in_buff[j*2+1];
			}
			//all extra inputs will be duplicated to firts input (we don't support them)
			for (i = 2; i < this->nAllocatedInbufs; ++i)
			{
				this->inBufs[i] = this->inBufs[0];
			}
		}
	}
	//sanity action: just clear output buffer
	memset(out_buff, 0, (sizeof(float)*2)*buff_size);

	//Actually, plugins can support two data processing methods:
	// First is Process - out += process(in)
	// Second is ProcessReplacing - out = process(in)
	if ((this->pEffect->pEffect->flags & effFlagsCanDoubleReplacing)&& (this->isReplacing == true))
    {
        /* this->pEffect->EffProcessDoubleReplacing(
            (double *)this->inBufs,
            (double *)this->outBufs,
            buff_size);
        */
    }
	else if ((this->pEffect->pEffect->flags & effFlagsCanReplacing) && (this->isReplacing == true))
	{
		this->pEffect->EffProcessReplacing(this->inBufs, this->outBufs, buff_size);
	}
	else
	{
		this->pEffect->EffProcess(this->inBufs, this->outBufs, buff_size);
	}

	for (i = 0; i < buff_size; ++i)
	{
		//lets duplicate output if effect has only one
		if (this->nAllocatedOutbufs == 1)
		{
			out_buff[i*2] = out_buff[i*2+1] = this->outBufs[0][i];
		}
		else if (this->nAllocatedOutbufs > 1)//effect has more than one buffer
		{
			//use only first two channels and ignore all other if any
			out_buff[i*2] = this->outBufs[0][i];
			out_buff[i*2+1] = this->outBufs[1][i];
		}
	}
	ReleaseMutex(this->hMutex);
}

void CVSTPlugin::SetParam(long index, float Value)
{
    this->pEffect->EffSetParameter(index, Value);
}

float CVSTPlugin::GetParam(long index)
{
    return (this->pEffect->EffGetParameter(index));
}
unsigned int CVSTPlugin::GetNumParams()
{
    return (this->pEffect->pEffect->numParams);
}
void CVSTPlugin::GetDisplayValue(long index, char** ppValDisp)
{
	if (ppValDisp != NULL)
	{
		if (*ppValDisp != NULL)
		{
			free(*ppValDisp);
		}

		*ppValDisp = (char*)malloc(sizeof(char) * VST_MAX_PARAM_VALUE_LENGTH);
        memset(*ppValDisp, 0, VST_MAX_PARAM_VALUE_LENGTH);
		this->pEffect->EffGetParamDisplay(index, *ppValDisp);
	}
}
void CVSTPlugin::GetParamLabel(long index, char **ppLabel)
{
	if (ppLabel != NULL)
	{
		if (*ppLabel != NULL)
		{
			free(*ppLabel);
		}

       *ppLabel = (char*)malloc(sizeof(char) * VST_MAX_PARAM_LABEL_LENGTH);
        memset(*ppLabel, 0, VST_MAX_PARAM_LABEL_LENGTH);
        this->pEffect->EffGetParamLabel(index, *ppLabel);
    }
}

void CVSTPlugin::GetParamName(long index, char** ppName)
{
	if (ppName != NULL)
	{
		if (*ppName != NULL)
		{
			free(*ppName);
		}

		*ppName = (char*) malloc(sizeof(char) * VST_MAX_NAME_LENGTH);
		this->pEffect->EffGetParamName(index, *ppName);
	}
}

/* If length is 0 it means we must return whole name without extension */
void CVSTPlugin::GetDisplayName(char *name, unsigned int length)
{
	char szBuf[256]="";
	size_t len = 0;

	WaitForSingleObject(this->hMutex,INFINITE);

	this->pEffect->EffGetProductString(szBuf);
	if (strlen(szBuf) == 0)
	{
		size_t i = strlen(this->pEffect->sName);
		unsigned int j = 0;
		bool         dots = false;
        /* Go backward till find a dot in file name */
        while (this->pEffect->sName[i--] != '.');

        /* Go backward until reach the first "\\" */
        while (this->pEffect->sName[i--] != '\\')
        {
            ++len; // Here we calc length of file name (without extension)
        }

        i+=2;

        if ((length != 0) && (len > length))
        {
            len = length - 3; // reserve space for three dotes in the end
            dots = true;
        }

        j = 0;
        while ((len--)!= 0)
        {
            name[j++] = this->pEffect->sName[i++];
        }

        if (dots == true)
        {
            strcat(name, "...");
        }
    }
    else
    {
        len = strlen(szBuf);

        if (length != 0)
        {
            len = (length > strlen(szBuf) ? strlen(szBuf) : length);
        }
        strncpy(name, szBuf, len);
    }
	ReleaseMutex(this->hMutex);
}

long CVSTPlugin::GetNumPresets()
{
    return (this->pEffect->pEffect->numPrograms);
}

void CVSTPlugin::GetProgramName(char *pName)
{
    this->pEffect->EffGetProgramName(pName);
}

long CVSTPlugin::GetProgram()
{
    return this->pEffect->EffGetProgram();
}

void CVSTPlugin::SetProgram(long index)
{
    void * pChunk = NULL;

    this->pEffect->EffSetProgram(index);
    this->pEffect->EffGetChunk(&pChunk, true);

    if ((this->pWnd != NULL) && (this->isWindowActive == true))
    {
        this->pWnd->SetTitle();
    }
}

void CVSTPlugin::ProcessEvents(VstEvents* pEvents)
{
    this->pEffect->EffProcessEvents(pEvents);
}

void CVSTPlugin::SetBPM(float bpm)
{
    //CChaoticEffect* pEffect = this->pEffect;
}

void CVSTPlugin::SetBufferSize(unsigned int uiBufferSize)
{
    CChaoticEffect* pEffect = this->pEffect;

    pEffect->EffStopProcess();
    pEffect->EffSuspend();

    pEffect->EffSetBlockSize(uiBufferSize);

    pEffect->EffResume();
    pEffect->EffStartProcess();
}

void CVSTPlugin::GetErrorText()
{
    CChaoticEffect* pEffect = this->pEffect;

    char ptrtext[256];
    pEffect->EffGetErrorText(ptrtext);
}

void CVSTPlugin::TurnOffProcessing()
{
    //That's strange - VST SDK says that Processing should be stopped
    // However, I noticed that it causes FX's and synths to keep their processing state
    // So, when you restart playback for example, you can hear that synthesis continues but not starts from beginning
    //pEffect->EffStopProcess();
    pEffect->EffSuspend();
}

void CVSTPlugin::TurnOnProcessing()
{
    pEffect->EffResume();
    //pEffect->EffStartProcess();
}

//When build with JUCE, we use juce's windows
#ifndef USE_JUCE
//=================================================================================================
//                             VST Effect Window Class Implementation
//=================================================================================================
CVSTEffWnd::CVSTEffWnd(CChaoticEffect* pEffect, void* ParentWnd, CVSTPlugin* pPlug)
{
    ERect *prc     = NULL;
    ERect  myRC = {0};
    long   lResult = 0;

    this->pPlugin = pPlug;
    this->pEffect = pEffect;
    this->ParentHWND = ParentWnd;
    this->height = 0;
    this->width = 0;
    lResult = this->pEffect->EffEditGetRect(&prc);

    if (prc)
    {
        myRC = *prc;
        myRC.bottom += GetSystemMetrics(SM_CYCAPTION) + (2 * GetSystemMetrics(SM_CYBORDER));
        myRC.right += (2 * GetSystemMetrics(SM_CXBORDER));
        this->width = myRC.right - myRC.left;
        this->height = myRC.bottom - myRC.top;


		this->hWndHandle = CreateWindowEx(WS_EX_DLGMODALFRAME | WS_EX_DLGMODALFRAME |\
                                    WS_EX_CONTROLPARENT,
									"_VSTEditor",
									"Plugin",
									//WS_CHILD |
									WS_CLIPSIBLINGS |\
									WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
									myRC.left, myRC.top,
									myRC.right - myRC.left, myRC.bottom - myRC.top,
									this->ParentHWND,
									NULL,
									NULL,
									NULL);

        SetWindowLongPtr(this->hWndHandle, GWL_USERDATA, (LONG_PTR)this);

        this->SetTitle();
    }
}

void CVSTEffWnd::SetTitle()
{
	if (this->pEffect != NULL)
	{
		char szBuf[256] = "";
		unsigned int len = 0;
		/* if V2 plugin */
		this->pEffect->EffGetProductString(szBuf);
		if (strlen(szBuf) == 0)
		{
			unsigned long i = strlen(this->pEffect->sName);
			unsigned int j = 0;
			while (this->pEffect->sName[i--] != '\\' );
			i += 2;
			while (i < strlen(this->pEffect->sName))
			{
			    szBuf[j++] = this->pEffect->sName[i++];
			}
			szBuf[j] = ' ';
			len = j;
		}

		char szProg[256] = {0};
		this->pEffect->EffGetProgramName(szProg);
		if ( (len < 255) && ((255 - len) >= strlen(szProg) + 3))
		{
		    strcat(szBuf," - ");
		    strcat(szBuf, szProg);
		}
		//::SetWindowText(this->hWnd, szBuf);
	}
}

//=================================================================================================
//                             WND PROC used by VSt Edit window class

//Description: Static members cannot have pointer "this" but we need it in order to properly update
//             isWindowActive. This flag is used to prevent multiple Edit windows for one plugin.
//             Every window has special field for various user data to store. So, we can save 
//             pointer to this in this location during window creation and further extract it.
//=================================================================================================
LRESULT CALLBACK CVSTEffWnd::VSTWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	HWND ParentHWND;
    CVSTEffWnd* pThis; // Fake pointer to this

    //Let's extract previously stored "this" pointer
    pThis = (CVSTEffWnd*) GetWindowLongPtr(hWnd, GWL_USERDATA);
    ParentHWND = GetParent(hWnd);

    switch (message)        // “ип сообщени€
    {
        case WM_CREATE:
//          InvalidateRect(hWnd, NULL, false);
            SetTimer(hWnd, IDLE_TIMER, IDLE_TIMER_INTERVAL, NULL);
            break;
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code here...
            EndPaint(hWnd, &ps);
            break;
//        case WM_DESTROY:
        case WM_CLOSE:
            KillTimer(hWnd, IDLE_TIMER);
            pThis->pEffect->EffEditClose();
            pThis->pPlugin->isWindowActive = false;
            //return (DefWindowProc(hWnd, message, wParam, lParam));
            SendMessage((HWND)pThis->ParentHWND, WM_VST_PLUGEDITOR_CLOSED, 0, (LPARAM)(pThis->pPlugin));
            DestroyWindow(hWnd);
            break;
//        case WM_MOUSEWHEEL:
//            break;
        case WM_CHAR:
        case WM_KEYDOWN:
        case WM_KEYUP:
           SendMessage(ParentHWND, message,wParam,lParam);
           break;
        case WM_SIZE:
            /* Need this, it enables parent window when eff edit is minimized */
            if (wParam == SIZE_MINIMIZED)
            {
                ::ShowWindow(ParentHWND, SW_SHOW);
            }
            break;
        case WM_SHOWWINDOW:
        {
             ERect *prc  = NULL;
             ERect  myRC = {0};
             if(pThis != NULL)
             {
                 pThis->pEffect->EffEditGetRect(&prc);
                 if (prc)
                 {
                     RECT rgn = {0};
                     unsigned int x_pos, y_pos;

                     ::GetWindowRect((HWND)pThis->ParentHWND, &rgn);
                     myRC = *prc;
                     myRC.bottom += GetSystemMetrics(SM_CYCAPTION) + (2 * GetSystemMetrics(SM_CYBORDER));
                     myRC.right += (2 * GetSystemMetrics(SM_CXBORDER));
                     x_pos = ((rgn.right - rgn.left) - (myRC.right - myRC.left))/2;
                     y_pos = ((rgn.bottom - rgn.top) - (myRC.bottom - myRC.top))/3;
                    ::SetWindowPos(hWnd, HWND_TOPMOST, x_pos,y_pos, myRC.right - myRC.left, myRC.bottom - myRC.top,
                                   SWP_NOACTIVATE | SWP_NOZORDER);
                 }
             }
         }
         break;
        case WM_TIMER:
        {
            switch(wParam)
            {
                case IDLE_TIMER:
                {
                    pThis->pPlugin->Idle();
                    pThis->pPlugin->EditIdle();
                }
                break;
            }
        break;
        }

        default:
        return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (0);
}
#endif // USE_JUCE

//=================================================================================================
//                             VST Collection Class Implementation
//=================================================================================================
VSTCollection::VSTCollection(void* MainWindowHandle)
{
    this->pMainHost = new CChaoticVSTHost();
    this->pMainHost->SetBlockSize(gBuffLen);
    this->pMainHost->SetSampleRate(fSampleRate);
    this->pMainHost->SetTempo(beats_per_minute);
    this->ParentHWND = MainWindowHandle;
    this->hMutex = CreateMutex(NULL, FALSE, NULL);

#ifdef USE_WIN32
    #ifndef USE_JUCE

    //  ласс VST-окна, который будет использоватьс€ дл€ отрисовки встроенных в VST плагины едиторов
    WNDCLASS        vst_wc;

    //«аполн€ем пол€ структуры
    vst_wc.style                = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    vst_wc.lpfnWndProc          = (WNDPROC) (CVSTEffWnd::VSTWndProc);
    vst_wc.cbClsExtra           = 0;
    vst_wc.cbWndExtra           = 0;
    vst_wc.hInstance            = NULL;
    vst_wc.hIcon                = NULL;
    vst_wc.hCursor              = NULL;
    vst_wc.hbrBackground        = NULL;
    vst_wc.lpszMenuName         = NULL;
    vst_wc.lpszClassName        = "_VSTEditor";

    //регистрируем новый класс.
    if(!RegisterClass(&vst_wc))
    {
        MessageBox(0,"Failed To Register The Window Class.Damn it! Fuck it all!!!",
            "Error",MB_OK|MB_ICONERROR);
    }

    #endif
#endif
}

VSTCollection::~VSTCollection()
{
	delete this->pMainHost;
	ReleaseMutex(this->hMutex);
}
void VSTCollection::AcquireSema()
{
    WaitForSingleObject(this->hMutex,INFINITE);
}

void VSTCollection::ReleaseSema()
{
    ReleaseMutex(this->hMutex);
}

CVSTPlugin* VSTCollection::LoadPlugin(Object* owner, char* path)
{
    int index = 0;
    CVSTPlugin *pPlugin = NULL;
//    OPENFILENAME ofn;       // common dialog box structure
    char szFile[260];       // buffer for file name

    if (path == NULL)
    {
        String strPlugDir(szWorkingDirectory);

        strPlugDir += LOCAL_PLUGIN_FOLDER;

        juce::File defDir(strPlugDir);

        //Let's show a dialog and allow user to choose a directory
        FileChooser *pFileSelector = new FileChooser(T("Open a file"),
                                                     defDir, T("*.dll;*.DLL"), true);
        if (true == pFileSelector->browseForFileToOpen())
        {
            juce::File vstFile = pFileSelector->getResult();

            vstFile.getFullPathName().copyToBuffer(szFile, 260 - 1);

            index = pMainHost->LoadPlugin(szFile);

            if (index != -1)
            {
                pPlugin = new CVSTPlugin(owner, index, this->pMainHost, this->ParentHWND);

                if (NULL == pPlugin)
                {
                    MessageBox(0,"Internal error occured on plugin loading.",
                               "Internal error",MB_OK|MB_ICONERROR);
                }
                else
                {
                    pPlugin->hMutex = this->hMutex;
                }
            }
            else
            {
                AlertWindow::showMessageBox(AlertWindow::InfoIcon,
                                            T(""),
                                            T("Can't load this dll as a plugin."),
                                            T("OK"));

            }

            delete pFileSelector;
        }
    }
    else
    {
        index = pMainHost->LoadPlugin(path);

        if (index != -1)
        {
            pPlugin = new CVSTPlugin(owner, index, this->pMainHost, this->ParentHWND);

            if (NULL == pPlugin)
            {
                MessageBox(0,"Internal error occured on plugin loading.",
                    "Internal error",MB_OK|MB_ICONERROR);
            }
            else
            {
                pPlugin->hMutex = this->hMutex;
            }
        }
        else
        {
/*
            AlertWindow::showMessageBox(AlertWindow::InfoIcon,
                                        T(""),
                                        T("Can't load this dll as a plugin."),
                                        T("OK"));
*/
        }
    }

    return pPlugin;
}

void VSTCollection::RemovePlugin(CVSTPlugin *pPlug)
{
    if (pPlug != NULL)
    {
        if (pPlug->isWindowActive == true)
        {
            pPlug->CloseEditor();
        }
//        pPlug->Close();
        delete pPlug;
        pPlug = NULL;
    }
}

bool VSTCollection::CheckPlugin(char *path, bool *isGenerator, char* name)
{
    bool ret_val = false;
    long index   = 0;

    index = this->pMainHost->LoadPlugin(path);

    if (index != -1)
    {
        CEffect *pEff = this->pMainHost->GetAt(index);

        memset(name, 0, MAX_NAME_STRING * sizeof(char));

        pEff->EffGetProductString(name);
        if (name[0] == 0)
        {
            pEff->EffGetEffectName(name);
        }

        if (pEff->pEffect->numInputs != 0)
        {
            long catg = pEff->EffGetPlugCategory();

            if((catg == kPlugCategSynth) || (catg == kPlugCategGenerator))
            {
                *isGenerator = true;
            }
            else
            {
                *isGenerator = false;
            }
        }
        else
        {
            *isGenerator = true;
        }

        this->pMainHost->RemoveAt(index);
        ret_val = true;
    }

    return ret_val;
}

