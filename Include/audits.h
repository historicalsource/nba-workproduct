/*
 *		$Archive: /video/Nba/Include/audits.h $
 *		$Revision: 11 $
 *		$Date: 3/23/99 4:06p $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifndef	__AUDITS_H__
#define	__AUDITS_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_audits_h = "$Workfile: audits.h $ $Revision: 11 $";
#endif

	/* Audit numbers */
enum {
	/* Coin Audits */
	LEFT_COIN_AUDIT_NUM,			// Left coin slot coins
	RIGHT_COIN_AUDIT_NUM,			// Right coin slot coins
	BILLS_COIN_AUDIT_NUM,			// Dollar bill validator bills
	CENTER_COIN_AUDIT_NUM,			// Center coin slot coins
	EXTRA_COIN_AUDIT_NUM,			// Extra coin slot coins
	GAME_START_AUD,					// Attract mode game starts (unless play editor)
	MID_GAME_START_AUD,				// Mid-game starts
	CONTINUE_AUD,					// Game continues
	CHALLENGER_ACCEPTED_AUD,		// Challenger buy ins
	FREE_QUARTERS_AUD,				// Free quarter awarded
	FREE_GAME_AUD,					// Free games awarded
	SERVICE_CREDITS_AUD,			// Total service credits
	TOTAL_PLAYS_AUD,				// Total number of plays
	TOTAL_PAID_CREDITS_AUD,			// Total number of credits paid for
	TOTAL_MONEY_AUDIT_NUM,			// Total amount of money taken in
	
	/* credits audits */
	CREDITS_AUD,					// Available credits
	
	/* misc game audits */
	NUM_MACHINE_POWER_ON_AUD,		// Number of time the machine was turned on
	UPTIME_AUDIT_NUM,				// Total game uptime
	PLAYTIME_AUD,					// Total play time
	PLAYTIME_PERCENT_AUD,			// Playtime vs uptime
	FULL_GAME_PURCHASE_AUD,			// Up-front full game purchases
	FULL_GAME_PURCHASE_PCT_AUD,		// full games purchased / starts
	INITIALS_ENTERED_AUD,			// Number of times initials entered
	INITIALS_ENTERED_PERCENT_AUD,	// Initials enterd percentage
	CREATE_PLAYER_AUD,				// create player start
	CREATE_PLAYER_PERCENT_AUD,		// create player start percentage
	FINISHED_GAMES_AUD,				// games finished (including ties in OT3)
	
	QUARTERS_PLAYED_AUD,			// ?p quarters played

	ONE_PLAYER_AUD,					// 1p quarters played
	TWO_PLAYER_AUD,					// 2p quarters played
	THREE_PLAYER_AUD,				// 3p quarters played
	FOUR_PLAYER_AUD,				// 4p quarters played
	TWOVCPU_PLAYER_AUD,				// 2p quarters played as 2 vs CPU
	
	ONE_PLAYER_PERCENT_AUD,			// 1p aud / quarters played
	TWO_PLAYER_PERCENT_AUD,			// 2p aud / quarters played
	THREE_PLAYER_PERCENT_AUD,		// 3p aud / quarters played
	FOUR_PLAYER_PERCENT_AUD,		// 4p aud / quarters played
	TWOVCPU_PLAYER_PERCENT_AUD,		// 2vcpu / 2p aud
	
	ONE_QUARTER_AUD,				// first quarters played
	TWO_QUARTER_AUD,				// second quarters played
	THREE_QUARTER_AUD,				// third quarters played
	FOUR_QUARTER_AUD,				// fourth quarters played
	ONE_OVERTIME_AUD,				// first overtimes played
	TWO_OVERTIME_AUD,				// second overtimes played
	THREE_OVERTIME_AUD,				// third overtimes played
	
	ONE_QUARTER_PERCENT_AUD, 		// One quarter play percentage
	TWO_QUARTER_PERCENT_AUD, 		// Two quarter play percentage
	THREE_QUARTER_PERCENT_AUD,		// Three quarter play percentage
	FOUR_QUARTER_PERCENT_AUD, 		// Four quarter play percentage
	ONE_OVERTIME_PERCENT_AUD, 		// One overtime quarter percentage
	TWO_OVERTIME_PERCENT_AUD, 		// Two overtime quarter percentage
	THREE_OVERTIME_PERCENT_AUD,	// Three overtime quarter percentage
	
	ONE_PLAYER_FINISH_AUD,			// games ending in 1p mode
	TWO_PLAYER_FINISH_AUD,			// games ending in 2p mode
	THREE_PLAYER_FINISH_AUD,		// games ending in 3p mode
	FOUR_PLAYER_FINISH_AUD,			// games ending in 4p mode
	TWOVCPU_PLAYER_FINISH_AUD,		// games ending in 2p mode with 2 vs cpu
	ONE_PLAYER_FINISH_PERCENT_AUD,	// games ending in 1p mode / finished
	TWO_PLAYER_FINISH_PERCENT_AUD,	// games ending in 2p mode / finished
	THREE_PLAYER_FINISH_PERCENT_AUD,// games ending in 3p mode / finished
	FOUR_PLAYER_FINISH_PERCENT_AUD,	// games ending in 4p mode / finished
	TWOVCPU_PLAYER_FINISH_PERCENT_AUD,		// games ending in 2p mode with 2 vs cpu / finished
	HVH_FINISH_AUD,					// games ending with humans on both teams
	HVC_FINISH_AUD,					// games ending with humans on only one team (no percent)
	HVH_FINISH_PERCENT_AUD,			// games ending with humans on both teams / finished
	
	TOTAL_HVSH_POINTS_AUD,			// Total points scored in h vs h games
	TOTAL_HVSC_POINTS_AUD,			// Total points scored in h vs cpu games
	AVERAGE_HVSH_POINTS_AUD,		// Average h vs h points
	AVERAGE_HVSC_POINTS_AUD,		// Average h vs cpu points
	TOTAL_WINNING_POINTS_AUD,		// Total winning points
	TOTAL_LOSING_POINTS_AUD,		// Total losing points
	AVERAGE_WINNING_SCORE_AUD,		// Average winning score
	AVERAGE_LOSING_SCORE_AUD,		// Average losing score
	TOTAL_CPU_VICTORIES_AUD,		// Total number of CPU victories
	CPU_VICTORY_PERCENT_AUD,		// Percentage number of CPU victories
	LARGEST_CPU_VICTORY_AUD,		// Largest cpu victory margin
	LARGEST_CPU_LOSS_AUD,			// Largest cpu loss margin
	
	LEFT_SIDE_WINS_AUD,				// Left side win count (hvh)
	LEFT_SIDE_WINS_PERCENT_AUD,		// Left side win percentage
	
	/* Team Audits */				// Number of times team was chosen
	ATLANTA_HAWKS_AUD,
	BOSTON_CELTICS_AUD,
	CHARLOTTE_HORNETS_AUD,
	CHICAGO_BULLS_AUD,
	CLEVLAND_CAVALIERS_AUD,
	DALLAS_MAVERICKS_AUD,
	DENVER_NUGGETS_AUD,
	DETROIT_PISTONS_AUD,
	GOLDEN_STATE_WARRIORS_AUD,
	HOUSTON_ROCKETS_AUD,
	INDIANAPOLIS_PACERS_AUD,
	LOS_ANGELES_CLIPPERS_AUD,
	LOS_ANGELES_LAKERS_AUD,
	MIAMI_HEAT_AUD,
	MILWAUKEE_BUCKS_AUD,
	MINNESOTA_TIMBERWOLVES_AUD,
	NEW_JERSEY_NETS_AUD,
	NEW_YORK_KNICKS_AUD,
	ORLANDO_MAGIC_AUD,
	PHILADELPHIA_SEVENTYSIXERS_AUD,
	PHOENIX_SUNS_AUD,
	PORTLAND_TRAILBLAZERS_AUD,
	SACRAMENTO_KINGS_AUD,
	SAN_ANTONIO_SPURS_AUD,
	SEATTLE_SUPERSONICS_AUD,
	TORONTO_RAPTORS_AUD,
	UTAH_JAZZ_AUD,
	VANCOUVER_GRIZZLIES_AUD,
	WASHINGTON_WIZARDS_AUD,
									// Percentage of times team was chosen
	ATLANTA_HAWKS_PERCENT_AUD,
	BOSTON_CELTICS_PERCENT_AUD,
	CHARLOTTE_HORNETS_PERCENT_AUD,
	CHICAGO_BULLS_PERCENT_AUD,
	CLEVLAND_CAVALIERS_PERCENT_AUD,
	DALLAS_MAVERICKS_PERCENT_AUD,
	DENVER_NUGGETS_PERCENT_AUD,
	DETROIT_PISTONS_PERCENT_AUD,
	GOLDEN_STATE_WARRIORS_PERCENT_AUD,
	HOUSTON_ROCKETS_PERCENT_AUD,
	INDIANAPOLIS_PACERS_PERCENT_AUD,
	LOS_ANGELES_CLIPPERS_PERCENT_AUD,
	LOS_ANGELES_LAKERS_PERCENT_AUD,
	MIAMI_HEAT_PERCENT_AUD,
	MILWAUKEE_BUCKS_PERCENT_AUD,
	MINNESOTA_TIMBERWOLVES_PERCENT_AUD,
	NEW_JERSEY_NETS_PERCENT_AUD,
	NEW_YORK_KNICKS_PERCENT_AUD,
	ORLANDO_MAGIC_PERCENT_AUD,
	PHILADELPHIA_SEVENTYSIXERS_PERCENT_AUD,
	PHOENIX_SUNS_PERCENT_AUD,
	PORTLAND_TRAILBLAZERS_PERCENT_AUD,
	SACRAMENTO_KINGS_PERCENT_AUD,
	SAN_ANTONIO_SPURS_PERCENT_AUD,
	SEATTLE_SUPERSONICS_PERCENT_AUD,
	TORONTO_RAPTORS_PERCENT_AUD,
	UTAH_JAZZ_PERCENT_AUD,
	VANCOUVER_GRIZZLIES_PERCENT_AUD,
	WASHINGTON_WIZARDS_PERCENT_AUD,

	/* game feature audits */
	ACTIVE_PLAYER_RECORD_AUD,			// Number of player records (by entering initials and create player)
	PURGED_RECORD_AUD,					// Number of player records purged to make space
	CREATED_PLAYER_RECORD_PERCENT_AUD,	// Percentage of records that were created by create player
	TOTAL_PLAYER_FIRE_AUD,				// Total number of times a player has gone on fire
	AVG_PTS_PLAYER_FIRE_AUD,			// Average points scored while on file
	TOTAL_TEAM_FIRE_AUD,				// Total number of times a team has gone on fire
	AVG_PTS_TEAM_FIRE_AUD,				// Avergae points scored while team on fire
	TOTAL_ALLEY_OOPS_AUD,				// Total number of alley oops
	TOTAL_DOUBLE_DUNKS_AUD,				// Total number of double dunks

	CREATEP_HEAD_MASCOT_PERCENT_AUD,	// Percent of created players that choose a mascot head
	CREATEP_HEAD_NBA_STAR_PERCENT_AUD,	// Percent of created players that choose a NBA player head
	CREATEP_HEAD_FEMALE_PERCENT_AUD,	// Percent of created players that choose a female head
	CREATEP_HEAD_UM_PERCENT_AUD,		// Percent of created players that choose a UM head
	CREATEP_HEAD_MID_PERCENT_AUD,		// Percent of created players that choose a Midway head
	CREATEP_HEAD_OTHER_PERCENT_AUD,		// Percent of created players that choose a silly head
	CREATEP_UNIFORM_DEFAULT_PERCENT_AUD,// Percent of created players that choose a default uniform
	CREATEP_UNIFORM_CUSTOM_PERCENT_AUD,	// Percent of created players that choose a custom uniform
	CREATEP_UNIFORM_TEAM_PERCENT_AUD,	// Percent of created players that choose a specific team uniform
	CREATEP_NICKNAME_AUD_PERCENT,		// Percent of created players that choose a nickname
	CREATEP_HEIGHT_AVG_AUD,				// Average number of attribute points for height
	CREATEP_WEIGHT_AVG_AUD,				// Average number of attribute points for weight
	CREATEP_POWER_AVG_AUD,				// Average number of attribute points for power
	CREATEP_SPEED_AVG_AUD,				// Average number of attribute points for speed
	CREATEP_TWOPTS_AVG_AUD,				// Average number of attribute points for shooting
	CREATEP_THREEPTS_AVG_AUD,			// Average number of attribute points for rebounding
	CREATEP_STEAL_AVG_AUD,				// Average number of attribute points for stealing
	CREATEP_BLOCK_AVG_AUD,				// Average number of attribute points for blocking
	CREATEP_DUNKS_AVG_AUD,				// Average number of attribute points for dunks
	CREATEP_DRIBBLE_AVG_AUD,			// Average number of attribute points for dribbling
	
	CREATEP_PRIV1_PERCENT_AUD,
	CREATEP_PRIV2_PERCENT_AUD,
	CREATEP_PRIV3_PERCENT_AUD,
	CREATEP_PRIV4_PERCENT_AUD,
	CREATEP_PRIV5_PERCENT_AUD,
	CREATEP_PRIV6_PERCENT_AUD,
	CREATEP_PRIV7_PERCENT_AUD,
	CREATEP_PRIV8_PERCENT_AUD,
	
	GAMES_PLAYED_WITH_CREATEP_PERCENT_AUD,	// Total number of games played with a created player involved
	WIN_VCPU_WITH_CREATEP_PRECENT_AUD,	// Win percentage vs CPU with a created player involved
	WIN_VHUM_WITH_CREATEP_PRECENT_AUD,	// Win percentage vs human with a created player involved
	GAME_IN_TOURNEY_MODE_PRECENT_AUD,	// Percentage of games played in tournement mode
	UNLOCK_LEVEL_AUD,					// Unlock level of the skill release characters
	
	/* Failure audits */
	TEX_LOAD_FAIL_AUD,				// Texture load failures
	SND_LOAD_FAIL_AUD,				// Sound Bank load failures
	
	/* Last audit */
	NEXT_AUD = SND_LOAD_FAIL_AUD + 20
};

/* Definitions of font sizes */
#define	LARGE_FONT	0
#define	MEDIUM_FONT	1
#define	SMALL_FONT	2

/* Definitions of prompt and audit horizontal justification modes */
#define	RIGHT_JUST	1
#define	LEFT_JUST	2
#define	CENTER_JUST	3

/* Definitions of formatting parameters */
#define	DECIMAL		"decimal"
#define	HEX			"hex"
#define	TIMESTAMP	"timestamp"
#define	DURATION	"duration"
#define	PERCENT		"percent"
#define	MONEY		"money"
#define	FLOAT		"float"
#define	STRING		"string"


// Macro definitions
#define	ADD_MENU_ITEM(a, b) 	fprintf(fp, "\r\nitem \"%s\" itemhelp \"%s\" itemdone\r\n", (a), (b))

#define	ADD_AUDIT(a, b, c, d, e, f)	fprintf(fp, "startaudit %d font %d pcolor %u prompt \"%s\" %s acolor %u auditdone\r\n", (a), (b), (c), (d), (e), (f))

#define	ADD_AUDIT_XY(a, b, c, d, e, f, g, h, i, j)	fprintf(fp, "startaudit %d font %d pcolor %u prompt \"%s\" %s acolor %u ax %u ay %u px %u py %u auditdone\r\n", (a), (b), (c), (d), (e), (f), (g), (h), (i), (j))

#define	MESSAGE(a, b, c, d, e)	fprintf(fp, "startaudit 0 font %d pcolor %u \"%s\" px %u py %u auditdone\r\n", (a), (b), (c), (d), (e))

#define	PAGE_BREAK	fprintf(fp, "pagebreak\r\n")

#define	COLUMNS(a)	fprintf(fp, "columns %d\r\n", (a))

#define	LINESPACE(a)	fprintf(fp, "linespace %d\r\n", (a))

#define	PROMPT_JUSTIFY	fprintf(fp, "padjust %d\r\n", (a))

#define	AUDIT_JUSTIFY	fprintf(fp, "aadjust %d\r\n", (a))


// Prototypes
void write_audit_info(FILE *fp);
int decrement_audit(int);
int add_to_audit( int, int );
void do_percent_aud(int, int, int);
void recalc_percent_aud(int, int, int);
void clear_all_audits(void);
void update_gamestart_percents( void );
void update_quarters_percents( void );
int get_audit_val( int );
void dump_raw_audits( void );

#endif
