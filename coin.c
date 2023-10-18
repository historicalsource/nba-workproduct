/*
 *		$Archive: /video/Nba/coin.c $
 *		$Revision: 34 $
 *		$Date: 9/03/99 7:09p $
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
char *ss_coin_c = "$Workfile: coin.c $ $Revision: 34 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ioctl.h>
#include <goose/goose.h>

/*
 *		USER INCLUDES
 */

#include "include/fontid.h"
#include "include/id.h"
#include "include/game.h"
#include "include/coin.h"
#include "include/sndcalls.h"
#include "include/audits.h"
#include "include/gameadj.h"
#include "include/mainproc.h"
#define	DEFINE_PRICE
#include "/video/diag/include/price.h"
#define	DEFINE_DIP_PRICING
#include "/video/diag/nba/dswtest.c"

//#define FREEPLAY_ONLY

#define	TOTALIZE_ONLY

/* Local defines nobody else should ever need */

#define CNTR_X							(SPRITE_HRES / 2.0f)

#define COIN_VOLUME_DISPLAY_TIME		3			/* number of seconds to display volume message */
#define COIN_VOLUME_STR_SIZE			20
#define COIN_VOLUME_X					256
#define COIN_VOLUME_Y					30
#define COIN_VOLUME_Y2					15

#define COIN_SLAM_SLEEP 				60			/* how many ticks to lock out a credit after a slam */

#define	TEXT_ID_COIN_MESG				300
#define	TEXT_ID_VOLUME_MESG				350
#define ATT_LIVE						0			/* attract mode keeps running */
#define ATT_DIE						 	1			/* attract mode cleans up and dies */
#define ATT_CHANGE						2 			/* attract mode needs immediate update */

#define COIN_CLICKER_MASK_LEFT			0x01
#define COIN_CLICKER_MASK_RIGHT			0x02
#define COIN_CLICKER_MASK_CENTER		0x04
#define COIN_CLICKER_MASK_EXTRA			0x08
#define COIN_CLICKER_MASK_DBV			0x10

#define COIN_CLICKER_UNLOCK				0x10

#define COIN_MESG_STR_SIZE				64

#define FULL_MSG_Y1						357.0F 		/* buy full game... */
#define FULL_MSG_Y2						333.0F 		/*  for ... */

#define FREE_MSG_Y1						306.0F 		/* free stuff */
#define FREE_MSG_Y2						279.0F 		/*  free stuff conditions */

#define COIN_MSG_Y1						246.0F 		/* credit: XX, was 250 */
#define COIN_MSG_Y2						207.0F	    /* X credits to start */
//#define COIN_MSG_Y3						171.0F		/* X credits to continue */
//#define COIN_MSG_Y4						142.0F		/* credit/coin msg */
#define COIN_MSG_Y5						75.0F		/* insert coins, freeplay, join in...etc */
#define COIN_MSG_Y6						48.0F		/* insert coins, freeplay, join in...etc */

#define COIN_PROMPT_INSERT				0			/* display "insert coins" */
#define COIN_PROMPT_CREDITS				1			/* display "N credits to start" */
#define COIN_PROMPT_START				0			/* display "press start" */
#define COIN_PROMPT_FREEPLAY			1			/* display "free play" */

#define COIN_INSERT_ON_TICKS			40			/* ticks that message is on */
#define COIN_INSERT_OFF_TICKS			30			/* ticks that message is hidden */
#define COIN_INSERT_SPEEDUP				15			/* amount to subtract for faster flashing */

enum {
	COIN_LEFT,
	COIN_RIGHT,
	COIN_CENTER,
	COIN_EXTRA,
	COIN_DBV
};
enum {
	COUNT_MODE_TOTALIZER,
	COUNT_MODE_CLICKER
};

extern unsigned int coin_snd_drop_calln;
extern unsigned int coin_snd_paid_calln;
extern font_info_t bast75_font;

void credit_page_proc(int *args);
void hide_string( void * );
void unhide_string( void * );

coin_credits_t coins;
static coin_pricing_t pricing;
static coin_totalizer_t totalizer;
static coin_state_t coin_state;
static int coin_sound_count;
static process_node_t *pflasher = NULL;
static ostring_t *pmsg[2];	/* purchase messages */
int full_game_credits;


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: coin_sw_handler()                                               */
/*                                                                           */
/* This function gets called when the following coin-related switches        */
/* transition from open->closed:                                             */
/*                                                                           */
/* left coin slot                                                            */
/* right coin slot                                                           */
/* center coin slot                                                          */
/* extra coin slot                                                           */
/* dollar bill validator                                                     */
/* service credits                                                           */
/*                                                                           */
/* This handler is setup by main_proc() in main.c                            */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 30 Jan 97                                                             */
/*                                                                           */
/*****************************************************************************/
void coin_sw_handler(int sw_state, int sw_id)
{
	static int paidup_sound_made = FALSE;				/* set state of paid up sound */
	
	/* only valid on transition from open->closed */

	if(sw_state == SWITCH_CLOSE &&
		(sw_id == LEFT_COIN_SWID || sw_id == RIGHT_COIN_SWID))
		return;

	if(sw_state == SWITCH_OPEN &&
		!(sw_id == LEFT_COIN_SWID || sw_id == RIGHT_COIN_SWID))
		return;

//	if (sw_state == SWITCH_CLOSE) {
		coin_control(sw_state, sw_id);
	
		got_coin = 1;
	
		/* fire the credit page up */
		qcreate("coinpg", CREDIT_PAGE_ID, credit_page_proc, 0, 0, 0, 0);
		
		/* are there enough credits to start ? */
		if (!check_credits_to_start()) {
			/* NOPE - paid up sound not made */
			paidup_sound_made = FALSE;
		}
	
		/* OK to do coin drop sounds ? */
		if (!coin_state.sound_holdoff) {
			if (game_info.game_state == GS_ATTRACT_MODE) {
				/* Make sure any sounds are shutoff */
				snd_stop_all();
				snd_set_reserved(SND_TRACK_0);
			}
			
			/* YES - enough credits to start AND paid up sound NOT made ? */
			if (check_credits_to_start() && !paidup_sound_made) {
				/* YES - do paid up sound */
				if (game_info.game_state == GS_GAME_MODE)
					snd_scall_direct(coin_snd_paid_calln, VOLUME3, 127, PRIORITY_LVL4);
				else
					snd_scall_direct(coin_snd_paid_calln, VOLUME3, 127, PRIORITY_LVL4);
				paidup_sound_made = TRUE;
			} else {
				/* NOPE - do normal drop sound */
				if (game_info.game_state == GS_GAME_MODE)
					snd_scall_direct(coin_snd_drop_calln, VOLUME3, 127, PRIORITY_LVL4);
				else
					snd_scall_direct(coin_snd_drop_calln, VOLUME3, 127, PRIORITY_LVL4);
			}
		}
//	}
}	/* coin_sw_handler */

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: coin_svc_cred_handler()                                         */
/*                                                                           */
/* This function gets called when the service credits button is pressed.     */
/*                                                                           */
/* Service credits are only allowed when the coin door is open.              */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 30 Jan 97                                                             */
/*                                                                           */
/*****************************************************************************/
void coin_svc_cred_handler(int sw_state, int sw_id)
{
	int door_state;			/* open or closed */
	int total_credits;		/* credits + service_credits */

	/* Is this a close transistion ? */
	if (sw_state == SWITCH_CLOSE) {
		/* Get the state of the coin door */
		door_state = get_dip_coin_current();
		
		/* Is the coin door open ? */
		if (door_state & COINDOOR_INTERLOCK_SW) {
			/* NOPE - Service credits are not allow unless coin door is open */
			return;
		} else {
			/* schedule credits string here ??? */
			qcreate("coinpg", CREDIT_PAGE_ID, credit_page_proc, 0, 0, 0, 0);
			
			/* OK to do coin drop sounds ? */
			if (!coin_state.sound_holdoff) {
				if (game_info.game_state == GS_ATTRACT_MODE) {
					/* Make sure any sounds are shutoff */
					snd_stop_all();
					snd_set_reserved(SND_TRACK_0);
				}
				/* do sound */
				snd_scall_direct(coin_snd_paid_calln, COIN_SND_PAID_VOLUME, 127, PRIORITY_LVL4);
			}
			
			/* Increment service credits */
			coins.service_credits++;
			
			/* check max credits allowed */
			total_credits = coins.credits + coins.service_credits;
			
			if (total_credits > pricing.maximum_credits) {
				/* clamp total whole credits to maximum */
				/* also clamp fractional credits to 0 */
				coins.service_credits = pricing.maximum_credits - coins.credits;
				total_credits = pricing.maximum_credits;
				coins.coin_units = 0;
				coins.remainder_units = 0;
			} else {
				/* Increment the service credits audit */
				increment_audit(SERVICE_CREDITS_AUD);
			}
			set_audit(CREDITS_AUD, total_credits);
		}
	}
}	/* coin_svc_cred_handler */

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: coin_control()                                                  */
/*                                                                           */
/* 1. Converts coins to units based on units/slot.                           */
/* 2. Converts units to credits based on pricing.                            */
/* 3. Deals with bonus and remainder units.                                  */
/* 4. Audits coins.                                                          */
/* 5. Updates coin clickers.                                                 */
/*                                                                           */
/* The best way to understand the units, credits and bonus units scheme is   */
/* to have someone explain it to you with an example.                        */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 30 Jan 97                                                             */
/*                                                                           */
/*****************************************************************************/
void coin_control(int sw_state, int sw_id)
{
	int slot;						/* which slot the coin got dropped in */
	int total_credits;				/* credits + service credits */
	int temp;						
	
//	if ((sw_state == SWITCH_CLOSE) && !coin_state.slam) {
	if (!coin_state.slam) {
		/* track counts for mechanical meters (clicker) */
		switch(sw_id) {
		default:
		case LEFT_COIN_SWID:
			slot = COIN_LEFT;
			increment_audit(LEFT_COIN_AUDIT_NUM);
			break;
		case RIGHT_COIN_SWID:
			slot = COIN_RIGHT;
			increment_audit(RIGHT_COIN_AUDIT_NUM);
			break;
		case CENTER_COIN_SWID:
			slot = COIN_CENTER;
			increment_audit(CENTER_COIN_AUDIT_NUM);
			break;
		case EXTRA_COIN_SWID:
			slot = COIN_EXTRA;
			increment_audit(EXTRA_COIN_AUDIT_NUM);
			break;
		case BILL_VALIDATOR_SWID:
			slot = COIN_DBV;
			increment_audit(BILLS_COIN_AUDIT_NUM);
			break;
		}
		
		/* bump up the click count for this slot */
		if (totalizer.use_multipliers)
			totalizer.count[slot] += totalizer.mult[slot];
		else
			totalizer.count[slot]++;
		
		/* schedule a coin-drop sound */
		coin_sound_count++;
		
		/* convert coin to units */
		coins.coin_units += pricing.units[slot];
		temp = coins.coin_units + coins.remainder_units;
		
		/* if we have enough for one credit */
		if ((temp >= pricing.minimum_units) && (temp >= pricing.units_per_credit)) {
			coins.credits += temp / pricing.units_per_credit;
			coins.remainder_units = temp % pricing.units_per_credit;
			coins.coin_units = 0;
		}
		
		/* deal with bonus units as set up in pricing mode */
		coins.bonus_units += pricing.units[slot];	
		
		/* units_per_bonus is zero, then no bonuses given, so skip */
		if ((pricing.units_per_bonus != 0) && (coins.bonus_units != 0) && (coins.bonus_units >= pricing.units_per_bonus)) {
			coins.credits += coins.bonus_units / pricing.units_per_bonus;
			coins.bonus_units = coins.bonus_units % pricing.units_per_bonus;
			coins.remainder_units = 0;
			coins.coin_units = 0;
		}
		
		/* keep track of max credits */
		total_credits = coins.credits + coins.service_credits;
		if ((pricing.maximum_credits != 0) && (total_credits > pricing.maximum_credits)) {
			/* clamp total whole credits to maximum */
			/* also clamp fractional credits to 0 */
			coins.credits = pricing.maximum_credits - coins.service_credits;
			coins.coin_units = 0;
			coins.remainder_units = 0;
			total_credits = pricing.maximum_credits;
		}

		/* Audit the credits */
		if (set_audit(CREDITS_AUD, total_credits)) {
			/* ERROR - try once more */
			set_audit(CREDITS_AUD, total_credits);
		}
	}
}	/* coin_control */

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: coin_init_all()                                                 */
/*                                                                           */
/* Initializes all coin structures to zero or default values.                */
/*                                                                           */
/* Intended to only be called on power-up or reset.                          */
/*                                                                           */
/* Actual values should subsequently be read from CMOS.                      */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 30 Jan 97                                                             */
/*                                                                           */
/*****************************************************************************/
void coin_init_all(void)
{
 	coin_init_state();
	coin_init_pricing();
 	coin_init_units();
	coin_init_totalizer();
}	/* coin_init_all */


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: coin_init_pricing()                                             */
/*                                                                           */
/* Initializes the local pricing info structure to a default of "USA1"       */
/* pricing mode.                                                             */
/*                                                                           */
/* The real pricing info will be read from the CMOS table.                   */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 31 Jan 97                                                             */
/*                                                                           */
/*****************************************************************************/

void coin_init_pricing(void)
{
	int	continent;
	int	country;
	int	price;
	int	status;
	int	*to;
	int	i;
	struct pricing_t	**p_array;
	union
	{
		dip_inputs_t	ds;
		int				val;
	} dipin;
	char custom_pricing_game_info[80];
	struct pricing_t custom_pricing, *pricing_ptr;

	
	/* The the pricing pointer */
	pricing_ptr = NULL;
	
	/* Read the dip switches */
	dipin.val = get_dip_coin_current();
	
	/* Set pricing from CMOS ? */
	if (dipin.ds.coinage_mode == DIP_COINAGE_MODE_CMOS) {
		/* Get the continent code from CMOS */
		if (!get_adjustment(CONTINENT_ADJ, &continent)) {
			/* NO ERROR - Get the country code from CMOS */
			if (!get_adjustment(COUNTRY_ADJ, &country)) {
				/* NO ERROR - Get the price code from CMOS */
				if (!get_adjustment(PRICE_ADJ, &price)) {
					/* Is CMOS pricing custom ? */
					if (continent == CUSTOM) {
						/* YES - Initialize the custom pricing structure */
						status = 0;
						
						/* Get pointer to beginning of pricing structure */
						to = &custom_pricing.left_units;
						
						/* Fill in pricing structure from CMOS */
						for (i = 0; i < sizeof(custom_pricing) / sizeof(int); i++) {
							if (get_adjustment(LEFT_SLOT_UNITS_ADJ + i, to))
								status = 1;
							++to;
						}
						
						/* get the pricing info string */
						custom_pricing.game_info = custom_pricing_game_info;
						to = (int *)custom_pricing.game_info;
						for (i = 0; i < 20; i++) {
							if (get_adjustment(PRICING_INFO_STRING_ADJ + i, to))
								status = 1;
							++to;
						}
						
						/* Any errors ? */
						if (status) {
							/* YES - use dip pricing */
							pricing_ptr = NULL;
							is_dip_coinage = 1;
						} else
							pricing_ptr = &custom_pricing;
					} else {
						/* NOPE - Set pricing from pricing tables according to CMOS */
						p_array = cprices[continent];
						pricing_ptr = p_array[country];
						pricing_ptr += price;
					}
				}
			}
		}
	}
	
	/* if dip coinage OR CMOS error - use dip coinage */
	if ((dipin.ds.coinage_mode == DIP_COINAGE_MODE_DIP) || (pricing_ptr == NULL)) {
		pricing_ptr = dip_pricing[dipin.ds.dip_coinage];
		is_dip_coinage = 1;
	}
	
	/* get adjustment for number of credits for a full game */
	i = pricing_ptr->credits_to_start + (3 * pricing_ptr->credits_to_continue);
	if (get_adjustment(CREDITS_FOR_FULL_GAME_ADJ, &full_game_credits)) {
		/* ERROR - set to number of credits to start + (3 * credits to continue) */
		full_game_credits = i;
	} else if (full_game_credits > i) {
		/* Cheaper to just buy a full game set to number of credits to start + (3 * credits to continue) */
		full_game_credits = i;
	}
	
	pricing.units[COIN_LEFT] = pricing_ptr->left_units;
	pricing.units[COIN_RIGHT] = pricing_ptr->right_units;
	pricing.units[COIN_CENTER] = pricing_ptr->center_units;
	pricing.units[COIN_EXTRA] = pricing_ptr->extra_units;
	pricing.units[COIN_DBV] = pricing_ptr->dbv_units;
	pricing.units_per_credit = pricing_ptr->units_per_credit;
	pricing.units_per_bonus = pricing_ptr->units_per_bonus;
	pricing.minimum_units = pricing_ptr->min_units;
	pricing.credits_to_start = pricing_ptr->credits_to_start;
	pricing.credits_to_continue = pricing_ptr->credits_to_continue;
	pricing.maximum_credits = pricing_ptr->max_credits;
	pricing.coins_per_bill = pricing_ptr->coins_per_bill;
	pricing.show_credit_fractions = pricing_ptr->show_credit_fractions;
	
	if (pricing_ptr->currency == DOLLAR) {
		strcpy(pricing.lead_str, "$");
		pricing.trail_str[0] = '\0';
	} else if (pricing_ptr->currency == POUND) {
		strcpy(pricing.lead_str, "L");
		pricing.trail_str[0] = '\0';
	} else {
		pricing.lead_str[0] = '\0';
		strcpy(pricing.trail_str, currency[pricing_ptr->currency]);
	}
	strcpy(pricing.info_str, pricing_ptr->game_info);
	
	/* use to be set up in coin_init_totalizer */
	totalizer.mult[COIN_LEFT] = pricing_ptr->left_coin_count;
	totalizer.mult[COIN_RIGHT] = pricing_ptr->right_coin_count;
	totalizer.mult[COIN_CENTER] = pricing_ptr->center_coin_count;
	totalizer.mult[COIN_EXTRA] = pricing_ptr->extra_coin_count;
	totalizer.mult[COIN_DBV] = pricing_ptr->dbv_coin_count;
}	/* coin_init_pricing */

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: coin_init_state()                                               */
/*                                                                           */
/* Sets the coin state to startup values.                                    */
/*                                                                           */
/* Free play should be read from the adjustments in CMOS.                    */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 30 Jan 97                                                             */
/*                                                                           */
/*****************************************************************************/
void coin_init_state(void)
{
	union {
		dip_inputs_t di;
		int val;
	} ds;
	int fp_adj;
	
	/* assume NO free play */
	coin_state.free_play = FALSE;
	
	/* Get dipswitches */
	ds.val = get_dip_coin_current();
	
#ifdef FREEPLAY_ONLY
	fprintf(stderr, "FREE PLAY -- due to FREEPLAY_ONLY compile flag\n");
	coin_state.free_play = TRUE;
#else
	// Coinage set from switches OR error in CMOS adjustment for free play ?
	if(ds.di.coinage_mode == DIP_COINAGE_MODE_DIP || get_adjustment(FREEPLAY_ADJ, &fp_adj))
	{
		if(ds.di.coinage_mode != DIP_COINAGE_MODE_DIP)
			fprintf(stderr, "Error getting FREE PLAY from CMOS adjustment\r\n");

		fprintf(stderr, "Checking FREE PLAY from DIP SWITCH\r\n");

		// YES - Dip switch set to free play ?
		if(ds.di.dip_coinage == DIP_COINAGE_FREEPLAY)
		{
			fprintf(stderr, "Enabling FREE PLAY from DIP SWITCH\r\n");
			// YES - Set free play mode
			coin_state.free_play = TRUE;
		}
	}
	// Not using dip coinage AND free play adjustment NOT in error AND free
	// play adjustment set to free play
	else
	if(fp_adj)
	{
		fprintf(stderr, "Enabling FREE PLAY from CMOS adjustment\r\n");
		// Set free play mode
		coin_state.free_play = TRUE;
	}
#endif
	
	/* clear slam tilt flag */
	coin_state.slam = FALSE;
	
	/* show long coin message */
	coin_state.message_type = COIN_MESSAGE_LONG;
	
	/* do not show volume adjust message */
	coin_state.show_volume = FALSE;
	
	/* OK to make coin sounds and volume adjustment calls */
	coin_state.sound_holdoff = FALSE;
	
	/* no coin sounds to be made */
	coin_sound_count = 0;
}	/* coin_init_state */

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: coin_init_units()                                               */
/*                                                                           */
/* Sets the coin credits and units to 0.                                     */
/*                                                                           */
/* Credits values should be read from CMOS so credits aren't lost.           */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 30 Jan 97                                                             */
/*                                                                           */
/*****************************************************************************/
void coin_init_units(void)
{
	int reset_audit;
	
	reset_audit = 0;
	/* get how many credits there are */
	if (get_audit(CREDITS_AUD, &coins.credits)) {
		/* error reading CMOS */
		coins.credits = 0;
		reset_audit = 1;
	} else if (coins.credits < 0) {
		/* credits less than 0 */
		coins.credits = 0;
		reset_audit = 1;
	} else if (coins.credits > pricing.maximum_credits) {
		/* credits greater than max credits */
		coins.credits = pricing.maximum_credits;
		reset_audit = 1;
	}
	if (reset_audit)
		set_audit(CREDITS_AUD, coins.credits);
	
	coins.coin_units = 0;
	coins.bonus_units = 0;
	coins.remainder_units = 0;
}	/* coin_init_units */

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: coin_init_totalizer()                                           */
/*                                                                           */
/* Sets the coin totalizer (clicker) structure to known default values.      */
/*                                                                           */
/* Actual values should subsequently be read from CMOS.                      */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 30 Jan 97                                                             */
/*                                                                           */
/*****************************************************************************/
void coin_init_totalizer(void)
{
	/* totalizer struct is declared global in this file */
	totalizer.count[COIN_LEFT] = 0;
	totalizer.count[COIN_RIGHT] = 0;
	totalizer.count[COIN_CENTER] = 0;
	totalizer.count[COIN_EXTRA] = 0;
	totalizer.count[COIN_DBV] = 0;
	
	/* the totalizer.mult[] are now set up in coin_init_pricing */
	
	/* According to Cary Mednick we no longer support multiple coin counters */
	/* or anything but totalizing mode with multipliers 7/25/97. */
#ifdef TOTALIZE_ONLY
	/* All counts go to left coin counter */
	/* In totalizer mode these do not matter - code always goes to left counter */
	totalizer.output[COIN_LEFT] = COIN_CLICKER_MASK_LEFT;
	totalizer.output[COIN_RIGHT] = COIN_CLICKER_MASK_LEFT;
	totalizer.output[COIN_CENTER] = COIN_CLICKER_MASK_LEFT;
	totalizer.output[COIN_EXTRA] = COIN_CLICKER_MASK_LEFT;
	totalizer.output[COIN_DBV] = COIN_CLICKER_MASK_LEFT;
	
	/* Always use multipliers */
	totalizer.use_multipliers = TRUE;
	
	/* Always in totalizer mode */
	totalizer.mode = COUNT_MODE_TOTALIZER;
#else
	/* FIX - The setting of this comes from dipswitches */
	totalizer.output[COIN_LEFT] = COIN_CLICKER_MASK_LEFT;
	totalizer.output[COIN_RIGHT] = COIN_CLICKER_MASK_RIGHT;
	totalizer.output[COIN_CENTER] = COIN_CLICKER_MASK_CENTER;
	totalizer.output[COIN_EXTRA] = COIN_CLICKER_MASK_EXTRA;
	totalizer.output[COIN_DBV] = COIN_CLICKER_MASK_DBV;
	
	/* FIX - The setting of this comes from the adjustments */
	totalizer.use_multipliers = TRUE;
	
	/* FIX - Should be determined from dipswitches */
	if (get_adjustment(TOTALIZER_ADJ, &i))
		totalizer.mode = COUNT_MODE_TOTALIZER;
	else
		totalizer.mode = i ? COUNT_MODE_TOTALIZER : COUNT_MODE_CLICKER;
#endif
}	/* coin_init_totalizer */

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: coin_slam_handler()                                             */
/*                                                                           */
/* Called when the slam switch goes from open->closed.                       */
/*                                                                           */
/* Creates coin_slam_proc(), which is basically a counter to lock out any    */
/* new coins for about 1 second.                                             */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 30 Jan 97                                                             */
/*                                                                           */
/*****************************************************************************/
void coin_slam_handler(int sw_state, int sw_id)
{
	/* only trigger on transition from open->closed */
	/* and not already in slam processing code */
	if ((sw_state == SWITCH_CLOSE) && !coin_state.slam) {
		/* set in progress */
		coin_state.slam = TRUE;
		
		/* create the timer process (indestructable) */
		qcreate("slam", NODESTRUCT, coin_slam_proc, 0, 0, 0, 0);
	}
}	/* coin_slam_handler */

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: coin_slam_proc()                                                */
/*                                                                           */
/* Sleeps for COIN_SLAM_SLEEP ticks and then clears the "slam is happening"  */
/* variable.                                                                 */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 30 Jan 97                                                             */
/*                                                                           */
/*****************************************************************************/
void coin_slam_proc(int *arg)
{
	sleep(COIN_SLAM_SLEEP);
	coin_state.slam = FALSE;
}	/* coin_slam_proc */

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: coin_clicker_proc()                                             */
/*                                                                           */
/* Strobes the appropriate mechanical counter when a coin comes into a slot. */
/* The clicks happen in non-realtime, so a count is kept and decremented as  */
/* the clicks are done.                                                      */
/*                                                                           */
/* This proc is spawned by main_proc() and should never be killed.           */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 30 Jan 97                                                             */
/*                                                                           */
/*****************************************************************************/
void coin_clicker_proc(int *args)
{
	int	val, tmp, i;

	for (;;) {
		sleep(1);
		for (i = 0; i < COIN_NUM_SLOTS; i++) {
			if (totalizer.count[i] != 0) {
#ifndef VEGAS
				/* clicker mode - clicks go to assigned meters */
				/* totalizer mode - clicks all go to left meter */
				coin_strobe_clicker((totalizer.mode == COUNT_MODE_CLICKER) ? totalizer.output[i] : COIN_CLICKER_MASK_LEFT);
				totalizer.count[i]--;
#else
				if (totalizer.mode == COUNT_MODE_CLICKER) {
					switch (totalizer.output[i]) {
					case COIN_CLICKER_MASK_LEFT:
						_ioctl(6, CIOCADDCOUNT0, totalizer.count[i]);
						break;
					case COIN_CLICKER_MASK_RIGHT:
						_ioctl(6, CIOCADDCOUNT1, totalizer.count[i]);
						break;
					case COIN_CLICKER_MASK_CENTER:
						_ioctl(6, CIOCADDCOUNT2, totalizer.count[i]);
						break;
					case COIN_CLICKER_MASK_EXTRA:
						_ioctl(6, CIOCADDCOUNT3, totalizer.count[i]);
						break;
					default:
						_ioctl(6, CIOCADDCOUNT0, totalizer.count[i]);
						break;
					}
				} else
					_ioctl(6, CIOCADDCOUNT0, totalizer.count[i]);
				
				if (!get_audit(TOTAL_MONEY_AUDIT_NUM, &val)) {
					tmp = 100 * totalizer.count[i];
					val += (pricing.coins_per_bill <= 1) ? tmp : (tmp / pricing.coins_per_bill);
					set_audit(TOTAL_MONEY_AUDIT_NUM, val);
				}
				totalizer.count[i] = 0;
#endif
			}
		}
	}
}	/* coin_clicker_proc */

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: coin_strobe_clicker()                                           */
/*                                                                           */
/* Strobes the given clicker based on the bit field mask passed in.          */
/*                                                                           */
/* This function sleeps and is assumed to be called by a process.            */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 30 Jan 97                                                             */
/*                                                                           */
/*****************************************************************************/
#ifndef VEGAS
void coin_strobe_clicker(unsigned int mask)
{
	int	val;
	
	/* unlock the clickers */
	_ioctl(4, FIOCSETCOINMETERS, COIN_CLICKER_UNLOCK);
	
	/* strobe the clicker bits high */
	_ioctl(4, FIOCSETCOINMETERS, mask);
	
	if (!get_audit(TOTAL_MONEY_AUDIT_NUM, &val)) {
		val += (pricing.coins_per_bill <= 1) ? 100 : (100 / pricing.coins_per_bill);
		set_audit(TOTAL_MONEY_AUDIT_NUM, val);
	}
	
	/* delay */
	sleep(4);
	
	/* unlock the clickers */
	_ioctl(4, FIOCSETCOINMETERS, COIN_CLICKER_UNLOCK);
	
	/* strobe the clicker bits low */
	_ioctl(4, FIOCSETCOINMETERS, 0);
	
	/* delay */
	sleep(4);
}	/* coin_strobe_clicker */
#endif

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: coin_game_over()                                                */
/*                                                                           */
/* Should be called at game over.                                            */
/*                                                                           */
/* Clears bonus_units.                                                       */
/*                                                                           */
/* Resets the "insert coins" message to the appropriate state.               */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 30 Jan 97                                                             */
/*                                                                           */
/*****************************************************************************/
void coin_game_over(void)
{
	coins.bonus_units = 0;
	
	/* if the player has no units/coins in, then */
	/* reset the "insert coins" message */
	if (!coin_check_units())
		coin_change_mesg_state(COIN_MESSAGE_SHORT);
}	/* coin_game_over */

int check_credits_to_start(void)
{
	return coin_check_freeplay() || ((coins.credits + coins.service_credits) >= pricing.credits_to_start);
}	/* check_credits_to_start */

int check_credits_to_continue(void)
{
	return coin_check_freeplay() || ((coins.credits + coins.service_credits) >= pricing.credits_to_continue);
}	/* check_credits_to_continue */

int get_credits(void)
{
	return coin_check_freeplay() ? pricing.maximum_credits : (coins.credits + coins.service_credits);
}	/* get_credits */

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: coin_check_freeplay()                                           */
/*                                                                           */
/* Checks whether or not free play is set. This will be set either by the    */
/* adjustments read in from CMOS, or by the DIP switch settings.             */
/*                                                                           */
/* Common function for external modules to call.                             */
/*                                                                           */
/* Returns TRUE or FALSE.                                                    */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 04 Feb 97                                                             */
/*                                                                           */
/*****************************************************************************/
int coin_check_freeplay(void)
{
#ifdef FREEPLAY_ONLY
	return TRUE;
#else
	return coin_state.free_play;
#endif
}	/* coin_check_freeplay */

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: coin_check_units()                                              */
/*                                                                           */
/* Checks whether or not the player has any units accumulated, which is      */
/* more or less equivalent to whether or not the player has inserted any     */
/* coins.                                                                    */
/*                                                                           */
/* Returns TRUE or FALSE.                                                    */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 03 Feb 97                                                             */
/*                                                                           */
/*****************************************************************************/
int coin_check_units(void)
{
	return (coins.coin_units > 0) || (coins.remainder_units > 0) || (coins.credits > 0) || (coins.service_credits > 0);
}	/* coin_check_units */
		
/*****************************************************************************/
/*                                                                           */
/* FUNCTION: coin_show_insert_proc()                                         */
/*                                                                           */
/* Displays the "insert coins" message, which has a few states:              */
/*                                                                           */
/* 1. located either one line from bottom, or moved up to show credits info  */
/*                                                                           */
/* 2. text is "insert coins", "N credits to start", or "press start"         */
/*                                                                           */
/* 3. if in free play, message alternates between "free play" and "press     */
/*    start"                                                                 */
/*                                                                           */
/* If the game state changes out of attract mode, this process will kill     */
/* itself and clean up.                                                      */
/*                                                                           */
/* Uses special sleep function, coin_show_insert_sleep(), that does a check  */
/* each tick to see if the attract mode state or coin state has changed.     */
/*                                                                           */
/* See also coin_show_credits_proc().                                        */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 03 Feb 97                                                             */
/*                                                                           */
/*****************************************************************************/

void coin_show_insert_proc(int *args)
{
	ostring_t *os_prompt;			
	int prompt;
	int status;
	int sleep_on;
	int sleep_off;
	char *insert_coins_str = "INSERT COINS";
	char *press_start_str = "PRESS START";
	char *join_in_str = "JOIN IN";
	font_node_t	*fn = NULL;
	int was_low_res = is_low_res;
	
	if (coin_check_freeplay()) {
		if ((fn = create_font(&bast75_font, FONTID_BAST75)) == NULL) {
#ifdef DEBUG
			lockup();
#else
			return;
#endif
		}
		set_text_justification_mode(HJUST_CENTER | VJUST_CENTER);
		set_text_transparency_mode(TRANSPARENCY_ENABLE);
		set_string_id(TEXT_ID_COIN_MESG + 1);
		set_text_font(FONTID_BAST75);
		set_text_bgnd_color(BLACK);
		set_text_color(LT_YELLOW);
		
		if (is_low_res)
			is_low_res = 5;
		set_text_z_inc(-0.10F);
		set_text_position(SPRITE_HRES / 2.0f, SPRITE_VRES * 0.65f, 5.0f);
		oprintf("FREE");
		set_text_position(SPRITE_HRES / 2.0f, SPRITE_VRES * 0.45f, 5.0f);
		oprintf("PLAY");
		set_text_z_inc(0.0f);
		is_low_res = was_low_res;
	}
	
	/* set up text modes */
	set_text_justification_mode(HJUST_CENTER | VJUST_TOP);
	set_text_transparency_mode(TRANSPARENCY_ENABLE);
	set_text_id(TEXT_ID_COIN_MESG);
#ifdef USE_CHATTEN_FONT
 	set_text_font(FONTID_CHATTEN25);
#else
 	set_text_font(FONTID_NBA25);
#endif
	set_text_bgnd_color(BLACK);	
	set_text_color(LT_YELLOW);

	/* Allow each sequential letter to overlap the previous letter - turn off at end! */
	set_text_z_inc(-0.10F);
	
	/* init the string object */
	set_text_position(CNTR_X, COIN_MSG_Y6, 5.0F);
	
	/* create the string object */
	os_prompt = oputs(" ");
	os_prompt->string[0] = '\0';
	
	/* start off showing credits_to_start string */
	/* or "free play" if in free play */
	if (coin_check_freeplay())
		prompt = COIN_PROMPT_FREEPLAY;
	else
		prompt = COIN_PROMPT_CREDITS;	
	
	/*** sit in a loop and flash the message ***/
	for (;;) {
		set_text_z_inc(-0.10F);
		
		/* reset flash rate */
		sleep_on = COIN_INSERT_ON_TICKS;
		sleep_off = COIN_INSERT_OFF_TICKS;
		
		/* simplest case is free play, check that first */
		if (coin_check_freeplay()) {
			/* put freeplay close to bottom of screen */
			os_prompt->state.y = SPRITE_VRES * 0.25f;
			
			if (prompt == COIN_PROMPT_FREEPLAY)
				os_prompt->state.color = LT_YELLOW;
			if (prompt == COIN_PROMPT_START)
				os_prompt->state.color = WHITE;
			strcpy(os_prompt->string, press_start_str);
			os_prompt->string[0] = '\0';		/* override display of press start message on free play screen */
			
			/* toggle between _FREEPLAY and _START */
			prompt ^= 1;
		} else {
			/* no coins in yet, and flag says use short form message */
			/* just flash "insert coins", unless player is paid up */
	  		if (coin_state.message_type == COIN_MESSAGE_SHORT) {
				/* place message near bottom of screen */
				os_prompt->state.y = COIN_MSG_Y6;
				
				if (check_credits_to_start())
					strcpy(os_prompt->string, press_start_str);
				else
					strcpy(os_prompt->string, insert_coins_str);
			}
			
			/* at least one coin dropped, or flag says show long message */
			/* alternate "insert coins" with "credits to start" */
			/* prompt == 0: show "insert coins" */
			/* prompt == 1: show "N credits to start" */
			/* if player is paid up, flash "press start" */
			
		  	if ((coin_state.message_type == COIN_MESSAGE_LONG) ||
				(coin_check_units() == TRUE)) {
				/* move message up to make room for more info */
				os_prompt->state.y = COIN_MSG_Y6;
				
				if (check_credits_to_start()) {
					strcpy(os_prompt->string, press_start_str);
					/* if we are paidup, but not in freeplay, flash faster */
					sleep_on -= COIN_INSERT_SPEEDUP;
					sleep_off -= COIN_INSERT_SPEEDUP;
				} else {
					if (prompt == COIN_PROMPT_INSERT)
						strcpy(os_prompt->string, insert_coins_str);
					if (prompt == COIN_PROMPT_CREDITS)
						strcpy(os_prompt->string, join_in_str);
					
					/* toggle between _INSERT and _CREDITS */
				  	prompt ^= 1;
				}
			}
		}
		
		/* finally draw the actual string */
		change_string_state(os_prompt);
		status = coin_show_insert_sleep(sleep_on);
		if (status == ATT_DIE)
			break;
		
		/* if we need an immediate update, then do not sleep */
		if (status != ATT_CHANGE) {
	 		/* turn the string off */
	 		os_prompt->string[0] = '\0';
	 		change_string_state(os_prompt);
			
	 		/* if we change out of attract mode, cleanup and die */
	 		status = coin_show_insert_sleep(sleep_off);
			if (status == ATT_DIE)
				break;
		}
		set_text_z_inc(0.0f);
	}
	
	set_text_z_inc(0.0f);
	
	/* free the string obj */
	delete_string(os_prompt);
	
	/* Delete the free play font if it exists */
	if (fn) {
		delete_multiple_strings(TEXT_ID_COIN_MESG + 1, -1);
		delete_font(fn);
	}
}	/* coin_show_insert_proc */

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: coin_show_insert_sleep()                                        */
/*                                                                           */
/* Special sleep function for use by coin_show_insert_proc().                */
/*                                                                           */
/* 1. Sleeps the desired number of ticks. On wakeup...                       */
/*                                                                           */
/* 2. Checks the game state each tick to see if the game state has changed   */
/*    to anything but GS_ATTRACT_MODE.                                       */
/*                                                                           */
/* 3. Checks whether or not the coin message state has changed.              */
/*                                                                           */
/* Returns either ATT_LIVE (0), ATT_DIE (1), or ATT_CHANGE (2).              */
/*                                                                           */
/* ATT_CHANGE signals that the calling process needs to update its strings   */
/* immediately, since something has changed.                                 */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 04 Feb 97                                                             */
/*                                                                           */
/*****************************************************************************/
int coin_show_insert_sleep(int sleep_time)
{
	int i;
	int message_type;
	int check_units;
	int paid_up;
	
	for (i = 0; i < sleep_time; i++) {
		/* save the state */
		message_type = coin_state.message_type;
		/* save the coin state */
		check_units = coin_check_units();
		/* save the paidup status */
		paid_up = check_credits_to_start();
		
		/* go to sleep */
		sleep(1);
		
		/* check game state */
		if (game_info.game_state != GS_ATTRACT_MODE) {
			set_text_z_inc(0.0F);
			return ATT_DIE;		
		}
		
		/* free play is constant */
		if (!coin_check_freeplay()) {
			/* did message state change? */
			if (coin_state.message_type != message_type) {
				set_text_z_inc(0.0F);
				return ATT_CHANGE;
			}
			
			/* if player went from no coins to coins, change */
			if (coin_check_units() != check_units) {
				set_text_z_inc(0.0F);
				return ATT_CHANGE;
			}
			
			/* if player becomes paid up, change */
			if (check_credits_to_start() != paid_up) {
				set_text_z_inc(0.0F);
				return ATT_CHANGE;
			}
		}
	}
	set_text_z_inc(0.0F);
	return ATT_LIVE;
}	/* coin_show_insert_sleep */

static void purchase_flasher(int *args)
{
	ostring_t *pmsg[2];
	int pmsg_org_color[2];
	int set_white;
	
	pmsg[0] = (ostring_t *)args[0];
	pmsg[1] = (ostring_t *)args[1];
	
	pmsg_org_color[0] = pmsg[0]->state.color;
	pmsg_org_color[1] = pmsg[1]->state.color;
	set_white = TRUE;
	
	for (;;) {
		sleep(15);
		pmsg[0]->state.color = set_white ? WHITE : pmsg_org_color[0];
		pmsg[1]->state.color = set_white ? WHITE : pmsg_org_color[1];
		set_text_z_inc(-0.10F);
		change_string_state(pmsg[0]);
		change_string_state(pmsg[1]);
		set_text_z_inc(0.0F);
		set_white = !set_white;
	}
}	/* purchase_flasher */

void coin_credits_exit_func(process_node_t *myproc, int cause)
{
	if (pflasher != NULL) {
		kill(pflasher, 0);
		delete_string(pmsg[0]);
		delete_string(pmsg[1]);
		pflasher = NULL;
	}
	set_text_z_inc(0.0f);
}	/* coin_credits_exit_func */

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: coin_show_credits_proc()                                        */
/*                                                                           */
/* Once a coin has gone in, or when the "long form" credits message is in    */
/* effect, displays the running total of credits the player has and also     */
/* the pricing string.                                                       */
/*                                                                           */
/* If the game state changes out of attract mode, this process will kill     */
/* itself and clean up.                                                      */
/*                                                                           */
/* Uses special sleep function, coin_mesg_sleep(), that does a check each    */
/* tick to see if the attract mode state or coin state has changed.          */
/*                                                                           */
/* See also COIN_MSG_proc(), which also has #DEFINES that are shared */
/* with this function.                                                       */
/*                                                                           */
/* If game is in freeplay, then this process just immediately kills itself.  */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 03 Feb 97                                                             */
/*                                                                           */
/*****************************************************************************/
void coin_show_credits_proc(int *args)
{
	char info_line[4][32];
	char credits_str[COIN_MESG_STR_SIZE];
	ostring_t *os_free[] = {0,0,0,0,0,0};	/* current free play msg */
	ostring_t *os_free2[] = {0,0,0,0,0,0};	/* current free play msg */
	ostring_t *os_credits;					/* how many credits player has */
	ostring_t *os_ready;					/* ready for N players msg */
	ostring_t *os_game;						/* "free game" */
	ostring_t *os_quarter;					/* "free quarter" */
	ostring_t *os_last = 0;					/* last "free ..." displayed */
	int credits_whole;						/* how many whole credits the player has */
	int credits_numerator;					/* numerator of fractional credits */
	int credits_denominator;				/* denominator of fractional credits */
	char *ready_one_plr_str = "READY FOR 1 PLAYER";
	char *ready_two_plrs_str = "READY FOR 2 PLAYERS";
	char *ready_three_plrs_str = "READY FOR 3 PLAYERS";
	char *ready_four_plrs_str = "READY FOR 4 PLAYERS";
	int	lines = 1;
	char *tmp;
	char *to;
	int	i;
	float ypos;
	float yinc;
	int	status = -1;
	int	freecnt = 0;
	
	/* if in freeplay, kill ourselves */
	if (coin_check_freeplay())
		return;
	
	pflasher = NULL;
	
	/* Allow each sequential letter to overlap the previous letter - turn off at end! */
	set_text_z_inc(-0.1f);
	
	/* Are any of the free period/game modes enabled ? */
#ifdef USE_CHATTEN_FONT
 	set_text_font(FONTID_CHATTEN20);
#else
 	set_text_font(FONTID_NBA20);
#endif
	set_text_justification_mode(HJUST_CENTER | VJUST_TOP);
	set_text_transparency_mode(TRANSPARENCY_ENABLE);
	set_text_color(LT_GREEN);
	set_text_position(CNTR_X, FREE_MSG_Y1, 5.0f);
	
	os_game = oputs("FREE GAME");
	os_quarter = oputs("FREE QUARTER");
	hide_string( os_game );
	hide_string( os_quarter );
	
	set_text_position(CNTR_X, FREE_MSG_Y2, 5.0f);
	
	if ((!get_adjustment(HC_FREE_QUARTER_ADJ, &i)) && i) {
		os_free2[0] = os_quarter;
		os_free[0] = oputs(four_plr_ver ? "IF LEADERS VS CPU" : "IF LEADING VS CPU");
		hide_string( os_free[0] );
		status = 0;
	}
	if ((!get_adjustment(HH_FREE_QUARTER_ADJ, &i)) && i) {
		os_free2[1] = os_quarter;
		os_free[1] = oputs(four_plr_ver ? "IF LEADER OF A 1-ON-1" : "IF LEADING VS HUMAN");
		hide_string( os_free[1] );
		status = 1;
	}
	if ((!get_adjustment(H4_FREE_QUARTER_ADJ, &i)) && i && four_plr_ver) {
		os_free2[2] = os_quarter;
		os_free[2] = oputs("IF LEADERS OF A 4 PLAYER");
		hide_string( os_free[2] );
		status = 2;
	}
	if ((!get_adjustment(HC_FREE_GAME_ADJ, &i)) && i) {
		os_free2[3] = os_game;
		os_free[3] = oputs(four_plr_ver ? "IF WINNERS VS CPU" : "IF WINNER VS CPU");
		hide_string( os_free[3] );
		status = 3;
	}
	if ((!get_adjustment(HH_FREE_GAME_ADJ, &i)) && i) {
		os_free2[4] = os_game;
		os_free[4] = oputs(four_plr_ver ? "IF WINNER OF A 1-ON-1" : "IF WINNER VS HUMAN");
		hide_string( os_free[4] );
		status = 4;
	}
	if ((!get_adjustment(H4_FREE_GAME_ADJ, &i)) && i && four_plr_ver) {
		os_free2[5] = os_game;
		os_free[5] = oputs("IF WINNERS OF A 4 PLAYER");
		hide_string( os_free[5] );
		status = 5;
	}
	
	/* Set default font */
#ifdef USE_CHATTEN_FONT
 	set_text_font(FONTID_CHATTEN20);
	yinc = get_font_height(FONTID_CHATTEN20);	/* FONTID_BAST18 */
#else
 	set_text_font(FONTID_NBA20);
	yinc = get_font_height(FONTID_NBA20);	/* FONTID_BAST18 */
#endif
	set_text_justification_mode(HJUST_CENTER | VJUST_TOP);
	
	/* create the "N credits to purchase full game" string */
	/* If credits for full game is not zero - then it is valid */
	if (full_game_credits) {
		set_text_color(LT_RED);
		set_text_position(CNTR_X, FULL_MSG_Y1 + (is_low_res ? 3.0f : 0.0f), 5.0F);
		pmsg[0] = oprintf("BUY FULL GAME AT START");
		set_text_position(CNTR_X, FULL_MSG_Y2, 5.0F);
		pmsg[1] = oprintf("FOR ONLY %d CREDITS!", get_full_game_credits());
		pflasher = qcreate("pflash", 0, purchase_flasher, (int)pmsg[0], (int)pmsg[1], 0, 0);
	}

	/* print credits to start */
	ypos = COIN_MSG_Y2 + (is_low_res ? 5.0f : 0.0f);
	yinc += (is_low_res) ? 9.0f : 1.0f;
	
	set_text_position(CNTR_X, ypos, 5.0F);
	oprintf("%dc%d CREDIT%s TO START", WHITE, pricing.credits_to_start,
			pricing.credits_to_start > 1 ? "S" : "");
	
	/* print credits to continue */
	ypos -= yinc;
	set_text_position(CNTR_X, ypos, 5.0F);
	oprintf("%dc%d CREDIT%s TO CONTINUE", WHITE, pricing.credits_to_continue,
			pricing.credits_to_continue > 1 ? "S" : "");

	/* Figure out how many lines of pricing info there are */
	tmp = pricing.info_str;
	to = info_line[0];
	while (*tmp) {
		if (*tmp == '\n') {
			*to = 0;
			to = &info_line[lines][0];
			lines++;
		} else
			*to++ = *tmp;
		tmp++;
	}
	*to = 0;
	
	/* print pricing description */
	for(i = 0; i < lines; i++) {
		ypos -= yinc;
		set_text_position(CNTR_X, ypos, 10.0f - i);
		oprintf("%dc%s", WHITE, info_line[i]);
	}
	
	/* set up text modes */
	set_text_justification_mode(HJUST_CENTER | VJUST_TOP);
	set_text_transparency_mode(TRANSPARENCY_ENABLE);
	set_text_id(TEXT_ID_COIN_MESG);
#ifdef USE_CHATTEN_FONT
 	set_text_font(FONTID_CHATTEN20);
#else
 	set_text_font(FONTID_NBA20);
#endif
	set_text_bgnd_color(BLACK);	
	
	/* create string object for credits count */
	set_text_position(CNTR_X, COIN_MSG_Y1 + (is_low_res ? 2.0f : 0.0f), 5.0F);
	os_credits = oputs(" ");
	os_credits->string[0] = '\0';
	
	/* create string object for 'READY FOR N PLAYERS' */
	set_text_position(CNTR_X, COIN_MSG_Y5 - (is_low_res ? 1.0f : 0.0f), 5.0F);
	os_ready = oputs(" ");
	os_ready->string[0] = '\0';
	
	/* init the credit tally */
	credits_whole = 0;
	credits_numerator = 0;
	credits_denominator = 0;

	set_text_z_inc( 0.0f);

	/*
	 * Sit in a loop and update the message
	 */
	while (1)
	{
		/* at least one coin dropped, or flag says show long message */
		if ((coin_state.message_type == COIN_MESSAGE_LONG) || coin_check_units())
		{
			/* show how many credits the player has */
			/* deal with whole and fractional credit display */
			
			credits_whole = coins.credits + coins.service_credits;
			credits_denominator = pricing.units_per_credit;
			credits_numerator = coins.coin_units + coins.remainder_units;
			
			/* determine which 'READY FOR' message should be shown */
			if (credits_whole >= (4 * get_credits_to_start()) && (four_plr_ver))
				strcpy(os_ready->string, ready_four_plrs_str);
			else if (credits_whole >= (3 *get_credits_to_start()) && (four_plr_ver))
				strcpy(os_ready->string, ready_three_plrs_str);
			else if (credits_whole >= (2 * get_credits_to_start()))
				strcpy(os_ready->string, ready_two_plrs_str);
			else if (get_total_credits() >= get_credits_to_start())
				strcpy(os_ready->string, ready_one_plr_str);
			else 
				os_ready->string[0] = '\0';						/* show nothing (not enough credits for anything) */
			
			if ((pricing.show_credit_fractions) && (credits_numerator != 0))
			{
				/* display fractional credits */
				if (credits_whole == 0)
					sprintf(credits_str, "CREDITS: %d/%d", credits_numerator, 
							 credits_denominator);
				else
					sprintf(credits_str, "CREDITS: %d  %d/%d", credits_whole,
							 credits_numerator, credits_denominator);
			}
			else
			{
				/* display whole integer credits only */
				if (credits_whole == 1)
					sprintf(credits_str, "CREDITS: %d", credits_whole);
				else {
					if (credits_whole >= pricing.maximum_credits)
						sprintf(credits_str, "CREDITS: %d (MAX)", credits_whole);
					else
						sprintf(credits_str, "CREDITS: %d", credits_whole);
				}
			}
			/* update the strings */
			strcpy(os_credits->string, credits_str);
		}
		else
		{
			/* hide the messages */
			os_credits->string[0] = '\0';
		}
		
		/*
		 * Show each msg as often as possible
		 * Current <credit_page> time is (about) 12*57+76=760
		 * which should provide a cycle count of (about) 6
		 * with the current freecnt timeouts
		 */
		if (status >= 0)
		{
			if (!freecnt++)
			{
				unhide_string( os_free[status] );
				unhide_string( os_free2[status] );
				os_last = os_free2[status];
			}
			else
			if (freecnt == (116))
			{
				hide_string( os_free[status] );
				do
					if (--status < 0)
						status = (sizeof(os_free)/sizeof(void *) - 1);
				while (!os_free[status]);

				if (os_last != os_free2[status])
					hide_string( os_last );
			}
			else
			if (freecnt == (116+12))
				freecnt = 0;
		}
		
		/* force the redraw */
		set_text_z_inc(-0.1f);
		os_ready->state.color = LT_YELLOW;
		change_string_state(os_credits);
		change_string_state(os_ready);
		set_text_z_inc( 0.0f);
		
	    if (coin_show_credits_sleep(1) == ATT_DIE)
			break;
	}
	
	/*
	 * BUG BUG BUG
	 * NOTE - This shit never gets done because this process is killed by
	 * another process!!!!!
	 */
	/* free the string objs */
	delete_string(os_credits);
	delete_string(os_ready);
	
	/* Stop the sequential letter to overlap mode */
	set_text_z_inc(0.0f);
}	/* coin_show_credits_proc */

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: coin_show_credits_sleep()                                       */
/*                                                                           */
/* Special sleep function for use by coin_show_credits_proc().               */
/*                                                                           */
/* 1. Sleeps the desired number of ticks. On wakeup...                       */
/*                                                                           */
/* 2. Checks the game state each tick to see if the game state has changed   */
/*    to anything but GS_ATTRACT_MODE.                                       */
/*                                                                           */
/* 3. Checks whether or not the coin state or coin count has changed.        */
/*                                                                           */
/* Returns either ATT_LIVE (0), ATT_DIE (1), or ATT_CHANGE (2).              */
/*                                                                           */
/* ATT_CHANGE signals that the calling process needs to update its strings   */
/* immediately, since something has changed.                                 */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 04 Feb 97                                                             */
/*                                                                           */
/*****************************************************************************/
int coin_show_credits_sleep(int sleep_time)
{
	int i;
	int message_type;
	int coin_units;
	int remainder_units;
	int total_credits;
	
	for (i = 0; i < sleep_time; i++) {
		/* save the state */
		message_type = coin_state.message_type;
		total_credits = coins.credits + coins.service_credits;
		coin_units = coins.coin_units;
		remainder_units = coins.remainder_units;
		
		/* go to sleep */
		sleep(1);
		
		/* check game state */
		if (game_info.game_state != GS_ATTRACT_MODE)
			return ATT_DIE;		
		
		/* did any coin or credit info change? */
		if (coin_state.message_type != message_type)
			return ATT_CHANGE;
		
		if ((coins.credits + coins.service_credits) != total_credits)
			return ATT_CHANGE;
		
		if (coins.coin_units != coin_units)
			return ATT_CHANGE;
		
		if (coins.remainder_units != remainder_units)
			return ATT_CHANGE;
	}
	return ATT_LIVE;
}	/* coin_show_credits_sleep */

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: coin_change_mesg_state()                                        */
/*                                                                           */
/* Common function to change coin state. Intended use is to force the        */
/* display of all three lines of coin and credit info for the credits page   */
/* in attract mode.                                                          */
/*                                                                           */
/* See also coin_show_insert_proc() and coin_show_credits_proc().            */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 03 Feb 97                                                             */
/*                                                                           */
/*****************************************************************************/
void coin_change_mesg_state(int state)
{
	coin_state.message_type = state;
}	/* coin_change_mesg_state */

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: coin_volume_handler()                                           */
/*                                                                           */
/* This function gets called whenever the volume up or down buttons are hit  */
/* during the game. All it does is recharge the flag, and coin_volume_proc() */
/* actually displays the message and updates the volume.                     */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 03 Feb 97                                                             */
/*                                                                           */
/*****************************************************************************/

void coin_volume_handler(int sw_state, int sw_id)
{
	if (sw_state == SWITCH_CLOSE)
		coin_state.show_volume = COIN_VOLUME_DISPLAY_TIME * get_tsec();
}	/* coin_volume_handler */

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: coin_volume_proc()                                              */
/*                                                                           */
/* When the volume up or down switches are hit, coin_volume_handler() sets   */
/* coin_state.show_volume to the full re-charge value. As long as it is      */
/* non-zero, the volume level is displayed and updated.                      */
/*                                                                           */
/* If sounds are being downloaded to the sound system, then no sound calls   */
/* should be made, so the volume adjustment sound calls are held off.        */
/*                                                                           */
/* This proc is spawned by main_proc() and should never be killed.           */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

void coin_volume_proc(int *args)
{
	int sw_state;
	int update;
	int display_ticks;
	int *volume_ptr;
	int other_value;
	char *volume_str, *other_str;
	int adj;
	float old_zinc;
	struct text_state_info tsi;
	
	display_ticks = COIN_VOLUME_DISPLAY_TIME * get_tsec();
	for (;;) {
		/* if downloading sounds, do not allow volume adjustment */
		if (coin_state.sound_holdoff)
			coin_state.show_volume = 0;
		
		if (coin_state.show_volume > 0) {
			update = FALSE;
			
			/* adjust the volume */
			sw_state = get_dip_coin_current();
			
			if (game_info.game_state == GS_ATTRACT_MODE) {
				volume_ptr = &game_info.attract_mode_volume;
				other_value = game_info.master_volume;
				adj = ATTRACT_VOLUME_LEVEL_ADJ;
				volume_str = "ATTRACT MODE";
				other_str = "GAME MODE";
			} else {
				volume_ptr = &game_info.master_volume;
				other_value = game_info.attract_mode_volume;
				adj = VOLUME_LEVEL_ADJ;
				volume_str = "GAME MODE";
				other_str = "ATTRACT MODE";
			}
			if (sw_state & VOLUME_UP_SW) {
				/* recharge as long as button is held */
				coin_state.show_volume = display_ticks;
				if (*volume_ptr < SND_VOLUME_MAX) {
					(*volume_ptr)++;
					set_adjustment(adj, *volume_ptr);
					update = TRUE;
				}
			}
			if (sw_state & VOLUME_DOWN_SW) {
				/* recharge as long as button is held */
				coin_state.show_volume = display_ticks;
				if ((*volume_ptr > SND_VOLUME_MIN) && (*volume_ptr > game_info.minimum_volume)) {
					(*volume_ptr)--;
					set_adjustment(adj, *volume_ptr);
					update = TRUE;
				}
			}
			if (update) {
				get_text_state(&tsi);
				old_zinc = get_text_z_inc();
				set_text_z_inc(0.0F);
				delete_multiple_strings(TEXT_ID_VOLUME_MESG, 0xFFFFFFFF);		/* delete volume msg */
				set_string_id(TEXT_ID_VOLUME_MESG);
				set_text_font(FONTID_BAST8T);
				set_text_justification_mode(HJUST_CENTER | VJUST_CENTER);
				set_text_position(COIN_VOLUME_X, COIN_VOLUME_Y, 1.0F);
				oprintf("%dc%s VOLUME: %d", WHITE, volume_str, *volume_ptr);
				set_text_position(COIN_VOLUME_X, COIN_VOLUME_Y2, 1.0F);
				oprintf("%dc%s VOLUME: %d", LT_YELLOW, other_str, other_value);
				set_text_state(&tsi);
				set_text_z_inc(old_zinc);
				
			  	snd_master_volume(*volume_ptr);
			}
			coin_state.show_volume--;
			if (coin_state.show_volume == 0)
				delete_multiple_strings(TEXT_ID_VOLUME_MESG, 0xFFFFFFFF);		/* delete volume msg */
		}
		sleep(1);
	}
}	/* coin_volume_proc */

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: coin_sound_holdoff()                                            */
/*                                                                           */
/* Sets the coin_state.sound_holdoff flag on or off. This allows master      */
/* volume updates and coin drop sounds to be held off when needed,           */
/* especially while downloading banks.                                       */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 16 Jan 97                                                             */
/*                                                                           */
/*****************************************************************************/
void coin_sound_holdoff(int flag)
{
	coin_state.sound_holdoff = flag;
}	/* coin_sound_holdoff */

/*
 * This routine decrements the TOTAL_CREDITS by a passed in parameter
 *
 * INPUT: Number of credits to remove from credit count
 * RETURN: none
 */
void decrement_credits(short dec_cnt)
{
	int	val, real_credit_count;
	
	if (coin_check_freeplay())
		return;
	
	/* eat service credits first */
	while ((dec_cnt > 0) && (coins.service_credits > 0)) {
		coins.service_credits--;
		dec_cnt--;
	}
	real_credit_count = dec_cnt;
	
	/* eat real credits next */
	while ((dec_cnt > 0) && (coins.credits > 0)) {
		coins.credits--;
		dec_cnt--;
	}
	
	/* Increment total paid credits audit */
	get_audit(TOTAL_PAID_CREDITS_AUD, &val);
	val += real_credit_count;
	set_audit(TOTAL_PAID_CREDITS_AUD, val);
	
	if (coins.credits > 0)
		/* Save credits available */
		set_audit(CREDITS_AUD, coins.credits);
	else
		set_audit(CREDITS_AUD, 0);
}	/* decrement_credits */

int	get_credits_to_start(void)
{
	return pricing.credits_to_start;
}	/* get_credits_to_start */

int	get_credits_to_continue(void)
{
	return pricing.credits_to_continue;
}	/* get_credits_to_continue */

int	get_total_credits(void)
{
	return coins.credits + coins.service_credits;
}	/* get_total_credits */

int get_full_game_credits(void)
{
	return full_game_credits;
}	/* get_full_game_credits */
