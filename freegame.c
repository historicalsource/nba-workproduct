#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <dir.h>
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif

#include "include/fontid.h"
#include "include/game.h"
#include "include/nbacmos.h"
#include "include/select.h"
#include "include/sndcalls.h"
#include "include/externs.h"
#include "include/id.h"
#include "include/gameadj.h"
#include "include/transit.h"
//#include "include/trivia.h"

extern image_info_t playresult;

extern pdata_t pdata_blocks[];
extern int fullgame_bought_ok;

extern tmu_ram_texture_t opt_scrn_textures_tbl[];

extern float station_xys[MAX_PLYRS][2][2];
extern float japbx_cntr_xys[MAX_PLYRS][2][2];

extern image_info_t *count_down_imgs[];

extern char plyr_bnk_str[];
extern char cursor_bnk_str[];
extern int	tsec;
extern int	four_plr_ver;
extern int  bought_in_first;

void flash_alpha_text_proc(int *args);
void flash_text_proc(int *args);
void plyr_strt_butn(int sig, int swid);


int show_player_free_game(int pnum);

static int	timeout;


//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
static void dummy(int sig, int id)
{
}

//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
static void free_game_timer(int *args)
{
	void * old_dcsw[MAX_PLYRS];
	int	pnum = args[0];
	int	seconds = 9;
	int	sw_mask = 0;
	int	half_second;
	int	sw = 0;
	sprite_info_t * digit_obj;
	int	was_low_res = is_low_res;


	old_dcsw[0] = set_dcsw_handler(P1_START_SWID, dummy);
	old_dcsw[1] = set_dcsw_handler(P2_START_SWID, dummy);
	if (four_plr_ver)
	{
		old_dcsw[2] = set_dcsw_handler(P3_START_SWID, dummy);
		old_dcsw[3] = set_dcsw_handler(P4_START_SWID, dummy);
	}

	if (pnum & 1)
		sw_mask |= P1_START_SW;
	if (!four_plr_ver)
	{
		if (pnum & 4)
			sw_mask |= P2_START_SW;
	}
	else
	{
		if (pnum & 2)
			sw_mask |= P2_START_SW;
		if (pnum & 4)
			sw_mask |= P3_START_SW;
		if (pnum & 8)
			sw_mask |= P4_START_SW;
	}

	digit_obj = beginobj(count_down_imgs[seconds],SPRITE_HRES/2.0f,(is_low_res?60.6F:90.0F),3.5f,CNTDWN_TID);
	digit_obj->id = OID_CNTDWN_DIGIT;

	while(--seconds >= 0)
	{
//		set_text_position(SPRITE_HRES/2.0f, 81.0f +SPRITE_VRES/384.0f, 1.1f);
//		set_text_color(LT_CYAN);
//		set_text_font(FONTID_NBA20);
//		set_text_justification_mode(HJUST_CENTER|VJUST_CENTER);
//		set_string_id(0x55a9);
//
//		oprintf("%d", seconds);

		is_low_res = 0;
		change_img_tmu(digit_obj,count_down_imgs[seconds],CNTDWN_TID);
		is_low_res = was_low_res;

		unhide_sprite(digit_obj);

		snd_scall_bank(cursor_bnk_str,TIMEDOWN_SND,VOLUME4,127,PRIORITY_LVL2);
		half_second = tsec;
		while(--half_second && !((sw = get_dip_coin_current() & sw_mask)))
		{
			sleep(1);
		}

//		delete_multiple_strings(0x55a9, -1);
		hide_sprite(digit_obj);

		if (sw) break;

		snd_scall_bank(cursor_bnk_str,TIMEDOWN_SND,VOLUME4,127,PRIORITY_LVL2);
		half_second = 25;
		while(--half_second && !((sw = get_dip_coin_current() & sw_mask)))
		{
			sleep(1);
		}
		if (sw) break;
	}

	set_dcsw_handler(P1_START_SWID, old_dcsw[0]);
	set_dcsw_handler(P2_START_SWID, old_dcsw[1]);
	if (four_plr_ver)
	{
		set_dcsw_handler(P3_START_SWID, old_dcsw[2]);
		set_dcsw_handler(P4_START_SWID, old_dcsw[3]);
	}

	bought_in_first = 0;

	if (sw)
	{
		if (sw & P1_START_SW)
			bought_in_first = 1;
		else
		if (sw & P2_START_SW)
			bought_in_first = (four_plr_ver?2:4);
		else
		if (sw & P3_START_SW)
			bought_in_first = 4;
		else
		if (sw & P4_START_SW)
			bought_in_first = 8;

		timeout = -1;
	}
	else
	{
		timeout = 1;
	}
	del1c(OID_CNTDWN_DIGIT,-1);
}

#ifdef DEBUG
void show_player_free_game_test(int min, int max);
void show_player_free_game_test(int min, int max)
{
	int i;
	draw_enable(1);

	// Turn on the plates
	turn_transit_on();

	while(1)
	{
		int sw = get_player_sw_current();
		if (!four_plr_ver)
			sw = (sw & 0x0ff) | ((sw & 0x0ff00) << 8);
		if (sw)
		{
			int pnum = 0;
			int p = 1;
			while (sw)
			{
				if (sw & (P_A|P_B|P_C)) pnum |= p;
				sw >>= 8;
				p <<= 1;
			}
			turn_transit_off(0);
			show_player_free_game(pnum);
		}
		sleep(2);
	}
}
#endif

//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
int show_player_free_game(int pnum)
{
	process_node_t * pflasher;
	sprite_info_t * psprite;
	ostring_t * pmsg0;
	ostring_t * pmsg1;
	float ypos, yinc;
	float fx, fy;
	int i, j;

	// Kill everything but the plates
	//if ((plates_on == PLATES_OFF) || (plates_on == PLATES_COMING_OFF))
	//	turn_transit_on();

	draw_enable(0);

	wipeout();

	// Re-create the wipeout'd plate objects
	//create_plates();


	if (load_textures_into_tmu(opt_scrn_textures_tbl))
	{
#ifdef DEBUG
		fprintf(stderr, "Couldn't load all free game screen textures into tmu\r\n");
		lockup();
#else
		return 0;
#endif
	}

	// Put up the background
	draw_backdrop();

	// Set up all of the text drawing stuff
	set_text_font(FONTID_NBA25);
	set_text_justification_mode(HJUST_CENTER|VJUST_CENTER);
	set_string_id(0x55aa);
	set_text_z_inc(0.1f);

//	printf("val passed in %d\n",pnum);

	switch (pnum)
	{
		case 4:
		case 12:
			i = LT_RED;
			break;
		case 8:
			i = LT_YELLOW;
			break;
		case 2:
			i = LT_GREEN;
			break;
		default:
			i = LT_BLUE;
			break;
	}
	set_text_color(i);

	yinc = (float)get_font_height(FONTID_NBA25) + 3.0f;

	ypos = 306.0f +SPRITE_VRES/384.0f;
	set_text_position(SPRITE_HRES/2.0f, ypos, 40.0f);
	pmsg0 = oprintf("CONGRATULATIONS");

	ypos -= yinc;
	if (is_low_res) ypos -= 8.0f;
	set_text_position(SPRITE_HRES/2.0f, ypos, 30.0f);
	if (pnum == 3 || pnum == 12)
		pmsg1 = oprintf("PLAYERS %d & %d!", pnum==3?1:3, pnum==3?2:4);
	else
		pmsg1 = oprintf("PLAYER %d!", pnum<4?pnum:(pnum==4?(!four_plr_ver?2:3):4));

	fx = (float)((int)(SPRITE_HRES - playresult.w) >> 1);
	fy = ypos + (float)(((int)(playresult.h + yinc) >> 1));
	if (is_low_res) fy += 6.0f;

	psprite = beginobj(&playresult, SPRITE_HRES/2.0f, SPRITE_VRES/2.0f+45.0f, 100.0f, SCR_PLATE_TID);
	psprite->w_scale = 2.22f;
	psprite->h_scale = 2.5f;
	psprite->id = 1;
	generate_sprite_verts(psprite);

	pflasher = qcreate("pflash2", 0, flash_alpha_text_proc, (int)pmsg0, (int)pmsg1, 0, 0);

	set_text_color(WHITE);

	ypos -= yinc * 2.0f;
//	if (is_low_res) ypos -= 3.0f;
	set_text_position(SPRITE_HRES/2.0f, ypos, 20.0f);
	oprintf("PRESS START NOW FOR");

	ypos -= yinc;
	if (is_low_res) ypos -= 8.0f;
	set_text_position(SPRITE_HRES/2.0f, ypos, 10.0f);
	oprintf("A FREE GAME!");

	j = pnum;
	i = 0;
	while (j)
	{
		if (j & 1)
		{
			// Show japple box
			qcreate("credbox", CREDIT_PID, plyr_credit_box,
				i,
				station_xys[i][four_plr_ver][X_VAL],
				station_xys[i][four_plr_ver][Y_VAL],
				JAPPLE_BOX_TID);

			print_players_name(i,
				japbx_cntr_xys[i][four_plr_ver][X_VAL],
				japbx_cntr_xys[i][four_plr_ver][Y_VAL],
				NULL,
				-1);
		}
		j >>= 1;
		i++;
	}

	draw_enable(1);

	// remove plates
//	turn_transit_off(0);

	// Let it be seen for up to 5 seconds
	timeout = 0;
	iqcreate("time", 0, free_game_timer, pnum, 0, 0, 0);

	while(!timeout)
	{
		sleep(1);
	}

	if (timeout == -1)
	{
		fullgame_bought_ok = 1;

		j = pnum;
		i = 0;
		while (j)
		{
			if (j & 1)
			{
				qcreate("fgbought",FULLGAME_BOUGHT_PID,fullgame_bought_msg,i,0,0,0);
				pdata_blocks[i].quarters_purchased = 4;
			}

			j >>= 1;
			i++;
		}

		sleep(1);
		while (existp(FULLGAME_BOUGHT_PID, 0xFFFFFFFF))
			sleep(1);
	}

	// Turn on the plates
	turn_transit_on();

	snd_stop_all();
	snd_set_reserved(SND_TRACK_0);

	// Delete the whole pile of crap
	killall(CREDIT_PID, -1);
//	kill(pflasher, 0);
	sleep(1);
	delete_multiple_strings(0x55aa, -1);

	// Delete the backgound
	del1c(1, -1);
	del1c(OID_JAPPLE_BOX, -1);
	del1c(OID_CNTDWN_DIGIT,-1);

	if(timeout > 0)
	{
		return(0);
	}
	return(1);
}
