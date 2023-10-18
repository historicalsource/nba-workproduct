/*
 *		$Archive: /video/Nba/attract.c $
 *		$Revision: 166 $
 *		$Date: 10/14/99 6:18p $
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
char *ss_attract_c = "$Workfile: attract.c $ $Revision: 166 $";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <goose/goose.h>
#include <glide/glide.h>
#include <string.h>
#include <dir.h>

#include "putil.h"
#include "fontid.h"
#include "game.h"
#include "coin.h"
#include "player.h"
#include "nbacmos.h"
#include "select.h"
#include "sndcalls.h"
#include "externs.h"
#include "id.h"
#include "audits.h"
#include "gameadj.h"
#if 0
#include "stream.h"
#endif
#include "movie.h"
#include "mainproc.h"
#include "transit.h"
#include "putil.h"
#include "particle.h"
#include "game_sel.h"
#include "sweeps.h"

#define IMAGEDEFINE	1
#include "amode.h"
#include "endgame.h"

////////////////////////////////////////

#define 	LOOPS_UNTIL_REBOOT	3		// Number of attract loops before a reboot

#ifdef DEBUG
//	#define TEST_EXEC			//Include for immediate EXEC execution
#endif

////////////////////////////////////////

typedef struct popular_team_info
{
	int		team_number;
	int		times_choosen;
} popular_team_info_t;

unsigned int tick_sndn;
unsigned int coin_snd_drop_calln;
unsigned int coin_snd_paid_calln;
int	credit_pg_on;

extern int which_court;
extern int game_just_finished;
extern int		p_status;	   					// from main.c; represents current plyrs in game with bit pos. (0-3)
extern int		show_plysel_idiot_box;
extern unsigned int	start_crc32;
extern char plyr_bnk_str[];
extern font_info_t bast75_font;
extern font_info_t nba14_font;
extern int pup_blizzard;
extern int pup_snow;
extern int pup_rain;
extern int alt_ladder_active;

extern image_info_t selbk,selbk_c1,selbk_c2,selbk_c3;
extern image_info_t nbcbk1,nbcbk1_c1,nbcbk1_c2,nbcbk1_c3;
extern image_info_t nbcsports,nbcsports_c1,nbcsports_c2,nbcsports_c3;
extern image_info_t chsmidbr,chsmidbr_c1;
extern ostring_t *ostring_list;
extern sprite_node_t *alpha_sprite_node_list;

////////////////////////////////////////
// Function prototypes found elsewhere
void update_other_cmos_to_disk(int sleep_allowed);
int set_coin_hold(int flag);
void launch_game_proc(void);
void coin_show_insert_proc(int *);
void coin_show_credits_proc(int *);
int get_total_credits(void);
void start_enable(int);
void backdoor_proc(int *);
unsigned int	get_text_addr(void);
unsigned int	get_text_end_addr(void);

static int pop_team_sort(const void *a, const void *b);
void test_trivia_contest(int min, int max);
void show_player_free_game_test(int min, int max);

////////////////////////////////////////
// Referenced data
extern player_record_info selected_players[];
extern plyr_record_t	*rec_sort_ram;			// pointer to the array of PLAYER RECORDs from cmos into ram
extern world_record_t	tmp_world_rec;
extern int	tsec;
extern char cursor_bnk_str[];
void unload_player_speech_banks(void);
void unload_team_speech_banks(void);


////////////////////////////////////////
// Structure definition of attract mode functions to run
typedef struct att_func_info
{
	void  (*func)(int, int);				// Function name to call
	int   loop_count;						// Loops before function is called
	float min_time;							// Minimum time to display (seconds)
	float timeout;							// Maximum time to display (seconds)
} att_func_info_t;

typedef struct {
	int x, y, w, h;
} flash_pos;

static flash_pos flash_tab[HI_SCORE_ENTRIES];
static void proc_top_sparkle(int *args);

static tmu_ram_texture_t hiscore_textures_tbl[] =
{
 {"nbcbk200.wms",HISCORE_PG_TID},	// 256x256
 {"nbcbk201.wms",HISCORE_PG_TID},
 {"nbcbk202.wms",HISCORE_PG_TID},
 {"optscr00.wms",HISCORE_PG_TID},	// for header bar
 {"hiscr.wms",HISCORE_PG_TID},	// for frames on world record page
 {"hiscr2.wms",HISCORE_PG_TID},	// for flashes on highscore page
 {NULL,0}
};

static char *world_record_titles[] = {
		"MOST REBOUNDS",
		"MOST 3 POINTERS",
		"MOST DUNKS",
		"MOST ASSISTS",
		"MOST STEALS",
		"MOST POINTS IN GAME" };

static float wrld_rec_plqx[NUM_WORLD_RECORDS] = {  // [world rec]
						120.0f,
						120.0f,
						120.0f,
						372.0f,
						372.0f,
						372.0f};

static float wrld_rec_plqy[NUM_WORLD_RECORDS] = {  // [world rec]
						240.0f,
						155.0f,
						70.0f,
						240.0f,
						155.0f,
						70.0f};

static float wrld_rec_xs[NUM_WORLD_RECORDS] = {  // [world rec]
						128.0f,
						128.0f,
						128.0f,
						394.0f,
						394.0f,
						394.0f};
		
static float wrld_rec_title_ys[NUM_WORLD_RECORDS] = {  // [world rec]
						255.0f,
						170.0f,
						85.0f,
						255.0f,
						170.0f,
						85.0f};

static float wrld_rec_ys[NUM_WORLD_RECORDS] = {  // [world rec]
						226.0f,
						141.0f,
						56.0f,
						226.0f,
						141.0f,
						56.0f};

static float hiscr_ys[HI_SCORE_ENTRIES] = {  // [y]
						278.0f,
						249.0f,
						220.0f,
						191.0f,
						162.0f,
						133.0f,
						104.0f,
						 75.0f,
						 46.0f,
						 17.0f };


static char *hi_scr_titles[] = {
		"GRAND CHAMPIONS",
		"GREATEST PLAYERS",
		"BIGGEST WINNERS",
		"BEST OFFENSE",
		"BEST DEFENSE",
		"BEST SHOOTER",
		"BEST DUNKER",
		"BEST WINSTREAK",
		"MOST EXPERIENCED",
		"WORLD RECORDS",
		"TRIVIA MASTERS",
		"POPULAR TEAMS",
		"ALL-STAR CHAMPIONS",
};

static char *hi_scr_subhdrs[] = {
		"(DEFEATED ALL 29 NBA TEAMS)",
		"(BEST WIN PERCENTAGE)",
		"(MOST GAMES WON)",
		"(POINTS SCORED PER GAME)",
		"(POINTS ALLOWED PER GAME)",
		"(3 PT SHOTS MADE)",
		"(DUNKS MADE)",
		"(CONSECUTIVE WINS)",
		"(MOST GAMES PLAYED)",
		"(BEST SINGLE GAME RECORDS)",
		"(MOST TRIVIA POINTS)",
		"(BASED ON THIS LOCATION)",
		"(COMPLETED MIDWAY ALL-STAR CHALLENGE)",
};

static float sub_hdr_xys[2] = {256.0f,300.0f};

//static image_info_t *hiscr_hdr_imgs[] = {
//		 &hs_grndch,&hs_grndch_c1,
//		 &hs_grtplr,&hs_grtplr_c1,
//		 &hs_bigwin,&hs_bigwin_c1,
//		 &hs_bstoff,&hs_bstoff_c1,
//		 &hs_bstdef,&hs_bstdef_c1,
//		 &hs_bstrus,&hs_bstrus_c1,
//		 &hs_bstpas,&hs_bstpas_c1,
//		 &hs_curwin,&hs_curwin_c1,
//		 &hs_expplr,&hs_expplr_c1,
//		 &hs_qbrate,&hs_qbrate_c1,
//		 &hs_wrldrc,&hs_wrldrc_c1,
//		 &hs_trivia,&hs_trivia_c1,
//		 &hs_poptms,&hs_poptms_c1 };

extern int do_grand_champ;
extern plyr_record_t * plyr_data_ptrs[];
extern int record_me;

static ostring_t * hstrings[3][HI_SCORE_ENTRIES];

static int simple_cycle_state = 0;
static int show_hilite;

static int simple_cycle_table[] = {
0xffffff00,
0xffffff1f,
0xffffff3f,
0xffffff5f,
0xffffff7f,
0xffffff9f,
0xffffffbf,
0xffffffdf,
0xffffffff,
0xffffffdf,
0xffffffbf,
0xffffff9f,
0xffffff7f,
0xffffff5f,
0xffffff3f,
0xffffff1f
};

static int simple_hilite_table[] = {
0xff0000ff,
0xff1f1fff,
0xff3f3fff,
0xff5f5fff,
0xff7f7fff,
0xff9f9fff,
0xffbfbfff,
0xffdfdfff,
0xffffffff,
0xffdfdfff,
0xffbfbfff,
0xff9f9fff,
0xff7f7fff,
0xff5f5fff,
0xff3f3fff,
0xff1f1fff
};

#define RATE1	5
#define RATE2	3


static popular_team_info_t	pop_team[NUM_TEAMS] = {
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0}
};

static char	*showtime_movie[] = {
"shwt", 0
};
char showtime_movie_snd_bnk_str[] = "mov_shwt";

static sndinfo showtime_movie_snds[] = {
	{showtime_movie_snd_bnk_str, 0, VOLUME7, 127, PRIORITY_LVL5},
	{NULL}
};

static char	*trophy_movie[] = {
"tphy", 0
};
char trophy_snd_bnk_str[] = "mov_endv";

static sndinfo trophy_movie_snds[] = {
	{trophy_snd_bnk_str, 0, VOLUME7, 127, PRIORITY_LVL5},
	{NULL}
};

static char	*midway_logo_movie[] = {
"mdwy",
0
};

char midway_logo_movie_snd_bnk_str[] = "mov_midw";

static sndinfo midway_logo_movie_snds[] = {
	{midway_logo_movie_snd_bnk_str, 0, VOLUME7, 127, PRIORITY_LVL5},
	{NULL}
};

static char	*nbc_logo_movie[] = {
"atts",
0
};

char nbc_logo_movie_snd_bnk_str[] = "mov_atts";

static sndinfo nbc_logo_movie_snds[] = {
	{nbc_logo_movie_snd_bnk_str, 0, VOLUME4, 127, PRIORITY_LVL5},
	{NULL}
};

static char	*eog_movie[] = {
"eog1",
0
};

char eog_movie_snd_bnk_str[] = "mov_elvi";

static sndinfo eog_movie_snds[] = {
	{eog_movie_snd_bnk_str, 0, VOLUME6, 127, PRIORITY_LVL5},
	{NULL}
};

////////////////////////////////////////
// Function prototypes for attract mode functions
void attract_loop(int *);
void credit_page(int, int);
void credit_page_proc( int *args);
void wait_for_any_butn_press_active_team(int, int, int);
void wait_for_any_butn_press_active_plyr(int, int, int);
int wait_for_any_butn_press(int, int);
void logo_3DFX_screen(int, int);
void logo_dcs_screen(int min_time, int max_time);
void wedding_photo_screen(int min_time, int max_time);
void nbalogo_screen(int min_time, int max_time);
static void fade_strings(int up_id, int down_id);
void legal_text_screen(int min_time, int max_time);
void show_attract_movie(char **movie, sndinfo *sounds, int allow_btn, int use_chroma_key, int ckey, int sound_delay, int wait_for_start, int kill_sound);
void show_midway_logo_movie(int min_time, int max_time);
void show_showtime_movie(int min_time, int max_time);
void title_screen(int min_time, int max_time);
void nbc_sports(int min_time, int max_time);
void test_font(int min_time, int max_time);
void do_amode_play( int min_time, int max_time );

void show_grand_champ_movie(int, int);

void design_credits(int min_time, int max_time);

struct process_node *coin_insert_proc;
struct process_node *coin_credits_proc;
int	show_them_hiscore_tables_first = 0;

#define APTIME	90.0f

#ifdef LONG_APLAY
#undef APTIME
#define APTIME	10000.0f
#endif

////////////////////////////////////////
// Functions to call during attract mode
//
static att_func_info_t	att_funcs[] = {
//{show_player_free_game_test, 1,   0.0,     0.0},
#ifdef DBG_TC
 {show_grand_champ_movie,  1,     0.0,     0.0},
//         {design_credits,  1,     0.0,     0.0},
//    {test_trivia_contest,  1,     0.0,     0.0},
#endif
         	 {nbc_sports,  1,     0.3,     4.0},
	       {title_screen,  1,     0.3,     4.0},
         {nbalogo_screen,  1,     0.5,     9.0},
	{ sweeps_attract, 1, 0.3, 6.0 },

#ifndef NOMOVIES
 {show_midway_logo_movie,  1,     0.0,     0.0},	// movie doesn't use timers
    {show_showtime_movie,  2,     0.0,     0.0},	// movie doesn't use timers
#endif
          {do_amode_play,  1,     1.0,  APTIME},
		{ sweeps_attract, 1, 0.3, 6.0 },
            {credit_page,  1,    12.0,     0.0},	// min is also max


    {show_hiscore_tables,  1,     0.0,     0.0},	// hiscore doesn't use timers
          {do_amode_play,  1,     1.0,  APTIME},
            {credit_page,  1,    12.0,     0.0},	// min is also max
//      {logo_dcs_screen,  1,     0.3,     4.0},
   {wedding_photo_screen,  5,     0.5,     7.0},
       {logo_3DFX_screen,  1,     0.3,     4.0},
      {legal_text_screen,  1,     0.3,     7.0}
//            {test_font,  1,    12.0,     0.0},
};
#define ATT_FUNC_CNT	((int)(sizeof(att_funcs) / sizeof(att_func_info_t)))

// static local data
static int att_keeper;				// Make attract function leave it up

#if 0
//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
void design_credits(int min_time, int max_time)
{
	extern BBDATA	bbdata[2];
	void player_proc_a( int *parg );
	void draw_goals( void * );
	void draw_court( void * );
	void init_drones( void );				// from drone.c
	void find_all_star_team(int *p1_team, int *p1_index, int *p2_team, int *p2_index);

	extern int last_shooter;
	extern int away_team_style;
	extern int players_ready;
	extern int ladder_level;
	int i;

	int p1_team = 0;
	int p2_team = 1;
	int p1_index= 0;
	int p2_index= 1;

	ladder_level = 2;

	find_all_star_team(&p1_team, &p1_index, &p2_team, &p2_index);
	set_player_record(p1_team, p1_index, FALSE, &selected_players[0]);
	set_player_record(p1_team, p2_index, FALSE, &selected_players[1]);
	set_player_record(p2_team, p1_index, FALSE, &selected_players[2]);
	set_player_record(p2_team, p2_index, FALSE, &selected_players[3]);


	if (randrng(100) <= 20)
		away_team_style = ALTERNATE_JERSEY;
	else
		away_team_style = AWAY_JERSEY;

	game_info.game_mode    = GM_INITIALIZING;
	game_info.off_side     = 0;
	game_info.poss_change  = 0;

	game_info.ball_handler = NO_BALLHANDLER;
	game_info.shooter      = NO_SHOOTER;
	game_info.passer       = NO_PASSER;
	game_info.sc_scored		 = NO_SHOOTER;
#if NUM_PERTEAM > 2
	game_info.receiver_a   = NO_RECEIVER;
	game_info.receiver_b   = NO_RECEIVER;
#endif
	game_info.ball_mode    = BM_FREE_ON_COURT;

	game_info.fire_player  = -1;
	game_info.fire_cnt     = 0;
	game_info.fire_timeout = -1;
	
	game_info.team_fire = -1;
	game_info.tm_fire_cnt[0] = 0;
	game_info.tm_fire_cnt[1] = 0;

	game_info.active_goal = 1;
	game_info.off_goal    = 1;


	// clear scores
////	for (i=0; i < MAX_QUARTERS; i++)
////	{
////		game_info.scores[0][i] = 0;
////		game_info.scores[1][i] = 0;
////	}
////
////	// clear foul counters
////	for (i=0; i < NUM_PLAYERS; i++) {
////		game_info.foul_cnt[i] = 0;
////		game_info.hotspot_made_cnt[i] = 0;
////		pushed_cnt[i] = 0;
////	}

	// clear sound system stuff
////	gSndTimer = 0;
////	gWaitSayName = 0;
	last_shooter = -1;

////////////////////////////////////////
// Enable the disk interrupt callback
//
////	dio_mode(DIO_MODE_BLOCK);
////	dio_mode(DIO_MODE_NO_BLOCK);
////
////	// load game textures....
////	load_ingame_textures();
////
////	//load_arena_textures();
////
////	Arena_proc(parg);

	dio_mode(DIO_MODE_BLOCK);

	load_plyr_txtrs(game_info.team[0], game_info.team[1]);
	load_referee_txtrs();

//	/* honor the big head and drone big head privs */
//	for (i = 0; i < MAX_PLYRS; i++)
//	{
//		if ((p_status & (1 << i)) && (selected_players[i].record_type == CREATP_RECORD))
//		{
//			if (selected_players[i].cmos_data.privileges & PRIV_BIG_HEAD)
//			{
//				pup_bighead |= 1 << i;
//			}
//			if (selected_players[i].cmos_data.privileges & PRIV_DRONE_BIG_HEAD)
//			{
//				teammate = 1 ^ i;
//				if ((p_status & (1 << teammate)) == 0)
//				{
//					pup_bighead |= 1 << teammate;
//				}
//			}
//		}
//	}
//	/* honor the stealth turbo priv */
//	priv_stealth_turbo = 0;
//	for (i = 0; i < MAX_PLYRS; i++)
//	{
//		if ((p_status & (1 << i)) && (selected_players[i].record_type == CREATP_RECORD))
//		{
//			if (selected_players[i].cmos_data.privileges & PRIV_STEALTH_TURBO)
//			{
//				priv_stealth_turbo |= 1 << i;
//			}
//		}
//	}
//	
//	/* set the show shot % for your drone */
//	for (i = 0; i < MAX_PLYRS; i++)
//	{
//		if ((pup_show_shotper & (1 << i)) && (p_status & (1 << i)))
//		{
//			teammate = 1 ^ i;
//			if ((p_status & (1 << teammate)) == 0)
//			{
//				pup_show_shotper |= 1 << teammate;
//			}
//		}
//	}
//
//	force_special_weather();
//
//	/* honor the head size adjustment */
//	if (!get_adjustment(PLAYER_HEAD_SIZE_ADJ, &val) && val)
//		pup_bighead = 0xF;
//
//	// create the Arena
////	iqcreate( "Arena", FIELD_PID, Arena_proc, 0, 0, 0, 0 );
//
//
//
//	// Create the drawing object to draw the arena
//	create_object("court", OID_FIELD, OF_NONE, DRAWORDER_FIELD, NULL,   draw_court);
//	create_object("goals", OID_GOALS, OF_NONE, DRAWORDER_GOALS, bbdata, draw_goals);
//
//	do_court_fog();


	// create the ball
	set_process_run_level( qcreate( "ball", BALL_PID, ball_proc, 0, 0, 0, 0 ), RL_BALL );

	// create the camera
	set_process_run_level( qcreate( "cambot", CAMBOT_PID, cambot_proc, 0, 0, 0, 0 ), RL_CAMBOT );

	players_ready = 0;

	for (i = 0; i < NUM_PLAYERS; i++)
	{
		int which_model;
		
		if (selected_players[i].team == TEAM_SECRET)
			which_model = selected_players[i].player_index;
		else
			which_model = NBA_PLAYER;
		load_player_head(FALSE, i, selected_players[i].team, selected_players[i].player_index,
			&player_load_info[i].head_model, &player_load_info[i].head_scale);
		load_player_model(which_model, &player_load_info[i].body_model,
			&player_load_info[i].jnum_model, &player_load_info[i].reflect_model);
	}
	load_player_head(FALSE, NUM_PLAYERS, TEAM_SECRET, REF_CHARACTER,
			&player_load_info[NUM_PLAYERS].head_model, &player_load_info[NUM_PLAYERS].head_scale);
	load_player_model(REF_CHARACTER, &player_load_info[NUM_PLAYERS].body_model,
			&player_load_info[NUM_PLAYERS].jnum_model, &player_load_info[NUM_PLAYERS].reflect_model);

	// create the players -- use iqcreate because we want the game process
	// to happen before the player processes every tick.
	//								plyrnum, team, position, unused
	set_process_run_level( qcreate( "pre_plyr", PLAYER_PID+1, pre_player_proc,  0, 0, 0, 0 ), RL_PLAYER-1);
	set_process_run_level( qcreate( "lastplyr", PLAYER_PID+1, last_player_proc, 0, 0, 0, 0 ), RL_LASTPLAYER);

//fprintf(stderr,"selected_players = %d %d %d %d\n",selected_players[0],selected_players[1],selected_players[2],selected_players[3]);

	for (i = 0; i < NUM_PLAYERS; i++)
	{
		static int position[] = { GUARD, FORWARD, CENTER };
		static char szpn[] = "plyr_0";
		szpn[sizeof(szpn)-2] = '0' + (char)i;
		set_process_run_level(
			qcreate(szpn, PLAYER_PID, /*simplified_*/player_proc_a,
				i,
				(i >= NUM_PERTEAM),
				position[i % NUM_PERTEAM],
				selected_players[i].player_index),
			RL_PLAYER+i);
	}

	// Create the cursors; proc dies after making the cursor textures & objs
//	qcreate( "cursors", 0, cursor_proc, 0, 0, 0, 0 );
//	if (!pup_no_hotspots)
//		qcreate( "shwhtspt", 0, show_hotspot_proc, 0, 0, 0, 0 );
//	//set_process_run_level( qcreate( "Shadows", 0, Player_Shadow_Proc, 0, 0, 0, 0 ), RL_CURSORS );

//	// Create various clock & status box procs
//	// <set_process_run_level> in this order so clock gets inited first
//	set_process_run_level( qcreate("gameclk",  PLAYER_PID,     game_clock_proc,     0, 0, 0, 0), RL_FIRSTUP+0);
//	set_process_run_level( qcreate("shtclk",   PLAYER_PID,     shot_clock_proc,     0, 0, 0, 0), RL_FIRSTUP+1);
//	set_process_run_level( qcreate("shtclkbx", STATUS_BOX_PID, shot_clock_box_proc, 0, 0, 0, 0), RL_FIRSTUP+2);
//	set_process_run_level( qcreate("score",    STATUS_BOX_PID, score_status_box,    0, 0, 0, 0), RL_FIRSTUP+3);

//	// initialize player info displays
//	plyrinfo_init();
//
//	// Create score plate box & NBC logo
//	qcreate( "scrplate", SCR_PLATE_PID, score_plate_proc,  0, 0, 0, 0);
//	qcreate( "peacock",  SCR_PLATE_PID, show_peacock_proc, 0, 0, 0, 0);
//
//	//qcreate( "plyrnms", PLYRINFO_PID, show_player_names_proc, 0, 0, 0, 0 );


	// Let everything init itself first
	sleep(2);

init_drones();
	players_ready = 1;

	// Enable drawing
	draw_enable(1);

	// Make sure we are at full intensity if a fade was happening
	// when this got started
	normal_screen();

	while(!get_player_sw_current())
	{
		grColorCombine (
			GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL,
			GR_COMBINE_FACTOR_ONE,
			GR_COMBINE_LOCAL_CONSTANT,
			GR_COMBINE_OTHER_TEXTURE,
			0);

		sleep(1);
	}

	draw_enable(0);

	wipeout();
}
#endif //0

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
void attract_exit(struct process_node *ppn, int reason)
{
	/* can not do this here, will kill the first coin in sound going into coin page */
//	snd_stop_all();
	
	// Stop any replay thats running
	set_playback_mode(RP_STOP, 0, 0, 0);

	// remove the player sound banks
	unload_team_speech_banks();
	unload_player_speech_banks();
	ptl_shutdown();
	// remove the player models
	unload_player_models();
	unload_player_heads();
}

//---------------------------------------------------------------------------------------------------------------------------
// 			THIS is the PROCESS that CONTROLS the ATTRACT MODE
//
// INPUT: none
// 
// called from MAIN.C
//---------------------------------------------------------------------------------------------------------------------------
void attract_loop(int *args)
{
	static int attract_loop_count = 0;
	unsigned int	start_addr;
	unsigned int	num_bytes;
	unsigned long	crc32;
	void * att_next;
	float ticks_per_second;
	int count, fn;

	// Set up our demise
	cur_proc->process_exit = attract_exit;

	if (!existp(BACKDOOR_PID,0xFFFFFFFF)) {
		// Create the backdoor to the diagnostics
		qcreate("bdoor", BACKDOOR_PID, backdoor_proc, 0, 0, 0, 0);
	}

	wipeout();
	snd_stop_track(SND_TRACK_0);		// stop game over music

	// Make start buttons active
	start_enable(TRUE);

	/* set attract mode master volume level */
	snd_master_volume(game_info.attract_mode_volume);
	/* potentially disable attract mode sounds */
	get_adjustment(ATTRACT_SOUND_ADJ, &fn);
	snd_bank_lockout(!fn);

	count = snd_get_first_sound(cursor_bnk_str);

	tick_sndn = count + TICK_SND;
	coin_snd_drop_calln = count + COIN_SND_DROP_CALL;
	coin_snd_paid_calln = count + COIN_SND_PAID_CALL;

	// Set game state
	game_info.game_state = GS_ATTRACT_MODE;

	// snd bank nedded at game start is loaded at game_over in nfl.c
	// load in the needed sounds

	// Initialize the attract mode switch handlers

	credit_pg_on = 0;
	count = 0;
	att_keeper = 0;
	ticks_per_second = (float)get_tsec();

//	pre_load_opt_scr_tex();
//	pre_load_movie_tex("star");

	draw_enable(1);

//  eloff's sound previewer via serial port
//	dcs_shell();
	// Are we supposed to show the hiscore tables first
	if(show_them_hiscore_tables_first)
	{
		// YES
		show_hiscore_tables(0, 0);
	}

	while(game_info.game_state == GS_ATTRACT_MODE)
	{
		// free up any heap that is sitting on the free process list
		clear_free_process_list();
		clear_free_sprite_list();

		// Generate the crc for the executable in RAM
		start_addr = get_text_addr();
		num_bytes = get_text_end_addr();
		num_bytes -= start_addr;
		crc32 = crc((unsigned char *)start_addr, num_bytes);
		if(crc32 != start_crc32) {
#ifdef DEBUG
			fprintf(stderr, "Text section is trashed\r\n");
			lockup();
#else
			int	restart_args[] = {0, 0, 0, 0};

//			increment_audit(RESTART_AUD);
//			increment_audit(WATCHDOG_AUDIT_NUM);

			write_cmos_to_disk(FALSE);
			update_other_cmos_to_disk(FALSE);
			exec("game.exe", 0xdeadbeef, restart_args);
#endif
		}

		write_cmos_to_disk(TRUE);
		update_other_cmos_to_disk(TRUE);

#ifndef TEST_EXEC
		// Loop through all of the attract mode screens
		for (fn = 0; fn < ATT_FUNC_CNT; fn++)
		{
			if (!fn)
				attract_loop_count++;
			att_next = att_funcs[(fn == (ATT_FUNC_CNT-1)) ? 0 : (fn+1)].func;

			// only do if in attract mode
			if (game_info.game_state != GS_ATTRACT_MODE)
				break;
		
			// Check to see if it is time to draw this screen
			if (!(attract_loop_count % att_funcs[fn].loop_count))
			{
//				att_keeper = (int)(att_next == do_amode_play);
				att_keeper = 0;

//				fprintf(stderr,"Heap goin' in --> %d\r\n",get_heap_available());
				// Call the function to draw this screen
				att_funcs[fn].func(
					(int)(ticks_per_second * att_funcs[fn].min_time),
					(int)(ticks_per_second * 
					(att_funcs[fn].timeout - att_funcs[fn].min_time)));
//				fprintf(stderr,"   Comin' out --> %d\r\n\r\n",get_heap_available());
					
				// if credit page is up...wait until off before goto next page
				while (credit_pg_on)
					sleep(2);
				// Sweeps Check!
				if ((get_joy_val_cur(0) & JOY_DOWN) ||
					(get_joy_val_cur(1) & JOY_DOWN) ||
					(get_joy_val_cur(2) & JOY_DOWN) ||
					(get_joy_val_cur(3) & JOY_DOWN))
				{
					qcreate( "sweeps", AMODE_GRP_PID, sweeps_proc, 0, 0, 0, 0 );			
					die (0);
				}
			}
		}

#else //TEST_EXEC
		fprintf(stderr,"--> Setting count = %d to do an EXEC\r\n",LOOPS_UNTIL_REBOOT);
		attract_loop_count = LOOPS_UNTIL_REBOOT;
		sleep(1);
#endif //TEST_EXEC

		// Check game select & game lock DIPs
		if (((get_dip_coin_current() & DIP_BIT_6) == 0 ||	// 0 if no lock, 1 if lock
			(get_dip_coin_current() & DIP_BIT_7) == 1) &&	// 0 if NFL, 1 if NBA
			(game_info.game_state == GS_ATTRACT_MODE))
		{
			if (attract_loop_count == LOOPS_UNTIL_REBOOT)
			{
				// REBOOT PLEASE!!!!
				#ifdef DEBUG
					fprintf( stderr,"REBOOT!!!\n");
				//	while(1) sleep(1);
				#endif
				{
					int args[4] = { 2, 0, 0, 0};

					write_cmos_to_disk(FALSE);
					update_other_cmos_to_disk(FALSE);

					set_coin_hold(TRUE);
					snd_master_volume(0);
					setdisk(1);		// Set NFL Partition
					write_game_info();		// Write out valuable info
					exec("GAME.EXE", 0xfeedbeef, args);
				}
			}
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
void coin_credits_exit_func(process_node_t *, int);

void credit_page(int min_time, int max_time)
{
	int prev_cred;
	sprite_info_t *bkgrnd_obj;
	
#if 0
	stream_stop();
	do { sleep (1); } while (stream_check_active());
	stream_release_key();
#endif
	
	wipeout();

	/* this is OK, the NBC logo sound bank lasts longer than the movie, it plays over the high score pages */
	// Fix!!!  Maybe.  We need to clear up movie snd banks that play in amode when
	// we come to credit page - normally, the bank is cleared up when the movie finishes

	snd_bank_delete(showtime_movie_snd_bnk_str);
	snd_bank_delete(trophy_snd_bnk_str);
	snd_bank_delete(midway_logo_movie_snd_bnk_str);
	snd_bank_delete(nbc_logo_movie_snd_bnk_str);
	snd_bank_delete(eog_movie_snd_bnk_str);

	if (rec_sort_ram) {
		free(rec_sort_ram);
		rec_sort_ram = NULL;
	}

	// create the insert coins message proc
	coin_insert_proc = qcreate("coinmesg",ZERO_PID,coin_show_insert_proc,0,0,0,0);

	// create the show player credits message proc
	coin_credits_proc = qcreate("credmesg",ZERO_PID,coin_show_credits_proc,0,0,0,0);
	coin_credits_proc->process_exit = coin_credits_exit_func;

	// let procs do disk loads so fade won't be interrupted or text won't
	//  appear after background
	sleep(1);

	draw_enable(1);

	credit_pg_on = 1;

	bkgrnd_obj = beginobj(&nbcbk2,    0.0F, SPRITE_VRES, 140.0f, 0xbeef);
	bkgrnd_obj = beginobj(&nbcbk2_c1, 0.0F, SPRITE_VRES, 140.0f, 0xbeef);
	bkgrnd_obj = beginobj(&nbcbk2_c2, 0.0F, SPRITE_VRES, 140.0f, 0xbeef);
	bkgrnd_obj = beginobj(&nbcbk2_c3, 0.0F, SPRITE_VRES, 140.0f, 0xbeef);
	
	fade(1.0, 10, NULL);
	
	do {
		prev_cred = get_total_credits();

		wait_for_any_butn_press(76, min_time);

	}	while (prev_cred != get_total_credits());
		
	// clean_up:
	delete_all_sprites();

	kill(coin_insert_proc,0);
	kill(coin_credits_proc,0);
	delete_multiple_strings(5, 0xfffffff);

	// delete background
	del1c(0,0xFFFFFFFF);

	credit_pg_on = 0;
}

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
extern int texture_load_in_progress;

void credit_page_proc( int *args)
{
	if (game_info.game_state == GS_ATTRACT_MODE && !credit_pg_on)
	{
		credit_pg_on = 1;

		start_enable(TRUE);		// Problem during rebooting

		// Make sure <game_proc> finishes loads &| dies
		if (existp(GAME_PROC_PID, 0xFFFFFFFF) && texture_load_in_progress)
			game_info.attract_coin_in = 1;

		// Make sure the movie player is shut down if it is running
		if(movie_get_state() > 1)
		{
			movie_abort();
		}

		// Do not kill any disk loads
		while (texture_load_in_progress) sleep(1);
		// Make sure the disk is in normal mode
		dio_mode(DIO_MODE_BLOCK);
		//sleep(1);

		// Make sure the we fade back to normal if we came here from
		// a screen that faded.
		normal_screen();

		// Make sure any transitions go away
		delete_process_id(TRANSIT_PID);
		kill_plates();

		// Show the page
		credit_page(12*get_tsec(),0);

		// Restart attract mode
		iqcreate("attract", AMODE_GRP_PID, attract_loop, 0, 0, 0, 0);
	}
}

//-------------------------------------------------------------------------------------------------
// Wait for a minimum # of ticks, then read buttons & joystick for x ticks then return
//  rtns: 0 if timed-out, !0 if buttoned-out
//-------------------------------------------------------------------------------------------------
int wait_for_any_butn_press(int min_ticks, int wait_ticks)
{
	int switches = 0;
	
	while (min_ticks--) sleep (1);

	while (wait_ticks--)
	{
		switches = get_player_sw_current();
		if (switches & ALL_ABCD_MASK && !(switches & ALL_D_MASK))
		{
//			if (!show_plysel_idiot_box)
				snd_scall_direct(tick_sndn,VOLUME2,127,1);
			break;
		}
		// Clr 'cuz no valid buttons were hit
		switches = 0;
		// Sleep here so we don't follow the <min_tick> sleep
		sleep (1);
	}
	return switches;
}

#if 0	//UNUSED!!!
//---------------------------------------------------------------------------------------------------------------------------
// Wait for any butn press of an active player on a team
// Used for player specific bozo boxes
// Checks to make sure guy is in game also
//---------------------------------------------------------------------------------------------------------------------------
void wait_for_any_butn_press_active_team(int min, int max, int pnum)
{
	int ticks = 0, done = FALSE;

	do
	{
		sleep(1);
		ticks++;
		if (pnum == 0)
		{
			if ((p_status & 0x1) && (get_player_sw_current() & (P1_A|P1_B|P1_C)) && (ticks >= min))
				done = TRUE;
			if (four_plr_ver)
			{
				if ((p_status & 0x2) && (get_player_sw_current() & (P2_A|P2_B|P2_C)) && (ticks >= min))
					done = TRUE;
			}
		}
		else if (pnum == 1)
		{
			if (four_plr_ver)
			{
				if ((p_status & 0x4) &&	(get_player_sw_current() & (P3_A|P3_B|P3_C)) && (ticks >= min))
					done = TRUE;
				if ((p_status & 0x8) &&	(get_player_sw_current() & (P4_A|P4_B|P4_C)) && (ticks >= min))
					done = TRUE;
			}
			else
			{
				if ((p_status & 0x2) && (get_player_sw_current() & (P2_A|P2_B|P2_C)) && (ticks >= min))
					done = TRUE;
			}
		}
	} while (ticks < max && !done);
}
#endif	//UNUSED!!!

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
void wait_for_any_butn_press_active_plyr(int min, int max, int pnum)
{
	int ticks = 0;

	do
	{
		sleep(1);

		if ((p_status & (1<<pnum)) && (ticks >= min))
			if (get_but_val_cur(pnum) & (A_BUTTON|B_BUTTON|C_BUTTON))
				break;

	} while (++ticks < max);
}

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
void show_midway_logo_movie(int min_time, int max_time)
{
	show_attract_movie(midway_logo_movie,  midway_logo_movie_snds, 1, FALSE, 0x000000, 12, FALSE, TRUE);
}

void show_showtime_movie(int min_time, int max_time)
{
	show_attract_movie(showtime_movie,  showtime_movie_snds, 1, FALSE, 0x000000, 12, FALSE, TRUE);
}

void show_attract_movie(char **movie, sndinfo *sounds, int allow_btn, int use_chroma_key, int ckey, int sound_delay, int wait_for_start, int kill_sound)
{
	int	wait_time = 120;		// 2 seconds
	int hold_count;
	int movie_sound, attract_sound;
	
	get_adjustment(MOVIE_SND_ADJ, &movie_sound);
	if (movie_sound) {
		get_adjustment(ATTRACT_SOUND_ADJ, &attract_sound);
		if (!attract_sound) {
			snd_bank_lockout(FALSE);
			movie_sound = -1;
		}
	}

	hold_count = use_chroma_key ? 0 : 30;
	movie_config_player(512.0f, 384.0f, 0.0f, 0.0f, 1.0f, 30);

	qcreate("mplay", 0, movie_proc, (int)movie, 0, hold_count, FALSE);

	fade(1.0, 20, NULL);

	if (use_chroma_key || wait_for_start) {
		draw_enable(0);
		while (--wait_time > 0) {
			if (movie_get_state() == MOVIE_STATE_RUN) {
				if (use_chroma_key) {
					movie_set_chromakey_mode(TRUE); 
					movie_set_chromakey(ckey);
				}
				break;
			}
			sleep(1);
		}
		draw_enable(1);
	}
	
	if (sounds && sounds->bank_name) {
		if (snd_bank_load(sounds->bank_name) == 0xeeee)
			increment_audit(SND_LOAD_FAIL_AUD);
		snd_set_reserved(SND_TRACK_0);
		if (sound_delay > 0)
			sleep(sound_delay);
		snd_scall_bank(sounds->bank_name,
			sounds->scall,
		    sounds->volume,
		    sounds->pan,
	   	 sounds->priority);
	}

	// Make 'em watch it for at least 1/2 second
	sleep(30);

	// As long as the movie is playing and the player has NOT pressed a button
	// do nothing
	if (!allow_btn)
	{
		while(movie_get_state() > 1)
		{
			if (movie_get_state() == MOVIE_STATE_HOLD) {
				fade(0.0f, 30, NULL);
				sleep(30);
			}
			sleep(1);
		}
	}
	else
	{
		while(!(get_player_sw_current() & (P1_ABCD_MASK|P2_ABCD_MASK|P3_ABCD_MASK|P4_ABCD_MASK)) &&
				!(get_dip_coin_current() & (P1_START_SW|P2_START_SW|P3_START_SW|P4_START_SW)) &&
				(movie_get_state() > 1))
		{
			if (movie_get_state() == MOVIE_STATE_HOLD) {
				fade(0.0f, 30, NULL);
				sleep(30);
			}
			sleep(1);
		}
	}

	if (((movie_get_state() > 1) || kill_sound) && sounds && sounds->bank_name)
	{
		snd_stop_all();
		snd_set_reserved(SND_TRACK_0);
		snd_bank_delete(sounds->bank_name);
	}

	// Make a beep if aborted
	if(movie_get_state() > 1)
	{
		snd_scall_direct(tick_sndn,VOLUME2,127,1);
		//snd_scall_bank(cursor_bnk_str,TICK_SND,VOLUME1,127,1);
	}

	// Make sure the movie player is shut down if it is running
	// This happens when the player whacks through the movie
	if(movie_get_state() > 1)
	{
		movie_abort();
	}

	if (movie_sound == -1)
		snd_bank_lockout(TRUE);
}

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
void logo_3DFX_screen(int min_time, int max_time)
{
	sprite_info_t *bkgrnd_obj;
	
	wipeout();
	
	bkgrnd_obj = beginobj(&l3dfx, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&l3dfx_c1, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&l3dfx_c2, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&l3dfx_c3, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	
	black_screen();
	
	sleep(2);
	fade(1.0f, 30, 0);		/* 1=full on 20=ticks */
	
	wait_for_any_butn_press(min_time,max_time);
	
	fade(0.0f, 30, 0);		/* 0=full off */
	sleep(30);
	
	delete_all_sprites();
}	/* logo_3DFX_screen */

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
void logo_dcs_screen(int min_time, int max_time)
{
	sprite_info_t *bkgrnd_obj;
	
	wipeout();
	
	bkgrnd_obj = beginobj(&dcsscrn, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&dcsscrn_c1, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&dcsscrn_c2, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&dcsscrn_c3, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	
	black_screen();
	
	sleep(2);
	fade(1.0f, 30, 0);		/* 1=full on 20=ticks */
	
	wait_for_any_butn_press(min_time,max_time);
	
	fade(0.0f, 30, 0);		/* 0=full off */
	sleep(30);
	
	delete_all_sprites();
}	/* logo_dcs_screen */

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
void wedding_photo_screen(int min_time, int max_time)
{
	sprite_info_t *bkgrnd_obj;
	
	wipeout();
	
	bkgrnd_obj = beginobj(&splash3, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&splash3_c1, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&splash3_c2, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&splash3_c3, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	
	black_screen();
	
	sleep(2);
	fade(1.0f, 30, 0);		/* 1=full on 20=ticks */
	
	wait_for_any_butn_press(min_time,max_time);
	
	fade(0.0f, 30, 0);		/* 0=full off */
	sleep(30);
	
	delete_all_sprites();
}	/* wedding_photo_screen */

void nbalogo_screen(int min_time, int max_time)
{
	sprite_info_t *bkgrnd_obj;
	
	wipeout();
	
	bkgrnd_obj = beginobj(&logopage, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&logopage_c1, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&logopage_c2, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&logopage_c3, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	
	black_screen();
	
	sleep(2);
	fade(1.0f, 30, 0);		/* 1=full on 20=ticks */
	
	wait_for_any_butn_press(min_time,max_time);
	
	fade(0.0f, 30, 0);		/* 0=full off */
	sleep(30);
	
	delete_all_sprites();
}	/* nbalogo_screen */

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
void title_screen(int min_time, int max_time)
{
	sprite_info_t *bkgrnd_obj;
	int width, height;
	
	// Disable drawing
	draw_enable(0);

	wipeout();
	
	bkgrnd_obj = beginobj(&splash1, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&splash1_c1, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&splash1_c2, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&splash1_c3, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&VER20, 96, 60, 400.0f, BARS_PG_TID);	//66 29
	
	// Take care of proper handling of transition
	if (plates_on != PLATES_OFF)
	{
		// Re-create the wipeout'd plate objects
//		create_plates();

		// Enable drawing since we know something is being or will be drawn
		draw_enable(1);

		turn_transit_off(0);
	}
	else
	{	
		// Enable drawing
		draw_enable(1);

		black_screen();

		sleep(2);
		fade(1.0f, 30, 0);		//1=full on 30=ticks
	}
	
	sleep(30);
	width = ((83 + (83 / 4)) / 52.0F) * 100.0F;
	height = (20 / 6.0F) * 100.0F;
	qcreate("topspark", SIMPLE_CYCLE_PROC_ID, proc_top_sparkle, 96, 60, width | (50 << 24), height);
	snd_scall_bank(cursor_bnk_str, SELECT_SND1, VOLUME2, 127, PRIORITY_LVL4);
	wait_for_any_butn_press(min_time,max_time);
	
	fade(0.0f, 30, 0);		/* 0=full off */
	sleep(30);
	
	delete_multiple_strings(5, 0xffffffff);
	delete_all_sprites();
}	/* title_screen */



//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
void nbc_sports(int min_time, int max_time)
{
	sprite_info_t *bkgrnd_obj;
	int save_lockout;
	
	// Disable drawing
	draw_enable(0);

	wipeout();
	
	bkgrnd_obj = beginobj(&nbcsports, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&nbcsports_c1, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&nbcsports_c2, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&nbcsports_c3, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);

	// Take care of proper handling of transition
	if (plates_on != PLATES_OFF) {
		// Enable drawing since we know something is being or will be drawn
		draw_enable(1);

		turn_transit_off(0);
	} else {	
		// Enable drawing
		draw_enable(1);
		
		if (game_just_finished) {
			game_just_finished = 0;
			/* allow the movie sound in spite of the attract mode sounds enabled adjustment */
			save_lockout = snd_get_bank_lockout();
			snd_bank_lockout(FALSE);
			show_attract_movie(eog_movie,  eog_movie_snds, 1, FALSE, 0x000000, 0, TRUE, TRUE);
			snd_bank_lockout(save_lockout);
		}
		
		black_screen();

		sleep(2);
		fade(1.0f, 30, 0);		//1=full on 30=ticks
	}
	
	wait_for_any_butn_press(min_time,max_time);
	
	fade(0.0f, 30, 0);		/* 0=full off */
	sleep(30);
	
	delete_multiple_strings(5, 0xffffffff);
	delete_all_sprites();
}	/* title_screen */



//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
static void proc_top_sparkle(int *args)
{
	static image_info_t *top_sparks[] = {&colfla01, &colfla02, &colfla03, &colfla04,
										 &colfla05, &colfla06, &colfla07, &colfla08,
										 &colfla09, &colfla10, &colfla11, &colfla12,
										 &colfla13, &colfla14
	};
	float w_scale, h_scale, z_level;
	int i;
	sprite_info_t *the_sprite;
	
	z_level = 2.0f + ((float)(args[2] >> 24)) / 10.0F;
	w_scale = (float)(args[2] & 0x00FFFFFF) / 100.0f;
	h_scale = (float)args[3] / 100.0f;
	the_sprite = beginobj(top_sparks[0], args[0], args[1], z_level, HISCORE_PG_TID);
	the_sprite->id = 0xFEEDABC;
	/* set to additive blending */
	the_sprite->state.alpha_rgb_dst_function = GR_BLEND_ONE;
	the_sprite->w_scale = w_scale;
	the_sprite->h_scale = h_scale;
	generate_sprite_verts(the_sprite);
	sleep(2);
	for (i = 1; i < ARRAY_SIZE(top_sparks); i++) {
		the_sprite->ii = top_sparks[i];
		the_sprite->tn = find_texture(top_sparks[i]->texture_name, HISCORE_PG_TID);
		generate_sprite_verts(the_sprite);
		sleep(2);
	}
	delobj(the_sprite);
}	/* proc_top_sparkle */

static void simple_cycle_proc(int *args)
{
	int	i, j, m, n, c;
	char hilite[HI_SCORE_ENTRIES];
	int flash_done[HI_SCORE_ENTRIES];
									
	for(i = 0; i < HI_SCORE_ENTRIES; i++)
		flash_done[i] = 0;
	for(i = 0; i < HI_SCORE_ENTRIES; i++)
	{
		hilite[i] = 0;
		if (show_hilite && !args[1])
		{
			for (j = 0; j < MAX_PLYRS; j++)
			{
				if (!(record_me & (1 << j)))
					continue;
				if (plyr_data_ptrs[j]->plyr_name[0] == '\0' ||
						plyr_data_ptrs[j]->plyr_name[0] == '-')
					continue;
				if (strncmp(rec_sort_ram[i].plyr_name,
							plyr_data_ptrs[j]->plyr_name,
							LETTERS_IN_NAME))
					continue;
				if (strncmp(rec_sort_ram[i].plyr_pin_nbr,
							plyr_data_ptrs[j]->plyr_pin_nbr,
							PIN_NUMBERS))
					continue;
				hilite[i] = 1;
			}
		}
	}

	i = 0;
	j = HI_SCORE_ENTRIES - 1;
	while(1)
	{
		if(!args[0])
		{
			for (m = 0; m < HI_SCORE_ENTRIES; m++)
			{
				for (n = 0; n < 3; n++)
				{
					if (hstrings[n][m])
					{
						c = hilite[m] ?
							simple_hilite_table[(i/RATE2)%15] : WHITE;
						if (m == j)
						{
							if (((j == 0) || hilite[j]) && !flash_done[j]) {
								int y_delta, width, height;
								
								y_delta = flash_tab[j].y + 4;
								width = ((flash_tab[j].w + (flash_tab[j].w / 4)) / 52.0F) * 100.0F;
								height = (flash_tab[j].h / 6.0F) * 100.0F;
								qcreate("topspark", SIMPLE_CYCLE_PROC_ID, proc_top_sparkle, flash_tab[j].x, y_delta, width | ((5 + j) << 24), height);
								flash_done[j] = 1;
							}
							if (j)
								c = LT_YELLOW;
							else if (c == (int)WHITE)
								c = simple_cycle_table[(i/RATE2)%15];
						}
						hstrings[n][m]->state.color = c;
						change_string_state(hstrings[n][m]);
					}
				}
			}
		}

		i++;
		if (j)
		{
			if (!(i % RATE1))
			{
				j--;
				if (!j)
				{
					i = 0;
					simple_cycle_state = 1;
				}
			}
		}
		else if (i > (40 * RATE2))
			simple_cycle_state = 2;

		sleep(1);
	}
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
static void do_simple_cycle(int sleep_time, int no_cyc, int no_hilite, int no_del)
{
	simple_cycle_state = 0;
	qcreate("scycle", SIMPLE_CYCLE_PROC_ID, simple_cycle_proc, no_cyc, no_hilite, 0, 0);

	// Don't button-out till state changes
	while(simple_cycle_state < 1)
		sleep(1);

	while(sleep_time && !(get_player_sw_current() & ALL_ABCD_MASK))
	{
		sleep(1);
		sleep_time--;
	}

	if(sleep_time)
		snd_scall_direct(tick_sndn,VOLUME2,127,1);
		//snd_scall_bank(cursor_bnk_str,TICK_SND,VOLUME1,127,1);

	killall(SIMPLE_CYCLE_PROC_ID, -1);

	if (!no_del)
	{
		del1c(0xFEEDABC, -1);
		del1c(2, -1);
		delete_multiple_strings(5, -1);
	}
}

//-------------------------------------------------------------------------------------------------
//			This routine prints all the hi-score tables
//
// called from ATTRACT.C
//-------------------------------------------------------------------------------------------------

#define HISCORE_TICKS	(7 * tsec)

void show_hiscore_tables(int min_time, int max_time)
{
	plyr_record_t *rec_ram;
	sprite_info_t *hiscr_hdr_obj;
	ostring_t *strobj;
	char *tmp;
	char *tmp1;
	float x1, x2, wid_s;
	short i;
	char buffer[80];
	int fid;
	sprite_info_t *world_plaques[NUM_WORLD_RECORDS];
	int was_low_res = is_low_res;

	show_hilite = show_them_hiscore_tables_first;
	show_them_hiscore_tables_first = 0;

	if(cmos_status != 2) {
			// Disable drawing
		draw_enable(0);
	
		wipeout();
		
		// Enable drawing
		draw_enable(1);
	
		if (load_textures_into_tmu(hiscore_textures_tbl)) 
		{
			// Not to worry, the wipeout killed the plates if they even existed
#ifdef DEBUG
			printf("Couldn't load all HI-SCORE page textures into tmu\n");
#endif
			return;
		}
		lock_multiple_textures(HISCORE_PG_TID, 0xFFFFFFFF);

		get_all_records();
	
		if(!rec_sort_ram)
		{
			// Not to worry, the wipeout killed the plates if they even existed
			return;
		}
	
		// Disable drawing
		draw_enable(0);
	
		// draw backdround
		beginobj(&nbcbk2,		0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
		beginobj(&nbcbk2_c1,	0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
		beginobj(&nbcbk2_c2,	0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
		beginobj(&nbcbk2_c3,	0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
		
		for (i = 0; i < NUM_WORLD_RECORDS; i++) {
			world_plaques[i] = beginobj(&cotpsgrf, 0.0F, SPRITE_VRES, 400.0F, BKGRND_TID);
			hide_sprite(world_plaques[i]);
		}
		
		// draw title bar
		beginobj(&chsmidbr, (SPRITE_HRES/2.0f)-21.0f, SPRITE_VRES-35.0f, 14.0F, HISCORE_PG_TID);
		beginobj(&chsmidbr_c1, (SPRITE_HRES/2.0f)-21.0f, SPRITE_VRES-35.0f, 14.0F, HISCORE_PG_TID);
	
		// Set step value for left-to-right text overlap
		set_text_z_inc(-0.1f);
	
		// Make sure we are at full intensity
		normal_screen();
	
//		if (is_low_res)
//			fid = FONTID_NBA14;
//		else
	#ifdef USE_CHATTEN_FONT
			fid = FONTID_CHATTEN20;
	#else
			fid = FONTID_NBA20;
	#endif
		{
			font_node_t * pfn =  get_font_handle(fid);
			image_info_t * pii = get_char_image_info(fid, 'S');
			wid_s = pii->w + (float)(pfn->font_info->char_space);
		}
	
		//
		// Hi-score page 0 (all star champion)
		//
		set_string_id(5);
		set_text_font(FONTID_NBA23);	/* FONTID_NBA25 */
		set_text_color(LT_YELLOW);
		set_text_position((SPRITE_HRES/2.0f)+30.0f,329.0f, 5.0F);
		oprintf("%dj%s", (HJUST_CENTER|VJUST_CENTER), hi_scr_titles[12]);
	
		set_text_font(FONTID_NBA10);
		set_text_position(sub_hdr_xys[0], sub_hdr_xys[1], 5.0F);
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, hi_scr_subhdrs[12]);
		
		rec_ram = rec_sort_ram;
		qsort(rec_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_teams_defeated);
		rec_ram = rec_sort_ram;
		
		if(create_font(&bast75_font, FONTID_BAST75))
			set_text_font(FONTID_BAST75);
		else
	#ifdef USE_CHATTEN_FONT
			set_text_font(FONTID_CHATTEN25);
	#else
			set_text_font(FONTID_NBA25);
	#endif
	
		x1 = is_low_res ? 7.0f : 0.0f;
		if (is_low_res)
			is_low_res = 5;
		if (calc_tms_def(rec_ram[0].teams_defeated) < (NUM_TEAMS + 1)) {
			set_text_position(SPRITE_HRES / 2, hiscr_ys[1], 30.0F);
			oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, ".  .  .  .  .  .");
		} else {
			set_text_position(SPRITE_HRES / 2, hiscr_ys[1] - x1, 30.0F);
			i = LETTERS_IN_NAME;
			while (i--) buffer[i] = rec_ram[0].plyr_name[i];
			i = LETTERS_IN_NAME;
			do buffer[i] = 0; while (buffer[--i] == ' ');
			oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_GREEN, buffer);
		}
	
		if (calc_tms_def(rec_ram[1].teams_defeated) < (NUM_TEAMS + 1)) {
			set_text_position(SPRITE_HRES / 2, hiscr_ys[4], 20.0F);
			oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, ".  .  .  .  .  .");
		} else {
			set_text_position(SPRITE_HRES / 2, hiscr_ys[4] - (2 * x1), 20.0F);
			i = LETTERS_IN_NAME;
			while (i--) buffer[i] = rec_ram[1].plyr_name[i];
			i = LETTERS_IN_NAME;
			do buffer[i] = 0; while (buffer[--i] == ' ');
			oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, buffer);
		}
	
		if (calc_tms_def(rec_ram[2].teams_defeated) < (NUM_TEAMS + 1)) {
			set_text_position(SPRITE_HRES / 2, hiscr_ys[7], 10.0F);
			oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, ".  .  .  .  .  .");
		} else {
			set_text_position(SPRITE_HRES / 2, hiscr_ys[7] - (3 * x1), 10.0F);
			i = LETTERS_IN_NAME;
			while (i--) buffer[i] = rec_ram[2].plyr_name[i];
			i = LETTERS_IN_NAME;
			do buffer[i] = 0; while (buffer[--i] == ' ');
			oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, buffer);
		}
		is_low_res = was_low_res;

		// Enable drawing
		draw_enable(1);
	
	#ifndef NOMOVIES
		if (!show_hilite) {
			show_attract_movie(nbc_logo_movie,  nbc_logo_movie_snds, 1, TRUE, 0xFF00FF, 0, FALSE, FALSE);
			normal_screen();
		}
	#endif
	
		// (ticks, no_cyc, no_hilite, no_del)
		do_simple_cycle(HISCORE_TICKS, 1, 1, 0);

		//
		// Hi-score page 1 (grand champions)
		//
		set_string_id(5);
		set_text_font(FONTID_NBA23);	/* FONTID_NBA25 */
		set_text_color(LT_YELLOW);
		set_text_position((SPRITE_HRES/2.0f)+30.0f,329.0f, 5.0F);
		oprintf("%dj%s", (HJUST_CENTER|VJUST_CENTER), hi_scr_titles[0]);
	
	//	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[0], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	//	hiscr_hdr_obj->id = 2;
	//	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[1], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	//	hiscr_hdr_obj->id = 2;
		
		set_text_font(FONTID_NBA10);
		set_text_position(sub_hdr_xys[0], sub_hdr_xys[1], 5.0F);
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, hi_scr_subhdrs[0]);
		
		rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
		qsort(rec_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_teams_defeated);
		rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
		
		if(create_font(&bast75_font, FONTID_BAST75))
			set_text_font(FONTID_BAST75);
		else
	#ifdef USE_CHATTEN_FONT
			set_text_font(FONTID_CHATTEN25);
	#else
			set_text_font(FONTID_NBA25);
	#endif
	
		x1 = is_low_res ? 7.0f : 0.0f;
		if (is_low_res)
			is_low_res = 5;
		if (calc_tms_def(rec_ram[0].teams_defeated) < NUM_TEAMS) {
			set_text_position(SPRITE_HRES / 2, hiscr_ys[1], 30.0F);
			oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, ".  .  .  .  .  .");
		} else {
			set_text_position(SPRITE_HRES / 2, hiscr_ys[1] - x1, 30.0F);
			i = LETTERS_IN_NAME;
			while (i--) buffer[i] = rec_ram[0].plyr_name[i];
			i = LETTERS_IN_NAME;
			do buffer[i] = 0; while (buffer[--i] == ' ');
			oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_GREEN, buffer);
		}
	
		if (calc_tms_def(rec_ram[1].teams_defeated) < NUM_TEAMS) {
			set_text_position(SPRITE_HRES / 2, hiscr_ys[4], 20.0F);
			oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, ".  .  .  .  .  .");
		} else {
			set_text_position(SPRITE_HRES / 2, hiscr_ys[4] - (2 * x1), 20.0F);
			i = LETTERS_IN_NAME;
			while (i--) buffer[i] = rec_ram[1].plyr_name[i];
			i = LETTERS_IN_NAME;
			do buffer[i] = 0; while (buffer[--i] == ' ');
			oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, buffer);
		}
	
		if (calc_tms_def(rec_ram[2].teams_defeated) < NUM_TEAMS) {
			set_text_position(SPRITE_HRES / 2, hiscr_ys[7], 10.0F);
			oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, ".  .  .  .  .  .");
		} else {
			set_text_position(SPRITE_HRES / 2, hiscr_ys[7] - (3 * x1), 10.0F);
			i = LETTERS_IN_NAME;
			while (i--) buffer[i] = rec_ram[2].plyr_name[i];
			i = LETTERS_IN_NAME;
			do buffer[i] = 0; while (buffer[--i] == ' ');
			oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, buffer);
		}
		is_low_res = was_low_res;
	
		// (ticks, no_cyc, no_hilite, no_del)
		do_simple_cycle(HISCORE_TICKS, 1, 1, 0);
		
		//
		// Hi-score page 2 (greatest players...win %)
		//
		set_string_id(5);
		set_text_font(FONTID_NBA23);	/* FONTID_NBA25 */
		set_text_color(LT_YELLOW);
		set_text_position((SPRITE_HRES/2.0f)+30.0f,329.0f, 5.0F);
		oprintf("%dj%s", (HJUST_CENTER|VJUST_CENTER), hi_scr_titles[1]);
	
	//	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[2], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	//	hiscr_hdr_obj->id = 2;
	//	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[3], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	//	hiscr_hdr_obj->id = 2;
		
		set_text_font(FONTID_NBA10);
		set_text_position(sub_hdr_xys[0], sub_hdr_xys[1], 5.0F);
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, hi_scr_subhdrs[1]);
		
		rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
		qsort(rec_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_win_percent);
		rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
		
		set_text_font(fid);	//FONTID_BAST23);
		x1 = 0.0f;
		x2 = 0.0f;
		for(i = 0; i < HI_SCORE_ENTRIES; i++)
		{
			sprintf(buffer, "#%d", i+1);
			if(get_string_width(buffer) > x1)
			{
				x1 = get_string_width(buffer);
			}
			sprintf(buffer, "%d WINS", rec_ram[i].games_won);
			if(get_string_width(buffer) > x2)
			{
				x2 = get_string_width(buffer);
			}
		}
		x2 = (SPRITE_HRES * 0.9f) - x2;
		x1 = (SPRITE_HRES * 0.1f) + x1;
		x1 += x2;
		x1 /= 2.0f;
		for (i=0; i < HI_SCORE_ENTRIES; i++,rec_ram++)
		{
			// print rank
			set_text_position(SPRITE_HRES * 0.1f, hiscr_ys[i], 5.0f);
			hstrings[0][i] = oprintf("%dj#%d", (HJUST_LEFT|VJUST_CENTER), i+1);
			
			// print name
			set_text_position(x1, hiscr_ys[i], 5.0F);
			flash_tab[i].x = x1;
			flash_tab[i].y = hiscr_ys[i];
			sprintf(buffer, "%1.*s", LETTERS_IN_NAME, rec_ram->plyr_name);
			tmp = buffer + (LETTERS_IN_NAME - 1);
			while(*tmp == ' ' && tmp >= buffer)
			{
				--tmp;
			}
			if(tmp != &buffer[LETTERS_IN_NAME-1])
			{
				*(tmp+1) = 0;
			}
			hstrings[1][i] = oprintf("%dj%1.*s", (HJUST_CENTER|VJUST_CENTER), LETTERS_IN_NAME, buffer);
			flash_tab[i].w = get_string_width(buffer);
			flash_tab[i].h = get_font_height(fid);
			
			// print value
			set_text_position(SPRITE_HRES * 0.9f, hiscr_ys[i], 5.0F);
			if (rec_ram->games_played == 0)
				hstrings[2][i] = oprintf("%dj.%d", (HJUST_RIGHT|VJUST_CENTER), ((rec_ram->games_won * 1000)/50));	// make low
			else if (rec_ram->games_won != rec_ram->games_played)	
				hstrings[2][i] = oprintf("%dj.%d", (HJUST_RIGHT|VJUST_CENTER), ((rec_ram->games_won * 1000)/rec_ram->games_played));
			else	
				hstrings[2][i] = oprintf("%dj1.000", (HJUST_RIGHT|VJUST_CENTER));
		}	
		// (ticks, no_cyc, no_hilite, no_del)
		do_simple_cycle(HISCORE_TICKS, 0, 0, 0);
		
		//
		// Hi-score page 3 (biggest winners - most games won)
		//
		set_string_id(5);
		set_text_font(FONTID_NBA23);	/* FONTID_NBA25 */
		set_text_color(LT_YELLOW);
		set_text_position((SPRITE_HRES/2.0f)+30.0f,329.0f, 5.0F);
		oprintf("%dj%s", (HJUST_CENTER|VJUST_CENTER), hi_scr_titles[2]);
	//	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[4], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	//	hiscr_hdr_obj->id = 2;
	//	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[5], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	//	hiscr_hdr_obj->id = 2;
		
		set_text_font(FONTID_NBA10);
		set_text_position(sub_hdr_xys[0], sub_hdr_xys[1], 5.0F);
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, hi_scr_subhdrs[2]);
		
		rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
		qsort(rec_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_wins);
		rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
		
		set_text_font(fid);
		x1 = 0.0f;
		x2 = 0.0f;
		for(i = 0; i < HI_SCORE_ENTRIES; i++)
		{
			sprintf(buffer, "#%d", i+1);
			if(get_string_width(buffer) > x1)
			{
				x1 = get_string_width(buffer);
			}
			sprintf(buffer, "%d WINS", rec_ram[i].games_won);
			if(get_string_width(buffer) > x2)
			{
				x2 = get_string_width(buffer);
			}
		}
		x2 = (SPRITE_HRES * 0.9f) - x2;
		x1 = (SPRITE_HRES * 0.1f) + x1;
		x1 += x2;
		x1 /= 2.0f;
		for (i=0; i < HI_SCORE_ENTRIES; i++,rec_ram++)
		{
			// print rank
			set_text_position(SPRITE_HRES * 0.1f, hiscr_ys[i], 5.0f);
			hstrings[0][i] = oprintf("%dj#%d", (HJUST_LEFT|VJUST_CENTER), i+1);
			
			// print name
			set_text_position(x1, hiscr_ys[i], 5.0F);
			flash_tab[i].x = x1;
			flash_tab[i].y = hiscr_ys[i];
			sprintf(buffer, "%1.*s", LETTERS_IN_NAME, rec_ram->plyr_name);
			tmp = buffer + (LETTERS_IN_NAME - 1);
			while(*tmp == ' ' && tmp >= buffer)
			{
				--tmp;
			}
			if(tmp != &buffer[LETTERS_IN_NAME-1])
			{
				*(tmp+1) = 0;
			}
			hstrings[1][i] = oprintf("%dj%1.*s", (HJUST_CENTER|VJUST_CENTER), LETTERS_IN_NAME, buffer);
			flash_tab[i].w = get_string_width(buffer);
			flash_tab[i].h = get_font_height(fid);
			
			// print value
			set_text_position((rec_ram->games_won == 1 ? (SPRITE_HRES * 0.9f - wid_s):(SPRITE_HRES * 0.9f)), hiscr_ys[i], 5.0F);
			hstrings[2][i] = oprintf("%dj%d WIN%c", (HJUST_RIGHT|VJUST_CENTER), rec_ram->games_won, (rec_ram->games_won == 1 ? '\0':'S'));
		}	
		// (ticks, no_cyc, no_hilite, no_del)
		do_simple_cycle(HISCORE_TICKS, 0, 0, 0);
	
		//
		// Hi-score page 4 (best offensive players)
		//
		set_string_id(5);
		set_text_font(FONTID_NBA23);	/* FONTID_NBA25 */
		set_text_color(LT_YELLOW);
		set_text_position((SPRITE_HRES/2.0f)+30.0f,329.0f, 5.0F);
		oprintf("%dj%s", (HJUST_CENTER|VJUST_CENTER), hi_scr_titles[3]);
	//	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[6], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	//	hiscr_hdr_obj->id = 2;
	//	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[7], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	//	hiscr_hdr_obj->id = 2;
		
		set_text_font(FONTID_NBA10);
		set_text_position(sub_hdr_xys[0], sub_hdr_xys[1], 5.0F);
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, hi_scr_subhdrs[3]);
		
		rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
		qsort(rec_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_points_scored);
		rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
		
		set_text_font(fid);
		x1 = 0.0f;
		x2 = 0.0f;
		for(i = 0; i < HI_SCORE_ENTRIES; i++)
		{
			sprintf(buffer, "#%d", i+1);
			if(get_string_width(buffer) > x1)
			{
				x1 = get_string_width(buffer);
			}
			sprintf(buffer, "%.2f PTS", (float)rec_ram[i].points_scored/(float)rec_ram[i].games_played);	// 3.2f
			if(get_string_width(buffer) > x2)
			{
				x2 = get_string_width(buffer);
			}
		}
		x2 = (SPRITE_HRES * 0.9f) - x2;
		x1 = (SPRITE_HRES * 0.1f) + x1;
		x1 += x2;
		x1 /= 2.0f;
		for (i=0; i < HI_SCORE_ENTRIES; i++,rec_ram++)
		{
			// print rank
			set_text_position(SPRITE_HRES * 0.1f, hiscr_ys[i], 5.0f);
			hstrings[0][i] = oprintf("%dj#%d", (HJUST_LEFT|VJUST_CENTER), i+1);
		
			// print name
			set_text_position(x1, hiscr_ys[i], 5.0F);
			flash_tab[i].x = x1;
			flash_tab[i].y = hiscr_ys[i];
			sprintf(buffer, "%1.*s", LETTERS_IN_NAME, rec_ram->plyr_name);
			tmp = buffer + (LETTERS_IN_NAME - 1);
			while(*tmp == ' ' && tmp >= buffer)
			{
				--tmp;
			}
			if(tmp != &buffer[LETTERS_IN_NAME-1])
			{
				*(tmp+1) = 0;
			}
			hstrings[1][i] = oprintf("%dj%1.*s", (HJUST_CENTER|VJUST_CENTER), LETTERS_IN_NAME, buffer);
			flash_tab[i].w = get_string_width(buffer);
			flash_tab[i].h = get_font_height(fid);
			
			// print value
			set_text_position(SPRITE_HRES * 0.9f, hiscr_ys[i], 5.0F);
			hstrings[2][i] = oprintf("%dj%.2f PTS", (HJUST_RIGHT|VJUST_CENTER), (float)rec_ram->points_scored/(float)rec_ram->games_played);	//3.2f
		}	
		// (ticks, no_cyc, no_hilite, no_del)
		do_simple_cycle(HISCORE_TICKS, 0, 0, 0);
		
		//
		// Hi-score page 5 (best defensive players)
		//
		set_string_id(5);
		set_text_font(FONTID_NBA23);	/* FONTID_NBA25 */
		set_text_color(LT_YELLOW);
		set_text_position((SPRITE_HRES/2.0f)+30.0f,329.0f, 5.0F);
		oprintf("%dj%s", (HJUST_CENTER|VJUST_CENTER), hi_scr_titles[4]);
	//	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[8], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	//	hiscr_hdr_obj->id = 2;
	//	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[9], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	//	hiscr_hdr_obj->id = 2;
		
		set_text_font(FONTID_NBA10);
		set_text_position(sub_hdr_xys[0], sub_hdr_xys[1], 5.0F);
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, hi_scr_subhdrs[4]);
		
		rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
		qsort(rec_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_points_allowed);
		rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
		
		set_text_font(fid);	//FONTID_BAST23);
		x1 = 0.0f;
		x2 = 0.0f;
		for(i = 0; i < HI_SCORE_ENTRIES; i++)
		{
			sprintf(buffer, "#%d", i+1);
			if(get_string_width(buffer) > x1)
			{
				x1 = get_string_width(buffer);
			}
			sprintf(buffer, "%.2f PTS", (float)rec_ram[i].points_allowed/(float)rec_ram[i].games_played);	//d%d ?
			if(get_string_width(buffer) > x2)
			{
				x2 = get_string_width(buffer);
			}
		}
		x2 = (SPRITE_HRES * 0.9f) - x2;
		x1 = (SPRITE_HRES * 0.1f) + x1;
		x1 += x2;
		x1 /= 2.0f;
		for (i=0; i < HI_SCORE_ENTRIES; i++,rec_ram++)
		{
			// print rank
			set_text_position(SPRITE_HRES * 0.1f, hiscr_ys[i], 5.0f);
			hstrings[0][i] = oprintf("%dj#%d", (HJUST_LEFT|VJUST_CENTER), i+1);
		
			// print name
			set_text_position(x1, hiscr_ys[i], 5.0F);
			flash_tab[i].x = x1;
			flash_tab[i].y = hiscr_ys[i];
			sprintf(buffer, "%1.*s", LETTERS_IN_NAME, rec_ram->plyr_name);
			tmp = buffer + (LETTERS_IN_NAME - 1);
			while(*tmp == ' ' && tmp >= buffer)
			{
				--tmp;
			}
			if(tmp != &buffer[LETTERS_IN_NAME-1])
			{
				*(tmp+1) = 0;
			}
			hstrings[1][i] = oprintf("%dj%1.*s", (HJUST_CENTER|VJUST_CENTER), LETTERS_IN_NAME, buffer);
			flash_tab[i].w = get_string_width(buffer);
			flash_tab[i].h = get_font_height(fid);
			
			// print value
			set_text_position(SPRITE_HRES * 0.9f, hiscr_ys[i], 5.0F);
			hstrings[2][i] = oprintf("%dj%.2f PTS", (HJUST_RIGHT|VJUST_CENTER), (float)rec_ram->points_allowed/(float)rec_ram->games_played);	//d%d ?
		}	
		// (ticks, no_cyc, no_hilite, no_del)
		do_simple_cycle(HISCORE_TICKS, 0, 0, 0);
		
		//
		// Hi-score page 6 (best 3 point shooters)
		//
		set_string_id(5);
		set_text_font(FONTID_NBA23);	/* FONTID_NBA25 */
		set_text_color(LT_YELLOW);
		set_text_position((SPRITE_HRES/2.0f)+30.0f,329.0f, 5.0F);
		oprintf("%dj%s", (HJUST_CENTER|VJUST_CENTER), hi_scr_titles[5]);
	//	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[10], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	//	hiscr_hdr_obj->id = 2;
	//	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[11], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	//	hiscr_hdr_obj->id = 2;
		
		set_text_font(FONTID_NBA10);
		set_text_position(sub_hdr_xys[0], sub_hdr_xys[1], 5.0F);
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, hi_scr_subhdrs[5]);
		
		rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
		qsort(rec_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_3pt_percent);
		rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
		
		set_text_font(fid);
		x1 = 0.0f;
		x2 = 0.0f;
		for(i = 0; i < HI_SCORE_ENTRIES; i++)
		{
			sprintf(buffer, "#%d", i+1);
			if(get_string_width(buffer) > x1)
			{
				x1 = get_string_width(buffer);
			}
			sprintf(buffer, "%d", (int)rec_ram[i].three_ptrs_made);
			if(get_string_width(buffer) > x2)
			{
				x2 = get_string_width(buffer);
			}
		}
		x2 = (SPRITE_HRES * 0.9f) - x2;
		x1 = (SPRITE_HRES * 0.1f) + x1;
		x1 += x2;
		x1 /= 2.0f;
		for (i=0; i < HI_SCORE_ENTRIES; i++,rec_ram++)
		{
			// print rank
			set_text_position(SPRITE_HRES * 0.1f, hiscr_ys[i], 5.0f);
			hstrings[0][i] = oprintf("%dj#%d", (HJUST_LEFT|VJUST_CENTER), i+1);
		
			// print name
			set_text_position(x1, hiscr_ys[i], 5.0F);
				flash_tab[i].x = x1;
				flash_tab[i].y = hiscr_ys[i];
			sprintf(buffer, "%1.*s", LETTERS_IN_NAME, rec_ram->plyr_name);
			tmp = buffer + (LETTERS_IN_NAME - 1);
			while(*tmp == ' ' && tmp >= buffer)
			{
				--tmp;
			}
			if(tmp != &buffer[LETTERS_IN_NAME-1])
			{
				*(tmp+1) = 0;
			}
			hstrings[1][i] = oprintf("%dj%1.*s", (HJUST_CENTER|VJUST_CENTER), LETTERS_IN_NAME, buffer);
			flash_tab[i].w = get_string_width(buffer);
			flash_tab[i].h = get_font_height(fid);
			
			// print value
			set_text_position(SPRITE_HRES * 0.9f, hiscr_ys[i], 5.0F);
			hstrings[2][i] = oprintf("%dj%d", (HJUST_RIGHT|VJUST_CENTER), (int)rec_ram->three_ptrs_made);
		}	
		// (ticks, no_cyc, no_hilite, no_del)
		do_simple_cycle(HISCORE_TICKS, 0, 0, 0);
		
		//
		// Hi-score page 7 (biggest showboaters)
		//
		set_string_id(5);
		set_text_font(FONTID_NBA23);	/* FONTID_NBA25 */
		set_text_color(LT_YELLOW);
		set_text_position((SPRITE_HRES/2.0f)+30.0f,329.0f, 5.0F);
		oprintf("%dj%s", (HJUST_CENTER|VJUST_CENTER), hi_scr_titles[6]);
	//	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[12], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	//	hiscr_hdr_obj->id = 2;
	//	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[13], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	//	hiscr_hdr_obj->id = 2;
		
		set_text_font(FONTID_NBA10);
		set_text_position(sub_hdr_xys[0], sub_hdr_xys[1], 5.0F);
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, hi_scr_subhdrs[6]);
		
		rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
		qsort(rec_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_dunk_percent);
		rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
		
		set_text_font(fid);
		x1 = 0.0f;
		x2 = 0.0f;
		for(i = 0; i < HI_SCORE_ENTRIES; i++)
		{
			sprintf(buffer, "#%d", i+1);
			if(get_string_width(buffer) > x1)
			{
				x1 = get_string_width(buffer);
			}
			sprintf(buffer, "%d", (int)rec_ram[i].dunks_made);
			if(get_string_width(buffer) > x2)
			{
				x2 = get_string_width(buffer);
			}
		}
		x2 = (SPRITE_HRES * 0.9f) - x2;
		x1 = (SPRITE_HRES * 0.1f) + x1;
		x1 += x2;
		x1 /= 2.0f;
		for (i=0; i < HI_SCORE_ENTRIES; i++,rec_ram++)
		{
			// print rank
			set_text_position(SPRITE_HRES * 0.1f, hiscr_ys[i], 5.0f);
			hstrings[0][i] = oprintf("%dj#%d", (HJUST_LEFT|VJUST_CENTER), i+1);
		
			// print name
			set_text_position(x1, hiscr_ys[i], 5.0F);
			flash_tab[i].x = x1;
			flash_tab[i].y = hiscr_ys[i];
			sprintf(buffer, "%1.*s", LETTERS_IN_NAME, rec_ram->plyr_name);
			tmp = buffer + (LETTERS_IN_NAME - 1);
			while(*tmp == ' ' && tmp >= buffer)
			{
				--tmp;
			}
			if(tmp != &buffer[LETTERS_IN_NAME-1])
			{
				*(tmp+1) = 0;
			}
			hstrings[1][i] = oprintf("%dj%1.*s", (HJUST_CENTER|VJUST_CENTER), LETTERS_IN_NAME, buffer);
			flash_tab[i].w = get_string_width(buffer);
			flash_tab[i].h = get_font_height(fid);

			// print value
			set_text_position(SPRITE_HRES * 0.9f, hiscr_ys[i], 5.0F);
			hstrings[2][i] = oprintf("%dj%d", (HJUST_RIGHT|VJUST_CENTER), (int)rec_ram->dunks_made);
		}	
		// (ticks, no_cyc, no_hilite, no_del)
		do_simple_cycle(HISCORE_TICKS, 0, 0, 0);
	
		//
		// Hi-score page 8 (consecutive wins)
		//
		set_string_id(5);
		set_text_font(FONTID_NBA23);	/* FONTID_NBA25 */

		set_text_color(LT_YELLOW);
		set_text_position((SPRITE_HRES/2.0f)+30.0f,329.0f, 5.0F);
		oprintf("%dj%s", (HJUST_CENTER|VJUST_CENTER), hi_scr_titles[7]);
	//	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[14], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	//	hiscr_hdr_obj->id = 2;
	//	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[15], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	//	hiscr_hdr_obj->id = 2;
		
		set_text_font(FONTID_NBA10);
		set_text_position(sub_hdr_xys[0], sub_hdr_xys[1], 5.0F);
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, hi_scr_subhdrs[7]);
	
		rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
		qsort(rec_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_win_streaks);
		rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
		
		set_text_font(fid);	//FONTID_BAST23);
		x1 = 0.0f;
		x2 = 0.0f;
		for(i = 0; i < HI_SCORE_ENTRIES; i++)
		{
			sprintf(buffer, "#%d", i+1);
			if(get_string_width(buffer) > x1)
			{
				x1 = get_string_width(buffer);
			}
			sprintf(buffer, "%d WINS", rec_ram[i].winstreak);
			if(get_string_width(buffer) > x2)
			{
				x2 = get_string_width(buffer);
			}
		}
		x2 = (SPRITE_HRES * 0.9f) - x2;
		x1 = (SPRITE_HRES * 0.1f) + x1;
		x1 += x2;
		x1 /= 2.0f;
		for (i=0; i < HI_SCORE_ENTRIES; i++,rec_ram++)
		{
			// print rank
			set_text_position(SPRITE_HRES * 0.1f, hiscr_ys[i], 5.0f);
			hstrings[0][i] = oprintf("%dj#%d", (HJUST_LEFT|VJUST_CENTER), i+1);
		
			// print name
			set_text_position(x1, hiscr_ys[i], 5.0F);
			flash_tab[i].x = x1;
			flash_tab[i].y = hiscr_ys[i];
			sprintf(buffer, "%1.*s", LETTERS_IN_NAME, rec_ram->plyr_name);
			tmp = buffer + (LETTERS_IN_NAME - 1);
			while(*tmp == ' ' && tmp >= buffer)
			{
				--tmp;
			}
			if(tmp != &buffer[LETTERS_IN_NAME-1])
			{
				*(tmp+1) = 0;
			}
			hstrings[1][i] = oprintf("%dj%1.*s", (HJUST_CENTER|VJUST_CENTER), LETTERS_IN_NAME, buffer);
			flash_tab[i].w = get_string_width(buffer);
			flash_tab[i].h = get_font_height(fid);
			
			// print value
			set_text_position((rec_ram->winstreak == 1 ? (SPRITE_HRES * 0.9f - wid_s):(SPRITE_HRES * 0.9f)), hiscr_ys[i], 5.0F);
			hstrings[2][i] = oprintf("%dj%d WIN%c", (HJUST_RIGHT|VJUST_CENTER), rec_ram->winstreak, (rec_ram->winstreak == 1 ? '\0':'S'));
		}	
		// (ticks, no_cyc, no_hilite, no_del)
		do_simple_cycle(HISCORE_TICKS, 0, 0, 0);
	
		//
		// Hi-score page 9 (most games played)
		//
		set_string_id(5);
		set_text_font(FONTID_NBA23);	/* FONTID_NBA25 */
		set_text_color(LT_YELLOW);
		set_text_position((SPRITE_HRES/2.0f)+30.0f,329.0f, 5.0F);
		oprintf("%dj%s", (HJUST_CENTER|VJUST_CENTER), hi_scr_titles[8]);
	//	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[16], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	//	hiscr_hdr_obj->id = 2;
	//	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[17], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	//	hiscr_hdr_obj->id = 2;
		
		set_text_font(FONTID_NBA10);
		set_text_position(sub_hdr_xys[0], sub_hdr_xys[1], 5.0F);
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, hi_scr_subhdrs[8]);
		
		rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
		qsort(rec_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_games_played);
		rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
		
		set_text_font(fid);	//FONTID_BAST23);
		x1 = 0.0f;
		x2 = 0.0f;
		for(i = 0; i < HI_SCORE_ENTRIES; i++)
		{
			sprintf(buffer, "#%d", i+1);
			if(get_string_width(buffer) > x1)
			{
				x1 = get_string_width(buffer);
			}
			sprintf(buffer, "%d GAMES", rec_ram[i].games_played);
			if(get_string_width(buffer) > x2)
			{
				x2 = get_string_width(buffer);
			}
		}
		x2 = (SPRITE_HRES * 0.9f) - x2;
		x1 = (SPRITE_HRES * 0.1f) + x1;
		x1 += x2;
		x1 /= 2.0f;
		for (i=0; i < HI_SCORE_ENTRIES; i++,rec_ram++)
		{
			// print rank
			set_text_position(SPRITE_HRES * 0.1f, hiscr_ys[i], 5.0f);
			hstrings[0][i] = oprintf("%dj#%d", (HJUST_LEFT|VJUST_CENTER), i+1);
		
			// print name
			set_text_position(x1, hiscr_ys[i], 5.0F);
			flash_tab[i].x = x1;
			flash_tab[i].y = hiscr_ys[i];
			sprintf(buffer, "%1.*s", LETTERS_IN_NAME, rec_ram->plyr_name);
			tmp = buffer + (LETTERS_IN_NAME - 1);
			while(*tmp == ' ' && tmp >= buffer)
			{
				--tmp;
			}
			if(tmp != &buffer[LETTERS_IN_NAME-1])
			{
				*(tmp+1) = 0;
			}
			hstrings[1][i] = oprintf("%dj%1.*s", (HJUST_CENTER|VJUST_CENTER), LETTERS_IN_NAME, buffer);
			flash_tab[i].w = get_string_width(buffer);
			flash_tab[i].h = get_font_height(fid);
			
			// print value
			set_text_position(SPRITE_HRES * 0.9f, hiscr_ys[i], 5.0F);
			hstrings[2][i] = oprintf("%dj%d GAMES", (HJUST_RIGHT|VJUST_CENTER), rec_ram->games_played);
		}	
		// (ticks, no_cyc, no_hilite, no_del)
		do_simple_cycle(HISCORE_TICKS, 0, 0, 0);
		
		//
		// Hi-score page 10 (world records)
		//
		set_string_id(5);
		set_text_font(FONTID_NBA23);	/* FONTID_NBA25 */
		set_text_color(LT_YELLOW);
		set_text_position((SPRITE_HRES/2.0f)+30.0f,329.0f, 5.0F);
		oprintf("%dj%s", (HJUST_CENTER|VJUST_CENTER), hi_scr_titles[9]);
	//	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[20], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	//	hiscr_hdr_obj->id = 2;
	//	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[21], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	//	hiscr_hdr_obj->id = 2;
		
		set_text_font(FONTID_NBA10);
		set_text_position(sub_hdr_xys[0], sub_hdr_xys[1], 5.0F);
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, hi_scr_subhdrs[9]);
		
		// print world records
		set_text_font(FONTID_NBA10);
		for (i = 0; i < NUM_WORLD_RECORDS; i++) {
			world_plaques[i]->x = wrld_rec_plqx[i];
			world_plaques[i]->y = wrld_rec_plqy[i];
			generate_sprite_verts(world_plaques[i]);
			unhide_sprite(world_plaques[i]);
		}
		for (i=0; i < NUM_WORLD_RECORDS; i++)
		{
			set_text_position(wrld_rec_xs[i], wrld_rec_title_ys[i], 5.0F);
			strobj = oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, world_record_titles[i]);
		}	
		set_text_font(fid);	//FONTID_BAST23);
		for (i=0; i < NUM_WORLD_RECORDS; i++)
		{
			float l, r;
			
			if (i > 2) {
				l = 105;
				r = 80;
			} else {
				l = 95.0f;
				r = 95.0f;
			}
			get_generic_record(WORLD_RECS_ARRAY,i,&tmp_world_rec);							// get cmos record into ram
			set_text_position(wrld_rec_xs[i]-l, wrld_rec_ys[i], 5.0F);
			oprintf("%dj%dc%1.*s", (HJUST_LEFT|VJUST_CENTER), WHITE, LETTERS_IN_NAME, tmp_world_rec.plyr_name);
			set_text_position(wrld_rec_xs[i]+r, wrld_rec_ys[i], 5.0F);
			oprintf("%dj%dc%d", (HJUST_RIGHT|VJUST_CENTER), LT_CYAN, tmp_world_rec.value);
		}
	
		// (ticks, no_cyc, no_hilite, no_del)
		do_simple_cycle(HISCORE_TICKS, 1, 1, 0);
		for (i = 0; i < NUM_WORLD_RECORDS; i++)
			hide_sprite(world_plaques[i]);


		//
		// Hi-score page 11 (trivia masters)
		//
		set_string_id(5);
		set_text_font(FONTID_NBA23);	/* FONTID_NBA25 */
		set_text_color(LT_YELLOW);
		set_text_position((SPRITE_HRES/2.0f)+30.0f,329.0f, 5.0F);
		oprintf("%dj%s", (HJUST_CENTER|VJUST_CENTER), hi_scr_titles[10]);
	//	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[22], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	//	hiscr_hdr_obj->id = 2;
	//	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[23], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	//	hiscr_hdr_obj->id = 2;
		
		set_text_font(FONTID_NBA10);
		set_text_position(sub_hdr_xys[0], sub_hdr_xys[1], 5.0F);
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, hi_scr_subhdrs[10]);
		
		rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
		qsort(rec_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_trivia);
		rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
		
		set_text_font(fid);	//FONTID_BAST23);
		x1 = 0.0f;
		x2 = 0.0f;
		for(i = 0; i < HI_SCORE_ENTRIES; i++)
		{
			sprintf(buffer, "#%d", i+1);
			if(get_string_width(buffer) > x1)
			{
				x1 = get_string_width(buffer);
			}
			sprintf(buffer, "%d PTS", rec_ram[i].trivia_pts);
			if(get_string_width(buffer) > x2)
			{
				x2 = get_string_width(buffer);
			}
		}
		x2 = (SPRITE_HRES * 0.9f) - x2;
		x1 = (SPRITE_HRES * 0.1f) + x1;
		x1 += x2;
		x1 /= 2.0f;
		for (i=0; i < HI_SCORE_ENTRIES; i++,rec_ram++)
		{
			// print rank
			set_text_position(SPRITE_HRES * 0.1f, hiscr_ys[i], 5.0f);
			hstrings[0][i] = oprintf("%dj#%d", (HJUST_LEFT|VJUST_CENTER), i+1);
			
			// print name
			set_text_position(x1, hiscr_ys[i], 5.0F);
			flash_tab[i].x = x1;
			flash_tab[i].y = hiscr_ys[i];
			sprintf(buffer, "%1.*s", LETTERS_IN_NAME, rec_ram->plyr_name);
			tmp = buffer + (LETTERS_IN_NAME - 1);
			while(*tmp == ' ' && tmp >= buffer)
			{
				--tmp;
			}
			if(tmp != &buffer[LETTERS_IN_NAME-1])
			{
				*(tmp+1) = 0;
			}
			hstrings[1][i] = oprintf("%dj%1.*s", (HJUST_CENTER|VJUST_CENTER), LETTERS_IN_NAME, buffer);
			flash_tab[i].w = get_string_width(buffer);
			flash_tab[i].h = get_font_height(fid);
			
			// print value
			set_text_position((rec_ram->trivia_pts == 1 ? (SPRITE_HRES * 0.9f - wid_s):(SPRITE_HRES * 0.9f)), hiscr_ys[i], 5.0F);
			hstrings[2][i] = oprintf("%dj%d PT%c", (HJUST_RIGHT|VJUST_CENTER), rec_ram->trivia_pts, (rec_ram->trivia_pts == 1 ? '\0':'S'));
		}	
		// (ticks, no_cyc, no_hilite, no_del)
		do_simple_cycle(HISCORE_TICKS, 0, 0, 0);


	
		//
		// Hi-score page 12 (most popular teams)
		//
		set_text_font(FONTID_NBA23);	/* FONTID_NBA25 */
		set_text_color(LT_YELLOW);
		set_text_position((SPRITE_HRES/2.0f)+25.0f,329.0f, 5.0F);
		oprintf("%dj%s", (HJUST_CENTER|VJUST_CENTER), hi_scr_titles[11]);
	
	//	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[24], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	//	hiscr_hdr_obj->id = 2;
	//	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[25], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	//	hiscr_hdr_obj->id = 2;
		
		set_string_id(5);
		set_text_font(FONTID_NBA10);
		set_text_position(sub_hdr_xys[0], sub_hdr_xys[1], 5.0F);
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, hi_scr_subhdrs[11]);
	
		// Fill in the popular team info structure
		for(i = 0; i < NUM_TEAMS; i++)
		{
			pop_team[i].team_number = i;
			if (get_audit(ATLANTA_HAWKS_AUD+i, &pop_team[i].times_choosen))
				pop_team[i].times_choosen = 0;
		}
	
		// Sort the popular teams array
		qsort(pop_team, NUM_TEAMS, sizeof(popular_team_info_t), pop_team_sort);
		
		set_text_font(fid);	//FONTID_BAST23);
	
		// Show the first 10 popular teams
		for(i = 0; i < 10; i++)
		{
			// print rank
			set_text_position(SPRITE_HRES * 0.02f, hiscr_ys[i], 5.0f);
			hstrings[0][i] = oprintf("%dj#%d", (HJUST_LEFT|VJUST_CENTER), i+1);
	
			// build team name
			tmp = buffer;
	
			tmp1 = (char *)(teaminfo[pop_team[i].team_number].szhome);
			do *tmp++ = *tmp1; while (*tmp1++);
			*(tmp - 1) = ' ';
	
			tmp1 = (char *)(teaminfo[pop_team[i].team_number].szname);
			do *tmp++ = *tmp1; while (*tmp1++);
	
			// print team name
			set_text_position(SPRITE_HRES / 2.0f, hiscr_ys[i], 5.0F);
			flash_tab[i].x = SPRITE_HRES / 2.0f;
			flash_tab[i].y = hiscr_ys[i];
			hstrings[1][i] = oprintf("%dj%s", (HJUST_CENTER|VJUST_CENTER), buffer);
			flash_tab[i].w = get_string_width(buffer);
			flash_tab[i].h = get_font_height(fid);
			hstrings[2][i] = NULL;
		}
		
		// (ticks, no_cyc, no_hilite, no_del)
		do_simple_cycle(HISCORE_TICKS, 0, 1, 1);
	
	
		// Done. Reset & cleanup
		set_text_z_inc(0.0f);
	
		if (!att_keeper)
			del1c(1,0xffffffff);			// delete background
		
		JFREE(rec_sort_ram);				// free the memory used for the sorting of records
		rec_sort_ram = NULL;
	
	}
	/* incase the NBC movie sound is lingering still */
	snd_stop_all();
	snd_set_reserved(SND_TRACK_0);
	snd_bank_delete(nbc_logo_movie_snd_bnk_str);
}

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
static int pop_team_sort(const void *a, const void *b)
{
	popular_team_info_t	*a1;
	popular_team_info_t	*b1;

	a1 = (popular_team_info_t *)a;
	b1 = (popular_team_info_t *)b;

	return(b1->times_choosen - a1->times_choosen);
//	return(a1->times_choosen - b1->times_choosen);
}

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
void do_amode_play( int min_time, int max_time )
{
	int i, p1, p2, p3, p4, perc;

	// Turn on the plates 
	turn_transit_on();

	// Disable here to go around the wipeout; <game_proc> does an enable
	draw_enable(0);

	wipeout();

	// Re-create the wipeout'd plate objects
//	create_plates();

	if (randrng(100) <= 5) {
		perc = randrng(100);
		if (perc <= 44)
			which_court = COURT_GILMORE;
		else if (perc >= 56)
			which_court = COURT_GEER;
		else if (perc <= 50)
			which_court = COURT_NBC;
		else
			which_court = COURT_MIDWAY;
	} else
		which_court = COURT_INDOOR;

	pup_blizzard = 0;
	pup_snow = 0;
	pup_rain = 0;
	alt_ladder_active = 0;
	if (COURT_IS_OUTDOORS) {
		if (randrng(100) <= 75) {
			perc = randrng(100);
			if (perc <= 33)
				pup_rain = 1;
			else if (perc <= 66)
				pup_snow = 1;
			else
				pup_blizzard = 1;
		}
	}

	game_info.team[0] = randrng(NUM_TEAMS);//team_index;
	game_info.team[1] = randrng(NUM_TEAMS-1);
	if( game_info.team[1] >= game_info.team[0] )
		game_info.team[1] += 1;

	if (game_info.game_state != GS_ATTRACT_MODE)
		die(0);

	do {
		p1 = randrng(teaminfo[game_info.team[0]].num_player);
		p2 = randrng(teaminfo[game_info.team[0]].num_player);
	} while (p1 == p2);
	do {
		p3 = randrng(teaminfo[game_info.team[1]].num_player);
		p4 = randrng(teaminfo[game_info.team[1]].num_player);
	} while (p3 == p4);
	set_player_record(game_info.team[0], p1, TRUE, &selected_players[0]);
	set_player_record(game_info.team[0], p2, TRUE, &selected_players[1]);
	set_player_record(game_info.team[1], p3, TRUE, &selected_players[2]);
	set_player_record(game_info.team[1], p4, TRUE, &selected_players[3]);

	halt = 0;								// I am stuck on Band-Aids...

	launch_game_proc();

	// Don't do anything till <game_proc> says its done initing; first wait till it starts
	while (game_info.game_mode != GM_INITIALIZING) sleep(1);
	while (game_info.game_mode == GM_INITIALIZING) sleep(1);

#ifdef DEBUG
	while (min_time || max_time)
	{
		sleep (1);

		if ((get_player_sw_current() & (ALL_ABCD_MASK & (~ALL_D_MASK))) && !min_time)
		{
			snd_scall_direct(tick_sndn,VOLUME2,127,1);
			break;
		}
		if (!halt && min_time) min_time--;
		if (!halt && max_time) max_time--;
	}
#else
	i = wait_for_any_butn_press(min_time,max_time);
#endif

	// Stop any replay thats running
	set_playback_mode(RP_STOP, 0, 0, 0);

	fade(0.0f, 30, 0);		//0=full off
	sleep(30);

	wipeout();
	snd_stop_all();
	snd_set_reserved(SND_TRACK_0);
	
	// remove the player sound banks
	unload_team_speech_banks();
	unload_player_speech_banks();
	ptl_shutdown();
	// remove the player models
	unload_player_models();
	unload_player_heads();
	
	sleep(1);
}

//---------------------------------------------------------------------------------------------------------------------------
//			Turn on the legal text
//---------------------------------------------------------------------------------------------------------------------------
static void fade_strings(int up_id, int down_id)
{
	long up_alpha, down_alpha;
	int i, step;
	ostring_t *str;
	sprite_node_t *slist;
	
	step = 16;
	for (i = 1; i <= step; i++) {
		up_alpha = ((256 / step) * i) - 1;
		down_alpha = 255 - up_alpha;
		
		/* the instruction strings */
		for (str = ostring_list; str != NULL; str = str->next)
			if ((str->id == up_id) || (str->id == down_id))
				for (slist = alpha_sprite_node_list; slist != NULL; slist = slist->next)
					if (slist->si->parent == str) {
						slist->si->state.constant_color &= 0x00FFFFFF;
						slist->si->state.constant_color |= ((str->id == up_id) ? up_alpha : down_alpha) << 24;
					}
		sleep(1);
	}
}	/* fade_string */

void legal_text_screen(int min_time, int max_time)
{
	sprite_info_t *bkgrnd_obj;
	int i,j;
		
	wipeout();
	
	bkgrnd_obj = beginobj(&legalbk,    0.0F, SPRITE_VRES, 140.0f, 0xbeef);
	bkgrnd_obj = beginobj(&legalbk_c1, 0.0F, SPRITE_VRES, 140.0f, 0xbeef);
	bkgrnd_obj = beginobj(&legalbk_c2, 0.0F, SPRITE_VRES, 140.0f, 0xbeef);
	bkgrnd_obj = beginobj(&legalbk_c3, 0.0F, SPRITE_VRES, 140.0f, 0xbeef);

	set_string_id(5);
	set_text_z_inc(-0.10F);
	set_text_font(FONTID_NBA10);
	set_text_justification_mode(HJUST_CENTER | VJUST_CENTER);
	set_text_color(WHITE);
	i = get_font_height(FONTID_NBA10) + 2;
	if (is_low_res) {
		i += 3;
		j = 205;
	} else
		j = 190;

	set_text_position(SPRITE_HRES/2, j, 50.0F); j -= i;
	oprintf("COIN-OPERATED VIDEO GAME (C) 1999 MIDWAY GAMES, INC.");
	set_text_position(SPRITE_HRES/2, j, 55.0F); j -= i;
	oprintf("ALL RIGHTS RESERVED.");
	set_text_position(SPRITE_HRES/2, j, 60.0F); j -= i;
	oprintf("MIDWAY IS A TRADEMARK OF MIDWAY GAMES, INC.");
	if (!is_low_res)
		j -= 4;
	set_text_position(SPRITE_HRES/2, j, 65.0F); j -= i;
	oprintf("THE NBA AND INDIVIDUAL NBA MEMBER TEAM IDENTIFICATIONS");
	set_text_position(SPRITE_HRES/2, j, 70.0F); j -= i;
	oprintf("USED ON OR IN THIS PRODUCT ARE TRADEMARKS, COPYRIGHTED");
	set_text_position(SPRITE_HRES/2, j, 75.0F); j -= i;
	oprintf("DESIGNS AND OTHER FORMS OF INTELLECTUAL PROPERTY");
	set_text_position(SPRITE_HRES/2, j, 80.0F); j -= i;
	oprintf("OF NBA PROPERTIES, INC. AND THE RESPECTIVE NBA");
	set_text_position(SPRITE_HRES/2, j, 85.0F); j -= i;
	oprintf("MEMBER TEAMS AND MAY NOT BE USED, IN WHOLE OR IN PART,");
	set_text_position(SPRITE_HRES/2, j, 90.0F); j -= i;
	oprintf("WITHOUT THE PRIOR WRITTEN CONSENT OF NBA PROPERTIES, INC.");
	set_text_position(SPRITE_HRES/2, j, 95.0F); j -= i;
	oprintf("(C) 1999 NBA PROPERTIES, INC.  ALL RIGHTS RESERVED.");
	if (!is_low_res)
		j -= 4;
	set_text_position(SPRITE_HRES/2, j, 100.0F); j -= i;
	oprintf("NBA TEAM ROSTERS ACCURATE AS OF 4/20/99");

	black_screen();
	
	sleep(2);
	fade(1.0f, 30, 0);		/* 1=full on 20=ticks */
	
	wait_for_any_butn_press(min_time,max_time);
	
	set_string_id(6);
	j = (is_low_res) ? 175 : 180;

	set_text_position(SPRITE_HRES/2, j, 30.0F); j -= i;
	oprintf("THE NBC NAMES, LOGOS, EMBLEMS, AND OTHER NBC");
	set_text_position(SPRITE_HRES/2, j, 35.0F); j -= i;
	oprintf("IDENTIFICATIONS USED ON OR IN THIS PRODUCT ARE");
	set_text_position(SPRITE_HRES/2, j, 40.0F); j -= i;
	oprintf("TRADEMARKS AND/OR OTHER FORMS OF INTELLECTUAL PROPERTY");
	set_text_position(SPRITE_HRES/2, j, 45.0F); j -= i;
	oprintf("THAT ARE THE EXCLUSIVE PROPERTY OF THE NATIONAL");
	set_text_position(SPRITE_HRES/2, j, 50.0F); j -= i;
	oprintf("BROADCASTING COMPANY, INC. AND MAY NOT BE USED, IN");
	set_text_position(SPRITE_HRES/2, j, 55.0F); j -= i;
	oprintf("WHOLE OR IN PART, WITHOUT ITS PRIOR WRITTEN CONSENT.");
	set_text_position(SPRITE_HRES/2, j, 60.0F); j -= i;
	oprintf("(C) 1999 NATIONAL BROADCASTING COMPANY, INC.");
	set_text_position(SPRITE_HRES/2, j, 65.0F); j -= i;
	oprintf("ALL RIGHTS RESERVED.");

	fade_strings(6, 5);
	delete_multiple_strings(5, 0xffffffff);

	wait_for_any_butn_press(min_time,max_time);

	set_string_id(5);
	j = (is_low_res) ? 175 : 180;

	/* overlay the 3dfx and um logo */
	bkgrnd_obj = beginobj(&legalbk2,    0.0F, SPRITE_VRES, 130.0f, 0xbeef);
	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	bkgrnd_obj = beginobj(&legalbk2_c1, 0.0F, SPRITE_VRES, 130.0f, 0xbeef);
	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	bkgrnd_obj = beginobj(&legalbk2_c2, 0.0F, SPRITE_VRES, 130.0f, 0xbeef);
	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	bkgrnd_obj = beginobj(&legalbk2_c3, 0.0F, SPRITE_VRES, 130.0f, 0xbeef);
	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;

	set_text_position(SPRITE_HRES/2, j, 10.0F); j -= i;
	oprintf("UNIVERSAL MONSTERS (TM) AND (C) 1998 UNIVERSAL CITY");
	set_text_position(SPRITE_HRES/2, j, 15.0F); j -= i;
	oprintf("STUDIOS, INC.  LICENSED BY UNIVERSAL STUDIOS LICENSING, INC.");
	set_text_position(SPRITE_HRES/2, j, 20.0F); j -= i;
	oprintf("ALL RIGHTS RESERVED.");
	j -= 6;
	set_text_position(SPRITE_HRES/2, j, 25.0F); j -= i;
	oprintf("GLIDE COPYRIGHT (C) 1999 3DFX INTERACTIVE, INC.");
	set_text_position(SPRITE_HRES/2, j, 30.0F); j -= i;
	oprintf("THE 3DFX INTERACTIVE LOGO IS A TRADEMARK OF");
	set_text_position(SPRITE_HRES/2, j, 35.0F); j -= i;
	oprintf("3DFX INTERACTIVE, INC.");

	fade_strings(5, 6);
	delete_multiple_strings(6, 0xffffffff);
	set_text_z_inc(0);

	wait_for_any_butn_press(min_time,max_time);

	fade(0.0f, 30, 0);		/* 0=full off */
	sleep(30);

	delete_multiple_strings(5, 0xffffffff);
	delete_all_sprites();
}	/* legal_text_screen */


#if 0
//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------

void test_font(int min_time, int max_time)
{
	sprite_info_t *bkgrnd_obj;
	
	wipeout();
	
	bkgrnd_obj = beginobj(&nbcbk2,    0.0F, SPRITE_VRES, 140.0f, 0xbeef);
	bkgrnd_obj = beginobj(&nbcbk2_c1, 0.0F, SPRITE_VRES, 140.0f, 0xbeef);
	bkgrnd_obj = beginobj(&nbcbk2_c2, 0.0F, SPRITE_VRES, 140.0f, 0xbeef);
	bkgrnd_obj = beginobj(&nbcbk2_c3, 0.0F, SPRITE_VRES, 140.0f, 0xbeef);
	
	black_screen();
	
	set_text_z_inc(-0.10F);

	set_text_font(FONTID_CHATTEN14);
	set_text_position(SPRITE_HRES/2,44, 10.0F);
	oprintf("%dj%dcabcdefghijklmnopqrstuvwxyz0123456789", (HJUST_CENTER|VJUST_CENTER), WHITE);

	set_text_font(FONTID_CHATTEN20);
	set_text_position(SPRITE_HRES/2,84, 10.0F);
	oprintf("%dj%dcabcdefghijklmnopqrstuvwxyz", (HJUST_CENTER|VJUST_CENTER), WHITE);
	set_text_font(FONTID_CHATTEN20);
	set_text_position(SPRITE_HRES/2,124, 10.0F);
	oprintf("%dj%dc!@#$%%^&*()<>,./?[];\':\"-_=+0123456789", (HJUST_CENTER|VJUST_CENTER), WHITE);

	set_text_font(FONTID_CHATTEN25);
	set_text_position(SPRITE_HRES/2,164, 10.0F);
	oprintf("%dj%dcabcdefghijklmnopqrstuvwxyz", (HJUST_CENTER|VJUST_CENTER), WHITE);
	set_text_font(FONTID_CHATTEN25);
	set_text_position(SPRITE_HRES/2,204, 10.0F);
	oprintf("%dj%dc!@#$%%^&*()<>,./?[];\':\"-_=+0123456789", (HJUST_CENTER|VJUST_CENTER), WHITE);

#if 0
	set_text_font(FONTID_CHATTEN20);
	set_text_position(SPRITE_HRES/2,200, 10.0F);
	oprintf("%dj%dcof cabbages and kings and were the seas", (HJUST_CENTER|VJUST_CENTER), WHITE);

	set_text_font(FONTID_CHATTEN25);
	set_text_position(SPRITE_HRES/2,280, 10.0F);
	oprintf("%dj%dcare boiling hot and wether pigs have wings!", (HJUST_CENTER|VJUST_CENTER), WHITE);
#endif

	set_text_z_inc(0);

	sleep(2);
	fade(1.0f, 30, 0);		/* 1=full on 20=ticks */
	
	wait_for_any_butn_press(min_time,max_time);
	
	fade(0.0f, 30, 0);		/* 0=full off */
	sleep(30);
	
	delete_multiple_strings(5, 0xffffffff);
	delete_all_sprites();
}
/* test_font */

#endif

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
extern int do_grand_champ;
extern plyr_record_t * plyr_data_ptrs[];

static tmu_ram_texture_t gc_textures_tbl[] = {
	{"endgam00.wms", BKGRND_TID},
	{"endgam01.wms", BKGRND_TID},
	{"endgam02.wms", BKGRND_TID},
	{"endgam03.wms", BKGRND_TID},
	{"endgam04.wms", BKGRND_TID},
	{"endgam05.wms", BKGRND_TID},
	{"endgam06.wms", BKGRND_TID},
	{"endgam07.wms", BKGRND_TID},
	{"endgam08.wms", BKGRND_TID},
	{"endgam09.wms", BKGRND_TID},
	{"endgam10.wms", BKGRND_TID},

	{NULL, 0}
};

static image_info_t * pii_gc_bg[] = {
	(image_info_t *) 0,
&nbacplqh,
&nbacplqh_c1,
	(image_info_t *) 1,
&nbacplqs,
&nbacplqs_c1,
	(image_info_t *) 2,
&pcokttl,
&pcokttl_c1,
&pcokttl_c2,
&pcokttl_c3,
&pcokttl_c4,
&pcokttl_c5,
	(image_info_t *) 3,
&bkgrnd1,
&bkgrnd1_c1,
&bkgrnd1_c2,
&bkgrnd1_c3,

	(image_info_t *) -1
};
#define MAX_GC_BG		4
#define MAX_GC_BG_OBJ	6

static image_info_t * pii_gc_fg[] = {

	(image_info_t *) 0,
&shwtmttl,
&shwtmttl_c1,
	(image_info_t *) 1,
&bsktbll,
&bsktbll_c1,
&bsktbll_c2,
&bsktbll_c3,

	(image_info_t *) -1
};
#define MAX_GC_FG		2
#define MAX_GC_FG_OBJ	4

enum {
	SCRIPTGC = 0x10000,

	SETDRAW,			// flag
	DOTRANS,			// flag
	DOMOVIE,			//

	SHOW_BG,			// bg#
	HIDE_BG,			// bg#
	CURR_BG,			// bg#
	FADE_BG,			// f:%,ticks
	POSI_BG,			// f:x,f:y
	VELI_BG,			// f:xv,f:yv

	SHOW_FG,			// fg#
	HIDE_FG,			// fg#
	CURR_FG,			// fg#
	FADE_FG,			// f:%,ticks
	POSI_FG,			// f:x,f:y
	VELI_FG,			// f:xv,f:yv

	COL_STR,			// color
	DEL_STR,			// id#
	FAD_STR,			// f:%,ticks
	FON_STR,			// font
	JUS_STR,			// justify
	NAM_STR,			// c:*,f:yinc
	OUT_STR,			// c:*,f:yinc
	POS_STR,			// f:x,f:y,f:z
	POX_STR,			// f:x
	SID_STR,			// id#
	VEL_STR,			// f:xv,f:yv
};

union IFC {
	int		i;
	float	f;
	char *	c;
};

#define BG_W	726.0f
#define BG_D	(BG_W-SPRITE_HRES)

#define FG0_W	334.0f
#define FG1_W	373.0f

#undef HRES
#undef VRES
#define HRES	(SPRITE_HRES)
#define VRES	(SPRITE_VRES)
#define HRAT	(SPRITE_HRES/512.0f)
#define VRAT	(SPRITE_VRES/384.0f)

#define HJ_L	(HJUST_LEFT)
#define HJ_C	(HJUST_CENTER)
#define HJ_R	(HJUST_RIGHT)
#define VJ_T	(VJUST_TOP)
#define VJ_C	(VJUST_CENTER)
#define VJ_B	(VJUST_BOTTOM)

union IFC gc_script[] = {
	{SETDRAW},	{0},

	{SHOW_BG},	{3},
	{POSI_BG},	{f:0.0},					{f:VRES},
	{SHOW_BG},	{2},
	{POSI_BG},	{f:-BG_D},					{f:VRES},

	{DOMOVIE},
//	{SETDRAW},	{1},

	{VELI_BG},	{f: BG_D /3000.0},			{f:0.0},

	{SID_STR},	{1},
	{COL_STR},	{LT_YELLOW},
	{FON_STR},	{FONTID_CHATTEN25},
	{JUS_STR},	{HJ_C|VJ_T},
	{POS_STR},	{f:HRES/2.0},				{f:327.0},					{f:9.0},
	{OUT_STR},	{c:"congratulations!"},		{f:-48.0 *VRAT},
	{SID_STR},	{2},
	{COL_STR},	{WHITE},
	{POS_STR},	{f:HRES/2.0},				{f:327.0},					{f:9.0},
	{OUT_STR},	{c:"congratulations!"},		{f:-48.0 *VRAT},
	{SID_STR},	{3},
	{FON_STR},	{FONTID_CHATTEN20},
	{OUT_STR},	{c:"you are the new"},		{f:-63.0 *VRAT},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{4},
	{COL_STR},	{LT_BLUE2},
	{FON_STR},	{FONTID_BAST75},
	{POX_STR},	{f:HRES/2.0 -15 *40},
	{OUT_STR},	{c:"grand"},				{f:-69.0 *VRAT},
	{SID_STR},	{5},
	{POX_STR},	{f:HRES/2.0 +15 *40},
	{OUT_STR},	{c:"champion!"},			{f:-69.0 *VRAT},
//0
	{SID_STR},	{1},
	{FAD_STR},	{f:0.0},					{0},						{25},
//25
	{SID_STR},	{1},
	{FAD_STR},	{f:1.0},					{0},
	{SID_STR},	{2},
	{FAD_STR},	{f:0.0},					{0},						{3},
	{SID_STR},	{1},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{2},
	{FAD_STR},	{f:1.0},					{0},						{3},
	{SID_STR},	{1},
	{FAD_STR},	{f:1.0},					{0},
	{SID_STR},	{2},
	{FAD_STR},	{f:0.0},					{0},						{3},
	{SID_STR},	{1},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{2},
	{FAD_STR},	{f:1.0},					{0},						{3},
	{SID_STR},	{1},
	{FAD_STR},	{f:1.0},					{0},
	{SID_STR},	{2},
	{FAD_STR},	{f:0.0},					{0},						{3},
	{SID_STR},	{1},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{2},
	{FAD_STR},	{f:1.0},					{0},						{3},
	{SID_STR},	{1},
	{FAD_STR},	{f:1.0},					{0},
	{SID_STR},	{2},
	{FAD_STR},	{f:0.0},					{0},						{3},
	{SID_STR},	{1},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{2},
	{FAD_STR},	{f:1.0},					{0},						{3},
	{SID_STR},	{1},
	{FAD_STR},	{f:1.0},					{0},
	{SID_STR},	{2},
	{FAD_STR},	{f:0.0},					{0},						{3},
	{70},
//122
	{SID_STR},	{3},
	{FAD_STR},	{f:1.0},					{50},						{50},
	{30},
//202
	{SID_STR},	{4},
	{VEL_STR},	{f: 15.0},					{f:0.0},
	{20},
	{SID_STR},	{5},
	{VEL_STR},	{f:-15.0},					{f:0.0},
	{20},
	{SID_STR},	{4},
	{VEL_STR},	{f: 0.0},					{f:0.0},
	{20},
	{SID_STR},	{5},
	{VEL_STR},	{f: 0.0},					{f:0.0},
	{30},
//292
	{SID_STR},	{4},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{5},
	{FAD_STR},	{f:0.0},					{0},						{3},
	{SID_STR},	{4},
	{FAD_STR},	{f:1.0},					{0},
	{SID_STR},	{5},
	{FAD_STR},	{f:1.0},					{0},						{3},
	{SID_STR},	{4},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{5},
	{FAD_STR},	{f:0.0},					{0},						{3},
	{SID_STR},	{4},
	{FAD_STR},	{f:1.0},					{0},
	{SID_STR},	{5},
	{FAD_STR},	{f:1.0},					{0},						{3},
	{SID_STR},	{4},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{5},
	{FAD_STR},	{f:0.0},					{0},						{3},
	{SID_STR},	{4},
	{FAD_STR},	{f:1.0},					{0},
	{SID_STR},	{5},
	{FAD_STR},	{f:1.0},					{0},						{3},
	{SID_STR},	{4},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{5},
	{FAD_STR},	{f:0.0},					{0},						{3},
	{SID_STR},	{4},
	{FAD_STR},	{f:1.0},					{0},
	{SID_STR},	{5},
	{FAD_STR},	{f:1.0},					{0},						{3},
	{SID_STR},	{4},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{5},
	{FAD_STR},	{f:0.0},					{0},						{3},
	{SID_STR},	{4},
	{FAD_STR},	{f:1.0},					{0},
	{SID_STR},	{5},
	{FAD_STR},	{f:1.0},					{0},						{3},
	{SID_STR},	{4},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{5},
	{FAD_STR},	{f:0.0},					{0},						{3},
	{SID_STR},	{4},
	{FAD_STR},	{f:1.0},					{0},
	{SID_STR},	{5},
	{FAD_STR},	{f:1.0},					{0},						{3},
	{230},
//558
	{SID_STR},	{1},
	{FAD_STR},	{f:0.0},					{70},
	{SID_STR},	{2},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{3},
	{FAD_STR},	{f:0.0},					{70},
	{SID_STR},	{4},
	{FAD_STR},	{f:0.0},					{70},
	{SID_STR},	{5},
	{FAD_STR},	{f:0.0},					{70},
	{SID_STR},	{6},
	{COL_STR},	{LT_YELLOW},
	{FON_STR},	{FONTID_CHATTEN20},
	{POS_STR},	{f:HRES/2.0},				{f:-3.0},					{f:8.0},
	{OUT_STR},	{c:"after all of the hard"},{f:-33.0 *VRAT},
	{OUT_STR},	{c:"work, you are now the"},{f:-33.0 *VRAT},
	{OUT_STR},	{c:"best in the league."},	{f:-63.0 *VRAT},
	{COL_STR},	{WHITE},
	{OUT_STR},	{c:"the nba showtime"},		{f:-33.0 *VRAT},
	{COL_STR},	{LT_BLUE2},
	{OUT_STR},	{c:"grand champ trophy"},	{f:-33.0 *VRAT},
	{COL_STR},	{WHITE},
	{OUT_STR},	{c:"is yours."},			{f:-33.0 *VRAT},
	{VEL_STR},	{f:0.0},					{f:1.0},
	{310},
	{DEL_STR},	{1},
	{DEL_STR},	{2},
	{DEL_STR},	{3},
	{DEL_STR},	{4},
	{DEL_STR},	{5},
	{VEL_STR},	{f:0.0},					{f:0.0},
	{420},
//1288
	{SID_STR},	{1},
	{COL_STR},	{LT_YELLOW},
	{POS_STR},	{f:HRES/2 +450.0},			{f:316.0},					{f:7.0},
	{OUT_STR},	{c:"but are you really"},	{f:-33.0 *VRAT},
	{OUT_STR},	{c:"the best of the best?"},{f:-63.0 *VRAT},
	{COL_STR},	{WHITE},
	{OUT_STR},	{c:"yeah, you have beat the"},{f:-33.0 *VRAT},
	{COL_STR},	{LT_BLUE2},
	{OUT_STR},	{c:"nba showtime league."},	{f:-63.0 *VRAT},
	{COL_STR},	{LT_YELLOW},
	{OUT_STR},	{c:"what about the"},		{f:-33.0 *VRAT},
	{OUT_STR},	{c:"showtime all-stars?"},	{f:-33.0 *VRAT},
	{VEL_STR},	{f:-1.0},					{f:0.0},
	{SID_STR},	{6},
	{FAD_STR},	{f:0.0},					{70},
	{450},
	{DEL_STR},	{6},
	{SID_STR},	{1},
	{VEL_STR},	{f:0.0},					{f:0.0},
	{420},
//2158
	{DEL_STR},	{1},
	{SID_STR},	{1},
	{COL_STR},	{WHITE},
	{POS_STR},	{f:HRES/2},					{f:316.0},					{f:9.0},
	{OUT_STR},	{c:"these guys are"},		{f:-33.0 *VRAT},
	{OUT_STR},	{c:"different."},			{f:-63.0 *VRAT},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{2},
	{COL_STR},	{LT_YELLOW},
	{OUT_STR},	{c:"they have a style of"},	{f:-33.0 *VRAT},
	{OUT_STR},	{c:"their own."},			{f:-63.0 *VRAT},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{3},
	{COL_STR},	{WHITE},
	{OUT_STR},	{c:"they play like you have"},{f:-33.0 *VRAT},
	{OUT_STR},	{c:"never seen before."},	{f:-33.0 *VRAT},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{4},
	{COL_STR},	{LT_RED2},
	{FON_STR},	{FONTID_CHATTEN25},
	{POS_STR},	{f:HRES/2},					{f:228.0},					{f:9.0},
	{OUT_STR},	{c:"and they are waiting"},	{f:-36.0 *VRAT},
	{OUT_STR},	{c:"for you."},				{f:-36.0 *VRAT},
	{FAD_STR},	{f:0.0},					{0},
	{60},
	{SID_STR},	{1},
	{FAD_STR},	{f:1.0},					{80},						{80},
	{40},
	{SID_STR},	{2},
	{FAD_STR},	{f:1.0},					{80},						{80},
	{40},
	{SID_STR},	{3},
	{FAD_STR},	{f:1.0},					{80},						{80},
	{140},
	{SID_STR},	{1},
	{FAD_STR},	{f:0.0},					{70},
	{SID_STR},	{2},
	{FAD_STR},	{f:0.0},					{70},
	{SID_STR},	{3},
	{FAD_STR},	{f:0.0},					{70},
	{70},
	{DEL_STR},	{1},
	{DEL_STR},	{2},
	{DEL_STR},	{3},
	{SID_STR},	{4},
	{FAD_STR},	{f:1.0},					{80},						{80},
	{210},
	{FAD_STR},	{f:0.0},					{80},						{80},
	{DEL_STR},	{4},
	{50},
//3168
	{SID_STR},	{1},
	{COL_STR},	{WHITE},
	{FON_STR},	{FONTID_CHATTEN20},
	{POS_STR},	{f:HRES/2},					{f:333.0},					{f:9.0},
	{OUT_STR},	{c:"the midway all-star"},	{f:-33.0 *VRAT},
	{OUT_STR},	{c:"challenge is next."},	{f:-63.0 *VRAT},
	{COL_STR},	{LT_YELLOW},
	{OUT_STR},	{c:"think you can do a repeat?"},{f:-33.0 *VRAT},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{2},
	{COL_STR},	{LT_RED2},
	{POS_STR},	{f:HRES/2},					{f:270.0},					{f:9.0},
	{FON_STR},	{FONTID_CHATTEN25},
	{OUT_STR},	{c:"the all-stars are ready."},{f:-33.0 *VRAT},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{3},
	{COL_STR},	{WHITE},
	{FON_STR},	{FONTID_BAST75},
	{JUS_STR},	{HJ_R|VJ_T},
	{POS_STR},	{f:464 +50 *10},			{f:159.0},					{f:9.0},
	{OUT_STR},	{c:"nba"},					{f:-69.0 *VRAT},
	{COL_STR},	{LT_BLUE2},
	{OUT_STR},	{c:"showtime"},				{f:-69.0 *VRAT},
	{COL_STR},	{LT_YELLOW},
	{FON_STR},	{FONTID_NBA20},
	{POS_STR},	{f:300 +50 *10},			{f:123.0},					{f:9.0},
	{OUT_STR},	{c:"thanks for playing"},	{f:-33.0 *VRAT},
	{VEL_STR},	{f:-50.0},					{f:0.0},
	{10},
	{VEL_STR},	{f:0.0},					{f:0.0},
	{50},
	{SID_STR},	{1},
	{FAD_STR},	{f:1.0},					{70},						{70},
	{300},
	{FAD_STR},	{f:0.0},					{70},						{70},
	{30},
	{SID_STR},	{2},
	{FAD_STR},	{f:1.0},					{70},						{70},
	{300},
//4068

//	{20000},
{SCRIPTGC}};
//
// congratulations!
// you are the new
// grand champion!
//
// after all of the hard work,
// you are now the best in the league.
// the nba showtime grand champ trophy is yours.
//
// but are you really the best of the best?
// yeah, you have beat the nba showtime league.
// what about the showtime all-stars?
//
// these guys are different.
// they have a style of their own.
// they play like you have never seen before.
// and they are waiting for you.
//
// thanks for playing nba showtime.
// the midway all-star challenge is next.
// think you can do a repeat?
// the all-stars are ready.
//


union IFC gc_script1[] = {
	{SETDRAW},	{0},

	{SHOW_BG},	{3},
	{POSI_BG},	{f:0.0},					{f:VRES},
	{SHOW_BG},	{2},
	{POSI_BG},	{f:-BG_D},					{f:VRES},

	{DOMOVIE},
//	{SETDRAW},	{1},

	{VELI_BG},	{f: BG_D /3000.0},			{f:0.0},

	{SID_STR},	{1},
	{COL_STR},	{LT_YELLOW},
	{FON_STR},	{FONTID_CHATTEN25},
	{JUS_STR},	{HJ_C|VJ_T},
	{POS_STR},	{f:HRES/2.0},				{f:327.0},					{f:9.0},
	{OUT_STR},	{c:"congratulations!"},		{f:-48.0 *VRAT},
	{SID_STR},	{2},
	{COL_STR},	{WHITE},
	{POS_STR},	{f:HRES/2.0},				{f:327.0},					{f:9.0},
	{OUT_STR},	{c:"congratulations!"},		{f:-48.0 *VRAT},
	{SID_STR},	{3},
	{FON_STR},	{FONTID_CHATTEN20},
	{OUT_STR},	{c:"you are the new"},		{f:-63.0 *VRAT},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{4},
	{COL_STR},	{LT_BLUE2},
	{FON_STR},	{FONTID_BAST75},
	{POX_STR},	{f:HRES/2.0 -15 *40},
	{OUT_STR},	{c:"all-star"},				{f:-69.0 *VRAT},
	{SID_STR},	{5},
	{POX_STR},	{f:HRES/2.0 +15 *40},
	{OUT_STR},	{c:"champion!"},			{f:-69.0 *VRAT},
//0
	{SID_STR},	{1},
	{FAD_STR},	{f:0.0},					{0},						{25},
//25
	{SID_STR},	{1},
	{FAD_STR},	{f:1.0},					{0},
	{SID_STR},	{2},
	{FAD_STR},	{f:0.0},					{0},						{3},
	{SID_STR},	{1},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{2},
	{FAD_STR},	{f:1.0},					{0},						{3},
	{SID_STR},	{1},
	{FAD_STR},	{f:1.0},					{0},
	{SID_STR},	{2},
	{FAD_STR},	{f:0.0},					{0},						{3},
	{SID_STR},	{1},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{2},
	{FAD_STR},	{f:1.0},					{0},						{3},
	{SID_STR},	{1},
	{FAD_STR},	{f:1.0},					{0},
	{SID_STR},	{2},
	{FAD_STR},	{f:0.0},					{0},						{3},
	{SID_STR},	{1},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{2},
	{FAD_STR},	{f:1.0},					{0},						{3},
	{SID_STR},	{1},
	{FAD_STR},	{f:1.0},					{0},
	{SID_STR},	{2},
	{FAD_STR},	{f:0.0},					{0},						{3},
	{SID_STR},	{1},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{2},
	{FAD_STR},	{f:1.0},					{0},						{3},
	{SID_STR},	{1},
	{FAD_STR},	{f:1.0},					{0},
	{SID_STR},	{2},
	{FAD_STR},	{f:0.0},					{0},						{3},
	{70},
//122
	{SID_STR},	{3},
	{FAD_STR},	{f:1.0},					{50},						{50},
	{30},
//202
	{SID_STR},	{4},
	{VEL_STR},	{f: 15.0},					{f:0.0},
	{20},
	{SID_STR},	{5},
	{VEL_STR},	{f:-15.0},					{f:0.0},
	{20},
	{SID_STR},	{4},
	{VEL_STR},	{f: 0.0},					{f:0.0},
	{20},
	{SID_STR},	{5},
	{VEL_STR},	{f: 0.0},					{f:0.0},
	{30},
//292
	{SID_STR},	{4},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{5},
	{FAD_STR},	{f:0.0},					{0},						{3},
	{SID_STR},	{4},
	{FAD_STR},	{f:1.0},					{0},
	{SID_STR},	{5},
	{FAD_STR},	{f:1.0},					{0},						{3},
	{SID_STR},	{4},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{5},
	{FAD_STR},	{f:0.0},					{0},						{3},
	{SID_STR},	{4},
	{FAD_STR},	{f:1.0},					{0},
	{SID_STR},	{5},
	{FAD_STR},	{f:1.0},					{0},						{3},
	{SID_STR},	{4},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{5},
	{FAD_STR},	{f:0.0},					{0},						{3},
	{SID_STR},	{4},
	{FAD_STR},	{f:1.0},					{0},
	{SID_STR},	{5},
	{FAD_STR},	{f:1.0},					{0},						{3},
	{SID_STR},	{4},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{5},
	{FAD_STR},	{f:0.0},					{0},						{3},
	{SID_STR},	{4},
	{FAD_STR},	{f:1.0},					{0},
	{SID_STR},	{5},
	{FAD_STR},	{f:1.0},					{0},						{3},
	{SID_STR},	{4},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{5},
	{FAD_STR},	{f:0.0},					{0},						{3},
	{SID_STR},	{4},
	{FAD_STR},	{f:1.0},					{0},
	{SID_STR},	{5},
	{FAD_STR},	{f:1.0},					{0},						{3},
	{SID_STR},	{4},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{5},
	{FAD_STR},	{f:0.0},					{0},						{3},
	{SID_STR},	{4},
	{FAD_STR},	{f:1.0},					{0},
	{SID_STR},	{5},
	{FAD_STR},	{f:1.0},					{0},						{3},
	{230},
//558
	{SID_STR},	{1},
	{FAD_STR},	{f:0.0},					{70},
	{SID_STR},	{2},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{3},
	{FAD_STR},	{f:0.0},					{70},
	{SID_STR},	{4},
	{FAD_STR},	{f:0.0},					{70},
	{SID_STR},	{5},
	{FAD_STR},	{f:0.0},					{70},
	{SID_STR},	{6},
	{COL_STR},	{LT_YELLOW},
	{FON_STR},	{FONTID_CHATTEN20},
	{POS_STR},	{f:HRES/2.0},				{f:300.0},					{f:8.0},
	{OUT_STR},	{c:"you hammered the all-star"},{f:-33.0 *VRAT},
	{OUT_STR},	{c:"league. you took the"},	{f:-33.0 *VRAT},
	{OUT_STR},	{c:"challenge and you won."},{f:-63.0 *VRAT},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{7},
	{COL_STR},	{LT_RED2},
	{OUT_STR},	{c:"but things are not"},	{f:-33.0 *VRAT},
	{OUT_STR},	{c:"over yet."},			{f:-33.0 *VRAT},
	{FAD_STR},	{f:0.0},					{0},
	{100},
	{DEL_STR},	{1},
	{DEL_STR},	{2},
	{DEL_STR},	{3},
	{DEL_STR},	{4},
	{DEL_STR},	{5},
	{SID_STR},	{6},
	{FAD_STR},	{f:1.0},					{70},						{70},
	{230},
	{SID_STR},	{7},
	{FAD_STR},	{f:1.0},					{70},						{70},
	{160},
	{SID_STR},	{6},
	{FAD_STR},	{f:0.0},					{70},						{70},
	{SID_STR},	{7},
	{FAD_STR},	{f:0.0},					{70},						{70},
	{DEL_STR},	{6},
	{DEL_STR},	{7},
	{SID_STR},	{1},
	{COL_STR},	{WHITE},
	{POS_STR},	{f:HRES/2.0},				{f:-3.0},					{f:8.0},
	{OUT_STR},	{c:"now the leagues will"},	{f:-33.0 *VRAT},
	{OUT_STR},	{c:"act as one."},			{f:-63.0 *VRAT},
	{COL_STR},	{LT_YELLOW},
	{OUT_STR},	{c:"each will send in"},	{f:-33.0 *VRAT},
	{OUT_STR},	{c:"its best."},			{f:-63.0 *VRAT},
	{COL_STR},	{WHITE},
	{OUT_STR},	{c:"this should give them"},{f:-33.0 *VRAT},
	{OUT_STR},	{c:"what it takes."},		{f:-63.0 *VRAT},
	{VEL_STR},	{f:0.0},					{f:1.0},
	{313},
	{VEL_STR},	{f:0.0},					{f:0.0},
	{420},
//
	{FAD_STR},	{f:0.0},					{70},						{70},
	{DEL_STR},	{1},
	{50},
//
	{SID_STR},	{1},
	{POS_STR},	{f:HRES/2.0},				{f:316.0},					{f:8.0},
	{COL_STR},	{LT_GREEN},
	{OUT_STR},	{c:"could there be a"},		{f:-33.0 *VRAT},
	{OUT_STR},	{c:"third time?"},			{f:-33.0 *VRAT},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{2},
	{COL_STR},	{WHITE},
	{FON_STR},	{FONTID_BAST75},
	{JUS_STR},	{HJ_R|VJ_T},
	{POS_STR},	{f:464 +50 *10},			{f:159.0},					{f:9.0},
	{OUT_STR},	{c:"nba"},					{f:-69.0 *VRAT},
	{COL_STR},	{LT_BLUE2},
	{OUT_STR},	{c:"showtime"},				{f:-69.0 *VRAT},
	{COL_STR},	{LT_YELLOW},
	{FON_STR},	{FONTID_NBA20},
	{POS_STR},	{f:300 +50 *10},			{f:123.0},					{f:9.0},
	{OUT_STR},	{c:"thanks for playing"},	{f:-33.0 *VRAT},
	{VEL_STR},	{f:-50.0},					{f:0.0},
	{10},
	{VEL_STR},	{f:0.0},					{f:0.0},
	{50},
	{SID_STR},	{1},
	{FAD_STR},	{f:1.0},					{70},						{70},
	{300},

//	{20000},
{SCRIPTGC}};
//
// congratulations!
// you are the new
// all-star champion!
//
// you hammered the all-star league.
// you took the challenge and you won.
// but things are not over yet.
//
// now the leagues will act as one.
// each will send in its best.
// this should give them what it takes.
//
// thanks for playing nba showtime.
// could there be a third time?
//

void show_grand_champ_movie(int min_time, int max_time)
{
	register union IFC * script;
	sprite_node_t * slist;
	sprite_info_t * psi_gc_bg[MAX_GC_BG][MAX_GC_BG_OBJ+1];	// +1 to ensure 0 terminate
	sprite_info_t * psi_gc_fg[MAX_GC_FG][MAX_GC_FG_OBJ+1];	// +1 to ensure 0 terminate
	sprite_info_t ** ppsi;
	image_info_t ** ppii;
	float fp;
	float fx = 0;
	float fy = 0;
	float fz = 0;
	int cur_bg = 0;
	int cur_fg = 0;
	int fid = 0;
	int sid = 0;
	int tex_load, fid23, fid25, fid75;
	int icur, inxt, inc;
	int i = 0, j = 0, p, q;
	int was_low_res = is_low_res;
	char sz_nms[LETTERS_IN_NAME*2+3+1];	// +3 for " & ", +1 for null terminator

	// Make sure all textures get loaded
	tex_load = load_textures_into_tmu(gc_textures_tbl);
#ifdef DEBUG
	if (tex_load)
	{
		fprintf(stderr, "Couldn't load all GRAND CHAMP textures into tmu\r\n");
		lockup();
	}
#endif
	lock_multiple_textures(BKGRND_TID, -1);		// lock gc tex
	lock_multiple_textures(0, -1);				// lock font/transition tex

	start_enable(FALSE);

	if (is_low_res) is_low_res = 6;

	// Make background/foreground sprites
	ppii = pii_gc_bg;
	while ((int)(*ppii) != (int)-1)
	{
		if ((int)(*ppii) >= (int)0)
		{
			// +1 to ensure 0 terminate
			j = MAX_GC_BG_OBJ+1;
			i = (int)(*ppii++);
			while (j) psi_gc_bg[i][--j] = 0;
			continue;
		}
		if (!tex_load)
		{
			psi_gc_bg[i][j] = beginobj(*ppii, 0.0f, 0.0f, 500.0f + i, BKGRND_TID);
			psi_gc_bg[i][j]->id = OID_GC_BG + i;
			set_veladd_mode(psi_gc_bg[i][j], TRUE);
			hide_sprite(psi_gc_bg[i][j++]);
		}
		ppii++;
	}
	ppii = pii_gc_fg;
	while ((int)(*ppii) != (int)-1)
	{
		if ((int)(*ppii) >= (int)0)
		{
			// +1 to ensure 0 terminate
			j = MAX_GC_FG_OBJ+1;
			i = (int)(*ppii++);
			while (j) psi_gc_fg[i][--j] = 0;
			continue;
		}
		if (!tex_load)
		{
			psi_gc_fg[i][j] = beginobj(*ppii, 0.0f, 0.0f, 10.0f + i, BKGRND_TID);
			psi_gc_fg[i][j]->id = OID_GC_FG + i;
			set_veladd_mode(psi_gc_fg[i][j], TRUE);
			hide_sprite(psi_gc_fg[i][j++]);
		}
		ppii++;
	}

	// Make sure fonts are available (at least something!)
	fid23 = FONTID_CHATTEN20;
	fid25 = FONTID_CHATTEN25;
	fid75 = FONTID_BAST75;
	if (!create_font(&bast75_font, fid75)) fid75 = FONTID_NBA25;

	script = (do_grand_champ & 0xffff) ? gc_script : gc_script1;
#ifdef DEBUG
	if (game_info.game_state == GS_ATTRACT_MODE)
	script = (get_player_sw_current() & (P1_UP|P2_UP|P3_UP|P4_UP)) ? gc_script1 : gc_script;
#endif
	while ((icur = script->i) != SCRIPTGC)
	{
		// Setup text
		set_text_transparency_mode(TRANSPARENCY_ENABLE);
		set_text_z_inc(-0.01f);

		script++;
		inxt = script->i;
		inc = 1;

		switch (icur)
		{
			case SETDRAW:											// flag
				draw_enable(inxt);
				break;

			case DOTRANS:											// flag
				if ((!inxt && plates_on == PLATES_ON) ||
					( inxt && plates_on == PLATES_OFF))
				{
					if (inxt)
						turn_transit_on();
					else
						turn_transit_off(0);
				}
				break;

			case DOMOVIE:											//
				show_attract_movie(
					trophy_movie,
					trophy_movie_snds,
					0,
					TRUE,
					0xFF00FF,
					0,
					FALSE,
					TRUE);
				inc = 0;
#ifndef NOAUDIO
				// Setup music
				// Disallow coin-in sounds while loading
				coin_sound_holdoff(TRUE);

				if (snd_bank_load_playing("tunbeach") == 0xeeee)
					increment_audit(SND_LOAD_FAIL_AUD);

				// track 0 is the tune, track 1 is the audience, track 5 is the announcer
				snd_set_reserved (SND_TRACK_0);

				snd_scall_bank("tunbeach", 2, VOLUME6, 127, SND_PRI_SET|SND_PRI_TRACK0);

				coin_sound_holdoff(FALSE);
#endif
				break;


			case SHOW_BG:											// bg#
			case SHOW_FG:											// fg#
				ppsi = icur == SHOW_BG ?
					psi_gc_bg[cur_bg = inxt]:psi_gc_fg[cur_fg = inxt];
				while (*ppsi) unhide_sprite(*ppsi++);
				break;

			case HIDE_BG:											// bg#
			case HIDE_FG:											// fg#
				ppsi = icur == HIDE_BG ?
					psi_gc_bg[cur_bg = inxt]:psi_gc_fg[cur_fg = inxt];
				while (*ppsi) hide_sprite(*ppsi++);
				break;

			case CURR_BG:											// bg#
				cur_bg = inxt;
				break;

			case CURR_FG:											// fg#
				cur_fg = inxt;
				break;

			case POSI_BG:											// f:x,f:y
			case POSI_FG:											// f:x,f:y
				ppsi = icur == POSI_BG ?
					psi_gc_bg[cur_bg]:psi_gc_fg[cur_fg];
				while (*ppsi)
				{
					ppsi[0]->x = script[0].f;
					ppsi[0]->y = script[1].f;
					ppsi++;
				}
				inc = 2;
				break;

			case VELI_BG:											// f:xv,f:yv
			case VELI_FG:											// f:xv,f:yv
				ppsi = icur == VELI_BG ?
					psi_gc_bg[cur_bg]:psi_gc_fg[cur_fg];
				while (*ppsi)
				{
					ppsi[0]->x_vel = script[0].f;
					ppsi[0]->y_vel = script[1].f;
					ppsi++;
				}
				inc = 2;
				break;


			case COL_STR:											// color
				set_text_color(inxt);
				break;

			case DEL_STR:											// id#
				delete_multiple_strings(inxt, -1);
				break;

			case FADE_BG:											// f:%,ticks
			case FADE_FG:											// f:%,ticks
				inxt = icur == FADE_BG ?
					OID_GC_BG + cur_bg:OID_GC_FG + cur_fg;
			case FAD_STR:											// f:%,ticks
				if (icur == FAD_STR)
					inxt = sid;
				fp= script[0].f;
				q = script[1].i;
				if (fp > 1.0f) fp = 1.0f;
				if (fp < 0.0f) fp = 0.0f;
				if (q < 0) q = 0;

				p = ((int)(255.0f * fp)) & 255;
				slist = alpha_sprite_node_list;
				while (slist)
				{
					if (slist->si->id == inxt)
					{
						if (q)
						{
							i = (slist->si->state.constant_color >> 24) & 255;
							j = p - i;
							i *= q;
							*(int *)(&slist->si->x_angle_end) = (p << 16) + q;
							*(int *)(&slist->si->y_angle_end) = i;
							*(int *)(&slist->si->z_angle_end) = j;
						}
						else
							slist->si->state.constant_color =
								(slist->si->state.constant_color & 0xffffff) |
								(p << 24);
					}
					slist = slist->next;
				}
				inc = 2;
				break;

			case FON_STR:											// font
				if (inxt == FONTID_CHATTEN20) fid = fid23;
				else
				if (inxt == FONTID_CHATTEN25) fid = fid25;
				else
				if (inxt == FONTID_BAST75) fid = fid75;
				else
				fid = inxt;
				set_text_font(fid);
				break;

			case JUS_STR:											// justify
				set_text_justification_mode(inxt);
				break;

			case NAM_STR:											// c:*,f:yinc
				*sz_nms = '\0';
				i = 0;
				p = 0;
				q = do_grand_champ;
				while (q)
				{
					if (q & 1)
					{
						j = LETTERS_IN_NAME;
						while (--j >= 0)
							if (plyr_data_ptrs[p]->plyr_name[j] != ' ')
								break;

						if (j < 0)
							sprintf(sz_nms,"%s%s(PLR%d)",
								*sz_nms ? sz_nms:"",
								*sz_nms ?  " & ":"",
								p+1);
						else
						{
							register char c = plyr_data_ptrs[p]->plyr_name[++j];
							plyr_data_ptrs[p]->plyr_name[j] = '\0';
							sprintf(sz_nms,"%s%s%s",
								*sz_nms ? sz_nms:"",
								*sz_nms ?  " & ":"",
								plyr_data_ptrs[p]->plyr_name);
							plyr_data_ptrs[p]->plyr_name[j] = c;
						}

						// If i=2, q should be going 0, but ...
						if (++i == 2) break;
					}
					q >>= 1;
					p++;
				}
				set_string_id(sid);
				oprintf("%s%s", sz_nms, script[0].c);
				fy += script[1].f;
				set_text_position(fx, fy, fz);
				inc = 2;
				break;

			case OUT_STR:											// c:*,f:yinc
				set_string_id(sid);
				oprintf(script[0].c);
				fy += script[1].f;
				set_text_position(fx, fy, fz);
				inc = 2;
				break;

			case POS_STR:											// f:x,f:y,f:z
				fx = script[0].f;
				fy = script[1].f;
				fz = script[2].f;
				set_text_position(fx, fy, fz);
				inc = 3;
				break;

			case POX_STR:											// f:x
				fx = script[0].f;
				set_text_position(fx, fy, fz);
				break;

			case SID_STR:											// id#
				set_string_id(sid = inxt);
				break;

			case VEL_STR:											// f:xv,f:yv
				slist = alpha_sprite_node_list;
				while (slist)
				{
					if (slist->si->id == sid)
					{
						slist->si->x_vel = script[0].f;
						slist->si->y_vel = script[1].f;

						set_veladd_mode(slist->si, TRUE);
					}
					slist = slist->next;
				}
				inc = 2;
				break;


			default:
				while (icur)
				{
					slist = alpha_sprite_node_list;
					while (slist)
					{
						p = *(int *)(&slist->si->x_angle_end);
						if (p)
						{
							q = p & 65535;
							p = (p >> 16) & 255;
							i  = *(int *)(&slist->si->y_angle_end);
							i += *(int *)(&slist->si->z_angle_end);
							j = i / q;
							if (j == p)
							{
								*(int *)(&slist->si->x_angle_end) = 0;
								*(int *)(&slist->si->y_angle_end) = 0;
								*(int *)(&slist->si->z_angle_end) = 0;
							}
							else
								*(int *)(&slist->si->y_angle_end) = i;

							slist->si->state.constant_color =
								(slist->si->state.constant_color & 0xffffff) |
								(j << 24);
						}
						slist = slist->next;
					}
					i = plates_on;
					sleep(1);
//					if (i == PLATES_COMING_ON && plates_on == PLATES_ON)
//						snd_scall_bank(plyr_bnk_str, LOCK_SND, VOLUME4, 127, PRIORITY_LVL5);
#ifdef DEBUG
	if (get_player_sw_close() & ALL_ABCD_MASK) break;
#endif
					icur--;
				}
				inc = 0;
		}
		script += inc;

		set_text_z_inc(0.0f);
	}

	// Clean out everything, remaking the plates it they were there
	wipeout();

//	if (plates_on == PLATES_ON || plates_on == PLATES_COMING_ON)
//		create_plates();

#ifndef NOAUDIO
	// Kill game over music
	snd_stop_track(SND_TRACK_0);

	snd_bank_delete("tunbeach");
#endif
	start_enable(TRUE);

	is_low_res = was_low_res;
}
