/*
 *		$Archive: /video/Nba/Include/coin.h $
 *		$Revision: 3 $
 *		$Date: 10/06/98 11:34a $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifndef __COIN_H__
#define __COIN_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_coin_h = "$Workfile: coin.h $ $Revision: 3 $";
#endif

#include <goose/goose.h>

#ifndef __SYSTEM_H__
#include "system.h"
#endif

/***** DEFINES ***************************************************************/

#define COIN_NUM_SLOTS			5

#define PRICE_LEAD_STR_SIZE		8		/* pricing leading monetary symbol str */
#define PRICE_TRAIL_STR_SIZE	8		/* pricing trailing monetary symbol str */
#define PRICE_INFO_STR_SIZE		80		/* pricing string e.g. "1 credit / 1 coin" */

/***** STRUCTURE DEFINITIONS *************************************************/


typedef struct coin_credits {
	int credits;
	int service_credits;
	int coin_units;
	int bonus_units;
	int remainder_units;
} coin_credits_t;

typedef struct coin_totalizer {
	int mult [COIN_NUM_SLOTS];			/* how many clicks per coin */
	int output [COIN_NUM_SLOTS];		/* which meter to click */
	int count [COIN_NUM_SLOTS];			/* how many clicks remain to be done */
	int use_multipliers;			    /* whether or not to use mult field */
	int mode;							/* clicker or totalizer */
} coin_totalizer_t;

typedef struct coin_state {
	int slam;		 					/* slam happened and is in debounce period */
    int message_type;                   /* how much coin message info displayed */
    int free_play;                      /* 0 = no, 1 = free play */    
	int show_volume;					/* 0 = no, N > 0 == show for N ticks */
	int sound_holdoff;					/* 1 = no sound calls, 0 = OK to make snd */
} coin_state_t;

enum {COIN_MESSAGE_SHORT, COIN_MESSAGE_LONG};

/* this pricing info will be set by the pricing mode */
/* stored in the CMOS adjustments - this is our local copy */

typedef struct coin_pricing {
	int units [COIN_NUM_SLOTS];			/* num units each slot gives per coin */
	int units_per_credit;				/* num units needed for a credit */
	int units_per_bonus;				/* num units before bonus credit given */
	int minimum_units;					/* minimum units to get any credits */
	int credits_to_start;				/* num credits needed to start */
	int credits_to_continue;			/* num units needed to continue */
	int maximum_credits;				/* max credits allowed to accumulate */
	int coins_per_bill;					/* used to calculate money strings */
	int show_credit_fractions;			/* y/n show fractions as they add up */
    char lead_str [PRICE_LEAD_STR_SIZE];    /* e.g. "$" or "L" or "Y" */
    char trail_str [PRICE_LEAD_STR_SIZE];   /* e.g. "DM" or "FR" */
    char info_str [PRICE_INFO_STR_SIZE];    /* e.g. "1 credit / 1 coin" */
} coin_pricing_t;

/***** FUNCTION PROTOTYPES ***************************************************/

void coin_sw_handler(int sw_state, int sw_id);
void coin_svc_cred_handler(int sw_state, int sw_id);
void coin_control(int sw_state, int sw_id);
void coin_init_all(void);
void coin_init_pricing(void);
void coin_init_state(void);
void coin_init_units(void);
void coin_init_totalizer(void);
void coin_slam_handler(int sw_state, int sw_id);
void coin_slam_proc(int *arg);
void coin_clicker_proc(int *args);
void coin_strobe_clicker(unsigned int mask);
void coin_game_over(void);
int check_credits_to_start(void);
int check_credits_to_continue(void);
int get_credits(void);
int coin_check_freeplay(void);
int coin_check_units(void);
void coin_show_insert_proc(int *args);
int coin_show_insert_sleep(int sleep_time);
void coin_credits_exit_func(process_node_t *myproc, int cause);
void coin_show_credits_proc (int *args);
int coin_show_credits_sleep(int sleep_time);
void coin_change_mesg_state(int state);
void coin_volume_handler(int sw_state, int sw_id);
void coin_volume_proc(int *args);
void coin_sound_holdoff(int flag);
void decrement_credits(short dec_cnt);
int	get_credits_to_start(void);
int	get_credits_to_continue(void);
int	get_total_credits(void);
int get_full_game_credits(void);

__EXTERN__ int got_coin __INIT__(0);
__EXTERN__ int is_dip_coinage __INIT__(0);

#endif
