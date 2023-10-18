/****************************************************************
*		$Archive: /video/Nba/ball.c $
*		$Revision: 206 $
*		$Date: 4/20/99 2:19a $
*
*		Copyright (c) 1998 Midway Games Inc.
*		All Rights Reserved
*
*		This file is confidential and a trade secret of Midway Games Inc.
*		Use, reproduction, adaptation, distribution, performance or
*		display of this computer program or the associated audiovisual work
*		is strictly forbidden unless approved in writing by Midway Games Inc.
*
*		Started 5/20/98 by Shawn Liptak
*
*.Last mod - 3/30/99 23:00
****************************************************************/

#ifdef INCLUDE_SSID
char *ss_ball_c = "$Workfile: ball.c $ $Revision: 206 $";
#endif

#ifndef RELEASE_VER
 #ifndef TEST_VER
  #ifdef DEBUG
//	#define DEBUG_BALLSTUCK		// Show stuck error messages
//	#define DEBUG_JOYBALL		// Move ball with joystick, no gravity
//	#define DEBUG_SHTBALL
//	#define DEBUG_CAMBALL
//	#define DEBUG_SIMINFO
//	#define DEBUG_MVBALL		// Move ball to plyr 4 on gnd hit
//	#define DEBUG_SIMTEST		// Calc sim time
//	#define DEBUG_RAINOFF
//	#define DEBUG_ALWAYS_DOINK	// Always doink dunks
//	#define DEBUG_HOTSPOT
  #endif //DEBUG
 #endif //TEST_VER
#endif //RELEASE_VER


/****************************************************************/
// System includes

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif


/****************************************************************/
// User includes

#include "include/game.h"
#include "include/player.h"
#include "include/plyrseq.h"
#include "include/externs.h"
#include "include/id.h"
#include "include/fontid.h"
#include "include/sndcalls.h"
#include "include/particle.h"
#include "include/nbacmos.h"

/****************************************************************/

//#define	FIRECNT	1

#undef	fsqrt
#define	fsqrt(x) \
({ float	__value, __arg = (x); \
	asm("sqrt.s	%0,%1": "=f" (__value): "f" (__arg)); \
	__value; })

#if	RIM_CZI
#error	!0, so uncomment some RIM_CZ usage that assumes 0
#endif

typedef	void	*PV;
typedef	struct object_node	OBJN;

#define INBOUND_X	OUT_OF_BOUNDS_X
#define INBOUND_Z	OUT_OF_BOUNDS_Z

#define	BKBRD_X		BACKBOARD_X
#define	BKBRD_Y		44.42185f
#define	BKBRD_Z		RIM_Z
#define	BKBRD_W		20.0f
#define	BKBRD_H		14.05881f

#define	_AVLV	(BALL_RADIUS*6.283f)	// Convert angular vel to linear vel units

#define	COLLPERTICK	20

#define	BALLDB_STRID	0x4000

//****************************************************************
// Global data & function prototypes

void show_tmfire_proc(int *args);
void SetUpOOBounds(int,int,int);

int	ptangle (float *p1, float *p2);

extern int plyr_launched_from_hotspot(int);

extern void	shot_made_speech_proc(int *args);
extern void freethrow_made_speech_proc(int *args);
extern void	shot_missed_speech_proc(int *args);
extern void show_3pt_proc(int *args);
extern void flash_basketball_proc(int *args);

extern int	tsec;
extern int	gAssist_time;
extern int ignore_steal;
extern plyr_record_t  * plyr_stat_ptrs[];

extern VIEWCOORD	cambot;

extern int IsShotGood;
extern ball_data	ball_obj;			// ball
extern int pup_snow, pup_rain, pup_blizzard, pup_show_hotspots;

//extern plyr_record_t * plyr_data_ptrs[];
extern plyr_record_t * plyr_stat_ptrs[];

#if defined(VEGAS)
extern Texture_node_t	*ball_decal;
extern Texture_node_t	*ball_shadow_decal;
extern Texture_node_t	*ballshade_decal;
extern Texture_node_t	*ballhigh_decal;
#else
extern GrMipMapId_t	ball_decal;
extern GrMipMapId_t	ball_shadow_decal;
extern GrMipMapId_t	ballshade_decal;
#endif

extern int	ball_ratio, ballshade_ratio, ball_shadow_ratio, ballhigh_ratio;
extern LIMB	limb_ballshadow_obj, limb_ball, limb_ballshade, limb_ballspec;
extern int	what_type_shot, crowd_cheer_time;

extern float	trigs[];

extern BBDATA	bbdata[];

extern int		update_status_box;
extern int		gRim_hits,gBack_board_hits;			// used for speech calls...when miss shot
extern int		reset_shotclock,clock_active;

extern char gameplay_bnk_str[], plyr_bnk_str[],
						crowd_bnk_str[], anc_misc_bnk[],
						anc_take_bnk[], anc_make_bnk[]
						;

extern VERTEX	*nt_swish_verts[],
	*nt_swshb_verts[],
	*nt_hard_verts[],
//	*nt_twist_verts[],
	*nt_dunk_verts[],
	*nt_rim_verts[],
	*nt_air_verts[];

#ifdef JONHEY
extern VERTEX	*nt_swish_bbox[],
	*nt_swshb_bbox[],
	*nt_hard_bbox[],
//	*nt_twist_bbox[],
	*nt_dunk_bbox[],
	*nt_rim_bbox[],
	*nt_air_bbox[];
#endif

extern struct texture_node	*smkpuf_decals[];

extern int	which_court;

//***************************************************************
// Local data & function prototypes

#define	BTST_END	-1
#define	PTL_GRAVITY	-0.05f

typedef	struct	_VEC {
	float		x;
	float		y;
	float		z;
} VEC;

typedef	struct	_BALLSIMST {	// Sim status
	VEC		p;			// Position
	VEC		v;			// Velocity
	int		scored;	// -= can't score, 0= could score, += scored
	int		time;		// Ticks remaining in sim when score or no score achieved
} BALLSIMST;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

void	balltest_proc (int *pa_p);
void	balltestcam_proc (int *pa_p);
void	balldst_draw (obj_3d *o_p);
int	ball_shotsim();
int	ball_move();
int	ball_collide();
void	ball_collrim (VEC *bv_p, float bkposx);
void	ball_collscore (float rcx, float bkposx);
void	ball_collchkmode (float rcx);
void	rimshake_proc (int *pa_p);
void	bbshake_proc (int *pa_p);
void	draw_ball (void *oi);
void	draw_ball_shadow(void *oi);
void	ball_trail();
PTLFRET	ptlf_trail (PTL *pt_p);
void	ball_fireset (int mode, int t, int t2);
void	ball_fire_proc (int *pa_p);
void	ball_firespin_proc (int *pa_p);
PTLFRET	ptlf_spiral (PTL *pt_p);
PTLFRET	ptlf_smk (PTL *pt_p);
void	ball_spark (int mode, int scnt);
PTLFRET	ptlf_spark (PTL *pt_p);
void	net_fire_proc (int *pa_p);
void	hotspot_fire_proc (int *pa_p);
PTLFRET	hotspot_ptlf_ring (PTL *pt_p);
PTLFRET	hotspot_ptlf_ring2 (PTL *pt_p);

PTLFRET	crowd_ptlf_flash (PTL *pt_p);

void	rain_proc (int *pa_p);
PTLFRET	ptlf_rain (PTL *pt_p);

void	mat_genvecrot (VEC *vec_p, int ang, MAT4 *m_p);
int		rndrng0 (int max);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

VEC		balls;			// Shooting location
VEC		balld;			// Destination
MAT4		ballmat;			// Current rot matrix
VEC		ballrotv;		// Ball rotation vel axis and magnitude (rots/tick)
int		bscoremd;		// Score mode
int		ballsim;			// !0=Collision simulator on
int		ballsimsc;		// >0=Sim scored, <0=Sim no score
BALLSIMST ballsimstat;
int		ballfiremode;	// 0=Plyr, 1=Team
int		ballfiretime;	// += Ball fire proc active
int		balldbias;		// Depth bias. !0 when in net
int		btstvmd;

extern VERTEX nt_rim_2_vtx[];
extern VERTEX nt_rim_3_vtx[];
extern VERTEX nt_rim_4_vtx[];
extern VERTEX nt_rim_31_vtx[];
extern VERTEX nt_rim_32_vtx[];
extern VERTEX nt_rim_33_vtx[];
extern VERTEX nt_rim_34_vtx[];
extern VERTEX nt_rim_35_vtx[];

#ifdef JONHEY
extern VERTEX nt_rim_2_bbox[];
extern VERTEX nt_rim_3_bbox[];
extern VERTEX nt_rim_4_bbox[];
extern VERTEX nt_rim_31_bbox[];
extern VERTEX nt_rim_32_bbox[];
extern VERTEX nt_rim_33_bbox[];
extern VERTEX nt_rim_34_bbox[];
extern VERTEX nt_rim_35_bbox[];
#endif

VERTEX *nt_tiny_verts[] =
{
	nt_rim_34_vtx,
	nt_rim_33_vtx,
	nt_rim_34_vtx,
	nt_rim_35_vtx,
	NULL
};

#ifdef JONHEY
VERTEX *nt_tiny_bbox[] =
{
	nt_rim_34_bbox,
	nt_rim_33_bbox,
	nt_rim_34_bbox,
	nt_rim_35_bbox,
	NULL
};
#endif

VERTEX *nt_gentle_verts[] =
{
	nt_rim_2_vtx,
	nt_rim_3_vtx,
	nt_rim_4_vtx,
	nt_rim_3_vtx,
	nt_rim_2_vtx,
	nt_rim_35_vtx,
	nt_rim_34_vtx,
	nt_rim_33_vtx,
	nt_rim_32_vtx,
	nt_rim_31_vtx,
	nt_rim_32_vtx,
	nt_rim_33_vtx,
	nt_rim_34_vtx,
	nt_rim_35_vtx,
	NULL
};

#ifdef JONHEY
VERTEX *nt_gentle_bbox[] =
{
	nt_rim_2_bbox,
	nt_rim_3_bbox,
	nt_rim_4_bbox,
	nt_rim_3_bbox,
	nt_rim_2_bbox,
	nt_rim_35_bbox,
	nt_rim_34_bbox,
	nt_rim_33_bbox,
	nt_rim_32_bbox,
	nt_rim_31_bbox,
	nt_rim_32_bbox,
	nt_rim_33_bbox,
	nt_rim_34_bbox,
	nt_rim_35_bbox,
	NULL
};
#endif


float  hotspot_xzs[NUM_HOTSPOTS+4][2] = {
					{-134.0f, -73.0f},
					{-127.0f, -73.0f},
					{-113.0f, -73.0f},
					{ -97.0f, -73.0f},
					{ -92.0f, -70.0f},
					{ -77.0f, -63.0f},
					{ -66.0f, -54.0f},
					{ -57.0f, -56.0f},
					{ -50.0f, -39.0f},
					{ -39.0f, -17.0f},
					{ -37.0f,  -2.0f},
					{ -43.0f,  24.0f},
					{ -44.0f,  44.0f},
					{ -64.0f,  55.0f},
					{ -77.0f,  65.0f},
					{ -96.0f,  75.0f},
					{-121.0f,  72.0f},
					{-130.0f,  72.0f},
					// these next four are four 'created players'
					{0.0f,  0.0f},
					{0.0f,  0.0f},
					{0.0f,  0.0f},
					{0.0f,  0.0f} };

/****************************************************************/

/********************************
 Init ball data
********************************/

void	ball_init()
{
	BBDATA	*bbd_p;

	void		*v_p;
#ifdef JONHEY
	void		*v_bbox;
#endif

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	ballmat.xx = 1;
	ballmat.xy = 0;
	ballmat.xz = 0;
	ballmat.xw = 0;
	ballmat.yx = 0;
	ballmat.yy = 1;
	ballmat.yz = 0;
	ballmat.yw = 0;
	ballmat.zx = 0;
	ballmat.zy = 0;
	ballmat.zz = 1;
	ballmat.zw = 0;

	ballfiretime = 0;

	ball_obj.flags &= ~BF_WHITE;
	ball_obj.color = 0xff000000;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	v_p = nt_air_verts;
#ifdef JONHEY
	v_bbox = nt_air_bbox;
#endif

	bbd_p = &bbdata[0];
	bbd_p->rimcolor = 0xff000000;
	bbd_p->bendcnt = 0;
	bbd_p->rimbendcnt = 0;
	bbd_p->netcolor = 0xff000000;
	bbd_p->netv_p = v_p;
#ifdef JONHEY
	bbd_p->netv_bbox = v_bbox;
#endif
	bbd_p->netvposf = 0;
	bbd_p->netvstep = 1;
	bbd_p->netya = 0;

	bbd_p = &bbdata[1];
	bbd_p->rimcolor = 0xff000000;
	bbd_p->bendcnt = 0;
	bbd_p->rimbendcnt = 0;
	bbd_p->netcolor = 0xff000000;
	bbd_p->netv_p = v_p;
#ifdef JONHEY
	bbd_p->netv_bbox = v_bbox;
#endif
	bbd_p->netvposf = 0;
	bbd_p->netvstep = 1;
	bbd_p->netya = 0;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#ifndef	DEBUG_RAINOFF
//	if ( COURT_IS_OUTDOORS && rndrng0 (2))
	if ((COURT_IS_OUTDOORS) && ((pup_rain) || (pup_snow) || (pup_blizzard)))
	// No random weather
//	if ((pup_rain) || (pup_snow) || (pup_blizzard))
		set_process_run_level (qcreate ("rain", PLAYER_PID, rain_proc, 0, 0, 0, 0), RL_BALL+20);
#endif

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#ifdef	DEBUG
	balls.x = (RIM_CX - 40) * BTST_END;
//	balls.x = RIM_X - 40;
	balls.y = 24;
	balls.z = 0;

	balld.x = RIM_CX * BTST_END;
	balld.y = RIM_CY;
	balld.z = RIM_CZ;
#endif

#ifdef	DEBUG_SHTBALL
#ifndef	DEBUG_MVBALL
	set_process_run_level (qcreate ("balltest", PLAYER_PID, balltest_proc, 0, 0, 0, 0), RL_BALL+10);
#endif
#endif
#ifdef	DEBUG_CAMBALL
	set_process_run_level (qcreate ("balltestcam", BALL_PID, balltestcam_proc, 0, 0, 0, 0), RL_CAMBOT+1);
#endif

}



/********************************
 Clear ball rotation
********************************/

void	ball_rotclr()
{

	ballrotv.x = 0;
	ballrotv.y = 0;
	ballrotv.z = 0;

//	ballmat.xx = 1;
//	ballmat.xy = 0;
//	ballmat.xz = 0;
//	ballmat.xw = 0;
//	ballmat.yx = 0;
//	ballmat.yy = 1;
//	ballmat.yz = 0;
//	ballmat.yw = 0;
//	ballmat.zx = 0;
//	ballmat.zy = 0;
//	ballmat.zz = 1;
//	ballmat.zw = 0;

}

/****************************************************************/
// Ball debug stuff

#ifdef	DEBUG

/********************************
 Test ball shooting
********************************/

void	balltest_proc (int *pa_p)
{
	ball_data	*bd_p;
	int		time;

//	float		bvy;

	int		i;
	float		fx, fy, fz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	sleep (20);

	bd_p = &ball_obj;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

//	while (1) {
//		int	i, i2, i3, i4;
//
//		bbplyr_data	*ppdata;
//
//		i = get_player_49way_current();
//		i2 = get_player_sw_current();
//		i3 = read_player12_direct();
//		i4 = read_player34_direct();
//		printf ("sw = %8x %8x %8x %8x\n", i, i2, i3, i4);
//
//		ppdata = &player_blocks[3];
//		printf ("AC %d\n", ppdata->auto_control);
//
//		sleep (1);
//	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	while (1) {

		time = 50;
		fx = balld.x;
		fy = balld.y;
		fz = balld.z;

		bd_p->flight_time = 0;
		bd_p->total_time = time;
		game_info.ball_mode = BM_SHOOTING;
		game_info.ball_handler = NO_BALLHANDLER;

		bd_p->odata.x = balls.x;
		bd_p->odata.y = balls.y;
		bd_p->odata.z = balls.z;

		bd_p->odata.vx = (fx - bd_p->odata.x) / (float) time;
		bd_p->odata.vy = ((fy - bd_p->odata.y) / (float) time) -
							 (.5f * BALL_GRAVITY * (float) time) - BALL_GRAVITY * .5f;
		bd_p->odata.vz = (fz - bd_p->odata.z) / (float) time;

		bd_p->odata.x -= bd_p->odata.vx;	// Ball proc will move before display
		bd_p->odata.y -= bd_p->odata.vy;
		bd_p->odata.vy -= BALL_GRAVITY;
		bd_p->odata.z -= bd_p->odata.vz;

		ballrotv.x = 0;
		ballrotv.y = 0;
		ballrotv.z = 0;

//		printf("Ball test\n");
//		printf("Cam XYZ %f %f %f\n", cambot.x, cambot.y, cambot.z);
//		printf("Cam ABC %f %f %f\n", cambot.a, cambot.b, cambot.c);

//		bvy = bd_p->odata.vy;

		i = time + 200;
		while (--i >= 0) {

//			printf("Time %d %d\n", bd_p->flight_time, bd_p->total_time);

//			bd_p->odata.x = RIM_CX;
//			bd_p->odata.y = RIM_CY;
//			bd_p->odata.z = RIM_CZ;

//			bvy += BALL_GRAVITY;
//			bd_p->odata.vy = bvy;

			if (bd_p->odata.vy < 0)
				if (bd_p->odata.y < RIM_CY - BALL_RADIUS * 4) i = 0;
			sleep (1);

//			{
//				int	i, i2, i3;
//				i = get_player_49way_current();
//				i2 = read_player34_direct();
//				i3 = ~get_dip_coin_current();
//				printf("Switch %x %x DIP %0x\n", i, i2, i3);
//			}
		}
	}

}

#ifdef	DEBUG_CAMBALL
/********************************
 Override cambot, show shot
********************************/
void	balltestcam_proc (int *pa_p)
{
	ball_data	*bd_p;
	float		rcx;
	float		end;						// End of court -1 or 1
	int		vmode;					// View mode
	VEC		vpos;						// View position
	obj_3d	dsto;
	OBJN		*dston_p;

	VEC		*vc_p;

	int		i;
	float		f;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	killall (REPLAY_PID, -1);	// Uses our buttons

	dston_p = create_object ("balldst", OID_BALL, OF_NONE, DRAWORDER_BALL+1, (PV) &dsto, (PV) balldst_draw);

	bd_p = &ball_obj;

	end = BTST_END;
	rcx = RIM_CX * end;

	vmode = 1;
	vpos.x = rcx - 61 * end;
	vpos.y = RIM_CY;
	vpos.z = RIM_CZ;

	while (1) {

		cambot.a = rcx;			//bd_p->odata.x;
		cambot.b = RIM_CY;
		cambot.c = RIM_CZ;		//bd_p->odata.z;

		if (get_p5_sw_close() & P_B) {
			vmode++;
			if (vmode > 2) vmode = 0;
		}
		btstvmd = vmode;

		i = get_p5_sw_current();
#ifdef	SHAWN
//		if (!halt) i = 0;
#endif

		vc_p = &balls;
		f = 2;
//		dston_p->object_flags |= OF_HIDDEN;

		if (vmode == 1) {
			vc_p = &vpos;
			f = 1;
		}
		if (vmode == 2) {
			vc_p = &balld;
			f = .005f;
			dston_p->object_flags &= ~OF_HIDDEN;
//			printf("Dest %f %f %f\n", vc_p->x, vc_p->y, vc_p->z);
		}

		f *= end;

		if (i & P_C) f *= 8;		// Much faster when holding mode change button

		if ((i & P_A) == 0) {
			if (i & P_UP)
				vc_p->x += f;
			if (i & P_DOWN)
				vc_p->x -= f;
		}
		else {
			if (i & P_UP)
				vc_p->y += fabs (f);
			if (i & P_DOWN)
				vc_p->y -= fabs (f);
		}
		if (i & P_LEFT)
			vc_p->z -= f;
		if (i & P_RIGHT)
			vc_p->z += f;

		cambot.x = balls.x - 30 * end;
		cambot.y = balls.y;
		cambot.z = balls.z;
		if (vmode > 0) {
			cambot.x = vpos.x;
			cambot.y = vpos.y;
			cambot.z = vpos.z;
		}
		cambot.e = cambot.a;
		cambot.f = cambot.b + 10;
		cambot.g = cambot.c;

		f = (ptangle (&cambot.x, &cambot.a) + 512) & 0x3ff;
		set_cambot_theta (DG2RD(f));

		update_cambot_xform();

		dsto.x = balld.x;
		dsto.y = balld.y;
		dsto.z = balld.z;

		set_text_font (FONTID_BAST7T);
		delete_string_id (BALLDB_STRID);
		set_string_id (BALLDB_STRID);
		set_text_position (20, VRESf-10, 1);
#ifdef	DEBUG
 		oprintf ("%dj%f %f %f", HJUST_LEFT, balld.x - rcx, balld.y - RIM_CY, balld.z - RIM_CZ);
#endif

		sleep (1);
	}

}
#endif //DEBUG_CAMBALL

/********************************
 Draw ball test
********************************/

void	balldst_draw (obj_3d *o_p)
{
	float		mat1[12];
	float		mat2[12];
	float		mat3[12];
	static	int glow;
	int		i;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

//	printf ("Ball dst:\n");
//	printf ("%f %f %f %f\n", cambot.xform[0], cambot.xform[1], cambot.xform[2], cambot.xform[3]);
//	printf ("%f %f %f %f\n", cambot.xform[4], cambot.xform[5], cambot.xform[6], cambot.xform[7]);
//	printf ("%f %f %f %f\n", cambot.xform[8], cambot.xform[9], cambot.xform[10], cambot.xform[11]);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	i = 0x40ffffff;
	if (btstvmd == 2) {
		glow = (glow + 2) & 0x1ff;
		i = glow;
		if (i > 0xff) i = 0x1ff - i;
		i = 0x80000000 | i << 16 | i << 8 | i;
//		i = 0x80ffff80;
	}

	grConstantColorValue (i);

//	grColorCombine (GR_COMBINE_FUNCTION_SCALE_OTHER, GR_COMBINE_FACTOR_ONE,
//			GR_COMBINE_LOCAL_ITERATED, GR_COMBINE_OTHER_CONSTANT, 0);
	grColorCombine (GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL, GR_COMBINE_FACTOR_ONE,
			GR_COMBINE_LOCAL_CONSTANT, GR_COMBINE_OTHER_TEXTURE, 0);

	grChromakeyMode (GR_CHROMAKEY_DISABLE);

	guAlphaSource (GR_ALPHASOURCE_CC_ALPHA);
	grAlphaBlendFunction (GR_BLEND_SRC_ALPHA, GR_BLEND_ONE,
//			GR_BLEND_ONE_MINUS_SRC_ALPHA,
			GR_BLEND_ONE, GR_BLEND_ZERO);

	grTexCombineFunction (0, GR_TEXTURECOMBINE_DECAL);
	grTexClampMode (0, GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP);

	grDepthBufferMode (GR_DEPTHBUFFER_WBUFFER);
	grDepthBufferFunction (GR_CMP_LESS);
	grDepthMask (FXTRUE);

	grCullMode (GR_CULL_NEGATIVE);

	mat1[3] = 0.0f;	mat1[7] = 0.0f;	mat1[11] = 0.0f;
	mat2[3] = 0.0f;	mat2[7] = 0.0f;	mat2[11] = 0.0f;
	mat3[3] = 0.0f;	mat3[7] = 0.0f;	mat3[11] = 0.0f;

	rotx ( 0, mat3 );
	roty ( 0, mat1 );
	mxm ( mat3, mat1, mat2 );
 	rotz ( 0, mat1 );
	mxm ( mat2, mat1, mat3 );

	mat3[3] = o_p->x - cambot.x;
	mat3[7] = o_p->y - cambot.y;
	mat3[11] = o_p->z - cambot.z;

	mxm (cambot.xform, mat3, mat1);

	render_limb (&limb_ball, mat1, &ball_decal, &ball_ratio);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Reset mode

	grColorCombine (GR_COMBINE_FUNCTION_SCALE_OTHER, GR_COMBINE_FACTOR_ONE,
			GR_COMBINE_LOCAL_ITERATED, GR_COMBINE_OTHER_TEXTURE, 0);

//	guColorCombineFunction (GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
//	guAlphaSource (GR_ALPHASOURCE_TEXTURE_ALPHA);
//	grAlphaBlendFunction (GR_BLEND_SRC_ALPHA,
//			GR_BLEND_ONE_MINUS_SRC_ALPHA,
//			GR_BLEND_ONE,
//			GR_BLEND_ZERO);
}

#endif	// DEBUG

/****************************************************************/

/********************************
 Shoot the ball
 (* plyr obj, target (0-?) B8=Shot good)
********************************/

#define SHOTTIME_HALFCRT		100
#define SHOTTIME_MIN			40
#define HALFDIST_MAX			150.0f
#define SHOTSIMS				12
#define BALLINRIMRAD			(RIM_RAD - RIM_EDGERAD - BALL_RADIUS)
#define BALLOUTRIMRAD		(RIM_RAD + RIM_EDGERAD + BALL_RADIUS)

void	ball_shoot (obj_3d *o3_p, int mode)
{
	ball_data	*bd_p;

	int		mode2;			// 0=max hits, !0=min hits

	float	rcx;
	float	gsign;			// Goal sign -1,1
	float	rdsth;			// Rim distance horizontally
	VEC		tv;				// Target pos
	int		time;			// Time to target or 0
	float	arc; 			// Arc value multiplied with time
	int		bang;
	int		good;
	int		simcnt;
	int		simgoodcnt;
	VEC		lstv;
	VEC		lstrv;
	int		besthits;
	int		besttm;
	VEC		bestv;
	VEC		bestrv;

	int		i, i2, i3;
	char	*c_p, *c2_p;
	float	f, f2, f3, fx, fy, fz;
	bbplyr_data *ppdata = (bbplyr_data *)o3_p;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Test

//	mode = rndrng0 (SHTMD_MAX - 1);
//	mode = SHTMD_RIMLONG;
//	mode = SHTMD_BOARD;

//	if (rand() & 1) mode |= M_SHTMD_GOOD;
//	mode |= M_SHTMD_GOOD;

#ifdef	DEBUG_SIMINFO
	delete_string_id (BALLDB_STRID+1);
#endif

	besttm = 0;		// Not necessary

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	bd_p = &ball_obj;

	good = mode & M_SHTMD_GOOD;
	mode &= 0xff;
	mode2 = 0;
	if (rndrng0 (3) == 0) mode2 = 1;		// Min hits


	i = game_info.game_time;	// 0xMMSSTT
	if (i < 0x300) {				// Less than 3 sec?
		mode2 = 0;					// Make more dramatic
		if (mode == SHTMD_NET) {
			mode = SHTMD_RIMLONG;
		}
	}

	// after first shot by player, turn off hotspot!
	if (pup_show_hotspots & (1<<ppdata->plyrnum))
	{
		delete_object_id(OID_HOTSPOT_CURSOR+ppdata->plyrnum);
		pup_show_hotspots &= ~(1<<ppdata->plyrnum);
	}

	game_info.ball_handler = NO_BALLHANDLER;

	if ((!(o3_p->flags & PF_DUNKER)) && (!(o3_p->flags & PF_ALLYDUNKER))) {

//		printf("*Shot mode=%d\n", mode);

		rcx = RIM_CX;
		gsign = 1;
		if (game_info.active_goal == 0) {
			rcx = -RIM_CX;
			gsign = -1;
		}

		fx = rcx - bd_p->odata.x;
		fz = RIM_CZ - bd_p->odata.z;
		rdsth = fsqrt (fx * fx + fz * fz);

#ifdef	DEBUG_CAMBALL
		printf("-Sim-\n");
#endif

		simcnt = SHOTSIMS;
		simgoodcnt = 0;

//		if (!good) {
//			if (rndrng0 (2) < 1) simcnt = 1;
//		}

		besthits = -1;
		if (mode2) besthits = 9999;

		arc = 1;
		if (mode != SHTMD_BOARD && rdsth > RIM_RAD * 6) {
			i = rndrng0 (19);
		// Debug shot types
//			i = 2;
			if (i < 2) arc = (115 + rndrng0 (15)) * .01f;	// Rainbow

// Maybe do more flat arcs for layups...

			if (i == 2) arc = (90 + rndrng0 (10)) * .01f;	// Flat
		}
//		arc = 1;

	//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Sim loop
sim:
		tv.x = rcx;
		tv.y = RIM_CY;
		tv.z = RIM_CZ;

		fx = tv.z - bd_p->odata.z;	// Rim vec rotated 90' clockwise
		fy = 0;
		fz = -(tv.x - bd_p->odata.x);
		f2 = rdsth;
		if (f2 < 50) f2 = 50;
		if (f2 > 120) f2 = 120;
		f2 = f2 * .0007f / rdsth;

		if (o3_p->flags & PF_LAYUP) f2 = -f2;	// Neg spin

		ballrotv.x = fx * f2;
		ballrotv.y = fy;
		ballrotv.z = fz * f2;

//		ballrotv.x = 0;	// DEBUG
//		ballrotv.y = 0;
//		ballrotv.z = 0;

		if (rdsth <= HALFDIST_MAX)
			time = (SHOTTIME_MIN + (int)(rdsth * ((SHOTTIME_HALFCRT - SHOTTIME_MIN) / HALFDIST_MAX)));
		else
			time = SHOTTIME_HALFCRT + (10 + rndrng0 (19));

		time -= (int)(o3_p->y * 0.5f);
		time *= arc;
		if (time < 2) time = 2;


		switch (mode) {

			case SHTMD_NET:

				tv.y = RIM_CY + BALL_RADIUS * .5f;
				simcnt = 0;
				break;

			case SHTMD_RIMLONG:

				i = 0x300;
				i2 = 90;
				i3 = 110;
				goto rim;

			case SHTMD_RIMSHORT:

				i = 0x100;
				i2 = -30;
				i3 = 100;
rim:
				bang = ptangle ((PV) &bd_p->odata.x, (PV) &tv) + i;
				bang += rndrng0 (0xff) - 0x80;
				if (good) {
					f = (i2 + rndrng0 (i3)) * .001f;
				}
				else {
					f = (150 + rndrng0 (400)) * .001f;
				}
				tv.x += (BALLINRIMRAD + (BALLOUTRIMRAD - BALLINRIMRAD) * f) * icos (bang);
				tv.y += BALL_RADIUS * .05f;
				tv.z -= (BALLINRIMRAD + (BALLOUTRIMRAD - BALLINRIMRAD) * f) * isin (bang);
				break;

			case SHTMD_RIML:

				i = 0;
				i2 = 20;
				i3 = 150;
				goto rim;

			case SHTMD_RIMR:

				i = 0x200;
				i2 = 20;
				i3 = 150;
				goto rim;


			case SHTMD_BOARD:

//				simcnt = 0;
				time *= .9f;	//.7f;
				tv.x += (BACKBOARD_X - RIM_CX - BALL_RADIUS) * gsign;
				if (good) {
					fx = fabs (bd_p->odata.x - tv.x);
					fz = bd_p->odata.z - RIM_CZ;

					f = fabs (fz / (fx + .00001f));
					if (f > 1.4f) time *= .8f;		// From side so flatten shot
//					if (f > 2.1f) time *= .9f;		// Flatten more
//					if (f > 3.0f) time *= .8f;		// Flatten more

//					if (simcnt == SHOTSIMS) printf ("Z/X %f T %d\n", f, time);

					tv.y += BALL_RADIUS * 4 * ((70 + rndrng0 (60)) * .01f);
					if (f > 2.1f) tv.y += BALL_RADIUS;
					if (f > 3.0f) tv.y += BALL_RADIUS;

					if (fx > RIM_CX / 3 && f < .7f) {
						tv.y -= BALL_RADIUS;		// Lower long front shot
//						if (simcnt == SHOTSIMS) printf ("Lower\n");
					}
					f2 = fx / (BACKBOARD_X - RIM_CX - BALL_RADIUS);
					f = 1 / (f2 + 1);	// Deflection percent of Z delta
					tv.z = RIM_CZ + fz * f;
				}
				else {
					i = rand();
					tv.y += BALL_RADIUS * 2.0f;
					if (i & 1) tv.y += BALL_RADIUS * 2.5f;
					f = (100 + rndrng0 (99)) * .01f * BALL_RADIUS;
					if (i & 2) f = -f;
					tv.z += f;
				}
				break;


			case SHTMD_AIR:	// Short

				fx = bd_p->odata.x - tv.x;
				fz = bd_p->odata.z - tv.z;
				f = (120 + rndrng0 (60)) * .01f;
				f = 1 / sqrt (fx * fx + fz * fz) * (RIM_RADOUT + BALL_RADIUS * f);
				fx *= f;
				fz *= f;
				tv.x += fx;
				tv.z += fz;
				simcnt = 0;
				break;


			default:

#ifdef	DEBUG
				printf("ERROR: shoot\n");
#endif
		}

//		tv.x *= .8f;	//DEBUG

		//if(o3_p->flags & PF_LAYUP)
		//	time /= 2;

		bd_p->flight_time = 0;
		bd_p->total_time = time;
		game_info.ball_mode = BM_SHOOTING; // shooting

		f = 1.0f / time;
		bd_p->odata.vx = (tv.x - bd_p->odata.x) * f;
		bd_p->odata.vy = (tv.y - bd_p->odata.y) * f - .5f * BALL_GRAVITY * (float)time - BALL_GRAVITY * .5f;
		bd_p->odata.vz = (tv.z - bd_p->odata.z) * f;

		if (simcnt-- > 0) {

			lstv.x = bd_p->odata.vx;
			lstv.y = bd_p->odata.vy;
			lstv.z = bd_p->odata.vz;
			lstrv.x = ballrotv.x;
			lstrv.y = ballrotv.y;
			lstrv.z = ballrotv.z;

			i = ball_shotsim();
#if	0
			printf ("Sim %2d %2d T%d %f %f %f", i, ballsimsc, time, lstv.x, lstv.y, lstv.z);
//			printf (", %f %f %f", ballrotv.x, ballrotv.y, ballrotv.z);
			printf ("\n");
#endif
			if (ballsimsc > 0) {		// Scored?
				if (!good) i = -1;	// Worst case
			}
			else {
				if (good) i = -1;		// Worst case
			}
			if (i > 0) {

				simgoodcnt++;

				if ((mode2 == 0 && i > besthits) ||
					 (mode2 != 0 && i < besthits)) {	// Best rim action?
#ifdef	DEBUG_CAMBALL
					printf("Sim %2d %d T%d %f %f %f", i, ballsimsc, time, lstv.x, lstv.y, lstv.z);
					printf(", %f %f %f\n", ballrotv.x, ballrotv.y, ballrotv.z);
#endif
					besthits = i;
					besttm = time;
					bestv.x = lstv.x;
					bestv.y = lstv.y;
					bestv.z = lstv.z;
					bestrv.x = lstrv.x;
					bestrv.y = lstrv.y;
					bestrv.z = lstrv.z;
				}
			}
			if (simcnt > 0) goto sim;

			if (simgoodcnt < 1) {	// All sims bad?
#ifdef	DEBUG
				printf ("Shoot: all sims bad!\n");
				simgoodcnt = -1;
#endif
				if (good) {
					mode = SHTMD_NET;
				}
				else {
					mode = SHTMD_AIR;
				}
				goto sim;
			}

			bd_p->flight_time = 0;
			bd_p->total_time = besttm;
			bd_p->odata.vx = bestv.x;
			bd_p->odata.vy = bestv.y;
			bd_p->odata.vz = bestv.z;
			ballrotv.x = bestrv.x;
			ballrotv.y = bestrv.y;
			ballrotv.z = bestrv.z;
		}

#ifdef	DEBUG_SIMINFO
		set_string_id (BALLDB_STRID+1);
		set_text_font (FONTID_BAST7T);
		set_text_position (50, VRESf-18, 1);
		i = simgoodcnt * 100 / SHOTSIMS;
		c_p = "?";
		if (mode == SHTMD_NET) c_p = "Net";
		if (mode == SHTMD_RIMLONG) c_p = "Rim long";
		if (mode == SHTMD_RIMSHORT) c_p = "Rim short";
		if (mode == SHTMD_RIML) c_p = "Rim left";
		if (mode == SHTMD_RIMR) c_p = "Rim right";
		if (mode == SHTMD_BOARD) c_p = "Board";
		if (mode == SHTMD_AIR) c_p = "Air";
		c2_p = "Miss";
		if (good) c2_p = "Good";

		i2 = LT_GREEN;
		if (simgoodcnt < 1) {
			besthits = 0;
			besttm = 0;
			if (simgoodcnt < 0) {	// Really bad?
				i = 0;
				i2 = LT_RED;
			}
		}
 		oprintf ("%dj%dc%s: %s %dcSim %d%% H%d T%d, Md%d", HJUST_LEFT, LT_YELLOW, c_p, c2_p, i2, i, besthits, besttm, mode2);
#endif

	}


//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	else {	// the ball is being dunked

		tv.x = RIM_CX;
		if (game_info.active_goal == 0)
			tv.x = -RIM_CX;

		tv.y = RIM_CY + BALL_RADIUS * .5f;

#ifndef DEBUG_ALWAYS_DOINK
		if (rndrng0 (99) < 2 && !(game_info.fire_player == ball_obj.who_shot && game_info.fire_cnt >= FIRECNT) &&
					!(game_info.team_fire == player_blocks[ball_obj.who_shot].team))
		{	// % for a bounce out or roll
			if (rand() & 1) {		// Bounce up
#endif
				f = .9f;
				fx = BALL_RADIUS * (rndrng0 (40) - 20) * .01f;
				fy = BALL_RADIUS * (rndrng0 (40) + 20) * .01f;
				fz = BALL_RADIUS * (rndrng0 (40) - 20) * .01f;
#ifdef DEBUG
				printf ("Dunk: out %f %f %f\n", fx, fy, fz);
#endif
				if ((rand() & 3) == 0) {	// Bounce way up
					fy += BALL_RADIUS * .9f;
				}
#ifndef DEBUG_ALWAYS_DOINK
			}
			else {		// Roll around
				f = (105 + rndrng0 (70)) * .01f;
				tv.y = RIM_CY + BALL_RADIUS * .95f;
				fx = BALL_RADIUS * (rndrng0 (60) - 30) * .01f;
				fy = BALL_RADIUS * (rndrng0 (15) -  0) * .01f;
				fz = BALL_RADIUS * (rndrng0 (60) - 30) * .01f;
#ifdef DEBUG
				printf ("Dunk: roll %f %f %f\n", fx, fy, fz);
#endif
			}
		}
		else {		// Slam it
			f = .9f;
			fx = 0.0f;
			fy = -.5f;
			fz = 0.0f;
		}
#endif
		i = o3_p->fwd + 512;
		tv.x += BALLINRIMRAD * f * isin (i);
		tv.z = RIM_CZ + BALLINRIMRAD * f * icos (i);

		bd_p->odata.x = tv.x;
		bd_p->odata.y = tv.y;
		bd_p->odata.z = tv.z;

		bd_p->odata.vx = fx;
		bd_p->odata.vy = fy;
		bd_p->odata.vz = fz;

		time = 0;
		bd_p->flight_time = -1;
		bd_p->total_time = 0;
		game_info.ball_mode = BM_SHOOTING;

		i = rndrng0 (99) < 50 ? DUNK1_SND : DUNK2_SND;
		make_dunk_snd(i);
		make_dunk_snd(i);
		snd_scall_bank (crowd_bnk_str, CROWD2_SWELL1_SND, VOLUME6, 127, PRIORITY_LVL3);

		reset_shotclock = 1;	// Reset shotclock
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

// DEBUG
//	game_info.team_fire = player_blocks[bd_p->who_shot].team;


//	if (1) {
	if ((plyr_launched_from_hotspot(ppdata->plyrnum) == YES) &&	(game_info.hotspot_made_cnt[ppdata->plyrnum] >= HOTSPOT_ACTIVE_CNT))
//			((game_info.hotspot_made_cnt[ppdata->plyrnum] == 0) || (game_info.hotspot_made_cnt[ppdata->plyrnum] >= HOTSPOT_ACTIVE_CNT)))
	{
		i = bd_p->total_time;
		ball_fireset (0x10 + ppdata->plyrnum, i + (IsShotGood ? 30 : 0), 0);
	}
	else if (game_info.team_fire == player_blocks[bd_p->who_shot].team)
	{
		i = bd_p->total_time;
		ball_fireset (1, i + (IsShotGood ? 30 : 0), i + 5);
	}
	else if (game_info.fire_cnt >= FIRECNT)
	{
		if (bd_p->who_shot == game_info.fire_player)
		{

			i = bd_p->total_time;
			ball_fireset (0, i + (IsShotGood ? 30 : 0), i + 5);
		}
	}

#ifdef	DEBUG_HOTSPOT
	i = bd_p->total_time;
	ball_fireset (0x10 + ppdata->plyrnum, i + (IsShotGood ? 30 : 0), 0);
#endif
}


/********************************
 Show time of a specific sim run
********************************/

#ifdef	DEBUG_SIMTEST

#ifdef	DEBUG
int	___get_count();
#define	GET_TIME(A)	(A) = (___get_count() * 13)
#endif

void	ball_shotsimtest()
{
	ball_data	*bd_p;

	static	float	dst = 1.032f;
	VEC		tv;				// Target pos
	int		tm;
	int		time;
	int		hits;

	int		n, i;
	float		f;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	bd_p = &ball_obj;


	i = get_p5_sw_current();
	if (!(i & P_C)) return;

	if (i & P_UP) dst += .00001f;
	if (i & P_DOWN) dst -= .00001f;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	bd_p->odata.x = RIM_CX * .5f;
	bd_p->odata.y = RIM_CY * .9f;
	bd_p->odata.z = RIM_CZ;

	tv.x = RIM_CX - dst;
	tv.y = RIM_CY;
	tv.z = RIM_CZ;

	time = 60;

	GET_TIME(tm);

	n = 100;
	while (--n > 0) {

		ballrotv.x = 0;
		ballrotv.y = 0;
		ballrotv.z = 0;

		bd_p->flight_time = 0;
		bd_p->total_time = time;
		game_info.ball_mode = BM_SHOOTING; // shooting

		f = 1.0f / time;
		bd_p->odata.vx = (tv.x - bd_p->odata.x) * f;
		bd_p->odata.vy = (tv.y - bd_p->odata.y) * f - .5f * BALL_GRAVITY * (float)time - BALL_GRAVITY * .5f;
		bd_p->odata.vz = (tv.z - bd_p->odata.z) * f;

		hits = ball_shotsim();
	}

	GET_TIME(i);
	tm = (i - tm) / 100000;
	printf ("Sim: dst %f time %d hits %d score %d\n", dst, tm, hits, ballsimsc);
}
#endif


/********************************
 Run shot simulation using current ball variables
> # hits detected
********************************/

int	ball_shotsim()
{
	ball_data	*bd_p;
	VEC		bpos;				// Ball position
	VEC		brotv;			// Rotation
	int		hits;

	int		n, i, i2;
	float		f, f2, fx, fy, fz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	bd_p = &ball_obj;

	ballsim = 1;
	ballsimsc = 0;

	bpos.x = bd_p->odata.x;		// Save
	bpos.y = bd_p->odata.y;
	bpos.z = bd_p->odata.z;
	brotv.x = ballrotv.x;
	brotv.y = ballrotv.y;
	brotv.z = ballrotv.z;

	hits = 0;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Move close to target

	n = bd_p->total_time - 10;
	bd_p->flight_time = n;

	do {
		bd_p->odata.vy += BALL_GRAVITY;

		i = COLLPERTICK;		// Must loop to get exact float errors as collide
		f = 1.0f / COLLPERTICK;
		fx = bd_p->odata.vx * f;
		fy = bd_p->odata.vy * f;
		fz = bd_p->odata.vz * f;
		do {
			bd_p->odata.x += fx;
			bd_p->odata.y += fy;
			bd_p->odata.z += fz;
		} while (--i > 0);

//		i = ball_collide();
//		if (i) printf ("Sim ERROR!\n");
	} while (--n > 0);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Collide

	n = 150;
	do {
		bd_p->odata.vy += BALL_GRAVITY;

		i = ball_collide();

		hits += i;
		if (ballsimsc) {
			break;
		}

	} while (--n > 0);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	bd_p->odata.x = bpos.x;		// Restore
	bd_p->odata.y = bpos.y;
	bd_p->odata.z = bpos.z;
	ballrotv.x = brotv.x;
	ballrotv.y = brotv.y;
	ballrotv.z = brotv.z;

	ballsim = 0;

	return (hits);
}



/********************************
 Run ball sim using current ball variables for x ticks. Report status
 (ticks to check ahead)
> # hits detected or - if ball in posession
********************************/

int	ball_calcstatus (int ticks)
{
	ball_data	*bd_p;
	VEC		bpos;				// Ball position
	VEC		bvel;
	VEC		brotv;			// Rotation
	int		ftime;
	int		bsmd;
	int		hits;
	BALLSIMST *bss_p;

	int		n, i, i2;
	float		f, f2, fx, fy, fz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	if (game_info.ball_mode != BM_SHOOTING &&
			game_info.ball_mode != BM_BOUNCE_ON_RIM) {

		return (-1);
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	bd_p = &ball_obj;

	ballsim = 1;
	ballsimsc = 0;

	bpos.x = bd_p->odata.x;		// Save
	bpos.y = bd_p->odata.y;
	bpos.z = bd_p->odata.z;
	bvel.x = bd_p->odata.vx;
	bvel.y = bd_p->odata.vy;
	bvel.z = bd_p->odata.vz;
	brotv.x = ballrotv.x;
	brotv.y = ballrotv.y;
	brotv.z = ballrotv.z;

	ftime = bd_p->flight_time;
	bsmd = bscoremd;

	hits = 0;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Collide

	n = ticks;
	do {
		bd_p->odata.vy += BALL_GRAVITY;

		i = ball_collide();

		hits += i;
		if (ballsimsc) {
			break;
		}

	} while (--n > 0);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Save status

	bss_p = &ballsimstat;
	bss_p->p.x = bd_p->odata.x;
	bss_p->p.y = bd_p->odata.y;
	bss_p->p.z = bd_p->odata.z;
	bss_p->v.x = bd_p->odata.vx;
	bss_p->v.y = bd_p->odata.vy;
	bss_p->v.z = bd_p->odata.vz;
	bss_p->scored = ballsimsc;
	bss_p->time = n;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	bd_p->odata.x = bpos.x;		// Restore
	bd_p->odata.y = bpos.y;
	bd_p->odata.z = bpos.z;
	bd_p->odata.vx = bvel.x;
	bd_p->odata.vy = bvel.y;
	bd_p->odata.vz = bvel.z;
	ballrotv.x = brotv.x;
	ballrotv.y = brotv.y;
	ballrotv.z = brotv.z;

	bd_p->flight_time = ftime;
	bscoremd = bsmd;

	ballsim = 0;

	return (hits);
}


/********************************
 Sim step testing
********************************/

#ifdef	DEBUG

void	ball_simsteptest()
{
	ball_data	*bd_p;
	VEC		bpos;				// Ball position
	VEC		brotv;			// Rotation

	register	VEC		t;
	volatile	VEC	vvc;
	volatile VEC	*vc_p;
	VEC		tvc;

	int		n, i, i2;
	float		f, f2, fx, fy, fz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	bd_p = &ball_obj;
	vc_p = &vvc;

	bpos.x = bd_p->odata.x;		// Save
	bpos.y = bd_p->odata.y;
	bpos.z = bd_p->odata.z;
	brotv.x = ballrotv.x;
	brotv.y = ballrotv.y;
	brotv.z = ballrotv.z;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Move close to target

	n = bd_p->total_time - 10;
	do {
		bd_p->odata.vy += BALL_GRAVITY;

//		t.x = bd_p->odata.x;
//		t.y = bd_p->odata.y;
//		t.z = bd_p->odata.z;
//
//		i = COLLPERTICK;
//		f = 1.0f / COLLPERTICK;
//		fx = bd_p->odata.vx * f;
//		fy = bd_p->odata.vy * f;
//		fz = bd_p->odata.vz * f;
//		i = 1;
//		do {
//			f2 = COLLPERTICK / 1.0f;
//			t.x += fx * f2;
//			t.y += fy * f2;
//			t.z += fz * f2;
//		} while (--i > 0);

		i = COLLPERTICK;			// Must loop to get exact float errors as coll
		f = 1.0f / COLLPERTICK;
		fx = bd_p->odata.vx * f;
		fy = bd_p->odata.vy * f;
		fz = bd_p->odata.vz * f;
		do {
			bd_p->odata.x += fx;
			bd_p->odata.y += fy;
			bd_p->odata.z += fz;
		} while (--i > 0);

	{
		int	i1, i2;
		tvc.x = t.x;
		tvc.y = t.y;
		tvc.z = t.z;
		i1 = *(int *) &tvc.x;
		i2 = *(int *) &bd_p->odata.x;
		if (i1 != i2) {
			printf ("XV err %x %x", i1, i2);
			printf (", %f %f\n", t.x, bd_p->odata.x);
		}
		i1 = *(int *) &tvc.y;
		i2 = *(int *) &bd_p->odata.y;
		if (i1 != i2) {
			printf ("YV err %x %x", i1, i2);
			printf (", %f %f\n", t.y, bd_p->odata.y);
		}
		i1 = *(int *) &tvc.z;
		i2 = *(int *) &bd_p->odata.z;
		if (i1 != i2) {
			printf ("ZV err %x %x", i1, i2);
			printf (", %f %f\n", t.z, bd_p->odata.z);
		}
	}

//		i = ball_collide();
//		if (i) printf ("Sim ERROR!\n");
	} while (--n > 0);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	bd_p->odata.x = bpos.x;		// Restore
	bd_p->odata.y = bpos.y;
	bd_p->odata.z = bpos.z;
	ballrotv.x = brotv.x;
	ballrotv.y = brotv.y;
	ballrotv.z = brotv.z;

}
#endif

/********************************
 Move ball, call collider if necessary
********************************/

int	ball_move()
{
	ball_data	*bd_p;

	int		i;
	float		f, fx, fz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Test

#if	0
	ball_calcstatus (50);
	printf ("Bstat: %d %d\n", ballsimstat.scored, ballsimstat.time);
#endif

#ifdef	DEBUG_SIMTEST
	ball_shotsimtest();
#endif

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	bd_p = &ball_obj;

	bd_p->odata.vy += BALL_GRAVITY;

	if (game_info.ball_mode == BM_BOUNCE_ON_RIM ||
			game_info.ball_mode == BM_SHOOTING ||
			game_info.ball_mode == BM_OFF_INTO_PLAY) {

		ball_collide();
	}
	else {
		bd_p->odata.x += bd_p->odata.vx;
		bd_p->odata.y += bd_p->odata.vy;
		bd_p->odata.z += bd_p->odata.vz;

		if (game_info.ball_mode == BM_PASSING)
			if (bd_p->flags & BF_TURBO)
				ball_trail();
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Calc ball depth bias

	balldbias = 0;		// None

	fx = bd_p->odata.x;
	if (fx < 0)
		fx -= -RIM_CX;
	else
		fx -= RIM_CX;

	fz = bd_p->odata.z - RIM_CZ;
	f = fx * fx + fz * fz;
	f = fsqrt (f);						// Distance

	if (f <= BALL_RADIUS * 1.6f) {	// Ball close to net?
		f = RIM_CY - bd_p->odata.y;

		if (f > -BALL_RADIUS * .3f && f < BALL_RADIUS * 5)	// In net?
			balldbias = 50;	// Move ball back
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#ifdef	DEBUG_JOYBALL
	if (!halt) {

		static int	on;
		VEC		*vc_p;
		float		f;

		game_info.ball_mode = BM_OFF_INTO_PLAY;

		player_blocks[0].no_ball_collide = 0;
		player_blocks[1].no_ball_collide = 0;
		player_blocks[2].no_ball_collide = 0;
		player_blocks[3].no_ball_collide = 0;
		


		i = get_player_sw_close();
		if (i & P4_A) on ^= -1;

		if (on) {
			i = get_p5_sw_current();

			vc_p = &balls;
			f = BALL_RADIUS * .3f;

			if ((i & P_C) == 0) {
				if (i & P_UP)
					vc_p->z -= f;
				if (i & P_DOWN)
					vc_p->z += f;
			}
			else {
				if (i & P_UP)
					vc_p->y += fabs (f);
				if (i & P_DOWN)
					vc_p->y -= fabs (f);
			}
			if (i & P_LEFT)
				vc_p->x -= f;
			if (i & P_RIGHT)
				vc_p->x += f;

			bd_p->odata.x = balls.x;
			bd_p->odata.y = balls.y;
			bd_p->odata.z = balls.z;
			bd_p->odata.vx = 0;
			bd_p->odata.vy = 0;
			bd_p->odata.vz = 0;
		}
	}
#endif

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	return (0);
}


/********************************
 Collide ball with rim, net, & backboard
********************************/

#define	RIM_SPRING		.70f		//.85f		// Rim springiness
#define	BKBRD_SPRING	.75f		// Glass springiness
#define	BKBRD_FSPRING	.75f		// Frame

static	VEC		bpos;				// Ball position

int	ball_collide()
{
	ball_data	*bd_p;
	BBDATA	*bbd_p;

	VEC		bv;				// Ball scaled vel
	float		vscl;				// Vel scale

	int		hitcnt;			// # of hits
	int		hitccnt;			// # consecutive hits
	int		hit;				// !0=Hit in loop

	float		rcx;
	float		brcdx;			// Ball to rim center dist
	float		brcdz;
	float		brcd;				// Ball distance

	VEC		redge;			// Closest rim edge
	float		dist;				// Ball to rim edge distance
	float		dx, dy, dz;
	VEC		bkpos;			// Backboard position

	VEC		vc;
	VEC		vc2;
	VEC		*vc_p;

	void		*v_p;
#ifdef JONHEY
	void		*v_bbox;
#endif
	int		n, i;
	float		f, f2, f3, fx, fy, fz;

#ifdef DEBUG
	int cnt = 0,idx;
#endif
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	bd_p = &ball_obj;
	vc_p = &ballrotv;

	bd_p->flight_time++;

	bpos.x = bd_p->odata.x;
	bpos.y = bd_p->odata.y;
	bpos.z = bd_p->odata.z;

	rcx = RIM_CX;
	bkpos.x = BKBRD_X;
	bkpos.y = BKBRD_Y;
	bkpos.z = BKBRD_Z;

	if (bd_p->odata.x < 0) {
		rcx = -RIM_CX;
		bkpos.x = -BKBRD_X;
	}

	hitcnt = 0;
	hitccnt = 0;

	n = COLLPERTICK;
	vscl = 1.0f / COLLPERTICK;

	do {

		bv.x = bd_p->odata.vx * vscl;
		bv.y = bd_p->odata.vy * vscl;
		bv.z = bd_p->odata.vz * vscl;

		bpos.x += bv.x;
		bpos.y += bv.y;
		bpos.z += bv.z;

		hit = 0;

	//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Backboard collision

		fx = bpos.x - bkpos.x;
		if (fabs (fx) < BALL_RADIUS) {

			fy = bpos.y - bkpos.y;
			fz = bpos.z - bkpos.z;
			if (fy > -BKBRD_H / 2 && fy < BKBRD_H / 2 &&
					fz > -BKBRD_W / 2 && fz < BKBRD_W / 2) {	// Glass hit?

				hit = 1;
				bpos.x -= bv.x;		// Backoff
				bpos.y -= bv.y;
				bpos.z -= bv.z;

				f = fabs (bd_p->odata.vx) * BKBRD_SPRING;
				if (fx < 0) f = -f;
				bd_p->odata.vx = f;
				bd_p->odata.vz *= BKBRD_SPRING;

				if (!ballsim) {

//					i = (VOLUME1 + 50) * fabs (f * 2);
					i = (VOLUME1 + 150) * fabs (f * 2);
					if (i > 5)
						make_hit_backboard1_snd(i);

					gBack_board_hits++;	// Keep an active per-shot count..for speech calls
//					printf("*Hit bkbrd x=%f vel=%f vol=%d\n", fx, f, i);
				}


			}
			else {	// Try edges

				dx = bkpos.x - bpos.x;		// Calc XYZ of top collision vector

				dy = bkpos.y + BKBRD_H / 2 - bpos.y;
				fz = bpos.z;
				if (fz < bkpos.z - BKBRD_W / 2) fz = bkpos.z - BKBRD_W / 2;
				if (fz > bkpos.z + BKBRD_W / 2) fz = bkpos.z + BKBRD_W / 2;
				dz = fz - bpos.z;
				dist = fsqrt (dx * dx + dy * dy + dz * dz);

				if (dist > BALL_RADIUS) {	// Top edge not in ball?

					fy = bpos.y;				// Calc YZ of coll vector
					if (fy < bkpos.y - BKBRD_H / 2) fy = bkpos.y - BKBRD_H / 2;
					if (fy > bkpos.y + BKBRD_H / 2) fy = bkpos.y + BKBRD_H / 2;
					dy = fy - bpos.y;

					dz = bkpos.z - bpos.z;
					if (dz < 0) dz += BKBRD_W / 2;
					else dz -= BKBRD_W / 2;

					dist = fsqrt (dx * dx + dy * dy + dz * dz);
				}

				if (dist <= BALL_RADIUS) {	// Top edge in ball?

					hit = 1;
					bpos.x -= bv.x;		// Backoff
					bpos.y -= bv.y;
					bpos.z -= bv.z;

					f = 1.0f / dist;		// Normalize coll vec
					dx *= f;
					dy *= f;
					dz *= f;

					bv.x = bd_p->odata.vx * BKBRD_FSPRING;
					bv.y = bd_p->odata.vy * BKBRD_FSPRING;
					bv.z = bd_p->odata.vz * BKBRD_FSPRING;

					// Rotate velocity 180 around distance vector
					f = dx * bv.x + dy * bv.y + dz * bv.z;		// Dot product
					fx = dx * f;
					fy = dy * f;
					fz = dz * f;
					fx += fx - bv.x;
					fy += fy - bv.y;
					fz += fz - bv.z;

					bd_p->odata.vx = -fx;	// Neg vel
					bd_p->odata.vy = -fy;
					bd_p->odata.vz = -fz;


					if (!ballsim) {

						f = fsqrt (fx * fx + fy * fy + fz * fz);
//						i = (VOLUME1 + 50) * f * 2;
						i = (VOLUME1 + 150) * f * 2;
						if (i > 5)
							make_hit_backboard1_snd(i);

						gBack_board_hits++;	// Keep an active per-shot count..for speech calls
//						printf("*Hit bkbrd edge vel=%f vol=%d\n", f, i);
					}
				}
			}

			if (hit) {
				hitcnt++;
				hitccnt++;
				if (ballsim) continue;

//				game_info.ball_mode = BM_OFF_INTO_PLAY;
				continue;
			}
		}

	//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Rim collision

		brcdx = bpos.x - rcx;
		brcdz = bpos.z;	//- RIM_CZ;
		brcd = brcdx * brcdx + brcdz * brcdz;

		if (brcd > .0001f) {
			brcd = fsqrt (brcd);		// Distance
		}

		if (brcd >= BALLINRIMRAD && brcd <= BALLOUTRIMRAD) {

			f = RIM_RAD / brcd;
			fx = brcdx;
			fz = brcdz;

			redge.x = rcx + fx * f;		// Rim edge = ball vec normalized * radius
			redge.y = RIM_CY;
			redge.z = RIM_CZ + fz * f;

			dx = redge.x - bpos.x;		// Collision vector
			dy = redge.y - bpos.y;
			dz = redge.z - bpos.z;

			dist = fsqrt (dx * dx + dy * dy + dz * dz);

			f = RIM_EDGERAD / dist;		// Rim edge + normalized coll vec * edge radius
			redge.x -= dx * f;
			redge.y -= dy * f;
			redge.z -= dz * f;

			dx = redge.x - bpos.x;		// Collision vector
			dy = redge.y - bpos.y;
			dz = redge.z - bpos.z;

			dist = fsqrt (dx * dx + dy * dy + dz * dz);

			if (dist <= BALL_RADIUS) {	// Rim in ball?

//				printf("*Hit  rim=%f %f %f dist=%f\n", redge.x, redge.y, redge.z, dist);

				hit = 1;
				hitcnt++;
				hitccnt++;
				bpos.x -= bv.x;		// Backoff
				bpos.y -= bv.y;
				bpos.z -= bv.z;

				f = 1.0f / dist;		// Normalize coll vec
				dx *= f;
				dy *= f;
				dz *= f;

//				printf("*Hit  bvec=%f %f %f\n", dx, dy, dz);
				vc.x = dy * bv.z - dz * bv.y;	// Cross prod coll vec & velocity
				vc.y = dz * bv.x - dx * bv.z;	//  = rotational vec
				vc.z = dx * bv.y - dy * bv.x;

				vc2.x = vc_p->y * dz - vc_p->z * dy;	// Cross prod rot vec & coll unit vec
				vc2.y = vc_p->z * dx - vc_p->x * dz;
				vc2.z = vc_p->x * dy - vc_p->y * dx;

				f2 = fsqrt (bv.x * bv.x + bv.y * bv.y + bv.z * bv.z);	// Vel unit vec
				if (f2 > .000001f) f2 = 1.0f / f2;
				fx = bv.x * f2;
				fy = bv.y * f2;
				fz = bv.z * f2;
				f2 = dx * fx + dy * fy + dz * fz;	// Dot coll vec & vel vec (0-1)
				f2 = f2 * .8f;		// Friction. Slow rotv even on perpendicular hit
				f2 = 1 - (1 - f2) * .8f;	// Slippage (f2 = .2 to 1)
				f3 = 1 - f2;

				f = f3 * (1/_AVLV) / vscl;		// Add full vel to rot vel
				ballrotv.x = vc_p->x * f2 + vc.x * f;
				ballrotv.y = vc_p->y * f2 + vc.y * f;
				ballrotv.z = vc_p->z * f2 + vc.z * f;

				f = RIM_SPRING;
//				if (hitccnt > 1)
//					f = RIM_SPRING + .2f;

				bv.x = bd_p->odata.vx * f;
				bv.y = bd_p->odata.vy * f;
				bv.z = bd_p->odata.vz * f;

//#define	_LV	.595f
//#define	_AV	.4f
#define	_LV	1
#define	_AV	.25f
				bv.x = bv.x * _LV + vc2.x * _AVLV * _AV;	// Add rot vel to vel
				bv.y = bv.y * _LV + vc2.y * _AVLV * _AV;	//  - some friction
				bv.z = bv.z * _LV + vc2.z * _AVLV * _AV;

//				printf ("Hit rot %f %f %f", ballrotv.x, ballrotv.y, ballrotv.z);
//				printf ("vel1 %f %f %f\n", bv.x, bv.y, bv.z);

				// Rotate velocity 180 around coll vector
				f = dx * bv.x + dy * bv.y + dz * bv.z;		// Dot product
				fx = dx * f;
				fy = dy * f;
				fz = dz * f;
				fx += fx - bv.x;
				fy += fy - bv.y;
				fz += fz - bv.z;

//				printf("*Hit  vel2=%f %f %f\n", fx, fy, fz);

				bd_p->odata.vx = -fx;	// Neg vel
				bd_p->odata.vy = -fy;
				bd_p->odata.vz = -fz;

				if (ballsim) continue;

				ball_collrim (&bv, bkpos.x);

				continue;
			}
		}

	//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ	Net collision

#if	1

		if (brcd <= BALL_RADIUS * 1.5f) {	// Ball close to net?
			f = RIM_CY - bpos.y;

			if (f > BALL_RADIUS * .02f && f < BALL_RADIUS * .41f && bv.y < 0)
			{
				bscoremd = -1;
				player_blocks[0].no_ball_collide =
				player_blocks[1].no_ball_collide =
				player_blocks[2].no_ball_collide =
				player_blocks[3].no_ball_collide = 25;

			}

			if (f > BALL_RADIUS * .4f && f < BALL_RADIUS * 3.1f) {

				fx = bd_p->odata.vx;
				fy = bd_p->odata.vy;
				fz = bd_p->odata.vz;
				f2 = .002f;
				f3 = .99f;
				if (brcd > BALL_RADIUS * .8f) {
					f2 = .004f;
					f3 = .93f;
					if (brcd > BALL_RADIUS * 1.2f)
						f3 = .6f;
				}
				fx = fx * f3 - brcdx * f2;	// Net friction
				fz = fz * f3 - brcdz * f2;
				fy *= .99f;
				ballrotv.x *= .99f;
				ballrotv.y *= .99f;
				ballrotv.z *= .99f;

				bd_p->odata.vx = fx;
				bd_p->odata.vy = fy;
				bd_p->odata.vz = fz;

#ifdef	DEBUG_SHTBALL
//				if (halt) printf("*Net  vel=%f %f %f\n", fx, fy, fz);
#endif
				if (bscoremd == -1 && f > BALL_RADIUS * .45f) {	// Score?

					if (ballsim) {
						ballsimsc = 1;
						return (hitcnt);
					}
#ifdef DEBUG
					idx = cnt;while(idx--)fprintf(stderr," ");cnt++;
					fprintf(stderr,"ball_collscore: %d\r\n",n);
#endif
					ball_collscore (rcx, bkpos.x);
				}

			}


		}
#endif

		hitccnt = 0;

	} while (--n > 0);


//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	if (hitccnt > 15) {	// Many consecutive hits? Probably stuck

		bpos.y += bd_p->odata.vy - .5f;	// Move down
#ifdef	DEBUG_BALLSTUCK
		printf("*Ball stuck error!\n");
#endif
	}


	bd_p->odata.x = bpos.x;
	bd_p->odata.y = bpos.y;
	bd_p->odata.z = bpos.z;


	if (hitcnt == 0) {
		ball_collchkmode (rcx);
	}

	return (hitcnt);
}

/********************************
 Hit rim
 (* ball's scaled vel, backboard x)
********************************/

void	ball_collrim (VEC *bv_p, float bkposx)
{
	ball_data	*bd_p;
	BBDATA	*bbd_p;

	int		bknum;
	VEC		bv;

	void		*v_p, *v2_p;
	int		i;
	float		f, f2, f3, fx, fy, fz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	bd_p = &ball_obj;

	bv.x = bv_p->x;
	bv.y = bv_p->y;
	bv.z = bv_p->z;

	f3 = fsqrt (bv.x * bv.x + bv.y * bv.y + bv.z * bv.z);	// Speed

	bknum = 0;
	if (bd_p->odata.x > 0) {
		bknum = 1;
	}
	bbd_p = &bbdata[bknum];

	if (bbd_p->netv_p == 0 || bbd_p->netvpritm <= 0) {

		if (f3 > .1f) {	// Speed worthy of shake?
			v_p = nt_tiny_verts;
#ifdef JONHEY
			v2_p = nt_tiny_bbox;
#endif
			if (f3 > .3f) {
				v_p = nt_gentle_verts;
#ifdef JONHEY
				v2_p = nt_gentle_bbox;
#endif
				if (f3 > .8f) {
					v_p = nt_rim_verts;
#ifdef JONHEY
					v2_p = nt_rim_bbox;
#endif
				}
			}
#ifdef	DEBUG_SHTBALL
			printf("Rim rv=%f\n", f3);
#endif
			bbd_p->netv_p = v_p;
#ifdef JONHEY
			bbd_p->netv_bbox = v2_p;
#endif
			bbd_p->netvposf = 0;
			bbd_p->netvstep = 1;
			bbd_p->netvpritm = 22;
		}
	}

	// % between rim front & backboard
	f = fabs (bpos.x - bkposx) - BALL_RADIUS;
	f *= 1.0f / ((BACKBOARD_X - RIM_X) * .7f);

	if (bv.y < -.2f) {

		bbd_p->rimmom += bv.y * 200.0f * f;	// Add momentum

		if (bbd_p->rimbendcnt < 1) {
			bbd_p->rimbend = bv.y * 120.0f * f;
			bbd_p->rimbendcnt = 1;
			set_process_run_level (qcreate ("rimshake", PLAYER_PID, rimshake_proc, bknum, 0, 0, 0), RL_BALL+2);
		}

		f2 = 1 - f + .2f;									// % of force to backboard
		bbd_p->mom += bv.y * 30.0f * f2;	// Add momentum

		if (bbd_p->bendcnt < 1) {
			bbd_p->bend = bv.y * 15.0f * f2;
			bbd_p->bendcnt = 1;
			set_process_run_level (qcreate ("bbshake", PLAYER_PID, bbshake_proc, bknum, 0, 0, 0), RL_BALL+2);
		}
#ifdef DEBUG
//		printf("Bkbd force %f\n", bv.y);
		printf("Rim shake 1 - allyoop?\n");
#endif
	}

//	printf("Rim force %f %f\n", f, f2);

	if (f3 > .3f) {
		i = VOLUME1 * (f3 + .1f);	// 10 to 140% of\ max
		i = i + 150;
		make_hit_rim_snd(i);

		if (bd_p->who_shot == game_info.fire_player)
		{
			if (game_info.fire_cnt >= FIRECNT)
				ball_spark (0, f3 * 40);
		}
		else if (game_info.team_fire == player_blocks[ball_obj.who_shot].team)
				ball_spark (0, f3 * 40);

	}
	game_info.ball_mode = BM_BOUNCE_ON_RIM;

	gRim_hits++;	// Keep an active per-shot count..for speech calls

	// dont allow player to snag ball off rim
	if (gRim_hits == 1)
	{
		ignore_steal = randrng(15) + 15;	//25
		player_blocks[0].no_ball_collide =
		player_blocks[1].no_ball_collide =
		player_blocks[2].no_ball_collide =
		player_blocks[3].no_ball_collide = ignore_steal;		//25
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	i = 0;
	if (bd_p->who_shot >= NUM_PERTEAM) i = 1;
	if (i != bknum) {	// Opponents basket?

		reset_shotclock = 1;	// Tell shot clock to reset - auto clean up any on screen graphics
	}
}

/********************************
 Ball scored
 (Rim center x, backboard x)
********************************/

void	ball_collscore (float rcx, float bkposx)
{
	ball_data	*bd_p;
	BBDATA	*bbd_p;

	int		bknum;
	VEC		vc;

	void		*v_p;
#ifdef JONHEY
	void		*v_bbox;
#endif
	int			i, wstm, ws, fp, was_fire;
	float		f, f2, f3, fx, fy, fz;
	
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	bd_p = &ball_obj;


	fx = bd_p->odata.vx;
	fy = bd_p->odata.vy;
	fz = bd_p->odata.vz;


#ifdef	DEBUG_CAMBALL
	printf ("Sctm %d\n", bd_p->flight_time);
#endif
	bscoremd = 0;

	f = fx * fx + fz * fz;
	f2 = fsqrt (fy * fy + f);		// Total speed
	f3 = fsqrt (f);					// Horizontal speed
//	printf("*Net  vel=%f\n", f);

	bknum = 0;
	if (bd_p->odata.x > 0) {
		bknum = 1;
	}
	bbd_p = &bbdata[bknum];

	v_p = nt_swish_verts;
#ifdef JONHEY
	v_bbox = nt_swish_bbox;
#endif
//	if (brcd < BALL_RADIUS * .2f)		// Near center?
//		if (f3 < .2f)
//			v_p = nt_rim_verts;

	if (f3 > .6f) {
		v_p = nt_swshb_verts;
#ifdef JONHEY
		v_bbox = nt_swshb_bbox;
#endif
//		if ((rand() & 3) == 0)
//			v_p = nt_twist_verts;

		if (f3 > .9f) {
			v_p = nt_hard_verts;
#ifdef JONHEY
			v_bbox = nt_hard_bbox;
#endif
		}
	}

	bbd_p->netv_p = v_p;
#ifdef JONHEY
	bbd_p->netv_bbox = v_bbox;
#endif
	bbd_p->netvposf = 0;

	f = f2 * .75f;
	if (f < .6f) f = .6f;
	if (f > 1.4f) f = 1.4f;
	bbd_p->netvstep = f;

	bbd_p->netvpritm = 45;

#ifdef	DEBUG_SHTBALL
	printf("Net v=%f %f St=%f\n", f2, f3, f);
#endif
	vc.x = rcx;
	vc.y = RIM_CY;
	vc.z = RIM_CZ;
	i = ptangle ((PV) &vc, (PV) &bpos) + 512;
	bbd_p->netya = i;

	i = VOLUME1 - 40 + f * 40;
	i = i + 90;
	make_net_snd(i);

	game_info.ball_mode = BM_OFF_INTO_PLAY;


	i = 0;
	if (bd_p->who_shot >= NUM_PERTEAM) i = 1;
	if (i == bknum) return;		// My basket? Don't score

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

// DEBUG
//	set_process_run_level (qcreate ("netfire", PLAYER_PID, net_fire_proc, 50, 0, 0, 0), RL_BALL+6);

	wstm = player_blocks[ball_obj.who_shot].team;
	ws = ball_obj.who_shot;
	fp = game_info.fire_player;
	was_fire = 0;


	// handle 'TEAM FIRE'
	// inc 'TEAM FIRE' count ??
	if ((game_info.tm_fire_cnt[wstm^1] == 0) && (player_blocks[ws].odata.flags & PF_ALLYDUNKER))
		game_info.tm_fire_cnt[wstm]++;
	else if (game_info.team_fire == wstm)
		game_info.tm_fire_cnt[wstm]++;
	else if (player_blocks[ws].odata.flags & PF_ALLYDUNKER)
	{	// other team did allyoop

		if ((player_blocks[ws].team != player_blocks[fp].team) && (game_info.team_fire != -1))
			was_fire = 1;

		game_info.team_fire = -1;
		game_info.tm_fire_cnt[wstm^1] = 0;
		game_info.tm_fire_cnt[wstm] = 1;

		// set so that player is the new 'PLAYER FIRE'
		game_info.fire_player = ws;
	}
//	else
//	{
//		game_info.tm_fire_cnt[wstm^1] = 0;
//#ifdef	DEBUG
//		printf ("Stop other tm from approaching tm fire start\n");
//#endif
//	}

	// check limits of team fire
	if (game_info.tm_fire_cnt[wstm] == TM_FIRECNT)
	{	// start 'TEAM FIRE'
		game_info.team_fire = wstm;
		// turn off 'PLAYER FIRE'
		game_info.fire_player = ws;
		game_info.fire_cnt = 0;

		ball_fireset (1, 50, 0);
		set_process_run_level (qcreate ("netfire", PLAYER_PID, net_fire_proc, 50, 0, 0, 0), RL_BALL+6);

		// Turn on text message reading team fire
		qcreate("tmfire", 0, show_tmfire_proc, 0, 0, 0, 0);


	}
	else if (game_info.tm_fire_cnt[wstm] > TM_FIRECNT)
	{
		//game_info.tm_fire_cnt[wstm]++;
		if (game_info.tm_fire_cnt[wstm] > MAX_TM_FIRE_SHOTS+TM_FIRECNT)
		{	// turn off 'team fire'
			game_info.tm_fire_cnt[wstm] = 0;
			game_info.team_fire = -1;
		}
		ball_fireset (1, 50, 0);
		set_process_run_level (qcreate ("netfire", PLAYER_PID, net_fire_proc, 50, 0, 0, 0), RL_BALL+6);
	}

	// handle 'PLAYER FIRE' only if not on team fire
	if (game_info.team_fire < 0)
	{
		if (ws == fp)
		{	// same person made shot as last one...
			 game_info.fire_cnt++;
			 game_info.fire_player = ws;
			 if (game_info.fire_cnt > MAX_FIRE_SHOTS+FIRECNT)
			 {
			 	game_info.fire_cnt = 0;
			 	game_info.fire_player = ws;
			 }
			 else if (game_info.fire_cnt >= FIRECNT)
			 {
			 	ball_fireset (0, 50, 0);
			 	set_process_run_level (qcreate ("netfire", PLAYER_PID, net_fire_proc, 50, 0, 0, 0), RL_BALL+6);
			 }
		}
		else
		{	// no, but teammate doesnt reset your count or stop teammates fire!
			if (player_blocks[ws].team != player_blocks[fp].team)
			{
				// reset other teams team-fire count
				game_info.tm_fire_cnt[wstm^1] = 0;

				if (game_info.fire_cnt >= FIRECNT)
					was_fire = 1;
					
				game_info.fire_cnt = 1;
				game_info.fire_player = ws;
			}
			else if (game_info.fire_cnt < FIRECNT)
			{
				game_info.fire_player = ws;
				game_info.fire_cnt = 1;
			}

			// clear other teams team-fire count
//			game_info.tm_fire_cnt[wstm^1] = 0;
		}
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	//if(player_blocks[ball_obj.who_shot].odata.adata[0].seq_index >= (SEQ_J_SHT1_R) && player_blocks[ball_obj.who_shot].odata.adata[0].seq_index <= (SEQ_J_SHT2_L))
	//	change_anim((obj_3d *)(player_blocks+ball_obj.who_shot), taunt_4_anim);

	game_info.sc_scored = ball_obj.who_shot;
//	ball_obj.who_shot = -1;	// OLD!!!

	i = 2;
	if (what_type_shot == JUMPER_3)
	{
		// Show 3 pt message under players name who scored at top of screen
		qcreate("3pts", 0, show_3pt_proc, 0, 0, 0, 0);
		i = 3;
	}
#ifdef DEBUG
	//DEBUG - test tm fire txt message
//	else
//		qcreate("tmfire", 0, show_tmfire_proc, 0, 0, 0, 0);
#endif

		
	game_info.scores[game_info.off_side][game_info.game_quarter] += i;
	// this flashes the basketball behind the turbo bar
	qcreate("flshball", 0, flash_basketball_proc, game_info.sc_scored, 0, 0, 0);

	// inc. game stat ?
	if (gAssist_time)
	{
		plyr_stat_ptrs[game_info.sc_scored^1]->assists++;
		gAssist_time = 0;
	}

	// inc. game stats
	if (game_info.game_mode != GM_FREE_THROW)
	{
		if (what_type_shot == JUMPER_3)
			plyr_stat_ptrs[game_info.sc_scored]->three_ptrs_made++;
		else if (what_type_shot == DUNK)
			plyr_stat_ptrs[game_info.sc_scored]->dunks_made++;
		else
			plyr_stat_ptrs[game_info.sc_scored]->two_ptrs_made++;

		iqcreate("made_sp",SPEECH_PID,shot_made_speech_proc,game_info.sc_scored,was_fire,wstm,(int)plyr_launched_from_hotspot(game_info.sc_scored));
	}
	else
	{
		// free throw are always 3 points!
	 	plyr_stat_ptrs[game_info.sc_scored]->three_ptrs_made++;

		qcreate("made_ft",SPEECH_PID,freethrow_made_speech_proc,game_info.sc_scored,0,0,0);
	}

	player_blocks[game_info.sc_scored].shots_missed = 0;

	if (plyr_launched_from_hotspot(game_info.sc_scored) == YES) 
	{
		game_info.hotspot_made_cnt[game_info.sc_scored] += 1;
		player_blocks[game_info.sc_scored].launched_from_hotspot = 0;
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	// Rattle the whole thing on a dunk...

	if (what_type_shot == DUNK && player_blocks[game_info.shooter].odata.flags & (PF_DUNKER|PF_ALLYDUNKER)) {

		float vy = -.21f;

		i = bd_p->odata.x > 0 ? 1 : 0;
		// % between rim front & backboard
		f = fabs (bkposx) - BALL_RADIUS;
		f *= 1.0f / ((BACKBOARD_X - RIM_X) * .7f);

		if (vy < -.2f) {

			bbd_p->rimmom += vy * 200.0f * f;	// Add momentum

			if(bbd_p->rimbendcnt < 1)
			{
				bbd_p->rimbend = vy * 120.0f * f;
				bbd_p->rimbendcnt = 1;
				set_process_run_level (qcreate ("rimshake", PLAYER_PID, rimshake_proc, i, 0, 0, 0), RL_BALL+2);
			}

			f2 = 1 - f + .2f;	// % of force to backboard
			bbd_p->mom += vy * 90.0f * f2;	// Add momentum

			if(bbd_p->bendcnt < 1)
			{
				bbd_p->bend = vy * 45.0f * f2;
				bbd_p->bendcnt = 1;
				set_process_run_level (qcreate ("bbshake", PLAYER_PID, bbshake_proc, i, 0, 0, 0), RL_BALL+2);
			}


#ifdef DEBUG
//			printf("Bkbd force %f\n", bv.y);
			printf("Rim shake 2 - allyoop?\n");
#endif

		}
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

//	game_info.shooter = -1;

	update_status_box = 1;	// Update score
	clock_active = 0;		// Stop game clock
	reset_shotclock = 1;	// Reset shotclock

	crowd_cheer_time = 180 * 3;

	if (game_info.game_mode != GM_FREE_THROW)
		SetUpOOBounds(0,0,0);
}


/********************************
 Check ball mode
 (Rim center x)
********************************/

void	ball_collchkmode (float rcx)
{
	ball_data	*bd_p;
	float		brcdx;			// Ball to rim center dist
	float		brcdz;
	float		brcd;				// Ball distance

	int		i;
	float		f;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	bd_p = &ball_obj;

	if (game_info.ball_mode == BM_SHOOTING ||
			game_info.ball_mode == BM_BOUNCE_ON_RIM) {

		if (bd_p->flight_time > bd_p->total_time) {

			i = 0;

			brcdx = bpos.x - rcx;
			brcdz = bpos.z - RIM_CZ;
			brcd = brcdx * brcdx + brcdz * brcdz;
			brcd = sqrt (brcd);		// Distance

			if (brcd > RIM_RAD + BALL_RADIUS) {		// Ball mostly out of rim cylinder?
				i = 1;
#ifdef	DEBUG_CAMBALL
				if (!ballsim)
					printf ("BMd play, rad\n");
#endif
			}
			f = RIM_CY - bpos.y;
			if (brcd > RIM_RAD && f > BALL_RADIUS * .05f && bd_p->odata.vy < 0) {
				i = 1;
#ifdef	DEBUG_CAMBALL
				if (!ballsim)
					printf ("BMd play, Y\n");
#endif
			}
			if (i) {
				if (ballsim) {
					ballsimsc = -1;
					return;
				}
				bscoremd = 0;

#ifndef	DEBUG_NO_ANNOUNCER
				if (game_info.ball_mode == BM_BOUNCE_ON_RIM) {

					qcreate("made_sp",SPEECH_PID+1,shot_missed_speech_proc,ball_obj.who_shot,0,0,0);
				}
#endif	
				game_info.ball_mode = BM_OFF_INTO_PLAY;

#ifdef	DEBUG_CAMBALL
				printf ("Pltm %d\n", bd_p->flight_time);
#endif

			}
		}
	}
}

/****************************************************************/

/********************************
 Backboard & rim shake
 (Goal # (0-1), mode (0=Release, +=Bend), shake amount)
********************************/

void	goal_shake (int gnum, int mode, float shk)
{
	BBDATA	*bbd_p;
	static	int	bent;
//	int		i;

	float		f, f2;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	bbd_p = &bbdata[gnum];
	f = .5f;

	if (mode) {		// Bend goal?
		bent = 1;
		shk *= 3;
		bbd_p->rimbend = shk * 120.0f * f;
		f2 = 1 - f + .2f;							// % of force to backboard
		bbd_p->bend = shk * 15.0f * f2;
		return;
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ If bent then bend and release

	if (!bent) return;	// Not bent?
	bent = 0;

	if (shk < -.2f) {

		bbd_p->rimmom += shk * 200.0f * f;	// Add momentum

		if (bbd_p->rimbendcnt <= 0) {
			bbd_p->rimbend = shk * 120.0f * f;
			bbd_p->rimbendcnt = 1;
			set_process_run_level (qcreate ("rimshake", PLAYER_PID, rimshake_proc, gnum, 0, 0, 0), RL_BALL+2);
		}

		f2 = 1 - f + .2f;									// % of force to backboard
		bbd_p->mom += shk * 30.0f * f2;	// Add momentum

		if (bbd_p->bendcnt <= 0) {
			bbd_p->bend = shk * 15.0f * f2;
			bbd_p->bendcnt = 1;
			set_process_run_level (qcreate ("bbshake", PLAYER_PID, bbshake_proc, gnum, 0, 0, 0), RL_BALL+2);
		}

#ifdef DEBUG
//		printf("Bkbd force %f\n", shk);
		printf("Rim shake 3 - allyoop?\n");
#endif
//		i = rndrng0 (99) < 50 ? DUNK1_SND : DUNK2_SND;
//		make_dunk_snd(i);
//		make_dunk_snd(i);
	}
}

/********************************
 Backboard rim shake
 (*[0] = Rim (0-1))
********************************/

void	rimshake_proc (int *pa_p)
{
	BBDATA	*bbd_p;
//	ball_data	*bd_p;
//	int		time;

	float		bend;
	float		mom;		// Momentum

//	int		i;
//	float		fx, fy, fz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	bbd_p = &bbdata[pa_p[0]];

//	if (bbd_p->rimbendcnt != 0) 
//		printf (">1 BENDS!!!\n");
//	bbd_p->rimbendcnt = 1;

	bend = bbd_p->rimbend;

	do {
		mom = bbd_p->rimmom;

		bend += mom;
		bbd_p->rimmom = mom * .9f - bend * .2f;

//		printf ("*Shk b=%f m=%f\n", bend, mom);

		bbd_p->rimbend = bend;
		sleep (1);

	} while ((bend > 5.0f || bend < -5.0f) || mom > 5.0f || mom < -5.0f);

//	printf ("Shake DONE\n");

	bbd_p->rimbendcnt = 0;
	bbd_p->rimbend = 0;

}


/********************************
 Backboard shake
 (*[0] = Rim (0-1))
********************************/

void	bbshake_proc (int *pa_p)
{
	BBDATA	*bbd_p;
//	ball_data	*bd_p;
//	int		time;

	float		bend;
	float		mom;		// Momentum

//	int		i;
//	float		fx, fy, fz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	bbd_p = &bbdata[pa_p[0]];

	bend = bbd_p->bend;

	do {
		mom = bbd_p->mom;

		bend += mom;
		bbd_p->mom = mom * .9f - bend * .2f;

//		printf ("*Shk b=%f m=%f\n", bend, mom);

		bbd_p->bend = bend;
		sleep (1);

	} while ((bend > 5.0f || bend < -5.0f) || mom > 5.0f || mom < -5.0f);

	bbd_p->bendcnt = 0;
	bbd_p->bend = 0;
}


/********************************
 Ball went into ground (not passing)
********************************/

void	ball_ingnd()
{
	ball_data	*bd_p;
	VEC		*vc_p;
	VEC		vc;
	int		i;
	float		f, fx, fz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	bd_p = &ball_obj;
	vc_p = &ballrotv;

//	if (fabs (bd_p->odata.vy) <= 0.4f && bd_p->odata.vy < 0.0f) {
//		bd_p->odata.vy = 0.0f;
//	}

	bd_p->odata.vy *= -.75f;

	bd_p->odata.y = BALL_RADIUS;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Apply angular velocity and velocity to each other

	fx = bd_p->odata.vx;
	fz = bd_p->odata.vz;

//	vc.x = vc_p->y * 0 - vc_p->z * -1;	// Cross prod rot vec & ball bottom unit vec
//	vc.y = vc_p->z * 0 - vc_p->x * 0;
//	vc.z = vc_p->x * -1 - vc_p->y * 0;
	vc.x = vc_p->z;	// Reduced version of ^
	vc.z = -vc_p->x;

	ballrotv.x = ballrotv.x * .60f - fz * (1/_AVLV) * .40f;	// Add vel to rot vel
	ballrotv.y = ballrotv.y * .60f;
	ballrotv.z = ballrotv.z * .60f + fx * (1/_AVLV) * .40f;

	bd_p->odata.vx = fx * .595f + vc.x * _AVLV * .40f;	// Add rot vel to vel
	bd_p->odata.vz = fz * .595f + vc.z * _AVLV * .40f;	//  - some friction

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	f = bd_p->odata.vy;
	if (f > 0.05) {
//		i = (f + .2f) * VOLUME1;
//		if (i < 10) i = 10;
//		if (i > SND_VOLUME_MAX - 20) i = SND_VOLUME_MAX - 20;
//		make_dribble1_snd(i);

		if (randrng(100) < 50)
			make_dribble1_snd(VOLUME7);
		else
			make_dribble2_snd(VOLUME7);
	}

#ifdef	DEBUG_CAMBALL
#ifndef	DEBUG_SHTBALL
{
	obj_3d	*o_p;

	o_p = (PV) &player_blocks[NUM_PERTEAM + 1];
	o_p->x = balls.x;
	o_p->z = balls.z;
	bd_p->odata.x = o_p->x;
//	bd_p->odata.y = o_p->y;
	bd_p->odata.z = o_p->z;

}
#endif
#endif

#ifdef	DEBUG_MVBALL
#ifndef	DEBUG_CAMBALL
{
	obj_3d	*o_p;

	o_p = (PV) &player_blocks[NUM_PERTEAM + 1];
	bd_p->odata.x = o_p->x;
//	bd_p->odata.y = o_p->y;
	bd_p->odata.z = o_p->z;

}
#endif
#endif

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

//	{
//		float	f1[3], f2[3];
//		f1[0] = 0;
//		f1[1] = 0;
//		f1[2] = 0;
//		f2[0] = 1;
//		f2[1] = 0;
//		f2[2] = 1;
//		i = ptangle (f1, f2);
//		printf("Ang %d\n", i);
//	}

}

/********************************
 Draw ball shadow
   Note that replays use this object code also
********************************/
//#define DEBUG_HEADS

void	draw_ball_shadow(void *oi)
{
	float mat[12] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f
	};
	float cm[12], ball_scale = 2.5f;

	//return;

	grConstantColorValue (PLAYBACK ? ((replay_obj_t *)oi)->color :
			((ball_data *)oi)->color);

//	grConstantColorValue (((ball_data *)oi)->alpha << 24);
	guAlphaSource (GR_ALPHASOURCE_CC_ALPHA);
//	guAlphaSource (GR_ALPHASOURCE_TEXTURE_ALPHA);

	grTexCombineFunction (0, GR_TEXTURECOMBINE_DECAL);
	grTexClampMode (0, GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP);

	grDepthBufferMode (GR_DEPTHBUFFER_WBUFFER);
	grDepthBufferFunction (GR_CMP_LESS);
	grDepthMask (FXTRUE);

	grCullMode (GR_CULL_NEGATIVE);
	grChromakeyMode (GR_CHROMAKEY_DISABLE);


	guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);
	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
	grTexClampMode(0,GR_TEXTURECLAMP_WRAP,GR_TEXTURECLAMP_WRAP);

	grDepthBufferMode( GR_DEPTHBUFFER_WBUFFER );
	grDepthBufferFunction( GR_CMP_ALWAYS );
	grDepthMask( FXTRUE );

//	grCullMode( GR_CULL_DISABLE );
	grChromakeyMode( GR_CHROMAKEY_DISABLE );

	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
	grAlphaBlendFunction( GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA,
			GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA );
	mat[ 3] = -cambot.x + ((obj_3d *)oi)->x;
	mat[ 7] = -cambot.y;
	mat[11] = -cambot.z + ((obj_3d *)oi)->z;

	roty( 0, mat );

	mxm( cambot.xform, mat, cm );
	cm[0] *= ball_scale; cm[1] *= ball_scale; cm[ 2] *= ball_scale;
	cm[4] *= ball_scale; cm[5] *= ball_scale; cm[ 6] *= ball_scale;
	cm[8] *= ball_scale; cm[9] *= ball_scale; cm[10] *= ball_scale;
	render_limb( &limb_ballshadow_obj, cm, &ball_shadow_decal, &ball_shadow_ratio );
}

/********************************
 Draw ball
   Note that replays use this object code also
********************************/

float bbx = 0, bby = 0, bbz = 0;

void	draw_ball (void *oi)
{
	VEC		vc;
	MAT4	m1;
	MAT4	m2;
	float	mat1[12], mat2[12], mat3[12];

	int		color, dbias, i;
	float	scale, f;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#if	0
	static	int glow;

	i = 0x40ffffff;
	if (btstvmd == 2) {
		glow = (glow + 2) & 0x1ff;
		i = glow;
		if (i > 0xff) i = 0x1ff - i;
		i = 0x80000000 | i << 16 | i << 8 | i;
//		i = 0x80ffff80;
	}

	grConstantColorValue (i);
	grColorCombine (GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL, GR_COMBINE_FACTOR_ONE,
			GR_COMBINE_LOCAL_CONSTANT, GR_COMBINE_OTHER_TEXTURE, 0);

	grChromakeyMode (GR_CHROMAKEY_DISABLE);

	guAlphaSource (GR_ALPHASOURCE_CC_ALPHA);
	grAlphaBlendFunction (GR_BLEND_SRC_ALPHA, GR_BLEND_ONE,
//			GR_BLEND_ONE_MINUS_SRC_ALPHA,
			GR_BLEND_ONE, GR_BLEND_ZERO);

	grTexCombineFunction (0, GR_TEXTURECOMBINE_DECAL);
	grTexClampMode (0, GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP);

	grDepthBufferMode (GR_DEPTHBUFFER_WBUFFER);
	grDepthBufferFunction (GR_CMP_LESS);
	grDepthMask (FXTRUE);

	grCullMode (GR_CULL_NEGATIVE);

	mat1[3] = 0.0f;	mat1[7] = 0.0f;	mat1[11] = 0.0f;
	mat2[3] = 0.0f;	mat2[7] = 0.0f;	mat2[11] = 0.0f;
	mat3[3] = 0.0f;	mat3[7] = 0.0f;	mat3[11] = 0.0f;

	rotx (0, mat3);
	roty (0, mat1);
	mxm (mat3, mat1, mat2);
 	rotz (0, mat1);
	mxm (mat2, mat1, mat3);

	mat3[3] = bbx - cambot.x;
	mat3[7] = bby - cambot.y;
	mat3[11] = bbz - cambot.z;
	mxm (cambot.xform, mat3, mat1);

	render_limb (&limb_ball, mat1, &ball_decal, &ball_ratio);
#endif

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Draw ball

	// Set up values per object-owner
	if (PLAYBACK)
	{
		color = ((replay_obj_t *)oi)->color;
		dbias = ((replay_obj_t *)oi)->dbias;
		scale = ((replay_obj_t *)oi)->scale;

		memcpy (&m1, ((replay_obj_t *)oi)->pq, sizeof(MAT4));
	}
	else
	{
		color = ((ball_data *)oi)->color;
		dbias = balldbias;
		scale = ((ball_data *)oi)->scale;

		if (((ball_data *)oi)->flags & BF_WHITE)
			color |= 0xffffff;

		m1.xw = 0;
		m1.yw = 0;
		m1.zw = 0;

		f = ballrotv.x * ballrotv.x + ballrotv.y * ballrotv.y + ballrotv.z * ballrotv.z;
		if (f < .0000001f || game_info.game_state == GS_EOQ ) {
			m1.xx = ballmat.xx;
			m1.xy = ballmat.xy;
			m1.xz = ballmat.xz;
			m1.yx = ballmat.yx;
			m1.yy = ballmat.yy;
			m1.yz = ballmat.yz;
			m1.zx = ballmat.zx;
			m1.zy = ballmat.zy;
			m1.zz = ballmat.zz;
//			printf("Rot 0\n");
		}
		else {
			f = fsqrt (f);
			i = f * -4096*16;
			if (i > 0 && i < 0x20) i = 0x20;
			else
			if (i < 0 && i > -0x20) i = -0x20;
//			printf("Rot %f %d\n", f, i);
			f = 1 / f;
			vc.x = ballrotv.x * f;	// Normalize
			vc.y = ballrotv.y * f;
			vc.z = ballrotv.z * f;
			mat_genvecrot (&vc, i, &m2);
			m2.xw = 0;
			m2.yw = 0;
			m2.zw = 0;

			mxm ((PV) &m2, (PV) &ballmat, (PV) &m1);
			ballmat.xx = m1.xx;
			ballmat.xy = m1.xy;
			ballmat.xz = m1.xz;
			ballmat.yx = m1.yx;
			ballmat.yy = m1.yy;
			ballmat.yz = m1.yz;
			ballmat.zx = m1.zx;
			ballmat.zy = m1.zy;
			ballmat.zz = m1.zz;

//			printf("M2  %f %f %f %f\n", m2.xx, m2.xy, m2.xz, m2.xw);
//			printf("    %f %f %f %f\n", m2.yx, m2.yy, m2.yz, m2.yw);
//			printf("    %f %f %f %f\n", m2.zx, m2.zy, m2.zz, m2.zw);
//
//			printf("M1  %f %f %f %f\n", m1.xx, m1.xy, m1.xz, m1.xw);
//			printf("    %f %f %f %f\n", m1.yx, m1.yy, m1.yz, m1.yw);
//			printf("    %f %f %f %f\n", m1.zx, m1.zy, m1.zz, m1.zw);
		}
	}

	grColorCombine (
			GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL,
			GR_COMBINE_FACTOR_ONE,
			GR_COMBINE_LOCAL_CONSTANT,
			GR_COMBINE_OTHER_TEXTURE,
			0);

	grChromakeyMode (GR_CHROMAKEY_DISABLE);

	grConstantColorValue (color);
//	grConstantColorValue (((ball_data *)oi)->alpha << 24 | color);

//	guAlphaSource (GR_ALPHASOURCE_TEXTURE_ALPHA);
	guAlphaSource (GR_ALPHASOURCE_CC_ALPHA);
	grAlphaBlendFunction (
			GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA,
			GR_BLEND_ONE,
			GR_BLEND_ZERO);

	grTexCombineFunction (0, GR_TEXTURECOMBINE_DECAL);
//	grTexClampMode (0, GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP);

	grDepthBufferMode (GR_DEPTHBUFFER_WBUFFER);
	grDepthBufferFunction (GR_CMP_LESS);
	grDepthMask (FXTRUE);

	grDepthBiasLevel (dbias);

#ifdef	DEBUG
	if (get_p5_sw_current() & P_B) {
		grDepthBiasLevel (0);
//		printf ("No Bias\n");
	}
#endif

	grCullMode (GR_CULL_NEGATIVE);

//	rotx (-o_p->fwd2, mat3);
//	roty (0, mat1);
//	mxm (mat3, mat1, mat2);
//	rotz (0, mat1);
//	mxm (mat2, mat1, mat3);

	m1.xx *= scale;
	m1.xy *= scale;
	m1.xz *= scale;
	m1.yx *= scale;
	m1.yy *= scale;
	m1.yz *= scale;
	m1.zx *= scale;
	m1.zy *= scale;
	m1.zz *= scale;

	m1.xw = ((obj_3d *)oi)->x - cambot.x;
	m1.yw = ((obj_3d *)oi)->y - cambot.y;
	m1.zw = ((obj_3d *)oi)->z - cambot.z;

	mxm (cambot.xform, (PV) &m1, (PV) &m2);

	render_limb (&limb_ball, (PV) &m2, &ball_decal, &ball_ratio);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Draw highlight and shading

#if 1
	if ((PLAYBACK) || !(((ball_data *)oi)->flags & BF_WHITE))
	{
		grConstantColorValue (0x80000000);

		grTexCombineFunction (0, GR_TEXTURECOMBINE_DECAL);
		grTexClampMode (0, GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP);

//	Same as above
//		grDepthBufferMode(GR_DEPTHBUFFER_WBUFFER);
//		grDepthBufferFunction(GR_CMP_LESS);
		grDepthMask (FXFALSE);

//		grChromakeyMode(GR_CHROMAKEY_DISABLE);
//		grCullMode (GR_CULL_NEGATIVE);

//		guColorCombineFunction (GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
		grColorCombine (GR_COMBINE_FUNCTION_LOCAL, GR_COMBINE_FACTOR_LOCAL,
				GR_COMBINE_LOCAL_CONSTANT, GR_COMBINE_OTHER_TEXTURE, 0);

		guAlphaSource (GR_ALPHASOURCE_TEXTURE_ALPHA_TIMES_CONSTANT_ALPHA);

		grAlphaTestFunction (GR_CMP_GEQUAL);
		grAlphaTestReferenceValue (0x10);

		grDepthBiasLevel (dbias - 10);

		mat1[3] = 0.0f;	mat1[7] = 0.0f;	mat1[11] = 0.0f;
//		mat2[3] = 0.0f;	mat2[7] = 0.0f;	mat2[11] = 0.0f;
		mat3[3] = 0.0f;	mat3[7] = 0.0f;	mat3[11] = 0.0f;

		rotx (0, mat3);
//		roty (((int)(RD2GR(cambot.theta) + 512) & 1023), mat1);

		// Set roty per current viewpoint
		f = m1.xw * m1.xw + m1.zw * m1.zw;
		if (f < 0.1f) f = 0.1f;
		f = rsqrt(f);
		mat1[0]  = -m1.zw * f;
		mat1[1]  =  0.0f;
		mat1[2]  = -m1.xw * f;
		mat1[4]  =  0.0f;
		mat1[5]  =  1.0f;
		mat1[6]  =  0.0f;
		mat1[8]  =  m1.xw * f;
		mat1[9]  =  0.0f;
		mat1[10] = -m1.zw * f;

		mxm (mat3, mat1, mat2);
 		rotz (0, mat1);
		mxm (mat2, mat1, mat3);

		mat3[3 ] = ((obj_3d *)oi)->x - cambot.x;
		mat3[7 ] = ((obj_3d *)oi)->y - cambot.y;
		mat3[11] = ((obj_3d *)oi)->z - cambot.z;

		mxm (cambot.xform, mat3, mat1);

		grAlphaBlendFunction (GR_BLEND_SRC_ALPHA, GR_BLEND_ONE_MINUS_SRC_ALPHA,
				GR_BLEND_ONE, GR_BLEND_ZERO);

		render_limb (&limb_ballshade, mat1, &ballshade_decal, &ballshade_ratio);


#ifdef	DEBUG
	if (get_p5_sw_current() & P_C) goto nospec;
#endif

		grConstantColorValue (0xffffffff);
		grColorCombine (GR_COMBINE_FUNCTION_SCALE_OTHER, GR_COMBINE_FACTOR_LOCAL,
				GR_COMBINE_LOCAL_CONSTANT, GR_COMBINE_OTHER_TEXTURE, 0);

		guAlphaSource (GR_ALPHASOURCE_CC_ALPHA);
		grAlphaBlendFunction (GR_BLEND_SRC_ALPHA, GR_BLEND_ONE,
				GR_BLEND_ONE, GR_BLEND_ZERO);

		render_limb (&limb_ballspec, mat1, &ballhigh_decal, &ballhigh_ratio);
nospec:
	}
#endif

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Restore

	grDepthBiasLevel (0);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#ifdef DEBUG_HEADS
	for(i = 0; i < NUM_PLAYERS; i++)
	{
		player_blocks[i].odata.fwd += 1;
		player_blocks[i].odata.fwd = player_blocks[i].odata.fwd & 1023;
		player_blocks[i].odata.tgt_fwd = player_blocks[i].odata.fwd;	
	}
#endif

}

/********************************
********************************/

void	flash_ball_proc (int *args)
{
	int i, cnt;

	// extract cnt from process args
	cnt = (int)args[0] + 1;

//	if (game_info.game_state != GS_ATTRACT_MODE)
//	{
		for (i = 0; i < cnt; i++)
		{
			if (game_info.game_state == GS_EOQ) break;
			ball_obj.flags |= BF_WHITE;
			sleep(3);
			ball_obj.flags &= ~BF_WHITE;
			sleep(3);
		}
//	}
	ball_obj.flags &= ~BF_WHITE;
}	

/********************************
********************************/

void	flash_ball (int cnt)
{
	qcreate ("BallFlash", 0, flash_ball_proc, cnt, 0, 0, 0);
#ifdef DEBUG
	printf ("Flash ball started!\n");
#endif
}

/********************************
 Make a ghost trail object behind ball
********************************/

#if	1
void	ball_trail()
{
	ball_data	*bd_p;
	PTL		*pt_p;

	int		i;
	float		f;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	bd_p = &ball_obj;

	if ((pt_p = ptl_get (0))) {

		pt_p->flgs = M_PTLF_TEX;
		pt_p->x = bd_p->odata.x;
		pt_p->y = bd_p->odata.y;
		pt_p->z = bd_p->odata.z;
//		f = (28 + rndrng0 (7)) * .01f;
		f = .7f;
		pt_p->vx = bd_p->odata.vx * f;
		pt_p->vy = bd_p->odata.vy * f;
		pt_p->vz = bd_p->odata.vz * f;
		pt_p->ay = BALL_GRAVITY * .9f;
		pt_p->drag = .99f;
		i = 12;	// 10 + rndrng0 (1);
		pt_p->fuel = i;
		pt_p->rad = BALL_RADIUS * .99f;
		pt_p->radv = -BALL_RADIUS * .9f / 100;

 		pt_p->img_p = PTLIMG(PTLI_BALLBLUR);
		pt_p->frm = 0;

		pt_p->alpha = 180;
		pt_p->alphav = -150.0f / i;
		pt_p->color = 0xff3f3f;

		pt_p->func_p = (PV) ptlf_trail;
	}

}

/********************************
 Ball trail particle function
********************************/

PTLFRET	ptlf_trail (PTL *pt_p)
{
	if ((game_info.ball_mode != BM_PASSING) &&
//		((game_info.ball_handler != NO_BALLHANDLER) && !(player_blocks[game_info.ball_handler].odata.flags & (PF_DUNKER|PF_ALLYDUNKER))))
		// Want ball trails to go away as soon as ally dunker catches ball
		((game_info.ball_handler != NO_BALLHANDLER) && !(player_blocks[game_info.ball_handler].odata.flags & (PF_DUNKER))))
	{
		pt_p->fuel = 0;
//		pt_p->drag = .6f;
	}
}

/********************************
 Old
********************************/

#else
void	ball_trail_proc (int *parg);

void	ball_trail()
{
	set_process_run_level ( qcreate( "balltrl", BALL_PID, ball_trail_proc, 0, 0, 0, 0 ), RL_BALL+1 );
}


void	ball_trail_proc (int *parg)
{
	ball_data	bobj;
	struct object_node *myball;

	bobj = ball_obj;
//	bobj.flags = BF_ALT;
	bobj.color = 155 << 24;
//	bobj.alpha = 155;
	bobj.scale = 0.90f;

	myball = create_object( "ball", OID_BALL, OF_NONE, DRAWORDER_BALL, (void *)&bobj, draw_ball);
	do {
		sleep( 1 );
	} while (halt);

	bobj.color = 100 << 24;
	bobj.scale = 0.80f;
	sleep( 1 );
	bobj.color = 85 << 24;
	bobj.scale = 0.65f;
	sleep( 1 );
	bobj.color = 70 << 24;
	bobj.scale = 0.50f;
	sleep( 1 );
	bobj.color = 55 << 24;
	bobj.scale = 0.30f;
	sleep( 1 );
	delete_object( myball );
	bobj.color = 255 << 24;
	bobj.scale = 1.0f;
}
#endif



/****************************************************************/

/********************************
 Set ball on fire if necessary
 (mode (0=plyr, 1=team, 2=hotspot), fire ticks, spiral ticks or 0)
********************************/

void	ball_fireset (int mode, int t, int t2)
{
	int		i;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

//	printf ("FIRE %d %d\n", t, t2);

	ballfiremode = mode;

	i = ballfiretime;
	if (t > i)
		ballfiretime = t;

	if (i <= 0) {		// !Active?

		set_process_run_level (qcreate ("ballfire", PLAYER_PID, ball_fire_proc, 0, 0, 0, 0), RL_BALL+5);
	}

	if (t2 > 0)
		set_process_run_level (qcreate ("ballfiresp", PLAYER_PID, ball_firespin_proc, t2, 0, 0, 0), RL_BALL+6);
}

/*******************************
 Create ball fire and smoke
 (Process *) *[0] = Spin duration, *[1] = Mode
********************************/

int smkc_t[] = { 0xe0e0e0, 0xd0e0e0, 0xe0d0e0, 0xe0e0d0 };
float bsmkspd_t[] = {
	0.0f, 1.6f, 0.8f, 2.4f, 0.4f, 2.0f, 1.2f, 2.8f,
	0.2f, 1.8f, 1.0f, 2.6f, 0.6f, 2.2f, 1.4f, 3.0f
};
int	hspotballcol_t[] = { 0xff0000f0, 0xff00c020, 0xffe00020, 0xff808020 };

void ball_fire_proc (int *pa_p)
{
	game_info_t	*gi_p = &game_info;
	ball_data	*bd_p = &ball_obj;
	int		tmfr;			// !0 = Team fire
	int		basecolor;
	PTL		*pt_p;

	VEC		bv;				// Ball velocity
	float	bspd;			// Ball speed

//	int		sprkdly = 5;
	int		smki = 0;		// Smoke index

	int		n, i, i2, i3;
	float	f, fx, fy, fz, fx2, fy2, fz2;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	tmfr = 0;
	basecolor = 0xffff2010;
	if (ballfiremode == 1) {
		tmfr = 1;
		basecolor = 0xff0020ff;
	}
	else if (ballfiremode >= 0x10) {
		basecolor = hspotballcol_t[ballfiremode & 0xf];
	}


	do {
		bd_p->color = basecolor + (rndrng0 (0x3f) << 8);

		bv.x = bd_p->odata.vx;
		bv.y = bd_p->odata.vy;
		bv.z = bd_p->odata.vz;

		bspd = bv.x * bv.x + bv.y * bv.y + bv.z * bv.z;
		if (bspd > .0000001f) bspd = fsqrt (bspd);

//		printf ("V %f\n", bspd);

//	//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Spark
//
//		if (--sprkdly < 0) {
//
//			sprkdly = 3 + rndrng0 (10);
//
//			if ((pt_p = ptl_get(NULL))) {
//
//				pt_p->flgs = M_PTLF_BLENDA;
//				pt_p->x = bd_p->odata.x;
//				pt_p->y = bd_p->odata.y;
//				pt_p->z = bd_p->odata.z;
//				pt_p->vx = bd_p->odata.vx + (-20 + rndrng0 (40)) * .01f;
//				pt_p->vy = bd_p->odata.vy + (-20 + rndrng0 (40)) * .01f;
//				pt_p->vz = bd_p->odata.vz + (-20 + rndrng0 (40)) * .01f;
//				pt_p->drag = .98f;
//				i = 50 + rndrng0 (50);
//				pt_p->fuel = i;
//				pt_p->rad = (30 + rndrng0 (20)) * .01f;
//				pt_p->radv = -.001f;
//				pt_p->txn_p = smkpuf_decals[0];
//				pt_p->alpha = 240;
//				pt_p->alphav = -240.0f / 2 / i;
//				pt_p->color = sprkfc_t[rndrng0 (5)];
//			}
//		}

	//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Fire

#if	1
		n = 4;
		do {
			if ((pt_p = ptl_get(NULL))) {

				pt_p->flgs = M_PTLF_BLENDA;
				pt_p->x = bd_p->odata.x + (-50 + rndrng0 (100)) * .01f * BALL_RADIUS;
				pt_p->y = bd_p->odata.y + (-50 + rndrng0 (100)) * .01f * BALL_RADIUS;
				pt_p->z = bd_p->odata.z + (-50 + rndrng0 (100)) * .01f * BALL_RADIUS;
				f = (28 + rndrng0 (7)) * .01f;
				pt_p->vx = bd_p->odata.vx * f;
				pt_p->vy = bd_p->odata.vy * f;
				pt_p->vz = bd_p->odata.vz * f;
				pt_p->ay = PTL_GRAVITY * -.5f;
				pt_p->drag = .95f;
				i = 22 + rndrng0 (3);
				pt_p->fuel = i;
				pt_p->rad = 2.1f;
				pt_p->radv = -.06f;

 				pt_p->img_p = PTLIMG(PTLI_X1);
				pt_p->frm = 1;

				pt_p->alpha = 200;
				pt_p->alphav = -200.0f / i;
				i = (basecolor & 0xff00ff) + (rndrng0 (0x7f) << 8);
				pt_p->color = i;
			}
		} while (--n > 0);
#endif

	//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Smoke

#if	1
		f = bsmkspd_t[++smki & 0xf];

		if (bspd >= f) {		// Fast enough? Make puff

			if ((pt_p = ptl_get(NULL))) {

				pt_p->flgs = 0;
				pt_p->x = bd_p->odata.x + (-30 + rndrng0 (60)) * .01f * BALL_RADIUS - bv.x * 3;
				pt_p->y = bd_p->odata.y + (-30 + rndrng0 (60)) * .01f * BALL_RADIUS - bv.y * 3;
				pt_p->z = bd_p->odata.z + (-30 + rndrng0 (60)) * .01f * BALL_RADIUS - bv.z * 3;
				pt_p->vx = bv.x * .5f;
				pt_p->vy = bv.y * .5f;
				pt_p->vz = bv.z * .5f;
				pt_p->ay = PTL_GRAVITY * -.05f;
				pt_p->drag = .95f;
				i = 80 + rndrng0 (40);
				pt_p->fuel = i;
				pt_p->rad = 4.0f;
				pt_p->radv = .08f;

//				pt_p->txn_p = smkpuf_decals[0];
//				i2 = rndrng0 (NUM_FIRE_TXTRS - 1);

	 			pt_p->img_p = PTLIMG(PTLI_X1);
				pt_p->frm = 0;
				i2 = rndrng0 (PTLIF_X1 - 1);

				pt_p->data2.i = 0;
				pt_p->data3.i = (i2 << 16) / i;

				pt_p->alpha = 110;
				pt_p->alphav = -110.0f / i;
//				pt_p->color = 0;

				pt_p->func_p =	(PV) ptlf_smk;
				pt_p->data1.i = 5;	// Color #
			}
		}
#endif
		sleep (1);

#if	NUM_PERTEAM > 2
#error FIX SHIFT BELOW!!!
#endif
		// Kill fire if ball owned by a non-fire person
		i = gi_p->ball_handler;

		if (i >= 0)
		{
			if ( !(gi_p->fire_player == i && gi_p->fire_cnt >= FIRECNT) &&
				 !(gi_p->team_fire == player_blocks[i].team))
				 // shawn, team_fire isnt a bit field its a 0 or 1 based on team not player -- jeff
				 //(gi_p->team_fire != i >> 1) )
			{
				ballfiretime = 0;
#ifdef	DEBUG
				printf ("Ball fire canceled!\n");
#endif
			}
		}


	} while (--ballfiretime > 0);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	bd_p->color = 0xff000000;
}


/********************************
 Smoke particle function
********************************/

PTLFRET	ptlf_smk (PTL *pt_p)
{
	int		col_t[] = { 0xf0f040, 0xf0f080, 0xf0f0a0, 0xf0e0c0, 0xe0e0c0 };
	int		i;

	i = --pt_p->data1.i;

	if (i >= 0) {

		pt_p->color = col_t[4 - i];
	}
	else {

		if (i == -1)
			pt_p->color = smkc_t[rndrng0 (3)];
//		pt_p->func_p =	0;
	}

	i = pt_p->data2.i;
	pt_p->data2.i = i + pt_p->data3.i;	// Step

	pt_p->frm = i >> 16;

}

/*******************************
 Create ball fire spiral
 (Process *) *[0] = Spin duration, *[1] = Mode
********************************/

int	spinc_t[] = { 0xe08000, 0xd0c000, 0xc0c060 };
//int	spinc_t[] = { 0xf00000, 0xf08000, 0xc0c000 }; old color
int	spinc2_t[] = { 0x909090, 0x0000ff, 0x00efef, 0x00ef00, 0xef0000, 0xef00ef };

#define	SPIRAL_DRAG	.97f

typedef	struct	_PTLSPIN {
	int	ang;			// Angle
	int	angv;			// Angular velocity
	int	color;
} PTLSPIN;

void	ball_firespin_proc (int *pa_p)
{
	ball_data	*bd_p;
	int		tmfr;			// !0 = Team fire
	VEC		bv;				// Ball velocity
	float		bspd;				// Ball speed
	PTL		*pt_p;

	int		spinfuel;
	int		spincnt;			// # spirals
	PTLSPIN	spin_t[5];

	MAT4		m;
	VEC		vc;
	VEC		vc2;

	int		n, n2, n3, i, i2, i3;
	float		f, fx, fy, fz, fx2, fy2, fz2;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	bd_p = &ball_obj;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Init spiral

	tmfr = 0;
	if (ballfiremode) tmfr = 1;


	spinfuel = pa_p[0];
	spincnt = 1 + rndrng0 (3);

	if (tmfr) spincnt = 4;

	n = 0;
	n2 = 0x400 + rndrng0 (0x1400);

	if (spincnt < 2) n2 += 0xc00;		// Faster spin for 1 spiral

	if (rand() & 1) n2 = -n2;
	i2 = 0;
	i3 = 0x10000 / spincnt;
	do {
		spin_t[n].ang = i2;
		if ((rand() & 3) == 0) n2 = -n2;
		spin_t[n].angv = n2;

		if (n & 1)
			i = spinc2_t[rndrng0 (5)];		// Rainbow
		else
			i = spinc_t[rndrng0 (2)];		// Natural

		if (tmfr) i = 0x30ff;

		spin_t[n].color = i;

		i2 += i3;
	} while (++n < spincnt);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	n = pa_p[0];

//	n = pa_p[0] + 30;
//	if (pa_p[1] == 0) n += 90;

	do {

		bv.x = bd_p->odata.vx;
		bv.y = bd_p->odata.vy;
		bv.z = bd_p->odata.vz;

		bspd = bv.x * bv.x + bv.y * bv.y + bv.z * bv.z;
		if (bspd > .0000001f) bspd = fsqrt (bspd);

//		printf ("V %f\n", bspd);

	//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Spiral emitter about velocity

#if	1
		if (spinfuel > 0) {
			spinfuel--;

			vc.x = bv.x;
			vc.y = bv.y;
			vc.z = bv.z;

			f = bspd;
			if (f < .0000001f) f = .0000001f;
			f = 1 / f;
			vc.x = vc.x * f;		// Make unit vector
			vc.y = vc.y * f;
			vc.z = vc.z * f;
//			printf ("V %f %f %f\n", vc.x, vc.y, vc.z);

			vc2.x = 1 * vc.z - 0 * vc.y;	// Cross prod 0,1,0 vec & velocity
//			vc2.y = 0 * vc.x - 0 * vc.z;
			vc2.z = 0 * vc.y - 1 * vc.x;

			f = vc2.x * vc2.x + vc2.z * vc2.z;
			if (f < .0000001f) f = .0000001f;
			f = 1 / fsqrt (f);
			vc2.x = vc2.x * f;		// Make unit vector
//			vc2.y = vc2.y * f;
			vc2.z = vc2.z * f;

			vc2.x += vc.x * 2;	// Unit scaled + cross prod unit result
			vc2.y = vc.y * 2;
			vc2.z += vc.z * 2;
			vc2.x *= .3f;			// Scale total for final velocity
			vc2.y *= .3f;
			vc2.z *= .3f;

			n2 = spincnt;
			do {

				i2 = spincnt - n2;

				n3 = 1;
				do {
					if ((pt_p = ptl_get(NULL))) {

						i = spin_t[i2].ang;
						spin_t[i2].ang += spin_t[i2].angv;
						mat_genvecrot (&vc, i, &m);

						fx = vc2.x * m.xx + vc2.y * m.xy + vc2.z * m.xz;	// Rotate
						fy = vc2.x * m.yx + vc2.y * m.yy + vc2.z * m.yz;
						fz = vc2.x * m.zx + vc2.y * m.zy + vc2.z * m.zz;

						pt_p->flgs = M_PTLF_BLENDA;
//						pt_p->flgs = 0;
						pt_p->x = bd_p->odata.x - fx * 3;
						pt_p->y = bd_p->odata.y - fy * 3;
						pt_p->z = bd_p->odata.z - fz * 3;
						pt_p->vx = bd_p->odata.vx - fx;
						pt_p->vy = bd_p->odata.vy - fy;
						pt_p->vz = bd_p->odata.vz - fz;
						pt_p->ay = PTL_GRAVITY * -.05f;
						pt_p->ay = 0;
						pt_p->drag = SPIRAL_DRAG;
						i = 50 + rndrng0 (20);
						pt_p->fuel = i;
						pt_p->rad = 3.0f;
						pt_p->radv = -3.0f / i;

						pt_p->img_p = PTLIMG(PTLI_SPRKA);
						pt_p->frm = 0;

						pt_p->alpha = 250;
						pt_p->alphav = -250.0f / 2 / i;

						i3 = spin_t[i2].color;
//						pt_p->color = i3;
						pt_p->data1.i = i3;

						pt_p->data2.i = 0;
						pt_p->data3.i = (PTLIF_SPRKA << 16) / i;
						pt_p->func_p =	(PV) ptlf_spiral;
					}
				} while (--n3 > 0);

			} while (--n2 > 0);
		}
#endif

		sleep (1);

	} while (--n > 0);


}


/********************************
 Spiral particle function
********************************/

PTLFRET	ptlf_spiral (PTL *pt_p)
{
	int		i;

	if (pt_p->fuel > 40) {

		pt_p->color = pt_p->data1.i;
		if ((rand() & 3) == 0)
			pt_p->color = 0xffffff;
	}
	else {
		pt_p->color = 0x8080c0;
	}

	i = pt_p->data2.i;
	pt_p->data2.i = i + pt_p->data3.i;	// Step for next frame

	pt_p->frm = i >> 16;

}

/****************************************************************/

void	ball_spark_proc (int *pa_p);

/********************************
 Create ball sparks
 (mode: 0=Rim hit 1=Steal 2=Block, ticks duration, # sparks per tick)
********************************/

void	ball_sparkcreate (int mode, int dur, int cnt)
{
	int		i;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#ifdef	DEBUG
	printf ("Sparks: %d %d %d\n", mode, dur, cnt);
#endif
	set_process_run_level (qcreate ("ballspark", PLAYER_PID, ball_spark_proc, mode, dur, cnt, 0), RL_BALL+5);
}


/*******************************
 Create ball sparks
 (Process *) [0] mode (see above), [1] duration, [2] # sparks
********************************/

//int	smkc_t[] = { 0xe0e0e0, 0xd0e0e0, 0xe0d0e0, 0xe0e0d0 };
//float	bsmkspd_t[] = {  0, 1.6f,  .8f, 2.4f, .4f, 2.0f, 1.2f, 2.8f,
//							.2f, 1.8f, 1.0f, 2.6f, .6f, 2.2f, 1.4f, 3.0f };

void	ball_spark_proc (int *pa_p)
{
	game_info_t	*gi_p;
	ball_data	*bd_p;
	PTL		*pt_p;

	int		n;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	gi_p = &game_info;
	bd_p = &ball_obj;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	n = pa_p[1];

	do {

		ball_spark (pa_p[0], pa_p[2]);
		sleep (1);

	} while (--n > 0);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

//	bd_p->color = 0xff000000;

}



/********************************
 Create ball sparks
 (mode 0=Rim hit; 1=Steal; 2=Block, # sparks)
********************************/

void	ball_spark (int mode, int cnt)
{
	ball_data	*bd_p;
	PTL		*pt_p;
	float		vel;
	int		cmask;

	int		n, i, i2;
	float		f, fx, fy, fz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	bd_p = &ball_obj;

	cmask = 0xffffff;
	vel = 70;
	if (mode == 1) {
		cmask = 0x7f7fff;
		vel = 30;
	}
	else if (mode == 2) {
		cmask = 0xffff00;
		vel = 30;
	}
	n = cnt;

	do {

		if ((pt_p = ptl_get(NULL))) {

			pt_p->flgs = M_PTLF_BLENDA;
			pt_p->x = bd_p->odata.x;
			pt_p->y = bd_p->odata.y;
			pt_p->z = bd_p->odata.z;
			pt_p->vx = bd_p->odata.vx + (-vel + rndrng0 (vel * 2)) * .01f;
			pt_p->vy = bd_p->odata.vy + (-vel + rndrng0 (vel * 2)) * .01f;
			pt_p->vz = bd_p->odata.vz + (-vel + rndrng0 (vel * 2)) * .01f;
			pt_p->drag = .98f;

			if (mode == 0) {
				if (rand() & 3)
					i = 5 + rndrng0 (50);
				else
					i = 50 + rndrng0 (150);
			}
			else {
				i = 30 + rndrng0 (20);
			}
			pt_p->fuel = i;

			pt_p->rad = (50 + rndrng0 (30)) * .01f;
			pt_p->radv = -.001f;

//			pt_p->txn_p = smkpuf_decals[0];
 			pt_p->img_p = PTLIMG(PTLI_X1);

			pt_p->alpha = 250;
			pt_p->alphav = -250.0f / 3 / i;
//			pt_p->color = sprkc_t[rndrng0 (5)];

			pt_p->func_p =	(PV) ptlf_spark;
			pt_p->data1.i = 0;
			pt_p->data2.i = 0x80 + rndrng0 (0x200);	// .5 to 2.5 steps
			pt_p->data3.i = cmask;
		}

	} while (--n > 0);

}


/********************************
 Spark particle function
********************************/

PTLFRET	ptlf_spark (PTL *pt_p)
{
	int		col_t[] = { 0x101010, 0x303030, 0x505050, 0x707070,
								0x909090, 0xb0b0b0, 0xd0d0d0, 0xf0f0f0,
								0xd0d0d0, 0xb0b0b0, 0x909090, 0x707070,
								0x505050, 0x303030, 0x101010, 0, 0, 0 };
	int		i, i2;

	i = pt_p->data1.i;
	i2 = col_t[i >> 8];

	if (i2 == 0) {			// End?
		i = 0;
		i2 = col_t[0];
	}
	pt_p->data1.i = i + pt_p->data2.i;

	i2 &= pt_p->data3.i;	// Mask
	pt_p->color = i2;

}

/********************************
 Create hotspot fire effect
 (Process *) *[0] = Duration, *[1] = Plyr #
********************************/

int	hspotcol_t[] = { 0x2020e0, 0x20e020, 0xe02020, 0xe0e020 };

void	hotspot_fire_proc (int *pa_p)
{
	bbplyr_data	*pl_p = &player_blocks[pa_p[1]];
//	ball_data	*bd_p;

	float		rcx,rcz,rcy;

//	VEC			bv;					// Ball velocity
//	float		bspd;				// Ball speed
	PTL			*pt_p;

	int			spinfuel;
	int			spincnt;			// # spirals
	PTLSPIN		spin_t[5];

	MAT4		m;
	VEC			vc;
	VEC			vc2;

	int			xfuel;
	int			xfuel2;
	int			n, n2, n3, i, i2, i3;
	float		f, fx, fy, fz, fx2, fy2, fz2;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

//	bd_p = &ball_obj;

	i = pl_p->p_spd->hotspot;
	rcx = hotspot_xzs[i][0];
	if (pl_p->odata.x > 0) {
		rcx = (float) ABS(hotspot_xzs[i][0]);
	}
	rcz = hotspot_xzs[i][1];
	rcy = BALL_RADIUS;

// TEMP!!
	rcx = pl_p->odata.x;
	rcz = pl_p->odata.z;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Init spiral

	spinfuel = pa_p[0];
	spincnt = 2 + rndrng0 (2);

	n = 0;
	i = 0x700 + rndrng0 (0x1000);
//	i = 0x800 + rndrng0 (0x1000);
	if (rand() & 1) i = -i;
	i2 = 0;
	i3 = 0x10000 / spincnt;
	do {
		spin_t[n].ang = i2;
		if ((rand() & 3) == 0) i = -i;
		spin_t[n].angv = i;
		spin_t[n].color = spinc_t[rndrng0 (5)];
		i2 += i3;
	} while (++n < spincnt);

	xfuel = 2;
	xfuel2 = 9;

	n = tsec * 1 + 10;

	do {

	//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Ring

#if	0
		if (xfuel > 0) {		// Thin whitish ring
			xfuel--;

			n2 = 30;
			do {
				if ((pt_p = ptl_get (0))) {

					pt_p->flgs = M_PTLF_BLENDA | M_PTLF_CLIPQ;
					pt_p->x = rcx + (-30 + rndrng0 (60)) * .01f * HOTSPOT_RADIUS;
					pt_p->y = rcy;	// + (-5 + rndrng0 (10)) * .01f * HOTSPOT_RADIUS;
					pt_p->z = rcz + (-5 + rndrng0 (10)) * .01f * HOTSPOT_RADIUS;
					i = rand();
					pt_p->vx = isin (i) * 25.0f * -PTL_GRAVITY;
					pt_p->vz = icos (i) * 25.0f * -PTL_GRAVITY;
					pt_p->vy = .2f;	//(30 + rndrng0 (0)) * .01f;
					pt_p->ay = PTL_GRAVITY * .5f;
					pt_p->drag = .97f;
					i = 29 + rndrng0 (5);
					pt_p->fuel = i;
					pt_p->rad = 1.5f;
					f = -.005f;
					pt_p->radv = f;		//-.07f;

					pt_p->img_p = PTLIMG(PTLI_X1);
					pt_p->frm = 1;

					pt_p->alpha = 150;
					pt_p->alphav = -150.0f / i;
					i = 0xffa0ff + (rndrng0 (0x5f) << 8);
					pt_p->color = i;

//					pt_p->func_p = (PV) hotspot_ptlf_ring;
//					pt_p->data1.i = 99;	// Delay
				}
			} while (--n2 > 0);
		}
#endif

#if 1
		if (xfuel2 > 0) {		// Thick fire ring
			xfuel2--;

			n2 = 30;
			do {
				if ((pt_p = ptl_get (0))) {

					pt_p->flgs = M_PTLF_BLENDA | M_PTLF_CLIPQ;

					pt_p->x = rcx + (-10 + rndrng0 (20)) * .01f * HOTSPOT_RADIUS;
					pt_p->y = rcy; // + (-15 + rndrng0 (30)) * .01f * HOTSPOT_RADIUS;
					pt_p->z = rcz + (-10 + rndrng0 (20)) * .01f * HOTSPOT_RADIUS;
					i = rand();
					pt_p->vx = isin (i) * 19.0f * -PTL_GRAVITY;
					pt_p->vz = icos (i) * 19.0f * -PTL_GRAVITY;
					pt_p->vy = .3f;	//(20 + rndrng0 (20)) * .01f;
					pt_p->ay = -PTL_GRAVITY * .5f;
					pt_p->drag = .97f;
					i = 40 + rndrng0 (18);
					pt_p->fuel = i;
					pt_p->rad = 2.9f;
					f = -.03f;
					if (n2 & 1) f = -.07f;
					pt_p->radv = f;		//-.07f;

					pt_p->img_p = PTLIMG(PTLI_X1);
					pt_p->frm = 1;

					pt_p->alpha = 190;
					pt_p->alphav = -190.0f / i;
					i = 0xff1000 + (rndrng0 (0x7f) << 8);
					pt_p->color = i;

//					pt_p->func_p = (PV) hotspot_ptlf_ring;
//					pt_p->data1.i = 99;	// Delay
				}
			} while (--n2 > 0);

			n2 = 2;		// Colored smoke
			do {
				if ((pt_p = ptl_get (0))) {

					pt_p->flgs = 0;
					pt_p->x = rcx + (-100 + rndrng0 (200)) * .01f * BALL_RADIUS;
					pt_p->y = rcy;	// + (-100 + rndrng0 (200)) * .01f * BALL_RADIUS;
					pt_p->z = rcz + (-100 + rndrng0 (200)) * .01f * BALL_RADIUS;
					i = rand();
					pt_p->vx = isin (i) * 15.0f * -PTL_GRAVITY;
					pt_p->vz = icos (i) * 15.0f * -PTL_GRAVITY;
					pt_p->vy = .1f;
					pt_p->ay = -PTL_GRAVITY * .01f;
					pt_p->drag = .97f;
					i = 70 + rndrng0 (40);
					pt_p->fuel = i;
					pt_p->rad = 4.0f;
					pt_p->radv = .08f;

		 			pt_p->img_p = PTLIMG(PTLI_X1);
					pt_p->frm = 0;
					i2 = rndrng0 (PTLIF_X1 - 1);

					pt_p->data2.i = 0;
					pt_p->data3.i = (i2 << 16) / i;

					pt_p->alpha = 100;
					pt_p->alphav = -100.0f / i;
					i = hspotcol_t[pa_p[1]];
					pt_p->color = i;

					pt_p->func_p = (PV) hotspot_ptlf_ring;
//					pt_p->data1.i = 5;	// Color #
				}
			} while (--n2 > 0);

		}
#endif

	//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Fire on floor

#if	0
		n2 = 3;
		do {
			if ((pt_p = ptl_get (0))) {

				pt_p->flgs = M_PTLF_BLENDA | M_PTLF_CLIPQ;
				pt_p->x = rcx + (-100 + rndrng0 (200)) * .01f * BALL_RADIUS;
				pt_p->y = rcy + (-100 + rndrng0 (200)) * .01f * BALL_RADIUS;
				pt_p->z = rcz + (-100 + rndrng0 (200)) * .01f * BALL_RADIUS;
				pt_p->vx = 0;
				pt_p->vy = 0;
				pt_p->vz = 0;
				pt_p->ay = PTL_GRAVITY * -.5f;
				pt_p->drag = .95f;
				i = 30 + rndrng0 (5);
				pt_p->fuel = i;
				pt_p->rad = 2.8f;
				pt_p->radv = -.07f;

 				pt_p->img_p = PTLIMG(PTLI_X1);
				pt_p->frm = 1;

				pt_p->alpha = 200;
				pt_p->alphav = -200.0f / i;
				i = 0xff0000 + (rndrng0 (0x7f) << 8);
				pt_p->color = i;
			}
		} while (--n2 > 0);
#endif

	//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Smoke on floor

#if	0
		if ((n & 7) == 0) {
			if ((pt_p = ptl_get (0))) {

				pt_p->flgs = 0;
				pt_p->x = rcx + (-100 + rndrng0 (200)) * .01f * BALL_RADIUS;
				pt_p->y = rcy + (-100 + rndrng0 (200)) * .01f * BALL_RADIUS;
				pt_p->z = rcz + (-100 + rndrng0 (200)) * .01f * BALL_RADIUS;
				pt_p->vx = 0;
				pt_p->vy = 0;
				pt_p->vz = 0;
				pt_p->ay = PTL_GRAVITY * -.15f;
				pt_p->drag = .95f;
				i = 80 + rndrng0 (40);
				pt_p->fuel = i;
				pt_p->rad = 4.0f;
				pt_p->radv = .08f;

	 			pt_p->img_p = PTLIMG(PTLI_X1);
				pt_p->frm = 0;
				i2 = rndrng0 (PTLIF_X1 - 1);

				pt_p->data2.i = 0;
				pt_p->data3.i = (i2 << 16) / i;

				pt_p->alpha = 80;
				pt_p->alphav = -80.0f / i;

				pt_p->func_p = (PV) ptlf_smk;
				pt_p->data1.i = 5;	// Color #
			}
		}
#endif

	//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Fire on feet

#if	0
		n2 = 2;
		do {
			if ((pt_p = ptl_get (0))) {

				pt_p->flgs = M_PTLF_BLENDA | M_PTLF_CLIPQ;
				pt_p->x = pl_p->odata.x + (-150 + rndrng0 (300)) * .01f * BALL_RADIUS;
				pt_p->y = pl_p->odata.y + (-30 + rndrng0 (70)) * .01f * BALL_RADIUS;
				pt_p->z = pl_p->odata.z + (-150 + rndrng0 (300)) * .01f * BALL_RADIUS;
				pt_p->vx = 0;
				pt_p->vy = 0;	//pl_p->odata.y * .1f;
				pt_p->vz = 0;
				pt_p->ay = PTL_GRAVITY * -.3f;
				pt_p->drag = .95f;
				i = 25 + rndrng0 (5);
				pt_p->fuel = i;
				pt_p->rad = 2.8f;
				pt_p->radv = -.07f;

 				pt_p->img_p = PTLIMG(PTLI_X1);
				pt_p->frm = 1;

				pt_p->alpha = 200;
				pt_p->alphav = -200.0f / i;
				i = 0xff0000 + (rndrng0 (0x7f) << 8);
				pt_p->color = i;
			}
		} while (--n2 > 0);
#endif

	//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Smoke on feet

#if	0
		if ((n & 0xf) == 0) {
			if ((pt_p = ptl_get (0))) {

				pt_p->flgs = 0;
				pt_p->x = pl_p->odata.x + (-100 + rndrng0 (200)) * .01f * BALL_RADIUS;
				pt_p->y = pl_p->odata.y + (-100 + rndrng0 (200)) * .01f * BALL_RADIUS;
				pt_p->z = pl_p->odata.z + (-100 + rndrng0 (200)) * .01f * BALL_RADIUS;
				pt_p->vx = 0;
				pt_p->vy = 0;	//pl_p->odata.y * .1f;
				pt_p->vz = 0;
				pt_p->ay = PTL_GRAVITY * -.15f;
				pt_p->drag = .95f;
				i = 70 + rndrng0 (40);
				pt_p->fuel = i;
				pt_p->rad = 4.0f;
				pt_p->radv = .08f;

	 			pt_p->img_p = PTLIMG(PTLI_X1);
				pt_p->frm = 0;
				i2 = rndrng0 (PTLIF_X1 - 1);

				pt_p->data2.i = 0;
				pt_p->data3.i = (i2 << 16) / i;

				pt_p->alpha = 80;
				pt_p->alphav = -80.0f / i;

				pt_p->func_p = (PV) ptlf_smk;
				pt_p->data1.i = 5;	// Color #
			}
		}
#endif


		sleep (1);

	} while (--n > 0);
}


/********************************
 Hotspot particle
********************************/

PTLFRET	hotspot_ptlf_ring (PTL *pt_p)
{
	int		i;

	i = pt_p->data2.i;
	pt_p->data2.i = i + pt_p->data3.i;	// Step

	pt_p->frm = i >> 16;

}

/********************************
 Hotspot particle
********************************/

PTLFRET	hotspot_ptlf_ring2 (PTL *pt_p)
{
	int		i;
	float	f;

	if (--pt_p->data1.i <= 0) {
		pt_p->func_p = 0;
		pt_p->vx = -pt_p->vx;
		pt_p->vz = -pt_p->vz;
	}

//	f = .0005f;
//	pt_p->vx += (10 - rndrng0 (20)) * f;
//	pt_p->vz += (10 - rndrng0 (20)) * f;
}


/********************************
 Create net fire and smoke
 (Process *) *[0] = Spin duration, *[1] = Mode
********************************/

//int	spinc_t[] = { 0x909090, 0x0000ff, 0x00efef, 0x00ef00, 0xef0000, 0xef00ef };
//int	smkc_t[] = { 0xe0e0e0, 0xd0e0e0, 0xe0d0e0, 0xe0e0d0 };
//float	bsmkspd_t[] = {  0, 1.6f,  .8f, 2.4f, .4f, 2.0f, 1.2f, 2.8f,
//							.2f, 1.8f, 1.0f, 2.6f, .6f, 2.2f, 1.4f, 3.0f };

#define	NETSPIR_DRAG	.97f

//typedef	struct	_PTLSPIN {
//	int	ang;			// Angle
//	int	angv;			// Angular velocity
//	int	color;
//} PTLSPIN;

void	net_fire_proc (int *pa_p)
{
	ball_data	*bd_p;
	BBDATA	*bbd_p;

	float		rcx;

	VEC		bv;				// Ball velocity
	float		bspd;				// Ball speed
	PTL		*pt_p;

	int		spinfuel;
	int		spincnt;			// # spirals
	PTLSPIN	spin_t[5];

	MAT4		m;
	VEC		vc;
	VEC		vc2;

	int		xfuel;
	int		xfuel2;
//	int		xdly;

	int		n, n2, n3, i, i2, i3;
	float		f, fx, fy, fz, fx2, fy2, fz2;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	bd_p = &ball_obj;

	i = 0;
	rcx = -RIM_CX;

	if (bd_p->odata.x > 0) {
		i = 1;
		rcx = RIM_CX;
	}
	bbd_p = &bbdata[i];

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	bbd_p->mom += 200;		// Add momentum

	if (bbd_p->bendcnt < 1) {
		bbd_p->bend = 80;
		bbd_p->bendcnt = 1;
		set_process_run_level (qcreate ("bbshake", PLAYER_PID, bbshake_proc, i, 0, 0, 0), RL_BALL+2);
	}

// rim hit snd
	make_hit_backboard3_snd (VOLUME6);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Init spiral

	pa_p[0] = 60;

	spinfuel = pa_p[0];
	spincnt = 2 + rndrng0 (2);

	n = 0;
	i = 0x800 + rndrng0 (0x1000);
	if (rand() & 1) i = -i;
	i2 = 0;
	i3 = 0x10000 / spincnt;
	do {
		spin_t[n].ang = i2;
		if ((rand() & 3) == 0) i = -i;
		spin_t[n].angv = i;
		spin_t[n].color = spinc_t[rndrng0 (5)];
		i2 += i3;
	} while (++n < spincnt);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	xfuel = 2;
	xfuel2 = 2;
//	xdly = 1;

	n = pa_p[0] + 30;
	if (pa_p[1] == 0) n += 90;

	do {

		bbd_p->rimcolor = 0xffff2010 + (rndrng0 (0x3f) << 8);

	//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Spiral emitter about velocity

#if	1
		if (spinfuel > 0) {
			spinfuel--;

			vc.x = 0;
			vc.y = 1;
			vc.z = 0;

			vc2.x = 1;
			vc2.y = 0;
			vc2.z = 0;

			n2 = spincnt;
			do {

				i2 = spincnt - n2;

				n3 = 1;
				do {
					if ((pt_p = ptl_get (0))) {

//						i = spin_t[i2].ang;
//						spin_t[i2].ang += spin_t[i2].angv;
						i = rand();
						mat_genvecrot (&vc, i, &m);

						fx = vc2.x * m.xx + vc2.y * m.xy + vc2.z * m.xz;	// Rotate
						fy = vc2.x * m.yx + vc2.y * m.yy + vc2.z * m.yz;
						fz = vc2.x * m.zx + vc2.y * m.zy + vc2.z * m.zz;

						pt_p->flgs = M_PTLF_BLENDA | M_PTLF_CLIPQ;
						pt_p->x = rcx - fx * 3;
						pt_p->y = RIM_CY - fy * 3;
						pt_p->z = RIM_CZ - fz * 3;
						pt_p->vx = fx;
						pt_p->vy = fy + (30 + rndrng0 (80)) * .01f;
						pt_p->vz = fz;
						pt_p->ay = PTL_GRAVITY * .3f;
						pt_p->drag = NETSPIR_DRAG;
						i = 70 + rndrng0 (70);
						pt_p->fuel = i;
						pt_p->rad = 3.0f;
						pt_p->radv = -3.0f / i;

						pt_p->img_p = PTLIMG(PTLI_SPRKA);
						pt_p->frm = 0;

						pt_p->alpha = 250;
						pt_p->alphav = -250.0f / 2 / i;

						i3 = spin_t[i2].color;
//						pt_p->color = i3;
						pt_p->data1.i = i3;

						pt_p->data2.i = 0;
						pt_p->data3.i = (PTLIF_SPRKA << 16) / i;
						pt_p->func_p =	(PV) ptlf_spiral;
					}
				} while (--n3 > 0);

			} while (--n2 > 0);
		}
#endif

	//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Ring

#if	1
		if (xfuel > 0) {		// Thing whitish ring
			xfuel--;

//		if (--xdly < 0) {

//			xdly = 20 + rndrng0 (30);
//			xdly = 9999;

			n2 = 150;
			do {
				if ((pt_p = ptl_get (0))) {

					pt_p->flgs = M_PTLF_BLENDA | M_PTLF_CLIPQ;
					pt_p->x = rcx + (-60 + rndrng0 (120)) * .01f * RIM_RAD;
					pt_p->y = RIM_CY + (-10 + rndrng0 (20)) * .01f * RIM_RAD;
					pt_p->z = RIM_CZ + (-10 + rndrng0 (20)) * .01f * RIM_RAD;
					i = rand();
					pt_p->vx = isin (i) * 28.0f * -PTL_GRAVITY;
					pt_p->vz = icos (i) * 28.0f * -PTL_GRAVITY;
					pt_p->vy = (30 + rndrng0 (0)) * .01f;
					pt_p->ay = PTL_GRAVITY * .25f;
					pt_p->drag = .96f;
					i = 50 + rndrng0 (5);
					pt_p->fuel = i;
					pt_p->rad = .8f;
					f = -.005f;
					pt_p->radv = f;		//-.07f;

					pt_p->img_p = PTLIMG(PTLI_X1);
					pt_p->frm = 1;

					pt_p->alpha = 200;
					pt_p->alphav = -200.0f / i;
					i = 0xff7fff + (rndrng0 (0x7f) << 8);
					pt_p->color = i;
				}
			} while (--n2 > 0);
		}

//		xfuel2 = 0;	// DEBUG

		if (xfuel2 > 0) {		// Thick yellow ring
			xfuel2--;

//		if (--xdly < 0) {

//			xdly = 20 + rndrng0 (30);
//			xdly = 9999;

			n2 = 150;
			do {
				if ((pt_p = ptl_get (0))) {

					pt_p->flgs = M_PTLF_BLENDA | M_PTLF_CLIPQ;
					pt_p->x = rcx + (-60 + rndrng0 (120)) * .01f * RIM_RAD;
					pt_p->y = RIM_CY + (-30 + rndrng0 (60)) * .01f * RIM_RAD;
					pt_p->z = RIM_CZ + (-60 + rndrng0 (120)) * .01f * RIM_RAD;
					i = rand();
					pt_p->vx = isin (i) * 23.0f * -PTL_GRAVITY;
					pt_p->vz = icos (i) * 23.0f * -PTL_GRAVITY;
					pt_p->vy = (20 + rndrng0 (20)) * .01f;
					pt_p->ay = PTL_GRAVITY * .25f;
					pt_p->drag = .96f;
					i = 50 + rndrng0 (6);
					pt_p->fuel = i;
					pt_p->rad = 2.8f;
					f = -.03f;
					if (n2 & 1) f = -.07f;
					pt_p->radv = f;		//-.07f;

					pt_p->img_p = PTLIMG(PTLI_X1);
					pt_p->frm = 1;

					pt_p->alpha = 200;
					pt_p->alphav = -200.0f / i;
					i = 0xff0000 + (rndrng0 (0x7f) << 8);
					pt_p->color = i;
				}
			} while (--n2 > 0);
		}
#endif

	//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Fire

#if	1
		n2 = 4;
		do {
			if ((pt_p = ptl_get (0))) {

				pt_p->flgs = M_PTLF_BLENDA | M_PTLF_CLIPQ;
				pt_p->x = rcx + (-60 + rndrng0 (120)) * .01f * RIM_RAD;
				pt_p->y = RIM_CY + (-60 + rndrng0 (120)) * .01f * RIM_RAD;
				pt_p->z = RIM_CZ + (-60 + rndrng0 (120)) * .01f * RIM_RAD;
				pt_p->vx = 0;
				pt_p->vy = 0;
				pt_p->vz = 0;
				pt_p->ay = PTL_GRAVITY * -.5f;
				pt_p->drag = .95f;
				i = 22 + rndrng0 (3);
				pt_p->fuel = i;
				pt_p->rad = 2.8f;
				pt_p->radv = -.07f;

 				pt_p->img_p = PTLIMG(PTLI_X1);
				pt_p->frm = 1;

				pt_p->alpha = 200;
				pt_p->alphav = -200.0f / i;
				i = 0xff0000 + (rndrng0 (0x7f) << 8);
				pt_p->color = i;
			}
		} while (--n2 > 0);
#endif

	//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Smoke

#if	1
		if ((n & 7) == 0) {

			if ((pt_p = ptl_get (0))) {

				pt_p->flgs = 0;
				pt_p->x = rcx + (-60 + rndrng0 (120)) * .01f * RIM_RAD;
				pt_p->y = RIM_CY + (-60 + rndrng0 (120)) * .01f * RIM_RAD;
				pt_p->z = RIM_CZ + (-60 + rndrng0 (120)) * .01f * RIM_RAD;
				pt_p->vx = 0;
				pt_p->vy = 0;
				pt_p->vz = 0;
				pt_p->ay = PTL_GRAVITY * -.15f;
				pt_p->drag = .95f;
				i = 80 + rndrng0 (40);
				pt_p->fuel = i;
				pt_p->rad = 4.0f;
				pt_p->radv = .08f;

	 			pt_p->img_p = PTLIMG(PTLI_X1);
				pt_p->frm = 0;
				i2 = rndrng0 (PTLIF_X1 - 1);

				pt_p->data2.i = 0;
				pt_p->data3.i = (i2 << 16) / i;

				pt_p->alpha = 80;
				pt_p->alphav = -80.0f / i;
//				pt_p->color = 0;

				pt_p->func_p =	(PV) ptlf_smk;
				pt_p->data1.i = 5;	// Color #
			}
		}
#endif
		sleep (1);

	} while (--n > 0);


	bbd_p->rimcolor = 0xff000000;

}


/****************************************************************/

#define	CAMFLASHQUADS	7

static	float	crdfpos_t[][12] = {	// Flash area quads. BL,BR,TL,TR
	{ -200,20, 70, -210,20, 20,  -320,85,170, -370,85,20},		// W
	{ -210,20,-20, -200,20,-70,  -370,85,-20, -320,85,-170},
	{ -160,20,-105, -100,20,-105,  -300,85,-245, -100,85,-245},	// NW
	{ -70,20,-105, 70,20,-105,  -70,85,-245, 70,85,-245},			// N
	{ 100,20,-105, 160,20,-105,  100,85,-245, 300,85,-245},		// NE
	{ 200,20, 70, 210,20, 20,  320,85,170, 370,85,20},				// E
	{ 210,20,-20, 200,20,-70,  370,85,-20, 320,85,-170},
//	{ -160,20,105, -100,20,105,  -300,85,245, -100,85,245},	// SW
//	{ -70,20,105, 70,20,105,  -70,85,245, 70,85,245},			// S
//	{ 100,20,105, 160,20,105,  100,85,245, 300,85,245},		// SE
};

static	int	crdfcol_t[] = {	0xffffff, 0xffffef, 0xffefff, 0xcfcfff };

/********************************
 Make camera flashes in crowd
 (Crowd section index (0-?))
********************************/

void	crowd_camflash()
{
	bbplyr_data *pl_p;
	PTL		*pt_p;

	int		ex;			// Excitement level (0-?)
	VEC		v;
	VEC		v2;

	int		n, n2, n3, i, i2, i3;
	float		f, f2, fx, fy, fz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#ifdef	DEBUG
	if (halt) return;
#endif

//	if (rand() & 3) return;

	ex = 0;
	n = NUM_PLAYERS;
	pl_p = player_blocks;

	do {	// Find % to flash

		i = pl_p->odata.flags;
		if (i & (PF_SHOOTING | PF_LAYUP))
			ex += 5;
		if (i & (PF_DUNKER | PF_ALLYDUNKER))
			ex += 20;

		if (game_info.ball_mode == BM_SHOOTING)
			ex += 5;

		if (pl_p->odata.adata[0].animode & MODE_REACTING)	// I'm flailing?
			ex += 2;
	
		if (game_info.game_mode == GM_FREE_THROW)
			ex += 17;

		if (game_info.game_state == GS_EOQ)
			ex += 5;

	} while (pl_p++, --n > 0);

	if (ex == 0) return;	// No action?

	if (rndrng0 (99) >= ex) return;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	n = CAMFLASHQUADS - 1;
	do {

//		n2 = 1;
//		do {

			f = rndrng0 (100) * .01f;
			f2 = 1 - f;
			v.x = crdfpos_t[n][0] * f + crdfpos_t[n][3] * f2;	// Rand between bot LR
			v.y = crdfpos_t[n][1] * f + crdfpos_t[n][4] * f2;
			v.z = crdfpos_t[n][2] * f + crdfpos_t[n][5] * f2;
			v2.x = crdfpos_t[n][6] * f + crdfpos_t[n][9] * f2;	// Top LR
			v2.y = crdfpos_t[n][7] * f + crdfpos_t[n][10] * f2;
			v2.z = crdfpos_t[n][8] * f + crdfpos_t[n][11] * f2;

			f = rndrng0 (100) * .01f;
			f2 = 1 - f;
			v.x = v.x * f + v2.x * f2;		// Rand between bottom & top
			v.y = v.y * f + v2.y * f2;
			v.z = v.z * f + v2.z * f2;


			if ((pt_p = ptl_get (0))) {

				pt_p->flgs = M_PTLF_BLENDA | M_PTLF_CLIPQ;
				pt_p->x = v.x;
				pt_p->y = v.y;
				pt_p->z = v.z;
				pt_p->vx = 0;
				pt_p->vy = 0;
				pt_p->vz = 0;
				pt_p->ay = 0;
				pt_p->drag = 0;
				i = 6 + rndrng0 (4);
				pt_p->fuel = i;
				pt_p->rad = 14.0f;
				pt_p->radv = -.05f;

				i2 = PTLI_SPRKA;
				i3 = PTLIF_SPRKA;
				if (rndrng0 (9) == 0) {
					i2 = PTLI_FLASHA;
					i3 = PTLIF_FLASHA;
					pt_p->rad = 11.0f;
				}
				pt_p->img_p = PTLIMG(i2);

				pt_p->alpha = 255;
				pt_p->alphav = -80.0f / i;
				pt_p->color = crdfcol_t[rndrng0 (3)];

				pt_p->func_p =	(PV) crowd_ptlf_flash;
				pt_p->data2.i = 0;				// Frame #
				pt_p->data3.i = (i3 << 16) / i;	// Stepping
			}
//		} while (--n2 > 0);

	} while (--n >= 0);

}

/********************************
 Camera flash particle function
********************************/

PTLFRET	crowd_ptlf_flash (PTL *pt_p)
{
	int		i;

	i = pt_p->data2.i;
	pt_p->data2.i = i + pt_p->data3.i;	// Step for next frame

	pt_p->frm = i >> 16;
}



/****************************************************************/

#ifndef DEBUG_RAINOFF
/********************************
 Create rain
 (Process *) *[0] = Unused
********************************/

#define	RAINSCL	1.2f

void	rain_proc (int *pa_p)
{
	int		dly;
	int		mode;		// 0 = Rain, 1 = Snow, 2 = Blowing snow

	float		bx, bz;
	float		stepx;
	float		stepz;

	PTL		*pt_p;

	int		n, n2, n3, i, i2, i3;
	float		f, fx, fy, fz, fx2, fy2, fz2;


	if (pup_snow)
		mode = 1;
	else if (pup_blizzard)
		mode = 2;
	else if (pup_rain)
		mode = 0;
	else
	{		
		mode = 0;
		if (rand() & 1) mode = 1 + rndrng0 (1);
	}

	dly = 0;

//	n = pa_p[0] + 30;
	n = 100000;

	do {

		if (--dly > 0) continue;
		dly = 1 + randrng(2);				// rain
		if (mode) dly = 3 + randrng(3);		// snow / blowing snow

	//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Spawn grid of rain

		stepx = COURT_L * RAINSCL * 2 / 6;
		stepz = COURT_W * RAINSCL * 2 / 6;

		bz = -COURT_W * RAINSCL;

		n2 = 6;
		do {

			bx = -COURT_L * RAINSCL;

			n3 = 6;
			do {
				if (((rand() & 3) == 0) && (pt_p = ptl_get(NULL))) {

					pt_p->flgs = M_PTLF_TEX | M_PTLF_CLIPQ;
					fx = bx + rndrng0 (99) * .01f * stepx;
					fz = bz + rndrng0 (99) * .01f * stepz;
					pt_p->x = fx;
					pt_p->y = RIM_CY * 3;
					pt_p->z = fz;
					pt_p->vx = 0;
					pt_p->vy = 0;
					pt_p->vz = 0;
					pt_p->ay = PTL_GRAVITY * .99f;

					pt_p->fuel = tsec * 7;
					pt_p->rad = 2.0f;
					pt_p->radv = -.002f;

					pt_p->frm = 0;

					pt_p->alpha = 120;
					pt_p->alphav = -.002f;

					if (mode == 0) {
						pt_p->img_p = PTLIMG(PTLI_RAIN);
						pt_p->drag = .98f;
					}
					else {
						pt_p->img_p = PTLIMG(PTLI_SNOW);
						pt_p->drag = .9f;
					}
					pt_p->color = 0xf0f0f0;
					pt_p->data1.i = mode;

					pt_p->func_p =	(PV) ptlf_rain;
				}
				bx += stepx;

			} while (--n3 > 0);

			bz += stepz;

		} while (--n2 > 0);


	} while (sleep (1), --n > 0);


}


/********************************
 Rain particle function
********************************/

PTLFRET	ptlf_rain (PTL *pt_p)
{
	int		i;
	float		f;

	i = pt_p->data1.i;

	if (i) {	// Snow?

		f = .005f;
		if (i == 2) {	// Blow around?
			f = .03f;
		}
		pt_p->vx += (10 - rndrng0 (20)) * f;
		pt_p->vz += (10 - rndrng0 (20)) * f;
	}

	if (pt_p->vy > 0) {

		pt_p->flgs = M_PTLF_CLIPQ;
		pt_p->func_p =	0;
		pt_p->alpha = 90;
		pt_p->rad = 1.0f;

		if ((rand() & 1) == 0) {	// Puddle

			pt_p->ay = 0;
			pt_p->drag = 0;
			i = tsec;
			if (pt_p->data1.i)		// Snow?
				i = tsec + rndrng0 (tsec);
			pt_p->fuel = i;
			pt_p->rad = 1.0f;
			pt_p->radv = 0;
//			pt_p->radv = -pt_p->rad / i;
			pt_p->alphav = -pt_p->alpha / i;
			pt_p->img_p = PTLIMG(PTLI_FLASHA);
			pt_p->color = 0xb0c0f0;
		}
		else {	// Bounce
			pt_p->vy *= .4f;
			i = 3 + rndrng0 (10);
			pt_p->fuel = i;
			pt_p->rad *= .5f;
			pt_p->radv = .01f;
			pt_p->alphav = -pt_p->alpha / i;
		}
	}

}
#endif //DEBUG_RAINOFF

/****************************************************************/

/********************************
 Generate matrix for rotation about a unit vector
 (* vector, Angle (12:4), * New matrix)
********************************/

void	mat_genvecrot (VEC *vec_p, int ang, MAT4 *m_p)
{
	float		sin, cos;

	int		i;
	float		f;
	float		xx, xy, xz, yy, yz, zz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	i = (ang + 32) >> 6 & 0x3ff;	// Round off, remove frac

	if (i < 256) {
		sin = trigs[i];
		cos = trigs[511-256-i];
	}
	else if (i < 512) {
		sin = trigs[511-i];
		cos = -trigs[i-256];
	}
	else if (i < 768) {
		sin = -trigs[i-512];
		cos = -trigs[1023-256-i];
	}
	else {
		sin = -trigs[1023-i];
		cos = trigs[i-768];
	}

//	ang = ang & 0xffff;
//	sin = fsin (ang * (M_TWOPI / (4096 << 4)));
//	cos = fsin ((ang + 0x4000) * (M_TWOPI / (4096 << 4)));

//	printf("BRot %f %f\n", cos, sin);

	f = vec_p->x;		// All combos of XYZ * XYZ
	xx = vec_p->x * f;
	xy = vec_p->y * f;
	xz = vec_p->z * f;
	f = vec_p->y;
	yy = vec_p->y * f;
	yz = vec_p->z * f;
	f = vec_p->z;
	zz = f * f;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ	Calc X axis

	m_p->xx = xx + cos * (1 - xx);
	m_p->yx = xy + cos * (0 - xy) + sin * vec_p->z;
	m_p->zx = xz + cos * (0 - xz) + sin * -vec_p->y;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ	Calc Y axis

	m_p->xy = xy +	cos * (0 - xy) + sin * -vec_p->z;
	m_p->yy = yy +	cos * (1 - yy);
	m_p->zy = yz +	cos * (0 - yz) + sin * vec_p->x;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ	Calc Z axis

	m_p->xz = xz +	cos * (0 - xz) + sin * vec_p->y;
	m_p->yz = yz +	cos * (0 - yz) + sin * -vec_p->x;
	m_p->zz = zz +	cos * (1 - zz);

}

/********************************
 Get a random value the range 0 to max
 (Max 0-7fff)
> 0 to max
********************************/

int	rndrng0 (int max)
{
	int		i;

	i = (rand() & 0xffff) * (max + 1) >> 16;

	return (i);
}

/****************************************************************/
// EOF
