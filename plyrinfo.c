/*
 *		$Archive: /video/Nba/plyrinfo.c $
 *		$Revision: 261 $
 *		$Date: 4/22/99 5:59p $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifdef INCLUDE_SSID
char *ss_plyrinfo_c = "$Workfile: plyrinfo.c $ $Revision: 261 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif

/*
 *		USER INCLUDES
 */

#include "fontid.h"
#include "id.h"
#include "ani3D.h"
#include "player.h"
#include "game.h"
#include "coin.h"
#include "mainproc.h"
#include "sndcalls.h"
#include "audits.h"
#include "select.h"
#include "externs.h"
#include "transit.h"
#include "putil.h"

#define	IMAGEDEFINE	1
#include "plyrinfo.h"
#include "scorinfo.h"
#include "plyrtext.h"


#ifndef RELEASE_VER
 #ifndef TEST_VER
  #ifdef DEBUG
//	#define D_ARROW			// Show off-screen arrow info
//	#define DBG_EOQ			// Enable EOQ camera editor
  #endif //DEBUG
 #endif //TEST_VER
#endif //RELEASE_VER

// Uncomment to see shot clock timer always; the text flasher will
//  get killed after 1st period, but thats OK
//#define DEBUG_SHOT_CLOCK_BOX


/*
** Referenced data & function prototypes
*/
void hide_string(void *);
void unhide_string(void *);

void plyr_strt_butn(int sig, int swid);
void no_start(int sig, int swid);

extern player_record_info selected_players[];

extern char index_2p[NUM_PLAYERS];
extern char shift_2p[NUM_PLAYERS];

extern image_info_t playresult;

extern int pup_no_fouls,pup_infturbo;
extern int last_handler;

extern int cmos_ticks_per_second;
extern what_type_shot;

extern sprite_node_t * alpha_sprite_node_list;
extern sprite_node_t * sprite_node_list;
extern VIEWCOORD cambot;

extern void win_game_speech_proc(int *args);
extern void goto_freethrow_line_speech(void);

extern void freethrow_taunt_speech_proc(void);

extern char gameplay_bnk_str[];
extern char plyr_bnk_str[];
extern int pre_tipoff_movie_delay;
extern int pup_tourney;
extern int p_status;
extern int tsec;
extern int is_low_res;
extern float hres, vres;
extern float gTm1_meter_height, gTm2_meter_height;
extern int	reset_shotclock;

extern int gTm1_meter_cnt;
extern int gTm2_meter_cnt;

extern pdata_t pdata_blocks[];

extern char anc_misc_bnk[];
extern char cursor_bnk_str[];
extern void wait_for_any_butn_press_active_plyr(int, int, int);
extern void disc_flash_3pt_proc(int *args);
extern int select_csi_item(CAMSEQINFO * pcsi);
extern CAMSEQINFO csi_fs_endqtr[];
extern CAMSEQINFO csi_rs_suplex[];
extern CAMSEQINFO csi_fs_endplay[];

int check_scores(void);
int compute_score(int);
void free_throw_msg_proc(int *args);
void show_player_names_proc(int *);			// from plyrinfo.c
void change_img_tmu(sprite_info_t *obj, image_info_t *ii, int tid);
void wait_for_all_buttons( int, int );
void tipoff_idiot_box_proc(int *args);
void show_3pt_proc(int *args);
void show_tmfire_proc(int *args);
void plr_tip_idiot_box_proc(int *args);
void opening_msg_proc(int *);
void flash_basketball_proc(int *args);

//extern image_info_t turbebar;				// message bar frame
//extern image_info_t turbecre;				// "credits:"
//extern image_info_t turbeins;				// "insert coins"
//extern image_info_t turbejoi;				// "to join in"
//extern image_info_t turbeple;				// "please wait"
//extern image_info_t turbepre;				// "press start"

//extern image_info_t turbfbar;				// turbo bar frame
//extern image_info_t turbfblu;				// blue turbo bar
//extern image_info_t turbfred;				// red turbo bar


/*
** Global data
*/
struct tagJMETER jmeter[2];					// Jump-meter info
struct image_info jmeter_ii[2];				// Changeable copies of meter image info
int jump_meters_on = 0;
int hide_peacock = 0;

ARROWINFO arrowinfo[NUM_PLAYERS];

int update_status_box, show_score_plate;

int curr_shot_pct;							// Shot %
int show_shot_pct;							// Ticks to display %

int ftmtr_per;								// Accuracy % for kick meter
int showftmtr_flag = 0;						// Hide-state for Freethrow meter
int showhide_flag   = SHOW;					// Hide-state for Turbo bars & arrows
int hide_status_box = SHOW;					// Hide-state for Top score box
int hide_arrows_one_tick = SHOW;			// Hide-state for arrows; <update_arrows_proc> unhides every tick
int ball_flash_timer = 0;					// 1=ball is flashing from a score - don't hide img

static int flash_ft_letters = FALSE;
static int flash_gt_letters = FALSE;
static int flash_sc_letters = FALSE;
static int flash_message_active = FALSE;
int	idiot_boxes_shown = 0;


/*
** Local data & function prototypes
*/
#ifdef DBG_KO
#define DBG_KO_BOOL	dbg_ko
extern int dbg_ko;
#else
#define DBG_KO_BOOL	0
#endif

#define STICK_CAM_BT	P_A					// Btn used in DBG_EOQ
#define STICK_CAM_SW	4					// Stk used in DBG_EOQ

#define X_VAL	0
#define Y_VAL	1

#ifndef DEBUG_SHOT_CLOCK_BOX
#define SHOT_CLOCK_DIGCNT	1				// # of digits: shot clock
#else
#define SHOT_CLOCK_DIGCNT	2				// # of digits: shot clock
#endif

#define SCRPLT_SCR_DIGCNT	3				// # of digits: scoreplate scores
#define SCRPLT_MIN_DIGCNT	2				// # of digits: scoreplate minutes
#define SCRPLT_SEC_DIGCNT	2				// # of digits: scoreplate seconds

#define JOININ_TIMER	70					// ticks for info cycle
#define CREDIT_TIMER	120					// ticks for credits
#define COININ_TIMER	300					// ticks for new credits
#define CREDIT_CYCLE	4					// should always be even

#define MSG_Z		200.1F //200.1f	//1.75f

#define BARX1		128.0f
#define BARX2		384.0f

#define BARX1_K		53.0f
#define BARX2_K		148.0f
#define BARX3_K		364.0f
#define BARX4_K		459.0f

#define BARY		348.0f

#define STBX_X		256.0f
#define STBX_Y		354.0f
#define STBX_Y_LR	240.0f
#define STBX_Z		2.0f

#define SHOT_X		0.0f
#define SHOT_Y		380.0f

//#define CREDSHOW_FONT		set_text_font(FONTID_BAST8T)
//#define CREDSHOW_KILL(csid)	delete_multiple_strings(\
//								CREDIT_MSGS_ID + (int)(csid), 0xffffffff)
//#define CREDSHOW_MAKE(csid)	set_string_id(\
//								CREDIT_MSGS_ID + (int)(csid));\
//							set_text_position(\
//								(float)(48+(int)pintx_turbo[/*four_plr_ver*/1][(int)(csid)]),\
//								(float)(pinty_turbo[1/*cambot.csi.mode == SIDELINE_CAM*/ ? 0:1][!!is_low_res]-6),\
//								1.1f);\
//							{\
//								int ilr=is_low_res;\
//								is_low_res=0;\
//								oprintf("%dj%dc %d",\
//									(HJUST_CENTER | VJUST_BOTTOM),\
//									LT_YELLOW,\
//									get_total_credits());\
//								is_low_res=ilr;\
//							}

// Protos

void freethrow_meter_exit(struct process_node *, int);
void freethrow_meter_proc(int *args);

void flash_obj_white(sprite_info_t *, int, int);
static void shrink_ft_letter_proc( int * );
static void shrink_sc_letter_proc( int * );
static void shrink_gt_letter_proc( int * );
static void shrink_gt_letter2_proc( int * );
static void GT_snd_proc(int *args);
void shot_clock_box_proc(int *args);
void change_alpha_cc(void * parent, int cc);
void change_ostring_value(sprite_node_t * psn, ostring_t * string, int value, int digmin, float xpos, int justify);
void update_ostring_value(ostring_t * string, int value, int digmin);
void update_ostring_value_nonalpha(ostring_t * string, int value, int digmin);
void eoq_score_plate_proc(int *args);
void score_plate_proc(int *args);
void show_peacock_proc(int *args);
void violation_proc(int *args);
void score_status_box(int *args);
static void plyrinfo_proc(int *parg);
void score_plate(int *args);
void plyrinfo_init(void);
static void update_arrows_proc(int *);
void show_player_names_proc(int *parg);
void flash_text_proc(int *args);
void flash_alpha_text_proc(int *args);
void flash_text_slow_proc(int *args);



extern image_info_t flarefl01;
extern image_info_t flarefl02;
extern image_info_t flarefl03;
extern image_info_t flarefl04;
extern image_info_t flarefl05;
extern image_info_t flarefl06;
extern image_info_t flarefl07;
extern image_info_t flarefl08;
extern image_info_t flarefl09;
extern image_info_t flarefl10;
extern image_info_t flarefl11;
extern image_info_t flarefl12;
extern image_info_t flarefl13;
extern image_info_t flarefl14;
extern image_info_t flarefl15;


// Data/variables

static int plr_color[] = {LT_BLUE, LT_GREEN, LT_RED, LT_YELLOW};

static image_info_t *status_box_imgs[] = {
	&scorclk2, &scorclk2, &shtprctbk, &shtprctbk
};
static image_info_t *qtr_imgs[] = {
	&sp_1st, &sp_2nd, &sp_3rd, &sp_4th,
	&sp_ot,  &sp_ot,  &sp_ot,  &sp_ot
};
// For EOQ score plate
static image_info_t *qtr2_imgs[] = {
	&endqtr1,  &endqtr2,  &endqtr3,  &endqtr4,
	&endqtrot, &endqtrot, &endqtrot, &endqtrot
};

static image_info_t * pii_turbo[MAX_PLYRS][2] = {	// [plyr#][res]
	{&trboblu2, &trboblu2},
	{&trbogrn2, &trbogrn2},
	{&trbored2, &trbored2},
	{&trboylw2, &trboylw2}
};

static image_info_t * pii_ft_box[MAX_PLYRS][2] = {	// [plyr#][res]
	{&t_foul_b, &t_foul_b2},
	{&t_foul_g, &t_foul_g2},
	{&t_foul_r, &t_foul_r2},
	{&t_foul_y, &t_foul_y2}
};

static image_info_t * pii_no_ft_box[MAX_PLYRS][2] = {	// [plyr#][res]
	{&turbo_b, &turbo_b},
	{&turbo_g, &turbo_g},
	{&turbo_r, &turbo_r},
	{&turbo_y, &turbo_y}
};


static image_info_t * trb_bar_fire_imgs[] = {
	&firbar01,&firbar02,&firbar03,&firbar04,&firbar05,
	&firbar06,&firbar07,&firbar08,&firbar09,&firbar10,
	&firbar11,&firbar12,&firbar13,&firbar14,&firbar15
};
static image_info_t * trb_bar_bfire_imgs[] = {
	&bfirbar01,&bfirbar02,&bfirbar03,&bfirbar04,&bfirbar05,
	&bfirbar06,&bfirbar07,&bfirbar08,&bfirbar09,&bfirbar10,
	&bfirbar11,&bfirbar12,&bfirbar13,&bfirbar14,&bfirbar15
};

static image_info_t * foul_num_imgs[] = {&t_0,&t_1,&t_2,&t_3,&t_4,&t_5,&t_6,&t_7,&t_8,&t_9};


#define	FIRE_FRAMES	(sizeof(trb_bar_fire_imgs)/sizeof(void *))
#define	FIRE_FDELAY 3


int pintx_turbo[MAX_PLYRS][2] = {			// [plyr#][2p,4p]
	{BARX1, BARX1_K},
	{-100,  BARX2_K},
	{BARX2, BARX3_K},
	{-100,  BARX4_K}
};

int pinty_turbo[][2] = {						// [obj][res]
	{BARY,      (((BARY +  0)*2)/3.0f)-0.0f},	// turbo frame/bar
	{BARY + 12, (((BARY + 12)*2)/3.0f)-0.0f},	// msg1
	{BARY +  0, (((BARY +  0)*2)/3.0f)-0.0f},	// msg2
};	

#if NUM_PERTEAM > 2
static sprite_info_t * rcvra_obj;
static sprite_info_t * rcvrb_obj;
//static sprite_info_t * pass_word_obj;
#endif

//static image_info_t ii_arr[MAX_PLYRS+1];		// dynamic copies of arrow img info
//static image_info_t ii_num[MAX_PLYRS+1];		// dynamic copies of arrow # img info
static image_info_t ii_turbo[MAX_PLYRS];		// dynamic copies of turbo bar img info


static float violation_xys[2][2][2] = {			// [vio #][res][x,y]
	{ {120, 100}, {120, 100} },					// 0 Goal Tending
	{ { 40,  25}, {120,  25} },					// 1 Shot Clock Violation
};

static float shotclock_box_xys[][2][2] = {									// [obj #][res][x,y]
	{ {SHOT_X+21, SHOT_Y+12 }, {SHOT_X+21, (((SHOT_Y+ 12)*2)/3.0f)-0.0f} },	// shotclock box itself
	{ {SHOT_X+50, SHOT_Y-351}, {SHOT_X+50, (((SHOT_Y-351)*2)/3.0f)-0.0f} },	// shot clock time
};

static float status_box_xys[][2][2] = {									// [obj#][res][x,y]
	{ {STBX_X+ 0,  STBX_Y+ 0 }, {STBX_X+ 0,  STBX_Y_LR+ 0 } },			//  0 status box itself
	{ {STBX_X- 35, STBX_Y- 2 }, {STBX_X- 35, STBX_Y_LR- 9 } },			//  1 team 1 score
	{ {STBX_X+ 37, STBX_Y- 2 }, {STBX_X+ 36, STBX_Y_LR- 9 } },			//  2 team 2 score
	{ {STBX_X+ 0,  STBX_Y+ 6 }, {STBX_X+ 0,  STBX_Y_LR+ 6 } },			//  3 quarter number str.
	{ {STBX_X- 6,  STBX_Y- 21}, {STBX_X- 4,  STBX_Y_LR- 21+4} },		//  4 minutes
	{ {STBX_X- 1,  STBX_Y- 17}, {STBX_X+ 1,  STBX_Y_LR- 21+5} },		//  5   :
	{ {STBX_X- 1,  STBX_Y- 21}, {STBX_X+ 3,  STBX_Y_LR- 21+4} },		//  6   seconds
	{ {STBX_X+ 3,  STBX_Y- 21}, {STBX_X+ 3,  STBX_Y_LR- 21+4} },		//  7 seconds
	{ {STBX_X+ 7,  STBX_Y- 18}, {STBX_X+ 7,  STBX_Y_LR- 21+5} },		//  8   .
	{ {STBX_X+ 7,  STBX_Y- 21}, {STBX_X+ 7,  STBX_Y_LR- 21+4} },		//  9   tenths
	{ {STBX_X+ 0,  STBX_Y+ 0 }, {STBX_X+ 0,  STBX_Y_LR+ 0 } },			// 10 shot % backdrop
	{ {STBX_X+ 1,  STBX_Y- 31}, {STBX_X+ 1,  STBX_Y_LR- 24} },			// 11 shot % text
};

// For game-time score plate
#define SCRPLT1_X	18
#define SCRPLT1_Y	384

static int fid_scrplt1[] = {											// [res]
	FONTID_NBA14, FONTID_NBA10
};
static float scrplt1_xys[][2][2] = {														// [obj#][res][x,y]
	{ {SCRPLT1_X,    SCRPLT1_Y    }, {SCRPLT1_X,    SCRPLT1_Y    } },						// 0 plate overlay
	{ {SCRPLT1_X+362,SCRPLT1_Y-340}, {SCRPLT1_X+362,SCRPLT1_Y-340} },						// 1 1st, 2nd, 3rd, 4th or OT (image)
	{ {SCRPLT1_X+350,SCRPLT1_Y-298}, {SCRPLT1_X+350,(((SCRPLT1_Y-298)*2.0f)/3.0f)-1.0f} },	// 2 team 1 city name
	{ {SCRPLT1_X+458,SCRPLT1_Y-297}, {SCRPLT1_X+458,(((SCRPLT1_Y-298)*2.0f)/3.0f)-1.0f} },	// 3 team 1 score
	{ {SCRPLT1_X+350,SCRPLT1_Y-323}, {SCRPLT1_X+350,(((SCRPLT1_Y-323)*2.0f)/3.0f)-1.0f} },	// 4 team 2 city name
	{ {SCRPLT1_X+458,SCRPLT1_Y-322}, {SCRPLT1_X+458,(((SCRPLT1_Y-323)*2.0f)/3.0f)-1.0f} },	// 5 team 2 score
	{ {SCRPLT1_X+424,SCRPLT1_Y-350}, {SCRPLT1_X+424,(((SCRPLT1_Y-350)*2.0f)/3.0f)-1.0f} },	// 6 minutes
	{ {SCRPLT1_X+431,SCRPLT1_Y-345}, {SCRPLT1_X+431,(((SCRPLT1_Y-345)*2.0f)/3.0f)-2.0f} },	// 7   :
	{ {SCRPLT1_X+432,SCRPLT1_Y-350}, {SCRPLT1_X+432,(((SCRPLT1_Y-350)*2.0f)/3.0f)-1.0f} },	// 8   seconds
	{ {SCRPLT1_X+436,SCRPLT1_Y-350}, {SCRPLT1_X+436,(((SCRPLT1_Y-350)*2.0f)/3.0f)-1.0f} },	// 9 seconds
	{ {SCRPLT1_X+443,SCRPLT1_Y-346}, {SCRPLT1_X+443,(((SCRPLT1_Y-350)*2.0f)/3.0f)-1.0f} },	//10   .
	{ {SCRPLT1_X+444,SCRPLT1_Y-350}, {SCRPLT1_X+444,(((SCRPLT1_Y-350)*2.0f)/3.0f)-1.0f} }	//11   tenths
};

// For EOQ score plate
#define SCRPLT2_X	18
#define SCRPLT2_Y	384

// For EOG score plate
#define SCRPLT3_X	0
#define SCRPLT3_Y	384

static int fid_scrplt2[][2] = {											// [plt#][res]
	{ FONTID_NBA20, FONTID_NBA14 },
	{ FONTID_NBA25, FONTID_NBA20 }
};
static float scrplt2_xys[][8][2][2] = {													// [plt#][obj#][res][x,y]
  { { {SCRPLT2_X,    SCRPLT2_Y    }, {SCRPLT2_X,    SCRPLT2_Y    } },					// 0 plate overlay
	{ {SCRPLT2_X+90, SCRPLT2_Y    }, {SCRPLT2_X+90, SCRPLT2_Y    } },					// 1 1st, 2nd, 3rd, 4th or OT (image)
	{ {SCRPLT2_X,    SCRPLT2_Y    }, {SCRPLT2_X,    SCRPLT2_Y    } },					// 2 team 1 city name
	{ {SCRPLT2_X+398,SCRPLT2_Y-299}, {SCRPLT2_X+398,(((SCRPLT2_Y-299)*2)/3.0f)-0.0f} },	// 3 team 1 score
	{ {SCRPLT2_X,    SCRPLT2_Y-28 }, {SCRPLT2_X,    SCRPLT2_Y-28 } },					// 4 team 2 city name
	{ {SCRPLT2_X+398,SCRPLT2_Y-327}, {SCRPLT2_X+398,(((SCRPLT2_Y-327)*2)/3.0f)-0.0f} },	// 5 team 2 score
  },
  { { {SCRPLT3_X,    SCRPLT3_Y    }, {SCRPLT3_X,    SCRPLT3_Y    } },					// 0 plate overlay
	{ {SCRPLT3_X,    SCRPLT3_Y    }, {SCRPLT3_X,    SCRPLT3_Y    } },					// 1 1st, 2nd, 3rd, 4th or OT (image)
	{ {SCRPLT3_X,    SCRPLT3_Y    }, {SCRPLT3_X,    SCRPLT3_Y    } },					// 2 team 1 city name
	{ {SCRPLT3_X+420,SCRPLT3_Y-274}, {SCRPLT3_X+420,(((SCRPLT3_Y-274)*2)/3.0f)-2.0f} },	// 3 team 1 score
	{ {SCRPLT3_X,    SCRPLT3_Y-56 }, {SCRPLT3_X,    SCRPLT3_Y-56 } },					// 4 team 2 city name
	{ {SCRPLT3_X+420,SCRPLT3_Y-330}, {SCRPLT3_X+420,(((SCRPLT3_Y-330)*2)/3.0f)-2.0f} },	// 5 team 2 score
	{ {SCRPLT3_X+128,SCRPLT3_Y-259}, {SCRPLT3_X+128,SCRPLT3_Y-259} },					// 6 team 1 logo
	{ {SCRPLT3_X+128,SCRPLT3_Y-315}, {SCRPLT3_X+128,SCRPLT3_Y-315} },					// 7 team 2 logo
  },
};

// Team names & logo imgs
static image_info_t * scrplt2_team_imgs[][3] = {						// [img#][team#]
	{ &atl20, &atlhawk, &atllog2 },										// atl
	{ &bos20, &boscelt, &boslog2 },										// bos
	{ &cha20, &chahorn, &chalog2 },										// cha
	{ &chi20, &chibull, &chilog2 },										// chi
	{ &cle20, &clecava, &clelog2 },										// cle
	{ &dal20, &dalmave, &dallog2 },										// dal
	{ &den20, &dennugg, &denlog2 },										// den
	{ &det20, &detpist, &detlog2 },										// det
	{ &gol20, &golwarr, &gollog2 },										// gol
	{ &hou20, &hourock, &houlog2 },										// hou
	{ &ind20, &indpace, &indlog2 },										// ind
	{ &lac20, &laclipp, &laclog2 },										// lac
	{ &lal20, &lalaker, &lallog2 },										// lak
	{ &mia20, &miaheat, &mialog2 },										// mia
	{ &mil20, &milbuck, &millog2 },										// mil
	{ &min20, &mintimb, &minlog2 },										// min
	{ &nwj20, &nwjnets, &nwjlog2 },										// nwj
	{ &nwy20, &nwyknic, &nwylog2 },										// nwy
	{ &orl20, &orlmagi, &orllog2 },										// orl
	{ &phi20, &phi76er, &philog2 },										// phi
	{ &pho20, &phosuns, &pholog2 },										// pho
	{ &por20, &portrai, &porlog2 },										// por
	{ &sac20, &sacking, &saclog2 },										// sac
	{ &san20, &sanspur, &sanlog2 },										// san
	{ &sea20, &seasupe, &sealog2 },										// sea
	{ &tor20, &torrapt, &torlog2 },										// tor
	{ &uta20, &utajazz, &utalog2 },										// uta
	{ &van20, &vangriz, &vanlog2 },										// van
	{ &was20, &waswiza, &waslog2 },										// was
	{ &als20, &midalst, &alslog2 }										// midway all-stars
};

static image_info_t * sparkle_imgs[] =
{
	&flarefl01,&flarefl02,&flarefl03,&flarefl04,&flarefl05,
	&flarefl06,&flarefl07,&flarefl08,&flarefl09,&flarefl10,
	&flarefl11,&flarefl12,&flarefl13,&flarefl14,&flarefl15
};

// Idiot box stuff
static image_info_t *idiot_boxes[2][MAX_PLYRS] = {	// plyr idiot box imgs [res][plyr#]
	{&boxblue, &boxgreen, &boxred, &boxyellow},
	{&boxblue, &boxgreen, &boxred, &boxyellow}};

static image_info_t *idiot_nums[2][MAX_PLYRS] = {	// plyr idiot box imgs [res][plyr#]
	{&playr1wrd, &playr2wrd, &playr3wrd, &playr4wrd},
	{&playr1wrd, &playr2wrd, &playr3wrd, &playr4wrd}};

static image_info_t *idiot_colr[2][MAX_PLYRS] = {	// plyr idiot box imgs [res][plyr#]
	{&wordblu, &wordgre, &wordred, &wordyel},
	{&wordblu, &wordgre, &wordred, &wordyel}};

//----------

static image_info_t * free_throw_letter_imgs[] =
{
	&fff,&otr,&gte,&gte,
	&scvt,&scvh,&otr,&oto,&ffw,&scvxc
};
static float free_throw_letter_xys[][2] = {	// [obj #][x,y]
	{128.0f+70,270},						// f
	{169.0f+70,270},						// r
	{216.0f+70,270},						// e
	{256.0f+70,270},						// e
	{088.0f+60,199},						// t
	{134.0f+60,199},						// h
	{182.0f+60,199},						// r
	{233.0f+60,199},						// o
	{294.0f+60,199},						// w
	{335.0f+60,199},						// !
	{337.0f+60,199},						// !
};
static float ft_letter_xvel[] =
{
	 -0.30f,		// f
	 -0.10f,		// r
	  0.10f,		// e
	  0.30f,		// e
	 -0.50f,		// t
	 -0.30f,		// h
	 -0.10f,		// r
	  0.10f,		// o
	  0.30f,		// w
	  0.50f,		// !
	  0.50f,		// !
};


static image_info_t * shot_clock_letter_imgs[] =
{
	&scvs,&scvh,&scvo,&scvt,&scvc,&scvl,&scvo,&scvc,&scvk,
	&scvv,&scvi,&scvo,&scvl,&scva,&scvt,&scvi,&scvo,&scvn,&scvxc
};
static float shot_clock_letter_xys[][2] = {	// [obj #][x,y]
	{028.0f+20,270},						// s
	{077.0f+20,270},						// h
	{127.0f+20,270},						// o
	{178.0f+20,270},						// t
	{247.0f+20,270},						// c
	{290.0f+20,270},						// l
	{336.0f+20,270},						// o
	{386.0f+20,270},						// c
	{437.0f+20,270},						// k
	{055.0f+20,199},						// v
	{088.0f+20,199},						// i
	{125.0f+20,199},						// o
	{168.0f+20,199},						// l
	{213.0f+20,199},						// a
	{262.0f+20,199},						// t
	{295.0f+20,199},						// i
	{333.0f+20,199},						// o
	{383.0f+20,199},						// n
	{427.0f+20,199},						// !
};
static float sc_letter_xvel[] =
{
	 -1.00f,		// s
	 -0.70f,		// h
	 -0.50f,		// o
	 -0.30f,		// t
	  0.00f,		// c
	  0.30f,		// l
	  0.50f,		// o
	  0.70f,		// c
	  1.00f,		// k
	 -1.00f,		// v
	 -0.70f,		// i
	 -0.50f,		// o
	 -0.30f,		// l
	 -0.10f,		// a
	  0.10f,		// t
	  0.30f,		// i
	  0.50f,		// o
	  0.70f,		// n
	  1.00f,		// !
};


static image_info_t *goaltending_letter_imgs[] =
{
	&gtg,&gto,&gta,&gtl,&gtt,&gte,&gtn,&gtd,
	&gti,&gtn,&gtg,&gtxc
};
static image_info_t *two_pts_letter_imgs[] =
{
	&gt2,&gtp,&gto2,&gti2,&gtn3,&gtt2,&gts
};
static image_info_t *three_pts_letter_imgs[] =
{
	&gt3,&gtp,&gto2,&gti2,&gtn3,&gtt2,&gts
};
static float goaltending_letter_xys[][2] = {// [obj #][x,y]
	{028.0f+10,270},						// g
	{073.0f+10,270},						// o
	{114.0f+10,270},						// a
	{156.0f+10,270},						// l
	{191.0f+10,270},						// t
	{231.0f+10,270},						// e
	{273.0f+10,270},						// n
	{319.0f+10,270},						// d
	{353.0f+10,270},						// i
	{388.0f+10,270},						// n
	{433.0f+10,270},						// g
	{468.0f+10,270},						// !
};
static float two_pts_letter_xys[][2] = {	// [obj #][x,y]
	{028.0f+ 98,210},						// 2
	{087.0f+103,210},						// p
	{129.0f+103,210},						// o
	{161.0f+103,210},						// i
	{192.0f+103,210},						// n
	{236.0f+103,210},						// t
	{272.0f+103,210},						// s
};
static float gt_letter_xvel[] =
{
	 -1.00f,		// g
	 -0.70f,		// o
	 -0.50f,		// a
	 -0.30f,		// l
	 -0.10f,		// t
	 -0.05f,		// e
	  0.05f,		// n
	  0.10f,		// d
	  0.30f,		// i
	  0.50f,		// n
	  0.70f,		// g
	  1.00f,		// !
	 -0.70f,		// 2/3
	 -0.50f,		// p
	 -0.30f,		// o
	  0.00f,		// i
	  0.30f,		// n
	  0.50f,		// t
	  0.70f,		// s
};

//----------

enum {
	JMETER_FRAME,
	JMETER_METER,
	JMETER_TEXT,
	JMETER_CNT
};

static float jmeter_xpos[][2][JMETER_CNT] = {	// [hi,lo][tm#][obj#]
	{{  0.0f,   0.0f,   0.0f}, {512.0f, 512.0f, 512.0f}},
	{{  0.0f,   0.0f,   0.0f}, {512.0f, 512.0f, 512.0f}}
};


//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
void flash_basketball_proc(int *args)
{
	int pnum = args[0];

	// Flash if:
	//	 not in attract AND
	//		anyone in a 4 player game OR
	//		a human in a 2 player game
	if ((game_info.game_state != GS_ATTRACT_MODE) &&
			(four_plr_ver || game_info.plyr_control[args[0]] >= 0)
		)
	{
		pdata_t * ppb = &pdata_blocks[pnum];

		ball_flash_timer = 1;

		unhide_sprite(ppb->si_frameglw);
		flash_obj_white(ppb->si_frameglw, 6, 4);	//3, 6
		hide_sprite(ppb->si_frameglw);
		
		ball_flash_timer = 0;
#ifdef DEBUG
		printf("Flashing ball behind guy who scored!\n");
#endif

	}
}


//---------------------------------------------------------------------------------------------------------------------------
//					This process prints the opening text messages
//---------------------------------------------------------------------------------------------------------------------------

//extern unsigned char rgold[];
//extern unsigned char rgb[];
//extern unsigned char bpr[];
//extern unsigned char decay[];
//extern unsigned char laser[];

void opening_msg_proc(int * args)
{
	struct tagXY {
		float	x;
		float	y;
	};
	struct tagXYF {
		float	x;
		float	y;
		int		f;
	};
	static struct tagXY obj_xys[][2] =		//[obj#][hi,lo][x,y]
	{
		{{ 262,  48 },{ 262,  48 }},		//0  tmode box
		{{ 262, 201 },{ 262, 201 }},		//1  tpage box
	};
	static struct tagXYF str_xyfs[][2] =								//[str#][hi,lo][x,y,f]
	{
		{{ 256,  72, FONTID_NBA20    },{ 256,  50, FONTID_NBA14    }},	//0  tmode hdr
		{{ 256,  39, FONTID_NBA10    },{ 256,  26, FONTID_BAST8TA  }},	//1  tmode text

		{{ 256, 315, FONTID_NBA20    },{ 256, 210, FONTID_NBA14    }},	//2  tpage1 hdr
		{{ 256, 282, FONTID_NBA12    },{ 256, 188, FONTID_NBA10    }},	//3  tpage1 text
		{{  97, 261, FONTID_NBA12    },{  97, 174, FONTID_NBA10    }},	//4  tpage1 text p1
		{{ 203, 261, FONTID_NBA12    },{ 203, 174, FONTID_NBA10    }},	//5  tpage1 text p2
		{{ 309, 261, FONTID_NBA12    },{ 309, 174, FONTID_NBA10    }},	//6  tpage1 text p3
		{{ 415, 261, FONTID_NBA12    },{ 415, 174, FONTID_NBA10    }},	//7  tpage1 text p4
		{{ 256, 213, FONTID_NBA12    },{ 256, 142, FONTID_NBA10    }},	//8  tpage1 text

		{{ 256, 315, FONTID_NBA20    },{ 256, 210, FONTID_NBA14    }},	//9  tpage2 hdr
		{{ 256, 282, FONTID_NBA12    },{ 256, 188, FONTID_NBA10    }},	//10 tpage2 text
		{{ 256, 246, FONTID_NBA12    },{ 256, 164, FONTID_NBA10    }},	//11 tpage2 text
		{{ 256, 195, FONTID_NBA12    },{ 256, 130, FONTID_NBA10    }},	//12 tpage2 text
		{{ 256, 159, FONTID_NBA12    },{ 256, 106, FONTID_NBA10    }},	//13 tpage2 text
	};
	static char plyrsz[] = "PLAYER 0";
	sprite_info_t * omp1_obj;
	sprite_info_t * omp2_obj;
	process_node_t * pflsh;
	float fx, fy, fdy;
	float obj_yo = 0;
	float str_yo = 0;
	int i, font, res = is_low_res;
	int was_hide_peacock = hide_peacock;

	hide_peacock = 1;
	if (!pup_tourney)
	{
		obj_yo = -21;
		str_yo = !res ? -21 : -14;
	}

	// Turn off start buttons to avoid any restart-suspend confusion
	set_dcsw_handler(P1_START_SWID, no_start);
	set_dcsw_handler(P2_START_SWID, no_start);
	if (four_plr_ver) {
		/* 2/4 plyr kit logic */
		set_dcsw_handler(P3_START_SWID, no_start);
		set_dcsw_handler(P4_START_SWID, no_start);
	}

	// Show text msg page 1
	omp1_obj = beginobj(&playresult, obj_xys[1][res].x, obj_xys[1][res].y+obj_yo, 25, SCR_PLATE_TID);
	omp1_obj->id = OID_MESSAGE;
	omp1_obj->w_scale = 2.10f;
	omp1_obj->h_scale = 3.00f;
	generate_sprite_verts(omp1_obj);

	// Put back when done!
	is_low_res = 0;
	set_text_z_inc(-0.01f);

	set_string_id(0xfeed00);
	set_text_justification_mode(HJUST_CENTER|VJUST_TOP);

	set_text_position(str_xyfs[2][res].x, str_xyfs[2][res].y+str_yo, 1.9f);
	set_text_font(str_xyfs[2][res].f);
	oprintf("%dcINSTRUCTIONS", LT_ORANGE);
	qcreate("OScycle", MESSAGE_PID, cycle_ostring_color_proc, 0xfeed00, (int)bpr, 0, 0);

	set_string_id(0xfeed10);

	set_text_position(str_xyfs[3][res].x, str_xyfs[3][res].y+str_yo, 1.9f);
	set_text_font(str_xyfs[3][res].f);
	oprintf("%dcYOUR STICK ALWAYS CONTROLS THE SAME PLAYER:", 0xffe0e0e0);

	for (i = 0; i < MAX_PLYRS; i++)
	{
		int j = i;
		int k = i;

		// Set j to reverse indexing of left team in 2 plyr mode
		if (!four_plr_ver)
		{
			if (i < NUM_PERTEAM)
				j = NUM_PERTEAM-1-i;
			else
				k--;
		}

		fx   = str_xyfs[4+j][res].x;
		fy   = str_xyfs[4+j][res].y;
		font = str_xyfs[4+j][res].f;

		set_text_position(fx, fy+str_yo, 1.9f);
		set_text_font(font);

		if (p_status & (1<<i))
		{
			plyrsz[sizeof(plyrsz)-2] = '1' + k;
			oprintf("%dc%s", plr_color[i], plyrsz);
		}
		else
		{
			oprintf("%dcCPU DRONE", 0xffe0e0e0);
		}

		fy -= get_font_height(font);
		set_text_position(fx, fy+str_yo, 1.9f);
		oprintf("%dc%s", LT_CYAN, player_blocks[i].p_spd->szlast);
	}

	fx   = str_xyfs[8][res].x;
	fy   = str_xyfs[8][res].y;
	font = str_xyfs[8][res].f;

	set_text_font(font);
	fdy = get_font_height(font);

	set_text_position(fx, fy+str_yo, 1.9f);
	oprintf("%dcYOUR PLAYER  # AND TURBO BAR COLOR MATCH YOUR", LT_GREEN);//, 0xffe0e0e0);
	fy -= fdy;
	set_text_position(fx, fy+str_yo, 1.9f);
	oprintf("%dcPLAYER CURSOR, ICON, AND OFF-SCREEN ARROW", LT_GREEN);//, 0xffe0e0e0);
//	fy -= fdy;
//	set_text_position(fx, fy+str_yo, 1.9f);
//	oprintf("%dcREMEMBER THE  # AND COLOR OF YOUR TEAMMATE!", 0xffe0e0e0);

	fy -= fdy;
	fy -= fdy;

	set_text_position(fx, fy+str_yo, 1.9f);
	oprintf("%dcYOUR BUTTONS CONTROL WHEN A DRONE TEAMMATE", 0xffe0e0e0);
	fy -= fdy;
	set_text_position(fx, fy+str_yo, 1.9f);
	oprintf("%dcWILL %dcSHOOT%dc OR %dcPASS", 0xffe0e0e0, LT_RED, 0xffe0e0e0, LT_BLUE);

	// Back it goes!
	is_low_res = res;
	set_text_z_inc( 0.00f);

	// Turn off plates
	if (plates_on == PLATES_ON)
		turn_transit_off(0);

	// Wait for plates to be gone
	while (plates_on == PLATES_COMING_OFF)
		sleep(1);

	// Pause...
	sleep(25);

	// Chk if tournament mode
	if (pup_tourney)
//	if (1)
	{
		// Show tournament mode msg
		omp2_obj = beginobj(&playresult, obj_xys[0][res].x, obj_xys[0][res].y, 24, SCR_PLATE_TID);
		omp2_obj->id = OID_MESSAGE;
		omp2_obj->w_scale = 2.10f;
		omp2_obj->h_scale = 0.90f;
		generate_sprite_verts(omp2_obj);

		qcreate("flash", MESSAGE_PID, flash_obj_proc, (int)omp2_obj, 5, 3, 0);

		snd_scall_bank(plyr_bnk_str, 57, VOLUME7, 127,PRIORITY_LVL5);

		// Put back when done!
		is_low_res = 0;
		set_text_z_inc(-0.01f);

		set_string_id(0xfeed01);
		set_text_justification_mode(HJUST_CENTER|VJUST_TOP);

		set_text_position(str_xyfs[0][res].x, str_xyfs[0][res].y, 1.9f);
		set_text_font(str_xyfs[0][res].f);
		oprintf("%dcTOURNAMENT MODE", LT_RED);
		qcreate("OScycle", MESSAGE_PID, cycle_ostring_color_proc, 0xfeed01, (int)bpr, 0, 0);

		set_string_id(0xfeed02);

		set_text_position(str_xyfs[1][res].x, str_xyfs[1][res].y, 1.9f);
		set_text_font(str_xyfs[1][res].f);
		oprintf("%dcNO CPU ASSISTANCE OR UNFAIR POWERUPS ALLOWED", WHITE);
//		qcreate("OScycle", MESSAGE_PID, cycle_ostring_color_proc, 0xfeed02, (int)laser, 0, 0);

		// Back it goes!
		is_low_res = res;
		set_text_z_inc( 0.00f);

//		hide_peacock = 1;
	}

	wait_for_all_buttons(15, 30 * tsec);

	// Delete page 1
	delete_multiple_strings(0xfeed10, -16);

	// Show text msg page 2

	// Put back when done!
	is_low_res = 0;
	set_text_z_inc(-0.01f);

//	set_string_id(0xfeed03);
//	set_text_justification_mode(HJUST_CENTER|VJUST_TOP);
//
//	set_text_position(str_xyfs[9][res].x, str_xyfs[9][res].y+str_yo, 1.9f);
//	set_text_font(str_xyfs[9][res].f);
//	oprintf("%dcINSTRUCTIONS", LT_GREEN);
//	qcreate("OScycle", MESSAGE_PID, cycle_ostring_color_proc, 0xfeed00, (int)rgold, 0, 0);

	set_string_id(0xfeed20);
	set_text_justification_mode(HJUST_CENTER|VJUST_TOP);

//	oprintf("%dcYOUR BUTTONS CONTROL A DRONE TEAMMATE. USE THEM", 0xffe0e0e0);

//	oprintf("%dcWHEN DRIBBLING, YOU CAN JUKE AN OPPONENT WITH A
//	oprintf("%dcYOU CAN JUKE AN OPPONENT WHILE DRIBBLING WITH A
//	oprintf("%dcQUICK DOUBLE-TAP OF TURBO BUTTON MAKES DRIBBLER JUKE", LT_YELLOW);
//	oprintf("%dcJUKE AN OPPONENT, BUT ONLY IF ENOUGH TURBO REMAINS", LT_YELLOW);
//	oprintf("%dcJUKE AN OPPONENT, YOU NEED ENOUGH TURBO FOR THIS!", LT_YELLOW);

	fx   = str_xyfs[10][res].x;
	fy   = str_xyfs[10][res].y;
	font = str_xyfs[10][res].f;
	set_text_font(font);
//	fdy = get_font_height(font);
	set_text_position(fx, fy+str_yo, 1.9f);
	oprintf("%dcHOLD DOWN YOUR TURBO BUTTON TO RUN FASTER", 0xffe0e0e0);//LT_YELLOW);
//	fy -= fdy;
//	set_text_position(fx, fy+str_yo, 1.9f);
//	oprintf("%dcYOU NEED TURBO FOR THIS!", 0xffe0e0e0);//LT_YELLOW);

	fx   = str_xyfs[11][res].x;
	fy   = str_xyfs[11][res].y;
	font = str_xyfs[11][res].f;
	set_text_font(font);
	fdy = get_font_height(font);
	set_text_position(fx, fy+str_yo, 1.9f);
	oprintf("%dcWHEN DRIBBLING, A DOUBLE-TAP OF YOUR TURBO BUTTON", LT_GREEN);//LT_YELLOW);
	fy -= fdy;
	set_text_position(fx, fy+str_yo, 1.9f);
	oprintf("%dcWILL DO A JUKE MOVE.  %dcYOU NEED TURBO FOR THIS!", LT_GREEN, LT_RED);//LT_YELLOW);

	fx   = str_xyfs[12][res].x;
	fy   = str_xyfs[12][res].y;
	font = str_xyfs[12][res].f;
	set_text_font(font);
//	fdy = get_font_height(font);
	set_text_position(fx, fy+str_yo, 1.9f);
	oprintf("%dcGET ON FIRE BY MAKING %dc3%dc CONSECUTIVE SHOTS", 0xffe0e0e0, LT_YELLOW, 0xffe0e0e0);
//	fy -= fdy;
//	set_text_position(fx, fy+str_yo, 1.9f);
//	oprintf("%dcNO OTHER PLAYER MAKES A SHOT.", 0xffe0e0e0);

	fx   = str_xyfs[13][res].x;
	fy   = str_xyfs[13][res].y;
	font = str_xyfs[13][res].f;
	set_text_font(font);
	fdy = get_font_height(font);
	set_text_position(fx, fy+str_yo, 1.9f);
	oprintf("%dcGET YOUR TEAM ON FIRE BY MAKING %dc3%dc ALLY-OOPS AND/OR", LT_GREEN, LT_YELLOW, LT_GREEN);//LT_YELLOW);//0xffe0e0e0);
	fy -= fdy;
	set_text_position(fx, fy+str_yo, 1.9f);
	oprintf("%dcDOUBLE-DUNKS WITH NO SHOTS MADE BY THE OTHER TEAM", LT_GREEN);//LT_YELLOW);//0xffe0e0e0);

	// Back it goes!
	is_low_res = res;
	set_text_z_inc( 0.00f);

	wait_for_all_buttons(15, 30 * tsec);

	// Delete page 2 text
	delete_multiple_strings(0xfeed20, -16);

	// Delete static text
	delete_multiple_strings(0xfeed00, -16);

	del1c(OID_MESSAGE, -1);
	killall(MESSAGE_PID, -1);

	hide_peacock = was_hide_peacock;

	// Turn on start buttons
	set_dcsw_handler(P1_START_SWID, plyr_strt_butn);
	set_dcsw_handler(P2_START_SWID, plyr_strt_butn);
	if (four_plr_ver) {
		/* 2/4 plyr kit logic */
		set_dcsw_handler(P3_START_SWID, plyr_strt_butn);
		set_dcsw_handler(P4_START_SWID, plyr_strt_butn);
	}
}


//-----------------------------------------------------------------------------------------------------------------------------
// freethrow_meter_exit() - 
//-----------------------------------------------------------------------------------------------------------------------------
static process_node_t * pflasher_ftm;

void freethrow_meter_exit(struct process_node * proc, int cause)
{
	del1c(OID_FTMTR, -1);
	delete_multiple_strings(SID_FTMTR, -1);

	if (pflasher_ftm)
		kill(pflasher_ftm, 0);

	showftmtr_flag = 0;
}

//-----------------------------------------------------------------------------------------------------------------------------
// kick_meter_proc() - 
//-----------------------------------------------------------------------------------------------------------------------------
#define VELBASE	(7.5f)

void freethrow_meter_proc(int *args)
{
	static float att_vel[] =
		{ 1.70f *VELBASE, 1.68f *VELBASE, 1.66f *VELBASE, 1.64f *VELBASE, 1.62f *VELBASE, 1.60f *VELBASE };
	int pnum = game_info.freethrow_shooter;
	int	dist_from_cntr;
	int	drone_make_prob = 0;
	int	max_drone_butn_time = 0;

	if (pnum < 0)
		die(0);

	ftmtr_per = 50;
	
	if (randrng(100) < 15)
		qcreate("taunt_sp",0,freethrow_taunt_speech_proc,0,0,0,0);

	// Only do meter if FG-type play, human, and not in attract mode
	if (game_info.game_state != GS_ATTRACT_MODE)
//	if ((game_info.game_state != GS_ATTRACT_MODE) &&
//		((!four_plr_ver && (p_status & (1 << (game_info.off_side)))) ||
//		(four_plr_ver && (p_status & (3 << (game_info.off_side*2))))))
	{
		ostring_t * pmsg;
		sprite_info_t * bx_obj;
		unsigned char old_color_combiner_function;
		int pos,vel,wo2;
		//register int x = (game_info.active_goal) ? BARX2 - 20 : BARX1 + 20;
		int x = (player_blocks[pnum].team ? 384.0f : 128.0f);
		sprite_info_t * ftbobj = beginobj(&ft_meter, x, 50.0f , 2.0f, SCR_PLATE_TID);
		sprite_info_t * ftmobj = beginobj(&ft_ball,  x, 50.0f , 1.0f, SCR_PLATE_TID);

		ftbobj->id = OID_FTMTR;
		ftmobj->id = OID_FTMTR;

		old_color_combiner_function = ftmobj->state.color_combiner_function;

		showftmtr_flag = 1;

		((struct process_node *)cur_proc)->process_exit = freethrow_meter_exit;
		pflasher_ftm = NULL;

		vel = (int)(att_vel[randrng(4)] * 10000.0f);
		wo2 = (int)(ftbobj->ii->w * (0.40f * 10000.0f));
		pos = 0;	// center of bar
		pos += (vel*randrng(4)) * 2;

		max_drone_butn_time = game_info.auto_throw_cntr;
		if (ISDRONETEAM(&player_blocks[pnum]) == YES)
		{
			max_drone_butn_time /= 2;
			max_drone_butn_time += randrng((game_info.auto_throw_cntr / 3));
			if (compute_score(pnum) <  compute_score(!player_blocks[pnum].team))
				drone_make_prob = 10;				// drone team losing... this prob is greater cause its check more than once
			else
				drone_make_prob = 5;				// drone team winning...
		}

		while ((--game_info.auto_throw_cntr > 0) && (--max_drone_butn_time > 0))
		{
			if (!drone_make_prob)
			{	// not a drone team! read buttons
				if (((ISDRONE(&player_blocks[pnum])) && (get_but_val_down(pnum^1))) ||
					((p_status & (1<<pnum)) && (get_but_val_down(pnum))))
					break;
			}
#if DEBUG
			if (!halt || (halt && get_player_sw_close() & P3_DOWN))
#endif
			pos += vel;

			if (pos >  wo2 || pos < -wo2)
			{
				vel = -vel;
				snd_scall_bank(cursor_bnk_str,4,VOLUME5,127,PRIORITY_LVL2);
			}

			// in middle ?
			if ((vel > 0 && pos <= vel && pos >= -vel) || (vel < 0 && pos <= -vel && pos >= vel))
			{
				if ((drone_make_prob) && (randrng(100) < drone_make_prob))
					break;
				
//				ftmobj->state.color_combiner_function = GR_COLORCOMBINE_DECAL_TEXTURE_ADD_CCALPHA;
//				ftmobj->state.constant_color = 0xc0000000;
			}
//			else
//			{
//				ftmobj->state.color_combiner_function = old_color_combiner_function;
//				ftmobj->state.constant_color = 0xff000000;
//			}

			ftmobj->x = x + pos / 10000;
			//printf("X = %2.2f\n",ftmobj->x);
			generate_sprite_verts(ftmobj);

			sleep(1);
		}

		// Do accuracy % and leave meter up for a bit
		if (pos < -wo2) pos = -wo2;
		if (pos >  wo2) pos =  wo2;
		if (pos < 0)
			pos += wo2;
		else
			pos -= wo2;

		// compute percent
		dist_from_cntr = abs(x - ftmobj->x);
		if (dist_from_cntr <= 10)
			ftmtr_per = 95 + randrng(5);
		else if (dist_from_cntr <= 20)
			ftmtr_per = 75 + randrng(5);
		else if (dist_from_cntr <= 45)
			ftmtr_per = 20 + randrng(5);
		else
			ftmtr_per = 2 + randrng(5);

		// but if on fire... 98%
		if ((game_info.fire_player == pnum  &&  game_info.fire_cnt >= FIRECNT) ||
			(game_info.team_fire == player_blocks[pnum].team))
		{
			ftmtr_per = 98;
		}

//		ftmtr_per = x - (100 * pos) / wo2;
//		ftmtr_per = (100 * pos) / wo2;
//		pos = abs(pos);
//		if (pos <= (wo2/8))
//			ftmtr_per = randrng(30) + 30;		// suck ass shot!
//		else
//			ftmtr_per = randrng(9) + 90;		// good shot!

//#if DEBUG
//		fprintf(stderr,"~~~~ pos = %d and wo2 = %d\n",pos,wo2);
//		fprintf(stderr,"~~~~~~~~~~~ dist from center = %d  and FREETHROW meter percent = %d\r\n",dist_from_cntr,ftmtr_per);
//#endif
		sleep(15);
	}
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void make_jump_meter_proc(int *args)
{
	int tm = args[0];
	int id = OID_JUMP_METER + tm;
	int cnt = args[1];
	struct tagJMETER *  pjm = &jmeter[tm];
	struct image_info * pii = &jmeter_ii[tm];
	float jmeter_ypos = is_low_res ? 300.0f : 300.0f;

	jump_meters_on = 0;
	hide_peacock = 0;

	pjm->frame_obj = beginobj(
			&jmpbar,
			jmeter_xpos[is_low_res][tm][JMETER_FRAME],
			jmeter_ypos,
			10.0f,
			SCR_PLATE_TID);
	if (tm) {
		pjm->frame_obj->x -= pjm->frame_obj->ii->w - 2 * pjm->frame_obj->ii->ax;
		pjm->frame_obj->mode = FLIP_TEX_H;
		generate_sprite_verts(pjm->frame_obj);
	}
	pjm->frame_obj->id = id;
	hide_sprite(pjm->frame_obj);

	pjm->meter_obj = beginobj(
			&jmpbarmtr,
			jmeter_xpos[is_low_res][tm][JMETER_METER],
			jmeter_ypos,
			5.0f,
			SCR_PLATE_TID);
	// Make a local copy of image info & tell sprite where it is
	*pii = *pjm->meter_obj->ii;
	pjm->meter_obj->ii = pii;
	// Get T difference & meter height
	pjm->t_dif = pii->t_end - pii->t_start;
	pjm->h_max = pii->h;
	// Set initial meter height & T start
	pii->h = 0;
	pii->t_start = pii->t_end - (pii->h / pjm->h_max) * pjm->t_dif;
	if (tm) {
		pjm->meter_obj->x -= pjm->meter_obj->ii->w - 2 * pjm->meter_obj->ii->ax;
		pjm->meter_obj->mode = FLIP_TEX_H;
	}
	generate_sprite_verts(pjm->meter_obj);
	pjm->meter_obj->id = id;
	hide_sprite(pjm->meter_obj);

	pjm->text_obj = beginobj(
			&jmpwrdl,
			jmeter_xpos[is_low_res][tm][JMETER_TEXT] - (tm + tm - 1) * pjm->frame_obj->ii->w,
			jmeter_ypos,
			4.5f,
			SCR_PLATE_TID);
	if (tm)
		pjm->text_obj->x -= pjm->text_obj->ii->w;
	pjm->text_obj->x += pjm->text_obj->ii->ax;
	generate_sprite_verts(pjm->text_obj);
	pjm->text_obj->id = id + 2;
	hide_sprite(pjm->text_obj);

	// wait for any GM_PRE_TIPOFF or spec'd ticks before starting
	while (game_info.game_mode == GM_PRE_TIPOFF || cnt)
	{
		// Any GM_PRE_TIPOFF will start the tip-off as soon as
		// it changes to GM_TIPOFF, thus supporting button-out
		if (game_info.game_mode == GM_PRE_TIPOFF)
			cnt = pre_tipoff_movie_delay;

		if (cnt) cnt--;

		sleep(1);
	}

	unhide_sprite(pjm->frame_obj);
	unhide_sprite(pjm->meter_obj);
	unhide_sprite(pjm->text_obj);

	hide_peacock = 1;

	sleep(3);
	qcreate("flash", JUMP_METER_PID, flash_obj_proc, (int)pjm->frame_obj, 4, 3, 0);
	qcreate("flash", JUMP_METER_PID, flash_obj_proc, (int)pjm->meter_obj, 4, 3, 0);
	qcreate("flash", JUMP_METER_PID, flash_obj_proc, (int)pjm->text_obj,  4, 3, 0);

	sleep(4*3 + 40);
	del1c(id + 2, -1);
}

void do_jump_meters_proc(int *args)
{
	float h = (int)(jmeter[0].h_max * 0.30f);	//### start
	float dh;
	int msk[2] = {0, 0};
	int i, sw;

	// Any GM_PRE_TIPOFF will start the tip-off as soon as
	// it changes to GM_TIPOFF, thus supporting button-out
	while (game_info.game_mode == GM_PRE_TIPOFF)
		sleep(1);

	if (!four_plr_ver)
	{
		if (p_status & (1<<0)) msk[0] = (P1_A|P1_B|P1_C);

		if (p_status & (1<<2)) msk[1] = (P2_A|P2_B|P2_C);
	}
	else
	{
		if (p_status & (1<<1)) msk[0] = (P2_A|P2_B|P2_C);
		else
		if (p_status & (1<<0)) msk[0] = (P1_A|P1_B|P1_C);

		if (p_status & (1<<2)) msk[1] = (P3_A|P3_B|P3_C);
		else
		if (p_status & (1<<3)) msk[1] = (P4_A|P4_B|P4_C);
	}

	do {
		struct tagJMETER *  pjm = &jmeter[0];
		struct image_info * pii = &jmeter_ii[0];

		// Shouldn't be needed now that game_proc() kills everything, but...
		if (DBG_KO_BOOL) goto abort_do_jump_meters_proc;

		if (jump_meters_on)
		{
			i = 0;
			do
			{
				dh = 0.0f;
				if (h)
					pii->h = h;
				else
#ifdef DEBUG
				if (!halt)
#endif
				{
					if (msk[i])
						// Player
						sw = get_player_sw_close() & msk[i];
					else
						//FIX!!! wants to get quicker as player improves
						// Drone
						sw = (randrng(100) < 22);

					dh = sw ? 2.80f : (float)(randrng(100) - 65) * 0.02f;
				}

				// Do nothing if already maxed
				if (pii->h != pjm->h_max)
				{
					pii->h += dh;

					// Validate height
					if (pii->h > pjm->h_max)
					{
						pii->h = pjm->h_max;
						if (jmeter_ii[!i].h == jmeter[!i].h_max)
							pii->h -= 0.01f;
					}
					if (pii->h < 0.0f)
						pii->h = 0.0f;
				}

				// Convert meter height to T value
				pii->t_start = pii->t_end - (pii->h / pjm->h_max) * pjm->t_dif;
				generate_sprite_verts(pjm->meter_obj);
			} while (++pii, ++pjm, ++i <= 1);
			h = 0.0f;
		}
		sleep(1);

	} while (game_info.ball_mode == BM_TIPOFF);
	
	// show meter for a bit...
	sleep(35);
		
abort_do_jump_meters_proc:
	// delete jump ball meter images...
	killall(JUMP_METER_PID, -1);
	del1c(OID_JUMP_METER, -4);
	hide_peacock = 0;
}

void do_jump_meters_flag(obj_3d * pobj, int flag)
{
	jump_meters_on = flag;
}

//-----------------------------------------------------------------------------------------------------------------------------
//
//	Display shot clock in lower left hand corner.  It stays alive the whole game
//	handling itself based on game_info.shotclock
//
//-----------------------------------------------------------------------------------------------------------------------------
static process_node_t	*pflasher = NULL;
static process_node_t	*pflasher2 = NULL;
static ostring_t		*pmsg[2];

void shot_clock_box_proc(int *args)
{
	sprite_info_t * shotclock_box_obj;
	process_node_t * pflasher;
	ostring_t * pmsg;
	int cleared = 0, temp_clock = 0;
	int	was_low_res = is_low_res;

	while (1)
	{
#ifndef DEBUG_SHOT_CLOCK_BOX
		// ONLY display when shot <= 10 seconds...
		while (((game_info.shotclock & 0x0000FF00)>>8) > 9 || show_score_plate > 0)
		{
			// Auto clear up shot clock any time shot clock is reset (Rim hits, scores, etc.), if needed
			// Some other code resets game_info.shotclock
			if(cleared != 0)
			{
				cleared = 0;
	
				// Clear up existing shot clock img, digits, string digit flasher proc
				delete_multiple_strings(SHOTCLOCK_TEXT_ID, 0xFFFFFFFF);
				delobj(shotclock_box_obj);
				kill(pflasher, 0);
			}
			sleep(1);
		}
#endif
		// Shot clock is low enough to be displayed
		if(cleared == 0)
		{
#ifndef DEBUG_SHOT_CLOCK_BOX
			// If game clock is below 10 secs, ignore displaying shotclock &
			//  reset the shotclock, giving away a free second
			if ((game_info.game_time & 0x00FFFF00) <= 0x00000a00)
				reset_shotclock = 1;
			else
#endif
			{				
				cleared = 1;
		
				// Turn on shot clock box
				shotclock_box_obj = beginobj(&shotclk1,
					shotclock_box_xys[0][0][X_VAL],
					shotclock_box_xys[0][0][Y_VAL],
					20.1f,
					SCR_PLATE_TID);
				shotclock_box_obj->id = OID_INFOBOX;

				set_text_font(was_low_res ? FONTID_NBA14 : FONTID_NBA20);
				set_string_id(SHOTCLOCK_TEXT_ID);
				set_text_justification_mode(HJUST_CENTER|VJUST_BOTTOM);
				set_text_color(LT_RED);
				is_low_res = 0;

				// print shot clock time
				set_text_position(
					shotclock_box_xys[1][was_low_res][X_VAL],
					shotclock_box_xys[1][was_low_res][Y_VAL],
					20.0f);

				pmsg = oprintf( "%0" TO_STRING(SHOT_CLOCK_DIGCNT) "d", game_info.shotclock >> 8 );

				is_low_res = was_low_res;

				pflasher = qcreate("pflash", MESSAGE_PID, flash_alpha_text_proc, (int)pmsg, 0, 0, 0);
				temp_clock = game_info.shotclock >> 8;
			}
		}
		else
		{
			// Update score values & colors
			if (temp_clock != (game_info.shotclock >> 8))
			{
				update_ostring_value(pmsg, (temp_clock = (game_info.shotclock >> 8)), 1);
				snd_scall_bank(gameplay_bnk_str, 117 , VOLUME3, 127, PRIORITY_LVL1);
			}
		}

		sleep(1);
	}
}

/*****************************************************************************/
// change the constant_color field in the status of all alpha sprites
// matching a given parent.
void change_alpha_cc(void * parent, int cc)
{
	register sprite_node_t * snode = alpha_sprite_node_list;

	// Walk a sprite list
	while(snode)
	{
		// Node we are looking for ?
		if(snode->si->parent == parent)
		{
			snode->si->state.constant_color = cc;
		}

		// Save the next node pointer
		snode = snode->next;
	}
}

/*****************************************************************************/
// if <value> is -1, string will be changed to a single '.'
// if <value> is -2, string will be changed to a single ':'
//
void change_ostring_value(sprite_node_t * psn, ostring_t * string, int value, int digmin, float xpos, int justify)
{
	register font_info_t * pfi = string->state.font_node->font_info;
	register image_info_t ** ppii = pfi->characters;
	register int wch = 0;
	register int wst = 0;
	register int val = value;
	register int bas = !!val;
	register int cnt = bas;
	sprite_node_t * psn_o = psn;
	int	was_low_res = is_low_res;

	// Ensure parms are useable
	if (digmin < 0)  digmin = 0;
	if (digmin > 10) digmin = 10;
	justify &= HJUST_MODE_MASK;

	if (val >= 0)
	{
		while (val /= 10)
		{
			cnt++;
			bas *= 10;
		}
	}
	else
		digmin = 1;

	// Only go into loop if we know something has to be displayed
	while (cnt || digmin)
	{
		if (val >= 0)
		{
			if (justify == HJUST_RIGHT)
			{
				if (!cnt && digmin)
				{
					val = 0;
					cnt++;
				}
				else
				{
					val = value % 10;
					value /= 10;
					bas /= 10;
				}
			}
			else
			{
				if (cnt < digmin)
				{
					val = 0;
					cnt++;
				}
				else
				{
					val = value / bas;
					value -= val * bas;
					bas /= 10;
				}
			}
		}

		// Walk sprite list to find next character
		while (psn)
		{
			register sprite_info_t * psi = psn->si;

			// Get the next node pointer
			psn = psn->next;

			// Node we are looking for?
			if (psi->parent == string)
			{
				// Yes; setup digit for display & break the find
				if (val >= 0)
					psi->ii = ppii[val + '0' - pfi->start_character];
				else
					psi->ii = ppii[(val == -1 ? '.':':') - pfi->start_character];
				if (justify == HJUST_RIGHT)
				{
					xpos -= psi->ii->w + (float)pfi->char_space;
					psi->x = xpos - psi->ii->ax;
				}
				else
				{
					psi->x = xpos - psi->ii->ax;
					wst  += wch = (int)psi->ii->w + pfi->char_space;
					xpos += wch;
				}
				unhide_sprite(psi);
				break;
			}
		}
		// Abort if none left
		if (!psn) break;

		// Digit was displayed; dec counters till zero
		if (digmin) digmin--;
		if (cnt)    cnt--;
	}

	// Walk sprite list & hide any remaining characters
	while (psn)
	{
		// Node we are looking for?
		if (psn->si->parent == string)
			hide_sprite(psn->si);

		// Get the next node pointer
		psn = psn->next;
	}

	// Do verts & chk if wants to be center-justified
	psn = psn_o;
	wch = wst >> 1;

	// Walk sprite list to find next character
	is_low_res = 0;
	while (psn)
	{
		// Node we are looking for?
		if (psn->si->parent == string &&
			!(psn->mode & HIDE_SPRITE))
		{
			if (justify == HJUST_CENTER)
				psn->si->x -= wch;

			generate_sprite_verts(psn->si);
		}
		// Get the next node pointer
		psn = psn->next;
	}
	is_low_res = was_low_res;
}

/*****************************************************************************/
// if <value> is -1, string will be changed to a single '.'
// if <value> is -2, string will be changed to a single ':'
//
void update_ostring_value(ostring_t * string, int value, int digmin)
{
	change_ostring_value(alpha_sprite_node_list, string, value, digmin, string->state.x, string->state.justification_mode);
}

/*****************************************************************************/
// if <value> is -1, string will be changed to a single '.'
// if <value> is -2, string will be changed to a single ':'
//
void update_ostring_value_nonalpha(ostring_t * string, int value, int digmin)
{
	change_ostring_value(sprite_node_list, string, value, digmin, string->state.x, string->state.justification_mode);
}

/*****************************************************************************/
void score_plate_proc(int *args)
{
	sprite_info_t * scrplt_obj;
	sprite_info_t * scrplt1_obj;
	sprite_info_t * scrplt2_obj;
	sprite_info_t * scrplt3_obj;

	ostring_t * po_score0, * po_score1, * po_min, * po_sec, * po_col;
	int cur_tick_score, color_switch_score;
	int score0, score1, min, sec, ten;
	int score_plate_shown, small_plate_shown, was_low_res = is_low_res, i;
	int res = (!!was_low_res);

	sleep(5);

	po_score0 = po_score1 = po_min = po_sec = po_col = NULL;
	cur_tick_score = color_switch_score =0;
	score0 = score1 = min = sec = ten = 0;

//	was_low_res = is_low_res;

	show_score_plate  = -1;
	score_plate_shown = 1;
	small_plate_shown = 1;

	// Main plate
	scrplt_obj = beginobj(
		&scrplaq1,
		scrplt1_xys[0][res][X_VAL],
		scrplt1_xys[0][res][Y_VAL],
		2.4f,
		SCR_PLATE_TID);
	scrplt_obj->id = OID_SCRPLATE;

	scrplt1_obj = beginobj(
		&scrplaq1_c1,
		scrplt1_xys[0][res][X_VAL],
		scrplt1_xys[0][res][Y_VAL],
		2.4f,
		SCR_PLATE_TID);
	scrplt1_obj->id = OID_SCRPLATE;

	// Current quarter
	scrplt2_obj = beginobj(
		qtr_imgs[game_info.game_quarter],
		scrplt1_xys[1][res][X_VAL],
		scrplt1_xys[1][res][Y_VAL],
		2.3f,
		SCR_PLATE_TID);
	scrplt2_obj->id = OID_SCRPLATE;

	// Plate under clock - for less than 1 minute mode (On all the time - inefficient)
	scrplt3_obj = beginobj(
		&scrplaqt,
		scrplt1_xys[0][res][X_VAL],
		scrplt1_xys[0][res][Y_VAL],
		2.4f,
		SCR_PLATE_TID);
	scrplt3_obj->id = OID_SCRPLATE;

	while (1)
	{
		// Need to show everything?
		if (!score_plate_shown &&
			 show_score_plate >= 0)
		{
			set_string_id(SCOREPLATE_TXT_ID);
			set_text_font(fid_scrplt1[res]);
			set_text_transparency_mode(TRANSPARENCY_ENABLE);
			set_text_color(WHITE);
			set_text_z_inc(-0.1f);

			// Print city names
			set_text_justification_mode(HJUST_LEFT|VJUST_BOTTOM);

			is_low_res = 0;
			set_text_position(
				scrplt1_xys[2][res][X_VAL],
				scrplt1_xys[2][res][Y_VAL],
				2.2f);
			oprintf("%s", teaminfo[game_info.team[0]].szprfx);

			set_text_position(
				scrplt1_xys[4][res][X_VAL],
				scrplt1_xys[4][res][Y_VAL],
				2.2f);
			oprintf("%s", teaminfo[game_info.team[1]].szprfx);

			// Print scores
			set_text_justification_mode(HJUST_RIGHT|VJUST_BOTTOM);

			set_text_position(
				scrplt1_xys[3][res][X_VAL],
				scrplt1_xys[3][res][Y_VAL],
				2.2f);
			po_score0 = oprintf("%0" TO_STRING(SCRPLT_SCR_DIGCNT) "d", 0);

			set_text_position(
				scrplt1_xys[5][res][X_VAL],
				scrplt1_xys[5][res][Y_VAL],
				2.2f);
			po_score1 = oprintf("%0" TO_STRING(SCRPLT_SCR_DIGCNT) "d", 0);
			is_low_res = was_low_res;

			// Show the plate & say we're done here
			set_text_z_inc(0.0f);
			unhide_sprite(scrplt_obj);
			unhide_sprite(scrplt1_obj);
			unhide_sprite(scrplt2_obj);
			score0 = score1 = -1;
			score_plate_shown = 1;
		}

		// Need to show at least the clock?
		if (!small_plate_shown &&
			(show_score_plate >= 0 || !(game_info.game_time & 0x00ff0000)))
		{
			set_string_id(SCOREPLATE_CLOCK_TXT_ID);
			set_text_font(fid_scrplt1[res]);
			set_text_transparency_mode(TRANSPARENCY_ENABLE);
			set_text_color(WHITE);
			set_text_z_inc(-0.1f);

			// Print game time
			is_low_res = 0;
			set_text_position(
				scrplt1_xys[6][res][X_VAL],
				scrplt1_xys[6][res][Y_VAL],
				2.2f);
			set_text_justification_mode(HJUST_RIGHT|VJUST_BOTTOM);
			po_min = oprintf("%0" TO_STRING(SCRPLT_MIN_DIGCNT) "d", 0);

			set_text_position(
				scrplt1_xys[7][res][X_VAL],
				scrplt1_xys[7][res][Y_VAL],
				2.3f);
			po_col = oprintf(":");

			set_text_position(
				scrplt1_xys[8][res][X_VAL],
				scrplt1_xys[8][res][Y_VAL],
				2.2f);
			set_text_justification_mode(HJUST_LEFT|VJUST_BOTTOM);
			po_sec = oprintf("%0" TO_STRING(SCRPLT_SEC_DIGCNT) "d", 0);
			is_low_res = was_low_res;

			// Show the plate & say we're done here
			set_text_z_inc(0.0f);
			unhide_sprite(scrplt3_obj);
			min = sec = ten = -1;
			small_plate_shown = 1;
		}

		// Need to hide everything?
		if (score_plate_shown &&
			show_score_plate < 0)
		{
			delete_multiple_strings(SCOREPLATE_TXT_ID, -1);
			hide_sprite(scrplt_obj);
			hide_sprite(scrplt1_obj);
			hide_sprite(scrplt2_obj);

			cur_tick_score = color_switch_score = 0;
			score_plate_shown = 0;
		}

		// Need to hide at least the clock?
		if (small_plate_shown &&
			(show_score_plate < 0 && game_info.game_time & 0x00ff0000))
		{
			delete_multiple_strings(SCOREPLATE_CLOCK_TXT_ID, -1);
			hide_sprite(scrplt3_obj);

			small_plate_shown = 0;
		}

		// Need to update everything?
		if (score_plate_shown)
		{
			// Update score values & colors
			if ((i = compute_score(0)) != score0)
				update_ostring_value(po_score0, (score0 = i), 1);
			if ((i = compute_score(1)) != score1)
				update_ostring_value(po_score1, (score1 = i), 1);

			if ((cur_tick_score++ % 5) == 0)
			{
				color_switch_score ^= 1;
				change_alpha_cc(po_score0, (game_info.off_scored == 0 && color_switch_score) ? LT_RED:WHITE);
				change_alpha_cc(po_score1, (game_info.off_scored == 1 && color_switch_score) ? LT_RED:WHITE);
			}
		}

		// Need to update at least the clock?
		if (small_plate_shown)
		{
			// Update game time values
			// Chk minutes
			if ((i = ((game_info.game_time>>16)&255)))
			{
				if (i != min)
					update_ostring_value(po_min, (min = i), 1);
			}
			else
			// Chk tenths
			if ((i = (((game_info.game_time&255)*10)/(cmos_ticks_per_second+1))) != min)
			{
				if (ten)
				{
					ten = 0;
					po_sec->state.x = scrplt1_xys[ 9][res][X_VAL];
					po_col->state.x = scrplt1_xys[10][res][X_VAL];
					po_min->state.x = scrplt1_xys[11][res][X_VAL];
					po_sec->state.justification_mode = HJUST_RIGHT;
					po_min->state.justification_mode = HJUST_LEFT;
					update_ostring_value(po_col, -1, 0);
				}
				update_ostring_value(po_min, (min = i), 1);
			}
			// Chk seconds
			if ((i = ((game_info.game_time>> 8)&255)) != sec)
				update_ostring_value(po_sec, (sec = i), SCRPLT_SEC_DIGCNT);
		}

		// Chk the timer
		if (show_score_plate >= 0) show_score_plate--;

		sleep(1);
	}
}

/*****************************************************************************/
//
// Show End of quarter score plate at bottom of screen
//
/*****************************************************************************/
void eoq_score_plate_proc(int *args)
{
	sprite_info_t * scrplt_obj;
	sprite_info_t * scrplt2_obj;
	sprite_info_t * scrplt3_obj;
	sprite_info_t * team1_obj;
	sprite_info_t * team2_obj;
	sprite_info_t * team1_logo_obj;
	sprite_info_t * team2_logo_obj;

	int was_low_res = is_low_res;
	int plt = !!((check_scores() && game_info.game_quarter >= EOQ_4) || (game_info.game_quarter == EOQ_OT3));
	int res = !!was_low_res;
//	int ldr = !!(compute_score(0) > compute_score(1));
	int cnt;

#ifdef DBG_EOQ
	static int csi_idx = -1;
	VIEWCOORD cam = cambot;
#endif //DBG_EOQ

	if (plt)
		qcreate("winspch",0, win_game_speech_proc, 0, 0, 0, 0);

	scrplt_obj = beginobj(
		(plt ? &scrplaq3 : &scrplaq2),
		scrplt2_xys[plt][0][res][X_VAL],
		scrplt2_xys[plt][0][res][Y_VAL],
		2.4f,
		SCR_PLATE_TID);
	scrplt_obj->id = OID_SCRPLATE;

	scrplt2_obj = beginobj(
		(plt ? &scrplaq3_c1 : &scrplaq2_c1),
		scrplt2_xys[plt][0][res][X_VAL],
		scrplt2_xys[plt][0][res][Y_VAL],
		2.4f,
		SCR_PLATE_TID);
	scrplt2_obj->id = OID_SCRPLATE;

	// Print quarter
	scrplt3_obj = beginobj(
		(plt ? &final : qtr2_imgs[game_info.game_quarter]),
		scrplt2_xys[plt][1][res][X_VAL],
		scrplt2_xys[plt][1][res][Y_VAL],
		2.3f,
		SCR_PLATE_TID);
	scrplt3_obj->id = OID_SCRPLATE;

	// Print team name
	team1_obj = beginobj(
		scrplt2_team_imgs[game_info.team[0]][plt],
		scrplt2_xys[plt][2][res][X_VAL],
		scrplt2_xys[plt][2][res][Y_VAL],
		2.3f,
		SCR_PLATE_TID);
	team1_obj->id = OID_SCRPLATE;

	team2_obj = beginobj(
		scrplt2_team_imgs[game_info.team[1]][plt],
		scrplt2_xys[plt][4][res][X_VAL],
		scrplt2_xys[plt][4][res][Y_VAL],
		2.3f,
		SCR_PLATE_TID);
	team2_obj->id = OID_SCRPLATE;

	if (plt)
	{
		// Print team logo
		team1_logo_obj = beginobj(
			scrplt2_team_imgs[game_info.team[0]][2],
			scrplt2_xys[plt][6][res][X_VAL],
			scrplt2_xys[plt][6][res][Y_VAL],
			2.3f,
			SCR_PLATE_TID);
		team1_logo_obj->id = OID_SCRPLATE;
	//	team1_logo_obj->w_scale = 0.5f;
	//	team1_logo_obj->h_scale = 0.5f;
	//	generate_sprite_verts(team1_logo_obj);

		team2_logo_obj = beginobj(
			scrplt2_team_imgs[game_info.team[1]][2],
			scrplt2_xys[plt][7][res][X_VAL],
			scrplt2_xys[plt][7][res][Y_VAL],
			2.2f,
			SCR_PLATE_TID);
		team2_logo_obj->id = OID_SCRPLATE;
	//	team2_logo_obj->w_scale = 0.5f;
	//	team2_logo_obj->h_scale = 0.5f;
	//	generate_sprite_verts(team2_logo_obj);
	}
	else
	{
		team1_logo_obj = NULL;
		team2_logo_obj = NULL;
	}

	// Print scores
	set_string_id(BIG_SCOREPLATE_TXT_ID);
	set_text_font(fid_scrplt2[plt][res]);
	set_text_z_inc(0.05f);
	is_low_res = 0;

	set_text_justification_mode(HJUST_RIGHT|VJUST_BOTTOM);

	set_text_position(
		scrplt2_xys[plt][3][res][X_VAL],
		scrplt2_xys[plt][3][res][Y_VAL],
		2.2f);

	pmsg[0] = oprintf( "%dc%d", LT_YELLOW, compute_score(0));

	set_text_position(
		scrplt2_xys[plt][5][res][X_VAL],
		scrplt2_xys[plt][5][res][Y_VAL],
		2.2f);

	pmsg[1] = oprintf( "%dc%d", LT_YELLOW, compute_score(1));

	is_low_res = was_low_res;
	set_text_z_inc(0.00f);

#ifndef NOSETCAM

	// Select random camera seq
//	select_csi_item( csi_fs_endqtr );
//	select_csi_item( csi_rs_suplex );
	select_csi_item( csi_fs_endplay );

#ifdef DBG_EOQ
	if (csi_idx != -1)
		cambot.csi = csi_fs_endplay[csi_idx];
#endif //DBG_EOQ

	if (game_info.ball_handler >= 0)
		last_handler = game_info.ball_handler;

	cambot.pmodeobj = &player_blocks[last_handler].odata;

#endif //NOSETCAM


	// Loop till times out
	cnt = 0;
	do
	{
#ifndef NOSETCAM
#ifdef DBG_EOQ
		static int itm_idx = -1;
		static float svelx = 0.0f, svely = 0.0f;
		static char * itm_txt[] =
			{ "TIME MC-TC", "ROTATE", "MOVE D-Y", "MOVE X0-Z0", "MOVE X1-Z1", "MOVE Y0-Y1" };
		enum { MT_ITM, TP_ITM, DY_ITM, XZ0_ITM, XZ1_ITM, Y_ITM, CNT_ITM };
		void * pmodeobj = cambot.pmodeobj;

		if (get_dip_coin_current() & COINDOOR_INTERLOCK_SW)
		{
			static int last_sw = 0;
			static int repeat = 0;
			int csi_cnt = 0;
			int csi_num;
#if STICK_CAM_SW == 4
			int sw = get_p5_sw_current();
#else
			int sw = get_player_sw_current() >> (STICK_CAM_SW * 8);
#endif //STICK_CAM_SW == 4
			int sc = (sw ^ last_sw) & sw;
			sw &= 255;
			sc &= 255;
			if (!repeat)
				repeat = 1;
			if (sw && sw == last_sw)
				repeat--;
			else
				repeat = 15;
			last_sw = sw;

			while (csi_fs_endplay[csi_cnt].mode != -1) csi_cnt++;

			if (csi_cnt)
			{
				if (sw & STICK_CAM_BT)
				{
					// Check stick movement
					if (sc & 15)
					{
						if ((sc & P_UP) && !(sc & (P_LEFT|P_RIGHT)))
						{
							if (--csi_idx < 0) csi_idx = csi_cnt-1;
							itm_idx = -1;
						}
						else
						if ((sc & P_DOWN) && !(sc & (P_LEFT|P_RIGHT)))
						{
							if (++csi_idx == csi_cnt) csi_idx = 0;
							itm_idx = -1;
						}
						else
						if ((sc & P_LEFT) && !(sc & (P_UP|P_DOWN)))
						{
							if (--itm_idx < 0) itm_idx = CNT_ITM-1;
						}
						else
						if ((sc & P_RIGHT) && !(sc & (P_UP|P_DOWN)))
						{
							if (++itm_idx == CNT_ITM) itm_idx = 0;
						}
					}
				}
				else
				if ((sw & 15) && (csi_idx >= 0) && (itm_idx >= 0))
				{
					static float sw_vel[] = {0.0f, -0.005f,  0.005f};
					static float sw_acc[] = {0.0f, -0.0004f, 0.0004f};
					CAMSEQINFO * pcsi = &csi_fs_endplay[csi_idx];

					if (svelx == 0.0f || !((sw>>2) & 3))
						svelx  = sw_vel[((sw>>2) & 3)];
					else
						svelx += sw_acc[((sw>>2) & 3)];
					if (svely == 0.0f || !(sw & 3))
						svely  = sw_vel[sw & 3];
					else
						svely += sw_acc[sw & 3];

					switch (itm_idx)
					{
					case MT_ITM:
						if ((sc & P_LEFT) ||
							(sw == P_LEFT && !repeat))
						{
							if (--pcsi->mode_cnt < 0)
								pcsi->mode_cnt = 0;
						}
						else
						if ((sc & P_RIGHT) ||
							(sw == P_RIGHT && !repeat))
						{
							++pcsi->mode_cnt;
						}
						else
						if ((sc & P_UP) ||
							(sw == P_UP && !repeat))
						{
							if (--pcsi->trav_cnt < 0)
								pcsi->trav_cnt = 0;
						}
						else
						if ((sc & P_DOWN) ||
							(sw == P_DOWN && !repeat))
						{
							++pcsi->trav_cnt;
						}
						break;

					case TP_ITM:
						if ((sc & P_LEFT) ||
							(sw == P_LEFT && !repeat))
							pcsi->theta = DG2RD((int)((pcsi->theta*180.0f)/M_PI - 1.0f + (pcsi->theta<0.0f?-0.5f:pcsi->theta>0.0f?0.5f:0.0f)));
						else
						if ((sc & P_RIGHT) ||
							(sw == P_RIGHT && !repeat))
							pcsi->theta = DG2RD((int)((pcsi->theta*180.0f)/M_PI + 1.0f + (pcsi->theta<0.0f?-0.5f:pcsi->theta>0.0f?0.5f:0.0f)));
						else
						if ((sc & P_UP) ||
							(sw == P_UP && !repeat))
							pcsi->phi = DG2RD((int)((pcsi->phi*180.0f)/M_PI - 1.0f + (pcsi->phi<0.0f?-0.5f:pcsi->phi>0.0f?0.5f:0.0f)));
						else
						if ((sc & P_DOWN) ||
							(sw == P_DOWN && !repeat))
							pcsi->phi = DG2RD((int)((pcsi->phi*180.0f)/M_PI + 1.0f + (pcsi->phi<0.0f?-0.5f:pcsi->phi>0.0f?0.5f:0.0f)));
						break;

					case DY_ITM:
						pcsi->d        += svelx;
						pcsi->trgy_adj += svely;
						break;

					case XZ0_ITM:
						((float *)pcsi->pseqinfo)[0] += svelx * 1.00f;
						((float *)pcsi->pseqinfo)[2] += svely * 1.00f;
						break;

					case XZ1_ITM:
						((float *)pcsi->pseqinfo)[3] += svelx * 1.00f;
						((float *)pcsi->pseqinfo)[5] += svely * 1.00f;
						break;

					case Y_ITM:
						((float *)pcsi->pseqinfo)[1] += svelx * 1.00f;
						((float *)pcsi->pseqinfo)[4] += svely * 1.00f;
						break;
					}
				}
				else
				{
					svelx = svely = 0.0f;
				}
			}
			cambot = cam;
			cambot.pmodeobj = pmodeobj;
			csi_num = (csi_idx >= 0) ? csi_idx : randrng(csi_cnt);
			cambot.csi = csi_fs_endplay[csi_num];
			// Reset elapsed-time to start`er up
			cambot.tick_cnt = 0;
			cnt = 0;

			delete_multiple_strings(0xbabacaba, -1);

			set_text_font(FONTID_NBA10);
			set_string_id(0xbabacaba);
			set_text_transparency_mode(TRANSPARENCY_ENABLE);
			set_text_z_inc(0.01f);

			set_text_position(256, 230, 1.0f);
			oprintf("%dj%dcCSI %d", (HJUST_CENTER|VJUST_BOTTOM), WHITE,
				csi_num);

			set_text_position(256, 217, 1.0f);
			oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_BOTTOM), LT_YELLOW,
				(itm_idx < 0 ? "SELECT" : itm_txt[itm_idx]) );

			if (csi_idx >= 0)
			{
				CAMSEQINFO * pcsi = &csi_fs_endplay[csi_idx];
				float fx_fs0 = 80;
				float fx_fs1 = 210;
				float fy_fs  = 204;
				float fx_csi = 340;
				float fy_csi = 204;
				float fyoff = -13;
				int c0 = LT_GREEN;
				int c1 = WHITE;

				set_text_position(fx_csi, fy_csi, 1.2f);
				oprintf("%dj%dc%s",(HJUST_LEFT|VJUST_CENTER),
					LT_YELLOW,"CSI PARMS:");
				fy_csi += fyoff;
				set_text_position(fx_csi, fy_csi, 1.2f);
				oprintf("%dj%dc%s%dc%d",(HJUST_LEFT|VJUST_CENTER),
					LT_YELLOW,"M-CNT: ",(itm_idx==MT_ITM?c1:c0), pcsi->mode_cnt);
				fy_csi += fyoff;
				set_text_position(fx_csi, fy_csi, 1.2f);
				oprintf("%dj%dc%s%dc%d",(HJUST_LEFT|VJUST_CENTER),
					LT_YELLOW,"T-CNT: ",(itm_idx==MT_ITM?c1:c0), pcsi->trav_cnt);
				fy_csi += fyoff;
				set_text_position(fx_csi, fy_csi, 1.2f);
				oprintf("%dj%dc%s%dc%d",(HJUST_LEFT|VJUST_CENTER),
					LT_YELLOW,"T: ",(itm_idx==TP_ITM?c1:c0), (int)((pcsi->theta*180.0f)/M_PI + (pcsi->theta<0.0f?-0.5f:pcsi->theta>0.0f?0.5f:0.0f)));
				fy_csi += fyoff;
				set_text_position(fx_csi, fy_csi, 1.2f);
				oprintf("%dj%dc%s%dc%d",(HJUST_LEFT|VJUST_CENTER),
					LT_YELLOW,"P: ",(itm_idx==TP_ITM?c1:c0), (int)((pcsi->phi*180.0f)/M_PI + (pcsi->phi<0.0f?-0.5f:pcsi->phi>0.0f?0.5f:0.0f)));
				fy_csi += fyoff;
				set_text_position(fx_csi, fy_csi, 1.2f);
				oprintf("%dj%dc%s%dc%f",(HJUST_LEFT|VJUST_CENTER),
					LT_YELLOW,"D: ",(itm_idx==DY_ITM?c1:c0), pcsi->d);
				fy_csi += fyoff;
				set_text_position(fx_csi, fy_csi, 1.2f);
				oprintf("%dj%dc%s%dc%f",(HJUST_LEFT|VJUST_CENTER),
					LT_YELLOW,"Y: ",(itm_idx==DY_ITM?c1:c0), pcsi->trgy_adj);


				set_text_position((fx_fs0+fx_fs1)*0.5f, fy_fs, 1.2f);
				oprintf("%dj%dc%s",(HJUST_LEFT|VJUST_CENTER),
					LT_YELLOW,"KNOTS:");
				fy_fs += fyoff;
				set_text_position(fx_fs0, fy_fs, 1.2f);
				oprintf("%dj%dc%s%dc%f",(HJUST_LEFT|VJUST_CENTER),
					LT_YELLOW,"X0: ",(itm_idx==XZ0_ITM?c1:c0), ((float *)pcsi->pseqinfo)[0]);
				set_text_position(fx_fs1, fy_fs, 1.2f);
				oprintf("%dj%dc%s%dc%f",(HJUST_LEFT|VJUST_CENTER),
					LT_YELLOW,"X1: ",(itm_idx==XZ1_ITM?c1:c0), ((float *)pcsi->pseqinfo)[3]);
				fy_fs += fyoff;
				set_text_position(fx_fs0, fy_fs, 1.2f);
				oprintf("%dj%dc%s%dc%f",(HJUST_LEFT|VJUST_CENTER),
					LT_YELLOW,"Y0: ",(itm_idx==Y_ITM?c1:c0), ((float *)pcsi->pseqinfo)[1]);
				set_text_position(fx_fs1, fy_fs, 1.2f);
				oprintf("%dj%dc%s%dc%f",(HJUST_LEFT|VJUST_CENTER),
					LT_YELLOW,"Y1: ",(itm_idx==Y_ITM?c1:c0), ((float *)pcsi->pseqinfo)[4]);
				fy_fs += fyoff;
				set_text_position(fx_fs0, fy_fs, 1.2f);
				oprintf("%dj%dc%s%dc%f",(HJUST_LEFT|VJUST_CENTER),
					LT_YELLOW,"Z0: ",(itm_idx==XZ0_ITM?c1:c0), ((float *)pcsi->pseqinfo)[2]);
				set_text_position(fx_fs1, fy_fs, 1.2f);
				oprintf("%dj%dc%s%dc%f",(HJUST_LEFT|VJUST_CENTER),
					LT_YELLOW,"Z1: ",(itm_idx==XZ1_ITM?c1:c0), ((float *)pcsi->pseqinfo)[5]);
			}
			set_text_z_inc(0.00f);
		}
		else
			delete_multiple_strings(0xbabacaba, -1);
#endif //DBG_EOQ
#endif //NOSETCAM

		sleep(1);

		if ((cnt > 60) && (
			((p_status & 1) && ((get_player_sw_current() & (P1_A|P1_B|P1_C)) == (P1_A|P1_B|P1_C))) ||
			((p_status & 2) && ((get_player_sw_current() & (P2_A|P2_B|P2_C)) == (P2_A|P2_B|P2_C))) ||
			(!four_plr_ver &&
				((p_status & 4) && ((get_player_sw_current() & (P2_A|P2_B|P2_C)) == (P2_A|P2_B|P2_C)))) ||
			( four_plr_ver &&
				((p_status & 4) && ((get_player_sw_current() & (P3_A|P3_B|P3_C)) == (P3_A|P3_B|P3_C)))) ||
			((p_status & 8) && ((get_player_sw_current() & (P4_A|P4_B|P4_C)) == (P4_A|P4_B|P4_C)))
			))
			break;

#ifdef DEBUG
		if (halt) continue;
#endif
		if ((++cnt >= EOQ_CNT)

#ifndef NOSETCAM
#ifdef DBG_EOQ
			&& (get_player_sw_current() & ALL_ABCD_MASK)
#endif //DBG_EOQ
#endif //NOSETCAM
			)
			break;

	} while (1);
}

/*****************************************************************************/
//
// Show NBC logo at bottom of screen
// Hide when regular score plate appears at bottom of screen
//
/*****************************************************************************/

void show_peacock_proc(int *args)
{
	sprite_info_t * peacock_obj;

	sleep(2);
	peacock_obj = beginobj(
		&nbccolor,
		0.0f,
		390.0f,
		2.9f,
		SCR_PLATE_TID);
	peacock_obj->id = OID_SCRPLATE;

	for (;;) {
		if ((hide_peacock) || (show_score_plate != -1) || (!(game_info.game_time & 0x00ff0000)))
			hide_sprite(peacock_obj);
		else
			unhide_sprite(peacock_obj);
		sleep(1);
	}
}


static float show_3pt_xys[MAX_PLYRS][2][2] =	// [plyr][2p,4p][x,y]
{
	{{103.0f,300.0f}, {103.0f,300.0f}},		// player 1
	{{103.0f,300.0f}, {103.0f,300.0f}},		// player 2
	{{410.0f,300.0f}, {410.0f,300.0f}},		// player 3
	{{410.0f,300.0f}, {410.0f,300.0f}},		// player 4
};											

static float show_tmfire_xys[MAX_PLYRS][2][2] =	// [plyr][2p,4p][x,y]
{
	{{108.0f,300.0f}, {108.0f,300.0f}},		// player 1
	{{108.0f,300.0f}, {108.0f,300.0f}},		// player 2
	{{405.0f,300.0f}, {405.0f,300.0f}},		// player 3
	{{405.0f,300.0f}, {405.0f,300.0f}},		// player 4
};											

//

//-------------------------------------------------------------------------------------------------
//	This process displays the 3 pt message under player who just scored
//
//	INPUT:	Nothing
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void show_3pt_proc(int *args)
{
	int		i, j, pnum, ball_x, ball_y;
	long alpha;
	static	sprite_info_t		*show_3pt_obj;
	static	sprite_info_t		*show_3pt_obj2;
	static	sprite_info_t		*show_3pt_obj3;
	static	sprite_info_t		*show_3pt_obj4;

	pnum = game_info.sc_scored;
	if ((pnum < 0) || (pnum > 3))
		return;

	ball_x = show_3pt_xys[pnum][four_plr_ver][X_VAL];
	ball_y = show_3pt_xys[pnum][four_plr_ver][Y_VAL];

//	while (1)
//	{
		// create the 3 pt bubble
		show_3pt_obj = beginobj(&threept, ball_x, ball_y, 1.4f, SCR_PLATE_TID);
		show_3pt_obj->id = OID_3PT_MSG;
		qcreate("flash", 1, flash_obj_proc, (int)show_3pt_obj, 2, 3, 0);

		// Turn on a big flash across 3pt bubble
		qcreate("dsc_flsh",SPFX_PID+pnum,disc_flash_3pt_proc,ball_x,ball_y,0,0);

		sleep(5);
		snd_scall_bank(gameplay_bnk_str, 119 , VOLUME7, 127, PRIORITY_LVL3);

		sleep(35);

		// Turn on a big flash across 3pt bubble
		qcreate("dsc_flsh",SPFX_PID+pnum,disc_flash_3pt_proc,ball_x,ball_y,0,0);

		sleep(2);
		snd_scall_bank(gameplay_bnk_str, 119 , VOLUME7, 127, PRIORITY_LVL3);
		sleep(28);

		for (j = 1; j <= 120; j += 3) {
			alpha = ((256 / 120) * j) - 1;
			alpha = 255 - alpha;
			show_3pt_obj->state.constant_color &= 0x00FFFFFF;
			show_3pt_obj->state.constant_color |= alpha << 24;
			sleep(1);
		}

		del1c(OID_3PT_MSG,0xFFFFFFFF);
//		delobj(show_3pt_obj);
//		sleep(20);
//	}
}

//-------------------------------------------------------------------------------------------------
//	This process displays the tm fire message under team who just scored
//
//	INPUT:	Nothing
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void show_tmfire_proc(int *args)
{
	int		i, j, pnum, ball_x, ball_y;
	long alpha;
	static	sprite_info_t		*show_tmfire_obj;

	pnum = game_info.sc_scored;
	if ((pnum < 0) || (pnum > 3))
		return;

	ball_x = show_tmfire_xys[pnum][four_plr_ver][X_VAL];
	ball_y = show_tmfire_xys[pnum][four_plr_ver][Y_VAL];

//	while (1)
//	{
		// create the tm fire txt
		show_tmfire_obj = beginobj(&teamfire, ball_x, ball_y, 1.4f, SCR_PLATE_TID);
		show_tmfire_obj->id = OID_TMFIRE_MSG;
		qcreate("flash", 1, flash_obj_proc, (int)show_tmfire_obj, 7, 4, 0);

		// Turn on a big flash across 3pt bubble
//		qcreate("dsc_flsh",SPFX_PID+pnum,disc_flash_3pt_proc,ball_x,ball_y,0,0);

		sleep(40);

		// Turn on a big flash across 3pt bubble
//		qcreate("dsc_flsh",SPFX_PID+pnum,disc_flash_3pt_proc,ball_x,ball_y,0,0);

		sleep(70);

		for (j = 1; j <= 120; j += 3) {
			alpha = ((256 / 120) * j) - 1;
			alpha = 255 - alpha;
			show_tmfire_obj->state.constant_color &= 0x00FFFFFF;
			show_tmfire_obj->state.constant_color |= alpha << 24;
			sleep(1);
		}

		del1c(OID_TMFIRE_MSG,0xFFFFFFFF);
//		delobj(show_tmfire_obj);
//		sleep(20);
//	}
}
//-------------------------------------------------------------------------------------------------
//	This process displays the free throw message
//-------------------------------------------------------------------------------------------------
void free_throw_msg_proc(int *args)
{
	int i, z;

//while(1)
//{	
//	if (!flash_message_active)
//	{
//		flash_message_active = TRUE;
		flash_ft_letters = FALSE;
		qcreate("ft_snd", 0, GT_snd_proc, 0, 0, 0, 0);
		for (i = 0, z = 91; i < 6; i++, z--) {
			if (i <= 3)
				iqcreate("shrinkft", VIOLATION_PID, shrink_ft_letter_proc, i, z, 0, 0);
			sleep(1);
			iqcreate("shrinkft", VIOLATION_PID, shrink_ft_letter_proc, i + 4, z, 0, 0);
			sleep(1);
		}
		sleep(20);
		flash_ft_letters = TRUE;
		sleep(60);
//		sleep(240);
		flash_ft_letters = FALSE;
		/* del1c(OID_SC_MESSAGE, 0xFFFFFFFF); really not need, the shrink proc deletes the objs */
//		flash_message_active = FALSE;

		goto_freethrow_line_speech();

//	}
//	sleep(20);
//}
}


//-------------------------------------------------------------------------------------------------
//	This process scales down letter on top line
//
//	INPUT:	args[0] = letter table index
//			args[1] = z
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void shrink_ft_letter_proc(int *args)
{
	int i, j;
	sprite_info_t *letter_obj;
	sprite_node_t *snode;
	long alpha;

	i = args[0];
	letter_obj = beginobj(free_throw_letter_imgs[i],
						  free_throw_letter_xys[i][X_VAL],
						  free_throw_letter_xys[i][Y_VAL],
						  args[1],
						  MESSAGE_TID);
	
	/* Get the sprite node pointer */
	snode = (sprite_node_t *)letter_obj->node_ptr;
	
	/* Turn on velocity add bit */
	snode->mode |= DO_VEL_ADD;
	
	letter_obj->id = OID_FOUL_MESSAGE;
	letter_obj->w_scale = 3.0f;
	letter_obj->h_scale = 3.0f;
	
	letter_obj->y_angle = 1.80f;
	letter_obj->y_ang_vel = -0.09f;
	generate_sprite_verts(letter_obj);
	
	while (letter_obj->w_scale > 1.1F) {
		letter_obj->w_scale -= .10f;
		letter_obj->h_scale -= .10f;
		generate_sprite_verts(letter_obj);
		sleep(1);
	}
	
	letter_obj->y_angle = 0;
	letter_obj->y_ang_vel = 0;
	
	letter_obj->w_scale = 1.00f;
	letter_obj->h_scale = 1.00f;
	generate_sprite_verts(letter_obj);
	
	while (!flash_ft_letters)
		sleep(1);
	
	flash_obj_white(letter_obj, 3, 6);
	
	sleep(20 - ((i > 3) ? (i - 3) : i));

	letter_obj->x_vel = ft_letter_xvel[i];
	letter_obj->y_vel = (i > 3) ? -0.15f : 0.15f;
	
	for (j = 1; j <= 120; j += 2) {
		alpha = ((256 / 120) * j) - 1;
		alpha = 255 - alpha;
		letter_obj->state.constant_color &= 0x00FFFFFF;
		letter_obj->state.constant_color |= alpha << 24;
		sleep(1);
	}
	delobj(letter_obj);
}


//-------------------------------------------------------------------------------------------------
//	This process displays the shot clock violation message on screen
//-------------------------------------------------------------------------------------------------
void shotclock_violation_proc(int *args)
{
	int i, z;
	
//	if (!flash_message_active) {
//		flash_message_active = TRUE;
		flash_sc_letters = FALSE;
		qcreate("SC", 0, GT_snd_proc, 0, 0, 0, 0);
		for (i = 0, z = 91; i < 10; i++, z--) {
			if (i != 9)
				iqcreate("shrinksc", VIOLATION_PID, shrink_sc_letter_proc, i, z, 0, 0);
			sleep(1);
			iqcreate("shrinksc", VIOLATION_PID, shrink_sc_letter_proc, i + 9, z, 0, 0);
			sleep(1);
		}
		sleep(20);
		flash_sc_letters = TRUE;
		sleep(240);
		flash_sc_letters = FALSE;
		/* del1c(OID_SC_MESSAGE, 0xFFFFFFFF); really not need, the shrink proc deletes the objs */
//		flash_message_active = FALSE;
//	}
}


//-------------------------------------------------------------------------------------------------
//	This process scales down letter on top line
//
//	INPUT:	args[0] = letter table index
//			args[1] = z
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void shrink_sc_letter_proc(int *args)
{
	int i, j;
	sprite_info_t *letter_obj;
	sprite_node_t *snode;
	long alpha;
	
	i = args[0];
	letter_obj = beginobj(shot_clock_letter_imgs[i],
						  shot_clock_letter_xys[i][X_VAL],
						  shot_clock_letter_xys[i][Y_VAL],
						  args[1],
						  MESSAGE_TID);
	
	/* Get the sprite node pointer */
	snode = (sprite_node_t *)letter_obj->node_ptr;
	
	/* Turn on velocity add bit */
	snode->mode |= DO_VEL_ADD;
	
	letter_obj->id = OID_SC_MESSAGE;
	letter_obj->w_scale = 3.0f;
	letter_obj->h_scale = 3.0f;
	
	letter_obj->y_angle = 1.80f;
	letter_obj->y_ang_vel = -0.09f;
	generate_sprite_verts(letter_obj);
	
	while (letter_obj->w_scale > 1.1F) {
		letter_obj->w_scale -= .10f;
		letter_obj->h_scale -= .10f;
		generate_sprite_verts(letter_obj);
		sleep(1);
	}
	
	letter_obj->y_angle = 0;
	letter_obj->y_ang_vel = 0;
	
	letter_obj->w_scale = 1.00f;
	letter_obj->h_scale = 1.00f;
	generate_sprite_verts(letter_obj);
	
	while (!flash_sc_letters)
		sleep(1);
	
	flash_obj_white(letter_obj, 3, 6);
	
	sleep(20 - ((i > 8) ? (i - 8) : i));

	letter_obj->x_vel = sc_letter_xvel[i];
	letter_obj->y_vel = (i > 8) ? -0.15f : 0.15f;
	
	for (j = 1; j <= 120; j += 2) {
		alpha = ((256 / 120) * j) - 1;
		alpha = 255 - alpha;
		letter_obj->state.constant_color &= 0x00FFFFFF;
		letter_obj->state.constant_color |= alpha << 24;
		sleep(1);
	}
	delobj(letter_obj);
}


//-------------------------------------------------------------------------------------------------
//	This process displays the goal tending message on screen
//-------------------------------------------------------------------------------------------------
void goaltending_proc(int *args)
{
	int i, z;
	
//	if (!flash_message_active) {
//		flash_message_active = TRUE;
		flash_gt_letters = FALSE;
		qcreate("GT", 0, GT_snd_proc, 0, 0, 0, 0);
		for (i = 0, z = 91; i < 12; i++, z--) {
			if (!((i <= 2) || (i >= 10)))
				iqcreate("shrinkgt", VIOLATION_PID, shrink_gt_letter2_proc, i, z, 0, 0);
			sleep(1);
			iqcreate("shrinkgt", VIOLATION_PID, shrink_gt_letter_proc, i, z, 0, 0);
			sleep(1);
		}
		sleep(20);
		flash_gt_letters = TRUE;
		sleep(240);
		flash_gt_letters = FALSE;
		/* del1c(OID_GT_MESSAGE, 0xFFFFFFFF); really not need, the shrink proc deletes the objs */
//		flash_message_active = FALSE;
//	}
}

//------------------------------------------------------------------------------------------------------------------------------
//	This process welcomes the player with some speech
//
//	INPUT:	Nothing
//	OUTPUT:	Nothing
//------------------------------------------------------------------------------------------------------------------------------
static void GT_snd_proc(int *args)
{
	snd_scall_bank(gameplay_bnk_str, 45 , VOLUME4, 127, PRIORITY_LVL4);
	sleep(20);
	snd_scall_bank(gameplay_bnk_str, 45 , VOLUME4, 127, PRIORITY_LVL4);
	sleep(20);
	snd_scall_bank(gameplay_bnk_str, 45 , VOLUME4, 127, PRIORITY_LVL4);
	sleep(20);
	snd_scall_bank(gameplay_bnk_str, 45 , VOLUME4, 127, PRIORITY_LVL4);
}

//-------------------------------------------------------------------------------------------------
//	This process scales down letter
//
//	INPUT:	args[0] = letter table index
//			args[1] = z
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void shrink_gt_letter_proc(int *args)
{
	int i, j;
	sprite_info_t *letter_obj;
	sprite_node_t *snode;
	long alpha;
	
	i = args[0];
	letter_obj = beginobj(goaltending_letter_imgs[i],
						  goaltending_letter_xys[i][X_VAL],
						  goaltending_letter_xys[i][Y_VAL],
						  args[1],
						  MESSAGE_TID);
	
	/* Get the sprite node pointer */
	snode = (sprite_node_t *)letter_obj->node_ptr;
	
	/* Turn on velocity add bit */
	snode->mode |= DO_VEL_ADD;
	
	letter_obj->id = OID_GT_MESSAGE;
	letter_obj->w_scale = 3.0f;
	letter_obj->h_scale = 3.0f;
	
	letter_obj->y_angle = 1.80f;
	letter_obj->y_ang_vel = -0.09f;
	generate_sprite_verts(letter_obj);
	
	while (letter_obj->w_scale > 1.1F) {
		letter_obj->w_scale -= .10f;
		letter_obj->h_scale -= .10f;
		generate_sprite_verts(letter_obj);
		sleep(1);
	}
	
	letter_obj->y_angle = 0;
	letter_obj->y_ang_vel = 0;
	
	letter_obj->w_scale = 1.00f;
	letter_obj->h_scale = 1.00f;
	generate_sprite_verts(letter_obj);
	
	while (!flash_gt_letters)
		sleep(1);
	
	flash_obj_white(letter_obj, 3, 6);
	
	sleep(20 - i);
	
	letter_obj->x_vel = gt_letter_xvel[i];
	letter_obj->y_vel = 0.15f;
	
	for (j = 1; j <= 120; j += 2) {
		alpha = ((256 / 120) * j) - 1;
		alpha = 255 - alpha;
		letter_obj->state.constant_color &= 0x00FFFFFF;
		letter_obj->state.constant_color |= alpha << 24;
		sleep(1);
	}
	delobj(letter_obj);
}



//-------------------------------------------------------------------------------------------------
//	This process scales down letter on second line
//
//	INPUT:	args[0] = letter table index
//			args[1] = z
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void shrink_gt_letter2_proc(int *args)
{
	int i, j;
	sprite_info_t *letter_obj;
	sprite_node_t *snode;
	long alpha;

	i = args[0];
	if (what_type_shot != JUMPER_3)
		letter_obj = beginobj(two_pts_letter_imgs[i - 3],
							  two_pts_letter_xys[i - 3][X_VAL],
							  two_pts_letter_xys[i - 3][Y_VAL],
							  args[1],
							  MESSAGE_TID);
	else
		letter_obj = beginobj(three_pts_letter_imgs[i - 3],
							  two_pts_letter_xys[i - 3][X_VAL],
							  two_pts_letter_xys[i - 3][Y_VAL],
							  args[1],
							  MESSAGE_TID);
	
	/* Get the sprite node pointer */
	snode = (sprite_node_t *)letter_obj->node_ptr;
	
	/* Turn on velocity add bit */
	snode->mode |= DO_VEL_ADD;
	
	letter_obj->id = OID_GT_MESSAGE;
	letter_obj->w_scale = 3.0f;
	letter_obj->h_scale = 3.0f;
	
	letter_obj->y_angle = 1.80f;
	letter_obj->y_ang_vel = -0.09f;
	generate_sprite_verts(letter_obj);
	
	while (letter_obj->w_scale > 1.1F) {
		letter_obj->w_scale -= .10f;
		letter_obj->h_scale -= .10f;
		generate_sprite_verts(letter_obj);
		sleep(1);
	}
	
	letter_obj->y_angle = 0;
	letter_obj->y_ang_vel = 0;
	
	letter_obj->w_scale = 1.00f;
	letter_obj->h_scale = 1.00f;
	generate_sprite_verts(letter_obj);
	
	while(!flash_gt_letters)
		sleep(1);
	
	flash_obj_white(letter_obj, 3, 6);
	
	sleep(20 - i);
	
	letter_obj->x_vel = gt_letter_xvel[i + 12 - 3];
	letter_obj->y_vel = -0.15f;
	
	for (j = 1; j <= 120; j += 2) {
		alpha = ((256 / 120) * j) - 1;
		alpha = 255 - alpha;
		letter_obj->state.constant_color &= 0x00FFFFFF;
		letter_obj->state.constant_color |= alpha << 24;
		sleep(1);
	}
	delobj(letter_obj);
}

//-------------------------------------------------------------------------------------------------
// This process updates the status display at the top/middle of the screen
//-------------------------------------------------------------------------------------------------
void score_status_box(int *args)
{
	sprite_info_t * status_box_obj;				// Box itself
	sprite_info_t * status_box2_obj;			// Quarter image
	sprite_info_t * status_box3_obj;			// Shot % backdrop
	ostring_t * po_min, * po_sec, * po_col;
	ostring_t * po_tm0, * po_tm1, * po_per;
	int was_hidden, qtr, min, sec, ten, i;
	int	was_low_res = is_low_res;

	qtr = -1;
	update_status_box = 1;						// print everything first time through
	show_shot_pct = 0;							// Ticks to display %
	hide_status_box = SHOW;						// Top score box
	was_hidden = SHOW;

	// Turn on actual status box at top
	status_box_obj = beginobj(status_box_imgs[was_low_res],
		status_box_xys[0][0][X_VAL],
		status_box_xys[0][0][Y_VAL],
		STBX_Z,
		SCR_PLATE_TID);
	status_box_obj->id = OID_INFOBOX;

	// Turn on status box shot percent backdrop
	status_box3_obj = beginobj(status_box_imgs[2],
		status_box_xys[10][0][X_VAL],
		status_box_xys[10][0][Y_VAL]+12,
		STBX_Z,
		SCR_PLATE_TID);
	status_box3_obj->id = OID_INFOBOX;

	status_box3_obj->h_scale = 1.55f;
	generate_sprite_verts(status_box3_obj);
	hide_sprite(status_box3_obj);

	// Make score strings
	if (is_low_res)
		set_text_font(FONTID_NBA12);
	else
		set_text_font(FONTID_NBA14);
	set_string_id(SCORE_STATUSBOX_TEXT_ID);
	set_text_color(WHITE);
	set_text_z_inc(0.01f);

	set_text_justification_mode(HJUST_CENTER|VJUST_BOTTOM);

	is_low_res = 0;
	set_text_position(
		status_box_xys[1][was_low_res][X_VAL],
		status_box_xys[1][was_low_res][Y_VAL],
		STBX_Z - 0.40f);
	po_tm0 = oprintf("%0" TO_STRING(SCRPLT_SCR_DIGCNT) "d", 0);

	set_text_position(
		status_box_xys[2][was_low_res][X_VAL],
		status_box_xys[2][was_low_res][Y_VAL],
		STBX_Z - 0.40f);
	po_tm1 = oprintf("%0" TO_STRING(SCRPLT_SCR_DIGCNT) "d", 0);

	// Make clock strings
	if (was_low_res)
		set_text_font(FONTID_BAST7T);
	else
		set_text_font(FONTID_NBA10);
	set_text_z_inc(0.01f);

	set_text_justification_mode(HJUST_RIGHT|VJUST_BOTTOM);

	set_text_position(
		status_box_xys[4][was_low_res][X_VAL],
		status_box_xys[4][was_low_res][Y_VAL],
		STBX_Z - 0.30f);
	po_min = oprintf("%0" TO_STRING(SCRPLT_MIN_DIGCNT) "d", 0);

	set_text_position(
		status_box_xys[5][was_low_res][X_VAL],
		status_box_xys[5][was_low_res][Y_VAL],
		STBX_Z - 0.20f);
	po_col = oprintf(":");

	set_text_justification_mode(HJUST_LEFT|VJUST_BOTTOM);

	set_text_position(
		status_box_xys[6][was_low_res][X_VAL],
		status_box_xys[6][was_low_res][Y_VAL],
		STBX_Z - 0.10f);
	po_sec = oprintf("%0" TO_STRING(SCRPLT_SEC_DIGCNT) "d", 0);
	is_low_res = was_low_res;

	// Make shot % string
	po_per = NULL;

	set_text_z_inc(0.00f);

	while (1)
	{
		// Chk to update qtr img & reset clock ordering
		if (qtr != game_info.game_quarter)
		{
			qtr = game_info.game_quarter;

			// Turn on quarter number image
			del1c((OID_INFOBOX)+1, -1);
			status_box2_obj = beginobj(qtr_imgs[qtr],
				status_box_xys[3][0][X_VAL],
				status_box_xys[3][0][Y_VAL],
				STBX_Z - 0.10f,
				SCR_PLATE_TID);
			status_box2_obj->id = (OID_INFOBOX)+1;

			po_sec->state.x = status_box_xys[6][was_low_res][X_VAL];
			po_col->state.x = status_box_xys[5][was_low_res][X_VAL];
			po_min->state.x = status_box_xys[4][was_low_res][X_VAL];
			po_sec->state.justification_mode = HJUST_LEFT;
			po_min->state.justification_mode = HJUST_RIGHT;

			min = sec = ten = -1;
			show_shot_pct = 0;
		}
		// Chk to hide & show status box
		while (hide_status_box == HIDE || hide_status_box != was_hidden)
		{
			if (hide_status_box != was_hidden)
			{
				if (hide_status_box == HIDE)
				{
					sprite_node_t * slist = alpha_sprite_node_list;
					while (slist)
					{
						if ((slist->si->parent == po_min) ||
							(slist->si->parent == po_sec) ||
							(slist->si->parent == po_col) ||
							(slist->si->parent == po_tm0) ||
							(slist->si->parent == po_tm1) ||
							(slist->si == status_box_obj) ||
							(slist->si == status_box2_obj) ||
							(slist->si == status_box3_obj))
						{
							hide_sprite(slist->si);
						}
						slist = slist->next;
					}
					if (was_low_res)
					{
						slist = sprite_node_list;
						while (slist)
						{
							if ((slist->si->parent == po_min) ||
								(slist->si->parent == po_sec) ||
								(slist->si->parent == po_col))
							{
								hide_sprite(slist->si);
							}
							slist = slist->next;
						}
					}

					delete_multiple_strings(SHOT_DISPLAY_TXT_ID, -1);
					po_per = NULL;
				}
				was_hidden = hide_status_box;
			}
			if (hide_status_box == SHOW && was_hidden == SHOW)
			{
				unhide_sprite(status_box_obj);
				unhide_sprite(status_box2_obj);
				min = sec = ten = -1;
				update_status_box = 1;
				break;
			}
			sleep(1);
		}
		// Update shot % status
		if (show_shot_pct != 0 && po_per == NULL)
		{
			// Make shot % string
			set_text_font(FONTID_BAST7T);
			set_string_id(SHOT_DISPLAY_TXT_ID);
			set_text_color(LT_YELLOW);
			set_text_z_inc(0.01f);
			is_low_res = 0;

			set_text_justification_mode(HJUST_CENTER|VJUST_CENTER);

			set_text_position(
				status_box_xys[11][was_low_res][X_VAL],
				status_box_xys[11][was_low_res][Y_VAL],
				STBX_Z - 0.40f);
			if ((po_per = oprintf("%d%%", curr_shot_pct)))
				unhide_sprite(status_box3_obj);
			else
				show_shot_pct = 0;

			is_low_res = was_low_res;
			set_text_z_inc(0.00f);
		}
		if (show_shot_pct == 0 && po_per != NULL)
		{
			hide_sprite(status_box3_obj);
			delete_multiple_strings(SHOT_DISPLAY_TXT_ID, -1);
			po_per = NULL;
		}
		if (show_shot_pct > 0)
			show_shot_pct--;
		// Update game time values
		// Chk minutes
		is_low_res = 0;
		if ((i = ((game_info.game_time>>16)&255)))
		{
			if (i != min)
			{
				if (was_low_res)
				{
					update_ostring_value_nonalpha(po_min, (min = i), 1);
					update_ostring_value_nonalpha(po_col, -2, 0);
				}
				else
				{
					update_ostring_value(po_min, (min = i), 1);
					update_ostring_value(po_col, -2, 0);
				}
			}
		}
		else
		// Chk tenths
		if ((i = (((game_info.game_time&255)*10)/(cmos_ticks_per_second+1))) != min)
		{
			is_low_res = 0;
			if (ten)
			{
				ten = 0;
				po_sec->state.x = status_box_xys[7][was_low_res][X_VAL];
				po_col->state.x = status_box_xys[8][was_low_res][X_VAL];
				po_min->state.x = status_box_xys[9][was_low_res][X_VAL];
				po_sec->state.justification_mode = HJUST_RIGHT;
				po_min->state.justification_mode = HJUST_LEFT;
				if (was_low_res)
					update_ostring_value_nonalpha(po_col, -1, 0);
				else
					update_ostring_value(po_col, -1, 0);
			}
			if (was_low_res)
				update_ostring_value_nonalpha(po_min, (min = i), 1);
			else
				update_ostring_value(po_min, (min = i), 1);
		}
		// Chk seconds
		if ((i = ((game_info.game_time>> 8)&255)) != sec)
		{
			if (was_low_res)
				update_ostring_value_nonalpha(po_sec, (sec = i), SCRPLT_SEC_DIGCNT);
			else
				update_ostring_value(po_sec, (sec = i), SCRPLT_SEC_DIGCNT);
		}

		// Only update score when it's needed
		if (update_status_box)
		{
			update_ostring_value(po_tm0, compute_score(0), 1);
			update_ostring_value(po_tm1, compute_score(1), 1);
			update_status_box = 0;
		}			
		is_low_res = was_low_res;
		sleep(1);
	}
	delobj(status_box_obj);
	delobj(status_box2_obj);
	delobj(status_box3_obj);
}

/******************************************************************************
** plyrinfo_proc() - Top-level player info process
*/
static void plyrinfo_proc(int *parg)
{
	ostring_t * cp_name[MAX_PLYRS];
	int free_play = coin_check_freeplay();
	int	old_start = check_credits_to_start();
	int msg_timer = 0;
	int msg_cycle = 0;
	int last_foul_cnt[MAX_PLYRS] = {-1,-1,-1,-1};
	int qtr_last = -1;
	int was_low_res = is_low_res;
	int i, i2, k;

	i = MAX_PLYRS;
	while (i--)
	{
		if (selected_players[i].record_type == CREATP_RECORD)
		{
			set_string_id(0xfeedface);
			set_text_font(FONTID_BAST8TO);
			set_text_justification_mode(HJUST_CENTER|VJUST_TOP);
			set_text_color(LT_YELLOW);
			set_text_position(
				pintx_turbo[i][four_plr_ver],
				pinty_turbo[1][0] + (is_low_res ? 6 : 6),
				10.0f + i);

			set_text_z_inc(-0.01f);

			if (is_low_res) is_low_res = 5;

			cp_name[i] = oprintf("%s", selected_players[i].player_data->szlast);
			hide_string(cp_name[i]);

			set_text_z_inc(0.0f);

			is_low_res = was_low_res;
		}
		else
		{
			cp_name[i] = NULL;
		}
	}

	// Top of proc loop
	while (1)
	{
		pdata_t			*ppb = pdata_blocks;
		image_info_t	*pii = ii_turbo;
		image_info_t	*pii_tmsg;
		image_info_t	**ii_pp;

		float	f,g;
		int		can_start = check_credits_to_start();

		// Reset cycle if start status changes
		if (old_start != can_start)
		{
			old_start = can_start;
			msg_timer = 0;
			msg_cycle = 0;
		}
		// Reset cycle at top of every qtr
		if (qtr_last != game_info.game_quarter)
		{
			qtr_last  = game_info.game_quarter;
			msg_timer = 0;
			msg_cycle = 0;
		}
		
		i = 0;
		do
		{
			// Set index into <player_blocks>
			if ((k = i) >= (MAX_PLYRS/2)) k += ((NUM_PLAYERS-MAX_PLYRS)/2);

			// Chk if player active
			if (p_status & (1 << i))
			{
				// Player active
				// Was, but must be an init'd player
				if (player_blocks[k].p_spd)
				{
					// Chk if player was a drone
					if ((ppb->is_human == 0) ||
						(ppb->si_msg1->ii != player_blocks[k].p_spd->pii_name))
					{
						// Show player name
					//	if (was_low_res) is_low_res = 3;
						change_img_tmu(ppb->si_msg1,
							player_blocks[k].p_spd->pii_name,
							SCR_PLATE_TID);
					//	is_low_res = was_low_res;

						// Hide second msg
						hide_sprite(ppb->si_msg2);

						// Kill any message sprites & show turbo bar
//						if (ppb->credits_show)
//						{
//							CREDSHOW_KILL(i);
//							ppb->credits_show = 0;
//						}

						// Make flag human
						ppb->is_human = 1;
					}
				}
				// Do the show/hide thing
				if (showhide_flag == SHOW)
				{
					unhide_sprite(ppb->si_frame);
					unhide_sprite(ppb->si_frameglw);
					unhide_sprite(ppb->si_turbo);
					if (cp_name[i])
					{
						unhide_string(cp_name[i]);
						hide_sprite(ppb->si_msg1);
					}
					else
						unhide_sprite(ppb->si_msg1);
					if (!pup_no_fouls)
					{
						unhide_sprite(ppb->si_foul_frm);
						unhide_sprite(ppb->si_foul_num);
					}

					// player fire flame
					if ((game_info.fire_player == i && game_info.fire_cnt >= FIRECNT) ||
						(game_info.team_fire == player_blocks[i].team))
					{
						unhide_sprite(ppb->si_fframe);

//						ii_pp = trb_bar_fire_imgs;
//						if (game_info.team_fire == player_blocks[i].team) {		// Team fire?
//							ii_pp = trb_bar_bfire_imgs;
//						}
//						change_img_tmu (ppb->si_fframe, ii_pp[ppb->fframe], SCR_PLATE_TID);

//						if (game_info.team_fire == player_blocks[i].team) {
//							i2 = 0xff8080ff;	// Blue
//							ppb->si_fframe->state.color_combiner_function = GR_COLORCOMBINE_CCRGB;
//						}
//						else {
//							i2 = 0xffffffff;	// White
//							ppb->si_fframe->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
//						}
//						ppb->si_fframe->state.constant_color = i2;
					}
					else
						hide_sprite(ppb->si_fframe);
				}
				else
				{
					hide_sprite(ppb->si_frame);
					hide_sprite(ppb->si_frameglw);
					hide_sprite(ppb->si_turbo);
					hide_sprite(ppb->si_msg1);
					hide_sprite(ppb->si_fframe);
					if (cp_name[i])
						hide_string(cp_name[i]);
					if (!pup_no_fouls)
					{
						hide_sprite(ppb->si_foul_frm);
						hide_sprite(ppb->si_foul_num);
					}
				}

				// Chk turbo button action
				if ((game_info.plyr_control[k] >= 0) &&
					((game_info.game_mode == GM_IN_PLAY) || (game_info.game_mode == GM_INBOUNDING)) &&
					(!(pup_infturbo & (1<<i))))
				{
					if (
						(get_but_val_cur(i) & C_BUTTON)
						&& !(game_info.fire_player == i && game_info.fire_cnt >= FIRECNT)
						&& !(game_info.team_fire == player_blocks[i].team)
						&& (game_info.game_mode != GM_INBOUNDING)
						)
					{
						ppb->turbo -= 0.30f;				// .35f , .4
						if (ppb->turbo < 0)
							ppb->turbo = 0;
					}
					else
					{
						// Replenish turbo (faster on in-bound)
						if((ppb->turbo += 0.8f) > 100.0f)	// .7
						{
							ppb->turbo = 100.0f;
						}
					}
				}
				// Chk if turbo bar needs updating
				if ((f = ppb->turbo) != ppb->turbo_last)
				{
					ppb->turbo_last = f;
					pii->w = g = ppb->turbo_w * f * (1.0f/100.0f);
					pii->s_end = pii->s_start + g * (1.0f/256.0f);	// !!! Watch This !!! 128

					// Only update verts if turbo bar is showing
					if (showhide_flag == SHOW)
					{
						generate_sprite_verts(ppb->si_turbo);
					}
				}

				// if team (player) on fire... animate fire behind turbo bar!
				if (((game_info.fire_player == i) && (game_info.fire_cnt >= FIRECNT)) ||
					((game_info.team_fire == player_blocks[i].team)))
				{
					if (--ppb->fdelay <= 0)
					{
						// change fire frame
						ppb->fdelay = FIRE_FDELAY;
						if (++ppb->fframe >= FIRE_FRAMES)
							ppb->fframe = 0;

						ii_pp = trb_bar_fire_imgs;
						if (game_info.team_fire == player_blocks[i].team) {		// Team fire?
							ii_pp = trb_bar_bfire_imgs;
						}
//						ppb->si_fframe->ii = ii_pp[ppb->fframe];

						change_img_tmu (ppb->si_fframe, ii_pp[ppb->fframe], SCR_PLATE_TID);
//						generate_sprite_verts(ppb->si_fframe);
					}
				}

				if (last_foul_cnt[i] != game_info.foul_cnt[i])
				{
					last_foul_cnt[i] = game_info.foul_cnt[i];
					change_img_tmu(ppb->si_foul_num,foul_num_imgs[game_info.foul_cnt[i]],SCR_PLATE_TID);
				}

				// Show ball behind turbo bar area if I'm ball owner or I just scored (ball flashes upon score)
				// - unless on fire/tm fire
				if ((game_info.ball_handler == i || (game_info.sc_scored == i && ball_flash_timer != 0)) &&
					(showhide_flag == SHOW) &&
					!((game_info.fire_player == i && game_info.fire_cnt >= FIRECNT) || (game_info.team_fire == player_blocks[i].team)))
				{
//					ppb->alpha_level += ppb->delta;
//					if (ppb->alpha_level < 128)
//					{
//						ppb->alpha_level = 128;
//						ppb->delta = +8;
//					}
//					else if (ppb->alpha_level > 255)
//					{
//						ppb->alpha_level = 255;
//						ppb->delta = -8;
//					}
//					ppb->si_frameglw->state.constant_color &= 0x00FFFFFF;
//					ppb->si_frameglw->state.constant_color |= (ppb->alpha_level << 24);
					unhide_sprite(ppb->si_frameglw);
				}
				else
					hide_sprite(ppb->si_frameglw);
					
//				else
//					ppb->si_msg1->state.constant_color = LT_YELLOW;
			}
			else
			{
				// Player inactive
				// Chk if player was a human
				if (ppb->is_human)
				{
					// Was; hide turbo bar
					hide_sprite(ppb->si_frame);
					hide_sprite(ppb->si_frameglw);
					hide_sprite(ppb->si_turbo);
					if (!pup_no_fouls)
					{
						hide_sprite(ppb->si_foul_frm);
						hide_sprite(ppb->si_foul_num);
					}

					// Kill any message sprites & show turbo bar
//					if (ppb->credits_show)
//					{
//						CREDSHOW_KILL(i);
//						ppb->credits_show = 0;
//					}

					// Make flag drone
					ppb->is_human = 0;
				}
				// Time to update msgs?
				if (!msg_timer && !(msg_cycle & 1))
				{
					// Yes; which way?
					if (game_info.game_state == GS_GAME_MODE && ppb->please_wait)
						// Please wait
						pii_tmsg = &tb_please;
					else
					if (!(msg_cycle % (2 <<1)))
						// Press/Insert
						pii_tmsg = (can_start || free_play) ? &tb_press : &tb_insert;
					else
					{
						// To control/Free (Credits?)/Press/Insert
						if (game_info.game_state == GS_GAME_MODE)
						{
							pii_tmsg = &tb_tocontr;

							// Set player name if init'd
							if (player_blocks[k].p_spd)
							{
							//	if (was_low_res) is_low_res = 3;
								change_img_tmu(ppb->si_msg2,
									player_blocks[k].p_spd->pii_name,
									SCR_PLATE_TID);
							//	is_low_res = was_low_res;
							}
						}
						else
						if (free_play)
							pii_tmsg = &tb_free;
						else
							pii_tmsg = (can_start) ? &tb_press : &tb_insert;
					}
				//	if (was_low_res) is_low_res = 3;
					change_img_tmu(ppb->si_msg1,
						pii_tmsg,
						SCR_PLATE_TID);
				//	is_low_res = was_low_res;
				}
				// Do the show/hide thing
				if (showhide_flag == SHOW && !(msg_cycle & 1))
				{
					unhide_sprite(ppb->si_msg1);
					if (ppb->si_msg1->ii == &tb_tocontr)
					{
						if (cp_name[i])
						{
							unhide_string(cp_name[i]);
							hide_sprite(ppb->si_msg2);
						}
						else
							unhide_sprite(ppb->si_msg2);
					}
					else
						hide_sprite(ppb->si_msg2);
				}
				else
				{
					hide_sprite(ppb->si_msg1);
					hide_sprite(ppb->si_msg2);
					if (cp_name[i])
						hide_string(cp_name[i]);
				}


				// Show ball behind turbo bar area if I'm ball owner or I just scored (ball flashes upon score)
				// - unless on fire/tm fire
				if ((game_info.ball_handler == i || (game_info.sc_scored == i && ball_flash_timer != 0)) &&
					(showhide_flag == SHOW) &&
					!((game_info.fire_player == i && game_info.fire_cnt >= FIRECNT) || (game_info.team_fire == player_blocks[i].team)))
				{
					unhide_sprite(ppb->si_frameglw);
				}
				else
					hide_sprite(ppb->si_frameglw);
					

				// Set drone-constants for turbo bar fire & text color
				hide_sprite(ppb->si_fframe);
				ppb->si_msg1->state.constant_color = LT_YELLOW;
			}
		} while (++ppb, ++pii, ++i < MAX_PLYRS);

		sleep(1);

		// Chk if time to cycle msg (off : on)
		if (++msg_timer == ((msg_cycle & 1) ? 10 : 50))
		{
			msg_timer = 0;
			msg_cycle++;
		}
	}
}

/******************************************************************************
** plyrinfo_init() - Starts player info bars
*/
enum {
	AI_ARR = 0,
	AI_NUM,
	AI_CPU,
	AI_TOTAL
};
//
// plyr# is indexed with <game_info.plyr_control[]>
//
image_info_t *arrow_imgs[][AI_TOTAL][MAX_PLYRS+1] = {		//[2p,4p][obj#][plyr#]
	{
		{&larrow_silver, &larrow_blue,   &larrow_silver, &larrow_red,    &larrow_silver},
		{&lballs_cpu,    &lball1s,       &lball1s,       &lball2sr,      &lball2sr},
		{&lballs_cpu,    &lball1s,       &lball1s,       &lball2sr,      &lball2sr},
	},
	{
		{&larrow_silver, &larrow_blue,   &larrow_green,  &larrow_red,    &larrow_yellow},
		{&lballs_cpu,    &lball1s,       &lball2s,       &lball3s,       &lball4s},
		{&lballs_cpu,    &lball1s,       &lball2s,       &lball3s,       &lball4s},
	}
};

void plyrinfo_init(void)
{
	register float fz;
	register int i;
	int was_low_res = is_low_res;

//	// Make sure you put this back before sleeping!!!
//	is_low_res = 0;

	fz = 2.0f;
	i = 0;
	do {
		register pdata_t *		ppb = &pdata_blocks[i];
		register image_info_t *	pii = &ii_turbo[i];

		// misc inits; probably dispose of or at least move
		ppb->turbo = 100.0f;				// should(?) be 0% to 100% left
		ppb->turbo_last = -1.0f;			// ensure turbo bar update

		// Make turbo & message sprites
	//	if (four_plr_ver)
	//	{
			ppb->si_frameglw = beginobj(&playball,
						pintx_turbo[i][four_plr_ver]-1.0f, pinty_turbo[0][0]+30.0f, 15.0f + i, SCR_PLATE_TID);

			if (pup_no_fouls)
			{
				ppb->si_frame = beginobj( ((i==0 || i==1) ? &trbobar_l : &trbobar_r),
						pintx_turbo[i][four_plr_ver]-1.0f, pinty_turbo[0][0],       10.3f + i, SCR_PLATE_TID);
			}
			else
			{
				ppb->si_frame = beginobj(pii_no_ft_box[i][!!was_low_res],
						pintx_turbo[i][four_plr_ver]-1.0f, pinty_turbo[0][0],       10.3f + i, SCR_PLATE_TID);
			}
	//	}
	//	else
	//	{
	//		ppb->si_frameglw = beginobj(&playball,
	//					pintx_turbo[i][four_plr_ver]-1.0f, pinty_turbo[0][0]+30.0f, 15.0f + i, SCR_PLATE_TID);
	//
	//		if (pup_no_fouls)
	//		{
	//			ppb->si_frame = beginobj( ((i==0) ? &trbobar_l : &trbobar_r),
	//					pintx_turbo[i][four_plr_ver]-1.0f, pinty_turbo[0][0],       10.3f + i, SCR_PLATE_TID);
	//		}
	//		else
	//		{
	//			ppb->si_frame = beginobj(pii_no_ft_box[i][!!was_low_res],
	//					pintx_turbo[i][four_plr_ver]-1.0f, pinty_turbo[0][0],       10.3f + i, SCR_PLATE_TID);
	//		}
	//	}

		ppb->si_turbo = beginobj(pii_turbo[i][!!was_low_res],
						pintx_turbo[i][four_plr_ver]-1.0f, pinty_turbo[0][0],       10.1f + i, SCR_PLATE_TID);
		if (i > 1)
		{
			ppb->si_turbo->mode = FLIP_TEX_H;
			generate_sprite_verts(ppb->si_turbo);
		}

	//	if (was_low_res) is_low_res = 3;
		ppb->si_msg1 = beginobj(&tb_insert,
						pintx_turbo[i][four_plr_ver],      pinty_turbo[1][0],       10.0f + i, SCR_PLATE_TID);

		ppb->si_msg2 = beginobj(&tb_insert,
						pintx_turbo[i][four_plr_ver],      pinty_turbo[2][0],        9.9f + i, SCR_PLATE_TID);
	//	is_low_res = was_low_res;

		// foul meter box
		ppb->si_foul_frm = beginobj(pii_ft_box[i][!!was_low_res],
						pintx_turbo[i][four_plr_ver]-1.0f, pinty_turbo[0][0],        6.0f + i, SCR_PLATE_TID);
			
		// foul meter count
		ppb->si_foul_num = beginobj(foul_num_imgs[0],
			pintx_turbo[i][four_plr_ver]+((i > 1) ? -39.0f : 43.0f), pinty_turbo[2][0]-5.0f, 2.0f + i, SCR_PLATE_TID);

		// make FIRE images under turbo bar
		ppb->fframe = 0;											// img nbr.
		ppb->fdelay = 0;		// FIRE_FDELAY;
		ppb->si_fframe = beginobj(trb_bar_fire_imgs[ppb->fframe],
			pintx_turbo[i][four_plr_ver]+3.0f, pinty_turbo[0][0] + 6.0f , 11.5f + i, SCR_PLATE_TID);
		ppb->si_fframe->w_scale = 1.7f;
		ppb->si_fframe->h_scale = 1.7f;
		generate_sprite_verts(ppb->si_fframe);

		ppb->si_frame->id = OID_TURBARROW;
		ppb->si_frameglw->id = OID_TURBARROW;
		ppb->si_turbo->id = OID_TURBARROW;
		ppb->si_msg1->id  = OID_TURBARROW;
		ppb->si_msg2->id  = OID_TURBARROW;
		ppb->si_fframe->id = OID_TURBARROW;
		ppb->si_foul_frm->id = OID_TURBARROW;
		ppb->si_foul_num->id = OID_TURBARROW;

		ppb->si_msg1->state.constant_color = LT_YELLOW;
		ppb->si_msg2->state.constant_color = LT_YELLOW;
		ppb->si_msg1->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
		ppb->si_msg2->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;

		// Assume drone
		ppb->is_human = 0;

		// for glow effect
		//ppb->alpha_level = 255;
		//ppb->delta = -8;

		// create & substitute the dynamic image info
		*pii = *pii_turbo[i][!!was_low_res];
		ppb->si_turbo->ii = pii;
		ppb->turbo_w = pii->w;

		// assume need to be hidden
		hide_sprite(ppb->si_frame);
		hide_sprite(ppb->si_frameglw);
		hide_sprite(ppb->si_turbo);
		hide_sprite(ppb->si_msg1);
		hide_sprite(ppb->si_msg2);
		hide_sprite(ppb->si_fframe);
		hide_sprite(ppb->si_foul_frm);
		hide_sprite(ppb->si_foul_num);

	} while (++i < MAX_PLYRS);

	// Back it goes!!!
	is_low_res = was_low_res;

	// Make the arrow objs
	i = 0;
	do {
		arrowinfo[i].num_obj = beginobj(arrow_imgs[four_plr_ver][AI_NUM][0], 0.0f, 0.0f, fz, SCR_PLATE_TID);
		//ii_num[i] = *((sprite_info_t *)arrowinfo[i].num_obj)->ii;
		//((sprite_info_t *)arrowinfo[i].num_obj)->ii = &ii_num[i];
		((sprite_info_t *)arrowinfo[i].num_obj)->id = OID_TURBARROW;
		//arrowinfo[i].num_ax = ii_num[i].ax;
		//arrowinfo[i].num_ay = ii_num[i].ay;
		fz += 0.1f; 

		arrowinfo[i].arr_obj = beginobj(arrow_imgs[four_plr_ver][AI_ARR][0], 0.0f, 0.0f, fz, SCR_PLATE_TID);
		//ii_arr[i] = *((sprite_info_t *)arrowinfo[i].arr_obj)->ii;
		//((sprite_info_t *)arrowinfo[i].arr_obj)->ii = &ii_arr[i];
		((sprite_info_t *)arrowinfo[i].arr_obj)->id = OID_TURBARROW;
		//arrowinfo[i].arr_ax = ii_arr[i].ax;
		//arrowinfo[i].arr_ay = ii_arr[i].ay;
		fz += 0.1f; 

		arrowinfo[i].show = -1;
		arrowinfo[i].ai_flags = 0;

	} while (++i < NUM_PLAYERS);

#if NUM_PERTEAM > 2
	rcvra_obj = beginobj(&balla, 0.0f, 0.0f, 1.00f, SCR_PLATE_TID);
	rcvra_obj->id = OID_TURBARROW;
	rcvrb_obj = beginobj(&ballb, 0.0f, 0.0f, 1.01f, SCR_PLATE_TID);
	rcvrb_obj->id = OID_TURBARROW;
//	pass_word_obj = beginobj(&lpass_word, 0.0f, 0.0f, 1.0f, SCR_PLATE_TID);
//	pass_word_obj->id = OID_TURBARROW;
#endif

	// Start the main line procs
	qcreate( "plrinfo", PLYRINFO_PID, plyrinfo_proc, 0, 0, 0, 0 );
	set_process_run_level( qcreate( "parrows", PLYRINFO_PID, update_arrows_proc, 0, 0, 0, 0 ), RL_ARROWS);
}

/******************************************************************************
** clip_line_to_screen()
** Note: <endpts> needs to contain 2 pair of points
** Returns: 1st pair clipped, 2nd not
*/
float clip_line_to_screen(VERTEX * verts, POINT * endpts, float xbord, float ybord, float * trig)
{
	struct { float x, y, t, b, l, r; } d[2];
	VERTEX v[2];
	float sin, cos, len;
	int k;

	v[0] = verts[0];
	v[1] = verts[1];

	// Pull in if behind the camera
	if (v[0].z < 1.0f) {
		float f = (1.0f - v[0].z) / (v[1].z - v[0].z);
		v[0].x = v[0].x + (v[1].x - v[0].x) * f;
		v[0].y = v[0].y + (v[1].y - v[0].y) * f;
		v[0].z = 1.0f;
	}
	if (v[1].z < 1.0f) {
		float f = (1.0f - v[1].z) / (v[0].z - v[1].z);
		v[1].x = v[1].x + (v[0].x - v[1].x) * f;
		v[1].y = v[1].y + (v[0].y - v[1].y) * f;
		v[1].z = 1.0f;
	}

	// Project screen coords
	endpts[0].x = endpts[2].x = (HRESf*0.5f) - (HRESf*fovfac) * v[0].x / v[0].z;
	endpts[0].y = endpts[2].y = (VRESf*0.5f) + (HRESf*fovfac) * v[0].y / v[0].z;

	endpts[1].x = endpts[3].x = (HRESf*0.5f) - (HRESf*fovfac) * v[1].x / v[1].z;
	endpts[1].y = endpts[3].y = (VRESf*0.5f) + (HRESf*fovfac) * v[1].y / v[1].z;

	sin = endpts[1].x - endpts[0].x;
	cos = endpts[1].y - endpts[0].y;
	len = sin * sin + cos * cos;

	if (len >= 1.0f)
	{
		len  = sqrt(len);
		sin /= len;
		cos /= len;
		if (sin < -1.0f) sin = -1.0f;
		if (sin >  1.0f) sin =  1.0f;
		if (cos < -1.0f) cos = -1.0f;
		if (cos >  1.0f) cos =  1.0f;
		trig[0] = sin;
		trig[1] = cos;

		if ((d[0].x = d[0].l = 0.0f  + xbord - endpts[0].x) < 0.0f) d[0].l = 0.0f;
		if ((         d[0].r = HRESf - xbord - endpts[0].x) > 0.0f) d[0].r = 0.0f;
		if ((d[0].y = d[0].b = 0.0f  + ybord - endpts[0].y) < 0.0f) d[0].b = 0.0f;
		if ((         d[0].t = VRESf - ybord - endpts[0].y) > 0.0f) d[0].t = 0.0f;
		if ((d[1].x = d[1].l = 0.0f  + xbord - endpts[1].x) < 0.0f) d[1].l = 0.0f;
		if ((         d[1].r = HRESf - xbord - endpts[1].x) > 0.0f) d[1].r = 0.0f;
		if ((d[1].y = d[1].b = 0.0f  + ybord - endpts[1].y) < 0.0f) d[1].b = 0.0f;
		if ((         d[1].t = VRESf - ybord - endpts[1].y) > 0.0f) d[1].t = 0.0f;

		// Trivial reject
		if (!((d[0].l > 0.0f && d[1].l > 0.0f) || (d[0].r < 0.0f && d[1].r < 0.0f) ||
			  (d[0].b > 0.0f && d[1].b > 0.0f) || (d[0].t < 0.0f && d[1].t < 0.0f)))
		{
			for (k = 0; k < 2; k++) {
				if (d[k].l > 0.0f && d[k].b > 0.0f) {
					if ((d[k].x = (d[k].b * sin) / cos) < d[k].l) d[k].x = d[k].l;
					if ((d[k].y = (d[k].l * cos) / sin) < d[k].b) d[k].y = d[k].b;
				}
				else
				if (d[k].l > 0.0f && d[k].t < 0.0f) {
					if ((d[k].x = (d[k].t * sin) / cos) < d[k].l) d[k].x = d[k].l;
					if ((d[k].y = (d[k].l * cos) / sin) > d[k].t) d[k].y = d[k].t;
				}
				else
				if (d[k].r < 0.0f && d[k].t < 0.0f) {
					if ((d[k].x = (d[k].t * sin) / cos) > d[k].r) d[k].x = d[k].r;
					if ((d[k].y = (d[k].r * cos) / sin) > d[k].t) d[k].y = d[k].t;
				}
				else
				if (d[k].r < 0.0f && d[k].b > 0.0f) {
					if ((d[k].x = (d[k].b * sin) / cos) > d[k].r) d[k].x = d[k].r;
					if ((d[k].y = (d[k].r * cos) / sin) < d[k].b) d[k].y = d[k].b;
				}
				else
				if (d[k].l > 0.0f) {
					float f = (d[k].l * cos) / sin;
					if ((endpts[k].y + f) < ybord || (endpts[k].y + f) > (VRESf - ybord)) goto reject;
					d[k].x = d[k].l;
					d[k].y = f;
				}
				else
				if (d[k].r < 0.0f) {
					float f = (d[k].r * cos) / sin;
					if ((endpts[k].y + f) < ybord || (endpts[k].y + f) > (VRESf - ybord)) goto reject;
					d[k].x = d[k].r;
					d[k].y = f;
				}
				else
				if (d[k].b > 0.0f) {
					float f = (d[k].b * sin) / cos;
					if ((endpts[k].x + f) < xbord || (endpts[k].x + f) > (HRESf - xbord)) goto reject;
					d[k].x = f;
					d[k].y = d[k].b;
				}
				else
				if (d[k].t < 0.0f) {
					float f = (d[k].t * sin) / cos;
					if ((endpts[k].x + f) < xbord || (endpts[k].x + f) > (HRESf - xbord)) goto reject;
					d[k].x = f;
					d[k].y = d[k].t;
				}
				else {
					d[k].x = 0.0f;
					d[k].y = 0.0f;
				}
			}
			endpts[0].x += d[0].x;
			endpts[0].y += d[0].y;
			endpts[1].x += d[1].x;
			endpts[1].y += d[1].y;

			return sqrt((endpts[1].x - endpts[0].x) * (endpts[1].x - endpts[0].x) +
						(endpts[1].y - endpts[0].y) * (endpts[1].y - endpts[0].y));
		}
reject:
		return 0.0f;						// valid but rejected
	}
	return -1.0f;							// invalid
}

/******************************************************************************
** update_arrows_proc() - Update player off-screen arrows
*/
#define P_NUMHIDE		(0.88f)
#define P_ARRHIDE		(0.99f)//(0.94f)
#define P_ARRSHOW		(1.02f)//(0.99f)
#define P_ARRMOVE		(0.10f)

#define CNT_NUMHIDE_Q1	(tsec * 1)
#define CNT_NUMHIDE		(tsec * 1)
#define CNT_ARRHIDE_Q1	(tsec * 1)
#define CNT_ARRHIDE		(tsec * 1)

#define CNT_ARRCYCLE	(4)
#define CNT_ARRFLASH	(CNT_ARRCYCLE * (1+1) * 3)

#define YFAC_ARR		(IN2UN * 0.70f)
#define YFAC_ICO		(IN2UN * 0.80f)

#define XBORD			(30)
#define YBORD			(-10)

#define SK				((VRESf/2.0f)/HRESf)

static void update_arrows_proc(int *parg)
{
	int arr_flash[NUM_PLAYERS];
	int gstart = 0;

	// Wait for other guys to start playing
	sleep(6);


	// Loop here till dead
	while (1)
	{
		ARROWINFO * pai = &arrowinfo[0];
		sprite_info_t * arr_obj;
		sprite_info_t * num_obj;
		bbplyr_data * ppdata;
		float psx,psy,psz,sx,sy;
		int receiver;
		int show, cnt, i, j;

#ifdef D_ARROW
		static int onoff = 0;
		static int osw = 0;
		int csw = get_player_sw_current();
		if (((csw ^ osw) & csw) & P4_D) onoff = ~onoff;
		osw = csw;

		delete_multiple_strings(0xbeadfeed, -1);
		del1c(0xbeadfeed, -1);
#endif

		// Chk if time to reset flash timers
		if (game_info.game_mode == GM_INITIALIZING ||
			game_info.game_state == GS_EOQ ||
			gstart == 0)
		{
			int * p = arr_flash + NUM_PLAYERS;
			while (p-- != arr_flash)
				*p = 0;

			gstart = 1;
		}

		// Set timer cnt per mode
		cnt = 0;

#if NUM_PERTEAM > 2
		// Always start with this hidden
		hide_sprite(rcvra_obj);
		hide_sprite(rcvrb_obj);
//		hide_sprite(pass_word_obj);
#endif

		// OK so far, go ahead and do'em
		i = 0;
		do
		{
			arr_obj = pai->arr_obj;
			num_obj = pai->num_obj;
			ppdata  = pai->ppdata;

			// Ensure arrow img synced with human/cpu
			if ((j = game_info.plyr_control[i]) >= 0)
				 j = ppdata->plyrnum;
			change_img_tmu(arr_obj, arrow_imgs[four_plr_ver][AI_ARR][j+1], SCR_PLATE_TID);

			// We wanna hide'em (assumed)
			show = -1;

			// Set active receiver flag
			receiver = 0;
#if NUM_PERTEAM > 2
			if (game_info.ball_handler >= 0)
			{
				int t_mask = TEAM_MASK(ppdata->team);
				int t_stat = p_status & t_mask;
				// Disallow receiver icons if its a drone team or
				//  if drone has the ball on a 2 human team
				if (t_stat
					&& (!four_plr_ver ||
						game_info.plyr_control[game_info.ball_handler] >= 0 ||
						t_stat != t_mask)
					)
				{
					if (game_info.receiver_a == ppdata->plyrnum) receiver = 1;
					if (game_info.receiver_b == ppdata->plyrnum) receiver = 2;
				}
			}
#endif
			// Do the timer-stuff
			if (!gstart)
				pai->timer = 1;
			else if (pai->timer < cnt)
				pai->timer = cnt;
			else if (pai->timer)
				pai->timer--;

			if (pai->timer < 0 || cnt < 0)
				pai->timer = 0;

			// Chk if arrows can even be shown
			if (showhide_flag == SHOW &&
				hide_arrows_one_tick == SHOW &&
				game_info.game_state != GS_ATTRACT_MODE)
			{
#ifdef D_ARROW
				// Plot the grid if enabled
				if (onoff) {
					static image_info_t ii = {NULL, 1, 1, 0, 0, 0, 1, 0, 1};
					static float a = 0;

					sprite_info_t * psi;
					VERTEX v[2];
					POINT p[4];
					float trig[2];
					float ang, len;
					int j;

					set_string_id(0xbeadfeed);
					set_text_transparency_mode(TRANSPARENCY_ENABLE);
					set_text_justification_mode(HJUST_RIGHT|VJUST_BOTTOM);
					set_text_font(FONTID_NBA10);
					set_text_z_inc(0.01f);

					set_text_color(plr_color[i]);
					set_text_position(80, (3-i) * 30 + 20, 1.2f);
					oprintf("%10.4f", ((pai->plyr_sx * (fovfac2x)) / pai->plyr_sz) );
					set_text_position(80, (3-i) * 30 + 7, 1.2f);
					oprintf("%10.4f", ((pai->plyr_sy * (fovfac2x*HRESf/VRESf)) / pai->plyr_sz) );

					set_text_z_inc(0.00f);

					for (j = 0; j < 2; j++) {
						// Do plyr X/Z-median endpts
						psy = -cambot.y + ppdata->odata.y + ppdata->p_spd->tall * YFAC_ARR;

						psx = -cambot.x + (j ? ppdata->odata.x : -COURT_L);
						psz = -cambot.z + (j ? -COURT_W : ppdata->odata.z);

						v[0].x = (psx * cambot.cos_theta - psz    * cambot.sin_theta);
						v[0].z = (psx * cambot.sin_theta + psz    * cambot.cos_theta);
						v[0].y = (psy * cambot.cos_phi   - v[0].z * cambot.sin_phi);
						v[0].z = (psy * cambot.sin_phi   + v[0].z * cambot.cos_phi);

						psx = -cambot.x + (j ? ppdata->odata.x : +COURT_L);
						psz = -cambot.z + (j ? +COURT_W : ppdata->odata.z);

						v[1].x = (psx * cambot.cos_theta - psz    * cambot.sin_theta);
						v[1].z = (psx * cambot.sin_theta + psz    * cambot.cos_theta);
						v[1].y = (psy * cambot.cos_phi   - v[1].z * cambot.sin_phi);
						v[1].z = (psy * cambot.sin_phi   + v[1].z * cambot.cos_phi);

						len = clip_line_to_screen(v, p, -32, -32, trig);
						if (!len) continue;

						ang = acos(trig[1]);
						if (trig[0] < 0.0f) ang = -ang;

						psi = beginobj(&ii, p[0].x, p[0].y, 1, 0);
						psi->id = 0xbeadfeed;
						psi->h_scale = 6;
						psi->z_angle += a;
						psi->state.constant_color = LT_MAGNETA;
						generate_sprite_verts(psi);

						psi = beginobj(&ii, p[1].x, p[1].y, 1, 0);
						psi->id = 0xbeadfeed;
						psi->h_scale = 6;
						psi->z_angle += a;
						psi->state.constant_color = LT_GREEN;
						generate_sprite_verts(psi);

						psi = beginobj(&ii, p[0].x, p[0].y, 1, 0);
						psi->id = 0xbeadfeed;
						psi->h_scale = len;
						psi->z_angle += ang;
						psi->state.constant_color = plr_color[i];
						generate_sprite_verts(psi);
					}
					a += DG2RD(2);
				}
#endif // D_ARROW

				// Set positioning data for plyr off-screen arrow
				sy  = -ppdata->odata.y + ppdata->p_spd->tall * YFAC_ARR;
				psx = pai->plyr_sx + cambot.xform[1] * sy;
				psy = pai->plyr_sy + cambot.xform[5] * sy;
				psz = pai->plyr_sz + cambot.xform[9] * sy;
				sx  = fabs(psx) * (fovfac2x);
				sy  = fabs(psy) * (fovfac2x*HRESf/VRESf);

				////////////////
				// Chk if not fully on-screen
				if ((psz * P_NUMHIDE) < sx ||
					(psz * P_NUMHIDE) < sy ||
					(psz * P_NUMHIDE) < 10.0f)
				{
					// Not fully; determine what to display if:
					//   active human and
					//     ball in play or
					//     ball inbounding or
					//     active timer
					// or
					//   active pass receiver
					//
					if ((//game_info.plyr_control[i] >= 0 &&
							(game_info.game_mode == GM_IN_PLAY ||
							 game_info.game_mode == GM_INBOUNDING ||
							 pai->timer)) ||
						receiver)
					{
						// Hide arrow if X, Y, & Z are inside the arrow-hide perimeter
						if ((show = (int)(
							(psz * P_ARRHIDE) < sx ||
							(psz * P_ARRHIDE) < sy ||
							(psz * P_ARRHIDE) < 10.0f)) == 0)
						{
							// Hide the arrow & set timer for when # goes away
							pai->ai_flags &= ~AF_OFFSCREENARROW;
							j = (game_info.game_quarter) ? CNT_NUMHIDE : CNT_NUMHIDE_Q1;
							if (pai->timer < j) pai->timer = j;
						}
						else
						{
							// Show arrow if X, Y, or Z are outside the arrow-show perimeter
							//   else leave arrow as it was, shown or hidden
							if ((psz * P_ARRSHOW) < sx ||
								(psz * P_ARRSHOW) < sy ||
								(psz * P_ARRSHOW) < 10.0f)
							{
								// Show the arrow
								pai->ai_flags |= AF_OFFSCREENARROW;
								if ((ppdata->odata.flags & (PF_DUNKER|PF_ALLYDUNKER|PF_LAYUP)) &&
									(ppdata->flight_time == 1) &&
									(game_info.plyr_control[i] >= 0))
								{
									arr_flash[i] = CNT_ARRFLASH;
								}
							}
							// Set timer for when # goes away
							j = (game_info.game_quarter) ? CNT_ARRHIDE : CNT_ARRHIDE_Q1;
							if (pai->timer < j) pai->timer = j;
						}
					}
				//	// Do special CPU processing
				//	if (game_info.plyr_control[i] < 0)
				//	{
				//		pai->timer = CNT_ARRHIDE;
				//		show = 0;
				//		if (!receiver)
				//			pai->ai_flags &= ~AF_OFFSCREENARROW;
				//	}
					// Ensure number img synced with human/cpu/condition
				//	if ((j = game_info.plyr_control[i]) >= 0)
				//		 j = ppdata->plyrnum;
				//	change_img_tmu(num_obj, arrow_imgs[four_plr_ver][AI_CPU][j+1], SCR_PLATE_TID);

					j = (game_info.plyr_control[i] >= 0) ? AI_NUM : AI_CPU;
					change_img_tmu(num_obj, arrow_imgs[four_plr_ver][j][ppdata->plyrnum+1], SCR_PLATE_TID);

				//	if (pai->pnum < 0)
				//		change_img_tmu(num_obj, arrow_imgs[four_plr_ver][AI_CPU][(pai->ai_flags & AF_OFFSCREENARROW) ? ppdata->plyrnum+1:0], SCR_PLATE_TID);
				//	else
				//		change_img_tmu(num_obj, arrow_imgs[four_plr_ver][AI_NUM][ppdata->plyrnum+1], SCR_PLATE_TID);
				}
				else
				{
					// Fully on; determine what to display if:
					//   active pass receiver or active timer
					//
					if (receiver || pai->timer)
					{
						// Hide the arrow
						pai->ai_flags &= ~AF_OFFSCREENARROW;

						// Chk if doing a special sequence
						if (ppdata->odata.flags & (PF_SHOOTING|PF_DUNKER|PF_ALLYDUNKER|PF_LAYUP))
							// We wanna hide'em
							pai->timer = 0;
						else
							// We wanna see'em
							show = 0;
					}
					// Ensure number img synced with human/cpu/condition
					change_img_tmu(num_obj, arrow_imgs[four_plr_ver][AI_NUM][ppdata->plyrnum+1], SCR_PLATE_TID);
				}

				////////////////
				// Chk if we need to do off-screen arrow positioning
				if (pai->ai_flags & AF_OFFSCREENARROW)
				{
					// Invalid X and/or Y; check Y
					if (sy > psz)
					{
						// Invalid Y, maybe X too; set Z limit
						if (psy > 0)
							// Top side; set maximum Z; <fabs> is to stop new <psz>
							//  from going negative (this would be bad); <psz> gets
							//  flaky near the top of the screen where the divisor
							//  tends to get a little small
							psz = fabs((psy * cambot.d) / (psy + SK * (cambot.d - psz)));
						else
							// Bottom side; set minimum Z; <fabs> is to stop new <psz>
							//  from going negative (this would be bad); <psz> gets
							//  flaky near the top of the screen where the divisor
							//  tends to get a little small
							psz = fabs((psy * cambot.d) / (psy - SK * (cambot.d - psz)));
					}

					// Do & check coordinate ranges to determine any arrow rotation
					psx /= psz;
					psy /= psz;

					if (fabs(psy) > fabs(psx))
					{
						// Bad Y only; not past left or right side
						arr_obj->z_angle = (sx = psx/psy) * DG2RD(45.0f);

						if (psy < 0)
						{
							// Off the bottom only
							arr_obj->x = (HRESf/2) - (VRESf/2-YBORD) * sx;
							arr_obj->y = (YBORD);
						//	arr_obj->z_angle += (0);
						}
						else
						{
							// Off the top only
							arr_obj->x = (HRESf/2) + (VRESf/2-YBORD) * sx;
							arr_obj->y = (VRESf-YBORD);
							arr_obj->z_angle += DG2RD(180.0f);
						}
					}
					else
					{
						// Bad X & maybe Y as well

						VERTEX v[2];
						POINT p[4];
						float trig[2];

						float  ang, len, rat, fb, fm;

						int i0, i1;

						float rx = -cambot.x - COURT_L;
						float ry = -cambot.y + ppdata->p_spd->tall * YFAC_ARR;
						float rz = -cambot.z + ppdata->odata.z;

						v[0].x = (rx * cambot.cos_theta - rz     * cambot.sin_theta);
						v[0].z = (rx * cambot.sin_theta + rz     * cambot.cos_theta);
						v[0].y = (ry * cambot.cos_phi   - v[0].z * cambot.sin_phi);
						v[0].z = (ry * cambot.sin_phi   + v[0].z * cambot.cos_phi);

						rx = -cambot.x + COURT_L;

						v[1].x = (rx * cambot.cos_theta - rz     * cambot.sin_theta);
						v[1].z = (rx * cambot.sin_theta + rz     * cambot.cos_theta);
						v[1].y = (ry * cambot.cos_phi   - v[1].z * cambot.sin_phi);
						v[1].z = (ry * cambot.sin_phi   + v[1].z * cambot.cos_phi);

						psx *= psz;			//!!! Don't forget these if prior
						psy *= psz;			//!!! "Do & check ..." changes

						i0 = !(psx >= 0);
						i1 =  (psx >= 0);

						len = clip_line_to_screen(v, p, XBORD, YBORD, trig);
						if (len >= 50.0f)
						{
							ang = acos(trig[1]) + DG2RD(180.0f) * (float)i0;
							if (trig[0] < 0.0f) ang = -ang;

							fm = (v[i1].z - v[i0].z) / (v[i1].x - v[i0].x);
							fb = v[i0].z - fm * v[i0].x;
							rx = fb / (fovfac2x - fm);

							// Do a delta-average if you need <ry>

							rz = rx * fovfac2x;

							// Correct for same-slope, opposite side
							if (i0) rx = -rx;

							fm =  sqrt( (psx - v[i0].x) * (psx - v[i0].x) +
							//			(psy - v[i0].y) * (psy - v[i0].y) +
										(psz - v[i0].z) * (psz - v[i0].z));
							fb = rsqrt( (rx  - v[i0].x) * (rx  - v[i0].x) +
							//			(ry  - v[i0].y) * (ry  - v[i0].y) +
										(rz  - v[i0].z) * (rz  - v[i0].z));

							if (ppdata->odata.x < -COURT_L || ppdata->odata.x >  COURT_L)
								rat = fm = 0.0f;
							else
								rat = fm * fb;

							arr_obj->z_angle = ang;

							arr_obj->x = p[i0].x + (p[i1].x - p[i0].x) * rat * P_ARRMOVE;
							arr_obj->y = p[i0].y + (p[i1].y - p[i0].y) * rat * P_ARRMOVE;
						}
						else
							show = -1;

/*						if (psx < 0)
						{
							// Off the left at least
							arr_obj->x = (XBORD);
							arr_obj->y = (VRESf/2) - (HRESf/2-XBORD) * sx;
							arr_obj->z_angle += DG2RD( 90.0f);
						}
						else
						{
							// Off the right at least
							arr_obj->x = (HRESf-XBORD);
							arr_obj->y = (VRESf/2) + (HRESf/2-XBORD) * sx;
							arr_obj->z_angle += DG2RD(270.0f);
						}

						if (arr_obj->y < (YBORD) || arr_obj->y > (VRESf-YBORD))
						{
							if (psy < 0 )
							{
								// Off the bottom as well
								arr_obj->x = (HRESf/2) - (VRESf/2-YBORD) * psx/psy;
								arr_obj->y = (YBORD);
							}
							else
							{
								// Off the top as well
								arr_obj->x = (HRESf/2) + (VRESf/2-YBORD) * psx/psy;
								arr_obj->y = (VRESf-YBORD);
							}
						}*/
					}
				}
				else
				// Chk if we need to do on-screen arrow positioning
				if (show >= 0)
				{
					// Chk for invalid Y
					if (sy > psz)
					{
						// Invalid; hide'em
						show = -1;
					}
					else
					{
						// Set positioning data for on-screen icon
						sy  = ppdata->p_spd->tall * YFAC_ICO;
						psx = pai->plyr_sx + cambot.xform[1] * sy;
						psy = pai->plyr_sy + cambot.xform[5] * sy;
						psz = pai->plyr_sz + cambot.xform[9] * sy;
						arr_obj->x = (HRESf*0.5f) - (HRESf*fovfac) * psx / psz;
						arr_obj->y = (VRESf*0.5f) + (HRESf*fovfac) * psy / psz;
					}
				}
			}

			////////////////
			// Hide'em or see'em?
			if (show < 0)
			{
				// Hide'em; local positioning #'s may not be valid here!
				hide_sprite(num_obj);
				hide_sprite(arr_obj);
#if NUM_PERTEAM > 2
				if (receiver)
					hide_sprite(receiver == 1 ? rcvra_obj : rcvrb_obj);
#endif
				continue;
			}

			// See'em & update positions
			if (pai->timer)
			{
				num_obj->x = arr_obj->x = (float)(int)arr_obj->x;
				num_obj->y = arr_obj->y = (float)(int)arr_obj->y;
				unhide_sprite(num_obj);
				generate_sprite_verts(num_obj);
			}
			else
			{
				hide_sprite(num_obj);
			}

			if (pai->ai_flags & AF_OFFSCREENARROW)
			{
				unhide_sprite(arr_obj);
				generate_sprite_verts(arr_obj);
			}
			else
			{
				hide_sprite(arr_obj);
			}

			if (arr_flash[i] > 0) arr_flash[i]--;
			if ((arr_flash[i] == 0) || (((arr_flash[i]/CNT_ARRCYCLE) & 1) == 0))
			{
				num_obj->state.color_combiner_function = GR_COLORCOMBINE_DECAL_TEXTURE;
				arr_obj->state.color_combiner_function = GR_COLORCOMBINE_DECAL_TEXTURE;
				num_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
				arr_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
			}
			else
			{
				num_obj->state.color_combiner_function = GR_COLORCOMBINE_ONE;
				arr_obj->state.color_combiner_function = GR_COLORCOMBINE_ONE;
				num_obj->state.chroma_key_mode = GR_CHROMAKEY_ENABLE;
				arr_obj->state.chroma_key_mode = GR_CHROMAKEY_ENABLE;
			}

#if NUM_PERTEAM > 2
			if (receiver)
			{
				sprite_info_t * pobj = (receiver == 1) ? rcvra_obj : rcvrb_obj;

				if (pai->ai_flags & AF_OFFSCREENARROW)
				{
					pobj->x = arr_obj->x;
					pobj->y = arr_obj->y;
				}
				else
				{
					sy = ppdata->p_spd->tall * IN2UN * 0.45f;
					psx += cambot.xform[1] * sy;
					psy += cambot.xform[5] * sy;
					psz += cambot.xform[9] * sy;
					pobj->x = (HRESf*0.5f) - (HRESf*fovfac) * psx / psz;
					pobj->y = (VRESf*0.5f) + (HRESf*fovfac) * psy / psz;
				}
				unhide_sprite(pobj);
				generate_sprite_verts(pobj);
			}
//			if (receiver)
//			{
//				pass_word_obj->x = arr_obj->x;
//				pass_word_obj->y = arr_obj->y;
//	if (pai->ai_flags & AF_OFFSCREENARROW)			//FIX!!!
//				unhide_sprite(pass_word_obj);
//	else											//FIX!!!
//	hide_sprite(pass_word_obj);						//FIX!!!
//				generate_sprite_verts(pass_word_obj);
//			}
#endif
		} while ((pai->show = show), ++pai, ++i < NUM_PLAYERS);

		// Unhide every tick
		hide_arrows_one_tick = SHOW;

		// Go let other guys play now
		sleep(1);
	}
}


/*****************************************************************************/
#define OFF_NAME_DY	(0.0f)
#define DEF_NAME_DY	(16.0f)

#define BUF_X	(6.0f)
#define BUF_Y	(2.0f)

#define KXFAC	((hres*0.5f)-(hres*0.5f)*fovfac)
#define KYFAC	((vres*0.5f)-(vres*0.5f)*fovfac)

void show_player_names_proc(int *parg)
{
	int was_low_res = is_low_res;

	if (1)//game_info.play_type != PT_KICKOFF && game_info.game_state != GS_ATTRACT_MODE)
	{
		sprite_info_t * names_obj[NUM_PLAYERS];
		char upd[NUM_PLAYERS];

		register sprite_info_t * psi;
		register sprite_info_t * psj;
		register float psx, psy;
		register int obtn, dbtn, team, i, j;

		//  This Needs to be here!
		//  Sync-up with <last_player> coordinate updating
		sleep(4);
#ifdef DEBUG
		printf("after SLEEP 4\n");
#endif
		i = NUM_PLAYERS;
		while (i--)
		{
			switch (player_blocks[i].position)
			{
				case GUARD:
				case FORWARD:
				case CENTER:
					// Get img *; null if isn't one
					if (((image_info_t *)psi = player_blocks[i].p_spd->pii_name))
					{

						// Make sure you put this back before sleeping!!!
						is_low_res = 0;

						psi = beginobj((image_info_t *)psi, 0, 0, 2.5f, SCR_PLATE_TID);
						psi->id = OID_TURBARROW;

						// Back it goes!!!
						is_low_res = was_low_res;

						psi->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
						psi->state.constant_color = LT_YELLOW;

						//if(player_blocks[i].plyrnum == game_info.ball_handler)
						hide_sprite(psi);
						//else
						//	unhide_sprite(psi);	

						// Flag a coordinate recalc
						upd[i] = -1;
					}
					names_obj[i] = psi;
					break;

				default:
					names_obj[i] = NULL;
					break;
			}
		}

		// Loop here till dead
		while (1)//game_info.game_mode != GM_IN_PLAY)
		{
			// Get button states to hide/unhide names
			if (game_info.off_side)
			{
				obtn = P2_C;
				dbtn = P1_C;
			}
			else
			{
				obtn = P1_C;
				dbtn = P2_C;
			}
			obtn &= (team = get_player_sw_close());
			dbtn &= (team);

			// Don't show names if instructions box is up
#if 0
			if (idiot_boxes_shown < 0)
			{
				obtn = 0;
				dbtn = 0;
			}
#endif
			i = NUM_PLAYERS;
			while (i--) {
				if ((psi = names_obj[i]))
				{
					// 1 if on offense, 0 if on defense
					team = (int)(player_blocks[i].team == game_info.off_side);

					// Recalc coordinates if init or player moved or camera moved
					if (upd[i] < 0 ||
							fabs(player_blocks[i].odata.vx) > 0.0001f ||
							fabs(player_blocks[i].odata.vy) > 0.0001f ||
							fabs(player_blocks[i].odata.vz) > 0.0001f ||
							fabs(cambot.dv) > 0.25f)
					{
//fix!!!						// Calc desired position & set recalc flag
//fix!!!						psy  = (team) ? (OFF_NAME_DY*hres*fovfac) : (DEF_NAME_DY*hres*fovfac);
//fix!!!						psy /= player_blocks[i].sz;
//fix!!!
//fix!!!						psi->x_end = (float)(int)(player_blocks[i].sx * fovfac + KXFAC);
//fix!!!						psi->y_end = player_blocks[i].sy * fovfac + KYFAC + psy;
//fix!!!
//fix!!!						if (was_low_res)
//fix!!!							// Reduced version of (-vres/2, * 2/3, +vres/2)
//fix!!!							psi->y_end = (4.0f * psi->y_end + vres) * 0.16666666f;
//fix!!!						psi->y_end = (float)(int)(psi->y_end);

						upd[i] = 1;
					}
					else
						upd[i] = 0;

					// If previous != desired, update position & set recalc flag
					psx = psi->x_end;
					psy = psi->y_end;
					if (psi->x != psx)
					{
						psi->x = psx;
						upd[i] = 1;
					}
					if (psi->y != psy)
					{
						psi->y = psy;
						upd[i] = 1;
					}

					// Chk if its time to hide/unhide names
					if(player_blocks[i].plyrnum == game_info.ball_handler)
						((sprite_node_t *)psi->node_ptr)->mode &= ~HIDE_SPRITE;
					else
						((sprite_node_t *)psi->node_ptr)->mode |= HIDE_SPRITE;
						
				}
				else
					// Clr update flag
					upd[i] = 0;
			}

			// Chk for any overlaps; should reitterate till no overlap
			i = NUM_PLAYERS - 1;
			do
			{
				if (!(psi = names_obj[i]))
					continue;

				j = i;
				while (j--)
				{
					if (!(psj = names_obj[j]))
						continue;

					// Y intersection? Continue if not
					psx = psi->y;
					psy = psj->y;
					if (psx <= (psy - (psj->ii->h + BUF_Y)) ||
							psy <= (psx - (psi->ii->h + BUF_Y)))
						continue;
					// X intersection? Continue if not
					if (psi->x > (psj->x + (psj->ii->w + BUF_X)) ||
							psj->x > (psi->x + (psi->ii->w + BUF_X)))
						continue;
					// Intersection; move whichever is higher
					if (psx < psy)
					{
						//psj->y += 1.0f;
						psj->y += psj->ii->h - psy + psx + BUF_Y;
						upd[j] = 1;
					}
					else
					{
						//psi->y += 1.0f;
						psi->y += psi->ii->h - psx + psy + BUF_Y;
						upd[i] = 1;
					}
					i = NUM_PLAYERS;
					break;
				}
			} while (--i);

			// Update positions
			i = NUM_PLAYERS;
			while (i--)
			{
				if (!upd[i])
					continue;

				// Make sure you put this back before sleeping!!!
				is_low_res = 0;

				generate_sprite_verts(names_obj[i]);

				// Back it goes!!!
				is_low_res = was_low_res;
			}
			sleep(1);
		}

		// Delete all the names
		i = NUM_PLAYERS;
		while (i--)
		{
			if ((psi = names_obj[i]))
				delobj(psi);
		}
	}
}

//-------------------------------------------------------------------------------------------------
//	This process flashes 1-4 lines of text white, then original color, then white, etc.
//  More slowly
//
//	INPUT:	Strings to flash
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void flash_text_slow_proc(int *args)
{ 
ostring_t	*pmsg[4];
int pmsg_org_color[4];
int i;


	pmsg[0] = (ostring_t *)*args;
	// Other strings could be blank
	pmsg[1] = (ostring_t *)*(args+1);
	pmsg[2] = (ostring_t *)*(args+2);
	pmsg[3] = (ostring_t *)*(args+3);

	pmsg_org_color[0] = pmsg[0]->state.color;

	if (pmsg[1])
		pmsg_org_color[1] = pmsg[1]->state.color;
	if (pmsg[2])
		pmsg_org_color[2] = pmsg[2]->state.color;
	if (pmsg[3])
		pmsg_org_color[3] = pmsg[3]->state.color;
		
	while(1)
	{
		sleep(12);
		pmsg[0]->state.color = WHITE;
		change_string_cc( pmsg[0], WHITE );

		// Check for other strings
		if (pmsg[1])
		{
			pmsg[1]->state.color = WHITE;
			change_string_cc( pmsg[1], WHITE );
		}
		if (pmsg[2])
		{
			pmsg[2]->state.color = WHITE;
			change_string_cc( pmsg[2], WHITE );
		}
		if (pmsg[3])
		{
			pmsg[3]->state.color = WHITE;
			change_string_cc( pmsg[3], WHITE );
		}
		sleep(12);

		pmsg[0]->state.color = pmsg_org_color[0];
		change_string_cc( pmsg[0], pmsg_org_color[0] );

		// Check for other strings
		if (pmsg[1])
		{
			pmsg[1]->state.color = pmsg_org_color[1];
			change_string_cc( pmsg[1], pmsg_org_color[1] );
		}
		if (pmsg[2])
		{
			pmsg[2]->state.color = pmsg_org_color[2];
			change_string_cc( pmsg[2], pmsg_org_color[2] );
		}
		if (pmsg[3])
		{
			pmsg[3]->state.color = pmsg_org_color[3];
			change_string_cc( pmsg[3], pmsg_org_color[3] );
		}
	}
}

//-------------------------------------------------------------------------------------------------
//	This process flashes 1-4 lines of text white, then original color, then white, etc.
//
//	INPUT:	Strings to flash
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void flash_text_proc(int *args)
{ 
ostring_t	*pmsg[4];
int pmsg_org_color[4];
int i;

	pmsg[0] = (ostring_t *)*args;
	// Other strings could be blank
	pmsg[1] = (ostring_t *)*(args+1);
	pmsg[2] = (ostring_t *)*(args+2);
	pmsg[3] = (ostring_t *)*(args+3);

	pmsg_org_color[0] = pmsg[0]->state.color;

	if (pmsg[1])
		pmsg_org_color[1] = pmsg[1]->state.color;
	if (pmsg[2])
		pmsg_org_color[2] = pmsg[2]->state.color;
	if (pmsg[3])
		pmsg_org_color[3] = pmsg[3]->state.color;
	
	
		
	while(1)
	{
		sleep(7);
		pmsg[0]->state.color = WHITE;
		change_string_cc( pmsg[0], WHITE );

		// Check for other strings
		if (pmsg[1])
		{
			pmsg[1]->state.color = WHITE;
			change_string_cc( pmsg[1], WHITE );
		}
		if (pmsg[2])
		{
			pmsg[2]->state.color = WHITE;
			change_string_cc( pmsg[2], WHITE );
		}
		if (pmsg[3])
		{
			pmsg[3]->state.color = WHITE;
			change_string_cc( pmsg[3], WHITE );
		}
		sleep(7);


		pmsg[0]->state.color = pmsg_org_color[0];
		change_string_cc( pmsg[0], pmsg_org_color[0] );

		// Check for other strings
		if (pmsg[1])
		{
			pmsg[1]->state.color = pmsg_org_color[1];
			change_string_cc( pmsg[1], pmsg_org_color[1] );
		}
		if (pmsg[2])
		{
			pmsg[2]->state.color = pmsg_org_color[2];
			change_string_cc( pmsg[2], pmsg_org_color[2] );
		}
		if (pmsg[3])
		{
			pmsg[3]->state.color = pmsg_org_color[3];
			change_string_cc( pmsg[3], pmsg_org_color[3] );
		}
	}
}

//-------------------------------------------------------------------------------------------------
//	This process flashes 1-4 lines of alpha text white, then original color, then white, etc.
//
//	INPUT:	Strings to flash
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void flash_alpha_text_proc(int *args)
{ 
ostring_t	*pmsg[4];
int pmsg_org_color[4];
int i;

	pmsg[0] = (ostring_t *)*args;
	// Other strings could be blank
	pmsg[1] = (ostring_t *)*(args+1);
	pmsg[2] = (ostring_t *)*(args+2);
	pmsg[3] = (ostring_t *)*(args+3);

	pmsg_org_color[0] = pmsg[0]->state.color;

	if (pmsg[1])
		pmsg_org_color[1] = pmsg[1]->state.color;
	if (pmsg[2])
		pmsg_org_color[2] = pmsg[2]->state.color;
	if (pmsg[3])
		pmsg_org_color[3] = pmsg[3]->state.color;
	
		
	while(1)
	{
		sleep(7);
		pmsg[0]->state.color = WHITE;
		change_alpha_cc( pmsg[0], WHITE );

		// Check for other strings
		if (pmsg[1])
		{
			pmsg[1]->state.color = WHITE;
			change_alpha_cc( pmsg[1], WHITE );
		}
		if (pmsg[2])
		{
			pmsg[2]->state.color = WHITE;
			change_alpha_cc( pmsg[2], WHITE );
		}
		if (pmsg[3])
		{
			pmsg[3]->state.color = WHITE;
			change_alpha_cc( pmsg[3], WHITE );
		}
		sleep(7);


		pmsg[0]->state.color = pmsg_org_color[0];
		change_alpha_cc( pmsg[0], pmsg_org_color[0] );

		// Check for other strings
		if (pmsg[1])
		{
			pmsg[1]->state.color = pmsg_org_color[1];
			change_alpha_cc( pmsg[1], pmsg_org_color[1] );
		}
		if (pmsg[2])
		{
			pmsg[2]->state.color = pmsg_org_color[2];
			change_alpha_cc( pmsg[2], pmsg_org_color[2] );
		}
		if (pmsg[3])
		{
			pmsg[3]->state.color = pmsg_org_color[3];
			change_alpha_cc( pmsg[3], pmsg_org_color[3] );
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------------
// Look for any active plyr butns to be pressed for several ticks before breaking out
//---------------------------------------------------------------------------------------------------------------------------
void wait_for_all_buttons( int min, int max )
{
	int	i;
	int ticks = 0;
	int	times[4];
	int time1 = 0;
	int time2 = 0;
	int time3 = 0;
	int time4 = 0;
	int	done = FALSE;
	int switches;

	do
	{
		sleep(1);
		ticks++;

		if (ticks < min)
			continue;

		for( i = 0; i < MAX_PLYRS; i++ )
		{
			if (p_status & (1<<i))
			{
				switches = get_but_val_cur(i) & (A_BUTTON|B_BUTTON|C_BUTTON);
				if (switches)
				{
					if (++times[i] >= 3)//8)
					{
						snd_scall_bank(cursor_bnk_str,TICK_SND,VOLUME7,127,PRIORITY_LVL2);
						done = TRUE;
					}
				}
				else
					times[i] = 0;
			}
		}

	} while( (done == FALSE) && (ticks < max));
}
