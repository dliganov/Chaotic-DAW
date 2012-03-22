#include "AudioEffectx.h"
#include "OfflineEditor.hpp"
#include "OfflineEffect.hpp"

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>

//-----------------------------------------------------------------------------

static int g_useCount = 0;
extern HINSTANCE hInstance;

enum 
{
	kEditorWidth = 400,
	kEditorHeight = 400,
};

const int k_infoText = 1000;

//-----------------------------------------------------------------------------

LONG WINAPI WindowProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_VSCROLL:
	{
		int newValue = SendMessage ((HWND)lParam, TBM_GETPOS, 0, 0);
		OfflineEditor* editor = (OfflineEditor*)GetWindowLong (hwnd, GWL_USERDATA);
		if (editor)
			editor->setValue ((void*)lParam, newValue);
		break;
	}
	case WM_COMMAND:
	{
		switch (HIWORD(wParam))
		{
		case BN_CLICKED:
		{
			OfflineEditor* editor = (OfflineEditor*)GetWindowLong (hwnd, GWL_USERDATA);
			if (editor)
			{
				editor->setValue(0, LOWORD(wParam));
				editor->getEffectX()->offlineRead(0, kVstOfflineQueryFiles);
			}
			break;
		}
		}
		break;
	}
	}
	return DefWindowProc (hwnd, message, wParam, lParam);
}

//-----------------------------------------------------------------------------

OfflineEditor::OfflineEditor (AudioEffectX *effectx)
	:
	AEffEditor(effectx),
	effectx(effectx)
{
	effect->setEditor(this);
}

//-----------------------------------------------------------------------------

OfflineEditor::~OfflineEditor ()
{
}

//-----------------------------------------------------------------------------

long OfflineEditor::getRect (ERect **erect)
{
	static ERect r = {0, 0, kEditorWidth, kEditorHeight };
	*erect = &r;
	return true;
}

//-----------------------------------------------------------------------------

HWND CreateRadioButton(HWND parent, int id, char* title, int x, int y, int w, int h)
{
    HWND hwnd = CreateWindowEx (0, "BUTTON", title,
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        x, y, w, h, parent, (HMENU)id, hInstance, NULL); 

	HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(hwnd, WM_SETFONT, (WPARAM)font, 0);
	return hwnd;
}

//-----------------------------------------------------------------------------

HWND CreateStaticControl(HWND parent, int id, char* title, int x, int y, int w, int h)
{
    HWND hwnd = CreateWindowEx (0, "STATIC", title,
        WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT,
        x, y, w, h, parent, (HMENU)id, hInstance, NULL); 

	HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(hwnd, WM_SETFONT, (WPARAM)font, 0);
	return hwnd;
}

//-----------------------------------------------------------------------------

long OfflineEditor::open (void *ptr)
{
	// Remember the parent window
	systemWindow = ptr;

	// Create window class, if we are called the first time
	g_useCount++;
	if (g_useCount == 1)
	{
		InitCommonControls();
		
		WNDCLASS windowClass;
		windowClass.style = 0;
		windowClass.lpfnWndProc = WindowProc;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = hInstance;
		windowClass.hIcon = 0;
		windowClass.hCursor = 0;
		windowClass.hbrBackground = GetSysColorBrush (COLOR_BTNFACE);
		windowClass.lpszMenuName = 0;
		windowClass.lpszClassName = "OfflineWindowClass";
		ATOM a = RegisterClass (&windowClass);
	}

	// Create our base window
	HWND hwnd = CreateWindowEx (0, "OfflineWindowClass", "",
		WS_CHILD | WS_VISIBLE, 
		0, 0, kEditorWidth, kEditorHeight, 
		(HWND)systemWindow, NULL, hInstance, NULL);
	uiHandle = hwnd;

	SetWindowLong (hwnd, GWL_USERDATA, (long)this);

	int y = 0;
	CreateStaticControl(hwnd, k_infoText, "", 0, y, 300, 40);
	y += 40;
	CreateRadioButton(hwnd, k_ofl_minus6dB, "Minus 6dB", 0,y,300,20);
	y += 20;
	CreateRadioButton(hwnd, k_ofl_reverse, "Reverse", 0,y,300,20);
	y += 20;
	CreateRadioButton(hwnd, k_ofl_reverse2, "Reverse-2", 0,y,300,20);
	y += 20;
	CreateRadioButton(hwnd, k_ofl_maximize, "MaximizeWindow", 0,y,300,20);
	y += 20;
	CreateRadioButton(hwnd, k_ofl_reverse_maximize, "Reverse then MaximizeWindow", 0,y,200,20);
	y += 20;
	CreateRadioButton(hwnd, k_ofl_mix, "Mix", 0,y,300,20);
	y += 20;
	CreateRadioButton(hwnd, k_ofl_cursorMarkerSelection, "Cursor / Marker / Selection", 0,y,300,20);
	y += 20;
	CreateRadioButton(hwnd, k_ofl_parameterRecording, "Parameter recording (invisible)", 0,y,300,20);
	y += 20;
	CreateRadioButton(hwnd, k_ofl_oneEcho, "One Echo with tail", 0,y,300,20);
	y += 20;
	CreateRadioButton(hwnd, k_ofl_halfPitchShift, "Half Pitch Shift", 0,y,300,20);
	y += 20;
	CreateRadioButton(hwnd, k_ofl_interleavedSwap, "Swap channels (Interleaved)", 0,y,300,20);
	y += 20;
	CreateRadioButton(hwnd, k_ofl_useOfTempFile, "Use of temp file", 0,y,300,20);
	y += 20;
	CreateRadioButton(hwnd, k_ofl_copyFile, "Copy file", 0,y,300,20);
	y += 20;
	CreateRadioButton(hwnd, k_ofl_sineGenerator, "Sine generator", 0,y,300,20);

//	int type = (int)effect->getParameter(kType);
//	CheckDlgButton(hwnd, type, true);

	return true;

}

//-----------------------------------------------------------------------------

void OfflineEditor::close ()
{
	g_useCount--;
	if (g_useCount == 0)
	{
		UnregisterClass ("OfflineWindowClass", hInstance);
	}
}

//-----------------------------------------------------------------------------

void OfflineEditor::idle ()
{
	if (updateFlag)
	{
		int type = (int)effect->getParameter(kType);
		for (int t = k_ofl_minus6dB; t < k_ofl_numTypes; t++)
			CheckDlgButton((HWND)uiHandle, t, type == t);
		updateFlag = false;
	}
	AEffEditor::idle ();
}

//-----------------------------------------------------------------------------

void OfflineEditor::setValue(void*, int value)
{
	effect->setParameter(0, (float)value);
}

//-----------------------------------------------------------------------------

// Chance to update the UI

void OfflineEditor::OnHostChange(VstAudioFile* files, long count)
{
	HWND h = (HWND)uiHandle;
	if (h)
	{
		char buf[1000];
		if (! files)
			buf[0] = 0;
		else
			sprintf(buf, "File with focus: %s\n Number of open files: %d", files[0].name, count);
		HWND hc = GetDlgItem(h, k_infoText);
		SetWindowText(hc, buf);
	}
}

//-----------------------------------------------------------------------------
