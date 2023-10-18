/*
 *		$Archive: /video/Nba/dribbler.c $
 *		$Revision: 80 $
 *		$Date: 3/24/99 4:35a $
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
char *ss_dribbler_c = "$Workfile: dribbler.c $ $Revision: 80 $";
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

#include "include/fontid.h"
#include "include/id.h"
#include "include/sndcalls.h"
#include "include/player.h"
#include "include/externs.h"
#include "include/game.h"
#include "anim/gen_seq.h"
#include "include/plyrseq.h"

//#include "anim/gen_seq.dbg"

//#define DEBUG_DRIBBLE
#define DEBUG_ID 0x7777

#define YES		1
#define NO 		0

#define BB_PLAYERS	6

#define RIGHT_HAND		0
#define LEFT_HAND		1

// this value is identical to the scale value in skeleton.h
#define PSCALE	0.1446258f

//#define BALL_RADIUS	1.30f
// also defined in player.c
//#define BALL_GRAVITY	-0.06f

extern ball_data 	ball_obj;
extern VIEWCOORD	cambot;

extern SNODE		bbplyr_skel[];

extern char gameplay_bnk_str[], plyr_bnk_str[],
			crowd_bnk_str[],
			*seq_names[]
			;
extern void
			get_joint_position(obj_3d *pobj, int sequence, int frame, int limb, int dir, float *x, float *y, float *z),
			get_joint_position_rel(obj_3d *pobj, int sequence, int frame, int limb, int dir, float *x, float *y, float *z, float relx, float rely, float relz)
			;
extern float dist3d( float *p1, float *p2 ), anix, aniz;

// also defined in player.c
#define R_INHAND_X_OFF	-25.8292f
#define R_INHAND_Y_OFF	117.9530f
#define R_INHAND_Z_OFF	2.25f
#define L_INHAND_X_OFF	25.8292f
#define L_INHAND_Y_OFF	117.9530f
#define L_INHAND_Z_OFF	2.25f
//-------

#define FRAMES		-7001
#define HOLD		-7002
#define B_RELEASE		-7003
#define HAND		-7004
#define REPEAT		-7777

#define H_SWITCH	-1000
#define H_LEFT		-1001
#define H_RIGHT		-1002
#define H_SAME		-1003

#define LAND_TIME	0.5f

//--------
struct tagDDATA {
	int name;
	int grav;
};
struct tagDDATA ddata_seqs[] = {
	{5000000, NO},
// seq name, real gravity?
	{SEQ_D_CYCL_1, NO},

	{SEQ_D_STND_R, NO},
	{SEQ_D_STND_L, NO},

	{SEQ_D_STD2_R, NO},
	{SEQ_D_STD2_L, NO},

	{SEQ_D_RUN2_R, NO},
	{SEQ_D_RUN2_L, NO},

	{SEQ_D_SP_T_R, NO},
	{SEQ_D_SP_T_L, NO},

	{SEQ_D_BLEG_R, NO},
	{SEQ_D_BLEG_L, NO},

	{SEQ_D_SPIN1R, NO},
	{SEQ_D_SPIN1L, NO},

	{SEQ_D_SPIN2R, NO},
	{SEQ_D_SPIN2L, NO},

	{SEQ_D_SPIN3R, NO},
	{SEQ_D_SPIN3L, NO},

	{SEQ_D_SPIN4R, NO},
	{SEQ_D_SPIN4L, NO},

	{SEQ_D_SPIN5R, NO},
	{SEQ_D_SPIN5L, NO},

	{SEQ_D_TS1_R, NO},
	{SEQ_D_TS1_L, NO},

	{SEQ_D_CHG2_R, NO},
	{SEQ_D_CHG2_L, NO},

	{SEQ_D_DBB1_R, NO},
	{SEQ_D_DBB1_L, NO},

	{SEQ_D_BLG1_R, NO},
	{SEQ_D_BLG1_L, NO},

	{SEQ_D_SLDR_R, NO},
	{SEQ_D_SLDL_L, NO},

	{SEQ_D_TBCK_R, NO},
	{SEQ_D_TBCK_L, NO},

	{SEQ_D_OVR1_R, NO},
	{SEQ_D_OVR1_L, NO},

	{SEQ_D_OVR3_R, NO},
	{SEQ_D_OVR3_L, NO},

	{SEQ_D_OVR5_R, NO},
	{SEQ_D_OVR5_L, NO},

	{SEQ_D_OVR6_R, NO},
	{SEQ_D_OVR6_L, NO},

	{SEQ_D_BKN1_R, NO},
	{SEQ_D_BKN1_L, NO},

	{SEQ_D_BKN2_R, NO},
	{SEQ_D_BKN2_L, NO},

	{SEQ_D_STD5_R, NO},
	{SEQ_D_STD5_L, NO},

	{SEQ_D_SPIDER, NO},

	{SEQ_D_SNP1_R, NO},
	{SEQ_D_SNP1_L, NO},

	{SEQ_D_STD6_R, NO},
	{SEQ_D_STD6_L, NO},

	{SEQ_D_SNP2_R, NO},
	{SEQ_D_SNP2_L, NO},

	{SEQ_D_WLK1_R, NO},
	{SEQ_D_WLK1_L, NO},

	{SEQ_D_OVR7_R, NO},
	{SEQ_D_OVR7_L, NO},

	{SEQ_D_COCK_1, NO},
	{SEQ_D_COCK_2, NO},

	{SEQ_D_REV1, NO},

	{SEQ_D_SID1_R, NO},
	{SEQ_D_SID1_L, NO},

	{SEQ_D_SPN1_R, NO},
	{SEQ_D_SPN1_L, NO},

//	{SEQ_FT1, NO},

	{SEQ_D_JUK8_R, NO},
	{SEQ_D_JUK8_L, NO},

	{SEQ_D_JUK7_R, NO},
	{SEQ_D_JUK7_L, NO},

};
#define NUM_DRIBBLE_SEQS	(sizeof(ddata_seqs)/sizeof(struct tagDDATA))

//--------

int ddata[NUM_DRIBBLE_SEQS][300] = {
	{FRAMES,0,400,H_SAME,HOLD, REPEAT }, // no dribbling

	//SEQ_D_CYCL_1
	{ FRAMES,0,7,H_LEFT,HOLD,
		FRAMES,7,5,H_SAME,B_RELEASE,768,1,H_RIGHT,
		FRAMES,12,5,H_SAME,HOLD,
		FRAMES,17,6,H_SAME,B_RELEASE,0,1,H_LEFT,
		FRAMES,23,3,H_SAME,HOLD,
		FRAMES,26,6,H_SAME,B_RELEASE,512,2,H_RIGHT,
		FRAMES,32,4,H_SAME,HOLD,
		FRAMES,36,4,H_SAME,B_RELEASE,0,1,H_LEFT,
		FRAMES,40,5,H_SAME,HOLD,
		FRAMES,45,4,H_SAME,B_RELEASE,0,1,H_RIGHT,
		FRAMES,49,5,H_SAME,HOLD,
		FRAMES,54,4,H_SAME,B_RELEASE,512,2,H_LEFT,
		FRAMES,58,6,H_SAME,HOLD,
		FRAMES,64,4,H_SAME,B_RELEASE,0,0,H_RIGHT,
		FRAMES,68,4,H_SAME,HOLD,
		FRAMES,72,5,H_SAME,B_RELEASE,0,0,H_LEFT,
		FRAMES,77,4,H_SAME,HOLD,
		FRAMES,81,4,H_SAME,B_RELEASE,512,2,H_RIGHT,
		FRAMES,85,5,H_SAME,HOLD,
		FRAMES,90,4,H_SAME,B_RELEASE,0,1,H_LEFT,
		FRAMES,94,7,H_SAME,HOLD,
		FRAMES,101,4,H_SAME,B_RELEASE,0,3,H_RIGHT,
		FRAMES,105,5,H_SAME,HOLD,
		FRAMES,110,4,H_SAME,B_RELEASE,0,3,H_LEFT,
		FRAMES,114,4,H_SAME,HOLD,
		FRAMES,118,5,H_SAME,B_RELEASE,512,2,H_RIGHT,
		FRAMES,123,5,H_SAME,HOLD,
		FRAMES,128,4,H_SAME,B_RELEASE,512,1,H_LEFT,
		FRAMES,132,6,H_SAME,HOLD,
		FRAMES,138,4,H_SAME,B_RELEASE,512,0,H_RIGHT,
		FRAMES,142,8,H_SAME,HOLD,
		REPEAT },

	// SEQ_D_STND_R
	{FRAMES,8,13,H_SAME,HOLD, FRAMES,22,24,H_SAME,B_RELEASE,900,8,H_SAME, REPEAT },
	// SEQ_D_STND_L
	{FRAMES,8,13,H_SAME,HOLD, FRAMES,22,24,H_SAME,B_RELEASE,124,8,H_SAME, REPEAT },

	// SEQ_D_STD2_R
	{ FRAMES,6,15,H_SAME,HOLD,
		FRAMES,21,13,H_SAME,B_RELEASE,945,10,H_SAME,
		REPEAT },
	// SEQ_D_STD2_L
	{ FRAMES,6,15,H_SAME,HOLD,
		FRAMES,21,13,H_SAME,B_RELEASE,79,10,H_SAME,
		REPEAT },
	//{FRAMES,6,15,H_SAME,HOLD, FRAMES,21,19,H_SAME,B_RELEASE,935,11,H_SAME, REPEAT },		// SEQ_D_STD2_R
	//{FRAMES,6,18,H_SAME,HOLD, FRAMES,25,15,H_SAME,B_RELEASE,115,11,H_SAME, REPEAT },		// SEQ_D_STD2_L

	// SEQ_D_RUN2_R	--	dribble_run2_anim_int_r
	{FRAMES,10,22,H_SAME,HOLD, FRAMES,32,15,H_SAME,B_RELEASE,950,7,H_SAME, REPEAT },
	// SEQ_D_RUN2_L	--	dribble_run2_anim_int_l
	{FRAMES,10,22,H_SAME,HOLD, FRAMES,32,15,H_SAME,B_RELEASE,74,7,H_SAME, REPEAT},

	// SEQ_D_SP_T_R		--  spin_behindback_anim_int_r
	{FRAMES,65,19,H_RIGHT,HOLD, FRAMES,10,12,H_SAME,B_RELEASE,512,4,H_LEFT,
	 FRAMES,23,25,H_SAME,HOLD, FRAMES,49,16,H_SAME,B_RELEASE,854,8,H_RIGHT, REPEAT},

	// SEQ_D_SP_T_L		--  spin_behindback_anim_int_l
	{FRAMES,65,19,H_LEFT,HOLD, FRAMES,10,12,H_SAME,B_RELEASE,512,4,H_RIGHT,
	 FRAMES,23,25,H_SAME,HOLD, FRAMES,49,16,H_SAME,B_RELEASE,170,8,H_LEFT, REPEAT},

	// SEQ_D_BLEG_R		--	between_both_legs_anim_int_r
	{FRAMES,0,32,H_SAME,HOLD, FRAMES,33,16,H_SAME,B_RELEASE,150,1,H_LEFT,
	 FRAMES,50,20,H_SAME,HOLD, FRAMES,71,16,H_SAME,B_RELEASE,900,1,H_RIGHT,
	 FRAMES,88,15,H_RIGHT,HOLD,  REPEAT },

	// SEQ_D_BLEG_L		--	between_both_legs_anim_int_l
	{FRAMES,0,32,H_SAME,HOLD, FRAMES,33,16,H_SAME,B_RELEASE,874,1,H_RIGHT,
	 FRAMES,50,20,H_SAME,HOLD, FRAMES,71,16,H_SAME,B_RELEASE,124,1,H_LEFT,
	 FRAMES,88,15,H_LEFT,HOLD,  REPEAT },

	// SEQ_D_SPIN1R
	{FRAMES,0,50,H_RIGHT,HOLD, FRAMES,44,20,H_SAME,B_RELEASE,-7,3,H_RIGHT,
	 FRAMES,64,20,H_SAME,HOLD, FRAMES,84,14,H_SAME,B_RELEASE,-4,5,H_RIGHT, REPEAT},

	// SEQ_D_SPIN1L
	{FRAMES,0,50,H_LEFT,HOLD, FRAMES,44,20,H_SAME,B_RELEASE,7,3,H_LEFT,
	 FRAMES,64,20,H_SAME,HOLD, FRAMES,84,14,H_SAME,B_RELEASE,4,5,H_LEFT, REPEAT},

	// SEQ_D_SPIN2R		--	standard spin move
	{	FRAMES,40,31,H_SAME,HOLD,
		FRAMES,24,15,H_SAME,B_RELEASE,164,9,H_LEFT,
		REPEAT},
	// SEQ_D_SPIN2L		--	standard spin move
	{	FRAMES,40,31,H_SAME,HOLD,
		FRAMES,24,15,H_SAME,B_RELEASE,860,9,H_RIGHT,
		REPEAT},

// Iverson spin
#if 0
// SEQ_D_SPIN3R
	{	FRAMES,9,7,H_SAME,B_RELEASE,900,5,H_RIGHT,
		FRAMES,16,17,H_SAME,HOLD,
		REPEAT},
// SEQ_D_SPIN3L
	{	FRAMES,9,7,H_SAME,B_RELEASE,124,5,H_LEFT,
		FRAMES,16,17,H_SAME,HOLD,
		REPEAT},
#else
// SEQ_D_SPIN3R
	{	FRAMES,9,5,H_SAME,B_RELEASE,900,5,H_RIGHT,
		FRAMES,14,19,H_SAME,HOLD,
		REPEAT},
// SEQ_D_SPIN3L
	{	FRAMES,9,5,H_SAME,B_RELEASE,124,5,H_LEFT,
		FRAMES,14,19,H_SAME,HOLD,
		REPEAT},
#endif
// SEQ_D_SPIN4R		--	half spin and then back
	{	FRAMES,37,53,H_SAME,HOLD,
		FRAMES,30,6,H_SAME,B_RELEASE,300,7,H_SAME,
		REPEAT},
// SEQ_D_SPIN4L		--	half spin and then back
	{	FRAMES,37,53,H_SAME,HOLD,
		FRAMES,30,6,H_SAME,B_RELEASE,724,7,H_SAME,
		REPEAT},

// SEQ_D_SPIN5R		--	standard spin move 2
	{	FRAMES,9,6,H_SAME,B_RELEASE,164,9,H_LEFT,
		FRAMES,15,15,H_SAME,HOLD,
		REPEAT},
// SEQ_D_SPIN5L		--	standard spin move 2
	{	FRAMES,9,6,H_SAME,B_RELEASE,860,9,H_RIGHT,
		FRAMES,15,15,H_SAME,HOLD,
		REPEAT},

// into a standing dribble
// SEQ_D_TS1_R		--	dribble intostand r
	{FRAMES,0,15,H_SAME,HOLD, FRAMES,16,11,H_SAME,B_RELEASE,2,3,H_LEFT,
	 FRAMES,28,28,H_LEFT,HOLD, FRAMES,56,8,H_LEFT,B_RELEASE,0,8,H_RIGHT,
	 FRAMES,64,17,H_SAME,HOLD, REPEAT },
// SEQ_D_TS1_L		--	dribble intostand l
	{FRAMES,0,15,H_SAME,HOLD, FRAMES,16,11,H_SAME,B_RELEASE,2,1021,H_RIGHT,
	 FRAMES,28,28,H_RIGHT,HOLD, FRAMES,56,8,H_RIGHT,B_RELEASE,0,8,H_LEFT,
	 FRAMES,64,17,H_SAME,HOLD, REPEAT },

// SEQ_D_CHG2_R
	{FRAMES,0,25,H_SAME,HOLD, FRAMES,26,20,H_SAME,B_RELEASE,5,7,H_LEFT,
	 FRAMES,46,20,H_SAME,HOLD, REPEAT },
// SEQ_D_CHG2_L
	{FRAMES,0,25,H_SAME,HOLD, FRAMES,26,20,H_SAME,B_RELEASE,-5,7,H_RIGHT,
	 FRAMES,46,20,H_SAME,HOLD, REPEAT },

// SEQ_D_DBB1_R		-- standing dribble behind back twice
	{FRAMES,0,22,H_SAME,HOLD, FRAMES,23,14,H_SAME,B_RELEASE,512,3,H_LEFT,
	 FRAMES,37,21,H_SAME,HOLD, FRAMES,58,14,H_SAME,B_RELEASE,512,3,H_RIGHT,
	 FRAMES,73,27,H_SAME,HOLD, REPEAT },
// SEQ_D_DBB1_L		-- standing dribble behind back twice
	{FRAMES,0,22,H_SAME,HOLD, FRAMES,23,14,H_SAME,B_RELEASE,512,3,H_RIGHT,
	 FRAMES,37,21,H_SAME,HOLD, FRAMES,58,14,H_SAME,B_RELEASE,512,3,H_LEFT,
	 FRAMES,73,27,H_SAME,HOLD, REPEAT },

// SEQ_D_BLG1_R		--	running between leg
	{FRAMES,0,14,H_SAME,HOLD, FRAMES,14,14,H_SAME,B_RELEASE,3,1,H_LEFT,
	 FRAMES,28,21,H_SAME,HOLD, REPEAT },
// SEQ_D_BLG1_L		--	running between leg
	{FRAMES,0,14,H_SAME,HOLD, FRAMES,14,14,H_SAME,B_RELEASE,1020,1,H_RIGHT,
	 FRAMES,28,21,H_SAME,HOLD, REPEAT },

// SEQ_D_SLDR_R
	{FRAMES,37,28,H_SAME,HOLD, FRAMES,21,16,H_SAME,B_RELEASE,768,12,H_SAME, REPEAT},
// SEQ_D_SLDL_L
	{FRAMES,37,28,H_SAME,HOLD, FRAMES,21,16,H_SAME,B_RELEASE,256,12,H_SAME, REPEAT},
	
// SEQ_TBCK_R -- 180 change of direction
	{FRAMES,32,36,H_SAME,HOLD, FRAMES,16,16,H_SAME,B_RELEASE,384,4,H_LEFT, REPEAT},
// SEQ_TBCK_L -- 180 change of direction
	{FRAMES,32,36,H_SAME,HOLD, FRAMES,16,16,H_SAME,B_RELEASE,128,4,H_RIGHT, REPEAT},
	
// SEQ_OVR1_R -- standing high crossover (Kobe Bryant)
	{	FRAMES,0,10,H_SAME,HOLD,
		FRAMES,11,8,H_SAME,B_RELEASE,909,7,H_SAME,
		FRAMES,19,18,H_SAME,HOLD,
		FRAMES,37,9,H_SAME,B_RELEASE,100,8,H_LEFT,
		FRAMES,47,14,H_SAME,HOLD,
		REPEAT},
// SEQ_OVR1_L -- standing high crossover (Kobe Bryant)
	{	FRAMES,0,10,H_SAME,HOLD,
		FRAMES,11,8,H_SAME,B_RELEASE,115,7,H_SAME,
		FRAMES,19,18,H_SAME,HOLD,
		FRAMES,37,9,H_SAME,B_RELEASE,924,8,H_RIGHT,
		FRAMES,47,14,H_SAME,HOLD,
		REPEAT},

	// SEQ_OVR3_R -- Hessitation high crossover (Kobe Bryant)
	{FRAMES,0,27,H_SAME,HOLD, FRAMES,28,11,H_SAME,B_RELEASE,190,3,H_LEFT, FRAMES,39,22,H_SAME,HOLD, REPEAT},
	// SEQ_OVR3_L -- Hessitation high crossover (Kobe Bryant)
	{FRAMES,0,27,H_SAME,HOLD, FRAMES,28,11,H_SAME,B_RELEASE,834,3,H_RIGHT, FRAMES,39,22,H_SAME,HOLD, REPEAT},

// SEQ_D_OVR5_R -- Tim Hardaway crossover
	{	FRAMES,30,8,H_SAME,B_RELEASE,115,2,H_LEFT,
		FRAMES,38,17,H_SAME,HOLD,
		FRAMES,55,6,H_SAME,B_RELEASE,900,7,H_RIGHT,
		FRAMES,61,49,H_SAME,HOLD,
		REPEAT},
// SEQ_D_OVR5_L -- Tim Hardaway crossover
	{	FRAMES,30,8,H_SAME,B_RELEASE,909,2,H_RIGHT,
		FRAMES,38,17,H_SAME,HOLD,
		FRAMES,55,6,H_SAME,B_RELEASE,124,7,H_LEFT,
		FRAMES,61,49,H_SAME,HOLD,
		REPEAT},

// SEQ_D_OVR6_R -- Hessitation high crossover (Kobe Bryant)
	{	FRAMES,0,25,H_SAME,HOLD,
		FRAMES,26,6,H_SAME,B_RELEASE,115,2,H_LEFT,
		FRAMES,32,8,H_SAME,HOLD,
		FRAMES,40,6,H_SAME,B_RELEASE,115,2,H_RIGHT,
		FRAMES,47,13,H_SAME,HOLD,
		REPEAT},
// SEQ_D_OVR6_L -- Hessitation high crossover (Kobe Bryant)
	{	FRAMES,0,25,H_SAME,HOLD,
		FRAMES,26,6,H_SAME,B_RELEASE,909,2,H_RIGHT,
		FRAMES,32,8,H_SAME,HOLD,
		FRAMES,40,6,H_SAME,B_RELEASE,115,2,H_LEFT,
		FRAMES,47,13,H_SAME,HOLD,
		REPEAT},

// SEQ_D_BKN1_R	-- backin dribble bumping defender
	{ FRAMES,5,38,H_SAME,HOLD,
		FRAMES,43,11,H_SAME,B_RELEASE,500,5,H_SAME,
		REPEAT },

// SEQ_D_BKN1_L	-- backin dribble bumping defender
	{ FRAMES,5,38,H_SAME,HOLD,
		FRAMES,43,11,H_SAME,B_RELEASE,524,5,H_SAME,
		REPEAT },

// SEQ_D_BKN2_R	-- backin dribble
	{ FRAMES,7,24,H_SAME,HOLD,
		FRAMES,31,15,H_SAME,B_RELEASE,590,9,H_SAME,
		REPEAT },

// SEQ_D_BKN2_L -- backin dribble
	{ FRAMES,7,24,H_SAME,HOLD,
		FRAMES,31,15,H_SAME,B_RELEASE,434,9,H_SAME,
		REPEAT },

// SEQ_D_STD5_R -- standing backin dribble
	{ FRAMES,36,29,H_SAME,HOLD,
		FRAMES,26,10,H_SAME,B_RELEASE,590,9,H_SAME,
		REPEAT },

// SEQ_D_STD5_L -- standing backin dribble
	{ FRAMES,36,29,H_SAME,HOLD,
		FRAMES,26,10,H_SAME,B_RELEASE,434,9,H_SAME,
		REPEAT },

// SEQ_D_SPIDER -- spider dribble
	{	FRAMES,7,4,H_LEFT,HOLD,
		FRAMES,11,7,H_SAME,B_RELEASE,990,2,H_RIGHT,
		FRAMES,18,4,H_SAME,HOLD,
		FRAMES,22,9,H_SAME,B_RELEASE,470,0,H_LEFT,
		FRAMES,31,4,H_SAME,HOLD,
		FRAMES,35,16,H_SAME,B_RELEASE,530,0,H_RIGHT,
		FRAMES,51,4,H_SAME,HOLD,
		FRAMES,55,11,H_SAME,B_RELEASE,20,2,H_LEFT,
		REPEAT},

// SEQ_D_SNP1_R -- standing dribble snippet 1
	{	FRAMES,2,8,H_SAME,B_RELEASE,900,7,H_SAME,
		FRAMES,11,10,H_SAME,HOLD,
		FRAMES,22,8,H_SAME,B_RELEASE,850,7,H_SAME,
		FRAMES,30,18,H_SAME,HOLD,
		FRAMES,48,8,H_SAME,B_RELEASE,850,9,H_SAME,
		FRAMES,56,15,H_SAME,HOLD,
		FRAMES,71,8,H_SAME,B_RELEASE,850,9,H_SAME,
		FRAMES,79,23,H_SAME,HOLD,
		REPEAT },
// SEQ_D_SNP1_L
	{	FRAMES,2,8,H_SAME,B_RELEASE,124,7,H_SAME,
		FRAMES,11,10,H_SAME,HOLD,
		FRAMES,22,8,H_SAME,B_RELEASE,174,7,H_SAME,
		FRAMES,30,18,H_SAME,HOLD,
		FRAMES,48,8,H_SAME,B_RELEASE,174,9,H_SAME,
		FRAMES,56,15,H_SAME,HOLD,
		FRAMES,71,8,H_SAME,B_RELEASE,174,9,H_SAME,
		FRAMES,79,23,H_SAME,HOLD,
		REPEAT },

// SEQ_D_STD6_R -- standing dribble 6(one leg back)
	{	FRAMES,2,7,H_SAME,B_RELEASE,834,8,H_SAME,
		FRAMES,9,10,H_SAME,HOLD,
		FRAMES,18,6,H_SAME,B_RELEASE,860,8,H_SAME,
		FRAMES,24,10,H_SAME,HOLD,
		FRAMES,34,6,H_SAME,B_RELEASE,860,8,H_SAME,
		FRAMES,40,11,H_SAME,HOLD,
		FRAMES,51,6,H_SAME,B_RELEASE,860,8,H_SAME,
		FRAMES,57,9,H_SAME,HOLD,
		FRAMES,66,6,H_SAME,B_RELEASE,860,8,H_SAME,
		FRAMES,72,12,H_SAME,HOLD,
		FRAMES,84,6,H_SAME,B_RELEASE,860,8,H_SAME,
		FRAMES,90,9,H_SAME,HOLD,
		FRAMES,99,6,H_SAME,B_RELEASE,860,8,H_SAME,
		FRAMES,105,5,H_SAME,HOLD,
		REPEAT
		},
// SEQ_D_STD6_L -- standing dribble 6(one leg back)
	{	FRAMES,2,7,H_SAME,B_RELEASE,220,8,H_SAME,
		FRAMES,9,10,H_SAME,HOLD,
		FRAMES,18,6,H_SAME,B_RELEASE,195,8,H_SAME,
		FRAMES,24,10,H_SAME,HOLD,
		FRAMES,34,6,H_SAME,B_RELEASE,195,8,H_SAME,
		FRAMES,40,11,H_SAME,HOLD,
		FRAMES,51,6,H_SAME,B_RELEASE,195,8,H_SAME,
		FRAMES,57,9,H_SAME,HOLD,
		FRAMES,66,6,H_SAME,B_RELEASE,195,8,H_SAME,
		FRAMES,72,12,H_SAME,HOLD,
		FRAMES,84,6,H_SAME,B_RELEASE,195,8,H_SAME,
		FRAMES,90,9,H_SAME,HOLD,
		FRAMES,99,6,H_SAME,B_RELEASE,195,8,H_SAME,
		FRAMES,105,5,H_SAME,HOLD,
		REPEAT
		},
// SEQ_D_SNP2_R -- standing dribble 2(one leg back)
	{	FRAMES,3,6,H_RIGHT,B_RELEASE,834,8,H_SAME,
		FRAMES,9,7,H_SAME,HOLD,
		FRAMES,16,6,H_SAME,B_RELEASE,870,8,H_SAME,
		FRAMES,22,7,H_SAME,HOLD,
		FRAMES,29,5,H_SAME,B_RELEASE,0,1,H_LEFT,
		FRAMES,34,5,H_SAME,HOLD,
		FRAMES,39,6,H_SAME,B_RELEASE,0,1,H_RIGHT,
		FRAMES,45,9,H_SAME,HOLD,
		FRAMES,54,5,H_SAME,B_RELEASE,0,1,H_LEFT,
		FRAMES,59,6,H_SAME,HOLD,
		FRAMES,65,5,H_SAME,B_RELEASE,0,1,H_RIGHT,
		FRAMES,70,8,H_SAME,HOLD,
		FRAMES,78,5,H_SAME,B_RELEASE,768,8,H_SAME,
		FRAMES,83,10,H_SAME,HOLD,

		FRAMES,93,5,H_SAME,B_RELEASE,870,8,H_SAME,
		FRAMES,98,4,H_SAME,HOLD,
		FRAMES,102,5,H_SAME,B_RELEASE,870,8,H_SAME,
		FRAMES,107,12,H_SAME,HOLD,
		FRAMES,119,6,H_SAME,B_RELEASE,870,8,H_SAME,
		FRAMES,125,4,H_SAME,HOLD,
		FRAMES,129,5,H_SAME,B_RELEASE,870,8,H_SAME,
		FRAMES,134,11,H_SAME,HOLD,
		FRAMES,145,5,H_SAME,B_RELEASE,0,1,H_LEFT,
		FRAMES,150,6,H_SAME,HOLD,

		FRAMES,156,5,H_SAME,B_RELEASE,0,1,H_RIGHT,
		FRAMES,161,14,H_SAME,HOLD,
		FRAMES,175,5,H_SAME,B_RELEASE,840,8,H_SAME,
		FRAMES,180,10,H_SAME,HOLD,
		FRAMES,190,5,H_SAME,B_RELEASE,840,8,H_SAME,
		FRAMES,195,8,H_SAME,HOLD,
		REPEAT
		},
// SEQ_D_SNP2_L		--	standing dribble 2(one leg back)
	{	FRAMES,3,6,H_LEFT,B_RELEASE,190,8,H_SAME,
		FRAMES,9,7,H_SAME,HOLD,
		FRAMES,16,6,H_SAME,B_RELEASE,154,8,H_SAME,
		FRAMES,22,7,H_SAME,HOLD,
		FRAMES,29,5,H_SAME,B_RELEASE,0,1,H_RIGHT,
		FRAMES,34,5,H_SAME,HOLD,
		FRAMES,39,6,H_SAME,B_RELEASE,0,1,H_LEFT,
		FRAMES,45,9,H_SAME,HOLD,
		FRAMES,54,5,H_SAME,B_RELEASE,0,1,H_RIGHT,
		FRAMES,59,6,H_SAME,HOLD,
		FRAMES,65,5,H_SAME,B_RELEASE,0,1,H_LEFT,
		FRAMES,70,8,H_SAME,HOLD,
		FRAMES,78,5,H_SAME,B_RELEASE,256,8,H_SAME,
		FRAMES,83,10,H_SAME,HOLD,

		FRAMES,93,5,H_SAME,B_RELEASE,154,8,H_SAME,
		FRAMES,98,4,H_SAME,HOLD,
		FRAMES,102,5,H_SAME,B_RELEASE,154,8,H_SAME,
		FRAMES,107,12,H_SAME,HOLD,
		FRAMES,119,6,H_SAME,B_RELEASE,154,8,H_SAME,
		FRAMES,125,4,H_SAME,HOLD,
		FRAMES,129,5,H_SAME,B_RELEASE,154,8,H_SAME,
		FRAMES,134,11,H_SAME,HOLD,
		FRAMES,145,5,H_SAME,B_RELEASE,0,1,H_RIGHT,
		FRAMES,150,6,H_SAME,HOLD,

		FRAMES,156,5,H_SAME,B_RELEASE,0,1,H_LEFT,
		FRAMES,161,14,H_SAME,HOLD,
		FRAMES,175,5,H_SAME,B_RELEASE,184,8,H_SAME,
		FRAMES,180,10,H_SAME,HOLD,
		FRAMES,190,5,H_SAME,B_RELEASE,184,8,H_SAME,
		FRAMES,195,8,H_SAME,HOLD,
		REPEAT
		},

// SEQ_D_WLK1_R		-- walking dribble 1
	{
		FRAMES,4,18,H_SAME,B_RELEASE,900,6,H_SAME,
		FRAMES,22,21,H_SAME,HOLD,
		FRAMES,43,15,H_SAME,B_RELEASE,900,6,H_SAME,
		FRAMES,58,20,H_SAME,HOLD,
		REPEAT
	},
// SEQ_D_WLK1_L -- walking dribble 1
	{
		FRAMES,4,18,H_SAME,B_RELEASE,79,6,H_SAME,
		FRAMES,22,21,H_SAME,HOLD,
		FRAMES,43,15,H_SAME,B_RELEASE,79,6,H_SAME,
		FRAMES,58,20,H_SAME,HOLD,
		REPEAT
	},

// SEQ_D_OVR7_R -- around the back twice then between the legs
	{
		FRAMES,110,38,H_SAME,HOLD,
		FRAMES,15,18,H_RIGHT,B_RELEASE,320,7,H_LEFT,
		FRAMES,33,28,H_SAME,HOLD,
		FRAMES,61,20,H_SAME,B_RELEASE,730,7,H_RIGHT,
		FRAMES,81,17,H_SAME,HOLD,
		FRAMES,98,11,H_SAME,B_RELEASE,90,3,H_LEFT,
		REPEAT
	},
// SEQ_D_OVR7_L -- around the back twice then between the legs
	{
		FRAMES,110,38,H_SAME,HOLD,
		FRAMES,15,18,H_LEFT,B_RELEASE,704,7,H_RIGHT,
		FRAMES,33,28,H_SAME,HOLD,
		FRAMES,61,20,H_SAME,B_RELEASE,294,7,H_LEFT,
		FRAMES,81,17,H_SAME,HOLD,
		FRAMES,98,11,H_SAME,B_RELEASE,934,3,H_RIGHT,
		REPEAT
	},

// SEQ_D_COCK_1 -- cocky dribble running forward
	{
		FRAMES,12,15,H_RIGHT,B_RELEASE,512,4,H_LEFT,
		FRAMES,27,17,H_SAME,HOLD,
		FRAMES,44,15,H_SAME,B_RELEASE,512,4,H_RIGHT,
		FRAMES,4,9,H_SAME,HOLD,
		REPEAT
	},

// SEQ_D_COCK_2 -- cocky dribble running forward
	{
		FRAMES,48,14,H_RIGHT,B_RELEASE,512,0,H_LEFT,
		FRAMES,10,11,H_SAME,HOLD,
		FRAMES,21,12,H_SAME,B_RELEASE,512,0,H_RIGHT,
		FRAMES,33,15,H_SAME,HOLD,
		REPEAT
	},

// SEQ_D_REV1 -- cocky dribble running backwards facing defender
	{
		FRAMES,11,21,H_LEFT,HOLD,
		FRAMES,32,15,H_SAME,B_RELEASE,980,2,H_RIGHT,
		FRAMES,47,24,H_SAME,HOLD,
		FRAMES,71,15,H_SAME,B_RELEASE,44,2,H_LEFT,
		REPEAT
	},

// SEQ_D_SID1_R -- side dribble transition into a run
	{
		FRAMES,44,12,H_SAME,B_RELEASE,64,2,H_RIGHT,
		FRAMES,6,38,H_SAME,HOLD,
		REPEAT
	},

// SEQ_D_SID1_L -- side dribble transition into a run
	{
		FRAMES,44,12,H_SAME,B_RELEASE,960,2,H_RIGHT,
		FRAMES,6,38,H_SAME,HOLD,
		REPEAT
	},

// SEQ_D_SPN1_R -- standard spin move 2
	{FRAMES,0,60,H_SAME,HOLD, REPEAT},
// SEQ_D_SPN1_L -- standard spin move 2
	{FRAMES,0,60,H_SAME,HOLD, REPEAT},

// SEQ_FT1 --- free throw dribbling
//	{
//		FRAMES,49,8,H_SAME,B_RELEASE,0,8,H_SAME,
//		FRAMES,57,21,H_SAME,HOLD,
//		FRAMES,78,8,H_SAME,B_RELEASE,0,8,H_SAME,
//		FRAMES,86,73,H_SAME,HOLD,
//		REPEAT
//	},
// SEQ_FT2 --- free throw dribbling
//	{
//		FRAMES,49,8,H_SAME,B_RELEASE,900,6,H_SAME,
//		FRAMES,57,21,H_SAME,HOLD,
//		FRAMES,79,8,H_SAME,B_RELEASE,900,6,H_SAME,
//		FRAMES,86,73,H_SAME,HOLD,
//		REPEAT
//	},

// SEQ_D_JUK8_R -- roll on ground spin move
	{	FRAMES,0,19,H_LEFT,HOLD,
		FRAMES,19,14,H_SAME,B_RELEASE,800,8,H_RIGHT,
		FRAMES,33,26,H_RIGHT,HOLD,
		REPEAT},
// SEQ_D_JUK8_L -- roll on ground spin move
	{	FRAMES,0,19,H_RIGHT,HOLD,
		FRAMES,19,14,H_SAME,B_RELEASE,224,8,H_LEFT,
		FRAMES,33,26,H_LEFT,HOLD,
		REPEAT},

// SEQ_D_JUK7_R -- behind back, spin, between legs
	{	FRAMES,9,6,H_SAME,B_RELEASE,512,2,H_LEFT,
		FRAMES,15,20,H_LEFT,HOLD,
		FRAMES,35,9,H_SAME,B_RELEASE,0,4,H_RIGHT,
		FRAMES,44,16,H_RIGHT,HOLD,
		REPEAT},
// SEQ_D_JUK7_L -- behind back, spin, between legs
	{	FRAMES,9,6,H_SAME,B_RELEASE,512,2,H_RIGHT,
		FRAMES,15,20,H_RIGHT,HOLD,
		FRAMES,35,9,H_SAME,B_RELEASE,0,4,H_LEFT,
		FRAMES,44,16,H_LEFT,HOLD,
		REPEAT},

};
//--------

/*
	Called from BallProc

	Dribbler handles all ball movement during the dribbling sequence
	Each sequence will have:
	 start-end hold frames, a release frame, tick count, handedness, angle and dist floor delta from player center
	 -777 terminates the dribble script for the current sequence
*/
void Dribbler(obj_3d *pobj)
{
bbplyr_data *ppdata = (bbplyr_data *)pobj;
int rtime = 0, i, hand, action = -1, joint, seq = 0, sequence, nframes, fspread = 0, UG = YES;
float time, hd, ciframew = -1.0f, ciframe, s, e, temp = 0.0f;
float hx = 0.0f, hy = 0.0f, hz = 0.0f, lhx,lhy,lhz, rhx, rhy, rhz, bx,by,bz, *tm, tx, ty, tz, p[3];
static int ball_to_floor = -1, new_hand = -1, did_gravity = NO, _did_gravity = NO, cseq = -1, lfwd = -1, rel = NO, dir_change = NO, grab = NO;
static float rx = -1000.0f, ry = 0.0f, rz = -1000.0f, vy = 0.0f, fx, fz;

	if(pobj->adata[1].animode & MODE_TWOPART)
	{
		sequence = pobj->adata[1].seq_index;
	}
	else
	{
		sequence = pobj->adata[0].seq_index;
	}

	for(i=0; i < NUM_DRIBBLE_SEQS; i++)
	{
		if(sequence == ddata_seqs[i].name)
		{
			seq = i;
			UG = ddata_seqs[i].grav;	// Real Gravity?
		}	
	}
	if(seq >= NUM_DRIBBLE_SEQS)
	{
#ifdef DEBUG
		fprintf(stderr,"DRIBBLE ERROR!\n");
		lockup();
#endif
	}	

	if(cseq != seq)
	{
		cseq = seq;
		did_gravity = _did_gravity = NO;
		grab = YES;
	}
// if player changes dir then rotate the ball to match the new dir
	if(lfwd != pobj->fwd)
	{
		lfwd = pobj->fwd;
		dir_change = YES;
	}

	if((pobj->adata[1].animode & MODE_TWOPART))
		nframes = pobj->adata[1].pahdr->nframes;
	else	
		nframes = pobj->adata[0].pahdr->nframes;

	game_info.ball_mode = BM_DRIBBLING;
//	if(pobj->flags & PF_PASSING)
//		game_info.ball_mode = BM_PASSING; // passing

	tm = cambot.xform;

// ball's position relative to joint's zero position
	bx = (R_INHAND_X_OFF * PSCALE);// * (1.0f + (1.0f - pobj->xscale));
	by = (R_INHAND_Y_OFF * PSCALE);// * (1.0f + (1.0f - pobj->xscale));
	//by = (R_INHAND_Y_OFF * PSCALE) * (1.0f + (1.0f - pobj->yscale));
	//by = ((R_INHAND_Y_OFF + 4.0f) * pobj->yscale) * PSCALE;
	bz = (R_INHAND_Z_OFF * PSCALE);// * (1.0f + (1.0f - pobj->zscale));
	joint = JOINT_RWRIST;
// screen space, world space
	tx = bx * ppdata->matrices[joint][0] + by * ppdata->matrices[joint][1] + bz * ppdata->matrices[joint][ 2] + ppdata->matrices[joint][ 3];
	ty = bx * ppdata->matrices[joint][4] + by * ppdata->matrices[joint][5] + bz * ppdata->matrices[joint][ 6] + ppdata->matrices[joint][ 7];
	tz = bx * ppdata->matrices[joint][8] + by * ppdata->matrices[joint][9] + bz * ppdata->matrices[joint][10] + ppdata->matrices[joint][11];
	rhx = tx * tm[0] + ty * tm[4] + tz * tm[ 8] + cambot.x;
	rhy = tx * tm[1] + ty * tm[5] + tz * tm[ 9] + cambot.y;
	rhz = tx * tm[2] + ty * tm[6] + tz * tm[10] + cambot.z;

	bx = (L_INHAND_X_OFF * PSCALE);// * (1.0f + (1.0f - pobj->xscale));
	by = (L_INHAND_Y_OFF * PSCALE);// * (1.0f + (1.0f - pobj->xscale));
	//by = (L_INHAND_Y_OFF * PSCALE) * (1.0f + (1.0f - pobj->yscale));
	//by = ((L_INHAND_Y_OFF + 4.0f) * pobj->yscale) * PSCALE;
	bz = (L_INHAND_Z_OFF * PSCALE);// * (1.0f + (1.0f - pobj->zscale));
	joint = JOINT_LWRIST;
	tx = bx * ppdata->matrices[joint][0] + by * ppdata->matrices[joint][1] + bz * ppdata->matrices[joint][ 2] + ppdata->matrices[joint][ 3];
	ty = bx * ppdata->matrices[joint][4] + by * ppdata->matrices[joint][5] + bz * ppdata->matrices[joint][ 6] + ppdata->matrices[joint][ 7];
	tz = bx * ppdata->matrices[joint][8] + by * ppdata->matrices[joint][9] + bz * ppdata->matrices[joint][10] + ppdata->matrices[joint][11];
	lhx = tx * tm[0] + ty * tm[4] + tz * tm[ 8] + cambot.x;
	lhy = tx * tm[1] + ty * tm[5] + tz * tm[ 9] + cambot.y;
	lhz = tx * tm[2] + ty * tm[6] + tz * tm[10] + cambot.z;

	if( (pobj->adata[0].animode|pobj->adata[1].animode) & MODE_INTERP)// || pobj->adata[1].animode & MODE_INTERP)
	{ // going from SEQ to SEQ
	float thx,thy,thz,dist;

		// sometimes after a 2 part steal	pobj->adata[1].animode & MODE_INTERP never got cleared, and as a result the ball never left the dribblers hand
		if(pobj->adata[1].animode & MODE_INTERP && !(pobj->adata[1].animode & MODE_TWOPART))
			pobj->adata[1].animode &= ~MODE_INTERP;

		//printf("going from %s:%d to %s:%d\n",	seq_names[pobj->adata[0].seq_index2],pobj->adata[0].iframe, seq_names[pobj->adata[2].seq_index], pobj->adata[2].iframe);
		p[0] = hx = ppdata->ball_in_hand == RIGHT_HAND ? rhx : lhx;
		p[1] = hy = ppdata->ball_in_hand == RIGHT_HAND ? rhy : lhy;
		p[2] = hz = ppdata->ball_in_hand == RIGHT_HAND ? rhz : lhz;
		dist = dist3d(&(ball_obj.odata.x), p);

		if(dist3d(&(ball_obj.odata.x), p) > 0.05f && grab == NO)
		{
			ball_obj.odata.x = (ball_obj.odata.x + hx) * 0.5f;
			ball_obj.odata.y = (ball_obj.odata.y + hy) * 0.5f;
			ball_obj.odata.z = (ball_obj.odata.z + hz) * 0.5f;
		}
		else
		{
			ball_obj.odata.x = hx;
			ball_obj.odata.y = hy;
			ball_obj.odata.z = hz;
			new_hand = -1;
			ball_to_floor = -1;

	// SEQUENCES THAT REQUIRE THE BALL IN BOTH HANDS
			if(pobj->flags & PF_BOTH_HANDS)
			{
				thx = ppdata->ball_in_hand == RIGHT_HAND ? lhx : rhx;
				thy = ppdata->ball_in_hand == RIGHT_HAND ? lhy : rhy;
				thz = ppdata->ball_in_hand == RIGHT_HAND ? lhz : rhz;
				ball_obj.odata.x = (hx + thx) / 2.0f;
				ball_obj.odata.y = (hy + thy) / 2.0f;
				ball_obj.odata.z = (hz + thz) / 2.0f;
			}
		}

	}
	else //if(pobj->adata[0].animode & MODE_INTSTREAM || pobj->adata[0].animode & MODE_INTSTREAM)
	{// GOING THRU A SEQUENCE
		if((pobj->adata[0].animode|pobj->adata[1].animode) & MODE_INTSTREAM)	// || pobj->adata[0].animode & MODE_INTSTREAM)
		{
			if((pobj->adata[1].animode & MODE_TWOPART))
				ciframe = pobj->adata[1].fframe;
			else
				ciframe = pobj->adata[0].fframe;
		}
		else
		{
			if((pobj->adata[1].animode & MODE_TWOPART))
				ciframe = (float)pobj->adata[1].iframe;
			else
				ciframe = (float)pobj->adata[0].iframe;
		}	
		i = 0; s = -1.0f; e = -1.0f;

		for(i = 0; ddata[seq][i] != REPEAT; i++)
		{// where in the script should I be?
			if((ddata[seq][i] != FRAMES)) continue;
			s = (float)ddata[seq][i+1];
			fspread = s + ddata[seq][i+2];
			e = (float)fspread;
			if(fspread > nframes - 1.0f)
			{	// wrap...
				if((ciframe >= s) && (ciframe <= (float)nframes - 1.0f))
					ciframew = ciframe;
				else
					ciframew = s + ((float)nframes - s + ciframe);
			}
			else
			{
				ciframew = ciframe;	// < ((float)nframes - 1.0f) ? ciframe : s + ((float)nframes - s + ciframe);
			}

			if(ciframew < s || ciframew > e) continue;

			action = ddata[seq][i+4];
			if(action == B_RELEASE)
			{
				if(ciframew < s + (((e - s) * LAND_TIME)) )
				{
					hand = ddata[seq][i+3];
					new_hand = -1;
				}
				else
				{
					hand = new_hand = ddata[seq][i+7];
				}
			}// END.IF action = release
			else
			{
				hand = ddata[seq][i+3];
				new_hand = -1;
			}

			switch(hand)
			{
				case H_SWITCH:
					ppdata->ball_in_hand = 1 - ppdata->ball_in_hand;
					break;
				case H_LEFT:
					ppdata->ball_in_hand = LEFT_HAND;
					break;
				case H_RIGHT:
					ppdata->ball_in_hand = RIGHT_HAND;
					break;
				case H_SAME:
					break;
				default:
#ifdef DEBUG
					printf("DRIBBLER: error in hand control.");
					lockup();
#endif
					break;
			}

			if(action == B_RELEASE)
			{
			int angle = ddata[seq][i+5], distx = ddata[seq][i+6], distz = ddata[seq][i+6];

	#if 0
			// floor x,z deltas
				if(1)//pobj->vx != 0.0f || pobj->vz != 0.0f)
				{
					//fx = ball_obj.odata.x + pobj->vx;
					//fz = ball_obj.odata.z + pobj->vz;
					distx *= (pobj->xscale);
					distz *= (pobj->zscale);
					fx = pobj->x + (distx * isin((pobj->fwd + angle) % 1024) );
					fz = pobj->z + (distz * icos((pobj->fwd + angle) % 1024) );
				//fprintf(stderr,"angle %d, dist %d fx %3.2f, fz %3.2f\n",angle,dist,fx,fz);
				}
	#endif
	// set ball's velocity
				if(did_gravity == NO && UG == YES)
				{
					//fx = ball_obj.odata.x + pobj->vx;	//pobj->x + (dist * isin((pobj->fwd + angle) % 1024) );
					//fz = ball_obj.odata.z + pobj->vz;	//pobj->z + (dist * icos((pobj->fwd + angle) % 1024) );
					did_gravity = YES;
					if((pobj->adata[1].animode & MODE_TWOPART))
						rtime = pobj->adata[1].scnt;
					else
						rtime = pobj->adata[0].scnt;
					//ball.vy = ((ty - yob) / (float)time) - (0.5f * BALL_GRAVITY * (float)time);
					time = (((e + s) * LAND_TIME) - s) / pobj->adata[0].sinc;
					vy = ball_obj.odata.vy = ((BALL_RADIUS - ball_obj.odata.y) / time ) - (0.5f * BALL_GRAVITY * time);
					//vy = ball_obj.odata.vy = -(fsqrt(BALL_RADIUS + 2.0f * (GRAVITY*-1.0f) * hy));
					//fprintf(stderr, "A VELOCITY BEING SET\n");
				}

			//fprintf(stderr,"icnt %d, scnt %d, sinc %f\n",pobj->adata[0].icnt, pobj->adata[0].scnt, pobj->adata[0].sinc);
	//----------------------------------------------------
	// get new LERP'd y
				hd = ((e-s)*LAND_TIME);
				if(ciframew < s + hd && (ball_to_floor != NO))
				{
				// ball is going to floor
					if(ball_to_floor == -1 || dir_change)
					{
						joint = ppdata->ball_in_hand == RIGHT_HAND ? JOINT_RWRIST : JOINT_LWRIST;
						get_joint_position(pobj, sequence, s, joint, pobj->tgt_fwd, &rx, &temp, &rz);
						rx += pobj->x;
						rz += pobj->z;

						rx = ball_obj.odata.x;
						rz = ball_obj.odata.z;
					// floor x,z del as
						fx = pobj->x + ((distx * pobj->xscale) * isin((pobj->fwd + angle) % 1024) );
						fz = pobj->z + ((distz * pobj->zscale) * icos((pobj->fwd + angle) % 1024) );
						dir_change = NO;
					//fprintf(stderr,"angle %d, dist %d fx %3.2f, fz %3.2f\n",angle,dist,fx,fz);

					//--- get ball's return position
						//joint = ppdata->ball_in_hand == RIGHT_HAND ? JOINT_RWRIST : JOINT_LWRIST;
						//get_joint_position(pobj, sequence, (int)(s + fspread) % (nframes - 1), joint, pobj->tgt_fwd, &rx, &temp, &rz);
						//rx += pobj->x;
						//rz += pobj->z;
						//fprintf(stderr, "ball_to_floor:  RESET rx,rz -fx,fz\n");
					}
					fx += pobj->vx;	// floor x
					fz += pobj->vz;	// floor z
					rx += pobj->vx;	// return hand x
					rz += pobj->vz;	// return hand z

					ball_to_floor = YES;

					if(UG == NO)
					{
				// ball is using interpolated gravity
						hy = ppdata->ball_in_hand == RIGHT_HAND ? rhy : lhy;
						temp = ( ((ciframew-s)/hd) * (hy + (BALL_RADIUS * 0.5f)) );
						if(temp == 0.0f) temp = BALL_RADIUS * 0.5f;

						ball_obj.odata.y = (hy + (BALL_RADIUS * 0.5f) ) - temp;	//((ciframew-s)/hd) * hy;
					}
					else
					{
						ball_obj.odata.y += ball_obj.odata.vy;
						ball_obj.odata.vy += GRAVITY;
						if( ball_obj.odata.y <= BALL_RADIUS )
						{
							ball_obj.odata.y = BALL_RADIUS;
							//fprintf(stderr, "BALL IS THROUGH FLOOR, REVERSING DIR\n");
							// dribble sound
							//make_dribble1_snd(VOLUME1);
							make_dribble2_snd(VOLUME2);
							ball_to_floor = NO;
						}

						if(ball_obj.odata.vy > 0.0f)
							_did_gravity = YES;
						else
							_did_gravity = NO;
					}

					if(1)//_did_gravity == NO)
					{
		 				ball_obj.odata.x = rx + ((ciframew-s)/hd) * (fx - rx);
	 					ball_obj.odata.z = rz + ((ciframew-s)/hd) * (fz - rz);	//	ball_obj.odata.z = hz + ((ciframew-s)/hd) * (fz - hz);
					}
	#ifdef DEBUG_DRIBBLE
		delete_multiple_strings(DEBUG_ID-1, 0xffffffff);
		set_text_font(FONTID_NBA10);
		set_text_transparency_mode(TRANSPARENCY_ENABLE);
		set_string_id(DEBUG_ID-1);

		set_text_position(240, 376, 1.2f);
		oprintf("%dj%dcTO FLOOR:", (HJUST_RIGHT|VJUST_CENTER), LT_CYAN);
		set_text_position(240, 364, 1.2f);
		oprintf("%dj%dcfx%dc %3.2f  %dcfz%dc %3.2f", (HJUST_RIGHT|VJUST_CENTER), LT_CYAN, LT_YELLOW, fx, LT_CYAN, LT_YELLOW, fz );

		set_text_position(240, 352, 1.2f);
		oprintf("%dj%dcrx%dc %3.2f  %dcrz%dc %3.2f", (HJUST_RIGHT|VJUST_CENTER), LT_CYAN, LT_YELLOW, rx, LT_CYAN, LT_YELLOW, rz );

		set_text_position(240, 340, 1.2f);
		oprintf("%dj%dcbx%dc %3.2f  %dcbz%dc %3.2f  %dcby%dc %3.2f %3.2f", (HJUST_RIGHT|VJUST_CENTER), LT_CYAN, LT_YELLOW, ball_obj.odata.x, LT_CYAN, LT_YELLOW, ball_obj.odata.z, LT_CYAN, LT_YELLOW, ball_obj.odata.y, ball_obj.odata.vy );
		set_text_position(360, 328, 1.2f);
		oprintf("%dj%dccif%dc %3.2f  %dccifw%dc %3.2f  %dcs%dc %3.2f  %dce%dc %3.2f  %dchd%dc %3.2f", (HJUST_RIGHT|VJUST_CENTER),
			LT_CYAN, LT_YELLOW, ciframe, LT_CYAN, LT_YELLOW, ciframew, LT_CYAN, LT_YELLOW, s, LT_CYAN, LT_YELLOW, e, LT_CYAN, LT_YELLOW, s + hd );
	#endif
				}
				else
				{
				// ball is going from floor
					if(ball_to_floor == NO || ball_to_floor == YES)
						ball_to_floor = -2;	// this is a flag to get balls' return x,z position
					rel = NO;

					if(dir_change || ball_to_floor == -2)
					{
						grab = NO;
						joint = ppdata->ball_in_hand == RIGHT_HAND ? JOINT_RWRIST : JOINT_LWRIST;
						if(ppdata->ball_in_hand == RIGHT_HAND)
						{
							get_joint_position_rel(pobj, sequence, fspread % (nframes - 1), joint, pobj->tgt_fwd, &rx, &ry, &rz, (R_INHAND_X_OFF * 1.0f) * PSCALE, (R_INHAND_Y_OFF * 1.0f) * PSCALE, (R_INHAND_Z_OFF * 1.0f) * PSCALE);
						}
						else
						{
							get_joint_position_rel(pobj, sequence, fspread % (nframes - 1), joint, pobj->tgt_fwd, &rx, &ry, &rz, (L_INHAND_X_OFF * 1.0f) * PSCALE, (L_INHAND_Y_OFF * 1.0f) * PSCALE, (L_INHAND_Z_OFF * 1.0f) * PSCALE);
						}

						rx += pobj->x;
						rz += pobj->z;

					// floor x,z deltas
						fx = pobj->x + (distx * isin((pobj->fwd + angle) % 1024) );
						fz = pobj->z + (distz * icos((pobj->fwd + angle) % 1024) );
						dir_change = NO;
						if(ball_to_floor == -2)
						{
							ball_to_floor = -3;	// -3 means balls' return position has been gotten!
							//make_dribble1_snd(VOLUME1);
							make_dribble2_snd(VOLUME2);
						}
					}

					fx += pobj->vx;	// floor x
					fz += pobj->vz;	// floor z
					rx += pobj->vx;
					rz += pobj->vz;

					hy = ppdata->ball_in_hand == RIGHT_HAND ? rhy : lhy;
					temp = ((ciframew - (s + hd)) / ((e - s) - hd));

#ifdef DEGUG
					p[0] = hx = ppdata->ball_in_hand == RIGHT_HAND ? rhx : lhx;
					p[1] = hy = ppdata->ball_in_hand == RIGHT_HAND ? rhy : lhy;
					p[2] = hz = ppdata->ball_in_hand == RIGHT_HAND ? rhz : lhz;
					if(temp >= 0.4f && temp <= 0.6f && dist3d(&(ball_obj.odata.x), p) <= 1.0f)
//						while(!(get_player_sw_current()&ALL_ABCD_MASK))scan_switches();				// Cool!  Debug pause game until butn is hit!!!
#endif

					if(UG == NO)
					{
						ball_obj.odata.y = (BALL_RADIUS * 0.5f) + (temp * hy) - (temp == 1.0f ? BALL_RADIUS : 0.0f);
					}
					else
					{
						if(_did_gravity == NO)
						{
							vy = ball_obj.odata.vy = ((ry - BALL_RADIUS) / time ) - (0.5f * BALL_GRAVITY * time);
							_did_gravity = YES;
						}
						if(grab == NO)
						{
							ball_obj.odata.y += ball_obj.odata.vy;
							ball_obj.odata.vy += GRAVITY;
						}
					}
					ball_obj.odata.x = fx + ((ciframew - (s + hd)) / ((e - s) - hd)) * (rx - fx);
					ball_obj.odata.z = fz + ((ciframew - (s + hd)) / ((e - s) - hd)) * (rz - fz);

				//if ball is close enough to the dribbling hand grab it...
//	#if 0
					if(UG == YES)
					{
						p[0] = ppdata->ball_in_hand == RIGHT_HAND ? rhx : lhx;
						p[1] = ppdata->ball_in_hand == RIGHT_HAND ? rhy : lhy;
						p[2] = ppdata->ball_in_hand == RIGHT_HAND ? rhz : lhz;
						if(dist3d(&(ball_obj.odata.x), p) <= 1.75f || grab == YES)
						{
							grab = YES;
							if(dist3d(&(ball_obj.odata.x), p) <= 1.75f)
							{
	#if 0
								ball_obj.odata.x = p[0];
								ball_obj.odata.y = p[1];
								ball_obj.odata.z = p[2];
	#else
							joint = ppdata->ball_in_hand == RIGHT_HAND ? JOINT_RWRIST : JOINT_LWRIST;
							if(ppdata->ball_in_hand == RIGHT_HAND)
								get_joint_position_rel(pobj, sequence, (int)ciframe, joint, pobj->tgt_fwd, &p[0], &p[1], &p[2], (R_INHAND_X_OFF * pobj->xscale) * PSCALE, (R_INHAND_Y_OFF * pobj->yscale) * PSCALE, (R_INHAND_Z_OFF * pobj->zscale) * PSCALE);
								//get_joint_position_rel(pobj, sequence, (int)ciframe, joint, pobj->tgt_fwd, &p[0], &p[1], &p[2], (R_INHAND_X_OFF * 1.0f) * PSCALE, (R_INHAND_Y_OFF * 1.0f) * PSCALE, (R_INHAND_Z_OFF * 1.0f) * PSCALE);
							else
								get_joint_position_rel(pobj, sequence, (int)ciframe, joint, pobj->tgt_fwd, &p[0], &p[1], &p[2], (L_INHAND_X_OFF * pobj->xscale) * PSCALE, (L_INHAND_Y_OFF * pobj->yscale) * PSCALE, (L_INHAND_Z_OFF * pobj->zscale) * PSCALE);
								ball_obj.odata.x = p[0] + pobj->x;
								ball_obj.odata.y = p[1];
								ball_obj.odata.z = p[2] + pobj->z;
		//printf("ciframe %d, joint %d, p[0] %3.3f\np[1] %3.3f p[2] %3.3f\nrhx %3.3f rhy %3.3f rhz %3.3f\n",(int)ciframe, joint, p[0] + pobj->x, p[1] * pobj->yscale, p[2] + pobj->z, rhx,rhy,rhz);
	#endif
							}
							else
							{
								ball_obj.odata.x = (ball_obj.odata.x + p[0]) * 0.5f;
								ball_obj.odata.y = (ball_obj.odata.y + p[1]) * 0.5f;
								ball_obj.odata.z = (ball_obj.odata.z + p[2]) * 0.5f;
							}
						}
					}	
//	#endif

	#ifdef DEBUG_DRIBBLE
		delete_multiple_strings(DEBUG_ID-1, 0xffffffff);
		set_text_font(FONTID_NBA10);
		set_text_transparency_mode(TRANSPARENCY_ENABLE);
		set_string_id(DEBUG_ID-1);

		set_text_position(240, 376, 1.2f);
		oprintf("%dj%dcFROM FLOOR:", (HJUST_RIGHT|VJUST_CENTER), LT_CYAN);
		set_text_position(240, 364, 1.2f);
		oprintf("%dj%dcfx%dc %3.2f  %dcfz%dc %3.2f", (HJUST_RIGHT|VJUST_CENTER), LT_CYAN, LT_YELLOW, fx, LT_CYAN, LT_YELLOW, fz );

		set_text_position(240, 352, 1.2f);
		oprintf("%dj%dcrx%dc %3.2f  %dcrz%dc %3.2f", (HJUST_RIGHT|VJUST_CENTER), LT_CYAN, LT_YELLOW, rx, LT_CYAN, LT_YELLOW, rz );

		set_text_position(240, 340, 1.2f);
		oprintf("%dj%dcbx%dc %3.2f  %dcbz%dc %3.2f  %dcby%dc %3.2f %3.2f", (HJUST_RIGHT|VJUST_CENTER), LT_CYAN, LT_YELLOW, ball_obj.odata.x, LT_CYAN, LT_YELLOW, ball_obj.odata.z, LT_CYAN, LT_YELLOW, ball_obj.odata.y, ball_obj.odata.vy );
		set_text_position(360, 328, 1.2f);
		oprintf("%dj%dccif%dc %3.2f  %dccifw%dc %3.2f  %dcs%dc %3.2f  %dce%dc %3.2f  %dchd%dc %3.2f", (HJUST_RIGHT|VJUST_CENTER),
			LT_CYAN, LT_YELLOW, ciframe, LT_CYAN, LT_YELLOW, ciframew, LT_CYAN, LT_YELLOW, s, LT_CYAN, LT_YELLOW, e, LT_CYAN, LT_YELLOW, s + hd );
		set_text_position(240, 316, 1.2f);
		oprintf("%dj%dcdist%dc %3.2f grab %d", (HJUST_RIGHT|VJUST_CENTER), LT_CYAN, LT_YELLOW, dist3d(&(ball_obj.odata.x), p), grab );
	#endif
				}
			}//END.if.B_RELEASE
			if(action == HOLD)
			{
			float thx, thy, thz, dist;

				p[0] = hx = ppdata->ball_in_hand == RIGHT_HAND ? rhx : lhx;
				p[1] = hy = ppdata->ball_in_hand == RIGHT_HAND ? rhy : lhy;
				p[2] = hz = ppdata->ball_in_hand == RIGHT_HAND ? rhz : lhz;
				dist = dist3d(&(ball_obj.odata.x), p);

				if(dist > 0.05f && grab == NO)
				{
#if 0
					joint = ppdata->ball_in_hand == RIGHT_HAND ? JOINT_RWRIST : JOINT_LWRIST;
					if(ppdata->ball_in_hand == RIGHT_HAND)
						//get_joint_position_rel(pobj, sequence, (int)ciframe, joint, pobj->tgt_fwd, &hx, &hy, &hz, (R_INHAND_X_OFF * 1.0f) * PSCALE, (R_INHAND_Y_OFF * 1.0f) * PSCALE, (R_INHAND_Z_OFF * 1.0f) * PSCALE);
						get_joint_position_rel(pobj, sequence, (int)ciframe, joint, pobj->tgt_fwd, &hx, &hy, &hz, (R_INHAND_X_OFF * pobj->xscale) * PSCALE, (R_INHAND_Y_OFF * pobj->yscale) * PSCALE, (R_INHAND_Z_OFF * pobj->zscale) * PSCALE);
					else
						get_joint_position_rel(pobj, sequence, (int)ciframe, joint, pobj->tgt_fwd, &hx, &hy, &hz, (L_INHAND_X_OFF * pobj->xscale) * PSCALE, (L_INHAND_Y_OFF * pobj->yscale) * PSCALE, (L_INHAND_Z_OFF * pobj->zscale) * PSCALE);
					ball_obj.odata.x = hx + pobj->x;
					ball_obj.odata.y = hy;
					ball_obj.odata.z = hz + pobj->z;
#else
					ball_obj.odata.x = ball_obj.odata.x + ((hx - ball_obj.odata.x) * 0.75f);
					ball_obj.odata.y = ball_obj.odata.y + ((hy - ball_obj.odata.y) * 0.75f);
					ball_obj.odata.z = ball_obj.odata.z + ((hz - ball_obj.odata.z) * 0.75f);
#endif
					#if 0
					if(grab == YES)
					{
						ball_obj.odata.x = hx;
						ball_obj.odata.y = hy;
						ball_obj.odata.z = hz;
					}
					#endif
				}
				else
				{
					ball_obj.odata.x = hx;
					ball_obj.odata.y = hy;
					ball_obj.odata.z = hz;

#if 0
					joint = ppdata->ball_in_hand == RIGHT_HAND ? JOINT_RWRIST : JOINT_LWRIST;
					if(ppdata->ball_in_hand == RIGHT_HAND)
						//get_joint_position_rel(pobj, sequence, (int)ciframe, joint, pobj->tgt_fwd, &hx, &hy, &hz, (R_INHAND_X_OFF * 1.0f) * PSCALE, (R_INHAND_Y_OFF * 1.0f) * PSCALE, (R_INHAND_Z_OFF * 1.0f) * PSCALE);
						get_joint_position_rel(pobj, sequence, (int)ciframe, joint, pobj->tgt_fwd, &hx, &hy, &hz, (R_INHAND_X_OFF * pobj->xscale) * PSCALE, (R_INHAND_Y_OFF * pobj->yscale) * PSCALE, (R_INHAND_Z_OFF * pobj->zscale) * PSCALE);
					else
						get_joint_position_rel(pobj, sequence, (int)ciframe, joint, pobj->tgt_fwd, &hx, &hy, &hz, (L_INHAND_X_OFF * pobj->xscale) * PSCALE, (L_INHAND_Y_OFF * pobj->yscale) * PSCALE, (L_INHAND_Z_OFF * pobj->zscale) * PSCALE);
					ball_obj.odata.x = hx + pobj->x;
					ball_obj.odata.y = hy;
					ball_obj.odata.z = hz + pobj->z;
#endif
				}
	// animations that require ball in both hands
				if(sequence == (SEQ_RDY_WB) || sequence == (SEQ_PICKUP_1)
				|| sequence == (SEQ_TRDY_R)
				//|| sequence == (SEQ_TRDY_R) || sequence == (SEQ_TRDY_L)
				|| sequence == (SEQ_C_SID6_R) || sequence == (SEQ_C_SID6_L)
				|| sequence == (SEQ_C_FWD1_B) || sequence == (SEQ_C_FWD1_B)
				|| ppdata->odata.flags & PF_BOTH_HANDS
				)
				{
					thx = ppdata->ball_in_hand == RIGHT_HAND ? lhx : rhx;
					thy = ppdata->ball_in_hand == RIGHT_HAND ? lhy : rhy;
					thz = ppdata->ball_in_hand == RIGHT_HAND ? lhz : rhz;
					ball_obj.odata.x = (hx + thx) / 2.0f;
					ball_obj.odata.y = (hy + thy) / 2.0f;
					ball_obj.odata.z = (hz + thz) / 2.0f;
				}

				//rx = rz = -1000.0f;
				did_gravity = _did_gravity = NO;// = grab = NO;
				new_hand = -1;
				ball_to_floor = -1;

	#ifdef DEBUG_DRIBBLE
		delete_multiple_strings(DEBUG_ID-1, 0xffffffff);
		set_text_font(FONTID_NBA10);
		set_text_transparency_mode(TRANSPARENCY_ENABLE);
		set_string_id(DEBUG_ID-1);

		set_text_position(240, 376, 1.2f);
		oprintf("%dj%dcHOLD- btf %d, dist %f g%d", (HJUST_RIGHT|VJUST_CENTER), LT_CYAN, ball_to_floor, dist3d(&(ball_obj.odata.x), p), grab);
		//set_text_position(40, 364, 1.2f);
		//oprintf("%dj%dcfx%dc %3.2f  %dcfz%dc %3.2f", (HJUST_RIGHT|VJUST_CENTER), LT_CYAN, LT_YELLOW, fx, LT_CYAN, LT_YELLOW, fz );
		//set_text_position(240, 352, 1.2f);
		//oprintf("%dj%dcbx%dc %3.2f  %dcbz%dc %3.2f  %dcby%dc %3.2f", (HJUST_RIGHT|VJUST_CENTER), LT_CYAN, LT_YELLOW, ball_obj.odata.x, LT_CYAN, LT_YELLOW, ball_obj.odata.z, LT_CYAN, LT_YELLOW, ball_obj.odata.y );
	#endif
			}//END.if.HOLD	
			if(ciframew >= s && ciframew <= e) break;
		}//END.for
	}
}

