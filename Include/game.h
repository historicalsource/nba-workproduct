/*
 *		$Archive: /video/Nba/Include/game.h $
 *		$Revision: 100 $
 *		$Date: 4/19/99 7:14p $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifndef __GAME_H__
#define __GAME_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_game_h = "$Workfile: game.h $ $Revision: 100 $";
#endif

//-------------------------------------------------------------------------------------------------
//                  This files has various info for NBA ON NBC
//-------------------------------------------------------------------------------------------------

// Fouls to give, before free-throw
#define	FOUL_LIMIT		5			// per qtr.

//
// Shot counts for fire
//
#define NUM_HOTSPOTS		18
#define	HOTSPOT_ACTIVE_CNT	2
#define	HOTSPOT_RADIUS		8.0f

#define TM_FIRECNT			3					// # of shots made to be on fire
#define MAX_TM_FIRE_SHOTS	4					// max # of shots before fire is turned off

#define FIRECNT				3					// # of shots made to be on fire
#define MAX_FIRE_SHOTS		4					// max # of shots before fire is turned off

//
// Number of humans/total players
//
#define MAX_PLYRS		4					// # of human players supported

#ifdef NUM_PLAYERS
#if NUM_PLAYERS != 4 && NUM_PLAYERS != 6
#error NUM_PLAYERS must be 4 or 6
#endif
#else
#define NUM_PLAYERS		4					// # of active players
#endif

#define NUM_PERTEAM		(NUM_PLAYERS/2)		// # of active players per team

// TIME in EACH QUARTER
#define QUARTER_TIME			0x00030000	// (3:00:00)

// TIME minimum needed to allow buy-in
#define MINIMUM_QUARTER_TIME	0x00011e00	// (1:30:00) min time left to allow buyin

// TIME in SHOT CLOCK
#define SHOTCLOCK_TIME			0x00001500	// (0:00:24) quicker of course

// Equates for QUARTERS
#define EOQ_CNT			((57 * 7) + 0)		// tick count for EOQ timeout

#define EOQ_1			0
#define EOQ_2			1
#define EOQ_3			2
#define EOQ_4			3

#define NUM_QUARTERS	4					// # of regular quarters per game

#define EOQ_OT1			4
#define EOQ_OT2			5
#define EOQ_OT3			6

#define MAX_QUARTERS	7					//  plus maximum # of overtimes

// Court unit-dimensions from mid to end & side (original)
#define COURT_ORGL		(124.0f * 1.1f)
#define COURT_ORGW		(72.0f * 1.05f)

// Court unit-dimensions from mid to end & side
#define COURT_L			(124.0f * 1.1f)
#define COURT_W			(72.0f * 1.05f)

// Court unit-dimensions from mid to end & side OB
#define OUT_OF_BOUNDS_X	COURT_L
#define OUT_OF_BOUNDS_Z	COURT_W

// Ball * rim defines
#define BALL_GRAVITY	-0.05f //.06f
#define BALL_RADIUS		1.667f

#define RIM_X			(112.166f * 1.1f)
#define RIM_Y			39.55f	//39.2f	//36.3f	//34.0f
#define RIM_Z			0.0f
#define RIM_CX			(115.3425f * 1.1f)
#define RIM_CY			RIM_Y
#define RIM_CZ			RIM_Z
#define RIM_CZI			0			// Integer version
#define BACKBOARD_X		(119.88f * 1.1f)

#define RIM_RAD			2.98f
#define RIM_EDGERAD		0.25f							// Radius of edge
#define RIM_RADOUT		(RIM_RAD+RIM_EDGERAD)	// Outer radius

#define OOB_RADIUS		0.5f					// Player shouldn't go past this point

//
// Team index constants (in TEAM CITY alphabetical order)
//
#define TEAM_ATL	0			// Atlanta
#define TEAM_BOS	1			// Boston
#define TEAM_CHA	2			// Charlotte
#define TEAM_CHI	3			// Chicago
#define TEAM_CLE	4			// Clevland
#define TEAM_DAL	5			// Dallas
#define TEAM_DEN	6			// Denver
#define TEAM_DET	7			// Detroit
#define TEAM_GOL	8			// Golden State
#define TEAM_HOU	9			// Houston
#define TEAM_IND	10			// Indianapolis
#define TEAM_LAC	11			// Los Angeles
#define TEAM_LAL	12			// Los Angeles
#define TEAM_MIA	13			// Miama
#define TEAM_MIL	14			// Milwaukee
#define TEAM_MIN	15			// Minnesota
#define TEAM_NJN	16			// New Jersey
#define TEAM_NYK	17			// New York
#define TEAM_ORL	18			// Orlando
#define TEAM_PHI	19			// Philadelphia
#define TEAM_PHO	20			// Phoenix
#define TEAM_POR	21			// Portland
#define TEAM_SAC	22			// Sacramento
#define TEAM_SAN	23			// San Antonio
#define TEAM_SEA	24			// Seattle
#define TEAM_TOR	25			// Toronto
#define TEAM_UTA	26			// Utah
#define TEAM_VAN	27			// Vancouver
#define TEAM_WAS	28			// Washington
#define TEAM_SEC	29			// The catch all secret team

#define NUM_TEAMS	29			// # NBA(excludes the secret team)of teams to choose from

#define TEAM_HAWKS			TEAM_ATL
#define TEAM_CELTICS		TEAM_BOS
#define TEAM_HORNETS		TEAM_CHA
#define TEAM_BULLS			TEAM_CHI
#define TEAM_CAVALIERS		TEAM_CLE
#define TEAM_MAVERICKS		TEAM_DAL
#define TEAM_NUGGETS		TEAM_DEN
#define TEAM_PISTONS		TEAM_DET
#define TEAM_WARRIORS		TEAM_GOL
#define TEAM_ROCKETS		TEAM_HOU
#define TEAM_PACERS			TEAM_IND
#define TEAM_CLIPPERS		TEAM_LAC
#define TEAM_LAKERS			TEAM_LAL
#define TEAM_HEAT			TEAM_MIA
#define TEAM_BUCKS			TEAM_MIL
#define TEAM_TIMBERWOLVES	TEAM_MIN
#define TEAM_NETS			TEAM_NJN
#define TEAM_KNICKS			TEAM_NYK
#define TEAM_MAGIC			TEAM_ORL
#define TEAM_SEVENTYSIXERS	TEAM_PHI
#define TEAM_SUNS			TEAM_PHO
#define TEAM_TRAILBLAZERS	TEAM_POR
#define TEAM_KINGS			TEAM_SAC
#define TEAM_SPURS			TEAM_SAN
#define TEAM_SUPERSONICS	TEAM_SEA
#define TEAM_RAPTORS		TEAM_TOR
#define TEAM_JAZZ			TEAM_UTA
#define TEAM_GRIZZLIES		TEAM_VAN
#define TEAM_WIZARDS		TEAM_WAS
#define TEAM_SECRET			TEAM_SEC

//
// Misc constants
//
#define NO		0
#define YES		1

#define HA_HOME	0
#define HA_AWAY	1

#define SIDE_0	0
#define SIDE_1	1

#define SHOW	0
#define HIDE	1

#define COURT_IS_INDOORS				((which_court == COURT_INDOOR) || (which_court == COURT_INDOOR2) ||\
											(which_court == COURT_MIDWAY) || (which_court == COURT_NBC))
#define COURT_IS_OUTDOORS				((which_court == COURT_GILMORE) || (which_court == COURT_GEER))

#define COURT_INDOOR			0		// team 1 court
#define COURT_GILMORE			1		// street court
#define COURT_GEER				2		// jungle court
#define COURT_INDOOR2			3		// team 2 court
#define COURT_MIDWAY			4		// Midway court
#define COURT_NBC				5		// NBC court

//
// Misc macros
//
// Based on a 94ft x 50ft court
//
#define FT2UN	(OUT_OF_BOUNDS_X / 47.0f)	// Feet-to-units factor (units/foot)
#define YD2UN	(FT2UN * 3.0f)				// Yards-to-units factor (units/yard)
#define IN2UN	(FT2UN / 12.0f)				// Inches-to-units factor (units/inch)
#define FT(x)	(FT2UN * x)

#define ISHUMAN(x)		(game_info.plyr_control[(x)->plyrnum] >= 0)
#define ISDRONE(x)		(!ISHUMAN(x))

#define TEAM1_MASK		(four_plr_ver ? 0x3 : 0x1)
#define TEAM2_MASK		(four_plr_ver ? 0xC : 0x4)
#define TEAM_MASK(x)	((x) ? TEAM2_MASK : TEAM1_MASK)

#define ISHUMANTEAM(x)	(p_status & TEAM_MASK((x)->team))
#define ISDRONETEAM(x)	(!ISHUMANTEAM(x))

#define FRELX(x,z,t)	(z * isin(t) + x * icos(t))
#define FRELZ(x,z,t)	(z * icos(t) - x * isin(t))

#define ABS(x)			(((x)<0) ? (-(x)) : (x))
#define LESSER(x,y)		((x)>(y) ? (y) : (x))
#define GREATER(x,y)	((x)>(y) ? (x) : (y))

//
// Player(1-4) data struture
//
typedef struct pdata
{
	struct sprite_info * si_frame;		// turbo bar frame sprite *
	struct sprite_info * si_frameglw;	// turbo bar glow sprite *
	struct sprite_info * si_turbo;		// turbo bar sprite *
	struct sprite_info * si_msg1;		// message sprite *
	struct sprite_info * si_msg2;		// message sprite *

	struct sprite_info * si_foul_frm; 	// freethrow disc sprite *
	struct sprite_info * si_foul_num;		// freethrow number sprite *

	float	turbo;						// turbo % (0.0 to 100.0) remaining
	float	turbo_last;					// last turbo % remaining
	float	turbo_w;					// turbo bar pixel width

	int		powerups;					// PU_???, defined below
	int		is_human;					// set if human, clear if not

	int		credits_paid;				// paid credits...for FULL game purchased
	int		credits_show;				// info box is showing # of credits
	int		quarters_purchased;			// bought quarters
	int		please_wait;				// player has to wait until quarter end

	struct	sprite_info * si_fframe;	// turbo bar fire frame sprite *
	int		fframe;						// frame nbr. of FIRE effect
	int		fdelay;						// fire frame change delay

	long	alpha_level;				// used for glowing turbo frame
	long	delta;						// used for glowing turbo frame
} pdata_t;

//
// Team (0-1) data structure...which keeps track of the stats (bookeeping)
// 
//typedef struct tdata
//{
//	int			time_of_poss;
//	short		two_pts_attempted;
//	short		two_pts_made;
//	short		three_pts_attempted;
//	short		three_pts_made;
//	short		dunks_attempted;
//	short		dunks_made;
//	short		blocks;
//	short		defense_blocks;
//	short		offense_rebounds;
//	short		defense_rebounds;
//	short		steals;
//	short		assists;
//	short		turnovers;
//} tdata_t;

//컴컴컴컴컴컴컴�
// Jump meter struct
//
struct tagJMETER {
	struct sprite_info * frame_obj;
	struct sprite_info * meter_obj;
	struct sprite_info * text_obj;
	float	t_dif;
	float	h_max;
};
// Externs from <plyrinfo.c>
extern struct tagJMETER jmeter[];		// Jump-meter info
extern struct image_info jmeter_ii[];	// Changeable copies of meter image info

//컴컴컴컴컴컴컴�
typedef	struct _BBDATA {	// Backboard data
	float	bend;			// +=Up
	float	rimbend;		// Rim: +=Up
	int		rimcolor;		// ARGB
	int		netcolor;		// ARGB
	int		netya;			// Y rot angle

//  ^^^ order of these elements is crucial to replay code -DJT

	void	**netv_p;		// * vertex list or 0
#ifdef JONHEY
	void	**netv_bbox;
#endif
	float	netvposf;		// Vertex list fractional pos (0-.999999)
	float	netvstep;		// Vertex list step value
	int		netvpritm;		// Vertex list priority time
	int		bendcnt;		// # procs bending
	float	mom;			// Momentum
	int		rimbendcnt;		// # procs bending
	float	rimmom;			// Momentum
	int		flgs;			// Flags
} BBDATA;


//
// Game info struct
//
typedef struct game_info_
{
	int		game_state;					// GS_???, defined below
	int		game_mode;					// GM_???, defined below
	int		game_quarter;				// game period # (0 to MAX_QUARTERS-1)
	int		game_time;					// game clock (byte order= 0 : min : sec : tenths)
	int 	shotclock;					// game shotclock
	int		team[2];					// TEAM_???, defined above
	int		scores[2][MAX_QUARTERS];	// scores by quarter

	int		off_scored;					// which side just scored
	int		off_side;					// which side (0 or 1) has the ball
	int		off_goal;					// XOR with side (0 or 1) for active goal
	int		active_goal;				// which goal (0 or 1) is active
	int		home_team;					// which side (0 or 1) is the home team
	int		poss_change;				// did the ball change hands during play? T/F

	int		plyr_control[NUM_PLAYERS];	// which human is controlling me? (-1 if none)
	int		ball_handler;				// player with the basketball
	int		ball_mode;					// BM_???, defined below
	int		passer;						// player who passed the ball
	int 	shooter;					// player shooting the ball
#if NUM_PERTEAM > 2
	int		receiver_a;					// icon passing
	int		receiver_b;					// icon passing
#endif
	int		fire_player;				// player who just scored
	int		fire_cnt;					// how many consecitive shots made
	int		fire_timeout;				// tick cnt for when fire cools off

	int		name_tick_wait;				// wait time for next name call 
	int		sc_scored;					// plyr who just scored last bucket
	int		sc_hooped;					// plyr who got scored on

	int 	master_volume;				// for compatibility with bootys sound/coin stuff.
	int 	minimum_volume;
	int 	attract_mode_volume;
	int 	attract_coin_in;
	int		team_handicap[2];			// 0=wimp, 5=normal, 10=super-tough
	int		auto_throw_cntr;			// used for FREETHROWS
	int		foul_cnt[NUM_PLAYERS];		// foul count for each player
	int		freethrow_shooter;			// player shooting free throws (-1 if none)

	int		team_fire;		    		// 1 = team on fire else 0
	int		tm_fire_cnt[2];				// how many consecitive allyoops made
	int		tm_fire_timeout;			// tick cnt for when fire cools off

	int		hotspot_made_cnt[NUM_PLAYERS];		// number times made shot from hotspot
} game_info_t;

// from mainproc.c
extern game_info_t game_info;
extern game_info_t game_info_last;

//
// Game state constants
//
#define GS_DIAGNOSTICS		-1			// diagnostics mode
#define GS_ATTRACT_MODE		0			// None of the below
#define GS_PRE_GAME			1			// All screens before game play
#define GS_GAME_MODE		2			// Actual game play
#define GS_EOQ				3			// all screens from end of quarter to start of next
#define GS_GAME_OVER		4			// Everything after game until start of ATTRACT MODE
#define GS_FREE_PRICE		5			// screen at game end...telling winning team stays free
#define GS_HALF_PRICE		6			// screen at game end...telling winning team stays 1/2 price

//
// Game mode constants
//
enum {
	GM_PRE_TIPOFF,						// Pre-game
	GM_TIPOFF,							// Tip-off
	GM_IN_PLAY,							// Active gameplay
	GM_INBOUNDING,						// In-bounding after score/ob
	GM_BALL_OUT,						// 
	GM_GAME_OVER,						// Game over
	GM_INITIALIZING,					// set before game starts
	GM_FREE_THROW						// after X fouls
};


// SHOT TYPES
#define JUMPER_2	0
#define JUMPER_3	1
#define DUNK		2
#define LAYUP		3

// BALL MODES
#define BM_OFF_INTO_PLAY	-2
#define BM_FREE_ON_COURT	-1
#define BM_DRIBBLING		0
#define BM_PASSING			1
#define BM_SHOT_RELEASE		2
#define BM_SHOOTING			3
#define BM_BOUNCE_ON_RIM	4
#define BM_TIPOFF			5

// DRIBBLE MODES
#define DM_CANT_DRIBBLE		-1
#define DM_CAN_DRIBBLE		0
#define DM_DRIBBLING		1

#define NO_JUKER		-1
#define NO_JUKEE		-1
#define NO_SHOOTER		-1
#define NO_PASSER		-1
#define NO_RECEIVER		-1
#define	NO_DRIBBLER		-1
#define NO_BALLHANDLER	-1
#define NO_HUMANPLAYER	-1

#define	SPRITE_HRES	512.0f
#define	SPRITE_VRES	384.0f

// Ball shoot modes

#define	M_SHTMD_GOOD	0x100

enum {
	SHTMD_NET = 0,
	SHTMD_RIMSHORT,
	SHTMD_RIMLONG,
	SHTMD_RIML,
	SHTMD_RIMR,
	SHTMD_BOARD,
	SHTMD_AIR,
	SHTMD_MAX,
};

//컴컴컴컴컴컴컴�
// replay objects ordering (in replay_objs)
enum {
	CAMX_INDEX,
	PLYR_INDEX,
	TWOP_INDEX = (NUM_PLAYERS + PLYR_INDEX),
	BALL_INDEX = (NUM_PLAYERS + TWOP_INDEX),
	BROT_INDEX,
	GOAL_INDEX, GOA2_INDEX,
	RSND_INDEX,
	NUM_ROBJS
};

#define SHTMD_RIM SHTMD_RIMSHORT
#define SHTMD_RIMMAX 5

#if defined(VEGAS)
#ifndef	HRESf
#define	HRESf	512.0f
#endif
#ifndef	VRESf
#define	VRESf	384.0f
#endif
#ifndef	HRES
#define	HRES	512
#endif
#ifndef	VRES
#define	VRES	384
#endif
#endif

#endif
