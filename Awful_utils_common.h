#ifndef UTILS_COMMON_H
#define UTILS_COMMON_H

#include "awful.h"
#include "awful_instruments.h"


#define c_abs(x) ((x) < 0 ? -(x) : (x))

extern float    NoteToFreq(int note);
extern void     CopyImageSection(Image* img1, Image* img2, int x1, int y1, int x2, int y2, int w, int h);
extern void     Vanish_CleanUp();
extern bool     IsPianoKeyPressed(int pkeynum);
extern int      Tick2X(float tick);
extern int      Line2Y(int line, Loc loc);
extern float    X2Tick(int x, Loc loc);
extern int      Y2Line(int y, Loc loc);
extern int      Gx(int xc, Loc loc);
extern int      Gy(int yc, Loc loc);
extern void     ToLowerCase(char* data);
extern void     ToUpperCase(char* data);
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
extern float    Frame2Tick(tframe frame);
extern float    Tick2Second(float tick);
extern float    Frame2Tick(double frame);
extern inline float Interpolate_Line(double x1, float y1, double x2, float y2, double x3);
extern int      Calc_PixLength(long num_frames, long sample_rate, float tickwidth);
extern float    Calc_PercentPan(float pan_percent);
extern float    Div100(float percent);
extern void     Vanish(Element* el);
extern void     Vanish_CleanUp();
extern int      RoundDouble(double val);
extern long     RoundDoubleLong(double val);
extern int      RoundFloat(float val);
extern int      RoundFloat1(float val);
extern int      MapKeyToNote(char character, bool relative);
extern CmdType  MapCNum2CType(int com_num);
extern int      MapCType2CNum(CmdType command);
extern int      s_Tick2X(float tick);
extern void     AuxCheck();
extern bool     CheckPlaneCrossing(int x1, int y1, int x2, int y2, int a1, int b1, int a2, int b2);
extern float    MapItem2Quant(MItemType itype);
extern Mixcell* GetMixcellFromFXString(DigitStr* dfxstr);
extern bool     CheckMixcellLoop(Mixcell* mc, Mixcell* outmc);
extern void     GetOriginalInstrumentAlias(const char* name, char* alias);
extern void     GetOriginalInstrumentName(const char* name, char* newname);
extern bool     AuxCheckPosData(int* posX);
extern void     CheckButtonsRepaint();
extern void     CheckTempDrawareas();
extern void     RefreshSeparates();
extern void     PanConstantRule(float pan, float* volL, float* volR);
extern void     PanLinearRule(float pan, float* volL, float* volR);
extern void     GetPatternNameImage(Pattern* pt);
extern void     GetInstrAliasImage(Instrument* instr);
extern Pattern* CheckStringForPatternName(char* string);
extern float    GetVolOutput(float val);
extern float    Second2Tick(float sec);

#endif 
