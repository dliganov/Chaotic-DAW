#include "awful.h"
#include "awful_audio.h"
#include "awful_instruments.h"
#include "awful_elements.h"
#include "awful_paramedit.h"
#include "awful_controls.h"
#include "awful_panels.h"
#include "awful_utils_common.h"
#include "VSTCollection.h"
#ifdef USE_OLD_JUCE
#include "juce_amalgamated.h"
#else
#include "juce_amalgamated_NewestMerged.h"
#endif
#include "awful_graphjuce.h"
#include "awful_cursorandmouse.h"
#include "awful_jucecomponents.h"
#include "awful_undoredo.h"
#include "Data sources/mfont.h"
#include "Data sources/tahoma.h"
#include "Data sources/tiny.h"
#include "Data sources/tiny1.h"
#include "Data sources/aripix.h"
#include "Data sources/instrum.h"
#include "Data sources/big.h"
#include "Data sources/images.h"
#include "Data sources/projs.h"
#include "Data sources/roxx.h"
#include "Data sources/dpix.h"
#include "Data sources/fixed.h"
#include "Data sources/bold.h"
#include "Data sources/visitor.h"


Font*   taho;
Font*   ti;
Font*   ti1;
Font*   ari;
Font*   ins;
Font*   bignum;
Font*   prj;
Font*   rox;
Font*   dix;
Font*   fix;
Font*   bld;
Font*   vis;

bool    J_IsUnder();
void    J_UnderPanel(Graphics& g);
void    J_UnderPanelRefresh();
int     J_DrawMixChannelContent(Graphics& g, int x, int y, MixChannel* mchan);

Colour  smpcolour = Colour(190, 190, 190);
Colour  gencolour = Colour(150, 150, 255);
Colour  panelcolourOld = Colour(65, 134, 110);
Colour  panelcolourOld1 = Colour(75, 105, 95);
//Colour  panelcolour = Colour(70, 85, 103);
Colour  panelcolour = Colour(68, 78, 82);
Colour  quantmenucolour = Colour(115, 155, 165);
Colour  auxcolour = Colour(50, 109, 90);

Image*  img_minimize1;
Image*  img_minimize2;

Image*  img_maximize1;
Image*  img_maximize2;

Image*  img_close1;
Image*  img_close2;

Image*  img_showpresets1;
Image*  img_showpresets2;

Image*  img_savepreset1;
Image*  img_savepreset2;

Image*  img_mcvst;
Image*  img_mcf0;
Image*  img_mcf1;
Image*  img_mcf2;
Image*  img_mcsend;
Image*  img_mc2sends;
Image*  img_mcgain;
Image*  img_mcdelay0;
Image*  img_mcdelay1;
Image*  img_mcdelay2;
Image*  img_mcrev0;
Image*  img_mcrev1;
Image*  img_mcrev2;
Image*  img_mccomp0;
Image*  img_mccomp1;
Image*  img_mccomp2;

Image*  img_mfold1;
Image*  img_mfold2;

Image*  img_closew1;
Image*  img_closew2;

Image*  img_instrclosed;
Image*  img_instropened;
Image*  img_instropened1;
Image*  img_instrempty;

Image*  img_vslpad;
Image*  img_vslpad2;

Image*  img_vslpad1;
Image*  img_vslpad11;

Image*  img_mc1;
Image*  img_mc2;

Image*  img_playbt1;
Image*  img_playbt2;

Image*  img_stopbt1;
Image*  img_stopbt2;

Image*  img_recbt1;
Image*  img_recbt2;

Image*  img_fwdbt1;
Image*  img_fwdbt2;

Image*  img_revbt1;
Image*  img_revbt2;

Image*  img_endbt1;
Image*  img_endbt2;

Image*  img_homebt1;
Image*  img_homebt2;

Image*  img_txtbt1;
Image*  img_txtbt2;

Image*  img_notebt1;
Image*  img_notebt2;

Image*  img_metrobt1;
Image*  img_metrobt2;

Image*  img_slidebt1;
Image*  img_slidebt2;
Image*  img_slidebt3;

Image*  img_fastbt1;
Image*  img_fastbt2;

Image*  img_brushbt1;
Image*  img_brushbt2;

Image*  img_cfgbt1;
Image*  img_cfgbt2;

Image*  img_rendbt1;
Image*  img_rendbt2;

Image*  img_solooff;
Image*  img_soloon;

Image*  img_solo1off;
Image*  img_solo1on;

Image*  img_solo1off_s;
Image*  img_solo1off_g;
Image*  img_solo1off_c;

Image*  img_muteoff;
Image*  img_muteon;

Image*  img_mute1off;
Image*  img_mute1on;

Image*  img_mute1off_s;
Image*  img_mute1off_g;
Image*  img_mute1off_c;

Image*  img_autoon;
Image*  img_autooff;

Image*  img_autooff_s;
Image*  img_autooff_g;

Image*  img_zoomback;

Image*  img_zoomin1;
Image*  img_zoomin2;

Image*  img_zoomout1;
Image*  img_zoomout2;

Image*  img_zoomindef1;
Image*  img_zoomindef2;

Image*  img_zoomoutdef1;
Image*  img_zoomoutdef2;

Image*  img_accmixer1;
Image*  img_accmixer2;

Image*  img_accgrid1;
Image*  img_accgrid2;

Image*  img_acccenter1;
Image*  img_acccenter2;

Image*  img_accgb1;
Image*  img_accgb2;

Image*  img_accinstr1;
Image*  img_accinstr2;

Image*  img_acceff1;
Image*  img_acceff2;

Image*  img_accheff1;
Image*  img_accheff2;

Image*  img_accseff1;
Image*  img_accseff2;

Image*  img_knobsmall;
Image*  img_knoblarge;
Image*  img_knobxlarge;
Image*  img_knobzlarge;

Image*  img_pattexp1;
Image*  img_pattexp2;

Image*  img_vols1;
Image*  img_vols2;

Image*  img_pans1;
Image*  img_pans2;

Image*  img_pitch1;
Image*  img_pitch2;

Image*  img_grid1;
Image*  img_grid2;

Image*  img_piano1;
Image*  img_piano2;

Image*  img_pr8_1;
Image*  img_pr8_2;

Image*  img_pr10_1;
Image*  img_pr10_2;

Image*  img_pr12_1;
Image*  img_pr12_2;

Image*  img_step1;
Image*  img_step2;

Image*  img_fold1;
Image*  img_fold2;

Image*  img_unfold1;
Image*  img_unfold2;

Image*  img_shut1;
Image*  img_shut2;

Image*  img_files1;
Image*  img_files2;

Image*  img_autopt1;
Image*  img_autopt2;

Image*  img_mix1;
Image*  img_mix2;

Image*  img_projects1;
Image*  img_projects2;

Image*  img_externalplugs1;
Image*  img_externalplugs2;

Image*  img_nativeplugs1;
Image*  img_nativeplugs2;

Image*  img_samples1;
Image*  img_samples2;

Image*  img_params1;
Image*  img_params2;

Image*  img_presets1;
Image*  img_presets2;

Image*  img_tracks1;
Image*  img_tracks2;

Image*  img_notracks1;
Image*  img_notracks2;

Image*  img_play2bt1;
Image*  img_play2bt2;

Image*  img_stop2bt1;
Image*  img_stop2bt2;

Image*  img_autoadv1;
Image*  img_autoadv2;

Image*  img_autoswitch1;
Image*  img_autoswitch2;

Image*  img_relswitch1;
Image*  img_relswitch2;

Image*  img_btup1;
Image*  img_btup2;

Image*  img_btdn1;
Image*  img_btdn2;

Image*  img_bighandle;
Image*  img_bighandle1;
Image*  img_tinyhandle1;
Image*  img_tinyhandle2;
Image*  img_tinyhandle3;
Image*  img_tinyhandle4;
Image*  img_smallhandle;

Image*  img_genhandle;
Image*  img_smphandle;

Image*  img_btforth1;
Image*  img_btforth2;

Image*  img_btback1;
Image*  img_btback2;

Image*  img_btback21;
Image*  img_btback22;

Image*  img_btforth21;
Image*  img_btforth22;

Image*  img_btup21;
Image*  img_btup22;

Image*  img_btdn21;
Image*  img_btdn22;

Image*  img_btstat1;
Image*  img_btstat2;

Image*  img_paramhandle;

Image*  img_node;
Image*  img_node2;

Image*  img_byp1;
Image*  img_byp2;

Image*  img_effbyp1;
Image*  img_effbyp2;

Image*  img_sample1;
Image*  img_sample2;

Image*  img_stepq;

Image*  img_tgparam;
Image*  img_tgparam1;
Image*  img_tgparamoff;

Image*  img_auto1;
Image*  img_auto2;

Image*  img_btwnd;
Image*  img_btwnd1;

Image*  img_btwnd_s;
Image*  img_btwnd_g;

Image*  img_btinsup1;
Image*  img_btinsup2;
Image*  img_btinsdown1;
Image*  img_btinsdown2;
Image*  img_btinsdownactive;
Image*  img_btinsupactive;

Image*  img_bthotkeys1;
Image*  img_bthotkeys2;

Image*  img_mixpad;
Image*  img_mixpadmaster;

Image*  img_mixpanback;
Image*  img_mixpanbody;
Image*  img_mixpanthumb;

Image*  img_curscopy;
Image*  img_cursclone;
Image*  img_cursslide;
Image*  img_cursbrush;
Image*  img_cursselect;

Image*  img_instrsmall1;
Image*  img_instrsmall2;

Image*  img_maingrid;
Image*  img_auxgrid1;
Image*  img_auxgrid2;

Image*  img_titlebkg;

Image*  img_sunlogo;

Image*  img_ctrl1;
Image*  img_ctrl2;
Image*  img_ctrl3;

Image*  img_display;

Image*  img_numback;
Image*  img_bpmback;

Image*  img_rect11;
Image*  img_rect12;
Image*  img_rect13;

Image*  img_rect21;
Image*  img_rect22;
Image*  img_rect23;

Image*  img_rect31;
Image*  img_rect32;
Image*  img_rect33;

Image*  img_instrvolbody;
Image*  img_instrpanbody;
Image*  img_instrvolthumb;
Image*  img_instrpanthumb;

Image*  img_genvolback;
Image*  img_genpanback;
Image*  img_smpvolback;
Image*  img_smppanback;

Image*  img_paramback;
Image*  img_parambody;
Image*  img_parambody1;

void Init_Images()
{
    img_paramback = ImageFileFormat::loadFrom(images::paramslider_back_png, images::paramslider_back_pngSize);
    img_parambody = ImageFileFormat::loadFrom(images::paramslider_body_png, images::paramslider_body_pngSize);
    img_parambody1 = ImageFileFormat::loadFrom(images::paramslider_body1_png, images::paramslider_body1_pngSize);

    img_instrvolthumb = ImageFileFormat::loadFrom(images::instr_volthumb_png, images::instr_volthumb_pngSize);
    img_instrpanthumb = ImageFileFormat::loadFrom(images::instr_panthumb_png, images::instr_panthumb_pngSize);

    img_genvolback = ImageFileFormat::loadFrom(images::instr_genvolback_png, images::instr_genvolback_pngSize);
    img_genpanback = ImageFileFormat::loadFrom(images::instr_genpanback_png, images::instr_genpanback_pngSize);

    img_smpvolback = ImageFileFormat::loadFrom(images::instr_smpvolback_png, images::instr_smpvolback_pngSize);
    img_smppanback = ImageFileFormat::loadFrom(images::instr_smppanback_png, images::instr_smppanback_pngSize);

    img_instrvolbody = ImageFileFormat::loadFrom(images::instr_volbody_png, images::instr_volbody_pngSize);
    img_instrpanbody = ImageFileFormat::loadFrom(images::instr_panbody_png, images::instr_panbody_pngSize);

    img_instrsmall1 = ImageFileFormat::loadFrom(images::instrsmall1_png, images::instrsmall1_pngSize);
    img_instrsmall2 = ImageFileFormat::loadFrom(images::instrsmall2_png, images::instrsmall2_pngSize);

    img_curscopy = ImageFileFormat::loadFrom(images::curscopy_png, images::curscopy_pngSize);
    img_cursclone = ImageFileFormat::loadFrom(images::cursclone_png, images::cursclone_pngSize);
    img_cursslide = ImageFileFormat::loadFrom(images::cursslide_png, images::cursslide_pngSize);

    img_cursbrush = ImageFileFormat::loadFrom(images::cursbrush_png, images::cursbrush_pngSize);

    img_cursselect = ImageFileFormat::loadFrom(images::cursselect_png, images::cursselect_pngSize);

    img_mixpad = ImageFileFormat::loadFrom(images::mixpad_png, images::mixpad_pngSize);
    img_mixpadmaster = ImageFileFormat::loadFrom(images::mixpadmaster_png, images::mixpadmaster_pngSize);

    img_mixpanback = ImageFileFormat::loadFrom(images::mix_panback_png, images::mix_panback_pngSize);
    img_mixpanbody = ImageFileFormat::loadFrom(images::mix_panbody_png, images::mix_panbody_pngSize);
    img_mixpanthumb = ImageFileFormat::loadFrom(images::mix_panthumb_png, images::mix_panthumb_pngSize);

    img_btinsup1 = ImageFileFormat::loadFrom(images::bt_insup1_png, images::bt_insup1_pngSize);
    img_btinsup2 = ImageFileFormat::loadFrom(images::bt_insup2_png, images::bt_insup2_pngSize);

    img_btinsdown1 = ImageFileFormat::loadFrom(images::bt_insdown1_png, images::bt_insdown1_pngSize);
    img_btinsdown2 = ImageFileFormat::loadFrom(images::bt_insdown2_png, images::bt_insdown2_pngSize);

    img_btinsdownactive = ImageFileFormat::loadFrom(images::bt_insdown_active_png, images::bt_insdown_active_pngSize);
    img_btinsupactive = ImageFileFormat::loadFrom(images::bt_insup_active_png, images::bt_insup_active_pngSize);

    img_btwnd = ImageFileFormat::loadFrom(images::bt_wnd_png, images::bt_wnd_pngSize);
    img_btwnd1 = ImageFileFormat::loadFrom(images::bt_wnd1_png, images::bt_wnd1_pngSize);

    img_btwnd_s = ImageFileFormat::loadFrom(images::bt_wnd_s_png, images::bt_wnd_s_pngSize);
    img_btwnd_g = ImageFileFormat::loadFrom(images::bt_wnd_g_png, images::bt_wnd_g_pngSize);

    img_minimize1 = ImageFileFormat::loadFrom(images::bt_minimize_1_png, images::bt_minimize_1_pngSize);
    img_minimize2 = ImageFileFormat::loadFrom(images::bt_minimize_2_png, images::bt_minimize_2_pngSize);

    img_maximize1 = ImageFileFormat::loadFrom(images::bt_maximize_1_png, images::bt_maximize_1_pngSize);
    img_maximize2 = ImageFileFormat::loadFrom(images::bt_maximize_2_png, images::bt_maximize_2_pngSize);

    img_close1 = ImageFileFormat::loadFrom(images::bt_close_1_png, images::bt_close_1_pngSize);
    img_close2 = ImageFileFormat::loadFrom(images::bt_close_2_png, images::bt_close_2_pngSize);

    img_showpresets1 = ImageFileFormat::loadFrom(images::bt_loadpreset_1_png, images::bt_loadpreset_1_pngSize);
    img_showpresets2 = ImageFileFormat::loadFrom(images::bt_loadpreset_2_png, images::bt_loadpreset_2_pngSize);

    img_savepreset1 = ImageFileFormat::loadFrom(images::bt_savepreset_1_png, images::bt_savepreset_1_pngSize);
    img_savepreset2 = ImageFileFormat::loadFrom(images::bt_savepreset_2_png, images::bt_savepreset_2_pngSize);

    img_mcvst = ImageFileFormat::loadFrom(images::mcvst_png, images::mcvst_pngSize);
    img_mcdelay0 = ImageFileFormat::loadFrom(images::mcdelay0_png, images::mcdelay0_pngSize);
    img_mcdelay1 = ImageFileFormat::loadFrom(images::mcdelay1_png, images::mcdelay1_pngSize);
    img_mcdelay2 = ImageFileFormat::loadFrom(images::mcdelay2_png, images::mcdelay2_pngSize);
    img_mc2sends = ImageFileFormat::loadFrom(images::mc2sends_blank_png, images::mc2sends_blank_pngSize);

    img_mfold1 = ImageFileFormat::loadFrom(images::mixfold1_png, images::mixfold1_pngSize);
    img_mfold2 = ImageFileFormat::loadFrom(images::mixfold2_png, images::mixfold2_pngSize);

    //img_instrclosed = ImageFileFormat::loadFrom(File("Graphic/instr_closed.png"));
    img_instropened = ImageFileFormat::loadFrom(images::instr_opened_png, images::instr_opened_pngSize);
    img_instropened1 = ImageFileFormat::loadFrom(images::instr_opened1_png, images::instr_opened1_pngSize);
    img_instrempty = ImageFileFormat::loadFrom(images::instr_empty_png, images::instr_empty_pngSize);

    img_playbt1 = ImageFileFormat::loadFrom(images::bt_play_1_png, images::bt_play_1_pngSize);
    img_playbt2 = ImageFileFormat::loadFrom(images::bt_play_2_png, images::bt_play_2_pngSize);

    img_stopbt1 = ImageFileFormat::loadFrom(images::bt_stop_1_png, images::bt_stop_1_pngSize);
    img_stopbt2 = ImageFileFormat::loadFrom(images::bt_stop_2_png, images::bt_stop_2_pngSize);

    img_recbt1 = ImageFileFormat::loadFrom(images::bt_rec_1_png, images::bt_rec_1_pngSize);
    img_recbt2 = ImageFileFormat::loadFrom(images::bt_rec_2_png, images::bt_rec_2_pngSize);

    img_fwdbt1 = ImageFileFormat::loadFrom(images::bt_fwd_1_png, images::bt_fwd_1_pngSize);
    img_fwdbt2 = ImageFileFormat::loadFrom(images::bt_fwd_2_png, images::bt_fwd_2_pngSize);

    img_revbt1 = ImageFileFormat::loadFrom(images::bt_rev_1_png, images::bt_rev_1_pngSize);
    img_revbt2 = ImageFileFormat::loadFrom(images::bt_rev_2_png, images::bt_rev_2_pngSize);

    img_endbt1 = ImageFileFormat::loadFrom(images::bt_end_1_png, images::bt_end_1_pngSize);
    img_endbt2 = ImageFileFormat::loadFrom(images::bt_end_2_png, images::bt_end_2_pngSize);

    img_homebt1 = ImageFileFormat::loadFrom(images::bt_home_1_png, images::bt_home_1_pngSize);
    img_homebt2 = ImageFileFormat::loadFrom(images::bt_home_2_png, images::bt_home_2_pngSize);

    img_notebt1 = ImageFileFormat::loadFrom(images::bt_note_1_png, images::bt_note_1_pngSize);
    img_notebt2 = ImageFileFormat::loadFrom(images::bt_note_2_png, images::bt_note_2_pngSize);

    img_metrobt1 = ImageFileFormat::loadFrom(images::bt_metro_1_png, images::bt_metro_1_pngSize);
    img_metrobt2 = ImageFileFormat::loadFrom(images::bt_metro_2_png, images::bt_metro_2_pngSize);

    img_zoomin1 = ImageFileFormat::loadFrom(images::bt_zoomin_1_png, images::bt_zoomin_1_pngSize);
    img_zoomin2 = ImageFileFormat::loadFrom(images::bt_zoomin_2_png, images::bt_zoomin_2_pngSize);

    img_zoomout1 = ImageFileFormat::loadFrom(images::bt_zoomout_1_png, images::bt_zoomout_1_pngSize);
    img_zoomout2 = ImageFileFormat::loadFrom(images::bt_zoomout_2_png, images::bt_zoomout_2_pngSize);

    img_zoomback = ImageFileFormat::loadFrom(images::zoomslider_back_png, images::zoomslider_back_pngSize);

    img_accgb1 = ImageFileFormat::loadFrom(images::acc_genbrowsa_1_png, images::acc_genbrowsa_1_pngSize);
    img_accgb2 = ImageFileFormat::loadFrom(images::acc_genbrowsa_2_png, images::acc_genbrowsa_2_pngSize);

    img_accinstr1 = ImageFileFormat::loadFrom(images::acc_instr_1_png, images::acc_instr_1_pngSize);
    img_accinstr2 = ImageFileFormat::loadFrom(images::acc_instr_2_png, images::acc_instr_2_pngSize);

    img_pattexp1 = ImageFileFormat::loadFrom(images::acc_pattexp_1_png, images::acc_pattexp_1_pngSize);
    img_pattexp2 = ImageFileFormat::loadFrom(images::acc_pattexp_2_png, images::acc_pattexp_2_pngSize);

    img_vols1 = ImageFileFormat::loadFrom(images::bt_vols_1_png, images::bt_vols_1_pngSize);
    img_vols2 = ImageFileFormat::loadFrom(images::bt_vols_2_png, images::bt_vols_2_pngSize);

    img_pans1 = ImageFileFormat::loadFrom(images::bt_pans_1_png, images::bt_pans_1_pngSize);
    img_pans2 = ImageFileFormat::loadFrom(images::bt_pans_2_png, images::bt_pans_2_pngSize);

    img_grid1 = ImageFileFormat::loadFrom(images::bt_grid_1_png, images::bt_grid_1_pngSize);
    img_grid2 = ImageFileFormat::loadFrom(images::bt_grid_2_png, images::bt_grid_2_pngSize);

    img_piano1 = ImageFileFormat::loadFrom(images::bt_pianoroll_1_png, images::bt_pianoroll_1_pngSize);
    img_piano2 = ImageFileFormat::loadFrom(images::bt_pianoroll_2_png, images::bt_pianoroll_2_pngSize);

    img_pr8_1 = ImageFileFormat::loadFrom(images::bt_pr8_1_png, images::bt_pr8_1_pngSize);
    img_pr8_2 = ImageFileFormat::loadFrom(images::bt_pr8_2_png, images::bt_pr8_2_pngSize);

    img_pr10_1 = ImageFileFormat::loadFrom(images::bt_pr10_1_png, images::bt_pr10_1_pngSize);
    img_pr10_2 = ImageFileFormat::loadFrom(images::bt_pr10_2_png, images::bt_pr10_2_pngSize);

    img_pr12_1 = ImageFileFormat::loadFrom(images::bt_pr12_1_png, images::bt_pr12_1_pngSize);
    img_pr12_2 = ImageFileFormat::loadFrom(images::bt_pr12_2_png, images::bt_pr12_2_pngSize);

    img_step1 = ImageFileFormat::loadFrom(images::bt_stepseq_1_png, images::bt_stepseq_1_pngSize);
    img_step2 = ImageFileFormat::loadFrom(images::bt_stepseq_2_png, images::bt_stepseq_2_pngSize);

    img_auto1= ImageFileFormat::loadFrom(images::bt_auto_1_png, images::bt_auto_1_pngSize);
    img_auto2 = ImageFileFormat::loadFrom(images::bt_auto_2_png, images::bt_auto_2_pngSize);

    img_autopt1 = ImageFileFormat::loadFrom(images::bt_autopt_1_png, images::bt_autopt_1_pngSize);
    img_autopt2  = ImageFileFormat::loadFrom(images::bt_autopt_2_png, images::bt_autopt_2_pngSize);

    img_mix1 = ImageFileFormat::loadFrom(images::bt_mix_1_png, images::bt_mix_1_pngSize);
    img_mix2  = ImageFileFormat::loadFrom(images::bt_mix_2_png, images::bt_mix_2_pngSize);

    img_files1 = ImageFileFormat::loadFrom(images::bt_files_1_png, images::bt_files_1_pngSize);
    img_files2 = ImageFileFormat::loadFrom(images::bt_files_2_png, images::bt_files_2_pngSize);

    img_projects1 = ImageFileFormat::loadFrom(images::bt_projs_1_png, images::bt_projs_1_pngSize);
    img_projects2 = ImageFileFormat::loadFrom(images::bt_projs_2_png, images::bt_projs_2_pngSize);

    img_externalplugs1 = ImageFileFormat::loadFrom(images::bt_plugs_1_png, images::bt_plugs_1_pngSize);
    img_externalplugs2 = ImageFileFormat::loadFrom(images::bt_plugs_2_png, images::bt_plugs_2_pngSize);

    img_samples1 = ImageFileFormat::loadFrom(images::bt_wave_1_png, images::bt_wave_1_pngSize);
    img_samples2 = ImageFileFormat::loadFrom(images::bt_wave_2_png, images::bt_wave_2_pngSize);

    img_params1 = ImageFileFormat::loadFrom(images::bt_params_1_png, images::bt_params_1_pngSize);
    img_params2 = ImageFileFormat::loadFrom(images::bt_params_2_png, images::bt_params_2_pngSize);

    img_presets1 = ImageFileFormat::loadFrom(images::bt_presets_1_png, images::bt_presets_1_pngSize);
    img_presets2 = ImageFileFormat::loadFrom(images::bt_presets_2_png, images::bt_presets_2_pngSize);

    img_tracks1 = ImageFileFormat::loadFrom(images::bt_tracks_1_png, images::bt_tracks_1_pngSize);
    img_tracks2 = ImageFileFormat::loadFrom(images::bt_tracks_2_png, images::bt_tracks_2_pngSize);

    img_notracks1 = ImageFileFormat::loadFrom(images::bt_notracks_1_png, images::bt_notracks_1_pngSize);
    img_notracks2 = ImageFileFormat::loadFrom(images::bt_notracks_2_png, images::bt_notracks_2_pngSize);

    img_play2bt1 = ImageFileFormat::loadFrom(images::bt_play2_1_png, images::bt_play2_1_pngSize);
    img_play2bt2 = ImageFileFormat::loadFrom(images::bt_play2_2_png, images::bt_play2_2_pngSize);

    img_stop2bt1 = ImageFileFormat::loadFrom(images::bt_stop2_1_png, images::bt_stop2_1_pngSize);
    img_stop2bt2 = ImageFileFormat::loadFrom(images::bt_stop2_2_png, images::bt_stop2_2_pngSize);

    img_btup1 = ImageFileFormat::loadFrom(images::bt_bpmup_1_png, images::bt_bpmup_1_pngSize);
    img_btup2 = ImageFileFormat::loadFrom(images::bt_bpmup_2_png, images::bt_bpmup_2_pngSize);

    img_btdn1 = ImageFileFormat::loadFrom(images::bt_bpmdn_1_png, images::bt_bpmdn_1_pngSize);
    img_btdn2 = ImageFileFormat::loadFrom(images::bt_bpmdn_2_png, images::bt_bpmdn_2_pngSize);

    img_btforth1 = ImageFileFormat::loadFrom(images::bt_forth_1_png, images::bt_forth_1_pngSize);
    img_btforth2 = ImageFileFormat::loadFrom(images::bt_forth_2_png, images::bt_forth_2_pngSize);

    img_btback1 = ImageFileFormat::loadFrom(images::bt_back_1_png, images::bt_back_1_pngSize);
    img_btback2 = ImageFileFormat::loadFrom(images::bt_back_2_png, images::bt_back_2_pngSize);

    img_btforth21 = ImageFileFormat::loadFrom(images::bt_forth2_1_png, images::bt_forth2_1_pngSize);
    img_btforth22 = ImageFileFormat::loadFrom(images::bt_forth2_2_png, images::bt_forth2_2_pngSize);

    img_btback21 = ImageFileFormat::loadFrom(images::bt_back2_1_png, images::bt_back2_1_pngSize);
    img_btback22 = ImageFileFormat::loadFrom(images::bt_back2_2_png, images::bt_back2_2_pngSize);

    img_btup21 = ImageFileFormat::loadFrom(images::bt_up2_1_png, images::bt_up2_1_pngSize);
    img_btup22 = ImageFileFormat::loadFrom(images::bt_up2_2_png, images::bt_up2_2_pngSize);

    img_btdn21 = ImageFileFormat::loadFrom(images::bt_dn2_1_png, images::bt_dn2_1_pngSize);
    img_btdn22 = ImageFileFormat::loadFrom(images::bt_dn2_2_png, images::bt_dn2_2_pngSize);

    img_btstat1 = ImageFileFormat::loadFrom(images::bt_stat_1_png, images::bt_stat_1_pngSize);
    img_btstat2 = ImageFileFormat::loadFrom(images::bt_stat_2_png, images::bt_stat_2_pngSize);

    img_solooff = ImageFileFormat::loadFrom(images::solo_off_png, images::solo_off_pngSize);
    img_soloon = ImageFileFormat::loadFrom(images::solo_on_png, images::solo_on_pngSize);

    img_muteoff = ImageFileFormat::loadFrom(images::mute_off_png, images::mute_off_pngSize);
    img_muteon = ImageFileFormat::loadFrom(images::mute_on_png, images::mute_on_pngSize);

    img_solo1off = ImageFileFormat::loadFrom(images::solo1_off_png, images::solo1_off_pngSize);
    img_solo1on = ImageFileFormat::loadFrom(images::solo1_on_png, images::solo1_on_pngSize);

    img_mute1off = ImageFileFormat::loadFrom(images::mute1_off_png, images::mute1_off_pngSize);
    img_mute1on = ImageFileFormat::loadFrom(images::mute1_on_png, images::mute1_on_pngSize);

    img_mute1off_g = ImageFileFormat::loadFrom(images::mute1_off_g_png, images::mute1_off_g_pngSize);
    img_mute1off_s = ImageFileFormat::loadFrom(images::mute1_off_s_png, images::mute1_off_s_pngSize);
    img_mute1off_c = ImageFileFormat::loadFrom(images::mute1_off_c_png, images::mute1_off_c_pngSize);

    img_solo1off_g = ImageFileFormat::loadFrom(images::solo1_off_g_png, images::solo1_off_g_pngSize);
    img_solo1off_s = ImageFileFormat::loadFrom(images::solo1_off_s_png, images::solo1_off_s_pngSize);
    img_solo1off_c = ImageFileFormat::loadFrom(images::solo1_off_c_png, images::solo1_off_c_pngSize);

    img_autooff = ImageFileFormat::loadFrom(images::auto_off_png, images::auto_off_pngSize);
    img_autoon = ImageFileFormat::loadFrom(images::auto_on_png, images::auto_on_pngSize);

    img_autooff_g = ImageFileFormat::loadFrom(images::auto_off_g_png, images::auto_off_g_pngSize);
    img_autooff_s = ImageFileFormat::loadFrom(images::auto_off_s_png, images::auto_off_s_pngSize);

    img_knoblarge = ImageFileFormat::loadFrom(images::knob_large_png, images::knob_large_pngSize);
    img_knobxlarge = ImageFileFormat::loadFrom(images::knob_xlarge_base1_png, images::knob_xlarge_base1_pngSize);

    img_bighandle = ImageFileFormat::loadFrom(images::handle_big_png, images::handle_big_pngSize);
    img_bighandle1 = ImageFileFormat::loadFrom(images::handle_big1_png, images::handle_big1_pngSize);
    img_smallhandle = ImageFileFormat::loadFrom(images::handle_small_png, images::handle_small_pngSize);
    img_paramhandle = ImageFileFormat::loadFrom(images::handle_params_png, images::handle_params_pngSize);
    img_tinyhandle1 = ImageFileFormat::loadFrom(images::handle_mini_png, images::handle_mini_pngSize);
    img_tinyhandle2 = ImageFileFormat::loadFrom(images::handle_mini1_png, images::handle_mini1_pngSize);
    img_tinyhandle3 = ImageFileFormat::loadFrom(images::handle_mini2_png, images::handle_mini2_pngSize);
    img_tinyhandle4 = ImageFileFormat::loadFrom(images::handle_mini3_png, images::handle_mini3_pngSize);

    img_genhandle = ImageFileFormat::loadFrom(images::handle_gen1_png, images::handle_gen1_pngSize);
    img_smphandle = ImageFileFormat::loadFrom(images::handle_smp1_png, images::handle_smp1_pngSize);

    img_effbyp1 = ImageFileFormat::loadFrom(images::effbypass_off_png, images::effbypass_off_pngSize);
    img_effbyp2 = ImageFileFormat::loadFrom(images::effbypass_on_png, images::effbypass_on_pngSize);

    img_stepq = ImageFileFormat::loadFrom(images::step_png, images::step_pngSize);

    img_vslpad = ImageFileFormat::loadFrom(images::pad_vslider1_png, images::pad_vslider1_pngSize);
    img_vslpad2 = ImageFileFormat::loadFrom(images::pad_vslider11_png, images::pad_vslider11_pngSize);

    img_vslpad1 = ImageFileFormat::loadFrom(images::pad_vslider2_png, images::pad_vslider2_pngSize);
    img_vslpad11 = ImageFileFormat::loadFrom(images::pad_vslider21_png, images::pad_vslider21_pngSize);

    img_tgparam = ImageFileFormat::loadFrom(images::tgparam_png, images::tgparam_pngSize);
    img_tgparam1 = ImageFileFormat::loadFrom(images::tgparam1_png, images::tgparam1_pngSize);
    img_tgparamoff = ImageFileFormat::loadFrom(images::tgparamoff_png, images::tgparamoff_pngSize);

    img_closew1 = ImageFileFormat::loadFrom(images::bt_closew_1_png, images::bt_closew_1_pngSize);
    img_closew2 = ImageFileFormat::loadFrom(images::bt_closew_2_png, images::bt_closew_2_pngSize);

    img_sunlogo = ImageFileFormat::loadFrom(images::sunlogo_png, images::sunlogo_pngSize);

    img_ctrl1 = ImageFileFormat::loadFrom(images::ctrl1_png, images::ctrl1_pngSize);
    img_ctrl2 = ImageFileFormat::loadFrom(images::ctrl2_png, images::ctrl2_pngSize);
    img_ctrl3 = ImageFileFormat::loadFrom(images::ctrl3_png, images::ctrl3_pngSize);

    img_display = ImageFileFormat::loadFrom(images::disp_png, images::disp_pngSize);

    img_bpmback = ImageFileFormat::loadFrom(images::bpmback_png, images::bpmback_pngSize);
    img_numback = ImageFileFormat::loadFrom(images::numback_png, images::numback_pngSize);

    img_rect11 = ImageFileFormat::loadFrom(images::rect11_png, images::rect11_pngSize);
    img_rect12 = ImageFileFormat::loadFrom(images::rect12_png, images::rect12_pngSize);
    img_rect13 = ImageFileFormat::loadFrom(images::rect13_png, images::rect13_pngSize);

    img_rect21 = ImageFileFormat::loadFrom(images::rect21_png, images::rect21_pngSize);
    img_rect22 = ImageFileFormat::loadFrom(images::rect22_png, images::rect22_pngSize);
    img_rect23 = ImageFileFormat::loadFrom(images::rect23_png, images::rect23_pngSize);

    img_rect31 = ImageFileFormat::loadFrom(images::rect31_png, images::rect31_pngSize);
    img_rect32 = ImageFileFormat::loadFrom(images::rect32_png, images::rect32_pngSize);
    img_rect33 = ImageFileFormat::loadFrom(images::rect33_png, images::rect33_pngSize);

    img_maingrid = NULL;
    img_auxgrid1 = NULL;
    img_auxgrid2 = NULL;
}

void AssignLoadedImages()
{
    for(int mc = 0; mc < NUM_MIXCHANNELS; mc++)
    {
        if(mc < 4)
        {
            gAux->sendchan[mc].kup->SetImages(img_btinsup1, img_btinsup2);
            gAux->sendchan[mc].down->SetImages(img_btinsdown1, img_btinsdown2);
        }
        gAux->mchan[mc].kup->SetImages(img_btinsup1, img_btinsup2);
        gAux->mchan[mc].down->SetImages(img_btinsdown1, img_btinsdown2);
    }
    gAux->masterchan.kup->SetImages(img_btinsup1, img_btinsup2);
    gAux->masterchan.down->SetImages(img_btinsdown1, img_btinsdown2);

    CP->minimize->SetImages(img_minimize1, img_minimize2);
    CP->maximize->SetImages(img_maximize1, img_maximize2);
    CP->close->SetImages(img_close1, img_close2);
    CP->play_butt->SetImages(img_playbt1, img_playbt2);

    CP->stop_butt->SetImages(img_stopbt1, img_stopbt2);
    CP->Record->SetImages(img_recbt1, img_recbt2);
    CP->FFwd->SetImages(img_fwdbt1, img_fwdbt2);
    CP->Rewind->SetImages(img_revbt1, img_revbt2);
    CP->Go2End->SetImages(img_endbt1, img_endbt2);
    CP->Go2Home->SetImages(img_homebt1, img_homebt2);
    CP->NoteMode->SetImages(img_notebt1, img_notebt2);
    CP->MetroMode->SetImages(img_metrobt1, img_metrobt2);
    CP->IncrScale->SetImages(img_zoomin1, img_zoomin2);
    gAux->IncrScale->SetImages(img_zoomin1, img_zoomin2);
    CP->DecrScale->SetImages(img_zoomout1, img_zoomout2);
    gAux->DecrScale->SetImages(img_zoomout1, img_zoomout2);
    CP->AccGenBrowsa->SetImages(img_accgb1, img_accgb2);
    CP->AccInstrPanel->SetImages(img_accinstr1, img_accinstr2);
    gAux->PattExpand->SetImages(img_pattexp1, img_pattexp2);
    gAux->Vols->SetImages(img_vols1, img_vols2);
    gAux->Vols1->SetImages(img_vols1, img_vols2);
    gAux->Pans->SetImages(img_pans1, img_pans2);
    gAux->Pans1->SetImages(img_pans1, img_pans2);
    gAux->PtUsual->SetImages(img_grid1, img_grid2);
    gAux->PtPianorol->SetImages(img_piano1, img_piano2);
    gAux->PR8->SetImages(img_pr8_1, img_pr8_2);
    gAux->PR10->SetImages(img_pr10_1, img_pr10_2);
    gAux->PR12->SetImages(img_pr12_1, img_pr12_2);
    gAux->PtStepseq->SetImages(img_step1, img_step2);
    gAux->PtAuto->SetImages(img_auto1, img_auto2);
    IP->edit_autopattern->SetImages(img_autopt1, img_autopt2);
    CP->view_mixer->SetImages(img_mix1, img_mix2);
    genBrw->ShowFiles->SetImages(img_files1, img_files2);
    mixBrw->ShowFiles->SetImages(img_files1, img_files2);
    genBrw->ShowProjects->SetImages(img_projects1, img_projects2);
    mixBrw->ShowProjects->SetImages(img_projects1, img_projects2);
    genBrw->ShowExternalPlugs->SetImages(img_externalplugs1, img_externalplugs2);
    mixBrw->ShowExternalPlugs->SetImages(img_externalplugs1, img_externalplugs2);
    genBrw->ShowSamples->SetImages(img_samples1, img_samples2);
    mixBrw->ShowSamples->SetImages(img_samples1, img_samples2);
    genBrw->ShowParams->SetImages(img_params1, img_params2);
    mixBrw->ShowParams->SetImages(img_params1, img_params2);
    genBrw->ShowPresets->SetImages(img_presets1, img_presets2);
    mixBrw->ShowPresets->SetImages(img_presets1, img_presets2);
    CP->ShowTrackControls->SetImages(img_tracks1, img_tracks2);
    mixBrw->HideTrackControls->SetImages(img_notracks1, img_notracks2);
    gAux->playbt->SetImages(img_play2bt1, img_play2bt2);
    gAux->stopbt->SetImages(img_stop2bt1, img_stop2bt2);
    CP->bpmer->btup->SetImages(img_btup1, img_btup2);
    CP->tpber->btup->SetImages(img_btup1, img_btup2);
    CP->bpber->btup->SetImages(img_btup1, img_btup2);
    CP->bpmer->btup->SetImages(img_btup1, img_btup2);
    CP->octaver->btup->SetImages(img_btup1, img_btup2);
    gAux->reloctaver->btup->SetImages(img_btup1, img_btup2);
    CP->bpmer->btdn->SetImages(img_btdn1, img_btdn2);
    CP->tpber->btdn->SetImages(img_btdn1, img_btdn2);
    CP->bpber->btdn->SetImages(img_btdn1, img_btdn2);
    CP->octaver->btdn->SetImages(img_btdn1, img_btdn2);
    gAux->reloctaver->btdn->SetImages(img_btdn1, img_btdn2);
    CP->Forth->SetImages(img_btforth1, img_btforth2);
    CP->Back->SetImages(img_btback1, img_btback2);
    gAux->Forth->SetImages(img_btforth21, img_btforth22);
    gAux->Back->SetImages(img_btback21, img_btback22);
    gAux->Up->SetImages(img_btup21, img_btup22);
    gAux->Down->SetImages(img_btdn21, img_btdn22);
    CP->BtFollowPlayback->SetImages(img_btstat1, img_btstat2);
    CP->up->SetImages(img_btup21, img_btup22);
    CP->down->SetImages(img_btdn21, img_btdn22);

    J_RefreshGridImage();
    J_RefreshAuxGridImage();
}


static void JDrawTriangle (Graphics& g, int x1, int y1, int x2, int y2, int x3, int y3, const Colour& fill, const Colour& outline) throw()
{
    Path p;
    p.addTriangle ((float)x1, (float)y1, (float)x2, (float)y2, (float)x3, (float)y3);
    g.setColour (fill);
    g.fillPath (p);

    g.setColour (outline);
    g.strokePath (p, PathStrokeType (0.3f));
}

void JUpArrow(Graphics& g, int x, int y, int side)
{
    JDrawTriangle(g, x, y, x + side, y + side, x - side,y + side, Colour(0xffFFEF2F), Colour(0xffFFEF2F));
}

void JDownArrow(Graphics& g, int x, int y, int side)
{
    JDrawTriangle(g, x, y, x + side, y - side, x - side,y - side, Colour(0xffFFEF2F), Colour(0xffFFEF2F));
}

void J_DRect(Graphics& g, int x1, int y1, int x2, int y2)
{
    //g.draw
}

void J_Line(Graphics& g, float x1, float y1, float x2, float y2)
{
    g.drawLine(x1, y1, x2, y2);
}

void J_Line(Graphics& g, int x1, int y1, int x2, int y2)
{
    g.drawLine((float)x1, (float)y1, (float)x2, (float)y2);
}

void J_Line2(Graphics& g, int x1, int y1, int x2, int y2)
{
    g.drawLine((float)x1, (float)y1, (float)x2, (float)y2, 2.0f);
}

void J_Line2(Graphics& g, float x1, float y1, float x2, float y2)
{
    g.drawLine(x1, y1, x2, y2, 2.0f);
}

void J_Circle(Graphics& g, int x1, int y1, float rad)
{
    g.drawEllipse((float)x1 - rad/2 + 1, (float)y1 - rad/2 + 1, rad, rad, 1.0f);
}

void J_Round(Graphics& g, int x1, int y1, float rad)
{
    g.fillEllipse((float)x1 - rad/2 + 1, (float)y1 - rad/2 + 1, rad, rad);
}

void J_FilledTri(Graphics& g, int x1, int y1, int x2, int y2, int x3, int y3)
{
    Path trip;
    trip.startNewSubPath((float)x1, (float)y1);
    trip.lineTo((float)x2, (float)y2);
    trip.lineTo((float)x3, (float)y3);
    trip.closeSubPath();

    g.fillPath(trip);
}

void J_LineTri(Graphics& g, int x1, int y1, int x2, int y2, int x3, int y3)
{
    J_Line(g, x1, y1, x2, y2);
    J_Line(g, x2, y2, x3, y3);
    J_Line(g, x3, y3, x1, y1);
}

void J_VLine(Graphics& g, int x, int y1, int y2)
{
    if(y2 >= y1)
    {
        g.drawVerticalLine(x, (float)y1, (float)y2);
    }
}

void J_HLine(Graphics& g, int y, int x1, int x2)
{
    if(x2 >= x1)
    {
        g.drawHorizontalLine(y, (float)x1, (float)x2);
    }
}

void J_LineRect(Graphics& g, int x1, int y1, int x2, int y2)
{
    //g.drawRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
    J_HLine(g, y1, x1 + 1, x2);
    J_HLine(g, y2, x1, x2 + 1);
    J_VLine(g, x1, y1, y2);
    J_VLine(g, x2, y1, y2);
}

void J_LineCRect(Graphics& g, int x1, int y1, int x2, int y2)
{
    //g.drawRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
    J_HLine(g, y1, x1 + 1, x2);
    J_HLine(g, y2, x1, x2);
    J_VLine(g, x1, y1, y2);
}

void J_LineGRect(Graphics& g, int x1, int y1, int x2, int y2)
{
    //g.drawRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
    J_HLine(g, y1, x1 + 1, x2);
    J_VLine(g, x1, y1, y2);
}

void J_FillRect(Graphics& g, int x1, int y1, int x2, int y2)
{
    g.fillRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
}

void J_DarkLine(Graphics& g, int x1, int y1, int x2, int y2)
{
    g.setColour(Colour(46, 94, 77));
    J_LineRect(g, x1, y1, x2, y2);
}

void J_DarkLine1(Graphics& g, int x1, int y1, int x2, int y2)
{
    g.setColour(Colour(0x3a000000));
    J_LineRect(g, x1, y1, x2, y2);
    g.setColour(Colour(0xff000000)); // to restore common alfa
}

void J_DarkRect(Graphics& g, int x1, int y1, int x2, int y2)
{
    /*
    g.setColour(Colour(2, 25, 22));
    J_FilledRect(g, x1, y1, x2, y2);

    g.setColour(Colour(5, 68, 60));
    J_LineRect(g, x1, y1, x2, y2);
    */
    g.setColour(Colour(32, 61, 61));
    J_FillRect(g, x1 + 1, y1 + 1, x2 - 1, y2 - 1);
}

void J_LightRect(Graphics& g, int x1, int y1, int x2, int y2)
{
    g.setColour(Colour(0xff0A3A3A));
    J_FillRect(g, x1, y1, x2, y2);

    g.setColour(Colour(0xff2A8A8A));
    J_LineRect(g, x1, y1, x2, y2);
}

void J_DarkRect2(Graphics& g, int x1, int y1, int x2, int y2)
{
    g.setColour(Colour(0xff021916));
    J_FillRect(g, x1, y1, x2, y2);

    g.setColour(Colour(0xff085046));
    J_LineRect(g, x1, y1, x2, y2);
}

void J_PanelLine(Graphics& g, int x, int y, int len)
{
    //g.setColour(Colour(10, 96, 82));
    g.setColour(panelcolour.withBrightness(0.25f));
    J_HLine(g, y, x, x + len - 1);

    //g.setColour(Colour(119, 177, 177));
    g.setColour(panelcolour.withBrightness(0.55f));
    J_HLine(g, y + 1, x, x + len - 1);
}

void J_LRectC(Graphics& g, int x1, int y1, int x2, int y2, Colour& clr)
{
    g.setColour(clr);
    int yc = y1;
    while(yc < y2)
    {
        J_HLine(g, yc, x1, x2);
        yc += 2;
    }
}

void J_LRect(Graphics& g, int x1, int y1, int x2, int y2)
{
    g.setColour(panelcolour.withAlpha(0.1f));
    int yc = y1;
    while(yc < y2)
    {
        J_HLine(g, yc, x1, x2);
        yc += 2;
    }
}

void J_HLineGrad(Graphics& g, int y, int x1, int x2, int incr, Colour clr, float ac = 1.8f)
{
    float a = 1;
    for(int yc = y; a >= 0.05f; yc += incr)
    {
        g.setColour(clr.withAlpha(a));
        J_HLine(g, yc, x1, x2);
        a /= ac;
    }
}

void J_VLineGrad(Graphics& g, int x, int y1, int y2, int incr, Colour clr)
{
    float a = 1;
    for(int xc = x; a >= 0.05f; xc += incr)
    {
        g.setColour(clr.withAlpha(a));
        J_VLine(g, xc, y1, y2);
        a /= 1.8f;
    }
}

void J_PanelRectC(Graphics& g, int x1, int y1, int x2, int y2)
{
    // Main background
    g.setColour(panelcolour);
    g.fillRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);

    g.setColour(Colour(0xffFFFFFF));
    g.drawImageAt(img_sunlogo, x2 - 155, y1 - 55, false);

    // Upper line
    g.setColour(panelcolour.withBrightness(0.7f));
    J_HLine(g, y1 + 1, x1 + 1, x2);

    J_HLineGrad(g, y1 + 2, x1, x2, 1, panelcolour.withBrightness(0.35f));
    J_HLineGrad(g, y1 + 4, x1, x2, 2, panelcolour.withBrightness(0.2f), 1.3f);

    g.setColour(Colours::white.withAlpha(0.4f));
    J_FillRect(g, x1 + 2, y1 + 1, x2 - 2, y1 + 1);

    // Left side line and bottom line 1
    g.setColour(panelcolour.withBrightness(0.5f));
    g.fillRect(x1 + 1, y1 + 2, 2, y2 - y1 - 1);
    J_HLine(g, y2 - 1, x1 + 3, x2);

    // Right side line 2
    g.setColour(panelcolour.withBrightness(0.4f));
    J_VLine(g, x2 - 1, y1 + 1, y2 - 1);

    // Right side line 1 and bottom line 2
    g.setColour(panelcolour.withBrightness(0.27f));
    J_VLine(g, x2, y1 + 1, y2 + 1);
    J_HLine(g, y2, x1, x2);

/*
    GradientBrush gb(Colour(0x32FFFFFF), 0, y1, 
                      Colour(0x00FFFFFF), 0, y2,
                      false);
    g.setBrush(&gb);
    g.fillRect(0, y1, 222, 11);
*/

    J_VLineGrad(g, x1, y1 + 1, y2 - 1, 1, panelcolour.withBrightness(0.7f));
    J_VLineGrad(g, x1 + 1, y1 + 1, y2 - 1, 2, panelcolour.withBrightness(0.6f));
}

void J_PanelRect(Graphics& g, int x1, int y1, int x2, int y2)
{
    g.saveState();
    g.reduceClipRegion(x1, y1, x2 - x1, y2 - y1);

    g.setColour(Colour(0xffFFFFFF));
    g.drawImageAt(img_rect31, x1, y2 - img_rect31->getHeight(), false);
    int x, y;
    y = y2 - img_rect31->getHeight();
    x = x1 + img_rect31->getWidth();
    while(x < x2 - img_rect33->getWidth())
    {
        g.drawImageAt(img_rect32, x, y, false);
        x += img_rect32->getWidth();
    }
    g.drawImageAt(img_rect33, x2 - img_rect33->getWidth(), y, false);

    y -= img_rect31->getHeight();
    while(y > y1)
    {
        x = x1;
        g.drawImageAt(img_rect21, x, y, false);
        x = x1 + img_rect21->getWidth();
        while(x < x2 - img_rect23->getWidth())
        {
            g.drawImageAt(img_rect22, x, y, false);
            x += img_rect22->getWidth();
        }
        g.drawImageAt(img_rect23, x2 - img_rect23->getWidth(), y, false);

        y -= img_rect21->getHeight();
    }

    g.drawImageAt(img_rect11, x1, y1, false);
    x = x1 + img_rect11->getWidth();
    while(x < x2 - img_rect13->getWidth())
    {
        g.drawImageAt(img_rect12, x, y1, false);
        x += img_rect12->getWidth();
    }
    g.drawImageAt(img_rect13, x2 - img_rect13->getWidth(), y1, false);

    g.restoreState();
}

void J_KPanelRect(Graphics& g, int x1, int y1, int x2, int y2)
{
    // Main background
    g.setColour(panelcolour);
    g.fillRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);

/*
    g.setColour(panelcolour.brighter(0.7f));
    g.drawRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);

    g.setColour(panelcolour.brighter(0.1f));
    g.drawRect(x1 + 1, y1 + 1, x2 - x1 - 1, y2 - y1 - 1);
*/

    // Upper line
    g.setColour(panelcolour.withBrightness(0.7f));
    J_HLine(g, y1 + 1, x1 + 1, x2);

    J_HLineGrad(g, y1 + 2, x1, x2, 1, panelcolour.withBrightness(0.75f), 1.2f);
    J_HLineGrad(g, y1 + 6, x1, x2, 2, panelcolour.withBrightness(0.6f), 1.3f);

    g.setColour(Colours::white.withAlpha(0.5f));
    J_FillRect(g, x1 + 2, y1 + 1, x2 - 2, y1 + 1);

    // Left side line and bottom line 1
    g.setColour(panelcolour.withBrightness(0.5f));
    g.fillRect(x1 + 1, y1 + 2, 2, y2 - y1 - 1);
    J_HLine(g, y2 - 1, x1 + 3, x2);

    // Right side line 2
    g.setColour(panelcolour.withBrightness(0.4f));
    J_VLine(g, x2 - 1, y1 + 1, y2 - 1);

    // Right side line 1 and bottom line 2
    g.setColour(panelcolour.withBrightness(0.27f));
    J_VLine(g, x2, y1 + 1, y2 + 1);
    J_HLine(g, y2, x1, x2);


    J_VLineGrad(g, x1, y1 + 1, y2 - 1, 1, panelcolour.withBrightness(0.7f));
    J_VLineGrad(g, x1 + 1, y1 + 1, y2 - 1, 2, panelcolour.withBrightness(0.6f));
 }

void J_PanelRect2(Graphics& g, int x1, int y1, int x2, int y2)
{
    // Main background
    g.setColour(auxcolour);
    g.fillRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);

    // Upper line
    g.setColour(Colour(89, 200, 200));
    J_HLine(g, y1 + 1, x1 + 1, x2);

    // Left side line and bottom line 1
    g.setColour(Colour(45, 165, 155));
    g.fillRect(x1 + 1, y1 + 2, 2, y2 - y1 - 1);
    J_HLine(g, y2 - 1, x1 + 3, x2);

    // Right side line 1
    g.setColour(Colour(30, 86, 82));
    J_VLine(g, x2 - 1, y1 + 2, y2 - 1);

    // Right side line 2 and bottom line 2
    g.setColour(Colour(20, 58, 50));
    J_VLine(g, x2, y1 + 2, y2 - 1);

    J_HLine(g, y2, x1, x2);
}

void J_PanelRect1(Graphics& g, int x1, int y1, int x2, int y2)
{
    // Main background
    g.setColour(Colour(10, 84, 96));
    g.fillRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);

    // Upper line
    g.setColour(Colour(10, 171, 219));
    J_HLine(g, y1 + 1, x1 + 1, x2);

    // Left side line and bottom line 1
    g.setColour(Colour(10, 106, 136));
    g.fillRect(x1 + 1, y1 + 2, 2, y2 - y1 - 1);
    J_HLine(g, y2 - 1, x1 + 3, x2);

    // Right side line 1
	g.setColour(Colour(10, 60, 78));
    J_VLine(g, x2 - 1, y1 + 2, y2 - 1);

    // Right side line 2 and bottom line 2
    g.setColour(Colour(10, 43, 55));
    J_VLine(g, x2, y1 + 2, y2);
    J_HLine(g, y2, x1, x2);
}

void J_PanelRectBW(Graphics& g, int x1, int y1, int x2, int y2)
{
    // Main background
    g.setColour(Colour(63, 63, 63));
    g.fillRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);

    // Upper line
    g.setColour(Colour(133, 133, 133));
    J_HLine(g, y1 + 1, x1 + 1, x2);

    // Left side line and bottom line 1
    g.setColour(Colour(84, 84, 84));
    g.fillRect(x1 + 1, y1 + 2, 2, y2 - y1 - 1);
    J_HLine(g, y2 - 1, x1 + 3, x2);

    // Right side line 1
	g.setColour(Colour(49, 49, 49));
    J_VLine(g, x2 - 1, y1 + 2, y2 - 1);
    
	// Right side line 2 and bottom line 2
    g.setColour(Colour(36, 36, 36));
    J_VLine(g, x2, y1 + 2, y2);
    J_HLine(g, y2, x1, x2);
}

void J_ContextRect(Graphics& g, int x1, int y1, int x2, int y2)
{
    // Main background
    g.setColour(Colour(0xff234E50));
    g.fillRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);

    // Upper and bottom bright line
    g.setColour(Colour(0xff3D9DB5));
    g.drawLine((float)(x1 + 1), (float)(y1 + 1), (float)(x2 - 1), (float)(y1 + 1));
    g.drawLine((float)(x1 + 1), (float)(y2 - 1), (float)(x2 - 1), (float)(y2 - 1));

    g.setColour(Colour(0xff194042));
    g.drawRect(x1, y1 + 2, x2 - x1 + 1, y2 - y1 - 4 + 1);
    //g.drawRect(x1 + 1, y1 + 3, x2 - x1 - 1, y2 - y1 - 6 + 1);
}

int J_TextBase_xy(Graphics& g, int x, int y, const char* str)
{
    g.setFont(*ari);
    String s = String(str);
    g.drawSingleLineText(s, x, y);
    return ari->getStringWidth(s);
}

int J_TextProj_xy(Graphics& g, int x, int y, const char* str)
{
    g.setFont(*prj);
    String s = String(str);
    g.drawSingleLineText(s, x, y);
    return prj->getStringWidth(s);
}

int J_TextBold_xy(Graphics& g, int x, int y, const char* str)
{
    g.setFont(*bld);
    String s = String(str);
    g.drawSingleLineText(s, x, y);
    return bld->getStringWidth(s);
}

int J_TextRox_xy(Graphics& g, int x, int y, const char* str)
{
    g.setFont(*rox);
    String s = String(str);
    g.drawSingleLineText(s, x, y);
    return rox->getStringWidth(s);
}

int J_TextDix_xy(Graphics& g, int x, int y, const char* str)
{
    g.setFont(*dix);
    String s = String(str);
    g.drawSingleLineText(s, x, y);
    return dix->getStringWidth(s);
}

int J_TextFix_xy(Graphics& g, int x, int y, const char* str)
{
    g.setFont(*fix);
    String s = String(str);
    g.drawSingleLineText(s, x, y);
    return fix->getStringWidth(s);
}

void J_TextBase_type(Graphics& g, const char *str)
{
    g.setFont(*ari);
	String s = String(str);
    g.drawSingleLineText(s, CursX, CursY);
	CursX += ari->getStringWidth(s);;
}

int J_TextSmall_xy(Graphics& g, int x, int y, const char *str)
{
    g.setFont(*ti);
	String s = String(str);
    g.drawSingleLineText(s, x, y);
	return ti->getStringWidth(s);
}

int J_TextSmall_type(Graphics& g, const char *str)        // Заказная функция "Печати" GL
{
    g.setFont(*ti);
	String s = String(str);
    g.drawSingleLineText(s, CursX, CursY);
    int w = ti->getStringWidth(s);
	CursX += w;
	return w;
}

int J_TextSmallWidth(const char *str)
{
	String s = String(str);
    return ti->getStringWidth(s);
}

int J_TextInstrWidth(const char *str)
{
	String s = String(str);
    return ins->getStringWidth(s);
}

int J_TextBaseWidth(const char *str)
{
	String s = String(str);
    return ari->getStringWidth(s);
}

int J_TextInstr_type(Graphics& g, const char *str)
{
    g.setFont(*ins);
	String s = String(str);
    g.drawSingleLineText(s, CursX, CursY);
    int w = ins->getStringWidth(s);
	CursX += w;
	return w;
}

int J_TextInstr_xy(Graphics& g, int x, int y, const char *str)
{
    g.setFont(*ins);
	String s = String(str);
	g.drawSingleLineText(s, x, y);
	return ins->getStringWidth(s);
}

int J_TextBig_xy(Graphics& g, int x, int y, const char *str)
{
    g.setFont(*taho);
	String s = String(str);
	g.drawSingleLineText(s, x, y);
	return taho->getStringWidth(s);
}

int J_TextBig_type(Graphics& g, const char *str)
{
    g.setFont(*taho);
	String s = String(str);
    g.drawSingleLineText(s, CursX, CursY);
    int w = taho->getStringWidth(s);
	CursX += w;
	return w;
}

int J_DigitStr_xy(Graphics& g, int x, int y, DigitStr* dg, bool bright, Colour clr = Colour(0x00000000))
{
    dg->x = x;
    dg->y = y;
    dg->bright = bright;

    dg->edx = 18;
    dg->edy = 11;

    char c1[2] = {dg->digits[0], 0};
    char c2[2] = {dg->digits[1], 0};

    int w1 = ins->getStringWidth(String(c1));
    int w2 = ins->getStringWidth(String(c2));

    if(dg->instr != NULL && false)
    {
        if(dg->instr->type == Instr_Sample)
        {
            g.setColour(Colour(35, 67, 71));
        }
        else
        {
            g.setColour(Colour(36, 61, 89));
        }
        g.fillRect(x - 1, y - 9, 18, 10);
    }
    else if(dg->send != NULL && false)
    {
        g.setColour(Colour(16, 29, 33));
        g.fillRect(x - 1, y - 10, 18, 11);
    }
    else
    {
        if(dg->bground == NULL)
            MC->MakeSnapshot(&dg->bground, x - 1, y - dg->edy + 1, dg->edx, dg->edy);
        else
            g.drawImageAt(dg->bground, x - 1, y - dg->edy + 1, false);

        //g.setColour(clr);
        //g.fillRect(x - 1, y - 10, 18, 11);
    }

    if(dg == C.curParam)
    {
        g.setColour(Colour(0x5f55FF55));
        if(dg->curPos == 0)
        {
            g.fillRect(dg->x - 1, dg->y - 9, w1 + 1, 10);
        }
        else if(dg->curPos == 1)
        {
            g.fillRect(dg->x + w1 - 1, dg->y - 9, w2 + 1, 10);
        }
    }

    if(bright)
    {
        if(c1[0] == '-' || c2[0] == '-')
        {
            g.setColour(Colour(135, 135, 135));
        }
        else if(dg->mchanout == &gAux->masterchan || dg->mchanout == NULL)
        {
            g.setColour(Colour(195, 65, 65));
        }
        else
        {
            g.setColour(Colour(175, 235, 235));
        }
    }
    else
    {
        g.setColour(Colour(51, 72, 62));
    }

    return J_TextFix_xy(g, x, y, dg->digits);
}

int J_TString_Instr_xy(Graphics& g, int x, int y, TString* str)
{
    str->x = x;
    str->y = y;

    char tc = str->string[str->curPos];
    str->string[str->curPos] = 0;
    str->cposx = ins->getStringWidth(String(str->string));
    str->string[str->curPos] = tc;

    return J_TextInstr_xy(g, x, y, str->string);
}

int J_TString_Small_xy(Graphics& g, int x, int y, TString* str)
{
    str->x = x;
    str->y = y;

    char tc = str->string[str->curPos];
    str->string[str->curPos] = 0;
    str->cposx = ti->getStringWidth(String(str->string));
    str->string[str->curPos] = tc;

    return J_TextSmall_xy(g, x, y, str->string);
}
    
int J_TString_Base_xy(Graphics& g, int x, int y, TString* str)
{
    str->x = x;
    str->y = y;

    char tc = str->string[str->curPos];
    str->string[str->curPos] = 0;
    str->cposx = ari->getStringWidth(String(str->string));
    str->string[str->curPos] = tc;

    return (int)J_TextBase_xy(g, x, y, str->string);
}

int J_TString_Instr_type(Graphics& g, TString* str, Image* img = NULL)
{
    str->x = CursX;
    str->y = CursY;

    int ow;
    if(img == NULL)
    {
        ow = J_TextInstr_type(g, str->string);
    }
    else
    {
        ow = ins->getStringWidth(str->string);
        CursX += ow;
        g.drawImageAt(img, str->x, str->y - (int)ins->getHeight(), false);
    }

    str->cposx = ow;
    return ow;
}

int J_TString_Small_type(Graphics& g, TString* str, Image* img = NULL)
{
    str->x = CursX;
    str->y = CursY;

    int ow;
    if(img == NULL)
    {
        ow = J_TextSmall_type(g, str->string);
    }
    else
    {
        ow = ti->getStringWidth(str->string);
        CursX += ow;
        g.drawImageAt(img, str->x, str->y - (int)ti->getHeight(), false);
    }

    str->cposx = ow;
    return ow;
}

void J_String_Instr_Ranged(Graphics& g, int x, int y, char* str, int maxwidth)
{
    char buff[MAX_NAME_STRING];
    memset(buff, 0, MAX_NAME_STRING);
    strcpy(buff, str);

    int len = strlen(str);
    int pos = len;
    int wi = ins->getStringWidth(String(str));
    int ww = ins->getStringWidth(String("..."));
    while((wi > (maxwidth - ww))&&(pos >= 0))
    {
        wi -= ins->getStringWidth(&(buff[pos]));
        buff[pos] = 0;
        pos--;
    }

    if(pos >= 0)
    {
        if(pos < len)
        {
            strcat(buff, "...");
        }
        J_TextInstr_xy(g, x, y, buff);
    }
}

void J_String_Small_Ranged(Graphics& g, int x, int y, char* str, int maxwidth)
{
    char buff[MAX_NAME_STRING];
    memset(buff, 0, MAX_NAME_STRING);
    strcpy(buff, str);

    int len = strlen(str);
    int pos = len;
    int wi = ti->getStringWidth(String(str));
    int ww = ti->getStringWidth(String("..."));
    while((wi > (maxwidth - ww))&&(pos >= 0))
    {
        wi -= ti->getStringWidth(&(buff[pos]));
        buff[pos] = 0;
        pos--;
    }

    if(pos >= 0)
    {
        if(pos < len)
        {
            strcat(buff, "...");
        }
        J_TextSmall_xy(g, x, y, buff);
    }
}

void J_String_InstrB_Ranged(Graphics& g, int x, int y, char* str, int maxwidth)
{
    char buff[MAX_NAME_STRING];
    memset(buff, 0, MAX_NAME_STRING);
    strcpy(buff, str);

    int len = strlen(str);
    int pos = len;
    int wi = ins->getStringWidth(String(str));
    int ww = ins->getStringWidth(String("..."));
    while((wi > (maxwidth - ww))&&(pos >= 0))
    {
        wi -= ins->getStringWidth(String(buff[pos]));
        buff[pos] = 0;
        pos--;
    }

    if(pos >= 0)
    {
        if(pos < len)
        {
            strcat(buff, "...");
        }
        J_TextInstr_xy(g, x, y, buff);
    }
}

void J_String_Base_Ranged(Graphics& g, int x, int y, char* str, int maxwidth)
{
    char buff[MAX_NAME_STRING];
    memset(buff, 0, MAX_NAME_STRING);
    strcpy(buff, str);

    int len = strlen(str);
    int pos = len;
    int wi = ari->getStringWidth(String(str));
    int ww = ari->getStringWidth(String("..."));
    while((wi > maxwidth)&&(pos >= 0))
    {
        wi = ari->getStringWidth(String(buff));
        buff[pos] = 0;
        pos--;
    }

    if(pos >= 0)
    {
        if(pos < len)
        {
            strcat(buff, "...");
        }
        J_TextBase_xy(g, x, y, buff);
    }
}

void J_String_Proj_Ranged(Graphics& g, int x, int y, char* str, int maxwidth)
{
    char buff[MAX_NAME_STRING];
    memset(buff, 0, MAX_NAME_STRING);
    strcpy(buff, str);

    int len = strlen(str);
    int pos = len;
    int wi = prj->getStringWidth(String(str));
    int ww = prj->getStringWidth(String("..."));
    while((wi > maxwidth)&&(pos >= 0))
    {
        wi = prj->getStringWidth(String(buff));
        buff[pos] = 0;
        pos--;
    }

    if(pos >= 0)
    {
        if(pos < len)
        {
            strcat(buff, "...");
        }
        J_TextProj_xy(g, x, y, buff);
    }
}

void J_String_Bold_Ranged(Graphics& g, int x, int y, char* str, int maxwidth)
{
    char buff[MAX_NAME_STRING];
    memset(buff, 0, MAX_NAME_STRING);
    strcpy(buff, str);

    int len = strlen(str);
    int pos = len;
    int wi = bld->getStringWidth(String(str));
    int ww = bld->getStringWidth(String("..."));
    while((wi > maxwidth)&&(pos >= 0))
    {
        wi = bld->getStringWidth(String(buff));
        buff[pos] = 0;
        pos--;
    }

    if(pos >= 0)
    {
        if(pos < len)
        {
            strcat(buff, "...");
        }
        J_TextBold_xy(g, x, y, buff);
    }
}

void J_String_Fix_Ranged(Graphics& g, int x, int y, char* str, int maxwidth)
{
    char buff[MAX_NAME_STRING];
    memset(buff, 0, MAX_NAME_STRING);
    strcpy(buff, str);

    int len = strlen(str);
    int pos = len;
    int wi = fix->getStringWidth(String(str));
    int ww = fix->getStringWidth(String("..."));
    while((wi > maxwidth)&&(pos >= 0))
    {
        wi = fix->getStringWidth(String(buff));
        buff[pos] = 0;
        pos--;
    }

    if(pos >= 0)
    {
        if(pos < len)
        {
            strcat(buff, "...");
        }
        J_TextFix_xy(g, x, y, buff);
    }
}

void J_String_Dix_Ranged(Graphics& g, int x, int y, char* str, int maxwidth)
{
    char buff[MAX_NAME_STRING];
    memset(buff, 0, MAX_NAME_STRING);
    strcpy(buff, str);

    int len = strlen(str);
    int pos = len;
    int wi = dix->getStringWidth(String(str));
    int ww = dix->getStringWidth(String("..."));
    while((wi > maxwidth)&&(pos >= 0))
    {
        wi = dix->getStringWidth(String(buff));
        buff[pos] = 0;
        pos--;
    }

    if(pos >= 0)
    {
        if(pos < len)
        {
            strcat(buff, "...");
        }
        J_TextDix_xy(g, x, y, buff);
    }
}

void J_String_Rox_Ranged(Graphics& g, int x, int y, char* str, int maxwidth)
{
    char buff[MAX_NAME_STRING];
    memset(buff, 0, MAX_NAME_STRING);
    strcpy(buff, str);

    int len = strlen(str);
    int pos = len;
    int wi = rox->getStringWidth(String(str));
    int ww = rox->getStringWidth(String("..."));
    while((wi > maxwidth)&&(pos >= 0))
    {
        wi = rox->getStringWidth(String(buff));
        buff[pos] = 0;
        pos--;
    }

    if(pos >= 0)
    {
        if(pos < len)
        {
            strcat(buff, "...");
        }
        J_TextRox_xy(g, x, y, buff);
    }
}

int J_ScopeWidth(Scope* scope)
{
    int w = 0;
    if(scope->mixcell != NULL)
    {
        w += J_TextSmallWidth(scope->mixcell->indexstr);
        w += J_TextInstrWidth(".");

        if(scope->eff != NULL)
        {
    		if(scope->eff->category == EffCategory_Effect)
    		{
    			w += J_TextInstrWidth(scope->eff->name);
    			w += J_TextInstrWidth(".");
    		}
    		else if(scope->eff->category == EffCategory_Generator)
    		{
    			w += J_TextInstrWidth(scope->eff->arec->alias);
    			w += J_TextInstrWidth(".");
    		}
        }
    }
    else if(scope->local == true)
    {
        w += J_TextInstrWidth("@");
        w += J_TextInstrWidth(".");
    }
    else if(scope->for_track == true)
    {
        if(scope->trkdata != NULL)
        {
            w += J_TextBaseWidth(String::formatted(T("%.3d"), scope->trkdata->trknum));
        }
        else
        {
            w += J_TextInstrWidth("=");
        }
        w += J_TextInstrWidth(".");
    }
    else if(scope->pt != NULL)
    {
        w += J_TextInstrWidth(scope->pt->name->string);
        w += J_TextInstrWidth(".");
    }
    else if(scope->instr != NULL)
    {
        w += J_TextInstrWidth(scope->instr->alias->string);
        w += J_TextInstrWidth(".");
    }
    else
    {
        w += J_TextInstrWidth(":");
    }

    return w;
}

void J_Scope_type(Graphics& g, Scope* scope)
{
    g.setColour(Colour(0xffFFFFFF));
    if(scope->mixcell != NULL)
    {
        if(scope->mixcell->mchan != NULL)
        {
            g.setColour(Colour(0xffFF4646));
            J_TextSmall_type(g, scope->mixcell->mchan->indexstr);
            J_TextSmall_type(g, ".");
        }
        //g.setColour(Colour(0xffFF4646));
        //J_TextSmall_type(g, scope->mixcell->indexstr);
        //J_TextInstr_type(g, ".");

        if(scope->eff != NULL && scope->eff->to_be_deleted == false)
        {
    		if(scope->eff->category == EffCategory_Effect)
    		{
                g.setColour(Colour(0xffFFFFFF));
    			J_TextSmall_type(g, scope->eff->name);
    			J_TextSmall_type(g, ".");
    		}
    		else if(scope->eff->category == EffCategory_Generator)
    		{
                g.setColour(Colour(0xff6496FF));
    			J_TextSmall_type(g, scope->eff->arec->alias);
    			J_TextSmall_type(g, ".");
    		}
        }
    }
    else if(scope->local == true)
    {
        J_TextSmall_type(g, "@");
        J_TextSmall_type(g, ".");
    }
    else if(scope->for_track == true)
    {
        g.setColour(Colour(0xff46FF46));
        if(scope->trkdata != NULL)
        {
            String fmtstr = String::formatted(T("%.3d"), scope->trkdata->trknum);
            g.setFont(*ari);
            g.drawSingleLineText(fmtstr, CursX, CursY);
            CursX += ari->getStringWidth(fmtstr);
        }
        else
        {
            J_TextSmall_type(g, "=");
        }
        J_TextSmall_type(g, ".");
    }
    else if(scope->pt != NULL)
    {
        J_TString_Small_type(g, scope->pt->name);
        J_TextSmall_type(g, ".");
    }
    else if(scope->instr != NULL)
    {
        if(scope->instr->alimg != NULL)
        {
            g.drawImageAt(scope->instr->alimg, CursX, CursY - (int)ins->getHeight(), false);
            CursX += ins->getStringWidth(scope->instr->alias->string);
        }

        J_TextSmall_type(g, ".");
    }
    else
    {
        //CursX--;
        //J_TextInstr_type(g, ":");
    }
}

int J_note_string(Graphics& g, int x, int y, int note, bool relative)
{
    bool neg = false;
    int nnum = note % 12;
	if(note < 0)
    {
        neg = true;
		nnum = 12 + nnum;
		nnum = nnum % 12;
    }
    int onum = note / 12;
	if(neg == true)
    {
        onum = abs(onum);
		if(nnum > 0)
			onum += 1;
    }
    int nw = J_TextSmall_xy(g, x, y, note_table[abs(nnum)]);
    char oct[3];
    Num2String(onum, oct);
    if(neg)
    {
        oct[1] = oct[0];
        oct[0] = '-';
        oct[2] = 0;
    }
    nw += J_TextSmall_xy(g, x + nw, y, oct);
    return nw;
}

void J_Note_xy(Graphics& g, int x, int y, Note* n)
{
    n->x = x;
    n->y = y;

    int cx = CursX;
    int cy = CursY;

    CursX = x;
    CursY = y;

    if(n->relative == false)
        g.setColour(Colour(0xffFF0000));
    else
        g.setColour(Colour(0xffFFAF00));
    n->pixwidth = J_note_string(g, x, y, n->value, n->relative);
    CursX = cx;
    CursY = cy;
}

void J_Note_type(Graphics& g, Note* n)
{
    n->x = CursX;
    n->y = CursY;

    if(n->relative == false)
        g.setColour(Colour(0xffFF0000));
    else
        g.setColour(Colour(0xffFFAF00));
    n->pixwidth = J_note_string(g, n->x, n->y, n->value, n->relative);
    CursX += n->pixwidth;;
}

void J_Percent_Instr_type(Graphics& g, Percent* p)
{
    p->x = CursX;
    p->y = CursY;

    g.setColour(Colour(0xffDCDCDC));
    J_TextInstr_type(g, (const char*)p->digits);
    J_TextInstr_type(g, "%");
}

void J_Percent_Patt_type(Graphics& g, Percent* p)
{
    p->x = CursX;
    p->y = CursY;

    g.setColour(Colour(0xffDCDCDC));
    J_TextInstr_type(g, (const char*)p->digits);
    J_TextInstr_type(g, "%");
}

void J_EnvLane(Graphics& g, Envelope* env, Loc loc, Trk* trk, Pattern* pt, int lx1, int ly1, int lx2, int ly2)
{
    env->x = lx1;
    env->y = ly1;
    env->hgt = ly2 - ly1;
    float scale = env->GetScale(loc);
    env->scale = scale;
    env->len = (float)(lx2 - lx1)/scale;
    env->edx = lx2 - lx1;
    float half = (float)(ly2 - ly1)/2;
	EnvPnt* p;
    float hincr;
    if(env->ctype == Cmd_PitchEnv)
    {
        g.setColour(Colour(165, 25, 25));
        J_Line(g, lx1, (int)(ly2 - half), lx2, (int)(ly2 - half));
        g.setColour(Colour(100, 10, 10));
        J_Line(g, lx1, (int)(ly2 - half) - 1, lx2, (int)(ly2 - half) - 1);
        g.setColour(Colour(100, 10, 10));
        J_Line(g, lx1, (int)(ly2 - half) + 1, lx2, (int)(ly2 - half) + 1);

        p = env->p_first;
        g.setColour(Colour((uint8)195, (uint8)195, (uint8)195, (uint8)100));
        while(p != NULL)
        {
            if(p->x <= env->len && (p->next == NULL || p->next->x <= env->len))
            {
                if(p->next != NULL)
                {
                    Path path;
                    hincr = p->y_norm*(float)env->hgt >= half ? 0.f : 1.f;
                    path.startNewSubPath((float)(lx1 + p->x*scale), ly2 + hincr - half);
                    path.lineTo((float)(lx1 + p->x*scale), (float)(ly2 + hincr - p->y_norm*(float)env->hgt));

                    hincr = p->next->y_norm*(float)env->hgt >= half ? 0.f : 1.f;
                    path.lineTo((float)(lx1 + p->next->x*scale), (float)(ly2 + hincr - p->next->y_norm*(float)env->hgt));
                    path.lineTo((float)(lx1 + p->next->x*scale), (float)ly2 + hincr - half);
                    path.closeSubPath();
                    g.fillPath(path);
                }
                else
                {
                    Path path;
                    hincr = p->y_norm*(float)env->hgt >= half ? 0.f : 1.f;
                    path.startNewSubPath((float)(lx1 + p->x*scale), (float)ly2 + hincr - half);
                    path.lineTo((float)(lx1 + p->x*scale), (float)(ly2 + hincr - p->y_norm*(float)env->hgt));
                    path.lineTo((float)lx2, (float)(ly2 + hincr - p->y_norm*(float)env->hgt));
                    path.lineTo((float)lx2, (float)ly2 + hincr - half);
                    path.closeSubPath();
                    g.fillPath(path);
                }
            }
            else
            {
                float px1 = p->x;
                float py1 = p->y_norm;
                float px2;
                float py2;

                if(p->next != NULL)
                {
                    px2 = p->next->x;
                    py2 = p->next->y_norm;
                }
                else
                {
                    break;
                    //px2 = p->x;
                    //py2 = p->y_norm;
                }

                float py3 = Interpolate_Line(px1, py1, px2, py2, env->len);

                Path path;
                hincr = py1*(float)env->hgt >= half ? 0.f : 1.f;
                path.startNewSubPath((float)(lx1 + px1*scale), (float)ly2 + hincr - half);
                path.lineTo((float)(lx1 + px1*scale), (float)(ly2 + hincr - py1*(float)env->hgt));
                hincr = py3*(float)env->hgt >= half ? 0.f : 1.f;
                path.lineTo((float)lx2, (float)(ly2 + hincr - py3*(float)env->hgt));
                path.lineTo((float)lx2, (float)ly2 + hincr - half);
                path.closeSubPath();
                g.fillPath(path);

                break;
            }
            p = p->next;
        }
    }
    else
    {
        p = env->p_first;
        g.setColour(Colour((uint8)165, (uint8)165, (uint8)165, (uint8)100));
        while(p != NULL)
        {
            if(p->x <= env->len && (p->next == NULL || p->next->x <= env->len))
            {
                if(p->next != NULL)
                {
                    Path path;
                    path.startNewSubPath((float)(lx1 + p->x*scale), (float)ly2 + 1);
                    path.lineTo((float)(lx1 + p->x*scale), (float)(ly2 - p->y_norm*(float)env->hgt));
                    path.lineTo((float)(lx1 + p->next->x*scale), (float)(ly2 - p->next->y_norm*(float)env->hgt));
                    path.lineTo((float)(lx1 + p->next->x*scale), (float)ly2 + 1);
                    path.closeSubPath();
                    g.fillPath(path);
                }
                else
                {
                    Path path;
                    path.startNewSubPath((float)(lx1 + p->x*scale), (float)ly2 + 1);
                    path.lineTo((float)(lx1 + p->x*scale), (float)(ly2 - p->y_norm*(float)env->hgt));
                    path.lineTo((float)lx2, (float)(ly2 - p->y_norm*(float)env->hgt));
                    path.lineTo((float)lx2, (float)ly2 + 1);
                    path.closeSubPath();
                    g.fillPath(path);
                }
            }
            else
            {
                float px1 = p->x;
                float py1 = p->y_norm;
                float px2;
                float py2;

                if(p->next != NULL)
                {
                    px2 = p->next->x;
                    py2 = p->next->y_norm;
                }
                else
                {
                    break;
                    //px2 = p->x;
                    //py2 = p->y_norm;
                }

                float py3 = Interpolate_Line(px1, py1, px2, py2, env->len);

                Path path;
                path.startNewSubPath((float)(lx1 + px1*scale), (float)ly2 + 1);
                path.lineTo((float)(lx1 + px1*scale), (float)(ly2 - py1*(float)env->hgt));
                path.lineTo((float)lx2, (float)(ly2 - py3*(float)env->hgt));
                path.lineTo((float)lx2, (float)ly2 + 1);
                path.closeSubPath();
                g.fillPath(path);

                break;
            }
            p = p->next;
        }
    }

    p = env->p_first;
    while(p != NULL)
    {
        g.setColour(Colour(0xffDCDCDC));
        if(p->x <= env->len)
        {
            J_Round(g, (int)(lx1 + p->x*scale), (int)(ly2 - p->y_norm*(float)env->hgt), 4);
            if(M.mmode & MOUSE_ENVELOPING && M.env_action == ENVPOINTING && M.active_pnt == p)
            {
                J_Circle(g, (int)(lx1 + p->x*scale), (int)(ly2 -  p->y_norm*(float)env->hgt), 7);
            }

            if(p != env->p_first)
            {
                J_Line(g, lx1 + p->x*scale, ly2 - p->y_norm*(float)env->hgt, 
                          lx1 + p->prev->x*scale, ly2 - p->prev->y_norm*(float)env->hgt);
            }
        }
        else // invisible point. draw last cut line and break
        {
			if(p != env->p_first)
			{
				float px1 = p->prev->x;
				float px2 = p->x;

				float py1 = p->prev->y_norm;
				float py2 = p->y_norm;

				float py3 = Interpolate_Line(px1, py1, px2, py2, env->len);

				J_Line(g, (float)(lx1 + px1*scale), (float)(ly2 - py1*(float)env->hgt),
						  (float)lx2, (float)(ly2 - py3*(float)env->hgt));
			}
            break;
        }
        p = p->next;
    }
}

void J_EnvLen(Graphics& g, int x, int y, Envelope* env, CmdType type, Loc loc)
{
    float scale = env->GetScale(loc);
    env->scale = scale;
    env->edx = RoundFloat(env->len*scale);
    env->x = x;
    env->y = y + 1;

    switch(type)
    {
        case Cmd_VolEnv:
        case Cmd_LocVolEnv:
            g.setColour(Colour(0xef39395D));
            break;
        case Cmd_PanEnv:
        case Cmd_LocPanEnv:
            g.setColour(Colour(0xef2F5454));
            break;
        case Cmd_ParamEnv:
            g.setColour(Colour(0xef4A4A4A));
            break;
    }

    g.fillRect(x, y - 9, env->edx, lineHeight - 4);

    g.setColour(Colour(0x4fFFFFFF));
    J_HLine(g, y - 9, x, x + env->edx);
    g.setColour(Colour(0x25FFFFFF));
    J_VLine(g, x, y - 9, y + 1);
    J_VLine(g, x + env->edx - 1, y - 8, y + 1);
    g.setColour(Colour(0x15FFFFFF));
    J_HLine(g, y, x, x + env->edx);
    g.setColour(Colour(0x25FFFFFF));
    J_HLine(g, y - 8, x, x + env->edx);
    g.setColour(Colour(0x15FFFFFF));
    J_HLine(g, y - 7, x, x + env->edx);
}

void J_TimeFold(Graphics& g, int x, int y, Toggle* tf)
{
    tf->x = x;
    tf->y = y - 7;
    tf->width = 36;
    tf->height = 7;

    g.setColour(Colour(0xff1CDC1C));
    if(*(tf->state) == true)
    {
        J_TextSmall_xy(g, x, y, "seconds");
    }
    else
    {
        J_TextSmall_xy(g, x, y, "BPM");
    }
}

void J_EnvFold(Graphics& g, int x, int y, Toggle* ef)
{
    ef->x = x - 2;
    ef->y = y - 1;
    ef->width = 10;
    ef->height = 7;

    g.setColour(Colour(0xffDCDC00));
    if(*(ef->state) == true)
    {
        J_Line(g, x, y + 3, x + 3, y);
        J_Line(g, x + 3, y, x + 7, y + 4);
    }
    else
    {
        J_Line(g, x, y, x + 3, y + 3);
        J_Line(g, x + 3, y + 3, x + 7, y - 1);
    }
/*
    g.setColour(Colour(0xff000000));
    J_FilledRect(g, x + 2, y + 1, x + 6, y + 7);

    if(*(ef->state) == true)
    {
        g.setColour(Colour(0xffDCDCDC));
        J_FilledRect(g, x + 2, y + 2, x + 6, y + 6);
    }

    g.setColour(Colour(0xff9B9B9B));
    J_LineRect(g, x, y, x + 8, y + 8);
*/
}

void J_EnvSize(int* w, int* h, Envelope* env, Loc loc)
{
    float scale = env->GetScale(loc);
    env->scale = scale;
   *w = RoundFloat(env->len*scale);

   *h = env->hgt;
}

void J_Env(Graphics& g, int x, int y, Envelope* env, CmdType type, Loc loc)
{
    EnvPnt* p;
    int yb = y;
    env->x = x;
    env->y = yb;

    float scale = env->GetScale(loc);
    env->scale = env->GetScale(loc);;
    env->edx = RoundFloat(env->len*scale);

    Colour clr;
    Colour clr1;
    switch(type)
    {
        case Cmd_VolEnv:
        case Cmd_LocVolEnv:
            clr = Colour(0xff29295D);
            clr1 = Colour(0xff4A4A7F);
            break;
        case Cmd_PanEnv:
        case Cmd_LocPanEnv:
            clr = Colour(0xff1F4444);
            clr1 = Colour(0xff256565);
            break;
        case Cmd_ParamEnv:
            clr = Colour(0xff3A3A3A);
            clr1 = Colour(0xff6A6A6A);
            break;
    }

    //g.setColour(clr);
    //g.drawRect(x, yb - 1, env->edx, env->hgt + 2);
    g.setColour(clr.withAlpha(0.6f));
    g.fillRect(x, yb, env->edx, env->hgt + 1);

    if(env->timebased == true)
    {
        g.setColour(Colour(0xff00FFFF));
        g.setFont(*ti);
        int tx = x + 1;
        int v = 0;
        while(tx < x + env->edx)
        {
            //String fmtstr = String::formatted(T("%d"), v);
            //g.drawSingleLineText(fmtstr, tx, yb + env->hgt - 1);
            J_VLine(g, tx, yb + env->hgt, yb + env->hgt - 4);
            tx += RoundFloat(scale);
            v++;
        }
    }

    if(type == Cmd_VolEnv || type == Cmd_LocVolEnv)
    {
        g.setColour(clr1.withAlpha(0.75f));
        int lhgt = RoundFloat(env->hgt*(1 - 1/VolRange));
        J_HLine(g, yb + lhgt, x + 1, x + env->edx - 1);
    }
    else if(type == Cmd_PanEnv || type == Cmd_LocPanEnv)
    {
        g.setColour(clr1.withAlpha(0.75f));
        J_HLine(g, RoundFloat(yb + (float)env->hgt/2), x, x + env->edx);
    }

    if(type != Cmd_PanEnv && type != Cmd_LocPanEnv)
    {
        g.setColour(Colour(0x1fFFFFFF));
        Path path;
        p = env->p_first;
        if(p != NULL)
		{
            path.startNewSubPath((float)(x + p->x*scale), (float)(yb + env->hgt));
            path.lineTo((float)(x + p->x*scale), (float)(yb + env->hgt - p->y_norm*(float)env->hgt));
		}
        while(p != NULL)
        {
            if(p->deleted == false)
            {
                if(p->x <= env->len && (p->next == NULL || p->next->x <= env->len))
                {
                    if(p->next != NULL)
                    {
                        path.lineTo((float)(x + p->next->x*scale), (float)(yb + env->hgt - p->next->y_norm*(float)env->hgt));
                    }
                    else
                    {
                        path.lineTo((float)(x + env->edx), (float)(yb + env->hgt - p->y_norm*(float)env->hgt));
                    }
                }
                else
                {
                    float px1 = p->x;
                    float py1 = p->y_norm;

                    float px2;
                    float py2;

                    if(p->next != NULL)
                    {
                        px2 = p->next->x;
                        py2 = p->next->y_norm;
                    }
                    else
                    {
                        px2 = p->x;
                        py2 = p->y_norm;
                    }

                    float py3 = Interpolate_Line(px1, py1, px2, py2, env->len);
                    path.lineTo((float)(x + env->edx), (float)(yb + env->hgt - py3*(float)env->hgt));
                    break;
                }
            }
            p = p->next;
        }
        path.lineTo((float)(x + env->edx), (float)(yb + env->hgt));
        path.closeSubPath();
        g.fillPath(path);
    }

    g.setColour(clr1.withAlpha(0.8f));
    J_HLine(g, yb + env->hgt, x, x + env->edx);

    J_VLine(g, x + env->edx - 1, yb, yb + env->hgt);
    J_VLine(g, x, yb, yb + env->hgt);

    //g.setColour(Colour(0x0fFFFFFF));
    //J_HLine(g, yb, x, x + env->edx);
    //J_HLine(g, yb + env->hgt - 1, x, x + env->edx);

    Path envpath;
    p = env->p_first;
    while(p != NULL)
    {
        if(p->x <= env->len)
        {
            if(p == env->susPoint)
            {
                g.setColour(Colour(0xffFFFF00));
                J_VLine(g, int(x + p->x*scale) + 1, yb + 1, yb + env->hgt);
                //J_FillRect(g, int(x + p->x*scale - 1) + 1, yb + 1, int(x + p->x*scale + 1) + 1, yb + 2);
                J_FillRect(g, int(x + p->x*scale - 2) + 1, yb + 1, int(x + p->x*scale + 2) + 1, yb + 1);

                if(M.mmode & MOUSE_ENVELOPING && M.active_env == env && M.env_action == ENVSUSTAINING)
                {
                    g.setColour(Colour(0x9fFFAF00));
                    J_VLine(g, int(x + p->x*scale), yb + 2, yb + env->hgt);
                    J_VLine(g, int(x + p->x*scale) + 2, yb + 2, yb + env->hgt);
                }
            }

//            g.setColour(Colour(0xffFF8F2F));
//            if(M.mmode & MOUSE_ENVELOPING && M.env_action == ENVPOINTING && M.active_pnt == p)
//            {
//                J_Circle(g, (int)(x + p->x*scale), (int)(yb + env->hgt -  p->y_norm*(float)env->hgt), 7);
//            }

            g.setColour(Colour(0xffFFFFFF));
            if(p != env->p_first)
            {
                J_Line(g, x + p->x*scale, yb + env->hgt - p->y_norm*(float)env->hgt, 
                         x + p->prev->x*scale, yb + env->hgt - p->prev->y_norm*(float)env->hgt);
            }
            //else
                //envpath.startNewSubPath(x + p->x*scale, yb + env->hgt - p->y_norm*(float)env->hgt);

/*
            J_VLine(g, xr, yr - 1, yr + 2);
            J_HLine(g, yr, xr - 1, xr + 2);
            J_FillRect(g, xr - 1, yr - 1, xr + 1, yr + 1);
            J_Round(g, (int)(x + p->x*scale), (int)(yb  + env->hgt - p->y_norm*(float)env->hgt), 3.5f);
*/

            int xr = RoundFloat(x + p->x*scale);
            int yr = RoundFloat(yb  + env->hgt - p->y_norm*(float)env->hgt);
            g.setColour(Colour(0xffFFFFFF));
            if(p->big)
            {
                J_VLine(g, xr, yr - 2, yr + 3);
                J_HLine(g, yr, xr - 2, xr + 3);
                J_HLine(g, yr - 1, xr - 1, xr + 2);
                J_HLine(g, yr + 1, xr - 1, xr + 2);
            }


            if(M.env_action == ENVPOINTING && M.active_pnt == p)
            {
                g.setColour(Colour(0xffFF6F2F));
                J_Line(g, xr - 3, yr, xr, yr - 3);
                J_Line(g, xr, yr - 3, xr + 3, yr);
                J_Line(g, xr + 3, yr, xr, yr + 3);
                J_Line(g, xr, yr + 3, xr - 3, yr);
                g.setPixel(xr + 3, yr);
                //J_Round(g, (int)(x + p->x*scale), (int)(yb + env->hgt -  p->y_norm*(float)env->hgt), 7);
            }

//            g.setColour(Colour(0xffFFFFFF));
//            J_Round(g, (int)(x + p->x*scale), (int)(yb  + env->hgt - p->y_norm*(float)env->hgt), 4);
            //int xr = (int)(x + p->x*scale);
            //int yr = (int)(yb  + env->hgt - p->y_norm*(float)env->hgt);
            //J_FillRect(g, xr - 1, yr - 1, xr + 3, yr + 3);
        }
        else // invisible point. draw last cut line and break
        {
            g.setColour(Colour(0xffFFFFFF));
            float px1 = p->prev->x;
            float px2 = p->x;

            float py1 = p->prev->y_norm;
            float py2 = p->y_norm;

            float py3 = Interpolate_Line(px1, py1, px2, py2, env->len);

            J_Line(g, (float)(x + px1*scale), (float)(yb + env->hgt - py1*(float)env->hgt),
                      (float)(x + env->edx), (float)(yb + env->hgt - py3*(float)env->hgt));
            break;
        }
        p = p->next;
    }
}

void J_HighlightSelectedElement(Graphics& g, Element* el)
{
    if(el->selected == true)
    {
        g.setColour(Colour(0x82FFA000));
        J_FillRect(g, el->abs_area.x1, el->abs_area.y1, el->abs_area.x2, el->abs_area.y2);
    }
}

void J_HighlightElement(Graphics& g, Element* el)
{
    if(el->selected == true)
    {
        g.setColour(Colour(0x42FFA000));
        J_FillRect(g, el->abs_area.x1, el->abs_area.y1, el->abs_area.x2, el->abs_area.y2);
    }
    else if(el->highlighted == true)
    {
        g.setColour(Colour(0x30FFA000));
        J_FillRect(g, el->abs_area.x1, el->abs_area.y1, el->abs_area.x2, el->abs_area.y2);
    }
}

void J_Muter(Graphics& g, Muter* mt, Loc loc)
{
    mt->GetRealCoordinates(loc);
    int x = mt->x;
    int y = mt->y;

    int length;
    if(loc == Loc_MainGrid)
    {
        length = (int)((mt->end_tick - mt->start_tick)*tickWidth);
    }
    else if(loc == Loc_SmallGrid)
    {
        length = (int)((mt->end_tick - mt->start_tick)*gAux->tickWidth);
    }

    g.setColour(Colour(0xff6F0000));
    J_FillRect(g, x, y - 7, x + length - 1, y - 1);
    g.setColour(Colour(0xffAF0000));
    J_LineRect(g, x, y - 7, x + length - 1, y - 1);
    mt->cx = x + length - 1;

    if(loc == Loc_MainGrid)
    {
        mt->SetAbsArea(mt->x, mt->y - 7, mt->cx, mt->y - 1);
    }
    else if(loc == Loc_SmallGrid)
    {
        mt->SetAbsArea(mt->x, mt->y - 7, mt->cx, mt->y - 1);
    }
}

void J_Break(Graphics& g, Break* b, Loc loc)
{
    b->GetRealCoordinates(loc);
    int x = b->x;
    int y = b->y;

    g.setColour(Colour(0xff5A5A5A));
    J_FillRect(g, x, y - 11, x + 5, y - 1);

    g.setColour(Colour(0xffFFFFFF));
    J_LineRect(g, x, y - 11, x + 5, y - 1);

    b->cx = x + 6;
    if(loc == Loc_MainGrid)
    {
        b->SetAbsArea(b->x, b->y - 11, b->cx - 1, b->y - 1);
    }
    else if(loc == Loc_SmallGrid)
    {
        b->SetAbsArea(b->x, b->y - 11, b->cx - 1, b->y - 1);
    }
}

void J_TrackSlide(Graphics& g, int x, int y, int length, bool dir)
{
    int ylow = 0; int yhigh = 5; int yd = yhigh - ylow;
    int xd0 = 2; int xd = xd0;

    float cf = (float)yd/xd;
    int end_x = x + length - 1;

    for(int xc = x; xc < end_x; xc += 3)
    {
        if((xc + xd) > end_x)
        {
            xd = end_x - xc;
            yd = (int)(xd*cf);

            if(dir == false)
            {
                ylow = yhigh - yd - 1;
            }
            else if(dir == true)
            {
                yhigh = ylow + yd + 1;
            }
        }

        g.setColour(Colour(0x8f00bFbF));
        if(dir == false)
        {
            J_Line(g, xc, y - yhigh - 1, xc + xd, y - ylow - 1);
        }
        else if(dir == true)
        {
            J_Line(g, xc, y - ylow - 1, xc + xd, y - yhigh - 1);
        }
    }
}

void J_Semitones(Graphics& g, int x, int y, Semitones* smt)
{
    g.setColour(Colour(0xffFFFFFF));
    J_TextSmall_xy(g, x - strlen(smt->str), y, smt->str);
}

void J_Slide(Graphics& g, Slide* sl, Loc loc)
{
    sl->GetRealCoordinates(loc);
    int x = sl->x;
    int y = sl->y;

    int length;
    if(loc == Loc_MainGrid)
    {
        length = (int)((sl->end_tick - sl->start_tick)*tickWidth);
    }
    else if(loc == Loc_SmallGrid)
    {
        length = (int)((sl->end_tick - sl->start_tick)*gAux->tickWidth);
    }

    g.setColour(Colour(0xff005454));
    J_FillRect(g, x, y - 7, x + length - 1, y - 1);
    g.setColour(Colour(0xff008F8F));
    J_LineRect(g, x, y - 7, x + length - 1, y - 1);

    J_TrackSlide(g, x, y, length, (sl->smt->value > 0));

    J_Semitones(g, x + length - 14, y + 3, sl->smt);

    sl->cx = x + length - 1;
    if(loc == Loc_MainGrid)
    {
        sl->SetAbsArea(sl->x, sl->y - 7, sl->cx, sl->y - 1);
    }
    else if(loc == Loc_SmallGrid)
    {
        sl->SetAbsArea(sl->x, sl->y - 7, sl->cx, sl->y - 1);
    }
}

void J_Transpose(Graphics& g, Transpose* tr, Loc loc)
{
    tr->GetRealCoordinates(loc);
    int x = tr->x;
    int y = tr->y;

    int length;
    if(loc == Loc_MainGrid)
    {
        length = (int)((tr->end_tick - tr->start_tick)*tickWidth);
    }
    else if(loc == Loc_SmallGrid)
    {
        length = (int)((tr->end_tick - tr->start_tick)*gAux->tickWidth);
    }

    g.setColour(Colour(0xff00009A));
    J_FillRect(g, x, y - 7, x + length - 1, y - 1);
    g.setColour(Colour(0xff0000FF));
    J_LineRect(g, x, y - 7, x + length - 1, y - 1);

    J_Semitones(g, x + length - 14, y + 3, tr->smt);
    tr->cx = x + length - 1;

    if(loc == Loc_MainGrid)
    {
        tr->SetAbsArea(tr->x, tr->y - 7, tr->cx, tr->y - 1);
    }
    else if(loc == Loc_SmallGrid)
    {
        tr->SetAbsArea(tr->x, tr->y - 7, tr->cx, tr->y - 1);
    }
}

void J_Reverse(Graphics& g, Reverse* r, Loc loc)
{
    r->GetRealCoordinates(loc);
    int x = r->x;
    int y = r->y;

    int xc = CursX, xc0 = CursX0, yc = CursY;
    CursX = x;
    CursX0 = x;
    CursY = y - 1;

    if(r->value == true)
    {
        g.setColour(Colour(0xff64C8FF));
        J_TextInstr_type(g, "<");
    }

    r->SetAbsArea(CursX0 - 1, CursY - 8, CursX, CursY);

    r->cx = CursX;

    CursX0 = xc0;
    CursX = xc;
    CursY = yc;
}

void J_Repeat(Graphics& g, Repeat* r, Loc loc)
{
    r->GetRealCoordinates(loc);
    int x = r->x;
    int y = r->y;

	int xc = CursX, xc0 = CursX0, yc = CursY;
	CursX = x - 2;
    CursX0 = x;
	CursY = y - 1;

    if(r->value == true)
    {
        //glColor3ub(100, 200, 255);
        g.setColour(Colour(0xff64C8FF));
	    J_TextInstr_type(g, ">");
    }

    r->SetAbsArea(CursX0 - 1, CursY - 7, CursX, CursY);
    r->cx = CursX;

    CursX0 = xc0;
    CursX = xc;
    CursY = yc;
}

void J_Line_cut(Graphics& g, int x1, int y1, int x2, int y2, int x_cut)
{
    if(x_cut > x2)
    {
        J_Line(g, x1, y1, x2, y2);
    }
    else if(x_cut < x1)
    {
        return;
    }
    else
    {
        float y_cut = Interpolate_Line((double)x1, (float)y1, (double)x2, (float)y2, (double)x_cut);
        J_Line(g, x1, y1, x_cut, (int)y_cut);
    }
}

void J_Vibrate(Graphics& g, Vibrate* v, Loc loc)
{
    v->GetRealCoordinates(loc);
    int x = v->x;
    int y = v->y;

    int length;
    if(loc == Loc_MainGrid)
    {
        length = (int)((v->end_tick - v->start_tick)*tickWidth);
    }
    else if(loc == Loc_SmallGrid)
    {
        length = (int)((v->end_tick - v->start_tick)*gAux->tickWidth);
    }

    g.setColour(Colour(0xff105410));
    J_FillRect(g, x, y - 7, x + length - 1, y - 1);
    g.setColour(Colour(0xff2F9F2F));
    J_LineRect(g, x, y - 7, x + length - 1, y - 1);

    v->cx = x + length - 1;

    if(loc == Loc_MainGrid)
    {
        v->SetAbsArea(v->x, v->y - 7, v->cx, v->y - 1);
    }
    else if(loc == Loc_SmallGrid)
    {
        v->SetAbsArea(v->x, v->y - 7, v->cx, v->y - 1);
    }

    int pe;
    if(loc == Loc_MainGrid)
    {
        pe = v->x + (int)(v->tick_length * 8);;
    }
    else if(loc == Loc_SmallGrid)
    {
        pe = v->x + (int)(v->tick_length * 8);
    }

    int npix = int(v->tick_length * (loc == Loc_MainGrid ? tickWidth : gAux->tickWidth));
    int kx = v->x; 
    int ky = v->y - 4;
    int pp = 2;

    g.saveState();
    g.reduceClipRegion(x, y - 7, length - 1, 6);
    g.setColour(Colour(0xff000000));
    while(npix > 0)
    {
        J_Line(g, kx, ky, kx + pp, ky - 2);
        J_Line(g, kx + pp, ky - 2, kx + 2*pp, ky);

        J_Line(g, kx + 2*pp, ky, kx + 3*pp, ky + 2);
        J_Line(g, kx + 3*pp, ky + 2, kx + 4*pp, ky);

        if(loc == Loc_MainGrid)
        {
            kx += 8;
        }
        else if(loc == Loc_SmallGrid)
        {
            kx += 8;
        }
        npix -= 8;
    }
    g.restoreState();

    v->cx = x + length - 1;
}

void J_SlideNoteLen(Graphics& g, int x_pix, int y, int length, Loc loc, bool connected)
{
    int tH = (loc == Loc_MainGrid || loc == Loc_StaticGrid) ? lineHeight : gAux->lineHeight;
    if(tH > 12)
    {
        tH = 12;
    }
    int noteheight = tH - 4;

    if(connected)
        g.setColour(Colour(0x7B42A6A6));
    else
        g.setColour(Colour(0x7BA6A642));
    J_FillRect(g, x_pix, y - tH + 2, x_pix + length - 1, y - tH + noteheight + 2);

    if(connected)
        g.setColour(Colour(0x9B2E2E74));
    else
        g.setColour(Colour(0x9BA6A642));
    J_LineRect(g, x_pix, y - tH + 2, x_pix + length - 1, y - tH + noteheight + 2);
}

void J_SmpNoteLen(Graphics& g, int x, int y, int pixlen1, int pixlen2, Loc loc)
{
    int tH = (loc == Loc_MainGrid || loc == Loc_StaticGrid) ? lineHeight : gAux->lineHeight;
    if(tH > 12)
    {
        tH = 12;
    }
    int noteheight = tH - 4;

	int incr2 = pixlen2 > 0 ? 1 : 0;
    //g.setColour(Colour(33, 73, 65));
    g.setColour(Colour(65, 65, 65));
    J_FillRect(g, x, y - tH + 2, x + pixlen2 - incr2, y - tH + noteheight + 2);

    int incr1 = pixlen1 > 0 ? 1 : 0;
    //g.setColour(Colour(60, 105, 100));
    g.setColour(Colour(90, 90, 90));
    J_LineRect(g, x, y - tH + 2, x + pixlen1 - incr1, y - tH + noteheight + 2);

    //g.setColour(Colour(0x4f6ACAB0));
    g.setColour(Colour(0x4fB0B0B0));
    J_VLine(g, x, y - tH + 2, y - tH + noteheight + 3);
    //g.setColour(Colour(0x3f6ACAB0));
    g.setColour(Colour(0x3fB0B0B0));
    J_VLine(g, x + pixlen1 - incr1, y - tH + 2, y - tH + noteheight + 3);
}

void J_GenNoteLen(Graphics& g, int x, int y, int pixlen, Loc loc)
{
    int tH = (loc == Loc_MainGrid || loc == Loc_StaticGrid) ? lineHeight : gAux->lineHeight;
    if(tH > 12)
    {
        tH = 12;
    }
    int noteheight = tH - 4;

	int incr = pixlen > 0 ? 1 : 0;
    g.setColour(Colour(0xff000000));
    J_FillRect(g, x, y - tH + 2, x + pixlen - incr, y - tH + noteheight + 2);

    g.setColour(Colour(0xff253A5F));
    J_FillRect(g, x, y - tH + 2, x + pixlen - incr, y - tH + noteheight + 2);

    g.setColour(Colour(0xff3A608A));
    J_LineRect(g, x, y - tH + 2, x + pixlen - incr, y - tH + noteheight + 2);

    g.setColour(Colour(0x4f6AB0CA));
    J_VLine(g, x, y - tH + 2, y - tH + noteheight + 3);
    g.setColour(Colour(0x3f6AB0CA));
    J_VLine(g, x + pixlen - incr, y - tH + 2, y - tH + noteheight + 3);
	//if(pixlen > 1)
	//{
    //    J_VLine(g, x + 1, y - tH + 2, y - tH + noteheight + 2);
	//}
}

void J_PattLen(Graphics& g, int x, int y, int pixlen, Loc loc, PattType ptype, bool muted, bool fat)
{
    int tH;
    int ptheight;
    if(fat)
    {
        tH = lineHeight*2;
        ptheight = lineHeight*2 - 2;
    }
    else
    {
        tH = lineHeight;
        ptheight = lineHeight - 2;
    }

    //g.setColour(Colour(0xff000000));
    //J_FillRect(g, x, y - tH + 1, x + pixlen - 1, y - tH + ptheight + 1);

/*
    if(ptype == Patt_StepSeq)
    {
        int gappy = 0;
        if(fat)
        {
            g.setColour(Colour(0x4f2D415A));
            J_FillRect(g, x, y - tH + 1, x + pixlen - 1, y - tH + ptheight + 1);
            g.setColour(Colour(0xff4A608A));
            //J_LineRect(g, x, y - tH + 1, x + pixlen - 1, y - tH + ptheight + 1);
            J_VLine(g, x, y - tH + 1, y - tH + ptheight + 1);
            J_HLine(g, y - tH + 1, x, x + pixlen - gappy);
            J_HLine(g, y - tH + ptheight + 1, x, x + pixlen - gappy);

            g.setColour(Colour(0x7f446888));
            J_HLine(g, y - tH + 2, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x6f446888));
            J_HLine(g, y - tH + 3, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x5f446888));
            J_HLine(g, y - tH + 4, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x5a446888));
            J_HLine(g, y - tH + 5, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x4f446888));
            J_HLine(g, y - tH + 6, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x4a446888));
            J_HLine(g, y - tH + 7, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x45446888));
            J_HLine(g, y - tH + 8, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x40446888));
            J_HLine(g, y - tH + 9, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x3a446888));
            J_HLine(g, y - tH + 10, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x35446888));
            J_HLine(g, y - tH + 11, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x30446888));
            J_HLine(g, y - tH + 12, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x2a446888));
            J_HLine(g, y - tH + 13, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x25446888));
            J_HLine(g, y - tH + 14, x + 1, x + pixlen - gappy);
        }
        else
        {
            g.setColour(Colour(0xdf2D415A));
            J_FillRect(g, x, y - tH + 1, x + pixlen - 1, y - tH + ptheight + 1);
            g.setColour(Colour(0xff4A608A));
            //J_LineRect(g, x, y - tH + 1, x + pixlen - 1, y - tH + ptheight + 1);
            J_VLine(g, x, y - tH + 1, y - tH + ptheight + 1);
            J_HLine(g, y - tH + 1, x, x + pixlen - gappy);
            J_HLine(g, y - tH + ptheight + 1, x, x + pixlen - gappy);

            g.setColour(Colour(0x6f446888));
            J_HLine(g, y - tH + 2, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x5f446888));
            J_HLine(g, y - tH + 3, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x4f446888));
            J_HLine(g, y - tH + 4, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x3f446888));
            J_HLine(g, y - tH + 5, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x2f446888));
            J_HLine(g, y - tH + 6, x + 1, x + pixlen - gappy);
        }
    }
    else if(ptype == Patt_Pianoroll)
    {
        int gappy = 0;
        if(fat)
        {
            g.setColour(Colour(0x4f4A4A4A));
            J_FillRect(g, x, y - tH + 1, x + pixlen - 1, y - tH + ptheight + 1);

            g.setColour(Colour(0xff686868));
            //J_LineRect(g, x, y - tH + 1, x + pixlen - 1, y - tH + ptheight + 1);
            J_VLine(g, x, y - tH + 1, y - tH + ptheight + 1);
            J_HLine(g, y - tH + 1, x, x + pixlen - gappy);
            J_HLine(g, y - tH + ptheight + 1, x, x + pixlen - gappy);

            g.setColour(Colour(0x6f787878));
            J_HLine(g, y - tH + 2, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x5f787878));
            J_HLine(g, y - tH + 3, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x4f787878));
            J_HLine(g, y - tH + 4, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x4a787878));
            J_HLine(g, y - tH + 5, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x45787878));
            J_HLine(g, y - tH + 6, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x3f787878));
            J_HLine(g, y - tH + 7, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x3a787878));
            J_HLine(g, y - tH + 8, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x35787878));
            J_HLine(g, y - tH + 9, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x30787878));
            J_HLine(g, y - tH + 10, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x2f787878));
            J_HLine(g, y - tH + 11, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x2a787878));
            J_HLine(g, y - tH + 12, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x25787878));
            J_HLine(g, y - tH + 13, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x1a787878));
            J_HLine(g, y - tH + 14, x + 1, x + pixlen - gappy);
        }
        else
        {
            g.setColour(Colour(0xdf4A4A4A));
            J_FillRect(g, x, y - tH + 1, x + pixlen - 1, y - tH + ptheight + 1);

            g.setColour(Colour(0xff686868));
            //J_LineRect(g, x, y - tH + 1, x + pixlen - 1, y - tH + ptheight + 1);
            J_VLine(g, x, y - tH + 1, y - tH + ptheight + 1);
            J_HLine(g, y - tH + 1, x, x + pixlen - gappy);
            J_HLine(g, y - tH + ptheight + 1, x, x + pixlen - gappy);

            g.setColour(Colour(0x6f787878));
            J_HLine(g, y - tH + 2, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x5f787878));
            J_HLine(g, y - tH + 3, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x4f787878));
            J_HLine(g, y - tH + 4, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x3f787878));
            J_HLine(g, y - tH + 5, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x2f787878));
            J_HLine(g, y - tH + 6, x + 1, x + pixlen - gappy);
        }
    }
    else if(ptype == Patt_Grid)
    {
        int gappy = 0;
        if(fat)
        {
            g.setColour(Colour(0x4f3A5A3A));
            J_FillRect(g, x, y - tH + 1, x + pixlen - 1, y - tH + ptheight + 1);

            g.setColour(Colour(0xff5A855A));
            //J_LineRect(g, x, y - tH + 1, x + pixlen - 1, y - tH + ptheight + 1);
            J_VLine(g, x, y - tH + 1, y - tH + ptheight + 1);
            J_HLine(g, y - tH + 1, x, x + pixlen - gappy);
            J_HLine(g, y - tH + ptheight + 1, x, x + pixlen - gappy);

            g.setColour(Colour(0x6f5A855A));
            J_HLine(g, y - tH + 2, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x5f5A855A));
            J_HLine(g, y - tH + 3, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x4f5A855A));
            J_HLine(g, y - tH + 4, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x4a5A855A));
            J_HLine(g, y - tH + 5, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x455A855A));
            J_HLine(g, y - tH + 6, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x3f5A855A));
            J_HLine(g, y - tH + 7, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x3a5A855A));
            J_HLine(g, y - tH + 8, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x355A855A));
            J_HLine(g, y - tH + 9, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x305A855A));
            J_HLine(g, y - tH + 10, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x2f5A855A));
            J_HLine(g, y - tH + 11, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x2a5A855A));
            J_HLine(g, y - tH + 12, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x255A855A));
            J_HLine(g, y - tH + 13, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x1a5A855A));
            J_HLine(g, y - tH + 14, x + 1, x + pixlen - gappy);
        }
        else
        {
            g.setColour(Colour(0xdf3A5A3A));
            J_FillRect(g, x, y - tH + 1, x + pixlen - 1, y - tH + ptheight + 1);

            g.setColour(Colour(0xff5A855A));
            //J_LineRect(g, x, y - tH + 1, x + pixlen - 1, y - tH + ptheight + 1);
            J_VLine(g, x, y - tH + 1, y - tH + ptheight + 1);
            J_HLine(g, y - tH + 1, x, x + pixlen - gappy);
            J_HLine(g, y - tH + ptheight + 1, x, x + pixlen - gappy);

            g.setColour(Colour(0x6f5A855A));
            J_HLine(g, y - tH + 2, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x5f5A855A));
            J_HLine(g, y - tH + 3, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x4f5A855A));
            J_HLine(g, y - tH + 4, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x3f5A855A));
            J_HLine(g, y - tH + 5, x + 1, x + pixlen - gappy);
            g.setColour(Colour(0x2f5A855A));
            J_HLine(g, y - tH + 6, x + 1, x + pixlen - gappy);
        }
    }
*/

    if(muted)
    {
        g.setColour(Colour(0x4f000000));
        J_FillRect(g, x, y - tH + 1, x + pixlen - 1, y - tH + ptheight + 1);
    }
}

void J_SlideNote(Graphics& g, SlideNote* sl, Loc loc, bool connected)
{
    int tH = (loc == Loc_MainGrid || loc == Loc_StaticGrid) ? lineHeight : gAux->lineHeight;

    sl->GetRealCoordinates(loc);

    int x = sl->x;
    int y = sl->y;

    unsigned int length;
    if(loc == Loc_MainGrid)
    {
        length = (int)((sl->end_tick - sl->start_tick)*tickWidth);
    }
    else if(loc == Loc_SmallGrid)
    {
        length = (int)((sl->end_tick - sl->start_tick)*gAux->tickWidth);
    }
    sl->cx = x + length;

    sl->SetAbsArea(x, y - tH + 1, sl->cx, y - 1);

    J_SlideNoteLen(g, x, y, length, loc, connected);
    if(sl->ed_note->visible)
    {
        J_Note_xy(g, x + 1, y, sl->ed_note);
    }
}

int J_Layout(Graphics& g, Instance* ii, Loc loc)
{
    int cx, scx;
    int cy, scy;

    cx = ii->x;
    cy = ii->y;

    scy = cy;
    scx = cx;
    int cx1, cx0, mx, cy1;
    int initial_end_x;
    float tW = (loc == Loc_MainGrid) ? tickWidth : (loc == Loc_StaticGrid) ? st_tickWidth : gAux->tickWidth;
    int tH = (loc == Loc_MainGrid || loc == Loc_StaticGrid) ? lineHeight : gAux->lineHeight;

/*
    Samplent* s = NULL;
    if(ii->type == El_Samplent)
    {
        s = (Samplent*)ii;
        initial_end_x = ApproxFloat(cx + Calc_PixLength(s->sample_frame_len, (int)(s->sample->info.samplerate), tW)*s->freq_ratio);
        //initial_end_x1 = ApproxFloat(cx + Calc_PixLength(s->sample_frame_len, (int)(s->sample->info.samplerate), tW)*s->freq_ratio*s->size);
    }
    else if(ii->type == El_Gennote) */
    {
        initial_end_x = cx + (int)(ii->tick_length*tW);
    }
        
    if(ii->instr->layout_visible == true)
    {
        SlideNote* slnote = ii->first_slide;
        if(slnote == NULL)
        {
            if(ii->type == El_Samplent)
            {
                J_SmpNoteLen(g, cx, cy, initial_end_x - cx, initial_end_x - cx, loc);
            }
            else if(ii->type == El_Gennote)
            {
                J_GenNoteLen(g, cx, cy, initial_end_x - cx, loc);
            }

            return initial_end_x - cx;
        }
        else
        {
            while(slnote != NULL)
            {
                if(slnote->IsPresent())
                {
                    if(loc == Loc_MainGrid)
                    {
                        cx1 = slnote->EndTickX();
                    }
                    else if(loc == Loc_SmallGrid)
                    {
                        cx1 = slnote->s_EndTickX();
                    }
                    //J_NoteLen(cx, cy, (cx1 - cx) - 1, main);

                    if(loc == Loc_MainGrid)
                    {
                        cx0 = slnote->StartTickX();
                        cy1 = slnote->TrackLine2Y();
                    }
                    else
                    {
                        cx0 = slnote->s_StartTickX();
                        cy1 = slnote->s_TrackLine2Y();
                    }

                    mx = abs(cx1 - cx0)/2;
                    g.setColour(Colour(0x5000A0A0));
                    if(cy1 < scy)
                    {
                        J_FillRect(g, cx0, cy1 - tH + SlideHeight + 2, cx1 - 1, scy - tH + 1);
                    }
                    else if(cy1 > scy)
                    {
                        J_FillRect(g, cx0, scy - tH + SmpHeight + 2, cx1 - 1, cy1 - tH + 1);
                    }
                    cx = cx1;
                    cy = cy1;

                    if(cx1 - 1 > initial_end_x)
                    {
                        initial_end_x = cx1 - 1;
                    }
                    J_SlideNote(g, slnote, loc, true);
                    J_HighlightElement(g, slnote);
                }

                slnote = slnote->s_next;
            }

			if(ii->type == El_Samplent)
            {
                J_SmpNoteLen(g, scx, scy, initial_end_x - scx, initial_end_x - scx, loc);
            }
            else if(ii->type == El_Gennote)
            {
                J_GenNoteLen(g, scx, scy, initial_end_x - scx, loc);
            }

            return initial_end_x - scx;
        }
    }
    else
    {
        return 0;
    }
}

int J_CommandNameWidth(Command* c)
{
    switch(c->cmdtype)
    {
        case Cmd_Mute:
        {
            if(c->reset_command == false)
            {
                return J_TextInstrWidth("mute");
            }
            else
            {
                return J_TextInstrWidth("unmute");
            }
        }break;
        case Cmd_Solo:
        {
            if(c->reset_command == false)
            {
                return J_TextInstrWidth("solo");
            }
            else
            {
                return J_TextInstrWidth("unsolo");
            }
        }break;
        case Cmd_Bypass:
        {
            if(c->reset_command == false)
            {
                return J_TextInstrWidth("bypass");
            }
            else
            {
                return J_TextInstrWidth("unbypass");
            }
        }break;
        case Cmd_VolEnv:
        {
            return J_TextInstrWidth("vol.env");
        }break;
        case Cmd_PanEnv:
        {
            return J_TextInstrWidth("pan.env");
        }break;
        case Cmd_Vol:
        {
            return J_TextInstrWidth("vol");
        }break;
        case Cmd_Pan:
        {
            return J_TextInstrWidth("pan");
        }break;
        case Cmd_Len:
        {
            return J_TextInstrWidth("len");
        }break;
        case Cmd_Offs:
        {
            return J_TextInstrWidth("offs");
        }break;
        case Cmd_ParamEnv:
        {
            return J_TextInstrWidth(c->param->name);
        }
        default:
        {
            return J_TextInstrWidth("default");
        }break;
    }
}

void J_CommandName(Graphics& g, Command* c)
{
    g.setColour(Colour(0xffDCDCDC));
    switch(c->cmdtype)
    {
        case Cmd_Mute:
        {
            if(c->reset_command == false)
            {
                J_TextSmall_type(g, "mute");
            }
            else
            {
                J_TextSmall_type(g, "unmute");
            }
        }break;
        case Cmd_Solo:
        {
            if(c->reset_command == false)
            {
                J_TextSmall_type(g, "solo");
            }
            else
            {
                J_TextSmall_type(g, "unsolo");
            }
        }break;
        case Cmd_Bypass:
        {
            if(c->reset_command == false)
            {
                J_TextSmall_type(g, "bypass");
            }
            else
            {
                J_TextSmall_type(g, "unbypass");
            }
        }break;
        case Cmd_VolEnv:
        {
            J_TextSmall_type(g, "vol");
        }break;
        case Cmd_PanEnv:
        {
            J_TextSmall_type(g, "pan");
        }break;
        case Cmd_LocVolEnv:
        {
            J_TextSmall_type(g, "vol.loc");
        }break;
        case Cmd_LocPanEnv:
        {
            J_TextSmall_type(g, "pan.loc");
        }break;
        case Cmd_Vol:
        {
            J_TextSmall_type(g, "vol");
        }break;
        case Cmd_Pan:
        {
            J_TextSmall_type(g, "pan");
        }break;
        case Cmd_Len:
        {
            J_TextSmall_type(g, "len");
        }break;
        case Cmd_Offs:
        {
            J_TextSmall_type(g, "offs");
        }break;
        case Cmd_ParamEnv:
        {
			if(c->param != NULL)
			{
                if(!(c->param->module != NULL && c->param->module->to_be_deleted == true))
                {
    				J_TextSmall_type(g, c->param->name);
                }
			}
        }break;
        default:
        {
            J_TextSmall_type(g, "default");
        }break;
    }
}

void J_CommandSize(Command* c, Loc loc, int* w, int* h)
{
   *w = 0;
   *h = (int)ins->getHeight();

   *w += J_ScopeWidth(c->scope);
   *w += J_CommandNameWidth(c);

    if(c->reset_command == false)
    {
        if(c->paramedit != NULL)
        {
            if((c->cmdtype == Cmd_VolEnv)||
               (c->cmdtype == Cmd_PanEnv)||
               (c->cmdtype == Cmd_ParamEnv))
            {
                Envelope* env = (Envelope*)c->paramedit;
                int he;
                int we;
                J_EnvSize(&we, &he, env, loc);
               *w += we;
               *h += he + 3;
            }
            else if(c->cmdtype == Cmd_Vol || 
                    c->cmdtype == Cmd_Pan || 
                    c->cmdtype == Cmd_Len || 
                    c->cmdtype == Cmd_Offs)
            {
               *w += J_TextInstrWidth("(");
               *w += J_TextInstrWidth(((Percent*)c->paramedit)->digits);
               *w += J_TextInstrWidth(")");
            }
        }
    }
    else
    {
       *w += J_TextInstrWidth(".");
       *w += J_TextInstrWidth("Reset");
    }
}

void J_Command_xy(Graphics& g, int x, int y, Command* c, Loc loc)
{
    int cx = CursX;
    int cy = CursY;
    CursX = x;
    CursY = y;
    c->x = x;
    c->y = y;

    c->cx = CursX;
    if(c->reset_command == false)
    {
        if(c->paramedit != NULL)
        {
            if((c->cmdtype == Cmd_VolEnv)||
               (c->cmdtype == Cmd_PanEnv)||
               (c->cmdtype == Cmd_ParamEnv))
            {
                Envelope* env = (Envelope*)c->paramedit;
                J_EnvLen(g, x, CursY, env, c->cmdtype, loc);
                J_Env(g, x, CursY + 1, env, c->cmdtype, loc);
            }
            else if(c->cmdtype == Cmd_Vol || 
                    c->cmdtype == Cmd_Pan || 
                    c->cmdtype == Cmd_Len || 
                    c->cmdtype == Cmd_Offs)
            {
                J_TextSmall_type(g, "(");
                J_Percent_Instr_type(g, (Percent*)c->paramedit);
                J_TextSmall_type(g, ")");
            }
        }
    }
    else
    {
        J_TextSmall_type(g, ".");
        J_TextSmall_type(g, "Reset");
    }

    J_Scope_type(g, c->scope);
    J_CommandName(g, c);

    CursX = cx;
    CursY = cy;
}

void J_Command(Graphics& g, Command* c, Loc loc)
{
    c->GetRealCoordinates(loc);
    int x = c->x;
    int y = c->y;

    int cx = CursX;
    int cy = CursY;
    int cx0 = CursX0;

    CursX = x;
    CursY = y;
    CursX0 = CursX;

    // Pre-name stuff
    if(c->reset_command == false && c->paramedit != NULL)
    {
        if(c->IsEnvelope())
        {
            Envelope* env = (Envelope*)c->paramedit;
            J_EnvLen(g, x, CursY, env, c->cmdtype, loc);
            if(env->folded == false)
            {
                J_Env(g, x, CursY + 1, env, c->cmdtype, loc);
            }
            c->SetAbsArea(x, CursY - lineHeight + 5, x + env->edx - 1, CursY);
        }
    }

	if(c->scope != NULL)
	{
		J_Scope_type(g, c->scope);
	}
    J_CommandName(g, c);

    // Post-name stuff
    if(c->reset_command == false && c->paramedit != NULL)
    {
        if(c->IsEnvelope())
        {
            Envelope* env = (Envelope*)c->paramedit;
            J_EnvFold(g, CursX + 3, CursY - 6, env->fold);
            if(env->timetg != NULL)
            {
                J_TimeFold(g, CursX + 14, CursY - 1, env->timetg);
            }
        }
        else if(c->cmdtype == Cmd_Vol ||
           c->cmdtype == Cmd_Pan ||
           c->cmdtype == Cmd_Len ||
           c->cmdtype == Cmd_Offs)
        {
            J_TextInstr_type(g, "(");
            J_Percent_Instr_type(g, (Percent*)c->paramedit);
            J_TextInstr_type(g, ")");
        }
    }
    c->cx = CursX;

    if(!c->IsEnvelope())
    {
        c->SetAbsArea(CursX0, CursY - (lineHeight - 1), c->cx, CursY - 1);
    }

    CursX0 = cx0;
    CursX = cx;
    CursY = cy;
}

void J_Wave(Graphics& g, Samplent* s, Loc loc)
{
    float tW = (loc == Loc_MainGrid) ? tickWidth : (loc == Loc_StaticGrid) ? st_tickWidth : gAux->tickWidth;

    int hh = s->wave_height_pix/2;
    g.setColour(Colour(0x645050C8));
    J_FillRect(g, CursX, s->y - 10, CursX + s->pix_length + 1, s->y - 10 - s->wave_height_pix);

    long sc = 0;
    long fc = 0;
    int xw = CursX + 1;
    int xw1 = CursX + 1;
    int yw, yw1;
    float dt, dt1;
    float hhf = (float)hh;

    int frm_incr = 60;
    int data_incr = s->sample->info.channels*frm_incr;
    float incr = s->pix_length/(float)s->sample->info.frames/data_incr;

    Path wave;
    bool started = false;
    while(fc < s->sample->info.frames)
    {
        dt = s->sample->sample_data[(int)sc];
        yw = (int)(hhf*dt);
        xw = s->x + (int)(sc*incr*frm_incr);
        sc += data_incr;
        fc += frm_incr;
        if(started == false)
        {
            started = true;
            wave.startNewSubPath((float)xw, (float)(s->y - 10 - hh + yw));
        }
        else
        {
            wave.lineTo((float)xw, (float)(s->y - 10 - hh + yw));
        }

        dt1 = s->sample->sample_data[(int)sc];
        yw1 = (int)(hhf*dt1);
        xw1 = s->x + (int)(sc*incr*frm_incr);
        sc += data_incr;
        fc += frm_incr;
        wave.lineTo ((float)xw1, (float)(s->y - 10 - hh + yw1));
    }
    wave.closeSubPath();
    g.setColour(Colours::white);
    g.strokePath (wave, PathStrokeType(1.0f));
}

void J_Txt(Graphics& g, Txt* t, Loc loc)
{
    t->GetRealCoordinates(loc);
    int bx = t->x;
    int by = t->y;

    int cx = CursX;
    int cy = CursY;
    int cx0 = CursX0;
    CursX = bx;
    CursY = by;
    CursX0 = CursX;

    if(t->bookmark == false)
    {
        g.setColour(Colour(225, 225, 225));
        J_TextBase_type(g, ((Txt*)t)->buff);
    }
    else
    {
        //g.setColour(Colour(0x7f8F9FFF));
        //J_Line(g, bx, GridY1, bx, GridY2);
        g.setColour(Colour(0xff8F9FFF));
        J_TextInstr_type(g, ((Txt*)t)->buff);
    }

    if(loc == Loc_MainGrid)
        t->SetAbsArea(CursX0, CursY - (lineHeight - 1), CursX, CursY - 1);
    else if(loc == Loc_SmallGrid)
        t->SetAbsArea(CursX0, CursY - (gAux->lineHeight - 1), CursX, CursY - 1);
    t->end_tick = X2Tick(CursX, loc);
    t->tick_length = t->end_tick - t->start_tick;

    t->cx = CursX;
    CursX = cx;
    CursY = cy;
    CursX0 = cx0;
}

void J_StepSeqEl(Graphics& g, Instance* ii)
{
    int x = ii->s_StartTickX();
    int y = ii->s_TrackLine2Y();

    /*
    if(ii->type == El_Samplent && ((Samplent*)ii)->revB == true)
    {
        g.setColour(Colour((0xff505F7D)));
        J_LineTri(g, x + 11, y - 1, x, y - 7, x + 7, y - 14);
    }
    else*/
    {
        g.setColour(Colour((0xffFFFFFF)));
        g.drawImageAt(img_stepq, x, y - 14, false);

        if(ii == C.curElem)
        {
            g.setColour(Colour((0xff14FF00)));
            g.drawVerticalLine(x, float(y - 14), float(y));
            g.drawHorizontalLine(y - 14, float(x), float(x + 16));
            g.drawHorizontalLine(y - 1, float(x), float(x + 16));
            //g.drawRect(x, y - 14, 16, 14);
            //g.drawRect(x + 1, y - 13, 14, 12);
        }
    }

    ii->x = x;
    ii->y = y;
    ii->cx = x + 8;
    ii->SetAbsArea(x, y - 14, x + 15, y - 1);
}

void J_Samplent(Graphics& g, Samplent* s, Loc loc)
{
    s->GetRealCoordinates(loc);

    int x = s->x;
    int y = s->y;

    int xc = CursX, yc = CursY;
    CursX = x;
    CursY = y;

    float tW = (loc == Loc_MainGrid) ? tickWidth : (loc == Loc_StaticGrid) ? st_tickWidth : gAux->tickWidth;
    s->pix_length = (int)(Calc_PixLength((long)(s->sample->info.frames), s->sample->info.samplerate, tW)*s->freq_ratio);

    if(s->wave_visible == true)
    {
        J_Wave(g, s, loc);
    }
    int pixlen = J_Layout(g, s, loc);

    g.setColour(Colour(0xffFFFFFF));
    if(s->patt->ptype != Patt_Pianoroll && s->patt->ibound == NULL)
    {
        //g.setColour(Colour(0xff6496FF));
        //J_TextInstr_type(g, s->sample->alias->string);
        if(s->sample->alimg != NULL)
        {
            g.drawImageAt(s->sample->alimg, CursX, CursY - (int)ins->getHeight(), false);
            CursX += ins->getStringWidth(s->sample->alias->string);
        }
    }

    if(s->revB == true)
    {
        g.setColour(Colour(0xffFFFFFF));
        J_TextInstr_xy(g, x + 1, y - 7, "<");
    }
    s->cx = CursX;

    if(s->ed_note->visible == true)
    {
        g.setColour(Colour(0xff6496FF));
        CursX++;
        CursY += 2;
        J_Note_type(g, s->ed_note);
        CursY -= 2;
    }

    g.setColour(Colour(0xffDCDCDC));
    if(s->ed_vol->visible == true)
    {
        J_TextInstr_type(g, ":vol(");
        J_Percent_Instr_type(g, s->ed_vol);
        J_TextInstr_type(g, ")");
    }
    if(s->ed_pan->visible == true)
    {
        J_TextInstr_type(g, ":pan(");
        J_Percent_Instr_type(g, s->ed_pan);
        J_TextInstr_type(g, ")");
    }
    if(s->ed_len->visible == true)
    {
        J_TextInstr_type(g, ":len(");
        J_Percent_Instr_type(g, s->ed_len);
        J_TextInstr_type(g, ")");
    }
    if(s->ed_offs->visible == true)
    {
        J_TextInstr_type(g, ":offs(");
        J_Percent_Instr_type(g, s->ed_offs);
        J_TextInstr_type(g, ")");
    }

    int cx = x + pixlen;
    if(CursX > cx)
    {
        cx = CursX;
    }

    if(loc == Loc_MainGrid || loc == Loc_StaticGrid)
    {
        s->SetAbsArea(x, CursY - (lineHeight - 1), cx, CursY - 1);
    }
    else if(loc == Loc_SmallGrid)
    {
        if(gAux->workPt->ptype == Patt_Pianoroll)
        {
            s->SetAbsArea(x, CursY - PianorollLineHeight + 1, cx, CursY - 1);
        }
        else
        {
            s->SetAbsArea(x, CursY - (gAux->lineHeight - 1), cx, CursY - 1);
        }
    }

    CursX = xc;
    CursY = yc;
}

void J_Gennote(Graphics& g, Gennote* gn, Loc loc)
{
    gn->GetRealCoordinates(loc);

    int x = gn->x;
    int y = gn->y;

    int xc = CursX, yc = CursY;
    CursX = x;
    CursY = y;

    int pixlen = J_Layout(g, gn, loc);

    if(gn->patt->ptype != Patt_Pianoroll && gn->patt->ibound == NULL)
    {
        g.setColour(Colour(0xffFFFFFF));
        //J_TextInstr_type(g, gn->instr->alias->string);
        if(gn->instr->alimg != NULL)
        {
            g.drawImageAt(gn->instr->alimg, CursX, CursY - (int)ins->getHeight(), false);
            CursX += ins->getStringWidth(gn->instr->alias->string);
        }
    }

    gn->cx = CursX;

    if(gn->ed_note->visible == true)
    {
        g.setColour(Colour(0xff6496FF));
        CursX++;
        CursY += 2;
        J_Note_type(g, gn->ed_note);
        CursY -= 2;
    }

    g.setColour(Colour(0xffDCDCDC));
    if(gn->ed_vol->visible == true)
    {
        J_TextInstr_type(g, ":vol(");
        J_Percent_Instr_type(g, gn->ed_vol);
        J_TextInstr_type(g, ")");
    }
    if(gn->ed_pan->visible == true)
    {
        J_TextInstr_type(g, ":pan(");
        J_Percent_Instr_type(g, gn->ed_pan);
        J_TextInstr_type(g, ")");
    }
    if(gn->ed_len->visible == true)
    {
        J_TextInstr_type(g, ":len(");
        J_Percent_Instr_type(g, gn->ed_len);
        J_TextInstr_type(g, ")");
    }

    int cx = x + pixlen;
    if(CursX > cx)
    {
        cx = CursX;
    }

    if(loc == Loc_MainGrid)
    {
        gn->SetAbsArea(x, CursY - (lineHeight - 1), cx, CursY - 1);
    }
    else if(loc == Loc_SmallGrid)
    {
        if(gAux->workPt->ptype == Patt_Pianoroll)
        {
            gn->SetAbsArea(x, CursY - PianorollLineHeight + 1, cx, CursY - 1);
        }
        else
        {
            gn->SetAbsArea(x, CursY - (gAux->lineHeight - 1), cx, CursY - 1);
        }
    }

    CursX = xc;
    CursY = yc;
}

void J_HPianoKey(Graphics& g, int x, int y, int width, int key, int height, bool full, bool cutup, bool cutdown, bool half, bool pressed)
{
    bool black = false;
    bool raise = false;
    bool lower = false;

    int hgt = half == false ? height : height/2;

    if(key == 1 || key == 3 || key == 6 || key == 8 || key == 10)
    {
        black = true;
    }
    else
    {
        if(cutup == false && (key == 0 || key == 2 || key == 5 || key == 7 || key == 9))
        {
            raise = true;
        }

        if(cutdown == false && (key == 2 || key == 7 || key == 9 || key == 4 || key == 11))
        {
            lower = true;
        }
    }

    if(full == true)
    {
        if(black == true)
        {
            g.setColour(Colour(0xff151515));
            if(pressed)
            {
                g.setColour(Colour(0xffEC8A3A));
            }
            J_FillRect(g, x, y + 1, int(x + width/1.5f), y + hgt - 1);
        }
        else
        {
            g.setColour(Colour(0xffffffff));
            if(pressed)
            {
                g.setColour(Colour(0xffEC8A3A));
            }
            J_FillRect(g, x, y + 1, x + width, y + hgt - 1);
        }
    }

    if(raise == true)
    {
        g.setColour(Colour(0xffffffff));
        if(pressed)
        {
            g.setColour(Colour(0xffEC8A3A));
        }
        J_FillRect(g, int(x + width/1.5f) + 1, y - hgt/2 + 1, x + width, y);
    }
    if(lower == true)
    {
        g.setColour(Colour(0xffFFFFFF));
        if(pressed)
        {
            g.setColour(Colour(0xffEC8A3A));
        }
        J_FillRect(g, int(x + width/1.5f) + 1, y + hgt - 1, x + width, y + hgt - 1 + hgt/2);
    }
}
/*
void GL_PlayToggle(int x, int y, Toggle* ptg)
{
    ptg->x = x;
    ptg->y = y;
    ptg->width = 16;
    ptg->height = 16;

    glLineWidth(1.0f);
    if(*(ptg->state) == false)
    {
        glColor3ub(130, 30, 50);
    }
    else
    {
        glColor3ub(255, 0, 55);
    }

    J_LineTri(x, y, x + 16, y + 8, x, y + 16);
}

void GL_KeysToggle(int x, int y, Toggle* ktg)
{
    ktg->x = x;
    ktg->y = y;
    ktg->width = 11;
    ktg->height = 7;

    glLineWidth(1.0f);
    glColor3ub(90, 170, 200);
    if(*(ktg->state) == false)
    {
        glColor3ub(0, 0, 0);
        J_FilledRect(x, y, x + 11, y + 7);
        glColor3ub(90, 170, 200);
        J_LineRect(x, y, x + 11, y + 7);
    }
    else
    {
        J_FilledRect(x, y, x + 10, y + 7);
    }
}
*/

void J_PatternFoldToggle(Graphics& g,  int x, int y, Toggle* ftg)
{
    ftg->x = x;
    ftg->y = y;
    ftg->width = 8;
    ftg->height = 8;

    g.setColour(Colour(0xff00C8D7));
    if(*(ftg->state) == true)
    {
        J_FilledTri(g, x + 8, y, x, y + 4, x + 8, y + 8);
        g.setColour(Colour(0xff006483));
        J_LineTri(g, x + 8, y, x, y + 4, x + 8, y + 8);
    }
    else if(*(ftg->state) == false)
    {
        J_FilledTri(g, x, y + 2, x + 8, y + 2, x + 4, y + 2 + 8);
        g.setColour(Colour(0xff006483));
        J_LineTri(g, x, y + 2, x + 8, y + 2, x + 4, y + 2 + 8);
    }
}

void J_Pattern(Graphics& g, Pattern* pt, Loc loc)
{
    float tW = (loc == Loc_StaticGrid) ? st_tickWidth : tickWidth;

    int cx = CursX;
    int cy = CursY;

    pt->GetRealCoordinates(loc);
    int gx1 = pt->x;
    int gy1 = pt->y;

	int gx2;
    if(loc == Loc_MainGrid)
    {
        gx2 = pt->EndTickX();
    }
    else if(loc == Loc_StaticGrid)
    {
        gx2 = pt->st_EndTickX();
    }

    int gy2 = Line2Y(pt->track_line + pt->num_lines - 1, Loc_MainGrid);
    int pl = (int)(pt->tick_length*tW);

    int tH, ptheight;
    if(pt->is_fat)
    {
        tH = lineHeight*2;
        ptheight = lineHeight*2 - 2;
    }
    else
    {
        tH = lineHeight;
        ptheight = lineHeight - 2;
    }

    //g.saveState();
    //g.reduceClipRegion(gx1, gy1 - tH + 1, pl, ptheight + 1);

    if(pt->folded == true)
    {
        //J_PattLen(g, gx1, gy1, pl, loc, pt->ptype, pt->muted, pt->OrigPt->is_fat);

        if(pt->is_fat)
        {
            if(pt->OrigPt->limg != NULL)
            {
                g.setColour(Colours::black);
                g.saveState();
                g.reduceClipRegion(gx1, gy1 - tH + 1, pl, tH + 5);
                g.drawImageAt(pt->OrigPt->limg, gx1, gy1 - tH + 2 - 1);
                g.restoreState();
            }
        }
        else if(pt->OrigPt->smallimg != NULL)
        {
            g.setColour(Colours::black);
            g.saveState();
            g.reduceClipRegion(gx1, gy1 - tH + 1, pl, tH + 5);
            g.drawImageAt(pt->OrigPt->smallimg, gx1, gy1 - tH + 2 - 1);
            g.restoreState();
        }

        if(pt->muted)
        {
            g.setColour(Colour(0x5a000000));
            J_FillRect(g, gx1, gy1 - tH + 1, gx1 + pl, gy1 - tH + ptheight + 1);
        }

        // Pattern editing highlight
        if(pt == gAux->workPt || pt == C.patt || pt->OrigPt == C.patt)
        {
            g.setColour(Colours::red);
            J_LineRect(g, gx1, gy1 - tH + 1, gx1 + pl - 1, gy1 - 1);
        }
    }
    else
    {
        g.setColour(Colour(0xff6E00FF));
        J_Line(g, gx1, gy1, gx1, gy1 - 10);
    }

	/*
    if(pt->folded == false)
    {
        g.saveState();
        g.reduceClipRegion(gx1 - 1, gy1, gx2 - 1 - gx1, gy2 - gy1);

        int xt = 0;
        int xtt = 0;
        int xc;
        while(1)
        {
            if((xt + pt->tick_offset/tW)%16 == 0 && (xt + pt->tick_offset/tW) != 0)
            {
                g.setColour(Colour(0xff646496));
            }
            else if((xt + pt->tick_offset/tW)%4 == 0)
            {
                g.setColour(Colour(0xff505078));
            }
            else
            {
                g.setColour(Colour(0xff323264));
            }
        
            xc = xtt + gx1;
            if(xc > gx2)
            {
                break;
            }

            if(xc >= gx1)
            {
                J_Line(g, xc, gy1, xc, gy2);
            }
            xt++;
            xtt += tW;
        }

        int line = 0; 
		int trknum;
        Trk* trk;

        int yc = 0;
        int ytt = 0;

        // Horizontal lines
        while(1)
        {
            yc = ytt + gy1;
            if(yc > gy2)
            {      
                break;
            }

            trknum = GetTrkNumForLine(line, pt);
            trk = GetTrkDataForTrkNum(trknum, pt);

            if(pt->ptype != Patt_Pianoroll)
            {
                g.setColour(Colour(0xff003264));
                if((trk->start_line == line && 
                   (trk->vol_lane.visible == true || trk->pan_lane.visible == true))||
                  ((trk->prev->end_line == line - 1) && 
                   (trk->prev->vol_lane.visible == true || trk->prev->pan_lane.visible == true)))
                {
                    g.setColour(Colour(0xff3232C8));
                }

                if(trk->buncho == true || trk->prev->bunchito == true)
                {
                    g.setColour(Colour(0xff006478));
                }
                else if(trk->bunched == true)
                {
                    g.setColour(Colour(0xff00283C));
                }
            }

            J_Line(g, gx1, yc, gx2 - 1, yc);

            ytt += lineHeight;
            line++;
        }

        for(int lnum = 0; lnum < pt->num_lines; lnum ++)
        {
            trk = GetTrkDataForLine(lnum, pt->OrigPt);
            if(trk->vol_lane.visible == true)
            {
                //J_VolLane(g, pt, trk, gx1 - 1, Line2Y(pt->track_line + trk->trk_end_line + trk->vol_lane.height) - 1, gx2 - gx1, trk->vol_lane.height*lineHeight - 1, false);
            }
            if(trk->pan_lane.visible == true)
            {
                //J_PanLane(g, pt, trk, gx1 - 1, Line2Y(pt->track_line + trk->end_line) - 1, gx2 - gx1, trk->pan_lane.height*lineHeight - 1, false);
            }
        }

        g.setColour(Colour(0xff6E00FF));
        J_LineRect(g, gx1 - 1, gy1, gx2, gy2);

        int h1 = pt->vol_lane_main.visible == false ? 0 : pt->vol_lane_main.height*lineHeight;
        int h2 = pt->pan_lane_main.visible == false ? 0 : pt->pan_lane_main.height*lineHeight;

        J_LineRect(g, gx1 - 1, gy2, gx2, gy2 + h1);
        J_LineRect(g, gx1 - 1, gy2 + h1, gx2, gy2 + h1 + h2);

        if(pt->vol_lane_main.visible == true)
        {
            //J_VolLane(g, pt, NULL, gx1 - 1, Line2Y(pt->track_line + pt->num_lines - 1 + pt->vol_lane_main.height) - 1, gx2 - gx1, pt->vol_lane_main.height*lineHeight - 1, false);
        }
        if(pt->pan_lane_main.visible == true)
        {
            //J_PanLane(g, pt, NULL, gx1 - 1, Line2Y(pt->track_line_end) - 1, gx2 - gx1, pt->pan_lane_main.height*lineHeight - 1, false);
        }

        if(pt->ranged == false)
        {
            g.restoreState();
        }

        Trigger* c;
        Event* ev;
        if(!pt->autopatt)
        {
        	ev = field->first_ev;
        }
        else
        {
        	ev = pt->first_ev;
        }
        while(ev != NULL)
        {
            c = ev->tg_first;
            while(c != NULL)
            {
                if(c->activator == true && c->el->patt == pt && c->el->visible == true && c->el->to_be_deleted == false)
                {
                    if(c->el->type == El_TextString)
                    {
                        J_Txt(g, (Txt*)c->el, loc);
                    }
                    else if(c->el->type == El_Command)
                    {
                        J_Command(g, (Command*)c->el, loc);
                    }
                    else if(c->el->type == El_Samplent)
                    {
                        J_Samplent(g, (Samplent*)c->el, loc);
                    }
                    else if(c->el->type == El_Gennote)
                    {
                        J_Gennote(g, (Gennote*)c->el, loc);
                    }
                    else if(c->el->type == El_Mute)
                    {
                        J_Muter(g, (Muter*)c->el, loc);
                    }
                    else if(c->el->type == El_Break)
                    {
                        J_Break(g, (Break*)c->el, loc);
                    }
                    else if(c->el->type == El_Slider)
                    {
                        J_Slide(g, (Slide*)c->el, loc);
                    }
                    else if(c->el->type == El_Reverse)
                    {
                        J_Reverse(g, (Reverse*)c->el, loc);
                    }
                    else if(c->el->type == El_Repeat)
                    {
                        J_Repeat(g, (Repeat*)c->el, loc);
                    }
                    else if(c->el->type == El_Vibrate)
                    {
                        J_Vibrate(g, (Vibrate*)c->el, loc);
                    }
                    else if(c->el->type == El_Transpose)
                    {
                        J_Transpose(g, (Transpose*)c->el, loc);
                    }
                }
                c = c->ev_next;
            }
            ev = ev->next;
        }

        if(pt->ranged == true)
        {
            g.restoreState();
        }
    }
	*/

    CursX = gx1 + 1;
    CursY = gy1;

    if(pt->muted == true)
        g.setColour(Colour(0x9fFFFFFF)); // Darken muted pattern names
    else
        g.setColour(Colour(0xffFFFFFF));
    if(pt->OrigPt != NULL)
    {
        //J_TString_Small_type(g, pt->name, pt->OrigPt->nmimg);
        pt->name->x = CursX;
        pt->name->y = CursY;
        pt->name->cposx = ti->getStringWidth(pt->name->string);
        CursX += pt->name->cposx;
    }

    if(pt->ed_vol->visible == true)
    {
        J_TextInstr_type(g, ":vol(");
        J_Percent_Patt_type(g, pt->ed_vol);
        J_TextInstr_type(g, ")");
    }

    if(pt->ed_pan->visible == true)
    {
        J_TextInstr_type(g, ":pan(");
        J_Percent_Patt_type(g, pt->ed_pan);
        J_TextInstr_type(g, ")");
    }

    //if(pt->ptype == Patt_Grid && pt->att_original == false)
    //{
    //    J_PatternFoldToggle(g, gx1 - 9, gy1 - 6, pt->foldtoggle);
    //}

    //g.restoreState();

    int mul = 1;
    if(pt->is_fat)
        mul = 2;

    if(pt->folded == true)
    {
        if(loc == Loc_MainGrid)
        {
            pt->SetAbsArea(gx1, CursY - lineHeight*mul + 1, Tick2X(pt->end_tick, loc) - 1, CursY - 1);
        }
        else if(loc == Loc_StaticGrid)
        {
            pt->SetAbsArea(gx1, CursY - lineHeight*mul + 1, Tick2X(pt->end_tick, loc) - 1, CursY - 1);
        }
    }
    else
    {
        if(loc == Loc_MainGrid)
        {
            pt->SetAbsArea(gx1 + 1, CursY - lineHeight + 1, CursX, CursY);
        }
        else if(loc == Loc_StaticGrid)
        {
            pt->SetAbsArea(gx1 + 1, CursY - lineHeight + 1, CursX, CursY);
        }
    }

    CursX = cx;
    CursY = cy;
}

void J_Button(Graphics& g, Butt* bt, int x, int y, int width, int height)
{
    bt->x = x;
    bt->y = y;
    bt->width = width;
    bt->height = height;

    //g.setColour(Colour(0xff1E4446));
    //J_FilledRect(g, bt->x, bt->y, bt->x + bt->width, bt->y + bt->height);

    // panelcolor
    //g.setColour(Colour(0xff11191B));
    //J_FilledRect(g, bt->x + 1, bt->y + 1, bt->x + bt->width, bt->y + bt->height);

    // Reset alpha
    g.setColour(Colour(0xffFFFFFF));

    int adv = 0;
    if(bt == gAux->PattExpand)
    {
        adv = 0;
    }

    if(bt->pressed == false && bt->imgup != NULL)
    {
        g.drawImageAt(bt->imgup, x, y, false);
    }
    else if(bt->pressed == true && bt->imgdn != NULL)
    {
        //drawGlassLozenge(g, x, y, width, height, Colour(0xFFFF8855), 0, 0, false, false, false, false);
        g.drawImageAt(bt->imgdn, x + adv, y + adv, false);
    }

    if(bt->freeset == true)
    {
        bt->drawarea->SetBounds(x, y, width, height);
    }

/*
    if(bt->panel == NULL || bt->coverable == false)
    {
        bt->drawarea->SetBounds(x, y, width, height);
    }
    else
    {
        if(bt == gAux->playbt || bt == gAux->stopbt)
        {
            bt->drawarea->SetBounds(x, y, width, height, 
                                    bt->panel->x,
                                    bt->panel->y,
                                    bt->panel->x + bt->panel->width - 1 - MixCenterWidth - 20,
                                    bt->panel->y + bt->panel->height);
        }
        else
        {
            bt->drawarea->SetBounds(x, y, width, height, 
                                    bt->panel->x,
                                    bt->panel->y,
                                    bt->panel->x + bt->panel->width - 1,
                                    bt->panel->y + bt->panel->height - 1);
        }
    }
*/
}

void J_ButtonRanged(Graphics& g, Butt* bt, int x, int y, int width, int height, int x1r, int x2r, int w, int h)
{
    J_Button(g, bt, x, y, width, height);
    bt->Activate(x, y, width, height, x1r, x2r, w, h);
}

void J_ScrollBard(Graphics& g, int r1, int r2, int coord, ScrollBard* sb, bool fixed, bool rect)
{
    sb->fixed = fixed;
    sb->rect = rect;

    Colour clrgrad = Colour(41, 75, 85);
    if(sb->type == Ctrl_VScrollBar)
    {
        g.setColour(Colour(36, 51, 62));
        J_FillRect(g, coord + 1, r1 + 1, coord + sb->tolschina, r2 - 1);
    
        g.setColour(clrgrad);
        J_LineRect(g, coord + 2, r1 + 2, coord + sb->tolschina + 1 - 2, r2 - 2);
        g.setColour(clrgrad.withAlpha(0.4f));
        J_LineRect(g, coord + 3, r1 + 3, coord + sb->tolschina + 1 - 3, r2 - 3);
        g.setColour(clrgrad.withAlpha(0.3f));
        J_LineRect(g, coord + 4, r1 + 4, coord + sb->tolschina + 1 - 4, r2 - 4);
    }
    else if(sb->type == Ctrl_HScrollBar)
    {
        g.setColour(Colour(36, 51, 62));
        J_FillRect(g, r1 + 1, coord + 1, r2 - 1, coord + sb->tolschina);
    
        g.setColour(clrgrad);
        J_LineRect(g, r1 + 2, coord + 2, r2 - 2, coord + sb->tolschina + 1 - 2);
        g.setColour(clrgrad.withAlpha(0.4f));
        J_LineRect(g, r1 + 3, coord + 3, r2 - 3, coord + sb->tolschina + 1 - 3);
        g.setColour(clrgrad.withAlpha(0.3f));
        J_LineRect(g, r1 + 4, coord + 4, r2 - 4, coord + sb->tolschina + 1 - 4);
    }

    if(rect == true)
    {
        if(sb->type == Ctrl_VScrollBar)
        {
            g.setColour(Colour(0xff3A8A8A));
            J_LineRect(g, coord, r1, coord + sb->tolschina + 1, r2);
            g.setColour(Colour(0xff1A3A3A));
            J_LineRect(g, coord + 1, r1 + 1, coord + sb->tolschina + 1 - 1, r2 - 1);
        }
        else if(sb->type == Ctrl_HScrollBar)
        {
            g.setColour(Colour(0xff3A8A8A));
            J_LineRect(g, r1, coord, r2, coord + sb->tolschina + 1);
            g.setColour(Colour(0xff1A3A3A));
            J_LineRect(g, r1 + 1, coord + 1, r2 - 1, coord + sb->tolschina + 1 - 1);
        }
    }

    if(sb->active == true)
    {
        int pixvisiblelen = r2 - r1;
        float fv = sb->full_len/(sb->full_len - sb->visible_len);
        float apvlff = (pixvisiblelen - 40)*fv;

        sb->pixlen = pixvisiblelen;
        if(sb == gAux->v_sbar)
        {
            sb->cf = apvlff/sb->full_len;
        }
        else
        {
            sb->cf = pixvisiblelen/sb->full_len;
        }

        sb->barpixoffs = RoundFloat(sb->offset*sb->cf);
        sb->actual_barpixoffs = RoundFloat(sb->actual_offset*sb->cf);
        if(fixed == false)
        {
            sb->barpixlen = (int)(sb->visible_len*sb->cf);
        }
        else
        {
            sb->barpixlen = 40;
        }

        if(sb->barpixoffs + sb->barpixlen > pixvisiblelen - 2)
        {
            sb->barpixoffs = pixvisiblelen - sb->barpixlen - 2;
            sb->actual_barpixoffs = sb->barpixoffs;
            //sb->offset = sb->barpixoffs/sb->cf;
        }

        if(pixvisiblelen > sb->barpixlen)
        {
            sb->Activate();
            if(sb->type == Ctrl_VScrollBar)
            {
                sb->y1 = r1;
                sb->y2 = r2;

                sb->x = coord + 1;
                sb->y = r1 + sb->barpixoffs;

                sb->width = sb->tolschina;
                sb->height = pixvisiblelen;

                g.setColour(Colour(84, 105, 110));
                J_FillRect(g, coord + 2, r1 + sb->actual_barpixoffs + 2, 
                             coord + sb->tolschina - 1, r1 + sb->actual_barpixoffs + sb->barpixlen);
                
                g.setColour(Colour(102, 132, 139));
                 J_LineRect(g, coord + 2, r1 + sb->actual_barpixoffs + 2, 
                            coord + sb->tolschina - 1, r1 + sb->actual_barpixoffs + sb->barpixlen);

                //g.setColour(Colour(65, 75, 85));
                //J_LineRect(g, coord + 3, r1 + sb->actual_barpixoffs + 3, 
                //            coord + sb->tolschina - 2, r1 + sb->actual_barpixoffs + sb->barpixlen - 1);

                //g.setColour(Colour(84, 93, 103));
                //J_LineRect(g, coord + 4, r1 + sb->actual_barpixoffs + 4, 
                //            coord + sb->tolschina - 3, r1 + sb->actual_barpixoffs + sb->barpixlen - 2);
                //g.setColour(Colour(77, 109, 131));
                //J_LineRect(g, coord + 4, r1 + sb->actual_barpixoffs + 4, 
                //            coord + sb->tolschina - 3, r1 + sb->actual_barpixoffs + sb->barpixlen - 2);
            }
            else if(sb->type == Ctrl_HScrollBar)
            {
                sb->x1 = r1;
                sb->x2 = r2;

                sb->x = r1 + sb->barpixoffs;
                sb->y = coord + 1;

                sb->width = pixvisiblelen;
                sb->height = sb->tolschina;

                g.setColour(Colour(84, 105, 110));
                J_FillRect(g, r1 + sb->actual_barpixoffs + 2, coord + 2, 
                              r1 + sb->actual_barpixoffs + sb->barpixlen, coord + sb->tolschina - 1);
                
                g.setColour(Colour(102, 132, 139));
                J_LineRect(g, r1 + sb->actual_barpixoffs + 2, coord + 2,
                            r1 + sb->actual_barpixoffs + sb->barpixlen, coord + sb->tolschina - 1);

                //g.setColour(Colour(65, 75, 85));
                //J_LineRect(g, r1 + sb->actual_barpixoffs + 3, coord + 3,
                //            r1 + sb->actual_barpixoffs + sb->barpixlen - 1, coord + sb->tolschina - 2);

                //g.setColour(Colour(84, 93, 103));
                //J_LineRect(g, r1 + sb->actual_barpixoffs + 4, coord + 4,
                //            r1 + sb->actual_barpixoffs + sb->barpixlen - 2, coord + sb->tolschina - 3);
                //g.setColour(Colour(77, 109, 131));
                //J_LineRect(g, r1 + sb->actual_barpixoffs + 4, coord + 4,
                //            r1 + sb->actual_barpixoffs + sb->barpixlen - 2, coord + sb->tolschina - 3);
            }
        }
        else
        {
            sb->Deactivate();
        }
    }
}

void J_RefreshGridImage()
{
    int ih = 280;
    int bw = RoundFloat((beats_per_bar*ticks_per_beat)*tickWidth);
    int iw = bw;
    while(iw < 400)
    {
        iw += bw;
    }

    if(img_maingrid != NULL)
        delete img_maingrid;

    img_maingrid = new Image(Image::ARGB, iw, ih, true);
    Graphics imageContext(*(img_maingrid));

    imageContext.fillAll(Colour(21, 23, 31));

/* // Bluish scheme
    Colour colBar = Colour(92, 92, 111);
    Colour colBeat = Colour(65, 65, 85);
    Colour colStep = Colour(41, 45, 51);

    Colour colHoriz = Colour(41, 55, 59);
*/

    // Blacknwhite scheme
    imageContext.fillAll(Colour(25, 25, 25));
    Colour colBar = Colour(85, 85, 85);
    Colour colBeat = Colour(65, 65, 65);
    Colour colStep = Colour(41, 41, 41);

    Colour colHoriz = Colour(41, 59, 59);

/*
    imageContext.fillAll(Colour(31, 47, 63).darker(0.4f));

    Colour colBar = Colour(0xff5F5F72);
    Colour colBeat = Colour(0xff4A4A5F);
    Colour colStep = Colour(0xff2F353F);

    Colour colHoriz = Colour(0xff3A3F4A);
*/

    int xtick = 0;
    float xpix = 0;
    int xcoord;
    if(tickWidthBack > 1.5f)
    {
        // Horizontal lines
        int yc = 0;
        int line = OffsLine;
        int ytt = 0;
        imageContext.setColour(colHoriz);
        while(1)
        {
            yc = ytt;
            if(yc > ih)
            {
                break;
            }
            J_HLine(imageContext, yc, 0, iw);
            ytt += lineHeight;
        }

        // Vertical lines
        while(1)
        {
            if(xtick%(beats_per_bar*ticks_per_beat)== 0)
            {
                imageContext.setColour(colBar);
            }
            else if(xtick%ticks_per_beat == 0)
            {
                imageContext.setColour(colBeat);
            }
            else
            {
                imageContext.setColour(colStep);
            }

            xcoord = RoundFloat(xpix);
            if(xcoord > iw)
                break;

            J_VLine(imageContext, xcoord, 0, ih);

            xtick++;
            xpix += tickWidth;
        }
    }
    else
    {
        if(tickWidthBack < 1.5f)
        {
            imageContext.setColour(Colour(0x8f1D1D25));
            imageContext.setColour(colStep.darker(0.1f));
            imageContext.fillAll();
        }
        else
        {
            imageContext.setColour(Colour(0x9f1D1D25));
            imageContext.setColour(colStep.darker(0.1f));
            imageContext.fillAll();
        }

        // Horizontal lines
        int yc = 0;
        int line = OffsLine;
        int ytt = 0;
        imageContext.setColour(colHoriz);
        while(1)
        {
            yc = ytt;
            if(yc > ih)
            {
                break;
            }
            J_HLine(imageContext, yc, 0, iw);
            ytt += lineHeight;
        }

        // Vertical lines
        int len = (beats_per_bar*ticks_per_beat);
        int g1 = beats_per_bar;
        int g2 = ticks_per_beat;
        float pixstep = float(tickWidth*ticks_per_beat);
        bool beats = float(pixstep - (int)pixstep) == 0; // Don't draw beats with noninteger step to avoid uneven shit
        beats = true;
        while(1)
        {
            if(g1 == beats_per_bar)
            {
                g1 = 0;
                imageContext.setColour(colBar);
            }
            else
            {
                if(beats)
                    imageContext.setColour(colBeat);
                else
                    imageContext.setColour(Colours::transparentWhite);
            }

            xcoord = int(xpix);
            if(xcoord > iw)
                break;

            J_VLine(imageContext, xcoord, 0, ih);

            g1++;

            xtick += ticks_per_beat;
            xpix += pixstep;
        }
    }
}

void J_Grid(Graphics& g)
{
    g.saveState();
    g.reduceClipRegion(MainX1, MainY1-1, GridX2 - MainX1, MainY2 - MainY1 + 2);

    int len = (beats_per_bar*ticks_per_beat);
    int xoffs = RoundFloat((OffsTick/len - (int)OffsTick/len)*tickWidth*len);

#ifdef USE_OLD_JUCE
    ImageBrush* ibrushMain = new ImageBrush(img_maingrid, GridX1 - xoffs, GridY1, 1);
    g.setBrush(ibrushMain);
#else
    g.setTiledImageFill(*img_maingrid, GridX1 - xoffs, GridY1, 1);
#endif
    J_FillRect(g, GridX1, GridY1, GridX2, GridY2);

    g.setColour(Colour(35, 55, 65));
    J_FillRect(g, MainX1 + 1, MainY1-1, GridX1 - 3, GridY2);

    int trknum;
    Trk* trk;
    int yc = 0;
    int line = OffsLine;
    int ytt = 0;
    g.setFont(*vis);    // For track indexes
    Colour clrindex = panelcolour.brighter(0.1f);
    trk = GetTrkDataForLine(OffsLine, field);
    trknum = trk->trknum;
    while(1 && trk != bottom_trk)
    {
        yc = ytt + GridY1;
        if(yc > GridY2)
            break;
        //J_HLine(g, yc, MainX1 + 1 + pra, GridX2);

        trk = GetTrkDataForLine(line, field);

        if(line == trk->start_line)
        {
/*
            g.setColour(Colour(46, 70, 80).withAlpha(0.6f));
            J_HLine(g, yc, MainX1 + 1, GridX1);
            //g.setColour(Colour(46, 70, 80).withAlpha(0.7f));
            J_HLine(g, yc + 2, MainX1 + 1, GridX1);
            J_HLine(g, yc + 4, MainX1 + 1, GridX1);
            J_HLine(g, yc + 6, MainX1 + 1, GridX1);
            J_HLine(g, yc + 8, MainX1 + 1, GridX1);
            J_HLine(g, yc + 10, MainX1 + 1, GridX1);
            J_HLine(g, yc + 12, MainX1 + 1, GridX1);
            //J_LineRect(g, MainX1 + 1, yc + 2, GridX1, yc + lineHeight - 2);
*/
            //g.setColour(clrindex);
            //g.drawSingleLineText(String::formatted(T("%.3d"), trk->trknum), MainX1 + 3, yc + lineHeight - 7);
        }

        line++;

        ytt += lineHeight;
    }

    /*
    g.setColour(Colour(8, 8, 8));
    J_FillRect(g, GridX1, GridY1, GridX2, GridY2);

    float xtickoffs = (OffsTick - (int)OffsTick)*tickWidth;
    int xtick = (int)OffsTick;
    float xpix = 0;
    int xc;

    Colour colBar = Colour(0xff525265);
    Colour colBeat = Colour(0xff35354A);
    Colour colStep = Colour(0xff22222A);
    if(tickWidthBack > 1.5f)
    {
        // Vertical lines
        while(1)
        {
            if(xtick%(beats_per_bar*ticks_per_beat)== 0)
            {
                g.setColour(colBar);
            }
            else if(xtick%ticks_per_beat == 0)
            {
                g.setColour(colBeat);
            }
            else
            {
                g.setColour(colStep);
            }

            xc = RoundFloat(xpix - xtickoffs + GridX1);
            if(xc > GridX2)
                break;

            if(xc >= GridX1)
            {
                //J_Line(g, xc, GridY1, xc, GridY2 + 1);
                J_VLine(g, xc, GridY1, GridY2 + 1);
            }
            xtick++;
            xpix += tickWidth;
        }
    }
    else
    {
        if(tickWidthBack < 1.5f)
        {
            //colBar = Colour(0xff47475A);
            //colBeat = Colour(0xff2F2F45);
            g.setColour(Colour(0x8f1D1D25));
            g.setColour(colStep.darker(0.1f));
            J_FillRect(g, GridX1, GridY1, GridX2, GridY2);
        }
        else
        {
            //colBar = Colour(0xff4C4C5F);
            //colBeat = Colour(0xff2F2F45);
            g.setColour(Colour(0x9f1D1D25));
            g.setColour(colStep.darker(0.1f));
            J_FillRect(g, GridX1, GridY1, GridX2, GridY2);
        }

        // Vertical lines
        int len = (beats_per_bar*ticks_per_beat);
        int xoffs = RoundFloat((OffsTick/len - (int)OffsTick/len)*tickWidth*len);
        int g1 = beats_per_bar;
        int g2 = ticks_per_beat;
        float pixstep = float(tickWidth*ticks_per_beat);
        bool beats = float(pixstep - (int)pixstep) == 0; // Don't draw beats with noninteger step to avoid uneven shit
        beats = true;
        while(1)
        {
            if(g1 == beats_per_bar)
            {
                g1 = 0;
                g.setColour(colBar);
            }
            else
            {
                if(beats)
                    g.setColour(colBeat);
                else
                    g.setColour(Colours::transparentWhite);
            }

            xc = int(xpix + GridX1 - xoffs);
            if(xc > GridX2)
                break;

            if(xc >= GridX1)
            {
                //J_Line(g, xc, GridY1, xc, GridY2 + 1);
                J_VLine(g, xc, GridY1, GridY2 + 1);
            }

            g1++;

            xtick += ticks_per_beat;
            xpix += pixstep;
        }
    }

    //g.setColour(Colour(4, 30, 35));
    g.setColour(Colour(10, 35, 40));
    J_FillRect(g, MainX1 + 2, MainY1, GridX1 - 3, GridY2);

    int trknum;
    Trk* trk;
    int yc = 0;
    int line = OffsLine;
    int ytt = 0;

    g.setFont(*fix);    // For track indexes
    Colour clrindex = Colour(110, 120, 150);
    trk = GetTrkDataForLine(OffsLine, field);
    trknum = trk->trknum;
    while(1 && trk != bottom_trk)
    {
        g.setColour(Colour(0xff0A3A3A));

        trknum = GetTrkNumForLine(line, field);
        trk = GetTrkDataForTrkNum(trknum, field);
        if((trk->start_line == line && 
           (trk->vol_lane.visible == true || trk->pan_lane.visible == true))||
           ((trk->prev->end_line == line - 1) && 
           (trk->prev->vol_lane.visible == true || trk->prev->pan_lane.visible == true)))
        {
            g.setColour(Colour(0xff5555AF));
        }

        if(trk->buncho == true || (trknum > 0 && trk->prev->bunchito == true))
		{
            g.setColour(Colour(0xff009585));
		}
        else if(trk->bunched == true)
        {
            g.setColour(Colour(0xff00283c));
        }

        yc = ytt + GridY1;
		if(yc > GridY2)
        {      
			break;
        }
        J_HLine(g, yc, MainX1 + 1, GridX2);

        if(line == trk->start_line)
        {
            g.setColour(Colour(46, 70, 80));
            J_HLine(g, yc, MainX1 + 1, GridX1);

            g.setColour(clrindex);
            g.drawSingleLineText(String::formatted(T("%.3d"), trknum), MainX1 + 4, yc + lineHeight - 1);
        }

        line++;

        ytt += lineHeight;
    }*/

    g.setColour(Colour(0xff000000));
    J_HLine(g, GridY1 + 1, GridX1, GridX2);

    g.restoreState();

    //g.setColour(Colour(24, 40, 40));
    //J_VLine(g, GridX1 - 3, MainY1, GridY2 + 1);
    //J_VLine(g, MainX1 + 2, MainY1, GridY2 + 1);

    // Grid border
    g.setColour(Colour(0xff343A42));
    J_VLine(g, GridX1 - 1, MainY1 - 1, MainY2 + 1);

    //J_VLine(g, MainX1 - 1, MainY1 - 1, MainY2);
    //J_VLine(g, MainX1 - 2, MainY1 - 1, MainY2);

    // Lines for mainscale
    //int mw = mixbrowse ? 0 : MixCenterWidth;
    int mw = MixCenterWidth;
    g.setColour(Colour(0xff387482));
    //g.setColour(panelcolour);
    J_HLine(g, MainY1 - 2, MainX1, MainX2 + mw + 1);
    g.setColour(Colour(24, 52, 59));
    //g.setColour(panelcolour.darker(0.4f));
    J_HLine(g, MainY1 - 1, GridX1, GridX2);
    g.setColour(Colour(0xff2A4A55));
    //g.setColour(panelcolour.darker(0.2f));
    J_HLine(g, GridY1, GridX1, GridX2);

    g.setColour(Colour(0xff3E5660));
    J_VLine(g, GridX1 - 2, MainY1 - 1, MainY2 + 1);
    g.setColour(Colour(56, 116, 130));
    J_VLine(g, MainX1, MainY1 - 1, MainY2 + 1);

    g.setColour(Colour(24, 35, 40));
    J_FillRect(g, MainX1 + 1, MainY1, GridX1 - 3, GridY1 - 2);
    g.setColour(Colour(26, 61, 71));
    J_LineRect(g, MainX1 + 1, MainY1, GridX1 - 3, GridY1 - 1);

    J_Button(g, CP->BtFollowPlayback, MainX1 + 2, MainY1, 21, 12);

    MC->MakeSnapshot(&MC->maingrid, GridX1, GridY1, GridX2 - GridX1, GridY2 - GridY1 + 1);
}

void J_Selection(Graphics& g)
{
    if(Sel_Active == true)
    {
        if(M.selloc == Loc_MainGrid || M.selloc == Loc_Other)
        {
            g.saveState();
            g.reduceClipRegion(GridX1, MainY1, GridX2 - GridX1, GridY2 - MainY1 + 1);
            if(M.selmode == Sel_Usual || M.selmode == Sel_Pattern)
            {
                g.setColour(Colour(0xff969696));
                J_LineRect(g, SelX1 + GridX1, SelY1 + GridY1, SelX2 + GridX1, SelY2 + GridY1);
            }
            else if(M.selmode == Sel_Tracks)
            {
                g.setColour(Colour(0x28FFFFFF));
                if(M.patt == field)
                {
                    J_FillRect(g, GridX1 + 1, GridY1 + SelY1, GridX2 - 1, SelY2 + GridY1);
                }
                else
                {
                    int gx1 = M.patt->StartTickX();
                    int gx2 = M.patt->EndTickX();
                    J_FillRect(g, gx1 + 1, GridY1 + SelY1, gx2 - 1, GridY1 + SelY2);
                }
            }
            else if(M.selmode == Sel_Fragment)
            {
                g.setColour(Colour(0x28FFFFFF));
                J_FillRect(g, GridX1 + SelX1, MainY1, GridX1 + SelX2 + 1, GridY2);
            }
            g.restoreState();
        }
        else if(gAux->auxmode == AuxMode_Pattern && M.selloc == Loc_SmallGrid)
        {
            g.saveState();
            g.reduceClipRegion(GridXS1, GridYS1 - 12, GridXS2 - GridXS1 + 1, GridYS2 - (GridYS1 - 12) + 1);
            if(M.selmode == Sel_Usual || M.selmode == Sel_Pattern)
            {
                g.setColour(Colour(0xff969696));
                J_LineRect(g, SelX1 + GridXS1, SelY1 + GridYS1, SelX2 + GridXS1, SelY2 + GridYS1);
            }
            else if(M.selmode == Sel_Tracks)
            {
                g.setColour(Colour(0x28FFFFFF));
                J_FillRect(g, GridXS1 + 1, GridYS1 + SelY1, GridXS2 - 1, SelY2 + GridYS1);
            }
            else if(M.selmode == Sel_Fragment)
            {
                g.setColour(Colour(0x28FFFFFF));
                J_FillRect(g, GridXS1 + SelX1, GridYS1 - 12, GridXS1 + SelX2 + 1, GridYS2);
            }
            g.restoreState();
        }
    }

    if(Loo_Active == true)
    {
        if(M.looloc == Loc_MainGrid)
        {
            g.saveState();
            g.reduceClipRegion(GridX1, MainY1, GridX2 - GridX1, GridY2 - MainY1 + 1);
            g.setColour(Colour(0x28FFFFFF));
            J_FillRect(g, GridX1 + LooX1, MainY1, GridX1 + LooX2 + 1, GridY2);
            g.restoreState();
        }
        else if(gAux->auxmode == AuxMode_Pattern && M.looloc == Loc_SmallGrid)
        {
            g.saveState();
            g.reduceClipRegion(GridXS1, GridYS1 - 12, GridXS2 - GridXS1 + 1, GridYS2 - (GridYS1 - 12) + 1);
            g.setColour(Colour(0x28FFFFFF));
            J_FillRect(g, GridXS1 + LooX1, GridYS1 - 12, GridXS1 + LooX2 + 1, GridYS2);
            g.restoreState();
        }
    }
}

void J_LayoutToggle(Graphics& g, Toggle* ltg, int x, int y)
{
    ltg->active = true;

    ltg->x = x;
    ltg->y = y;

    ltg->width = 21;
    ltg->height = 11;

    if(*(ltg->state) == true)
    {
        //J_SmpNoteLen(g, x, y + lineHeight, 21, Loc_MainGrid);
        //J_LineRect(x + 2, y + 2, x + ftg->width - 2, y + ftg->height - 2);
    }
    else
    {
        //glColor3ub(155, 155, 155);
        g.setColour(Colour(0xff9B9B9B));
        J_LineRect(g, x, y, x + 20, y + 10);
    }
}

void J_WindowToggle(Graphics& g, ButtFix* wbt, int x, int y)
{
    wbt->x = x;
    wbt->y = y;
    wbt->width = 26;
    wbt->height = 17;

    if(wbt->pressed == true)
    {
        if(wbt->imgup != NULL)
        {
            g.drawImageAt(wbt->imgup, x, y, false);
        }
        else
            g.drawImageAt(img_btwnd1, x, y, false);
    }
    else
    {
        if(wbt->imgdn != NULL)
        {
            g.drawImageAt(wbt->imgdn, x, y, false);
        }
        else
            g.drawImageAt(img_btwnd, x, y, false);
    }
}

void J_InstrFoldToggle(Graphics& g, Toggle* ftg, Instrument* instr, int x, int y)
{
    ftg->active = true;

    ftg->x = x;
    ftg->y = y;

    ftg->width = 11;
    ftg->height = 11;

    g.setColour(Colour(0xff000000));
    //J_FilledRect(g, x, y, x + ftg->width - 1, y + ftg->height - 1);

    if(instr->type == Instr_Sample)
    {
        g.setColour(Colour(49, 138, 206));
    }
    else
    {
        //glColor3ub(70, 72, 120);
        g.setColour(Colour(0xff464878));
    }

    //J_LineRect(g, x, y, x + ftg->width - 1, y + ftg->height - 1);
    if(*(ftg->state) == true)
    {
        //g.drawImageAt(img_instrfold1, x, y, false);
        //J_FilledRect(g, x + 3, y + 3, x + ftg->width - 3, y + ftg->height - 3);
    }
    else
    {
        //g.drawImageAt(img_instrfold2, x, y, false);
    }
}

void J_StepSeqCur(Graphics& g, int x, int y)
{
    int xc = CursX, yc = CursY;
    CursX = x - 1;
    CursY = y - 1;

    //glColor4ub(255, 255, 0, 150);
    g.setColour(Colour(0xC8FFFF00));
    J_Line(g, x, y - 15, x + 1, y - 1);

    CursX = xc;
    CursY = yc;
}

void J_InstrCursor_XY(Graphics& g, int x, int y, Instrument* instr, int note)
{
    int xc = CursX, yc = CursY;
    CursX = x;
    CursY = y;

    //glColor4ub(100, 150, 255, 160);
    g.setColour(Colour(0xa06496FF));
    J_TextInstr_type(g, instr->alias->string);

    //glColor4ub(255, 50, 0, 160); // TODO: alpha 255, 50, 0, 150
    //J_note_string(CursX + 1, y - 1, note);

    CursX = xc;
    CursY = yc;
}

void J_GetKeyCursorRect(int* x, int* y, int* w, int* h)
{
    if(C.type == CType_InstanceNote)
	{
       *w = ins->getStringWidth(String(current_instr->alias->string));
       *h = (int)ins->getHeight();
       *x = CursX;
       *y = CursY - *h;
       *h += 3;
	}
    else if(C.type == CType_SlideNote)
	{
       *w = 7;
       *h = SlideHeight + 1;
       *x = CursX;
       *y = CursY - lineHeight + 2;
	}
    else if(C.type == CType_Quad)
	{
       *w = 4;
       *h = 15;
       *x = CursX;
       *y = CursY - *h;
	}
    else
    {
       *w = CursX - CursX0 + 3;
       *h = 14;
       *x = CursX0 - 1;
       *y = CursY - *h + 1;
	}
}

/*
    Returns rectangle for mouse-held stuff, if any.
*/
bool J_GetMouseCursorRect(int* x, int* y, int* w, int* h)
{
    if(M.mmode & MOUSE_DRAG_N_DROPPING)
    {
        if(M.drag_data.drag_type == Drag_Instrument_Alias)
        {
           *h = (int)ins->getHeight() + 2;

            if(M.loc == Loc_MainGrid)
            {
                TString* al = (TString*)M.drag_data.drag_stuff;
                int alw = ins->getStringWidth(al->string);
                Instrument* instr = al->instr;

               *x = Tick2X(M.snappedTick, Loc_MainGrid);
               *y = Line2Y((M.mouse_y - GridY1)/lineHeight + OffsLine, Loc_MainGrid) - *h + 1;

                if(instr->type == Instr_Sample)
                {
                   *w = Calc_PixLength((long)((Sample*)instr)->info.frames, ((Sample*)instr)->info.samplerate, tickWidth);
                }
                else
                {
                   *w = int(instr->last_length*tickWidth);
                }
                if(*w < alw) *w = alw;
            }
            else if(M.loc == Loc_SmallGrid && gAux->workPt->ptype != Patt_StepSeq)
            {
                TString* al = (TString*)M.drag_data.drag_stuff;
                int alw = ins->getStringWidth(al->string);
                Instrument* instr = al->instr;

               *x = Tick2X(M.snappedTick, Loc_SmallGrid);
               *y = Line2Y(M.snappedLine, Loc_SmallGrid) - gAux->bottomincr - *h + 1;

                Loc loc = gAux->workPt->ptype == Patt_Pianoroll ? Loc_SmallGrid : Loc_MainGrid;

                if(al->instr->type == Instr_Sample)
                {
                   *w = (int)Calc_PixLength((long)((Sample*)instr)->info.frames, ((Sample*)instr)->info.samplerate, gAux->tickWidth);
                    if(loc == Patt_Pianoroll)
                    {
                        int note = NUM_PIANOROLL_LINES - M.snappedLine;
                       *w = (int)(*w * (1.0f/(CalcFreqRatio(note - 60))));
                    }
                }
                else
                {
                   *w = int(instr->last_length*gAux->tickWidth);
                }
                if(*w < alw) *w = alw;
            }
            else
            {
                TString* al = (TString*)M.drag_data.drag_stuff;
                Instrument* instr = al->instr;
               *x = M.mouse_x - ins->getStringWidth(instr->name)/2;
               *y = M.mouse_y - *h + 3;
               *w = ins->getStringWidth(instr->name);
            }
        }
        else if(M.drag_data.drag_type == Drag_Instrument_File)
        {
            FileData* fd = (FileData*)M.drag_data.drag_stuff;
            *h = img_instropened->getHeight();
            *w = img_instropened->getWidth();

            *x = M.mouse_x - img_instropened->getWidth()/2;
            *y = M.mouse_y - img_instropened->getHeight()/2;
            *h += 3;
        }
        else if(M.drag_data.drag_type == Drag_Effect_File)
        {
            FileData* fd = (FileData*)M.drag_data.drag_stuff;
            *h = img_mcvst->getHeight();
            *w = img_mcvst->getWidth();

            *x = M.mouse_x - img_mcvst->getWidth()/2;
            *y = M.mouse_y - img_mcvst->getHeight()/2;
            *h += 3;
        }
        else if(M.drag_data.drag_type == Drag_MixCell_Content)
        {
            Mixcell* mc2 = (Mixcell*)M.drag_data.drag_stuff;
            if(mc2->effect == NULL)
            {
               *h = (int)ari->getHeight();
               *w = ari->getStringWidth(String(mc2->mixstr->string));
               *x = M.mouse_x - *w/2;
               *y = M.mouse_y + 4 - *h + 1;
            }
            else
            {
               *h = (int)ins->getHeight();
               *w = ins->getStringWidth(String(mc2->mixstr->string));
               *x = M.mouse_x - *w/2;
               *y = M.mouse_y + 4 - *h + 1;
            }
        }
        else if(M.drag_data.drag_type == Drag_MixCell_Indeks || M.drag_data.drag_type == Drag_MixChannel_Indeks)
        {
            //*Mixcell* mc = (Mixcell*)M.drag_data.drag_stuff;
            //*w = ari->getStringWidth(String(mc->indexstr)) + 1;
           *w = 21;
           *h = (int)ari->getHeight();
           *x = M.mouse_x - 6;
           *y = M.mouse_y + 4 - *h;

            /*
            PEdit* pe = firstPE;
            while(pe != NULL)
            {
                M.active_paramfx = NULL;
                if(M.mouse_x >= pe->x && M.mouse_x <= pe->x + pe->edx &&
                   M.mouse_y <= pe->y && M.mouse_y >= pe->y - pe->edy)
                {
                    if(pe->type == Param_FX)
                    {
                        M.active_paramfx = (DigitStr*)pe;
                        break;
                    }
                }
                pe = pe->next;
            }
            if(M.active_paramfx != NULL)
            {
                g.setColour(Colour(0x58FFFFFF));
                J_FilledRect(g, M.active_paramfx->x, M.active_paramfx->y, 
                                M.active_paramfx->x + M.active_paramfx->edx, 
                                M.active_paramfx->y - M.active_paramfx->edy);
            }*/
        }
        else if(M.drag_data.drag_type == Drag_Command)
        {
            Command* cmd = (Command*)M.drag_data.drag_stuff;
            int hs = (int)ins->getHeight();
            if(M.loc == Loc_MainGrid)
            {
               *x = Tick2X(M.snappedTick, Loc_MainGrid) - 3;
                J_CommandSize(cmd, Loc_MainGrid, w, h);
               *y = Line2Y((M.mouse_y - GridY1)/lineHeight + OffsLine, Loc_MainGrid) - hs;
            }
            else if(M.loc == Loc_SmallGrid && gAux->workPt->ptype == Patt_Grid)
            {
               *x = Tick2X(M.snappedTick, Loc_SmallGrid) - 3;
                J_CommandSize(cmd, Loc_SmallGrid, w, h);
               *y = Line2Y((M.mouse_y - GridYS1)/gAux->lineHeight + gAux->OffsLine, Loc_SmallGrid) - hs;
            }
            else if(M.loc == Loc_StaticGrid)
            {
               *x = Tick2X(M.snappedTick, Loc_StaticGrid) - 3;
                J_CommandSize(cmd, Loc_StaticGrid, w, h);
               *y = Line2Y((M.mouse_y - GridY1)/lineHeight + OffsLine, Loc_MainGrid) - hs;
            }
            else
            {
               *x = M.mouse_x - 3;
                J_CommandSize(cmd, Loc_MainGrid, w, h);
               *y = M.mouse_y - hs;
            }
           *w += 3;
           *h += 3;
        }
        return true;
    }
    /*
    else if(M.mmode & MOUSE_ON_GRID && !(M.mmode & MOUSE_LANEAUXING) && !(M.mmode & MOUSE_SELECTING) &&
            (CP->FastMode->pressed == true || (M.loc == Loc_SmallGrid && Aux->workpt->ptype == Patt_Pianoroll)) &&
             M.pickedup == false && M.LMB == false)
    {
       *h = (int)ins->getHeight();
        if(M.loc == Loc_MainGrid)
        {
           *x = Tick2X(M.mpx + M.patt->start_tick);
           *y = Line2Y(M.mpy + M.patt->track_line) - *h;
        }
        else if(M.loc == Loc_SmallGrid)
        {
           *x = s_Tick2X(M.mpx);
           *y = s_Line2Y(M.mpy) - *h;
        }
        
       *w = ins->getStringWidth(String(current_instr->alias->string));
        return true;
    }
    */
    else
    {
       *x = *y = *w = *h = 0;
        return false;
    }
}

/*
    Draws mouse-held stuff if any. Don't forget to update J_GetMouseCursorRect() function when you add or
    change something here.
*/
void J_MouseCursor(Graphics& g)
{
    if(M.mmode & MOUSE_DRAG_N_DROPPING)
    {
        if(M.drag_data.drag_type == Drag_Instrument_Alias)
        {
            if(M.loc == Loc_MainGrid)
            {
                g.saveState();
                g.reduceClipRegion(GridX1, GridY1, GridX2 - GridX1, GridY2 - GridY1);
                TString* al = (TString*)M.drag_data.drag_stuff;
                Instrument* instr = al->instr;
                int x = Tick2X(M.snappedTick, Loc_MainGrid);
                int y = Line2Y((M.mouse_y - GridY1)/lineHeight + OffsLine, Loc_MainGrid);
                if(instr->type == Instr_Sample)
                {
                    int len = Calc_PixLength((long)((Sample*)instr)->info.frames, ((Sample*)instr)->info.samplerate, tickWidth);
                    J_SmpNoteLen(g, x, y, len, len, Loc_MainGrid);

                    g.setColour(Colour(smpcolour));
                }
                else
                {
                    J_GenNoteLen(g, x, y, int(instr->last_length*tickWidth), Loc_MainGrid);

                    g.setColour(Colour(gencolour));
                }
                J_TextInstr_xy(g, x, y, al->string);
                g.restoreState();
            }
            else if(M.loc == Loc_SmallGrid && gAux->workPt->ptype != Patt_StepSeq)
            {
                g.saveState();
                g.reduceClipRegion(GridXS1, GridYS1, GridXS2 - GridXS1, GridYS2 - GridYS1);
                TString* al = (TString*)M.drag_data.drag_stuff;
                Instrument* instr = al->instr;
                int x = Tick2X(M.snappedTick, Loc_SmallGrid);
                int y = Line2Y(M.snappedLine, Loc_SmallGrid) - gAux->bottomincr;
                Loc loc = gAux->workPt->ptype == Patt_Pianoroll ? Loc_SmallGrid : Loc_MainGrid;
                if(al->instr->type == Instr_Sample)
                {
                    float len = (float)Calc_PixLength((long)((Sample*)instr)->info.frames, ((Sample*)instr)->info.samplerate, gAux->tickWidth);
                    if(loc == Patt_Pianoroll)
                    {
						int note = NUM_PIANOROLL_LINES - M.snappedLine;
                        len *= (1.0f/(CalcFreqRatio(note - 60)));
                    }
                    J_SmpNoteLen(g, x, y, (int)len, (int)len, loc);
                    g.setColour(Colour(smpcolour));
                }
                else
                {
                    J_GenNoteLen(g, x, y, int(instr->last_length*gAux->tickWidth), loc);
                    g.setColour(Colour(gencolour));
                }
                J_TextInstr_xy(g, x, y, al->string);
                g.restoreState();
            }
            else
            {
                TString* al = (TString*)M.drag_data.drag_stuff;
                Instrument* instr = al->instr;
				if(instr->type == Instr_Sample)
                {
                    g.setColour(Colour(smpcolour));
                }
                else
                {
                    g.setColour(Colour(gencolour));
                }
                J_String_Instr_Ranged(g, M.mouse_x - ins->getStringWidth(instr->name)/2, M.mouse_y + 2, instr->name, GenBrowserWidth - 20);
            }
        }
        else if(M.drag_data.drag_type == Drag_Instrument_File || M.drag_data.drag_type == Drag_Effect_File)
        {
            FileData* fd = (FileData*)M.drag_data.drag_stuff;
            //g.setColour(Colour(0xbf000000));
            //g.drawImageAt(img_instropened, M.mouse_x, M.mouse_y, false);
            DragType dtype = M.drag_data.drag_type;

            // Alpha set first
            g.setColour(Colour(0xaf000000));
            if(fd->ftype == FType_VST)
            {
                if(dtype == Drag_Instrument_File)
                {
                    g.drawImageAt(img_instropened, M.mouse_x - img_instropened->getWidth()/2, M.mouse_y - img_instropened->getHeight()/2, false);
                    g.setColour(Colour(0xff9FCFFF));
                }
                else
                {
                    g.drawImageAt(img_mcvst, M.mouse_x - img_mcvst->getWidth()/2, M.mouse_y - img_mcvst->getHeight()/2, false);
                    g.setColour(Colour(0xcfCFEFFF));
                    J_String_Bold_Ranged(g, M.mouse_x - img_mcvst->getWidth()/2 + 3, M.mouse_y - img_mcvst->getHeight()/2 + 10, fd->name, 91);
                }
            }
            else if(fd->ftype == FType_Native)
            {
                if(dtype == Drag_Instrument_File)
                {
                    g.drawImageAt(img_instropened, M.mouse_x - img_instropened->getWidth()/2, M.mouse_y - img_instropened->getHeight()/2, false);
                    g.setColour(Colour(0xffCACAFF));
                }
                else
                {
                    g.drawImageAt(img_mcdelay0, M.mouse_x - img_mcdelay0->getWidth()/2, M.mouse_y - img_mcdelay0->getHeight()/2, false);
                    g.setColour(Colour(0xbf9FDFFF));
                    char* name = strstr(fd->name, " ");
                    name++;
                    J_String_Bold_Ranged(g, M.mouse_x - img_mcdelay0->getWidth()/2 + 3, M.mouse_y - img_mcdelay0->getHeight()/2 + 10, name, 91);
                }
            }
            else if(fd->ftype == FType_Wave)
            {
                if(dtype == Drag_Instrument_File)
                {
                    g.drawImageAt(img_instropened1, M.mouse_x - img_instropened->getWidth()/2, M.mouse_y - img_instropened->getHeight()/2, false);
                    g.setColour(Colour(0xefB2B2B2));
                }
            }

            if(dtype == Drag_Instrument_File)
                J_String_Instr_Ranged(g, M.mouse_x - img_instropened->getWidth()/2 + 3, M.mouse_y - img_instropened->getHeight()/2 + 11, fd->name, GenBrowserWidth - 20);
        }
        else if(M.drag_data.drag_type == Drag_MixCell_Content)
        {
            Mixcell* mc2 = (Mixcell*)M.drag_data.drag_stuff;

            if(mc2->effect == NULL)
            {
                g.setColour(Colour(0xff6482C8));
                J_TextBase_xy(g, M.mouse_x - ari->getStringWidth(String(mc2->mixstr->string))/2, M.mouse_y + 4, mc2->mixstr->string);
            }
            else
            {
                g.setColour(Colour(0xffFFFFFF));
                J_TextInstr_xy(g, M.mouse_x - ins->getStringWidth(String(mc2->mixstr->string))/2, M.mouse_y + 4, mc2->mixstr->string);
            }
        }
        else if(M.drag_data.drag_type == Drag_MixCell_Indeks || M.drag_data.drag_type == Drag_MixChannel_Indeks)
        {
            PEdit* pe = firstPE;
            while(pe != NULL)
            {
                M.active_paramfx = NULL;
                if(M.mouse_x >= pe->x && M.mouse_x <= pe->x + pe->edx &&
                   M.mouse_y <= pe->y && M.mouse_y >= pe->y - pe->edy)
                {
                    if(pe->type == Param_FX)
                    {
                        M.active_paramfx = (DigitStr*)pe;
                        break;
                    }
                }
                pe = pe->next;
            }
            if(M.active_paramfx != NULL)
            {
                g.setColour(Colour(0x58FFFFFF));
                J_FillRect(g, M.active_paramfx->x, M.active_paramfx->y, 
                                M.active_paramfx->x + M.active_paramfx->edx, 
                                M.active_paramfx->y - M.active_paramfx->edy);
            }

            char* indexstr;
            if(M.drag_data.drag_type == Drag_MixCell_Indeks)
            {
                Mixcell* mc = (Mixcell*)M.drag_data.drag_stuff;
                indexstr = mc->indexstr;
            }
            else
            {
                MixChannel* mchan = (MixChannel*)M.drag_data.drag_stuff;
                indexstr = mchan->indexstr;
            }

            g.setColour(Colour(0xffFFFFFF));
            J_TextInstr_xy(g, M.mouse_x - 6, M.mouse_y + 4, indexstr);
        }
        else if(M.drag_data.drag_type == Drag_Command)
        {
            Command* cmd = (Command*)M.drag_data.drag_stuff;
            if(M.loc == Loc_MainGrid)
            {
                int x = Tick2X(M.snappedTick, Loc_MainGrid);
                int y = Line2Y((M.mouse_y - GridY1)/lineHeight + OffsLine, Loc_MainGrid);
                J_Command_xy(g, x, y, cmd, Loc_MainGrid);
            }
            else if(M.loc == Loc_SmallGrid && gAux->workPt->ptype == Patt_Grid)
            {
                int x = Tick2X(M.snappedTick, Loc_SmallGrid);
                int y = Line2Y((M.mouse_y - GridYS1)/gAux->lineHeight + gAux->OffsLine, Loc_SmallGrid);
                J_Command_xy(g, x, y, cmd, Loc_SmallGrid);
            }
            else if(M.loc == Loc_StaticGrid)
            {
                int x = Tick2X(M.snappedTick, Loc_StaticGrid);
                int y = Line2Y((M.mouse_y - GridY1)/lineHeight + OffsLine, Loc_MainGrid);
                J_Command_xy(g, x, y, cmd, Loc_StaticGrid);
            }
            else
            {
                J_Command_xy(g, M.mouse_x, M.mouse_y, cmd, Loc_MainGrid);
            }
        }
    }
    else if(M.mmode & MOUSE_ON_GRID && !(M.mmode & MOUSE_LANEAUXING) && !(M.mmode & MOUSE_SELECTING) &&
            CP->FastMode->pressed == true && M.pickedup == false && M.LMB == false)
    {
        int x = M.mouse_x, y = M.mouse_y;
        if(M.loc == Loc_MainGrid)
        {
            x = Tick2X(M.snappedTick + M.patt->start_tick, Loc_MainGrid);
            y = Line2Y(M.snappedLine + M.patt->track_line, Loc_MainGrid);
        }
        else if(M.loc == Loc_SmallGrid)
        {
            x = Tick2X(M.snappedTick, Loc_SmallGrid);
            y = Line2Y(M.snappedLine, Loc_SmallGrid);
        }

        if(overriding_instr != NULL)
        {
            //J_InstrCursor_XY(g, x, y, overriding_instr, 60);
        }
        else
        {
            //J_InstrCursor_XY(g, x, y, current_instr, 60);
        }
    }
    else if(M.loc == Loc_SmallGrid && !(M.mmode & MOUSE_LANEAUXING) && M.pickedup == false && M.LMB == false && M.RMB == false)
    {
        int x = Tick2X(M.snappedTick, Loc_SmallGrid);
        int y = Line2Y(M.snappedLine, Loc_SmallGrid) - gAux->bottomincr;

        if(gAux->workPt->ptype == Patt_StepSeq)
        {
            if(!(M.mmode & MOUSE_SELECTING))
            {
                //J_StepSeqCur(x, y);
            }
        }
        else if(current_instr != NULL && !(M.mmode & MOUSE_LOWAUXEDGE) && 
               (gAux->workPt->ptype == Patt_Pianoroll || CP->FastMode->pressed == true))
        {
            //J_InstrCursor_XY(g, x, y, current_instr, 60);
        }
    }
    else if(M.mmode & MOUSE_ENVELOPING)
    {
        if(M.env_action == ENVPOINTING)
        {
            Envelope* env = M.active_env;
            Command* cmd = (Command*)M.active_env->element;
            Parameter* param = cmd->param;
            EnvPnt* pnt = M.active_pnt;
        }
    }
}

void J_KeyCursor(Graphics& g)
{
    g.saveState();
    if(C.loc == Loc_MainGrid)
    {
        g.reduceClipRegion(GridX1, GridY1, GridX2 - GridX1, GridY2 - GridY1 + 1);
    }
    else if(C.loc == Loc_SmallGrid)
    {
        g.reduceClipRegion(GridXS1, GridYS1, GridXS2 - GridXS1 + 1, GridYS2 - GridYS1 + 1);
    }

    if(C.type != CType_None)
    {
        int ch = 13;
		if(C.loc == Loc_SmallGrid)
        {
            if(gAux->workPt->ptype == Patt_Pianoroll)
            {
                ch = 9;
            }
        }

    	if(C.patt == NULL || C.CheckVisibility() == NULL)
    	{
            g.setColour(Colour(0xff14FF00));
			if(C.type == CType_UnderVert)
			{
				J_VLine(g, CursX, CursY - ch, CursY);
			    J_HLine(g, CursY, CursX0, CursX + 1);
			}
			else if(C.type == CType_Vertical)
			{
				J_VLine(g, CursX, CursY - ch, CursY);
			}
			else if(C.type == CType_Underline)
			{
			    J_HLine(g, CursY, CursX0, CursX);
			}
			else if(C.type == CType_SlideNote)
			{
				J_LineRect(g, CursX, CursY - lineHeight + 2, CursX + 6, CursY - lineHeight + SlideHeight + 2);
                J_FilledTri(g, CursX, CursY - lineHeight + 2, CursX + 6, CursY - lineHeight + 2, CursX, CursY - lineHeight + SlideHeight + 2);
			}
			else if(C.type == CType_InstanceNote)
			{
				if(current_instr != NULL)
				{
					J_InstrCursor_XY(g, CursX, CursY, current_instr, C.last_note);
				}
			}
			else if(C.type == CType_Quad && 
                    C.loc == Loc_SmallGrid && 
                    CursY - 14 < GridYS2)
            {
                g.setColour(Colour(0xff14C800));
                J_VLine(g, CursX, CursY - 14, CursY);
                //J_VLine(g, CursX + 1, CursY - 13, CursY - 1);
            }
        }
    }
    g.restoreState();
}

void J_Cursors(Graphics& g)
{
    J_MouseCursor(g);
    J_KeyCursor(g);
}

void J_MuteToggle1(Graphics& g, Toggle* mtg, int x, int y)
{
    mtg->active = true;
    mtg->x = x;
    mtg->y = y;

    mtg->width = 11;
    mtg->height = 12;

    g.setColour(Colour(0xffFFFFFF));
    if(*(mtg->state) == true)
    {
        g.drawImageAt(img_muteon, x, y, false);
    }
    else
    {
        g.drawImageAt(img_muteoff, x, y, false);
    }
}

void J_SoloToggle1(Graphics& g, Toggle* stg, int x, int y)
{
    stg->active = true;
    stg->x = x;
    stg->y = y;

    stg->width = 13;
    stg->height = 12;

    g.setColour(Colour(0xffFFFFFF));
    if(*(stg->state) == true)
    {
        g.drawImageAt(img_soloon, x, y, false);
    }
    else
    {
        g.drawImageAt(img_solooff, x, y, false);
    }
}

void J_MuteToggle(Graphics& g, Toggle* mtg, int x, int y)
{
    mtg->active = true;
    mtg->x = x;
    mtg->y = y;

    mtg->width = 12;
    mtg->height = 13;

    g.setColour(Colour(0xffFFFFFF));

    if(*(mtg->state) == true)
    {
        if(mtg->imgup != NULL)
        {
            g.drawImageAt(mtg->imgup, x, y, false);
        }
        else
            g.drawImageAt(img_mute1on, x, y, false);
    }
    else
    {
        if(mtg->imgdn != NULL)
        {
            g.drawImageAt(mtg->imgdn, x, y, false);
        }
        else
            g.drawImageAt(img_mute1off_c, x, y, false);
    }
}

void J_SoloToggle(Graphics& g, Toggle* stg, int x, int y)
{
    stg->active = true;
    stg->x = x;
    stg->y = y;

    stg->width = 12;
    stg->height = 13;

    g.setColour(Colour(0xffFFFFFF));
    if(*(stg->state) == true)
    {
        if(stg->imgup != NULL)
        {
            g.drawImageAt(stg->imgup, x, y, false);
        }
        else
            g.drawImageAt(img_solo1on, x, y, false);
    }
    else
    {
        if(stg->imgdn != NULL)
        {
            g.drawImageAt(stg->imgdn, x, y, false);
        }
        else
            g.drawImageAt(img_solo1off_c, x, y, false);
    }
}

void J_AutoToggle(Graphics& g, Butt* atg, int x, int y)
{
    atg->active = true;
    atg->x = x;
    atg->y = y;

    atg->width = 15;
    atg->height = 15;

    g.setColour(Colour(0xffFFFFFF));
    if(atg->pressed == true)
    {
        if(atg->imgup != NULL)
        {
            g.drawImageAt(atg->imgup, x, y, false);
        }
        else
            g.drawImageAt(img_autoon, x, y, false);
    }
    else
    {
        if(atg->imgdn != NULL)
        {
            g.drawImageAt(atg->imgdn, x, y, false);
        }
        else
            g.drawImageAt(img_autooff, x, y, false);
    }
}

void J_ScaleToggle(Graphics& g, int x, int y, Toggle* stg)
{
    stg->x = x;
    stg->y = y;

    stg->width = 20;
    stg->height = 12;

    if(*(stg->state) == true)
    {
        g.setColour(Colour(0xffC8FF32));
    }
    else
    {
        g.setColour(Colour(0xff644632));
    }
    J_LineRect(g, x, y, x + 19, y + 11);
}

void J_TriLine(Graphics& g, int x, int y, bool left, bool center, bool right)
{
    int delay = 7;

    if(center)
    {
        g.setColour(Colour(0xffFFFFFF));
    }
    else
    {
        g.setColour(Colour(0xff646464));
    }
    J_Line(g, x, y - delay - 1, x, y - 13);

    if(right)
    {
        g.setColour(Colour(0xffFFFFFF));
    }
    else
    {
        g.setColour(Colour(0xff646464));
    }
    J_Line(g, x - 1 + delay, y + delay, x + 9, y + 10);

    if(left)
    {
        g.setColour(Colour(0xffFFFFFF));
    }
    else
    {
        g.setColour(Colour(0xff646464));
    }
    J_Line(g, x - delay, y + delay, x - 10, y + 10);
}

void J_KnobXLarge(Graphics& g, Knob* rr, bool marks, int x, int y)
{
    rr->x = x;
    rr->y = y;
    rr->active = true;

    g.drawImageAt(img_knobxlarge, x - 13, y - 12, false);

    if(marks)
    {
        J_TriLine(g, x, y, false, false, false);
    }

    float xadv;
    if(rr->pos >= PI/2)
    {
        xadv = rr->rad*cos(rr->pos);
    }
    else
    {
        xadv = -(float)(rr->rad*cos(PI/2 + (PI/2 - rr->pos)));
    }
    xadv = -(float)(rr->rad*cos(PI/2 + (PI/2 - rr->pos)));

    g.setColour(Colour(0xffFFFFFF));
    J_Line2(g, rr->x + 1, rr->y + 1, RoundFloat(rr->x + 1 + xadv), RoundFloat(rr->y + 1 - rr->rad*sin(rr->pos)));
}

void J_KnobZLarge(Graphics& g, Knob* rr, bool marks, int x, int y)
{
    rr->x = x;
    rr->y = y;
    rr->active = true;

    g.drawImageAt(img_knobzlarge, x - 13, y - 13, false);

    if(marks)
    {
        J_TriLine(g, x, y, false, false, false);
    }

    float xadv;
    if(rr->pos >= PI/2)
    {
        xadv = rr->rad*cos(rr->pos);
    }
    else
    {
        xadv = -(float)(rr->rad*cos(PI/2 + (PI/2 - rr->pos)));
    }
    xadv = -(float)(rr->rad*cos(PI/2 + (PI/2 - rr->pos)));

    g.setColour(Colour(0xffFFFFFF));
    J_Line2(g, rr->x + 1, rr->y + 1, RoundFloat(rr->x + 1 + xadv), RoundFloat(rr->y + 1 - rr->rad*sin(rr->pos)));
}

void J_KnobLarge(Graphics& g, Knob* rr, bool marks, int x, int y)
{
    rr->x = x;
    rr->y = y;
    rr->active = true;

    g.drawImageAt(img_knoblarge, x - 11, y - 10, false);

    if(marks)
    {
        //J_TriLine(x, y, rr->pos == PI*1.25f, abs(rr->pos - rr->anchorpos) < 0.05, rr->pos == 0 - PI*0.25);
        //J_TriLine(x, y, false, false, false);
    }

    float xadv;
    if(rr->pos >= PI/2)
    {
        xadv = rr->rad*cos(rr->pos);
    }
    else
    {
        xadv = -(float)(rr->rad*cos(PI/2 + (PI/2 - rr->pos)));
    }
    xadv = -(float)(rr->rad*cos(PI/2 + (PI/2 - rr->pos)));

    g.setColour(Colour(0xdf7FDFFF));
    J_Line2(g, rr->x + 1, rr->y + 1, RoundFloat(rr->x + 1 + xadv), RoundFloat(rr->y + 1 - rr->rad*sin(rr->pos)));
}

void J_KnobSmall(Graphics& g, Knob* rr, int x, int y)
{
    rr->x = x;
    rr->y = y;
    rr->active = true;

    g.drawImageAt(img_knobsmall, x - 6, y - 6, false);

    float xadv;
    if(rr->pos >= PI/2)
    {
        xadv = rr->rad*cos(rr->pos);
    }
    else
    {
        xadv = -(float)(rr->rad*cos(PI/2 + (PI/2 - rr->pos)));
    }
    xadv = -(float)(rr->rad*cos(PI/2 + (PI/2 - rr->pos)));

    g.setColour(Colour(0xffFFFFFF));
    J_Line(g, rr->x, rr->y,  RoundFloat(rr->x + xadv), RoundFloat(rr->y - rr->rad*sin(rr->pos)));
}

void J_BigVU(Graphics& g, VU* vu, int x, int y, int h, int oh, bool main = false)
{
    vu->x = x;
    vu->y = y;
    vu->h = h;
    vu->oh = oh;

    int hL = (int)((h + 2)*(vu->getL()));
    int hR = (int)((h + 2)*(vu->getR()));
    int hc;

    Colour clroff;
    Colour clron;
    if(main == false)
    {
        clroff = Colour(24, 73, 93);
        clron = Colour(0xff6FBFFF);
    }
    else
    {
        clroff = Colour(60, 60, 60);
        clron = Colour(0xffAFBFCF).brighter(0.25f);
    }

    hc = 0;
    for(int yy = y; yy > y - (h + 2); yy -= 2)
    {
        if(hc > hL || hL == 0)
        {
            g.setColour(clroff);
        }
        else
        {
            g.setColour(clron);
        }

        J_HLine(g, yy, x, x + 14);
        hc += 2;
    }

    int xw = 33;
    if(main)
        xw = 34;

    hc = 0;
    for(int yy = y; yy > y - (h + 2); yy -= 2)
    {
        if(hc > hR || hR == 0)
        {
            g.setColour(clroff);
        }
        else
        {
            g.setColour(clron);
        }

        J_HLine(g, yy, x + xw, x + xw + 14);
        hc += 2;
    }

    if(vu->getL() > 1)
    {
        g.setColour(Colour(255, 55, 25));
    }
    else
    {
        g.setColour(Colour(78, 38, 38));
    }
    g.fillRect(x, y - (h + oh/2), 14, oh);

    if(vu->getR() > 1)
    {
        g.setColour(Colour(255, 55, 25));
    }
    else
    {
        g.setColour(Colour(78, 38, 38));
    }

    g.fillRect(x + xw, y - (h + oh/2), 14, oh);
}

void J_SliderZoom(Graphics& g, SliderZoom* sl, int x, int y)
{
    sl->x = x;
    sl->y = y;
    sl->width = sl->length + 1;
    
    sl->active = true;

    //J_DarkRect(g, sl->x - 1, sl->y, sl->x + sl->length + 1, sl->y + sl->height - 1);

    //g.setColour(panelcolour.withBrightness(0.35f));
    //J_FillRect(g, sl->x, sl->y + 1, sl->x + sl->length, sl->y + sl->height - 2);
    g.saveState();
    g.reduceClipRegion(sl->x, sl->y + 1, sl->length + 1, sl->height - 2);
    g.drawImageAt(img_zoomback, x - 31, y - 1, false);
    g.restoreState();

    g.setColour(Colour(113, 173, 183).darker(0.1f));
    J_FillRect(g, sl->x, sl->y + 1, sl->x + sl->pos, sl->y + sl->height - 2);

    g.setColour(Colour(113, 173, 183));
    J_LineRect(g, sl->x, sl->y + 1, sl->x + sl->pos, sl->y + sl->height - 2);

    g.setColour(Colour(175, 235, 245).darker(0.5f));
    J_HLine(g, sl->y + 2, sl->x + 1, sl->x + sl->pos);

}

void J_SliderVertical_Main1(Graphics& g, SliderBase* sl, int x, int y, int w)
{
    sl->x = x;
    sl->y = y;
    sl->width = w;
    sl->active = true;

    int yv = y - sl->pos;
    int th = img_bighandle1->getHeight()/2;

    g.setColour(Colour(0xff000000));
    g.drawImageAt(img_vslpad1, x, sl->y - sl->height - th - 3, false);

    // Draw anchor
    if(sl->anchored && sl->param->val == sl->anchorval)
    {
        g.setColour(Colour(0xffFFFFFF));
    }
    else
    {
        g.setColour(Colour(68, 68, 68));
    }

    int anch = RoundFloat(sl->height*InvVolRange);
    J_VLine(g, x, sl->y - anch - 2, sl->y - anch + 2);
    J_VLine(g, x + 17, sl->y - anch - 2, sl->y - anch + 2);

/*
    g.setColour(Colour(107, 153, 177));
    J_FillRect(g, sl->x + 2, yv + 1, sl->x + 12, sl->y - 1);

    g.setColour(Colour(80, 114, 124));
    J_LineRect(g, sl->x + 2, yv + 1, sl->x + 12, sl->y - 1);
*/

    g.saveState();
    g.reduceClipRegion(sl->x + 2, yv + 1, 14, sl->height);
    g.drawImageAt(img_vslpad11, x, sl->y - sl->height - th - 3, false);
    g.restoreState();

    g.drawImageAt(img_bighandle1, x + 2, yv - 4, false);

    J_BigVU(g, sl->vu, x - 16, y + th, sl->height, 2, true);
}

void J_SliderVertical_Main(Graphics& g, SliderBase* sl, int x, int y, int w)
{
    sl->x = x;
    sl->y = y;
    sl->width = w;
    sl->active = true;

    int th = img_bighandle1->getHeight()/2;

    int yv = y - sl->pos;

    // Restoration colour
    //g.setColour(Colour(15, 52, 68));
    //g.fillRect(x, sl->y - sl->height - 4, 15, sl->height + 7);

    g.setColour(Colour(0xff000000));
    g.drawImageAt(img_vslpad1, x, sl->y + th - img_vslpad1->getHeight() + 3, false);

    g.saveState();
    g.reduceClipRegion(x - 1, yv + 1, 17, sl->pos + th);
    g.drawImageAt(img_vslpad11, x, sl->y + th - img_vslpad11->getHeight() + 3, false);
    g.restoreState();

    // Draw anchor
    if(sl->anchored && sl->param->val == sl->anchorval)
    {
        g.setColour(Colour(0xffFFFFFF));
    }
    else
    {
        g.setColour(Colour(28, 76, 96));
    }

    int anch = RoundFloat(sl->height*InvVolRange);
    J_VLine(g, x, sl->y - anch - 2, sl->y - anch + 2);
    J_VLine(g, x + 17, sl->y - anch - 2, sl->y - anch + 2);

/*
    g.setColour(Colour(32, 133, 157).darker(0.0f));
    J_FillRect(g, sl->x + 2, yv + 1, sl->x + 12, sl->y - 1);

    g.setColour(Colour(20, 104, 114).darker(0.2f));
    J_LineRect(g, sl->x + 2, yv + 1, sl->x + 12, sl->y - 1);
*/

    g.drawImageAt(img_bighandle1, x + 2, yv - img_bighandle1->getHeight()/2, false);

    J_BigVU(g, sl->vu, x - 15, y + th - 1, img_vslpad1->getHeight() - 6, 2, true);
}

void J_SliderVertical(Graphics& g, SliderBase* sl, int x, int y, int w)
{
    sl->x = x;
    sl->y = y;
    sl->width = w;
    sl->active = true;

    int th = img_bighandle->getHeight()/2 + 1;

    int yv = y - sl->pos;

    // Restoration colour
    //g.setColour(Colour(15, 52, 68));
    //g.fillRect(x, sl->y - sl->height - 4, 15, sl->height + 7);

    g.setColour(Colour(0xff000000));
    g.drawImageAt(img_vslpad, x, sl->y + th - img_vslpad->getHeight(), false);

    g.saveState();
    g.reduceClipRegion(x - 1, yv + 1, 17, sl->pos + th);
    g.drawImageAt(img_vslpad2, x, sl->y + th - img_vslpad2->getHeight(), false);
    g.restoreState();

    // Draw anchor
    if(sl->anchored && sl->param->val == sl->anchorval)
    {
        g.setColour(Colour(0xffFFFFFF));
    }
    else
    {
        g.setColour(Colour(28, 76, 96));
    }

    int anch = RoundFloat(sl->height*InvVolRange);
    J_VLine(g, x - 1, y - anch - 3, y - anch + 3);
    J_VLine(g, x + 15, y - anch - 3, y - anch + 3);

/*
    g.setColour(Colour(32, 133, 157).darker(0.0f));
    J_FillRect(g, sl->x + 2, yv + 1, sl->x + 12, sl->y - 1);

    g.setColour(Colour(20, 104, 114).darker(0.2f));
    J_LineRect(g, sl->x + 2, yv + 1, sl->x + 12, sl->y - 1);
*/

    g.drawImageAt(img_bighandle, x + 1, yv - img_bighandle->getHeight()/2, false);

    J_BigVU(g, sl->vu, x - 16, y + th - 2, img_vslpad->getHeight() - 4, 4);
}

void J_SliderInstrPan(Graphics& g, SliderHPan* sl, Instrument* instr, int x, int y, bool rect)
{
    sl->x = x;
    sl->y = y;
    sl->active = true;

    sl->height = 7;

    int tw = img_instrpanthumb->getWidth()/2;

    if(sl->anchored && sl->pos == sl->anchorpos)
    {
        g.setColour(Colour(0xffFFFFFF));
    }
    else
    {
        g.setColour(Colour(0xff0F5F5F));
    }

    //int anch = (int)(sl->width*0.5);
    //J_Line(g, sl->x + anch, sl->y - 1, sl->x + anch + 3, sl->y - 1);
    //J_Line(g, sl->x + anch, sl->y + sl->height + 1, sl->x + anch + 3, sl->y + sl->height + 1);

    if(instr->type == Instr_Sample)
    {
        g.drawImageAt(img_smppanback, x - tw, y, false);
    }
    else
    {
        g.drawImageAt(img_genpanback, x - tw, y, false);
    }

    if(sl->pos >= sl->width*0.5)
    {
        /*
        g.setColour(clr2);
        J_FillRect(g, sl->x + sl->width/2 + 1, sl->y + 1, sl->x + sl->pos + 1, sl->y + sl->height - 1);

        g.setColour(Colour(13, 87, 157));
        J_LineRect(g, sl->x + sl->width/2 + 1, sl->y + 1, sl->x + sl->pos + 1, sl->y + sl->height - 1);

        g.setColour(Colour(0xffFFFFFF));
        J_Line(g, sl->x + sl->pos + 1, sl->y + 1, sl->x + sl->pos + 1, sl->y + sl->height);
        */

        g.saveState();
        g.reduceClipRegion(sl->x + sl->width/2, sl->y + 1, sl->pos - sl->width/2 + 1 , sl->height);
        g.drawImageAt(img_instrpanbody, sl->x + sl->width/2 + 1, sl->y + 1, false);
        g.restoreState();
    }
    else
    {
        /*
        g.setColour(Colour(19, 131, 221));
        g.setColour(clr2);
        J_FillRect(g, sl->x + sl->pos + 1, sl->y + 1, sl->x + sl->width/2 + 1, sl->y + sl->height - 1);

        g.setColour(Colour(13, 87, 157));
        g.setColour(clr1);
        J_LineRect(g, sl->x + sl->pos + 1, sl->y + 1, sl->x + sl->width/2 + 1, sl->y + sl->height - 1);

        g.setColour(Colour(0xffFFFFFF));
        J_Line(g, sl->x + sl->pos + 1, sl->y + 1, sl->x + sl->pos + 1, sl->y + sl->height);
        */

        g.saveState();
        g.reduceClipRegion(sl->x + sl->pos + 1, sl->y + 1, sl->width/2 + 1, sl->height);
        g.drawImageAt(img_instrpanbody, sl->x + 1, sl->y + 1, false);
        g.restoreState();
    }

    g.drawImageAt(img_instrpanthumb, x + sl->pos - tw + 1, y + 1, false);
}

void J_SliderMixPan(Graphics& g, SliderHPan* sl, int x, int y, int h, bool rect)
{
    sl->x = x;
    sl->y = y;
    sl->active = true;

    sl->height = h;

    if(sl->anchored && sl->pos == sl->anchorpos)
    {
        g.setColour(Colour(0xffFFFFFF));
    }
    else
    {
        g.setColour(Colour(0xff0F5F5F));
    }

    g.drawImageAt(img_mixpanback, x - 1, y, false);

    if(sl->pos >= sl->width*0.5f)
    {
        g.saveState();
        g.reduceClipRegion(sl->x + sl->width/2, sl->y + 1, sl->pos - sl->width/2 + 1 , sl->height);
        g.drawImageAt(img_mixpanbody, sl->x + sl->width/2 + 1, sl->y + 1, false);
        g.restoreState();
    }
    else
    {
        g.saveState();
        g.reduceClipRegion(sl->x + sl->pos + 1, sl->y + 1, sl->width/2 + 1, sl->height);
        g.drawImageAt(img_mixpanbody, sl->x , sl->y + 1, false);
        g.restoreState();
    }

    g.drawImageAt(img_mixpanthumb, sl->x + sl->pos - 1, sl->y + 1, false);
}

void J_SliderInstrVol(Graphics& g, SliderHVol* sl, Instrument* instr, int x, int y)
{
    sl->x = x;
    sl->y = y;
    sl->active = true;
    int anch;

    int tw = img_instrvolthumb->getWidth()/2;

    sl->height = 9;

    if(sl->anchored && sl->pos == sl->anchorpos)
    {
        g.setColour(Colour(0xffFFFFFF));
    }
    else
    {
        g.setColour(Colour(122, 122, 122));
    }

    anch = RoundFloat(sl->width*InvVolRange);
    J_FillRect(g, sl->x + anch, sl->y - 1, sl->x + anch + 2, sl->y + sl->height + 1);

/*
    // Background panel color
    if(instr->type == Instr_Sample)
    {
        g.setColour(Colour(43, 88, 93));
    }
    else
    {
        g.setColour(Colour(26, 73, 109));
    }
    J_FillRect(g, sl->x - 1, sl->y, sl->x + sl->width + 3, sl->y + sl->height);

    g.setColour(Colour(0x6f353535));
    J_FillRect(g, sl->x, sl->y, sl->x + sl->width + 2, sl->y + sl->height);

    g.setColour(Colour(0x5f000000));
    J_LineRect(g, sl->x, sl->y, sl->x + sl->width + 2, sl->y + sl->height);
*/

    if(instr->type == Instr_Sample)
    {
        g.drawImageAt(img_smpvolback, x - tw, y, false);
    }
    else
    {
        g.drawImageAt(img_genvolback, x - tw, y, false);
    }

    g.saveState();
    g.reduceClipRegion(x - tw - 1, y + 1, sl->pos + tw, sl->height);
    g.drawImageAt(img_instrvolbody, x - tw + 1, y + 1, false);
    g.restoreState();

    g.drawImageAt(img_instrvolthumb, x + sl->pos - tw + 1, y + 1, false);
}

void J_SliderTrackPan(Graphics& g, SliderHPan* sl, int x, int y, bool rect)
{
    sl->x = x;
    sl->y = y;
    sl->active = true;

    sl->height = lineHeight - 4;

    if(sl->anchored && sl->pos == sl->anchorpos)
    {
        g.setColour(Colour(0xffFFFFFF));
    }
    else
    {
        g.setColour(Colour(0xff0F5F5F));
    }

    //int anch = (int)(sl->width*0.5);
    //J_Line(g, sl->x + anch, sl->y - 1, sl->x + anch + 3, sl->y - 1);
    //J_Line(g, sl->x + anch, sl->y + sl->height + 1, sl->x + anch + 3, sl->y + sl->height + 1);

    g.setColour(Colour(13, 35, 43));
    J_FillRect(g, sl->x, sl->y, sl->x + sl->width + 2, sl->y + sl->height);

    g.setColour(Colour(8, 16, 17));
    J_LineRect(g, sl->x, sl->y, sl->x + sl->width + 2, sl->y + sl->height);

    if(rect)
    {
        if(sl->scope->for_track == true)
        {
            g.setColour(Colour(44, 105, 114));
            J_LineRect(g, sl->x, sl->y - 1, sl->x + sl->width + 2, sl->y + sl->height + 1);
        }
        else if(sl->scope->mixcell != NULL)
        {
            g.setColour(Colour(22, 76, 66));
            J_LineRect(g, sl->x, sl->y - 1, sl->x + sl->width + 2, sl->y + sl->height + 1);
        }
        else
        {
            //g.setColour(Colour(13, 35, 43));
            //J_FilledRect(g, sl->x, sl->y, sl->x + sl->width + 2, sl->y + sl->height);
        }
    }

    if(sl->pos >= sl->width*0.5)
    {
        g.setColour(Colour(0xff1382DD));
        J_FillRect(g, sl->x + sl->width/2 + 1, sl->y + 1, sl->x + sl->pos + 1, sl->y + sl->height - 1);

        g.setColour(Colour(0xff0C4D8E));
        J_LineRect(g, sl->x + sl->width/2 + 1, sl->y + 1, sl->x + sl->pos + 1, sl->y + sl->height - 1);

        g.setColour(Colour(0xffFFFFFF));
        J_Line(g, sl->x + sl->pos + 1, sl->y + 1, sl->x + sl->pos + 1, sl->y + sl->height);
    }
    else
    {
        g.setColour(Colour(0xff1382DD));
        J_FillRect(g, sl->x + sl->pos + 1, sl->y + 1, sl->x + sl->width/2 + 1, sl->y + sl->height - 1);

        g.setColour(Colour(0xff0C4D8E));
        J_LineRect(g, sl->x + sl->pos + 1, sl->y + 1, sl->x + sl->width/2 + 1, sl->y + sl->height - 1);

        g.setColour(Colour(0xffFFFFFF));
        J_Line(g, sl->x + sl->pos + 1, sl->y + 1, sl->x + sl->pos + 1, sl->y + sl->height);
    }

    //g.drawImageAt(img_smallhandle, sl->x + sl->pos - 3, sl->y, false);
}

void J_SliderTrackVol(Graphics& g, SliderHVol* sl, int x, int y)
{
    sl->x = x;
    sl->y = y;
    sl->active = true;
    int anch;

    sl->height = lineHeight - 4;

    g.setColour(Colour(0xff142428));
    J_FillRect(g, sl->x - 1, sl->y, sl->x + sl->width + 3, sl->y + sl->height);

    g.setColour(Colour(44, 105, 114));
    J_LineRect(g, sl->x - 1, sl->y - 1, sl->x + sl->width + 3, sl->y + sl->height + 1);

    if(sl->anchored && sl->pos == sl->anchorpos)
    {
        g.setColour(Colour(0xffFFFFFF));
    }
    else
    {
        g.setColour(Colour(8, 56, 56));
    }

    anch = RoundFloat(sl->width*InvVolRange);
    J_FillRect(g, sl->x + anch, sl->y - 1, sl->x + anch + 2, sl->y + sl->height + 1);

    g.setColour(Colour(13, 35, 43));
    //g.setColour(Colour(0, 0, 0));
    J_FillRect(g, sl->x, sl->y, sl->x + sl->width + 2, sl->y + sl->height);

    g.setColour(Colour(8, 16, 17));
    J_LineRect(g, sl->x, sl->y, sl->x + sl->width + 2, sl->y + sl->height);

    //g.setColour(Colour(44, 105, 114));
    //J_LineRect(g, sl->x, sl->y, sl->x + sl->width + 2, sl->y + sl->height);

    g.setColour(Colour(47, 153, 177));
    J_FillRect(g, sl->x + 1, sl->y + 1, sl->x + sl->pos + 1, sl->y + sl->height - 1);

    g.setColour(Colour(10, 94, 104));
    J_LineRect(g, sl->x + 1, sl->y + 1, sl->x + sl->pos + 1, sl->y + sl->height - 1);

    int xpos = sl->x + sl->pos;

    g.setColour(Colour(255, 255, 255));
    J_LineRect(g, xpos - 2, sl->y + 1, xpos + 3, sl->y + sl->height - 2);

    g.setColour(Colour(115, 115, 115));
    J_FillRect(g, xpos - 1, sl->y + 2, xpos + 2, sl->y + sl->height - 3);

    //g.drawImageAt(img_smallhandle, sl->x + sl->pos - 1, sl->y + 1, false);
}

void J_SliderCommon(Graphics& g, SliderParam* sl, int x, int y, int width, int height, bool auxmix = true)
{
    sl->x = x;
    sl->y = y;
    sl->height = height;
    sl->width = width;
    sl->length = width;

    sl->active = true;


    int xpos = sl->x + sl->pos;

    g.setColour(Colour(23, 49, 58));
    J_FillRect(g, sl->x - 1, sl->y, sl->x + sl->width + 1, sl->y + height - 1);

    g.setColour(Colour(38, 70, 81));
    J_FillRect(g, sl->x - 1, sl->y, xpos, sl->y + height - 1);

    g.setColour(Colour(43, 81, 94));
    J_LineRect(g, sl->x, sl->y + 1, xpos, sl->y + height - 2);

    g.setColour(Colour(10, 25, 30));
    J_LineRect(g, sl->x - 1, sl->y, sl->x + sl->width + 1, sl->y + height - 1);

    //g.drawImageAt(img_paramback, x - 1, y, false);

    g.saveState();
    g.reduceClipRegion(x, y + 1, sl->pos, height);
    if(sl->height == 10)
        g.drawImageAt(img_parambody, x, y + 1, false);
    else if(sl->height == 11)
        g.drawImageAt(img_parambody1, x, y + 1, false);
    g.restoreState();

    // Thumb
    //g.setColour(Colour(76, 101, 127));
    //J_LineRect(g, xpos - 2, sl->y + 1, xpos + 3, sl->y + height - 2);

    //g.setColour(Colour(43, 55, 68));
    //J_FillRect(g, xpos - 1, sl->y + 2, xpos + 2, sl->y + height - 3);

    if(sl->height == 10)
        g.drawImageAt(img_tinyhandle3, sl->x + sl->pos - 1, sl->y + 1, false);
    else if(sl->height == 11)
        g.drawImageAt(img_tinyhandle4, sl->x + sl->pos - 1, sl->y + 1, false);
}

void J_SliderCommon2(Graphics& g, SliderParam* sl, int x, int y, int width, int height, bool auxmix = true)
{
    sl->x = x;
    sl->y = y;
    sl->height = height;
    sl->width = width;
    sl->length = width;

    sl->active = true;

    int xcenter = sl->x + sl->width/2;
    int xpos = sl->x + sl->pos;

    g.setColour(Colour(23, 49, 58));
    J_FillRect(g, sl->x - 1, sl->y, sl->x + sl->width + 1, sl->y + height - 1);

    if(xpos >= xcenter)
    {
        g.setColour(Colour(38, 70, 81));
        J_FillRect(g, xcenter, sl->y, xpos, sl->y + height - 1);

        g.setColour(Colour(43, 81, 94));
        J_LineRect(g, xcenter, sl->y + 1, xpos, sl->y + height - 2);

        g.saveState();
        g.reduceClipRegion(xcenter, y + 1, xpos - xcenter, height);
        if(sl->height == 10)
            g.drawImageAt(img_parambody, xcenter, y + 1, false);
        else if(sl->height == 11)
            g.drawImageAt(img_parambody1, xcenter, y + 1, false);
        g.restoreState();
    }
    else
    {
        g.setColour(Colour(38, 70, 81));
        J_FillRect(g, xpos, sl->y, xcenter, sl->y + height - 1);

        g.setColour(Colour(43, 81, 94));
        J_LineRect(g, xpos, sl->y + 1, xcenter, sl->y + height - 2);

        g.saveState();
        g.reduceClipRegion(xpos, y + 1, xcenter - xpos + 1, height);
        if(sl->height == 10)
            g.drawImageAt(img_parambody, x - img_parambody->getWidth()/2, y + 1, false);
        else if(sl->height == 11)
            g.drawImageAt(img_parambody1, x - img_parambody1->getWidth()/2, y + 1, false);
        g.restoreState();
    }

    g.setColour(Colour(10, 25, 30));
    J_LineRect(g, sl->x - 1, sl->y, sl->x + sl->width + 1, sl->y + height - 1);

    // Thumb
    // g.setColour(Colour(76, 101, 127));
    // J_LineRect(g, xpos - 2, sl->y + 1, xpos + 3, sl->y + height - 2);

    // g.setColour(Colour(43, 55, 68));
    // J_FillRect(g, xpos - 1, sl->y + 2, xpos + 2, sl->y + height - 3);

    if(sl->height == 10)
        g.drawImageAt(img_tinyhandle3, sl->x + sl->pos - 1, sl->y + 1, false);
    else if(sl->height == 11)
        g.drawImageAt(img_tinyhandle4, sl->x + sl->pos - 1, sl->y + 1, false);
}

void J_InPin_mbus(Graphics& g, int x, int y, InPin* in)
{
    in->x = x;
    in->y = y;

    in->xc = x + 4;
    in->yc = y - 2;

    in->width = 9; //J_TextInstr_xy(x, y, "in");
    in->height = 5;

    g.setColour(Colour(0x959F9F9F));
    J_Line(g, x + 3, y, x + 3, 27);
    J_Line(g, x + 4, y, x + 4, 27);
    
    g.setColour(Colour(0x509F9F9F));
    J_Line(g, x + 2, y, x + 2, 27);
    J_Line(g, x + 5, y, x + 5, 27);

    g.setColour(Colour(0x3A9F9F9F));
    J_Line(g, x + 1, y, x + 1, 27);
    J_Line(g, x + 6, y, x + 6, 27);

    g.setColour(Colour(0x1A9F9F9F));
    J_Line(g, x, y, x, 27);
    J_Line(g, x + 7, y, x + 7, 27);

    g.setColour(Colour(0x0F9F9F9F));
    J_Line(g, x - 1, y, x - 1, 27);
    J_Line(g, x + 8, y, x + 8, 27);
}

void J_InPin(Graphics& g, int x, int y, InPin* in)
{
    in->x = x;
    in->y = y;

    in->xc = x + 4;
    in->yc = y + 3;

    in->width = 8; //J_TextInstr_xy(x, y, "in");
    in->height = 7;

    g.setColour(Colour(0xff000000));
    J_Round(g, in->xc, in->yc, 6);

    if(in->owna->effect == NULL)
    {
        g.setColour(Colour(0xff1E4696));
    }
    else
    {
        g.setColour(Colour(0xff0032FF));
    }
    J_Circle(g, in->xc, in->yc, 6);
}

void J_InPin_pic(Graphics& g, int x, int y, InPin* in)
{
    in->x = x;
    in->y = y;

    in->xc = x + 5;
    in->yc = y;

    in->width = 10; //J_TextInstr_xy(x, y, "in");
    in->height = 7;

    g.setColour(Colour(0xff000000));

    if(in->owna->effect == NULL)
    {
        //pic_inpin_off(x - 2, y - 3);
        g.drawImageAt(img_mc1, x - 2, y - 3, false);
    }
    else
    {
        //pic_inpin_on(x - 2, y - 3);
        g.drawImageAt(img_mc2, x - 2, y - 3, false);
    }
}

void J_OutPin(Graphics& g, int x, int y, OutPin* out)
{
    out->xc = x + 4;
    out->yc = y + 3;

    g.setColour(Colour(33, 56, 65));
    J_LineRect(g, x, y, x + 6, y + 6);

    //glColor3ub(255, 255, 255);
    g.setColour(Colour(52, 107, 127));
    J_FillRect(g, x + 1, y + 1, x + 5, y + 5);

    out->x = x - 1;
    out->y = y - 1;

    out->width = 8;
    out->height = 8;
}

void J_ButtClose(Graphics& g, Butt* bt, int x, int y)
{
    bt->x = x;
    bt->y = y;
    bt->width = 18;
    bt->height = 18;

    if(bt->pressed == false)
    {
        //glColor3ub(0, 0, 255);
        g.setColour(Colour(0xff0000FF));
    }
    else
    {
        //glColor3ub(255, 100, 0);
        g.setColour(Colour(0xffFF6400));
    }
    J_LineRect(g, x, y, x + 18, y + 18);

    //glColor3ub(255, 255, 255);
    g.setColour(Colour(0xffFFFFFF));
    J_Line(g, x + 2, y + 2, x + 15, y + 15);
    J_Line(g, x + 15, y + 2, x + 2, y + 15);
}

void J_TextButton(Graphics& g, int x, int y, int width, int height, Butt* bt)
{
    bt->x = x;
    bt->y = y;
    bt->width = width;
    bt->height = height;

    if(bt->pressed == true)
    {
        //glColor3ub(255, 0, 0);
        g.setColour(Colour(0xffFF0000));
    }
    else if (bt->enabled == true)
    {
        //glColor3ub(0, 100, 250);
        g.setColour(Colour(0xff0064FA));
    }
    else
    {
        //glColor3ub(120, 120, 120);
        g.setColour(Colour(0xff787878));
    }

    J_LineRect(g, bt->x, bt->y, bt->x + bt->width, bt->y + bt->height);

    if ((bt->title != NULL) && (bt->title[0] != 0))
    {
        if (bt->enabled == true)
        {
            //glColor3ub(60, 136, 250);
            g.setColour(Colour(0xff3C88FA));
        }
        else
        {
            //glColor3ub(120, 120, 120);
            g.setColour(Colour(0xff787878));
        }

        J_TextInstr_xy(g, bt->x + 2, bt->y + 9, bt->title);
    }
}

void J_ButtUp(Graphics& g, Butt* bt)
{
    if(bt->pressed == true)
    {
        //glColor3ub(220, 140, 20);
        g.setColour(Colour(0xffDC6414));
    }
    else
    {
        //glColor3ub(20, 140, 220);
        g.setColour(Colour(0xff148CDC));
    }
    J_LineRect(g, bt->x, bt->y, bt->x + bt->width, bt->y + bt->height);

    //glColor3ub(222, 222, 222);
    g.setColour(Colour(0xff148CDC));
    J_TextInstr_xy(g, bt->x + 3, bt->y + 10, "up");
}

void J_ButtDn(Graphics& g, Butt* bt)
{
    if(bt->pressed == true)
    {
        //glColor3ub(220, 140, 20);
        g.setColour(Colour(0xffDC8C14));
    }
    else
    {
        //glColor3ub(20, 140, 220);
        g.setColour(Colour(0xff148CDC));
    }
    J_LineRect(g, bt->x, bt->y, bt->x + bt->width, bt->y + bt->height);

    //glColor3ub(222, 222, 222);
    g.setColour(Colour(0xffDEDEDE));
    J_TextInstr_xy(g, bt->x + 3, bt->y + 10, "dn");
}

void J_WindowButton(Graphics& g, int x, int y, int width, int height, Butt* bt)
{
    bt->x = x;
    bt->y = y;
    bt->width = width;
    bt->height = height;

    g.setColour(Colour(71, 126, 127));
    J_FillRect(g, bt->x, bt->y, bt->x + bt->width, bt->y + bt->height);

    if(bt->pressed == true)
    {
        g.setColour(Colour(0xffFF0000));
    }
    else if (bt->enabled == true)
    {
        g.setColour(Colour(34, 61, 61));
    }
    else
    {
        g.setColour(Colour(48, 68, 78));
    }

    J_LineRect(g, bt->x, bt->y, bt->x + bt->width, bt->y + bt->height);
    J_Line(g, bt->x, bt->y + 2, bt->x + bt->width, bt->y + 2);

    if(bt->freeset == true)
    {
        bt->drawarea->SetBounds(x, y, width, height);
    }
}

void J_QuantRect(Graphics & g, int x1, int y1, int x2, int y2)
{
    //g.setColour(Colour(35, 36, 71));
    g.setColour(quantmenucolour.darker(1));
    J_FillRect(g, x1 + 1, y1 + 1, x2 - 1, y2 - 1);

    //g.setColour(Colour(10, 105, 173));
    g.setColour(quantmenucolour.darker(0.1f));
    J_LineRect(g, x1, y1, x2, y2);

    //g.setColour(Colour(0x8f105FA3));
    g.setColour(quantmenucolour.darker(0.7f));
    J_LineRect(g, x1 + 1, y1 + 1, x2 - 1, y2 - 1);

    //g.setColour(Colour(0x4f105FA3));
    g.setColour(quantmenucolour.darker(0.8f));
    J_LineRect(g, x1 + 1, y1 + 2, x2 - 2, y2 - 2);

    //g.setColour(Colour(0x2f105FA3));
    g.setColour(quantmenucolour.darker(0.85f));
    J_LineRect(g, x1 + 1, y1 + 3, x2 - 3, y2 - 3);
}

void J_QuantMenu(Graphics& g, int x, int y, DropMenu* qm)
{
    if(M.quanting == false || (M.quanting == true && M.active_menu != qm))
    {
        if(qm->panel != NULL)
        {
            g.saveState();
            if(qm == gAux->qMenu)
            {
                g.reduceClipRegion(qm->panel->x, 
                                   qm->panel->y, 
                                   qm->panel->width - MixCenterWidth - 20,
                                   qm->panel->height);
            }
            else
            {
                g.reduceClipRegion(qm->panel->x, qm->panel->y, qm->panel->width, qm->panel->height);
            }
        }

        if(qm->dir == true)
        {
            qm->x = x;
            qm->y = y;

            qm->width = 56;
            qm->height = 20;

            J_QuantRect(g, x - 1, y - 1, x + qm->width + 1, y + 20 + 1);

            g.setColour(Colour(0xdfFFFFFF));
            if(qm->main_item != NULL)
            {
                J_TextInstr_xy(g, x + 5, y + 16, qm->main_item->title);
            }
            J_FilledTri(g, x + qm->width - 8, y + 1, x + qm->width, y + 1, x + qm->width - 4, y + 5);
        }
        else
        {
            qm->x = x;
            qm->y = y;

            qm->width = 56;
            qm->height = 20;

            J_QuantRect(g, x - 1, y - 1, x + qm->width + 1, y + 20 + 1);

            g.setColour(Colour(0xdfFFFFFF));
            if(qm->main_item != NULL)
            {
                J_TextInstr_xy(g, x + 5, y + 16, qm->main_item->title);
            }
            J_FilledTri(g, x + qm->width - 8, y + 20, x + qm->width, y + 20, x + qm->width - 4, y + 16);
        }

        if(qm->panel != NULL)
        {
            g.restoreState();
        }

        qm->UpdateBounds(false);
    }
    else if(M.quanting == true && M.active_menu == qm)
    {
        if(qm->dir == true)
        {
            qm->x = x;
            qm->y = y;

            qm->width = 56;
            qm->height = 15*qm->num_items + 9;

            J_QuantRect(g, x - 1, y - 1, x + qm->width + 1, y + 15*qm->num_items + 9 + 1);


            int ni = 0;
            MenuItem* mi = qm->first_item;
            while(mi != NULL)
            {
                mi->x = x + 2;
                mi->y = y + 5 + ni*15;
                mi->width = qm->width - 3;
                mi->height = 14;

                if(mi == qm->main_item)
                {
                //    g.setColour(Colour(0xff1E6464));
                //    J_FilledRect(g, mi->x - 1, mi->y, mi->x + mi->width, mi->y + mi->height);
                }
                if(mi == M.active_ctrl)
                {
                    g.setColour(Colour(0x2fFFFFFF));
                    J_FillRect(g, mi->x - 1, mi->y, mi->x + mi->width, mi->y + mi->height);
                }

                if(mi == qm->main_item)
                {
                    g.setColour(Colour(0xffFFFFFF));
                }
                else
                {
                    g.setColour(Colour(0xafFFFFFF));
                }
                J_TextInstr_xy(g, x + 5, y + 16 + ni*15, mi->title);
                ni++;
                if(mi->itemtype == MItem_Divider)
                {
                    mi->active = false;
                }
                mi = mi->inext;
            }
            g.setColour(Colour(0xffFFFFFF));
            J_FilledTri(g, x + qm->width - 8, y + qm->height, x + qm->width, y + qm->height, x + qm->width - 4, y + qm->height - 4);
        }
        else
        {
            qm->x = x;

            qm->width = 56;
            qm->height = 15*qm->num_items + 9;
            qm->y = y + 20 - qm->height;

            J_QuantRect(g, x - 1, qm->y - 1, x + qm->width + 1, qm->y + qm->height + 1);

            int ni = 0;
            MenuItem* mi = qm->first_item;
            while(mi != NULL)
            {
                mi->x = x + 2;
                mi->y = y + 1 - ni*15;
                mi->width = qm->width - 3;
                mi->height = 14;

                if(mi == qm->main_item)
                {
                //    g.setColour(Colour(0xff1E6464));
                //    J_FilledRect(g, mi->x - 1, mi->y, mi->x + mi->width, mi->y + mi->height);
                }
                if(mi == M.active_ctrl)
                {
                    g.setColour(Colour(0x2fFFFFFF));
                    J_FillRect(g, mi->x - 1, mi->y, mi->x + mi->width, mi->y + mi->height);
                }

                if(mi == qm->main_item)
                {
                    g.setColour(Colour(0xffFFFFFF));
                }
                else
                {
                    g.setColour(Colour(0xafFFFFFF));
                }
                J_TextInstr_xy(g, x + 5, y + 12 - ni*15, mi->title);
                ni++;
                if(mi->itemtype == MItem_Divider)
                {
                    mi->active = false;
                }
                mi = mi->inext;
            }
            g.setColour(Colour(0xffFFFFFF));
            J_FilledTri(g, x + qm->width - 8, qm->y + 1, x + qm->width, qm->y + 1, x + qm->width - 4, qm->y + 5);
        }
        qm->UpdateBounds(true);
    }
}

void J_AutoAdvToggle(Graphics& g, int x, int y, Toggle* atg)
{
    atg->x = x;
    atg->y = y;

    atg->width = 70;
    atg->height = 14;

    //pic_autoadv(x, y, *(atg->state));
    if(*(atg->state) == false)
    {
        g.drawImageAt(img_autoadv1, x, y, false);
    }
    else
    {
        g.drawImageAt(img_autoadv2, x, y, false);
    }
}

void J_AutoSwitchToggle(Graphics& g, int x, int y, Toggle* atg)
{
    atg->x = x;
    atg->y = y;

    atg->width = 70;
    atg->height = 14;

    if(*(atg->state) == false)
    {
        g.drawImageAt(img_autoswitch1, x, y, false);
    }
    else
    {
        g.drawImageAt(img_autoswitch2, x, y, false);
    }
}

void J_RelSwitchToggle(Graphics& g, int x, int y, Toggle* rtg)
{
    rtg->x = x;
    rtg->y = y;

    rtg->width = 50;
    rtg->height = 14;

    if(*(rtg->state) == false)
    {
        g.drawImageAt(img_relswitch1, x, y, false);
    }
    else
    {
        g.drawImageAt(img_relswitch2, x, y, false);
    }
}

int J_MainMenuItem(Graphics& g, int xc, int yc, int adv, int width, int height, MenuItem* mitem)
{
    mitem->y = yc - 15;

    mitem->height = height;
    mitem->width = width;

    int w = prj->getStringWidth(String(mitem->title));
    mitem->x = xc;

    if(M.active_ctrl == mitem || (M.menu_active == true && !M.quanting && mitem->submenu == M.contextmenu))
    {
        g.setColour(Colour(165, 165, 165));
        J_FillRect(g, mitem->x, mitem->y, mitem->x + mitem->width - 1, mitem->y + mitem->height - 1);
    }

    int txtadv = adv;
    g.setColour(Colour(0xffF5F5F5));
    J_TextRox_xy(g, xc + txtadv + (width - w)/2, yc - 1, mitem->title);

    return w;
}

int J_MenuItem(Graphics& g, int xc, int yc, int adv, int width, MenuItem* mitem)
{
    mitem->x = xc;
    mitem->y = yc - 12;

    mitem->width = width;
    mitem->height = 18;

    if(M.active_ctrl != mitem)
    {
        g.setColour(Colour(217, 217, 217));
    }
    else
    {
        g.setColour(Colour(61, 61, 83));
        J_FillRect(g, mitem->x, mitem->y, mitem->x + mitem->width + 2, mitem->y + mitem->height - 1);
        g.setColour(Colour(0xffFFFFFF));
    }

    int w = 0;
    int txtadv = adv;

    if((mitem->title != NULL) && (mitem->title[0] != 0))
    {
        J_String_Instr_Ranged(g, xc + txtadv, yc, mitem->title, width - 25);
        w = width;
    }
    else
    {
        switch (mitem->itemtype)
        {
            case MItem_MountPianoRoll:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Mount piano roll");
                break;
            case MItem_UnmountPianoRoll:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Unmount piano roll");
                break;
            case MItem_Delete:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Delete element");
                break;
            case MItem_Load:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Load file");
                break;
            case MItem_DrawPicture:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Draw picture");
                break;
            case MItem_AddElement:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Add element:");
                break;
            case MItem_ShowVolumeLane:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Show volumes");
                break;
            case MItem_ShowPanLane:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Show pans");
                break;
            case MItem_HideVolumeLane:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Hide volumes");
                break;
            case MItem_HidePanLane:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Hide pans");
                break;
            case MItem_DeleteDefinition:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Delete definition");
                break;
            case MItem_File:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "File");
                break;
            case MItem_Edit:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Edit");
                break;
            case MItem_View:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "View");
                break;
            case MItem_Help:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Help");
                break;
            case MItem_New:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "New project");
                break;
            case MItem_Open:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Open project");
                g.setColour(Colour(175, 195, 217));
                J_TextInstr_xy(g, xc + txtadv + 138, yc, "Ctrl+O");
                break;
            case MItem_Save:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Save");
                g.setColour(Colour(175, 195, 217));
                J_TextInstr_xy(g, xc + txtadv + 138, yc, "Ctrl+S");
                break;
            case MItem_SaveAs:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Save as...");
                g.setColour(Colour(175, 195, 217));
                J_TextInstr_xy(g, xc + txtadv + 110, yc, "Ctrl+Shift+S");
                break;
            case MItem_Render:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Render");
                break;
            case MItem_Exit:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Exit");
                break;
            case MItem_CleanSong:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Clean all elements");
                break;
            case MItem_Preferences:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Preferences");
                break;
            case MItem_ShowAll:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Show all");
                break;
            case MItem_HideAll:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Hide all");
                break;
            case MItem_FullScreen:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Toggle fullscreen");
                break;
            case MItem_Contents:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Contents");
                break;
            case MItem_HotKeys:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Keyboard reference");
                break;
            case MItem_About:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "About");
                break;
            case MItem_InsertTrack:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Insert track");
                break;
            case MItem_DeleteTrack:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Delete track");
                break;
            case MItem_DedicatePianoRoll:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Dedicate pianoroll");
                break;
            case MItem_DeleteInstrument:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Delete instrument");
                break;
            case MItem_CreateUsualPattern:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Create usual pattern");
                break;
            case MItem_CreatePianorollPattern:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Create pianoroll pattern");
                break;
            case MItem_CleanMixCell:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Delete content");
                break;
            case MItem_SetMixCellToGain:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Set to gain mode");
                break;
            case MItem_SetMixCellToSend:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Set to send mode");
                break;
            case MItem_LoadInstrumentPlugin:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Load instrument plug-ins");
                break;
            case MItem_LoadEffectPlugin:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Load effect plug-ins");
                break;
            case MItem_LoadSample:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Load samples");
                break;
            case MItem_LoadProject:
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Load projects");
                break;
            case MItem_Undo:
                if(uM->current_action->prev == NULL)
                {
                    mitem->doable = false;
                    g.setColour(Colour(117, 117, 117));
                }
                else
                    mitem->doable = true;
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Undo");
                /*
                if(mitem->doable == true)
                {
                    w += 3;
                    switch(uM->current_action->atype)
                    {
                        case Action_Add:
                            J_TextInstr_xy(g, xc + txtadv + w + 3, yc, "Add");
                            break;
                        case Action_Delete:
                            J_TextInstr_xy(g, xc + txtadv + w + 3, yc, "Delete");
                            break;
                        case Action_Move:
                            J_TextInstr_xy(g, xc + txtadv + w + 3, yc, "Move");
                            break;
                        case Action_Bind:
                            J_TextInstr_xy(g, xc + txtadv + w + 3, yc, "Binding");
                            break;
                        case Action_ParamChange:
                            J_TextInstr_xy(g, xc + txtadv + w + 3, yc, "Param Editing");
                            break;
                    }
                }
                */
                break;
            case MItem_Redo:
                if(uM->current_action->next == NULL)
                {
                    mitem->doable = false;
                    g.setColour(Colour(117, 117, 117));
                }
                else
                    mitem->doable = true;
                w = J_TextInstr_xy(g, xc + txtadv, yc, "Redo");
                /*
                if(mitem->doable == true)
                {
                    w += 3;
                    switch(uM->current_action->next->atype)
                    {
                        case Action_Add:
                            w += J_TextInstr_xy(g, xc + txtadv + w + 3, yc, "Add");
                            break;
                        case Action_Delete:
                            w += J_TextInstr_xy(g, xc + txtadv + w + 3, yc, "Delete");
                            break;
                        case Action_Move:
                            w += J_TextInstr_xy(g, xc + txtadv + w + 3, yc, "Move");
                            break;
                        case Action_Bind:
                            w += J_TextInstr_xy(g, xc + txtadv + w + 3, yc, "Binding");
                            break;
                        case Action_ParamChange:
                            w += J_TextInstr_xy(g, xc + txtadv + w + 3, yc, "Param Editing");
                            break;
                    }
                }
                */
                break;
            case MItem_Divider:
                g.setColour (Colour (52, 52, 66));
                J_HLine(g, yc - 8, xc + txtadv - 19, xc + width - 2);
                
                g.setColour (Colour (105, 105, 142));
                J_HLine(g, yc - 7, xc + txtadv - 19, xc + width - 2);

                w = 5;
                break;
            default:
                break;
        }
    }
    return w;
}

void J_ContextMenu(Graphics& g, Menu* menu)
{
    if(menu->first_item != NULL)
    {
        g.setColour(Colour(77, 77, 99));
        J_FillRect(g, menu->x, menu->y, menu->x + menu->width, menu->y + menu->height);
    /*
        g.setColour(Colour(163, 163, 163));
        J_Line(g, menu->x, menu->y, menu->x + menu->width, menu->y);
        J_Line(g, menu->x, menu->y, menu->x, menu->y + menu->height);

        J_Line(g, menu->x + menu->width, menu->y, menu->x + menu->width, menu->y + menu->height);
        J_Line(g, menu->x, menu->y + menu->height, menu->x + menu->width, menu->y + menu->height);
    */
        g.setColour(Colour(115, 115, 155));
        J_VLine(g, menu->x, menu->y, menu->y + menu->height);
        J_HLine(g, menu->y, menu->x, menu->x + menu->width + 1);
        J_VLine(g, menu->x + menu->width, menu->y + 1, menu->y + menu->height);
        J_HLine(g, menu->y + menu->height, menu->x, menu->x + menu->width + 1);

        //J_ContextRect(g, menu->x, menu->y, menu->x + menu->width, menu->y + menu->height);

        int w = menu->width;
        int wi;
        int xc = menu->x + 1;
        int yc = menu->y + 15;

        MenuItem* mitem = menu->first_item;
        while(mitem != NULL)
        {
            if(mitem->active)
            {
                wi = J_MenuItem(g, xc, yc, 24, menu->width - 4, mitem);
                if(wi > w)
                    w = wi;
                if(mitem->itemtype != MItem_Divider)
                {
                    yc += MITEM_HEIGHT;
                }
                else
                {
                    yc += DIVIDER_HEIGHT;
                }
            }
    		mitem = mitem->inext;
        }
        menu->width = w;
    }
}

void J_MainMenu(Graphics& g, Menu* menu, int x, int y, bool bckgrnd)
{
    int width, height;

    menu->x = x;
    menu->y = y;
    menu->width = width = 276;
    menu->height = height = 22;

    if(bckgrnd)
    {
        // Main menu background
        g.setColour(Colour(49, 57, 71));
        J_LineRect(g, x, y, x + menu->width , y + height);

        //g.setColour(Colour(0xff19292D));
        //g.setColour(Colour(56, 78,89));
        g.setColour(panelcolour.darker(0.2f));
        J_FillRect(g, x + 1, y + 1, x + menu->width - 1, y + height - 1);
    }

    g.setColour(Colour(0xffFFFFFF));
    int xc = x + 3;
    int yc = y + 13;
    int w;
    MenuItem* mitem = menu->first_item;
    while(mitem != NULL)
    {
        w = J_MainMenuItem(g, xc, yc, 0, 40, height - 3, mitem);
        xc += w + 22;
		mitem = mitem->inext;
        if(mitem != NULL && mitem->itemtype == MItem_Render)
            xc += 22;
    }
}

void J_Timer(Graphics& g, int x, int y)
{
    CP->tmx = x;
    CP->tmy = y;
    CP->tmh = 28;

    //g.setColour(Colour(42, 61, 66));
    //g.fillRect(x, y, 238, CP->tmh);

    //g.setColour(Colour(66, 75, 91));
    //J_LineRect(g, x, y, x + 238, y + CP->tmh);

    g.drawImageAt(img_display, x, y - 3, false);

    g.setFont(*taho);
    g.setColour(Colour(0xcfDFFFEF));
    g.drawSingleLineText(String::formatted(T("%.3d:%.2d:%.3d"), curr_min, curr_sec, curr_msec), x + 3, y + 26);
    g.drawSingleLineText(String::formatted(T("%.3d:%.2d:%1.1f"), curr_bar, curr_beat, curr_step), x + 98, y + 26);

    //g.drawSingleLineText(String::formatted(T("%.3d:%.2d"), total_min, total_sec), x + 195, y + 25 - 1);

    //g.setColour(Colour(255, 255, 255).withAlpha(0.6f));
    g.setColour(Colour(0xcfDFFFEF));
    J_TextInstr_xy(g, x + 9, y + 11, "min");
    J_TextInstr_xy(g, x + 36, y + 11, "sec");
    J_TextInstr_xy(g, x + 62, y + 11, "msec");

    J_TextInstr_xy(g, x + 105, y + 11, "bar");
    J_TextInstr_xy(g, x + 129, y + 11, "beat");
    J_TextInstr_xy(g, x + 156, y + 11, "step");
}

void J_NumbaNum(Graphics& g, Numba* dg, int x, int y)
{
    //g.setColour(panelcolour.darker(0.2f));
    //J_FillRect(g, x + 26, y + 13, x + dg->width, y + dg->height - 1);
    if(dg->img == NULL)
        MC->MakeSnapshot(&dg->img, x + 1, y + 13, dg->width, dg->height - 1);
    else
        g.drawImageAt(dg->img, x + 1, y + 13, false);

    g.setFont(*taho);
    g.setColour(Colour(0xefBFEFFF));
    if(dg->ntype == Numba_BPM)
    {
        g.drawSingleLineText(String::formatted(T("%.2f"), *(dg->fnum)), x + 2, y + dg->height - 1);
    }
    else
    {
        g.drawSingleLineText(String::formatted(T("%d"), *(dg->num)), x + 2, y + dg->height - 1);
    }
}

void J_Numba(Graphics& g, Numba* dg, int x, int y)
{
    dg->active = true;
    dg->x = x;
    dg->y = y;
    
    dg->height = 28;

    int w;
    if(dg->ntype == Numba_BPM)
    {
        w = 79;
    }
    else
    {
        w = 56;
    }

    dg->width = w;

    //g.setColour(panelcolour.darker(0.1f));
    //J_FillRect(g, x, y, x + w + 1, y + dg->height - 1);

    //g.setColour(Colour(41, 178, 148));
    //J_LineRect(g, x - 1, y - 1, x + w + 2, y + dg->height + 1);

    //g.setColour(Colour(49, 57, 71));
    //J_LineRect(g, x, y, x + w + 1, y + dg->height);

    //g.setColour(panelcolour);
    //J_FillRect(g, x, y + 11, x + w - 26, y + 11);

    if(dg->ntype == Numba_BPM)
        g.drawImageAt(img_bpmback, x, y, false);
    else
        g.drawImageAt(img_numback, x, y, false);

    g.setColour(Colour(0xff9FCFDF));
    J_TextInstr_xy(g, x + 2, y + 10, dg->str);

    g.setColour(Colour(0xff8FCFFF));
    Panel* panel = CP;
    J_Button(g, dg->btup, x + w - 25, y + 2, 18, 12);
    J_Button(g, dg->btdn, x + w - 25, y + 15, 18, 12);
    if(dg->ntype == Numba_relOctave)
    {
        panel = gAux;
    }

    J_NumbaNum(g, dg, x, y);

    dg->drawarea->SetBounds(dg->x,
                            dg->y,
                            dg->width,
                            dg->height,
                            panel->x, 
                            panel->y, 
                            panel->x + panel->width - 2,
                            panel->y + panel->height);
}

void J_UpperButtons(Graphics& g, int x, int y)
{
    //J_Button(g, CP->AccBasicGrid, x + 235, y + 1, 75, 54);
    //J_Button(g, CP->AccBasicPatt, x + 2 + 43 + 5, y + 1, 43, 31);
    //J_Button(g, CP->AccBasicMixer, x + 2 + 43 + 5 + 43 + 5, y + 1, 43, 31);

    J_DarkRect(g, x, y, x + 84, y + 21);

    J_Button(g, CP->Config, x + 2, y + 2, 40, 18);
    J_Button(g, CP->Render, x + 2 + 40 + 1, y + 2, 40, 18);
}

void J_MainZoomButtons(Graphics& g, int x, int y, int xc)
{
    //J_DarkRect(g, x, y, x + 152, y + 26);
    //J_Button(g, CP->DefaultScaleMin, x + 2, y + 2, 49, 27);
    //J_Button(g, CP->DefaultScaleMax, x + 2 + 49 + 1, y + 2, 49, 27);
    g.drawImageAt(img_zoomback, x, y, false);

    g.setColour(Colour(0x15FFFFFF));
    J_LineRect(g, x, y, x + img_zoomback->getWidth() - 1, y + img_zoomback->getHeight() - 1);

    J_Button(g, CP->DecrScale, x + 2, y + 2, 22, 19);
    J_Button(g, CP->IncrScale, x + 5 + 22 + 70, y + 2, 22, 19);

    SliderZoom* sl = CP->slzoom;
    J_SliderZoom(g, sl, x + 25, y + 1);
    sl->Activate(sl->x, sl->y, sl->width, sl->height, 0, 0, xc - 1, MainY1);
}

void J_EditModes(Graphics& g, int x, int y)
{
    //J_DarkRect(g, x, y, x + 30, y + 26);
    //J_DarkLine(g, x, y, x + 30, y + 26);

    J_Button(g, CP->NoteMode, x + 2, y + 2, 28, 23);
    J_Button(g, CP->MetroMode, x + 2 + 27 + 2, y + 2, 28, 23);
    //J_Button(g, CP->NoteMode, x + 2 + 27 + 1, y + 2, 27, 27);
    //J_Button(g, CP->SlideMode, x + 2 + 27*2 + 2, y + 2, 27, 27);
    //J_AutoAdvToggle(g, CtrlAdvance + 1, MainY1 - NavHeight - 53, CP->autoadv);
    //J_Button(g, CP->BrushMode, x + 2 + 27*3 + 3, y + 2, 27, 27);
    //J_Button(g, CP->FastMode, x + 2 + 27*3 + 3 + 27 + 1, y + 2, 27, 27);
}

void J_TransportStuff(Graphics& g, int x, int y)
{
    J_DarkRect(g, x, y, x + 240, y + 26);

    J_Button(g, CP->play_butt, x + 2, y + 2, 52, 23);
    J_Button(g, CP->stop_butt, x + 2 + 52 + 1, y + 2, 39, 23);
    J_Button(g, CP->Record, x + 2 + 52 + 1 + 39 + 1, y + 2, 28, 23);

    J_Button(g, CP->Go2Home, x + 2 + 52 + 1 + 39 + 1 + 28 + 1, y + 2, 28, 23);
    J_Button(g, CP->Rewind, x + 2 + 52 + 1 + 39 + 1 + 28 + 1 + 28 + 1, y + 2, 28, 23);
    J_Button(g, CP->FFwd, x + 2 + 52 + 1 + 39 + 1 + 28 + 1 + 28 + 1 + 28 + 1, y + 2, 28, 23);
    J_Button(g, CP->Go2End, x + 2 + 52 + 1 + 39 + 1 + 28 + 1 + 28 + 1 + 28 + 1 + 28 + 1, y + 2, 28, 23);

    J_Timer(g, x + 1, y - 28);
    CP->tmarea->Enable();
    CP->tmarea->SetBounds(CP->tmx, CP->tmy, 187, 87);
}

void J_MixButtons(Graphics& g, int x, int y)
{
    //J_DarkRect(g, x, y, x + 65, y + 23);
    //J_DarkLine(g, x, y, x + 86, y + 23);
    //J_Button(g, mixBrw->HideTrackControls, x + 2, y + 2, 20, 20);
    //J_Button(g, mB->ShowFiles, x + 2, y + 2 + 20 + 1, 20, 20);
    //J_Button(g, CP->ShowTrackControls, x + 2+ 20 + 1, y + 2, 20, 20);
    //J_Button(g, mB->ShowNativePlugs, x + 2, y + 2 + 20 + 1 + 20 + 1 + 20 + 1, 20, 20);
    J_Button(g, mixBrw->ShowExternalPlugs, x + 2,  y + 2, 20, 20);
    J_Button(g, mixBrw->ShowParams, x + 2 + 20 + 1, y + 2, 20, 20);
    J_Button(g, mixBrw->ShowPresets, x + 2 + 20 + 1 + 20 + 1, y + 2, 20, 20);
}

void J_SystemButtons(Graphics& g, int x, int y)
{
    //g.setColour(Colour(16, 71, 61));
    //g.fillRect(x, y, 65, 21);

    //g.setColour(Colour(0xff19292D));
    //g.fillRect(x + 1, y + 1, 63, 19);

    //J_Button(g, CP->HotKeys, x + 2, y  + 2, 27, 17);
    J_Button(g, CP->minimize, x + 2 + 27 + 2, y + 2, 20, 19);
    J_Button(g, CP->maximize, x + 2 + 27 + 2 + 20 + 1, y + 2, 20, 19);
    J_Button(g, CP->close, x + 2 + 27 + 2 + 20 + 1 + 20 + 1, y + 2, 20, 19);
}

void J_CtrlPanel(Graphics& g)
{
    int xc;
    //if(mixbrowse == false)
    {
        xc = MainX2 + MixCenterWidth + 2;
        xc = WindWidth;
    }
    /*
    else
    {
        xc = MainX2;
    }
    */

    CP->x = 0;
    CP->y = 0;
    CP->width = xc;
    CP->height = CtrlPanelHeight + 6;

    //J_PanelRectC(g, 0, MainY1 - 3 - NavHeight - 1 - CtrlPanelHeight + 1, xc + 5, MainY1 - 3);
    g.drawImageAt(img_ctrl1, 0, MainY1 - 3 - NavHeight - 1 - CtrlPanelHeight + 1, false);
    int x2 = img_ctrl1->getWidth();
    while(x2 < WindWidth)
    {
        g.drawImageAt(img_ctrl2, x2, MainY1 - 3 - NavHeight - 1 - CtrlPanelHeight + 1, false);
        x2 += img_ctrl2->getWidth();
    }
    g.drawImageAt(img_ctrl3, WindWidth - img_ctrl3->getWidth(), MainY1 - 3 - NavHeight - 1 - CtrlPanelHeight + 1, false);

    g.saveState();
    g.reduceClipRegion(0, MainY1 - 3 - NavHeight - 1 - CtrlPanelHeight, xc, CtrlPanelHeight + 37);

/*
    GradientBrush gb(Colour(0x00000000), 0, float(MainY1 - 3 - NavHeight - 1 - CtrlPanelHeight + 23), 
                      Colour(0x22000000), 0, float(MainY1 - 11),
                      false);
    g.setBrush(&gb);
    g.fillRect(0, MainY1 - 3 - NavHeight - 1 - CtrlPanelHeight, WindWidth, CtrlPanelHeight + 37);
*/

    Colour gclr = Colours::black;
    int yc = MainY1 - 3 - NavHeight - 1 - CtrlPanelHeight + 1;

/*
    float a = 0;
    for(int k = 1; k < 58; k += 1)
    {
        g.setColour(gclr.withAlpha(a));
        J_HLine(g, yc + k, 2, WindWidth - 3);
        a = float(k)/275;
    }
*/

    Menu* mn = CP->MainMenu;
    J_MainMenu(g, mn, 1, MainY1 - 3 - NavHeight - 1 - CtrlPanelHeight + 4, false);
    mn->drawarea->EnableWithBounds(mn->x, mn->y, mn->width, mn->height,
                                   0, 0, xc - 1, MainY1);

    int sysx = WindWidth - 95;
    if(sysx < CtrlAdvance + 259 + 7)
    {
        sysx = CtrlAdvance + 259 + 7;
    }

    J_SystemButtons(g, sysx, MainY1 - 3 - NavHeight - 1 - CtrlPanelHeight);

    int yb = MainY1 - NavHeight + 2;

    J_Numba(g, CP->bpmer, TranspAdvance - 4, yb - 29);
    J_Numba(g, CP->tpber, TranspAdvance + 75, yb - 29);
    J_Numba(g, CP->bpber, TranspAdvance + 76 + 55, yb - 29);
    J_Numba(g, CP->octaver, TranspAdvance + 77 + 55 + 55, yb - 29);

    // Master volume slida
    SliderBase* sl = CP->MVol;
    J_SliderVertical_Main(g, sl, CtrlAdvance + 259, yb - 5, 14);
    sl->Activate(sl->x - 13 + 11, sl->y - sl->height - 7, 21, 60, 
                 0, 0, xc - 1, MainY1);
    sl->vu->drawarea->EnableWithBounds(sl->x - 17, sl->y - sl->height - 9, 51, 63,
                 0, 0, xc - 1, MainY1);

    J_TransportStuff(g, CtrlAdvance, yb - 27);

    //J_UpperButtons(g, CtrlAdvance + 254, MainY1 - 3 - NavHeight - 1 - CtrlPanelHeight + 3);
    //J_PanelLine(g, CtrlAdvance + 2, MainY1 - 3 - NavHeight - 1 - CtrlPanelHeight + 27, xc);

    int ctrlx = WindWidth - 445;
    if(ctrlx < CtrlAdvance + 303)
        ctrlx = CtrlAdvance + 303;

    g.setColour(Colour(85, 95, 105).darker(0.45f));
    g.fillRect(ctrlx, yb - 27, 63, 27);
    g.setColour(Colour(85, 95, 105).darker(0.2f));
    g.drawRect(ctrlx, yb - 27, 63, 27);
    DropMenu* dmn = CP->qMenu;
    if(M.quanting == false || M.active_menu != dmn)
    {
        J_QuantMenu(g, ctrlx + 3, yb - 24, dmn);
    }
    g.setColour(Colour(255, 255, 255).withAlpha(0.39f));
    J_TextSmall_xy(g, ctrlx + 3, yb - 27, "Grid snap");


    J_MainZoomButtons(g, ctrlx + 84, yb - 24, xc);

    J_EditModes(g, ctrlx + 224, yb - 26);

    //g.setColour(Colour(255, 255, 255).withAlpha(0.4f));
    //J_TextSmall_xy(g, CtrlAdvance + 620, yb - 24, "Zoom");

    /*
    int mixBbX = WindWidth - 110;
    //if(gAux->auxmode == AuxMode_Mixer && mixBrw->CurrButt != mixBrw->HideTrackControls)
    //    mixBbX = WindWidth - MixCenterWidth - 77;

    if(mixBbX < CtrlAdvance + 815)
        mixBbX = CtrlAdvance + 815;
    */

    //J_DarkRect(g, mixBbX, yb - 27, mixBbX + 72, yb - 1);
    J_Button(g, CP->view_mixer, ctrlx + 335, yb - 24, 106, 23);

    g.restoreState();

    /*
    if(M.quanting == true && M.active_menu == dmn)
    {
        J_QuantMenu(g, CtrlAdvance + 319 - 41, MainY1 - NavHeight - 27, dmn);
    }*/

    if(gAux->auxmode == AuxMode_Mixer)
    {
        int mixbX = WindWidth - 69;
        //if(mixBrw->CurrButt != mixBrw->HideTrackControls)
        //    mixbX = WindWidth - MixCenterWidth - 91;

        if(mixbX < CtrlAdvance + 258 + 47)
            mixbX = CtrlAdvance + 258 + 47;
    }

    MC->MakeSnapshot(&img_titlebkg, 0, MainY1 - NavHeight + 2, MainX1 - 7, 24);

    J_MainBar(g);
    J_MainVBar(g);

    CP->CheckVisibility(xc);
}

void J_MainVBar(Graphics& g)
{
    g.setColour(Colour(36, 51, 62));
    J_FillRect(g, GridX2, MainY1 - 1, GridX2 + 19, MainY2);
    g.setColour(panelcolour);
    J_VLine(g, GridX2, MainY1, MainY2);

    //main_v_bar->visible_len = (float)(GridY2 - GridY1)/(float)lineHeight;
    //main_v_bar->offset = main_v_bar->actual_offset = float(OffsLine);

    main_v_bar->Activate();
    J_ScrollBard(g, MainY1 + 16, GridY2 - 17, GridX2, main_v_bar, true, false);

    J_Button(g, CP->up, GridX2 + 2, MainY1, 17, 17);
    J_Button(g, CP->down, GridX2 + 2, GridY2 - 17, 17, 17);
}

void J_MainBar(Graphics& g)
{
    main_bar->visible_len = (float)(GridX2 - GridX1)/(float)tickWidth;
    main_bar->offset = main_bar->actual_offset = OffsTick;

    float px;
    if(C.loc == Loc_MainGrid)
    {
        px = CTick;
    }
    else
    {
        px = C.fieldposx;
    }

    int xr;
    //if(mixbrowse == false)
    {
        xr = MainX2 + MixCenterWidth - 33;
    }
    //else
    //{
    //    xr = MainX2 - 34;
    //}
    CtrlPanelXRange = xr;
    //J_PanelRect1(g, 0, MainY1 - 3 - NavHeight, xr + 32, MainY1 - 3);

    //g.setColour(Colour(0xff000000));
    //J_FilledRect(g, 35, MainY1 - 3 - NavHeight + 3, xr - 1, MainY1 - 3 - 3);

    int yc = MainY1 - NavHeight + 2;
    int xoffs = MainX1 - 7;

    g.setColour(Colour(0xffFFFFFF));
    g.drawImageAt(img_titlebkg, 0, MainY1 - NavHeight + 2, false);

    //g.setColour(Colour(62, 79, 94));
    //J_FillRect(g, 0, MainY1 - NavHeight + 3, xoffs + 3, MainY1 - 3);
    //g.setColour(Colour(49, 57, 71));
    //J_LineRect(g, 0, MainY1 - NavHeight + 3, xoffs + 3, MainY1 - 3);

    char* tstr = new char[MAX_NAME_STRING];
    tstr[0] = 0;
    strcat(tstr, PrjData.projname);
    if(!PrjData.projpath.isEmpty())
        strcat(tstr, ".cmm");

    if(ChangesHappened)
        strcat(tstr, "*");

    g.setColour(Colour(0xffDFDFDF));
    J_String_Rox_Ranged(g, 2, yc + 20, tstr, xoffs);

    J_DarkRect(g, 5 + xoffs, yc, 37 + xoffs, MainY1 - 2);
    J_Button(g, CP->Back, 7 + xoffs, yc + 2, 29, 21);

    J_DarkRect(g, xr + 1, yc, xr + 33, MainY1 - 3);
    J_Button(g, CP->Forth, xr + 3, yc + 2, 29, 21);

    main_bar->Activate();
    J_ScrollBard(g, 35 + xoffs, xr + 3, yc, main_bar, true, false);

    if(main_bar->active)
    {
        Txt* txt;
        int xt;
        Element* elem = firstElem;
        while(elem != NULL)
        {
            if(elem->IsPresent() && elem->type == El_TextString && elem->patt == field)
            {
                txt = (Txt*)elem;
                if(txt->bookmark == true)
                {
                    xt = (int)(txt->start_tick/main_bar->full_len*(main_bar->pixlen - 2) + 37 + xoffs);
                    g.setColour(Colour(0xbf8F9FEF));
                    J_VLine(g, xt, MainY1 - 4 - NavHeight + 8, MainY1 - 3);
                    g.setColour(Colour(0xef8F9FEF));
                    J_TextSmall_xy(g, xt + 2, MainY1 - NavHeight + 11, &txt->buff[1]);
                }
            }
            elem = elem->next;
        }
    }
    // To restore alpha
    g.setColour(Colour(0xffFFFFFF));
}

void J_VolLane(Graphics& g, Loc loc, Trk* trk, Pattern* pt, int lx1, int ly1, int lx2, int ly2)
{
    int vol_height, cx, cx1;
	float val;
    //int vol_height1;
    //float outval;
    int auxed;
    int h = ly2 - ly1;
	int hgt1 = (int)((float)(h)/VolRange);

    Trk* actual_trk;
    if(trk != NULL && trk->trkbunch != NULL)
    {
        actual_trk = trk->trkbunch->trk_start;
    }
    else
    {
        actual_trk = trk;
    }

    if(trk != NULL)
    {
        trk->vol_lane.y = ly2;
    }
    else if(pt != NULL)
    {
        pt->vol_lane_main.y = ly2;
    }

    g.setColour(Colour(0xff444E80));
    J_HLine(g, ly2 - hgt1, lx1, lx2);

    //////////////////////////////////////////
    // Draw nonselected elements, if any
    Element* el;
    if(loc == Loc_SmallGrid)
    {
        el = gAux->workPt->OrigPt->first_elem;
    }
    else
    {
        el = firstElem;
    }

    while(el != NULL)
    {
        if(!el->selected)
        {
            if(loc == Loc_SmallGrid)
            {
                auxed = GridXS1 + el->auxauxed;
            }
            else if(loc == Loc_MainGrid)
            {
                auxed = GridX1 + el->gridauxed;
            }
            else
            {
                auxed = StX1 + el->gridauxed;
            }

            if(el->IsPresent() &&
               (pt == NULL || el->patt == pt)&& 
               (trk == NULL || el->field_trkdata == actual_trk)&&
               (auxed != 0xFFFF && auxed >= lx1 && auxed <= lx2))
            {
                if(loc == Loc_SmallGrid)
                {
                    cx = el->s_StartTickX();
                }
                else if(loc == Loc_StaticGrid)
                {
                    cx = el->st_StartTickX();
                }
                else
                {
                    cx = el->StartTickX();
                }

                switch(el->type)
                {
                    case El_Samplent:
                        val = ((Samplent*)el)->loc_vol->val;
                        //outval = ((Samplent*)el)->loc_vol->outval;
                        break;
                    case El_Gennote:
                        val = ((Gennote*)el)->loc_vol->val;
                        //outval = ((Gennote*)el)->loc_vol->outval;
                        break;
                    case El_SlideNote:
    					val = ((SlideNote*)el)->loc_vol->val;
    					//outval = ((SlideNote*)el)->loc_vol->outval;
                        break;
                    case El_Pattern:
                        val = (((Pattern*)el)->loc_vol->val);
                        //outval = (((Pattern*)el)->loc_vol->outval);
                        break;
                }

                vol_height = (int)(((float)h/VolRange)*val);
                //vol_height1 = (int)(((float)(h)/(VolRange))*outval);
                switch (el->type)
                {
                    case El_Samplent:
                        g.setColour(Colour(0xff12DF58));
                        J_VLine(g, cx, ly2 - vol_height, ly2 + 1);
                        //g.setColour(Colour(0xff00FF00));
                        //J_FilledRect(g, cx, ly2 - vol_height1, cx, ly2);

                        g.setColour(Colour(0xffFFFFFF));
                        J_HLine(g, ly2 - vol_height, cx, cx + 3);
                        //J_FilledRect(g, cx, ly2 - vol_height1, cx + 3, ly2 - vol_height1);
                        break;
                    case El_Gennote:
                        g.setColour(Colour(0xff3F3FFF));
                        J_VLine(g, cx, ly2 - vol_height, ly2 + 1);

                        g.setColour(Colour(0xffFFFFFF));
                        J_HLine(g, ly2 - vol_height, cx, cx + 3);
                        break;
                    case El_SlideNote:
                        cx1 = Tick2X(el->end_tick, loc);
                        g.setColour(Colour(0xff12FFC8));
                        J_VLine(g, cx, ly2 - vol_height, ly2 + 1);

                        g.setColour(Colour(0xffFFFFFF));
                        J_HLine(g, ly2 - vol_height, cx, cx1);
                        break;
                    case El_Pattern:
                        cx1 = el->EndTickX();
                        g.setColour(Colour(0xffAFAFAF));
                        J_VLine(g, cx, ly2 - vol_height, ly2 + 1);

                        g.setColour(Colour(0xffFFFFFF));
                        J_HLine(g, ly2 - vol_height, cx, cx + 3);
                        break;

                    default:
                        break;
                }
            }
        }

        if(loc == Loc_SmallGrid)
        {
            el = el->patt_next;
        }
        else
        {
            el = el->next;
        }
    }

    //////////////////////////////////////////
    // Draw selected elements, if any
    if(loc == Loc_SmallGrid)
    {
        el = gAux->workPt->OrigPt->first_elem;
    }
    else
    {
        el = firstElem;
    }

    while(el != NULL)
    {
        if(el->selected)
        {
            if(loc == Loc_SmallGrid)
            {
                auxed = GridXS1 + el->auxauxed;
            }
            else if(loc == Loc_MainGrid)
            {
                auxed = GridX1 + el->gridauxed;
            }
            else
            {
                auxed = StX1 + el->gridauxed;
            }

            if(el->IsPresent() &&
               (pt == NULL || el->patt == pt)&& 
               (trk == NULL || el->field_trkdata == actual_trk)&&
               (auxed != 0xFFFF && auxed >= lx1 && auxed <= lx2))
            {
                if(loc == Loc_SmallGrid)
                {
                    cx = el->s_StartTickX();
                }
                else if(loc == Loc_StaticGrid)
                {
                    cx = el->st_StartTickX();
                }
                else
                {
                    cx = el->StartTickX();
                }

                switch(el->type)
                {
                    case El_Samplent:
                        val = ((Samplent*)el)->loc_vol->val;
                        //outval = ((Samplent*)el)->loc_vol->outval;
                        break;
                    case El_Gennote:
                        val = ((Gennote*)el)->loc_vol->val;
                        //outval = ((Gennote*)el)->loc_vol->outval;
                        break;
                    case El_SlideNote:
    					val = ((SlideNote*)el)->loc_vol->val;
    					//outval = ((SlideNote*)el)->loc_vol->outval;
                        break;
                    case El_Pattern:
                        val = (((Pattern*)el)->loc_vol->val);
                        //outval = (((Pattern*)el)->loc_vol->outval);
                        break;
                }

                vol_height = (int)(((float)h/VolRange)*val);
                //vol_height1 = (int)(((float)(h)/(VolRange))*outval);
                switch (el->type)
                {
                    case El_Samplent:
                        g.setColour(Colour(0xffFF0000));
                        J_VLine(g, cx, ly2 - vol_height, ly2 + 1);

                        g.setColour(Colour(0xffFFFFFF));
                        J_HLine(g, ly2 - vol_height, cx, cx + 3);
                        //J_FilledRect(g, cx, ly2 - vol_height1, cx + 3, ly2 - vol_height1);
                        break;
                    case El_Gennote:
                        g.setColour(Colour(0xffFF0000));
                        J_VLine(g, cx, ly2 - vol_height, ly2 + 1);

                        g.setColour(Colour(0xffFFFFFF));
                        J_HLine(g, ly2 - vol_height, cx, cx + 3);
                        break;
                    case El_SlideNote:
                        cx1 = Tick2X(el->end_tick, loc);
                        g.setColour(Colour(0xffFF0000));
                        J_VLine(g, cx, ly2 - vol_height, ly2 + 1);

                        g.setColour(Colour(0xffFFFFFF));
                        J_HLine(g, ly2 - vol_height, cx, cx1);
                        break;
                    case El_Pattern:
                        cx1 = el->EndTickX();
                        g.setColour(Colour(0xffFF0000));
                        J_VLine(g, cx, ly2 - vol_height, ly2 + 1);

                        g.setColour(Colour(0xffFFFFFF));
                        J_HLine(g, ly2 - vol_height, cx, cx + 3);
                        break;

                    default:
                        break;
                }
            }
        }

        if(loc == Loc_SmallGrid)
        {
            el = el->patt_next;
        }
        else
        {
            el = el->next;
        }
    }
}

void J_PanLane(Graphics& g, Loc loc, Trk* trk, Pattern* pt, int lx1, int ly1, int lx2, int ly2)
{
    int pan_height, cx, cx1;
	float val;
    int auxed;
    int h = ly2 - ly1;
	int halfaux = (int)((float)h/2);

    Trk* actual_trk;
    if(trk != NULL && trk->trkbunch != NULL)
    {
        actual_trk = trk->trkbunch->trk_start;
    }
    else
    {
        actual_trk = trk;
    }

    if(trk != NULL)
    {
        trk->pan_lane.y = ly2;
    }
    else if(pt != NULL)
    {
        pt->pan_lane_main.y = ly2;
    }

    g.setColour(Colour(0xff444E80));
    J_HLine(g, ly2 - halfaux, lx1, lx2);

    //////////////////////////////////////////
    // Draw nonselected elements, if any
    Element* el;
    if(loc == Loc_SmallGrid)
    {
        el = gAux->workPt->OrigPt->first_elem;
    }
    else
    {
        el = firstElem;
    }

    while(el != NULL)
    {
        if(!el->selected)
        {
            if(loc == Loc_SmallGrid)
            {
                auxed = GridXS1 + el->auxauxed;
            }
            else if(loc == Loc_MainGrid)
            {
                auxed = GridX1 + el->gridauxed;
            }
            else
            {
                auxed = StX1 + el->gridauxed;
            }

            if(el->IsPresent() &&
               (pt == NULL || el->patt == pt)&& 
               (trk == NULL || el->field_trkdata == actual_trk)&&
               (auxed != 0xFFFF && auxed >= lx1 && auxed <= lx2))
            {
                if(loc == Loc_SmallGrid)
                {
                    cx = el->s_StartTickX();
                }
                else if(loc == Loc_StaticGrid)
                {
                    cx = el->st_StartTickX();
                }
                else
                {
                    cx = el->StartTickX();
                }

                switch(el->type)
                {
                    case El_Samplent:
                        val = ((((Samplent*)el)->loc_pan->val)*50 + 50);

                        break;
                    case El_Gennote:
                        val = ((((Gennote*)el)->loc_pan->val)*50 + 50);
                        break;
                    case El_SlideNote:
                        val = ((((SlideNote*)el)->loc_pan->val)*50 + 50);
                        break;
                    case El_Pattern:
                        val = ((((Pattern*)el)->loc_pan->val)*50 + 50);
                        break;
                }

                pan_height = RoundFloat1(((float)h/100)*val);
                switch (el->type)
                {
                    case El_Samplent:
                        g.setColour(Colour(0xff12DF58));
                        if(halfaux >= pan_height)
                        {
                            J_VLine(g, cx, ly2 - halfaux, ly2 - pan_height);
                        }
                        else
                        {
                            J_VLine(g, cx, ly2 - pan_height, ly2 - halfaux);
                        }

                        g.setColour(Colour(0xffFFFFFF));
                        J_HLine(g, ly2 - pan_height, cx, cx + 3);
                        break;
                    case El_Gennote:
                        g.setColour(Colour(0xff3F3FFF));
                        if(halfaux >= pan_height)
                        {
                            J_VLine(g, cx, ly2 - halfaux, ly2 - pan_height);
                        }
                        else
                        {
                            J_VLine(g, cx, ly2 - pan_height, ly2 - halfaux);
                        }

                        g.setColour(Colour(0xffFFFFFF));
                        J_HLine(g, ly2 - pan_height, cx, cx + 3);
                        break;
                    case El_SlideNote:
                        g.setColour(Colour(0xff12FFC8));
                        if(halfaux >= pan_height)
                        {
                            J_VLine(g, cx, ly2 - halfaux, ly2 - pan_height);
                        }
                        else
                        {
                            J_VLine(g, cx, ly2 - pan_height, ly2 - halfaux);
                        }

                        g.setColour(Colour(0xffFFFFFF));
                        cx1 = Tick2X(el->end_tick, loc);
                        J_HLine(g, ly2 - pan_height, cx, cx1);
                        break;
                    case El_Pattern:
                        g.setColour(Colour(0xffAFAFAF));
                        if(halfaux >= pan_height)
                        {
                            J_VLine(g, cx, ly2 - halfaux, ly2 - pan_height);
                        }
                        else
                        {
                            J_VLine(g, cx, ly2 - pan_height, ly2 - halfaux);
                        }

                        g.setColour(Colour(0xffFFFFFF));
                        J_HLine(g, ly2 - pan_height, cx, cx + 3);
                        break;
                    default:
                        break;
                }
            }
        }

        if(loc == Loc_SmallGrid)
        {
            el = el->patt_next;
        }
        else
        {
            el = el->next;
        }
    }


    //////////////////////////////////////////
    // Draw selected elements, if any
    if(loc == Loc_SmallGrid)
    {
        el = gAux->workPt->OrigPt->first_elem;
    }
    else
    {
        el = firstElem;
    }

    while(el != NULL)
    {
        if(el->selected)
        {
            if(loc == Loc_SmallGrid)
            {
                auxed = GridXS1 + el->auxauxed;
            }
            else if(loc == Loc_MainGrid)
            {
                auxed = GridX1 + el->gridauxed;
            }
            else
            {
                auxed = StX1 + el->gridauxed;
            }

            if(el->IsPresent() &&
               (pt == NULL || el->patt == pt)&& 
               (trk == NULL || el->field_trkdata == actual_trk)&&
               (auxed != 0xFFFF && auxed >= lx1 && auxed <= lx2))
            {
                if(loc == Loc_SmallGrid)
                {
                    cx = el->s_StartTickX();
                }
                else if(loc == Loc_StaticGrid)
                {
                    cx = el->st_StartTickX();
                }
                else
                {
                    cx = el->StartTickX();
                }

                switch(el->type)
                {
                    case El_Samplent:
                        val = ((((Samplent*)el)->loc_pan->val)*50 + 50);

                        break;
                    case El_Gennote:
                        val = ((((Gennote*)el)->loc_pan->val)*50 + 50);
                        break;
                    case El_SlideNote:
                        val = ((((SlideNote*)el)->loc_pan->val)*50 + 50);
                        break;
                    case El_Pattern:
                        val = ((((Pattern*)el)->loc_pan->val)*50 + 50);
                        break;
                }

                pan_height = RoundFloat1(((float)h/100)*val);
                switch (el->type)
                {
                    case El_Samplent:
                        g.setColour(Colour(0xffFF0000));
                        if(halfaux >= pan_height)
                        {
                            J_VLine(g, cx, ly2 - halfaux, ly2 - pan_height);
                        }
                        else
                        {
                            J_VLine(g, cx, ly2 - pan_height, ly2 - halfaux);
                        }

                        g.setColour(Colour(0xffFFFFFF));
                        J_HLine(g, ly2 - pan_height, cx, cx + 3);
                        break;
                    case El_Gennote:
                        g.setColour(Colour(0xffFF0000));
                        if(halfaux >= pan_height)
                        {
                            J_VLine(g, cx, ly2 - halfaux, ly2 - pan_height);
                        }
                        else
                        {
                            J_VLine(g, cx, ly2 - pan_height, ly2 - halfaux);
                        }

                        g.setColour(Colour(0xffFFFFFF));
                        J_HLine(g, ly2 - pan_height, cx, cx + 3);
                        break;
                    case El_SlideNote:
                        g.setColour(Colour(0xffFF0000));
                        if(halfaux >= pan_height)
                        {
                            J_VLine(g, cx, ly2 - halfaux, ly2 - pan_height);
                        }
                        else
                        {
                            J_VLine(g, cx, ly2 - pan_height, ly2 - halfaux);
                        }

                        g.setColour(Colour(0xffFFFFFF));
                        cx1 = Tick2X(el->end_tick, loc);
                        J_HLine(g, ly2 - pan_height, cx, cx1);
                        break;
                    case El_Pattern:
                        g.setColour(Colour(0xffFF0000));
                        if(halfaux >= pan_height)
                        {
                            J_VLine(g, cx, ly2 - halfaux, ly2 - pan_height);
                        }
                        else
                        {
                            J_VLine(g, cx, ly2 - pan_height, ly2 - halfaux);
                        }

                        g.setColour(Colour(0xffFFFFFF));
                        J_HLine(g, ly2 - pan_height, cx, cx + 3);
                        break;
                    default:
                        break;
                }
            }
        }

        if(loc == Loc_SmallGrid)
        {
            el = el->patt_next;
        }
        else
        {
            el = el->next;
        }
    }
}

void J_AuxLanes(Graphics& g)
{
    Pattern* pt = gAux->workPt;
    Trk* trk;
    int ylane;
    int hgt;

    if(pt->ptype != Patt_Pianoroll)
    {
        for(int tnum = 0; tnum < 90; tnum ++)
        {
            trk = GetTrkDataForTrkNum(tnum, pt->OrigPt);
			if(trk != NULL)
			{
				if(trk->vol_lane.visible == true)
				{
					ylane = Line2Y(trk->trk_end_line + trk->vol_lane.height, Loc_SmallGrid) - 1 - gAux->bottomincr;
					hgt = trk->vol_lane.height*gAux->lineHeight - 1;

					g.setColour(Colour(0xff000000));
					J_FillRect(g, GridXS1, ylane - hgt + 1, GridXS2, ylane);

					J_VolLane(g, Loc_SmallGrid, trk, NULL, GridXS1, ylane - hgt + 1, GridXS2, ylane);
				}
				if(trk->pan_lane.visible == true)
				{
					ylane = Line2Y(trk->end_line, Loc_SmallGrid) - 1 - gAux->bottomincr;
					hgt = trk->pan_lane.height*gAux->lineHeight - 1;

					g.setColour(Colour(0xff000000));
					J_FillRect(g, GridXS1, ylane - hgt + 1, GridXS2, ylane);

					J_PanLane(g, Loc_SmallGrid, trk, NULL, GridXS1, ylane - hgt + 1, GridXS2, ylane);
				}
			}
        }
    }
}

void J_GridLanes(Graphics& g)
{
    int trknum;
    Trk* trk;
    Trk* trk1;

    int ylane;
    int hgt;
    int yc = 0;
    int line = OffsLine;
    int ytt = 0;
    while(1)
    {
        g.setColour(Colour(0xff003264));
        trknum = GetTrkNumForLine(line, field);
        trk = GetTrkDataForTrkNum(trknum, field);
        trk1 = GetTrkDataForTrkNum(trknum, st->patt);

        if(trknum == 0xFFFF)
        {
            break;
        }

        yc = ytt + GridY1;
        if(line == trk->start_line)
        {
            if(trk->vol_lane.visible == true)
            {
                ylane = Line2Y(trk->trk_end_line + trk->vol_lane.height, Loc_MainGrid) - 1;
                hgt = trk->vol_lane.height*lineHeight - 1;
                g.setColour(Colour(0xff000000));
                J_FillRect(g, GridX1, ylane - hgt + 1, GridX2, ylane);
                J_VolLane(g, Loc_MainGrid, trk, NULL, GridX1, ylane - hgt + 1, GridX2, ylane);
                //J_VolLane(g, st->patt, trk1, StX1, Line2Y(trk->trk_end_line + trk->vol_lane.height) - 1, StX2 - StX1, trk->vol_lane.height*lineHeight - 1, false);
            }

            if(trk->pan_lane.visible == true)
            {
                ylane = Line2Y(trk->end_line, Loc_MainGrid) - 1;
                hgt = trk->pan_lane.height*lineHeight - 1;
                g.setColour(Colour(0xff000000));
                J_FillRect(g, GridX1, ylane - hgt + 1, GridX2, ylane);
                J_PanLane(g, Loc_MainGrid, trk, NULL, GridX1, ylane - hgt + 1, GridX2, ylane);
                //J_PanLane(g, st->patt, trk1, StX1, Line2Y(trk->end_line) - 1, StX2 - StX1, trk->pan_lane.height*lineHeight - 1, false);
            }
        }

		if(yc > GridY2)
        {      
			break;
        }
        line++;
        ytt += lineHeight;
    }
}

void J_Content_Main(Graphics& g)
{
    g.saveState();
    g.reduceClipRegion(GridX1, GridY1 + 1, GridX2 - GridX1, GridY2 - GridY1);

    J_GridLanes(g);

    Trigger* tg;
    Element* el;
    Event* ev;

    if(!PatternsOverlapping)
    {
        g.saveState();
        ev = field->last_ev;
        while(ev != NULL)
        {
            tg = ev->tg_last;
            while(tg != NULL)
            {
                el = tg->el;
                if(el->IsPresent() && el->visible && el->type == El_Pattern && tg->activator && 
                   el->patt == field && !((Pattern*)el)->OrigPt->autopatt)
                {
                    J_Pattern(g, (Pattern*)el, Loc_MainGrid);
                    J_HighlightElement(g, el);

                    g.excludeClipRegion(el->abs_area.x1, el->abs_area.y1, 
                                       el->abs_area.x2 - el->abs_area.x1 + 1, 
                                       el->abs_area.y2 - el->abs_area.y1 + 1);
                }
                tg = tg->ev_prev;
            }
            ev = ev->prev;
        }
        g.restoreState();
    }
    else
    {
        ev = field->first_ev;
        while(ev != NULL)
        {
            tg = ev->tg_first;
            while(tg != NULL)
            {
                el = tg->el;
                if(el->IsPresent() && el->visible && el->type == El_Pattern && tg->activator && 
                   el->patt == field && !((Pattern*)el)->OrigPt->autopatt)
                {
                    J_Pattern(g, (Pattern*)el, Loc_MainGrid);
                    J_HighlightElement(g, el);
                }
                tg = tg->ev_next;
            }
            ev = ev->next;
        }
    }

    g.saveState();
    bool exclude;
    ev = field->last_ev;
    while(ev != NULL)
    {
        tg = ev->tg_first;
        while(tg != NULL)
        {
            el = tg->el;
            if(el->IsPresent() && tg->activator && el->patt == field && el->visible)
            {
                exclude = false;
                switch(el->type)
                {
                    case El_Samplent:
                    {
                        J_Samplent(g, (Samplent*)el, Loc_MainGrid);
                        J_HighlightElement(g, el);
                        exclude = true;
                    }break;
                    case El_Gennote:
                    {
                        J_Gennote(g, (Gennote*)el, Loc_MainGrid);
                        J_HighlightElement(g, el);
                        exclude = true;
                    }break;
                }

                if(exclude)
                {
                    g.excludeClipRegion(el->abs_area.x1, el->abs_area.y1, 
                                       el->abs_area.x2 - el->abs_area.x1 + 1, 
                                       el->abs_area.y2 - el->abs_area.y1 + 1);
                }
            }
            tg = tg->ev_next;
        }
        ev = ev->prev;
    }
    g.restoreState();

    ev = field->first_ev;
    while(ev != NULL)
    {
        tg = ev->tg_first;
        while(tg != NULL)
        {
            if(tg->el->IsPresent() && tg->activator == true && tg->patt == field && tg->el->visible == true)
            {
                switch (tg->el->type)
                {
                    case El_Command:
                        J_Command(g, (Command*)tg->el, Loc_MainGrid);
                        J_HighlightElement(g, tg->el);
                        break;
                    case El_Mute:
                        J_Muter(g, (Muter*)tg->el, Loc_MainGrid);
                        J_HighlightElement(g, tg->el);
                        break;
                    case El_Break:
                        J_Break(g, (Break*)tg->el, Loc_MainGrid);
                        J_HighlightElement(g, tg->el);
                        break;
                    case El_Slider:
                        J_Slide(g, (Slide*)tg->el, Loc_MainGrid);
                        J_HighlightElement(g, tg->el);
                        break;
                    case El_Reverse:
                        J_Reverse(g, (Reverse*)tg->el, Loc_MainGrid);
                        J_HighlightElement(g, tg->el);
                        break;
                    case El_Repeat:
                        J_Repeat(g, (Repeat*)tg->el, Loc_MainGrid);
                        J_HighlightElement(g, tg->el);
                        break;
                    case El_Vibrate:
                        J_Vibrate(g, (Vibrate*)tg->el, Loc_MainGrid);
                        J_HighlightElement(g, tg->el);
                        break;
                    case El_Transpose:
                        J_Transpose(g, (Transpose*)tg->el, Loc_MainGrid);
                        J_HighlightElement(g, tg->el);
                        break;
                    case El_SlideNote:
                    {
						SlideNote* sl = (SlideNote*)tg->el;
                        if(sl->parent == NULL)
                        {
                            J_SlideNote(g, sl, Loc_MainGrid, false);
                            J_HighlightElement(g, tg->el);
                        }
                    }break;
                    case El_TextString:
                    {
                        J_Txt(g, (Txt*)tg->el, Loc_MainGrid);
                        J_HighlightElement(g, tg->el);
                        exclude = true;
                    }break;
                    default:
                        break;
                }
            }
            tg = tg->ev_next;
        }
        ev = ev->next;
    }

    // Marker where a pattern will be created per edit in aux area
    if(C.loc == Loc_SmallGrid && gAux->isBlank())
    {
        int x = Tick2X(C.fieldposx, Loc_MainGrid);
        int y = Line2Y(C.fieldposy, Loc_MainGrid);
    
        g.setColour(Colour(0xffFF05FF));
        J_VLine(g, x, y - lineHeight, y - 1);
    
        g.setColour(Colour(0xffFF05FF));
        J_HLine(g, y - 1, x, x + 15);
        // J_Line(g, x, y - lineHeight, x + 15, y - lineHeight);
    }

    g.restoreState();
}

void J_Auxaux(Graphics& g)
{
    if(gAux->eux > 0)
    {
        g.saveState();
        g.reduceClipRegion(GridXS1, GridYS2 + 2, GridXS2 - GridXS1, WindHeight - GridYS2 - 5);
        g.setColour(Colour(0xff1F2F3F));
        //g.setColour(panelcolour.darker(1).darker(0.25f));
        g.fillRect(GridXS1, GridYS2 + 2, GridXS2 - GridXS1, WindHeight - GridYS2 - 5);

        Colour col1 = Colour(0xbf525265);
        Colour col2 = Colour(0xbf35354A);
        float xo = (gAux->OffsTick - (int)gAux->OffsTick)*gAux->tickWidth;
        int xt = (int)gAux->OffsTick;
        float xtt = 0;
        int xc;
        while(1)
        {
            if(xt%(beats_per_bar*ticks_per_beat) == 0)
            {
                g.setColour(col1);

                xc = RoundFloat(xtt - xo + GridXS1);
                if(xc > GridXS2)
                {
                    break;
                }

                //J_Line(g, xc, py1, xc, py2);
                J_VLine(g, xc, GridYS2 + 2, WindHeight - 3);
            }
            else if(xt%ticks_per_beat == 0)
            {
                g.setColour(col2);

                xc = RoundFloat(xtt - xo + GridXS1);
                if(xc > GridXS2)
                {
                    break;
                }

                //J_Line(g, xc, py1, xc, py2);
                J_VLine(g, xc, GridYS2 + 2, WindHeight - 3);
            }

            xt++;
            xtt += gAux->tickWidth;
        }

        if(gAux->ltype == Lane_Vol)
        {
            J_VolLane(g, Loc_SmallGrid, NULL, NULL, GridXS1, GridYS2 + 2, GridXS2, WindHeight - 4);
        }
        else if(gAux->ltype == Lane_Pan)
        {
            J_PanLane(g, Loc_SmallGrid, NULL, NULL, GridXS1, GridYS2 + 2, GridXS2, WindHeight - 4);
        }
        else if(gAux->ltype == Lane_Pitch)
        {
            if(gAux->workPt->OrigPt->pitch != NULL && GridXS2 > GridXS1)
            {
                Envelope* env = (Envelope*)gAux->workPt->OrigPt->pitch->paramedit;
                J_EnvLane(g, env, Loc_SmallGrid, NULL, NULL, GridXS1, GridYS2 + 3, GridXS2, WindHeight - 4);
            }
        }
        g.restoreState();
    }
}

void J_Content_Aux(Graphics& g)
{
    if(GridYS2 > GridYS1 + 1)
    {
        g.saveState();
        g.reduceClipRegion(GridXS1, GridYS1, GridXS2 - GridXS1, GridYS2 - GridYS1);

        J_AuxLanes(g);

        Pattern* pt = gAux->workPt;
        Trigger* tg;
        Event* ev;

        // First draw notes
        if(pt->autopatt == false)
        {
            ev = field->first_ev;
        }
        else
        {
            ev = pt->first_ev;
        }

        while(ev != NULL)
        {
            tg = ev->tg_first;
            while(tg != NULL)
            {
                if(tg->activator == true && 
                   tg->patt == pt && 
                   tg->el->auxvisible == true && 
                   tg->el->IsPresent() &&
                   tg->el->IsDisplayableOnPatternType(tg->patt->ptype))
                {
                    switch (tg->el->type)
                    {
                        case El_TextString:
                            J_Txt(g, (Txt*)tg->el, Loc_SmallGrid);
                            J_HighlightElement(g, tg->el);
                            break;
                        case El_Samplent:
                            if(pt->ptype != Patt_StepSeq)
                            {
                                J_Samplent(g, (Samplent*)tg->el, Loc_SmallGrid);
                                J_HighlightElement(g, tg->el);
                            }
                            else
                            {
                                J_StepSeqEl(g, (Instance*)tg->el);
                                J_HighlightSelectedElement(g, tg->el);
                            }
                            break;
                        case El_Gennote:
                            if(pt->ptype != Patt_StepSeq)
                            {
                                J_Gennote(g, (Gennote*)tg->el, Loc_SmallGrid);
                                J_HighlightElement(g, tg->el);
                            }
                            else
                            {
                                J_StepSeqEl(g, (Instance*)tg->el);
                                J_HighlightSelectedElement(g, tg->el);
                            }
                            break;
                        case El_SlideNote:
                        {
                            SlideNote* sl = (SlideNote*)tg->el;
                            if(sl->parent == NULL)
                            {
                                J_SlideNote(g, sl, Loc_SmallGrid, false);
                                J_HighlightElement(g, tg->el);
                            }
                        }break;
                        default:
                            break;
                    }
                }
                tg = tg->ev_next;
            }
            ev = ev->next;
        }

        // Second draw effects
        if(pt->autopatt == false)
        {
            ev = field->first_ev;
        }
        else
        {
            ev = pt->first_ev;
        }

        while(ev != NULL)
        {
            tg = ev->tg_first;
            while(tg != NULL)
            {
                if(tg->activator == true && 
                   tg->patt == pt && 
                   tg->el->auxvisible == true && 
                   tg->el->IsPresent() &&
                   tg->el->IsDisplayableOnPatternType(tg->patt->ptype))
                {
                    switch (tg->el->type)
                    {
                        case El_Command:
                            J_Command(g, (Command*)tg->el, Loc_SmallGrid);
                            J_HighlightElement(g, tg->el);
                            break;
                        case El_Mute:
                            J_Muter(g, (Muter*)tg->el, Loc_SmallGrid);
                            J_HighlightElement(g, tg->el);
                            break;
                        case El_Break:
                            J_Break(g, (Break*)tg->el, Loc_SmallGrid);
                            J_HighlightElement(g, tg->el);
                            break;
                        case El_Slider:
                            J_Slide(g, (Slide*)tg->el, Loc_SmallGrid);
                            J_HighlightElement(g, tg->el);
                            break;
                        case El_Reverse:
                            J_Reverse(g, (Reverse*)tg->el, Loc_SmallGrid);
                            J_HighlightElement(g, tg->el);
                            break;
                        case El_Repeat:
                            J_Repeat(g, (Repeat*)tg->el, Loc_SmallGrid);
                            J_HighlightElement(g, tg->el);
                            break;
                        case El_Vibrate:
                            J_Vibrate(g, (Vibrate*)tg->el, Loc_SmallGrid);
                            J_HighlightElement(g, tg->el);
                            break;
                        case El_Transpose:
                            J_Transpose(g, (Transpose*)tg->el, Loc_SmallGrid);
                            J_HighlightElement(g, tg->el);
                            break;
                        default:
                            break;
                    }
                }
                tg = tg->ev_next;
            }
            ev = ev->next;
        }

        g.restoreState();
    }
}

void J_Pos(Graphics& g, int pos, int gx, int gy)
{
    g.setColour(Colour(0xffFF7800));

    //J_LineRect(g, gx + pos - 3, gy, gx + pos + 3, gy + LinerHeight);
    J_HLine(g, gy + 3, gx + pos - 3, gx + pos + 4);
    J_HLine(g, gy + 4,  gx + pos - 2, gx + pos + 3);
    J_HLine(g, gy + 5, gx + pos - 1, gx + pos + 2);
    J_HLine(g, gy + 2, gx + pos, gx + pos + 1);
    J_FillRect(g, gx + pos - 3, gy, gx + pos + 3, gy + 3);

    g.setColour(Colour(0xff8F3F00));
    J_FillRect(g, gx + pos - 2, gy + 1, gx + pos + 2, gy + 3);
    J_HLine(g, gy + 4, gx + pos - 1, gx + pos + 2);
    J_VLine(g, gx + pos, gy + 4, gy + 6);

    //g.setColour(Colour(0x8fFF7800));
    //J_FillRect(g, gx + pos - 2, gy + 1, gx + pos + 2, gy + LinerHeight - 1);

    if(pos >= 0 && pos <= (GridX2 - GridX1))
    {
        g.setColour(Colour(0xffFF7800));
        J_VLine(g, gx + pos, gy + 6, gy + LinerHeight + GridY2 - GridY1 + 3);
    }

    /*
    int xoffs = MainX1 - 12;
    int xt = (int)(pbMain->currTick/main_bar->full_len*(main_bar->pixlen - 2) + 37 + xoffs);
    g.setColour(Colour(0xffFF7800));
    J_VLine(g, xt, MainY1 - 4 - NavHeight + 8, MainY1 - 3);
    */
}

void J_PosAux(Graphics& g, int pos, int gx, int gy)
{
    if(pos >= 0 && pos <= (GridXS2 - GridXS1))
    {
        g.setColour(Colour(0xffFF7800));
        J_VLine(g, gx + pos, GridYS1 - 12, GridYS2);
    }
}

void J_MainScale(Graphics& g)
{
    g.saveState();
    g.reduceClipRegion(GridX1, MainY1, GridX2 - GridX1, LinerHeight + 1);

    g.setColour(Colour(35, 55, 65));
    g.fillRect(GridX1, MainY1, GridX2 - GridX1, LinerHeight + 1);

    //g.setColour(Colour(160, 170, 160));
    g.setColour(Colour(140, 165, 170));

    int ticklen = (beats_per_bar*ticks_per_beat);
    float pixlen = tickWidth*ticklen;

    int step = 1;
    while(pixlen*step < 19)
        step++;
    int number = int(OffsTick/ticklen);
    int stepcnt = (step - number%step)%step;

    float xoffs = ((OffsTick/ticklen - int(OffsTick)/ticklen)*pixlen);
    float xcoord = GridX1 - xoffs;
    g.setFont(*ti);
    while(1)
    {
        if(xcoord > GridX2)
        {
            break;
        }

        if(stepcnt == 0)
        {
            g.drawSingleLineText(String::formatted(T("%3d"), number), RoundFloat(xcoord) - 7, GridY1 - 1);
            stepcnt = step;
        }
        stepcnt--;

        number += 1;
        xcoord += pixlen;
    }
    g.restoreState();
}

void J_VPianoKey(Graphics& g, int x, int y, int height, int key)
{
    bool black = false;
    bool raise = false;
    bool lower = false;

    int pkey = key%12;
    bool pressed = false;
    if(M.pianokey_pressed == true && M.pianokey_num == key)
    {
        pressed = true;
    }

    if(pkey == 1 || pkey == 3 || pkey == 6 || pkey == 8 || pkey == 10)
    {
        black = true;
    }
    else
    {
        if(pkey == 0 || pkey == 2 || pkey == 5 || pkey == 7 || pkey == 9)
        {
            raise = true;
        }

        if(pkey == 2 || pkey == 7 || pkey == 9 || pkey == 4 || pkey == 11)
        {
            lower = true;
        }
    }

    if(black == true)
    {
        g.setColour(Colour(0xff000000));
        if(pressed)
        {
            g.setColour(Colour(0xffC8C800));
        }
        J_FillRect(g, x, y, x + keys_width, (int)(y + height/1.5f));
        //J_Line(x + keys_width*0.5f, y + height/1.5f, x + keys_width*0.5f, y + height);
    }
    else
    {
        g.setColour(Colour(0xffFFFFFF));
        if(pressed)
        {
            g.setColour(Colour(0xffC8C800));
        }
        J_FillRect(g, x, y, x + keys_width, y + height);

        g.setColour(Colour(0xff000000));
        if(pressed)
        {
            g.setColour(Colour(0xffC8C800));
        }
        if(pkey == 0 || pkey == 5)
        {
            J_Line(g, x, y, x, y + height);
        }
    }

    if(raise == true)
    {
        g.setColour(Colour(0xffFFFFFF));
        if(pressed)
        {
            g.setColour(Colour(0xffC8C800));
        }
        J_FillRect(g, x + keys_width, (int)(y + height/1.5f), x + keys_width + keys_width/2, y + height);
    }
    if(lower == true)
    {
        g.setColour(Colour(0xffFFFFFF));
        if(pressed)
        {
            g.setColour(Colour(0xffC8C800));
        }
        J_FillRect(g, x, (int)(y + height/1.5f), x - keys_width/2, y + height);

        g.setColour(Colour(0xff000000));
        if(pressed)
        {
            g.setColour(Colour(0xffC8C800));
        }
        J_Line(g, x - keys_width/2, y, x - keys_width/2, y + height);
    }
}

void J_StepVU1(Graphics& g, int x, int y, VU* vu)
{
    vu->x = x;
    vu->y = y;

    float vval = (vu->getL() + vu->getR())/2;
    if(vu->instr->type == Instr_Sample)
    {
        g.setColour(Colour(0xff012121));
    }
    else
    {
        g.setColour(Colour(0xff010121));
    }

    J_FillRect(g, x, y, x + 8, y + 7);

    Colour col;
    if(vu->instr->type == Instr_Sample)
    {
        col = Colour(57, 130, 130);
    }
    else
    {
        col = Colour(54, 121, 158);
    }
    g.setColour(col);
    J_LineRect(g, x, y, x + 8, y + 7);
    g.setColour(col.withAlpha(0.4f));
    J_LineRect(g, x + 1, y + 1, x + 7, y + 6);

    col = Colour(0xffFFAF4F);
    g.setColour(col.withAlpha(jlimit(0.f, 1.f, vval)));
    J_FillRect(g, x + 2, y + 2, x + 6, y + 5);
}

void J_StepVU(Graphics& g, int x, int y, VU* vu)
{
    vu->x = x;
    vu->y = y;

    float vval = (vu->getL() + vu->getR())/2;
    /*
    if(vu->instr->type == Instr_Sample)
    {
        g.setColour(Colour(0xff416161));
    }
    else
    {
        g.setColour(Colour(0xff414161));
    }
    */
    g.setColour(Colour(0, 0, 0));
    J_VLine(g, x + 5, y, y + 4);
    J_VLine(g, x + 3, y, y + 4);
    J_VLine(g, x + 1, y, y + 4);

    if(vu->instr->type == Instr_Sample)
    {
        g.setColour(Colour(26, 53, 56));
    }
    else
    {
        g.setColour(Colour(0xff151A35));
    }
    //J_FilledRect(g, x + 5, y + 1, x + 5, y + 8);

    g.setColour(Colour(0xaf7FFF5F));
    if(vval > 0)
        J_VLine(g, x + 1, y, y + 4);
    if(vval > 0.2)
        J_VLine(g, x + 3, y, y + 4);
    if(vval > 0.4)
        J_VLine(g, x + 5, y, y + 4);
}

void J_MixTxt(Graphics& g, int xb, int yb)
{
    g.setColour(Colour(0, 0, 0));
    if(mixbrowse == true)
    {
        switch(mixBrw->brwmode)
        {
            case Browse_Params:
                J_TextInstr_xy(g, xb + 1, yb + 13, "FX Params");
                g.setColour(Colour(255, 255, 255));
                J_TextInstr_xy(g, xb, yb + 12, "FX Params");
                break;
            case Browse_Presets:
                J_TextInstr_xy(g, xb + 1, yb + 13, "FX Presets");
                g.setColour(Colour(255, 255, 255));
                J_TextInstr_xy(g, xb, yb + 12, "FX Presets");
                break;
            case Browse_Files:
                J_TextInstr_xy(g, xb + 1, yb + 13, "Files");
                g.setColour(Colour(255, 255, 255));
                J_TextInstr_xy(g, xb, yb + 12, "Files");
                break;
            case Browse_Projects:
                J_TextInstr_xy(g, xb + 1, yb + 13, "Projects");
                g.setColour(Colour(255, 255, 255));
                J_TextInstr_xy(g, xb, yb + 12, "Projects");
                break;
            case Browse_Plugins:
                J_TextInstr_xy(g, xb + 1, yb + 13, "FX Plugins");
                g.setColour(Colour(255, 255, 255));
                J_TextInstr_xy(g, xb, yb + 12, "FX Plugins");
                break;
            case Browse_Samples:
                J_TextInstr_xy(g, xb + 1, yb + 13, "Samples");
                g.setColour(Colour(255, 255, 255));
                J_TextInstr_xy(g, xb, yb + 12, "Samples");
                break;
        }
    }
    else
    {
        J_TextInstr_xy(g, xb - 21, yb + 13, "Tracks controls");
        g.setColour(Colour(255, 255, 255));
        J_TextInstr_xy(g, xb - 20, yb + 12, "Tracks controls");
    }
}

void J_Aux_ModeButtons(Graphics& g, int x, int y)
{
    //J_DarkLine(g, x, y, x + 72, y + 107);
    //J_DarkRect(g, x, y, x + 72, y + 112);
    J_Button(g, gAux->Vols, x + 2, y + 2, 71, 22);
    J_Button(g, gAux->Pans, x + 2, y + 5 + 12 + 8, 71, 22);

    g.setColour(panelcolour);
    J_FillRect(g, x, y + 5 + 12 + 9 + 21, x + 72, y + 5 + 12 + 6 + 24);

    J_PanelLine(g, AuxX2, y + 5 + 12 + 9 + 23, 176);

    J_Button(g, gAux->PtUsual, x + 2, y + 9 + 12 + 3 + 20 + 9, 71, 22);
    J_Button(g, gAux->PtPianorol, x + 2, y + 9 + 12 + 3 + 20 + 11 + 20 + 1, 71, 22);
    J_Button(g, gAux->PtStepseq, x + 2, y + 9 + 12 + 3 + 20 + 13 + 20 + 1 + 20 + 1, 71, 22);
    //J_Button(g, gAux->PtAuto, x + 2, y + 5 + 12 + 3 + 20 + 4 + 20 + 1 + 20 + 1 + 21, 69, 20);

    if(gAux->auxmode == AuxMode_Pattern && gAux->CurrPt == gAux->PtPianorol)
    {
        //J_DarkLine(g, x + 72, y + 5 + 12 + 3 + 20 + 4 + 19, x + 70 + 67, y + 5 + 12 + 3 + 20 + 4 + 19 + 23);
        //J_DarkRect(g, x + 72, y + 8 + 12 + 3 + 20 + 6 + 19, x + 70 + 67, y + 5 + 12 + 3 + 20 + 6 + 19 + 26);
        J_Button(g, gAux->PR8, x + 74, y + 9 + 12 + 3 + 20 + 13 + 20, 20, 20);
        J_Button(g, gAux->PR10, x + 75 + 20, y + 9 + 12 + 3 + 20 + 13 + 20, 20, 20);
        J_Button(g, gAux->PR12, x + 76 + 20 + 20, y + 9 + 12 + 3 + 20 + 13 + 20, 20, 20);
    }
    else
    {
        g.setColour(Colour(50, 114, 99));
        J_FillRect(g, x + 2 + 72, y + 5 + 12 + 3 + 20 + 4 + 20, 66, 22);
    }
}

void J_Aux_PattButtons(Graphics& g, int x, int y, int y1)
{
    g.setColour(Colour(0x15FFFFFF));
    J_LineRect(g, x, y, x + img_zoomback->getWidth() - 1, y + img_zoomback->getHeight() - 1);

    g.drawImageAt(img_zoomback, x, y, false);

    J_Button(g, gAux->DecrScale, x + 2, y + 2, 22, 19);
    J_Button(g, gAux->IncrScale, x + 5 + 22 + 70, y + 2, 22, 19);

    SliderZoom* sl = gAux->slzoom;
    J_SliderZoom(g, sl, x + 25, y + 1);
    sl->Activate();
    sl->drawarea->SetBounds(sl->x, sl->y, sl->width, sl->height);

    //J_DarkLine(g, x, y1, x + 72, y1 + 44);
    J_Button(g, gAux->Vols1, x + 2, y1 + 2, 71, 22);
    J_Button(g, gAux->Pans1, x + 2, y1 + 25, 71, 22);
    // Pitch postponed
    //J_Button(g, gAux->Pitch1, x + 2, y1 + 44, 69, 20);
}

void J_Aux_PianoKeys(Graphics& g)
{
    if(gAux->workPt->ptype == Patt_Pianoroll)
    {
        g.setColour(Colour(68, 143, 122));
        g.fillRect(keyX + 3, keyY, keyW - 3, keyH);

        Pattern* pt = gAux->workPt;

        int num_lines = (gAux->ptdrawy2 - gAux->ptdrawy1)/gAux->lineHeight;
        int vo = (gAux->ptdrawy2 - gAux->ptdrawy1)%gAux->lineHeight;
        int line;
        int key;
        int ty = -1;
        int proll_key;
        bool cutup;
        bool cutdown;
        Trk* trk;

        int yc = 0;
        int ytt = 0;
        int absline = 0;

        // Horizontal lines
        while(1)
        {
            yc = ytt + gAux->ptdrawy1;
            if(yc > gAux->ptdrawy2)
            {
                break;
            }

            if(pt->ptype == Patt_Pianoroll)
            {
                line = gAux->note_offset - absline;
            }
            else
            {
                line = gAux->OffsLine + absline;
            }
            trk = GetTrkDataForLine(line, pt->OrigPt);

            if(pt->ptype == Patt_Pianoroll)
            {
                cutup = false;
                cutdown = false;
                key = line;
                proll_key = key%12;
            
                if(absline == 0)
                {
                    J_HPianoKey(g, gAux->ptdrawx1 - gAux->keywidth, yc - gAux->lineHeight, gAux->keywidth - 1, proll_key + 1, gAux->lineHeight, true, false, false, false, IsPianoKeyPressed(key + 1));
                    J_HPianoKey(g, gAux->ptdrawx1 - gAux->keywidth, yc - gAux->lineHeight - gAux->lineHeight, gAux->keywidth - 1, proll_key + 2, gAux->lineHeight, true, false, false, false, IsPianoKeyPressed(key + 2));
                }
            
                if(absline == num_lines - 1)
                {
                    J_HPianoKey(g, gAux->ptdrawx1 - gAux->keywidth, yc + gAux->lineHeight, gAux->keywidth - 1, proll_key - 1, gAux->lineHeight, true, false, false, false, IsPianoKeyPressed(key - 1));
                    J_HPianoKey(g, gAux->ptdrawx1 - gAux->keywidth, yc + gAux->lineHeight + gAux->lineHeight, gAux->keywidth - 1, proll_key - 2, gAux->lineHeight, true, false, false, false, IsPianoKeyPressed(key - 2));
                }
            
                J_HPianoKey(g, gAux->ptdrawx1 - gAux->keywidth, yc, gAux->keywidth - 1, proll_key, gAux->lineHeight, true, cutup, cutdown, false, IsPianoKeyPressed(key));
                if(proll_key == 11)
                {
                    g.setColour(Colour(0xff000000));
                    J_note_string(g, gAux->ptdrawx1 - 16, yc - 2, key + 1, false);
                }
            
                if(proll_key == 11)    // line divider between octaves in piano-roll
                {
                    g.setColour(Colour(0xff46505A));
                }
                else
                {
                    g.setColour(Colour(0xff0A2D2D));
                }

                if(proll_key == 0)
                {
                    g.setColour(Colour(0xff000000));
                    J_note_string(g, gAux->ptdrawx1 - 16, yc + gAux->lineHeight - 2, key, false);
                }
            }

            absline++;
            ytt += gAux->lineHeight;
        }
    }
    else if(gAux->workPt->OrigPt->autopatt == false)
    {
        J_PanelRect(g, AuxX1, AuxY1 + 2, AuxX1 + gAux->keywidth + 2, WindHeight);
        J_LRect(g, AuxX1, AuxY1 + 2, AuxX1 + gAux->keywidth + 2, WindHeight);

        //J_LRectC(g, AuxX1 + 2, GridYS1, AuxX1 + gAux->keywidth + 2, GridYS2, Colour(0x5f000000));
    }
}

void J_RefreshAuxGridImage()
{
    Pattern* pt = gAux->workPt;

    int ih = gAux->lineHeight*10;
    int bw = RoundFloat((beats_per_bar*ticks_per_beat)*gAux->tickWidth);
    int iw = bw;
    while(iw < 200)
    {
        iw += bw;
    }

    if(img_auxgrid1 != NULL)
        delete img_auxgrid1;

    if(img_auxgrid2 != NULL)
        delete img_auxgrid2;

    img_auxgrid1 = new Image(Image::ARGB, iw, ih, true);
    Graphics imageContext1(*(img_auxgrid1));
    imageContext1.fillAll(Colours::black);

    img_auxgrid2 = new Image(Image::ARGB, iw, ih, true);
    Graphics imageContext2(*(img_auxgrid2));
    imageContext2.fillAll(Colours::black);

    int xtick = 0;
    float xpix = 0;
    int xcoord;

    Colour col1;
    Colour col2;
    Colour col3;

    Colour col11;
    Colour col21;
    Colour col31;

    if(pt->ptype == Patt_StepSeq)
    {
        col1 = Colour(0xff4545BF);
        col2 = Colour(0xff3A4A70);
        col3 = Colour(0xff202040);

        col11 = Colour(0xff303070);
        col21 = Colour(0xff2A2A4A);
        col31 = Colour(0xff20202A);
    }
    else if(pt->OrigPt && pt->OrigPt->autopatt)
    {
        col1 = Colour(0xff7F5050);
        col2 = Colour(0xff6F3535);
        col3 = Colour(0xff451515);

        col11 = Colour(0xff552F2F);
        col21 = Colour(0xff3F1818);
        col31 = Colour(0xff2F0505);
    }
    else
    {
        col1 = Colour(0xff55557F);
        col2 = Colour(0xff36365E);
        col3 = Colour(0xff252540);

        col11 = Colour(0xff454560);
        col21 = Colour(0xff22224A);
        col31 = Colour(0xff15152A);
    }

    if(gAux->tickWidthBack > 1.5f)
    {
        // Vertical lines
        while(1)
        {
            if(xtick%(beats_per_bar*ticks_per_beat) == 0)
            {
                imageContext1.setColour(col1);
                imageContext2.setColour(col11);
            }
            else if(xtick%ticks_per_beat == 0)
            {
                imageContext1.setColour(col2);
                imageContext2.setColour(col21);
            }
            else
            {
                imageContext1.setColour(col3);
                imageContext2.setColour(col31);
            }

            xcoord = RoundFloat(xpix);
            if(xcoord > iw)
                break;

            J_VLine(imageContext1, xcoord, 0, ih);
            J_VLine(imageContext2, xcoord, 0, ih);

            xtick++;
            xpix += gAux->tickWidth;
        }
    }
    else
    {
        if(gAux->tickWidthBack < 1.5f)
        {
            imageContext2.setColour(col31.darker(0.1f));
            imageContext2.fillAll();

            imageContext1.setColour(col3.withAlpha(1.0f));
            imageContext1.fillAll();
        }
        else
        {
            imageContext2.setColour(col31.darker(0.1f));
            imageContext2.fillAll();

            imageContext1.setColour(col3.darker(0.1f));
            imageContext1.fillAll();
        }

        // Vertical lines
        int ticksInBar = (ticks_per_beat*beats_per_bar);
        int g1 = beats_per_bar;
        int g2 = ticks_per_beat;
        float xcoord = 0;
        float pixstep = float(gAux->tickWidth*ticks_per_beat);
        bool beats = float(pixstep - (int)pixstep) == 0; // Don't draw beats with noninteger step to avoid uneven shit
        while(1)
        {
            if(g1 == beats_per_bar)
            {
                g1 = 0;
                imageContext1.setColour(col1);
                imageContext2.setColour(col11);
            }
            else
            {
                if(beats)
                {
                    imageContext1.setColour(col2);
                    imageContext2.setColour(col21);
                }
                else
                {
                    imageContext1.setColour(Colours::transparentWhite);
                    imageContext2.setColour(Colours::transparentWhite);
                }
            }

            if(xcoord > iw)
                break;

            J_VLine(imageContext1, RoundFloat(xcoord), 0, ih);
            J_VLine(imageContext2, RoundFloat(xcoord), 0, ih);

            g1++;

            xcoord += pixstep;
        }
    }

    int ty = -1;

    int yc = 0;
    int ytt = 0;
    int absline = 0;

    // Horizontal lines
    while(1)
    {
        yc = ytt;
        if(yc > ih)
        {
            break;
        }

        if(pt->ptype != Patt_Pianoroll)
        {
            // Active color
            if(pt->ptype == Patt_StepSeq)
            {
                imageContext1.setColour(Colour(0xff20204F));
                imageContext2.setColour(Colour(0xff141432));
            }
            else if(pt->OrigPt && pt->OrigPt->autopatt)
            {
                imageContext1.setColour(Colour(0xff3A1414));
                imageContext2.setColour(Colour(0xff301010));
            }
            else
            {
                imageContext1.setColour(Colour(0xff0A3A3A));
                imageContext2.setColour(Colour(0xff0A2A2A));
            }

            J_HLine(imageContext1, yc, 0, iw);
            J_HLine(imageContext2, yc, 0, iw);
        }
        else //(pt->ptype == Patt_Pianoroll)
        {
            imageContext1.setColour(Colour(0xff0A2D2D));
            J_HLine(imageContext1, yc, 0, iw);
            imageContext2.setColour(Colour(0xff0A2D2D));
            J_HLine(imageContext2, yc, 0, iw);
        }

        absline++;
        ytt += gAux->lineHeight;
    }
}

void J_Aux_Grid(Graphics& g)
{
    g.saveState();
    g.reduceClipRegion(GridXS1, GridYS1 + 1, GridXS2 - GridXS1, GridYS2 - GridYS1 - 1);

    g.setColour(Colour(0xffFFFFFF));
    //J_FillRect(g, GridXS1, GridYS1, GridXS2, GridYS2);

    Pattern* pt = gAux->workPt;

    int cx = CursX;
    int cy = CursY;

    int px1 = gAux->ptdrawx1;
    int px2 = gAux->ptdrawx2;
    int py1 = gAux->ptdrawy1;
    int py2 = gAux->ptdrawy2;

    keyX = MainX1;
    keyY = GridYS1 + 1;
    if(GridXS2 > GridXS1)
    {
        keyW = gAux->keywidth + 3;
    }
    else
    {
        keyW =  GridXS2 - (MainX1);
    }
    keyH = GridYS2 - GridYS1;

    int ptx = (int)(gAux->tickWidth*gAux->workPt->tick_length - gAux->tickWidth*gAux->OffsTick);
    if(ptx < 0)
    {
        ptx = 0;
    }
    else if(ptx > (px2 - px1 + 1))
    {
        ptx = px2 - px1 + 1;
    }

    // Tile grid with pregenerated image
    int xrange = RoundFloat((gAux->workPt->tick_length - gAux->OffsTick)*gAux->tickWidth);
    int len = (beats_per_bar*ticks_per_beat);
    int xoffs = RoundFloat((gAux->OffsTick/len - (int)gAux->OffsTick/len)*gAux->tickWidth*len);

    // Active part of the grid
    if(gAux->workPt->tick_length > gAux->OffsTick)
    {
#ifdef USE_OLD_JUCE
        ImageBrush* ibrush1 = new ImageBrush(img_auxgrid1, GridXS1 - xoffs, GridYS1 - gAux->bottomincr, 1);
        g.setBrush(ibrush1);
#else
        g.setTiledImageFill(*img_auxgrid1, GridXS1 - xoffs, GridYS1 - gAux->bottomincr, 1);;
#endif
        J_FillRect(g, GridXS1, GridYS1, GridXS1 + int((gAux->workPt->tick_length - gAux->OffsTick)*gAux->tickWidth), GridYS2);
    }

    // Inactive part of the grid
    if((gAux->workPt->tick_length - gAux->OffsTick)*gAux->tickWidth < (GridXS2 - GridXS1))
    {
#ifdef USE_OLD_JUCE
        ImageBrush* ibrush2 = new ImageBrush(img_auxgrid2, GridXS1 - xoffs, GridYS1 - gAux->bottomincr, 1);
        g.setBrush(ibrush2);
#else
        g.setTiledImageFill(*img_auxgrid2, GridXS1 - xoffs, GridYS1 - gAux->bottomincr, 1);;
#endif
        J_FillRect(g, GridXS1 +  int((gAux->workPt->tick_length - gAux->OffsTick)*gAux->tickWidth), GridYS1, GridXS2, GridYS2);
    }

/*
    Colour col1;
    Colour col2;
    Colour col3;

    Colour col11;
    Colour col21;
    Colour col31;

    if(pt->ptype == Patt_StepSeq)
    {
        col1 = Colour(0xff4545BF);
        col2 = Colour(0xff3A4A70);
        col3 = Colour(0xff202040);

        col11 = Colour(0xff303070);
        col21 = Colour(0xff2A2A4A);
        col31 = Colour(0xff20202A);
    }
    else if(pt->OrigPt && pt->OrigPt->autopatt)
    {
        col1 = Colour(0xff7F5050);
        col2 = Colour(0xff6F3535);
        col3 = Colour(0xff451515);

        col11 = Colour(0xff552F2F);
        col21 = Colour(0xff3F1818);
        col31 = Colour(0xff2F0505);
    }
    else
    {
        col1 = Colour(0xff55557F);
        col2 = Colour(0xff36365E);
        col3 = Colour(0xff252540);

        col11 = Colour(0xff454560);
        col21 = Colour(0xff22224A);
        col31 = Colour(0xff15152A);
    }

    float pixoffs = (gAux->OffsTick - (int)gAux->OffsTick)*gAux->tickWidth;
    int xt = (int)gAux->OffsTick;
    float xcoord = px1 - pixoffs;
    if(gAux->tickWidthBack > 1.5f)
    {
        while(1)
        {
            if(xt < gAux->workPt->tick_length)
            {
                if(xt%(beats_per_bar*ticks_per_beat) == 0)
                {
                    g.setColour(col1);
                }
                else if(xt%ticks_per_beat == 0)
                {
                    g.setColour(col2);
                }
                else
                {
                    g.setColour(col3);
                }
            }
            else
            {
                if(xt%(beats_per_bar*ticks_per_beat) == 0)
                {
                    g.setColour(col11);
                }
                else if(xt%ticks_per_beat == 0)
                {
                    g.setColour(col21);
                }
                else
                {
                    g.setColour(col31);
                }
            }

            if(xcoord > px2)
            {
                break;
            }

            J_VLine(g, RoundFloat(xcoord), py1, py2);

            xt++;
            xcoord += gAux->tickWidth;
        }
    }
    else
    {
        if(gAux->tickWidthBack < 1.5f)
        {
            g.setColour(col31.darker(0.1f));
            J_FillRect(g, GridXS1, GridYS1, GridXS2, GridYS2);
            if((gAux->workPt->tick_length - gAux->OffsTick) > 0)
            {
                g.setColour(col3.withAlpha(1.0f));
                J_FillRect(g, GridXS1, GridYS1, GridXS1 + int((gAux->workPt->tick_length - gAux->OffsTick)*gAux->tickWidth), GridYS2);
            }
        }
        else
        {
            g.setColour(col31.darker(0.1f));
            J_FillRect(g, GridXS1, GridYS1, GridXS2, GridYS2);
            if((gAux->workPt->tick_length - gAux->OffsTick) > 0)
            {
                g.setColour(col3.darker(0.1f));
                J_FillRect(g, GridXS1, GridYS1, GridXS1 + int((gAux->workPt->tick_length - gAux->OffsTick)*gAux->tickWidth), GridYS2);
            }
        }

        // Vertical lines
        int ticksInBar = (ticks_per_beat*beats_per_bar);
        float xoffs = ((gAux->OffsTick/ticksInBar - int(gAux->OffsTick/ticksInBar))*gAux->tickWidth*ticksInBar);
        int g1 = beats_per_bar;
        int g2 = ticks_per_beat;
        float xcoord = float(px1) - xoffs;
        float pixstep = float(gAux->tickWidth*ticks_per_beat);
        bool beats = float(pixstep - (int)pixstep) == 0; // Don't draw beats with noninteger step to avoid uneven shit
        while(1)
        {
            if(xcoord < xrange + px1)
            {
                if(g1 == beats_per_bar)
                {
                    g1 = 0;
                    g.setColour(col1);
                }
                else
                {
                    if(beats)
                        g.setColour(col2);
                    else
                        g.setColour(Colours::transparentWhite);
                }
            }
            else
            {
                if(g1 == beats_per_bar)
                {
                    g1 = 0;
                    g.setColour(col11);
                }
                else
                {
                    if(beats)
                        g.setColour(col21);
                    else
                        g.setColour(Colours::transparentWhite);
                }
            }

            if(xcoord > px2)
                break;

            J_VLine(g, RoundFloat(xcoord), py1, py2);

            g1++;

            xcoord += pixstep;
        }
    }
*/

    // Draw pattern range line
    if(pt->OrigPt && pt->OrigPt->autopatt)
    {
        g.setColour(Colour(0xafDC2A2A));
        J_VLine(g, px1 + xrange, py1, py2);
        g.setColour(Colour(0xaf320000));
        J_VLine(g, px1 + xrange + 1, py1, py2);
    }
    else
    {
        g.setColour(Colour(0xaf5ADCDC));
        J_VLine(g, px1 + xrange, py1, py2);
        g.setColour(Colour(0xaf0A3232));
        J_VLine(g, px1 + xrange + 1, py1, py2);
    }

    g.restoreState();

    g.saveState();
    g.reduceClipRegion(px1 - gAux->keywidth - 3, GridYS1 + 1, GridXS2 - (px1 - gAux->keywidth) + 3, GridYS2 - GridYS1 - 1);

    int num_lines = (py2 - py1)/gAux->lineHeight;
    int vo = (py2 - py1)%gAux->lineHeight;
    int line;
    int key;
    int ty = -1;
    int proll_key;
    bool cutup;
    bool cutdown;
    //bool lane;
    Trk* trk;

    int yc = 0;
    int ytt = 0;
    int absline = 0;

    // Horizontal lines
    while(1)
    {
        yc = ytt + py1;
        if(yc - gAux->lineHeight > py2)
        {
            break;
        }

        if(pt->ptype == Patt_Pianoroll)
        {
            line = gAux->note_offset - absline;
        }
        else
        {
            line = gAux->OffsLine + absline;
        }

        trk = GetTrkDataForLine(line, pt->OrigPt);

        if(trk != NULL)
        {
            if(pt->ptype != Patt_Pianoroll)
            {
/*
                // Active color
                if(pt->ptype == Patt_StepSeq)
                {
                    g.setColour(Colour(0xff20204F));
                }
                else if(pt->OrigPt && pt->OrigPt->autopatt)
                {
                    g.setColour(Colour(0xff3A1414));
                }
                else
                {
                    g.setColour(Colour(0xff0A3A3A));
                }

                // Check for lanes and draw bunch ranges if any (not for step sequencer)
                lane = false;
                if(pt->ptype != Patt_StepSeq)
                {
                    if((trk->start_line == line && 
                       (trk->vol_lane.visible == true || trk->pan_lane.visible == true))||
                      ((trk->prev->end_line == line - 1) && 
                       (trk->prev->vol_lane.visible == true || trk->prev->pan_lane.visible == true)))
                    {
                        // Color for lane ranges
                        g.setColour(Colour(0xff3232C8));
                        lane = true;
                    }
                    else
                    {
                        lane = false;
                    }

                    if(trk->buncho == true || trk->prev->bunchito == true)
                    {
                        g.setColour(Colour(0xff006478));
                    }
                    else if(trk->bunched == true)
                    {
                        g.setColour(Colour(0xff5555AF));
                    }
                }
                J_HLine(g, yc, px1, px2);

                // Draw inactive line
                if(lane == false)
                {
                    if(pt->ptype == Patt_StepSeq)
                    {
                        g.setColour(Colour(0xff141432));
                    }
                    else if(pt->OrigPt && pt->OrigPt->autopatt)
                    {
                        g.setColour(Colour(0xff301010));
                    }
                    else
                    {
                        g.setColour(Colour(0xff0A2A2A));
                    }
                }
                else
                {
                    g.setColour(Colour(0xff002A2A));
                }

                J_HLine(g, yc, px1 - 1 + ptx, px2 - 1);
*/

                if(pt->ptype == Patt_StepSeq)
                {
                    // Definarium
                    if(trk->defined_instr != NULL)
                    {
                        if(line == trk->start_line)
                        {
                            g.setColour(Colour(0xffFFFFFF));
                            if(trk->defined_instr->type == Instr_Sample)
                            {
                                g.drawImageAt(img_instrsmall1, px1 - gAux->keywidth - 2, yc + 1, false);
                            }
                            else
                            {
                                g.drawImageAt(img_instrsmall2, px1 - gAux->keywidth - 2, yc + 1, false);
                            }
                            //g.setColour(Colour(0xff1F2F30));
                            //J_HLine(g, yc + 16, px1 - gAux->keywidth - 2, px1 - 1);

                            if(trk->defined_instr == current_instr)
                            {
                                g.setColour(Colour(0xffFF4F20));
                                J_LineCRect(g, px1 - gAux->keywidth - 1, yc + 1, px1 - 1, yc + gAux->lineHeight - 1);
                                g.setColour(Colour(0x8fFF4F20));
                                J_HLine(g, yc + 3, px1 - gAux->keywidth - 1, px1 - 2);
                            }

                            /*
                            J_StepVU1(g, px1 - 10, yc + 2, trk->defined_instr->stepvu);
                            trk->defined_instr->stepvu->drawarea->SetBounds(px1 - 10, 
                                                                            yc + 2, 
                                                                            7, 
                                                                            10, 
                                                                            MainX1, 
                                                                            GridYS1, 
                                                                            GridXS2, 
                                                                            GridYS2);
                            */

                            //g.setColour(Colour(0xff2A2A2A));
                            //J_String_Small_Ranged(g, px1 - gAux->keywidth + 2, yc + gAux->lineHeight - 4, trk->defined_instr->name, gAux->keywidth - 2);
                            if(trk->defined_instr->type == Instr_Sample)
                            {
                                g.setColour(Colour(0xffDFDFDF));
                            }
                            else
                            {
                                g.setColour(Colour(0xff8FBFFF));
                            }
                            J_String_Small_Ranged(g, px1 - gAux->keywidth + 1, yc + gAux->lineHeight - 5, trk->defined_instr->name, gAux->keywidth - 2);
                        }
                    }
                    else
                    {
                        if(ty == -1)
                        {
                            ty = yc;
                        }
                    }
                }
            }
            else //(pt->ptype == Patt_Pianoroll)
            {
                cutup = false;
                cutdown = false;
                key = line;
                proll_key = key%12;
            
                if(absline == 0)
                {
                    J_HPianoKey(g, px1 - gAux->keywidth, yc - gAux->lineHeight, gAux->keywidth - 1, proll_key + 1, gAux->lineHeight, true, false, false, false, IsPianoKeyPressed(key + 1));
                    J_HPianoKey(g, px1 - gAux->keywidth, yc - gAux->lineHeight - gAux->lineHeight, gAux->keywidth - 1, proll_key + 2, gAux->lineHeight, true, false, false, false, IsPianoKeyPressed(key + 2));
                }
            
                if(absline == num_lines - 1)
                {
                    J_HPianoKey(g, px1 - gAux->keywidth, yc + gAux->lineHeight, gAux->keywidth - 1, proll_key - 1, gAux->lineHeight, true, false, false, false, IsPianoKeyPressed(key - 1));
                    J_HPianoKey(g, px1 - gAux->keywidth, yc + gAux->lineHeight + gAux->lineHeight, gAux->keywidth - 1, proll_key - 2, gAux->lineHeight, true, false, false, false, IsPianoKeyPressed(key - 2));
                }
            
                J_HPianoKey(g, px1 - gAux->keywidth, yc, gAux->keywidth - 1, proll_key, gAux->lineHeight, true, cutup, cutdown, false, IsPianoKeyPressed(key));
                if(proll_key == 11)
                {
                    g.setColour(Colour(0xff000000));
                    J_note_string(g, px1 - 16, yc - 2, key + 1, false);
                }

                if(proll_key == 11)    // line divider between octaves
                {
                    g.setColour(Colour(0xff46505A));
                    J_HLine(g, yc, px1, px1 + ptx);

                    g.setColour(Colour(0xff324141));
                    J_HLine(g, yc, px1 + ptx, px2);
                }
/*
                if(proll_key == 11)    // line divider between octaves
                {
                    g.setColour(Colour(0xff46505A));
                }
                else
                {
                    g.setColour(Colour(0xff0A2D2D));
                }
                J_HLine(g, yc, px1, px1 + ptx);

                if(proll_key == 11)
                {
                    g.setColour(Colour(0xff324141));
                    J_HLine(g, yc, px1 + ptx, px2);
                }
                else
                {
                    g.setColour(Colour(0xff0A2D2D));
                    J_HLine(g, yc, px1 + ptx, px2);
                }
*/
                if(proll_key == 0)
                {
                    g.setColour(Colour(0xff000000));
                    J_note_string(g, px1 - 16, yc + gAux->lineHeight - 2, key, false);
                }
            }
        }

        absline++;
        ytt += gAux->lineHeight;
    }

    g.setColour(Colour(0xff000000));
    J_HLine(g, GridYS1 + 1, GridXS1, GridXS2);

    if(ty != -1)
    {
        g.setColour(Colour(0xaf2F3F7F));
        J_HLine(g, ty, px1, px2);

        g.setColour(Colour(0x30000000));
        J_FillRect(g, px1, ty + 1, px2, py2);
    }
    g.restoreState();

    CursX = cx;
    CursY = cy;

    MC->MakeSnapshot(&MC->auxgrid, GridXS1, GridYS1 + 1, GridXS2 - GridXS1 + 1, GridYS2 - GridYS1 - 1);
}

void J_Aux_KeysMode(Graphics& g)
{
    int key;

    int x1 = MainX1;
    int x2 = GridX2 + keys_width;

    for(int cc = x1; cc < x2; cc++)
    {
        if((cc - x1 + keys_offset)%keys_width == 0)
        {
            key = (cc - x1 + keys_offset)/keys_width;
            J_VPianoKey(g, cc, GridY2 + 3, AuxKeysHeight, key);
        }
    }
}

void J_Aux_Scale(Graphics& g)
{
    g.saveState();
    g.reduceClipRegion(GridXS1, GridYS1 - 12, GridXS2 - GridXS1, 13);

    g.setColour(Colours::black);
    g.fillRect(GridXS1, GridYS1 - 12, GridXS2 - GridXS1, 12);
    g.setColour(Colour(35, 55, 65));
    g.fillRect(GridXS1, GridYS1 - 12, GridXS2 - GridXS1, 12);

    g.setColour(Colour(140, 165, 170));

    int ticklen = (beats_per_bar*ticks_per_beat);
    float pixlen = gAux->tickWidth*ticklen;

    int step = 1;
    while(pixlen*step < 19)
        step++;
    int number = int(gAux->OffsTick/ticklen);
    int stepcnt = (step - number%step)%step;

    float xoffs = ((gAux->OffsTick/ticklen - int(gAux->OffsTick)/ticklen)*pixlen);
    float xcoord = GridXS1 - xoffs;
	g.setFont(*ti);
    while(1)
    {
        if(xcoord > GridXS2)
        {
            break;
        }

        if(stepcnt == 0)
        {
            g.drawSingleLineText(String::formatted(T("%3d"), number), RoundFloat(xcoord) - 7, GridYS1 - 1);
            stepcnt = step;
        }
        stepcnt--;

        number += 1;
        xcoord += pixlen;
    }

    //int pos = Aux->curr_play_x - (int)(Aux->OffsTick*Aux->tickWidth) + GridXS1;
    g.restoreState();
}

void J_Aux_PattMode(Graphics& g)
{
    g.saveState();
    g.reduceClipRegion(AuxX1, AuxY1, AuxX2 - AuxX1 + 1, AuxY2 - AuxY1 + 1);

    J_PanelRect(g, AuxX1, AuxY1 + 2, AuxX1 + gAux->keywidth + 2, WindHeight);
    J_LRect(g, AuxX1, AuxY1 + 2, AuxX1 + gAux->keywidth + 2, WindHeight);
    J_PanelRect(g, AuxX1, GridYS2, AuxX1 + gAux->keywidth + 2, WindHeight);
    J_LRect(g, AuxX1, GridYS2, AuxX1 + gAux->keywidth + 2, WindHeight);

    //if(gAux->workPt->ptype != Patt_Pianoroll)
    //    J_LRectC(g, AuxX1 + 2, GridYS1, AuxX1 + gAux->keywidth + 2, GridYS2, Colour(0x5f000000));

    gAux->UpdateDrawInfo();

    J_Aux_Grid(g);

    // Erase controls area
    g.setColour(Colour(4, 40, 35));
    J_FillRect(g, gAux->pattx1 - 1, gAux->patty1 + 2, gAux->pattx1 + gAux->keywidth - 1, gAux->patty1 + 1 + 32);
    // Draw play and stop buttons
    J_Button(g, gAux->playbt, gAux->pattx1 - 2, gAux->patty1 + 1 + 13, 35, 19);
    J_Button(g, gAux->stopbt, gAux->pattx1 + 34, gAux->patty1 + 1 + 13, 27, 19);
    g.setColour(Colour(0xff3250A0));
    J_HLine(g, gAux->patty1 + 1 + 11, gAux->pattx1 - 1, gAux->pattx1 + gAux->keywidth - 1);
    g.setColour(Colour(0xff003278));
    J_HLine(g, gAux->patty1 + 1 + 12, gAux->pattx1 - 1, gAux->pattx1 + gAux->keywidth - 1);
    if(gAux->workPt != gAux->blankPt)
    {
        //g.setColour(Colour(0xffFF0000));
        //J_LineRect(g, gAux->pattx1 - 1, gAux->patty1 + 2, gAux->pattx1 + gAux->keywidth - 1, gAux->patty1 + 13);

        if(gAux->workPt->OrigPt->autopatt == false)
        {
            g.setColour(Colour(0xffEFEFEF));
            J_String_Instr_Ranged(g, gAux->pattx1, gAux->patty1 + 1 + 10, gAux->workPt->name->string, gAux->keywidth + 2);
        }
        else
        {
            if(gAux->workPt->OrigPt->instr_owner->type == Instr_Sample)
            {
                g.setColour(smpcolour);
            }
            else
            {
                g.setColour(gencolour);
            }
            J_String_Instr_Ranged(g, gAux->pattx1, gAux->patty1 + 1 + 10, gAux->workPt->OrigPt->instr_owner->name, gAux->keywidth + 2);
        }
    }
    else
    {
        g.setColour(Colour(0xffAFAFAF));
        J_TextBase_xy(g, gAux->pattx1, gAux->patty1 + 1 + 10, "blank");
    }

    int yq;
    if(gAux->workPt->ptype != Patt_Pianoroll)
    {
        yq = GridYS2 + 2;
    }
    else
    {
        yq = GridYS2 + 24;

        // Dark line at keys
        g.setColour(Colour(36, 122, 115));
        J_VLine(g, gAux->pattx1 - 1, GridYS1, GridYS2);

        g.setColour(panelcolour.darker(0.8f));
        J_FillRect(g, gAux->pattx1 - 2, GridYS2 + 2, gAux->pattx1 + gAux->keywidth + 3, GridYS2 + 18);
        g.setColour(panelcolour.darker(0.3f));
        J_LineRect(g, gAux->pattx1 - 2, GridYS2 + 2, gAux->pattx1 + gAux->keywidth + 2, GridYS2 + 19);

        gAux->bindX = gAux->pattx1;
        gAux->bindY = GridYS2 + 2;
        if(gAux->workPt->ibound == NULL)
        {
            g.setColour(Colour(125, 125, 125));
            J_TextBase_xy(g, gAux->bindX, gAux->bindY + 15, "unbound");
        }
        else
        {
            //g.drawImageAt(gAux->workPt->ibound->alimg, gAux->bindX, gAux->bindY + 5, false);

            if(gAux->workPt->ibound->type == Instr_Sample)
            {
                g.setColour(smpcolour);
            }
            else
            {
                g.setColour(gencolour);
            }
            J_String_Small_Ranged(g, gAux->bindX, gAux->bindY + 15, gAux->workPt->ibound->name, gAux->keywidth + 2);
        }
    }

    g.setColour(panelcolour.darker(0.45f));
    J_FillRect(g, gAux->pattx1 - 2, yq, gAux->pattx1 + gAux->keywidth, yq + 25);
    g.setColour(panelcolour.darker(0.2f));
    J_LineRect(g, gAux->pattx1 - 2, yq, gAux->pattx1 + gAux->keywidth - 1, yq + 26);

    DropMenu* dmn = gAux->qMenu;
    J_QuantMenu(g, gAux->pattx1, yq + 3, dmn);

    g.setColour(panelcolour);
    J_FillRect(g, gAux->ptdrawx2 - 19, gAux->patty1 + 18, gAux->ptdrawx2 + 1, gAux->patty2);
    J_LRect(g, gAux->ptdrawx2 - 19, gAux->patty1 + 18, gAux->ptdrawx2 + 1, gAux->patty2);

    // PattExpand button
    g.setColour(Colour(0xffFFFFFF)); // to restore common alpha value
    J_Button(g, gAux->PattExpand, gAux->ptdrawx2 - 16, gAux->patty1 + 2, 18, 18);

    int hw = (gAux->ptdrawx2 - 19 - 24) - (gAux->pattx1 + gAux->keywidth + 23);

    if(hw > 0)
    {
        gAux->h_sbar->Activate();
        gAux->Back->Activate();
        gAux->Forth->Activate();

        J_ScrollBard(g, gAux->pattx1 + gAux->keywidth + 22, gAux->ptdrawx2 - 19 - 24, gAux->patty1 + 1, gAux->h_sbar, true, false);
        gAux->h_sbar->Activate(gAux->h_sbar->x1, gAux->h_sbar->y, gAux->h_sbar->width, gAux->h_sbar->height);

        J_DarkRect(g, gAux->pattx1 + gAux->keywidth - 1, gAux->patty1 + 1, gAux->pattx1 + gAux->keywidth + 23, gAux->patty1 + 3 + 18);
        J_Button(g, gAux->Back, gAux->pattx1 + gAux->keywidth, gAux->patty1 + 3, 23, 15);
        J_DarkRect(g, gAux->ptdrawx2 - 19 - 25, gAux->patty1 + 1, gAux->ptdrawx2 - 19, gAux->patty1 + 19);
        J_Button(g, gAux->Forth, gAux->ptdrawx2 - 19 - 24, gAux->patty1 + 3, 23, 15);
    }
    else
    {
        gAux->h_sbar->Deactivate();
        gAux->Back->Deactivate();
        gAux->Forth->Deactivate();
    }

    int vh = (gAux->patty2 - 20) - (gAux->patty1 + 1 + 39);

    if(vh > 0)
    {
        gAux->Up->Activate();
        gAux->Down->Activate();

        J_ScrollBard(g, gAux->patty1 + 1 + 37, gAux->patty2 - 18, gAux->ptdrawx2 - 19, gAux->v_sbar, true, false);
        gAux->v_sbar->Activate(gAux->v_sbar->x, gAux->v_sbar->y1, gAux->v_sbar->width, gAux->v_sbar->height);

        J_DarkRect(g, gAux->ptdrawx2 - 19, gAux->patty1 + 1 + 19, gAux->ptdrawx2 - 1, gAux->patty1 + 1 + 19 + 19 + 1);
        J_Button(g, gAux->Up, gAux->ptdrawx2 - 17, gAux->patty1 + 1 + 21, 17, 17);
        J_DarkRect(g, gAux->ptdrawx2 - 19, gAux->patty2 - 2 - 18, gAux->ptdrawx2 - 1, gAux->patty2);
        J_Button(g, gAux->Down, gAux->ptdrawx2 - 17, gAux->patty2 - 18, 17, 17);
    }
    else
    {
        gAux->v_sbar->Deactivate();
        gAux->Up->Deactivate();
        gAux->Down->Deactivate();
    }

    if(GridXS2 > GridXS1)
    {
        // Lines for auxscale
        g.setColour(Colour(0xff386482));
        J_HLine(g, gAux->patty1 + 1 + gAux->h_sbar->tolschina + 1, gAux->pattx1 + gAux->keywidth, gAux->ptdrawx2 - 19);
        g.setColour(Colour(0xff386482).darker(0.6f));
        J_HLine(g, gAux->patty1 + 1 + gAux->h_sbar->tolschina + 2, gAux->pattx1 + gAux->keywidth, gAux->ptdrawx2 - 19);
        g.setColour(Colour(0xff385472));
        J_HLine(g, gAux->patty1 + 1 + gAux->h_sbar->tolschina + 15, gAux->pattx1 + gAux->keywidth, gAux->ptdrawx2 - 19);

        //g.setColour(Colour(0x8f144650));
        //J_HLine(g, gAux->patty1 + 2 + 22, gAux->pattx1 + gAux->keywidth - 1, gAux->ptdrawx2 - 19);
        //g.setColour(Colour(0x4f144650));
        //J_HLine(g, gAux->patty1 + 2 + 23, gAux->pattx1 + gAux->keywidth - 1, gAux->ptdrawx2 - 19);

        // Lines for auxaux
        g.setColour(Colour(0xff344660));
        J_HLine(g, GridYS2, gAux->pattx1 + gAux->keywidth, gAux->ptdrawx2 - 19);
        g.setColour(Colour(0xff344660).brighter(0.2f));
        J_HLine(g, GridYS2 + 1, gAux->pattx1 + gAux->keywidth, gAux->ptdrawx2 - 19);

        // Bottom dark line
        g.setColour(Colour(40, 50, 70));
        J_HLine(g, WindHeight - 3, GridXS1, GridXS2);
    }
    else
    {
        g.setColour(Colour(0xff144650));
        J_Line(g, gAux->pattx1, gAux->patty1 + 1 + 34, GridXS2, gAux->patty1 + 1 + 34);
    }

    g.restoreState();
    J_Aux_Scale(g);
    J_Content_Aux(g);
    J_Auxaux(g);
}

void J_2Sends(Graphics& g, int x, int y, MixChannel* mchan, int x1r, int y1r, int x2r, int y2r)
{
    //g.drawImageAt(img_mc2sends, x, y, false);

    //g.setColour(Colour(0xffAFFFFF));
    //J_TextSmall_xy(g, x + 2, y + 9, "Send");

    g.setColour(Colour(0xffFFFFFF));

    Knob* kb = mchan->r_amount3;
	J_KnobLarge(g, kb, false, x + 84, y + 11);
    kb->Activate(kb->x - kb->rad - 2, kb->y - kb->rad, 20, 20, x1r, y1r, x2r, y2r);

    kb = mchan->r_amount2;
	J_KnobLarge(g, kb, false, x + 62, y + 11);
    kb->Activate(kb->x - kb->rad - 2, kb->y - kb->rad, 20, 20, x1r, y1r, x2r, y2r);

    kb = mchan->r_amount1;
	J_KnobLarge(g, kb, false, x + 40, y + 11);
    kb->Activate(kb->x - kb->rad - 2, kb->y - kb->rad, 20, 20, x1r, y1r, x2r, y2r);
}

void J_AuxMixerHighlights(Graphics& g, bool hlredraw)
{
    if(gAux->auxmode == AuxMode_Mixer)
    {
        if(hlredraw)
        {
            MixChannel* mchan = M.active_mixchannel;
            MixChannel* mchan1;
            g.saveState();
            g.reduceClipRegion(mixX, mixY, mixW - MixChanWidth - 11, mixH - 16);
            for(int mc = 0; mc < NUM_MIXCHANNELS; mc++)
            {
                if(gAux->mchan[mc].visible == true)
                {
                    mchan1 = &gAux->mchan[mc];
                    if(mchan != NULL && mchan == &gAux->mchan[mc] && M.auxcellindexing)
                    {
                        g.setColour(Colour(0xffFF5F2F));
                        J_TextFix_xy(g, mchan->x + 81, mchan->y - 1, mchan->indexstr);
                    }
                    else
                    {
                        g.setColour(Colour(61, 135, 148));
                        J_TextFix_xy(g, gAux->mchan[mc].x + 81, gAux->mchan[mc].y - 1, gAux->mchan[mc].indexstr);
                    }
                }
            }
            g.restoreState();
        }

        // While hlredraw flag represents occasional redraw stuff, there're also stuff, that we must
        // redraw always to prevent highlight corruption
        if(gAux->current_eff != NULL)
        {
            MixChannel* mchan = gAux->current_eff->mixcell->mchan;
            if(mchan->visible == true)
            {
                int y1 = mchan->ry1;
                int y2 = mchan->ry2;
                g.saveState();
                if(mchan == &gAux->masterchan)
                {
                    g.reduceClipRegion(mixX + mixW - MixChanWidth - 11, y1, MixChanWidth, y2 - y1);
                }
                else
                {
                    g.reduceClipRegion(mixX, y1, mixW - MixChanWidth - 11, y2 - y1);
                }

                Eff* eff = gAux->current_eff;
                g.setColour(Colour(0xffFF6830));
                J_HLine(g, eff->y, eff->x, eff->x + eff->w + 2);
                J_HLine(g, eff->y + eff->h - 1, eff->x, eff->x + eff->w + 2);

                g.setColour(Colour(0x8fFF6830));
                J_HLine(g, eff->y + 1, eff->x, eff->x + eff->w + 2);
                J_HLine(g, eff->y + eff->h - 2, eff->x, eff->x + eff->w + 2);

                g.restoreState();
            }
        }

        if(hlredraw && M.mmode & MOUSE_DRAG_N_DROPPING && 
            (M.drag_data.drag_type == Drag_Effect_File || M.drag_data.drag_type == Drag_MixChannel_Effect) &&
            M.active_dropmixchannel != NULL)
        {
            if(M.drag_data.drag_type == Drag_MixChannel_Effect && 
                (M.drag_data.drag_stuff == M.dropeff1 || M.drag_data.drag_stuff == M.dropeff2))
            {
                // Avoid highlight in this case
            }
            else
            {
                MixChannel* mchan = M.active_dropmixchannel;
                if(M.active_dropmixchannel == &gAux->masterchan)
                {
                    g.reduceClipRegion(mixX + mixW - MixChanWidth - 11, mchan->ry1, MixChanWidth, mchan->ry2 - mchan->ry1);
                }
                else
                {
                    g.reduceClipRegion(mixX, mchan->ry1, mixW - MixChanWidth - 11, mchan->ry2 - mchan->ry1);
                }

                int x, y, w, h;
                if(M.dropeff1 != NULL || M.active_dropmixchannel->first_eff == NULL ||
                    (M.dropeff1 == NULL && M.active_dropmixchannel->first_eff != NULL))
                {
                    if(M.dropeff1 != NULL)
                    {
                        x = M.dropeff1->x;
                        y = M.dropeff1->y;
                        w = M.dropeff1->w + 2;
                        h = M.dropeff1->h;
                    }
                    else
                    {
                        x = M.active_dropmixchannel->x;
                        y = M.active_dropmixchannel->ry1;
                        w = MixChanWidth;
                        h = 0;
                    }

                    g.setColour(Colour(0xffFFFF30));
                    J_Line(g, x, y + h, x + w, y + h);
                    g.setColour(Colour(0x7fFFFF30));
                    J_Line(g, x, y + h + 1, x + w, y + h + 1);
                    g.setColour(Colour(0x3fFFFF30));
                    J_Line(g, x, y + h + 2, x + w, y + h + 2);
                }

                if(M.dropeff2 != NULL)
                {
                    x = M.dropeff2->x;
                    y = M.dropeff2->y;
                    w = M.dropeff2->w + 2;
                    h = M.dropeff2->h;
                    g.setColour(Colour(0xffFFFF30));
                    J_Line(g, x, y, x + w, y);
                    g.setColour(Colour(0x7fFFFF30));
                    J_Line(g, x, y - 1, x + w, y - 1);
                    g.setColour(Colour(0x3fFFFF30));
                    J_Line(g, x, y - 2, x + w, y - 2);
                }
            }
        }
    }
}

void J_MChanArrows(Graphics& g, MixChannel* mchan)
{
    if(mchan->voffs > 0)
    {
        JUpArrow(g, mchan->x + MixChanWidth/2, mchan->y, 7);
    }

    if(mchan->cheight - mchan->voffs > mchan->ry2 - mchan->ry1)
    {
        JDownArrow(g, mchan->x + MixChanWidth/2, mchan->ry2, 7);
    }
}

void J_MixChannel(Graphics& g, int x, int y, int h, MixChannel* mchan, int x1r, int y1r, int x2r, int y2r)
{
    mchan->x = x;
    mchan->y = y;
    mchan->width = MixChanWidth;
    mchan->height = h;

    mchan->x1r = x1r;
    mchan->x2r = x2r;
    mchan->y1r = y1r;
    mchan->y2r = y2r;

    int yb = y + h;

    int baseh = img_mixpadmaster->getHeight();

    g.setColour(Colour(0xff002535));
    J_FillRect(g, x, y, x + MixChanWidth - 1, yb);
    g.setColour(Colour(0x1fFFFFFF));
    J_LineRect(g, x, y, x + MixChanWidth - 1, yb);
    g.setColour(Colour(0x1F000000));
    //J_LineRect(g, x + 1, y + 1, x + MixChanWidth - 2, yb - 1);
    int yrc = y + 1;
    while(yrc < yb)
    {
        J_HLine(g, yrc, x + 1, x + MixChanWidth - 1);
        yrc += 2;
    }

    //g.setColour(Colour(0x7f1F3F4D));
    //J_VLine(g, x + 2, y, yb);
    //J_VLine(g, x + MixChanWidth - 3, y, yb);
    g.setColour(Colour(0x2f1F3F4D));
    J_VLine(g, x + 2, y, yb);
    J_VLine(g, x + MixChanWidth - 3, y, yb);

    g.setColour(Colour(0xffFFFFFF));
    //J_PanelRect(g, x, yb - baseh, x + MixChanWidth - 1, yb);
    if(mchan->master == false && mchan->send == false)
        g.drawImageAt(img_mixpad, x, yb - img_mixpad->getHeight(), false);
    else
        g.drawImageAt(img_mixpadmaster, x, yb - img_mixpadmaster->getHeight(), false);

/*
    // Make gradient
    GradientBrush gb(Colour(0x11000000), float(x), float(yb - baseh), 
                      Colour(0x3f000000), float(x + 31), float(yb),
                      false);
    g.setBrush(&gb);
    g.fillRect(x, yb - baseh, MixChanWidth - 1, baseh);
    

    J_LRect(g, x, yb - baseh, x + MixChanWidth - 1, yb);
*/

    SliderBase* sl = mchan->mc_main->vol_slider;
    J_SliderVertical(g, sl, x + 64, yb - 7, 15);
    sl->Activate(sl->x - 1, sl->y - img_vslpad->getHeight(), 17, img_vslpad->getHeight() + 5, x1r, y1r, x2r, y2r);
    sl->vu->drawarea->EnableWithBounds(sl->x - 17, sl->y - img_vslpad->getHeight(), 49, img_vslpad->getHeight() + 5, x1r, y1r, x2r, y2r);

    if(mchan->master == false)
    {
        SliderHPan* slp = mchan->mc_main->pan_slider;
        J_SliderMixPan(g, slp, x + 1, yb - img_mixpad->getHeight() + 28, 10, false);
        slp->Activate(slp->x - 2, slp->y, slp->width + 2 + 4, slp->height + 2, x1r, y1r, x2r, y2r);

        Toggle* tg = mchan->mc_main->mute_toggle;
        J_MuteToggle(g, tg, x + 2, yb - 76);
        tg->Activate(tg->x, tg->y, tg->width, tg->height, x1r, y1r, x2r, y2r);

        tg = mchan->mc_main->solo_toggle;
        J_SoloToggle(g, tg, x + 16, yb - 76);
        tg->Activate(tg->x, tg->y, tg->width, tg->height, x1r, y1r, x2r, y2r);
    }

    int yb1;
    if(mchan->master == false && mchan->send == false)
    {
        yb1 = yb - img_mixpad->getHeight();
    }
    else
    {
        yb1 = yb - baseh + 1;
    }

    // First draw scroll buttons as we use their coordinates when draw paramcells for fx
    //////
    mchan->ry1 = y + 1;
    mchan->ry2 = yb1;

    // Now draw mixchannel content i.e. effects
    g.saveState();
    g.reduceClipRegion(x, mchan->ry1, MixChanWidth, mchan->ry2 - mchan->ry1 - 1);
    mchan->cheight = J_DrawMixChannelContent(g, x, mchan->ry1 - mchan->voffs, mchan);
    g.restoreState();

    /*
    if(mchan->voffs > 0)
    {
        mchan->up->SetImages(img_btinsupactive, img_btinsup2);
    }
    else
    {
        mchan->up->SetImages(img_btinsup1, img_btinsup2);
    }

    if(mchan->cheight - mchan->voffs > mchan->ry2 - mchan->ry1)
    {
        mchan->down->SetImages(img_btinsdownactive, img_btinsdown2);
    }
    else
    {
        mchan->down->SetImages(img_btinsdown1, img_btinsdown2);
    }

    J_Button(g, mchan->up, x, y, 97, 10);
    J_Button(g, mchan->down, x, yb1, 97, 10);
    Butt* bt = mchan->up;
    bt->Activate(bt->x, bt->y, bt->width, bt->height, x1r, y1r, x2r, y2r);
    bt = mchan->down;
    bt->Activate(bt->x, bt->y, bt->width, bt->height, x1r, y1r, x2r, y2r);
    */

    if(mchan != &gAux->masterchan && mchan->send == false)
    {
        J_2Sends(g, x, yb - img_mixpad->getHeight(), mchan, x1r, y1r, x2r, y2r);
    }

    J_MChanArrows(g, mchan);

    if(mchan->master == false && mchan->send == false)
    {
        DigitStr* rstr = mchan->routestr;
        if(rstr->bground == NULL)
        {
            MC->MakeSnapshot(&rstr->bground, 4, img_mixpad->getHeight() - 14, 18, 11, img_mixpad);
        }
        J_DigitStr_xy(g, x + 5, yb - 4, rstr, true, Colour(15, 52, 68));
        rstr->Activate(x + 2, yb - 4 - rstr->edy, rstr->edx, rstr->edy, x1r, y1r, x2r, y2r);
    }

    mchan->drawarea->SetBounds(mchan->x, mchan->y, mchan->width, mchan->height, x1r, y1r, x2r, y2r);
}

void J_Aux_MixMode(Graphics& g)
{
    g.saveState();
    g.reduceClipRegion(mixX, mixY, mixW, mixH);

    g.setColour(Colour(50, 114, 99));
    J_FillRect(g, mixX, mixY, mixX + 6, mixY + 8);

    g.setColour(Colour(0xff000000));
    J_FillRect(g, mixX, mixY, mixX + mixW, mixY + mixH);

    int mixMasterX = mixX + mixW - MixChanWidth - 10;
    int mcx = mixX - gAux->hoffs;
    for(int mc = 0; mc < NUM_MIXCHANNELS; mc++)
    {
        if(mcx + MixChanWidth > mixX && mcx < mixMasterX - 1)
        {
            gAux->mchan[mc].Enable();
            J_MixChannel(g, mcx, mixY + mixHeading, mixH - gAux->sbarH - mixHeading + 1, &gAux->mchan[mc], mixX, mixY, mixMasterX - 2, mixY + mixH - gAux->sbarH + 1);

            g.setColour(Colour(0, 37, 53));
            J_FillRect(g, mcx, mixY, mcx + MixChanWidth - 1, mixY + mixHeading - 1);

            g.setColour(Colour(22, 52, 69));
            J_LineRect(g, mcx, mixY + 1, mcx + MixChanWidth - 1, mixY + mixHeading - 2);

            //g.setColour(Colour(0x8f000000));
            //J_TextInstr_xy(g, mcx + 83, mixY + mixHeading, gAux->mchan[mc].indexstr);
            g.setColour(Colour(61, 135, 148));
            J_TextFix_xy(g, mcx + 81, mixY + mixHeading - 1, gAux->mchan[mc].indexstr);
        }
        else
        {
            gAux->mchan[mc].Disable();
        }
        mcx += MixChanWidth + 1;
    }

    // Send channels
    mcx += 10;
    g.setColour(Colour(31, 49, 63));
    J_FillRect(g, mcx - 10, mixY + mixHeading, mcx - 2, mixY + mixH - gAux->sbarH + 1);

    for(int sc = 0; sc < 3; sc++)
    {
        if(mcx + MixChanWidth > mixX && mcx < mixMasterX - 1)
        {
            gAux->sendchan[sc].Enable();
            J_MixChannel(g, mcx, mixY + mixHeading, mixH - gAux->sbarH - 9, &gAux->sendchan[sc], mixX, mixY, mixMasterX - 2, mixY + mixH - gAux->sbarH + 1);

            g.setColour(Colour(0, 37, 53));
            J_FillRect(g, mcx + 1, mixY, mcx + MixChanWidth - 2, mixY + mixHeading - 1);

            g.setColour(Colour(22, 52, 69));
            J_LineRect(g, mcx, mixY + 1, mcx + MixChanWidth - 1, mixY + mixHeading - 2);

            //g.setColour(Colour(0xff000000));
            //J_TextInstr_xy(g, mcx + 68, mixY + 8, gAux->sendchan[sc].indexstr);
            g.setColour(Colour(84, 150, 178));
            J_TextFix_xy(g, mcx + 63, mixY + mixHeading - 1, gAux->sendchan[sc].indexstr);
        }
        else
        {
            gAux->sendchan[sc].Disable();
        }
        mcx += MixChanWidth + 1;
    }

    gAux->mix_sbar->full_len = float(NUM_MIXCHANNELS*(MixChanWidth + 1) + 14 + 3*(MixChanWidth + 1));
    gAux->mix_sbar->offset = gAux->mix_sbar->actual_offset = float(gAux->hoffs);
    gAux->mix_sbar->visible_len = float(mixMasterX - (mixX) - 1);
    J_ScrollBard(g, mixX - 2, mixMasterX, mixY + mixH - gAux->sbarH, gAux->mix_sbar, false, false);

    J_MixChannel(g, mixMasterX, mixY, mixH, &gAux->masterchan, mixX, mixY, mixX + mixW, mixY + mixH);

    //GradientBrush gb(Colour(0x00000000), float(mixMasterX - 10), float(mixY + (mixH - 15)/2), 
    //                  Colour(0x8f000000), float(mixMasterX - 1), float(mixY + (mixH - 15)/2),
    //                  false);
    //g.setBrush(&gb);
    //g.fillRect(mixMasterX - 10, mixY, 10, mixH - 15);
    g.setColour(Colour(0, 21, 37));
    J_Line(g, mixMasterX - 1, mixY, mixMasterX - 1, mixY + mixH);
    J_Line(g, mixX + mixW - 10, mixY, mixX + mixW - 10, mixY + mixH);

    g.setColour(Colour(31, 49, 63));
    J_FillRect(g, mixX + mixW - 9, mixY, mixX + mixW, mixY + mixH);

    g.restoreState();
}

void J_Aux(Graphics& g)
{
    //J_PanelRect1(g, AuxX1, MainY2 + 1, MainX2 + MixCenterWidth, AuxY1);
    J_PanelRect(g, AuxX1, AuxY1, MainX2 + MixCenterWidth, AuxY2 + 2);
    J_LRect(g, AuxX1 + 2, AuxY1 + 2, MainX2 + MixCenterWidth - 2, AuxY2);

    if(gAux->isVolsPansMode())
    {
        g.setColour(Colour(0xff1F2F3F));
        //g.setColour(panelcolour.darker(1).darker(0.25f));
        g.fillRect(AuxX1 + 25, AuxY1 + 2, AuxX2 - AuxX1 - 26, AuxY2 - AuxY1 - 3);

        g.setColour(Colour(37, 55, 64));
        J_FillRect(g, AuxX1 + 1, AuxY1 + 2, AuxX1 + 22, WindHeight);
        g.setColour(Colour(0xff2E4650));
        J_VLine(g, AuxX1 + 23, AuxY1 + 2, WindHeight);
        g.setColour(Colour(52, 58, 66));
        J_VLine(g, AuxX1 + 24, AuxY1 + 2, WindHeight);

        g.setColour(Colour(56, 116, 130));
        J_VLine(g, AuxX1, AuxY1 + 2, WindHeight);

        /*
        g.setColour(Colour(27, 33, 40));
        int y = AuxY1 + 2;
        while(y < WindHeight)
        {
            J_HLine(g, y, AuxX1 + 2, AuxX1 + 23);
            y += 2;
        }*/

        //g.setColour(Colour(0xff00403D));
        //J_VLine(g, AuxX2 - 1, AuxY1 + 2, AuxY2 - 1);

        g.setColour(Colour(40, 50, 70));
        J_HLine(g, WindHeight - 3, AuxX1 + 25, AuxX2);

        // Vertical lines
        Colour col1 = Colour(0xbf525265);
        Colour col2 = Colour(0xbf35354A);
        Colour col3 = Colour(0xff3F3F4A);
        float xo =(OffsTick - (int)OffsTick)*tickWidth;
        int xt = (int)OffsTick;
        float xtt = 0;
        int xc;
        while(1)
        {
            if(xt%(beats_per_bar*ticks_per_beat)== 0)
            {
                g.setColour(col1);

                xc = RoundFloat(xtt - xo + GridX1);
                if(xc > GridX2)
                    break;

                if(xc >= GridX1 && xc <= AuxX2)
                {
                    //J_Line(g, xc, GridY1, xc, GridY2 + 1);
                    J_VLine(g, xc, AuxY1 + 3, AuxY2);
                }
            }
            else if(xt%ticks_per_beat == 0)
            {
                g.setColour(col2);
                xc = RoundFloat(xtt - xo + GridX1);
                if(xc > AuxX2 - 2)
                    break;

                if(xc >= GridX1 && xc <= AuxX2)
                {
                    //J_Line(g, xc, GridY1, xc, GridY2 + 1);
                    J_VLine(g, xc, AuxY1 + 3, AuxY2);
                }
            }
            else
            {
                g.setColour(col3);
            }

            xt++;
            xtt += tickWidth;
        }
    }
    else if(gAux->auxmode == AuxMode_Pattern)
    {
        g.setColour(Colour(0xff000000));
        g.fillRect(AuxX1 + gAux->keywidth + 3, AuxY1 + 3, AuxX2 - AuxX1 - gAux->keywidth - 23, AuxY2 - AuxY1 - 4);
    }

    if(gAux->isVolsPansMode() || (gAux->auxmode == AuxMode_Pattern && gAux->workPt->ptype != Patt_Pianoroll))
    {
        if(MC->listen->comboCC != NULL && MC->listen->comboCC->isVisible() == true)
        {
            MC->listen->comboCC->setVisible(false);
        }
    }

    if(!(gAux->auxmode == AuxMode_Pattern && gAux->workPt->OrigPt->autopatt == true))
    {
        gAux->relswitch->Deactivate();
        gAux->reloctaver->btdn->Deactivate();
        gAux->reloctaver->btup->Deactivate();
    }

    gAux->DisableStepVUs();

    gAux->x = MainX1;
    gAux->y = AuxY1;
    gAux->width = AuxX2 - AuxX1 + MixCenterWidth;
    gAux->height = AuxY2 - AuxY1 + 1;

    gAux->v_sbar->active = false;
    gAux->h_sbar->active = false;

    if(gAux->auxmode == AuxMode_Vols)
    {
        g.saveState();
        g.reduceClipRegion(AuxRX1, AuxRY1, AuxRX2 - AuxRX1 + 1, AuxRY2 - AuxRY1 + 1);
        J_VolLane(g, Loc_MainGrid, NULL, NULL, AuxRX1, AuxRY1, AuxRX2, AuxRY2);
        g.restoreState();
    }
    else if(gAux->auxmode == AuxMode_Pans)
    {
        g.saveState();
        g.reduceClipRegion(AuxRX1, AuxRY1, AuxRX2 - AuxRX1 + 1, AuxRY2 - AuxRY1 + 1);
        J_PanLane(g, Loc_MainGrid, NULL, NULL, AuxRX1, AuxRY1, AuxRX2, AuxRY2);;
        g.restoreState();
    }
    else if(gAux->auxmode == AuxMode_Pattern)
    {
        J_Aux_PattMode(g);
    }
    else if(gAux->auxmode == AuxMode_Mixer)
    {
        J_Aux_MixMode(g);
    }

    if(gAux->auxmode != AuxMode_Mixer)
    {
        J_Aux_ModeButtons(g, AuxX2 , AuxY1 + 2);
        if(gAux->auxmode == AuxMode_Pattern)
        {
            int yy = GridYS2 > AuxY1 + 188 ? GridYS2 : AuxY1 + 188;
            J_Aux_PattButtons(g, AuxX2, AuxY1 + 148, yy);
            if(gAux->workPt->ptype == Patt_StepSeq)
            {
                //J_Button(Aux->revsmp, MainX2 + 4, MainY2 + 3 + 30 + 3 + 20 + 54 + 25 + 3 + 25 + 3, 26, 25);
            }
            else if(gAux->workPt->ptype == Patt_Pianoroll)
            {
            }
            else if(gAux->workPt->ptype == Patt_Grid)
            {
                if(GridYS2 > GridYS1)
                {
                    g.saveState();
                    g.reduceClipRegion(AuxX1, GridYS1, gAux->keywidth, GridYS2 - GridYS1);
                    if(gAux->workPt->OrigPt->autopatt == true)
                    {
                        /* //Postponed
                        gAux->relswitch->Activate();
                        Toggle* rtg = gAux->relswitch;
                        J_RelSwitchToggle(g, AuxX1 + 5, GridYS1 + 5, rtg);
                        rtg->drawarea->Enable();
                        rtg->drawarea->SetBounds(rtg->x,
                                                 rtg->y,
                                                 rtg->width,
                                                 rtg->height);
                        */

                        gAux->reloctaver->btdn->Activate();
                        gAux->reloctaver->btup->Activate();
                        J_Numba(g, gAux->reloctaver, AuxX1 + 4, GridYS1 + 4);
                    }
                    g.restoreState();
                }
            }

            /* //Postponed
            if(MC->listen->comboCC->isVisible() == false)
            {
                MC->listen->comboCC->setVisible(true);
            }
            MC->listen->comboCC->setBounds(MainX2 + 64, yy + 1, 97, 18);
            */
        }

        /*
        if(gAux->auxmode != AuxMode_Mixer)
        {
            J_MixTxt(g, MainX2 + MixCenterWidth - 58, MainY2 + 5);
        }
        */
    }
}

void J_ValueString1(Graphics& g, int x, int y, ValueString* vstr)
{
    g.setFont(*ins);
    switch(vstr->vstrtype)
    {
        case VStr_String:
            J_TextSmall_xy(g, x, y, vstr->val.str);
            break;
        case VStr_Hz:
            g.drawSingleLineText(String::formatted(T("%.0f Hz"), vstr->val.hz), x, y);
            break;
        case VStr_kHz:
            g.drawSingleLineText(String::formatted(T("%.2f kHz"), vstr->val.hz), x, y);
            break;
        case VStr_Percent:
            g.drawSingleLineText(String::formatted(T("%d %s"), vstr->val.percent, "%"), x, y);
            break;
        case VStr_Integer:
            g.drawSingleLineText(String::formatted(T("%d"), vstr->val.integer), x, y);
            break;
        case VStr_dB:
            if(vstr->val.n_val > 0)
			    g.drawSingleLineText(String::formatted(T("%.1f dB"), vstr->val.n_val), x, y);
            else
			    g.drawSingleLineText(String::formatted(T("%.1f dB"), vstr->val.n_val), x, y);
            break;
        case VStr_msec:
			g.drawSingleLineText(String::formatted(T("%.1f ms"), vstr->val.n_val), x, y);
            break;
        case VStr_second:
			g.drawSingleLineText(String::formatted(T("%.1f sec"), vstr->val.n_val), x, y);
            break;
        case VStr_oct:
			g.drawSingleLineText(String::formatted(T("%.2f oct"), vstr->val.n_val), x, y);
            break;
        case VStr_Default:
            //g.setFont(*ti);
            if (vstr->label[0] != '\0')
            {
                J_TextSmall_xy(g, x, y, vstr->label);
            }
            else
            {
                g.drawSingleLineText(String::formatted(T("%.2f"), vstr->val.n_val), x, y);
            }
            break;
    }
}

void J_ValueString(Graphics& g, int x, int y, ValueString* vstr)
{
    switch(vstr->vstrtype)
    {
        case VStr_String:
            J_TextSmall_xy(g, x, y, vstr->outstr);
            break;
        case VStr_Hz:
        case VStr_kHz:
        case VStr_fHz:
        case VStr_fHz1:
        case VStr_Percent:
        case VStr_Integer:
        case VStr_dB:
        case VStr_msec:
        case VStr_msec2:
        case VStr_second:
        case VStr_oct:
        case VStr_Beats:
        case VStr_Semitones:
        case VStr_DryWet:
            J_TextInstr_xy(g, x, y, vstr->outstr);
            break;
        case VStr_Default:
            if(vstr->label[0] != '\0')
            {
                J_TextSmall_xy(g, x, y, vstr->outstr);
            }
            else
            {
                J_TextInstr_xy(g, x, y, vstr->outstr);
            }
            break;
    }
}

void J_ParamToggle(Graphics& g, Toggle* ptg, int x, int y, int w, int h)
{
    ptg->active = true;
    ptg->x = x;
    ptg->y = y;

    ptg->width = w;
    ptg->height = h;

    g.setColour(Colour(37, 55, 74));
    J_LineRect(g, x, y, x + 12, y + 8);
    if(*(ptg->state) == true)
    {
        g.drawImageAt(img_tgparam, x + 1, y + 1, false);
    }
    else
    {
        g.drawImageAt(img_tgparamoff, x + 1, y + 1, false);
    }
}

void J_ParamRadioToggle(Graphics& g, Toggle* ptg, int x, int y, int w, int h)
{
    ptg->active = true;
    ptg->x = x;
    ptg->y = y;

    ptg->width = w;
    ptg->height = h;

    //g.setColour(Colour(33, 45, 58));
    //J_FillRect(g, x, y, x + 14, y + 9);
    g.setColour(Colour(10, 25, 30));
    J_LineRect(g, x, y, x + 14, y + 9);
    g.setColour(Colour(42, 55, 61));
    J_LineRect(g, x + 1, y + 1, x + 13, y + 8);
    if(*(ptg->state) == true)
    {
        g.drawImageAt(img_tgparam1, x + 1, y + 1, false);
    }
    else
    {
        //g.setColour(Colour(30, 30, 30));
        //J_FillRect(g, x + 1, y + 1, x + 11, y + 8);
    }
}

void J_BrwParamCell(Graphics& g, Paramcell* pcell, int x, int yc)
{
/*
    g.setColour(Colour(55, 55, 55));
    J_FillRect(g, pcell->x, yc, pcell->x + pcell->w - 1, yc + pcell->h1 - 1);

    g.setColour(Colour(0xff151515));
    J_HLine(g, yc, pcell->x, pcell->x + pcell->w);
    J_HLine(g, yc + pcell->h1, pcell->x, pcell->x + pcell->w);

    g.setColour(Colour(78, 78, 78));
    J_HLine(g, yc + 1, pcell->x, pcell->x + pcell->w);
*/

    g.setColour(Colour(28, 60, 71).darker(0.1f));
    J_FillRect(g, x, yc, x + pcell->w - 1, yc + pcell->h1 - 1);
    
    g.setColour(Colour(20, 43, 50).darker(0.1f));
    J_FillRect(g, x, yc, x + pcell->w - 1, yc);
    
    g.setColour(Colour(32, 70, 81).darker(0.1f));
    J_VLine(g, x, yc + 1, yc + pcell->h1);
    J_VLine(g, x + pcell->w - 1, yc + 1, yc + pcell->h1);
    g.setColour(Colour(38, 82, 96).darker(0.1f));
    J_HLine(g, yc + 1, x, x + pcell->w - 1);


    if(pcell->pctype == PCType_MixSlider)
    {
        pcell->slider->Activate();
        if(pcell->param->type == Param_Default_Bipolar || pcell->param->type == Param_Pan)
            J_SliderCommon2(g, pcell->slider, pcell->x + 3, yc + 12, 80, 11, false);
        else
            J_SliderCommon(g, pcell->slider, pcell->x + 3, yc + 12, 80, 11, false);

        pcell->slider->y = yc + 4;
        pcell->slider->height = 20;

        g.setColour(Colour(0xff55B5E5));
        J_TextSmall_xy(g, pcell->x + 3, yc + 11, pcell->title);

        if(pcell->vstring != NULL)
        {
            //g.setColour(Colour(0xff55B5E5));
            g.setColour(Colour(0xff55CACA));
            J_ValueString(g, pcell->x + 90, yc + 22, pcell->vstring);
        }
    }
    else if(pcell->pctype == PCType_MixToggle)
    {
        J_ParamRadioToggle(g, pcell->toggle, x + 2, yc + 6, pcell->w - 4, 10);

        if(pcell->vstring != NULL)
        {
            g.setColour(Colour(0xff55B5E5));
            J_ValueString(g, x + 22, yc + 14, pcell->vstring);
        }
        else if(pcell->param != NULL)
        {
            g.setColour(Colour(0xff55B5E5));
            J_TextSmall_xy(g, x + 22, yc + 14, pcell->param->name);
        }
    }
    else if(pcell->pctype == PCType_MixRadio)
    {
        //g.setColour(Colour(0xffA5A5A5));
        //J_TextSmall_xy(g, x + 3, yc + 12, pcell->title);

        int yt = yc + 5;
        ToggleParam* tgp;
        Control* ct = pcell->ctrl_first;
        while(ct != NULL)
        {
            tgp = (ToggleParam*)ct;
            J_ParamRadioToggle(g, tgp, x + 2, yt, pcell->w - 4, 11);

            if(tgp->param->vstring != NULL)
            {
                g.setColour(Colour(0xff55B5E5));
                J_ValueString(g, x + 22, yt + 9, tgp->param->vstring);
            }

            yt += 12;
            ct = ct->lst_next;
        }
    }
}

void J_SamplesBackground(Graphics& g, int x, int y, int w, int h)
{
    g.setColour(Colour(0x109BFFFF));
    for(int xc = x; xc < x + w; xc += 2)
    {
        J_VLine(g, xc, y, y + h);
    }

    //for(int yc = y; yc < y + h; yc += 2)
    //{
    //    J_HLine(g, yc, x + 5, x + w);
    //}
}

void J_BrwBackground(Graphics& g, int x, int y, int w, int h, Colour clr)
{
    float a = 0.5f;
    for(int yc = y; yc < y + 8; yc++)
    {
        g.setColour(clr.withAlpha(a));
        J_HLine(g, yc, x + 5, x + w);
        J_HLine(g, y + h - (yc - y), x + 5, x + w);
        a /= 1.9f;
    }
}

void J_EmptyRects(Graphics& g, int x, int y, int w, int h, int eh, BrwMode bmode)
{
    Colour clr = Colours::black;
    switch(bmode)
    {
        case Browse_Files:
        {
            clr = Colour(0xff2F2F2F);
        }break;
        case Browse_Samples:
        {
            clr = Colour(0xff1A4535);
        }break;
        case Browse_Projects:
        {
            clr = Colour(0xff452515);
        }break;
        case Browse_Plugins:
        {
            clr = Colour(0xff32324A);
        }break;
        case Browse_Presets:
        {
            clr = Colour(0xff2A353A);
        }break;
        case Browse_Params:
        {
            clr = Colour(0xff2A353A);
        }break;
    }

    int yc = y;
    while(yc < h)
    {
        g.setColour(clr.darker(0.6f));
        g.fillRect(x + 1, yc, w - 2, eh - 1);
        g.setColour(clr);
        g.drawRect(x + 1, yc, w - 2, eh - 1);
        yc += eh;
    }
}

void J_EntryRect(Graphics& g, int x, int y, int w, int h, BrwMode bmode, bool delimiter = false)
{
    Colour clr = Colours::black;
    switch(bmode)
    {
        case Browse_Files:
        {
            clr = Colour(0xff2F2F2F);
            if(delimiter)
                clr = panelcolour.darker(0.1f);
        }break;
        case Browse_Samples:
        {
            clr = Colour(0xff1A4535);
        }break;
        case Browse_Projects:
        {
            clr = Colour(0xff553525);
        }break;
        case Browse_Plugins:
        {
            clr = Colour(0xff1F2F45);
        }break;
        case Browse_Presets:
        {
            clr = Colour(0xff25353A);
        }break;
    }

    if(bmode == Browse_Projects)
    {
        g.setColour(clr.darker(0.4f));
        g.fillRect(x + 1, y + 1, w - 2, h - 2);
        g.setColour(clr);
        g.drawRect(x + 1, y + 1, w - 2, h - 2);
    }
    else
    {
        g.setColour(clr);
        g.fillRect(x, y, w, h);
        g.setColour(clr.darker(0.6f));
        g.drawRect(x, y, w, h);
        g.setColour(clr.brighter(0.1f));
        J_HLine(g, y + 1, x + 1, x + w - 1);
    }
}

void J_Browser(Graphics& g, Browser* brw, int x, int y, int w, int h)
{
    brw->x = x;
    brw->y = y;
    brw->width = w;
    brw->height = h;
    brw->sbar->active = false;

    int xi = -1000, yi = -1000;

    brw->num_visible = h/brw->entry_height;

    g.setColour(panelcolour.darker(1).darker(1));
    g.fillRect(x, y, w - 1, h);
    /*
    g.setColour(Colour(0xff0A0A0A));
    int yrc = y + 1;
    while(yrc < y + h)
    {
        J_HLine(g, yrc, x + 1, x + w - 1);
        yrc += 2;
    }
    */

    g.saveState();
    g.reduceClipRegion(x, y, w, h);

/*    g.setColour(Colour(0x1fAAAAAA));
    for(int yc = y; yc < y + h; yc += 2)
    {
        J_Line(g, x, yc, x + w, yc);
    }*/

    //J_PanelRect1(g, x, y - 1, x + 4, y + h + 1);
    g.setColour(Colour(20, 64, 96).brighter(0.2f).withAlpha(0.6f));
    J_VLine(g, x, y - 1, y + h + 1);
    g.setColour(Colour(20, 64, 96).brighter(0.2f));
    J_VLine(g, x + 1, y - 1, y + h + 1);
    if(brw != NULL)
    {
        int index = 0;
        int xo = x + 5;
        if(brw->brwmode == Browse_Files || 
           brw->brwmode == Browse_Plugins ||
           brw->brwmode == Browse_Projects ||
           brw->brwmode == Browse_Samples)
        {
            /*
            if(brw->brwmode == Browse_Samples)
                J_BrwBackground(g, x, y, w - 1, h, Colour(0xff8AF8FF));
            else if(brw->brwmode == Browse_Plugins)
                J_BrwBackground(g, x, y, w - 1, h, Colour(0xff8AFAAA));
            else if(brw->brwmode == Browse_Files)
                J_BrwBackground(g, x, y, w - 1, h, Colour(0xffFAFAFA));
            else if(brw->brwmode == Browse_Projects)
                J_BrwBackground(g, x, y, w - 1, h, Colour(0xffFA8AFA));
            */

            char dname[MAX_NAME_STRING];
            int yc = y + 11 + (int)brw->main_offs;
            BrwEntry* be = brw->entry_first;
            FileData* fd;
            while(be != NULL)
            {
                fd = (FileData*)be->entry;
                if(fd->ftype == FType_Delimiter)
                {
                    fd->list_index = index;
                    index++;
                    if(yc < y + h + 12 && yc > y - 14)
                    {
                        J_EntryRect(g, xo - 3, yc - 11, w - 3, BrwEntryHeight + 1, brw->brwmode, true);

                        /*
                        int baroffs = 0;
                        g.setColour(Colour(0x379BFFFF));
                        J_FillRect(g, xo + 1, yc - 11, xo + w - baroffs - 5, yc + 2);
                        g.setColour(Colour(0xff374B73));
                        J_TextDix_xy(g, xo + 2, yc + 1, fd->name);
                        */
                    }
                    yc += BrwEntryHeight;
                }
                else if((fd->ftype == FType_Directory || 
                            fd->ftype == FType_LevelDirectory ||
                            fd->ftype == FType_DiskSelector ||
                            fd->ftype == FType_DiskDrive)
                        && brw->ItemIsVisibleWithSorting(fd->name))
                {
                    fd->list_index = index;
                    if(brw->current_index == -1 && brw->current_fd == fd)
                    {
                        brw->current_index = index;
                    }
                    index++;

                    if(yc < y + h + 12 && yc > y - 14)
                    {
                        if(brw->current_index == fd->list_index)
                        {
                            xi = xo;
                            yi = yc;
                        }

                        if(fd->ftype == FType_Directory)
                            J_EntryRect(g, xo - 3, yc - 11, w - 3, BrwEntryHeight + 1, brw->brwmode);
                        else
                            J_EntryRect(g, xo - 3, yc - 11, w - 3, BrwEntryHeight + 1, brw->brwmode, true);

                        if(fd->ftype == FType_Directory)
                        {
                            strcpy(dname, " [");
                            strcat(dname, fd->name);
                            strcat(dname, "]");
                            g.setColour(Colour(0xffDFDFDF));
                            J_String_Dix_Ranged(g, xo, yc + 1, dname, w - 20);
                        }
                        else
                        {
                            if(fd->ftype == FType_LevelDirectory)
                            {
                                if(fd->list_index > 1) // Not for drive letter
                                {
                                    //strcpy(dname, "\\");
                                    strcpy(dname, fd->name);
                                }
                                else
                                    strcpy(dname, fd->name);
                                strcat(dname, "\\");
                            }
                            else
                                strcpy(dname, fd->name);
                            g.setColour(Colour(0xffEFEFEF));
                            J_String_Dix_Ranged(g, xo, yc + 1, dname, w - 20);
                        }
                    }
                    yc += BrwEntryHeight;
                }

                be = be->next;
            }

            be = brw->entry_first;
            while(be != NULL)
            {
                fd = (FileData*)be->entry;
                if(fd->ftype & brw->viewmask && brw->ItemIsVisibleWithSorting(fd->name))
                {
                    fd->list_index = index;
                    if(brw->current_index == -1 && brw->current_fd == fd)
                    {
                        brw->current_index = index;
                    }
                    index++;

                    if(yc < y + h + 12 && yc > y - 14)
                    {
                        if(brw->current_index == fd->list_index)
                        {
                            xi = xo;
                            yi = yc;
                        }

                        J_EntryRect(g, xo - 3, yc - 11, w - 3, BrwEntryHeight + 1, brw->brwmode);

                        if(fd->ftype == FTYPE_UNKNOWN)
                        {
                            g.setColour(Colour(0xff787878));
                        }
                        else if(fd->ftype == FType_Wave)
                        {
                            g.setColour(Colour(0xef82EAB2));
                        }
                        else if(fd->ftype == FType_VST)
                        {
                            g.setColour(Colour(0xff8A8AEF));
                        }
                        else if(fd->ftype == FType_Native)
                        {
                            g.setColour(Colour(0xffCACAFF));
                        }
                        else if(fd->ftype == FType_Projects)
                        {
                            //g.setColour(Colour(165, 175, 185));
                        }

                        if(brw->brwmode == Browse_Files)
                        {
                            strcpy(dname, " ");
                            strcat(dname, fd->name);
                        }
                        else
                            strcpy(dname, fd->name);
                        if(fd->ftype == FType_Projects)
                        {
                            g.setColour(Colour(245, 195, 195));
                            J_String_Rox_Ranged(g, xo, yc + 1, dname, w - 20);
                        }
                        else
                        {
                            if(brw->brwmode == Browse_Files)
                            {
                                J_String_Dix_Ranged(g, xo, yc + 1, dname, w - 20);
                            }
                            else
                                J_String_Instr_Ranged(g, xo, yc + 1, dname, w - 20);
                        }
                    }

                    yc += BrwEntryHeight;
                }
                be = be->next;
            }

            J_EmptyRects(g, xo - 3, yc - 10, w - 3, y + h, BrwEntryHeight, brw->brwmode);

            brw->sbar->full_len = (float)(yc - (11 + y + brw->main_offs));
            brw->sbar->offset = brw->sbar->actual_offset = -(float)(brw->main_offs);
            brw->sbar->visible_len = (float)(h - 2);
            if(brw->sbar->visible_len > brw->sbar->full_len)
            {
                brw->sbar->active = false;
            }
            else
            {
                brw->sbar->active = true;
            }
        }
        else if(brw->brwmode == Browse_Params)
        {
            //g.setColour(Colour(55, 55, 55));
            //J_FillRect(g, xo - 2, y - 23, xo + w - 7, y + h);
            int yc = y + (int)brw->main_offs;
            Paramcell* pd;
            BrwEntry* be = brw->entry_first;
            while(be != NULL)
            {
                pd = (Paramcell*)be->entry;
                if(pd->visible && 
                    (pd->param == NULL || brw->ItemIsVisibleWithSorting(pd->param->name)))
                {
                    if(yc < (y + h) && yc > (y - 23))
                    {
                        pd->Enable();

						pd->x = xo - 2;
                        pd->y = yc;
                        pd->w = w - 5;

                        J_BrwParamCell(g, pd, xo - 2, yc);

                        pd->drawarea->SetBounds(xo - 1, yc, w - 10, pd->h1);
                    }
                    else
                    {
                        pd->Disable();
                    }

                    yc += pd->h1;
                }
                be = be->next;
            }

            J_EmptyRects(g, xo - 3, yc + 1, w - 3, y + h, 20, brw->brwmode);

            brw->sbar->full_len = (float)(yc - (y + brw->main_offs) + 1);
            brw->sbar->offset = brw->sbar->actual_offset = -(float)(brw->main_offs);
            brw->sbar->visible_len = (float)h;
            if(brw->sbar->visible_len > brw->sbar->full_len)
            {
                brw->sbar->active = false;
            }
            else
            {
                brw->sbar->active = true;
            }

            be = brw->entry_first;
            while(be != NULL)
            {
                pd = (Paramcell*)be->entry;
                if(pd->drawarea->isEnabled() == true)
                {
                    pd->drawarea->SetBounds(pd->drawarea->ax, 
                                            pd->drawarea->ay, 
                                            pd->drawarea->aw, 
                                            pd->drawarea->ah,
                                            xo - 1,
                                            y,
                                           (brw->sbar->active == true) ? x + w - 6 - brw->sbar->tolschina : x + w,
                                            y + h - 1);
                }
                be = be->next;
            }
        }
        else if(brw->brwmode == Browse_Presets)
        {
            int idx = 0;
            int yc = y + 11 + (int)brw->main_offs;
            BrwEntry* be = brw->entry_first;
            PresetData* prd;

            //J_BrwBackground(g, x, y, w - 1, h, Colour(0xffFFCA9A));
            //glColor3ub(240, 0, 80);
            //text::print_simple(hint_font, (GLfloat)xo+3, (GLfloat)yc + 12, "----- Native Presets -----");
            //yc += 14;
            while (be != NULL)
            {
                if(be->type == BEType_Delimiter)
                {
                    if(strcmp(be->rame, "Native presets:") == 0)
                    {
                        if(yc < y + h + 12 && yc > y)
                        {
                            g.setColour(Colour(0xff64E1FF));
                            J_TextBase_xy(g, xo, yc + 1, be->rame);
                        }
                        yc += BrwEntryHeight;
                    }
                }
                else
                {
                    prd = (PresetData*) (be->entry);
                    if (prd->pPreset->native == true && brw->ItemIsVisibleWithSorting(prd->name))
                    {
                        prd->list_index = idx++;
                        if(brw->current_index == -1 && brw->current_prd == prd)
                        {
                            brw->current_index = prd->list_index;
                        }

                        if(yc < y + h + 12 && yc > y)
                        {
                            J_EntryRect(g, xo - 3, yc - 11, w - 3, BrwEntryHeight + 1, brw->brwmode);

                            if(brw->current_index == prd->list_index)
                            {
                                xi = xo;
                                yi = yc;
                                g.setColour(Colour(0xff8AEAFA));
                            }
                            else
                                g.setColour(Colour(0xff6ABAEA));

                            J_TextProj_xy(g, xo, yc + 1, prd->name);
                        }
                        yc += BrwEntryHeight;
                    }
                }
                be = be->next;
            }

            J_EmptyRects(g, xo - 3, yc - 10, w - 3, y + h, BrwEntryHeight, brw->brwmode);

            brw->sbar->full_len = (float)(yc - (11 + y + brw->main_offs));
            brw->sbar->offset = brw->sbar->actual_offset = -(float)(brw->main_offs);
            brw->sbar->visible_len = (float)(h - 2);
            if(brw->sbar->visible_len > brw->sbar->full_len)
            {
                brw->sbar->active = false;
            }
            else
            {
                brw->sbar->active = true;
            }
        }

        if(brw->sbar->active == true)
        {
            //g.setColour(Colour(0xff000000));
            //J_FillRect(g, x + w - brw->sbar->tolschina, y, x + w - 1, y + h - 1);

            J_ScrollBard(g, y - 1, y + h, x + w - brw->sbar->tolschina - 3, brw->sbar, false, false);

            g.setColour(Colour(10, 84, 96));
            J_VLine(g, x + w - brw->sbar->tolschina - 2, y - 1, y + h);
            g.setColour(Colour(18, 23, 28));
            J_VLine(g, x + w - 2, y - 1, y + h);
        }
    }

    if(xi > -1000 && yi > -1000)
    {
        int baroffs = brw->sbar->active == true ? brw->sbar->tolschina : 0;
        g.saveState();
        g.reduceClipRegion(x, y, w - baroffs - 2, h);
        baroffs = 0;
        g.setColour(Colour(0x3cFFFFFF));
        J_FillRect(g, xi - 1, yi - 9, xi + w - baroffs - 9, yi + 1);
        g.setColour(Colour(0x4AFFFFFF));
        J_LineRect(g, xi - 2, yi - 10, xi + w - baroffs - 8, yi + 2);
        g.restoreState();
    }

    g.restoreState();
}

void J_GenBrwButtons(Graphics& g, int x, int y, int xr, int yr, int wr, int hr)
{
    //J_DarkRect(g, x, y, x + 128, y + 23);
    //J_DarkLine(g, x, y, x + 128, y + 23);
    J_ButtonRanged(g, genBrw->ShowFiles, x + 2, y + 1, 20, 20, xr, yr, xr + wr, yr + hr);
    J_ButtonRanged(g, genBrw->ShowProjects, x + 2 + 21, y + 1, 20, 20, xr, yr, xr + wr, yr + hr);
    J_ButtonRanged(g, genBrw->ShowExternalPlugs, x + 2 + 42, y + 1, 20, 20, xr, yr, xr + wr, yr + hr);
    //J_Button(g, gB->ShowNativePlugs, x + 2 + 63, y + 2, 20, 20);
    J_ButtonRanged(g, genBrw->ShowSamples, x + 2 + 63, y + 1, 20, 20, xr, yr, xr + wr, yr + hr);
    J_ButtonRanged(g, genBrw->ShowParams, x + 2 + 84, y + 1, 20, 20, xr, yr, xr + wr, yr + hr);
    J_ButtonRanged(g, genBrw->ShowPresets, x + 2 + 105, y + 1, 20, 20, xr, yr, xr + wr, yr + hr);
}

void J_InstrCenter(Graphics& g, int xb, int yb)
{
    int gbwidth = MainX1 - InstrPanelWidth - InstrCenterOffset + 1;
    if(MainX1 - InstrPanelWidth - 3 > 0)
    {
        g.saveState();
        g.reduceClipRegion(0, yb, gbwidth, WindHeight - yb);

        J_PanelRect(g, xb, yb, xb + GenBrowserWidth + 3, yb + brw_heading);

        //GradientBrush gb(Colour(0xF510FFAF), float(xb), float(yb), 
        //                  Colour(0x0010FFAF), float(xb), float(yb + brw_heading),
        //                  false);
        //g.setBrush(&gb);
        //g.fillRect(xb, yb, GenBrowserWidth, brw_heading);

        J_LRect(g, xb, yb, xb + GenBrowserWidth - 1, yb + brw_heading);

        genBrw->Enable();
        J_GenBrwButtons(g, xb + GenBrowserWidth - 127, yb + 2, xb, yb, gbwidth - 1, brw_heading);
        g.setColour(Colour(0, 0, 0));
        int xt = xb + 1;
        switch(genBrw->brwmode)
        {
            case Browse_Params:
                J_TextSmall_xy(g, xt + 1, yb + 12, "Params");
                g.setColour(Colour(255, 255, 255));
                J_TextSmall_xy(g, xt, yb + 11, "Params");
                break;
            case Browse_Presets:
                J_TextSmall_xy(g, xt + 1, yb + 12, "Presets");
                g.setColour(Colour(255, 255, 255));
                J_TextSmall_xy(g, xt, yb + 11, "Presets");
                break;
            case Browse_Files:
                J_TextSmall_xy(g, xt + 1, yb + 12, "Files");
                g.setColour(Colour(255, 255, 255));
                J_TextSmall_xy(g, xt, yb + 11, "Files");
                break;
            case Browse_Projects:
                J_TextSmall_xy(g, xt + 1, yb + 12, "Projects");
                g.setColour(Colour(255, 255, 255));
                J_TextSmall_xy(g, xt, yb + 11, "Projects");
                break;
            case Browse_Plugins:
                J_TextSmall_xy(g, xt + 1, yb + 12, "Plugins");
                g.setColour(Colour(255, 255, 255));
                J_TextSmall_xy(g, xt, yb + 11, "Plugins");
                break;
            case Browse_Samples:
                J_TextSmall_xy(g, xt + 1, yb + 12, "Samples");
                g.setColour(Colour(255, 255, 255));
                J_TextSmall_xy(g, xt, yb + 11, "Samples");
                break;
        }

        J_Browser(g, genBrw, xb, yb + brw_heading + 1, gbwidth, WindHeight - (yb + brw_heading + 1));

        //g.setColour(Colour(59, 95, 122));
        //J_VLine(g, xb + GenBrowserWidth + 1, yb + brw_heading + 1, WindHeight);
        //g.setColour(Colour(43, 66, 85));
        //J_VLine(g, xb + GenBrowserWidth + 2, yb + brw_heading + 1, WindHeight);

        /*
        g.setColour(Colour(10, 84, 96));
        J_LineRect(g, xb, yb + brw_heading + 1, xb + gbwidth + 1, yb + brw_heading + 1 + 17);
        if(strlen(genBrw->sortstr->string) == 0)
        {
            g.setColour(Colour(33, 46, 56));
        }
        else
        {
            g.setColour(Colour(50, 20, 10));
        }
        J_FillRect(g, xb + 1, yb + brw_heading + 2, xb + gbwidth, yb + brw_heading + 17);
        g.setColour(Colour(244, 244, 244));
        J_TString_Base_xy(g, xb + 2, yb + brw_heading + 17, genBrw->sortstr);
        genBrw->sortstr->edx = gbwidth - 4;
        genBrw->sortstr->edy = 12;
        */

        g.restoreState();
    }
    else
    {
        genBrw->Disable();
    }
}

void J_InstrVU(Graphics& g, int xv, int yv, VU* vu)
{
    vu->x = xv;
    vu->y = yv;

    int h = 18;
    int w = 25;
    Colour clron;
    Colour clroff = Colour(24, 24, 24);

    // Background panel color
    g.setColour(Colour(24, 24, 24));
    g.drawRect(xv, yv - h + 2, w + 1, h - 1);
    if(vu->instr == NULL)
    {
        g.setColour(Colour(0xff412525));
    }
    else if(vu->instr->type == Instr_Sample)
    {
        clroff = Colour(95, 107, 111);
    }
    else
    {
        clroff = Colour(71, 113, 137);
    }

    g.setColour(Colour(24, 24, 24));
    g.fillRect(xv + 1, yv - h + 3, w, h - 2);

    float l = vu->getL();
    float r = vu->getR();
    float lr = (l + r)/2.f;
    //lr = pow(lr, 0.25f);

    int hO = int(lr*30);
    int hc = 0;

    if(vu->instr == NULL)
    {
        clron = Colour(0xffAFAFAF).withAlpha(0.9f);
    }
    else if(vu->instr->type == Instr_Sample)
    {
        clron = Colour(0xffd5e1ef).withAlpha(0.9f);
    }
    else
    {
        clron = Colour(0xff6FdFeF).withAlpha(0.9f);
    }

    for(int yy = yv - 1; yy > yv - h + 1; yy -= 2)
    {
        if(hO > 0 && hc <= hO)
        {
            g.setColour(clron);
            J_HLine(g, yy, xv + 1, xv + w + 1);
        }
        else
        {
            g.setColour(clroff);
            J_HLine(g, yy, xv + 1, xv + w + 1);
        }
        hc += 2;
    }
}

void J_InstrVU1(Graphics& g, int xv, int yv, VU* vu)
{
    vu->x = xv;
    vu->y = yv;

    int h = 18;
    int w = 25;

    // Background panel color
    g.setColour(Colour(24, 24, 24));
    g.drawRect(xv, yv - h + 2, w + 1, h - 1);
    if(vu->instr == NULL)
    {
        g.setColour(Colour(0xff412525));
    }
    else if(vu->instr->type == Instr_Sample)
    {
        g.setColour(Colour(58, 92, 96));
    }
    else
    {
        g.setColour(Colour(56, 81, 109));
    }

    g.fillRect(xv + 1, yv - h + 3, w, h - 2);

    float l = vu->getL();
    float r = vu->getR();
    float lr = (l + r)/2.f;
    //lr = pow(lr, 0.25f);

    int hO = int(lr*30);
    int hc = 0;

    Colour clron;
    Colour clroff = Colour(24, 24, 24);
    if(vu->instr == NULL)
    {
        clron = Colour(0xffAFAFAF);
    }
    else if(vu->instr->type == Instr_Sample)
    {
        clron = Colour(0xff7FFFAF);
    }
    else
    {
        clron = Colour(0xff7FEFFF);
    }

    g.setColour(clroff);
    for(int yy = yv - 1; yy > yv - h + 1; yy -= 2)
    {
        //if(hO > 0 && hc <= hO)
        {
            //g.setColour(clron);
            J_HLine(g, yy + 1, xv + 1, xv + w + 1);
        }
        //else
        //{
        //    g.setColour(clroff);
        //    J_HLine(g, yy, xv + 1, xv + w + 1);
        //}
        hc += 2;
    }

    g.setColour(clron.withAlpha(jlimit(0.f, 1.f, lr)));
    for(int yy = yv - 1; yy > yv - h + 1; yy -= 2)
    {
        //if(hO > 0 && hc <= hO)
        {
            //g.setColour(clron);
            J_HLine(g, yy, xv + 1, xv + w + 1);
        }
        //else
        //{
        //    g.setColour(clroff);
        //    J_HLine(g, yy, xv + 1, xv + w + 1);
        //}
        hc += 2;
    }
}

int J_Instr(Graphics& g, int x, int y, Instrument* instr)
{
    int height;
    g.setColour(Colour(0xffFFFFFF));
    if(instr->folded == true)
    {
        height = InstrFoldedHeight;
        g.drawImageAt(img_instrclosed, x + 2, y + 2, false);
    }
    else
    {
        height = InstrUnfoldedHeight;
        if(instr->type == Instr_Sample)
            g.drawImageAt(img_instropened1, x + 2, y + 2, false);
        else
            g.drawImageAt(img_instropened, x + 2, y + 2, false);
    }

    instr->x = x;
    instr->y = y;
    instr->width = InstrCellWidth - 2;
    instr->height = height;

    // Shadow probably isn't needed
    //g.setColour(Colour(0xff000000));
    //J_String_Base_Ranged(g, x + 7, y + InstrFoldedHeight - 1, ((Sample*)instr)->sample_name, InstrAliasOffset - 21);

    if(instr->type == Instr_Sample)
    {
        g.setColour(Colour(0xffE5E5E5));
    }
    else
    {
        g.setColour(Colour(0xff9FCFFF));
    }
    J_String_Instr_Ranged(g, x + 5, y + InstrFoldedHeight - 3, instr->name, InstrAliasOffset - 16);

    int bx1 = MainX1 - 1 - InstrPanelWidth;
    int by1 = MainY1 + brw_heading;
    int bx2 = MainX1 - 3;
    int by2 = MainY1 + InstrPanelHeight;
    IP->rx1 = bx1;
    IP->ry1 = by1;
    IP->rx2 = bx2;
    IP->ry2 = by2;

    int alx = x + InstrAliasOffset - 10;
    int aly = y + InstrFoldedHeight - 1;
    g.saveState();
    g.reduceClipRegion(alx, aly - 13, 58, 14);
    instr->alias->Activate(alx, aly - 13, 58, 14, bx1, by1, bx2, by2);
    instr->alias->edx = 39;
    instr->alias->edy = 12;

    if(instr->type == Instr_Sample)
    {
        g.setColour(smpcolour);
    }
    else
    {
        g.setColour(gencolour);
    }
    //g.setColour(Colour(0xff6496FF));
    int aw = J_TString_Instr_xy(g, alx, y + InstrFoldedHeight - 3, instr->alias);
    g.restoreState();

    if((x + 94) >= 0)
    {
        instr->awidth = aw;
    }

    J_DigitStr_xy(g, x + instr->width - 26, y + InstrFoldedHeight - 2, instr->dfxstr, true);
    instr->dfxstr->Activate(instr->dfxstr->x - 1, instr->dfxstr->y - 11, instr->dfxstr->edx, instr->dfxstr->edy,
                            bx1, by1, bx2, by2);

    if(instr == current_instr)
    {
        g.setColour(Colour(0xffFF5F30));
        J_LineGRect(g, x + 2, y + 3, x + InstrPanelWidth - 1, y + height + 1);
        g.setColour(Colour(0x7fFF5F30));
        J_VLine(g, x + 4, y + 5, y + 21);
        J_HLine(g, y + 5, x + 4, x + 97);
        J_HLine(g, y + 17, x + 4, x + 95);
        J_LineGRect(g, x + 2, y + 2, x + InstrPanelWidth - 1, y + height + 2);

        g.setColour(Colour(0xbfFF5F30));
        J_HLine(g, y + 18, x + 4, x + 95);

        J_VLine(g, x + InstrPanelWidth - 2, y + 3, y + height + 2);

        //g.setColour(Colour(0x5fFF5F30));
        //J_LineCRect(g, x + 1, y + 1, x + InstrPanelWidth - 17, y + height + 3);

        //g.setColour(Colour(0x7fFF5F30));
        //J_LineRect(g, x + 3, y + 4, x + InstrPanelWidth - 18, y + height + 1);

        g.setColour(Colour(0xffFFFFFF));
    }

    if((M.mmode & MOUSE_DRAG_N_DROPPING && (M.drag_data.drag_type == Drag_Instrument_File)) || 
        (M.dropinstr1 != current_instr && M.dropinstr2 != current_instr))
    {
        if(instr == M.dropinstr1)
        {
            g.setColour(Colour(0xffFFFF30));
            J_Line(g, x + 3, y + height + 3, x + InstrPanelWidth - 1, y + height + 3);
            g.setColour(Colour(0x7fFFFF30));
            J_Line(g, x + 3, y + height + 2, x + InstrPanelWidth - 1, y + height + 2);
            g.setColour(Colour(0x3fFFFF30));
            J_Line(g, x + 3, y + height + 1, x + InstrPanelWidth - 1, y + height + 1);
        }
        else if(instr == M.dropinstr2)
        {
            g.setColour(Colour(0xffFFFF30));
            J_Line(g, x + 3, y + 1, x + InstrPanelWidth - 1, y + 1);
            g.setColour(Colour(0x7fFFFF30));
            J_Line(g, x + 3, y + 2, x + InstrPanelWidth - 1, y + 2);
            g.setColour(Colour(0x3fFFFF30));
            J_Line(g, x + 3, y + 3, x + InstrPanelWidth - 1, y + 3);
        }
    }

    if(instr->folded == false)
    {
        J_InstrVU(g, x + 3, y + InstrUnfoldedHeight + 2, instr->vu);
        J_SliderInstrPan(g, instr->mpan, instr, x + 94, y + 27 + 1, false);
        J_SliderInstrVol(g, instr->mvol, instr, x + 94, y + 17);
        g.setColour(Colour(38, 38, 38));
        g.fillRect(x + instr->width - 114, y + 21, 26, 15);
        J_MuteToggle(g, instr->mute, x + instr->width - 114, y + 22);
        J_SoloToggle(g, instr->solo, x + instr->width - 101, y + 22);
        //J_AutoToggle(g, instr->autopt, x + instr->width - 92, y + 20);
        g.setColour(Colour(38, 38, 38));
        g.fillRect(x + 29, y + 19, 26, 17);
        J_WindowToggle(g, instr->pEditButton, x + 30, y + InstrFoldedHeight + 4);

        instr->Enable();
        instr->vu->drawarea->SetBounds(x + 4, y + InstrUnfoldedHeight - 17, 28, 22,
                                       bx1, by1, bx2, by2);
        instr->mute->Activate(instr->mute->x, instr->mute->y, instr->mute->width, instr->mute->height,
                              bx1, by1, bx2, by2);
        instr->solo->Activate(instr->solo->x, instr->solo->y, instr->solo->width, instr->solo->height,
                              bx1, by1, bx2, by2);
        instr->autopt->Activate(instr->autopt->x, instr->autopt->y, instr->autopt->width, instr->autopt->height,
                              bx1, by1, bx2, by2);
        instr->mvol->Activate(instr->mvol->x - 5, instr->mvol->y - 1, instr->mvol->width + 5 + 5, instr->mvol->height + 3,
                              bx1, by1, bx2, by2);
        instr->mpan->Activate(instr->mpan->x - 5, instr->mpan->y, instr->mpan->width + 5 + 5, instr->mpan->height + 2,
                              bx1, by1, bx2, by2);
        instr->pEditButton->Activate(instr->pEditButton->x, instr->pEditButton->y, instr->pEditButton->width, instr->pEditButton->height,
                              bx1, by1, bx2, by2);
    }
    else
    {
        instr->Disable();
    }

    instr->instr_drawarea->SetBounds(instr->x, instr->y, instr->width + 1, instr->height + 2,
                                     bx1, by1, bx2, by2);

    // debug stuff
    //g.setColour(Colour(0x8fFFFF30));
    //g.fillRect(instr->x, instr->y, instr->width, instr->height);

    // Folding is oficially cancelled
    //J_InstrFoldToggle(g, instr->fold, instr, x + 2, y + InstrFoldedHeight - 9);
    return height;
}

void J_InstrPanelButtons(Graphics& g, int x, int y)
{
    //J_DarkRect(g, x, y, x + 34, y + 23);
    //J_Button(g, IP->fold_all, x + 2, y + 2, 28, 20);
    //J_Button(g, IP->expand_all, x + 2 + 28 + 1, y + 2, 28, 20);
    //J_Button(g, IP->shut_all_windows, x + 2 + 58, y + 2, 44, 20);
    //J_Button(g, IP->shut_all_windows, x + 2, y + 2, 37, 20);
    //J_Button(g, IP->edit_autopattern, x + 2, y + 2, 31, 20);

    J_Button(g, CP->AccInstrPanel, x + InstrPanelWidth - 60, y + 3, 23, 17);
    J_Button(g, CP->AccGenBrowsa, x + InstrPanelWidth - 30, y + 3, 23, 17);
}

void J_InstrPanel(Graphics& g, int x, int y)
{
    IP->x = x;
    IP->y = y;

    J_PanelRect(g, x, y, x + InstrPanelWidth - 2, y + brw_heading);
    //GradientBrush gb(Colour(0x3510FFAF), float(x), float(y), 
    //                  Colour(0x0010FFAF), float(x), float(y + brw_heading),
    //                  false);
    //g.setBrush(&gb);
    //g.fillRect(x, y, InstrPanelWidth, brw_heading);

    J_LRect(g, x, y, x + InstrPanelWidth - 3, y + brw_heading);

    g.setColour(panelcolour.darker(1).darker(1));
    J_FillRect(g, x, y + brw_heading + 1, x + InstrPanelWidth - 2, y + InstrPanelHeight + 2);
    J_InstrPanelButtons(g, x + 1, y + 2);

    g.saveState();
    g.reduceClipRegion(x - 2, y + brw_heading + 1, InstrPanelWidth + 2, InstrPanelHeight - 1);

    int instroffs;
    int yoffs = brw_heading + IP->main_offs - 1;

    Instrument* i = first_instr;
    while(i != NULL)
    {
        if(i->to_be_deleted == false && i->temp == false)
        {
            if(i->folded == true)
            {
                instroffs = InstrFoldedHeight;
            }
            else
            {
                instroffs = InstrUnfoldedHeight;
            }

            if(((yoffs + instroffs) > 0 && (yoffs + instroffs) < InstrPanelHeight)||(yoffs > 0 && yoffs < InstrPanelHeight))
            {
                J_Instr(g, x - 2, y + yoffs, i);
            }
            else
            {
                i->Disable();
            }

            yoffs += instroffs;
            yoffs += 1;
        }
        i = i->next;
    }
    int fulllen = yoffs - (brw_heading + IP->main_offs) + 20;

    yoffs += 1;
    g.setColour(Colour(48, 48, 48));
    while(yoffs < InstrPanelHeight)
    {
        g.drawImageAt(img_instrempty, x + 1, y + yoffs + 2, false);
        //J_PanelRect(g, x + 1, y + yoffs + 2, x + 1 + 159, y + yoffs + 2 + 33);
        //J_HLine(g, y + yoffs + InstrUnfoldedHeight + 1, x + 1, x + InstrPanelWidth - 19);
        yoffs += InstrUnfoldedHeight + 1;
    }

/*
    IP->sbar->full_len = (float)fulllen;
    IP->sbar->offset = -(float)IP->main_offs;
    IP->sbar->visible_len = (float)(InstrPanelHeight - brw_heading);
    if(IP->sbar->visible_len >= IP->sbar->full_len)
    {
        IP->sbar->active = false;
    }
    else
    {
        IP->sbar->active = true;
    }

    J_ScrollBard(g, y + brw_heading, y + GenBrowserHeight + 2, x + InstrPanelWidth - 20, IP->sbar, false, false);
*/
    g.restoreState();
}

void J_InstrPanel(Graphics& g)
{
    J_InstrPanel(g, IP->x, IP->y);
}

void J_LineToggle(Graphics& g, int x, int y, Toggle* fold, LaneType ltype)
{
    fold->x = x;
    fold->y = y;
    fold->width = 20;
    fold->height = 11;

    if(*(fold->state) == false)
    {
        g.setColour(Colour(0xff0A379B));
    }
    else
    {
        g.setColour(Colour(0xffFFFFFF));
    }
    J_LineRect(g, x, y, x + 19, y + 10);
    //J_TextBase_xy(x + 2, y + 10, "");
}

void J_MixFoldToggle(Graphics& g, int x, int y, Toggle* mtg)
{
    mtg->x = x ;
    mtg->y = y;

    if(*(mtg->state) == true)
    {
        g.drawImageAt(img_mfold1, x, y, false);
    }
    else
    {
        g.drawImageAt(img_mfold2, x, y, false);
    }

    mtg->width = 15;
    mtg->height = 9;
}

void J_MixBypassToggle(Graphics& g, int x, int y, Toggle* btg)
{
    btg->x = x;
    btg->y = y;
    btg->width = 7;
    btg->height = 11;

    if(*(btg->state) == true)
    {
        g.drawImageAt(img_byp2, x, y, false);
    }
    else
    {
        g.drawImageAt(img_byp1, x, y, false);
    }
}

void J_EffBypassToggle(Graphics& g, int x, int y, Toggle* btg)
{
    btg->x = x;
    btg->y = y;
    btg->width = 15;
    btg->height = 8;

    g.setColour(Colour(0xffFFFFFF));
    if(*(btg->state) == false)
    {
        g.drawImageAt(img_effbyp2, x, y, false);
    }
    else
    {
        g.drawImageAt(img_effbyp1, x, y, false);
    }
}

int J_EffParamCell(Graphics& g, Paramcell* pcell, int x, int y, int w)
{
    pcell->x = x;
    pcell->y = y;

    pcell->w = w;


    g.setColour(Colour(28, 60, 71).darker(0.1f));
    J_FillRect(g, x, y, x + w, y + pcell->h - 1);
    
    g.setColour(Colour(20, 43, 50).darker(0.1f));
    J_FillRect(g, x, y, x + w, y);
    
    g.setColour(Colour(32, 70, 81).darker(0.1f));
    J_Line(g, x, y + 1, x, y + pcell->h);
    J_Line(g, x + w, y + 1, x + w, y + pcell->h);
    g.setColour(Colour(38, 82, 96).darker(0.1f));
    J_HLine(g, y + 1, x, x + w);

    if(pcell->pctype == PCType_MixSlider)
    {
        if(pcell->param->type == Param_Default_Bipolar)
            J_SliderCommon2(g, pcell->slider, x + 2, y + 11, 90, 10);
        else
            J_SliderCommon(g, pcell->slider, x + 2, y + 11, 90, 10);

        g.setColour(Colour(0xcf55B5C5));
        int nw = J_TextSmall_xy(g, x + 3, y + 10, pcell->param->name);
        nw += J_TextSmall_xy(g, x + 3 + nw, y + 10, ":");

        g.setColour(Colour(0xff55CACA));
        J_ValueString(g, x + 3 + nw + 2, y + 10, pcell->vstring);

        pcell->slider->y = y;
        pcell->slider->height = pcell->h;
    }
    else if(pcell->pctype == PCType_MixToggle)
    {
        J_ParamRadioToggle(g, pcell->toggle, x + 1, y + 3, w - 4, 10);

        if(pcell->vstring != NULL)
        {
            g.setColour(Colour(0xcf55B5C5));
            J_ValueString(g, x + 20, y + 11, pcell->vstring);
        }
    }
    else if(pcell->pctype == PCType_MixRadio)
    {
        //g.setColour(Colour(0xffE5E5E5));
        //J_TextSmall_xy(g, x + 3, y + 10, pcell->title);

        int yt = y + 3;
        ToggleParam* tgp;
        Control* ct = pcell->ctrl_first;
        while(ct != NULL)
        {
            tgp = (ToggleParam*)ct;
            J_ParamRadioToggle(g, tgp, x + 1, yt, w - 4, 11);

            if(tgp->param->vstring != NULL)
            {
                g.setColour(Colour(0xcf55B5C5));
                J_ValueString(g, x + 21, yt + 8, tgp->param->vstring);
            }

            yt += 11;
            ct = ct->lst_next;
        }
    }

    return pcell->h;
}

int J_EffParamCells_Aux(Graphics& g, Eff* eff, int x, int y, int w, MixChannel* mchan)
{
    Paramcell* pcell = eff->first_pcell;
    int yc = y;
    int cellhgt;
	int hgt = 0;
    while(pcell != NULL)
    {
        if(pcell->visible)
        {
            cellhgt = J_EffParamCell(g, pcell, x, yc, w);
            pcell->drawarea->SetBounds(x, yc, w, cellhgt, mchan->x1r, mchan->ry1, 
                                                          mchan->x2r, mchan->ry2 - 2);
            if(pcell->drawarea->isEnabled() == false)
            {
                pcell->Disable();
            }
            else
            {
                pcell->Enable();
            }

            yc += cellhgt;
            hgt += cellhgt;
        }
        pcell = pcell->next;
    }

    return hgt;
}

int J_EffParamCells(Graphics& g, Eff* eff, int x, int y, int w)
{
    Paramcell* pcell = eff->first_pcell;
    int yc = y;
    int cellhgt;
	int hgt = 0;
    while(pcell != NULL)
    {
        if(pcell->visible)
        {
            cellhgt = J_EffParamCell(g, pcell, x, yc, w);
            pcell->drawarea->SetBounds(x, yc, w, cellhgt, MixX, MixY + 27, MixX + MixW - 1, MixY + MixH - 1);

            yc += cellhgt;
            hgt += cellhgt;
        }
        pcell = pcell->next;
    }

    return hgt;
}

int J_BlankEffect(Graphics& g, int x, int y, BlankEffect* beff, Mixcell* mcell)
{
    int xf = x + 1 + MixCellGap;
    int yf;
    int hh;

    if(mcell->folded == false)
    {
        hh = 5 + img_mcf1->getHeight() + img_mcf2->getHeight() + beff->GetHeight();

        yf = y - hh;

        g.drawImageAt(img_mcf1, x + MixCellGap + 1, yf - 3, false);
        g.drawImageAt(img_mcf2, x + MixCellGap + 1, y - 8 - img_mcf2->getHeight(), false);

        J_EffParamCells(g, beff, xf, yf - 3 + img_mcf1->getHeight(), MixCellWidth - MixCellGap - 2);
    }
    else
    {
        yf = y - MixCellBasicHeight;
        g.drawImageAt(img_mcf0, x + MixCellGap + 1, yf - 3, false);

        hh = MixCellBasicHeight;
    }

    J_MixFoldToggle(g, xf + 14, yf - 4, mcell->fold_toggle);

    g.setColour(Colour(0xffFFFFFF));
    J_TString_Instr_xy(g, x + MixCellGap + 3, y - 11, mcell->mixstr);

    return hh;
}

int J_Filter(Graphics& g, int x, int y, Filter* flt, Mixcell* mcell)
{
    int xf = x + 1 + MixCellGap;
    int yf;
    int hh;

    if(mcell->folded == false)
    {
        hh = 5 + img_mcf1->getHeight() + img_mcf2->getHeight() + flt->GetHeight();

        yf = y - hh;

        g.drawImageAt(img_mcf1, x + MixCellGap + 1, yf - 3, false);
        g.drawImageAt(img_mcf2, x + MixCellGap + 1, y - 8 - img_mcf2->getHeight(), false);

        J_EffParamCells(g, flt, xf, yf - 3 + img_mcf1->getHeight(), MixCellWidth - MixCellGap - 2);
    }
    else
    {
        yf = y - MixCellBasicHeight;
        g.drawImageAt(img_mcf0, x + MixCellGap + 1, yf - 3, false);

        hh = MixCellBasicHeight;
    }

    J_MixFoldToggle(g, xf + 14, yf - 4, mcell->fold_toggle);

    g.setColour(Colour(0xffFFFFFF));
    J_TString_Instr_xy(g, x + MixCellGap + 3, y - 11, mcell->mixstr);

    return hh;
}

int J_Equalizer1(Graphics& g, int x, int y, EQ1* flt, Mixcell* mcell)
{
    int xf = x + 1 + MixCellGap;
    int yf;
    int hh;

    if(mcell->folded == false)
    {
        hh = 5 + img_mcf1->getHeight() + img_mcf2->getHeight() + flt->GetHeight();

        yf = y - hh;

        g.drawImageAt(img_mcf1, x + MixCellGap + 1, yf - 3, false);
        g.drawImageAt(img_mcf2, x + MixCellGap + 1, y - 8 - img_mcf2->getHeight(), false);

        J_EffParamCells(g, flt, xf, yf - 3 + img_mcf1->getHeight(), MixCellWidth - MixCellGap - 2);
    }
    else
    {
        yf = y - MixCellBasicHeight;
        g.drawImageAt(img_mcf0, x + MixCellGap + 1, yf - 3, false);

        hh = MixCellBasicHeight;
    }

    J_MixFoldToggle(g, xf + 14, yf - 4, mcell->fold_toggle);

    g.setColour(Colour(0xffFFFFFF));
    J_TString_Instr_xy(g, x + MixCellGap + 3, y - 11, mcell->mixstr);

    return hh;
}

int J_Equalizer3(Graphics& g, int x, int y, EQ3* eq3, Mixcell* mcell)
{
    int xf = x + 1 + MixCellGap;
    int yf;
    int hh;

    if(mcell->folded == false)
    {
        hh = 5 + img_mcf1->getHeight() + img_mcf2->getHeight() + eq3->GetHeight();

        yf = y - hh;

        g.drawImageAt(img_mcf1, x + MixCellGap + 1, yf - 3, false);
        g.drawImageAt(img_mcf2, x + MixCellGap + 1, y - 8 - img_mcf2->getHeight(), false);

        J_EffParamCells(g, eq3, xf, yf - 3 + img_mcf1->getHeight(), MixCellWidth - MixCellGap - 2);
    }
    else
    {
        yf = y - MixCellBasicHeight;
        g.drawImageAt(img_mcf0, x + MixCellGap + 1, yf - 3, false);

        hh = MixCellBasicHeight;
    }

    J_MixFoldToggle(g, xf + 14, yf - 4, mcell->fold_toggle);

    g.setColour(Colour(0xffFFFFFF));
    J_TString_Instr_xy(g, x + MixCellGap + 3, y - 11, mcell->mixstr);

    return hh;
}

int J_GraphicEQ(Graphics& g, int x, int y, GraphicEQ* geq, Mixcell* mcell)
{
    int xf = x + 1 + MixCellGap;
    int yf;
    int hh;

    if(mcell->folded == false)
    {
        hh = 5 + img_mcf1->getHeight() + img_mcf2->getHeight() + geq->GetHeight();

        yf = y - hh;

        g.drawImageAt(img_mcf1, x + MixCellGap + 1, yf - 3, false);
        g.drawImageAt(img_mcf2, x + MixCellGap + 1, y - 8 - img_mcf2->getHeight(), false);

        J_EffParamCells(g, geq, xf, yf - 3 + img_mcf1->getHeight(), MixCellWidth - MixCellGap - 2);
    }
    else
    {
        yf = y - MixCellBasicHeight;
        g.drawImageAt(img_mcf0, x + MixCellGap + 1, yf - 3, false);

        hh = MixCellBasicHeight;
    }

    J_MixFoldToggle(g, xf + 14, yf - 4, mcell->fold_toggle);

    g.setColour(Colour(0xffFFFFFF));
    J_TString_Instr_xy(g, x + MixCellGap + 3, y - 11, mcell->mixstr);

    return hh;
}

int J_CFilter(Graphics& g, int x, int y, CFilter* cflt, Mixcell* mcell)
{
    int xf = x + 1 + MixCellGap;
    int yf;
    int hh;

    if(mcell->folded == false)
    {
        hh = 5 + img_mcf1->getHeight() + img_mcf2->getHeight() + cflt->GetHeight();

        yf = y - hh;

        g.drawImageAt(img_mcf1, x + MixCellGap + 1, yf - 3, false);
        g.drawImageAt(img_mcf2, x + MixCellGap + 1, y - 8 - img_mcf2->getHeight(), false);

        J_EffParamCells(g, cflt, xf, yf - 3 + img_mcf1->getHeight(), MixCellWidth - MixCellGap - 2);
    }
    else
    {
        yf = y - MixCellBasicHeight;
        g.drawImageAt(img_mcf0, x + MixCellGap + 1, yf - 3, false);

        hh = MixCellBasicHeight;
    }

    J_MixFoldToggle(g, xf + 14, yf - 4, mcell->fold_toggle);

    g.setColour(Colour(0xffFFFFFF));
    J_TString_Instr_xy(g, x + MixCellGap + 3, y - 11, mcell->mixstr);

    return hh;
}

int J_Compressor(Graphics& g, int x, int y, Compressor* comp, Mixcell* mcell)
{
    int xf = x + 1 + MixCellGap;
    int yf;
    int hh;

    if(mcell->folded == false)
    {
        hh = 5 + img_mccomp1->getHeight() + img_mccomp2->getHeight() + comp->GetHeight();

        yf = y - hh;

        g.drawImageAt(img_mccomp1, x + MixCellGap + 1, yf - 3, false);
        g.drawImageAt(img_mccomp2, x + MixCellGap + 1, y - 8 - img_mccomp2->getHeight(), false);

        J_EffParamCells(g, comp, xf, yf - 3 + img_mccomp1->getHeight(), MixCellWidth - MixCellGap - 2);
    }
    else
    {
        yf = y - MixCellBasicHeight;
        g.drawImageAt(img_mccomp0, x + MixCellGap + 1, yf - 3, false);

        hh = MixCellBasicHeight;
    }

    J_MixFoldToggle(g, xf + 14, yf - 4, mcell->fold_toggle);

    g.setColour(Colour(0xffFFFFFF));
    J_TString_Instr_xy(g, x + MixCellGap + 3, y - 11, mcell->mixstr);

    return hh;
}

int J_Reverb(Graphics& g, int x, int y, CReverb* crev, Mixcell* mcell)
{
    int xf = x + 1 + MixCellGap;
    int yf;
    int hh;

    if(mcell->folded == false)
    {
        hh = 5 + img_mcrev1->getHeight() + img_mcrev2->getHeight() + crev->GetHeight();

        yf = y - hh;

        g.drawImageAt(img_mcrev1, x + MixCellGap + 1, yf - 3, false);
        g.drawImageAt(img_mcrev2, x + MixCellGap + 1, y - 8 - img_mcrev2->getHeight(), false);

        J_EffParamCells(g, crev, xf, yf - 3 + img_mcrev1->getHeight(), MixCellWidth - MixCellGap - 2);
    }
    else
    {
        yf = y - MixCellBasicHeight;
        g.drawImageAt(img_mcrev0, x + MixCellGap + 1, yf - 3, false);

        hh = MixCellBasicHeight;
    }

    J_MixFoldToggle(g, xf + 14, yf - 4, mcell->fold_toggle);

    g.setColour(Colour(0xffFFFFFF));
    J_TString_Instr_xy(g, x + MixCellGap + 3, y - 11, mcell->mixstr);

    return hh;
}

int J_Tremolo(Graphics& g, int x, int y, CTremolo* trem, Mixcell* mcell)
{
    int xf = x + 1 + MixCellGap;
    int yf;
    int hh;

    if(mcell->folded == false)
    {
        hh = 5 + img_mcf1->getHeight() + img_mcf2->getHeight() + trem->GetHeight();

        yf = y - hh;

        g.drawImageAt(img_mcf1, x + MixCellGap + 1, yf - 3, false);
        g.drawImageAt(img_mcf2, x + MixCellGap + 1, y - 8 - img_mcf2->getHeight(), false);

        J_EffParamCells(g, trem, xf, yf - 3 + img_mcf1->getHeight(), MixCellWidth - MixCellGap - 2);
    }
    else
    {
        yf = y - MixCellBasicHeight;
        g.drawImageAt(img_mcf0, x + MixCellGap + 1, yf - 3, false);

        hh = MixCellBasicHeight;
    }

    J_MixFoldToggle(g, xf + 14, yf - 4, mcell->fold_toggle);

    g.setColour(Colour(0xffFFFFFF));
    J_TString_Instr_xy(g, x + MixCellGap + 3, y - 11, mcell->mixstr);

    return hh;
}

int J_XDelay(Graphics& g, int x, int y, XDelay* flt, Mixcell* mcell)
{
    int xf = x + 1 + MixCellGap;
    int yf;
    int hh;

    if(mcell->folded == false)
    {
        hh = 5 + img_mcdelay1->getHeight() + img_mcdelay2->getHeight() + flt->GetHeight();

        yf = y - hh;

        g.drawImageAt(img_mcdelay1, x + MixCellGap + 1, yf - 3, false);
        g.drawImageAt(img_mcdelay2, x + MixCellGap + 1, y - 8 - img_mcdelay2->getHeight(), false);

        J_EffParamCells(g, flt, xf, yf - 3 + img_mcdelay1->getHeight(), MixCellWidth - MixCellGap - 2);
    }
    else
    {
        yf = y - MixCellBasicHeight;
        g.drawImageAt(img_mcdelay0, x + MixCellGap + 1, yf - 3, false);

        hh = MixCellBasicHeight;
    }

    J_MixFoldToggle(g, xf + 14, yf - 4, mcell->fold_toggle);

    g.setColour(Colour(0xffFFFFFF));
    J_TString_Instr_xy(g, x + MixCellGap + 3, y - 11, mcell->mixstr);

    return hh;
}

void J_GainFolded(Graphics& g, int x, int y, Gain* amp, Mixcell* mcell)
{
    g.drawImageAt(img_mcgain, x + MixCellGap + 1, y - MixCellBasicHeight - 3, false);

    Knob* kb = amp->r_level;
    g.setColour(Colour(0xffC8C8C8));
    J_KnobXLarge(g, kb, false, x + MixCellGap + 70, y - 24);
    kb->Activate(kb->x - kb->rad, kb->y - kb->rad, kb->rad*2, kb->rad*2,
                         MixX, MixY, MixX + MixW, MixY + MixH);

    g.setColour(Colour(0xffFFFFFF));
    J_TString_Instr_xy(g, x + MixCellGap + 3, y - 11, mcell->mixstr);
}

void J_SendFolded(Graphics& g, int x, int y, Send* snd, Mixcell* mcell)
{
    g.drawImageAt(img_mcsend, x + MixCellGap + 1, y - MixCellBasicHeight - 3, false);

    J_DigitStr_xy(g, x + MixCellGap + 31, y - 23, snd->fxstr, snd->send_mixcell != NULL);
    snd->fxstr->edx = 18;
    snd->fxstr->edy = 12;
    snd->fxstr->Activate(snd->fxstr->x - 1, snd->fxstr->y - 11, snd->fxstr->edx, snd->fxstr->edy + 3,
                         MixX, MixY, MixX + MixW, MixY + MixH);

    Knob* kb = snd->r_amount;
	J_KnobXLarge(g, kb, false, x + MixCellGap + 70, y - 24);
    kb->Activate(kb->x - kb->rad, kb->y - kb->rad, kb->rad*2, kb->rad*2,
                         MixX, MixY, MixX + MixW, MixY + MixH);

    g.setColour(Colour(0xffFFFFFF));
    J_TString_Instr_xy(g, x + MixCellGap + 3, y - 11, mcell->mixstr);
}

void J_VSTPluginFolded(Graphics& g, int x, int y, Mixcell* mcell)
{
    CVSTPlugin* pPlug = NULL;
    

    if (mcell->effect != NULL)
    {
        pPlug = (CVSTPlugin*) (((VSTEffect*)(mcell->effect))->pPlug);
    }

    if (pPlug != NULL)
    {
        g.drawImageAt(img_mcvst, x + MixCellGap + 1, y - MixCellBasicHeight - 3, false);

        char name[21];
        char fx_name[19] = {0};

        memset(name, 0, sizeof(name));
        pPlug->GetDisplayName(fx_name, 18);
        //strcpy(name, "(");
        strcat(name, fx_name);
        //strcat(name, ")");

        //glColor3ub(70, 245, 180);
        g.setColour(Colour(0xff46F5B4));
        J_String_Small_Ranged(g, x + MixCellGap + 3, y - 11, name, MixCellWidth - MixCellGap - 6);
        //J_TextInstr_xy(x, y, name);
        //J_TextButton(x, y - mcell->height + 11, 20, 10, ((VSTEffect*)(mcell->effect))->pEditButton);
        J_WindowButton(g, x + MixCellGap + 16, y - MixCellBasicHeight - 2, 20, 12, ((VSTEffect*)(mcell->effect))->pEditButton);
    }
}

int J_DrawMixcellContent(Graphics& g, int x, int y, Mixcell* mcell)
{
    int ret_val = MixCellBasicHeight;

    switch(mcell->effect->type)
    {
        case EffType_Tremolo:
        {
            ret_val = J_Tremolo(g, x, y, (CTremolo*)(mcell->effect), mcell);
        }break;
        case EffType_Default:
        {
            ret_val = J_BlankEffect(g, x, y, (BlankEffect*)(mcell->effect), mcell);
        }break;
        case EffType_VSTPlugin:
        {
            J_VSTPluginFolded(g, x, y, mcell);
            mcell->mixstr->active = false;
        }
        break;
        case EffType_Gain:
        {
            J_GainFolded(g, x, y, (Gain*)(mcell->effect), mcell);
        }break;
        case EffType_Send:
        {
            J_SendFolded(g, x, y, (Send*)(mcell->effect), mcell);
        }break;
        case EffType_Filter:
        {
            ret_val = J_Filter(g, x, y, (Filter*)(mcell->effect), mcell);
        }break;
        case EffType_Equalizer1:
        {
            ret_val = J_Equalizer1(g, x, y, (EQ1*)(mcell->effect), mcell);
        }break;
        case EffType_Equalizer3:
        {
            ret_val = J_Equalizer3(g, x, y, (EQ3*)(mcell->effect), mcell);
        }break;
        case EffType_GraphicEQ:
        {
            ret_val = J_GraphicEQ(g, x, y, (GraphicEQ*)(mcell->effect), mcell);
        }break;
        case EffType_CFilter:
        {
            ret_val = J_CFilter(g, x, y, (CFilter*)(mcell->effect), mcell);
        }break;
        case EffType_XDelay:
        {
            ret_val = J_XDelay(g, x, y, (XDelay*)(mcell->effect), mcell);
        }break;
        case EffType_Compressor:
        {
            ret_val = J_Compressor(g, x, y, (Compressor*)(mcell->effect), mcell);
        }break;
        case EffType_Reverb:
        {
            ret_val = J_Reverb(g, x, y, (CReverb*)(mcell->effect), mcell);
        }break;
    }

    // Draw common part for any FX. Right, Save button for presets
    //GL_FxCommonButtons();
    return ret_val;
}

int J_VSTPlugin(Graphics& g, int x, int y, VSTEffect* eff, MixChannel* mchan)
{
    int hh = 0;
	CVSTPlugin* pPlug = NULL; 

    pPlug = (CVSTPlugin*)eff->pPlug;

    if (pPlug != NULL)
    {
        g.drawImageAt(img_mcvst, x, y, false);

        char name[21];
        char fx_name[19] = {0};

        memset(name, 0, sizeof(name));
        pPlug->GetDisplayName(fx_name, 18);
        //strcpy(name, "(");
        strcat(name, fx_name);
        //strcat(name, ")");

        g.setColour(Colour(0xcfCFEFFF));
        //g.setColour(Colour(0xff46FFB4));
        J_String_Bold_Ranged(g, x + 2, y + 10, name, 91);
        //J_TextInstr_xy(x, y, name);
        //J_TextButton(x, y - mcell->height + 11, 20, 10, ((VSTEffect*)(mcell->effect))->pEditButton);
        J_WindowButton(g, x + 77, y + 10, 18, 11, eff->pEditButton);
    }
	hh = img_mcvst->getHeight();

    Toggle* ebp = eff->bypass_toggle;
    J_EffBypassToggle(g, x + 61, y + 14, ebp);
    ebp->Activate(ebp->x,
                  ebp->y,
                  ebp->width,
                  ebp->height,
                  mchan->x1r,
                  mchan->y1r,
                  mchan->x2r,
                  mchan->y2r);

    return hh;
}

int J_FXNative(Graphics& g, int x, int y, Eff* eff, MixChannel* mchan)
{
    int hh = 0;

    if(eff->folded == false)
    {
        g.drawImageAt(img_mcdelay1, x, y, false);
        hh += img_mcdelay1->getHeight();

        hh += J_EffParamCells_Aux(g, eff, x, y + hh, MixCellWidth - MixCellGap - 1, mchan);

        g.drawImageAt(img_mcdelay2, x, y + hh, false);
        hh += img_mcdelay2->getHeight();
    }
    else
    {
        g.drawImageAt(img_mcdelay0, x, y, false);
        hh = img_mcdelay0->getHeight();
    }

    g.setColour(Colour(0xbf9FDFFF));
    J_TextBold_xy(g, x + 2, y + 10, eff->name);

    if(eff->folded == true)
    {
        J_MixFoldToggle(g, x + 80, y + 14, eff->fold_toggle);
        J_EffBypassToggle(g, x + 61, y + 14, eff->bypass_toggle);
    }
    else
    {
        J_MixFoldToggle(g, x + 80, y + 11, eff->fold_toggle);
        J_EffBypassToggle(g, x + 61, y + 11, eff->bypass_toggle);
    }

    Toggle* ebp = eff->bypass_toggle;
    ebp->Activate(ebp->x,
                  ebp->y,
                  ebp->width,
                  ebp->height,
                  mchan->x1r,
                  mchan->y1r,
                  mchan->x2r,
                  mchan->y2r);

    return hh;
}

int J_Reverb(Graphics& g, int x, int y, CReverb* rev, MixChannel* mchan)
{
    int hh = 0;

    if(rev->folded == false)
    {
		g.drawImageAt(img_mcdelay1, x, y, false);
        hh += img_mcdelay1->getHeight();

        hh += J_EffParamCells_Aux(g, rev, x, y + hh, MixCellWidth - MixCellGap - 2, mchan);

        g.drawImageAt(img_mcdelay2, x, y + hh, false);
        hh += img_mcdelay2->getHeight();
    }
    else
    {
        g.drawImageAt(img_mcdelay0, x, y, false);
        hh = img_mcdelay0->getHeight();
    }

    g.setColour(Colour(0xffFFFFFF));
    J_TextInstr_xy(g, x + 3, y + 11, rev->name);

    if(rev->folded == true)
    {
        J_MixFoldToggle(g, x + 79, y + 15, rev->fold_toggle);
    }
    else
    {
        J_MixFoldToggle(g, x + 79, y + 12, rev->fold_toggle);
    }

    Toggle* ebp = rev->bypass_toggle;
    J_EffBypassToggle(g, x + 82, y + 1, ebp);
    ebp->Activate(ebp->x,
                  ebp->y,
                  ebp->width,
                  ebp->height,
                  mchan->x1r,
                  mchan->y1r,
                  mchan->x2r,
                  mchan->y2r);

    return hh;
}

int J_Equalizer1(Graphics& g, int x, int y, EQ1* eq1, MixChannel* mchan)
{
    int hh = 0;

    if(eq1->folded == false)
    {
		g.drawImageAt(img_mcdelay1, x, y, false);
        hh += img_mcdelay1->getHeight();

        hh += J_EffParamCells_Aux(g, eq1, x, y + hh, MixCellWidth - MixCellGap - 2, mchan);

        g.drawImageAt(img_mcdelay2, x, y + hh, false);
        hh += img_mcdelay2->getHeight();
    }
    else
    {
        g.drawImageAt(img_mcdelay0, x, y, false);
        hh = img_mcdelay0->getHeight();
    }

    g.setColour(Colour(0xffFFFFFF));
    J_TextInstr_xy(g, x + 3, y + 11, eq1->name);

    if(eq1->folded == true)
    {
        J_MixFoldToggle(g, x + 79, y + 15, eq1->fold_toggle);
    }
    else
    {
        J_MixFoldToggle(g, x + 79, y + 12, eq1->fold_toggle);
    }

    Toggle* ebp = eq1->bypass_toggle;
    J_EffBypassToggle(g, x + 82, y + 1, ebp);
    ebp->Activate(ebp->x,
                  ebp->y,
                  ebp->width,
                  ebp->height,
                  mchan->x1r,
                  mchan->y1r,
                  mchan->x2r,
                  mchan->y2r);

    return hh;
}

int J_Equalizer3(Graphics& g, int x, int y, EQ3* eq3, MixChannel* mchan)
{
    int hh = 0;

    if(eq3->folded == false)
    {
		g.drawImageAt(img_mcdelay1, x, y, false);
        hh += img_mcdelay1->getHeight();

        hh += J_EffParamCells_Aux(g, eq3, x, y + hh, MixCellWidth - MixCellGap - 2, mchan);

        g.drawImageAt(img_mcdelay2, x, y + hh, false);
        hh += img_mcdelay2->getHeight();
    }
    else
    {
        g.drawImageAt(img_mcdelay0, x, y, false);
        hh = img_mcdelay0->getHeight();
    }

    g.setColour(Colour(0xffFFFFFF));
    J_TextInstr_xy(g, x + 3, y + 11, eq3->name);

    if(eq3->folded == true)
    {
        J_MixFoldToggle(g, x + 79, y + 15, eq3->fold_toggle);
    }
    else
    {
        J_MixFoldToggle(g, x + 79, y + 12, eq3->fold_toggle);
    }

    Toggle* ebp = eq3->bypass_toggle;
    J_EffBypassToggle(g, x + 82, y + 1, ebp);
    ebp->Activate(ebp->x,
                  ebp->y,
                  ebp->width,
                  ebp->height,
                  mchan->x1r,
                  mchan->y1r,
                  mchan->x2r,
                  mchan->y2r);

    return hh;
}

int J_GraphicEQ(Graphics& g, int x, int y, GraphicEQ* greq, MixChannel* mchan)
{
    int hh = 0;

    if(greq->folded == false)
    {
		g.drawImageAt(img_mcdelay1, x, y, false);
        hh += img_mcdelay1->getHeight();

        hh += J_EffParamCells_Aux(g, greq, x, y + hh, MixCellWidth - MixCellGap - 2, mchan);

        g.drawImageAt(img_mcdelay2, x, y + hh, false);
        hh += img_mcdelay2->getHeight();
    }
    else
    {
        g.drawImageAt(img_mcdelay0, x, y, false);
        hh = img_mcdelay0->getHeight();
    }

    g.setColour(Colour(0xffFFFFFF));
    J_TextInstr_xy(g, x + 3, y + 11, greq->name);

    if(greq->folded == true)
    {
        J_MixFoldToggle(g, x + 79, y + 15, greq->fold_toggle);
    }
    else
    {
        J_MixFoldToggle(g, x + 79, y + 12, greq->fold_toggle);
    }

    Toggle* ebp = greq->bypass_toggle;
    J_EffBypassToggle(g, x + 82, y + 1, ebp);
    ebp->Activate(ebp->x,
                  ebp->y,
                  ebp->width,
                  ebp->height,
                  mchan->x1r,
                  mchan->y1r,
                  mchan->x2r,
                  mchan->y2r);

    return hh;
}

int J_Compressor(Graphics& g, int x, int y, Compressor* compr, MixChannel* mchan)
{
    int hh = 0;

    if(compr->folded == false)
    {
		g.drawImageAt(img_mcdelay1, x, y, false);
        hh += img_mcdelay1->getHeight();

        hh += J_EffParamCells_Aux(g, compr, x, y + hh, MixCellWidth - MixCellGap - 2, mchan);

        g.drawImageAt(img_mcdelay2, x, y + hh, false);
        hh += img_mcdelay2->getHeight();

	    //hh = 5 + img_mcdelay1->getHeight() + img_mcdelay2->getHeight() + delay->GetHeight();
	}
    else
    {
        g.drawImageAt(img_mcdelay0, x, y, false);
        hh = img_mcdelay0->getHeight();
    }

    g.setColour(Colour(0xffFFFFFF));
    J_TextInstr_xy(g, x + 3, y + 11, compr->name);

    if(compr->folded == true)
    {
        J_MixFoldToggle(g, x + 79, y + 15, compr->fold_toggle);
    }
    else
    {
        J_MixFoldToggle(g, x + 79, y + 12, compr->fold_toggle);
    }

    Toggle* ebp = compr->bypass_toggle;
    J_EffBypassToggle(g, x + 82, y + 1, ebp);
    ebp->Activate(ebp->x,
                  ebp->y,
                  ebp->width,
                  ebp->height,
                  mchan->x1r,
                  mchan->y1r,
                  mchan->x2r,
                  mchan->y2r);

    return hh;
}

int J_CFilter(Graphics& g, int x, int y, CFilter* flt, MixChannel* mchan)
{
    int hh = 0;

    if(flt->folded == false)
    {
		g.drawImageAt(img_mcdelay1, x, y, false);
        hh += img_mcdelay1->getHeight();

        hh += J_EffParamCells_Aux(g, flt, x, y + hh, MixCellWidth - MixCellGap - 2, mchan);

        g.drawImageAt(img_mcdelay2, x, y + hh, false);
        hh += img_mcdelay2->getHeight();

	    //hh = 5 + img_mcdelay1->getHeight() + img_mcdelay2->getHeight() + delay->GetHeight();
	}
    else
    {
        g.drawImageAt(img_mcdelay0, x, y, false);
        hh = img_mcdelay0->getHeight();
    }

    g.setColour(Colour(0xffFFFFFF));
    J_TextInstr_xy(g, x + 3, y + 11, flt->name);

    if(flt->folded == true)
    {
        J_MixFoldToggle(g, x + 79, y + 15, flt->fold_toggle);
    }
    else
    {
        J_MixFoldToggle(g, x + 79, y + 12, flt->fold_toggle);
    }

    Toggle* ebp = flt->bypass_toggle;
    J_EffBypassToggle(g, x + 82, y + 1, ebp);
    ebp->Activate(ebp->x,
                  ebp->y,
                  ebp->width,
                  ebp->height,
                  mchan->x1r,
                  mchan->y1r,
                  mchan->x2r,
                  mchan->y2r);

    return hh;
}

int J_XDelay(Graphics& g, int x, int y, XDelay* delay, MixChannel* mchan)
{
    int hh = 0;

    if(delay->folded == false)
    {
        g.drawImageAt(img_mcdelay1, x, y, false);
        hh += img_mcdelay1->getHeight();

        hh += J_EffParamCells_Aux(g, delay, x, y + hh, MixCellWidth - MixCellGap - 2, mchan);

        g.drawImageAt(img_mcdelay2, x, y + hh, false);
        hh += img_mcdelay2->getHeight();

        //hh = 5 + img_mcdelay1->getHeight() + img_mcdelay2->getHeight() + delay->GetHeight();
    }
    else
    {
        g.drawImageAt(img_mcdelay0, x, y, false);
        hh = img_mcdelay0->getHeight();
    }

    g.setColour(Colour(0xffFFFFFF));
    J_TextInstr_xy(g, x + 3, y + 11, delay->name);

    if(delay->folded == true)
    {
        J_MixFoldToggle(g, x + 79, y + 15, delay->fold_toggle);
    }
    else
    {
        J_MixFoldToggle(g, x + 79, y + 12, delay->fold_toggle);
    }

    Toggle* ebp = delay->bypass_toggle;
    J_EffBypassToggle(g, x + 82, y + 1, ebp);
    ebp->Activate(ebp->x,
                  ebp->y,
                  ebp->width,
                  ebp->height,
                  mchan->x1r,
                  mchan->y1r,
                  mchan->x2r,
                  mchan->y2r);

    return hh;
}

int J_DrawMixChannelContent(Graphics& g, int x, int y, MixChannel* mchan)
{
    int h = 0;
    int effh;
    Eff* eff = mchan->first_eff;
    while(eff != NULL)
    {
		eff->x = x;
		eff->w = MixChanWidth - 2;
        eff->y = y + h;
		effh = 0; // Insurance
        switch(eff->type)
        {
            case EffType_Tremolo:
            {
                //ret_val = J_Tremolo(g, x, y, (CTremolo*)(mcell->effect), mcell);
            }break;
            case EffType_Default:
            {
                //ret_val = J_BlankEffect(g, x, y, (BlankEffect*)(mcell->effect), mcell);
            }break;
            case EffType_VSTPlugin:
            {
                effh = J_VSTPlugin(g, x, y + h, (VSTEffect*)eff, mchan);
                //mcell->mixstr->active = false;
            }
            break;
            case EffType_Gain:
            {
                //J_GainFolded(g, x, y, (Gain*)eff, mchan);
            }break;
            case EffType_Send:
            {
                //J_SendFolded(g, x, y, (Send*)eff, mchan);
            }break;
            case EffType_Filter:
            {
                //ret_val = J_Filter(g, x, y, (Filter*)(mcell->effect), mcell);
            }break;
            case EffType_Equalizer1:
            {
                effh = J_FXNative(g, x, y + h, eff, mchan);
                //effh = J_Equalizer1(g, x, y + h, (EQ1*)eff, mchan);
            }break;
            case EffType_Equalizer3:
            {
                effh = J_FXNative(g, x, y + h, eff, mchan);
                //effh = J_Equalizer3(g, x, y + h, (EQ3*)eff, mchan);
            }break;
            case EffType_GraphicEQ:
            {
                effh = J_FXNative(g, x, y + h, eff, mchan);
                //effh = J_GraphicEQ(g, x, y + h, (GraphicEQ*)eff, mchan);
            }break;
            case EffType_CFilter:
            {
                effh = J_FXNative(g, x, y + h, eff, mchan);
                //effh = J_CFilter(g, x, y + h, (CFilter*)eff, mchan);
            }break;
            case EffType_XDelay:
            {
                effh = J_FXNative(g, x, y + h, eff, mchan);
                //effh = J_XDelay(g, x, y + h, (XDelay*)eff, mchan);
            }break;
            case EffType_Compressor:
            {
                effh = J_FXNative(g, x, y + h, eff, mchan);
                //effh = J_Compressor(g, x, y + h, (Compressor*)eff, mchan);
            }break;
            case EffType_Reverb:
            {
                effh = J_FXNative(g, x, y + h, eff, mchan);
                //effh = J_Reverb(g, x, y + h, (CReverb*)eff, mchan);
            }break;
            case EffType_Chorus:
            {
                effh = J_FXNative(g, x, y + h, eff, mchan);
            }break;
            case EffType_Flanger:
            {
                effh = J_FXNative(g, x, y + h, eff, mchan);
            }break;
            case EffType_Phaser:
            {
                effh = J_FXNative(g, x, y + h, eff, mchan);
            }break;
            case EffType_WahWah:
            {
                effh = J_FXNative(g, x, y + h, eff, mchan);
            }break;
            case EffType_Distortion:
            {
                effh = J_FXNative(g, x, y + h, eff, mchan);
            }break;
            case EffType_BitCrusher:
            {
                effh = J_FXNative(g, x, y + h, eff, mchan);
            }break;
            case EffType_Stereo:
            {
                effh = J_FXNative(g, x, y + h, eff, mchan);
            }break;
            case EffType_CFilter2:
            {
                effh = J_FXNative(g, x, y + h, eff, mchan);
            }break;
            case EffType_CFilter3:
            {
                effh = J_FXNative(g, x, y + h, eff, mchan);
            }break;
        }

        if(eff->y + eff->h < mchan->ry1 || eff->y > mchan->ry2)
        {
            eff->visible = false;
        }
        else
        {
            eff->visible = true;
        }

        eff->h = effh;
        h += effh;

        eff = eff->cnext;
    }

    return h;
}

void J_BlankMixcell(Graphics& g, int x, int y, Mixcell* mcell)
{
    if(mcell->effect == NULL)
    {
        g.setColour(Colour(33, 56, 65));
        J_LineRect(g, x + MixCellGap + 1, y - mcell->height - 2, x + MixCellWidth - 1, y - 10);
        
        g.setColour(Colour(15, 41, 47));
        J_Line(g, x + MixCellGap + 1, y - mcell->height - 3, x + MixCellWidth - 1, y - mcell->height - 3);
        J_Line(g, x + MixCellGap + 1, y - 9, x + MixCellWidth - 1, y - 9);
    
        J_Line(g, x + MixCellGap + 2, y - mcell->height + 10, x + MixCellWidth - 1, y - mcell->height + 10);
    }
}

void J_Mixcell(Graphics& g, int xc, int yc, Mixcell* mcell, bool visible, int xb)
{
    mcell->visible = visible;
    if(visible == true)
    {
        mcell->x = xc + MixCellGap + 1;
        mcell->x1 = xc + MixCellWidth - 1;
        mcell->y1 = yc - 10;
        
        int xstr;
        xstr = xc + MixCellGap + 3;
        if(xstr < xb || xstr >= WindWidth)
        {
            mcell->mixstr->active = false;
        }
        else
        {
            mcell->mixstr->active = true;
        }

        if(mcell->effect == NULL)
        {
            mcell->height = MixCellBasicHeight;
            J_BlankMixcell(g, xc, yc, mcell);
            g.setColour(Colour(0xff6482C8));
            J_TString_Base_xy(g, xstr, yc - 11, mcell->mixstr);

            mcell->fold_toggle->Deactivate();
        }
        else
        {
            mcell->height = J_DrawMixcellContent(g, xc, yc, mcell);
            J_MixBypassToggle(g, xc + MixCellWidth + 1, yc - mcell->height + 6, mcell->bypass_toggle);
            mcell->bypass_toggle->Activate(mcell->bypass_toggle->x,
                                           mcell->bypass_toggle->y,
                                           mcell->bypass_toggle->width,
                                           mcell->bypass_toggle->height,
                                           MixX,
                                           MixY,
                                           MixX + MixW,
                                           MixY + MixH);
            mcell->fold_toggle->Activate();
        }
        mcell->y = yc - mcell->height - 2;

        int edxlen = MixCellWidth - 40;
        if(xstr + edxlen > WindWidth)
        {
            edxlen -= (xstr + edxlen) - WindWidth;
        }
        mcell->mixstr->edx = edxlen;
        mcell->mixstr->edy = 14;

        g.setColour(Colour(0xff000000));
        J_TextSmall_xy(g, mcell->x + 3, mcell->y + 11, mcell->indexstr);
        if(mcell->queued == true)
        {
            g.setColour(Colour(0xffFF2000));
            J_TextSmall_xy(g, mcell->x + 2, mcell->y + 10, mcell->indexstr);
            g.setColour(Colour(0x4fFF2000));
            J_TextSmall_xy(g, mcell->x + 1, mcell->y + 10, mcell->indexstr);
            J_TextSmall_xy(g, mcell->x + 3, mcell->y + 10, mcell->indexstr);
        }
        else
        {
            //g.setColour(Colour(122, 164, 144));
            if(mcell->effect == NULL)
            {
                g.setColour(Colour(83, 106, 115));
            }
            else
            {
                g.setColour(Colour(122, 164, 144));
            }
            J_TextSmall_xy(g, mcell->x + 2, mcell->y + 10, mcell->indexstr);
        }

        if(mcell->outpin->offset == false)
        {
            mcell->outpin->offset = true;
            if(mcell->outcell != NULL && mcell->outcell->inpin->type == Ctrl_InPin_Bus)
            {
                mcell->outpin->xc_offs = 0;
                mcell->outpin->yc_offs = yc - mcell->outcell->inpin->yc - 8;
            }
        }
        J_InPin_pic(g, xc + MixCellWidth - 9, yc - mcell->height - 1, mcell->inpin);
        J_OutPin(g, xc + MixCellWidth - 7, yc - 16, mcell->outpin);
        mcell->drawarea->SetBounds(mcell->x, mcell->y, mcell->x1 - mcell->x + 1, mcell->y1 - mcell->y + 1);
        mcell->Enable();
    }
    else if(visible == false)
    {
        mcell->inpin->x = xc + MixCellWidth - 10;
        mcell->inpin->y = yc - mcell->height - 2;

        mcell->inpin->xc = mcell->inpin->x + 4;
        mcell->inpin->yc = mcell->inpin->y + 3;

        mcell->outpin->x = xc + MixCellWidth - 10;
        mcell->outpin->y = yc - 16;

        mcell->outpin->xc = mcell->outpin->x + 5;
        mcell->outpin->yc = mcell->outpin->y + 3;

        mcell->Disable();
    }
}

void J_DrawHint(Graphics& g, Control *ctrl, int x , int y)
{
    if ((ctrl != NULL) && (ctrl->hint != NULL) && (strlen(ctrl->hint) != 0) && (ctrl->hint[0] != 0))
    {
        unsigned int width = 200;

        //width = hint_font.width(ctrl->hint);
        width = ins->getStringWidth(String(ctrl->hint));

        y += 20;
        //glColor4ub(154, 57, 33, 100);
        g.setColour(Colour(0xff9A3921));
        J_LineRect(g, x, y, x + width + 8, y + 22);

        //glColor4ub(211, 250, 254, 170);
        g.setColour(Colour(0xffDFA9FE));
        J_FillRect(g, x + 2, y + 2, x + width + 6, y + 20);

        //glColor4ub(0, 30, 30, 230);
        g.setColour(Colour(0xff001E1E));
        //text::print_simple(hint_font, (GLfloat)x + 4, (GLfloat)y + 16, ctrl->hint);
        J_TextInstr_xy(g, x + 4, y + 16, ctrl->hint);
    }
}

bool J_SingleTrackControls(Graphics& g, int trknum, Trk* trkdata, SliderHPan* pan, SliderHVol* vol, Toggle* mute, Toggle* solo, int x, int y)
{
    g.setColour(Colour(0xff144650));
    J_LineRect(g, x - 10, y, x + MixCenterWidth - 53, y + lineHeight);

    g.setColour(Colour(0xff145660));
    J_LineRect(g, x + 127, y, x + MixCenterWidth - 13, y + lineHeight);
    
    DigitStr* dstr = mix->trkfxstr[trknum];
    J_DigitStr_xy(g, x + 131, y + lineHeight - 1, dstr, true);
    dstr->edx = 34;
    dstr->edy = 10;
    dstr->active = true;
    dstr->drawarea->Enable();
    dstr->drawarea->SetBounds(dstr->x - 1, dstr->y - 11, dstr->edx, dstr->edy + 3,
                              MainX2 + 3, MainY1 + 18, MainX2 + MixCenterWidth, GridY2);

    pan->active = vol->active = mute->active = solo->active = true;

    //J_KnobSmall(g, pan, x, y + 6);
    J_SliderTrackPan(g, pan, x - 8, y + 2, true);
    pan->drawarea->Enable();
    pan->drawarea->SetBounds(pan->x, pan->y, pan->width + 2, pan->height + 2,
                             MainX2 + 3, MainY1 + 18, MainX2 + MixCenterWidth, GridY2);
    //pan->drawarea->SetBounds(pan->x - 6, pan->y - 6, pan->rad*2 + 1, pan->rad*2 + 1,
    //                         MainX2 + 3, MainY1 + 18, MainX2 + MixCenterWidth, GridY2);

    J_SliderTrackVol(g, vol, x + 37, y + 2);
    vol->drawarea->Enable();
    vol->drawarea->SetBounds(vol->x - 1, vol->y - 1, vol->width + 5, vol->height + 3,
                             MainX2 + 3, MainY1 + 18, MainX2 + MixCenterWidth, GridY2);
    
    J_MuteToggle(g, mute, x + 101, y + 1);
    mute->drawarea->Enable();
    mute->drawarea->SetBounds(mute->x, mute->y, mute->width, mute->height,
                              MainX2 + 3, MainY1 + 18, MainX2 + MixCenterWidth, GridY2);

    J_SoloToggle(g, solo, x + 113, y + 1);
    solo->drawarea->Enable();
    solo->drawarea->SetBounds(solo->x, solo->y, solo->width, solo->height,
                              MainX2 + 3, MainY1 + 18, MainX2 + MixCenterWidth, GridY2);

    if((trkdata == C.maintrk && C.maintrk->bunchito == false)||
       ((trkdata->buncho && C.maintrk == trkdata)||
        (C.maintrk->bunched && C.maintrk->trkbunch->trk_start == trkdata)))
    {
        //g.setColour(Colour(0xff64FFFF));
        //J_FilledRect(g, x - 10, y + 3, x - 8, yc + lineHeight - 3);
        //J_Line(g, x - 10, y + 1, x + 126, y + 1);
        //J_Line(g, x - 10, y + lineHeight - 1, x + 126, y + lineHeight - 1);
        return true;
    }
    else
    {
        return false;
    }
}

void J_TrackControls(Graphics& g, int x, int y, int w, int h)
{
    bool prolled;
    int linenum = OffsLine;
    int trknum;
    Trk* trkdata;
    TrkBunch* ttb = NULL;
    bool    bunched;
    bool    buncho;
    bool    curtrk;
    int     bunchsize;

    mixBrw->x = x;
    mixBrw->y = y;
    mixBrw->width = w;
    mixBrw->height = h;

    g.saveState();
    g.reduceClipRegion(x, y, w, h);

    trkdata = GetTrkDataForLine(OffsLine, field);
    trknum = trkdata->trknum;
    if(trknum < 999)
    {
        J_PanelRect1(g, x, y - 1, x + 4, MainY2 + 1);

        g.setColour(Colour(0xff142428));
        J_FillRect(g, x + 4, MainY1 + 16, x + MixCenterWidth - 39, GridY2 + 1);

        g.setColour(Colour(0xff000000));
        //J_Line(g, x + 9, MainY1 + 16, x + 9, GridY2 + 1);
        g.fillRect(x + MixCenterWidth - 39, MainY1 + 16, 39, GridY2 - GridY1);

        for(int yc = GridY1; yc <= GridY2; yc += lineHeight)
        {
            bunched = false;
            buncho = false;
            bunchsize = 1;

            prolled = false;
            if(linenum > trkdata->start_line)
            {
                prolled = true;
            }

            ttb = trkdata->trkbunch;    //CheckIfTrackBunched(trknum, field);
            if(ttb != NULL)
            {
                bunchsize = ttb->end_track - ttb->start_track + 1;
                if(trknum == ttb->start_track)
                {
                    buncho = true;
                }
                else
                {
                    bunched = true;
                }
            }

            if(bunched == false && prolled == false)
            {
                curtrk = J_SingleTrackControls(g, trknum, trkdata, trkdata->pan, trkdata->vol, trkdata->mute, trkdata->solo, x + 13, yc);
                if(curtrk)
                {
                    g.setColour(Colour(0xff14C800));
                    J_FillRect(g, x + 2, yc, x + 2, yc + lineHeight);
                }
            }
            linenum++;

            if(trkdata->end_line < linenum)
            {
                trkdata = trkdata->next;
                trknum = trkdata->trknum;
                if(trknum > 999)
                {
                    break;
                }
            }
        }
    }
    g.restoreState();
}

void J_TrackControls(Graphics& g)
{
    J_TrackControls(g, mixBrw->x, mixBrw->y, mixBrw->width, mixBrw->height);
}

void J_NodeSmall(Graphics& g, int x, int y)
{
    g.drawImageAt(img_node, x, y, false);
}

void J_NodeBig(Graphics& g, int x, int y)
{
    //g.drawImageAt(img_node, x, y, false);
    g.setColour(Colour(0xff4F4F4F));
    g.fillEllipse((float)x, (float)y, 10, 10);

    g.setColour(Colour(0xffFFFFFF));
    g.fillEllipse((float)x + 1, (float)y + 1, 8, 8);
}

/*==================================================================================================
BRIEF: Draws mixer connection lines, input and output pin highlights, mixcell highlights and current
       mixcell.

PARAMETERS:
[IN]
    g - graphics context.

[OUT]
N/A

OUTPUT: N/A
==================================================================================================*/
void J_MixerConnectionsAndHighlights(Graphics& g)
{
/*
    Mixcell* mcell;
    mcell = mix->current_mixcell;
    if(mcell != NULL && mcell->visible)
    {
        g.setColour(Colour(0xffFF6830));
        J_LineRect(g, mcell->x - 1, mcell->y - 1, mcell->x1 + 1, mcell->y1 + 1);
        g.setColour(Colour(0x3fFF6830));
        J_LineRect(g, mcell->x - 2, mcell->y - 2, mcell->x1 + 2, mcell->y1 + 2);
        //g.setColour(Colour(0x3ffF6830));
        //J_LineRect(g, mcell->x - 3, mcell->y - 3, mcell->x1 + 3, mcell->y1 + 3);
        //g.setColour(Colour(0x1fFF6830));
        //J_LineRect(g, mcell->x - 4, mcell->y - 4, mcell->x1 + 4, mcell->y1 + 4);
    }

    mcell = M.active_dropmixcell;
    if(mcell != NULL)
    {
        g.setColour(Colour(0xffFAE600));
        J_LineRect(g, mcell->x, mcell->y - 1, mcell->x1, mcell->y1 + 1);
    }

    mcell = M.active_mixcell;
    if(mcell != NULL && M.mixcellindexing)
    {
        g.setColour(Colour(0xffFFFFFF));
        J_TextSmall_xy(g, mcell->x + 2, mcell->y + 10, mcell->indexstr);
    }

    for(int xc = 0; xc < NUM_MIXER_COLUMNS; xc++)
    {
        for(int yc = 0; yc < NUM_MIXER_ROWS; yc++)
        {
            mcell = &(mix->r_cell[xc][yc]);
            if(mcell->outcell != NULL && mcell->outpin->connected == true)
            {
                if(mcell->outpin->connected == true)
                {
                    g.setColour(Colour(0xff8F8F8F));
                    J_FillRect(g, mcell->outpin->x + 2, 
                                    mcell->outpin->y + 2, 
                                    mcell->outpin->x + 6, 
                                    mcell->outpin->y + 6);
                    g.setColour(Colour(0xffFFFFFF));
                    J_FillRect(g, mcell->outpin->x + 3, 
                                    mcell->outpin->y + 3, 
                                    mcell->outpin->x + 5, 
                                    mcell->outpin->y + 5);
                }

                if(mcell->outcell->inpin->type == Ctrl_InPin_Bus)
                {
                    J_NodeBig(g, mcell->outcell->inpin->xc + mcell->outpin->xc_offs - 5,  mcell->outcell->inpin->yc + mcell->outpin->yc_offs - 5);
                }
                else
                {
                    J_NodeSmall(g, mcell->outcell->inpin->xc + mcell->outpin->xc_offs - 4,  mcell->outcell->inpin->yc + mcell->outpin->yc_offs - 1);
                }

                g.setColour(Colour(0xffFFFFFF));
                J_Line(g, mcell->outpin->xc - 1, mcell->outpin->yc,
                          mcell->outcell->inpin->xc + mcell->outpin->xc_offs, 
                          mcell->outcell->inpin->yc + mcell->outpin->yc_offs);
            }
        }
    }
    
    if(M.mmode & MOUSE_MIXING && M.mmode & MOUSE_CONTROLLING && M.active_ctrl->type == Ctrl_OutPin_Point)
    {
        OutPin* op = (OutPin*)M.active_ctrl;
        if(MixMasterHeight - mix->v_offs > 0 && op->yconn > WindHeight - (MixMasterHeight - mix->v_offs) - 1)
        {
            op->yconn = WindHeight - (MixMasterHeight - mix->v_offs) - 1;
        }

        g.setColour(Colour(0xffFFFFFF));
        if(op->connected == false)
        {
            J_Line(g, op->xc, op->yc, op->xconn, op->yconn);
            if(op->xconn != op->xc || op->yconn != op->yc)
            {
                g.setColour(Colour(0xff8F8F8F));
                J_FillRect(g, op->x + 2, 
                                op->y + 2, 
                                op->x + 6, 
                                op->y + 6);
                g.setColour(Colour(0xffFFFFFF));
                J_FillRect(g, op->x + 3, 
                                op->y + 3, 
                                op->x + 5, 
                                op->y + 5);
            }
        }

        J_LineRect(g, op->x, op->y, op->x + op->width, op->y + op->height);
        if(op->point_begin == false)
        {
            if(op->connected)
            {
                if(op->dst->type == Ctrl_InPin_Point)
                {
                    g.drawImageAt(img_inlight, op->dst->xc - 8 + op->xc_offs, op->dst->yc - 4 + op->yc_offs, false);
                }
                else if(op->dst->type == Ctrl_InPin_Bus)
                {
                    J_Circle(g, op->dst->xc - 1 + op->xc_offs, op->dst->yc - 1 + op->yc_offs, 10);
                }
            }
        }
    }
	*/
}

void J_MixCenter(Graphics& g)
{
    //ResetTrackControls();

    if(mixbrowse)
    {
        int yacc;
        //if(mixbrowse == false)
        {
            yacc = MainY1 - 1;
        }
        //else
        //{
        //    yacc = 0;
        //}


        {
            //J_Browser(g, mixBrw, MainX2, yacc + 18, MixCenterWidth + 2, GridY2 - (yacc + 17));
            J_Browser(g, mixBrw, MainX2 + 1, mixY + 23, MixCenterWidth + 2, mixH - brw_heading + 4);
            J_MixButtons(g, MainX2 + 2, mixY - 1);
/*
            g.setColour(Colour(10, 84, 96));
            J_LineRect(g, MainX2, yacc, MainX2 + MixCenterWidth, yacc + 17);
            if(strlen(mixBrw->sortstr->string) == 0)
            {
                g.setColour(Colour(33, 46, 56));
            }
            else
            {
                g.setColour(Colour(50, 20, 10));
            }
            J_FillRect(g, MainX2 + 1, yacc + 1, MainX2 + MixCenterWidth - 1, yacc + 16);
            g.setColour(Colour(244, 244, 244));
            J_TString_Base_xy(g, MainX2 + 2, yacc + 16, mixBrw->sortstr);
            mixBrw->sortstr->edx = MixCenterWidth - 4;
            mixBrw->sortstr->edy = 12;
*/
        }
    }
    /*
    // Mode name writing
    if(gAux->auxmode != AuxMode_Mixer)
    {
        int xb = MainX2 + MixCenterWidth - 58;
        int yb = MainY2 + 5;
        g.setColour(Colour(50, 114, 99));
        J_FillRect(g, xb, yb + 2, xb + 52, yb + 14);
        J_MixTxt(g, xb, yb);
    }
    */

    //J_Button(g, gAux->AccMixer, MainX2 + 5, yacc + 3, 33, 12);
    //J_Button(g, gAux->AccGrid, MainX2 + MixCenterWidth - 35, yacc + 3, 33, 12);
    //J_Button(g, gAux->AccCenter, MainX2 + 79, yacc + 3, 40, 17);
}

void J_Mixcells(Graphics& g, int xb, int yb, int xb_offs)
{
    mix->full_height = 0;
    bool visible;
    Mixcell* mcell;
    SliderBase* sl;
    SliderHPan* sl1;
    Toggle* mute;
    Toggle* solo;
    int ycell;
    int xcell = 0;
    int xbb;
    int yc, xc;
    for(xc = xb_offs; xc < (xb_offs + MixCellWidth*NUM_MIXER_COLUMNS); xc += MixCellWidth)
    {
        g.setColour(Colour(0xffFFFFFF));
        xbb = xb_offs + xcell*MixCellWidth + MixCellWidth + 7;
        J_InPin_mbus(g, xbb, yb + 12, mix->f_cell[xcell].inpin);
    
        mute = mix->f_cell[xcell].mute_toggle;
        solo = mix->f_cell[xcell].solo_toggle;
		sl = mix->f_cell[xcell].vol_slider;
        sl1 = mix->f_cell[xcell].pan_slider;

        if(xc < WindWidth && xbb + 50 > xb)
        {
            mix->f_cell[xcell].Enable();

            g.setColour(Colour(0xff000000));
            J_TextInstr_xy(g, xbb - 41, WindHeight - MixMasterHeight + 10 + mix->v_offs, mix->f_cell[xcell].indexstr);
            g.setColour(Colour(0xffFFFFFF));
            J_TextInstr_xy(g, xbb - 42, WindHeight - MixMasterHeight + 9 + mix->v_offs, mix->f_cell[xcell].indexstr);
    
            J_SliderVertical(g, sl, xbb - 4, WindHeight - 3 + mix->v_offs, 14);

            sl->Activate(sl->x - 13 + 11, sl->y - sl->height - 5, 18, 78 + 5,
                         xb - 1, WindHeight - MixMasterHeight + mix->v_offs, WindWidth, WindHeight);

            J_SliderMixPan(g, sl1, xbb - 28, WindHeight - MixMasterHeight + 14 + mix->v_offs, 8, true);
            sl1->Activate(sl1->x, sl1->y, sl1->width + 2, sl1->height + 2,
                          xb - 1, WindHeight - MixMasterHeight + mix->v_offs, WindWidth, WindHeight);

            g.setColour(Colour(20, 40, 35));
            J_FillRect(g, xbb - 10, WindHeight - MixMasterHeight - 1 + mix->v_offs,
                          xbb + 17, WindHeight - MixMasterHeight + 11 + mix->v_offs);

            //g.setColour(Colour(5, 66, 57));
            //J_FillRect(g, xbb + 3, WindHeight - MixMasterHeight - 3 + 1 + mix->v_offs,
            //              xbb + 4, WindHeight - MixMasterHeight - 3 + 12 + mix->v_offs);

            J_MuteToggle1(g, mute, xbb - 9, WindHeight - MixMasterHeight + mix->v_offs);
            mute->Activate(mute->x, mute->y, mute->width, mute->height,
                           xb - 1, WindHeight - MixMasterHeight + mix->v_offs, WindWidth, WindHeight);

            J_SoloToggle1(g, solo, xbb + 4, WindHeight - MixMasterHeight + mix->v_offs);
            solo->Activate(solo->x, solo->y, solo->width, solo->height,
                                      xb - 1, WindHeight - MixMasterHeight + mix->v_offs, WindWidth, WindHeight);

        }
        else
        {
            mix->f_cell[xcell].Disable();
        }
    
        visible = false;
        if((xc < (WindWidth))&&((xc + MixCellWidth - 1) > xb))
        {
            visible = true;
        }
    
        ycell = NUM_MIXER_ROWS - 1;
        yc = yb;        // for condition:  yc > (yb - MixCellBasicHeight*16)
        while(ycell >= 0)
        {
            mcell = &(mix->r_cell[xcell][ycell]);
            J_Mixcell(g, xc, yc, mcell, visible, xb);

            if(ycell < NUM_MIXER_ROWS - 1)
            {
                g.setColour(Colour(23, 36, 46));
                g.fillRect(xc + MixCellGap + 2, yc - 7, 94, 3);
                //g.setColour(Colour(48, 82, 94));
                //g.drawRect(xc + MixCellGap + 2, yc - 8, 94, 5);
            }

            yc -= mcell->height;
            ycell--;
        }
    
        if((WindHeight - yc) > mix->full_height)
        {
            mix->full_height = WindHeight - yc;
        }
    
        xcell++;
        if(xcell > NUM_MIXER_COLUMNS - 1)
        {
            break;
        }
    }
    
    // Draw master cells if they're visible
    if(mix->horiz_bar->offset + mix->horiz_bar->visible_len >= mix->master_offset)
    {
        xc += MixCellWidth + 10;
        ycell = NUM_MASTER_CELLS - 1;
        yc = yb;
        while(ycell >= 0)
        {
            mcell = &(mix->o_cell[ycell]);
            J_Mixcell(g, xc, yc, mcell, true, xb);
    
            yc -= mcell->height;
            ycell--;
        }
    
        g.setColour(Colour(52, 107, 127));
        for(yc = 0; yc < NUM_MASTER_CELLS; yc++)
        {
            mcell = &(mix->o_cell[yc]);
            if(mcell->outcell != NULL && mcell->outpin->connected == true)
            {
                J_Line(g, mcell->outpin->xc - 1, mcell->outpin->yc,
                        mcell->outcell->inpin->xc + mcell->outpin->xc_offs, mcell->outcell->inpin->yc + mcell->outpin->yc_offs);

                g.drawImageAt(img_node2, mcell->outcell->inpin->xc + mcell->outpin->xc_offs - 4, mcell->outcell->inpin->yc + mcell->outpin->yc_offs - 1, false);
            }
    
            mcell->outpin->active = false;
            mcell->inpin->active = false;
        }
    
        J_Line(g, mcell->outpin->xc - 1, mcell->outpin->yc, mcell->outpin->xc - 1, yb + 12);
        J_FillRect(g, mcell->outpin->xc - 6, yb + 10, mcell->outpin->xc + 3, yb + 12);
    
        sl = mix->m_cell.vol_slider;
        J_SliderVertical(g, sl, mcell->outpin->xc - 7, WindHeight - 3 + mix->v_offs, 14);
    
        sl->drawarea->Enable();
        sl->drawarea->SetBounds(sl->x - 16, sl->y - sl->height - 5, 47, 78 + 5);
    
        sl->vu->drawarea->Enable();
        sl->vu->drawarea->SetBounds(sl->x - 16, sl->y - sl->height - 5, 47, 78 + 5);
    }
    else
    {
        mix->m_cell.vol_slider->drawarea->Disable();
        mix->m_cell.vol_slider->vu->drawarea->Disable();
    
        for(int y = 0; y < NUM_MASTER_CELLS; y++)
        {
            mix->o_cell[y].Disable();
        }
    }
}

void J_Mixer(Graphics& g)
{
    int xb;
    xb = MainX2 + MixCenterWidth+ 8;

    MixX = xb - 1;
    MixY = 0;
    MixW = WindWidth - xb + 1;
    MixH = WindHeight;

    bool skip_mix_area;
    if((WindWidth - xb) <= 1)
    {
        skip_mix_area = true;
        mix->horiz_bar->active = false;
		mix->visible = false;

        for(int x = 0; x < NUM_MIXER_COLUMNS; x++)
        {
            mix->f_cell[x].Disable();
            for(int y = 0; y < NUM_MIXER_ROWS; y++)
            {
                mix->r_cell[x][y].Disable();
            }
        }

        for(int y = 0; y < NUM_MASTER_CELLS; y++)
        {
            mix->o_cell[y].Disable();
        }

        mix->m_cell.Disable();
    }
    else
    {
        skip_mix_area = false;
        mix->visible = true;
    }

    if(skip_mix_area == false)
    {
        g.setColour(Colour(0xff3264A0));
        //J_Line(g, xb - 2, 28, WindWidth, 28);

        g.setColour(Colour(0xff3C3C64));
        //J_Line(g, xb - 2, 29, WindWidth, 29);

        int yb = WindHeight - 1 - MixMasterHeight - 6 + mix->v_offs;
        mix->horiz_bar->visible_len = (float)(WindWidth - xb);
        if(mix->horiz_bar->visible_len > 120)
        {
            mix->horiz_bar->active = true;
            J_ScrollBard(g, xb, WindWidth - 1, 0, mix->horiz_bar, false, true);
        }
        else
        {
            mix->horiz_bar->active = false;
        }

        g.saveState();
        g.reduceClipRegion(xb - 1, 27, WindWidth - xb + 1, WindHeight - 27);

        g.setColour(Colour(0xff000000));
        g.fillRect(xb - 1, 27, WindWidth - xb + 1, WindHeight - 27);

        int xb_offs = xb - (int)(mix->horiz_bar->offset);

        // Master area
        J_PanelRect(g, xb_offs, WindHeight - 1 - MixMasterHeight + mix->v_offs, WindWidth, WindHeight - 1 - MixMasterHeight + 120 + mix->v_offs);

        J_Mixcells(g, xb, yb, xb_offs);

        MC->MakeSnapshot(&MC->mixer, xb - 1, 29, WindWidth - xb + 1, WindHeight - 29 - ((MixMasterHeight - mix->v_offs) > 0 ? (MixMasterHeight - mix->v_offs) : 0));

        J_MixerConnectionsAndHighlights(g);

        g.restoreState();
    }
}

void J_Menu(Graphics& g, Menu* mn)
{
    switch(mn->mtype)
    {
        case Menu_Main:
        {
            if(mn == CP->MainMenu)
            {
                J_MainMenu(g, mn, mn->x, mn->y, false);
            }
            else
            {
                J_ContextMenu(g, mn);
            }
        }break;
        case Menu_Context:
        {
            J_ContextMenu(g, mn);
        }break;
        case Menu_Quant:
        {
            if(mn == CP->qMenu)
            {
                J_QuantMenu(g, CP->qMenu->x, CP->qMenu->y, CP->qMenu);
            }
            else if(mn == gAux->qMenu)
            {
                J_QuantMenu(g, gAux->qMenu->x, gAux->qMenu->y - 20 + gAux->qMenu->height, gAux->qMenu);
            }
        }break;
    }

    if(mn->panel == CP)
    {
        CP->CheckControlVisibility(mn);
    }
}

void J_RefreshButtons(Graphics& g)
{
    Butt* bt;
    Control* ct = firstCtrl;
    while(ct != NULL)
    {
        if(ct->active == true && (ct->type == Ctrl_Button || ct->type == Ctrl_ButtFix))
        {
            bt = (Butt*)ct;
            if(bt->needs_refresh == true)
            {
                //bt->needs_refresh = false;
                g.saveState();
                g.reduceClipRegion(bt->drawarea->ax, bt->drawarea->ay, bt->drawarea->aw, bt->drawarea->ah);
                J_Button(g, bt, bt->x, bt->y, bt->width, bt->height);
                g.restoreState();
            }
        }
        ct = ct->next;
    }
}

void J_Menus(Graphics& g)
{
    DrawArea* da = MC->first_drawarea;
    while(da != NULL)
    {
        if(da->isEnabled() == true && da->type == Draw_Menu)
        {
            g.saveState();
            g.reduceClipRegion(da->ax, da->ay, da->aw, da->ah);
            Menu* menu = (Menu*)da->drawobject;
            J_Menu(g, menu);
            da->Change();
            g.restoreState();
        }
        da = da->next;
    }
}

INLINE void J_Separates(Graphics& g)
{
    bool hlrenew = false;

    DrawArea* da = MC->first_drawarea;
    while(da != NULL)
    {
        if(da->isEnabled() == true && da->redraw == true && da->type != Draw_Menu)
        {
            da->redraw = false;
            g.saveState();
            g.reduceClipRegion(da->ax, da->ay, da->aw, da->ah);
            if(da->type == Draw_VertSlider)
            {
                SliderBase* sl = (SliderBase*)da->drawobject;
                if(sl->scope == &CP->scope)
                {
                    J_SliderVertical_Main(g, sl, sl->x, sl->y, sl->width);
                }
                else
                {
                    J_SliderVertical(g, sl, sl->x, sl->y, sl->width);
                }
            }
            else if(da->type == Draw_ZoomSlider)
            {
                SliderZoom* sl = (SliderZoom*)da->drawobject;
                J_SliderZoom(g, sl, sl->x, sl->y);
            }
            else if(da->type == Draw_SliderHVol)
            {
                SliderHVol* sl = (SliderHVol*)da->drawobject;
                if(sl->scope->instr != NULL)
                {
                    J_SliderInstrVol(g, sl, sl->scope->instr, sl->x, sl->y);

                    //g.setColour(Colour(0xFF000000));
                    //g.fillRect(da->ax, da->ay, da->aw, da->ah);
                }
                else if(sl->scope->for_track == true)
                {
                    J_SliderTrackVol(g, sl, sl->x, sl->y);
                }
            }
            else if(da->type == Draw_SliderHPan)
            {
                SliderHPan* sl = (SliderHPan*)da->drawobject;
                bool rect = (sl->scope->for_track || sl->scope->mixcell != NULL);
                if(sl->scope->instr != NULL)
                {
                    J_SliderInstrPan(g, sl, sl->scope->instr, sl->x, sl->y, rect);
                }
                else if(sl->scope->for_track == true)
                {
                    J_SliderTrackPan(g, sl, sl->x, sl->y, rect);
                }
                else
                {
                    J_SliderMixPan(g, sl, sl->x, sl->y, sl->height, false);
                }
            }
            else if(da->type == Draw_Knob)
            {
                Knob* kb = (Knob*)da->drawobject;
				if(kb->ktype == Knob_Large)
                {
                    J_KnobLarge(g, kb, false, kb->x, kb->y);
                }
                else if(kb->ktype == Knob_XLarge)
                {
                    J_KnobXLarge(g, kb, false, kb->x, kb->y);
                }
                else if(kb->ktype == Knob_ZLarge)
                {
                    J_KnobZLarge(g, kb, false, kb->x, kb->y);
                }
                else if(kb->ktype == Knob_Small)
                {
                    J_KnobSmall(g, kb, kb->x, kb->y);
                }
            }
            else if(da->type == Draw_DigitStr)
            {
                DigitStr* dstr = (DigitStr*)da->drawobject;
                J_DigitStr_xy(g, dstr->x, dstr->y, dstr, dstr->bright);
            }
            else if(da->type == Draw_Toggle)
            {
                Toggle* tg = (Toggle*)da->drawobject;
                if(tg->tgtype == Toggle_Mute)
                {
                    J_MuteToggle(g, tg, tg->x, tg->y);
                }
                else if(tg->tgtype == Toggle_Mute1)
                {
                    J_MuteToggle1(g, tg, tg->x, tg->y);
                }
                else if(tg->tgtype == Toggle_Solo)
                {
                    J_SoloToggle(g, tg, tg->x, tg->y);
                }
                else if(tg->tgtype == Toggle_Solo1)
                {
                    J_SoloToggle1(g, tg, tg->x, tg->y);
                }
                else if(tg->tgtype == Toggle_AutoSwitch)
                {
                    J_AutoSwitchToggle(g, tg->x, tg->y, tg);
                }
                else if(tg->tgtype == Toggle_RelSwitch)
                {
                    J_RelSwitchToggle(g, tg->x, tg->y, tg);
                }
                else if(tg->tgtype == Toggle_MixBypass)
                {
                    J_MixBypassToggle(g, tg->x, tg->y, tg);
                }
                else if(tg->tgtype == Toggle_EffBypass)
                {
                    J_EffBypassToggle(g, tg->x, tg->y, tg);
                }
            }
            else if(da->type == Draw_Instrument)
            {
                Instrument* instr = (Instrument*)da->drawobject;
                J_Instr(g, instr->x, instr->y, instr);
            }
            else if(da->type == Draw_Mixcell)
            {
                Mixcell* mc = (Mixcell*)da->drawobject;
                J_Mixcell(g, mc->x - MixCellGap - 1, mc->y + mc->height + 2, mc, mc->visible, MixX + 1);
            }
            else if(da->type == Draw_Param)
            {
                Paramcell* pcell = (Paramcell*)da->drawobject;

                if(pcell->panel != NULL)
                {
                    if(pcell->panel->type == Panel_Browser)
                    {
                        J_BrwParamCell(g, pcell, pcell->x, pcell->y);
                    }
                    else if(pcell->panel->type == Panel_MixCell)
                    {
                        J_EffParamCell(g, pcell, pcell->x, pcell->y, pcell->w);
                    }
                }
				else
                {
                    J_EffParamCell(g, pcell, pcell->x, pcell->y, pcell->w);

                    J_MChanArrows(g, pcell->scope->mixcell->mchan);
                }
			}
            else if(da->type == Draw_Timer)
            {
                J_Timer(g, CP->tmx, CP->tmy);
            }
            else if(da->type == Draw_Numba)
            {
                Numba* num = (Numba*)da->drawobject;
                J_NumbaNum(g, num, num->x, num->y);
            }
            else if(da->type == Draw_VU)
            {
                VU* vu = (VU*)da->drawobject;
                if(vu->vtype == VU_Big)
                {
                    J_BigVU(g, vu, vu->x, vu->y, vu->h, vu->oh);
                }
                else if(vu->vtype == VU_BigMain)
                {
                    J_BigVU(g, vu, vu->x, vu->y, vu->h, vu->oh, true);
                }
                else if(vu->vtype == VU_Instr)
                {
                    J_InstrVU(g, vu->x, vu->y, vu);
                }
                else if(vu->vtype == VU_StepLine)
                {
                    J_StepVU1(g, vu->x, vu->y, vu);
                }
            }
            else if(da->type == Draw_MixChannel)
            {
                MixChannel* mchan = (MixChannel*)da->drawobject;
                J_MixChannel(g, mchan->x, mchan->y, mchan->height, mchan, mchan->x1r, mchan->y1r, mchan->x2r, mchan->y2r);
                MC->listen->auxhighlights_redraw = true;
            }
            else if(da->type == Draw_ScrollBar)
            {
                ScrollBard* sbar = (ScrollBard*)da->drawobject;
                if(sbar->type == Ctrl_VScrollBar)
                {
                    J_ScrollBard(g, sbar->y1, sbar->y2, sbar->x - 1, sbar, sbar->fixed, sbar->rect);
                }
                else if(sbar->type == Ctrl_HScrollBar)
                {
                    J_ScrollBard(g, sbar->x1, sbar->x2, sbar->y - 1, sbar, sbar->fixed, sbar->rect);
                }
            }

            g.restoreState();

            // Update mixer snapshot
            //if(da->ax >= MixX && ((da->ay + da->ah) < (WindHeight - 1 - MixMasterHeight + mix->v_offs)))
            //{
            //    CopyImageSection(MC->bufferedImage, MC->mixer, da->ax, da->ay, da->ax - MixX, da->ay - 29, da->aw, da->ah);
            //    hlrenew = true;
            //}
        }
        da = da->next;
    }

    if(hlrenew)
    {
        g.saveState();
        g.reduceClipRegion(MixX, 29, MixW, MixH - 29);
        J_MixerConnectionsAndHighlights(g);
        g.restoreState();
    }
}

void Init_Fonts()
{
	MemoryInputStream fontStream5(tahoma::fontbin_bin, tahoma::fontbin_binSize, false );
#ifdef USE_OLD_JUCE
	Typeface* tf = new Typeface(fontStream5);
	taho = new Font(*tf);
	delete tf;
#else
	CustomTypeface* tf = new CustomTypeface(fontStream5);
	taho = new Font(tf);
#endif
	taho->setHeight(20.0f);
	taho->setHorizontalScale(1.0f);

	MemoryInputStream fontStream6(tiny::fontbin_bin, tiny::fontbin_binSize, false );
#ifdef USE_OLD_JUCE
	tf = new Typeface(fontStream6);
	ti = new Font(*tf);
	delete tf;
#else
	tf = new CustomTypeface(fontStream6);
	ti = new Font(tf);
#endif
	ti->setHeight(13.0f);
	ti->setHorizontalScale(1.0f);

	MemoryInputStream fontStream7(aripix::fontbin_bin, aripix::fontbin_binSize, false );
#ifdef USE_OLD_JUCE
	tf = new Typeface(fontStream7);
	ari = new Font(*tf);
	delete tf;
#else
	tf = new CustomTypeface(fontStream7);
	ari = new Font(tf);
#endif
	ari->setHeight(12.0f);
	ari->setHorizontalScale(1.0f);

	MemoryInputStream fontStream10(instrum::fontbin_bin, instrum::fontbin_binSize, false );
#ifdef USE_OLD_JUCE
	tf = new Typeface(fontStream10);
	ins = new Font(*tf);
	delete tf;
#else
	tf = new CustomTypeface(fontStream10);
	ins = new Font(tf);
#endif
	ins->setHeight(10.0f);
	ins->setHorizontalScale(1.0f);

	MemoryInputStream fontStream11(tiny1::fontbin_bin, tiny1::fontbin_binSize, false );
#ifdef USE_OLD_JUCE
	tf = new Typeface(fontStream11);
	ti1 = new Font(*tf);
#else
	tf = new CustomTypeface(fontStream11);
	ti1 = new Font(tf);
#endif
	ti1->setHeight(13.0f);
	ti1->setHorizontalScale(1.0f);

	MemoryInputStream fontStream12(projs::fontbin_bin, projs::fontbin_binSize, false );
#ifdef USE_OLD_JUCE
	tf = new Typeface(fontStream12);
	prj = new Font(*tf);
#else
	tf = new CustomTypeface(fontStream12);
	prj = new Font(tf);
#endif
	prj->setHeight(12.0f);
	prj->setHorizontalScale(1.0f);

	MemoryInputStream fontStream13(roxx::fontbin_bin, roxx::fontbin_binSize, false );
#ifdef USE_OLD_JUCE
	tf = new Typeface(fontStream13);
	rox = new Font(*tf);
#else
	tf = new CustomTypeface(fontStream13);
	rox = new Font(tf);
#endif
	rox->setHeight(11.0f);
	rox->setHorizontalScale(1.0f);

	MemoryInputStream fontStream14(dpix::fontbin_bin, dpix::fontbin_binSize, false );
#ifdef USE_OLD_JUCE
	tf = new Typeface(fontStream14);
	dix = new Font(*tf);
#else
	tf = new CustomTypeface(fontStream14);
	dix = new Font(tf);
#endif
	dix->setHeight(13.0f);
	dix->setHorizontalScale(1.0f);

	MemoryInputStream fontStream15(fixed::fontbin_bin, fixed::fontbin_binSize, false );
#ifdef USE_OLD_JUCE
	tf = new Typeface(fontStream15);
	fix = new Font(*tf);
#else
	tf = new CustomTypeface(fontStream15);
	fix = new Font(tf);
#endif
	fix->setHeight(9.0f);
	fix->setHorizontalScale(1.0f);

	MemoryInputStream fontStream16(Bold::fontbin_bin, Bold::fontbin_binSize, false );
#ifdef USE_OLD_JUCE
	tf = new Typeface(fontStream16);
	bld = new Font(*tf);
#else
	tf = new CustomTypeface(fontStream16);
	bld = new Font(tf);
#endif
	bld->setHeight(11.0f);
	bld->setHorizontalScale(1.0f);

	MemoryInputStream fontStream17(visitor::fontbin_bin, visitor::fontbin_binSize, false );
#ifdef USE_OLD_JUCE
	tf = new Typeface(fontStream17);
	vis = new Font(*tf);
#else
	tf = new CustomTypeface(fontStream17);
	vis = new Font(tf);
#endif
	vis->setHeight(9.0f);
	vis->setHorizontalScale(1.0f);
}

void J_TestFonts(Graphics& g)
{
    /*
    g.setColour(Colour(0xffFFFFFF));
	int x = 222;
	int y = 222;

	g.setFont((*micro));
    g.drawSingleLineText(String("tada hh tst c s cl r"), x, y + 42);
    g.setFont((*visi));
    g.drawSingleLineText(String("tada hh tst c s cl r"), x, y + 52);
    g.setFont((*ti));
    g.drawSingleLineText(String("tada hh tst c s cl r"), x, y + 62);
    g.setFont((*ari));
    g.drawSingleLineText(String("tada hh tst c s cl r"), x, y + 82);
    g.setFont((*taho));
    g.drawSingleLineText(String("tada hh tst c s cl r"), x, y + 92);

    int kk = 0;
    for(float h = 6; h < 21; h += 1)
    {
        taho->setHeight((float)h);
        g.setFont(*taho);

        g.drawSingleLineText(String("Taho Arial Master Font 12345678 Quick Brown Fox The PSP Saturation"), x, y + 92 + kk);
        kk += 22;
    }
    */
}

void J_UnderPanel(Graphics& g)
{
    J_PanelRectBW(g, MainX1, GridY1 + (field->num_lines - OffsLine)*lineHeight, MainX2 + MixCenterWidth, GridY2);
}

void J_UnderPanelRefresh()
{
    int y1 = GridY1 + (field->num_lines - OffsLine)*lineHeight;
    MC->listen->repaint(MainX1, y1, MainX2 + MixCenterWidth - MainX1, GridY2 - y1 + 1);
}

void J_DrawAll(Graphics& g)
{
    J_CtrlPanel(g);

    AuxCheck();

    J_Grid(g);

    J_Content_Main(g);

    //J_InstrCenter(g, MainX1 - InstrPanelWidth - GenBrowserGap - GenBrowserWidth + 5 - InstrCenterOffset, MainY1 - 2);
    J_InstrCenter(g, 0, MainY1 - 2);

    J_InstrPanel(g, MainX1 - InstrPanelWidth - InstrCenterOffset, MainY1 - 2);

    J_Aux(g);

    J_MainScale(g);

    //J_Mixer(g);
    J_MixCenter(g);

    if(J_IsUnder())
    {
        J_UnderPanel(g);
    }

    if(M.menu_active == true && M.contextmenu != NULL)
    {
        Menu* mn = M.contextmenu;
        J_ContextMenu(g, mn);

        mn->drawarea->Enable();
        mn->drawarea->SetBounds(mn->x, mn->y, mn->width + 1, mn->height + 1);
    }

    if(M.hint_active == true)
    {
        J_DrawHint(g, M.active_ctrl, M.mouse_x, M.mouse_y);
    }
}

void J_UnderDrawAll(Graphics& g)
{
    // This is very useful function for navigation in Source Insight
}

bool J_IsUnder()
{
    if(OffsLine + numFieldLines + 1 > field->num_lines)
    {
        return true;
    }
    else
    {
        return false;
    }
}

