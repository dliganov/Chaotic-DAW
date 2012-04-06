#ifndef GRAPHJUCE_H
#define GRAPHJUCE_H


#ifdef USE_OLD_JUCE
#include "juce_amalgamated.h"
#else
#include "juce_amalgamated_NewestMerged.h"
#endif
#include "awful_jucecomponents.h"

class SliderBase;
class Pattern;
class Instrument;

enum SliderType;

extern Image*  img_closew1;
extern Image*  img_closew2;

extern Image*  img_showpresets1;
extern Image*  img_showpresets2;

extern Image*  img_savepreset1;
extern Image*  img_savepreset2;

extern Colour  smpcolour;
extern Colour  gencolour;

extern Image*  img_btinsup1;
extern Image*  img_btinsup2;

extern Image*  img_btinsdown1;
extern Image*  img_btinsdown2;

extern Image*  img_curscopy;
extern Image*  img_cursclone;
extern Image*  img_cursslide;
extern Image*  img_cursbrush;
extern Image*  img_cursselect;

extern Image*  img_solo1off_s;
extern Image*  img_solo1off_g;
extern Image*  img_solo1off_c;

extern Image*  img_mute1off_s;
extern Image*  img_mute1off_g;
extern Image*  img_mute1off_c;

extern Image*  img_mute1off;
extern Image*  img_mute1on;
extern Image*  img_solo1off;
extern Image*  img_solo1on;

extern Image*  img_autooff_s;
extern Image*  img_autooff_g;

extern Image*  img_btwnd_s;
extern Image*  img_btwnd_g;

extern Image*  img_genvolback;
extern Image*  img_smpvolback;
extern Image*  img_genpanback;
extern Image*  img_smppanback;

extern Image*  img_instrsmall1;
extern Image*  img_instrsmall2;

extern void	J_DrawAll(Graphics& g);
extern void Init_Fonts();
extern void J_MainScale(Graphics& g);
extern void J_Pos(Graphics& g, int pos, int gx, int gy);
extern void J_PosAux(Graphics& g, int pos, int gx, int gy);
extern void J_TrackControls(Graphics& g);
extern void J_Separates(Graphics& g);
extern void J_Grid(Graphics& g);
extern void J_Content_Main(Graphics& g);
extern void J_Cursors(Graphics& g);
extern void J_MainBar(Graphics& g);
extern void J_MainVBar(Graphics& g);
extern void J_Aux(Graphics& g);
extern void J_SelectionHighlights(Graphics& g);
extern void J_Selection(Graphics& g);
extern void J_Browser(Graphics& g);
extern void J_RefreshButtons(Graphics& g);
extern void J_GridHighlights(Graphics& g);
extern void J_AuxHighlights(Graphics& g);
extern void J_Mixer(Graphics& g);
extern void J_InstrPanel(Graphics& g);
extern void J_InstrPanel(Graphics& g, int x, int y);
extern void J_KeyCursor(Graphics& g);
extern void J_MouseCursor(Graphics& g);
extern void J_Content_Aux(Graphics& g);
extern void J_MixerConnectionsAndHighlights(Graphics& g);
extern void J_AuxMixerHighlights(Graphics& g, bool hlredraw);
extern bool J_GetMouseCursorRect(int* x, int* y, int* w, int* h);
extern void J_MainMenu(Graphics& g);
extern void J_Menus(Graphics& g);
extern void J_MixCenter(Graphics& g);
extern void J_Aux_PianoKeys(Graphics& g);
extern void J_GridLanes(Graphics& g);
extern void J_AuxLanes(Graphics& g);
extern void J_GetKeyCursorRect(int* x, int* y, int* w, int* h);
extern void J_Auxaux(Graphics& g);
extern void J_Aux_Grid(Graphics& g);
extern void J_Aux_Scale(Graphics& g);
extern void J_InstrCenter(Graphics& g, int xb, int yb);
extern int J_TextInstr_xy(Graphics& g, int x, int y, const char *str);
extern int J_TextSmall_xy(Graphics& g, int x, int y, const char *str);
extern bool J_IsUnder();
extern void J_UnderPanel(Graphics& g);
extern void J_UnderPanelRefresh();
extern void J_Round(Graphics& g, int x1, int y1, float rad);
extern void J_FillRect(Graphics& g, int x1, int y1, int x2, int y2);
extern void J_Circle(Graphics& g, int x1, int y1, float rad);
extern void J_Line(Graphics& g, int x1, int y1, int x2, int y2);
extern void J_Line(Graphics& g, float x1, float y1, float x2, float y2);
extern void J_HLine(Graphics& g, int y, int x1, int x2);
extern void J_VLine(Graphics& g, int x, int y1, int y2);
extern void J_Line2(Graphics& g, int x1, int y1, int x2, int y2);
extern void J_RefreshGridImage();
extern void J_RefreshAuxGridImage();
extern void Init_Images();
extern void AssignLoadedImages();

#endif
