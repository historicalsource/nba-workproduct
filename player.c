/*
 *		$Archive: /video/Nba/player.c $
 *		$Revision: 1054 $
 *		$Date: 9/29/99 5:17p $
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
char *ss_player_c = "$Workfile: player.c $ $Revision: 1054 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <goose/goose.h>
#if defined(VEGAS)
#include <glide.h>
#else
#include <glide/glide.h>
#endif

/*
 *		USER INCLUDES
 */

#include "game.h"
#include "player.h"
#include "plyrseq.h"
#include "externs.h"
#include "id.h"
#include "fontid.h"
#include "sndcalls.h"
#include "anim/gen_seq.h"
#include "audits.h"
#include "mainproc.h"
#include "putil.h"
#include "mugshots.h"
#include "movie.h"
#include "transit.h"
#include "nbacmos.h"
#include "select.h"
#include "camdefs.h"

#include "anim/gen_seq.dbg"
#include "ani3d/srmath.h"
#include "ani3d/clip.h"

#if defined(VEGAS)
#define	GR_ASPECT_1x1	GR_ASPECT_LOG2_8x1
#endif

// THINGS THAT WANT TO REMAIN ENABLED IN TEST !!!
#ifdef TEST_VER
	//#define DEBUG_NO_SPEED_LOGIC
	#define DEBUG_ALLOW_BACKBOARD_ALLYS
#endif //TEST_VER


#ifndef RELEASE_VER
 #ifndef TEST_VER
	#define DEBUG_ALLOW_BACKBOARD_ALLYS
//	#define DEBUG_NO_FREETHROWS
//	#define DEBUG_ALLYOOP_DUNKS
//	#define DEBUG_NO_ALLYOOP
//	#define DEBUG_DOUBLE_DUNKS
//	#define DEBUG_SHOT_PERCENTAGE
//	#define DEBUG_NO_SPEED_LOGIC
//	#define DEBUG_HEADS
//	#define DEBUG_EXTREME
//	#define DEBUG_SHAKE_MOVES
//	#define DEBUG_PASSING
//	#define DEBUG_CATCHING
//	#define DEBUG_SWATS
//	#define DEBUG_TIPOFF
//	#define DEBUG_NO_FIRE
//	#define DEBUG_DONT_ALLOW_OOB		// Inbounding ball on/off
//	#define DEBUG_NOHEAD_TURNS
//	#define DEBUG_SHOT_PCT
//	#define ITERATIVE_GEN_MATRICES
//	#define DEBUG_TEST_LAYUPS
//	#define DEBUG_TEST_DUNKS
//	#define DEBUG_NO_TIPOFF
//	#define DEBUG_TEST_FLESH
//	#define DEBUG_DUNKS
//	#define DEBUG_REBOUNDS
//	#define DEBUG_BLOCKS
 #endif //TEST_VER
#endif //RELEASE_VER

//#define	NO_HOTSPOTS

#define YES			1
#define NO			0
#define NOT_READY	0

#define Q8_BEHIND			0
#define Q8_LEFT_BEHIND		1
#define Q8_LEFT				2
#define Q8_LEFT_FRONT		3
#define Q8_FRONT			4
#define Q8_RIGHT_FRONT		5
#define Q8_RIGHT			6
#define Q8_RIGHT_BEHIND		7

#define Q_BEHIND			0x01
#define Q_LEFT_BEHIND		0x02
#define Q_LEFT				0x04
#define Q_LEFT_FRONT		0x08
#define Q_FRONT				0x10
#define Q_RIGHT_FRONT		0x20
#define Q_RIGHT				0x40
#define Q_RIGHT_BEHIND		0x80
#define Q_QUAD_ALL			(Q_BEHIND | Q_LEFT_BEHIND | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT | Q_RIGHT_BEHIND)

#define Q4_BEHIND			0
#define Q4_LEFT				1
#define Q4_FRONT			2
#define Q4_RIGHT			3

#define BIG_BODY 			0
#define NORMAL_BODY 		1

#define	HRES	512

#define COCKY_DRIBBLE_TWAIT 60

#define INBOUND_X	134.0f
#define INBOUND_Z	82.0f

#define R_INHAND_X_OFF	-24.8292f
#define R_INHAND_Y_OFF	114.9530f
#define R_INHAND_Z_OFF	-2.8502f
#define L_INHAND_X_OFF	24.8292f
#define L_INHAND_Y_OFF	114.9530f
#define L_INHAND_Z_OFF	-2.8502f
// this value is identical to the scale value in skeleton.h
#define PSCALE	0.1446258f

// sunlight angle vector
#define SUN_X	0.0f
#define SUN_Y	-3.07f
#define INV_SUN_Y	1.0f/SUN_Y
#define SUN_Z	0.0f

// Misc. pass vels
#define MAX_PASS_TIME		79.0f //75.0f //83.0f //78.0f //75.0f //80.0f //90.0f
#define MIN_PASS_TIME		16.0f //14.0f //17.0f //15.0f //25.0f	// 20
#define MAX_PASS_DIST		280.0f

#define TURBO_BOOST		1.30f	//1.25f	// Used for running faster

#define	DRN_FOOT	3.92f					// Units per foot

//
// global data & function prototypes
//

#ifdef DBG_KO
#define DBG_KO_BOOL	dbg_ko
extern int dbg_ko;
#else
#define DBG_KO_BOOL	0
#endif


float hres_2 = (HRES*0.5f);
float vres_2 = (VRES*0.5f);


//#define GEOM_STATS 1
typedef	struct stats
{
	int		tris_processed;
	int		tris_bbox_processed;
	int		tris_drawn;
	int		verts_processed;
	int		verts_bbox_processed;
	int		verts_transformed;
	int		verts_drawn;
	int		bbox_verts;
	int		bbox_verts_transformed;
	int		tris_clip_processed;
	int		tris_clipped;
	int		strips_processed;
	int		strips_bbox_processed;
	int		strips_drawn;
} stats_t;

extern stats_t		stats;
extern int alt_ladder_active;
extern int cpu_team;

void crowd_stomp_proc(int *args);
void monkey_snds_proc(int *args);
void monkey_snds2_proc(int *args);

void grDrawFanReduced(ReducedVertex *a, int num_verts);

void clip_triangle_t (ReducedVertex *in1, ReducedVertex *in2, ReducedVertex *in3);
void convert_2_jonhey (jonhey_t *jh_obj, LIMB *model, float *matrix, 
					   Texture_node_t **decals);

void opening_msg_proc(int *args);
void hotspot_fire_proc(int *args);
//void tipoff_idiot_box_proc(int *args);
void show_3pt_proc(int *args);
void show_tmfire_proc(int *args);

extern float
	solve_quad( float a, float b, float c )
	;

typedef	struct	_VEC {
	float		x;
	float		y;
	float		z;
} VEC;

extern VEC ballrotv;		// Ball rotation vel axis and magnitude (rots/tick)

extern plyr_record_t  * plyr_stat_ptrs[];

extern int
	randrng(int), debug_num,
	tsec, update_status_box, p_status, show_score_plate, curr_shot_pct, show_shot_pct,
	clock_active, shotclock_active, reset_shotclock,
	players_ready, away_team_style,
	dir2stick( int ), dir2stick2( int , int),
	plyr_crossed_halfcourt(int ),
	intercept_time( int , int , float ),
	_mid_angle( int, int ),
	get_shot_base(bbplyr_data * ),
	get_steal_base(bbplyr_data * ),
	get_swat_base(bbplyr_data * ),
	get_keepball_base(bbplyr_data *ppdata)
	;

extern int which_court;

extern void dump_anim_data2( int ), dump_game_state( int ),
						goaltending_proc(int *args), drone_update( void ),
						Dribbler(obj_3d * ), shoe_squeak(obj_3d *pobj),
						shake_defender_speech(),juke_shoe_squeak(obj_3d *pobj),
						pass_to_speech(obj_3d *pobj, int receiver),
						ball_rotclr(void),
						steal_ball_speech_proc(int *args),
						loose_ball_speech(obj_3d *),
						swated_ball_speech_proc(int *args),
						blocked_shot_speech_proc(int *args),
						rebound_speech_proc(int *args),
						shotclock_violation_proc(int *args),
						game_underway_speech(void),
						goaltending_speech(void),
						intercepted_speech(void),
						deflected_ball_speech(void),
						dunk_take_fx(float),
						free_throw_msg_proc(int *args)
						;

void *select_ptr(void *);

void reinit_player_data(int, int);

void freethrow_meter_proc(int *args);
void do_freethrow(void);
void setup_referee_head(void);
void setup_referee_body(void);
static void airball_proc(int *args);

//extern int	ball_calcstatus(int ticks);
//extern BALLSIMST ballsimstat;
void	goal_shake (int gnum, int mode, float shk);
void	do_shot(bbplyr_data *ppdata);

extern float hotspot_xzs[NUM_HOTSPOTS][2];

extern int four_plr_ver;
extern int hide_peacock;
extern int hide_arrows_one_tick;	// Hide-state for arrows; <update_arrows_proc> unhides every tick
extern int showhide_flag;
extern int ftmtr_per;
extern LIMB *plyr_limbs_rm[];
extern float qpq[60], qjpos[15][3], qmatrices[15][12];
extern float qmat[12], qcm[12], *qtm, anix, aniy, aniz;
extern float dunkfx,dunkfy,dunkfz,dunkoffx,dunkoffy,dunkoffz;
extern char gameplay_bnk_str[], plyr_bnk_str[], crowd_bnk_str[], anc_misc_bnk[];
extern char cursor_bnk_str[];
extern char players_bnk_str[];
extern struct process_node *plist;
extern ARROWINFO arrowinfo[];
extern ReducedVertex		tmp_grvx[1024];
//static ReducedVertex	*tmp_grv = &tmp_grvx[1];
void get_drone_switches( bbplyr_data * );

extern int	gAssist_time;

int pre_tipoff_movie_delay;
int		rndrng0 (int max);

int halt = FALSE;
int haltstep = FALSE;

int no_pump_fake = YES, crowd_cheer_time = 0, drone_attempt_allyoop = 0;
float OOB_XZ[3];
//float bb_shake_l = 0.0f, bb_shake_r = 0.0f
//int rim_rattle_l = 0, rim_rattle_r = 0, shakecnt = 0; 
int OOBPasser = NO_PASSER, dribble_tweens = NO, crowd_chanting = NO;
#if NUM_PERTEAM > 2
int head_wait[6] = {0,0,0,0,0,0}, look[6] = {-3,-3,-3,-3,-3,-3};
#else
int head_wait[4] = {0,0,0,0}, look[4] = {-3,-3,-3,-3};
#endif
char *catch_anim = NULL;
int catch_time = NULL, non_twopart_catch = NO, oob_corrected_catch = NO,
		auto_rebounding = NO, allow_cocky_dribble = YES, ticks_since_cocky = 0,
		cocky_dribbler = -1, allow_cocky_oob = NO, gDouble_dunk = NO, auto_layup = 0;

int	gRim_hits = 0;					// used for speech calls...when miss shot
int gBack_board_hits = 0;		// used for speech calls...when miss shot

void drone_inboundinit (int mode);
void dump_drone_states( void );

void plyr_will_be_at(int *nfwd, float *rx, float *rz, int catch_time, int plyr);
void LookAt(bbplyr_data *, int);	// look in the direction of specified player
void rot_mat(float *, int);
void TurnHead(bbplyr_data *ppdata);
void ShowDebugInfo(bbplyr_data *);
void get_joint_position(obj_3d *pobj, int sequence, int frame, int limb, int dir, float *x, float *y,  float *z);
void get_joint_position_rel(obj_3d *pobj, int sequence, int frame, int limb, int dir, float *x, float *y, float *z, float relx, float rely, float relz);


void do_goaltending_logic(void);
void do_tipoff(void);
void ball_init();
void ball_shoot (obj_3d *o3_p, int mode);
void ball_trail();
void draw_ball (void *oi);
void draw_ball_shadow(void *oi);
void ball_move();
void ball_ingnd();
void lineup_players_for_freethrow(void);
void lineup_players_new_quarter(void);
void lineup_players_for_tipoff(void);
void lineup_players_for_debug(void);

void defensive_steal_logic(bbplyr_data *);
void defensive_swat_logic(bbplyr_data *);
void defensive_block_logic(bbplyr_data *);
void rebound_logic(bbplyr_data *);
int plyr_launched_from_hotspot(int plyr);

int	ball_collide();
int	ball_hit_me(obj_3d *pobj);
void	ball_fireset (int mode, int t, int t2);
void	ball_sparkcreate (int mode, int dur, int cnt);



//
// local data & function prototypes
//
static void ball_fe_trail_proc(int *parg);
static void draw_spfx(void *oi);
static void dump_joint_positions( void );
static void draw_hotspot_cursors( void *oi );
static void draw_cursors( void *oi );
static void draw_referee( void * );
static void draw_player( void * );
static void draw_player_reflection( void *oi );
static void debug_draw_player( void *oi );
static void zerovels( obj_3d * );
static void turn_toward_tgt( obj_3d * );
int in_bounds( obj_3d * );
float dist3d( float * , float * );
float odistxz( obj_3d *, obj_3d * );
//static int plyrangle( obj_3d *, obj_3d * );
//static void set_shoot_ball_vls( obj_3d * );
static void set_ball_vels( obj_3d *, obj_3d *, int );
static void player_pre_draw( void * );
static void player_shadow_init( void );
void player_shadow_predraw( void * );
static void flash_plyr_proc(int *args);
static void flash_plyr_red_proc(int *args);
void ghost_draw_proc(int *args);
void ghost_draw(int *args);
void shadow_player_draw(int );

void draw_player_dummy(void *oi);

//void Prep_Shadows( void );
//void Rend_Shadows( obj_3d *, int );
//void Draw_Shadows( void * );

void SetUpOOBounds(int,int,int);

static void clear_rim_hangs_proc(int *parg);
static void offense_run( obj_3d *pobj );
static void defense_run( obj_3d *pobj );

//static int zcmp( const void *, const void * );
static void dump_anim_data( void );

void pick_random_tipoff_anim(obj_3d *pobj);

static int slap_snd_wait = 0;
static int put_back_already = 0;

int compute_shot_pct(obj_3d *pobj);
int smooth_dir_turn(int , int );
int nearest_to_rim(int );
int nearest_to_me(int , int );
int nearest_to_ball(int );
int nearest_drone_to_ball(int );
int in_between_ball_basket( bbplyr_data *);
int in_between_ball_basket_pow(bbplyr_data *ppdata);
int in_between_me_basket_pow(int me, int him);
int oob_correction(obj_3d *pobj, int *new_dir);

void flash_ball( int );
void draw_player_shadow( void *oi );
//void draw_player_shadow2( void *oi );
//void Dribbler(obj_3d *pobj);

//void Dribbler(obj_3d *pobj);
unsigned int get_some_hands(obj_3d *pobj);
void Get_A_LayUp(obj_3d *pobj);
void Get_A_Dunk(obj_3d *pobj);
void Get_A_Pass(obj_3d *pobj, int receiver, int allyoop);
void Get_A_Double_Dunk(obj_3d * pobj);
void Get_A_AllyOop(obj_3d * pobj);
//void Get_Ally_Pass(obj_3d *pobj);
void SetAllyPassVels(obj_3d *pobj);
void change_possession(void);
//void ChangePassReceiver(void);
void choose_shake_anim(bbplyr_data * );

// player modes
static int _player_standstill(bbplyr_data *ppdata);

int _mode_freethrow(bbplyr_data *ppdata);
int _mode_tipoff(bbplyr_data *ppdata);
static int _mode_offense(bbplyr_data *ppdata);
static int _mode_out_of_bounds(bbplyr_data *ppdata);
static int _mode_defense(bbplyr_data *ppdata);
static int _mode_chill(bbplyr_data *ppdata);
static int WhatKindOfShot( obj_3d * );
int PlayerInAllyZone(obj_3d *);
int RimPhysics(void);
int plyr_bearing( int plyr1, int plyr2 );
int in_the_paint(bbplyr_data * , int );
int three_pt_check(obj_3d *pobj);
int get_angle_to_rim(bbplyr_data * );
float get_dist_to_rim(bbplyr_data * );
void begin_twopart_mode( obj_3d * );
void whistle_proc(int *info);

// ANI_CODE functions
void flash_plyr( obj_3d *pobj, int plyr);
void flash_plyr_red( obj_3d *pobj, int plyr);
void get_backboard_ally( obj_3d *pobj);
int plyr_right_of_me( obj_3d *pobj , int plyr);
int defender_near_me( obj_3d *pobj );
int plyr_behind_backboard( obj_3d *pobj );
int plyr_in_front_of_me( obj_3d *pobj );
void show_shot_percentage(obj_3d *pobj );
int plyr_rand( obj_3d *pobj, int arg );
int referee_wait_timeout(obj_3d * );
void referee_release_ball( obj_3d *pobj, int arg );
void dampen_yvel(obj_3d * pobj, int clear_xz_vels);
int maybe_fadeshot(obj_3d *pobj);
void maybe_throw_elbows(obj_3d *pobj);
int maybe_hookshot_right(obj_3d *pobj);
void maybe_taunt(obj_3d *, int );
void shake_defender(obj_3d *pobj, int way);
void set_ball_hand( obj_3d *pobj, int hand );
int plyr_have_ball(obj_3d * );
int jumpshot_timeout(obj_3d * );
int swat_plyr_button_hit_timeout(obj_3d * );
int plyr_have_ball_hitgnd(obj_3d * );
int receiver_pass_timeout(obj_3d * );
void face_teammate(obj_3d *pobj);
void face_receiver(obj_3d *pobj);
void maybe_do_rim_hang_anim(obj_3d *pobj, int, int, int );
void SetHangOnRim(obj_3d *pobj, int , int , int );
void clear_pumpfake(obj_3d * );
int nopump_fake(obj_3d * );
void SetTipoffVels(obj_3d *,int);
void rotate_to_face_ball(obj_3d *);
void rotate_to_face_rim(obj_3d *);
void face_rim(obj_3d *);
void SetQuickReboundVels(obj_3d *,int,int,int);
void SetReboundVels(obj_3d *,int,int,int);
void SetBlockShotVels( obj_3d *pobj,int,int,int);
void swat_tipoff_ball(obj_3d *);
void pass_ball( obj_3d *pobj, obj_3d *ptgt, int hurl);
void PassRelease(obj_3d * , int, int );
void AllyPassRelease(obj_3d * );
void freethrow_release(obj_3d * );
void Shot_Release(obj_3d * );
void set_unint_lowerhalf (obj_3d *, int );
void set_trgt_fwd( obj_3d *, int, int);
void rotate_anim( obj_3d *, int);
void turn_player( obj_3d *, int);
void turn_player_with_vel( obj_3d *, int );
int is_taunt_done( obj_3d * );
int ball_in_righthand(obj_3d * );
void rotate_to_face_closet(obj_3d *pobj, int angle);
int hand_closet_to_ball(obj_3d *pobj);
int plyr_is_righthanded(obj_3d * );
int maybe_pumpfake(obj_3d *pobj, int but);
int plyr_is_holding_but(obj_3d *pobj, int );
int non_bh_plyr_is_holding_but(obj_3d *pobj, int );
void play_rand_taunt( obj_3d * );
void celebrate( obj_3d *, int );

void reset_x_position( obj_3d * );
void do_printf_in_anim( obj_3d * );
void move_player_in_anim( obj_3d *, int, int, int );

void * rebound_auto(bbplyr_data * pl_p);

#if defined(VEGAS)
void frender_node_mpc(short *script, SNODE *toplimb, LIMB **model, float *matrix[],
		Texture_node_t **decal_array);
#else
void frender_node_mpc(short *script, SNODE *toplimb, LIMB **model, float *matrix[],
		GrMipMapId_t *decal_array);
#endif
extern void fgenerate_matrices(short *script,  SNODE *toplimb, bbplyr_data *ppdata,
						float *parent_matrix, float *quats, float hs, float fs);


#if defined(VEGAS)
 #ifndef DEBUG_PLAYERS
	Texture_node_t	*team_decals[NUM_PLAYERS][NUM_PLYRTEXTURES];
	Texture_node_t	*plyr_decals[NUM_PLAYERS][NUM_PLYRTEXTURES];
 #else
	Texture_node_t	*team_decals[8][NUM_PLYRTEXTURES];
	Texture_node_t	*plyr_decals[8][NUM_PLYRTEXTURES];
 #endif

Texture_node_t	*referee_decals[NUM_PLYRTEXTURES];

#else
GrMipMapId_t team_decals[NUM_PLAYERS][NUM_PLYRTEXTURES];
GrMipMapId_t plyr_decals[NUM_PLAYERS][NUM_PLYRTEXTURES];
#endif
int team_ratios[NUM_PLAYERS][NUM_PLYRTEXTURES];
int referee_ratios[NUM_PLYRTEXTURES];
int referee_active = 0;
int priv_stealth_turbo;

char index_2p[NUM_PLAYERS] = { 0, 2, 1, 3};
char shift_2p[NUM_PLAYERS] = { 1, 4, 2, 8};

char * qtr1_movie[] = { "qtr1" , 0 }; // NBA on NBC - blue
char * qtr2_movie[] = { "qtr2" , 0 }; // NBA on NBC - yellow
char * qtr3_movie[] = { "qtr3" , 0 }; // Blue peacock
char * qtr4_movie[] = { "qtr4" , 0 }; // Blue NBA word

char ** qtr_movies[] = {&qtr1_movie[0], &qtr1_movie[0], &qtr3_movie[0], &qtr4_movie[0], NULL};
char ** foul_movies[] = {&qtr2_movie[0], &qtr2_movie[0], &qtr2_movie[0], &qtr2_movie[0], NULL};


// move direction table
int dir49_table[] = {
		128,  96,  52,   0, 972, 928, 896,
		160, 128,  76,   0, 948, 896, 864,
		204, 180, 128,   0, 896, 844, 820,
		256, 256, 256,  -1, 768, 768, 768,
		308, 332, 384, 512, 640, 692, 716,
		352, 384, 436, 512, 588, 640, 672,
		384, 416, 460, 512, 564, 608, 640
};

// distance table (recomputed every tick)
extern int tick_counter;

extern struct texture_node	*smkpuf_decals[NUM_FIRE_TXTRS];

////////////////

char * cursor_names[] = {
	"pcircl_1.wms", "pcircl_2.wms", "pcircl_3.wms", "pcircl_4.wms"
#if NUM_PERTEAM > 2
	,"pcircl_a.wms", "pcircl_b.wms"//, "pcircl_c.wms"
#endif
};
char * cursor_names_2p[] = {
	"pcircl_1.wms", "pcircr_2.wms", "pcircl_3.wms", "pcircl_4.wms"
#if NUM_PERTEAM > 2
	,"pcircl_a.wms", "pcircl_b.wms"//, "pcircl_c.wms"
#endif
};
//char * shadow_names[] = {
//	"p_shadow.wms"
//};
#define CURSOR_CNT	(sizeof(cursor_names) / sizeof(char *))
//#define SHADOW_CNT	(sizeof shadow_names / sizeof(char *))

#define CURSOR_TEX_S	128
#define CURSOR_TEX_T	128

#if defined(VEGAS)
Texture_node_t	* target_decal = (Texture_node_t *)-1;
Texture_node_t	* ball_decal = (Texture_node_t *)-1;
Texture_node_t	* ballshade_decal = (Texture_node_t *)-1;
Texture_node_t	* ballhigh_decal = (Texture_node_t *)-1;
Texture_node_t	* ball_shadow_decal = (Texture_node_t *)-1;
Texture_node_t	* cursor_decals[CURSOR_CNT];
//Texture_node_t	* hotspot_cursor_decals[CURSOR_CNT];
//Texture_node_t	* shadow_decals[SHADOW_CNT];
#else
GrMipMapId_t	target_decal = -1;
GrMipMapId_t	ball_decal = -1;
GrMipMapId_t	ball_shadow_decal = -1;
GrMipMapId_t	cursor_decals[CURSOR_CNT];
//GrMipMapId_t	shadow_decals[SHADOW_CNT];
#endif

int ball_ratio, ballshade_ratio, ballhigh_ratio;
int ball_shadow_ratio;
int cursor_ratio = GR_ASPECT_1x1;
int player_shadow_ratio = GR_ASPECT_1x1;

////////////////

bbplyr_data	referee_data;

//bbplyr_data	player_blocks[NUM_PLAYERS];
bbplyr_data	player_blocks[8];

ball_data	ball_obj;

float ShotVel = 0.5f, oob_cx, oob_cz;
int gbear = 0.0f, gdir = 0, gangle = 0.0f;
int balltrail = 0, both_hands_on_rim = NO;
int ib_mode = 0;				// inbound mode set by <SetUpOOBounds>
int RimBounceFlavor = SHTMD_RIM, RimBounceFlavorPrev = -1, RimBounces = -1,
	RimBounceAngle = 0, IsShotGood = YES;

// shadow data
static float		fx,fy,fz;		// world origin in camera coords
static float		sx,sy,sz;		// sunlight vector in camera system
static float		nx,ny,nz;		// field normal in camera system (xform middle row)

//
// referenced data
//
extern VIEWCOORD cambot;

extern int ptex_ratios[];

extern SNODE
	bbplyr_skel[], bbplyr_skel_test[],
	tophalf_skel[], tophalf_skel_test[],
	bothalf_skel[], bothalf_skel_test[]
	;

extern LIMB
	limb_fist_r1, limb_fist_l1,
	limb_hand_r1, limb_hand_l1, limb_ohand_r2, limb_ohand_l2,
	limb_fhand_r, limb_fhand_l,
	limb_gndsquare_obj, limb_spfx;

extern ST gndsquare_obj_st[];

extern float trigs[];
extern player_record_info selected_players[];
extern int ignore_steal, ignore_pass, ignore_shoot;
extern int p2p_angles[NUM_PLAYERS][NUM_PLAYERS];
extern float distxz[NUM_PLAYERS][NUM_PLAYERS];
float *qcam2 = cambot.xform;
float gjpos[15][3], gmatrices[15][12];
extern pdata_t pdata_blocks[];

LIMB *hand_sets[] = 
{
	&limb_hand_r1, &limb_hand_l1,
	&limb_fist_r1, &limb_fist_l1,
	&limb_ohand_r2, &limb_ohand_l2,
	&limb_fhand_r, &limb_fhand_l,
};

//--------------------------------------------
// base_speed HAS TO MATCH VALUE IN ANIM.C
// PLAYER RUN SPEED TABLE
//#define base_speed  1.10f
//#define base_speed  1.133f
#define base_speed  1.40f //1.37f //1.30 //1.25f//1.344f //1.120f
#define base_inc_tot   0.08	//0.10f //0.12f	//.15
float speeds[] =
{
	base_speed, base_speed * (1.0f + (base_inc_tot / 19.0f)),
	base_speed * (1.05f + (base_inc_tot / 18.0f)), base_speed * (1.05f + (base_inc_tot / 17.0f)),
	base_speed * (1.05f + (base_inc_tot / 16.0f)), base_speed * (1.05f + (base_inc_tot / 15.0f)),
	base_speed * (1.05f + (base_inc_tot / 14.0f)), base_speed * (1.05f + (base_inc_tot / 13.0f)),
	base_speed * (1.05f + (base_inc_tot / 12.0f)), base_speed * (1.05f + (base_inc_tot / 11.0f)),
	base_speed * (1.05f + (base_inc_tot / 10.0f)), base_speed * (1.05f + (base_inc_tot / 9.0f)),
	base_speed * (1.05f + (base_inc_tot / 8.0f)), base_speed * (1.05f + (base_inc_tot / 7.0f)),
	base_speed * (1.05f + (base_inc_tot / 6.0f)), base_speed * (1.05f + (base_inc_tot / 5.0f)),
	base_speed * (1.05f + (base_inc_tot / 4.0f)), base_speed * (1.05f + (base_inc_tot / 3.0f)),
	base_speed * (1.00f + (base_inc_tot / 2.0f)), base_speed * (1.00f + (base_inc_tot / 1.0f)),
};
//--------------------------------------------

int rob = NO;	// if rob is set to YES the game halts.

int	ally_player, ally_jump_wait;


static char *wait_freethrow_anim_tbl[] =
{
//	s_breath2_anim,
//	s_breath1_anim,
//	s_breath2_anim,
	wait_freethrow_1_anim,
	wait_freethrow_2_anim,
	wait_freethrow_3_anim,
	s_breath3_anim,
};

static char *freethrow_dribbles_anim_tbl[] =
{
	freethrow_dribble_2_anim,
	freethrow_dribble_2_anim
};

static char *freethrow_shoot_anim_tbl[] =
{
	freethrow_shot_2_anim,
	freethrow_shot_2_anim
};


static char *oneh_rim_hang_tbl[] =
{
	rim_hang_1h1_anim,
	rim_hang_1h1_anim
};

static char *twoh_rim_hang_tbl[] =
{
	rim_hang_2h1_anim,
	rim_hang_2h1_anim
//	rim_hang_2h2_anim
};


static char *pre_tip_anims_tbl[] =
{
	pre_tip1_anim,
	pre_tip4_anim,
	pre_tip3_anim,
	pre_tip5_anim,
};

static char *tipoff_ready_anims_tbl[] =
{
	tipoff_ready_1_anim,
	tipoff_ready_2_anim,
};

char *block_anims[] =
{
	block_2h_anim,
	block_2h_2_anim,
	block_2h_2_anim,
	block_2h_3_anim,
	block_1h_anim,
	block_1h_anim,
	swat_1h_1_anim,
	swat_1h_1_anim,
	swat_1h_1_anim
};

// Used by on fire goaltends if team is winning
char *swat_anims[] =
{
	// FIX!!!  this swat seq.
//	swat_1h_2_anim
	swat_2h_2_anim,
	swat_2h_2_anim,
	swat_2h_1_anim,
	swat_1h_1_anim,
	swat_1h_1_anim,
	swat_1h_1_anim,
	swat_1h_1_anim
};

// Used by on fire goaltends if team is losing - keep ball more often
char *swat_fire_anims[] =
{
	// FIX!!!  this swat seq.
//	swat_1h_2_anim
	swat_2h_2_anim,
	swat_2h_2_anim,
	swat_2h_1_anim,
	swat_2h_1_anim,
	swat_1h_1_anim,
	swat_1h_1_anim,
	swat_1h_1_anim,
	swat_1h_1_anim,
	swat_1h_1_anim,
	swat_1h_1_anim,
	block_2h_3_anim,
	block_2h_2_anim,
	block_1h_anim,
	block_2h_anim,
	swat_1h_1_anim
};

char *swat_n_block_anims[] =
{
// Added 4/7
//	swat_1h_2_anim,
//	swat_1h_2_anim,
	block_2h_3_anim,
	block_2h_2_anim,
	swat_2h_2_anim,
	block_1h_anim,
	block_1h_anim,
	block_2h_anim,
	block_1h_anim,
	swat_1h_1_anim,
	swat_1h_1_anim,
	swat_1h_1_anim,
	swat_1h_1_anim,
	swat_1h_1_anim,
	swat_1h_1_anim,
	swat_2h_1_anim,
	swat_2h_1_anim
};

char *quick_jump_block_anims[] =
{
	quick_block_1h_anim,
	quick_block_2h_1_anim,
	quick_block_2h_2_anim,
};

char *push_anims[] =
{
	push_1h_1_anim,
	push_2h_1_anim,
	push_2h_1_anim,
	push_2h_1_anim,
	push_2h_1_anim,
	push_2h_1_anim,
	push_2h_1_anim,
	push_2h_1_anim,
	push_2h_1_anim,
	push_2h_1_anim,
	push_2h_1_anim,
	push_2h_1_anim
};

static char *dunk_layup_anims[] =
{
	dunk_layup_1,
	dunk_layup_2,
	dunk_layup_3
};

static char *breath_anims[] =
{
	//s_breath1_anim_int,
	//s_breath2_anim_int,
	s_breath3_anim_int,
	//s_breath5_anim_int
};

static char *debug_breath_anims[] =
{
	s_breath1_anim,
	//s_breath2_anim,
	s_breath3_anim,
	s_breath5_anim
};

static char *buyin_stance_anims[] =
{
	buyin_stance4_anim,
	buyin_stance2_anim,
	buyin_stance3_anim,
	buyin_stance1_anim,
//	buyin_stance1a_anim,
};

static char *rebound_anims[] =
{
	rebound1_anim,
	rebound1_anim,
	rebound3_anim,
	rebound3_anim,
	rebound6_anim,
	rebound6_anim,
	rebound7_anim,
	rebound7_anim
};

static char *short_rebound_anims[] =
{
	quick_rebound_1_anim,
//	quick_rebound_1_anim,
	low_rebound_1_anim,
	low_rebound_1_anim,
	low_rebound_2_anim,
	quick_rebound_2_anim
//	quick_rebound_2_anim
};

static char *after_dunk_taunts[] =
{
	s_breath3_anim,
	//taunt_1_anim,
	//taunt_2_anim,
	//taunt_3_anim,
};

static char *hook_shot_anims[] =
{
	shot_hook_1,
	shot_hook_2,
	shot_hook_3
};

#if DEBUG
char *quad4_s[] =
{
	"BEHIND",
	"LEFT",
	"FRONT",
	"RIGHT",
};
char *quad8_s[] =
{
	"BEHIND",
	"LEFT.BEHIND",
	"LEFT",
	"LEFT.FRONT",
	"FRONT",
	"RIGHT.FRONT",
	"RIGHT",
	"RIGHT.BEHIND",
};
// <ball_mode_names> exists in PMISC.C & is extern'd here!
#endif

extern int
	pup_show_shotper,
	pup_show_hotspots,
	pup_aba_ball,
	pup_bighead,
	pup_no_hotspots,
	pup_tourney,
	pup_team_jersey,
	pup_home_jersey,
	pup_away_jersey,
	pup_alt_jersey,
	pup_midway_jersey
	;
	
char plyr_bnk0[13], plyr_bnk1[13], plyr_bnk2[13], plyr_bnk3[13];
#if NUM_PERTEAM > 2
char plyr_bnk4[13], plyr_bnk5[13];
#endif

char *plyr_bnks[] = 
{
	&plyr_bnk0[0],&plyr_bnk1[0],&plyr_bnk2[0],&plyr_bnk3[0]
#if NUM_PERTEAM > 2
	,&plyr_bnk4[0],&plyr_bnk5[0]
#endif
};


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void delete_referee(void)
{
	killall(REFEREE_PID,-1);
	delete_multiple_objects(OID_REFEREE,-1);
	// Make referee inactive
	referee_active = 0;
}


//------------------------------------------------------------------------------------------------------------------------------
// referee proc - main player process.
// arg0 = ticks to wait before animation begins
// arg1 = 
// arg2 = 
// arg3 = 
//------------------------------------------------------------------------------------------------------------------------------
#define REF_PRE_Z	1000.0f

void referee_proc(int * parg)
{
	struct object_node * pon;
	bbplyr_data	*ppdata;
	obj_3d		*pobj;
	int cnt = parg[0];

	// setup referee data struct...
	ppdata = &referee_data;
	pobj = (obj_3d *)ppdata;

	/* set up the referee characters static data */
	ppdata->p_spd = &teaminfo[TEAM_SECRET].p_spd[REF_CHARACTER];

	pobj->x        = 0.0f;			// center court
	pobj->y        = 0.0f;			// on floor!
	pobj->z        = REF_PRE_Z;		// Go outside till GM_PRE_TIPOFF is done
	pobj->fwd      = 0;
	pobj->friction = 0.0f;

	ppdata->no_ball_collide   = 0;
	ppdata->defense_anim_wait = 0;

	ppdata->auto_control   = NO;
	ppdata->HangingOnRim   = NO;
	ppdata->stick_cur      = 24;
	ppdata->but_cur        = 0;
	ppdata->dribble_mode   = DM_CAN_DRIBBLE;
	ppdata->attack_mode    = ATTMODE_NONE;
	ppdata->attack_time    = 0;
	ppdata->celebrate_time = 0;
	ppdata->flight_time    = 0;
	ppdata->total_time     = 0;
	ppdata->misc_wait_time = -1;
	ppdata->head_tick_wait = 120;

	// initialize anim data
	pobj->pdecal = &(referee_decals[0]);
	calc_player_scale(FALSE, 0, ppdata->p_spd->tall, ppdata->p_spd->weight, &pobj->zscale, &pobj->yscale);
	pobj->xscale = pobj->zscale;
	pobj->ascale = pobj->yscale;
	
	ppdata->f_headsc      = player_load_info[NUM_PLAYERS].head_scale;
	ppdata->head_model    = player_load_info[NUM_PLAYERS].head_model;
	ppdata->body_model    = player_load_info[NUM_PLAYERS].body_model;
	ppdata->jnum_model    = NULL;
	ppdata->reflect_model = player_load_info[NUM_PLAYERS].reflect_model;

	pobj->adata[0].adata   = ppdata->anidata[0];
	pobj->adata[0].idata   = ppdata->anidata[1];
	pobj->adata[1].adata   = ppdata->anidata[2];
	pobj->adata[1].idata   = ppdata->anidata[3];
	pobj->adata[2].adata   = ppdata->anidata[4];
	pobj->adata[3].adata   = ppdata->anidata[5];
	pobj->adata[1].animode = MODE_SECONDARY;

	// init object & animation
	pon = create_object("referee", OID_REFEREE, OF_NONE, DRAWORDER_PLAYER, (void *)ppdata, draw_referee);
	change_anim(pobj, referee_tipoff_anim);
	// Make referee active but not aniimating
	referee_active = -1;

	// referee logic loop
	while (!(pon->object_flags & OF_HIDDEN))
	{
		sleep(1);

		if (game_info.game_mode != GM_PRE_TIPOFF)
		{
			// Bring him inside if he is outside
			if (pobj->z == REF_PRE_Z)
				pobj->z = -25.0f;

	  		animate_model( pobj );
		}
		else
		{
			// Any GM_PRE_TIPOFF will start the tip-off as soon as
			// it changes to GM_TIPOFF, thus supporting button-out
			cnt = pre_tipoff_movie_delay;
		}
		// wait spec'd ticks before starting
		if (cnt-- == 0.0f)
			// Make referee active
			referee_active = 1;
	}
	delete_referee();
}

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
int referee_wait_timeout(obj_3d * pobj)
{
	return (referee_active > 0);
}

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
void referee_release_ball( obj_3d *pobj, int arg )
{
//	game_info.game_mode = GM_IN_PLAY;			// do this when catch ball!!
	game_info.ball_mode = BM_FREE_ON_COURT;
	game_info.ball_handler = -1;

	ball_obj.odata.vy = 1.95f; //1.85f

	// say "heres the tip..."
	snd_scall_bank(anc_misc_bnk, 271, ANCR_VOL2, 127, LVL2);
}


//------------------------------------------------------------------------------------------------------------------------------
// simplified_player_proc - main player process.
// arg0 = plyrnum
// arg1 = team
// arg2 = field position
// arg3 = who to be (0 to (NUM_PERTEAM_SPD-1)) on which team (tm# * NUM_PERTEAM_SPD)
//------------------------------------------------------------------------------------------------------------------------------
void simplified_player_proc(int *parg)
{
	static float p_x[][MAX_PLYRS] = {	//[2p,4p][plyr#]
		{ 11.0,  0.0, -14.0,   0.0},
		{ 16.8,  7.3,  -6.5, -17.0},
	};
	bbplyr_data	*ppdata;
	int			plyrnum, plyrteam;
	obj_3d		*pobj;
	//char		cbuf[8];

	// extract arguments
	plyrnum = parg[0];
	plyrteam = parg[1];

	ppdata = &player_blocks[plyrnum];
	pobj = (obj_3d *)ppdata;

	// init player data
	pobj->x = p_x[four_plr_ver][plyrnum];


	pobj->y     = -12.0f;
	pobj->z     = 100.0f;
	pobj->fwd   = 512;
	ppdata->dribble_mode      = DM_CAN_DRIBBLE;
	ppdata->no_ball_collide   = 0;
	ppdata->HangingOnRim      = NO;
	ppdata->auto_control      = NO;
	ppdata->defense_anim_wait = 0;
	ppdata->total_time        = 0;
	ppdata->flight_time       = 0;

	ppdata->stick_cur   = 24;
	ppdata->but_cur     = 0;
	pobj->friction      = 0.0f;
	ppdata->attack_mode    = ATTMODE_NONE;
	ppdata->attack_time    = 0;
	ppdata->celebrate_time = 0;
	ppdata->misc_wait_time = -1;

//	do_pup_player_scale(ppdata);

	// initialize anim data
	pobj->adata[0].adata = ppdata->anidata[0];
	pobj->adata[0].idata = ppdata->anidata[1];
	pobj->adata[1].adata = ppdata->anidata[2];
	pobj->adata[1].idata = ppdata->anidata[3];

	pobj->adata[2].adata = ppdata->anidata[4];
	pobj->adata[3].adata = ppdata->anidata[5];

	pobj->adata[1].animode = MODE_SECONDARY;

	// player
	create_object("buy_plr", OID_BUYIN_PLYR, OF_NONE, DRAWORDER_PLAYER, (void *)ppdata, draw_player2);
	// players shadow pre-draw
//	if (plyrnum == 0)
//		create_object( "preshad", OID_PLAYER, OF_NONE, DRAWORDER_PLAYER + 1, NULL, player_shadow_predraw2 );
	// players shadow
//	create_object("plrshdw", OID_BUYIN_PLYR, OF_NONE, DRAWORDER_PLAYER + 2, (void *)ppdata, draw_player_shadow2);

	// init animation
	change_anim(pobj, buyin_stance_anims[plyrnum]);

	// 'simple' player loop
	while( 1 )
	{
		// decrement timer on celebration bit
		if( ppdata->celebrate_time )
		{
			if(( ppdata->celebrate_time -= 1) == 0 )
				pobj->flags &= ~PF_CELEBRATE;
		}

		// animate
  		animate_model( pobj );

		// sleep
		sleep( 1 );
	}
}

//------------------------------------------------------------------------------------------------------------------------------
// simplified_player_proc - main player process.
// arg0 = plyrnum
// arg1 = team
// arg2 = field position
// arg3 = who to be (0 to (NUM_PERTEAM_SPD-1)) on which team (tm# * NUM_PERTEAM_SPD)
//------------------------------------------------------------------------------------------------------------------------------
 #ifdef DEBUG_PLAYERS
void debug_player_proc(int *parg)
{
	bbplyr_data	*ppdata;
	int			plyrnum, plyrteam;
	float lod_bias = -1.25f;
	obj_3d		*pobj;
	char cbuf[20];

	// extract arguments
	plyrnum = parg[0];
	plyrteam = parg[1];

	ppdata = &player_blocks[plyrnum];
	pobj = (obj_3d *)ppdata;

	// Set ptr to player's static data
	ppdata->p_spd    = selected_players[plyrnum].player_data;
	// init player data
	ppdata->plyrnum           = plyrnum;
	ppdata->team              = plyrteam;

	ppdata->f_headsc      = player_load_info[plyrnum].head_scale;
	ppdata->head_model    = player_load_info[plyrnum].head_model;
	ppdata->body_model    = player_load_info[plyrnum].body_model;
	ppdata->jnum_model    = player_load_info[plyrnum].jnum_model;
	ppdata->reflect_model = player_load_info[plyrnum].reflect_model;

	calc_player_scale(FALSE, 0, ppdata->p_spd->tall, ppdata->p_spd->weight, &pobj->zscale, &pobj->yscale);
	pobj->xscale = pobj->zscale;
	pobj->ascale = pobj->yscale;
	ppdata->position = ppdata->p_spd->position;

	// init player data
	pobj->x = 0.0f + (10.0f * plyrnum);

	pobj->y     = 0.0f;
	pobj->z     = 0.0f;
	pobj->fwd   = 0;
	ppdata->dribble_mode      = DM_CAN_DRIBBLE;
	ppdata->no_ball_collide   = 0;
	ppdata->HangingOnRim      = NO;
	ppdata->auto_control      = NO;
	ppdata->defense_anim_wait = 0;
	ppdata->total_time        = 0;
	ppdata->flight_time       = 0;

	ppdata->stick_cur   = 24;
	ppdata->but_cur     = 0;
	pobj->friction      = 0.0f;
	ppdata->attack_mode    = ATTMODE_NONE;
	ppdata->attack_time    = 0;
	ppdata->celebrate_time = 0;
	ppdata->misc_wait_time = -1;

	pobj->pdecal = plyr_decals[plyrnum];
	plyr_decals[plyrnum][0] = team_decals[plyrnum][1];				// flesh		0
	plyr_decals[plyrnum][1] = team_decals[plyrnum][0];				// head			4
	plyr_decals[plyrnum][2] = team_decals[plyrnum][2];				// jersey		8
	plyr_decals[plyrnum][3] = team_decals[plyrnum][3];				// numbers	12
	plyr_decals[plyrnum][4] = team_decals[plyrnum][4];				// reflection	16

	guTexChangeLodBias( plyr_decals[plyrnum][0], lod_bias);
	guTexChangeLodBias( plyr_decals[plyrnum][1], lod_bias);
	guTexChangeLodBias( plyr_decals[plyrnum][2], lod_bias);
	guTexChangeLodBias( plyr_decals[plyrnum][3], lod_bias);
	guTexChangeLodBias( plyr_decals[plyrnum][4], lod_bias);

	if(ppdata->p_spd->flags & RIGHT)
		ppdata->ball_in_hand = ppdata->handedness = RIGHT_HAND;
	else
		ppdata->ball_in_hand = ppdata->handedness = LEFT_HAND;

	// initialize anim data
	pobj->adata[0].adata = ppdata->anidata[0];
	pobj->adata[0].idata = ppdata->anidata[1];
	pobj->adata[1].adata = ppdata->anidata[2];
	pobj->adata[1].idata = ppdata->anidata[3];

	pobj->adata[2].adata = ppdata->anidata[4];
	pobj->adata[3].adata = ppdata->anidata[5];

	pobj->adata[1].animode = MODE_SECONDARY;

	ppdata->head_dir = ppdata->head_tgt_dir = -1; // body dir is also look dir
	ppdata->head_turn_rate = 32;

	// player
	sprintf( cbuf, "plyr%d", plyrnum );
	create_object( cbuf, OID_BUYIN_PLYR, OF_NONE, DRAWORDER_PLAYER, (void *)ppdata, debug_draw_player);
	sprintf( cbuf, "shad%d", plyrnum );
	create_object( cbuf, OID_PLAYER, OF_NONE, DRAWORDER_FIELDFX + 0x20 + plyrnum, (void *)ppdata, draw_player_shadow );

	if( ppdata->plyrnum == 0 )
	{
		create_object( "preplyr", OID_PLAYER, OF_NONE, DRAWORDER_PLAYER-1, NULL, player_pre_draw );
		player_shadow_init();
		create_object( "preshad", OID_PLAYER, OF_NONE, DRAWORDER_FIELDFX + 0x1F, NULL, player_shadow_predraw );
	}

	// init animation
	change_anim(pobj, debug_breath_anims[randrng(sizeof(debug_breath_anims)/sizeof(char *))]);

	// 'simple' player loop
	while( 1 )
	{
		// decrement timer on celebration bit
		if( ppdata->celebrate_time )
		{
			if(( ppdata->celebrate_time -= 1) == 0 )
				pobj->flags &= ~PF_CELEBRATE;
		}

		if(!(pobj->flags & PF_ANITICK ))
			animate_model( pobj );

		// sleep
		sleep( 1 );
	}
}
#endif

//////////////////////////////////////////////////////////////////////////////
// called from buyin_stance_anim's to reset X position at LOOP point
//////////////////////////////////////////////////////////////////////////////
void reset_x_position(obj_3d *pobj)
{
	int		plyrnum;
	bbplyr_data *ppdata = (bbplyr_data *)pobj;

	plyrnum = ppdata->plyrnum;
	// init player data
	if (plyrnum == -1)				/* for creatp */
		pobj->x = -21.0f;
	else if (plyrnum == 0)
		pobj->x = 18.8f;
	else if (plyrnum == 1)
		pobj->x = 7.3f;
	else if (plyrnum == 2)
		pobj->x = -5.5f;
	else
		pobj->x = -17.0f;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------
void reinit_player_data(int plyrnum, int plyrteam)
{
	bbplyr_data	* ppdata;
	pdata_t * ppdb;
	obj_3d * pobj;
	float lod_bias = -1.25f;
	int base;


	ppdata = &player_blocks[plyrnum];
	ppdata->p_spd = selected_players[plyrnum].player_data;
	pobj   = (obj_3d *)ppdata;

	ppdata->f_headsc      = player_load_info[plyrnum].head_scale;
	ppdata->head_model    = player_load_info[plyrnum].head_model;
	ppdata->body_model    = player_load_info[plyrnum].body_model;
	ppdata->jnum_model    = player_load_info[plyrnum].jnum_model;
	ppdata->reflect_model = player_load_info[plyrnum].reflect_model;

	calc_player_scale(FALSE, 0, ppdata->p_spd->tall, ppdata->p_spd->weight, &pobj->zscale, &pobj->yscale);
	pobj->xscale = pobj->zscale;
	pobj->ascale = pobj->yscale;
	ppdata->position = ppdata->p_spd->position;

	if(pup_bighead & (1<<ppdata->plyrnum))
		ppdata->f_headsc *= bighead_scale(selected_players[plyrnum].team, selected_players[plyrnum].player_index);

	// re-fill in player ratings
	ppdata->_shooting  = ppdata->p_spd->shot;
	ppdata->_threeshot = ppdata->p_spd->rebound;
	ppdata->_dribbling = ppdata->p_spd->dribble;
	ppdata->_stealing  = ppdata->p_spd->steal;
	ppdata->_blocking  = ppdata->p_spd->block;
	ppdata->_passing   = ppdata->p_spd->pass;
	ppdata->_power     = ppdata->p_spd->power;
	ppdata->_dunking   = ppdata->p_spd->dunk;
	ppdata->_clutch    = ppdata->p_spd->clutch;

	// bump up spd if player is a human
	base = ppdata->p_spd->speed;
	if (!ISDRONE(ppdata)) {
		if (base < 14)
			base = 14;
	}
	ppdata->_speed 	   = speeds[base];
//	ppdata->_speed 	   = speeds[(int)ppdata->p_spd->speed];

	pobj->pdecal = plyr_decals[plyrnum];
	plyr_decals[plyrnum][0] = team_decals[plyrnum][1];				// flesh		0
	plyr_decals[plyrnum][1] = team_decals[plyrnum][0];				// head			4
	plyr_decals[plyrnum][2] = team_decals[plyrnum][2];				// jersey		8
	plyr_decals[plyrnum][3] = team_decals[plyrnum][3];				// numbers	12
	plyr_decals[plyrnum][4] = team_decals[plyrnum][4];				// reflection	16

	guTexChangeLodBias( plyr_decals[plyrnum][0], lod_bias);
	guTexChangeLodBias( plyr_decals[plyrnum][1], lod_bias);
	guTexChangeLodBias( plyr_decals[plyrnum][2], lod_bias);
	guTexChangeLodBias( plyr_decals[plyrnum][3], lod_bias);
	guTexChangeLodBias( plyr_decals[plyrnum][4], lod_bias);

	if(ppdata->p_spd->flags & RIGHT)
		ppdata->ball_in_hand = ppdata->handedness = RIGHT_HAND;
	else
		ppdata->ball_in_hand = ppdata->handedness = LEFT_HAND;
}

#if 0
//////////////////////////////////////////////////////////////////////////////
// player_proc - main player process.
// arg0 = plyrnum
// arg1 = team
//
//extern int I40_present;
void player_proc_a( int *parg )
{
	bbplyr_data	* ppdata;
	obj_3d * pobj;
	pdata_t * ppdb;
	float lod_bias = -1.25f;
	int plyrnum, plyrteam, plyrpdata, i;
	char cbuf[20];
	float hx2, hy2, hz2;
	float hx, hy, hz, *tm;
	int limb, base;


	int (*mode_functions[])(bbplyr_data *) =	//FIX!!!??? each player with local copy?
	{
		 _mode_chill, _mode_offense, _mode_defense,
		 _mode_out_of_bounds, _mode_tipoff, _mode_freethrow
	};

#if 0
	if( I40_present )
		fprintf( stderr, "I40 board found\n" );
	else
		fprintf( stderr, "I40 board not foun\n" );
#endif

	ball_obj.int_receiver = NO_RECEIVER;
	ball_obj.who_threw    = NO_PASSER;
	ball_obj.who_shot     = NO_SHOOTER;
	ball_obj.scale = 1.0f;
	ball_obj.color = 0xff000000;
	//ball_obj.alpha = 255;

	// extract arguments
	plyrnum  = parg[0];
	plyrteam = parg[1];

	if ((plyrpdata = plyrnum) >= (MAX_PLYRS/2))
	{
		plyrpdata -= (NUM_PLAYERS/2);
		if (plyrpdata < 0 || plyrpdata >= (MAX_PLYRS/2))
			plyrpdata = -1;
		else
			plyrpdata += (MAX_PLYRS/2);
	}

	ppdata = &player_blocks[plyrnum];
	pobj   = (obj_3d *)ppdata;
	ppdb   = (plyrpdata < 0) ? NULL : &pdata_blocks[plyrpdata];

	pobj->x = pobj->y = pobj->z = 0.0f;
	pobj->flags = 0;

	// Set ptr to player's static data
	ppdata->p_spd    = selected_players[plyrnum].player_data;
	// init player data
	ppdata->plyrnum           = plyrnum;
	ppdata->team              = plyrteam;

	ppdata->f_headsc      = player_load_info[plyrnum].head_scale;
	ppdata->head_model    = player_load_info[plyrnum].head_model;
	ppdata->body_model    = player_load_info[plyrnum].body_model;
	ppdata->jnum_model    = player_load_info[plyrnum].jnum_model;
	ppdata->reflect_model = player_load_info[plyrnum].reflect_model;

	calc_player_scale(FALSE, 0, ppdata->p_spd->tall, ppdata->p_spd->weight, &pobj->zscale, &pobj->yscale);
	pobj->xscale = pobj->zscale;
	pobj->ascale = pobj->yscale;
	ppdata->position = ppdata->p_spd->position;

	if(pup_bighead & (1<<ppdata->plyrnum))
		ppdata->f_headsc *= bighead_scale(selected_players[plyrnum].team, selected_players[plyrnum].player_index);

	ppdata->head_tick_wait    = 120;
	ppdata->dribble_mode      = DM_CAN_DRIBBLE;
	ppdata->no_ball_collide   = 0;
	ppdata->HangingOnRim      = NO;
	ppdata->auto_control      = NO;
	ppdata->defense_anim_wait = 0;
	ppdata->total_time        = 0;
	ppdata->flight_time       = 0;
	ppdata->offender          = (plyrnum + NUM_PERTEAM) % NUM_PLAYERS;

	// fill in player ratings
	ppdata->_shooting  = ppdata->p_spd->shot;
	ppdata->_threeshot = ppdata->p_spd->rebound;
	ppdata->_dribbling = ppdata->p_spd->dribble;
	ppdata->_stealing  = ppdata->p_spd->steal;
	ppdata->_blocking  = ppdata->p_spd->block;
	ppdata->_passing   = ppdata->p_spd->pass;
	ppdata->_power     = ppdata->p_spd->power;
	ppdata->_dunking   = ppdata->p_spd->dunk;
	ppdata->_clutch    = ppdata->p_spd->clutch;


	// bump up spd if player is a human
	base = ppdata->p_spd->speed;
	if (!ISDRONE(ppdata)) {
		if (base < 14)	//12
			base = 14;	//12
	}
	ppdata->_speed 	   = speeds[base];
//	ppdata->_speed 	   = speeds[(int)ppdata->p_spd->speed];

	if (ppdb) ppdb->turbo = 100.0f;

	//fprintf(stderr, "PLAYER PROC %d offender = %d, position = %d\n", plyrnum, ppdata->offender, ppdata->position);
	//if(ppdata->plyrnum == 0)
	//	fprintf(stderr, "is_low_res = %d\n", is_low_res);

	pobj->pdecal = plyr_decals[plyrnum];
	plyr_decals[plyrnum][0] = team_decals[plyrnum][1];				// flesh		0
	plyr_decals[plyrnum][1] = team_decals[plyrnum][0];				// head			4
	plyr_decals[plyrnum][2] = team_decals[plyrnum][2];				// jersey		8
	plyr_decals[plyrnum][3] = team_decals[plyrnum][3];				// numbers	12
	plyr_decals[plyrnum][4] = team_decals[plyrnum][4];				// reflection	16

	guTexChangeLodBias( plyr_decals[plyrnum][0], lod_bias);
	guTexChangeLodBias( plyr_decals[plyrnum][1], lod_bias);
	guTexChangeLodBias( plyr_decals[plyrnum][2], lod_bias);
	guTexChangeLodBias( plyr_decals[plyrnum][3], lod_bias);
	guTexChangeLodBias( plyr_decals[plyrnum][4], lod_bias);
//	fprintf(stderr, "%d: assigned texture ptrs\n", ppdata->plyrnum); 

	ppdata->stick_cur = 24;
	ppdata->but_cur   = 0;

	//if(plyrnum == 0)
	//	pobj->plyrmode = MODE_CHILL;
	//else
#ifndef NO_TIP
		pobj->plyrmode = MODE_TIPOFF;	//MODE_OFFENSE;
#else
		pobj->plyrmode = (plyrnum / NUM_PERTEAM) ? MODE_OFFENSE : MODE_DEFENSE;
#endif

	pobj->friction = 0.0f;

	ppdata->attack_mode    = ATTMODE_NONE;
	ppdata->attack_time    = 0;
	ppdata->celebrate_time = 0;
	ppdata->misc_wait_time = -1;

	if(ppdata->p_spd->flags & RIGHT)
		ppdata->ball_in_hand = ppdata->handedness = RIGHT_HAND;
	else
		ppdata->ball_in_hand = ppdata->handedness = LEFT_HAND;

	// initialize anim data
	pobj->adata[0].adata = ppdata->anidata[0];
	pobj->adata[0].idata = ppdata->anidata[1];
	pobj->adata[1].adata = ppdata->anidata[2];
	pobj->adata[1].idata = ppdata->anidata[3];

	pobj->adata[2].adata = ppdata->anidata[4];
	pobj->adata[3].adata = ppdata->anidata[5];

	pobj->adata[1].animode = MODE_SECONDARY;

	ppdata->head_dir = ppdata->head_tgt_dir = -1; // body dir is also look dir
	ppdata->head_turn_rate = 32;

// Put players into standard standstill
_player_standstill(ppdata);

	if (COURT_IS_INDOORS) {
		sprintf(cbuf, "plyrrefl%d", ppdata->plyrnum);
		create_object(cbuf, OID_PLAYER, OF_NONE, DRAWORDER_FIELD + ppdata->plyrnum, (void *)ppdata, draw_player_reflection);
	}

	sprintf( cbuf, "plyr%d", plyrnum );
	create_object( cbuf, OID_PLAYER, OF_NONE, DRAWORDER_PLAYER + plyrnum, (void *)ppdata, draw_player );
	sprintf( cbuf, "shad%d", plyrnum );
	create_object( cbuf, OID_PLAYER, OF_NONE, DRAWORDER_FIELDFX + 0x20 + plyrnum, (void *)ppdata, draw_player_shadow );

//	if(randrng(100) <= 50)
//		change_anim( pobj, s_breath1_anim );
//	else
//		change_anim( pobj, s_breath1_anim );

	if( ppdata->plyrnum == 0 )
	{
		create_object( "preplyr", OID_PLAYER, OF_NONE, DRAWORDER_PLAYER-1, NULL, player_pre_draw );
		player_shadow_init();
		create_object( "preshad", OID_PLAYER, OF_NONE, DRAWORDER_FIELDFX + 0x1F, NULL, player_shadow_predraw );
	}
	change_anim(pobj, buyin_stance_anims[plyrnum]);

	// 'simple' player loop
	while( 1 )
	{
		// decrement timer on celebration bit
		if( ppdata->celebrate_time )
		{
			if(( ppdata->celebrate_time -= 1) == 0 )
				pobj->flags &= ~PF_CELEBRATE;
		}

		// animate
  		animate_model( pobj );

		// sleep
		sleep( 1 );
	}
}
#endif //0

//////////////////////////////////////////////////////////////////////////////
// player_proc - main player process.
// arg0 = plyrnum
// arg1 = team
//
//extern int I40_present;
void player_proc( int *parg )
{
	bbplyr_data	* ppdata;
	obj_3d * pobj;
	pdata_t * ppdb;
	float lod_bias = -1.25f;
	int plyrnum, plyrteam, plyrpdata, i;
	char cbuf[20];
	float hx2, hy2, hz2;
	float hx, hy, hz, *tm;
	int limb, base;


	int (*mode_functions[])(bbplyr_data *) =	//FIX!!!??? each player with local copy?
	{
		 _mode_chill, _mode_offense, _mode_defense,
		 _mode_out_of_bounds, _mode_tipoff, _mode_freethrow
	};

#if 0
	if( I40_present )
		fprintf( stderr, "I40 board found\n" );
	else
		fprintf( stderr, "I40 board not foun\n" );
#endif

	ball_obj.int_receiver = NO_RECEIVER;
	ball_obj.who_threw    = NO_PASSER;
	ball_obj.who_shot     = NO_SHOOTER;
	ball_obj.scale = 1.0f;
	ball_obj.color = 0xff000000;
	//ball_obj.alpha = 255;

	// extract arguments
	plyrnum  = parg[0];
	plyrteam = parg[1];

	if ((plyrpdata = plyrnum) >= (MAX_PLYRS/2))
	{
		plyrpdata -= (NUM_PLAYERS/2);
		if (plyrpdata < 0 || plyrpdata >= (MAX_PLYRS/2))
			plyrpdata = -1;
		else
			plyrpdata += (MAX_PLYRS/2);
	}

	ppdata = &player_blocks[plyrnum];
	pobj   = (obj_3d *)ppdata;
	ppdb   = (plyrpdata < 0) ? NULL : &pdata_blocks[plyrpdata];

	pobj->x = pobj->y = pobj->z = 0.0f;
	pobj->flags = 0;

	// Set ptr to player's static data
	ppdata->p_spd    = selected_players[plyrnum].player_data;
	// init player data
	ppdata->plyrnum           = plyrnum;
	ppdata->team              = plyrteam;

	ppdata->f_headsc      = player_load_info[plyrnum].head_scale;
	ppdata->head_model    = player_load_info[plyrnum].head_model;
	ppdata->body_model    = player_load_info[plyrnum].body_model;
	ppdata->jnum_model    = player_load_info[plyrnum].jnum_model;
	ppdata->reflect_model = player_load_info[plyrnum].reflect_model;

	calc_player_scale(FALSE, 0, ppdata->p_spd->tall, ppdata->p_spd->weight, &pobj->zscale, &pobj->yscale);
	pobj->xscale = pobj->zscale;
	pobj->ascale = pobj->yscale;
	ppdata->position = ppdata->p_spd->position;

	if(pup_bighead & (1<<ppdata->plyrnum))
		ppdata->f_headsc *= bighead_scale(selected_players[plyrnum].team, selected_players[plyrnum].player_index);

	ppdata->head_tick_wait    = 120;
	ppdata->dribble_mode      = DM_CAN_DRIBBLE;
	ppdata->no_ball_collide   = 0;
	ppdata->HangingOnRim      = NO;
	ppdata->auto_control      = NO;
	ppdata->defense_anim_wait = 0;
	ppdata->total_time        = 0;
	ppdata->flight_time       = 0;
	ppdata->offender          = (plyrnum + NUM_PERTEAM) % NUM_PLAYERS;

	// fill in player ratings
	ppdata->_shooting  = ppdata->p_spd->shot;
	ppdata->_threeshot = ppdata->p_spd->rebound;
	ppdata->_dribbling = ppdata->p_spd->dribble;
	ppdata->_stealing  = ppdata->p_spd->steal;
	ppdata->_blocking  = ppdata->p_spd->block;
	ppdata->_passing   = ppdata->p_spd->pass;
	ppdata->_power     = ppdata->p_spd->power;
	ppdata->_dunking   = ppdata->p_spd->dunk;
	ppdata->_clutch    = ppdata->p_spd->clutch;


	// bump up spd if player is a human
	base = ppdata->p_spd->speed;
	if (!ISDRONE(ppdata)) {
		if (base < 14)	//12
			base = 14;	//12
	}
	ppdata->_speed 	   = speeds[base];
//	ppdata->_speed 	   = speeds[(int)ppdata->p_spd->speed];

	if (ppdb) ppdb->turbo = 100.0f;

	//fprintf(stderr, "PLAYER PROC %d offender = %d, position = %d\n", plyrnum, ppdata->offender, ppdata->position);
	//if(ppdata->plyrnum == 0)
	//	fprintf(stderr, "is_low_res = %d\n", is_low_res);

	pobj->pdecal = plyr_decals[plyrnum];
	plyr_decals[plyrnum][0] = team_decals[plyrnum][1];				// flesh		0
	plyr_decals[plyrnum][1] = team_decals[plyrnum][0];				// head			4
	plyr_decals[plyrnum][2] = team_decals[plyrnum][2];				// jersey		8
	plyr_decals[plyrnum][3] = team_decals[plyrnum][3];				// numbers	12
	plyr_decals[plyrnum][4] = team_decals[plyrnum][4];				// reflection	16

	guTexChangeLodBias( plyr_decals[plyrnum][0], lod_bias);
	guTexChangeLodBias( plyr_decals[plyrnum][1], lod_bias);
	guTexChangeLodBias( plyr_decals[plyrnum][2], lod_bias);
	guTexChangeLodBias( plyr_decals[plyrnum][3], lod_bias);
	guTexChangeLodBias( plyr_decals[plyrnum][4], lod_bias);
//	fprintf(stderr, "%d: assigned texture ptrs\n", ppdata->plyrnum); 

	ppdata->stick_cur = 24;
	ppdata->but_cur   = 0;

	//if(plyrnum == 0)
	//	pobj->plyrmode = MODE_CHILL;
	//else
#ifndef NO_TIP
		pobj->plyrmode = MODE_TIPOFF;	//MODE_OFFENSE;
#else
		pobj->plyrmode = (plyrnum / NUM_PERTEAM) ? MODE_OFFENSE : MODE_DEFENSE;
#endif

	pobj->friction = 0.0f;

	ppdata->attack_mode    = ATTMODE_NONE;
	ppdata->attack_time    = 0;
	ppdata->celebrate_time = 0;
	ppdata->misc_wait_time = -1;

	if(ppdata->p_spd->flags & RIGHT)
		ppdata->ball_in_hand = ppdata->handedness = RIGHT_HAND;
	else
		ppdata->ball_in_hand = ppdata->handedness = LEFT_HAND;

	// initialize anim data
	pobj->adata[0].adata = ppdata->anidata[0];
	pobj->adata[0].idata = ppdata->anidata[1];
	pobj->adata[1].adata = ppdata->anidata[2];
	pobj->adata[1].idata = ppdata->anidata[3];

	pobj->adata[2].adata = ppdata->anidata[4];
	pobj->adata[3].adata = ppdata->anidata[5];

	pobj->adata[1].animode = MODE_SECONDARY;

	ppdata->head_dir = ppdata->head_tgt_dir = -1; // body dir is also look dir
	ppdata->head_turn_rate = 32;

// Put players into standard standstill
_player_standstill(ppdata);

	if (COURT_IS_INDOORS) {
		sprintf(cbuf, "plyrrefl%d", ppdata->plyrnum);
		create_object(cbuf, OID_PLAYER, OF_NONE, DRAWORDER_FIELD + ppdata->plyrnum, (void *)ppdata, draw_player_reflection);
	}

	sprintf( cbuf, "plyr%d", plyrnum );
	create_object( cbuf, OID_PLAYER, OF_NONE, DRAWORDER_PLAYER + plyrnum, (void *)ppdata, draw_player );
	sprintf( cbuf, "shad%d", plyrnum );
	create_object( cbuf, OID_PLAYER, OF_NONE, DRAWORDER_FIELDFX + 0x20 + plyrnum, (void *)ppdata, draw_player_shadow );

	if(randrng(100) <= 50)
		change_anim( pobj, s_breath1_anim );
	else
		change_anim( pobj, s_breath1_anim );

	if( ppdata->plyrnum == 0 )
	{
		create_object( "preplyr", OID_PLAYER, OF_NONE, DRAWORDER_PLAYER-1, NULL, player_pre_draw );
		player_shadow_init();
		create_object( "preshad", OID_PLAYER, OF_NONE, DRAWORDER_FIELDFX + 0x1F, NULL, player_shadow_predraw );
	}

	// main player loop
	while( 1 )
	{
//#if 1
//if ((plyrnum == 0) && (haltstep))
//{
//	//for (i=0; i < 12; i++)
//	//	fprintf(stderr,"cambot xform [%d]=%f\n",i,cambot.xform[i]);
//	fprintf(stderr,"cambot xyz %f %f %f\n",cambot.x,cambot.y,cambot.z);
//}
//#endif

		if( ppdata->stick_cur == ppdata->stick_old )
			ppdata->stick_time += 1;
		else
			ppdata->stick_time = 0;

		if( ppdata->stick_cur != 24 )
			ppdata->stick_offcenter_time += 1;
		else
			ppdata->stick_offcenter_time = 0;

		ppdata->stick_old = ppdata->stick_cur;
		ppdata->but_old   = ppdata->but_cur;

		// get current switches
		if( game_info.plyr_control[plyrnum] > -1 && ppdata->auto_control == NO)
		{
			// human
			ppdata->stick_cur = (get_player_49way_current() >> (game_info.plyr_control[plyrnum]<<3)) & P_SWITCH_MASK;
			ppdata->but_cur   = (get_player_sw_current()    >> (game_info.plyr_control[plyrnum]<<3)) & P_ABCD_MASK;
		}
		else
		{
			// drone
			get_drone_switches( ppdata );
		}
		ppdata->but_new = ppdata->but_cur & ~(ppdata->but_old);

// check to see if dribble tweens are necessary
		if(ppdata->stick_offcenter_time > 60
		&& game_info.ball_handler == plyrnum
		&& randrng(100) > 30
//		&& plyr_bearing(plyrnum, nearest_to_me(plyrnum, !plyrteam)) <= 80
//		&& distxz[plyrnum][nearest_to_me(plyrnum, !plyrteam)] <= 50.0f
		&& (pobj->adata[0].seq_index != (SEQ_D_BKN1_R)) && (pobj->adata[0].seq_index != (SEQ_D_BKN1_L))
		&& (pobj->adata[0].seq_index != (SEQ_D_BKN2_R)) && (pobj->adata[0].seq_index != (SEQ_D_BKN2_L))
		)
			dribble_tweens = YES;
		else
			dribble_tweens = NO;

		// decrement timer on attack boxes
		if( ppdata->attack_time )
		{
			if( --ppdata->attack_time == 0 )
				ppdata->attack_mode = 0;
		}

	if(!(game_info.ball_mode == BM_PASSING && ppdata->plyrnum == ball_obj.int_receiver))
	{
		// set or clear turbo bit
		if (pobj->flags & PF_TURBO)
			pobj->flags |= PF_OLD_TURBO;
		else
			pobj->flags &= ~PF_OLD_TURBO;

		// If turbo button and (is a drone or is a human with some turbo left)
		//FIX!!!??? where is drone turbo count to check?
		if ((ppdata->but_cur & P_C) &&
				((game_info.plyr_control[plyrnum] < 0) ||
				 (ppdb && ppdb->turbo > 0.0f))
			)
		{
			pobj->flags |= PF_TURBO;

			// must hit turbo within 10 ticks of last release for spin move
			if (ppdata->non_turbo_time > 0 && ppdata->non_turbo_time < 9)
				ppdata->non_turbo_time = -1;
			else
				ppdata->non_turbo_time = 0;
		}
		else
		{
			pobj->flags &= ~PF_TURBO;
			ppdata->non_turbo_time++;
		}
	}

		// read double tap of (A) button ?Y?
		if( ppdata->but_cur & P_A )
		{
			// must hit turbo within 5 ticks of last release for dive move
			if (ppdata->non_buta_time > 0 && ppdata->non_buta_time < 9)
				ppdata->non_buta_time = -1;
			else
				ppdata->non_buta_time = 0;
		}
		else
			ppdata->non_buta_time++;

		// read double tap of (B) button ?Y?
		if( ppdata->but_cur & P_B )
		{
			// must hit turbo within 5 ticks of last release for dive move
			if (ppdata->non_butb_time > 0 && ppdata->non_butb_time < 9)
				ppdata->non_butb_time = -1;
			else
				ppdata->non_butb_time = 0;
		}
		else
			ppdata->non_butb_time++;

		if (ppdata->flight_time < ppdata->total_time) ppdata->flight_time++;

		// allow smooth defense motion
		if (ppdata->defense_anim_wait > 0) ppdata->defense_anim_wait--;

		ppdata->flail_time = GREATER(ppdata->flail_time - 1, 0);

		////////////////////////////
		// call plyrmode function //
		mode_functions[ pobj->plyrmode ]( ppdata );

		// animate
		if(!(pobj->flags & PF_ANITICK ))
			animate_model( pobj );

		if (ppdata->misc_wait_time >= 0) ppdata->misc_wait_time--;

		if (ppdata->no_ball_collide > 0) ppdata->no_ball_collide--;

		if( !(pobj->adata[0].animode & MODE_PUPPET ))
		{
			// stop player from running outta bounds!!  and flying thru backboard
//			if((plyrnum != OOBPasser && pobj->flags & PF_INBND) || (pobj->flags & PF_INBND && OOBPasser == NO_PASSER))
			if(plyrnum != OOBPasser)
			{
				pobj->z =  LESSER(pobj->z, (OUT_OF_BOUNDS_Z - OOB_RADIUS));
				pobj->z = GREATER(pobj->z,-(OUT_OF_BOUNDS_Z - OOB_RADIUS));
				pobj->x =  LESSER(pobj->x, (OUT_OF_BOUNDS_X - OOB_RADIUS));
				pobj->x = GREATER(pobj->x,-(OUT_OF_BOUNDS_X - OOB_RADIUS));
			}

			// now stop the player from going thru backboard
			if ((pobj->y > 13.0f) &&
				(plyr_behind_backboard(pobj) == NO) &&
				!(pobj->flags & (PF_DUNKER|PF_ALLYDUNKER)) &&
				!(ppdata->odata.adata[0].animode & MODE_REACTING) &&
				(fsqrt(pobj->vx * pobj->vx + pobj->vz * pobj->vz) != 0.0f))
			{
				if ((pobj->x <= -RIM_X) &&
					((pobj->z >= -12.0f) && (pobj->z <= 12.0f)))
				{
					pobj->vz = 0.0f;
					pobj->vx = 0.0f;
					//pobj->x += 0.05f;
				}
				else if ((pobj->x >= RIM_X) &&
						 ((pobj->z >= -12.0f) && (pobj->z <= 12.0f)))
				{
					pobj->vz = 0.0f;
					pobj->vx = 0.0f;
					//pobj->x -= 0.05f;
				}
			}

			if( !(pobj->adata[0].animode & MODE_UNINT ))
			{
				// out-of-bounds correction
				if(plyrnum != OOBPasser)
				{
					if( pobj->z + pobj->vz > OUT_OF_BOUNDS_Z)
					{
						if( pobj->vz > 0.0f )
						{
							// velocity points out of bounds
							pobj->vz *= -1.0f;	// = -0.5f;
						}
						if( pobj->z + pobj->vz <= OUT_OF_BOUNDS_Z)
							pobj->vz = OUT_OF_BOUNDS_Z - pobj->z;
					}

					if( pobj->z + pobj->vz < -OUT_OF_BOUNDS_Z)
					{
						if( pobj->vz < 0.0f )
						{
							// velocity points out of bounds
							pobj->vz *= -1;	//= 0.5f;
						}
						if( pobj->z + pobj->vz >= -OUT_OF_BOUNDS_Z)
							pobj->vz = -OUT_OF_BOUNDS_Z - pobj->z;
					}
	
					if( pobj->x + pobj->vx > OUT_OF_BOUNDS_X)
					{
						if( pobj->vx > 0.0f )
						{
							// velocity points out of bounds
							pobj->vx *= -1.0f;	//= -0.5f;
						}
						if( pobj->x + pobj->vx <= OUT_OF_BOUNDS_X)
							pobj->vx = OUT_OF_BOUNDS_X - pobj->x;
					}

					if( pobj->x + pobj->vx < -OUT_OF_BOUNDS_X)
					{
						if( pobj->vx < 0.0f )
						{
							// velocity points out of bounds
							pobj->vx *= -1.0f;	//= 0.5f;
						}
						if( pobj->x + pobj->vx >= -OUT_OF_BOUNDS_X)
							pobj->vx = -OUT_OF_BOUNDS_X - pobj->x;
					}
				}// END. oob correction
			} // if !UNINT

			// add velocity
			if(/*ppdata->misc_wait_time < 0
				&&*/ ppdata->HangingOnRim == NO
				)
			{
				pobj->x += pobj->vx;
				pobj->y += pobj->vy + GRAVITY / 2.0f;
				pobj->z += pobj->vz;

			// apply gravity
				pobj->vy += GRAVITY;

#if 0
				if(plyrnum == 0
				&& (ppdata->odata.adata[0].animode & MODE_REACTING
				))
				{
					;
					//printf("flight time %d, total time %d, vy %3.3f, y %3.3f, next y %3.3f\n",
					//player_blocks[0].flight_time,player_blocks[0].total_time,player_blocks[0].odata.vy,player_blocks[0].odata.y,
					//player_blocks[0].odata.y + 1.0f * player_blocks[0].odata.vy + 0.5f * GRAVITY * 1.0f * 1.0f);
				}
#endif
				if( pobj->y <= 0.0f )
				{
					pobj->y = 0.0f;
					pobj->vy = 0.0f;
			 //	pobj->flags &= ~(PF_REBOUND|PF_DUNKER|PF_ALLYDUNKER|PF_SWATING);
				}
			}
			else
			{// clear dunkers vel.
				pobj->vx = pobj->vy = pobj->vz = 0.0f;
			}
		}// END.IF !PUPPET

		// apply friction
		if( pobj->friction != 0.0f )
		{
			float mxz = fsqrt( pobj->vx * pobj->vx + pobj->vz * pobj->vz );
			float fx, fz;

			if( mxz < 0.0001f )
			{
				pobj->vx = 0.0f;
				pobj->vz = 0.0f;
				pobj->friction = 0.0f;
			}
			else
			{
				if( pobj->friction > mxz )
				{
					fx = pobj->vx;
					fz = pobj->vz;
					pobj->friction = 0;
				}
				else
				{
					fx = pobj->friction * pobj->vx / mxz;
					fz = pobj->friction * pobj->vz / mxz;
				}

				pobj->vx -= fx;
				pobj->vz -= fz;
			}
		}
		pobj->flags |= PF_INBND;
		if (!in_bounds(pobj))
			pobj->flags ^= PF_INBND;

#ifdef DEBUG
// Make this a 3 to check out cool debug stuff
		if(plyrnum == 30)
			ShowDebugInfo(ppdata);
#endif

#ifdef DEBUG_SHOT_PERCENTAGE
	if(plyrnum == game_info.ball_handler)
	{
		set_text_font(FONTID_BAST7T);
		delete_multiple_strings(SHOT_DISPLAY_TXT_ID, 0xFFFFFFFF);
		set_string_id(SHOT_DISPLAY_TXT_ID);
		set_text_position(256.0f, 375.0f, 2.0f);
		oprintf("%dj%dcShot %d", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, compute_shot_pct(pobj));
	}
#endif
		sleep( 1 );

//if (ppdata->plyrnum == 3)
//{
//float *tm,rhy,lhy,lfy,rfy,fy,ty;
//bbplyr_data *ppdata = (bbplyr_data *)pobj;
//
//		tm = cambot.xform;
//
//		ny  = ppdata->jpos[JOINT_NECK][0]   * tm[1] + ppdata->jpos[JOINT_NECK][1]   * tm[5] + ppdata->jpos[JOINT_NECK][2]   * tm[9] + cambot.y;
//		rhy = ppdata->jpos[JOINT_RWRIST][0] * tm[1] + ppdata->jpos[JOINT_RWRIST][1] * tm[5] + ppdata->jpos[JOINT_RWRIST][2] * tm[9] + cambot.y;
//		lhy = ppdata->jpos[JOINT_LWRIST][0] * tm[1] + ppdata->jpos[JOINT_LWRIST][1] * tm[5] + ppdata->jpos[JOINT_LWRIST][2] * tm[9] + cambot.y;
//
//		rfy = ppdata->jpos[JOINT_RANKLE][0] * tm[1] + ppdata->jpos[JOINT_RANKLE][1] * tm[5] + ppdata->jpos[JOINT_RANKLE][2] * tm[9] + cambot.y;
//		lfy = ppdata->jpos[JOINT_LANKLE][0] * tm[1] + ppdata->jpos[JOINT_LANKLE][1] * tm[5] + ppdata->jpos[JOINT_LANKLE][2] * tm[9] + cambot.y;
//
//		fy = rfy <= lfy ? rfy : lfy;
//		ty = rhy >= lhy ? rhy : lhy;
//		ty = ny >= ty ? ny : ty;
//
//		printf("ball_y = %3.3f, ty = %3.3f, fy = %3.3f\n", ball_obj.odata.y, ty, fy);
//}

	}
}

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
#define PI_RAD			3.1415926f

extern char *ball_mode_names[];

extern float axz[][3];
extern int sect_1, sect_2, sect_3, read_player34_direct(void), allow_switching, allow_switching_timeout, reset_switching;
//extern int rim_times[], best_cutoff[], offenders[];

int dangle;
float d_dist = 0.0f;

void ShowDebugInfo(bbplyr_data *ppdata)
{
#define DEBUG_ID 0x7757
obj_3d *pobj = (obj_3d *)ppdata;
//obj_3d *passer3d = (obj_3d *)ppdata, *pobj = (obj_3d *)(player_blocks + 0);//, hex = read_player34_direct();
int quad4 = 0, quad8 = 0, angle = 0, rim_bearing, bearing, range;
float rim_pt[3];	//p[3],pp[3], xf[2], yf[2],
float *cam = cambot.xform;
//float rx,ry,rz,x,y,z,xf,yf;
#if 0
char *ddir[] = {
	"forward",
	"backward",
	"left",
	"right"
	};
#endif

char *q8[] = {
	"BEHIND",
	"LEFT BEHIND",
	"LEFT",
	"LEFT FRONT",
	"FRONT",
	"RIGHT FRONT",
	"RIGHT",
	"RIGHT BEHIND"
	};

char *shot[] = {
	"JUMPER_2",
	"JUMPER_3",
	"DUNK",
	"LAYUP"
	};

//char junk[18];
//char *pjunk = &junk[0];

//
//	for(i=15; i>=0; i--) {
//		if(hex & 1<<i) *pjunk++ = '1';
//		else
//			*pjunk++ = '0';
//		if (i == 8) *pjunk++ = ' ';	
//		}
//	*pjunk = '\0';

	set_text_font(FONTID_BAST7T);
	delete_multiple_strings(DEBUG_ID, 0xFFFFFFFF);
	set_string_id(DEBUG_ID);

	set_text_position(5.0f, 305.0f, 2.0f);

/////////////////////////////////////////////////////
//	oprintf("%dj %d no ball coll: %d %d %d %d - vel 0x%3.2f,z%3.2f 1x%3.2f,z%3.2f 2x%3.2f,z%3.2f 3x%3.2f,z%3.2f",
//			HJUST_LEFT, players_ready, player_blocks[0].no_ball_collide, player_blocks[1].no_ball_collide, player_blocks[2].no_ball_collide, player_blocks[3].no_ball_collide,
//			player_blocks[0].odata.vx, player_blocks[0].odata.vz, player_blocks[1].odata.vx, player_blocks[1].odata.vz, player_blocks[2].odata.vx, player_blocks[2].odata.vz, player_blocks[3].odata.vx, player_blocks[3].odata.vz);

	set_text_position(5.0f, 295.0f, 2.0f);
	oprintf("%dj ignore pass = %d ignore steal = %d ignore_shoot = %d",
			HJUST_LEFT,
			ignore_pass, ignore_steal, ignore_shoot
			);
	return;

///////////////////////////////////////////////////
// debug player interactions
	if(game_info.ball_handler != NO_BALLHANDLER)
	{
 		quad8 = player_blocks[game_info.ball_handler].odata.fwd - p2p_angles[game_info.ball_handler][nearest_to_ball(!player_blocks[game_info.ball_handler].team)];
 		quad4 = ((quad8 + 1536 + 128)%1024) / 256;		// four directions
 		quad8 = ((quad8 + 1536 + 64)%1024) / 128;			// eight directions

		oprintf("%dj%dc bearing %d fwd %d angle me-him %d dist %3.2f q8 = %s btwn %s",
				HJUST_LEFT, LT_RED,
				plyr_bearing(game_info.ball_handler, nearest_to_ball(!player_blocks[game_info.ball_handler].team)),
				player_blocks[game_info.ball_handler].odata.fwd,
				p2p_angles[game_info.ball_handler][nearest_to_ball(!player_blocks[game_info.ball_handler].team)],
				distxz[game_info.ball_handler][nearest_to_ball(!player_blocks[game_info.ball_handler].team)],
				q8[quad8],
				in_between_ball_basket(&player_blocks[nearest_to_ball(!player_blocks[game_info.ball_handler].team)]) == YES ? "YES" : "NO"
				);
	}
	return;

///////////////////////////////////////////////////
// is the offender going toward the basket at all?
	set_text_position(10.0f, 290.0f, 2.0f);
	if(game_info.ball_handler != NO_BALLHANDLER)
	{
		rim_pt[0] = game_info.active_goal ? RIM_CX : -RIM_CX; rim_pt[1] = 0.0f; rim_pt[2] = 0.0f;
		//rim_bearing = (player_blocks[game_info.ball_handler ^ 1].odata.fwd - ptangle( &(player_blocks[game_info.ball_handler ^ 1].odata.x), rim_pt)) & 1023;
		rim_bearing = (player_blocks[game_info.ball_handler].odata.fwd - ptangle( &(player_blocks[game_info.ball_handler].odata.x), rim_pt)) & 1023;
		//rim_bearing = (rim_bearing > 512) ? 1024 - rim_bearing : rim_bearing;
		//quad8 = (rim_pt[0] < 0.0f ? 768 : 256) - ptangle( &player_blocks[game_info.ball_handler].odata.x, rim_pt );

		quad8 = get_angle_to_rim(&player_blocks[game_info.ball_handler ^ 1]) - ptangle(&player_blocks[game_info.ball_handler ^ 1].odata.x, &ball_obj.odata.x);
		quad4 = ((quad8 + 1536 + 128)&1023) / 256;		// four directions
		quad8 = ((quad8 + 1536 + 64) &1023) / 128;		// eight directions

		oprintf("%dj%dc  rim bear %d,%d dist rim = %3.2f quad %s x%3.3f z%3.3f shot %s", HJUST_LEFT, LT_RED,
							rim_bearing, rim_bearing > 512 ? 1024 - rim_bearing : rim_bearing,
							dist3d(&(player_blocks[game_info.ball_handler ^ 1].odata.x), rim_pt),
							q8[quad8],
							pobj->x, pobj->z,
							shot[WhatKindOfShot(&player_blocks[game_info.ball_handler].odata)]
							);
	}
	return;

//////////////////////////////////////////////////////////
// is the nearest defender inbetween the ball and basket 2
	if(game_info.ball_handler == NO_BALLHANDLER)
		return;

	pobj = (obj_3d *)(player_blocks + game_info.ball_handler);

	bearing = (get_angle_to_rim((bbplyr_data *)pobj) - ptangle( &pobj->x, &player_blocks[nearest_to_ball(!player_blocks[game_info.ball_handler].team)].odata.x)) & 1023;
	bearing = (bearing > 512) ? 1024 - bearing : bearing;

	range = 64 + (int)(48.0f * (player_blocks[nearest_to_ball(!player_blocks[game_info.ball_handler].team)]._power / 20.0f));

	oprintf("%dj%dc bearing %d range %d inbetween %s dist %3.3f already %d", HJUST_LEFT,LT_YELLOW,
		bearing, range,
		in_between_ball_basket_pow(&player_blocks[nearest_to_ball(!player_blocks[game_info.ball_handler].team)]) == YES ? "YES" : "NO",
		odistxz(pobj, (obj_3d *)&player_blocks[nearest_to_ball(!player_blocks[game_info.ball_handler].team)]),
		put_back_already
		);

	return;

//////////////////////////////////////////////////////////
// is the nearest defender inbetween the ball and basket 1
	if(game_info.ball_handler == NO_BALLHANDLER)
		return;

	pobj = game_info.ball_handler == NO_BALLHANDLER ? (obj_3d *)(&ball_obj) : (obj_3d *)(player_blocks + game_info.ball_handler);
	oprintf("%dj%dc ball %d near %d  dist: ball %3.2f  near %3.2f inbetween %s dist %3.3f", HJUST_LEFT,LT_YELLOW,
		get_angle_to_rim(&player_blocks[game_info.ball_handler]),
		get_angle_to_rim(&player_blocks[nearest_to_ball(!player_blocks[game_info.ball_handler].team)]),
		get_dist_to_rim(&player_blocks[game_info.ball_handler]),
		get_dist_to_rim(&player_blocks[nearest_to_ball(!player_blocks[game_info.ball_handler].team)]),
		in_between_ball_basket(&player_blocks[nearest_to_ball(!player_blocks[game_info.ball_handler].team)]) == YES ? "YES" : "NO",
		odistxz(pobj, (obj_3d *)&player_blocks[nearest_to_ball(!player_blocks[game_info.ball_handler].team)])
		);

	return;

///////////////////////////////////////////////////
// debug team interactions
	if(game_info.ball_handler != NO_BALLHANDLER)
	{
 		quad8 = player_blocks[game_info.ball_handler].odata.fwd - p2p_angles[game_info.ball_handler][game_info.ball_handler ^ 1];
 		quad4 = ((quad8 + 1536 + 128)%1024) / 256;		// four directions
 		quad8 = ((quad8 + 1536 + 64)%1024) / 128;			// eight directions

		oprintf("%dj%dc bearing %d fwd %d angle me-him %d dist %3.2f q8 = %s",
				HJUST_LEFT, LT_RED,
				player_blocks[game_info.ball_handler ^ 1].no_ball_collide,
				player_blocks[game_info.ball_handler].odata.fwd,
				p2p_angles[game_info.ball_handler][game_info.ball_handler ^ 1],
				distxz[game_info.ball_handler][game_info.ball_handler ^ 1],
				q8[quad8]);
	}
	return;

///////////////////////////////////////////////////
// debug misc stuff...2
	oprintf("%dj flail_times: %d %d %d %d",
			HJUST_LEFT,
			player_blocks[0].flail_time,
			player_blocks[1].flail_time,
			player_blocks[2].flail_time,
			player_blocks[3].flail_time
			);

	return;

///////////////////////////////////////////////////
// debug misc stuff...2
	if(game_info.ball_handler != NO_BALLHANDLER)
	{
	int air_time;
	float a,b,c,b2m4ac,root1,root2;

	a = GRAVITY / 2.0f;
	b = player_blocks[game_info.ball_handler].odata.vy;
	c = player_blocks[game_info.ball_handler].odata.y;
	b2m4ac = b * b - 4.0f * a * c;
	root1 = (-1.0f * b + fsqrt( b2m4ac )) / (2.0f * a);
	root2 = (-1.0f * b - fsqrt( b2m4ac )) / (2.0f * a);
	air_time = 1;
	if (root1 > 1.0f)
		air_time = (int)(root1+0.9f);

	if (root2 > 1.0f)
		air_time = (int)(root2+0.9f);

	oprintf("%dj root1 = %3.3f root2 = %3.3f air_time = %d",
			HJUST_LEFT, root1 + .9f, root2 + .9f, air_time
			);
	}
	return;

///////////////////////////////////////////////////
// debug misc stuff...1
	oprintf("%dj ignore pass = %d ignore steal = %d",
			HJUST_LEFT,
			ignore_pass, ignore_steal
			);

	return;

///////////////////////////////////////////////////
// debug player speeds
	oprintf("%dj icnt = %d scnt = %d sinc = %f iframe %d fframe %f vx %3.3f vz %3.3f v %3.3f",
			HJUST_LEFT,
			pobj->adata[0].icnt,
			pobj->adata[0].scnt,
			pobj->adata[0].sinc,
			pobj->adata[0].iframe,
			pobj->adata[0].fframe,
			pobj->vx,
			pobj->vz,
			fsqrt((pobj->vx * pobj->vx) + (pobj->vz * pobj->vz))
			);

	return;

///////////////////////////////////////////////////
	rim_pt[0] = game_info.active_goal ? RIM_CX : -RIM_CX; rim_pt[1] = 0.0f; rim_pt[2] = 0.0f;

	if(game_info.ball_handler != NO_BALLHANDLER)
	{
		quad8 = (rim_pt[0] < 0.0f ? 768 : 256) - ptangle( &player_blocks[game_info.ball_handler].odata.x, rim_pt );
		quad4 = ((quad8 + 1536 + 128)&1023) / 256;		// four directions
		quad8 = ((quad8 + 1536 + 64) &1023) / 128;		// eight directions

		oprintf("%dj%dc q8 %s crowd cheer time %d sect = %d,%d,%d", HJUST_LEFT, LT_GRAY, q8[quad8], crowd_cheer_time,sect_1, sect_2, sect_3); 
	}
	return;

/////////////////////////////////////////////////////
	if(game_info.ball_handler != -1)
	{
		oprintf("%dj%dc bh:%d near %d bear %3d itime %2d", HJUST_LEFT,LT_GRAY,
	game_info.ball_handler,
	nearest_to_me(player_blocks[game_info.ball_handler].plyrnum, !player_blocks[game_info.ball_handler].team),
	plyr_bearing(player_blocks[game_info.ball_handler].plyrnum, nearest_to_me(player_blocks[game_info.ball_handler].plyrnum, !player_blocks[game_info.ball_handler].team)),
	intercept_time(player_blocks[game_info.ball_handler].plyrnum, nearest_to_me(player_blocks[game_info.ball_handler].plyrnum, !player_blocks[game_info.ball_handler].team), player_blocks[game_info.ball_handler]._speed )
				);
	}
	return;

/////////////////////////////////////////////////////
// debug head turning
	oprintf("%dj%dc  0 %2d:%2d - 1 %2d:%2d", HJUST_LEFT,LT_GRAY,
						head_wait[0], look[0], head_wait[1], look[1]);
	set_text_position(5.0f, 325.0f, 2.0f);
	oprintf("%dj%dc  2 %2d:%2d - 3 %2d:%2d", HJUST_LEFT,LT_GRAY,
						head_wait[2], look[2], head_wait[3], look[3]);
	return;

//////////////////////////////////////////////////////
	quad8 = (rim_pt[0] < 0.0f ? 768 : 256) - ptangle( &(player_blocks[NUM_PERTEAM].odata.x), rim_pt );
	quad4 = ((quad8 + 1536 + 128)&1023) / 256;		// four directions
	quad8 = ((quad8 + 1536 + 64) &1023) / 128;		// eight directions

	//oprintf("%dj%dcshot %d", HJUST_LEFT, LT_GRAY, compute_shot_pct((obj_3d *)(player_blocks + game_info.ball_handler)));
	//return;

	angle = (int)((atan2(player_blocks[NUM_PERTEAM].odata.vx, player_blocks[NUM_PERTEAM].odata.vz) / M_PI) * 512.0f);
	angle = angle < 0 ? 1024 + angle : angle;
	angle = (player_blocks[NUM_PERTEAM].odata.vx == 0.0f && player_blocks[NUM_PERTEAM].odata.vz == 0.0f) ? 0 : angle;

	oprintf("%dj%dc angle %d", HJUST_LEFT,LT_GRAY, angle);

	return;

	//quad8 = player_blocks[0].odata.fwd - p2p_angles[0][1];
	//quad4 = ((quad8 + 1536 + 128)&1023) / 256;		// four directions
	//quad8 = ((quad8 + 1536 + 64) &1023) / 128;		// eight directions
	//oprintf("%djquad4 = %d, quad8 = %d",HJUST_LEFT,quad4,quad8);
}
#endif //DEBUG

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
static int _player_standstill(bbplyr_data * ppdata)
{
	static float stand_xz[][2] = {
		{ 50,-30 }, { 70, 50 }, { 10, 65 }, { 0, 0 },
#if NUM_PERTEAM > 2
		{-75, 40 }, { 40, 60 }
#endif
	};
	ppdata->odata.fwd = ppdata->odata.tgt_fwd = 0;

#ifdef DEBUG_HEADS
	ppdata->odata.z = 0.0f;// - ((ppdata->plyrnum % NUM_PERTEAM) * 7.0f);
	ppdata->odata.x = (6.0f * (ppdata->plyrnum < NUM_PERTEAM ? -1.0f : 1.0f)) +
					(((ppdata->plyrnum % NUM_PERTEAM) * 15.0f) * (ppdata->plyrnum < NUM_PERTEAM ? -1.0f : 1.0f));
//	ppdata->odata.x = (5.0f * (ppdata->plyrnum < NUM_PERTEAM ? -1.0f : 1.0f)) +
//					(((ppdata->plyrnum % NUM_PERTEAM) * 5.0f) * (ppdata->plyrnum < NUM_PERTEAM ? -1.0f : 1.0f));
#else
	ppdata->odata.x = stand_xz[ppdata->plyrnum][0];
	ppdata->odata.z = stand_xz[ppdata->plyrnum][1];
#endif

	ppdata->odata.ax = 0.0f;
	ppdata->odata.az = 0.0f;
	zerovels( &ppdata->odata );
	ppdata->stick_cur = 24;
	ppdata->but_cur   = 0;
	ppdata->hangtime  = -1;

	if(randrng(100) <= 100)
		change_anim( &ppdata->odata, s_breath1_anim );
	else
		change_anim( &ppdata->odata, s_breath1_anim );

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
#define SHOT_LEANER_VEL_DAMPEN	0.55f
#define FADE_ANGLE_TOLERANCE	128
#define FREE_THROW_LINE_X		43.0f


char *put_back_dunk_anims[] =
{
	put_back_dunk_1,
	put_back_dunk_1a,
	put_back_dunk_2,
	put_back_dunk_3,
};

char *double_dunk_anims[] =
{
	allyoop_1
};

char *shot_anims[] =
{
	shot3_anim,
//#if 0
	shot2_anim,
	shot1_anim,
	shot2_anim,
	shot4_anim,
	shot1_anim,
	shot3_anim,
	shot2_anim,
	shot5_anim,
	shot3_anim,
	shot6_anim,
	shot8_anim,
//#endif
};
int whichshot = 0, whichdunk = 0, what_type_shot = 0;

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
static int _mode_offense(bbplyr_data * ppdata)
{
	bbplyr_data *pl_p;

	obj_3d * pobj = (obj_3d *)ppdata;
	float p[3], dist_to_rim;
	int i, close = NO, cframe, angle, rim_bearing, quad8, quad4,
		min = (game_info.game_time & 0x00FF0000)>> 16,
		sec = (game_info.game_time & 0x0000FF00) >> 8;

#ifdef DEBUG_EXTREME
	for(i = 0; i < ppdata->plyrnum; i++) fprintf(stderr, " ");
	fprintf(stderr, "ENTER: _mode_offense %d\n",ppdata->plyrnum);
#endif

#if 0
	if(ppdata->plyrnum == game_info.ball_handler)
		ppdata->ghost = YES;
	else
		ppdata->ghost = NO;
#endif

// CHECK to see if OOBPasser MUST be cleared
	if(OOBPasser == ppdata->plyrnum)
	{
		if(in_bounds(pobj))
		{
			//printf("%s is the OOBPasser\n",ppdata->p_spd->szlast);
			OOBPasser = NO_PASSER;

			pl_p = player_blocks;
			i = NUM_PLAYERS;
			do {
				pl_p->auto_control = 0;
			} while (pl_p++, --i > 0);
		}
	}

	pobj->turn_rate = 64;
	pobj->turn_rate2 = 64;

// turn toward target heading
	turn_toward_tgt( pobj );

#ifdef DEBUG_HEADS
	ppdata->stick_cur = 24;
	ppdata->but_cur = 0;
	pobj->fwd = pobj->tgt_fwd = 768;
	pobj->z = 0.0f - ((ppdata->plyrnum % NUM_PERTEAM) * 7.0f);
	pobj->x = (5.0f * (ppdata->plyrnum < NUM_PERTEAM ? -1.0f : 1.0f)) + (((ppdata->plyrnum % NUM_PERTEAM) * 5.0f) * (ppdata->plyrnum < NUM_PERTEAM ? -1.0f : 1.0f));
	return YES;
#endif

// check to see if catching an ally oop
//	if(ppdata->plyrnum == ally_player)
//	{
//		if (ally_jump_wait > -1) ally_jump_wait--;
//		if (ally_jump_wait == 0)
//		{
//			change_anim((obj_3d *)(player_blocks + ally_player), allyoop_1);
//			ally_player = -1;
//			//fprintf(stderr,"throwing ally\n");
//			return 1;
//		}
//	}

	// allow a put-back, this close to the rim
	p[0] = game_info.active_goal ? RIM_X : -RIM_X; p[1] = 0.0f; p[2] = 0.0f;

//	if (dist3d(&(pobj->x), p) > 20.0f)
//	{
//		void * pani = rebound_auto(ppdata);		// Check for auto rebounding
//		if (pani)
//			change_anim(pobj, pani);
//	}

// am I receiving a pass?
	if(!(pobj->adata[0].animode & (MODE_UNINT))
	&& game_info.ball_mode == BM_PASSING
	&& ball_obj.int_receiver == ppdata->plyrnum
	&& ball_obj.flight_time <= ball_obj.total_time)
	{
	int edit = NO, ndir = pobj->tgt_fwd;

	// play a catch animation
		if((catch_anim != NULL)
		&& ((ball_obj.total_time - ball_obj.flight_time) <= catch_time)
		&& !(pobj->flags & PF_ALLYDUNKER))
		{
			if(!(player_blocks[ball_obj.int_receiver].odata.adata[0].animode & MODE_REACTING))
			{
				if((pobj->vx != 0.0f || pobj->vz != 0.0f) && non_twopart_catch == NO && oob_corrected_catch == NO)
				{
					begin_twopart_mode(pobj);
					change_anim1(pobj, dribble_run_2);
					change_anim2(pobj, catch_anim);
				}
				else
				{
					change_anim(pobj, catch_anim);
					if(oob_corrected_catch == YES)
					{
						pobj->tgt_fwd = ptangle( &(pobj->x), &ball_obj.odata.x);
	#ifdef DEBUG_CATCHING
						printf("OFFENSE - oob_corrected_catch: catch_time = %d ball.flighttime = %d, tot_time = %d\n",
							catch_time, ball_obj.flight_time, ball_obj.total_time);
	#endif
					}
				}
			}
			catch_anim = NULL;
			oob_corrected_catch = NO;

			// inc. game stat
			gAssist_time = 360;	// 6 seconds to score to credit assist
			ppdata->launched_from_hotspot = 0;
		}

		edit = oob_correction(pobj, &ndir);
		if(edit != NO)
		{
			if(edit == YES)
			{
				pobj->vx = FRELX( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? TURBO_BOOST : 1.0f)), ndir );
				pobj->vz = FRELZ( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? TURBO_BOOST : 1.0f)), ndir );
#ifdef DEBUG_CATCHING
				printf("OFFENSE - FIX ME!!!: %s is moving\n", ppdata->p_spd->szlast);
#endif
			}
			else
			{
				pobj->vx = pobj->vz = 0.0f;
			}
		}
		else
		{
			ppdata->stick_cur = ppdata->stick_old;
		}
		return 1;
	}

#if 0//DEBUG
// FIX: HACK!!!
	//if(ppdata->but_cur == P_D)
	if(ppdata->plyrnum == game_info.ball_handler)
	{
		//if( get_player_sw_current() & (P1_D|P2_D|P3_D|P4_D) )
		if( ((get_player_sw_current() >> (game_info.plyr_control[ppdata->plyrnum]<<3)) & P_ABCD_MASK) & P_D)
			ppdata->odata.adata[0].animode |= MODE_ANIDEBUG;
		else
			ppdata->odata.adata[0].animode &= ~MODE_ANIDEBUG;
	}
#endif

	if(pobj->adata[0].animode & MODE_UNINT)
	{
		// is this player wanting a quick layup...
		if((ppdata->but_new == P_A) &&
			!(pobj->flags & (PF_LAYUP|PF_SHOOTING|PF_PASSING)) &&  
			!(pobj->adata[0].animode & MODE_REACTING) &&
			(ppdata->plyrnum == game_info.ball_handler) &&
			//(pobj->flags & (PF_DUNKER|PF_ALLYDUNKER)))
			(pobj->y >= 7.0f || pobj->flags & (PF_DUNKER|PF_ALLYDUNKER)))
		{
			p[0] = game_info.active_goal ? RIM_CX : -RIM_CX; p[1] = 0.0f; p[2] = 0.0f;
			rim_bearing = (pobj->fwd - ptangle( &(pobj->x), p)) & 1023;
			dist_to_rim = dist3d(&(pobj->x), p);

//			if (((ppdata->total_time - ppdata->flight_time) >= 15) && (rim_bearing < 128) && (dist_to_rim < 55.0f))
			if (((ppdata->total_time - ppdata->flight_time) >= 15) && ((dist_to_rim > 20.0f) && (dist_to_rim < 80.0f)))
			{
				if (ppdata->flight_time <= 6)
					change_anim(pobj, quick_dunk_layup_1);
				else
					change_anim(pobj, dunk_layup_anims[randrng(sizeof(dunk_layup_anims)/sizeof(char *))]);
				// was this player cocky dribbling...
				if(pobj->adata[0].seq_index == (SEQ_D_COCK_1) || pobj->adata[0].seq_index == (SEQ_D_COCK_2))
					ticks_since_cocky = -COCKY_DRIBBLE_TWAIT, cocky_dribbler = NO_BALLHANDLER;
			}
		}

		// this is here for a player receiving a pass and needs to control his player.
		if(game_info.ball_mode == BM_DRIBBLING
		&& ppdata->plyrnum == game_info.ball_handler
		&& pobj->flags & PF_CATCHING)
		{
			offense_run( pobj );
		}

	// this is here for a player receiving a pass and is headed OOB.
		if(
		(pobj->vx != 0.0f || pobj->vz != 0.0f)
		&& game_info.ball_mode == BM_PASSING
		&& ppdata->plyrnum == ball_obj.int_receiver
		&& !(pobj->flags & (PF_SHOOTING|PF_DUNKER|PF_ALLYDUNKER|PF_LAYUP|PF_FUNKY_DRIBBLE))
		)
		{
			offense_run( pobj );
		}

#if 1
// allow shooting the ball as soon as a catch happens
		if((game_info.ball_handler == ppdata->plyrnum)
			//&& ((ppdata->but_cur == P_A) || (ppdata->but_cur == (P_A|P_C)))
			//&& ((ppdata->but_new & P_A))
			&& ((ppdata->but_cur & P_A))
			&& !(pobj->flags & (PF_LAYUP|PF_SHOOTING|PF_DUNKER|PF_ALLYDUNKER))
			&& !(pobj->adata[0].animode & MODE_REACTING)
			&& (pobj->flags & PF_CATCHING)
			&& ppdata->HangingOnRim == NO
			&& ppdata->odata.y == 0.0f
		  )	//(ppdata->but_new & P_A) && !(ppdata->but_new & P_C)) {
		{
			do_shot(ppdata);
			return YES;
		}// if shot button was pressed and he is the ball handler
#endif
	}	// END.if MODE_UNINT

	if (!(pobj->adata[0].animode & MODE_UNINT) &&
		!(game_info.ball_handler != NO_BALLHANDLER &&
			(player_blocks[game_info.ball_handler].odata.flags & PF_PASSING) &&
			(ppdata->plyrnum == ball_obj.int_receiver)))
	{
		// we're not in MODE_UNINT
		offense_run( pobj );

		p[0] = game_info.active_goal ? RIM_X : -RIM_X; p[1] = 0.0f; p[2] = 0.0f;

		if(game_info.ball_handler != ppdata->plyrnum)
		{
			// is player wanting a PUSH/KNOCKDOWN move
			if((ppdata->but_cur & P_C)
			&& (ppdata->but_new & P_B)
			&& (pobj->y == 0.0f)
			&& (pobj->flags & PF_TURBO))
			{
				if((pdata_blocks[ppdata->plyrnum].turbo >= 25.0f) || ISDRONE(ppdata))
				{
					if((game_info.fire_player != ppdata->plyrnum)
					&& (game_info.team_fire != player_blocks[ppdata->plyrnum].team)
					&& ISHUMAN(ppdata))
						pdata_blocks[ppdata->plyrnum].turbo -= 25.0f;

					change_anim(pobj, push_anims[randrng(sizeof(push_anims)/sizeof(char *))]);

					// debounce the P_B button so that a drone won't pass to you every
					// time you push somebody
					ppdata->but_new &= ~(P_B);
					ppdata->but_cur &= ~(P_B);
				}
			}

			// test for put-back dunks
			else if(
			(ppdata->but_cur == (P_A|P_C))
			&& !(pobj->flags & (PF_SHOOTING|PF_DUNKER|PF_ALLYDUNKER|PF_IN_AIR_BLOCK|PF_REBOUND|PF_SWATING))
			&& ((ball_obj.total_time - ball_obj.flight_time < 15) || (ball_obj.total_time+10 < ball_obj.flight_time))
			&& (game_info.ball_mode == BM_SHOOTING || game_info.ball_mode == BM_OFF_INTO_PLAY || game_info.ball_mode == BM_BOUNCE_ON_RIM)
			&& (ppdata->HangingOnRim == NO)
			&& (pobj->y == 0.0f)
			// Add 5% chance that he tries this n a good shot also
			&& ((ISDRONE(ppdata) && IsShotGood != YES) || (ISHUMAN(ppdata)))
			&& (put_back_already == 0)
			&& (abs(pobj->x) < RIM_X-5.0f)
//			&& (dist3d(&(ball_obj.odata.x), p) < 20.0f)
			&& (dist3d(&(pobj->x), p) < 20.0f))
			{
				//p[0] = game_info.active_goal ? RIM_X : -RIM_X; p[1] = 0.0f; p[2] = 0.0f;
			  	rim_bearing = (pobj->fwd - ptangle( &(pobj->x), p)) & 1023;
			  	whichdunk = rand() % (sizeof(put_back_dunk_anims) / sizeof(int));
			  	pobj->tgt_fwd = ptangle(&pobj->x, p);
			  	change_anim( pobj, put_back_dunk_anims[whichdunk]);
				// Only one guy at a time
				put_back_already = 1;				
			}
			// check for an ally oop
			else if(
		 			((ISHUMAN(ppdata) && (ppdata->but_cur & P_C) && (ppdata->but_new == P_A))
		 		|| (ISDRONE(ppdata) && (ppdata->but_cur == (P_A|P_C))) )
			&& !(pobj->flags & (PF_SHOOTING|PF_DUNKER|PF_ALLYDUNKER))
			&& game_info.ball_mode == BM_DRIBBLING
			&& ppdata->HangingOnRim == NO
			&& WhatKindOfShot(pobj) == DUNK	)
			{
				//p[0] = game_info.active_goal ? RIM_X : -RIM_X; p[1] = 0.0f; p[2] = 0.0f;
				//rim_bearing = (pobj->fwd - ptangle( &(pobj->x), p)) & 1023;
				//whichdunk = rand() % (sizeof(double_dunk_anims) / sizeof(int));
				//pobj->tgt_fwd = ptangle(&pobj->x, p);
				//change_anim( pobj, double_dunk_anims[whichdunk]);
				if(game_info.ball_handler != NO_BALLHANDLER
				&& player_blocks[game_info.ball_handler].odata.flags & (PF_DUNKER|PF_ALLYDUNKER|PF_LAYUP))			//y > 0.0f)
				{
				// double dunks look bad when to close to the rim
					if(get_dist_to_rim(ppdata) > 30.0f)
						Get_A_Double_Dunk(pobj);
					else
					{
						if(ISHUMAN(ppdata))
							flash_plyr_red(pobj, -1);
#ifdef DEBUG
						printf("%s is to close to rim for a DOUBLE DUNK\n", ppdata->p_spd->szlast);
#endif
					}	
#ifdef DEBUG
				printf("DOUBLE DUNK\n");
#endif
				}
				else
				{
					if(ISDRONE(ppdata))
					{
						if(drone_attempt_allyoop <= 0)
							Get_A_AllyOop(pobj);
					}
					else
						Get_A_AllyOop(pobj);
#ifdef DEBUG
				printf("ALLYOOP DUNK\n");
#endif
				}
			}
			// player doesn't have the ball
			else if(game_info.ball_handler == NO_BALLHANDLER)
			{
				if(pobj->y == 0.0f)
				{
					// did player press rebound button
					if ((ppdata->but_new & P_A) && (pobj->flags & PF_ONSCREEN))
					{
						if (ball_obj.odata.y > RIM_Y)
							change_anim(pobj, rebound_anims[randrng(sizeof(rebound_anims)/sizeof(char *))]);
						else if (ball_obj.odata.y > RIM_Y-10.f)
							change_anim(pobj, short_rebound_anims[randrng(sizeof(short_rebound_anims)/sizeof(char *))]);

						return YES;
					}
				}
			}
		}

// Shoot the ball if it is ok
		if((game_info.ball_handler == ppdata->plyrnum)
			//&& ((ppdata->but_cur == P_A) || (ppdata->but_cur == (P_A|P_C)))
			//&& ((ppdata->but_new & P_A))
			&& ((ppdata->but_cur & P_A))
			&& !(pobj->flags & PF_SHOOTING)
			&& ppdata->HangingOnRim == NO
		  )	//(ppdata->but_new & P_A) && !(ppdata->but_new & P_C)) {
		{
			//if(ISDRONE(ppdata)
			//{
				if(!ignore_shoot)
					do_shot(ppdata);
				else
				{
#ifdef DEBUG
					printf("ignoring a spastic fullcourt shot\n");
//					while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
#endif
				}
			//}
			//else
			//	do_shot(ppdata);
		}// if shot button was pressed and he is the ball handler

// dribbler should periodically switch hands when a defender is near his dribbling hand
#if 0
		if(game_info.ball_handler == ppdata->plyrnum
		&& ppdata->stick_time >= 10
		&& ppdata->stick_offcenter_time >= 30
		&& !(pobj->flags & PF_PLYRFLAGS)
		&& (pobj->adata[0].seq_index == (SEQ_D_RUN2_R) || pobj->adata[0].seq_index == (SEQ_D_RUN2_L))
		)
		{
		int joint = ppdata->ball_in_hand == RIGHT_HAND ? JOINT_RWRIST : JOINT_LWRIST;
		float p[3];

			get_joint_position(pobj, pobj->adata[0].seq_index, pobj->adata[0].iframe, joint, pobj->tgt_fwd, &p[0], &p[1], &p[3]);
			if(dist3d(&(player_blocks[nearest_to_ball(!ppdata->team)].odata.x), p) <= 15.0f)
			{
				change_anim(pobj, ppdata->ball_in_hand == RIGHT_HAND ? dribble_switch2_r : dribble_switch2_l);
			}
		}
#endif

// Shake and Bake was pressed
		if((game_info.ball_handler == ppdata->plyrnum)
			&& (ppdata->but_new == P_C && ppdata->non_turbo_time == -1)
			&& ppdata->dribble_mode >= DM_CAN_DRIBBLE
			&& ppdata->HangingOnRim == NO)
#if 1
		{
			if(ISDRONE(ppdata) || pdata_blocks[ppdata->plyrnum].turbo >= 35.0f)
			{

				if(!(game_info.fire_player == ppdata->plyrnum && game_info.fire_cnt >= FIRECNT) &&
					!(game_info.team_fire == player_blocks[ppdata->plyrnum].team) &&
					 ISHUMAN(ppdata))
				 	pdata_blocks[ppdata->plyrnum].turbo -= 35.0f;

				choose_shake_anim(ppdata);
				for(i = 0; i < NUM_PLAYERS; i++)// reset head turn defaults
					head_wait[i] = 0;
			}
		}  //do shake and bake dribbles
#else
		{
				choose_shake_anim(ppdata);
				for(i = 0; i < NUM_PLAYERS; i++)// reset head turn defaults
					head_wait[i] = 0;
		}  //do shake and bake dribbles
#endif
// Throw wild elbows
		if((game_info.ball_handler == ppdata->plyrnum)
			&& (ppdata->but_new == P_C && ppdata->non_turbo_time == -1)
			&& ppdata->dribble_mode == DM_CANT_DRIBBLE
			&& ppdata->HangingOnRim == NO
			&& (game_info.game_time & 0x00FFFF00) > 0x00000a00)
		{

			if(plyr_in_front_of_me(pobj))
			{
				if (randrng(100) < 20)
					change_anim(pobj, kick_anim);
				else
					change_anim(pobj, hit_dude_wb_anim);
			}
			else
				change_anim(pobj, throw_elbows_wb_2_anim);

			// no steals for a bit...
			ignore_steal = 140;
		}

#if 0
		if((game_info.passer == NO_PASSER)
			&& ppdata->plyrnum == game_info.ball_handler
			&& game_info.ball_handler != NO_BALLHANDLER
			&& (ppdata->but_new & P_D))
		{
			ChangePassReceiver();
		}
#endif
	}	// END.if !UNINT

	// passing can take place anytime if you have the ball
	i = -1;
	if((ppdata->plyrnum == game_info.ball_handler)

		// Don't allow a pass from mode reacting
		// We may have to allow passing if guy has been mode reacting for too long
		// Waiting for complete getup before a pass will suck!
//		&& !(pobj->adata[0].animode & MODE_REACTING)

#if NUM_PERTEAM > 2
		&& (ppdata->but_new & (P_B|P_D))
#else
		&& (ppdata->but_new & P_B)
#endif
		&& (!(pobj->flags & PF_PASSING))
		&& (ppdata->flail_time <= 0)
		)
	{
	int contpass = YES;

#if NUM_PERTEAM > 2
		if(ppdata->but_new & P_B) i = game_info.receiver_a;
		if(ppdata->but_new & P_D) i = game_info.receiver_b;
#else
		i = ppdata->plyrnum ^ 1;
#endif

//		if (pobj->adata[0].animode & MODE_REACTING)
//		{
//			pobj->vx = 0.0f;
//			pobj->vy = 0.0f;
//			pobj->vz = 0.0f;
//			pobj->y = 0.0f;			// put dude on ground
//		}

		if(ISDRONE(ppdata))
		{
			if(pobj->flags & (PF_PASSING|PF_FUNKY_DRIBBLE) && ISDRONE(&player_blocks[i]))
				contpass = NO;
			if(pobj->flags & (PF_FUNKY_DRIBBLE) && ISDRONE(&player_blocks[i]) == NO)
				contpass = YES;
		}

		if(i > -1 && ignore_pass == 0 && contpass == YES)
		{
			if (player_blocks[i].odata.flags & PF_ALLYDUNKER)
			{
				Get_A_Pass(pobj, i, YES);
			}
			else
			{
				Get_A_Pass(pobj, i, NO);
			}
		}
	}

#ifdef DEBUG
	if(halt && ppdata->plyrnum == 0 && (pobj->flags & PF_ALLYDUNKER))
	{
	//	printf("ball_handler %d, passer %d, who_shot %d\nball->y %f, pobj->y %f\n",game_info.ball_handler, game_info.passer, ball_obj.who_shot,ball_obj.odata.y,pobj->y);
			int limb = ppdata->ball_in_hand == RIGHT_HAND ? JOINT_RWRIST : JOINT_LWRIST;
			float hx =	ppdata->jpos[limb][0] * cambot.xform[ 0] +
						ppdata->jpos[limb][1] * cambot.xform[ 4] +
						ppdata->jpos[limb][2] * cambot.xform[ 8] + cambot.x;
			float hy =	ppdata->jpos[limb][0] * cambot.xform[ 1] +
						ppdata->jpos[limb][1] * cambot.xform[ 5] +
						ppdata->jpos[limb][2] * cambot.xform[ 9] + cambot.y;
			float hz =	ppdata->jpos[limb][0] * cambot.xform[ 2] +
						ppdata->jpos[limb][1] * cambot.xform[ 6] +
						ppdata->jpos[limb][2] * cambot.xform[10] + cambot.z;
			float dist = fsqrt( ((ball_obj.odata.x - hx) * (ball_obj.odata.x - hx)) +
								((ball_obj.odata.y - hy) * (ball_obj.odata.y - hy)) +
								((ball_obj.odata.z - hz) * (ball_obj.odata.z - hz))
								);
			printf(
					"ball_handler %d, passer %d, who_shot %d\nball->y %f, pobj->y %f, ball_mode %d, dist %f\n"
#if NUM_PERTEAM > 2
					"receiver_a %d, receiver_b %d, "
#endif
					"int_receiver %d\n",
					game_info.ball_handler, game_info.passer, ball_obj.who_shot,
					ball_obj.odata.y, pobj->y, game_info.ball_mode, dist,
#if NUM_PERTEAM > 2
					game_info.receiver_a, game_info.receiver_b,
#endif
					ball_obj.int_receiver );

#if 0
			pobj->x = dunkfx - FRELX(dunkoffx,dunkoffz,pobj->tgt_fwd);
			pobj->y = dunkfy - dunkoffy;
			pobj->z = dunkfz - FRELZ(dunkoffx,dunkoffz,pobj->tgt_fwd);
			printf("x = %f, y = %f, z = %f\nhx = %f, hy = %f, hz = %f\n",pobj->x,pobj->y,pobj->z,hx,hy,hz);
#endif
	}
#endif

	// Collisions with the ball
	{
		float * cm = cambot.xform;

		// transform target x,y,z into world coords
		i = ppdata->ball_in_hand == RIGHT_HAND ? JOINT_RWRIST : JOINT_LWRIST;

		p[0] =	ppdata->jpos[i][0] * cm[ 0] +
				ppdata->jpos[i][1] * cm[ 4] +
				ppdata->jpos[i][2] * cm[ 8] + cambot.x;
		p[1] =	ppdata->jpos[i][0] * cm[ 1] +
				ppdata->jpos[i][1] * cm[ 5] +
				ppdata->jpos[i][2] * cm[ 9] + cambot.y;
		p[2] =	ppdata->jpos[i][0] * cm[ 2] +
				ppdata->jpos[i][1] * cm[ 6] +
				ppdata->jpos[i][2] * cm[10] + cambot.z;

		if((pobj->flags & PF_ALLYDUNKER) || (pobj->flags & PF_DUNKER))
		{
			// is ball close enough to hand...
			if(dist3d(&(ball_obj.odata.x), p) <= 6.50f)
			{
				close = YES;
			}
		}
		else
		if(pobj->flags & PF_CATCHING)
		{
			// is ball close enough to hand...
			if(dist3d(&(ball_obj.odata.x), p) <= 5.0f)
				close = YES;

			if(pobj->flags & PF_BOTH_HANDS)
			{
				i = ppdata->ball_in_hand == RIGHT_HAND ? JOINT_LWRIST : JOINT_RWRIST;

				p[0] +=	ppdata->jpos[i][0] * cm[ 0] +
						ppdata->jpos[i][1] * cm[ 4] +
						ppdata->jpos[i][2] * cm[ 8] + cambot.x;
				p[1] +=	ppdata->jpos[i][0] * cm[ 1] +
						ppdata->jpos[i][1] * cm[ 5] +
						ppdata->jpos[i][2] * cm[ 9] + cambot.y;
				p[2] +=	ppdata->jpos[i][0] * cm[ 2] +
						ppdata->jpos[i][1] * cm[ 6] +
						ppdata->jpos[i][2] * cm[10] + cambot.z;

				p[0] *= 0.5f;
				p[1] *= 0.5f;
				p[2] *= 0.5f;

				if(dist3d(&(ball_obj.odata.x), p) <= 5.0f)
					close = YES;
			}

			p[0] =	ppdata->jpos[JOINT_PELVIS][0] * cm[ 0] +
					ppdata->jpos[JOINT_PELVIS][1] * cm[ 4] +
					ppdata->jpos[JOINT_PELVIS][2] * cm[ 8] + cambot.x;
			p[1] =	ppdata->jpos[JOINT_PELVIS][0] * cm[ 1] +
					ppdata->jpos[JOINT_PELVIS][1] * cm[ 5] +
					ppdata->jpos[JOINT_PELVIS][2] * cm[ 9] + cambot.y;
			p[2] =	ppdata->jpos[JOINT_PELVIS][0] * cm[ 2] +
					ppdata->jpos[JOINT_PELVIS][1] * cm[ 6] +
					ppdata->jpos[JOINT_PELVIS][2] * cm[10] + cambot.z;

			if(dist3d(&(ball_obj.odata.x), p) <= 7.0f)
				close = YES;

			p[0] =	ppdata->jpos[JOINT_TORSO][0] * cm[ 0] +
					ppdata->jpos[JOINT_TORSO][1] * cm[ 4] +
					ppdata->jpos[JOINT_TORSO][2] * cm[ 8] + cambot.x;
			p[1] =	ppdata->jpos[JOINT_TORSO][0] * cm[ 1] +
					ppdata->jpos[JOINT_TORSO][1] * cm[ 5] +
					ppdata->jpos[JOINT_TORSO][2] * cm[ 9] + cambot.y;
			p[2] =	ppdata->jpos[JOINT_TORSO][0] * cm[ 2] +
					ppdata->jpos[JOINT_TORSO][1] * cm[ 6] +
					ppdata->jpos[JOINT_TORSO][2] * cm[10] + cambot.z;

			if(dist3d(&(ball_obj.odata.x), p) <= 7.0f)
				close = YES;
		}
		else
		{
			// Here was a duplicate transform of the one done at the top
			// of this ball collisions distancing block!

			// is ball close enough to hand...
			if(pobj->flags & PF_BOTH_HANDS)
			{
				i = ppdata->ball_in_hand == RIGHT_HAND ? JOINT_LWRIST : JOINT_RWRIST;

				p[0] +=	ppdata->jpos[i][0] * cm[ 0] +
						ppdata->jpos[i][1] * cm[ 4] +
						ppdata->jpos[i][2] * cm[ 8] + cambot.x;
				p[1] +=	ppdata->jpos[i][0] * cm[ 1] +
						ppdata->jpos[i][1] * cm[ 5] +
						ppdata->jpos[i][2] * cm[ 9] + cambot.y;
				p[2] +=	ppdata->jpos[i][0] * cm[ 2] +
						ppdata->jpos[i][1] * cm[ 6] +
						ppdata->jpos[i][2] * cm[10] + cambot.z;

				p[0] *= 0.5f;
				p[1] *= 0.5f;
				p[2] *= 0.5f;

				if(dist3d(&(ball_obj.odata.x), p) <= 5.0f)
					close = YES;
			}

			if(dist3d(&(ball_obj.odata.x), &(ppdata->odata.x)) <= 5.0f)
				close = YES;

			p[0] = pobj->x;
			p[1] = pobj->y + pobj->ay;
			p[2] = pobj->z;

			// check to see if ball is close enough to pelvis
			if(dist3d(&(ball_obj.odata.x), p) <= 12.0f)
				close = YES;

			// Here was a bogus 2d distancing that set close=YES, causing
			// ball-snap from arbitrary Y position

			if(pobj->flags & PF_CELEBRATE)
			{
				close = NO;
				//printf("PLAYER IS CELEBRATE\n");
			}
		}
	}

	// last few seconds of a period a PF_ALLYDUNKER can't connect with a BM_SHOOTING ball...
	if((min == 0 && sec < 5)
	&& game_info.ball_mode == BM_SHOOTING
	&& pobj->flags & PF_ALLYDUNKER
	&& ball_obj.who_shot == (ppdata->plyrnum ^ 1)
	&& ball_obj.int_receiver != ppdata->plyrnum
	&& ball_obj.flight_time < ball_obj.total_time)
		ppdata->no_ball_collide = (ball_obj.total_time - ball_obj.flight_time) + 12;

	if((game_info.ball_handler == NO_BALLHANDLER
		&& (pobj->flags & PF_ALLYDUNKER || pobj->flags & PF_DUNKER)
		&& !(pobj->adata[0].animode & MODE_REACTING)
		&& ppdata->no_ball_collide <= 0))
	{
		//if(close == YES || ball_hit_me(pobj) == YES)	//dist <= 7.0f)	//(BALL_RADIUS*4.0f))	// within ? feet
		if(close == YES)
		{
#if 0
// This #if'd block is not fixed for A/B passing buttons or 4/6 player stuff!
			if(game_info.receiver_a == -1)
			{
				game_info.receiver_a = ((ppdata->plyrnum + 1) % NUM_PERTEAM) + (NUM_PERTEAM * ppdata->team);
			}
			else if(ball_obj.int_receiver == ppdata->plyrnum)
			{
				if(game_info.passer != -1)
				{
					game_info.receiver_a = game_info.passer;
					game_info.receiver_a = (game_info.receiver_a % NUM_PERTEAM) + (NUM_PERTEAM * ppdata->team);

#if 0
					if(game_info.ball_handler != NO_BALLHANDLER && game_info.ball_handler == game_info.receiver_a)
					{
						game_info.receiver_a = ((game_info.receiver_a + 1) % NUM_PERTEAM) + (NUM_PERTEAM * ppdata->team);
					}

					if(ball_obj.who_threw != ball_obj.int_receiver && game_info.plyr_control[ball_obj.who_threw] != -1)
					{
						game_info.plyr_control[ball_obj.int_receiver] = game_info.plyr_control[ball_obj.who_threw];
						game_info.plyr_control[ball_obj.who_threw] = -1;
					}
#endif
				}
				else
				{
					game_info.receiver_a = ((ppdata->plyrnum + 1) % NUM_PERTEAM) + (NUM_PERTEAM * ppdata->team);
					if(ISDRONETEAM(ppdata) == NO)
					{
						for(i = ppdata->team * 3; i < 3 + ppdata->team * 3; i++)
						{
							if(i == ppdata->plyrnum) continue;
							if(ISDRONE(&player_blocks[i]) == YESS)
								continue;
							else
								game_info.receiver_a = i;
						}
					}	
				}
			}
#endif
//			printf("ALLY: ball was passed from a human, passer %d act_receiver %d\n",ball_obj.who_threw, ball_obj.int_receiver);
#if NUM_PERTEAM > 2
			// Reset some essentials
			if (!((game_info.receiver_a = ppdata->plyrnum + 1) % NUM_PERTEAM))
				game_info.receiver_a -= NUM_PERTEAM;
			if (!((game_info.receiver_b = game_info.receiver_a + 1) % NUM_PERTEAM))
				game_info.receiver_b -= NUM_PERTEAM;
			if (ISDRONE(&player_blocks[game_info.receiver_a]) &&
				ISHUMAN(&player_blocks[game_info.receiver_b]))
			{
				i = game_info.receiver_a;
				game_info.receiver_a = game_info.receiver_b;
				game_info.receiver_b = i;
			}
#endif
			what_type_shot = three_pt_check(pobj) ? JUMPER_3 : JUMPER_2;

			game_info.ball_handler = ppdata->plyrnum;

			ppdata->launched_from_hotspot = 0;

			//game_info.game_mode    = GM_IN_PLAY;
			game_info.passer       = NO_PASSER;
			game_info.ball_mode    = BM_DRIBBLING;

			ball_obj.who_shot = ppdata->plyrnum;	// player who shot ball can get it now.
			ball_obj.flags   &= ~BF_NOINTERCEPT;	// make sure this flag is cleared
			ball_obj.odata.vx = ball_obj.odata.vy = ball_obj.odata.vz = 0.0f;

			no_pump_fake = YES;
			
			if (game_info.game_mode == GM_INBOUNDING)
				clock_active = 1;	   // Restart game clock

	// crowd says "oh!"
			//snd_scall_bank(crowd_bnk_str, CROWD_OH_SND, VOLUME3, 127, PRIORITY_LVL2);
			if(pobj->flags & (PF_REBOUND|PF_ALLYDUNKER|PF_DUNKER))
			{
				// swat ball sound
				snd_scall_bank(gameplay_bnk_str,STEAL1_SND,VOLUME7,127,PRIORITY_LVL4);
				flash_ball(4);	// flash the ball
			}
		}
	}
	else // If ball is loose and i'm close...get it
	{

		if((pobj->flags & PF_REBOUND)
		&& (ppdata->no_ball_collide <= 0)
		&& (OOBPasser == NO_PASSER))
		{
			rebound_logic(ppdata);
		}

		if((game_info.ball_handler == NO_BALLHANDLER
				&& !(pobj->adata[0].animode & MODE_REACTING)
				&& ppdata->plyrnum != game_info.passer
				&& !(pobj->flags & PF_DUNKER)
				&& !(pobj->flags & PF_ALLYDUNKER)
				&& (game_info.ball_mode != BM_PASSING))
			|| ((game_info.ball_mode == BM_PASSING && ball_obj.int_receiver == ppdata->plyrnum)
				&& !(pobj->adata[0].animode & MODE_REACTING)
				&& game_info.ball_handler == NO_BALLHANDLER
				&& ppdata->plyrnum != game_info.passer
				&& !(pobj->flags & PF_DUNKER)
				&& !(pobj->flags & PF_ALLYDUNKER))
			)
		{
			if(ppdata->no_ball_collide <= 0 && (close == YES || ball_hit_me(pobj) == YES))
			{
				auto_rebounding = NO;
				//if( ((ball_obj.odata.y >= (pobj->y + pobj->ay - 4.0f)) && (ball_obj.odata.y <= (pobj->y + pobj->ay + 20.0f )))
				//		|| (pobj->y == 0.0f && ball_obj.odata.y <= 12.0f)
//			//			|| (pobj->flags & PF_REBOUND 
				//		|| (ball_obj.odata.y >= (pobj->y + pobj->ay - 4.0f)
				//			&& ball_obj.odata.y <= (pobj->y + pobj->ay + 20.0f)
				//			&& odistxz(pobj, (obj_3d *)&ball_obj) < 8.0f)
				//	)
				//{
//					if(pobj->flags & PF_REBOUND)
//					{
//						// Reset some essentials
//						auto_rebounding 			 = NO;
//						game_info.passer    = NO_PASSER;
//						game_info.ball_mode = BM_DRIBBLING;
//
//						ball_obj.who_shot = NO_SHOOTER;
//						ball_obj.flags   &= ~BF_NOINTERCEPT;
//
//						ppdata->dribble_mode = DM_CAN_DRIBBLE;
//						no_pump_fake = YES;
//						
//						ball_obj.odata.vx = ball_obj.odata.vz = 0.0f;
//						ball_obj.odata.y = BALL_RADIUS;
//						ppdata->no_ball_collide = 7;
//						game_info.ball_handler = ppdata->plyrnum;
//						//flash_ball(2);
//						snd_scall_bank(gameplay_bnk_str,STEAL1_SND,VOLUME1,127,PRIORITY_LVL4);
//						if ((pobj->adata[0].seq_index == (SEQ_RB1_MIS)) ||
//							(pobj->adata[0].seq_index == (SEQ_RB3_MIS)))
//							change_anim(pobj, rebound5_grab_anim);
//						else if (pobj->adata[0].seq_index == (SEQ_RB2_MIS))
//							change_anim(pobj, rebound2_grab_anim);
//						else
//							change_anim(pobj, rebound6_grab_anim);
//						return 1;
//					}
					if(ball_obj.odata.y <= 6.0f
					&& pobj->adata[0].seq_index != (SEQ_PICKUP_1)
					&& pobj->y == 0.0f
					&& fabs(ball_obj.odata.vy) <= 0.7f
					&& pobj->ay >= 3.0f)
					{
						auto_rebounding = NO;
						ball_obj.odata.vx = ball_obj.odata.vz = ball_obj.odata.vy = 0.0f;
						ball_obj.odata.y = BALL_RADIUS;
						ppdata->no_ball_collide = 6;
						ppdata->odata.tgt_fwd = ptangle( &(ppdata->odata.x), &(ball_obj.odata.x));
						change_anim( pobj, ball_pickup_1 );
						what_type_shot = three_pt_check(pobj) ? JUMPER_3 : JUMPER_2;
						no_pump_fake = YES;
						return 1;
					}

//					printf("ball was passed from a human, passer %d act_receiver %d\n",ball_obj.who_threw, ball_obj.int_receiver);
#ifdef DEBUG
					//if (ppdata->plyrnum == ball_obj.int_receiver && !(pobj->flags & PF_CATCHING))
					//	printf("ERROR: plyr %d has ball and is receiver\n",ppdata->plyrnum);
#endif
					// Grab the ball
					game_info.ball_handler = ppdata->plyrnum;

					if(game_info.game_mode == GM_INBOUNDING)
						clock_active = 1;	   // Restart game clock

					game_info.game_mode    = GM_IN_PLAY;

					// Reset some essentials
					auto_rebounding  	= NO;
					game_info.passer    = NO_PASSER;
					game_info.ball_mode = BM_DRIBBLING;

					//ball_obj.who_shot = NO_SHOOTER;
					ball_obj.flags   &= ~BF_NOINTERCEPT;

					ppdata->dribble_mode = DM_CAN_DRIBBLE;
					no_pump_fake = YES;

					if(pobj->flags & (PF_REBOUND|PF_ALLYDUNKER))
					{
						// swat ball sound
						snd_scall_bank(gameplay_bnk_str,STEAL1_SND,VOLUME7,127,PRIORITY_LVL4);
						flash_ball(4);
					}
	
					what_type_shot = three_pt_check(pobj) ? JUMPER_3 : JUMPER_2;

//		if(ISDRONE(ppdata) && plyr_crossed_halfcourt(ppdata->plyrnum) == YES)
//			rob = YES;

				//}
			}
		}
	}

#ifdef DEBUG_EXTREME
	for(i = 0; i < ppdata->plyrnum; i++) fprintf(stderr, " ");
	fprintf(stderr, "LEAVE: _mode_offense %d\n",ppdata->plyrnum);
#endif
	return 1;
}

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
void do_shot(bbplyr_data *ppdata)
{
obj_3d *pobj = (obj_3d *)ppdata;
int i, angle, rim_bearing, quad8, quad4;
float p[3], dist_to_rim;

// default to looking at rim...
	head_wait[ppdata->plyrnum] = 1, look[ppdata->plyrnum] = -3;

// player just forfited his right to dribble the ball
// !!!moved to end of routine!!!
//	if(ppdata->dribble_mode == DM_CANT_DRIBBLE || ppdata->dribble_mode == DM_DRIBBLING)
//		ppdata->dribble_mode = DM_CANT_DRIBBLE;

// see where player is at...2,3, or dunk?...if a jump shot determine what type...
// if player is in a dunking range...and headed toward the basket...
// see who's in front of him and where...pick a dunk
// ...SHOWTIME! ! !
	p[0] = game_info.active_goal ? RIM_X : -RIM_X; p[1] = 0.0f; p[2] = 0.0f;
	rim_bearing = (pobj->fwd - ptangle( &(pobj->x), p)) & 1023;
//rim_bearing = (rim_bearing > 512) ? 1024 - rim_bearing : rim_bearing;
	what_type_shot = WhatKindOfShot(pobj);
	if(auto_layup)
	{
		auto_layup = 0;
		what_type_shot = LAYUP;
	}
	whichshot = rand() % (sizeof(shot_anims) / sizeof(int));

	//if(whichshot > 2 && ppdata->_shooting >= 7 && randrng(100) <= 15);
	//else whichshot = randrng(3);

	//whichshot = 5;

	p[0] = game_info.active_goal ? RIM_CX : -RIM_CX; p[1] = 0.0f; p[2] = 0.0f;
	dist_to_rim = dist3d(&(pobj->x), p);

// quad8 works as follows:
//	0-behind, 1-left.behind, 2-left, 3-left.front
//	4-front, 5-right.front, 6-right, 7-right.behind
	quad8 = (p[0] < 0.0f ? 768 : 256) - ptangle(&(pobj->x), p);
	quad4 = ((quad8 + 1536 + 128)& 1023) / 256;		// four directions
	quad8 = ((quad8 + 1536 + 64) & 1023) / 128;		// eight directions

	if(ppdata->stick_cur == 24)
	{
		pobj->vx = pobj->vz = 0.0f;
	}

	switch(what_type_shot)
	{
	case JUMPER_2:
	// was this player cocky dribbling...
		if(pobj->adata[0].seq_index == (SEQ_D_COCK_1) || pobj->adata[0].seq_index == (SEQ_D_COCK_2))
			ticks_since_cocky = -COCKY_DRIBBLE_TWAIT, cocky_dribbler = NO_BALLHANDLER;

// now determine what kind of jumpshot
		if(1)
		{
	//whichshot = 0;

			// FIX!!!  Hook shots are too hard to trigger
			if(((pobj->fwd >= 484 && pobj->fwd <= 540) || (pobj->fwd >= 996 || pobj->fwd <= 28))
			&& (dist_to_rim >= 30.0f && dist_to_rim <= 65.0f)
			&& (fabs(pobj->x) >= 64.0f && fabs(pobj->x) <= 80.0f)
			&& (pobj->vx != 0.0f || pobj->vz != 0.0f)
			)
			{	// shoot the hook shot
//						change_anim( pobj, shot_hook_1 );
				change_anim( pobj, shot_hook_3 );
				//printf("JUMPER_2\n");
			}
			else
			{
				if(pobj->vx == 0.0f && pobj->vz == 0.0f)
				{
					if(dist_to_rim <= 16.0f)
					{
						if(plyr_behind_backboard(pobj) == YES)
						{
							pobj->tgt_fwd = angle = 1024 - ptangle(&pobj->x, p);
							change_anim(pobj, shot_behind_rim_anim);
						}
						else// if((quad8 == Q8_FRONT || quad8 == Q8_RIGHT_FRONT || quad8 == Q8_LEFT_FRONT || quad8 == Q8_LEFT || quad8 == Q8_RIGHT))
						{
							pobj->tgt_fwd = get_angle_to_rim(ppdata);
							Get_A_Dunk(pobj);
						}
					}
					else
					{
						ppdata->ball_in_hand = ppdata->handedness;
						pobj->tgt_fwd = get_angle_to_rim(ppdata);

						if((game_info.active_goal == 0) && (pobj->x <= -RIM_X))
						{	// baseline shot
							if(pobj->z <= 0.0f)
								change_anim( pobj, shot_leaner_3_l_anim);
							else
								change_anim( pobj, shot_leaner_3_r_anim);
						}
						else if((game_info.active_goal == 1) && (pobj->x >= RIM_X))
						{	// baseline shot
							if(pobj->z <= 0.0f)
								change_anim( pobj, shot_leaner_3_r_anim);
							else
								change_anim( pobj, shot_leaner_3_l_anim);
						}
						else
						{
							if (ppdata->dribble_mode == DM_CANT_DRIBBLE)
								change_anim(pobj, shot_stand1_anim);
							else
								change_anim( pobj, shot_anims[whichshot]);
						}
					}
				}
				else
				{	// player was moving when shot button was pressed
					if(randrng(100) <= 50)
						shoe_squeak(pobj);

					if(plyr_crossed_halfcourt(game_info.ball_handler) == YES)
					{
						pobj->tgt_fwd = angle = ptangle(&pobj->x, p);

						if(dist_to_rim <= 16.0f)
						{
							if (plyr_behind_backboard(pobj) == YES)
							{
								pobj->tgt_fwd = angle = 1024 - ptangle(&pobj->x, p);
								change_anim(pobj, shot_behind_rim_anim);
							}
							else if ((quad8 == Q8_FRONT || quad8 == Q8_RIGHT_FRONT || quad8 == Q8_LEFT_FRONT || quad8 == Q8_LEFT || quad8 == Q8_RIGHT))
							{
								pobj->tgt_fwd = get_angle_to_rim(ppdata);
								Get_A_Dunk(pobj);
							}
						}

						// Check for running one handed floating jump shots
						else if((dist_to_rim >= 30.0f && dist_to_rim <= 80.0f)
						&& (quad8 >= Q8_LEFT_FRONT && quad8 <= Q8_RIGHT_FRONT)
						&& ((rim_bearing >= 160 && rim_bearing <= 300/* && ppdata->handedness == LEFT_HAND && ppdata->ball_in_hand == LEFT_HAND*/)
							|| (rim_bearing >= 672 && rim_bearing <= 812/* && ppdata->handedness == RIGHT_HAND && ppdata->ball_in_hand == RIGHT_HAND*/))
						&& ((rim_bearing >= 160 && rim_bearing <= 300)
						|| (rim_bearing >= 672 && rim_bearing <= 812)) )
						{
							ppdata->ball_in_hand = RIGHT_HAND;
							if(rim_bearing >= 160 && rim_bearing <= 300)
								ppdata->ball_in_hand = LEFT_HAND;

							pobj->vx *= SHOT_LEANER_VEL_DAMPEN;
							pobj->vz *= SHOT_LEANER_VEL_DAMPEN;
							change_anim( pobj, shot_run_leaner_1);
							//printf("JUMPER_2\n");
						}
						else
						if((dist_to_rim >= FREE_THROW_LINE_X)
						&& ((rim_bearing >= 0 && rim_bearing <= 50) || (rim_bearing >= 974 && rim_bearing <= 1024))	)
						{
							if(randrng(100) <= 50)
								change_anim( pobj, shot_run_leaner_frwd_3);
							else
								change_anim( pobj, shot_run_leaner_frwd_2);
								//change_anim( pobj, shot_run_leaner_frwd);
							//printf("JUMPER_2: RUNNING ONE HANDER TO RIM\n");
						}
						else
						{ // check for the fade away
							ppdata->ball_in_hand = ppdata->handedness;
							if(rim_bearing >= (512 - FADE_ANGLE_TOLERANCE) && rim_bearing <= (512 + FADE_ANGLE_TOLERANCE))
								change_anim( pobj, randrng(100) < 50 ? shot_fade1_anim : shot_fade2_anim );// fade away
							else
								change_anim( pobj, shot_anims[whichshot]);
						}
					}
					else
					{
						pobj->tgt_fwd = get_angle_to_rim(ppdata);
						change_anim( pobj, shot_run_heave_1);
					}
				}
			}
		}
		break;
	case JUMPER_3:
	// was this player cocky dribbling...
		if(pobj->adata[0].seq_index == (SEQ_D_COCK_1) || pobj->adata[0].seq_index == (SEQ_D_COCK_2))
			ticks_since_cocky = -COCKY_DRIBBLE_TWAIT, cocky_dribbler = NO_BALLHANDLER;
// now determine what kind of jumpshot
		if(1)
		{
			//whichshot = 0;
			if(pobj->vx == 0.0f && pobj->vz == 0.0f)
			{
				p[0] = game_info.active_goal ? RIM_X : -RIM_X; p[1] = 32.0f; p[2] = 0.0f;
				//pobj->tgt_fwd = angle = ptangle( &pobj->x, p);
			 	pobj->tgt_fwd = get_angle_to_rim(ppdata);
				ppdata->ball_in_hand = ppdata->handedness;
				if(plyr_crossed_halfcourt(game_info.ball_handler) == YES)
					change_anim( pobj, shot_anims[whichshot]);
				else
				 	change_anim( pobj, shot_run_heave_1 );
			}
			else
			{
				if(randrng(100) <= 50)
					shoe_squeak(pobj);

				p[0] = game_info.active_goal ? RIM_X : -RIM_X; p[1] = 32.0f; p[2] = 0.0f;
				//pobj->tgt_fwd = angle = ptangle( &pobj->x, p);
				pobj->tgt_fwd = get_angle_to_rim(ppdata);
				if(plyr_crossed_halfcourt(game_info.ball_handler) == YES)
				{
			// Check for running one handed floating jump shots
					if((dist_to_rim >= 30.0f && dist_to_rim <= 80.0f)
					&& (quad8 >= Q8_LEFT_FRONT && quad8 <= Q8_RIGHT_FRONT)
					&& ((rim_bearing >= 160 && rim_bearing <= 300 /*&& ppdata->handedness == LEFT_HAND && ppdata->ball_in_hand == LEFT_HAND*/)
						|| (rim_bearing >= 672 && rim_bearing <= 812 /*&& ppdata->handedness == RIGHT_HAND && ppdata->ball_in_hand == RIGHT_HAND*/))
					&& ((rim_bearing >= 160 && rim_bearing <= 300)
					|| (rim_bearing >= 672 && rim_bearing <= 812))
					)
					{
						ppdata->ball_in_hand = RIGHT_HAND;
						if(rim_bearing >= 160 && rim_bearing <= 300)
							ppdata->ball_in_hand = LEFT_HAND;

						pobj->vx *= SHOT_LEANER_VEL_DAMPEN;
						pobj->vz *= SHOT_LEANER_VEL_DAMPEN;
						change_anim( pobj, shot_run_leaner_1);
						//printf("JUMPER_3: RUNNING ONE HANDER\n");
					}
					else
					if((dist_to_rim >= FREE_THROW_LINE_X)
					&& ((rim_bearing >= 0 && rim_bearing <= 50) || (rim_bearing >= 974 && rim_bearing <= 1024))
					)
					{
						if(randrng(100) <= 50)
							change_anim( pobj, shot_run_leaner_frwd_3);
						else
							change_anim( pobj, shot_run_leaner_frwd_2);
							//change_anim( pobj, shot_run_leaner_frwd);
						//printf("JUMPER_3: RUNNING ONE HANDER TO RIM\n");
					}
					else
					{ // check for the fade away
						ppdata->ball_in_hand = ppdata->handedness;
						if(rim_bearing >= 480 && rim_bearing <= 615)
						{
							change_anim( pobj, randrng(100) < 50 ? shot_fade1_anim : shot_fade2_anim ); // fade away
							//printf("FADE AWAY 2\n");
						}
						else
						{
							change_anim( pobj, shot_anims[whichshot]);
							//printf("JUMPER_3: 2\n");
						}
					}
				}
				else
				{
					change_anim( pobj, shot_run_heave_1 );
				}
			}
		}
		break;
	case LAYUP:
		change_anim(pobj, quick_dunk_layup_1);
		break;
	case DUNK:
	// was this player cocky dribbling...
		if(pobj->adata[0].seq_index == (SEQ_D_COCK_1) || pobj->adata[0].seq_index == (SEQ_D_COCK_2))
			ticks_since_cocky = -COCKY_DRIBBLE_TWAIT, cocky_dribbler = NO_BALLHANDLER;
#ifdef DEBUG_TEST_DUNKS
					pobj->tgt_fwd = get_angle_to_rim(ppdata);
					Get_A_Dunk(pobj);
					break;
#endif
#ifdef DEBUG_TEST_LAYUPS
					pobj->tgt_fwd = ptangle(&pobj->x, p);
					Get_A_LayUp(pobj);
					break;
#endif
		if(((pobj->fwd >= 484 && pobj->fwd <= 540) || (pobj->fwd >= 996 || pobj->fwd <= 28))
		&& (fabs(dist_to_rim) >= 40.0f && fabs(dist_to_rim) <= 60.0f)
		&& (fabs(pobj->x) >= 64.0f && fabs(pobj->x) <= 80.0f)
		&& (pobj->vx != 0.0f || pobj->vz != 0.0f)
		)
		{
			change_anim( pobj, shot_hook_3);
			//printf("DUNK - hook\n");
		}
		else
		{
			// Check for running one handed floating jump shots
			if((dist_to_rim <= 85.0f)
			&& ((rim_bearing >= 160 && rim_bearing <= 300)
			|| (rim_bearing >= 672 && rim_bearing <= 812)) )
			{
				ppdata->ball_in_hand = RIGHT_HAND;
				if(rim_bearing >= 160 && rim_bearing <= 300)
					ppdata->ball_in_hand = LEFT_HAND;

				//pobj->tgt_fwd = ptangle(&pobj->x, p);
				pobj->tgt_fwd = get_angle_to_rim(ppdata);
				pobj->vx *= SHOT_LEANER_VEL_DAMPEN;
				pobj->vz *= SHOT_LEANER_VEL_DAMPEN;
				change_anim( pobj, shot_run_leaner_1);
				//printf("DUNK -- leaner\n");
			}
			else
			{ 	// check for the dunk
				// can this player dunk the ball?  Or is he allowed to dunk depending on situation
				//!!!FIX

				if((game_info.active_goal == 0) && (pobj->x <= -RIM_X) && fabs(pobj->z) <= 30.0f)
				{
#ifdef DEBUG
				printf("****** shot_leanear 666\n");
#endif
					if(pobj->z <= 0.0f)
						change_anim( pobj, shot_leaner_3_l_anim);
					else
						change_anim( pobj, shot_leaner_3_r_anim);
				}
				else if((game_info.active_goal == 1) && (pobj->x >= RIM_X) && fabs(pobj->z) <= 30.0f)
				{
#ifdef DEBUG
				printf("****** shot_leaner 666\n");
#endif
					if(pobj->z <= 0.0f)
						change_anim( pobj, shot_leaner_3_r_anim);
					else
						change_anim( pobj, shot_leaner_3_l_anim);
				}

//				else if(randrng(50) <= (int)(48.0f * (float)ppdata->_dunking / 20.0f))
				// If weak dunker, do layups
				else if ((ppdata->_dunking > 3) && (randrng(100) > 8))
				{
					pobj->tgt_fwd = get_angle_to_rim(ppdata);
// Must be running at rim for x ticks before a dunk is allowed!
					if((ppdata->but_cur == (P_A|P_C) || ISDRONE(ppdata))
						&& ppdata->stick_time > 3)


					// now determine what kind of dunk to do
						Get_A_Dunk(pobj);
					else
						change_anim(pobj, shot_anims[whichshot]);

//					if(randrng(100) <= 100)
						shoe_squeak(pobj);
				}
				else	// a layup
				{
					pobj->tgt_fwd = get_angle_to_rim(ppdata);
					if (dist_to_rim < 30.0f)		//25
					{
#ifdef DEBUG
//						printf("----- Shot behind rim 3\n");
//						printf("Will try dunk\n");
//						while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
#endif
						Get_A_Dunk(pobj);
  //					change_anim(pobj, shot_anims[whichshot]);
						//pobj->tgt_fwd = get_angle_to_rim(ppdata);
						//change_anim(pobj, shot_behind_rim_anim);
					}
					else
					{
						Get_A_LayUp(pobj);
					}
				}
			}
		}
		break;
	}// END.switch

	// player just forfited his right to dribble the ball
	if(ppdata->dribble_mode == DM_CANT_DRIBBLE || ppdata->dribble_mode == DM_DRIBBLING)
		ppdata->dribble_mode = DM_CANT_DRIBBLE;
}

//*********************************************************
#define D_SHORT			0x100
#define D_MED 			0x200
#define D_FAR  			0x400
#define D_ANYWHERE		0x800			// mainly for debugging
#define D_ATTRIB_LVL1	0x1000		// 0-3
#define D_ATTRIB_LVL2	0x2000		// 4-8
#define D_ATTRIB_LVL3	0x4000		// 9-15
#define D_ATTRIB_LVL4	0x8000		// 16-19
#define D_INVERT		0x10000		// inverts the contact angle...256 becomes 768 etc...
#define D_RIGHT_HAND	0x20000		// BALL HAS TO BE IN RIGHT HAND
#define D_LEFT_HAND		0x40000		// BALL HAS TO BE IN LEFT HAND
#define D_RIM_FRONT		0x80000		// auto send dunker to the front of rim
#define D_RIM_RIGHT		0x100000		// auto send dunker to the right of rim
#define D_RIM_LEFT		0x200000	// auto send dunker to the left of rim
#define D_BALL_TRAIL	0x400000	// auto send dunker to the left of rim
#define D_ATTRIB_ALL	(D_ATTRIB_LVL1|D_ATTRIB_LVL2|D_ATTRIB_LVL3|D_ATTRIB_LVL4)

#define	D_DIST_SHORT	18.0f		//15
#define	D_DIST_FAR		40.0f		//35

struct _Dunk
{
	void	*dunk;			// dunk script
	int		pcnt;			// percent of dunk actually happening
	int		flags;			// exclusions???
};

struct _Dunk dunks[] =
{
#if 1
// FAR range dunks
	{&dunk_kwin2b, 			33,	D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT},
	{&dunk_kwin2a,			33,	D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT},
	{&dunk_kwin2,			33,	D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT},
	{&dunk_combined_1,		100,D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_combined_2,		100,D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_combined_3,		100,D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_RIGHT | D_BALL_TRAIL},
	{&dunk_combined_4,		100,D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_combined_5,		30,	D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT | D_BALL_TRAIL},
	{&dunk_combined_6,		10,	D_FAR | D_ATTRIB_ALL | Q_LEFT_FRONT | Q_RIGHT_FRONT},	// low proitoirty
	{&dunk_combined_7,		50,	D_FAR | D_ATTRIB_ALL | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT },
	{&dunk_combined_8,		25,	D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT },
	{&dunk_combined_9,		60,	D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_combined_10, 	90,	D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_combined_11, 	50, D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_RIGHT},
	{&dunk_combined_11, 	50, D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_RIGHT | D_BALL_TRAIL},
	{&dunk_combined_12, 	 5,	D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_combined_12a,	 5,	D_FAR | D_ATTRIB_ALL | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_combined_13, 	80,	D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_combined_14, 	80,	D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_combined_16, 	75,	D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT},
	{&dunk_combined_17, 	75,	D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT},
	{&dunk_combined_18, 	100,D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT | D_BALL_TRAIL},
	{&dunk_combined_19, 	10,	D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT}, // LP
	{&dunk_combined_20, 	20, D_FAR | D_ATTRIB_ALL | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT | D_BALL_TRAIL},
	{&dunk_combined_20a,	20,	D_FAR | D_ATTRIB_ALL | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT |  D_BALL_TRAIL},
	{&dunk_combined_21,		35, D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_combined_21a,	35,	D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_combined_21b,	35,	D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_combined_22,		30,	D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_combined_22a,	30,	D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_scissor_1a, 		100,D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_windmill_1a,		50,	D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_windmill_2a,		50,	D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_tomahawk_1a,		90,	D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_2hpmp_1a,   		100,D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_2hpmp_2a,   		100,D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_360_3a,     		100,D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_2hrev_1a,   		100,D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_2hrev_2a,   		100,D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_360_1a,     		100,D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_2h360_1a,   		100,D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_1hand_2a,   		100,D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_1hand_4a,   		100,D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_180_1a,     		100,D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_270h_1a,    		60,	D_FAR | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},

// MEDIUM range dunks
	{&dunk_scissor_2,  		100,	D_MED | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_combined_4a,		100,	D_MED | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_combined_2a,		100,	D_MED | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_combined_1a,		100,	D_MED | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_pump_1,			100,	D_MED | D_RIGHT_HAND | D_ATTRIB_ALL | D_RIM_FRONT | Q_LEFT},
	{&dunk_pump_1,			100,	D_MED | D_LEFT_HAND | D_ATTRIB_ALL | D_RIM_FRONT | Q_RIGHT},
	{&dunk_1hand_but_out,	100,	D_MED | D_ATTRIB_ALL | Q_QUAD_ALL},
	{&dunk_1_hand_1,		100,	D_MED | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_scissor_1, 		100,	D_MED | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_windmill_1,		100,	D_MED | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_windmill_2,		100,	D_MED | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_tomahawk_1,		100,	D_MED | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_180_1,     		100,	D_MED | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_270h_1,    		100,	D_MED | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_2hrev_1,   		100,	D_MED | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_2hrev_2,   		100,	D_MED | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_2hpmp_1,   		100,	D_MED | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_360_1,     		100,	D_MED | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_1hand_4,   		100,	D_MED | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_1hand_2,   		100,	D_MED | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_2h360_1,   		100,	D_MED | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_2hpmp_2,   		100,	D_MED | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
//#endif
//#if 0
// VERY CLOSE range dunks
	{&dunk_quick_1,			100,		D_SHORT | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_quick_2,			100,		D_SHORT | D_ATTRIB_ALL | D_INVERT | Q_LEFT_BEHIND | Q_BEHIND | Q_RIGHT_BEHIND},
	{&dunk_quick_4,			100,		D_SHORT | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
	{&dunk_quick_5,			100,		D_SHORT | D_RIGHT_HAND | D_ATTRIB_ALL | D_RIM_FRONT | Q_LEFT | Q_LEFT_BEHIND},
	{&dunk_quick_5,			100,		D_SHORT | D_LEFT_HAND | D_ATTRIB_ALL | D_RIM_FRONT | Q_RIGHT | Q_RIGHT_BEHIND},
	{&dunk_windup_1,		100,		D_SHORT | D_ATTRIB_ALL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT | Q_RIGHT},
#endif
};

//-----------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
#define NUM_DUNKS	(sizeof(dunks)/sizeof(struct _Dunk))

// quad4 works as follows:
// receiver is: 0-behind, 1-left, 2-front, 3-right
// quad8 works as follows:
// receiver is:
//	0-behind, 1-left.behind, 2-left, 3-left.front
//	4-front, 5-right.front, 6-right, 7-right.behind

extern int invert_dunk_angle, rim_lock_angle;
void Get_A_Dunk(obj_3d * pobj)
{
	register bbplyr_data * ppd = (bbplyr_data *)pobj;
	register game_info_t * pgi = &game_info;
	//register ball_data * pbo = &ball_obj;

	int i, indx, rim_bearing, angle_to_board, prob, select;
	int quad4, quad8, quad_test = 0, select_dunk[NUM_DUNKS];
	float dist_to_board, rim_pt[3];
	//float c_time, dist, nx, nz;
	char *dunk_anim = NULL;

	// Are we doing an ally off the backboard?
	// is the offender going toward the basket at all?
	rim_pt[0] = pgi->active_goal ? RIM_CX : -RIM_CX; rim_pt[1] = 0.0f; rim_pt[2] = 0.0f;

// quad4 works as follows:
// receiver is: 0-behind, 1-left, 2-front, 3-right
//
// quad8 works as follows:
//	0-behind, 1-left.behind, 2-left, 3-left.front
//	4-front, 5-right.front, 6-right, 7-right.behind
	quad8 = (rim_pt[0] < 0.0f ? 768 : 256) - ptangle( &(pobj->x), rim_pt );
	quad4 = ((quad8 + 1536 + 128)&1023) / 256;		// four directions
	quad8 = ((quad8 + 1536 + 64) &1023) / 128;		// eight directions

	angle_to_board = ptangle( &(ppd->odata.x), rim_pt);	
	rim_bearing = (ppd->odata.fwd - ptangle( &(ppd->odata.x), rim_pt)) & 1023;
	if (rim_bearing > 512) rim_bearing = 1024 - rim_bearing;
	dist_to_board = fsqrt(
		(rim_pt[0] - ppd->odata.x) * (rim_pt[0] - ppd->odata.x) +
		(rim_pt[2] - ppd->odata.z) * (rim_pt[2] - ppd->odata.z));

// dunks are chosen from a table of dunks by:
//	1. dunker->fwd
//	2. angle between dunker and rim
//	3. distance between the two

	// TAG ALL LEGAL DUNKS
	quad_test |= 1<<quad8;
	for(i = 0, indx = -1; i < NUM_DUNKS; i++)
	{
		select_dunk[i] = -1;

		// does this dunk work with this quad8...
		if(!(dunks[i].flags & quad_test))
			continue;

		// does this dunk require the ball to be in a particular hand...
		if((dunks[i].flags & D_LEFT_HAND  && ppd->ball_in_hand == RIGHT_HAND)
		|| (dunks[i].flags & D_RIGHT_HAND && ppd->ball_in_hand == LEFT_HAND))
			continue;

		// does this dunk work within FAR distance
		if((dist_to_board >= D_DIST_FAR) && !(dunks[i].flags & (D_FAR|D_ANYWHERE))) 
			continue;

		// does this dunk work within FAR distance
		if((dist_to_board > D_DIST_SHORT) && (dist_to_board < D_DIST_FAR) && !(dunks[i].flags & (D_MED|D_ANYWHERE))) 
			continue;

		// does this dunk work within FAR distance
		if((dist_to_board <= D_DIST_SHORT) && !(dunks[i].flags & (D_SHORT|D_ANYWHERE))) 
			continue;

		if((dunks[i].flags & D_ATTRIB_LVL3) && (ppd->_dunking < 9) && !(dunks[i].flags & (D_ATTRIB_ALL|D_ATTRIB_LVL4)))
			continue;

		if((dunks[i].flags & D_ATTRIB_LVL2) && (ppd->_dunking < 4 || ppd->_dunking > 8) && !(dunks[i].flags & (D_ATTRIB_ALL|D_ATTRIB_LVL4)))
			continue;

		if((dunks[i].flags & D_ATTRIB_LVL1) && (ppd->_dunking > 3) && !(dunks[i].flags & (D_ATTRIB_ALL|D_ATTRIB_LVL4)))
			continue;

		select_dunk[++indx] = i;
	}

	if(indx == -1)
	{
		// can't find a dunk that fulfills the requirements
#ifdef DEBUG_DUNKS
		printf("ERROR: CANT FIND A DUNK SEQ\n");
//		while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
#endif
	// since a dunk can't be found take a jump shot...
		change_anim(pobj, shot_anims[randrng(sizeof(shot_anims)/sizeof(char *))]);
		return;
	}
	else
	{
		// now sum the probabilities from the legal dunks
		prob = 0;
		//for(i=0; select_dunk[i] != -1; i++)
		for(i = 0; select_dunk[i] != -1 && i < NUM_DUNKS; i++)
			prob += dunks[select_dunk[i]].pcnt;

		select = 1+randrng(prob);
		i = -1;
		while( select > 0 )
		{
			i += 1;
			select -= dunks[select_dunk[i]].pcnt;
		}

		// now select a dunk based on rnd_num into table
		dunk_anim = (char *)((int) dunks[select_dunk[i]].dunk | 0x80000000);

		// do a dunk!
//		i = select_dunk[randrng(1+indx)];
//		dunk_anim = (char *)((int)dunks[i].dunk | 0x80000000);

		if(dunks[i].flags & D_INVERT)
			invert_dunk_angle = YES;
		else
			invert_dunk_angle = NO;

		if(dunks[i].flags & D_RIM_LEFT)
			rim_lock_angle = 1;
		else
		if(dunks[i].flags & D_RIM_FRONT)
			rim_lock_angle = 2;
		else
		if(dunks[i].flags & D_RIM_RIGHT)
			rim_lock_angle = 3;
		else
			rim_lock_angle = -1;

#ifdef DEBUG_DUNKS
		{
		int y;
		printf("indx = %d, select_dunk[%d] = %d\n",indx,indx,i);
		for(y = 0; y < NUM_DUNKS; y++)
			printf("select_dunk[%d] = %d\n",y,select_dunk[y]);
		}
#endif

		// clear for all dunk starts
		ppd->odata.flags &= ~PF_BOTH_HANDS;

		change_anim(&ppd->odata, dunk_anim);
		dunk_take_fx(dist_to_board);
		if(dunks[i].flags & D_BALL_TRAIL)
			balltrail = 1;

		what_type_shot = DUNK;
		
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
struct _Dunk double_dunks[] =
{
	{&ddunk_1, 100, Q_QUAD_ALL},				// SEQ_K_QUIK_6
	{&ddunk_2, 100, Q_QUAD_ALL},				// SEQ_K_WIND_L
	//{&allyoop_1, 100, Q_BEHIND | Q_LEFT_BEHIND | Q_RIGHT_BEHIND},		// SEQ_K_ALY3_R,SEQ_K_ALY3_L

	//{&allyoop_2, 100, Q_FRONT | Q_LEFT_FRONT | Q_RIGHT_FRONT},				// SEQ_K_QUIK_6

	//{&allyoop_3, 100, Q_BEHIND | Q_LEFT_BEHIND | Q_RIGHT_BEHIND},		// SEQ_K_2HREV2

	//{&allyoop_4, 100, Q_LEFT | Q_LEFT_BEHIND | Q_LEFT_FRONT},				// SEQ_K_WIND_L
	//{&allyoop_4a, 100, Q_RIGHT | Q_RIGHT_BEHIND | Q_RIGHT_FRONT},		// SEQ_K_WIND_R
};

#define NUM_DOUBLE_DUNKS	(sizeof(double_dunks)/sizeof(struct _Dunk))
//----------------------------------------------------------------------------
void Get_A_Double_Dunk(obj_3d * pobj)
{
	bbplyr_data * ppd = (bbplyr_data *)pobj;
	game_info_t * pgi = &game_info;
	int i, indx, quad4, quad8, quad_test = 0;
	int rim_bearing, select_dunk[NUM_DOUBLE_DUNKS];
	char *dunk_anim = NULL;
	float rim_pt[3], dist_to_rim;

	rim_pt[0] = pgi->active_goal ? RIM_CX : -RIM_CX; rim_pt[1] = 0.0f; rim_pt[2] = 0.0f;
	rim_bearing = (pobj->fwd - ptangle( &(pobj->x), rim_pt)) & 1023;
	rim_bearing = (rim_bearing > 512) ? 1024 - rim_bearing : rim_bearing;

	dist_to_rim = get_dist_to_rim(ppd);

	if(rim_bearing > 100 || dist_to_rim > 50.0f)
	{
#ifdef DEBUG_DOUBLE_DUNKS
		printf("ERROR - DOUBLE_DUNKS: NOT RUNNING AT RIM OR TOO FAR AWAY\n");
#endif
		return;
	}

// quad8 works as follows:
//	0-behind, 1-left.behind, 2-left, 3-left.front
//	4-front, 5-right.front, 6-right, 7-right.behind
	//quad8 = pobj->tgt_fwd - ptangle(&pobj->x, &ball_obj.odata.x);
	quad8 = get_angle_to_rim(ppd) - ptangle(&pobj->x, &ball_obj.odata.x);
	quad4 = ((quad8 + 1536 + 128)&1023) / 256;		// four directions
	quad8 = ((quad8 + 1536 + 64) &1023) / 128;		// eight directions

	// TAG ALL LEGAL DOUBLE DUNKS
	quad_test |= 1<<quad8;
	for(i = 0, indx = -1; i < NUM_DOUBLE_DUNKS; i++)
	{
		select_dunk[i] = -1;

		// does this dunk work with this quad8...
		if(!(double_dunks[i].flags & quad_test))
			continue;

		select_dunk[++indx] = i;
	}

	if(indx == -1)
	{
		// can't find a double dunk that fulfills the requirements
#ifdef DEBUG_DOUBLE_DUNKS
		printf("ERROR: CANT FIND A DOUBLE DUNK SEQ\n");
//		while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
#endif
	// since a double dunk can't be found take a jump shot...
		//change_anim(pobj, shot_anims[randrng(sizeof(shot_anims)/sizeof(char *))]);
		return;
	}
	else
	{
		// do a double dunk!
		i = select_dunk[randrng(1+indx)];
		dunk_anim = (char *)((int)double_dunks[i].dunk | 0x80000000);

#ifdef DEBUG_DOUBLE_DUNKS
		{
		int y;
		printf("indx = %d, select_dunk[%d] = %d\n",indx,indx,i);
		for(y = 0; y < NUM_DOUBLE_DUNKS; y++)
			printf("select_dunk[%d] = %d\n",y,select_dunk[y]);
		}
#endif

		// clear for all double dunk starts
		ppd->odata.flags &= ~PF_BOTH_HANDS;
		
		pobj->tgt_fwd = get_angle_to_rim(ppd);

		change_anim(&ppd->odata, dunk_anim);
		dunk_take_fx(dist_to_rim);

		what_type_shot = DUNK;

	}
}

struct _Dunk allyoop_dunks[] =
{
//	{&allyoop_1, Q_QUAD_ALL },
//	{&allyoop_2, Q_QUAD_ALL },
	{&allyoop_1, 100, Q_BEHIND | Q_LEFT_BEHIND | Q_RIGHT_BEHIND},		// SEQ_K_ALY3_R,SEQ_K_ALY3_L
	{&allyoop_2, 100, Q_FRONT | Q_LEFT_FRONT | Q_RIGHT_FRONT},				// SEQ_K_QUIK_6
	{&allyoop_3, 100, Q_BEHIND | Q_LEFT_BEHIND | Q_RIGHT_BEHIND},		// SEQ_K_2HREV2
	{&allyoop_1, 100, Q_BEHIND | Q_LEFT_BEHIND | Q_RIGHT_BEHIND},		// SEQ_K_ALY3_R,SEQ_K_ALY3_L
	{&allyoop_2, 100, Q_FRONT | Q_LEFT_FRONT | Q_RIGHT_FRONT},				// SEQ_K_QUIK_6
	{&allyoop_3, 100, Q_BEHIND | Q_LEFT_BEHIND | Q_RIGHT_BEHIND},		// SEQ_K_2HREV2
	{&allyoop_1, 100, Q_BEHIND | Q_LEFT_BEHIND | Q_RIGHT_BEHIND},		// SEQ_K_ALY3_R,SEQ_K_ALY3_L
	{&allyoop_2, 100, Q_FRONT | Q_LEFT_FRONT | Q_RIGHT_FRONT},				// SEQ_K_QUIK_6
	{&allyoop_3, 100, Q_BEHIND | Q_LEFT_BEHIND | Q_RIGHT_BEHIND},		// SEQ_K_2HREV2

	{&allyoop_4, 100, Q_LEFT | Q_LEFT_BEHIND | Q_LEFT_FRONT},				// SEQ_K_WIND_L
	{&allyoop_4a,100, Q_RIGHT | Q_RIGHT_BEHIND | Q_RIGHT_FRONT},		// SEQ_K_WIND_R
};

#define NUM_ALLYOOP_DUNKS	(sizeof(allyoop_dunks)/sizeof(struct _Dunk))
//----------------------------------------------------------------------------
void Get_A_AllyOop(obj_3d * pobj)
{
	bbplyr_data * ppd = (bbplyr_data *)pobj;
	game_info_t * pgi = &game_info;
	int i, indx, quad4, quad8, quad_test = 0;
	int rim_bearing, select_dunk[NUM_ALLYOOP_DUNKS];
	char *dunk_anim = NULL;
	float rim_pt[3], dist_to_rim;

	rim_pt[0] = pgi->active_goal ? RIM_CX : -RIM_CX; rim_pt[1] = 0.0f; rim_pt[2] = 0.0f;
	rim_bearing = (pobj->fwd - ptangle( &(pobj->x), rim_pt)) & 1023;
	rim_bearing = (rim_bearing > 512) ? 1024 - rim_bearing : rim_bearing;

	dist_to_rim = get_dist_to_rim(ppd);

	if(rim_bearing > 100 || dist_to_rim > 50.0f)
	{
#ifdef DEBUG
		printf("ERROR - ALLYOOP_DUNKS: NOT RUNNING AT RIM OR TOO FAR AWAY\n");
#endif
		return;
	}

// quad8 works as follows:
//	0-behind, 1-left.behind, 2-left, 3-left.front
//	4-front, 5-right.front, 6-right, 7-right.behind
	//quad8 = pobj->tgt_fwd - ptangle(&pobj->x, &ball_obj.odata.x);

//	quad8 = (rim_pt[0] < 0.0f ? 768 : 256) - ptangle( &(pobj->x), rim_pt );
//	quad4 = ((quad8 + 1536 + 128)&1023) / 256;		// four directions
//	quad8 = ((quad8 + 1536 + 64) &1023) / 128;		// eight directions
	quad8 = get_angle_to_rim(ppd) - ptangle(&pobj->x, &ball_obj.odata.x);
	quad4 = ((quad8 + 1536 + 128)&1023) / 256;		// four directions
	quad8 = ((quad8 + 1536 + 64) &1023) / 128;		// eight directions

	// TAG ALL LEGAL ALLYOOP DUNKS

	quad_test |= 1<<quad8;
	for(i = 0, indx = -1; i < NUM_ALLYOOP_DUNKS; i++)
	{
		select_dunk[i] = -1;

		// does this dunk work with this quad8...
		if(!(allyoop_dunks[i].flags & quad_test))
			continue;

		select_dunk[++indx] = i;
	}

	if(indx == -1)
	{
		// can't find a allyoop dunk that fulfills the requirements
#ifdef DEBUG_ALLYOOP_DUNKS
		printf("ERROR: CANT FIND AN ALLYOOP DUNK SEQ\n");
//		while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
#endif
	// since a ALLYOOP DUNKS can't be found take a jump shot...
		//change_anim(pobj, shot_anims[randrng(sizeof(shot_anims)/sizeof(char *))]);
		return;
	}
	else
	{
	// if I'm a drone i've used my free attempt
		if(ISDRONE(ppd))
			drone_attempt_allyoop = 1;
	
		// do a ALLYOOP_DUNKS!
		i = select_dunk[randrng(1 + indx)];
		dunk_anim = (char *)((int)allyoop_dunks[i].dunk | 0x80000000);

#ifdef DEBUG_ALLYOOP_DUNKS
		{
		int y;
		printf("indx = %d, select_dunk[%d] = %d\n",indx,indx,i);
		for(y = 0; y < NUM_ALLYOOP_DUNKS; y++)
			printf("select_dunk[%d] = %d\n",y,select_dunk[y]);
		}
#endif

		// clear for all double dunk starts
		ppd->odata.flags &= ~PF_BOTH_HANDS;
		
		pobj->tgt_fwd = get_angle_to_rim(ppd);

		change_anim(&ppd->odata, dunk_anim);
		//dunk_take_fx(dist_to_rim);
	}
}

//////////////////////////////////////////////////////////////////////////////
char *layup_anims[] =
{
	layup25_anim,
	layup26_anim,
	layup27_anim,
	layup1_anim,
	layup6_anim,
	layup4_anim,
	layup7_anim,
	layup9_anim,
	layup23_anim,
	layup24_anim,
	layup16_anim,
	layup17_anim
};

// quad8 works as follows:
// player is:
//	0-behind, 1-left.behind, 2-left, 3-left.front
//	4-front, 5-right.front, 6-right, 7-right.behind

// quad4 works as follows:
// player is: 0-behind, 1-left, 2-front, 3-right

void Get_A_LayUp(obj_3d *pobj)
{
	int layup = 0, quad8, quad4;
	float rim_pt[3];
	bbplyr_data * ppd = (bbplyr_data *)pobj;
	//bbplyr_data *ppdata = (bbplyr_data *)pobj;

	rim_pt[0] = RIM_X * game_info.active_goal == 0 ? -1.0f : 1.0f; rim_pt[1] = rim_pt[2] = 0.0f;

	quad8 = (rim_pt[0] < 0.0f ? 768 : 256) - ptangle( &(pobj->x), rim_pt );
	quad4 = ((quad8 + 1536 + 128)&1023) / 256;		// four directions
	quad8 = ((quad8 + 1536 + 64) &1023) / 128;		// eight directions

	layup = rand() % (sizeof(layup_anims)/sizeof(int));

	// clear for all layup starts
	ppd->odata.flags &= ~PF_BOTH_HANDS;

	change_anim( pobj, layup_anims[layup]);

	what_type_shot = LAYUP;

	return;
}

char *j_rt_rt_lt_lt[] =
{
	cross_over_5,
	cross_over_6,
	spinback_1,
	behindback_spin_1,
	spinback_2,
	between_both_legs_1,
	spin_6,
	//around_back_twice,
};

char *j_front_close[] =
{
	spin_6,
	spin_5,
	behindback_spin_1,
	spin_1,
	//spin_4,
	spinback_1,
	behindback_spin_1,
	between_both_legs_1,
	cross_over_5,
	cross_over_6,
	cross_over_1,
	cross_over_3,
	//around_back_twice,
};

char *j_rt_lt_lt_rt[] =
{
	spin_1,
	spin_5,
	cross_over_1,
	cross_over_3,
	//spin_4,
//	between_legrun
};

char *j_random[] =
{
	spin_1,
	//spin_5,
	//spin_4,
	between_both_legs_1,
	behindback_spin_1,
//	between_legrun
	//spin_3,		// a stationary spin move
	//spin_2,		// not sure if I like yet...
};

char *jukes_debug[] = 
{
	spin_6,
#if 0
	spin_5,
	spin_1,
	spinback_2,
	spinback_1,
	cross_over_1,
	cross_over_3,
	cross_over_5,
	cross_over_6,

	between_both_legs_1,
	behindback_spin_1,

	spin_3,	// a stationary spin move
	spin_4,
	//spin_2,	// not sure if I like yet...
	//spin_3,	// a stationary spin move
	//spin_4,
	//spin_2,	// not sure if I like yet...
	//around_back_twice,
	//between_leg_spin_r,
#endif
};

char *jukes_weak[] =
{
	spin_1,
	spinback_1,
	spin_1,
	spin_1,
};

char *jukes_med[] =
{
	spin_1,
	between_both_legs_1,
	spinback_1,
	behindback_spin_1,
	spinback_2,
};

//----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
extern int juke_flail_allow;
void choose_shake_anim(bbplyr_data *ppdata)
{
obj_3d *pobj = (obj_3d *)ppdata;
int avoid_dir, avoid_dist, cp = nearest_to_ball(!ppdata->team), quad4, quad8,
	available_shakes, which_shake, rim_bearing, im_moving, brg = plyr_bearing(ppdata->plyrnum, cp);
char **shake_table;
float	p[3] = {0.0f, 0.0f, 0.0f};
static int debug_shake = 0;

	juke_flail_allow = 26;		//30
	juke_shoe_squeak(pobj);

	// force juke script to set this bit...if needed.
	ppdata->odata.flags &= ~PF_BOTH_HANDS;

#ifdef DEBUG_SHAKE_MOVES
	if((pobj->adata[0].seq_index == (SEQ_D_BKN1_R) || pobj->adata[0].seq_index == (SEQ_D_BKN1_L))
	|| (pobj->adata[0].seq_index == (SEQ_D_BKN2_R) || pobj->adata[0].seq_index == (SEQ_D_BKN2_L))
	|| (pobj->adata[0].seq_index == (SEQ_D_STD5_R) || pobj->adata[0].seq_index == (SEQ_D_STD5_L))
	)
	{
		change_anim( (obj_3d *)ppdata, spin_1 );
		return;
	}

	change_anim(pobj, jukes_debug[randrng(sizeof(jukes_debug)/sizeof(char *))]);
	return;
#endif

	//ppdata->action++;	// inc. players juke cnt

	if((pobj->adata[0].seq_index == (SEQ_D_BKN1_R) || pobj->adata[0].seq_index == (SEQ_D_BKN1_L))
	|| (pobj->adata[0].seq_index == (SEQ_D_BKN2_R) || pobj->adata[0].seq_index == (SEQ_D_BKN2_L))
	|| (pobj->adata[0].seq_index == (SEQ_D_STD5_R) || pobj->adata[0].seq_index == (SEQ_D_STD5_L))
	)
	{
		change_anim(pobj, spin_1);
		return;
	}

	if(ppdata->_dribbling < 8)
	{
		change_anim(pobj, jukes_weak[randrng(sizeof(jukes_weak)/sizeof(char *))]);
		return;
	}

	if(ppdata->_dribbling < 14)
	{
		change_anim(pobj, jukes_med[randrng(sizeof(jukes_med)/sizeof(char *))]);
		return;
	}

	p[0] = game_info.active_goal ? RIM_X : -RIM_X; p[1] = 0.0f; p[2] = 0.0f;
	rim_bearing = (ppdata->odata.fwd - ptangle( &(ppdata->odata.x), p)) & 1023;
	if (rim_bearing > 512) rim_bearing = 1024 - rim_bearing;

	im_moving = (pobj->vx != 0.0f || pobj->vz != 0.0f);

// where is nearest defender...
//  quad8 works as follows:
//  defender is:
//	 0-behind, 1-left.behind, 2-left, 3-left.front
//	 4-front, 5-right.front, 6-right, 7-right.behind
	quad8 = ppdata->odata.fwd - p2p_angles[ppdata->plyrnum][cp];
	quad4 = ((quad8 + 1536 + 128)&1023) / 256;		// four directions
	quad8 = ((quad8 + 1536 + 64) &1023) / 128;		// eight directions

// make sure defender is REALLY in front of the dribbler
	//if(quad8 == Q8_FRONT && distxz[ppdata->plyrnum][cp] >= FT(5.0f))
	//{
	//	if(brg <= 38)
	//		quad8 = Q8_FRONT;
	//	else if(brg > 38 && (ppdata->odata.fwd - p2p_angles[ppdata->plyrnum][cp]) > 0)
	//		quad8 = Q8_RIGHT_FRONT;
	//	else if(brg > 38 && (ppdata->odata.fwd - p2p_angles[ppdata->plyrnum][cp]) < 0)
	//		quad8 = Q8_LEFT_FRONT;
	//}

// is he far away...
	if(distxz[ppdata->plyrnum][cp] > FT(10.0))
	{
		// maybe do a juke that works from a standstill...
		//if(!im_moving)
		//{
			// am I going towards the rim at all...
			//if(rim_bearing < 200)
			//{
				// do a spin_1 or between the leg anim...
				change_anim(pobj, j_random[randrng(sizeof(j_random)/sizeof(char *))]);
			//}
			//else
			//{ // not going towards the rim...
			//	change_anim(pobj, spin_1 );
			//}
		//}
		//else
		//{ // do a juke that works with velocity...most of them do...
		//	;
		//}
	}
	else
	{
	// face the defender if he's in range
		if(brg <= 96)
			pobj->fwd = pobj->tgt_fwd = p2p_angles[ppdata->plyrnum][cp];

	// no dribbles can have the ball in both hands.
		ppdata->odata.flags &= ~PF_BOTH_HANDS;
		if((quad8 == Q8_LEFT_FRONT && ppdata->ball_in_hand == RIGHT_HAND)
		|| (quad8 == Q8_RIGHT_FRONT && ppdata->ball_in_hand == LEFT_HAND))
		{
			change_anim(pobj, j_rt_rt_lt_lt[randrng(sizeof(j_rt_rt_lt_lt)/sizeof(char *))]);
			return;
		}

		if((quad8 == Q8_RIGHT_FRONT && ppdata->ball_in_hand == RIGHT_HAND)
		|| (quad8 == Q8_LEFT_FRONT && ppdata->ball_in_hand == LEFT_HAND))
		{
			change_anim(pobj, j_rt_lt_lt_rt[randrng(sizeof(j_rt_lt_lt_rt)/sizeof(char *))]);
			return;
		}

		if((quad8 == Q8_FRONT))
		{
			change_anim(pobj, j_front_close[randrng(sizeof(j_front_close)/sizeof(char *))]);
			return;
		}

		change_anim(pobj, j_random[randrng(sizeof(j_random)/sizeof(char *))]);
		return;
	}

}


//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void maybe_throw_elbows(obj_3d *pobj)
{
	bbplyr_data * ppdata = (bbplyr_data *)pobj;

	if (randrng(100) < 45)
		return;

	if (game_info.ball_handler != ppdata->plyrnum)
		return;

	if (distxz[ppdata->plyrnum][nearest_to_me(ppdata->plyrnum, !ppdata->team)] <= 20.0f)
		change_anim(pobj,throw_elbows_wb_anim);
}

//-----------------------------------------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------------------------------------
void maybe_do_rim_hang_anim(obj_3d *pobj, int which_hang, int percent, int hands)
{
int plyr = ((bbplyr_data *)pobj)->plyrnum;

	if (pobj->flags & PF_ALLYDUNKER)
		return;
	
	if(player_blocks[plyr ^ 1].HangingOnRim == YES)
		return;

	if (randrng(100) > percent)
		return;

	if (which_hang == -1)
	{
		if (hands == 0)		// two hands ?
			change_anim(pobj, oneh_rim_hang_tbl[randrng(sizeof(oneh_rim_hang_tbl)/sizeof(char *))]);
		else
			change_anim(pobj, twoh_rim_hang_tbl[randrng(sizeof(twoh_rim_hang_tbl)/sizeof(char *))]);
	}
	else
	{
		if (hands == 0)		// two hands ?
			change_anim(pobj, oneh_rim_hang_tbl[which_hang]);
		else
			change_anim(pobj, twoh_rim_hang_tbl[which_hang]);
	}
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void SetHangOnRim(obj_3d *pobj, int set, int both_hands, int clear_both_hands_on_rim)
{

	int		i;

	// if player is coming off of a rim hang dont let him collide with ball for a bit
	if(set == NO && ((bbplyr_data *)pobj)->HangingOnRim == YES)
		((bbplyr_data *)pobj)->no_ball_collide = 15;

	((bbplyr_data *)pobj)->HangingOnRim = set;

	if(clear_both_hands_on_rim > -1)
		both_hands_on_rim = both_hands;

//	if(set == YES)
	goal_shake(pobj->x < 0 ? 0 : 1, set, -5);

	//printf("set hang on rim: both hands = %s, clear both hands = %d, set = %s\n",
	//	both_hands == YES ? "YES" : "NO", clear_both_hands_on_rim, set == YES ? "YES" : "NO" );

	if(set == YES)
	{
		i = rndrng0 (99) < 50 ? DUNK1_SND : DUNK2_SND;
		snd_scall_bank( gameplay_bnk_str, i, VOLUME7, 127, PRIORITY_LVL3 );
//		make_dunk_snd(i);
	}



}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
int nopump_fake(obj_3d *pobj)
{
	return (no_pump_fake == YES);

//	if(no_pump_fake == YES)
//	{
//		//printf("no pump fake\n");
//		return 1;	// no pump fake 
//	}
//	else
//	{
//		//printf("do pump fake\n");
//		return 0;	// do pump fake
//	}
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void clear_pumpfake(obj_3d *pobj)
{
	no_pump_fake = YES;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void SetTipoffVels(obj_3d *pobj, int time)
{
	//bbplyr_data *ppdata = (bbplyr_data *)pobj;
	long angle_to_ball;//, limb;
	//float dist_to_ball, ax, ay, az, tx, ty, tz,	//p[3],
	//		rx, ry, rz, offx, offy, offz, time, vxz, min_xzv, max_xzv, max_yv;
	float dist_to_ball, ax, ay, az, tx, ty, tz, offy, vxz, min_xzv, max_xzv, max_yv;

	dist_to_ball = fsqrt( ((ball_obj.odata.x - pobj->x) * (ball_obj.odata.x - pobj->x)) + ((ball_obj.odata.z - pobj->z) * (ball_obj.odata.z - pobj->z)) );
	angle_to_ball = ptangle( &(pobj->x), &ball_obj.odata.x);
	pobj->tgt_fwd = angle_to_ball;

	//time = 11;
	get_joint_position(pobj, -1, 29, (pobj->x > 0.0f) ? JOINT_RWRIST : JOINT_RWRIST , pobj->tgt_fwd, &ax, &ay, &az);
	ax += pobj->x;
	az += pobj->z;

	tx = ball_obj.odata.x + time * ball_obj.odata.vx;
	ty = ball_obj.odata.y + (time * ball_obj.odata.vy) + (0.5f * BALL_GRAVITY * (time * time));
	tz = ball_obj.odata.z + time * ball_obj.odata.vz;

	pobj->vx = (tx - ax) / time;
	pobj->vy = ((ty - ay) / time) - (0.5f * GRAVITY * time);

	if(pobj->vy > 1.8f)
		pobj->vy = 1.8f;

	pobj->vz = (tz - az) / time;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void rotate_to_face_ball(obj_3d *pobj)
{
	float	dist_to_ball;
	long	angle_to_ball;

	dist_to_ball = fsqrt( ((ball_obj.odata.x - pobj->x) * (ball_obj.odata.x - pobj->x)) + ((ball_obj.odata.z - pobj->z) * (ball_obj.odata.z - pobj->z)) );
	angle_to_ball = ptangle( &(pobj->x), &ball_obj.odata.x);
	pobj->tgt_fwd = angle_to_ball;
}


//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void rotate_to_face_rim(obj_3d *pobj)
{
	float	rim_pt[3];

	rim_pt[0] = game_info.active_goal ? RIM_X : -RIM_X;
	rim_pt[1] = rim_pt[2] = 0.0f;
	pobj->tgt_fwd = ptangle( &(player_blocks[game_info.ball_handler].odata.x), rim_pt);
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void face_rim(obj_3d *pobj)
{
	float	rim_pt[3];

	rim_pt[0] = game_info.active_goal ? RIM_X : -RIM_X;
	rim_pt[1] = rim_pt[2] = 0.0f;
	pobj->fwd = ptangle( &(player_blocks[game_info.ball_handler].odata.x), rim_pt);
	pobj->tgt_fwd = pobj->fwd;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void SetReboundVels(obj_3d *pobj, int seq, int frame, int contact_time)
{
	long angle_to_ball;//, limb;
	float dist_to_ball, ax, ay, az, tx, ty, tz, offy, time, vxz, min_xzv, max_xzv, min_yv, max_yv;
	float	a,b,c,b2m4ac,root1,root2;
	int		air_time;

	time = contact_time;
//	ball_calcstatus(time);

	max_xzv = 0.0f;
	min_xzv = 0.0f;
	max_yv = 0.0f;
	min_yv = 0.0f;

	if (pobj->flags & PF_TURBO)
	{
		max_yv = 1.85f;
		min_yv = 1.45f;
		max_xzv = 0.65f;
	}
	else
	{
		max_yv = 1.60f;//40f;
		min_yv = 1.35f;
		max_xzv = 0.45f;
	}

//	if (plyr_behind_backboard(pobj) == YES)
//	{
//		max_xzv = 0.02f;
//		max_yv = 0.95;
//		min_yv = 0.80;
//	}

	get_joint_position_rel(pobj, seq, frame, JOINT_RWRIST, pobj->tgt_fwd, &ax, &ay, &az, 0.0f, 15.0f, 0.0f);
	offy = ay;
	ax += pobj->x;
	az += pobj->z;

	tx = ball_obj.odata.x + time * ball_obj.odata.vx;
	ty = ball_obj.odata.y + (time * ball_obj.odata.vy) + (0.5f * BALL_GRAVITY * (time * time));
	tz = ball_obj.odata.z + time * ball_obj.odata.vz;

	pobj->vx = (tx - ax) / time;
	pobj->vy = ((ty - ay) / time) - (0.5f * GRAVITY * time);
	pobj->vz = (tz - az) / time;
	

	vxz = fsqrt( pobj->vx * pobj->vx + pobj->vz * pobj->vz );

	// adjust Y vel
	if (pobj->vy > max_yv)
		pobj->vy = max_yv;

	if (pobj->vy < min_yv)
		pobj->vy = min_yv;

	// adjust XZ vel
	if (vxz > max_xzv)
	{
		pobj->vx = pobj->vx / vxz * max_xzv;
		pobj->vz = pobj->vz / vxz * max_xzv;
	}

	if (vxz < min_xzv)
	{
		pobj->vx = FRELX(0.0f,min_xzv,pobj->tgt_fwd);
		pobj->vz = FRELZ(0.0f,min_xzv,pobj->tgt_fwd);
	}

//	if (((ball_obj.total_time - ball_obj.flight_time) < time) && (game_info.ball_handler == NO_BALLHANDLER))
//		pobj->vy = 1.15f;

	((bbplyr_data *)pobj)->flight_time = 0;


	// figure out time in air
	a = GRAVITY / 2.0f;
	b = pobj->vy;
	c = 0.0f;
	b2m4ac = b * b - 4.0f * a * c;
	root1 = (-1.0f * b + fsqrt( b2m4ac )) / (2.0f * a);
	root2 = (-1.0f * b - fsqrt( b2m4ac )) / (2.0f * a);
	air_time = 1;
	if (root1 > 1.0f)
		air_time = (int)(root1+0.9f);

	if (root2 > 1.0f)
		air_time = (int)(root2+0.9f);

	((bbplyr_data *)pobj)->total_time = air_time;
//printf("REBOUND VEL(): y vels: %2.2f\n",pobj->vy);
}	

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void SetQuickReboundVels(obj_3d *pobj, int seq, int frame, int contact_time)
{
	long angle_to_ball;//, limb;
	float dist_to_ball, ax, ay, az, tx, ty, tz, offy, time, vxz, min_xzv, max_xzv, min_yv, max_yv;

	time = contact_time;

	max_yv = .45f;//40f;
	min_yv = .10f;
	max_xzv = 0.45f;
	min_xzv = 0.0f;

	get_joint_position_rel(pobj, seq, frame, JOINT_RWRIST, pobj->tgt_fwd, &ax, &ay, &az, 0.0f, 15.0f, 0.0f);
	offy = ay;
	ax += pobj->x;
	az += pobj->z;

	tx = ball_obj.odata.x + time * ball_obj.odata.vx;
	ty = ball_obj.odata.y + (time * ball_obj.odata.vy) + (0.5f * BALL_GRAVITY * (time * time));
	tz = ball_obj.odata.z + time * ball_obj.odata.vz;

	pobj->vx = (tx - ax) / time;
	pobj->vy = ((ty - ay) / time) - (0.5f * GRAVITY * time);
	pobj->vz = (tz - az) / time;
	

	vxz = fsqrt( pobj->vx * pobj->vx + pobj->vz * pobj->vz );

	// adjust Y vel
	if (pobj->vy > max_yv)
		pobj->vy = max_yv;

	if (pobj->vy < min_yv)
		pobj->vy = min_yv;

	// adjust XZ vel
	if (vxz > max_xzv)
	{
		pobj->vx = pobj->vx / vxz * max_xzv;
		pobj->vz = pobj->vz / vxz * max_xzv;
	}

	if (vxz < min_xzv)
	{
		pobj->vx = FRELX(0.0f,min_xzv,pobj->tgt_fwd);
		pobj->vz = FRELZ(0.0f,min_xzv,pobj->tgt_fwd);
	}

	((bbplyr_data *)pobj)->flight_time = 0;
	((bbplyr_data *)pobj)->total_time = (int)time;
//printf("QUICK REBOUND VEL(): y vels: %2.2f\n",pobj->vy);
}	

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void SetBlockShotVels(obj_3d *pobj, int seq, int frame, int joint)
{
	float	ax, ax2, ay, ay2, az, az2, tx=0.0f, ty=0.0f, tz=0.0f, time, vxz, min_xzv, max_xzv, min_yv, max_yv;
	float	rim_pt[3],dist_to_rim,dist;
	float	a,b,c,b2m4ac,root1,root2;
	int		angle_to_rim,air_time,bearing;
	bbplyr_data * ppdata = (bbplyr_data *)pobj;

	
	time = 21.0f;		//min time to get there

	// default...non-turbo velocities
//	max_xzv = 0.50f;
	max_xzv = .55f;
	min_xzv = 0.0f;
	max_yv = 0.0f;
	min_yv = 1.25f;

	switch(game_info.ball_mode)
	{
		case BM_PASSING:
			max_yv = ((player_blocks[ball_obj.int_receiver].odata.flags & PF_ALLYDUNKER) ? 1.45f : 1.30f);
			break;
		case BM_FREE_ON_COURT:
			max_yv = ((player_blocks[game_info.ball_handler].odata.flags & PF_SHOOTING) ? 1.45f : 1.30f);
			break;
		case BM_DRIBBLING:
		case BM_SHOOTING:
		case BM_BOUNCE_ON_RIM:
		case BM_OFF_INTO_PLAY:
			max_yv = 1.70f;
			break;
		default:
			max_yv = 1.70f;
		#if DEBUG
		printf("SetBlockShotVels(): reached a default jump case!!! BAD!!!\n");
		#endif
			break;
	}


	// increase if TURBO...pressed
	if (pobj->flags & PF_TURBO)
	{
		min_yv *= 1.10;
		max_yv *= 1.05;
	}

//	// jump straight up... cause the ball will hit the rim before we'll reach it!!! (let player aim)
//	if (((ball_obj.total_time - ball_obj.flight_time) < time) && (game_info.ball_mode == BM_SHOOTING))
//		max_xzv = 0.02f;

	if (joint == JOINT_BWRIST)
	{
		get_joint_position_rel(pobj, seq, frame, JOINT_LWRIST, pobj->tgt_fwd, &ax, &ay, &az, 0.0f, 0.0f, 0.0f);
		get_joint_position_rel(pobj, seq, frame, JOINT_RWRIST, pobj->tgt_fwd, &ax2, &ay2, &az2, 0.0f, 0.0f, 0.0f);
		ax = (ax + ax2) / 2; 
		ay = (ay + ay2) / 2; 
		az = (az + az2) / 2; 
	}
	else
		get_joint_position_rel(pobj, seq, frame, joint, pobj->tgt_fwd, &ax, &ay, &az, 0.0f, 0.0f, 0.0f);
//		get_joint_position_rel(pobj, seq, frame, joint, pobj->tgt_fwd, &ax, &ay, &az, 0.0f, 15.0f, 0.0f);

	ax += pobj->x;
	az += pobj->z;

	rim_pt[0] = game_info.active_goal ? RIM_X : -RIM_X;
	rim_pt[1] = 0.0f;
	rim_pt[2] = 0.0f;
	dist_to_rim = fsqrt( ((rim_pt[0] - pobj->x) * (rim_pt[0] - pobj->x)) + ((0.0f - pobj->z) * (0.0f - pobj->z)) );

	// transform target x,y,z into world coords
	tx = ball_obj.odata.x + time * ball_obj.odata.vx;
	tz = ball_obj.odata.z + time * ball_obj.odata.vz;
	ty = ball_obj.odata.y + (time * ball_obj.odata.vy) + (0.5f * BALL_GRAVITY *  (time * time));

	// jump to a point (computed) X units in front of player with ball, unless hes shooting
	if ((game_info.ball_handler != NO_BALLHANDLER) &&
		!(player_blocks[game_info.ball_handler].odata.flags & (PF_DUNKER|PF_LAYUP|PF_SHOOTING)))
	{

#if DEBUG
	printf("SetBlockShotVels(): player not in a dunk, layup or shot\n");
#endif
		angle_to_rim = ptangle( &(player_blocks[game_info.ball_handler].odata.x), rim_pt);
		dist = distxz[ppdata->plyrnum][game_info.ball_handler];

		if (distxz[ppdata->plyrnum][game_info.ball_handler] < 15.0f)
		{
			max_xzv /= 2.0f;
			tx = ball_obj.odata.x + (18.0f * isin(angle_to_rim));
			tz = ball_obj.odata.z + (18.0f * icos(angle_to_rim));
		}
		else
		{
			tx = ball_obj.odata.x + (15.0f * isin(angle_to_rim));
			tz = ball_obj.odata.z + (15.0f * icos(angle_to_rim));
		}


// With this version, I was jumping up in front of shooter, slightly to side, and
// I never drifted infront of ball.  We need some drift - it feels bad to be just off to side, and get no help
		if (((distxz[ppdata->plyrnum][game_info.ball_handler] <= 8.0f) &&	//25
			 (plyr_bearing(player_blocks[game_info.ball_handler].plyrnum, ppdata->plyrnum) <= 256)))
		{	// im very close to shooter, jump straight up!
//			max_xzv = 0.0f;
			max_xzv /= 4.0f;
			tx = player_blocks[ppdata->plyrnum].odata.x;
			tx = player_blocks[ppdata->plyrnum].odata.z;
		}
//		else
//		{
//			tx = ball_obj.odata.x + (15.0f * isin(angle_to_rim));
//			tz = ball_obj.odata.z + (15.0f * icos(angle_to_rim));
//		}

		ty = ball_obj.odata.y + 20.0f;
	}
//	else if ((dist_to_rim < 9.0f) && (plyr_behind_backboard(pobj) == NO))
//	{	// jump straight up, for GOALTENDING!
//		max_xzv = 0.00f;
//		min_xzv = 0.00f;
//	}
	else if (plyr_behind_backboard(pobj) == YES)
	{	// special case, dont jump thru backboard, jump but slide outward toward rim
		min_xzv = 0.60f;
		max_xzv = 0.60f;
		max_yv = 1.50;
		min_yv = 1.50;
	}


	pobj->vx = (tx - ax) / time;
	pobj->vy = ((ty - ay) / time) - (0.5f * GRAVITY * time);
	pobj->vz = (tz - az) / time;

	vxz = fsqrt( pobj->vx * pobj->vx + pobj->vz * pobj->vz );

	// adjust Y vel
	if (pobj->vy > max_yv)
		pobj->vy = max_yv;

	if (pobj->vy < min_yv)
		pobj->vy = min_yv;

	// adjust XZ vel
	if (vxz > max_xzv)
	{
		pobj->vx = pobj->vx / vxz * max_xzv;
		pobj->vz = pobj->vz / vxz * max_xzv;
		pobj->vy = max_yv;
	}

	if (vxz < min_xzv)
	{
		pobj->vx = FRELX(0.0f,min_xzv,pobj->tgt_fwd);
		pobj->vz = FRELZ(0.0f,min_xzv,pobj->tgt_fwd);
		pobj->vy = max_yv;
	}

	// figure out time in air
	a = GRAVITY / 2.0f;
	b = pobj->vy;
	c = 0.0f;
	b2m4ac = b * b - 4.0f * a * c;
	root1 = (-1.0f * b + fsqrt( b2m4ac )) / (2.0f * a);
	root2 = (-1.0f * b - fsqrt( b2m4ac )) / (2.0f * a);
	air_time = 1;
	if (root1 > 1.0f)
		air_time = (int)(root1+0.9f);

	if (root2 > 1.0f)
		air_time = (int)(root2+0.9f);

	// set those values
	((bbplyr_data *)pobj)->flight_time = 0;
	((bbplyr_data *)pobj)->total_time = air_time;

//	#if DEBUG
//printf("BLOCK SHOT VEL(): x,y,z vels: %2.2f %2.2f %2.2f\n",pobj->vx,pobj->vy,pobj->vz);
//	#endif
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
#define SWAT_COLL_DIST		7.0f
#define STEAL_COLL_DIST		7.0f
#define BLOCK_COLL_DIST		5.0f
#define	REBOUND_COLL_DIST	8.0f	//7.0

static int _mode_defense(bbplyr_data *ppdata)
{
	float p[3], rim_pt[3], *tm, hx, hy, hz, dist, tx, ty, tz, ball_xzvel;
	int ball_angle, limb = JOINT_RWRIST, angle, rnd = 0, chance = 100;
	int i, index = 0, time = 10, auto_has_ball = NO, goal_tended = NO, them, us;
	obj_3d *pobj = (obj_3d *)ppdata;
	float dist_to_rim;
	int on_fire;
	int	score_diff, my_score = compute_score( game_info.off_side ),
			his_score = compute_score( 1 - game_info.off_side )
			;

#ifdef DEBUG_EXTREME
	for(i = 0; i < ppdata->plyrnum; i++) fprintf(stderr, " ");
	fprintf(stderr, "ENTER: _mode_defense %d\n",ppdata->plyrnum);
#endif

	ppdata->flail_time = 0;

	slap_snd_wait = GREATER(0, slap_snd_wait - 1);
	pobj->turn_rate = 64;
	pobj->turn_rate2 = 64;

// turn toward target heading
	turn_toward_tgt( pobj );

#ifdef DEBUG_HEADS
	ppdata->stick_cur = 24;
	ppdata->but_cur = 0;
	pobj->fwd = pobj->tgt_fwd = 768;
	pobj->z = 0.0f - ((ppdata->plyrnum % NUM_PERTEAM) * 7.0f);
	pobj->x = (5.0f * (ppdata->plyrnum < NUM_PERTEAM ? -1.0f : 1.0f)) + (((ppdata->plyrnum % NUM_PERTEAM) * 5.0f) * (ppdata->plyrnum < NUM_PERTEAM ? -1.0f : 1.0f));
	return YES;
#endif

//	{
//		void * pani = rebound_auto(ppdata);		// Check for auto rebounding
//		if (pani)
//			change_anim(pobj, pani);
//	}

#if 0//DEBUG
	if(1)
	{
		//if( get_player_sw_current() & (P1_D|P2_D|P3_D|P4_D) )
		if( ((get_player_sw_current() >> (game_info.plyr_control[ppdata->plyrnum]<<3)) & P_ABCD_MASK) & P_D)
			ppdata->odata.adata[0].animode |= MODE_ANIDEBUG;
		else
			ppdata->odata.adata[0].animode &= ~MODE_ANIDEBUG;
	}
#endif

	if(!( pobj->adata[0].animode & MODE_UNINT ))
	{
		// we're not in MODE_UNINT
		if(game_info.game_mode == GM_INBOUNDING)
		{
			offense_run(pobj);
			return YES;
		}
		else
			defense_run( pobj );

		if(game_info.ball_handler != ppdata->plyrnum)
		{
			// swat or block or rebound or trying to goaltend ?
			if(ppdata->but_new & P_A)
			{
				// check for rebound
				if((game_info.ball_mode == BM_OFF_INTO_PLAY) ||
					(game_info.ball_mode == BM_BOUNCE_ON_RIM))
				{
					if (ball_obj.odata.y > RIM_Y)
						change_anim(pobj, rebound_anims[randrng(sizeof(rebound_anims)/sizeof(char *))]);
					else if (ball_obj.odata.y > RIM_Y-10.f)
						change_anim(pobj, short_rebound_anims[randrng(sizeof(short_rebound_anims)/sizeof(char *))]);
				}
				else if(game_info.ball_mode != BM_FREE_ON_COURT)
				{	// check for block/swat
					if(game_info.ball_handler != NO_BALLHANDLER)
					{	// ball handler
						// If too far away from ball owner, do a short jump - unless a pass is being sent to an ally ooper - then jump high!
						if((distxz[ppdata->plyrnum][game_info.ball_handler] >= 95.0f) &&
							ball_obj.int_receiver != NO_RECEIVER && !(player_blocks[ball_obj.int_receiver].odata.flags & (PF_ALLYDUNKER)))
						{
							change_anim(pobj, quick_jump_block_anims[randrng(sizeof(quick_jump_block_anims)/sizeof(char *))]);
						}
						else if(player_blocks[game_info.ball_handler].odata.flags & PF_SHOOTING)
						{	// player shooting ball
							if((((pobj->x < player_blocks[game_info.ball_handler].odata.x) && (game_info.active_goal)) ||
							 	 ((pobj->x >= player_blocks[game_info.ball_handler].odata.x) && (!game_info.active_goal))) &&
								(distxz[ppdata->plyrnum][game_info.ball_handler] < 12.0f))
							{
								change_anim(pobj, swat_1h_behind_anim);
							}
							else
								change_anim(pobj, swat_n_block_anims[randrng(sizeof(swat_n_block_anims)/sizeof(char *))]);
						}
						else if((player_blocks[game_info.ball_handler].odata.flags & (PF_LAYUP|PF_DUNKER)) &&
								 (((pobj->x < player_blocks[game_info.ball_handler].odata.x) && (game_info.active_goal)) ||
							 	 ((pobj->x >= player_blocks[game_info.ball_handler].odata.x) && (!game_info.active_goal))))
							{
								change_anim(pobj, swat_1h_behind_anim);
							}
						else
							change_anim(pobj, block_anims[randrng(sizeof(block_anims)/sizeof(char *))]);
					}
					else
					{	// no ball handler
						rim_pt[0] = game_info.active_goal ? RIM_X : -RIM_X; rim_pt[1] = rim_pt[2] = 0.0f;
						dist_to_rim = fsqrt( ((rim_pt[0] - pobj->x) * (rim_pt[0] - pobj->x)) + ((0.0f - pobj->z) * (0.0f - pobj->z)) );
						
						// If on fire, always do swats at ball when near rim
						if ((dist_to_rim < 35.0f) && (game_info.ball_mode == BM_SHOOTING)
						&& (((ppdata->plyrnum == game_info.fire_player) && (game_info.fire_cnt >= FIRECNT))
						|| (game_info.team_fire == player_blocks[ppdata->plyrnum].team)))
						{
							if(compute_score(ppdata->team) > compute_score(!ppdata->team))
								change_anim(pobj, swat_anims[randrng(sizeof(swat_anims)/sizeof(char *))]);
							else
								// Losing team will do some blocks (keep ball) instead of swats
								change_anim(pobj, swat_fire_anims[randrng(sizeof(swat_fire_anims)/sizeof(char *))]);
						}
						else
							change_anim(pobj, swat_n_block_anims[randrng(sizeof(swat_n_block_anims)/sizeof(char *))]);
					}
				}
			}
			// push ??
			if((ppdata->but_cur & P_B) && (pobj->vy == 0.0f))
			{
				if((ppdata->but_cur & P_C) && (pobj->flags & PF_TURBO))
				{
					if(pdata_blocks[ppdata->plyrnum].turbo >= 25.0f || ISDRONE(ppdata))
					{
						if(ISHUMAN(ppdata))
							pdata_blocks[ppdata->plyrnum].turbo -= 25.0f;
						if(ppdata->but_new & P_B)
						{
							change_anim(pobj, push_anims[randrng(sizeof(push_anims)/sizeof(char *))]);
							if(ppdata->stick_cur != 24)
							{
								if(fabs(pobj->z) <= 72.0f && fabs(pobj->x) <= 124.0f)
									pobj->tgt_fwd = 1023 & (dir49_table[ppdata->stick_cur] + (int)RD2GR(cambot.theta));
							}
							ppdata->but_new &= ~(P_B);
							ppdata->but_cur &= ~(P_B);
						}
					}
				}
				else if(((pobj->vx == 0.0f) && (pobj->vz == 0.0f))
				|| (pobj->adata[0].seq_index == (SEQ_DSLIDE_R)
						|| pobj->adata[0].seq_index == (SEQ_DSLIDE_L)
						|| pobj->adata[0].seq_index == (SEQ_DSLID2_R)
						|| pobj->adata[0].seq_index == (SEQ_DSLID2_L)
						|| pobj->adata[0].seq_index == (SEQ_DSLIDE_F)
						|| pobj->adata[0].seq_index == (SEQ_DSLIDE_B)
						))
				{
					if(ISDRONE(ppdata) && game_info.ball_mode == BM_FREE_ON_COURT)
					{
						pobj->flags &= ~PF_STEALING;
#ifdef DEBUG
//						printf("DRONE: %s is trying to steal a free ball - IGNORED\n", ppdata->p_spd->szlast);
#endif
//						while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
					}
					else
					{
						if(ball_obj.odata.y < 8.0f)
							change_anim(pobj, swipe_low_anim);
						else if(ball_obj.odata.y < 13.0f)
							change_anim(pobj, swipe_mid_anim);
						else
							change_anim(pobj, swipe_high_anim);
					}
				}
				else // player is moving and pressed steal and no turbo button
				{
					if(ppdata->but_new & P_B)
					{
						if(!(pobj->adata[1].animode & MODE_TWOPART))
						{
							begin_twopart_mode(pobj);
							//change_anim2(pobj, (ppdata->handedness == RIGHT_HAND ? lunge1_r_anim : lunge1_l_anim));
							change_anim2(pobj, swipe3_anim_run);
//						printf("%s is doing a moving steal\n", ppdata->p_spd->szlast);
						}
					}	
				}
			}
		}// END ! ball.handler
	}// END ! UNINT


////////////////////////////////////
// Handle collisions with the ball
////////////////////////////////////
	if(game_info.game_mode == GM_INBOUNDING)
		return YES;

	//
	// handle player defensive moves logic
	//
	if ((pobj->flags & PF_STEALING) &&
		(ppdata->no_ball_collide <= 0))
//		(OOBPasser == NO_PASSER))
	{
		defensive_steal_logic(ppdata);
	}
	else if ((pobj->flags & PF_SWATING) &&
			 (ppdata->no_ball_collide <= 0))
//			 (OOBPasser == NO_PASSER))
	{
		defensive_swat_logic(ppdata);
	}
	else if ((pobj->flags & PF_IN_AIR_BLOCK) &&
		  	 (ppdata->no_ball_collide <= 0))
//		  	 (OOBPasser == NO_PASSER))
	{
		defensive_block_logic(ppdata);
	}
	else if ((pobj->flags & PF_REBOUND) &&
			  (ppdata->no_ball_collide <= 0))
//			  (OOBPasser == NO_PASSER))
	{
		rebound_logic(ppdata);
	}

	tm = cambot.xform;
	limb = ppdata->ball_in_hand == RIGHT_HAND ? JOINT_RWRIST : JOINT_LWRIST;
	hx = ppdata->jpos[limb][0] * tm[0] + ppdata->jpos[limb][1] * tm[4] + ppdata->jpos[limb][2] * tm[8] + cambot.x;
	hy = ppdata->jpos[limb][0] * tm[1] + ppdata->jpos[limb][1] * tm[5] + ppdata->jpos[limb][2] * tm[9] + cambot.y;
	hz = ppdata->jpos[limb][0] * tm[2] + ppdata->jpos[limb][1] * tm[6] + ppdata->jpos[limb][2] * tm[10] + cambot.z;
	dist = fsqrt(((ball_obj.odata.x - hx) * (ball_obj.odata.x - hx)) +
				 ((ball_obj.odata.y - hy) * (ball_obj.odata.y - hy)) +
				 ((ball_obj.odata.z - hz) * (ball_obj.odata.z - hz)));

	// If ball is loose and i'm close...get it 
	if(
//	else if(
			(game_info.ball_handler == NO_BALLHANDLER
			&& OOBPasser == NO_PASSER
			&& !(pobj->flags & PF_CELEBRATE)
			&& !(ball_obj.flags & BF_NOINTERCEPT)
			&& game_info.ball_mode != BM_SHOOTING)
		//||
		//	(auto_has_ball == YES
		//	//&& !(ball_obj.flags & BF_NOINTERCEPT)
		//	&& !(pobj->flags & PF_CELEBRATE)
		//	&& game_info.ball_mode != BM_SHOOTING)
		||
			(game_info.ball_handler == NO_BALLHANDLER
			//&& !(ball_obj.flags & BF_NOINTERCEPT)
			&& OOBPasser != NO_PASSER
			&& game_info.ball_mode == BM_FREE_ON_COURT
			&& player_blocks[NUM_PERTEAM * game_info.off_side].odata.plyrmode != MODE_OUT_OF_BOUNDS)
		)
	{
//#ifdef DEBUG
//		if(ppdata->no_ball_collide <= 0
//		&& (ball_hit_me(pobj) == YES && (game_info.ball_mode == BM_BOUNCE_ON_RIM || game_info.ball_mode == BM_OFF_INTO_PLAY) && dist > 3.0f) )
//	printf("DEFENSE: REGULAR hit %s, bmode = %s\n, THIS WOULD'VE GLITCHED THE BALL TO HIS HAND!\n", ppdata->p_spd->szlast, ball_mode_names[game_info.ball_mode + 2]);
//	while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
//#endif
	
		if(ppdata->no_ball_collide <= 0
		&& (  (ball_hit_me(pobj) == YES && game_info.ball_mode != BM_BOUNCE_ON_RIM)
			 || (ball_hit_me(pobj) == YES && (game_info.ball_mode == BM_BOUNCE_ON_RIM || game_info.ball_mode == BM_OFF_INTO_PLAY) && dist <= 1.5f) )
		)
		//|| auto_has_ball == YES)	//auto_has_ball???
		{
			// ...and between pelvis and a point five feet above it
			//if( ((ball_obj.odata.y > (pobj->y + pobj->ay - 4.0f)) && (ball_obj.odata.y < (pobj->y + pobj->ay + 14.0f )))
			//		|| (pobj->y <= 1.0f && ball_obj.odata.y <= 12.0f)
			//		|| (auto_has_ball == YES)//???
			//		)
		//if(ball_hit_me(pobj) == YES || auto_has_ball == YES)
		//{
		#if 0
			if(ball_obj.odata.y <= 6.0f && pobj->adata[0].seq_index != (SEQ_PICKUP_1) && pobj->y == 0.0f && pobj->ay >= 3.0f)
			{
				auto_rebounding = NO;
				ball_obj.odata.vx = ball_obj.odata.vz = ball_obj.odata.vy = 0.0f;
				ball_obj.odata.y = BALL_RADIUS;
				ppdata->no_ball_collide = 6;
				ppdata->odata.tgt_fwd = ptangle( &(ppdata->odata.x), &(ball_obj.odata.x));
				change_anim( pobj, ball_pickup_1 );
				no_pump_fake = YES;
				return 1;
			}
		#endif

			// Guy up in air after a blocked shot, in a block seq probably, should I attach to ball?

			if((ball_obj.odata.vy < -0.30f) && (game_info.ball_mode == BM_SHOOTING))
			 	goal_tended = YES;

			if(pobj->y > 0.0f && dist > 3.5f && !goal_tended)
			{
#ifdef DEBUG
//				printf("%s: BALL to far from in-air player to attach - IGNORING\n",ppdata->p_spd->szlast);
//				while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
#endif
				return YES;
			}

#if 0		/* FIX THIS NOW */
			// Grab the ball
			if(game_info.off_side != ppdata->team && !goal_tended)
			{
				//printf("There was a change of possesion\n");
				change_possession();
			}
#else
			// Grab the ball - unless this is a fire def guy, who is going to probably blast the ball away, and not
			// have a goaltend called on him
			if(game_info.off_side != ppdata->team && !goal_tended)
			{
#ifdef DEBUG
				printf("player flags: %x\n",ppdata->odata.flags);
#endif
				
				on_fire = ((game_info.fire_player == ppdata->plyrnum) && (game_info.fire_cnt >= FIRECNT)) ||
					(game_info.team_fire == player_blocks[ppdata->plyrnum].team);

				if (!on_fire || (on_fire && ((ppdata->odata.flags & (PF_SWATING | PF_IN_AIR_BLOCK)) == 0)))
				{
#ifdef DEBUG
						printf("There was a change of possession\n");
#endif
						change_possession();
				}
			}
#endif


#ifdef DEBUG
			printf("Defender %s picked up the ball\n", ppdata->p_spd->szlast);
			//printf("ball_obj.flight_time = %d, total_time = %d\n", ball_obj.flight_time, ball_obj.total_time);
#endif
			// Stole inbounds pass?  Restart clock
			if(game_info.game_mode == GM_INBOUNDING)
				clock_active = 1;	   // Restart game clock

			// Reset some essentials
			OOBPasser				= NO_PASSER;
			game_info.ball_handler  = ppdata->plyrnum;
			game_info.game_mode     = GM_IN_PLAY;
			//game_info.shooter     = NO_SHOOTER;
			game_info.passer       	= NO_PASSER;
			game_info.ball_mode    	= BM_DRIBBLING;
			auto_rebounding = NO;

			//ball_obj.who_shot			 = NO_SHOOTER;

			ppdata->dribble_mode	 = DM_CAN_DRIBBLE;	
			no_pump_fake					 = YES;

			flash_ball(4);
			// swat ball sound
			snd_scall_bank(gameplay_bnk_str,STEAL1_SND,VOLUME7,127,PRIORITY_LVL4);

		 	ball_rotclr();
			if(goal_tended == NO)
			{
				//printf("CROWD SWELL - change possesion\n");
//				if (COURT_IS_INDOORS)
				if (1)
				{
					i = randrng(100);
					if (i < 6)
						snd_scall_bank(crowd_bnk_str, CROWD_BOO_SND, (randrng(100) < 50) ? VOLUME6 : VOLUME5, 127, PRIORITY_LVL3);
					else if (i < 40)
						snd_scall_bank(crowd_bnk_str, CROWD2_SWELL1_SND, (randrng(100) < 50) ? VOLUME6 : VOLUME5, 127, PRIORITY_LVL3);
					else if (i < 60)
						snd_scall_bank(crowd_bnk_str, CROWD2_SWELL2_SND, (randrng(100) < 50) ? VOLUME6 : VOLUME5, 127, PRIORITY_LVL3);
					else
						snd_scall_bank(crowd_bnk_str, CROWD2_SWELL3_SND, (randrng(100) < 50) ? VOLUME6 : VOLUME5, 127, PRIORITY_LVL3);
				}
				what_type_shot = three_pt_check(pobj) ? JUMPER_3 : JUMPER_2;
			}
			else
			{
#ifdef DEBUG
				printf("%s: is about to goaltend\n", ppdata->p_spd->szlast);
//				while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
#endif
				do_goaltending_logic();

				// inc. game stat - will inc blocks on goaltends also!
				plyr_stat_ptrs[ppdata->plyrnum]->blocks++;

			}
			
		//}
		}
	}

// check to see if body collided with the ball...
	ball_xzvel = fsqrt((ball_obj.odata.vx * ball_obj.odata.vx) + (ball_obj.odata.vz * ball_obj.odata.vz));
	ball_angle = (int)((atan2(ball_obj.odata.vx, ball_obj.odata.vz) / M_PI) * 512.0f);
	ball_angle &= 1023;
	ball_angle = (ball_obj.odata.vx == 0.0f && ball_obj.odata.vz == 0.0f) ? 0 : ball_angle;

#define DEFLECT_RANGE	200
//	if ((game_info.ball_handler == NO_BALLHANDLER) &&
//		(game_info.ball_mode == BM_PASSING) &&
//		(pobj->y == 0.0f))
	if(game_info.ball_handler == NO_BALLHANDLER
	&& (game_info.ball_mode == BM_PASSING
	 || game_info.ball_mode == BM_SHOOTING
	 //|| game_info.ball_mode == BM_OFF_INTO_PLAY
	 || game_info.ball_mode == BM_FREE_ON_COURT)
	&& !(ppdata->odata.adata[0].animode & MODE_REACTING) )
	//&& !(ppdata->odata.adata[0].animode & MODE_REACTING || ppdata->odata.flags & (PF_SWATING | PF_IN_AIR_BLOCK)) )
	{
	int goaltended = NO, rnd = randrng(220);
	float pcent;

		pcent = 0.0f;		/* what is the right value to init to? 0 or 50 */
		if ((ball_obj.total_time > 0) && (ball_obj.flight_time >= 10))
		{
			pcent = (float)ball_obj.flight_time / (float)ball_obj.total_time;
		}
		// Don't allow deflecting a pass in first x ticks
		else if (((ball_obj.total_time > 0) && (ball_obj.flight_time < 10)) / 2)
			pcent = 0.0f;

		if(game_info.ball_mode == BM_SHOOTING)
		{
			// Need to check goaltending
			// Shot ball must be colliding with a swatter?
			pcent = 1.0f;	//1.0f
			rnd = 0;
		}
		else if ((game_info.ball_mode == BM_PASSING) && (player_blocks[ball_obj.int_receiver].odata.flags & PF_ALLYDUNKER))
		{
			// Need to check goaltending
			// Shot ball must be colliding with a swatter?
			pcent = 1.0f;	//1.0f
			rnd = 0;
		}

		if ((ppdata->plyrnum == game_info.fire_player) && (game_info.fire_cnt >= FIRECNT))
		{
			rnd = 0;
		}
		if (game_info.team_fire == player_blocks[ppdata->plyrnum].team)
		{
			rnd = 0;
		}

	 	if(
			(game_info.ball_mode == BM_SHOOTING
		|| game_info.ball_mode == BM_PASSING
		|| game_info.ball_mode == BM_OFF_INTO_PLAY
		|| game_info.ball_mode == BM_FREE_ON_COURT)
		&& ball_hit_me(pobj) == YES
		&& ignore_steal <= 0
		&& pcent >= .1f
		&& what_type_shot != DUNK
		&& ppdata->no_ball_collide <= 0
		&& game_info.game_mode != GM_INBOUNDING
		&& (rnd <= get_keepball_base(ppdata)))
		//&& (randrng(220) <= get_keepball_base(ppdata)))
		{
		int stick_to_hand = YES;

#ifdef DEBUG
		printf("DEFENSE: cylinder hit %s, bmode = %s\n", ppdata->p_spd->szlast, ball_mode_names[game_info.ball_mode + 2]);
#endif

//			if((ball_obj.flight_time > (ball_obj.total_time/2)) && (game_info.ball_mode == BM_SHOOTING))
			// We allow some downward velocity on the ball, to allow some arguments, and to get rid of some GT's
			if((ball_obj.odata.vy < -0.30f) && (game_info.ball_mode == BM_SHOOTING))
			 	goaltended = YES;

		// don't want the ball attaching to a defender if ball isn't near a wrist
			if(game_info.ball_mode == BM_SHOOTING && dist > 2.0f && pobj->y > 6.0f)
				stick_to_hand = NO;


			if((ppdata->plyrnum == game_info.fire_player) && (game_info.fire_cnt >= FIRECNT))
			{
				goaltended = NO;
				stick_to_hand = YES;
			}

			if(game_info.team_fire == player_blocks[ppdata->plyrnum].team)
			{
				goaltended = NO;
				stick_to_hand = YES;
			}

//			if (what_type_shot == DUNK)
//				goaltended = NO;

			// Don't allow drones to goaltend unless their tmate is on fire, or is about to get on fire
			if ((ISDRONE(&player_blocks[ppdata->plyrnum]))
				&& (goaltended == YES)
				&& !(((ppdata->plyrnum ^ 1 ) == game_info.fire_player) && (game_info.fire_cnt >= FIRECNT)))
			{
#ifdef DEBUG
				printf("Throwing away a cylinder goaltend by a drone!!\n");
//				while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
#endif
				goaltended = NO;
				return YES;
			}

			if(ppdata->odata.flags & (PF_SWATING|PF_IN_AIR_BLOCK))
			{	// swat ball
			int team = !ppdata->team;
			float vel;

				// inc. game stat - will inc blocks on goaltends also!
				plyr_stat_ptrs[ppdata->plyrnum]->blocks++;

				// don't want the ball attaching to a defender if ball isn't near a wrist
				if(dist <= 8.0f)
				{
				 	// Put ball in swatters hand before it goes
					ball_obj.odata.x = ppdata->jpos[limb][0] * tm[0] + ppdata->jpos[limb][1] * tm[4] + ppdata->jpos[limb][2] * tm[8] + cambot.x;
					ball_obj.odata.y = ppdata->jpos[limb][0] * tm[1] + ppdata->jpos[limb][1] * tm[5] + ppdata->jpos[limb][2] * tm[9] + cambot.y;
					ball_obj.odata.z = ppdata->jpos[limb][0] * tm[2] + ppdata->jpos[limb][1] * tm[6] + ppdata->jpos[limb][2] * tm[10] + cambot.z;
#ifdef DEBUG
					printf("Attaching to wrist! - on cylinder hit\n");
#endif
				}
#ifdef DEBUG
				else
					printf("Didn't attach to wrist - too far on swat hit: %f\n",dist);
#endif

				if (randrng(100) < 50)
					angle = pobj->fwd - randrng(64);
				else
					angle = pobj->fwd + randrng(64);

				vel = ((float)randrng(1650)) / 1000.0f;
				if(vel < 0.55f)
					vel = .75f;

//				if (randrng(100) < 3)
//					angle = pobj->fwd + 512;

				ball_obj.odata.vx = FRELX(0.0f,vel,angle);
				ball_obj.odata.vz = FRELZ(0.0f,vel,angle);



				if(dist > 8.0f)
				{
					// Ball didn't really hit wrist, so just bnc off body without much vels
					ball_obj.odata.vx *= 0.5f;
					ball_obj.odata.vz *= 0.5f;
				}


				ball_obj.odata.vy = ((float)randrng(1200)) / 1000.0f;

				no_pump_fake = YES;
				game_info.ball_handler = NO_BALLHANDLER;
				game_info.passer    = NO_PASSER;
				game_info.ball_mode = BM_FREE_ON_COURT;
				//ball_obj.who_shot   = NO_SHOOTER;
				ppdata->dribble_mode = DM_CAN_DRIBBLE;	
				ppdata->no_ball_collide = 15;

				player_blocks[2 * team + 0].no_ball_collide =
				player_blocks[2 * team + 1].no_ball_collide = 15;

				ball_sparkcreate (2, 10, 4);

				// swat ball sound
				snd_scall_bank(gameplay_bnk_str,STEAL1_SND,VOLUME7,127,PRIORITY_LVL5);
				snd_scall_bank(gameplay_bnk_str,STEAL1_SND,VOLUME7,127,PRIORITY_LVL5);
				flash_ball(4);	// flash the ball

				if(goaltended)
					do_goaltending_logic();
				else
					qcreate("swat_spch", SPEECH_PID, swated_ball_speech_proc, ppdata->plyrnum, 0, 0, 0);
			}
			else
			if((goaltended == YES) && (what_type_shot == LAYUP) && (randrng(100) <= 75 &&
			(game_info.team_fire != player_blocks[ppdata->plyrnum].team
				&& ((ppdata->plyrnum != game_info.fire_player) && (game_info.fire_cnt < FIRECNT)))))
			{
				goaltended = NO;
	 			flash_ball(4);
				if(randrng(100) < 20)
					snd_scall_bank(gameplay_bnk_str,STEAL1_SND,VOLUME5,127,PRIORITY_LVL4);
#ifdef DEBUG
				printf("DEFENSE: Tossing away a cylinder'd layup!\n");
//				while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
#endif
			}
	 		else if(goaltended == YES
			|| (stick_to_hand == YES && game_info.ball_mode == BM_SHOOTING)
			|| (randrng(100) <= 5 && game_info.ball_mode != BM_SHOOTING)
			|| (game_info.ball_mode == BM_FREE_ON_COURT))
	 		{	// defender stole the ball...
	 		
	 		
	 		
	 		
	 			OOBPasser				= NO_PASSER;
	 			game_info.ball_handler	= ppdata->plyrnum;
	 			game_info.game_mode    	= GM_IN_PLAY;
	 			//game_info.shooter     = NO_SHOOTER;
	 			game_info.passer       	= NO_PASSER;
	 			//ball_obj.who_shot		= NO_SHOOTER;
	 			ppdata->dribble_mode	= DM_CAN_DRIBBLE;
	 			auto_rebounding = NO;


				// inc. game stat - will inc blocks on goaltends also!
				plyr_stat_ptrs[ppdata->plyrnum]->blocks++;


	 			if(game_info.off_side != ppdata->team && !goaltended)
	 				change_possession();

				if(goaltended == NO)
					intercepted_speech();

	 		 	ball_rotclr();

	 			if(game_info.ball_mode == BM_PASSING && ball_obj.int_receiver != NO_RECEIVER)
	 			{
	 			int plyr = ball_obj.int_receiver;

	 				if(player_blocks[plyr].odata.adata[1].animode & MODE_TWOPART)
	 					change_anim2( (obj_3d *)(player_blocks + plyr), back_into_run_from_catch);
	 				else
	 				if(player_blocks[plyr].odata.adata[0].animode & MODE_UNINT
	 				&& player_blocks[plyr].odata.vx == 0
	 				&& player_blocks[plyr].odata.vz == 0
	 				&& player_blocks[plyr].odata.y == 0)
	 				{
	 					change_anim( (obj_3d *)(player_blocks + plyr), breath_anims[randrng(sizeof(breath_anims)/sizeof(char *))]);
	 				}
	 			}
	 			game_info.ball_mode    = BM_DRIBBLING;
	 			flash_ball(4);
				snd_scall_bank(gameplay_bnk_str,STEAL1_SND,VOLUME7,127,PRIORITY_LVL4);
				// loose ball snd 
				snd_scall_bank(gameplay_bnk_str, LOOSE_BALL_SND, VOLUME7, 127, PRIORITY_LVL5);

				if(goaltended == YES)
					do_goaltending_logic();
				else
					what_type_shot = three_pt_check(pobj) ? JUMPER_3 : JUMPER_2;
	 		}
	 		else
	 		{	// ball was deflected...
	 			OOBPasser				= NO_PASSER;
	 			game_info.ball_handler 	= NO_BALLHANDLER;
	 			game_info.game_mode    	= GM_IN_PLAY;
	 			//game_info.shooter     = NO_SHOOTER;
	 			game_info.passer       	= NO_PASSER;
	 			//ball_obj.who_shot		= NO_SHOOTER;
	 			ppdata->dribble_mode	= DM_CAN_DRIBBLE;
	 			auto_rebounding = NO;

				// inc. game stat - will inc blocks on goaltends also!
				plyr_stat_ptrs[ppdata->plyrnum]->blocks++;


				// deflect the ball
//	 			ball_angle = ((ball_angle - DEFLECT_RANGE) + randrng(DEFLECT_RANGE * 2)) & 1023;
	 			ball_angle = ((ball_angle - DEFLECT_RANGE) + randrng(DEFLECT_RANGE * 3)) & 1023;
	 			ball_obj.odata.vx = FRELX( 0.0f, LESSER(ball_xzvel, 3.0f) * .55f, ball_angle );
	 			ball_obj.odata.vz = FRELZ( 0.0f, LESSER(ball_xzvel, 3.0f) * .55f, ball_angle );

#ifdef DEBUG
//				printf("Cylinder ball collide - ball goes up?\n");
//				fprintf(stderr,"Ball\tx= %f\ty= %f\tz= %f\r\n",ball_obj.odata.vx,ball_obj.odata.vy,ball_obj.odata.vz);
//				while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
#endif
	 			if(ball_obj.odata.vy >= 0.0f)
		 			ball_obj.odata.vy *= 1.25f;
				else
		 			ball_obj.odata.vy *= .9f;

	 			if(ball_obj.odata.vy >= 1.5f)
		 			ball_obj.odata.vy = 1.5f;

	 			flash_ball(4);
				snd_scall_bank(gameplay_bnk_str,STEAL1_SND,VOLUME7,127,PRIORITY_LVL4);

				//if(game_info.ball_mode == BM_PASSING)
					snd_scall_bank(gameplay_bnk_str, LOOSE_BALL_SND, VOLUME7, 127, PRIORITY_LVL5);

				deflected_ball_speech();
//	 			ppdata->no_ball_collide = 25 - 8 * (ppdata->_stealing / 20);
	 			ppdata->no_ball_collide = (randrng(10) + 20);

	 			if(game_info.ball_mode == BM_PASSING && ball_obj.int_receiver != NO_RECEIVER)
	 			{
	 			int plyr = ball_obj.int_receiver;

	 				if(player_blocks[plyr].odata.adata[1].animode & MODE_TWOPART)
	 					change_anim2( (obj_3d *)(player_blocks + plyr), back_into_run_from_catch);
	 				else
	 				if(player_blocks[plyr].odata.adata[0].animode & MODE_UNINT
	 				&& player_blocks[plyr].odata.vx == 0
	 				&& player_blocks[plyr].odata.vz == 0
	 				&& player_blocks[plyr].odata.y == 0)
	 				{
	 					change_anim( (obj_3d *)(player_blocks + plyr), breath_anims[randrng(sizeof(breath_anims)/sizeof(char *))]);
	 				}
	 			}

//				what_type_shot = three_pt_check(pobj) ? JUMPER_3 : JUMPER_2;

	 			game_info.ball_mode = BM_FREE_ON_COURT;
	 		}
	 	}
	}

#ifdef DEBUG_EXTREME
	for(i = 0; i < ppdata->plyrnum; i++) fprintf(stderr, " ");
	fprintf(stderr, "ENTER: _mode_defense %d\n",ppdata->plyrnum);
#endif

return YES;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------

int ball_hit_me(obj_3d *pobj)
{
float *tm,rhy,lhy,lfy,rfy,fy,ty;
bbplyr_data *ppdata = (bbplyr_data *)pobj;

	if(odistxz(pobj, (obj_3d *)&ball_obj) <= 8.0f)	//(5.0f + 3.0f * (pobj->flags & PF_DUNKER)))
	{	// now do a more detailed check on the extreme y's on each ligament...
		tm = cambot.xform;

		ny  = ppdata->jpos[JOINT_NECK][0]   * tm[1] + ppdata->jpos[JOINT_NECK][1]   * tm[5] + ppdata->jpos[JOINT_NECK][2]   * tm[9] + cambot.y;
		rhy = ppdata->jpos[JOINT_RWRIST][0] * tm[1] + ppdata->jpos[JOINT_RWRIST][1] * tm[5] + ppdata->jpos[JOINT_RWRIST][2] * tm[9] + cambot.y;
		lhy = ppdata->jpos[JOINT_LWRIST][0] * tm[1] + ppdata->jpos[JOINT_LWRIST][1] * tm[5] + ppdata->jpos[JOINT_LWRIST][2] * tm[9] + cambot.y;

		rfy = ppdata->jpos[JOINT_RANKLE][0] * tm[1] + ppdata->jpos[JOINT_RANKLE][1] * tm[5] + ppdata->jpos[JOINT_RANKLE][2] * tm[9] + cambot.y;
		lfy = ppdata->jpos[JOINT_LANKLE][0] * tm[1] + ppdata->jpos[JOINT_LANKLE][1] * tm[5] + ppdata->jpos[JOINT_LANKLE][2] * tm[9] + cambot.y;

		fy = rfy <= lfy ? rfy : lfy;
		ty = rhy >= lhy ? rhy : lhy;
		ty = ny >= ty ? ny : ty;

		// This causes bad cylinder collisions
		if(ny >= rhy && ny >= lhy)
			ty += 1.0f;

#ifdef DEBUG_REBOUNDS
//		if(game_info.ball_mode == BM_OFF_INTO_PLAY)// || game_info.ball_mode == BM_FREE_ON_COURT) 
//			printf("%s: Looking to snag! ball_y = %3.3f, ty = %3.3f, fy = %3.3f\n",
//				ppdata->p_spd->szlast, ball_obj.odata.y, ty, fy);
#endif

		if(ball_obj.odata.y >= fy && ball_obj.odata.y <= ty)
		{
#ifdef DEBUG
//			printf("%s: First cylinder y hit! ball_y = %3.3f, ty = %3.3f, fy = %3.3f\n",
//				ppdata->p_spd->szlast, ball_obj.odata.y, ty, fy);
#endif
			return YES;
		}

		// Also get ball when it's on grnd
		if(fabs(ball_obj.odata.y - fy) <= 3.5f
		|| fabs(ball_obj.odata.y - ty) <= 1.5f)
		{
#ifdef DEBUG
//			printf("%s: Second cylinder y hit!\n", ppdata->p_spd->szlast);
#endif
			return YES;
		}
	}

return NO;
}

/********************************
 Try auto rebound
********************************/

void * rebound_auto (bbplyr_data * pl_p)
{
	void * pani = NULL;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#if	1
	if ((game_info.game_mode != GM_INBOUNDING)
//		&& (pl_p->odata.plyrmode != MODE_DEFENSE)
		&& (!auto_rebounding)
		&& (!(pl_p->odata.adata[0].animode & MODE_UNINT))
		&& (game_info.ball_handler == NO_BALLHANDLER)
		&& (game_info.ball_mode == BM_OFF_INTO_PLAY || game_info.ball_mode == BM_FREE_ON_COURT)
		)
	{
		if(!(pl_p->odata.flags & (PF_REBOUND|PF_SHOOTING|PF_PASSING|PF_DUNKER|PF_ALLYDUNKER|PF_STEALING|PF_SWATING|PF_LAYUP))) {

			if (pl_p->odata.y <= 0)
			{
				ball_data * bd_p = &ball_obj;
				float fx = pl_p->odata.x - (bd_p->odata.x + bd_p->odata.vx * 16);	//16
				float fz = pl_p->odata.z - (bd_p->odata.z + bd_p->odata.vz * 16);	//16
				float f  = fx * fx + fz * fz;

//				printf ("ARebnd: dist %f\n", fsqrt(f));

				if (f < SQR(DRN_FOOT * 4) )	//5
				{
					float fy = bd_p->odata.vy + BALL_GRAVITY * 5;
					fy = bd_p->odata.y + fy * 10;	//10

					// Ball higher than me?
					if (fy > (DRN_FOOT * 8) )
					{
						// Not too high?
						if (fy <= (DRN_FOOT * 14) )
						{
							if (bd_p->odata.vy < -BALL_GRAVITY * 2)
							{
								auto_rebounding = YES;

								// inc. game stat
								plyr_stat_ptrs[pl_p->plyrnum]->rebounds++;

								if (fy > RIM_Y-5.0f)
									pani = rebound_anims[randrng(sizeof(rebound_anims)/sizeof(char *))];
								else
									pani = short_rebound_anims[randrng(sizeof(short_rebound_anims)/sizeof(char *))];
#ifdef DEBUG
								printf ("ARebnd: y %f\n", fy);
#endif
							}
						}
					}
				}
			}
		}
	}
#endif
	return pani;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void defensive_steal_logic(bbplyr_data *ppdata)
{
	float p[3], *tm, hx, hy, hz, dist, tx, ty, tz, time, vel;
	int limb, angle, rnd = 0, chance = 100;
	int	score_diff,dribbler = 0;
	int my_score = compute_score(game_info.off_side);
	int his_score = compute_score(1 - game_info.off_side);
	obj_3d	*pobj = (obj_3d *)ppdata;
	int smack_away = NO;
	int my_stealing = ppdata->_stealing;
	int his_dribbling = 0;

	// cant steal ball until its inbounded!!!
	if (OOBPasser != NO_PASSER)
		return;

	// cant steal ball if no one is holding it!!
	if (game_info.ball_handler == NO_BALLHANDLER)
		return;

	// dont keep stealing...if I already have ball
	if (game_info.ball_handler == ppdata->plyrnum)
		return;

// can't steal a ball from juker in 3rd or greater juke attempt...
	if((player_blocks[game_info.ball_handler].odata.flags & PF_FUNKY_DRIBBLE)
	&& player_blocks[game_info.ball_handler].action > 2)
	{
		if(randrng(100) < 80)
			return;
	}

	dribbler = game_info.ball_handler;

	// transform target x,y,z into world coords
	tm = cambot.xform;

	if ((pobj->adata[0].seq_index  == (SEQ_SWIPE1_L)) ||
		(pobj->adata[0].seq_index  == (SEQ_F_ST2_L)) ||
		(pobj->adata[0].seq_index  == (SEQ_F_ST4_L)))
		limb = JOINT_LWRIST;
	else
		limb = JOINT_RWRIST;

	hx = ppdata->jpos[limb][0] * tm[0] + ppdata->jpos[limb][1] * tm[4] + ppdata->jpos[limb][2] * tm[8] + cambot.x;
	hy = ppdata->jpos[limb][0] * tm[1] + ppdata->jpos[limb][1] * tm[5] + ppdata->jpos[limb][2] * tm[9] + cambot.y;
	hz = ppdata->jpos[limb][0] * tm[2] + ppdata->jpos[limb][1] * tm[6] + ppdata->jpos[limb][2] * tm[10] + cambot.z;

	dist = fsqrt(((ball_obj.odata.x - hx) * (ball_obj.odata.x - hx)) + ((ball_obj.odata.z - hz) * (ball_obj.odata.z - hz)));

	// if hand is near the ball make it known
	if ((dist <= STEAL_COLL_DIST) && (fabs(hy - ball_obj.odata.y) <= 5.0f))
	{
		ppdata->odata.flags &= ~(PF_PLYRFLAGS);

		flash_ball(4);

		if(slap_snd_wait <= 0)
		{
			snd_scall_bank(gameplay_bnk_str,STEAL1_SND,VOLUME5,127,PRIORITY_LVL4);
			slap_snd_wait = 12;
		}

		// handicap logic goes here....
		// TODO: rules go here
		if(player_blocks[game_info.ball_handler].odata.flags & (PF_LAYUP|PF_DUNKER|PF_ALLYDUNKER))
			chance = 2;	//5
		else if (player_blocks[game_info.ball_handler].odata.adata[0].animode & MODE_REACTING)
			chance = 5;
		else if(player_blocks[game_info.ball_handler].odata.flags & (PF_FUNKY_DRIBBLE))
			chance = 1;
		else
			chance = get_steal_base(ppdata); //40 //90; //!!!FIX
		//	chance = 35; //40 //90; //!!!FIX

		rnd = randrng(100);

		// no steal can happen if this variable is set...
		if(ignore_steal)
			chance = -1;

		// sometimes, flail the guy who is trying to steal
		his_dribbling = player_blocks[dribbler]._dribbling;
		if ((player_blocks[game_info.ball_handler].odata.flags & PF_FUNKY_DRIBBLE) &&
			(my_stealing <= his_dribbling) &&
			(randrng(my_stealing + his_dribbling) >= his_dribbling) &&
			(in_between_ball_basket(ppdata) == YES) &&
			randrng(100) <= 50 &&
			player_blocks[game_info.ball_handler].action <= 2
			)
		{
// FIX!!  Do variety...
			change_anim(pobj, dfake1_butt_anim);

			player_blocks[game_info.ball_handler].action++;	// inc. players juke cnt
			return;		// NO STEAL
		}

		if (rnd <= chance)
		{
			// swat ball sound
			snd_scall_bank(gameplay_bnk_str,STEAL1_SND,VOLUME7,127,PRIORITY_LVL4);
			flash_ball(4);	// flash the ball

			player_blocks[game_info.ball_handler].no_ball_collide = 25;		//20

			// inc. game stat
			plyr_stat_ptrs[ppdata->plyrnum]->steals++;
			// inc. game stat
			plyr_stat_ptrs[game_info.ball_handler]->turnovers++;

			// does this player take the ball or smack it away?
			//if(player_blocks[game_info.ball_handler].odata.flags & (PF_LAYUP|PF_DUNKER|PF_ALLYDUNKER))
			//	chance = 3;
			//else
			chance = get_keepball_base(ppdata);
			//(int)(95.0f * ((float)ppdata->_stealing / 20.0f));
			//get_steal_base(ppdata);	//30;

			if (randrng(100) <= chance)
				smack_away = NO;
			else
				smack_away = YES;

			if(smack_away == YES)
			{
				player_blocks[game_info.ball_handler].dribble_mode = DM_CAN_DRIBBLE;
				ppdata->no_ball_collide = 12 + randrng(15);
				ppdata->dribble_mode = DM_CAN_DRIBBLE;	

				ball_sparkcreate (1, 5, 5);

				// make dude react to steal
				if (player_blocks[game_info.ball_handler].odata.y == 0.0f && randrng(100) <= 80)
					change_anim( (obj_3d *)(player_blocks + game_info.ball_handler), dribble_steal_react);

				no_pump_fake = YES;

				game_info.passer    = NO_PASSER;
				game_info.ball_mode = BM_FREE_ON_COURT;
				//ball_obj.who_shot   = NO_SHOOTER;

				//OOBPasser = NO_PASSER;					//!!! had to be = just to get here, but ...
				game_info.ball_handler = NO_BALLHANDLER;	// clear so ball can go flying
#if NUM_PERTEAM > 2
				game_info.receiver_a   = NO_RECEIVER;
				game_info.receiver_b   = NO_RECEIVER;
#endif

				// set ball vels
				if (limb == JOINT_LWRIST)
					angle = pobj->fwd - 128 - randrng(128);
				else
					angle = pobj->fwd + 128 + randrng(128);

				vel = ((float)randrng(1100)) / 1000.0f;
				if (vel < 0.65f)
					vel = .65f;

				ball_obj.odata.vx = FRELX(0.0f,vel,angle);
				ball_obj.odata.vz = FRELZ(0.0f,vel,angle);
				ball_obj.odata.vy = ((float)randrng(1500)) / 1000.0f;

				loose_ball_speech(pobj);
				ignore_steal = 110;
			}
			else
			{
#ifdef DEBUG
//				printf("DEFENSE: steal_logic hit %s, bmode = %s\n", ppdata->p_spd->szlast, ball_mode_names[game_info.ball_mode + 2]);
#endif

				// make dude react to steal
				if (player_blocks[game_info.ball_handler].odata.y == 0.0f && randrng(100) <= 80)
					change_anim( (obj_3d *)(player_blocks + game_info.ball_handler), dribble_steal_react);

				game_info.ball_handler = ppdata->plyrnum;
				game_info.ball_mode = BM_DRIBBLING;
				change_possession();

#if NUM_PERTEAM > 2
				game_info.receiver_a   = NO_RECEIVER;
				game_info.receiver_b   = NO_RECEIVER;
#endif
				//auto_has_ball = YES;
				qcreate("stl_spch", SPEECH_PID, steal_ball_speech_proc, ppdata->plyrnum, 0, 0, 0);
				ignore_steal = 110;
				ignore_pass = 25;
			}
		}
		else if (randrng(100) == 30)
			qcreate("swat_spch", SPEECH_PID, swated_ball_speech_proc, ppdata->plyrnum, 0, 0, 0);
	}
}


//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void defensive_swat_logic(bbplyr_data *swater)
{
	int		i,angle,joint,keep_ball = NO,goaltended = NO, rnd;
	float	p[3], hx, hy, hz, vel;
	float	bx, by, bz, *tm, dist, block_pct = (float)swater->_blocking / 20.0f;
	obj_3d	*pobj = (obj_3d *)swater;

	// dont allow player to snag ball off of rim!
//	if((game_info.ball_mode == BM_BOUNCE_ON_RIM) || (game_info.ball_mode == BM_PASSING))
	if (((game_info.ball_mode == BM_BOUNCE_ON_RIM) || (game_info.ball_mode == BM_PASSING)) && (randrng(100) < 96))
		return;	
	// just scored ?
	if((game_info.ball_mode == BM_OFF_INTO_PLAY) && (game_info.game_mode == GM_INBOUNDING))
		return;

	// Is limb close enough to the ball ?
	if(swater->ball_in_hand == RIGHT_HAND)
	{
		bx = (R_INHAND_X_OFF + 0.0f) * PSCALE;
		by = R_INHAND_Y_OFF * PSCALE;
		bz = (R_INHAND_Z_OFF + 0.0f) * PSCALE;
		joint = JOINT_RWRIST;
	}
	else
	{
		bx = (L_INHAND_X_OFF + 0.0f) * PSCALE;
		by = L_INHAND_Y_OFF * PSCALE;
		bz = (L_INHAND_Z_OFF + 0.0f) * PSCALE;
		joint = JOINT_LWRIST;
	}

	tm = cambot.xform;
	hx = swater->jpos[joint][0] * tm[0] + swater->jpos[joint][1] * tm[4] + swater->jpos[joint][2] * tm[8] + cambot.x;
	hy = swater->jpos[joint][0] * tm[1] + swater->jpos[joint][1] * tm[5] + swater->jpos[joint][2] * tm[9] + cambot.y;
	hz = swater->jpos[joint][0] * tm[2] + swater->jpos[joint][1] * tm[6] + swater->jpos[joint][2] * tm[10] + cambot.z;

	dist = fsqrt(((ball_obj.odata.x - hx) * (ball_obj.odata.x - hx)) + ((ball_obj.odata.z - hz) * (ball_obj.odata.z - hz)));

	if(((ignore_steal <= 0)
		&& (dist <= SWAT_COLL_DIST)
		&& (fabs(hy - ball_obj.odata.y) <= 6.0f))
//		&& ((pobj->flags & (PF_DUNKER|PF_LAYUP|PF_ALLYDUNKER) && randrng(100) < (3 + (int)(10 * block_pct)) )
		// Allow a few more blocked dunks
		&& ((pobj->flags & (PF_DUNKER|PF_LAYUP|PF_ALLYDUNKER) && randrng(100) < (5 + (int)(10 * block_pct)) )
			 || randrng(100) < get_swat_base(swater)) )	//40) )
	{
		swater->odata.flags &= ~(PF_PLYRFLAGS);
		flash_ball(4);
		snd_scall_bank(gameplay_bnk_str,STEAL1_SND,VOLUME7,127,PRIORITY_LVL4);

		// inc. game stat
		plyr_stat_ptrs[swater->plyrnum]->blocks++;

		// now check if swated ball while someone has it
		if(game_info.ball_handler != NO_BALLHANDLER)
		{
			// shooter/dunker...still holding ball.. maybe have him lose it!
			if((player_blocks[game_info.ball_handler].odata.flags & PF_SHOOTING) && (randrng(100) < 50))
			{
				change_anim((obj_3d *)(player_blocks + game_info.ball_handler), jumper_react_to_swat);

				// Smacked in middle of jump shot - should he lose ball ?
				// Tough/big guys should keep ball more often
				if(randrng(100) < (int)(80.0f * (player_blocks[game_info.ball_handler]._power / 20.0f)) )
				{	// player loses ball
					player_blocks[game_info.ball_handler].no_ball_collide = 15;
					player_blocks[game_info.ball_handler].dribble_mode = DM_CAN_DRIBBLE;
				}
				else
				{	// player retains possesion but cant dribble
					player_blocks[game_info.ball_handler].dribble_mode = DM_CANT_DRIBBLE;
					qcreate("swat_spch", SPEECH_PID, swated_ball_speech_proc, swater->plyrnum, 1, 0, 0);
					return;
				}
			}
		}
		// check for goal tending!!
//		else if((ball_obj.flight_time > (ball_obj.total_time/2)) && (game_info.ball_mode == BM_SHOOTING))
		// We allow some downward velocity on the ball, to allow some arguments, and to get rid of some GT's
		else if((ball_obj.odata.vy < -0.30f) && (game_info.ball_mode == BM_SHOOTING))
			goaltended = YES;

		if ((swater->plyrnum == game_info.fire_player) && (game_info.fire_cnt >= FIRECNT))
			goaltended = NO;
		if (game_info.team_fire == player_blocks[swater->plyrnum].team)
			goaltended = NO;

		if (what_type_shot == DUNK)
			goaltended = NO;


		// Don't allow drones to goaltend unless their tmate is on fire, or is about to get on fire
		if ((ISDRONE(&player_blocks[swater->plyrnum]))
			&& (goaltended == YES)
			&& !(((swater->plyrnum ^ 1 ) == game_info.fire_player) && (game_info.fire_cnt >= FIRECNT)))
		{
#ifdef DEBUG
			printf("Throwing away a swat goaltend by a drone!!\n");
//			while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
#endif
			return;
		}

		// If a layup, let most collisions go...
		// Could do some percentaging here...
		//FIX!!!!  If on fire, allow blocking layups also!
		if ((what_type_shot == LAYUP) && (randrng(100) <= 75 &&
			(game_info.team_fire != player_blocks[swater->plyrnum].team
				&& ((swater->plyrnum != game_info.fire_player) && (game_info.fire_cnt < FIRECNT)))))
		{
			goaltended = NO;
 			flash_ball(4);
			if(randrng(100) < 20)
				snd_scall_bank(gameplay_bnk_str,STEAL1_SND,VOLUME5,127,PRIORITY_LVL4);
#ifdef DEBUG
			printf("Tossing away a swatted layup!\n");
//			while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
#endif
		}
		else
		{
			// dont allow a SWATED shot to be swated again.
//			if ((game_info.ball_mode != BM_SHOOTING) && (game_info.ball_mode != BM_DRIBBLING))
//				keep_ball = YES;
			if ((ball_obj.flight_time < 10) && (game_info.ball_mode == BM_SHOOTING))
				keep_ball = (randrng(100) < 10 ? YES : NO);		//3
			else if (game_info.ball_mode == BM_SHOOTING)
//				keep_ball = (randrng(100) < get_swat_base(swater) ? YES : NO);
				keep_ball = (randrng(100) < 5 ? YES : NO);
			else
			{
#ifdef DEBUG
			printf("Defaulting to YES keep ball on a swat!\n");
//			while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
#endif
				keep_ball = YES;
			}

			if (goaltended)
				keep_ball = (randrng(100) > 90 ? YES : NO);

			keep_ball = NO;
				
			// swat ball sound
			snd_scall_bank(gameplay_bnk_str,STEAL1_SND,VOLUME7,127,PRIORITY_LVL4);
			flash_ball(4);	// flash the ball

//			if ((keep_ball) || (goaltended && randrng(100) <50))
			

			if (keep_ball)
			{	// Keeping ball

				// inc. game stat
				plyr_stat_ptrs[swater->plyrnum]->steals++;
				// inc. game stat
				if ((!goaltended) && (ball_obj.who_shot != NO_SHOOTER))
					plyr_stat_ptrs[ball_obj.who_shot]->turnovers++;

#ifdef DEBUG
//				printf("DEFENSE: swat_logic hit %s, bmode = %s\n", swater->p_spd->szlast, ball_mode_names[game_info.ball_mode + 2]);
#endif

				no_pump_fake = YES;
				game_info.ball_handler = swater->plyrnum;
				game_info.passer = NO_PASSER;
				//ball_obj.who_shot = NO_SHOOTER;
				swater->dribble_mode = DM_CAN_DRIBBLE;
				game_info.ball_mode = BM_DRIBBLING;
			 	ball_rotclr();
				rnd = randrng(100);
				if (rnd > 80)
					change_anim(pobj, rebound2_grab_anim);
				else if (rnd > 60)
					change_anim(pobj, rebound5_grab_anim);
				else if (rnd > 40)
					change_anim(pobj, rebound6_grab_anim);
				else
					change_anim(pobj, rebound7_grab_anim);

				if(!goaltended)
				{
					qcreate("stl_spch", SPEECH_PID, steal_ball_speech_proc, swater->plyrnum, 0, 0, 0);
					change_possession();
					// no steals for a bit...
					ignore_steal = 80;
				}
				else
					do_goaltending_logic();
			}
			else
			{	// swat ball
			int team = !swater->team;

				if (randrng(100) < 50)
					angle = pobj->fwd - randrng(64);
				else
					angle = pobj->fwd + randrng(64);

				vel = ((float)randrng(1700)) / 1000.0f;
				if(vel < 0.55f)
					vel = .75f;

//				if (randrng(100) < 3)
//					angle = pobj->fwd + 512;

				ball_obj.odata.vx = FRELX(0.0f,vel,angle);
				ball_obj.odata.vz = FRELZ(0.0f,vel,angle);

				ball_obj.odata.vy = ((float)randrng(1200)) / 1000.0f;

				no_pump_fake = YES;
				game_info.ball_handler = NO_BALLHANDLER;
				game_info.passer    = NO_PASSER;
				game_info.ball_mode = BM_FREE_ON_COURT;
				//ball_obj.who_shot   = NO_SHOOTER;
				swater->dribble_mode = DM_CAN_DRIBBLE;	
//				swater->no_ball_collide = 15;

				player_blocks[2 * team + 0].no_ball_collide =
				player_blocks[2 * team + 1].no_ball_collide = 15;

				ball_sparkcreate (2, 10, 4);

				// swat ball sound
				snd_scall_bank(gameplay_bnk_str,STEAL1_SND,VOLUME7,127,PRIORITY_LVL4);
				flash_ball(4);	// flash the ball

				if(goaltended)
					do_goaltending_logic();
				else
					qcreate("swat_spch", SPEECH_PID, swated_ball_speech_proc, swater->plyrnum, 0, 0, 0);
				
				swater->no_ball_collide = 7;

			}
		}
	}
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void defensive_block_logic(bbplyr_data *blocker)
{
	int		i,angle,joint,keep_ball = NO,goaltended = NO, rnd;
	float	p[3], hx, hy, hz, vel, dist, block_pct = (float)blocker->_blocking / 20.0f;
	float	bx, by, bz, *tm;
	obj_3d	*pobj = (obj_3d *)blocker;

	// dont allow player to snag ball off of rim!
	if (((game_info.ball_mode == BM_BOUNCE_ON_RIM) || (game_info.ball_mode == BM_PASSING)) && (randrng(100) < 96))
		return;	
	// just scored ?
	if ((game_info.ball_mode == BM_OFF_INTO_PLAY) && (game_info.game_mode == GM_INBOUNDING))
		return;

	// Is limb close enough to the ball ?
	if(blocker->ball_in_hand == RIGHT_HAND)
	{
		bx = (R_INHAND_X_OFF + 0.0f) * PSCALE;
		by = R_INHAND_Y_OFF * PSCALE;
		bz = (R_INHAND_Z_OFF + 0.0f) * PSCALE;
		joint = JOINT_RWRIST;
	}
	else
	{
		bx = (L_INHAND_X_OFF + 0.0f) * PSCALE;
		by = L_INHAND_Y_OFF * PSCALE;
		bz = (L_INHAND_Z_OFF + 0.0f) * PSCALE;
		joint = JOINT_LWRIST;
	}

	tm = cambot.xform;
	hx = blocker->jpos[joint][0] * tm[0] + blocker->jpos[joint][1] * tm[4] + blocker->jpos[joint][2] * tm[8] + cambot.x;
	hy = blocker->jpos[joint][0] * tm[1] + blocker->jpos[joint][1] * tm[5] + blocker->jpos[joint][2] * tm[9] + cambot.y;
	hz = blocker->jpos[joint][0] * tm[2] + blocker->jpos[joint][1] * tm[6] + blocker->jpos[joint][2] * tm[10] + cambot.z;

	dist = fsqrt(((ball_obj.odata.x - hx) * (ball_obj.odata.x - hx)) + ((ball_obj.odata.z - hz) * (ball_obj.odata.z - hz)));



	if(((ignore_steal <= 0) && (dist <= BLOCK_COLL_DIST) && (fabs(hy - ball_obj.odata.y) <= 6.0f)) &&	//6.0
//		(((pobj->flags & (PF_DUNKER|PF_LAYUP|PF_ALLYDUNKER)) && (randrng(100) < (3 + (int)(10 * block_pct)))) || (randrng(100) < get_swat_base(blocker))))
		// Allow a few more blocked dunks
		(((pobj->flags & (PF_DUNKER|PF_LAYUP|PF_ALLYDUNKER)) && (randrng(100) < (5 + (int)(10 * block_pct)))) || (randrng(100) < get_swat_base(blocker))))
	{
		blocker->odata.flags &= ~(PF_PLYRFLAGS);
		flash_ball(4);
		snd_scall_bank(gameplay_bnk_str,STEAL1_SND,VOLUME7,127,PRIORITY_LVL4);

		// If a layup, let most collisions go...
		// Could do some percentaging here...
		// Perhaps if this is a rebound put back layup, we allow it to be blocked

		// FIX!!!  when on fire, still block these layups!
		if ((what_type_shot == LAYUP) && (randrng(100) <= 75 &&
			(game_info.team_fire != player_blocks[blocker->plyrnum].team
				&& ((blocker->plyrnum != game_info.fire_player) && (game_info.fire_cnt < FIRECNT)))))
		{
#ifdef DEBUG
			printf("Tossing away a blocked layup!\n");
//			while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
#endif
			return;
		}

		// FIX!!!  Maybe, if a player begins a jump shot, he can get flailed just because he is near opponent
		// don't take ball from person, just dribbling
		if((game_info.ball_mode == BM_DRIBBLING) &&
			(game_info.ball_handler != NO_BALLHANDLER) &&
			!(pobj->flags & PF_SHOOTING))
		{
//			qcreate("blk_spch", SPEECH_PID, blocked_shot_speech_proc, blocker->plyrnum, 1, 0, 0);
			return;
		}

		// inc. game stat - will inc blocks on goaltends also!
		plyr_stat_ptrs[blocker->plyrnum]->blocks++;


		// check for goal tending!!
//		if((ball_obj.flight_time > (ball_obj.total_time/2)) && (game_info.ball_mode == BM_SHOOTING))
		// We allow some downward velocity on the ball, to allow some arguments, and to get rid of some GT's
		if((ball_obj.odata.vy < -0.30f) && (game_info.ball_mode == BM_SHOOTING))
			goaltended = YES;

		if((ball_obj.flight_time < 10) && (game_info.ball_mode == BM_SHOOTING))
			keep_ball = (randrng(100) < 3 ? YES : NO);
		else if (game_info.ball_mode == BM_SHOOTING)
			keep_ball = (randrng(100) < get_swat_base(blocker) ? YES : NO);
		else
			keep_ball = YES;

		if ((blocker->plyrnum == game_info.fire_player) && (game_info.fire_cnt >= FIRECNT))
		{
			goaltended = NO;
			keep_ball = YES;
		}
		if (game_info.team_fire == player_blocks[blocker->plyrnum].team)
		{
			goaltended = NO;
			keep_ball = YES;
		}

		if (goaltended)
			keep_ball = (randrng(100) > 80 ? YES : NO);

		// Don't allow drones to goaltend unless their tmate is on fire, or is about to get on fire
		if ((ISDRONE(&player_blocks[blocker->plyrnum]))
			&& (goaltended == YES)
			&& !(((blocker->plyrnum ^ 1) == game_info.fire_player) && (game_info.fire_cnt >= FIRECNT)))
		{
#ifdef DEBUG
			printf("Throwing away a goaltend by a drone!!\n");
//			while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
#endif
			return;
		}

		// swat ball sound
		snd_scall_bank(gameplay_bnk_str,STEAL1_SND,VOLUME7,127,PRIORITY_LVL4);
		flash_ball(4);	// flash the ball


		if(keep_ball || goaltended)
		{	// keeping ball

#ifdef DEBUG
//				printf("DEFENSE: block_logic hit %s, bmode = %s\n", blocker->p_spd->szlast, ball_mode_names[game_info.ball_mode + 2]);
#endif
			// inc. game stat
			plyr_stat_ptrs[blocker->plyrnum]->steals++;
			// inc. game stat
			if ((!goaltended) && (ball_obj.who_shot != NO_SHOOTER))
				plyr_stat_ptrs[ball_obj.who_shot]->turnovers++;

			no_pump_fake = YES;
			game_info.ball_handler = blocker->plyrnum;
			game_info.passer = NO_PASSER;
			blocker->dribble_mode = DM_CAN_DRIBBLE;	
			game_info.ball_mode = BM_DRIBBLING;
			//ball_obj.who_shot = NO_SHOOTER;
		 	ball_rotclr();
			rnd = randrng(100);
			if (rnd > 75)
				change_anim(pobj, rebound2_grab_anim);
			else if (rnd > 50)
				change_anim(pobj, rebound5_grab_anim);
			else if (rnd > 25)
				change_anim(pobj, rebound6_grab_anim);
			else
				change_anim(pobj, rebound7_grab_anim);

			if(!goaltended)
			{
				qcreate("stl_spch", SPEECH_PID, steal_ball_speech_proc, blocker->plyrnum, 0, 0, 0);
				change_possession();
				// no steals for a bit...
				ignore_steal = 90;
			}
			else
				do_goaltending_logic();
		}
		else
		{	// deflect ball
			if (randrng(100) < 50)
				angle = pobj->fwd - randrng(32);
			else
				angle = pobj->fwd + randrng(32);

			vel = ((float)randrng(1000)) / 1000.0f;
			if (vel < 0.50f)
				vel = .50f;

			if (randrng(100) < 3)
				angle = pobj->fwd + 512;

			ball_obj.odata.vx = FRELX(0.0f,vel,angle);
			ball_obj.odata.vz = FRELZ(0.0f,vel,angle);

			ball_obj.odata.vy = ((float)randrng(600)) / 1000.0f;

			no_pump_fake = YES;
			game_info.ball_handler = NO_BALLHANDLER;
			game_info.passer    = NO_PASSER;
			game_info.ball_mode = BM_FREE_ON_COURT;
			//ball_obj.who_shot   = NO_SHOOTER;
			blocker->dribble_mode = DM_CAN_DRIBBLE;	
			blocker->no_ball_collide = 15;

			ball_sparkcreate (2, 10, 4);

			if(goaltended)
			{
				do_goaltending_logic();
			}
			else
				qcreate("blk_spch", SPEECH_PID, blocked_shot_speech_proc, blocker->plyrnum, 0, 0, 0);
		}
	}
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void rebound_logic(bbplyr_data *rebounder)
{
	int		i,angle,joint;
	float	p[3], hx, hy, hz, dist, time;
	float	xob, yob, zob, bx, by, bz, *tm, tx = 0.0f, ty = 0.0f, tz = 0.0f;
	obj_3d	*pobj = (obj_3d *)rebounder;

	// dont allow a REBOUND...if ball is on the rim 90% of the time...
	if(game_info.ball_mode == BM_BOUNCE_ON_RIM)
	{
		if(randrng(100) <= 22)
			;
		else
			return;
	}
	else if(game_info.ball_mode != BM_OFF_INTO_PLAY)//	 && game_info.ball_mode != BM_FREE_ON_COURT )
		{
#ifdef DEBUG
//			if(game_info.ball_handler == NO_BALLHANDLER)
//			{
//				printf("Skipping rebound code - %s\n", rebounder->p_spd->szlast);
//				printf("bmode = %s\n", ball_mode_names[game_info.ball_mode + 2]);
//				while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
//			}
#endif
				if(game_info.ball_mode != BM_FREE_ON_COURT )
					return;
		}

	// Is limb close enough to the ball ?
//	if(rebounder->ball_in_hand == RIGHT_HAND)
//	{
		bx = (R_INHAND_X_OFF + 0.0f) * PSCALE;
		by = R_INHAND_Y_OFF * PSCALE;
		bz = (R_INHAND_Z_OFF + 0.0f) * PSCALE;
		joint = JOINT_RWRIST;
//	}
//	else
//	{
//		bx = (L_INHAND_X_OFF + 0.0f) * PSCALE;
//		by = L_INHAND_Y_OFF * PSCALE;
//		bz = (L_INHAND_Z_OFF + 0.0f) * PSCALE;
//		joint = JOINT_LWRIST;
//	}

	tm = cambot.xform;
	hx = rebounder->jpos[joint][0] * tm[0] + rebounder->jpos[joint][1] * tm[4] + rebounder->jpos[joint][2] * tm[8] + cambot.x;
	hy = rebounder->jpos[joint][0] * tm[1] + rebounder->jpos[joint][1] * tm[5] + rebounder->jpos[joint][2] * tm[9] + cambot.y;
	hz = rebounder->jpos[joint][0] * tm[2] + rebounder->jpos[joint][1] * tm[6] + rebounder->jpos[joint][2] * tm[10] + cambot.z;

	dist = fsqrt(((ball_obj.odata.x - hx) * (ball_obj.odata.x - hx)) + ((ball_obj.odata.z - hz) * (ball_obj.odata.z - hz)));

	if((dist <= REBOUND_COLL_DIST) && (fabs(hy - ball_obj.odata.y) <= 6.0f) && (game_info.ball_handler == NO_BALLHANDLER))	//5.0
	{
//		if(randrng(100) < 5)
//		{
//			// dont grab rebound... instead...knock it upward a little bit
//			rebounder->no_ball_collide = 10;
//			rebounder->odata.flags &= ~(PF_PLYRFLAGS);
//		}
//		else
		{
			if(rebounder->no_ball_collide > 0)
			{
#ifdef DEBUG
//				printf("ignoring collision! no_collide = %d\n",rebounder->no_ball_collide);
#endif
				return;
			}
				
#ifdef DEBUG
//			if(rebounder->odata.plyrmode == MODE_DEFENSE)
//				printf("DEFENSE: rebound_logic hit %s, bmode = %s\n", rebounder->p_spd->szlast, ball_mode_names[game_info.ball_mode + 2]);
//			else
//				printf("OFFENSE: rebound_logic hit %s, bmode = %s\n", rebounder->p_spd->szlast, ball_mode_names[game_info.ball_mode + 2]);
#endif
			// Shot rebounded could have been a 3 ptr - make sure any rebound put back is only 2 pts
			what_type_shot == JUMPER_2;

			// dont shoot ball as soon as catch it
			rebounder->but_new &= ~(P_A);
			//rebounder->but_cur &= ~(P_A);

			// Reset some essentials
			auto_rebounding = NO;
			game_info.passer    = NO_PASSER;
			game_info.ball_mode = BM_DRIBBLING;

			//ball_obj.who_shot = NO_SHOOTER;
			ball_obj.flags   &= ~BF_NOINTERCEPT;

			rebounder->dribble_mode = DM_CAN_DRIBBLE;
			no_pump_fake = YES;
		
			ball_obj.odata.vx = ball_obj.odata.vz = 0.0f;
			ball_obj.odata.y = BALL_RADIUS;
			//rebounder->no_ball_collide = 7;
			game_info.ball_handler = rebounder->plyrnum;

			if(!(rebounder->odata.flags & PF_QUICK_REBOUND))
			{
				// change into a rebound GRAB animation
				if ((pobj->adata[0].seq_index == (SEQ_RB1_MIS)) ||
					(pobj->adata[0].seq_index == (SEQ_RB3_MIS)))
					change_anim(pobj, rebound5_grab_anim);
				else if (pobj->adata[0].seq_index == (SEQ_RB2_MIS))
					change_anim(pobj, rebound2_grab_anim);
				else if (pobj->adata[0].seq_index == (SEQ_RB7_MIS))
					change_anim(pobj, rebound7_grab_anim);
				else
					change_anim(pobj, rebound6_grab_anim);
			}

			rebounder->odata.flags &= ~(PF_PLYRFLAGS);

			qcreate("rb_spch", SPEECH_PID, rebound_speech_proc, 0, 0, 0, 0);

			// inc. game stat
			plyr_stat_ptrs[rebounder->plyrnum]->rebounds++;

			// Grab the ball
			if(game_info.off_side != rebounder->team)
				change_possession();

			what_type_shot = three_pt_check(pobj) ? JUMPER_3 : JUMPER_2;

			flash_ball(4);
			snd_scall_bank(gameplay_bnk_str,STEAL1_SND,VOLUME7,127,PRIORITY_LVL3);
	 		ball_rotclr();
		}
	}
}


//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void do_goaltending_logic(void)
{
	
	if(!(existp(VIOLATION_PID, -1)))
		qcreate("goaltending", VIOLATION_PID, goaltending_proc, 0, 0, 0, 0);
	
	goaltending_speech();

	//snd_scall_bank(crowd_bnk_str, CROWD2_SWELL1_SND, VOLUME3, 127, PRIORITY_LVL5);

	if(what_type_shot == JUMPER_3)
	{
		game_info.scores[game_info.off_side][game_info.game_quarter] += 3;
		plyr_stat_ptrs[ball_obj.who_shot]->three_ptrs_made++;
		update_status_box = 1;	// update score
		// Show 3 pt message under players name who scored at top of screen
//		qcreate("3pts", 0, show_3pt_proc, 0, 0, 0, 0);
	}
	else
	{
		game_info.scores[game_info.off_side][game_info.game_quarter] += 2;
		plyr_stat_ptrs[ball_obj.who_shot]->two_ptrs_made++;
		update_status_box = 1;	// update score
	}

	//ball_obj.who_shot = -1;	// player who shot ball can get it now.
	//game_info.shooter = -1;
	clock_active = 0;		// Stop game clock
	reset_shotclock = 1;	// Reset shotclock

	SetUpOOBounds(0,1,0);
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
#define DEFENSE_ANIM_RANGE 384
void defense_run( obj_3d *pobj )
{
	static int def_seqs[] =
	{
		SEQ_DSLIDE_R, SEQ_DSLID2_R,
		SEQ_DSLIDE_L, SEQ_DSLID2_L,
		SEQ_DSLIDE_B, SEQ_DSLIDE_F,
		SEQ_RUN_R, SEQ_RUN_L,
		SEQ_S_READY
	};
	static char *int_def_scripts[] =
	{
		dslide_anim_int_r, dslide_anim2_int_r,
		dslide_anim_int_l, dslide_anim2_int_l,
		dslide_anim_back, dslide_anim_forward,
		run_anim_int, run_anim_int,
		s_ready_anim_int
	};

	bbplyr_data * ppdata  = (bbplyr_data *)pobj;
	bbplyr_data * ballman = (game_info.ball_handler == NO_BALLHANDLER) ? NULL : &player_blocks[game_info.ball_handler];
	float dist_to_ball, p[3], phc[3], rim_pt[3], speedup = 0.0f;
	float dist_to_rim;
	int seq_index = 6 + ppdata->ball_in_hand, new_dir, angle_to_ball, bearing, look_dir, lo_angle, defense_angle, ball_to_rim, coming_back_on_screen;//, angle_to_rim
	int team_winning;
	float	f;

	if( ppdata->stick_cur != 24 )
	{
		if(pobj->flags & PF_CELEBRATE)
		{
			pobj->flags &= ~PF_CELEBRATE;
			//printf("CELEBRATE cleared.\n");
		}
		// stick is down.  set vels & facing and make sure we're in the right anim
		look_dir = new_dir = 1023 & (dir49_table[ppdata->stick_cur] + (int)RD2GR(cambot.theta));

		if(new_dir != pobj->fwd)
		{
			if(randrng(100) == 30)
				shoe_squeak(pobj);
		}

	// keep the player inside the invisible wall...
		oob_correction(pobj, &new_dir);

		if( ppdata->stick_cur != 24 )
		{
			pobj->tgt_fwd = new_dir;
			pobj->tgt_fwd2 = new_dir;

// TODO: put some defensive rules here....
			if (ballman)
			{
// if defensive player is within a certain range of the line between rim and ball then use defensive animations.
				rim_pt[0] = game_info.active_goal ? RIM_X : -RIM_X; rim_pt[1] = rim_pt[2] = 0.0f;
				ball_to_rim = ptangle( &(ballman->odata.x), rim_pt);
				lo_angle = (ball_to_rim - 512 - (DEFENSE_ANIM_RANGE/2)) & 1023;	//hi_angle = (1024 + angle_to_rim - 512 + 192) % 1024;

				p[0] = ball_obj.odata.x; p[1] = 0.0f; p[2] = ball_obj.odata.z;
				angle_to_ball = ptangle( &(pobj->x), &(ballman->odata.x));
				defense_angle = (angle_to_ball + (1024 - lo_angle)) & 1023;

// is the offender going toward the basket at all?
				bearing = (ballman->odata.fwd - ptangle( &(ballman->odata.x), rim_pt)) & 1023;
				if (bearing > 512) bearing = 1024 - bearing;
				gbear = bearing;

				if((bearing <= 384)
					&& (defense_angle >= 0 && defense_angle <= DEFENSE_ANIM_RANGE
					&& game_info.ball_handler == ppdata->offender
					&& plyr_crossed_halfcourt(game_info.ball_handler) == YES)
					)
				{
					dist_to_ball = distxz[ppdata->plyrnum][game_info.ball_handler];		//fsqrt( ((player_blocks[game_info.ball_handler].odata.x - pobj->x) * (player_blocks[game_info.ball_handler].odata.x - pobj->x)) + ((player_blocks[game_info.ball_handler].odata.z - pobj->z) * (player_blocks[game_info.ball_handler].odata.z - pobj->z)) );
		// what dir is the defender moving in relation the ball_handler?
					bearing = (new_dir - ptangle( &(pobj->x), &(ballman->odata.x))) & 1023;
					if(dist_to_ball < 60.0f)
					{
						//if( bearing > 896 || bearing <= 128 )	// moving in ball direction
						if( bearing > 960 || bearing <= 64 )	// moving in ball direction
						{
							seq_index = 5;	// defensive forward
							look_dir = angle_to_ball;
							//printf("forward\n");
							gdir = 0;
						}
						//if( bearing > 384 && bearing <= 640 )	// (45 deg)384, 512
						if( bearing > 448 && bearing <= 576 )	// (45 deg)384, 512
						{
							seq_index = 4;	// defensive backward
							look_dir = angle_to_ball;
							//printf("backward\n");
							gdir = 1;
						}
						//if( bearing > 640 && bearing <= 896 )	// moving in ball direction
						if( bearing > 576 && bearing <= 960 )	// moving in ball direction
						{
							if(pobj->adata[0].seq_index < (SEQ_DSLIDE_R) || pobj->adata[0].seq_index > (SEQ_DSLID2_L)) // this order is set up in the anim makefile
								seq_index = 0 + randrng(2);	// defensive right
							else if(pobj->adata[0].seq_index == (SEQ_DSLIDE_R))
								seq_index = 0;
							else
								seq_index = 1;
							look_dir = angle_to_ball;
							//printf("right\n");
							gdir = 3;
						}
						//if( bearing > 128 && bearing <= 384 )	// (45 deg)384, 512
						if( bearing > 64 && bearing <= 448 )	// (45 deg)384, 512
						{
							if(pobj->adata[0].seq_index < (SEQ_DSLIDE_R) || pobj->adata[0].seq_index > (SEQ_DSLID2_L)) // this order is set up in the anim makefile
								seq_index = 1 + randrng(2);	// defensive left
							else if(pobj->adata[0].seq_index == (SEQ_DSLIDE_L))
								seq_index = 2;
							else
								seq_index = 3;
							look_dir = angle_to_ball;
							//printf("left\n");
							gdir = 2;
						}
					}
				}
			}// END.IF there was a ball handler

//			if( pobj->flags & PF_TURBO )
//				seq_index += 1;

				//pobj->vx = FRELX( 0.0f, (ppdata->_speed * (seq_index == 6 ? 1.0f : 1.0f)), pobj->tgt_fwd );
				//pobj->vz = FRELZ( 0.0f, (ppdata->_speed * (seq_index == 6 ? 1.0f : f)), pobj->tgt_fwd );

// FIX!!!!  Should not be a multiplier
			pobj->vx = FRELX( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? TURBO_BOOST : 1.0f)), pobj->tgt_fwd );
			pobj->vz = FRELZ( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? TURBO_BOOST : 1.0f)), pobj->tgt_fwd );

//#ifndef DEBUG_NO_SPEED_LOGIC
			// any defender offscreen runs way faster...
			if (!(pobj->flags & PF_ONSCREEN) && !(pobj->adata[0].animode & MODE_REACTING))
			{
				speedup += base_speed * .2f;

				// project position to halfcourt line (if running adjasent to it... speedup)
				phc[0] = 0.0f;		// x
				phc[1] = 0.0f;		// y
				phc[2] = pobj->z;	// z
				bearing = (pobj->fwd - ptangle( &(pobj->x), phc)) & 1023;

				if (bearing > 512)
					bearing = 1024 - bearing;

				if (bearing < 64)
				{
					speedup += base_speed * 1.80f;	// running toward half-court-line, but still off screen
					//printf("speedup = %2.2f\n",speedup);
				}
				else if (bearing > 448)
				{
					speedup += base_speed * .85f;	// running directly away from it...but still off screen
					//printf("speedup = %2.2f\n",speedup);
				}
			}

			// defender gets another speed boost if coming towards screen while offscreen...
//				if(
//				(cambot.x > pobj->x && pobj->tgt_fwd < 512 && pobj->tgt_fwd > 0)
//				|| (cambot.x < pobj->x && pobj->tgt_fwd > 512 && pobj->tgt_fwd < 1020))
//					coming_back_on_screen = YES;
//				else
//					coming_back_on_screen = NO;
//				if(!(pobj->flags & PF_ONSCREEN) && coming_back_on_screen == YES)
//				{
//					speedup += base_speed * .4f;
//				}

			// add 12% boost for player fire!
			if ((game_info.fire_player == ppdata->plyrnum && game_info.fire_cnt >= FIRECNT) || (game_info.team_fire == ppdata->team))
			{
				speedup += base_speed * .12f;	//.10
			}

			team_winning = NO;
			if(compute_score(ppdata->team) > compute_score(!ppdata->team))
				team_winning = YES;

			// if game_time is under 50 secs speed up losing players...
			if( ((game_info.game_time & 0x0000FF00)>>8) <= 50
				&& team_winning == NO)
				{
					speedup += base_speed * .10f;
//#ifdef DEBUG
//					printf("speeding up losing players on defense\n");
//#endif
 				}


 			//printf("plyr %d, defensive speedup %f\n", ppdata->plyrnum, speedup);
			pobj->vx += FRELX( 0.0f, speedup, pobj->tgt_fwd );
			pobj->vz += FRELZ( 0.0f, speedup, pobj->tgt_fwd );
//#endif
			f = ppdata->drone_vel;
			if (f)
			{
				pobj->vx = FRELX( 0.0f, f, pobj->tgt_fwd );
				pobj->vz = FRELZ( 0.0f, f, pobj->tgt_fwd );
			}

			if(look_dir != new_dir && seq_index < 6
			&& pobj->adata[0].seq_index != (SEQ_PUSH_6_L)
			&& pobj->adata[0].seq_index != (SEQ_PUSH_4))
			{
				pobj->tgt_fwd = look_dir;
				pobj->tgt_fwd2 = look_dir;
			}

// if current seq != defensive sequence...change it
			if( (pobj->adata[0].seq_index != def_seqs[seq_index] && !((pobj->adata[0].animode|pobj->adata[1].animode) & MODE_UNINT))
				|| ( ((pobj->adata[0].animode & MODE_TWOPART)) && (!(pobj->adata[1].animode & MODE_TWOPART)) )
			)
			{
				if(ppdata->defense_anim_wait == 0)
				{
					if((pobj->adata[1].animode & MODE_TWOPART) && (pobj->adata[1].seq_index >= (SEQ_F_ST2_R) && pobj->adata[1].seq_index <= (SEQ_F_ST2_L)))
						;	// if player is doing a run steal don't change the animation only dir
					else
						change_anim( pobj, int_def_scripts[seq_index] );
					ppdata->defense_anim_wait = 10;	// run anim for n ticks
				}
			}
		}
	} // ENDIF stick_cur != 24

	// this isn't an else to the above if because the test value can change in
	// the above block and we have to run both cases.
	if( ppdata->stick_cur == 24 && !(pobj->flags & PF_CELEBRATE))
	{
		//pobj->vx = pobj->vz = 0.0f;
		if(0)	//ISDRONE(ppdata) == YES)
		{
			if((pobj->vx != 0.0f || pobj->vz != 0.0f))
				pobj->friction = 0.09f;
		}
		else
		{
			pobj->vx = pobj->vz = 0.0f;
		}

		p[0] = ball_obj.odata.x; p[1] = 0.0f; p[2] = ball_obj.odata.z;

		if (ballman && game_info.ball_handler == ppdata->offender)
		{
			dist_to_ball = distxz[ppdata->plyrnum][game_info.ball_handler];
			angle_to_ball = ptangle( &(pobj->x), &(ballman->odata.x));
		}
		else
		{
			dist_to_ball = distxz[ppdata->plyrnum][ppdata->offender];
			angle_to_ball = ptangle( &(pobj->x), &(player_blocks[ppdata->offender].odata.x));
		}
		bearing = (pobj->fwd - angle_to_ball) & 1023;

		seq_index = -1;
		look_dir = pobj->tgt_fwd;

		if (ballman/* && game_info.ball_handler == ppdata->offender*/)
		{
			if (
				(ballman->odata.x <  0.0f && angle_to_ball >= 64  && angle_to_ball <= 448) ||
				(ballman->odata.x >= 0.0f && angle_to_ball >= 576 && angle_to_ball <= 960)
			   )
			{
				if(dist_to_ball < 60.0f)
				{
					if( bearing > 896 || bearing <= 128 )	// moving in ball direction
					{
						seq_index = 8;	// defensive forward
						look_dir = angle_to_ball;
					}
					if( bearing > 384 && bearing <= 640 )	// (45 deg)384, 512
					{
						seq_index = 8;	// defensive backward
						look_dir = angle_to_ball;
					}
					if( bearing > 640 && bearing <= 896 )	// moving in ball direction
					{
						seq_index = 8;	// defensive right
						look_dir = angle_to_ball;
					}
					if( bearing > 128 && bearing <= 384 )	// (45 deg)384, 512
					{
						seq_index = 8;	// defensive left
						look_dir = angle_to_ball;
					}
				}
			}
		}

// defender is still so face offender if he already isn't
		if (ballman///* && game_info.ball_handler == ppdata->offender*/)
		&& pobj->adata[0].seq_index != (SEQ_PUSH_6_L)
		&& pobj->adata[0].seq_index != (SEQ_PUSH_4))
		{
			if (
				//look_dir != pobj->tgt_fwd &&
				(seq_index != 6 && seq_index != 7)	// a running sequence
				&& ppdata->stick_time >= 120			//240
				&& (ballman->odata.vx == 0.0f && ballman->odata.vz == 0.0f)
			   )
			{

//Put back in 3/14/99 - look at ball ownner
				pobj->tgt_fwd = p2p_angles[ppdata->plyrnum][game_info.ball_handler];
			}
			else
			{
				if(ppdata->stick_time >= 120)			//240
					pobj->tgt_fwd = look_dir;
			}
		}
		else
		{
			if(look_dir != pobj->tgt_fwd
			&& ppdata->stick_time >= 120			//240
			&& (ball_obj.odata.vx == 0.0f && ball_obj.odata.vz == 0.0f)
			&& pobj->adata[0].seq_index != (SEQ_PUSH_6_L)
			&& pobj->adata[0].seq_index != (SEQ_PUSH_4)
			)
				pobj->tgt_fwd = look_dir;
		}

		if(pobj->adata[0].seq_index != (SEQ_S_READY)
			&& pobj->adata[0].seq_index != (SEQ_T_RDY1_L)
			&& pobj->adata[0].seq_index != (SEQ_T_RDY1_R)
			&& pobj->adata[0].seq_index != (SEQ_WAIT_1)
			&& pobj->adata[0].seq_index != (SEQ_WAIT_3)
			&& pobj->adata[0].seq_index != (SEQ_SBREATH1)
			&& pobj->adata[0].seq_index != (SEQ_SBREATH3)
			&& (seq_index == 8 || seq_index == -1) )
		{
			if(pobj->vx == 0.0f && pobj->vz == 0.0f)
			{
				if(distxz[ppdata->plyrnum][ppdata->offender] >= 30.0f)
				{
					seq_index = 8;
					//printf("PLEASE STOP ME\n");
					//change_anim( pobj, int_def_scripts[seq_index] );
				}
				else
				{
					seq_index = seq_index == -1 ? 8 : seq_index;
					//printf("PLEASE STOP ME\n");
					//change_anim( pobj, int_def_scripts[seq_index] );
				}
				ppdata->defense_anim_wait = 0;
			}
		}

		// stick is neutral.
		if(( ppdata->stick_old-24 ))	// || !(pobj->adata[0].animode & MODE_UNINT ))
		{

			rim_pt[0] = game_info.active_goal ? RIM_X : -RIM_X; rim_pt[1] = rim_pt[2] = 0.0f;
			dist_to_rim = fsqrt( ((rim_pt[0] - pobj->x) * (rim_pt[0] - pobj->x)) + ((0.0f - pobj->z) * (0.0f - pobj->z)) );
			if ((game_info.ball_handler == NO_BALLHANDLER) &&
				(dist_to_rim < 25.0f) &&
				(game_info.ball_mode == BM_SHOOTING) &&
				(game_info.game_mode == GM_IN_PLAY) &&
				(ISDRONE(&player_blocks[ppdata->plyrnum])))
			{
				change_anim( pobj, randrng(100) < 50 ? wait_rebound_1_anim : wait_rebound_2_anim );
			}
			else
			if( dist_to_ball >= 25.0f && seq_index == -1
				&& (pobj->adata[0].seq_index != (SEQ_SBREATH1))
				&& (pobj->adata[0].seq_index != (SEQ_T_RDY1_L))
				&& (pobj->adata[0].seq_index != (SEQ_T_RDY1_R))
				&& (pobj->adata[0].seq_index != (SEQ_S_READY)) )
			{
				seq_index = 8;
				change_anim( pobj, int_def_scripts[seq_index] );		// got into BREATH stance
				ppdata->defense_anim_wait = 0;
			}
			else if( dist_to_ball <= 25.0f )
			{
				seq_index = seq_index < 0 ? 8 : seq_index;
				change_anim( pobj, int_def_scripts[seq_index] );
				if(seq_index == 8)
					ppdata->defense_anim_wait = 0;
			}
			else
			{
				seq_index = seq_index < 0 ? 8 : seq_index;
				change_anim( pobj, int_def_scripts[seq_index] );
			}
		}
	} // ENDIF stick_cur == 24
}

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
int jumpshot_timeout(obj_3d *pobj)
{
	bbplyr_data * ppdata = (bbplyr_data *)pobj;
	int time_left = ppdata->total_time - ppdata->flight_time, time_out = 10;
	int min = (game_info.game_time & 0x00FF0000)>> 16, sec = (game_info.game_time & 0x0000FF00) >> 8;

	if(min == 0 && sec < 3	//4
	&& (pobj->adata[0].seq_index == (SEQ_J_HVE1_R) || pobj->adata[0].seq_index == (SEQ_J_HVE1_L)) )
		return YES;

	// if less than 4 seconds left... dont allow them to hold button
//	if (((game_info.game_time & 0x0000FF00)>>8) <= 4)
//		return NO;

	if(
		 (pobj->adata[0].seq_index == (SEQ_J_RUN_R)  || pobj->adata[0].seq_index == (SEQ_J_RUN_L))
	|| (pobj->adata[0].seq_index == (SEQ_J_RUN2_R) || pobj->adata[0].seq_index == (SEQ_J_RUN2_L))
	|| (pobj->adata[0].seq_index == (SEQ_J_RUN3_R) || pobj->adata[0].seq_index == (SEQ_J_RUN3_L))
	)
	{
		time_out = 5;
	}
	else if(
			 (pobj->adata[0].seq_index == (SEQ_J_SHT1_R) || pobj->adata[0].seq_index == (SEQ_J_SHT1_L))
		|| (pobj->adata[0].seq_index == (SEQ_J_SHT2_R) || pobj->adata[0].seq_index == (SEQ_J_SHT2_L))
		|| (pobj->adata[0].seq_index == (SEQ_J_SHT3_R) || pobj->adata[0].seq_index == (SEQ_J_SHT3_L))
		|| (pobj->adata[0].seq_index == (SEQ_J_SHT4_R) || pobj->adata[0].seq_index == (SEQ_J_SHT4_L))
		|| (pobj->adata[0].seq_index == (SEQ_J_SHT5_R) || pobj->adata[0].seq_index == (SEQ_J_SHT5_L))
		|| (pobj->adata[0].seq_index == (SEQ_J_SHT6_R) || pobj->adata[0].seq_index == (SEQ_J_SHT6_L))
		|| (pobj->adata[0].seq_index == (SEQ_J_SHT8_R) || pobj->adata[0].seq_index == (SEQ_J_SHT8_L))
		)
	{
		time_out = 15;
	}
	else
		time_out = 5;	

	if((ppdata->but_cur & P_A))
	{
		if(time_left > time_out)
		{
			//printf("swat_p_but_hit_timeout: NO\n");
			return NO;
		}
		else
		{
			//printf("swat_p_but_hit_timeout: YES\n");
			return YES;	
		}
	}
	else
	{
		//printf("swat_p_but_hit_timeout: YES\n");
		return YES;
	}
}

//-----------------------------------------------------------------------------------------------------------------------------
// wait until the button is hit or a timeout has occured
//-----------------------------------------------------------------------------------------------------------------------------
int swat_plyr_button_hit_timeout(obj_3d *pobj)
{
	bbplyr_data * ppdata = (bbplyr_data *)pobj;
	int time_left = ppdata->total_time - ppdata->flight_time;
	int team = !ppdata->team, angle;
	int goaltended = NO, limb;
	float vel, *tm, dist,hx,hy,hz;


	if(ppdata->plyrnum == game_info.ball_handler)
	{
#ifdef DEBUG
//	 	printf("I own ball, let my swat hold frame go!  Slap it!\n");
//	 	while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
#endif



		tm = cambot.xform;
		limb = ppdata->ball_in_hand == RIGHT_HAND ? JOINT_RWRIST : JOINT_LWRIST;
		hx = ppdata->jpos[limb][0] * tm[0] + ppdata->jpos[limb][1] * tm[4] + ppdata->jpos[limb][2] * tm[8] + cambot.x;
		hy = ppdata->jpos[limb][0] * tm[1] + ppdata->jpos[limb][1] * tm[5] + ppdata->jpos[limb][2] * tm[9] + cambot.y;
		hz = ppdata->jpos[limb][0] * tm[2] + ppdata->jpos[limb][1] * tm[6] + ppdata->jpos[limb][2] * tm[10] + cambot.z;
		dist = fsqrt(((ball_obj.odata.x - hx) * (ball_obj.odata.x - hx)) +
					 ((ball_obj.odata.y - hy) * (ball_obj.odata.y - hy)) +
					 ((ball_obj.odata.z - hz) * (ball_obj.odata.z - hz)));


		// don't want the ball attaching to a defender if ball isn't near a wrist
		if(dist <= 8.0f)
		{
		 	// Put ball in swatters hand before it goes
			ball_obj.odata.x = ppdata->jpos[limb][0] * tm[0] + ppdata->jpos[limb][1] * tm[4] + ppdata->jpos[limb][2] * tm[8] + cambot.x;
			ball_obj.odata.y = ppdata->jpos[limb][0] * tm[1] + ppdata->jpos[limb][1] * tm[5] + ppdata->jpos[limb][2] * tm[9] + cambot.y;
			ball_obj.odata.z = ppdata->jpos[limb][0] * tm[2] + ppdata->jpos[limb][1] * tm[6] + ppdata->jpos[limb][2] * tm[10] + cambot.z;
#ifdef DEBUG
			printf("Attaching to wrist! - on swat hit\n");
#endif
		}
#ifdef DEBUG
		else
			printf("Didn't attach to wrist - too far on swat hit: %f\n",dist);
#endif

  		// We allow some downward velocity on the ball, to allow some arguments, and to get rid of some GT's
  		if((ball_obj.odata.vy < -0.30f) && (game_info.ball_mode == BM_SHOOTING))
  		 	goaltended = YES;


  		if((ppdata->plyrnum == game_info.fire_player) && (game_info.fire_cnt >= FIRECNT))
  			goaltended = NO;

  		if(game_info.team_fire == player_blocks[ppdata->plyrnum].team)
  			goaltended = NO;

  		// Don't allow drones to goaltend unless their tmate is on fire, or is about to get on fire
  		if ((ISDRONE(&player_blocks[ppdata->plyrnum]))
  			&& (goaltended == YES)
  			&& !(((ppdata->plyrnum ^ 1 ) == game_info.fire_player) && (game_info.fire_cnt >= FIRECNT)))
  		{
#ifdef DEBUG
  			printf("Throwing away a cylinder goaltend by a drone!!\n");
//			while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
#endif
			goaltended = NO;
		}


		// Send ball on it's way!
		if (randrng(100) < 50)
			angle = pobj->fwd - randrng(64);
		else
			angle = pobj->fwd + randrng(64);

		vel = ((float)randrng(1700)) / 1000.0f;
		if(vel < 0.55f)
			vel = .75f;

				if (randrng(100) < 3)
					angle = pobj->fwd + 512;

		ball_obj.odata.vx = FRELX(0.0f,vel,angle);
		ball_obj.odata.vz = FRELZ(0.0f,vel,angle);

		if(dist > 8.0f)
		{
			// Ball didn't really hit wrist, so just bnc off body
			ball_obj.odata.vx *= 0.5f;
			ball_obj.odata.vz *= 0.5f;
		}



		ball_obj.odata.vy = ((float)randrng(1200)) / 1000.0f;

		no_pump_fake = YES;
		game_info.ball_handler = NO_BALLHANDLER;
		game_info.passer    = NO_PASSER;
		game_info.ball_mode = BM_FREE_ON_COURT;
		//ball_obj.who_shot   = NO_SHOOTER;
		ppdata->dribble_mode = DM_CAN_DRIBBLE;	
		ppdata->no_ball_collide = 15;

		player_blocks[2 * team + 0].no_ball_collide =
		player_blocks[2 * team + 1].no_ball_collide = 15;

		ball_sparkcreate (2, 10, 4);

		// swat ball sound
		snd_scall_bank(gameplay_bnk_str,STEAL1_SND,VOLUME7,127,PRIORITY_LVL5);
		snd_scall_bank(gameplay_bnk_str,STEAL1_SND,VOLUME7,127,PRIORITY_LVL5);
		flash_ball(4);	// flash the ball

		if(goaltended)
			do_goaltending_logic();
		else
			qcreate("swat_spch", SPEECH_PID, swated_ball_speech_proc, ppdata->plyrnum, 0, 0, 0);


	 	return YES;	
	}
	


	if((ppdata->but_cur & P_A))
	{
		if(time_left > 15)
		{
			//printf("swat_p_but_hit_timeout: NO\n");
			return NO;
		}
		else
		{
			//printf("swat_p_but_hit_timeout: YES\n");
			return YES;	
		}
	}
	else
	{
		//printf("swat_p_but_hit_timeout: YES\n");
		return YES;
	}
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
#define THREE_POINT_DIST	77.0f
int three_pt_check(obj_3d *pobj)
{
float nx, nz, dist_to_rim, p[3];

	nx = pobj->x * .965f;	//.90f;
	nz = pobj->z * .965f;	//.95f;

	p[0] = game_info.active_goal ? RIM_X : -RIM_X; p[1] = p[2] = 0.0f;

	dist_to_rim = fsqrt( ((p[0] - nx) * (p[0] - nx)) + ((0.0f - nz) * (0.0f - nz)) );

	if(fabs(pobj->z) >= 66.0f
	|| dist_to_rim >= THREE_POINT_DIST)
		return YES;
	else
		return NO;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
int in_the_paint(bbplyr_data *ppdata, int my_paint)
{

// these values represent the coords of the paint in absolute world space
	if(((my_paint == YES ? ppdata->team : 1 - ppdata->team) ^ game_info.off_goal) == 1)
	{
		if(ppdata->odata.x >= (71.3864f * 1.1f) && fabs(ppdata->odata.z) <= (26.214f * 1.05f))
			return YES;
		else
			return NO;
	}
	else
	{
		if(ppdata->odata.x <= (-71.3864f * 1.1f) && fabs(ppdata->odata.z) <= (26.214f * 1.05f))
			return YES;
		else
			return NO;
	}
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
#define DUNK_DIST_CHECK		62.0f
static int WhatKindOfShot( obj_3d *pobj )
{
int allow_dunk = YES, me = ((bbplyr_data *)pobj)->plyrnum, him = nearest_to_me(me, !((bbplyr_data *)pobj)->team),
	my_pow = ((bbplyr_data *)pobj)->_power, his_pow = player_blocks[him]._power;
float dist_to_rim, rim_pt[3], rim_bearing, bearing, his_dist_to_rim, nx, nz;

	//rim_pt[0] = (((bbplyr_data *)pobj)->team ^ game_info.off_goal) ? RIM_X : -RIM_X; rim_pt[1]= 0.0f; rim_pt[2] = 0.0f;
	rim_pt[0] = game_info.active_goal ? RIM_X : -RIM_X; rim_pt[1] = rim_pt[2] = 0.0f;
	rim_bearing = (pobj->fwd - ptangle( &(pobj->x), rim_pt)) & 1023;

	dist_to_rim = fsqrt( ((rim_pt[0] - pobj->x) * (rim_pt[0] - pobj->x)) + ((0.0f - pobj->z) * (0.0f - pobj->z)) );
	his_dist_to_rim = fsqrt( ((rim_pt[0] - player_blocks[him].odata.x) * (rim_pt[0] - player_blocks[him].odata.x)) + ((0.0f - player_blocks[him].odata.z) * (0.0f - player_blocks[him].odata.z)) );

	if(three_pt_check(pobj) == YES) return JUMPER_3;

// determine if a dunk can happen or not...

//ROB, this is getting called when guy is already up in air doing a dunk


	if( !(pobj->adata[0].animode & MODE_REACTING) && !(pobj->flags & (PF_DUNKER|PF_ALLYDUNKER|PF_FUNKY_DRIBBLE)) )
	{
		//if((distxz[me][him] <= 30.0f && his_dist_to_rim >= 22.0f && dist_to_rim <= DUNK_DIST_CHECK))
		if(dist_to_rim <= DUNK_DIST_CHECK && ((rim_bearing >= 0 && rim_bearing <= 200) || (rim_bearing >= 824 && rim_bearing <= 1024)))
		{// are we close enough to matter...

			// FIX!!!  Maybe we allow allyoop attempts to happen more easily than a regular dunk - distance 15?
			if((in_between_me_basket_pow(me, him) == YES && plyr_bearing(me, him) < 128 && distxz[me][him] <= 24.0f)
			|| (in_between_me_basket_pow(me, him ^ 1) == YES && plyr_bearing(me, him ^ 1) < 128 && distxz[me][him ^ 1] <= 24.0f))
		//	if( (plyr_bearing(me, him) < 64 && distxz[me][him] <= 15.0f) || (plyr_bearing(me, him ^ 1) < 64 && distxz[me][him ^ 1] <= 15.0f))
			{
				allow_dunk = NO;
				if(allow_dunk == NO)
				{
					if (me == game_info.ball_handler)
					{
						// If my dunk was stopped, maybe allow a quick layup...
						if ((randrng(100) <= 25) && (dist_to_rim <= 50) && (distxz[me][him] <= 24.0f) && (distxz[me][him ^ 1] <= 24.0f))
						{
							auto_layup = YES;
#ifdef DEBUG
//							printf("%s: AUTO LAYUP\n",((bbplyr_data *)pobj)->p_spd->szlast);
//							while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
#endif
							// Dampen his vels
							pobj->vx = pobj->vx / 2.0f;
							pobj->vz = pobj->vz / 2.0f;
						}
						else
							// Zero his vels
							pobj->vx = pobj->vz = 0.0f;
					}
					else
					{
						if(ISHUMAN((bbplyr_data*)pobj))
							flash_plyr_red(pobj, -1);
					}

#ifdef DEBUG
//					printf("%s's dunk/allyoop got STOPPED\n",((bbplyr_data *)pobj)->p_spd->szlast);
//					while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
#endif
				}
			}
		}
	}

	if(dist_to_rim <= DUNK_DIST_CHECK
	&& (pobj->vx != 0.0f || pobj->vz != 0.0f)
	&& ((rim_bearing >= 0 && rim_bearing <= 200) || (rim_bearing >= 824 && rim_bearing <= 1024))
	&& allow_dunk == YES
	)
		return DUNK;
	else
		return JUMPER_2;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
static int _mode_chill(bbplyr_data *ppdata)
{
	int k = 0, kk = 0;
	static int cnt = 1;

	cnt++;
	if(cnt < 20)
	{
#ifdef DEBUG
		if(cnt == 5 || cnt == 15)
			dump_joint_positions();
#endif
		for(k = 0; k < 15; k++)
			for(kk = 0; kk < 3; kk++)
				gjpos[k][kk] = ppdata->jpos[k][kk];
		for(k = 0; k < 15; k++)
			for(kk = 0; kk < 12; kk++)
				gmatrices[k][kk] = ppdata->matrices[k][kk];
	}
	else
	{
		((obj_3d *)ppdata)->y = 0.0f;
		((obj_3d *)ppdata)->plyrmode = MODE_OFFENSE;
		change_anim( ((obj_3d *)ppdata), s_breath1_anim );
		cnt = 1;
	}

	return 0;
}

//*********************************************************
#define P_MOVE				0x100		// MOVING PASS
#define P_STILL 			0x200		// STANDING PASS
#define P_AIR				0x400		// SHOT PASS
#define P_NOT_TWOPART		0x800		// NOT A TWO PART MOVING PASS
#define P_LEFT_HAND			0x1000		// ONLY WORKS IF BALL IS IN LEFT HAND
#define P_RIGHT_HAND		0x2000		// ONLY WORKS IF BALL IS IN RIGHT HAND
#define P_BOUNCE			0x4000		// THIS IS A BOUNCE PASS ONLY
#define P_ALLY				0x8000		// THIS PASS CAN BE USED AS AN ALLY OOP PASS
#define P_DOUBLE_DUNK		0x10000		// THIS PASS IS ONLY FOR DOUBLE DUNKS

struct _Pass
{
	void *pass;			// pass script
	float min_dist;		// minimum distance from catcher
	float max_dist;		// maximum distance from catcher
	int flags;			// exclusions???
};

struct _Pass passes[] =
{
// STANDING PASSES
	{&chest_pass_2,				50, -1, P_STILL | Q_QUAD_ALL},										// SEQ_P_CH1_1P	- done
	{&pass_forward_1x, 			-1, -1, P_STILL | Q_QUAD_ALL},   									// SEQ_P_FWD2_B, SEQ_P_FWD2_B - done
	{&pass_forward_1x2,			-1, -1, P_MOVE | P_NOT_TWOPART| Q_QUAD_ALL}, 						// SEQ_P_FWD2_B, SEQ_P_FWD2_B - done
	{&chest_pass_side_1,		40, 120, P_STILL | P_LEFT_HAND | Q_LEFT},							// SEQ_P_SID2_R, SEQ_P_SID2_L - done
	{&chest_pass_3,				40, -1, P_STILL | Q_QUAD_ALL},										// SEQ_P_CH2_1P - done
	{&right_pass_1_r,			40, 105, P_STILL | Q_RIGHT},										// SEQ_P_SID1_R, SEQ_P_SID1_L - done
	{&left_pass_1_l,			40, 105, P_STILL | Q_LEFT},											// SEQ_P_SID1_R, SEQ_P_SID1_L - done
	{&cross_body_side_2, 		-1, 45, P_STILL | P_LEFT_HAND | Q_RIGHT | Q_RIGHT_FRONT},			// SEQ_P_SID3_R, SEQ_P_SID3_L - done
	{&cross_body_side_2, 		-1, 45, P_STILL | P_RIGHT_HAND | Q_LEFT | Q_LEFT_FRONT},			// SEQ_P_SID3_R, SEQ_P_SID3_L - done
	{&run_cross_body_side_2, -1, 60, P_MOVE | P_LEFT_HAND | Q_RIGHT | Q_RIGHT_FRONT},				// SEQ_P_SID3_R, SEQ_P_SID3_L - done
	{&run_cross_body_side_2, -1, 60, P_MOVE | P_RIGHT_HAND | Q_LEFT | Q_LEFT_FRONT},				// SEQ_P_SID3_R, SEQ_P_SID3_L - done
	{&run_cross_body_side_2, -1, 60, P_MOVE | P_LEFT_HAND | Q_RIGHT | Q_RIGHT_FRONT},				// SEQ_P_SID3_R, SEQ_P_SID3_L - done
	{&run_cross_body_side_2, -1, 60, P_MOVE | P_RIGHT_HAND | Q_LEFT | Q_LEFT_FRONT},				// SEQ_P_SID3_R, SEQ_P_SID3_L - done
	{&cross_body_side_3, 		-1, 45, P_STILL | P_LEFT_HAND | Q_RIGHT | Q_RIGHT_FRONT},			// SEQ_P_SID4_R, SEQ_P_SID4_L - done
	{&cross_body_side_3, 		-1, 45, P_STILL | P_RIGHT_HAND | Q_LEFT | Q_LEFT_FRONT},			// SEQ_P_SID4_R, SEQ_P_SID4_L - done
	{&run_cross_body_side_3, -1, 50, P_MOVE | P_LEFT_HAND | Q_RIGHT | Q_RIGHT_FRONT},				// SEQ_P_SID4_R, SEQ_P_SID4_L - done
	{&run_cross_body_side_3, -1, 50, P_MOVE | P_RIGHT_HAND | Q_LEFT | Q_LEFT_FRONT},				// SEQ_P_SID4_R, SEQ_P_SID4_L - done
	{&cross_body_side_4, 		-1, 45, P_STILL | Q_RIGHT | Q_RIGHT_FRONT | Q_FRONT},				// SEQ_P_SID5_R, SEQ_P_SID5_L - done
	{&cross_body_side_4, 		-1, 45, P_STILL | Q_LEFT | Q_LEFT_FRONT | Q_FRONT},					// SEQ_P_SID5_R, SEQ_P_SID5_L - done
	{&run_cross_body_side_4, -1, 50, P_MOVE | Q_RIGHT | Q_RIGHT_FRONT | Q_FRONT},					// SEQ_P_SID5_R, SEQ_P_SID5_L - done
	{&run_cross_body_side_4, -1, 50, P_MOVE | Q_LEFT | Q_LEFT_FRONT | Q_FRONT},						// SEQ_P_SID5_R, SEQ_P_SID5_L - done
	{&pass_forward_2, 			-1, 45, P_STILL | Q_FRONT | Q_LEFT_FRONT | Q_RIGHT_FRONT },			// SEQ_P_FWD1_R, SEQ_P_FWD1_L - done
	{&run_pass_forward_2, -1, 60, P_MOVE | Q_FRONT | Q_LEFT_FRONT | Q_RIGHT_FRONT },				// SEQ_P_FWD1_R, SEQ_P_FWD1_L - done
	{&pass_forward_3, 			20, 60, P_STILL | Q_QUAD_ALL},   									// SEQ_P_FWD3_B, SEQ_P_FWD3_B - done
	{&run_pass_forward_1, 10, 70, P_MOVE | Q_FRONT | Q_LEFT_FRONT | Q_RIGHT_FRONT },				// SEQ_P_FWD2_B - done
	{&run_pass_forward_3, 10, 70, P_MOVE | Q_FRONT},												// SEQ_P_FWD3_B - done
	{&cross_body_diag_behind_1, 10, 60, P_STILL | P_LEFT_HAND | Q_RIGHT_BEHIND | Q_BEHIND},			// SEQ_P_BCHS_R, SEQ_P_BCHS_L - done
	{&cross_body_diag_behind_1, 10, 60, P_STILL | P_RIGHT_HAND | Q_LEFT_BEHIND | Q_BEHIND},			// SEQ_P_BCHS_R, SEQ_P_BCHS_L - done
	{&run_cross_body_diag_behind_1, 10, 80, P_MOVE | P_LEFT_HAND | Q_RIGHT_BEHIND | Q_BEHIND},		// SEQ_P_BCHS_R, SEQ_P_BCHS_L - done
	{&run_cross_body_diag_behind_1, 10, 80, P_MOVE | P_RIGHT_HAND | Q_LEFT_BEHIND | Q_BEHIND},		// SEQ_P_BCHS_R, SEQ_P_BCHS_L - done
	{&cross_body_side_1, 		10, 70, P_STILL | P_LEFT_HAND | Q_RIGHT},							// SEQ_P_CROS_R, SEQ_P_CROS_L - done
	{&cross_body_side_1, 		10, 70, P_STILL | P_RIGHT_HAND | Q_LEFT},							// SEQ_P_CROS_R, SEQ_P_CROS_L - done
	{&run_cross_body_side_1, 10, 70, P_MOVE | P_LEFT_HAND | Q_RIGHT},								// SEQ_P_CROS_R, SEQ_P_CROS_L - done
	{&run_cross_body_side_1, 10, 70, P_MOVE | P_RIGHT_HAND | Q_LEFT},								// SEQ_P_CROS_R, SEQ_P_CROS_L - done
	{&pass_behind_1, 			15, 80, P_STILL | Q_BEHIND },										// SEQ_P_BACK_R, SEQ_P_BACK_L - done
	{&run_pass_behind_1, 15, 80, P_MOVE | Q_BEHIND },												// SEQ_P_BACK_R, SEQ_P_BACK_L - done
	{&pass_behind_2, 			15, 45, P_STILL | Q_BEHIND },										// SEQ_P_BACK2R, SEQ_P_BACK2L - done
	{&run_pass_behind_2, 15, 45, P_MOVE | Q_BEHIND },												// SEQ_P_BACK2R, SEQ_P_BACK2L - done
	{&pass_behind_3, 			30, 80, P_STILL | Q_BEHIND },										// SEQ_P_OBCK_R, SEQ_P_OBCK_L - done
	{&run_pass_behind_3, 30, 80, P_MOVE | Q_BEHIND },												// SEQ_P_OBCK_R, SEQ_P_OBCK_L - done
	{&pass_behind_back_side_1,	20, 50, P_STILL | P_RIGHT_HAND | Q_LEFT },							// SEQ_P_BBCK_R, SEQ_P_BBCK_L - done
	{&pass_behind_back_side_1,	20, 50, P_STILL | P_LEFT_HAND | Q_RIGHT },							// SEQ_P_BBCK_R, SEQ_P_BBCK_L - done
	{&run_pass_behind_back_side_1, 20, 50, P_MOVE | P_RIGHT_HAND | Q_LEFT },						// SEQ_P_BBCK_R, SEQ_P_BBCK_L - done
	{&run_pass_behind_back_side_1, 20, 50, P_MOVE | P_LEFT_HAND | Q_RIGHT },						// SEQ_P_BBCK_R, SEQ_P_BBCK_L - done
	{&pass_behind_back_side_2,	10, 50, P_STILL | P_RIGHT_HAND | Q_LEFT },							// SEQ_P_BBKB_R, SEQ_P_BBKB_L - done
	{&pass_behind_back_side_2,	10, 50, P_STILL | P_LEFT_HAND | Q_RIGHT },							// SEQ_P_BBKB_R, SEQ_P_BBKB_L - done
	{&run_pass_behind_back_side_2, 10, 50, P_MOVE | P_RIGHT_HAND | Q_LEFT },						// SEQ_P_BBKB_R, SEQ_P_BBKB_L - done
	{&run_pass_behind_back_side_2, 10, 50, P_MOVE | P_LEFT_HAND | Q_RIGHT },						// SEQ_P_BBKB_R, SEQ_P_BBKB_L - done
	{&bounce_pass_side_1, 35, 70, P_STILL | P_BOUNCE | P_RIGHT_HAND | Q_LEFT | Q_LEFT_BEHIND},		// SEQ_P_DIAB_R, SEQ_P_DIAB_L - done
	{&bounce_pass_side_1, 35, 70, P_STILL | P_BOUNCE | P_LEFT_HAND | Q_RIGHT | Q_RIGHT_BEHIND},		// SEQ_P_DIAB_R, SEQ_P_DIAB_L - done
	{&run_bounce_pass_side_1, 35, 70, P_MOVE | P_BOUNCE | P_RIGHT_HAND | Q_LEFT | Q_LEFT_BEHIND},	// SEQ_P_DIAB_R, SEQ_P_DIAB_L - done
	{&run_bounce_pass_side_1, 35, 70, P_MOVE | P_BOUNCE | P_LEFT_HAND | Q_RIGHT | Q_RIGHT_BEHIND},	// SEQ_P_DIAB_R, SEQ_P_DIAB_L - done
	{&run_bounce_pass_side_1, 35, 70, P_MOVE | P_BOUNCE | P_RIGHT_HAND | Q_LEFT | Q_LEFT_BEHIND},	// SEQ_P_DIAB_R, SEQ_P_DIAB_L - done
	{&run_bounce_pass_side_1, 35, 70, P_MOVE | P_BOUNCE | P_LEFT_HAND | Q_RIGHT | Q_RIGHT_BEHIND},	// SEQ_P_DIAB_R, SEQ_P_DIAB_L - done
	{&over_shoulder_pass, 30, -1, P_STILL | P_RIGHT_HAND | Q_LEFT_BEHIND | Q_BEHIND},				// SEQ_P_BCK1_R, SEQ_P_BCK1_L - done
	{&over_shoulder_pass, 30, -1, P_STILL | P_LEFT_HAND | Q_BEHIND | Q_RIGHT_BEHIND},				// SEQ_P_BCK1_R, SEQ_P_BCK1_L - done
	{&running_over_shoulder_pass, 30, -1, P_MOVE | P_RIGHT_HAND | Q_BEHIND | Q_LEFT_BEHIND},		// SEQ_P_BCK1_R, SEQ_P_BCK1_L - done
	{&running_over_shoulder_pass, 30, -1, P_MOVE | P_LEFT_HAND | Q_BEHIND | Q_RIGHT_BEHIND},		// SEQ_P_BCK1_R, SEQ_P_BCK1_L - done
	{&pass_behind_4, 20, 40, P_STILL | Q_BEHIND },													// SEQ_P_BCK2_R, SEQ_P_BCK2_L - done
	{&overhead_2_pass, 75, -1, P_STILL | Q_QUAD_ALL},												// SEQ_P_OVR2_B - done
	{&overhead_2_pass, 75, -1, P_STILL | Q_QUAD_ALL},												// SEQ_P_OVR2_B - done
	{&overhead_2_pass, 75, -1, P_STILL | Q_QUAD_ALL},												// SEQ_P_OVR2_B - done  //58
	{&overhead_2_pass, 75, -1, P_STILL | Q_QUAD_ALL},												// SEQ_P_OVR2_B - done
	{&forward_bounce_pass_1, 30, 130, P_STILL | P_BOUNCE | Q_FRONT},								// SEQ_P_FWDB_R, SEQ_P_FWDB_L - done
	{&run_forward_bounce_pass_1, 30, 130, P_MOVE | P_BOUNCE | Q_FRONT},								// SEQ_P_FWDB_R, SEQ_P_FWDB_L - done
	{&forward_hardpass_1, 70, -1, P_STILL | Q_QUAD_ALL},											// SEQ_P_HARD_R, SEQ_P_HARD_L - done
	{&run_forward_hardpass_1, 70, -1, P_MOVE | Q_LEFT_FRONT | Q_FRONT},								// SEQ_P_HARD_R, SEQ_P_HARD_L - done
// pass # 67 is next
	{&overhead_1_pass, 95, -1, P_STILL | Q_QUAD_ALL},												// SEQ_P_OVR_HD - done
	{&running_overhead_1_pass, 95, -1, P_MOVE | Q_FRONT },											// SEQ_P_OVR_HD - done

	{&pass_fullcourt_1, 100, -1, P_STILL | Q_QUAD_ALL},												// SEQ_P_FCP1_R, SEQ_P_FCP1_L - done
	{&run_pass_fullcourt_1, 100, -1, P_MOVE | Q_FRONT},												// SEQ_P_FCP1_R, SEQ_P_FCP1_L - done

	{&run_forward_spinpass_1, 50, 180, P_BOUNCE | P_NOT_TWOPART | P_MOVE | P_RIGHT_HAND | Q_LEFT_FRONT | Q_FRONT},	// SEQ_P_PAS1_R, SEQ_P_PAS1_L - done
	{&run_forward_spinpass_1, 50, 180, P_NOT_TWOPART | P_MOVE | P_RIGHT_HAND | Q_LEFT_FRONT | Q_FRONT},			 	// SEQ_P_PAS1_R, SEQ_P_PAS1_L - done
	{&run_forward_spinpass_1, 50, 180, P_BOUNCE | P_NOT_TWOPART | P_MOVE | P_LEFT_HAND | Q_RIGHT_FRONT | Q_FRONT},	// SEQ_P_PAS1_R, SEQ_P_PAS1_L - done
	{&run_forward_spinpass_1, 50, 180, P_NOT_TWOPART | P_MOVE | P_LEFT_HAND | Q_RIGHT_FRONT | Q_FRONT},				// SEQ_P_PAS1_R, SEQ_P_PAS1_L - done

	// lower freq. on this pass please...
	{&chest_pass_1, 90, -1, P_STILL | Q_QUAD_ALL},													// SEQ_P_CHEST - done
	{&running_chest_pass, 90, -1, P_MOVE | Q_FRONT},												// SEQ_P_CHEST - done

	{&run_pass_forward_4, 90, -1, P_NOT_TWOPART | P_RIGHT_HAND | P_MOVE | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT},	// SEQ_P_FRD1_R, SEQ_P_FRD1_L - done
	{&run_pass_forward_4, 90, -1, P_NOT_TWOPART | P_LEFT_HAND | P_MOVE | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT}, 	// SEQ_P_FRD1_R, SEQ_P_FRD1_L - done

 	{&run_pass_diag_left_1, 25, 130, P_MOVE | Q_LEFT_FRONT },										// SEQ_P_DIA1_R - done
	{&run_pass_diag_right_1, 25, 130, P_MOVE | Q_RIGHT_FRONT },										// SEQ_P_DIA1_L - done
 	{&run_pass_diag_left_1, 25, 130, P_MOVE | Q_LEFT_FRONT },										// SEQ_P_DIA1_R - done
	{&run_pass_diag_right_1, 25, 130, P_MOVE | Q_RIGHT_FRONT },										// SEQ_P_DIA1_L - done
	{&running_pass_thru_legs_behind, 50, 120, P_NOT_TWOPART | P_MOVE | Q_BEHIND},					// SEQ_P_BTWN_R, SEQ_P_BTWN_L - done

	{&chest_pass_side_1,		40, 120, P_STILL | P_RIGHT_HAND | Q_RIGHT},							// SEQ_P_SID2_R, SEQ_P_SID2_L - done
	{&run_chest_pass_side_1, 40, -1, P_MOVE | Q_RIGHT_FRONT | Q_LEFT_FRONT | Q_RIGHT | Q_LEFT},	  	// SEQ_P_SID2_R, SEQ_P_SID2_L - done
	{&run_chest_pass_side_1, 40, -1, P_MOVE | Q_RIGHT_FRONT | Q_LEFT_FRONT | Q_RIGHT | Q_LEFT},	  	// SEQ_P_SID2_R, SEQ_P_SID2_L - done
	{&run_chest_pass_side_1, 40, -1, P_MOVE | Q_RIGHT_FRONT | Q_LEFT_FRONT | Q_RIGHT | Q_LEFT},	  	// SEQ_P_SID2_R, SEQ_P_SID2_L - done
	{&pass_forward_1, 			20, 55, P_STILL | Q_QUAD_ALL},   									// SEQ_P_FWD2_B, SEQ_P_FWD2_B - done

// JUMPSHOT/IN AIR PASSES

	{&air_pass_1, -1, -1, P_AIR | Q_BEHIND },														// SEQ_PAIR1_R, SEQ_PAIR1_L - done
	{&air_pass_2, -1, -1, P_AIR | Q_LEFT_BEHIND | Q_BEHIND | Q_RIGHT_BEHIND},						// SEQ_PAIR2_R, SEQ_PAIR2_L - done
//	{&air_pass_3, -1, -1, P_AIR | Q_LEFT_FRONT  | Q_FRONT  | Q_RIGHT_FRONT},						// SEQ_PAIR3_B - done
	{&air_pass_3, -1, -1, P_AIR | Q_QUAD_ALL},														// SEQ_PAIR3_B - done
	{&air_pass_4, -1, -1, P_AIR | Q_RIGHT},															// SEQ_PAIR4_R, SEQ_PAIR4_L - done
	{&air_pass_4, -1, -1, P_AIR | Q_LEFT},															// SEQ_PAIR4_R, SEQ_PAIR4_L - done
	{&air_pass_5, -1, -1, P_AIR | Q_RIGHT},															// SEQ_PAIR5_R, SEQ_PAIR5_L - done
	{&air_pass_5, -1, -1, P_AIR | Q_LEFT},															// SEQ_PAIR5_R, SEQ_PAIR5_L - done
	// Need 1 default pass for every angle for in air pass attempts
//	{&chest_pass_2,		-1, -1, P_AIR | Q_QUAD_ALL},												// SEQ_P_CH1_1P - fix/need new default seq

// ALLY OOP PASSES
	// STANDING
	//{&lob1_pass_anim, -1, -1, P_ALLY | P_MOVE | P_NOT_TWOPART | Q_QUAD_ALL},						// SEQ_P_CHEST
	//{&lob1_pass_anim, -1, -1, P_ALLY | P_STILL | Q_QUAD_ALL},										// SEQ_P_CHEST
//	{&overhead_1_pass_ally, -1, -1, P_ALLY | P_NOT_TWOPART | Q_QUAD_ALL},											// SEQ_P_OVR_HD
	//{&allyoop_pass_1, -1, -1, P_ALLY | P_NOT_TWOPART | Q_QUAD_ALL},										// SEQ_P_ALLY3
	{&allyoop_pass_2, -1, -1, P_ALLY | P_NOT_TWOPART | Q_QUAD_ALL},										// SEQ_P_ALLY3
	{&ally_air_pass_1,-1, -1, P_ALLY | P_AIR | P_STILL | Q_QUAD_ALL},														// SEQ_PAIR3_B - done

// RUNNING ALLY OOP PASSES
	//{&twohand_ally_pass_1, -1, -1, P_ALLY | P_MOVE | P_NOT_TWOPART | Q_FRONT | Q_BEHIND},					// SEQ_P_ALLY1
	{&running_overhead_1_pass_ally, -1, -1, P_ALLY | P_MOVE | Q_FRONT},							// SEQ_P_OVR_HD

// DOUBLE DUNK PASSES - Pass 99
	{&dunk_pass_1, -1, -1, P_DOUBLE_DUNK | P_AIR | Q_QUAD_ALL},										// SEQ_P_OVR_HD
	{&dunk_pass_2, -1, -1, P_DOUBLE_DUNK | P_AIR | Q_QUAD_ALL},										// SEQ_P_OVR_HD

// UNFINISHED PASSES
//	{&pass_right_1, -1, -1, P_STILL | Q_RIGHT_FRONT | Q_RIGHT},										// SEQ_P_BBK1_R, SEQ_P_BBK1_L - not done
//	{&pass_left_1, -1, -1, P_STILL | Q_LEFT_FRONT | Q_LEFT},										// SEQ_P_BBK1_R, SEQ_P_BBK1_L - not done
//	{&chest_pass_anim, -1, 50, P_STILL | Q_QUAD_ALL},												// SEQ_P_OUTB1 - ugly!
//	{&run_chest_pass_side_1, -1, -1, P_MOVE | P_RIGHT_HAND | Q_RIGHT},								// SEQ_P_SID2_R, SEQ_P_SID2_L - not needed?
//	{&run_chest_pass_side_1, -1, -1, P_MOVE | P_LEFT_HAND | Q_RIGHT},								// SEQ_P_SID2_R, SEQ_P_SID2_L - not needed?
//	{&run_chest_pass_side_1, -1, -1, P_MOVE | P_LEFT_HAND | Q_LEFT},								// SEQ_P_SID2_R, SEQ_P_SID2_L - not needed?
//	{&run_pass_side_1, -1, -1, P_MOVE | P_RIGHT_HAND | Q_RIGHT_FRONT | Q_RIGHT},					// SEQ_P_SID1_R, SEQ_P_SID1_L - broken
//	{&run_pass_side_1, -1, -1, P_MOVE | P_LEFT_HAND | Q_LEFT_FRONT | Q_LEFT},						// SEQ_P_SID1_R, SEQ_P_SID1_L - broken

};

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
#define OOB_STOP_CATCH_RANGE 86
#define MIN_CATCH_TIME	15
#define NUM_PASSES	(sizeof(passes)/sizeof(struct _Pass))

// quad4 works as follows:
// receiver is: 0-behind, 1-left, 2-front, 3-right
// quad8 works as follows:
// receiver is:
//	0-behind, 1-left.behind, 2-left, 3-left.front
//	4-front, 5-right.front, 6-right, 7-right.behind
int is_bounce_pass = NO;
void Get_A_Pass(obj_3d * pobj, int receiver, int allyoop)
{
	register bbplyr_data * ppd = (bbplyr_data *)pobj;
	register game_info_t * pgi = &game_info;
	register ball_data * pbo = &ball_obj;

	int i, indx, rim_bearing, angle_to_board, stop_him = NO, gnd_pass = NO, air_time = 1000;
	int quad4, quad8, nfwd, quad_test = 0, select_pass[NUM_PASSES];
	float dist_to_bboard, rim_pt[3], p[3], pvel;
	float f_time, c_time, dist, time_to_oob, tx, tz, cx, cz, rx, rz;
	float a,b,c,b2m4ac,root1,root2;
	char * pass_anim = NULL;

	// should a pass be allowed...
	if(player_blocks[receiver].odata.flags & (PF_SWATING|PF_STEALING|PF_DUNKER|PF_IN_AIR_BLOCK|PF_REBOUND|PF_QUICK_REBOUND|PF_SHOOTING|PF_LAYUP))
		return;

	if((player_blocks[receiver].odata.adata[0].animode & MODE_REACTING) && (player_blocks[receiver].odata.y > 0.0f))
		return;

//	if (player_blocks[receiver].odata.adata[0].animode & MODE_REACTING)
//		return;
	if ((player_blocks[receiver].odata.y > 0.0f) && !(player_blocks[receiver].odata.flags & (PF_ALLYDUNKER|PF_DUNKER|PF_LAYUP)))
		return;

	// If ally ooper receiver, and I'm across half court - ignore pass butn
	if((player_blocks[receiver].odata.flags & PF_ALLYDUNKER)
	&& !(plyr_crossed_halfcourt(ppd->plyrnum)))
	//&& distxz[ppd->plyrnum][receiver] > 80.0f)
	{
#ifdef DEBUG
		printf("Pass to ally oop ignored - not over half court!\n");
#endif
		return;
	}

//	|| (player_blocks[receiver].odata.y > 0.0f && !(player_blocks[receiver].odata.flags & PF_ALLYDUNKER))
//#ifndef DEBUG_NO_ALLYOOP
//	|| (player_blocks[receiver].odata.y > 0.0f && (player_blocks[receiver].odata.flags & PF_ALLYDUNKER))
//#endif
//	|| (player_blocks[receiver].odata.adata[0].animode & MODE_REACTING)
//	)
//		return;

	if(player_blocks[receiver].total_time == 0)
		f_time = 0.0f;
	else
		f_time = player_blocks[receiver].total_time - player_blocks[receiver].flight_time;
		//f_time = (float)player_blocks[receiver].flight_time / (float)player_blocks[receiver].total_time;

	// If ally ooper too close to rim, ignore pass butn
	//if(player_blocks[receiver].odata.flags & PF_ALLYDUNKER && f_time > 0.525f)
	if(player_blocks[receiver].odata.flags & PF_ALLYDUNKER && f_time < 35)
	{
#ifdef DEBUG
		printf("too late on the ALLYOOP PASS!!!\n");
//	while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
#endif
		return;
	}

// was this player cocky dribbling...
	ticks_since_cocky = -COCKY_DRIBBLE_TWAIT, cocky_dribbler = NO_BALLHANDLER;

// should this pass just be a regular ground pass...
	a = GRAVITY / 2.0f;	b = pobj->vy;	c = pobj->y;	b2m4ac = b * b - 4.0f * a * c;
	root1 = (-1.0f * b + fsqrt( b2m4ac )) / (2.0f * a);
	root2 = (-1.0f * b - fsqrt( b2m4ac )) / (2.0f * a);
	if(root1 > 1.0f)
		air_time = (int)(root1 + 0.9f);

	if(root2 > 1.0f)
		air_time = (int)(root2 + 0.9f);

	if(air_time <= 8)
		gnd_pass = YES;

#ifdef DEBUG_PASSING
	printf("air_time = %d, gnd_pass = %s\n",air_time, gnd_pass == YES ? "YES" : "NO");
#endif

#ifdef DEBUG
	if(receiver < 0) { fprintf(stderr,"No receiver # in Get_A_Pass()\r\n"); lockup(); }
#endif

#ifdef DEBUG_PASSING
	if(ppd->odata.flags & PF_SHOOTING)
		printf("Passer is also shooting\n");
#endif

	pbo->int_receiver = -1;

	// Are we doing an ally off the backboard?
	// is the offender going toward the basket at all?
	rim_pt[0] = pgi->active_goal ? RIM_X : -RIM_X; rim_pt[1] = 0.0f; rim_pt[2] = 0.0f;

	angle_to_board = ptangle( &(ppd->odata.x), rim_pt);

	rim_bearing = (ppd->odata.fwd - ptangle( &(ppd->odata.x), rim_pt)) & 1023;

	if(rim_bearing > 512) rim_bearing = 1024 - rim_bearing;

	dist_to_bboard = fsqrt(
		(rim_pt[0] - ppd->odata.x) * (rim_pt[0] - ppd->odata.x) +
		(rim_pt[2] - ppd->odata.z) * (rim_pt[2] - ppd->odata.z));

//#ifdef DEBUG_ALLOW_BACKBOARD_ALLYS
	if(!(ppd->odata.flags & (PF_DUNKER|PF_ALLYDUNKER|PF_SHOOTING))
	&& !(ppd->odata.adata[0].animode & MODE_REACTING)
	&& ((ISHUMAN(ppd)) || (ISDRONE(ppd) && ISDRONE(&player_blocks[ppd->plyrnum ^ 1])))
	)
	{
	int nearest = nearest_to_me(ppd->plyrnum, !ppd->team);

		// allow if not already dunking
		// does the circumstance allow the funkyness?
		if ((dist_to_bboard >= 50.0f && dist_to_bboard <= 70.0f)
			//&& (rim_bearing <= 128 || rim_bearing >= 896)
			&& (rim_bearing <= 64)
			&& (
				(ppd->odata.x > 0.0f && angle_to_board >= 224 && angle_to_board <= 288) ||
				(ppd->odata.x < 0.0f && angle_to_board >= 736 && angle_to_board <= 800)
			)
			//&& ((plyr_bearing(ppd->plyrnum, nearest) > 128) || (plyr_bearing(ppd->plyrnum, nearest) < 128 && distxz[ppd->plyrnum][nearest] >= 40.0f))
			&& !( (in_between_ball_basket_pow(&player_blocks[nearest]) == YES && distxz[ppd->plyrnum][nearest] <= 30.0f)
				 || (in_between_ball_basket_pow(&player_blocks[nearest ^ 1]) == YES && distxz[ppd->plyrnum][nearest ^ 1] <= 30.0f))
			&& (ppd->odata.vx != 0.0f || ppd->odata.vz != 0.0f)
			&& (ppd->odata.y == 0.0f)
			&& ( (game_info.active_goal == 0 && player_blocks[receiver].odata.x > ppd->odata.x)
				|| (game_info.active_goal == 1 && player_blocks[receiver].odata.x < ppd->odata.x))
			&& distxz[ppd->plyrnum][ppd->plyrnum ^ 1] >= 35.0f
			)
		{
			// Throw a funky ally off the backboard
			pgi->passer       = ppd->plyrnum;
#if NUM_PERTEAM > 2
			pgi->receiver_a   = NO_RECEIVER;
			pgi->receiver_b   = NO_RECEIVER;
#endif
			pbo->who_threw    = ppd->plyrnum;
			pbo->int_receiver = ppd->plyrnum;
			pbo->flags       |= BF_NOINTERCEPT;
			change_anim(&ppd->odata, run_forward_spinpass_bb_1 );
#ifdef DEBUG
			printf("%s is performing an ally off the backboard\n", ppd->p_spd->szlast);
#endif
			return;
		}
	}
//#endif

// passes are chosen from a table of passes by:
//	1. passer->fwd
//	2. angle between passer and receiver
//	3. distance between the two

// is the receiver an PF_ALLYDUNKER...
	if(allyoop == NO
		&& player_blocks[receiver].odata.flags & PF_ALLYDUNKER
		&& !(pobj->flags & (PF_DUNKER|PF_ALLYDUNKER))
		)
		allyoop = YES;

// if passer is attempting a double dunk pass clear the allow allyoop flag
	if(player_blocks[receiver].odata.flags & PF_ALLYDUNKER && pobj->flags & (PF_DUNKER|PF_ALLYDUNKER|PF_LAYUP))
		allyoop = NO, gDouble_dunk = YES;
	else
		gDouble_dunk = NO;

// get time to receiver
	//if(pobj->vx == 0.0f && pobj->vz == 0.0f)
	//	dist = distxz[ppd->plyrnum][receiver];
	//else
	//{
		cx = pobj->x + 25.0f * pobj->vx;
		cz = pobj->z + 25.0f * pobj->vz;
		rx = player_blocks[receiver].odata.x + 25.0f * player_blocks[receiver].odata.vx;
		rz = player_blocks[receiver].odata.z + 25.0f * player_blocks[receiver].odata.vz;

		dist = fsqrt((cx - rx) * (cx - rx) + (cz - rz) * (cz - rz));
	//}

// ball's travel time is capped at min 15.0f, max 75.0f
	if(dist <= 25.0f)
		c_time = 11.0f; //10 //07	// for close in passes...
	else
	{
		c_time = (MAX_PASS_TIME * (dist / MAX_PASS_DIST));
		c_time = GREATER(c_time, MIN_PASS_TIME);
	}

	if(dist <= 19.0f)
		c_time = 10.0f; //9.0f; // 07 //15.0f;	// for close in passes...

	pbo->flags &= ~BF_TURBO;

	if(pobj->flags & PF_TURBO)
	{
		c_time *= 0.93f; //0.90f; // .85 //0.95f;	//.90
		pbo->flags |= BF_TURBO;
		//printf("A TURBO PASS: catch_time = %d\n",(int)c_time);
	}

	p[1] = 0.0f;
	plyr_will_be_at(&nfwd, &p[0], &p[2], (int)c_time, receiver);

	rx = p[0];
	rz = p[2];
	dist = fsqrt((cx - rx) * (cx - rx) + (cz - rz) * (cz - rz));

	quad8 = player_blocks[pgi->ball_handler].odata.tgt_fwd - ptangle(&(ppd->odata.x), p);
	quad4 = ((quad8 + 1536 + 128)&1023) / 256;		// four directions
	quad8 = ((quad8 + 1536 +  64)&1023) / 128;		// eight directions

// quad4 works as follows:
// receiver is: 0-behind, 1-left, 2-front, 3-right
//
// quad8 works as follows:
// receiver is:
//	0-behind, 1-left.behind, 2-left, 3-left.front
//	4-front, 5-right.front, 6-right, 7-right.behind

	pvel = fsqrt((pobj->vx * pobj->vx) + (pobj->vz * pobj->vz));

	// DETERMINE IF PASSER SHOULD BE STOPPED
	if(pvel != 0.0f)	//pobj->vx != 0.0f || pobj->vz != 0.0f)
	{
		if(fabs(pobj->x + 25.0f * pobj->vx) >= OUT_OF_BOUNDS_X
		|| fabs(pobj->z + 25.0f * pobj->vz) >= OUT_OF_BOUNDS_Z)
			stop_him = YES;//, printf("1: STOPPED PASSER\n");

		if(plyr_bearing(ppd->plyrnum, nearest_to_me(ppd->plyrnum, !ppd->team)) <= 70
			&& intercept_time(ppd->plyrnum, nearest_to_me(ppd->plyrnum, !ppd->team), ppd->_speed ) < 20)
			stop_him = YES;// printf("22: STOPPED PASSER\n");

		if((pvel <= 1.0f) || pobj->flags & PF_FUNKY_DRIBBLE)
			stop_him = YES;

	// Make the passer stop some of the time even though he could do a running pass
	// It may look better to sometimes stop
	// there is a chance that the passer will stop
//		if(randrng(100) > 80)
//			stop_him = YES;
	}

	// TAG ALL LEGAL PASSES
	quad_test |= 1<<quad8;

	for(i = 0, indx = -1; i < NUM_PASSES; i++)
	{
		select_pass[i] = -1;

		// does this pass require the ball to be in a particular hand...
		if((passes[i].flags & P_LEFT_HAND  && ppd->ball_in_hand == RIGHT_HAND)
		|| (passes[i].flags & P_RIGHT_HAND && ppd->ball_in_hand == LEFT_HAND))
			continue;

// TAKE OUT THESE TWO LINES TO TEST PASSES
		// does this pass work with this quad8...
		if(passes[i].flags != -1 && !(passes[i].flags & quad_test))
			continue;

// TAKE OUT THESE 3 LINES TO TEST PASSES
		// does this pass work with the min and max allowed dist between the passer and receiver...
		if((passes[i].min_dist > 0.0f && dist < passes[i].min_dist)
		|| (passes[i].max_dist > 0.0f && dist3d(&ppd->odata.x, p) > passes[i].max_dist))
			continue;

		// if player is passing in the air does this pass work...
		if( ((ppd->odata.flags & (PF_SHOOTING|PF_DUNKER|PF_ALLYDUNKER) || ppd->odata.y > 0.0f) && !(passes[i].flags & P_AIR) && gnd_pass == NO)
		|| (passes[i].flags & P_AIR && gnd_pass == NO && ppd->odata.y > 0.0f && ppd->odata.flags & (PF_SHOOTING|PF_DUNKER|PF_ALLYDUNKER) && ( ((float)ppd->total_time * .84f) < ppd->flight_time) )
		|| (passes[i].flags & P_AIR && gnd_pass == NO && (ppd->odata.y <= 0.0f && !(ppd->odata.flags & (PF_SHOOTING|PF_DUNKER|PF_ALLYDUNKER))) ))
			continue;

		// is this a standing pass only...
		if((passes[i].flags & P_STILL) && gnd_pass == NO && ((ppd->odata.vx != 0.0f || ppd->odata.vz != 0.0f) && !stop_him))
			continue;

		// is this a moving pass only...
		if((passes[i].flags & P_MOVE) && ((ppd->odata.vx == 0.0f && ppd->odata.vz == 0.0f) || stop_him || gnd_pass == YES))
			continue;

		// if this is an ally oop shot can this pass be used...
		if((allyoop == YES && !(passes[i].flags & P_ALLY))
		|| (allyoop == NO && passes[i].flags & P_ALLY))
			continue;

		// is this a double dunk pass...
		if( ((ppd->odata.flags & (PF_DUNKER|PF_ALLYDUNKER|PF_LAYUP) && (player_blocks[receiver].odata.flags & PF_ALLYDUNKER)) && !(passes[i].flags & P_DOUBLE_DUNK))
		|| (passes[i].flags & P_DOUBLE_DUNK
			&& (!(ppd->odata.flags & (PF_DUNKER|PF_ALLYDUNKER|PF_LAYUP))
		|| 	 (!(player_blocks[receiver].odata.flags & PF_ALLYDUNKER))
		/*|| 	 ( ((float)player_blocks[receiver].total_time * .75f) < player_blocks[receiver].flight_time)*/ ) )
		)
			continue;

		select_pass[++indx] = i;
	}

	// This is for a pass from a fallen ball owner - he just wings/hurls the ball in the direction
	// of his tmate
	// If he is near, the pass will go to him, otherwise - his tmate is not locked into a leading pass
	if (pobj->adata[0].animode & MODE_REACTING)
	{

		// Needed here because regular pass snd is called from script
		// These flail passes don't go to any seq.
		snd_scall_bank(gameplay_bnk_str, PASS1_SND, VOLUME7, 127, PRIORITY_LVL5);

		if(distxz[ppd->plyrnum][receiver] <= 60.0f + (100.0f * ppd->_passing / 20.0f))
		{
			pbo->flags |= BF_NOINTERCEPT;
			pgi->ball_mode = BM_PASSING;
			pbo->int_receiver = ppd->plyrnum ^ 1;
			pbo->who_threw    = ppd->plyrnum;
			pgi->passer       = ppd->plyrnum;
			is_bounce_pass = NO;
			game_info.ball_handler = NO_BALLHANDLER;

			pass_ball(pobj,&player_blocks[ppd->plyrnum ^ 1].odata, NO);

			if(!(player_blocks[ppd->plyrnum ^ 1].odata.flags & PF_ALLYDUNKER))
				player_blocks[ppd->plyrnum ^ 1].odata.adata[0].animode &= ~MODE_UNINT;

			return;
		}
		else
		{
			pbo->flags |= BF_NOINTERCEPT;
			pgi->ball_mode = BM_FREE_ON_COURT;
			pbo->int_receiver = NO_RECEIVER;
			pbo->who_threw    = NO_PASSER;
			pgi->passer       = NO_PASSER;
			is_bounce_pass		= NO;
			game_info.ball_handler = NO_BALLHANDLER;

			pass_ball(pobj,&player_blocks[ppd->plyrnum ^ 1].odata, YES);

			//if(!(pobj->flags & PF_ALLYDUNKER))
			//	player_blocks[ppd->plyrnum^1].odata.adata[0].animode &= ~MODE_UNINT;

			return;
		}
	}

	if(indx == -1)
	{
	char *q8[] = {
		"BEHIND",	"LEFT BEHIND", "LEFT", "LEFT FRONT",
		"FRONT", "RIGHT FRONT",	"RIGHT", "RIGHT BEHIND"
		}, *hand[2] = {"RIGHT", "LEFT"};
		// can't find a pass that fulfills the requirements
//#ifdef DEBUG_PASSING
#ifdef DEBUG
		// Had an attempted allyoop pass ignored!
		printf("ERROR - %s: CANT FIND A PASSER SEQ\n", ppd->p_spd->szlast);
		printf("ball_in_hand = %s, receiver quad %s, receiver dist %f, vel = %f\n",hand[ppd->ball_in_hand], q8[quad8], dist, fsqrt((pobj->vx * pobj->vx) + (pobj->vz * pobj->vz)));
		while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
#endif
		return;

//		return;
		// Should never see this again!
		// Keep this stuff for release!!!!
		change_anim(&ppd->odata, pass_forward_1x);
		if (!(pobj->flags & PF_ALLYDUNKER))
			player_blocks[receiver].odata.adata[0].animode &= ~MODE_UNINT;
		pgi->ball_mode = BM_PASSING;
		is_bounce_pass = NO;
		return;

		//lockup();
//#endif
	}
	else
	{
		// Debug for passing
//		printf("receiver dist %f\n", dist); 
		// Throw the pass
		pgi->passer       = ppd->plyrnum;
#if NUM_PERTEAM > 2
		pgi->receiver_a   = NO_RECEIVER;
		pgi->receiver_b   = NO_RECEIVER;
#endif
		pbo->who_threw    = ppd->plyrnum;
		pbo->int_receiver = receiver;

		i = select_pass[randrng(1+indx)];
		pass_anim = (char *)((int)passes[i].pass | 0x80000000);

		is_bounce_pass = passes[i].flags & P_BOUNCE ? YES : NO;
		is_bounce_pass = pobj->y > 0.0f ? NO : is_bounce_pass;

#ifdef DEBUG_PASSING
		{
		int y;
		printf("indx = %d, select_pass[%d] = %d\n",indx,indx,i);
//		for(y = 0; y < NUM_PASSES; y++)
//			printf("select_pass[%d] = %d\n",y,select_pass[y]);
		}
#endif
		if ((stop_him || gnd_pass)
			//|| (ppd->odata.vx == 0.0f && ppd->odata.vz == 0.0f)
			//|| ((ppd->odata.vx != 0.0f || ppd->odata.vz != 0.0f) && passes[i].flags & P_NOT_TWOPART)
			|| (pvel <= .5f)
			|| ((pvel != 0.0f) && passes[i].flags & P_NOT_TWOPART)
			|| (ppd->odata.flags & (PF_SHOOTING|PF_DUNKER|PF_ALLYDUNKER))
			|| (passes[i].flags & P_AIR)
			)
		{
			// force pass script to set this bit...
			ppd->odata.flags &= ~PF_BOTH_HANDS;

			if(ppd->odata.flags & (PF_SHOOTING|PF_DUNKER|PF_ALLYDUNKER))
			{
				ppd->odata.flags &= ~PF_PLYRFLAGS;
				ppd->odata.flags |= PF_PASSING;
			}

			change_anim(&ppd->odata, pass_anim);
			if(stop_him)
				ppd->odata.friction = 0.15;
		}
		else
		{
			// do a two part pass...
			if(ppd->odata.adata[0].seq_index != (SEQ_RUN_R)
				&& ppd->odata.adata[0].seq_index != (SEQ_RUN_L)
				)
			{
				change_anim(&ppd->odata, run_anim);
#ifdef DEBUG
				if(pobj->y > 0.0f)
				{
				int y;

					printf("indx = %d, select_pass[%d] = %d\n",indx,indx,i);
					printf("air_time = %d, gnd_pass = %s, stop_him = %s, vx = %3.3f vz = %3.3f\n",
						air_time, gnd_pass == YES ? "YES" : "NO", stop_him == YES ? "YES" : "NO", pobj->vx, pobj->vz);
					printf("He's got running legs -- IN THE AIR ! ! !\n");
//					while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
				}
#endif
			}
			// force pass script to set this bit...
			ppd->odata.flags &= ~PF_BOTH_HANDS;

			begin_twopart_mode(&ppd->odata);
			change_anim2(&ppd->odata, pass_anim);
		}
		pgi->ball_mode = BM_PASSING;
	}

	// DETERMINE IF RECEIVER SHOULD BE STOPPED
	pobj = &player_blocks[receiver].odata;

	if (!(player_blocks[receiver].odata.flags & PF_ALLYDUNKER) &&
		!(player_blocks[receiver].odata.adata[0].animode & MODE_REACTING))
	{
		player_blocks[receiver].odata.adata[0].animode &= ~MODE_UNINT;
	}

// RULES: determine if the pass can be stolen or not
	pbo->flags |= BF_NOINTERCEPT;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void face_receiver(obj_3d *pobj)
{
	bbplyr_data * ppdata = (bbplyr_data *)pobj;
	obj_3d * rec = (obj_3d *)(player_blocks + ball_obj.int_receiver);
	float dist, c_time, p[3];
	int nfwd;

	if(rec->vx == 0.0f && rec->vz == 0.0f)
		pobj->tgt_fwd = p2p_angles[ppdata->plyrnum][ball_obj.int_receiver];
	else
	{
	// get time to receiver
		dist = distxz[ppdata->plyrnum][ball_obj.int_receiver];

		c_time = (MAX_PASS_TIME * (dist / MAX_PASS_DIST));
		c_time = GREATER(c_time, MIN_PASS_TIME);
	
		p[1] = 0.0f;
		plyr_will_be_at(&nfwd, &p[0], &p[2], (int)c_time, ball_obj.int_receiver);
		pobj->tgt_fwd = ptangle( &pobj->x, p);
	}
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void face_teammate(obj_3d *pobj)
{
	bbplyr_data *ppdata = (bbplyr_data *)pobj;

	pobj->tgt_fwd = p2p_angles[ppdata->plyrnum][ppdata->plyrnum ^ 1];
}

//-----------------------------------------------------------------------------------------------------------------------------
extern float bbx,bby,bbz;
//-----------------------------------------------------------------------------------------------------------------------------
void AllyPassRelease(obj_3d *pobj)
{
	register bbplyr_data * passer_3d = (bbplyr_data *)pobj;
	register game_info_t * pgi = &game_info;
	register ball_data * pbo = &ball_obj;


	// reset hotspot stuff
	passer_3d->launched_from_hotspot = 0;

	if(passer_3d != (player_blocks + pgi->ball_handler))
	{
#ifdef DEBUG
		fprintf(stderr,"--> passer_3d/pobj mismatch in AllyPassRelease()\r\n");
		printf("%s is the passer\n", selected_players[player_blocks[pgi->ball_handler].plyrnum].player_data->szlast);
		printf("%s should be the passer\n", passer_3d->p_spd->szlast);
		//lockup();
#endif
		return;
	}

	// Clr passer flags	since he no longer has possession
	passer_3d->dribble_mode =  DM_CAN_DRIBBLE;
	passer_3d->odata.flags &= ~(PF_PASSING|PF_SHOOTING|PF_DUNKER|PF_ALLYDUNKER|PF_LAYUP);

	if(pgi->ball_handler != NO_BALLHANDLER)
	{
		//register bbplyr_data * catcher_3d = player_blocks + pbo->int_receiver;

		// Throw the pass
		pgi->ball_handler = NO_BALLHANDLER;
		pgi->ball_mode    = BM_PASSING;
#if NUM_PERTEAM > 2
		pgi->receiver_a   = NO_RECEIVER;
		pgi->receiver_b   = NO_RECEIVER;
#endif
		pbo->flags &= ~BF_BOUNCEPASS;		// don't do a bounce pass

		SetAllyPassVels(&passer_3d->odata);

		pass_to_speech(&passer_3d->odata, pbo->int_receiver);

		// Lock-out passer from grabbing the ball back
		passer_3d->no_ball_collide = 15;

		passer_3d = player_blocks;
		if(!pgi->off_side) passer_3d += NUM_PERTEAM;
#if NUM_PERTEAM > 2
		passer_3d[0].no_ball_collide =
		passer_3d[1].no_ball_collide =
		passer_3d[2].no_ball_collide = pbo->total_time / 2;
#else
		passer_3d[0].no_ball_collide =
		passer_3d[1].no_ball_collide = pbo->total_time / 2;
#endif
	}
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void PassRelease(obj_3d *pobj, int bouncepass, int both_hands)
{
	register bbplyr_data * passer_3d = (bbplyr_data *)pobj;
	register game_info_t * pgi = &game_info;
	register ball_data * pbo = &ball_obj;
	int i;

	// reset hotspot stuff
	passer_3d->launched_from_hotspot = 0;

	if (passer_3d != (player_blocks + pgi->ball_handler))
	{
#ifdef DEBUG
		printf("--> passer_3d/pobj mismatch in PassRelease()\r\n");
		printf("%s is the passer\n", selected_players[player_blocks[pgi->ball_handler].plyrnum].player_data->szlast);
		printf("%s should be the passer\n", passer_3d->p_spd->szlast);
		//lockup();
#endif
		return;
	}

	if(pbo->int_receiver != -1)
	{
		if(player_blocks[pbo->int_receiver].no_ball_collide > 0)
		{
#ifdef DEBUG
			printf("RECEIVER: %s has no_ball_collide set. %d\n",
				selected_players[pbo->int_receiver].player_data->szlast,
				player_blocks[pbo->int_receiver].no_ball_collide);
#endif
		
			player_blocks[pbo->int_receiver].no_ball_collide = 0;
		}
	}

	// Clr passer flags	since he no longer has possession
	passer_3d->dribble_mode =  DM_CAN_DRIBBLE;
	passer_3d->odata.flags &= ~(PF_PLYRFLAGS);

//	balltrail = 1;

	i = pgi->ball_handler;

	if(pgi->ball_handler != NO_BALLHANDLER)
	{
		register bbplyr_data * catcher_3d = player_blocks + pbo->int_receiver;

		pass_to_speech(&passer_3d->odata, pbo->int_receiver);

		if(bouncepass == 1 && is_bounce_pass == YES)
			pbo->flags |= BF_BOUNCEPASS;
		else
			pbo->flags &= ~BF_BOUNCEPASS;

		pgi->ball_handler = NO_BALLHANDLER;
		pgi->ball_mode    = BM_PASSING;
#if NUM_PERTEAM > 2
		pgi->receiver_a   = NO_RECEIVER;
		pgi->receiver_b   = NO_RECEIVER;
#endif

		set_ball_vels(&passer_3d->odata, &catcher_3d->odata, both_hands);

		// Chk if on-fire
		if (pgi->fire_cnt >= FIRECNT)
		{
			if (i == pgi->fire_player)
				ball_fireset (0, pbo->total_time, 15);
		}
		else if (game_info.team_fire == player_blocks[i].team)
				ball_fireset (1, pbo->total_time, 15);


		// Tell receiver how long he should wait
		catcher_3d->receive_ticks_left = pbo->total_time;

		// force catch script to set this bit
		catcher_3d->odata.flags &= ~PF_BOTH_HANDS;

		// Lock-out passer from grabbing the ball back
		passer_3d->no_ball_collide = 15;

		passer_3d = player_blocks;
		if (!pgi->off_side) passer_3d += NUM_PERTEAM;
		passer_3d[0].no_ball_collide =
		passer_3d[1].no_ball_collide = 7;
#if NUM_PERTEAM > 2
		passer_3d[2].no_ball_collide = 7;
#endif

		for(i = 0; i < NUM_PLAYERS; i++)
			head_wait[i] = 0;
	}
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------

typedef struct _AllyOop
{
	void *ally;
	int seq1, seq2, frame, time;
} AllyOop;

AllyOop ally_data[] =
{
	{&allyoop_x, SEQ_K_1HND_R, SEQ_K_1HND_L, 18, 25},
	{&allyoop_2, SEQ_K_QUIK_6, SEQ_K_QUIK_6, 15, 20},
};

int which_ally = 0;
//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void SetAllyPassVels(obj_3d *pobj)
{
	obj_3d * dunker = (obj_3d *)(player_blocks + ball_obj.int_receiver);
	float time, temp_t, tick_wait;
	float sx, sz, offx, offy, offz,rx,ry,rz,p[3];
	float vx, vy, vz, tx, ty, tz, ax, ay, az, frame;
	int i,angle;

	// check to see if receiver is already in a dunk seq...
	if(pobj == dunker)
	{
		ally_player  = -1;
		ally_jump_wait = -1;

		//temp_t = 20.0f;
		//frame = ally_data[which_ally].frame;
		temp_t = ally_data[which_ally].time;
		frame = (int)(dunker->adata[0].fframe + (dunker->adata[0].sinc * temp_t)) % (dunker->adata[0].pahdr->nframes - 1);

#ifdef DEBUG
		printf("frame %3.3f temp_t = %3.3f for %s\n",frame, temp_t, ((bbplyr_data *)pobj)->p_spd->szlast);
#endif

	//get_joint_position(
	//	dunker,
	//	ally_data[i].seq,
	//	ally_data[i].frame,
	//	ally_data[i].limb,
	//	dunker->tgt_fwd, &offx, &offy, &offz);
		get_joint_position(
			dunker,
			-1,						//ally_data[i].seq,
			(int)frame,	//ally_data[i].frame,
			((bbplyr_data *)dunker)->ball_in_hand == RIGHT_HAND ? JOINT_RWRIST : JOINT_LWRIST,
			dunker->tgt_fwd,
			&offx, &offy, &offz);

		ax = dunker->x + offx;
		ay = dunker->y + offy;
		az = dunker->z + offz;

		tx = ax + temp_t * dunker->vx;
		ty = ay + temp_t * dunker->vy + (0.5f * GRAVITY * temp_t * temp_t);
		tz = az + temp_t * dunker->vz;

		if( fabs(ty - RIM_Y) >= 3.0f && ty < RIM_Y)
		{
#ifdef DEBUG
			printf("adjusting ally oop y to RIM\n");
#endif
			ty = RIM_Y + 2.5f;
		}

		ball_obj.odata.vx = (tx - ball_obj.odata.x) / temp_t;
		ball_obj.odata.vy = (ty - ball_obj.odata.y) / temp_t - (0.5f * BALL_GRAVITY * temp_t);
		ball_obj.odata.vz = (tz - ball_obj.odata.z) / temp_t;

		ball_obj.flight_time = 0;
		ball_obj.total_time  = (int)temp_t;

#ifdef DEBUG
		printf("%s: ALLYOOP OFF BACKBOARD\n",player_blocks[ball_obj.int_receiver].p_spd->szlast);
#endif
		if(pobj->flags & PF_ALLYDUNKER)
			pobj->flags = (pobj->flags & ~PF_ALLYDUNKER) | PF_DUNKER;
	}
	else if(dunker->flags & PF_ALLYDUNKER && gDouble_dunk == YES)
	{
	int ftime = ((bbplyr_data *)dunker)->total_time - ((bbplyr_data *)dunker)->flight_time;

		ally_player  = -1;
		ally_jump_wait = -1;

		if( ftime - 5 > 5)
			temp_t = ftime - 5;
		else
		{
			temp_t = 80;
#ifdef DEBUG
			printf("%s: SHOULD do a LAYUP here because of DOUBLE DUNK time constraints\n",((bbplyr_data *)pobj)->p_spd->szlast);
//			while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
#endif
		}
		frame = (int)(dunker->adata[0].fframe + (dunker->adata[0].sinc * temp_t)) % (dunker->adata[0].pahdr->nframes - 1);

#ifdef DEBUG
		printf("frame %3.3f temp_t = %3.3f for %s\n",frame, temp_t, ((bbplyr_data *)pobj)->p_spd->szlast);
#endif

		get_joint_position(dunker, -1, (int)frame, ((bbplyr_data *)dunker)->ball_in_hand == RIGHT_HAND ? JOINT_RWRIST : JOINT_LWRIST, dunker->tgt_fwd, &offx, &offy, &offz);

		ax = dunker->x + offx;
		ay = dunker->y + offy;
		az = dunker->z + offz;

		tx = ax + temp_t * dunker->vx;
		ty = ay + temp_t * dunker->vy + (0.5f * GRAVITY * temp_t * temp_t);
		tz = az + temp_t * dunker->vz;

		if( fabs(ty - RIM_Y) >= 3.0f && ty < RIM_Y)
		{
#ifdef DEBUG
			printf("adjusting double dunk Y to RIM_Y\n");
#endif
			ty = RIM_Y + 2.5f;
		}

		ball_obj.odata.vx = (tx - ball_obj.odata.x) / temp_t;
		ball_obj.odata.vy = (ty - ball_obj.odata.y) / temp_t - (0.5f * BALL_GRAVITY * temp_t);
		ball_obj.odata.vz = (tz - ball_obj.odata.z) / temp_t;

		ball_obj.flight_time = 0;
		ball_obj.total_time  = (int)temp_t;
	}
	else if(dunker->flags & PF_ALLYDUNKER && gDouble_dunk == NO)
	{
	// find the seq the dunker is in...
		//for(i = 0; i < (sizeof(ally_data) / sizeof(AllyOop)) && dunker->adata[0].seq_index != ally_data[i].seq; i++) ;
		//printf("i = %d\n",i);

		ally_player  = -1;
		ally_jump_wait = -1;
		//get_joint_position(dunker, ally_data[i].seq, ally_data[i].frame, ally_data[i].limb, dunker->tgt_fwd, &offx, &offy, &offz);

// ball's travel time gets capped
		temp_t = (MAX_PASS_TIME * (distxz[((bbplyr_data*)pobj)->plyrnum][ball_obj.int_receiver] / MAX_PASS_DIST));
		temp_t = GREATER(temp_t, MIN_PASS_TIME);

		frame = (int)(dunker->adata[0].fframe + (dunker->adata[0].sinc * temp_t)) % (dunker->adata[0].pahdr->nframes - 1);

#ifdef DEBUG
		printf("frame %3.3f temp_t = %3.3f for %s\n",frame, temp_t, ((bbplyr_data *)pobj)->p_spd->szlast);
#endif

		get_joint_position(dunker, -1, (int)frame, ((bbplyr_data *)dunker)->ball_in_hand == RIGHT_HAND ? JOINT_RWRIST : JOINT_LWRIST, dunker->tgt_fwd, &offx, &offy, &offz);

		ax = dunker->x + offx;
		ay = dunker->y + offy;
		az = dunker->z + offz;

		tx = ax + temp_t * dunker->vx;
		ty = ay + temp_t * dunker->vy + (0.5f * GRAVITY * temp_t * temp_t);
		tz = az + temp_t * dunker->vz;

		if( fabs(ty - RIM_Y) >= 3.0f && ty < RIM_Y)
		{
#ifdef DEBUG
			printf("adjusting ally oop y to RIM\n");
#endif
			ty = RIM_Y + 2.5f;
		}

		ball_obj.odata.vx = (tx - ball_obj.odata.x) / temp_t;
		ball_obj.odata.vy = (ty - ball_obj.odata.y) / temp_t - (0.5f * BALL_GRAVITY * temp_t);
		ball_obj.odata.vz = (tz - ball_obj.odata.z) / temp_t;

		ball_obj.flight_time = 0;
		ball_obj.total_time  = (int)temp_t;
	}
	else
	{	// throw a traditional ally oop, which allyoop to show

		ally_player    = ball_obj.int_receiver;

		//i = 4 + ((bbplyr_data*)dunker)->ball_in_hand;
		//time = (float)ally_data[i].tick_length;
		//tick_wait = (float)ally_data[i].tick_wait + 4.0f;/*8 feet planting time*/
		//ally_jump_wait = ally_data[i].tick_wait;

		//i = ((bbplyr_data*)dunker)->ball_in_hand;
		time = player_blocks[ally_player].total_time - player_blocks[ally_player].flight_time;

		//tick_wait = (float)ally_data[i].tick_wait + 4.0f;

		ally_jump_wait = 0;

//		if(game_info.active_goal == 0)
//			p[0] = -RIM_CX; p[1] = 0.0f; p[2] = 0.0f;
//		else
//			p[0] = RIM_CX; p[1] = 0.0f; p[2] = 0.0f;

		angle = ptangle(&dunker->x, p);

		get_joint_position(dunker,
							player_blocks[ally_player].odata.adata[0].seq_index,
							player_blocks[ally_player].odata.adata[0].iframe + time-10,		// 15 = pass tick count
							((((bbplyr_data*)dunker)->ball_in_hand == RIGHT_HAND) ? JOINT_RWRIST : JOINT_LWRIST),
							angle,
							&offx,
							&offy,
							&offz);
		//get_joint_position(dunker, ally_data[i].seq, ally_data[i].frame, ally_data[i].limb, angle, &offx, &offy, &offz);

		// dunkers' starting point
//		sx = dunker->x + (dunker->vx * ally_data[i].tick_wait);
//		sz = dunker->z + (dunker->vz * ally_data[i].tick_wait);
		sx = dunker->x + dunker->vx;
		sz = dunker->z + dunker->vz;

		// start jump from where he's going to be
		ax = sx + offx;
		ay = dunker->y + offy;
		az = sz + offz;

		// dunker's vel to get to rim
		if(game_info.active_goal == 0)
		{
			p[0] = -RIM_CX; p[1] = 0.0f; p[2] = 0.0f;
			rx = -RIM_CX + ((RIM_RADOUT) * isin( ptangle(&dunker->x, p) - 0 ) );
		}
		else
		{
			p[0] = RIM_CX; p[1] = 0.0f; p[2] = 0.0f;
			rx =  RIM_CX + ((RIM_RADOUT) * isin( ptangle(&dunker->x, p) - 0 ) );
		}
		ry = RIM_Y + 2.0f;
		rz = ((RIM_RADOUT) * icos(ptangle(&pobj->x, p) - 0) );
		bbx = rx; bby = ry; bbz = rz;

#ifdef DEBUG
		printf("rx = %3.3f, rz = %3.3f\n",rx,rz);
#endif

		// time = 60
		vx = (rx - ax) / time;
		vy = (ry - ay) / time - (0.5f * GRAVITY * time);
		vz = (rz - az) / time;

		// where are the player's hands in tick_catch ticks?
		temp_t = time;
//		temp_t = (time - (float)ally_data[i].tick_catch) + ally_data[i].tick_wait;
		tx = ax + temp_t * vx;
		ty = ay + temp_t * vy + (0.5f * GRAVITY * temp_t * temp_t);
		tz = az + temp_t * vz;

		bbx = tx; bby = ty; bbz = tz;

//		ball_obj.odata.vx = (tx - ball_obj.odata.x) / (tick_wait + temp_t);
//		ball_obj.odata.vy = (ty - ball_obj.odata.y) / (tick_wait + temp_t) - (0.5f * BALL_GRAVITY * (tick_wait + temp_t));
//		ball_obj.odata.vz = (tz - ball_obj.odata.z) / (tick_wait + temp_t);
		ball_obj.odata.vx = (tx - ball_obj.odata.x) / (0 + temp_t);
		ball_obj.odata.vy = (ty - ball_obj.odata.y) / (0 + temp_t) - (0.5f * BALL_GRAVITY * (0 + temp_t));
		ball_obj.odata.vz = (tz - ball_obj.odata.z) / (0 + temp_t);

		ball_obj.flight_time = 0;
		ball_obj.total_time  = (int)temp_t;// 8 is feet planting time
//		ball_obj.total_time  = ally_data[i].tick_wait + (int)temp_t + 4;// 8 is feet planting time

		dunker->adata[0].animode |= MODE_UNINT;
	}
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
#define PASS_RELEASE_TIME		12.0f
int PlayerInAllyZone(obj_3d *pobj)
{
	float dist_to_rim;

	if(((bbplyr_data *)pobj)->team ^ game_info.off_goal)	// on a side view, the goal to the right, End view the far away goal
	{
		if(pobj->x == 0.0f && pobj->z == 0.0f)
		{
			dist_to_rim = fsqrt( ((RIM_X - pobj->x) * (RIM_X - pobj->x)) + ((0.0f - pobj->z) * (0.0f - pobj->z)) );
			if( ((pobj->x >= 71.3864f) && (fabs(pobj->z) <= 42.214f)) || (dist_to_rim <= 47.0f) ) // in the paint
				return YES;
			else
				return NO;
		}
		else
		{
			dist_to_rim = fsqrt( ((RIM_X - (pobj->x + pobj->vx * PASS_RELEASE_TIME)) * (RIM_X - (pobj->x + pobj->vx * PASS_RELEASE_TIME)))
													+ ((0.0f - (pobj->z + pobj->vz * PASS_RELEASE_TIME)) * (0.0f - (pobj->z + pobj->vz * PASS_RELEASE_TIME))) );
			if( (((pobj->x + pobj->vx * PASS_RELEASE_TIME) >= 71.3864f) && (fabs(pobj->z + pobj->vz * PASS_RELEASE_TIME) <= 42.214f)) || (dist_to_rim <= 47.0f) ) // in the paint
				return YES;
			else
				return NO;
		}
	}
	else
	{
		if(pobj->x == 0.0f && pobj->z == 0.0f)
		{
			dist_to_rim = fsqrt( ((-RIM_X - pobj->x) * (-RIM_X - pobj->x)) + ((0.0f - pobj->z) * (0.0f - pobj->z)) );
			if( (in_the_paint( ((bbplyr_data *)pobj), YES) == YES) || (dist_to_rim <= 47.0f) ) // in the paint
				return YES;
			else
				return NO;
		}
		else
		{
			dist_to_rim = fsqrt( ((-RIM_X - (pobj->x + pobj->vx * PASS_RELEASE_TIME)) * (-RIM_X - (pobj->x + pobj->vx * PASS_RELEASE_TIME)))
													+ ((0.0f - (pobj->z + pobj->vz * PASS_RELEASE_TIME)) * (0.0f - (pobj->z + pobj->vz * PASS_RELEASE_TIME))) );
			if( (((pobj->x + pobj->vx * PASS_RELEASE_TIME) <= -71.3864f) && (fabs(pobj->z + pobj->vz * PASS_RELEASE_TIME) <= 42.214f)) || (dist_to_rim <= 47.0f) ) // in the paint
				return YES;
			else
				return NO;
		}	
	}	
	return NO;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void swat_tipoff_ball(obj_3d *pobj)
{
	register bbplyr_data * swater = (bbplyr_data *)pobj;
	register game_info_t * pgi = &game_info;
	register ball_data * pbo = &ball_obj;
	register bbplyr_data * catcher = player_blocks + (swater->plyrnum^1);
	int	i;
	//register bbplyr_data * catcher_3d = player_blocks + pbo->int_receiver;

	// Clr passer flags	since he no longer has possession
	swater->dribble_mode =  DM_CAN_DRIBBLE;
	swater->odata.flags &= ~(PF_PASSING|PF_SHOOTING|PF_DUNKER|PF_ALLYDUNKER|PF_LAYUP);

	pgi->ball_handler = NO_BALLHANDLER;
	pbo->who_threw    = swater->plyrnum;
	pbo->int_receiver = (swater->plyrnum^1);

	game_info.ball_mode = BM_PASSING;

	set_ball_vels(&swater->odata, &catcher->odata, NO);

	// Tell receiver how long he should wait
	catcher->receive_ticks_left = pbo->total_time;

	// Lock-out passer from grabbing the ball back
	swater->no_ball_collide = 15;

	// swat ball sound
	snd_scall_bank(gameplay_bnk_str,STEAL1_SND,VOLUME6,127,PRIORITY_LVL3);

	// clear head looking aroung logic
	for(i = 0; i < NUM_PLAYERS; i++)
		head_wait[i] = 0;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
//void ChangePassReceiver(void)
//{
//int team = player_blocks[game_info.receiver_a].team;
//
//	game_info.receiver_a++;
//	game_info.receiver_a = ((game_info.receiver_a + 1) % NUM_PERTEAM) + (NUM_PERTEAM * team);
//
//	if(game_info.ball_handler == game_info.receiver_a)
//	{
//		game_info.receiver_a++;
//		game_info.receiver_a = ((game_info.receiver_a + 1) % NUM_PERTEAM) + (NUM_PERTEAM * team);
//	}
//}

//////////////////////////////////////////////////////////////////////////////
// ANI_CODE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void get_backboard_ally( obj_3d *pobj)
{
int ally;

	which_ally = randrng(sizeof(ally_data) / sizeof(AllyOop));
	change_anim(pobj, (char *)((int)ally_data[which_ally].ally | 0x80000000));
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
int plyr_rand( obj_3d *pobj, int arg )
{
	return( rand() % arg );
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void dampen_yvel(obj_3d * pobj, int flag)
{
//	return;

//	pobj->vy = 0.21f;
	if(flag == 1)
	{
		pobj->vy /= 2.0f;
	}
//		pobj->vx *= 0.20f;
//		pobj->vz *= 0.20f;
//	}
//	else if(clear_xz_vels == 1)
//	{
//		pobj->vx = pobj->vz = 0.0f;
//	}
//	else if(clear_xz_vels == 3)


	if(flag == 3)
	{	// THIS IS FOR IN AIR PASSES...
//#if 0
//		if(pobj->vy < 0.5f)
//			pobj->vy = 1.0f;
//		else if(pobj->vy >= 0.5f)
#ifdef DEBUG
		printf("Passer - x,y,z vels, y: %2.2f %2.2f %2.2f %2.2f\n",pobj->vx,pobj->vy,pobj->vz,pobj->y);
#endif
		if(pobj->y == 0.0f)
			pobj->vy = 1.00f;
		else if((pobj->y <= 12.5f) && (pobj->vy <= 0.0f))
		{
			pobj->vy = 0.21f;
#ifdef DEBUG
			printf("adjusting Passer - x,y,z vels, y: %2.2f %2.2f %2.2f %2.2f\n",pobj->vx,pobj->vy,pobj->vz,pobj->y);
#endif
		}
//#else
		//if(pobj->vy < 0.0f)
		//	pobj->vy *= 0.95f;
		//else
//			pobj->vy *= 1.5f;
		//else if(pobj->vy >= 0.5f)
		//	pobj->vy *= 1.0;
//#endif
//		pobj->vx *= 0.35f;
//		pobj->vz *= 0.35f;
	}
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
int maybe_fadeshot(obj_3d *pobj)
{
int rim_bearing, new_dir;
float p[3];

	p[0] = game_info.active_goal ? RIM_X : -RIM_X; p[1] = 0.0f; p[2] = 0.0f;

	if(((bbplyr_data *)pobj)->stick_cur != 24)
	{
		new_dir = 1023 & (dir49_table[((bbplyr_data *)pobj)->stick_cur] + (int)RD2GR(cambot.theta));

		rim_bearing = (new_dir - ptangle( &(pobj->x), p)) & 1023;

		if(rim_bearing >= (512 - FADE_ANGLE_TOLERANCE) && rim_bearing <= (512 + FADE_ANGLE_TOLERANCE))
			return YES;
		else
			return NO;
	}
	else
		return NO;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
int maybe_hookshot_right(obj_3d *pobj)
{
	if((pobj->fwd >= 484 && pobj->fwd <= 540 && pobj->x < 0.0f)
		|| ((pobj->fwd >= 996 || pobj->fwd <= 28) && pobj->x > 0.0f))
		return YES;
	else
		return NO;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void maybe_taunt( obj_3d *pobj, int type_of_taunt )
{
	//bbplyr_data *ppdata = (bbplyr_data *)pobj;

	return;

	if(type_of_taunt == 0)
	{	// after a dunk
		if((randrng(100) < 100))
		{
			// have player go into a taunt
			pobj->flags &= ~PF_PLYRFLAGS;
			change_anim(pobj, after_dunk_taunts[randrng(sizeof(after_dunk_taunts)/sizeof(char *))]);
		}
	}
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
int plyr_behind_backboard( obj_3d *pobj )
{
	bbplyr_data *ppdata = (bbplyr_data *)pobj;
	float px,pz;

	px = abs(pobj->x);
	pz = abs(pobj->z);

	if ((px > RIM_X+9.0f) && (pz <= 13.5f))
		return YES;
	else
		return NO;
}

//------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
int plyr_in_front_of_me( obj_3d *pobj )
{
	bbplyr_data *ppdata = (bbplyr_data *)pobj;
	int cp = nearest_to_ball(!ppdata->team);

	if(plyr_bearing(ppdata->plyrnum, cp) <= 96
		&& plyr_bearing(cp, ppdata->plyrnum) <= 128
		&& distxz[ppdata->plyrnum][cp] <= 40.0f)
			return YES;
		else
			return NO;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
int get_angle_to_rim(bbplyr_data *ppdata)
{
float p[3];

	//p[0] = ((ppdata->team ^ game_info.off_goal) == 1) ? RIM_CX : -RIM_CX;
	p[0] = game_info.active_goal ? RIM_CX : -RIM_CX;
	p[1] = p[2] = 0.0f;
	
	return ptangle(&ppdata->odata.x, p);
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
float get_dist_to_rim(bbplyr_data *ppd)
{
float p[3];

	//p[0] = ((ppd->team ^ game_info.off_goal) == 1) ? RIM_CX : -RIM_CX;
	p[0] = game_info.active_goal ? RIM_CX : -RIM_CX;
	p[1] = p[2] = 0.0f;
	
	return fsqrt( ((p[0] - ppd->odata.x) * (p[0] - ppd->odata.x)) + ((p[2] - ppd->odata.z) * (p[2] - ppd->odata.z)) );
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
int defender_near_me( obj_3d *pobj )
{
	bbplyr_data *ppdata = (bbplyr_data *)pobj;
	int cp = nearest_to_ball(!ppdata->team);

	if ((ppdata->plyrnum == game_info.ball_handler) &&
		(distxz[ppdata->plyrnum][cp] <= 40.0f))
		return YES;
	else
		return NO;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
int plyr_right_of_me( obj_3d *pobj, int plyr )
{
	bbplyr_data *ppdata = (bbplyr_data *)pobj;
	int plyrnum = ppdata->plyrnum, plyr_bear;
	
	if(plyr == -1)
		plyr = plyrnum ^ 1;

	plyr_bear = (pobj->tgt_fwd - p2p_angles[plyrnum][plyr]) & 1023;

	if(plyr_bear >= 512)
		return YES;
	else
		return NO;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
int ball_in_righthand(obj_3d *pobj)
{
	if( ((bbplyr_data *)(pobj))->ball_in_hand == LEFT_HAND)
		return NO;
	else
		return YES;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
int plyr_is_righthanded(obj_3d *pobj)
{
	if( ((bbplyr_data *)(pobj))->handedness == RIGHT_HAND)
		return YES;
	else
		return NO;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
int hand_closet_to_ball(obj_3d *pobj)
{
	float *tm, hx, hy, hz, lw_dist, rw_dist;
	int		limb;
	bbplyr_data *ppdata = (bbplyr_data *)pobj;

	// get dist of LEFT wrist to ball...
	tm = cambot.xform;
	limb = JOINT_LWRIST;
	hx = ppdata->jpos[limb][0] * tm[0] + ppdata->jpos[limb][1] * tm[4] + ppdata->jpos[limb][2] * tm[ 8] + cambot.x;
	hy = ppdata->jpos[limb][0] * tm[1] + ppdata->jpos[limb][1] * tm[5] + ppdata->jpos[limb][2] * tm[ 9] + cambot.y;
	hz = ppdata->jpos[limb][0] * tm[2] + ppdata->jpos[limb][1] * tm[6] + ppdata->jpos[limb][2] * tm[10] + cambot.z;
	lw_dist = fsqrt(((ball_obj.odata.x - hx) * (ball_obj.odata.x - hx)) + ((ball_obj.odata.z - hz) * (ball_obj.odata.z - hz)));

	limb = JOINT_RWRIST;
	hx = ppdata->jpos[limb][0] * tm[0] + ppdata->jpos[limb][1] * tm[4] + ppdata->jpos[limb][2] * tm[ 8] + cambot.x;
	hy = ppdata->jpos[limb][0] * tm[1] + ppdata->jpos[limb][1] * tm[5] + ppdata->jpos[limb][2] * tm[ 9] + cambot.y;
	hz = ppdata->jpos[limb][0] * tm[2] + ppdata->jpos[limb][1] * tm[6] + ppdata->jpos[limb][2] * tm[10] + cambot.z;
	rw_dist = fsqrt(((ball_obj.odata.x - hx) * (ball_obj.odata.x - hx)) + ((ball_obj.odata.z - hz) * (ball_obj.odata.z - hz)));

	if(rw_dist < lw_dist)
	{
		ppdata->ball_in_hand = RIGHT_HAND;
		return YES;				// use right hand to swipe at ball
	}
	else
	{
		ppdata->ball_in_hand = LEFT_HAND;
		return NO;				// use left hand to swipe at ball
	}
}

//////////////////////////////////////////////////////////////////////////////
void rotate_to_face_closet(obj_3d *pobj, int max_angle)
{
	bbplyr_data		*ppdata = (bbplyr_data *)pobj;
	bbplyr_data		*pplyr;
	int				i,pnum,brng;
	float			plyr_dist,x,z,dist;
	obj_3d 			*o1;
	obj_3d 			*o2;

//	if (angle !=0)
//	{
//		pobj->fwd2 = pobj->fwd;
//		pobj->tgt_fwd2 = pobj->fwd;
//		return 0;
//	}

	pnum = -1;


	plyr_dist = 1000;
	// find closet player on opposing team...and rotate torso toward him
	for (i=0; i < NUM_PLAYERS; i++)
	{
		// dont check myself
		if (ppdata->plyrnum == i)
			continue;

		pplyr = &player_blocks[i];

		// dont check man IM dragging
//		if ((pobj->flags & PF_DRAGGING_PLR) &&
//			((ppdata->puppet_link == pplyr) && (pplyr->puppet_link == ppdata)))
//			continue;

		// ignore if on same team
		if (ppdata->team == pplyr->team)
			continue;

		// ignore players reacting to moves.
		if (pplyr->odata.adata[0].animode & MODE_REACTING)
			continue;

		// Instead of just checking distance... take into account where the
		// opponent will be in 7 ticks (according to stiff_arm_anim)

		// o1 is plyr, o2 is i
		o1 = &player_blocks[ppdata->plyrnum].odata;
		o2 = &player_blocks[i].odata;
		x = o2->x + (o2->vx * 7.0f);
		z = o2->z + (o2->vz * 7.0f);
		dist = fsqrt((o2->x - o1->x) * (o2->x - o1->x) + (o2->z - o1->z) * (o2->z - o1->z));
		if (dist < plyr_dist)

//		// get distance of opponent to me
//		if (distxz[ppdata->plyrnum][i]/YD2UN < plyr_dist)
		{
//			plyr_dist = distxz[ppdata->plyrnum][i]/YD2UN;
			plyr_dist = dist;
			pnum = i;
		}

	}

	// If guy is 5 yards behind or in front, allow it
//	if (FIELDX(pobj->x)-3 <= FIELDX(player_blocks[pnum].odata.x))
	{
		brng = (1024 + pobj->fwd - p2p_angles[ppdata->plyrnum][pnum]) % 1024;
		brng = 1024 - brng;

//		// test for maybe doing the head plow move
//		if ((flag) &&
//			(randrng(100) < 35) &&
//			((game_info.off_side ? -pobj->vx : pobj->vx) > 0.0))
//		{
//			if (brng <= 64)
//				return 1;
//			if (brng >= 960)
//				return 1;
//		}

		// stiff arm
		if ((brng >= 192) && (brng <= 512))
			brng = 192;
		if ((brng >= 513) && (brng <= 832))
			brng = 832;
		pobj->fwd2 += brng + 13.0f;			// add fudge
		pobj->tgt_fwd2 = pobj->fwd2;
	}
//	return 0;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
int maybe_pumpfake(obj_3d *pobj, int but)
{
	bbplyr_data *ppdata = (bbplyr_data *)pobj;
	int min = (game_info.game_time & 0x00FF0000)>> 16;
	int sec = (game_info.game_time & 0x0000FF00) >> 8;

	// dont allow a pump fake in last few seconds of period...
	if(min == 0 && sec < 4)
	{
		return YES;
	}

	switch(but)
	{
		case 0:
			but = P_A;
			break;
		case 1:
			but = P_B;
			break;
		case 2:
			but = P_C;
			break;
		case 3:
			but = P_D;
			break;
		default:
#ifdef DEBUG
			printf("ERROR: plyr_is_holding_button - but = %d\n",but);
#endif
			return NO;
			break;
	}

	if (ppdata->but_cur & but)
		return YES;
	else
		return NO;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
int plyr_is_holding_but(obj_3d *pobj, int but)
{
	switch(but)
	{
		case 0:
			but = P_A;
			break;
		case 1:
			but = P_B;
			break;
		case 2:
			but = P_C;
			break;
		case 3:
			but = P_D;
			break;
		default:
#ifdef DEBUG
			printf("ERROR: plyr_is_holding_button - but = %d\n",but);
#endif
			return NO;
			break;
	}

	if( ((bbplyr_data *)(pobj))->but_cur & but)
		return YES;
	else
		return NO;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
int non_bh_plyr_is_holding_but(obj_3d *pobj, int but)
{
	bbplyr_data *ppdata = (bbplyr_data *)pobj;
	
	if (ppdata->plyrnum == game_info.ball_handler)
		return NO;

	switch(but)
	{
		case 0:
			but = P_A;
			break;
		case 1:
			but = P_B;
			break;
		case 2:
			but = P_C;
			break;
		case 3:
			but = P_D;
			break;
		default:
#ifdef DEBUG
			printf("ERROR: plyr_is_holding_button - but = %d\n",but);
#endif
			return NO;
			break;
	}

	if( ((bbplyr_data *)(pobj))->but_cur & but)
		return YES;
	else
		return NO;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void set_ball_hand( obj_3d *pobj, int hand )
{

((bbplyr_data *)pobj)->ball_in_hand = hand;

}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void shake_defender(obj_3d *pobj, int way)
{
	bbplyr_data *ppdata = (bbplyr_data *)pobj;
	int cp = nearest_to_ball(!ppdata->team);

	// determine what defender gets the punishment!!!
	//if(way == 3)
	if(ISDRONE(&player_blocks[cp]))
	{
		if(plyr_bearing(ppdata->plyrnum, cp) <= 40
		&& plyr_bearing(cp, ppdata->plyrnum) <= 40
		&& distxz[((bbplyr_data *)pobj)->plyrnum][cp] <= 40.0f)
//	&& my offense is better than your defense
		{
			if(pobj->y == 0.0f && pobj->vy == 0.0f)
			{
				//if(randrng(100) <= 50)
					change_anim((obj_3d *)(player_blocks + cp), dfake1_butt_anim);
					shake_defender_speech();
				//else
					//change_anim((obj_3d *)(player_blocks + cp), way == 0 ? dfake1_r_anim : dfake1_l_anim);
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void SetShotVels( obj_3d *pobj, int fade)
{
	int angle, time;
	float a,b,c,b2m4ac,root1,root2, p[3];
	bbplyr_data *ppdata = (bbplyr_data *)pobj;
	float hsx,hsz;


	//FIX!!! don't these wanna be RIM_X,Y? do they?
	p[0] = game_info.active_goal ? 116.406f : -116.406f; p[1] = 32.0f; p[2] = 0.0f;
	angle = ptangle( &pobj->x, p);

	crowd_cheer_time = 60*4;

	if(
	(pobj->adata[0].seq_index == (SEQ_J_RUN_R) || pobj->adata[0].seq_index == (SEQ_J_RUN_L))
	|| (pobj->adata[0].seq_index == (SEQ_J_RUN2_R) || pobj->adata[0].seq_index == (SEQ_J_RUN2_L))
	)
	{
		//pobj->vy = 1.6f;
		pobj->vy = 1.15f;
	}
	else //if((pobj->adata[0].seq_index >= (SEQ_J_SHT1_R) && pobj->adata[0].seq_index <= (SEQ_J_SHT2_L)))
	{
		//pobj->vy = 1.5f;
		pobj->vy = 1.15f;
	}

	if(pobj->flags & PF_TURBO)
		pobj->vy *= 1.10f;

	if(fade == 1)
	{
		pobj->vx = 0.54f * (float)(isin(angle - 512)); 
		pobj->vz = 0.54f * (float)(icos(angle - 512)); 
	}

	// fade == 2 is for (SEQ_J_HVE1_R|SEQ_J_HVE1_L)
	else if(fade == 2)
	{
		pobj->vy = 1.10f;
		pobj->vx = 0.54f * (float)(isin(angle)); 
		pobj->vz = 0.54f * (float)(icos(angle)); 
	}

	else if(fade == 3)
	{
		pobj->vy = 1.10f;
		p[0] = game_info.active_goal ? -RIM_X : RIM_X; p[1] = 0.0f; p[2] = 0.0f;	// invert cause want to jump from underneath rim
		angle = ptangle( &pobj->x, p);
		pobj->vx = 0.60f * (float)(isin(angle)); 
		pobj->vz = 0.60f * (float)(icos(angle)); 
	}

	game_info.shooter = ((bbplyr_data *)pobj)->plyrnum;

	a = GRAVITY / 2.0f;
	b = pobj->vy;
	c = 0.0f;
	b2m4ac = b * b - 4.0f * a * c;
	root1 = (-1.0f * b + fsqrt( b2m4ac )) / (2.0f * a);
	root2 = (-1.0f * b - fsqrt( b2m4ac )) / (2.0f * a);
	time = 1;
	if (root1 > 1.0f)
		time = (int)(root1+0.9f);

	if (root2 > 1.0f)
		time = (int)(root2+0.9f);

	((bbplyr_data *)pobj)->flight_time = 0;
	((bbplyr_data *)pobj)->total_time = time;
	//printf("b2m4ac = %f, root1 = %f, root2 = %f\n",b2m4ac, root1, root2);

	// HOTSPOT logic
	ppdata->launched_from_hotspot = 0;

	hsx = (hotspot_xzs[ppdata->p_spd->hotspot][0] * (ppdata->team ? 1 : -1));
	hsz = hotspot_xzs[ppdata->p_spd->hotspot][1];
	hsx -= ppdata->odata.x;
	hsz -= ppdata->odata.z;

	if ((hsx*hsx + hsz*hsz) < (HOTSPOT_RADIUS*HOTSPOT_RADIUS))
//	if (plyr_launched_from_hotspot(game_info.shooter) == YES) 
	{
		ppdata->launched_from_hotspot = 1;
		if (game_info.hotspot_made_cnt[game_info.shooter] >= HOTSPOT_ACTIVE_CNT)
//		if ((game_info.hotspot_made_cnt[game_info.shooter] == 0) || (game_info.hotspot_made_cnt[game_info.shooter] >= HOTSPOT_ACTIVE_CNT))
		{
			set_process_run_level (iqcreate ("htsptfir", PLAYER_PID, hotspot_fire_proc, 50, game_info.shooter, 0, 0), RL_BALL+6);
		}
		snd_scall_bank(gameplay_bnk_str,135,VOLUME7,127,PRIORITY_LVL3);		// hotspot particle sound effect
	}
}

//////////////////////////////////////////////////////////////////////////////
//
// misc player functions
//
//////////////////////////////////////////////////////////////////////////////
void neutral_stick_general_run( obj_3d *pobj )
{
// this code checks to see if you have been neutral the past 2 ticks and if so makes the last tick 1 so
// change anim can work
bbplyr_data *ppdata = (bbplyr_data *)pobj;

	if(ppdata->stick_cur == 24 && (ppdata->stick_old-24) == 0)
		ppdata->stick_old = 1;

	if(ppdata->team == game_info.off_side)
		offense_run( pobj );
	else
		defense_run( pobj );
}

static char *stand_dribble_anims[] =
{
	dribble_stand_cocky,
	dribble_snippet_2,
	dribble_snippet_2,
	dribble_snippet_2,
	dribble_snippet_2,
	dribble_stand_6,
	dribble_snippet_1,
	dribble_stand_2,		// FIX!!  Would use this only on weak dribble ratings
//	dribble_stand_1,
//	dribble_spider_tween
};

//------------------------------------------------------------------------------------------------------------------------------
#define OOB_STOP_RANGE 160
//------------------------------------------------------------------------------------------------------------------------------
int del_buf[10] = {0,0,0,0,0,0,0,0,0,0}, del_indx = 0;
float last_vel[4] = {-1.0f, -1.0f, -1.0f, -1.0f};
void offense_run(obj_3d *pobj)
{
	static int run_seqs[] = {
		SEQ_D_RUN2_R, SEQ_D_RUN2_R, SEQ_D_RUN2_L, SEQ_D_RUN2_L,
		SEQ_D_WLK1_R, SEQ_D_WLK1_R, SEQ_D_WLK1_L, SEQ_D_WLK1_L,
		SEQ_D_COCK_1, SEQ_D_COCK_1, SEQ_D_COCK_1, SEQ_D_COCK_1,
		SEQ_D_COCK_2, SEQ_D_COCK_2, SEQ_D_COCK_2, SEQ_D_COCK_2,
		SEQ_D_REV1, 	SEQ_D_REV1, 	SEQ_D_REV1, 	SEQ_D_REV1,
		SEQ_D_BKN2_R, SEQ_D_BKN2_R, SEQ_D_BKN2_L, SEQ_D_BKN2_L,
		SEQ_D_SID1_R, SEQ_D_SID1_R, SEQ_D_SID1_L, SEQ_D_SID1_L,

// run variations
		SEQ_RUN_R, SEQ_RUN_R,	SEQ_RUN_L, SEQ_RUN_L,	//SEQ_TRUN,
		SEQ_JOG_1, SEQ_JOG_1, SEQ_JOG_1, SEQ_JOG_1,
		SEQ_CKY_RUN1, SEQ_CKY_RUN1, SEQ_CKY_RUN1, SEQ_CKY_RUN1,
		SEQ_JOG_1, SEQ_JOG_1,	SEQ_JOG_1, SEQ_JOG_1,	// SEQ_BOXR_RUN, SEQ_BOXR_RUN,
		SEQ_JOG_1, SEQ_JOG_1,	SEQ_JOG_1, SEQ_JOG_1,//		SEQ_WALK, SEQ_WALK, SEQ_WALK, SEQ_WALK,
		SEQ_AFTER_1, SEQ_AFTER_1,	SEQ_AFTER_1, SEQ_AFTER_1,
	};
	static char *run_scripts[] = {
		dribble_run_2, dribble_run_2, dribble_run_2, dribble_run_2,
		dribble_walk_1, dribble_walk_1, dribble_walk_1, dribble_walk_1,
		dribble_run_cocky, dribble_run_cocky, dribble_run_cocky, dribble_run_cocky,
		dribble_run_cocky_2, dribble_run_cocky_2, dribble_run_cocky_2, dribble_run_cocky_2,
		dribble_run_cocky_rev, dribble_run_cocky_rev, dribble_run_cocky_rev, dribble_run_cocky_rev,
		dribble_backin_2, dribble_backin_2, dribble_backin_2, dribble_backin_2,
		dribble_run_right, dribble_run_right, dribble_run_left, dribble_run_left,

// run variations
		run_anim, run_anim, run_anim, run_anim,	//run_animt,
		jog_1_anim, jog_1_anim, jog_1_anim, jog_1_anim,
		cocky_run_1_anim, cocky_run_1_anim, cocky_run_1_anim, cocky_run_1_anim,
		boxer_run_legs_1_anim, boxer_run_legs_1_anim, boxer_run_legs_1_anim, boxer_run_legs_1_anim,
		walk_1_anim, walk_1_anim, walk_1_anim, walk_1_anim,
		jog_2_anim, jog_2_anim, jog_2_anim, jog_2_anim,
	};
	static char *int_run_scripts[] = {
		dribble_run_2_int, dribble_run_2_int, dribble_run_2_int, dribble_run_2_int,
		dribble_walk_1_int, dribble_walk_1_int, dribble_walk_1_int, dribble_walk_1_int,
		dribble_run_cocky, dribble_run_cocky, dribble_run_cocky, dribble_run_cocky,
		dribble_run_cocky_2, dribble_run_cocky_2, dribble_run_cocky_2, dribble_run_cocky_2,
		dribble_run_cocky_rev, dribble_run_cocky_rev, dribble_run_cocky_rev, dribble_run_cocky_rev,
		dribble_backin_2, dribble_backin_2, dribble_backin_2, dribble_backin_2,
		dribble_run_right, dribble_run_right, dribble_run_left, dribble_run_left,

// run variations
		run_anim, run_anim, run_anim, run_anim,	//run_animt,
		jog_1_anim, jog_1_anim, jog_1_anim, jog_1_anim,
		cocky_run_1_anim, cocky_run_1_anim, cocky_run_1_anim, cocky_run_1_anim,
		boxer_run_legs_1_anim, boxer_run_legs_1_anim, boxer_run_legs_1_anim, boxer_run_legs_1_anim,
		walk_1_anim, walk_1_anim, walk_1_anim, walk_1_anim,
		jog_2_anim, jog_2_anim, jog_2_anim, jog_2_anim,
	};
	bbplyr_data * ppdata = (bbplyr_data *)pobj;
	float rim_pt[3], speedup = 0.0f, anim_speed = 0.68f, vel;
	int defender_bearing, rim_bearing, team_winning;
	int seq_index = 0, new_dir, dir_delta = 0, ldir_delta = 0,
		ndir_delta = 0, dribble_variation = 0, run_variation = 0, ticks;
	int coming_back_on_screen, rev_dir;
	float		f,dist_to_rim;

	if(pobj->flags & PF_SHOOTING)
	{
		//printf("in standard run and shooting!!!\n");
		return;
	}

	rim_pt[0] = game_info.active_goal ? RIM_X : -RIM_X; rim_pt[1] = 0.0f; rim_pt[2] = 0.0f;
	rim_bearing = (pobj->fwd - ptangle( &(pobj->x), rim_pt)) & 1023;

	// check to see if player should be doing any variation runs...
	if(pobj->plyrmode == MODE_OFFENSE && ppdata->plyrnum == game_info.sc_scored)
		game_info.sc_scored = -1;

#if 1
	if(game_info.game_mode == GM_INBOUNDING)
	{
	// was I the player who just scored...
		if(pobj->plyrmode == MODE_DEFENSE
		&& ppdata->plyrnum == game_info.sc_scored)
		{
			if(allow_cocky_oob == YES)
			{
				// check out the cocky run
				if(pobj->adata[0].seq_index == (SEQ_CKY_RUN1))
					run_variation = 2;
				else	// check out the boxer anim
				if(pobj->adata[0].seq_index == (SEQ_JOG_1) && (pobj->adata[1].animode & MODE_TWOPART))
					run_variation = 4;
				else	// check out the cool jog anim
				if(pobj->adata[0].seq_index == (SEQ_JOG_1))
					run_variation = 1;
				else	// check out the walk anim
				if(pobj->adata[0].seq_index == (SEQ_WALK))
					run_variation = 3;
				else	// check out the celebration run
				if(pobj->adata[0].seq_index == (SEQ_AFTER_1))
					run_variation = 5;
				else
				{
				// randomize between the `cocky run', `boxer', `cool jog'
					run_variation = 1 + randrng(3);
					if(randrng(100) <= 35)
						run_variation = 5;	// just the celebration run
				}
			}
			else
				run_variation = 0;	// standard runs
		}
	}
#endif

	// this is here for a player receiving a pass and is headed OOB.
	if((pobj->adata[0].animode & MODE_UNINT) 
	&& ((pobj->vx != 0.0f || pobj->vz != 0.0f) && pobj->y == 0.0f)
	&& ppdata->plyrnum == ball_obj.int_receiver
	&& ppdata->plyrnum != game_info.ball_handler)
	{
		ppdata->stick_cur = dir2stick(pobj->fwd);
	}

	if(ppdata->stick_cur != 24)
	{
		if(pobj->flags & PF_CELEBRATE)
		{
			pobj->flags &= ~PF_CELEBRATE;
			//printf("CELEBRATE cleared.\n");
		}

		//----- Do OOB Correcting -----
		// stick is down.  set vels & facing
		new_dir = 1023 & (dir49_table[ppdata->stick_cur] + (int)RD2GR(cambot.theta));

		if(pobj->plyrmode == MODE_OUT_OF_BOUNDS && ppdata->auto_control == NO)
			if(ABS(new_dir - (256 + (512 * !game_info.active_goal))) <= OOB_STOP_RANGE)
				new_dir = (256 + (512 * !game_info.active_goal)) +
									((OOB_STOP_RANGE - (new_dir >= 256 && new_dir <= 768) *
									(OOB_STOP_RANGE * 2)) * (-1 + !game_info.active_goal * 2));

		// this is here for a player receiving a pass and is headed OOB.
		if((pobj->adata[0].animode & MODE_UNINT) 
		&& ((pobj->vx != 0.0f || pobj->vz != 0.0f) && pobj->y == 0.0f)
		&& ppdata->plyrnum == ball_obj.int_receiver
		&& ppdata->plyrnum != game_info.ball_handler)
		{
			new_dir = pobj->fwd;
		}

		if(new_dir != pobj->fwd)
		{
			if ((randrng(100) == 30) && (ppdata->dribble_mode >= DM_CAN_DRIBBLE))
				shoe_squeak(pobj);
		}

		if(ppdata->plyrnum != OOBPasser)
		// keep the player inside the invisible wall...
			oob_correction(pobj, &new_dir);

	//___________________________

#ifdef DEBUG
		if(pobj->flags & PF_SHOOTING)
		{
			printf("in standard run and shooting!!!\n");
		}
#endif

		if(ppdata->dribble_mode >= DM_CAN_DRIBBLE)
		{
			rim_bearing = (new_dir - ptangle( &(pobj->x), rim_pt)) & 1023;
			rim_bearing = (rim_bearing > 512) ? 1024 - rim_bearing : rim_bearing;

			if(pobj->adata[0].animode & MODE_END
			&& pobj->adata[0].seq_index == (SEQ_D_REV1)
			&& game_info.ball_handler == ppdata->plyrnum)
			{
				dribble_variation = 0;
				ticks_since_cocky = -COCKY_DRIBBLE_TWAIT, cocky_dribbler = NO_BALLHANDLER;
			}

		// CHECK FOR COCKY DRIBBLING...
			if(ppdata->plyrnum == game_info.ball_handler && (ppdata->plyrnum == cocky_dribbler || cocky_dribbler == NO_BALLHANDLER))
			{
				if(pobj->adata[0].seq_index != (SEQ_D_COCK_1) && pobj->adata[0].seq_index != (SEQ_D_COCK_2))
					ticks_since_cocky = LESSER(300, ticks_since_cocky + 1);

				cocky_dribbler = ppdata->plyrnum;

				//if(rim_bearing <= 256)
				if(rim_bearing <= 500)
				{
					if(pobj->adata[0].seq_index == (SEQ_D_COCK_1))
						dribble_variation = 2;
					else if(pobj->adata[0].seq_index == (SEQ_D_COCK_2))
						dribble_variation = 3;
					else
					{
						//if(distxz[ppdata->plyrnum][nearest_to_me(ppdata->plyrnum, !ppdata->team)] <= 100.0f)
						//{
							if(plyr_crossed_halfcourt(ppdata->plyrnum)
							|| (!plyr_crossed_halfcourt(ppdata->plyrnum) && distxz[ppdata->plyrnum][nearest_to_me(ppdata->plyrnum, !ppdata->team)] >= 25.0f))
							{
								if(ticks_since_cocky >= 180)
								{
									if(randrng(100) <= 35)
									{
										if((ppdata->_dribbling >= 12)
										|| (ppdata->_dribbling < 12 && randrng(100) <= 3))
											dribble_variation = 2 + randrng(2);
									}
									//else
									//	ticks_since_cocky = -COCKY_DRIBBLE_TWAIT;	
								}
							}
						//}
					}
				}
				else
				{	// don't allow this move for at least a few seconds...
					if(pobj->adata[0].seq_index == (SEQ_D_COCK_1) || pobj->adata[0].seq_index == (SEQ_D_COCK_2))
						ticks_since_cocky = -COCKY_DRIBBLE_TWAIT, cocky_dribbler = NO_BALLHANDLER;
				}

			// CHECK FOR REVERSE DRIBBLE...
				rim_bearing = (pobj->fwd - ptangle( &(pobj->x), rim_pt)) & 1023;
				rev_dir = 1023 & (new_dir + 512);
				if( ABS(rev_dir - get_angle_to_rim(ppdata)) <= 96
				&& ((rim_bearing >= 0 && rim_bearing <= 100) || (rim_bearing >= 924 && rim_bearing <= 1024))
				&& (dribble_variation == 0 || dribble_variation == 5 || dribble_variation == 2 || dribble_variation == 3)
				//&& pobj->adata[0].seq_index != (SEQ_D_REV1)
				&& ticks_since_cocky >= 120
				&& plyr_crossed_halfcourt(ppdata->plyrnum) == YES)
				{
					dribble_variation = 4;
				}

			}

			ppdata->dribble_mode = DM_DRIBBLING;	// player is dribbling
			if( ppdata->stick_cur != 24 )
			{
		// check for an abrupt change of direction
				dir_delta = (new_dir - pobj->fwd) & 1023;
				if(ldir_delta != dir_delta)
				{
					ldir_delta = dir_delta;
					ndir_delta = dir_delta;
				}

				if(ppdata->plyrnum == game_info.ball_handler)
				{
					del_buf[(del_indx % 10)] = dir_delta; del_indx++; del_indx %= 1000;
				}

#if 0
		set_text_font(FONTID_BAST7T);
		delete_multiple_strings(0xbeef, 0xFFFFFFFF);
		set_string_id(0xbeef);
		set_text_position(30, 300, 2.0f);
		oprintf("%dj%dc%3d %3d %3d %3d %3d %3d %3d %3d %3d %3d", (HJUST_LEFT|VJUST_CENTER), WHITE,
			del_buf[0],del_buf[1],del_buf[2],del_buf[3],del_buf[4],del_buf[5],del_buf[6],del_buf[7],del_buf[8],del_buf[9]);
		set_text_position(30, 312, 2.0f);
		oprintf("%dj%dcdir_delta %d new dir %d fwd %d", (HJUST_LEFT|VJUST_CENTER), WHITE, ndir_delta, new_dir, pobj->tgt_fwd);
#endif

				pobj->tgt_fwd = new_dir;
				pobj->tgt_fwd2 = new_dir;

				if( ppdata->plyrnum != game_info.ball_handler)
					//seq_index = 28 + (run_variation * 2);
					seq_index = 28 + (ppdata->ball_in_hand * 2) + (run_variation * 4);
				else
					seq_index = (ppdata->ball_in_hand * 2) + (dribble_variation * 4);

				if( pobj->flags & PF_TURBO )
					seq_index++;

					if(pobj->adata[0].seq_index == (SEQ_D_BKN1_R) || pobj->adata[0].seq_index == (SEQ_D_BKN1_L))
					{
						pobj->vx = FRELX( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? 0.55f : 0.4f)), pobj->tgt_fwd );
						pobj->vz = FRELZ( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? 0.55f : 0.4f)), pobj->tgt_fwd );
					}
					else
					if(pobj->adata[0].seq_index == (SEQ_D_WLK1_R) || pobj->adata[0].seq_index == (SEQ_D_WLK1_L))
					{
						pobj->vx = FRELX( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? 0.6f : 0.5f)), pobj->tgt_fwd );
						pobj->vz = FRELZ( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? 0.6f : 0.5f)), pobj->tgt_fwd );
					}
					else
					if(pobj->adata[0].seq_index == (SEQ_AFTER_1) || pobj->adata[0].seq_index == (SEQ_AFTER_1))
					{
						pobj->vx = FRELX( 0.0f, ppdata->_speed * 0.55f, pobj->tgt_fwd );
						pobj->vz = FRELZ( 0.0f, ppdata->_speed * 0.55f, pobj->tgt_fwd );
					}
					else
					if(pobj->adata[0].seq_index == (SEQ_JOG_1) || pobj->adata[0].seq_index == (SEQ_JOG_1))
					{
						pobj->vx = FRELX( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? 0.45f : 0.45f)), pobj->tgt_fwd );
						pobj->vz = FRELZ( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? 0.45f : 0.45f)), pobj->tgt_fwd );
					}
					else
					if(pobj->adata[0].seq_index == (SEQ_D_REV1) || pobj->adata[0].seq_index == (SEQ_D_REV1))
					{
						pobj->vx = FRELX( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? 1.0f : 1.0f)), pobj->tgt_fwd );
						pobj->vz = FRELZ( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? 1.0f : 1.0f)), pobj->tgt_fwd );
					}
					//else
					//if(pobj->adata[0].seq_index == (SEQ_CKY_RUN1) || pobj->adata[0].seq_index == (SEQ_CKY_RUN1))
					//{
					//	pobj->vx = FRELX( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? 1.0f : 1.0f)), pobj->tgt_fwd );
					//	pobj->vz = FRELZ( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? 1.0f : 1.0f)), pobj->tgt_fwd );
					//}
					else
					if(pobj->adata[0].seq_index == (SEQ_WALK) || pobj->adata[0].seq_index == (SEQ_WALK))
					{
						pobj->vx = FRELX( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? 0.35f : 0.35f)), pobj->tgt_fwd );
						pobj->vz = FRELZ( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? 0.35f : 0.35f)), pobj->tgt_fwd );
					}
					else
					if((game_info.ball_mode == BM_PASSING) && (ppdata->plyrnum == ball_obj.int_receiver))
					{
						// Don't change my velocity in middle of a leading pass!
//						printf("RECEIVER VELS 1 - x,y,z vels: %2.2f %2.2f %2.2f\n",pobj->vx,pobj->vy,pobj->vz);
//						pobj->vx = FRELX( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? 1.0f : 1.0f)), pobj->tgt_fwd );
//						pobj->vz = FRELZ( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? 1.0f : 1.0f)), pobj->tgt_fwd );
//						printf("RECEIVER VELS 2 - x,y,z vels: %2.2f %2.2f %2.2f\n",pobj->vx,pobj->vy,pobj->vz);
					}
					// Am I the intended receiver of an inbounds pass? If so, don't allow turbo run as he sprints downcourt
					else if((game_info.game_mode == GM_INBOUNDING) && (pobj->plyrmode == MODE_OUT_OF_BOUNDS))
					{
						// Print guys last name
//						printf("%s \n", ppdata->p_spd->szlast);
//						printf("Sprinting down court slow!\n");
//						pobj->vx = FRELX( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? 1.00f : 0.90f)), pobj->tgt_fwd );
//						pobj->vz = FRELZ( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? 1.00f : 0.90f)), pobj->tgt_fwd );
						pobj->vx = FRELX( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? 1.00f : 1.00f)), pobj->tgt_fwd );
						pobj->vz = FRELZ( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? 1.00f : 1.00f)), pobj->tgt_fwd );
					}
					else
					{
						pobj->vx = FRELX( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? TURBO_BOOST : 1.0f)), pobj->tgt_fwd );
						pobj->vz = FRELZ( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? TURBO_BOOST : 1.0f)), pobj->tgt_fwd );
					}

			// Speed UP these dudes when they're offscreen when game_mode = GM_INBOUNDING
				if(game_info.game_mode == GM_INBOUNDING
				//&& pobj->plyrmode == MODE_DEFENSE
				&& ppdata->auto_control == YES
				&& !(pobj->flags & PF_ONSCREEN))
				{
					pobj->vx *= 3.0f;
				 	pobj->vz *= 3.0f;
				}

// FIX!!!!!!
{
	int _dir, stick_dir;

	if(
	//(game_info.game_mode == GM_INBOUNDING && game_info.ball_mode != BM_PASSING)
	//&& (pobj->plyrmode == MODE_OUT_OF_BOUNDS || pobj->plyrmode == MODE_OFFENSE)
	(game_info.ball_mode != BM_PASSING && ppdata->stick_cur != 24 && pobj->y == 0.0f))
	{
		_dir = 1023 & (int)((atan2(pobj->vx, pobj->vz) / M_PI) * 512.0f);
		stick_dir = 1023 & (dir49_table[ppdata->stick_cur] + (int)RD2GR(cambot.theta));
		if(_dir != stick_dir && ABS(_dir - stick_dir) > 1)
		{
#ifdef DEBUG
//			if(ISDRONE(ppdata))
//			{
//				printf("ERROR - OFFENSE_RUN: %s is running WRONG -- stick:%d, s_cur %d, vel_angle %d, fwd %d, t_fwd %d\n",
//					ppdata->p_spd->szlast, stick_dir, ppdata->stick_cur, _dir, pobj->fwd, pobj->tgt_fwd);
//				printf("THIS ERROR IS BEING CORRECTED\n");
//				while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
//			}
#endif
			pobj->vx = FRELX( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? TURBO_BOOST : 1.0f)), pobj->tgt_fwd);//stick_dir );
			pobj->vz = FRELZ( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? TURBO_BOOST : 1.0f)), pobj->tgt_fwd);//stick_dir );
		}
	}

// catches a guy who is in a run animation with a low velocity and with the stick centered!
	if(ppdata->stick_cur == 24 && pobj->y == 0.0f)
	{
		_dir = 1023 & (int)((atan2(pobj->vx, pobj->vz) / M_PI) * 512.0f);
		stick_dir = pobj->tgt_fwd;
		if(_dir != stick_dir && ABS(_dir - stick_dir) > 1)
		{
#ifdef DEBUG
			if(ISDRONE(ppdata))
			{
				printf("***** ERROR - OFFENSE_RUN STICK 24: %s is running WRONG -- stick:%d, s_cur %d, vel_angle %d, fwd %d, t_fwd %d\n",
					ppdata->p_spd->szlast, stick_dir, ppdata->stick_cur, _dir, pobj->fwd, pobj->tgt_fwd);
				printf("PLAYER CAN'T RUN SO FORCING A PASS\n");
			}
#endif
		// put player in a default state where he can't dribble and he has to pass the ball
			pobj->vx = pobj->vz = 0.0f;
			pobj->flags &= ~PF_PLYRFLAGS;
			ppdata->dribble_mode = DM_CANT_DRIBBLE;
			game_info.passer       = NO_PASSER;
			game_info.ball_mode    = BM_DRIBBLING;
		}
	}
}

//#ifndef DEBUG_NO_SPEED_LOGIC
//				if((game_info.ball_mode != BM_PASSING) && (ppdata->plyrnum != ball_obj.int_receiver))
//				{
//				
//					// any offender offscreen runs way faster...
//					if(!(pobj->flags & PF_ONSCREEN))
//					{
//						speedup += base_speed * .3f;
//						//speedup += base_speed * .2f;
//					}
//
//					// offender gets another speed boost if coming towards screen while offscreen...
//					if ((cambot.x > pobj->x && pobj->tgt_fwd < 512 && pobj->tgt_fwd > 0) ||
//						(cambot.x < pobj->x && pobj->tgt_fwd > 512 && pobj->tgt_fwd < 1020))
//						coming_back_on_screen = YES;
//					else
//						coming_back_on_screen = NO;
//
//					if (!(pobj->flags & PF_ONSCREEN) && coming_back_on_screen == YES)
//						speedup += base_speed * .6f;
//
//					pobj->vx += FRELX( 0.0f, speedup, pobj->tgt_fwd );
//					pobj->vz += FRELZ( 0.0f, speedup, pobj->tgt_fwd );
//				}
//#endif

				f = ppdata->drone_vel;
				if (f) {
					pobj->vx = FRELX( 0.0f, f, pobj->tgt_fwd );
					pobj->vz = FRELZ( 0.0f, f, pobj->tgt_fwd );
				}

				team_winning = NO;
				if(compute_score(ppdata->team) > compute_score(!ppdata->team))
					team_winning = YES;

				if(dribble_variation <= 0)
				{
					if( !((pobj->flags & (PF_CATCHING|PF_PASSING)) || (game_info.ball_mode == BM_PASSING && ppdata->plyrnum == ball_obj.int_receiver)) )
					{

//			printf("Doing velocity........speedups\n");

					// player is trying to waste time...trying to cheat...
						if(game_info.ball_handler == ppdata->plyrnum
//						&& (((game_info.shotclock & 0x0000FF00)>>8) <= 9)	//9
						&& plyr_crossed_halfcourt(ppdata->plyrnum) == NO
						&& team_winning == YES)
						{
							if((((game_info.game_time & 0x0000FF00)>>8) <= 50
							&& ((game_info.shotclock & 0x0000FF00)>>8) <= 12))	//9
								speedup -= base_speed * .15f;		//.08
							else if(((game_info.shotclock & 0x0000FF00)>>8) <= 9)
								speedup -= base_speed * .10f;		//.08
							//printf("slowing down cheater\n");
						}


#ifndef DEBUG_NO_SPEED_LOGIC
					// if game_time is under 50 secs speed up losing players...
						if( ((game_info.game_time & 0x0000FF00)>>8) <= 50
						&& team_winning == NO)
						{
							speedup += base_speed * .15f;
//#ifdef DEBUG
//							printf("speeding up losing players on offense\n");
//#endif
						}

						// Slow down team with ball
						if(game_info.ball_handler == ppdata->plyrnum || game_info.ball_handler == (ppdata->plyrnum ^ 1))
						{
							speedup -= base_speed * .12f;	// 15
						}

						// Slow down ball owner even more!
						if(game_info.ball_handler == ppdata->plyrnum)
						{
							speedup -= base_speed * .08f;	// 10
						}

						// add 12% boost for player fire!
						if ((game_info.fire_player == ppdata->plyrnum && game_info.fire_cnt >= FIRECNT) || (game_info.team_fire == ppdata->team))
						{
							speedup += base_speed * .12f;
						}

						//printf("plyr %d, offensive speedup %f\n", ppdata->plyrnum, speedup);
						pobj->vx += FRELX( 0.0f, speedup, pobj->tgt_fwd );
						pobj->vz += FRELZ( 0.0f, speedup, pobj->tgt_fwd );
#endif
					}
				}

			// has to happen only for this dribble sequence...the reverse direction dribble
				if(dribble_variation == 4 && game_info.ball_handler == ppdata->plyrnum)
				{
					pobj->tgt_fwd = (new_dir + 512) & 1023;
					pobj->tgt_fwd2 = (new_dir + 512) & 1023;
				}

				// this is here for a player receiving a pass and is headed OOB.
				if((pobj->adata[0].animode & MODE_UNINT) 
				&& (pobj->vx != 0.0f || pobj->vz != 0.0f)
				&& ppdata->plyrnum == ball_obj.int_receiver
				&& ppdata->plyrnum != game_info.ball_handler)
				{
					return;
				}

				// if a player wants to move after catching the ball clear PF_CATCHING
				if(!(pobj->adata[1].animode & MODE_TWOPART)
				&& game_info.ball_mode == BM_DRIBBLING
				&& pobj->flags & PF_CATCHING)
					pobj->flags &= ~PF_CATCHING;

// special change of direction
#if 0
				if(ppdata->plyrnum == game_info.ball_handler
				&& ISDRONE(ppdata)
				//&& ppdata->stick_offcenter_time >= 60
				&& !(pobj->adata[1].animode & MODE_TWOPART)
				&& (ABS(dir_delta) >= 384 && ABS(dir_delta) <= 640)
				&& randrng(100) <= 20	)
				{
					change_anim((obj_3d *)ppdata, dribble_turn180_1 );
					//change_anim((obj_3d *)ppdata, ppdata->ball_in_hand == RIGHT_HAND ? dribble_turn180_r_anim : dribble_turn180_l_anim);
					pobj->tgt_fwd = pobj->fwd;
					return;
				}
#endif
#if 0
				if(ppdata->plyrnum == game_info.ball_handler
				&& ppdata->stick_offcenter_time >= 60
				&& !(pobj->adata[1].animode & MODE_TWOPART)
				&& (
					 (ppdata->ball_in_hand == RIGHT_HAND && del_buf[((del_indx + 7)%10)] > 0 && del_buf[((del_indx + 8)%10)] > 0 && del_buf[((del_indx + 9)%10)] > 0)
				|| (ppdata->ball_in_hand == LEFT_HAND && del_buf[((del_indx + 7)%10)] < 0 && del_buf[((del_indx + 8)%10)] < 0 && del_buf[((del_indx + 9)%10)] < 0)
					 )
				//&& pobj->tgt_fwd == pobj->fwd
				//&& ABS(dir_delta) >= 100 && ABS(dir_delta) <= 384
				)
				{
					// - = right, + = left
					if(
						//((ppdata->ball_in_hand == RIGHT_HAND && dir_delta > 0) && (dir_delta >= 220 && dir_delta <= 256))
						//|| ((ppdata->ball_in_hand == LEFT_HAND && dir_delta < 0)	&& (dir_delta >= -256 && dir_delta <= -220))
						//((ppdata->ball_in_hand == RIGHT_HAND && dir_delta > 0) && (/*dir_delta >= 220 && dir_delta <= 256*/1))
						//|| ((ppdata->ball_in_hand == LEFT_HAND && dir_delta < 0) && (/*dir_delta >= -256 && dir_deta <= -220*/1))
						1)
					{
						if((rand() % 100) < 75 && distxz[ppdata->plyrnum][nearest_to_me(ppdata->plyrnum, !ppdata->team)] <= 20.0f)
						{
							if(ISDRONE(ppdata))
							{
								if(randrng(100) < 15)
									change_anim((obj_3d *)ppdata, ppdata->ball_in_hand == RIGHT_HAND ? between_legrun_anim_r : between_legrun_anim_l);
							}
							else
								change_anim((obj_3d *)ppdata, ppdata->ball_in_hand == RIGHT_HAND ? between_legrun_anim_r : between_legrun_anim_l);
							//printf("new_dir %d, ->fwd %d, dir_delta %d\n",new_dir, pobj->fwd, dir_delta);
							return;
						}
					}
				}//END.if change of dir	
#endif

#if 0
				if(ppdata->stick_offcenter_time <= 1
				&& ppdata->plyrnum == game_info.ball_handler
				&& ppdata->dribble_mode >= DM_CAN_DRIBBLE)
				{
					// to the right
					if(dir_delta < 0 && (ABS(dir_delta) >= 206 && ABS(dir_delta) <= 306))
					{
						change_anim((obj_3d *)ppdata, ppdata->ball_in_hand == RIGHT_HAND ? dribble_slide_right_r_anim : dribble_slide_right_l_anim);
						pobj->tgt_fwd = pobj->fwd;
						//printf("HELLO: RIGHT\n");
						return;
					}
					// to the left
					if(dir_delta > 0 && (dir_delta >= 206 && dir_delta <= 306))
					{
						change_anim((obj_3d *)ppdata, ppdata->ball_in_hand == RIGHT_HAND ? dribble_slide_left_r_anim : dribble_slide_left_l_anim);
						pobj->tgt_fwd = pobj->fwd;
						//printf("HELLO: LEFT\n");
						return;
					}
				}
#endif
	// if current seq != run_seq...change it
				//defender_bearing = (1024 + player_blocks[ppdata->plyrnum].odata.fwd - p2p_angles[ppdata->plyrnum][nearest_to_me(ppdata->plyrnum, !ppdata->team)]) % 1024;

				if((ppdata->plyrnum == 7)	//game_info.ball_handler)
					&& (plyr_bearing(ppdata->plyrnum, nearest_to_me(ppdata->plyrnum, !ppdata->team)) <= 160)
					//&& ((defender_bearing >= 0 && defender_bearing <= 50) || (defender_bearing >= 974 && defender_bearing <= 1024))
					&& ((rim_bearing >= 0 && rim_bearing <= 200) || (rim_bearing >= 824 && rim_bearing <= 1024))
					&& (distxz[ppdata->plyrnum][nearest_to_me(ppdata->plyrnum, !ppdata->team)] <= 35.0f)
					//&& ((dist3d(&(pobj->x), rim_pt) <= 70.0f))
					&& (ISHUMAN(ppdata) == YES)
				)
				{
					if(pobj->adata[0].seq_index != (SEQ_D_BKN1_R) && pobj->adata[0].seq_index != (SEQ_D_BKN1_L))
					{
						;//change_anim( pobj, dribble_backin_1 );
					}
				}
				else
				{
					if( pobj->adata[0].seq_index != run_seqs[seq_index] )
					{
						if(!(game_info.ball_handler == ppdata->plyrnum && pobj->flags & PF_CATCHING))
						{
		// make sure player isn't running on rim...
							ppdata->HangingOnRim = NO;
							if((pobj->adata[0].animode & MODE_INTXITION)
							//|| (ppdata->plyrnum != game_info.ball_handler && (pobj->adata[0].seq_index >= (SEQ_D_RUN2_R) && pobj->adata[0].seq_index <= (SEQ_D_RUN2_L)))
					    && (!(pobj->adata[0].animode & MODE_ROTONLY))
							)
							{
								//printf("offense_run: MODE_INTXITION is set\n");
								pobj->flags &= ~PF_PLYRFLAGS;
								change_anim( pobj, int_run_scripts[seq_index] );
							// the boxer...
								if(run_variation == 4
								&& game_info.ball_handler != ppdata->plyrnum
								&& !(pobj->adata[1].animode & MODE_TWOPART))
								{
									begin_twopart_mode(pobj);
									change_anim2(pobj, boxer_1_anim);
								}
							}
							else
							{
								if(!(pobj->adata[0].animode & MODE_ROTONLY))
								{
									pobj->flags &= ~PF_PLYRFLAGS;
									change_anim( pobj, run_scripts[seq_index] );
							// the boxer...
									if(run_variation == 4
									&& game_info.ball_handler != ppdata->plyrnum
									&& !(pobj->adata[1].animode & MODE_TWOPART))
									{
										begin_twopart_mode(pobj);
										change_anim2(pobj, boxer_1_anim);
									}
								}
							}
						}
					}
				}
			}	// END.IF stick_cur != 24
		}	// END.IF dribble_mode >= DM_CAN_DRIBBLE
		else
		{	// allow the rotation of player....
			pobj->tgt_fwd = new_dir;
			pobj->tgt_fwd2 = new_dir;
		}
	} // END.IF stick_cur != 24

	// this isn't an else to the above if because the test value can change in
	// the above block and we have to run both cases.
	if(ppdata->stick_cur == 24 && !(pobj->flags & PF_CELEBRATE))
//	if ((ppdata->stick_cur == 24) && !(pobj->flags & PF_CELEBRATE) && (ppdata->stick_old == 1))
	{
	// cocky dribble checks...
		if(ppdata->plyrnum == game_info.ball_handler && (ppdata->plyrnum == cocky_dribbler || cocky_dribbler == NO_BALLHANDLER))
		{
			if(pobj->adata[0].seq_index != (SEQ_D_COCK_1) && pobj->adata[0].seq_index != (SEQ_D_COCK_2))
				ticks_since_cocky = LESSER(300, ticks_since_cocky + 1), cocky_dribbler = NO_BALLHANDLER;
		}

		// make sure dribble seqs begin with SEQ_D_RUN2_R and end with SEQ_D_TBCK_L
		// in order for this next check to work
		if((pobj->adata[0].seq_index >= (SEQ_D_RUN2_R) && pobj->adata[0].seq_index <= (SEQ_D_TBCK_L))
				&& ppdata->plyrnum != game_info.ball_handler)
			ppdata->stick_old = 1;

		// stick is neutral.
		if(( ppdata->stick_old-24 ))
		{
			//pobj->vx = pobj->vz = 0.0f;
			if(game_info.ball_handler == ppdata->plyrnum)
			{	// go into a standstill dribble
				if(ppdata->dribble_mode > DM_CAN_DRIBBLE)	// -=Can't dribble, 0=Can dribble, +=Dribbling
				{
					if(
						 (pobj->adata[0].seq_index != (SEQ_D_SPIDER))
					|| (pobj->adata[0].seq_index != (SEQ_D_TS1_R)) || (pobj->adata[0].seq_index != (SEQ_D_TS1_L))
					|| (pobj->adata[0].seq_index != (SEQ_D_DBB1_R)) || (pobj->adata[0].seq_index != (SEQ_D_DBB1_L))
					|| (pobj->adata[0].seq_index != (SEQ_D_STND_R)) || (pobj->adata[0].seq_index != (SEQ_D_STND_L))
					|| (pobj->adata[0].seq_index != (SEQ_D_STD2_R)) || (pobj->adata[0].seq_index != (SEQ_D_STD2_L))
					|| (pobj->adata[0].seq_index != (SEQ_D_STD6_R)) || (pobj->adata[0].seq_index != (SEQ_D_STD6_L))
					|| (pobj->adata[0].seq_index != (SEQ_D_STD5_R)) || (pobj->adata[0].seq_index != (SEQ_D_STD5_L))
					|| (pobj->adata[0].seq_index != (SEQ_D_SNP1_R)) || (pobj->adata[0].seq_index != (SEQ_D_SNP1_L))
					|| (pobj->adata[0].seq_index != (SEQ_D_SNP2_R)) || (pobj->adata[0].seq_index != (SEQ_D_SNP2_L))
					)
					{
						if(dribble_tweens == YES
						|| (pobj->adata[0].seq_index == (SEQ_D_COCK_1) || pobj->adata[0].seq_index == (SEQ_D_COCK_2))
						)
						{
						float p[3];

						// pick a transition into a standing dribble
							//change_anim(pobj, ppdata->ball_in_hand == RIGHT_HAND ? dribble_intostand_r_1 : dribble_intostand_l_1);
							change_anim(pobj, dribble_to_stop_1 );
							dribble_tweens = NO;
						//fprintf(stderr,"HELLO: 1 dribble tween anims\n");
						}
						else
						{
							if((pobj->adata[0].seq_index != (SEQ_D_BKN1_R) && pobj->adata[0].seq_index != (SEQ_D_BKN1_L))
							&& (pobj->adata[0].seq_index != (SEQ_D_BKN2_R) && pobj->adata[0].seq_index != (SEQ_D_BKN2_L))	)
							{
								if(randrng(100) <= (ISHUMAN(ppdata) ? 70 : 40))
									change_anim( pobj, stand_dribble_anims[rand() % (sizeof(stand_dribble_anims)/sizeof(int))]);
								else
								{
									//pobj->tgt_fwd = get_angle_to_rim(ppdata);
									if(ppdata->_dribbling >= 14)
										change_anim( pobj, dribble_spider);
									else
										change_anim( pobj, stand_dribble_anims[rand() % (sizeof(stand_dribble_anims)/sizeof(int))]);
								}
							}
							else
								change_anim( pobj, dribble_stand_backin );
						//fprintf(stderr,"HELLO: 1 dribble anims\n");
						}
					}
					else
					{
						if((pobj->adata[0].seq_index != (SEQ_D_BKN1_R) && pobj->adata[0].seq_index != (SEQ_D_BKN1_L))
						&& (pobj->adata[0].seq_index != (SEQ_D_BKN2_R) && pobj->adata[0].seq_index != (SEQ_D_BKN2_L))
						)
							change_anim( pobj, stand_dribble_anims[rand() % (sizeof(stand_dribble_anims)/sizeof(int))]);
						else
							change_anim( pobj, dribble_stand_backin );
						//change_anim( pobj, stand_dribble_anims[rand() % (sizeof(stand_dribble_anims)/sizeof(int))]);
						//fprintf(stderr,"HELLO: 2 dribble anims\n");
					}
				}
				else
				{
					// player can't dribble
					if( pobj->adata[0].seq_index < (SEQ_RDY_WB) || pobj->adata[0].seq_index > (SEQ_D_FAKE_L) )
					{
				
						if((pobj->flags & (PF_DUNKER|PF_ALLYDUNKER|PF_LAYUP)))
						{
#ifdef DEBUG
							printf("BUG!!!!!!!  in a dunk, about to go into a triple ready stance!\n");
//							while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
#endif
						}
						else		
							change_anim(pobj, ready_wb_2_anim);
							//change_anim(pobj, ready_wb_anim);
					}
				}
			}
			else
			{	// go into an offensive stance

				dist_to_rim = fsqrt( ((rim_pt[0] - pobj->x) * (rim_pt[0] - pobj->x)) + ((0.0f - pobj->z) * (0.0f - pobj->z)) );
				if((game_info.ball_handler == NO_BALLHANDLER) &&
					(dist_to_rim < 25.0f) &&
					(game_info.ball_mode == BM_SHOOTING) &&
					(game_info.game_mode == GM_IN_PLAY)	)
				{
					change_anim( pobj, randrng(100) < 50 ? wait_rebound_1_anim : wait_rebound_2_anim );
				}
				else
				if((pobj->adata[0].seq_index != (SEQ_SBREATH1))
				|| (pobj->adata[0].seq_index != (SEQ_SBREATH3))
				|| (pobj->adata[0].seq_index != (SEQ_S_CYC_1))
				|| (pobj->adata[0].seq_index != (SEQ_S_CYC_3))
				)
				{
					change_anim(pobj, breath_anims[randrng(sizeof(breath_anims)/sizeof(char *))]);
				}
			}
		}
	} // ENDIF stick_cur == 24

// if player's ppdata->dribble_mode == -1 he can't be dribbling
//	if ((game_info.ball_handler == ppdata->plyrnum) && (ppdata->stick_old == 1))
	if(game_info.ball_handler == ppdata->plyrnum)
	{
		if(ppdata->dribble_mode <= DM_CAN_DRIBBLE)
		{
			if(pobj->y == 0.0f)
			{
				if(!(pobj->flags & (PF_SHOOTING|PF_PASSING|PF_DUNKER|PF_ALLYDUNKER|PF_LAYUP|PF_CATCHING)))
				{
					if( pobj->adata[0].seq_index < (SEQ_RDY_WB) || pobj->adata[0].seq_index > (SEQ_D_FAKE_L) )
					{
						if(!(pobj->adata[1].animode & MODE_TWOPART))
						{
							if(ppdata->stick_cur != 24)
								pobj->fwd = 1023 & (dir49_table[ppdata->stick_cur] + (int)RD2GR(cambot.theta));

							//change_anim(pobj, ready_wb_anim);
							change_anim(pobj, ready_wb_2_anim);
							//printf("2: from catch\n");
							pobj->vx = pobj->vy = pobj->vz = 0.0f;
						}
					}
				}
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
int oob_correction(obj_3d *pobj, int *new_dir)
{
bbplyr_data *ppdata = (bbplyr_data *)pobj;
int change = NO;

	if( pobj->z - (-OUT_OF_BOUNDS_Z) <= 0.7f && *new_dir > 256 && *new_dir < 768)
	{	// top
	int dif = ABS(512 - *new_dir);

		change = YES;
		if(dif <= OOB_STOP_CATCH_RANGE)
		{	// stand still
			pobj->tgt_fwd = *new_dir = 512;
			ppdata->stick_cur = 24;
			pobj->z = -OUT_OF_BOUNDS_Z;

			//printf("oob_corrected: RUNNER STOPPED - TOP\n");
	// receiver stops, faces ball, and waits for ball...
			if(ppdata->plyrnum == ball_obj.int_receiver
			&& game_info.ball_mode == BM_PASSING
			&& ppdata->plyrnum != game_info.ball_handler)
			{
#ifdef DEBUG_CATCHING
				printf("oob_corrected: RECEIVER STOPPED - TOP\n");
#endif
				pobj->tgt_fwd = ptangle(&(pobj->x),&(ball_obj.odata.x));
				pobj->vx = pobj->vz = 0.0f;
				change_anim(pobj, catch_stand_forward_2);
			}
			change++;
		}
		else if( *new_dir < 512)
		{	// *new_dir = 256
			pobj->tgt_fwd = *new_dir = 256;
			ppdata->stick_cur = dir2stick(*new_dir);
			pobj->z = -OUT_OF_BOUNDS_Z;
#ifdef DEBUG_CATCHING
			printf("%s is glancing off TOP WALL, going RIGHT\n",ppdata->p_spd->szlast);
#endif
			pobj->vx = FRELX( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? TURBO_BOOST : 1.0f)), pobj->tgt_fwd );
			pobj->vz = FRELZ( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? TURBO_BOOST : 1.0f)), pobj->tgt_fwd );
		}
		else
		{	// *new_dir = 768
			pobj->tgt_fwd = *new_dir = 768;
			ppdata->stick_cur = dir2stick(*new_dir);
			pobj->z = -OUT_OF_BOUNDS_Z;
#ifdef DEBUG_CATCHING
			printf("%s is glancing off TOP WALL, going LEFT\n",ppdata->p_spd->szlast);
#endif
			pobj->vx = FRELX( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? TURBO_BOOST : 1.0f)), pobj->tgt_fwd );
			pobj->vz = FRELZ( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? TURBO_BOOST : 1.0f)), pobj->tgt_fwd );
		}
	}
	else if( (pobj->z - OUT_OF_BOUNDS_Z) >= -0.7f && ((*new_dir > 768 && *new_dir <= 1023) || (*new_dir >= 0 && *new_dir < 256)) )
	{	// bottom
	int dif = ABS( 512 - ((*new_dir + 512) % 1024) );

		change = YES;
		if(dif <= OOB_STOP_CATCH_RANGE)
		{	// stand still
			pobj->tgt_fwd = *new_dir = 0;
			ppdata->stick_cur = 24;
			pobj->z = OUT_OF_BOUNDS_Z;

			//printf("oob_corrected: RUNNER STOPPED - BOTTOM\n");
	// receiver stops, faces ball, and waits for ball...
			if(ppdata->plyrnum == ball_obj.int_receiver
			&& game_info.ball_mode == BM_PASSING
			&& ppdata->plyrnum != game_info.ball_handler)
			{
#ifdef DEBUG_CATCHING
				printf("oob_corrected: RECEIVER STOPPED - BOTTOM\n");
#endif
				pobj->tgt_fwd = ptangle(&(pobj->x),&(ball_obj.odata.x));
				pobj->vx = pobj->vz = 0.0f;
				change_anim(pobj, catch_stand_forward_2);
			}
			change++;
		}
		else if( *new_dir < 512)
		{	// *new_dir = 256
			pobj->tgt_fwd = *new_dir = 256;
			ppdata->stick_cur = dir2stick(*new_dir);
			pobj->z = OUT_OF_BOUNDS_Z;
#ifdef DEBUG_CATCHING
			printf("%s is glancing off BOTTOM WALL, going RIGHT\n", ppdata->p_spd->szlast);
#endif
			pobj->vx = FRELX( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? TURBO_BOOST : 1.0f)), pobj->tgt_fwd );
			pobj->vz = FRELZ( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? TURBO_BOOST : 1.0f)), pobj->tgt_fwd );
		}
		else
		{	// *new_dir = 768
			pobj->tgt_fwd = *new_dir = 768;
			ppdata->stick_cur = dir2stick(*new_dir);
			pobj->z = OUT_OF_BOUNDS_Z;
#ifdef DEBUG_CATCHING
			printf("%s is glancing off BOTTOM WALL, going LEFT\n", ppdata->p_spd->szlast);
#endif
			pobj->vx = FRELX( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? TURBO_BOOST : 1.0f)), pobj->tgt_fwd );
			pobj->vz = FRELZ( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? TURBO_BOOST : 1.0f)), pobj->tgt_fwd );
		}
	}

	if( pobj->x - (-OUT_OF_BOUNDS_X) <= 0.7f && *new_dir > 512 && *new_dir < 1023)
	{	// left
	int dif = ABS(768 - *new_dir);

		change = YES;
		if(dif <= OOB_STOP_CATCH_RANGE)
		{	// stand still
			pobj->tgt_fwd = *new_dir = 768;
			ppdata->stick_cur = 24;
			pobj->x = -OUT_OF_BOUNDS_X;

	// receiver stops, faces ball, and waits for ball...
			if(ppdata->plyrnum == ball_obj.int_receiver
			&& game_info.ball_mode == BM_PASSING
			&& ppdata->plyrnum != game_info.ball_handler)
			{
#ifdef DEBUG_CATCHING
				printf("oob_corrected: RECEIVER STOPPED - LEFT\n");
#endif
				pobj->tgt_fwd = ptangle(&(pobj->x),&(ball_obj.odata.x));
				pobj->vx = pobj->vz = 0.0f;
				change_anim(pobj, catch_stand_forward_2);
			}
			change++;
		}
		else if( *new_dir < 768)
		{	// *new_dir = 512
			pobj->tgt_fwd = *new_dir = 512;
			ppdata->stick_cur = dir2stick(*new_dir);
			pobj->x = -OUT_OF_BOUNDS_X;
#ifdef DEBUG_CATCHING
			printf("%s is glancing off LEFT WALL, going UP\n",ppdata->p_spd->szlast);
#endif
			pobj->vx = FRELX( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? TURBO_BOOST : 1.0f)), pobj->tgt_fwd );
			pobj->vz = FRELZ( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? TURBO_BOOST : 1.0f)), pobj->tgt_fwd );
		}
		else
		{	// *new_dir = 0
			pobj->tgt_fwd = *new_dir = 0;
			ppdata->stick_cur = dir2stick(*new_dir);
			pobj->x = -OUT_OF_BOUNDS_X;
#ifdef DEBUG_CATCHING
			printf("%s is glancing off LEFT WALL, going DOWN\n",ppdata->p_spd->szlast);
#endif
			pobj->vx = FRELX( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? TURBO_BOOST : 1.0f)), pobj->tgt_fwd );
			pobj->vz = FRELZ( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? TURBO_BOOST : 1.0f)), pobj->tgt_fwd );
		}
	}
	else if( (pobj->x - OUT_OF_BOUNDS_X) >= -0.7f && *new_dir > 0 && *new_dir < 512)
	{	// right
	int dif = ABS(256 - *new_dir);

		change = YES;
		if(dif <= OOB_STOP_CATCH_RANGE)
		{	// stand still
			pobj->tgt_fwd = *new_dir = 256;
			ppdata->stick_cur = 24;
			pobj->x = OUT_OF_BOUNDS_X;

	// receiver stops, faces ball, and waits for ball...
			if(ppdata->plyrnum == ball_obj.int_receiver
			&& game_info.ball_mode == BM_PASSING
			&& ppdata->plyrnum != game_info.ball_handler)
			{
#ifdef DEBUG_CATCHING
				printf("oob_corrected: RECEIVER STOPPED - RIGHT\n");
#endif
				pobj->tgt_fwd = ptangle(&(pobj->x),&(ball_obj.odata.x));
				pobj->vx = pobj->vz = 0.0f;
				change_anim(pobj, catch_stand_forward_2);
			}
			change++;
		}
		else if( *new_dir < 256)
		{	// *new_dir = 0
			pobj->tgt_fwd = *new_dir = 0;
			ppdata->stick_cur = dir2stick(*new_dir);
			pobj->x = OUT_OF_BOUNDS_X;
#ifdef DEBUG_CATCHING
			printf("%s is glancing off RIGHT WALL, going DOWN\n",ppdata->p_spd->szlast);
#endif
			pobj->vx = FRELX( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? TURBO_BOOST : 1.0f)), pobj->tgt_fwd );
			pobj->vz = FRELZ( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? TURBO_BOOST : 1.0f)), pobj->tgt_fwd );
		}
		else
		{	// *new_dir = 512
			pobj->tgt_fwd = *new_dir = 512;
			ppdata->stick_cur = dir2stick(*new_dir);
			pobj->x = OUT_OF_BOUNDS_X;
#ifdef DEBUG_CATCHING
			printf("%s is glancing off RIGHT WALL, going UP\n",ppdata->p_spd->szlast);
#endif
			pobj->vx = FRELX( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? TURBO_BOOST : 1.0f)), pobj->tgt_fwd );
			pobj->vz = FRELZ( 0.0f, (ppdata->_speed * (pobj->flags & PF_TURBO ? TURBO_BOOST : 1.0f)), pobj->tgt_fwd );
		}
	}

// was there a correction...
	return change;
}

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
void begin_twopart_mode( obj_3d *pobj )
{
	int		i;

	if( pobj->adata[1].animode & MODE_TWOPART )
		return;

	// begin with the top half animating just like the bottom
	pobj->adata[1].seq_index = pobj->adata[0].seq_index;
	pobj->adata[1].pscrhdr   = pobj->adata[0].pscrhdr;
	pobj->adata[1].pscript   = pobj->adata[0].pscript;
	pobj->adata[1].anicnt    = pobj->adata[0].anicnt;
	pobj->adata[1].pframe0   = pobj->adata[0].pframe0;
	pobj->adata[1].pframe    = pobj->adata[0].pframe;
	pobj->adata[1].pxlate    = pobj->adata[0].pxlate;
	pobj->adata[1].iframe    = pobj->adata[0].iframe;
	pobj->adata[1].pahdr     = pobj->adata[0].pahdr;

	if( pobj->adata[0].pahdr->flags & AH_COMPRESSED )
	{
		pobj->adata[1].pq = pobj->adata[1].adata;
		for( i = 0; i < 60; i++ )
		{
			pobj->adata[1].pq[i] = pobj->adata[0].pq[i];
		}
	}
	else
	{
		pobj->adata[1].pq = pobj->adata[0].pq;
	}

	pobj->fwd2 = pobj->fwd;
	pobj->tgt_fwd2 = pobj->tgt_fwd;

	// Fixes guy going thru flr on running swat!  Maybe others...
	pobj->adata[1].animode |= (MODE_TWOPART|MODE_SECONDARY);
}

//////////////////////////////////////////////////////////////////////////////
void set_trgt_fwd( obj_3d *pobj, int tfwd, int trot)
{
	pobj->tgt_fwd = (pobj->fwd - tfwd) & 1023;
	pobj->turn_rate = trot;
}

//////////////////////////////////////////////////////////////////////////////
void rotate_anim( obj_3d *pobj, int angle)
{
	pobj->fwd = (pobj->fwd + angle) & 1023;
	pobj->tgt_fwd = pobj->fwd;
}

//////////////////////////////////////////////////////////////////////////////
void do_printf_in_anim( obj_3d *pobj)
{
#ifdef DEBUG
	fprintf(stderr,"----- total time = %d     ft = %d\n",ball_obj.total_time,ball_obj.flight_time);
#endif
}

//////////////////////////////////////////////////////////////////////////////
void move_player_in_anim( obj_3d *pobj, int x, int y, int z)
{
#ifdef DEBUG
printf("called MOVE_PLAYER_IN_ANIM(): with pobj-z %2.2f\n",pobj->z);
#endif
	pobj->x += (float)x;
	pobj->y += (float)y;
	pobj->z += (float)z;
#ifdef DEBUG
printf("returned from MOVE_PLAYER_IN_ANIM(): with pobj-z %2.2f\n",pobj->z);
#endif
}

//////////////////////////////////////////////////////////////////////////////
void turn_player( obj_3d *pobj, int angle)
{
	pobj->fwd = (pobj->fwd + angle) & 1023;
	pobj->tgt_fwd = pobj->fwd;
}

//////////////////////////////////////////////////////////////////////////////
void turn_player_with_vel( obj_3d *pobj, int angle)
{
	pobj->fwd = (pobj->fwd + angle) & 1023;
	pobj->tgt_fwd = pobj->fwd;
	pobj->vx = FRELX( 0.0f, ((bbplyr_data *)pobj)->_speed, pobj->tgt_fwd );
	pobj->vz = FRELZ( 0.0f, ((bbplyr_data *)pobj)->_speed, pobj->tgt_fwd );
}

//////////////////////////////////////////////////////////////////////////////
void face_carrier( obj_3d *pobj )
{
	int	me,him;

	bbplyr_data *ppdata = (bbplyr_data *)pobj;
	me = ppdata->plyrnum;
	him = game_info.ball_handler;

	if( him == -1 )
		return;

	pobj->tgt_fwd = p2p_angles[me][him];
}

//////////////////////////////////////////////////////////////////////////////
void face_catcher(obj_3d *pobj)
{
	int	me,him;

	bbplyr_data *ppdata = (bbplyr_data *)pobj;
	me = ppdata->plyrnum;
	him = ball_obj.int_receiver;

	if( him == -1 )
		return;

	pobj->tgt_fwd = p2p_angles[me][him];
}

//////////////////////////////////////////////////////////////////////////////
void face_velocity( obj_3d *pobj, int offset )
{
	float		newpt[3];
	int			angle;

	newpt[0] = pobj->x + pobj->vx;
	newpt[2] = pobj->z + pobj->vz;

	angle = ptangle( &(pobj->x), newpt );

	angle = (angle + offset) & 1023;

	pobj->tgt_fwd = angle;
}

//////////////////////////////////////////////////////////////////////////////
int is_taunt_done( obj_3d *pobj )
{
/*
	if ((pobj->pobj_hit->adata->seq_index == (SEQ_T_DSHOUL)) ||
			(pobj->pobj_hit->adata->seq_index == (SEQ_GET_UP_C)) ||
			(pobj->pobj_hit->adata->seq_index == (SEQ_GET_UP_B)) ||
			(pobj->pobj_hit->adata->seq_index == (SEQ_TAUNT_1)) ||
			(pobj->pobj_hit->adata->seq_index == (SEQ_TAUNT_3)) ||
			(pobj->pobj_hit->adata->seq_index == (SEQ_TAUNT_4)) ||
			(pobj->pobj_hit->adata->seq_index == (SEQ_TAUNT_6)))
		return(FALSE);
	else 
*/
		return(TRUE);
}

//////////////////////////////////////////////////////////////////////////////
void set_unint_lowerhalf( obj_3d *pobj, int set)
{

//	pobj->adata[1].seq_index = 0;
	if(set == 1)
		pobj->adata[0].animode |= MODE_UNINT;
	else
		pobj->adata[0].animode &= ~MODE_UNINT;
}

//////////////////////////////////////////////////////////////////////////////
int in_bounds( obj_3d *pobj )
{
	if( fabs( pobj->z ) > (OUT_OF_BOUNDS_Z - OOB_RADIUS) )
		return FALSE;

	if( fabs( pobj->x ) > (OUT_OF_BOUNDS_X - OOB_RADIUS) )
		return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
static void zerovels( obj_3d *pobj )
{
	pobj->vx = 0.0f;
	pobj->vy = 0.0f;
	pobj->vz = 0.0f;
}

//////////////////////////////////////////////////////////////////////////////
#define TURN_THRESH 16
#define SMOOTH_TURN_RATE 2
int smooth_dir_turn(int cur_dir, int new_dir)
{
	int 	turn_dist;
//	bbplyr_data *ppdata = (bbplyr_data *)pobj;

	if( (ABS(new_dir - cur_dir) > TURN_THRESH)
		|| (ABS(new_dir - cur_dir) <= SMOOTH_TURN_RATE) )
		return new_dir;
	else
	{
		if(new_dir > cur_dir)
			cur_dir += SMOOTH_TURN_RATE;
		else
			cur_dir -= SMOOTH_TURN_RATE;
		
		cur_dir &= 1023;

		return cur_dir;	
	}

	turn_dist = new_dir - cur_dir;

	// move toward target direction
	if( turn_dist != 0 )
	{
		if( ABS(turn_dist) <= SMOOTH_TURN_RATE )
		{
			cur_dir = new_dir;
		}
		else
		{
			if((( turn_dist > -1024 ) && ( turn_dist < -512 )) ||
			   (( turn_dist > 0 ) && ( turn_dist < 513 )))
			{
				cur_dir += 2;	//SMOOTH_TURN_RATE;
			}
			else
			{
				cur_dir -= 2;	//SMOOTH_TURN_RATE;
			}
		}
		cur_dir &= 1023;
	}

	return cur_dir;
}

#if	0
//////////////////////////////////////////////////////////////////////////////
static void turn_toward_tgt( obj_3d *pobj )
{
	int 	turn_dist;
//	bbplyr_data *ppdata = (bbplyr_data *)pobj;

	turn_dist = pobj->tgt_fwd - pobj->fwd;

	// move toward target direction
	if( turn_dist != 0 )
	{
		if( ABS(turn_dist) <= pobj->turn_rate )
		{
			pobj->fwd = pobj->tgt_fwd;
		}
		else
		{
			if((( turn_dist > -1024 ) && ( turn_dist < -512 )) ||
			   (( turn_dist > 0 ) && ( turn_dist < 513 )))
			{
				pobj->fwd += pobj->turn_rate;
			}
			else
			{
				pobj->fwd -= pobj->turn_rate;
			}
		}
		pobj->fwd &= 1023;
	}

	turn_dist = pobj->tgt_fwd2 - pobj->fwd2;

	// move toward target direction
	if( turn_dist != 0 )
	{
		if( ABS(turn_dist) <= pobj->turn_rate2 )
		{
			pobj->fwd2 = pobj->tgt_fwd2;
		}
		else
		{
			if((( turn_dist > -1024 ) && ( turn_dist < -512 )) ||
			   (( turn_dist > 0 ) && ( turn_dist < 513 )))
			{
				pobj->fwd2 += pobj->turn_rate2;
			}
			else
			{
				pobj->fwd2 -= pobj->turn_rate2;
			}
		}
		pobj->fwd2 &= 1023;
	}
}

#else

/********************************
 Turn towards target (test version)
********************************/

static void turn_toward_tgt( obj_3d *pobj )
{
	int 	turn_dist;
	int		tr;
	int		i, i2;

	i2 = pobj->tgt_fwd - pobj->fwd;
	i = i2 & 0x3ff;
	if (i > 512) i = i - 1024;		// Dir difference (+/-512)

	// move toward target direction
	if (i) {

		tr = pobj->turn_rate;
		i2 = ABS (i);
		if (i2 < 128)				// Threshhold
			tr = tr * i2 / 128;
		if (tr < 2) tr = 2;

		if (i2 <= tr) {
			pobj->fwd = pobj->tgt_fwd;
		}
		else {
			if (i > 0) {
				pobj->fwd += tr;
			}
			else {
				pobj->fwd -= tr;
			}
		}
		pobj->fwd &= 1023;
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	i2 = pobj->tgt_fwd2 - pobj->fwd2;
	i = i2 & 0x3ff;
	if (i > 512) i = i - 1024;		// Dir difference (+/-512)

	// move toward target direction
	if (i) {

		tr = pobj->turn_rate2;
		i2 = ABS (i);
		if (i2 < 128)				// Threshhold
			tr = tr * i2 / 128;
		if (tr < 2) tr = 2;

		if (i2 <= tr) {
			pobj->fwd2 = pobj->tgt_fwd2;
		}
		else {
			if (i > 0) {
				pobj->fwd2 += tr;
			}
			else {
				pobj->fwd2 -= tr;
			}
		}
		pobj->fwd2 &= 1023;
	}

}
#endif

//////////////////////////////////////////////////////////////////////////////
// Stuff that needs to happen before any player processes execute.
void pre_player_proc( int *parg )
{
	int i;

	while(1)
	{
		// clear anitick bits
		for(i = 0; i < NUM_PLAYERS; i++)
		{
			player_blocks[i].odata.flags &= ~PF_ANITICK;
		}

#ifdef DEBUG
		allow_switching = YES;
#endif

		drone_update();

		if ((auto_rebounding == NO) && !halt && !PLAYBACK)
		{
			// Chk auto rebounding here so everyone has a fair chance
			bbplyr_data * ppdata = player_blocks;
			void * ppani[NUM_PLAYERS];
			float p[] = { RIM_X, 0.0f, 0.0f };
			int cnt = 0;
			int	check_order[4] = {0,1,2,3}, ck_index = 0;
			int done = FALSE;

			if (game_info.active_goal == 0)
				p[0] = -RIM_X;

#if 1
			// check_order is an array of four ints, 0-3.  These are the player
			// indices in order of who gets priority on auto rebounding.  In order:
			// - human defenders (if any)
			// - drone defenders (if any)
			// - human offense (if any)
			// - drone offense (if any)

			// initialize check_order to 0,1,2,3 (done by default)

			// for each pair (0,1) and (2,3):
			// - if first is human and second is drone, leave it alone.
			// - if second is human and first is drone, swap them.
			// - otherwise, swap them 50% of the time.

			switch (p_status & 0x3)
			{
				case 1:
					break;
				case 0:
				case 3:
					if (randrng(2))
						break;
					// else fall through to swap case
				case 2:
					check_order[0] = 1;
					check_order[1] = 0;
					break;
			}
			
			switch ((p_status & 0xC) >>2)
			{
				case 1:
					break;
				case 0:
				case 3:
					if (randrng(2))
						break;
					// else fall through to swap case
				case 2:
					check_order[2] = 3;
					check_order[3] = 2;
					break;
			}
			
			// now, if team (2,3) is defense, swap (1,2) with (2,3)
			if (game_info.active_goal == 1)
			{
				int		swp;

				swp = check_order[0];
				check_order[0] = check_order[2];
				check_order[2] = swp;
				swp = check_order[1];
				check_order[1] = check_order[3];
				check_order[3] = swp;
			}

			for ( i = 0; (i < NUM_PLAYERS) && !done; i++ )
			{
				ppdata = player_blocks + check_order[i];

				if ((ppani[0] = rebound_auto(ppdata)) != NULL)
				{
					done = TRUE;
#ifdef DEBUG_REBOUNDS
					printf( "rebound priority order = %d %d %d %d\n",
						check_order[0],
						check_order[1],
						check_order[2],
						check_order[3] );

					fprintf(stderr,"Auto-Rebounder P:%d, J#:%2X\r\n",
						check_order[i], ppdata->p_spd->number);
#endif
					change_anim(&ppdata->odata, ppani[0]);
					auto_rebounding = YES;
				}
			}
#endif
#if 0
			for (i = 0; i < NUM_PLAYERS; ppdata++, i++)
			{
				ppani[i] = NULL;

				if ((ppdata->odata.plyrmode == MODE_DEFENSE) ||
					((ppdata->odata.plyrmode == MODE_OFFENSE)
				//		&& (dist3d(&ppdata->odata.x, p) > 20.0f)
					))
				{
					if ((ppani[i] = rebound_auto(ppdata))) cnt++;
#ifdef DEBUG_REBOUNDS
					if (ppani[i] != NULL)
						fprintf(stderr,"Auto-Rebound Contender P:%d, J#:%2X\r\n",
							i, ppdata->p_spd->number);
#endif
					auto_rebounding = NO;
				}
			}

			if (cnt)
			{
				cnt = randrng(cnt);
				do {
					do {
						ppdata--;
						i--;
					} while (ppani[i] == NULL);
				} while (cnt--);

#ifdef DEBUG_REBOUNDS
				fprintf(stderr,"Auto-Rebounder P:%d, J#:%2X\r\n",
					i, ppdata->p_spd->number);
#endif

				change_anim(&ppdata->odata, ppani[i]);
				auto_rebounding = YES;
			}
#endif
		}

		sleep(1);
	}
}

//////////////////////////////////////////////////////////////////////////////
// Stuff that needs to happen before any player processes execute.
#ifdef DEBUG_PLAYERS
void debug_pre_player_proc( int *parg )
{
	int i;

	while(1)
	{
		// clear anitick bits
		for(i = 0; i < teaminfo[game_info.team[0]].num_player; i++)
		{
			player_blocks[i].odata.flags &= ~PF_ANITICK;
		}

#ifdef DEBUG
		allow_switching = YES;
#endif

		drone_update();
		sleep(1);
	}
}
#endif

//////////////////////////////////////////////////////////////////////////////
// Various player bookkeeping.  Have this go off every tick AFTER all the
// player processes, but before the collision checks.
//////////////////////////////////////////////////////////////////////////////
#define NEW_SCALE
int tickdelay = 2, firsttime = 1;
unsigned short hier[]  =
{
	JOINT_PELVIS,
	JOINT_TORSO,
	JOINT_RSHOULDER,
	JOINT_RELBOW,
	JOINT_RWRIST,
	JOINT_TORSO|0x80,
	JOINT_LSHOULDER,
	JOINT_LELBOW,
	JOINT_LWRIST,
	JOINT_TORSO|0x80,
	JOINT_NECK,
	JOINT_PELVIS|0x80,
	JOINT_RHIP,
	JOINT_RKNEE,
	JOINT_RANKLE,
	JOINT_PELVIS|0x80,
	JOINT_LHIP,
	JOINT_LKNEE,
    JOINT_LANKLE,
	DONE,
};

unsigned short hier_bot[]  = 
{
	JOINT_PELVIS,
	JOINT_RHIP,
	JOINT_RKNEE,
	JOINT_RANKLE,
	JOINT_PELVIS|0x80,
	JOINT_LHIP,
	JOINT_LKNEE,
    JOINT_LANKLE,
	DONE,
	0,
};
unsigned short hier_top[] = {
	JOINT_PELVIS,
	JOINT_TORSO,
	JOINT_RSHOULDER,
	JOINT_RELBOW,
	JOINT_RWRIST,
	JOINT_TORSO|0x80,
	JOINT_LSHOULDER,
	JOINT_LELBOW,
	JOINT_LWRIST,
	JOINT_TORSO|0x80,
	JOINT_NECK,
	DONE,
};

void last_player_proc( int *parg )
{
	bbplyr_data	*ppdata;
	obj_3d		*pobj;
	float		rx,ry,rz,x,y,z,xf,yf;
	float		*cam = cambot.xform;
	float		mat[12],cm[12],cm2[12];
	float		cm_flipped[12],cm2_flipped[12];
	int			i,onscreen;
	float 		m1[16];
	float 		m2[16];
	float 		m3[16];

	float 		mtmp[16];

	int			second_pass = FALSE;

	// second_pass: If a player is hanging from the rims, we adjust his world coordinates based on the
	// matrices that have already been generated.  Since the world coordinates are inputs to the matrix
	// generation process, we have to then repeat the process for that player.  We do this by decrementing
	// the player loop counter and setting the second_pass flag to TRUE.

	while( 1 )
	{
		// Reinit to assume controlled players will be drones; gets
		//  set in the <for> loop if humans are found
		bbplyr_skel[JOINT_NECK].flags |= SN_HEAD_SCALE;	// somewhere this bit is being cleared ! ! !

		ppdata = player_blocks;

		// flag onscreen players
		onscreen = 0;
		for( i = 0; i < NUM_PLAYERS + 1; ppdata++, i++ )
		{
			if(i == NUM_PLAYERS)
			{
				if (!referee_active)
					continue;
				ppdata = &referee_data;
			}
			pobj = (obj_3d *)ppdata;

			rx = pobj->x - cambot.x;
			ry = pobj->y - cambot.y + pobj->ay;
			rz = pobj->z - cambot.z;

			// Compute camera coords of player center
			x =  (cam[0]*rx+cam[1]*ry+cam[ 2]*rz)+cam[3];
			y = ((cam[4]*rx+cam[5]*ry+cam[ 6]*rz) * (is_low_res ? 0.6666f : 1.0f)) + cam[7];
			z =  (cam[8]*rx+cam[9]*ry+cam[10]*rz)+cam[11];

			if(i < NUM_PLAYERS)
			{
			 	// Setup player arrows info
				yf = pobj->ay;
				arrowinfo[i].plyr_sx = x - (cam[1]*yf);
				arrowinfo[i].plyr_sy = (y - cam[7]) * (is_low_res ? 1.5f : 1.0f) - cam[5]*yf + cam[7];
				arrowinfo[i].plyr_sz = z - (cam[9]*yf);
				//arrowinfo[i].plyr_sx = ppdata->jpos[JOINT_TORSO][0] - 0.4f * (ppdata->jpos[JOINT_TORSO][0] - ppdata->jpos[JOINT_NECK][0]);
				//arrowinfo[i].plyr_sy = ppdata->jpos[JOINT_TORSO][1] - 0.4f * (ppdata->jpos[JOINT_TORSO][1] - ppdata->jpos[JOINT_NECK][1]);
				//arrowinfo[i].plyr_sz = ppdata->jpos[JOINT_TORSO][2] - 0.4f * (ppdata->jpos[JOINT_TORSO][2] - ppdata->jpos[JOINT_NECK][2]);
				arrowinfo[i].ppdata  = ppdata;
			}
			else
				yf = pobj->ay - referee_data.p_spd->tall * IN2UN;

			// Compute screen coords of player center
			//  Result modifiers added: DJT
			xf = 1.05f * (z * ((0.5f + 25.0f / hres) /  fovfac ));
			yf = 1.15f * (z * ((vres + 25.0f * 2.0f) / (fovfac2x * hres)));

			// set or clear onscreen bit
			if(( z < 10.0f ) || ( fabs(x) > xf ) || ( fabs(y) > yf ))
			{
				pobj->flags &= ~PF_ONSCREEN;
				if(i < NUM_PLAYERS)
					arrowinfo[i].ai_flags &= ~AF_ONSCREEN;
			}
			else
			{
				pobj->flags |= PF_ONSCREEN;
				if(i < NUM_PLAYERS)
				{
					arrowinfo[i].ai_flags |=  AF_ONSCREEN;
					onscreen++;
				}
			}

			// onscreen or not, we need the matrices for collision checking
			mat[3]  = rx;
			mat[7]  = ry;
			mat[11] = rz;

			roty( pobj->fwd, mat );
			mxm( cambot.xform, mat, cm );

			mat[7]  = -pobj->y - cambot.y + -pobj->ay;
			mat[1] *= -1.0f;
			mat[5] *= -1.0f;
			mat[9] *= -1.0f;
			mxm( cambot.xform, mat, cm_flipped );

			cm[0] *= pobj->xscale; cm[1] *= pobj->yscale; cm[ 2] *= pobj->zscale;
			cm[4] *= pobj->xscale; cm[5] *= pobj->yscale; cm[ 6] *= pobj->zscale;
			cm[8] *= pobj->xscale; cm[9] *= pobj->yscale; cm[10] *= pobj->zscale;
			cm_flipped[0] *= pobj->xscale; cm_flipped[1] *= pobj->yscale; cm_flipped[ 2] *= pobj->zscale;
			cm_flipped[4] *= pobj->xscale; cm_flipped[5] *= pobj->yscale; cm_flipped[ 6] *= pobj->zscale;
			cm_flipped[8] *= pobj->xscale; cm_flipped[9] *= pobj->yscale; cm_flipped[10] *= pobj->zscale;
			if( pobj->adata[1].animode & MODE_TWOPART )
			{
				// do top half first so bottom overwrites pelvis matrix (top and bottom
				// each have a pelvis, but only the bottom's pelvis counts.
				mat[7]  = ry;
				roty( pobj->fwd2, mat );
				mxm( cambot.xform, mat, cm2 );
				
				mat[1] *= -1.0f;
				mat[5] *= -1.0f;
				mat[9] *= -1.0f;
				mat[7]  = -pobj->y - cambot.y + -pobj->ay;
				mxm( cambot.xform, mat, cm2_flipped );

#ifdef NEW_SCALE
				cm2[0] *= pobj->xscale; cm2[1] *= pobj->yscale; cm2[ 2] *= pobj->zscale;
				cm2[4] *= pobj->xscale; cm2[5] *= pobj->yscale; cm2[ 6] *= pobj->zscale;
				cm2[8] *= pobj->xscale; cm2[9] *= pobj->yscale; cm2[10] *= pobj->zscale;
#else
				cm2[0] *= pobj->ascale; cm2[1] *= pobj->ascale; cm2[ 2] *= pobj->ascale;
				cm2[4] *= pobj->ascale; cm2[5] *= pobj->ascale; cm2[ 6] *= pobj->ascale;
				cm2[8] *= pobj->ascale; cm2[9] *= pobj->ascale; cm2[10] *= pobj->ascale;
#endif

#ifdef ITERATIVE_GEN_MATRICES
			fgenerate_matrices(hier_top,  bbplyr_skel, ppdata,	
						cm2, pobj->adata[1].pq, ppdata->f_headsc, ppdata->plyrnum);
			fgenerate_matrices(hier_bot,  bbplyr_skel, ppdata,	
						cm, pobj->adata[0].pq, ppdata->f_headsc, ppdata->plyrnum);
#else
			generate_matrices( tophalf_skel, cm2, pobj->adata[1].pq, ppdata->matrices[0], ppdata->jpos[0], ppdata->f_headsc, ppdata->plyrnum );
			generate_matrices( bothalf_skel, cm, pobj->adata[0].pq, ppdata->matrices[0], ppdata->jpos[0], ppdata->f_headsc, ppdata->plyrnum );
#endif // ITERATIVE_GEN_MATRICES


			m1[0] = cm_flipped[0];
			m1[1] = cm_flipped[1];
			m1[2] = cm_flipped[2];
			m1[3] = cm_flipped[3];
			m1[4] = cm_flipped[4];
			m1[5] = cm_flipped[5];
			m1[6] = cm_flipped[6];
			m1[7] = cm_flipped[7];
			m1[8] = cm_flipped[8];
			m1[9] = cm_flipped[9];
			m1[10] = cm_flipped[10];
			m1[11] = cm_flipped[11];
			m1[12] = 0.0f;
			m1[13] = 0.0f;
			m1[14] = 0.0f;
			m1[15] = 1.0f;

			if ( get_inv_matrix4x4(m2, m1) == FALSE)
				{
				matrix_set_identity (m2);
				}

 			matrix_trans (m3, m2, pobj->x,(pobj->y+pobj->ay),pobj->z);

			generate_matrices_flipy( bothalf_skel, cm_flipped, pobj->adata[0].pq, ppdata->matrices_flipped[0], ppdata->jpos[0], ppdata->f_headsc, ppdata->plyrnum, m3,ppdata->inv_camera[0] );
			}
			else
			{
#ifdef ITERATIVE_GEN_MATRICES
				fgenerate_matrices(hier,  bbplyr_skel, ppdata,	
						cm, pobj->adata[0].pq, ppdata->f_headsc, ppdata->plyrnum);
#else
				generate_matrices( bbplyr_skel, cm, pobj->adata[0].pq, ppdata->matrices[0], ppdata->jpos[0], ppdata->f_headsc, ppdata->plyrnum );
#endif // ITERATIVE_GEN_MATRICES

			m1[0] = cm_flipped[0];
			m1[1] = cm_flipped[1];
			m1[2] = cm_flipped[2];
			m1[3] = cm_flipped[3];
			m1[4] = cm_flipped[4];
			m1[5] = cm_flipped[5];
			m1[6] = cm_flipped[6];
			m1[7] = cm_flipped[7];
			m1[8] = cm_flipped[8];
			m1[9] = cm_flipped[9];
			m1[10] = cm_flipped[10];
			m1[11] = cm_flipped[11];
			m1[12] = 0.0f;
			m1[13] = 0.0f;
			m1[14] = 0.0f;
			m1[15] = 1.0f;

			if ( get_inv_matrix4x4(m2, m1) == FALSE)
				{
				matrix_set_identity (m2);
				}

			matrix_trans (m3, m2, pobj->x,(pobj->y+pobj->ay),pobj->z);

			generate_matrices_flipy( bothalf_skel, cm_flipped, pobj->adata[0].pq, ppdata->matrices_flipped[0], ppdata->jpos[0], ppdata->f_headsc, ppdata->plyrnum,m3,ppdata->inv_camera[0]  );

#ifndef DEBUG_NOHEAD_TURNS
			if(i < NUM_PLAYERS) // ???
				LookAt(ppdata, -NUM_PERTEAM);
#endif
			}

		// Check to see if player is hanging on the rim
			if(ppdata->HangingOnRim == YES)
			{
				float hx2, hy2, hz2;
				float hx, hy, hz, *tm;
				int limb;

#if 1
				if (second_pass)
				{
					second_pass = FALSE;
				}
				else
				{	
					second_pass = TRUE;
					i--;
#endif
					//get_joint_position(dunker, -1, -1, ally[i].limb, 256, &offx, &offy, &offz);
					tm = cambot.xform;
					limb = ppdata->ball_in_hand == RIGHT_HAND ? JOINT_RWRIST : JOINT_LWRIST;
					hx = ppdata->jpos[limb][0] * tm[0] + ppdata->jpos[limb][1] * tm[4] + ppdata->jpos[limb][2] * tm[8] + cambot.x;
					hy = ppdata->jpos[limb][0] * tm[1] + ppdata->jpos[limb][1] * tm[5] + ppdata->jpos[limb][2] * tm[9] + cambot.y;
					hz = ppdata->jpos[limb][0] * tm[2] + ppdata->jpos[limb][1] * tm[6] + ppdata->jpos[limb][2] * tm[10] + cambot.z;

	// BOTH HANDS ON RIM
					if(both_hands_on_rim == YES)
					{
						limb = (limb == JOINT_RWRIST) ? JOINT_LWRIST : JOINT_RWRIST;
						hx2 = ppdata->jpos[limb][0] * tm[0] + ppdata->jpos[limb][1] * tm[4] + ppdata->jpos[limb][2] * tm[8] + cambot.x;
						hy2 = ppdata->jpos[limb][0] * tm[1] + ppdata->jpos[limb][1] * tm[5] + ppdata->jpos[limb][2] * tm[9] + cambot.y;
						hz2 = ppdata->jpos[limb][0] * tm[2] + ppdata->jpos[limb][1] * tm[6] + ppdata->jpos[limb][2] * tm[10] + cambot.z;

						hx = (hx + hx2) * 0.5f;
						hy = (hy + hy2) * 0.5f;
						hz = (hz + hz2) * 0.5f;
					}

					pobj->x += (dunkfx - hx);
					pobj->y += (dunkfy - hy) - 1.0f;
					pobj->z += (dunkfz - hz);
					//printf("hx %3.1f, hy %3.1f, hz %3.1f, dunkfx %3.1f, dunkfy %3.1f, dunkfz %3.1f\n",hx, hy, hz, dunkfx, dunkfy, dunkfz);

					ppdata--;
				}
			}

		}

		sleep( 1 );
	}
}

#ifdef DEBUG_PLAYERS
void debug_last_player_proc( int *parg )
{
	bbplyr_data	*ppdata;
	obj_3d		*pobj;
	float		rx,ry,rz,x,y,z,xf,yf;
	float		*cam = cambot.xform;
	float		mat[12],cm[12],cm2[12];
	float		cm_flipped[12],cm2_flipped[12];
	int			i,onscreen;

	while( 1 )
	{
		// Reinit to assume controlled players will be drones; gets
		//  set in the <for> loop if humans are found
		bbplyr_skel[JOINT_NECK].flags |= SN_HEAD_SCALE;	// somewhere this bit is being cleared ! ! !

		ppdata = player_blocks;

		// flag onscreen players
		onscreen = 0;
		for( i = 0; i < teaminfo[game_info.team[0]].num_player; ppdata++, i++ )
		{
			pobj = (obj_3d *)ppdata;

			rx = pobj->x - cambot.x;
			ry = pobj->y - cambot.y + pobj->ay;
			rz = pobj->z - cambot.z;

			// Compute camera coords of player center
			x = cam[0]*rx+cam[1]*ry+cam[2]*rz+cam[3];
			y = cam[4]*rx+cam[5]*ry+cam[6]*rz+cam[7];
			z = cam[8]*rx+cam[9]*ry+cam[10]*rz+cam[11];

			// Compute screen coords of player center
			//  Result modifiers added: DJT
			xf = 1.05f * (z * ((0.5f + 25.0f / hres) /  fovfac ));
			yf = 1.15f * (z * ((vres + 25.0f * 2.0f) / (fovfac2x * hres)));

			// set or clear onscreen bit
			if(( z < 10.0f ) || ( fabs(x) > xf ) || ( fabs(y) > yf ))
				pobj->flags &= ~PF_ONSCREEN;
			else
				pobj->flags |= PF_ONSCREEN;

			// onscreen or not, we need the matrices for collision checking
			mat[3]  = rx;
			mat[7]  = ry;
			mat[11] = rz;

			roty( pobj->fwd, mat );
			mxm( cambot.xform, mat, cm );

			mat[7]  = -pobj->y - cambot.y + -pobj->ay;
			mat[1] *= -1.0f;
			mat[5] *= -1.0f;
			mat[9] *= -1.0f;
			mxm( cambot.xform, mat, cm_flipped );

			cm[0] *= pobj->xscale; cm[1] *= pobj->yscale; cm[ 2] *= pobj->zscale;
			cm[4] *= pobj->xscale; cm[5] *= pobj->yscale; cm[ 6] *= pobj->zscale;
			cm[8] *= pobj->xscale; cm[9] *= pobj->yscale; cm[10] *= pobj->zscale;
			cm_flipped[0] *= pobj->xscale; cm_flipped[1] *= pobj->yscale; cm_flipped[ 2] *= pobj->zscale;
			cm_flipped[4] *= pobj->xscale; cm_flipped[5] *= pobj->yscale; cm_flipped[ 6] *= pobj->zscale;
			cm_flipped[8] *= pobj->xscale; cm_flipped[9] *= pobj->yscale; cm_flipped[10] *= pobj->zscale;

			if( pobj->adata[1].animode & MODE_TWOPART )
			{
				// do top half first so bottom overwrites pelvis matrix (top and bottom
				// each have a pelvis, but only the bottom's pelvis counts.
				mat[7]  = ry;
				roty( pobj->fwd2, mat );
				mxm( cambot.xform, mat, cm2 );
				
				mat[1] *= -1.0f;
				mat[5] *= -1.0f;
				mat[9] *= -1.0f;
				mat[7]  = -pobj->y - cambot.y + -pobj->ay;
				mxm( cambot.xform, mat, cm2_flipped );

				cm2[0] *= pobj->xscale; cm2[1] *= pobj->yscale; cm2[ 2] *= pobj->zscale;
				cm2[4] *= pobj->xscale; cm2[5] *= pobj->yscale; cm2[ 6] *= pobj->zscale;
				cm2[8] *= pobj->xscale; cm2[9] *= pobj->yscale; cm2[10] *= pobj->zscale;
				generate_matrices( tophalf_skel, cm2, pobj->adata[1].pq, ppdata->matrices[0], ppdata->jpos[0], ppdata->f_headsc, ppdata->plyrnum );
				generate_matrices( bothalf_skel, cm, pobj->adata[0].pq, ppdata->matrices[0], ppdata->jpos[0], ppdata->f_headsc, ppdata->plyrnum );
			}
			else
			{
				generate_matrices( bbplyr_skel, cm, pobj->adata[0].pq, ppdata->matrices[0], ppdata->jpos[0], ppdata->f_headsc, ppdata->plyrnum );

#ifndef DEBUG_PLAYERS
 #ifndef DEBUG_NOHEAD_TURNS
			if(i < NUM_PLAYERS) // ???
				LookAt(ppdata, -NUM_PERTEAM);
 #endif
#endif
			}

		}

		sleep( 1 );
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////
// zcmp() - Z-test function for qsort().
#if 0
static int zcmp( const void *p1, const void *p2 )
{
	int		i1,i2;

	i1 = *((int *)p1);
	i2 = *((int *)p2);

	if( player_blocks[i2].sz > player_blocks[i1].sz )
		return -1;

	if( player_blocks[i2].sz < player_blocks[i1].sz )
		return 1;

	return 0;
}
#endif

#ifndef DEBUG_NOHEAD_TURNS
//////////////////////////////////////////////////////////////////////////////
void LookAt(bbplyr_data *p1, int i)
{
	obj_3d *pobj = (obj_3d *)p1;
	float p[3];
	int rim_bearing, bearing, new_dir = 0, plyr = p1->plyrnum;

	p[0] = (p1->team ^ game_info.off_goal) == 1 ? RIM_X : -RIM_X; p[1] = RIM_Y; p[2] = 0.0f;
	rim_bearing = (p1->odata.fwd - ptangle( &(p1->odata.x), p)) & 1023;
	if(rim_bearing > 512) rim_bearing = 1024 - rim_bearing;

#ifdef DEBUG
	if (!halt)
#endif
	head_wait[plyr] = GREATER(0, head_wait[plyr] - 1);

	// FIX!!  Allow pre tip to look some small % of time
	if(pobj->flags & PF_NO_LOOK || game_info.game_time <= 0)
		look[plyr] = -3, head_wait[plyr] = 1;

	if(head_wait[plyr] <= 0)
	{
		if(plyr == game_info.ball_handler)
		{
			if(pobj->flags & PF_FUNKY_DRIBBLE)
			{
	// look straight ahead...
				look[plyr] = -3, head_wait[plyr] = 60;
			}
			else if(pobj->flags & (PF_SHOOTING|PF_DUNKER))
			{
	// look at the rim...
				look[plyr] = -1, head_wait[plyr] = 60;

	// maybe look at a close defender...
				if((pobj->flags & (PF_DUNKER|PF_ALLYDUNKER))
					&& distxz[plyr][nearest_to_me(plyr, !p1->team)] <= 40.0f
					&& randrng(100) <= 45)
					look[plyr] = nearest_to_me(plyr, !p1->team), head_wait[plyr] = 60;
			}
			else if(pobj->flags & PF_PASSING)
			{
	// I might look at the nearest defender in front or really close...
				if(randrng(100) <= 50)
					look[plyr] = ball_obj.int_receiver, head_wait[plyr] = 45;
				else
					look[plyr] = nearest_to_me(plyr, !p1->team), head_wait[plyr] = 45;
			}
			else
			{	// I aint passing
				look[plyr] = -3, head_wait[plyr] = 60 * 2;	// default to looking ahead...

	// I might look at the nearest defender in front or really close...
				if( (plyr_bearing(plyr, nearest_to_me(plyr, !p1->team)) <= 128
						|| distxz[plyr][nearest_to_me(plyr, !p1->team)] <= 30.0f)
				&& in_the_paint(p1, YES) == NO
				&& rim_bearing <= 128
				&& randrng(100) <= 30)
					look[plyr] = nearest_to_me(plyr, !p1->team), head_wait[plyr] = 45 * 3;

	// I might look at the nearest defender to the rim...
				if((plyr_bearing(plyr, nearest_to_rim(!p1->team)) <= 128)
				&& rim_bearing <= 64
				&& in_the_paint(p1, YES) == NO
				&& distxz[plyr][nearest_to_rim(!p1->team)] <= 60.0f
				&& randrng(100) <= 30)
					look[plyr] = nearest_to_rim(!p1->team), head_wait[plyr] = 45 * 3;

	// I might look at my teammate
				if((plyr_bearing(plyr, nearest_to_me(plyr, p1->team)) <= 196)
				&& randrng(100) <= 30)
					look[plyr] = nearest_to_me(plyr, p1->team), head_wait[plyr] = 45 * 3;
			}
		}
		else
		{
			if(p1->team == game_info.off_side)
			{	// offense...
				look[plyr] = -3, head_wait[plyr] = 30;	// default to looking ahead...

				if(game_info.ball_mode == BM_PASSING && plyr == ball_obj.int_receiver)
					look[plyr] = -2, head_wait[plyr] = 45;	// look at ball...

				else if(plyr_bearing(plyr, nearest_to_me(plyr, p1->team)) <= 200 && randrng(100) <= 40)
					look[plyr] = nearest_to_me(plyr, p1->team), head_wait[plyr] = 45 * 3;

				else if(plyr_bearing(plyr, nearest_to_me(plyr, !p1->team)) >= 196
					&& distxz[plyr][nearest_to_me(plyr, !p1->team)] <= 40.0f
					&& randrng(100) <= 35)
						look[plyr] = nearest_to_me(plyr, !p1->team), head_wait[plyr] = 60;	// default to looking ahead...

				if(pobj->plyrmode == MODE_TIPOFF)
					look[plyr] = (randrng(100) <= 50) ? nearest_to_me(plyr, !p1->team) : nearest_to_me(plyr, !p1->team) ^ 1, head_wait[plyr] = 45;	// look at other team

				if(game_info.ball_handler == NO_BALLHANDLER)
					look[plyr] = (randrng(100) <= 20) ?  -2 : -3, head_wait[plyr] = 96;	// look at ball or straight ahead
			}
			else
			{	// defense...
				look[plyr] = -3, head_wait[plyr] = 60 * 1;	// default to looking ahead...
				if(game_info.ball_mode == BM_SHOOTING || game_info.ball_mode == BM_PASSING)
				{
					if(plyr_bearing(plyr, nearest_to_me(plyr, !p1->team)) <= 128
					&& distxz[plyr][nearest_to_me(plyr, !p1->team)] <= 30.0f
					&& randrng(100) <= 15)
						look[plyr] = nearest_to_me(plyr, !p1->team), head_wait[plyr] = 120;	// default to looking ahead...
					else
						look[plyr] = -2, head_wait[plyr] = 120 * 1;	// default to looking at ball...
				}
				else
				{
					if(randrng(100) <= 10)
						look[plyr] = plyr ^ 1, head_wait[plyr] = 60 * 1;	// look at teammate...

					if((nearest_to_me(plyr, !p1->team) ^ 1) == game_info.ball_handler
					&& plyr_bearing(plyr, (nearest_to_me(plyr, !p1->team) ^ 1)) <= 220
					&& distxz[plyr][nearest_to_me(plyr, !p1->team)] > 60.0f
					&& randrng(100) <= 45)
						look[plyr] = nearest_to_me(plyr, !p1->team) ^ 1, head_wait[plyr] = 120 * 3;	// default to looking ball handler...

					if(plyr_bearing(plyr, nearest_to_me(plyr, !p1->team)) <= 128
					&& distxz[plyr][nearest_to_me(plyr, !p1->team)] <= 60.0f
					&& randrng(100) <= 75)
						look[plyr] = nearest_to_me(plyr, !p1->team), head_wait[plyr] = 120 * 3;	// default to looking ahead...

					if(pobj->plyrmode == MODE_TIPOFF)
						look[plyr] = (randrng(100) <= 50) ? nearest_to_me(plyr, !p1->team) : nearest_to_me(plyr, !p1->team) ^ 1, head_wait[plyr] = 45;	// look at other team

					if(game_info.ball_handler == NO_BALLHANDLER)
						look[plyr] = (randrng(100) <= 20) ?  -2 : -3, head_wait[plyr] = 120 * 2;	// look at ball or straight ahead
				}
			}
		}
	}	

// I might look at the rim...
	if(look[plyr] == -1)
	{
		p[0] = (p1->team ^ game_info.off_goal) == 1 ? RIM_X : -RIM_X; p[1] = RIM_Y; p[2] = 0.0f;
		p1->head_tgt_dir = bearing = (-pobj->fwd + ptangle(&pobj->x, p)) & 1023;
	}
// I might look at the ball...
	else if(look[plyr] == -2)
	{
		p1->head_tgt_dir = bearing = (-pobj->fwd + ptangle(&pobj->x, &ball_obj.odata.x)) & 1023;
	}
// I might look at a specified player...
	else if(look[plyr] >= 0)
	{
		p1->head_tgt_dir = bearing = (-pobj->fwd + p2p_angles[p1->plyrnum][look[p1->plyrnum]]) & 1023;
	}
// I will look straight ahead...
	else	// look[plyr] <= -3
		p1->head_tgt_dir = bearing = 0;

	if( bearing > 512 )
		p1->head_tgt_dir = bearing - 1024;

	TurnHead( p1 );
	rot_mat( p1->matrices[JOINT_NECK], p1->head_dir );

}
#endif

//////////////////////////////////////////////////////////////////////////////
void rot_mat(float *pm, int angle)
{
	float ry[12], cm[12];

	ry[3] = ry[7] = ry[11] = 0;
	roty( angle, ry );
	mxm( pm, ry, cm );

	pm[0]  = cm[0];
	pm[1]  = cm[1];
	pm[2]  = cm[2];
	pm[4]  = cm[4];
	pm[5]  = cm[5];
	pm[6]  = cm[6];
	pm[8]  = cm[8];
	pm[9]  = cm[9];
	pm[10] = cm[10];
}

//////////////////////////////////////////////////////////////////////////////
void TurnHead(bbplyr_data *ppdata)
{
//obj_3d *pobj = (obj_3d *)ppdata;
int turn_dist, cur_dir, want_dir;

#if 0

	int		tr;
	int		i, i2;

	i2 = ppdata->head_tgt_dir - ppdata->head_dir;
	i = i2 & 0x3ff;
	if (i > 512) i = i - 1024;		// Dir difference (+/-512)

	// move toward target direction
	if (i) {

		tr = ppdata->head_turn_rate;
		i2 = ABS (i);
		if (i2 < 128)				// Threshhold
			tr = tr * i2 / 128;
		if (tr < 2) tr = 2;

		if (i2 <= tr) {
			ppdata->head_dir = ppdata->head_tgt_dir;
		}
		else {
			if (i > 0) {
				ppdata->head_dir += tr;
			}
			else {
				ppdata->head_dir -= tr;
			}
		}
		ppdata->head_dir &= 1023;
	}

#else
	cur_dir = ppdata->head_dir;
	want_dir = ppdata->head_tgt_dir;
	
	turn_dist = cur_dir - want_dir;

	// move toward target direction
	if( turn_dist != 0 )
	{
		if( ABS(turn_dist) <= ppdata->head_turn_rate )
		{
			if(want_dir > cur_dir)
			{
				ppdata->head_dir += ppdata->head_turn_rate / 4;
				if(ppdata->head_dir > want_dir)
					ppdata->head_dir = want_dir;
			}
			else
			{
				ppdata->head_dir -= ppdata->head_turn_rate / 4;
				if(ppdata->head_dir < want_dir)
					ppdata->head_dir = want_dir;
			}
			//ppdata->head_dir = ppdata->head_tgt_dir;
		}
		else
		{
			if(want_dir > cur_dir)
				ppdata->head_dir += ppdata->head_turn_rate;
			else
				ppdata->head_dir -= ppdata->head_turn_rate;
		}
	}
#endif
	ppdata->head_dir = ppdata->head_dir >  220 ?  220 : ppdata->head_dir;
	ppdata->head_dir = ppdata->head_dir < -220 ? -220 : ppdata->head_dir;
}

//////////////////////////////////////////////////////////////////////////////
// player draw functions
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// call this one before any players get drawn
//////////////////////////////////////////////////////////////////////////////
static void player_pre_draw( void *oi )
{
	// init glide states
	guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);
	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
	grTexClampMode(0,GR_TEXTURECLAMP_WRAP,GR_TEXTURECLAMP_WRAP);

	grDepthBufferMode( GR_DEPTHBUFFER_WBUFFER );
	grDepthMask( FXTRUE );

	grChromakeyMode( GR_CHROMAKEY_DISABLE );

	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
	grAlphaBlendFunction( GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA,
			GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA );

	grDepthBufferFunction( GR_CMP_LESS );
	grDepthBiasLevel( 0 );
}

//////////////////////////////////////////////////////////////////////////////
#define C_BOTH				0x100		// 2 HANDED CATCH
#define C_LEFT	 			0x200		// PUT BALL IN LEFT HAND
#define C_RIGHT				0x400		// PUT BALL IN RIGHT HAND
#define C_NON_TWOPART	0x800		// NON TWO PART MOVING CATCH
#define C_NO_BOUNCE		0x1000	// CATCH DOESN'T WORK WITH BOUNCE PASS
#define C_OOBCC_CATCH 0x2000	// CATCH WORKS FOR OOB CORRECTION
struct Catch
{
	void *s_scr;		// standing catch script
	void *r_scr;		// running catch script
	int min_time;		// minimum catch time
	int quad;				// quad this catch is valid from(-1 = any quad)
	int seq;
	int frame;
	//int prob;				// probability of this catch happening
};

struct Catch catches[] =
{
// BEHIND CATCHES...
	{&catch_run_spin_behind_r, &catch_run_spin_behind_r, 20, C_BOTH | C_LEFT | C_NON_TWOPART | Q_BEHIND, (SEQ_C_SPN1_R), 17},
	{&catch_run_spin_behind_l, &catch_run_spin_behind_l, 20, C_BOTH | C_RIGHT  | C_NON_TWOPART | Q_BEHIND, (SEQ_C_SPN1_L), 17},

	{&catch_stand_behind_l, &catch_run_behind_l, -1, C_BOTH | C_LEFT | Q_LEFT_BEHIND | Q_BEHIND, (SEQ_C_BCK4_L), 43},
	{&catch_stand_behind_r, &catch_run_behind_r, -1, C_BOTH | C_RIGHT | Q_RIGHT_BEHIND | Q_BEHIND, (SEQ_C_BCK4_R), 43},

// FORWARD CATCHES...
	{&catch_stand_forward_2, &catch_run_forward_2, -1, C_BOTH | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT, (SEQ_C_FWD1_B), 27},

	{&catch_stand_overhead_forward, &catch_run_overhead_forward, 45, C_NO_BOUNCE | C_BOTH | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT, (SEQ_C_OVR_HD), 11},
//	{&catch_stand_forward_2, &catch_run_forward_2, -1, C_OOBCC_CATCH | C_BOTH | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT, (SEQ_C_FWD1_B), 27},

	{&catch_stand_forward_2, &catch_run_forward_2, -1, C_BOTH | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT, (SEQ_C_FWD1_B), 27},
	{&catch_stand_forward_2, &catch_run_forward_2, -1, C_BOTH | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT, (SEQ_C_FWD1_B), 27},
	{&catch_stand_forward_2, &catch_run_forward_2, -1, C_BOTH | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT, (SEQ_C_FWD1_B), 27},
	{&catch_stand_forward_2, &catch_run_forward_2, -1, C_BOTH | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT, (SEQ_C_FWD1_B), 27},
	{&catch_stand_forward_2, &catch_run_forward_2, -1, C_BOTH | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT, (SEQ_C_FWD1_B), 27},
	{&catch_stand_forward_2, &catch_run_forward_2, -1, C_BOTH | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT, (SEQ_C_FWD1_B), 27},
	{&catch_stand_forward_2, &catch_run_forward_2, -1, C_BOTH | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT, (SEQ_C_FWD1_B), 27},
	{&catch_stand_forward_2, &catch_run_forward_2, -1, C_BOTH | Q_LEFT_FRONT | Q_FRONT | Q_RIGHT_FRONT, (SEQ_C_FWD1_B), 27},

// RIGHT - LEFT CATCHES...
	{&catch_stand_right_1, &catch_run_right_1, -1, C_BOTH | C_RIGHT | Q_RIGHT, (SEQ_C_SID6_R), 35},
	{&catch_stand_left_1, &catch_run_left_1, -1, C_BOTH | C_LEFT | Q_LEFT, (SEQ_C_SID6_L), 35},

	{&catch_stand_right_2, &catch_run_right_2, 30, C_BOTH | Q_RIGHT, (SEQ_C_SID4_R), 26},
	{&catch_stand_left_2, &catch_run_left_2, 30, C_BOTH | Q_LEFT, (SEQ_C_SID4_L), 26},

	{&catch_stand_right_3, &catch_run_right_3, 30, C_NO_BOUNCE | C_RIGHT | Q_RIGHT, (SEQ_C_SID2_R), 16},
	{&catch_stand_left_3, &catch_run_left_3, 30, C_NO_BOUNCE | C_LEFT | Q_LEFT, (SEQ_C_SID2_L), 16},

	{&catch_stand_right_4, &catch_run_right_4, 30, C_LEFT | Q_RIGHT, (SEQ_C_SID5_R), 29},
	{&catch_stand_left_4, &catch_run_left_4, 30, C_RIGHT | Q_LEFT, (SEQ_C_SID5_L), 29},
};

#define BOUNCE_PERCENT	0.50f
#define NUM_CATCHES	(sizeof(catches)/sizeof(struct Catch))
//#define MIN_CATCH_TIME	15

//////////////////////////////////////////////////////////////////////////////
void pass_ball( obj_3d *pobj, obj_3d *ptgt, int hurl)
{
	bbplyr_data *ppdata = (bbplyr_data *)pobj;
	bbplyr_data *tgt = (bbplyr_data *)ptgt;
	float *tm, c_time;
	float tx,ty,tz,cx,cy,cz;
	int	time;

	tm = cambot.xform;

	cx = tgt->jpos[JOINT_PELVIS][0];
	cy = tgt->jpos[JOINT_PELVIS][1];
	cz = tgt->jpos[JOINT_PELVIS][2];

	tx = cx * tm[0] + cy * tm[4] + cz * tm[ 8] + cambot.x;
	ty = cx * tm[1] + cy * tm[5] + cz * tm[ 9] + cambot.y;
	tz = cx * tm[2] + cy * tm[6] + cz * tm[10] + cambot.z;

	if(hurl == NO && ptgt->y == 0.0f)
	{
		c_time = (MAX_PASS_TIME * (distxz[ppdata->plyrnum][tgt->plyrnum] / MAX_PASS_DIST));
		c_time = GREATER(c_time, MIN_PASS_TIME);

		time = (int)c_time;

		ball_obj.odata.vx = (tx - ball_obj.odata.x) / (float)time;
		ball_obj.odata.vy = ((ty - ball_obj.odata.y) / (float)time) - (0.5f * BALL_GRAVITY * (float)time);
		ball_obj.odata.vz = (tz - ball_obj.odata.z) / (float)time;
		ball_obj.flags &= ~BF_BOUNCEPASS;	// don't do a bounce pass

		ball_obj.flight_time = 0;
		ball_obj.total_time = time;
		ppdata->no_ball_collide = time;

		ptgt->tgt_fwd = p2p_angles[tgt->plyrnum][ppdata->plyrnum];

		if(!(ptgt->flags & PF_ALLYDUNKER))
		{
			catch_time = MIN_PASS_TIME;
			catch_anim = (char *)((int)catch_stand_forward_2 | 0x80000000);
			ptgt->vx = ptgt->vz = 0.0f;
			change_anim(ptgt, breath_anims[randrng(sizeof(breath_anims)/sizeof(char *))]);
		}
	}
	else
	{
		c_time = (MAX_PASS_TIME * (100.0f / MAX_PASS_DIST));
		c_time = GREATER(c_time, MIN_PASS_TIME);

		time = (int)c_time;

		ball_obj.odata.vx = ((tx - ball_obj.odata.x) / (float)time) * 0.5f;//(.4f + (.4f * tgt->_passing / 20.0f));
		ball_obj.odata.vy = (((ty + 10.0f) - ball_obj.odata.y) / (float)time) - (0.5f * BALL_GRAVITY * (float)time);
		ball_obj.odata.vz = ((tz - ball_obj.odata.z) / (float)time) * 0.5f;//(.4f + (.4f * tgt->_passing / 20.0f));
		ball_obj.flags &= ~BF_BOUNCEPASS;	// don't do a bounce pass
		
		ball_obj.flight_time = 0;
		ball_obj.total_time = time;
		ppdata->no_ball_collide = time;
	}	
}

//////////////////////////////////////////////////////////////////////////////
static void set_ball_vels( obj_3d *pobj, obj_3d *ptgt, int both )
{
	float	vx,vy,vz,cur_x,cur_y,cur_z,tgt_x,tgt_y,tgt_z,tgt_x2,tgt_y2,tgt_z2;
	float	dist, c_time = 0.0f, nx, nz, time_to_oob;
	float	*tm, p[3], tx = 0.0f, ty = 0.0f, tz = 0.0f,cx,cy,cz,nvel;
	int		error = 0, throw_joint, catch_joint, time, quad4, quad8, nfwd, select_catch[NUM_CATCHES], i, indx, quad_test = 0; //prob
	bbplyr_data *ppdata, *tgt;

	ppdata = (bbplyr_data *)pobj;
	tgt = (bbplyr_data *)ptgt;

	non_twopart_catch = NO;

	tm = cambot.xform;

	if(oob_corrected_catch == YES)
	{
		oob_corrected_catch = NO;
#ifdef DEBUG
		printf("oob_corrected_catch was set!!!\n");
#endif
	}

// no bounce passes on initial tipoff pass
	if(game_info.game_mode == GM_TIPOFF)
		ball_obj.flags &= ~BF_BOUNCEPASS;

	if(both == YES || pobj->flags & PF_BOTH_HANDS)
	{	// THIS SEQUENCE REQUIRES THAT THE BALL BE PASSED FROM BOTH HANDS...
	float bx,by,bz,hx,hy,hz,rhx,rhy,rhz,thx,thy,thz,lhx,lhy,lhz;

	// ball's position relative to joint's zero position
		bx = R_INHAND_X_OFF * (pobj->xscale * PSCALE);
		by = R_INHAND_Y_OFF * (pobj->yscale * PSCALE);
		bz = R_INHAND_Z_OFF * (pobj->zscale * PSCALE);

	// from screen space to world space
		throw_joint = JOINT_RWRIST;
		tx = bx * ppdata->matrices[throw_joint][0] + by * ppdata->matrices[throw_joint][1] + bz * ppdata->matrices[throw_joint][2] + ppdata->matrices[throw_joint][3];
		ty = bx * ppdata->matrices[throw_joint][4] + by * ppdata->matrices[throw_joint][5] + bz * ppdata->matrices[throw_joint][6] + ppdata->matrices[throw_joint][7];
		tz = bx * ppdata->matrices[throw_joint][8] + by * ppdata->matrices[throw_joint][9] + bz * ppdata->matrices[throw_joint][10] + ppdata->matrices[throw_joint][11];

		rhx = tx * tm[0] + ty * tm[4] + tz * tm[8] + cambot.x;
		rhy = tx * tm[1] + ty * tm[5] + tz * tm[9] + cambot.y;
		rhz = tx * tm[2] + ty * tm[6] + tz * tm[10] + cambot.z;

		bx = L_INHAND_X_OFF * PSCALE;
		by = L_INHAND_Y_OFF * PSCALE;
		bz = L_INHAND_Z_OFF * PSCALE;

		throw_joint = JOINT_LWRIST;
		tx = bx * ppdata->matrices[throw_joint][0] + by * ppdata->matrices[throw_joint][1] + bz * ppdata->matrices[throw_joint][2] + ppdata->matrices[throw_joint][3];
		ty = bx * ppdata->matrices[throw_joint][4] + by * ppdata->matrices[throw_joint][5] + bz * ppdata->matrices[throw_joint][6] + ppdata->matrices[throw_joint][7];
		tz = bx * ppdata->matrices[throw_joint][8] + by * ppdata->matrices[throw_joint][9] + bz * ppdata->matrices[throw_joint][10] + ppdata->matrices[throw_joint][11];

		lhx = tx * tm[0] + ty * tm[4] + tz * tm[8] + cambot.x;
		lhy = tx * tm[1] + ty * tm[5] + tz * tm[9] + cambot.y;
		lhz = tx * tm[2] + ty * tm[6] + tz * tm[10] + cambot.z;

		hx = ppdata->ball_in_hand == RIGHT_HAND ? rhx : lhx;
		hy = ppdata->ball_in_hand == RIGHT_HAND ? rhy : lhy;
		hz = ppdata->ball_in_hand == RIGHT_HAND ? rhz : lhz;

		thx = ppdata->ball_in_hand == RIGHT_HAND ? lhx : rhx;
		thy = ppdata->ball_in_hand == RIGHT_HAND ? lhy : rhy;
		thz = ppdata->ball_in_hand == RIGHT_HAND ? lhz : rhz;

		cur_x = (hx + thx) * 0.5f;
		cur_y = (hy + thy) * 0.5f;
		cur_z = (hz + thz) * 0.5f;
	}
	else
	{
		cur_x = ball_obj.odata.x;
		cur_y = ball_obj.odata.y;
		cur_z = ball_obj.odata.z;
	}
// get time to receiver
	dist = distxz[ppdata->plyrnum][tgt->plyrnum];

	if(dist <= 25.0f)
		c_time = 11.0f; //10 //07	// for close in passes...
	else
	{
		c_time = (MAX_PASS_TIME * (dist / MAX_PASS_DIST));
		c_time = GREATER(c_time, MIN_PASS_TIME);
	}

	if(dist <= 19.0f)
		c_time = 10.0f; //9.0f; // 07 //15.0f;	// for close in passes...

//	c_time = (MAX_PASS_TIME * (dist / MAX_PASS_DIST));
//	c_time = GREATER(c_time, MIN_PASS_TIME);

//	if(ball_obj.flags & BF_BOUNCEPASS)	- out
//		c_time *= 1.1f;	//1.2

//	if(dist <= 19.0f)
//		c_time = MIN_PASS_TIME; //11.0f; //7.0f;//15.0f;	// for close in passes...
//		c_time = 10.0f; //11.0f; //7.0f;//15.0f;	// for close in passes...

	if(pobj->flags & PF_TURBO)
	{
		c_time *= 0.93f; //0.85f; //0.95f;	//.90
		//printf("A TURBO PASS: catch_time = %d\n",(int)c_time);
	}

	if(game_info.game_mode == GM_TIPOFF)
		c_time = 50.0f;

	if(game_info.game_mode == GM_INBOUNDING)
		c_time = 30.0f;

	//printf("set_ball_vels: c_time = %f\n",c_time);

	plyr_will_be_at(&nfwd, &nx, &nz, (int)c_time, ball_obj.int_receiver);
	if(oob_corrected_catch == YES)
		nx = oob_cx, nz = oob_cz;

	if(oob_corrected_catch == NO)
	{
		catch_anim = NULL;
		catch_time = (int)c_time;
	}

// quad8 works as follows:
// passer is:
//	0-behind, 1-left.behind, 2-left, 3-left.front
//	4-front, 5-right.front, 6-right, 7-right.behind
	p[0] = nx; p[1] = 0.0f; p[2] = nz;

	quad8 = nfwd - ptangle(p, &(ppdata->odata.x));// ptgt->fwd - p2p_angles[tgt->plyrnum][ppdata->plyrnum];
	quad4 = ((quad8 + 512 + 128)& 1023) / 256;		// four directions
	quad8 = ((quad8 + 512 + 64) & 1023) / 128;		// eight directions

#ifdef DEBUG_CATCHING
	printf("nfwd = %d, nx = %3.3f, nz = %3.3f, c_time = %d\nquad4 = %s, quad8 = %s\n",nfwd, nx, nz, (int)c_time, quad4_s[quad4], quad8_s[quad8]);
#endif

	//catch_anim = NULL;	// default
	quad_test |= 1<<quad8;
	for(i = 0, indx = -1; i < NUM_CATCHES; i++)
	{	// tag legal catches
		select_catch[i] = -1;

		// is this a oob_corrected_catch...
		if(!(catches[i].quad & C_OOBCC_CATCH) && oob_corrected_catch == YES) continue;
		error = 1;
#ifdef DEBUG_CATCHING
//		printf("made past: oob_corrected catch...\n");
#endif

		// is this catch allowed in this quad...
		if(catches[i].quad != -1 && !(catches[i].quad & quad_test)) continue;
		error = 2;
#ifdef DEBUG_CATCHING
//		printf("made past: is this catch allowed in this quad...\n");
#endif

		// is there enough time for catch to take place...
		if(catches[i].min_time > 0 && catches[i].min_time > catch_time) continue;
		error = 3;
#ifdef DEBUG_CATCHING
//		printf("made past: is there enough time for catch to take place...\n");
#endif

		// is this a moving catch only...
		if(catches[i].quad & C_NON_TWOPART && (ptgt->vx == 0.0f && ptgt->vz == 0.0f)) continue;
		error = 4;
#ifdef DEBUG_CATCHING
//		printf("made past: is this a moving catch only...\n");
#endif

		// does this catch work with bounce passes...
		if(is_bounce_pass && catches[i].quad & C_NO_BOUNCE)
		{
			is_bounce_pass = NO;
			continue;
		}
		error = 5;
#ifdef DEBUG_CATCHING
//		printf("made past: does this catch work with bounce passes...\n");
#endif

		select_catch[++indx] = i;
	}
	if(indx == -1 || (tgt->odata.adata[0].animode & MODE_REACTING))
	{
		cx = tgt->jpos[JOINT_PELVIS][0];
		cy = tgt->jpos[JOINT_PELVIS][1];
		cz = tgt->jpos[JOINT_PELVIS][2];

		tgt_x = cx * tm[0] + cy * tm[4] + cz * tm[ 8] + cambot.x;
		tgt_y = cx * tm[1] + cy * tm[5] + cz * tm[ 9] + cambot.y;
		tgt_z = cx * tm[2] + cy * tm[6] + cz * tm[10] + cambot.z;

		tgt_y += 4.0f;

#ifdef DEBUG
		printf("%s will perform a PELVIS!!! c_time %3.1f: c_time >= MIN_CATCH_TIME\n", tgt->p_spd->szlast, c_time);
		printf("\t quad %s oob_corrected_catch = %d, ERROR = %d\n", quad8_s[quad8], oob_corrected_catch, error);
#endif
	}
	else
	{
		i = select_catch[randrng(1+indx)];
		catch_joint = tgt->ball_in_hand == RIGHT_HAND ? JOINT_RWRIST : JOINT_LWRIST;

		if(catches[i].quad & (C_RIGHT|C_LEFT))
			catch_joint = catches[i].quad & C_RIGHT ? JOINT_RWRIST : JOINT_LWRIST;

		if(catches[i].quad & (C_RIGHT|C_LEFT))
			tgt->ball_in_hand = catches[i].quad & C_RIGHT ? RIGHT_HAND : LEFT_HAND;

		if(catches[i].quad & C_NON_TWOPART)
			non_twopart_catch = YES;

		if(oob_corrected_catch == YES)
			nfwd = p2p_angles[tgt->plyrnum][ppdata->plyrnum];
	
		get_joint_position(ptgt, catches[i].seq, catches[i].frame, catch_joint, nfwd, &tgt_x, &tgt_y, &tgt_z);

		if(catches[i].quad & C_BOTH)
		{
			float fx,fy,fz;

			get_joint_position(ptgt, catches[i].seq, catches[i].frame, catch_joint == JOINT_RWRIST ? JOINT_LWRIST : JOINT_RWRIST, nfwd, &tgt_x2, &tgt_y2, &tgt_z2);
			fx = (tgt_x + tgt_x2) / 2;
			fy = (tgt_y + tgt_y2) / 2;
			fz = (tgt_z + tgt_z2) / 2;
			nx += fx;
			nz += fz;
			fy += ptgt->y;
			fy *= ptgt->yscale;
			tgt_x = fx; tgt_z = fz; tgt_y = fy;
		}
		else
		{
			nx += tgt_x;
			nz += tgt_z;
			tgt_y += ptgt->y;
			tgt_y *= ptgt->yscale;
		}

		if(oob_corrected_catch == YES || (ptgt->vx == 0.0f && ptgt->vz == 0.0f))
			catch_anim = (char *)((int)catches[i].s_scr | 0x80000000);
		else
			catch_anim = (char *)((int)catches[i].r_scr | 0x80000000);

#ifdef DEBUG_CATCHING
		{
		int y;

		printf("indx = %d, select_catch[%d] = %d\n",indx,indx,i);
//		for(y = 0; y < NUM_CATCHES; y++)
//			printf("select_catch[%d] = %d\n",y,select_catch[y]);
		}
#endif
	}

// check to see if a bounce pass is legal
	if(0)
	{
	// if(this == TRUE) then
		ball_obj.flags &= ~BF_BOUNCEPASS;
	// else if(this != TRUE) then	
		ball_obj.flags |= BF_BOUNCEPASS;
	}

// check to see if pass is going off the backboard
	if(ppdata->plyrnum == ball_obj.int_receiver)
	{
		if(pobj->x > 0)
		{
			p[0] = BACKBOARD_X - 2.0f;
			p[1] = 0.0f;
			p[2] = 0.0f;
			tx = BACKBOARD_X - 2.0f;
			ty = (RIM_Y + 3.0f) + BALL_RADIUS;
			tz = 0.0f;
		}
		else
		{
			p[0] = -BACKBOARD_X + 2.0f;
			p[1] = 0.0f;
			p[2] = 0.0f;
			tx = -BACKBOARD_X + 2.0f;
			ty = (RIM_Y + 3.0f) + BALL_RADIUS;
			tz = 0.0f;
		}
		catch_time = time = 25;
		ball_obj.odata.vx = vx = (tx - cur_x) / (float)time;
		ball_obj.odata.vy = vy = ((ty - cur_y) / (float)time) - (0.5f * BALL_GRAVITY * (float)time);
		ball_obj.odata.vz = vz = (tz - cur_z) / (float)time;
		bbx = ball_obj.tx = pobj->x + ((float)time + 1.0f) * ball_obj.odata.vx;
		bbz = ball_obj.tz = pobj->z + ((float)time + 1.0f) * ball_obj.odata.vz;
		bby = ball_obj.ty = ball_obj.odata.y + (((float)time + 1.0f) * vy) + (0.5f * BALL_GRAVITY * (((float)time + 1.0f) * ((float)time + 1.0f)));
		ball_obj.flags &= ~BF_BOUNCEPASS;	// don't do a bounce pass
#ifdef DEBUG_CATCHING
		fprintf(stderr, "Pass is going off the backboard\n");
#endif
		ball_obj.flight_time = 0;
		ball_obj.total_time = catch_time;
		snd_scall_bank(gameplay_bnk_str, PASS1_SND, VOLUME6, 127, PRIORITY_LVL5);
		return;
	}
	else
	{
		vx = (nx - cur_x) / (float)catch_time;
		vz = (nz - cur_z) / (float)catch_time;
		vy = ((tgt_y - cur_y) / catch_time) - (0.5f * BALL_GRAVITY * catch_time);

		bbx = ball_obj.tx = nx;
		bbz = ball_obj.tz = nz;
		bby = ball_obj.ty = tgt_y;

#ifdef DEBUG
		if(indx == -1) {
			fprintf(stderr,"For the PELVIS catch:\r\n");
			fprintf(stderr,"\t vx:%16f  vy:%16f  vz:%16f \r\n",vx,vy,vz);
			fprintf(stderr,"\t tx:%16f  ty:%16f  tz:%16f \r\n",bbx,bby,bbz);
			if(oob_corrected_catch == YES)
				fprintf(stderr,"\t Was oob_correct_catch so tx=nx=oob_cx & tz=nz=oob_cz\r\n");
		}
#endif
	}

	if(ball_obj.flags & BF_BOUNCEPASS)
	{
		if(ppdata->plyrnum == ball_obj.int_receiver)
		{
//			ball_obj.odata.vx = ((nx - cur_x) * BOUNCE_PERCENT) / (catch_time * BOUNCE_PERCENT) + ptgt->vx;
			ball_obj.odata.vy = (BALL_RADIUS - cur_y) / (catch_time * BOUNCE_PERCENT) + GRAVITY / -2.0f * (catch_time * BOUNCE_PERCENT);
//			ball_obj.odata.vz = ((nz - cur_z) * BOUNCE_PERCENT) / (catch_time * BOUNCE_PERCENT) + ptgt->vz;
		}
		else
		{
#if 1
		ball_obj.odata.vx = vx;
//		ball_obj.odata.vy = vy;
		ball_obj.odata.vz = vz;

//			ball_obj.odata.vx = ((nx - cur_x) * BOUNCE_PERCENT) / ((float)catch_time * BOUNCE_PERCENT);
			ball_obj.odata.vy = (BALL_RADIUS - cur_y) / ((float)catch_time * BOUNCE_PERCENT) - (0.5f * BALL_GRAVITY * ((float)catch_time * BOUNCE_PERCENT));
//			ball_obj.odata.vz = ((nz - cur_z) * BOUNCE_PERCENT) / ((float)catch_time * BOUNCE_PERCENT);
#else
			ball_obj.odata.vx = ((nx - cur_x) * BOUNCE_PERCENT) / (catch_time * BOUNCE_PERCENT) + ptgt->vx;
			ball_obj.odata.vy = (BALL_RADIUS - cur_y) / (catch_time * BOUNCE_PERCENT) + GRAVITY / -2.0f * (catch_time * BOUNCE_PERCENT);
			ball_obj.odata.vz = ((nz - cur_z) * BOUNCE_PERCENT) / (catch_time * BOUNCE_PERCENT) + ptgt->vz;
#endif
		}

		ball_obj.flight_time = 0;
		ball_obj.total_time = catch_time;
	}
	else
	{
		ball_obj.odata.vx = vx;
		ball_obj.odata.vy = vy;
		ball_obj.odata.vz = vz;

		ball_obj.flight_time = 0;
		ball_obj.total_time = catch_time;
	}

#ifdef DEBUG_CATCHING
	printf("SET_BALL_VELS: catch_time = %d\n",catch_time);
#endif

	if(catches[i].min_time > 0)
		catch_time = catches[i].min_time;
	else
		catch_time = catch_time	> MIN_CATCH_TIME ? MIN_CATCH_TIME : catch_time;

	if(ball_obj.total_time > 0)
	{
		ballrotv.x = (ball_obj.odata.z - ptgt->z) * 0.00025f;
		ballrotv.y = 0;
		ballrotv.z = (ball_obj.odata.x - ptgt->x) * 0.00025f;
	}

#if 1
	// DETERMINE IF RECEIVER STOPS OR GLANCES OFF THE OOB'S LINE
	pobj = (obj_3d *)tgt;
	time_to_oob = -1;
	tx = tz = 0.0f;

//#ifdef DEBUG
//	printf("\toob_corrected_catch=NO, catch_time = %d\n", catch_time);
//#endif
	oob_corrected_catch = NO;
	if(pobj->vx != 0.0f || pobj->vz != 0.0f)
	{
		//printf("receiver has vel\n");

		if(fabs(pobj->x + c_time * pobj->vx) >= OUT_OF_BOUNDS_X)
		{
			tx = (pobj->x > 0.0f) ? OUT_OF_BOUNDS_X : -OUT_OF_BOUNDS_X;
			time_to_oob = (fabs(tx) - fabs(pobj->x)) / fabs(pobj->vx);
			tz = pobj->z + time_to_oob * pobj->vz;

			//printf("X: tx = %f , tz = %f, x = %f, vx = %f, time_to_oob = %f\n",tx, tz, pobj->x, pobj->vx, time_to_oob);

#ifdef DEBUG_CATCHING
			printf("X: receiver will hit 1 wall: vel = %f\n",fsqrt((pobj->vx * pobj->vx) + (pobj->vz * pobj->vz)));
#endif
			nvel = FRELZ( 0.0f, fsqrt((pobj->vx * pobj->vx) + (pobj->vz * pobj->vz)), (pobj->tgt_fwd >= 256 && pobj->tgt_fwd <= 768) ? 512 : 0 );
			if(fabs(tz + (c_time - time_to_oob) * nvel)	>= OUT_OF_BOUNDS_Z)
			{
				if(ABS(768 - pobj->tgt_fwd) <= OOB_STOP_CATCH_RANGE || ABS(256 - pobj->tgt_fwd) <= OOB_STOP_CATCH_RANGE)
				{
#ifdef DEBUG_CATCHING
					printf("X: He'll hit 2 walls, but I'm stopping him at the first\n");
#endif
				}
				else
				{
#ifdef DEBUG_CATCHING
					printf("X: receiver will hit 2 walls\n");
#endif
					tz = (pobj->z > 0.0f) ? OUT_OF_BOUNDS_Z : -OUT_OF_BOUNDS_Z;
				}
			}
		}
		else if(fabs(pobj->z + c_time * pobj->vz) >= OUT_OF_BOUNDS_Z)
		{
			tz = (pobj->z > 0.0f) ? OUT_OF_BOUNDS_Z : -OUT_OF_BOUNDS_Z;
			time_to_oob = (fabs(tz) - fabs(pobj->z)) / fabs(pobj->vz);
			tx = pobj->x + time_to_oob * pobj->vx;

			//printf("Z: tx = %f , tz = %f, z = %f, vz = %f, time_to_oob = %f\n",tx, tz, pobj->z, pobj->vz, time_to_oob);

#ifdef DEBUG_CATCHING
			printf("Z: receiver will hit 1 wall: vel = %f\n",fsqrt((pobj->vx * pobj->vx) + (pobj->vz * pobj->vz)));
#endif
			nvel = FRELX( 0.0f, fsqrt((pobj->vx * pobj->vx) + (pobj->vz * pobj->vz)), (pobj->tgt_fwd >= 512 && pobj->tgt_fwd <= 1023) ? 768 : 256 );
			if(fabs(tx + (c_time - time_to_oob) * nvel) >= OUT_OF_BOUNDS_X)
			{
				if( ABS( 512 - ((pobj->tgt_fwd + 512) % 1024) ) <= OOB_STOP_CATCH_RANGE || ABS(512 - pobj->tgt_fwd) <= OOB_STOP_CATCH_RANGE )
				{
#ifdef DEBUG_CATCHING
					printf("Z: He'll hit 2 walls, but I'm stopping him at the first\n");
#endif
				}
				else
				{
#ifdef DEBUG_CATCHING
					printf("Z: receiver will hit 2 walls\n");
#endif
					tx = (pobj->x > 0.0f) ? OUT_OF_BOUNDS_X : -OUT_OF_BOUNDS_X;
				}
			}
		}

		if(time_to_oob > -1)
		{
			if(
	// TOP
				 ( tz - (-OUT_OF_BOUNDS_Z) <= 0.7f && pobj->tgt_fwd > 256 && pobj->tgt_fwd < 768 && ABS(512 - pobj->tgt_fwd) <= OOB_STOP_CATCH_RANGE)
	// BOTTOM
			|| ( (tz - OUT_OF_BOUNDS_Z) >= -0.7f && ((pobj->tgt_fwd > 768 && pobj->tgt_fwd <= 1023) || (pobj->tgt_fwd >= 0 && pobj->tgt_fwd < 256))
			&& ABS( 512 - ((pobj->tgt_fwd + 512) % 1024) ) <= OOB_STOP_CATCH_RANGE)
	// LEFT
			|| ( tx - (-OUT_OF_BOUNDS_X) <= 0.7f && pobj->tgt_fwd > 512 && pobj->tgt_fwd < 1023 && ABS(768 - pobj->tgt_fwd) <= OOB_STOP_CATCH_RANGE)
	// RIGHT
			|| ( (tx - OUT_OF_BOUNDS_X) >= -0.7f && pobj->tgt_fwd > 0 && pobj->tgt_fwd < 512 && ABS(256 - pobj->tgt_fwd) <= OOB_STOP_CATCH_RANGE)
				)
			{
#ifdef DEBUG_CATCHING
				printf("doin it - c_time = %f, time_to_oob = %f\n",c_time, time_to_oob);
#endif
				get_joint_position(pobj, SEQ_C_FWD1_B, 27, JOINT_RWRIST, ptangle(&(pobj->x), &ball_obj.odata.x), &tgt_x,  &tgt_y,  &tgt_z);
				get_joint_position(pobj, SEQ_C_FWD1_B, 27, JOINT_LWRIST, ptangle(&(pobj->x), &ball_obj.odata.x), &tgt_x2, &tgt_y2, &tgt_z2);
				fx = (tgt_x + tgt_x2) / 2.0f;
				fy = (tgt_y + tgt_y2) / 2.0f;
				fz = (tgt_z + tgt_z2) / 2.0f;
				tx += fx;
				tz += fz;
				fy += ptgt->y;
				fy *= ptgt->yscale;
				tgt_x = fx; tgt_z = fz; tgt_y = fy;

				ball_obj.odata.vx = (tx - cur_x) / c_time;
				ball_obj.odata.vz = (tz - cur_z) / c_time;
				ball_obj.odata.vy = ((tgt_y - cur_y) / c_time) - (0.5f * BALL_GRAVITY * c_time);

				//catch_time = (int)(c_time - time_to_oob);
				catch_time = GREATER(0, (int)(c_time - time_to_oob));
				catch_anim = (char *)((int)catch_stand_forward_2 | 0x80000000);
				oob_corrected_catch = YES;
#ifdef DEBUG
				printf(">>> oob_corrected_catch = YES,  time_to_oob = %f\n",time_to_oob);
#endif
			}
		}
	}
#endif

#ifdef DEBUG
if(ball_obj.total_time <= 0)
	printf("3: ball_obj.total_time <= 0\n");
#endif

#ifdef DEBUG_CATCHING
	printf("LEAVING: SET_BALL_VELS\n");
#endif
}

//////////////////////////////////////////////////////////////////////////////
// figure out where the player will be based on current velocities and time
void plyr_will_be_at(int *nfwd, float *rx, float *rz, int catch_time, int plyr)
{
int angle;
float tx, tz, time_to_oob, vx = 0.0f, vz = 0.0f, vel, xvel, zvel;
obj_3d *pobj = (obj_3d *)(player_blocks + plyr);

	angle = (int)((atan2(pobj->vx, pobj->vz) / M_PI) * 512.0f);
	angle = angle < 0 ? 1024 + angle : angle;
	angle = (vx == 0.0f && vz == 0.0f) ? 0 : angle;

	*nfwd = pobj->tgt_fwd;

	vel = fsqrt((pobj->vx * pobj->vx) + (pobj->vz * pobj->vz));

	tx = pobj->x + catch_time * pobj->vx;
	if(fabs(tx) >= OUT_OF_BOUNDS_X)
	{
		tx = (tx > 0.0f) ? OUT_OF_BOUNDS_X : -OUT_OF_BOUNDS_X;
		time_to_oob = (tx - pobj->x) / pobj->vx;
		tz = pobj->z + time_to_oob * pobj->vz;

		vx = 0.0f;
		if(pobj->vz != 0.0f)
		{
			//vz = FRELZ( 0.0f, (((bbplyr_data *)pobj)->_speed * (pobj->flags & PF_TURBO ? 1.0f : 0.90f)), pobj->vz > 0.0f ? 0 : 512 );
			vz = FRELZ( 0.0f, vel, pobj->vz > 0.0f ? 0 : 512 );
			*nfwd = pobj->vz > 0.0f ? 0 : 512;
		}
#ifdef DEBUG_PASSING
		printf("X OUT: corrected z angle %d\n", pobj->vz > 0.0f ? 0 : 512);
#endif
		tz += vz * ((float)catch_time - time_to_oob);
		tz = LESSER(tz, OUT_OF_BOUNDS_Z);
		tz = GREATER(tz,-OUT_OF_BOUNDS_Z);

#ifdef DEBUG_PASSING
		printf("X OUT: time_to_oob %d ticks, catch_time %d, OX %3.3f, OZ %3.3f, NX %3.3f NZ %3.3f\n", (int)time_to_oob, catch_time, ball_obj.tx, ball_obj.tz, tx, tz);
#endif
		*rx = ball_obj.tx = tx;
		*rz = ball_obj.tz = tz;
		//ball_obj.odata.vx = (tx - ball_obj.odata.x) / catch_time;
		//ball_obj.odata.vz = (tz - ball_obj.odata.z) / catch_time;
		//ball_obj.odata.vy = (tgt_y - cur_y) / root1 + BALL_GRAVITY / -2.0f * root1;
		return;
	}
	tz = pobj->z + catch_time * pobj->vz;
	if(fabs(tz) >= OUT_OF_BOUNDS_Z)
	{
		tz = (tz > 0.0f) ? OUT_OF_BOUNDS_Z : -OUT_OF_BOUNDS_Z;
		time_to_oob = (tz - pobj->z) / pobj->vz;
		tx = pobj->x + time_to_oob * pobj->vx;

		vz = 0.0f;
		if(pobj->vx != 0.0f)
		{
			//vx = FRELX( 0.0f, (((bbplyr_data *)pobj)->_speed * (pobj->flags & PF_TURBO ? 1.0f : 0.90f)), pobj->vx > 0.0f ? 256 : 768 );
			vx = FRELX( 0.0f, vel, pobj->vx > 0.0f ? 256 : 768 );
			*nfwd = pobj->vx > 0.0f ? 256 : 768;
		}
#ifdef DEBUG_PASSING
		printf("Z OUT: corrected x angle %d\n", pobj->vx > 0.0f ? 256 : 768);
#endif
		tx += vx * ((float)catch_time - time_to_oob);
		tx = LESSER(tx, OUT_OF_BOUNDS_X);
		tx = GREATER(tx,-OUT_OF_BOUNDS_X);

#ifdef DEBUG_PASSING
		printf("Z OUT: time_to_oob %d ticks, catch_time %d, OX %3.3f, OZ %3.3f, NX %3.3f NZ %3.3f\n", (int)time_to_oob, catch_time, ball_obj.tx, ball_obj.tz, tx, tz);
#endif
		*rx = ball_obj.tx = tx;
		*rz = ball_obj.tz = tz;
		//ball_obj.odata.vx = (tx - ball_obj.odata.x) / (float)catch_time;
		//ball_obj.odata.vz = (tz - ball_obj.odata.z) / (float)catch_time;
		//ball_obj.odata.vy = (tgt_y - cur_y) / root1 + BALL_GRAVITY / -2.0f * root1;
		return;
	}

	*rx = ball_obj.tx = tx;
	*rz = ball_obj.tz = tz;
	//ball_obj.odata.vx = (tx - ball_obj.odata.x) / (float)catch_time;
	//ball_obj.odata.vz = (tz - ball_obj.odata.z) / (float)catch_time;
#ifdef DEBUG_PASSING
	printf("RECEIVER WILL STAY INBOUNDS\n");
#endif
}

//////////////////////////////////////////////////////////////////////////////
int receiver_pass_timeout(obj_3d *pobj)
{

	if(game_info.ball_handler == NO_BALLHANDLER)
	{
		if(game_info.ball_mode == BM_PASSING)
			return NO;
		else
			return YES;
	}
	else
	{
		return YES;
	}
}

//////////////////////////////////////////////////////////////////////////////
int plyr_have_ball_hitgnd(obj_3d *pobj)
{

	if(game_info.ball_handler == ((bbplyr_data *)pobj)->plyrnum || pobj->y <= 0.0f)
		return YES;
	else
		return NO;
}

//////////////////////////////////////////////////////////////////////////////
int plyr_have_ball(obj_3d *pobj)
{

	return (game_info.ball_handler == ((bbplyr_data *)pobj)->plyrnum);
}

//////////////////////////////////////////////////////////////////////////////
float dist3d( float *p1, float *p2 )
{
	float	dx,dy,dz;

	dx = p2[0] - p1[0];
	dy = p2[1] - p1[1];
	dz = p2[2] - p1[2];

	return fsqrt(dx*dx + dy*dy + dz*dz);
}

//////////////////////////////////////////////////////////////////////////////
float odistxz( obj_3d *o1, obj_3d *o2 )
{
	return fsqrt((o2->x - o1->x) * (o2->x - o1->x) +
				 (o2->z - o1->z) * (o2->z - o1->z));
}

//////////////////////////////////////////////////////////////////////////////
// measures angle between two 3d objects.  USES THEIR TRANSFORMED SCREEN
// COORDINATES!
#if 0
static int plyrangle( obj_3d *obj1, obj_3d *obj2 )
{
	float	dy,h,sin;
	int		angle;
	bbplyr_data *o1 = (bbplyr_data *)obj1;
	bbplyr_data *o2 = (bbplyr_data *)obj2;

	h = fsqrt((o2->sx - o1->sx) * (o2->sx - o1->sx) + (o2->sy - o1->sy) * (o2->sy - o1->sy));

	dy = fabs( o2->sy - o1->sy );
	sin = dy / h;

#if THROW_DEBUG
//	fprintf( stderr, "qb: %f %f   tgt: %f %f  h: %f dx: %f sin: %f7\n", o1->sx, o1->sy, o2->sx, o2->sy, h, dy, sin );
#endif

	angle = iasin( sin );

	if( o2->sx < o1->sx )
	{
		if( o2->sy < o1->sy )
			return( 512 + angle );
		else
			return( 512 - angle);
	}
	else
	{
		if( o2->sy < o1->sy )
			return( 1024 - angle );
		else
			return( angle );
	}

	return( angle );

}
#endif

//////////////////////////////////////////////////////////////////////////////
// draw_jersey_numbers()
//
// ENTRY: draw modes must be set
// LEAVE: modifies depth bias, texture clamp & source
//
void draw_jersey_numbers( int plyrnum, const void * spds, void * decals, float * mat )
{
	const struct stat_plyr_data * p_spd = spds;
	Texture_node_t ** pdecal = decals;
	jonhey_t jh_obj;


	
	LIMB ** jnum_mod;
	unsigned char j1, j2;

	grTexClampMode( 0, GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP );

	grDepthBiasLevel( -8L );


	j1 = (unsigned char)p_spd->number >> 4;
	j2 = (unsigned char)p_spd->number & 0x0F;

	jnum_mod = player_blocks[plyrnum].jnum_model;
	if (jnum_mod != NULL)
	{
		if ((j1 != 0) || (j2 == 0))
			{
			convert_2_jonhey (&jh_obj, jnum_mod[1], mat, 
							&pdecal[PLAYER_TEXTURE_JERSEY_NUMBER_INDEX] );
			render_limb_jers(&jh_obj, j2, j1);
			}
		else
			{
			convert_2_jonhey (&jh_obj, jnum_mod[0], mat, 
							&pdecal[PLAYER_TEXTURE_JERSEY_NUMBER_INDEX] );
			render_limb_jers(&jh_obj, j2, j2);
			}
	}
	grTexClampMode( 0, GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP );
}

//////////////////////////////////////////////////////////////////////////////
int shoe_color_flags[] = {SN_TURBO_COLOR_BLUE, SN_TURBO_COLOR_GREEN, SN_TURBO_COLOR_RED, SN_TURBO_COLOR_YELLOW};

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
void draw_player( void *oi )
{
	bbplyr_data * ppdata = oi;
	obj_3d * pobj = oi;
	LIMB ** body;
	LIMB * tmp_hand_r;
	LIMB * tmp_hand_l;
//	LIMB * tmp_neck;
	float mat[12],cm[12];
	int shoe = 0;

//#ifndef DEBUG_HEADS
	if (!(pobj->flags & PF_ONSCREEN))
		return;
//#endif

	// generate player position & orientation matrix
	roty( pobj->fwd, mat );
	mat[ 3] = pobj->x - cambot.x;
	mat[ 7] = pobj->y + pobj->ay - cambot.y;
	mat[11] = pobj->z - cambot.z;

	mxm( cambot.xform, mat, cm );
	cm[0] *= pobj->xscale; cm[1] *= pobj->yscale; cm[ 2] *= pobj->zscale;
	cm[4] *= pobj->xscale; cm[5] *= pobj->yscale; cm[ 6] *= pobj->zscale;
	cm[8] *= pobj->xscale; cm[9] *= pobj->yscale; cm[10] *= pobj->zscale;

	// select proper model
	body = ppdata->body_model;

	if(pobj->flags & PF_FLASHWHITE)
	{
		grConstantColorValue( (128 << 24) );
		guColorCombineFunction(GR_COLORCOMBINE_DECAL_TEXTURE_ADD_CCALPHA);
	}
	else if(pobj->flags & PF_FLASHRED)
	{
		grConstantColorValue( (0<<24) + (255<<16) + (30<<8) + 30);
		guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB);
	}
	else
	{
		// Set any turbo button shoe coloring
		if((ppdata->but_cur & P_C)
			&& game_info.plyr_control[ppdata->plyrnum] >= 0
			&& game_info.game_mode != GM_PRE_TIPOFF
			&& game_info.game_mode != GM_TIPOFF
			&& game_info.game_mode != GM_FREE_THROW
			&& ((priv_stealth_turbo & (1 << ppdata->plyrnum)) == 0))
		{
			shoe = shoe_color_flags[game_info.plyr_control[ppdata->plyrnum]];
			bbplyr_skel[JOINT_LANKLE].flags |= (shoe|SN_TURBO_LIGHT);
			bbplyr_skel[JOINT_RANKLE].flags |= (shoe|SN_TURBO_LIGHT);
		}

		grConstantColorValue( (255 << 24) );
		guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
	}

	grDepthBiasLevel(0);
													 
	guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);
	grAlphaBlendFunction( GR_BLEND_SRC_ALPHA, GR_BLEND_ONE_MINUS_SRC_ALPHA, GR_BLEND_SRC_ALPHA,	GR_BLEND_ONE_MINUS_SRC_ALPHA );

	// Save default hands for later so we don't crash in the unload-reload!
	tmp_hand_r = body[JOINT_RWRIST];
	tmp_hand_l = body[JOINT_LWRIST];
//	tmp_neck   = body[JOINT_NECK];

	// Give this player a head on his shoulders
	if( ppdata->f_headsc != 0.0f )
		body[JOINT_NECK] = ppdata->head_model;
	else
		body[JOINT_NECK] = body[JOINT_PELVIS];

	if (ppdata->body_model == plyr_limbs_rm)
	{
		ppdata->hands = get_some_hands(pobj);
		body[JOINT_RWRIST] = hand_sets[(ppdata->hands & MASK_RIGHT_HAND)];
		body[JOINT_LWRIST] = hand_sets[(ppdata->hands & MASK_LEFT_HAND)>>BITS_HAND];
	}

	render_node_mpc( bbplyr_skel, body, ppdata->matrices[0], pobj->pdecal, ppdata->jpos[0]);

	// Put back the default hands
	body[JOINT_RWRIST] = tmp_hand_r;
	body[JOINT_LWRIST] = tmp_hand_l;

	// Clr any turbo button shoe coloring
	bbplyr_skel[JOINT_LANKLE].flags &= ~(SN_TURBO_ALLCOLORS|SN_TURBO_LIGHT);
	bbplyr_skel[JOINT_RANKLE].flags &= ~(SN_TURBO_ALLCOLORS|SN_TURBO_LIGHT);

	draw_jersey_numbers( ppdata->plyrnum, ppdata->p_spd, pobj->pdecal, ppdata->matrices[1] );

	// Leave with default mode set
	grConstantColorValue( (255 << 24) );
	guColorCombineFunction( GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA );
}

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
void debug_draw_player( void *oi )
{
	LIMB * tmp_hand_r;
	LIMB * tmp_hand_l;
	float	mat[12],cm[12];
	LIMB **body;
	int		shoe;
	obj_3d	*pobj;
	bbplyr_data	*ppdata;

	pobj = (obj_3d *)oi;
	ppdata = (bbplyr_data *)oi;

	if(!(pobj->flags & PF_ONSCREEN))
		return;

	// generate player position & orientation matrix
	roty( pobj->fwd, mat );
	mat[3] = pobj->x - cambot.x;
	mat[7] = pobj->y + pobj->ay - cambot.y;
	mat[11] = pobj->z - cambot.z;

	mxm( cambot.xform, mat, cm );
	cm[0] *= pobj->xscale; cm[1] *= pobj->yscale; cm[ 2] *= pobj->zscale;
	cm[4] *= pobj->xscale; cm[5] *= pobj->yscale; cm[ 6] *= pobj->zscale;
	cm[8] *= pobj->xscale; cm[9] *= pobj->yscale; cm[10] *= pobj->zscale;

	// select proper model
	body = ppdata->body_model;

	grConstantColorValue( (255 << 24) );
	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);

	grDepthBiasLevel(0);

	guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);
	grAlphaBlendFunction( GR_BLEND_SRC_ALPHA, GR_BLEND_ONE_MINUS_SRC_ALPHA, GR_BLEND_SRC_ALPHA,	GR_BLEND_ONE_MINUS_SRC_ALPHA );

	// Save default hands for later so we don't crash in the unload-reload!
	tmp_hand_r = body[JOINT_RWRIST];
	tmp_hand_l = body[JOINT_LWRIST];

	// Give this player a head on his shoulders
	if( ppdata->f_headsc != 0.0f )
		body[JOINT_NECK] = ppdata->head_model;
	else
		body[JOINT_NECK] = body[JOINT_PELVIS];

	if(ppdata->body_model == plyr_limbs_rm)
	{
		ppdata->hands = get_some_hands(pobj);
		body[JOINT_RWRIST] = hand_sets[(ppdata->hands & MASK_RIGHT_HAND)];
		body[JOINT_LWRIST] = hand_sets[(ppdata->hands & MASK_LEFT_HAND)>>BITS_HAND];
	}

	render_node_mpc( bbplyr_skel, body, ppdata->matrices[0], pobj->pdecal, ppdata->jpos[0]);

	// Put back the default hands
	body[JOINT_RWRIST] = tmp_hand_r;
	body[JOINT_LWRIST] = tmp_hand_l;

	draw_jersey_numbers( ppdata->plyrnum, ppdata->p_spd, pobj->pdecal, ppdata->matrices[1] );
}

//////////////////////////////////////////////////////////////////////////////
#define JNUM(x) ((x) ? ((x)-1) : 9 )
void draw_player2( void *oi )
{

	float	xlate[12] = { 1.0f, 0.0f, 0.0f, 0.0f,
	 		  			  0.0f, 1.0f, 0.0f, 0.0f,
						  0.0f, 0.0f, 1.0f, 0.0f };

	float	rotate[12] = { 1.0f, 0.0f, 0.0f, 0.0f,
						   0.0f, 1.0f, 0.0f, 0.0f,
						   0.0f, 0.0f, 1.0f, 0.0f };

	float	cm[12];

	LIMB **body;
	obj_3d	*pobj;
	bbplyr_data	*ppdata;
	//int		j1,j2;
	//LIMB *tmplimb;
	LIMB *tmp_hand_r;
	LIMB *tmp_hand_l;

	pobj = (obj_3d *)oi;
	ppdata = (bbplyr_data *)oi;

	body = ppdata->body_model;

	grDepthBufferMode( GR_DEPTHBUFFER_WBUFFER );
	grDepthBufferFunction( GR_CMP_LESS );
	grDepthMask( FXTRUE );
	grDepthBiasLevel( -8L );

	bbplyr_skel[JOINT_RANKLE].flags &= ~SN_TURBO_LIGHT;
	bbplyr_skel[JOINT_LANKLE].flags &= ~SN_TURBO_LIGHT;

	if (!(p_status & (1<<ppdata->plyrnum)))
//	if (ISDRONE(ppdata))
	{
		grConstantColorValue( (255<<24) + (128<<16) + (128<<8) + 128);
		guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB);
//		guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
	}
	else
	{
		grConstantColorValue( (255<<24) + (255<<16) + (255<<8) + 255);
		guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
	}

	guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);
	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
	grTexClampMode(0,GR_TEXTURECLAMP_WRAP,GR_TEXTURECLAMP_WRAP);

	grAlphaBlendFunction( GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA,
			GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA );

	grChromakeyMode(GR_CHROMAKEY_DISABLE);

//	grDepthBiasLevel(0L);

	// Save default hands for later so we don't crash in the unload-reload!
	tmp_hand_r = body[JOINT_RWRIST];
	tmp_hand_l = body[JOINT_LWRIST];

	// Give this player a head on his shoulders
	if( ppdata->f_headsc != 0.0f )
		body[JOINT_NECK] = ppdata->head_model;
	else
		body[JOINT_NECK] = body[JOINT_PELVIS];

	if (ppdata->body_model == plyr_limbs_rm)
	{
		ppdata->hands = get_some_hands(pobj);
		body[JOINT_RWRIST] = hand_sets[(ppdata->hands & MASK_RIGHT_HAND)];
		body[JOINT_LWRIST] = hand_sets[(ppdata->hands & MASK_LEFT_HAND)>>BITS_HAND];
	}

	bbplyr_skel[JOINT_NECK].flags |= SN_HEAD_SCALE;	// somewhere this bit is being cleared ! ! !

	xlate[3] = pobj->x;
	xlate[7] = pobj->y + pobj->ay;
	xlate[11] = pobj->z;
	roty(pobj->fwd,rotate);
	mxm(xlate, rotate, cm);

	cm[0] *= pobj->xscale; cm[1] *= pobj->yscale; cm[ 2] *= pobj->zscale;
	cm[4] *= pobj->xscale; cm[5] *= pobj->yscale; cm[ 6] *= pobj->zscale;
	cm[8] *= pobj->xscale; cm[9] *= pobj->yscale; cm[10] *= pobj->zscale;

	generate_matrices(bbplyr_skel, cm, pobj->adata[0].pq, ppdata->matrices[0], ppdata->jpos[0], ppdata->f_headsc, ppdata->f_headsc);

	render_node_mpc(bbplyr_skel, body, ppdata->matrices[0], pobj->pdecal, ppdata->jpos[0]);

	draw_jersey_numbers( ppdata->plyrnum, ppdata->p_spd, pobj->pdecal, ppdata->matrices[1] );

	bbplyr_skel[JOINT_RANKLE].flags |= SN_TURBO_LIGHT;
	bbplyr_skel[JOINT_LANKLE].flags |= SN_TURBO_LIGHT;

	// Put back the default hands
	body[JOINT_RWRIST] = tmp_hand_r;
	body[JOINT_LWRIST] = tmp_hand_l;
}

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
void draw_referee( void *oi )
{
//	LIMB * tmp_hand_r;
//	LIMB * tmp_hand_l;
	float	mat[12],cm[12];
	LIMB **body;
	obj_3d	*pobj;
	bbplyr_data	*ppdata;

	pobj = (obj_3d *)oi;
	ppdata = (bbplyr_data *)oi;

	if (!(pobj->flags & PF_ONSCREEN))
	{
		if (game_info.game_mode != GM_PRE_TIPOFF &&
			game_info.game_mode != GM_TIPOFF)
			hide_multiple_objects( OID_REFEREE, -1 );
		return;
	}

	// generate player position & orientation matrix
	roty( pobj->fwd, mat );
	mat[3] = pobj->x - cambot.x;
	mat[7] = pobj->y + pobj->ay - cambot.y;
	mat[11] = pobj->z - cambot.z;

	mxm( cambot.xform, mat, cm );
	cm[0] *= pobj->xscale; cm[1] *= pobj->yscale; cm[ 2] *= pobj->zscale;
	cm[4] *= pobj->xscale; cm[5] *= pobj->yscale; cm[ 6] *= pobj->zscale;
	cm[8] *= pobj->xscale; cm[9] *= pobj->yscale; cm[10] *= pobj->zscale;

	// select proper model
	body = ppdata->body_model;

	grConstantColorValue( (255 << 24) );
	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
	grDepthBiasLevel(0);
	guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);
	grAlphaBlendFunction(GR_BLEND_SRC_ALPHA, GR_BLEND_ONE_MINUS_SRC_ALPHA, GR_BLEND_SRC_ALPHA,	GR_BLEND_ONE_MINUS_SRC_ALPHA);

	// Give this player a head on his shoulders
	if( ppdata->f_headsc != 0.0f )
		body[JOINT_NECK] = ppdata->head_model;
	else
		body[JOINT_NECK] = body[JOINT_PELVIS];

	render_node_mpc( bbplyr_skel, body, ppdata->matrices[0], pobj->pdecal, ppdata->jpos[0]);
}

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------


void render_node_mpc_reflect( SNODE *limb, LIMB **model, float *matrix,
							Texture_node_t **decal_array, float *jpos, float *inv_camera );

void draw_player_reflection(void *oi)
{
	LIMB *reflect_limbs[15];
	obj_3d	*pobj;
	bbplyr_data	*ppdata;
#if defined(VEGAS)
	Texture_node_t	*rdecals[4];
#else
	GrMipMapId_t rdecals[4];
#endif
	pobj = (obj_3d *)oi;
	ppdata = (bbplyr_data *)oi;
	
	reflect_limbs[JOINT_PELVIS] = ppdata->body_model[JOINT_PELVIS];
	reflect_limbs[JOINT_TORSO] = ppdata->body_model[JOINT_TORSO];
	reflect_limbs[JOINT_RSHOULDER] = ppdata->body_model[JOINT_RSHOULDER];
	reflect_limbs[JOINT_RELBOW] = ppdata->body_model[JOINT_RELBOW];
	reflect_limbs[JOINT_RWRIST] = ppdata->body_model[JOINT_RWRIST];
	reflect_limbs[JOINT_LSHOULDER] = ppdata->body_model[JOINT_LSHOULDER];
	reflect_limbs[JOINT_LELBOW] = ppdata->body_model[JOINT_LELBOW];
	reflect_limbs[JOINT_LWRIST] = ppdata->body_model[JOINT_LWRIST];
	reflect_limbs[JOINT_NECK] = ppdata->body_model[JOINT_NECK];
	reflect_limbs[JOINT_RHIP] = ppdata->body_model[JOINT_RHIP];
	reflect_limbs[JOINT_RKNEE] = ppdata->reflect_model[3];
	reflect_limbs[JOINT_RANKLE] = ppdata->reflect_model[1];
	reflect_limbs[JOINT_LHIP] = ppdata->body_model[JOINT_LHIP];
	reflect_limbs[JOINT_LKNEE] = ppdata->reflect_model[2];
	reflect_limbs[JOINT_LANKLE] = ppdata->reflect_model[0];
	
	if (!(pobj->flags & PF_ONSCREEN))
		return;
	
	grDepthBiasLevel(0);
	
	bothalf_skel[JOINT_PELVIS].flags |= SN_NODRAW;
	bothalf_skel[JOINT_RHIP].flags   |= SN_NODRAW;
	bothalf_skel[JOINT_LHIP].flags   |= SN_NODRAW;		 
	
	rdecals[0] = pobj->pdecal[4];
	rdecals[1] = pobj->pdecal[4];
	rdecals[2] = pobj->pdecal[4];
	rdecals[3] = pobj->pdecal[4];
	
	render_node_mpc_reflect(bothalf_skel, reflect_limbs, ppdata->matrices_flipped[0], rdecals, ppdata->jpos[0],ppdata->inv_camera[0]);
	bothalf_skel[JOINT_PELVIS].flags &= ~SN_NODRAW;
	bothalf_skel[JOINT_RHIP].flags   &= ~SN_NODRAW;
	bothalf_skel[JOINT_LHIP].flags   &= ~SN_NODRAW;
}

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
void flash_plyr( obj_3d *pobj, int plyr)
{
	if ((game_info.ball_handler == NO_BALLHANDLER) && !(pobj->flags & (PF_ALLYDUNKER)))
		return;

	// dont flash dude...near end-of-qtr to avoid staying white
	if (game_info.game_time <= 0x00000b00)
		return;

	qcreate ("PlayerFlashW", PLAYER_PID, flash_plyr_proc, ((plyr == -1) ? ((bbplyr_data *)(pobj))->plyrnum : plyr), 0, 0, 0);
}

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
void flash_plyr_proc(int *args)
{
int i;
obj_3d *pobj;

	pobj = (obj_3d *)(player_blocks + args[0]);

	for(i = 0; i < 4; i++)
	{
		pobj->flags |= PF_FLASHWHITE;
		sleep(3);
		pobj->flags &= ~PF_FLASHWHITE;
		sleep(3);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
void flash_plyr_red( obj_3d *pobj, int plyr)
{
	// dont flash dude...near end-of-qtr to avoid staying red
	if (game_info.game_time <= 0x00000b00)
		return;

	qcreate ("PlayerFlashR", PLAYER_PID, flash_plyr_red_proc, ((plyr == -1) ? ((bbplyr_data *)(pobj))->plyrnum : plyr), 0, 0, 0);
}

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
void flash_plyr_red_proc(int *args)
{
int i;
obj_3d *pobj;

	pobj = (obj_3d *)(player_blocks + args[0]);

	for(i = 0; i < 4; i++)
	{
		pobj->flags |= PF_FLASHRED;
		sleep(3);
		pobj->flags &= ~PF_FLASHRED;
		sleep(3);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
#if 0
void shadow_player_draw(int plyr)
{
	LIMB **body;
int dir, seq, frame;
float x,y,z, scalex, scaley,scalez;
bbplyr_data *ppdata;
obj_3d *pobj;

	ppdata = player_blocks + plyr;
	pobj = (obj_3d *)ppdata;

	dir   = ppdata->odata.fwd;
	body = ppdata->body_model;
	seq   = pobj->adata[0].seq_index;
		frame = pobj->adata[0].iframe;

	x = pobj->x;
	y = pobj->y;
	z = pobj->z;
	scalex = pobj->xscale; 
	scaley = pobj->yscale; 
	scalez = pobj->zscale; 

	get_frame(seq, frame, qpq, NULL);
	//roty(dir, qmat);
	rotxyz(0, ((dir /* + 512*/) % 1024), 512, qmat);
	qmat[3] = x + anix - cambot.x;
	qmat[7] = (y + /*0.0f aniy*/ - cambot.y) - aniy;
	qmat[11] = z + aniz - cambot.z;
	mxm(qcam2, qmat, qcm);

	qcm[0] *= scalex; qcm[1] *= scaley; qcm[ 2] *= scalez;
	qcm[4] *= scalex; qcm[5] *= scaley; qcm[ 6] *= scalez;
	qcm[8] *= scalex; qcm[9] *= scaley; qcm[10] *= scalez;

	generate_matrices(bbplyr_skel, qcm, qpq, qmatrices[0], qjpos[0], ppdata->f_headsc, ppdata->f_headsc);

	//grConstantColorValue( (255 << 24) );
	grConstantColorValue( (255<<24) + (0<<16) + (0<<8) + 200);
	//guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB);
	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);

//	grDepthBufferMode( GR_DEPTHBUFFER_DISABLE );
	grDepthBufferMode( GR_DEPTHBUFFER_WBUFFER );
	grDepthBufferFunction( GR_CMP_GREATER );
	grDepthMask( FXTRUE );

	bbplyr_skel[JOINT_NECK].flags      |= SN_NODRAW;
	bbplyr_skel[JOINT_TORSO].flags     |= SN_NODRAW;
	bbplyr_skel[JOINT_PELVIS].flags    |= SN_NODRAW;

	bbplyr_skel[JOINT_RSHOULDER].flags |= SN_NODRAW;
	bbplyr_skel[JOINT_RWRIST].flags    |= SN_NODRAW;
	bbplyr_skel[JOINT_RELBOW].flags    |= SN_NODRAW;
	bbplyr_skel[JOINT_RHIP].flags      |= SN_NODRAW;

	bbplyr_skel[JOINT_LSHOULDER].flags |= SN_NODRAW;
	bbplyr_skel[JOINT_LWRIST].flags    |= SN_NODRAW;
	bbplyr_skel[JOINT_LELBOW].flags    |= SN_NODRAW;
	bbplyr_skel[JOINT_LHIP].flags      |= SN_NODRAW;

	grDepthBiasLevel( 0 );
	render_node_mpc(bbplyr_skel, body, qmatrices[0], pobj->pdecal, qjpos[0]);

	bbplyr_skel[JOINT_NECK].flags      &= ~SN_NODRAW;
	bbplyr_skel[JOINT_TORSO].flags     &= ~SN_NODRAW;
	bbplyr_skel[JOINT_PELVIS].flags    &= ~SN_NODRAW;

	bbplyr_skel[JOINT_RSHOULDER].flags &= ~SN_NODRAW;
	bbplyr_skel[JOINT_RWRIST].flags    &= ~SN_NODRAW;
	bbplyr_skel[JOINT_RELBOW].flags    &= ~SN_NODRAW;
	bbplyr_skel[JOINT_RHIP].flags      &= ~SN_NODRAW;

	bbplyr_skel[JOINT_LSHOULDER].flags &= ~SN_NODRAW;
	bbplyr_skel[JOINT_LWRIST].flags    &= ~SN_NODRAW;
	bbplyr_skel[JOINT_LELBOW].flags    &= ~SN_NODRAW;
	bbplyr_skel[JOINT_LHIP].flags      &= ~SN_NODRAW;
}
#endif

//////////////////////////////////////////////////////////////////////////////
void ghost_draw_proc(int *args)
{
int i, plyr = args[0];

	player_blocks[plyr].ghost = YES;
	for(i = 0; i < 15; i++)
	{
		qcreate("ghost_draw", PLAYER_PID + 4, ghost_draw, plyr, i, 0, 0);
		//sleep(2);
		sleep(2);
	}
	player_blocks[plyr].ghost = NO;
}

//////////////////////////////////////////////////////////////////////////////
void ghost_draw(int *args)	//int plyr, int time)
{
	LIMB **body;
int i, dir, seq, frame, plyr = args[0];
float x,y,z, scalex,scaley,scalez;
bbplyr_data *ppdata;

obj_3d *pobj;

	ppdata = player_blocks + plyr;
	pobj = (obj_3d *)ppdata;

	dir   = ppdata->odata.fwd;
	body = ppdata->body_model;
	seq   = pobj->adata[0].seq_index;
	frame = pobj->adata[0].iframe;

	x = pobj->x;
	y = pobj->y;
	z = pobj->z;
	scalex = pobj->xscale;// - ((float)args[1] / 7.0f); 
	scaley = pobj->yscale;// - ((float)args[1] / 7.0f); 
	scalez = pobj->zscale;// - ((float)args[1] / 7.0f); 

	for(i = 0; i < 3; i++)
	//for(i = 0; i < 5; i++)
	{
		get_frame(seq, frame, qpq, NULL);
		roty(dir, qmat);
		qmat[3] = x + anix - cambot.x;
		qmat[7] = y + aniy - cambot.y;
		qmat[11] = z + aniz - cambot.z;
		mxm(qcam2, qmat, qcm);

		qcm[0] *= scalex; qcm[1] *= scaley; qcm[ 2] *= scalez;
		qcm[4] *= scalex; qcm[5] *= scaley; qcm[ 6] *= scalez;
		qcm[8] *= scalex; qcm[9] *= scaley; qcm[10] *= scalez;

		generate_matrices(bbplyr_skel, qcm, qpq, qmatrices[0], qjpos[0], ppdata->f_headsc, ppdata->f_headsc);

		//grConstantColorValue( (255 << 24) );
		grConstantColorValue( (255<<24) + (0<<16) + (0<<8) + 200);
		//guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB);
		guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);

		render_node_mpc(bbplyr_skel, body, qmatrices[0], pobj->pdecal, qjpos[0]);
		sleep(1);
	}
}

//////////////////////////////////////////////////////////////////////////////
void ghosting(obj_3d *pobj, int what, int doball)
{
//int k,kk;

	if(what == YES)
		qcreate("ghost_draw_proc", PLAYER_PID + 3, ghost_draw_proc, ((bbplyr_data*)pobj)->plyrnum, 0, 0, 0);
#if 0
	((bbplyr_data *)pobj)->ghost = what;
	((bbplyr_data *)pobj)->ghostdelay = 2;
	if(what == 1)
	{
		for(k = 0; k < 15; k++)
			for(kk = 0; kk < 3; kk++)
				gjpos[k][kk] = ((bbplyr_data *)pobj)->jpos[k][kk];
		for(k = 0; k < 15; k++)
			for(kk = 0; kk < 12; kk++)
				gmatrices[k][kk] = ((bbplyr_data *)pobj)->matrices[k][kk];
	}
#endif

	if(doball == 1)
	{
		balltrail = 1;
	}
	else
	{
		balltrail = 0;
	}
		
}

//////////////////////////////////////////////////////////////////////////////
unsigned int get_some_hands(obj_3d *pobj)
{
	bbplyr_data *ppdata = (bbplyr_data *)pobj;
	int hands;

	// Give this player a different hand
	hands = (RIGHT_HAND) | (LEFT_HAND << BITS_HAND);

	if((pobj->adata[0].seq_index >= (SEQ_SBREATH1) && pobj->adata[0].seq_index <= (SEQ_S_CYC_3)) )
	{
		hands = (RIGHT_HAND) | (LEFT_HAND << BITS_HAND);

		if(pobj->adata[0].seq_index == (SEQ_S_CYC_3)
		&& (pobj->adata[0].fframe >= 121.0f && pobj->adata[0].fframe <= 142.0f) )
		{
			hands = (RIGHT_FIST) | (hands & MASK_LEFT_HAND);
		}
#if 0
		if(pobj->adata[0].seq_index == (SEQ_S_CYC_3)
		&& (pobj->adata[0].fframe >= 34.0f && pobj->adata[0].fframe <= 82.0f) )
		{
			hands = (RIGHT_HAND_OPEN_H) | (LEFT_HAND_OPEN_H << BITS_HAND);
		}
#endif	
	}
	else if(
		 pobj->adata[0].seq_index >= (SEQ_S_LOOP_1)
	|| pobj->adata[0].seq_index >= (SEQ_S_LOOP_2)
	|| pobj->adata[0].seq_index >= (SEQ_S_LOOP_3)
	|| pobj->adata[0].seq_index >= (SEQ_S_LOOP_4))
	{
		hands = (RIGHT_HAND) | (LEFT_HAND << BITS_HAND);
	}
	else if(
		 pobj->adata[0].seq_index >= (SEQ_S_LOOP_1)
	|| pobj->adata[0].seq_index >= (SEQ_S_LOOP_2)
	|| pobj->adata[0].seq_index >= (SEQ_S_LOOP_3)
	|| pobj->adata[0].seq_index >= (SEQ_S_LOOP_4))
	{
		hands = (RIGHT_HAND) | (LEFT_HAND << BITS_HAND);
	}
	else if((pobj->flags & PF_IN_AIR_BLOCK) || (pobj->flags & PF_SWATING) || (pobj->flags & PF_REBOUND))
	{
		hands = (RIGHT_HAND_OPEN_H) | (LEFT_HAND_OPEN_H << BITS_HAND);
	}

	else if(ppdata->attack_mode == ATTMODE_PUSH)
	{
		hands = (RIGHT_HAND_OPEN_H) | (LEFT_HAND_OPEN_H << BITS_HAND);
	}

	else if(pobj->adata[0].seq_index >= (SEQ_D_RUN2_R) && pobj->adata[0].seq_index <= (SEQ_D_RUN2_L))
	{
		if(ppdata->ball_in_hand == RIGHT_HAND)
			hands = (RIGHT_HAND) | (hands & MASK_LEFT_HAND);
		else
			hands = (hands & MASK_RIGHT_HAND) | (LEFT_HAND << BITS_HAND);
	}
	else if(pobj->adata[0].seq_index >= (SEQ_D_STND_R) && pobj->adata[0].seq_index <= (SEQ_D_SPIDER))
	{	// STANDING DRIBBLES
		hands = (RIGHT_HAND) | (LEFT_HAND << BITS_HAND);
		if(ppdata->ball_in_hand == RIGHT_HAND)
			hands = (RIGHT_HAND) | (hands & MASK_LEFT_HAND);
		else
			hands = (hands & MASK_RIGHT_HAND) | (LEFT_HAND << BITS_HAND);
	}
	else if((pobj->adata[0].seq_index >= (SEQ_SWIPE1_R) && pobj->adata[0].seq_index <= (SEQ_SWIPE1_L))
	|| (pobj->adata[0].seq_index == (SEQ_S_SBLK_1))
	|| (pobj->adata[0].seq_index == (SEQ_CKY_RUN1))
	)
	{
		hands = (RIGHT_HAND_OPEN_H) | (LEFT_HAND_OPEN_H << BITS_HAND);
	}
	else if(
		 (pobj->adata[0].seq_index >= (SEQ_F_ST2_R) && pobj->adata[0].seq_index <= (SEQ_F_ST4_L))
	|| ((pobj->adata[1].animode & MODE_TWOPART) && (pobj->adata[1].seq_index >= (SEQ_F_ST2_R) && pobj->adata[1].seq_index <= (SEQ_F_ST4_L)))
	)
	{	// STEAL ANIMS
//		if(ppdata->handedness == RIGHT_HAND)
			hands = (RIGHT_HAND_OPEN_H) | (hands & MASK_LEFT_HAND);
//		else
			hands = (hands & MASK_RIGHT_HAND) | (LEFT_HAND_OPEN_H << BITS_HAND);
	}
	else if(pobj->adata[0].seq_index == (SEQ_S_READY) || pobj->adata[0].seq_index == (SEQ_RDY_WB))
	{
		hands = (RIGHT_HAND_OPEN_H) | (LEFT_HAND_OPEN_H << BITS_HAND);
	}
//	else if(pobj->adata[0].seq_index == (SEQ_TAUNT1))
//	{
//		hands = (RIGHT_FIST) | (LEFT_FIST << BITS_HAND);
//	}
	else
	if(pobj->adata[0].seq_index == (SEQ_RUN_R)
	|| pobj->adata[0].seq_index == (SEQ_RUN_L)
	|| pobj->adata[0].seq_index == (SEQ_TRUN)
	|| pobj->adata[1].seq_index == (SEQ_BOXR_RUN)
	)
	{
		hands = (RIGHT_FIST) | (LEFT_FIST << BITS_HAND);
	}
	else
	if(pobj->adata[0].seq_index == (SEQ_AFTER_1))
		hands = (RIGHT_FIST) | (LEFT_HAND << BITS_HAND);
	else
	{	// 2 hands is default
		hands = (RIGHT_HAND) | (LEFT_HAND << BITS_HAND);
	}

	if((pobj->adata[1].animode & MODE_TWOPART))
	{
		if((pobj->adata[1].seq_index == (SEQ_C_OVR_HD))
		|| (pobj->adata[1].seq_index == (SEQ_P_CHEST))
		|| (pobj->adata[1].seq_index == (SEQ_P_OVR_HD))
//		|| (pobj->adata[1].seq_index >= (SEQ_C_FS_1PR) && pobj->adata[1].seq_index <= (SEQ_C_FR_1PL))
		|| (pobj->adata[1].seq_index >= (SEQ_P_FWDB_R) && pobj->adata[1].seq_index <= (SEQ_P_OBCK_L))
		|| (pobj->adata[1].seq_index == (SEQ_P_SID5_R) || pobj->adata[1].seq_index == (SEQ_P_SID5_L))
		|| (pobj->adata[1].seq_index == (SEQ_P_SID4_R) || pobj->adata[1].seq_index == (SEQ_P_SID4_L))
		|| (pobj->adata[1].seq_index == (SEQ_P_SID3_R) || pobj->adata[1].seq_index == (SEQ_P_SID3_L))
		|| (pobj->adata[1].seq_index == (SEQ_P_SID2_R) || pobj->adata[1].seq_index == (SEQ_P_SID2_L))
		|| (pobj->adata[1].seq_index == (SEQ_P_SID1_R) || pobj->adata[1].seq_index == (SEQ_P_SID1_L))
		|| (pobj->adata[1].seq_index == (SEQ_P_FWD2_B) || pobj->adata[1].seq_index == (SEQ_P_FWD3_B))
		|| (pobj->adata[1].seq_index == (SEQ_P_FWD1_R) || pobj->adata[1].seq_index == (SEQ_P_FWD1_L))
		|| (pobj->adata[1].seq_index == (SEQ_P_DIA1_R) || pobj->adata[1].seq_index == (SEQ_P_DIA1_L))
		|| (pobj->adata[1].seq_index == (SEQ_P_BCK1_R) || pobj->adata[1].seq_index == (SEQ_P_BCK1_L))
		|| (pobj->flags & PF_CATCHING)	
		)
		{	// CATCHES and PASSES
			hands = (RIGHT_HAND_OPEN_H) | (LEFT_HAND_OPEN_H << BITS_HAND);
		}

		if(pobj->adata[1].seq_index == (SEQ_BOXR_RUN))
		{
			hands = (RIGHT_FIST) | (LEFT_FIST << BITS_HAND);
		}
	}

// if player is hanging on rim give him fists.
	if(ppdata->HangingOnRim == YES)
	{
		if(both_hands_on_rim == YES)
		{
			hands = (RIGHT_FIST) | (LEFT_FIST << BITS_HAND);
		}
		else if(ppdata->ball_in_hand == RIGHT_HAND)
			hands = (RIGHT_FIST) | (hands & MASK_LEFT_HAND);
		else
			hands = (hands & MASK_RIGHT_HAND) | (LEFT_FIST << BITS_HAND);
	}
	return hands;
}

//////////////////////////////////////////////////////////////////////////////
// player shadow functions
//////////////////////////////////////////////////////////////////////////////
#if defined(VEGAS)
Texture_node_t	*ps_decals[4];
#else
GrMipMapId_t	ps_decals[4];
#endif
int				ps_ratios[4];

static void player_shadow_init( void )
{
	char *shadow_names[] = { "psshould.wms", "pslimb.wms", "pstorso.wms",
			"pshand.wms", NULL };

	load_textures( shadow_names, ps_ratios, ps_decals, 4, TXID_PLAYER,
		GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR );
}

//////////////////////////////////////////////////////////////////////////////
void player_shadow_predraw( void *oi )
{
	float		cx,cy,cz;		// cambot x,y,z

	// do some fancy math
	cx = -cambot.x;	cy = -cambot.y;	cz = -cambot.z;

	fx = cx * cambot.xform[0] + cy * cambot.xform[1] + cz * cambot.xform[2];
	fy = cx * cambot.xform[4] + cy * cambot.xform[5] + cz * cambot.xform[6];
	fz = cx * cambot.xform[8] + cy * cambot.xform[9] + cz * cambot.xform[10];

	sx = SUN_X * cambot.xform[0] + SUN_Y * cambot.xform[1] + SUN_Z * cambot.xform[2];
	sy = SUN_X * cambot.xform[4] + SUN_Y * cambot.xform[5] + SUN_Z * cambot.xform[6];
	sz = SUN_X * cambot.xform[8] + SUN_Y * cambot.xform[9] + SUN_Z * cambot.xform[10];

	nx = cambot.xform[1];
	ny = cambot.xform[5];
	nz = cambot.xform[9];

		// init glide states
	guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);
	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
	grTexClampMode(0,GR_TEXTURECLAMP_WRAP,GR_TEXTURECLAMP_WRAP);
	grAlphaTestFunction( GR_CMP_GREATER );
	//grAlphaTestReferenceValue( 54 );
	grAlphaTestReferenceValue( 95 );

	grChromakeyMode( GR_CHROMAKEY_DISABLE );

	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
	grAlphaBlendFunction( GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA,
			GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA );

	grDepthBufferMode( GR_DEPTHBUFFER_WBUFFER );
	grDepthBufferFunction( GR_CMP_GREATER );
	grDepthMask( FXTRUE );
}

//////////////////////////////////////////////////////////////////////////////
typedef struct sh_link {
	int		l1,l2;		// joint indices
	float	w1,w2;		// (half)width
	float	k1,k2;		// length extensions
	int		tex;		// texture to use
} sh_link_t;

#define LTOEX	(7.9955f * PSCALE)
#define LTOEY	(80.7099f * PSCALE)
#define LTOEZ	(-3.3730f * PSCALE)

#define RTOEX	(-7.9955f * PSCALE)
#define RTOEY	(80.7099f * PSCALE)
#define RTOEZ	(-3.3730f * PSCALE)





void draw_player_shadow( void *oi )
{
	static sh_link_t links[] = {
		{11,15,	1.44f,1.44f,	1.0f,1.6f, 1},
		{14,16,	1.44f,1.44f,	1.0f,1.6f, 1},
		{0,8,	2.4f,2.4f,		0.3f,0.8f, 2},
		{2,5,	1.92f,1.92f,	0.3f,0.3f, 0},
		{3,2,	1.44f,1.56f,	0.3f,0.3f, 1},
		{4,3,	1.44f,1.44f,	0.3f,0.3f, 3},
		{6,5,	1.44f,1.56f,	0.3f,0.3f, 1},
		{7,6,	1.44f,1.44f,	0.3f,0.3f, 3},
		{10,0,	1.92f,1.92f,	0.3f,0.3f, 1},
		{11,10,	1.44f,1.44f,	0.3f,0.3f, 1},
		{13,0,	1.92f,1.92f,	0.3f,0.3f, 1},
		{14,13,	1.44f,1.44f,	0.3f,0.3f, 1}};
	static int used[] = {0,2,3,4,5,6,7,8,10,11,13,14,15,16};

	bbplyr_data	*ppdata = oi;
	obj_3d		*pobj = oi;
	int			depth = 68;	//34;

	static sh_link_t *links_ptr;
	int		l1,l2;		// joint indices
	float	w1,w2;		// (half)width
	float	k1,k2;		// length extensions

	float k1u0;
	float k1u1;
	float k1u2;
	float k2u0;
	float k2u1;
	float k2u2;
	float w1v0;
	float w1v1;
	float w1v2;
	float w2v0;
	float w2v1;
	float w2v2;

	float *mat;
	float *jpos;

	ReducedVertex	v[4];
	ReducedVertex	*v_ptr;

	vec3d		vtx[17];
	float		*p1,*p2;
	float		p1p2[3],vr[3],vu[3],p1p2xn[3],rmagp1p2,rmagp1p2xn,t;
	float		tx,ty,tz,d,ex,ey,ez;
	float		xf,yf;
	int			i,j;

	//
	// Abort if:
	//    not in GM_PRE_TIPOFF and
	//       not on screen or
	//       not in replay and not a drone
	//
	if (game_info.game_mode != GM_PRE_TIPOFF && (
			!(pobj->flags & PF_ONSCREEN)
#ifndef DEBUG_PLAYERS
			|| (!PLAYBACK && game_info.plyr_control[ppdata->plyrnum] != -1)
#endif
		)) return;

	xf = hres_2;
	yf = hres_2 * (is_low_res ? 0.666667f : 1.0f);
	vres_2 = (VRES*(is_low_res ? 0.666667f : 1.0f)*0.5f);

	// project the joints onto the ground
	for( j = 0; j < 12; j++ )
	{
		i = used[j];

		jpos = &ppdata->jpos[i][0];
		t = (nx*(fx-jpos[0]) + ny*(fy-jpos[1]) + nz*(fz-jpos[2])) * INV_SUN_Y;
		vtx[i].x = jpos[0] + t*sx;
		vtx[i].y = jpos[1] + t*sy;
		vtx[i].z = jpos[2] + t*sz;
	}
	// compute screen coords of toe joints
	mat = &ppdata->matrices[JOINT_RANKLE][0];

	ex = RTOEX * mat[0] + RTOEY * mat[1] + RTOEZ * mat[2]  + mat[3];
	ey = RTOEX * mat[4] + RTOEY * mat[5] + RTOEZ * mat[6]  + mat[7];
	ez = RTOEX * mat[8] + RTOEY * mat[9] + RTOEZ * mat[10] + mat[11];

	t = (nx * (fx - ex) + ny * (fy - ey) + nz * (fz - ez)) * INV_SUN_Y;
	vtx[15].x = ex + t*sx;
	vtx[15].y = ey + t*sy;
	vtx[15].z = ez + t*sz;

	mat = &ppdata->matrices[JOINT_LANKLE][0];

	ex = LTOEX * mat[0] + LTOEY * mat[1] + LTOEZ * mat[2]  + mat[3];
	ey = LTOEX * mat[4] + LTOEY * mat[5] + LTOEZ * mat[6]  + mat[7];
	ez = LTOEX * mat[8] + LTOEY * mat[9] + LTOEZ * mat[10] + mat[11];

	t = (nx * (fx - ex) + ny * (fy - ey) + nz * (fz - ez)) * INV_SUN_Y;
	vtx[16].x = ex + t*sx;
	vtx[16].y = ey + t*sy;
	vtx[16].z = ez + t*sz;

	// now draw each shadow link
	for( i = 0; i < 12; i++ )
	{
		links_ptr = &links[i];

		l1 = links_ptr->l1;
		l2 = links_ptr->l2;
		w1 = links_ptr->w1;
		w2 = links_ptr->w2;	
		k1 = links_ptr->k1;
		k2 = links_ptr->k2;	

		p1 = (float *)&vtx[l1];
		p2 = (float *)&vtx[l2];

		p1p2[0] = vtx[l2].x - vtx[l1].x;
		p1p2[1] = vtx[l2].y - vtx[l1].y;
		p1p2[2] = vtx[l2].z - vtx[l1].z;

		d = fsqrt( p1p2[0]*p1p2[0] + p1p2[1]*p1p2[1] + p1p2[2]*p1p2[2] );
		rmagp1p2 = 1.0f / d;
		k1 = k1*d;
		k2 = k2*d;
		p1p2xn[0] = p1p2[1] * nz - p1p2[2] * ny;
		p1p2xn[1] = p1p2[2] * nx - p1p2[0] * nz;
		p1p2xn[2] = p1p2[0] * ny - p1p2[1] * nx;
		rmagp1p2xn = rsqrt( p1p2xn[0]*p1p2xn[0] + p1p2xn[1]*p1p2xn[1] + p1p2xn[2]*p1p2xn[2] );

		vu[0] = p1p2[0] * rmagp1p2;
		vu[1] = p1p2[1] * rmagp1p2;
		vu[2] = p1p2[2] * rmagp1p2;

		k1u0 = k1 * vu[0];
		k1u1 = k1 * vu[1];
		k1u2 = k1 * vu[2];

		k2u0 = k2 * vu[0];
		k2u1 = k2 * vu[1];
		k2u2 = k2 * vu[2];

		vr[0] = p1p2xn[0] * rmagp1p2xn;
		vr[1] = p1p2xn[1] * rmagp1p2xn;
		vr[2] = p1p2xn[2] * rmagp1p2xn;

		w1v0 = w1 * vr[0];
		w1v1 = w1 * vr[1];
		w1v2 = w1 * vr[2];

		w2v0 = w2 * vr[0];
		w2v1 = w2 * vr[1];
		w2v2 = w2 * vr[2];

		
		v_ptr = &v[0];									  
		tz = p1[2] + w1v2 - k1u2;
		tx = p1[0] + w1v0 - k1u0;
		ty = p1[1] + w1v1 - k1u1;
		v_ptr->oow = 1.0f / tz;
		tx = -(tx * fovfac2x * xf);
		ty =   ty * fovfac2x * yf;
		tx *= v_ptr->oow;
		ty *= v_ptr->oow;
		tx += hres_2;
		ty += vres_2;
		v_ptr->x = tx;
		v_ptr->y = ty;
		v_ptr->sow = 255.0f * v_ptr->oow;
		v_ptr->tow = 255.0f * v_ptr->oow;

		v_ptr++;
		tz = p2[2] + w2v2 + k2u2;
		tx = p2[0] + w2v0 + k2u0;
		ty = p2[1] + w2v1 + k2u1;
		v_ptr->oow = 1.0f / tz;
		tx = -(tx * fovfac2x * xf);
		ty =   ty * fovfac2x * yf;
		tx *= v_ptr->oow;
		ty *= v_ptr->oow;
		tx += hres_2;
		ty += vres_2;
		v_ptr->x = tx;
		v_ptr->y = ty;
		v_ptr->sow = 255.0f * v_ptr->oow;
		v_ptr->tow = 0.0f;

		v_ptr++;
		tz = p2[2] - w2v2 + k2u2;
		tx = p2[0] - w2v0 + k2u0;
		ty = p2[1] - w2v1 + k2u1;
		v_ptr->oow = 1.0f / tz;

		tx = -(tx * fovfac2x * xf);
		ty =   ty * fovfac2x * yf;
		tx *= v_ptr->oow;
		ty *= v_ptr->oow;
		tx += hres_2;
		ty += vres_2;
		v_ptr->x = tx;
		v_ptr->y = ty;
		v_ptr->sow = 0.0f;
		v_ptr->tow = 0.0f;

		v_ptr++;
		tz = p1[2] - w1v2 - k1u2;
		tx = p1[0] - w1v0 - k1u0;
		ty = p1[1] - w1v1 - k1u1;
		v_ptr->oow = 1.0f / tz;
		tx = -(tx * fovfac2x * xf);
		ty =   ty * fovfac2x * yf;
		tx *= v_ptr->oow;
		ty *= v_ptr->oow;
		tx += hres_2;
		ty += vres_2;
		v_ptr->x = tx;
		v_ptr->y = ty;
		v_ptr->sow = 0.0f;
		v_ptr->tow = 255.0f * v_ptr->oow;

		grDepthBiasLevel( depth -= 2 );
		guTexSource( ps_decals[links[i].tex] );

	#ifdef GEOM_STATS
	stats.verts_processed += 4;
	stats.verts_bbox_processed += 4;
	stats.verts_transformed += 4;
	stats.tris_processed  += 2;
	stats.tris_bbox_processed  += 2;
	stats.strips_processed += 1;
	stats.strips_bbox_processed += 1;
	#endif


		if ( ( v[0].x >= CLIP_XMIN) &&
			 ( v[0].x <  CLIP_XMAX) &&
   		     ( v[0].y >= CLIP_YMIN) &&
		     ( v[0].y <  CLIP_YMAX) &&
		     ( v[1].x >= CLIP_XMIN) &&
		     ( v[1].x <  CLIP_XMAX) &&
		     ( v[1].y >= CLIP_YMIN) &&
		     ( v[1].y <  CLIP_YMAX) &&
		     ( v[2].x >= CLIP_XMIN) &&
			 ( v[2].x <  CLIP_XMAX) &&
			 ( v[2].y >= CLIP_YMIN) &&
 			 ( v[2].y <  CLIP_YMAX) &&
		     ( v[3].x >= CLIP_XMIN) &&
			 ( v[3].x <  CLIP_XMAX) &&
			 ( v[3].y >= CLIP_YMIN) &&
 			 ( v[3].y <  CLIP_YMAX) &&
  			 ( v[0].oow > 0.0F ) && ( v[0].oow <= 1.0F ) &&
			 ( v[1].oow > 0.0F ) && ( v[1].oow <= 1.0F ) &&
			 ( v[2].oow > 0.0F ) && ( v[2].oow <= 1.0F ) &&
			 ( v[3].oow > 0.0F ) && ( v[3].oow <= 1.0F )  )	{
			grDrawFanReduced(v, 4);

			#ifdef GEOM_STATS
			stats.tris_drawn  += 2;
			stats.verts_drawn += 4;
			stats.strips_drawn += 1;
			#endif

		}
		else {
  			clip_triangle_t(&v[0],&v[1],&v[2]);
  			clip_triangle_t(&v[2],&v[3],&v[0]);

			#ifdef GEOM_STATS
			stats.tris_drawn  += 2;
			stats.verts_drawn += 4;
			stats.tris_clipped += 2;
			stats.tris_clip_processed += 2;
			#endif

		}



	}
}



#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
//
// debug switches
//
//////////////////////////////////////////////////////////////////////////////
extern struct texture_node *tlist;

#ifdef SHAWN
	#define STEP_SW	(P2_D | P4_D)
#else
	#define STEP_SW	(P2_D)
#endif
#define STEP_DEL	20
#define STEP_RPT	2

void debug_switch_proc( int *parg )
{
	static int ps5_old = 0;
	static int psw_old = 0;
	static int dip_old = 0;
	static int step_del = 0;

	while( 1 )
	{
		int ps5_cur = get_p5_sw_current();
		int ps5_new = ps5_cur & ~ps5_old;
		int psw_cur = get_player_sw_current();
		int psw_new = psw_cur & ~psw_old;
		int dip_cur = get_dip_coin_current();
		int dip_new = dip_cur & ~dip_old;

		if (step_del >= 0) step_del--;

		if (!PLAYBACK)
		{
			if( haltstep )
			{
				haltstep = FALSE;
				halt = !halt;
				suspend_multiple_processes( PLAYER_PID, -1 );
				suspend_multiple_processes( BALL_PID, -1 );
				suspend_multiple_processes( REFEREE_PID, -1 );
				suspend_multiple_processes( FREETHROW_METER_PID, -1 );
			}

			if (ps5_new & P_D || rob == YES)
			{
				// 'rob' is used for debugging, set to YES and the game halts.
				halt = !halt;
				rob = NO;

				if( halt )
				{
#ifdef DEBUG
					fprintf( stderr, "Program Halted\n" );
#endif
					suspend_multiple_processes( PLAYER_PID, -1 );
					suspend_multiple_processes( BALL_PID, -1 );
					suspend_multiple_processes( REFEREE_PID, -1 );
					suspend_multiple_processes( FREETHROW_METER_PID, -1 );
				}
				else
				{
#ifdef DEBUG
					fprintf( stderr, "Program Unhalted\n" );
#endif
					if (game_info.game_state != GS_EOQ && !playback)
					{
						resume_multiple_processes( PLAYER_PID, -1 );
						resume_multiple_processes( BALL_PID, -1 );
					}
					resume_multiple_processes( REFEREE_PID, -1 );
					resume_multiple_processes( FREETHROW_METER_PID, -1 );
				}
			}

			if (halt && (psw_cur & STEP_SW))
			{
				if (step_del <= 0)
				{
					haltstep = TRUE;
					halt = !halt;
					if (game_info.game_state != GS_EOQ && !playback)
					{
						resume_multiple_processes( PLAYER_PID, -1 );
						resume_multiple_processes( BALL_PID, -1 );
					}
					resume_multiple_processes( REFEREE_PID, -1 );
					resume_multiple_processes( FREETHROW_METER_PID, -1 );

					step_del = !step_del ? STEP_RPT : STEP_DEL;
				}
			}
			else
				step_del = -1;
		}
		else
			haltstep = FALSE;

		if( halt )
		{
			if( psw_new & P1_A && !(psw_cur & P1_C))
			{
				dump_anim_data2(0);
			}
			if( psw_new & P1_B && !(psw_cur & P1_C) )
			{
				dump_anim_data2(1);
			}
#if NUM_PERTEAM > 2
			if( psw_new & P1_D && !(psw_cur & P1_C) )
			{
				dump_anim_data2(2);
			}
#endif
			if( psw_new & P1_A && psw_cur & P1_C )
			{
				dump_anim_data2(NUM_PERTEAM+0);
			}
			if( psw_new & P1_B && psw_cur & P1_C )
			{
				dump_anim_data2(NUM_PERTEAM+1);
			}
#if NUM_PERTEAM > 2
			if( psw_new & P1_D && psw_cur & P1_C )
			{
				dump_anim_data2(NUM_PERTEAM+2);
			}
#endif
			if( psw_new & P1_C)
			{
				dump_game_state( 0 );
			}
			if( psw_new & P2_C )
			{
				process_node_t *p = plist;

				while( p )
				{
#ifdef DEBUG
					fprintf( stderr, "%s %x %d\n", p->process_name, p->process_id, p->run_level );
#endif
					p = p->next;
				}
			}

			if( psw_new & P2_B )
			{
				dump_drone_states();
			}

			if( dip_cur & P1_START_SW && psw_new & P1_C )
			{
				struct texture_node *tn;

				tn = tlist;
				while( tn )
				{
#ifdef DEBUG
#if defined(SEATTLE)
					fprintf( stderr, "%ld: %s\n", tn->texture_handle, tn->texture_name );
#else
					fprintf( stderr, "%p: %s\n", tn->texture_handle, tn->texture_name );
#endif
#endif
					tn = tn->next;
				}
			}
#if 0
			if( psw_new & P1_A )
			{
				dump_alloclist();
			}
#endif
			if( psw_new & P2_LEFT )
			{
				dump_anim_data();
			}
		}

		ps5_old = ps5_cur;
		psw_old = psw_cur;
		dip_old = dip_cur;

		sleep( 1 );
	}
}

//////////////////////////////////////////////////////////////////////////////
void watch_halt_proc( int * pargs)
{
	while (1)
	{
		// Chk if halted
		if (halt)
		{
			// Is; all PLAYER_PID procs should be suspended
			struct process_node	* pn = plist;
			while (pn)
			{
				if ((pn->process_id == PLAYER_PID) &&
					!(pn->process_sleep_time & 0x8000))
				{
#ifdef DEBUG
					fprintf(stderr,"A Very Bad Halt Has Occured - a PLAYER_PID is still active!\r\n");
					fprintf(stderr,"Any button continues\r\n");
//					do scan_switches(); while(!(get_player_sw_current()&ALL_ABCD_MASK));
#endif
					break;
				}
				pn = pn->next;
			}
		}
		sleep(1);
	}
}

//////////////////////////////////////////////////////////////////////////////
extern char *animode_names[], *player_flag_names[], *player_mode_names[];
static void dump_anim_data( void )
{
	int			i,j;
	bbplyr_data	*ppdata;
	obj_3d		*pobj;

	fprintf( stderr, "\n" );
	for( j = 0; j < NUM_PLAYERS; j++ )
	{
		ppdata = player_blocks + j;
		pobj = (obj_3d *)ppdata;
		fprintf( stderr, "%2x:%s %s %s %s\n",
			ppdata->p_spd->number,
			ppdata->p_spd->szlast,
			player_mode_names[pobj->plyrmode],
			ISDRONE(ppdata) == YES ? "DRONE" : "HUMAN",
			ISDRONETEAM(ppdata) == YES ? "DRONE-TEAM" : "HUMAN-TEAM"
			);
		fprintf( stderr, " %10s:%2d ",
				seq_names[pobj->adata[0].seq_index],
				pobj->adata[0].iframe
			);

		if( pobj->adata[0].animode )
		{
			//fprintf( stderr, "  animode: " );
			for( i = 0; animode_names[i]; i++ )
			{
				if( pobj->adata[0].animode & (1<<i))
					fprintf( stderr, " %s", animode_names[i]+5 );
			}
			//fprintf( stderr, "\n" );
		}
		if( pobj->flags )
		{
			//fprintf( stderr, "  plrflgs: " );
			for( i = 0; player_flag_names[i]; i++ )
			{
				if( pobj->flags & (1<<i))
					fprintf( stderr, " %s", player_flag_names[i]+3 );
			}
			fprintf( stderr, "\n" );
		}
		if( pobj->adata[1].animode & MODE_TWOPART )
		{
			if( pobj->adata[1].animode )
			{
				fprintf( stderr, " %10s:%2d",
					seq_names[pobj->adata[1].seq_index], pobj->adata[1].iframe );
				//fprintf( stderr, "  animode: " );
				for( i = 0; animode_names[i]; i++ )
				{
					if( pobj->adata[1].animode & (1<<i))
						fprintf( stderr, " %s", animode_names[i]+5 );
				}
				fprintf( stderr, "\n" );
			}
		}
	}
}
#endif //DEBUG

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void show_hotspot_proc( int *args)
{
	struct texture_node * pnode;
	int i;

#ifndef	NO_HOTSPOTS
	// Load textures
	for( i = 0; i < MAX_PLYRS; i++ )
	{
		if (!(pup_show_hotspots & (1<<i)))
			continue;

		create_object("hotspots", OID_HOTSPOT_CURSOR+i, OF_NONE,DRAWORDER_FIELDFX+0x10, NULL, draw_hotspot_cursors);
	}
#endif
}


//////////////////////////////////////////////////////////////////////////////
static void draw_hotspot_cursors( void *oi )
{
	register bbplyr_data * ppd;
	static ST cursor_obj_st[] = {
#ifdef JONHEY
		{      0.5f/CURSOR_TEX_S * 256.0F, (1.0f-0.5f/CURSOR_TEX_T) * 256.0F },
		{      0.5f/CURSOR_TEX_S * 256.0F,      0.5f/CURSOR_TEX_T * 256.0F },
		{ (1.0f-0.5f/CURSOR_TEX_S) * 256.0F, (1.0f-0.5f/CURSOR_TEX_T) * 256.0F },
		{ (1.0f-0.5f/CURSOR_TEX_S) * 256.0F,      0.5f/CURSOR_TEX_T * 256.0F }};
#else
		{      0.5f/CURSOR_TEX_S, 1.0f-0.5f/CURSOR_TEX_T },
		{      0.5f/CURSOR_TEX_S,      0.5f/CURSOR_TEX_T },
		{ 1.0f-0.5f/CURSOR_TEX_S, 1.0f-0.5f/CURSOR_TEX_T },
		{ 1.0f-0.5f/CURSOR_TEX_S,      0.5f/CURSOR_TEX_T }};
#endif
	float cm[12];
	float mat[12] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f };
	int i, j;

	grConstantColorValue( 255 << 24 );

	guAlphaSource( GR_ALPHASOURCE_TEXTURE_ALPHA );
	grTexCombineFunction( 0, GR_TEXTURECOMBINE_DECAL );
	grTexClampMode( 0, GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP);

	grDepthBufferMode( GR_DEPTHBUFFER_WBUFFER );
	grDepthBufferFunction( GR_CMP_ALWAYS );
	grDepthMask( FXTRUE );

	guColorCombineFunction( GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA );
	grChromakeyMode( GR_CHROMAKEY_DISABLE );

	{
	static int a = 0;
	static int b = 2;

//	grConstantColorValue((0<<24) + (a<<16) + (a<<8) + a);
//	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB);

	grConstantColorValue(a<<24);
	guColorCombineFunction(GR_COLORCOMBINE_DECAL_TEXTURE_ADD_CCALPHA);

	a+=b;
	if(a==0||a==110)b=-b;
	}

	grAlphaBlendFunction(
		GR_BLEND_SRC_ALPHA,
		GR_BLEND_ONE_MINUS_SRC_ALPHA,
		GR_BLEND_SRC_ALPHA,
		GR_BLEND_ONE_MINUS_SRC_ALPHA );

	limb_gndsquare_obj.pst = cursor_obj_st;

	ppd = player_blocks;

	for (i = 0; i < NUM_PLAYERS; ppd++, i++)
	{
		j = game_info.plyr_control[i];
		if ((j >= 0) && (game_info.game_mode != GM_PRE_TIPOFF) && (pup_show_hotspots & (1<<i)))
		{
			mat[3] = (hotspot_xzs[ppd->p_spd->hotspot][0] * (ppd->team ? 1 : -1)) - cambot.x;
			mat[11] = hotspot_xzs[ppd->p_spd->hotspot][1] - cambot.z;
			mat[7]  = -cambot.y;

			roty(512 + (int)RD2GR(cambot.theta), mat);
			mxm(cambot.xform, mat, cm);
			render_limb(&limb_gndsquare_obj, cm, cursor_decals + j, &cursor_ratio);
		}
	}
	limb_gndsquare_obj.pst = gndsquare_obj_st;
}


//////////////////////////////////////////////////////////////////////////////
//
// cursors
//
//////////////////////////////////////////////////////////////////////////////
void cursor_proc( int *parg )
{
	struct texture_node * pnode;
	int i;

	// Load textures
	for( i = 0; i < NUM_PLAYERS; i++ )
	{
		pnode = create_texture( (four_plr_ver ? cursor_names[i]:cursor_names_2p[i]), 0,
			0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR );

		if( !pnode )
		{
#ifdef DEBUG
			fprintf( stderr, "Error loading %s\n", (four_plr_ver ? cursor_names[i]:cursor_names_2p[i]) );
#endif
			increment_audit( TEX_LOAD_FAIL_AUD );
		}
		else
			cursor_decals[i] = pnode->texture_handle;
	}

	// Create objects
	create_object( "cursors", OID_CURSOR,
		OF_NONE,
		DRAWORDER_FIELDFX+0x10,
		NULL,
		draw_cursors );
}

//////////////////////////////////////////////////////////////////////////////
static void draw_cursors( void *oi )
{
	register bbplyr_data * ppd;
	static ST cursor_obj_st[] = {
#ifdef JONHEY
		{      0.5f/CURSOR_TEX_S * 256.0F, (1.0f-0.5f/CURSOR_TEX_T) * 256.0F },
		{      0.5f/CURSOR_TEX_S * 256.0F,      0.5f/CURSOR_TEX_T * 256.0F },
		{ (1.0f-0.5f/CURSOR_TEX_S) * 256.0F, (1.0f-0.5f/CURSOR_TEX_T) * 256.0F },
		{ (1.0f-0.5f/CURSOR_TEX_S) * 256.0F,      0.5f/CURSOR_TEX_T * 256.0F }};
#else
		{      0.5f/CURSOR_TEX_S, 1.0f-0.5f/CURSOR_TEX_T },
		{      0.5f/CURSOR_TEX_S,      0.5f/CURSOR_TEX_T },
		{ 1.0f-0.5f/CURSOR_TEX_S, 1.0f-0.5f/CURSOR_TEX_T },
		{ 1.0f-0.5f/CURSOR_TEX_S,      0.5f/CURSOR_TEX_T }};
#endif
	float cm[12];
	float mat[12] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f };
	int i, j;

//	int cursor_colors[][MAX_PLYRS] = {
//		{0xFF0600FF, 0xFF00C800, 0x00000000, 0x00000000},
//		{0x000600FF, 0x0000C800, 0x00FF0000, 0x00F5F505}};

	grConstantColorValue( 255 << 24 );

	guAlphaSource( GR_ALPHASOURCE_TEXTURE_ALPHA );
	grTexCombineFunction( 0, GR_TEXTURECOMBINE_DECAL );
	grTexClampMode( 0, GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP);

	grDepthBufferMode( GR_DEPTHBUFFER_WBUFFER );
	grDepthBufferFunction( GR_CMP_ALWAYS );
	grDepthMask( FXTRUE );

//	guColorCombineFunction( GR_COLORCOMBINE_CCRGB );
	guColorCombineFunction( GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA );
	grChromakeyMode( GR_CHROMAKEY_DISABLE );

//	grAlphaCombine(
//		GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL,
//		GR_COMBINE_FACTOR_ONE_MINUS_OTHER_ALPHA,
//		GR_COMBINE_LOCAL_CONSTANT,
//		GR_COMBINE_OTHER_TEXTURE,
//		FXFALSE );
	grAlphaBlendFunction(
		GR_BLEND_SRC_ALPHA,
		GR_BLEND_ONE_MINUS_SRC_ALPHA,
		GR_BLEND_SRC_ALPHA,
		GR_BLEND_ONE_MINUS_SRC_ALPHA );

	limb_gndsquare_obj.pst = cursor_obj_st;

	ppd = player_blocks;

	for (i = 0; i < NUM_PLAYERS; ppd++, i++)
	{
		j = game_info.plyr_control[i];
//		if (ppd->plyrnum == game_info.receiver_a) j = MAX_PLYRS + 0;
//		if (ppd->plyrnum == game_info.receiver_b) j = MAX_PLYRS + 1;
//
		if (j >= 0 && game_info.game_mode != GM_PRE_TIPOFF)
		//if( arrowinfo[i].show >= 0 )
		{
//			fprintf(stderr,"cursor %d\r\n",j);
			mat[3]  = ppd->odata.x - cambot.x;
			mat[7]  = -cambot.y;
			mat[11] = ppd->odata.z - cambot.z;

			roty( 512 + (int)RD2GR(cambot.theta), mat );
			mxm( cambot.xform, mat, cm );
//			grConstantColorValue( game_info.plyr_control[i] >= 0 ?
//				cursor_colors[four_plr_ver][game_info.plyr_control[i]]:
//				0x08808080);
			render_limb( &limb_gndsquare_obj, cm,
				cursor_decals + j,
				&cursor_ratio );
		}
	}
	limb_gndsquare_obj.pst = gndsquare_obj_st;
}

//////////////////////////////////////////////////////////////////////////////
//void Player_Shadow_Proc( int *parg )
//{
//	struct texture_node * pnode;
//	int i;
//
//	// Load textures
//	for( i = 0; i < SHADOW_CNT; i++ )
//	{
//		pnode = create_texture( shadow_names[i], 0,
//			0, CREATE_NORMAL_TEXTURE,
//			GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
//			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR );
//
//		if( !pnode )
//		{
//#ifdef DEBUG
//			fprintf( stderr, "Error loading %s\n", shadow_names[i] );
//#endif
//			increment_audit( TEX_LOAD_FAIL_AUD );
//		}
//		else
//			shadow_decals[i] = pnode->texture_handle;
//	}
//
//	// Create objects
//	create_object( "Player_Shadows", OID_CURSOR,
//		OF_NONE,
//		DRAWORDER_FIELDFX+0x10,
//		NULL,
//		Draw_Shadows );
//}

//////////////////////////////////////////////////////////////////////////////
//void Prep_Shadows( void )
//{
//	guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);
//	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
//	grTexClampMode(0,GR_TEXTURECLAMP_WRAP,GR_TEXTURECLAMP_WRAP);
//
//	grDepthBufferMode( GR_DEPTHBUFFER_WBUFFER );
//	grDepthBufferFunction( GR_CMP_ALWAYS );
//	grDepthMask( FXTRUE );
//
////	grCullMode( GR_CULL_DISABLE );
//	grChromakeyMode( GR_CHROMAKEY_DISABLE );
//
//	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
//	grAlphaBlendFunction( GR_BLEND_SRC_ALPHA,
//			GR_BLEND_ONE_MINUS_SRC_ALPHA,
//			GR_BLEND_SRC_ALPHA,
//			GR_BLEND_ONE_MINUS_SRC_ALPHA );
//}
//
//void Rend_Shadows( obj_3d *pobj, int ballflag )
//{
//	float			mat[12] = { 1.0f, 0.0f, 0.0f, 0.0f,
//							    0.0f, 1.0f, 0.0f, 0.0f,
//							    0.0f, 0.0f, 1.0f, 0.0f };
//	float			cm[12];
//
//	mat[3]  = pobj->x - cambot.x;
//	mat[7]  = -cambot.y;
//	mat[11] = pobj->z - cambot.z;
//
//	if (ballflag)
//		roty( pobj->fwd, mat );
//
//	mxm( cambot.xform, mat, cm );
//
//	if (ballflag)
//		render_limb( &limb_ballshadow_obj, cm, &ball_shadow_decal, &ball_shadow_ratio );
//	else
//		render_limb( &limb_gndsquare_obj, cm, shadow_decals + 0, &player_shadow_ratio );
//}
//
//void Draw_Shadows( void *oi )
//{
//	int i;
//
//	Prep_Shadows();
//
//	// shadow's
//	for(i = 0; i < NUM_PLAYERS; i++)
//	{
//		Rend_Shadows( (obj_3d *)(player_blocks + i), FALSE );
//	}		
//
//#ifdef DEBUG
//	{
//		float			mat[12] = { 1.0f, 0.0f, 0.0f, 0.0f,
//								    0.0f, 1.0f, 0.0f, 0.0f,
//								    0.0f, 0.0f, 1.0f, 0.0f };
//		float			cm[12];
//
//		if(game_info.ball_handler == -1)
//		{
//			//mat[3] = -cambot.x;
//			//mat[7] = -cambot.y;
//			//mat[11] = -cambot.z;
//			mat[3] = ball_obj.tx - cambot.x;
//			mat[7] = -cambot.y;
//			mat[11] = ball_obj.tz - cambot.z;
//		}
//		else
//		{
//			mat[3] = drone_blocks[game_info.ball_handler].avoid_x - cambot.x;
//			mat[7] = -cambot.y;
//			mat[11] = drone_blocks[game_info.ball_handler].avoid_z - cambot.z;
//		}
//
//		mxm( cambot.xform, mat, cm );
//		render_limb( &limb_gndsquare_obj, cm, &target_decal, &cursor_ratio );
//	}
//#endif
//
//	// ball shadow & target cursor
//	if( game_info.ball_handler == -1 )
//	{
//		Rend_Shadows( (obj_3d *)&ball_obj, TRUE );
//#if 0
//		mat[3] = ball_obj.tx - cambot.x;
//		mat[7] = -cambot.y;
//		mat[11] = ball_obj.tz - cambot.z;
//
//		mxm( cambot.xform, mat, cm );
//		render_limb( &limb_gndsquare_obj, cm, &target_decal, &cursor_ratio );
//#endif
//	}
//}

//////////////////////////////////////////////////////////////////////////////
//
// ball
//
//////////////////////////////////////////////////////////////////////////////
void ball_proc( int *parg )
{
	game_info_t	*gi_p;

	struct texture_node	*pnode;
	int				joint;//, fire = 0;
	float 			xob,yob,zob,bx,by,bz,*tm, tx = 0.0f, ty = 0.0f, tz = 0.0f;//, p[3];
	float			rhx,rhy,rhz;
	bbplyr_data	*ppdata;
	obj_3d			*pobj;

	gi_p = &game_info;

	// load the ball texture
	if(pup_aba_ball != 0)
	{
		pnode = create_texture( "BBallRWB.wms", 0, 0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR );
	}
	else
	{
		pnode = create_texture( "BBall_1.wms", 0, 0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR );
	}		

	if( pnode == NULL )
#ifdef DEBUG
		fprintf( stderr, "Error loading BBall_1.wms\n" );
#else
	;
#endif
	else
		ball_decal = pnode->texture_handle;

	ball_ratio = pnode->texture_info.header.aspect_ratio;

#if 1
	// load the ball shade texture
	pnode = create_texture( "bballhlt.wms", 0, 0, CREATE_NORMAL_TEXTURE,
		GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
		GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR );

	if( pnode == NULL )
#ifdef DEBUG
		fprintf( stderr, "Error loading bballhlt.wms\n" );
#else
	;
#endif
	else
		ballhigh_decal = pnode->texture_handle;

	ballhigh_ratio = pnode->texture_info.header.aspect_ratio;
#endif

#if 1
	// load the ball shade texture
	pnode = create_texture( "BBall_A.wms", 0, 0, CREATE_NORMAL_TEXTURE,
		GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
		GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR );

	if( pnode == NULL )
#ifdef DEBUG
		fprintf( stderr, "Error loading BBall_A.wms\n" );
#else
	;
#endif
	else
		ballshade_decal = pnode->texture_handle;

	ballshade_ratio = pnode->texture_info.header.aspect_ratio;
#endif

#if 0
	// load the target marker
	pnode = create_texture( "c_x_yel.wms", 0, 0, CREATE_NORMAL_TEXTURE,
		GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
		GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR );

	if( pnode == NULL )
#ifdef DEBUG
		fprintf( stderr, "Error loading off.wms\n" );
#else
	;
#endif
	else
		target_decal = pnode->texture_handle;
#endif

	// load the ball shadow
	pnode = create_texture( "shadow.wms", 0, 0, CREATE_NORMAL_TEXTURE,
		GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
		GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR );

	if( pnode == NULL )
#ifdef DEBUG
		fprintf( stderr, "Error loading ball shadow.wms\n" );
#else
	;
#endif
	else
		ball_shadow_decal = pnode->texture_handle;

	ball_shadow_ratio = pnode->texture_info.header.aspect_ratio;

	guTexChangeAttributes( ball_shadow_decal,
                      -1, -1,
                      -1,
                      GR_MIPMAP_DISABLE,
                      -1, -1,
                      -1,
											-1,
                      -1,
											GR_TEXTUREFILTER_BILINEAR,//POINT_SAMPLED,
											GR_TEXTUREFILTER_BILINEAR
                      );


#ifdef DEBUG_HEADS
	ball_obj.odata.z = ball_obj.tz = 40.0f;
#else
	ball_obj.odata.z = ball_obj.tz = 0.0f;
#endif
	ball_obj.odata.y = 35.0f;
	ball_obj.odata.x = ball_obj.tx = 0.0f;
	ball_obj.odata.vx = 0.0f;
	ball_obj.odata.vy = -1.01f;
	ball_obj.odata.vz = 0.0f;
	//printf("made ball proc\n");

	create_object( "ball", OID_BALL, OF_NONE, DRAWORDER_BALL, (void *)&ball_obj, draw_ball);
	create_object( "ball shadow", OID_BALL, OF_NONE, DRAWORDER_FIELDFX + 0x1F - 1, (void *)&ball_obj, draw_ball_shadow );

	ball_init();

	while( 1 )
	{
#ifdef DEBUG_NO_FIRE
		game_info.fire_cnt = 1;
		game_info.tm_fire_cnt[0] =
		game_info.tm_fire_cnt[1] =
		game_info.tm_fire_cnt[2] =
		game_info.tm_fire_cnt[3] = 0;
#endif

		if(game_info.ball_handler != NO_BALLHANDLER)
		{ // somebody has the ball
			pobj = (obj_3d *)(player_blocks + game_info.ball_handler);
			ppdata = &player_blocks[game_info.ball_handler];

#if NUM_PERTEAM > 2
			// Update current pass receiver #'s
			// <receiver_a> wants to always be a human or at least just the
			//  lower numbered player (in case someone buys-in)
			if (!((game_info.receiver_a = ppdata->plyrnum + 1) % NUM_PERTEAM))
				game_info.receiver_a -= NUM_PERTEAM;
			if (!((game_info.receiver_b = game_info.receiver_a + 1) % NUM_PERTEAM))
				game_info.receiver_b -= NUM_PERTEAM;
			if ((ISDRONE(&player_blocks[game_info.receiver_a]) &&
				 ISHUMAN(&player_blocks[game_info.receiver_b]))
				||
				 (game_info.receiver_a > game_info.receiver_b))
			{
				i = game_info.receiver_a;
				game_info.receiver_a = game_info.receiver_b;
				game_info.receiver_b = i;
			}
#endif

	// if player isn't passing or shooting, call Dribbler
	// ball_mode...-1=free on court, 0=dribbling, 1=passing, 2=shooting, 3=Bouncing On Rim
			if(!(pobj->flags & PF_SHOOTING)
//			if(pobj->y == 0.0f
				&& game_info.passer == NO_PASSER
				&& game_info.ball_mode < BM_PASSING)	//game_info.ball_mode != BM_PASSING && game_info.ball_mode != BM_SHOOTING)
			{
				ball_rotclr();
				//ball_obj.odata.vx = ball_obj.odata.vy = ball_obj.odata.vz = 0.0f;
				ball_obj.odata.vx = player_blocks[game_info.ball_handler].odata.vx;
				ball_obj.odata.vy = player_blocks[game_info.ball_handler].odata.vy;
				ball_obj.odata.vz = player_blocks[game_info.ball_handler].odata.vz;

				Dribbler(pobj);

				if(balltrail)
					ball_trail();
			}
			else
			{
				if(ppdata->ball_in_hand == RIGHT_HAND)
				{
					bx = (R_INHAND_X_OFF + 0.0f) * PSCALE;
					by = R_INHAND_Y_OFF * PSCALE;
					bz = (R_INHAND_Z_OFF + 0.0f) * PSCALE;
					joint = JOINT_RWRIST;
				}
				else
				{
					bx = (L_INHAND_X_OFF + 0.0f) * PSCALE;
					by = L_INHAND_Y_OFF * PSCALE;
					bz = (L_INHAND_Z_OFF + 0.0f) * PSCALE;
					joint = JOINT_LWRIST;
				}
	
				tm = cambot.xform;
	
	// go to screen space
				tx = bx * ppdata->matrices[joint][0] + by * ppdata->matrices[joint][1] + bz * ppdata->matrices[joint][2] + ppdata->matrices[joint][3];
				ty = bx * ppdata->matrices[joint][4] + by * ppdata->matrices[joint][5] + bz * ppdata->matrices[joint][6] + ppdata->matrices[joint][7];
				tz = bx * ppdata->matrices[joint][8] + by * ppdata->matrices[joint][9] + bz * ppdata->matrices[joint][10] + ppdata->matrices[joint][11];
	// and then to world space
				xob = tx * tm[0] + ty * tm[4] + tz * tm[8] + cambot.x;
				yob = tx * tm[1] + ty * tm[5] + tz * tm[9] + cambot.y;
				zob = tx * tm[2] + ty * tm[6] + tz * tm[10] + cambot.z;
	// put the ball in passing hand
				ball_obj.odata.x = xob;
				ball_obj.odata.y = yob;
				ball_obj.odata.z = zob;

				if(pobj->flags & PF_BOTH_HANDS
				//|| (pobj->adata[0].seq_index >= (SEQ_RBND3_B) && pobj->adata[0].seq_index <= (SEQ_RBND3_B))
				//|| pobj->adata[0].seq_index == (SEQ_J_SHT1_R)
				//|| pobj->adata[0].seq_index == (SEQ_J_SHT1_L)
				//|| pobj->adata[0].seq_index == (SEQ_J_SHT2_R)
				//|| pobj->adata[0].seq_index == (SEQ_J_SHT2_L)
				|| (pobj->adata[0].seq_index == (SEQ_K_2HPMP1))
				|| (pobj->adata[0].seq_index == (SEQ_K_2HPMP2))
				|| pobj->adata[0].seq_index == (SEQ_P_OVR_HD)

				|| ((pobj->adata[1].animode & MODE_TWOPART) && pobj->adata[1].seq_index == (SEQ_P_FWD2_B))
				|| ((pobj->adata[1].animode & MODE_TWOPART) && pobj->adata[1].seq_index == (SEQ_P_FWD3_B))
				|| (pobj->adata[0].seq_index == (SEQ_P_FWD2_B))
				|| (pobj->adata[0].seq_index == (SEQ_P_FWD2_B))

				|| ((pobj->adata[1].animode & MODE_TWOPART) && pobj->adata[1].seq_index == (SEQ_P_SID5_R))
				|| ((pobj->adata[1].animode & MODE_TWOPART) && pobj->adata[1].seq_index == (SEQ_P_SID5_L))
				|| (pobj->adata[0].seq_index == (SEQ_P_SID5_R))
				|| (pobj->adata[0].seq_index == (SEQ_P_SID5_L))

				|| pobj->adata[0].seq_index == (SEQ_P_CHEST)
				|| (pobj->adata[0].seq_index >= (SEQ_P_CH1_1P) && pobj->adata[0].seq_index <= (SEQ_P_CH2_1P))
				|| ((pobj->adata[1].animode & MODE_TWOPART) && pobj->adata[1].seq_index >= (SEQ_P_CH1_1P) && pobj->adata[1].seq_index <= (SEQ_P_CH2_1P))
				)
				{			// passing animations with ball in both hands
				float hx,hy,hz,rhx,rhy,rhz,thx,thy,thz,lhx,lhy,lhz;

				// ball's position relative to joint's zero position
					bx = R_INHAND_X_OFF * (pobj->xscale * PSCALE);
					by = R_INHAND_Y_OFF * (pobj->yscale * PSCALE);
					bz = R_INHAND_Z_OFF * (pobj->zscale * PSCALE);
					joint = JOINT_RWRIST;
				// screen space, world space
					tx = bx * ppdata->matrices[joint][0] + by * ppdata->matrices[joint][1] + bz * ppdata->matrices[joint][2] + ppdata->matrices[joint][3];
					ty = bx * ppdata->matrices[joint][4] + by * ppdata->matrices[joint][5] + bz * ppdata->matrices[joint][6] + ppdata->matrices[joint][7];
					tz = bx * ppdata->matrices[joint][8] + by * ppdata->matrices[joint][9] + bz * ppdata->matrices[joint][10] + ppdata->matrices[joint][11];
					rhx = tx * tm[0] + ty * tm[4] + tz * tm[8] + cambot.x;
					rhy = tx * tm[1] + ty * tm[5] + tz * tm[9] + cambot.y;
					rhz = tx * tm[2] + ty * tm[6] + tz * tm[10] + cambot.z;

					bx = L_INHAND_X_OFF * PSCALE;
					by = L_INHAND_Y_OFF * PSCALE;
					bz = L_INHAND_Z_OFF * PSCALE;
					joint = JOINT_LWRIST;
					tx = bx * ppdata->matrices[joint][0] + by * ppdata->matrices[joint][1] + bz * ppdata->matrices[joint][2] + ppdata->matrices[joint][3];
					ty = bx * ppdata->matrices[joint][4] + by * ppdata->matrices[joint][5] + bz * ppdata->matrices[joint][6] + ppdata->matrices[joint][7];
					tz = bx * ppdata->matrices[joint][8] + by * ppdata->matrices[joint][9] + bz * ppdata->matrices[joint][10] + ppdata->matrices[joint][11];
					lhx = tx * tm[0] + ty * tm[4] + tz * tm[8] + cambot.x;
					lhy = tx * tm[1] + ty * tm[5] + tz * tm[9] + cambot.y;
					lhz = tx * tm[2] + ty * tm[6] + tz * tm[10] + cambot.z;
					hx = ppdata->ball_in_hand == RIGHT_HAND ? rhx : lhx;
					hy = ppdata->ball_in_hand == RIGHT_HAND ? rhy : lhy;
					hz = ppdata->ball_in_hand == RIGHT_HAND ? rhz : lhz;

					thx = ppdata->ball_in_hand == RIGHT_HAND ? lhx : rhx;
					thy = ppdata->ball_in_hand == RIGHT_HAND ? lhy : rhy;
					thz = ppdata->ball_in_hand == RIGHT_HAND ? lhz : rhz;
					ball_obj.odata.x = (hx + thx) / 2.0f;
					ball_obj.odata.y = (hy + thy) / 2.0f;
					ball_obj.odata.z = (hz + thz) / 2.0f;
				}
			}//END.ELSE	

// HE'S ON FIRE ! ! !
			if (gi_p->fire_cnt >= FIRECNT)
			{
				if (gi_p->ball_handler == gi_p->fire_player)
					ball_fireset (0, 10, 0);
			}
			else if ((gi_p->ball_handler != NO_BALLHANDLER) && (gi_p->team_fire == player_blocks[gi_p->ball_handler].team))
					ball_fireset (1, 10, 0);
		}//END.IF.ball_handler != NO_BALLHANDLER
		else
		{
			// Nobody has the ball
			if ((game_info.ball_mode == BM_TIPOFF && referee_active) ||
				(game_info.game_mode == GM_PRE_TIPOFF))
			{
				// No matter what, don't fall into ball bounce code!
				ball_obj.odata.y  = BALL_RADIUS;
				ball_obj.odata.vy = 0.0f;

				if (referee_active)
				{
					// put ball in REFEREE hand
					ppdata = &referee_data;
					
					tm = cambot.xform;
					// ball's position relative to joint's zero position
					bx = (R_INHAND_X_OFF * PSCALE);
					by = (R_INHAND_Y_OFF * PSCALE);
					bz = (R_INHAND_Z_OFF * PSCALE);
					joint = JOINT_RWRIST;
					// screen space, world space
					tx = bx * ppdata->matrices[joint][0] + by * ppdata->matrices[joint][1] + bz * ppdata->matrices[joint][ 2] + ppdata->matrices[joint][ 3];
					ty = bx * ppdata->matrices[joint][4] + by * ppdata->matrices[joint][5] + bz * ppdata->matrices[joint][ 6] + ppdata->matrices[joint][ 7];
					tz = bx * ppdata->matrices[joint][8] + by * ppdata->matrices[joint][9] + bz * ppdata->matrices[joint][10] + ppdata->matrices[joint][11];
					rhx = tx * tm[0] + ty * tm[4] + tz * tm[ 8] + cambot.x;
					rhy = tx * tm[1] + ty * tm[5] + tz * tm[ 9] + cambot.y;
					rhz = tx * tm[2] + ty * tm[6] + tz * tm[10] + cambot.z;
					
					ball_obj.odata.x = rhx;
					ball_obj.odata.y = rhy;
					ball_obj.odata.z = rhz;
				}
			}
			else
			{
		
				// the ball is loose
#ifdef	DEBUG_BLOCKS
				if ((game_info.ball_mode == BM_SHOOTING) ||
					 (game_info.ball_mode == BM_BOUNCE_ON_RIM))
				{
					game_info.ball_handler = NO_BALLHANDLER;
					game_info.ball_mode = BM_SHOOTING;
					ball_obj.odata.vx = 0.0f;
					ball_obj.odata.vy = 1.01f;
					ball_obj.odata.vz = 0.0f;
					ball_obj.odata.x = -108.724228f;
					ball_obj.odata.y = RIM_Y+7.0f;
					ball_obj.odata.z = -0.8318752f;
				}
				else
#endif 

//#ifdef	DEBUG_REBOUNDS
//				if ((game_info.ball_mode == BM_SHOOTING) ||
//					 (game_info.ball_mode == BM_BOUNCE_ON_RIM))
//				{
//					game_info.ball_mode = BM_BOUNCE_ON_RIM;
//					ball_obj.odata.vx = 0.0f;
//					ball_obj.odata.vy = 1.01f;
//					ball_obj.odata.vz = 0.0f;
//					ball_obj.odata.x = -108.724228f;
//					//ball_obj.odata.y = 45.859619f;
//					ball_obj.odata.y = RIM_Y - 10.f;
//					ball_obj.odata.z = -0.8318752f;
//				}
//				else
//#endif
				ball_move();

//#ifdef DEBUG_PASSING
				if(game_info.ball_mode == BM_PASSING
				&& ball_obj.flight_time > ball_obj.total_time)
				//&& rob == NO)
				{
				float p[3], *cm = cambot.xform;
					//rob = YES;
					//halt = YES;
				p[0] =	player_blocks[nearest_to_ball(game_info.off_side)].jpos[JOINT_PELVIS][0] * cm[ 0] +
						player_blocks[nearest_to_ball(game_info.off_side)].jpos[JOINT_PELVIS][1] * cm[ 4] +
						player_blocks[nearest_to_ball(game_info.off_side)].jpos[JOINT_PELVIS][2] * cm[ 8] + cambot.x;
				p[1] =	player_blocks[nearest_to_ball(game_info.off_side)].jpos[JOINT_PELVIS][0] * cm[ 1] +
						player_blocks[nearest_to_ball(game_info.off_side)].jpos[JOINT_PELVIS][1] * cm[ 5] +
						player_blocks[nearest_to_ball(game_info.off_side)].jpos[JOINT_PELVIS][2] * cm[ 9] + cambot.y;
				p[2] =	player_blocks[nearest_to_ball(game_info.off_side)].jpos[JOINT_PELVIS][0] * cm[ 2] +
						player_blocks[nearest_to_ball(game_info.off_side)].jpos[JOINT_PELVIS][1] * cm[ 6] +
						player_blocks[nearest_to_ball(game_info.off_side)].jpos[JOINT_PELVIS][2] * cm[10] + cambot.z;
#ifdef DEBUG
				//printf("ERROR: A pass didn't connect and its %2.2f away from player\n",dist3d(&(ball_obj.odata.x), &(player_blocks[nearest_to_ball(game_info.off_side)].odata.ax)));
				printf("ERROR: A pass didn't connect and its %2.2f away from player\n",dist3d(&(ball_obj.odata.x), p));
#endif
					//if(dist3d(&(ball_obj.odata.x), &(player_blocks[nearest_to_ball(game_info.off_side)].odata.ax)) <= 10.0f)
					if(dist3d(&(ball_obj.odata.x), p) <= 15.0f)
					{
						if(!(player_blocks[nearest_to_ball(game_info.off_side)].odata.flags & PF_ALLYDUNKER) ||
							// If reacting, don't catch ball most of time
							//((player_blocks[nearest_to_ball(game_info.off_side)].odata.adata[0].animode & MODE_REACTING) && randrng(100) < 10 ))
							((player_blocks[nearest_to_ball(game_info.off_side)].odata.adata[0].animode & MODE_REACTING) && randrng(100) < 10 ))
						{
							// Grab the ball
							game_info.ball_handler = nearest_to_ball(game_info.off_side);

							if(game_info.game_mode == GM_INBOUNDING)
								clock_active = 1;	   // Restart game clock

							game_info.game_mode    = GM_IN_PLAY;

							// Reset some essentials
							auto_rebounding = NO;
							game_info.passer    = NO_PASSER;
							game_info.ball_mode = BM_DRIBBLING;

							ball_obj.flags   &= ~BF_NOINTERCEPT;

							player_blocks[nearest_to_ball(game_info.off_side)].dribble_mode = DM_CAN_DRIBBLE;
							no_pump_fake = YES;
						}
						else
						{
							if(game_info.game_mode == GM_INBOUNDING)
								clock_active = 1;	   // Restart game clock

							game_info.game_mode = GM_IN_PLAY;
							game_info.ball_mode = BM_FREE_ON_COURT;
							ball_obj.flags   &= ~BF_NOINTERCEPT;
						}
					}
					else
					{
						if(game_info.game_mode == GM_INBOUNDING)
							clock_active = 1;	   // Restart game clock

						game_info.game_mode = GM_IN_PLAY;
						game_info.ball_mode = BM_FREE_ON_COURT;
						ball_obj.flags   &= ~BF_NOINTERCEPT;
					}
				}
				else
				{
					rob = NO;
					//halt = NO;
				}
//#endif

	// if ball is passing or shooting increment flight_time..if ball gets to rim set ball_mode to Rim Physics(3)
				if(game_info.ball_mode == BM_PASSING
					|| game_info.ball_mode == BM_SHOOTING
					|| game_info.ball_mode == BM_OFF_INTO_PLAY)
				{
					if(game_info.ball_mode == BM_PASSING)
						ball_obj.flight_time++;

					// this next condition should only run once
					if(ball_obj.flight_time != -1
						&& ball_obj.flight_time > ball_obj.total_time
						&& game_info.ball_mode == BM_SHOOTING
						)
						game_info.ball_mode = BM_BOUNCE_ON_RIM;
						//val = RimPhysics();
				}

		// ON FIRE ! ! !

				if((game_info.fire_cnt >= 3) &&
						(	 (game_info.ball_mode == BM_PASSING && game_info.passer == game_info.fire_player)
						|| (game_info.ball_mode == BM_SHOOTING && ball_obj.who_shot == game_info.fire_player)
						|| (game_info.ball_mode == BM_OFF_INTO_PLAY && ball_obj.who_shot == game_info.fire_player)
						|| (game_info.ball_mode == BM_BOUNCE_ON_RIM && ball_obj.who_shot == game_info.fire_player))	)
				{
#ifndef DEBUG_NO_FIRE
					if( ball_obj.flight_time % 2 == 0 )
					{
						// create smoke puff and proc
						//qcreate( "smkpuf", PLAYER_PID, ball_fire_trail_proc, 4, 2, 0, 0 );
						//qcreate( "smkpuf", PLAYER_PID, ball_fire_trail_proc, 3,-1, 2, 0 );
						//qcreate( "smkpuf", PLAYER_PID, ball_fire_trail_proc, 3,-1,-2, 0 );
					}
#endif
				}

	// apply gravity to ball
	#if 0
				if(val == 0)
				{
					ball_obj.odata.x += ball_obj.odata.vx;
					ball_obj.odata.y += ball_obj.odata.vy;
					ball_obj.odata.z += ball_obj.odata.vz;
					if(ball_obj.odata.vy != 0.0f || ball_obj.odata.y > BALL_RADIUS)	//0.0f)
					{
						ball_obj.odata.vy += BALL_GRAVITY;
					#if 0
						if(ball_obj.odata.y < BALL_RADIUS)
						{
#ifdef DEBUG
							printf("ball is below BALL_RADIUS\n");
#endif							ball_obj.odata.y = BALL_RADIUS;

							if(game_info.ball_mode == BM_OFF_INTO_PLAY)
								game_info.ball_mode = BM_FREE_ON_COURT;
						}
					#endif	
					}
				}
	#endif
			// ally off the backboard!
			// game_info.passer == ball_obj.int_receiver is a flag to know that this special dunk has been done
				if(game_info.ball_mode == BM_PASSING
					&& ball_obj.flight_time == ball_obj.total_time
					&& game_info.passer == ball_obj.int_receiver)
				{
					ball_obj.flight_time = -1;
					game_info.ball_handler = NO_BALLHANDLER;	// ball is in the air now
					game_info.ball_mode = BM_FREE_ON_COURT;
					SetAllyPassVels( (obj_3d *)(player_blocks + ball_obj.int_receiver) );
					ally_jump_wait = 10;
					//change_anim((obj_3d *)(player_blocks + ball_obj.int_receiver), allyoop_2);
					//printf("BALL_PROC: change_anim(pobj, allyoop_2 )\n");
					game_info.passer = NO_PASSER;
		// off the backboard pass sound
					make_hit_backboard3_snd(VOLUME6);
				}

				if(OOBPasser == NO_PASSER || (OOBPasser != NO_PASSER && game_info.ball_handler == NO_BALLHANDLER))
				{
					//ball_obj.odata.z = LESSER(ball_obj.odata.z,(OUT_OF_BOUNDS_Z - OOB_RADIUS));
					//ball_obj.odata.z = GREATER(ball_obj.odata.z,-(OUT_OF_BOUNDS_Z - OOB_RADIUS));
					//ball_obj.odata.x = LESSER(ball_obj.odata.x,(OUT_OF_BOUNDS_X - OOB_RADIUS));
					//ball_obj.odata.x = GREATER(ball_obj.odata.x,-(OUT_OF_BOUNDS_X - OOB_RADIUS));
					if(ball_obj.odata.vx > 0.0f)
					{
						if(ball_obj.odata.x > OUT_OF_BOUNDS_X - OOB_RADIUS + 2)
						{
							ball_obj.odata.x = OUT_OF_BOUNDS_X - OOB_RADIUS + 2;
							ball_obj.odata.vx = 0;
						}
					}
					else
					{
						if(ball_obj.odata.x < -(OUT_OF_BOUNDS_X - OOB_RADIUS + 2))
						{
							ball_obj.odata.x = -(OUT_OF_BOUNDS_X - OOB_RADIUS + 2);
							ball_obj.odata.vx = 0;
						}
					}
					if(ball_obj.odata.vz > 0.0f)
					{
						if(ball_obj.odata.z > OUT_OF_BOUNDS_Z - OOB_RADIUS)
						{
							ball_obj.odata.z = OUT_OF_BOUNDS_Z - OOB_RADIUS;
							ball_obj.odata.vz = 0;
						}
					}
					else
					{
						if(ball_obj.odata.z < -(OUT_OF_BOUNDS_Z - OOB_RADIUS))
						{
							ball_obj.odata.z = -(OUT_OF_BOUNDS_Z - OOB_RADIUS);
							ball_obj.odata.vz = 0;
						}
					}
				
					if(ball_obj.odata.vy > 0.0f)
					{
						if(ball_obj.odata.y >= 200.0f)
						{
							ball_obj.odata.y = 200.0f;
							ball_obj.odata.vy = -1.0f;
						}
					}
				}
#if 0
				else
				{
		// dont let the ball go too far out of bounds...
					if(game_info.game_mode != GM_IN_PLAY)
					{
						if(ball_obj.odata.vx > 0.0f)
						{
							if(ball_obj.odata.x > OUT_OF_BOUNDS_X - (OOB_RADIUS + 15))
							{
								ball_obj.odata.x = OUT_OF_BOUNDS_X - (OOB_RADIUS + 15);
								ball_obj.odata.vx = 0;
								printf("OOB_X_1\n");
							}
						}
						else
						{
							if(ball_obj.odata.x < -(OUT_OF_BOUNDS_X - (OOB_RADIUS + 15)))
							{
								ball_obj.odata.x = -(OUT_OF_BOUNDS_X - (OOB_RADIUS + 15));
								ball_obj.odata.vx = 0;
								printf("OOB_X_2\n");
							}
						}
						if(ball_obj.odata.vz > 0.0f)
						{
							if(ball_obj.odata.z > OUT_OF_BOUNDS_Z - (OOB_RADIUS + 15))
							{
								ball_obj.odata.z = OUT_OF_BOUNDS_Z - (OOB_RADIUS + 15);
								ball_obj.odata.vz = 0;
								printf("OOB_Z_1\n");
							}
						}
						else
						{
							if(ball_obj.odata.z < -(OUT_OF_BOUNDS_Z - (OOB_RADIUS + 15)))
							{
								ball_obj.odata.z = -(OUT_OF_BOUNDS_Z - (OOB_RADIUS + 15));
								ball_obj.odata.vz = 0;
								printf("OOB_Z_2\n");
							}
						}
					}
				}
#endif
			}
		}// END..ELSE.IF ball is loose

		// ball can't go under ground
		if(game_info.ball_handler == NO_BALLHANDLER
			&& ball_obj.odata.y <= BALL_RADIUS
			&& ball_obj.odata.vy < 0.0f
			)
		{
			if(ball_obj.flags & BF_BOUNCEPASS
				&& game_info.ball_mode == BM_PASSING	// passing
				&& game_info.passer != NO_PASSER
				)
			{
				ball_obj.odata.y = 0.0f + BALL_RADIUS;
				ball_obj.odata.vx = (ball_obj.tx - ball_obj.odata.x) / (float)(ball_obj.total_time - ball_obj.flight_time);
				ball_obj.odata.vy = (ball_obj.ty - BALL_RADIUS) / (float)(ball_obj.total_time - ball_obj.flight_time) + GRAVITY / -2.0f * (float)(ball_obj.total_time - ball_obj.flight_time);
				ball_obj.odata.vz = (ball_obj.tz - ball_obj.odata.z) / (float)(ball_obj.total_time - ball_obj.flight_time);
				if(ball_obj.flight_time > ball_obj.total_time)
				{
					//game_info.passer = NO_PASSER;
					//game_info.ball_mode = BM_FREE_ON_COURT; // bouncing on the court
					//game_info.receiver_a = NO_RECEIVER;
					ball_obj.flags &= ~BF_BOUNCEPASS;	// clear bounce pass flag
				}
				// dribble sound
				if(ball_obj.odata.vy > 0.05)
					if (randrng(100) < 50)
						make_dribble1_snd(VOLUME7);
					else
						make_dribble2_snd(VOLUME7);
			}
			else
			{
		// clear these flags so no one is active receiver and there is no passer
#if NUM_PERTEAM > 2
				game_info.receiver_a = NO_RECEIVER;
				game_info.receiver_b = NO_RECEIVER;
#endif
				game_info.passer     = NO_PASSER;
				if(OOBPasser != NO_PASSER
					&& player_blocks[OOBPasser].odata.plyrmode == MODE_OFFENSE
					&& game_info.ball_mode == BM_PASSING)
				{
					int j = game_info.off_side ? NUM_PERTEAM : 0;
					int k = NUM_PERTEAM;

					while (k--) player_blocks[j++].dribble_mode = DM_CAN_DRIBBLE;

					if(player_blocks[OOBPasser].odata.plyrmode != MODE_OUT_OF_BOUNDS)
						OOBPasser = NO_PASSER;
				}

				ball_obj.flags &= ~BF_NOINTERCEPT;	// make sure this flag is cleared
				//ball_obj.who_shot = NO_SHOOTER;	// player who shot ball can get it now.

				game_info.ball_mode = BM_FREE_ON_COURT; // bouncing on the court
				ignore_steal = 0;

				if(player_blocks[0].odata.plyrmode == MODE_TIPOFF)
				{
					player_blocks[0].odata.plyrmode = MODE_OFFENSE;
					player_blocks[1].odata.plyrmode = MODE_OFFENSE;
					player_blocks[2].odata.plyrmode = MODE_DEFENSE;
					player_blocks[3].odata.plyrmode = MODE_DEFENSE;
					game_info.game_mode = GM_IN_PLAY;
#ifdef DEBUG
					printf("ERROR: TIP OFF PASS WAS MISSED\n");
#endif
				}
				ball_ingnd();

			// These lines might need to be uncommented out...not sure yet.
				//if(game_info.game_mode == GM_INBOUNDING)
				//	game_info.game_mode = GM_IN_PLAY;
	
#if 0
				if(fabs(ball_obj.odata.vy) <= 0.4f && ball_obj.odata.vy < 0.0f)
				{ // max bounces
					ball_obj.odata.vy = 0.0f;
					ball_obj.odata.y = BALL_RADIUS;
				}
				ball_obj.odata.vy *= -1.0f;
				ball_obj.odata.vy *= 0.60f;
				if(ball_obj.odata.vy == 0.0f && ball_obj.odata.y == BALL_RADIUS)
				{
					ball_obj.odata.vx *= 0.99f;
					ball_obj.odata.vz *= 0.99f;
				}
				ball_obj.odata.y = BALL_RADIUS;
				//printf("tx %f, ty %f, tz %f\n", ball_obj.tx, ball_obj.ty, ball_obj.tz);	

				// dribble sound
			if(ball_obj.odata.vy > 0.05)
				if (randrng(100) < 50)
					make_dribble1_snd(VOLUME7);
				else
					make_dribble2_snd(VOLUME7);
#endif
			}
		}
	
		// ball can't go through backboards either
		if(game_info.ball_handler == NO_BALLHANDLER
			&& game_info.ball_mode == BM_FREE_ON_COURT
			&& game_info.passer == NO_PASSER
			&& ball_obj.who_shot == NO_SHOOTER
			&& (ball_obj.odata.y >= 32.070145f && ball_obj.odata.y <= 45.937908f)
			)
		{
			if(ball_obj.odata.x > 0.0f
				&& ball_obj.odata.x-BALL_RADIUS/2.0f < BACKBOARD_X
				&& ball_obj.odata.x+BALL_RADIUS/2.0f > BACKBOARD_X
			)
			{
				ball_obj.odata.x = BACKBOARD_X;
				ball_obj.odata.vx *= -1.0f;
			}
			else if(ball_obj.odata.x < 0.0f
				&& ball_obj.odata.x-BALL_RADIUS/2.0f < -BACKBOARD_X
				&& ball_obj.odata.x+BALL_RADIUS/2.0f > -BACKBOARD_X
			)
			{
				ball_obj.odata.x = -BACKBOARD_X;
				ball_obj.odata.vx *= -1.0f;
			}
		}
		sleep(1);
	}	// END.while
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
//void show_shot_percentage(obj_3d *pobj)
//{
//	curr_shot_pct = compute_shot_pct(pobj);
//	show_shot_pct = 130;
//}
//////////////////////////////////////////////////////////////////////////////
int compute_shot_pct(obj_3d *pobj)
{
int i, team, diff, rnd, base, score0, score1, plyr, limb, winning, seconds, minute;
float dist_to_rim, p[3], *tm, h[3];

	// Allow someone to try a put back dunk again
	put_back_already = 0;

	minute = ((game_info.game_time & 0x00FF0000)>>16);
	seconds = ((game_info.game_time & 0x0000FF00)>>8);

	winning = (score0 > score1);

	plyr = ((bbplyr_data *)pobj)->plyrnum;
	team = ((bbplyr_data *)pobj)->team;

	score0 = compute_score(game_info.off_side);
	score1 = compute_score(1 - game_info.off_side);

	base = get_shot_base((bbplyr_data *)pobj);

	// this is for 3pt lean in jumpers becoming 2pt shots
	if(what_type_shot == JUMPER_3 && three_pt_check(pobj) == NO)
		base += 5;

 	p[0] = game_info.active_goal ? RIM_X : -RIM_X; p[1] = 0.0f; p[2] = 0.0f;
 	dist_to_rim = dist3d(&(pobj->x), p);
	if(what_type_shot == JUMPER_2)
	{
		base -= (int)(20.0f * (dist_to_rim / 70.0f));
	}
	else
	{
	// based on a halfcourt distance of 142 units
		dist_to_rim -= 65.0f;

		base -= (int)(60.0f * (dist_to_rim / 50.0f));		//40  75
	}

// if somebody's in shooters' way decrease the percentage
	i = !team ? NUM_PERTEAM : 0;
	do
	{
		if(in_between_ball_basket(&player_blocks[i]) == YES)
		{
			if(player_blocks[i].odata.adata[0].animode & MODE_REACTING) continue;
			if(distxz[plyr][i] <= 37.0f)	//40
			{
				base -= 10;		//18
				tm = cambot.xform;
				limb = player_blocks[i].handedness == RIGHT_HAND ? JOINT_RWRIST : JOINT_LWRIST;
				h[0] = player_blocks[i].jpos[limb][0] * tm[0] + player_blocks[i].jpos[limb][1] * tm[4] + player_blocks[i].jpos[limb][2] * tm[8] + cambot.x;
				h[1] = player_blocks[i].jpos[limb][0] * tm[1] + player_blocks[i].jpos[limb][1] * tm[5] + player_blocks[i].jpos[limb][2] * tm[9] + cambot.y;
				h[2] = player_blocks[i].jpos[limb][0] * tm[2] + player_blocks[i].jpos[limb][1] * tm[6] + player_blocks[i].jpos[limb][2] * tm[10] + cambot.z;

				//printf("dist3d(ball, h) = %3.3f, ball:%3.3f,%3.3f,%3.3f  %s h:%3.3f,%3.3f,%3.3f\n", dist3d(&(ball_obj.odata.x), h), ball_obj.odata.x, ball_obj.odata.y, ball_obj.odata.z,  limb == JOINT_RWRIST ? "RT" : "LT", h[0], h[1], h[2]);
				if(dist3d(&(ball_obj.odata.x), h) <= 25.0f)
				{
					if(h[1] >= ball_obj.odata.y)
					{
						//printf("hand y higher than ball\n");
						base -= 27;
					}
					if(dist3d(&(ball_obj.odata.x), h) <= 8.0f)
					{
						//printf("hand is real close to ball\n");
						base -= 25;
					}
				}
			}
		}
	}
	while(++i % NUM_PERTEAM);

// maybe do specific checks for different types of shots...

// If hookshot, increase %




// If layup, increase %
	if((pobj->flags & (PF_LAYUP)) || what_type_shot == LAYUP)
	{
		base += 15;		//30
//	 	printf("Layup!  Increase shot percentage!\n");
	}

// Check to see if he has missed 3 in a row.  If so, make next shot 90%!


// if this guy is on fire...
	if(plyr == game_info.fire_player
	&& game_info.fire_cnt >= FIRECNT
	&& plyr_crossed_halfcourt(plyr) == YES)
		base += 60;
	else if ((game_info.team_fire == player_blocks[plyr].team)
		 && (plyr_crossed_halfcourt(plyr) == YES))
		 base += 60;

	else if ((plyr_launched_from_hotspot(plyr) == YES) && (game_info.hotspot_made_cnt[plyr] == 0))
		 // First shot from hotspot
		 base += 75;
	else if ((plyr_launched_from_hotspot(plyr) == YES)
		 && (game_info.hotspot_made_cnt[plyr] >= HOTSPOT_ACTIVE_CNT)
		 // Don't bump up hot spot % is spot is too far away - create a player cheaters...
		 && (dist_to_rim <= 90))
		 	base += 30;		//25
	else if(plyr_crossed_halfcourt(plyr) == NO)
		base = 3;


// adjust for missing multiple shots in a row
	if ((player_blocks[plyr].shots_missed >= 3) && (dist_to_rim <= 120))
	{
		base += 40;
#if DEBUG
	printf("shots missed is greater than 3 and BASE = %d \n",base);
#endif
	}


// clock adjustment shot percentaging here
// If this will cause a tie game, bump up %
// FIX!!!
	if(!pup_tourney)
	{
	 	if(minute == 0 && seconds <= 4)
		{
			if(game_info.game_quarter >= 3)
			{
		// if shot is going to tie the game bump up the chances
			 	if(what_type_shot == JUMPER_3
			 	&& (score0 + 3) == score1
				&& plyr_crossed_halfcourt(plyr) == YES)
					base += 30;
				else
					base += 10;
			
		// if shot is to win game and scores are tied decrease chances
				if(score0 == score1)
					base = GREATER(base - 15, 2);
			}

		// in first 3 qtrs if team is losing bump up chances
			if(game_info.game_quarter < 3)
			{
			 	if(what_type_shot == JUMPER_3
				&& score0 < score1)
				{
					if(plyr_crossed_halfcourt(plyr) == NO)
						base = GREATER(10, base);
					else
						base += 15;
				}
			}
		}

	}

	base = GREATER(3, base);
	base = LESSER(95, base);

	return base;
}

//////////////////////////////////////////////////////////////////////////////
void freethrow_release(obj_3d *pobj)
{
	bbplyr_data *ppdata = (bbplyr_data *)pobj;
	float p[3];
	int angle, score_diff, rnd = 0,i;


	// always score a 3 point shot!! (if make)
	what_type_shot = JUMPER_3;

	if(game_info.ball_handler == ppdata->plyrnum)
	{
		ball_obj.who_shot = ppdata->plyrnum;
		game_info.ball_handler = NO_BALLHANDLER;

		// value from 
		if(rand()%100 <= ftmtr_per)
			IsShotGood = YES;
		else
			IsShotGood = NO;
		
		p[0] = game_info.active_goal ? RIM_X : -RIM_X; p[1] = 0.0f; p[2] = 0.0f;
		angle = ptangle(&ball_obj.odata.x, p);

//		RimBounceFlavor = SHTMD_RIM + randrng(SHTMD_RIMMAX);
		RimBounceFlavor = SHTMD_RIM;

		i = 0;
		if (IsShotGood)
			i = M_SHTMD_GOOD;

//		// Put rules on air ball
//		if(IsShotGood == NO && randrng(100) == 2)
//		{
//			RimBounceFlavor = SHTMD_AIR;
//			// Crowd stomps
//			qcreate( "air",  0, airball_proc, 0, 0, 0, 0);
//		}

		if ((IsShotGood == YES) &&
			(ppdata->plyrnum == game_info.fire_player && game_info.fire_cnt >= FIRECNT) &&
			(randrng(100) <= 90))
		{
			RimBounceFlavor = SHTMD_NET;
		}

		// added for speech calls
		gRim_hits = 0;
		gBack_board_hits = 0;

		ball_shoot (pobj, RimBounceFlavor | i);
	}

	// allow this player to dribble
	ppdata->dribble_mode = DM_CAN_DRIBBLE;	// -=Can't dribble, 0=Can drib, +=Dribbling

	// inc. game stat
	plyr_stat_ptrs[ppdata->plyrnum]->three_ptrs_attempted++;

	for(i = 0; i < NUM_PLAYERS; i++)
		head_wait[i] = 0;
}

//------------------------------------------------------------------------------------------------------------------------------
//	This process welcomes the player with some crowd stomps
//
// 	INPUT:	Nothing
//	OUTPUT:	Nothing
//------------------------------------------------------------------------------------------------------------------------------
static void airball_proc(int *args)
{
	sleep(80);
	// Airball
	// FIX!!!  Need outdoor airball speech
	if (COURT_IS_INDOORS)
//	if (1)
		snd_scall_bank(crowd_bnk_str, 1 , VOLUME6, 127, PRIORITY_LVL4);
}

//////////////////////////////////////////////////////////////////////////////
void Shot_Release(obj_3d *pobj)
{		// This is called by dunks also
	bbplyr_data *ppdata = (bbplyr_data *)pobj;
	float p[3];
	int angle, score_diff, rnd = 0;
	int my_score = compute_score(game_info.off_side);
	int his_score = compute_score(1 - game_info.off_side);
	int i;

	if(pobj->flags & (PF_DUNKER|PF_ALLYDUNKER))
	{
		// don't allow anyone to grab ball while in its falling into hoop on a dunk
		// This gets rid of goaltending a dunk
		player_blocks[0].no_ball_collide = 7;
		player_blocks[1].no_ball_collide = 7;
		player_blocks[2].no_ball_collide = 7;
		player_blocks[3].no_ball_collide = 7;

#ifdef DEBUG
	 	printf("Don't attach to ball because I'm dunking!\n");
#endif

		// dont allow a dunker to attach to a ball after attempt
		ppdata->no_ball_collide = 20;
	}
	// dont allow a dunker to attach to a ball after attempt
//	((bbplyr_data *)pobj)->no_ball_collide = 20;

	// cocky dribble bookkeeping...
	ticks_since_cocky = -COCKY_DRIBBLE_TWAIT, cocky_dribbler = NO_BALLHANDLER;

	if( (pobj->flags & PF_PASSING && pobj->flags & (PF_LAYUP|PF_DUNKER|PF_ALLYDUNKER|PF_SHOOTING)) )
	{
		pobj->flags &= ~(PF_LAYUP|PF_DUNKER|PF_ALLYDUNKER);
		return;
	}

// make sure player has ball..on an allyoop the ball might not have been caught...
	if(game_info.ball_handler == ppdata->plyrnum)
	{
		ball_obj.who_shot = ppdata->plyrnum;
		game_info.ball_handler = NO_BALLHANDLER;
#if NUM_PERTEAM > 2
		game_info.receiver_a   = NO_RECEIVER;
		game_info.receiver_b   = NO_RECEIVER;
#endif
		p[0] = game_info.active_goal ? RIM_X : -RIM_X; p[1] = 0.0f; p[2] = 0.0f;
		angle = ptangle( &ball_obj.odata.x, p);
		if(game_info.active_goal == 1)
		{
			if(angle <= 480 && angle >= 32)
			{
				RimBounceFlavor = SHTMD_RIM + randrng(SHTMD_RIMMAX);
			}
			else
			{
				RimBounceFlavor = SHTMD_RIM;
			}
		}
		else
		{
			if(angle >= 544 && angle <= 992)
			{
				RimBounceFlavor = SHTMD_RIM + randrng(SHTMD_RIMMAX);
			}
			else
			{
				RimBounceFlavor = SHTMD_RIM;
			}
		}

		if(RimBounceFlavor == SHTMD_NET && IsShotGood == NO)
			IsShotGood = YES;

		rnd = compute_shot_pct(pobj);
		if(rand()%100 <= rnd)
			IsShotGood = YES;
		else
		{
			IsShotGood = NO;
			if (plyr_crossed_halfcourt(ppdata->plyrnum) == YES)
			{
				ppdata->shots_missed++;
#if DEBUG
	printf("shots missed inc for player %d \n",ppdata->plyrnum);
#endif
			}
		}

	// display shot percentage...
#ifndef DEBUG_SHOT_PERCENTAGE
		if(pup_show_shotper & (1 << ppdata->plyrnum))
		{
			curr_shot_pct = rnd;
			show_shot_pct = 130;
		}
#endif

#ifdef	DEBUG_REBOUNDS
		IsShotGood = NO;
#endif

		i = 0;
		if (IsShotGood) i = M_SHTMD_GOOD;

// 15% of the time a good shot will be a straight net shot
			
		if(IsShotGood == YES && randrng(100) <= 20 && curr_shot_pct > 50)
			RimBounceFlavor = SHTMD_NET;

		// FIX!!!  Add in a few airballs when this is a full court shot
		if(IsShotGood == NO
		&& ppdata->plyrnum != game_info.fire_player
		&& (game_info.team_fire != player_blocks[ppdata->plyrnum].team)
		&& randrng(100) == 1)
		
		{
			RimBounceFlavor = SHTMD_AIR;
			// Crowd stomps
			qcreate( "air",  0, airball_proc, 0, 0, 0, 0);
		}

		if(IsShotGood == YES
		&& ppdata->plyrnum == game_info.fire_player
		&& game_info.fire_cnt >= FIRECNT
		&& randrng(100) <= 95)
			RimBounceFlavor = SHTMD_NET;

		else
		if(IsShotGood == YES
		&& (game_info.team_fire == player_blocks[ppdata->plyrnum].team)
		&& randrng(100) <= 95)
			RimBounceFlavor = SHTMD_NET;

		// added for speech calls
		gRim_hits = 0;
		gBack_board_hits = 0;

		ball_shoot(pobj, RimBounceFlavor | i);

		// don't allow offensive collisions with ball while in its route to the hoop...
		player_blocks[ppdata->plyrnum].no_ball_collide = ball_obj.total_time + 3;
		player_blocks[ppdata->plyrnum ^ 1].no_ball_collide = ball_obj.total_time + 3;

		// don't let the dunker collide with a missed dunk while ball is on the rim
		if(pobj->flags & (PF_DUNKER|PF_ALLYDUNKER) || (what_type_shot == DUNK))
			ppdata->no_ball_collide = 16;	//12

	// display shot percentage...
#ifndef DEBUG_SHOT_PERCENTAGE
		if(pup_show_shotper & (1 << ppdata->plyrnum))
		{
			curr_shot_pct = rnd;
			show_shot_pct = 130;
		}
#endif
	}
// allow this player to dribble
	ppdata->dribble_mode = DM_CAN_DRIBBLE;	// -=Can't dribble, 0=Can drib, +=Dribbling

	for(i = 0; i < NUM_PLAYERS; i++)
		head_wait[i] = 0;

	// inc. game stats
	if (what_type_shot == JUMPER_3)
		plyr_stat_ptrs[ppdata->plyrnum]->three_ptrs_attempted++;
	else if (what_type_shot == DUNK)
		plyr_stat_ptrs[ppdata->plyrnum]->dunks_attempted++;
	else
		plyr_stat_ptrs[ppdata->plyrnum]->two_ptrs_attempted++;

// reset the shot clock only when ball hits rim - could be airball
//	reset_shotclock = YES;
}


//-------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------
int plyr_launched_from_hotspot(int plyr)
{
//	float hsx,hsz;
	bbplyr_data *ppdata;

#ifdef NO_HOTSPOTS
	return NO;
#endif

	if (pup_no_hotspots)
		return NO;

	ppdata = player_blocks + plyr;

	// this is used, cause player could've launced from spot then floated outta it
	if (ppdata->launched_from_hotspot)
		return YES;

//	hsx = (hotspot_xzs[ppdata->p_spd->hotspot][0] * (ppdata->team ? 1 : -1));
//	hsz = hotspot_xzs[ppdata->p_spd->hotspot][1];
//	hsx -= ppdata->odata.x;
//	hsz -= ppdata->odata.z;

//	if ((hsx*hsx + hsz*hsz) < (HOTSPOT_RADIUS*HOTSPOT_RADIUS))
//		return YES;

	return NO;
}

//-------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------
void load_referee_txtrs(void)
{
	player_team team_info;
	player_textures tex;
	char *files[NUM_PLYRTEXTURES + 1] = {tex.flesh_texture, tex.head_texture, tex.jersey_texture,
									tex.jnum_texture, tex.reflect_texture, NULL};
	int ratios[NUM_PLYRTEXTURES];
	
	team_info.my_team = 0;
	team_info.opp_team = 0;
	team_info.jersey = DEFAULT_UNIFORM_TEAM;
	team_info.jersey_style = AWAY_JERSEY;
	get_player_textures(&team_info, TEAM_SECRET, REF_CHARACTER, &tex, FALSE);
	load_head_textures(files, ratios, referee_decals, NUM_PLYRTEXTURES,
				  TXID_REFEREE, GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR, 1);
}

//-------------------------------------------------------------------------------------------------------------------------------
static void check_pup(int *pup)
{
	if (four_plr_ver) {
		if ((*pup & 0x1) || (*pup & 0x2))
			*pup |= 0x3;
		if ((*pup & 0x4) || (*pup & 0x8))
			*pup |= 0xC;
	} else {
		int p1, p2;
		
		p1 = *pup & 0x1;
		p2 = *pup & 0x4;
		if (p1)
			*pup = 0x03;
		if (p2)
			*pup |= 0x0C;
	}
}	/* check_pup */

//-------------------------------------------------------------------------------------------------------------------------------
void load_plyr_txtrs(int team1, int team2)
{
	player_team team_info;
	player_textures tex;
	char *files[] = {tex.head_texture, tex.flesh_texture, tex.jersey_texture,
					tex.jnum_texture, tex.reflect_texture, NULL};
	int ratios[NUM_PLYRTEXTURES], i;
	int jersey_city, jersey_style;
	
	check_pup(&pup_home_jersey);
	check_pup(&pup_away_jersey);
	check_pup(&pup_alt_jersey);
	check_pup(&pup_midway_jersey);
	for (i = 0; i < NUM_PLAYERS; i++) {
		team_info.my_team = (i < NUM_PERTEAM) ? team1 : team2;
		team_info.opp_team = (i < NUM_PERTEAM) ? team2 : team1;

		jersey_city = DEFAULT_UNIFORM_TEAM;
		jersey_style = (i < NUM_PERTEAM) ? HOME_JERSEY : away_team_style;
		if (!pup_team_jersey) {
			if ((selected_players[i].record_type == CREATP_RECORD) && (selected_players[i].cmos_data.privileges & PRIV_JERSEY)) {
				jersey_city = selected_players[i].cmos_data.jersey_city;
				jersey_style = selected_players[i].cmos_data.jersey_style;
			} else {
				if ((pup_midway_jersey & (1 << i)) != 0)
					jersey_city = MIDWAY_UNIFORM_TEAM;
				if ((pup_alt_jersey & (1 << i)) != 0)
					jersey_style = ALTERNATE_JERSEY;
				else if ((pup_home_jersey & (1 << i)) != 0)
					jersey_style = HOME_JERSEY;
				else if ((pup_away_jersey & (1 << i)) != 0)
					jersey_style = AWAY_JERSEY;
			}
		}
		/* force the special teams into the black midway uniform */
		if ((alt_ladder_active) && (((cpu_team == 0) && (i < NUM_PERTEAM)) || ((cpu_team == 1) && (i >= NUM_PERTEAM)))) {
			int human_team;
			
			human_team = game_info.team[!cpu_team];
			jersey_city = MIDWAY_UNIFORM_TEAM;
			if ((human_team == TEAM_CLE) || (human_team == TEAM_MIA) || (human_team == TEAM_PHI) ||
				(human_team == TEAM_POR) || (human_team == TEAM_SAC) || (human_team == TEAM_SAN))
				jersey_style = HOME_JERSEY;
			else
				jersey_style = AWAY_JERSEY;
		}
		team_info.jersey = jersey_city;
		team_info.jersey_style = jersey_style;
		get_player_textures(&team_info, selected_players[i].team, selected_players[i].player_index, &tex, selected_players[i].record_type == CREATP_RECORD);
		load_head_textures(files, ratios, &team_decals[i][0], NUM_PLYRTEXTURES,
					  TXID_HEADS, GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR, 0);
	}
}

//-------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------
#ifdef DEBUG_PLAYERS
void debug_load_plyr_txtrs(int team1)
{
	player_team team_info;
	player_textures tex;
	char *files[NUM_PLYRTEXTURES + 1] = {tex.head_texture, tex.flesh_texture, tex.jersey_texture,
					tex.jnum_texture, tex.reflect_texture, NULL};
	int ratios[NUM_PLYRTEXTURES], i, away_style;
	
	if (randrng(100) <= 20)
		away_style = ALTERNATE_JERSEY;
	else
		away_style = AWAY_JERSEY;
	for (i = 0; i < teaminfo[game_info.team[0]].num_player; i++) {
		team_info.my_team = team1;
		team_info.opp_team = (team1 + 1) % NUM_TEAMS; 
		if (selected_players[i].record_type == CREATP_RECORD) {
			team_info.jersey = selected_players[i].cmos_data.jersey_city;
			team_info.jersey_style = selected_players[i].cmos_data.jersey_style;
		} else {
			team_info.jersey = DEFAULT_UNIFORM_TEAM;
			team_info.jersey_style = HOME_JERSEY;
		}
		get_player_textures(&team_info, selected_players[i].team, selected_players[i].player_index, &tex, selected_players[i].record_type == CREATP_RECORD);
		load_head_textures(files, ratios, &team_decals[i][0], NUM_PLYRTEXTURES,
					  TXID_HEADS, GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR, 0);
	}
}
#endif

//////////////////////////////////////////////////////////////////////////////
#if 1
/********************************
 Find angle on XZ plane from 2nd 3D point to 1st
 (* pt 1, * pt 2)
> Angle (0-1023) (0=6 oclock, 256=3 oclock)
********************************/
int	ptangle (float *p1, float *p2)
{
	float		dx, h, sin;
	int		angle;

	float		fx, fz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	fx = p2[0] - p1[0];
	fz = p2[2] - p1[2];

	h = fsqrt (fx * fx + fz * fz);
	if (h == 0) return (0);		// Prevent div by 0

	dx = fabs (fx);
	sin = dx / h;

	angle = iasin (sin);

	if (fz < 0) {

		if (fx < 0)
			return (512 + angle);

		return (512 - angle);
	}
	if (fx < 0)
		return (1024 - angle);

	return (angle);
}
#else
int ptangle( float *p1, float *p2 )
{
	float	dx,h,sin;
	int		angle;

	h = fsqrt((p2[0] - p1[0]) * (p2[0] - p1[0]) +
			  (p2[2] - p1[2]) * (p2[2] - p1[2]));
	dx = fabs( p2[0] - p1[0] );
	sin = dx / h;

	angle = iasin( sin );

	if( p2[2] < p1[2] )
	{
		if( p2[0] < p1[0] )
			return( 512 + angle );
		else
			return( 512 - angle);
	}
	else
	{
		if( p2[0] < p1[0] )
			return( 1024 - angle );
		else
			return( angle );
	}

	return( angle );
}
#endif
//////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG
static void dump_joint_positions( void )
{
	int		i, j;
	float	rx,ry,rz, *tm, tx,ty,tz;
	bbplyr_data *ppdata;
	obj_3d	*pobj;

	tm = cambot.xform;

	for( i = 0; i < 1; i++ )
	{
		ppdata = player_blocks + i;
		pobj = (obj_3d *)ppdata;

		fprintf( stderr, "%2x: %10s:%d\n",
				ppdata->p_spd->number,
				seq_names[pobj->adata[0].seq_index], pobj->adata[0].iframe );

		for( j = 0; j < 15; j++ )
		{
			rx = ppdata->jpos[j][0];
			ry = ppdata->jpos[j][1];
			rz = ppdata->jpos[j][2];
			tx = rx * tm[0] + ry * tm[4] + rz * tm[8] + cambot.x;
			ty = rx * tm[1] + ry * tm[5] + rz * tm[9] + cambot.y;
			tz = rx * tm[2] + ry * tm[6] + rz * tm[10] + cambot.z;

			tx -= ppdata->odata.x;
			ty -= ppdata->odata.y;
			tz -= ppdata->odata.z;

			fprintf( stderr, "  joint %d: %f,%f,%f\n", j, tx, ty, tz );
		}
	}
}
#endif

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
void get_joint_position(obj_3d *pobj, int sequence, int frame, int limb, int dir, float *x, float *y, float *z)
{
bbplyr_data *ppdata = (bbplyr_data *)pobj;
float offx,offy,offz,rx,ry,rz;

	if(sequence < 0)	// use current sequence
		sequence = pobj->adata[0].seq_index;

	if(frame < 0)
		frame = pobj->adata[0].iframe;

// get correct hand offsets
	get_frame( sequence, frame, qpq, NULL );
	roty( 0, qmat );
	qmat[3] = anix - cambot.x;
	qmat[7] = aniy - cambot.y;
	qmat[11] = aniz - cambot.z;
	mxm( qcam2, qmat, qcm );
	qcm[0] *= pobj->xscale; qcm[1] *= pobj->yscale; qcm[ 2] *= pobj->zscale;
	qcm[4] *= pobj->xscale; qcm[5] *= pobj->yscale; qcm[ 6] *= pobj->zscale;
	qcm[8] *= pobj->xscale; qcm[9] *= pobj->yscale; qcm[10] *= pobj->zscale;
	generate_matrices( bbplyr_skel, qcm, qpq, qmatrices[0], qjpos[0], ppdata->f_headsc, ppdata->plyrnum );

// get target x,y,z (in camera coords)
	rx = qjpos[limb][0];
	ry = qjpos[limb][1];
	rz = qjpos[limb][2];

// transform target x,y,z into world coords
	offx = rx * qcam2[0] + ry * qcam2[4] + rz * qcam2[ 8] + cambot.x;
	offy = rx * qcam2[1] + ry * qcam2[5] + rz * qcam2[ 9] + cambot.y;
	offz = rx * qcam2[2] + ry * qcam2[6] + rz * qcam2[10] + cambot.z;

	offx -= anix;
	//offy -= aniy;
	offz -= aniz;

// return rotated joint positions
	*x = FRELX(offx, offz, dir);
	*y = offy;
	*z = FRELZ(offx, offz, dir);
}

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
void get_joint_position_rel(obj_3d *pobj, int sequence, int frame, int limb, int dir, float *x, float *y, float *z, float relx, float rely, float relz)
{
bbplyr_data *ppdata = (bbplyr_data *)pobj;
float offx,offy,offz,rx,ry,rz;

	if(sequence < 0)	// use current sequence
		sequence = pobj->adata[0].seq_index;

	if(frame < 0)
		frame = pobj->adata[0].iframe;

// get correct hand offsets
	get_frame( sequence, frame, qpq, NULL );
	roty( 0, qmat );
	qmat[3] = anix - cambot.x;
	qmat[7] = aniy - cambot.y;
	qmat[11] = aniz - cambot.z;
	mxm( qcam2, qmat, qcm );
	qcm[0] *= pobj->xscale; qcm[1] *= pobj->yscale; qcm[ 2] *= pobj->zscale;
	qcm[4] *= pobj->xscale; qcm[5] *= pobj->yscale; qcm[ 6] *= pobj->zscale;
	qcm[8] *= pobj->xscale; qcm[9] *= pobj->yscale; qcm[10] *= pobj->zscale;
	generate_matrices( bbplyr_skel, qcm, qpq, qmatrices[0], qjpos[0], ppdata->f_headsc, ppdata->plyrnum );

// get target x,y,z (in camera coords)
// xform rel point into camera coords
	rx = relx * qmatrices[limb][0] + rely * qmatrices[limb][1] + relz * qmatrices[limb][02] + qmatrices[limb][ 3];
	ry = relx * qmatrices[limb][4] + rely * qmatrices[limb][5] + relz * qmatrices[limb][06] + qmatrices[limb][ 7];
	rz = relx * qmatrices[limb][8] + rely * qmatrices[limb][9] + relz * qmatrices[limb][10] + qmatrices[limb][11];

// xform rel point x,y,z into world coords
	//qtm = cambot.xform;
	offx = rx * qcam2[0] + ry * qcam2[4] + rz * qcam2[ 8] + cambot.x;
	offy = rx * qcam2[1] + ry * qcam2[5] + rz * qcam2[ 9] + cambot.y;
	offz = rx * qcam2[2] + ry * qcam2[6] + rz * qcam2[10] + cambot.z;

	offx -= anix;
	//offy += aniy;
	offz -= aniz;

// return rotated joint positions
	*x = FRELX(offx, offz, dir);
	*y = offy;
	*z = FRELZ(offx, offz, dir);

#if 0
printf("anix %3.3f, aniy %3.3f, aniz %3.3f\n",anix,aniy,aniz);
delete_object_id(OID_PLAYER + 15);
create_object( "dummy", OID_PLAYER + 15, OF_NONE, DRAWORDER_PLAYER, (void *)ppdata, draw_player_dummy);
#endif
}

//////////////////////////////////////////////////////////////////////////////
void draw_player_dummy(void *oi)
{
	obj_3d	*pobj;
	bbplyr_data	*ppdata;

	pobj = (obj_3d *)oi;
	ppdata = (bbplyr_data *)oi;

	grConstantColorValue( (255 << 24) );
	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);

	grDepthBiasLevel( 0 );
	render_node_mpc(bbplyr_skel, ppdata->body_model, qmatrices[0], pobj->pdecal, qjpos[0]);
}

//////////////////////////////////////////////////////////////////////////////
void change_possession(void)
{
	register bbplyr_data * ppd = player_blocks + NUM_PLAYERS;
	register game_info_t * pgi = &game_info;
	int	min = (game_info.game_time & 0x00FF0000)>> 16,
		sec = (game_info.game_time & 0x0000FF00) >> 8;

#ifdef DEBUG_EXTREME
	for(i = 0; i < debug_num; i++) fprintf(stderr, " ");
	fprintf(stderr, "ENTER: change_possession %d\n",debug_num);
#endif

	pgi->active_goal = 1 - pgi->active_goal;
	pgi->off_side    = 1 - pgi->off_side;

// trying to get rid of spastic fullcourt shots
	if(game_info.ball_handler != NO_BALLHANDLER
	&& plyr_crossed_halfcourt(player_blocks[game_info.ball_handler].plyrnum) == NO)
	{
		if(player_blocks[game_info.ball_handler].odata.plyrmode == MODE_DEFENSE)
		{
			ignore_shoot = ISDRONE(&player_blocks[game_info.ball_handler]) ? 15 : 10;
		}
	}

// give the person who got the ball a 20%(???) turbo boost...
	if(game_info.ball_handler != NO_BALLHANDLER
	&& ISHUMAN(&player_blocks[game_info.ball_handler]))
		pdata_blocks[game_info.ball_handler].turbo = LESSER(pdata_blocks[game_info.ball_handler].turbo * 1.2f, 1.0f);

	do
	{
		ppd--;
		ppd->enemy_player = -1;
		ppd->odata.plyrmode = (ppd->odata.plyrmode == MODE_OFFENSE) ? MODE_DEFENSE : MODE_OFFENSE;
		// reset hotspot stuff
		ppd->launched_from_hotspot = 0;
		ppd->action = 0;	// reset players' juke count
	}
	while (ppd != player_blocks);

#if NUM_PERTEAM > 2
	pgi->receiver_a  = NO_RECEIVER;
	pgi->receiver_b  = NO_RECEIVER;
#endif
	
	update_status_box = 1;	// update score
	reset_shotclock = YES;
	shotclock_active = 1;
	
	ball_obj.flags &= ~BF_NOINTERCEPT;	// make sure this flag is cleared
	
#ifdef DEBUG_EXTREME
	for(i = 0; i < debug_num; i++) fprintf(stderr, " ");
	fprintf(stderr, "LEAVE: change_possession %d\n",debug_num);
#endif
}

//////////////////////////////////////////////////////////////////////////////
// FIX!!  Need alternate inbound that will take place on sideline.
// The alternate inbound happens on shot clock violations
void SetUpOOBounds(int location,int goaltended,int threw_freethrow)
{
	register bbplyr_data * ppd = player_blocks + NUM_PLAYERS;
	//int i, my_score, his_score, score_diff;
	int i;

// clear the drone allyoop count
	drone_attempt_allyoop = 0;

// clear ignore_pass and ignore_steal...
	ignore_pass = ignore_steal = ignore_shoot = 0;

// make sure this flag is cleared
	ball_obj.flags &= ~BF_NOINTERCEPT;

// stop the shotclock from running
	show_score_plate = 2 * tsec-30;//4 * tsec-20;
//	show_score_plate = 6 * tsec-30;

	game_info.off_scored  = game_info.off_side;

// was this player cocky dribbling...
	ticks_since_cocky = -COCKY_DRIBBLE_TWAIT, cocky_dribbler = NO_BALLHANDLER;

// this is an idiot check...
//	qcreate("clear rim hang", PLAYER_PID, clear_rim_hangs_proc, 0, 0, 0, 0);
#ifdef DEBUG_DONT_ALLOW_OOB
#ifdef NO_TIP
	if ( !(location & 0x10000) )
#endif
	return;
#endif

	allow_cocky_oob = randrng(100) <= 30 ? YES : NO;

	shotclock_active = 0;

	do
	{
		ppd--;
		ppd->no_ball_collide = 30;
		ppd->flail_time = 0;
		ppd->dribble_mode = DM_CAN_DRIBBLE;
		ppd->action = 0;	// reset players' juke count

		if (!threw_freethrow)
		{
			if (ppd->team == game_info.off_side)
				// Switch offensive team into defensive mode
				ppd->odata.plyrmode = MODE_DEFENSE;
			else
			{
				// Switch previous defensive team into out of bounds mode
				ppd->odata.flags &= ~(PF_PLYRFLAGS & ~PF_CELEBRATE);
				ppd->odata.plyrmode = MODE_OUT_OF_BOUNDS;
				ppd->attack_mode = 0;
				ppd->attack_time = 0;
				ppd->enemy_player   = -1;
			}
		}
		else
		{
			if (ppd->team == game_info.off_side)
			{
				ppd->odata.flags &= ~(PF_PLYRFLAGS & ~PF_CELEBRATE);
				ppd->odata.plyrmode = MODE_OUT_OF_BOUNDS;
				ppd->attack_mode = 0;
				ppd->attack_time = 0;
				ppd->enemy_player   = -1;
			}
			else
				ppd->odata.plyrmode = MODE_DEFENSE;
		}
		// Reinit player flags
		ppd->auto_control   = YES;
	}
	while (ppd != player_blocks);

	if (!threw_freethrow)
	{
		game_info.active_goal = 1 - game_info.active_goal;
		game_info.off_side    = 1 - game_info.off_side;
	}
#if NUM_PERTEAM > 2
	game_info.receiver_a  = NO_RECEIVER;
	game_info.receiver_b  = NO_RECEIVER;
#endif

	if ((goaltended) && (game_info.ball_handler != NO_BALLHANDLER))
	{
		OOBPasser = game_info.ball_handler;
	}
	else
	{
		OOBPasser = nearest_drone_to_ball(game_info.off_side);
		if (OOBPasser < 0)
			OOBPasser = nearest_to_ball(game_info.off_side);
	}

	OOB_XZ[0] = OOB_XZ[1] = OOB_XZ[2] = 0.0f;

	// turn on auto control for player who is to get the ball
	ppd[OOBPasser].dribble_mode = DM_CAN_DRIBBLE;

	// clear misc variables
	ally_player = -1;

	game_info.game_mode = GM_INBOUNDING;

	// Tell cambot the location or -1 if it was goaltend
	ib_mode = goaltended ? -1 : location;

//printf("called drone_inboundinit with %d\n",OOBPasser);
	drone_inboundinit(location);
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
static void clear_rim_hangs_proc(int *parg)
{
int i;

	sleep(4 * 60);
	for(i = 0; i < NUM_PLAYERS; i++)
		player_blocks[i].HangingOnRim = NO;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
static int _mode_out_of_bounds(bbplyr_data *ppdata)
{
	obj_3d	*pobj = (obj_3d *)ppdata;
	float vel;

	pobj->turn_rate  = 64;
	pobj->turn_rate2 = 64;

	vel = fsqrt((pobj->vx * pobj->vx) + (pobj->vz * pobj->vz));
	if(pobj->adata[0].animode & MODE_END
	|| (vel == 0.0f
			&&
			(pobj->adata[0].seq_index == (SEQ_WAIT_1)
			|| pobj->adata[0].seq_index == (SEQ_WAIT_3)
			|| pobj->adata[0].seq_index == (SEQ_S_READY))) )
	{
//#ifdef DEBUG
//		printf("ERROR: %s is not animating properly SEQ:%s\n", ppdata->p_spd->szlast, seq_names[pobj->adata[0].seq_index]);
//#endif
		change_anim(pobj, breath_anims[randrng(sizeof(breath_anims)/sizeof(char *))]);
	}

	if(game_info.ball_mode != BM_PASSING)
		ppdata->odata.flags &= ~(PF_PLYRFLAGS);

// turn toward target heading
	turn_toward_tgt( pobj );

	if(!( pobj->adata[0].animode & MODE_UNINT ))
	{
		// we're not in MODE_UNINT
		offense_run( pobj );
	}// END.IF !UNINT

// If ball is loose and i'm close...get it
	if(game_info.ball_handler == NO_BALLHANDLER)
	{
		if(ppdata->no_ball_collide <= 0 && ppdata->plyrnum == OOBPasser
			&& odistxz(pobj, (obj_3d *)&ball_obj) < 8.0f)
		{
			// ...and between pelvis and a point five feet above it
			if( ((ball_obj.odata.y > (pobj->y+pobj->ay - 4.0f)) && (ball_obj.odata.y < (pobj->y + pobj->ay + 14.0f )))
					|| (pobj->y == 0.0f && ball_obj.odata.y < 12.0f))
			{
// reset some essentials
				game_info.ball_handler = ppdata->plyrnum;
				//game_info.shooter      = NO_SHOOTER;
				game_info.passer       = NO_PASSER;
				game_info.ball_mode    = BM_DRIBBLING;

				ppdata->dribble_mode    = DM_CAN_DRIBBLE;
				ppdata->no_ball_collide = 0;	

				//ball_obj.who_shot = NO_SHOOTER;	// player who shot ball can get it now.
				no_pump_fake = YES;

				//arrowinfo[ppdata->team].timer = 100000;

//FIX!!! depends on game_info.active_goal

				OOB_XZ[0] = ball_obj.odata.x < 0.0f ? -140.0f : 140.0f;
				OOB_XZ[2] = -5.0f + (float)randrng(10);
			}
		}
	}
	return 1;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
int _mode_freethrow(bbplyr_data *ppdata)
{
	obj_3d * pobj = (obj_3d *)ppdata;

	if(!(ppdata->odata.adata[0].animode & MODE_REACTING) &&
		(pobj->y <= 0.0f) &&
		(pobj->adata[0].seq_index != (SEQ_WAIT1)) &&
		(pobj->adata[0].seq_index != (SEQ_WAIT2)) &&
		(pobj->adata[0].seq_index != (SEQ_WAIT3)) &&
		(pobj->adata[0].seq_index != (SEQ_S_CYC_1)) &&
		(pobj->adata[0].seq_index != (SEQ_FT1)) &&
		(pobj->adata[0].seq_index != (SEQ_D_CYCL_1)) &&
		(pobj->adata[0].seq_index != (SEQ_FT1_SHTL)) &&
		(pobj->adata[0].seq_index != (SEQ_FT1_SHTR)) &&
		(pobj->adata[0].seq_index != (SEQ_PUSH_6_L)) &&
		(pobj->adata[0].seq_index != (SEQ_PUSH_6_R)) &&
		(pobj->adata[0].seq_index != (SEQ_PUSH_4)))
	{
		change_anim(pobj,s_breath1_anim);
	}
	
	return 1;
}

//-----------------------------------------------------------------------------------------------------------------------------
// players before start half
//-----------------------------------------------------------------------------------------------------------------------------
int _mode_tipoff(bbplyr_data *ppdata)
{
	register bbplyr_data * ppd = ppdata;
	register game_info_t * pgi = &game_info;
	register ball_data * pbo = &ball_obj;
	register int i, j;
	float p[3], *tm, hx, hy, hz, dist;
	int close = NO, limb = JOINT_RWRIST;

#if 0
	pobj->tgt_fwd = pobj->fwd = (pobj->fwd + 10) & 1023;
#endif
#ifdef DEBUG_EXTREME
	for(i = 0; i < ppdata->plyrnum; i++) fprintf(stderr, " ");
	fprintf(stderr, "ENTER: _mode_tipoff %d\n",ppdata->plyrnum);
#endif

	// has the ball been thrown up ?

	// read button whacks to see who gets control of the ball ?

	// set players into their default locations
//	change_anim( pobj, stance3pt_jumpback_anim );

//#ifdef DEBUG
//	if( !( ppdata->odata.adata[0].animode & MODE_UNINT ))
//		offense_run((obj_3d*)ppdata);
//#endif

	// am I receiving a pass?
	if((game_info.ball_mode == BM_PASSING)
	&& ball_obj.int_receiver == ppdata->plyrnum
	&& ball_obj.flight_time <= (ball_obj.total_time))
	{
	// play a catch animation
		if((catch_anim != NULL) && (ball_obj.total_time - ball_obj.flight_time) <= catch_time)
		{
			change_anim(&ppdata->odata, catch_anim);
			catch_anim = NULL;
		}

//		ppdata->stick_cur = ppdata->stick_old;
//		return 1;
	}

	// If ball is loose and i'm close...get it 
// transform target x,y,z into world coords
	tm = cambot.xform;
	limb = ppdata->ball_in_hand == RIGHT_HAND ? JOINT_RWRIST : JOINT_LWRIST;
	p[0] = hx = ppdata->jpos[limb][0] * tm[0] + ppdata->jpos[limb][1] * tm[4] + ppdata->jpos[limb][2] * tm[ 8] + cambot.x;
	p[1] = hy = ppdata->jpos[limb][0] * tm[1] + ppdata->jpos[limb][1] * tm[5] + ppdata->jpos[limb][2] * tm[ 9] + cambot.y;
	p[2] = hz = ppdata->jpos[limb][0] * tm[2] + ppdata->jpos[limb][1] * tm[6] + ppdata->jpos[limb][2] * tm[10] + cambot.z;
	if(ppd->odata.flags & PF_CATCHING)
	{
	// is ball close enough to hand...
		if(dist3d(&(ball_obj.odata.x), p) <= 5.0f)
			close = YES;

		if(ppd->odata.flags & PF_BOTH_HANDS)
		{
			float hx2,hy2,hz2;

			limb = ppdata->ball_in_hand == RIGHT_HAND ? JOINT_LWRIST : JOINT_RWRIST;
			hx2 = ppdata->jpos[limb][0] * tm[0] + ppdata->jpos[limb][1] * tm[4] + ppdata->jpos[limb][2] * tm[ 8] + cambot.x;
			hy2 = ppdata->jpos[limb][0] * tm[1] + ppdata->jpos[limb][1] * tm[5] + ppdata->jpos[limb][2] * tm[ 9] + cambot.y;
			hz2 = ppdata->jpos[limb][0] * tm[2] + ppdata->jpos[limb][1] * tm[6] + ppdata->jpos[limb][2] * tm[10] + cambot.z;
			p[0] = (hx + hx2) / 2;
			p[1] = (hy + hy2) / 2;
			p[2] = (hz + hz2) / 2;

			if(dist3d(&(ball_obj.odata.x), p) <= 5.0f)
				close = YES;
		}
	}

	if((players_ready == 1) &&
		(pgi->ball_handler == NO_BALLHANDLER) &&
		(pbo->int_receiver == ppd->plyrnum) &&
		(OOBPasser == NO_PASSER))
	{
		if((close == YES) || (ppd->no_ball_collide <= 0 && odistxz(&ppd->odata, &pbo->odata) < 8.0f))
		{
			// ...and between pelvis and a point five feet above it
			if((pbo->odata.y > (ppd->odata.y + ppd->odata.ay - 4.0f) &&	//FIX!!!
				 pbo->odata.y < (ppd->odata.y + ppd->odata.ay + 14.0f ))	//FIX!!!
				||
				(ppd->odata.y == 0.0f && pbo->odata.y < 12.0f)				//FIX!!!
				|| (close == YES)
			)
			{
#ifdef DEBUG_TIPOFF
				printf("TIPOFF: %s %d got the ball\n",
					pgi->plyr_control[ppd->plyrnum] < 0 ? "DRONE":"HUMAN",
					ppd->plyrnum);
#endif

				// Reset some essentials
				pgi->ball_handler = ppdata->plyrnum;
				pgi->game_mode    = GM_IN_PLAY;
				pgi->shooter      = NO_SHOOTER;
				pgi->passer       = NO_PASSER;
				pgi->ball_mode    = BM_DRIBBLING;

				ppd->dribble_mode = DM_CAN_DRIBBLE;	
				pbo->who_shot     = 0;	//NO_SHOOTER;

				no_pump_fake = YES;

				j = ppd->team;
				i = 0;
				ppd = player_blocks;
				do
				{
					ppd->odata.plyrmode = (ppd->team == j) ? MODE_OFFENSE : MODE_DEFENSE;
				}
				while(ppd++, ++i < NUM_PLAYERS);

				pgi->off_side = j;
				if(pgi->game_quarter < 3)	//FIX!!!??? 1 & 3 & etc??
				{
					pgi->active_goal = 1 - j;
					pgi->off_goal    = 1;
				}
				else
				{
					pgi->active_goal = j;
					pgi->off_goal    = 0;
				}
#ifdef DEBUG_TIPOFF
				printf("active_goal = %d, off_side = %d\n",
					pgi->active_goal, pgi->off_side);
#endif
			}
		}
	}

#ifdef DEBUG_EXTREME
	for(i = 0; i < ppdata->plyrnum; i++) fprintf(stderr, " ");
	fprintf(stderr, "LEAVE: _mode_tipoff %d\n",ppdata->plyrnum);
#endif
	return 1;
}

//-----------------------------------------------------------------------------------------------------------------------------
void freethrow_hide_arrows(int * pargs)
{
	// wait till ball is shot
	while ((game_info.ball_handler != NO_BALLHANDLER))
	{
		// Hide every tick
		hide_arrows_one_tick = HIDE;

		sleep(1);
	}

	// wait till ball is handled or hits the ground
	while ((game_info.ball_handler == NO_BALLHANDLER) && (game_info.ball_mode != BM_FREE_ON_COURT))
	{
		// Hide every tick
		hide_arrows_one_tick = HIDE;

		sleep(1);
	}
}

//-----------------------------------------------------------------------------------------------------------------------------
#define ICON_TIME		180
#define WAIT_TIME		180
#define INIT_TIMEOUT	(WAIT_TIME + 1)
#define THROW_TIMEOUT	180
#define THROW_CAMTIME	440

void do_freethrow(void)
{
	static float fs_flagrant[] = {
		0,			0,			0.40f,
		0,			0,			0.80f
	};
	static float fs_freethrow[] = {
		-0.200f,	0,			0.300f,
		-0.200f,	0,			0.700f
	};
//		mode,			mode_cnt,			trav_cnt,			theta,
//		phi,			d,					pseqinfo			trgy_adj
	static CAMSEQINFO csi_freethrow[] = {
	{	CAM_SP_CNFR,	INIT_TIMEOUT,		WAIT_TIME * 9/10,	DG2RD(0),	// flagrant msg
		DG2RD(-89),		60,					fs_flagrant,		0
	},
	{	CAM_ZO_NFR,		1,					0,					0,			// freethrow position snap
		DG2RD(-4),		170,				0,					-TRGY_ZOOM
	},
	{	CAM_SP_CNFR,	10000,				THROW_CAMTIME,		DG2RD(40),	// freethrow camera
		DG2RD(-4),		170,				fs_freethrow,		0
	},
	{	CAM_SP_CNFR,	10000,				0,					0,			// ball follow
		DG2RD(-20),		65,					fs_freethrow,		0
	},
	};

	bbplyr_data * ppd = player_blocks + NUM_PLAYERS;
	ARROWINFO * pai = arrowinfo + NUM_PLAYERS;
	VERTEX rim = { RIM_X, RIM_Y * 0.67f, RIM_Z };
	float fcnt, fcur;
	int	wait_time = WAIT_TIME;
	int	movie_timeout = INIT_TIMEOUT;

	// Set up pan-in on fouled player & start`er up
	cambot.csi = csi_freethrow[0];
	cambot.pmodeobj = &player_blocks[game_info.freethrow_shooter].odata.x;
	cambot.tick_cnt = 0;

	// set game states..etc.
	game_info.game_mode    = GM_FREE_THROW;
	game_info.poss_change  = FALSE;
	OOBPasser = NO_PASSER;
	game_info.passer = NO_PASSER;

	clock_active = 0;
	shotclock_active = 0;

	// put players into MODE_FREETHROW
	do
	{
		ppd--;
		pai--;
		ppd->odata.plyrmode = MODE_FREETHROW;
		pai->timer = 0;
	}
	while (ppd != player_blocks);

	// Start the movie during transit so it can be paused & ready
//	dio_mode(DIO_MODE_NO_BLOCK);
	clear_free_process_list();
	movie_config_player(512.0f, 384.0f, 0.0f, 0.0f, 1.0f, 30);
	qcreate("mplay", 0, movie_proc, (int)select_ptr(foul_movies), 0, 0, TRUE);
	
	if(!(existp(VIOLATION_PID, -1)))
		// show FOUL msg
		qcreate("ftmsg",VIOLATION_PID,free_throw_msg_proc,0,0,0,0);
#ifdef DEBUG
	else
		printf("A violation PID exists!  No free throw message!\n");
		while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
#endif

//	showhide_flag = HIDE;

	// Wait for movie to load and msg to blink
	while ((--wait_time >= 0 || movie_get_state() != MOVIE_STATE_PAUSED) && (--movie_timeout >= 0))
	{
		if ((movie_get_state() == MOVIE_STATE_INIT) ||
			(movie_get_state() == MOVIE_STATE_RUN))
			movie_hide();

		if (movie_get_state() == MOVIE_STATE_RUN)
			movie_pause();

		sleep(1);
	}
//	dio_mode(DIO_MODE_BLOCK);
//	goto_freethrow_line_speech();

	fcur = fcnt = control.num_frames * 2;

	// Do free-throw camera position "snap"
	cambot.csi = csi_freethrow[1];
	cambot.theta_trg = DG2RD(70);
	if (!game_info.active_goal)
	{
		rim.x = -rim.x;
		cambot.theta_trg += DG2RD(180);
	}
	cambot.pmodeobj = &rim;
	cambot.tick_cnt = 0;

	reset_shotclock = 1;

	lineup_players_for_freethrow();

	hide_peacock = 1;

	// Turn on shooter icon
	if (game_info.ball_handler != NO_BALLHANDLER)
	{
		if (p_status & (1<<game_info.ball_handler))
			arrowinfo[game_info.ball_handler].timer = ICON_TIME;
	}

	// Chk if we can start the movie
	if (movie_get_state() != MOVIE_STATE_DONE && movie_timeout)
	{
		movie_resume();
		movie_unhide();
		snd_scall_bank(gameplay_bnk_str, 97, VOLUME5, 127, PRIORITY_LVL2);

		// Wait till movie is done
		movie_timeout = 6 * tsec;
	}
	else
		// Clr to just start the camera & fall thru
		movie_timeout = 0;

	do {
		float f;

		if (!halt && movie_timeout)
		{
			f = (1.4f * fcur) / fcnt;		//###
			if (f > 1.0f)
				f = 1.0f;

			movie_set_transparency(f);

			if (--fcur < 0.0f)
				fcur = 0.0f;
		}
		sleep(1);

		if (cambot.csi.mode_cnt < 2)
		{
			// Do free-throw camera
			cambot.csi = csi_freethrow[2];
			cambot.tick_cnt = 0;
		}
		//showhide_flag = SHOW;
	} while ((movie_get_state() != MOVIE_STATE_DONE) && (--movie_timeout >= 0));

	// Make sure the movie player is shut down
	movie_abort();

	snd_scall_bank(plyr_bnk_str, WHISTLE_SND, VOLUME6, 127, PRIORITY_LVL4);

	game_info.auto_throw_cntr = THROW_TIMEOUT;			// ticks till auto shoot!
	iqcreate("ftmtr",FREETHROW_METER_PID,freethrow_meter_proc,0,0,0,0);

	// wait till player shoots ball or times-out
	do {
		sleep(1);
	} while (existp(FREETHROW_METER_PID, -1));

	// shoot ball
//	change_anim(&player_blocks[game_info.freethrow_shooter].odata, freethrow_shot_1_anim);
	change_anim(&player_blocks[game_info.freethrow_shooter].odata, freethrow_shot_2_anim);

	set_process_run_level( qcreate( "fthidarr", PLAYER_PID, freethrow_hide_arrows, 0, 0, 0, 0), RL_COLLIS+1 );

	// wait till ball scores or hits the ground
	movie_timeout = wait_time = -1;
	while (wait_time != 0 || (game_info.game_mode != GM_IN_PLAY && game_info.ball_handler == NO_BALLHANDLER))
	{
		if (game_info.ball_mode == BM_SHOOTING && wait_time == -1)
		{
			cambot.csi = csi_freethrow[3];
			cambot.csi.trav_cnt = (ball_obj.total_time*5)/4;

			fcur = DG2RD(randrng(180-30)+(30/2));
			if (cambot.theta_trg >= DG2RD(180))
				fcur += DG2RD(180);
			cambot.csi.theta += fcur - cambot.theta_trg;

			cambot.tick_cnt = 0;

			wait_time = ball_obj.total_time;
			fcur = (RIM_Y - rim.y)/(float)wait_time;
		}
		if (wait_time > 0)
		{
			cambot.csi.trgy_adj += fcur;
			wait_time--;
		}

		if((game_info.ball_mode == BM_OFF_INTO_PLAY || game_info.ball_mode == BM_FREE_ON_COURT) && movie_timeout)
		{
#if DEBUG
			printf("^^^^ After freethrow, game state set to GM_IN_PLAY\n");
#endif 
			game_info.game_mode = GM_IN_PLAY;

			// last flag indicates that same team keeps ball...
			if (IsShotGood == YES)
				SetUpOOBounds(1 | 0x4000,0,1);
			else
			{
				clock_active = 1;
				ppd = player_blocks + NUM_PLAYERS;
				do
				{
					ppd--;
					ppd->auto_control = NO;
					ppd->odata.plyrmode = ppd->team==game_info.off_side ? MODE_OFFENSE : MODE_DEFENSE;
				}
				while (ppd != player_blocks);

			}
			movie_timeout = 0;
		}
		sleep(1);
	}

	if (movie_timeout != 0)
	{
#if DEBUG
		printf("^^^^ After freethrow, Bogus exit\n");
#endif 

		if (IsShotGood == YES)
			SetUpOOBounds(1 | 0x4000,0,1);
		else
		{
			clock_active = 1;
			ppd = player_blocks + NUM_PLAYERS;
			do
			{
				ppd--;
				ppd->auto_control = NO;
				ppd->odata.plyrmode = ppd->team==game_info.off_side ? MODE_OFFENSE : MODE_DEFENSE;
			}
			while (ppd != player_blocks);

		}
	}

	hide_peacock = 0;

	// Revert the camera
	cambot.csi.mode_cnt = cambot.tick_cnt;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void lineup_players_for_freethrow(void)
{
	bbplyr_data * ppd = player_blocks + NUM_PLAYERS;
	bbplyr_data	*ppdata;
//	bbplyr_data * ppdata = player_blocks;
	int i, swap;
	char * panim = NULL;

	do
	{
		ppd--;
		//ppd->no_ball_collide = 30;
		ppd->odata.flags &= ~(PF_PLYRFLAGS & ~PF_CELEBRATE);
		ppd->odata.plyrmode = MODE_FREETHROW;
		ppd->attack_mode = 0;
		ppd->attack_time = 0;
		ppd->enemy_player = -1;

		// Reinit player flags
		ppd->auto_control   = YES;
	}
	while (ppd != player_blocks);

	// put shooter in place
	game_info.ball_handler = game_info.freethrow_shooter;
	game_info.ball_mode    = BM_DRIBBLING;

	player_blocks[game_info.freethrow_shooter].odata.x = (game_info.active_goal) ? 71.0f : -71.0f;
	player_blocks[game_info.freethrow_shooter].odata.y = 0.0f;
	player_blocks[game_info.freethrow_shooter].odata.z = 0.0f;
	player_blocks[game_info.freethrow_shooter].odata.fwd = (game_info.active_goal) ? 256 : 768;
	change_anim(&player_blocks[game_info.freethrow_shooter].odata, dribble_stand_cocky);
//	change_anim(&player_blocks[game_info.freethrow_shooter].odata, freethrow_dribble_2_anim);

	ppdata = player_blocks;

	swap = (randrng(100) < 20 ? 1 : 0);

	// put rest of players in place
	for(i = 0; i < NUM_PLAYERS; ppdata++, i++)
	{
		if(i == game_info.freethrow_shooter)
			continue;

		if(ppdata->team == player_blocks[game_info.freethrow_shooter].team)
		{	// position teammate
			if (!swap)
				ppdata->odata.x = (game_info.active_goal) ? 95.0f : -95.0f;
			else
				ppdata->odata.x = (game_info.active_goal) ? 120.0f : -120.0f;

			ppdata->odata.y = 0.0f;
			ppdata->odata.z = 21.0f;
			ppdata->odata.fwd = (ppdata->odata.z < 0) ? 0 : 512;
			change_anim(&ppdata->odata, wait_freethrow_anim_tbl[ppdata->plyrnum]);
		}
		else
		{
			if(player_blocks[i].offender == game_info.freethrow_shooter)
			{
				ppdata->odata.x = (game_info.active_goal) ? 108.0f : -108.0f;
				ppdata->odata.z = -21.0f;
			}
			else
			{
				if (!swap)
					ppdata->odata.x = (game_info.active_goal) ? 120.0f : -120.0f;
				else
					ppdata->odata.x = (game_info.active_goal) ? 95.0f : -95.0f;
				ppdata->odata.z = 21.0f;
			}
			ppdata->odata.y = 0.0f;
			ppdata->odata.fwd = (ppdata->odata.z < 0) ? 0 : 512;
			change_anim(&ppdata->odata, wait_freethrow_anim_tbl[ppdata->plyrnum]);
		}
	}
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
#define TIPPER_T1	(NUM_PERTEAM - 1 - !four_plr_ver)
#define TIPPER_T2	(NUM_PLAYERS - 1 - (NUM_PERTEAM == 2))
#define	TIPOFF_TIME	tsec*6

void do_tipoff(void)
{
	float fcnt, fcur;
	int flag;

	// Test stuff

//	p_status = 0;		// Watch drones play forever (noclock also) with snds loaded in
//	qcreate("shotclock", VIOLATION_PID, shotclock_violation_proc, 0, 0, 0, 0);
//	qcreate("shotclock", VIOLATION_PID, goaltending_proc, 0, 0, 0, 0);
//	qcreate("shotclock", VIOLATION_PID, free_throw_msg_proc, 0, 0, 0, 0);
//	qcreate("tmfire", 0, show_tmfire_proc, 0, 0, 0, 0);

//	qcreate("idiots",0,tipoff_idiot_box_proc,0,0,0,0);

	// set game states..etc.
	game_info.game_mode    = GM_PRE_TIPOFF;
	game_info.ball_mode    = BM_TIPOFF;
	game_info.ball_handler = NO_BALLHANDLER;
	game_info.poss_change  = FALSE;
	OOBPasser = NO_PASSER;

	// Doing GM_PRE_TIPOFF
	// lineup players in their default positions and clear/set some ram
	lineup_players_for_tipoff();

	// clear the ball's rotation values
	ball_rotclr();

	// Make sure the movie player is shut down
	movie_abort();
	sleep(1);

	if(DBG_KO_BOOL) goto abort_do_tipoff;

	// Start the movie during transit so it can be paused & ready
	clear_free_process_list();
	movie_config_player(512.0f, 384.0f, 0.0f, 0.0f, 1.0f, 30);
	qcreate("mplay", 0, movie_proc, (int)select_ptr(qtr_movies), 0, 0, FALSE);

	pre_tipoff_movie_delay = 1;

	// Wait for any GM_PRE_TIPOFF to finish
	flag = 1;
	while (game_info.game_mode == GM_PRE_TIPOFF)
	{
		if ((movie_get_state() == MOVIE_STATE_INIT) || (movie_get_state() == MOVIE_STATE_RUN))
			movie_hide();

		if ((movie_get_state() == MOVIE_STATE_DONE) || (movie_get_state() == MOVIE_STATE_RUN))
		{
			if (movie_get_state() == MOVIE_STATE_RUN)
			{
				movie_pause();
				pre_tipoff_movie_delay = control.num_frames * 2;
			}
			if (flag)
			{
				suspend_multiple_processes( CAMBOT_PID,     -1 );
				suspend_multiple_processes( PLAYER_PID,     -1 );
				suspend_multiple_processes( REFEREE_PID,    -1 );
				suspend_multiple_processes( BALL_PID,       -1 );
				suspend_multiple_processes( JUMP_METER_PID, -1 );

				hide_multiple_objects( OID_PLAYER, -1 );

				opening_msg_proc((int *)NULL);

				resume_multiple_processes( CAMBOT_PID,     -1 );
				resume_multiple_processes( PLAYER_PID,     -1 );
				resume_multiple_processes( REFEREE_PID,    -1 );
				resume_multiple_processes( BALL_PID,       -1 );
				resume_multiple_processes( JUMP_METER_PID, -1 );

				unhide_multiple_objects( OID_PLAYER, -1 );

				if (COURT_IS_INDOORS)
				{
					// Crowd stomps
					qcreate( "stomp", 0, crowd_stomp_proc, 0, 0, 0, 0 );
					crowd_cheer_time = 180 * 3;
				}
				else if (which_court == COURT_GEER)
				{
					// Drums
//					snd_scall_bank(crowd_bnk_str,38,VOLUME7,127,PRIORITY_LVL5);
					qcreate( "snds", 0, monkey_snds2_proc, 0, 0, 0, 0 );
				}

				flag = 0;
			}
		}
		if(DBG_KO_BOOL) goto abort_do_tipoff;
		sleep(1);
	}

	fcur = fcnt = pre_tipoff_movie_delay;

	if (movie_get_state() != MOVIE_STATE_DONE) {
		// Restart the movie
		movie_resume();
		movie_unhide();
	}

	// Play cool movie whoosh snd
	snd_scall_bank(gameplay_bnk_str, 97, VOLUME5, 127, PRIORITY_LVL4);
	qcreate("whistle", 0, whistle_proc, 0, 0, 0, 0);

	// lineup players in their default positions and clear/set some ram
	lineup_players_for_tipoff();

	// Wait till movie is done
	while (movie_get_state() != MOVIE_STATE_DONE && fcur)
	{
		float f;

		if(DBG_KO_BOOL) goto abort_do_tipoff;

		if (!halt)
		{
			f = (1.4f * fcur) / fcnt;		//###
			if (f > 1.0f)
				f = 1.0f;

			movie_set_transparency(f);

			if (--fcur < 0.0f)
				fcur = 0.0f;
		}
		sleep(1);
	}

	// Make sure the movie player is shut down
	movie_abort();

	// Doing GM_TIPOFF

	// Wait for ball to be thrown up
	do
	{
		if (DBG_KO_BOOL) goto abort_do_tipoff;
		sleep(1);
	}
	while (game_info.ball_mode == BM_TIPOFF);

	// Set anims per who gets the ball
	change_anim((obj_3d *)(player_blocks + TIPPER_T1),
		(jmeter_ii[0].h > jmeter_ii[1].h) ?
		tipoff_jump_up_r_anim : tipoff_miss_r_anim);

	change_anim((obj_3d *)(player_blocks + TIPPER_T2),
		(jmeter_ii[0].h > jmeter_ii[1].h) ?
		tipoff_miss_l_anim : tipoff_jump_up_r_anim);

	// Wait for ball to be caught
	while (game_info.game_mode != GM_IN_PLAY)
	{
		if (DBG_KO_BOOL) goto abort_do_tipoff;

		sleep(1);
	}

	player_blocks[game_info.ball_handler].odata.plyrmode = MODE_OFFENSE;
	player_blocks[game_info.ball_handler^1].odata.plyrmode = MODE_OFFENSE;
	player_blocks[(!player_blocks[game_info.ball_handler].team)*2].odata.plyrmode = MODE_DEFENSE;
	player_blocks[((!player_blocks[game_info.ball_handler].team)* 2)+1].odata.plyrmode = MODE_DEFENSE;

	game_underway_speech();

abort_do_tipoff:
	// tipoff complete
	movie_abort();
}

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
void whistle_proc(int * parg)
{
	// Wait for proper whistle time
	int cnt	= 1*tsec;
	while(cnt--)
	{
		if (DBG_KO_BOOL) die(0);
		sleep(1);
	}
	snd_scall_bank(plyr_bnk_str, WHISTLE_SND, VOLUME5, 127, PRIORITY_LVL4);
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void pick_random_tipoff_anim(obj_3d *pobj)
{
	change_anim(pobj, tipoff_ready_anims_tbl[randrng(sizeof(tipoff_ready_anims_tbl)/sizeof(char *))]);
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void lineup_players_new_quarter(void)
{
	int	i;
	bbplyr_data * ppdata = player_blocks;
	obj_3d		*pobj;

	game_info.ball_mode = BM_DRIBBLING;
	game_info.ball_handler = NO_BALLHANDLER;
	ball_obj.int_receiver = NO_RECEIVER;

	ball_obj.odata.vx = 0.0f;
	ball_obj.odata.vy = 0.0f;
	ball_obj.odata.vz = 0.0f;
	ball_obj.odata.y = 0.0f;

	// set/clear control stuff for all players
	for( i = 0; i < NUM_PLAYERS; ppdata++, i++ )
	{
		//game_info.plyr_control[i] = -1;
		pobj = (obj_3d *)ppdata;

		ppdata->no_ball_collide = 0;
		ppdata->odata.flags   &= ~(PF_PLYRFLAGS);
		ppdata->attack_time    = 0;
		ppdata->attack_mode    = 0;
		ppdata->flight_time    = 0;
		ppdata->total_time     = 0;
		ppdata->celebrate_time = 0;
		ppdata->puppet_link    = NULL;

		ppdata->odata.adata[0].animode &= ~(MODE_INTERP|MODE_SECONDARY|MODE_PUPPET|MODE_PUPPETEER|MODE_ANCHOR);
		ppdata->odata.adata[1].animode &= ~(MODE_INTERP|MODE_SECONDARY|MODE_PUPPET|MODE_PUPPETEER|MODE_ANCHOR);
		ppdata->odata.adata[2].animode &= ~(MODE_INTERP|MODE_SECONDARY|MODE_PUPPET|MODE_PUPPETEER|MODE_ANCHOR);
		ppdata->odata.adata[3].animode &= ~(MODE_INTERP|MODE_SECONDARY|MODE_PUPPET|MODE_PUPPETEER|MODE_ANCHOR);
		ppdata->odata.adata[4].animode &= ~(MODE_INTERP|MODE_SECONDARY|MODE_PUPPET|MODE_PUPPETEER|MODE_ANCHOR);
		ppdata->odata.adata[5].animode &= ~(MODE_INTERP|MODE_SECONDARY|MODE_PUPPET|MODE_PUPPETEER|MODE_ANCHOR);

		ppdata->stick_cur = 24;
		ppdata->but_cur   = 0;

		ppdata->dribble_mode   = DM_CAN_DRIBBLE;
		ppdata->attack_mode    = ATTMODE_NONE;

		change_anim(pobj, s_breath3_anim);
		
		ppdata->odata.vx = 0.0f;
		ppdata->odata.vy = 0.0f;
		ppdata->odata.vz = 0.0f;
		if (game_info.ball_handler == NO_BALLHANDLER)
			ppdata->odata.y = 0.0f;
	}

	SetUpOOBounds(0x10000 + 0x8000 + 0,0,0);		// = new qtr + immediate pos + back court inbounds

	game_info.ball_handler = OOBPasser;
	game_info.ball_mode = BM_DRIBBLING;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void lineup_players_for_tipoff(void)
{
	static float lineup_fwdxz[][2][3] = {		// [plyr#][pre,tip][fwd,x,z]
#if NUM_PERTEAM > 2
		{ {256, -50,  30}, {256, -40,  30} },
#endif
		{ {256, -50,  18}, {192, -40, -30} },
		{ {256, -50,   6}, {256, -20,   0} },
		{ {256, -50,  -6}, {768,  13,   0} },
		{ {256, -50, -18}, {832,  40, -30} },
#if NUM_PERTEAM > 2
		{ {256, -50, -30}, {768,  40,  30} }
#endif
	};
	bbplyr_data * ppdata = player_blocks;
	int gm = game_info.game_mode == GM_TIPOFF;
	int i;
	char * panim = NULL;


	// set/clear control stuff for all players
	for( i = 0; i < NUM_PLAYERS; ppdata++, i++ )
	{
		int j = (!four_plr_ver&&i<NUM_PERTEAM)?(NUM_PERTEAM-1-i):(i);

		//game_info.plyr_control[i] = -1;

		ppdata->no_ball_collide = 0;
		ppdata->odata.plyrmode = MODE_TIPOFF;
		ppdata->odata.flags   &= ~(PF_NOCOLL | PF_CELEBRATE);
		ppdata->attack_time    = 0;
		ppdata->attack_mode    = 0;
		ppdata->celebrate_time = 0;
		ppdata->puppet_link    = NULL;

		ppdata->odata.adata[0].animode &= ~(MODE_INTERP|MODE_SECONDARY|MODE_PUPPET|MODE_PUPPETEER|MODE_ANCHOR);
		ppdata->odata.adata[1].animode &= ~(MODE_INTERP|MODE_SECONDARY|MODE_PUPPET|MODE_PUPPETEER|MODE_ANCHOR);
		ppdata->odata.adata[2].animode &= ~(MODE_INTERP|MODE_SECONDARY|MODE_PUPPET|MODE_PUPPETEER|MODE_ANCHOR);
		ppdata->odata.adata[3].animode &= ~(MODE_INTERP|MODE_SECONDARY|MODE_PUPPET|MODE_PUPPETEER|MODE_ANCHOR);
		ppdata->odata.adata[4].animode &= ~(MODE_INTERP|MODE_SECONDARY|MODE_PUPPET|MODE_PUPPETEER|MODE_ANCHOR);
		ppdata->odata.adata[5].animode &= ~(MODE_INTERP|MODE_SECONDARY|MODE_PUPPET|MODE_PUPPETEER|MODE_ANCHOR);

		ppdata->odata.fwd = lineup_fwdxz[j][gm][0];
		ppdata->odata.x   = lineup_fwdxz[j][gm][1];
		ppdata->odata.z   = lineup_fwdxz[j][gm][2];

		ppdata->odata.tgt_fwd = ppdata->odata.fwd;

		ppdata->stick_cur = 24;
		ppdata->but_cur   = 0;

		if (!gm)
			// Doing GM_PRE_TIPOFF
			panim = pre_tip_anims_tbl[j];
		else
		if (i == TIPPER_T1)
		{
			// Doing GM_TIPOFF
			panim = tipoff_jump_ready_l_anim;
		}
		else
		if (i == TIPPER_T2)
		{
			// Doing GM_TIPOFF
			panim = tipoff_jump_ready_r_anim;
		}
		else
		{	// Doing GM_TIPOFF
			if (i < NUM_PERTEAM)
			{
				panim = (randrng(100) < 50) ? tipoff_ready_1_anim : tipoff_ready_2_anim;
			}
			else
			{
				if (randrng(100) > 80)
					panim = tipoff_ready_3_anim;
				else if (randrng(100) > 60)
					panim = tipoff_ready_4_anim;
				else
					panim = tipoff_ready_5_anim;
			}
		}
		change_anim( &ppdata->odata, panim );

		ppdata->odata.y = 0.0f;
	}
}

#ifdef DEBUG
//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void lineup_players_for_debug(void)
{
	int	i;
	bbplyr_data	*ppdata;
	obj_3d			*pobj;

	// set/clear control stuff for all players
	for( i = 0; i < NUM_PLAYERS; i++ )
	{
		ppdata = player_blocks + i;
		pobj = (obj_3d *)ppdata;

		game_info.plyr_control[i] = -1;

		pobj->plyrmode = MODE_TIPOFF;

		pobj->flags &= ~(PF_NOCOLL | PF_CELEBRATE);
		ppdata->attack_time = 0;
		ppdata->attack_mode = 0;
		ppdata->celebrate_time = 0;
		ppdata->puppet_link = NULL;

		pobj->adata[0].animode &= ~(MODE_PUPPET|MODE_PUPPETEER);
		pobj->adata[1].animode &= ~(MODE_PUPPET|MODE_PUPPETEER);

//		pobj->fwd = ppdata->team ? 768 : 256;
		pobj->fwd = ppdata->team ? 768 : 768;
		pobj->tgt_fwd = pobj->fwd;

		ppdata->stick_cur = 24;
		ppdata->but_cur = 0;
	}

	// setup player 1
	pobj = (obj_3d *)(player_blocks + 0);
	pobj->x = -55.0f;
	pobj->z = 4.0f;
	change_anim( pobj, s_breath_dbg_anim_int );

	// setup player 2
	pobj = (obj_3d *)(player_blocks + 1);
	pobj->x = -55.0f;
	pobj->z = 14.0f;
	change_anim( pobj, s_breath_dbg_anim_int );

	// setup team #1 drone
	pobj = (obj_3d *)(player_blocks + 2);
	pobj->x = -55.0f;
	pobj->z = 24.0f;
	change_anim( pobj, s_breath_dbg_anim_int );


	// setup player 3
	pobj = (obj_3d *)(player_blocks + 3);
	pobj->x = -55.0f;
	pobj->z = 34.0f;
	change_anim( pobj, s_breath_dbg_anim_int );

#if NUM_PERTEAM > 2
	// setup player 4
	pobj = (obj_3d *)(player_blocks + 4);
	pobj->x = -55.0f;
	pobj->z = 44.0f;
	change_anim( pobj, s_breath_dbg_anim_int );

	// setup team #2 drone
	pobj = (obj_3d *)(player_blocks + 5);
	pobj->x = -145.0f;
	pobj->z = 53.0f;
	change_anim( pobj, s_breath_dbg_anim_int );
#endif
}
#endif

//////////////////////////////////////////////////////////////////////////////
#define INBETWEEN_RANGE 48
//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
int in_between_ball_basket(bbplyr_data *ppdata)
{
	obj_3d *pobj;
	float rim_pt[3], ball_rim_dist;
	int inbetween_angle, ball_angle_to_rim;

	pobj = game_info.ball_handler == NO_BALLHANDLER ? (obj_3d *)(&ball_obj) : (obj_3d *)(player_blocks + game_info.ball_handler);
	rim_pt[0] = game_info.active_goal ? RIM_CX : -RIM_CX; rim_pt[1] = rim_pt[2] = 0.0f;

	ball_angle_to_rim = ptangle(&pobj->x, rim_pt);
	ball_rim_dist = fsqrt( ((rim_pt[0] - pobj->x) * (rim_pt[0] - pobj->x)) + ((rim_pt[2] - pobj->z) * (rim_pt[2] - pobj->z)) );

	inbetween_angle = ABS(ball_angle_to_rim - get_angle_to_rim(ppdata));

	return (inbetween_angle <= INBETWEEN_RANGE && get_dist_to_rim(ppdata) <= ball_rim_dist);
}

//-----------------------------------------------------------------------------------------------------------------------------
// Checks to see if player is inbetween the ball and hoop and scales the allowable range based on the blockers' power.
//-----------------------------------------------------------------------------------------------------------------------------
int in_between_ball_basket_pow(bbplyr_data *ppdata)
{
#if 1
	obj_3d *pobj;
	int bearing, range = 32 + (int)(6.0f * (ppdata->_power / 20.0f));

	pobj = game_info.ball_handler == NO_BALLHANDLER ? (obj_3d *)(&ball_obj) : (obj_3d *)(player_blocks + game_info.ball_handler);

	// FIX!!!  defender could be behind hoop, but in same angle, and it will stop dunkers!  Do subtract of both angles...
	bearing = (get_angle_to_rim((bbplyr_data *)pobj) - ptangle( &pobj->x, &ppdata->odata.x)) & 1023;
	bearing = (bearing > 512) ? 1024 - bearing : bearing;

	return (bearing <= range);
#else

	return (inbetween_angle <= (32 + (int)(6.0f * (ppdata->_power / 20.0f))) && get_dist_to_rim(ppdata) <= ball_rim_dist);
//	return (inbetween_angle <= INBETWEEN_RANGE && get_dist_to_rim(ppdata) <= ball_rim_dist);
#endif
}

//-----------------------------------------------------------------------------------------------------------------------------
// Checks to see if player is inbetween me and hoop and scales the allowable range based on the blockers' power.
//-----------------------------------------------------------------------------------------------------------------------------
int in_between_me_basket_pow(int me, int him)
{
#if 1
	obj_3d *pobj = (obj_3d *)(player_blocks + me);
	float rim_pt[3], ball_rim_dist;
	int inbetween_angle, ball_angle_to_rim;

	rim_pt[0] = game_info.active_goal ? RIM_CX : -RIM_CX; rim_pt[1] = rim_pt[2] = 0.0f;

	ball_angle_to_rim = ptangle(&pobj->x, rim_pt);
	ball_rim_dist = fsqrt( ((rim_pt[0] - pobj->x) * (rim_pt[0] - pobj->x)) + ((rim_pt[2] - pobj->z) * (rim_pt[2] - pobj->z)) );

	inbetween_angle = ABS(ball_angle_to_rim - get_angle_to_rim(&player_blocks[him]));

	return (inbetween_angle <= (48 + (int)(32.0f * (player_blocks[him]._power / 20.0f))) && get_dist_to_rim(&player_blocks[him]) <= ball_rim_dist);
#else
	bbplyr_data *ppd_me = &player_blocks[me];
	bbplyr_data *ppd_him = &player_blocks[him];
	obj_3d *pobj;
	int bearing, range = 32 + (int)(6.0f * (ppd_him->_power / 20.0f));

	pobj = game_info.ball_handler == NO_BALLHANDLER ? (obj_3d *)(&ball_obj) : (obj_3d *)(player_blocks + me);

	// FIX!!!  defender could be behind hoop, but in same angle, and it will stop dunkers!  Do subtract of both angles...
	bearing = (get_angle_to_rim((bbplyr_data *)pobj) - ptangle( &pobj->x, &ppd_him->odata.x)) & 1023;
	bearing = (bearing > 512) ? 1024 - bearing : bearing;

	return (bearing <= range);

	//return (inbetween_angle <= (32 + (int)(6.0f * (ppd_him->_power / 20.0f))) && get_dist_to_rim(ppd_him) <= ball_rim_dist);
//	return (inbetween_angle <= INBETWEEN_RANGE && get_dist_to_rim(ppdata) <= ball_rim_dist);
#endif
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
int nearest_to_rim(int team)
{
	obj_3d	*pobj;
	float	best,dist, rim_pt[3] = {0.0f, 0.0f, 0.0f};
	int		new,i;

	rim_pt[0] = (team ^ game_info.off_goal) == 1 ? RIM_X : -RIM_X;

	best = 100000.0f;
	new = NUM_PERTEAM * team;
	i = new;
	do
	{
		pobj = (obj_3d *)(player_blocks + i);

		dist = (rim_pt[0]-pobj->x)*(rim_pt[0]-pobj->x)+(rim_pt[2]-pobj->z)*(rim_pt[2]-pobj->z);

		if( dist < best )
		{
			best = dist;
			new = player_blocks[i].plyrnum;
		}
	}
	while(++i % NUM_PERTEAM);

	return new;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
int nearest_to_me(int me, int team)
{
	obj_3d	*pobj;
	float	tx, tz, best,dist;
	int		new,i;

	tx = player_blocks[me].odata.x;
	tz = player_blocks[me].odata.z;

	// default to new guy
	new = player_blocks[me].plyrnum;
	best = 100000.0f;

	// find new closest
	i = NUM_PERTEAM * team;
	do
	{
		// skip my self
		if(i == player_blocks[me].plyrnum)
			continue;

		pobj = (obj_3d *)(player_blocks + i);

		dist = (tx-pobj->x)*(tx-pobj->x)+(tz-pobj->z)*(tz-pobj->z);

		if( dist < best )
		{
			best = dist;
			new = player_blocks[i].plyrnum;
		}
	}
	while(++i % NUM_PERTEAM);

	return new;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
int nearest_drone_to_ball(int team)
{
	register float dist = 0.0f, d;
	register int plyr = -1;
	register int i = 0;

	// Init plyr # per team 0 or 1
	if (team) team = NUM_PERTEAM;

	// Find nearest drone
	while (i < NUM_PERTEAM)
	{
		if (game_info.plyr_control[team] == -1)
		{
			d = odistxz(&player_blocks[team].odata, &ball_obj.odata);
			if ((plyr < 0) || d < dist)
			{
				dist = d;
				plyr = team;
			}
		}
		team++;
		i++;
	}
#ifdef DEBUG
	// Chk if no drone was found
	if (plyr < 0)
		fprintf(stderr,"SORT FAIL: nearest_drone_to_ball()\r\n");
#endif

	return plyr;
}

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
int nearest_to_ball(int team)
{
	register float dist = 0.0f, d;
	register int plyr = -1;
	register int i = 0;

	// Init plyr # per team 0 or 1
	if (team) team = NUM_PERTEAM;

	// Find nearest
	while (i < NUM_PERTEAM)
	{
		d = odistxz(&player_blocks[team].odata, &ball_obj.odata);
		if ((plyr < 0) || d < dist)
		{
			dist = d;
			plyr = team;
		}
		team++;
		i++;
	}

	return plyr;
}

///////////////////////////////////////////////////////////////////////////////
// returns the bearing on plyr2 from plyr1
// bearing is the angle between plyr1's facing direction and the vector
// connecting plyr1 to plyr2.
// Returns the ABS() of the facing-angles diff
int plyr_bearing( int plyr1, int plyr2 )
{
	register int b = (player_blocks[plyr1].odata.fwd - p2p_angles[plyr1][plyr2]) & 1023;

	if (b > 512) b = 1024 - b;

	return b;
}

///////////////////////////////////////////////////////////////////////////////
void ball_fire_trail_proc(int *args)
{
	obj3d_data_t		smkpuff_obj;
	struct object_node	*p_smkobj;
	int					i,j;

	smkpuff_obj.ratio = smkpuf_decals[0]->texture_info.header.aspect_ratio;
	smkpuff_obj.decal = smkpuf_decals[0]->texture_handle;
	smkpuff_obj.limb_obj = &limb_spfx;

	smkpuff_obj.odata.x = ball_obj.odata.x - randrng(args[0]);
	smkpuff_obj.odata.y = ball_obj.odata.y + args[1];
	smkpuff_obj.odata.z = ball_obj.odata.z + args[2];

#ifndef DEBUG_NO_FIRE
	p_smkobj = create_object( "puff", OID_SMKPUFF, OF_NONE, DRAWORDER_BALL+1, (void *)&smkpuff_obj, draw_spfx);
	//p_smkobj = create_object( "puff", OID_SMKPUFF, OF_NONE, DRAWORDER_FIELD-1, (void *)&smkpuff_obj, draw_spfx);

	smkpuff_obj.odata.vy = .45f;

	// flame into smoke frames
	j = NUM_FIRE_TXTRS - randrng(5);
	for (i=randrng(5); i < j; i++)
	{
		// change texture on 3d obj.
		smkpuff_obj.ratio = smkpuf_decals[i]->texture_info.header.aspect_ratio;
		smkpuff_obj.decal = smkpuf_decals[i]->texture_handle;

		smkpuff_obj.odata.y += smkpuff_obj.odata.vy;

		sleep(1);
	}
	delete_object(p_smkobj);
#endif
}

//////////////////////////////////////////////////////////////////////////////
void draw_spfx(void *oi)
{
	float		cm[12] = { 0.0f, 1.0f, 0.0f, 0.0f,
						   1.0f, 0.0f, 0.0f, 0.0f,
						   0.0f, 0.0f, -1.0f, 0.0f };
	float		tx,ty,tz;

	obj_3d	*pobj = (obj_3d *)oi;
	obj3d_data_t *ppuff = (obj3d_data_t *)oi;

	guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);
	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
	grTexClampMode(0,GR_TEXTURECLAMP_WRAP,GR_TEXTURECLAMP_WRAP);

	grDepthBufferMode( GR_DEPTHBUFFER_WBUFFER );
	grDepthBufferFunction( GR_CMP_ALWAYS );
	grAlphaTestFunction( GR_CMP_ALWAYS );
	grDepthMask( FXTRUE );

	grChromakeyMode( GR_CHROMAKEY_DISABLE );

	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
	grAlphaBlendFunction(
			GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE,	//_MINUS_SRC_ALPHA,
			GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA );

	tx = pobj->x - cambot.x;
	ty = pobj->y - cambot.y;
	tz = pobj->z - cambot.z;

	cm[3] = tx * cambot.xform[0] + ty * cambot.xform[1] + tz * cambot.xform[2];
	cm[7] = tx * cambot.xform[4] + ty * cambot.xform[5] + tz * cambot.xform[6];
	cm[11] = tx * cambot.xform[8] + ty * cambot.xform[9] + tz * cambot.xform[10];

	render_limb( ppuff->limb_obj, cm, &(ppuff->decal), &(ppuff->ratio) );
}

// code that used to be physics calcs for ball's path to rim
///*
//	ds = fsqrt( ((tgt_x - xob) * (tgt_x - xob)) + ((tgt_z - zob) * (tgt_z - zob)) );
//	dy = fabs(tgt_y - yob);
//
//	_t1 = ((0.5f GRAVITY) * (ds * ds));
//	_t2 = ( (icos(fly_Angle) * icos(fly_Angle) ) * ( (ds * ( isin(fly_Angle) / icos(fly_Angle) ) ) - dy) );
//	flyVel = fsqrt(fabs(_t1/_t2));
//	k1 = tgt_z - zob;
//	k1 = k1 == 0.0f ? 0.1f : k1;
//	gamma = iasin( (tgt_x - xob) / ds );
//	vY = flyVel * isin(fly_Angle);
//	vS = flyVel * icos(fly_Angle);
//	
//	vx = vS * isin( iasin( (tgt_x - xob) / ds ) );
//	vy = vY;
//	vz = vS * icos( iasin( (tgt_x - xob) / ds ) );
//
//	printf("\nxDis = %f, xStrt %f, xEnd %f\n", tgt_x - xob, xob, tgt_x);
//	printf("zDis = %f, zStrt %f, zEnd %f\n", tgt_z - zob, zob, tgt_z);
//	printf("Gamma = %f, ds = %f, dy = %f\n_t1 = %f, _t2 = %f, vY = %f, vS = %f\n",
//				gamma, ds, dy, _t1, _t2, vY, vS);
//	printf("xVel = %f, yVel = %f, zVel = %f\n\n",vx, vy, vz);
//
//#if 0
//	if(both == YES)
//	{
//	float bx,by,bz,hx,hy,hz,rhx,rhy,rhz,thx,thy,thz,lhx,lhy,lhz;
//
//	// ball's position relative to joint's zero position
//		bx = R_INHAND_X_OFF * (pobj->xscale * PSCALE);
//		by = R_INHAND_Y_OFF * (pobj->yscale * PSCALE);
//		bz = R_INHAND_Z_OFF * (pobj->zscale * PSCALE);
//		throw_joint = JOINT_RWRIST;
//	// from screen space to world space
//		tx = bx * ppdata->matrices[throw_joint][0] + by * ppdata->matrices[throw_joint][1] + bz * ppdata->matrices[throw_joint][2] + ppdata->matrices[throw_joint][3];
//		ty = bx * ppdata->matrices[throw_joint][4] + by * ppdata->matrices[throw_joint][5] + bz * ppdata->matrices[throw_joint][6] + ppdata->matrices[throw_joint][7];
//		tz = bx * ppdata->matrices[throw_joint][8] + by * ppdata->matrices[throw_joint][9] + bz * ppdata->matrices[throw_joint][10] + ppdata->matrices[throw_joint][11];
//		rhx = tx * tm[0] + ty * tm[4] + tz * tm[8] + cambot.x;
//		rhy = tx * tm[1] + ty * tm[5] + tz * tm[9] + cambot.y;
//		rhz = tx * tm[2] + ty * tm[6] + tz * tm[10] + cambot.z;
//
//		bx = L_INHAND_X_OFF * PSCALE;
//		by = L_INHAND_Y_OFF * PSCALE;
//		bz = L_INHAND_Z_OFF * PSCALE;
//		throw_joint = JOINT_LWRIST;
//		tx = bx * ppdata->matrices[throw_joint][0] + by * ppdata->matrices[throw_joint][1] + bz * ppdata->matrices[throw_joint][2] + ppdata->matrices[throw_joint][3];
//		ty = bx * ppdata->matrices[throw_joint][4] + by * ppdata->matrices[throw_joint][5] + bz * ppdata->matrices[throw_joint][6] + ppdata->matrices[throw_joint][7];
//		tz = bx * ppdata->matrices[throw_joint][8] + by * ppdata->matrices[throw_joint][9] + bz * ppdata->matrices[throw_joint][10] + ppdata->matrices[throw_joint][11];
//		lhx = tx * tm[0] + ty * tm[4] + tz * tm[8] + cambot.x;
//		lhy = tx * tm[1] + ty * tm[5] + tz * tm[9] + cambot.y;
//		lhz = tx * tm[2] + ty * tm[6] + tz * tm[10] + cambot.z;
//		hx = ppdata->ball_in_hand == RIGHT_HAND ? rhx : lhx;
//		hy = ppdata->ball_in_hand == RIGHT_HAND ? rhy : lhy;
//		hz = ppdata->ball_in_hand == RIGHT_HAND ? rhz : lhz;
//
//		thx = ppdata->ball_in_hand == RIGHT_HAND ? lhx : rhx;
//		thy = ppdata->ball_in_hand == RIGHT_HAND ? lhy : rhy;
//		thz = ppdata->ball_in_hand == RIGHT_HAND ? lhz : rhz;
//		ball_obj.odata.x = (hx + thx) * 0.5f;
//		ball_obj.odata.y = (hy + thy) * 0.5f;
//		ball_obj.odata.z = (hz + thz) * 0.5f;
//	}
//	else
//	{
//		throw_joint = ppdata->ball_in_hand == RIGHT_HAND ? JOINT_RWRIST : JOINT_LWRIST;
//
//		rx = ppdata->jpos[throw_joint][0
//
//*/
//
