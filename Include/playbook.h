/*
 *		$Archive: /video/Nba/Include/playbook.h $
 *		$Revision: 2 $
 *		$Date: 4/21/98 12:25a $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifndef __PLAYBOOK_H__
#define __PLAYBOOK_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_playbook_h = "$Workfile: playbook.h $ $Revision: 2 $";
#endif

// formation element
typedef struct _fline
{
	float	x,z;
	char	*seq;
} fline_t;

// play struct
typedef struct _play
{
	//fline_t	*formation;
	int		flags;
	int		*routes[3];
} play_t;

// drone script commands

enum
{
	DS_WAIT_TICK	=	1,
	DS_CALL_BALL,
	DS_PASS_BALL,
	DS_GOTO,
	DS_SHOOTBALL,
	DS_SET_PICK,
	DS_WAIT_FOR_PICK,
	DS_ROLL_TO_BASKET,
	DS_IMPROVISE,
	DS_PENETRATE,
	DS_ROAM_3PT_LINE,
	DS_AVOID_PLAYER,
	DS_WAIT_PLAYER_TO_STOP,
	DS_WAIT_PLAYER_DIST,
	DS_WAIT_PLAYER_ANGLE,
	DS_WAIT_PLAYER_TO_POINT,
	DS_RUN_AROUND,
	DS_RESET_SCRIPT
};

#define DS_WAIT_SNAP		1
#define DS_FACE				2
#define DS_GOTO_LOS_PLUS	3
#define DS_RUN_YDS			4
#define DS_IMPROVISE		5
#define DS_ASSIGN_COVERAGE	6
#define DS_COVER_TGT		7
#define DS_MOTION			8
#define DS_RUSH_QB			9

#define DA_MAN2MAN			1

#endif
