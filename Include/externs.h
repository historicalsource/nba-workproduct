/*
 *		$Archive: /video/Nba/Include/externs.h $
 *		$Revision: 28 $
 *		$Date: 3/29/99 5:26p $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifndef __EXTERNS_H__
#define __EXTERNS_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_externs_h = "$Workfile: externs.h $ $Revision: 28 $";
#endif

// from anim.c
int get_frame( int, int, float *, float * );

// from cambot.c
void set_cambot_theta(float);
void set_cambot_phi(float);
float ValidateTheta(float);
float ValidatePhi(float);
void update_cambot_xform(void);
void cambot_proc(int *);

// from detect.c
void player_detect_proc(int *);

// from Arena.c
void Arena_proc( int * );

// from nba.c
void game_proc( int * );
void wipeout(void);
int compute_score(int);
int check_scores();

// from player.c
extern int halt;

void debug_switch_proc( int * );
void watch_halt_proc( int * );

void draw_player2( void * );
void simplified_player_proc(int *);
void player_proc( int * );
void last_player_proc( int * );
void pre_player_proc( int * );
void load_uniform( int, int, int, int );
void load_referee_txtrs(void);
void load_plyr_txtrs( int, int);
void ball_proc( int * );
void show_hotspot_proc( int * );
void cursor_proc( int * );
void Player_Shadow_Proc( int * );
void reinit_player( int, int );
int ptangle( float *, float * );
void player_shadow_predraw( void * );
void draw_player_shadow( void * );
void yell_signals_proc( int * );
void change_possession(void);
void draw_jersey_numbers( int, const void *, void *, float * );

void make_jump_meter_proc(int *args);
void do_jump_meters_proc(int *args);

// from main.c
void main_load_fonts(void);
void main_init_sound (void);

// from mainproc.c
extern int pcount;

// from replay.c
extern int	recorder;					// recorder on? set with RR_??? <set_recorder_mode>
extern int	playback;					// playback on? set with RP_??? <set_playback_mode>

void replay_proc( int * );
int set_recorder_mode( int );
int set_playback_mode( int, int, int, int );
void * get_replay_obj_ptr( int );
#ifdef DEBUG
#define PLAYBACK	(!!(playback | dbg_playback))
extern int dbg_playback;
void write_replay_data( void );
#else
#define PLAYBACK	(!!(playback))
#endif

// from sounds.c
void make_dunk_snd(unsigned int snd);
void make_net_on_fire_snd(unsigned int snd);
void make_net_on_fire2_snd(unsigned int snd);
void make_hit_backboard1_snd(unsigned int vol);
void make_hit_backboard3_snd(unsigned int vol);
void make_hit_rim_snd(unsigned int vol);
void make_net_snd(unsigned int vol);
void make_dribble1_snd(unsigned int vol);
void make_dribble2_snd(unsigned int vol);

#endif
