/****************************************************************
*		$Archive: /video/Nba/drone.c $
*		$Revision: 152 $
*		$Date: 4/22/99 3:25p $
*
*		Copyright (c) 1998 Midway Games Inc.
*		All Rights Reserved
*
*		This file is confidential and a trade secret of Midway Games Inc.
*		Use, reproduction, adaptation, distribution, performance or
*		display of this computer program or the associated audiovisual work
*		is strictly forbidden unless approved in writing by Midway Games Inc.
*
*		Started 12/10/98 by Shawn Liptak from Hangtime drone.asm
*
*.Last mod - 4/7/99 3:56
****************************************************************/

#ifdef INCLUDE_SSID
char *ss_drone_c = "$Workfile: drone.c $ $Revision: 152 $";
#endif

#ifndef	SHAWN
#undef	DEBUG
#endif

// TEMP!
//#define	DEBUG
#ifndef TEST_VER
#define	DEBUG_SKILL
#endif

#ifdef	DEBUG 
//#define	DEBUG_DRNINFO
#define	DEBUG_DRNSEEK			// Show seek positions
//#define	DEBUG_DRNDRV			// Show ballhandler drive info
//#define	DEBUG_DRNPASS
//#define	DEBUG_DRNJMP
//#define	DEBUG_DRNPLAY
//#define	DEBUG_DRNOWOB			// Show offense wo ball info
//#define	DEBUG_DRNCHASE			// Show chasemode info
//#define	DEBUG_INBND

//#define	DEBUG_DRNNOREBND
//#define	DEBUG_DRNNOSTEAL		// Can't steal
//#define	DEBUG_DRNSAFEPASS		// Only safe passes
//#define	DEBUG_DRNPERDEF		// Perfect defense

//#define	DRONE_OFF
//#define	DRONE_NOMOVE
#endif

#ifdef	DEBUG
#define	OPRINTFDB	oprintf
#define	HALT	halt
#else
#define	OPRINTFDB	//
#endif

/****************************************************************/
/* Todo:

- Check 3 pt
- Pass through opps
- Check pass back fm drone tmate
- Add turbo stuff

- Find stand at top left corner bug

 Change dir & dist to use precomputed
 Pass back to better ball handler on inbound
 Pass to better shooter if need 3

- Press shoot & pass together: alleyoop
		check if close enough to board
		int PlayerInAllyZone(obj_3d *pobj)
 Double dunk
- Adjust diff every minute
- Use player skills
- Defense block shot: seek infront of shooter
*/

/****************************************************************/
// System includes

#include <math.h>
#include <goose/goose.h>
#include <stdlib.h>

/****************************************************************/
// User includes

#include "include/ani3d.h"
#include "include/game.h"
#include "include/mainproc.h"
#include "include/fontid.h"
#include "include/player.h"
#include "include/id.h"
#include "anim/gen_seq.h"
#include "gameadj.h"
#include "include/select.h"
#include "include/putil.h"

/****************************************************************/

#ifdef	DEBUG
//#define	FIRECNT	1
#endif

// From player.c. Should be in .h
#define MAX_PASS_TIME		79.0f
#define MIN_PASS_TIME		16.0f
#define MAX_PASS_DIST		280.0f

#define	TSEC			tsec
#define	PLYR_MAX		NUM_PLAYERS
#define	TEAM_MAX		(NUM_PLAYERS / 2)
#define	JOY_CENTER	24

typedef	void	*PV;
typedef	struct object_node	OBJN;

//****************************************************************
// Global data & function prototypes

int	ptangle (float *p1, float *p2);
int	rndrng0 (int max);
int	compute_score (int);
int	three_pt_check (obj_3d *o_p);
int	in_bounds (obj_3d *pobj);
void	inbound_ball_speech_proc(int *args);
int	inbound_passwait();

extern int	halt;
extern int	tsec;
extern int	p_status;
extern player_record_info	selected_players[];
extern VIEWCOORD cambot;
extern int	p2p_angles[NUM_PLAYERS][NUM_PLAYERS];
extern pdata_t pdata_blocks[];
extern int 	auto_rebounding;

extern int	OOBPasser;
extern int	update_status_box;
extern int	reset_shotclock;
extern int	shotclock_active;
extern int	clock_active;
extern int	pushed_cnt[];
extern int	pup_tourney;

//***************************************************************
// Local data & function prototypes

typedef	struct	_VEC {
	float		x;
	float		y;
	float		z;
} VEC;

// Skill threshholds
#define	DRNSKL_OTURB		-9		// Offense turbo run
#define	DRNSKL_OTURBB		-7		// Turbo breakaway
#define	DRNSKL_O3LESS		-7		// Shoot less threes
#define	DRNSKL_OSLESS		-6		// Shoot less often
#define	DRNSKL_OALLY		-8		// Less alleyoops
#define	DRNSKL_NASTYRND	-10	// Random nasty
#define	DRNSKL_NASTYLS		-7		// Low second nasty
#define	DRNSKL_PUSH			5		// Don't push until hit

#define	DRN_TOURNSKL		1		// Skill level for tournament mode

#define	DRONEDB_STRID	0x4020
#define	DRN_FOOT	3.92f					// Units per foot
#define	DRN_INCH	(DRN_FOOT/12.0f)	// Units per inch

#define	M_BUTS	P_A		// Shoot
#define	M_BUTP	P_B		// Pass
#define	M_BUTT	P_C		// Turbo

#define	COLLRESEEKCNT	15	// # consecutive collisions for a reseek

#define	PF_ANYSHOT	(PF_SHOOTING | PF_DUNKER | PF_ALLYDUNKER | PF_LAYUP)

enum {
	DRNMD_CHASEB, DRNMD_OFF, DRNMD_OFFWOB, DRNMD_DEF };

static char	*drnmd_pt[] = { "Cha", "Off", "Owob", "Def" };

enum {
	DRNMD2_NONE, DRNMD2_INBND, DRNMD2_REBND, DRNMD2_REBNDJMP,
	DRNMD2_OWAIT, DRNMD2_ALLY };

static char	*drnmd2_pt[] = { "None", "InBn", "Reb", "RebJ", "OWt", "Ally" };

typedef	struct _SKPOS {	// Seek position
	float		x;
	float		z;
} SKPOS;

typedef	struct _DRONE {
	int			flgs;
	int			cflgs;			// Command flags
	int			mode;
	int			mode2;

	int			num;				// My # (0-5)
	bbplyr_data	*pl_p;			// * my data
	float			myhoopx;			// My hoop x (I defend)
	int			myhoopdir;		// Current direction (10:0)
	float			myhoopdist;		// Current distance
	float			ohoopx;			// Opponents hoop x (I shoot at)
	float			ohoopdist;		// Current distance to opponents hoop
	float			balldist;		// Current distance to ball
	int			mate;				// Teammates # (0-5)
	bbplyr_data	*matepl_p;		// * buddy's data

	int			but;				// Buttons
//	int			butold;
//	int			butotime;
	int			stick;			// Joystick dir (0-49)
	int			seekdir;			// Dir we're seeking (0-1023) or -1
	int			seekcnt;			// Countdown to new seek
	SKPOS			seek_t[2];		// Positions
	int			seekzmd;			// Zigzag mode (+=Hoop side angle, -=Far side)
	int			skill;			// -15 to 15
	int			ptsdown;			// Score difference. -=winning, +=losing
	int			nastycnt;		// Countdown to end nastymode
	int			lowsecagr;		// Low time to be agressive (seconds << 8)
	int			guardnum;		// # of guy i'm guarding (0-5)
	int			guardmatenum;	// # of teammate of guy i'm guarding (0-5)
	float			guarddist;		// Guarding distance to maintain

	float			odist;			// Closest opponent. Set by closest_optome

	int			collcnt;			// # of consecutive collisions (0-?)
} DRONE;

DRONE	drone_t[PLYR_MAX];

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

void	drone_offense (DRONE *drn_p);
int	drone_chk3ptr (DRONE *drn_p, int mode);
int	drone_drvtorim (DRONE *drn_p);
int	drone_pass (DRONE *drn_p);
void	drone_injmp (DRONE *drn_p, int onfire);
int	drone_findplay (DRONE *drn_p);
int	pt_dir8 (float sx, float sz, float dx, float dz);
void	drone_playload();
void	drone_offwoball (DRONE *drn_p);
void	drone_defense (DRONE *drn_p);
int	drone_defensechk (DRONE *drn_p, DRONE *drn2_p);
void	drone_chaseball (DRONE *drn_p);
int	drone_inbound (DRONE *drn_p);
int	drone_inbounddef (DRONE *drn_p);
int	drone_getcurskill (DRONE *drn_p);

DRONE	*drone_closest_opptoball (DRONE *drn_p);
DRONE	*drone_closest_opptome (DRONE *drn_p, float time);
DRONE	*drone_closest_opptopush (DRONE *drn_p, float time);
int	drone_firechk (DRONE *drn_p);
int	drone_seek (DRONE *drn_p);
int	drone_seekxy (DRONE *drn_p, float skx, float skz);
int	drone_getcurskill();
int	drone_timeok();
int	dir2stick (int dir);
int	pt_angledist (void *pt1_p, void *pt2_p, float *dist_p);

void	drone_seekdraw (void *o_p);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

int	allow_switching = 1;

static int	teammate_t[] = {	// Buddy's number
#if	PLYR_MAX == 4
 1,0, 3,2
#else
 1,0,0, 4,3,3
#endif
 };

/****************************************************************/

/********************************
********************************/

void	init_drones()
{
	DRONE		*drn_p;
	int		n, i;

	drn_p = drone_t;
	n = 0;
	do {

		drn_p->num = n;
		drn_p->pl_p = &player_blocks[n];

		drn_p->skill = 0;
		drn_p->mode = DRNMD_CHASEB;
		drn_p->mode2 = DRNMD2_NONE;
		drn_p->seekcnt = 1;
		drn_p->seekzmd = 256;

		i = -RIM_CX;
		if (n >= PLYR_MAX / 2) i = -i;
		drn_p->myhoopx = i;
		drn_p->ohoopx = -i;

		i = teammate_t[n];
		drn_p->mate = i;
		drn_p->matepl_p = &player_blocks[i];

		drn_p->guardnum = (n + PLYR_MAX / 2) % PLYR_MAX;
		drn_p->guardmatenum = (i + PLYR_MAX / 2) % PLYR_MAX;

		drn_p->lowsecagr = 10 << 8;

		drn_p++;
	} while (++n < PLYR_MAX);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	drone_playload();

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#ifdef	DEBUG_DRNSEEK
	create_object ("droneseek", OID_PLAYER,
			OF_NONE, DRAWORDER_FIELDFX+0x10,	NULL, drone_seekdraw);
#endif
}


/********************************
 Do global drone update. Occurs before player procs
********************************/

void	drone_update()
{
	DRONE		*drn_p;
	ball_data	*bd_p;
	bbplyr_data *pl_p;
	pdata_t	*pd_p;

	int		scoredif;

	int		n, i;
	float		fx, fz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	if (game_info.game_mode == GM_INITIALIZING) return;

	bd_p = &ball_obj;

	scoredif = compute_score (1) - compute_score (0);

	drn_p = drone_t;
	pd_p = pdata_blocks;

	n = 0;
	do {

		pl_p = drn_p->pl_p;

		fx = bd_p->odata.x - pl_p->odata.x;
		fz = bd_p->odata.z - pl_p->odata.z;
		fx = fsqrt (fx * fx + fz * fz);
		drn_p->balldist = fx;

		fx = drn_p->ohoopx - pl_p->odata.x;
		fz = RIM_CZ - pl_p->odata.z;
		fx = fsqrt (fx * fx + fz * fz);
		drn_p->ohoopdist = fx;

		if (n == PLYR_MAX / 2) scoredif = -scoredif;	// Invert for team2
		drn_p->ptsdown = scoredif;

		i = drn_p->collcnt + 1;
		if (pl_p->odata.pobj_hit == 0) i = 0;
		drn_p->collcnt = i;

		i = pl_p->plyrnum;
#if	NUM_PERTEAM > 2
		if (i >= NUM_PERTEAM) i -= 1;	// pstatus only has 4 bits
#endif
		if ((p_status & 1 << i) == 0)	// I'm drone?
			pd_p->turbo = 100;

		drn_p++;
		pd_p++;
	} while (++n < PLYR_MAX);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#ifdef	DEBUG
	if (!halt) {
		delete_multiple_strings (DRONEDB_STRID, 0xffffffff);
//		set_text_font (FONTID_NBA10);
//		set_string_id (DRONEDB_STRID);
//		set_text_position (50, 200, 1);
//		oprintf ("%djpstat %x", HJUST_LEFT, p_status);
	}
#endif

}

/********************************
********************************/

void	drone_calcstatus (DRONE *drn_p)
{
	bbplyr_data *pl_p;
	float		f_t[3];
	int		dir;

	int		n, i;
	float		fx, fz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	pl_p = drn_p->pl_p;

	f_t[0] = drn_p->myhoopx;
	f_t[2] = RIM_CZ;

	dir = pt_angledist (&pl_p->odata.x, f_t, &drn_p->myhoopdist);
	drn_p->myhoopdir = dir;

}


/********************************
 Find angle and distance on XZ plane from 1st 3d point to 2nd
 (* pt 1, * pt 2, * float to save distance)
> Angle (0-1023) (0=6 oclock, 256=3 oclock)
********************************/

int	pt_angledist (void *pt1_p, void *pt2_p, float *dist_p)
{
	VEC		*vc1_p;
	VEC		*vc2_p;
	float		dist;
	float		sin;
	int		angle;

	float		fx, fz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	vc1_p = pt1_p;
	vc2_p = pt2_p;

	fx = vc2_p->x - vc1_p->x;
	fz = vc2_p->z - vc1_p->z;

	dist = fsqrt (fx * fx + fz * fz);
	*dist_p = dist;
	if (dist == 0) return (0);		// Prevent div by 0

	sin = fabs (fx) / dist;
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

/********************************
********************************/

void	get_drone_switches (bbplyr_data *pl_p)
{
	DRONE		*drn_p;
	ball_data	*bd_p;
	int		pnum;

	int		i;
	float		fx, fy, fz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	pnum = pl_p->plyrnum;
	drn_p = &drone_t[pnum];

#if	NUM_PERTEAM > 2
	if (pnum == 2 || pnum == 5) {
		pl_p->stick_cur = drn_p->stick = JOY_CENTER;
		pl_p->but_cur = M_BUTP;
		return;
	}
#endif

#ifdef	DEBUG
	set_text_font (FONTID_NBA10);
	set_string_id (DRONEDB_STRID);
#endif

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Do mode

	i = game_info.ball_handler;

	if (OOBPasser >= 0) {
		if (OOBPasser == pnum || OOBPasser == drn_p->mate)
			drone_inbound (drn_p);
		else
			drone_inbounddef (drn_p);
	}
	else {
		if (i < 0)		// No owner?
			drone_chaseball (drn_p);

		else if (i == pnum)
			drone_offense (drn_p);

		else if (i == drn_p->mate)
			drone_offwoball (drn_p);		// Teammate has ball?

		else	drone_defense (drn_p);
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#ifdef	DEBUG_DRNINFO
	i = pl_p->plyrnum;
	fy = VRESf - 13 - i * 13;
	fz = 1;

//	set_text_position (20, fy, fz);
//	oprintf ("%djSk %3.0f %3.0f", HJUST_LEFT, drn_p->seek_t[0].x, drn_p->seek_t[0].z);

	set_text_position (140, fy, fz);
	oprintf ("%dj%s", HJUST_LEFT, drnmd_pt[drn_p->mode]);

	set_text_position (200, fy, fz);
	oprintf ("%djBDIST %3.0f", HJUST_LEFT, drn_p->balldist);

	set_text_position (300, fy, fz);
	oprintf ("%djCTRL %2x %2x", HJUST_LEFT, drn_p->stick, drn_p->but);
#endif

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#ifdef	DRONE_NOMOVE
	drn_p->stick = JOY_CENTER;
#endif

	pl_p->stick_cur = drn_p->stick;
	pl_p->but_cur   = drn_p->but;


//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#ifdef	DRONE_OFF
//	if (pl_p->plyrnum != 0) {
		pl_p->stick_cur = drn_p->stick = JOY_CENTER;
		pl_p->but_cur   = drn_p->but   = 0;
//	}
#endif
}

/********************************
********************************/

#define	M_DRNCF_SHOOT	1
#define	M_DRNCF_PASS	2

int	omdsk_t[] = {	// Mode switch max seek time
	90,90,40,35,30,	// Up 15-11
	25,20,16,14,12,	// 10-6
	10,9,8,7,6,			// 5-1
	6,						// Even score
	6,5,5,5,4,			// Dn 1-5
	4,4,3,3,3,			// 6-10
	2,2,2,1,1			// 11-15
};

void	drone_offense (DRONE *drn_p)
{
	game_info_t	*gi_p;
	bbplyr_data *pl_p;
	bbplyr_data *pl2_p;
	DRONE		*drng_p;		// Guy normally guarded
	DRONE		*drngm_p;	// Other opp
	DRONE		*drn2_p;
	int		matehum;		// !0=Teammate is human
	int		onfire;		// -=Opps, 0=none, +=Us

	int		n, i, i2;
	float		f;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#ifdef	DEBUG
	set_text_position (10, 250, 1);
#endif

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	gi_p = &game_info;
	pl_p = drn_p->pl_p;
	pl2_p = drn_p->matepl_p;

	i = drn_p->mate;

#if	NUM_PERTEAM > 2
	if (i >= NUM_PERTEAM) i -= 1;			// pstatus only has 4 bits
#endif

	matehum = 0;

	if (p_status & 1 << i) {	// Teammate is human?

		matehum = 1;
//		printf ("Drn: chk human\n");

//		i = drn_p->cflgs;			// Not currently used
//		drn_p->cflgs = 0;

		i = pl2_p->but_new;		// Read mates buttons

//		printf ("Drn: cmd %d\n", i);

		i2 = 0;

		if ((i & (M_BUTP | M_BUTT)) == M_BUTP) {	// Push?
//		if (i & M_DRNCF_PASS) {

			if (!(pl2_p->but_cur & M_BUTT))	// No turbo?
				i2 = M_BUTP | M_BUTT;
			else {
#ifdef	DEBUG
				printf ("Ignoring turbo pass - human is pushing\n");
				while (!(get_player_sw_current() & ALL_ABCD_MASK)) scan_switches();		// Debug pause game until butn is hit!!!
#endif
			}
		}

		if (i & M_BUTS) {		// Shoot?
//		if (i & M_DRNCF_SHOOT) {
			if (pl2_p->odata.flags & PF_ALLYDUNKER) {

				if(pl2_p->but_new & M_BUTS && pl2_p->odata.y > 0.0f)
					i2 |= M_BUTS | M_BUTT;
			}
			else if (pl_p->odata.flags & (PF_ALLYDUNKER|PF_DUNKER))
			{
#ifdef DEBUG
				printf("I told my drone to layup out of a dunk\n");
#endif
				i2 |= M_BUTS;
			}
			else
			{
//				if (!(pl2_p->but_cur & M_BUTT))
					i2 |= M_BUTS | M_BUTT;
			}
		}

		if (i2) {
//			pl_p->but_old = 0;
			drn_p->but = i2;
			return;
		}

	}


	onfire = drone_firechk (drn_p);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Check mode

	if (drn_p->mode != DRNMD_OFF) {

		drn_p->mode = DRNMD_OFF;
		drn_p->mode2 = DRNMD2_NONE;

//		i = drone_getcurskill (drn_p);
//		i = omdsk_t[i];
//		i = rndrng0 (i) + 2;

		i = 1;
		drn_p->seekcnt = i;
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Check wait mode (spider dribble)

	if (drn_p->mode2 == DRNMD2_OWAIT) {
		drn2_p = drone_closest_opptoball (drn_p);
		if (drn2_p->balldist < DRN_FOOT * 4) {		// Too close?
			drn_p->mode2 = DRNMD2_NONE;
			drn_p->seekcnt = 1;
		}
#ifdef	DEBUG
		printf ("O: Waiting\n");
#endif
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Update seek

	i = drn_p->seekcnt;
	if (i > 0) {
		drn_p->seekcnt = --i;
		if (i <= 0) {

			drn_p->seek_t[0].x = drn_p->ohoopx;
			drn_p->seek_t[0].z = RIM_CZ + rndrng0 (22) - 11;

			drone_findplay (drn_p);
		}
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	drng_p = &drone_t[drn_p->guardnum];
	drngm_p = &drone_t[drn_p->guardmatenum];

	i = pl_p->odata.flags;

	if (i & PF_PASSING) {	// Passing?
kilbuts:
		drn_p->but = 0;
		drn_p->stick = JOY_CENTER;
		return;
	}

	if (pl_p->odata.y > 0) goto injmp;		// In air?

	if (drn_p->but & M_BUTS) goto fake;		// Shoot button down?

	if (i & (PF_SHOOTING | PF_DUNKER | PF_ALLYDUNKER)) goto injmp;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Check passing

	if (!matehum) {

		drn2_p = &drone_t[drn_p->mate];
		f = drn2_p->ohoopdist + DRN_FOOT * 3;

		i = 0;
		if (pl_p->odata.adata[0].animode & MODE_REACTING)	// I'm flailing?
			if (rndrng0 (19) == 0) i = 1;	// 5%

		if (i || f < drn_p->ohoopdist)	// Flail or mate closer?
			if (drone_pass (drn_p)) return;
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Check breakaway

	drone_calcstatus (drng_p);
	drone_calcstatus (drngm_p);

	f = drn_p->ohoopdist;

	if (pl_p->dribble_mode >= 0) {	// Can drib?

//		printf ("HDst %f, %f, %f\n", f, drng_p->myhoopdist, drng_p->myhoopdist);
//inspin:
		if (f >= drng_p->myhoopdist) goto shootrnd;	// No breakaway?
		if (f >= drngm_p->myhoopdist) goto shootrnd;

#ifdef DEBUG
		OPRINTFDB ("%djOFF: breakaway", HJUST_LEFT);
#endif

		drone_seek (drn_p);

		i = 0;
		if (drn_p->skill > DRNSKL_OTURBB) i = M_BUTT;	// Smart enough? Turbo
		drn_p->but = i;

		if (!drone_timeok()) goto shoot3;

		if (drone_chk3ptr (drn_p, 1)) goto shoot3;

		if (f < DRN_FOOT * 4) goto shoot2;	// Close to opp hoop

		if (matehum) return;

		if (f > DRN_FOOT * 13.6f) return;	// Too far?

		if (rndrng0 (11)) return;	// 92% skip

		goto shoot2;
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Can't dribble

#ifdef DEBUG
	OPRINTFDB ("%djOFF: no drib", HJUST_LEFT);
#endif

//	if (inspin) goto inspin;
//	if (elbos) return;

	if (f < DRN_FOOT * 19) goto shoot2;		// Close enough for shot?

	if (!matehum)
		if (drone_pass (drn_p)) return;

	if (rndrng0 (31)) return;	// 97% skip
	goto shoot2;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
shootrnd:

#ifdef DEBUG
	OPRINTFDB ("%djOFF: drib", HJUST_LEFT);
#endif

	if (!drone_timeok()) goto shoot3;

	i = drone_findplay (drn_p);
	if (i) 
		drone_seek (drn_p);
	else {

		i = drone_drvtorim (drn_p);
		if (i == 2) return;				// Pass?

		drone_seek (drn_p);
		if (i >= 3) return;				// Spin or wait?
	}

	i = 0;
	if (drn_p->skill > DRNSKL_OTURB) i = M_BUTT;		// Smart enough? Turbo
	drn_p->but = i;

//	if (drn_p->ohoopdist < DRN_FOOT * 6.4f) goto shoot2;	// Dunk range?

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
tryshot:

	if (f < DRN_FOOT * 4) goto shoot2;	// Dunk range?
	if (matehum) return;

	if (drone_chk3ptr (drn_p, 0)) goto shoot2;

	if (pl2_p->odata.flags & (PF_DUNKER | PF_ALLYDUNKER)) goto pass;

	if (onfire < 0)
		if (f > DRN_FOOT * 9) return;	// Too far?

	if (f > DRN_FOOT * 20) return;	// Too far?

//	Check heatup

	i = 30;
	if (drn_p->skill < DRNSKL_OSLESS) i = 50;	// Dumb? Shoot less
	if (rndrng0 (i)) return;		// Skip shot?

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

shoot2:
	if (pl2_p->odata.flags & (PF_DUNKER | PF_ALLYDUNKER)) {
pass:
		if (f > DRN_FOOT * 22) return;	// Too far?
		drn_p->but = M_BUTP | M_BUTT;
		pl_p->but_old = 0;
#ifdef	DEBUG
		printf ("Drn: trying alley pass\n");
#endif
		return;
	}
shoot3:

	i = M_BUTS;					// Shoot
	if (drn_p->ptsdown >= -4)	// Not winning by much?
		i = M_BUTS | M_BUTT;	// Shoot with turbo

	if (rndrng0 (9) == 0) i ^= M_BUTT;	// Invert 10%

	drn_p->but = i;

	if (f < DRN_FOOT * 15) return;	// Dunk range?

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Stop running if OK

	if (rand() & 1) {		// 50%? Check for close opps

		i = 1 - pl_p->team;
		drn2_p = &drone_t[i * TEAM_MAX];

		n = TEAM_MAX - 1;
		do {
			if (drn2_p->balldist < DRN_FOOT * 8) return;	// Too close?

		} while (drn2_p++, --n >= 0);
	}

	drn_p->stick = JOY_CENTER;		// Stop moving
#ifdef	DEBUG
	printf ("O: Stop move\n");
#endif
	return;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

fake:

#ifdef DEBUG
	OPRINTFDB ("%djOFF: fake", HJUST_LEFT);
#endif

	if (matehum) return;

	if (drng_p->balldist < DRN_FOOT * 6.4f ||
		 drngm_p->balldist < DRN_FOOT * 6.4f) {

		if (rndrng0 (31) == 0) {

			i = drn_p->lowsecagr;
			if (gi_p->game_time < i) return;		// No time for fake?
			if (gi_p->shotclock >= i) {			// Time for fake?
				drn_p->but = 0;
				drn_p->stick = JOY_CENTER;
			}
		}
	}
	return;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

injmp:
	drone_injmp (drn_p, onfire);

}

/********************************
 Check if this drone needs a 3 ptr
 (* drone, mode (0=normal, 1=breakaway)
> !0 = Need 3
********************************/

int	drone_chk3ptr (DRONE *drn_p, int mode)
{
	game_info_t	*gi_p;
	bbplyr_data *pl_p;
	DRONE		*drno_p;

	int		n, i, i2;
	float		f;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	f = drn_p->ohoopdist;

	if (f > RIM_CX * .737f) return (0);		// Too far?

	i = drn_p->mate;

#if	NUM_PERTEAM > 2
	if (i >= NUM_PERTEAM) i -= 1;			// pstatus only has 4 bits
#endif
	if (p_status & 1 << i) return (0);	// Teammate is human?

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Check onfire status

	gi_p = &game_info;
	pl_p = drn_p->pl_p;

	i = drone_firechk (drn_p);
	if (i >= 2) return (1);		// I'm on fire?
	if (i) return (0);			// Someone else on fire?

//	if (gi_p->fire_cnt >= FIRECNT) {
//		i = gi_p->fire_player;
//
//		if (pl_p->plyrnum == i) return (1);		// I'm on fire?
//		return (0);		// Buddy or opps on fire
//	}


	if (!three_pt_check (&pl_p->odata)) return (0);	// Too close?

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Do random

	i = 1 - pl_p->team;
	drno_p = &drone_t[i * TEAM_MAX];

	n = TEAM_MAX;
	do {
		if (drno_p->balldist < DRN_FOOT * 8) break;	// Too close?

	} while (drno_p++, --n > 0);

	if (n)	// Opp close?
		i = 10 + pl_p->_threeshot * 4;		// 1-9%
	else
		i = 50 + pl_p->_threeshot * 10;		// 5-25%

	n = 6;
	i2 = gi_p->game_time;		// 0xMMSSTT

	if (i2 <= 0x10a00) n = 3;		// Less than 1 min 10 sec?
	if (drn_p->ptsdown >= n) {

		if (i2 < 10 << 8) i *= 5;			// Less than 10 secs?
		else if (i2 < 40 << 8) i *= 3;	// Less than 40 secs?
		else i *= 2;
	}

	if (mode) i = i >> 2;	// Breakaway? % / 4

	if (drn_p->skill < DRNSKL_O3LESS) i = i >> 1;	// Dumb? % / 2

#ifdef DEBUG
	printf ("Chk 3: %d%%  3shot=%d\n", i, pl_p->_threeshot);
#endif
	if (rndrng0 (999) >= i) return (0);	// No shot?

	return (1);
}


/********************************
 Replace with rob's
********************************/

#if	0
int	three_pt_check (obj_3d *o_p)
{
	bbplyr_data *pl_p;
	DRONE		*drn_p;
	float		f;

	pl_p = (PV) o_p;
	drn_p = drone_t + pl_p->plyrnum;
	f = drn_p->ohoopdist;

	if (f < 77) return (0);			// Too close?

	return (1);
}
#endif

/********************************
 Drive to rim avoiding opponents
> !0 = Avoiding
********************************/

#define	_N	22	//20
int	drvsp_t[] = {		// % to spin
	1*_N,2*_N,3*_N,4*_N,5*_N,
	6*_N,7*_N,8*_N,9*_N,10*_N,
	12*_N,14*_N,16*_N,17*_N,18*_N,
	20*_N,
	22*_N,24*_N,26*_N,28*_N,29*_N,
	30*_N,32*_N,34*_N,36*_N,38*_N,
	40*_N,50*_N,60*_N,80*_N,100*_N
};
#undef	_N

int	drone_drvtorim (DRONE *drn_p)
{
	bbplyr_data *pl_p;
	bbplyr_data *plo_p;
	DRONE		*drn2_p;		// Teammate
	DRONE		*drno_p;		// Defender
	int		dirh;			// Dir to hoop
	int		diro;			// Opponent dir
	float		f_t[3];

	int		n, i, i2;
	float		f, f2, *f_p, fz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#ifdef	DEBUG_DRNDRV
	set_text_position (10, 200, 1);
	oprintf ("%djZig %d", HJUST_LEFT, drn_p->seekzmd);
#endif

	if (drn_p->seekcnt > 1) return (-1);

	pl_p = drn_p->pl_p;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Find closest opp

	drno_p = drone_closest_opptoball (drn_p);

//	i = 1 - pl_p->team;
//	drn2_p = &drone_t[i * TEAM_MAX];
//	f = drn2_p->balldist;
//	drno_p = drn2_p;
//	n = TEAM_MAX - 1;
//
//	do {
//		drn2_p++;
//		if (drn2_p->balldist < f) {
//			f = drn2_p->balldist;
//			drno_p = drn2_p;
//		}
//	} while (--n > 0);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	drn2_p = &drone_t[drn_p->mate];

//	i = 1 - pl_p->team;
//	drno_p = &drone_t[i * TEAM_MAX];
//
//	n = TEAM_MAX - 1;
//	do {

		if (drno_p->balldist < DRN_FOOT * 10.0f) {

			plo_p = drno_p->pl_p;

			f_p = &pl_p->odata.x;
			f_t[0] = drn_p->ohoopx;
			f_t[2] = RIM_CZ;

			dirh = pt_angledist (f_p, f_t, &f);
			diro = pt_angledist (f_p, &plo_p->odata.x, &f);

			i2 = diro - dirh;
			i = i2 & 0x3ff;
			if (i > 512) i = i - 1024;		// Dir difference (+/-512)


			if (i > -128 && i < 128 && f < DRN_FOOT * 8.5f) {	// Defense in my way?

#ifdef	DEBUG_DRNDRV
				printf ("Drv dist %f, Dir: H%d, O%d, diff %d\n", f, dirh, diro, i);
#endif

				if (f > DRN_FOOT * 5.5f) {	// Defense far enough?
#ifdef	DEBUG_DRNDRV
					printf ("O: Wait?\n");
#endif
					if (drn_p->ohoopdist < DRN_FOOT * 28) {
						if (rndrng0 (2) == 0) {
							drn_p->mode2 = DRNMD2_OWAIT;
							drn_p->seek_t[0].x = pl_p->odata.x;		// Don't seek
							drn_p->seek_t[0].z = pl_p->odata.z;
							drn_p->seekcnt = 30 + rndrng0 (tsec * 2);
							pl_p->odata.tgt_fwd = dirh;	// Face hoop
#ifdef	DEBUG_DRNDRV
							printf ("O: Wait\n");
#endif
							return (4);
						}
					}
				}

				f2 = DRN_FOOT * 4;
				if (rndrng0 (9) == 0)	// 10%? Do early spin
					f2 = DRN_FOOT * 10;
//				if (1) {
				if (i > -100 && i < 100 && f < f2) {	// In front & close

					i2 = drone_getcurskill (drn_p);
					i2 = drvsp_t[i2];

					if (i2 > rndrng0 (999)) {	// Spin?
						drn_p->but = M_BUTT;	// Try spin
						pl_p->but_old = 0;
//						pl_p->non_turbo_time = -1;
						pl_p->non_turbo_time = 1;
						drn_p->seekcnt = 4;
#ifdef	DEBUG_DRNDRV
						printf ("Spin %d turbo %f\n", i, pdata_blocks[pl_p->plyrnum].turbo);
#endif
						return (3);
					}
				}

				i2 = drn2_p->balldist;
				if (i2 > DRN_FOOT * 7.0f) {
					if (drone_pass (drn_p)) return (2);
				}

				if (rndrng0 (2) == 0) {		// 33%
					i2 = rndrng0 (100) + 200;
					if (rndrng0 (2) == 0) i2 = -i2;
//					i2 = fabs (i2);	// DEBUG
					drn_p->seekzmd = i2;
				}

				i2 = drn_p->seekzmd;	// Angle to go around

				if (drn_p->ohoopdist < DRN_FOOT * 6) {
					if (f < DRN_FOOT * 4) i2 = i2 * 7 / 4;		// Back away
				}
				else {
					if (f > DRN_FOOT * 6) i2 >>= 1;
					if (f < DRN_FOOT * 2.5f) i2 = i2 * 3 / 2;	// Angle back
				}

#ifdef	DEBUG_DRNDRV
				printf ("Drv zmd %d\n", i2);
				set_text_position (100, 200, 1);
				oprintf ("%djDrv %d", HJUST_LEFT, i2);
#endif

				if (i < 0) i2 = -i2;
				i = diro - i2;
				drn_p->seek_t[0].x = f_p[0] + isin (i) * DRN_FOOT * 10;
				fz = f_p[2] + icos (i) * DRN_FOOT * 10;
				if (fz > OUT_OF_BOUNDS_Z - OOB_RADIUS) fz = 0;
				if (fz < -OUT_OF_BOUNDS_Z + OOB_RADIUS) fz = 0;
				drn_p->seek_t[0].z = fz;
				drn_p->seekcnt = 20;
				return (1);
			}
		}

//	} while (drno_p++, --n >= 0);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	return (0);
}


/********************************
 Drone code - pass if clear
> !0 = pass OK
********************************/

#define	_T	9		// Ticks ahead to look

int	drone_pass (DRONE *drn_p)
{
	bbplyr_data *pl_p;
	bbplyr_data *pl2_p;
	bbplyr_data *plo_p;
	DRONE		*drn2_p;		// Teammate
	DRONE		*drno_p;		// Defender

	int		tmdir;		// Mate dir
	float		tmdist;		// Mate distance
	int		odir;			// Opponent dir
	VEC		vc;
	VEC		vc2;
	int		ptime;		// Pass time

	int		n, i, i2;
	float		f, f2, *f_p, fx, fz, fx2, fz2;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#ifdef	DEBUG_DRNINFO
//	set_text_position (10, 200, 1);
//	OPRINTFDB ("%djZig %d", HJUST_LEFT, drn_p->seekzmd);
#endif
	if (pcount & 1) return (0);	// Skip 50%

	if (!drone_timeok()) return (0);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Mate is human

	i = drn_p->mate;

#if	NUM_PERTEAM > 2
	if (i >= NUM_PERTEAM) i -= 1;			// pstatus only has 4 bits
#endif
	if (p_status & 1 << i) return (0);	// Teammate is human?

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	pl_p = drn_p->pl_p;
	pl2_p = drn_p->matepl_p;
	drn2_p = &drone_t[drn_p->mate];

	if (pl2_p->odata.adata[0].animode & MODE_REACTING) {	// Mate in flail?
		return (0);	// Skip
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

//	tmdir = p2p_angles[pl_p->plyrnum][pl2_p->plyrnum];

	vc.x = pl_p->odata.x + pl_p->odata.vx * _T;	// Calc pos when pass starts
	vc.z = pl_p->odata.z + pl_p->odata.vz * _T;
	vc2.x = pl2_p->odata.x + pl2_p->odata.vx * _T;
	vc2.z = pl2_p->odata.z + pl2_p->odata.vz * _T;
	tmdir = pt_angledist (&vc, &vc2, &tmdist);

	f = MAX_PASS_TIME * (tmdist / MAX_PASS_DIST);
	ptime = GREATER (f, MIN_PASS_TIME);

//	plyr_will_be_at (&i, &fx, &fz, ptime + _T, drn_p->mate);
	f = ptime;
	vc2.x += pl2_p->odata.vx * f;		// Receiver pos at end of pass
	vc2.z += pl2_p->odata.vz * f;

	tmdir = pt_angledist (&vc, &vc2, &tmdist);
	if (tmdist < DRN_FOOT * 2.5f) return (0);	// Rcvr way too close?

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#ifndef	DEBUG_DRNSAFEPASS
	if (rndrng0 (99) == 0) {
#ifdef	DEBUG_DRNPASS
		printf ("Pass: rnd\n");
#endif
		goto pass;
	}
#endif

	i = 1 - pl_p->team;
	drno_p = &drone_t[i * TEAM_MAX];

#ifdef	DEBUG_DRNPASS
//		printf ("Pass: tmdir %d tmdist %f time %d\n", tmdir, tmdist, ptime);
#endif

	n = TEAM_MAX - 1;
	do {

		plo_p = drno_p->pl_p;

		vc2.x = plo_p->odata.x + plo_p->odata.vx * _T;	// Opp pos at pass start
		vc2.z = plo_p->odata.z + plo_p->odata.vz * _T;
		fx = vc2.x - vc.x;
		fz = vc2.z - vc.z;
		f = fsqrt (fx * fx + fz * fz);

		f = f - tmdist;
		if (f > DRN_FOOT * 4) {	// Opp behind rcvr?
			continue;
		}

		f  = icos (tmdir);		// Transform opp pos to pass coords
		f2 = isin (tmdir);
		fx2 = fx * f - fz * f2;
		fz2 = fx * f2 + fz * f;

		if (fz2 <= 0) continue;				// Opp behind me?
		f = ptime * fz2 / tmdist;			// Time available

		if (plo_p->odata.adata[0].animode & MODE_REACTING)
			f -= 10;

		if (plo_p->odata.y > 0) {	// Opp in air?
			f -= 10;
			if (plo_p->odata.vy > 0)	// Going up?
				f -= 40;
		}
#ifdef	DEBUG_DRNPASS
//		printf (" XZ %f %f op time %f\n", fx2, fz2, f);
#endif

		f = f * plo_p->p_spd->speed + 4.0f;	// X range intercept is possible

		if (fabs (fx2) < f) return (0);	// Too close to pass?

	} while (drno_p++, --n >= 0);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

pass:
//	halt = 1;

	pl_p->but_old = 0;

	i = M_BUTP;					// Pass
	if (drn_p->ptsdown >= -3)		// Not winning by much?
		i = M_BUTP | M_BUTT;			// Pass with turbo

	if (rndrng0 (9) == 0) i ^= M_BUTT;	// Invert 10%
	drn_p->but = i;

#ifdef	DEBUG_DRNPASS
	printf (" Pass!\n");
#endif

	return (1);
}


/********************************
 Offense in a jump
********************************/

void	drone_injmp (DRONE *drn_p, int onfire)
{
	bbplyr_data *pl_p;
	bbplyr_data *plo_p;
	DRONE		*drn2_p;		// Teammate
	DRONE		*drno_p;		// Defender
	int		wait;			// !0 = Waiting on opp
	int		hdir;			// Dir to hoop
	float		hdist;
	int		odir;			// Opponent dir
	float		f_t[3];

	int		n, i, i2;
	float		f, *f_p, fx, fy, fz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#ifdef DEBUG
	OPRINTFDB ("%djOFF: jump", HJUST_LEFT);
#endif

	pl_p = drn_p->pl_p;
	drn2_p = &drone_t[drn_p->mate];

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	if (drn2_p->pl_p->odata.flags & (PF_DUNKER | PF_ALLYDUNKER)) {	// Mate dunking?
		drn_p->but = M_BUTP | M_BUTT;
		return;
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	if (pl_p->odata.flags & (PF_DUNKER | PF_ALLYDUNKER)) {
		if (pl_p->flight_time > pl_p->total_time * 3 / 4) return;	// Almost done? Skip
	}

	f_p = &pl_p->odata.x;
	f_t[0] = drn_p->ohoopx;
	f_t[2] = RIM_CZ;
	hdir = pt_angledist (f_p, f_t, &hdist);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	i = 1 - pl_p->team;
	drno_p = &drone_t[i * TEAM_MAX];

	wait = 0;

	n = TEAM_MAX - 1;
	do {

		plo_p = drno_p->pl_p;

		if (drno_p->balldist < DRN_FOOT * 10.0f) {

			odir = pt_angledist (f_p, &plo_p->odata.x, &f);

			i2 = odir - hdir;
			i = i2 & 0x3ff;
			if (i > 512) i = i - 1024;		// Dir difference (+/-512)

#ifdef	DEBUG_DRNJMP
			printf ("Jmp= dist %f, Dir: H%d, O%d, diff %d\n", f, hdir, odir, i);
#endif

			if (i > -128 && i < 128 && f < DRN_FOOT * 5) {	// Defense in my way?

				fy = pl_p->odata.y;
				if (fy < DRN_FOOT * .5f) return;		// I'm just off gnd?

				fy += pl_p->p_spd->tall * DRN_INCH;

				f = plo_p->odata.y;
				f += plo_p->p_spd->tall * DRN_INCH;

				if (fy < f) {		// I'm lower?

					i2 = drn2_p->balldist;
					if (i2 > DRN_FOOT * 1.0f) {
						if (drone_pass (drn_p)) return;
					}
					if (pl_p->odata.vy > 0)
						if (plo_p->odata.vy < 0) wait = 1;
				}
			}
		}

	} while (drno_p++, --n >= 0);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	if (wait || pl_p->odata.vy > .5f) {	// Waiting or on the way up?

		if (drone_timeok()) return;	// Time to wait?
	}

	drn_p->but = 0;	// Release shot

#ifdef	DEBUG_DRNJMP
	printf (" Release!\n");
#endif
}



/********************************
 Find a play and run
 NOT USED CURRENTLY! Maybe next version
********************************/

typedef	struct	_PLAYDRN {	// Drone state data for play
	DRONE		*drn_p;
	VEC		vc;
	int		dir;
	float		dist;
} PLAYDRN;

typedef	struct	_PLAY {
	int		id;
	int		dir;
	float		dist;
	float		distmax;
} PLAY;

#define	PLAYID_TM	0
#define	PLAYID_OP	1
#define	PLAYID_SOL	2

PLAY	play_t[] = {
	{ PLAYID_OP, 0, 5, 10 },
	{ PLAYID_SOL, 1, 20 },
	{ PLAYID_OP, 0, 0, 5 },
	{ PLAYID_SOL, 4, 20 },
	{ PLAYID_SOL, 0, 20 },	// Straight
	{ -1 }
};

#undef	_T
#define	_T	3		// Ticks ahead to look

int	drone_findplay (DRONE *drn_p)
{
	bbplyr_data *pl_p;
	bbplyr_data *pl2_p;
	bbplyr_data *plo_p;
	DRONE		*drn2_p;		// Teammate
	DRONE		*drno_p;		// Defender
	VEC		vc;
	VEC		vc2;
	int		hdir;
	float		hdist;
	int		team;			// Offensive team

	int		tmcnt;
	PLAYDRN	tm_t[TEAM_MAX - 1];
	PLAYDRN	*tm_tp[TEAM_MAX - 1];
	PLAYDRN	*tm_p;
	int		opcnt;
	PLAYDRN	op_t[TEAM_MAX];
	PLAYDRN	*op_tp[TEAM_MAX];
	PLAYDRN	*op_p;
	PLAYDRN	*pd_p;		// Temp

	PLAY		*ply_p;
	int		flgs;

	int		tmstat;
	int		opstat;

	int		n, i, i2;
	float		f, f2, fx, fy, fz, fx2, fz2;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ DEBUG

#ifndef	DEBUG_DRNPLAY
	return (0);
#endif

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	pl_p = drn_p->pl_p;
	pl2_p = drn_p->matepl_p;
	drn2_p = &drone_t[drn_p->mate];

	if (pl_p->odata.adata[0].animode & MODE_REACTING) return (0);

//	if (pl2_p->odata.adata[0].animode & MODE_REACTING) return (0);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

//	tmdir = p2p_angles[pl_p->plyrnum][pl2_p->plyrnum];

	vc.x = pl_p->odata.x + pl_p->odata.vx * _T;	// Calc pos when play starts
	vc.z = pl_p->odata.z + pl_p->odata.vz * _T;
	vc2.x = drn_p->ohoopx;
	vc2.z = RIM_CZ;
	hdir = pt_angledist (&vc, &vc2, &hdist);

#ifdef	DEBUG_DRNPLAY
	printf ("Play: hdir %d hdist %f\n", hdir, hdist);
#endif

	team = pl_p->team;

	drn2_p = drone_t;
	tm_p = tm_t;
	op_p = op_t;

	n = 0;

	do {

		if (n == pl_p->plyrnum) continue;		// Me?

		pl2_p = drn2_p->pl_p;

		if (pl2_p->odata.adata[0].animode & MODE_REACTING)
			continue;

		if (pl2_p->odata.y > 0) {	// Opp in air?
			continue;
		}

		vc2.x = pl2_p->odata.x + pl2_p->odata.vx * _T;	// Pos at play start
		vc2.z = pl2_p->odata.z + pl2_p->odata.vz * _T;
		fx = vc2.x - vc.x;
		fz = vc2.z - vc.z;
		fy = fsqrt (fx * fx + fz * fz);

		i = hdir + 512;
		f  = icos (i);		// Transform pos to play coords
		f2 = isin (i);
		fx2 = fx * f - fz * f2;
		fz2 = fx * f2 + fz * f;

		if (pl2_p->team == team) {
			pd_p = tm_p++;
		}
		else {
			pd_p = op_p++;
		}
		pd_p->drn_p = drn2_p;	// Save info
		pd_p->vc.x = fx2;
		pd_p->vc.z = fz2;
		pd_p->dist = fy;

		i = pt_dir8 (0, 0, fx2, fz2);
		pd_p->dir = i;

#ifdef	DEBUG_DRNPLAY
		printf (" XZ %f %f dir %d dist %f\n", fx2, fz2, i, fy);
#endif

//		f = f * plo_p->p_spd->speed + 4.0f;	// X range intercept is possible
//		if (fabs (fx2) < f) return (0);	// Too close to pass?

	} while (drn2_p++, ++n < PLYR_MAX);

	tmcnt = tm_p - tm_t;
	opcnt = op_p - op_t;

#ifdef	DEBUG_DRNPLAY
	printf (" tmcnt %d opcnt %d\n", tmcnt, opcnt);
#endif

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	ply_p = play_t;
	flgs = 0;
	tmstat = 0;
	opstat = 0;
	op_tp[0] = 0;

	do {
		i = ply_p->id;

		if (i != PLAYID_SOL) {

			switch (i) {

				case PLAYID_TM:
					break;

				case PLAYID_OP:

					opstat = -1;

					n = opcnt;
					pd_p = op_t;

					while (n-- > 0) {

						i = ply_p->dir;
						if (i < 0 || pd_p->dir == i)
							f = pd_p->dist;
							if (f >= ply_p->dist && f <= ply_p->distmax) {

								i2 = 1 << n;
								if ((flgs & i2) == 0) {
									flgs |= i2;
									opstat = 0;
									op_tp[0] = pd_p;
									break;
								}
							}
						pd_p++;
					}

					break;
			}
		}
		else {

			if (flgs) {
				i = ply_p->dir * (-1024 / 8) + hdir;
				f = ply_p->dist * DRN_FOOT;

				fx = isin (hdir) * f;
				fz = icos (hdir) * f;

				drn_p->seekcnt = 10;
				drn_p->seek_t[0].x = pl_p->odata.x + fx;
				drn_p->seek_t[0].z = pl_p->odata.z + fz;

#ifdef	DEBUG_DRNPLAY
				printf (" set %x dir %d\n", flgs, ply_p->dir);
#endif
				return (1);
				break;
			}
		}

	} while (ply_p++, ply_p->id >= 0);


	return (0);
}



/********************************
 Get dir from source pt to dest pt
 EAX = Dest X
 EBX = Dest Z
 ECX = Src X
 EDX = Src Z
> (0-255)*256 (8:8)
********************************/

#define	OCT	(16 / 8)

int	pt_dir8 (float sx, float sz, float dx, float dz)
{
	int		dir;
	float		acc;

	float		x, z;
	float		f;

//	float		sx, sz;
//	sx = 0;
//	sz = 0;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	dir = 0;					//	Octant 0-1

	x = dx - sx;			// DestX-SrcX
	if (x >= 0) {			// Oct 0-3

		z = dz - sz;		// DestZ-SrcZ
		z = -z;

		if (z >= 0) goto o0145;

		dir = OCT * 2;		// Oct 2-3
	}
	else {
		x = -x;

		dir = OCT * 4;		// Oct 4-5
		z = dz - sz;		// DestZ-SrcZ

		if (z >= 0) goto o0145;
		dir = OCT * 6;		// Oct 6-7
	}
	z = -z;
	f = x;					// Swap
	x = z;
	z = f;

o0145:
	acc = 0;

	if (x < z) {					// Cmp slope

		z = z * (1.0f / OCT);	// Bigger / OCT
		if (z != 0) goto zstrt;	// OK?
		goto x;

		do {
			dir++;					// Next 1/OCT oct
			acc += z;				// +1/OCT
zstrt:
		} while (acc < x);
	}

	else {

		dir += OCT * 2 - 1;		// End of next octant

		x = x * (1.0f / OCT);	// Bigger / OCT
		if (x != 0) goto xstrt;	// OK?
		goto x;

		do {
			dir--;					// Next 1/OCT oct
			acc += x;				// +1/OCT
xstrt:
		} while (acc < z);

	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
x:
	return ((dir + 1) >> 1);
}



/********************************
 Load play file
********************************/

void	drone_playload()
{
	char		*err_p;
	FILE		*p_fp;

	long		cnt;
	PLAY		play;
	PLAY		*ply_p;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#ifndef	DEBUG_DRNPLAY
	return;
#endif

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	p_fp = fopen ("drnplay.dat", "rb");
	if (!p_fp) {

		err_p = "open file";
		goto err;
	}


	err_p = "read file";

	if (fread (&cnt, 4, 1, p_fp) != 1) goto err;

	while (--cnt >= 0) {

		ply_p = JMALLOC (sizeof(PLAY));
		if (fread (&cnt, sizeof(PLAY), 1, p_fp) != 1) goto err;
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	err_p = 0;
err:
	if (err_p) {
#ifdef DEBUG
		fprintf (stderr, "Error: Drone play %s\n", err_p);
#endif
	}

	fclose (p_fp);
}

/********************************
 Offense without ball
********************************/

int	owmdsk_t[] = {	// Mode switch max seek time
	50,45,40,35,30,	// Up 15-11
	25,20,16,14,12,	// 10-6
	10,9,8,7,6,			// 5-1
	6,						// Even score
	6,5,5,5,4,			// Dn 1-5
	4,4,3,3,3,			// 6-10
	2,2,2,1,1			// 11-15
};
#define	_N	.5f
int	owps_t[] = {		// % to push
	1*_N,2*_N,3*_N,4*_N,5*_N,
	6*_N,7*_N,8*_N,9*_N,10*_N,
	12*_N,14*_N,16*_N,17*_N,18*_N,
	20*_N,
	24*_N,26*_N,28*_N,30*_N,31*_N,
	33*_N,35*_N,39*_N,40*_N,42*_N,
	45*_N,60*_N,80*_N,100*_N,150*_N
};
#undef	_N

void	drone_offwoball (DRONE *drn_p)
{
	bbplyr_data *pl_p;
	bbplyr_data *plb_p;
	DRONE		*drnb_p;		// Teammate
	DRONE		*drno_p;		// Defender

	int		skill;

	int		dirsk;		// Dir to seek pos
	int		dirb;			// Dir to ball handler
	VEC		vc;

	int		n, i, i2, i3;
	float		f, *f_p, fx, fz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	pl_p = drn_p->pl_p;

	drnb_p = 0;
	plb_p = 0;

	i = game_info.ball_handler;		// -1 if passing
	if (i >= 0) {					// Someone has ball?
		drnb_p = &drone_t[i];
		plb_p = drnb_p->pl_p;
	}

	drn_p->cflgs = 0;
	drn_p->but = 0;

	skill = drone_getcurskill (drn_p);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Check mode

	if (drn_p->mode != DRNMD_OFFWOB) {

		drn_p->mode = DRNMD_OFFWOB;
		drn_p->mode2 = DRNMD2_NONE;

		i = owmdsk_t[skill];		// Mode delay
		i = rndrng0 (i) + 1;

		drn_p->seekcnt = i;
		drn_p->seek_t[1].x = 0;
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	i = drn_p->seekcnt;
	if (1) {
//	if (i > 0) {
		--i;

		if (i <= 0) {

			if (drn_p->seek_t[1].x == 0) {
				f = (rndrng0 (70) + 30) * .01f;
				drn_p->seek_t[1].x = drn_p->ohoopx * f;
				drn_p->seek_t[1].z = RIM_CZ + (rndrng0 (200) - 100) * .01f * (COURT_W - DRN_FOOT * 2);
			}

			drn_p->seek_t[0].x = drn_p->seek_t[1].x;
			drn_p->seek_t[0].z = drn_p->seek_t[1].z;

			i = rndrng0 (TSEC * 2) + TSEC;
		}

		drn_p->seekcnt = i;
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Avoid ball handler

	if (i > 15) {

		if (drnb_p && drnb_p->balldist < DRN_FOOT * 8) {

			f_p = &pl_p->odata.x;
			vc.x = drn_p->seek_t[0].x;
			vc.z = drn_p->seek_t[0].z;

			dirsk = pt_angledist (f_p, &vc, &f);
			dirb = pt_angledist (f_p, &plb_p->odata.x, &f);

			i2 = dirb - dirsk;
			i = i2 & 0x3ff;
			if (i > 512) i = i - 1024;		// Dir difference (+/-512)

			if (i > -100 && i < 100 && f < DRN_FOOT * 7) {	// Mate in my way?

#ifdef	DEBUG_DRNOWOB
				printf ("dist %f, Dir: sk%d, b%d, diff %d\n", f, dirsk, dirb, i);
#endif

				if (rand() & 3) {		// 75%? Temp avoid seek
					i2 = 150 + rndrng0 (80);	// Angle away

//					if (f > DRN_FOOT * 6) i2 >>= 1;
					if (f < DRN_FOOT * 2) i2 = i2 * 3 / 2;

#ifdef	DEBUG_DRNOWOB
					printf ("Avoid %d\n", i2);
					set_text_position (100, 180, 1);
					oprintf ("%djAvoid %d", HJUST_LEFT, i2);
#endif
					if (i < 0) i2 = -i2;
					i = dirb - i2;
					drn_p->seek_t[0].x = f_p[0] + isin (i) * DRN_FOOT * 10;
					fz = f_p[2] + icos (i) * DRN_FOOT * 10;
					if (fz > OUT_OF_BOUNDS_Z - OOB_RADIUS) fz = RIM_CZ;
					if (fz < -OUT_OF_BOUNDS_Z + OOB_RADIUS) fz = RIM_CZ;
					drn_p->seek_t[0].z = fz;
					drn_p->seekcnt = 20;
				}
				else {	// New seek
					drn_p->seekcnt = 1;
					drn_p->seek_t[1].x = 0;
				}
			}
		}
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Alley-oop

	if (drn_p->mode2 != DRNMD2_ALLY) {

		i = drn_p->ohoopdist;

		i2 = 17;	//20
		if (drn_p->skill < DRNSKL_OALLY) i2 = 80;	// Dumb? Lower %	//100

		i3 = drone_firechk (drn_p);
		if (i3 < 0) i2 = 1;	// Opp on fire?
#ifdef	DEBUG
//		i2 = 0;
#endif
		if (i > DRN_FOOT * 9 && i < DRN_FOOT * 16 && !rndrng0 (i2)) {

			if (drnb_p && drnb_p->ohoopdist < DRN_FOOT * 25) {

				drn_p->mode2 = DRNMD2_ALLY;		// Setup mode

				drn_p->seek_t[0].x = drn_p->ohoopx;
				drn_p->seek_t[0].z = RIM_CZ;
				drn_p->seekcnt = 15;
			}
		}
	}
	else {
//	if (drn_p->mode2 == DRNMD2_ALLY) {
		if (drn_p->seekcnt == 1) {
			drn_p->mode2 = DRNMD2_NONE;
			drn_p->but = M_BUTS | M_BUTT;	// Shoot & turbo
			pl_p->but_old = 0;
		}
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#ifdef	DEBUG_DRNOWOB
	set_text_position (10, 180, 1);
	oprintf ("%djOWOB sk%d", HJUST_LEFT, drn_p->seekcnt);
#endif

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Push

	if (drn_p->skill < DRNSKL_PUSH)	// Dumb?
		if (pushed_cnt[pl_p->plyrnum] == 0) goto skipps;		// I've never been pushed?

	i = owps_t[skill];
//	i = 100;

	if (i > rndrng0 (999)) {	// Push?

		drone_closest_opptopush (drn_p, 5);

		if (drn_p->odist < DRN_FOOT * 3.5f) {

			drn_p->but = M_BUTP | M_BUTT;

#ifdef	DEBUG_DRNOWOB
		printf ("OWOB: push %f\n", drn_p->odist);
#endif
		}
	}
skipps:

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Seek

	i = drone_seek (drn_p);

	if (i == JOY_CENTER) {
		if ((pcount & 0xf) == 0) {

			if (rand() & 1) {		// 50%?
				drn_p->seekcnt = 1;
				drn_p->seek_t[1].x = 0;
			}
		}
	}
	else {
		if (drn_p->collcnt >= COLLRESEEKCNT) {	// Too many? New seek
			drn_p->seekcnt = 1;
			drn_p->seek_t[1].x = 0;
#ifdef	DEBUG_DRNOWOB
			printf ("OWOB: collide\n");
#endif
		}
	}

}


/********************************
 Defense
********************************/

int	dmdsk_t[] = {	// Mode switch max seek time
	50,50,50,50,50,	// Up 15-11
	50,50,45,45,40,	// 10-6
	33,25,22,18,16,	// 5-1
	14,					// Even score
	10,8,6,4,4,			// Dn 1-5
	4,3,3,3,2,			// 6-10
	2,2,2,2,1			// 11-15
};

#define	_N	0
int	dnast_t[] = {	// % to become nasty
	0,0,0,0,0,
	_N+1,_N+1,_N+2,_N+2,_N+2,
	_N+3,_N+3,_N+3,_N+4,_N+5,
	_N+6,
	_N+7,_N+8,_N+9,_N+10,_N+12,
	_N+14,_N+17,_N+19,_N+22,_N+24,
	_N+26,_N+30,_N+34,_N+38,_N+40
};
#undef	_N

#define	_N	28		//25
int	dskt_t[] = {		// Max seek time (75% avrg)
	_N+55,_N+55,_N+55,_N+55,_N+52,
	_N+48,_N+44,_N+40,_N+36,_N+33,
	_N+30,_N+29,_N+28,_N+27,_N+26,
	_N+25,					// Even score
	_N+24,_N+23,_N+21,_N+17,_N+14,
	_N+11,_N+8,_N+ 6,_N+ 3,_N+ 0,
	_N- 1,_N- 2,_N- 3,_N- 5,_N-10
};
#undef	_N

#define	_N	.4f
float	ddist_t[] = {	// Max dist to try push/steal
	110*_N, 100*_N, 100*_N, 100*_N, 100*_N,
	90*_N, 90*_N, 80*_N, 80*_N, 80*_N,
	80*_N, 70*_N, 70*_N, 60*_N, 60*_N,
	60*_N,
	50*_N, 50*_N, 50*_N, 50*_N, 50*_N,
	50*_N, 50*_N, 50*_N, 50*_N, 50*_N,
	50*_N, 50*_N, 50*_N, 50*_N, 50*_N
};
#undef	_N

#define	_N	.22f //.25f	// .3
int	dps_t[] = {		// % to push/steal
	1*_N,2*_N,2*_N,2*_N,2*_N,
	3*_N,3*_N,3*_N,3*_N,3*_N,
	4*_N,4*_N,4*_N,5*_N,6*_N,
	6*_N,
	7*_N,7*_N,8*_N,9*_N,12*_N,
	14*_N,15*_N,16*_N,19*_N,21*_N,
	28*_N,30*_N,40*_N,60*_N,80*_N
};
#undef	_N

int	dblk_t[] = {		// % to block
	1,1,1,2,2,
	3,3,3,3,4,
	4,4,5,6,7,
	9,
	13,14,16,18,22,
	25,28,32,38,40,
	45,50,50,50,50
};

void	drone_defense (DRONE *drn_p)
{
	game_info_t	*gi_p;
	bbplyr_data *pl_p;
	bbplyr_data *pl2_p;
	bbplyr_data *plg_p;
	bbplyr_data *plb_p;
	DRONE		*drn2_p;		// * my buddy
	DRONE		*drng_p;		// * opponent we guard
	DRONE		*drnb_p;		// * opponent with ball
	int		skill;
	int		onfire;		// -=Opps, 0=none, +=Us
	int		skcnt;		// Seek

	int		i, i2, i3;
	float		f, f2, fx, fx2, fz, fz2;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	gi_p = &game_info;
	pl_p = drn_p->pl_p;

	pl2_p = drn_p->matepl_p;
	drn2_p = &drone_t[drn_p->mate];

	drng_p = &drone_t[drn_p->guardnum];

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	drnb_p = drng_p;
	i = gi_p->ball_handler;
	if (i >= 0)
		drnb_p = &drone_t[i];

//	if (drnb_p == 0) {
//		return;
//	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	drn_p->cflgs = 0;

	skill = drone_getcurskill (drn_p);

	onfire = drone_firechk (drn_p);
	if (onfire < 0) {	// Other team?
		skill += 10;
		if (skill > 30) skill = 30;
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Mode

	if (drn_p->mode != DRNMD_DEF) {

		drn_p->mode = DRNMD_DEF;

		if (drn_p->mode2 == DRNMD2_INBND) {
			i = tsec - 5;
			drn_p->seekcnt = 5 + rndrng0 (i) + rndrng0 (i);	// Bell curve average
		}
		else {
			i = dmdsk_t[skill];
			i = rndrng0 (i) + 1 + i / 4;
			drn_p->seekcnt = i;
		}

		drn_p->mode2 = DRNMD2_NONE;

		drn_p->nastycnt = 0;

		if (rndrng0 (2))
			f = (3 + rndrng0 (4)) * DRN_FOOT;	// From opp
		else
			f = (14 + rndrng0 (7)) * -DRN_FOOT;	// From hoop

		drn_p->guarddist = f;

//		drn_p->guarddist = rndrng0 (30) + 170;
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	if (drn_p->skill > DRNSKL_NASTYLS) {	// Smart enough?

		if (rndrng0 (60) == 0) {

			i = gi_p->game_time;
			i2 = drn_p->lowsecagr;
			i3 = 40;

			if (i < i2) goto nasty;		// Low time in quarter?

			if (gi_p->game_quarter >= 3) {
				if (drn_p->ptsdown > -3)
					if (i < i2 * 5) goto nasty;
			}
			if (gi_p->shotclock < i2 / 2) goto nasty;
		}
	}
	i3 = drn_p->nastycnt;

	if (i3 <= 0) {

		i3 = 10;
		if ((rand() & 0xff) == 0) goto nasty;
		i3 = 0;

		if (drn_p->skill > DRNSKL_NASTYRND) {	// Smart enough?
			i = rndrng0 (999);
			if (i >= dnast_t[skill]) goto nonast;
			i3 = rndrng0 (TSEC-20) + 20;
		}
	}
	i3--;
nasty:
	drn_p->nastycnt = i3;
nonast:

	if (i3 == 10) goto newsk;	// Seek again

//	if (drn_p->nastycnt > 0) printf ("#%d N %d\n", pl_p->plyrnum, drn_p->nastycnt);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	skcnt = drn_p->seekcnt - 1;
	if (skcnt <= 0) {
newsk:
		if (drn_p->guardnum == gi_p->ball_handler) goto guard;

		if (drnb_p->ohoopdist > RIM_CX * .99f) goto guard;	// Too far too worry?

//		drone_calcstatus (drn_p);
		if (drone_defensechk (drn2_p, drnb_p))	// Failed?
			drng_p = drnb_p;							// Guard ball carrier
guard:

		plg_p = drng_p->pl_p;
		fx = plg_p->odata.x + plg_p->odata.vx * 16;
		fz = plg_p->odata.z + plg_p->odata.vz * 16;
		fx2 = drn_p->myhoopx;
		fz2 = RIM_CZ;

		if (drn_p->nastycnt > 0) {
			f = DRN_FOOT * 2;
			goto seekclose;
		}

		plb_p = drnb_p->pl_p;

		if (plb_p->odata.flags & PF_ANYSHOT && onfire <= 1) {	// Opp shooting?

			f = DRN_FOOT * 6.0f;
			if (plb_p->odata.y > DRN_FOOT * 2)	// Opp in air?
				f = DRN_FOOT * 3.0f;

			if (drn_p->balldist < f) {		// Close enough to contest?
				f = DRN_FOOT * 3;
seekclose:
				fx2 -= fx;
				fz2 -= fz;
				f = f / fsqrt (fx2 * fx2 + fz2 * fz2);
				fx2 *= f;	// Set length
				fz2 *= f;
				goto setseek;
			}
		}

//		fx -= fx2;
//		fz -= fz2;
//
//		f = drn_p->guarddist;
//		if (onfire > 0) {
//			f = 77;
//			if (drng_p->ohoopdist < RIM_CX * 1) f = 154;
//		}
//		fx *= f / 256.0f;
//		fz *= f / 256.0f;


		fx2 -= fx;
		fz2 -= fz;
		f = fsqrt (fx2 * fx2 + fz2 * fz2);
		if (f < DRN_FOOT * 4) {
			f2 = DRN_FOOT * 3;
		}
		else {
			if (onfire >= 2) {			// I'm on fire?
				f2 = DRN_FOOT * 3.5f;	// Stay close to hoop
			}
			else {
				f2 = drn_p->guarddist;
				if (f2 < 0) {				// Dist is from hoop?
					f2 = -f2;
					if (f <= f2 + DRN_FOOT * 3)	// Opp closer?
						f2 = f - DRN_FOOT * 3;
				}
				else f2 = f - drn_p->guarddist;	// Dist is from opp
			}
		}
		f2 = f - f2;

		f = f2 / f;
		fx2 *= f;	// Set length
		fz2 *= f;

setseek:
		fx += fx2;
		fz += fz2;

		drn_p->seek_t[0].x = fx;
		drn_p->seek_t[0].z = fz;

		skcnt = 25;
		if (drng_p->ohoopdist < RIM_CX * .85f) {	// Opp close to my hoop

			i = dskt_t[skill] >> 1;
			skcnt = rndrng0 (i) + i;		// 50 to 100%
			if (drn_p->nastycnt > 0) skcnt -= 5;
//			printf ("Def seek: skill %d tm %d cnt %d\n", skill, i, skcnt);
		}
#ifdef	DEBUG_DRNPERDEF
		skcnt = 2;
#endif
	}
	drn_p->seekcnt = skcnt;

	drone_seek (drn_p);
	drn_p->but = 0;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Turbo if goon is closer to hoop than me

	f = drng_p->ohoopdist - 3.13f;
	if (f < RIM_CZ * .8f) f -= 19;			// Close? Turbo early
	if (f < drn_p->ohoopdist)					// Opp closer?
		if (drn_p->stick != JOY_CENTER)
			drn_p->but |= M_BUTT;				// Moving? Use turbo

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Push / steal

#ifdef	DEBUG_DRNNOSTEAL
	goto skipps;
#endif

	i = dps_t[skill];

	if (pcount & 1)
		if (drn_p->nastycnt > 0)
			i = 200;		// 20% nasty

	if (i <= rndrng0 (999)) goto skipps;	// Skip push?

//	printf ("Def: p/s %d%%\n", i);


	plb_p = drnb_p->pl_p;

	if (rand() & 1) {		// 50% steal?

		f = drn_p->balldist;
		if (f > DRN_FOOT * 3)	// !In his face?
			if (f > ddist_t[skill]) goto skipps;		// Out of steal range?

		if (plb_p->odata.y < DRN_FOOT * 1)	// On or near gnd?
			drn_p->but = M_BUTP;				// Steal?
#ifdef	DEBUG
			printf ("Def: steal\n");
#endif
			return;
	}
skipp:

	if (drn_p->skill < DRNSKL_PUSH)	// Dumb?
		if (pushed_cnt[pl_p->plyrnum] == 0) goto skipps;	// I've never been pushed?

	drone_closest_opptopush (drn_p, 5);
	f = drn_p->odist;
	if (f > ddist_t[skill]) goto skipps;	// Out of range?

	f = plb_p->odata.y - pl_p->odata.y;
	if (f < DRN_FOOT * 7) {					// He's low enough?
		drn_p->but = M_BUTP | M_BUTT;	// Push?
#ifdef	DEBUG
		printf ("Def: push\n");
#endif
		return;
	}
skipps:

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Chk if I can block ball

	f = drn_p->balldist;

	if (f < DRN_FOOT * 5.2f) {	// In his face?

		plb_p = drnb_p->pl_p;
		i = plb_p->odata.flags;

		if (i & (PF_DUNKER | PF_ALLYDUNKER | PF_LAYUP)) {	// Dunking?

			if (f > DRN_FOOT * 2.8f) goto noblk;	// Too far?

			if (drn2_p->balldist > DRN_FOOT * 3.6f) goto tryblk;	// Mate not close?

//			if (pl2_p->flight_time > 0)				// Mate in air?
			if (pl2_p->odata.flags & PF_SWATING)	// Mate blocking?
				if (pcount & 7) goto noblk;	// Skip 88%?
		}
		else {
			if ((i & PF_SHOOTING) == 0) goto noblk;	// !Shooting?

			if (plb_p->odata.y <= 0) {				// Plyr with ball on gnd?
				if (rndrng0 (11)) goto noblk;
				else goto blk;
			}
			f = pl2_p->odata.y;
			if (f > 0 && f < DRN_FOOT * 3) {	// Mate in air?
				if (rndrng0 (29)) goto noblk;	// Don't block usually?
			}

		}

tryblk:
		i2 = dblk_t[skill];		// Block %

		if (i & (PF_DUNKER | PF_ALLYDUNKER)) i2 >>= 1;	// % / 2

		if (drn_p->nastycnt > 0) i2 *= 2;	// % * 2

		if (rndrng0 (99) < i2) {	// Block?
blk:
			drn_p->but = M_BUTS; // | M_BUTT;	// Block
		}
	}
noblk:

}

/********************************
 Check if 1st drone is successfully defending against 2nd drone
 (* 1st, * 2nd)
> !0 = Bad defense
********************************/

#undef	_T
#define	_T	5		// Ticks ahead to look

int	drone_defensechk (DRONE *drn_p, DRONE *drn2_p)
{
	bbplyr_data *pl_p;
	bbplyr_data *pl2_p;
	VEC		vc;
	VEC		vc2;
	int		dird;			// Dir to defense
	int		dirh;			// Dir to hoop

	int		n, i;
	float		f, fx, fz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	pl_p = drn_p->pl_p;
	pl2_p = drn2_p->pl_p;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Check opp skill versus distance

// FIX! find avrg shooter skill

	f = RIM_CX * 1.0f;
	i = pl2_p->_shooting;
	if (i < 4)	// Worse?
		f = RIM_CX * .85f;
	else if (i > 7)	// Better?
		f = RIM_CX * 1.2f;

	if (drn2_p->ohoopdist > f) return (0);	// Far from hoop?

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	i = pl_p->odata.adata[0].animode;
	if (i & MODE_REACTING) return (1);

	vc.x = pl2_p->odata.x + pl2_p->odata.vx * _T;
	vc.z = pl2_p->odata.z + pl2_p->odata.vz * _T;

	vc2.x = drn2_p->ohoopx;
	vc2.z = RIM_CZ;

	dirh = pt_angledist (&vc, &vc2, &f);

	vc2.x = pl_p->odata.x + pl_p->odata.vx * _T;
	vc2.z = pl_p->odata.z + pl_p->odata.vz * _T;

	dird = pt_angledist (&vc, &vc2, &f);

	i = fabs (dird - dirh);
	if (i > 512) i = 1024 - i;		// Dir difference

	if (fabs (i) > 128) return (1);	// Defense not in path of offense?
	if (f > DRN_FOOT * 12) return (1);	// Defense too far?

	return (0);	// OK
}

/********************************
 Drone mode chase ball
********************************/

int	cbmdsk_t[] = {	// Mode switch max seek time
	50,50,50,50,50,	// Up 15-11
	50,50,40,40,40,	// 10-6
	33,25,22,18,16,	// 5-1
	13,					// Even score
	10,8,6,4,4,			// Dn 1-5
	4,3,3,3,2,			// 6-10
	2,2,2,2,1			// 11-15
};
int	cbskt_t[] = {	// Max seek time
	50,50,45,45,45,
	40,40,30,30,22,
	20,17,16,15,15,
	14,
	14,13,13,12,12,
	12,12,12,12,11,
	11,10,10,9,8
};
#define	_N	.313f
float	cbdist_t[] = {	// Max dist to try push
	110*_N, 100*_N, 100*_N, 100*_N, 100*_N,
	90*_N, 90*_N, 80*_N, 80*_N, 80*_N,
	80*_N, 70*_N, 70*_N, 60*_N, 60*_N,
	60*_N,
	50*_N, 50*_N, 50*_N, 50*_N, 50*_N,
	50*_N, 50*_N, 50*_N, 50*_N, 50*_N,
	50*_N, 50*_N, 50*_N, 50*_N, 50*_N
};
#undef	_N

int	cbp_t[] = {		// % to push
	1,2,2,2,2,
	3,3,3,3,3,
	4,4,4,5,5,
	6,
	6,7,9,11,13,
	16,18,19,20,30,
	40,60,80,150,250
};

#define	_PT	5		// Push ticks to check ahead

void	drone_chaseball (DRONE *drn_p)
{
	game_info_t	*gi_p;
	bbplyr_data *pl_p;
	bbplyr_data *pl2_p;
	bbplyr_data *plo_p;
	DRONE			*drn2_p;		// * my buddy
	DRONE			*drno_p;		// Opp
	ball_data	*bd_p;

	int		skill;

	int		skcnt;
	VEC		vc;
	VEC		vc2;
	int		diro;
	int		plnum;

	int		n, i, i2;
	float		f, fx, fy, fz, fx2, fy2, fz2;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	gi_p = &game_info;

	pl_p = drn_p->pl_p;
	pl2_p = drn_p->matepl_p;
	drn2_p = &drone_t[drn_p->mate];

	bd_p = &ball_obj;


//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Check passing

	if (gi_p->ball_mode == BM_PASSING) {

		if (pl_p->plyrnum == gi_p->passer ||	// I'm rcving ball?
			 pl2_p->plyrnum == gi_p->passer) {	// Teammate rcving ball?

			drone_offwoball (drn_p);
			return;
		}

		if ((pl_p->odata.flags & PF_ONSCREEN) == 0) goto def;		// We're off screen so don't try intercept

		i2 = 0;
		n = 1;
		fx = bd_p->odata.x;
		fz = bd_p->odata.z;

		do {
			fx += bd_p->odata.vx;
			fz += bd_p->odata.vz;
			fx2 = fx - pl_p->odata.x;
			fz2 = fz - pl_p->odata.z;
			f = fsqrt (fx2 * fx2 + fz2 * fz2);

			if (f < pl_p->_speed * n + DRN_FOOT * 2) {	// Close enough?
				i2 = 1;
#ifdef	DEBUG_DRNCHASE
//				printf ("Chase: Intrcpt\n");
#endif
				break;
			}
		} while (++n < 40);

		if (i2 == 0) {
def:
			drone_defense (drn_p);
			return;
		}
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Mode

	skill = drone_getcurskill (drn_p);


	drn_p->cflgs = 0;

	if (drn_p->mode != DRNMD_CHASEB) {

		drn_p->mode = DRNMD_CHASEB;
		drn_p->mode2 = DRNMD2_NONE;

		i = cbmdsk_t[skill];
		i = rndrng0 (i) + 2;

		drn_p->seekcnt = i;
	}


//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Check for back peddle after shot

	i = gi_p->ball_mode;
	if (i == BM_SHOOTING || i == BM_BOUNCE_ON_RIM) {	// In shot?

		i = pl_p->odata.adata[0].seq_index;
		if (i == SEQ_J_SHT2_R
		|| i == SEQ_J_SHT2_L
		|| i == SEQ_J_SHT8_R
		|| i == SEQ_J_SHT8_L) {
			drn_p->seek_t[0].x = pl_p->odata.x;		// Don't seek
			drn_p->seek_t[0].z = pl_p->odata.z;
			drn_p->seekcnt = 2;
		}
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Seek

	skcnt = drn_p->seekcnt - 1;


	if (drn_p->mode2 == DRNMD2_REBND) {		// Waiting?
		i = gi_p->ball_mode;
		if (i != BM_SHOOTING && i != BM_BOUNCE_ON_RIM) {	// Done?

			drn_p->mode2 = DRNMD2_NONE;
// FIX! Use skill!!!
			skcnt = rndrng0 (20);
		}
	}


	if (skcnt <= 0) {

		i = cbskt_t[skill];
		skcnt = rndrng0 (i) + 5;

		if (gi_p->ball_mode == BM_PASSING) goto chase;	// Opponents passing?

		if (pl2_p->odata.adata[0].animode & MODE_REACTING) goto chase;

		if (pl2_p->odata.y > 0) goto chase;			// Teammate in air?

		fz = RIM_CZ;

		i = drone_firechk (drn_p);
		if (i == 2) {	// I'm on fire?
			fx = drn_p->myhoopx * .9375f;	// Go in front of my hoop for goaltend
			goto	setxz;
		}
		if (i == 1) goto chase;	// Mate on fire?

		if (drn_p->balldist <= drn2_p->balldist) goto chase;

		if (i == 3) {
			fx = drn_p->myhoopx * .9375f;	// Go in front of my hoop for goaltend
			goto	setxz;
		}

		if (bd_p->odata.vy < 0) {	// Ball on the way down?

			fx = drn_p->ohoopx * .25f;	// Go for opponents top of 3 pt line
			goto	setxz;
		}

chase:
		i = gi_p->ball_mode;
		if (i == BM_SHOOTING || i == BM_BOUNCE_ON_RIM) {// Prepare to rebound?

			fx = RIM_CX - (rndrng0 (4) + 2) * DRN_FOOT;	// Rebound XZ
			if (gi_p->active_goal == 0) {
				fx = -fx;
			}
			fz = RIM_CZ + (rndrng0 (10) - 5) * DRN_FOOT;

			i = 0;	// Check if anyone has my spot reserved
			plnum = pl_p->plyrnum;
			drno_p = drone_t;
			n = 0;
			do {
				if (n == plnum) continue;	// Self
				if (drno_p->mode == DRNMD_CHASEB && drno_p->mode2 == DRNMD2_REBND) {

					fx2 = fx - drno_p->seek_t[0].x;
					fz2 = fz - drno_p->seek_t[0].z;
					f = fsqrt (fx * fx + fz * fz);
					if (f < DRN_FOOT * 2.6f) {		// Too close?
						i = 1;
						break;
					}
		   		}
			} while (drno_p++, ++n < PLYR_MAX);

			if (i) {	// Failed: wait
#ifdef DEBUG
				printf ("Rebnd %d: reseek\n", plnum);
#endif
				skcnt = 1;
				fx = drn_p->seek_t[0].x;
				fz = drn_p->seek_t[0].z;
			}
			else {
				drn_p->mode2 = DRNMD2_REBND;
				skcnt = tsec * 3;
			}
		}
		else {

			fy = bd_p->odata.y;
			if (fy < DRN_FOOT * 6) {
				f = 20;
			}
			else {

				n = 40;
				fy2 = bd_p->odata.vy;
				do {
					fy2 += BALL_GRAVITY;
					fy += fy2;

					if (fy2 < 0 && fy < DRN_FOOT * 10) break;	// Ball in reach?

				} while (--n > 0);

				f = 40 - n;
			}
			fx = bd_p->odata.x + bd_p->odata.vx * f;
			fz = bd_p->odata.z + bd_p->odata.vz * f;
		}

setxz:
		drn_p->seek_t[0].x = fx;
		drn_p->seek_t[0].z = fz;

	}

	drn_p->seekcnt = skcnt;

	i = drone_seek (drn_p);
	drn_p->but = 0;
	if (i != JOY_CENTER) {

		if (drn_p->collcnt >= COLLRESEEKCNT) {

			if (bd_p->odata.y > DRN_FOOT * 5) {		// Ball high enough?

				drn_p->seek_t[0].x = pl_p->odata.x;		// Don't seek
				drn_p->seek_t[0].z = pl_p->odata.z;
				drn_p->seekcnt = tsec;
#ifdef DEBUG
				printf ("Chase: collide\n");
#endif
			}
		}
		drn_p->but |= M_BUTT;	// Turbo
	}
	else {

		if (drn_p->mode2 == DRNMD2_REBND) {	// Waiting for rebound?

			vc.x = RIM_CX;
			if (gi_p->active_goal == 0) vc.x = -RIM_CX;
			vc.z = RIM_CZ;
			i = pt_angledist (&pl_p->odata.x, &vc, &f);

			pl_p->odata.tgt_fwd = i;	// Face hoop
		}
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Push

//#ifdef	DEBUG_DRNNOSTEAL
//	goto skipps;
//#endif

	if (pl_p->odata.y > 0) goto skipp;

//	if (pcount & 1) goto skipp;

	i = 1 - pl_p->team;
	drno_p = &drone_t[i * TEAM_MAX];

	n = TEAM_MAX - 1;
	do {

		plo_p = drno_p->pl_p;

		if (1) {

			if (plo_p->odata.adata[0].animode & MODE_REACTING) continue;

			vc.x = pl_p->odata.x + pl_p->odata.vx * _PT;	// Calc ahead
			vc.z = pl_p->odata.z + pl_p->odata.vz * _PT;
			vc2.x = plo_p->odata.x + plo_p->odata.vx * _PT;
			vc2.z = plo_p->odata.z + plo_p->odata.vz * _PT;
			diro = pt_angledist (&vc, &vc2, &f);

			i2 = diro - pl_p->odata.fwd;
			i = i2 & 0x3ff;
			if (i > 512) i = i - 1024;		// Dir difference (+/-512)

#ifdef	DEBUG_DRNCHASE
//			printf ("Push dist %f, Dir: Me%d, O%d, diff %d\n", f, pl_p->odata.fwd, diro, i);
#endif
			if (i > -64 && i < 64 && f < DRN_FOOT * 4) {	// In front & close

				if (f < DRN_FOOT * 2.8f) {	// In his face?

					if (drn_p->nastycnt > 0) goto push;
				}
				if (f > cbdist_t[skill] * .5f) continue;	// Out of range?

				i = rndrng0 (999);
				if (i >= cbp_t[skill] * 2) continue;		// Skip push?
push:
				f = plo_p->odata.y;
				if (f < RIM_CY * .7f) {					// He's low enough?
// FIX! set down transition?
					drn_p->but |= M_BUTP | M_BUTT;	// Push?

#ifdef	DEBUG_DRNCHASE
					printf ("Push!\n");
#endif
					return;
				}
			}
		}

	} while (drno_p++, --n >= 0);

skipp:


//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Chk if I can jump at ball or steal

	if (drn_p->balldist < DRN_FOOT * 8) {

		fx = pl_p->odata.x - (bd_p->odata.x + bd_p->odata.vx * 16);
		fz = pl_p->odata.z - (bd_p->odata.z + bd_p->odata.vz * 16);
		fx = fx * fx + fz * fz;
		f = fsqrt (fx);

		if (f > DRN_FOOT * 4.8f) goto nojmp;


//	move	@balltmshotcnt,b0
//	subk	TMFIRE_MINCNT-1,b0
//	jrne	#notmheat		;None heating up?
//
//	move	@gmqrtr,b0
//	subk	3,b0
//	jrlt	#tmhqok			;Q123?
//
//	move	@game_time,a14,L
//	srl	8+8,a14			;Remove one & tenths
//	subk	6,a14
//	jrlt	#notmheat		;Less than 60 secs?
//#tmhqok
//	move	@balltmscored,a1
//	srl	5,a1			;0=Tm2, 1=Tm1
//	move	*a13(plyr_num),a0
//	srl	1,a0
//	cmp	a0,a1
//	jrne	#ifire			;My tm?
//#notmheat

		i = drone_firechk (drn_p);
		if (i >= 1) {	// Any of us on fire? Check try goaltend

			fx = pl_p->odata.x - drn_p->myhoopx;
			fz = pl_p->odata.z - RIM_CZ;
			fx = fx * fx + fz * fz;
			fx = fsqrt (fx);

			if (fx < DRN_FOOT * 16) {

				fy = bd_p->odata.y;
				if (fy > RIM_CY) {		// Ball higher than rim?
					if (fy > RIM_CY + BALL_RADIUS * 15) goto nojmp; // Too high?
					if (bd_p->odata.vy < 0) {
						drn_p->but = M_BUTS | M_BUTT;// Turbo jump (need state change?)
#ifdef	DEBUG_DRNCHASE
						printf ("Chase: goaltend\n");
#endif
					}
					goto nojmp;
				}
			}
		}

	//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Jump - rebound

#ifndef	DEBUG_DRNNOREBND

//		if (!auto_rebounding && pl_p->odata.y <= 0) {
		if (pl_p->odata.y <= 0) {

			i = rndrng0 (19);		// 5%
			i2 = gi_p->ball_mode;

			if (!i || (i2 != BM_SHOOTING && i2 != BM_BOUNCE_ON_RIM)) {

				i = bd_p->who_shot;
				if (i != pl_p->plyrnum && i != pl2_p->plyrnum) {	// Defense?

					// If someone is auto jumping, don't jump up again because we don't want to snag
					// ball from human who is auto rebounding
					if (auto_rebounding && (randrng(100) < 95)) goto nojmp;
					if ((i2 == BM_SHOOTING) && (bd_p->flight_time < bd_p->total_time - 18))
						goto nojmp;	// Not at shot end? Skip
				}
				if (f < DRN_FOOT * 6) {

					// Don't want offense to jump up if someone is already auto rebounding
					if (auto_rebounding && (randrng(100) < 95)) goto nojmp;

					fy = bd_p->odata.vy + BALL_GRAVITY * 5;
					fy = bd_p->odata.y + fy * 10;

					if (fy > DRN_FOOT * 8) {	// Ball higher than me?
						if (fy > DRN_FOOT * 14) goto nojmp; // Too high?

						if (bd_p->odata.vy < -BALL_GRAVITY * 2 && rand() & 1) {
							drn_p->but |= M_BUTS | M_BUTT;// Turbo jump (need state change?)
#ifdef	DEBUG_DRNCHASE
							printf ("Chase: rebnd\n");
#endif
						}
						goto nojmp;
					}
				}
				else {
#ifdef DEBUG
					printf ("Drn: might have jumped up!\n");
//					while (!(get_player_sw_current() & ALL_ABCD_MASK)) scan_switches();		// Debug pause game until butn is hit!!!
#endif
				}

			}
		}
#endif

	//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Steal

		if (bd_p->odata.y < RIM_CY * .6f) {	// Ball lower than my head?

			if (bd_p->odata.y > BALL_RADIUS * 2) {	// Ball above gnd?

				if ((rand() & 7) == 0)		// 87%
					drn_p->but = M_BUTP;		// Steal
			}
		}

	}
nojmp:


}

/****************************************************************/

/********************************
 Init inbounding
 (Mode: 0=At off end of court, 1=Near side, 2=Far side,
  | 0x4000 with 1 or 2 for def end, | 0x8000 to instantly move)
********************************/

#define	DRN_IBFREETIME	12
#define	DRN_IBSPDBOOST .7f;
#define	DRN_IBPASSDLY	20		// Human pass but delay after offense in place

static int	inbndrcvrmove;		// !0 = Ball receiver can move if he wants
static int	inbndpassdly;		// - = Ball receiver can pass
static int	inbndmode;			// Save mode to inboundinit & 7fff

static float	ibddist_t[] = { .1f, 0, -.3f, -.5f };	// Defense dist

void	drone_inboundinit (int mode)
{
	DRONE		*drn_p;
	bbplyr_data *pl_p;
	ball_data	*bd_p;
	int		flgs;

	int		n, i;
	float		fx, fz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	inbndmode = mode & 0x7fff;
	inbndrcvrmove = 0;	// Reset
	inbndpassdly = DRN_IBPASSDLY;

//	mode = 0x4002;	// DEBUG

	flgs = mode;
	mode &= 0xff;

	bd_p = &ball_obj;

	drn_p = drone_t;
	n = 0;
	do {
		pl_p = drn_p->pl_p;

//		drn_p->mode = 0;
		drn_p->seekcnt = 1;
		drn_p->nastycnt = 0;

		if (n == OOBPasser) {

			fx = -(COURT_L + 16);
			if (n >= TEAM_MAX) fx = -fx;
			fz = COURT_W * (rndrng0 (10) - 5) * .01f;

			if (mode) {
				fx = drn_p->myhoopx * .5f;
				fz = COURT_W + 16;
			}
			if (flgs & 0x4000) {			// Move to def half
				fx = drn_p->ohoopx * .15f;
			}
		}
		else if (OOBPasser == drn_p->mate) {	// Receiver

			fx = drn_p->myhoopx * .88f;
			fz = COURT_W * (rndrng0 (10) - 5) * .01f;
			if (mode) {
				fx = drn_p->myhoopx * .5f;
				fz = COURT_W * .7f;
			}
			if (flgs & 0x4000) {			// Move to def half
				fx = drn_p->ohoopx * .15f;
			}
		}
		else {											// Defense

			i = pl_p->plyrnum;
#if	NUM_PERTEAM > 2
			if (i >= NUM_PERTEAM) i -= 1;			// pstatus only has 4 bits
#endif
			if (p_status & 1 << i)	// Human?
				i = 0;
			else i = rndrng0 (3);

			fx = ibddist_t[i];
			fx = drn_p->ohoopx * fx;				// Distance
			fz = -.4f * COURT_W;
			if (pl_p->plyrnum & 1) fz = -fz;
			if (mode) {
				fx = drn_p->ohoopx * .1f;
				fz += COURT_W * .3f;
			}
			if (flgs & 0x4000) {			// Move to def half
				fx = drn_p->myhoopx * .55f;
			}
			drn_p->mode2 = DRNMD2_INBND;
		}

		if (mode == 2) fz = -fz;	// Flip sides

		drn_p->seek_t[1].x = fx;
		drn_p->seek_t[1].z = fz;

		if (flgs & 0x8000) {			// Jump to spot?
			pl_p->odata.x = fx;
			pl_p->odata.y = 0;
			pl_p->odata.z = fz;
			pl_p->odata.vx = 0;
			pl_p->odata.vy = 0;
			pl_p->odata.vz = 0;

			if (n == OOBPasser) {
				bd_p->odata.x = fx;
				bd_p->odata.y = DRN_FOOT * 3;
				bd_p->odata.z = fz;
			}

		}

	} while (drn_p++, ++n < PLYR_MAX);

}

/********************************
 Get ball and inbound
********************************/

int	drone_inbound (DRONE *drn_p)
{
	game_info_t	*gi_p;
	bbplyr_data *pl_p;
	ball_data	*bd_p;
	DRONE		*drn2_p;
	int		pn;
	int		human;

	int		dir;
	float		dist;

	int		n, i, i2, i3;
	float		fx, fz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

//	printf ("#%d N %d\n", pl_p->plyrnum, drn_p->nastycnt);

	gi_p = &game_info;
	pl_p = drn_p->pl_p;
	bd_p = &ball_obj;

#ifdef	DEBUG
#if	NUM_PERTEAM > 2
	if (OOBPasser == 5) {
		OOBPasser = 4;
		printf ("Bad OOBPasser!\n");
	}
	if (OOBPasser == 2) {
		OOBPasser = 1;
		printf ("Bad OOBPasser!\n");
	}
#endif
#endif

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

//	if ((pl_p->odata.adata[0].animode | pl_p->odata.adata[1].animode) & (MODE_UNINT|MODE_REACTING))
//		return (1);

	pn = pl_p->plyrnum;

	if (pn != OOBPasser) {	// Ball rcvr?

		drn_p->seek_t[0].x = drn_p->seek_t[1].x;
		drn_p->seek_t[0].z = drn_p->seek_t[1].z;
		i2 = drone_seek (drn_p);
		drn_p->but = 0;

		if (inbndmode > 0) {
			fx = pl_p->odata.vx;		// Speed boost
			fz = pl_p->odata.vz;
			pl_p->odata.x += fx * DRN_IBSPDBOOST;
			pl_p->odata.z += fz * DRN_IBSPDBOOST;
		}

		if (i2 != JOY_CENTER) return (0);		// !At destination?

		human = 0;
		i = pn;
#if	NUM_PERTEAM > 2
		if (i >= NUM_PERTEAM) i -= 1;			// pstatus only has 4 bits
#endif
		if (p_status & 1 << i) human = 1;


		if (human && inbndpassdly <= 0) {	// Human & can pass?
				i = get_player_sw_current() >> (gi_p->plyr_control[pn] << 3) & P_ABCD_MASK;
				if (i & M_BUTP) {
					drn_p->matepl_p->misc_wait_time = -1;
#ifdef	DEBUG
					printf ("IB: early pass\n");
#endif
				}
		}

		if (inbndrcvrmove) {	// Move allowed?

			if (human) {	// I'm human?

				i = get_player_49way_current() >> (gi_p->plyr_control[pn] << 3) & P_SWITCH_MASK;
				if (i != JOY_CENTER) {

//					printf ("IB: All non IB Autoctrl clr\n");

//					if (pl_p->misc_wait_time > 3)
//						pl_p->misc_wait_time = 3;

					i = OOBPasser;		// Clear autocontrols except inbounder
					drn2_p = drone_t;
					n = 0;
					do {
						if (i != n) {	// Not inbounder
							drn2_p->pl_p->auto_control = 0;
						}
					} while (drn2_p++, ++n < PLYR_MAX);
				}

			}
			else {	// Drone?
				if (rndrng0 (9) == 0) {	// 10%? Go random direction
					i = rand() & 0x7f;
					if (rand() & 1) i = 512 - i;
					fx = pl_p->odata.x;
					if (fx > 0) i += 512;
					fx += isin (i) * DRN_FOOT * 20;
					fz = pl_p->odata.z + icos (i) * DRN_FOOT * 20;
					drn_p->seek_t[1].x = fx;
					drn_p->seek_t[1].z = fz;
#ifdef	DEBUG
					printf ("IB: rcv seek %f %f\n", drn_p->seek_t[1].x, drn_p->seek_t[1].z);
#endif
				}
			}


		}

		return (0);
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ We're OOBPasser

	else {

#ifdef	DEBUG_INBND
//		printf ("IB %d\n", pn);
#endif
		if (pl_p->dribble_mode == DM_CANT_DRIBBLE &&
			 pn == gi_p->ball_handler) {

			// he's out of bounds with the ball and he can't move.
			drn_p->but = 0;
			drn_p->stick = JOY_CENTER;

#ifdef	DEBUG_INBND
			printf ("No drib\n");
#endif

		}
		else if (pl_p->auto_control == YES) {	// Run in?

			if (inbndmode > 0) {
				fx = pl_p->odata.vx;		// Speed boost
				fz = pl_p->odata.vz;
				pl_p->odata.x += fx * DRN_IBSPDBOOST;
				pl_p->odata.z += fz * DRN_IBSPDBOOST;
			}

			if (gi_p->game_mode == GM_IN_PLAY || gi_p->ball_mode == BM_PASSING) {
				drn_p->seek_t[0].x = 0;
				drn_p->seek_t[0].z = 0;
				i = drone_seek (drn_p);
//				if (in_bounds (&pl_p->odata))
//					pl_p->auto_control = 0;
			}

			else if (gi_p->ball_handler == NO_BALLHANDLER) {	// Get ball?

				fx = bd_p->odata.x + bd_p->odata.vx * 8;
				fz = bd_p->odata.z + bd_p->odata.vz * 8;
				drn_p->seek_t[0].x = fx;
				drn_p->seek_t[0].z = fz;
				drone_seek (drn_p);
				drn_p->but = 0;
			}

			// move this passer out of bounds so he can pass in the ball

			else if (pl_p->dribble_mode != DM_CANT_DRIBBLE) {

				drn_p->seek_t[0].x = drn_p->seek_t[1].x;
				drn_p->seek_t[0].z = drn_p->seek_t[1].z;
				i = drone_seek (drn_p);

				pl_p->dribble_mode = 0;

				// Close enough to the oob point?
				if (i == JOY_CENTER) {

					update_status_box = 1;		// update score
					pl_p->dribble_mode = DM_CANT_DRIBBLE;
#ifdef DEBUG_INBND
					printf ("IB can't dribble\n");
#endif
					i = bd_p->odata.x < 0.0f ? 256 : 768;
					if (bd_p->odata.z > COURT_W / 3) i = 256;
					if (bd_p->odata.z < -COURT_W / 3) i = 0;
					pl_p->odata.tgt_fwd = i;

					pl_p->misc_wait_time = tsec * 4 - 20;
				}
			}
		}	
	}	

	//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Check if ready

	if (pn == gi_p->ball_handler &&
			pl_p->dribble_mode == DM_CANT_DRIBBLE) {	// Ready to pass?

		i = 1;	// Check if everyone in place
		i2 = 1;
		drn2_p = drone_t;
		n = 0;
		do {
			if (drn2_p->stick != JOY_CENTER) {		// Moving?
				if (pl_p->team == drn2_p->pl_p->team) {
					i = 0;	// Not ready
				}
				else {
					if (drn2_p->balldist < (RIM_CX-10.0f)) {	// Are opps far enough?
						i2 = 0;	// Not ready
					}
				}
			}
		} while (drn2_p++, ++n < PLYR_MAX);



		if (i && !inbound_passwait())		// Countdown?
			--inbndpassdly;


		i3 = pl_p->misc_wait_time;

		if (i & i2 && i3 > DRN_IBFREETIME)
			pl_p->misc_wait_time = DRN_IBFREETIME;


		if ((i & i2) || i3 < 0) {	// In place or timeout?

			inbndrcvrmove = 1;

//			drn2_p = drone_t;
//			n = 0;
//			do {
//				if (pn != n) {	// Not inbounder
//					drn2_p->pl_p->auto_control = 0;
//				}
//			} while (drn2_p++, ++n < PLYR_MAX);
		}


	//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Pass in

		if (pl_p->misc_wait_time < 0) {

			drn_p->but = M_BUTP;			// Pass
// MJT - maybe do turbo?
//			drn_p->but = M_BUTP | M_BUTT;			//  Turbo Pass


			//if (ISDRONETEAM(pl_p) == YES)???
			ball_obj.int_receiver = drn_p->mate;
//			ball_obj.int_receiver = gi_p->receiver_a = nearest_to_me(pn, pl_p->team);

			// reset the shot clock and start it running
			reset_shotclock  = YES;
			shotclock_active = 1;
//			clock_active = 1;		// start game clock

			qcreate ("inbndsp", 0, inbound_ball_speech_proc, 0,0,0,0);

			i = gi_p->off_side * NUM_PERTEAM;
			player_blocks[i].odata.plyrmode = MODE_OFFENSE;
			player_blocks[i+1].odata.plyrmode = MODE_OFFENSE;
#if	NUM_PERTEAM > 2
			player_blocks[i+2].odata.plyrmode = MODE_OFFENSE;
#endif

#ifdef DEBUG_INBND
			printf ("IB %d pass\n", pn);
#endif
		}
	}
	return (1);
}




/********************************
 Defend against inbound
********************************/

int	drone_inbounddef (DRONE *drn_p)
{
	game_info_t	*gi_p;
	bbplyr_data *pl_p;
	ball_data	*bd_p;

	int		dir;
	float		dist;

	int		i;
	float		f, fx, fz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

//	printf ("#%d N %d\n", pl_p->plyrnum, drn_p->nastycnt);

	gi_p = &game_info;
	pl_p = drn_p->pl_p;
	bd_p = &ball_obj;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	if (pl_p->auto_control) {

		if(pl_p->odata.y <= 0)
			pl_p->odata.flags &= ~(PF_PLYRFLAGS & ~PF_CELEBRATE);

//		fx = drn_p->ohoopx * .45f;
//		fz = -1;
//		if (pl_p->plyrnum & 1) fz = 1;
//		fz = .4f * COURT_W * fz;
//		drn_p->seek_t[0].x = fx;
//		drn_p->seek_t[0].z = fz;

		drn_p->seek_t[0].x = drn_p->seek_t[1].x;
		drn_p->seek_t[0].z = drn_p->seek_t[1].z;
		i = drone_seek (drn_p);
		drn_p->but = 0;

		if (i == JOY_CENTER) {

			i = pt_angledist (&pl_p->odata.x, &bd_p->odata.x, &f);
			pl_p->odata.tgt_fwd = i;
		}

		if (inbndmode > 0) {
			fx = pl_p->odata.vx;		// Speed boost
			fz = pl_p->odata.vz;
			pl_p->odata.x += fx * DRN_IBSPDBOOST;
			pl_p->odata.z += fz * DRN_IBSPDBOOST;
		}

//		fx = pl_p->odata.vx;
//		fz = pl_p->odata.vz;
//		f = 4.0f / fsqrt (fx * fx + fz * fz);
//		pl_p->odata.vx *= f;
//		pl_p->odata.vz *= f;
//		printf ("Vel %f %f * %f\n", pl_p->odata.vx, pl_p->odata.vz, f);

		return (0);
	}
	else {

		drone_defense (drn_p);

	}
	return (1);
}


#if	1
/********************************
 Check if inbound passing should wait
> !0 = Wait
********************************/

int	inbound_passwait()
{
	return (0);
}
#endif


/********************************
 Find closest opponent to ball
> * closest opponent
********************************/

DRONE	*drone_closest_opptoball (DRONE *drn_p)
{
	bbplyr_data *pl_p;
	DRONE		*drno_p;		// Current opp
	DRONE		*drnc_p;		// Closest opp

	int		n, i;
	float		f;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	pl_p = drn_p->pl_p;

	i = 1 - pl_p->team;
	drno_p = &drone_t[i * TEAM_MAX];

	f = drno_p->balldist;
	drnc_p = drno_p;

	n = TEAM_MAX - 1;

	do {
		drno_p++;
		if (drno_p->balldist < f) {
			f = drno_p->balldist;
			drnc_p = drno_p;
		}
	} while (--n > 0);

	return (drnc_p);
}


/********************************
 Find closest opponent to me
> * closest opponent
********************************/

DRONE	*drone_closest_opptome (DRONE *drn_p, float time)
{
	bbplyr_data *pl_p;
	DRONE		*drno_p;		// Current opp
	DRONE		*drnc_p;		// Closest opp
	bbplyr_data *plo_p;

	VEC		vc;
	VEC		vc2;
	float		dist;

	int		n, i;
	float		f;
	float		fx, fz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	pl_p = drn_p->pl_p;

	vc.x = pl_p->odata.x + pl_p->odata.vx * time;	// Calc future pos
	vc.z = pl_p->odata.z + pl_p->odata.vz * time;

	i = 1 - pl_p->team;
	drno_p = &drone_t[i * TEAM_MAX];
	drnc_p = drno_p;

	dist = 99999999;

	n = TEAM_MAX;

	do {
		plo_p = drno_p->pl_p;

		vc2.x = plo_p->odata.x + plo_p->odata.vx * time;
		vc2.z = plo_p->odata.z + plo_p->odata.vz * time;

		fx = vc2.x - vc.x;
		fz = vc2.z - vc.z;

		f = fsqrt (fx * fx + fz * fz);

		if (f < dist) {	// Closer?
			dist = f;
			drnc_p = drno_p;
		}
	} while (drno_p++, --n > 0);

	drn_p->odist = dist;	// Save

	return (drnc_p);
}


/********************************
 Find closest opponent to push
> * closest opponent. odist is set
********************************/

DRONE	*drone_closest_opptopush (DRONE *drn_p, float time)
{
	bbplyr_data *pl_p;
	DRONE		*drno_p;		// Current opp
	DRONE		*drnc_p;		// Closest opp
	bbplyr_data *plo_p;

	VEC		vc;
	VEC		vc2;
	float		dist;
	int		diro;

	int		n, i;
	float		f;
	float		fx, fz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	pl_p = drn_p->pl_p;

	vc.x = pl_p->odata.x + pl_p->odata.vx * time;	// Calc future pos
	vc.z = pl_p->odata.z + pl_p->odata.vz * time;

	i = 1 - pl_p->team;
	drno_p = &drone_t[i * TEAM_MAX];
	drnc_p = drno_p;

	dist = 99999999;

	n = TEAM_MAX;

	do {
		plo_p = drno_p->pl_p;

		if (plo_p->odata.adata[0].animode & MODE_REACTING) continue;	// Skip?

		vc2.x = plo_p->odata.x + plo_p->odata.vx * time;
		vc2.z = plo_p->odata.z + plo_p->odata.vz * time;

		diro = pt_angledist (&vc, &vc2, &f);

		i = diro - pl_p->odata.fwd;
		i = i & 0x3ff;
		if (i > 512) i = i - 1024;		// Dir difference (+/-512)

		if (i > -64 && i < 64) {	// In front

			if (f < dist) {	// Closer?
				dist = f;
				drnc_p = drno_p;

#ifdef	DEBUG_DRNPUSH
				printf ("Push dist %f, Dir: Me%d, O%d, diff %d\n", f, pl_p->odata.fwd, diro, i);
#endif
			}
		}

	} while (drno_p++, --n > 0);

	drn_p->odist = dist;	// Save

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	return (drnc_p);
}


/********************************
 Check fire status
> -3=Opp team, -1=Opp fire, 0=No fire, 1=Mate, 2=Me, 3=Team
********************************/

int	drone_firechk (DRONE *drn_p)
{
	game_info_t	*gi_p;
	bbplyr_data *pl_p;

	int		i;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#ifdef	DEBUG
//	return (-3);
#endif
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	gi_p = &game_info;
	pl_p = drn_p->pl_p;

	i = gi_p->team_fire;
	if (i == pl_p->team) return (3);
	if (i >= 0) return (-3);

	if (gi_p->fire_cnt >= FIRECNT) {
		i = gi_p->fire_player;
		if (drn_p->num == i) return (2);
		if (drn_p->mate == i) return (1);
		return (-1);
	}

	return (0);
}

/********************************
 Push stick to move drone towards seek location
 (drone data *)
> Joy value or JOY_CENTER
********************************/

int	drone_seek (DRONE *drn_p)
{
	return (drone_seekxy (drn_p, drn_p->seek_t[0].x, drn_p->seek_t[0].z));
}

/********************************
 Push stick to move drone towards an XZ location
 (drone data *, X, Z)
> Joy value or JOY_CENTER
********************************/

int	drone_seekxy (DRONE *drn_p, float skx, float skz)
{
	bbplyr_data	*pl_p;
	float		f_t[3];
	int		dir;
	int		stick;

	int		i;
	float		x, z;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	pl_p = drn_p->pl_p;

#ifdef	DEBUG
	if (OOBPasser < 0) {

//		if (skx < -OUT_OF_BOUNDS_X || skx > OUT_OF_BOUNDS_X ||
//				skz < -OUT_OF_BOUNDS_Z || skz > OUT_OF_BOUNDS_Z)
//			printf ("Drn %d seek bad, %f %f\n", pl_p->plyrnum, skx, skz);
	}
#endif

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	x = fabs (pl_p->odata.x - skx);
	z = fabs (pl_p->odata.z - skz);

	dir = -1;
	stick = JOY_CENTER;

	if (x > DRN_FOOT * 1.0f ||
			z > DRN_FOOT * 1.0f) {

		f_t[0] = skx;
		f_t[2] = skz;

		dir = ptangle (f_t, &pl_p->odata.x);
		stick = dir2stick (dir - (int)RD2GR(cambot.theta) - 512);

	}
	else {
		drn_p->seek_t[0].x = skx;
		drn_p->seek_t[0].z = skz;
	}

	if (drn_p->stick != JOY_CENTER) {
		if (dir >= 0) {
			i = fabs (drn_p->seekdir - dir);
			if (i > 512) i = 1024 - i;

			if (i < 16) {
				return (drn_p->stick);	// No change
			}
		}
	}
	drn_p->seekdir = dir;
	drn_p->stick = stick;

	return (stick);
}


/********************************
 Get the current skill value
> Skill (0-30)
********************************/

int	drone_getcurskill (DRONE *drn_p)
{
	int		i;

	i = drn_p->ptsdown + drn_p->skill;

//	if (drnsmart) i += 5;

	i += 15;
	if (i < 0) i = 0;
	if (i > 30) i = 30;

	if (pup_tourney) i = DRN_TOURNSKL + 15;

	return (i);
}


/********************************
 Adjust all drone abilities (at every minute dec of game clock)
 (Game clock minute count before dec (1-3))
********************************/

int	skadj_t[] = {
	-5,-5,-5,-5,-5,
	-5,-5,-5,-4,-3,
	-2,-1,-1,0,0,
	0,
	0,1,1,2,2,
	3,3,4,4,5,
	5,6,6,6,7
};
int	skmin_t[] = { -15,-12,-10, -9,-8,-7, -7,-6,-6, -6,-6,-6 };

#define	SKTEAM	-13,-12,-11, -10,-9,-9, -8,-8,-8, -7,-7,-7		// Worst
#define	SKTEAM2	-10,-9,-8, -8,-7,-7, -7,-6,-6, -6,-5,-5

int	sktmmin_t[] = {
	-7,-7,-6, -6,-5,-5, -4,-4,-3, -3,-2,-3,	// ATL (14)
	-9,-8,-8, -7,-7,-6, -6,-5,-4, -4,-3,-4,	// BOS (19)
	-9,-8,-8, -7,-7,-6, -6,-6,-5, -5,-4,-4,	// CHA (20)
	SKTEAM2,												// CHI
	-8,-8,-7, -7,-6,-6, -5,-4,-3, -3,-2,-3,	// CLE (16)
	SKTEAM2,												// DAL
	SKTEAM,												// DEN
	-6,-5,-5, -4,-3,-3, -2,-2,-2, -2,-1,-2,	// DET (12)
	SKTEAM2,												// GOL
	-1,-1,0, 1,1,1, 2,2,2, 3,4,3,					// HOU (6)
	4,4,4, 4,4,5, 5,5,7, 8,9,8,					// IND (3)
	SKTEAM,												// LAC
	1,1,1, 2,2,2, 3,3,3, 4,5,4,					// LAL (5)
	2,2,2, 3,3,3, 4,4,4, 5,6,5,					// MIA (4)
	-6,-5,-5, -4,-3,-3, -3,-2,-2, -2,-1,-2,	// MIL (13)
	-9,-8,-8, -7,-7,-6, -6,-5,-4, -4,-3,-4,	// MIN (18)
	SKTEAM,												// NJN
	-4,-3,-3, -2,-1,-1, 0,0,0, 1,2,1,			// NYK (8)
	-3,-2,-2, -1,0,0, 1,1,1, 2,3,2,				// ORL (7)
	-6,-5,-5, -4,-3,-3, -2,-2,-2, -1,0,-1,		// PHI (11)
	-7,-7,-6, -6,-5,-5, -4,-4,-3, -3,-2,-3,	// PHO (15)
	4,5,6, 6,8,8, 9,9,9, 10,11,10,				// POR (2)
	SKTEAM2,												// SAC
	-6,-5,-5, -4,-3,-3, -2,-2,-2, -1, 0,-1,	// SAN (10)
	-5,-4,-4, -3,-2,-2, -1,-1,-1, 0,1,0,		// SEA (9)
	SKTEAM2,												// TOR
	6,7,7, 8,9,9, 10,10,11, 11,13,12,			// UTA (1)
	SKTEAM,												// VAN
	-8,-8,-7, -7,-6,-6, -5,-4,-3, -3,-2,-3,	// WAS (17)
};

void	drone_adjskill (int min)
{
	game_info_t	*gi_p;
	DRONE	*drn_p;
	bbplyr_data *pl_p;

	int		qtr;			// Quarter 0-3
	int		indx;			// Time index 0-11
	int		diff;			// Difficulty -14 to 6
	int		ptsdn;			// Score difference, +=losing
	int		sk;				// Skill
	int		win_t[2];		// # wins
	int		lstrk_t[2];		// Loosing streak

	int		n, i, i2;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

//	if (min > 3) return;	// Start of quarter?

#ifdef	DEBUG_SKILL
	printf ("Drn skill: minute %d ", min);
#endif

	gi_p = &game_info;

	qtr = gi_p->game_quarter;
	if (qtr > 3) qtr = 3;
	indx = qtr * 3 + (3 - min);		// 0-11

	if (get_adjustment (DIFFICULTY_ADJ, &i))	// Error? Should be 0-10
		i = 5;
	diff = (i - 7) * 2;

	ptsdn = compute_score (1) - compute_score (0);

#ifdef	DEBUG_SKILL
	printf ("indx %d diff %d ptsdn %d, ", indx, diff, ptsdn);
#endif

	win_t[0] = -1;
	win_t[1] = -1;
	lstrk_t[0] = 0;
	lstrk_t[1] = 0;

	n = 3;
	do {
		if (p_status & 1 << n)	// Human?
			if (selected_players[n].record_num >= 0) {
				i2 = n >> 1;	// 0-1

				i = selected_players[n].cmos_data.games_won;
				if (i > win_t[i2]) win_t[i2] = i;	// Get max

				i = selected_players[n].cmos_data.winstreak;	// - if loosing
				if (i > 0) i = 0;
				i = -i;
				if (i > lstrk_t[i2]) lstrk_t[i2] = i;	// Get max
			}
	} while (--n >= 0);


	drn_p = drone_t;
	n = 0;

	do {
//		pl_p = drn_p->pl_p;

		sk = drn_p->skill;

		if (n == TEAM_MAX) ptsdn = -ptsdn;	// Invert for team2
		i = ptsdn + 15;
		if (i < 0) i = 0;
		if (i > 29) i = 29;
		sk += skadj_t[i];


		i = drn_p->mate;

#if	NUM_PERTEAM > 2
		if (i >= NUM_PERTEAM) i -= 1;		// pstatus only has 4 bits
#endif
		if ((p_status & 1 << i) == 0) {	// Teammate is drone?

			i2 = (n >> 1) ^ 1;
			i = win_t[i2];
			if (i == 0 || i == 1) {				// Other team has few wins?
				sk = -11 + diff + qtr + i * 5;	// Low skill
			}
			else {
				i = gi_p->team[0];
				if (n >= TEAM_MAX)
					i = gi_p->team[1];
				if (i >= NUM_TEAMS) i = TEAM_UTA;	// Special team set to best team

				i = sktmmin_t[i * 12 + indx] + diff;
				if (indx == 0) sk = i;		// 1st minute?

				if (sk < i) sk = i;			// Below minimun?
				i += 8;
				if (sk > i) sk = i;			// Above max?

				sk -= lstrk_t[i2];			// I'm dumber if other team on loosing streak
#ifdef	DEBUG_SKILL
				printf ("Min%d ", i-8);
				if (lstrk_t[i2]) printf ("LStrk%d ", lstrk_t[i2]);
#endif
			}
		}
		else {		// Mate is human
			i = skmin_t[indx] + diff;
			if (sk < i) sk = i;				// Below minimum?
		}

		if (pup_tourney) sk = DRN_TOURNSKL;

		drn_p->skill = sk;

#ifdef	DEBUG_SKILL
		printf ("%d, ", sk);
#endif

	} while (drn_p++, ++n < PLYR_MAX);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#ifdef	DEBUG_SKILL
	printf ("\n");
#endif

}

/********************************
 Check clocks
> !0 = Time OK
********************************/

int	drone_timeok()
{
	game_info_t	*gi_p;
	int		i;

	gi_p = &game_info;

	i = gi_p->game_time;			// 0xMMSSTT
	if (i < 0x100) return (0);	// Less than 2 sec?

	if (gi_p->shotclock < 0x100) return (0);

	return (1);
}

/********************************
 Convert direction to stick value
 (dir 0-1023; 0 = Up going counter clockwise)
> Stick value (0-48)
********************************/

static int dir2st_t[] = {
	 3,  2,  9,  1,  0,  7, 15, 14,
	21, 28, 29, 35, 42, 43, 37, 44,
	45, 46, 39, 47, 48, 41, 33, 34,
	27, 20, 19, 13,  6,  5, 11,  4
};

int	dir2stick (int dir)
{
	dir = (dir + 16) >> 5 & 0x1f;	// Round off & mask

	return (dir2st_t[dir]);
}


/********************************
 Dump drone debug info
********************************/

void	dump_drone_states()
{
	DRONE		*drn_p;
	bbplyr_data	*pl_p;

	int		n, i, i2;
	char		*c_p, *c2_p;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#ifndef RELEASE_VER
 #ifndef TEST_VER

	fprintf (stderr, "Drone states:\n");

	drn_p = drone_t;
	n = 0;

	do {

		pl_p = drn_p->pl_p;

//		drn_p->myhoopx = i;
//		drn_p->ohoopx = -i;
//		drn_p->mate = i;
//		drn_p->guardnum = (n + (PLYR_MAX / 2)) % PLYR_MAX;
//		drn_p->guardmatenum = (i + (PLYR_MAX / 2)) % PLYR_MAX;
//		drn_p->lowsecagr = 10 << 8;

		i = pl_p->plyrnum;

		i2 = ' ';
		if (i == game_info.ball_handler)
			i2 = '*';

		fprintf (stderr, "%d%c ------", i, i2);

		c_p = drnmd_pt[drn_p->mode];
		c2_p = drnmd2_pt[drn_p->mode2];

		fprintf (stderr, " modes %s %s flgs %x cflgs %x", c_p, c2_p, drn_p->flgs, drn_p->cflgs);
		fprintf (stderr, "  stick %d but %x", drn_p->stick, drn_p->but);
		fprintf (stderr, "\n");
		fprintf (stderr, " My hoop: x %4.0f dir %d dist %f", drn_p->myhoopx, drn_p->myhoopdir, drn_p->myhoopdist);
		fprintf (stderr, "\n");
		fprintf (stderr, " Op hoop: x %4.0f dist %f", drn_p->ohoopx, drn_p->ohoopdist);
		fprintf (stderr, "  ball dist %f", drn_p->balldist);
		fprintf (stderr, "\n");

		fprintf (stderr, " mate %d", drn_p->mate);
		fprintf (stderr, "  Seek: dir %d cnt %d xz %3.0f %3.0f zmd %d", drn_p->seekdir, drn_p->seekcnt, drn_p->seek_t[0].x, drn_p->seek_t[0].z, drn_p->seekzmd);
		fprintf (stderr, "  collcnt %d", drn_p->collcnt);
		fprintf (stderr, "\n");

		fprintf (stderr, " skill %d ptsdn %d", drn_p->skill, drn_p->ptsdown);
		fprintf (stderr, " nastyc %d lowsecagr %x", drn_p->nastycnt, drn_p->lowsecagr);
		fprintf (stderr, " Guard: # %d mate # %d dist %f", drn_p->guardnum, drn_p->guardmatenum, drn_p->guarddist);
		fprintf (stderr, "\n");

		drn_p++;
	} while (++n < PLYR_MAX);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	fprintf (stderr, "\n");
 #endif
#endif
}


/********************************
 Draw drone seek positions
********************************/

#ifdef	DEBUG_DRNSEEK
extern LIMB	limb_gndsquare_obj;
extern Texture_node_t	*cursor_decals[];
#ifdef	VEGAS
#define	GR_ASPECT_1x1	GR_ASPECT_LOG2_8x1
#endif
#define	SKSCL	.7f

void	drone_seekdraw (void *o_p)
{
	DRONE		*drn_p;

	float		cm[12];
	float		mat[12] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f };

	int	ratio = GR_ASPECT_1x1;

	int	n, i;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	grConstantColorValue (0xe0000000);

	guAlphaSource (GR_ALPHASOURCE_TEXTURE_ALPHA);
	grTexCombineFunction (0, GR_TEXTURECOMBINE_DECAL);
	grTexClampMode (0, GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP);

	grDepthBufferMode (GR_DEPTHBUFFER_WBUFFER);
	grDepthBufferFunction (GR_CMP_ALWAYS);
	grDepthMask (FXTRUE);

	guColorCombineFunction (GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
	grChromakeyMode (GR_CHROMAKEY_DISABLE);

//	grAlphaCombine(
//		GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL,
//		GR_COMBINE_FACTOR_ONE_MINUS_OTHER_ALPHA,
//		GR_COMBINE_LOCAL_CONSTANT,
//		GR_COMBINE_OTHER_TEXTURE,
//		FXFALSE );
	grAlphaBlendFunction (GR_BLEND_SRC_ALPHA, GR_BLEND_ONE_MINUS_SRC_ALPHA,
			GR_BLEND_ONE, GR_BLEND_ZERO);


	drn_p = drone_t;
	n = 0;
	do {

		i = game_info.plyr_control[n];

		if (i < 0) {
			mat[3]  = drn_p->seek_t[0].x - cambot.x;
			mat[7]  = -cambot.y;
			mat[11] = drn_p->seek_t[0].z - cambot.z;

			roty (512 + (int) RD2GR(cambot.theta), mat);
			mxm (cambot.xform, mat, cm);

			cm[0] *= SKSCL;
			cm[1] *= SKSCL;
			cm[2] *= SKSCL;
			cm[4] *= SKSCL;
			cm[5] *= SKSCL;
			cm[6] *= SKSCL;
			cm[8] *= SKSCL;
			cm[9] *= SKSCL;
			cm[10] *= SKSCL;

			render_limb (&limb_gndsquare_obj, cm,
				cursor_decals + n, &ratio);
//			printf ("Seek %d %x\n", n, (int) cursor_decals[n]);
		}
		drn_p++;
	} while (++n < PLYR_MAX);

}
#endif


/****************************************************************/
// Old

/********************************
********************************/

#if	0
static int dir_table[] = {
// angles
	  0,  52,  76,  96, 128, 160, 180, 204,
	256, 308, 332, 352, 384, 416, 436, 460,
	512, 564, 588, 608, 640, 672, 692, 716,
	768, 820, 844, 864, 896, 928, 948, 972,
	1024,
// sticks
	 3,  2,  9,  1,  0,  7, 15, 14,
	21, 28, 29, 35, 42, 43, 37, 44,
	45, 46, 39, 47, 48, 41, 33, 34,
	27, 20, 19, 13,  6,  5, 11,  4,
	 3
};
#define CNT_TABLE	(sizeof(dir_table)/sizeof(int)/2)

// 0 <= dir <= 1023
int dir2stick( int dir )
{
	register int d = dir & 1023;
	register int * p = dir_table + CNT_TABLE/2;

	while (d < *p) p--;
	while (d > *++p);
	if ((*p - d) < (d - *--p)) p++;

	return p[CNT_TABLE];
}
#endif

/********************************
********************************/

int	plyr_crossed_halfcourt (int player)
{
	if (( game_info.active_goal && player_blocks[player].odata.x > 0.0f) ||
		(!game_info.active_goal && player_blocks[player].odata.x < 0.0f))
	{
		return YES;
	}
	return NO;	
}


/****************************************************************/

#if	0
/*
**************************************************************
*
* Owner:		Shawn
*
* Software:		Shawn Liptak
* Initiated:		9/17/92
*
* Modified:		Shawn Liptak, 3/24/93	-New compatible version
* 			Shawn Liptak, 1/17/94	-Smarter for tournament ed.
* 			Shawn Liptak, 3/20/96	-Smarter for HangTime
*
* COPYRIGHT (C) 1992-1996 WILLIAMS ELECTRONICS GAMES, INC.
*
*.Last mod - 4/8/96 17:01
**************************************************************
	.file	"drone.asm"
	.title	"basketball drone code"
	.width	132
	.option	b,d,l,t
	.mnolist


	.include	"mproc.equ"		;Mproc equates
	.include	"disp.equ"		;Display proc equates
	.include	"gsp.equ"		;Gsp asm equates
	.include	"sys.equ"
	.include	"audit.equ"
	.include	"macros.hdr"		;Macros
	.include	"world.equ"		;Court-world defs
	.include	"game.equ"
	.asg		0,SEQT
	.include	"plyr.equ"


;sounds external


;symbols externally defined

	.ref	plyrobj_t,plyrproc_t
	.ref	ballobj_p
	.ref	ballpnum,ballpnumshot
	.ref	ballnumscored,ballpnumscored
	.ref	balltmshotcnt,balltmscored
	.ref	plyr_onfire
	.ref	seekdirdist_obxz128

	.ref	game_time,gmqrtr
	.ref	shotimer

	.ref	team1,team2

	.ref	PCNT
	.ref	RNDPER
	.ref	PSTATUS
	.ref	GET_ADJ


;symbols defined in this file


;uninitialized ram definitions

	.bss	drnzzcnt	,16	;Drone zigzag mode cntdn
	.bss	drnzzmode	,16	;Drone zigzag mode (0-?)
	BSSX	drone2on	,16	;!0=Use drone version2 code

	BSSX	dronesmrt	,16	;Bit 0-3 if 1 = Smarter drone


;equates for this file


BV5	equ	>1040->80+>42*6

NOPUSHSTEAL	equ	0	;!0=No push or stealing
ONLY3		equ	0	;!0=Only shoot 3's

	.text



#***************************************************************
* Main drone logic
* A8 = *Obj
* A9 = *Plyr secondary data
* A11= *Ctrl bits
* A13= *Plyr process
* Trashes scratch, A2-A5

 SUBR	drone_main


;	move	*a13(plyr_d_seekcnt),a0
;	cmpi	TSEC*4/2,a0
;	jrle	#_skok
;	LOCKUP
;#_skok

;	move	@ballnumscored,b0
;	subk	ONFIRE_MINCNT-1,b0
;	jrge	#_warm				;None heating up?
;	movk	2,b0
;	move	b0,@ballnumscored
;#_warm

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	move	@ballpnum,a14
	jrn	drone_chaseball		;No owner?

;ÄÄÄÄÄÄÄ
;DEBUG
;	move	*a13(plyr_ohoopx),a0
;	movi	200,a1
;	cmpi	WRLDMID,a0
;	jrlt	#_1
;	neg	a1
;#_1
;	add	a1,a0
;	move	a0,*a13(plyr_d_seekx)
;	movi	CZMID-100,a0
;	move	a0,*a13(plyr_d_seeky)
;ÄÄÄÄÄÄÄ

	move	*a13(plyr_ownball),a1
	jrz	drone_defense		;We don't have ball?
	jrn	drone_offwoball		;Teammate has ball?


;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ			>Offense

	move	@PSTATUS,a0		;Plyr start bits 0-3
	move	*a13(plyr_num),a1
	XORK	1,a1
	btst	a1,a0
	jrz	#tmdrone		;Teammate is a drone?

	move	*a13(plyr_d_cflgs),a2
	btst	DRN_PASS_B,a2
	jrz	#nopass

	movi	BUT2_M<<8+BUT2_M,a14	;>Make him pass
	jruc	#docmd

#nopass
	btst	DRN_SHOOT_B,a2
	jrz	#noshoot
	movi	BUT1_M<<8|BUT1_M|BUT3_M,a14	;>Make him shoot
;	movk	3,a0
;	move	a0,*a13(plyr_d_seekcnt)
#docmd
	move	*a11,a0
	sll	32-4,a0
	srl	32-4,a0
;	move	a2,a2
;	jrnn	#noturb
	ori	BUT3_M,a0		;+turbo
#noturb
	or	a14,a0
	move	a0,*a11
	clr	a0
	move	a0,*a13(plyr_d_cflgs)
	jruc	#x

#noshoot
#tmdrone

	move	*a13(plyr_d_mode),a14
	subk	2,a14
	jrge	#inmd			;Already in mode?

	movk	2,a1			;Offense with ball
	move	a1,*a13(plyr_d_mode)
	movk	1,a1
	jruc	#setskc
#inmd
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ			>Update tob mode

	move	*a13(plyr_d_seekcnt),a1
	jrle	#notob
#setskc
	subk	1,a1
	move	a1,*a13(plyr_d_seekcnt)
	jrgt	#notob

	move	*a13(plyr_ohoopx),*a13(plyr_d_seekx)

	movi	70,a0
	callr	rndrng0
	addi	CZMID-35,a0
	move	a0,*a13(plyr_d_seeky)
#notob
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	move	*a13(plyr_seqflgs),a2
	btst	PASS_B,a2
	jrnz	#kilbuts

	move	*a13(plyr_jmpcnt),a14
	jrnz	#injmp

	move	*a11,a0
	btst	BUT1_B,a0
	jrnz	#fake			;Shoot button down?


	btst	SHOOT_B,a2
	jrnz	#injmp

	btst	DUNK_B,a2
	jrnz	#injmp

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ			>Breakaway

	move	*a13(plyr_ohpdist),a4	;A4=Hoop distance

	move	*a13(plyr_dribmode),a14
	jrn	#nodrib
#inspin

	move	*a13(plyr_num),a14	;>Chk for breakaway
	srl	1,a14
	movk	1,a0
	xor	a0,a14
	sll	6,a14			;*64
	addi	plyrproc_t,a14
	move	*a14+,a2,L
	move	*a14+,a3,L

	move	*a2(plyr_hpdist),a2
	move	*a3(plyr_hpdist),a3

	cmp	a4,a2
	jrlt	#shootrnd		;He's closer?
	cmp	a4,a3
	jrlt	#shootrnd		;He's closer?

	callr	drone_seek

	move	*a13(plyr_d_skill),a14
	addk	8,a14
	jrle	#noturb2
	ori	BUT3_M,a0		;Push turbo
	move	a0,*a11
#noturb2

	move	@game_time,a1,L
	srl	8,a1			;Remove tenths
	subk	2,a1
	jrlt	#shoot3			;Less than 2 secs?
	move	@shotimer+16,a1		;Tens
	jrnz	#scok
	move	@shotimer,a1		;Ones
	subk	2,a1
	jrlt	#shoot3			;Less than 2 secs?
#scok
	callr	drone_chk3ptr
	jrnz	#shoot3			;Need a 3?

	cmpi	50,a4
	jrlt	#shoot2			;Close?

	move	@PSTATUS,a0		;Plyr start bits 0-3
	move	*a13(plyr_num),a1
	XORK	1,a1
	btst	a1,a0
	jrnz	#x			;Teammate is a human?

	cmpi	170,a4
	jrge	#x			;Too far?

	movk	9,a0
	callr	rndrng0
	TEST	a0
	jrnz	#x

	jruc	#shoot2

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ			>Can't dribble

#nodrib
	move	*a13(plyr_seq),a0
	cmpi	SPIN_MOVE_SEQ,a0
	jreq	#inspin			;Spinning?

	subi	ELBO_SEQ,a0
	jreq	#x			;Elbows?

	subk	ELBO2_SEQ-ELBO_SEQ,a0
	jreq	#x			;Elbows?

	cmpi	240,a4
	jrlt	#shoot2

	callr	drone_pass
	jrnz	#x			;Pass OK?

	movk	>1f,a0
	callr	rnd
	jrnz	#x			;97%?
	jruc	#shoot2

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#shootrnd
	move	@game_time,a1,L
	srl	8,a1			;Remove tenths
	subk	2,a1
	jrlt	#shoot3			;Less than 2 secs?
	move	@shotimer+16,a1
	jrnz	#scok2
	move	@shotimer,a1
	subk	2,a1
	jrlt	#shoot3			;Less than 2 secs?
#scok2
	PUSH	a6,a7
	move	*a13(plyr_ohoopx),a6
	movi	CZMID,a7
	calla	seekdirdist_obxz128
	PULL	a6,a7

	move	*a13(plyr_o1dist),a14
	cmpi	80,a14
	jrgt	#o1dok			;He's too far?
	cmp	a14,a1
	jrlt	#o1dok			;I'm closer?
	move	*a13(plyr_o1dir),a2
	sub	a0,a2
	abs	a2
	cmpi	>40,a2
	jrle	#o1dsml
	subi	>80,a2
	abs	a2
#o1dsml	subk	32,a2
	jrlt	#goaround		;In front of me?
#o1dok
	move	*a13(plyr_o2dist),a14
	cmpi	80,a14
	jrgt	#o2dok			;He's too far?
	cmp	a14,a1
	jrlt	#o2dok			;I'm closer?
	move	*a13(plyr_o2dir),a2
	sub	a0,a2
	abs	a2
	cmpi	>40,a2
	jrle	#o2dsml
	subi	>80,a2
	abs	a2
#o2dsml	subk	32,a2
	jrlt	#goaround		;In front of me?
#o2dok
	jruc	#runath

#goaround				;>Opponent in my way
	move	*a13(plyr_tmdist),a0
	cmpi	80,a0
	jrlt	#goa			;Teammate too close?

	callr	drone_pass
	jrnz	#x			;Pass OK?

#goa
	movi	drnzzcnt,a2
	move	*a2,a0
	subk	1,a0
	jrgt	#zzsame

	move	*a13(plyr_dirtime),a0
	subk	8,a0
	jrle	#zz			;Too little time in dir?

	move	*a13(plyr_attrib_p),a0,L
	move	*a0(PAT_BVEL),a0	;Speed
	cmpi	BV5,a0
	jrle	#zz			;Too slow?

;	movk	1,a0			;50% spin move
;	callr	rnd
;	jrnz	#zz

	movk	2,a0
	move	a0,*a13(plyr_tbutn)

	callr	drone_seek
	ori	BUT3_M<<8|BUT3_M,a0	;Turbo
	move	a0,*a11

	jruc	#tryshot

#zz
	movk	5,a0			;New mode
	callr	rndrng0
	ANDK	3,a0
	move	a0,*a2(drnzzmode-drnzzcnt)

	movi	TSEC-10,a0
	callr	rndrng0
	addk	28,a0
#zzsame
	move	a0,*a2

	callr	drone_seek
;	jrz	#shoot2			;In position?
	sll	3,a0			;*8
	addi	#jbits_t,a0

	move	*a2(drnzzmode-drnzzcnt),a14
	sll	4+3,a14			;*16*8
	add	a14,a0
	movb	*a0,a0
	move	a0,*a11

	move	*a8(OZPOS),a1

	btst	JOYU_B,a0
	jrz	#nju
	cmpi	CZMIN+40,a1
	jrle	#xzmd			;Flip to other circle mode?
#nju
	btst	JOYD_B,a0
	jrz	#njd
	cmpi	CZMAX-40,a1
	jrlt	#njd
#xzmd
	move	*a2(drnzzmode-drnzzcnt),a3
	movk	1,a14
	xor	a14,a3
	and	a14,a3
	move	a3,*a2(drnzzmode-drnzzcnt)
#njd

	cmpi	80,a4
	jrlt	#shoot2			;Close enough for jam?

	jruc	#tryshot

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#runath					;>I have a clr path to hoop!
;	move	*a13(plyr_o1dist),a14
;	cmpi	65,a14
;	jrlt	#goa			;He's too close?
;	move	*a13(plyr_o2dist),a14
;	cmpi	65,a14
;	jrlt	#goa			;He's too close?

	callr	drone_seek
	move	*a13(plyr_d_skill),a14
	addk	7,a14
	jrle	#tryshot		;Dumb?
	ori	BUT3_M,a0		;Turbo
	move	a0,*a11


#tryshot
	cmpi	50,a4
	jrlt	#shoot2			;Close enough for jam?

	move	@PSTATUS,a0		;Plyr start bits 0-3
	move	*a13(plyr_num),a1
	XORK	1,a1
	btst	a1,a0
	jrnz	#x			;Teammate is a human?

	callr	drone_chk3ptr
	jrnz	#shoot2			;Need a 3?

	cmpi	255,a4
	jrge	#x			;Too far?

	move	*a13(plyr_tmproc_p),a0,L
	move	*a0(plyr_seqflgs),a0
	btst	DUNK_B,a0
	jrnz	#pass			;Alleyoop?


	move	@balltmshotcnt,b0
	subk	TMFIRE_MINCNT-1,b0
	jrne	#notmheat		;None heating up?

	move	@balltmscored,a1
	srl	5,a1			;0=Tm2, 1=Tm1
	move	*a13(plyr_num),a0
	srl	1,a0
	cmp	a0,a1
	jreq	#notmheat		;!My tm?

	movi	200,a0
	jruc	#rndsht
#notmheat

	movi	50,a0
	move	*a13(plyr_d_skill),a14
	addk	7,a14
	jrgt	#rndsht			;Smarter?
	movk	30,a0
#rndsht
	callr	rndrng0
	move	a0,a0
	jrnz	#x


#shoot2
	move	*a13(plyr_tmproc_p),a0,L
	move	*a0(plyr_seqflgs),a0
	btst	DUNK_B,a0
	jrz	#shoot3			;No alleyoop?
#pass
	move	*a11,a0			;>Pass
	ori	BUT2_M<<8|BUT2_M|BUT3_M,a0
	move	a0,*a11

	jruc	#x

#shoot3
	move	*a11,a0			;>Shoot
	ori	BUT1_M<<8|BUT1_M|BUT3_M,a0
	move	a0,*a11
;	movk	1,a0			;Max fakes
;	move	a0,*a13(plyr_d_seekcnt)

	jruc	#x


;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ			>Jumping but still on gnd

#fake
;	move	*a13(plyr_d_seekcnt),a2
;	jrle	#x			;No fakes?

	move	@PSTATUS,a0		;Plyr start bits 0-3
	move	*a13(plyr_num),a1
	XORK	1,a1
	btst	a1,a0
	jrnz	#x			;Teammate is a human?

	move	*a13(plyr_o1dist),a14
	cmpi	80,a14
	jrlt	#fkc			;He's close?

	move	*a13(plyr_o2dist),a14
	cmpi	80,a14
	jrge	#x			;He's far?
#fkc
	movk	>1f,a0
	callr	rnd
	jrnz	#x

	move	*a9(pld_d_lowsecagr),a14
	move	@game_time,a1,L
	srl	8,a1			;Remove tenths
	cmp	a14,a1
	jrlt	#x			;Less than x secs?
	move	@shotimer+16,a1		;Tens
	jrnz	#fk
	move	@shotimer,a1		;Ones
	cmp	a14,a1
	jrlt	#x			;Less than x secs?
#fk
;	subk	1,a2
;	move	a2,*a13(plyr_d_seekcnt)
	jruc	#kilbuts

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#injmp
	move	@PSTATUS,a0		;Plyr start bits 0-3
	move	*a13(plyr_num),a1
	XORK	1,a1
	btst	a1,a0
	jrz	#tmdrn			;Teammate is a drone?

	move	*a13(plyr_tmproc_p),a0,L
	move	*a0(PA11),a0,L
	move	*a0,a0			;Get teammates ctrl bits
	btst	BUT1_B,a0
	jrnz	#x			;Holding shoot button?
	jruc	#kilbuts
#tmdrn
	move	*a13(plyr_seqflgs),a0
	btst	BLOCKREB_B,a0
	jrnz	#kilbuts		;Got a rebound?

	btst	DUNK_B,a0
	jrz	#nodnk			;Try alleyoop?

	move	@game_time,a1,L
	srl	8,a1			;Remove tenths
	subk	2,a1
	jrlt	#shoot3			;Less than 2 secs?
	move	@shotimer+16,a1		;Tens
	jrnz	#nodnk
	move	@shotimer,a1		;Ones
	subk	2,a1
	jrlt	#shoot3			;Less than 2 secs?
#nodnk

	move	*a13(plyr_tmproc_p),a0,L
	move	*a0(plyr_seqflgs),a0
	btst	DUNK_B,a0
	jrnz	#pss			;Try alleyoop?


	move	*a13(plyr_num),a2	;>Chk for close blockers
	srl	1,a2
	XORK	1,a2
	sll	6,a2			;*64
	addi	plyrobj_t,a2

	move	*a2+,a3,L

	move	*a13(plyr_o1dist),a14
	cmpi	70,a14
	jrgt	#o1sdok			;He's too far?

	move	*a3(OYPOS),a0
	addk	20,a0
	move	*a8(OYPOS),a1
	cmp	a0,a1
	jrgt	#rndrel			;I'm lower?
#o1sdok
	move	*a13(plyr_o2dist),a14
	cmpi	70,a14
	jrgt	#kilbuts		;He's too far? Shoot

	move	*a2+,a3,L

	move	*a3(OYPOS),a0
	addk	20,a0
	move	*a8(OYPOS),a1
	cmp	a0,a1
	jrle	#kilbuts		;I'm higher, so shoot?

#rndrel
	movk	30,a0
	callr	rndrng0
	move	a0,a0
	jrz	#kilbuts		;Cause shoot?

	movk	7,a0
	callr	rnd
	jrnz	#x			;88%?

	move	*a13(plyr_ptsdown),a14
	addk	5,a14
	jrlt	#pss			;Winning by >5?
	move	*a13(plyr_seqflgs),a14
	btst	DUNK_B,a14
	jrnz	#x			;In a dunk?
	move	@game_time,a1,L
	cmpi	>200,a1
	jrlt	#x			;Less than 2 secs?
#pss
	callr	drone_pass

	jruc	#x

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ


#kilbuts
	clr	a0			;>Let go of shoot button
	move	a0,*a11

#x
	rets


#jbits_t
	.byte	0,JOYL_M,JOYR_M,0			;90~ clockwise
	.byte	JOYD_M,JOYD_M|JOYL_M,JOYD_M|JOYR_M,0
	.byte	JOYU_M,JOYU_M|JOYL_M,JOYU_M|JOYR_M,0
	.byte	0,0,0,0

	.byte	0,JOYR_M,JOYL_M,0			;90~ cntr clkwise
	.byte	JOYU_M,JOYU_M|JOYR_M,JOYU_M|JOYL_M,0
	.byte	JOYD_M,JOYD_M|JOYR_M,JOYD_M|JOYL_M,0
	.byte	0,0,0,0

	.byte	0,JOYD_M|JOYL_M,JOYU_M|JOYR_M,0		;135~ clkwise
	.byte	JOYD_M|JOYR_M,JOYD_M,JOYR_M,0
	.byte	JOYU_M|JOYL_M,JOYL_M,JOYU_M,0
	.byte	0,0,0,0

	.byte	0,JOYD_M|JOYR_M,JOYU_M|JOYL_M,0		;135~ cntr clkwise
	.byte	JOYU_M|JOYR_M,JOYR_M,JOYU_M,0
	.byte	JOYD_M|JOYL_M,JOYD_M,JOYL_M,0
	.byte	0,0,0,0


#*******************************
* Check if this drone needs a 3 ptr
* A4 = Distance from opponents hoop
*>A0 = !0 if needed (CC)
* Trashes scratch

 SUBRP	drone_chk3ptr

	cmpi	290,a4
	jrgt	#x0			;Too far?

	move	*a13(plyr_num),a1
	move	@PSTATUS,a0		;Plyr start bits 0-3
	movk	1,a14
	xor	a14,a1
	btst	a1,a0
	jrnz	#x0			;Teammate is a human?

	xor	a14,a1
	move	@plyr_onfire,a0
	btst	a1,a0
	jrnz	#x1			;I'm on fire?

	cmpi	230,a4
	jrlt	#x0			;Too close?

	XORK	2,a1
	btst	a1,a0
	jrnz	#x0			;Opp 1 on fire?
	XORK	1,a1
	btst	a1,a0
	jrnz	#x0			;Opp 2 on fire?


	movk	6,a1
	move	@game_time,a0,L
	cmpi	>1010000,a0
	jrgt	#hvtime			;Enough time?
	movk	3,a1
#hvtime
	move	*a13(plyr_ptsdown),a14
	cmp	a1,a14
	jrlt	#x0

	cmpi	>40000,a0
	jrlt	#x1			;Less than 40 secs?

	move	*a13(plyr_o1dist),a14
	cmpi	70,a14
	jrlt	#rndsht			;He's close?
	move	*a13(plyr_o2dist),a14
	cmpi	70,a14
	jrge	#x1			;He's far?
#rndsht
	movk	8,a0
	callr	rndrng0
	move	a0,a0
	jrnz	#x0

#x1
	addk	1,a0
	rets
#x0
	clr	a0
	rets


#*******************************
* Drone in offense with out ball
* A8 = *Obj
* A9 = *Plyr secondary data
* A11= *Ctrl bits
* A13= *Plyr process
* Trashes scratch, A2-A5

 SUBRP	drone_offwoball

	clr	a0
	move	a0,*a13(plyr_d_cflgs)

	callr	drone_getcurskillo
	move	a0,a5				;A5=Skill offset


	movk	1,a4				;A4=Fire flag (+=Me, -=Tm)
	move	*a13(plyr_num),a0
	move	@plyr_onfire,a1
	btst	a0,a1
	jrnz	#mefr				;I'm on fire?

	subk	2,a4				;=-1

	XORK	1,a0
	btst	a0,a1
	jrnz	#mefr				;Tm on fire?
	clr	a4
#mefr


;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	move	*a13(plyr_d_mode),a14
	subk	1,a14
	jreq	#inmd				;Already in mode?

	movk	1,a0				;Offense wo ball
	move	a0,*a13(plyr_d_mode)

	move	a5,a0
	addi	#mdsk_t,a0
	move	*a0,a0
	callr	rndrng0
	addk	1,a0
	move	a0,*a13(plyr_d_seekcnt)
#inmd
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ


	move	*a13(plyr_dir),a3

	move	*a13(plyr_o1dist),a14
	subi	50,a14
	jrgt	#o1far				;Too far?
	move	*a13(plyr_o1dir),a2
	sub	a3,a2
	abs	a2
	cmpi	>40,a2
	jrle	#o1dsml
	subi	>80,a2
	abs	a2
#o1dsml	subk	16,a2
	jrlt	#pusho				;In front of me?
#o1far
	move	*a13(plyr_o2dist),a14
	subi	50,a14
	jrgt	#nopush				;Too far?
	move	*a13(plyr_o2dir),a2
	sub	a3,a2
	abs	a2
	cmpi	>40,a2
	jrle	#o2dsml
	subi	>80,a2
	abs	a2
#o2dsml	subk	16,a2
	jrge	#nopush				;!In front?
#pusho
	movi	99,a0
	callr	rndrng0

	move	a5,a14
	addi	#p_t,a14
	move	*a14,a1

	TEST	a4
	jrle	#pshnf				;No fire?
	addk	30,a1				;Push alot more!
#pshnf
	move	@balltmshotcnt,b0
	subk	TMFIRE_MINCNT-1,b0
	jrlt	#noth				;None heating up?
	addk	20,a1				;Push more!
#noth
	cmp	a1,a0
	jrge	#newseek			;Skip push?

	.if	NOPUSHSTEAL
	jruc	#x
	.endif

	move	*a11,a0				;Push
	ori	BUT2_M<<8|BUT2_M|BUT3_M,a0
	move	a0,*a11

	jruc	#x

#nopush
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ				>Try dunk wo ball

	move	*a13(plyr_ohpdist),a3
	cmpi	65,a3
	jrle	#noalyo				;Too close?
	cmpi	180,a3
	jrgt	#noalyo				;Too far?

	TEST	a4
	jrnz	#noalyo				;We on fire?

	move	a5,a14
	addi	#aly_t,a14
	move	*a14,a2

	move	*a13(plyr_tmproc_p),a1,L
	move	*a1(plyr_seqflgs),a14
	btst	DUNK_B,a14
	jrz	#norm				;Tm not dunking?

	move	*a1(plyr_slam_ticks),a14
	move	*a1(plyr_jmpcnt),a1
	sub	a1,a14
	subk	20,a14
	jrle	#noalyo				;Not enough time?

;	clr	a0
#norm
	move	*a13(plyr_d_seeky),a0
	cmpi	CZMID+1,a0
	jrne	#rndaly				;!in allyo seek?

	callr	drone_seek
	ori	BUT1_M<<8|BUT1_M|BUT3_M,a0	;Turbo shoot
	move	a0,*a11

;	move	*a13(plyr_tbutn),a0
;	subk	30,a0
;	jrle	#x

	jruc	#x

#rndaly
	move	@balltmshotcnt,b0
	subk	TMFIRE_MINCNT-1,b0
	jrlt	#notmheat			;None heating up?

;	move	@balltmscored,a1
;	srl	5,a1				;0=Tm2, 1=Tm1
;	move	*a13(plyr_num),a0
;	srl	1,a0
;	cmp	a0,a1
;	jreq	#notmheat			;!My tm?

	sll	1,a2				;% * 2
#notmheat

	movi	99,a0
	callr	rndrng0
	cmp	a2,a0
	jrge	#noalyo

	cmpi	80,a3
	jrle	#noalyo				;Too close?

	move	*a13(plyr_PDATA_p),a1,L
	move	*a1(ply_turbo),a14
	subk	3,a14
	jrle	#noalyo				;No turbo?

	move	*a13(plyr_ohoopx),a0
	movi	CZMID+1,a1
	move	a0,*a13(plyr_d_seekx)
	move	a1,*a13(plyr_d_seeky)

	callr	drone_seekxy

	jruc	#x

#noalyo

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ				>Seek

;	movi	>7f,a0
;	callr	rnd
;	jrz	#newseek

	move	*a13(plyr_d_seeky),a0
	cmpi	CZMID+1,a0
	jreq	#newseek			;Failed allyo?

	move	*a13(plyr_d_seekcnt),a0
	subk	1,a0
	jrgt	#seek

#newseek
;	move	*a13(plyr_newdir),a0
;	jrnn	#contsk				;Turning?

	movk	16-1,a0

	TEST	a4
	jrle	#no3				;I'm not on fire?
	movk	7-1,a0
#no3
	.if	ONLY3
	movk	7-1,a0
	.endif

	callr	rndrng0
	sll	5,a0				;*32
	addi	#seek_t,a0

	move	*a0+,a1

	move	*a13(plyr_ohoopx),a14
	cmpi	WRLDMID,a14
	jrlt	#lft
	neg	a1
#lft
	add	a1,a14
	move	a14,*a13(plyr_d_seekx)

	move	*a0+,a1
	move	a1,*a13(plyr_d_seeky)

	movi	TSEC*3/2,a0
	callr	rndrng0
	addk	TSEC/2,a0

#seek
	move	a0,*a13(plyr_d_seekcnt)
#contsk
	callr	drone_seek
	jrnz	#notthere

	movk	>1f,a0				;3%
	callr	rnd
	jrnz	#x

	clr	a0
	move	a0,*a13(plyr_d_seekcnt)

#notthere
	TEST	a4
	jrle	#notur				;I'm not on fire?

	move	*a11,a0
	ori	BUT3_M,a0			;+turbo
	move	a0,*a11
#notur

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#x
	rets


#mdsk_t					;Mode switch max seek time
	.word	50,50,50,50,50		;Up 15-11
	.word	40,40,40,35,30		;10-6
	.word	25,22,20,18,16		;5-1
	.word	14			;Even score
	.word	10,8,6,4,4		;Dn 1-5
	.word	4,3,3,3,2		;6-10
	.word	2,2,2,2,1		;11-15

#p_t					;% to push
	.word	1,1,1,1,1
	.word	2,2,2,2,3
	.word	3,3,4,4,5
	.word	5
	.word	5,6,8,10,13
	.word	15,17,18,20,20
	.word	25,30,35,40,50

	.asg	10,N
#aly_t					;% to jump at backboard
	.word	1,2,3,4,5
	.word	N+05,N+10,N+15,N+15,N+20
	.word	N+20,N+20,N+20,N+22,N+25
	.word	N+25
	.word	N+25,N+26,N+28,N+30,N+35
	.word	N+40,N+45,N+50,N+55,N+60
	.word	N+65,N+70,N+75,N+90,N+99


	.asg	CZMID,Z
#seek_t
	.word	0,Z-150, 80,Z-150, 200,Z-100	;3ptrs
	.word	255,Z
	.word	200,Z+115, 80,Z+190, 0,Z+190

	.word	0,Z-100, 50,Z-90, 100,Z-80	;2ptrs
	.word	150,Z
	.word	100,Z+100, 50,Z+110, 0,Z+120

	.word	30,Z-40, 30,Z+40


#*******************************
* Drone code - pass if clear
* A8  = *Obj
* A11 = *Ctrl bits
* A13 = *Plyr process
*>A0 = !0 if pass OK (CC)
* Trashes scratch

 SUBRP	drone_pass

	move	@PSTATUS,a0			;Plyr start bits 0-3
	move	*a13(plyr_num),a1
	XORK	1,a1
	btst	a1,a0
	jrnz	#x				;Teammate is a human?

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	move	*a13(plyr_tmdist),a0
	addk	30,a0

	move	*a13(plyr_o1dist),a1		;>Chk if o1 in my way
	cmp	a1,a0
	jrlt	#o1ok

	move	*a13(plyr_tmdir),a14
	move	*a13(plyr_o1dir),a1
	sub	a14,a1
	abs	a1
	cmpi	64,a1
	jrle	#dsml
	subi	128,a1
	abs	a1
#dsml
	subk	16,a1
	jrlt	#inway
#o1ok

	move	*a13(plyr_o2dist),a1		;>Chk if o2 in my way
	cmp	a1,a0
	jrlt	#o2ok

	move	*a13(plyr_tmdir),a14
	move	*a13(plyr_o2dir),a1
	sub	a14,a1
	abs	a1
	cmpi	64,a1
	jrle	#dsml2
	subi	128,a1
	abs	a1
#dsml2
	subk	16,a1
	jrlt	#inway
#o2ok

	move	@ballnumscored,b0
	subk	ONFIRE_MINCNT-1,b0
	jrlt	#noheat				;None heating up?

	move	@ballpnumscored,a1
	move	*a13(plyr_num),a0
	cmp	a0,a1
	jrne	#noheat				;!Me?

	move	*a13(plyr_ohpdist),a0
	cmpi	350,a0
	jrlt	#x				;Too close? Don't pass
#noheat

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
#iwpass
	move	*a13(plyr_tmproc_p),a1,L
#tmclos
	move	*a1(plyr_seqflgs),a0
	btst	DUNK_B,a0
	jrnz	#pass				;Tm dunking?

	move	*a1(plyr_seq),a0
	subk	RUNDRIBTURB_SEQ,a0
	jrhi	#x				;Tm is doing something?
#pass
	move	*a11,a0				;>Pass
	ori	BUT2_M<<8|BUT2_M|BUT3_M,a0
	move	a0,*a11

	rets

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#inway
	move	@ballnumscored,b0
	subk	ONFIRE_MINCNT-1,b0
	jrlt	#noheatiw			;None heating up?

	move	@ballpnumscored,a1
	move	*a13(plyr_num),a0
	XORK	1,a0
	cmp	a0,a1
	jrne	#noheatiw			;!Tm?

	move	*a13(plyr_tmproc_p),a1,L
	move	*a1(plyr_ohpdist),a0
	cmpi	300,a0
	jrlt	#iwpass				;He's Close? Risk pass

#noheatiw
	move	*a13(plyr_ohpdist),a1
	cmpi	250,a1
	jrlt	#x				;I'm close to hoop?

	move	*a13(plyr_tmproc_p),a1,L
	move	*a1(plyr_ohpdist),a0
	cmpi	240,a0
	jrlt	#tmclos				;Teammate is close to hoop?

#x
	clr	a0
	rets



#*******************************
* Drone code - defense
* A8 = *Obj
* A9 = *Plyr secondary data
* A11= *Ctrl bits
* A13= *Plyr process

 SUBRP	drone_defense

	PUSH	a6,a7,a10

	clr	a0
	move	a0,*a13(plyr_d_cflgs)


	move	@ballpnum,a5
	sll	5,a5
	addi	plyrproc_t,a5
	move	*a5,a5,L		;A5=*Proc of opponent with ball


	callr	drone_getcurskillo
	move	a0,a7			;A7=Ptsdn+skill for indexing (*16)


	movk	1,a6			;A6=Fire flag (+=Me, -=Tm)
	move	*a13(plyr_num),a0
	move	@plyr_onfire,a1
	btst	a0,a1
	jrnz	#mefr			;I'm on fire?

	subk	2,a6			;=-1

	XORK	1,a0
	btst	a0,a1
	jrnz	#mefr			;Tm on fire?
	clr	a6
#mefr


;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ			>Mode

	move	*a13(plyr_d_mode),a14
	jrn	#inmd			;Already in mode?

	not	a14
	move	a14,*a13(plyr_d_mode)	;Neg

	move	a7,a14
	addi	#mdsk_t,a14
	move	*a14,a0
	move	a0,a2
	srl	2,a2			;/4

	callr	rndrng0
	addk	1,a0
	add	a2,a0
	move	a0,*a13(plyr_d_seekcnt)

	clr	a0
	move	a0,*a9(pld_d_nastycnt)

	movk	30,a0
	callr	rndrng0

	addi	190-20,a0
	move	a0,*a9(pld_d_grddist)
#inmd

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ			>Update nasty mode

	move	*a9(pld_d_lowsecagr),a0

	movk	2,a10
	move	@game_time,a14,L
	srl	8,a14			;Remove tenths
	cmp	a0,a14
	jrlt	#nasty			;Less than x secs?

	move	@gmqrtr,a2
	subk	3,a2
	jrlt	#chkst
	move	*a13(plyr_ptsdown),a1
	addk	3,a1
	jrle	#chkst			;Winning by 3 or more?
	srl	8,a14			;Remove ones
	sll	1,a14			;*2
	cmp	a0,a14
	jrlt	#nasty			;Less than x0 secs?
#chkst
	move	@shotimer+16,a14	;Tens
	jrnz	#scok
	move	@shotimer,a14		;Ones
	srl	1,a0			;/2
	cmp	a0,a14
	jrlt	#nasty			;Less than x secs?
#scok
	move	*a9(pld_d_nastycnt),a10
	jrgt	#naston

	movk	10,a10
	movi	0ffh,a0
	callr	rnd
	jrz	#nasty

	clr	a10

	movi	999,a0
	callr	rndrng0

	move	a7,a14
	addi	#nast_t,a14
	move	*a14,a1
	cmp	a1,a0
	jrge	#nonast			;No nasty?

	movi	TSEC-20,a0
	callr	rndrng0
	addk	20,a0
	move	a0,a10
#naston
	subk	1,a10
#nasty
	move	a10,*a9(pld_d_nastycnt)

	cmpi	10,a10
	jreq	#newsk
#nonast

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	move	*a13(plyr_d_seekcnt),a0
	subk	1,a0
	jrgt	#seek
#newsk
	move	*a13(plyr_num),a2
	XORK	2,a2
	move	a2,a4
	sll	5,a4			;*32
	addi	plyrproc_t,a4
	move	*a4,a4,L
	cmp	a5,a4
	jreq	#guard			;I'm on guy with ball?

	move	*a5(plyr_ohpdist),a0
	cmpi	300,a0
	jrgt	#guard			;Too far to worry about?

;	move	*a5(plyr_seqflgs),a0
;	btst	DUNK_B,a0
;	jrnz	#gbc			;He's dunking?

	move	*a13(plyr_tmproc_p),a3,L
	move	*a3(plyr_seq),a0
	subi	STAGGER_SEQ,a0
	jrls	#tmok
	subk	FLYBACKWB2_SEQ-STAGGER_SEQ,a0
	jrls	#gbc			;Teammate staggered?
#tmok
	move	*a3(plyr_o1dist),a14
	move	*a3(plyr_o1dir),a1
	btst	0,a2
	jrnz	#p1
	move	*a3(plyr_o2dist),a14
	move	*a3(plyr_o2dir),a1
#p1
	move	*a3(plyr_hpdir),a0	;Find dir difference
	sub	a1,a0
	abs	a0
	cmpi	>40,a0
	jrle	#dsml
	subi	>80,a0
	abs	a0
#dsml	subk	28,a0
	jrle	#gbc			;TM not between op and hoop?

	cmpi	160,a14
	jrle	#guard			;TM guarding?

#gbc
	move	a5,a4			;Guard ball carrier
#guard

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ			>Seek

	move	*a4(PA8),a2,L		;*Obj

	move	*a2(OXPOS),a0
	move	*a2(OXANI+16),a14
	add	a14,a0			;X
	move	*a2(OZPOS),a1		;Z

	move	*a2(OXVEL),a14,L
	sra	16-4,a14		;16 ticks from now
	add	a14,a0
	move	*a2(OZVEL),a14,L
	sra	16-4,a14		;16 ticks from now
	add	a14,a1


	TEST	a10
	jrgt	#setseek		;Nasty on?


	move	*a5(plyr_seqflgs),a14
	btst	SHOOT_B,a14
	jrz	#nosh			;!Starting a shot?

	TEST	a6
	jrgt	#nosh			;I'm on fire?

	move	*a13(plyr_balldist),a14
	cmpi	70,a14
	jrle	#setseek		;In his face?

#nosh
	move	*a13(plyr_myhoopx),a2	;Stay near oplyr between my basket
	movi	CZMID,a3

	sub	a2,a0
	sub	a3,a1

	move	*a9(pld_d_grddist),a14

	TEST	a6
	jrle	#nofire			;I'm not on fire?

	movi	77,a14			;30%

	move	a4,b0
	move	*b0(plyr_ohpdist),b0
	cmpi	400,b0
	jrge	#nofire			;Opp far from my hoop?
	movi	154,a14			;60%
#nofire
	mpys	a14,a1
	sra	8,a1			;/256
	add	a1,a3

	move	a0,a1
	mpys	a14,a1
	sra	8,a1			;/256
	add	a1,a2

	move	a2,a0
	move	a3,a1

#setseek
	move	a0,*a13(plyr_d_seekx)
	move	a1,*a13(plyr_d_seeky)

	movk	25,a0
	move	*a4(plyr_ohpdist),a1
	cmpi	320,a1
	jrge	#seek			;Opp far from my hoop?

	move	a7,a14
	addi	#skt_t,a14
	move	*a14,a0
	srl	1,a0			;/2
	move	a0,a2
	callr	rndrng0
	add	a2,a0

#seek
	move	a0,*a13(plyr_d_seekcnt)

	callr	drone_seek
	move	a0,a2

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ			>Turbo if opp closer to my basket

	move	*a13(plyr_num),a14	;>Get opponents proc
	addk	2,a14
	sll	32-2,a14
	srl	32-2-5,a14		;*32
	addi	plyrproc_t,a14
	move	*a14,a0,L

	move	*a0(plyr_ohpdist),a0
	subk	10,a0
	cmpi	300,a0
	jrgt	#onclose		;!close?
	subi	60,a0			;Turbo earlier
#onclose
	move	*a13(plyr_hpdist),a1
	cmp	a0,a1
	jrlt	#icloser

	TEST	a2
	jrz	#icloser		;I'm not moving?
	addi	BUT3_M,a2		;Turbo
#icloser

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ			>Push/steal

	move	*a13(plyr_balldist),a1
	cmpi	35,a1
	jrgt	#psrnd			;!In his face?

	move	@PCNT,a0
	sll	32-1,a0
	jrnz	#skipsp			;Skip 50%?

	move	a10,a10
	jrgt	#ps			;Nasty on?

#psrnd
	move	a7,a14
	addi	#dist_t,a14
	move	*a14,a0
	cmp	a0,a1
	jrgt	#skipsp			;Ball too far?

	movi	999,a0
	callr	rndrng0

	move	a7,a14
	addi	#ps_t,a14
	move	*a14,a1
	cmp	a1,a0
	jrge	#skipsp			;Skip push?
#ps
	movk	1,a0
	callr	rnd
	jrnz	#push			;50%?


	move	*a5(plyr_jmpcnt),a1
	jrnz	#push			;Plyr with ball is in air?

	sll	32-4,a2
	srl	32-4,a2
	addk	BUT2_M,a2		;Steal
	jruc	#x

#push
	move	*a5(PA8),a0,L
	move	*a0(OYPOS),a1
	move	*a5(plyr_aniy),a14
	add	a14,a1			;His feet Y position
	move	*a8(OYPOS),a0
	sub	a1,a0
	addk	10,a0
	jrgt	#skipsp			;Feet above my shoulders?

	ori	BUT2_M<<8|BUT2_M|BUT3_M,a2
	jruc	#x

#skipsp

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ			>Chk if I can block ball

	move	*a13(plyr_balldist),a14
	cmpi	65,a14
	jrge	#noblk

	move	*a5(plyr_seqflgs),a4


	btst	DUNK_B,a4
	jrz	#nodnk

	cmpi	35,a14
	jrgt	#noblk

	move	*a13(plyr_tmproc_p),a3,L

	move	*a3(plyr_balldist),a14
	cmpi	45,a14
	jrgt	#tryblk			;Teammate far?

	move	*a3(plyr_jmpcnt),a1
	jrz	#tryblk			;Tm on gnd?

	move	@PCNT,a0
	sll	32-3,a0
	jrnz	#noblk			;Skip 88%?

	jruc	#tryblk

#nodnk
	btst	SHOOT_B,a4
	jrz	#noblk			;!Starting a shot?

	move	*a5(plyr_jmpcnt),a1
	jrnz	#tryblk			;Plyr with ball is in air?

	movk	11,a0
	callr	rndrng0
	TEST	a0
	jrnz	#noblk			;92% ignore?
	jruc	#blk

#tryblk
	movi	99,a0
	callr	rndrng0
	move	a7,a14
	addi	#blk_t,a14
	move	*a14,a1

	btst	DUNK_B,a4
	jrz	#nd
	sra	1,a1			;Dunk % /2
#nd
	TEST	a10
	jrle	#nstoff			;Nasty off?
	sll	1,a1			;%*2
#nstoff
	cmp	a1,a0
	jrge	#noblk

#blk
	sll	32-4,a2
	srl	32-4,a2
	addi	BUT1_M<<8|BUT1_M|BUT3_M,a2 ;Block
#noblk

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
#x
	.if	NOPUSHSTEAL
	andi	0dfh,a2
	.endif

	move	a2,*a11


	PULL	a6,a7,a10
	rets


#mdsk_t					;Mode switch max seek time
	.word	50,50,50,50,50		;Up 15-11
	.word	50,50,45,45,40		;10-6
	.word	33,25,22,18,16		;5-1
	.word	14			;Even score
	.word	10,8,6,4,4		;Dn 1-5
	.word	4,3,3,3,2		;6-10
	.word	2,2,2,2,1		;11-15

	.asg	0,N
#nast_t					;% to become nasty
	.word	0,0,0,0,0
	.word	N+1,N+1,N+2,N+2,N+2
	.word	N+3,N+3,N+3,N+4,N+5
	.word	N+5
	.word	N+6,N+7,N+8,N+9,N+10
	.word	N+12,N+13,N+14,N+15,N+16
	.word	N+18,N+20,N+24,N+28,N+30

	.asg	25,N
#skt_t					;Max seek time (75% avrg)
	.word	N+55,N+55,N+55,N+55,N+52
	.word	N+48,N+44,N+40,N+36,N+33
	.word	N+30,N+29,N+28,N+27,N+26
	.word	N+25			;Even score
	.word	N+24,N+23,N+21,N+19,N+17
	.word	N+14,N+11,N+08,N+06,N+03
	.word	N+01,N-01,N-03,N-05,N-10
#dist_t					;Max dist to try push/steal
	.word	110,100,100,100,100
	.word	90,90,80,80,80
	.word	80,70,70,60,60
	.word	60
	.word	50,50,50,50,50
	.word	50,50,50,50,50
	.word	50,50,50,50,50
#ps_t					;% to push/steal
	.word	1,2,2,2,2
	.word	3,3,3,3,3
	.word	4,4,4,5,5
	.word	6
	.word	6,6,8,10,13
	.word	15,17,18,20,30
	.word	40,60,80,150,250
#blk_t					;% to block
	.word	1,1,2,3,3
	.word	3,3,4,4,5
	.word	6,7,8,10,12
	.word	14
	.word	16,18,20,25,30
	.word	35,40,45,50,50
	.word	50,50,50,50,50


#*******************************
* Setup drones for ball takeout
* Trashes scratch

 SUBR	drone_setuptob

	PUSH	a2,a3,a4,a13

	movk	4,a4
	movi	plyrproc_t,a3

#lp
	move	*a3+,a13,L
	move	*a13(PA9),a2,L

	movk	1,a0
	move	*a13(plyr_ownball),a14
	jrz	#def			;Defense?
					;>Setup offense
	jrn	#wob
	movk	2,a0
#wob
	move	a0,*a13(plyr_d_mode)

	movi	TSEC-10,a0
	callr	rndrng0
	addk	5,a0
	move	a0,*a13(plyr_d_seekcnt)

	movk	9-1,a0
	callr	rndrng0
	sll	5,a0			;*32
	addi	#seek_t,a0

	move	*a0+,a1
	move	*a13(plyr_num),a14
	subk	2,a14
	jrlt	#lft
	neg	a1
#lft
	addi	WRLDMID,a1
	move	a1,*a13(plyr_d_seekx)

	move	*a0+,a1
	move	a1,*a13(plyr_d_seeky)

	jruc	#nxt


#def					;>Setup defense
	movi	-1,a14
	move	a14,*a13(plyr_d_mode)	;Defense

	clr	a0
	move	a0,*a2(pld_d_nastycnt)

	movi	190,a0
	move	a0,*a2(pld_d_grddist)

	movk	4,a0
	callr	rndrng0
	addk	5,a0
	move	a0,*a2(pld_d_lowsecagr)	;5-9

	callr	drone_getcurskillo

	addi	#mdsk_t,a0
	move	*a0,a0
	callr	rndrng0
	addk	1,a0
	move	a0,*a13(plyr_d_seekcnt)

#nxt
	dsj	a4,#lp

	PULL	a2,a3,a4,a13
	rets


	.asg	CZMID,Z
#seek_t
	.word	-260,Z-160, -200,Z-150,	-100,Z-130, -50,Z-100
	.word	0,Z
	.word	-260,Z+160, -200,Z+150,	-100,Z+130, -50,Z+100


#mdsk_t					;Mode switch max seek time
	.word	30,30,30,25,25		;Up 15-11
	.word	25,25,20,20,20		;10-6
	.word	20,18,16,14,12		;5-1
	.word	10			;Even score
	.word	8,7,6,5,4		;Dn 1-5
	.word	4,3,3,2,2		;6-10
	.word	2,1,1,1,1		;11-15


#*******************************
* Drone code - nobody has ball
* A8 = *Obj
* A11= *Ctrl bits
* A13= *Plyr process

 SUBRP	drone_chaseball

	move	*a13(plyr_rcvpass),a14
	jrgt	drone_offwoball		;I'm rcving ball?

	move	*a13(plyr_tmproc_p),a4,L
	move	*a4(plyr_rcvpass),a14
	jrgt	drone_offwoball		;Teammate rcving ball?


;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ			>Mode

	clr	a0
	move	a0,*a13(plyr_d_cflgs)

	move	*a13(plyr_d_mode),a14
	jrz	#inmd			;Already in mode?

	move	a0,*a13(plyr_d_mode)

	callr	drone_getcurskillo
	addi	#mdsk_t,a0
	move	*a0,a0

	callr	rndrng0
	addk	2,a0
	move	a0,*a13(plyr_d_seekcnt)

#inmd
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ			>Seek

	move	*a13(plyr_d_seekcnt),a0
	subk	1,a0
	jrgt	#seek

	move	@ballobj_p,a5,L


	move	*a13(plyr_num),a14	;>Chk for pass
	srl	1,a14
	movk	1,a0
	xor	a0,a14
	sll	6,a14			;*64
	addi	plyrproc_t,a14

	move	*a14+,a1,L
	move	*a1(plyr_rcvpass),a0
	jrgt	#chaseb			;Opp rcving ball?

	move	*a14+,a1,L
	move	*a1(plyr_rcvpass),a0
	jrgt	#chaseb			;Opp rcving ball?


	move	*a4(plyr_seq),a0
	subi	STAGGER_SEQ,a0
	jrls	#tmok
	subk	FLYBACKWB2_SEQ-STAGGER_SEQ,a0
	jrls	#chaseb			;Teammate staggered?
#tmok
	move	*a4(plyr_jmpcnt),a14
	jrnz	#chaseb			;Tm in air?

	movi	CZMID,a1

;FIX! Also if team almost on fire..
	move	*a13(plyr_num),a0
	move	@plyr_onfire,a2
	btst	a0,a2
	jrz	#nof			;I'm not on fire?

	XORK	1,a0
	btst	a0,a2
	jrz	#skf			;Tm not on fire?

	move	*a13(plyr_balldist),a0
	move	*a4(plyr_balldist),a14
	cmp	a14,a0
	jrle	#chaseb			;I'm closer?
#skf
	move	*a13(plyr_myhoopx),a0	;Go in front of my hoop for goaltend
	move	a0,a14
	subi	WRLDMID,a14
	sra	4,a14			;/16
	sub	a14,a0
	jruc	#setxz

#nof
	XORK	1,a0
	btst	a0,a2
	jrnz	#chaseb			;Tm on fire? I get ball

	move	*a13(plyr_balldist),a0
	move	*a4(plyr_balldist),a14
	cmp	a14,a0
	jrle	#chaseb			;I'm closer?

	move	*a5(OYVEL+16),a0
	jrlt	#chaseb			;Going up?

	move	*a13(plyr_ohoopx),a0	;Go for opponents top of 3 pt line
	subi	WRLDMID,a0
	sra	2,a0			;/4
	addi	WRLDMID,a0
	jruc	#setxz

#chaseb
	move	*a5(OXPOS),a0
	move	*a5(OXANI+16),a14
	add	a14,a0
	move	*a5(OXVEL),a14,L
	sra	16-4,a14		;16 ticks from now
	add	a14,a0

	move	*a5(OZPOS),a1
	move	*a5(OZVEL),a14,L
	sra	16-4,a14		;16 ticks from now
	add	a14,a1
#setxz
	move	a0,*a13(plyr_d_seekx)
	move	a1,*a13(plyr_d_seeky)


	callr	drone_getcurskillo
	addi	#skt_t,a0
	move	*a0,a0

	callr	rndrng0
	addk	5,a0

#seek
	move	a0,*a13(plyr_d_seekcnt)

	callr	drone_seek
	jrz	#sk0
	ori	BUT3_M,a0		;Turbo
	move	a0,*a11
#sk0

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ			>Chk if I can jump at ball or steal


	move	*a13(plyr_balldist),a0
	cmpi	100,a0
	jrgt	#nojmp

	move	@ballobj_p,a5,L


	move	*a5(OXPOS),a0		;>Calc distance (long+short/2.667)
	move	*a5(OXANI+16),a14
	add	a14,a0
	move	*a5(OXVEL),a14,L
	sra	16-4,a14		;16 ticks from now
	add	a14,a0

	move	*a5(OZPOS),a1
	move	*a5(OZVEL),a14,L
	sra	16-4,a14		;16 ticks from now
	add	a14,a1

	move	*a8(OXPOS),a2
	move	*a8(OXANI+16),a14
	add	a14,a2
	move	*a8(OZPOS),a3

	sub	a0,a2
	abs	a2
	sub	a1,a3
	abs	a3

	cmp	a2,a3
	jrge	#a3bg
	SWAP	a2,a3
#a3bg
	srl	1,a2			;Shorter/2
	add	a2,a3
	srl	2,a2			;Shorter/8
	sub	a2,a3			;A3=Dist in 16 ticks


	cmpi	60,a3
	jrgt	#nojmp


	move	@balltmshotcnt,b0
	subk	TMFIRE_MINCNT-1,b0
	jrne	#notmheat		;None heating up?

	move	@gmqrtr,b0
	subk	3,b0
	jrlt	#tmhqok			;Q123?

	move	@game_time,a14,L
	srl	8+8,a14			;Remove one & tenths
	subk	6,a14
	jrlt	#notmheat		;Less than 60 secs?
#tmhqok
	move	@balltmscored,a1
	srl	5,a1			;0=Tm2, 1=Tm1
	move	*a13(plyr_num),a0
	srl	1,a0
	cmp	a0,a1
	jrne	#ifire			;My tm?
#notmheat

	move	*a13(plyr_num),a1
	move	@plyr_onfire,a0
	btst	a1,a0
	jrnz	#ifire			;I'm on fire?

	XORK	1,a1
	btst	a1,a0
	jrz	#nofire			;Tm not on fire?
#ifire
	move	*a13(plyr_hpdist),a14
	cmpi	150,a14
	jrge	#nofire			;Too far for goaltend?

	move	*a8(OYPOS),a0
	move	*a5(OYPOS),a1
	sub	a1,a0
	subk	20,a0
	jrle	#nofire			;Ball too low?

	subi	160-20,a0
	jrgt	#nojmp			;Ball too high?

	move	*a5(OYVEL+16),a0
	jrgt	#j			;Going down? Goaltend!
	jruc	#nojmp

#nofire
	move	*a8(OYPOS),a0
	move	*a5(OYPOS),a1
	sub	a1,a0
	subk	10,a0
	jrgt	#nojmp			;Ball too high?

	addk	32,a1
	jrge	#nojmp			;Ball close to gnd?

	movk	7,a0
	callr	rnd
	jrnz	#nojmp			;87%?

	move	*a11,a2
	sll	32-4,a2
	srl	32-4,a2
	addk	BUT2_M,a2		;Steal
	move	a2,*a11
	jruc	#nojmp

#j
	move	*a11,a0			;Jmp
	ori	BUT1_M<<8|BUT1_M|BUT3_M,a0
	move	a0,*a11
#nojmp
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	rets


#mdsk_t					;Mode switch max seek time
	.word	50,50,50,50,50		;Up 15-11
	.word	50,50,40,40,40		;10-6
	.word	33,25,22,18,16		;5-1
	.word	14			;Even score
	.word	10,8,6,4,4		;Dn 1-5
	.word	4,3,3,3,2		;6-10
	.word	2,2,2,2,1		;11-15

#skt_t					;Max seek time
	.word	50,50,45,45,45
	.word	40,40,30,30,22
	.word	20,17,16,15,15
	.word	15
	.word	15,14,14,13,13
	.word	12,12,12,12,12
	.word	11,11,11,10,10


#*******************************
* Push stick to move drone towards his seek position

 SUBRP	drone_seek

	move	*a13(plyr_d_seekx),a0
	move	*a13(plyr_d_seeky),a1

#*******************************
* Push stick to move drone towards an XZ location
* A0 = X to seek
* A1 = Z
* A8 = *Obj
* A11= *Ctrl bits
*>A0 = Joy bits set or 0 (Pass CC)
* Trashes scratch

 SUBRP	drone_seekxy

	move	a2,b0

	move	*a8(OXPOS),a2
	move	*a8(OXANI+16),a14
	add	a14,a2
	sub	a0,a2

	clr	a0

	move	a2,a14
	abs	a2
	subk	10,a2
	jrle	#onx
	move	a14,a14
	jrlt	#nolft
	subk	4,a0			;Left

#nolft	addk	8,a0			;Rgt
#onx
	move	*a8(OZPOS),a2

	sub	a1,a2
	move	a2,a14
	abs	a2
	subk	10,a2
	jrle	#onz
	move	a14,a14
	jrlt	#noup
	subk	1,a0			;Up

#noup	addk	2,a0			;Dn
#onz
	move	a0,*a11

	move	b0,a2
	move	a0,a0
	rets


#*******************************
* Get the current skill offset
*>A0 = Offset (0-30) *16
* Trashes scratch

 SUBRP	drone_getcurskillo

	move	*a13(plyr_ptsdown),a0
	move	*a13(plyr_d_skill),a14
	add	a14,a0


	move	*a13(plyr_num),a1
	move	@dronesmrt,a14
	btst	a1,a14
	jrz	#nosmrt			;Normal?
	addk	5,a0
#nosmrt


	move	@ballnumscored,a1
	subk	ONFIRE_MINCNT-1,a1
	jrlt	#noheat			;None heating up?

	addk	2,a0			;Get tougher

	move	@ballpnumscored,a1
	move	*a13(plyr_num),a14
	cmp	a1,a14
	jrne	#noheat			;Not me?

	addk	6,a0			;Get tougher
#noheat

	subk	15,a0
	jrle	#mxdnok
	clr	a0
#mxdnok
	addk	15+15,a0
	jrge	#dnok
	clr	a0
#dnok
	sll	4,a0			;Ptsdn+skill for indexing (*16)

	rets



#*******************************
* Adjust all drone abilities (at every minute dec of game clock)
* A0 = Game clock minute count before dec (0-2)
* Trashes scratch

 SUBR	drone_adjskill

	PUSH	a2,a3,a4,a5,a6

	move	a0,a5
	subk	2,a5
	abs	a5
	move	@gmqrtr,a1
	cmpi	3,a1
	jrls	#qok
	movk	3,a1			;Overtime
#qok
	movk	3,a0
	mpyu	a0,a1
	add	a1,a5			;A5=Quarter+minute index (0-11)


	movk	ADJDIFF,a0		;Get difficulty level
	calla	GET_ADJ			;1-5
	subk	4,a0			;-3 to 1
	move	a0,a6
	sll	1,a0			;*2
	add	a0,a6			;A6=Difficulty adj (-9,-6,-3,0,3)


	movi	plyrproc_t,a4
	movk	4,b0
#lp
	move	*a4+,a3,L

	move	*a3(plyr_d_skill),a2

	move	*a3(plyr_ptsdown),a14
	subk	15,a14
	jrle	#mxdnok
	clr	a14
#mxdnok
	addk	15+15,a14		;0-30
	jrge	#dnok
	clr	a14
#dnok
	sll	4,a14
	addi	#adj_t,a14
	move	*a14,a14
	add	a14,a2			;Modify skill

	move	a5,a14			;>Chk minute minimum
	sll	3,a14
	addi	#min_t,a14
	movb	*a14,a14
	add	a6,a14
	cmp	a14,a2
	jrge	#minok			;Min OK?
	move	a14,a2
#minok

	move	*a3(plyr_num),a1
	XORK	1,a1
	move	@PSTATUS,a14
	btst	a1,a14
	jrnz	#done			;Teammate is human?


	move	@team1,a1		;>Chk team minimum
	cmpi	3,b0
	jrge	#t1
	move	@team2,a1
#t1
	cmpi	29,a1
	jrlo	#tnumok
	movk	1,a1
#tnumok
	movk	12,a0
	mpyu	a0,a1

	add	a5,a1
	sll	3,a1			;*8
	addi	#tdmin_t,a1
	movb	*a1,a14
	add	a6,a14
	cmp	a14,a2
	jrge	#tminok
	move	a14,a2
#tminok
	addk	8,a14			;Max
	cmp	a14,a2
	jrle	#tmaxok
	move	a14,a2
#tmaxok
#done
	move	a2,*a3(plyr_d_skill)

	dsj	b0,#lp


	PULL	a2,a3,a4,a5,a6
	rets


#adj_t	.word	-5,-5,-5,-5,-5
	.word	-5,-5,-5,-4,-3
	.word	-2,-1,-1,0,0
	.word	0
	.word	0,1,1,2,2
	.word	3,3,4,4,5
	.word	5,6,6,6,7

#min_t	.byte	-15,-12,-10, -9,-8,-7, -7,-6,-6, -6,-6,-6

TMDIFF	.macro
	.byte	-10,-9,-8, -8,-7,-7, -6,-6,-5, -5,-5,-5
	.endm
#tdmin_t
	.byte	-6,-5,-5, -4,-3,-3, -2,-2,-2, -1, 0,-1	;ATL (11)
	TMDIFF						;BOS
	.byte	-7,-7,-6, -6,-5,-5, -4,-4,-3, -3,-2,-3	;CHA (14)
	.byte	6,7,7, 8,9,9, 10,10,11, 11,13,12	;CHI (1)
	.byte	-6,-5,-5, -4,-3,-3, -2,-2,-2, -1, 0,-1	;CLE (10)
	TMDIFF						;DAL
	.byte	-9,-8,-8, -7,-7,-6, -6,-5,-4, -4,-3,-4	;DEN (20)
	.byte	-6,-5,-5, -4,-3,-3, -2,-2,-2, -2,-1,-2	;DET (12)
	.byte	-9,-8,-8, -7,-7,-6, -6,-5,-4, -4,-3,-4	;GOL (18)
	.byte	-1,-1,0, 1,1,1, 2,2,2, 3,4,3		;HOU (6)
	.byte	-4,-3,-3, -2,-1,-1, 0,0,0, 1,2,1	;IND (8)
	TMDIFF						;LAC
	.byte	-5,-4,-4, -3,-2,-2, -1,-1,-1, 0,1,0	;LAL (9)
	.byte	-8,-8,-7, -7,-6,-6, -5,-4,-3, -3,-2,-3	;MI  (17)
	TMDIFF						;MIL
	TMDIFF						;MIN
	TMDIFF						;NJ
	.byte	-3,-2,-2, -1,0,0, 1,1,1, 2,3,2		;NY  (7)
	.byte	4,4,4, 4,4,5, 5,5,7, 7,8,7		;ORL (3)
	TMDIFF						;PHI
	.byte	-8,-8,-7, -7,-6,-6, -5,-4,-3, -3,-2,-3	;PHX (16)
	.byte	-7,-7,-6, -6,-5,-5, -4,-4,-3, -3,-2,-3	;POR (15)
	.byte	-6,-5,-5, -4,-3,-3, -3,-2,-2, -2,-1,-2	;SAC (13)
	.byte	1,1,1, 2,2,2, 3,3,3, 4,5,4		;SAN (5)
	.byte	4,4,5, 5,6,6, 7,7,7, 7,9,8		;SEA (2)
	TMDIFF						;TOR
	.byte	2,2,2, 3,3,3, 4,4,4, 5,6,5		;UTA (4)
	TMDIFF						;VAN
	.byte	-9,-8,-8, -7,-7,-6, -6,-5,-4, -4,-3,-4	;WAS (19)
	.even


********************************
* Get random # with mask
* A0 = Mask
*>A0 = Rnd # (Pass CC)
* Trashes scratch

 SUBRP	rnd

	move	@RAND,a1,L
	rl	a1,a1
	move	@HCOUNT,a14
	rl	a14,a1
	add	sp,a1
	move	a1,@RAND,L

	and	a1,a0
	rets


********************************
* Quickly produce a random # in range 0-X
* A0 = X
*>A0 = Random # (0 to A0) (No CC)
* Trashes scratch

 SUBRP	rndrng0

	move	@RAND,a1,L
	rl	a1,a1
	move	@HCOUNT,a14
	rl	a14,a1
	add	sp,a1
	move	a1,@RAND,L

	addk	1,a0
	mpyu	a1,a0		;Condition codes not valid!

	rets



********************************

	.end
*/
#endif

