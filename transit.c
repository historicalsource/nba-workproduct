/*
 *		$Archive: /video/Nba/transit.c $
 *		$Revision: 15 $
 *		$Date: 3/08/99 12:23a $
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
char *ss_transit_c = "$Workfile: transit.c $ $Revision: 15 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include <stdlib.h>
#include <math.h>
#include <goose/goose.h>

/*
 *		USER INCLUDES
 */

#include "transit.h"
#include "game.h"
#include "amode.h"
#include "id.h"
#include "sndcalls.h"

/*
 *		DEFINES
 */

#define NUM_STRIP			9
#define SPRITE_PER_STRIP	3
#define NUM_SPRITES			(NUM_STRIP * SPRITE_PER_STRIP)
#define TRANS_Z				1.0f

#define	P_ABC_MASK			(P_A|P_B|P_C)
#define	P1_ABC_MASK			(P_ABCD_MASK << (P1_SHIFT))
#define	P2_ABC_MASK			(P_ABCD_MASK << (P2_SHIFT))
#define	P3_ABC_MASK			(P_ABCD_MASK << (P3_SHIFT))
#define	P4_ABC_MASK			(P_ABCD_MASK << (P4_SHIFT))

typedef struct {
	int start_x, end_x, delta_x;
	int start_y, end_y, delta_y;
	int tick_count, delay_count;
} trans_strip;

enum {
	HORZ_LEFT_TOP,
	HORZ_RIGHT_TOP,
	HORZ_ZIG_ZAG_LEFT,
	HORZ_ZIG_ZAG_RIGHT,
	HORZ_LEFT_BOTTOM,
	HORZ_RIGHT_BOTTOM,
	WEDGE_INNER_LEFT,
	WEDGE_INNER_RIGHT,
	VERT_TOP,
	VERT_BOTTOM,
	SCISSOR_LEFT,
	SCISSOR_RIGHT,
	WEIRD1,
	WEIRD2,
	WEDGE_OUTTER_LEFT,
	WEDGE_OUTTER_RIGHT,
	NUM_TRANS
};

extern char plyr_bnk_str[];

static void get_sprite_pos(sprite_info_t *sprite, int *x, int *y);
static void set_sprite_pos(sprite_info_t *sprite, int x, int y);
static int build_trans(int which_trans, int transit_mode, trans_strip *strip_data);

/*
 *		GLOBAL VARIABLES
 */

/* Use this globally to track transition state */
volatile int plates_on = PLATES_OFF;

static sprite_info_t *plate_obj[NUM_SPRITES] = {
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};
static image_info_t *image_list[NUM_SPRITES] = {
	&tr_nbacb, &tr_nbacb_c1, &tr_nbacb_c2,
	&tr_plain, &tr_plain_c1, &tr_plain_c2,
	&tr_nbacb, &tr_nbacb_c1, &tr_nbacb_c2,
	&tr_nba, &tr_nba_c1, &tr_nba_c2,
	&tr_nbacb, &tr_nbacb_c1, &tr_nbacb_c2,
	&tr_nbc, &tr_nbc_c1, &tr_nbc_c2,
	&tr_nbacb, &tr_nbacb_c1, &tr_nbacb_c2,
	&tr_plain, &tr_plain_c1, &tr_plain_c2,
	&tr_nbacb, &tr_nbacb_c1, &tr_nbacb_c2
};

static void get_sprite_pos(sprite_info_t *sprite, int *x, int *y)
{
	*x = sprite->x;
	*y = sprite->y;
}	/* get_sprite_pos */

static void set_sprite_pos(sprite_info_t *sprite, int x, int y)
{
	sprite->x = x;
	sprite->y = y;
	generate_sprite_verts(sprite);
}	/* set_sprite_pos */

static int build_trans(int which_trans, int transit_mode, trans_strip *strip_data)
{
	static int y_table[NUM_STRIP] = {	
		SPRITE_VRES - -47,
		SPRITE_VRES -   6,
		SPRITE_VRES -  59,
		SPRITE_VRES - 112,
		SPRITE_VRES - 165,
		SPRITE_VRES - 218,
		SPRITE_VRES - 271,
		SPRITE_VRES - 324,
		SPRITE_VRES - 377
	};
	int i, tmp, delay_time;
	int top_to_bottom, right_to_left, vert, zig_zag, wedge, weird;
	
	wedge = (which_trans == WEDGE_OUTTER_LEFT) || (which_trans == WEDGE_OUTTER_RIGHT) ||
		(which_trans == WEDGE_INNER_LEFT) || (which_trans == WEDGE_INNER_RIGHT) ||
		(which_trans == SCISSOR_LEFT) || (which_trans == SCISSOR_RIGHT);
	top_to_bottom = (which_trans == HORZ_LEFT_TOP) || (which_trans == HORZ_RIGHT_TOP) ||
		(which_trans == HORZ_ZIG_ZAG_LEFT) || (which_trans == HORZ_ZIG_ZAG_RIGHT) ||
		(which_trans == VERT_TOP) || (which_trans == WEIRD1) || wedge;
	right_to_left = (which_trans == HORZ_LEFT_TOP) || (which_trans == HORZ_LEFT_BOTTOM) ||
		(which_trans == HORZ_ZIG_ZAG_LEFT) || (which_trans == WEDGE_OUTTER_LEFT) ||
		(which_trans == WEDGE_INNER_LEFT) || (which_trans == SCISSOR_LEFT);
	weird = (which_trans == WEIRD1) || (which_trans == WEIRD2);
	zig_zag = (which_trans == HORZ_ZIG_ZAG_LEFT) || (which_trans == HORZ_ZIG_ZAG_RIGHT) ||
		weird || (which_trans == SCISSOR_LEFT) || (which_trans == SCISSOR_RIGHT);
	vert = (which_trans == VERT_TOP) || (which_trans == VERT_BOTTOM);
	delay_time = 0;
	for (i = 0; i < NUM_STRIP; i++) {
		if (weird) {
			strip_data[i].start_x = right_to_left ? (SPRITE_HRES + 21) : (-SPRITE_HRES - 21);
			strip_data[i].end_x = 0;
			strip_data[i].start_y = top_to_bottom ? -64 : 0;
			strip_data[i].end_y = y_table[i];
		} else if (vert) {
			strip_data[i].start_x = 0;
			strip_data[i].end_x = 0;
			strip_data[i].start_y = top_to_bottom ? -64 : 0;
			strip_data[i].end_y = y_table[i];
		} else {
			strip_data[i].start_x = right_to_left ? (SPRITE_HRES + 21) : (-SPRITE_HRES - 21);
			strip_data[i].end_x = 0;
			strip_data[i].start_y = y_table[i];
			strip_data[i].end_y = y_table[i];
		}
		/* swap direction for plates coming off */
		if (transit_mode == PLATES_COMING_OFF) {
			tmp = strip_data[i].start_x;
			strip_data[i].start_x = strip_data[i].end_x;
			strip_data[i].end_x = tmp;
			
			tmp = strip_data[i].start_y;
			strip_data[i].start_y = strip_data[i].end_y;
			strip_data[i].end_y = tmp;
		}
		tmp = get_player_sw_current();
		if (((tmp & P1_ABC_MASK) == P1_ABC_MASK) || ((tmp & P2_ABC_MASK) == P2_ABC_MASK) ||
			((tmp & P3_ABC_MASK) == P3_ABC_MASK) || ((tmp & P4_ABC_MASK) == P4_ABC_MASK)) {
			strip_data[i].tick_count = 7;
		} else
			strip_data[i].tick_count = 14;
		strip_data[i].delay_count = top_to_bottom ? (i * 3) : (24 - (i * 3));
		strip_data[i].delta_x = (strip_data[i].end_x - strip_data[i].start_x) / (float)strip_data[i].tick_count;
		strip_data[i].delta_y = (strip_data[i].end_y - strip_data[i].start_y) / (float)strip_data[i].tick_count;
		if (zig_zag)
			right_to_left = !right_to_left;
		tmp = strip_data[i].tick_count + strip_data[i].delay_count;
		if (tmp > delay_time)
			delay_time = tmp;
	}
	if (wedge)
		for (i = 0 ; i <= 4; i++)
			strip_data[i].delay_count = strip_data[8 - i].delay_count = ((which_trans == WEDGE_INNER_LEFT) || (which_trans == WEDGE_INNER_RIGHT)) ? 16 - (i * 4) : (i * 4);
	return delay_time;
}	/* build_trans */

/*
 *		GLOBAL FUNCTIONS
 */

void create_plates(void)
{
	trans_strip the_trans[NUM_STRIP];
	int i, j, index;
	
	build_trans(HORZ_LEFT_TOP, PLATES_COMING_OFF, the_trans);
	for (i = 0; i < NUM_STRIP; i++) {
		for (j = 0; j < SPRITE_PER_STRIP; j++) {
			index = (i * SPRITE_PER_STRIP) + j;
			plate_obj[index] = beginobj(image_list[index], 0.0f, SPRITE_VRES, TRANS_Z + (0.01f * i), TRANS_TID);
			plate_obj[index]->id = 2;
			plate_obj[index]->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
			set_sprite_pos(plate_obj[index], the_trans[i].start_x, the_trans[i].start_y);
		}
	}
}	/* create_plates */

void kill_plates(void)
{
	int i;
	
	for (i = 0; i < NUM_SPRITES; i++)
		if (plate_obj[i] != NULL) {
			delobj(plate_obj[i]);
			plate_obj[i] = NULL;
		}
	
	/* set plates flag */
	plates_on = PLATES_OFF;
}	/* kill_plates */

void transit_proc(int *args)
{
	static int last_one = NUM_TRANS;
	trans_strip the_trans[NUM_STRIP];
	int i, j, index, tcount, free_plates, total_delay;
	int any_null, all_null;
	
	any_null = FALSE;
	all_null = TRUE;
	for (i = 0; i < NUM_SPRITES; i++) {
		if (plate_obj[i] == NULL)
			any_null = TRUE;
		else
			all_null = FALSE;
	}
	
	/* make sure the object exist */
	if (any_null) {
		if (all_null) {
			/* create the objects */
			create_plates();
			
			/* Set plates flag */
			plates_on = PLATES_COMING_ON;
			
			/* Lets see them */
			draw_enable(1);
			normal_screen();
		} else {
#if DEBUG
			lockup();
#endif
			kill_plates();
			return;
		}
	} else {
		/* Set plates flag */
		plates_on = PLATES_COMING_OFF;
	}

	last_one = (last_one == NUM_TRANS) ? 0 : last_one + 1;
	total_delay = build_trans(last_one, plates_on, the_trans);
	/* place the sprites in the starting spot */
	for (i = 0; i < NUM_STRIP; i++)
		for (j = 0; j < SPRITE_PER_STRIP; j++)
			set_sprite_pos(plate_obj[(i * SPRITE_PER_STRIP) + j], the_trans[i].start_x, the_trans[i].start_y);
	
	/* Initialize the tick counter */
	tcount = 0;
	
	/* move the plates */
	for (;;) {
		if ((plates_on == PLATES_COMING_ON) || (plates_on == PLATES_COMING_OFF)) {
			for (i = 0; i < NUM_STRIP; i++) {
				if (the_trans[i].delay_count == 0) {
					if (the_trans[i].tick_count != 0) {
						the_trans[i].tick_count--;
						for (j = 0; j < SPRITE_PER_STRIP; j++) {
							index = (i * SPRITE_PER_STRIP) + j;
							if (the_trans[i].tick_count == 0) {
								set_sprite_pos(plate_obj[index], the_trans[i].end_x, the_trans[i].end_y);
//								if ((plates_on == PLATES_COMING_ON) && ((i == 1) || (i == 3) || (i == 5) || (i == 7)))
//									snd_scall_bank(screen_trans_bnk_str, EXPLOSION2_SND, 10, 127, PRIORITY_LVL4);
							} else {
								int x, y;
								
								get_sprite_pos(plate_obj[index], &x, &y);
								set_sprite_pos(plate_obj[index], x + the_trans[i].delta_x, y + the_trans[i].delta_y);
							}
						}
					}
				} else {
					if (--the_trans[i].delay_count == 0)
						if ((i & 1) == 0)
							snd_scall_bank(plyr_bnk_str, WHOOSH_SND, VOLUME2, 127, PRIORITY_LVL4);
				}
			}
			sleep(1);
		}
		
		if ((++tcount == total_delay) || (plates_on == PLATES_OFF)) {
			if ((plates_on == PLATES_COMING_ON) || (plates_on == PLATES_COMING_OFF)) {
				if (plates_on == PLATES_COMING_ON) {
//					snd_scall_bank(screen_trans_bnk_str, EXPLOSION2_SND, 10, 127, PRIORITY_LVL4);
					plates_on = PLATES_ON;				
					free_plates = FALSE;
				} else
					free_plates = TRUE;
				
			} else
				free_plates = TRUE;
			
			if (free_plates)
				kill_plates();
			break;
		}
	}
}	/* transit_proc */

void turn_transit_on()
{
	/* Take care of proper handling of transition */
	if (plates_on != PLATES_ON) {
		/* Enable drawing since we know something is being or will be drawn */
		draw_enable(1);
		
		if (plates_on != PLATES_COMING_ON) {
			if (plates_on != PLATES_OFF) {
				/* Do it this way so we can sync-up the sound */
				while (plates_on != PLATES_OFF)
					sleep(1);
				
				/* Make sure plates go away */
				plates_on = PLATES_OFF;
				sleep(1);
			}
			/* Turn on the plates */
			iqcreate("transit", TRANSIT_PID, transit_proc, 0, 0, 0, 0);
		}
		/* Do it this way so we can sync-up the sound */
		while (plates_on != PLATES_ON)
			sleep(1);
	}
}	/* turn_transit_on */

void turn_transit_off(int sleep_until_off)
{
	if ((plates_on != PLATES_OFF) && (plates_on != PLATES_COMING_OFF)) {
		iqcreate("transit", TRANSIT_PID, transit_proc, 0, 0, 0, 0);
		if (sleep_until_off)
			while (plates_on != PLATES_OFF)
				sleep(1);
	}
}	/* turn_transit_off */
