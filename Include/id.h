/*
 *		$Archive: /video/Nba/Include/id.h $
 *		$Revision: 59 $
 *		$Date: 4/23/99 11:48a $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifndef __ID_H__
#define __ID_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_id_h = "$Workfile: id.h $ $Revision: 59 $";
#endif

//-----------------------------------------------------------------------------
//
//  General NBA ID's (procss, texture, string, etc.)
//
//-----------------------------------------------------------------------------
//  PID - Process ID's 
//-----------------------------------------------------------------------------
#define BONUS_PID           0x0F9e				// Bonus message on yes/no scrn

#define	FREETHROW_METER_PID	0x6666

#define	SPEECH_PID			0x6660
#define NODESTRUCT      	0x8000
#define	GAME_GRP_PID		0x0000				// General game group process ids
#define GAME_PROC_PID       0x0010				// game_proc
#define	AMODE_GRP_PID		0x0100				// Attract mode process group ids
#define	CC_GRP_PID			0x0200				// Coin counter process group ids
#define	VOLUME_GRP_PID		0x0300				// Volume adjust process id
#define	COIN_IN_GRP_PID		0x0400				// Coin drop sound process
#define PLAYER_PID			0x0500
#define REFEREE_PID			0x0550
#define SPFX_PID 	        0x0580              // special effects...
#define FIELD_PID			0x0600
#define STATUS_BOX_PID		0x0700
#define SCR_PLATE_PID		0x0800
#define BALL_PID			0x0900
#define CAMBOT_PID			0x0990
//#define COLLIS_PID			0x0A00			//Unused!!! just to be sure -DJT
#define REPLAY_PID			0x0B00
#define PRE_GAME_SCRNS_PID  0x0C00
#define SMOKE_PID           0x0D00
#define AUDIBLE_PID			0x0E00
#define GAME_CLOCK_PID		0x0F00
#define MESSAGE_PID         0x0F10					// Display big messages process
#define PLYRINFO_PID		0x0F80
#define PLYR_CURSOR_PID		0x0F88					// Trivia cursor processes
#define VIOLATION_PID		0x0F90					// turnovers(GoalTending, ShotClock Violation)
#define IDIOT_BOX_PID		0x0F9a					// Idiot boxes
#define EOQ_PID             0x0F9c					// EOQ message proc
#define CREDIT_PAGE_ID      0x5600
#define STREAM_PID			0x1100					// streaming process
#define FADE_PID			0x1200					// stream fader process
#define JUMP_METER_PID		0x1300					// stream fader process
#define INGAME_BUYIN_PID	0x1400
#define SHOT_PERCENTAGE_PID	0x0F95					// PUP shot percentage
#define FULLGAME_BOUGHT_PID	(0x45aa+NODESTRUCT)		// Transition proc
#define TRANSIT_PID			(0x55aa+NODESTRUCT)		// Transition proc
#define DEBUG_SW_PID		(0x6000+NODESTRUCT)		// Debug switches proc
#define BACKDOOR_PID		(0x7000+NODESTRUCT)		// Backdoor proc

#define BUYIN_PLAYER_PID	0x0BBB

#define SIMPLE_CYCLE_PROC_ID  0x5555				// Hiscore list color cycler

//-----------------------------------------------------------------------------
//  TID - Texture ID's
//-----------------------------------------------------------------------------
#define TXID_PLAYER			1   // moved from \\video\include\goose\texture.h
#define TXID_COURT			2
#define TXID_STADIUM		3
#define TXID_PLRSHDW		4
#define TXID_HEADS			5
#define TXID_REFEREE		6

#define P1_YN_PLAQ_TID		20000   // leave 4 (1 for each player)
#define P2_YN_PLAQ_TID      20005   // leave 4 (1 for each player)
#define P3_YN_PLAQ_TID      20010   // leave 4 (1 for each player)
#define P4_YN_PLAQ_TID      20015   // leave 4 (1 for each player)

#define YES_NO_PLAQ_TID		20020
#define PLAQ_CURSORS_TID    20021
#define JAPPLE_BOX_TID      20022
#define TM_SEL_LOGOS_TID    20023
#define TM_SELECT_TID	 	20024
#define	STATS_PG_TID	    20025
#define	PLAY_SELECT_TID	    20026
#define	NFL_LOGO_TID		20027
#define	PLYR_SELECT_TID		20029
#define HISCORE_PG_TID		20030
#define	BARS_PG_TID         20031
#define	OPT_SCR_TID         20032
#define	MUGSHOT2_TID        20033
#define	MUG_TID  		    20033
#define	BKGRND_TID        	20034
#define	BKGRND2_TID        	20035

#define PLYRINFO_TID		21001				// Game-time player info tid
#define SCR_PLATE_TID		21002				// Game-time score plate tid

#define PLYRINFO_TID		21001				// Game-time player info tid
#define SCR_PLATE_TID		21002				// Game-time score plate tid
#define MESSAGE_TID	    	21003				// Game-time big messages tid
#define MOVIE_TID           21004				// movie loading textures
#define TRANS_TID			21005				// transition plates
#define CAP_TID				0x6000				// create-a-play ID base
#define CAP_TOP_TID			0x63FF				// everything between these two reserved for CAP
#define JOININ_TID			21010				// stuff for in-game joinin

#define SPFX_TID            10                  // id for the SPECIAL EFFECTS...smoke..etc
#define EOQ_TID				20
#define CNTDWN_TID          30                  // id for the countdown digits


//-----------------------------------------------------------------------------
//  SID - String ID's
//-----------------------------------------------------------------------------
#define	CREDIT_MSGS_ID		60					// & next 3 for "Credit" msgs

#define	STAT_PG_TXT_ID		85

#define SHOTCLOCK_TEXT_ID		0x07
#define SCORE_STATUSBOX_TEXT_ID	0x06
#define SCOREPLATE_TXT_ID		0x08
#define BIG_SCOREPLATE_TXT_ID	0x09
#define SCOREPLATE_CLOCK_TXT_ID	0x20		// For the clock portion of scoreplate
#define SHOT_DISPLAY_TXT_ID		0xbee0		// for pup_display_shot_percentage

#define SID_TLCLOCK			0x0006
#define SID_TLSCORES		0x0005
#define SID_TLNAMES			0x0004

#define	SID_FTPER			800					// show fg percentage text
#define	SID_FTMTR			801					// fg meter text


//-----------------------------------------------------------------------------
//  MID - Message ID's
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  OID - Object ID's
//-----------------------------------------------------------------------------

#define	OID_CNTDWN_DIGIT	0x580

#define OID_JAPPLE_BOX		0x250
#define OID_TEAM1			0x350
#define OID_TEAM2			0x450

#define OID_VERSUS		5000
#define	OID_STAT_PG		55				// leave 4 after this for (plyr + 1)
#define	OID_STAT2_PG	60				// leave 4 after this for (plyr + 1)
#define	OID_STAT3_PG	65				// leave 4 after this for (plyr + 1)

#define OID_P1PLAQ		0x550
#define OID_P2PLAQ		0x551
#define OID_P3PLAQ		0x552
#define OID_P4PLAQ		0x553

#define OID_P1AWARD		0x570
#define OID_P2AWARD		0x571
#define OID_P3AWARD		0x572
#define OID_P4AWARD		0x573

#define OID_P1ALPHA		0x650
#define OID_P2ALPHA		0x651
#define OID_P3ALPHA		0x652
#define OID_P4ALPHA		0x653

#define OID_JUMP_METER	0x1000				// allow 4 for this id

// 2D & 3D object ID's
#define OID_FIELD		0x100
#define OID_CURSOR		OID_FIELD|0x1
#define OID_GOALS		OID_FIELD|0x2
#define OID_PLAYER		0x200
#define OID_HOTSPOT_CURSOR		0x220
#define OID_REFEREE		0x240
#define OID_PRESHADOW	0x280
#define OID_SHADOW		0x290
#define OID_STADIUM		0x300
#define OID_BALL		0x400
#define OID_MISC		0x500
#define OID_SMKPUFF		0x510
#define OID_AMODE		0x600
#define OID_PLYRINFO	0x700
#define OID_SCRPLATE	OID_PLYRINFO|1
#define OID_INFOBOX 	OID_PLYRINFO|2
#define OID_RESULTBOX 	OID_PLYRINFO|4
#define OID_TURBARROW  	OID_PLYRINFO|8
#define OID_MESSAGE		0x800               // Big messages during gameplay                
#define OID_MESSAGE2	0x801               // Clock low
#define OID_GT_MESSAGE	0x802
#define OID_SC_MESSAGE	0x803
#define OID_MESSAGE3	0x810               // Fumble background
#define OID_IDIOTBOX	0x820               // Fumble background
#define OID_P1CURSOR	0x900
#define OID_P2CURSOR	0x901
#define OID_BUYIN_PLYR	0x902
#define OID_3PT_MSG		0x903				// 3pts scored message
#define OID_TMFIRE_MSG	0x904				// TM fire txt message

#define OID_FOUL_MESSAGE 0x910				// 910-922 used for FLAGRANT FOUL! msg

#define	OID_FTPER		810					// show fg percentage text
#define	OID_FTMTR		811					// fg meter text

#define OID_JOININ		0x980				// mid-game joinin base id
#define OID_JOININ_BK	OID_JOININ+1
#define OID_JOININ_BTN	OID_JOININ+2
#define OID_JOININ_BFSH	OID_JOININ+3



#define	AMODE_GRP_OID	(0<<8)

#define	AMODE_CREDPROC_OID			(AMODE_GRP_OID|1)
#define	AMODE_CREDPROC_CREDITS_STRING_OID	(AMODE_GRP_OID|2)

#define	OID_NBR3D		0x700
#define OID_REPLAY		0x800

#define OID_GC_BG		0xb000					// grand champ ID's
#define OID_GC_FG		0xf000

//-----------------------------------------------------------------------------
//  DRAWORDER - 3D object draw sequence
//-----------------------------------------------------------------------------

#define DRAWORDER_FOGON		0x00001000
#define	DRAWORDER_FIELD		0x00010000
#define	DRAWORDER_FIELDFX	0x00020000
#define DRAWORDER_PRESHADOW	0x00020100
#define DRAWORDER_SHADOW	0x00020200
#define	DRAWORDER_REPLAY	0x00030000
#define	DRAWORDER_PLAYER	0x00040000
#define DRAWORDER_FOGOFF	0x00041000
#define	DRAWORDER_BALL		0x00050000
#define	DRAWORDER_GOALS		0x00060000
#define	DRAWORDER_PTL		0x000f0000

//-----------------------------------------------------------------------------
//  RL - Process run levels
//  This ordering (some of it, anyway) is now fairly critical!
//  Danger! Danger! see DJT
//-----------------------------------------------------------------------------
#define RL_FIRSTUP		0x100
#define RL_JMETER		0x180
#define RL_PLAYER		0x200
#define RL_CAMBOT		0x300
#define RL_LASTPLAYER	0x400
#define RL_COLLIS		0x500
#define RL_BALL			0x600
#define RL_ARROWS		0x700
#define RL_REPLAY		0x800
#define RL_LASTUP		0x900

//-----------------------------------------------------------------------------

#endif
