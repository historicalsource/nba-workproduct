/*
 *		$Archive: /video/Nba/playbook.c $
 *		$Revision: 2 $
 *		$Date: 4/20/98 11:54p $
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
char *ss_playbook_c = "$Workfile: playbook.c $ $Revision: 2 $";
#endif

/*
 *		USER INCLUDES
 */

#include "include/playbook.h"
#include "include/plyrseq.h"


// offensive formations

fline_t for_pro_set[] =
{
	{-9.0, -17.5,stance3pt_anim},  	// left guard
	{-9.0, 0.0,center_set_anim},	// center
	{-9.0, 17.5,stance3pt_anim},  	// right guard
	{-15.5, 0.0,qbset_anim},  		// QB
	{-32.5, 10.0,wr_set_anim},  	// RB
	{-13.5, 60.0,wr_set_anim},		// WR1
	{-13.5, -60.0,wr_set_anim},		// WR2
};

fline_t for_shotgun[] =
{
	{-9.0, -17.5,stance3pt_anim},  	// left guard
	{-9.0, 0.0,center_set_anim},	// center
	{-9.0, 17.5,stance3pt_anim},  	// right guard
	{-47.5, 0.0,qbset_anim},  		// QB
	{-8.0, 30.0,wr_set_anim},  		// WR1 (TE)
	{-13.5, 75.0,wr_set_anim},		// WR2
	{-13.5, -60.0,wr_set2_anim},	// WR3
};

// defensive formations

fline_t for_std_def[] = 
{
	{-9.0, -17.5,stance3pt_anim},  	// LINE1
	{-9.0, 0.0,stance3pt_anim},	  	// LINE2
	{-9.0, 17.5,stance3pt_anim},  	// LINE3
	{-20.0, -30.0,stance2pt_anim_rnd},	// DB1
	{-20.0, 30.0,stance2pt_anim_rnd},  	// DB2
	{-29.5, 75.0,stance2pt_anim_rnd},	// DB3
	{-29.5, -60.0,stance2pt_anim_rnd},	// DB4
};

// pass routes

int pr_hitch[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,10,
	DS_FACE,256+64,
	DS_RUN_YDS,10,
	DS_FACE,256,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

int pr_slant[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,10,
	DS_FACE,256-64,
	DS_RUN_YDS,20,
	DS_IMPROVISE
};

int pr_motion_slant[] =
{
	DS_MOTION,
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,10,
	DS_FACE,256-64,
	DS_RUN_YDS,20,
	DS_IMPROVISE
};

int pr_drive[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,15,
	DS_FACE,256-64,
	DS_RUN_YDS,20,
	DS_IMPROVISE
};

int pr_post[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,20,
	DS_FACE,128,
	DS_RUN_YDS,20,
	DS_IMPROVISE
};

int pr_short_out[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,10,
	DS_FACE,-(265+64),
	DS_RUN_YDS,5,
	DS_IMPROVISE
};

// defensive back assignments
int db_man_c[] =
{
	DS_ASSIGN_COVERAGE,DA_MAN2MAN,
	DS_WAIT_SNAP,
	DS_COVER_TGT
};

int db_man[] =
{
	DS_WAIT_SNAP,
	DS_COVER_TGT
};

int db_rush_qb[] = 
{
	DS_WAIT_SNAP,
	DS_RUSH_QB
};

// misc drone scripts
int ds_none[] =
{
	DS_IMPROVISE,
};

// pass plays
play_t play_quick_outs =
{
	for_shotgun,
	{ds_none,ds_none,ds_none,
	ds_none,
	pr_slant,pr_short_out,pr_post}
};

play_t play_cross_pass =
{
	for_pro_set,
	{ds_none,ds_none,ds_none,
	ds_none,
	pr_motion_slant,pr_short_out,pr_post}
};

// defense plays
play_t play_three_man =
{
	for_std_def,
	{ds_none,ds_none,ds_none,
	db_man,db_rush_qb,db_man,db_man_c}
};

