
#include "awful.h"
#include "awful_audio.h"
#include "awful_instruments.h"
#include "awful_elements.h"
#include "awful_paramedit.h"
#include "awful_controls.h"
#include "awful_panels.h"
#include "awful_cursorandmouse.h"
#include "awful_utils_common.h"
#include "awful_preview.h"

extern float    NoteToFreq(int note);
extern void     PanConstantRule(float pan, float* volL, float* volR);
extern void     PanLinearRule(float pan, float* volL, float* volR);
extern void     CheckTempDrawareas();
extern void     CheckButtonsRepaint();
extern void     GetOriginalInstrumentAlias(const char* name, char* alias);
extern bool     CheckMixcellLoop(Mixcell* mc, Mixcell* outmc);
extern Mixcell* GetMixcellFromFXString(DigitStr* dfxstr);
extern bool     IsPianoKeyPressed(int pkeynum);
extern bool     CheckPlaneCrossing(int x1, int y1, int x2, int y2, int a1, int b1, int a2, int b2);
extern void     AuxCheck();
extern int      Tick2X(float tick, Loc loc);
extern float    X2Tick(int x, Loc loc);
extern int      Line2Y(int line, Loc loc);
extern int      Y2Line(int y, Loc loc);
extern int      Gx(int xc, Loc loc);
extern int      Gy(int yc, Loc loc);
extern void     LowerCase(char* data);
extern void     UpperCase(char* data);
extern void     Get_FileName(char* source, char* dest);
extern char*    GetOriginalPatternName();
extern void     Num2String(long num, char* string);
extern void     ParamNum2String(long num, char* string);
extern long     ParamString2Num(char* string);
extern Symbol	GetSymbolFromString(char* data);
extern AliasRecord*   GetAliasRecordFromString(char* str);
extern Instrument*    CheckStringForInstrumentAlias(char* string);
extern float    CalcSampleFreqIncrement(Sample* sample, int semitones);
extern float    Calc_SlideMultiplier(unsigned long frame_length, int semitones);
extern float    CalcFreqRatio(int semitones);
extern long     Tick2Frame(float tick);
extern float    Tick2Second(float tick);
extern float    Frame2Tick(unsigned long frame);
extern float    Frame2Tick(double frame);
extern inline float Interpolate_Line(double x1, float y1, double x2, float y2, double x3);
extern int      Calc_PixLength(long num_frames, long sample_rate, float tickwidth);
extern float    Calc_PercentPan(float pan_percent);
extern inline float    Div100(float percent);
extern void     Vanish(Element* el);
extern void     Vanish_CleanUp();
extern int      RoundDouble(double val);
extern int      RoundFloat(float val);
extern int      MapKeyToNote(char character, bool relative);
extern CmdType  MapCNum2CType(int com_num);
extern int      MapCType2CNum(CmdType command);
extern float    MapItem2Quant(MItemType itype);
extern void     CopyImageSection(Image* img1, Image* img2, int x1, int y1, int x2, int y2, int w, int h);
extern Pattern* CheckStringForPatternName(char* string);
extern float    GetVolOutput(float val);
extern float    Second2Tick(float sec);

int MapKeyToNote(char character, bool relative)
{
    int oct;
    if(!relative)
    {
        oct = Octave;
    }
    else
    {
        oct = relOctave;
    }

    LowerCase(&character);
    int note = -1000;
    int base0 = oct * 12 - 12;
    int base1 = oct * 12;

    switch(character)
    {
        // Upper row:
        case 0x71:
            note = base1 + 0;
            break;
        case 0x32:
            note = base1 + 1;
            break;
        case 0x77:
            note = base1 + 2;
            break;
        case 0x33:
            note = base1 + 3;
            break;
        case 0x65:
            note = base1 + 4;
            break;
        case 0x72:
            note = base1 + 5;
            break;
        case 0x35:
            note = base1 + 6;
            break;
        case 0x74:
            note = base1 + 7;
            break;
        case 0x36:
            note = base1 + 8;
            break;
        case 0x79:
            note = base1 + 9;
            break;
        case 0x37:
            note = base1 + 10;
            break;
        case 0x75:
            note = base1 + 11;
            break;
        case 0x69:
            note = base1 + 12;
            break;
        case 0x39:
            note = base1 + 13;
            break;
        case 0x6F:
            note = base1 + 14;
            break;
        case 0x30:
            note = base1 + 15;
            break;
        case 0x70:
            note = base1 + 16;
            break;

        // Lower row:
        case 0x7A:
            note = base0 + 0;
            break;
        case 0x73:
            note = base0 + 1;
            break;
        case 0x78:
            note = base0 + 2;
            break;
        case 0x64:
            note = base0 + 3;
            break;
        case 0x63:
            note = base0 + 4;
            break;
        case 0x76:
            note = base0 + 5;
            break;
        case 0x67:
            note = base0 + 6;
            break;
        case 0x62:
            note = base0 + 7;
            break;
        case 0x68:
            note = base0 + 8;
            break;
        case 0x6E:
            note = base0 + 9;
            break;
        case 0x6A:
            note = base0 + 10;
            break;
        case 0x6D:
            note = base0 + 11;
            break;
        case 0x2C:
            note = base0 + 12;
            break;
        case 0x6C:
            note = base0 + 13;
            break;
        case 0x2E:
            note = base0 + 14;
            break;
        case 0x3B:
            note = base0 + 15;
            break;
    }
    return note;
}

CmdType MapCNum2CType(int com_num)
{
    CmdType type;
    switch(com_num)
    {
        case 0:
            type = Cmd_Default;
            break;
        case 1:
            type = Cmd_Vol;
            break;
        case 2:
            type = Cmd_Pan;
            break;
        case 3:
            type = Cmd_Mute;
            break;
        case 4:
            type = Cmd_Solo;
            break;
        case 5:
            type = Cmd_VolEnv;
            break;
        case 6:
            type = Cmd_PanEnv;
            break;
        default:
            break;
    }
    return type;
}

int MapCType2CNum(CmdType command)
{
    int num = 0;
    switch(command)
    {
        case Cmd_Default:
            num = 0;
            break;
        case Cmd_Vol:
            num = 1;
            break;
        case Cmd_Pan:
            num = 2;
            break;
        case Cmd_Mute:
            num = 3;
            break;
        case Cmd_Solo:
            num = 4;
            break;
        case Cmd_VolEnv:
            num = 5;
            break;
        case Cmd_PanEnv:
            num = 6;
            break;
        case Cmd_LocVolEnv:
            num = 5;
            break;
        case Cmd_LocPanEnv:
            num = 6;
            break;
        default:
            break;
    }
    return num;
}

float MapItem2Quant(MItemType itype)
{
    switch(itype)
    {
        case MItem_Step:
            return 1;
            break;
        case MItem_Step12:
            return 0.5;
            break;
        case MItem_Step14:
            return 0.25;
            break;
        case MItem_Step13:
            return 1.0f/3.0f;
            break;
        case MItem_Step16:
            return 1.0f/6.0f;
            break;
        case MItem_Beat:
            return (float)ticks_per_beat;
            break;
        case MItem_Bar:
            return float(ticks_per_beat*beats_per_bar);
            break;
        case MItem_None:
            return -1.f;
            break;
        default:
            return 1;
    }
}

/*==================================================================================================
BRIEF: Converts tick into x-coordinate (app window relative).

PARAMETERS:
[IN]
    tick - tick value
    loc - grid location
[OUT]
N/A

OUTPUT: x-coordinate
==================================================================================================*/
int Tick2X(float tick, Loc loc)
{
    if(loc == Loc_MainGrid)
    {
        //return ApproxFloat((tick - OffsTick)*tickWidth + GridX1);
		return RoundFloat(tick*tickWidth) - RoundFloat(OffsTick*tickWidth) + GridX1;
    }
    else if(loc == Loc_SmallGrid)
    {
        //return ApproxFloat((tick - gAux->OffsTick)*gAux->tickWidth + GridXS1);
		return RoundFloat(tick*gAux->tickWidth) - RoundFloat(gAux->OffsTick*gAux->tickWidth) + GridXS1;
    }
    else
    {
        return 0;
    }
}

/*==================================================================================================
BRIEF: Converts x-coordinate (app window relative) into tick.

PARAMETERS:
[IN]
    x - x-coordinate
    loc - grid location
[OUT]
N/A

OUTPUT: tick value
==================================================================================================*/
float X2Tick(int x, Loc loc)
{
    if(loc == Loc_MainGrid)
    {
        return ((float)x - GridX1)/tickWidth + OffsTick;
    }
    else if(loc == Loc_SmallGrid)
    {
        return ((float)x - GridXS1)/gAux->tickWidth + gAux->OffsTick;
    }
    else if(loc == Loc_StaticGrid)
    {
        return ((float)x - StX1)/st_tickWidth + st->tick_offset;
    }
    else
    {
        return 0;
    }
}

/*==================================================================================================
BRIEF: Converts line into y-coordinate (app window relative).

PARAMETERS:
[IN]
    line - line number
    loc - grid location
[OUT]
N/A

OUTPUT: y-coordinate
==================================================================================================*/
int Line2Y(int line, Loc loc)
{
    if(loc == Loc_MainGrid)
    {
        return ((line - OffsLine)*lineHeight + lineHeight + GridY1);
    }
    else if(loc == Loc_SmallGrid)
    {
        return ((line - gAux->OffsLine)*gAux->lineHeight + gAux->lineHeight + GridYS1);
    }
    else
    {
        return 0;
    }
}

/*==================================================================================================
BRIEF: Converts y-coordinate (app window relative) into line.

PARAMETERS:
[IN]
    y - y-coordinate
    loc - grid location
[OUT]
N/A

OUTPUT: line number
==================================================================================================*/
int Y2Line(int y, Loc loc)
{
    if(loc == Loc_MainGrid)
    {
        return (y - GridY1)/lineHeight + OffsLine;
    }
    else if(loc == Loc_SmallGrid)
    {
        return (y - GridYS1)/gAux->lineHeight + gAux->OffsLine;
    }
    else
    {
        return 0;
    }
}

/*==================================================================================================
BRIEF: Converts x-coordinate relative to grid window into absolute grid x-coordinate.

PARAMETERS:
[IN]
    xc - x-coordinate relative to grid window
    loc - grid location
[OUT]
N/A

OUTPUT: absolute grid x-coordinate
==================================================================================================*/
int Gx(int xc, Loc loc)
{
    if(loc == Loc_MainGrid)
    {
        return (int)(xc + OffsTick * tickWidth - GridX1);
    }
    else if(loc == Loc_SmallGrid)
    {
        return (int)(xc + gAux->OffsTick*gAux->tickWidth - GridXS1);
    }
    else if(loc == Loc_StaticGrid)
    {
        return (int)(xc + st->tick_offset * st_tickWidth - StX1);
    }
    else
    {
        return 0;
    }
}

/*==================================================================================================
BRIEF: Converts y-coordinate relative to grid window into absolute grid y-coordinate.

PARAMETERS:
[IN]
    yc - y-coordinate relative to grid window
    loc - grid location
[OUT]
N/A

OUTPUT: absolute grid y-coordinate
==================================================================================================*/
int Gy(int yc, Loc loc)
{
    if(loc == Loc_MainGrid)
    {
        return yc + OffsLine * lineHeight - GridY1;
    }
    else if(loc == Loc_SmallGrid)
    {
        return yc + gAux->OffsLine * gAux->lineHeight - GridYS1;
    }
    else
    {
        return 0;
    }
}

//
////
///////////
////////////////////////// / / /
// Function LowerCase(char* data) - transforms all big letters into small ones
void LowerCase(char* data)
{
    int ic, len, code;
    len = strlen(data);

    for(ic = 0; ic < len; ic++)
    {
        code = (short)data[ic];
        if(code >= 0x41 && code <= 0x5A)
        {
            code += 0x20;
            data[ic] = code;
        }
    }
}

//
////
///////////
////////////////////////// / / /
// Function LowerCase(char* data) - transforms all big letters into small ones
void UpperCase(char* data)
{
    int ic, len, code;
    len = strlen(data);

    for(ic = 0; ic < len; ic++)
    {
        code = (short)data[ic];
        if(code >= 0x61 && code <= 0x7A)
        {
            code -= 0x20;
            data[ic] = code;
        }
    }
}

//
//////
//////////////////////
////////////
///////
/////
////
//
//
///
/////
////////
//////////
void Get_FileName(char* source, char* dest)
{
    int ic, len, code;

    len = strlen(source);

    for(ic = 0; ic < len; ic++)
    {
        code = source[ic];

        if((code >= 0x41 && code <= 0x5A)||
           (code >= 0x61 && code <= 0x7A))
        {
            dest[ic] = code;
        }
        else
        {
            break;
        }
    }
    dest[ic] = 0;
}

char* GetOriginalPatternName()
{
    char* n = new char[5];  // TODO: overflow zone
    memset(n, 0, 5);

    //n[0] = 'p';
    //n[1] = 't';

    int index = 1;
    Num2String(index, (char*)&n[0]);

    Element* el = firstElem;
    while(el != NULL)
    {
        if(el->IsPresent() && el->type == El_Pattern)
        {
            if(strcmp(n, ((Pattern*)el)->name->string) == 0)
            {
                index++;
                Num2String(index, (char*)&(n[0]));
				el = firstElem;
                continue;
			}
		}
		el = el->next;
    }
    return n;
}

Pattern* CheckStringForPatternName(char* string)
{
    Element* el = firstElem;
    while(el != NULL)
    {
        if(el->IsPresent() && el->type == El_Pattern)
        {
            if(strcmp(string, ((Pattern*)el)->name->string) == 0)
            {
                return (Pattern*)el;
            }
        }
        el = el->next;
    }
    return NULL;
}

void Num2String(long num, char* string)
{
    long    count_digits = 100000;
    long    count_num;
    short   digit;
    char    char_digit;
    char*   p_string = string;
    bool    start = false;

    count_num = num;

    while(count_digits > 0)
    {
        digit = (short)(count_num / count_digits);
        count_num = count_num % count_digits;

        if((digit !=0)||(start == true))
        {
            char_digit = (char)(digit + 48);

           *p_string = char_digit;
            p_string++;

            start = true;
        }
        count_digits /= 10;
    }

	if(start == false)
	{
		*p_string = (char)48; // zero
		*p_string++;
	}

   *p_string = 0;
}

void ParamNum2String(long num, char* string)
{
    long    count_digits = 10;
    long    count_num;
    short   digit;
    char    char_digit;
    char*   p_string = string;

    if(num == 100)  // make dashed string "--" if 100
    {
        *p_string++ = 0x2D;
        *p_string++ = 0x2D;
    }
    else if(num < 100)
    {
        count_num = num;

        while(count_digits > 0)
        {
            digit = (short)(count_num / count_digits);
            count_num = count_num % count_digits;

            if(digit == 0)
			{
               *p_string = '0';
                p_string++;
			}
            if(digit !=0)
            {
                char_digit = (char)(digit + 48);
               *p_string = char_digit;
                p_string++;
            }

            count_digits /= 10;
        }
    }

   *p_string = 0;
}

long ParamString2Num(char* string)
{
    long    count_digits = 1;
    int     digit, nc;
    int     len = strlen(string);
    long    num = 0;
    int     dashcount = 0;

    if(len != 0)
    {
        for(nc = len - 1; nc >= 0; nc--)
        {
            if(string[nc] == 0x2D)
            {
                dashcount++;
            }

            if((string[nc] >= 0x30)&&(string[nc] <= 0x39))
            {
                digit = string[nc] - 0x30;
                num = num + digit * count_digits;
                count_digits *= 10;
            }
            else if((string[nc] == 0x2D)||(string[nc] == 0x25))
            {
                count_digits *= 10;
            }
        }

        if(dashcount == len)
        {
            num = 100; // 100 is a workaround for 2-digit number, likely to be changed to 10^len (len power of 10)
        }
    }

    return(num);
}

float CalcSampleFreqIncrement(Sample* sample, int semitones)
{
    float rate_inc = (float)((float)sample->info.samplerate / (float)fSampleRate);
    float note_mul = (float)(CalcFreqRatio(semitones));
    return rate_inc*note_mul;
}

float Calc_SlideMultiplier(unsigned long frame_length, int semitones)
{
    float r = CalcFreqRatio(semitones);
    float sm = pow(r, (float)((double)1/(double)frame_length));
    return sm;
}

float NoteToFreq(int note)
{
    return 440.0f*CalcFreqRatio(note - 57);
}

////
//////////////////////////////////////////////
// Returns relative frequency multiplier between two notes
////////////////////////////////////////////////////////////
float CalcFreqRatio(int semitones)
{
    int octave = abs(semitones / 12);
    int note_oct = abs(semitones % 12);
    float r = (float)freq_mul_table[note_oct]*(pow((float)2, octave));

    if(semitones >= 0)
        return r;
    else
        return (float)(1.0/r);
}

/////////////////////////////////////////////
// Certain frame, corresponding to certain tick. Depends on BPM.
inline long Tick2Frame(float tick)
{
    return (unsigned long)(tick*frames_per_tick);
}

/////////////////////////////////////////////
// Certain frame, corresponding to certain tick. Depends on BPM.
float Tick2Second(float tick)
{
    return Tick2Frame(tick)*one_divided_per_sample_rate;
}

/////////////////////////////////////////////
// Certain frame, corresponding to certain tick. Depends on BPM.
float Second2Tick(float sec)
{
    return sec*fSampleRate*one_divided_per_frames_per_tick;
}

/////////////////////////////////////////////
// CTick, corresponding to certain frame. Depends on BPM.
float Frame2Tick(tframe frame)
{
    return (float)(frame*one_divided_per_frames_per_tick);
}

float Frame2Tick(double frame)
{
    return (float)(frame*one_divided_per_frames_per_tick);
}

///
///////////////////////////////////////
// Returns pixel length for a number of frames, depending on sample rate and current tick width
int Calc_PixLength(long num_frames, long sample_rate, float tickwidth)
{
    float rate = (float)fSampleRate/sample_rate;
	int l = RoundDouble(((double)num_frames/(double)fSampleRate)/seconds_in_tick*tickwidth * rate);
	return l;
}

////////////////////////////////////////////////////
// Returns multiplier coefficients for left and right channels based on panning in percents.
float Calc_PercentPan(float pan_percent)
{
    return pan_percent/50 - 1;
}

///////////////////////////
// Returns float edinitsa-based value
inline float Div100(float percent)
{
    return(percent/100);
}

///////////////////////////////////////
// Returns linearly interpolated value between two values. Return value is y3 for given x3
// Interval is defined by [x1,x2] where x3 is any point on this interval
// y - is amplitude value of the signal being interpolated
INLINE float Interpolate_Line(double x1, float y1, double x2, float y2, double x3)
{
    return (float)(y1 + (x3 - x1)*((y2 - y1)/(x2 - x1)));
}

void Vanish(Element* el)
{
    if(el == M.active_elem)
    {
        M.active_elem = NULL;
    }

    if(el == C.curElem)
    {
        C.ExitToCurrentMode();
    }

    delList[delCount] = el;
    delCount++;
}

// TODO: optimize adding/deleting
void Vanish_CleanUp()
{
    Element* el;
    while(delCount > 0)
    {
        delCount--;

        el = delList[delCount];
		DeleteElement(el, false, false);
    }
}

int RoundDouble(double val)
{
    int ival = abs((int)val);
    if((fabs(val) - ival) >= 0.5)
        ival++;

	if(val > 0)
	{
		return ival;
	}
	else
	{
		return 0 - ival;
	}
}

long RoundDoubleLong(double val)
{
    long ival = c_abs((long)val);
    if((c_abs(val) - ival) >= 0.5)
        ival++;

	if(val > 0)
	{
		return ival;
	}
	else
	{
		return 0 - ival;
	}
}

int RoundFloat(float val)
{
    int ival = abs((int)val);
    if((fabs(val) - ival) >= 0.5)
        ival++;

	if(val > 0)
	{
		return ival;
	}
	else
	{
		return 0 - ival;
	}
}

int RoundFloat1(float val)
{
    int ival = abs((int)val);
    if((fabs(val) - ival) > 0.5)
        ival++;

	if(val > 0)
	{
		return ival;
	}
	else
	{
		return 0 - ival;
	}
}

Symbol GetSymbolFromString(char* data)
{
    char* buffer;
    int len;
    Symbol s;

    len = strlen(data);
    buffer = (char*)malloc(len + 1);

    memcpy(buffer, data, len);
    buffer[len] = 0;

    LowerCase(buffer);

    if(strcmp(buffer, "#") == 0)
    {
        s = Symbol_Mute;
    }
    else if(strcmp(buffer, "^") == 0)
    {
        s = Symbol_Volume;
    }
    else if(strcmp(buffer, "*") == 0)
    {
        s = Symbol_Panning;
    }
    else if(strcmp(buffer, "/") == 0)
    {
        s = Symbol_Slider;
    }
    else if(strcmp(buffer, ">") == 0)
    {
        s = Symbol_Repeat;
    }
    else if(strcmp(buffer, "<") == 0)
    {
        s = Symbol_Reflect;
    }
    else if(strcmp(buffer, "~") == 0)
    {
        s = Symbol_Vibrate;
    }
    else if(strcmp(buffer, "=") == 0)
    {
        s = Symbol_Track;
    }
    else if(strcmp(buffer, "@") == 0)
    {
        s = Symbol_Local;
    }
    else if(strcmp(buffer, ".") == 0)
    {
        s = Symbol_Command;
    }
    else if(strcmp(buffer, "|") == 0)
    {
        s = Symbol_Break;
    }
    else if(strcmp(buffer, "%") == 0)
    {
        s = Symbol_Transpose;
    }
    else if(strcmp(buffer, "{") == 0)
    {
        s = Symbol_Bookmark;
    }
    else
    {
        s = SYMBOL_UNKNOWN;
    }

    free(buffer);
    return s;
}

AliasRecord* GetAliasRecordFromString(char* str)
{
    char* buffer;
    int len;

    len = strlen(str);
    buffer = (char*)malloc(len + 1);
    memcpy(buffer, str, len);
    buffer[len] = 0;
    LowerCase(buffer);

    AliasRecord* ar = first_alias_record;
    while(ar != NULL)
    {
        if(strcmp(buffer, ar->alias) == 0)
        {
            break;
        }
        ar = ar->next;
    }

    free(buffer);
    return ar;
}

Instrument* CheckStringForInstrumentAlias(char* string)
{
    Instrument* i = first_instr;
    while(i != NULL)
    {
		if(strcmp(string, i->alias->string) == 0)
        {
            return i;
        }
        i = i->next;
    }
	return NULL;

/*
    for(int nc = 0; nc < num_instrs; nc++)
    {
		if(strcmp(string, instr[nc]->alias->string) == 0)
        {
            return instr[nc];
        }
    }
    return NULL;
    */
}

void AuxCheck()
{
    int cx, elx;
    float tick;
    float right_tick_margin1 = (GridX2 - GridX1)/tickWidth + OffsTick;
    float right_tick_margin2 = (StX2 - StX1)/st_tickWidth + st->tick_offset;
	
	int offsx = Tick2X(OffsTick, Loc_MainGrid);

    Element* el = firstElem;
    while(el != NULL)
    {
        el->gridauxed = el->auxauxed = 0xFFFF;
        if(el->IsPresent() && el->displayable == true)
        {
            tick = el->start_tick + el->patt->start_tick;
            if(el->patt == field && (el->type == El_Samplent || 
                                     el->type == El_Gennote ||
                                     el->type == El_Pattern ||
                                    (el->type == El_SlideNote && el->patt->ptype != Patt_StepSeq)))
            {
                if(el->patt == st->patt)
                {
                    elx = el->st_StartTickX();
                    if(elx >= StX1 && elx <= StX2)
                    {
                        cx = el->st_StartTickX();
                        el->gridauxed = cx - StX1;
                    }
                }
                else
                {
                    elx = el->StartTickX();
                    if(elx >= GridX1 && elx <= GridX2)
                    {
                        cx = elx;
                        el->gridauxed = cx - GridX1;
                    }
                }
            }
        }
        el = el->next;
    }

    if(gAux->auxmode == AuxMode_Pattern)
    {
        right_tick_margin1 = (GridXS2 - GridXS1)/gAux->tickWidth + gAux->OffsTick - 1;
        el = gAux->workPt->OrigPt->first_elem;
        while(el != NULL)
        {
            if(el->IsPresent() && el->displayable == true)
            {
                elx = el->s_StartTickX();
                if((el->type == El_Samplent || 
                    el->type == El_SlideNote || 
                    el->type == El_Gennote ||
                    el->type == El_Pattern)&&
                   (elx >= GridXS1 && elx <= GridXS2))
                {
                    cx = elx;
                    el->auxauxed = cx - GridXS1;
                }
            }
            el = el->patt_next;
        }
    }
}

bool CheckPlaneCrossing(int x1, int y1, int x2, int y2, int a1, int b1, int a2, int b2)
{
    if((((x1 >= a1 && x1 <= a2)||(x2 >= a1 && x2 <= a2))&&
        ((y1 >= b1 && y1 <= b2)||(y2 >= b1 && y2 <= b2)))||
        ((x1 < a1 && x2 > a2)&&
        ((y1 >= b1 && y1 <= b2)||(y2 >= b1 && y2 <= b2)))||
        ((y1 < b1 && y2 > b2)&&
        ((x1 >= a1 && x1 <= a2)||(x2 >= a1 && x2 <= a2)))||
        ((x1 < a1 && x2 > a2)&&(y1 < b1 && y2 > b2)))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool IsPianoKeyPressed(int pkeynum)
{
    bool pressed = false;
    if(M.pianokey_pressed && M.pianokey_num == pkeynum)
    {
        pressed = true;
    }
    else
    {
        for(int ic = 0; ic < MAX_PREVIEW_ELEMENTS; ic++)
        {
            if(PrevSlot[ic].keybound_pianokey == true && PrevSlot[ic].ii != NULL)
            {
                if(PrevSlot[ic].ii->ed_note->value == pkeynum)
                {
                    pressed = true;
                    break;
                }
            }
        }
    }

    return pressed;
}

Mixcell* GetMixcellFromFXString(DigitStr* dfxstr)
{
    int d1, d2;
    if(dfxstr->digits[0] == 0 || dfxstr->digits[1] == 0)
    {
        return &mix->m_cell;
    }
    else
    {
        if(dfxstr->digits[0] >= 0x30 && dfxstr->digits[0] <= 0x39)
        {
            d1 = int(dfxstr->digits[0] - 0x30);
        }
        else if(dfxstr->digits[0] >= 0x41 && dfxstr->digits[0] <= 0x40 + NUM_MIXER_COLUMNS - 10)
        {
            d1 = int(dfxstr->digits[0] - 0x41 + 10);
        }
        else if(dfxstr->digits[0] >= 0x61 && dfxstr->digits[0] <= 0x60 + NUM_MIXER_COLUMNS - 10)
        {
            d1 = int(dfxstr->digits[0] - 0x61 + 10);
        }
        else
        {
            return &mix->m_cell;
        }
    
        if(dfxstr->digits[1] >= 0x30 && dfxstr->digits[1] <= 0x39)
        {
            d2 = int(dfxstr->digits[1] - 0x30);
        }
        else if(dfxstr->digits[1] >= 0x41 && dfxstr->digits[1] <= 0x40 + NUM_MIXER_ROWS - 10)
        {
            d2 = int(dfxstr->digits[1] - 0x41 + 10);
        }
        else if(dfxstr->digits[1] >= 0x61 && dfxstr->digits[1] <= 0x60 + NUM_MIXER_ROWS - 10)
        {
            d2 = int(dfxstr->digits[1] - 0x61 + 10);
        }
        else
        {
            return &mix->m_cell;
        }
    
        return &mix->r_cell[d1][d2];
    }
}

////////////////////////////////////////////////////
// Recursively checks for mixcell loops
bool CheckMixcellLoop(Mixcell * mc, Mixcell * outmc)
{
    Mixcell* mm;
    Mixcell* mlast;
    Mixcell* m = outmc;
    while(m != NULL)
    {
        mm = mc;
        while(mm != NULL)
        {
            if(m == mm)
            {
                m = mc;
                while(m->check_next != NULL)
                {
                    mm = m->check_next;
                    m->check_next = NULL;
                    m = mm;
                }
                return true;
            }
            mlast = mm;
            mm = mm->check_next;
        }

        if(m->effect != NULL && m->effect->type == EffType_Send)
        {
            Send* snd = (Send*)m->effect;
            if(snd->send_mixcell != NULL)
            {
                mlast->check_next = m;
                m->check_next = NULL;
                if(CheckMixcellLoop(mc, snd->send_mixcell) == true)
                {
                    return true;
                }
                mlast->check_next = NULL;
            }
        }

        m = m->outcell;
    }

    return false;
}

void GetOriginalInstrumentAlias(const char* name, char* alias)
{
    char n[MAX_ALIAS_STRING];  // TODO: overflow zone
    memset(n, 0, 5);

    n[0] = name[0];

	LowerCase(n);

    int index = 0;

    Instrument* i = first_instr;
    while(i != NULL)
    {
        if(i->temp == false && strcmp(n, i->alias->string) == 0)
        {
            index++;
            Num2String(index, (char*)&(n[1]));
            i = first_instr;
        }
        i = i->next;
    }

    strcpy(alias, n);
}

void GetOriginalInstrumentName(const char* name, char* newname)
{
    char ncheck[MAX_NAME_STRING];  // TODO: overflow zone
    memset(ncheck, 0, MAX_NAME_STRING);
    strcpy(ncheck, name);
    int len = strlen(ncheck);
    int index = 0;
    Instrument* i = first_instr;
    while(i != NULL)
    {
        if(i->temp == false && strcmp(ncheck, i->name) == 0)
        {
            index++;
            strcpy(ncheck, name);
            strcat(ncheck, "_");
            Num2String(index, (char*)&(ncheck[len + 1]));
            i = first_instr;
        }
        i = i->next;
    }

    strcpy(newname, ncheck);
}

bool AuxCheckPosData(int* posX)
{
    if(gAux->auxmode == AuxMode_Pattern && gAux->workPt != gAux->blankPt)
    {
       *posX = gAux->curr_play_x - RoundFloat(gAux->OffsTick*gAux->tickWidth);
        return true;
    }
    else
    {
        return false;
    }
}

void CheckButtonsRepaint()
{
    Butt* bt;
    DrawArea* da;
    Control* ctrl = firstCtrl;
    while(ctrl != NULL)
    {
        if(ctrl->active == true && (ctrl->type == Ctrl_Button || ctrl->type == Ctrl_ButtFix))
        {
            bt = (Butt*)ctrl;
            if(bt->needs_refresh == true)
            {
                bt->needs_refresh = false;
                da = bt->drawarea;
                MC->listen->repaint(da->ax, da->ay, da->aw, da->ah);
            }
        }
        ctrl = ctrl->next;
    }
}

void CheckTempDrawareas()
{
    DrawArea* da;
    DrawArea* da1;
    da = MC->listen->first_temp_drawarea;
    while(da != NULL)
    {
        if(da->isEnabled() == true)
        {
            if(da->type == Draw_Highlight)
            {
                Element* el = (Element*)da->drawobject;
                MC->listen->repaint(el->abs_area.x1, 
                                       el->abs_area.y1, 
                                       el->abs_area.x2 - el->abs_area.x1 + 1, 
                                       el->abs_area.y2 - el->abs_area.y1 + 1);
            }
        }
        else
        {
            da1 = da;
            da = da->next;
            MC->listen->RemoveTempDrawArea(da1);
            continue;
        }

        da = da->next;
    }
}

inline void PanConstantRule(float pan, float* volL, float* volR)
{
    float pp = (float)PI*(pan + 1)/4;
   *volL = wt_cosine[int(pp/wt_angletoindex)];//cos(pp); // TODO: precalculate
   *volR = wt_sine[int(pp/wt_angletoindex)];//sin(pp); // TODO: precalculate
}

inline void PanLinearRule(float pan, float* volL, float* volR)
{
   *volL = *volR = 1;
    if(pan > 0)
    {
       *volL -= pan;
    }
    else if(pan < 0)
    {
       *volR += pan;
    }
}

void CopyImageSection(Image* img1, Image* img2, int x1, int y1, int x2, int y2, int w, int h)
{
    if(img1 != NULL && img2 != NULL && w > 0 && h > 0)
    {
        Image* img = new Image(MC->flags.opaqueFlag ? Image::RGB : Image::ARGB, w, h, true);
        
        Graphics ic1(*img);
        ic1.setOrigin (-x1, -y1);
        ic1.drawImageAt(img1, 0, 0);

        Graphics ic2(*img2);

        ic2.drawImageAt(img, x2, y2, false);
    }
}

void GetPatternNameImage(Pattern* pt)
{
    if(pt->nmimg != NULL)
	{
		delete pt->nmimg;
		pt->nmimg = NULL;
	}

    int nw = ti->getStringWidth(pt->name->string);
    if(nw > 0)
    {
        pt->nmimg = new Image(Image::ARGB, nw, (int)(ti->getHeight() + 5), true);
        Graphics imageContext(*(pt->nmimg));
        imageContext.setColour(Colour(0xffDFDFDF));
        J_TextSmall_xy(imageContext, 0, (int)(ti->getHeight()), pt->name->string);
    }
}

void GetInstrAliasImage(Instrument* instr)
{
    if(instr->alimg != NULL)
	{
       delete instr->alimg;
	   instr->alimg = NULL;
	}

    int aw = ins->getStringWidth(instr->alias->string);
	if(aw > 0)
	{
		instr->alimg = new Image(Image::ARGB, aw, (int)(ins->getHeight() + 3), true);
		Graphics imageContext(*(instr->alimg));
		//imageContext.setColour(Colour(0xff6496FF));
		if(instr->type == Instr_Sample)
		    imageContext.setColour(Colour(smpcolour));
        else
		    imageContext.setColour(Colour(gencolour));
		J_TextInstr_xy(imageContext, 0, (int)(ins->getHeight()), instr->alias->string);
	}
}

inline float GetVolOutput(float val)
{
    if(val < 1)
    {
        //outval = 1 - pow((1 - val), 1.0f/1.5f);
        //outval = pow(10, 2*log10(1 - val));
        //outval = pow(val, 2) + 1 - pow((1 - val), 1.0f/1.5f);
        //outval /= 2;
        //outval = pow(val, 2);
        return (pow(val, 2) + (1 - pow((1 - val), 1.0f/1.5f)))/2;
    }
    else
    {
        return pow(val, 2);
    }
}
