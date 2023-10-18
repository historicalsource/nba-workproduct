/*
 *		$Archive: /video/Nba/sounds.c $
 *		$Revision: 202 $
 *		$Date: 4/20/99 6:58p $
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
char *ss_sounds_c = "$Workfile: sounds.c $ $Revision: 202 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include <stdio.h>
#include <stdlib.h>
#include	<math.h>
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif

/*
 *		USER INCLUDES
 */

#include "id.h"
#include "sndcalls.h"
#include "game.h"
#include "player.h"
#include "externs.h"
#include "anim/gen_seq.h"



#define	SD_2PT_JUMPER		1
#define	SD_3PT_JUMPER		2
#define	SD_3PT_FADE_AWAY	3	
#define	SD_FADE_AWAY		3
#define	SD_1_HAND_LEAN		4
#define	SD_LAYUP			5
#define	SD_DUNK				6
#define	SD_DESPERATION		7
#define	SD_ALLYOOP			8
#define	SD_HOOK_SHOT		9

#define	NAME_DELAY			200
//#define DEBUG_ANNOUNCE
//#define DEBUG_NO_ANNOUNCER

extern unsigned int rsnd[];						// replay sound data indexed with RSND_???

extern int nearest_to_me(int,int);
extern int randrng(int);
extern int what_type_shot;
extern int gRim_hits;
extern int gBack_board_hits;
extern int ally_player;

extern int p2p_angles[NUM_PLAYERS][NUM_PLAYERS];
extern int which_court;

extern int plyr_launched_from_hotspot(int);

extern char	*plyr_bnks[];
extern char team1_bnk_str[];
extern char team2_bnk_str[];
extern char crowd_bnk_str[];
extern char plyr_bnk_str[];
extern char gameplay_bnk_str[];
extern char anc_make_bnk[];
extern char cursor_bnk_str[];
extern char anc_take_bnk[];
extern char anc_misc_bnk[];
extern char players_bnk_str[];
extern int gWaitSayName, gSndTimer, clock_active;

extern float distxz[NUM_PLAYERS][NUM_PLAYERS];

void announce_players_name(obj_3d *pobj);		// called from PRE_TIPOFF seqs.

void goto_freethrow_line_speech(void);

void freethrow_take_speech(obj_3d *pobj);
void freethrow_make_speech(void);
void throw_elbows_speech(void);
void foul_speech(void);
void shake_defender_speech(void);
void deflected_ball_speech(void);
void pump_fake_speech(obj_3d *pobj, int);
void push_speech(bbplyr_data *pusher, int hit_ball_handler);
void goaltending_speech(void);
void one_minute_speech(void);
void clock_low_speech(void);
void shotclock_low_speech(void);
void shotclock_expired_speech(void);
void intercepted_speech(void);
void game_underway_speech(void);
void dunk_take_fx(float);
void pass_to_speech( obj_3d *pobj, int receiver );
void dunk_ball_speech( obj_3d *pobj , int );
void layup_ball_speech( obj_3d *pobj , int );
//void dunk_made_speech( obj_3d *pobj );
void plyr_speech(obj_3d *, int );
void shoe_squeak(obj_3d *pobj);
void juke_shoe_squeak(obj_3d *pobj);
void celebrate_speech(obj_3d *, int);
void loose_ball_speech(obj_3d *);
void replay_speech(void);
void one_on_one_move_speech(obj_3d *,int);
void put_back_speech(int *args);
void freethrow_taunt_speech_proc(int *args);

void hotspot_shot_speech_proc(int *args);

void get_hit_snd(obj_3d *, int, int, int);
void plyr_taunt_speech(obj_3d *, int, int, int);
void grnd_hit_snd(obj_3d *, int, int, int);
void attack_snd(obj_3d *, int, int, int);

void juke_squeaks_proc(int *args);
void shoot_ball_speech_proc(int *args);
void steal_ball_speech_proc(int *args);
void swated_ball_speech_proc(int *args);
void blocked_shot_speech_proc(int *args);
void rebound_speech_proc(int *args);
void shot_missed_speech_proc(int *args);
void freethrow_made_speech_proc(int *args);
void shot_made_speech_proc(int *args);
void versus_speech_proc(void);
void loose_ball_speech_proc(int *args);
void expired_speech_proc(int *args);
void inbound_ball_speech_proc(int *args);

void announce_ball_handler_name_proc(int *args);

int plyr_crossed_halfcourt(int);
int rndrng0(int max);

// external ram
extern int crowd_chanting;
extern int tsec;

int shot_description = -1;
int last_shooter = -1;
int	same_shooter;
int	made_dunk_sp_num = 0;		// used to index into tables...so no repeats...
int	made_2ptr_sp_num = 0;		// used to index into tables...so no repeats... 
int	made_3ptr_sp_num = 0;		// used to index into tables...so no repeats... 

int said_last_second_sp;

static int plyr_hit_snds[] = {2,3,4,12,13,14,20,23,24,25,33,35,39,41};

static int grnd_hit_snds[] = {39,39};

static int plr_land_grunt_snds[] = {5,6,9,12,14,15,16,17,18,24};

static int plr_attack_snds[] = {1,1,10,11,20,36,37,38};

// #95 is a horse snd - use only on horses
//static int far_dunk_spfx_snds[] = {61,62,63,64,65,95};// in ball_etc.bnk
static int far_dunk_spfx_snds[] = {61,62,63,64,65};// in ball_etc.bnk
static int yell_dunk_spfx_snds[] = {7,8,9};// in player bank - yells/grunts
static int short_yell_dunk_spfx_snds[] = {6,23,24,22,19,18,9,6};// in player bank - yells/grunts

//static int swat_snds[] = {53};		// in ball_etc
static int swat_snds[] = {SWIPE1_SND,SWIPE2_SND};

static int pass_snds[] = {PASS1_SND,PASS2_SND};
static int yell_pass_snds[] = {11,12,15,17,18,19};	// Effort grunts played on long passes
//static int juke_grunt_snds[] = {1,10};				// Effort grunts played on jukes

static int pass_to_sp[] = {
			//TO_NAME_PB,
			//HANDS_TO_NAME_PB,
			DISHES_TO_NAME_PB,
			FINDS_NAME_PB };

static int plyr_sp[] = {
			THATS_ALL_NIGHT,			
			YOU_BETTER_RECOGNIZE,	
			YOU_BETTER_ASK_SOMEBODY,
			HOOPERS_DONT_DANCE };

static int shoe_noise[] = {
			SHOE_SQUEAK_1,
			SHOE_SQUEAK_2,
			SHOE_SQUEAK_3,
			SHOE_SQUEAK_4,
			SHOE_SQUEAK_5,
			SHOE_SQUEAK_6 };

// Outdoor shoe noise
static int shoe_noise2[] = {
			SHOE_SCRAPE_1,
			SHOE_SCRAPE_2,
			SHOE_SCRAPE_3,
			SHOE_SCRAPE_4 };


static int after_dunk_taunt_sp[] = {49,50,51,52,53,54,55};		// in 'players.bnk'
static int freethrow_taunt_sp[] = {56,57,58,59};					// in 'players.bnk'

static int after_score_taunt_rob_sp[] = {76,62,62,63,64,68,69,76,70,71,72,73,74,75,76};	// in 'players.bnk'

static int put_back_make_sp[] = {6,6,7,8,152,155,329};

static int running_leaner_take_sp[] = {9,10,11,17,19,20};
static int running_leaner_make_sp[] = {1,5,17,18,21,25,27};

static int fade_away_take_sp[] = {8,14,28,16,15,16,17,96,93,101};
static int fade_away_make_sp[] = {17,19,22,23,24,25,26,27,28,30,31,32,32,33,33,34,155};

static int layup_take_sp[] = {12,12,13,91,134};
static int layup_make_sp[] = {2,3,4,5,9,19,27,29,35,45};
static int layup_miss_sp[] = {54,56,72,79,79,80,80,82,165};

static int jumper2_take_sp[] = {2,3,17,45,49,63,73,2,20,28,97,98,100,106,107};
static int jumper2_far_take_sp[] = {0,1,215,216};
static int jumper2_make_sp[] = {9,17,18,19,22,23,24,25,26,27,28,29,30,31,32,33,34,37,38,40,42,45,48};
static int jumper2_miss_sp[] = {49,50,51,52,53,54,55,56,58,59,61,63,64,65,66,71,72,73,74,75,79,81,82,85,86};

static int wide_open3_take_sp[] = {132,133,133};
static int hand_in_face_take_sp[] = {54,131,131};

static int jumper3_take_sp[] = {6,7,21,7,25,17,23,2,29,32,43,49,63,66,64,69,72,97,98,99,7,3,21,104,108,116,130};
static int jumper3_make_sp[] = {25,27,30,31,38,40,43,96,115,148,149,150,155,158,161,173,174,175};
static int jumper3_4th_qtr_make_sp[] = {161,162,163,164,173};
static int jumper3_miss_sp[] = {49,50,51,52,53,54,55,56,58,59,61,63,64,65,66,71,72,73,74,75,76,77,79,81,82,85,86,157,165,166};

static int heating_up_sp[] = {176,178,184};
static int on_fire_make_sp[] = {177,179,180,181,182,187,189};
static int get_on_fire_sp[] = {183,188,331,331,332};
static int put_fire_out_sp[] = {338,339};

static int tm_heating_up_sp[] = {335,335};
static int tm_fire_make_sp[] = {177,181,189,328,333,334};
static int get_on_tm_fire_sp[] = {336,336,337};


//static int hotspot_take_sp[] = {340,341,342,343};	// in 'sca_makX' bank
static int hotspot_make_sp[] = {179,180,181,182,186,189};

static int hook_shot_take_sp[] = {76,77,78,79,80,81};
static int hook_shot_make_sp[] = {15,15,15,29,42};

static int desperation_take_sp[] = {4,23,36,37,38,61,62,86,120};
static int desperation_make_sp[] = {24,25,26,344,345};
static int last_second_take_sp[] = {55,58,60,71};
static int last_second_make_sp[] = {344,345,346,347,348};
static int last_second_miss_sp[] = {54,56,56};
static int make_and_take_lead_sp[] = {149,150,161,162,164,171,172};

static int baseline_take_sp[] = {30,31,70,89,90,117,134};
static int top_of_key_sp[] = {0,1,82,82,83};
static int in_paint_take_sp[] = {68,26,27,73,74,100,106,107,125,126,127,129,135,136};
static int all_net_make_sp[] = {46,87,88,89,90,91,92};
static int lucky_bounce_make_sp[] = {158,170,171};
static int unlucky_bounce_miss_sp[] = {52,56,61,63,76,77};

static int freethrow_take_sp[] = {2,40,40};

static int again_take_sp[] = {51,52,84,84,85};
static int again_miss_sp[] = {60,60,81};

static int backboard_miss_sp[] = {62,69,70,71};
static int backboard_make_sp[] = {171,209,211,212,212};

static int dunk_take_sp[] = {22,50,97,118,119,119,119,122,122,123,124,137,138,139,140,141,142,143,144,145,146,147,148,149};
static int dunk_make_sp[] = {99,100,101,103,104,105,106,107,108,109,110,111,112,114,115,115,118,122,126,129,133,134,135,137};
static int dunked_on_sp[] = {86,93,95,96,103,106,113,235};
static int dunk_miss_sp[] = {83,84,145,146,147};

static int allyoop_make_sp[] = {300,301,305,306,308,310,312,314};
static int allyoop_miss_sp[] = {83,84,145,146,147};


// misc bank
static int freethrow_make_sp[] = {23,37,38,96};

static int goaltending_sp[] = {328,448,448,449,450,451,452};
static int clock_low_sp[] = {295,323,330};
static int shotclock_low_sp[] = {324,327,329};
static int shotclock_expired_sp[] = {326,325,328,352};
static int game_underway_sp[] = {268,269,272,273};
static int inbound_close_game_misc_sp[] = {288,289,290,291,292,299,300,302,303,308,314,315};
static int inbound_losing_game_misc_sp[] = {216,217,290,291,293};
static int blowout_sp[] = {215,278,293,309,311};
static int loose_ball_sp[] = {41,42,332,335,341,346,346,346};
static int in_air_steal_ball_sp[] = {42,194,340,340,455,460,462,470,471};
static int steal_ball_sp[] = {37,38,39,194,194,334,340,342,343,344,345,349,472};
static int ball_swated_sp[] = {47,50,67,283,467,467,468,468,469,487};
static int post_ball_swated_sp[] = {42,47,58,59,72,77,284,481,482,494};
static int shot_blocked_sp[] = {44,48,50,69,283,284,485};
static int post_shot_blocked_sp[] = {47,50,58,59,61,77,282,283};
static int got_piece_of_ball_sp[] = {480,481,482,483,483};
static int rebound_sp[] = {245,246,249,251,252,256};
static int intercepted_sp[] = {240,338,340,342,343,347,456};
static int deflected_sp[] = {48,480,481,485};
static int replay_sp[] = {143,144,145,149};

static int goto_line_sp[] = {535,537,538,539,543,546};
static int pushed_sp[] = {281,360,361,365,370,371,374,377,380};
static int pushed_ball_handler_sp[] = {281,360,361,365,370,371,374,375,377,380};
//static int pushed_ball_handler_sp[] = {530,532,533,534,555};

static int elbowed_sp[] = {175,178,372,373,374};

static int pump_fake_sp[] = {169,393,393,394,399};

static int fancy_footwork_sp[] = {406,420,421,425,434,435,436,440,441};
static int juke_sp[] = {6,7,9,167,399,425,435};
static int juke_plyr_sp[] = {4,20,21,23,382,383,384,385,386,387,388,389,392,395,397,423,434};
static int juke_left_sp[] = {14,14,393,410,412,415,426};
static int juke_right_sp[] = {15,15,164,393,411,415,413};
static int juke_cross_sp[] = {10,401,405,409};
static int juke_spin_sp[] = {13,170,396,397,427,431,435};





#define TCNV(x)	(((int)((float)(x) * 1.046f)) +1 +1 +1)			//+rndup +endtck +buff

static float full_team_name1_sp_lengths[NUM_TEAMS + 1] = {63.6f,72.8f,70.3f,89.6f,92.9f,78.3f,75.8f,
														75.3f,93.3f,85.4f,88.3f,96.3f,105.1f,
														71.6f,64.7f,83.3f,75.3f,76.6f,77.9f,
														113.8f,63.6f,96.3f,77.4f,80.8f,92.1f,
														78.7f,91.7f,74.5f,91.2f, 90.4f};

static float have_defeated_sp_lengths[NUM_TEAMS + 1] = {
									128.5f,
									115.5f,
									119.3f,
									132.3f,
									134.8f,
									123.1f,
									117.2f,
									132.7f,
									133.1f,
									117.2f,
									141.1f,
									149.8f,
									119.3f,
									121.8f,
									122.2f,
									149.0f,
									131.8f,
									129.8f,
									126.4f,
									105.5f,
									161.1f,
									149.8f,
									146.1f,
									146.5f,
									150.3f,
									137.7f,
									116.4f,
									142.3f,
									143.1f,
									116.8f
};


static float sca_take_sp_lengths[] =  {
			  0.0f, 73.7f, 42.3f, 60.7f, 69.1f, 54.4f, 77.4f, 53.2f, 72.0f, 48.1f,
			   81.6f, 93.8f, 46.9f, 52.3f, 92.9f, 58.6f, 28.9f, 34.3f, 82.5f,123.5f,
			    49.4f, 87.5f, 70.3f, 88.3f, 61.9f, 58.6f, 61.1f, 45.2f, 33.9f, 51.1f,
				 51.9f, 48.6f,110.9f, 76.2f, 95.0f, 79.9f, 77.0f, 57.8f, 64.9f, 57.8f,
				  67.0f, 67.0f,110.1f, 81.6f,108.4f, 54.8f, 79.1f, 50.2f, 49.8f, 36.0f,
				   65.7f, 49.0f, 51.1f, 47.3f, 59.0f, 81.6f, 83.7f, 70.3f, 58.6f, 56.5f,
				    58.6f,103.0f, 96.3f, 40.2f, 57.8f, 56.1f, 45.2f, 48.1f, 50.2f, 80.4f,
					 41.9f, 51.1f, 72.4f, 62.8f, 69.5f, 83.7f, 64.5f, 57.8f, 57.8f, 53.2f,
					  54.8f, 54.8f, 68.2f, 99.6f, 64.9f, 59.4f, 70.3f, 65.3f, 78.3f, 67.0f,
					   58.2f, 88.7f, 69.1f, 78.3f, 79.5f, 84.1f, 54.4f, 59.4f, 59.9f, 64.9f,
					    77.9f, 97.9f, 83.7f, 64.9f, 54.0f, 64.9f, 50.2f, 65.7f, 53.6f,  0.0f,
						 76.2f,111.8f,  0.0f, 59.0f, 64.9f,  0.0f, 49.0f, 47.7f, 44.4f, 51.9f,
						  63.2f, 51.1f, 61.5f, 82.9f, 86.2f, 55.7f, 46.0f, 64.0f, 47.3f, 48.1f, 
						   61.5f, 78.7f, 75.3f, 69.1f, 42.3f, 44.8f, 46.0f, 52.7f, 51.9f, 59.4f,
						    57.8f, 97.1f, 69.9f, 90.4f, 54.0f, 68.6f, 111.3f, 55.7f, 70.7f, 68.6f  };


//static float sca_make_sp_lengths[] =  {92.9f,72.0f,84.5f,41.9f,29.3f,55.2f,86.6f,80.4f,38.1f,44.4f,64.9f,111.3f,71.2f,
//										79.5f,76.2f,64.9f,43.5f,41.9f,41.0f,68.6f,80.8f,60.3f,90.4f,90.4f,60.3f,29.3f,
//										61.9f,78.3f,56.5f,30.6f,37.7f,48.6f,68.6f,51.5f,38.1f,60.3f,70.3f,49.4f,41.9f,
//										41.9f,71.6f,0.0f,0.0f,0.0f,42.7f,36.4f,48.1f,41.9f,106.3f,57.3f,0.0f,67.8f,74.5f,
//										44.4f, 36.8f, 87.9f, 69.5f,0.0f,58.2f, 73.7f, 98.4f, 59.9f, 62.8f, 87.1f,112.2f,112.2f,
//										90.8f, 98.8f, 24.3f, 85.0f, 66.1f, 66.1f, 41.0f, 92.9f, 29.7f, 47.3f, 45.2f, 42.3f, 46.5f,
//										65.7f, 34.3f, 39.3f, 86.2f, 33.5f, 61.1f,131.8f,103.4f, 99.6f,142.7f,0.0f,128.9f,145.7f,
//										0.0f, 72.8f, 69.1f, 62.8f, 56.5f, 54.8f, 76.6f, 65.3f, 75.3f,137.3f, 52.7f,103.0f, 80.4f,
//										0.0f,88.7f, 67.4f, 85.8f, 41.0f,115.1f, 87.9f,0.0f,0.0f, 88.3f,0.0f, 56.1f,0.0f,0.0f,0.0f,
//										91.2f,148.2f,137.7f, 53.6f, 39.8f, 56.5f, 89.6f, 63.2f, 98.8f, 58.2f,0.0f,0.0f,105.1f,
//										112.6f,149.8f, 81.6f,105.9f, 92.9f,0.0f, 76.2f, 79.5f, 98.8f, 90.8f, 75.8f,0.0f, 65.7f,
//										101.7f, 90.4f,153.2f, 59.9f,138.1f,150.3f,162.4f,0.0f,0.0f,108.4f,0.0f,128.1f, 77.4f,0.0f,
//										102.1f,0.0f,0.0f,149.8f,0.0f, 73.7f, 38.5f, 94.6f, 82.0f,103.4f, 62.4f, 67.0f, 90.4f, 55.7f,
//										69.9f, 57.8f, 54.4f, 37.3f, 54.4f, 49.8f, 32.6f, 47.7f, 62.4f, 46.0f, 67.4f, 62.4f, 49.0f,
//										47.3f, 43.1f, 37.7f, 46.5f, 95.0f,0.0f,0.0f,0.0f, 86.2f, 64.5f,0.0f, 54.8f,102.5f, 50.2f,
//										63.6f, 70.7f, 63.2f, 82.5f, 48.1f,131.0f,0.0f, 79.9f,105.5f, 43.1f,0.0f, 81.2f, 56.5f,
//										80.8f,0.0f, 91.2f, 73.7f, 52.3f,0.0f, 77.4f, 96.3f, 79.1f, 81.6f, 78.7f, 80.4f, 85.4f,
//										76.2f, 43.9f,117.2f, 36.0f, 76.2f,123.1f,101.3f,0.0f, 80.4f,103.0f, 81.2f,110.1f,111.3f, 97.5f,
//										0.0f,102.1f, 55.7f, 51.1f, 76.2f,0.0f,0.0f,0.0f,107.6f, 95.0f,112.6f,110.5f,109.7f,120.5f,
//										117.6f,113.8f, 86.2f, 51.5f, 69.5f, 44.4f, 76.6f,0.0f,0.0f,0.0f,0.0f,0.0f,102.5f, 77.9f,
//										107.6f, 89.6f,0.0f,0.0f, 77.4f, 92.1f,117.6f,0.0f, 93.3f,0.0f, 89.2f, 69.1f,113.0f, 83.3f,
//										93.3f, 83.7f, 74.9f, 82.0f, 84.5f, 98.8f, 82.5f,154.0f, 57.3f,0.0f,124.7f,0.0f,108.8f,
//										73.7f, 78.7f,0.0f,0.0f,0.0f, 56.1f, 67.4f,0.0f,0.0f,95.4f, 76.6f, 62.8f,0.0f,73.7f,0.0f,
//										0.0f,0.0f,0.0f, 91.2f,107.6f,0.0f, 51.1f,0.0f};


//static float sca_misc_sp_lengths[] = {};

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void freethrow_taunt_speech_proc(int *args)
{
	sleep(40);
	snd_scall_bank(players_bnk_str, freethrow_taunt_sp[randrng(sizeof(freethrow_taunt_sp)/sizeof(int))], VOLUME3, 127, PRIORITY_LVL3);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void win_game_speech_proc(int *args)
{

	// wait a little bit..for other speech to finish... ie, last shot missed/made speech
	while(gSndTimer > 0)
		sleep(1);

	sleep(90);	//30

	if (compute_score(0) > compute_score(1))
	{	// team 1 won
		if (randrng(100) < 50)
			snd_scall_bank(team1_bnk_str, 6, ANCR_VOL2, 127, LVL5);
		else
		{
			snd_scall_bank(team1_bnk_str, 7, VOLUME7, 127, LVL5);
			sleep(TCNV(have_defeated_sp_lengths[game_info.team[0]]));
			snd_scall_bank(team2_bnk_str, 2, VOLUME7, 127, LVL5);
		}
	}
	else
	if (compute_score(1) > compute_score(0))
	{	// team 2 won
		if (randrng(100) < 50)
			snd_scall_bank(team2_bnk_str, 6, ANCR_VOL2, 127, LVL5);
		else
		{
			snd_scall_bank(team2_bnk_str, 7, VOLUME7, 127, LVL5);
			sleep(TCNV(have_defeated_sp_lengths[game_info.team[1]]));
			snd_scall_bank(team1_bnk_str, 2, VOLUME7, 127, LVL5);
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void throw_elbows_speech(void)
{
	snd_scall_bank(anc_misc_bnk, elbowed_sp[randrng(sizeof(elbowed_sp)/sizeof(int))], ANCR_VOL2, 127, LVL1);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void freethrow_take_speech(obj_3d *pobj)
{
	int		shooter = ((bbplyr_data *)pobj)->plyrnum;


	// if on fire... make special sound
	if (((game_info.fire_cnt >= FIRECNT) && (shooter == game_info.fire_player)) ||
		 (game_info.team_fire == player_blocks[shooter].team))
	{
		snd_scall_bank(gameplay_bnk_str, ON_FIRE_SHOT_SND, VOLUME7, 127, PRIORITY_LVL4);
	}

	// say name or general phrase
	if (randrng(100) < 85)
		snd_scall_bank(plyr_bnks[shooter], LAST_NAME_2_PB, ANCR_VOL2, 127, LVL3);
	else
	 	snd_scall_bank(anc_take_bnk, freethrow_take_sp[randrng(sizeof(freethrow_take_sp)/sizeof(int))], ANCR_VOL2, 127, LVL3);
}				

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void freethrow_make_speech(void)
{
	int rnd;

//	if (COURT_IS_INDOORS)
	if (1)
	{
		rnd = randrng(100);
		if (rnd < 6)
			snd_scall_bank(crowd_bnk_str, CROWD_BOO_SND, (randrng(100) < 50) ? VOLUME6 : VOLUME4, 127, PRIORITY_LVL3);
		else if (rnd < 40)
			snd_scall_bank(crowd_bnk_str, CROWD2_SWELL1_SND, (randrng(100) < 50) ? VOLUME7 : VOLUME4, 127, PRIORITY_LVL3);
		else if (rnd < 80)
			snd_scall_bank(crowd_bnk_str, CROWD2_SWELL2_SND, (randrng(100) < 50) ? VOLUME6 : VOLUME4, 127, PRIORITY_LVL3);
		else
			snd_scall_bank(crowd_bnk_str, CROWD2_SWELL3_SND, (randrng(100) < 50) ? VOLUME6 : VOLUME4, 127, PRIORITY_LVL3);
	}

 	snd_scall_bank(anc_misc_bnk, freethrow_make_sp[randrng(sizeof(freethrow_make_sp)/sizeof(int))], ANCR_VOL2, 127, LVL3);
}				

	
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void announce_ball_handler_name_proc(int *args)
{
	
	gWaitSayName = 0;
	while(game_info.game_time > 0)
	{
		sleep(NAME_DELAY);
		if ((gSndTimer == 0) &&
			(game_info.game_mode == GM_IN_PLAY) &&
			(clock_active) &&
			(game_info.ball_handler != NO_BALLHANDLER) &&
			(!gWaitSayName))
		{
			snd_scall_bank(plyr_bnks[game_info.ball_handler], LAST_NAME_1_PB, ANCR_VOL2, 127, LVL2);
			gWaitSayName = NAME_DELAY;
			gSndTimer = 29;
		}
	}
}
										

//--------------------------------------------------------------------------------------------------------------------------
// called from SEQ's
//--------------------------------------------------------------------------------------------------------------------------
void announce_players_name(obj_3d *pobj)
{
	int dude = ((bbplyr_data *)pobj)->plyrnum;
	int rnd;	

	// have crowd make some noise
//	if (COURT_IS_INDOORS)
	if (1)
	{
		rnd = randrng(100);
		if (rnd < 6)
			snd_scall_bank(crowd_bnk_str, CROWD_BOO_SND, (randrng(100) < 50) ? VOLUME6 : VOLUME4, 127, PRIORITY_LVL3);
		else if (rnd < 40)
			snd_scall_bank(crowd_bnk_str, CROWD2_SWELL1_SND, (randrng(100) < 50) ? VOLUME6 : VOLUME4, 127, PRIORITY_LVL3);
		else if (rnd < 60)
			snd_scall_bank(crowd_bnk_str, CROWD2_SWELL2_SND, (randrng(100) < 50) ? VOLUME5 : VOLUME4, 127, PRIORITY_LVL3);
		else
			snd_scall_bank(crowd_bnk_str, CROWD2_SWELL3_SND, (randrng(100) < 50) ? VOLUME6 : VOLUME4, 127, PRIORITY_LVL3);
	}
	
	// announce name	
	snd_scall_bank(plyr_bnks[dude], FULL_NAME_PB, ANCR_VOL2, 127, LVL1);
}


//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void game_underway_speech(void)
{
 	snd_scall_bank(anc_misc_bnk, game_underway_sp[randrng(sizeof(game_underway_sp)/sizeof(int))], VOLUME7, 127, LVL3);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void goto_freethrow_line_speech(void)
{
	snd_scall_bank(anc_misc_bnk, goto_line_sp[randrng(sizeof(goto_line_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void push_speech(bbplyr_data *pusher, int hit_ball_handler)
{
	// no speech, even if lower priority...
	if (gSndTimer > 0)
		return;

	// only say speech...for ball handler...unless no ball handler
//	if ((game_info.ball_handler != pusher->plyrnum) && (game_info.ball_handler != NO_BALLHANDLER))
//		return;

//	if (COURT_IS_INDOORS)
	if (1)
	{
		if (randrng(100) < 8)
			snd_scall_bank(crowd_bnk_str, CROWD_OH_SND, (randrng(100) < 50) ? VOLUME6 : VOLUME5, 127, PRIORITY_LVL2);
		else if (randrng(100) < 4)
			snd_scall_bank(crowd_bnk_str, CROWD_BOO_SND, (randrng(100) < 50) ? VOLUME6 : VOLUME5, 127, PRIORITY_LVL2);
	}
	if (hit_ball_handler)
	 	snd_scall_bank(anc_misc_bnk, pushed_ball_handler_sp[randrng(sizeof(pushed_ball_handler_sp)/sizeof(int))], ANCR_VOL2, 127, LVL3);
	else if (randrng(100) < 5)
	 	snd_scall_bank(anc_misc_bnk, pushed_sp[randrng(sizeof(pushed_sp)/sizeof(int))], ANCR_VOL2, 127, LVL1);
	gSndTimer = 80;
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void goaltending_speech(void)
{
 	snd_scall_bank(anc_misc_bnk, goaltending_sp[randrng(sizeof(goaltending_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
}


//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void one_minute_speech(void)
{
	if(game_info.game_quarter < EOQ_4)
		// 1 min left
	 	snd_scall_bank(anc_misc_bnk, 286, ANCR_VOL2, 127, LVL2);
	else
	{
		if (randrng(2))
			// 1 min left in regulation
		 	snd_scall_bank(anc_misc_bnk, 287, ANCR_VOL2, 127, LVL2);
		else
			// 1 min left
		 	snd_scall_bank(anc_misc_bnk, 286, ANCR_VOL2, 127, LVL2);
	}
}


//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void clock_low_speech(void)
{
 	snd_scall_bank(anc_misc_bnk, clock_low_sp[randrng(sizeof(clock_low_sp)/sizeof(int))], ANCR_VOL2, 127, LVL2);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void shotclock_low_speech(void)
{
 	snd_scall_bank(anc_misc_bnk, shotclock_low_sp[randrng(sizeof(shotclock_low_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void shotclock_expired_speech(void)
{
	qcreate("expired", SPEECH_PID, expired_speech_proc, 0, 0, 0, 0);
}


//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void expired_speech_proc(int *args)
{

	snd_scall_bank(gameplay_bnk_str, LOOSE_BALL_SND, VOLUME7, 127, PRIORITY_LVL4);

	sleep(30);
		
//	if (randrng(100) < 50))
   	snd_scall_bank(anc_misc_bnk, shotclock_expired_sp[randrng(sizeof(shotclock_expired_sp)/sizeof(int))], ANCR_VOL2, 127, LVL3);
}


//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void intercepted_speech(void)
{
	// loose ball snd 
	snd_scall_bank(gameplay_bnk_str, LOOSE_BALL_SND, VOLUME7, 127, PRIORITY_LVL4);
 	snd_scall_bank(anc_misc_bnk, intercepted_sp[randrng(sizeof(intercepted_sp)/sizeof(int))], ANCR_VOL2, 127, LVL2);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void versus_speech_proc(void)
{
//	snd_scall_bank(anc_misc_bnk,202,VOLUME3,127,LVL2);
//	sleep(TCNV(58.6f));

	snd_scall_bank(team1_bnk_str,1,VOLUME6,127,LVL2);
	sleep((int)(full_team_name1_sp_lengths[game_info.team[0]]));

	if (randrng(2))
	{	snd_scall_bank(anc_misc_bnk,354,VOLUME6,127,LVL3);	// say "take on"
		sleep(TCNV(36.8f));
	}
	else
	{
		snd_scall_bank(anc_misc_bnk,355,VOLUME6,127,LVL3); // say "go up against"
		sleep(TCNV(53.6f));
	}

	snd_scall_bank(team2_bnk_str,2,VOLUME6,127,LVL4);
//	sleep(full_team_name2_sp_lengths[game_info.team[1]]);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void inbound_ball_speech_proc(int *args)
{
	int off_scr,def_scr,scr_diff, rnd;

	off_scr = compute_score(game_info.off_side);
	def_scr = compute_score(!game_info.off_side);
	scr_diff = ABS(off_scr - def_scr);

	if (gSndTimer > 50)
		die(0);
	
	while(gSndTimer > 0)
		sleep(1);

	// last 15 seconds of 4th/overtime quarter and just took lead ?
	if ((game_info.game_quarter >= 3) &&
		(scr_diff <= 3) &&
		(game_info.game_time < 0x00000f00))
	{
		snd_scall_bank(anc_misc_bnk, inbound_close_game_misc_sp[randrng(sizeof(inbound_close_game_misc_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
		return;
	}

	// last 30 seconds of 4th/overtime quarter and just took lead ?
	if ((game_info.game_quarter >= 3) &&
		(scr_diff < 12) &&
		(game_info.game_time <= 0x00002d00) &&
		(game_info.game_time > 0x00000a00))
	{
		snd_scall_bank(anc_misc_bnk, inbound_losing_game_misc_sp[randrng(sizeof(inbound_losing_game_misc_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
		return;
	}

	//
	// isiah speech...should go here ???
	//

	// maybe announce score difference...
	// up by 1 ?
	if ((randrng(100) < 30) && (scr_diff == 1) && (off_scr > def_scr))
	{
		snd_scall_bank((!game_info.off_side ? team1_bnk_str : team2_bnk_str), 3, ANCR_VOL2, 127, LVL4);
	}
	// up by 2 ?
	else if ((randrng(100) < 30) && (scr_diff == 2) && (off_scr > def_scr))
	{
		snd_scall_bank((!game_info.off_side ? team1_bnk_str : team2_bnk_str), 4, ANCR_VOL2, 127, LVL4);
	}
	// up by 3 ?
	else if ((randrng(100) < 30) && (scr_diff == 3) && (off_scr > def_scr))
	{
		snd_scall_bank((!game_info.off_side ? team1_bnk_str : team2_bnk_str), 5, ANCR_VOL2, 127, LVL4);
	}
	// tied ?
	else if ((randrng(100) < 30) && (scr_diff == 0) && (game_info.game_quarter > 1))
	{
		snd_scall_bank(anc_misc_bnk, 307, ANCR_VOL2, 127, LVL4);
	}
	// if the scores are close and the defense is losing... yell...DEFENSE...DEFENSE
	rnd = randrng(100);
	if ((rnd < 20) && (crowd_chanting == NO) && (game_info.game_time > 0x00000900))
	{
		crowd_chanting = YES;
		sleep(3 * tsec + randrng(60));
		if (game_info.game_state == GS_EOQ)
			die(0);
//		if (COURT_IS_INDOORS)
		if (1)
			snd_scall_bank(crowd_bnk_str, CROWD1_YELL_DEFENSE, VOLUME6, 127, PRIORITY_LVL2);
		sleep(TCNV(84.5f));
		if (game_info.game_state == GS_EOQ)
			die(0);
//		if (COURT_IS_INDOORS)
		if (1)
			snd_scall_bank(crowd_bnk_str, 3, VOLUME5, 127, PRIORITY_LVL2);
		sleep(TCNV(103.8f));
		if (game_info.game_state == GS_EOQ)
			die(0);
//		if (COURT_IS_INDOORS)
		if (1)
			snd_scall_bank(crowd_bnk_str, CROWD1_YELL_DEFENSE, VOLUME6, 127, PRIORITY_LVL2);
		crowd_chanting = NO;
	}
	else if ((rnd < 40) && (crowd_chanting == NO) && (game_info.game_time > 0x00000900))
	{
		crowd_chanting = YES;
		sleep(3 * tsec + randrng(60));
		if (game_info.game_state == GS_EOQ)
			die(0);
//		if (COURT_IS_INDOORS)
		if (1)
			snd_scall_bank(crowd_bnk_str, CROWD_GO_TM_GO_SP, VOLUME6, 127, PRIORITY_LVL2);
		sleep(TCNV(87.9f));
		if (game_info.game_state == GS_EOQ)
			die(0);
//		if (COURT_IS_INDOORS)
		if (1)
			snd_scall_bank(crowd_bnk_str, CROWD_GO_TM_GO_SP, VOLUME6, 127, PRIORITY_LVL2);
		crowd_chanting = NO;
	}
//	else if ((rnd < 50) && (crowd_chanting == NO) && (game_info.game_time > 0x00000900))
//	{
//		snd_scall_bank(gameplay_bnk_str, 122, VOLUME3, 127, PRIORITY_LVL2);
//	}
}

//--------------------------------------------------------------------------------------------------------------------------
//				This ANI_CODE routine plays the sound call specified or if -1 then a random sound
//
// NOTE: 'PLAYERS.BNK' is assumed
//
// INPUT: -1 = play random else sound call number
//--------------------------------------------------------------------------------------------------------------------------
void plyr_speech(obj_3d *pobj, int sp)
{

	if (sp == -1)
		snd_scall_bank(players_bnk_str, plyr_sp[randrng(sizeof(plyr_sp)/sizeof(int))], VOLUME5, 127, PRIORITY_LVL1);
	else
		snd_scall_bank(players_bnk_str, sp, VOLUME6, 127, PRIORITY_LVL2);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void shoe_squeak(obj_3d *pobj)
{
	if(pobj->flags & PF_ONSCREEN)
//		if (COURT_IS_INDOORS)
			snd_scall_bank(gameplay_bnk_str, shoe_noise[randrng(sizeof(shoe_noise)/sizeof(int))], VOLUME2, 127, PRIORITY_LVL1);
//		else
//		  	snd_scall_bank(gameplay_bnk_str, shoe_noise[randrng(sizeof(shoe_noise)/sizeof(int))], VOLUME2, 127, PRIORITY_LVL1);
// These suck
//			snd_scall_bank(gameplay_bnk_str, shoe_noise2[randrng(sizeof(shoe_noise)/sizeof(int))], VOLUME2, 127, PRIORITY_LVL1);
}
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void juke_shoe_squeak(obj_3d *pobj)
{
	qcreate("squeak", SPEECH_PID, juke_squeaks_proc, 0, 0, 0, 0);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void juke_squeaks_proc(int *args)
{
	snd_scall_bank(gameplay_bnk_str, shoe_noise[randrng(sizeof(shoe_noise)/sizeof(int))], VOLUME4, 127, PRIORITY_LVL1);
	sleep(randrng(8)+4);
	snd_scall_bank(gameplay_bnk_str, shoe_noise[randrng(sizeof(shoe_noise)/sizeof(int))], VOLUME4, 127, PRIORITY_LVL1);
	sleep(randrng(8)+4);
	snd_scall_bank(gameplay_bnk_str, shoe_noise[randrng(sizeof(shoe_noise)/sizeof(int))], VOLUME4, 127, PRIORITY_LVL1);
	sleep(randrng(8)+4);
	snd_scall_bank(gameplay_bnk_str, shoe_noise[randrng(sizeof(shoe_noise)/sizeof(int))], VOLUME4, 127, PRIORITY_LVL1);
//	sleep(randrng(8)+4);
//	snd_scall_bank(gameplay_bnk_str, shoe_noise[randrng(sizeof(shoe_noise)/sizeof(int))], VOLUME3, 127, PRIORITY_LVL1);

}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void pass_to_speech(obj_3d *pobj, int receiver)
{
	int plyr = ((bbplyr_data *)pobj)->plyrnum;

#ifdef DEBUG_NO_ANNOUNCER
	return;
#endif
	// swoosh sound for passing ball
	if (player_blocks[ball_obj.int_receiver].odata.flags & (PF_ALLYDUNKER))
		snd_scall_bank(gameplay_bnk_str, 110, VOLUME7, 127, PRIORITY_LVL5);
	else
		snd_scall_bank(gameplay_bnk_str, pass_snds[randrng(sizeof(pass_snds)/sizeof(int))], VOLUME7, 127, PRIORITY_LVL5);

	// if pass will travel less than 40 somthings....no speech.
	if (distxz[plyr][player_blocks[receiver].plyrnum] <= 40.0f)
//		return;
		;
	else if (distxz[plyr][player_blocks[receiver].plyrnum] >= 130.0f)
		snd_scall_bank(players_bnk_str, yell_pass_snds[randrng(sizeof(yell_pass_snds)/sizeof(int))], VOLUME3, 127, PRIORITY_LVL3);

	// sometimes just say nothing
	if ((game_info.game_mode == GM_INBOUNDING) && (randrng(100) < 50))
	{
		if (randrng(100) < 75)
			snd_scall_bank(plyr_bnks[receiver], HANDS_TO_NAME_PB, ANCR_VOL2, 127, LVL2);
		else
			snd_scall_bank(plyr_bnks[receiver], TO_NAME_PB, ANCR_VOL2, 127, LVL2);
	
		gSndTimer = 75;
		return;
	}

	if (randrng(100) < 60)
		return;

	if ((distxz[plyr][receiver] <= 25.0f) && (randrng(100) < 30))
		snd_scall_bank(plyr_bnks[receiver], HANDS_TO_NAME_PB, ANCR_VOL2, 127, LVL2);
	else
		snd_scall_bank(plyr_bnks[receiver], pass_to_sp[randrng(sizeof(pass_to_sp)/sizeof(int))], ANCR_VOL2, 127, LVL2);
	
	gSndTimer = 75;

}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void steal_ball_speech_proc(int *args)
{
	int stealer = player_blocks[args[0]].plyrnum;

#ifdef DEBUG_NO_ANNOUNCER
	die(0);
#endif
	if (gSndTimer < 8)
	{
		while(gSndTimer > 0)
			sleep(1);
	}

	if (player_blocks[stealer].odata.y > 4.0f)
		snd_scall_bank(anc_misc_bnk, in_air_steal_ball_sp[randrng(sizeof(in_air_steal_ball_sp)/sizeof(int))], ANCR_VOL2, 127, LVL3);
	else
		snd_scall_bank(anc_misc_bnk, steal_ball_sp[randrng(sizeof(steal_ball_sp)/sizeof(int))], ANCR_VOL2, 127, LVL3);

	//!!!FIX -- read from table
	gSndTimer = 60;
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void deflected_ball_speech(void)
{
#ifdef DEBUG_NO_ANNOUNCER
	return;
#endif
   	snd_scall_bank(anc_misc_bnk, deflected_sp[randrng(sizeof(deflected_sp)/sizeof(int))], ANCR_VOL2, 127, LVL2);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void loose_ball_speech(obj_3d *pobj)
{

#ifdef DEBUG_NO_ANNOUNCER
	return;
#endif

	qcreate("lbs", SPEECH_PID, loose_ball_speech_proc, 0, 0, 0, 0);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void loose_ball_speech_proc(int *args)
{
	// loose ball snd 
	snd_scall_bank(gameplay_bnk_str, LOOSE_BALL_SND, VOLUME7, 127, PRIORITY_LVL4);

	while(gSndTimer > 0)
		sleep(1);

	sleep(10);

	if (game_info.ball_handler != NO_BALLHANDLER)
		die(0);
		
	snd_scall_bank(anc_misc_bnk, loose_ball_sp[randrng(sizeof(loose_ball_sp)/sizeof(int))], ANCR_VOL2, 127, LVL3);

	//!!!FIX -- read from table
	gSndTimer = 60;
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void swated_ball_speech_proc(int *args)
{
	int swater = args[0];
	int	post_speech = 0;			// flag indicates wheter to say INSTANT speech or after the fact speech
	int	ball_not_free = args[1];
#ifdef DEBUG_NO_ANNOUNCER
	die(0);
#endif

	// loose ball snd 
	snd_scall_bank(gameplay_bnk_str, LOOSE_BALL_SND, VOLUME7, 127, PRIORITY_LVL5);

	// wait a little bit...but not too long :)
	if (gSndTimer < 25)
	{
		while(gSndTimer > 0)
			sleep(1);
	}

	if (ball_not_free)
		snd_scall_bank(anc_misc_bnk, got_piece_of_ball_sp[randrng(sizeof(got_piece_of_ball_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
	else if (randrng(100) < 50)
//	else if (post_speech)
	  	snd_scall_bank(anc_misc_bnk, post_ball_swated_sp[randrng(sizeof(post_ball_swated_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
	else
		snd_scall_bank(anc_misc_bnk, ball_swated_sp[randrng(sizeof(ball_swated_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);

	//!!!FIX -- read from table ?
	gSndTimer = 70;
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void blocked_shot_speech_proc(int *args)
{
	int blocker = args[0];
	int	post_speech = 0;			// flag indicates wheter to say INSTANT speech or after the fact speech
#ifdef DEBUG_NO_ANNOUNCER
	die(0);
#endif


	// loose ball snd 
	snd_scall_bank(gameplay_bnk_str, LOOSE_BALL_SND, VOLUME7, 127, PRIORITY_LVL5);
	
	// wait a little bit...but not too long :)
	if (gSndTimer < 25)
	{
		while(gSndTimer > 0)
			sleep(1);
	}

	if (args[1])
		snd_scall_bank(anc_misc_bnk, got_piece_of_ball_sp[randrng(sizeof(got_piece_of_ball_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
	else if (randrng(100) < 50)
//	else if (post_speech)
	  	snd_scall_bank(anc_misc_bnk, post_shot_blocked_sp[randrng(sizeof(post_shot_blocked_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
	else
	  	snd_scall_bank(anc_misc_bnk, shot_blocked_sp[randrng(sizeof(shot_blocked_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);

	//!!!FIX -- read from table ?
	gSndTimer = 70;
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void shake_defender_speech(void)
{
	snd_scall_bank(anc_misc_bnk, juke_plyr_sp[randrng(sizeof(juke_plyr_sp)/sizeof(int))], ANCR_VOL2, 127, LVL3);
	gSndTimer = 65;
}

//--------------------------------------------------------------------------------------------------------------------------
// quad8 works as follows:
// defender is:
//	0-behind, 1-left.behind, 2-left, 3-left.front
//	4-front, 5-right.front, 6-right, 7-right.behind
//--------------------------------------------------------------------------------------------------------------------------
void one_on_one_move_speech(obj_3d *pobj,int seq)
{
	int		quad8 = 0,cp = 0;
	bbplyr_data	* juker = (bbplyr_data *)pobj;

#ifdef DEBUG_NO_ANNOUNCER
	return;
#endif

//	snd_scall_bank(gameplay_bnk_str,  JUKE_MOVE_SND, VOLUME3, 127, PRIORITY_LVL2);
//	snd_scall_bank(players_bnk_str, juke_grunt_snds[randrng(sizeof(juke_grunt_snds)/sizeof(int))], VOLUME3, 127, PRIORITY_LVL3);

//	if (randrng(100) < 20)
//		return;				// dont say something everytime

	// if other speech...not this!
	if(gSndTimer > 0)
		return;

	cp = nearest_to_me(juker->plyrnum, !juker->team);
	quad8 = pobj->fwd - p2p_angles[juker->plyrnum][cp];
	quad8 = ((quad8 + 1536 + 64) &1023) / 128;		// eight directions

	if ((quad8 == 4) && (distxz[juker->plyrnum][cp] <= FT(4.0f)))
	{
		shake_defender_speech();
		gSndTimer = 75;
		return;
	}
	switch(seq)
	{
		case SEQ_D_BLEG_L:
		case SEQ_D_BLG1_L:
		case SEQ_D_BLEG_R:
		case SEQ_D_BLG1_R:
			if (randrng(100) < 10)
				snd_scall_bank(anc_misc_bnk, 407, ANCR_VOL2, 127, LVL2);	// between legs
			else
				snd_scall_bank(anc_misc_bnk, juke_cross_sp[randrng(sizeof(juke_cross_sp)/sizeof(int))], ANCR_VOL2, 127, LVL2);
			break;
		case SEQ_D_SPIN1L:
		case SEQ_D_SPIN1R:
		case SEQ_D_SPIN2L:
		case SEQ_D_SPIN2R:
		case SEQ_D_SPIN3L:
		case SEQ_D_SPIN3R:
		case SEQ_D_SPIN4L:
		case SEQ_D_SPIN4R:
		case SEQ_D_SPIN5L:
		case SEQ_D_SPIN5R:
			snd_scall_bank(anc_misc_bnk, juke_spin_sp[randrng(sizeof(juke_spin_sp)/sizeof(int))], ANCR_VOL2, 127, LVL2);
			break;
		case SEQ_D_OVR1_L:
		case SEQ_D_OVR1_R:
		case SEQ_D_OVR3_L:
		case SEQ_D_OVR3_R:
		case SEQ_D_OVR5_L:
		case SEQ_D_OVR5_R:
		case SEQ_D_OVR6_L:
		case SEQ_D_OVR6_R:
		case SEQ_D_OVR7_L:
		case SEQ_D_OVR7_R:
			snd_scall_bank(anc_misc_bnk, juke_cross_sp[randrng(sizeof(juke_cross_sp)/sizeof(int))], ANCR_VOL2, 127, LVL2);
			break;
		case SEQ_D_REV1:
		case SEQ_D_COCK_1:
			snd_scall_bank(anc_misc_bnk, fancy_footwork_sp[randrng(sizeof(fancy_footwork_sp)/sizeof(int))], ANCR_VOL2, 127, LVL2);
			break;
		default:
			snd_scall_bank(anc_misc_bnk, juke_sp[randrng(sizeof(juke_sp)/sizeof(int))], ANCR_VOL2, 127, LVL2);
			break;
	}

	gSndTimer = 75;
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void rebound_speech_proc(int *args)
{
#ifdef DEBUG_NO_ANNOUNCER
	die(0);
#endif
	if (gSndTimer > 35)
		return;

	while(gSndTimer > 0)
		sleep(1);
	
	if ((game_info.ball_mode == BM_DRIBBLING) && (game_info.ball_handler != NO_BALLHANDLER))
	{
		if (randrng(100) < 60)
			snd_scall_bank(plyr_bnks[game_info.ball_handler], LAST_NAME_1_PB, ANCR_VOL2, 127, LVL3);
		else
			snd_scall_bank(anc_misc_bnk, rebound_sp[randrng(sizeof(rebound_sp)/sizeof(int))], ANCR_VOL2, 127, LVL3);
	}
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void replay_speech(void)
{
	int	i;

#ifdef DEBUG_NO_ANNOUNCER
	return;
#endif

	snd_scall_bank(anc_misc_bnk, replay_sp[randrng(sizeof(replay_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);


//	if (COURT_IS_INDOORS)
	if (1)
	{
		i = randrng(100);
		if (i < 6)
			snd_scall_bank(crowd_bnk_str, CROWD_BOO_SND, (randrng(100) < 50) ? VOLUME6 : VOLUME5, 127, PRIORITY_LVL3);
		else if (i < 40)
			snd_scall_bank(crowd_bnk_str, CROWD2_SWELL1_SND, (randrng(100) < 50) ? VOLUME7 : VOLUME5, 127, PRIORITY_LVL3);
//			snd_scall_bank(crowd_bnk_str, CROWD1_LOOP1_SND,VOLUME1, 127, SND_PRI_SET|SND_PRI_TRACK1);
		else if (i < 60)
			snd_scall_bank(crowd_bnk_str, CROWD2_SWELL2_SND, (randrng(100) < 50) ? VOLUME6 : VOLUME5, 127, PRIORITY_LVL3);
		else
			snd_scall_bank(crowd_bnk_str, CROWD2_SWELL3_SND, (randrng(100) < 50) ? VOLUME6 : VOLUME5, 127, PRIORITY_LVL3);
	}



}

//--------------------------------------------------------------------------------------------------------------------------
// this is called from ***** ANIMIMATIONS ***** within PLYRSEQ.c
//--------------------------------------------------------------------------------------------------------------------------
void dunk_ball_speech(obj_3d *pobj, int which_dunk)
{
	int		dunker = ((bbplyr_data *)pobj)->plyrnum;

#ifdef DEBUG_NO_ANNOUNCER
	return;
#endif

	if (dunker != ball_obj.who_shot)
		return;

	// dunk explosion sound
	if (((game_info.fire_cnt == FIRECNT) && (dunker == game_info.fire_player)) ||
		 (game_info.team_fire == player_blocks[dunker].team))
		make_net_on_fire2_snd(PRIORITY_LVL5);
	else if (((game_info.fire_cnt > FIRECNT) && (dunker == game_info.fire_player)) ||
		 (game_info.team_fire == player_blocks[dunker].team))
		make_net_on_fire_snd(PRIORITY_LVL5);
	else
	{
		make_hit_rim_snd(VOLUME7);
		make_hit_rim_snd(VOLUME7);
	}

	if (dunker != ball_obj.who_shot)
		return;

	if (randrng(100) < 50)
		snd_scall_bank(players_bnk_str, short_yell_dunk_spfx_snds[randrng(sizeof(short_yell_dunk_spfx_snds)/sizeof(int))], VOLUME7, 127, PRIORITY_LVL2);

	if (randrng(100) > 50)
//		snd_scall_bank(players_bnk_str, short_yell_dunk_spfx_snds[randrng(sizeof(short_yell_dunk_spfx_snds)/sizeof(int))], VOLUME7, 127, PRIORITY_LVL2);
		snd_scall_bank(players_bnk_str, short_yell_dunk_spfx_snds[randrng(sizeof(short_yell_dunk_spfx_snds)/sizeof(int))], VOLUME7, 127, PRIORITY_LVL3);
	
	// only make sound if the ball is held by this dude... really here for alley-oops and double dunks
//	if (dunker != game_info.ball_handler)
//		return;

	shot_description = SD_DUNK;
}


//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void dunk_take_fx(float dist)
{
	int spch;

#ifdef DEBUG_NO_ANNOUNCER
	return;
#endif


// FIX!!!  Check here to see if it the horse taking this dunk
// if so, go to alternate spfx table
//	if (((dist > 40.0f) && (randrng(100) < 8)) || (gSndTimer))
	if (dist > 35.0f)
	{
//		if (randrng(100) > 10)
			snd_scall_bank(gameplay_bnk_str, far_dunk_spfx_snds[randrng(sizeof(far_dunk_spfx_snds)/sizeof(int))], VOLUME6, 127, PRIORITY_LVL3);
//		else
//			snd_scall_bank(players_bnk_str, yell_dunk_spfx_snds[randrng(sizeof(yell_dunk_spfx_snds)/sizeof(int))], VOLUME5, 127, PRIORITY_LVL3);
	}
	else
		if (randrng(100) > 70)
			snd_scall_bank(players_bnk_str, short_yell_dunk_spfx_snds[randrng(sizeof(short_yell_dunk_spfx_snds)/sizeof(int))], VOLUME6, 127, PRIORITY_LVL1);

	spch = randrng(sizeof(dunk_take_sp)/sizeof(int));
	snd_scall_bank(anc_take_bnk, dunk_take_sp[spch], ANCR_VOL2, 127, LVL2);
	gSndTimer = (int)sca_take_sp_lengths[dunk_take_sp[spch]];
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void layup_ball_speech(obj_3d *pobj, int kind)
{
	int		layuper = ((bbplyr_data *)pobj)->plyrnum;


	shot_description = SD_LAYUP;

	snd_scall_bank(anc_take_bnk, layup_take_sp[randrng(sizeof(layup_take_sp)/sizeof(int))], ANCR_VOL2, 127, LVL2);
}

//--------------------------------------------------------------------------------------------------------------------------
// this is called from ***** ANIMIMATIONS ***** within PLYRSEQ.c
//--------------------------------------------------------------------------------------------------------------------------
void put_back_ball_speech(obj_3d *pobj, int not_used)
{
	int		dunker = ((bbplyr_data *)pobj)->plyrnum;

#ifdef DEBUG_NO_ANNOUNCER
	return;
#endif
	if (game_info.ball_handler == NO_BALLHANDLER)
		return;

	// dunk explosion sound
	if (((game_info.fire_cnt == FIRECNT) && (dunker == game_info.fire_player)) ||
		 (game_info.team_fire == player_blocks[dunker].team))
		make_net_on_fire2_snd(PRIORITY_LVL5);
	else if (((game_info.fire_cnt > FIRECNT) && (dunker == game_info.fire_player)) ||
		 (game_info.team_fire == player_blocks[dunker].team))
		make_net_on_fire_snd(PRIORITY_LVL5);
	else
	{
		make_hit_rim_snd(VOLUME7);
		make_hit_rim_snd(VOLUME7);
	}

	snd_scall_bank(anc_make_bnk, put_back_make_sp[randrng(sizeof(put_back_make_sp)/sizeof(int))], VOLUME7, 127, LVL4);

//	if (game_info.ball_handler != dunker)
//		return;

	if (randrng(100) > 50)
		snd_scall_bank(players_bnk_str, short_yell_dunk_spfx_snds[randrng(sizeof(short_yell_dunk_spfx_snds)/sizeof(int))], VOLUME7, 127, PRIORITY_LVL1);

}

//--------------------------------------------------------------------------------------------------------------------------
// this is called from ***** ANIMIMATIONS ***** within PLYRSEQ.c
//--------------------------------------------------------------------------------------------------------------------------
void pump_fake_speech(obj_3d *pobj, int not_used)
{
 	snd_scall_bank(anc_misc_bnk, pump_fake_sp[randrng(sizeof(pump_fake_sp)/sizeof(int))], ANCR_VOL2, 127, LVL2);
	gSndTimer = 65;
}

//--------------------------------------------------------------------------------------------------------------------------
// this is called from ***** ANIMIMATIONS ***** within PLYRSEQ.c
//--------------------------------------------------------------------------------------------------------------------------
void shoot_ball_speech(obj_3d *pobj, int not_used)
{
	int		shooter = ((bbplyr_data *)pobj)->plyrnum;

#ifdef DEBUG_NO_ANNOUNCER
	return;
#endif

	qcreate("sht_sp", SPEECH_PID, shoot_ball_speech_proc, shooter, 0, 0, 0);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void shoot_ball_speech_proc(int *args)	
{
	int shooter = args[0];
	float	ftlx,pbtz,pbbz,blx,keyx;
	float	dist_to_rim, rim_pt[3];
	float	px,pz;
	int		spch = 0;
	obj_3d * pobj = &player_blocks[shooter].odata;
	int		cp = nearest_to_me(shooter, !player_blocks[shooter].team);

	rim_pt[0] = game_info.active_goal == 1 ? RIM_X : -RIM_X; rim_pt[1] = 0.0f; rim_pt[2] = 0.0f;
	dist_to_rim = fsqrt( ((pobj->x - rim_pt[0]) * (pobj->x - rim_pt[0]))  + ((pobj->z - rim_pt[2]) * (pobj->z - rim_pt[2])) );

	// if on fire... make special sound
	if (((game_info.fire_cnt >= FIRECNT) && (shooter == game_info.fire_player)) ||
		 (game_info.team_fire == player_blocks[shooter].team))
	{
		if (dist_to_rim < 40.0f)
			snd_scall_bank(gameplay_bnk_str, ON_FIRE_SHORT_SHOT_SND, VOLUME7, 127, PRIORITY_LVL5);
		else
			snd_scall_bank(gameplay_bnk_str, ON_FIRE_SHOT_SND, VOLUME7, 127, PRIORITY_LVL5);
			//snd_scall_bank(gameplay_bnk_str, ON_FIRE_SHOT_SND, VOLUME3, 127, PRIORITY_LVL3);
	}

	// HOTSPOT speech!
	if (plyr_launched_from_hotspot(shooter) == YES)
	{
		if (game_info.hotspot_made_cnt[shooter] >= HOTSPOT_ACTIVE_CNT)
		{
			sleep(10);
			snd_scall_bank(gameplay_bnk_str, ON_FIRE_SHOT_SND, VOLUME7, 127, PRIORITY_LVL3);
			die(0);
		}
	}
//		snd_scall_bank(anc_make_bnk, hotspot_take_sp[randrng(sizeof(hotspot_take_sp)/sizeof(int))], ANCR_VOL2, 127, LVL3);
//		if (game_info.hotspot_made_cnt[shooter] >= HOTSPOT_ACTIVE_CNT)
//		{
//			sleep(10);
//			snd_scall_bank(gameplay_bnk_str, ON_FIRE_SHOT_SND, VOLUME7, 127, PRIORITY_LVL3);
//		}
//		die(0);
//	}

	// last second shot in 4th/overtime quarter ?
	if (game_info.game_time < 0x00000400)
	{
#if DEBUG
	printf("last second SHOOT speech\n");
#endif
		spch = randrng(sizeof(last_second_take_sp)/sizeof(int));
		snd_scall_bank(anc_take_bnk, last_second_take_sp[spch], ANCR_VOL2, 127, LVL4);
		gSndTimer = 0;
		die(0);
	}

	// delay a little... to say speech
	if (gSndTimer < 20)
	{
		while(gSndTimer > 0)
			sleep(1);
	}

	blx = COURT_ORGL-13.0f;						// base line X  (based on ACTIVE_GOAL == 1)
	ftlx = COURT_ORGL-64.0f;					// free throw line x
	keyx = COURT_ORGL-83.0f;					// top of key X
	pbtz = 0.0f - 21.0f;								// paint box TOP Z
	pbbz = 0.0f + 21.0f;								// paint box BOTTOM Z
	px = ABS(pobj->x);
	pz = ABS(pobj->z);

	// same shooter shooting as last shot ?
	same_shooter = 0;		// default to NO
	if (last_shooter != shooter)
		last_shooter = shooter;
	else
		same_shooter = 1;	// YEP

	// default to this case
	shot_description = -1;

	// are we taking a TWO POINT shot ?
	if (what_type_shot == JUMPER_2)
	{
		shot_description = SD_2PT_JUMPER;
		
		// baseline ?
		if (px > blx)
//		if (((game_info.active_goal == 0) && (px <= blx)) ||
//			((game_info.active_goal == 1) && (px >= blx)))
		{
			spch = randrng(sizeof(baseline_take_sp)/sizeof(int));
			snd_scall_bank(anc_take_bnk, baseline_take_sp[spch], ANCR_VOL2, 127, LVL2);
			gSndTimer = (int)sca_take_sp_lengths[baseline_take_sp[spch]];
		}

		// hook shot ?
		else if(pobj->adata[0].seq_index >= (SEQ_J_HOOK_R) && pobj->adata[0].seq_index <= (SEQ_J_HOK3_L))
		{
			shot_description = SD_HOOK_SHOT;
			spch = randrng(sizeof(hook_shot_take_sp)/sizeof(int));
			snd_scall_bank(anc_take_bnk, hook_shot_take_sp[spch], ANCR_VOL2, 127, LVL2);
			gSndTimer = (int)sca_take_sp_lengths[hook_shot_take_sp[spch]];
		}

		// in the paint (short range) ?
		else if (((px > ftlx) && (pz <= pbtz) && (pz <= pbbz)) && !(pobj->flags & PF_LAYUP))
//		else if ((((game_info.active_goal == 0) && (px <= ftlx) && (pz <= pbz) && (pz >= -pbz)) ||
//				 ((game_info.active_goal == 1) && (px <= ftlx) && (pz <= pbz) && (pz >= -pbz))) &&
//				!(pobj->flags & PF_LAYUP))
		{
			spch = randrng(sizeof(in_paint_take_sp)/sizeof(int));
			snd_scall_bank(anc_take_bnk, in_paint_take_sp[spch], ANCR_VOL2, 127, LVL2);
			gSndTimer = (int)sca_take_sp_lengths[in_paint_take_sp[spch]];
		}

		// top of key ?
		else if ((px <= keyx) && (pz <= pbtz) && (pz <= pbbz))
//		else if (((game_info.active_goal == 0) && (pobj->x >= -keyx) && (pobj->z <= pbz) && (pobj->z >= -pbz)) ||
//				 ((game_info.active_goal == 1) && (pobj->x <= keyx) && (pobj->z <= pbz) && (pobj->z >= -pbz)))
		{
			spch = randrng(sizeof(top_of_key_sp)/sizeof(int));
			snd_scall_bank(anc_take_bnk, top_of_key_sp[spch], ANCR_VOL2, 127, LVL2);
			gSndTimer = (int)sca_take_sp_lengths[top_of_key_sp[spch]];
		}

		else if(pobj->adata[0].seq_index >= (SEQ_JSHT1F_R) && pobj->adata[0].seq_index <= (SEQ_JSHT1F_L))
		{	// shoots a fade away
			shot_description = SD_FADE_AWAY;
			if ((randrng(100) < 75) || (gWaitSayName))
			{
				spch = randrng(sizeof(fade_away_take_sp)/sizeof(int));
				snd_scall_bank(anc_take_bnk, fade_away_take_sp[spch], ANCR_VOL2, 127, LVL2);
				gSndTimer = (int)sca_take_sp_lengths[fade_away_take_sp[spch]];
			}
			else
			{
				snd_scall_bank(plyr_bnks[shooter], LAST_NAME_2_PB, ANCR_VOL2, 127, LVL2);
				gSndTimer = 0;
				gWaitSayName = NAME_DELAY;
			}
		}
		else if(pobj->adata[0].seq_index >= (SEQ_J_RUN1_R) && pobj->adata[0].seq_index <= (SEQ_J_RUN1_L))
		{	// shoots a running one handed leaner
			shot_description = SD_1_HAND_LEAN;
			spch = randrng(sizeof(running_leaner_take_sp)/sizeof(int));
			snd_scall_bank(anc_take_bnk, running_leaner_take_sp[spch], ANCR_VOL2, 127, LVL2);
			gSndTimer = (int)sca_take_sp_lengths[running_leaner_take_sp[spch]];

		}
		else
		{
			if((dist_to_rim >= 55.0f) && (randrng(10) < 7))
			{
				spch = randrng(sizeof(jumper2_far_take_sp)/sizeof(int));
				snd_scall_bank(anc_take_bnk, jumper2_far_take_sp[spch], ANCR_VOL2, 127, LVL2);
				gSndTimer = (int)sca_take_sp_lengths[jumper2_far_take_sp[spch]];
			}
			if ((randrng(100) < 75) || (gWaitSayName))
			{
				spch = randrng(sizeof(jumper2_take_sp)/sizeof(int));
				snd_scall_bank(anc_take_bnk, jumper2_take_sp[spch], ANCR_VOL2, 127, LVL2);
				gSndTimer = (int)sca_take_sp_lengths[jumper2_take_sp[spch]];
			}
			else
			{
				snd_scall_bank(plyr_bnks[shooter], LAST_NAME_2_PB, ANCR_VOL2, 127, LVL2);
				gSndTimer = 0;
				gWaitSayName = NAME_DELAY;
			}
		}
	}
	// are we taking a THREE POINT shot ?
	else if(what_type_shot == JUMPER_3)
	{
		shot_description = SD_3PT_JUMPER;

		// desperation shot ?
		if (plyr_crossed_halfcourt(shooter) == NO)
		{	// player is beyond half-court
			shot_description = SD_DESPERATION;
			spch = randrng(sizeof(desperation_take_sp)/sizeof(int));
			snd_scall_bank(anc_take_bnk, desperation_take_sp[spch], ANCR_VOL2, 127, LVL2);
			gSndTimer = (int)sca_take_sp_lengths[desperation_take_sp[spch]];
		}
		// wide open ?
		else if	((distxz[shooter][cp] >= FT(9.0f) && (randrng(100) < 40)))
		{
			spch = randrng(sizeof(wide_open3_take_sp)/sizeof(int));
			snd_scall_bank(anc_take_bnk, wide_open3_take_sp[spch], ANCR_VOL2, 127, LVL2);
			gSndTimer = (int)sca_take_sp_lengths[wide_open3_take_sp[spch]];
		}
		// hand in face ?
		else if ((distxz[shooter][cp] <= FT(3.0f)) && (player_blocks[cp].odata.flags & (PF_IN_AIR_BLOCK|PF_SWATING)))
		{
			spch = randrng(sizeof(hand_in_face_take_sp)/sizeof(int));
			snd_scall_bank(anc_take_bnk, hand_in_face_take_sp[spch], ANCR_VOL2, 127, LVL2);
			gSndTimer = (int)sca_take_sp_lengths[hand_in_face_take_sp[spch]];
		}
		// fade away ?
		else if (pobj->adata[0].seq_index >= (SEQ_JSHT1F_R) && pobj->adata[0].seq_index <= (SEQ_JSHT1F_L))
		{
			shot_description = SD_3PT_FADE_AWAY;
			if ((randrng(100) < 75) || (gWaitSayName))
			{
				spch = randrng(sizeof(fade_away_take_sp)/sizeof(int));
				snd_scall_bank(anc_take_bnk, fade_away_take_sp[spch], ANCR_VOL2, 127, LVL2);
				gSndTimer = (int)sca_take_sp_lengths[fade_away_take_sp[spch]];
			}
			else
			{
				snd_scall_bank(plyr_bnks[shooter], LAST_NAME_2_PB, ANCR_VOL2, 127, LVL2);
				gSndTimer = 0;
				gWaitSayName = NAME_DELAY;
			}
		}
		// repeat shooter ?
		else if (same_shooter)
		{
			spch = randrng(sizeof(again_take_sp)/sizeof(int));
			snd_scall_bank(anc_take_bnk, again_take_sp[spch], ANCR_VOL2, 127, LVL2);
			gSndTimer = (int)sca_take_sp_lengths[again_take_sp[spch]];
		}
		// regular 3 point shot
		else
		{
			if ((randrng(100) < 75) || (gWaitSayName))
			{
				spch = randrng(sizeof(jumper3_take_sp)/sizeof(int));
				snd_scall_bank(anc_take_bnk, jumper3_take_sp[spch], ANCR_VOL2, 127, LVL2);
				gSndTimer = (int)sca_take_sp_lengths[jumper3_take_sp[spch]];
			}
			else
			{
				snd_scall_bank(plyr_bnks[shooter], LAST_NAME_2_PB, ANCR_VOL2, 127, LVL2);
				gSndTimer = 0;
				gWaitSayName = NAME_DELAY;
			}
		}	
	}

	if (shot_description == -1)
		shot_description = SD_2PT_JUMPER;				// shouldnt have gotton here... just say 2pt speech
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void freethrow_made_speech_proc(int *args)
{
	int scorer = player_blocks[args[0]].plyrnum;

	snd_scall_bank(anc_make_bnk, freethrow_make_sp[randrng(sizeof(freethrow_make_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);

	// make net-fire sound
	if (((game_info.fire_cnt == FIRECNT) && (scorer == game_info.fire_player)) ||
		 (game_info.team_fire == player_blocks[scorer].team))
		make_net_on_fire2_snd(PRIORITY_LVL5);
	else if (((game_info.fire_cnt > FIRECNT) && (scorer == game_info.fire_player)) ||
		 (game_info.team_fire == player_blocks[scorer].team))
		make_net_on_fire_snd(PRIORITY_LVL5);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void shot_made_speech_proc(int *args)
{
	int scorer = player_blocks[args[0]].plyrnum;
	obj_3d * pobj = &player_blocks[args[0]].odata;
	int	off_scr, def_scr, scr_diff, rnd;
	int	was_fire = args[1];
	int	tm_scored = args[2];

#ifdef DEBUG_NO_ANNOUNCER
	die(0);
#endif

	off_scr = compute_score(tm_scored);
	def_scr = compute_score(!tm_scored);
	scr_diff = abs(off_scr - def_scr);

	// random crowd swell
//	if (COURT_IS_INDOORS)
	if (1)
	{
		rnd = randrng(100);
		if (rnd < 6)
			snd_scall_bank(crowd_bnk_str, CROWD_BOO_SND, (randrng(100) < 50) ? VOLUME6 : VOLUME4, 127, PRIORITY_LVL3);
		else if (rnd < 40)
			snd_scall_bank(crowd_bnk_str, CROWD2_SWELL1_SND, (randrng(100) < 50) ? VOLUME6 : VOLUME4, 127, PRIORITY_LVL3);
		else if (rnd < 80)
			snd_scall_bank(crowd_bnk_str, CROWD2_SWELL2_SND, (randrng(100) < 50) ? VOLUME6 : VOLUME4, 127, PRIORITY_LVL3);
		else
			snd_scall_bank(crowd_bnk_str, CROWD2_SWELL3_SND, (randrng(100) < 50) ? VOLUME6 : VOLUME4, 127, PRIORITY_LVL3);
	}

	// make net-fire sound
	if (((game_info.fire_cnt == FIRECNT) && (scorer == game_info.fire_player)) ||
		 (game_info.team_fire == player_blocks[scorer].team))
		make_net_on_fire2_snd(PRIORITY_LVL5);
	else if (((game_info.fire_cnt > FIRECNT) && (scorer == game_info.fire_player)) ||
		 (game_info.team_fire == player_blocks[scorer].team))
		make_net_on_fire_snd(PRIORITY_LVL5);

	// last second shot made in 4th/overtime quarter ?
	if (game_info.game_time < 0x00000100)
	{
		if ((game_info.game_quarter == 1) || (game_info.game_quarter >= 3))
//		if (((game_info.game_quarter == 1) || (game_info.game_quarter >= 3)) && ((off_scr > def_scr) && (scr_diff <= 3)))
			snd_scall_bank(anc_make_bnk, last_second_make_sp[randrng(sizeof(last_second_make_sp)/sizeof(int))], VOLUME7, 127, LVL4);
		else
			snd_scall_bank(anc_make_bnk, 24, VOLUME7, 127, LVL4);

		gSndTimer = 0;
		die(0);
	}

	while (gSndTimer > 0)
		sleep(1);

	gSndTimer = 0;

	if (was_fire)
	{
		snd_scall_bank(anc_make_bnk, put_fire_out_sp[randrng(sizeof(put_fire_out_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
		die(0);
	}

	// last 15 seconds of 4th/overtime quarter and just took lead ?
	if ((game_info.game_time < 0x00000f00) &&
		(game_info.game_quarter >= 3) &&
		(off_scr >= def_scr) &&
		(scr_diff <= 3))
	{
		snd_scall_bank(anc_make_bnk, make_and_take_lead_sp[randrng(sizeof(make_and_take_lead_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
		die(0);
	}

	// 50% of time in 4th qtr/overtime, less than 30 seconds, takes the lead/ties game ?
	if ((randrng(100) < 50) &&
		(game_info.game_time < 0x00001e00) &&
		(game_info.game_quarter >= 3) &&
		(off_scr >= def_scr) &&
		(scr_diff <= 3))
	{
		snd_scall_bank(anc_make_bnk, jumper3_4th_qtr_make_sp[randrng(sizeof(jumper3_4th_qtr_make_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
		die(0);
	}

	// put fire out ?
//	if ((game_info.fire_player == scorer) && (game_info.fire_cnt == 0))
//	{
//		snd_scall_bank(anc_make_bnk, put_fire_out_sp[randrng(sizeof(put_fire_out_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
//		die(0);
//	}

	// game tied with this shot ?
	if ((randrng(100) < 35) && (off_scr > 20) && (scr_diff == 0) && (shot_description == SD_3PT_JUMPER))
		snd_scall_bank(anc_make_bnk, 95, ANCR_VOL2, 127, LVL4);		// "ties it with a 3"

	// blowout speech ?
	if ((randrng(100) < 80) &&						// 80% of time
		(game_info.game_quarter >= 3) &&			// 4th or overtime
		(game_info.game_time < 0x00003c00) &&		// under 1 minute
		(off_scr > def_scr) &&
		(scr_diff >= 8))
	{
		snd_scall_bank(anc_misc_bnk, blowout_sp[randrng(sizeof(blowout_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
		die(0);
	}
	
	// HOTSPOT speech!
	if (args[3])
//	if (plyr_launched_from_hotspot(scorer) == YES)
	{
		if (game_info.hotspot_made_cnt[scorer] == HOTSPOT_ACTIVE_CNT)
		{
			snd_scall_bank(anc_make_bnk, 185, ANCR_VOL2, 127, LVL4);
			die(0);
		}
		if (game_info.hotspot_made_cnt[scorer] > HOTSPOT_ACTIVE_CNT)
		{
			snd_scall_bank(anc_make_bnk, hotspot_make_sp[randrng(sizeof(hotspot_make_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
			die(0);
		}
	}

	// heating up speech ?
	if (game_info.fire_cnt == FIRECNT-1)
	{
		snd_scall_bank(anc_make_bnk, heating_up_sp[randrng(sizeof(heating_up_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
		die(0);
	}

	// get on-fire
	else if ((game_info.fire_cnt == FIRECNT) && (scorer == game_info.fire_player))
	{
		snd_scall_bank(anc_make_bnk, get_on_fire_sp[randrng(sizeof(get_on_fire_sp)/sizeof(int))], VOLUME7, 127, LVL4);
		die(0);
	}

	// already on fire
	else if ((game_info.fire_cnt >= FIRECNT) && (scorer == game_info.fire_player))
	{
		snd_scall_bank(anc_make_bnk, on_fire_make_sp[randrng(sizeof(on_fire_make_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
		die(0);
	}
	else if (game_info.tm_fire_cnt[player_blocks[scorer].team] > TM_FIRECNT)
	{
		snd_scall_bank(anc_make_bnk, tm_fire_make_sp[randrng(sizeof(tm_fire_make_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
		die(0);
	}

	// announce players name ?
	if(randrng(100) < 15)
		snd_scall_bank(plyr_bnks[scorer], ((randrng(100) < 50) ? LAST_NAME_3_PB : FULL_NAME_PB), ANCR_VOL2, 127, LVL4);
	// dunk made ?

	else if (pobj->flags & PF_ALLYDUNKER)
	{
		// allyoop made ?
		if (game_info.tm_fire_cnt[player_blocks[scorer].team] == TM_FIRECNT-1)
			snd_scall_bank(anc_make_bnk, tm_heating_up_sp[randrng(sizeof(tm_heating_up_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
		else if (game_info.tm_fire_cnt[player_blocks[scorer].team] == TM_FIRECNT)
			snd_scall_bank(anc_make_bnk, get_on_tm_fire_sp[randrng(sizeof(get_on_tm_fire_sp)/sizeof(int))], VOLUME7, 127, LVL4);
		else if (randrng(100) < 20)
			snd_scall_bank(plyr_bnks[args[0]], pass_to_sp[TO_NAME_PB], ANCR_VOL2, 127, LVL4);
		else
			snd_scall_bank(anc_make_bnk, allyoop_make_sp[randrng(sizeof(allyoop_make_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);

		if (pobj->flags & PF_ONSCREEN)
		{
			if (game_info.team_fire == player_blocks[scorer].team)
				make_net_on_fire_snd(PRIORITY_LVL5);
			else
			{
				make_hit_rim_snd(VOLUME7);
				make_hit_rim_snd(VOLUME7);
			}
		}
	}
	else if(pobj->flags & PF_DUNKER)
	{
		// dunk explosion sound
		if (game_info.fire_cnt >= FIRECNT)
			make_net_on_fire_snd(PRIORITY_LVL5);
		else
		{
			make_hit_rim_snd(VOLUME7);
			make_hit_rim_snd(VOLUME7);
		}
			
//		if(randrng(100) < 30)
//			if(randrng(100) < 30)
//				// Long yell
//	   			snd_scall_bank(players_bnk_str, yell_dunk_spfx_snds[randrng(sizeof(yell_dunk_spfx_snds)/sizeof(int))], VOLUME5, 127, PRIORITY_LVL2);
//			else
//				// Short yell
//				snd_scall_bank(players_bnk_str, short_yell_dunk_spfx_snds[randrng(sizeof(short_yell_dunk_spfx_snds)/sizeof(int))], VOLUME7, 127, PRIORITY_LVL2);
			
		if(randrng(100) < 80)
		{
			// did player dunk on somebody ?
			//if(randrng(100) < -25) // temp: randrng()...if(slammed_on == YES)
			//	snd_scall_bank(anc_make_bnk, dunked_on_sp[randrng(sizeof(dunked_on_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
			made_dunk_sp_num++;
			if (made_dunk_sp_num > (sizeof(dunk_make_sp)/sizeof(int))-1)
				made_dunk_sp_num = 0;
			snd_scall_bank(anc_make_bnk, dunk_make_sp[made_dunk_sp_num], ANCR_VOL2, 127, LVL3);
		}
		else
			snd_scall_bank(plyr_bnks[scorer], randrng(100) < 25 ? LAST_NAME_3_PB : FULL_NAME_PB, ANCR_VOL2, 127, LVL4);


//		if (COURT_IS_INDOORS)
		if (1)
		{
			rnd = randrng(100);
			if (rnd < 6)
				snd_scall_bank(crowd_bnk_str, CROWD_BOO_SND, (randrng(100) < 50) ? VOLUME6 : VOLUME5, 127, PRIORITY_LVL3);
			else if (rnd < 40)
				snd_scall_bank(crowd_bnk_str, CROWD2_SWELL1_SND, (randrng(100) < 50) ? VOLUME6 : VOLUME5, 127, PRIORITY_LVL3);
			else if (rnd < 60)
				snd_scall_bank(crowd_bnk_str, CROWD2_SWELL2_SND, (randrng(100) < 50) ? VOLUME6 : VOLUME5, 127, PRIORITY_LVL3);
			else
				snd_scall_bank(crowd_bnk_str, CROWD2_SWELL3_SND, (randrng(100) < 50) ? VOLUME6 : VOLUME5, 127, PRIORITY_LVL3);
		}
	}
	// lots of rim action ?
	else if (gRim_hits >= 10)
		snd_scall_bank(anc_make_bnk, lucky_bounce_make_sp[randrng(sizeof(lucky_bounce_make_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);

	// off backboard ?
	else if ((gBack_board_hits > 0) && (gRim_hits < 4) && (randrng(100) < 50))
		snd_scall_bank(anc_make_bnk, backboard_make_sp[randrng(sizeof(backboard_make_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);

	// made a two point shot ?
	else if(shot_description == SD_2PT_JUMPER)
	{
		if ((gRim_hits == 0) && (gBack_board_hits == 0))
			snd_scall_bank(anc_make_bnk, all_net_make_sp[randrng(sizeof(all_net_make_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
		else
		{
		 	made_2ptr_sp_num++;
		 	if (made_2ptr_sp_num > (sizeof(jumper2_make_sp)/sizeof(int))-1)
		 		made_2ptr_sp_num = 0;
			snd_scall_bank(anc_make_bnk, jumper2_make_sp[made_2ptr_sp_num], ANCR_VOL2, 127, LVL3);
			//snd_scall_bank(anc_make_bnk, jumper2_make_sp[randrng(sizeof(jumper2_make_sp)/sizeof(int))], ANCR_VOL2, 127, LVL3);
		}
	}
	// made a three point desperation shot ?
	else if(shot_description == SD_DESPERATION)
		snd_scall_bank(anc_make_bnk, desperation_make_sp[randrng(sizeof(desperation_make_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
	// made a three pointer ?
	else if(shot_description == SD_3PT_JUMPER)
	{
		if ((gRim_hits == 0) && (gBack_board_hits == 0))
			snd_scall_bank(anc_make_bnk, all_net_make_sp[randrng(sizeof(all_net_make_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
		else
		{
		 	made_3ptr_sp_num++;
		 	if (made_3ptr_sp_num > (sizeof(jumper3_make_sp)/sizeof(int))-1)
		 		made_3ptr_sp_num = 0;
			snd_scall_bank(anc_make_bnk, jumper3_make_sp[made_3ptr_sp_num], ANCR_VOL2, 127, LVL3);
			//snd_scall_bank(anc_make_bnk, jumper3_make_sp[randrng(sizeof(jumper3_make_sp)/sizeof(int))], ANCR_VOL2, 127, LVL3);
		}
	}
	// made 3pt fade away shot ?
	else if(shot_description == SD_3PT_FADE_AWAY)
		snd_scall_bank(anc_make_bnk, fade_away_make_sp[randrng(sizeof(fade_away_make_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
	// made hook shot ?
	else if(shot_description == SD_HOOK_SHOT)
		snd_scall_bank(anc_make_bnk, hook_shot_make_sp[randrng(sizeof(hook_shot_make_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
	// made 2pt fade away shot ?
	else if(shot_description == SD_FADE_AWAY)
		snd_scall_bank(anc_make_bnk, fade_away_make_sp[randrng(sizeof(fade_away_make_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
	// running leaner shot made ?
	else if(shot_description == SD_1_HAND_LEAN)
		snd_scall_bank(anc_make_bnk, running_leaner_make_sp[randrng(sizeof(running_leaner_make_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
	// layup made ?
	else if(shot_description == SD_LAYUP)
		snd_scall_bank(anc_make_bnk, layup_make_sp[randrng(sizeof(layup_make_sp)/sizeof(int))], ANCR_VOL2, 127, LVL4);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void shot_missed_speech_proc(int *args)
{
	int misser = player_blocks[args[0]].plyrnum;
	obj_3d * pobj = &player_blocks[misser].odata;
	int	spch = 0;

#ifdef DEBUG_NO_ANNOUNCER
	die(0);
#endif

	if (game_info.game_time < 0x00000100)
	{
		snd_scall_bank(anc_make_bnk, last_second_miss_sp[randrng(sizeof(last_second_miss_sp)/sizeof(int))], VOLUME7, 127, LVL5);
		gSndTimer = 0;
		die(0);
	}

	// dont wait too long to say something... cuase other action will take place.. ie rebounds
	if (gSndTimer > 60)
		die(0);

	// wait for other speech to finish
	while (gSndTimer > 0)
		sleep(1);

	gSndTimer = 0;


	// miss multiple in a row ?
	if (player_blocks[misser].shots_missed == 3)
	{
		snd_scall_bank(anc_make_bnk, 78, ANCR_VOL2, 127, LVL4);
		gSndTimer = 77;
	}

	// miss allyoop ?
	else if(shot_description == SD_ALLYOOP)
	{
		spch = randrng(sizeof(allyoop_miss_sp)/sizeof(int));
		snd_scall_bank(anc_make_bnk, allyoop_miss_sp[spch], ANCR_VOL2, 127, LVL3);
		gSndTimer = 70;
		//gSndTimer = (int)sca_make_sp_lengths[allyoop_miss_sp[spch]];
	}
	// miss layup ?
	else if ((shot_description == SD_DUNK) && (pobj->flags & PF_LAYUP))
	{
		spch = randrng(sizeof(layup_miss_sp)/sizeof(int));
		snd_scall_bank(anc_make_bnk, layup_miss_sp[spch], ANCR_VOL2, 127, LVL3);
		gSndTimer = 70;
		//gSndTimer = (int)sca_make_sp_lengths[layup_miss_sp[spch]];
	}
	// miss dunk ?
	else if (shot_description == SD_DUNK)
	{
		// dunk doink sounds
		if (game_info.ball_mode != BM_BOUNCE_ON_RIM)
			snd_scall_bank(gameplay_bnk_str,96,VOLUME7,127,PRIORITY_LVL4);

		spch = randrng(sizeof(dunk_miss_sp)/sizeof(int));
		snd_scall_bank(anc_make_bnk, dunk_miss_sp[spch], ANCR_VOL2, 127, LVL3);
		gSndTimer = 70;
		//gSndTimer = (int)sca_make_sp_lengths[dunk_miss_sp[spch]];
	}
	// same shooter miss ?
	else if ((same_shooter) && (randrng(100) < 90))
	{
		spch = randrng(sizeof(again_miss_sp)/sizeof(int));
		snd_scall_bank(anc_make_bnk, again_miss_sp[spch], ANCR_VOL2, 127, LVL3);
		gSndTimer = 70;
		//gSndTimer = (int)sca_make_sp_lengths[again_miss_sp[spch]];
	}
	// lots of rim action and miss ?
	else if (gRim_hits >= 10)
	{
		spch = randrng(sizeof(unlucky_bounce_miss_sp)/sizeof(int));
		snd_scall_bank(anc_make_bnk, unlucky_bounce_miss_sp[spch], ANCR_VOL2, 127, LVL3);
		gSndTimer = 70;
		//gSndTimer = (int)sca_make_sp_lengths[unlucky_bounce_miss_sp[spch]];
	}
	// hit backboard amd miss ?
	else if ((gRim_hits == 0) && (gBack_board_hits > 0))
	{
		spch = randrng(sizeof(backboard_miss_sp)/sizeof(int));
		snd_scall_bank(anc_make_bnk, backboard_miss_sp[spch], ANCR_VOL2, 127, LVL3);
		gSndTimer = 70;
		//gSndTimer = (int)sca_make_sp_lengths[backboard_miss_sp[spch]];
	}
	// 3ptr miss ?
	else if (shot_description == SD_3PT_JUMPER)
	{
		spch = randrng(sizeof(jumper3_miss_sp)/sizeof(int));
		snd_scall_bank(anc_make_bnk, jumper3_miss_sp[spch], ANCR_VOL2, 127, LVL3);
		gSndTimer = 70;
		//gSndTimer = (int)sca_make_sp_lengths[jumper3_miss_sp[spch]];
	}
	// 2ptr miss ?
	else if (shot_description == SD_2PT_JUMPER)
	{
		spch = randrng(sizeof(jumper2_miss_sp)/sizeof(int));
		snd_scall_bank(anc_make_bnk, jumper2_miss_sp[spch], ANCR_VOL2, 127, LVL3);
		gSndTimer = 70;
		//gSndTimer = (int)sca_make_sp_lengths[jumper2_miss_sp[spch]];
	}
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void get_hit_snd(obj_3d *pobj, int snd1, int snd2, int snd3)
{
	if(snd1 < 0)
	{
		snd1 = randrng(sizeof(plyr_hit_snds)/sizeof(int));
		snd_scall_bank(players_bnk_str,plyr_hit_snds[snd1],VOLUME5,127,PRIORITY_LVL2);
	}
	else if (snd1 > 0)
		snd_scall_bank(players_bnk_str,snd1,VOLUME5,127,PRIORITY_LVL2);

	if (snd2 > 0)
		snd_scall_bank(players_bnk_str,snd2,VOLUME5,127,PRIORITY_LVL2);
	if (snd3 > 0)
		snd_scall_bank(players_bnk_str,snd3,VOLUME5,127,PRIORITY_LVL2);
}

//--------------------------------------------------------------------------------------------------------------------------
//				This ANI_CODE routine plays the sound call specified or if -1 then a random sound
//
// NOTE: 'PLAYERS.BNK' is assumed
//
// INPUT: -1 = play random else sound call number
//--------------------------------------------------------------------------------------------------------------------------
void grnd_hit_snd(obj_3d *pobj, int snd1, int snd2, int snd3)
{
	// get hit sounds
	if (snd1 < 0)
	{
		snd1 = randrng(sizeof(grnd_hit_snds)/sizeof(int));
		snd_scall_bank(gameplay_bnk_str,grnd_hit_snds[snd1],VOLUME4,127,PRIORITY_LVL2);
	}
	else if (snd1 > 0)
		snd_scall_bank(gameplay_bnk_str,snd1,VOLUME4,127,PRIORITY_LVL1);
	// grnd hit speech (grunts)
	if (snd2 < 0)
	{
		snd2 = randrng(sizeof(plr_land_grunt_snds)/sizeof(int) );
		snd_scall_bank(players_bnk_str,plr_land_grunt_snds[snd2],VOLUME3,127,PRIORITY_LVL1);
	}
	else
		snd_scall_bank(players_bnk_str,snd2,VOLUME3,127,PRIORITY_LVL1);
	// nothing right now
	if (snd3 > 0)
		snd_scall_bank(players_bnk_str,snd3,VOLUME4,127,PRIORITY_LVL1);
}

//--------------------------------------------------------------------------------------------------------------------------
//				This ANI_CODE routine plays the sound call specified or if -1 then a random sound
//
// NOTE: 'PLAYERS.BNK' is assumed
//
// INPUT: -1 = play random else sound call number
//--------------------------------------------------------------------------------------------------------------------------
void attack_snd(obj_3d *pobj, int snd1, int snd2, int snd3)
{
	if (!(pobj->flags & PF_ONSCREEN))
		return;

	if (snd1 < 0)
	{
		snd1 = randrng(sizeof(swat_snds)/sizeof(int) );
		snd_scall_bank(gameplay_bnk_str,swat_snds[snd1],VOLUME4,127,PRIORITY_LVL1);
	}
	else if (snd1 > 0)
		snd_scall_bank(gameplay_bnk_str,snd1,VOLUME3,127,PRIORITY_LVL1);
			
	if (snd2 < 0)
	{
		snd2 = randrng(sizeof(plr_attack_snds)/sizeof(int) );
		snd_scall_bank(players_bnk_str,plr_attack_snds[snd2],VOLUME5,127,PRIORITY_LVL2);
	}
	else
		snd_scall_bank(players_bnk_str,snd2,VOLUME4,127,PRIORITY_LVL1);

	if (snd3 > 0)
		snd_scall_bank(gameplay_bnk_str,snd3,VOLUME3,127,PRIORITY_LVL1);
}

//--------------------------------------------------------------------------------------------------------------------------
//				This ANI_CODE routine plays the sound call specified or if -1 then a random sound
//
// NOTE: 'PLAYERS.BNK' is assumed
//
// INPUT: -1 = play random else sound call number
//--------------------------------------------------------------------------------------------------------------------------
void plyr_taunt_speech(obj_3d *pobj, int snd1, int snd2, int snd3)
{
	if(!(pobj->flags & PF_ONSCREEN))
		return;

	if(snd1 < 0 && (randrng(100) < 45))
	{
		if( ((bbplyr_data *)pobj)->p_spd->flags & BLK )
		{
			snd1 = randrng(sizeof(after_score_taunt_rob_sp)/sizeof(int) );
			snd_scall_bank(players_bnk_str,after_score_taunt_rob_sp[snd1],VOLUME5,127,PRIORITY_LVL3);
		}
		else
		{
			snd1 = randrng(sizeof(after_dunk_taunt_sp)/sizeof(int) );
			snd_scall_bank(players_bnk_str,after_dunk_taunt_sp[snd1],VOLUME5,127,PRIORITY_LVL3);
		}
	}
	else if (snd1 > 0)
		snd_scall_bank(players_bnk_str,snd1,VOLUME3,127,PRIORITY_LVL3);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void make_dunk_snd(unsigned int snd)
{
	snd_scall_bank( gameplay_bnk_str, snd, VOLUME7, 127, PRIORITY_LVL4 );
	if (!PLAYBACK) rsnd[RSND_DUNK] = snd;
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void make_net_on_fire_snd(unsigned int lvl)
{
	snd_scall_bank( gameplay_bnk_str, NET_ON_FIRE_SND, VOLUME7, 127, lvl );
	if (!PLAYBACK) rsnd[RSND_NET_ON_FIRE] = lvl;
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void make_net_on_fire2_snd(unsigned int lvl)
{
	snd_scall_bank( gameplay_bnk_str, NET_ON_FIRE_SND, VOLUME7, 127, lvl );
//	snd_scall_bank( gameplay_bnk_str, 135, VOLUME7, 127, lvl );
	if (!PLAYBACK) rsnd[RSND_NET_ON_FIRE] = lvl;
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void make_hit_backboard1_snd(unsigned int vol)
{
	snd_scall_bank( gameplay_bnk_str, HIT_BACKBOARD1_SND, vol, 127, PRIORITY_LVL1 );
	if (!PLAYBACK) rsnd[RSND_HIT_BACKBOARD1] = vol;
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void make_hit_backboard3_snd(unsigned int vol)
{
	snd_scall_bank( gameplay_bnk_str, HIT_BACKBOARD1_SND, vol, 127, PRIORITY_LVL5 );
	if (!PLAYBACK) rsnd[RSND_HIT_BACKBOARD3] = vol;
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void make_hit_rim_snd(unsigned int vol)
{
	snd_scall_bank( gameplay_bnk_str, HIT_RIM2_SND, vol, 127, PRIORITY_LVL3 );
	if (!PLAYBACK) rsnd[RSND_HIT_RIM] = vol;
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void make_net_snd(unsigned int vol)
{
	snd_scall_bank( gameplay_bnk_str, NET1_SND+rndrng0(3), vol, 127, PRIORITY_LVL3 );
	if (!PLAYBACK) rsnd[RSND_NET] = vol;
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void make_dribble1_snd(unsigned int vol)
{
	snd_scall_bank( gameplay_bnk_str, DRIBBLE1_SND, vol, 127, PRIORITY_LVL2 );
	if (!PLAYBACK) rsnd[RSND_DRIBBLE1] = vol;
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void make_dribble2_snd(unsigned int vol)
{
	snd_scall_bank( gameplay_bnk_str, DRIBBLE2_SND, vol, 127, PRIORITY_LVL2 );
	if (!PLAYBACK) rsnd[RSND_DRIBBLE2] = vol;
}

//--------------------------------------------------------------------------------------------------------------------------
