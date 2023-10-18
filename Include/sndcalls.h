/*
 *		$Archive: /video/Nba/Include/sndcalls.h $
 *		$Revision: 52 $
 *		$Date: 3/27/99 5:56p $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifndef __SNDCALLS_H__
#define __SNDCALLS_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_sndcalls_h = "$Workfile: sndcalls.h $ $Revision: 52 $";
#endif

//
// PRIORITIES
//
#define	PRIORITY_LVL0		20  //0             // lowest...
#define	PRIORITY_LVL1		15  //1
#define	PRIORITY_LVL2		10  //5
#define	PRIORITY_LVL3		5   //10
#define	PRIORITY_LVL4		1   //15
#define	PRIORITY_LVL5		0   //20            // highest priority

// defines for SPEECH
#define	LVL0	SND_PRI_SET_NOINT|SND_PRI_TRACK5|PRIORITY_LVL0
#define	LVL1	SND_PRI_SET_NOINT|SND_PRI_TRACK5|PRIORITY_LVL1
#define	LVL2	SND_PRI_SET_NOINT|SND_PRI_TRACK5|PRIORITY_LVL2
//#define	LVL2	SND_PRI_SET|SND_PRI_TRACK5|PRIORITY_LVL2
#define	LVL3	SND_PRI_SET_NOINT|SND_PRI_TRACK5|PRIORITY_LVL3
#define	LVL4	SND_PRI_SET_NOINT|SND_PRI_TRACK5|PRIORITY_LVL4
#define	LVL5	SND_PRI_SET_NOINT|SND_PRI_TRACK5|PRIORITY_LVL5


//
// SOUND CALLS
//
// BASIC LIST OF SOUNDS
#define CROWD_BOO_SND					2
#define CROWD_OH_SND					7
#define CROWD1_LOOP1_SND				12
#define CROWD1_LOOP2_SND				20		// Outdoor - no weather
#define CROWD1_LOOP3_SND				30		// Outdoor - no weather
#define CROWD1_STOMP_SND				3		// Pre tip stomp
#define CROWD2_SWELL1_SND				16
#define CROWD2_SWELL2_SND				8		// Ambient crowd
#define CROWD2_SWELL3_SND				9		// Ambient crowd
#define CROWD1_YELL_DEFENSE				6	
#define CROWD1_YEAH_SND					5
#define CROWD_GO_TM_GO_SP				4
#define DRIBBLE1_SND					22
#define DRIBBLE2_SND					23
#define NET1_SND						24
#define STEAL1_SND						2
#define HIT_RIM2_SND					15
#define HIT_BACKBOARD1_SND				4
#define DUNK1_SND						8
#define DUNK2_SND						6
#define FLAIL1_SND						12
#define SHOE_SQUEAK_1					16
#define SHOE_SQUEAK_2					17
#define SHOE_SQUEAK_3					18
#define SHOE_SQUEAK_4					19
#define SHOE_SQUEAK_5					20
#define SHOE_SQUEAK_6					21
#define SHOE_SCRAPE_1					98
#define SHOE_SCRAPE_2					99
#define SHOE_SCRAPE_3					100
#define SHOE_SCRAPE_4					101

// in balletc.bnk
#define HORN_SND							13
#define WHISTLE_SND						14
#define ON_FIRE_SHOT_SND				60
#define ON_FIRE_SHORT_SHOT_SND			35
#define BODY_HIT_GRND_SND				39
#define CAMERA_FLASH1_SND				40
#define CAMERA_FLASH2_SND				41
#define	EXPLOSION_SND					42
#define	NET_ON_FIRE_SND					44
#define	WARNING_BEEP_SND				45
#define	PASS1_SND						85
#define	PASS2_SND						86
#define	SWIPE1_SND						85
#define	SWIPE2_SND						86
#define	SECRET_CODE_ENTERED_SND			57
#define	LOOSE_BALL_SND					58
#define	DUNK_CAR_SND					59
#define	DUNK_PLANE1_SND					60
#define	DUNK_PLANE2_SND					61
#define	DUNK_PLANE3_SND					62
#define	DUNK_PLANE4_SND					63
#define	DUNK_PLANE5_SND					64
#define	DUNK_PLANE6_SND					65
#define	SPIN_MOVE_SND					77
#define	JUKE_MOVE_SND					96

#define	WHOOSH_SND						114 	// Ball in
#define	WHOOSH2_SND						112		// Ball out
#define	EXPLOSION2_SND					113

// MISC PLAYER SPEECH

#define THATS_ALL_NIGHT					37
#define YOU_BETTER_RECOGNIZE			46
#define YOU_BETTER_ASK_SOMEBODY			47
#define HOOPERS_DONT_DANCE				19

#define HUH_1							10
#define HUH_2							4
#define YEAH_1							23
#define YEAH_2							24
#define YEAH_3							25
#define CHUMP							80
#define UNGH							67
#define AHHH							68
#define DUNK_YELL						69
#define YEAH_EXCITED					70
#define DUNK_YELL_2						71
#define UGH								85
#define OO_OH							86
#define AHH_PAIN						87
#define UGH_2							88
#define UGH_3							89
#define UMM_QUICK						90
#define UMM_BUMP						91
#define UMM_MEAN						92
#define OH_PAIN							93
#define AIH_SCREAM						94
#define AHH_PAIN_2						95
#define UMM_MEAN_2						96
#define UMM_WHY							97


// ANNOUNCER SPEECH
#define TO_NAME_PB			 		5
#define HANDS_TO_NAME_PB 			6
#define DISHES_TO_NAME_PB 			7
#define FINDS_NAME_PB 				8

#define FULL_NAME_PB  		1	// excited ! ! !
#define LAST_NAME_1_PB		2	// general case (every n seconds?)
#define LAST_NAME_2_PB		3	// doing something exciting(going up for a shot, etc...)
#define LAST_NAME_3_PB		4	// finishes something good(long shot, dunk)


#define	OPTION_CURSR_SND	5
#define	OPTION_SELECT_SND	40


#define	RECORD_FOUND_SND	22

#define	CURSOR_SND1			22 //27		// in 'plyr_bnk_str'
#define	SELECT_SND1			27 //24
#define	NEXT_PAGE_SND		38

#define	TMSEL_CURSR_SND		22 // in 'plr_bnk_str'
#define	TM_SELECTED_SND		40 //29 //40  in cursors1.bnk
#define	SEL_YESNO_SND		35//27
#define	TIMEDOWN_SND		38

#define	PLAQ_SLIDE_SND		33//17
#define	PLAQ_STOP_SND		9

#define	MUGSHOT_MOVE_SND	5
#define	MUGSHOT_SELECT_SND	41

#define	RND_SELCT_STRT_SND	31//16
#define	RND_SELCT_STP_SND	8

#define	VS_WRONG_CODE_SND	56
#define	VS_GOT_CODE_SND		45 //27
#define	VS_CHANGE_ICON_SND	14

#define	FULLGM_APPEAR_SND	35
#define	FULLGM_HIT_SND		36

#define	STAT_AWARD_SND		22

//
// misc sounds
//
#define	SWOOSH2_SND      	17		// sounds for transit plates coming and going
#define	LOCK_SND          	9		// sounds for transit plates closing
#define CLOCK_LOW_SND       15
#define WARNING_SND         15
#define WHISTLE1_SND        14		// plyr_bnk_str
#define RICOCHET_SND        22     // plyr_bnk_str -- Ricochet or idiot box go away

//
// COIN-IN SOUNDS and INFO
//
#define COIN_SND_DROP_BANK 		"cursors"		/* which bank the coin drop sound is in */
#define COIN_SND_DROP_CALL 		37//22				/* which sound call number in the bank */
#define COIN_SND_DROP_VOLUME 	50				 	/* track volume */

#define COIN_SND_PAID_BANK 		"cursors" 	/* special sound when player gets paidup */
#define COIN_SND_PAID_CALL 		37//22
#define COIN_SND_PAID_VOLUME 	50

#define COIN_SOUND_INTERVAL 	15				/* min delay between coindrop sounds */
#define	EAT_CREDIT_SND			39   			// when press start button
#define	TICK_SND			    38              /* Tick sound for attract toggling */

/* CREATP SOUNDS */
#define CREATP_BANK				"CREATEP3"
#define CREATP_MUSIC			"TUNCREA2"
#define CREATP_SELECT			1
#define CREATP_UP				2
#define CREATP_DOWN				3
#define CREATP_CANNOT_SELECT	4
#define CREATP_ATTR1			7
#define CREATP_ATTR2			8
#define CREATP_ATTR3			9
#define CREATP_ATTR4			10
#define CREATP_ATTR5			11
#define CREATP_ATTR6			12
#define CREATP_ATTR7			13
#define CREATP_ATTR8			14
#define CREATP_ATTR9			15
#define CREATP_ATTR10			16
#define CREATP_ATTR11			17
#define CREATP_ATTR12			18
#define CREATP_ATTR13			19
#define CREATP_ATTR14			20
#define CREATP_ATTR15			21
#define CREATP_ATTR16			22
#define CREATP_ATTR17			23
#define CREATP_ATTR18			24
#define CREATP_ATTR19			25
#define CREATP_ATTR20			26
#define CREATP_BADNAME			27
#define CREATP_ELECTRIC			29

//
// VOLUMES
//
//#define COIN_SND_DROP_VOLUME 	195			 	/* track volume */
//#define COIN_SND_PAID_VOLUME 	195

#define VOLUME1                 135-70          // General stuff/speech
#define VOLUME2                 180-70          // Speech & effects we really want to hear
#define VOLUME3                 225-70          // ??? Big tackles perhaps
#define VOLUME4                 255-70          // 
#define VOLUME5                 255-45          // 
#define VOLUME6                 255-20          // 
#define VOLUME7                 255             // Maximum

#define	ANCR_VOL1				225-20
#define	ANCR_VOL2				255-10

typedef struct sndinfo {
	char *bank_name;
	unsigned int scall;
    unsigned int volume;
    unsigned int pan;
    unsigned int priority;
	int max_times;
	int loops;
} sndinfo;

#endif
