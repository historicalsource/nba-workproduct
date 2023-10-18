/*
 *		$Archive: /video/Nba/Include/nbacmos.h $
 *		$Revision: 19 $
 *		$Date: 4/16/99 10:29p $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifndef __NBACMOS_H__
#define __NBACMOS_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_nbacmos_h = "$Workfile: nbacmos.h $ $Revision: 19 $";
#endif

#include "audits.h"

#define CMOS_SIZE           32768	/* was 8192 */
#define NUM_AUDITS          NEXT_AUD
#define NUM_ADJUSTMENTS     100
#define NUM_PLYR_RECORDS    350
#define NUM_WORLD_RECORDS   6

#define LETTERS_IN_NAME     6
#define PIN_NUMBERS         4

#define GEN_REC_ARRAYS      2		// player records and world records

#define PLYR_RECS_ARRAY     0       // generic rec arrays in cmos
#define WORLD_RECS_ARRAY    1       // generic rec arrays in cmos
#define TEAM_RECS_ARRAY     2       // generic rec arrays in cmos

#define REC_MIN_GAMES       4       // nbr. games required before valid for High scores pages
#define HI_SCORE_ENTRIES    10

#define NUM_GSTATS          8		// games of comps/atts/yds/tds/ints data stored per player

#define UCHAR unsigned char
#define SCHAR signed char
#define USHORT unsigned short
#define UINT unsigned int
/* PLAYER record in CMOS */
typedef struct player_record {
	char			plyr_name[LETTERS_IN_NAME] __attribute__((packed));		/* players name */
	char			plyr_pin_nbr[PIN_NUMBERS] __attribute__((packed));		/* players PIN number */
	int				teams_defeated __attribute__((packed));					/* bit mask of team defeated */
	short			last_used __attribute__((packed));						/* games since last use of record */
	short			games_played __attribute__((packed));					/* total number of games played */
	short			games_won __attribute__((packed));						/* number of game won */
	short			two_ptrs_attempted __attribute__((packed));				/* total number of 2 pt shots attempted */
	short			two_ptrs_made __attribute__((packed));					/* total number of 2 pt shots made */
	short			three_ptrs_attempted __attribute__((packed));			/* total number of 3 pt shots attempted */
	short			three_ptrs_made __attribute__((packed));				/* total number of 3 pt shots made */
	short			dunks_attempted __attribute__((packed));				/* total number of dunks attempted */
	short			dunks_made __attribute__((packed));						/* total number of dunks made */
	short			rebounds __attribute__((packed));						/* total number of rebounds made */
	short			assists __attribute__((packed));						/* total number of assists made */
	short			steals __attribute__((packed));							/* total number of steals */
	short			blocks __attribute__((packed));							/* total number of steals */
	short			turnovers __attribute__((packed));						/* total number of steals */
	short			points_scored __attribute__((packed));					/* total number of points scored */
	short			points_allowed __attribute__((packed));					/* total number of points allowed */
	short			trivia_pts __attribute__((packed));						/* number of trivia points */
	char			winstreak __attribute__((packed));						/* current winning streak, if neg losing streak */
	char			alt_ladder __attribute__((packed));						/* level of second ladder progress */

	char	is_creatp_record;												/* has this CMOS record ever been touched by create player */
	SCHAR	height __attribute__((packed));									/* 1 - 20 */
	SCHAR	weight __attribute__((packed));									/* 1 - 20 */
	SCHAR	power __attribute__((packed));									/* 1 - 20 */
	SCHAR	speed __attribute__((packed));									/* 1 - 20 */
	SCHAR	twop __attribute__((packed));									/* 1 - 20 */
	SCHAR	threep __attribute__((packed));									/* 1 - 20 */
	SCHAR	steal __attribute__((packed));									/* 1 - 20 */
	SCHAR	block __attribute__((packed));									/* 1 - 20 */
	SCHAR	dunks __attribute__((packed));									/* 1 - 20 */
	SCHAR	dribble __attribute__((packed));								/* 1 - 20 */

	SCHAR	team __attribute__((packed));
	SCHAR	player_index __attribute__((packed));
	SCHAR	jersey_city  __attribute__((packed));							/* TEAM, DEFAULT_UNIFORM_TEAM, CUSTOM_UNIFORM_TEAM */
	SCHAR	jersey_style __attribute__((packed));							/* HOME_JERSEY, AWAY_JERSEY, ALTERNATE_JERSEY */
	UCHAR	jersey_num __attribute__((packed));								/* 00 - 99 */

	UCHAR	privileges __attribute__((packed));								/* bit mask of privs */
	UCHAR	nickname __attribute__((packed));								/* index into nickname list 0 for none */
	UCHAR	unused_attribute_points __attribute__((packed));				/* unused attribute points */
	UCHAR	unused_privilege_points __attribute__((packed));				/* unused privilege attribute points */
	UINT hotspot;															/* X, Z */
} plyr_record_t;
#undef UCHAR
#undef SCHAR
#undef USHORT
#undef UINT

/* WORLD record in CMOS */
typedef struct world_record {
	char    plyr_name[LETTERS_IN_NAME];
	char    plyr_pin_nbr[PIN_NUMBERS];
	signed short	value;
} world_record_t;

/* NFL teams and their DEFEATED bit position (team #'s defined in GAME.H) */
#define HAWKS	    	(1<<TEAM_ATL)
#define CELTICS			(1<<TEAM_BOS)
#define HORNETS			(1<<TEAM_CHA)
#define BULLS			(1<<TEAM_CHI)
#define CAVALIERS		(1<<TEAM_CLE)
#define MAVERICKS		(1<<TEAM_DAL)
#define NUGGETS	    	(1<<TEAM_DEN)
#define PISTONS	    	(1<<TEAM_DET)
#define WARRIORS    	(1<<TEAM_GOL)
#define ROCKETS			(1<<TEAM_HOU)
#define PACERS	    	(1<<TEAM_IND)
#define CLIPPERS 		(1<<TEAM_LAC)
#define LAKERS	    	(1<<TEAM_LAL)
#define HEAT			(1<<TEAM_MIA)
#define BUCKS			(1<<TEAM_MIL)
#define TIMBERWOLVES	(1<<TEAM_MIN)
#define NETS			(1<<TEAM_NJN)
#define KNICKS			(1<<TEAM_NYK)
#define MAGIC			(1<<TEAM_ORL)
#define SEVENTYSIXERS	(1<<TEAM_PHI)
#define SUNS	    	(1<<TEAM_PHO)
#define TRAILBLAZERS	(1<<TEAM_POR)
#define KINGS			(1<<TEAM_SAC)
#define SPURS			(1<<TEAM_SAN)
#define SUPERSONICS		(1<<TEAM_SEA)
#define RAPTORS			(1<<TEAM_TOR)
#define JAZZ			(1<<TEAM_UTA)
#define GRIZZLIES		(1<<TEAM_VAN)
#define WIZARDS			(1<<TEAM_WAS)

/* Defines for ALPHA CHARACTERS stored in players name and pin number */
#define CH_0          0x30
#define CH_1          0x31
#define CH_2          0x32
#define CH_3          0x33
#define CH_4          0x34
#define CH_5          0x35
#define CH_6          0x36
#define CH_7          0x37
#define CH_8          0x38
#define CH_9          0x39

#define CH_A          0x41
#define CH_B          0x42
#define CH_C          0x43
#define CH_D          0x44
#define CH_E          0x45
#define CH_F          0x46
#define CH_G          0x47
#define CH_H          0x48
#define CH_I          0x49
#define CH_J          0x4a
#define CH_K          0x4b
#define CH_L          0x4c
#define CH_M          0x4d
#define CH_N          0x4e
#define CH_O          0x4f
#define CH_P          0x50
#define CH_Q          0x51
#define CH_R          0x52
#define CH_S          0x53
#define CH_T          0x54
#define CH_U          0x55
#define CH_V          0x56
#define CH_W          0x57
#define CH_X          0x58
#define CH_Y          0x59
#define CH_Z          0x5a
#define CH_DSH        0x2d
#define CH_AST        0x2a
#define CH_EXC        0x21
#define CH_SPC        0x20
#define CH_DEL        0x7F
#define CH_END        0x0d


/* Routine declarations */
int init_cmos(void);
int  validate_plyr_records(void);
void set_default_plyr_records(void);
void set_default_world_records(void);
void clear_cmos_record(char *rec,int bytes_in_rec);
void get_player_record_from_cmos(char *name_n_pin, plyr_record_t *plyr_rec);
int find_record_in_cmos(char *plyr_name_and_pin);
int find_record_in_ram(char *pname_and_pin);
int compare_record(char *plr_name_n_pin1,char *plr_name_n_pin2, int n);
int get_new_record_into_ram(char *);
int get_free_record_nbr(void);
void save_player_records(void);
void get_all_records(void);
int sort_trivia(plyr_record_t *rec1, plyr_record_t *rec2);
//int sort_qb_rating(plyr_record_t *rec1, plyr_record_t *rec2);
int sort_wins(plyr_record_t *rec1, plyr_record_t *rec2);
int sort_win_streaks(plyr_record_t *rec1, plyr_record_t *rec2);
int sort_win_percent(plyr_record_t *rec1, plyr_record_t *rec2);
int sort_games_played(plyr_record_t *rec1, plyr_record_t *rec2);
int sort_teams_defeated(plyr_record_t *rec1, plyr_record_t *rec2);
int sort_points_scored(plyr_record_t *rec1, plyr_record_t *rec2);
int sort_points_allowed(plyr_record_t *rec1, plyr_record_t *rec2);
int sort_dunk_percent(plyr_record_t *rec1, plyr_record_t *rec2);
int sort_3pt_percent(plyr_record_t *rec1, plyr_record_t *rec2);
int calc_tms_def(int tms_def);
void check_world_records(void);
void show_hiscore_tables(int, int);
void show_world_records(int, int);
void inc_cmos_records_used_cnt(void);

#endif
