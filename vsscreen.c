/******************************************************************************/
/*                                                                            */
/* vsscreen.c  - Versus screen code                                           */
/*                                                                            */
/* Written by: JAPPLE & DJT.                                                 */
/* $Revision: 79 $                                                            */
/*                                                                            */
/* Copyright (c) 1996,1997 by Williams Electronics Games Inc.                 */
/* All Rights Reserved                                                        */
/*                                                                            */
/******************************************************************************/
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif
#include <string.h>

#include "/video/nba/include/audits.h"
#include "/video/nba/include/externs.h"
#include "/video/nba/include/fontid.h"
#include "/video/nba/include/game.h"
#include "/video/nba/include/id.h"
#include "/video/nba/include/player.h"
#include "/video/nba/include/select.h"
#include "/video/nba/include/sndcalls.h"
#include "/video/nba/include/transit.h"
#include "/video/nba/include/putil.h"

//#include "/video/nba/include/flash.h"
#define IMAGEDEFINE 1
#include "/video/nba/include/vs.h"


/*
** referenced data & function prototypes
*/
//extern void init_bog_monitor(void);

extern char index_2p[NUM_PLAYERS];
extern char shift_2p[NUM_PLAYERS];

extern int	is_low_res;

extern volatile int imgs_loaded;

extern int which_court;
extern int tsec;
extern int four_plr_ver;						// 0 = low res, 1 = med. res
extern int p_status;
extern float hres, vres;

extern char crowd_bnk_str[];
extern char buyin_bnk_str[];
extern char game_tune_bnk_str[];
extern char ancr_swaped_bnk_str[];
extern char cursor_bnk_str[];
extern char plyr_bnk_str[];
extern gameplay_bnk_str[];
extern char team1_bnk_str[];
extern char team2_bnk_str[];
extern char taunt_bnk_str[];
extern char anc_make_bnk[];

extern char * announcer_strs[];
extern char * qtr_tunes_tbl[];
extern char * game_tunes_tbl[];
extern char * taunt_bnk_strs[];

extern image_info_t chsmidbr,chsmidbr_c1;
extern float title_bar_xys[2][2][2];	// [res][objects][x,y]
extern char start_movie_snd_bnk_str[];

extern player_record_info selected_players[];

extern void load_player_speech_banks(void);
extern void load_team_speech_banks(void);

char * get_ancr_str(char *);
char * get_taunt_str(char *);
void coin_sound_holdoff(int);

struct texture_node	* create_flash_texture(void);

void versus_speech_proc(void);
void update_other_cmos_to_disk(int sleep_allowed);

//static void stick_stadium();

//
// global data & function prototypes
//
int vs_page_showing = 0;

// UP stick
int pup_fog = 0;						//U01 Set for fog game
int pup_infturbo = 0;					//U02 Set for infinite turbo
#ifdef Q_GAME
int pup_qgame = 0;						//U03 Set for quick game
#endif
int pup_no_replays = 0;					//U04
int pup_no_tipoff = 0;					//U05

// DOWN stick
int pup_tourney = 0;					//D01 Set for tournament mode
int pup_thickfog = 0;					//D02 Set for thick fog
int pup_show_shotper = 0;				//D03 Set for shot % display
int pup_no_pushes = 0;					//D04 Set to eliminate pushes
int	pup_show_hotspots = 0;				//D05 Set to show players hotspot until he takes his first shot
int	pup_no_hotspots = 0;				//D05 Set to disallow hotspots
int pup_team_jersey = 0;
int pup_midway_jersey = 0;
int pup_home_jersey = 0;
int pup_away_jersey = 0;
int pup_alt_jersey = 0;

// RIGHT stick
int pup_sfog = 0;						// swamp (green) fog
int pup_bighead = 0;					//R01 Set for player to have a big head
int pup_aba_ball = 0;					//R02 Set for RWB ABA BALL
int pup_no_fouls = 0;					//R03 Set for no freethrow fouls

// LEFT stick
int pup_nfog = 0;						// night (black) fog
int	pup_no_fire = 0;
int	pup_blizzard = 0;
int	pup_snow = 0;
int	pup_rain = 0;

/*
// Stick combo when exitting team select
int	pup_violence = 0;					// Set for violent moves
//int pup_field = 0;						// 0:newgrass,1:oldgrass,2:turf,3:dirt,4:snow
int	pup_stadium = 0;					// 0:day,1:night,2:snow,3:NO stadium

// RIGHT stick
int pup_pullback = 0;					//R1 Set for camera to pull back further
int pup_bigheadteam = 0;				//R3 Set for team to have big heads
int pup_bigplyrs = 0;					//R4 Set for team to have big players
int pup_babyplyrs = 0;					//R5 Set for team to have little players
int pup_bigball = 0;					//R6 Set for oversize football
int pup_rain = 0;						//R8 Set to have rain
int pup_altstadium = 0;					//R9 Set for alternate stadium
int pup_always_receiver = 0;			//R10 Player is never captain on offense

// DOWN stick (all rev 1.0 is here)
int pup_noassist = 0;					//D2 Set for no computer assistance
int pup_superdrones = 0;				//D4 Set drone team to be super-unbeatable
int pup_showfgper = 0;					//D5 Set to show fg percentage
//int pup_groundfog = 0;					//D8 Set for ground fog
int pup_secret_page = 0;				//D9 Set for secret page of offensive plays (old plays from blitz)
int pup_notarget = 0;					//D10 Set for no hiliting target receiver

// UP stick
int pup_offense = 0;					//U1 Set for powerup offense
int pup_defense = 0;					//U2 Set for powerup defense
int pup_drones = 0;						//U3 Set for powerup drones
int pup_blitzers = 0;					//U4 Set for super blitzers
int pup_nopunt = 0;						//U6 Set for no punts
int pup_nointercept = 0;				//U7 Set for no interceptions
int pup_no1stdown = 0;					//U8 Set for no 1st downs
int pup_hugehead = 0;					//U9 Set for player to have a huge head
int pup_noalways = 0;					//U10 Set to cancel any all-time qb/wr on team

// LEFT stick
int pup_nostadium = 0;					//L1 Set for no stadium
int pup_noplaysel = 0;					//L2 Set for no play select screen
int pup_fastpass = 0;					//L3 Set to always have fast passes
int pup_fastturborun = 0;				//L4 Set to have turbo run go faster
int pup_speed = 0;						//L5 Set for powerup speed (players run faster)
int pup_block = 0;						//L6 Set for powerup blocking
int pup_noshowqb = 0;					//L7 Set to not show QB in MODE_QB
int pup_runob = 0;						//L8 Set to have player run as well as jump OB
int pup_superfg = 0;					//L9 Set for no range limit field goals
int pup_always_qb = 0;					//R10 Player is always captain on offense

//#define	Z_HELMET		100
//#define Z_PLATES		1
//#define	NUM_PIECES		19

// Use this globally to track transition state
//extern int plates_on = PLATES_OFF;


// Can use this stuff globally for customization
//char plate_name[20];

//sprite_info_t *plate_obj[] = {NULL, NULL, NULL, NULL};

//image_info_t plate_imgs[] = {
//	{plate_name, 256.0F, 192.0F, 0.0F,      0.0F, 0.0F, 1.0F, 0.0F, 0.75F},	// BL
//	{plate_name, 256.0F, 192.0F, 0.0F,   -192.0F, 0.0F, 1.0F, 0.0F, 0.75F},	// TL
//	{plate_name, 256.0F, 192.0F, 256.0F,    0.0F, 0.0F, 1.0F, 0.0F, 0.75F},	// BR
//	{plate_name, 256.0F, 192.0F, 256.0F, -192.0F, 0.0F, 1.0F, 0.0F, 0.75F}	// TR
//};
*/

//
// local data & function prototypes
//
#define Z_PUP_OVAL		90
#define Z_PUP_ICON		80
#define Z_PUP_TEXT		70
#define Z_SPARKLE		60


#define PUP_CODE_CNT	(sizeof(pupcodes)/sizeof(struct tagPUPCODE))

#define PUP_ICON_CNT	3
#define PUP_ICON_IMGCNT	(sizeof(pii_pup_icon)/sizeof(void *))

#define SPARKLE_IMGCNT	(sizeof(sparkle_imgs)/sizeof(void *))

struct tagPUPCODE {
	int    code;
	int    stick;
	int	   control;
	int *  flag;
	char * text1;
	char * text2;
};

// control bits for powerups
#define PC_TRNY		0x01		// not allowed in tournament mode
#define PC_1P		0x02		// only works if one team is all-drone
#define PC_2P		0x04		// only works if humans are on both teams
#define PC_AGREE	0x08		// both teams must set
#define PC_TCANCEL	0x10		// cleared if set by teammates
#define PC_4PONLY	0x20		// not available in 2-player kit

static struct tagPUPCODE pupcodes[] =
{
	{ 0x123, JOY_UP,	0,							&pup_fog,			"FOG ON",			"OUTDOORS ONLY" },
//	{ 0x411, JOY_UP,	PC_TRNY,					&pup_infturbo,		"INFINITE TURBO",	NULL },
#ifdef Q_GAME
	{ 0x111, JOY_UP,	0,							&pup_qgame,			"QUICK GAME",		"(ONLY FOR TEST!!)" },
#endif
	{ 0x444, JOY_UP,	PC_AGREE,					&pup_no_tipoff,		"NO TIPOFF",		"(TEAMS MUST AGREE)" },
	{ 0x201, JOY_UP,	PC_AGREE,					&pup_no_hotspots,	"NO HOTSPOTS",		"(TEAMS MUST AGREE)" },

	{ 0x111, JOY_DOWN,	PC_2P,						&pup_tourney,		"TOURNAMENT MODE",	"(IN A 2 TEAM GAME)" },
	{ 0x123, JOY_DOWN,	0,							&pup_thickfog,		"THICK FOG ON",		"OUTDOORS ONLY" },
	{ 0x001, JOY_DOWN,	0,							&pup_show_shotper,	"SHOW SHOT %",		NULL },
	{ 0x100, JOY_DOWN,	0,							&pup_show_hotspots,	"SHOW HOTSPOT",		NULL },
//	{ 0x321, JOY_DOWN,	PC_TRNY|PC_AGREE,			&pup_no_pushes,		"NO PUSHING",		NULL },

	{ 0x123, JOY_RIGHT,	0,							&pup_sfog,			"SWAMP FOG ON",		"OUTDOORS ONLY" },
	{ 0x200, JOY_RIGHT,	0,							&pup_bighead,		"BIG HEAD",			NULL },
	{ 0x232, JOY_RIGHT,	0,							&pup_aba_ball,		"ABA BALL",			NULL },
//	{ 0x222, JOY_RIGHT,	PC_TRNY|PC_AGREE,			&pup_no_fouls,		"NO FOULS",			"(IN A 2 TEAM GAME)" },
	{ 0x400, JOY_RIGHT,	PC_AGREE,					&pup_team_jersey,	"TEAM UNIFORM",		"(TEAMS MUST AGREE)" },
	{ 0x401, JOY_RIGHT,	0,							&pup_midway_jersey,	"MIDWAY UNIFORM",	NULL },
	{ 0x410, JOY_RIGHT,	0,							&pup_home_jersey,	"HOME UNIFORM",		NULL },
	{ 0x420, JOY_RIGHT,	0,							&pup_away_jersey,	"AWAY UNIFORM",		NULL },
	{ 0x430, JOY_RIGHT,	0,							&pup_alt_jersey,	"ALTERNATE UNIFORM",NULL },

	{ 0x123, JOY_LEFT,	0,							&pup_nfog,			"NIGHT FOG ON",		"OUTDOORS ONLY" },
//	{ 0x222, JOY_LEFT,	0,							&pup_no_fire,		"NO FIRE",			NULL },
	{ 0x121, JOY_LEFT,	0,							&pup_snow,			"SNOW"			,	"OUTDOORS ONLY" },
	{ 0x131, JOY_LEFT,	0,							&pup_blizzard,		"BLIZZARD"		,	"OUTDOORS ONLY" },
	{ 0x141, JOY_LEFT,	0,							&pup_rain,			"RAINY DAYS",	  	"OUTDOORS ONLY" },
	{ 0x331, JOY_LEFT,	0,							&pup_no_replays,	"NO REPLAYS",		NULL },
};

//FIX!!!
/*	{ 0x203, JOY_RIGHT, 0,							&pup_bigheadteam,  "TEAM BIG HEADS",     NULL },
//	{ 0x021, JOY_RIGHT, PC_AGREE,					&pup_pullback,     "SHOW MORE FIELD",    "(TEAMS MUST AGREE)" },
	{ 0x141, JOY_RIGHT, PC_TRNY,					&pup_bigplyrs,     "TEAM BIG PLAYERS",   NULL },
	{ 0x310, JOY_RIGHT, PC_TRNY,					&pup_babyplyrs,    "TEAM TINY PLAYERS",  NULL },
	{ 0x050, JOY_RIGHT, PC_TRNY,					&pup_bigball,      "BIG FOOTBALL",       NULL },
	{ 0x102, JOY_RIGHT, 0,							&pup_rcvrname,     "HIDE RECEIVER NAME", NULL },
//	{ 0x555, JOY_RIGHT, PC_TRNY,					&pup_rain,         "WEATHER: RAIN",      NULL },
//	{ 0x534, JOY_RIGHT, 0,							&pup_altstadium,   "SKY STADIUM",        NULL },
	{ 0x222, JOY_RIGHT,	PC_TCANCEL|PC_4PONLY,		&pup_always_receiver, "ALWAYS RECEIVER", "(REQUIRES HUMAN TEAMMATE)" },
	{ 0x232, JOY_DOWN,  0,							&pup_groundfog,    "GROUND FOG ON",      NULL },

	{ 0x012, JOY_DOWN,  PC_2P|PC_AGREE,				&pup_noassist,     "NO CPU ASSISTANCE",  "(TEAMS MUST AGREE)" },
	{ 0x423, JOY_DOWN,  PC_TRNY|PC_AGREE,			&pup_nofumbles,    "NO RANDOM FUMBLES",  "(TEAMS MUST AGREE)" },
	{ 0x314, JOY_DOWN,  PC_TRNY|PC_1P,				&pup_superdrones,  "SMART CPU OPPONENT", "(IN A 1 TEAM GAME)" },
	{ 0x001, JOY_DOWN,  0,							&pup_showfgper,    "SHOW FIELD GOAL %",  NULL },
	{ 0x321, JOY_DOWN,	PC_TRNY,					&pup_notarget,     "NO HILITING ON",	 "TARGET RECEIVER" },
	{ 0x333, JOY_DOWN,  0,							&pup_secret_page,  "SECRET PLAYS",		 NULL },

	{ 0x312, JOY_UP,    PC_TRNY,					&pup_offense,      "POWERUP OFFENSE",    NULL },
	{ 0x421, JOY_UP,    PC_TRNY,					&pup_defense,      "POWERUP DEFENSE",    NULL },
	{ 0x233, JOY_UP,    PC_TRNY,					&pup_drones,       "POWERUP TEAMMATES",  NULL },
	{ 0x045, JOY_UP,    PC_TRNY,					&pup_blitzers,     "SUPER BLITZING",     NULL },
	{ 0x514, JOY_UP,    PC_TRNY,					&pup_infturbo,     "INFINITE TURBO",     NULL },
	{ 0x151, JOY_UP,    PC_TRNY,					&pup_nopunt,       "NO PUNTING",         NULL },
	{ 0x344, JOY_UP,    PC_TRNY,					&pup_nointercept,  "NO INTERCEPTIONS",   NULL },
	{ 0x210, JOY_UP,    PC_TRNY,					&pup_no1stdown,    "NO FIRST DOWNS",     NULL },
	{ 0x040, JOY_UP,    PC_TRNY,					&pup_hugehead,     "HUGE HEAD",          NULL },
	{ 0x333, JOY_UP,    PC_4PONLY,					&pup_noalways,     "CANCEL ALWAYS",      "QB/RECEIVER" },

//	{ 0x500, JOY_LEFT,  PC_TRNY,					&pup_nostadium,    "TURN OFF STADIUM",   NULL },
	{ 0x115, JOY_LEFT,  PC_TRNY,					&pup_noplaysel,    "NO PLAY SELECTION",  "(TEAMS MUST AGREE)" },
	{ 0x250, JOY_LEFT,  PC_TRNY,					&pup_fastpass,     "FAST PASSES",        NULL },
	{ 0x032, JOY_LEFT,  PC_TRNY,					&pup_fastturborun, "FAST TURBO RUNNING", NULL },
	{ 0x404, JOY_LEFT,  PC_TRNY|PC_AGREE,			&pup_speed,        "POWERUP SPEED",      "(TEAMS MUST AGREE)" },
	{ 0x312, JOY_LEFT,  PC_TRNY,					&pup_block,        "POWERUP BLOCKERS",   NULL },
//	{ 0x342, JOY_LEFT,  PC_TRNY,					&pup_noshowqb,     "INVISIBLE QB",       NULL },
	{ 0x211, JOY_LEFT,  PC_TRNY,					&pup_runob,        "ALLOW STEPPING OB",  NULL },
	{ 0x123, JOY_LEFT,  PC_TRNY,					&pup_superfg,      "SUPER FIELD GOALS",  NULL },
	{ 0x222, JOY_LEFT,	PC_TCANCEL|PC_4PONLY,		&pup_always_qb,    "ALWAYS QB",			NULL }
};*/

//static image_info_t *sparkle_imgs[] =
//{
//	&sparkc01,&sparkc02,&sparkc03,&sparkc04,&sparkc05,
//	&sparkc06,&sparkc07,&sparkc08,&sparkc09				//,&sparkc10
//};

static image_info_t * vs_logo_imgs[2][NUM_TEAMS + 1] = {	/* + 1 for the secret team */
	{	&atll,&bosl,&cha_l,&chil,&clel,&dall,
		&denl,&detl,&goll,&houl,&indl,&lacl,
		&lakl,&mial,&mill,&minl,&nwjl,&nwyl,
		&orll,&phil,&phol,&porl,&sacl,&sanl,
		&seal,&torl,&utal,&vanl,&wasl,&alsl},

	{	&atlr,&bosr,&cha_r,&chir,&cler,&dalr,
		&denr,&detr,&golr,&hour,&indr,&lacr,
		&lakr,&miar,&milr,&minr,&nwjr,&nwyr,
		&orlr,&phir,&phor,&porr,&sacr,&sanr,
		&sear,&torr,&utar,&vanr,&wasr,&alsr}
};


static image_info_t * pii_pup_icon[] = {
	&vs_mdwy,&vs_ball,&vs_a,&vs_b,&vs_c,&vs_n
};


// X positions for icons
#define D	38.0f

#define	PX1	 28.0f
#define	PX2	155.0f
#define	PX3	280.0f
#define	PX4	407.0f

static float x_pup_icon[2][MAX_PLYRS][PUP_ICON_CNT] = {		// [2/4 plyr][plyr][x]
	{{111, 111+D, 111-D}, 									// p1 (2 player version)
	 {  0,     0,     0},									// p2
	 {404, 404+D, 404-D}, 									// p3
	 {  0,     0,     0}},									// p4

	{{PX1+D, PX1+D*2, PX1},	 								// p1 (4 player version)
	 {PX2+D, PX2+D*2, PX2},									// p2
	 {PX3+D, PX3+D*2, PX3},									// p3
	 {PX4+D, PX4+D*2, PX4}}									// p4
};


static float y_pup_icon[] = { 35, 35 };						// Y [2/4 plr version]
static float y_pup_text[2][4] = {{90,   0, 90,   0},		// Y [2 plr version]
								 {90, 100, 90, 100}};		// Y [4 plr version]

static float z_pup_icon[] = { Z_PUP_ICON + 1.0f, Z_PUP_ICON + 0.0f, Z_PUP_ICON + 2.0f };
static float z_pup_oval[] = { Z_PUP_OVAL + 1.0f, Z_PUP_OVAL + 0.0f, Z_PUP_OVAL + 2.0f };


//static char *field_selected_msg[] = {
//	"STRIPE GRASS FIELD",
//	"BLITZ FIELD",
//	"ASTRO TURF FIELD",
//	"DIRT FIELD",
//	"SNOW FIELD",
//	NULL };

//static char *stadium_selected_msg[] = {
//	"DAY STADIUM",
//	"NIGHT STADIUM",
//	"SNOW STADIUM",
//	"NO STADIUM",
//	NULL };


extern image_info_t nbcbk2,nbcbk2_c1,nbcbk2_c2,nbcbk2_c3;

//-------------------------------------------------------------------------------------------------
//  Just clear all the power-up flags NOT allowed in Tournament Mode
//-------------------------------------------------------------------------------------------------
void clear_tourney_pups(void)
{
	int	i;

	for( i = 0; i < PUP_CODE_CNT; i++ )
	{
		if( pupcodes[i].control & PC_TRNY )
		{
#ifdef PUP_DEBUG
			if( pupcodes[i].flag[0] )
				fprintf( stderr, "PUP: %s cleared\n", pupcodes[i].text1 );
#endif
			pupcodes[i].flag[0] = 0;
		}
	}
}

//-------------------------------------------------------------------------------------------------
//  Just clear all the power-up flags... called from game over and vs scrn
//-------------------------------------------------------------------------------------------------
void clear_pups(void)
{
	int	i;

	for( i = 0; i < PUP_CODE_CNT; i++ )
		pupcodes[i].flag[0] = 0;
}

//-------------------------------------------------------------------------------------------------
// flash_code_proc() - assumes sparkle-texture preloaded with
//    id SCR_PLATE_TID (default; see <create_flash_texture> in nfl.c)
//  INPUT: x,y,z for sparkle
//-------------------------------------------------------------------------------------------------
void flash_code_proc(int *args)
{
//	sprite_info_t * pobj;
//	int i;
//
//	pobj = beginobj(sparkle_imgs[0],
//			(float)args[0],
//			(float)args[1],
//			(float)args[2],
//			SCR_PLATE_TID);
//	pobj->id = OID_VERSUS;
//	pobj->w_scale = 2.5f;
//	pobj->h_scale = 2.5f;
//	generate_sprite_verts(pobj);
//
//	for (i = 1; i < SPARKLE_IMGCNT; i++) {
//		sleep(3);
//		change_img_tmu(pobj,sparkle_imgs[i],SCR_PLATE_TID);
//	}
//	// Show the last frame
	sleep(3);
//	delobj(pobj);
}

//-------------------------------------------------------------------------------------------------
// vs_unhide_proc() - 
//-------------------------------------------------------------------------------------------------
void vs_unhide_proc(int *args)
{
	hide_sprite((sprite_info_t *)(args[1]));

	while (!(p_status & (1 << args[0])))
	{
		sleep(1);
	}

	unhide_sprite((sprite_info_t *)(args[1]));
}

//-------------------------------------------------------------------------------------------------
// 					vs_pups_proc() - 
//-------------------------------------------------------------------------------------------------
#ifdef XJASONX
#define VS_TICKS		(99 * tsec)					//5 // total ticks
#else
#define VS_TICKS		(7 * tsec)					//5 // total ticks
#endif

void vs_pups_proc(int *args)
{
	int cnt, i;
	
#ifndef NOVS
	int sw_curr;
	int sw_down;
	int sw_last = 0;
	sprite_info_t * obj_oval[MAX_PLYRS];
	sprite_info_t * obj_icon[MAX_PLYRS][PUP_ICON_CNT];
	int cur_icon[MAX_PLYRS][PUP_ICON_CNT];

	int j, k, n, s, t;
	float y;


	// Init the pup flags
	i = 0;
	do {
		pupcodes[i].flag[0] = 0;
	} while (++i < PUP_CODE_CNT);

	// Init all the objects, keeping them hidden till later
	i = 0;
	do {
		// create ICON backplate
		obj_oval[i] = beginobj(&vs_ibox,
				x_pup_icon[four_plr_ver][i][0],			// use center ICON as positioner
				y_pup_icon[four_plr_ver],
				z_pup_oval[0],
				BARS_PG_TID);
		obj_oval[i]->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
		obj_oval[i]->id = OID_VERSUS;

		qcreate("vs_unhd", PRE_GAME_SCRNS_PID, vs_unhide_proc, i, (int)(obj_oval[i]), 0, 0);

		j = 0;
		do {
			obj_icon[i][j] = beginobj(pii_pup_icon[cur_icon[i][j] = 0],
					x_pup_icon[four_plr_ver][i][j],
					y_pup_icon[four_plr_ver],
					z_pup_icon[j],
					BARS_PG_TID);
			obj_icon[i][j]->id = OID_VERSUS;

			qcreate("vs_unhd", PRE_GAME_SCRNS_PID, vs_unhide_proc, i, (int)(obj_icon[i][j]), 0, 0);

		} while (++j < PUP_ICON_CNT);
	} while (++i < MAX_PLYRS);
#endif	//NOVS

	// Wait a bit before making input active
	sleep(6);


	// Loop here till its time to go
	cnt = VS_TICKS;
	do {
#ifndef NOVS
		sw_curr = get_player_sw_current();
		sw_down = (sw_curr ^ sw_last) & sw_curr;
		sw_last = sw_curr;
		s = 0;
		i = 0;
		do {
			if (p_status & (1 << i))
			{
				if ((k = (sw_down >> (((four_plr_ver?i:index_2p[i])<<3)+4) ) & ((P_A|P_B|P_C)>>4) ))
				{
					s |= 1;
					n = 0;
					do {
						if (k & 1)
						{
							if (++cur_icon[i][n] == PUP_ICON_IMGCNT)
								cur_icon[i][n] = 0;
						}
					} while (k >>= 1, ++n < PUP_ICON_CNT);
					// Set flag to do <change_img_tmu>
					k = 1;
				}
				if ((t = (sw_down >> ((four_plr_ver?i:index_2p[i])<<3) ) & P_RLDU_MASK))
				{
					// Reorder the counts to be buttons C,A,B
					j = (cur_icon[i][2] << 8) |
						(cur_icon[i][0] << 4) |
						(cur_icon[i][1]);
					// Always clr the counts
					cur_icon[i][0] = cur_icon[i][1] = cur_icon[i][2] = 0;
					// Always clr any old text if a non-000 was entered
					if (j) delete_multiple_strings( i+5, -1);
					// See if code does something
					n = PUP_CODE_CNT;
					while (n--)
						if (pupcodes[n].code == j && pupcodes[n].stick == t)
							break;

					// ignore PC_4PONLY codes if two-player kit
					if ((n != -1) && (!four_plr_ver) && (pupcodes[n].control & PC_4PONLY))
						n = -1;

					if (n >= 0)
					{
						// Don't do multiple entries of the same code!
						if (!(pupcodes[n].flag[0] & (1<<i)))
						{
							// Do the valid code
							pupcodes[n].flag[0] |= (1<<i);
#ifdef PUP_DEBUG
							fprintf(stderr,"PUP: %s is now 0x%x\r\n",pupcodes[n].text1, pupcodes[n].flag[0]);
#endif
							qcreate("vs_sprk", PRE_GAME_SCRNS_PID, flash_code_proc,
								x_pup_icon[four_plr_ver][i][0], y_pup_icon[four_plr_ver], Z_SPARKLE+1.0f, 0);
							qcreate("vs_sprk", PRE_GAME_SCRNS_PID, flash_code_proc,
								x_pup_icon[four_plr_ver][i][1], y_pup_icon[four_plr_ver], Z_SPARKLE+0.0f, 0);
							qcreate("vs_sprk", PRE_GAME_SCRNS_PID, flash_code_proc,
								x_pup_icon[four_plr_ver][i][2], y_pup_icon[four_plr_ver], Z_SPARKLE+2.0f, 0);

							set_string_id( i+5 );
							set_text_font( is_low_res ? FONTID_BAST8TA : FONTID_NBA10 );
							set_text_justification_mode( HJUST_CENTER|VJUST_CENTER );
							set_text_z_inc( -0.01f );

							y = y_pup_text[four_plr_ver][i];
							if (pupcodes[n].text2)
							{
								set_text_position(x_pup_icon[four_plr_ver][i][0], y - (is_low_res ? 18.0f : 14.0f), Z_PUP_TEXT);
								oprintf("%dc%s", LT_YELLOW, pupcodes[n].text2);
							}
							set_text_position(x_pup_icon[four_plr_ver][i][0], y, Z_PUP_TEXT);
							oprintf("%dc%s", LT_YELLOW, pupcodes[n].text1);

							set_text_z_inc( 0.0f );

							s |= 2;

							// SPECIAL:  If noalways, SET always_qb and always_wr for both members of team
							//if (pupcodes[n].flag == &pup_noalways)
							//{
							//	pup_always_qb |= TEAM_MASK(i/2);
							//	pup_always_receiver |= TEAM_MASK(i/2);
							//}

							// SPECIAL:  If always_receiver, clear always_qb, and vice versa
							//if (pupcodes[n].flag == &pup_always_qb)
							//	pup_always_receiver &= ~(1<<i);
							//if (pupcodes[n].flag == &pup_always_receiver)
							//	pup_always_qb &= ~(1<<i);
						}
						else
						{
							s |= 4;
						}
					}
					else
					{
						if (j) s |= 4;
					}
					// Set flag to do <change_img_tmu>
					k = 1;
				}
				if (k)
				{
					obj_icon[i][0]->ii = pii_pup_icon[cur_icon[i][0]];
					generate_sprite_verts(obj_icon[i][0]);

					obj_icon[i][1]->ii = pii_pup_icon[cur_icon[i][1]];
					generate_sprite_verts(obj_icon[i][1]);

					obj_icon[i][2]->ii = pii_pup_icon[cur_icon[i][2]];
					generate_sprite_verts(obj_icon[i][2]);
				}
			}
		} while (++i < MAX_PLYRS);

		if (s & 4)
			snd_scall_bank(cursor_bnk_str,VS_WRONG_CODE_SND,VOLUME3,127,PRIORITY_LVL3);		// wrong combo
		if (s & 2)
			snd_scall_bank(cursor_bnk_str,VS_GOT_CODE_SND,VOLUME3,127,PRIORITY_LVL3); 		// got one!!
		if (s & 1)
			snd_scall_bank(cursor_bnk_str,VS_CHANGE_ICON_SND,VOLUME3,127,PRIORITY_LVL3);	// changing icon
#endif	//NOVS

		sleep(1);
	} while (cnt--);

#ifdef PUP_DEBUG
	fprintf(stderr, "checking powerup flags\n");
#endif
	// process flags on powerups
	for( i = 0; i < PUP_CODE_CNT; i++ )
	{
		// clear any PC_AGREE powerups that haven't been set by both teams
		if ((pupcodes[i].control & PC_AGREE) &&
			(pupcodes[i].flag[0]) && (
				!(pupcodes[i].flag[0] & 0x3) ||
				!(pupcodes[i].flag[0] & 0xC)))
		{
			pupcodes[i].flag[0] = 0;
#ifdef PUP_DEBUG
			fprintf( stderr, "PUP: %s not agreed, cleared\n", pupcodes[i].text1 );
#endif
		}

		// One or two human teams?
		if ((p_status & 0x3) && (p_status & 0xC))
		{
			// Two; clear any PC_1P powerups
			if (pupcodes[i].control & PC_1P)
			{
#ifdef PUP_DEBUG
				if (pupcodes[i].flag[0])
				fprintf( stderr, "PUP: %s used with 2 human teams, cleared\n", pupcodes[i].text1 );
#endif
				pupcodes[i].flag[0] = 0;
			}
		}
		else
		{
			// One; clear any PC_2P powerups
			if (pupcodes[i].control & PC_2P)
			{
#ifdef PUP_DEBUG
				if (pupcodes[i].flag[0])
				fprintf( stderr, "PUP: %s used with 1 human team, cleared\n", pupcodes[i].text1 );
#endif
				pupcodes[i].flag[0] = 0;
			}
		}

		// clear any PC_TCANCEL powerups that were set by both teammates
		if ((pupcodes[i].control & PC_TCANCEL) &&
			(four_plr_ver))
		{
			if ((pupcodes[i].flag[0] & 0x03) == 0x03)
				pupcodes[i].flag[0] &= ~0x03;
			if ((pupcodes[i].flag[0] & 0x0C) == 0x0C)
				pupcodes[i].flag[0] &= ~0x0C;
		}
	}
}

//-------------------------------------------------------------------------------------------------
//			 vs_screen() - 
//-------------------------------------------------------------------------------------------------
void vs_screen(void)
{
	struct texture_node	* tn;
	sprite_info_t 	*bkgrnd_obj;
	image_info_t	**img;

	// set random field & stadium
	// choose turf just over half the time
//	pup_field = randrng(5);
//	if (randrng(2))
//		pup_field = 2;
//	pup_stadium = (pup_field == 4) ? 2 : randrng(2);

	// now look for player override
//	stick_stadium();

	wipeout();
	sleep(1);
	
	vs_page_showing = 1;
//	#ifdef DEBUG
//	init_bog_monitor();
//	#endif

	// Re-create the wipeout'd plate objects
//	create_plates();

	// Re-enable draw
	draw_enable(1);
	normal_screen();

	iqcreate("vs_spch", PRE_GAME_SCRNS_PID+1, versus_speech_proc, 0, 0, 0, 0);

	// Make versus page background
//	bkgrnd_obj = beginobj(&nbcbk2,    0.0F, SPRITE_VRES, 140.0f, BARS_PG_TID);
//	bkgrnd_obj->id = OID_VERSUS;
//	bkgrnd_obj = beginobj(&nbcbk2_c1, 0.0F, SPRITE_VRES, 140.0f, BARS_PG_TID);
//	bkgrnd_obj->id = OID_VERSUS;
//	bkgrnd_obj = beginobj(&nbcbk2_c2, 0.0F, SPRITE_VRES, 140.0f, BARS_PG_TID);
//	bkgrnd_obj->id = OID_VERSUS;
//	bkgrnd_obj = beginobj(&nbcbk2_c3, 0.0F, SPRITE_VRES, 140.0f, BARS_PG_TID);
//	bkgrnd_obj->id = OID_VERSUS;

	bkgrnd_obj = beginobj(&vsbk2,   	  0.0F, SPRITE_VRES, 500.0F, BARS_PG_TID);
	bkgrnd_obj->id = OID_VERSUS;
	bkgrnd_obj = beginobj(&vsbk2_c1,	  0.0F, SPRITE_VRES, 500.0F, BARS_PG_TID);
	bkgrnd_obj->id = OID_VERSUS;
	bkgrnd_obj = beginobj(&vsbk2_c2,   	  0.0F, SPRITE_VRES, 500.0F, BARS_PG_TID);
	bkgrnd_obj->id = OID_VERSUS;
	bkgrnd_obj = beginobj(&vsbk2_c3,   	  0.0F, SPRITE_VRES, 500.0F, BARS_PG_TID);
	bkgrnd_obj->id = OID_VERSUS;

	// put up title bar and text
	bkgrnd_obj = beginobj(&chsmidbr, title_bar_xys[four_plr_ver][1][X_VAL], title_bar_xys[four_plr_ver][1][Y_VAL], 14.0F, BARS_PG_TID);
	bkgrnd_obj->id = OID_VERSUS;
	bkgrnd_obj = beginobj(&chsmidbr_c1, title_bar_xys[four_plr_ver][1][X_VAL], title_bar_xys[four_plr_ver][1][Y_VAL], 14.0F, BARS_PG_TID);
	bkgrnd_obj->id = OID_VERSUS;
	set_string_id(89);
	set_text_z_inc(0.01f);
	set_text_font(FONTID_NBA23);	/* FONTID_NBA25 */
	set_text_color(LT_YELLOW);
	set_text_position((SPRITE_HRES/2.0f)+25.0f,329.0f, 2.0F);
	oprintf("%djTONIGHT'S MATCHUP", (HJUST_CENTER|VJUST_CENTER));
	set_text_z_inc(0.0f);

	// put up VS image
	bkgrnd_obj = beginobj(&vs, 0.0f, SPRITE_VRES, 94.0f, BARS_PG_TID);
	bkgrnd_obj->id = OID_VERSUS;

	// team 1 name logo
	bkgrnd_obj = beginobj(vs_logo_imgs[0][game_info.team[0]], 128.0f, SPRITE_VRES/2.0f, 95.0f, BARS_PG_TID);
	bkgrnd_obj->id = OID_VERSUS;

	// team 2 name logo
	bkgrnd_obj = beginobj(vs_logo_imgs[1][game_info.team[1]], 384.0f+10.0f, SPRITE_VRES/2.0f, 95.0f, BARS_PG_TID);
	bkgrnd_obj->id = OID_VERSUS;

//#ifndef NOVS
//	// Load the rest of the textures
//	if (!create_texture(pii_pup_icon[0]->texture_name,
//			BARS_PG_TID,
//			0,
//			CREATE_NORMAL_TEXTURE,
//			GR_TEXTURECLAMP_CLAMP,
//			GR_TEXTURECLAMP_CLAMP,
//			GR_TEXTUREFILTER_BILINEAR,
//			GR_TEXTUREFILTER_BILINEAR))
//	{
//#ifdef DEBUG
//		fprintf( stderr,"Error loading %s\r\n",pii_pup_icon[0]->texture_name);
//#endif
//		increment_audit(TEX_LOAD_FAIL_AUD);
//	}
//	// Sparkle texture is loaded as the initial/between qtr load with an id
//	//  of SCR_PLATE_TID
//	if (!(tn = create_flash_texture()))
//	{
//#ifdef DEBUG
//		fprintf( stderr,"Error loading %s\r\n",sparkle_imgs[0]->texture_name);
//#endif
//		increment_audit(TEX_LOAD_FAIL_AUD);
//	}

//	lock_texture(tn);
//#endif	//NOVS
	lock_multiple_textures(BARS_PG_TID,0xffffffff);

	iqcreate("vs_pups", PRE_GAME_SCRNS_PID+1, vs_pups_proc, 0, 0, 0, 0);


#ifndef NOAUDIO
//////////////////////////////////////////
// Load all the sound data
//
	// delete movie sound bank
	snd_stop_all();
	snd_set_reserved(SND_TRACK_0|SND_TRACK_5);

	// play vs. tune
	snd_scall_bank(buyin_bnk_str, 1, VOLUME6, 127, SND_PRI_SET|SND_PRI_TRACK0);

	snd_bank_delete(start_movie_snd_bnk_str);

	coin_sound_holdoff(TRUE);

	// load court bank
	if (COURT_IS_INDOORS)
		strcpy(crowd_bnk_str, "nbccrowd");
	else if (which_court == COURT_GILMORE)
		strcpy(crowd_bnk_str, "outcrowd");
	else
		strcpy(crowd_bnk_str, "jngcrowd");

	if (snd_bank_load_playing(crowd_bnk_str) == 0xeeee)
		increment_audit(SND_LOAD_FAIL_AUD);

	// load speech banks
	load_team_speech_banks();
	load_player_speech_banks();

	// load MAKE shot speech bank
	strcpy(anc_make_bnk, "sca_mak1");
	if (snd_bank_load_playing(anc_make_bnk) == 0xeeee)	// swapped at halftime
		increment_audit(SND_LOAD_FAIL_AUD);

	// load game tune
	strcpy(game_tune_bnk_str, game_tunes_tbl[0]);
	//if (snd_bank_load(game_tune_bnk_str) == 0xeeee)
	if (snd_bank_load_playing(game_tune_bnk_str) == 0xeeee)
	{
		increment_audit(SND_LOAD_FAIL_AUD);
		#if DEBUG
		fprintf(stderr,"error loading game tune\n");
		#endif
	}

	coin_sound_holdoff(FALSE);
#endif
	/* flush the CMOS cache */
	write_cmos_to_disk(TRUE);
	update_other_cmos_to_disk(TRUE);
	
	sleep(1);

	turn_transit_off(1);

#if 0
	game_info.team[0] = TEAM_UTA;
	game_info.team[1] = TEAM_LAL;
	set_player_record(TEAM_UTA, 0, TRUE, &selected_players[0]);
	set_player_record(TEAM_UTA, 1, TRUE, &selected_players[1]);
	set_player_record(TEAM_LAL, 2, TRUE, &selected_players[2]);
	set_player_record(TEAM_LAL, 0, TRUE, &selected_players[3]);
#endif
}

//////////////////////////////////////////////////////////////////////////////
// read all the sticks & buttons looking for stadium powerups.  If more than
// one player has an opinion, pick one at random and listen to him.
//static void stick_stadium( void )
//{
//	int		fields[4];
//	int		stadiums[4];
//	int		i, field, stadium, sw, ocnt = 0;
//
//	for( i = 0; i < 4; i++ )
//	{
//		sw = get_player_sw_current() >> (8*i);
//		field = -1;
//		stadium = -1;
//
//		if (!(p_status & (1<<i)))
//			continue;
//
//		if (sw & P1_C)
//		{
//			if (sw & P1_UP)
//				field = 0;
//			else if (sw & P1_DOWN)
//				field = 1;
//			else if (sw & P1_LEFT)
//				field = 2;
//			else if (sw & P1_RIGHT)
//				field = 3;
//		}
//
//		if ((sw & P1_A) && (sw & P1_B))
//		{
//			stadium = 2;
//			field = 4;
//		}
//		else if (sw & P1_A)
//			stadium = 0;
//		else if (sw & P1_B)
//			stadium = 1;
//
//		if ((field != -1) || (stadium != -1))
//		{
//			fields[ocnt] = field;
//			stadiums[ocnt++] = stadium;
//		}
//	}
//
//	if (ocnt == 0)
//		return;
//
//	i = randrng(ocnt);
//
//	if (fields[i] != -1)
//		pup_field = fields[i];
//	if (stadiums[i] != -1)
//		pup_stadium = stadiums[i];
//}

//////////////////////////////////////////////////////////////////////////////
