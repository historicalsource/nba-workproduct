/*
 *		$Archive: /video/Nba/Include/select.h $
 *		$Revision: 31 $
 *		$Date: 4/20/99 12:14a $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifndef __SELECT_H__
#define __SELECT_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_select_h = "$Workfile: select.h $ $Revision: 31 $";
#endif

#define X_VAL				0
#define Y_VAL				1

#define OPTION_SCRN	 		0
#define NAME_ENTRY_SCRN	 	1
#define TM_SELECT_SCRN 	 	2
#define PLR_SELECT_SCRN  	3
#define VS_SCRN        	 	4
#define BUY_IN_SCRN    	 	5
#define HALFTIME_SCRN  	 	6

#define SCREENS         7

//
// general stuff
//
extern int four_plr_ver;

#define ALL_ABCD_MASK	((four_plr_ver)?(P1_ABCD_MASK|P2_ABCD_MASK|P3_ABCD_MASK|P4_ABCD_MASK):(P1_ABCD_MASK|P2_ABCD_MASK))
#define ALL_D_MASK		((four_plr_ver)?(P1_D|P2_D|P3_D|P4_D):(P1_D|P2_D))

#define	JOY_UP		1
#define	JOY_DOWN	2
#define	JOY_LEFT	4
#define	JOY_RIGHT	8

#define	JOY_DWN_LFT		6
#define	JOY_DWN_RGT		10
#define	JOY_UP_LFT		5
#define	JOY_UP_RGT		9

#define	A_BUTTON		1		// red
#define	B_BUTTON	    2		// blue
#define	C_BUTTON	    4		// white
#define	D_BUTTON	    8		// other button

#define	TURBO_BUTTON	C_BUTTON


//
// process id's
//
#define ZERO_PID     	 	0

#define	P1_PLYR_SEL_PID		800
#define	P2_PLYR_SEL_PID		801
#define	P3_PLYR_SEL_PID		802
#define	P4_PLYR_SEL_PID		803
#define HIDDEN_ATTRIB_PID	804

#define P1_CURSOR_PID    	200
#define P1_CHALNGR_PID   	201

#define P2_CURSOR_PID    	202
#define P2_CHALNGR_PID    	203

#define P3_CURSOR_PID    	204
#define P3_CHALNGR_PID    	205

#define P4_CURSOR_PID    	206
#define P4_CHALNGR_PID    	207

#define CREDIT_PID      	900

#define TM1_TM_SELECT_PID   400
#define TM1_CHALENGER_PID   401

#define TM2_TM_SELECT_PID   402
#define TM2_CHALENGER_PID   403

#define P1_PLAY_SEL_PID		500
#define P2_PLAY_SEL_PID		501

#define FLASH_PID			600
#define TIMER_PID			601
//#define ROTATE_LOGO_PID		602

//#define	NUM3D_PID			603

#define	PLAQ_AWARD_PID		604			// leave 4 after this for the players

//
// string ids
//
#define	ROSTER_STAT1_ID		751
#define	ROSTER_STAT2_ID		752
#define	ROSTER_STAT3_ID		753
#define	ROSTER_STAT4_ID		754
#define	ROSTER_STAT5_ID		755
#define	ROSTER_STAT6_ID		756

#define	ROSTER_NAME1_ID		757
#define	ROSTER_NAME2_ID		758
#define	ROSTER_NAME3_ID		759
#define	ROSTER_NAME4_ID		760
#define	ROSTER_NAME5_ID		761
#define	ROSTER_NAME6_ID		762

#define TIMER_MSG_ID	80
#define CREDIT_MSG_ID	81

#define P1_GEN_STR_ID    1001
#define P1_NAME_STR_ID   1101
#define P1_PIN_STR_ID    1201

#define P2_GEN_STR_ID    2001
#define P2_NAME_STR_ID   2101
#define P2_PIN_STR_ID    2201

#define P3_GEN_STR_ID    3101
#define P3_NAME_STR_ID   3201
#define P3_PIN_STR_ID    3301

#define P4_GEN_STR_ID    4201
#define P4_NAME_STR_ID   4301
#define P4_PIN_STR_ID    4401

#define	PLAY_SEL_MSG_ID		50
#define	PLAY_SELECT_ID		51
//#define	STAT_PG_TXT_ID		52
#define	STAT_PG_OBJ_ID		53				// leave 1 after this for (plyr + 1)

//
// General Player defines
//
#define PLYR_1          0
#define PLYR_2          1
#define PLYR_3          2
#define PLYR_4          3

#define PLYR_1_BIT      (1<<0)
#define PLYR_2_BIT      (1<<1)
#define PLYR_3_BIT      (1<<2)
#define PLYR_4_BIT      (1<<3)

//
//  structure def's
//
typedef struct tmu_ram_texture {
	char						*t_name;
	int							tex_id;
	} tmu_ram_texture_t;

//
// Global ref's
//
extern unsigned char rgold[];
extern unsigned char rgb[];
extern unsigned char bpr[];
extern unsigned char decay[];
extern unsigned char laser[];

//
// Routine typedef's
//
float print_players_name(int pnum, float x, float y, char *name, int showpts);
void challenger_needed_msg(int *);
void player_cursor(int *);
void timedown(int *);
void timedown_buyin(int *);			// 3d objects rather than fonts
void credit_msg(int *);
void plyr_credit_box(int *);
void name_entry(int);
int court_selection(void);
void team_selection(void);
void stats_page(void);
void clear_pups(void);
void vs_screen(void);
void stats_page(void);
void print_player_stats(int p_num, int cntr_x, int rnum);
void wait_for_button_press(int plyr_num, int min_ticks, int wait_ticks);
void pre_game_scrns(int *);
void player_select_roster_cntrl(int *args);
void team_select_cntrl(int *args);
void flash_yes_no(sprite_info_t *cur_obj, float flsh_x, float flsh_y, short answer);
//void trans_to_yn_plaq(sprite_info_t *p_plaq_obj, sprite_info_t *p_plaq_obj2, sprite_info_t *p_shdw_obj, sprite_info_t *p_shdw_obj2, int pnum, float y_dest);
//void trans_to_init_plaq(sprite_info_t *p_plaq_obj, sprite_info_t *p_plaq_obj2, sprite_info_t *p_shdw_obj, sprite_info_t *p_shdw_obj2, int pnum);
//void trans_to_pin_nbr_plaq(sprite_info_t *p_plaq_obj, sprite_info_t *p_plaq_obj2, sprite_info_t *p_shdw_obj, sprite_info_t *p_shdw_obj2, int pnum);
void fullgame_bought_msg(int *args);
//void update_cursor_imgs(sprite_info_t *cur_obj, short level, int c_pos, int tid);
void change_img_tmu(sprite_info_t *obj, image_info_t *ii, int tid);
void update_tm_data(short c_pos, short tm_nbr);
int load_textures_into_tmu(tmu_ram_texture_t *tex_tbl);
int	get_joy_val_down(int pnum);
int	get_joy_val_cur(int pnum);
int	get_but_val_down(int pnum);
int	get_but_val_cur(int pnum);
int	get_psw_val_down(int pnum);
int	get_psw_val_cur(int pnum);
void helmet_draw_function(void *);
void midway_logo_draw_function(void *oi);
void stat_flash(int *args);
void flash_cursor(sprite_info_t *, int);
void bckgrnd_scroll(int *args);
void helmet_proc(int *args);
void dim_text(int *args);
void rotate_logo(int *args);
void create_and_anim_midway_logo(int min_time, int max_time);
void flash_obj_proc(int *args);
void flash_obj_white(sprite_info_t *obj, int, int);
void draw_backdrop(void);
void game_over_scrn(void);
void coaching_tip_scrn(void);
int buyin_screen(void);
void halftime_substitutions(void);
void draw_backgrnd(float z, int tid);
void flash_credit_balls(int *args);
void flash_rank_plq(int *args);
void cycle_ostring_color_proc(int * args);
int get_alt_ladder_size(void);
#endif
