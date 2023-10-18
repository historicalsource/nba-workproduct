/*
 *		$Archive: /video/Nba/cambot.c $
 *		$Revision: 192 $
 *		$Date: 10/29/99 1:55p $
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
char *ss_cambot_c = "$Workfile: cambot.c $ $Revision: 192 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif

/*
 *		USER INCLUDES
 */

#define NOT_READY		//!!!

#ifndef RELEASE_VER
 #ifndef TEST_VER
  #ifdef DEBUG
	#define CAMERA_BT		(P_A)				// P5 button for view select (see IOASIC.H)

	#define STICK_CAM_SW	4					// Stick (0-4) to use
	#define STICK_CAM_BT	(P_A)				// P5 button to use (see IOASIC.H)

	#define SPECIAL_MODES						// Def for additional camera modes
	#define STICK_CAM_ON						// Def to enable stick control
//	#define CAMERA_GLUE							// Def to freeze camera during game play

//	#define S_CCRAP								// Def for mode display & CREDIT camera data dump
	#define S_CBALL								// Def for COIN4 ball data dump
	#define S_CPLYR								// Def for COIN3 plyr data dump
//	#define S_CTRAK								// Def for 
  #endif //DEBUG
 #endif //TEST_VER
#endif //RELEASE_VER

#include "game.h"
#include "player.h"
#include "camdefs.h"
#include "externs.h"
#include "id.h"
#include "fontid.h"
#include "system.h"
#include "mainproc.h"
#include "select.h"
#include "optscrn.h"
#include "sndcalls.h"

#ifdef	SHAWN
#undef	TRGY_SIDE
#undef	SIDE_DIST_DEF
#define	TRGY_SIDE		(10*FT2UN)
#define	SIDE_DIST_DEF	(120)
#endif

/*
 *		DEFINES
 */

#define PLAYOVER_TICKS	2						// Play over transition ticks (must be at least 2)

#define	DOBYPASS		0						// Post mode-specific action flags
#define	UNROTATE		(1 << 0)				// Unrotate target coords
#define	SNAPTARG		(1 << 1)				// Snap target to coords/angles
#define	ACCLTPOS		(1 << 2)				// Do target coords acceleration math
#define	ACCLTANG		(1 << 3)				// Do target angles acceleration math
#define	MOVETPOS		(1 << 4)				// Accelerate target coords
#define	MOVETANG		(1 << 5)				// Accelerate target angles
#define	MOVEVIEW		(1 << 6)				// Move viewpt coords
#define	NEWTHETA		(1 << 7)				// Calculate new theta values

#define	ACCLTARG		(ACCLTPOS | ACCLTANG)	// Do target coords/angles acceleration math
#define	MOVETARG		(MOVETPOS | MOVETANG)	// Accelerate target coords/angles

#define	DOROTCAMX(x,z)	( x * cambot.cos_theta - z * cambot.sin_theta)
#define	DOROTCAMZ(x,z)	( x * cambot.sin_theta + z * cambot.cos_theta)
#define	DONOXCAMX(z)	(-z * cambot.sin_theta)
#define	DONOZCAMX(x)	( x * cambot.cos_theta)
#define	DONOXCAMZ(z)	( z * cambot.cos_theta)
#define	DONOZCAMZ(x)	( x * cambot.sin_theta)
#define	UNROTCAMX(x,z)	( z * cambot.sin_theta + x * cambot.cos_theta)
#define	UNROTCAMZ(x,z)	( z * cambot.cos_theta - x * cambot.sin_theta)

// Tracking buffer constants

#define TRACK_DEFAULT		1.0f				// Track buffer: default
#define TRACK_P2P			0.25f				// Track buffer: <cambot_p2p> mode
#define TRACK_HT_RUN		0.001f				// Track buffer: htime running

#define T_ANG_DEFAULT		DG2RD(0.25f)		// Track buffer: angle default

// Acceleration constants

#define ACCEL_P2P			0.25f				// Accel rate: <cambot_p2p> mode
#define ACCEL_HT_RUN		0.15f				// Accel rate: htime running
#define ACCEL_DUNKING		0.11f				// Accel rate: default
#define ACCEL_DEFAULT		0.08f				// Accel rate: default
#define ACCEL_DEFAULT2		0.09f				// Accel rate: default
#define ACCEL_DEFAULT3		0.06f				// Accel rate: default
#define ACCEL_ATTRACT		0.04f				// Accel rate: attract mode
#define ACCEL_HT_IB			0.025f				// Accel rate: htime inbound mode
#define ACCEL_DEF_NOBH		0.0005f				// Accel rate: default but no ball handler

#define A_ANG_DEFAULT		0.05f				// Accel rate: angle default

// Deceleration constants

#define DECEL_DEFAULT		0.35f				// Decel rate: default

#define D_ANG_DEFAULT		0.35f				// Decel rate: angle default

/*
 *		STRUCTURES/TYPEDEFS
 */

// info structure for cambot positioning

typedef struct tagCAMBOTINFO {
	float fa,fb,fc,fd;
	float fdx,fdz;
	float * p_track;
	float * p_accel;
	int handler_mode;
	int inbound_mode;
	int ci_ball_mode;
//	int player_valid,player_count;
//	float frot_dxmin,frot_dxmax;
//	float frot_zmin,frot_zmax;
//	float frot_x[NUM_PLAYERS];
//	float frot_z[NUM_PLAYERS];
} CAMBOTINFO;

/*
 *		PROTOTYPES
 */

void set_cambot_theta(float);
void set_cambot_phi(float);
void update_cambot_xform(void);

float ValidateTheta(float);
float ValidatePhi(float);

static void BuffAccel(float *, float, float, float);

void cambot_proc(int *);

void * select_ptr(void *);
void replay_transition_proc( int * );

//static void TrackObjects(VIEWCOORD *, CAMBOTINFO *);
static void DoSnap(VIEWCOORD *, CAMBOTINFO *, int);
static void DoSpline(VERTEX *, SPLINE *, int, int);
static void DoAccelDecel(int, int);
static void SetModeAngleTrgs(int);
static int ModeRevert(VIEWCOORD *, CAMBOTINFO *);

static int cambot_side(VIEWCOORD *, CAMBOTINFO *);
static int cambot_htime(VIEWCOORD *, CAMBOTINFO *);
static int cambot_end(VIEWCOORD *, CAMBOTINFO *);
static int cambot_shawn(VIEWCOORD *, CAMBOTINFO *);
static int cambot_attract(VIEWCOORD *, CAMBOTINFO *);
static int cambot_zoom(VIEWCOORD *, CAMBOTINFO *);
static int cambot_spline(VIEWCOORD *, CAMBOTINFO *);
static int cambot_rail(VIEWCOORD *, CAMBOTINFO *);
static int cambot_path(VIEWCOORD *, CAMBOTINFO *);
static int cambot_course(VIEWCOORD *, CAMBOTINFO *);
static int cambot_p2p(VIEWCOORD *, CAMBOTINFO *);
static int cambot_gopher(VIEWCOORD *, CAMBOTINFO *);
static int cambot_resume(VIEWCOORD *, CAMBOTINFO *);
#ifdef SPECIAL_MODES
static int cambot_helmet(VIEWCOORD *, CAMBOTINFO *);
#endif //SPECIAL_MODES
static int cambot_special(VIEWCOORD *, CAMBOTINFO *, int);

/*
 *		GLOBAL REFERNCES
 */

extern int juker_pnum;
extern int jukee_pnum;

extern POP_INFO * ppi_cooldunk1[];
extern POP_INFO * ppi_layup1[];
extern POP_INFO * ppi_cooljuke1[];

extern int ib_mode;				// inbound mode set by <SetUpOOBounds>

extern char gameplay_bnk_str[];

extern int pup_no_replays;

extern int	tsec;
extern int	p_status;
extern float hres, vres;

extern TO_INFO * ppto_info[];
extern char referee_tipoff_anim_delay[];

//extern int flg_hhteam;							// Set per case at start of each play
//extern int flg_pullback;						// Set per case at start of each play
//extern int idiot_boxes_shown;

//void show_message_proc(int *);
void referee_proc( int * );

#ifdef DBG_KO
#define DBG_KO_BOOL	dbg_ko
int dbg_ko = 0;
#else
#define DBG_KO_BOOL	0
#endif

/*
 *		GLOBAL VARIABLES
 */

VIEWCOORD cambot;

//float p2p_dist[NUM_PLAYERS][NUM_PLAYERS];

int shaker_amp;									// Shaker amplification level
int shaker_tot;									// Shaker count total
int shaker_cnt;									// Shaker count

/*
 *		STATIC VARIABLES
 */

static int ddunk_flg;

static bbplyr_data * ppbh;
static bbplyr_data * ppws;
static bbplyr_data * ppwt;
static bbplyr_data * ppir;

#ifdef STICK_CAM_ON
static POP_INFO ** pppi;
#endif
static POP_INFO * ppi;
static CAMSEQINFO * pcsi;
static pos_3d event = {0.0f, 0.0f, 0.0f, 0};
static int flags_curr[MAX_PLYRS];
static int flags_prev[MAX_PLYRS];
//static int pop_btn;
static int pop_dly;
static int pop_fwd;
static int pop_itm;
static int pop_slp;
static int pop_plyr;
static int pop_team;
static int gip_cnt;
static int gpo_cnt;
#ifndef DEBUG
static
#endif
int pop_cnt;


// Tracking value tables

static float track_def[] = {
	TRACK_DEFAULT,	TRACK_DEFAULT,	TRACK_DEFAULT,	TRACK_DEFAULT
};
static float track_ht_running[] = {
	TRACK_HT_RUN,	TRACK_DEFAULT,	TRACK_DEFAULT,	TRACK_DEFAULT
};
static float track_ht_dunking[] = {
	TRACK_DEFAULT,	TRACK_HT_RUN,	TRACK_DEFAULT,	TRACK_DEFAULT
};
static float track_def_xz[] = {
	TRACK_DEFAULT,	0.0f,			TRACK_DEFAULT,	0.0f
};
static float track_p2p[] = {
	TRACK_P2P,		TRACK_P2P,		TRACK_P2P,		0.0f
};
static float track_tight[] = {
	TRACK_P2P,		TRACK_P2P,		TRACK_P2P,		TRACK_P2P
};

// Acceleration value tables

static float accel_def[] = {
	ACCEL_DEFAULT,	ACCEL_DEFAULT,	ACCEL_DEFAULT,	ACCEL_DEFAULT
};
static float accel_ht_running[] = {
	ACCEL_HT_RUN,	ACCEL_DEFAULT,	ACCEL_DEFAULT,	ACCEL_DEFAULT
};
static float accel_ht_dunking[] = {
	ACCEL_ATTRACT,	ACCEL_HT_RUN,	ACCEL_ATTRACT,	ACCEL_ATTRACT
};
static float accel_ht_lb[] = {
	ACCEL_DEFAULT3,	ACCEL_DEFAULT,	ACCEL_ATTRACT,	ACCEL_DEFAULT
};
static float accel_ht_ib[] = {
	ACCEL_HT_IB,	ACCEL_P2P,		ACCEL_DEFAULT,	ACCEL_ATTRACT
};
static float accel_def_nobh[] = {
	ACCEL_DEFAULT,	ACCEL_DEFAULT,	ACCEL_DEFAULT,	ACCEL_DEF_NOBH
};
static float accel_attract[] = {
	ACCEL_ATTRACT,	ACCEL_ATTRACT,	ACCEL_ATTRACT,	0.0f
};
static float accel_p2p[] = {
	ACCEL_P2P,		ACCEL_P2P,		ACCEL_P2P,		0.0f
};
static float accel_ht_shooting[] = {
	ACCEL_ATTRACT,	ACCEL_DEFAULT2,	ACCEL_ATTRACT,	ACCEL_ATTRACT
};
static float accel_tight[] = {
	ACCEL_P2P,		ACCEL_P2P,		ACCEL_P2P,		ACCEL_P2P
};

/*// Mode Parameters
#define NQ	0
#define QB	1

enum {
	MP_DIST,
	MP_SIDE,
	MP_CNT
};
//                	    M   MP    QB 4P PB
static float mode_parms[][MP_CNT][2][2][2] =
{
 {	// CAM_NORMAL
  {
   {{NORMAL_DIST_MIN,		NORMAL_DIST_MINPB},
    {NORMAL_DIST_MIN4P,		NORMAL_DIST_MIN4PPB}},
   {{NORMAL_DIST_QB,		NORMAL_DIST_QBPB},
    {NORMAL_DIST_QB4P,		NORMAL_DIST_QB4PPB}}
  },
  {
   {{NORMAL_SIDELIM,		NORMAL_SIDELIM_PB},
    {NORMAL_SIDELIM_4P,		NORMAL_SIDELIM_4PPB}},
   {{NORMAL_SIDELIM_QB,		NORMAL_SIDELIM_QBPB},
    {NORMAL_SIDELIM_QB4P,	NORMAL_SIDELIM_QB4PPB}}
  }
 }
};*/


static int idiot_flag;
//static int show_sideline;

//static float sin_alpha;
//static float sin_delta;

#ifdef SPECIAL_MODES
static int helmet_fang;							// helmet-cam facing angle
static int helmet_tang;							// helmet-cam target angle
#endif //SPECIAL_MODES

// Function calls for modal camera control

static int (*cambot_funcs[])(VIEWCOORD *, CAMBOTINFO *) = {
		cambot_side,
		cambot_htime,
		cambot_end,
		cambot_shawn,
		cambot_attract,
// transient modes
		cambot_zoom,   cambot_zoom,   cambot_zoom,
		cambot_spline, cambot_spline, cambot_spline, cambot_spline,
		cambot_rail,   cambot_rail,   cambot_rail,
		cambot_path,   cambot_path,   cambot_path,
		cambot_course, cambot_course, cambot_course,
		cambot_p2p,    cambot_p2p,
		cambot_gopher, cambot_resume,
#ifdef SPECIAL_MODES
		NULL, NULL,
		cambot_helmet
#endif //SPECIAL_MODES
};

#ifdef DEBUG
static char * szmode[] = {
	"CAM-SIDE",
	"CAM-HTIME",
	"CAM-END",
	"CAM-SHAWN",
	"CAM-ATTRACT",
	"CAM-ZOOM",  "CAM-ZO-NR","CAM-ZO-NFR",
	"CAM-SPLINE","CAM-SP-NR","CAM-SP-NFR","CAM-SP-CNFR",
	"CAM-RAIL",  "CAM-RA-NF","CAM-RA-NFT",
	"CAM-PATH",  "CAM-PA-NF","CAM-PA-NFT",
	"CAM-COURSE","CAM-CO-C", "CAM-CO-CNT",
	"CAM-P2P",   "CAM-P2P-X",
	"CAM-GOPHER","CAM-RESUME",
#ifdef SPECIAL_MODES
	"MODECNT",   "NOCSI",
	"CAM-HELMET"
#endif //SPECIAL_MODES
};
#endif //DEBUG


// Shaker exponent table (2^(-x) with 8 steps between each integer)

static float shaker_exp[8 * 8] = {
//      +.000       +.125       +.250       +.375       +.500       +.625       +.750       +.875
/*0*/	1.00000000f,0.91700404f,0.84089641f,0.77110541f,0.70710678f,0.64841977f,0.59460355f,0.54525386f,
/*1*/	0.50000000f,0.45850202f,0.42044820f,0.38555270f,0.35355339f,0.32420988f,0.29730177f,0.27262693f,
/*2*/	0.25000000f,0.22925101f,0.21022410f,0.19277635f,0.17677669f,0.16210494f,0.14865088f,0.13631346f,
/*3*/	0.12500000f,0.11462550f,0.10511205f,0.09638817f,0.08838834f,0.08105247f,0.07432544f,0.06815673f,
/*4*/	0.06250000f,0.05731275f,0.05255602f,0.04819408f,0.04419417f,0.04052623f,0.03716272f,0.03407836f,
/*5*/	0.03125000f,0.02865637f,0.02627801f,0.02409704f,0.02209708f,0.02026311f,0.01858136f,0.01703918f,
/*6*/	0.01562500f,0.01432818f,0.01313900f,0.01204852f,0.01104854f,0.01013155f,0.00929068f,0.00851959f,
/*7*/	0.00781250f,0.00716409f,0.00656950f,0.00602426f,0.00552427f,0.00506577f,0.00464534f,0.00425979f
};

static CAMBOTINFO cambotinfo;
static CAMBOTINFO cambotinfo_init = {
	0.0f,0.0f,0.0f,0.0f,		// fa,fb,fc,fd
	0.0f,0.0f,					// fdx,fdz
	track_def,					// p_track
	accel_def,					// p_accel
	NO_BALLHANDLER,				// handler_mode
	0,							// inbound_mode
	0,							// ci_ball_mode
//	2000.0f,-2000.0f,			// frot_dxmin,frot_dxmax
//	2000.0f,-2000.0f,			// frot_zmin,frot_zmax
//	0,0							// player_valid,player_count
};


#ifdef STICK_CAM_ON
static char * mc_txt[] =
	{"MOVE X-Z", "MOVE D-Y", "ROTATE", "ROLL", "FOV", "FREEZE"};

enum{ MC_MXZ, MC_MDY, MC_ROT, MC_ROL, MC_FOV, MC_FRZ, MC_CNT };

static char * to_txt[] =
	{"EDIT-SELECT", "MOVE X-Z", "TIME RF-MOVE Y", "TIME M0-M1"};

enum{ TO_SEL, TO_MXZ, TO_RFY, TO_T01, TO_CNT };

static char * ti_txt[] =
	{"EDIT-SELECT", "TIME MC-TC", "ROTATE", "MOVE D"};

enum{ TI_SEL, TI_TMT, TI_ROT, TI_MVD, TI_CNT };

static int halt_cnt = 0;
#endif //STICK_CAM_ON


/******************************************************************************
** set_cambot_theta() - Set new cambot XZ theta,trigs
*/
void set_cambot_theta(float theta)
{
	cambot.theta = cambot.theta_trg = ValidateTheta(theta);
	cambot.sin_theta = fsin(cambot.theta);
	cambot.cos_theta = fcos(cambot.theta);
}

/******************************************************************************
** set_cambot_phi() - Set new cambot Y phi,trigs
*/
void set_cambot_phi(float phi)
{
	cambot.phi = cambot.phi_trg = ValidatePhi(phi);
	cambot.sin_phi = fsin(cambot.phi);
	cambot.cos_phi = fcos(cambot.phi);
}

/******************************************************************************
** update_cambot_xform() - Recompute cambot_xform based on current cambot
** settings.
*/
void update_cambot_xform(void)
{
	/* see Foley / van Dam pg 220 */

	vec3d p1p2;
	vec3d p1p3;
	vec3d p13xp12;
	vec3d ry;

	p1p2.x = cambot.a - cambot.x;
	p1p2.y = cambot.b - cambot.y;
	p1p2.z = cambot.c - cambot.z;
	p1p2.w = 1.0f;

	p1p3.x = cambot.e - cambot.x;
	p1p3.y = cambot.f - cambot.y;
	p1p3.z = cambot.g - cambot.z;
	p1p3.w = 1.0f;

	vxv(&p1p3,&p1p2,&p13xp12);
	norm(&p13xp12);
	norm(&p1p2);
	vxv(&p1p2,&p13xp12,&ry);

	cambot.xform[0] = p13xp12.x;
	cambot.xform[1] = p13xp12.y;
	cambot.xform[2] = p13xp12.z;
	cambot.xform[3] = 0.0f;

	cambot.xform[4] = ry.x;
	cambot.xform[5] = ry.y;
	cambot.xform[6] = ry.z;
	cambot.xform[7] = 0.0f;

	cambot.xform[8] = p1p2.x;
	cambot.xform[9] = p1p2.y;
	cambot.xform[10] = p1p2.z;
	cambot.xform[11] = 0.0f;
}

/******************************************************************************
** ValidateTheta()
*/
float ValidateTheta(float theta)
{
	register float fy = THETA_MAX;
	register float fz = THETA_MIN;

	while (theta >= fy) theta -= fy;
	while (theta <  fz) theta += fy;

	return theta;
}

/******************************************************************************
** ValidatePhi()
*/
float ValidatePhi(float phi)
{
	register float fy = PHI_MAX;
	register float fz = PHI_MIN;

	if (phi < fy) phi = fy;
	if (phi > fz) phi = fz;

	return phi;
}

/******************************************************************************
** BuffAccel()
*/
#define P_DA	&cambot.da
#define P_XA	&cambot.xa
#define P_YA	&cambot.ya
#define P_ZA	&cambot.za

#define P_S_X1	&cambot.s.x1
#define P_S_Y1	&cambot.s.y1
#define P_S_Z1	&cambot.s.z1

static void BuffAccel(float * pf, float diff, float buff, float rate)
{
	if (diff < -buff) buff = -buff;
	else
	if (diff <= buff) {*pf = 0.0f; return;}

	*pf = rate * (diff - buff);
}

#ifdef DEBUG
/******************************************************************************
*/
void display_mode(int *);
void display_mode(int *parg)
{
	int mylast = -1;
	int timer = 0;

	while (1) {
		if (timer) {
			timer--;
			if (!timer)
				delete_multiple_strings(0xfabe, 0xffffffff);
		}
		if (mylast != cambot.csi.mode) {
			mylast = cambot.csi.mode;
			delete_multiple_strings(0xfabe, 0xffffffff);
			set_text_font(FONTID_NBA10);
			set_text_transparency_mode(TRANSPARENCY_ENABLE);
			set_text_position(256, 370, 1.2f);
			set_text_z_inc(0.01f);
			set_string_id(0xfabe);
			oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_BOTTOM), LT_CYAN, (mylast >= 0) ? szmode[mylast]:"(NULL)");
			set_text_z_inc(0.0f);
			timer = tsec * 3;
		}
		sleep(1);
	}
}
#endif //DEBUG

/******************************************************************************
** cambot_proc() - Top-level cambot control process.
*/
void cambot_proc(int *parg)
{
	// Set some initial defaults
#ifndef SHAWN
	cambot.csi.mode     = CAM_HTIME;		//SIDE;
#else
	cambot.csi.mode     = CAM_SHAWN;
#endif

	if (game_info.game_state == GS_ATTRACT_MODE)
		cambot.csi.mode =
			randrng(100) < 50 ? CAM_SIDE :
			randrng(100) < 70 ? CAM_HTIME : CAM_END;

	cambot.mode_prev    = cambot.csi.mode;
	cambot.was_turnover = 0;
	cambot.new_quarter  = 1;
//	cambot.off_prev     = -1;
	cambot.csi.trgy_adj = 0.0f;

	SetModeAngleTrgs(cambot.csi.mode);
	set_cambot_theta(cambot.theta_trg);
	set_cambot_phi(cambot.phi_trg);

	cambot.dv = cambot.xv = cambot.yv = cambot.zv = cambot.tv = cambot.pv = 0.0f;
	cambot.da = cambot.xa = cambot.ya = cambot.za = cambot.ta = cambot.pa = 0.0f;

	cambot.d = 100.0f;
//	cambot.length = 2.0f;

	idiot_flag = 0;

//	// Set constants
//	sin_alpha = fsin(ALPHA);

	
	// Do default setup so we at least have a valid transform matrix
	cambot.x = -300.0f;
	cambot.y = 10.0f;
	cambot.z = 0.0f;

	cambot.e = (cambot.a = 0.0f);
	cambot.f = (cambot.b = TRGY_HTIME) + 10.0f;	//SIDE
	cambot.g = (cambot.c = 0.0f);

	update_cambot_xform();


//	// Wait for all the other idiots to initialize
//	while (game_info.game_mode == GM_INITIALIZING)
//		sleep(1);
//
//
//	// Do setup for initial kickoff
//	cambot.x = (game_info.last_pos) ? FIELDHZ : -FIELDHZ;
//	cambot.y =  25.0f;
//	cambot.z = (game_info.last_pos) ? -35.0f : 35.0f;
//
//	cambot.e = (cambot.a = WORLDX(FIRSTKO_LOS));
//	cambot.f = (cambot.b = TARGY_DEF_NORMAL) + 10.0f;
//	cambot.g = (cambot.c = 0.0f);
//
//	update_cambot_xform();
//#ifdef DEBUG_KICKOFF
//	printf("%f  %f  %f\n",ball_obj.odata.x,ball_obj.odata.y,ball_obj.odata.z);
//	printf("%f  %f  %f\n",cambot.x,cambot.y,cambot.z);
//	printf("%f  %f  %f\n",cambot.a,cambot.b,cambot.c);
//#endif //DEBUG_KICKOFF

#ifndef RELEASE_VER
#ifndef TEST_VER
#ifdef DEBUG
#ifdef S_CCRAP
	set_process_run_level( iqcreate( "camdisp", 0, display_mode, 0, 0, 0, 0 ), RL_CAMBOT+1 );
#endif //S_CCRAP
#endif //DEBUG
#endif //TEST_VER
#endif //RELEASE_VER

	// Top of proc loop
	while (1) {
		register VIEWCOORD *pcam = &cambot;
		register CAMBOTINFO *pci = &cambotinfo;
		int flag = DOBYPASS;

#ifdef STICK_CAM_ON
		extern int haltstep;
		static int bt_idx = -1;
		static int to_idx = -1;
		static int roll = 0;
		static float svelx = 0.0f, svely = 0.0f;
		static float fovval_def = 0.0f;

		if (fovval_def == 0.0f) fovval_def = fovval;

		// Bypass if in replay mode
		if (!PLAYBACK)
		{

#ifdef DBG_KO
			{
				static int dc_cnt = 0;
				static int dc_flg = 0;
				if (dc_cnt) dc_cnt--;
				if ((get_player_sw_close() & P3_D) && (game_info.game_mode != GM_TIPOFF))
				{
					if (dc_flg) dc_flg = FALSE;
					else
					if (dc_cnt) dc_flg = TRUE;
					else
					dc_cnt = 6;
				}
				if (get_dip_coin_current() & COINDOOR_INTERLOCK_SW || dc_flg)
					dbg_ko = TRUE;
			}
#endif //DBG_KO

			if (halt || DBG_KO_BOOL)
			{
				static int last_sw = 0;
				static int repeat = 0;
#if STICK_CAM_SW == 4
				int sw = get_p5_sw_current();
#else
				int sw = get_player_sw_current() >> (STICK_CAM_SW * 8);
#endif //STICK_CAM_SW == 4
				int sc = (sw ^ last_sw) & sw;
				if (!repeat)
					repeat = 2;
				if (sw && sw == last_sw)
					repeat--;
				else
					repeat = 20;
				last_sw = sw;
				if (bt_idx < 0 && DBG_KO_BOOL)
				{
					TO_INFO ** ppti = ppto_info;
					while(ppti[0])
						ppti[0]->ti_idx = -1, ppti++;
				}
				if (halt)
					halt_cnt++;
				if (sc & STICK_CAM_BT)
				{
					bt_idx++;
					if ((bt_idx == MC_CNT && !DBG_KO_BOOL) ||
						(bt_idx == TO_CNT &&  DBG_KO_BOOL))
						bt_idx = 0;
					if (to_idx < 0 && DBG_KO_BOOL)
						bt_idx = TO_SEL;
					svelx = svely = 0.0f;
				}
				else
				if (sw & STICK_CAM_BT)
				{
					// Check stick movement
					if ((sw &= 15))
					{
						static float sw_vel[] = {0.0f, -0.5f,  0.5f};
						static float sw_acc[] = {0.0f, -0.04f, 0.04f};

						if (svelx == 0.0f || !(sw>>2))
							svelx  = sw_vel[sw>>2];
						else
							svelx += sw_acc[sw>>2];
						if (svely == 0.0f || !(sw &3))
							svely  = sw_vel[sw &3];
						else
							svely += sw_acc[sw &3];

						switch (bt_idx)
						{
							case MC_MXZ:
						//	case TO_SEL:
						//	case TI_SEL:
								if (DBG_KO_BOOL)
								{
									if (svely != 0 || (to_idx < 0 && svelx != 0))
									{
										if (to_idx < 0)
										{
											to_idx = 0;
										}
										else
										if (ppto_info[to_idx]->ti_idx < 0)
										{
											if (sc & P_UP)
											{
												if (--to_idx < 0)
												{
													for (to_idx = 0;
														ppto_info[to_idx+1];
														to_idx++);
												}
											}
											else
											if (sc & P_DOWN)
											{
												if (!ppto_info[++to_idx])
												{
													to_idx = 0;
												}
											}
										}
										ppto_info[to_idx]->ti_idx = -1;
									}
									else
									if (svelx != 0)
									{
										TO_INFO * pti = ppto_info[to_idx];
										if (pti->ti_idx < 0)
										{
											pti->ti_idx = 0;
										}
										else
										{
											if (sc & P_LEFT)
											{
												if (--pti->ti_idx < 0)
													pti->ti_idx = pti->csi_cnt - 1;
											}
											else
											if (sc & P_RIGHT)
											{
												if (++pti->ti_idx == pti->csi_cnt)
													pti->ti_idx = 0;
											}
										}
									}
								}
								else
								{
									pcam->a -= svely * pcam->sin_theta + svelx * pcam->cos_theta;
									pcam->c -= svely * pcam->cos_theta - svelx * pcam->sin_theta;
									flag = (MOVEVIEW | ACCLTARG);
								}
								break;
							case MC_MDY:
						//	case TO_MXZ:
						//	case TI_TMT:
								if (DBG_KO_BOOL)
								{
									TO_INFO * pti = ppto_info[to_idx];
									if (pti->ti_idx < 0)
									{
										pti->x -= svelx * 0.5f;
										pti->z -= svely * 0.5f;
									}
									else
									{
										CAMSEQINFO * pcsi = &pti->pcsi[pti->ti_idx];
										if ((sc & P_LEFT) ||
											(sw == P_LEFT && !repeat))
										{
											if (--pcsi->mode_cnt < 0)
												pcsi->mode_cnt = 0;
										}
										else
										if ((sc & P_RIGHT) ||
											(sw == P_RIGHT && !repeat))
										{
											++pcsi->mode_cnt;
										}
										else
										if ((sc & P_UP) ||
											(sw == P_UP && !repeat))
										{
											if (--pcsi->trav_cnt < 0)
												pcsi->trav_cnt = 0;
										}
										else
										if ((sc & P_DOWN) ||
											(sw == P_DOWN && !repeat))
										{
											++pcsi->trav_cnt;
										}
									}
								}
								else
								{
									pcam->d -= svelx;
									pcam->b -= svely;
									flag = (MOVEVIEW | ACCLTARG);
								}
								break;
							case MC_ROT:
						//	case TO_RFY:
						//	case TI_ROT:
								if (DBG_KO_BOOL)
								{
									TO_INFO * pti = ppto_info[to_idx];
									if (pti->ti_idx < 0)
									{
										pti->y -= svely * 0.5f;
										if ((sc & P_LEFT) ||
											(sw == P_LEFT && !repeat))
										{
											--pti->ref_cnt;
										}
										else
										if ((sc & P_RIGHT) ||
											(sw == P_RIGHT && !repeat))
										{
											++pti->ref_cnt;
										}
										if (pti->ref_cnt < 0)
											pti->ref_cnt = 0;
										if (pti->jm0_cnt > (pti->ref_cnt+(unsigned char)referee_tipoff_anim_delay[1]))
											pti->jm0_cnt = (pti->ref_cnt+(unsigned char)referee_tipoff_anim_delay[1]);
										if (pti->jm1_cnt > (pti->ref_cnt+(unsigned char)referee_tipoff_anim_delay[1]))
											pti->jm1_cnt = (pti->ref_cnt+(unsigned char)referee_tipoff_anim_delay[1]);
									}
									else
									{
										CAMSEQINFO * pcsi = &pti->pcsi[pti->ti_idx];
										pcsi->theta = ValidateTheta(pcsi->theta + DG2RD(svelx * 0.25f));
										pcsi->phi =   ValidatePhi(pcsi->phi +     DG2RD(svely * 0.25f));
									}
								}
								else
								{
									if (svelx < 0.0f)
										set_cambot_theta(pcam->theta_trg-DG2RD(1.0f));
									else if (svelx > 0.0f)
										set_cambot_theta(pcam->theta_trg+DG2RD(1.0f));
									if (svely < 0.0f)
										set_cambot_phi(pcam->phi_trg-DG2RD(1.0f));
									else if (svely > 0.0f)
										set_cambot_phi(pcam->phi_trg+DG2RD(1.0f));
									flag = (MOVEVIEW | ACCLTARG);
								}
								break;
							case MC_ROL:
						//	case TO_T01:
						//	case TI_MVD:
								if (DBG_KO_BOOL)
								{
									TO_INFO * pti = ppto_info[to_idx];
									if (pti->ti_idx < 0)
									{
										if ((sc & P_LEFT) ||
											(sw == P_LEFT && !repeat))
											--pti->jm0_cnt;
										else
										if ((sc & P_RIGHT) ||
											(sw == P_RIGHT && !repeat))
											++pti->jm0_cnt;
										else
										if ((sc & P_UP) ||
											(sw == P_UP && !repeat))
											--pti->jm1_cnt;
										else
										if ((sc & P_DOWN) ||
											(sw == P_DOWN && !repeat))
											++pti->jm1_cnt;

										if (pti->jm0_cnt < 0)
											pti->jm0_cnt = 0;
										if (pti->jm1_cnt < 0)
											pti->jm1_cnt = 0;
										if (pti->jm0_cnt > (pti->ref_cnt+(unsigned char)referee_tipoff_anim_delay[1]))
											pti->ref_cnt = (pti->jm0_cnt-(unsigned char)referee_tipoff_anim_delay[1]);
										if (pti->jm1_cnt > (pti->ref_cnt+(unsigned char)referee_tipoff_anim_delay[1]))
											pti->ref_cnt = (pti->jm1_cnt-(unsigned char)referee_tipoff_anim_delay[1]);
									}
									else
									{
										CAMSEQINFO * pcsi = &pti->pcsi[pti->ti_idx];
										pcsi->d -= svelx;
									}
								}
								else
								{
									roll = (roll + (int)svelx) & 1023;
									if (svely) roll = 0;
								}
								break;
							case MC_FOV:
								if (DBG_KO_BOOL)
								{
								}
								else
								{
									pcam->d /= fovfac;

									fovval -= svely;
									if (svelx) fovval = fovval_def;

									fovfac   = fovval / 512.0f;
									fovfac2x = fovfac + fovfac;

									pcam->d *= fovfac;

									flag = MOVEVIEW;
								}
								break;
							default:
								svelx = svely = 0.0f;
								break;
						}
					}
					else
					{
						svelx = svely = 0.0f;
					}
				}
				else
				if (!(bt_idx == MC_FRZ && !DBG_KO_BOOL))
					delete_multiple_strings(0xfeed, 0xffffffff);

				if (last_sw & STICK_CAM_BT)
				{
					delete_multiple_strings(0xfeed, 0xffffffff);
					set_text_font(FONTID_NBA10);
					set_string_id(0xfeed);
					set_text_transparency_mode(TRANSPARENCY_ENABLE);
					set_text_z_inc(0.01f);

					set_text_position(256, DBG_KO_BOOL ? 210 : 318, 1.0f);
					oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_BOTTOM), LT_YELLOW,
						(!DBG_KO_BOOL) ?
							mc_txt[bt_idx] :
							((to_idx < 0) || ppto_info[to_idx]->ti_idx < 0) ?
								to_txt[bt_idx] :
								ti_txt[bt_idx]);

					if (bt_idx == MC_FOV && !DBG_KO_BOOL)
					{
						set_text_position(256, 306, 1.0);
						oprintf("%dj%dc%f", (HJUST_CENTER|VJUST_BOTTOM), WHITE, fovval);
					}

					set_text_z_inc(0.0f);
				}
				if (to_idx >= 0)
					ppto_info[to_idx]->bt_idx = (last_sw & STICK_CAM_BT) ? bt_idx : -1;
			}
			else if (bt_idx != -1 && bt_idx != MC_FRZ) {
				delete_multiple_strings(0xfeed, 0xffffffff);
				bt_idx = -1;
			}
		}
#endif //STICK_CAM_ON

		// Set player ptrs for this time around
		ppws = ppwt = ppir = ppbh = NULL;
		if (ball_obj.who_shot      != NO_SHOOTER)     ppws = player_blocks + ball_obj.who_shot;
		if (ball_obj.who_threw     != NO_PASSER)      ppwt = player_blocks + ball_obj.who_threw;
		if (ball_obj.int_receiver  != NO_RECEIVER)    ppir = player_blocks + ball_obj.int_receiver;
		if (game_info.ball_handler != NO_BALLHANDLER) ppbh = player_blocks + game_info.ball_handler;

//		// Do player-to-player distancing
//		{
//			register fbplyr_data *ppdi;
//			register fbplyr_data *ppdj;
//			register fbplyr_data *ppdk;
//			register float dx,dz;
//			register int i,j;
//			for (i = 0; i < NUM_PERTEAM; i++) {
//				ppdi = &player_blocks[i];
//				for (j = NUM_PERTEAM; j < NUM_PLAYERS; j++) {
//
//					// Do team 0 against team 1
//					ppdj = &player_blocks[j];
//					dx = ppdi->odata.x - ppdj->odata.x;
//					dz = ppdi->odata.z - ppdj->odata.z;
//					p2p_dist[i][j] = p2p_dist[j][i] = fsqrt(dx*dx + dz*dz);
//
//					if ((j - NUM_PERTEAM) > i) {
//
//						// Do team 0 against itself
//						ppdk = &player_blocks[j -= NUM_PERTEAM];
//						dx = ppdi->odata.x - ppdk->odata.x;
//						dz = ppdi->odata.z - ppdk->odata.z;
//						p2p_dist[i][j] = p2p_dist[j][i] = fsqrt(dx*dx + dz*dz);
//						j += NUM_PERTEAM;
//
//						// Do team 1 against itself
//						ppdk = &player_blocks[i += NUM_PERTEAM];
//						dx = ppdj->odata.x - ppdk->odata.x;
//						dz = ppdj->odata.z - ppdk->odata.z;
//						p2p_dist[i][j] = p2p_dist[j][i] = fsqrt(dx*dx + dz*dz);
//						i -= NUM_PERTEAM;
//					}
//				}
//			}
//		}

#ifdef STICK_CAM_ON
		if ((!halt && bt_idx != MC_FRZ) ||
			(
			 (halt || bt_idx == MC_FRZ) &&
				(playback || (haltstep && bt_idx != MC_FRZ) ||
#if STICK_CAM_SW == 4
					((get_p5_sw_close()) & P_C)
#else
					((get_player_sw_close() >> (STICK_CAM_SW * 8)) & P_C)
#endif //STICK_CAM_SW == 4
				)
			 )
			)
		{
#endif //STICK_CAM_ON

		// Revert to previous mode if in a transient & starting a new quarter;
		//  resets <trgy_adj> if reverts
		{
			if (pcam->csi.mode != CAM_SIDE
				&& pcam->csi.mode != CAM_HTIME
				&& pcam->csi.mode != CAM_END
//				&& pcam->csi.mode != CAM_SHAWN
//				&& pcam->csi.mode != CAM_ATTRACT
				)
			{
				if (pcam->new_quarter && game_info.game_quarter)
				{
#ifdef DEBUG
					if (pcam->csi.mode != pcam->mode_prev)
						printf("reverting to mode %d\n",pcam->mode_prev);
#endif
					pcam->csi.mode = pcam->mode_prev;
					pcam->csi.trgy_adj = 0.0f;
				}
			}
		}

		// Chk double dunk situation
		if ((game_info.game_mode == GM_IN_PLAY) &&
			(game_info.ball_mode == BM_DRIBBLING ||
			 game_info.ball_mode == BM_PASSING))
		{
			if ((player_blocks[(game_info.off_side<<1)+0].odata.flags & (PF_DUNKER|PF_ALLYDUNKER|PF_LAYUP)) &&
				(player_blocks[(game_info.off_side<<1)+1].odata.flags & (PF_DUNKER|PF_ALLYDUNKER|PF_LAYUP)))
			{
				ddunk_flg = 1;
			}
		}
		else
			ddunk_flg = 0;

		// Do mode specifics if its not a n-mode
		if (pcam->csi.mode >= 0)
			flag = cambot_funcs[pcam->csi.mode](pcam, pci);

		// Track any mode-switching
		if (pcam->mode_last != pcam->csi.mode) {
			// Only let <mode_prev> be a non-transient
			if (pcam->mode_last == CAM_SIDE
				|| pcam->mode_last == CAM_HTIME
				|| pcam->mode_last == CAM_END
				|| pcam->mode_last == CAM_SHAWN
//				|| pcam->mode_last == CAM_ATTRACT
#ifdef SPECIAL_MODES
				|| pcam->mode_last == CAM_HELMET
#endif //SPECIAL_MODES
				)
			{
				pcam->mode_prev = pcam->mode_last;
			}
		}
		pcam->mode_last = pcam->csi.mode;

		// Validate cambot angles
		pcam->theta     = ValidateTheta(pcam->theta);
		pcam->theta_trg = ValidateTheta(pcam->theta_trg);

		pcam->phi     = ValidatePhi(pcam->phi);
		pcam->phi_trg = ValidatePhi(pcam->phi_trg);

		// Do the unrotate if mode spec'd
		if (flag & UNROTATE) {
			register float fa = pci->fa;
			pci->fa = pci->fc * pcam->sin_theta + fa * pcam->cos_theta;
			//pci->fb = pci->fb;
			pci->fc = pci->fc * pcam->cos_theta - fa * pcam->sin_theta;
		}

		// Chk and do any special cameras
		flag = cambot_special( pcam, pci, flag );

#ifdef CAMERA_GLUE
		if (game_info.game_mode != GM_IN_PLAY)
		{
#endif //CAMERA_GLUE

		// Process diffs to determine new velocities
		if (flag) {
			// Snap target to coords/angles
			// Overrides any further position/velocity processing
			if (flag & SNAPTARG) {
				DoSnap(pcam, pci, flag);
			}
			else {
				if (flag & ACCLTPOS) {
					// Do target coords acceleration math
					// from current to desired
					register float * pf1 = pci->p_track;
					register float * pf2 = pci->p_accel;

					BuffAccel(P_XA, pci->fa - pcam->a, *pf1++, *pf2++);
					BuffAccel(P_YA, pci->fb - pcam->b, *pf1++, *pf2++);
					BuffAccel(P_ZA, pci->fc - pcam->c, *pf1++, *pf2++);
					BuffAccel(P_DA, pci->fd - pcam->d, *pf1++, *pf2++);

					pcam->xv *= DECEL_DEFAULT;
					pcam->yv *= DECEL_DEFAULT;
					pcam->zv *= DECEL_DEFAULT;
					pcam->dv *= DECEL_DEFAULT;
				}
				if (flag & ACCLTANG) {
					// Do target angles acceleration math
					// from current to desired
					register float fx = pcam->theta_trg - pcam->theta;
					register float fy = pcam->phi_trg - pcam->phi;

					if (fx) {
						if (fx >  ( THETA_MAX / 2)) fx -= THETA_MAX;
						if (fx <= (-THETA_MAX / 2)) fx += THETA_MAX;
					}
					BuffAccel(&cambot.ta, fx, T_ANG_DEFAULT, A_ANG_DEFAULT);
					BuffAccel(&cambot.pa, fy, T_ANG_DEFAULT, A_ANG_DEFAULT);

					pcam->tv *= D_ANG_DEFAULT;
					pcam->pv *= D_ANG_DEFAULT;
				}
				if (flag & MOVETPOS) {
					// Accelerate target coords
					pcam->a += (pcam->xv += pcam->xa);
					pcam->b += (pcam->yv += pcam->ya);
					pcam->c += (pcam->zv += pcam->za);
					pcam->d += (pcam->dv += pcam->da);
				}
				if (flag & MOVETANG) {
					// Accelerate target angles
					pcam->theta += (pcam->tv += pcam->ta);
					pcam->phi   += (pcam->pv += pcam->pa);
					if (pcam->tv) {
						pcam->sin_theta = fsin(pcam->theta);
						pcam->cos_theta = fcos(pcam->theta);
					}
					if (pcam->pv) {
						pcam->sin_phi = fsin(pcam->phi);
						pcam->cos_phi = fcos(pcam->phi);
					}
				}
			}
		}

#ifdef CAMERA_GLUE
		}
#endif //CAMERA_GLUE
#ifdef STICK_CAM_ON
		}
#endif //STICK_CAM_ON

#ifdef S_CCRAP
	{
	static int osw = 0;
	int csw = get_dip_coin_current();

	if (((csw ^ osw) & csw) & SERVICE_CREDIT_SW) {
	fprintf(stderr," x= %f   y= %f   z= %f\r\n",pcam->x,pcam->y,pcam->z);
	fprintf(stderr," a= %f   b= %f   c= %f   d= %f\r\n",pcam->a,pcam->b,pcam->c,pcam->d);
	fprintf(stderr," T= %f   P= %f   fovval= %f\r\n\r\n",((pcam->theta*180.0f)/M_PI),((pcam->phi*180.0f)/M_PI),fovval);
	}
	osw = csw;
	}
#endif //S_CCRAP

		// Put the viewpoint where it goes
		if (flag & MOVEVIEW) {
			pcam->x = pcam->a - pcam->d * pcam->cos_phi * pcam->sin_theta;
			pcam->y = pcam->b - pcam->d * pcam->sin_phi;
			pcam->z = pcam->c - pcam->d * pcam->cos_phi * pcam->cos_theta;
		}

		// Calculate new theta values
		if (flag & NEWTHETA) {
			register float fx  = pcam->a - pcam->x;
			register float fz  = pcam->c - pcam->z;
			register float ooh = rsqrt(fx * fx + fz * fz);

			pcam->sin_theta = (fx *= ooh);
			pcam->cos_theta = (fz *= ooh);

			// Just to be sure...
			if (fz < -1.0f) fz = -1.0f;
			if (fz >  1.0f) fz =  1.0f;

			fz = acos(fz);
			if (fx < 0)
				fz = M_TWOPI - fz;

			pcam->theta = ValidateTheta(fz);
		}

		// Do camera shaker
		if (shaker_cnt < shaker_tot) {
			register float expx;
			register float expz;
			expx = expz = shaker_exp[(sizeof(shaker_exp)/sizeof(*shaker_exp))*shaker_cnt/shaker_tot];
			shaker_cnt++;

			expx *= (float)(shaker_amp * (randrng(3) - 1));
			expz *= (float)(shaker_amp * (randrng(3) - 1));
			pcam->a += expx;
			pcam->c += expz;
			pcam->x += expx;
			pcam->z += expz;
		}

		// Finish positioning & update transform matrix
		pcam->e = pcam->a;
		pcam->f = pcam->b + 10.0f;
		pcam->g = pcam->c;
#ifdef STICK_CAM_ON
		pcam->e = pcam->a - (10.0f * isin(roll))*pcam->cos_theta;
		pcam->f = pcam->b + (10.0f * icos(roll));
		pcam->g = pcam->c + (10.0f * isin(roll))*pcam->sin_theta;
#endif //STICK_CAM_ON

		update_cambot_xform();

		// Check for camera view change (only if there is an active button)
#ifdef DBG_KO
		if (dbg_ko)
		{
			pcam->new_quarter = 1;
#ifdef STICK_CAM_ON
			if (to_idx >= 0)
				pcam->new_quarter = -1 - to_idx;
#endif //STICK_CAM_ON
		}
		else
		{
#endif //DBG_KO
#ifdef CAMERA_BT
		{
#ifdef STICK_CAM_ON
		if (!halt)
		{
#endif //STICK_CAM_ON
			if (get_p5_sw_close() & CAMERA_BT) {
				switch (pcam->csi.mode) {
					case CAM_SIDE:
						pcam->csi.mode = CAM_HTIME;
						break;

					case CAM_HTIME:
						pcam->csi.mode = CAM_END;
						break;

					case CAM_END:
						pcam->csi.mode = CAM_SHAWN;
						break;

					case CAM_SHAWN:
//						pcam->csi.mode = ???;
#ifdef SPECIAL_MODES
						if (get_p5_sw_current() & P_UP) {
							pcam->csi.mode = CAM_ATTRACT;
							break;
						}

					case CAM_ATTRACT:
						if (get_p5_sw_current() & P_UP) {
							pcam->csi.mode = CAM_GOPHER;
							break;
						}

					case CAM_GOPHER:
						if (get_p5_sw_current() & P_UP) {
							// Determine player # to track
							int pnum = GUARD;
							if (game_info.off_side) pnum += NUM_PERTEAM;
							if (game_info.game_mode == GM_IN_PLAY) {
								if ((pnum = game_info.ball_handler) < 0)
									pnum = ball_obj.int_receiver;
							}
							helmet_tang = player_blocks[pnum].odata.fwd & 1023;
							helmet_fang = helmet_tang;
							pcam->csi.mode = CAM_HELMET;
							break;
						}

					case CAM_HELMET:
#endif //SPECIAL_MODES
						pcam->csi.mode = CAM_SIDE;
						break;
				}
			}
#ifdef STICK_CAM_ON
		}
#endif //STICK_CAM_ON
		}
#endif //CAMERA_BT
#ifdef DBG_KO
		}
#endif //DBG_KO

#ifdef S_CBALL
	{
	static int onoff = 0;
	static int osw = 0;
	int csw = get_dip_coin_current();

	if (onoff) delete_multiple_strings(0xbead, 0xffffffff);
	if (((csw ^ osw) & csw) & EXTRA_COIN_SW) onoff = ~onoff;
	osw = csw;
	
	if (onoff){
	extern int OOBPasser;
	extern char * game_mode_names[];
	extern char * ball_mode_names[];
	int i;

	set_text_font(FONTID_NBA10);
	set_text_transparency_mode(TRANSPARENCY_ENABLE);
	set_string_id(0xbead);
	set_text_z_inc(0.01f);

	set_text_position(500, 376, 1.2f);
	oprintf("%dj%dc%s", (HJUST_RIGHT|VJUST_CENTER), LT_YELLOW, game_mode_names[game_info.game_mode-GM_PRE_TIPOFF]);

	set_text_position(500, 364, 1.2f);
	oprintf("%dj%dc%s", (HJUST_RIGHT|VJUST_CENTER), LT_YELLOW, ball_mode_names[game_info.ball_mode-BM_OFF_INTO_PLAY]);

	set_text_position(470, 352, 1.2f);
	oprintf("%dj%dcFT", (HJUST_RIGHT|VJUST_CENTER), LT_CYAN);
	set_text_position(500, 352, 1.2f);
	oprintf("%dj%dc%d", (HJUST_RIGHT|VJUST_CENTER), LT_YELLOW, ball_obj.flight_time);

	set_text_position(470, 340, 1.2f);
	oprintf("%dj%dcTT", (HJUST_RIGHT|VJUST_CENTER), LT_CYAN);
	set_text_position(500, 340, 1.2f);
	oprintf("%dj%dc%d", (HJUST_RIGHT|VJUST_CENTER), LT_YELLOW, ball_obj.total_time);

	set_text_position(470, 328, 1.2f);
	oprintf("%dj%dcBH", (HJUST_RIGHT|VJUST_CENTER), LT_CYAN);
	set_text_position(500, 328, 1.2f);
	oprintf("%dj%dc%d", (HJUST_RIGHT|VJUST_CENTER), LT_YELLOW, game_info.ball_handler);

	set_text_position(470, 316, 1.2f);
	oprintf("%dj%dcIR", (HJUST_RIGHT|VJUST_CENTER), LT_CYAN);
	set_text_position(500, 316, 1.2f);
	oprintf("%dj%dc%d", (HJUST_RIGHT|VJUST_CENTER), LT_YELLOW, ball_obj.int_receiver);
#if NUM_PERTEAM > 2
	set_text_position(470, 304, 1.2f);
	oprintf("%dj%dcRA", (HJUST_RIGHT|VJUST_CENTER), LT_CYAN);
	set_text_position(500, 304, 1.2f);
	oprintf("%dj%dc%d", (HJUST_RIGHT|VJUST_CENTER), LT_YELLOW, game_info.receiver_a);

	set_text_position(470, 292, 1.2f);
	oprintf("%dj%dcRB", (HJUST_RIGHT|VJUST_CENTER), LT_CYAN);
	set_text_position(500, 292, 1.2f);
	oprintf("%dj%dc%d", (HJUST_RIGHT|VJUST_CENTER), LT_YELLOW, game_info.receiver_b);
#endif
	set_text_position(470, 280, 1.2f);
	oprintf("%dj%dcOB", (HJUST_RIGHT|VJUST_CENTER), LT_CYAN);
	set_text_position(500, 280, 1.2f);
	oprintf("%dj%dc%d", (HJUST_RIGHT|VJUST_CENTER), LT_YELLOW, OOBPasser);

	set_text_position(470, 268, 1.2f);
	oprintf("%dj%dcWT", (HJUST_RIGHT|VJUST_CENTER), LT_CYAN);
	set_text_position(500, 268, 1.2f);
	oprintf("%dj%dc%d", (HJUST_RIGHT|VJUST_CENTER), LT_YELLOW, ball_obj.who_threw);

	set_text_position(470, 256, 1.2f);
	oprintf("%dj%dcWS", (HJUST_RIGHT|VJUST_CENTER), LT_CYAN);
	set_text_position(500, 256, 1.2f);
	oprintf("%dj%dc%d", (HJUST_RIGHT|VJUST_CENTER), LT_YELLOW, ball_obj.who_shot);

	set_text_z_inc(0.0f);
	}}
#endif //S_CBALL

#ifdef S_CPLYR
	{
	static int onoff = 0;
	static int osw = 0;
	int csw = get_dip_coin_current();

	if (onoff) delete_multiple_strings(0xbeaded, 0xffffffff);
	if (((csw ^ osw) & csw) & CENTER_COIN_SW) onoff = ~onoff;
	osw = csw;
	
	if (onoff){
	bbplyr_data * ppdata = ppbh;
	int i = 0;
	
	if (!ppdata) {
	if (game_info.ball_mode==BM_PASSING)
	ppdata = ppwt;
	else
	ppdata = ppws;}
	
	{
	extern char * player_flag_names[];
	unsigned int j;

	set_text_font(FONTID_NBA10);
	set_text_z_inc(0.01f);
	set_text_transparency_mode(TRANSPARENCY_ENABLE);
	set_string_id(0xbeaded);

	if (ppdata){
	set_text_position(12, 376, 1.2f);
	oprintf("%dj%dc%s%d", (HJUST_LEFT|VJUST_CENTER), LT_CYAN, "P", i);
	set_text_position(50, 376, 1.2f);
	oprintf("%dj%dc%d", (HJUST_RIGHT|VJUST_CENTER), LT_YELLOW, ppdata->hangtime);

	set_text_position(12, 364, 1.2f);
	oprintf("%dj%dc%s", (HJUST_LEFT|VJUST_CENTER), LT_CYAN, "FT");
	set_text_position(50, 364, 1.2f);
	oprintf("%dj%dc%d", (HJUST_RIGHT|VJUST_CENTER), LT_YELLOW, ppdata->flight_time);

	set_text_position(12, 352, 1.2f);
	oprintf("%dj%dc%s", (HJUST_LEFT|VJUST_CENTER), LT_CYAN, "TT");
	set_text_position(50, 352, 1.2f);
	oprintf("%dj%dc%d", (HJUST_RIGHT|VJUST_CENTER), LT_YELLOW, ppdata->total_time);
	}
	if (ppdata)
	j = (unsigned int)ppdata->odata.flags; else j = (unsigned int)pci->handler_mode;
	i = 0;
	while (j) {
	if (j & 1) {
	set_text_position(12, 338-(i*12), 1.2f);
	oprintf("%dj%dc%s", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, player_flag_names[i]);}
	j >>= 1;
	i++;}
	set_text_z_inc(0.00f);
	}}}
#endif //S_CPLYR

//		if (game_info.game_mode == GM_PLAY_OVER || game_info.game_mode == GM_GAME_OVER) {
//#ifdef S_CCRAP
//			if (pcam->was_playover != PLAYOVER_TICKS)
//				printf("was_playover = %d   %s\n",PLAYOVER_TICKS,(game_info.game_flags & GF_QUICK_KICK) ? "GF_QUICK_KICK":"\0");
//#endif //S_CCRAP
//			pcam->was_playover = PLAYOVER_TICKS;
//		}
//		else if (pcam->was_playover) {
//			pcam->was_playover--;
//#ifdef S_CCRAP
//			printf("sleep - was_playover: %d   game_mode: %d   %s\n",pcam->was_playover,game_info.game_mode,(game_info.game_flags & GF_QUICK_KICK) ? "GF_QUICK_KICK":"\0");
//#endif //S_CCRAP
//		}

		if (game_info.game_mode != GM_PRE_TIPOFF &&
			game_info.game_mode != GM_TIPOFF)
		{
			pcam->off_prev = (game_info.ball_mode != BM_FREE_ON_COURT) ?
				game_info.off_side : -1;
		}
		else
		if (game_info.ball_mode == BM_PASSING)
		{
			pcam->off_prev = ppir->team;
		}

		sleep(1);
	}
}

/******************************************************************************
** Determine cambot positioning info for valid objects
*
//#define XTGT_OFFSCRN	(1.0f * (HRESf/HRESf/2/FOVFAC))	//0.90f
//#define YTGT_OFFSCRN	(1.0f * (VRESf/HRESf/2/FOVFAC))	//0.90f

#define XTGT_OFFSCRN	(1.0f * (hres/hres/2/FOVFAC))	//0.90f
#define YTGT_OFFSCRN	(1.0f * (vres/hres/2/FOVFAC))	//0.90f

#define MODE_QB_DFAC	(0.40f)			// Higher = go further away as QB goes to sideline
#define MODE_QB_ZFAC	(0.68f* 0.82f)	//0.90f Higher = quicker pan to sideline
#define SNAP_RATIO		(0.75f)			//0.60f Higher = slower pan to off screen target

static void TrackObjects(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
#ifndef NOT_READY
	register game_info_t * pgi = &game_info;
	register ball_data * pbo = &ball_obj;
#endif //NOT_READY
	register bbplyr_data *ppd;
	register float fxbc,fzbc,fda,fdb;
	register int player_valid = 0;
	register int player_count = 0;
	register int pnum = NUM_PLAYERS;
#ifndef NOT_READY
	register int carrier;
#endif //NOT_READY
	float fx,fz;

	cambotinfo = cambotinfo_init;

	fxbc = (fzbc = 0.0f);

#ifndef NOT_READY
	if ((carrier = pgi->ball_handler) >= 0 ||
			((carrier = pbo->int_receiver) >= 0 &&
			pgi->play_result == PR_INCOMPLETE &&
			pgi->game_flags & GF_PLAY_ENDING))
			//pbo->flags & BF_BOUNCE))
	{
		// Set player as primary tracking point
		ppd = &player_blocks[carrier];
		pci->carrier_mode = ppd->odata.plyrmode;
		fxbc = ppd->odata.x;
		fzbc = ppd->odata.z;
		// Set player distance to his goal line
		carrier_goal_dist = FIELDHZ + fxbc * (ppd->team * 2 - 1);
		// Clr off-screen target flag
//if (pbo->flags & BF_TGT_OFFSCRN) {
//	printf("RESET RESET RESET RESET RESET RESET RESET RESET RESET: %d\n",pbo->flags & ~BF_TGT_OFFSCRN);
//	while (!(get_dip_coin_close() & SLAM_SW));
//}
		pbo->flags &= ~BF_TGT_OFFSCRN;
	}
	else
	{
//delete_multiple_strings(0xabe, 0xffffffff);
//set_text_transparency_mode(TRANSPARENCY_ENABLE);
//set_text_font(FONTID_NBA10);
//set_string_id(0xabe);
//set_text_position(200, 72, 1.2f);
//oprintf("%dj%dcPxz", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW);
//set_text_position(200, 60, 1.2f);
//oprintf("%dj%dcPyz", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW);
//set_text_position(200, 48, 1.2f);
//oprintf("%dj%dcPZ", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW);
//set_text_position(200, 36, 1.2f);
//oprintf("%dj%dcBxz", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW);
//set_text_position(200, 24, 1.2f);
//oprintf("%dj%dcByz", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW);
//set_text_position(200, 12, 1.2f);
//oprintf("%dj%dcBZ", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW);
//
//set_text_position(230, 72, 1.2f);
//oprintf("%dj%dc%f", (HJUST_LEFT|VJUST_CENTER), ((fabs(player_blocks[pbo->int_receiver].tsx / player_blocks[pbo->int_receiver].tsz) > XTGT_OFFSCRN) ? LT_RED:LT_YELLOW), player_blocks[pbo->int_receiver].tsx / player_blocks[pbo->int_receiver].tsz);
//set_text_position(230, 60, 1.2f);
//oprintf("%dj%dc%f", (HJUST_LEFT|VJUST_CENTER), ((fabs(player_blocks[pbo->int_receiver].tsy / player_blocks[pbo->int_receiver].tsz) > YTGT_OFFSCRN) ? LT_RED:LT_YELLOW), player_blocks[pbo->int_receiver].tsy / player_blocks[pbo->int_receiver].tsz);
//set_text_position(230, 48, 1.2f);
//oprintf("%dj%dc%f", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, player_blocks[pbo->int_receiver].tsz);
//set_text_position(230, 36, 1.2f);
//oprintf("%dj%dc%f", (HJUST_LEFT|VJUST_CENTER), ((fabs(pbo->tsx / pbo->tsz) > XTGT_OFFSCRN) ? LT_RED:LT_YELLOW), pbo->tsx / pbo->tsz);
//set_text_position(230, 24, 1.2f);
//oprintf("%dj%dc%f", (HJUST_LEFT|VJUST_CENTER), ((fabs(pbo->tsy / pbo->tsz) > YTGT_OFFSCRN) ? LT_RED:LT_YELLOW), pbo->tsy / pbo->tsz);
//set_text_position(230, 12, 1.2f);
//oprintf("%dj%dc%f", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, pbo->tsz);

		carrier = -1;
		if (!(pgi->game_flags & GF_QUICK_KICK && pcam->kickoff_flag) &&
				!(pgi->game_flags & GF_HIKING_BALL))
		{
			// Assume loop should process the ball/on-screen pass target
			pnum++;

			// Is it an as yet unretouched pass?
			if (pbo->type == LB_PASS && !(pbo->flags & BF_BOUNCE))
					//!(pgi->game_flags & (GF_POSS_CHANGE)))
			{
				// Assume loop should process an on-screen pass target
				pnum++;

				// Chk if target is off-screen if it wasn't before
				if ( !(pbo->flags & BF_TGT_OFFSCRN) && pbo->flight_time < 2 && (
						player_blocks[pbo->int_receiver].tsz < 10.0f ||
						pbo->tsz < 10.0f ||
						(fabs(player_blocks[pbo->int_receiver].tsy / player_blocks[pbo->int_receiver].tsz) > YTGT_OFFSCRN &&
						 fabs(pbo->tsy / pbo->tsz) > YTGT_OFFSCRN)
						||
						(fabs(player_blocks[pbo->int_receiver].tsx / player_blocks[pbo->int_receiver].tsz) > XTGT_OFFSCRN &&
						 fabs(pbo->tsx / pbo->tsz) > XTGT_OFFSCRN) ))
				{
					// Set off-screen target flag
					pbo->flags |= BF_TGT_OFFSCRN;
//printf("SET   SET   SET   SET   SET   SET   SET   SET   SET  : %d\n",pbo->flags);
//while (!(get_dip_coin_close() & SLAM_SW));
				}
				// Treat as on- or off-screen pass?
				if (pbo->flags & BF_TGT_OFFSCRN)
				{
					// Make loop process the off-screen pass target
					pnum ++;
					pnum ++;
				}
			}
		}
	}
//{
//	static last_flags;
//	if (last_flags != pbo->flags)
//	{
//		static char * crap[] = {"BF_WHITE","BF_VEL_PITCH","BF_TURBO_PASS","BF_FIRE_PASS","BF_BOBBLE","BF_BOUNCE","BF_ALT","BF_FREEZE","BF_TGT_OFFSCRN"};
//		int i = 0;
//		last_flags = pbo->flags;
//		while (last_flags)
//		{
//			if (last_flags & 1)
//				printf("%s  ",crap[i]);
//
//			last_flags >>= 1;
//			i++;
//		}
//		printf("\ntsx= %f  tsz= %f  irx= %f  irz= %f\n",pbo->tsx, pbo->tsz, player_blocks[pbo->int_receiver].tsx, player_blocks[pbo->int_receiver].tsz);
//		last_flags = pbo->flags;
//	}
//}

	while (--pnum >= 0)
	{
		if (pnum == (NUM_PLAYERS + 3))
		{
			// Do ball for the off-screen pass target
			fxbc = ball_obj.odata.x - ball_obj.tx;
			fzbc = ball_obj.odata.z - ball_obj.tz;
			fxbc *= SNAP_RATIO;
			fzbc *= SNAP_RATIO;
			fxbc += ball_obj.tx;
			fzbc += ball_obj.tz;

			if (FIELDX(ball_obj.tx) < pgi->los) fzbc *= MODE_QB_ZFAC;

			// Rotate to the Z axis
			fx = DOROTCAMX(fxbc,fzbc);
			fz = DOROTCAMZ(fxbc,fzbc);

			pci->carrier_mode = (NOBALLCARRIER * 2);
		}
		else
		if (pnum == (NUM_PLAYERS + 2))
		{
			// Do the off-screen pass target
			fxbc = ball_obj.tx;
			fzbc = ball_obj.tz;

			// Rotate to the Z axis
			fx = DOROTCAMX(fxbc,fzbc);
			fz = DOROTCAMZ(fxbc,fzbc);

			pnum = NUM_PLAYERS;	//!!!skip to the players
		}
		else
		if (pnum == (NUM_PLAYERS + 1))
		{
			// Do unretouched ball for the on-screen pass target
			fxbc = ball_obj.odata.x;
			fzbc = ball_obj.odata.z;

			if (FIELDX(ball_obj.tx) < pgi->los) fzbc *= MODE_QB_ZFAC;

			// Rotate to the Z axis
			fx = DOROTCAMX(fxbc,fzbc);
			fz = DOROTCAMZ(fxbc,fzbc);

			pnum = NUM_PLAYERS;	//!!!skip to the players
		}
		else
		if (pnum == NUM_PLAYERS)
		{
			// Do ball for the on-screen pass target
			fxbc = ball_obj.odata.x;
			fzbc = ball_obj.odata.z;

			// Rotate to the Z axis
			fx = DOROTCAMX(fxbc,fzbc);
			fz = DOROTCAMZ(fxbc,fzbc);
		}
		else
		{
			ppd = &player_blocks[pnum];
			fda = ppd->odata.x;
			fdb = ppd->odata.z;
			player_valid <<= 1;

			if (pgi->play_type == PT_KICKOFF)
			{
				if (pnum != carrier)
				{
					if (carrier >= 0) continue;
					if (pgi->game_flags & GF_QUICK_KICK)
					{
						if (pnum != 3 && pnum != 10) continue;		//3 & 10 for kick rcvrs
					}
					else
					{
						if (pnum != 0 && pnum != 7) continue;		//0 & 7 for kickers
					}
					//if (game_info.game_flags & GF_QUICK_KICK) {
						if (ppd->team != pgi->last_pos) continue;
					//}
					//else {
					//	if (ppd->team == pgi->last_pos) continue;
					//}
				}
			}
			else
			if (pgi->game_mode == GM_PLAY_OVER)
			{
				// Watch only the ball carrier
				if (pnum != carrier)
				{
					if (carrier >= 0) continue;
					if (pnum != 3 && pnum != 10) continue;
					if (ppd->team != pgi->last_pos) continue;
					if (!(pgi->game_flags & GF_BALL_KICKED)) continue;
				}
			}
			else
			if (pgi->game_mode == GM_LINING_UP ||
					 pgi->game_mode == GM_PRE_SNAP)
			{
				// Watch only the offense before the snap
				if (pgi->last_pos != ppd->team) continue;
			}
			else
			if (pci->carrier_mode == MODE_QB)
			{
				// Watch only the ball carrier
				if (pnum != carrier) continue;
				// Do Z percentage to make the field move sooner
				//  Probably only good for the behind view
				fdb *= MODE_QB_ZFAC;
			}
			else
			if (pci->carrier_mode > NOBALLCARRIER)
			{
				// Watch only the ball carrier
				if (pnum != carrier) continue;
			}
			else
			if (pgi->game_flags & GF_HIKING_BALL)
			{
				// Watch only the QB
				if (pnum != 3 && pnum != 10) continue;				//3 & 10 for qbs
				if (ppd->team != pgi->last_pos) continue;
			}
			else
			{
				continue;
			}
			// If got here, its time to process a player coordinate
			// Count # of valid plyrs
			player_valid++;
			player_count++;
			// Rotate to the Z axis
			pci->frot_x[pnum] = fx = DOROTCAMX(fda,fdb);
			pci->frot_z[pnum] = fz = DOROTCAMZ(fda,fdb);
		}
		// Process a valid coordinate
		// Make sure <fda> & <fdb> stay available after the rotate
		// Do X delta for phi-biased Z-axis intercepts; keep min/max intercepts
		fda = fz / 2.0f * pcam->cos_phi;
		if ((fdb = fx + fda) < pci->frot_dxmin) pci->frot_dxmin = fdb;
		if ((fdb = fx - fda) > pci->frot_dxmax) pci->frot_dxmax = fdb;
		// Keep min/max rotated Z for Z-based min distance
		if (fz < pci->frot_zmin) pci->frot_zmin = fz;
		if (fz > pci->frot_zmax) pci->frot_zmax = fz;
	} // End of while

	pci->player_valid = player_valid;
	pci->player_count = player_count;

	// Do default positioning
	// Rotated Y target
	pci->fb = pcam->b;

	if (pgi->play_type != PT_KICKOFF &&
			(pgi->game_mode == GM_LINING_UP || pgi->game_mode == GM_PRE_SNAP))
	{
		// Rotated X target is QB rotated X
		pci->fa = pci->frot_x[pgi->last_pos*NUM_PERTEAM+PPOS_QBACK];
		// Rotated Z target is rotated LOS
		pci->fc = DOROTCAMZ(WORLDX(pgi->los),0);

		// Calc intercept-to-target X delta
		fda = pci->frot_dxmax - pci->fa;
		fdb = pci->fa - pci->frot_dxmin;
		if (fdb > fda) fda = fdb;
		// Set Z/X ratio value
		fdb = FIX_FDX_PS;
	}
	else
	{
		// Rotated X target is mid of min/max X intercepts
		pci->fa = (pci->frot_dxmin + pci->frot_dxmax) / 2;
		// Rotated Z target is phi-biased from mid of min/max Z
		//   Goes from min-to-mid Z as phi goes from ground-to-overhead
		pci->fc = pci->frot_zmin - ((pci->frot_zmax - pci->frot_zmin) / 2 * pcam->sin_phi);

		// Calc intercept-to-target X delta
		fda = pci->frot_dxmax - pci->fa;
		// Set Z/X ratio value
		fdb = FIX_FDX;
	}

	// Do X-based min distance; use Y=mX+b on slope <m> [cos_phi/2],
	//   rotated Z target <X> [fc], & Z-axis intercept <b> -to-target
	//   X delta [fda]; take the ABS() times the Z/X ratio [fdb]
	//   to get the distance
	pci->fdx = fabs(
			(pci->fc * pcam->cos_phi / 2.0f) + fda
		) * fdb;

	// Do Z-based min distance; use Law of Sines on angle <A> [ALPHA],
	//   opposite side <a> [Ztrg-Zmin], & 2nd angle <B> [DELTA]
	//   to do 2nd opposite side <b> times buffer-factor [FIX_FDZ]
	//   to get the distance
	pci->fdz = (
			(pci->fc - pci->frot_zmin) *
			(sin_delta = fsin(DELTA + pcam->phi)) /
			(sin_alpha)
		) * FIX_FDZ;
#endif //NOT_READY

#ifdef S_CTRAK
{
	char * t = (pci->fdx < pci->fdz) ? "Z":"X";
	delete_multiple_strings(0xab, 0xffffffff);
	set_text_transparency_mode(TRANSPARENCY_ENABLE);
	set_text_font(FONTID_NBA10);
	set_string_id(0xab);
	set_text_position(150, 368, 1.2f);
	oprintf("%dj%dc%s", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, t);
	set_text_position(170, 368, 1.2f);
	oprintf("%dj%dcfdx %.2f", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, pci->fdx);
	set_text_position(170, 354, 1.2f);
	oprintf("%dj%dcfdz %.2f", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, pci->fdz);
}
#endif //S_CTRAK

}*/

/******************************************************************************
** DoSnap()
**  Snap the VIEWCOORD reference to the CAMBOTINFO reference
*/
static void DoSnap(VIEWCOORD * pcam, CAMBOTINFO * pci, int flags)
{
	pcam->a = pci->fa;
	pcam->b = pci->fb;
	pcam->c = pci->fc;
	pcam->d = pci->fd;

	pcam->theta_trg = ValidateTheta(pcam->theta_trg);
	pcam->phi_trg   = ValidatePhi(pcam->phi_trg);

	if (pcam->theta != pcam->theta_trg)
		set_cambot_theta(pcam->theta_trg);
	if (pcam->phi   != pcam->phi_trg)
		set_cambot_phi(pcam->phi_trg);

	pcam->xv = pcam->yv = pcam->zv = pcam->dv = pcam->tv = pcam->pv = 0.0f;
	pcam->xa = pcam->ya = pcam->za = pcam->da = pcam->ta = pcam->pa = 0.0f;

	// Put the viewpoint where it goes
	if (flags & MOVEVIEW) {
		pcam->x = pcam->a - pcam->d * pcam->cos_phi * pcam->sin_theta;
		pcam->y = pcam->b - pcam->d * pcam->sin_phi;
		pcam->z = pcam->c - pcam->d * pcam->cos_phi * pcam->cos_theta;
	}
}

/******************************************************************************
** DoSpline()
**  Standard Bezier algorithm
*/
static void DoSpline(VERTEX * pv, SPLINE * ps, int cur_tick, int tot_tick)
{
	register float k1, k2, k3, k4, u1;

// this is not a fix to the problem...
	if(tot_tick == 0)
		tot_tick = 1;

	k2 = (float)cur_tick / (float)tot_tick;		// u
	k3 = 1.0f - k2;								// 1-u
	k4 = k2 * k2;								// u^2
	u1 = k3;									// 1-u

	k1  = k3 * k3;								//     (1-u)^2
	k3 *= k4 * 3.0f;							// 3u^2(1-u)
	k4 *= k2;									//  u^3
	k2 *= k1 * 3.0f;							//   3u(1-u)^2
	k1 *= u1;									//     (1-u)^3

	pv->x = ps->x1 * k1 + ps->x2 * k2 + ps->x3 * k3 + ps->x4 * k4;
	pv->y = ps->y1 * k1 + ps->y2 * k2 + ps->y3 * k3 + ps->y4 * k4;
	pv->z = ps->z1 * k1 + ps->z2 * k2 + ps->z3 * k3 + ps->z4 * k4;
}

/******************************************************************************
** accelerate/decelerate
**   Call with cur_tick = 0 to do setup calc;
**    not meant to have accels used on cur_tick = 0 as this
**    would actually add 1 to tot_tick
*/
static void DoAccelDecel(int cur_tick, int tot_tick)
{
	register VIEWCOORD *pcam = &cambot;
	register CAMBOTINFO *pci = &cambotinfo;
	register float fs,ft;
	register int ts;

	// Kill acceleration after <tot_tick> ticks, then exit
	if (cur_tick > tot_tick) {
		pcam->xa = pcam->ya = pcam->za = pcam->da = pcam->ta = pcam->pa = 0.0f;
		return;
	}

	// Nothing to do if <tot_tick> is < 2
	if (tot_tick < 2)
		return;

	// If <cur_tick> is 0, do calc for half the distance in half the time
	if (!cur_tick) {
		fs = 4.0f;
		ts = tot_tick;
		// Start from a stand-still
		pcam->xv = pcam->yv = pcam->zv = pcam->dv = pcam->tv = pcam->pv = 0.0f;
	}
	else {
		// Calc the halfway tick #; is also the tick cnt to use for the
		//  accel recalc after halfway thru an odd-tick case
		ts = tot_tick / 2 + 1;
		// At or just passed halfway?
		if (cur_tick != ts)
			return;
		// Doing an odd-tick case?
		if (tot_tick & 1)
			// Odd; just passed halfway
			fs = -2.0f;
		else {
			// Not; at halfway
			pcam->xa = -pcam->xa;
			pcam->ya = -pcam->ya;
			pcam->za = -pcam->za;
			pcam->da = -pcam->da;

			pcam->ta = -pcam->ta;
			pcam->pa = -pcam->pa;
			return;
		}
	}
	fs /= (float)(ts * ts);
	pcam->xa = fs * (pci->fa - pcam->a);
	pcam->ya = fs * (pci->fb - pcam->b);
	pcam->za = fs * (pci->fc - pcam->c);
	pcam->da = fs * (pci->fd - pcam->d);

	ft = pcam->theta_trg - pcam->theta;
	if (ft >   THETA_MAX / 2) ft -= THETA_MAX;
	if (ft <= -THETA_MAX / 2) ft += THETA_MAX;
	pcam->ta = fs * ft;
	pcam->pa = fs * (pcam->phi_trg - pcam->phi);
}

/******************************************************************************
**
*/
static void SetModeAngleTrgs(int mode)
{
	register float theta;
	register float phi;

	// Set mode angle targets
	switch (mode) {
		case CAM_SIDE:
			theta = SIDE_THETA;
			phi   = SIDE_PHI;
			break;

		case CAM_HTIME:
			theta = HTIME_THETA;
			phi   = HTIME_PHI;
			break;

		case CAM_END:
			theta =
				(((game_info.game_quarter & 2) >> 1) ^ game_info.home_team) ?
				END_THETA_OPP : END_THETA;
			phi   = END_PHI;
			break;

		default:
			return;
	}
	cambot.phi_trg   = phi;
	cambot.theta_trg = theta;
}

/*****************************************************************************/
static int ModeRevert(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	// Revert to previous mode
	// Called when transients expire
	register int flag = (pcam->csi.mode = pcam->mode_prev);
	if (pcam->csi.trav_cnt < 2) {
		// Unsnap angles from snap mode
		SetModeAngleTrgs(flag);
		set_cambot_theta(pcam->theta_trg);
		set_cambot_phi(pcam->phi_trg);
	}
	flag = cambot_funcs[flag](pcam, pci);
	if (pcam->csi.trav_cnt < 2) {
		flag |= SNAPTARG | MOVEVIEW;
	}
	pcam->csi.trgy_adj = 0.0f;
	return flag;
}

/*****************************************************************************/
void * select_ptr( void * p )
{
	// Count # of table entries & select a random entry in that range
	register int count = 0;
	while ( ((int *)p)[count] ) count++;
	(int *)p += randrng(count);

	return (void *)((int *)p)[0];
};

/*****************************************************************************/
void replay_transition_proc( int * pargs )
{
	static image_info_t * pii[] =
	{
		&nbcfla15,&nbcfla14,&nbcfla13,&nbcfla12,&nbcfla11,
		&nbcfla10,&nbcfla09,&nbcfla08,&nbcfla07,&nbcfla06,
		&nbcfla05,&nbcfla04,&nbcfla03,&nbcfla02,&nbcfla01,
	};
	sprite_info_t * psi;
	int * flag = (int *)pargs[0];
	int i = 0;

	// replay snd effect
	snd_scall_bank(gameplay_bnk_str,132,VOLUME7,127,PRIORITY_LVL4);

	psi = beginobj(*pii, 0, VRESf, 1, SPFX_TID);
	psi->state.alpha_rgb_dst_function = GR_BLEND_ONE;
	psi->w_scale = 4.0f;
	psi->h_scale = 4.0f;
	generate_sprite_verts(psi);

	do
	{
		change_img_tmu(psi, pii[i], SPFX_TID);

		// Tell the replay when to start
		if ((pii[i] == &nbcfla06) && (flag != NULL) && (*flag < 0))
			*flag = 0;
		i++;

		sleep(2);
	} while ( i != (sizeof(pii)/sizeof(void *)) );

//	snd_scall_bank(gameplay_bnk_str,134,VOLUME7,127,PRIORITY_LVL4);

	delobj(psi);
}

/*****************************************************************************/
// Primary Game Camera
//  CAM_SIDE side view 

//BM_OFF_INTO_PLAY
//BM_FREE_ON_COURT
//BM_DRIBBLING
//BM_PASSING
//BM_SHOT_RELEASE
//BM_SHOOTING
//BM_BOUNCE_ON_RIM

//PF_SHOOTING		// player is shooting the ball
//PF_PASSING		// player is passing the ball
//PF_DUNKER			// player is dunking
//PF_ALLYDUNKER		// player is ally opp'ing
//PF_LAYUP			// player is doing a layup

static int cambot_side(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	VERTEX vtx;

	register game_info_t * pgi = &game_info;
	register ball_data * pbo = &ball_obj;
	register float fs, ft, fx, fy, fz;
	float fu;
	register int i;

	int flag = ACCLTARG | MOVETARG | MOVEVIEW;

	// Init <cambotinfo>
	// *pci = cambotinfo_init;

	// Set default track/accel for this mode
	pci->p_track = track_def;
	pci->p_accel = accel_def;

	pci->fd = SIDE_DIST_DEF;

	SetModeAngleTrgs(pcam->csi.mode);

	// Set sign-multiplier per offense
	fs = (float)(1 - (pgi->off_side + pgi->off_side));

	// Make a copy we can play with
	pci->ci_ball_mode = pgi->ball_mode;

	// Chk ball possession
	if (pgi->ball_handler < 0)
	{
		// No one has the ball
		pcam->was_turnover = 0;

		if (pgi->ball_mode == BM_PASSING)
		{
			// Pass in the air
			if (pbo->flight_time == 0)
			{
				// Build the spline on first tick
				float * fpba = (pgi->game_mode == GM_TIPOFF) ?
					&pbo->odata.x :
					&ppwt->odata.x;
				float * fptg = &pbo->tx;
				float * fpsp = &pcam->s.x1;

				do {
					ft = (fpsp[0] = *fpba++);
					fu = (fpsp[9] = *fptg++) - ft;
					fpsp[3] = ft + fu * 0.950f;					//###
					fpsp[6] = ft + fu * 0.999f;					//###
				} while (++fpsp < &pcam->s.x2);
			}
			// Track the spline path
			i = pbo->flight_time;
			if (pgi->game_mode != GM_INBOUNDING
			 && pgi->game_mode != GM_TIPOFF
			 	)
			{
				i += pbo->total_time / 4;						//###
				if (i > pbo->total_time) i = pbo->total_time;
			}
			DoSpline(&vtx, &pcam->s, i, pbo->total_time);
			fx = vtx.x;
			fy = vtx.y;
			fz = vtx.z;
		}
		else
		{
			// Loose Ball
			fx = pbo->odata.x;
			fy = pbo->odata.y;
			fz = pbo->odata.z;

			if (pgi->game_mode != GM_TIPOFF)
			{
				if (pgi->ball_mode != BM_FREE_ON_COURT)
				{
					fy *= 1.0f;//0.75f;
//					pcam->phi_trg = DG2RD(0);
				}
				else
				{
					//fy = 0.0f;
					fy *= 0.65f;								//###
					//pci->fd *= 1.4f;
					pci->handler_mode = 0;
				}
			}
		}
	}
	else
	{
		fx = ppbh->odata.x;
		fy = ppbh->odata.y;
		fz = ppbh->odata.z;
		i  = ppbh->odata.flags;
		pci->handler_mode = i;

		fx += ppbh->odata.vx;

		if (i & PF_SHOOTING && fy > 0.0f)
		{
			// Doing a jumper
			fy = fy * 1.5f + TRGY_SIDE;
			if (fy < pci->fb) fy = pci->fb;
		}
		else if (i & PF_LAYUP)
		{
			// Doing a layup

			fy = fy * 1.5f + TRGY_SIDE;
			if (fy < pci->fb) fy = pci->fb;

//			fy *= 2.0f;//1.75f;
//			if (fy < pci->fb) fy = pci->fb;
//			pcam->phi_trg = DG2RD(0);
		}
		else if (i & (PF_DUNKER|PF_ALLYDUNKER))
		{
			// Doing a dunk
			fy = fy * 1.5f + TRGY_SIDE;
			if (fy < pci->fb) fy = pci->fb;

//			fy *= 2.0f;//1.75f;
			// Pull in for certain seq types
//			if (pci->handler_mode & (PF_DUNKER|PF_ALLYDUNKER))
//				pci->fd *= 0.75f;

//			pcam->phi_trg = DG2RD(0);
		}
	}

	// Do mode-specific logic
	switch (pgi->game_mode)
	{
	case GM_PRE_TIPOFF:
	case GM_TIPOFF:
		pci->fd = (NUM_PERTEAM > 2) ? SIDE_DIST_TP3 : SIDE_DIST_TP2;

		fx = 0.0f;
		fz = TRGZ_SIDE_TIP;

		fy *= 0.80f;											//###
		if (fy < TRGY_SIDE_TIP)
			fy = TRGY_SIDE_TIP;

		pcam->was_turnover = 0;

//		flag = SNAPTARG | MOVEVIEW;
		break;

	case GM_INBOUNDING:
		if (pci->ci_ball_mode != BM_PASSING)
		{
			if (pgi->ball_handler == NO_BALLHANDLER)
			{
				fs = -fs;
			}
			else
			{
				fx = pgi->active_goal ? -TRGX_SIDE_IB : TRGX_SIDE_IB;
				fy = TRGY_SIDE_IB;
				fz = TRGZ_SIDE_IB;

				pci->fd = SIDE_DIST_IB0;

				break;
			}
		}
		pci->p_track = track_tight;
		pci->p_accel = accel_ht_ib;

		pci->inbound_mode = 0;

		pcam->was_turnover = 0;

	default:
		if (fy < TRGY_SIDE)
			fy = TRGY_SIDE;

		// Make position into normalized units
		pci->fdx = (fs * OUT_OF_BOUNDS_X + fx) * (1.0f/OUT_OF_BOUNDS_X*0.5f);
		pci->fdz = (     OUT_OF_BOUNDS_Z - fz) * (1.0f/OUT_OF_BOUNDS_Z*0.5f);

		// Do general-case theta offset
		ft = DG2RD(90) * pci->fdx * (pci->fdz > 0.5f ? 1.0f : pci->fdz + pci->fdz);
		if ((ft * fs) < DG2RD(10)) ft = DG2RD(10) * fs;
		pcam->theta_trg -= ft;

		// Slow down X-following near either endline
		//  <.5 for net-end bias, >.5 for far-end bias
		ft = pci->fdx - 0.40f * fs;
		//  < for more pronounced slowdown, > for lesser
		ft = 1.05f - ft * ft;
		if (ft > 1.0f) ft = 1.0f;
		fx *= ft;

		// Do biasing for net-end near corner
		//  > for more pronounced bias, < for lesser
		if ((ft = 0.4f - pci->fdz) < 0.0f) ft = 0.0f;
		ft *= pci->fdx * pci->fdx * fs;
		pcam->theta_trg += DG2RD(50) * ft;
		fx += 0.45f * OUT_OF_BOUNDS_X * ft;

		pci->fd += 0.55f * (OUT_OF_BOUNDS_Z - fz);
		fz += 0.60f * OUT_OF_BOUNDS_Z * pci->fdz * pci->fdz;
	}

	pci->fa = fx;
	pci->fb = fy;
	pci->fc = fz;

	return flag;
}

/*****************************************************************************/
// Primary Game Camera
//  CAM_HTIME Hangtime-ish view
static int cambot_htime(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	VERTEX vtx;

	static float fxt, fzt;
	int fib = 0;

	register bbplyr_data * ppd;
	register game_info_t * pgi = &game_info;
	register ball_data * pbo = &ball_obj;
	register float fs, ft, fx, fy, fz;
	float fb, fo, fu;
	register int i;

	int flag = ACCLTARG | MOVETARG | MOVEVIEW;

#ifdef DEBUG
	int showy = 0;
	if(showy)fprintf(stderr,"-----\r\n");
#endif

	if (pcam->new_quarter && pgi->game_quarter)
	{
		pcam->new_quarter--;
		flag |= SNAPTARG;
	}

	// Init <cambotinfo>
	// *pci = cambotinfo_init;

	// Set default track/accel for this mode
	pci->p_track = track_def;
	pci->p_accel = accel_def;

	pci->fd = HTIME_DIST_DEF;

	SetModeAngleTrgs(pcam->csi.mode);

	// Set sign-multiplier per offense
	fs = (float)(1 - pgi->off_side - pgi->off_side);

	// Make a copy we can play with
	pci->ci_ball_mode = pgi->ball_mode;

	// Chk ball possession
	if ((pgi->ball_handler < 0) && (ddunk_flg == 0))
	{
#if 0//def DEBUG
		if (pcam->was_turnover && pgi->ball_mode == BM_PASSING)
		{
			fprintf(stderr,"S-S-S-Spline Collision\r\n");
			while(!(get_player_sw_close() & P3_D))
				scan_switches();
		}
#endif
		// No one has the ball; don't clr if < 0 (running inbound)
		if (pcam->was_turnover >= 0) pcam->was_turnover = 0;

		if (pgi->ball_mode == BM_PASSING && pgi->game_mode != GM_TIPOFF)
		{
			// Pass in the air
			float * pfsp = &pcam->s.x1;

			// Chk to decide ball Y tracking
			fy = 0.0f;
			if ((ppir) && ppir->odata.flags &
					(PF_DUNKER|PF_ALLYDUNKER|PF_LAYUP))
				fy = pbo->odata.y;

			// Chk to set begining knot on first tick
			if (pbo->flight_time == 0)
			{
				float * pfbk = (pgi->game_mode == GM_TIPOFF) ?
					&pbo->odata.x :
					&ppwt->odata.x;
				pfsp[0] = pfbk[0];
				pfsp[1] = pfbk[1] + fy;
				pfsp[2] = pfbk[2];
#ifdef DEBUG
	if(showy)fprintf(stderr,"Knot y: %f\r\n",pfsp[1]);
#endif
			}
#ifdef DEBUG
	if(showy)fprintf(stderr,"Ball y: %f\r\n",pbo->odata.y);
#endif
			// Chk to update spline before pass hits target
			if (pbo->flight_time <= pbo->total_time)
			{
				float * pftk = &vtx.x;
				vtx.x = pbo->tx;
				vtx.y = pbo->ty + fy;
				vtx.z = pbo->tz;

				do {
					ft = (pfsp[0]);
					fu = (pfsp[9] = *pftk++) - ft;
					pfsp[3] = ft + fu * 0.850f;					//###
					pfsp[6] = ft + fu * 0.999f;					//###
				} while (++pfsp <= &pcam->s.z1);
			}
			// Set current tick to use & ensure current <= total
			if (pgi->game_mode == GM_INBOUNDING)
				i = (float)pbo->flight_time * 0.40f;//.50			//### <--
			else
				i = (float)pbo->flight_time * 0.60f;//.50			//### <--
			if (i > pbo->total_time)
				i = pbo->total_time;

			// Track the spline path
			DoSpline(&vtx, &pcam->s, i, pbo->total_time);
			fx = vtx.x;
			fy = vtx.y;
			fz = vtx.z;
#ifdef DEBUG
	if(showy)fprintf(stderr,"y: %f\r\n",fy);
#endif
			pcam->was_turnover = -2;
		}
		else
		{
			// Loose Ball
			fx = pbo->odata.x;
			fy = pbo->odata.y;
			fz = pbo->odata.z;

			if (pgi->game_mode != GM_TIPOFF)
			{
				if (pgi->ball_mode == BM_FREE_ON_COURT)
				{
					//fy = 0.0f;
					fy *= 0.75f;								//###
					//pci->fd *= 1.2f;							//###
					if (pgi->game_mode != GM_INBOUNDING)
						pci->handler_mode = 0;

					// Change mode if it was a dunk shot
					//if (pci->handler_mode & (PF_DUNKER))
					//	pci->ci_ball_mode = BM_OFF_INTO_PLAY;

					pci->p_accel = accel_ht_lb;
				}
				else
				{
					if (pci->handler_mode != (PF_DUNKER|PF_ALLYDUNKER))
					{
						// Take Z-biased Y ratio
						ft = -1.0f/OUT_OF_BOUNDS_Z * fz;
						// Adjust # per near:far Z
						ft *= (ft < 0.0f) ? 0.0f : 0.25f;			//###
						// Adjust # per mid-court Z
						fy *= ft + 0.75f;							//###
					//	// Set Y cap
					//	ft = RIM_Y * 1.4F;						//###
					//	if (fy > ft) fy = ft;

//						pcam->phi_trg = HTIME_PHI_LB;
						if (pgi->ball_mode == BM_SHOOTING)
							pci->p_accel = accel_ht_shooting;
					}
				}
			}
		}
	}
	else
	if (ddunk_flg != 0)
	{
		if (ppbh)
		{
			fx = ppbh->odata.x;
			fy = ppbh->odata.y;
			fz = ppbh->odata.z;
			fy += ppbh->p_spd->tall * IN2UN;
		}
		else
		{
			fx = pbo->odata.x;
			fy = pbo->odata.y;
			fz = pbo->odata.z;
		}
		if ((player_blocks[(game_info.off_side<<1)+0].odata.vy > 0) ||
			(player_blocks[(game_info.off_side<<1)+1].odata.vy > 0))
		{
			if (fy < pci->fb)
				fy = pci->fb;
		}
		pci->p_track = track_ht_dunking;
		pci->p_accel = accel_ht_dunking;
	}
	else
	{
		// Someone has the ball
		fx = ppbh->odata.x;
		fy = ppbh->odata.y;
		fz = ppbh->odata.z;
		//fx += ppbh->odata.vx;

		i = pci->handler_mode = ppbh->odata.flags;

		// Chk special cases
		if (i & PF_SHOOTING && fy > 0.0f)
		{
			// Doing a jumper

			// Take Z-biased Y ratio
			ft = -1.0f/OUT_OF_BOUNDS_Z * fz;
			// Adjust # per near:far Z
			ft *= (ft < 0.0f) ? 0.0f : 0.40f;					//###
			// Adjust # per mid-court Z
			fy *= ft + 1.0f;									//###

			// Add in minimum Y & disallow from going back down
			if ((fy += TRGY_HTIME) < pci->fb) fy = pci->fb;

//			// Do ascending ratio of default phi -> target (loose ball) phi
//			//  <total_time> = 0 if btn is released before plyr jumps
//			if ((ft = (float)ppbh->total_time) != 0.0f)
//				// < for slower change, > for quicker change
//				ft = 3.0f * (float)ppbh->flight_time / ft;		//###
//			if ((ft = 1.0f - ft) < 0.0f) ft = 0.0f;

//			pcam->phi_trg = (pcam->phi_trg - HTIME_PHI_LB) * ft + HTIME_PHI_LB;
		}
		else if (i & PF_LAYUP)
		{
			// Doing a layup
			fy *= 1.30f;										//###

			// Add in minimum Y & disallow from going back down
			if ((fy += TRGY_HTIME) < pci->fb) fy = pci->fb;

//			pcam->phi_trg = HTIME_PHI_LAY;
		}
		else if (i & (PF_DUNKER|PF_ALLYDUNKER))
		{
			// Doing a dunk
			fy += ppbh->p_spd->tall * IN2UN;
//			fy *= 1.50f;										//###

			// Add in minimum Y & disallow from going back down
			//if ((fy += TRGY_HTIME) < pci->fb) fy = pci->fb;

			// When a guy goes up for a dunk, lower camera perspective down a bit
			//pcam->phi_trg = HTIME_PHI_DUNK;
			
			// Pull in for certain seq types
			//if (i & (PF_DUNKER|PF_ALLYDUNKER))
			// Zoom in when a dunk happens
			//	pci->fd *= 0.65f;								//###

			pci->p_track = track_ht_dunking;
			pci->p_accel = accel_ht_dunking;
		}
		else if (ppbh->stick_cur != 24)
		{
			// Use alternate tracking tables
			pci->p_track = track_ht_running;
			pci->p_accel = accel_ht_running;
		}
	}

#ifdef S_CTRAK
	{
	delete_multiple_strings(0xdecaffed, 0xffffffff);
	set_string_id(0xdecaffed);
	set_text_transparency_mode(TRANSPARENCY_ENABLE);
	set_text_font(FONTID_NBA10);
	set_text_z_inc(0.01f);

	set_text_position(20, 128, 1.2f);
	oprintf("%dj%dc%s", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, "SIN");
	set_text_position(50, 128, 1.2f);
	oprintf("%dj%dc%f", (HJUST_LEFT|VJUST_CENTER), WHITE, pcam->sin_theta);

	set_text_position(20, 116, 1.2f);
	oprintf("%dj%dc%s", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, "COS");
	set_text_position(50, 116, 1.2f);
	oprintf("%dj%dc%f", (HJUST_LEFT|VJUST_CENTER), WHITE, pcam->cos_theta);

	set_text_z_inc(0.00f);
	}
#endif

	// Do mode-specific logic
	switch (pgi->game_mode)
	{
	case GM_PRE_TIPOFF:
	case GM_TIPOFF:
		pci->fd = (NUM_PERTEAM > 2) ? HTIME_DIST_TP3 : HTIME_DIST_TP2;

		if (pgi->ball_mode != BM_PASSING)
		{
			fx  = 0.0f;
			flag |= SNAPTARG;
		}
		fy *= 0.85f;											//###
		if (fy < TRGY_HTIME_TIP)
			fy = TRGY_HTIME_TIP;
	
		fz  = TRGZ_HTIME_TIP;

		pcam->was_turnover = 0;

//		flag = SNAPTARG | MOVEVIEW;
		break;

	case GM_INBOUNDING:
		if (!ib_mode)
		{
			pcam->was_turnover = 0;

		 	if (pgi->ball_handler == NO_BALLHANDLER &&
		 		pci->ci_ball_mode != BM_PASSING)
			{
				fs = -fs;
				fib = 2;
			}
			else
			{
				//FIX!!! Only supports 2 on 2
		 		if (pci->ci_ball_mode != BM_PASSING)
					ppd = player_blocks + (pgi->ball_handler ^ 1);
				else
					ppd = ppir;




				pcam->was_turnover = -1;




				if ((ppd->auto_control == NO) &&
					(ppd->odata.vx || ppd->odata.vz))
				{
					fs = -fs;
					fx = ppd->odata.x;
					fy = ppd->odata.y;
					fz = ppd->odata.z;

					pcam->was_turnover = -1;
				}
				else
		 		if (pgi->ball_handler != NO_BALLHANDLER)
					fib = 1;
			}

	//	 	if (pgi->ball_handler != NO_BALLHANDLER)
	//			fib = 1;
	//		else
	//	 	if (pci->ci_ball_mode != BM_PASSING)
	//		{
	//			fs = -fs;
	//			fib = 2;
	//		}
	//		if (pbo->int_receiver != NO_RECEIVER)
	//		{
	//			ppd = player_blocks + pbo->int_receiver;
	//			if ((ppd->auto_control == NO) &&
	//				(ppd->odata.vx || ppd->odata.vz))
	//			{
	//				fx += (ppd->odata.x - fx)*0.5f;				//###
	//				fz += (ppd->odata.z - fz)*0.5f;				//###
	//			}
	//		}

		}

//		if (!ib_mode)
//		{
//			if (pgi->ball_handler != NO_BALLHANDLER
//				|| (
//					pci->ci_ball_mode != BM_PASSING &&
//					pci->ci_ball_mode != BM_OFF_INTO_PLAY))
//			{
//				if (pci->ci_ball_mode == BM_FREE_ON_COURT)
//					pci->inbound_mode = 1;
//
////				if (pci->ci_ball_mode != BM_PASSING)
//				{
//					if (1)//FIX!!! pci->inbound_mode == 0)
//					{
//						fx = pgi->active_goal ? HTIME_THETA_IB0:HTIME_THETA_IB0_OPP;
//						fy = HTIME_PHI_IB0;
//						fz = HTIME_DIST_IB0;
//					}
//					else
//					{
//						fx = pgi->active_goal ? HTIME_THETA_IB1:HTIME_THETA_IB1_OPP;
//						fy = HTIME_PHI_IB1;
//						fz = HTIME_DIST_IB1;
//					}
//
//					pcam->theta_trg = fx;
//					pcam->phi_trg   = fy;
//					pci->fd         = fz;
//
//					fx = pgi->active_goal ? -TRGX_HTIME_IB : TRGX_HTIME_IB;
//					fy = TRGY_HTIME_IB;
//					fz = TRGZ_HTIME_IB;
//				}
//				break;
//			}
//			pci->p_track = track_tight;
//			pci->p_accel = accel_ht_ib;
//
//			pci->inbound_mode = 0;
//		}
//		pcam->was_turnover = 0;

	default:

#ifdef S_CTRAK
	{
	delete_multiple_strings(0xfadeface, 0xffffffff);
	set_string_id(0xfadeface);
	set_text_transparency_mode(TRANSPARENCY_ENABLE);
	set_text_font(FONTID_NBA10);
	set_text_z_inc(0.01f);

	set_text_position(20, 104, 1.2f);
	oprintf("%dj%dc%s", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, "FX");
	set_text_position(50, 104, 1.2f);
	oprintf("%dj%dc%f", (HJUST_LEFT|VJUST_CENTER), WHITE, fx);

	set_text_position(20, 92, 1.2f);
	oprintf("%dj%dc%s", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, "FZ");
	set_text_position(50, 92, 1.2f);
	oprintf("%dj%dc%f", (HJUST_LEFT|VJUST_CENTER), WHITE, fz);

	set_text_z_inc(0.00f);
	}
#endif
		// Make position into normalized units
		pci->fdx = (fs * OUT_OF_BOUNDS_X + fx) * (1.0f/OUT_OF_BOUNDS_X*0.5f);
		pci->fdz = (     OUT_OF_BOUNDS_Z + fz) * (1.0f/OUT_OF_BOUNDS_Z*0.5f);

		// Do applicable limits
		if (fy < TRGY_HTIME)
			fy = TRGY_HTIME;

		// Do biasing-factor (0..1) for net-end near corner
		//  < for more pronounced bias, > for lesser
		fb = (pci->fdz      - 0.40f) / (1.0f - 0.40f);			//###		.50 .50
		ft = (pci->fdx * fs - 0.70f) / (1.0f - 0.70f);			//###		.80 .80
		if (fb >= 0.0f && ft >= 0.0f)
			fb *= ft  * fs;
		else
			fb = 0.0f;

		// Do offset-factor (-1..1)
		fo  = pci->fdx - fs * 0.50f;
		fo += fo;

		// Do general-case theta offset
		pcam->theta_trg -= fo * DG2RD(16);						//###

		// Bias view angle
		//pcam->theta_trg += DG2RD(50) * fb;

#ifdef S_CTRAK
	fprintf(stderr,"fx: %f\tfz: %f\trfx: %f\trfz: %f\r\n",fx,fz,DOROTCAMX(fx,fz),DOROTCAMZ(fx,fz));
	fprintf(stderr,"cx: %f\tcz: %f\trdz: %f\trcz: %f\r\n",pcam->x,pcam->z,DOROTCAMZ(pcam->x,pcam->z) - DOROTCAMZ(fx,fz),DOROTCAMZ(pcam->x,pcam->z));
#endif

		// Pull stuff into camera space & do camera-target dZ
		ft = DOROTCAMX(fx,fz);
		fz = DOROTCAMZ(fx,fz);
		fx = ft;
		fz = DOROTCAMZ(pcam->x,pcam->z) - fz;

		// Do camera center viewline distance-to-target Z
		fz = fsqrt((pcam->y * pcam->y) + (fz * fz));

		// Do camera center viewline X-to-target dX
		//  2.0f + < moves target to screen edge,
		//  2.0f + > moves target to screen middle
		if (pgi->game_mode == GM_INBOUNDING)
			fz /= (fovfac * (2.0f + 1.60f));						//###	//2.0		//4
		else
			fz /= (fovfac * (2.0f + 1.60f));				//3.0		//###	//2.0

		// Chk X lock-down zones
		if (pci->handler_mode & (PF_DUNKER|PF_ALLYDUNKER|PF_LAYUP))
			ft = -HTIME_LOCK_DUNK;
		else
		if (pci->handler_mode & (PF_SHOOTING) && (pgi->ball_handler < 0 || pgi->game_mode == GM_INBOUNDING))
			ft = -HTIME_LOCK_SHOT;
		else
//		if (pci->ci_ball_mode != BM_FREE_ON_COURT)
			ft = -HTIME_LOCK_DEF;
//		else
//		{
//			// Clear dX & biasing-factor
//			ft = -HTIME_LOCK_LB;
////			fb = 0.0f;
////			fz = 0.0f;
//
//			if ((ft + fs * fx) > -HTIME_PRE_LOCK)
//			{
//				fx = ft + fs * fx;
//				if ((fx -= HTIME_POST_LOCK) < 0)
//				{
//					fx *= (1.0f / (HTIME_PRE_LOCK + HTIME_POST_LOCK));
//					fx *= -HTIME_PRE_LOCK * fx;
//				}
//				else
//					fx = 0.0f;
//				fx = fs * (fx - ft);
//			}
//		}
		if ((ft - fs * fx) > -HTIME_PRE_LOCK)
		{
			fx = ft - fs * fx;
			if ((fx -= HTIME_POST_LOCK) < 0)
			{
				fx *= (1.0f / (HTIME_PRE_LOCK + HTIME_POST_LOCK));
				fx *= -HTIME_PRE_LOCK * fx;
			}
			else
				fx = 0.0f;
			fx = fs * (ft - fx);
		}

		// Bias target dX
		// Bring more of the lower corners onto screen
//		fx -= 0.34f * OUT_OF_BOUNDS_X * fb;						//### was: 0.25, 0.42
//		fx -= 0.25f * OUT_OF_BOUNDS_X * fb;						//### was: 0.25, 0.42
		fx -= 0.45f * OUT_OF_BOUNDS_X * fb;						//### was: 0.25, 0.42	34

		// Do (rotated) X
		ft = fx - fs * fz;

		// Do (rotated) Z; slow down following going "in" to the screen
		//  > for more pronounced slowdown, < for lesser
		fz = pci->fdz - 0.50f;//0.20f;							//###
		if (fz < 0.0f) fz = 0.0f;
		fz *= fz;
		fz = -((OUT_OF_BOUNDS_Z - TRGZ_HTIME) * fz + TRGZ_HTIME);

		// Do general-case Z offset
		fz += fo * fo * 22.0f; //19.0f;//28.0f;									//### was: fabs(fo) * 30.0f

		// Chk if inbounding
		if (fib == 1)
		{
			// Wait for handler to pass where ball was
			if ((fxt < 0 && fxt < ft) ||
				(fxt > 0 && fxt > ft))
			{
				ft = fxt;
				fz = fzt;
			}
		}
		else
		if (fib == 2)
		{
			fxt = ft;
			fzt = fz;
		}

#ifdef S_CTRAK
	{
	delete_multiple_strings(0xdaffed, 0xffffffff);
	set_string_id(0xdaffed);
	set_text_transparency_mode(TRANSPARENCY_ENABLE);
	set_text_font(FONTID_NBA10);
	set_text_z_inc(0.01f);

	set_text_position(20, 80, 1.2f);
	oprintf("%dj%dc%s", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, "FXR");
	set_text_position(50, 80, 1.2f);
	oprintf("%dj%dc%f", (HJUST_LEFT|VJUST_CENTER), WHITE, ft);

	set_text_position(20, 68, 1.2f);
	oprintf("%dj%dc%s", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, "FZR");
	set_text_position(50, 68, 1.2f);
	oprintf("%dj%dc%f", (HJUST_LEFT|VJUST_CENTER), WHITE, fz);

	set_text_z_inc(0.00f);
	}
#endif
		// Back to world space
		fx = UNROTCAMX(ft,fz);
		fz = UNROTCAMZ(ft,fz);

		// Chk for possession change
		if ((pgi->game_mode != GM_INBOUNDING) && (
				(pcam->was_turnover == -1)
			||
				(pcam->was_turnover == -2 &&
				 pgi->ball_handler != NO_BALLHANDLER)
			||
				(pgi->off_side != pcam->off_prev &&
				 pgi->ball_handler != NO_BALLHANDLER)
			))
		{
#ifdef DEBUG
			if (pcam->was_turnover == 1) {
				fprintf(stderr,"Turnover during a turnover\r\n");
#ifdef PAUSE
				fprintf(stderr,"Any button continues\r\n");
				while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();
#endif
			}
#endif
			pcam->csi.trav_cnt = (pcam->was_turnover < 0) ? 50:50;							//### was: 35, 30 , 40 ,50
			pcam->tick_cnt     = 0;
			pcam->was_turnover = 1;
		}
		// Chk if tracking a possession change
		if (pcam->was_turnover > 0)
		{
			if (pcam->tick_cnt <= pcam->csi.trav_cnt)
			{
				// (Re)build the spline
				float * fpbp = &pcam->s.x1;
				float * fptp = &pcam->s.x4;
				float * fpsp = fpbp;

				if (!pcam->tick_cnt) fpbp = &pci->fa;

				fptp[0] = fx;
				fptp[1] = fy;
				fptp[2] = fz;

				do {
					ft = (fpsp[0] = *fpbp++);
					fu = *fptp++ - ft;
					fpsp[3] = ft + fu * 0.080f;					//### was: 100f,300f,500f
					fpsp[6] = ft + fu * 0.800f;					//###
				} while (++fpsp < &pcam->s.x2);

				// Track the spline path
				DoSpline(&vtx, &pcam->s, pcam->tick_cnt, pcam->csi.trav_cnt);
				fx = vtx.x;
				fy = vtx.y;
				fz = vtx.z;

				pcam->tick_cnt++;
			}
			else pcam->was_turnover = 0;
		}
#ifdef DEBUG
	if(showy)fprintf(stderr,"y: %f\r\n",fy);
#endif
	}
#ifdef S_CTRAK
	{
	delete_multiple_strings(0xfadecafe, 0xffffffff);
	set_string_id(0xfadecafe);
	set_text_transparency_mode(TRANSPARENCY_ENABLE);
	set_text_font(FONTID_NBA10);
	set_text_z_inc(0.01f);

	set_text_position(20, 56, 1.2f);
	oprintf("%dj%dc%s", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, "FXC");
	set_text_position(50, 56, 1.2f);
	oprintf("%dj%dc%f", (HJUST_LEFT|VJUST_CENTER), WHITE, fx);

	set_text_position(20, 44, 1.2f);
	oprintf("%dj%dc%s", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, "FZC");
	set_text_position(50, 44, 1.2f);
	oprintf("%dj%dc%f", (HJUST_LEFT|VJUST_CENTER), WHITE, fz);

	set_text_position(20, 32, 1.2f);
	oprintf("%dj%dc%s", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, "FDX");
	set_text_position(50, 32, 1.2f);
	oprintf("%dj%dc%f", (HJUST_LEFT|VJUST_CENTER), WHITE, pci->fdx);

	set_text_position(20, 20, 1.2f);
	oprintf("%dj%dc%s", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, "FDZ");
	set_text_position(50, 20, 1.2f);
	oprintf("%dj%dc%f", (HJUST_LEFT|VJUST_CENTER), WHITE, pci->fdz);

	set_text_z_inc(0.00f);

	fprintf(stderr,"fx:%f  fy:%f  fz:%f\r\n",fx,fy,fz);
	fprintf(stderr,"----\r\n");
	}
#endif

	pci->fa = fx;
	pci->fb = fy;
	pci->fc = fz;

	return flag;
}

/*****************************************************************************/
// Primary Game Camera
//  CAM_END endline view 
static int cambot_end(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	register game_info_t * pgi = &game_info;
	register obj_3d * pobj;
	register int flag = ACCLTARG | MOVETARG | MOVEVIEW;

	// Init <cambotinfo>
	// *pci = cambotinfo_init;

	// Set default track/accel for this mode
	pci->p_track = track_def_xz;
	pci->p_accel = accel_def;

	SetModeAngleTrgs(pcam->csi.mode);

	switch (pgi->game_mode) {
		case GM_PRE_TIPOFF:
		case GM_TIPOFF:
		case GM_BALL_OUT:
		case GM_IN_PLAY:
		default:
			if (pgi->ball_handler < 0) {
				pobj = &ball_obj.odata;
				pci->fb = END_MULT_Y_BALL;
			}
			else {
				pobj = &player_blocks[pgi->ball_handler].odata;
				pci->fb = END_MULT_Y_PLYR;
			}
			pci->fa  = pobj->x;
			pci->fb *= pobj->y;
			pci->fc  = pobj->z * END_MULT_Z;
			pci->fd  = END_DIST_DEF;

			if (pci->fa >  END_RANG_X) pci->fa =  END_RANG_X;
			if (pci->fa < -END_RANG_X) pci->fa = -END_RANG_X;
			if (pci->fb < TRGY_END)  pci->fb = TRGY_END;

			if (pci->handler_mode == (NO_BALLHANDLER * 2))
				pci->p_accel = accel_def_nobh;

			break;
	}
	return flag;
}

/*****************************************************************************/
// Primary Game Camera
//  CAM_SHAWN obj follower
static int cambot_shawn(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	static float scam_theta;
	static float scam_phi;
	static float scam_b;
	static float scam_d;
	static int halt_cnt_scam = -1;

	register game_info_t * pgi = &game_info;
	register ball_data * pbo = &ball_obj;

	if (halt_cnt_scam < 0)
	{
		scam_theta = DG2RD(180);
		scam_phi   = DG2RD(-89.5);
		scam_b     = 10;
		scam_d     = 225;

		halt_cnt_scam = 0;
	}

#ifdef STICK_CAM_ON
	if (halt_cnt_scam != halt_cnt)
	{
		scam_theta = pcam->theta;
		scam_phi   = pcam->phi;
		scam_b     = pcam->b;
		scam_d     = pcam->d;

		halt_cnt_scam = halt_cnt;
	}
#endif //STICK_CAM_ON

	// Init <cambotinfo>
	// *pci = cambotinfo_init;

	// Set default track/accel for this mode
	pci->p_track = track_def;
	pci->p_accel = accel_tight;

	SetModeAngleTrgs(pcam->csi.mode);

	pcam->theta_trg = scam_theta;
	pcam->phi_trg   = scam_phi;
	pci->fd         = scam_d;

	if (game_info.ball_handler > NO_BALLHANDLER)
	{
		pci->fa = player_blocks[game_info.ball_handler].odata.x;
		pci->fb = scam_b;
		pci->fc = player_blocks[game_info.ball_handler].odata.z;
	}
	else
	{
		pci->fa = ball_obj.odata.x;
		pci->fc = ball_obj.odata.z;
		if (game_info.ball_mode == BM_PASSING)
			pci->fb = scam_b;
		else
			pci->fb = ball_obj.odata.y;
	}

	return ACCLTARG | MOVETARG | MOVEVIEW;
}

/*****************************************************************************/
// Attract mode camera
//  CAM_ATTRACT
static int cambot_attract(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	register game_info_t * pgi = &game_info;
	register obj_3d * pobj;

	// Pick obj to watch
	pobj = (obj_3d *)((pgi->ball_handler >= 0) ?
			&(player_blocks[pgi->ball_handler].odata) : &(ball_obj.odata));

	// Set default track/accel for this mode
	pci->p_track = track_def;
	pci->p_accel = accel_attract;

//	if (pcam->was_playover && pgi->game_mode == GM_PRE_SNAP) {
	if (pcam->csi.mode != pcam->mode_last) {	//!!!fix!!!
		pcam->s.x1 = pci->fa = pobj->x;
		pcam->s.y1 = pci->fb = pobj->y;
		pcam->s.z1 = pci->fc = pobj->z;
#ifndef ROB
		pci->fd = (float)randrng((int)(25.0f*YD2UN)) + 12.0f*YD2UN;

		pcam->theta_trg =  DG2RD(randrng(THETA_MAX_DG));
		pcam->phi_trg   = -DG2RD(randrng(40) + 20);
#else
		pci->fd = (10.0f * YD2UN) + 12.0f * YD2UN;

		pcam->theta_trg =  DG2RD(290);
		pcam->phi_trg   = -DG2RD(20 + 20);
#endif
		pcam->s.x2 = pcam->s.y2 = pcam->s.z2 = 0.0f;

		return SNAPTARG | MOVEVIEW;
	}

	if (!(pcam->s.x2) && !(pcam->s.y2) && !(pcam->s.z2)) {
		pcam->s.x2 = pcam->x;
		pcam->s.y2 = pcam->y;
		pcam->s.z2 = pcam->z;
	}

	pcam->x = pcam->s.x2 + (pcam->a - pcam->s.x1) * 0.5f;
	pcam->y = pcam->s.y2 + (pcam->b - pcam->s.y1) * 0.5f;
	pcam->z = pcam->s.z2 + (pcam->c - pcam->s.z1) * 0.5f;

	pcam->theta_trg = pcam->theta;

	pci->fa = pobj->x;
	pci->fb = pobj->y;
	pci->fc = pobj->z;

	return ACCLTPOS | MOVETPOS | NEWTHETA;
}

/*****************************************************************************/
// Zoom the viewpoint to the specified position from <pmodeobj>
//  camera will snap if <trav_ct> <2
//  CAM_ZOOM
//  CAM_ZO_NR (no ranging)
//  CAM_ZO_NFR (no facing, no ranging)
static int cambot_zoom(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	register obj_3d * pobj = (obj_3d *)pcam->pmodeobj;
	register float fa, fb, fc;
	register int m;
	register int flag = SNAPTARG | MOVEVIEW;

	// Calc current-to-last target diffs so we can follow the target
	fa = pobj->x - pci->fa;
	fb = pobj->y + TRGY_ZOOM + pcam->csi.trgy_adj - pci->fb;
	fc = pobj->z - pci->fc;

	// Set unrotated target/distance
	pci->fa = pobj->x;
	pci->fb = pobj->y + TRGY_ZOOM + pcam->csi.trgy_adj;
	pci->fc = pobj->z;
	pci->fd = pcam->csi.d;

	if ((unsigned int)(pcam->tick_cnt) >= (unsigned int)(pcam->csi.mode_cnt)) {
		// Revert to previous mode
		flag = ModeRevert(pcam, pci);
	}
	else if (!pcam->tick_cnt) {
		// Set facing-relative theta
		pcam->csi.theta += (pcam->csi.mode != CAM_ZO_NFR) ?
				GR2RD((float)(pobj->fwd)) : pcam->theta_trg;
		pcam->csi.theta = ValidateTheta(pcam->csi.theta);

		// Does this sub-mode want d-theta limits?
		if (pcam->csi.mode == CAM_ZOOM) {
			// Do corrected-ranging for current target to desired target diff
			fa = pcam->csi.theta - pcam->theta_trg;
			fc = THETA_MAX / 2;
			if (fa <= -fc) fa += fc + fc;
			if (fa >=  fc) fa -= fc + fc;

			// Do +/- 90 deg swing limit
			fc = THETA_MAX / 4;
			if (fa >  fc) pcam->csi.theta -= 2.0f * (fa - fc);
			if (fa < -fc) pcam->csi.theta -= 2.0f * (fa + fc);
		}

		if (pcam->csi.trav_cnt < 2) {
			// Snap mode
			set_cambot_theta(pcam->csi.theta);
			set_cambot_phi(pcam->csi.phi);
		}
		else {
			// Zoom mode
			// Do initial accels to target
			pcam->theta_trg = pcam->csi.theta;
			pcam->phi_trg   = pcam->csi.phi;
			DoAccelDecel(0,pcam->csi.trav_cnt);
			// Blow off tick 0 of accelerations
			flag = MOVEVIEW;
		}
	}
	else {
		if (pcam->csi.trav_cnt < 2) {
			// Snap mode
			pcam->a += fa;
			pcam->b += fb;
			pcam->c += fc;
		}
		else {
			// Zoom mode
			// Follow the target
			pcam->a += fa;
			pcam->b += fb;
			pcam->c += fc;

			DoAccelDecel(pcam->tick_cnt,pcam->csi.trav_cnt);
			flag = MOVETARG | MOVEVIEW;
		}
	}

	pcam->tick_cnt++;

	return flag;
}

/*****************************************************************************/
// Attach the viewpoint to a 2-point Float Spline in <*pseqinfo>
//  spline is projected over a period of <trav_cnt> ticks
//  starting point is the current viewpoint
//  ending point is specified by the CAMSEQINFO offsets from target
//  points are assumed to have been rotated & normalized to the Z-axis
//    such that pt1 would have been (0,0,0) & pt4 would have been (0,0,1)
//  points are rotated, in XZ & Y, per angles formed by start & end points
//  points are ratioed, in XZ & Y, per distance between start & end points
//  points are translated, in XZ & Y, per position of start & end points
//  CAM_SPLINE initial theta is offset (limited) by <pmodeobj.fwd> (facing);
//         spline is stretched to follow <pmodeobj> target
//  CAM_SP_NR initial theta is offset by <pmodeobj.fwd> (facing, no ranging);
//         spline is stretched to follow <pmodeobj> target
//  CAM_SP_NFR initial theta is offset by current <theta_trg> (no facing,
//         no ranging); spline is stretched to follow <pmodeobj> target
//  CAM_SP_CNFR initial theta is offset by current <theta_trg> (no facing,
//         no ranging); uses <cambot> as the target & watches <pmodeobj>
static int cambot_spline(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	static float trgx, trgy, trgz;
	obj_3d * pobj = (obj_3d *)pcam->pmodeobj;
	float tsin, tcos, fd1, fd2;

	if ((unsigned int)(pcam->tick_cnt) >= (unsigned int)(pcam->csi.mode_cnt))
	{
		// Revert to previous mode
		return ModeRevert(pcam, pci);
	}
	else if (!pcam->tick_cnt)
	{
		// 1st time here; setup spline array
		float * pfs = (float *)pcam->csi.pseqinfo;
		float fdx, fdy, fdz;

		// Set facing-relative theta
		pcam->csi.theta += (pcam->csi.mode == CAM_SPLINE || pcam->csi.mode == CAM_SP_NR) ?
				GR2RD((float)(pobj->fwd)) : pcam->theta_trg;
		pcam->csi.theta = ValidateTheta(pcam->csi.theta);

		// Does this sub-mode want d-theta limits?
		if (pcam->csi.mode == CAM_SPLINE)
		{
			// Do corrected-ranging for current target to desired target diff
			fd1 = pcam->csi.theta - pcam->theta_trg;
			fd2 = THETA_MAX / 2;
			if (fd1 <= -fd2) fd1 += fd2 + fd2;
			if (fd1 >=  fd2) fd1 -= fd2 + fd2;

			// Do +/- 90 deg swing limit
			fd2 = THETA_MAX / 4;
			if (fd1 < -fd2) pcam->csi.theta -= 2.0f * (fd1 + fd2);
			if (fd1 >  fd2) pcam->csi.theta -= 2.0f * (fd1 - fd2);
		}

		fd1 = fcos(pcam->csi.phi) * (fd2 = pcam->csi.d);

		if (pcam->csi.mode == CAM_SP_CNFR)
		{
			fdx = pcam->a;
			fdy = pcam->b;
			fdz = pcam->c;

			// Init focus pt & distance
			pcam->a = pobj->x;
			pcam->b = pobj->y;
			pcam->c = pobj->z;
			pcam->d = pcam->csi.d;
		}
		else
		{
			fdx = pobj->x;
			fdy = pobj->y;
			fdz = pobj->z;
		}

		// Set spline destination pt4, starting pt1, & deltas
		fdx = (pcam->s.x4 = (trgx = fdx) - fsin(pcam->csi.theta) * fd1) - (pcam->s.x1 = pcam->x);
		fdy = (pcam->s.y4 = (trgy = fdy) - fsin(pcam->csi.phi)   * fd2) - (pcam->s.y1 = pcam->y);
		fdz = (pcam->s.z4 = (trgz = fdz) - fcos(pcam->csi.theta) * fd1) - (pcam->s.z1 = pcam->z);

		fd1 = fsqrt((fd2 = fdx * fdx + fdz * fdz) + fdy * fdy);
		fd2 = fsqrt(fd2);

		if (fabs(fd1) < 0.000001f) fd1 = (fd1 < 0.0f) ? -0.000001f : 0.000001f;
		if (fabs(fd2) < 0.000001f) fd2 = (fd2 < 0.0f) ? -0.000001f : 0.000001f;

		// Rotate in X
		// Knots 2 & 3 can rotate in X since they are normalized to Z
		tsin = fdy / fd1;
		tcos = fd2 / fd1;
		pcam->s.y2 = pfs[2] * tsin + pfs[1] * tcos;
		pcam->s.z2 = pfs[2] * tcos - pfs[1] * tsin;
		pcam->s.y3 = pfs[5] * tsin + pfs[4] * tcos;
		pcam->s.z3 = pfs[5] * tcos - pfs[4] * tsin;

		// Rotate in Y
		tsin = fdx / fd2;
		tcos = fdz / fd2;
		fd2 = pfs[0];
		pcam->s.x2 = pcam->s.z2 * tsin + fd2 * tcos;
		pcam->s.z2 = pcam->s.z2 * tcos - fd2 * tsin;
		fd2 = pfs[3];
		pcam->s.x3 = pcam->s.z3 * tsin + fd2 * tcos;
		pcam->s.z3 = pcam->s.z3 * tcos - fd2 * tsin;

		// Scale & translate
		pcam->s.x2 = pcam->s.x2 * fd1 + pcam->s.x1;
		pcam->s.y2 = pcam->s.y2 * fd1 + pcam->s.y1;
		pcam->s.z2 = pcam->s.z2 * fd1 + pcam->s.z1;
		pcam->s.x3 = pcam->s.x3 * fd1 + pcam->s.x1;
		pcam->s.y3 = pcam->s.y3 * fd1 + pcam->s.y1;
		pcam->s.z3 = pcam->s.z3 * fd1 + pcam->s.z1;

		// Clear accelerations we care about
		pcam->xa = pcam->ya = pcam->za = pcam->da = 0.0f;

#ifdef DBG_EOQ
		if (get_player_sw_close() & P4_D){
		#define RD2DG(x) ((180.0f * (float)x) * (1.0f / (float)M_PI))
		float dx = pobj->x-cambot.s.x4;
		float dy = pobj->y-cambot.s.y4;
		float dz = pobj->z-cambot.s.z4;
		float d = fsqrt(dx*dx+dy*dy+dz*dz);
		float dxz = fsqrt(dx*dx+dz*dz);
		float ac = RD2DG(acos(dz/dxz));
		float as = RD2DG(asin(dx/dxz));
		if (dz < 0.0f) as = 180.0f - as;
		else
		if (dx < 0.0f) as = 360.0f + as;
		if (dx < 0.0f) ac = 360.0f - ac;
		fprintf(stderr,"X1=%10.4f  Y1=%10.4f  Z1=%10.4f\r\n",cambot.s.x1,cambot.s.y1,cambot.s.z1);
		fprintf(stderr,"X2=%10.4f  Y2=%10.4f  Z2=%10.4f\r\n",cambot.s.x2,cambot.s.y2,cambot.s.z2);
		fprintf(stderr,"X3=%10.4f  Y3=%10.4f  Z3=%10.4f\r\n",cambot.s.x3,cambot.s.y3,cambot.s.z3);
		fprintf(stderr,"X4=%10.4f  Y4=%10.4f  Z4=%10.4f\r\n",cambot.s.x4,cambot.s.y4,cambot.s.z4);
		fprintf(stderr,"XO=%10.4f  YO=%10.4f  ZO=%10.4f  d=%10.4f  csi.d=%10.4f\r\n",pobj->x,pobj->y,pobj->z,d,cambot.csi.d);
		fprintf(stderr,"CSI=%7.2f  TRG=%7.2f  AC=%7.2f  AS=%7.2f\r\n",RD2DG(cambot.csi.theta),RD2DG(cambot.theta_trg),ac,as);}
#endif //DBG_EOQ
	}

	if (pcam->csi.mode == CAM_SP_CNFR)
	{
		// Set default track/accel for this mode
		pci->p_track = track_p2p;
		pci->p_accel = accel_p2p;

		pcam->b = pobj->y * 0.4f;

		// Disallow Y from going less than starting target Y
		if (pcam->b < (trgy + pcam->csi.trgy_adj))
			pcam->b =  trgy + pcam->csi.trgy_adj;

		// Follow Y
		pci->fb = pcam->b;
	}
	else
	{
		// Track obj movement & current position
		fd1  = pobj->x - trgx;
		fd2  = pobj->z - trgz;
		trgx = pobj->x;
		trgz = pobj->z;

		// Stretch the spline to follow any object movement
		pcam->s.x4 += (fd1);
		pcam->s.z4 += (fd2);
		pcam->s.x3 += (fd1 * (2.0f/3.0f));
		pcam->s.z3 += (fd2 * (2.0f/3.0f));
		pcam->s.x2 += (fd1 * (1.0f/3.0f));
		pcam->s.z2 += (fd2 * (1.0f/3.0f));
//		delete_multiple_strings(0xbbbbcccc,-1);
//		set_string_id(0xbbbbcccc);
//		set_text_font(FONTID_NBA10);
//		set_text_justification_mode(HJUST_LEFT|VJUST_TOP);
//		set_text_color(LT_YELLOW);
//		set_text_z_inc(-0.01f);
//		set_text_position(256,273,2);
//		oprintf("XV %f",fd1);
//		set_text_position(256,260,2);
//		oprintf("ZV %f",fd2);
//		set_text_z_inc( 0.00f);

		// Set default track/accel for this mode
		pci->p_track = track_tight;//def;
		pci->p_accel = accel_def;

		// Do buffered acceleration for Y
		pci->fb = pobj->y + TRGY_SPLINE + pcam->csi.trgy_adj;
	}

	// Update target
	// Do buffered acceleration for X, Z, & D
	pci->fa = pobj->x;
	pci->fc = pobj->z;
	pci->fd = pcam->d;

	if (pcam->tick_cnt <= pcam->csi.trav_cnt)
		DoSpline((VERTEX *)&(pcam->x), &(pcam->s), pcam->tick_cnt, pcam->csi.trav_cnt);

	pcam->tick_cnt++;

	return ACCLTPOS | MOVETPOS | NEWTHETA;
}

/*****************************************************************************/
// Attach the viewpoint to a 4-point Rail Spline in <*pseqinfo>
//  spline is projected over a period of <trav_cnt> ticks
//  points are assumed to have been rotated to the Z-axis
//  points are rotated, in Y, per initial theta angle
//  points are ratioed, in XZ & Y, per specified <csi.d>
//  points are translated, in XZ & Y, to initial <pmodeobj> viewpoint target
//  CAM_RAIL initial theta is offset by <pmodeobj.fwd> (facing)
//  CAM_RA_NF initial theta is offset by current <theta_trg> (no facing)
//  CAM_RA_NFT initial theta is offset by current <theta_trg> (no facing);
//         offsets from the world-origin while it watches the <pmodeobj>
//         viewpoint target (no tracking)
static int cambot_rail(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	register obj_3d * pobj = (obj_3d *)pcam->pmodeobj;

	if ((unsigned int)(pcam->tick_cnt) >= (unsigned int)(pcam->csi.mode_cnt)) {
		// Revert to previous mode
		return ModeRevert(pcam, pci);
	}
	else if (!pcam->tick_cnt) {
		// 1st time here; setup spline matrix
		register float * pfs = (float *)pcam->csi.pseqinfo;
		register float * pfc = (float *)&pcam->s.x1;
		register float tsin, tcos, fx, fz, fd;
		register int i;

		// Set facing-relative theta
		pcam->csi.theta += (pcam->csi.mode == CAM_RAIL) ?
				GR2RD((float)(pobj->fwd)) : pcam->theta_trg;
		pcam->csi.theta = ValidateTheta(pcam->csi.theta);

		tsin = fsin(pcam->csi.theta);
		tcos = fcos(pcam->csi.theta);
		fd = pcam->csi.d;

		// Rotate in Y
		i = 4;
		while (i--) {
			fx = pfs[0];
			fz = pfs[2];
			pfc[0] = fd * (fz * tsin + fx * tcos);
			pfc[1] = fd * (pfs[1]);
			pfc[2] = fd * (fz * tcos - fx * tsin);
			if (pcam->csi.mode != CAM_RA_NFT) {
				pfc[0] += pobj->x;
				pfc[1] += pobj->y;
				pfc[2] += pobj->z;
			}
			pfs += 3;
			pfc += 3;
		}
	}

	// Update target
	pcam->a = pobj->x;
	pcam->b = pobj->y + TRGY_RAIL;
	pcam->c = pobj->z;

	if (pcam->tick_cnt <= pcam->csi.trav_cnt)
		DoSpline((VERTEX *)&(pcam->x), &(pcam->s), pcam->tick_cnt, pcam->csi.trav_cnt);
//	if (pcam->tick_cnt == pcam->csi.trav_cnt)
//		pcam->s.y1 = pcam->s.y2 = pcam->s.y3 = pcam->s.y4;
//	DoSpline((VERTEX *)&(pcam->x), &(pcam->s), pcam->tick_cnt, pcam->csi.trav_cnt);

	pcam->tick_cnt++;

	return NEWTHETA;
}

/*****************************************************************************/
// Play back a Coordinate Data point-stream from <*pseqinfo>
//  0,0,0 signifies the end of the stream
//  points are assumed to have been rotated to the Z-axis
//  points are rotated, in Y, per initial theta angle
//  points are ratioed, in XZ & Y, per specified <csi.d>
//  CAM_PATH initial theta is offset by <pmodeobj.fwd> (facing);
//         follows the <pmodeobj> viewpoint target
//  CAM_PA_NF initial theta is offset by current <theta_trg> (no facing);
//         follows the <pmodeobj> viewpoint target
//  CAM_PA_NFT initial theta is offset by current <theta_trg> (no facing)
//         offsets from the world-origin while it watches the <pmodeobj>
//         viewpoint target (no tracking)
static int cambot_path(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	static float v_sin, v_cos;

	while (!pcam->csi.mode_cnt ||
			(unsigned int)(pcam->tick_cnt) < (unsigned int)(pcam->csi.mode_cnt)) {
		register obj_3d * pobj;
		register float * pf = (float *)pcam->csi.pseqinfo;
		register float fa = pf[0];
		register float fb = pf[1];
		register float fc = pf[2];
		register float fd;

		if (!fa && !fb && !fc) break;

		pobj = (obj_3d *)pcam->pmodeobj;

		if (!pcam->tick_cnt) {
			// Do first time setup
			// Set facing-relative theta
			pcam->csi.theta += (pcam->csi.mode == CAM_PATH) ?
					GR2RD((float)(pobj->fwd)) : pcam->theta_trg;
			pcam->csi.theta = ValidateTheta(pcam->csi.theta);

			v_sin = fsin(pcam->csi.theta);
			v_cos = fcos(pcam->csi.theta);
		}
		// Unrotate to new angle
		fd = fa;
		fa = fc * v_sin + fd * v_cos;
		fc = fc * v_cos - fd * v_sin;

		fd = pcam->csi.d;

		if (pcam->csi.mode == CAM_PA_NFT) {
			pcam->a = pobj->x;
			pcam->b = pobj->y * 0.5f;
			pcam->c = pobj->z;
			pcam->x = fa * fd;
			pcam->y = fb * fd;
			pcam->z = fc * fd;
		}
		else {
			pcam->x = fa * fd + (pcam->a = pobj->x);
			pcam->y = fb * fd + (pcam->b = pobj->y);
			pcam->z = fc * fd + (pcam->c = pobj->z);
		}
		pcam->b += TRGY_PATH;

		pcam->csi.pseqinfo = pf + 3;
		pcam->tick_cnt++;

		return NEWTHETA;
	}

	// Revert to previous mode
	pcam->csi.trav_cnt = 0;
	return ModeRevert(pcam, pci);
}

/*****************************************************************************/
// Play back a Coordinate Data point-stream from <*pseqinfo>
//  0,0,0 signifies the end of the stream & would be the viewpoint target
//  points are rotated, in Y, to the Z-axis
//  points are rotated, in Y, per initial angle of the viewpoint to target
//  points are ratioed, in XZ & Y, per initial distances from viewpoint to target
//  point projection results in 1st point being at the current viewpoint
//  CAM_COURSE uses as the target & follows <pmodeobj>
//  CAM_CO_C uses <cambot> as the target & follows <pmodeobj>
//  CAM_CO_CNT uses <cambot> as the target (no tracking)
static int cambot_course(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	static float n_sin, n_cos;
	static float v_sin, v_cos;
	static float drat_h, drat_y;

	while (!pcam->csi.mode_cnt ||
			(unsigned int)(pcam->tick_cnt) < (unsigned int)(pcam->csi.mode_cnt)) {
		register obj_3d * pobj;
		register float * pf = (float *)pcam->csi.pseqinfo;
		register float fa = pf[0];
		register float fb = pf[1];
		register float fc = pf[2];
		register float fd, fe;

		if (!fa && !fb && !fc) break;

		pobj = (obj_3d *)pcam->pmodeobj;

		if (!pcam->tick_cnt) {
			// Do first time setup
			n_sin = fa * (fd = rsqrt(fa * fa + fc * fc));
			n_cos = fc * fd;

			if (pcam->csi.mode == CAM_COURSE) {
				pcam->a = pobj->x;
				pcam->b = pobj->y;
				pcam->c = pobj->z;
			}
			if (pcam->csi.mode != CAM_CO_CNT) {
				pci->fa = pobj->x;
				pci->fb = pobj->y;
				pci->fc = pobj->z;
			}
			fe = fsqrt(((fe = v_sin = pcam->x - pcam->a) * fe) +
					   ((fe = v_cos = pcam->z - pcam->c) * fe));
			v_sin /= fe;
			v_cos /= fe;

			drat_h = fe * fd;
			drat_y = fabs((pcam->y - pcam->b) / fb);

			// Set up to quickly compensate for any position offset between the
			//  object & the initial camera focus, making the object the focus
			pci->fb += TRGY_COURSE;	//pcam->csi.d;
			DoAccelDecel(0, 50);
			pci->fb -= TRGY_COURSE;	//pcam->csi.d;

			pcam->s.x1 = pcam->s.y1 = pcam->s.z1 = 0.0f;
		}
		else {
			if (pcam->csi.mode != CAM_CO_CNT) {
				// Track object new position offsets
				BuffAccel(P_S_X1, pobj->x - pci->fa, TRACK_DEFAULT, ACCEL_DEFAULT);
				BuffAccel(P_S_Y1, pobj->y - pci->fb, TRACK_DEFAULT, ACCEL_DEFAULT);
				BuffAccel(P_S_Z1, pobj->z - pci->fc, TRACK_DEFAULT, ACCEL_DEFAULT);
				pcam->s.x1 *= DECEL_DEFAULT;
				pcam->s.y1 *= DECEL_DEFAULT;
				pcam->s.z1 *= DECEL_DEFAULT;
				pci->fa += pcam->s.x1;
				pci->fb += pcam->s.y1;
				pci->fc += pcam->s.z1;

				// Update focus with initial compensation & any new offset
				DoAccelDecel(pcam->tick_cnt, 50);
				pcam->a += (pcam->xv += pcam->xa);
				pcam->b += (pcam->yv += pcam->ya);
				pcam->c += (pcam->zv += pcam->za);

				pcam->a += pcam->s.x1;
				pcam->b += pcam->s.y1;
				pcam->c += pcam->s.z1;
			}
		}
		// Rotate to Z
		fd = fa;
		fa = fd * n_cos - fc * n_sin;
		fc = fd * n_sin + fc * n_cos;

		// Unrotate to new angle
		fd = fa;
		fa = fc * v_sin + fd * v_cos;
		fc = fc * v_cos - fd * v_sin;

		pcam->x = fa * drat_h + pcam->a;
		pcam->y = fb * drat_y + pcam->b;
		pcam->z = fc * drat_h + pcam->c;

		pcam->csi.pseqinfo = pf + 3;
		pcam->tick_cnt++;

		return NEWTHETA;
	}
	// Revert to previous mode
	pcam->csi.trav_cnt = 0;
	return ModeRevert(pcam, pci);
}

/*****************************************************************************/
// Play back a Normalized Data point-stream from <*pseqinfo>
//  0,0,0 signifies the end of the stream
//  points are assumed to have been rotated & normalized to the Z-axis
//    such that 1st pt (deleted) would have been (0,0,0) & last will be (0,0,x)
//  points are rotated, in XZ & Y, per angles formed by start & end points
//  points are ratioed, in XZ & Y, per initial distance from viewpoint to target
//  point projection results in 1st point being at the current viewpoint
//  CAM_P2P uses <cambot> as the target & watches <pmodeobj>
//  CAM_P2P_X is same but negates X
static int cambot_p2p(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	static float t_sin, t_cos;
	static float p_sin, p_cos;
	static float drat;

	while (!pcam->csi.mode_cnt ||
			(unsigned int)(pcam->tick_cnt) < (unsigned int)(pcam->csi.mode_cnt)) {
		register obj_3d * pobj = (obj_3d *)pcam->pmodeobj;
		register float * pf = (float *)pcam->csi.pseqinfo;
		register float fa = pf[0];
		register float fb = pf[1];
		register float fc = pf[2];
		register float fd, fe;

		if (pcam->csi.mode == CAM_P2P_X) fa = -fa;

		if (pobj == &ball_obj.odata && game_info.ball_handler >= 0)
			pobj = &(player_blocks[game_info.ball_handler].odata);

		if (!fa && !fb && !fc) {
			// If <mode_cnt> is specified, the viewpoint remains where it
			//  was last while still tracking the target
			if (!pcam->csi.mode_cnt)
				break;
		}
		else {
			if (!pcam->tick_cnt) {
				register int cnt;
				// Do first time setup
				// Set theta for destination pt calc
				pcam->csi.theta = ValidateTheta((pcam->csi.theta += pcam->theta_trg));

				// Do destination pt & deltas; save target pt; save starting pt for later translations
				fe = (fd = pcam->csi.d) * fcos(pcam->csi.phi);
				fa = ((pcam->s.x2 = pcam->a) - fe * fsin(pcam->csi.theta)) - (pcam->s.x1 = pcam->x);
				fb = ((pcam->s.y2 = pcam->b) - fd * fsin(pcam->csi.phi))   - (pcam->s.y1 = pcam->y);
				fc = ((pcam->s.z2 = pcam->c) - fe * fcos(pcam->csi.theta)) - (pcam->s.z1 = pcam->z);

				fd = fsqrt((fe = fa * fa + fc * fc) + fb * fb);
				fe = fsqrt(fe);

				// Set trigs for rotate in X
				p_sin = fb / fd;
				p_cos = fe / fd;

				// Set trigs for rotate in Y
				t_sin = fa / fe;
				t_cos = fc / fe;

				// Find last pt; abort after <cnt> pts
				cnt = 2000;
				while ((pf[0] || pf[1] || pf[2]) && --cnt)
					pf += 3;
				if (!cnt)
					break;
				// We "know" X & Y should be 0 so just use Z for unit equivalence
				pf--;
				drat = fd / *pf;

				// Init focus pt & distance
				pcam->a = pobj->x;
				pcam->b = pobj->y;
				pcam->c = pobj->z;
				pcam->d = pcam->csi.d;

				// Clear accelerations we care about
				pcam->xa = pcam->ya = pcam->za = pcam->da = 0.0f;

				// "Retrieve" 1st normalized pt
				fa = fb = fc = 0.0f;
			}
			else
				pcam->csi.pseqinfo = pf + 3;

			// Set default track/accel for this mode
			pci->p_track = track_p2p;
			pci->p_accel = accel_p2p;

			// Unrotate in X
			fd = fb;
			fb = fc * p_sin + fd * p_cos;
			fc = fc * p_cos - fd * p_sin;
			// Unrotate in Y
			fd = fa;
			fa = fc * t_sin + fd * t_cos;
			fc = fc * t_cos - fd * t_sin;

			pcam->x = fa * drat + pcam->s.x1;
			pcam->y = fb * drat + pcam->s.y1;
			pcam->z = fc * drat + pcam->s.z1;
		}

		// Update target
		pci->fa = pobj->x;
		pci->fb = pobj->y * 0.4f;
		pci->fc = pobj->z;

		// Disallow Y from going less than starting target Y
		if (pci->fb < pcam->s.y2) pci->fb = pcam->s.y2;
		if (pcam->b < pcam->s.y2) pcam->b = pcam->s.y2;

		pcam->tick_cnt++;

		return ACCLTPOS | MOVETPOS | NEWTHETA;
	}

	// Revert to previous mode
	pcam->csi.trav_cnt = 2;
	pcam->dv = pcam->xv = pcam->yv = pcam->zv = pcam->da = pcam->xa = pcam->ya = pcam->za = 0.0f;
/*{
int flag;
fprintf(stderr,"th= %f  ph= %f\r\n\r\n",pcam->theta,pcam->phi);
fprintf(stderr," x= %f   y= %f   z= %f\r\n\r\n",pcam->x,pcam->y,pcam->z);
fprintf(stderr," a= %f   b= %f   c= %f   d= %f\r\n",pcam->a,pcam->b,pcam->c,pcam->d);
fprintf(stderr,"ta= %f  tb= %f  tc= %f  td= %f\r\n\r\n",pcam->s.x2,pcam->s.y2,pcam->s.z2,pcam->csi.d);
fprintf(stderr,"xv= %f  yv= %f  zv= %f  dv= %f\r\n",pcam->xv,pcam->yv,pcam->zv,pcam->dv);
fprintf(stderr,"xa= %f  ya= %f  za= %f  da= %f\r\n\r\n",pcam->xa,pcam->ya,pcam->za,pcam->da);
	flag = ModeRevert(pcam, pci);
fprintf(stderr," a= %f   b= %f   c= %f   d= %f\r\n",pci->fa,pci->fb,pci->fc,pci->fd);
fprintf(stderr,"th= %f  ph= %f\r\n\r\n",pcam->theta,pcam->phi);
	return flag;
}*/
	return ModeRevert(pcam, pci);
}

/*****************************************************************************/
// Put the camera undergound, looking down
//  CAM_GOPHER
static int cambot_gopher(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	// Set unrotated target/distance
	pcam->x = -1000.0f;
	pcam->y = -10.0f;
	pcam->z = 0.0f;

	pcam->a = -900.0f;
	pcam->b = -75.0f;
	pcam->c = 0.0f;

	return DOBYPASS;
}

/*****************************************************************************/
// Resume previous camera mode
// Set <trav_cnt> to control what ModeRevert() does to the camera:
//  < 2 returns SNAPTARG|MOVEVIEW, >= 2 returns what <mode_prev> returns
//  CAM_RESUME
static int cambot_resume(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	return ModeRevert(pcam, pci);
}

#ifdef SPECIAL_MODES
/*****************************************************************************/
// "Helmet" camera
//  CAM_HELMET

#define DIFF_MAX 20

static int cambot_helmet(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	obj_3d * pobj;
	int pnum,diff;

	// Determine player # to track
	pnum = GUARD;
	if (game_info.off_side) pnum += NUM_PERTEAM;
	if (game_info.game_mode == GM_IN_PLAY) {
		if ((pnum = game_info.ball_handler) < 0)
			pnum = ball_obj.int_receiver;
	}
	pobj = &player_blocks[pnum].odata;

	pcam->x = pobj->x;
	pcam->y = pobj->y + 15.0f;
	pcam->z = pobj->z;

	helmet_tang = pobj->fwd & 1023;
	diff = helmet_tang - helmet_fang;
	if (diff) {
		if (diff < -512)
			diff += 1024;
		else if (diff > 512)
			diff -= 1024;
		if (diff < -DIFF_MAX)
			diff = -DIFF_MAX;
		else if (diff > DIFF_MAX)
			diff = DIFF_MAX;
		helmet_fang += diff;
	}
	helmet_fang &= 1023;
	pcam->a = pcam->x + 10.0f * isin(helmet_fang);
	pcam->b = pcam->y - 2.0f;
	pcam->c = pcam->z + 10.0f * icos(helmet_fang);

	return DOBYPASS;
}
#endif //SPECIAL_MODES

/*****************************************************************************/
// Special-case camera controls/modifiers
//
static int cambot_special( VIEWCOORD * pcam, CAMBOTINFO * pci, int flags )
{
	register game_info_t * pgi = &game_info;
	register ball_data * pbo = &ball_obj;

#ifdef DEBUG
	{
	// Debug slow-mo button
	int tcnt = 6000;
	while((get_player_sw_current() & P3_D) && !(get_player_sw_current() & P3_C) && (tcnt--))
		scan_switches();
	}
#endif

#if 1
	////////////////////////////////
	// Handle special camera for shot ball-follow
	{
		static int tog = 0;

		if (pgi->ball_mode == BM_SHOOTING &&
			pbo->total_time > 45 &&									//### min ticks
			pbo->total_time < 85)									//### max ticks
		{
			if (pcam->csi.mode == NM_DUNK)
			{
				// Move camera
				pcam->x += pcam->xv;
				pcam->y += pcam->yv;
				pcam->z += pcam->zv;

				// Ensure "last" acceleration leaves velocity = 0.0
				if (fabs(pcam->xv) <= fabs(pcam->xa)) pcam->xa = -pcam->xv;
				if (fabs(pcam->yv) <= fabs(pcam->ya)) pcam->ya = -pcam->yv;
				if (fabs(pcam->zv) <= fabs(pcam->za)) pcam->za = -pcam->zv;

				// Accelerate camera
				pcam->xv += pcam->xa;
				pcam->yv += pcam->ya;
				pcam->zv += pcam->za;

				flags = DOBYPASS;
			}
			else
			if ((!tog)
				&& (p_status == 0)
				&& ((pbo->flight_time * 3) > pbo->total_time)		//### cur/tot ratio
				)
			{
				tog =1;

				// Show ball on jumpshots
				if (randrng(100) < 10)								//### active odds
				{
					register float fa, fb, fc, fd, fe;

					fa = pcam->a = (pgi->active_goal) ? RIM_X : -RIM_X;
					fb = pcam->b = RIM_Y;
					fc = pcam->c = RIM_Z;

					fa -= pbo->odata.x;
					fb -= pbo->odata.y;
					fc -= pbo->odata.z;

					fe = sqrt((fd = fa * fa + fc * fc) + fb * fb);
					fd = sqrt(fd);

					fa /= fd;	// sin theta
					fb /= fe;	// sin phi
					fc /= fd;	// cos theta
					fd /= fe;	// cos phi

					fe += 35 + randrng(40);							//### distance
					fd *= fe;

					pcam->x = pcam->a - fa * fd;
					pcam->y = pcam->b - fb * fe;
					pcam->z = pcam->c - fc * fd;

					fa = (float)(pbo->flight_time - pbo->total_time);

					pcam->xa = pcam->xv = pbo->odata.vx;
					pcam->ya = pcam->yv = pbo->odata.vy;
					pcam->za = pcam->zv = pbo->odata.vz;

					pcam->xa /= fa;
					pcam->ya /= fa;
					pcam->za /= fa;

					pcam->csi.mode = NM_DUNK;

					flags = DOBYPASS;
				}
			}
		}
		else
		{
			if ((pcam->csi.mode == NM_DUNK) && (
				(pbo->odata.y < (RIM_Y - 8.0f)) ||					//### height
				(pbo->odata.y > (RIM_Y + 8.0f) &&					//### height
				 	pbo->odata.vy > 0)
				))
			{
				if (pgi->ball_handler != NO_BALLHANDLER ||
					pgi->ball_mode == BM_OFF_INTO_PLAY ||
					pgi->ball_mode == BM_FREE_ON_COURT ||
					pgi->ball_mode == BM_BOUNCE_ON_RIM)
				{
					pcam->csi.mode = CAM_RESUME;

					flags = DOBYPASS;
				}
			}
			tog = 0;
		}
	}
#endif

#if 0
	////////////////////////////////
	// Handle special camera for dunks
	{
		static CAMSEQINFO csi_dunk1 = {
			CAM_ZO_NR, 0, 0, DG2RD(0.0f), DG2RD(-45.0f), 200.0f, NULL
		};
		static int last_ht = -1;

		if (pgi->ball_handler >= 0)
		{
			if (last_ht < 0)
			{
				register int ht = player_blocks[pgi->ball_handler].hangtime;

				if (ht >= 0)
				{
					set_recorder_mode(RR_RESTART);
					last_ht = ht;
				}
			}
		}
		else if (last_ht >= 0)
		{
#ifdef DEBUG
			if (0)//get_player_sw_current() & (P1_A+P2_A+P3_A+P4_A))
#endif //DEBUG
			{
			switch (randrng(3))
			{
				case 0:

				case 1:

				default:
					pcam->csi          = csi_dunk1;
					pcam->csi.mode_cnt = last_ht * 2;
					pcam->csi.theta    = DG2RD((float)randrng(360));

					pcam->s.x1 = (pgi->active_goal) ? RIM_X-10.0f : -RIM_X+10.0f;
					pcam->s.y1 = 0.0f;
					pcam->s.z1 = 0.0f;

					pcam->csi.trgy_adj = 0.0f;

					pcam->pmodeobj = &pcam->s;
					pcam->tick_cnt = 0;

					set_playback_mode(RP_STARTALL,0,0,1);
					break;
			}
			}
			last_ht = -1;
		}
	}
#endif

//	GM_PRE_TIPOFF,						// Pre-game
//	GM_TIPOFF,							// Tip-off
//	GM_IN_PLAY,							// Active gameplay
//	GM_INBOUNDING,						// In-bounding after score/ob
//*	GM_BALL_OUT,						// 
//	GM_GAME_OVER,						// Game over
//	GM_INITIALIZING,					// set before game starts
//	GM_FREE_THROW						// after X fouls

	////////////////////////////////
	// Do instant replays
	{
		// Store previous & get current player flags
		bbplyr_data * ppbh = player_blocks + MAX_PLYRS;
		bbplyr_data * ppws;
		bbplyr_data * ppwt;
		bbplyr_data * ppir;
		int * pfp = flags_prev + MAX_PLYRS;
		int * pfc = flags_curr + MAX_PLYRS;
		do {
			ppbh--; pfp--; pfc--;
			*pfp = *pfc;
			*pfc = ppbh->odata.flags;
		} while (pfc != flags_curr);

		ppws = pbo->who_shot     == NO_SHOOTER     ? NULL : ppbh + pbo->who_shot;
		ppwt = pbo->who_threw    == NO_PASSER      ? NULL : ppbh + pbo->who_threw;
		ppir = pbo->int_receiver == NO_RECEIVER    ? NULL : ppbh + pbo->int_receiver;
		ppbh = pgi->ball_handler == NO_BALLHANDLER ? NULL : ppbh + pgi->ball_handler;

		// Chk for reset conditions
		if (pgi->game_mode == GM_INITIALIZING
			|| (pgi->game_mode == GM_PRE_TIPOFF)
			|| (pgi->game_mode == GM_TIPOFF)
			|| (pgi->game_mode == GM_INBOUNDING && pgi->ball_mode == BM_PASSING)
			|| (pgi->game_state == GS_EOQ)
			)
		{
			// Re-init stuff
			pcsi          = NULL;
			pcam->pop_flg = POP_IDLE;
			pop_dly       = 0;
			gip_cnt       = 0;
			gpo_cnt       = 0;

			event.fwd = 0;
		}
		else
		// Chk in-play conditions
		if (pgi->game_mode == GM_IN_PLAY && recorder)
		{
			gip_cnt++;
		
			{	// Chk for cool dunk
				if ((pgi->ball_mode == BM_SHOOTING)
					&& (ppws)
//					&& (ppws->odata.flags & (PF_DUNKER|PF_ALLYDUNKER|PF_LAYUP))
					&& (ppws->odata.flags & (PF_DUNKER))
					)
				{
					if (pcam->pop_flg < POP_COOLDUNK)
					{
						if ((ppi = select_ptr(ppi_cooldunk1)))
						{
#ifdef STICK_CAM_ON
							pppi = ppi_cooldunk1;
#endif
							// New; set event data
							event.x  = pbo->odata.x;
							event.y  = pbo->odata.y;
							event.z  = pbo->odata.z;
							pop_fwd  = ppws->odata.fwd;
							pop_plyr = ppws->plyrnum;
							pop_team = ppws->team;
			
							pcam->pop_flg = POP_COOLDUNK;
							pop_cnt       = 0;
						}
					}
				}
				// Cool dunk won't unselect itself here
			}

			{	// Chk for layup
				if ((pgi->ball_mode == BM_SHOOTING)
					&& (ppws)
					&& (ppws->odata.flags & (PF_LAYUP))
					)
				{
					if (pcam->pop_flg < POP_LAYUP)
					{
						if ((ppi = select_ptr(ppi_layup1)))
						{
#ifdef STICK_CAM_ON
							pppi = ppi_layup1;
#endif
							// New; set event data
							event.x  = pbo->odata.x;
							event.y  = pbo->odata.y;
							event.z  = pbo->odata.z;
							pop_fwd  = ppws->odata.fwd;
							pop_plyr = ppws->plyrnum;
							pop_team = ppws->team;
			
							pcam->pop_flg = POP_LAYUP;
							pop_cnt       = 0;
						}
					}
				}
				// Layup won't unselect itself here
			}

			{	// Chk for cool juke
				if (pcam->pop_flg < POP_COOLJUKE)
				{
					// The if is a little redundant, but what the heck...
					if ((ppbh)
						&& (juker_pnum == ppbh->plyrnum)
						&& (juker_pnum != NO_JUKER)
						&& (jukee_pnum != NO_JUKEE)
						)
					{
						if ((ppi = select_ptr(ppi_cooljuke1)))
						{
#ifdef STICK_CAM_ON
							pppi = ppi_cooljuke1;
#endif
#ifdef DEBUG
							fprintf(stderr,"JUKER:%d JUKEE:%d\r\n",juker_pnum,jukee_pnum);
#endif
							// New; set event data
							event.x  = ppbh->odata.x;
							event.y  = ppbh->odata.y;
							event.z  = ppbh->odata.z;
							pop_fwd  = ppbh->odata.fwd;
							pop_plyr = ppbh->plyrnum;
							pop_team = ppbh->team;
			
							pcam->pop_flg = POP_COOLJUKE;
							pop_cnt       = 0;
						}
					}
				}
				else
				// Chk juke insta-kill cases
				// Kill if lost possession or
				//   doing a freethrow or
				//   violation
				if ((pcam->pop_flg == POP_COOLJUKE) && (
					(pop_team != pgi->off_side) ||
					(pgi->game_mode == GM_FREE_THROW) ||
					(existp(VIOLATION_PID, -1))))
				{
#ifdef DEBUG
					fprintf(stderr,"JUKE replay killed\r\n");
#endif
					pcam->pop_flg = POP_IDLE;
				}
			}
		}
		else
		// Chk play-over conditions
		if (pgi->game_mode == GM_INBOUNDING && pgi->ball_mode != BM_PASSING && recorder)
		{
			gpo_cnt++;

			{	// Chk for juke finish
				if ((pcam->pop_flg == POP_COOLJUKE) &&
					(pop_team == pgi->off_scored) &&
					(ppi->ecnt > (pop_cnt + POP_CJUKE_FCNT)))
				{
					// Advance post-event count for consistent finish after
					//  the score; will further preceed the juke event
					pop_cnt = ppi->ecnt - POP_CJUKE_FCNT;
#ifdef DEBUG
					fprintf(stderr,"JUKE replay FINISH\r\n");
#endif
				}
			}
		}

		// Chk to inc the pop-replay post-event count
		if (pcam->pop_flg != POP_IDLE && pcam->pop_flg < POP_ACTIVE)
		{
			if (++pop_cnt >= ppi->ecnt)
			{
				// Do any required event-validation before
				// starting the pop-replay
				switch (pcam->pop_flg)
				{
					case POP_COOLDUNK:
					{
						// Chk insta-kill cases
						// Kill if it was goaltended
						if (ib_mode == -1)
						{
							pcam->pop_flg = POP_IDLE;
							break;
						}
#ifdef DEBUG
						if (1)		// 1 to chk the kill, 0 to not
#endif
						// Chk % to NOT kill the replay
						if (
							(pgi->game_mode != GM_INBOUNDING)
							||
							((pgi->game_state == GS_ATTRACT_MODE ? 60 : 30) < randrng(100))
						) {
							pcam->pop_flg = POP_IDLE;
							break;
						}
					}
					if (0)
					case POP_LAYUP:
					{
						// Chk insta-kill cases
						// Kill if it was goaltended or
						//   has been to long with no score
						if ((ib_mode == -1) ||
							(pop_dly > (int)(57.0f * 2.0f)))
						{
							pcam->pop_flg = POP_IDLE;
							break;
						}
						// Chk if replay can be shown yet
						if (pgi->game_mode != GM_INBOUNDING)
						{
							pop_dly++;
							break;
						}
#ifdef DEBUG
						if (1)		// 1 to chk the kill, 0 to not
#endif
						// Chk % to NOT kill the replay
						if (
							(pop_plyr != pgi->sc_scored)
							||
							((pgi->game_state == GS_ATTRACT_MODE ? 60 : 30) < randrng(100))
						) {
							pcam->pop_flg = POP_IDLE;
							break;
						}
					}
					if (0)
					case POP_COOLJUKE:
					{
						// Chk insta-kill cases
						// Kill if not inbounding (no score) or
						//   not the scoring team or
						//   doing a freethrow or
						//   violation
						if ((pgi->game_mode != GM_INBOUNDING) ||
							(pop_team != pgi->off_scored) ||
							(pgi->game_mode == GM_FREE_THROW) ||
							(existp(VIOLATION_PID, -1)))
						{
#ifdef DEBUG
							fprintf(stderr,"JUKE replay killed\r\n");
#endif
							pcam->pop_flg = POP_IDLE;
							break;
						}
#ifdef DEBUG
						if (1)		// 1 to chk the kill, 0 to not
#endif
						// Chk % to NOT kill the replay
						if (
							((pgi->game_state == GS_ATTRACT_MODE ? 80 : 50) < randrng(100))
						) {
#ifdef DEBUG
							fprintf(stderr,"JUKE replay killed: pop_team:%d  off_scored:%d\r\n",pop_team,pgi->off_scored);
#endif
							pcam->pop_flg = POP_IDLE;
							break;
						}
#ifdef DEBUG
						fprintf(stderr,"JUKE replay ACTIVE\r\n");
#endif
					}
					if (!pup_no_replays)
//					if (1)
					default:
					{
				//		int pmsk = 1 << (MAX_PLYRS - 1);
				//		int bmsk = (P_A|P_B|P_C) << ((MAX_PLYRS - 1) * 8);

						// Threshhold crossed; flag replay events
				//		if (pgi->game_state == GS_ATTRACT_MODE)
				//			attract_flag |= (1<<pcam->pop_flg);
				//		else
				//			inplay_flag |= (1<<pcam->pop_flg);

						// Start the pop-replay
						pcsi          = ppi->pcsi;
						pcam->pop_flg = POP_ACTIVE;
				//		pop_btn       = 0;
						pop_cnt       = 0;
						pop_itm       = 0;

						// Launch transition with ptr to pause flag to clr
						// when its time to proceed with the replay
						qcreate("rtrans", 0, replay_transition_proc , (int)&pop_slp, 0, 0, 0);
						pop_slp = -1;

				//		// Make active button mask
				//		while (pmsk)
				//		{
				//			if (p_status & pmsk)
				//				pop_btn |= bmsk;
				//			bmsk >>= 8;
				//			pmsk >>= 1;
				//		}
					}
				}
			}
			else
			{
				pop_dly = 0;
			}
		}

		if (pcam->pop_flg == POP_ACTIVE && pop_cnt > 0)
		{
			if (--pop_cnt)
			{
				// Put replay to sleep if camera is to move first
				if (pop_slp)
				{
					add_sleep_process_id( REPLAY_PID, pop_slp );
					pop_cnt += pop_slp;
					pop_slp  = 0;
				}
				// Chk for button-out
				if (pcam->tick_cnt > 25 || pop_itm > 1)
				{
					int i = MAX_PLYRS;
					while (i--)
					{
						if ((p_status & (1<<i)) &&
							((get_but_val_cur(i) & (~D_BUTTON)) == (A_BUTTON|B_BUTTON|C_BUTTON)))
						{
							pcam->tick_cnt = pcam->csi.mode_cnt;
							pcam->csi.trav_cnt = 0;
							pop_cnt = 0;
							while (pcsi->mode_cnt != 0)
								pcsi++;
							break;
						}
					}
				}
			}
		}

#ifdef STICK_CAM_ON
		// Chk to restart/edit the replay
		{
			static int txt = 0;
			static int cnt = 0;
			static int idx;
			if (txt)
				delete_multiple_strings(0xdadadada,-1);
			txt = 0;
			if (pcam->pop_flg == POP_ACTIVE && pop_itm)
			{
				static int tog = 0;
				static int swl = 0;
#if STICK_CAM_SW == 4
				int sw = get_p5_sw_current()&255;
#else
				int sw = (get_player_sw_current() >> (STICK_CAM_SW * 8))&255;
#endif //STICK_CAM_SW == 4
				if (cnt == 0)
				{
					while (pppi[cnt])
					{
						if (pppi[cnt] == ppi)
							idx = cnt;
						cnt++;
					}
				}
				if (sw & STICK_CAM_BT)
				{
					static float sw_vel[] = {0.0f, -0.5f,  0.5f};
					static float sw_acc[] = {0.0f, -0.04f, 0.04f};
					static float svelx = 0.0f, svely = 0.0f;
					float fx_nam = 256;
					float fx_csi = 210;
					float fy_csi = 192;
					float fyoff = -13;
					int i = 0;
					int c0 = LT_GREEN;
					int c1 = WHITE;
					int nm = (ppi->pcsi->mode < -1);

					if (svelx == 0.0f || !((sw&15)>>2))
						svelx  = sw_vel[(sw&15)>>2];
					else
						svelx += sw_acc[(sw&15)>>2];
					if (svely == 0.0f || !(sw &3))
						svely  = sw_vel[sw &3];
					else
						svely += sw_acc[sw &3];

					while(ppi->szname[i])
						if (ppi->szname[i++] == '_')
							ppi->szname[i-1]  = '-';

					if (tog == 0)
					{
						if ((((swl ^ sw) & sw) & 15) == P_UP)
						{
							if (--idx < 0) idx = cnt - 1;
						}
						if ((((swl ^ sw) & sw) & 15) == P_DOWN)
						{
							if (++idx == cnt) idx = 0;
						}
						ppi = pppi[idx];
					}
					else
					if (tog == 1)
					{
						ppi->pcsi->theta = ValidateTheta(ppi->pcsi->theta + DG2RD(svelx * 0.25f));
						ppi->pcsi->phi =   ValidatePhi(ppi->pcsi->phi +     DG2RD(svely * 0.25f));
					}
					else
					{
						ppi->pcsi->d -= svelx;
						ppi->pcsi->trgy_adj -= svely;
						if (nm)
							ppi->pcsi[1].trgy_adj -= svely;
					}

					set_string_id(0xdadadada);
					set_text_font(FONTID_NBA10);
					set_text_transparency_mode(TRANSPARENCY_ENABLE);
					set_text_z_inc(0.01f);

					set_text_position(fx_nam, fy_csi, 1.2f);
					oprintf("%dj%dccsip_%s",(HJUST_CENTER|VJUST_CENTER),
						(tog==0)?c1:LT_YELLOW,ppi->szname);
					fy_csi += fyoff;
					set_text_position(fx_csi, fy_csi, 1.2f);
					oprintf("%dj%dc%s%dc%f",(HJUST_LEFT|VJUST_CENTER),
						LT_YELLOW,"T: ",(tog==1)?c1:c0, (ppi->pcsi->theta*180.0f)/M_PI);
					fy_csi += fyoff;
					set_text_position(fx_csi, fy_csi, 1.2f);
					oprintf("%dj%dc%s%dc%f",(HJUST_LEFT|VJUST_CENTER),
						LT_YELLOW,"P: ",(tog==1)?c1:c0, (ppi->pcsi->phi*180.0f)/M_PI);
					fy_csi += fyoff;
					set_text_position(fx_csi, fy_csi, 1.2f);
					oprintf("%dj%dc%s%dc%f",(HJUST_LEFT|VJUST_CENTER),
						LT_YELLOW,"D: ",(tog==2)?c1:c0, ppi->pcsi->d);
					fy_csi += fyoff;
					set_text_position(fx_csi, fy_csi, 1.2f);
					oprintf("%dj%dc%s%dc%f",(HJUST_LEFT|VJUST_CENTER),
						LT_YELLOW,"Y: ",(tog==2)?c1:c0, ppi->pcsi->trgy_adj);

					set_text_z_inc(0.00f);
					txt = 1;

					// Restart the replay
					pcsi    = ppi->pcsi;
					pop_cnt = 0;
					pop_itm = 0;
					pop_slp = 0;
				}
				else
				if (swl & STICK_CAM_BT)
				{
					tog = ++tog % 3;
				}
				swl = sw;
			}
			else
				cnt = 0;
		}
#endif //STICK_CAM_ON

		// Chk if waiting for transition
		if (pcam->pop_flg == POP_ACTIVE && pop_cnt == 0 && pop_slp < 0)
		{
			// Inc tick delay cnt
			pop_dly++;
		}

		if (pcam->pop_flg == POP_ACTIVE && pop_cnt == 0 && pop_slp == 0 && pcsi->mode_cnt != 0)
		{
			// Do replay only if any delay is still within the tape limits
			if ((pop_dly + ppi->rcnt) < REPLAY_FRAMES)
			{
				pop_cnt = set_playback_mode(RP_START,
					pop_dly + ppi->rcnt,
					pop_dly + 1,
					ppi->rdly);
#ifdef DEBUG
				fprintf(stderr,"Replay tried to run csip_%s\r\n",ppi->szname);
#endif //DEBUG
			}
			if (
				((pcsi->trav_cnt <  0) && (pcsi->mode_cnt == (pop_cnt))) ||
				((pcsi->trav_cnt >= 0) && (pcsi->mode_cnt == (pop_cnt + pcsi->trav_cnt)))
			   )
			{
				int idx, mode;
#ifdef DEBUG
				fprintf(stderr,"Replay selection is running\r\n");
#endif //DEBUG
				// Set target obj ptr per PI spec
				// Will default to the ball if a player was specified
				// but that value was < 0
				pcam->pmodeobj = &event;		// assume default
				switch (ppi->tobj)
				{
					case POP_TOBJ_PLAYER:
						idx = pop_plyr;
						if (0)					// skip next line
					case POP_TOBJ_WHO_SHOT:
						idx = pbo->who_shot;
						if (0)					// skip next line
					case POP_TOBJ_WHO_THREW:
						idx = pbo->who_threw;
						if (0)					// skip next line
					case POP_TOBJ_INT_RECEIVER:
						idx = pbo->int_receiver;
						if (idx < 0)			// do next line if invalid
					case POP_TOBJ_BALL:
						idx = BALL_INDEX - PLYR_INDEX;
						pcam->pmodeobj = get_replay_obj_ptr(idx + PLYR_INDEX);
				}
				SetModeAngleTrgs(pcam->mode_prev);

				do
				{
					// Copy seq control data to cambot struct &
					// reset elapsed-time to start`er up
					pcam->csi = *pcsi;
					pcam->tick_cnt = 0;

					// Convert any n-mode
					if (pcam->csi.mode == NM_RSNAP_NR)
						pcam->csi.mode = CAM_ZO_NR;
					if (pcam->csi.mode == NM_RSNAP_NFR)
						pcam->csi.mode = CAM_ZO_NFR;

					// Chk if its a No Facing mode
					if ((pcam->csi.mode == CAM_ZO_NFR) ||
						(pcam->csi.mode == CAM_SP_NFR) ||
						(pcam->csi.mode == CAM_SP_CNFR)||
						(pcam->csi.mode == CAM_RA_NF)  ||
						(pcam->csi.mode == CAM_RA_NFT) ||
						(pcam->csi.mode == CAM_PA_NF)  ||
						(pcam->csi.mode == CAM_PA_NFT))
					{
						// Clr <theta_trg> so CSI theta will be the mode target
						pcam->theta_trg = DG2RD(0);

						// Chk if theta_trg or csi.theta should flip
						if (pop_team)
						{
							if (ppi->tobj == POP_TOBJ_EVENT && pop_team)
								pcam->theta_trg = DG2RD(180);
							else
								pcam->csi.theta = -pcam->csi.theta;
						}
					}
					else
					{
						// Add event frame facing so CSI theta applies to the event frame
						// Subtract 1st frame facing which gets added back by camera mode
						pcam->csi.theta += GR2RD((float)
							(pop_fwd - ((obj_3d *)pcam->pmodeobj)->fwd));
					}

					// Set replay sleep-time, if any, so camera can move first
					if (pcam->csi.trav_cnt >= 0)
						pop_slp = pcam->csi.trav_cnt;
					else
						pcam->csi.trav_cnt = -pcam->csi.trav_cnt;

					// Do mode specifics to setup for upcoming replay draw
					flags = cambot_funcs[pcam->csi.mode](pcam, pci);

					if (pcsi->mode < 0)
						DoSnap(pcam, pci, flags);

					pop_itm++;
				} while (pcsi++->mode < 0);

				// Inc <mode_cnt> to make up for the <cambot_funcs> call
				pcam->csi.mode_cnt++;
			}
			else
			{
				// Abort if desired # of ticks couldn't happen
				pop_cnt = 0;

				// Go to end of script if it had been running
				while (pop_itm > 0 && pcsi->mode_cnt != 0)
					pcsi++;
			}
		}
		if ((pcam->pop_flg == POP_ABORT) ||
			(pcam->pop_flg == POP_ACTIVE && pop_cnt == 0 && pop_slp == 0))
		{
			// Turn off replay & restart recorder
			set_playback_mode(RP_STOP, 0, 0, 0);
			set_recorder_mode(RR_START);

			pcam->pop_flg = POP_IDLE;

			// There is a camera to return only if script was running
			if (pcam->pop_flg_last == POP_ACTIVE && pop_itm > 0)
			{
				// Chk & set whether or not camera does a snap return
				pcam->csi.trav_cnt = (pcsi->mode == CAM_ZOOM) ? 2 : 0;

				flags = ModeRevert(pcam, pci);

				// Do the unrotate if mode spec'd
				if (flags & UNROTATE) {
					register float fa = pci->fa;
					pci->fa = pci->fc * pcam->sin_theta + fa * pcam->cos_theta;
					//pci->fb = pci->fb;
					pci->fc = pci->fc * pcam->cos_theta - fa * pcam->sin_theta;
				}
			}
		}
		pcam->pop_flg_last = pcam->pop_flg;
	}

	////////////////////////////////
	// Handle special camera for initial tipoff

#ifndef NO_TIP
	if (pgi->game_state == GS_ATTRACT_MODE)
#endif
		pcam->new_quarter = 0;

	if (pcam->new_quarter && !pgi->game_quarter)
	{
		static float vtx_pts[] = {
			-45.0f,		0.0f,	-5.0f,
			-45.0f,		0.0f,	 5.0f
		};
		static VERTEX vtx;
		static TO_INFO * pti;
		static CAMSEQINFO * pcsi;				// ptr to CAMSEQINFO seq table
		static int csi_cnt; 					// CAMSEQINFO seq item count

		float f;

		// Chk if first time here or if last transient is about to expire
		if (pcam->new_quarter <= 1 ||
			pcam->tick_cnt == pcam->csi.mode_cnt)
		{
			// Chk first time initialization
			if (pcam->new_quarter <= 1)
			{
				SetModeAngleTrgs(pcam->mode_prev);

				pti = select_ptr(ppto_info);
#ifdef DBG_KO
				if (pcam->new_quarter < 0)
					pti = ppto_info[-1 - pcam->new_quarter];
#endif
				pcam->new_quarter = 2;

				pcam->a = 0.0f;
				pcam->b = TRGY_HTIME_TIP;
				pcam->c = TRGZ_HTIME_TIP;

				pcam->x = pti->x;
				pcam->y = pti->y;
				pcam->z = pti->z;
				csi_cnt = pti->csi_cnt;
				pcsi = pti->pcsi;

				// create the referee
				set_process_run_level(qcreate("referee",REFEREE_PID,referee_proc,pti->ref_cnt,0,0,0),RL_PLAYER);

				// create the jump meters
				set_process_run_level(qcreate("meter1",JUMP_METER_PID,make_jump_meter_proc,0,pti->jm0_cnt,0,0),RL_JMETER+0);
				set_process_run_level(qcreate("meter2",JUMP_METER_PID,make_jump_meter_proc,1,pti->jm1_cnt,0,0),RL_JMETER+1);
				set_process_run_level(qcreate("jmeter",JUMP_METER_PID,do_jump_meters_proc,0,0,0,0),RL_JMETER+2);
			}

			// Do next script item till done
			if (csi_cnt--)
			{
				// Get next script element
				pcam->csi = *pcsi++;

				pcam->pmodeobj = &vtx;
				pcam->tick_cnt = 0;
			}
#ifdef STICK_CAM_ON
#ifdef DBG_KO
			delete_multiple_strings(0xfdeb, 0xffffffff);
			if (!dbg_ko) {
//				fprintf(stderr,"Cam\tx= %f\ty= %f\tz= %f\r\n",pcam->x,pcam->y,pcam->z);
//				fprintf(stderr,"\ta= %f\tb= %f\tc= %f\r\n",pcam->a,pcam->b,pcam->c);
//				fprintf(stderr,"Ball\tx= %f\ty= %f\tz= %f\r\n",ball_obj.odata.x,ball_obj.odata.y,ball_obj.odata.z);
			}
			else {
				float fx_to = (pti->ti_idx < 0) ? 210 : 80;
				float fy_to = 192;
				float fx_ti = 210;
				float fy_ti = 192;
				float fx_csi = 340;
				float fy_csi = 192;
				float fyoff = -12;
				int i = 0;
				int c0 = LT_GREEN;
				int c1 = (pti->ti_idx < 0 &&
#if STICK_CAM_SW == 4
					get_p5_sw_current() & STICK_CAM_BT) ?
#else
					get_player_sw_current() >> (STICK_CAM_SW * 8)) ?
#endif //STICK_CAM_SW == 4
					WHITE : c0;

				set_string_id(0xfdeb);
				set_text_font(FONTID_NBA10);
				set_text_transparency_mode(TRANSPARENCY_ENABLE);
				set_text_z_inc(0.01f);

				while(pti->szname[i])
					if (pti->szname[i++] == '_')
						pti->szname[i-1]  = '-';

				set_text_position(fx_to, fy_to, 1.2f);
				oprintf("%dj%dc%s",(HJUST_LEFT|VJUST_CENTER),
					LT_YELLOW,pti->szname);
				fy_to += fyoff;
				set_text_position(fx_to, fy_to, 1.2f);
				oprintf("%dj%dc%s%dc%f",(HJUST_LEFT|VJUST_CENTER),
					LT_YELLOW,"X: ",(pti->bt_idx==TO_MXZ)?c1:c0, pti->x);
				fy_to += fyoff;
				set_text_position(fx_to, fy_to, 1.2f);
				oprintf("%dj%dc%s%dc%f",(HJUST_LEFT|VJUST_CENTER),
					LT_YELLOW,"Y: ",(pti->bt_idx==TO_RFY)?c1:c0, pti->y);
				fy_to += fyoff;
				set_text_position(fx_to, fy_to, 1.2f);
				oprintf("%dj%dc%s%dc%f",(HJUST_LEFT|VJUST_CENTER),
					LT_YELLOW,"Z: ",(pti->bt_idx==TO_MXZ)?c1:c0, pti->z);
				fy_to += fyoff;
				set_text_position(fx_to, fy_to, 1.2f);
				oprintf("%dj%dc%s%dc%d",(HJUST_LEFT|VJUST_CENTER),
					LT_YELLOW,"REF-CNT: ",(pti->bt_idx==TO_RFY)?c1:c0, pti->ref_cnt);
				fy_to += fyoff;
				set_text_position(fx_to, fy_to, 1.2f);
				oprintf("%dj%dc%s%dc%d",(HJUST_LEFT|VJUST_CENTER),
					LT_YELLOW,"JM0-CNT: ",(pti->bt_idx==TO_T01)?c1:c0, pti->jm0_cnt);
				fy_to += fyoff;
				set_text_position(fx_to, fy_to, 1.2f);
				oprintf("%dj%dc%s%dc%d",(HJUST_LEFT|VJUST_CENTER),
					LT_YELLOW,"JM1-CNT: ",(pti->bt_idx==TO_T01)?c1:c0, pti->jm1_cnt);

				if (pti->ti_idx >= 0) {
					CAMSEQINFO * pcsi = &pti->pcsi[pti->ti_idx];
					set_text_position(fx_ti, fy_ti, 1.2f);
					oprintf("%dj%dc%s",(HJUST_LEFT|VJUST_CENTER),
						LT_YELLOW,"CSI ITEMS:");
					i = 0;
					while(i < pti->csi_cnt) {
						fy_ti += fyoff;
						set_text_position(fx_ti, fy_ti, 1.2f);
						oprintf("%dj%dc%s",(HJUST_LEFT|VJUST_CENTER),
							(i==pti->ti_idx)?WHITE:LT_GRAY, szmode[pti->pcsi[i].mode]);
						i++;
					}
					c1 = WHITE;
					set_text_position(fx_csi, fy_csi, 1.2f);
					oprintf("%dj%dc%s",(HJUST_LEFT|VJUST_CENTER),
						LT_YELLOW,"CSI PARMS:");
					fy_csi += fyoff;
					set_text_position(fx_csi, fy_csi, 1.2f);
					oprintf("%dj%dc%s%dc%d",(HJUST_LEFT|VJUST_CENTER),
						LT_YELLOW,"M-CNT: ",(pti->bt_idx==TI_TMT)?c1:c0, pcsi->mode_cnt);
					fy_csi += fyoff;
					set_text_position(fx_csi, fy_csi, 1.2f);
					oprintf("%dj%dc%s%dc%d",(HJUST_LEFT|VJUST_CENTER),
						LT_YELLOW,"T-CNT: ",(pti->bt_idx==TI_TMT)?c1:c0, pcsi->trav_cnt);
					fy_csi += fyoff;
					set_text_position(fx_csi, fy_csi, 1.2f);
					oprintf("%dj%dc%s%dc%f",(HJUST_LEFT|VJUST_CENTER),
						LT_YELLOW,"T: ",(pti->bt_idx==TI_ROT)?c1:c0, (pcsi->theta*180.0f)/M_PI);
					fy_csi += fyoff;
					set_text_position(fx_csi, fy_csi, 1.2f);
					oprintf("%dj%dc%s%dc%f",(HJUST_LEFT|VJUST_CENTER),
						LT_YELLOW,"P: ",(pti->bt_idx==TI_ROT)?c1:c0, (pcsi->phi*180.0f)/M_PI);
					fy_csi += fyoff;
					set_text_position(fx_csi, fy_csi, 1.2f);
					oprintf("%dj%dc%s%dc%f",(HJUST_LEFT|VJUST_CENTER),
						LT_YELLOW,"D: ",(pti->bt_idx==TI_MVD)?c1:c0, pcsi->d);
				}
				set_text_z_inc(0.00f);
			}
#endif //DBG_KO
#endif //STICK_CAM_ON
		}
		// Update camera target
		f = (float)pcam->tick_cnt / (float)pcam->csi.mode_cnt;

		vtx.x = (vtx_pts[3] - vtx_pts[0]) * f + vtx_pts[0];
		vtx.y = (vtx_pts[4] - vtx_pts[1]) * f + vtx_pts[1];
		vtx.z = (vtx_pts[5] - vtx_pts[2]) * f + vtx_pts[2];

		// Init camera mode if 1st tick
		if (csi_cnt >= 0 && pcam->tick_cnt == 0)
			flags = cambot_funcs[pcam->csi.mode](pcam, pci);

		// Inc button-out delay timer?
		if (pcam->new_quarter > 1)
			pcam->new_quarter++;

		// Chk if done or button-out
		if (pcam->new_quarter >= 60)
		{
			int i = MAX_PLYRS;
			while (i--)
			{
				if ((p_status & (1<<i)) &&
					((get_but_val_cur(i) & (~D_BUTTON)) == (A_BUTTON|B_BUTTON|C_BUTTON)))
				{
					csi_cnt = -1;
				}
			}
		}
		if (csi_cnt < 0)
		{
			pcam->csi.mode = pcam->mode_prev;
			pcam->new_quarter = 0;
			pgi->game_mode = GM_TIPOFF;
		}
	}

#ifndef NOT_READY
#ifndef NOSETCAM

	////////////////////////////////
	// Chk special camera for initial QB idiot box
	if (idiot_flag < 1)
	{
		static CAMSEQINFO csi_qb_idiot = {
			CAMBOT_ZO_NR, 1000, 0, DG2RD(90.0f), DG2RD(-15.0f), 1500.0f, NULL
		};
		static obj_3d obj_qb_idiot;

		if (idiot_boxes_shown < 0)	// makes plyr2 rotate back: (&& pcam->was_playover <= 1)
		{
			idiot_flag = idiot_boxes_shown;

			if (pcam->csi.mode != CAMBOT_ZO_NR)
			{
				register obj_3d * pobj = &obj_qb_idiot;

				pcam->csi      = csi_qb_idiot;
				pcam->pmodeobj = pobj;

				pcam->a = pci->fa = pobj->x = 0.0f;
				pcam->b = pci->fb = pobj->y = 0.0f;
				pcam->c = pci->fc = pobj->z = 0.0f;

				pobj->fwd = (game_info.last_pos) ? 512 : 0;

				pcam->tick_cnt = 0;
			}
		}
		else if (idiot_boxes_shown > 0  && idiot_flag < 0) {

			idiot_flag = idiot_boxes_shown;

			pcam->csi.mode_cnt = pcam->tick_cnt;
			pcam->csi.trav_cnt = 2;
		}
	}	

	////////////////////////////////
	// Do special camera for possession changes; works only if other procs
	//  don't use <suspend> during GM_IN_PLAY/GM_PLAY_OVER else contention
	//  is possible
	if (game_info.game_mode == GM_IN_PLAY || game_info.game_mode == GM_PLAY_OVER)
	{
		static int tout;
		int msgnum = -1;

		if (pcam->off_prev != game_info.last_pos)
		{
			if (pcam->off_prev < 0)
			{
				// Wait for message to timeout before turning everyone back on
				if (!existp(MESSAGE_PID+1,0xFFFFFFFF) || !(--tout))
				{
					resume_all();
					pcam->off_prev = (game_info.game_mode == GM_IN_PLAY) ?
							game_info.last_pos : game_info.off_side;
				}
			}
			else if (game_info.game_mode == GM_IN_PLAY)
			{
				if (!(ball_obj.flags & BF_BOUNCE) &&
						(ball_obj.type == LB_PASS ||
						 ball_obj.type == LB_BOBBLE ||
						 ball_obj.type == LB_LATERAL))
				{
					msgnum = MS_INTERCEPTION;
				}
				else if (ball_obj.type == LB_FUMBLE ||
						 ball_obj.type == LB_LATERAL)
				{
					msgnum = MS_TURNOVER2;
				}
				else if (((game_info.game_flags & (GF_BOBBLE | GF_BALL_PUNTED | GF_FUMBLE)) == GF_BALL_PUNTED) &&
						(game_info.game_state != GS_ATTRACT_MODE))
				{
					msgnum = MS_PUNTRETURN;
				}
				else
				{
					pcam->off_prev = game_info.last_pos;
				}
			}
		}
		// Chk very special camera for interception at play over
		else if (game_info.game_mode == GM_PLAY_OVER)
		{
			if (pcam->off_prev != game_info.off_side)
			{
				if (!(ball_obj.flags & BF_BOUNCE) &&
						game_info.play_result != PR_FAILED_CONVERSION &&
						!game_info.off_turnover &&
						(ball_obj.type == LB_PASS ||
						 ball_obj.type == LB_BOBBLE ||
						 ball_obj.type == LB_LATERAL))
				{
					msgnum = MS_INTERCEPTION;
				}
				else
				{
					pcam->off_prev = game_info.off_side;
				}
			}
		}
		if (msgnum >= 0)
		{
			suspend_multiple_processes(0, NODESTRUCT);
			resume_multiple_processes(PLYRINFO_PID, 0xfffffffe);	// don't look at low bit
			resume_multiple_processes(POST_PLAYER_PID, -1);

			qcreate("message", MESSAGE_PID+1, show_message_proc, msgnum, 0, 0, 0);
			
			sleep(40);

			tout = 6 * tsec;

			pcam->off_prev = -1;
		}
	}
	else
	{
		pcam->off_prev = game_info.last_pos;
	}
#endif //NOSETCAM
#endif //NOT_READY

	return flags;
}
