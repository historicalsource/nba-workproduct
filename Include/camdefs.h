//////////////////////////////////////////////////////////////////////////////
//  Camera angle & distance defines
//
#ifndef CAMDEFS_H
#define CAMDEFS_H

#include "game.h"

// Camera angle defines

#define THETA_MIN_DG	0
#define THETA_MAX_DG	360
#define THETA_MIN		(DG2RD((float)THETA_MIN_DG))
#define THETA_MAX		(DG2RD((float)THETA_MAX_DG))

#define PHI_MIN_DG		89
#define PHI_MAX_DG		-89
#define PHI_MIN			(DG2RD((float)PHI_MIN_DG))
#define PHI_MAX			(DG2RD((float)PHI_MAX_DG))

#define THETA_STEP_MAX	(DG2RD(6.0f))			// Max theta stepping
#define PHI_STEP_MAX	(DG2RD(3.0f))			// Max phi stepping

#define ALPHA			(DG2RD(20.556f)/fovfac)	// Angle of viewline to vertical limit
#define DELTA			(DG2RD(180.0f)-ALPHA)	// Angle opposite of viewline (base)

// Camera distancing defines

#define FOV2UN(x)		(fovfac * (FT2UN * x))	// Distance conversion per FOV factor

//#define FIX_FDX			((0.5f+2.0f)*fovfac)	// X-distance Z/X ratio & buffer-factor
//#define FIX_FDX_PS		((0.1f+2.0f)*fovfac)	//  same but for pre-snap
//#define FIX_FDZ			((0.3f+1.0f)*fovfac)	// Z-distance buffer-factor

//#define FIXUN	(YD2UN / fovfac * FIX_FDX_PS)	// Modified YD2UN for pre-snap

// Camera-mode parameters/limits

#define SIDE_DIST_DEF		(FOV2UN(15.17f))	// Distance, default
//#define SIDE_DIST_MIN		(17.0f*YD2UN)		// Distance, minimum
//#define SIDE_DIST_MAX		(19.0f*FIXUN)		// Distance, maximum
/////
//#define SIDE_RANG_X			(98.0f)				// Range, +/- from world origin
/////
#define SIDE_DIST_TP2		(FOV2UN(23.34f))	// Distance, 2 on 2 tip-off
#define SIDE_DIST_TP3		(FOV2UN(32.67f))	// Distance, 3 on 3 tip-off
/////
#define SIDE_DIST_IB0		(FOV2UN(28.43f))	// Distance, inbounding
/////
#define SIDE_THETA			(DG2RD(180.0f))		// Theta, default
#define SIDE_THETA_OPP		(DG2RD(  0.0f))		// Theta, default opposing
#define SIDE_PHI			(DG2RD(-11.0f))		// Phi, default
/////
#define SIDE_THETA_IB0		(DG2RD(150.0f))		// Theta, inbounding
#define SIDE_THETA_IB0_OPP	(DG2RD(210.0f))		// Theta, inbounding opposing
#define SIDE_PHI_IB0		(DG2RD( -9.0f))		// Phi, inbounding
/////
#define TRGY_SIDE			(13.36f)			// Target Y, default
#define TRGZ_SIDE			(23.00f)			// Target Z, default
/////
#define TRGY_SIDE_TIP		(21.11f)			// Target Y, tipoff
#define TRGZ_SIDE_TIP		( 0.00f)			// Target Z, tipoff
/////
#define TRGX_SIDE_IB		(120.0f)			// Target X, inbounding
#define TRGY_SIDE_IB		(TRGY_SIDE)			// Target Y, inbounding
#define TRGZ_SIDE_IB		( 0.00f)			// Target Z, inbounding


//#define HTIME_DIST_DEF		(FOV2UN(34.35f))//31.70f))//23.50f))	// Distance, default
#define HTIME_DIST_DEF		(FOV2UN(31.70f))//31.70f))//23.50f))	// Distance, default
//#define HTIME_DIST_MIN		(17.0f*YD2UN)		// Distance, minimum
//#define HTIME_DIST_MAX		(19.0f*FIXUN)		// Distance, maximum
/////
#define HTIME_DIST_TP2		(FOV2UN(26.53f))	// Distance, 2 on 2 tip-off
#define HTIME_DIST_TP3		(FOV2UN(32.67f))	// Distance, 3 on 3 tip-off
/////
#define HTIME_DIST_IB0		(FOV2UN(43.20f))	// Distance, inbounding
#define HTIME_DIST_IB1		(FOV2UN(50.20f))	// Distance, inbounding
/////
//#define HTIME_RANG_X		(98.0f)				// Range, +/- from world origin
//#define HTIME_RANG_Z		(26.0f)				// Range, +/- from world origin
/////
#define HTIME_THETA			(DG2RD(180.0f))		// Theta, default
#define HTIME_THETA_OPP		(DG2RD(  0.0f))		// Theta, default opposing
#define HTIME_PHI			(DG2RD(-13.0f))//9.0f))//12.0f))		// Phi, default
/////
#define HTIME_THETA_IB0		(DG2RD(150.0f))		// Theta, inbounding
#define HTIME_THETA_IB0_OPP	(DG2RD(210.0f))		// Theta, inbounding opposing
#define HTIME_PHI_IB0		(DG2RD( -9.0f))		// Phi, inbounding
/////
#define HTIME_THETA_IB1		(DG2RD( 90.0f))		// Theta, inbounding
#define HTIME_THETA_IB1_OPP	(DG2RD(270.0f))		// Theta, inbounding opposing
#define HTIME_PHI_IB1		(DG2RD(-35.0f))		// Phi, inbounding
/////
#define HTIME_PHI_LB		(DG2RD(  -6.0f)) 	// Phi, loose ball
#define HTIME_PHI_DUNK		(DG2RD(  0.0f))		// Phi, dunk
#define HTIME_PHI_LAY		(DG2RD(  0.0f))		// Phi, layup
/////
#define HTIME_LOCK_DEF		( 55.0f)			// Lock X, default
#define HTIME_LOCK_LB		(105.0f)			// Lock X, loose ball
#define HTIME_LOCK_DUNK		( 70.0f)			// Lock X, dunk
#define HTIME_LOCK_SHOT		( 60.0f)			// Lock X, dunk
#define HTIME_PRE_LOCK		( 10.0f)			// Lock X range, pre-lock
#define HTIME_POST_LOCK		( 10.0f)			// Lock X range, post-lock
/////
#define TRGY_HTIME			(20.00f)//17.80f)			// Target Y, default
//#define TRGZ_HTIME			(21.00f)//19.79f)			// Target Z, default
#define TRGZ_HTIME			(19.79f)			// Target Z, default
/////
#define TRGY_HTIME_TIP		(17.00f)			// Target Y, tipoff
#define TRGZ_HTIME_TIP		( 0.00f)			// Target Z, tipoff
/////
#define TRGX_HTIME_IB		(86.00f)			// Target X, inbounding
#define TRGY_HTIME_IB		(TRGY_HTIME)		// Target Y, inbounding
#define TRGZ_HTIME_IB		( 0.00f)			// Target Z, inbounding


#define END_DIST_DEF		(FOV2UN(32.85f))	// Distance, default
//#define END_DIST_MIN		(17.0f*YD2UN)		// Distance, minimum
//#define END_DIST_MAX		(19.0f*FIXUN)		// Distance, maximum
/////
#define END_MULT_Y_BALL		(0.7f)				// Multiplier, axis-offset modifier
#define END_MULT_Y_PLYR		(1.2f)				// Multiplier, axis-offset modifier
#define END_MULT_Z			(0.6f)				// Multiplier, axis-offset modifier
/////
#define END_RANG_X			(94.0f)				// Range, +/- from world origin
/////
#define END_THETA			(DG2RD( 90.0f))		// Theta, default
#define END_THETA_OPP		(DG2RD(270.0f))		// Theta, default opposing
#define END_PHI				(DG2RD( -4.0f))		// Phi, default
/////
#define TRGY_END			(15.83f)			// Target Y, default


#define TRGY_ZOOM			(13.20f)			// Target Y, default
#define TRGY_SPLINE			(20.00f)			// Target Y, default		// Y focus	//15
#define TRGY_RAIL			(13.20f)			// Target Y, default
#define TRGY_PATH			(13.20f)			// Target Y, default
#define TRGY_COURSE			(13.20f)			// Target Y, default


////////////////////////
// CAMSEQINFO structures & defines

#define EOQ_TC		(EOQ_CNT + 25)				// ticks for EOQ trav_cnt (+25 for transist plates)
#define EOQ_MC		(EOQ_CNT + 50)				// ticks for EOQ mode_cnt

#define CSI(m,mc,tc,t,p,d,pi,y)	{m,mc,tc,DG2RD(t),DG2RD(p),d,pi,y}


////////////////////////
// Camera tipoff structures & defines

typedef struct tagTO_INFO
{
	float			x,y,z;						// camera x,y,z position
	int				ref_cnt; 					// ticks till do_jump_meters
	int				jm0_cnt; 					// ticks till make_jump_meter 0
	int				jm1_cnt; 					// ticks till make_jump_meter 1
	int				csi_cnt; 					// CAMSEQINFO seq item count
	CAMSEQINFO *	pcsi;						// ptr to CAMSEQINFO seq table
#ifdef DEBUG
	char *			szname;						// data id string
	int				ti_idx; 					// item # being edited
	int				bt_idx; 					// active menu item #
#endif
} TO_INFO;

#ifdef DEBUG
#define TI(n,x,y,z,rf,m0,m1)	TO_INFO ti_##n = {x,y,z,rf,m0,m1,sizeof(n)/sizeof(CAMSEQINFO),n,#n }
#else
#define TI(n,x,y,z,rf,m0,m1)	TO_INFO ti_##n = {x,y,z,rf,m0,m1,sizeof(n)/sizeof(CAMSEQINFO),n }
#endif


////////////////////////
// Camera pop-replay structures & defines

// Possible pop-replay camera owners in order of priority
enum {
	POP_IDLE = -1,								// Lowest: no owner

	POP_COOLDUNK,
	POP_LAYUP,
	POP_COOLJUKE,

	POP_ACTIVE,									// Highest: owner running

	POP_ABORT									// Abort pop-replay camera
};

typedef struct tagPOP_INFO
{
	int				tobj;						// Replay target obj
	int				ecnt;						// Replay frame cnt: post-event
	int				rcnt;						// Replay frame cnt: total
	int				rdly;						// Replay slomo delay ticks
	CAMSEQINFO *	pcsi;						// Replay CSI table ptr
#ifdef DEBUG
	char *			szname;						// data id string
#endif
} POP_INFO;

#ifdef DEBUG
#define PI(n,ob,ec,rc,rd)	POP_INFO pi_##n = {ob,ec,rc,rd,csip_##n,#n }
#else
#define PI(n,ob,ec,rc,rd)	POP_INFO pi_##n = {ob,ec,rc,rd,csip_##n }
#endif

#define POP_TOBJ_EVENT			0				// Watch event vertex
#define POP_TOBJ_PLAYER			1				// Watch event player
#define POP_TOBJ_BALL			2				// Watch ball
#define POP_TOBJ_WHO_SHOT		3				// Watch <ball_obj.who_shot>
#define POP_TOBJ_WHO_THREW		4				// Watch <ball_obj.who_threw>
#define POP_TOBJ_INT_RECEIVER	5				// Watch <ball_obj.int_receiver>

////////////////////////

#define POP_CDNK1_TOBJ	POP_TOBJ_WHO_SHOT		// Replay target obj
#define POP_CDNK1_ECNT	40						// Replay frame cnt: post-event
#define POP_CDNK1_RCNT	110						// Replay frame cnt: total
#define POP_CDNK1_RDLY	1						// Replay slomo delay ticks

#define POP_CDNK1_MCNT	(POP_CDNK1_RCNT*\
						(POP_CDNK1_RDLY+1))		// Replay mode tick total

#define POP_CDNK2_TOBJ	POP_TOBJ_EVENT			// Replay target obj
#define POP_CDNK2_ECNT	50						// Replay frame cnt: post-event
#define POP_CDNK2_RCNT	120						// Replay frame cnt: total
#define POP_CDNK2_RDLY	1						// Replay slomo delay ticks

#define POP_CDNK2_MCNT	(POP_CDNK2_RCNT*\
						(POP_CDNK2_RDLY+1))		// Replay mode tick total

////////////////////////

#define POP_CJUK1_TOBJ	POP_TOBJ_PLAYER			// Replay target obj
#define POP_CJUK1_ECNT	140						// Replay frame cnt: post-event
#define POP_CJUK1_RCNT	170						// Replay frame cnt: total
#define POP_CJUK1_RDLY	0						// Replay slomo delay ticks
#define POP_CJUK1_RDLY	0						// Replay slomo delay ticks

#define POP_CJUK1_MCNT	(POP_CJUK1_RCNT*\
						(POP_CJUK1_RDLY+1))		// Replay mode tick total

#define POP_CJUKE_FCNT	30						// Replay finish count

//////////////////////////////////////////////////////////////////////////////

#endif	// #ifndef CAMDEFS_H
