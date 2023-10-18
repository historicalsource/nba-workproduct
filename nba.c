/*
 *		$Archive: /video/Nba/nba.c $
 *		$Revision: 481 $
 *		$Date: 9/17/99 11:02p $
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
char *ss_nba_c = "$Workfile: nba.c $ $Revision: 481 $";
#endif


/*
 *		SYSTEM INCLUDES
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif

/*
 *		USER INCLUDES
 */

#include "plyrseq.h"
#include "player.h"
#include "externs.h"
#include "game.h"
#include "anim/gen_seq.h"
#include "playbook.h"
#include "sndcalls.h"
#include "id.h"
#include "fontid.h"
#include "select.h"
#include "transit.h"
#include "ani3d.h"
#include "audits.h"
#include "movie.h"
#include "gameadj.h"
#include "nbacmos.h"
#include "audits.h"
#include "movie.h"
#include "putil.h"
#include "mainproc.h"
#include "particle.h"

#ifndef RELEASE_VER
 #ifndef TEST_VER
//	#define T_CONSL
//	#define MORPH
//	#define NO_CLOCK
//	#define NO_SHOTCLOCK
//	#define SHORT_QTR

  #ifdef SHORT_QTR
	#ifndef NO_SHOTCLOCK
	#define NO_SHOTCLOCK
	#endif
  #endif

 #endif //TEST_VER
#endif //RELEASE_VER



//
// referenced data
//
extern char index_2p[NUM_PLAYERS];
extern char shift_2p[NUM_PLAYERS];

extern VIEWCOORD cambot;
extern BBDATA	bbdata[2];

extern plyr_record_t  * plyr_data_ptrs[];
extern plyr_record_t player1_data;
extern plyr_record_t player2_data;
extern plyr_record_t player3_data;
extern plyr_record_t player4_data;
extern int crowd_cheer_time;

extern int which_court;
extern int showhide_flag;						// Turbo bars
extern int hide_status_box;						// Top score box
extern int hide_peacock;
extern int priv_stealth_turbo;

extern int referee_active;
extern int vs_page_showing;
extern int game_purchased;           			// set bit (0-4) for plyr which bought full game
extern int tsec;
extern float hres, vres;
extern image_info_t gameclock;
extern image_info_t bast7_up,bast7_dn;
extern char away_jersey[30][30];
extern int pup_rain,pup_fog;
extern int pup_thickfog;
extern int pup_sfog;
extern int pup_nfog;
extern int pup_tourney;
extern int pup_bighead;
extern int pup_no_hotspots;
extern int last_shooter;
extern int pup_show_shotper;

extern void force_special_weather(void);
extern void clear_tourney_pups(void);
extern void eoq_score_plate_proc(int *args);
extern void announce_ball_handler_name_proc(int *args);
extern void arena_fog_adjustment(int fog_on);

extern int		p_status;						// from main.c; represents current plyrs in game with bit pos. (0-3)
extern int		p_status2;						// from main.c; represents previous plyrs in game with bit pos. (0-3)
extern pdata_t pdata_blocks[];					// from main.c; defined in "game.h"
extern int		four_plr_ver;
extern int bought_in_first;
extern int show_them_hiscore_tables_first;

extern int is_guest[NUM_PLAYERS];

extern int update_status_box;
extern int pushed_cnt[NUM_PLAYERS];


//
// referenced routines
//
extern void do_tipoff(void);
extern void one_minute_speech(void);
extern void clock_low_speech(void);
extern void shotclock_low_speech(void);
extern void shotclock_expired_speech(void);
int get_credits_to_start(void);
int get_credits_to_continue(void);
int get_full_game_credits(void);
void update_other_cmos_to_disk(int sleep_allowed);



extern void morph_proc( int *parg );			// from morph.c
extern void paid_credits_deduct(void);			// from main.c
extern void attract_loop(void);					// from attract.c
extern void plyrinfo_init(void);				// from plyrinfo.c
extern void show_player_names_proc(int *);		// from plyrinfo.c
extern void lineup_players_for_tipoff(void);
extern void lineup_players_new_quarter(void);
static void do_court_fog(void);
static void draw_fogon(void *dummy);
static void draw_fogoff(void *dummy);
extern char
	*plyr_bnks[],
	gameplay_bnk_str[],
	crowd_bnk_str[],
	plyr_bnk_str[],
	anc_make_bnk[],
	team1_bnk_str[],
	cursor_bnk_str[],
	eoq_tune_bnk_str[],
	game_tune_bnk_str[],
	start_movie_snd_bnk_str[],
	qtr_trans_bnk_str[],
	team2_bnk_str[];

void cambot_proc( int * );				// from cambot.c

void score_plate_proc(int *args);
void show_peacock_proc(int *args);
void shot_clock_box_proc(int *args);
void score_status_box(int *args);
void score_plate(int *args);
void yardage_info_plate(int *args);
void shotclock_violation_proc(int *args);

void init_drones( void );				// from drone.c
void drone_adjskill (int min);

void show_final_player_stats(void);
void game_over_screen(void);


void draw_goals( void * );
void draw_court( void * );

void load_permanent_snds(void);

int randrng(int);


//
// Global data
// 
#ifndef DEBUG_PLAYERS
load_info player_load_info[NUM_PLAYERS + 1];
#else
load_info player_load_info[7];
#endif

// Indexed by (qtr#) = 0 to (MAX_QUARTER - 1)
//char * game_tunes_tbl[]  =
//{
//	"tungangs", "tunextrm", "tundmjr4", "tuneurof",
//	"tungrupn", "tunhappy", "tuncrea2"
//};
//char * eoq_tunes_tbl[] =
//{
//	"startune", "tunhalft", "startune", "startune",
//	"startune", "startune", "startune"
//};

int cmos_ticks_per_second = 3;					// Initial setting will be overwritten

int qtr_pstatus[MAX_PLYRS];						/* human/drone by period.  bit0 = first qtr, bit6 = 3rd OT, 1=human, 0=drone */
int	old_game_purchased = 0;
int record_me;									// Flag bits of players to record to cmos
int do_grand_champ;								// Flg to do grand champ screen
int clock_active, reset_clock;
int shotclock_active, reset_shotclock = 0;
int players_ready = 0, ignore_pass = 0, ignore_steal = 0, ignore_shoot = 0;
int gSndTimer = 0, gWaitSayName = 0, juke_flail_allow = 0;
int	gAssist_time = 0;

int last_handler = 0;
int game_just_finished = 0;
int away_team_style;

// selected_players gets filled in at team select
#ifndef DEBUG_PLAYERS
player_record_info selected_players[NUM_PLAYERS];
#else
player_record_info selected_players[7];
#endif

#ifdef DBG_KO
#define DBG_KO_BOOL	dbg_ko
extern int dbg_ko;
#else
#define DBG_KO_BOOL	0
#endif


//
// function prototypes
//
void delete_referee(void);

void show_grand_champ_movie(int, int);
int	show_player_free_game(int);
//void start_off_proc(int *);
void launch_game_proc(void);
void crowd_stomp_proc(int *args);
void monkey_snds_proc(int *args);
void monkey_snds2_proc(int *args);
static void	shot_clock_expired_proc(int *args);
static void	shot_clock_proc(int *args);
static void game_clock_proc(int *args);
static void start_new_quarter(void);
void game_over(int ingame);
static void eoq_handling(void);
static void pre_new_quarter_handling(void);

void start_enable(int);
void coin_sound_holdoff(int);
int	compute_score(int);

void SetUpOOBounds(int, int, int);

void load_ingame_textures(void);
void load_player_speech_banks(void);
void unload_player_speech_banks(void);
void load_team_speech_banks(void);
void unload_team_speech_banks(void);
void flash_ball( int );

//
// local data
//
struct texture_node	*smkpuf_decals[NUM_FIRE_TXTRS];
char	spfx_texture[12];					// name of texture.

process_node_t player_procs[NUM_PLAYERS];

struct process_node *game_proc_ptr;
int	free_game_count[2] = {0,0};		// Team-based


static char *team_name_tex[NUM_TEAMS + 1] =
{	
	"txt_atl.wms",								// atl
	"txt_bos.wms",								// bos
	"txt_cha.wms",								// cha
	"txt_chi.wms",								// chi
	"txt_cle.wms",								// cle
	"txt_dal.wms", 								// dal
	"txt_den.wms",								// den
	"txt_det.wms",								// det
	"txt_gol.wms",								// gol
	"txt_hou.wms",								// hou
	"txt_ind.wms",								// ind
	"txt_lac.wms",								// lac
	"txt_lal.wms",								// lak
	"txt_mia.wms",								// mia
	"txt_mil.wms",								// mil
	"txt_min.wms",								// min
	"txt_nwj.wms",								// nwj
	"txt_nwy.wms",								// nwy
	"txt_orl.wms", 								// orl
	"txt_phi.wms",								// phi
	"txt_pho.wms",								// pho
	"txt_por.wms",								// por
	"txt_sac.wms",								// sac
	"txt_san.wms",								// san
	"txt_sea.wms",								// sea
	"txt_tor.wms", 								// tor
	"txt_uta.wms",								// uta
	"txt_van.wms",								// van
	"txt_was.wms",								// was
	"txt_mid.wms"								// midway
};

static char *team_speech_banks[NUM_TEAMS + 1] = {
		"ahawk",
		"bcelt",
		"chorn",
		"cbull",
		"ccavs",
		"dmavs",
		"dnugg",
		"dpist",
		"gswar",
		"hrock",
		"ipace",
		"lclip",
		"llake",
		"mheat",
		"mbuck",
		"mtimb",
		"njnet",
		"nykni",
		"omagi",
		"psixr",
		"phsun",
		"ptrai",
		"sking",
		"sansp",
		"seaso",
		"trapt",
		"ujazz",
		"vgriz",
		"wwiza",
		"midwa"
 };


/***************************************************************************/
/****************************************************************************/
void wipeout()
{
	/* Set the background color */
	set_bgnd_color(0xFF000000);

	delete_all_sprites();
	delete_all_strings();
	delete_all_background_sprites();
	clear_object_list();
	clear_texture_list(TEXTURES_ALL);
	delete_fonts();
	killall( 0, 0 );						// kill all processes except INDESTRUCTABLES!!!!
	guTexMemReset();
	main_load_fonts();
	if (plates_on != PLATES_OFF)
		create_plates();
}

#ifdef DEBUG
/****************************************************************************/
void game_proc_debug( int *parg )
{
	int i, plyr;

	// Make sure the movie player is shut down if it is running
	if(movie_get_state() > 1)
	{
		movie_abort();
	}

	/* cleanup */
	wipeout();

#ifndef DEBUG_PLAYERS
	// select roster
	game_info.team[0] = randrng(NUM_TEAMS);
	game_info.team[1] = randrng(NUM_TEAMS-1);
	if( game_info.team[1] >= game_info.team[0] )
		game_info.team[1]++;

	for (i = 0; i < NUM_PLAYERS; i++)
	{
		//plyr = randrng(100) % teaminfo[game_info.team[i >= NUM_PERTEAM]].num_player;
		plyr = randrng(teaminfo[game_info.team[i > 1 ? 1 : 0]].num_player);
		if(i & 1 && selected_players[i ^ 1].player_index == plyr) plyr = plyr ^ 1;
		printf("plyr = %d, game_info.team[%d >= NUM_PERTEAM] = %d\n", plyr, i, game_info.team[i >= NUM_PERTEAM]);
		//set_player_record(game_info.team[i >= NUM_PERTEAM], i % NUM_PERTEAM, TRUE, &selected_players[i]);
		set_player_record(game_info.team[i >= NUM_PERTEAM], plyr, TRUE, &selected_players[i]);
	}
#if 0
	set_player_record(TEAM_SECRET, HORSE_CHARACTER, TRUE, &selected_players[0]);
	set_player_record(TEAM_SECRET, CLOWN_CHARACTER, TRUE, &selected_players[1]);
	set_player_record(TEAM_SECRET, CHEERLEADER_KH_CHARACTER, TRUE, &selected_players[2]);
	set_player_record(TEAM_SECRET, NIKKO_CHARACTER, TRUE, &selected_players[3]);
	set_player_record(TEAM_LAL, 0, TRUE, &selected_players[3]);
#endif

	load_player_speech_banks();
	load_team_speech_banks();
#else

// SET team[0] to the team that you want debugged...
	game_info.team[0] = TEAM_WAS;
	game_info.team[1] = randrng(NUM_TEAMS);

	for (i = 0; i < teaminfo[game_info.team[0]].num_player; i++)
		set_player_record(game_info.team[0], i, TRUE, &selected_players[i]);
#endif
	launch_game_proc();
}
#endif //DEBUG

/****************************************************************************/
void launch_game_proc(void)
{
	game_proc_ptr = qcreate("game", GAME_PROC_PID, game_proc, 0, 0, 0, 0);
}

#ifdef T_CONSL
/****************************************************************************/
void test_console_proc(int * parg)
{
	static char * szsw = "UDLRABCD";
	static int x = 80, y = 200, dx = 14, dy = -20;
	static int last_sw,last_49,last_p5;

	while (game_info.game_quarter) sleep(1);

	while (!game_info.game_quarter)
	{
		int cur_sw = get_player_sw_current();
		int cur_49 = get_player_49way_current();
		int cur_p5 = get_p5_sw_current();

		if (cur_sw != last_sw || cur_49 != last_49 || cur_p5 != last_p5) {
			int x1, y1, i, j, sw;
			delete_multiple_strings(0xcafebabe, 0xffffffff);
			set_text_font(FONTID_NBA10);
			set_text_transparency_mode(TRANSPARENCY_ENABLE);
			set_string_id(0xcafebabe);
			y1 = y;
			i = 0;
			do {
				char * psz = szsw;
				x1 = x;
				if (i < 4)
					sw = cur_sw >> (i * 8);
				else
					sw = cur_p5;
				j = 8;
				while (j--) {
					set_text_position(x1, y1, 1.2f);
					oprintf("%dj%dc%c", (HJUST_CENTER|VJUST_CENTER), ((sw & 1) ? LT_CYAN:MED_GRAY), *psz++);
					x1 += dx;
					sw >>= 1;
				}
				if (i < 4) {
					sw = cur_49 >> (i * 8);
					sw &= 0xff;
					set_text_position((x - dx * 3), y1, 1.2f);
					oprintf("%dj%dc%i", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, sw);
				}
				y1 += dy;
			} while (++i < 5);
		}
		last_sw = cur_sw; last_49 = cur_49; last_p5 = cur_p5;

		sleep(1);
	}
	delete_multiple_strings(0xcafebabe, 0xffffffff);
}
#endif //T_CONSL

/****************************************************************************/
void set_plyr_control(void)
{
	int i;

	// (Re)init plyr controls
	for (i = 0; i < NUM_PLAYERS; i++)
	{
		int index_p = (four_plr_ver?i:index_2p[i]);
		game_info.plyr_control[i] = (p_status & (1<<i)) ? index_p : -1;
	}
}

/****************************************************************************/
extern void
	debug_player_proc(int *parg),
	debug_pre_player_proc( int *parg ),
	debug_last_player_proc( int *parg ),
	debug_load_plyr_txtrs(int team1)
	;

void game_proc( int *parg )
{
	int		i,j, teammate;
	char *pstruct;
	int pnum;
	int	val;
	int	vaud;
	int	gaud;
	int active_players, hvh;


#if 0
fprintf(stderr, "record_type = %d, team = %d, index = %d, record_num = %d\n", selected_players[0].record_type, selected_players[0].team, selected_players[0].player_index, selected_players[0].record_num);
fprintf(stderr, "record_type = %d, team = %d, index = %d, record_num = %d\n", selected_players[1].record_type, selected_players[1].team, selected_players[1].player_index, selected_players[1].record_num);
fprintf(stderr, "record_type = %d, team = %d, index = %d, record_num = %d\n", selected_players[2].record_type, selected_players[2].team, selected_players[2].player_index, selected_players[2].record_num);
fprintf(stderr, "record_type = %d, team = %d, index = %d, record_num = %d\n", selected_players[3].record_type, selected_players[3].team, selected_players[3].player_index, selected_players[3].record_num);
#endif

	// Make sure the movie player is shut down if it is running
	if(movie_get_state() > 1)
		movie_abort();
	clear_free_sprite_list();

	// free up any heap that's sitting on the free process list
//	clear_free_process_list();

	// Tell string handler to keep all letters at the same z
	set_text_z_inc(0);

	// audit full games
	for( i = 0; i < (four_plr_ver ? MAX_PLYRS : 2); i++)
		if (game_purchased & (1<<i))
			do_percent_aud(FULL_GAME_PURCHASE_AUD, GAME_START_AUD, FULL_GAME_PURCHASE_PCT_AUD);

	// audit initials entered
	for( i = 0; i < MAX_PLYRS; i++)
	{
		int shift_p = (four_plr_ver?1<<i:shift_2p[i]);
		if ((p_status & shift_p) &&
			(plyr_data_ptrs[i]->plyr_name[0] != '-') &&
			(plyr_data_ptrs[i]->plyr_name[0] != 0))
			do_percent_aud(INITIALS_ENTERED_AUD, GAME_START_AUD, INITIALS_ENTERED_PERCENT_AUD);
	}

////////////////////////////////////////
// initialize game_info
//
	for (i = 0; i < MAX_PLYRS; qtr_pstatus[i] = 0, i++);
	do_grand_champ = 0;

	record_me = 0;

//	if( game_info.game_state != GS_ATTRACT_MODE )
//	{
//		//Not in attract mode; set game state & clear last game stats
//		//game_info.game_state = GS_GAME_MODE;
//		game_info_last.game_quarter = 0;
//	}
	if (randrng(100) <= 20)
		away_team_style = ALTERNATE_JERSEY;
	else
		away_team_style = AWAY_JERSEY;

	game_info.game_mode    = GM_INITIALIZING;
	game_info.off_side     = 0;
	game_info.poss_change  = 0;
	game_info.shotclock    = 24;

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

// are we linked to another machine ??? HUH ???
	game_info.active_goal = 1;
	game_info.off_goal    = 1;

	// set to start at quarter 1
//#ifndef DEBUG_PLAYERS
//	game_info.game_quarter = 0;
//#else
//	game_info.game_quarter = 1;
//#endif

	// clear scores
	for (i=0; i < MAX_QUARTERS; i++)
	{
		game_info.scores[0][i] = 0;
		game_info.scores[1][i] = 0;
	}

	// clear foul counters
	for (i=0; i < NUM_PLAYERS; i++) {
		game_info.foul_cnt[i] = 0;
		game_info.hotspot_made_cnt[i] = 0;
		pushed_cnt[i] = 0;
	}

	// clear sound system stuff
	gSndTimer = 0;
	gWaitSayName = 0;
	last_shooter = -1;

//	game_info.scores[0][0] = 3;	//FIX!!! temp

// init everybody is a drone(-1)
//	for(i = 0; i < NUM_PLAYERS; i++)
//		game_info.plyr_control[i] = -1;
// set human controlled players
//	game_info.plyr_control[0] = 0;
//	game_info.plyr_control[1] = 1;
//	game_info.plyr_control[NUM_PERTEAM+0] = 2;
//	game_info.plyr_control[NUM_PERTEAM+1] = 3;

#ifdef MORPH
	set_process_run_level( qcreate( "morph", PLAYER_PID, morph_proc,  0, 0, 0, 0 ), RL_CURSORS);
	while( 1 )
		sleep( 1 );
#endif

////////////////////////////////////////
// Enable the disk interrupt callback
//
//	dio_mode(DIO_MODE_BLOCK);
	dio_mode(DIO_MODE_NO_BLOCK);

	// load game textures....
	load_ingame_textures();

	//load_arena_textures();

	Arena_proc(parg);


////////////////////////////////////////
// Kill the versus screen
//
	dio_mode(DIO_MODE_BLOCK);

	// Turn on transition here to kill vs screen; attract-mode does it there
	if (game_info.game_state != GS_ATTRACT_MODE)
	{
		// wait for VS screens procs to DIE
		while (existp(PRE_GAME_SCRNS_PID+1,0xFFFFFFFF))
			sleep(1);
		killall(PRE_GAME_SCRNS_PID, -1);
		turn_transit_on();

		//Not in attract mode; set game state & clear last game stats
		game_info.game_state = GS_GAME_MODE;
		game_info_last.game_quarter = 0;
	}

	unlock_multiple_textures(BARS_PG_TID,0xFFFFFFFF);
	del1c(OID_VERSUS,0xffffffff);
	delete_multiple_strings(89,-1);
	delete_multiple_strings(5, -1);
	delete_multiple_strings(6, -1);
	delete_multiple_strings(7, -1);
	delete_multiple_strings(8, -1);
	delete_multiple_textures(BARS_PG_TID,0xFFFFFFFF);
	vs_page_showing = 0;

	/* honor the tournament adjustment */
	if (!get_adjustment(TOURNAMENT_MODE_ADJ, &val))
		if (val && (
				(p_status & 0x3) && (p_status & 0xC) ))
			pup_tourney = 1;

	if (pup_tourney)
		clear_tourney_pups();
	
	if (pup_tourney)
		increment_audit(GAME_IN_TOURNEY_MODE_PRECENT_AUD);

	if (pup_rain)
		// Thunder
		snd_scall_bank(crowd_bnk_str,23,VOLUME7,127,PRIORITY_LVL5);

#ifndef DEBUG_PLAYERS
	load_plyr_txtrs(game_info.team[0], game_info.team[1]);
	load_referee_txtrs();
#else
	debug_load_plyr_txtrs(game_info.team[0]);
#endif

	/* honor the big head and drone big head privs */
	for (i = 0; i < MAX_PLYRS; i++)
	{
		if ((p_status & (1 << i)) && (selected_players[i].record_type == CREATP_RECORD))
		{
			if (selected_players[i].cmos_data.privileges & PRIV_BIG_HEAD)
			{
				pup_bighead |= 1 << i;
			}
			if (selected_players[i].cmos_data.privileges & PRIV_DRONE_BIG_HEAD)
			{
				teammate = 1 ^ i;
				if ((p_status & (1 << teammate)) == 0)
				{
					pup_bighead |= 1 << teammate;
				}
			}
		}
	}
	/* honor the stealth turbo priv */
	priv_stealth_turbo = 0;
	for (i = 0; i < MAX_PLYRS; i++)
	{
		if ((p_status & (1 << i)) && (selected_players[i].record_type == CREATP_RECORD))
		{
			if (selected_players[i].cmos_data.privileges & PRIV_STEALTH_TURBO)
			{
				priv_stealth_turbo |= 1 << i;
			}
		}
	}
	
	/* set the show shot % for your drone */
	for (i = 0; i < MAX_PLYRS; i++)
	{
		if ((pup_show_shotper & (1 << i)) && (p_status & (1 << i)))
		{
			teammate = 1 ^ i;
			if ((p_status & (1 << teammate)) == 0)
			{
				pup_show_shotper |= 1 << teammate;
			}
		}
	}

	force_special_weather();

	/* honor the head size adjustment */
	if (!get_adjustment(PLAYER_HEAD_SIZE_ADJ, &val) && val)
		pup_bighead = 0xF;

	// create the Arena
//	iqcreate( "Arena", FIELD_PID, Arena_proc, 0, 0, 0, 0 );



	// Create the drawing object to draw the arena
	create_object("court", OID_FIELD, OF_NONE, DRAWORDER_FIELD, NULL,   draw_court);
	create_object("goals", OID_GOALS, OF_NONE, DRAWORDER_GOALS, bbdata, draw_goals);

	do_court_fog();


#ifndef DEBUG_PLAYERS
	// create the ball
//	set_process_run_level( qcreate( "ball", PLAYER_PID, ball_proc, 0, 0, 0, 0 ), RL_BALL );
	set_process_run_level( qcreate( "ball", BALL_PID, ball_proc, 0, 0, 0, 0 ), RL_BALL );

#ifndef N_RPLAY
	// create the instant replay recorder proc
	set_process_run_level( qcreate( "replay", REPLAY_PID, replay_proc, 0, 0, 0, 0), RL_REPLAY );
#endif

	// create the player-to-player collision detect proc
	set_process_run_level( qcreate( "detp2p", PLAYER_PID, player_detect_proc, 0, 0, 0, 0), RL_COLLIS );
#endif

	// create the camera
	set_process_run_level( qcreate( "cambot", CAMBOT_PID, cambot_proc, 0, 0, 0, 0 ), RL_CAMBOT );

	players_ready = 0;

#ifndef DEBUG_PLAYERS
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
#else
	for (i = 0; i < teaminfo[game_info.team[0]].num_player; i++)
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

	// create the players -- use iqcreate because we want the game process
	// to happen before the player processes every tick.
	//								plyrnum, team, position, unused
	set_process_run_level( qcreate( "pre_plyr", PLAYER_PID+1, debug_pre_player_proc,  0, 0, 0, 0 ), RL_PLAYER-1);
	set_process_run_level( qcreate( "lastplyr", PLAYER_PID+1, debug_last_player_proc, 0, 0, 0, 0 ), RL_LASTPLAYER);
#endif

//fprintf(stderr,"selected_players = %d %d %d %d\n",selected_players[0],selected_players[1],selected_players[2],selected_players[3]);

#ifdef DEBUG_PLAYERS
	for(i = 0; i < teaminfo[game_info.team[0]].num_player; i++)
	{
		static int position[] = { GUARD, FORWARD, CENTER };
		static char szpn[] = "plyr_0";
		szpn[sizeof(szpn)-2] = '0' + (char)i;
		set_process_run_level(
			qcreate(szpn, PLAYER_PID, debug_player_proc,
				i,
				0,
				position[i % NUM_PERTEAM],
				selected_players[i].player_index),
			RL_PLAYER+i);
	}
#else
	for (i = 0; i < NUM_PLAYERS; i++)
	{
		static int position[] = { GUARD, FORWARD, CENTER };
		static char szpn[] = "plyr_0";
		szpn[sizeof(szpn)-2] = '0' + (char)i;
		set_process_run_level(
			qcreate(szpn, PLAYER_PID, player_proc,
				i,
				(i >= NUM_PERTEAM),
				position[i % NUM_PERTEAM],
				selected_players[i].player_index),
			RL_PLAYER+i);
	}
#endif

#ifndef DEBUG_PLAYERS
	// Create the cursors; proc dies after making the cursor textures & objs
	qcreate( "cursors", 0, cursor_proc, 0, 0, 0, 0 );
	if (!pup_no_hotspots)
		qcreate( "shwhtspt", 0, show_hotspot_proc, 0, 0, 0, 0 );
	//set_process_run_level( qcreate( "Shadows", 0, Player_Shadow_Proc, 0, 0, 0, 0 ), RL_CURSORS );

	// Create various clock & status box procs
	// <set_process_run_level> in this order so clock gets inited first
	set_process_run_level( qcreate("gameclk",  PLAYER_PID,     game_clock_proc,     0, 0, 0, 0), RL_FIRSTUP+0);
	set_process_run_level( qcreate("shtclk",   PLAYER_PID,     shot_clock_proc,     0, 0, 0, 0), RL_FIRSTUP+1);
	set_process_run_level( qcreate("shtclkbx", STATUS_BOX_PID, shot_clock_box_proc, 0, 0, 0, 0), RL_FIRSTUP+2);
	set_process_run_level( qcreate("score",    STATUS_BOX_PID, score_status_box,    0, 0, 0, 0), RL_FIRSTUP+3);

	// initialize player info displays
	plyrinfo_init();

	// Create score plate box & NBC logo
	qcreate( "scrplate", SCR_PLATE_PID, score_plate_proc,  0, 0, 0, 0);
	qcreate( "peacock",  SCR_PLATE_PID, show_peacock_proc, 0, 0, 0, 0);

#endif
	//qcreate( "plyrnms", PLYRINFO_PID, show_player_names_proc, 0, 0, 0, 0 );

#ifdef T_CONSL
	// Create console test proc
	qcreate( "t_consl", 0, test_console_proc, 0, 0, 0, 0 );
#endif //T_CONSL

#ifndef RELEASE_VER
 #ifndef TEST_VER
  #ifdef DEBUG
	// create the debug switches
	if (!existp( DEBUG_SW_PID, -1 ))
		set_process_run_level( qcreate( "debug", DEBUG_SW_PID, debug_switch_proc, 0, 0, 0, 0 ), RL_LASTUP);
	set_process_run_level( qcreate( "dbghalt", 0, watch_halt_proc, 0, 0, 0, 0 ), RL_LASTUP+1);
  #endif //DEBUG
 #endif //TEST_VER
#endif //RELEASE_VER

	// Let everything init itself first
	sleep(2);

	players_ready = 1;

	// Enable drawing
	draw_enable(1);

	// Make sure we are at full intensity if a fade was happening
	// when this got started
	normal_screen();

#ifndef NOAUDIO
	snd_stop_track(SND_TRACK_0);
	snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);
//	if (COURT_IS_INDOORS)
		snd_scall_bank(game_tune_bnk_str, 1, VOLUME3-30, 127, SND_PRI_SET|SND_PRI_TRACK0);
//	else
//		snd_scall_bank(game_tune_bnk_str, 1, VOLUME3, 127, SND_PRI_SET|SND_PRI_TRACK0);

	if (COURT_IS_INDOORS)
		snd_scall_bank(crowd_bnk_str, CROWD1_LOOP1_SND,VOLUME1, 127, SND_PRI_SET|SND_PRI_TRACK1);
	else if (which_court == COURT_GILMORE)
		snd_scall_bank(crowd_bnk_str, CROWD1_LOOP2_SND,VOLUME4, 127, SND_PRI_SET|SND_PRI_TRACK1);	// No weather
	else	/* court is GEER */
		snd_scall_bank(crowd_bnk_str, CROWD1_LOOP3_SND,VOLUME5, 127, SND_PRI_SET|SND_PRI_TRACK1);	// No weather
		//|| 	 ((float)player_blocks[receiver].flight_time / (float)player_blocks[receiver].total_time) > .75f) )


#endif

/*****************************************************************************/
/* main play loop ************************************************************/
/*****************************************************************************/
	while(( game_info.game_state == GS_GAME_MODE ) ||
		(( game_info.game_state == GS_ATTRACT_MODE ) && (1)))
	{
		// re-initialize drones
		init_drones();
		clock_active = 0;				// start game clock

		// (Re)init plyr controls
		// Necessary before tipoff/inbound
		set_plyr_control();

		// Reset to default values
		juke_flail_allow = ignore_pass = ignore_steal = ignore_shoot = 0;

#ifndef DEBUG_PLAYERS
#ifndef NO_TIP
		// Do tipoff
		if (game_info.game_quarter == 0 &&
			game_info.game_state != GS_ATTRACT_MODE)
//		if (game_info.game_quarter == 0)
		{	
			do_tipoff();

			clock_active = 1;				// start game clock
			shotclock_active = 1;
		}
		else
#endif
		{
			lineup_players_new_quarter();
#endif
			if (plates_on == PLATES_ON)
				turn_transit_off(0);

#ifndef DEBUG_PLAYERS
		}

		// Wait for any inbounding to finish
		while (game_info.game_mode == GM_INBOUNDING)
			sleep(1);

		qcreate("call_nme",0, announce_ball_handler_name_proc, 0, 0, 0, 0);

#ifndef N_RPLAY
		// Start the replay recorder
		set_recorder_mode( RP_START );
#endif
		// Start game-play mode
		game_info.game_mode = GM_IN_PLAY;

#else
		game_info.game_mode = GM_INBOUNDING;
		game_info.game_mode = GM_IN_PLAY;
		game_info.game_time = QUARTER_TIME;
#endif

		// Loop till clock is done
		while ((game_info.game_time != 0) && (!DBG_KO_BOOL))
		{
			// (Re)init plyr controls
			// Necessary in case someone bought in
			set_plyr_control();

			// Timers to always touch, even during a replay
			if (--gSndTimer < 0) gSndTimer    = 0;

			if(!halt)
			{
				// Timers not to touch during a replay
				if( ((game_info.game_time & 0x00FF0000)>> 16) == 0 && ((game_info.game_time & 0x0000FF00) >> 8) <= 7) ignore_shoot = 0;
				if((game_info.ball_handler != NO_BALLHANDLER
				&& player_blocks[game_info.ball_handler].odata.y == 0.0f) ||
				game_info.game_mode != GM_IN_PLAY)
				{ // only decrement if player is on the ground or the game_mode isn't GM_IN_PLAY
				  if(--ignore_shoot    < 0) ignore_shoot = 0;
				}
				if (--juke_flail_allow < 0) juke_flail_allow  = 0;
				if (--ignore_pass      < 0) ignore_pass  = 0;
				if (--ignore_steal     < 0) ignore_steal = 0;
				if (--gWaitSayName     < 0) gWaitSayName = 0;
				if (--gAssist_time     < 0) gAssist_time = 0;
			}

			if (game_info.ball_handler >= 0)
				last_handler = game_info.ball_handler;
			else
			if (ball_obj.who_shot >= 0)
				last_handler = ball_obj.who_shot;
			else
			if (game_info.ball_mode == BM_PASSING)
				last_handler = ball_obj.who_threw;

			sleep(1);
		}// WHILE (game_time != 0)

#ifdef DBG_KO
		if (dbg_ko)
		{
			game_info.game_mode = GM_PRE_TIPOFF;
			game_info.ball_handler = NO_BALLHANDLER;

			ball_obj.odata.x = 0.0f;
			ball_obj.odata.y = BALL_RADIUS;
			ball_obj.odata.z = -COURT_W;

			reset_shotclock = 1;
			shotclock_active = 0;
			reset_clock = 1;
			clock_active = 0;
//			said_latehit_sp = 0;
//			said_30_sp = 0;
//			said_20_sp = 0;
//			said_10_sp = 0;
//			said_5_sp = 0;
//		 	taunted = 0;

			// delete referee...for now
			delete_referee();

			// delete jump ball meter images...
			killall(JUMP_METER_PID, -1);
			del1c(OID_JUMP_METER, -4);
			hide_peacock = 0;

			dbg_ko = FALSE;

			sleep(1);
			continue;
		}
#endif //DBG_KO

		// check for END OF QUARTER
		if (game_info.game_time == 0)
		{
			start_enable(FALSE);

			// make sure ball is not flashing white
			ball_obj.flags &= ~BF_WHITE;

			// update qtr_pstatus & count active players
			active_players = 0;
			i = MAX_PLYRS;
			while (i--)
			{
				if (p_status & (1<<i))
				{
					qtr_pstatus[i] |= 1<<game_info.game_quarter;
					active_players += 1;
				}
			}

			// chalk quarters_played * XXX_PLAYER_AUD
			increment_audit(QUARTERS_PLAYED_AUD);
			increment_audit(ONE_PLAYER_AUD+active_players-1);
			recalc_percent_aud(ONE_PLAYER_AUD,QUARTERS_PLAYED_AUD,ONE_PLAYER_PERCENT_AUD);
			recalc_percent_aud(TWO_PLAYER_AUD,QUARTERS_PLAYED_AUD,TWO_PLAYER_PERCENT_AUD);
			recalc_percent_aud(THREE_PLAYER_AUD,QUARTERS_PLAYED_AUD,THREE_PLAYER_PERCENT_AUD);
			recalc_percent_aud(FOUR_PLAYER_AUD,QUARTERS_PLAYED_AUD,FOUR_PLAYER_PERCENT_AUD);
			recalc_percent_aud(TWOVCPU_PLAYER_AUD,QUARTERS_PLAYED_AUD,TWOVCPU_PLAYER_PERCENT_AUD);

			// human vs human?
			hvh = ((p_status & TEAM_MASK(0)) && (p_status & TEAM_MASK(1)));

			// maybe hit two-vs-cpu audit
			if (four_plr_ver && (active_players == 2) && !hvh)
				increment_audit( TWOVCPU_PLAYER_AUD );

			// now update all quarter-dependent percents
//			update_quarters_percents();

			switch (game_info.game_quarter)
			{
				case EOQ_1:
				case EOQ_3:
					if(game_info.game_quarter == EOQ_1)
					{
						add_to_audit( ONE_QUARTER_AUD, active_players );
						recalc_percent_aud( ONE_QUARTER_AUD, GAME_START_AUD, ONE_QUARTER_PERCENT_AUD);
					}
					else
					{
						add_to_audit( THREE_QUARTER_AUD, active_players );
						recalc_percent_aud( THREE_QUARTER_AUD, GAME_START_AUD, THREE_QUARTER_PERCENT_AUD);
					}

					eoq_handling();

					coaching_tip_scrn();
					if (buyin_screen())
						game_over(FALSE);
					else
						start_new_quarter();
					break;
				case EOQ_2:
					add_to_audit( TWO_QUARTER_AUD, active_players );
					recalc_percent_aud( TWO_QUARTER_AUD, GAME_START_AUD, TWO_QUARTER_PERCENT_AUD);

					eoq_handling();

					// blow out the player uniform textures (do we have to ??)
//					delete_multiple_textures(TXID_PLAYER,0xffffffff);
//					delete_multiple_textures(TXID_FIELD,0xffffffff);
					stats_page();
					if (buyin_screen())
						game_over(FALSE);
					else
					{
						halftime_substitutions();
						start_new_quarter();
					}
					break;
				case EOQ_4:
				case EOQ_OT1:
				case EOQ_OT2:
				case EOQ_OT3:
					if(game_info.game_quarter == EOQ_4)
					{
						add_to_audit( FOUR_QUARTER_AUD, active_players );
						recalc_percent_aud( FOUR_QUARTER_AUD, GAME_START_AUD, FOUR_QUARTER_PERCENT_AUD);
					}
					else if(game_info.game_quarter == EOQ_OT1)
					{
						add_to_audit( ONE_OVERTIME_AUD, active_players );
						recalc_percent_aud( ONE_OVERTIME_AUD, GAME_START_AUD, ONE_OVERTIME_PERCENT_AUD);
					}
					else if(game_info.game_quarter == EOQ_OT2)
					{
						add_to_audit( TWO_OVERTIME_AUD, active_players );
						recalc_percent_aud( TWO_OVERTIME_AUD, GAME_START_AUD, TWO_OVERTIME_PERCENT_AUD);
					}
					else
					{
						add_to_audit( THREE_OVERTIME_AUD, active_players );
						recalc_percent_aud( THREE_OVERTIME_AUD, GAME_START_AUD, THREE_OVERTIME_PERCENT_AUD);
					}

					// Either way, do end-of-quarter stuff
					eoq_handling();

					// All done if there is a winner or still tied at end of 3rd overtime
					if (check_scores() || game_info.game_quarter == MAX_QUARTERS)
					{
						// hit the FINISHED_GAMES audit
						increment_audit( FINISHED_GAMES_AUD );

						increment_audit( ONE_PLAYER_FINISH_AUD-1+active_players );

						if (four_plr_ver & (active_players == 2) && !hvh)
							increment_audit( TWOVCPU_PLAYER_FINISH_AUD );

						recalc_percent_aud( ONE_PLAYER_FINISH_AUD, FINISHED_GAMES_AUD,
							ONE_PLAYER_FINISH_PERCENT_AUD );
						recalc_percent_aud( TWO_PLAYER_FINISH_AUD, FINISHED_GAMES_AUD,
							TWO_PLAYER_FINISH_PERCENT_AUD );
						recalc_percent_aud( THREE_PLAYER_FINISH_AUD, FINISHED_GAMES_AUD,
							THREE_PLAYER_FINISH_PERCENT_AUD );
						recalc_percent_aud( FOUR_PLAYER_FINISH_AUD, FINISHED_GAMES_AUD,
							FOUR_PLAYER_FINISH_PERCENT_AUD );
						recalc_percent_aud( TWOVCPU_PLAYER_FINISH_AUD, FINISHED_GAMES_AUD,
							TWOVCPU_PLAYER_FINISH_PERCENT_AUD );

						if (hvh)
						{	// humans on both sides
							for (i = 0; i < NUM_PLAYERS; i++)
								if (selected_players[i].record_type == CREATP_RECORD) {
									increment_audit(WIN_VHUM_WITH_CREATEP_PRECENT_AUD);
									break;
								}
							increment_audit( HVH_FINISH_AUD );
							get_audit(TOTAL_HVSH_POINTS_AUD, &vaud);
							vaud += compute_score(0);
							vaud += compute_score(1);
							set_audit(TOTAL_HVSH_POINTS_AUD, vaud);
							if(!(get_audit(HVH_FINISH_AUD, &gaud)))
							{
								vaud /= gaud;
								set_audit(AVERAGE_HVSH_POINTS_AUD, vaud);
							}

							if(compute_score(0) > compute_score(1))
							{
								do_percent_aud(LEFT_SIDE_WINS_AUD, HVH_FINISH_AUD, LEFT_SIDE_WINS_PERCENT_AUD);
							}

							// Don't audit the tie games
							if(compute_score(0) != compute_score(1))
							{
								if(!(get_audit(TOTAL_WINNING_POINTS_AUD, &vaud)))
								{
									if(compute_score(0) > compute_score(1))
									{
										vaud += compute_score(0);
									}
									else
									{
										vaud += compute_score(1);
									}
									set_audit(TOTAL_WINNING_POINTS_AUD, vaud);
									if(!(get_audit(HVH_FINISH_AUD, &gaud)))
									{
										vaud /= gaud;
										set_audit(AVERAGE_WINNING_SCORE_AUD, vaud);
									}
								}

								if(!(get_audit(TOTAL_LOSING_POINTS_AUD, &vaud)))
								{
									if(compute_score(0) < compute_score(1))
									{
										vaud += compute_score(0);
									}
									else
									{
										vaud += compute_score(1);
									}
									set_audit(TOTAL_LOSING_POINTS_AUD, vaud);
									if(!(get_audit(HVH_FINISH_AUD, &gaud)))
									{
										vaud /= gaud;
										set_audit(AVERAGE_LOSING_SCORE_AUD, vaud);
									}
								}
							}
						}
						else
						{	// human vs cpu
							for (i = 0; i < NUM_PLAYERS; i++)
								if (selected_players[i].record_type == CREATP_RECORD) {
									increment_audit(WIN_VCPU_WITH_CREATEP_PRECENT_AUD);
									break;
								}
							increment_audit( HVC_FINISH_AUD );
							get_audit(TOTAL_HVSC_POINTS_AUD, &vaud);
							vaud += compute_score(0);
							vaud += compute_score(1);
							set_audit(TOTAL_HVSC_POINTS_AUD, vaud);
							if(!(get_audit(FINISHED_GAMES_AUD, &gaud)))
							{
								vaud /= gaud;
								set_audit(AVERAGE_HVSC_POINTS_AUD, vaud);
							}

							// Don't audit the tie games
							if(compute_score(0) != compute_score(1))
							{
								if (p_status2 & TEAM_MASK(0))
								{
									if(compute_score(0) < compute_score(1))
									{
										do_percent_aud(TOTAL_CPU_VICTORIES_AUD, HVC_FINISH_AUD, CPU_VICTORY_PERCENT_AUD);
										if(!(get_audit(LARGEST_CPU_VICTORY_AUD, &vaud)))
										{
											if(compute_score(1) - compute_score(0) > vaud)
											{
												set_audit(LARGEST_CPU_VICTORY_AUD, compute_score(1) - compute_score(0));
											}
										}
									}
									else
									{
										if(!(get_audit(LARGEST_CPU_LOSS_AUD, &vaud)))
										{
											if(compute_score(0) - compute_score(1) > vaud)
											{
												set_audit(LARGEST_CPU_LOSS_AUD, compute_score(0) - compute_score(1));
											}
										}
									}
								}
								else
								{
									if(compute_score(0) > compute_score(1))
									{
										do_percent_aud(TOTAL_CPU_VICTORIES_AUD, HVC_FINISH_AUD, CPU_VICTORY_PERCENT_AUD);
										if(!(get_audit(LARGEST_CPU_VICTORY_AUD, &vaud)))
										{
											if(compute_score(0) - compute_score(1) > vaud)
											{
												set_audit(LARGEST_CPU_VICTORY_AUD, compute_score(0) - compute_score(1));
											}
										}
									}
									else
									{
										if(!(get_audit(LARGEST_CPU_LOSS_AUD, &vaud)))
										{
											if(compute_score(1) - compute_score(0) > vaud)
											{
												set_audit(LARGEST_CPU_LOSS_AUD, compute_score(1) - compute_score(0));
											}
										}
									}
								}
							}
						}

						recalc_percent_aud( HVH_FINISH_AUD,
											FINISHED_GAMES_AUD,
											HVH_FINISH_PERCENT_AUD );

//$						if((p_status2 & 3) == 3)
//$						{
//$							if(compute_score(0) > compute_score(1))
//$							{
//$								do_percent_aud(LEFT_SIDE_WINS_AUD, TWO_PLAYER_AUD, LEFT_SIDE_WINS_PERCENT_AUD);
//$							}
//$						}

						// Save stats for attract mode display
//						game_info_last = game_info;
//						tdata_blocks_last[0] = tdata_blocks[0];
//						tdata_blocks_last[1] = tdata_blocks[1];
						game_info.game_mode = GM_GAME_OVER;

						// blow out the player uniform textures (do we have to ??)
//						delete_multiple_textures(TXID_PLAYER,0xffffffff);
//						delete_multiple_textures(TXID_FIELD,0xffffffff);

						// Set record-to-cmos bits
						i = (1 << game_info.game_quarter) - 1;
						for (pnum = 0; pnum < MAX_PLYRS; pnum++)
						{
							// Set record flag if valid initials
							if ((plyr_data_ptrs[pnum]->plyr_name[0] != 0) &&
								(plyr_data_ptrs[pnum]->plyr_name[0] != '-') &&
								(selected_players[pnum].record_num != -1) &&
								(qtr_pstatus[pnum] == i))
							{
								record_me |= 1 << pnum;
							}
						}


						// Do game finishing screens
						stats_page();
						game_over(FALSE);

						// Grand Champ stuff happens next
						if (do_grand_champ)					//(1)
						{
							show_grand_champ_movie(0, 0);
						}

						// This is where the free stuff is awarded - free stuff is
						// only awarded if the player(s) bought a full game up front

						// Make humans-were-involved mask
						for (pnum = MAX_PLYRS-1, i = 0; pnum >= 0; pnum--)
						{
							i <<= 1;
							if (qtr_pstatus[pnum]) i++;
						}

						// Determine if & who to chk for free stuff
						while (1)
						{
							// Set to chk H vs H scores
							pnum = -1;

							// HH vs HH and full game was bought up front?
							if (!get_adjustment(H4_FREE_GAME_ADJ, &val))
								if (old_game_purchased == 15)
									break;

							// H vs H and full game was bought up front?
							if (!get_adjustment(HH_FREE_GAME_ADJ, &val))
								if ((old_game_purchased == 5  && i == 5) ||
									(old_game_purchased == 6  && i == 6) ||
									(old_game_purchased == 9  && i == 9) ||
									(old_game_purchased == 10 && i == 10))
									break;

							// H vs CPU and full game was bought up front?
							if (!get_adjustment(HC_FREE_GAME_ADJ, &val))
							{
								// Set to chk team 1 H vs CPU scores
								pnum = 0;
								if (four_plr_ver)
								{
									if (old_game_purchased == 3  && i == 3)
										break;

									// The Europe Rules
									if (old_game_purchased == 1  && i == 1)
										break;
									if (old_game_purchased == 2  && i == 2)
										break;
								}
								else if (old_game_purchased == 1 && i == 1)
									break;

								// Set to chk team 2 H vs CPU scores
								pnum = 1;
								if (four_plr_ver)
								{
									if (old_game_purchased == 12 && i == 12)
										break;

									// The Europe Rules
									if (old_game_purchased == 4  && i == 4)
										break;
									if (old_game_purchased == 8  && i == 8)
										break;
								}
								else if (old_game_purchased == 4 && i == 4)
									break;
							}

							// No free stuff to chk!
							val = 0;
							break;
						}

						// Do the chk if set
						if (val)
						{
							// Did team 1 beat team 2 & was it allowed ?
							if (compute_score(0) > compute_score(1) && pnum != 1)
							{
								// YES - Give team 1 the free game
								pnum = 0;
							}
							else
							// Did team 2 beat team 1 & was it allowed ?
							if (compute_score(1) > compute_score(0) && pnum != 0)
							{
								// YES - Give team 2 tthe free game
								pnum = 1;
							}
							else
								// Tie game or not allowed - no free stuff!
								pnum = -1;

							// If at this point pnum is >= 0 it indicates a team
							// is being awarded free stuff; it only gets free stuff
							// if its last game DID NOT have any free stuff.
							if (pnum >= 0)
							{
								if (!free_game_count[pnum])
								{
									// Reset other teams free game count
									free_game_count[pnum ^ 1] = 0;

									val = (four_plr_ver) ?
										((pnum) ? 12 : 3):
										((pnum) ? 4  : 1);

									// Tell the team it has a free game coming
									// He/they MUST press start BEFORE timer on this
									// screen runs out to actually get the free game.
									if (show_player_free_game(old_game_purchased & val))
									{
										// Put the player/team in the game
										p_status = old_game_purchased & val;
										p_status2 = p_status;
										game_purchased = p_status;

										// Increment this teams free game count
										// (except it was a 4 plyr - let them repeat)
										if (old_game_purchased != 15)
											free_game_count[pnum]++;

										// Set game state to pre-game
										game_info.game_state   = GS_PRE_GAME;
										game_info.game_mode    = GM_INITIALIZING;
										game_info.game_quarter = 0;

										// Give team the free stuff
										for (i = 0; i < MAX_PLYRS; i++)
										{
											if (p_status & (1<<i))
											{
												pdata_blocks[i].quarters_purchased = 4;
												// Chalk a free stuff
												increment_audit(FREE_GAME_AUD);
											}
											else
											{
												pdata_blocks[i].quarters_purchased = 0;
											}
										}

										// Start the pre-game screens
										qcreate("pre_game", PRE_GAME_SCRNS_PID, pre_game_scrns, 0, 0, 0, 0);
	//									qcreate("pre_game", PRE_GAME_SCRNS_PID, pre_game_scrns, 1, 0, 0, 0);

										// Kill myself
										die(0);
									}
								}
							}
						}

						// Game is totally over if it gets here
						free_game_count[0] = 0;
						free_game_count[1] = 0;
						game_info.game_state = GS_GAME_OVER;
					}
					else
					{
						coaching_tip_scrn();
						if (buyin_screen())
							game_over(FALSE);
						else
							start_new_quarter();
					}
					break;
			} // end SWITCH
		}
		sleep(1);
	} // while GS_GAME_MODE

	// Create the attract mode loop
	iqcreate("attract", AMODE_GRP_PID, attract_loop, 0, 0, 0, 0);
}


//---------------------------------------------------------------------------------------------------------------------------
// Load in announcer speech call for each player
//---------------------------------------------------------------------------------------------------------------------------
void load_player_speech_banks(void)
{
int i = 0, team = 0;

	for(i = 0; i < NUM_PLAYERS; i++)
	{
		team = (i >= NUM_PERTEAM);
		sprintf(plyr_bnks[i], "sc_%s",
				selected_players[i].sound_bank);
		if(snd_bank_load_playing(plyr_bnks[i]) == 0xeeee)	// announcer player speech bank
		{

//FIX!!!  Make sure all printf's are in DEBUG only
#ifdef DEBVG
			printf("player bank load failed: %s\n", plyr_bnks[i]);
#endif
			increment_audit(SND_LOAD_FAIL_AUD);
		}
	}
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void unload_player_speech_banks(void)
{
	int i;
	
	// must delete in opposite order loaded
	for (i = NUM_PLAYERS-1; i >= 0; i--)
		snd_bank_delete(plyr_bnks[i]);
}

//---------------------------------------------------------------------------------------------------------------------------
// Load in announcer speech call for each team
//---------------------------------------------------------------------------------------------------------------------------
void load_team_speech_banks(void)
{
	int i = 0, team = 0;

	// team 1
	sprintf(team1_bnk_str, "tm_%s", team_speech_banks[game_info.team[0]]);
	if(snd_bank_load_playing(team1_bnk_str) == 0xeeee)	// announcer player speech bank
	//if(snd_bank_load(team1_bnk_str) == 0xeeee)	// announcer player speech bank
	{
#ifdef	DEBUG
		printf("team 1 bank load failed: tm_%s\n", team_speech_banks[game_info.team[0]]);
#endif
		increment_audit(SND_LOAD_FAIL_AUD);
	}
	// team 2
	sprintf(team2_bnk_str, "tm_%s", team_speech_banks[game_info.team[1]]);
	//if(snd_bank_load(team2_bnk_str) == 0xeeee)	// announcer player speech bank
	if(snd_bank_load_playing(team2_bnk_str) == 0xeeee)	// announcer player speech bank
	{
#ifdef	DEBUG
		printf("team 2 bank load failed: tm_%s\n", team_speech_banks[game_info.team[1]]);
#endif
		increment_audit(SND_LOAD_FAIL_AUD);
	}
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void unload_team_speech_banks(void)
{
	snd_bank_delete(team2_bnk_str);
	snd_bank_delete(team1_bnk_str);
}


//---------------------------------------------------------------------------------------------------------------------------
// Load general textures needed during gameplay
//--------------------------------------------------------------------------------------------------------------------------
void load_ingame_textures(void)
{
	static tmu_ram_texture_t ingame_tex[] =
	{
		{"ARROWS.WMS",   SCR_PLATE_TID},
		{"TURBAR.WMS",   SCR_PLATE_TID},
		{"MSGS.WMS",     SCR_PLATE_TID},
		{"JMETER.WMS",   SCR_PLATE_TID},
		{"SCINFO00.WMS", SCR_PLATE_TID},
		{"SCINFO01.WMS", SCR_PLATE_TID},
		{"SCINFO02.WMS", SCR_PLATE_TID},
		{"SCINFO03.WMS", SCR_PLATE_TID},
		{"LETTRS00.WMS", MESSAGE_TID},
		{"LETTRS01.WMS", MESSAGE_TID},
		{"PNAMES00.WMS", SCR_PLATE_TID},
		{"PNAMES01.WMS", SCR_PLATE_TID},
		{"PNAMES02.WMS", SCR_PLATE_TID},
		{"PNAMES03.WMS", SCR_PLATE_TID},
		{"PNAMES04.WMS", SCR_PLATE_TID},
		{"PNAME2.WMS",	 SCR_PLATE_TID},
		{"PLYBALL.WMS",	 SCR_PLATE_TID},
		{"TRANS00.WMS",  TRANS_TID},
		{"TRANS01.WMS",  TRANS_TID},
		{"TRANS02.WMS",  TRANS_TID},
		{"TRANS03.WMS",  TRANS_TID},
		{"TRANS04.WMS",  TRANS_TID},

		// replay transition
		{"NBCFLA00.WMS", SPFX_TID},
		{"NBCFLA01.WMS", SPFX_TID},
		{"NBCFLA02.WMS", SPFX_TID},
		{"NBCFLA03.WMS", SPFX_TID},

		// Added - 3/13/99 - 3 ptr flash
		{"plqfsh00.wms", SPFX_TID},
		{"plqfsh01.wms", SPFX_TID},
		{"plqfsh02.wms", SPFX_TID},
		{"plqtop00.wms", SPFX_TID},
		{"plqtop01.wms", SPFX_TID},

		{"firebr.wms",   SCR_PLATE_TID},
		{"bfirebr.wms",  SCR_PLATE_TID},

		// added 21 Mar 99 - Jason, ingame buyin
		{"joinin00.wms", JOININ_TID},
		{"joinin01.wms", JOININ_TID},
		{"joinin02.wms", JOININ_TID},
		{"plrcrd00.wms", JAPPLE_BOX_TID},
		{"plrcrd01.wms", JAPPLE_BOX_TID},

		{"QTR11.WMS",    0x666},
		{"QTR12.WMS",    0x666},
		{"QTR21.WMS",    0x666},
		{"QTR22.WMS",    0x666},
		{"QTR31.WMS",    0x666},
		{"QTR32.WMS",    0x666},
		{"QTR41.WMS",    0x666},
		{"QTR42.WMS",    0x666},

// halftime subs textures
	 {"optbk00.wms",BKGRND_TID},			// 256x256
	 {"optbk01.wms",BKGRND_TID},
	 {"optbk02.wms",BKGRND_TID},

	 {"tmsel01.wms",TM_SELECT_TID},			// 128x256
	 {"tmsel02.wms",TM_SELECT_TID},			// 128x256
	 {"tmsel03.wms",TM_SELECT_TID},			// 128x256
	 {"tmsel04.wms",TM_SELECT_TID},			// 128x256

	 {"selplr00.wms",TM_SELECT_TID},
	 {"selplr01.wms",TM_SELECT_TID},

	 {"cntnbr.wms",CNTDWN_TID},
	 
		{NULL,0}
	};
	int i;

	/* load the particle system textures */
	ptl_init();
	
	// Load the SPECIAL EFFECTS textures
#if 0
	for (i=0; i < NUM_FIRE_TXTRS; i++)
	{
		//sprintf( spfx_texture, "onfire%02d.WMS", i+1);
		sprintf( spfx_texture, "trail%d.WMS", i+1);

		smkpuf_decals[i] = create_texture(spfx_texture, SPFX_TID, 0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
		//chk_game_proc_abort();

		if (!smkpuf_decals[i])
		{
#ifdef DEBUG
			fprintf( stderr,"Error loading special effect texture smoke%d, \r\n",i+1);
#endif
			//increment_audit(TEX_LOAD_FAIL_AUD);
		}
	}
#endif

	// Load the texture table
	if(load_textures_into_tmu(ingame_tex))
	{
#ifdef DEBUG
		fprintf( stderr,"Error loading %s\r\n","ingame_tex[]");
#endif
	}


	// Load in team 1 logo/scoreplate name
	if(!(create_texture(team_name_tex[game_info.team[0]], SCR_PLATE_TID, 0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP,
			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR)))
	{
#ifdef DEBUG
		fprintf( stderr,"Error loading %s\r\n","Team 1.WMS");
#endif
	}

	// Load in team 2 logo/scoreplate name
	if(!(create_texture(team_name_tex[game_info.team[1]], SCR_PLATE_TID, 0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP,
			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR)))
	{
//#ifdef DEBUG
//		fprintf(stderr, "loaded tex:%s  handle:%d\r\n" ,team_name_tex[game_info.team[1]], (int)tex_node->texture_handle);
//#endif
#ifdef DEBUG
		fprintf( stderr,"Error loading %s\r\n","Team 2.WMS");
#endif
	}

	lock_multiple_textures(SPFX_TID,0xffffffff);
	lock_multiple_textures(SCR_PLATE_TID,0xffffffff);
	lock_multiple_textures(MESSAGE_TID,0xffffffff);
	lock_multiple_textures(TRANS_TID,0xffffffff);
	lock_multiple_textures(JOININ_TID,0xffffffff);
	lock_multiple_textures(0x666,0xffffffff);
	
	lock_multiple_textures(JAPPLE_BOX_TID,0xffffffff);
	lock_multiple_textures(BKGRND_TID,0xffffffff);
	lock_multiple_textures(TM_SELECT_TID,0xffffffff);
	lock_multiple_textures(CNTDWN_TID,0xffffffff);
	
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void start_new_quarter(void)
{
	register int i;

	// Try and keep the heap clean!
	clear_free_process_list();

	// this might be a bad location for this sound stuff...re-think later.
#ifndef NOAUDIO
//	coin_sound_holdoff(TRUE);

	snd_stop_track(SND_TRACK_0);			// stop tune
	// track 0 is the tune, track 1 is the audience, track 5 is the announcer
	snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);

	// re-start main_play tune and audience sounds
//	if (COURT_IS_INDOORS)
		snd_scall_bank(game_tune_bnk_str, 1, VOLUME3-30, 127, SND_PRI_SET|SND_PRI_TRACK0);
//	else
//		snd_scall_bank(game_tune_bnk_str, 1, VOLUME3, 127, SND_PRI_SET|SND_PRI_TRACK0);



	if (COURT_IS_INDOORS)
		snd_scall_bank(crowd_bnk_str, CROWD1_LOOP1_SND,VOLUME1, 127, SND_PRI_SET|SND_PRI_TRACK1);
	else if (which_court == COURT_GILMORE)
		snd_scall_bank(crowd_bnk_str, CROWD1_LOOP2_SND,VOLUME4, 127, SND_PRI_SET|SND_PRI_TRACK1);	// No weather
	else	/* court is GEER */
		snd_scall_bank(crowd_bnk_str, CROWD1_LOOP3_SND,VOLUME5, 127, SND_PRI_SET|SND_PRI_TRACK1);	// No weather


//	coin_sound_holdoff(FALSE);
#endif

	// initialize player info displays - we deleted them upon going to buyin screen/coaching tips

	// Clr player continue flags if not in-game
	i = 0;
	do {
		if (!(p_status & (1<<i)))
			p_status2 &= ~(1<<i);
	} while (++i < MAX_PLYRS);

	// reset foul counts
	for (i=0; i < NUM_PLAYERS; i++)
		game_info.foul_cnt[i] = 0;

	unhide_all_3d_objects();
	hide_multiple_objects(OID_REPLAY, -1 );

	resume_all();

	showhide_flag   = SHOW;		// Turbo bars
	hide_status_box = SHOW;		// Top score box

	// Make sure starts are enabled
	start_enable(TRUE);

//	// If NOT in attract mode, disable starts after 30 seconds
//	if(game_info.game_state != GS_ATTRACT_MODE)
//	{
//		qcreate("soff", 0, start_off_proc, 0, 0, 0, 0);
//	}

	// Re-create score plate box & NBC logo
	qcreate( "scrplate", SCR_PLATE_PID, score_plate_proc,  0, 0, 0, 0);
	qcreate( "peacock",  SCR_PLATE_PID, show_peacock_proc, 0, 0, 0, 0);

	game_info.game_state = GS_GAME_MODE;

	reset_clock = 1;

	// Set appropriate team to take out the ball
	if ((game_info.game_quarter & 1) == (game_info.off_side & 1))
	{
		game_info.active_goal = 1 - game_info.active_goal;
		game_info.off_side    = 1 - game_info.off_side;
	}

	lineup_players_new_quarter();

	// Set # of ticks camera needs to know about the new quarter
	// Currently this is 1 for the exit back to game_proc(), after which
	//  everything gets setup, then 1 to look at it
	cambot.new_quarter = 2;

	qcreate("call_nme",0, announce_ball_handler_name_proc, 0, 0, 0, 0);

	// Don't need a sleep here; we get one when this exits back to game_proc()
}

//------------------------------------------------------------------------------------------------------------------------------
//	This process welcomes the player with some crowd stomps
//
// 	INPUT:	Nothing
//	OUTPUT:	Nothing
//------------------------------------------------------------------------------------------------------------------------------
void crowd_stomp_proc(int *args)
{
// Test
//	printf("Crowd stomp proc start\n");
	sleep(7*57);
	snd_scall_bank(crowd_bnk_str, CROWD1_STOMP_SND , VOLUME5, 127, PRIORITY_LVL1);
	sleep(90);
	snd_scall_bank(crowd_bnk_str, CROWD1_STOMP_SND , VOLUME5, 127, PRIORITY_LVL1);
	sleep(90);
	snd_scall_bank(crowd_bnk_str, CROWD1_STOMP_SND , VOLUME5, 127, PRIORITY_LVL1);
	snd_scall_bank(crowd_bnk_str, CROWD2_SWELL1_SND, (randrng(100) < 50) ? VOLUME7 : VOLUME5, 127, PRIORITY_LVL3);
	sleep(90);
	snd_scall_bank(crowd_bnk_str, CROWD1_STOMP_SND , VOLUME5, 127, PRIORITY_LVL1);
	sleep(90);
	snd_scall_bank(crowd_bnk_str, CROWD1_STOMP_SND , VOLUME5, 127, PRIORITY_LVL1);
	sleep(90);
	snd_scall_bank(crowd_bnk_str, CROWD1_STOMP_SND , VOLUME5, 127, PRIORITY_LVL1);
	sleep(90);
	snd_scall_bank(crowd_bnk_str, CROWD1_STOMP_SND , VOLUME5, 127, PRIORITY_LVL1);
}


//-------------------------------------------------------------------------------------------------
//					This routine compares the players scores
//
// RETURNS: TRUE if someone is winning, else FALSE
//-------------------------------------------------------------------------------------------------
int check_scores(void)
{
	return (compute_score(0) - compute_score(1));
}

//-------------------------------------------------------------------------------------------------
// RETURNS: team <tm_nbr> score
//-------------------------------------------------------------------------------------------------
int compute_score(int tm_nbr)
{
int scr, i;

	scr = 0;
	for (i=0; i < MAX_QUARTERS; i++)
		scr += game_info.scores[tm_nbr][i];
	return(scr);
}


//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
//void start_off_proc(int *args)
//{
//	while((game_info.game_time & 0x00ffffff) > 0x00011d18)	// 01:29:23
//	{
//		sleep(1);
//	}
//	start_enable(FALSE);
//}

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
void eoq_handling(void)
{
	clock_active = 0;
	shotclock_active = 0;

//	fade(0.8f, 3, NULL);

	// try to get crowd cheering at end of quarter
	crowd_cheer_time = 60*5;

	// eoq horn
	snd_scall_bank(gameplay_bnk_str,HORN_SND,VOLUME6,127,PRIORITY_LVL4);

	killall(MESSAGE_PID, -1);
	del1c(OID_MESSAGE , -1);

	// kill any speech procs....waiting
	gSndTimer = 0;
	killall(SPEECH_PID, -1);

	// Kill possible shot_clock_voilation proc
//	killall(VIOLATION_PID, -1);
	killall(SCR_PLATE_PID,-1);					// Yardage info stuff
	del1c(OID_SCRPLATE,-1);
	delete_multiple_strings(SCOREPLATE_CLOCK_TXT_ID, -1);
	delete_multiple_strings(SCOREPLATE_TXT_ID, -1);

	// wait for ball to hit rim or ground....before truly ending quarter!
	while ((game_info.ball_mode != BM_FREE_ON_COURT) && (game_info.ball_handler < 0))
		sleep(1);

	showhide_flag   = HIDE;		// Turbo bars
	hide_status_box = HIDE;		// Top score box

	suspend_multiple_processes( PLAYER_PID, -1 );
	suspend_multiple_processes( BALL_PID, -1 );
//	suspend_multiple_processes(PLAYER_PID+1, -1);
//	resume_multiple_processes( CAMBOT_PID, -1 );

	qcreate("eoqmsg",EOQ_PID, eoq_score_plate_proc, 0, 0, 0, 0);

	// Don't need to sleep since EOQ_PID now exists!

#ifndef NOAUDIO
//	coin_sound_holdoff(TRUE);

	snd_stop_track(SND_TRACK_0);
	snd_stop_track(SND_TRACK_1);
	// track 0 is the tune, track 1 is the audience, track 5 is the announcer
	snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);
	// play EOQ transition music
	snd_scall_bank(qtr_trans_bnk_str, 1, VOLUME7, 127, PRIORITY_LVL5);

//	coin_sound_holdoff(FALSE);
#endif


	// Wait until message times out
	game_info.game_state = GS_EOQ;
//	while (existp(EOQ_PID, -1))
	while (existp(EOQ_PID, -1) || existp(VIOLATION_PID, -1))
	{
		sleep(1);
	}

//	if (!i)
//		dio_mode(DIO_MODE_BLOCK);

	// Delete any 3 pt message
	del1c(OID_3PT_MSG,0xFFFFFFFF);

	// Turn on the plates
	turn_transit_on();


//#ifndef NOAUDIO
//	coin_sound_holdoff(TRUE);
//	snd_bank_delete(game_tune_bnk_str);
//
//	// swap MAKE shot speech bank
//	if (game_info.game_quarter == 1)
//	{
//		snd_bank_delete(anc_make_bnk);
//		strcpy(anc_make_bnk, "sca_mak2");
//		if (snd_bank_load_playing(anc_make_bnk) == 0xeeee)
//			increment_audit(SND_LOAD_FAIL_AUD);
//	}
//
//	// If game isn't over, load next quarter game tune
//	if ((unsigned int)game_info.game_quarter < (unsigned int)MAX_QUARTERS)
//	{
//		strcpy(game_tune_bnk_str, game_tunes_tbl[game_info.game_quarter]);
//		if (snd_bank_load_playing(game_tune_bnk_str) == 0xeeee)
//		{
//			increment_audit(SND_LOAD_FAIL_AUD);
//			#if DEBUG
//			fprintf(stderr,"error loading game tune\n");
//			#endif
//		}
//	}
//
//	coin_sound_holdoff(FALSE);
//#endif

	// Do this here so turbo bars don't freak out
	paid_credits_deduct();

	/* update CMOS */
	write_cmos_to_disk(TRUE);
	update_other_cmos_to_disk(TRUE);

	hide_status_box = HIDE;		// Top score box
	sleep(1);					// Hide'em

	suspend_multiple_processes(STATUS_BOX_PID, -1);

	// Delete EOQ score plate
	killall(EOQ_PID, -1);
	killall(MESSAGE_PID, -1);
	del1c(OID_SCRPLATE,-1);
	delete_multiple_strings(BIG_SCOREPLATE_TXT_ID,   -1);
	delete_multiple_strings(SCOREPLATE_CLOCK_TXT_ID, -1);

//	game_info.game_state = GS_EOQ;
	game_info.game_quarter++;

	hide_all_3d_objects();
}

//--------------------------------------------------------------------------------------------------
//						This routine handles getting the game from GAME_STATE to ATTRACT MODE state
//-------------------------------------------------------------------------------------------------
void game_over(int ingame)
{
	int		i;
	int		cts,ctc;

	clear_pups();
	bought_in_first = 0;
	show_them_hiscore_tables_first = 0;
	old_game_purchased = game_purchased;
	game_purchased = 0;

	ctc = get_credits_to_continue();
	cts = get_credits_to_start();

	if( !ingame )
		p_status = 0;

	p_status2 = 0;

	if( ingame )
		p_status2 = p_status;

	if (ingame)
	{
		for( i = 0; i < MAX_PLYRS; i++ )
		{
//			if (p_status & (1<<i))
//				pdata_blocks[i].quarters_purchased -= game_info.game_quarter;

			if (pdata_blocks[i].quarters_purchased)
			{
				pdata_blocks[i].credits_paid = cts +
					(pdata_blocks[i].quarters_purchased-1)*ctc;
			}
			else
			{
				pdata_blocks[i].credits_paid = 0;
			}

			if (get_full_game_credits() == pdata_blocks[i].credits_paid)
				game_purchased |= (1<<i);
		}
	}
	else
	{
		pdata_blocks[0].quarters_purchased = 0;
		pdata_blocks[1].quarters_purchased = 0;
		pdata_blocks[2].quarters_purchased = 0;
		pdata_blocks[3].quarters_purchased = 0;
	}

	player_blocks[0].auto_control = -1;
	player_blocks[1].auto_control = -1;
	player_blocks[2].auto_control = -1;
	player_blocks[3].auto_control = -1;

	which_court = COURT_INDOOR;
	game_info.game_state = GS_GAME_OVER;
	
	// save cmos records if end of game and there is a winner!
	if (((game_info.game_quarter > EOQ_4 && check_scores()) ||
		(game_info.game_quarter == MAX_QUARTERS)) &&
		!ingame)
	{
		save_player_records();
		check_world_records();

		snd_scall_bank(plyr_bnk_str,RICOCHET_SND,VOLUME2,127,PRIORITY_LVL5);
		show_final_player_stats();
	}

//	game_over_screen();
//	turn_transit_off(0);

	// reset state ram....and stuff
	game_info.game_quarter = 0;


#ifndef NOAUDIO
	// !!!FIX (probably play tune here...or something)

	// kill game tune
	snd_stop_track(SND_TRACK_0);
	// kill audient crowd noise
	snd_stop_track(SND_TRACK_1);
	snd_set_reserved(SND_TRACK_0);
	
	snd_bank_delete(game_tune_bnk_str);
	snd_bank_delete(anc_make_bnk);

	// remove the player sound banks
	unload_player_speech_banks();
	unload_team_speech_banks();

	snd_bank_delete(crowd_bnk_str);

	coin_sound_holdoff(TRUE);

//	load_permanent_snds();

	// load GAME START movie tune
	if (snd_bank_load_playing(start_movie_snd_bnk_str) == 0xeeee)
		increment_audit(SND_LOAD_FAIL_AUD);

	coin_sound_holdoff(FALSE);
#endif
	// Disable drawing
	draw_enable(0);

	// do END-OF-GAME clean up
	wipeout();

//	turn_transit_on();

	ptl_shutdown();
	unload_player_heads();
	unload_player_models();

	// Unhide turbo bars/text for attract game play
	showhide_flag   = SHOW;		// Turbo bars
	hide_status_box = SHOW;		// Top score box
}


//---------------------------------------------------------------------------------------------------------------------------
//			This process controls the GAME CLOCK.
//---------------------------------------------------------------------------------------------------------------------------

// Ticks per second adjustment
#ifndef SHORT_QTR
static int time_table[5] =			//FIX!!!
{
	37, //30,						// EXTRA SLOW   APPROX. PERIOD TIME 4:00
	32, //25,						// SLOW				  APPROX. PERIOD TIME 3:30
	27, //20,						// MEDIUM			  APPROX. PERIOD TIME 2:00
	25, //18,						// FAST				  APPROX. PERIOD TIME 2:00
	23  //16						// EXTRA FAST   APPROX. PERIOD TIME 1:45
};
#else
static int time_table[5] =			//FIX!!!
{
	1, //30,						// EXTRA SLOW   APPROX. PERIOD TIME 4:00
	1, //25,						// SLOW				  APPROX. PERIOD TIME 3:30
	1, //20,						// MEDIUM			  APPROX. PERIOD TIME 2:00
	1, //18,						// FAST				  APPROX. PERIOD TIME 2:00
	1  //16						// EXTRA FAST   APPROX. PERIOD TIME 1:45
};
#endif


#define CNT_TIME_TABLE	(sizeof(time_table)/sizeof(int))

static void game_clock_proc(int *args)
{
	int eoq_flg = 0;

	// Get cmos ticks-per-second setting to index into <time_table>
	if (get_adjustment(TIME_ADJ, &cmos_ticks_per_second))
		cmos_ticks_per_second = -1;

	if (cmos_ticks_per_second < 0 || cmos_ticks_per_second >= CNT_TIME_TABLE)
	{
#ifdef DEBUG
		fprintf(stderr, "Bogus CMOS ticks-per-second value: %d\n", cmos_ticks_per_second);
		lockup();
#endif
		// Default for bad CMOS read
		cmos_ticks_per_second = 3;
	}
#ifdef DEBUG
	fprintf(stderr, "CMOS ticks-per-second value 1: %d\n", cmos_ticks_per_second);
#endif
	cmos_ticks_per_second = time_table[cmos_ticks_per_second];
#ifdef DEBUG
	fprintf(stderr, "CMOS ticks-per-second value 2: %d\n", cmos_ticks_per_second);
#endif

//	clock_active = 1;			//FIX!!! Does this wanna happen here?!
	reset_clock = 1;

	// Main loop
	while (1)
	{
		if (reset_clock)
		{
#ifdef NO_CLOCK
			// Set clock to 1:59 - some flags may look at clock
			game_info.game_time = 0x013B00;
#else
			// Set the clock
			game_info.game_time = QUARTER_TIME;
//			game_info.game_time = 0x00010900;	// Debug - 1 minute
#endif
#ifdef DEBUG
			fprintf(stderr,"Clock = %02d:%02d\r\n",
				(game_info.game_time>>16)&255, (game_info.game_time>>8)&255);
#endif
			eoq_flg = 0;
			reset_clock = 0;
		}

#ifndef NO_CLOCK
		if (clock_active && game_info.game_state != GS_ATTRACT_MODE)
		{
			// Get current time
			register int gt = game_info.game_time;

			// Subtract a tenth if nonzero
			if (gt & 0x000000FF)
				gt -= 0x00000001;
			else
			{
				// Second has passed; bump up play time cntr
//				game_info.play_time++;
//				tdata_blocks[game_info.off_side].time_of_poss += 1;

				// Subtract a second if nonzero & reset tenths
				if (gt & 0x0000FF00)
				{
					gt -= 0x00000100;

					// Clock at one minute?
					if (game_info.game_time == 0x00003b00)
						one_minute_speech();
					// Clock getting low speech
					if (game_info.game_time == 0x00001f00)
						clock_low_speech();
					// Less than 10 seconds left in quarter?
					if (game_info.game_time <= 0x00000b00)
					{
						// Play a warning snd every other clock second
						if (game_info.game_time & 0x00000100)
							snd_scall_bank(cursor_bnk_str,CLOCK_LOW_SND,VOLUME6,127,PRIORITY_LVL3); //3
					}

					// Determine here any alternate 10ths timing
					//  else use default
					gt |= cmos_ticks_per_second;
				}
				// Subtract a minute if nonzero & reset seconds & tenths
				else if (gt & 0x00FF0000)
				{
					// Organ?
					if (game_info.game_time == 0x00020000 || game_info.game_time == 0x00010000)
//						&& (game_info.game_quarter == 0 || game_info.game_quarter == 2 || game_info.game_quarter >= 3))
					{
						if (COURT_IS_INDOORS && randrng(100) < 50)
							snd_scall_bank(gameplay_bnk_str,randrng(3)+122,VOLUME4,127,PRIORITY_LVL4);
						else if (which_court == COURT_GILMORE)
							snd_scall_bank(crowd_bnk_str,randrng(5)+25,VOLUME7,127,PRIORITY_LVL5);
						else if (which_court == COURT_GEER)
						{
							if (randrng(100) < 15)
								snd_scall_bank(crowd_bnk_str,(randrng(100) < 50) ? 29 : 27,VOLUME7,127,PRIORITY_LVL5);
							else
								qcreate("snds",0,monkey_snds_proc,0,0,0,0);
								// Monkeys etc.
//								snd_scall_bank(crowd_bnk_str,randrng(7)+31,VOLUME7,127,PRIORITY_LVL5);
						}
					}

					drone_adjskill (gt >> 16);
					gt -= 0x00010000;
					gt |= 0x00003B00;
					gt |= cmos_ticks_per_second;
				}
				// Period is over!
				else
				{
					// Done eoq stuff yet?
					//if (!eoq_flg)
					//{
					//	// Play EOQ horn
					//	snd_scall_bank(plyr_bnk_str, HORN_SND, VOLUME4, 127, PRIORITY_LVL5);
					//
					//	eoq_flg = 1;
					//}
				}
#ifdef DEBUG
				if ((get_player_sw_current() & P3_D) && (get_player_sw_current() & P3_C) && (gt & 0x00ffffff))
				{
					gt = (gt & 0x00ffff00) | 1;
				}
#endif
// Build with -DQ_GAME to make it a quick game
#ifdef Q_GAME
				{
					extern int pup_qgame;
					if (pup_qgame && (gt & 0x00ffffff))
					{
						gt = (gt & 0x00ffff00) | 1;
					}
				}
#endif
			}
			// Show everyone else the current time
			game_info.game_time = gt;
		}
#endif //NO_CLOCK

		sleep(1);
	}
}

//-------------------------------------------------------------------------------------------------
//	This process plays beach snds
//
// 	INPUT:	Nothing
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void monkey_snds_proc(int *args)
{
	// Monkeys etc.
	snd_scall_bank(crowd_bnk_str,31,VOLUME7,127,PRIORITY_LVL3);
	snd_scall_bank(crowd_bnk_str,32,VOLUME7,127,PRIORITY_LVL2);
	snd_scall_bank(crowd_bnk_str,34,VOLUME7,127,PRIORITY_LVL1);

	sleep(randrng(2*60) + 120);

	snd_scall_bank(crowd_bnk_str,31,VOLUME7,127,PRIORITY_LVL3);
	snd_scall_bank(crowd_bnk_str,35,VOLUME7,127,PRIORITY_LVL2);
	snd_scall_bank(crowd_bnk_str,36,VOLUME7,127,PRIORITY_LVL1);
	snd_scall_bank(crowd_bnk_str,37,VOLUME7,127,PRIORITY_LVL1);

	sleep(randrng(2*60) + 180);

	snd_scall_bank(crowd_bnk_str,33,VOLUME7,127,PRIORITY_LVL2);
	snd_scall_bank(crowd_bnk_str,34,VOLUME7,127,PRIORITY_LVL2);
	snd_scall_bank(crowd_bnk_str,38,VOLUME7,127,PRIORITY_LVL1);
	
	sleep(randrng(6*60) + 3*60);

	snd_scall_bank(crowd_bnk_str,27,VOLUME7,127,PRIORITY_LVL2);
	sleep(randrng(2*60)+30);
	snd_scall_bank(crowd_bnk_str,29,VOLUME7,127,PRIORITY_LVL1);
	sleep(randrng(2*60)+50);
	snd_scall_bank(crowd_bnk_str,27,VOLUME7,127,PRIORITY_LVL2);

	snd_scall_bank(crowd_bnk_str,randrng(10)+31,VOLUME7,127,PRIORITY_LVL3);
	sleep(randrng(2*60)+1);
	snd_scall_bank(crowd_bnk_str,randrng(10)+31,VOLUME7,127,PRIORITY_LVL2);
	sleep(randrng(2*60)+1);
	snd_scall_bank(crowd_bnk_str,randrng(10)+31,VOLUME7,127,PRIORITY_LVL1);
}

//-------------------------------------------------------------------------------------------------
//	This process plays beach snds
//
// 	INPUT:	Nothing
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void monkey_snds2_proc(int *args)
{
	// Monkeys etc.
	sleep(60);
	snd_scall_bank(crowd_bnk_str,31,VOLUME7,127,PRIORITY_LVL3);
	snd_scall_bank(crowd_bnk_str,32,VOLUME7,127,PRIORITY_LVL2);
	snd_scall_bank(crowd_bnk_str,34,VOLUME7,127,PRIORITY_LVL2);

	sleep(randrng(2*60) + 120);

	snd_scall_bank(crowd_bnk_str,31,VOLUME7,127,PRIORITY_LVL3);
	snd_scall_bank(crowd_bnk_str,35,VOLUME7,127,PRIORITY_LVL2);
	snd_scall_bank(crowd_bnk_str,36,VOLUME7,127,PRIORITY_LVL2);
	snd_scall_bank(crowd_bnk_str,37,VOLUME7,127,PRIORITY_LVL2);

	sleep(randrng(2*60) + 120);

	snd_scall_bank(crowd_bnk_str,33,VOLUME7,127,PRIORITY_LVL2);
	snd_scall_bank(crowd_bnk_str,34,VOLUME7,127,PRIORITY_LVL2);
	snd_scall_bank(crowd_bnk_str,38,VOLUME7,127,PRIORITY_LVL3);
	
	sleep(randrng(10*60) + 3*60);

	snd_scall_bank(crowd_bnk_str,randrng(10)+31,VOLUME7,127,PRIORITY_LVL3);
	snd_scall_bank(crowd_bnk_str,randrng(10)+31,VOLUME7,127,PRIORITY_LVL2);
	snd_scall_bank(crowd_bnk_str,randrng(10)+31,VOLUME7,127,PRIORITY_LVL1);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
static void shot_clock_proc(int *args)
{
	reset_shotclock = 1;
	shotclock_active = 0;

//fprintf(stderr,"Top of shot clock proc - shotclock_active = %d\n", shotclock_active);
//fprintf(stderr,"Top of shot clock proc - clock_active = %d\n", clock_active);

	// Main loop
	while (1)	//shottime != 0)
	{
		// Get current time
		int st = game_info.shotclock;

		if (!reset_shotclock)
		{
#ifndef NO_SHOTCLOCK
			if (shotclock_active)
			{
				if (st & 0x00FF)				// 10ths != 0 ?
					st -= 0x0001;				// subtract a 10th
				else 
				{
					// Tick off another second
					if (st & 0xFF00)	 		// seconds != 0 ?
					{
						st -= 0x0100;			// subtract a second & reset "10ths"
						st |= cmos_ticks_per_second + 5;			// Make shot clock a bit slower than game clock

						// Say, somebody better shoot it!		 
						if (((st & 0xFF00) == 0x0300 ) && (randrng(100) < 30))
							shotclock_low_speech();
					}
					else
					{
						// Shotclock expired
						reset_shotclock = 1;
						shotclock_active = 0;	// turn it off here - let someone else restart it!

						// If clock is less than 1
						// Shot Clock is not displayed if less than 11 seconds existed when it was time
						// to turn it on - so, if less than 1 now, the clock isn't even displayed
						if (!((game_info.game_time & 0x00FFFF00) < 0x00000200))
							qcreate("expired",PLAYER_PID,shot_clock_expired_proc,0,0,0,0);
					}
				}
			}
#endif // NO_SHOTCLOCK
		}
		else
		{
			st = SHOTCLOCK_TIME;
			reset_shotclock = 0;
		}

#ifdef DEBUG
		if ((get_player_sw_current() & P3_D) && (get_player_sw_current() & P3_C))
			reset_shotclock = 1;
#endif

		// Show everyone else the current time
		game_info.shotclock = st;

		sleep(1);
	}
}

//-------------------------------------------------------------------------------------------------
// Shot clock expired.  Put up a message, knock ball loose, go into scramble mode
//-------------------------------------------------------------------------------------------------
extern int RimBounceFlavor;
static void shot_clock_expired_proc(int *args)
{
//	obj_3d	*pobj = (obj_3d *)ppdata;
	int st;

	// If ball is in air, and it is not going to be an airball, DIE
	if(game_info.ball_mode == BM_SHOOTING && RimBounceFlavor != SHTMD_AIR)
	{
		reset_shotclock = 1;
		shotclock_active = 1;	// Ball probably hit rim - but if same team recovers it, the clock should still be running
		die(0);
	}

	// Otherwise, if it is an airball, wait until the ball clears the hoop
	// and the players realize it was an airball, then continue
	if(game_info.ball_mode == BM_SHOOTING && RimBounceFlavor == SHTMD_AIR)
	{
		 //Wait until ball is lower than rim
		while(ball_obj.flight_time < ball_obj.total_time+0)
			sleep (1);
		sleep(5);
	}

	shotclock_expired_speech();
	clock_active = 0;				// stop game clock

	if(!(existp(VIOLATION_PID, -1)))
		// Put up text message
		qcreate("shotclock", VIOLATION_PID, shotclock_violation_proc, 0, 0, 0, 0);

	player_blocks[game_info.ball_handler].no_ball_collide = 65;
	player_blocks[game_info.ball_handler].dribble_mode = DM_CAN_DRIBBLE;


	ball_obj.odata.vx = 0;
	ball_obj.odata.vy = 1.2f;
	ball_obj.odata.vz = 0;
	ball_obj.flight_time = 0;
	ball_obj.total_time = 0;					//time;

	flash_ball(5);								// flash the ball

	game_info.passer    = NO_PASSER;
	game_info.ball_mode = BM_FREE_ON_COURT;
	//ball_obj.who_shot   = NO_SHOOTER;

	game_info.ball_handler = NO_BALLHANDLER;

	//sleep (5 *tsec);
	SetUpOOBounds(2,0,0);

}

static void do_court_fog(void)
{
	static GrFog_t fog_table[64];
	int fog_color;
	float far_z;
	
	if (COURT_IS_OUTDOORS && (pup_fog || pup_thickfog || pup_sfog || pup_nfog)) {
		arena_fog_adjustment(TRUE);
		if (pup_sfog || pup_nfog || pup_thickfog) {
			fog_color = 0X00A0A0A0;
			if (which_court == COURT_GILMORE) {
				if (pup_sfog)
					fog_color = 0x0096D764;
				else if (pup_nfog)
					fog_color = 0x00202020;
				far_z = 500.0f;
			} else {
				if (pup_sfog)
					fog_color = 0x0096D764;
				else if (pup_nfog)
					fog_color = 0x00000000;
				far_z = 800.0f;
			}
		} else {
			fog_color = 0x00808080;
			if (which_court == COURT_GILMORE)
				far_z = 950.0f;
			else
				far_z = 800.0f;
		}

		// Thunder
		snd_scall_bank(crowd_bnk_str,23,VOLUME7,127,PRIORITY_LVL5);


		guFogGenerateLinear(fog_table, 75.0f, far_z);
		grFogColorValue(fog_color);
		grFogTable(fog_table);
		
		create_object("FOGON", OID_FIELD, OF_NONE, DRAWORDER_FOGON, NULL, draw_fogon);
		create_object("FOGOFF", OID_FIELD, OF_NONE, DRAWORDER_FOGOFF, NULL, draw_fogoff);
	} else
		arena_fog_adjustment(FALSE);
}	/* do_court_fog */

static void draw_fogon(void *dummy)
{
	grFogMode(GR_FOG_WITH_TABLE_ON_FOGCOORD_EXT);
}	/* draw_fogon */

static void draw_fogoff(void *dummy)
{
	grFogMode(GR_FOG_DISABLE);
}	/* draw_fogoff */

/*

Old Hangtime shot clock & regular clock loop


#****************************************************************
* Start shot clock timer, when clock gets down to 10 seconds,
* then display the clock in the lower corner furthest away from
* the hoop.
* A0=0 shooting at rgt hoop, 1=left hoop
* Trashes scratch

 SUBR	shot_clock

	.if 	HEADCK
	rets
	.endif

	.if DEBUG | DUNKTST
	move	@CLOCK_OFF,a14
	jrz	#ok
	rets
#ok
	.endif

	PUSH	a7,a8

	move	a0,a8

	movi	clockid,a0
	calla	KIL1C 			;Kill existing shot clock proc
	movi	CLSDEAD|clockid,a0
	calla	obj_del1c		;Delete any 24 second shot clock imgs

	CREATE	clockid,clock24
	move	a0,@sc_proc,L

	movi	[1,0],a0
	move	a0,@shotimer,L

	PULL	a7,a8
     	rets


 SUBRP	clock24 
	clr	a10
	movk	7,a9			;13

	movi	game_time,a1
;check to see if less than 24 seconds remain on clock.  If so, die!
	movb	*a1(8),a0		;restuff single seconds byte
	movb	*a1(16),a2		;grab 10 seconds byte


	movb	*a1(24),a3		;grab minutes byte
	jrnz	#lp
;Possibly less than a minute left

	move	a2,a2
	jrnz	#not

#nosc
	clr	a0
	move	a0,@sc_proc,L
	DIE
#not
	cmpi	1,a2
	jrnz	#lp
	cmpi	5,a0
	jrle	#nosc

#lp
	SLEEP	40
	move	@HALT,a0
	jrnz	#lp

;	jruc	#lp

	dsj	a9,#lp

#lp0	SLEEPK	10

	move	@tvpanelon,a0,L
	jrnz	#lp0
	move	@ballgoaltcnt,a0
	jrgt	#lp0
	move	@ballpnum,a0
	jrn	#skp
	sll	5,a0
	addi	plyrproc_t,a0
	move	*a0,a0,L
	move	*a0(plyr_jmpcnt),a14
	jrnz	#lp0
	move	*a0(plyr_seqflgs),a0
	btst	DUNK_B,a0
	jrnz	#lp0			;!Dunking?
#skp
	SLEEPK	10

;wait 13 seconds before we actually display shot clock in corner
;left side hoop

	movi	#init_t,a11
	movi	shotram,a10		;for holding onto img pointer
	move	a8,a8
	jrnz	hoopl
	movi	#init_t2,a11
hoopl
	jruc	#status

	movi	#init_t,a11
	jruc	#status
#lp2
	move	*a11+,a1		;Y
	sll	16,a0
	sll	16,a1
	move	*a11+,a3		;Z
	movi	DMAWNZ|M_NOCOLL|M_SCRNREL,a4
	move	*a11+,a2,L
	movi	CLSDEAD|clockid,a5
	clr	a6
	clr	a7
	calla	BEGINOBJ2
	move	a0,*a10+,L		;hold onto img pointer

#status	move	*a11+,a0		;X
	cmpi	4000,a0
	jrne	#lp2			;!End?

#lp3
	move	@tvpanelon,a0,L
	jrnz	#x

	move	@shotram+32,a8,L		;first digit
	move	@shotram+64,a9,L		;second digit
	move	@shotimer,a2,L
	clr	a0
	movy	a2,a0
	srl	11,a0
	addi	bnumbs,a0
	move	*a0,a0,L
	move	*a8(OCTRL),a1
	calla	obj_aniq

;Do 2nd digit
	sll	16,a2
	srl	11,a2
	addi	bnumbs,a2
	move	*a2,a0,L
	move	a9,a8
	move	*a8(OCTRL),a1
	calla	obj_aniq

#lp3t	SLEEP	32
	move	@HALT,a0
	jrnz	#lp3t

	move	@shotimer,a0
	jrz	#tag
	dec	a0
	move	a0,@shotimer

	cmpi	4,a0
	jrgt	#nowarn
	jrne	#nospch

	move	@ballpnum,a14
	jrn	#nospch
	SOUND1	sht_ball_sp
#nospch
	SOUND1	warn_snd	      ;Warn players shot clock is running down
#nowarn

	jruc	#lp3
#tag	move	@shotimer+16,a0
	jrz	#tag1
	dec	a0
	move	a0,@shotimer+16
	movk	9,a0
	move	a0,@shotimer
	jruc	#lp3
#tag1
;	SOUND1	error_found

#tag1X
	SLEEPK	1

;Shot clock ran out - turnover!  Unless the ball is in the air toward hoop!
	move	@ballgoaltcnt,a0
	jrgt	markdi

	movk	1,a0
	move	a0,@clock_active

	CREATE0	shot_text
	CREATE0	shot_clock_speech

	move	@ballpnum,a11
	jrnn	#ok1
	move	@ballpnumshot,a11
#ok1

	srl	1,a11
	subk	1,a11
	CREATE0	plyr_takeoutball3


#showstick
	movi	P1DATA,a10
	callr	stick_number
	movi	P2DATA,a10
	callr	stick_number
	movi	P3DATA,a10
	callr	stick_number
	movi	P4DATA,a10
	callr	stick_number

 
	SLEEP	TSEC*3

	clr	a0
	move	a0,@clock_active

sclockx	move	a10,a10
	jrz	#x2
#x	movi	CLSDEAD|clockid,a0
	calla	obj_del1c		;Delete any 24 second shot clock imgs
#x2	clr	a0
	move	a0,@sc_proc,L
#die	DIE

markdi	SLEEP	120
	movi	clockid,a0
	calla	KIL1C 			;Kill existing shot clock proc
	movi	CLSDEAD|clockid,a0
	calla	obj_del1c		;Delete any 24 second shot clock imgs
      	jruc	#x2

numbs
	.ref	BRSH12_0,BRSH12_1,BRSH12_2,BRSH12_3,BRSH12_4,BRSH12_5
	.ref	BRSH12_6,BRSH12_7,BRSH12_8,BRSH12_9
	.long	BRSH12_0,BRSH12_1,BRSH12_2,BRSH12_3,BRSH12_4,BRSH12_5
	.long	BRSH12_6,BRSH12_7,BRSH12_8,BRSH12_9
;	.long	font90,font91,font92,font93,font94,font95,font96,font97
;	.long	font98,font99

bnumbs
	.long	BRSH20_0,BRSH20_1,BRSH20_2,BRSH20_3,BRSH20_4,BRSH20_5
	.long	BRSH20_6,BRSH20_7,BRSH20_8,BRSH20_9


#init_t
	.word	>13d,>e0,5000
	.long	NSHOTCLK
	.word	>143,>d9,5000
	.long	BRSH20_0		;font90r
	.word	>158,>d9,5000
	.long	BRSH20_0		;font90r
	.word	4000

#init_t2
	.word	>1c,>e0,5000
	.long	NSHOTCLK
	.word	>22,>d9,5000
	.long	BRSH20_0		;font90r
	.word	>37,>d9,5000
	.long	BRSH20_0		;font90r
	.word	4000
		


****************************************************************
* Start main game clock


timer_table
	.asg	5100h,BASETM

	.word	BASETM*70/100		;1 (slowest)
	.word	BASETM*85/100		;2 (slower)
	.word	BASETM			;3 (default)	;Approx 10 minutes
	.word	BASETM*115/100		;4 (faster)
	.word	BASETM*130/100		;5 (fastest)


 SUBR	clock_strt

	movi	ADJSPEED,a0		;Get game clk speed (1-5)
	calla	GET_ADJ
	move	a0,@CMOS_clock_speed

	clr	a0
	move	a0,@tvpanelon,L
	move	a0,@ballflash

	callr	prt_credits
	callr	prt_qrtr

	movi	clock_imgs,a10
	movi	#init_t,a11		;>Setup status images
	jruc	#status
#lp
	move	*a11+,a1		;Y
	move	*a11+,a3		;Z
	move	*a11+,a2,L		;*IMG
	sll	16,a0
	sll	16,a1
	movi	DMAWNZ|M_NOCOLL|M_SCRNREL,a4	;M_3DQ
	movi	CLSDEAD|gclockid,a5
	clr	a6
	clr	a7
	calla	BEGINOBJ2
	move	a0,*a10+,L
;	movi	gndpos_t+32*16,a1
;	move	a1,*a8(ODATA_p),L

#status	move	*a11+,a0		;X
	cmpi	4000,a0
	jrne	#lp			;!End?

	movk	1,a0			;Make clk inactive
	move	a0,@clock_active

;----------
; Top of clock loop

#lp1	calla	prt_top_clock		;Update clk
#lp1a	SLEEPK	1
	move	@HALT,a0
	jrnz	#lp1a

	.if DEBUG | DUNKTST
	move	@CLOCK_OFF,a0	;When testing dunks - PUT IN!
	jrnz	#lp1a
	.endif

	.if	IMGVIEW
	jruc	#lp1a			;Debug - no time clk on
	.endif

	move	@clock_active,a0	;Clk stopped (inbounding)? Yes if !0
	jrnz	#lp1a

;We shouldn't do this GET_ADJ during game loop!
;Get at start of game 1 time only!
;	movi	ADJSPEED,a0		;Get game clk speed (1-5)
;	calla	GET_ADJ

	move	@CMOS_clock_speed,a0	;RAM copy of CMOS adjustment

	sll	4,a0
	addi	timer_table-16,a0
	move	*a0,a1
	move	@PSTATUS,a0
	cmpi	>f,a0			;Is it a 4 plyr game? No if !0
	jrnz	#not4
	addi	200h,a1			;100h;Yes. Make clk run fastr!
#not4
;	movi	>ffff,a1		;Debug - make clk super fast!

	move	@clock_speed,a0		;Add to clk speed frac
	add	a1,a0
	move	a0,@clock_speed
	jrnc	#lp1a			;Time for a clk tick? No if !C

	movi	game_time,a10

	movb	*a10,a0			;Get 10ths byte
	jrz	#tag			;Full sec? Yes if 0
	dec	a0			;No. Dec 10ths
	movb	a0,*a10
	move	*a10,a0,L		;Chk if qrtr over. Yes if 0
	jrz	#qtrend
	srl	32-8,a0			;Under a min left?
	jrnz	#lp1a			;No if !0 - no clk update
	jruc	#lp1			;Update clk
#tag	
	movk	9,a0			;Reset 10ths
	movb	a0,*a10

	movb	*a10(1*8),a0		;Get secs byte
	jrz	#tag1			;Full ten? Yes if 0
	dec	a0			;No. Dec secs
	movb	a0,*a10(1*8)
	jruc	#lp1			;Update clk
#tag1
	movk	9,a0			;Reset secs
	movb	a0,*a10(1*8)

	movb	*a10(2*8),a0		;Get tens byte
	jrz	#tag2			;Full min? Yes if 0
	dec	a0			;No. Dec tens
	movb	a0,*a10(2*8)
	jruc	#lp1			;Update clk
#tag2	
	movk	5,a0			;Reset tens
	movb	a0,*a10(2*8)

	movb	*a10(3*8),a0		;Get mins byte
	cmpi	3,a0			;Time to adjust drone skills?
	jrhs	#nodaj			;No if >=

;MJT Start

	cmpi	1,a0
	jrnz	#reg
	LOCKUP
;Bong clock on outside court at 1 minute
;	.ref	pup_court
;	move	@pup_court,a14
;	jrz	#reg
 	.ref	chrch_bel_sp
	SOUND1	chrch_bel_sp
#reg
	movb	*a10(3*8),a0		;Get mins byte
;MJT End


	.ref	drone_adjskill
	calla	drone_adjskill		;Yes. Pass A0
#nodaj
	movb	*a10(3*8),a0		;Get mins byte & dec it
	dec	a0
	movb	a0,*a10(24)
	jrnz	#lp1			;Update clk if more than a min left

;Rearrange top clock to allow tenths

	move	@clock_imgs,a0,L
	move	@clock_imgs+32,a1,L
	move	*a0(OXPOS),a14
	move	a14,*a1(OXPOS)
	move	*a1(OSIZEX),a1
	add	a1,a14
	move	a14,*a0(OXPOS)
	calla	prt_top_clock		;Update clk



	CREATE0	tick_tock		;Qrtr almost over sounds
	CREATE0	one_min_to_go

;Possibly rearrange bottom clock

	move	@bclok_imgs,a0,L	;Screen bottom clk on?
	calla	ISOBJ
	jrnz	#bclkon
	CREATE	tvid,scr_clock		;No. Turn on bottom clk

	SLEEPK	2

	move	@bclok_imgs,a0,L
#bclkon
	move	*a0(OIMG),a1,L		;Is bottom clk min:sec or sec:10th?
	cmpi	clock,a1
	jrnz	#lp1			;Assume sec:10th if !0

;Rearrange bottom clock to allow tenths

	move	a0,a8			;Make bottom clk sec:10th
	movi	clock2,a0
	move	*a8(OCTRL),a1
	calla	obj_aniq

	move	@bclok_imgs+32,a1,L
	movi	>136-15,a0
	move	a0,*a1(OXXPOS)
	move	@bclok_imgs+64,a1,L
	movi	>116-10,a0
	move	a0,*a1(OXPOS)
	move	@bclok_imgs+96,a1,L
	movi	>120-10,a0
	move	a0,*a1(OXPOS)
	jruc	#lp1			;Update clk

;----------
; Quarter ended.
;
; Rearrange clock imgs back to min:sec

#qtrend
	clr	a0
	calla	drone_adjskill
	move	@clock_imgs,a0,L
	move	@clock_imgs+32,a1,L
	move	*a1(OXPOS),a14
	move	a14,*a0(OXPOS)
	move	*a0(OSIZEX),a0
	add	a0,a14
	move	a14,*a1(OXPOS)

	movk	1,a0			;Make clk inactive
	move	a0,@clock_active
	clr	a0
	move	a0,@clock_speed		;Reset clk speed frac
	move	a0,@game_time,L		;Make clk 0:00.0
	calla	prt_top_clock		;Update clk

	move	@tvpanelon,a0,L		;Is tv panel on? Yes if !0
	jrnz	#tag3
	movi	tvid,a0			;No. Delete bottom clk that is on
	calla	KIL1C			; screen by itself
	movi	CLSDEAD|bclockid,a0
	calla	obj_del1c
#tag3
	SOUND1	horn_snd

	move	@_4plyrsingame,a0
	jrnz	#skip
	move	@PSTATUS,a0
	cmpi	>f,a0			;Is it a 4 plyr game? Yes if 0
	jrz	#skip
	movk	1,a0
	move	a0,@_4plyrsingame
#skip
	move	@_2plyr_competitive,a0
	jrnz	#skip1			;br=not 2 plr competitive
	move	@PSTATUS,a0
	cmpi	1111b,a0
	jreq	#skip1			;br=2 plyr competitive
	cmpi	1010b,a0
	jreq	#skip1			;br=2 plyr competitive
	cmpi	1001b,a0
	jreq	#skip1			;br=2 plyr competitive
	cmpi	0110b,a0
	jreq	#skip1			;br=2 plyr competitive
	cmpi	0101b,a0
	jreq	#skip1			;br=2 plyr competitive
	movk	1,a0
	move	a0,@_2plyr_competitive
#skip1
	JSRP	plyr_endofqrtr

 SUBR	cliplockretp			;Restore point for clip lockup code

	movk	1,a0			;Make clk inactive
	move	a0,@clock_active

	movi	P1DATA,a11		;Refill turbo meters between qrtrs
	callr	refill_turbo
	movi	P2DATA,a11
	callr	refill_turbo
	movi	P3DATA,a11
	callr	refill_turbo
	movi	P4DATA,a11
	callr	refill_turbo

	callr	prt_qrtr		;Update score plate

	movi	QRTRTIME,a0		;>Regular qrtr time
	move	@gmqrtr,a1
	subk	4,a1			;In an OT qrtr? No if <
	jrlt	#regqtr

; Setup for OT quarter

;	SOUND1	intoot_snd

	move	@tvpanelon,a0,L		;Is tv panel on? No if 0
	jrz	#skp
	calla	process_exist		;Yes. Its process better exist!
	jrz	#tvpwt
	movi	tv3a,a7			;Turn off tv panel
	calla	XFERPROC
#tvpwt
	SLEEPK	1
	move	@tvpanelon,a0,L		;Wait for tv panel to turn off
	jrnz	#tvpwt
#skp	
;;;;	movk	1,a0		;>OT qrtr time
	movi	QRTRTIME,a0		;>OT qrtr time

; Common setup for regular & OT quarter

#regqtr
	move	a0,@game_time,L

;Make sure alley oop cnt
	

*/
