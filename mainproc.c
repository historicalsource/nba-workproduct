/*
 *		$Archive: /video/Nba/mainproc.c $
 *		$Revision: 169 $
 *		$Date: 10/18/99 4:55p $
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
char *ss_mainproc_c = "$Workfile: mainproc.c $ $Revision: 169 $";
#endif


/*
 *		SYSTEM INCLUDES
 */

#include <stdio.h>
#include <stdlib.h>
#include <goose/goose.h>
#include <string.h>

/*
 *		USER INCLUDES
 */

#include "debug.h"
#include "creatp.h"
#include "game.h"
#include "coin.h"
#include "ani3d.h"
#include "select.h"
#include "externs.h"
#include "nbacmos.h"
#include "audits.h"
#include "sndcalls.h"
#include "id.h"
#include "fontid.h"
#include "anim/gen_seq.h"
#include "mainproc.h"
#include "audits.h"
#include "include/profile.h"
#include "include/gameadj.h"
//#include "player.h"
#include "movie.h"
#include "camdefs.h"
#include "game_sel.h"

#include "/video/diag/dbcmos.c"

#define STACK_DEPTH		16
#define STACK_COUNT		8

int set_coin_hold(int flag);
void load_game_info(void);
void do_ingame_restart( int *parg );
int get_full_game_credits(void);
void game_proc_debug( int *parg );
int get_heap_available(void);
void dump_last_pass(int, int);
void srmain_test_sound (void);
void show_pic_info(float *y_pos);
void show_sound_version(float *y_pos);
void check_files(void);
void attract_loop(void);
void get_resolution(float *, float *);
void plyr_strt_butn(int sig, int swid);
static void dump_tlist(int sig, int swid);
void snd_board_error_recovery(void);
void load_permanent_snds(void);
static void inc_pcount(void);
void uptime_proc(int *);
void start_diags(int *);
void do_test(int, int);
void main_proc(int *);
int paid_credits_add(int, int);
void paid_credits_deduct(void);
int do_start_press(int, int);
void do_file_check(void);
void start_enable(int);
void no_start(int, int);
int get_start_state(void);
void backdoor_proc(int *);
#if defined(SEATTLE)
static void dump_audits(int, int);
void dump_stack_trace(void);
#endif

#if defined(VEGAS)
unsigned long long get_timer_val(void);
#endif

extern int reload;
extern int p_active;

extern unsigned int tick_sndn;
extern unsigned int coin_snd_drop_calln;
extern unsigned int coin_snd_paid_calln;

extern char index_2p[NUM_PLAYERS];
extern char shift_2p[NUM_PLAYERS];

extern VIEWCOORD cambot;
extern void game_over( int );
extern void load_player_speech_banks(void);
extern void load_team_speech_banks(void);
extern int texture_load_in_progress;
extern font_info_t	bast7t_font;
extern font_info_t	bast7w_font;
extern font_info_t	bast8t_font;
extern font_info_t	bast8to_font;
extern font_info_t	bast8ta_font;

extern font_info_t	nba10_font;
extern font_info_t	nba12_font;
extern font_info_t	nba14_font;
extern font_info_t	nba20_font;
extern font_info_t	nba23_font;
extern font_info_t	nba25_font;
extern font_info_t	ctn14_font;
extern font_info_t	ctn20_font;
extern font_info_t	ctn25_font;

extern int vs_page_showing;
extern int	credit_pg_on;
#if defined(SEATTLE)
extern int tick_counter;
#endif
extern texture_node_t *tlist;
extern int is_dip_coinage;
extern void credit_page_proc(int *args);
extern void grab_install_proc(void);

#if defined(VEGAS)
unsigned long long	get_elapsed_time(void);
#endif

#if DEBUG
char *game_version = "SHOWTIME DEBUG 3.0  ";
#else
//char *game_version = "SHOWTIME   2.1 TEST ";
char *game_version = "NBA SHOWTIME 2.1    ";
//char *game_version = "SHOWTIME REV 2.0E   ";
#endif

void write_bios_flags(unsigned int val);
unsigned int read_bios_flags(void);
#define POWER_UP_SIGNATURE		0xF060765F

int	bought_in_first;					/* represents plyr on teams wh obought in first */
int	p_status;							/* represents current plyrs in game with bit pos. (0-3) */
int	p_status2;							/* represents previous plyrs in game with bit pos. (0-3) */
int	winning_team;						/* which set of players won last game (0=1&2, 1=3&4) */
int	game_purchased;           			/* set bit (0-4) for plyr which bought full game */
int sysfont_height;
int	checksum;							/* perm. snd bank checksum */
int	snd_ver_num;
int start_state = FALSE;
int	do_playtime = 0;

char plyr_bnk_str[] = "balletc";
char cursor_bnk_str[] = "cursors1";
char players_bnk_str[] = "players";
char gameplay_bnk_str[] = "balletc";
char start_movie_snd_bnk_str[] = "mov_sta2";
char qtr_trans_bnk_str[] = "tuntran5";
//char screen_trans_bnk_str[] = "screens1";
char buyin_bnk_str[] = "startune";
char anc_take_bnk[] = "sca_take";
char anc_misc_bnk[] = "sca_misc";
char eoq_tune_bnk_str[] = "tunhalft";

//char crowd_bnk_str[] = "nbccrowd";
char crowd_bnk_str[9];
char anc_make_bnk[9];
char ancr_swaped_bnk_str[9];	/* swap between TKBANK1.BNK and TKBANK2.BNK */
char team1_bnk_str[9];
char team2_bnk_str[9];
char taunt_bnk_str[9];			/*  swap between TAUNT1.BNK and TAUNT2.BNK */
char game_tune_bnk_str[9];

/* create the INFO structures for the game & last game played */
game_info_t game_info;
game_info_t game_info_last;

//tdata_t tdata_blocks[MAX_PLYRS];					/* bookkeeping structure...for stats during gameplay per player */
//tdata_t tdata_blocks_last[MAX_PLYRS];				/* bookkeeping structure...for stats during gameplay per player */


struct process_node *clicker_proc;
struct process_node *volume_proc;


pdata_t pdata_blocks[MAX_PLYRS];			/* defined in "game.h" */


int check_all_audits(void);

#if defined(SEATTLE)
extern int	uptime;
#endif

typedef struct	font_load_info {
	font_info_t	*font_info;
	int	id;
} font_load_info_t;

/* Every wipeout calls main_load_fonts */
static font_load_info_t	mid_res_fonts[] = {
{&bast7t_font,  FONTID_BAST7T},
{&bast8t_font,  FONTID_BAST8T},
{&bast8to_font,  FONTID_BAST8TO},
{&bast8ta_font,  FONTID_BAST8TA},
{&nba10_font,  FONTID_NBA10},
{&nba12_font,  FONTID_NBA12},
{&nba14_font,  FONTID_NBA14},
{&nba20_font,  FONTID_NBA20},
{&nba23_font,  FONTID_NBA23},
{&nba25_font,  FONTID_NBA25},
#ifdef USE_CHATTEN_FONT
{&ctn14_font,  FONTID_CHATTEN14},
{&ctn20_font,  FONTID_CHATTEN20},
{&ctn25_font,  FONTID_CHATTEN25},
#endif
{NULL, 0}
};

static font_load_info_t	low_res_fonts[] = {
{&bast7w_font,  FONTID_BAST7T},
{&bast8t_font,  FONTID_BAST8T},
{&bast8to_font,  FONTID_BAST8TO},
{&bast8ta_font,  FONTID_BAST8TA},
{&nba10_font,  FONTID_NBA10},
{&nba12_font,  FONTID_NBA12},
{&nba14_font,  FONTID_NBA14},
{&nba20_font,  FONTID_NBA20},
{&nba20_font,  FONTID_NBA23},
{&nba23_font,  FONTID_NBA25},
#ifdef USE_CHATTEN_FONT
{&ctn14_font,  FONTID_CHATTEN14},
{&ctn14_font,  FONTID_CHATTEN20},
{&ctn20_font,  FONTID_CHATTEN25},
#endif
{NULL, 0}
};

extern font_node_t *font_list;

#ifdef DBG_HEAP
/*****************************************************************************/
int	_write(int, void *, int);
void * heap_walk(void * packet, unsigned int * owner, unsigned int * size,
								unsigned int * used, unsigned int * next);
int textify(char * sz, unsigned int val, int base10, int zeros);
unsigned int get_heap_count(void);

void do_heap_dump_handler(int sw_state, int sw_id)
{
	if (sw_state == SWITCH_CLOSE)
	{
		char * psz;
		void * packet1 = NULL;
		void * packet2 = NULL;
		void * packet3 = NULL;
		void * packet_next;
		char cbuf[200];
		unsigned int owner;
		unsigned int size;
		unsigned int used;
		unsigned int next;
		unsigned int count;
		unsigned int column1;
		unsigned int column2 = 0;
		unsigned int column3 = 0;
		int len;

		count = get_heap_count();
		if (count)
		{
			packet1 = heap_walk(NULL, NULL, NULL, NULL, NULL);
			column1 = count / 3;
			if (column1)
			{
				psz = " ADDRESS    OWNER    SIZE USE     ADDRESS    OWNER    SIZE USE     ADDRESS    OWNER    SIZE USE\r\n";

				column3 = column1;
				column2 = column1 + !!((count % 3) > 1);
				column1 = column1 + !!((count % 3) > 0);

				packet2 = packet1;
				len = column1;
				while (len--)
					packet2 = heap_walk(packet2, NULL, NULL, NULL, NULL);

				packet3 = packet2;
				len = column2;
				while (len--)
					packet3 = heap_walk(packet3, NULL, NULL, NULL, NULL);
			}
			else
			{
				psz = " ADDRESS    OWNER    SIZE USE\r\n";
				column1 = count;
			}

			_write(1, "\r\n", 2);
			_write(1, "System Heap:\r\n", 14);
			_write(1, psz, strlen(psz));

			psz = "$$$$$$$$ $$$$$$$$ $$$$$$$ YES";

			while (column1)
			{
				strcpy(cbuf, psz);

				packet_next = heap_walk(packet1, &owner, &size, &used, &next);

				len =
				textify(cbuf, (unsigned int)size,    1, 0);
				textify(cbuf, (unsigned int)owner,   0, 1);
				textify(cbuf, (unsigned int)packet1, 0, 1);

				if (!used)
				{
					cbuf[len-3] = ' ';
					cbuf[len-2] = '-';
					cbuf[len-1] = ' ';
				}

				packet1 = packet_next;
				column1--;

				if (column2)
				{
					strcat(cbuf, "    ");
					strcat(cbuf, psz);

					packet_next = heap_walk(packet2, &owner, &size, &used, &next);

					len =
					textify(cbuf, (unsigned int)size,    1, 0);
					textify(cbuf, (unsigned int)owner,   0, 1);
					textify(cbuf, (unsigned int)packet2, 0, 1);

					if (!used)
					{
						cbuf[len-3] = ' ';
						cbuf[len-2] = '-';
						cbuf[len-1] = ' ';
					}

					packet2 = packet_next;
					column2--;
				}

				if (column3)
				{
					strcat(cbuf, "    ");
					strcat(cbuf, psz);

					packet_next = heap_walk(packet3, &owner, &size, &used, &next);

					len =
					textify(cbuf, (unsigned int)size,    1, 0);
					textify(cbuf, (unsigned int)owner,   0, 1);
					textify(cbuf, (unsigned int)packet3, 0, 1);

					if (!used)
					{
						cbuf[len-3] = ' ';
						cbuf[len-2] = '-';
						cbuf[len-1] = ' ';
					}

					packet3 = packet_next;
					column3--;
				}

				strcat(cbuf, "\r\n");

				_write(1, cbuf, strlen(cbuf));
			}

			strcpy(cbuf, "Total # of Packets: $$$$$\r\n");
			textify(cbuf, (unsigned int)count, 1, 0);

			_write(1, cbuf, strlen(cbuf));
			_write(1, "\r\n", 2);
		}
		else
		{
			_write(1, "\r\n", 2);
			_write(1, "Corrupted System Heap!\r\n", 24);
		}
	}
}
#endif

/*****************************************************************************/
void uptime_proc(int *args)
{
#if defined(SEATTLE)
	int	play_time, sleep_time;

	sleep_time = 5 * get_tsec();
	if (get_audit(UPTIME_AUDIT_NUM, &uptime) || (uptime < 0))
		uptime = 0;
	for (;;) {
		sleep(sleep_time);

		// uptime gets incremented every second in the inthandler
		set_audit(UPTIME_AUDIT_NUM, uptime);
		if (game_info.game_state != GS_ATTRACT_MODE)
		{
			if (get_audit(PLAYTIME_AUD, &play_time) || (play_time < 0))
				play_time = 0;
			else
				play_time += 5;
			set_audit(PLAYTIME_AUD, play_time);
		}
		recalc_percent_aud(PLAYTIME_AUD, UPTIME_AUDIT_NUM, PLAYTIME_PERCENT_AUD);
	}
#elif defined(VEGAS)
	int				play_time;
	int				uptime;
	int				start_uptime;
	unsigned int	start_time = (unsigned int)get_elapsed_time();
	
	if(get_audit(UPTIME_AUDIT_NUM, &start_uptime) || (start_uptime < 0))
	{
		start_uptime = 0;
	}
	for (;;) {
		sleep(get_tsec() * 5);
		
		uptime = (unsigned int)get_elapsed_time() - start_time;
		uptime += start_uptime;

		/* uptime gets incremented every second in the inthandler */
		set_audit(UPTIME_AUDIT_NUM, uptime);
		if (game_info.game_state != GS_ATTRACT_MODE) {
			if (get_audit(PLAYTIME_AUD, &play_time) || (play_time < 0))
				play_time = 0;
			else
				play_time += 5;
			set_audit(PLAYTIME_AUD, play_time);
		}
		recalc_percent_aud(PLAYTIME_AUD, UPTIME_AUDIT_NUM, PLAYTIME_PERCENT_AUD);
	}
#endif
}	/* uptime_proc */

void start_diags(int *args)
{
	/* shut down the movie player if it is running */
	if (movie_get_state() > 1)
		movie_abort();

	/* do not start diags while initializing the game */
	while (game_info.game_mode == GM_INITIALIZING)
		sleep(1);

	/* remove the disk callback handler */
	dio_mode(DIO_MODE_BLOCK);

	/* write out the CMOS before we shut down */
	write_cmos_to_disk(FALSE);
	update_other_cmos_to_disk(FALSE);
	
	exec("diag.exe", 0xcafebeef, args);
}	/* start_diags */

void do_test(int sig, int id)
{
	if (sig == SWITCH_CLOSE)
		qcreate("diags", 0, start_diags, 0, 0, 0, 0);
}	/* do_test */

void main_proc(int *args)
{
	float	y_pos;
	int		adj_status;
	int		val;
	int 	flag;
	int		failed;
	union {
		dip_inputs_t	di;
		unsigned int	val;
	} dip_in;

	dip_in.val = (read_dip_direct() ^ 0xFFFFFFFF) << DIP_SHIFT;
	
	/* Set the background color */
	set_bgnd_color(0xFF000000);
	
	/* enable tex and sound CRCs */
#if 0
#ifndef NO_CRCS
	guTexUseCRC();
	snd_crc_check_enable();
#endif
#endif

	/* load the fonts */
	main_load_fonts();
	
	/* start bog-dependent tick counter */
	pcount = 0;
	pre_proc_func(inc_pcount);

	// put somethin' up if we are reloading
	if (reload)
	{
		draw_enable(0);
		display_loading(NBA_RUNNING);
		draw_enable(1);
		black_screen();
		sleep(2);
		fade(1.0, 30, NULL);
		sleep(30);
		draw_enable(0);	
	}

	/* Get the system font height */
	sysfont_height = get_font_height(FONTID_NBA10) + (is_low_res ? 5 : 3);

	/* Select the font and preset a couple of text items */
	set_text_font(FONTID_NBA10);
	set_text_color(WHITE);
	set_text_z_inc(-0.10F);
	set_string_id(3);

	/* Display the game name and version */
	y_pos = SPRITE_VRES * (is_low_res ? 0.9F : 0.85F);
	set_text_position(SPRITE_HRES / 2.0F, y_pos, 200.0F);

	if (!reload)
	{
		set_string_id(2);
		set_text_justification_mode(HJUST_CENTER | VJUST_CENTER);

		set_text_color(LT_YELLOW);
		y_pos -= sysfont_height;
		set_text_position(SPRITE_HRES / 2.0F, y_pos, 190.0F);
		oprintf("Version: %s", game_version);

		y_pos -= sysfont_height;
		set_text_position(SPRITE_HRES / 2.0F, y_pos, 180.0F);
		oprintf("BUILD DATE: %s", __DATE__);

		y_pos -= sysfont_height;
		set_text_position(SPRITE_HRES / 2.0F, y_pos, 170.0F);
		oprintf("BUILD TIME: %s", __TIME__);
	}

	/* Get and display the serial number and dom from the PIC */
	show_pic_info(&y_pos);
	
	/* restore the CMOS disk shadow */
	read_cmos_from_disk(FALSE);
	flag = 0;
	if (!init_cmos()) {
		/* check that NEXT_AUD is readable and equals the NEXT_AUD number */
		if (get_audit(NEXT_AUD, &val) || val != NEXT_AUD) {
			/* reset the audits */
			clear_all_audits();
			/* set the valid signature */
			set_audit(NEXT_AUD, NEXT_AUD);
		}
		
#ifndef NOTESTS
		/* Check the adjustments */
		adj_status = check_all_adjustments();
		
		/* any errors detected ? */
		if (adj_status) {
			/* YES - restore all adjustments to factory defaults */
			restore_all_adjustments();
			if (check_all_adjustments())
				cmos_status = CMOS_DEAD;
			else
				cmos_status = CMOS_ADJ_RESTORED;
		}
		
		/* Check the player records */
		if (validate_plyr_records() && !cmos_status)
			cmos_status = CMOS_ADJ_RESTORED;
#endif
		
		/* Should a restore to factory setting be done ? */
		if (get_adjustment(FACTORY_RESTORE_ADJ, &val) || val) {
			cmos_status = CMOS_ADJ_RESTORED;
			set_default_plyr_records();
			set_default_world_records();
			set_adjustment(FACTORY_RESTORE_ADJ, 0);
			clear_all_audits();
		}		
		
		/* Should a clear player records setting be done ? */
		if (!get_adjustment(CLEAR_PLAYER_RECORDS_ADJ, &val) && val) {
			set_default_plyr_records();
			set_adjustment(CLEAR_PLAYER_RECORDS_ADJ, 0);
			flag += 1;
		}
		/* Should a clear world records setting be done ? */
		if (!get_adjustment(CLEAR_WORLD_RECORDS_ADJ, &val) && val) {
			set_default_world_records();
			set_adjustment(CLEAR_WORLD_RECORDS_ADJ, 0);
			flag += 2;
		}
		
#ifndef NOTESTS
		if (!check_all_audits())
			cmos_status = CMOS_DEAD;
		else if (check_all_adjustments())
			cmos_status = CMOS_DEAD;
		else if (validate_plyr_records())
			cmos_status = CMOS_DEAD;
#endif
		
		if (cmos_status != CMOS_DEAD) {
			/* check to see if this is a fresh power on, as opposed to a return from diags */
			if (read_bios_flags() != POWER_UP_SIGNATURE) {
				write_bios_flags(POWER_UP_SIGNATURE);
				increment_audit(NUM_MACHINE_POWER_ON_AUD);
			}
#ifndef NOTESTS
			/* Check the audits */
			check_all_audits();
#endif
			/* Create the uptime process */
			qcreate("utime", NODESTRUCT, uptime_proc, 0, 0, 0, 0);
		}
	} else
		cmos_status = CMOS_DEAD;
	
	y_pos -= sysfont_height;
	set_text_position(SPRITE_HRES/2.0F, y_pos, 140.0F);
	
#ifdef GRAB_SCREEN
	grab_install_proc();
#endif

	/* init coin structs before allowing coins */
	coin_init_all();

	if (!reload)
	{	
		if (!cmos_status) {
			if (flag != 0)
				oprintf("%dcCMOS: %dc%s%s%sCLEARED", WHITE, LT_YELLOW,
						(flag & 1) ? "PLAYER RECORDS " : "", (flag == 3) ? "AND " : "",
						(flag & 2) ? "WORLD RECORDS " : "");
			else
				oprintf("%dcCMOS: %dcOK", WHITE, LT_GREEN);
		} else if(cmos_status == CMOS_ADJ_RESTORED)
			oprintf("%dcCMOS: %dcFACTORY SETTINGS RESTORED", WHITE, LT_YELLOW);
		else
			oprintf("%dcCMOS:  %dcBAD", WHITE, LT_RED);

		y_pos -= sysfont_height;
		set_text_position(SPRITE_HRES / 2.0F, y_pos, 130.0F);
		if(cmos_status == CMOS_DEAD || is_dip_coinage)
			oprintf("%dcUSING DIP COINAGE", LT_RED);
		else
			oprintf("%dcUSING CMOS COINAGE", LT_YELLOW);
	}
		
	/* Get and display the version number from the sound system */
	show_sound_version(&y_pos);
	
	/* Load up the sounds */
	main_init_sound();
	
	//printf("mem:%d - before sound load\n",get_heap_available());
#ifndef NOAUDIO
	load_permanent_snds();
#endif


	/* Assign functions to the coin inputs */
	set_dcsw_handler(LEFT_COIN_SWID, coin_sw_handler);
	set_dcsw_handler(RIGHT_COIN_SWID, coin_sw_handler);
	set_dcsw_handler(CENTER_COIN_SWID, coin_sw_handler);
	set_dcsw_handler(EXTRA_COIN_SWID, coin_sw_handler);
	set_dcsw_handler(BILL_VALIDATOR_SWID, coin_sw_handler);

	/* assign switch handler for service credits button */
#ifndef DBG_HEAP
	set_dcsw_handler(SERVICE_CREDIT_SWID, coin_svc_cred_handler);
#else
	set_dcsw_handler(SERVICE_CREDIT_SWID, do_heap_dump_handler);
#endif

	/* assign switch handler for coin door slam button */
	set_dcsw_handler(SLAM_SWID, coin_slam_handler);

	/* assign switch handler for volume buttons */
	set_dcsw_handler(VOLUME_UP_SWID, coin_volume_handler);
	set_dcsw_handler(VOLUME_DOWN_SWID, coin_volume_handler);

	set_dcsw_handler(TEST_SWID, do_test);
	set_dcsw_handler(DIP_BIT_15_SWID, do_test);
	
#ifdef DEBUG
	set_dcsw_handler(SLAM_SWID, dump_tlist);
#endif
#ifdef PROFILE
	set_dcsw_handler(SLAM_SWID, profile_dump);
	set_dcsw_handler(EXTRA_COIN_SWID, reset_profiles);
#endif
#if defined(SEATTLE)
#ifdef TILT_HEAP
	set_dcsw_handler(SLAM_SWID, dump_audits);
#endif
#endif
#ifdef PASS_DEBUG
	set_dcsw_handler(SLAM_SWID, dump_last_pass);
#endif

	
	//printf("mem:%d - before animation load\n",get_heap_available());
 	/* Load the anim data */
	anim_sys_init();
	failed = load_anim_block(ASB_GEN_SEQ, "GEN_SEQ.ANI", GEN_SEQ_VERSION );
	if (failed)
#ifdef DEBUG
		lockup();
#else
		while (1);
#endif
	//printf("mem:%d - after animation load\n",get_heap_available());

	/* Show the loading message */
	if (!reload)
	{
		y_pos -= sysfont_height * 2;
		set_text_position(SPRITE_HRES / 2.0f, y_pos, 10.0f);
		oprintf("%dcINITIALIZING GAME...", LT_GREEN);
	}

	set_text_z_inc(0.0F);
	
	/* enable the cpu bog detector */
	mthread_enable_framerate();

	/* Clr last game quarter so no bogus stats will be shown in attract */
	game_info_last.game_quarter = 0;

	/* set four_plr_ver */
	four_plr_ver = dip_in.di.reserved2 ? 1 : 0;

	/* used for Team select */
	bought_in_first = 0;

	/* Let the stuff be displayed for a bit */
	sleep(1);

	/* create the coin totalizer/clicker process */
	clicker_proc = qcreate("ccproc", CC_GRP_PID | NODESTRUCT, coin_clicker_proc, 0, 0, 0, 0);

	/* create the volume adjust process */
	volume_proc = qcreate("volume", VOLUME_GRP_PID | NODESTRUCT, coin_volume_proc, 0, 0, 0, 0);

	/* initialize profiling */
#ifdef PROFILE
	reset_profiles(SWITCH_CLOSE, 0);
#endif

	if (reload == 2)		// Switching over from other attract mode?
		reload = 0;

	load_game_info();		// load up any credits from reboot!?

	if (!reload)
	{
		/* Create the attract mode loop */
		set_coin_hold(FALSE);
		iqcreate("attract", AMODE_GRP_PID, attract_loop, 0, 0, 0, 0);
	} else {

		snd_stop_all();
		snd_set_reserved(SND_TRACK_0);
		set_coin_hold(FALSE);

		{
			int count;

			count = snd_get_first_sound(cursor_bnk_str);

			tick_sndn = count + TICK_SND;
			coin_snd_drop_calln = count + COIN_SND_DROP_CALL;
			coin_snd_paid_calln = count + COIN_SND_PAID_CALL;
		}

		game_info.game_state   = GS_PRE_GAME;
		qcreate("pre_game", PRE_GAME_SCRNS_PID, pre_game_scrns, 0, p_active, 0, 0);
	}


	/* Disable drawing until attract starts up */
	draw_enable(0);
}	/* main_proc */

/*
 * This routine is called from 'plyr_strt_butn' and it handles the updating and setting of
 * the players credit info.
 *
 * RETURNS: 0 - plr cant buy anymore, 1 - ok
 */
int paid_credits_add(int pnum, int creds)
{
	game_info_t *pgi = &game_info;
	pdata_t *pdata = pdata_blocks + pnum;
	int paid, full;
	
	// Calc proper # of buyable quarters
	full = NUM_QUARTERS - pgi->game_quarter;
	if (full < 1)
		full = 1;
	
	// Exit if player has bought all available game quarters
	if (pdata->quarters_purchased >= full)
		return 0;
	
	// Are we in pre-game ?
	if (pgi->game_state == GS_PRE_GAME)
	{
		// YES - Has the player already purchased at least 1 quarter
		if (pdata->quarters_purchased)
		{
			// YES - how much has he paid so far
			paid = get_credits_to_start() + ((pdata->quarters_purchased - 1) * get_credits_to_continue());
			
			// How many more does he need for a full game
			full = get_full_game_credits() - paid;
			
			// Is the number left for a full game purchase less than the number to continue ?
			if (full <= get_credits_to_continue())
			{
				// YES - Are there enough credits to get the full game ?
				if (full <= get_credits())
				{
					// YES - set the creds value to full
					creds = full;
					
					// Award a full game
					pdata->quarters_purchased = NUM_QUARTERS;
					
					// Incrment amount player paid
					pdata->credits_paid += creds;
					
					// Decrement available credits
					decrement_credits(creds);
					
					// Set flag saying which player has bought a full game
					game_purchased |= (1 << pnum);
					
					// Make it obvious to the player, but not during the VS page.
					if (!vs_page_showing)
						qcreate("fgbought",FULLGAME_BOUGHT_PID,fullgame_bought_msg,pnum,0,0,0);
					
					return 1;
				} else {
					// NOT enough credits to get the full game, Do nothing
					return 0;
				}
			} else if (get_credits() >= get_credits_to_continue()) {
				// Takes more to get full game than to continue
				// Are there enough credits to continue ?
				// YES - Award another quarter
				pdata->quarters_purchased++;
				
				// Increment amount player paid
				pdata->credits_paid += creds;
				
				//  Decrement available credits
				decrement_credits(creds);
				
				return 1;
			} else {
				/* Otherwise not enough credits for full game and not enough */
				/* credits to continue so do not do anything */
				return 0;
			}
		} else {
			/* No quarters purchase yet - award only one quarter */
			/* Award a quarter */
			pdata->quarters_purchased = 1;
			
			/* Increment amount paid */
			pdata->credits_paid += creds;
			
			/* Decrement number of credits available */
			decrement_credits(creds);
			
			return 1;
		}
	} else {
		/* NOT in pre game - only allow full continue or start price */
		/* Award a quarter */
		pdata->quarters_purchased++;
		
		/* Increment amount he paid */
		pdata->credits_paid += creds;
		
		/* Decrement available credits */
		decrement_credits(creds);
		
		return 1;
	}
}	/* paid_credits_add */

/*
 * This routine is called at the end of every QUARTER. It updates the players purchased info.
 * Clears players' <please_wait>.
 *
 * RETURNS: nothing
 */
void paid_credits_deduct(void)
{
	pdata_t *pdata;
	int i;
	
	for (i = 0; i < MAX_PLYRS; i++) {
		pdata = pdata_blocks + i;
		/* Is player in game? */
		if (p_status & (1 << i)) {
			/* YES - decrement quarters purchased cnt (should always happen) */
			if (pdata->quarters_purchased)
				pdata->quarters_purchased--;
			
			/* Does this player have any quarters left ? */
			if (pdata->quarters_purchased <= 0) {
				/* NOPE - Take him out of the game */
				p_status &= ~(1 << i);
				pdata->credits_paid = 0;
			}
		}
		pdata->please_wait = 0;
	}
}	/* paid_credits_deduct */

/*
 * Do common start button processing; called from <plyr_strt_butn>.
 *
 * RETURNS: 0 - buyin failed; 1 - ok
 */
int do_start_press(int pnum, int creds)
{
	int player_mask;
	
	if (paid_credits_add(pnum, creds)) {
		player_mask = 1 << pnum;
		
		// Make start button press sound 
		//snd_scall_bank(cursor_bnk_str, EAT_CREDIT_SND, VOLUME4, 127, 1);
		snd_scall_bank(cursor_bnk_str, EAT_CREDIT_SND, VOLUME6, 127, PRIORITY_LVL4);
		
		// Was this guy already in game ?
		if ((p_status2 & player_mask) == 0) {
			/* NOPE - Increment total plays */
			increment_audit(TOTAL_PLAYS_AUD);
		}
		
		// Set in-game bit
		p_status |= player_mask;
		
		// Set was in-game bit for future continue
		p_status2 |= player_mask;
		
		return 1;
	} else
		return 0;
}	// do_start_press

/*
 * This is the routine which gets called upon each START BUTTON press
 *
 *   INPUT: signal (up or down), and switch ID
 *  OUTPUT: none
 *
 * Question for Mark - If the player buys a full game up front and the
 * score is tied after 4 quarters, does the player then get overtime quarters
 * until there are either no more or the score at the end of one of the
 * overtime quarters is NOT a tie?  If NOT, we should change the special
 * deal message to read "Purchase 4 quarters up front for only X credits"!!
 */
void plyr_strt_butn(int sig, int swid)
{
	game_info_t *pgi = &game_info;
	pdata_t *pdata;
	int pnum;
	int pnum_p2 = four_plr_ver?1:index_2p[1];
	int can_start;
	int can_continue;
	
	int		i;

#ifdef NO_STARTS
	return;
#endif
	
	/* We only do something when the switch closes */
	if (sig == SWITCH_OPEN)
		return;
	
	/* Check to see if there are enough credits to start */
	can_start = check_credits_to_start();
	
	/* Check to see if there are enough credits to continue */
	can_continue = check_credits_to_continue();

	/* Get player number */
	switch (swid) {
	default:
	case P1_START_SWID:		/* Player 1 */
		pnum = 0;
		if (!(p_status & (1<<pnum_p2)) && can_start)	/* if player two isnt in... set plyr 1's bit */
			bought_in_first |= (1<<0);
		break;
	case P2_START_SWID:		/* Player 2 */
		pnum = pnum_p2;
		if (!(p_status & (1<<0)) && can_start)	/* if player one isnt in... set plyr 2's bit */
			bought_in_first |= (1<<pnum);
		break;
	case P3_START_SWID:		/* Player 3 */
		pnum = 2;
		if (!(p_status & (1<<3)) && can_start)	/* if player four isnt in... set plyr 3's bit */
			bought_in_first |= (1<<2);
		break;
	case P4_START_SWID:		/* Player 4 */
		pnum = 3;
		if (!(p_status & (1<<2)) && can_start)	/* if player three isnt in... set plyr 4's bit */
			bought_in_first |= (1<<3);
		break;
	}
	
	/* Set ptr to player data block */
	pdata = pdata_blocks + pnum;

	/* Seed random # generator */
#if defined(SEATTLE)
	srandom(tick_counter);
#elif defined(VEGAS)
	srandom((int)get_timer_val());
#endif
	
#ifndef RELEASE_VER
 #ifndef TEST_VER
  #ifdef DEBUG
	i = get_joy_val_cur (pnum);

	if (i && (game_info.game_state == GS_ATTRACT_MODE))
	{
		// In case we were in a fade
		normal_screen();

//#define TEAM_ATL	0			// Atlanta
//#define TEAM_BOS	1			// Boston
//#define TEAM_CHA	2			// Charlotte
//#define TEAM_CHI	3			// Chicago
//#define TEAM_CLE	4			// Clevland
//#define TEAM_DAL	5			// Dallas
//#define TEAM_DEN	6			// Denver
//#define TEAM_DET	7			// Detroit
//#define TEAM_GOL	8			// Golden State
//#define TEAM_HOU	9			// Houston
//#define TEAM_IND	10			// Indianapolis
//#define TEAM_LAC	11			// Los Angeles
//#define TEAM_LAL	12			// Los Angeles
//#define TEAM_MIA	13			// Miama
//#define TEAM_MIL	14			// Milwaukee
//#define TEAM_MIN	15			// Minnesota
//#define TEAM_NJN	16			// New Jersey
//#define TEAM_NYK	17			// New York
//#define TEAM_ORL	18			// Orlando
//#define TEAM_PHI	19			// Philadelphia
//#define TEAM_PHO	20			// Phoenix
//#define TEAM_POR	21			// Portland
//#define TEAM_SAC	22			// Sacramento
//#define TEAM_SAN	23			// San Antonio
//#define TEAM_SEA	24			// Seattle
//#define TEAM_TOR	25			// Toronto
//#define TEAM_UTA	26			// Utah
//#define TEAM_VAN	27			// Vancouver
//#define TEAM_WAS	28			// Washington
		
		/* Generate two random teams */
		game_info.team[0] = TEAM_MIA;	//randrng(NUM_TEAMS);
		game_info.team[1] = TEAM_IND;	//randrng(NUM_TEAMS);
		
		/* Set up assorted state information */
		if (four_plr_ver)
		{
			p_status = 0xf;
//			game_purchased = 0xf;
			pdata_blocks[0].quarters_purchased = 0;
			pdata_blocks[1].quarters_purchased = 0;
			pdata_blocks[2].quarters_purchased = 0;
			pdata_blocks[3].quarters_purchased = 0;
			
			if (i == JOY_DOWN) {
				p_status = 1 << pnum;
				game_purchased = p_status;
				pdata_blocks[pnum].quarters_purchased = 4;
			}
			if (i == JOY_LEFT) {
				p_status = 0;		// All drones
			}
#ifdef	SHAWN
//			p_status = 0x4;
//			game_purchased = 0x4;
#endif
#ifdef JASON
			p_status = 0x2;
			game_purchased = 0x2;
			pdata_blocks[1].quarters_purchased = 4;
			pdata_blocks[1].credits_paid = 8;
#endif
		}
		else
		{
			p_status = 5;
			game_purchased = 5;
			pdata_blocks[0].quarters_purchased = 0;
			pdata_blocks[2].quarters_purchased = 0;

			if (i == JOY_DOWN) {
				p_status = 1 << pnum;
				game_purchased = p_status;
				pdata_blocks[pnum].quarters_purchased = 4;
			}
			if (i == JOY_LEFT) {
				p_status = 0;		// All drones
			}
		}

		p_status2 = p_status;

		game_info.game_state = GS_PRE_GAME;

		// Create the game process
		fprintf(stderr, "create game_proc_debug\r\n");
		qcreate("game", 0, game_proc_debug, 0, 0, 0, 0);
		fprintf(stderr, "done with create game_proc_debug\r\n");

		// All done
		return;
	}
  #endif //DEBUG
 #endif //TEST_VER
#endif //RELEASE_VER
	
	/* task handler for start button - we only get here if the player has NOT */
	/* already purchased a full game. */
	switch (pgi->game_state)
	{
		/* Diagnostics mode - We should NEVER see this state */
	case GS_DIAGNOSTICS:
		break;
		
		/* Attract mode - The only thing a player can do from attract mode
		 * is start.  Once started from attract mode the game goes to PRE_GAME
		 * state so continues from attract mode can NOT occur.
		 */
	case GS_ATTRACT_MODE:
		/* Don't allow press if <game_proc> is loading */
		// Make sure the movie player is shut down if it is running
		if(movie_get_state() > 1)
			movie_abort();
		if (existp(GAME_PROC_PID, 0xFFFFFFFF) && texture_load_in_progress)
			return;
		
		snd_stop_all();
		/* trk 0: music, trk 1:audience, trk 5: annoucner */
//		snd_set_reserved(SND_TRACK_0 | SND_TRACK_1 | SND_TRACK_5);
		snd_set_reserved(SND_TRACK_0);
		
		// Enough credits to start ?
		if (can_start)
		{
			// YES - Initialize player state information
			p_status = 0;							// just to make sure
			p_status2 = 0;
			game_purchased = 0;						// no one has bought full game...

			halt = 0;								// I am stuck on Band-Aids...
			
			// clear plyr credit ram
			pdata->credits_paid = 0;
			pdata->quarters_purchased = 0;
			
			pdata_blocks[0].please_wait = 0;
			pdata_blocks[1].please_wait = 0;
			pdata_blocks[2].please_wait = 0;
			pdata_blocks[3].please_wait = 0;
			
			// Give this player 1 quarter
			if (do_start_press(pnum, get_credits_to_start()))
			{
				// Disable the credit page
				credit_pg_on = 0;
				
				// Switch game to PRE_GAME state
				pgi->game_state = GS_PRE_GAME;
				
				// Quarter number is 0
				pgi->game_quarter = 0;

				// Create the pre-game screens
				qcreate("pre_game", PRE_GAME_SCRNS_PID, pre_game_scrns, 0, pnum, 0, 0);
			}
			else
			{
				/* Lockup if failed!!! */
				while (1) ;
			}
		}
		else if (!credit_pg_on)
		{
			/* Fire up the credit page */
			qcreate("coinpg", CREDIT_PAGE_ID, credit_page_proc, 0, 0, 0, 0);
		}
		break;
		
		// Pre-Game state - In this state the player can start, continue, and
		// get the special full game purchase deal
	case GS_PRE_GAME:
		// Has the player already pressed start at least 1 time ?
		if (p_status2 & (1 << pnum))
		{
			// YES - Does the player have enough to continue or has the
			// player bought at least 1 quarter already ?
			if (can_continue || pdata->quarters_purchased)
			{
				// YES - Go award the player another quarter
				if (do_start_press(pnum, get_credits_to_continue()))
				{
#if 0
					/* chalk 'game continue' audit */
					increment_audit(CONTINUE_AUD);
#endif
				}
			}
		}
		else if (can_start)
		{
			/* Player has not started yet.  In two player situations starting
			 * from attract mode the second player to start ends up here.  If
			 * the game is already in this state and a player has not yet started
			 * the player ends up here too.
			 */
			// Clear this players state information
			pdata->credits_paid = 0;
			pdata->quarters_purchased = 0;
			pdata->please_wait = 0;
			
			// Go award a quarter
			if (do_start_press(pnum, get_credits_to_start()))
			{
				// chalk 'game start' audit
				increment_audit(GAME_START_AUD);
				update_gamestart_percents();
			}
		}
		break;
		
		// End of quarter state - In this state the player can start or
		// continue but can NOT get the full game purchase deal.
	case GS_EOQ:
		// Nothing to do if game is over
		if (game_info.game_mode != GM_GAME_OVER)
		{
			/* Has the player already pressed start at least 1 time ? */
			if (p_status2 & (1 << pnum))
			{
				/* YES - Does the player have enough to continue ? */
				if (can_continue)
				{
					/* YES - Go award the player another quarter */
					if (do_start_press(pnum, get_credits_to_continue()))
					{
						/* chalk 'game continue' audit */
						increment_audit(CONTINUE_AUD);
					}
				}
			}
			else if (can_start)
			{
				/* Are there enough credits for the player to start ? */
				/* Clear this players state information */
				pdata->credits_paid = 0;
				pdata->quarters_purchased = 0;
				pdata->please_wait = 0;
				
#ifndef NO_INGAME_RESTART
				// human on team?
				if ((p_status & TEAM_MASK(pnum/2)) || (existp(INGAME_BUYIN_PID,-1)))
				{	// human on team
#endif
					/* chalk 'mid-game start' */
					if (do_start_press(pnum, get_credits_to_start()))
						increment_audit(MID_GAME_START_AUD);
#ifndef NO_INGAME_RESTART
				}
				else
				{	// no human on team
					qcreate("restart", INGAME_BUYIN_PID,do_ingame_restart,pnum,0,0,0);
#ifdef DEBUG
					printf( "---> creating do_ingame_restart process for player %d\n", pnum );
#endif
				}
#endif
			}
		}
		break;
		
		/* In Game state.  In this state we only allow new players that have 
		 * not already started to start up and play as the opposing team if
		 * there is at least MINIMUM_QUARTER_TIME left in the current quarter.
		 */
#ifndef NO_INGAME_RESTART
	case GS_GAME_MODE:
		/* Is the player in the game ? */
		if ((p_status & (1 << pnum)))
			break;

		/* NOPE - are there enough credits for him to start ? */
		if (!can_start)
			break;

		// please wait?
		if (pdata->please_wait)
			break;

		// if guy wants to buy into all-drone team, pop menu
		// for restart.  If human on team, regular start
//		if (p_status & TEAM_MASK(pnum/2))
		if ((p_status & TEAM_MASK(pnum/2)) || (existp(INGAME_BUYIN_PID,-1)))
		{	// human on team
			if ((pgi->game_time >= MINIMUM_QUARTER_TIME) || (coin_check_freeplay() && pgi->game_time)) {
				/* YES - Go award him the quarter */
				if (do_start_press(pnum, get_credits_to_start())) {
					/* chalk 'mid-game start' audit */
					increment_audit(MID_GAME_START_AUD);
				} else {
					/* Lockup if failed!!! */
					while (1) ;
				}
			} else if (!coin_check_freeplay()) {
				/* Not enough time left in the quarter */
				/* Tell him to wait */
				/* (or something else for 4th quarter or OT?) */
				pdata->please_wait = 1;
			}
		}
		else
		{	// no human on team
			/* Are there enough credits for the player to start ? */
			/* Clear this players state information */
			pdata->credits_paid = 0;
			pdata->quarters_purchased = 0;
			pdata->please_wait = 0;
		
			// Pop up the menu and see if these guys want to
			// start the whole game over again.
			qcreate("restart", INGAME_BUYIN_PID,do_ingame_restart,pnum,0,0,0);
#ifdef DEBUG
			printf( "---> creating do_ingame_restart process for player %d\n", pnum );
#endif
		}

		break;
#else
	// #if0'd out here is the old GS_GAME_MODE logic, before Jason started trying
	// to add the 'go-back-and-start-over-on-buyin' code.
	case GS_GAME_MODE:
		/* Is the player in the game ? */
		if (!(p_status & (1 << pnum))) {
			/* NOPE - Is the please wait message NOT being displayed AND */
			/* there ARE enough credits for him to start ? */
			if (!pdata->please_wait && can_start) {
				/* Is there enough time remaining in the current quarter
				 * to allow this player to start or
				 * is it free play with any time left at all?
				 */
				if ((pgi->game_time >= MINIMUM_QUARTER_TIME) || (coin_check_freeplay() && pgi->game_time)) {
					/* YES - Go award him the quarter */
					if (do_start_press(pnum, get_credits_to_start())) {
						/* chalk 'mid-game start' audit */
						increment_audit(MID_GAME_START_AUD);
					} else {
						/* Lockup if failed!!! */
						while (1) ;
					}
				} else if (coin_check_freeplay()) {
					/* Not enough time left in the quarter */
					/* Tell him to wait */
					/* (or something else for 4th quarter or OT?) */
					pdata->please_wait = 1;
				}
			}
		}
		break;
#endif

		/* In this game state the player can only start a new game and should
		 * be allowed to do the same as game states ATTRACT and PRE_GAME.
		 */
	case GS_GAME_OVER:
		break;
		
		/* The game should NEVER be in this state */
	case GS_FREE_PRICE:
	case GS_HALF_PRICE:
		break;
	}
}	/* plyr_strt_butn */

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: main_init_sound()                                               */
/*                                                                           */
/* Initializes the sound system.                                             */
/*                                                                           */
/* Downloads the engine sounds from disk to the sound system.                */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 01 Feb 97                                                             */
/*                                                                           */
/*****************************************************************************/
void main_init_sound(void)
{
	/* Initialize the sound system */
	if (snd_init_multi(SND_OPSYS_0223) != 0)
	{
#ifdef DEBUG
		debugf("main_init_sound(): ERROR initializing sound system.\n");
		lockup();
#endif
	}
	
	/* Check for bogus opsys */
	if (snd_get_opsys_ver() == 0xeeee)
	{
#ifdef DEBUG
		debugf("main_init_sound(): ERROR in sound system\n");
		lockup();
#endif
	}
	
	/* Initialize the sound bank stuff */
	snd_bank_init();
	
	/* Get the volume level from CMOS */
	if (get_adjustment(VOLUME_LEVEL_ADJ, &game_info.master_volume)) {
		/* CMOS Failure - set volume level to some default value */
		game_info.master_volume = 127;
	}
	/* Get the attract mode volume level from CMOS */
	if (get_adjustment(ATTRACT_VOLUME_LEVEL_ADJ, &game_info.attract_mode_volume)) {
		/* CMOS Failure - set volume level to some default value */
		game_info.attract_mode_volume = 40;
	}
	
	/* Set the master volume level */
	snd_master_volume(game_info.master_volume);
}	/* main_init_sound */

void snd_board_error_recovery(void)
{
	/* Initialize the sound system */
	if (snd_init() != 0)
	{
#ifdef DEBUG
		debugf("main_init_sound(): ERROR initializing sound system.\n");
		lockup();
#endif
	}
	
	/* Initialize the sound bank managment goop */
	snd_bank_init();
	
	/* Load permanent sounds */
	load_permanent_snds();
}	/* snd_board_error_recovery */

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void load_permanent_snds(void)
{
	int bad_load = 0;
	int tries = 0;

	// Hold off coin sounds while downloading banks
	coin_sound_holdoff(TRUE);

	// This is totally stupid, but...
	// Since these banks aren't swapped out, we don't necessarily care about
	// the order... so let's just try to load the banks a few times because
	// it's either now or never! -andy the hacking sound guy

	for (tries=0;tries<7;tries++) {
		if (snd_bank_load(anc_misc_bnk) == 0xeeee) {
			bad_load++;
		}
		if (snd_bank_load(anc_take_bnk) == 0xeeee) {
			bad_load++;
		}
		if (snd_bank_load(gameplay_bnk_str) == 0xeeee){	// ball sounds, etc...
			bad_load++;
		}
		if (snd_bank_load(cursor_bnk_str) == 0xeeee){	// front end cursor sounds
			bad_load++;
		}
		if (snd_bank_load(qtr_trans_bnk_str) == 0xeeee) {
			bad_load++;
		}
		if (snd_bank_load(players_bnk_str) == 0xeeee) {
			bad_load++;
		}
//		if (snd_bank_load(crowd_bnk_str) == 0xeeee)	{// NBAonNBC Crowd
//			bad_load++;
//		}
		if (snd_bank_load(buyin_bnk_str) == 0xeeee)	{// buyin-page tune
			bad_load++;
		}
		if (snd_bank_load(start_movie_snd_bnk_str) == 0xeeee) {
			bad_load++;
		}
		if (bad_load == 0) break;
#ifdef DEBUG
		fprintf(stderr,"snd_load_permanent(): UGH! have to try again to load permanent banks (try #%i)\n",tries+1);
#endif
		bad_load = 0;
	}
	if (bad_load != 0) increment_audit(SND_LOAD_FAIL_AUD);

	// Reserve tracks trk 0: music, trk 1:audience, trk 5: annoucner
//	snd_set_reserved(SND_TRACK_0 | SND_TRACK_1 | SND_TRACK_5);
	snd_set_reserved(SND_TRACK_0);

	// Coin sounds are OK to do now
	coin_sound_holdoff(FALSE);
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void main_load_fonts(void)
{
	font_load_info_t *pfli;
	font_node_t *pfnode;
	font_node_t *pfn;
	unsigned int failed;

	/* Load attract or game fonts? */
	pfli = is_low_res ? low_res_fonts : mid_res_fonts;
	
	/* Walk the font load list and load all fonts */
	pfnode = NULL;
	failed = 0;
	while (pfli->font_info) {
		/* Create the font */
		if (!(pfn = create_font(pfli->font_info, pfli->id))) {
#ifdef DEBUG
			debugf("main_load_fonts(): create_font() failed ID# %d\n", pfli->id);
			lockup();
#endif
			failed++;
		} else {
			/* Save ptr to the 1st one created (smallest per table order) */
			if (pfnode == NULL)
				pfnode = pfn;
		}
		failed <<= 1;
		pfli++;
	}
	/* Fail completely if could not create any of them */
	if (!pfnode) {
#ifdef DEBUG
		debugf("main_load_fonts(): all create_font() calls failed -- aborting\n");
		lockup();
#endif
		while (1);
	}

	/* Try fix it if any one of them failed */
	while (failed) {
		/* Go to the next one that failed */
		while (!(failed & 1)) {
			failed >>= 1;
			pfli--;
		}
#ifdef DEBUG
		debugf("main_load_fonts(): failed %X\n", failed);
#endif
		/* Allocate memory for the font node; abort if fails */
		if (!(pfn = (font_node_t *)malloc(sizeof(font_node_t)))) {
#ifdef DEBUG
			debugf("main_load_fonts(): Can not allocate memory for font node -- aborting\n");
			lockup();
#endif
			while(1);
		}
		/* Got a node; default it to the 1st one created but with the new ID */
		*pfn = *pfnode;
		pfn->id = pfli->id;

		/* Link the new node to the font list */
		pfn->prev = NULL;
		pfn->next = font_list;
		if (font_list != NULL)
			font_list->prev = pfn;
		font_list = pfn;

		/* Clr current failure bit */
		failed &= ~1;
	}
}	/* main_load_fonts */

static void inc_pcount(void)
{
	pcount++;
}	/* inc_pcount */

static void dump_tlist(int sig, int id)
{
	if (sig == SWITCH_CLOSE)
		do_list_dump();
}	/* dump_tlist */

void no_start(int sig, int swid)
{
}	/* no_start */

void start_enable(int flag)
{
	int pnum_p2 = four_plr_ver?1:index_2p[1];

	/* Are we in free play mode ? */
	if (flag || coin_check_freeplay()) {
		/* YES - Never disable start buttons */
		start_state = TRUE;
		set_dcsw_handler(P1_START_SWID, plyr_strt_butn);
		set_dcsw_handler(P2_START_SWID, plyr_strt_butn);
		if ((p_status & (1<<0)) == 0)
			pdata_blocks[0].please_wait = 0;
		if ((p_status & (1<<pnum_p2)) == 0)
			pdata_blocks[pnum_p2].please_wait = 0;

		if (four_plr_ver) {
			/* 2/4 plyr kit logic */
			set_dcsw_handler(P3_START_SWID, plyr_strt_butn);
			set_dcsw_handler(P4_START_SWID, plyr_strt_butn);
			if ((p_status & (1<<2)) == 0)
				pdata_blocks[2].please_wait = 0;
			if ((p_status & (1<<3)) == 0)
				pdata_blocks[3].please_wait = 0;
		}
		return;
	} else {
		start_state = FALSE;
		set_dcsw_handler(P1_START_SWID, no_start);
		set_dcsw_handler(P2_START_SWID, no_start);
		if ((p_status & (1<<0)) == 0)
			pdata_blocks[0].please_wait = 1;
		if ((p_status & (1<<pnum_p2)) == 0)
			pdata_blocks[pnum_p2].please_wait = 1;

		if (four_plr_ver) {
			/* 2/4 plyr kit logic */
			set_dcsw_handler(P3_START_SWID, no_start);
			set_dcsw_handler(P4_START_SWID, no_start);
			if ((p_status & (1<<2)) == 0)
				pdata_blocks[2].please_wait = 1;
			if ((p_status & (1<<3)) == 0)
				pdata_blocks[3].please_wait = 1;
		}
	}
}	/* start_enable */

//-----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
int get_start_state(void)
{
	return start_state;
}	/* get_start_state */


static int sw_expect[] = {P1_A, P1_A, P1_B, P1_A, P1_A, P1_B, P1_B, P1_C, P1_C, P1_A};

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
static int check_val(int *sw_array, unsigned count)
{
	int i;

	for (i = 0; i < count; i++)
		if (sw_array[i] != sw_expect[i])
			return 0;
	return 1;
}	/* check_val */

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void backdoor_proc(int *args)
{
	int sw[32];
	int val;
	unsigned	count;

	for (;;) {
		sleep(1);

		/* Reset count */
		count = 0;

		/* Fill in array witch switches */
		while(count < (sizeof(sw_expect) / sizeof(int)) && (get_player_sw_current() & P1_UP)) {
			/* Get player switches */
			val = get_player_sw_current() & ~(P1_UP|0xFFFF0000);

			/* Is one pressed ? */
			if (val) {
				/* Wait for it to release */
				while (get_player_sw_current() & ~P1_UP)
					sleep(1);

				/* Put it in the array */
				sw[count++] = val;

				/* Is what we have so far valid ? */
				if (!check_val(sw, count)) {
					/* NOPE - Start all over */
					break;
				}
			}
			sleep(1);
		}

		/* Is the array filled ? */
		if (count == sizeof(sw_expect) / sizeof(int)) {
			/* YES - Is it the sequence we are looking for ? */
			if (check_val(sw, count)) {
				/* YES - Run diagnostics */
				sw[0] = 0;
				sw[1] = 0;
				sw[2] = 0;
				sw[3] = 0;
				start_diags(sw);
			}
		}
	}
}	/* backdoor_proc */

#if defined(SEATTLE)
//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
static void dump_audits(int sw_state, int sw_id)
{
	if (sw_state == SWITCH_OPEN)
		dump_raw_audits();
}	/* dump_audits */

void dump_stack_trace(void)
{
	int pc, sp, ra, i;
	int *regs = (int *)0x80014280;
	int level = 0, done = FALSE, new = FALSE, new_ra;
	int inst, offset;
	int frames[STACK_DEPTH], copy_buffer[STACK_DEPTH * (STACK_COUNT-1)];
	int *addr, *user_addr;

	pc = regs[PC];
	sp = regs[GP29];
	ra = regs[GP31];

	while ((!done) && (level < STACK_DEPTH)) {
		frames[level] = pc;
		new = FALSE;
		new_ra = FALSE;
		
		while( !new ) {
			pc -= 4;
			inst = *((int *)pc);
			offset = inst & 0xffff;
			/* sign-extend */
			offset = offset << 16;
			offset = offset >> 16;

			if (pc == (int)0x800c4000) {
				done = TRUE;
				new = TRUE;
			} else if (inst == 0x03e00008) {	/* jr ra */
				if ((new_ra == FALSE) && (level > 0)) {
					done = TRUE;
					new = TRUE;
				} else {
					pc = ra;
					new = TRUE;
				}
			} else if ((inst & 0xffff0000) == 0x27bd0000) {	/* addiu sp,sp,CONST */
				sp -= offset;
			} else if ((inst & 0xffff0000) == 0xafbf0000) {	/* sw ra,CONST(sp) */
				ra = *((int *)(sp + offset));
				new_ra = TRUE;
			}
		}
		level++;
	}

	for(i = level; i < STACK_DEPTH; i++)
		frames[i] = 0;

	/* now write them to cmos */
	addr = get_cmos_user_address();
	user_addr = addr + 4096;

	/* shuffle banks */
	cmos_read(user_addr, (char *)copy_buffer, STACK_DEPTH * (STACK_COUNT-1) * sizeof(int) );
	cmos_write(user_addr + STACK_DEPTH * sizeof(int), (char *)copy_buffer, STACK_DEPTH * (STACK_COUNT-1) * sizeof(int));

	/* write new one */
	cmos_write(user_addr, (char *)frames, STACK_DEPTH * sizeof(int));
}	/* dump_stack_trace */
#endif

/*****************************************************************************/
int joinin_selection(int);
void do_ingame_restart( int *parg )
{
	extern struct process_node * plist;
	int suspend_pid[][2] = {
		{-1, PLAYER_PID},
		{-1, BALL_PID},
		{-1, REFEREE_PID},
		{-1, FREETHROW_METER_PID},
		{-1, CAMBOT_PID},
	};
	struct process_node * plst;
	int		will_restart = FALSE;
	int		pnum;
	int		eoq_mode;
	int		was_halt;
	int		i;

	pnum = parg[0];

	#ifdef DEBUG
	printf( "---> starting do_ingame_restart process for player %d\n", pnum );
	#endif

	// Abort if game is over!
	if ((game_info.game_state == GS_EOQ) && (check_scores()) && (
		(game_info.game_mode == GM_GAME_OVER) || (game_info.game_quarter >= EOQ_4)
		))
		return;

	/* Don't allow press if <game_proc> is loading */
	if (existp(GAME_PROC_PID, -1) && texture_load_in_progress)
		return;

	// enough money?
	if (!do_start_press(pnum, get_credits_to_start()))
		return;

	// Make sure the movie player is shut down if it is running
	if(movie_get_state() > 1)
		movie_abort();

	// kill any replay that may be running
	cambot.pop_flg = POP_ABORT;

	#ifdef DEBUG
	printf( "---> sleeping do_ingame_restart process for player %d\n", pnum );
	#endif

	sleep(1);

	// do whatever we need to do, (process freezing, screen dimming, etc) for
	// while the menu is up.
	eoq_mode = (existp(BUYIN_PLAYER_PID, -1) != NULL);
	was_halt = halt;

	if (eoq_mode)
	{
		suspend_multiple_processes( BUYIN_PLAYER_PID, -1 );
		suspend_multiple_processes( TIMER_PID, -1 );
	}
	else
	{
		i = sizeof(suspend_pid)/sizeof(int)/2;
		while (i--)
		{
			plst = plist;
			while (plst)
			{
				if ((plst != cur_proc) &&
					((plst->process_id & suspend_pid[i][0]) == suspend_pid[i][1]))
				{
					if (plst->process_sleep_time & 0x8000)
						plst->process_flags |= PF_SUSPENDED;
					else
						suspend_process(plst);
				}
				plst = plst->next;
			}
		}
		halt = TRUE;
	}

	// do the menu business
	#ifdef DEBUG
	printf( "---> calling joinin_selection for player %d\n", pnum );
	#endif
	will_restart = joinin_selection(pnum);

	if (eoq_mode)
	{
		resume_multiple_processes( BUYIN_PLAYER_PID, -1 );
		resume_multiple_processes( TIMER_PID, -1 );
	}
	else
	{
		i = sizeof(suspend_pid)/sizeof(int)/2;
		while (i--)
		{
			plst = plist;
			while (plst)
			{
				if ((plst != cur_proc) &&
					((plst->process_id & suspend_pid[i][0]) == suspend_pid[i][1]))
				{
					if (plst->process_flags & PF_SUSPENDED)
						plst->process_flags &= ~PF_SUSPENDED;
					else
						resume_process(plst);
				}
				plst = plst->next;
			}
		}
		halt = was_halt;
	}

	if (!will_restart)
	{
		increment_audit(MID_GAME_START_AUD);
		return;
	}

	// Make sure the movie player is shut down if it is running
	if(movie_get_state() > 1)
		movie_abort();

	halt = 0;								// I am stuck on Band-Aids...

	game_over(TRUE);

	// initialize bought_in_first to some reasonable value
	bought_in_first = p_status;
	
	// Create the pre-game screens
	game_info.game_state = GS_PRE_GAME;
	qcreate("pre_game", PRE_GAME_SCRNS_PID, pre_game_scrns, 0, pnum, 0, 0);
	
	/* chalk 'game start' audit */
	increment_audit(CHALLENGER_ACCEPTED_AUD);		/* GAME_START_AUD */
	update_gamestart_percents();

	return;
}

/*****************************************************************************/
