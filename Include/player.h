/*
 *		$Archive: /video/Nba/Include/player.h $
 *		$Revision: 121 $
 *		$Date: 4/19/99 12:54a $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifndef __PLAYER_H__
#define __PLAYER_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_player_h = "$Workfile: player.h $ $Revision: 121 $";
#endif

#include "ani3D.h"

#define BLK				(1<<0)
#define WHT				(1<<1)
#define RIGHT			(1<<2)
#define LEFT			(1<<3)
#define BIG				(1 << 4)			/* player flag for big body model */
#define NORMAL			(1 << 5)			/* player flag for normal body model */
#define FEMALE			(1 << 6)			/* use the female body model */


#define RIGHT_HAND			0
#define LEFT_HAND			1
#define RIGHT_FIST			2
#define LEFT_FIST			3
#define RIGHT_HAND_OPEN_H	4	
#define LEFT_HAND_OPEN_H	5
#define RIGHT_HAND_POINT	6
#define LEFT_HAND_POINT		7

#define BITS_HAND			3

#define MASK_RIGHT_HAND		((1<<BITS_HAND)-1)
#define MASK_LEFT_HAND		(MASK_RIGHT_HAND<<BITS_HAND)


typedef struct {
	LIMB *head_model;
	float head_scale;
	LIMB **body_model;
	LIMB **jnum_model;
	LIMB **reflect_model;
} load_info;

extern load_info player_load_info[];

enum
{
	CHA_MASCOT,					/* MASCOTS */
	CHI_MASCOT,
	IND_MASCOT,
	MIN_MASCOT,
	TOR_MASCOT,
	HOU_MASCOT,
	ATL_MASCOT,
	PHO_MASCOT,
	NJN_MASCOT,
	DEN_MASCOT,
	SEA_MASCOT,
	UTA_MASCOT,
	ALIEN_CHARACTER,			/* CREATE PLAYER CHARACTERS */
	JAM_CHARACTER,
	NIKKO_CHARACTER,
	LIA_CHARACTER,
	KERRI_CHARACTER,
	RETRO_CHARACTER,
	OLDMAN_CHARACTER,
	CLOWN_CHARACTER,
	PUMPKIN_CHARACTER,
	HORSE_CHARACTER,
	WIZARD_CHARACTER,
	FRANK_CHARACTER,
	MUMMY_CHARACTER,
	BRIDE_CHARACTER,
	WOLFMAN_CHARACTER,
	REF_CHARACTER,
	CHEERLEADER_LM_CHARACTER,
	CHEERLEADER_KH_CHARACTER,
	CHEERLEADER_JH_CHARACTER,
	PINTO_CHARACTER,
	CREATURE_CHARACTER,
	TOTAL_MASCOTS,
	NBA_PLAYER = TOTAL_MASCOTS,	/* NBA PLAYER */
	MARK_T_CHARACTER,			/* TEAM MIDWAY */
	ROB_G_CHARACTER,
	MARK_G_CHARACTER,
	DAN_T_CHARACTER,
	JEFF_J_CHARACTER,
	SAL_D_CHARACTER,
	JENNIFER_H_CHARACTER,
	EUGENE_G_CHARACTER,
	MATT_G_CHARACTER,
	TIM_B_CHARACTER,
	JIM_G_CHARACTER,
	JON_H_CHARACTER,
	SHAWN_L_CHARACTER,			/* OTHER CHARACTERS */
	ISAIAH_T_CHARACTER,
	TIM_K_CHARACTER,
	TIM_M_CHARACTER,
	WILLY_M_CHARACTER,
	CHAD_CHARACTER,
	CHRIS_S_CHARACTER,
	BETH_S_CHARACTER,
	DAVE_G_CHARACTER,
	JIM_T_CHARACTER,
	LARRY_W_CHARACTER,
	MIKE_L_CHARACTER,
	JASON_S_CHARACTER,
	PAULO_G_CHARACTER,
	BRIAN_L_CHARACTER,
	LEX_CHARACTER,
	GREG_C_CHARACTER,
	JOHN_R_CHARACTER,
	ANDY_E_CHARACTER,
	PAUL_M_CHARACTER,
	RODMAN_CHARACTER,
	TOTAL_SECRET
};

/* Created Players Privileges masks, use with  selected_players[pnum].cmos_data.privileges */
#define PRIV_STEALTH_TURBO			0x01
#define PRIV_BIG_HEAD				0x02
#define PRIV_DRONE_BIG_HEAD			0x04
#define PRIV_COURTSEL				0x08
#define PRIV_HOTSPOT				0x10
#define PRIV_HIDEATTR				0x20
#define PRIV_JERSEY					0x40
#define PRIV_RESERVED3				0x80

// player run speeds
#define QB_SPEED			1.00f
#define QB_TURBO_SPEED		1.30f

#define WPN_SPEED			1.00f
#define WPNPS_SPEED			1.00f
#define WPN_TURBO_SPEED		1.30f

#define CARRIER_SPEED	   	1.00f
#define CARRIER_TURBO_SPEED	1.30f

#define DB_SPEED			1.10f
#define DBPS_SPEED			1.00f
#define DB_TURBO_SPEED		1.40f

#define OTHER_SPEED			1.00f
#define OTHER_TURBO_SPEED	1.30f

// player modes - mirrored in player.equ
enum {
	MODE_CHILL = 0,
	MODE_OFFENSE,
	MODE_DEFENSE,
	MODE_OUT_OF_BOUNDS,
	MODE_TIPOFF,
	MODE_FREETHROW,
	MODE_END_OF_GAME,
	MODE_END_OF_QTR
	};

// attack modes
#define ATTMODE_NONE	0
#define ATTMODE_SWAT	1
#define ATTMODE_DUNK	2
#define ATTMODE_RUN		3
#define ATTMODE_ELBOWS	4
#define ATTMODE_PUSH	5

///////////////////////////////////////////////////////////////////////////////
// Player model defines

// Limbs
#define JOINT_BWRIST	127		// average x,y,z of both wrists

#define JOINT_PELVIS	0
#define JOINT_TORSO		1
#define JOINT_RSHOULDER	2
#define JOINT_RELBOW	3
#define JOINT_RWRIST	4
#define JOINT_LSHOULDER	5
#define JOINT_LELBOW	6
#define JOINT_LWRIST	7
#define JOINT_NECK		8
#define JOINT_RHIP		9
#define JOINT_RKNEE		10
#define JOINT_RANKLE	11
#define JOINT_LHIP		12
#define JOINT_LKNEE		13
#define JOINT_LANKLE	14

// Textures
#define PLAYER_TEXTURE_FLESH_INDEX			0
#define PLAYER_TEXTURE_HEAD_INDEX			1
#define PLAYER_TEXTURE_JERSEY_INDEX			2
#define PLAYER_TEXTURE_JERSEY_NUMBER_INDEX	3
#define PLAYER_TEXTURE_REFLECTION_INDEX		4
#define NUM_PLYRTEXTURES					5

#define	NUM_FIRE_TXTRS	    17	//23

// misc. 3d obj. struct
typedef struct _obj3d_data
{
	obj_3d				odata;				// 3D object data (see note in ani3dD.h)
	LIMB				*limb_obj;
#if defined(VEGAS)
	Texture_node_t		*decal;
#else
	GrMipMapId_t		decal;
#endif
	int					ratio;
} obj3d_data_t;
///////////////////////////////////////////////////////////////////////////////
// Static team data struct
struct stat_team_data
{
	char	szhome[16];						// Home city string
	char	szname[16];						// Team name string
	char	szabbr[4];						// 3-letter abbreviation string
	char	szprfx[4];						// 3-letter prefix string
	void 	*pii_logo;						// Team logo image_info *
	const struct stat_plyr_data *p_spd;		// team static player data *
	LIMB	*court;							// court limb
	char	**court_tex;					// court textures list
	int		num_player;						// number of players on this team
	void 	*pii_logo2;						// Team logo (for team select) image_info *
	void 	*pii_logo3;						// Team logo (for vs page) image_info *
	void 	*pii_cn;						// Team city name (sprite)
};

// Static player data struct
struct stat_plyr_data
{
	char	speed;							// attributes
	char	shot;
	char	dunk;
	char	pass;
	char	dribble;
	char	block;
	char	steal;
	char	power;
	char	clutch;
	char	drone;
	int	hotspot;
	char	tall;								// height in inches
	char	number;							// jersey number
	char	position;						// position
	char	flags;							// skin color (ex: BLACK, WHITE, HANDEDNESS, BODY TYPE)
	char	sound;							// announcer sound number
	float	f_headsc;						// head scale factor
	LIMB *	p_headlm;					// head limb *
	void *	pii_mug;						// mugshot image_info *
	void *	pii_mini;					// mini mugshot image_info *
	void *	pii_name;					// name image_info *
	char	sztext[6];						// text element for player filenames
	char	szlast[16];						// last name text
	char	szfirst[16];					// first name text
	short	weight;							// weight/girth (1-20) 10 is normal size
	char	rebound;
};

enum { PS_NORM, PS_FAT };
enum { PC_BLACK, PC_WHITE };

// Globals in <teamdata.c>
extern const struct stat_team_data teaminfo[];

///////////////////////////////////////////////////////////////////////////////
// drone data struct
//typedef struct _drone_t
//{
//	int		*pscript;
//	int		script_num;			// which route this player is running(1, 2, 3)
//	int		stick, but;
//	int		old_stick, old_but;	// prev different stick/but
//	int		os_time, ob_time;	// ticks spent at prev stick/but
//	int		offender;			// drone we're covering, guy covering us, guy we're blocking, etc
//	int		target;				// drone we're covering, guy covering us, guy we're blocking, etc
//	int		state;				// drone's current offensive state
//	int		flags;				// drone's current offensive/defensive flags
//	int		hold_button_ticks;	// how long a button is released
//	int		avoid_dir_ticks;	// how long should player continue in avoidance dir
//	int		avoid_dir;			// avoidance dir
//	float	avoid_x, avoid_z;	// drones' avoid to points
//	int		think_time;
//	int		wait_time;			// how long to stand still
//	int		juke_time;			// ticks before weapon will consider more juke moves
//	int		pick_player;		// player to go and pick
//	int 	clone_offender;		// defense gets stick dir from offender
//	int		arc_dir;			// 3PT dir from the basket
//	float	def_dist;			// distance plyr wants to be from offender
//	float	delta;
//	float	ax, az;				// drones' arrival points
//} drone_t;
//
//extern drone_t drone_blocks[];
//
//#define DF_CALLING_BALL		0x01
//#define DF_SHOOTING_BALL	0x02
//#define DF_ROAM_LEFT		0x04
//#define DF_ROAM_RIGHT		0x08
//#define DF_ROAM_CLOSE		0x10
//#define DF_ROAM_FAR			0x20
//#define DF_ROAM_PAINT		0x40
//#define DF_ROAM_TRIANGLE	0x80
//#define DF_ROAM_2_3_PAINT	0x100
//#define DF_ROAM_REBOUND		0x200
//#define DF_ROAM_SET_PICK	0x400
//#define DF_ROAM_TO_POINT	0x800

///////////////////////////////////////////////////////////////////////////////
// ball data struct
typedef struct _ball_data
{
	obj_3d		odata;					// 3D object data (see note in ani3dD.h)
	float		tx,ty,tz;				// target spot
	int			int_receiver;			// intended receiver (plyrnum)
	int			flight_time;			// ticks since throw
	int			total_time;				// total ticks to target
	int			who_threw;				// guy who threw me (passes only, not fumbles)
	int			who_shot;				// guy who shot the ball
	unsigned int	color;				// ARGB (8:8:8:8)
	//int			alpha;					// ball transparency
    float       scale;					// size
	int			flags;					// BF_???, defined below
	int			bounces;				// # of bounces on ground - used for bnc snd
} ball_data;

///////////////////////////////////////////////////////////////////////////////
// throw solution struct
struct	throw_solution
{
	int			t;						// time to target (in ticks)
	float		vx,vy,vz;				// initial velocity (units/tick)
	float		xi,yi,zi;				// initial position (qb hand, probably)
	float		xf,yf,zf;				// numbers-height target position (for X)
	int			angle;					// angle of shot (world, from QB)
	float		dist;					// dist from i to f
	float		v;						// net (xyz) velocity
};

///////////////////////////////////////////////////////////////////////////////
// player data struct
typedef struct _plyr_data
{
	obj_3d		odata;					// 3D object data (see note in ani3D.h)
	float		anidata[6][60];			// four quat blocks:
										// 0: primary anim uncompressed quats
										// 1: primary anim interpolation src
										// 2: secondary anim uncompressed quats
										// 3: secondary anim interpolation src
										// 4: primary anim interpolation dest
										// 5: secondary anim interpolation dest
//unused!!!	float		sx,sy,sz;				// last projected screen X,Y,Z of center ground
	float		jpos[15][3];			// joint positions (in camera space, for collisions)
	float		matrices[15][12];		// limb xlation & rotation matrices
	float		matrices_flipped[15][12]; // fliped joint matrices
	float		inv_camera[15][16];
 	int			attack_mode;			// attack mode
	int			attack_time;			// ticks remaining on current attack
	int			celebrate_time;			// ticks remaining on current attack
	int			acj1,acj2;				// attack cylinder joints 1 and 2
	float		acx1,acx2;				// attack cylinder extensions 1 and 2
	float		acr;					// attack cylinder radius
	int			plyrnum;				// player number
	int			position;
	int			team;					// 0 or 1
	int			torso_mode, legs_mode;	// only used for QB (so far)
	int			stick_cur,but_cur;		// current switches
	int			stick_old,but_old;		// last tick's switches
	int			but_new;				// cur & ~old
	int			non_turbo_time;			// # of ticks turbo has not been held down (For spin mv)
	int			stick_time;				// # of ticks stick has been at current value
	int			stick_offcenter_time;	// # of ticks stick has not been centered
	struct _plyr_data *puppet_link;		// pointer to linked puppet type character
	int			throw_target;			// guy I mean to throw at
	const struct stat_plyr_data *p_spd;	// * to static player data struct
	int			non_buta_time;			// # of ticks BUT A has not been held down (For dive mv)
	int			non_butb_time;			// # of ticks BUT B has not been held down (For lateral mv)

// BASKETBALL STUFF
	float		f_headsc;				// head scale factor
	LIMB		*head_model;			// head limb
	LIMB		**body_model;			// body limb
	LIMB		**jnum_model;			// jersey number limb
	LIMB		**reflect_model;		// body reflection limb
	unsigned int hands;					// hand numbers to use
	int			head_dir;				// -1 is .odata.fwd, else 0-1023
	int			head_tgt_dir;			// ->fwd + (-240(Right)...+240(Left))
	int			head_turn_rate;			// head turn rate per tick
	int			head_tick_wait;			// ticks till head will look elsewhere
    int			action;					// Each bit tells what player is currently doing (PA_???)
	int			dribble_mode;			// -=Can't dribble, 0=Can drib, +=Dribbling
	int			ball_in_hand;			// 0 = right hand, 1 = left hand
	int			handedness;				// 0 = right handed, 1 = left handed
	int			receive_ticks_left;		// ticks till I receive pass
//unused!!!	int 	receiver_of_pass;		// player catching this pass
	int			auto_control;			// player is under temp CPU control
	int			flail_time;				// Ticks since last flail happended
	int			flight_time;			// how long player has been in the air
	int			total_time;				// total time player is to be in the air
	int			hangtime;				// -1=not dunking, >=0 is dunking
	int			hangtime_fy;			// where player should be when hangtime is 0
	int			misc_wait_time;			// ?????
	int			ghost;					// player is in ghost mode.
	int			ghostdelay;				// delay the ghost.
	int			no_ball_collide;		// # of ticks player can't collide with ball
	int			HangingOnRim;			// player is hanging on the rim
	int			defense_anim_wait;		// 0=allow anim switch, +=wait n ticks before change allowed
	int			enemy_player;			// I want to hurt this player
	int			shots_missed;			// consective shots missed
	int			launched_from_hotspot;	

// Drone data
	int			drone_vel;				// drone's computed velocity
	int			offender;				// who this player is holding

// Player skill ratings
    int		_dribbling;			// dribbling ability
	int		_stealing;			// stealing ability
    int		_shooting;			// shooting ability
    int		_threeshot;			// 3pt shooting ability
	int		_dunking;			// dunking ability
	int		_passing;			// passing ability
	int		_blocking;			// blocking ability
	int		_defense;			// basic defensive ability
	int		_power;				// strength or power of player
	int		_clutch;			// clutch rating...is this player a prime time player
	float	_speed;				// how fast he can run

} bbplyr_data;

#define PA_SET_PICK 0x1

///////////////////////////////////////////////////////////////////////////////
// Replay data structs/defines
//
#define REPLAY_FRAMES	(57 * 8)		// total # of frames in circular buffer ("tape")

// replay recorder commands; playback will be stopped
#define RR_START	0x01				// start recorder at current frame
#define RR_RESTART	0x02				// rewind tape; start recorder; reload player data
#define RR_REWIND	0x03				// rewind tape; stop recorder; reload player data
#define RR_STOP		0x04				// stop recorder

// replay playback commands; recorder will be stopped
#define RP_STARTALL	0x01				// start or restart playback of entire tape
#define RP_START	0x02				// start or restart playback at specified parms
#define RP_STOP		0x03				// stop playback

// replay flags
#define R_UNUSED	0x01
#define R_INTTOSEQ	0x02
#define R_INTSTREAM	0x04
#define R_ONSCREEN	0x08
//#define R_DRAWBALL	0x10
//#define R_LBALL		0x20
#define R_RBALL		0x40
#define R_TWOPART	0x80

#define RF_TURNHACK	0x43210000			// added to lower 16 bits of <adata.hackval> to flag a MODE_TURNHACK

// replay sound indecies
enum {
	RSND_DRIBBLE1,
	RSND_DRIBBLE2,
	RSND_NET,
	RSND_HIT_BACKBOARD1,
	RSND_HIT_BACKBOARD3,
	RSND_HIT_RIM,
	RSND_DUNK,
	RSND_NET_ON_FIRE,
	RSND_COUNT
};

typedef struct _replay_ball_t			// BALL frame
{
	float		x,y,z;					// [obj_3d] world position
	unsigned int color;					// [ball_data] ARGB (8:8:8:8)
	int			dbias;					// <balldbias> Depth bias. !0 when in net
	float		scale;					// [ball_data] size
//	int			flags;					// replay ops flag
} replay_ball_t;

typedef struct _replay_goal_t			// GOAL frame
{
	float		bend;					// [BBDATA] +=Up
	float		rimbend;				// [BBDATA] Rim: +=Up
	int			rimcolor;				// [BBDATA] ARGB
	int			netcolor;				// [BBDATA] ARGB
	int			netya;					// [BBDATA] Y rot angle
	VERTEX *	pvtx;					// [LIMB] net ptr
#ifdef JONHEY
	VERTEX *	bbox;					// [LIMB] net ptr
#endif
} replay_goal_t;

typedef struct _replay_mtrx_t			// MATRIX data frame
{
	float		xform[12];				// [VIEWCOORD] matrix
} replay_mtrx_t;

typedef struct _replay_plyr_t			// PLAYER frame
{
	float		x,y,z;					// [obj_3d] world position
	int			fwd;					// [obj_3d] facing direction (0-1023)
	int			seq1;					// [obj_3d.adata] sequence index (from xxxxxSEQ.H)
	int			seq2;					// [obj_3d.adata] interpolation source frame seq index
	int			frame1,frame2;			// [obj_3d.adata] frame numbers
	float		t;						// [obj_3d.adata] interpolation frame number (fractional)
	int			hackval;				// [obj_3d.adata] MODE_TURNHACK arg
	unsigned int hands;					// [bbplyr_data] hand numbers to use
	int			head_dir;				// [bbplyr_data] -1 is .odata.fwd, else 0-1023
	int			flags;					// replay ops flag
} replay_plyr_t;

typedef struct _replay_rsnd_t			// REPLAY SOUNDS data frame
{
	unsigned int rsnd[RSND_COUNT];		// replay sounds data matrix
} replay_rsnd_t;

typedef union _replay_t
{
	replay_ball_t	rb;
	replay_goal_t	rg;
	replay_mtrx_t	rm;
	replay_plyr_t	rp;
	replay_rsnd_t	rs;
} replay_t;

typedef struct _replay_obj_t
{
	float		x,y,z;					// [obj_3d] world position
	int			fwd;					// [obj_3d] facing direction (0-1023)
	int			hackval;				// [obj_3d.adata] MODE_TURNHACK arg
	unsigned int hands;					// [bbplyr_data] hand numbers to use
	int			head_dir;				// [bbplyr_data] -1 is .odata.fwd, else 0-1023
	float		jpos[15][3];			// [bbplyr_data] joint positions (in camera space, for collisions)
	float		matrices[15][12];		// [bbplyr_data] limb xlation & rotation matrices
	unsigned int color;					// [ball_data] ARGB (8:8:8:8)
	int			dbias;					// <balldbias> Depth bias. !0 when in net
	float		scale;					// [ball_data] size
	float		pq[60];					// current frame data (script or object struct ram)
	int			last_seq1;				// last seq1 index
	int			last_frame1;			// last frame1 number
	int			last_seq2;				// last seq2 index
	int			last_frame2;			// last frame2 number
	int			flags;					// replay ops flag
} replay_obj_t;


///////////////////////////////////////////////////////////////////////////////
// models
#define MOD_266		0x0
#define MOD_FAT		0x1
#define MOD_498		0x2
#define MOD_LBALL	0x4
#define	MOD_RBALL	0x8
#define MOD_2DIGITS	0x4

// from player.c
extern bbplyr_data player_blocks[];		// player data blocks
extern ball_data ball_obj;				// ball data

#define JERSEYNUM(x)	(((x) > -1) ? (player_blocks[(x)].p_spd->number) : (0))

//
// flags (for obj_3d.flags, mirrored in player.equ)
//
#define PF_REBOUND			0x01		// player is rebounding
#define PF_ONSCREEN			0x02		// player is on screen this tick
#define PF_FLASHRED			0x04		// draw player in mixed in red
#define PF_TURBO			0x08		// player is in turbo mode
#define PF_OLD_TURBO		0x10		// previous tick's turbo
#define PF_NOCOLL			0x20		// collisions disabled
#define PF_ANITICK			0x40		// animated to frame break this tick
#define PF_INBND			0x80		// player is in bounds this tick
#define PF_CELEBRATE		0x100		// player is reacting/celebrating.  don't end action
#define PF_SHOOTING			0x200		// player is shooting the ball
#define PF_PASSING			0x400		// player is passing the ball
#define PF_FLASHWHITE		0x800		// draw player in bright white
#define PF_DUNKER			0x1000		// player is dunking
#define PF_ALLYDUNKER		0x2000		// player is ally opp'ing
#define PF_SWATING			0x4000		// player is swinging at ball being shot
#define PF_LAYUP			0x8000		// player is doing a layup
#define PF_FUNKY_DRIBBLE	0x10000		// funky fresh shake move
#define PF_STEALING			0x20000		// player is stealing the ball
#define PF_CATCHING			0x40000		// player is catching the ball
#define PF_BOTH_HANDS		0x80000		// put ball in both hands
#define PF_NO_LOOK			0x100000	// dont allow head turn logic
#define PF_IN_AIR_BLOCK		0x200000	// defense blocks ball in air
#define PF_QUICK_REBOUND	0x400000	// quick rebound!
#define PF_PLYRFLAGS	(PF_SHOOTING|PF_PASSING|PF_DUNKER|PF_ALLYDUNKER|PF_CELEBRATE|PF_SWATING|PF_LAYUP|PF_FUNKY_DRIBBLE|PF_STEALING|PF_CATCHING|PF_BOTH_HANDS|PF_IN_AIR_BLOCK|PF_REBOUND|PF_QUICK_REBOUND)
//
// Ball flags
//
#define BF_WHITE		0x01			// ball is solid white
#define BF_BOUNCEPASS	0x02			// bounce pass indstead of an air pass
#define BF_NOSTEAL		0x04			// ball can't be stolen
#define BF_NOINTERCEPT	0x08			// ball is being passed and can't be stolen
#define BF_TURBO		0x10			// Ball in turbo pass

// positions
#define GUARD	0
#define FORWARD	1
#define CENTER	2

// macros for iterative generate_matrices
#define SETPARENT 0x80
#define NOMATRIX  0x40
#define DONE  512

#endif
