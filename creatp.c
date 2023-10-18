/*#define GRAB_SCREEN*/
/*#define UNLOCK_ALL_CHARACTERS*/
/*#define SHOW_ALL_HEADS*/
/*#define INFINITE_CLOCK*/
/*#define SHORT_CLOCK*/
/*#define ROTATE_PLAYER*/

/*
 *		$Archive: /video/Nba/creatp.c $
 *		$Revision: 165 $
 *		$Date: 9/29/99 5:34p $
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
char *ss_creatp_c = "$Workfile: creatp.c $ $Revision: 165 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <goose/goose.h>

/*
 *		USER INCLUDES
 */

#include "creatp.h"
#include "ani3d.h"
#include "audits.h"
#include "fontid.h"
#include "game.h"
#include "plyrseq.h"
#include "hdextrn.h"
#include "id.h"
#include "player.h"
#include "mainproc.h"
#include "coin.h"
#include "externs.h"
#include "transit.h"
#include "nbacmos.h"
#include "putil.h"
#include "sndcalls.h"

#include "optscrn.h"									/* flashes and sparkles image headers */
#include "include/mugshots.h"
#define	IMAGEDEFINE	1
#include "cplyr.h"										/* AI88 and ARGB4444 image headers */


/*
 *		DEFINES
 */

#define	P_ABC_MASK					(P_A|P_B|P_C)

#define CREATP_HEADS 				44

#define I2S_ORIG					0
#define I2S_DELTA					1
#define I2S_ABS						2

#define HOTSPOT_DATA_STAMP			0xA5
#define HOTSPOT_LEFT				(153 - 8)
#define HOTSPOT_RIGHT				(307 + 8)
#define HOTSPOT_TOP					(145 - 8)
#define HOTSPOT_BOTTOM				(312 + 8)

#define ARRAY_SIZE(a)				(sizeof(a) / sizeof(a[0]))
#define ARRAY_COUNT(a)				(ARRAY_SIZE(a) - 1)

#define OFF_YELLOW_COLOR			0xFFFFEA00			/* not quite a full yellow */
#define GRAY_COLOR					0xFF7F7F7F
#define LOW_RES_GRAY_COLOR			0xFFA6A6A6

#define SWITCH_DELAY				30
#define REPEAT_DELAY				6
#define SMALL_REPEAT_DELAY			4

#define SMALL_SPARKLE				0
#define MED_SPARKLE					1
#define LARGE_SPARKLE				2

#define MAX_SHADOW_LINE				15

/* temp here, mode into id.h */
#define CREATP_PID					0x0FA0
#define TIMER_PID					0x0FA1
#define SPARKLE_PID					0x0FA2
#define OVAL_SPARKLE_PID			0x0FA3
#define CLOCK_SPARKLE_PID			0x0FA4
#define TOP_SPARKLE_PID				0x0FA5
#define CREDIT_SPARKLE_PID			0x0FA6
#define PLAYER_DISPLAY_PID			0x0FA7
#define GLOW_PID					0x0FA8
#define CREDIT_PID					0x0FA9
#define SPRITE_FLASHER_PID			0x0FAA
#define	CREDIT_NOT_TAKEN1_PID		0x0FB0
#define	CREDIT_NOT_TAKEN2_PID		0x0FB1
#define	CREDIT_NOT_TAKEN3_PID		0x0FB2
#define	CREDIT_NOT_TAKEN4_PID		0x0FB3

#define CREATP_TID					10
#define CREATP_PLAYER_FLESH_TID		21011
#define CREATP_PLAYER_HEAD_TID		21012
#define CREATP_PLAYER_JERSEY_TID	21013
#define CREATP_PLAYER_JNUM_TID		21014
#define CREATP_GUEST_TID			21015
#define CREATP_CACHE_FLESH_TID		21016
#define CREATP_CACHE_HEAD_TID		21017

#define CREATP_SID					0x0100
#define TIMER_SID					0x0101
#define BUTTON_INSTRUCTION_SID		0x0102
#define PRIV_SID					0x0103
#define ATTR_SID					0x0104
#define CREDIT_SID					0x0105
#define STATS_SID					0x0106

#define CREATP_OID					0x0A00

#define BACKGROUND_BLUE_Z			300.0F
#define BACKGROUND_GRAY_Z			290.0F
#define BACKGROUND_SHADOW_Z			280.0F
#define BACKGROUND_TITLE_Z			270.0F
#define MODE_GFX_SHADOW_Z			260.0F				/* alpha shadow plate for mode graphics */
#define MODE_GFX_5_Z				250.0F				/* mode graphics layer one */
#define MODE_GFX_4_Z				240.0F				/* mode graphics layer two */
#define MODE_GFX_3_Z				230.0F				/* mode graphics layer three */
#define MODE_GFX_2_Z				220.0F				/* mode graphics layer four */
#define MODE_GFX_1_Z				210.0F				/* mode graphics layer five */
#define MODE_TITLE_SHADOW_Z			200.0F				/* shadow plate for mode title */
#define MODE_TITLE_Z				190.0F				/* mode title text */
#define BUTTON_TOP_Z				180.0F				/* top part of a button */
#define BUTTON_MIDDLE_Z				170.0F				/* middle background of a button */
#define BUTTON_TITLE_Z				160.0F				/* title of a button */
#define BUTTON_BOTTOM_Z				150.0F				/* bottom shadow of a button */
#define BUTTON_EXTRA_Z				140.0F				/* highest layer */
#define CREDIT_BAR_Z				130.0F
#define CREDIT_TEXT_Z				120.0F
#define GLOW_SPARKLE_Z				110.0F				/* effect sparkles */
#define SPARKLE_Z					100.0F				/* effect sparkles */
#define OVAL_SPARKLE_Z				90.0F				/* effect sparkles */
#define TOP_SPARKLE_Z				80.0F				/* effect sparkles */
#define INSTRUCTION_Z				30.f

#define CREDITS_X					218
#define CREDITS_Y					2
#define CREDITS_Z					MODE_TITLE_Z

#define TIMER_X						418
#define TIMER_Y						(SPRITE_VRES - 32)
#define TIMER_MIN_Z					MODE_TITLE_Z
#define TIMER_COL_Z					MODE_TITLE_Z - 1.0f
#define TIMER_SEC_Z					MODE_TITLE_Z - 2.0f
#ifdef SHORT_CLOCK
#define TIMER_INIT_VALUE			(tsec * 10)
#else
#define TIMER_INIT_VALUE			((2 * tsec * 60) + (tsec * 30))
#endif

#define NUM_VIEW_STAT_BAR			5

#define NUM_ATTRIBUTE_LEVEL			20

#define PRIV_STEALTH_TURBO			0x01
#define PRIV_BIG_HEAD				0x02
#define PRIV_DRONE_BIG_HEAD			0x04
#define PRIV_COURTSEL				0x08
#define PRIV_HOTSPOT				0x10
#define PRIV_HIDEATTR				0x20
#define PRIV_JERSEY					0x40
#define PRIV_RESERVED3				0x80

#define MINI_ATTR_NAME				0x0001
#define MINI_ATTR_HEIGHT			0x0002
#define MINI_ATTR_WEIGHT			0x0004
#define MINI_ATTR_POWER				0x0008
#define MINI_ATTR_SPEED				0x0010
#define MINI_ATTR_TWOP				0x0020
#define MINI_ATTR_THREEP			0x0040
#define MINI_ATTR_STEAL				0x0080
#define MINI_ATTR_BLOCK				0x0100
#define MINI_ATTR_DUNKS				0x0200
#define MINI_ATTR_DRIBBLE			0x0400
#define MINI_ATTR_ALL				0xFFFF

#define AUTO_SELECT_TIME_15			(15 * tsec)
#define AUTO_SELECT_TIME_6			(6 * tsec)

#define HEAD_MODE_Y_OFFSET			-24
#define UNIFORM_NUM_Y_OFFSET		EDITNAME_MODE_Y_OFFSET
#define PRIV_MODE_Y_OFFSET			-28
#define NAMELIST_MODE_Y_OFFSET		-12
#define EDITNAME_MODE_Y_OFFSET		-34
#define YES_NO_MODE_Y_OFFSET		-50

#define ATTR_BUTTON_DELTA			-23
#define PRIV_BUTTON_DELTA			-30
#define YES_NO_BUTTON_DELTA			-87

#define FADE_LIST_SIZE				85
#define FADE_UP						0
#define FADE_DOWN					1
#define FADE_UP_IMMED				2
#define FADE_DOWN_IMMED				3

extern int pup_team_jersey;
extern int tsec;
extern SNODE bbplyr_skel[];
extern int		p_status;
extern int		p_status2;
extern int		bought_in_first;
extern pdata_t pdata_blocks[];
extern ostring_t *ostring_list;
extern sprite_node_t *alpha_sprite_node_list;
extern plyr_record_t *rec_sort_ram;
extern char plyr_bnk_str[];
extern char cursor_bnk_str[];
extern char players_bnk_str[];
extern char crowd_bnk_str[];
extern char gameplay_bnk_str[];
extern char start_movie_snd_bnk_str[];
extern char qtr_trans_bnk_str[];
extern char anc_make_bnk[];
extern char anc_take_bnk[];
extern char anc_misc_bnk[];
extern float hotspot_xzs[NUM_HOTSPOTS + 4][2];
void start_enable(int);
void attract_loop(void);
void change_alpha_cc(void * parent, int cc);
void convert_2_jonhey(jonhey_t *jh_obj, LIMB *model, float *matrix, Texture_node_t **decals);
void update_other_cmos_to_disk(int sleep_allowed);

/*
 *		TYPEDEFS
 */

typedef int (*qsort_proc)(const void *a, const void *b);

typedef struct {
	int overall_rank;
	int game_win;
	int game_loss;
	int off_rank;
	int def_rank;
	int winstreak;
	int winstreak_rank;
	int pts_scored;
	int pts_allowed;
	int num_2pts_made;
	int num_2pts_attempted;
	int twop_rank;
	int num_3pts_made;
	int num_3pts_attempted;
	int threep_rank;
	int num_dunk_made;
	int num_dunk_attempted;
	int dunk_rank;
	int num_rebounds;
	int rebound_rank;
	int num_assists;
	int assist_rank;
	int num_steals;
	int steal_rank;
	int num_blocks;
	int block_rank;
	int num_turnovers;
	int turnover_rank;
	int trivia_pts;
	int trivia_rank;
	int unused_attribute_pts;
	int unused_privilege_pts;
	int teams_defeated;
	int defeated_count;
} stat_info;

typedef struct {
	char *texture_name;
	int texture_id;
} texture_page_t;

typedef struct {
	image_info_t *image;
	sprite_info_t *sprite;
} image_sprite;

typedef void *(*init_func)(void);
typedef int (*loop_func)(int input, void *data);
typedef void (*exit_func)(void *data);

enum {
	CREATP_VIEW_STAT_BUTTON,
	CREATP_HEAD_BUTTON,
	CREATP_NUMBER_BUTTON,
	CREATP_ATTRIBUTES_BUTTON,
	CREATP_PRIVILEGES_BUTTON,
	CREATP_NICKNAME_BUTTON,
	CREATP_EDIT_NAME_BUTTON,
	CREATP_EXIT_SAVE_BUTTON,
	CREATP_BUTTON_COUNT,

	UNIFORM_JERSEY_MODE = -1,
	EDIT_NAME_MODE = -2,
	ENTER_NAME_MODE = -3,
	ENTER_PIN_MODE = -4,
	HOTSPOT_MODE = -5,
	MODE_COUNT = 5
};

enum {
	GLOW_ATTR,
	GLOW_PRIV,
	GLOW_CURSOR,
	GLOW_YES_NO,
	GLOW_HEAD,
	GLOW_NAMELIST,
	GLOW_HOTSPOT
};

enum {
	CLOCK_NORMAL,
	CLOCK_FASTER,
	CLOCK_SLOWER
};

enum {
	RECORD_FOUND,
	RECORD_NOT_FOUND,
	RES_RECORD_FOUND
};

typedef struct {
	image_sprite unselected_middle;
	image_sprite selected_middle;
	image_sprite button_bottom;
	image_sprite title;
} creatp_button_t;

typedef struct {
	int current_button;
	int force_enter_name_pin;
} creatp_data_t;

typedef struct {
	image_sprite bar;
	image_sprite top1;
	image_sprite top5;
	image_sprite top10;
} view_stats_extra_data;

typedef struct {
	stat_info info;
	int page_number;
	int auto_select_timer;
	int last_index;
	int did_first_flash;
} view_stats_data_t;

typedef struct {
	int team;
	int player_index;
	int release_level;
	int availiable;
	image_sprite mugshot;
} head_info;

typedef struct {
	image_sprite button;
	image_sprite title;
	image_sprite left_arrow;
	image_sprite right_arrow;
} head_button_t;

typedef struct {
	int head_list_index;
	int arrow_timer;
	sprite_info_t *lit_arrow;
#ifdef SHOW_ALL_HEADS
	int head_roster_index;
	int head_count;	
	sprite_info_t *head_name;
#endif
} head_data_t;

enum {
	UNIFORM_NUMBER_BACK_SHADOW,
	UNIFORM_NUMBER_BACK_DROP,
	UNIFORM_NUMBER_CRESCENT,
	UNIFORM_NUMBER_CURSOR,
	UNIFORM_NUMBER_0,
	UNIFORM_NUMBER_1,
	UNIFORM_NUMBER_2,
	UNIFORM_NUMBER_3,
	UNIFORM_NUMBER_4,
	UNIFORM_NUMBER_5,
	UNIFORM_NUMBER_6,
	UNIFORM_NUMBER_7,
	UNIFORM_NUMBER_8,
	UNIFORM_NUMBER_9,
	UNIFORM_NUMBER_BACK,
	UNIFORM_NUMBER_END,
	UENTER_DASH3,
	UENTER_DASH4,
	UENTER_LETTER3,
	UENTER_LETTER4,
	UNIFORM_NUMBER_SPRITE_COUNT,
};

enum {
	ENTERING_NUMBER1,
	ENTERING_NUMBER2
};

typedef struct {
	int mode;
	int cursor_pos;
	char number[2];
} uniform_number_data_t;

enum {
	ATTRIBUTES_HEIGHT_BUTTON,
	ATTRIBUTES_WEIGHT_BUTTON,
	ATTRIBUTES_POWER_BUTTON,
	ATTRIBUTES_SPEED_BUTTON,
	ATTRIBUTES_TWOP_BUTTON,
	ATTRIBUTES_THREEP_BUTTON,
	ATTRIBUTES_STEAL_BUTTON,
	ATTRIBUTES_BLOCK_BUTTON,
	ATTRIBUTES_DUNKS_BUTTON,
	ATTRIBUTES_DRIBBLE_BUTTON,
	ATTRIBUTES_BUTTON_COUNT
};

typedef struct {
	image_sprite unselected_button;
	image_sprite selected_button;
	image_sprite back_frame;
	image_sprite tick_mark;
	image_sprite title;
} attributes_button_t;

typedef struct {
	int current_button;
	int button_state[ATTRIBUTES_BUTTON_COUNT];
} attributes_data_t;

enum {
	PRIVILEGES_STEALTH_BUTTON,
	PRIVILEGES_HEAD_BUTTON,
	PRIVILEGES_DHEAD_BUTTON,
	PRIVILEGES_COURTSEL_BUTTON,
	PRIVILEGES_HOTSPOT_BUTTON,
	PRIVILEGES_HIDEATTR_BUTTON,
	PRIVILEGES_JERSEY_BUTTON,
	PRIVILEGES_BUTTON_COUNT
};

typedef struct {
	image_sprite blue_box;
	image_sprite green_check_mark;
	image_sprite check_mark_shadow;
	image_sprite black_oval;
	image_sprite blue_oval;
	image_sprite title;
} privileges_button_t;

typedef struct {
	int current_button;
	int button_state[PRIVILEGES_BUTTON_COUNT];
} privileges_data_t;

typedef union {
	unsigned int data;
	struct {
		unsigned char stamp;
		unsigned char index;
		unsigned char x;
		unsigned char z;
	} packed_data;
} hotspot_data;

typedef struct {
	int x_pos;
	int z_pos;
} hotspot_data_t;

enum {
	TEAM_LIST,
	NAME_LIST
};

typedef struct {
	image_sprite up_arrow;
	image_sprite selected_up_arrow;
	image_sprite down_arrow;
	image_sprite selected_down_arrow;
	image_sprite frame_piece;
	image_sprite middle_piece;
	image_sprite disc_piece;
	image_sprite team_text;
	image_sprite name_text;
} namelist_button_t;

typedef struct {
	int current_list_index;
	int list_size;
	image_sprite *list_base;
	int arrow_timer;
	int selected_arrow;
	int last_sound_call;
	int sound_call_timer;
} namelist_data_t;

enum {
	NAME_BACK_SHADOW,
	NAME_BACK_DROP,
	NAME_CRESCENT,
	NAME_CURSOR,
	NAME_A,
	NAME_B,
	NAME_C,
	NAME_D,
	NAME_E,
	NAME_F,
	NAME_G,
	NAME_H,
	NAME_I,
	NAME_J,
	NAME_K,
	NAME_L,
	NAME_M,
	NAME_N,
	NAME_O,
	NAME_P,
	NAME_Q,
	NAME_R,
	NAME_S,
	NAME_T,
	NAME_U,
	NAME_V,
	NAME_W,
	NAME_X,
	NAME_Y,
	NAME_Z,
	NAME_EXCLM,
	NAME_BACK,
	NAME_SPACE,
	NAME_END,
	PIN_0,
	PIN_1,
	PIN_2,
	PIN_3,
	PIN_4,
	PIN_5,
	PIN_6,
	PIN_7,
	PIN_8,
	PIN_9,
	PIN_SPACE,
	PIN_BACK,
	PIN_ASTRX,
	ENTER_DASH1,
	ENTER_DASH2,
	ENTER_DASH3,
	ENTER_DASH4,
	ENTER_DASH5,
	ENTER_DASH6,
	ENTER_LETTER1,
	ENTER_LETTER2,
	ENTER_LETTER3,
	ENTER_LETTER4,
	ENTER_LETTER5,
	ENTER_LETTER6,
	EDIT_NAME_SPRITE_COUNT,
};

enum {
	ENTERING_NAME1,
	ENTERING_NAME2,
	ENTERING_NAME3,
	ENTERING_NAME4,
	ENTERING_NAME5,
	ENTERING_NAME6,
	ENTERING_PIN1,
	ENTERING_PIN2,
	ENTERING_PIN3,
	ENTERING_PIN4
};

typedef struct {
	int mode;
	int cursor_pos;
	char name[LETTERS_IN_NAME];
	char pin[PIN_NUMBERS];
} edit_name_data_t;

enum {
	YES_NO_YES_BUTTON,
	YES_NO_NO_BUTTON,
	YES_NO_BUTTON_COUNT
};

typedef struct {
	image_sprite unselected_button;
	image_sprite selected_button;
	image_sprite title;
} yes_no_button_t;

typedef struct {
	int current_button;
	int auto_select_timer;
} yes_no_data_t;

typedef struct {
	image_sprite list_title;					/* title for button, NAME or TEAM */
	image_sprite up_arrow;						/* selected up arrow */
	image_sprite down_arrow;					/* selected down arrow */
	image_sprite arrow_box;						/* blue arrow frame */
	image_sprite list_button;					/* list box button */
	image_sprite item_frame;					/* frame for selected list item */
} list_button_t;

/*
 *		STATIC PROTOTYPES
 */

static void cache_data(void);
static void load_creatp_sound_banks(void);
static void reload_sound_banks(void);
static void start_button_handler(int sig, int swid);
static void credit_not_taken_proc(int *args);
static void player_data_init(int new_record);
static unsigned int get_random_hotspot(void);

static void creatp_exit_page(void);
static void print_message(char *line1, char *line2, char *line3, int *y_pos, int gap, int fh);
static void *creatp_init(void);
static void creatp_exit(void *data);
static void creatp_exit_message(int which_message);
static void calc_creatp_audits(void);
static int creatp_loop(int input, void *data);
static void select_creatp_button(int button);
static void show_creatp_title(int button);
static void hide_creatp_title(void);
static void show_creatp_shadow(int y_pos, int title_bar, int num_lines, int bar_pos, int bottom_piece);
static void hide_creatp_shadow(void);
static void mini_attribute_init(void);
static void mini_attribute_exit(void);
static void mini_attribute_show_labels(void);
static void mini_attribute_reset_labels(void);
static void mini_attribute_update(int which, int selected_attr);
static void mini_attribute_draw_name_string(char *str, int num_char, int start_x, int start_y, int sprite_index);
static void mini_attribute_draw_num_string(char *str, int num_char, int start_x, int start_y, int sprite_index, image_info_t **num_list, int weight_str, long color);
static int mini_attribute_x_delta(int value);
static void mini_attribute_set_sprite(sprite_info_t *sprite, image_info_t *image, int x_pos, int y_pos, long color);
static long mini_attribute_color(char attr);

static void *view_stat_init(void);
static void view_stat_exit(void *data);
static int view_stat_loop(int input, void *data);
static void print_stat(char *line1, char *line2, int *y_pos, int gap, int fh, int index);
static void flash_stat(int *y_pos, int gap, int fh, int index, int rank);
static void page1(view_stats_data_t *data, int y, int fh, int gap, int total_games);
static void page2(view_stats_data_t *data, int y, int fh, int gap, int total_games);
static void page3(view_stats_data_t *data, int y, int fh, int gap, int total_games);
static void page4(view_stats_data_t *data, int y, int fh, int gap);
static int show_view_stat(view_stats_data_t *data);
static void hide_view_stat(void);
static void get_stat_info(plyr_record_t *rec, stat_info *info);
static int cmp_winstreak(int a, int b);
static int sort_2pt_percent(plyr_record_t *rec1, plyr_record_t *rec2);
static int sort_rebounds(plyr_record_t *rec1, plyr_record_t *rec2);
static int sort_assists(plyr_record_t *rec1, plyr_record_t *rec2);
static int sort_steals(plyr_record_t *rec1, plyr_record_t *rec2);
static int sort_blocks(plyr_record_t *rec1, plyr_record_t *rec2);
static int sort_turnovers(plyr_record_t *rec1, plyr_record_t *rec2);

static void *head_init(void);
static void head_exit(void *data);
static int head_loop(int input, void *data);
static int player_record_info_to_head_list_index(plyr_record_t *player_info);
static void head_list_index_to_info(int index, plyr_record_t *player_record);
static void head_list_left_right(int center, int *left, int *right);
static void init_head_list(void);
static void unlock_head_list();
static void select_head_button(int center);

static void *uniform_number_init(void);
static void uniform_number_exit(void *data);
static int uniform_number_loop(int input, void *data);
static void move_uniform_number_cursor(int input, uniform_number_data_t *uniform_number_data);
static void set_uniform_number_cursor(int cursor_pos);
static void update_uniform_number_dash_letter(uniform_number_data_t *uniform_number_data);

static void *privileges_init(void);
static void privileges_additional_init(privileges_data_t *data);
static void privileges_exit(void *data);
static void privileges_additional_exit(void);
static int privileges_loop(int input, void *data);
static void select_privileges_button(int button);
static void color_privileges_title(privileges_data_t *privileges_data);
static void update_privileges_check(int button, int state);
static void privileges_hide(void);
static void privileges_unhide(privileges_data_t *data);

static void *hotspot_init(void);
static void hotspot_exit(void *data);
static int hotspot_loop(int input, void *data);
static void hotspot_set_cursor(hotspot_data_t *data);

static void *attributes_init(void);
static void attributes_exit(void *data);
static int attributes_loop(int input, void *data);
static void select_attributes_button(int button);
static void update_attributes_value(int button, int value);
static void attributes_update_point_message(attributes_data_t *attributes_data, long start_color);

static void *namelist_init(void);
static void namelist_exit(void *data);
static int namelist_loop(int input, void *data);
static void update_namelist(namelist_data_t *data);
static int item_to_index(namelist_data_t *data, int item);

static void *edit_name_init(void);
static void edit_name_exit(void *data);
static int edit_name_loop(int input, void *data);
static void edit_name_wrapper(int startup_name);
static void load_guest_mug(plyr_record_t *player_info);
static void edit_name_record_message(int which_message);
static void fade_name_to_pin(edit_name_data_t *edit_name_data);
static void move_edit_name_cursor(int input, edit_name_data_t *edit_name_data);
static void set_edit_name_cursor(int cursor_pos);
static void update_dash_letter(edit_name_data_t *edit_name_data);

static void *yes_no_init(void);
static void yes_no_exit(void *data);
static int yes_no_loop(int input, void *data);
static void select_yes_no_button(int button);

static void proc_timer(int *args);
static void proc_credit_msg(int *args);
static void proc_sparkle(int *args);
static void proc_oval_sparkle(int *args);
static void proc_top_sparkle(int *args);
static void proc_glow_sparkle(int *args);
static void do_top_sparkle(sprite_info_t *sprite, int w_scale, int h_scale, int short_sparkle, int credit_bar);
static void do_glow_sparkle(sprite_info_t *sprite);
static void proc_flash_text(int *args);
static void mode_loop(init_func initf, loop_func loopf, exit_func exitf);
static void image_to_sprite(image_sprite *img, float z_level, char x_mode, char y_mode, int x_pos, int y_pos, int hide);
static void free_image_to_sprite(image_sprite *img);
static int load_texture_list(texture_page_t *texture_page_list);
static void find_sprite_center(sprite_info_t *sprite, int *x_pos, int *y_pos);
static void get_sprite_pos(sprite_info_t *sprite, int *x_pos, int *y_pos);
static void set_sprite_pos(sprite_info_t *sprite, int x, int y);
static void set_sprite_xyz(sprite_info_t *sprite, int x, int y, int z);
static void set_sprite_color(sprite_info_t *sprite, long color);
static void flash_sprite_proc(int *args);
static void flash_sprite(sprite_info_t *obj, int color, int cnt, int delay);
static void reset_fade_list(void);
static void fade_sprite_list(int fade_dir, int slow_fade);
static void show_button_instruction(int button_number);
static void show_mode_instruction(int button_number);
static void hide_instruction(void);
static int next_button(int first_button, int last_button, int *current_button, int input, int wrap);
static void wait_for_button(int min_time, int max_time);
static char *rank_suffix(int num);

static void player_display_init(void);
static void player_display_exit(void);
#if defined(VEGAS)
static void load_a_player_texture(char *tex_name, int tex_id, Texture_node_t **tex_data, int wrap);
#else
static void load_a_player_texture(char *tex_name, int tex_id, GrMipMapId_t *tex_data, int wrap);
#endif
static void load_player_textures(void);
static void init_player_pos(obj_3d *obj);
static void proc_player_display(int *args);
static void update_player_display(void);
static void player_pre_draw(void);
static void adjust_player(obj_3d *obj);
static void draw_player(obj_3d *obj);

/*
 *		STATIC VARIABLES
 */

static texture_page_t creatp_texture_list[] = {
	/* ARGB4444 */
	{"cp444400.wms", CREATP_TID},
	{"cp444401.wms", CREATP_TID},
	{"cp444402.wms", CREATP_TID},
	{"cp444403.wms", CREATP_TID},
	{"cp444404.wms", CREATP_TID},
	{"cp444405.wms", CREATP_TID},
	{"cp444406.wms", CREATP_TID},
	{"cp444407.wms", CREATP_TID},
	{"cp444408.wms", CREATP_TID},
	{"cp444409.wms", CREATP_TID},
	{"cp444410.wms", CREATP_TID},
	{"cp444411.wms", CREATP_TID},
	{"cp444412.wms", CREATP_TID},
	{"cp444413.wms", CREATP_TID},
	{"cp444414.wms", CREATP_TID},
	{"cp444415.wms", CREATP_TID},
	{"cp444416.wms", CREATP_TID},
	{"cp444417.wms", CREATP_TID},
	{"cp444418.wms", CREATP_TID},
	{"cp444419.wms", CREATP_TID},
	{"cp444420.wms", CREATP_TID},
	{"cp444421.wms", CREATP_TID},
	{"cp444422.wms", CREATP_TID},
	{"cp444423.wms", CREATP_TID},
	{"cp444424.wms", CREATP_TID},
	{"cp444425.wms", CREATP_TID},
	{"cp444426.wms", CREATP_TID},
	{"cp444427.wms", CREATP_TID},
	{"cp444428.wms", CREATP_TID},
	{"cp444429.wms", CREATP_TID},
	{"cp444430.wms", CREATP_TID},
	{"cpxtra.wms", CREATP_TID},
	
	{"plqfsh00.wms", CREATP_TID},
	{"plqfsh01.wms", CREATP_TID},
	{"plqfsh02.wms", CREATP_TID},
	
	{"plqtop00.wms", CREATP_TID},
	{"plqtop01.wms", CREATP_TID},
	
	{"TRANS00.WMS", TRANS_TID},
	{"TRANS01.WMS", TRANS_TID},
	{"TRANS02.WMS", TRANS_TID},
	{"TRANS03.WMS", TRANS_TID},
	{"TRANS04.WMS", TRANS_TID},
	
	/* AI88 */
	{"cpai88.wms", CREATP_TID},
	{NULL, 0}
};

static creatp_button_t creatp_button_list[CREATP_BUTTON_COUNT] = {
	{{&reglbar2, NULL},
	 {&selctbar, NULL},
	 {&reglbarf, NULL},
	 {&viewsts1, NULL}},
	{{&reglbar2, NULL},
	 {&selctbar, NULL},
	 {&reglbarf, NULL},
	 {&head1, NULL}},
	{{&reglbar2, NULL},
	 {&selctbar, NULL},
	 {&reglbarf, NULL},
	 {&number1, NULL}},
	{{&reglbar2, NULL},
	 {&selctbar, NULL},
	 {&reglbarf, NULL},
	 {&attribt1, NULL}},
	{{&reglbar2, NULL},
	 {&selctbar, NULL},
	 {&reglbarf, NULL},
	 {&privilg1, NULL}},
	{{&reglbar2, NULL},
	 {&selctbar, NULL},
	 {&reglbarf, NULL},
	 {&nicknam1, NULL}},
	{{&reglbar2, NULL},
	 {&selctbar, NULL},
	 {&reglbarf, NULL},
	 {&editnam1, NULL}},
	{{&reglbar2, NULL},
	 {&selctbar, NULL},
	 {&reglbarf, NULL},
	 {&exitsav1, NULL}}
};
static image_sprite creatp_title_list[CREATP_BUTTON_COUNT + MODE_COUNT] = {
	{&viewstsm, NULL},
	{&headm, NULL},
	{&numberm, NULL},
	{&attribtm, NULL},
	{&privilgm, NULL},
	{&nicknamm, NULL},
	{&editnamm, NULL},
	{&exitsavm, NULL},
	{&jerseym, NULL},		/* UNIFORM_JERSEY_MODE */
	{&editnamm, NULL},		/* EDIT_NAME_MODE */
	{&entrnamm, NULL},		/* ENTER_NAME_MODE */
	{&entrpinm, NULL},		/* ENTER_PIN_MODE */
	{&hotspotm, NULL},		/* HOTSPOT_MODE */
};
static image_sprite creatp_shadow_list[MAX_SHADOW_LINE + 4] = {
	{&shadotop, NULL},
	{&shadtop2, NULL},
	{&shadomid, NULL},
	{&shadomid, NULL},
	{&shadomid, NULL},
	{&shadomid, NULL},
	{&shadomid, NULL},
	{&shadomid, NULL},
	{&shadomid, NULL},
	{&shadomid, NULL},
	{&shadomid, NULL},
	{&shadomid, NULL},
	{&shadomid, NULL},
	{&shadomid, NULL},
	{&shadomid, NULL},
	{&shadomid, NULL},
	{&shadomid, NULL},
	{&shadobtm, NULL},
	{&shmidbar, NULL}
};
static image_sprite creatp_background_list1[] = {
	{&backgdnw, NULL},
	{&backgdnw_c1, NULL},
	{&backgdnw_c2, NULL},
	{&backgdnw_c3, NULL}
};
static image_sprite creatp_background_list2[] = {
	{&clock, NULL}
};
static image_sprite creatp_background_list3[] = {
	{&midbar, NULL},
	{&midbar_c1, NULL},
	{&rtnambar, NULL}
};
static image_sprite creatp_background_list4[] = {
	{&rtnambrs, NULL}
};
static image_sprite creatp_background_list5[] = {
	{&mdcpwrd2, NULL}
};
static image_sprite creatp_instruction_list[] = {
	{&midinfbx, NULL},
	{&midinfbt, NULL},
	{&midinfb2, NULL}
};

static creatp_button_t *creatp_selected_button;

static view_stats_extra_data view_stats_bar[NUM_VIEW_STAT_BAR] = {
	{{&statbar, NULL}, {&sb_first, NULL}, {&sb_top5, NULL}, {&sb_top10, NULL}},
	{{&statbar, NULL}, {&sb_first, NULL}, {&sb_top5, NULL}, {&sb_top10, NULL}},
	{{&statbar, NULL}, {&sb_first, NULL}, {&sb_top5, NULL}, {&sb_top10, NULL}},
	{{&statbar, NULL}, {&sb_first, NULL}, {&sb_top5, NULL}, {&sb_top10, NULL}},
	{{&statbar, NULL}, {&sb_first, NULL}, {&sb_top5, NULL}, {&sb_top10, NULL}}
};

enum {
	RLEVEL_1,
	RLEVEL_2,
	RLEVEL_3,
	RLEVEL_4,
	RLEVEL_5,
	RLEVEL_6,
	RLEVEL_7,
	RLEVEL_8,
	RLEVEL_9,
	RLEVEL_10,
	RLEVEL_11,
	RLEVEL_12,
	RLEVEL_13,
	RLEVEL_14,
	RLEVEL_15,
	RLEVEL_16,
	RLEVEL_17,
	RLEVEL_18,
	RLEVEL_19,
	RLEVEL_20
};

static head_info creatp_head_list[CREATP_HEADS] = {
	{-1, -1, RLEVEL_1, TRUE, {&cp_quest, NULL}},							/* Slot for guest head */
	{-1, -1, RLEVEL_1, TRUE, {&cp_spipp, NULL}},							/* Scottie Pippen */
	{-1, -1, RLEVEL_1, TRUE, {&cp_sonea, NULL}},							/* Saquel O Neal */
	{-1, -1, RLEVEL_1, TRUE, {&cp_thard, NULL}},							/* Tim Hardaway */
	{-1, -1, RLEVEL_1, TRUE, {&cp_kbrya, NULL}},							/* Kobie Bryant */
	{-1, -1, RLEVEL_1, TRUE, {&cp_ghill, NULL}},							/* Grant Hill */
	{-1, -1, RLEVEL_1, TRUE, {&cp_rmill, NULL}},							/* Reggie Miller */
	{-1, -1, RLEVEL_1, TRUE, {&cp_kmalo, NULL}},							/* Karl Malone */
	{-1, -1, RLEVEL_1, TRUE, {&cp_grice, NULL}},							/* Glen Rice */
	{-1, -1, RLEVEL_1, TRUE, {&cp_gpayt, NULL}},							/* Gary Payton */
	{-1, -1, RLEVEL_1, TRUE, {&cp_cmull, NULL}},							/* Chris Mullin */
	
	{TEAM_SECRET, ATL_MASCOT, RLEVEL_1, TRUE, {&cp_ahawk, NULL}},			/* ATL_MASCOT */
	{TEAM_SECRET, CHA_MASCOT, RLEVEL_1, TRUE, {&cp_chugo, NULL}},			/* CHA_MASCOT */
	{TEAM_SECRET, CHI_MASCOT, RLEVEL_1, TRUE, {&cp_benny, NULL}},			/* CHI_MASCOT */
	{TEAM_SECRET, DEN_MASCOT, RLEVEL_1, TRUE, {&cp_rocky, NULL}},			/* DEN_MASCOT */
	{TEAM_SECRET, HOU_MASCOT, RLEVEL_1, TRUE, {&cp_hbear, NULL}},			/* HOU_MASCOT */
	{TEAM_SECRET, IND_MASCOT, RLEVEL_1, TRUE, {&cp_boomr, NULL}},			/* IND_MASCOT */
	{TEAM_SECRET, MIN_MASCOT, RLEVEL_1, TRUE, {&cp_crunc, NULL}},			/* MIN_MASCOT */
	{TEAM_SECRET, NJN_MASCOT, RLEVEL_1, TRUE, {&cp_njsly, NULL}},			/* NJN_MASCOT */
	{TEAM_SECRET, PHO_MASCOT, RLEVEL_1, TRUE, {&cp_goril, NULL}},			/* PHO_MASCOT */
	{TEAM_SECRET, SEA_MASCOT, RLEVEL_1, TRUE, {&cp_sasqu, NULL}},			/* SEA_MASCOT */
	{TEAM_SECRET, TOR_MASCOT, RLEVEL_1, TRUE, {&cp_rapto, NULL}},			/* TOR_MASCOT */
	{TEAM_SECRET, UTA_MASCOT, RLEVEL_1, TRUE, {&cp_ubear, NULL}},			/* UTA_MASCOT */
	
	{TEAM_SECRET, LIA_CHARACTER, RLEVEL_1, TRUE, {&cp_lmont, NULL}},		/* LIA_CHARACTER */
	{TEAM_SECRET, KERRI_CHARACTER, RLEVEL_1, TRUE, {&cp_khosk, NULL}},		/* KERRI_CHARACTER */
	
	{TEAM_SECRET, TIM_K_CHARACTER, RLEVEL_1, TRUE, {&cp_timk, NULL}},		/* TIM_K_CHARACTER */
	{TEAM_SECRET, CHAD_CHARACTER, RLEVEL_1, TRUE, {&cp_chad, NULL}},		/* CHAD_CHARACTER */
	{TEAM_SECRET, RETRO_CHARACTER, RLEVEL_1, TRUE, {&cp_retro, NULL}},		/* RETRO_CHARACTER */
	{TEAM_SECRET, PINTO_CHARACTER, RLEVEL_1, TRUE, {&cp_pinto, NULL}},		/* PINTO_CHARACTER */
	
	{TEAM_SECRET, OLDMAN_CHARACTER, RLEVEL_2, TRUE, {&cp_grand, NULL}},		/* OLDMAN_CHARACTER */
	{TEAM_SECRET, CLOWN_CHARACTER, RLEVEL_3, TRUE, {&cp_clown, NULL}},		/* CLOWN_CHARACTER */
	{TEAM_SECRET, ALIEN_CHARACTER, RLEVEL_4, TRUE, {&cp_alien, NULL}},		/* ALIEN_CHARACTER */
	{TEAM_SECRET, WIZARD_CHARACTER, RLEVEL_5, TRUE, {&cp_wizar, NULL}},		/* WIZARD_CHARACTER */
	{TEAM_SECRET, JAM_CHARACTER, RLEVEL_6, TRUE, {&cp_jalie, NULL}},		/* JAM_CHARACTER */
	{TEAM_SECRET, PUMPKIN_CHARACTER, RLEVEL_7, TRUE, {&cp_jacko, NULL}},	/* PUMPKIN_CHARACTER */
	
	{TEAM_SECRET, FRANK_CHARACTER, RLEVEL_8, TRUE, {&cp_frank, NULL}},		/* FRANK_CHARACTER */
	{TEAM_SECRET, BRIDE_CHARACTER, RLEVEL_9, TRUE, {&cp_bride, NULL}},		/* BRIDE_CHARACTER */
	{TEAM_SECRET, MUMMY_CHARACTER, RLEVEL_10, TRUE, {&cp_mummy, NULL}},		/* MUMMY_CHARACTER */
	{TEAM_SECRET, WOLFMAN_CHARACTER, RLEVEL_11, TRUE, {&cp_wolfm, NULL}},	/* WOLFMAN_CHARACTER */
	{TEAM_SECRET, CREATURE_CHARACTER, RLEVEL_12, TRUE, {&cp_creat, NULL}},	/* CREATURE_CHARACTER */
	{TEAM_SECRET, REF_CHARACTER, RLEVEL_13, TRUE, {&cp_ref, NULL}},			/* REF_CHARACTER */
	{TEAM_SECRET, TIM_M_CHARACTER, RLEVEL_14, TRUE, {&cp_timm, NULL}},		/* TIM_M_CHARACTER */
	
	{TEAM_SECRET, JON_H_CHARACTER, RLEVEL_15, TRUE, {&cp_jhey, NULL}},		/* JON_H_CHARACTER */
	{TEAM_SECRET, WILLY_M_CHARACTER, RLEVEL_1, TRUE, {&cp_will, NULL}},		/* WILLY_M_CHARACTER */
};
static image_sprite horse_mugshot = {&cp_horse, NULL};
static sprite_info_t *pinto_sprite = NULL;

static void *nba_player_list[10] = {
	&spipp,
	&sonea,
	&thard,
	&kbrya,
	&ghill,
	&rmill,
	&kmalo,
	&grice,
	&gpayt,
	&cmull
};

static head_button_t head_button_list = {
	{&hedsplq2, NULL},
	{&hedswrd2, NULL},
	{&harrwlf2, NULL},
	{&harrwrt2, NULL}
};
static image_sprite head_additional[2] = {
	{&hedsfrm2, NULL},
	{&hedsfrm3, NULL}
};

static image_sprite uniform_number_sprite_list[UNIFORM_NUMBER_SPRITE_COUNT] = {
	{&pinbkshd, NULL},
	{&pinbkdrp2, NULL},
	{&crescent, NULL},
	{&editdisc, NULL},
	
	{&wrd0, NULL},
	{&wrd1, NULL},
	{&wrd2, NULL},
	{&wrd3, NULL},
	{&wrd4, NULL},
	{&wrd5, NULL},
	{&wrd6, NULL},
	{&wrd7, NULL},
	{&wrd8, NULL},
	{&wrd9, NULL},
	{&backwrd, NULL},
	{&endwrd, NULL},
	
	{&tikwrd, NULL},
	{&tikwrd, NULL},
	
	{&tikwrd, NULL},
	{&tikwrd, NULL}
};

static privileges_button_t privileges_button_list[PRIVILEGES_BUTTON_COUNT] = {
	{{&pbluebox, NULL},
	 {&checkmrk, NULL},
	 {&chkmrksh, NULL},
	 {&privibar, NULL},
	 {&privibrs, NULL},
	 {&stlthtrb, NULL}},
	{{&pbluebox, NULL},
	 {&checkmrk, NULL},
	 {&chkmrksh, NULL},
	 {&privibar, NULL},
	 {&privibrs, NULL},
	 {&bighead, NULL}},
	{{&pbluebox, NULL},
	 {&checkmrk, NULL},
	 {&chkmrksh, NULL},
	 {&privibar, NULL},
	 {&privibrs, NULL},
	 {&drobghed, NULL}},
	{{&pbluebox, NULL},
	 {&checkmrk, NULL},
	 {&chkmrksh, NULL},
	 {&privibar, NULL},
	 {&privibrs, NULL},
	 {&crtselct, NULL}},
	{{&pbluebox, NULL},
	 {&checkmrk, NULL},
	 {&chkmrksh, NULL},
	 {&privibar, NULL},
	 {&privibrs, NULL},
	 {&hotspwrd, NULL}},
	{{&pbluebox, NULL},
	 {&checkmrk, NULL},
	 {&chkmrksh, NULL},
	 {&privibar, NULL},
	 {&privibrs, NULL},
	 {&hideattr, NULL}},
	{{&pbluebox, NULL},
	 {&checkmrk, NULL},
	 {&chkmrksh, NULL},
	 {&privibar, NULL},
	 {&privibrs, NULL},
	 {&chooseje, NULL}}
};

static image_sprite hotspot_list[2] = {
	{&hotspgrf, NULL},
	{&hdiscred, NULL}
};

static attributes_button_t attributes_button_list[ATTRIBUTES_BUTTON_COUNT] = {
	{{&atrbdscr, NULL},
	 {&atrbdscs, NULL},
	 {&atrbarsh, NULL},
	 {&atrbtk01, NULL},
	 {&height1, NULL}},
	{{&atrbdscr, NULL},
	 {&atrbdscs, NULL},
	 {&atrbarsh, NULL},
	 {&atrbtk01, NULL},
	 {&weight1, NULL}},
	{{&atrbdscr, NULL},
	 {&atrbdscs, NULL},
	 {&atrbarsh, NULL},
	 {&atrbtk01, NULL},
	 {&power1, NULL}},
	{{&atrbdscr, NULL},
	 {&atrbdscs, NULL},
	 {&atrbarsh, NULL},
	 {&atrbtk01, NULL},
	 {&speed1, NULL}},
	{{&atrbdscr, NULL},
	 {&atrbdscs, NULL},
	 {&atrbarsh, NULL},
	 {&atrbtk01, NULL},
	 {&twop1, NULL}},
	{{&atrbdscr, NULL},
	 {&atrbdscs, NULL},
	 {&atrbarsh, NULL},
	 {&atrbtk01, NULL},
	 {&threep1, NULL}},
	{{&atrbdscr, NULL},
	 {&atrbdscs, NULL},
	 {&atrbarsh, NULL},
	 {&atrbtk01, NULL},
	 {&steal1, NULL}},
	{{&atrbdscr, NULL},
	 {&atrbdscs, NULL},
	 {&atrbarsh, NULL},
	 {&atrbtk01, NULL},
	 {&block1, NULL}},
	{{&atrbdscr, NULL},
	 {&atrbdscs, NULL},
	 {&atrbarsh, NULL},
	 {&atrbtk01, NULL},
	 {&dunks1, NULL}},
	{{&atrbdscr, NULL},
	 {&atrbdscs, NULL},
	 {&atrbarsh, NULL},
	 {&atrbtk01, NULL},
	 {&dribble1, NULL}}
};

static image_info_t *attributes_tick_list[NUM_ATTRIBUTE_LEVEL] = {
	&atrbtk01,
	&atrbtk02,
	&atrbtk03,
	&atrbtk04,
	&atrbtk05,
	&atrbtk06,
	&atrbtk07,
	&atrbtk08,
	&atrbtk09,
	&atrbtk10,
	&atrbtk11,
	&atrbtk12,
	&atrbtk13,
	&atrbtk14,
	&atrbtk15,
	&atrbtk16,
	&atrbtk17,
	&atrbtk18,
	&atrbtk19,
	&atrbtk20
};

static image_sprite mini_attribute_list_mid[] = {
	{&lpower, NULL},
	{&lspeed, NULL},
	{&ltwop, NULL},
	{&lthreep, NULL},
	{&lsteal, NULL},
	{&lblock, NULL},
	{&ldunks, NULL},
	{&ldribble, NULL},
	/* 8 sprites for player stats */
	{&lattr_1, NULL},
	{&lattr_1, NULL},
	{&lattr_1, NULL},
	{&lattr_1, NULL},
	{&lattr_1, NULL},
	{&lattr_1, NULL},
	{&lattr_1, NULL},
	{&lattr_1, NULL},
	/* 6 character for player name */
	{&awrd12, NULL},
	{&awrd12, NULL},
	{&awrd12, NULL},
	{&awrd12, NULL},
	{&awrd12, NULL},
	{&awrd12, NULL},
	/* 4 character for player height */
	{&awrd12, NULL},
	{&awrd12, NULL},
	{&awrd12, NULL},
	{&awrd12, NULL},
	/* 4 character for player weight */
	{&awrd12, NULL},
	{&awrd12, NULL},
	{&awrd12, NULL},
	{&lpounds, NULL}
};
static image_sprite mini_attribute_list_low[] = {
	{&lrpower, NULL},
	{&lrspeed, NULL},
	{&lrtwop, NULL},
	{&lrthreep, NULL},
	{&lrsteal, NULL},
	{&lrblock, NULL},
	{&lrdunks, NULL},
	{&lrdribble, NULL},
	/* 8 sprites for player stats */
	{&lrattr_1, NULL},
	{&lrattr_1, NULL},
	{&lrattr_1, NULL},
	{&lrattr_1, NULL},
	{&lrattr_1, NULL},
	{&lrattr_1, NULL},
	{&lrattr_1, NULL},
	{&lrattr_1, NULL},
	/* 6 character for player name */
	{&awrd12, NULL},
	{&awrd12, NULL},
	{&awrd12, NULL},
	{&awrd12, NULL},
	{&awrd12, NULL},
	{&awrd12, NULL},
	/* 4 character for player height */
	{&awrd12, NULL},
	{&awrd12, NULL},
	{&awrd12, NULL},
	{&awrd12, NULL},
	/* 4 character for player weight */
	{&awrd12, NULL},
	{&awrd12, NULL},
	{&awrd12, NULL},
	{&lrpounds, NULL}
};
static image_sprite *mini_attribute_list;

static namelist_button_t namelist_sprite_list = {
	{&arrwupnwb, NULL},
	{&arrwupnwg, NULL},
	{&arrwdwnnwb, NULL},
	{&arrwdwnnwg, NULL},
	{&nknmgrf2, NULL},
	{&nknmbar, NULL},
	{&nknmdsc, NULL},
	{&teamwrdnw, NULL},
	{&namwrdnw, NULL}
};
static int namelist_mode;

static image_sprite team_list_sprites[] = {
	{&atlanta, NULL},
	{&boston, NULL},
	{&charlott, NULL},
	{&chicago, NULL},
	{&clevelan, NULL},
	{&dallas, NULL},
	{&denver, NULL},
	{&detroit, NULL},
	{&goldenst, NULL},
	{&houston, NULL},
	{&indiana, NULL},
	{&laclippr, NULL},
	{&lalakers, NULL},
	{&miami, NULL},
	{&milwauke, NULL},
	{&minnesot, NULL},
	{&newjerse, NULL},
	{&newyork, NULL},
	{&orlando, NULL},
	{&philadel, NULL},
	{&phoenixt, NULL},
	{&portland, NULL},
	{&sacramen, NULL},
	{&sananton, NULL},
	{&seattlet, NULL},
	{&toronto, NULL},
	{&utah, NULL},
	{&vancouvr, NULL},
	{&washingt, NULL},
	{&custom, NULL},
	{&midwayt, NULL}
};

static image_sprite name_list[] = {
	{&defaultt, NULL},
	{&thealien, NULL},
	{&badger, NULL},
	{&bear, NULL},
	{&benny_nm, NULL},
	{&boomer, NULL},
	{&thebride, NULL},
	{&thechief, NULL},
	{&coyote, NULL},
	{&creed, NULL},
	{&crispy, NULL},
	{&crunch, NULL},
	{&cyan, NULL},
	{&dart, NULL},
	{&dice, NULL},
	{&dragon, NULL},
	{&falcon, NULL},
	{&frankens, NULL},
	{&frost, NULL},
	{&thegoril, NULL},
	{&grandpa, NULL},
	{&hawk, NULL},
	{&horse_nm, NULL},
	{&hugo, NULL},
	{&iceman, NULL},
	{&jackonm, NULL},
	{&kerri, NULL},
	{&legend, NULL},
	{&lia, NULL},
	{&themummy, NULL},
	{&nikkonm, NULL},
	{&oldman, NULL},
	{&phat, NULL},
	{&pinkertn, NULL},
	{&pinto, NULL},
	{&therapto, NULL},
	{&theref, NULL},
	{&retrorob, NULL},
	{&therock, NULL},
	{&rocky_nm, NULL},
	{&sasquatc, NULL},
	{&shaman, NULL},
	{&shoop, NULL},
	{&sly, NULL},
	{&spacey, NULL},
	{&stallion, NULL},
	{&toolman, NULL},
	{&turbo, NULL},
	{&twilight, NULL},
	{&tzone, NULL},
	{&valkyrie, NULL},
	{&thewizz, NULL},
	{&wizard, NULL},
	{&wolfman, NULL},
	{&zephyr, NULL}
};
static char *name_list_bank[] = {
	NULL,				/* default */
	"alian",			/* thealien */
	"badge",			/* badger */
	"ubear",			/* bear */
	"benny",			/* benny_nm */
	"boome",			/* boomer */
	"bride",			/* thebride */
	"chief",			/* thechief */
	"coyot",			/* coyote */
	"creed",			/* creed */
	"clown",			/* crispy */
	"crunc",			/* crunch */
	"cyan",				/* cyan */
	"dart",				/* dart */
	"dice",				/* dice */
	"drago",			/* dragon */
	"falco",			/* falcon */
	"frank",			/* frankens */
	"frost",			/* frost */
	"goril",			/* thegoril */
	"grand",			/* grandpa */
	"hawk",				/* hawk */
	"horse",			/* horse_nm */
	"horn",				/* hugo */
	"icema",			/* iceman */
	"jacko",			/* jackonm */
	"khosk",			/* kerri */
	"legen",			/* legend */
	"femal",			/* lia */
	"mummy",			/* themummy */
	"nikko",			/* nikkonm */
	"oman",				/* oldman */
	"phat",				/* phat */
	"pinke",			/* pinkertn */
	"pinto",			/* pinto */
	"raptr",			/* therapto */
	"creff",			/* theref */
	"retro",			/* retrorob */
	"rock",				/* therock */
	"rocky",			/* rocky_nm */
	"sasqu",			/* sasquatc */
	"shama",			/* shaman */
	"shoop",			/* shoop */
	"sly",				/* sly */
	"jam",				/* spacey */
	"stall",			/* stallion */
	"toolm",			/* toolman */
	"hbear",			/* turbo */
	"twili",			/* twilight */
	"tzone",			/* tzone */
	"valky",			/* valkyrie */
	"wizz", 			/* thewizz */
	"wizrd",			/* wizard */
	"wolfm",			/* wolfman */
	"zephy",			/* zephyr */
	"ekeen",			/* special ed keenan bank */
	"cmedn",			/* special cary mednick bank */
	"uteeb",			/* special UTB bank */
	"pumpk",			/* special pumpkin-head bank */
};
static int name_list_bank_delay[] = {
	0, 44, 39, 46, 33, 38, 47, 44, 45, 35,
	41, 35, 49, 28, 31, 49, 36, 64, 34, 39,
	52, 36, 32, 46, 46, 46, 39, 41, 42, 49,
	47, 59, 26, 43, 45, 44, 39, 64, 39, 40,
	47, 38, 25, 42, 49, 46, 49, 50, 36, 43,
	46, 43, 41, 60, 39
};

static image_sprite edit_name_sprite_list[EDIT_NAME_SPRITE_COUNT] = {
	{&pinbkshd, NULL},
	{&pinbkdrp2, NULL},
	{&crescent, NULL},
	{&editdisc, NULL},
	{&awrd, NULL},
	{&bwrd, NULL},
	{&cwrd, NULL},
	{&dwrd, NULL},
	{&ewrd, NULL},
	{&fwrd, NULL},
	{&gwrd, NULL},
	{&hwrd, NULL},
	{&iwrd, NULL},
	{&jwrd, NULL},
	{&kwrd, NULL},
	{&lwrd, NULL},
	{&mwrd, NULL},
	{&nwrd, NULL},
	{&owrd, NULL},
	{&pwrd, NULL},
	{&qwrd, NULL},
	{&rwrd, NULL},
	{&swrd, NULL},
	{&twrd, NULL},
	{&uwrd, NULL},
	{&vwrd, NULL},
	{&wwrd, NULL},
	{&xwrd, NULL},
	{&ywrd, NULL},
	{&zwrd, NULL},
	{&exclmwrd, NULL},
	{&backwrd, NULL},
	{&spacewrd, NULL},
	{&endwrd, NULL},

	{&wrd0, NULL},
	{&wrd1, NULL},
	{&wrd2, NULL},
	{&wrd3, NULL},
	{&wrd4, NULL},
	{&wrd5, NULL},
	{&wrd6, NULL},
	{&wrd7, NULL},
	{&wrd8, NULL},
	{&wrd9, NULL},
	{&spacewrd, NULL},
	{&backwrd, NULL},
	{&astrxwrd, NULL},
	
	{&tikwrd, NULL},
	{&tikwrd, NULL},
	{&tikwrd, NULL},
	{&tikwrd, NULL},
	{&tikwrd, NULL},
	{&tikwrd, NULL},

	{&tikwrd, NULL},
	{&tikwrd, NULL},
	{&tikwrd, NULL},
	{&tikwrd, NULL},
	{&tikwrd, NULL},
	{&tikwrd, NULL}
};

static yes_no_button_t yes_no_button_list[YES_NO_BUTTON_COUNT] = {
	{{&exitdscr, NULL},
	 {&exitdscs, NULL},
	 {&yeswrdnw, NULL}},
	{{&exitdscr, NULL},
	 {&exitdscs, NULL},
	 {&nowrdnw, NULL}}
};

static int valid_head;
static int clock_mode;
static int player_mask;
static float save_fovval;
static int creatp_active;
static int kill_transit;
static process_node_t *flasher_proc;
static ostring_t *flasher_str;
static sprite_info_t *oval_sparkle;
static sprite_info_t *top_sparkle;
static sprite_info_t *credit_sparkle;
static sprite_info_t *glow_sparkle_target = NULL;
static int glow_cursor_x_pos, glow_cursor_y_pos;
static int yes_no_result;
static int fade_sprite_list_index;
static sprite_info_t *fade_sprite_list_sprites[FADE_LIST_SIZE];
static char current_flesh_texture[13];
static char current_head_texture[13];
static char current_jersey_texture[13];
static char current_jnum_texture[13];
static bbplyr_data *creatp_obj_data;
static player_record_info creatp_player_data;
static int guest_present;				/* is there a guest creatp head set up */
static int master_key;
static char *lock_pick_table[] = {"UNLOCK", "ALL   ", "HEADS "};
static char creatp_music_bank[] = CREATP_MUSIC;
static char creatp_snd_bank[] = CREATP_BANK;
static char creatp_name_bank[] = "ALLCREAT";

/*
 *		GLOBAL FUNCTIONS
 */

/* args[0] is the player number(0 - 3) that is creating the player */
void creatp_proc(int *args)
{
	process_node_t *timer_proc, *credit_proc, *glow_proc;
	int i, err;
	
	/* revector start button to the credit not taken plaque */
	start_enable(FALSE);
	
	set_dcsw_handler(P1_START_SWID, start_button_handler);
	set_dcsw_handler(P2_START_SWID, start_button_handler);
	if (four_plr_ver) {
		/* 2/4 plyr kit logic */
		set_dcsw_handler(P3_START_SWID, start_button_handler);
		set_dcsw_handler(P4_START_SWID, start_button_handler);
	}
	/* kill p_status */
	p_status = 0;
	p_status2 = 0;
	bought_in_first = 0;
	
	pdata_blocks[0].quarters_purchased = 0;
	pdata_blocks[1].quarters_purchased = 0;
	pdata_blocks[2].quarters_purchased = 0;
	pdata_blocks[3].quarters_purchased = 0;
	
	turn_transit_on();
	/* kill any lingering upfront stuff */
	wipeout();
	/* make sure drawing is enabled */
	draw_enable(1);
	
	load_creatp_sound_banks();
	
	if (load_texture_list(creatp_texture_list)) {
		lock_multiple_textures(CREATP_TID, 0xFFFFFFFF);
		lock_multiple_textures(TRANS_TID, 0xFFFFFFFF);
		
		clock_mode = CLOCK_NORMAL;
		
		/* create the timer process */
		timer_proc = qcreate("CP_timer", TIMER_PID, proc_timer, 0, 0, 0, 0);
		
		/* create the credit display process */
		credit_proc = qcreate("CP_CRED", CREDIT_PID, proc_credit_msg, 0, 0, 0, 0);
		
		/* create the glow proc */
		glow_proc = qcreate("CP_GLOW", GLOW_PID, proc_glow_sparkle, 0, 0, 0, 0);
		
		/* set the global so everyone can find the control input */
		player_mask = args[0] << 3;
		
		/* start the main loop */
		creatp_active = TRUE;
		master_key = 0;
		mini_attribute_list = is_low_res ? mini_attribute_list_low : mini_attribute_list_mid;
		mode_loop(creatp_init, creatp_loop, creatp_exit);
		
		/* kill the timer proc */
		if (existp(TIMER_PID, 0xFFFFFFFF) != NULL)
			kill(timer_proc, PD_DIE);
		
		/* kill the credit proc */
		if (existp(CREDIT_PID, 0xFFFFFFFF) != NULL)
			kill(credit_proc, PD_DIE);
		
		/* kill the glow proc */
		if (existp(GLOW_PID, 0xFFFFFFFF) != NULL)
			kill(glow_proc, PD_DIE);
		
		wipeout();
		
		/* clear the head cache pointers */
		unload_player_heads();
		unload_player_models();
		
		creatp_exit_page();
		turn_transit_off(1);
		wait_for_button(1 * tsec, 18 * tsec);
		fade(0.0f, 30, NULL);
		sleep(30);

		/* kill any lingering strings */
		delete_multiple_strings(TIMER_SID, 0xFFFFFFFF);
		delete_multiple_strings(BUTTON_INSTRUCTION_SID, 0xFFFFFFFF);
		
		unlock_multiple_textures(TRANS_TID, 0xFFFFFFFF);
		delete_multiple_textures(TRANS_TID, 0xFFFFFFFF);
		unlock_multiple_textures(CREATP_TID, 0xFFFFFFFF);
		delete_multiple_textures(CREATP_TID, 0xFFFFFFFF);
	}
	
	reload_sound_banks();
	
	/* restore the start buttons */
	start_enable(TRUE);
	/* start up the attract mode */
	iqcreate("attract", AMODE_GRP_PID, attract_loop, 0, 0, 0, 0);
	/* kill this proc */
	die(0);
}	/* creatp_proc */

char *creatp_index_to_nickname(int nickname_num)
{
	if ((nickname_num < 0) || (nickname_num > ARRAY_SIZE(name_list_bank)))
		return NULL;
	else
		return name_list_bank[nickname_num];
}	/* creatp_index_to_nickname */

/*
 *		STATIC FUNCTIONS
 */

static void cache_data(void)
{
	Texture_node_t *temp_tex;
	player_textures tex;
	player_team team;
	int i;
	
	/* preload all of the body models */
	for (i = CHA_MASCOT; i <= NBA_PLAYER; i++) {
		LIMB **body_model, **jnum_model, **reflect_model;
		
		load_player_model(i, &body_model, &jnum_model, &reflect_model);
	}
	/* preload all of the head models */
	
	/* preload all of the flesh textures */
	team.my_team = TEAM_ATL;
	team.opp_team = NO_OTHER_TEAM;
	team.jersey = DEFAULT_UNIFORM_TEAM;
	team.jersey_style = AWAY_JERSEY;
	for (i = 1; i < CREATP_HEADS; i++) {		/* skip the inital guest slot */
		get_player_textures(&team, creatp_head_list[i].team, creatp_head_list[i].player_index, &tex, TRUE);
		load_a_player_texture(tex.flesh_texture, CREATP_CACHE_FLESH_TID, &temp_tex, TRUE);
		lock_multiple_textures(CREATP_CACHE_FLESH_TID, 0xFFFFFFFF);
	}
	/* preload all of the head textures */
	for (i = 1; i < CREATP_HEADS; i++) {		/* skip the inital guest slot */
		get_player_textures(&team, creatp_head_list[i].team, creatp_head_list[i].player_index, &tex, TRUE);
		load_a_player_texture(tex.head_texture, CREATP_CACHE_HEAD_TID, &temp_tex, FALSE);
		lock_multiple_textures(CREATP_CACHE_HEAD_TID, 0xFFFFFFFF);
	}
}	/* cache_data */
	
static void load_creatp_sound_banks(void)
{
	int err;
	
	err = snd_bank_load_playing(creatp_music_bank);
	if (err == 0xEEEE)
		increment_audit(SND_LOAD_FAIL_AUD);
	/* start the looping background music */
	snd_stop_track(SND_TRACK_0);
	snd_scall_bank(creatp_music_bank, 1, VOLUME7, 127, SND_PRI_SET | SND_PRI_TRACK0);

	err = snd_bank_load_playing(creatp_snd_bank);
	if (err == 0xEEEE)
		increment_audit(SND_LOAD_FAIL_AUD);
	
	err = snd_bank_load_playing(creatp_name_bank);
	if (err == 0xEEEE)
		increment_audit(SND_LOAD_FAIL_AUD);
}	/* load_creatp_sound_banks */

static void reload_sound_banks(void)
{
	int err;
	
	/* stop the background music */
	snd_stop_all();
	/* delete the creatp banks */
	snd_bank_delete(creatp_name_bank);
	snd_bank_delete(creatp_snd_bank);
	snd_bank_delete(creatp_music_bank);
	snd_set_reserved(SND_TRACK_0);
}	/* reload_sound_banks */

/*
 * the plaque is 121 wide
 * the gap between plaques 1 and 2, 3 and 4 is 6
 * the gap between 2 and 3 is 4
*/
static void start_button_handler(int sig, int swid)
{
	int x_pos, pid;
	
	if (sig == SWITCH_OPEN)
		return;
	
	switch (swid) {
	default:
	case P1_START_SWID:
		if (existp(CREDIT_NOT_TAKEN1_PID, 0xFFFFFFFF) != NULL)
			return;
		if (four_plr_ver)
			x_pos = 5;
		else
			x_pos = 68;
		pid = CREDIT_NOT_TAKEN1_PID;
		break;
	case P2_START_SWID:
		if (existp(CREDIT_NOT_TAKEN2_PID, 0xFFFFFFFF) != NULL)
			return;
		if (four_plr_ver)
			x_pos = 5 + 121 + 6;
		else
			x_pos = 68 + 121 + 131;
		pid = CREDIT_NOT_TAKEN2_PID;
		break;
	case P3_START_SWID:
		if (!four_plr_ver || (existp(CREDIT_NOT_TAKEN3_PID, 0xFFFFFFFF) != NULL))
			return;
		x_pos = 5 + 121 + 6 + 121 + 4;
		pid = CREDIT_NOT_TAKEN3_PID;
		break;
	case P4_START_SWID:
		if (!four_plr_ver || (existp(CREDIT_NOT_TAKEN4_PID, 0xFFFFFFFF) != NULL))
			return;
		x_pos = 5 + 121 + 6 + 121 + 4 + 121 + 6;
		pid = CREDIT_NOT_TAKEN4_PID;
		break;
	}
	qcreate("CRED_PLQ", pid, credit_not_taken_proc, x_pos, 0, 0, 0);
}	/* start_button_handler */

/*
 * the plaque is 138 tall
 * raise the plaque over .25 seconds
 * display the plaque for 1.75 seconds
 * lower the plaque over .15 seconds
 * args[0] is the xpos to place the plaque
*/
#define PLAQUE_HEIGHT		138
#define PLAQUE_BOTTOM_GAP	2
static void credit_not_taken_proc(int *args)
{
	sprite_info_t *plaque;
	float ticks, plaque_velocity, y, final_pos;
	int i;	

	final_pos = PLAQUE_HEIGHT + PLAQUE_BOTTOM_GAP;
	plaque = beginobj(&credtwrn, 0.0f, SPRITE_VRES, 1.0F, 0);
	ticks = 15;
	plaque_velocity = final_pos / ticks;
	y = 0.0F;
	
	set_sprite_pos(plaque, args[0], y);
	sleep(1);
	
	/* bring the plaque on */
	for (i = 0; i < ticks; i++) {
		y += plaque_velocity;
		if (y > final_pos)
			y = final_pos;
		set_sprite_pos(plaque, args[0], y);
		sleep(1);
	}
	set_sprite_pos(plaque, args[0], final_pos);
	
	/* let them read the plaque */
	sleep(tsec * 1.75);

	ticks = 7;
	plaque_velocity = final_pos / ticks;
	/* take the plaque away */
	for (i = 0; i < ticks; i++) {
		y -= plaque_velocity;
		set_sprite_pos(plaque, args[0], y);
		sleep(1);
	}
	
	delobj(plaque);
	die(0);
}	/* credit_not_taken_proc */

static void player_data_init(int new_record)
{
	int jnum;
	
	/* do not init the name and pin, edit_name_exit() fills them in */
	creatp_player_data.cmos_data.last_used = 0;
	if (new_record) {
		creatp_player_data.cmos_data.games_played = 0;
		creatp_player_data.cmos_data.games_won = 0;
		creatp_player_data.cmos_data.winstreak = 0;
		creatp_player_data.cmos_data.two_ptrs_attempted = 0;
		creatp_player_data.cmos_data.two_ptrs_made = 0;
		creatp_player_data.cmos_data.three_ptrs_attempted = 0;
		creatp_player_data.cmos_data.three_ptrs_made = 0;
		creatp_player_data.cmos_data.dunks_attempted = 0;
		creatp_player_data.cmos_data.dunks_made = 0;
		creatp_player_data.cmos_data.rebounds = 0;
		creatp_player_data.cmos_data.assists = 0;
		creatp_player_data.cmos_data.steals = 0;
		creatp_player_data.cmos_data.blocks = 0;
		creatp_player_data.cmos_data.turnovers = 0;
		creatp_player_data.cmos_data.points_scored = 0;
		creatp_player_data.cmos_data.points_allowed = 0;
		creatp_player_data.cmos_data.teams_defeated = 0;
		creatp_player_data.cmos_data.alt_ladder = 0;
		creatp_player_data.cmos_data.trivia_pts = 0;
	}
	
	/* tag this as a creatp record */
	creatp_player_data.cmos_data.is_creatp_record = TRUE;
	/* initial attribute points */
	creatp_player_data.cmos_data.height = 7;
	creatp_player_data.cmos_data.weight = 7;
	creatp_player_data.cmos_data.speed = 7;
	creatp_player_data.cmos_data.power = 7;
	creatp_player_data.cmos_data.twop = 7;
	creatp_player_data.cmos_data.threep = 7;
	creatp_player_data.cmos_data.dunks = 7;
	creatp_player_data.cmos_data.steal = 7;
	creatp_player_data.cmos_data.block = 7;
	creatp_player_data.cmos_data.dribble = 7;
	
	/* default to the first NBA players head */
	creatp_player_data.cmos_data.team = creatp_head_list[1].team;
	creatp_player_data.cmos_data.player_index = creatp_head_list[1].player_index;
	
	creatp_player_data.cmos_data.jersey_city = TEAM_ATL;				/* the default team jersey */
	creatp_player_data.cmos_data.jersey_style = AWAY_JERSEY;			/* away style */
	jnum = randrng(99);													/* random jersey number */
	creatp_player_data.cmos_data.jersey_num = ((jnum / 10) << 4) | (jnum % 10);
	creatp_player_data.cmos_data.privileges = 0x00;						/* no privs set */
	creatp_player_data.cmos_data.nickname = 0;							/* no nickname set */
	creatp_player_data.cmos_data.unused_attribute_points = 10;			/* start with 10 points to allocate */
	creatp_player_data.cmos_data.unused_privilege_points = 2;			/* start with 2 privs to allocate */
	creatp_player_data.cmos_data.hotspot = get_random_hotspot();
}	/* player_data_init */

static unsigned int get_random_hotspot(void)
{
	hotspot_data hs;
	
	hs.packed_data.stamp = HOTSPOT_DATA_STAMP;
	hs.packed_data.index = randrng(NUM_HOTSPOTS);
	hs.packed_data.x = (134.0F * randrng(HOTSPOT_RIGHT - HOTSPOT_LEFT)) / (HOTSPOT_RIGHT - HOTSPOT_LEFT);
	hs.packed_data.z = (146.0F * randrng(HOTSPOT_BOTTOM - HOTSPOT_TOP)) / (HOTSPOT_BOTTOM - HOTSPOT_TOP);
	return hs.data;
}	/* get_random_hotspot */

void creatp_decode_hotspot(unsigned int data, int use_custom, float *x, float *z)
{
	hotspot_data hs;
	
	hs.data = data;
	if ((hs.packed_data.stamp != HOTSPOT_DATA_STAMP) || !use_custom) {
		*x = hotspot_xzs[hs.packed_data.index % NUM_HOTSPOTS][0];
		*z = hotspot_xzs[hs.packed_data.index % NUM_HOTSPOTS][1];
	} else {
		*x = -hs.packed_data.x;
		*z = hs.packed_data.z - 73;
	}
}	/* creatp_decode_hotspot */

/*
 *	creatp mode functions
 */

static void creatp_exit_page(void)
{
	int i, fh, top, gap;
	
	/* draw backdround */
	beginobj(&cp_end,		0.0F, SPRITE_VRES, 500.0F, CREATP_TID);
	beginobj(&cp_end_c1,	0.0F, SPRITE_VRES, 500.0F, CREATP_TID);
	beginobj(&cp_end_c2,	0.0F, SPRITE_VRES, 500.0F, CREATP_TID);
	beginobj(&cp_end_c3,	0.0F, SPRITE_VRES, 500.0F, CREATP_TID);
	
	set_string_id(BUTTON_INSTRUCTION_SID);
	set_text_justification_mode(HJUST_CENTER);
	set_text_font(FONTID_CHATTEN14);
	set_text_z_inc(-0.10F);
	fh = get_font_height(FONTID_CHATTEN14) + (is_low_res ? 6 : 4);
	
	top = 270;
	gap = is_low_res ? 28 : 32;
	set_text_color(WHITE);
	print_message("YOU CAN REENTER CREATE PLAYER BY",
				  "USING YOUR PLAYERS NAME AND",
				  "PIN NUMBER", &top, gap, fh);
	
	set_text_color(LT_YELLOW);
	print_message("EXPERIMENT WITH DIFFERENT ATTRIBUTE",
				  "POINT COMBINATIONS TO FINE TUNE",
				  "YOUR PLAYER", &top, gap, fh);
	
	set_text_color(WHITE);
	print_message("EARN ADDITIONAL ATTRIBUTE POINTS",
				  "BY WINNING 3 GAMES",
				  NULL, &top, gap, fh);
	
	set_text_color(LT_YELLOW);
	print_message("UNLOCK SECRET PLAYERS BY WINNING", "GAMES", NULL, &top, gap, fh);
}	/* creatp_exit_page */

static void print_message(char *line1, char *line2, char *line3, int *y_pos, int gap, int fh)
{
	int y;
	
	y = *y_pos;
	set_text_position(256, y, 200.0f);
	oprintf(line1);
	y -= fh;
	set_text_position(256, y, 300.0f);
	oprintf(line2);
	if (line3 != NULL) {
		y -= fh;
		set_text_position(256, y, 400.0f);
		oprintf(line3);
	}
	*y_pos = y - gap;
}	/* print_message */

static void *creatp_init(void)
{
	creatp_data_t *data;
	int y_pos, i;
	
	data = malloc(sizeof(creatp_data_t));
	if (data != NULL) {
		
		/* build the background sprites */
		for (i = 0; i < ARRAY_SIZE(creatp_background_list1); i++)
			image_to_sprite(&creatp_background_list1[i], BACKGROUND_BLUE_Z, I2S_ORIG, I2S_ORIG, 0, 0, FALSE);
		for (i = 0; i < ARRAY_SIZE(creatp_background_list2); i++)
			image_to_sprite(&creatp_background_list2[i], BACKGROUND_GRAY_Z, I2S_ORIG, I2S_ORIG, 0, 0, FALSE);
		for (i = 0; i < ARRAY_SIZE(creatp_background_list3); i++)
			image_to_sprite(&creatp_background_list3[i], BACKGROUND_TITLE_Z, I2S_ORIG, I2S_ORIG, 0, 0, FALSE);
		for (i = 0; i < ARRAY_SIZE(creatp_background_list4); i++)
			image_to_sprite(&creatp_background_list4[i], BACKGROUND_SHADOW_Z, I2S_ORIG, I2S_ORIG, 0, 0, FALSE);
		for (i = 0; i < ARRAY_SIZE(creatp_background_list5); i++)
			image_to_sprite(&creatp_background_list5[i], BACKGROUND_TITLE_Z - 5.0f, I2S_ORIG, I2S_ORIG, 0, 0, FALSE);
		
		/* build the shadow sprites */
		for (i = 0; i < MAX_SHADOW_LINE + 3; i++)
			image_to_sprite(&creatp_shadow_list[i], MODE_TITLE_SHADOW_Z - (i >=2 ? 1.0F : 0.0F), I2S_ORIG, I2S_ORIG, 0, 0, TRUE);
		/* the middle bar get a high Z value */
		image_to_sprite(&creatp_shadow_list[MAX_SHADOW_LINE + 3], MODE_TITLE_Z, I2S_ORIG, I2S_ORIG, 0, 0, TRUE);
		
		/* build the button sprites */
		y_pos = SPRITE_VRES - 76;
		for (i = CREATP_VIEW_STAT_BUTTON; i < CREATP_BUTTON_COUNT; i++) {
			image_to_sprite(&creatp_button_list[i].unselected_middle, BUTTON_MIDDLE_Z, I2S_DELTA, I2S_ABS, 1, y_pos, FALSE);
			image_to_sprite(&creatp_button_list[i].button_bottom, BUTTON_TOP_Z, I2S_DELTA, I2S_ABS, 1, y_pos - 1, FALSE);
			image_to_sprite(&creatp_button_list[i].title, BUTTON_TITLE_Z, I2S_DELTA, I2S_ORIG, 1, 0, FALSE);
			
			image_to_sprite(&creatp_button_list[i].selected_middle, BUTTON_MIDDLE_Z, I2S_DELTA, I2S_ABS, 1, y_pos, TRUE);
			y_pos -= 36;
		}
		
		/* build the title sprites */
		for (i = 0; i < ARRAY_SIZE(creatp_title_list); i++)
			image_to_sprite(&creatp_title_list[i], MODE_TITLE_Z, I2S_ORIG, I2S_ORIG, 0, 0, TRUE);
		
		/* build the instruction background sprites */
		for (i = 0; i < ARRAY_SIZE(creatp_instruction_list); i++)
			image_to_sprite(&creatp_instruction_list[i], BACKGROUND_TITLE_Z - (i == 1 ? 6.0f : 5.0f), I2S_ORIG, I2S_ORIG, 0, 0, TRUE);
		
		/* the title starts hidden */
		hide_creatp_title();
		
		/* force the player to enter their name and pin first */
		data->force_enter_name_pin = TRUE;
	}
	return data;
}	/* creatp_init */

static void creatp_exit(void *data)
{
	int i;
	
	/* if the player record is valid (have a valid name/pin) save it to CMOS */
	if (creatp_player_data.record_num != -1) {
		snd_scall_bank(creatp_snd_bank, CREATP_ELECTRIC, VOLUME4, 127, PRIORITY_LVL3);
		set_generic_record(PLYR_RECS_ARRAY, creatp_player_data.record_num, &creatp_player_data.cmos_data);
		creatp_exit_message(TRUE);
	} else
		creatp_exit_message(FALSE);
	
	/* hide the screen */
	turn_transit_on();
	
	calc_creatp_audits();
	
	/* flush the CMOS cache */
	write_cmos_to_disk(TRUE);
	update_other_cmos_to_disk(TRUE);
		
	/* hide the mini attributes */
	mini_attribute_exit();
	
	/* delete the player */
	player_display_exit();
	
	hide_instruction();
	
	/* delete the instruction background sprites */
	for (i = 0; i < ARRAY_SIZE(creatp_instruction_list); i++)
		free_image_to_sprite(&creatp_instruction_list[i]);
	
	/* delete the title sprites */
	for (i = 0; i < ARRAY_SIZE(creatp_title_list); i++)
		free_image_to_sprite(&creatp_title_list[i]);
	
	/* delete the button sprites */
	for (i = CREATP_VIEW_STAT_BUTTON; i < CREATP_BUTTON_COUNT; i++) {
		free_image_to_sprite(&creatp_button_list[i].unselected_middle);
		free_image_to_sprite(&creatp_button_list[i].selected_middle);
		free_image_to_sprite(&creatp_button_list[i].button_bottom);
		free_image_to_sprite(&creatp_button_list[i].title);
	}
	
	/* delete the shadow sprites */
	for (i = 0; i < MAX_SHADOW_LINE + 4; i++)
		free_image_to_sprite(&creatp_shadow_list[i]);
	
	/* delete the background sprites */
	for (i = 0; i < ARRAY_SIZE(creatp_background_list1); i++)
		free_image_to_sprite(&creatp_background_list1[i]);
	for (i = 0; i < ARRAY_SIZE(creatp_background_list2); i++)
		free_image_to_sprite(&creatp_background_list2[i]);
	for (i = 0; i < ARRAY_SIZE(creatp_background_list3); i++)
		free_image_to_sprite(&creatp_background_list3[i]);
	for (i = 0; i < ARRAY_SIZE(creatp_background_list4); i++)
		free_image_to_sprite(&creatp_background_list4[i]);
	for (i = 0; i < ARRAY_SIZE(creatp_background_list5); i++)
		free_image_to_sprite(&creatp_background_list5[i]);
	free(data);
}	/* creatp_exit */

static void creatp_exit_message(int which_message)
{
	static char *mode_instruction[] = {
		"TIMER EXPIRED", "NO RECORD CREATED",
		"RECORD SAVED", "YOU CAN PLAY THIS",
		"CHARACTER BY ENTERING", "NAME AND PIN AT",
		"START OF NEW GAME"
	};
	image_sprite title;
	float x, y, fh, delta;
	int font_id;
	
	title.image = &namepinm;
	title.sprite = NULL;
	
	do_glow_sparkle(NULL);
	hide_creatp_shadow();
	hide_creatp_title();
	hide_instruction();
	
	image_to_sprite(&title, MODE_TITLE_Z, I2S_ORIG, I2S_ABS, 0, SPRITE_VRES - 126, FALSE);
	show_creatp_shadow(SPRITE_VRES - (126 + title.image->h + 2), 1, which_message ? 5 : 2, 0, 1);
	
	font_id = is_low_res ? FONTID_NBA10 : FONTID_NBA12;
	set_text_font(font_id);
	set_string_id(BUTTON_INSTRUCTION_SID);
	set_text_justification_mode(HJUST_CENTER);
	set_text_color(OFF_YELLOW_COLOR);
	set_text_z_inc(-0.10F);
	
	fh = get_font_height(font_id);
	x = 246;
	y = SPRITE_VRES - 174;
	
	if (which_message) {
		delta = (fh * 0.5f) + 10;
		set_text_position(x, y + (fh * 0.5f), INSTRUCTION_Z);
		flasher_str = oprintf("%s", mode_instruction[2]);
		set_text_position(x, y - delta, INSTRUCTION_Z);
		oprintf("%s", mode_instruction[3]);
		set_text_position(x, y - 2 * delta, INSTRUCTION_Z);
		oprintf("%s", mode_instruction[4]);
		set_text_position(x, y - 3 * delta, INSTRUCTION_Z);
		oprintf("%s", mode_instruction[5]);
		set_text_position(x, y - 4 * delta, INSTRUCTION_Z);
		oprintf("%s", mode_instruction[6]);
	} else {
		set_text_position(x, y + (fh * 0.5f), INSTRUCTION_Z);
		oprintf("%s", mode_instruction[0]);
		set_text_position(x, y - ((fh * 0.5f) + 5), INSTRUCTION_Z);
		flasher_str = oprintf("%s", mode_instruction[1]);
	}
	set_text_z_inc(0.0F);
	
	flasher_proc = qcreate("flashE", MESSAGE_PID, proc_flash_text, 0, 0, 0, 0);
	wait_for_button(20, 5 * tsec);
	
	/* stop the text flashing */
	if (flasher_proc != NULL) {
		kill(flasher_proc, 0);
		flasher_proc = NULL;
	}
	delete_multiple_strings(BUTTON_INSTRUCTION_SID, 0xFFFFFFFF);
	hide_creatp_shadow();
	free_image_to_sprite(&title);
}	/* creatp_exit_message */

static void calc_creatp_audits(void)
{
	plyr_record_t tmp_record;
	int i, total_records, total_noncp_records;
	int total_mascot_head, total_nba_head, total_female_head, total_um_head, total_mid_head, total_other_head;
	int total_def_uniform, total_custom_uniform, total_team_uniform;
	int total_nickname;
	int total_height, total_weight, total_power, total_speed, total_twop, total_threep;
	int total_steal, total_block, total_dribble, total_dunk;
	int total_priv1, total_priv2, total_priv3, total_priv4;
	int total_priv5, total_priv6, total_priv7, total_priv8;
	
	get_all_records();
	total_records = total_noncp_records = 0;
	total_mascot_head = total_nba_head = total_female_head = total_um_head = total_mid_head = total_other_head = 0;
	total_def_uniform = total_custom_uniform = total_team_uniform = 0;
	total_nickname = 0;
	total_height = total_weight = total_power = total_speed = total_twop = total_threep = 0;
	total_steal = total_block = total_dribble = total_dunk = 0;
	total_priv1 = total_priv2 = total_priv3 = total_priv4 = 0;
	total_priv5 = total_priv6 = total_priv7 = total_priv8 = 0;
	for (i = 0; i < NUM_PLYR_RECORDS; i++) {
		get_generic_record(PLYR_RECS_ARRAY, i, &tmp_record);
		if (tmp_record.is_creatp_record || (tmp_record.games_played > 0))
			total_noncp_records++;
		if (tmp_record.is_creatp_record) {
			total_records++;
			if (tmp_record.team == TEAM_SECRET) {
				if ((tmp_record.player_index >= CHA_MASCOT) && (tmp_record.player_index <= UTA_MASCOT))
					total_mascot_head++;
				else if ((tmp_record.player_index == LIA_CHARACTER) ||
						 (tmp_record.player_index == KERRI_CHARACTER) ||
						 (tmp_record.player_index == CHEERLEADER_LM_CHARACTER) ||
						 (tmp_record.player_index == CHEERLEADER_KH_CHARACTER) ||
						 (tmp_record.player_index == CHEERLEADER_JH_CHARACTER) ||
						 (tmp_record.player_index == JENNIFER_H_CHARACTER) ||
						 (tmp_record.player_index == BETH_S_CHARACTER))
					total_female_head++;
				else if ((tmp_record.player_index == FRANK_CHARACTER) ||
						 (tmp_record.player_index == MUMMY_CHARACTER) ||
						 (tmp_record.player_index == BRIDE_CHARACTER) ||
						 (tmp_record.player_index == WOLFMAN_CHARACTER) ||
						 (tmp_record.player_index == CREATURE_CHARACTER))
					total_um_head++;
				else if (tmp_record.player_index >= MARK_T_CHARACTER)
					total_mid_head++;
				else
					total_other_head++;
			} else
				total_nba_head++;
			if ((tmp_record.privileges & PRIV_JERSEY) == 0)
				total_def_uniform++;
			else if (tmp_record.jersey_city == CUSTOM_UNIFORM_TEAM)
				total_custom_uniform++;
			else
				total_team_uniform++;
			if (tmp_record.nickname != 0)
				total_nickname++;
			total_height += tmp_record.height;
			total_weight += tmp_record.weight;
			total_power += tmp_record.power;
			total_speed += tmp_record.speed;
			total_twop += tmp_record.twop;
			total_threep += tmp_record.threep;
			total_steal += tmp_record.steal;
			total_block += tmp_record.block;
			total_dunk += tmp_record.dunks;
			total_dribble += tmp_record.dribble;
			if (tmp_record.privileges & 0x01)
				total_priv1++;
			if (tmp_record.privileges & 0x02)
				total_priv2++;
			if (tmp_record.privileges & 0x04)
				total_priv3++;
			if (tmp_record.privileges & 0x08)
				total_priv4++;
			if (tmp_record.privileges & 0x10)
				total_priv5++;
			if (tmp_record.privileges & 0x20)
				total_priv6++;
			if (tmp_record.privileges & 0x40)
				total_priv7++;
			if (tmp_record.privileges & 0x80)
				total_priv8++;
		}
	}
	set_audit(CREATED_PLAYER_RECORD_PERCENT_AUD, (total_records * 100) / total_noncp_records);
	set_audit(CREATEP_HEAD_MASCOT_PERCENT_AUD, (total_mascot_head * 100) / total_records);
	set_audit(CREATEP_HEAD_NBA_STAR_PERCENT_AUD, (total_nba_head * 100) / total_records);
	set_audit(CREATEP_HEAD_FEMALE_PERCENT_AUD, (total_female_head * 100) / total_records);
	set_audit(CREATEP_HEAD_UM_PERCENT_AUD, (total_um_head * 100) / total_records);
	set_audit(CREATEP_HEAD_MID_PERCENT_AUD, (total_mid_head * 100) / total_records);
	set_audit(CREATEP_HEAD_OTHER_PERCENT_AUD, (total_other_head * 100) / total_records);
	set_audit(CREATEP_UNIFORM_DEFAULT_PERCENT_AUD, (total_def_uniform * 100) / total_records);
	set_audit(CREATEP_UNIFORM_CUSTOM_PERCENT_AUD, (total_custom_uniform * 100) / total_records);
	set_audit(CREATEP_UNIFORM_TEAM_PERCENT_AUD, (total_team_uniform * 100) / total_records);
	set_audit(CREATEP_NICKNAME_AUD_PERCENT, total_nickname);
	set_audit(CREATEP_HEIGHT_AVG_AUD, total_height / total_records);
	set_audit(CREATEP_WEIGHT_AVG_AUD, total_weight / total_records);
	set_audit(CREATEP_POWER_AVG_AUD, total_power / total_records);
	set_audit(CREATEP_SPEED_AVG_AUD, total_speed / total_records);
	set_audit(CREATEP_TWOPTS_AVG_AUD, total_twop / total_records);
	set_audit(CREATEP_THREEPTS_AVG_AUD, total_threep / total_records);
	set_audit(CREATEP_STEAL_AVG_AUD, total_steal / total_records);
	set_audit(CREATEP_BLOCK_AVG_AUD, total_block / total_records);
	set_audit(CREATEP_DUNKS_AVG_AUD, total_dunk / total_records);
	set_audit(CREATEP_DRIBBLE_AVG_AUD, total_dribble / total_records);
	set_audit(CREATEP_PRIV1_PERCENT_AUD, total_priv1);
	set_audit(CREATEP_PRIV2_PERCENT_AUD, total_priv2);
	set_audit(CREATEP_PRIV3_PERCENT_AUD, total_priv3);
	set_audit(CREATEP_PRIV4_PERCENT_AUD, total_priv4);
	set_audit(CREATEP_PRIV5_PERCENT_AUD, total_priv5);
	set_audit(CREATEP_PRIV6_PERCENT_AUD, total_priv6);
	set_audit(CREATEP_PRIV7_PERCENT_AUD, total_priv7);
	set_audit(CREATEP_PRIV8_PERCENT_AUD, total_priv8);
	free(rec_sort_ram);
	rec_sort_ram = NULL;
}	/* calc_creatp_audits */

static int creatp_loop(int input, void *data)
{
	creatp_data_t *creatp_data;
	int done;
	int start_x, start_y;
	
	creatp_data = data;
	done = FALSE;

	if (creatp_data->force_enter_name_pin) {
		/* start the mini attributes */
		mini_attribute_init();
		
		kill_transit = TRUE;
		
		/* init the head list */
		init_head_list();
		
#if !defined(DEBUG) && !defined(GRAB_SCREEN)
		/* not enough DRAM to capture screen shots with all models loaded up */
		cache_data();
#endif
		
		/* get the name and pin */
		edit_name_wrapper(TRUE);
		
		if (!creatp_active)
			return TRUE;
		
		/* show the labels */
		mini_attribute_show_labels();
		
		/* show all of the stats */
		mini_attribute_update(MINI_ATTR_ALL, -1);
		
		/* start working in normal mode */
		creatp_data->force_enter_name_pin = FALSE;
		creatp_data->current_button = CREATP_VIEW_STAT_BUTTON;
		select_creatp_button(creatp_data->current_button);
		show_button_instruction(creatp_data->current_button);		
		
		/* start diplaying the player */
		player_display_init();
	}
	if (next_button(CREATP_VIEW_STAT_BUTTON, CREATP_EXIT_SAVE_BUTTON, &creatp_data->current_button, input, TRUE)) {
		/* issue a sound call */
		snd_scall_bank(creatp_snd_bank, input == P_DOWN ? CREATP_DOWN : CREATP_UP, VOLUME4, 127, PRIORITY_LVL3);
		select_creatp_button(creatp_data->current_button);
		show_button_instruction(creatp_data->current_button);
	} else if ((input & P_ABC_MASK) != 0) {
		/* issue a sound call */
		snd_scall_bank(creatp_snd_bank, CREATP_SELECT, VOLUME4, 127, PRIORITY_LVL5);
		hide_creatp_shadow();
		show_creatp_title(creatp_data->current_button);
		
		find_sprite_center(creatp_selected_button->selected_middle.sprite, &start_x, &start_y);
		start_y -= 5;
		qcreate("CP_sprk", SPARKLE_PID, proc_sparkle, start_x, SPRITE_VRES - start_y, 200, 200);
		
		switch (creatp_data->current_button) {
		case CREATP_VIEW_STAT_BUTTON:
			mode_loop(view_stat_init, view_stat_loop, view_stat_exit);
			break;
		case CREATP_HEAD_BUTTON:
			mode_loop(head_init, head_loop, head_exit);
			break;
		case CREATP_NUMBER_BUTTON:
			mode_loop(uniform_number_init, uniform_number_loop, uniform_number_exit);
			break;
		case CREATP_ATTRIBUTES_BUTTON:
			mode_loop(attributes_init, attributes_loop, attributes_exit);
			break;
		case CREATP_PRIVILEGES_BUTTON:
			mode_loop(privileges_init, privileges_loop, privileges_exit);
			break;
		case CREATP_NICKNAME_BUTTON:
			namelist_mode = NAME_LIST;
			mode_loop(namelist_init, namelist_loop, namelist_exit);
			break;
		case CREATP_EDIT_NAME_BUTTON:
			/* make sure they really want to change the name/pin */
			show_creatp_title(EDIT_NAME_MODE);
			mode_loop(yes_no_init, yes_no_loop, yes_no_exit);
			if (yes_no_result)
				edit_name_wrapper(FALSE);
			break;
		case CREATP_EXIT_SAVE_BUTTON:
			mode_loop(yes_no_init, yes_no_loop, yes_no_exit);
			if (yes_no_result)
				creatp_active = FALSE;
			break;
		}
		hide_creatp_shadow();
		hide_creatp_title();
		if (creatp_active)
			show_button_instruction(creatp_data->current_button);
	}
	return done;
}	/* creatp_loop */

static void select_creatp_button(int button)
{
	int i;
	
	creatp_selected_button = &creatp_button_list[button];
	for (i = 0; i < CREATP_BUTTON_COUNT; i++) {
		hide_sprite(creatp_button_list[i].selected_middle.sprite);
		unhide_sprite(creatp_button_list[i].unselected_middle.sprite);
		set_sprite_color(creatp_button_list[i].title.sprite, WHITE);
	}
	hide_sprite(creatp_selected_button->unselected_middle.sprite);
	unhide_sprite(creatp_selected_button->selected_middle.sprite);
	set_sprite_color(creatp_selected_button->title.sprite, OFF_YELLOW_COLOR);
}	/* select_creatp_button */

static void show_creatp_title(int button)
{
	int i, orig_button;
	
	hide_creatp_title();
	
	/* show the title bar drop shadow */
	switch (button) {
	case CREATP_VIEW_STAT_BUTTON:
	case CREATP_NUMBER_BUTTON:
	case CREATP_HEAD_BUTTON:
	case CREATP_EDIT_NAME_BUTTON:
	case ENTER_NAME_MODE:
	case ENTER_PIN_MODE:
	case HOTSPOT_MODE:
		/* two lines */
		unhide_sprite(creatp_instruction_list[0].sprite);
		break;
	case CREATP_ATTRIBUTES_BUTTON:
		/* two lines and message area */
		unhide_sprite(creatp_instruction_list[0].sprite);
		unhide_sprite(creatp_instruction_list[1].sprite);
		set_sprite_pos(creatp_instruction_list[1].sprite, 149, SPRITE_VRES - 120);
		break;
	case CREATP_PRIVILEGES_BUTTON:
		/* three lines and message area */
		unhide_sprite(creatp_instruction_list[2].sprite);
		unhide_sprite(creatp_instruction_list[1].sprite);
		set_sprite_pos(creatp_instruction_list[1].sprite, 149, SPRITE_VRES - 136);
		break;
	case CREATP_NICKNAME_BUTTON:
	case CREATP_EXIT_SAVE_BUTTON:
	case UNIFORM_JERSEY_MODE:
	case EDIT_NAME_MODE:
		/* three lines */
		unhide_sprite(creatp_instruction_list[2].sprite);
		break;
	}
	orig_button = button;
	if (button < 0)
		button = CREATP_EXIT_SAVE_BUTTON + -button;
	unhide_sprite(creatp_title_list[button].sprite);

	/* show the mode instructions */
	show_mode_instruction(orig_button);
}	/* show_creatp_title */

static void hide_creatp_title(void)
{
	int i;
	
	/* hide the title bar drop shadow */
	for (i = 0; i < ARRAY_SIZE(creatp_instruction_list); i++)
		hide_sprite(creatp_instruction_list[i].sprite);
	for (i = 0; i < ARRAY_SIZE(creatp_title_list); i++)
		hide_sprite(creatp_title_list[i].sprite);
}	/* hide_creatp_title */

static void show_creatp_shadow(int y_pos, int title_bar, int num_lines, int bar_pos, int bottom_piece)
{
	image_sprite *is;
	int i, height;
	
	if (num_lines > MAX_SHADOW_LINE)
		num_lines = MAX_SHADOW_LINE;
	
	if (title_bar >= 0) {
		is = &creatp_shadow_list[title_bar];
		height = (title_bar == 0) ? 12 : 18;
		y_pos += height;
		
		/* place the top piece */
		set_sprite_pos(is->sprite, 146, y_pos);
		unhide_sprite(is->sprite);
		y_pos -= height;
	}
	
	/* place the middle pieces */
	for (i = 0; i < num_lines; i++) {
		is = &creatp_shadow_list[i + 2];
		set_sprite_pos(is->sprite, 153, y_pos);
		unhide_sprite(is->sprite);
		y_pos -= is->image->h;
	}
	
	/* place the bottom piece */
	if (bottom_piece) {
		is = &creatp_shadow_list[MAX_SHADOW_LINE + 2];
		set_sprite_pos(is->sprite, 150, y_pos);
		unhide_sprite(is->sprite);
	}
	
	/* place the optional middle bar */
	if (bar_pos > 0) {
		is = &creatp_shadow_list[MAX_SHADOW_LINE + 3];
		set_sprite_pos(is->sprite, 150, bar_pos);
		unhide_sprite(is->sprite);
	}
	
	/* make sure the alpha is full */
	for (i = 0; i < MAX_SHADOW_LINE + 4; i++)
		creatp_shadow_list[i].sprite->state.constant_color |= 0xFF000000;
}	/* show_creatp_shadow */
	
static void hide_creatp_shadow(void)
{
	int i;
	
	for (i = 0; i < MAX_SHADOW_LINE + 4; i++)
		hide_sprite(creatp_shadow_list[i].sprite);
}	/* hide_creatp_shadow */

static int mini_attribute_y_pos[4] = {39, 31, 23, 15};
static void mini_attribute_init(void)
{
	int i, save_low_res, delta;
	
	save_low_res = is_low_res;
	if (is_low_res)
		is_low_res = 3;
	for (i = 0; i < ARRAY_SIZE(mini_attribute_list_mid); i++) {
		if (is_low_res && (i < 16)) {
			is_low_res = 0;
			image_to_sprite(&mini_attribute_list[i], BUTTON_TOP_Z - i, I2S_ORIG, I2S_ABS, 0, mini_attribute_y_pos[i % 4], TRUE);
			is_low_res = 3;
		} else
			image_to_sprite(&mini_attribute_list[i], BUTTON_TOP_Z - i, I2S_ORIG, I2S_ORIG, 0, 0, TRUE);
		mini_attribute_list[i].sprite->state.texture_filter_mode = GR_TEXTUREFILTER_POINT_SAMPLED;
	}
	is_low_res = save_low_res;
}	/* mini_attribute_init */

static void mini_attribute_exit(void)
{
	int i;
	
	for (i = 0; i < ARRAY_SIZE(mini_attribute_list_mid); i++)
		free_image_to_sprite(&mini_attribute_list[i]);
}	/* mini_attribute_exit */

static void mini_attribute_show_labels(void)
{
	int i;
	
	for (i = 0; i < 8; i++)
		unhide_sprite(mini_attribute_list[i].sprite);
}	/* mini_attribute_show_labels */

static void mini_attribute_reset_labels(void)
{
	int i;
	
	for (i = 22; i <= 29; i++)
		set_sprite_color(mini_attribute_list[i].sprite, WHITE);
	for (i = 0; i < ATTRIBUTES_BUTTON_COUNT - 2; i++)
		set_sprite_color(mini_attribute_list[i].sprite, WHITE);
}	/* mini_attribute_reset_labels */

static void mini_attribute_update(int which, int selected_attr)
{
	static image_info_t *num_list[21] = {
		&lattr_0, &lattr_1, &lattr_2, &lattr_3, &lattr_4, &lattr_5,
		&lattr_6, &lattr_7, &lattr_8, &lattr_9, &lattr_10,
		&lattr_11, &lattr_12, &lattr_13, &lattr_14, &lattr_15,
		&lattr_16, &lattr_17, &lattr_18, &lattr_19, &lattr_20
	};
	static image_info_t *num_list_low[21] = {
		&lrattr_0, &lrattr_1, &lrattr_2, &lrattr_3, &lrattr_4, &lrattr_5,
		&lrattr_6, &lrattr_7, &lrattr_8, &lrattr_9, &lrattr_10,
		&lrattr_11, &lrattr_12, &lrattr_13, &lrattr_14, &lrattr_15,
		&lrattr_16, &lrattr_17, &lrattr_18, &lrattr_19, &lrattr_20
	};
	image_info_t **nlist;
	char actual_height;					/* height in inches */
	short actual_weight;				/* weight in pounds */
	char height_str[16], weight_str[16];
	int save_low_res;
	
	nlist = is_low_res ? num_list_low : num_list;
	if (which & MINI_ATTR_NAME)
		mini_attribute_draw_name_string(creatp_player_data.cmos_data.plyr_name, LETTERS_IN_NAME, 392, SPRITE_VRES - 285, 16);
	if ((which & MINI_ATTR_HEIGHT) || (which & MINI_ATTR_WEIGHT)) {
		creatp_calc_height_weight(creatp_player_data.cmos_data.height,
								  creatp_player_data.cmos_data.weight,
								  &actual_height, &actual_weight);
		sprintf(height_str, "%d-%d", actual_height / 12, actual_height % 12);
		mini_attribute_draw_num_string(height_str, 4, 392, SPRITE_VRES - 312, 22, nlist, FALSE, (selected_attr == ATTRIBUTES_HEIGHT_BUTTON) ? OFF_YELLOW_COLOR : WHITE);
		
		sprintf(weight_str, "%d", actual_weight);
		mini_attribute_draw_num_string(weight_str, 3, 432, SPRITE_VRES - 312, 26, nlist, TRUE, (selected_attr == ATTRIBUTES_WEIGHT_BUTTON) ? OFF_YELLOW_COLOR : WHITE);
	}
	save_low_res = is_low_res;
	is_low_res = 0;
	if (which & MINI_ATTR_POWER)
		mini_attribute_set_sprite(mini_attribute_list[8].sprite, nlist[(int)creatp_player_data.cmos_data.power],  406 + mini_attribute_x_delta(creatp_player_data.cmos_data.power), save_low_res ? mini_attribute_y_pos[0] : SPRITE_VRES - 325, mini_attribute_color(creatp_player_data.cmos_data.power));
	if (which & MINI_ATTR_SPEED)
		mini_attribute_set_sprite(mini_attribute_list[9].sprite, nlist[(int)creatp_player_data.cmos_data.speed],  406 + mini_attribute_x_delta(creatp_player_data.cmos_data.speed), save_low_res ? mini_attribute_y_pos[1] : SPRITE_VRES - 337, mini_attribute_color(creatp_player_data.cmos_data.speed));
	if (which & MINI_ATTR_TWOP)
		mini_attribute_set_sprite(mini_attribute_list[10].sprite, nlist[(int)creatp_player_data.cmos_data.twop],  406 + mini_attribute_x_delta(creatp_player_data.cmos_data.twop), save_low_res ? mini_attribute_y_pos[2] : SPRITE_VRES - 349, mini_attribute_color(creatp_player_data.cmos_data.twop));
	if (which & MINI_ATTR_THREEP)
		mini_attribute_set_sprite(mini_attribute_list[11].sprite, nlist[(int)creatp_player_data.cmos_data.threep],  406 + mini_attribute_x_delta(creatp_player_data.cmos_data.threep), save_low_res ? mini_attribute_y_pos[3] : SPRITE_VRES - 361, mini_attribute_color(creatp_player_data.cmos_data.threep));
	if (which & MINI_ATTR_STEAL)
		mini_attribute_set_sprite(mini_attribute_list[12].sprite, nlist[(int)creatp_player_data.cmos_data.steal],  483 + mini_attribute_x_delta(creatp_player_data.cmos_data.steal), save_low_res ? mini_attribute_y_pos[0] : SPRITE_VRES - 325, mini_attribute_color(creatp_player_data.cmos_data.steal));
	if (which & MINI_ATTR_BLOCK)
		mini_attribute_set_sprite(mini_attribute_list[13].sprite, nlist[(int)creatp_player_data.cmos_data.block],  483 + mini_attribute_x_delta(creatp_player_data.cmos_data.block), save_low_res ? mini_attribute_y_pos[1] : SPRITE_VRES - 337, mini_attribute_color(creatp_player_data.cmos_data.block));
	if (which & MINI_ATTR_DUNKS)
		mini_attribute_set_sprite(mini_attribute_list[14].sprite, nlist[(int)creatp_player_data.cmos_data.dunks],  483 + mini_attribute_x_delta(creatp_player_data.cmos_data.dunks), save_low_res ? mini_attribute_y_pos[2] : SPRITE_VRES - 349, mini_attribute_color(creatp_player_data.cmos_data.dunks));
	if (which & MINI_ATTR_DRIBBLE)
		mini_attribute_set_sprite(mini_attribute_list[15].sprite, nlist[(int)creatp_player_data.cmos_data.dribble],  483 + mini_attribute_x_delta(creatp_player_data.cmos_data.dribble), save_low_res ? mini_attribute_y_pos[3] : SPRITE_VRES - 361, mini_attribute_color(creatp_player_data.cmos_data.dribble));
	is_low_res = save_low_res;
}	/* mini_attribute_update */

static void mini_attribute_draw_name_string(char *str, int num_char, int start_x, int start_y, int sprite_index)
{
	static image_info_t *alph_list[26] = {
		&awrd12, &bwrd12, &cwrd12, &dwrd12, &ewrd12, &fwrd12, &gwrd12,
		&hwrd12, &iwrd12, &jwrd12, &kwrd12, &lwrd12, &mwrd12, &nwrd12,
		&owrd12, &pwrd12, &qwrd12, &rwrd12, &swrd12, &twrd12, &uwrd12,
		&vwrd12, &wwrd12, &xwrd12, &ywrd12, &zwrd12
	};
	image_info_t *image;
	int i;
	
	for (i = 0; i < num_char; i++) {
		if (str[i] == '-')
			image = &tikwrd12;
		else if (str[i] == '!') {
			image = &exclam12;
			start_x += 2;
		} else if (str[i] == ' ')
			image = &xwrd12;
		else
			image = alph_list[str[i] - 'A'];
		mini_attribute_set_sprite(mini_attribute_list[sprite_index + i].sprite, image, start_x, start_y, WHITE);
		if (str[i] == ' ')
			hide_sprite(mini_attribute_list[sprite_index + i].sprite);
		/* bump back the drop shadow */
		start_x += image->w - 2;
	}
}	/* mini_attribute_draw_name_string */

static void mini_attribute_draw_num_string(char *str, int num_char, int start_x, int start_y, int sprite_index, image_info_t **num_list, int weight_str, long color)
{
	image_info_t *image;
	int i, y_pos;
	
	for (i = 0; i < num_char; i++) {
		if (str[i] == '\0')
			hide_sprite(mini_attribute_list[sprite_index + i].sprite);
		else {
			if (str[i] == '-') {
				image = is_low_res ? &lrhyphen : &lhyphen;
				y_pos = start_y - 3;
			} else {
				image = num_list[str[i] - '0'];
				y_pos = start_y;
			}
			mini_attribute_set_sprite(mini_attribute_list[sprite_index + i].sprite, image, start_x, y_pos, color);
			/* bump back the drop shadow */
			start_x += image->w + 1;
		}
	}
	if (weight_str)
		mini_attribute_set_sprite(mini_attribute_list[sprite_index + i].sprite, is_low_res ? &lrpounds : &lpounds, start_x + 2, start_y, color);
}	/* mini_attribute_draw_num_string */

static int mini_attribute_x_delta(int value)
{
	if (value == 20)
		return 0;
	else if (value == 1)
		return 4;
	else if (value >= 10)
		return 1;
	else 
		return 3;
}	/* mini_attribute_x_delta */

static void mini_attribute_set_sprite(sprite_info_t *sprite, image_info_t *image, int x_pos, int y_pos, long color)
{
	int save_low_res;
	
	save_low_res = is_low_res;
	if (is_low_res)
		is_low_res = 3;
	unhide_sprite(sprite);
	sprite->ii = image;
	sprite->tn = find_texture(image->texture_name, CREATP_TID);
	set_sprite_pos(sprite, x_pos, y_pos);
	set_sprite_color(sprite, color);
	is_low_res = save_low_res;
}	/* mini_attribute_set_sprite */

static long mini_attribute_color(char attr)
{
	if (attr <= 3)
		return LT_RED;
	else if (attr >= 13)
		return LT_GREEN;
	else
		return LT_YELLOW;
}	/* mini_attribute_color */

#define H(ft, in, w)				((((ft) * 12) + (in)) * w)
#define EXTRA_WEIGHT				-1
#define _							+1
void creatp_calc_height_weight(char cp_height, char cp_weight, char *height, short *weight)
{
	static char height_table[NUM_ATTRIBUTE_LEVEL] = {
		H(5, 7, _), H(5, 8, _), H(5, 9, _), H(5, 10, _),
		H(5, 11, _), H(6, 0, _), H(6, 1, _), H(6, 2, EXTRA_WEIGHT),
		H(6, 3, _), H(6, 4, EXTRA_WEIGHT), H(6, 5, _), H(6, 6, EXTRA_WEIGHT),
		H(6, 7, _), H(6, 8, EXTRA_WEIGHT), H(6, 9, _), H(6, 10, EXTRA_WEIGHT),
		H(7, 0, EXTRA_WEIGHT), H(7, 2, EXTRA_WEIGHT), H(7, 4, EXTRA_WEIGHT), H(7, 6, EXTRA_WEIGHT)
	};
	static short weight_table[NUM_ATTRIBUTE_LEVEL] = {
		160, 165, 170, 175, 180, 185, 190, 195, 200, 207,
		214, 221, 228, 235, 242, 250, 258, 266, 276, 290
	};
	int i;
	
	*height = ABS(height_table[cp_height - 1]);
	*weight = weight_table[creatp_player_data.cmos_data.weight - 1];
	for (i = 0; i < cp_height; i++)
		if (height_table[i] < 0)
			*weight += 5;
}	/* creatp_calc_height_weight */
#undef H
#undef _


/*
 *	view stat mode functions
 */

static void *view_stat_init(void)
{
	view_stats_data_t *data;
	int i;
	
	/* allocate the private data */
	data = malloc(sizeof(view_stats_data_t));
	if (data != NULL) {
		data->page_number = 0;
		data->auto_select_timer = AUTO_SELECT_TIME_6;
		data->last_index = 0;
		data->did_first_flash = FALSE;
		for (i = 0; i < NUM_VIEW_STAT_BAR; i++) {
			image_to_sprite(&view_stats_bar[i].bar, BUTTON_TOP_Z, I2S_ORIG, I2S_ORIG, 0, 0, TRUE);
			image_to_sprite(&view_stats_bar[i].top1, GLOW_SPARKLE_Z, I2S_ORIG, I2S_ORIG, 0, 0, TRUE);
			image_to_sprite(&view_stats_bar[i].top5, GLOW_SPARKLE_Z, I2S_ORIG, I2S_ORIG, 0, 0, TRUE);
			image_to_sprite(&view_stats_bar[i].top10, GLOW_SPARKLE_Z, I2S_ORIG, I2S_ORIG, 0, 0, TRUE);
		}
		/* build the fade list */
		for (i = 0; i < MAX_SHADOW_LINE + 4; i++, fade_sprite_list_index++)
			fade_sprite_list_sprites[fade_sprite_list_index] = creatp_shadow_list[i].sprite;
		get_stat_info(&creatp_player_data.cmos_data, &data->info);
		show_view_stat(data);
	}
	return data;
}	/* view_stat_init */

static void view_stat_exit(void *data)
{
	int i;
	
	for (i = 0; i < NUM_VIEW_STAT_BAR; i++) {
		free_image_to_sprite(&view_stats_bar[i].bar);
		free_image_to_sprite(&view_stats_bar[i].top1);
		free_image_to_sprite(&view_stats_bar[i].top5);
		free_image_to_sprite(&view_stats_bar[i].top10);
	}
	hide_view_stat();
	set_text_z_inc(0.0F);
	free(data);
}	/* view_stat_exit */

static int view_stat_loop(int input, void *data)
{
	view_stats_data_t *view_stat_data;
	int done, fh, y, gap;
	
	view_stat_data = data;
	done = FALSE;

	if (!view_stat_data->did_first_flash) {
		view_stat_data->did_first_flash = TRUE;
		/* do the flashs for page 1 */
		fh = get_font_height(is_low_res ? FONTID_BAST8TA : FONTID_NBA10) + 3;
		gap = get_font_height(FONTID_NBA10) + 15;
		y = SPRITE_VRES - 154;
		flash_stat(&y, 0, fh, 0, view_stat_data->info.overall_rank);
		flash_stat(&y, gap, fh, 1, 0);
		flash_stat(&y, gap, fh, 2, view_stat_data->info.winstreak_rank);
		flash_stat(&y, gap, fh, 3, view_stat_data->info.off_rank);
		flash_stat(&y, gap, fh, 4, view_stat_data->info.def_rank);
		view_stat_data->auto_select_timer = AUTO_SELECT_TIME_6;
	}

	if (((input & P_ABC_MASK) != 0) || (--view_stat_data->auto_select_timer == 0)) {
		view_stat_data->auto_select_timer = AUTO_SELECT_TIME_6;
		snd_scall_bank(cursor_bnk_str, 38, VOLUME4, 127, PRIORITY_LVL4);
		done = show_view_stat(data);
	}
	return done;
}	/* view_stat_loop */

static void print_stat(char *line1, char *line2, int *y_pos, int gap, int fh, int index)
{
	int y;
	
	y = *y_pos - gap;
	
	unhide_sprite(view_stats_bar[index].bar.sprite);
	view_stats_bar[index].bar.sprite->x = 250;
	view_stats_bar[index].bar.sprite->y = y;
	generate_sprite_verts(view_stats_bar[index].bar.sprite);
	
	set_text_position(250, y + (is_low_res ? 3 : 1), INSTRUCTION_Z);
	oprintf("\e%dc""%s", LT_YELLOW, line1);
	y -= fh;
	set_text_position(250, y - (is_low_res ? 3 : 0), INSTRUCTION_Z);
	oprintf("\e%dc""%s", WHITE, line2);
	*y_pos = y;	
}	/* print_stat */

static void flash_stat(int *y_pos, int gap, int fh, int index, int rank)
{
	sprite_info_t *rank_sprite;
	int y, sound_num, delay;
	float scale;
	
	y = *y_pos - gap;
	delay = TRUE;
	sound_num = 27;
	if ((rank <= 10) && (rank != 0)) {
		if (rank <= 5)
			if (rank == 1) {
				rank_sprite = view_stats_bar[index].top1.sprite;
				sound_num = 29;
				delay = FALSE;
			} else
				rank_sprite = view_stats_bar[index].top5.sprite;
		else
			rank_sprite = view_stats_bar[index].top10.sprite;
		
		unhide_sprite(rank_sprite);
		rank_sprite->x = 250;
		rank_sprite->y = y;
		for (scale = 2.5f; scale > 1.0f; scale -= 0.10f) {
			rank_sprite->h_scale = scale;
			rank_sprite->w_scale = scale;
			generate_sprite_verts(rank_sprite);
			sleep(1);
		}
		
		/* sparkle the plaque */
		snd_scall_bank(cursor_bnk_str, sound_num, VOLUME4, 127, PRIORITY_LVL4);
		do_top_sparkle(rank_sprite, 100, 100, delay, FALSE);
		do_oval_sparkle(rank_sprite, 31, 46, delay, FALSE, FALSE);
		wait_for_button(15, delay ? 30 : 45);
	}
	*y_pos = y - fh;
}	/* flash_stat */

static void page1(view_stats_data_t *data, int y, int fh, int gap, int total_games)
{
	char line[256];
	
	data->page_number = 1;
	sprintf(line, "%d%s", data->info.overall_rank, rank_suffix(data->info.overall_rank));
	print_stat("OVERALL RANK", line, &y, 0, fh, 0);
	
	sprintf(line, "%dW-%dL %5.2f %%", data->info.game_win, data->info.game_loss, (data->info.game_win / (float)total_games) * 100.0f);
	print_stat("RECORD", line, &y, gap, fh, 1);
	
	if (data->info.winstreak < 0) {
		int tmp;
		
		tmp = -data->info.winstreak;
		sprintf(line, "%d LOSS%s", tmp, tmp != 1 ? "ES" : "");
	} else
		sprintf(line, "%d WIN%s", data->info.winstreak, data->info.winstreak != 1 ? "S" : "");
	print_stat("WIN STREAK", line, &y, gap, fh, 2);
	
	sprintf(line, "%d%s", data->info.off_rank, rank_suffix(data->info.off_rank));
	print_stat("OFFENSIVE RANK", line, &y, gap, fh, 3);
	
	sprintf(line, "%d%s", data->info.def_rank, rank_suffix(data->info.def_rank));
	print_stat("DEFENSIVE RANK", line, &y, gap, fh, 4);
}	/* page1 */

static void page2(view_stats_data_t *data, int y, int fh, int gap, int total_games)
{
	char line[256];
	float perc;
	int save_y;
	
	save_y = y;
	data->page_number = 2;
	sprintf(line, "%d - %5.2f / GAME", data->info.pts_scored, data->info.pts_scored / (float)total_games);
	print_stat("POINTS SCORED", line, &y, 0, fh, 0);
	
	sprintf(line, "%d - %5.2f / GAME", data->info.pts_allowed, data->info.pts_allowed / (float)total_games);
	print_stat("POINTS ALLOWED", line, &y, gap, fh, 1);
	
	if (data->info.num_2pts_attempted == 0)
		perc = 0.0f;
	else
		perc = (data->info.num_2pts_made / (float)data->info.num_2pts_attempted) * 100.0f;
	sprintf(line, "%d / %d - %5.2f %%", data->info.num_2pts_made, data->info.num_2pts_attempted, perc);
	print_stat("2 POINT SHOTS", line, &y, gap, fh, 2);
	
	if (data->info.num_3pts_attempted == 0)
		perc = 0.0f;
	else
		perc = (data->info.num_3pts_made / (float)data->info.num_3pts_attempted) * 100.0f;
	sprintf(line, "%d / %d - %5.2f %%", data->info.num_3pts_made, data->info.num_3pts_attempted, perc);
	print_stat("3 POINT SHOTS", line, &y, gap, fh, 3);
	
	sprintf(line, "%d", data->info.num_dunk_made);
	print_stat("DUNKS", line, &y, gap, fh, 4);
	
	flash_stat(&save_y, 0, fh, 0, data->info.off_rank);
	flash_stat(&save_y, gap, fh, 1, data->info.def_rank);
	flash_stat(&save_y, gap, fh, 2, data->info.twop_rank);
	flash_stat(&save_y, gap, fh, 3, data->info.threep_rank);
	flash_stat(&save_y, gap, fh, 4, data->info.dunk_rank);
	data->auto_select_timer = AUTO_SELECT_TIME_6;
}	/* page2 */

static void page3(view_stats_data_t *data, int y, int fh, int gap, int total_games)
{
	char line[256];
	int save_y;
	
	save_y = y;
	data->page_number = 3;
	sprintf(line, "%d - %5.2f / GAME", data->info.num_rebounds, data->info.num_rebounds / (float)total_games);
	print_stat("TOTAL REBOUNDS", line, &y, 0, fh, 0);
	
	sprintf(line, "%d - %5.2f / GAME", data->info.num_assists, data->info.num_assists / (float)total_games);
	print_stat("TOTAL ASSISTS", line, &y, gap, fh, 1);
	
	sprintf(line, "%d - %5.2f / GAME", data->info.num_steals, data->info.num_steals / (float)total_games);
	print_stat("TOTAL STEALS", line, &y, gap, fh, 2);
	
	sprintf(line, "%d - %5.2f / GAME", data->info.num_blocks, data->info.num_blocks / (float)total_games);
	print_stat("TOTAL BLOCKS", line, &y, gap, fh, 3);
	
	sprintf(line, "%d - %5.2f / GAME", data->info.num_turnovers, data->info.num_turnovers / (float)total_games);
	print_stat("TURNOVERS", line, &y, gap, fh, 4);
	
	flash_stat(&save_y, 0, fh, 0, data->info.rebound_rank);
	flash_stat(&save_y, gap, fh, 1, data->info.assist_rank);
	flash_stat(&save_y, gap, fh, 2, data->info.steal_rank);
	flash_stat(&save_y, gap, fh, 3, data->info.block_rank);
	flash_stat(&save_y, gap, fh, 4, data->info.turnover_rank);
	data->auto_select_timer = AUTO_SELECT_TIME_6;
}	/* page3 */

static void page4(view_stats_data_t *data, int y, int fh, int gap)
{
	char line[256];
	int save_y;
	
	y = SPRITE_VRES - 190;
	save_y = y;
	data->page_number = 4;
	sprintf(line, "%d", data->info.trivia_pts);
	print_stat("TRIVIA POINTS", line, &y, 0, fh, 0);
	
	sprintf(line, "%d", data->info.unused_attribute_pts);
	print_stat("ATTRIBUTE POINTS", line, &y, gap, fh, 1);
	
	sprintf(line, "%d", data->info.unused_privilege_pts);
	print_stat("PRIVILEGES LEFT", line, &y, gap, fh, 2);
	
	if (data->info.defeated_count == 0)
		strcpy(line, "NONE");
	else if (data->info.defeated_count == 29)
		strcpy(line, "ALL 29 TEAMS");
	else
		sprintf(line, "%d / 29 TEAMS", data->info.defeated_count);
	print_stat("TEAMS DEFEATED", line, &y, gap, fh, 3);
	
	flash_stat(&save_y, 0, fh, 0, data->info.trivia_rank);
	flash_stat(&save_y, gap, fh, 1, 0);
	flash_stat(&save_y, gap, fh, 2, 0);
	flash_stat(&save_y, gap, fh, 3, 0);
	data->auto_select_timer = AUTO_SELECT_TIME_6;
}	/* page4 */

static int show_view_stat(view_stats_data_t *data)
{
	int done, total_games, fh, y, gap;
	int i, left, start, num, font_id;
	float perc;
	
	hide_creatp_shadow();
	for (i = 0; i < NUM_VIEW_STAT_BAR; i++) {
		hide_sprite(view_stats_bar[i].bar.sprite);
		hide_sprite(view_stats_bar[i].top1.sprite);
		hide_sprite(view_stats_bar[i].top5.sprite);
		hide_sprite(view_stats_bar[i].top10.sprite);
	}
	font_id = is_low_res ? FONTID_BAST8TA : FONTID_NBA10;
	set_text_font(font_id);
	set_string_id(STATS_SID);
	set_text_justification_mode(HJUST_CENTER | VJUST_BOTTOM);
	set_text_color(WHITE);
	set_text_z_inc(-0.10F);
	fh = get_font_height(font_id) + 3;
	gap = get_font_height(FONTID_NBA10) + 15;
		
	y = SPRITE_VRES - 154;
	total_games = data->info.game_win + data->info.game_loss;
	done = FALSE;
	if (total_games == 0) {
		show_creatp_shadow(SPRITE_VRES - 200, 0, 2, 0, 1);
		set_text_position(250, SPRITE_VRES - 230, INSTRUCTION_Z);
		oprintf("NO RECORD");
		data->did_first_flash = TRUE;
		done = TRUE;
	} else {
		hide_view_stat();
		if (data->page_number == 0) {
			page1(data, y, fh, gap, total_games);
		} else if (data->page_number == 1) {
			page2(data, y, fh, gap, total_games);
		} else if (data->page_number == 2) {
			page3(data, y, fh, gap, total_games);
		} else if (data->page_number == 3) {
			page4(data, y, fh, gap);
		} else if ((data->page_number == 4) || (data->page_number == 41)) {
			if ((data->info.defeated_count == 0) || (data->info.defeated_count == 29))
				done = TRUE;
			else {
				show_creatp_shadow(SPRITE_VRES - 142, 0, 10, 0, 1);
				y = SPRITE_VRES - 167;
				if (is_low_res)
					fh += 2;
				else
					fh -= 2;
				set_text_position(250, y, INSTRUCTION_Z);
				oprintf("\e%dc""DEFEATED TEAMS", LT_YELLOW);
				y -= gap;
				left = TRUE;
				start = (data->page_number == 41) ? data->last_index : 0;
				data->page_number = 5;
				num = 0;
				for (i = start; i < NUM_TEAMS; i++) {
					if ((data->info.teams_defeated & (1 << i)) != 0) {
						if (++num > 24) {
							data->last_index = i;
							data->page_number = 41;
							break;
						}
						if (left) {
							set_text_justification_mode(HJUST_LEFT);
							set_text_position(155, y, INSTRUCTION_Z);
							oprintf("\e%dc""%s", WHITE, teaminfo[i].szname);
							left = FALSE;
						} else {
							set_text_justification_mode(HJUST_RIGHT);
							set_text_position(338, y, INSTRUCTION_Z);
							oprintf("\e%dc""%s", WHITE, teaminfo[i].szname);
							y -= fh;
							left = TRUE;
						}
					}
				}
			}
		} else if ((data->page_number == 5) || (data->page_number == 51)) {
			show_creatp_shadow(SPRITE_VRES - 142, 0, 10, 0, 1);
			y = SPRITE_VRES - 167;
			if (is_low_res)
				fh += 2;
			else
				fh -= 2;
			set_text_position(250, y, INSTRUCTION_Z);
			oprintf("\e%dc""REMAINING TEAMS", LT_YELLOW);
			y -= gap;
			left = TRUE;
			start = (data->page_number == 51) ? data->last_index : 0;
			data->page_number = 6;
			num = 0;
			for (i = start; i < NUM_TEAMS; i++) {
				if ((data->info.teams_defeated & (1 << i)) == 0) {
					if (++num > 24) {
						data->last_index = i;
						data->page_number = 51;
						break;
					}
					if (left) {
						set_text_justification_mode(HJUST_LEFT);
						set_text_position(155, y, INSTRUCTION_Z);
						oprintf("\e%dc""%s", WHITE, teaminfo[i].szname);
						left = FALSE;
					} else {
						set_text_justification_mode(HJUST_RIGHT);
						set_text_position(338, y, INSTRUCTION_Z);
						oprintf("\e%dc""%s", WHITE, teaminfo[i].szname);
						y -= fh;
						left = TRUE;
					}
				}
			}
		} else
			done = TRUE;
	}
	return done;
}	/* show_view_stat */

static void hide_view_stat(void)
{
	delete_multiple_strings(STATS_SID, 0xFFFFFFFF);
}	/* hide_view_stat */

static void get_stat_info(plyr_record_t *rec, stat_info *info)
{
	int i;
	
	get_all_records();
	qsort(rec_sort_ram, NUM_PLYR_RECORDS, sizeof(plyr_record_t), (qsort_proc)sort_wins);
	info->overall_rank = find_record_in_ram(rec->plyr_name);
	info->game_win = rec->games_won;
	info->game_loss = rec->games_played - rec->games_won;
	qsort(rec_sort_ram, NUM_PLYR_RECORDS, sizeof(plyr_record_t), (qsort_proc)sort_points_scored);
	info->off_rank = find_record_in_ram(rec->plyr_name);
	qsort(rec_sort_ram, NUM_PLYR_RECORDS, sizeof(plyr_record_t), (qsort_proc)sort_points_allowed);
	info->def_rank = find_record_in_ram(rec->plyr_name);
	info->winstreak = rec->winstreak;
	qsort(rec_sort_ram, NUM_PLYR_RECORDS, sizeof(plyr_record_t), (qsort_proc)sort_win_streaks);
	info->winstreak_rank = find_record_in_ram(rec->plyr_name);
	info->pts_scored = rec->points_scored;
	info->pts_allowed = rec->points_allowed;
	info->num_2pts_made = rec->two_ptrs_made;
	info->num_2pts_attempted = rec->two_ptrs_attempted;
	qsort(rec_sort_ram, NUM_PLYR_RECORDS, sizeof(plyr_record_t), (qsort_proc)sort_2pt_percent);
	info->twop_rank = find_record_in_ram(rec->plyr_name);
	info->num_3pts_made = rec->three_ptrs_made;
	info->num_3pts_attempted = rec->three_ptrs_attempted;
	qsort(rec_sort_ram, NUM_PLYR_RECORDS, sizeof(plyr_record_t), (qsort_proc)sort_3pt_percent);
	info->threep_rank = find_record_in_ram(rec->plyr_name);
	info->num_dunk_made = rec->dunks_made;
	info->num_dunk_attempted = rec->dunks_attempted;
	qsort(rec_sort_ram, NUM_PLYR_RECORDS, sizeof(plyr_record_t), (qsort_proc)sort_dunk_percent);
	info->dunk_rank = find_record_in_ram(rec->plyr_name);
	info->num_rebounds = rec->rebounds;
	qsort(rec_sort_ram, NUM_PLYR_RECORDS, sizeof(plyr_record_t), (qsort_proc)sort_rebounds);
	info->rebound_rank = find_record_in_ram(rec->plyr_name);
	info->num_assists = rec->assists;
	qsort(rec_sort_ram, NUM_PLYR_RECORDS, sizeof(plyr_record_t), (qsort_proc)sort_assists);
	info->assist_rank = find_record_in_ram(rec->plyr_name);
	info->num_steals = rec->steals;
	qsort(rec_sort_ram, NUM_PLYR_RECORDS, sizeof(plyr_record_t), (qsort_proc)sort_steals);
	info->steal_rank = find_record_in_ram(rec->plyr_name);
	info->num_blocks = rec->blocks;
	qsort(rec_sort_ram, NUM_PLYR_RECORDS, sizeof(plyr_record_t), (qsort_proc)sort_blocks);
	info->block_rank = find_record_in_ram(rec->plyr_name);
	info->num_turnovers = rec->turnovers;
	qsort(rec_sort_ram, NUM_PLYR_RECORDS, sizeof(plyr_record_t), (qsort_proc)sort_turnovers);
	info->turnover_rank = find_record_in_ram(rec->plyr_name);
	info->trivia_pts = rec->trivia_pts;
	qsort(rec_sort_ram, NUM_PLYR_RECORDS, sizeof(plyr_record_t), (qsort_proc)sort_trivia);
	info->trivia_rank = find_record_in_ram(rec->plyr_name);
	info->unused_attribute_pts = rec->unused_attribute_points;
	info->unused_privilege_pts = rec->unused_privilege_points;
	info->teams_defeated = rec->teams_defeated;
	info->defeated_count = 0;
	for (i = 0; i < NUM_TEAMS; i++)
		if (((1 << i) & rec->teams_defeated) != 0)
			info->defeated_count++;
	free(rec_sort_ram);
	rec_sort_ram = NULL;
}	/* get_stat_info */

static int cmp_winstreak(int a, int b)
{
	if ((a < 0) && (b < 0)) {
		a = -a;
		b = -b;
		return b - a;
	} else if ((a >= 0) && (b >= 0))
		return a - b;
	else if (a < 0)
		return -1;
	else
		return +1;
}	/* cmp_winstreak */

static int sort_2pt_percent(plyr_record_t *rec1, plyr_record_t *rec2)
{
	int tmp;
	
	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;
	
	tmp = (int)(rec2->two_ptrs_made - rec1->two_ptrs_made);
	if (!tmp) {
		tmp = (int)(rec2->games_won - rec1->games_won);
		if (!tmp)
			return cmp_winstreak(rec2->winstreak, rec1->winstreak);
	}
	return (tmp);
}	/* sort_2pt_percent */

static int sort_rebounds(plyr_record_t *rec1, plyr_record_t *rec2)
{
	int tmp;
	
	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;
	
	tmp = (int)(rec2->rebounds - rec1->rebounds);
	if (!tmp) {
		tmp = (int)(rec2->games_won - rec1->games_won);
		if (!tmp)
			return cmp_winstreak(rec2->winstreak, rec1->winstreak);
	}
	return (tmp);
}	/* sort_rebounds */

static int sort_assists(plyr_record_t *rec1, plyr_record_t *rec2)
{
	int tmp;
	
	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;
	
	tmp = (int)(rec2->assists - rec1->assists);
	if (!tmp) {
		tmp = (int)(rec2->games_won - rec1->games_won);
		if (!tmp)
			return cmp_winstreak(rec2->winstreak, rec1->winstreak);
	}
	return (tmp);
}	/* sort_assists */

static int sort_steals(plyr_record_t *rec1, plyr_record_t *rec2)
{
	int tmp;
	
	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;
	
	tmp = (int)(rec2->steals - rec1->steals);
	if (!tmp) {
		tmp = (int)(rec2->games_won - rec1->games_won);
		if (!tmp)
			return cmp_winstreak(rec2->winstreak, rec1->winstreak);
	}
	return (tmp);
}	/* sort_steals */

static int sort_blocks(plyr_record_t *rec1, plyr_record_t *rec2)
{
	int tmp;
	
	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;
	
	tmp = (int)(rec2->blocks - rec1->blocks);
	if (!tmp) {
		tmp = (int)(rec2->games_won - rec1->games_won);
		if (!tmp)
			return cmp_winstreak(rec2->winstreak, rec1->winstreak);
	}
	return (tmp);
}	/* sort_blocks */

static int sort_turnovers(plyr_record_t *rec1, plyr_record_t *rec2)
{
	int tmp;
	
	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;
	
	tmp = (int)(rec1->turnovers - rec2->turnovers);
	if (!tmp) {
		tmp = (int)(rec2->games_won - rec1->games_won);
		if (!tmp)
			return cmp_winstreak(rec2->winstreak, rec1->winstreak);
	}
	return (tmp);
}	/* sort_turnovers */

/*
 *	head mode functions
 */

static void *head_init(void)
{
	head_data_t *data;
	int i, left, right;
	
	/* allocate the private data */
	data = malloc(sizeof(head_data_t));
	if (data != NULL) {
		/* get the current selected head */
		data->head_list_index = player_record_info_to_head_list_index(&creatp_player_data.cmos_data);
		data->arrow_timer = 0;
		data->lit_arrow = NULL;		
		
		image_to_sprite(&head_button_list.button, BUTTON_MIDDLE_Z, I2S_ORIG, I2S_DELTA, 0, HEAD_MODE_Y_OFFSET, FALSE);
		head_button_list.button.sprite->id = GLOW_HEAD;
		do_glow_sparkle(head_button_list.button.sprite);
		image_to_sprite(&head_button_list.title, BUTTON_TITLE_Z, I2S_ORIG, I2S_DELTA, 0, HEAD_MODE_Y_OFFSET, FALSE);
		image_to_sprite(&head_button_list.left_arrow, BUTTON_BOTTOM_Z, I2S_ORIG, I2S_DELTA, 0, HEAD_MODE_Y_OFFSET, TRUE);
		image_to_sprite(&head_button_list.right_arrow, BUTTON_BOTTOM_Z, I2S_ORIG, I2S_DELTA, 0, HEAD_MODE_Y_OFFSET, TRUE);
		image_to_sprite(&head_additional[0], BUTTON_MIDDLE_Z, I2S_ORIG, I2S_DELTA, 0, HEAD_MODE_Y_OFFSET, FALSE);
		image_to_sprite(&head_additional[1], BUTTON_BOTTOM_Z, I2S_ORIG, I2S_DELTA, 0, HEAD_MODE_Y_OFFSET, FALSE);
		
		image_to_sprite(&horse_mugshot, 2000.0F, I2S_ORIG, I2S_ORIG, 0, 0, TRUE);
		for (i = 0; i < ARRAY_SIZE(creatp_head_list); i++) {
			image_to_sprite(&creatp_head_list[i].mugshot, 2000.0F, I2S_ORIG, I2S_ORIG, 0, 0, TRUE);
			if (creatp_head_list[i].mugshot.image == &cp_pinto) {
				pinto_sprite = creatp_head_list[i].mugshot.sprite;
				if (creatp_player_data.cmos_data.player_index == HORSE_CHARACTER)
					creatp_head_list[i].mugshot.sprite = horse_mugshot.sprite;
			}
		}
		
		/* init the set of three heads for choosing */
		select_head_button(data->head_list_index);
#ifdef SHOW_ALL_HEADS
		data->head_list_index = 0;
		data->head_count = 0;
		data->head_name = NULL;
		/* include the secret team */
		for (i = 0; i <= NUM_TEAMS; i++)
			data->head_count += teaminfo[i].num_player;
		{
			int team, player_index;
			
			data->head_roster_index = 0;
			for (team = 0; team <= NUM_TEAMS; team++)
				for (player_index = 0; player_index < teaminfo[team].num_player; player_index++)
					if ((creatp_player_data.cmos_data.team == team) &&
						(creatp_player_data.cmos_data.player_index == player_index))
						break;
					else
						data->head_roster_index++;
		}
#endif
	}
	return data;
}	/* head_init */

static void head_exit(void *data)
{
	head_data_t *head_data;
	int i;
	
	head_data = (head_data_t *)data;
#ifdef SHOW_ALL_HEADS
	delete_multiple_strings(0xABCDE, 0xFFFFFFFF);
	if (head_data->head_name != NULL)
		delobj(head_data->head_name);
#endif
	/* delete the button sprites */
	free_image_to_sprite(&head_button_list.button);
	free_image_to_sprite(&head_button_list.title);
	free_image_to_sprite(&head_button_list.left_arrow);
	free_image_to_sprite(&head_button_list.right_arrow);
	
	free_image_to_sprite(&head_additional[0]);
	free_image_to_sprite(&head_additional[1]);
	
	free_image_to_sprite(&horse_mugshot);
	for (i = 0; i < ARRAY_SIZE(creatp_head_list); i++) {
		if (creatp_head_list[i].mugshot.image == &cp_pinto)
			creatp_head_list[i].mugshot.sprite = pinto_sprite;
		free_image_to_sprite(&creatp_head_list[i].mugshot);
	}
	
	/* free the private data */
	free(data);
}	/* head_exit */

static int head_loop(int input, void *data)
{
	head_data_t *head_data;
	int i, j, done, left, right;
	
	head_data = data;
	done = FALSE;
	if ((head_data->lit_arrow != NULL) && (--head_data->arrow_timer == 0)) {
		hide_sprite(head_data->lit_arrow);
		head_data->lit_arrow = NULL;
	}
	if ((input == P_LEFT) || (input == P_RIGHT)) {
		/* issue a sound call */
		snd_scall_bank(creatp_snd_bank, input == P_LEFT ? CREATP_DOWN : CREATP_UP, VOLUME4, 127, PRIORITY_LVL3);
		if (head_data->lit_arrow != NULL)
			hide_sprite(head_data->lit_arrow);
		head_data->arrow_timer = REPEAT_DELAY;
		
#ifdef SHOW_ALL_HEADS
		if (input == P_LEFT) {
			head_data->lit_arrow = head_button_list.left_arrow.sprite;
			head_data->head_roster_index = (head_data->head_roster_index == 0) ? head_data->head_count - 1 : head_data->head_roster_index - 1;
		} else {
			head_data->lit_arrow = head_button_list.right_arrow.sprite;
			head_data->head_roster_index = (head_data->head_roster_index == head_data->head_count - 1) ? 0 : head_data->head_roster_index + 1;
		}

		{
			int index, team, player_index;
			struct stat_plyr_data *player;
			
			index = 0;
			player_index = 0;
			player = NULL;
			for (team = 0; (team <= NUM_TEAMS) && (index != -1); team++)
				for (player_index = 0; player_index < teaminfo[team].num_player; player_index++)
					if (index == head_data->head_roster_index) {
						player = (struct stat_plyr_data *)&teaminfo[team].p_spd[player_index];
						creatp_player_data.cmos_data.team = team;
						creatp_player_data.cmos_data.player_index = player_index;
						index = -1;
						break;
					} else
						index++;
			/* print out the players name */
			delete_multiple_strings(0xABCDE, 0xFFFFFFFF);
			set_text_font(FONTID_BAST8TO);
			set_string_id(0xABCDE);
			set_text_justification_mode(HJUST_CENTER);
			set_text_color(WHITE);
			set_text_position(240, SPRITE_VRES - 300, INSTRUCTION_Z);
			if (head_data->head_name != NULL)
				delobj(head_data->head_name);
			if (player != NULL) {
				oprintf("%s %s", player->szfirst, player->szlast);
				head_data->head_name = beginobj(player->pii_name, 240.0f, SPRITE_VRES - 280, 1.0F, 0);
			}
		}
		
		/* force the mugshot to be updated */
		free_image_to_sprite(&creatp_head_list[0].mugshot);
		load_guest_mug(&creatp_player_data.cmos_data);
		image_to_sprite(&creatp_head_list[0].mugshot, 2000.0F, I2S_ORIG, I2S_ORIG, 0, 0, TRUE);
#else
		/* find the head to the left and right */
		head_list_left_right(head_data->head_list_index, &left, &right);
		
		if (input == P_LEFT) {
			head_data->lit_arrow = head_button_list.left_arrow.sprite;
			head_data->head_list_index = left;
		} else {
			head_data->lit_arrow = head_button_list.right_arrow.sprite;
			head_data->head_list_index = right;
		}
		
		/* get the new player info */
		head_list_index_to_info(head_data->head_list_index, &creatp_player_data.cmos_data);
#endif
		/* update the mugshots */
		select_head_button(head_data->head_list_index);

		/* light up the proper arrow */
		unhide_sprite(head_data->lit_arrow);
		
		/* update the model and textures */
		update_player_display();

	} else if ((input == P_UP) || (input == P_DOWN)) {
		int currently_female, female, cheerleader;
		
		if (creatp_player_data.cmos_data.team == TEAM_SECRET) {
			/* toggle between horse and pinto */
			if ((creatp_player_data.cmos_data.player_index == HORSE_CHARACTER) ||
				(creatp_player_data.cmos_data.player_index == PINTO_CHARACTER)) {
				hide_sprite(creatp_head_list[head_data->head_list_index].mugshot.sprite);
				if (creatp_player_data.cmos_data.player_index == PINTO_CHARACTER) {
					creatp_player_data.cmos_data.player_index = HORSE_CHARACTER;
					creatp_head_list[head_data->head_list_index].mugshot.sprite = horse_mugshot.sprite;
				} else {
					creatp_player_data.cmos_data.player_index = PINTO_CHARACTER;
					creatp_head_list[head_data->head_list_index].mugshot.sprite = pinto_sprite;
				}
				snd_scall_bank(creatp_snd_bank, input == P_DOWN ? CREATP_DOWN : CREATP_UP, VOLUME4, 127, PRIORITY_LVL3);
				select_head_button(head_data->head_list_index);
				update_player_display();
			} else {
				/* toggle between female and cheerleaders */
				if ((creatp_player_data.cmos_data.player_index == LIA_CHARACTER) ||
					(creatp_player_data.cmos_data.player_index == CHEERLEADER_LM_CHARACTER)) {
					currently_female = creatp_player_data.cmos_data.player_index == LIA_CHARACTER;
					female = LIA_CHARACTER;
					cheerleader = CHEERLEADER_LM_CHARACTER;
				} else if ((creatp_player_data.cmos_data.player_index == KERRI_CHARACTER) ||
						   (creatp_player_data.cmos_data.player_index == CHEERLEADER_KH_CHARACTER)) {
					currently_female = creatp_player_data.cmos_data.player_index == KERRI_CHARACTER;
					female = KERRI_CHARACTER;
					cheerleader = CHEERLEADER_KH_CHARACTER;
				} else {
					currently_female = FALSE;
					female = -1;
					cheerleader = CHEERLEADER_KH_CHARACTER;
				}
				if (female != -1) {
					if (currently_female) {
						creatp_player_data.cmos_data.player_index = cheerleader;
						snd_scall_bank(cursor_bnk_str, 57, VOLUME4, 127, PRIORITY_LVL4);
					} else {
						creatp_player_data.cmos_data.player_index = female;
						snd_scall_bank(creatp_snd_bank, input == P_DOWN ? CREATP_DOWN : CREATP_UP, VOLUME4, 127, PRIORITY_LVL3);
					}
					update_player_display();
				}
			}
		}
	} else if ((input & P_ABC_MASK) != 0) {
		/* issue a sound call */
		snd_scall_bank(cursor_bnk_str, SELECT_SND1, VOLUME4, 127, PRIORITY_LVL4);
		flash_sprite(creatp_head_list[head_data->head_list_index].mugshot.sprite, WHITE, 4, 2);
		done = TRUE;
	}
	return done;
}	/* head_loop */

static int player_record_info_to_head_list_index(plyr_record_t *player_info)
{
	int i, pindex;
	
	if ((player_info->team == TEAM_SECRET) && (player_info->player_index == CHEERLEADER_LM_CHARACTER))
		pindex = LIA_CHARACTER;
	else if ((player_info->team == TEAM_SECRET) && (player_info->player_index == CHEERLEADER_KH_CHARACTER))
		pindex = KERRI_CHARACTER;
	else if ((player_info->team == TEAM_SECRET) && (player_info->player_index == HORSE_CHARACTER))
		pindex = PINTO_CHARACTER;
	else
		pindex = player_info->player_index;
	
	for (i = 0; i < CREATP_HEADS; i++) {
		if ((player_info->team == creatp_head_list[i].team)
			&& (pindex == creatp_head_list[i].player_index))
			return i;
	}
	return -1;
}	/* player_record_info_to_head_list_index */

static void head_list_index_to_info(int index, plyr_record_t *player_record)
{
	if ((index < 0) || (index >= CREATP_HEADS)) {
#ifdef DEBUG
		fprintf(stderr, "head_list_index_to_info bad index %d\n", index);
#endif
		index = 1;
	}
	player_record->team = creatp_head_list[index].team;
	player_record->player_index = creatp_head_list[index].player_index;
}	/* head_list_index_to_info */

static void head_list_left_right(int center, int *left, int *right)
{
	int first_slot, last_slot;
	
	first_slot = guest_present ? 0 : 1;
	last_slot = CREATP_HEADS - 1;
	*left = (center == first_slot) ? last_slot : center - 1;
	*right = (center == last_slot) ? first_slot : center + 1;
	while (!creatp_head_list[*left].availiable)
		*left = (*left == first_slot) ? last_slot : *left - 1;
	while (!creatp_head_list[*right].availiable)
		*right = (*right == last_slot) ? first_slot : *right + 1;
}	/* head_list_left_right */

static void init_head_list(void)
{
	int i, j, k;
	
#if 0
	for (i = 0; i < NUM_TEAMS; i++) {
		for (j = 0; j < teaminfo[i].num_player; j++) {
			if (teaminfo[i].p_spd[j].number == 0)
				fprintf(stderr, "%3.3s 00 %s %s\n", teaminfo[i].szprfx,
						teaminfo[i].p_spd[j].szfirst, teaminfo[i].p_spd[j].szlast);
			else
				fprintf(stderr, "%3.3s %2x %s %s\n", teaminfo[i].szprfx, teaminfo[i].p_spd[j].number,
						teaminfo[i].p_spd[j].szfirst, teaminfo[i].p_spd[j].szlast);
		}
	}
#endif
	/* reset the guest slot */
	guest_present = FALSE;
	creatp_head_list[0].team = -1;
	creatp_head_list[0].player_index = -1;
	creatp_head_list[0].availiable = TRUE;
	creatp_head_list[0].mugshot.image = &cp_quest;
	creatp_head_list[0].mugshot.sprite = NULL;
	/* init the head list pointer into NBA database */
	for (i = 0; i < NUM_TEAMS; i++)
		for (j = 0; j < teaminfo[i].num_player; j++)
			for (k = 0; k < 10; k++) {
				if (teaminfo[i].p_spd[j].pii_mug == nba_player_list[k]) {
					creatp_head_list[k + 1].team = i;
					creatp_head_list[k + 1].player_index = j;
					break;
				}
			}
	
	/* refind Pippen because the teamdatabase is littered with temp Pippens */
	for (j = 0; j < teaminfo[TEAM_HOU].num_player; j++)
		if (teaminfo[TEAM_HOU].p_spd[j].pii_mug == nba_player_list[0]) {
			creatp_head_list[1].team = TEAM_HOU;
			creatp_head_list[1].player_index = j;
			break;
		}
	
#ifdef DEBUG
	for (i = 1; i < 11; i++) {
		if (creatp_head_list[i].team == -1) {
			fprintf(stderr, "Could not find nba player %d in teamdata.database\n", i);
			lockup();
		}
	}
#endif
}	/* init_head_list */

#define UNLOCK_FILE "unlock.fmt"
static void unlock_head_list()
{
	FILE *fp;
	int won_boost, play_boost;
	int up_time_boost, play_time_boost;
	int i, unlock_level, index, up_time, play_time;
	
	unlock_level = RLEVEL_1;
	/* unlock one every 3 games won */
	won_boost = creatp_player_data.cmos_data.games_won / 3;
	/* unlock one every 10 games played */
	play_boost = creatp_player_data.cmos_data.games_played / 10;
	unlock_level += MAX(won_boost, play_boost);
	
	if (get_audit(UPTIME_AUDIT_NUM, &up_time))
		up_time = 0;
	if (get_audit(PLAYTIME_AUD, &play_time))
		play_time = 0;
	/* unlock one every 2 weeks, assuming a 14 hr uptime per day */
	up_time_boost = up_time / (2 * 7 * 14 * 60 * 60);
	/* unlock one every 2 weeks, assuming 5 hr play per day */
	play_time_boost = play_time / (2 * 5 * 14 * 60 * 60);
	i = MAX(up_time_boost, play_time_boost);
	
	/* read the last up_time/play_time boost from the file(in case CMOS gets reset) */
	fp = fopen(UNLOCK_FILE, "rb");
	if (fp == NULL)
		index = 0;
	else {
		if (fread(&index, sizeof(index), 1, fp) != 1)
			index = 0;
		else {
#ifdef DEBUG
			fprintf(stderr, "%d read from UNLOCK.FMT\n", index);
#endif
		}
		fclose(fp);
	}
	if (index < 0)
		index = 0;
	if (index > 17)
		index = 17;
	if ((fp == NULL) || (index < i)) {
		index = i;
		fp = fopen(UNLOCK_FILE, "wb");
		if (fp != NULL) {
			fwrite(&index, sizeof(index), 1, fp);
			fclose(fp);
		}
	}
	set_audit(UNLOCK_LEVEL_AUD, index);
	
	unlock_level += index;
#ifdef UNLOCK_ALL_CHARACTERS
	unlock_level = RLEVEL_20;
#endif
	if (master_key == ARRAY_SIZE(lock_pick_table))
		unlock_level = RLEVEL_20;
	for (i = 0; i < CREATP_HEADS; i++)
		creatp_head_list[i].availiable = unlock_level >= creatp_head_list[i].release_level;
	/* the current selected head is always unlocked */
	index = player_record_info_to_head_list_index(&creatp_player_data.cmos_data);
	if (index != -1)
		creatp_head_list[index].availiable = TRUE;
} /* unlock_head_list */

static void select_head_button(int center)
{
	int left, right;
	sprite_info_t *left_mug, *center_mug, *right_mug;
	int i;
	
	head_list_left_right(center, &left, &right);
	for (i = 0; i < CREATP_HEADS; i++)
		hide_sprite(creatp_head_list[i].mugshot.sprite);
	
	left_mug = creatp_head_list[left].mugshot.sprite;
	left_mug->h_scale = left_mug->w_scale = 0.59f;
	set_sprite_xyz(left_mug, 145, SPRITE_VRES - 162 + HEAD_MODE_Y_OFFSET, BUTTON_MIDDLE_Z - 1.0F);
	unhide_sprite(left_mug);
	
	center_mug = creatp_head_list[center].mugshot.sprite;
	center_mug->h_scale = center_mug->w_scale = 1.0f;
	set_sprite_xyz(center_mug, 190, SPRITE_VRES - 106 + HEAD_MODE_Y_OFFSET, BUTTON_BOTTOM_Z - 1.0F);
	unhide_sprite(center_mug);
	
	right_mug = creatp_head_list[right].mugshot.sprite;
	right_mug->h_scale = right_mug->w_scale = 0.59f;
	set_sprite_xyz(right_mug, 283, SPRITE_VRES - 162 + HEAD_MODE_Y_OFFSET, BUTTON_MIDDLE_Z - 1.0F);
	unhide_sprite(right_mug);
}	/* select_head_button */

/*
 *	uniform number mode functions
 */

static void *uniform_number_init(void)
{
	uniform_number_data_t *data;
	int i;
	
	/* allocate the private data */
	data = malloc(sizeof(uniform_number_data_t));
	if (data != NULL) {
		data->mode = ENTERING_NUMBER1;
		data->cursor_pos = UNIFORM_NUMBER_0;
		for (i = 0; i < 2; i++)
			data->number[i] = ' ';
		
		/* create the image sprites */
		image_to_sprite(&uniform_number_sprite_list[UNIFORM_NUMBER_BACK_SHADOW], BUTTON_TOP_Z, I2S_ORIG, I2S_DELTA, 0, UNIFORM_NUM_Y_OFFSET, FALSE);
		image_to_sprite(&uniform_number_sprite_list[UNIFORM_NUMBER_BACK_DROP], BUTTON_TOP_Z + 1.0F, I2S_ORIG, I2S_DELTA, 0, UNIFORM_NUM_Y_OFFSET, FALSE);
		image_to_sprite(&uniform_number_sprite_list[UNIFORM_NUMBER_CRESCENT], BUTTON_MIDDLE_Z, I2S_ORIG, I2S_DELTA, 0, UNIFORM_NUM_Y_OFFSET, FALSE);
		image_to_sprite(&uniform_number_sprite_list[UNIFORM_NUMBER_CURSOR], BUTTON_TITLE_Z, I2S_ORIG, I2S_DELTA, 0, UNIFORM_NUM_Y_OFFSET, FALSE);
		uniform_number_sprite_list[UNIFORM_NUMBER_CURSOR].sprite->id = GLOW_CURSOR;
		for (i = UNIFORM_NUMBER_0; i <= UNIFORM_NUMBER_9; i++)
			image_to_sprite(&uniform_number_sprite_list[i], BUTTON_EXTRA_Z, I2S_ORIG, I2S_DELTA, 0, UNIFORM_NUM_Y_OFFSET, FALSE);
		image_to_sprite(&uniform_number_sprite_list[UNIFORM_NUMBER_BACK], BUTTON_EXTRA_Z, I2S_ABS, I2S_ABS, 229, SPRITE_VRES - 227 + UNIFORM_NUM_Y_OFFSET, FALSE);
		image_to_sprite(&uniform_number_sprite_list[UNIFORM_NUMBER_END], BUTTON_EXTRA_Z, I2S_ABS, I2S_ABS, 254, SPRITE_VRES - 230 + UNIFORM_NUM_Y_OFFSET, FALSE);

		image_to_sprite(&uniform_number_sprite_list[UENTER_DASH3], BUTTON_EXTRA_Z, I2S_ABS, I2S_ABS, 225, SPRITE_VRES - 312 + UNIFORM_NUM_Y_OFFSET, FALSE);
		image_to_sprite(&uniform_number_sprite_list[UENTER_DASH4], BUTTON_EXTRA_Z, I2S_ABS, I2S_ABS, 247, SPRITE_VRES - 312 + UNIFORM_NUM_Y_OFFSET, FALSE);

		image_to_sprite(&uniform_number_sprite_list[UENTER_LETTER3], BUTTON_EXTRA_Z, I2S_ORIG, I2S_ORIG, 0, 0, TRUE);
		image_to_sprite(&uniform_number_sprite_list[UENTER_LETTER4], BUTTON_EXTRA_Z, I2S_ORIG, I2S_ORIG, 0, 0, TRUE);
		
		/* set the cursor position */
		set_uniform_number_cursor(data->cursor_pos);
		do_glow_sparkle(uniform_number_sprite_list[UNIFORM_NUMBER_CURSOR].sprite);
	}
	return data;
}	/* uniform_number_init */

static void uniform_number_exit(void *data)
{
	uniform_number_data_t *uniform_number_data;
	int i;
	
	uniform_number_data = (uniform_number_data_t *)data;
	if ((uniform_number_data->number[0] == ' ') && (uniform_number_data->number[1] == ' '))
		creatp_player_data.cmos_data.jersey_num = 0;
	else if ((uniform_number_data->number[0] != ' ') && (uniform_number_data->number[1] != ' ')) {
		uniform_number_data->number[0] -= '0';
		uniform_number_data->number[1] -= '0';
		creatp_player_data.cmos_data.jersey_num = (uniform_number_data->number[0] << 4) | uniform_number_data->number[1];
	} else
		creatp_player_data.cmos_data.jersey_num = uniform_number_data->number[0] - '0';
	free_image_to_sprite(&uniform_number_sprite_list[UNIFORM_NUMBER_BACK_SHADOW]);
	free_image_to_sprite(&uniform_number_sprite_list[UNIFORM_NUMBER_BACK_DROP]);
	free_image_to_sprite(&uniform_number_sprite_list[UNIFORM_NUMBER_CRESCENT]);
	free_image_to_sprite(&uniform_number_sprite_list[UNIFORM_NUMBER_CURSOR]);
	for (i = UNIFORM_NUMBER_0; i <= UNIFORM_NUMBER_9; i++)
		free_image_to_sprite(&uniform_number_sprite_list[i]);
	free_image_to_sprite(&uniform_number_sprite_list[UNIFORM_NUMBER_BACK]);
	free_image_to_sprite(&uniform_number_sprite_list[UNIFORM_NUMBER_END]);
	
	free_image_to_sprite(&uniform_number_sprite_list[UENTER_DASH3]);
	free_image_to_sprite(&uniform_number_sprite_list[UENTER_DASH4]);
	
	free_image_to_sprite(&uniform_number_sprite_list[UENTER_LETTER3]);
	free_image_to_sprite(&uniform_number_sprite_list[UENTER_LETTER4]);
	/* free the private data */
	free(data);
}	/* uniform_number_exit */

static int uniform_number_loop(int input, void *data)
{
	uniform_number_data_t *uniform_number_data;
	int i, done;
	char *ch;
	
	uniform_number_data = data;
	done = FALSE;
	move_uniform_number_cursor(input, uniform_number_data);
	if ((input & P_ABC_MASK) != 0) {
		/* number select sound call */
		snd_scall_bank(cursor_bnk_str, SELECT_SND1, VOLUME4, 127, PRIORITY_LVL4);
		if (uniform_number_data->cursor_pos == UNIFORM_NUMBER_BACK) {
			if (uniform_number_data->mode != ENTERING_NUMBER1)
				uniform_number_data->mode--;
			return done;
		}
		if (uniform_number_data->cursor_pos == UNIFORM_NUMBER_END) {
			for (i = uniform_number_data->mode - ENTERING_NUMBER1; i < 2; i++)
				uniform_number_data->number[i] = ' ';
			uniform_number_data->mode = ENTERING_NUMBER2;
		} else {
			ch = &uniform_number_data->number[uniform_number_data->mode - ENTERING_NUMBER1];
			*ch = uniform_number_data->cursor_pos - UNIFORM_NUMBER_0 + '0';
		}
		
		if (uniform_number_data->mode == ENTERING_NUMBER2)
			done = TRUE;
		else
			uniform_number_data->mode++;
		
		do_top_sparkle(uniform_number_sprite_list[3].sprite, 100, 100, TRUE, FALSE);
		do_oval_sparkle(uniform_number_sprite_list[3].sprite, 31, 46, TRUE, FALSE, done);
	}
	return done;
}	/* uniform_number_loop */

static void move_uniform_number_cursor(int input, uniform_number_data_t *uniform_number_data)
{
	int base;
	
	/* calculate the new cursor position */
	base = uniform_number_data->cursor_pos - UNIFORM_NUMBER_0;
	if ((input & P_UP) != 0)
		base = ((base >= 0) && (base <= 2)) ? base + 9 : base - 3;
	if ((input & P_DOWN) != 0)
		base = ((base >= 9) && (base <= 11)) ? base - 9 : base + 3;
	if ((input & P_LEFT) != 0)
		base = (base % 3) == 0 ? base + 2 : base - 1;
	if ((input & P_RIGHT) != 0)
		base = ((base - 2) % 3) == 0 ? base - 2 : base + 1;
	base += UNIFORM_NUMBER_0;
	/* move the cursor sprite */
	if (uniform_number_data->cursor_pos != base) {
		/* cursor move sound call */
		snd_scall_bank(plyr_bnk_str, CURSOR_SND1, VOLUME4, 127, PRIORITY_LVL3);
		uniform_number_data->cursor_pos = base;
		set_uniform_number_cursor(uniform_number_data->cursor_pos);
	}
	/* update the dash position to the letter */
	update_uniform_number_dash_letter(uniform_number_data);
}	/* move_uniform_number_cursor */

static void set_uniform_number_cursor(int cursor_pos)
{
	sprite_info_t *sprite;
	int cursor_delta_x, cursor_delta_y;
	
	find_sprite_center(uniform_number_sprite_list[cursor_pos].sprite, &glow_cursor_x_pos, &glow_cursor_y_pos);
	glow_cursor_y_pos--;
	sprite = uniform_number_sprite_list[UNIFORM_NUMBER_CURSOR].sprite;
	if ((cursor_pos == UNIFORM_NUMBER_1) || (cursor_pos == UNIFORM_NUMBER_4))
		cursor_delta_x = (sprite->ii->w / 2) + 1;
	else
		cursor_delta_x = (sprite->ii->w / 2) + 2;
	cursor_delta_y = sprite->ii->h / 2;
	
	glow_cursor_x_pos = glow_cursor_x_pos - cursor_delta_x;
	glow_cursor_y_pos = SPRITE_VRES - (glow_cursor_y_pos - cursor_delta_y);
	set_sprite_pos(sprite, glow_cursor_x_pos, glow_cursor_y_pos);
}	/* set_uniform_number_cursor */

static void update_uniform_number_dash_letter(uniform_number_data_t *uniform_number_data)
{
	sprite_info_t *dash_sprite, *letter_sprite;
	image_info_t *src_letter;
	int cursor_pos;
	int dash_sprite_index, letter_sprite_index;
	int x_pos, y_pos;
	int delta_x, delta_y;
	
	cursor_pos = uniform_number_data->cursor_pos;
	dash_sprite_index = uniform_number_data->mode - ENTERING_NUMBER1 + UENTER_DASH3;
	letter_sprite_index = uniform_number_data->mode - ENTERING_NUMBER1 + UENTER_LETTER3;
	src_letter = uniform_number_sprite_list[cursor_pos].image;
	dash_sprite = uniform_number_sprite_list[dash_sprite_index].sprite;
	letter_sprite = uniform_number_sprite_list[letter_sprite_index].sprite;
	if ((cursor_pos == UNIFORM_NUMBER_END) || (cursor_pos == UNIFORM_NUMBER_BACK)) {
		/* hide the letter character */
		hide_sprite(letter_sprite);
		/* show the dash character */
		unhide_sprite(dash_sprite);
	} else {
		/* hide the dash character */
		hide_sprite(dash_sprite);
		/* show the letter character and update to the correct sprite */
		letter_sprite->ii = src_letter;
		letter_sprite->tn = find_texture(src_letter->texture_name, CREATP_TID);
		find_sprite_center(dash_sprite, &x_pos, &y_pos);
		delta_x = src_letter->w / 2;
		delta_y = src_letter->h / 2;
		set_sprite_pos(letter_sprite, x_pos - delta_x, SPRITE_VRES - (y_pos - delta_y));
		unhide_sprite(letter_sprite);
	}
}	/* update_uniform_number_dash_letter */

/*
 *	attributes mode functions
 */

static void *attributes_init(void)
{
	attributes_data_t *data;
	int i, y_pos, save_low_res;
	float z_inc;
	
	/* allocate the private data */
	data = malloc(sizeof(attributes_data_t));
	if (data != NULL) {
		data->current_button = ATTRIBUTES_HEIGHT_BUTTON;
		
		data->button_state[ATTRIBUTES_HEIGHT_BUTTON] = creatp_player_data.cmos_data.height;
		data->button_state[ATTRIBUTES_WEIGHT_BUTTON] = creatp_player_data.cmos_data.weight;
		data->button_state[ATTRIBUTES_POWER_BUTTON] = creatp_player_data.cmos_data.power;
		data->button_state[ATTRIBUTES_SPEED_BUTTON] = creatp_player_data.cmos_data.speed;
		data->button_state[ATTRIBUTES_TWOP_BUTTON] = creatp_player_data.cmos_data.twop;
		data->button_state[ATTRIBUTES_THREEP_BUTTON] = creatp_player_data.cmos_data.threep;
		data->button_state[ATTRIBUTES_STEAL_BUTTON] = creatp_player_data.cmos_data.steal;
		data->button_state[ATTRIBUTES_BLOCK_BUTTON] = creatp_player_data.cmos_data.block;
		data->button_state[ATTRIBUTES_DUNKS_BUTTON] = creatp_player_data.cmos_data.dunks;
		data->button_state[ATTRIBUTES_DRIBBLE_BUTTON] = creatp_player_data.cmos_data.dribble;
		
		/* build the button sprites */
		y_pos = SPRITE_VRES - 138;
		z_inc = 0.0F;
		for (i = ATTRIBUTES_HEIGHT_BUTTON; i < ATTRIBUTES_BUTTON_COUNT; i++) {
			image_to_sprite(&attributes_button_list[i].unselected_button, BUTTON_MIDDLE_Z + z_inc, I2S_ORIG, I2S_ABS, 0, y_pos, FALSE);
			image_to_sprite(&attributes_button_list[i].selected_button, BUTTON_MIDDLE_Z + z_inc, I2S_ORIG, I2S_ABS, 0, y_pos, TRUE);
			attributes_button_list[i].selected_button.sprite->id = (i << 16) | GLOW_ATTR;

			image_to_sprite(&attributes_button_list[i].back_frame, BUTTON_TOP_Z + z_inc, I2S_ORIG, I2S_ABS, 0, y_pos - 3, FALSE);

			image_to_sprite(&attributes_button_list[i].tick_mark, BUTTON_BOTTOM_Z + z_inc, I2S_ORIG, I2S_ORIG, 0, 0, TRUE);
			save_low_res = is_low_res;
			if (is_low_res)
				is_low_res = 3;
			image_to_sprite(&attributes_button_list[i].title, BUTTON_TITLE_Z + z_inc, I2S_ORIG, I2S_ORIG, 0, 0, FALSE);
			is_low_res = save_low_res;
			
			y_pos += ATTR_BUTTON_DELTA;
			z_inc -= 0.1F;
		}
		
		for (i = 0; i < ATTRIBUTES_BUTTON_COUNT; i++)
			update_attributes_value(i, data->button_state[i]);
		
		/* select the initial button */
		select_attributes_button(data->current_button);
		
		/* show the attribute point message */
		attributes_update_point_message(data, WHITE);
		flasher_proc = qcreate("flashA", MESSAGE_PID, proc_flash_text, 0, 0, 0, 0);
	}
	return data;
}	/* attributes_init */

static void attributes_exit(void *data)
{
	attributes_data_t *attributes_data;
	int i;
	
	attributes_data = (attributes_data_t *)data;
	
	/* stop the text flashing */
	if (flasher_proc != NULL) {
		kill(flasher_proc, 0);
		flasher_proc = NULL;
	}
	
	/* delete the attribute point message */
	delete_multiple_strings(ATTR_SID, 0xFFFFFFFF);
	
	/* delete the button sprites */
	for (i = ATTRIBUTES_HEIGHT_BUTTON; i < ATTRIBUTES_BUTTON_COUNT; i++) {
		free_image_to_sprite(&attributes_button_list[i].unselected_button);
		free_image_to_sprite(&attributes_button_list[i].selected_button);
		free_image_to_sprite(&attributes_button_list[i].back_frame);
		free_image_to_sprite(&attributes_button_list[i].tick_mark);
		free_image_to_sprite(&attributes_button_list[i].title);
	}
	
	/* reset all of the mini attributes to white */
	mini_attribute_reset_labels();
	
	/* free the private data */
	free(data);
}	/* attributes_exit */

static int attributes_loop(int input, void *data)
{
	attributes_data_t *attributes_data;
	int current_button, value, done, allowed;
	
	attributes_data = data;
	done = FALSE;
	if ((input == P_LEFT) || (input == P_RIGHT)) {
		current_button = attributes_data->current_button;
		value = attributes_data->button_state[current_button];
		if (input == P_LEFT) {
			if (value > 1) {
				value--;
				if (creatp_player_data.cmos_data.unused_attribute_points < 250)
					creatp_player_data.cmos_data.unused_attribute_points++;
				allowed = TRUE;
			} else
				allowed = FALSE;
		} else {
			if (value < 20) {
				if (creatp_player_data.cmos_data.unused_attribute_points > 0) {
					value++;
					creatp_player_data.cmos_data.unused_attribute_points--;
					allowed = TRUE;
				} else
					allowed = FALSE;
			} else
				allowed = FALSE;
		}
		if (allowed) {
			snd_scall_bank(creatp_snd_bank, CREATP_ATTR1 + value - 1, VOLUME4, 127, PRIORITY_LVL3);
			
			switch (current_button) {
			case ATTRIBUTES_HEIGHT_BUTTON:
				creatp_player_data.cmos_data.height = value;
				break;
			case ATTRIBUTES_WEIGHT_BUTTON:
				creatp_player_data.cmos_data.weight = value;
				break;
			case ATTRIBUTES_POWER_BUTTON:
				creatp_player_data.cmos_data.power = value;
				break;
			case ATTRIBUTES_SPEED_BUTTON:
				creatp_player_data.cmos_data.speed = value;
				break;
			case ATTRIBUTES_TWOP_BUTTON:
				creatp_player_data.cmos_data.twop = value;
				break;
			case ATTRIBUTES_THREEP_BUTTON:
				creatp_player_data.cmos_data.threep = value;
				break;
			case ATTRIBUTES_STEAL_BUTTON:
				creatp_player_data.cmos_data.steal = value;
				break;
			case ATTRIBUTES_BLOCK_BUTTON:
				creatp_player_data.cmos_data.block = value;
				break;
			case ATTRIBUTES_DUNKS_BUTTON:
				creatp_player_data.cmos_data.dunks = value;
				break;
			case ATTRIBUTES_DRIBBLE_BUTTON:
				creatp_player_data.cmos_data.dribble = value;
				break;
			}
			attributes_data->button_state[current_button] = value;
			update_attributes_value(current_button, value);
			mini_attribute_update(1 << (current_button + 1), current_button);
			
			/* update the attribute point message */
			attributes_update_point_message(data, flasher_str->state.color);
		} else
			snd_scall_bank(creatp_snd_bank, CREATP_CANNOT_SELECT, VOLUME4, 127, PRIORITY_LVL4);
	} else if (next_button(ATTRIBUTES_HEIGHT_BUTTON, ATTRIBUTES_DRIBBLE_BUTTON, &attributes_data->current_button, input, TRUE)) {
		/* issue a sound call */
		snd_scall_bank(creatp_snd_bank, input == P_DOWN ? CREATP_DOWN : CREATP_UP, VOLUME4, 127, PRIORITY_LVL3);
		select_attributes_button(attributes_data->current_button);
	} else if ((input & P_ABC_MASK) != 0) {
		int i;
		
		/* issue a sound call */
		snd_scall_bank(cursor_bnk_str, SELECT_SND1, VOLUME4, 127, PRIORITY_LVL4);
		done = TRUE;
	}
	return done;
}	/* attributes_loop */

static void select_attributes_button(int button)
{
	int i;
	
	for (i = 0; i < ATTRIBUTES_BUTTON_COUNT; i++) {
		hide_sprite(attributes_button_list[i].selected_button.sprite);
		unhide_sprite(attributes_button_list[i].unselected_button.sprite);
		set_sprite_color(attributes_button_list[i].title.sprite, is_low_res ? LOW_RES_GRAY_COLOR : GRAY_COLOR);
	}
	unhide_sprite(attributes_button_list[button].selected_button.sprite);
	hide_sprite(attributes_button_list[button].unselected_button.sprite);
	set_sprite_color(attributes_button_list[button].title.sprite, OFF_YELLOW_COLOR);
	do_glow_sparkle(attributes_button_list[button].selected_button.sprite);
	
	mini_attribute_reset_labels();
	/* color the selected mini attribute label */
	if (button == ATTRIBUTES_HEIGHT_BUTTON)
		for (i = 22; i <= 25; i++)
			set_sprite_color(mini_attribute_list[i].sprite, OFF_YELLOW_COLOR);
	else if (button == ATTRIBUTES_WEIGHT_BUTTON)
		for (i = 26; i <= 29; i++)
			set_sprite_color(mini_attribute_list[i].sprite, OFF_YELLOW_COLOR);
	else
		set_sprite_color(mini_attribute_list[button - 2].sprite, OFF_YELLOW_COLOR);
}	/* select_attributes_button */

static void update_attributes_value(int button, int value)
{
	int y_pos;
	sprite_info_t *sprite;
	
	if (value > 20)
		value = 20;
	if (value < 1)
		hide_sprite(attributes_button_list[button].tick_mark.sprite);
	else {
		value--;
		y_pos = SPRITE_VRES - 144;
		y_pos -= (button - ATTRIBUTES_HEIGHT_BUTTON) * 23;
		sprite = attributes_button_list[button].tick_mark.sprite;
		sprite->ii = attributes_tick_list[value];
		sprite->tn = find_texture(attributes_tick_list[value]->texture_name, CREATP_TID);
		set_sprite_pos(sprite, 231, y_pos);
		unhide_sprite(sprite);
	}
}	/* update_attributes_value */

static void attributes_update_point_message(attributes_data_t *attributes_data, long start_color)
{
	int total, i;
	
	/* delete the old message */
	delete_multiple_strings(ATTR_SID, 0xFFFFFFFF);
	
	/* show the attribute point message */
	set_text_font(is_low_res ? FONTID_BAST8TA : FONTID_NBA10);
	set_string_id(ATTR_SID);
	set_text_justification_mode(HJUST_LEFT);
	set_text_color(start_color);
	set_text_position(180, SPRITE_VRES - (is_low_res ? 134 : 137), INSTRUCTION_Z);
	set_text_z_inc(-0.10F);
	
	for (i = 0, total = 0; i < 10; i++)
		total += attributes_data->button_state[i];
	total += creatp_player_data.cmos_data.unused_attribute_points;
	flasher_str = oprintf("TOTAL POINTS: %d/%d", creatp_player_data.cmos_data.unused_attribute_points, total);
	set_text_z_inc(0.0F);
}	/* attributes_update_point_message */

/*
 *	privileges mode functions
 */

static void *privileges_init(void)
{
	privileges_data_t *data;
	int i, y_pos;
	float z_inc;
	
	/* allocate the private data */
	data = malloc(sizeof(privileges_data_t));
	if (data != NULL) {
		data->current_button = PRIVILEGES_STEALTH_BUTTON;
		
		data->button_state[PRIVILEGES_STEALTH_BUTTON] = (creatp_player_data.cmos_data.privileges & PRIV_STEALTH_TURBO) != 0;
		data->button_state[PRIVILEGES_HEAD_BUTTON] = (creatp_player_data.cmos_data.privileges & PRIV_BIG_HEAD) != 0;
		data->button_state[PRIVILEGES_DHEAD_BUTTON] = (creatp_player_data.cmos_data.privileges & PRIV_DRONE_BIG_HEAD) != 0;
		data->button_state[PRIVILEGES_COURTSEL_BUTTON] = (creatp_player_data.cmos_data.privileges & PRIV_COURTSEL) != 0;
		data->button_state[PRIVILEGES_HOTSPOT_BUTTON] = (creatp_player_data.cmos_data.privileges & PRIV_HOTSPOT) != 0;
		data->button_state[PRIVILEGES_HIDEATTR_BUTTON] = (creatp_player_data.cmos_data.privileges & PRIV_HIDEATTR) != 0;
		data->button_state[PRIVILEGES_JERSEY_BUTTON] = (creatp_player_data.cmos_data.privileges & PRIV_JERSEY) != 0;
		
		/* build the button sprites */
		y_pos = SPRITE_VRES - 133 + PRIV_MODE_Y_OFFSET;
		z_inc = 0.0F;
		for (i = PRIVILEGES_STEALTH_BUTTON; i < PRIVILEGES_BUTTON_COUNT; i++) {
			image_to_sprite(&privileges_button_list[i].blue_box, BUTTON_TOP_Z + z_inc, I2S_ORIG, I2S_ABS, 0, y_pos, FALSE);
			image_to_sprite(&privileges_button_list[i].green_check_mark, BUTTON_TITLE_Z + z_inc, I2S_ORIG, I2S_ABS, 0, y_pos + 15, TRUE);
			image_to_sprite(&privileges_button_list[i].check_mark_shadow, BUTTON_TITLE_Z + z_inc, I2S_ORIG, I2S_ABS, 0, y_pos + 15, TRUE);
			image_to_sprite(&privileges_button_list[i].black_oval, BUTTON_MIDDLE_Z + z_inc, I2S_ORIG, I2S_ABS, 0, y_pos + 4, FALSE);
			image_to_sprite(&privileges_button_list[i].blue_oval, BUTTON_MIDDLE_Z + z_inc, I2S_ORIG, I2S_ABS, 0, y_pos + 4, TRUE);
			privileges_button_list[i].blue_oval.sprite->id = (i << 16) | GLOW_PRIV;
			image_to_sprite(&privileges_button_list[i].title, BUTTON_TITLE_Z + z_inc, I2S_ORIG, I2S_DELTA, 0, PRIV_MODE_Y_OFFSET, FALSE);
			y_pos += PRIV_BUTTON_DELTA;
			z_inc -= 0.1F;
		}
		
		privileges_additional_init(data);
	}
	return data;
}	/* privileges_init */

static void privileges_additional_init(privileges_data_t *data)
{
	int i;
	
	for (i = 0; i < PRIVILEGES_BUTTON_COUNT; i++)
		update_privileges_check(i, data->button_state[i]);
	
	/* select the initial button */
	color_privileges_title(data);
	select_privileges_button(data->current_button);
	
	/* show the check 2 of the 7 message */
	set_text_font(is_low_res ? FONTID_BAST8TA : FONTID_NBA10);
	set_string_id(PRIV_SID);
	set_text_justification_mode(HJUST_LEFT);
	set_text_color(WHITE);
	set_text_position(190, SPRITE_VRES - (is_low_res ? 150 : 152), INSTRUCTION_Z);
	set_text_z_inc(-0.10F);
	flasher_str = oprintf("CHECK 2 OF THE 7");
	set_text_z_inc(0.0F);
	flasher_proc = qcreate("flashP", MESSAGE_PID, proc_flash_text, 0, 0, 0, 0);
}	/* privileges_additional_init */

static void privileges_exit(void *data)
{
	privileges_data_t *privileges_data;
	int i;
	
	privileges_data = (privileges_data_t *)data;
	
	privileges_additional_exit();
	/* delete the button sprites */
	for (i = PRIVILEGES_STEALTH_BUTTON; i < PRIVILEGES_BUTTON_COUNT; i++) {
		free_image_to_sprite(&privileges_button_list[i].blue_box);
		free_image_to_sprite(&privileges_button_list[i].green_check_mark);
		free_image_to_sprite(&privileges_button_list[i].check_mark_shadow);
		free_image_to_sprite(&privileges_button_list[i].black_oval);
		free_image_to_sprite(&privileges_button_list[i].blue_oval);
		free_image_to_sprite(&privileges_button_list[i].title);
	}
	/* free the private data */
	free(data);
}	/* privileges_exit */

static void privileges_additional_exit(void)
{
	/* stop the text flashing */
	if (flasher_proc != NULL) {
		kill(flasher_proc, 0);
		flasher_proc = NULL;
	}
	
	/* delete the check 2 of 7 message */
	delete_multiple_strings(PRIV_SID, 0xFFFFFFFF);
}	/* privileges_additional_exit */

static int privileges_loop(int input, void *data)
{
	privileges_data_t *privileges_data;

	int current_button, done, allowed, i, push_list_index;
	sprite_info_t *push_list[FADE_LIST_SIZE];
	
	privileges_data = data;
	done = FALSE;
	if ((input == P_LEFT) || (input == P_RIGHT)) {
		current_button = privileges_data->current_button;

		if (privileges_data->button_state[current_button]) {
			/* this priv was set, clear it and get back the priv point */
			creatp_player_data.cmos_data.unused_privilege_points++;
			allowed = TRUE;
		} else {
			/* this priv was not set, check if there are any points, then set and take a priv point */
			if (creatp_player_data.cmos_data.unused_privilege_points > 0) {
				creatp_player_data.cmos_data.unused_privilege_points--;
				allowed = TRUE;
			} else
				allowed = FALSE;
		}
		if (allowed) {
			privileges_data->button_state[current_button] = !privileges_data->button_state[current_button];
			/* issue a sound call */
			snd_scall_bank(cursor_bnk_str, privileges_data->button_state[current_button] ? 12 : 9, VOLUME4, 127, PRIORITY_LVL4);
			
			switch (current_button) {
			case PRIVILEGES_STEALTH_BUTTON:
				creatp_player_data.cmos_data.privileges ^= PRIV_STEALTH_TURBO;
				break;
			case PRIVILEGES_HEAD_BUTTON:
				creatp_player_data.cmos_data.privileges ^= PRIV_BIG_HEAD;
				break;
			case PRIVILEGES_DHEAD_BUTTON:
				creatp_player_data.cmos_data.privileges ^= PRIV_DRONE_BIG_HEAD;
				break;
			case PRIVILEGES_COURTSEL_BUTTON:
				creatp_player_data.cmos_data.privileges ^= PRIV_COURTSEL;
				break;
			case PRIVILEGES_HOTSPOT_BUTTON:
				creatp_player_data.cmos_data.privileges ^= PRIV_HOTSPOT;
				if ((creatp_player_data.cmos_data.privileges & PRIV_HOTSPOT) != 0) {
					/* save the current sprite list */
					push_list_index = fade_sprite_list_index;
					for (i = 0; i < fade_sprite_list_index; i++)
						push_list[i] = fade_sprite_list_sprites[i];
					fade_sprite_list(FADE_DOWN, FALSE);
					privileges_hide();
					show_creatp_title(HOTSPOT_MODE);
					mode_loop(hotspot_init, hotspot_loop, hotspot_exit);
					show_creatp_title(CREATP_PRIVILEGES_BUTTON);
					privileges_unhide(privileges_data);
					/* restore the sprite list */
					fade_sprite_list_index = push_list_index;
					for (i = 0; i < fade_sprite_list_index; i++)
						fade_sprite_list_sprites[i] = push_list[i];
					/* slow fade to give the cursor sparkle a chance to fade away */
					fade_sprite_list(FADE_UP, TRUE);
				}
				break;
			case PRIVILEGES_HIDEATTR_BUTTON:
				creatp_player_data.cmos_data.privileges ^= PRIV_HIDEATTR;
				break;
			case PRIVILEGES_JERSEY_BUTTON:
				creatp_player_data.cmos_data.privileges ^= PRIV_JERSEY;
				if ((creatp_player_data.cmos_data.privileges & PRIV_JERSEY) != 0) {
					/* save the current sprite list */
					push_list_index = fade_sprite_list_index;
					for (i = 0; i < fade_sprite_list_index; i++)
						push_list[i] = fade_sprite_list_sprites[i];
					fade_sprite_list(FADE_DOWN, FALSE);
					privileges_hide();
					show_creatp_title(UNIFORM_JERSEY_MODE);
					namelist_mode = TEAM_LIST;
					/* set to the selected jersey */
					load_player_textures();
					mode_loop(namelist_init, namelist_loop, namelist_exit);
					show_creatp_title(CREATP_PRIVILEGES_BUTTON);
					privileges_unhide(privileges_data);
					/* restore the sprite list */
					fade_sprite_list_index = push_list_index;
					for (i = 0; i < fade_sprite_list_index; i++)
						fade_sprite_list_sprites[i] = push_list[i];
					/* slow fade to give the cursor sparkle a chance to fade away */
					fade_sprite_list(FADE_UP, TRUE);
				} else 
					load_player_textures();
				break;
			}
			update_privileges_check(current_button, privileges_data->button_state[current_button]);
		} else
			snd_scall_bank(creatp_snd_bank, CREATP_CANNOT_SELECT, VOLUME4, 127, PRIORITY_LVL4);
	} else if (next_button(PRIVILEGES_STEALTH_BUTTON, PRIVILEGES_JERSEY_BUTTON, &privileges_data->current_button, input, TRUE)) {
		/* issue a sound call */
		snd_scall_bank(creatp_snd_bank, input == P_DOWN ? CREATP_DOWN : CREATP_UP, VOLUME4, 127, PRIORITY_LVL3);
		color_privileges_title(privileges_data);
		select_privileges_button(privileges_data->current_button);
	} else if ((input & P_ABC_MASK) != 0) {
		/* issue a sound call */
		snd_scall_bank(cursor_bnk_str, SELECT_SND1, VOLUME4, 127, PRIORITY_LVL4);
		for (i = 0; i < PRIVILEGES_BUTTON_COUNT; i++)
			if (privileges_data->button_state[i])
				qcreate("flasher", SPRITE_FLASHER_PID, flash_sprite_proc,
						(int)privileges_button_list[i].green_check_mark.sprite, WHITE, 4, 2);
		while (existp(SPRITE_FLASHER_PID, 0xFFFFFFFF))
			sleep(1);
		done = TRUE;
	}	
	return done;
}	/* privileges_loop */

static void select_privileges_button(int button)
{
	int i;
	
	for (i = 0; i < PRIVILEGES_BUTTON_COUNT; i++) {
		hide_sprite(privileges_button_list[i].blue_oval.sprite);
		unhide_sprite(privileges_button_list[i].black_oval.sprite);
	}
	hide_sprite(privileges_button_list[button].black_oval.sprite);
	unhide_sprite(privileges_button_list[button].blue_oval.sprite);
	do_glow_sparkle(privileges_button_list[button].blue_oval.sprite);
}	/* select_privileges_button */

static void color_privileges_title(privileges_data_t *privileges_data)
{
	int i;
	long color;
	
	for (i = 0; i < PRIVILEGES_BUTTON_COUNT; i++) {
		if (i == privileges_data->current_button)
			color = OFF_YELLOW_COLOR;
		else {
			if (privileges_data->button_state[i])
				color = WHITE;
			else
				color = GRAY_COLOR;
		}
		set_sprite_color(privileges_button_list[i].title.sprite, color);
	}
}	/* color_privileges_title */

static void update_privileges_check(int button, int state)
{
	if (state) {
		unhide_sprite(privileges_button_list[button].green_check_mark.sprite);
		unhide_sprite(privileges_button_list[button].check_mark_shadow.sprite);
	} else {
		hide_sprite(privileges_button_list[button].green_check_mark.sprite);
		hide_sprite(privileges_button_list[button].check_mark_shadow.sprite);
	}
}	/* toggle_privileges_check */

static void privileges_hide(void)
{
	int i;
	
	/* hide all of the button elements */
	for (i = PRIVILEGES_STEALTH_BUTTON; i < PRIVILEGES_BUTTON_COUNT; i++) {
		hide_sprite(privileges_button_list[i].blue_box.sprite);
		hide_sprite(privileges_button_list[i].green_check_mark.sprite);
		hide_sprite(privileges_button_list[i].check_mark_shadow.sprite);
		hide_sprite(privileges_button_list[i].black_oval.sprite);
		hide_sprite(privileges_button_list[i].blue_oval.sprite);
		hide_sprite(privileges_button_list[i].title.sprite);
	}
	
	privileges_additional_exit();
}	/* privileges_hide */

static void privileges_unhide(privileges_data_t *data)
{
	int i;
	
	for (i = PRIVILEGES_STEALTH_BUTTON; i < PRIVILEGES_BUTTON_COUNT; i++) {
		unhide_sprite(privileges_button_list[i].blue_box.sprite);
		unhide_sprite(privileges_button_list[i].green_check_mark.sprite);
		unhide_sprite(privileges_button_list[i].check_mark_shadow.sprite);
		unhide_sprite(privileges_button_list[i].black_oval.sprite);
		unhide_sprite(privileges_button_list[i].blue_oval.sprite);
		unhide_sprite(privileges_button_list[i].title.sprite);
	}
	
	privileges_additional_init(data);
}	/* privileges_unhide */

/*
 *	hotspot functions
 */

static void *hotspot_init(void)
{
	hotspot_data_t *data;
	hotspot_data hs;
	int x, z;
	
	/* allocate the private data */
	data = malloc(sizeof(hotspot_data_t));
	if (data != NULL) {
		hs.data = creatp_player_data.cmos_data.hotspot;

		if (hs.packed_data.stamp != HOTSPOT_DATA_STAMP) {
			x = hotspot_xzs[hs.packed_data.index % NUM_HOTSPOTS][0];
			z = hotspot_xzs[hs.packed_data.index % NUM_HOTSPOTS][1];
		} else {
			x = hs.packed_data.x;
			z = hs.packed_data.z;
		}
		data->x_pos = HOTSPOT_LEFT + (x * (HOTSPOT_RIGHT - HOTSPOT_LEFT)) / 134.0F;
		data->z_pos = HOTSPOT_TOP + (z * (HOTSPOT_BOTTOM - HOTSPOT_TOP)) / 146.0F;
		/* build the button sprites */
		image_to_sprite(&hotspot_list[0], BUTTON_TOP_Z, I2S_ORIG, I2S_ORIG, 0, 0, FALSE);
		image_to_sprite(&hotspot_list[1], BUTTON_TITLE_Z, I2S_ORIG, I2S_ORIG, 0, 0, FALSE);
		hotspot_list[1].sprite->id = GLOW_HOTSPOT;
		do_glow_sparkle(hotspot_list[1].sprite);
		
		hotspot_set_cursor(data);
	}
	return data;
}	/* hotspot_init */

static void hotspot_exit(void *data)
{
	hotspot_data_t *hdata;
	hotspot_data hs;
	int final_x, final_z;
	
	hdata = (hotspot_data_t *)data;
	
	hs.data = creatp_player_data.cmos_data.hotspot;			/* get the index */
	hs.packed_data.stamp = HOTSPOT_DATA_STAMP;
	hs.packed_data.x = (134.0F * (hdata->x_pos - HOTSPOT_LEFT)) / (HOTSPOT_RIGHT - HOTSPOT_LEFT);
	hs.packed_data.z = (146.0F * (hdata->z_pos - HOTSPOT_TOP)) / (HOTSPOT_BOTTOM - HOTSPOT_TOP);
	creatp_player_data.cmos_data.hotspot = hs.data;

	/* delete the button sprites */
	free_image_to_sprite(&hotspot_list[0]);
	free_image_to_sprite(&hotspot_list[1]);
	/* free the private data */
	free(data);
}	/* hotspot_exit */

static int hotspot_loop(int input, void *data)
{
	static int last_dir = 0;
	static int delta = 1;
	hotspot_data_t *hotspot_data;
	int done;
	
	hotspot_data = data;

	if (last_dir == input) {
		delta++;
		if (delta > 10)
			delta = 10;
	} else
		delta = 1;
	last_dir = input;
	
	if ((input & P_ABC_MASK) != 0) {
		done = TRUE;
		snd_scall_bank(cursor_bnk_str, SELECT_SND1, VOLUME4, 127, PRIORITY_LVL4);
		do_top_sparkle(hotspot_list[1].sprite, 80, 80, TRUE, FALSE);
		do_oval_sparkle(hotspot_list[1].sprite, 25, 37, TRUE, FALSE, done);
	} else {
		done = FALSE;
		if (input == P_LEFT)
			hotspot_data->x_pos -= delta;
		else if (input == P_RIGHT)
			hotspot_data->x_pos += delta;
		else if (input == P_UP)
			hotspot_data->z_pos -= delta;
		else if (input == P_DOWN)
			hotspot_data->z_pos += delta;
		hotspot_set_cursor(hotspot_data);
	}
	
	return done;
}	/* hotspot_loop */

static void hotspot_set_cursor(hotspot_data_t *data)
{
	/* pin the cursor to the legal range */
	if (data->x_pos < HOTSPOT_LEFT)
		data->x_pos = HOTSPOT_LEFT;
	if (data->x_pos > HOTSPOT_RIGHT)
		data->x_pos = HOTSPOT_RIGHT;
	if (data->z_pos < HOTSPOT_TOP)
		data->z_pos = HOTSPOT_TOP;
	if (data->z_pos > HOTSPOT_BOTTOM)
		data->z_pos = HOTSPOT_BOTTOM;
	
	glow_cursor_x_pos = data->x_pos;
	glow_cursor_y_pos = SPRITE_VRES - data->z_pos;
	
	set_sprite_pos(hotspot_list[1].sprite, glow_cursor_x_pos, glow_cursor_y_pos);
}	/* hotspot_set_cursor */

/*
 *	nickname mode functions
 */

static void *namelist_init(void)
{
	namelist_data_t *data;
	int i;
	
	/* allocate the private data */
	data = malloc(sizeof(namelist_data_t));
	if (data != NULL) {
		data->arrow_timer = 0;
		data->selected_arrow = -1;
		data->last_sound_call = 0;
		data->sound_call_timer = 0;
		
		/* build the button sprites */
		image_to_sprite(&namelist_sprite_list.up_arrow, BUTTON_BOTTOM_Z, I2S_ORIG, I2S_DELTA, 0, NAMELIST_MODE_Y_OFFSET, FALSE);
		image_to_sprite(&namelist_sprite_list.selected_up_arrow, BUTTON_BOTTOM_Z, I2S_ORIG, I2S_DELTA, 0, NAMELIST_MODE_Y_OFFSET, TRUE);
		image_to_sprite(&namelist_sprite_list.down_arrow, BUTTON_BOTTOM_Z, I2S_ORIG, I2S_DELTA, 0, NAMELIST_MODE_Y_OFFSET, FALSE);
		image_to_sprite(&namelist_sprite_list.selected_down_arrow, BUTTON_BOTTOM_Z, I2S_ORIG, I2S_DELTA, 0, NAMELIST_MODE_Y_OFFSET, TRUE);
		
		image_to_sprite(&namelist_sprite_list.disc_piece, BUTTON_MIDDLE_Z, I2S_ORIG, I2S_DELTA, 0, NAMELIST_MODE_Y_OFFSET, FALSE);
		namelist_sprite_list.disc_piece.sprite->id = GLOW_NAMELIST;
		do_glow_sparkle(namelist_sprite_list.disc_piece.sprite);
		image_to_sprite(&namelist_sprite_list.middle_piece, BUTTON_TOP_Z - 1.0F, I2S_ORIG, I2S_DELTA, 0, NAMELIST_MODE_Y_OFFSET, FALSE);
		image_to_sprite(&namelist_sprite_list.frame_piece, BUTTON_TOP_Z, I2S_ORIG, I2S_DELTA, 0, NAMELIST_MODE_Y_OFFSET, FALSE);
		if (namelist_mode == TEAM_LIST) {
			data->current_list_index = creatp_player_data.cmos_data.jersey_city;
			image_to_sprite(&namelist_sprite_list.team_text, BUTTON_BOTTOM_Z, I2S_ORIG, I2S_DELTA, 0, NAMELIST_MODE_Y_OFFSET, FALSE);
			data->list_base = team_list_sprites;
			data->list_size = ARRAY_SIZE(team_list_sprites);
		} else {
			/* check for the keenan name hack */
			if (creatp_player_data.cmos_data.nickname >= ARRAY_SIZE(name_list))
				data->current_list_index = 0;
			else
				data->current_list_index = creatp_player_data.cmos_data.nickname;
			image_to_sprite(&namelist_sprite_list.name_text, BUTTON_BOTTOM_Z, I2S_ORIG, I2S_DELTA, 0, NAMELIST_MODE_Y_OFFSET, FALSE);
			data->list_base = name_list;
			data->list_size = ARRAY_SIZE(name_list);
		}
		
		/* init the list to scroll */
		for (i = 0; i < data->list_size; i++)
			image_to_sprite(&data->list_base[i], BUTTON_TITLE_Z, I2S_ORIG, I2S_ORIG, 0, 0, TRUE);
		
		/* select the initial button */
		update_namelist(data);
	}
	return data;
}	/* namelist_init */

static void namelist_exit(void *data)
{
	namelist_data_t *namelist_data;
	int i;
	
	namelist_data = (namelist_data_t *)data;
	
	/* free the scroll data */
	for (i = 0; i < namelist_data->list_size; i++)
		free_image_to_sprite(&namelist_data->list_base[i]);
	
	/* delete the button sprites */
	free_image_to_sprite(&namelist_sprite_list.up_arrow);
	free_image_to_sprite(&namelist_sprite_list.selected_up_arrow);
	free_image_to_sprite(&namelist_sprite_list.down_arrow);
	free_image_to_sprite(&namelist_sprite_list.selected_down_arrow);
	free_image_to_sprite(&namelist_sprite_list.disc_piece);
	free_image_to_sprite(&namelist_sprite_list.middle_piece);
	free_image_to_sprite(&namelist_sprite_list.frame_piece);
	free_image_to_sprite(&namelist_sprite_list.team_text);
	free_image_to_sprite(&namelist_sprite_list.name_text);
	/* free the private data */
	free(data);
}	/* namelist_exit */

static int namelist_loop(int input, void *data)
{
	namelist_data_t *namelist_data;
	int done;
	
	namelist_data = data;
	done = FALSE;
	if ((namelist_data->selected_arrow != -1) && (--namelist_data->arrow_timer == 0)) {
		if (namelist_data->selected_arrow == P_UP) {
			hide_sprite(namelist_sprite_list.selected_up_arrow.sprite);
			unhide_sprite(namelist_sprite_list.up_arrow.sprite);
		} else {
			hide_sprite(namelist_sprite_list.selected_down_arrow.sprite);
			unhide_sprite(namelist_sprite_list.down_arrow.sprite);
		}
		namelist_data->selected_arrow = -1;
	}
	if ((input == P_UP) || (input == P_DOWN)) {
		/* issue a sound call */
		snd_scall_bank(creatp_snd_bank, input == P_DOWN ? CREATP_DOWN : CREATP_UP, VOLUME4, 127, PRIORITY_LVL3);
		
		if (namelist_data->selected_arrow != -1) {
			if (namelist_data->selected_arrow == P_UP) {
				hide_sprite(namelist_sprite_list.selected_up_arrow.sprite);
				unhide_sprite(namelist_sprite_list.up_arrow.sprite);
			} else {
				hide_sprite(namelist_sprite_list.selected_down_arrow.sprite);
				unhide_sprite(namelist_sprite_list.down_arrow.sprite);
			}
		}
		
		namelist_data->arrow_timer = REPEAT_DELAY;
		namelist_data->selected_arrow = input;
		if (input == P_UP) {
			hide_sprite(namelist_sprite_list.up_arrow.sprite);
			unhide_sprite(namelist_sprite_list.selected_up_arrow.sprite);
			namelist_data->current_list_index--;
		} else {
			hide_sprite(namelist_sprite_list.down_arrow.sprite);
			unhide_sprite(namelist_sprite_list.selected_down_arrow.sprite);
			namelist_data->current_list_index++;
		}
		
		namelist_data->current_list_index = item_to_index(namelist_data, namelist_data->current_list_index);
		
		update_namelist(namelist_data);
		
		/* save the selected item */
		if (namelist_mode == TEAM_LIST) {
			creatp_player_data.cmos_data.jersey_city = namelist_data->current_list_index;
			creatp_player_data.cmos_data.jersey_style = AWAY_JERSEY;
			/* update the player uniform */
			load_player_textures();
		} else
			creatp_player_data.cmos_data.nickname = namelist_data->current_list_index;
	} else if ((input == P_LEFT) || (input == P_RIGHT)) {
		if (namelist_mode == TEAM_LIST) {
			int has_alt;
			
			switch ((int)creatp_player_data.cmos_data.jersey_city) {
			case TEAM_CHI:
			case TEAM_MIA:
			case TEAM_MIL:
			case TEAM_MIN:
			case TEAM_ORL:
			case TEAM_PHO:
			case TEAM_SAC:
			case TEAM_VAN:
				has_alt = TRUE;
				break;
			default:
				has_alt = FALSE;
				break;
			}
			/* issue a sound call */
			snd_scall_bank(creatp_snd_bank, input == P_LEFT ? CREATP_DOWN : CREATP_UP, VOLUME4, 127, PRIORITY_LVL3);
			/* cycle thru home/away/alterate style */
			if (input == P_LEFT)
				creatp_player_data.cmos_data.jersey_style--;
			else
				creatp_player_data.cmos_data.jersey_style++;
			if (creatp_player_data.cmos_data.jersey_city == CUSTOM_UNIFORM_TEAM) {
				if (creatp_player_data.cmos_data.jersey_style < BLUE_UCOLOR)
					creatp_player_data.cmos_data.jersey_style = CUSTOM_UCOLOR;
				else if (creatp_player_data.cmos_data.jersey_style > CUSTOM_UCOLOR)
					creatp_player_data.cmos_data.jersey_style = BLUE_UCOLOR;
			} else {
				if (creatp_player_data.cmos_data.jersey_style < AWAY_JERSEY)
					creatp_player_data.cmos_data.jersey_style = has_alt ? ALTERNATE_JERSEY : HOME_JERSEY;
				else if ((creatp_player_data.cmos_data.jersey_style > ALTERNATE_JERSEY) || ((creatp_player_data.cmos_data.jersey_style == ALTERNATE_JERSEY) && !has_alt))
					creatp_player_data.cmos_data.jersey_style = AWAY_JERSEY;
			}
			/* update the player uniform */
			load_player_textures();
		} else
			/* generate the sound call of the selected name */
			if (creatp_player_data.cmos_data.nickname != 0) {
				if ((namelist_data->last_sound_call != creatp_player_data.cmos_data.nickname) ||
					(pcount > namelist_data->sound_call_timer)) {
					snd_scall_bank(creatp_name_bank, creatp_player_data.cmos_data.nickname, VOLUME4, 127, PRIORITY_LVL4);
					namelist_data->last_sound_call = creatp_player_data.cmos_data.nickname;
					namelist_data->sound_call_timer = pcount + name_list_bank_delay[creatp_player_data.cmos_data.nickname];
				}
			}
	} else if ((input & P_ABC_MASK) != 0) {
		if (namelist_mode == TEAM_LIST) {
			/* load the selected textures */
			load_player_textures();
			/* issue a sound call */
			snd_scall_bank(cursor_bnk_str, SELECT_SND1, VOLUME4, 127, PRIORITY_LVL4);
		} else {
			/* generate the sound call of the selected name */
			if (creatp_player_data.cmos_data.nickname == 0)
				snd_scall_bank(cursor_bnk_str, SELECT_SND1, VOLUME4, 127, PRIORITY_LVL4);
			else
				snd_scall_bank(creatp_name_bank, creatp_player_data.cmos_data.nickname, VOLUME4, 127, PRIORITY_LVL4);
		}
		done = TRUE;
	}
	
	return done;
}	/* namelist_loop */
	
/* the current list item is always positioned at 212, 227 */
static void update_namelist(namelist_data_t *data)
{
	sprite_info_t *sprite;
	int i, y_pos, count, y_base;
	
	if (data->list_size > 0) {
		/* find how many to display on either side */
		count = (data->list_size - 1) / 2;
		if (count > 4)
			count = 4;
		
		for (i = 0; i < data->list_size; i++)
			hide_sprite(data->list_base[i].sprite);
		
		y_base = (namelist_mode == NAME_LIST) ? 228 : 227;
		y_base += 12;
		
		/* position the selected line */
		sprite = data->list_base[data->current_list_index].sprite;
		y_pos = y_base;
		set_sprite_pos(sprite, 212, SPRITE_VRES - y_pos);
		unhide_sprite(sprite);
		
		/* position after */
		y_pos = y_base + 6;
		for (i = 1; i <= count; i++) {
			sprite = data->list_base[item_to_index(data, data->current_list_index + i)].sprite;
			y_pos += 20;
			set_sprite_pos(sprite, 212, SPRITE_VRES - y_pos);
			unhide_sprite(sprite);
		}
		
		/* position before */
		y_pos = y_base - 6;
		for (i = 1; i <= count; i++) {
			sprite = data->list_base[item_to_index(data, data->current_list_index - i)].sprite;
			y_pos -= 20;
			set_sprite_pos(sprite, 212, SPRITE_VRES - y_pos);
			unhide_sprite(sprite);
		}
	}
}	/* select_namelist_item */

static int item_to_index(namelist_data_t *data, int item)
{
	if (item < 0)
		return data->list_size + item;
	else if (item >= data->list_size)
		return item - data->list_size;
	else
		return item;
}	/* item_to_index */

/*
 *	edit name functions
 */

static void *edit_name_init(void)
{
	edit_name_data_t *data;
	int i, x_pos;
	
	/* allocate the private data */
	data = malloc(sizeof(edit_name_data_t));
	if (data != NULL) {
		data->mode = ENTERING_NAME1;
		data->cursor_pos = NAME_A;
		for (i = 0; i < LETTERS_IN_NAME; i++)
			data->name[i] = ' ';
		for (i = 0; i < PIN_NUMBERS; i++)
			data->pin[i] = ' ';
		
		/* create the image sprites */
		image_to_sprite(&edit_name_sprite_list[NAME_BACK_SHADOW], BUTTON_TOP_Z, I2S_ORIG, I2S_DELTA, 0, EDITNAME_MODE_Y_OFFSET, FALSE);
		image_to_sprite(&edit_name_sprite_list[NAME_BACK_DROP], BUTTON_TOP_Z + 1.0F, I2S_ORIG, I2S_DELTA, 0, EDITNAME_MODE_Y_OFFSET, FALSE);
		image_to_sprite(&edit_name_sprite_list[NAME_CRESCENT], BUTTON_MIDDLE_Z, I2S_ORIG, I2S_DELTA, 0, EDITNAME_MODE_Y_OFFSET, FALSE);
		image_to_sprite(&edit_name_sprite_list[NAME_CURSOR], BUTTON_TITLE_Z, I2S_ORIG, I2S_DELTA, 0, EDITNAME_MODE_Y_OFFSET, FALSE);
		edit_name_sprite_list[NAME_CURSOR].sprite->id = GLOW_CURSOR;
		for (i = NAME_A; i <= NAME_END; i++)
			image_to_sprite(&edit_name_sprite_list[i], BUTTON_BOTTOM_Z, I2S_ORIG, I2S_DELTA, 0, EDITNAME_MODE_Y_OFFSET, FALSE);
		for (i = PIN_0; i <= PIN_ASTRX; i++)
			image_to_sprite(&edit_name_sprite_list[i], BUTTON_EXTRA_Z, I2S_ORIG, I2S_DELTA, 0, EDITNAME_MODE_Y_OFFSET, TRUE);
		for (i = ENTER_DASH1; i <= ENTER_DASH6; i++)
			image_to_sprite(&edit_name_sprite_list[i], BUTTON_EXTRA_Z, I2S_ORIG, I2S_DELTA, 0, EDITNAME_MODE_Y_OFFSET, FALSE);
		for (i = ENTER_LETTER1; i <= ENTER_LETTER6; i++)
			image_to_sprite(&edit_name_sprite_list[i], BUTTON_BOTTOM_Z, I2S_ORIG, I2S_DELTA, 0, EDITNAME_MODE_Y_OFFSET, TRUE);
		
		/* line up the dashes */
		x_pos = 181;
		for (i = ENTER_DASH1; i <= ENTER_DASH6; i++){
			set_sprite_pos(edit_name_sprite_list[i].sprite, x_pos, SPRITE_VRES - 312 + EDITNAME_MODE_Y_OFFSET);
			x_pos += 22;
		}
		set_sprite_pos(edit_name_sprite_list[NAME_BACK].sprite, 226, SPRITE_VRES - 258 + EDITNAME_MODE_Y_OFFSET);
		set_sprite_pos(edit_name_sprite_list[NAME_SPACE].sprite, 254, SPRITE_VRES - 260 + EDITNAME_MODE_Y_OFFSET);
		set_sprite_pos(edit_name_sprite_list[PIN_SPACE].sprite, 231, SPRITE_VRES - 229 + EDITNAME_MODE_Y_OFFSET);
		set_sprite_pos(edit_name_sprite_list[PIN_BACK].sprite, 254, SPRITE_VRES - 228 + EDITNAME_MODE_Y_OFFSET);
		
		/* set the cursor position */
		set_edit_name_cursor(data->cursor_pos);
		do_glow_sparkle(edit_name_sprite_list[NAME_CURSOR].sprite);
	}
	
	/* remove the transition plates if just starting up */
	if (kill_transit) {
		kill_transit = FALSE;
		turn_transit_off(1);
	}
	return data;
}	/* edit_name_init */

static void edit_name_exit(void *data)
{
	edit_name_data_t *edit_name_data;
	int i, key_length;
	
	edit_name_data = (edit_name_data_t *)data;
	/* do not update the name if the timer expired while entering data */
	if (creatp_active) {
		for (i = 0; i < LETTERS_IN_NAME; i++)
			creatp_player_data.cmos_data.plyr_name[i] = edit_name_data->name[i];
		key_length = ARRAY_SIZE(lock_pick_table);
		if ((master_key < key_length) && (strncmp(creatp_player_data.cmos_data.plyr_name, lock_pick_table[master_key], LETTERS_IN_NAME) == 0)) {
			if (++master_key == key_length)
				snd_scall_bank(cursor_bnk_str, 57, VOLUME4, 127, PRIORITY_LVL4);
		} else if (master_key != key_length)
			master_key = 0;
		for (i = 0; i < PIN_NUMBERS; i++)
			creatp_player_data.cmos_data.plyr_pin_nbr[i] = edit_name_data->pin[i];
	}
	
	for (i = 0; i < EDIT_NAME_SPRITE_COUNT; i++)
		free_image_to_sprite(&edit_name_sprite_list[i]);
	/* free the private data */
	free(data);
}	/* edit_name_exit */

static int edit_name_loop(int input, void *data)
{
	edit_name_data_t *edit_name_data;
	int i, done, save_mode;
	char *ch;
	
	edit_name_data = data;
	done = FALSE;
	move_edit_name_cursor(input, edit_name_data);
	if ((input & P_ABC_MASK) != 0) {
		if ((edit_name_data->cursor_pos == NAME_BACK) || (edit_name_data->cursor_pos == PIN_BACK)) {
			if ((edit_name_data->mode != ENTERING_NAME1) && (edit_name_data->mode != ENTERING_PIN1))
				edit_name_data->mode--;
		} else {
			save_mode = edit_name_data->mode;
			if (edit_name_data->cursor_pos == NAME_END) {
				for (i = edit_name_data->mode - ENTERING_NAME1; i < LETTERS_IN_NAME; i++)
					edit_name_data->name[i] = ' ';
				edit_name_data->mode = ENTERING_NAME6;
			} else {
				if (edit_name_data->mode <= ENTERING_NAME6) {
					ch = &edit_name_data->name[edit_name_data->mode - ENTERING_NAME1];
					if (edit_name_data->cursor_pos == NAME_SPACE)
						*ch = ' ';
					else if (edit_name_data->cursor_pos == NAME_EXCLM)
						*ch = '!';
					else
						*ch = edit_name_data->cursor_pos - NAME_A + 'A';
				} else {
					ch = &edit_name_data->pin[edit_name_data->mode - ENTERING_PIN1];
					if (edit_name_data->cursor_pos == PIN_SPACE)
						*ch = ' ';
					else
						*ch = edit_name_data->cursor_pos - PIN_0 + '0';
				}
			}
			
			if (edit_name_data->mode == ENTERING_PIN4)
				done = TRUE;
			else {
				if (edit_name_data->mode == ENTERING_NAME6) {
					if (creatp_check_bad_name(edit_name_data->name, LETTERS_IN_NAME)) {
						/* bad name entered sound call */
						snd_scall_bank(creatp_snd_bank, CREATP_BADNAME, VOLUME4, 127, PRIORITY_LVL4);
						edit_name_data->mode = save_mode;
					} else {
						/* enter letter sound call */
						snd_scall_bank(cursor_bnk_str, SELECT_SND1, VOLUME4, 127, PRIORITY_LVL4);
						do_top_sparkle(edit_name_sprite_list[3].sprite, 100, 100, TRUE, FALSE);
						do_oval_sparkle(edit_name_sprite_list[3].sprite, 31, 46, TRUE, FALSE, TRUE);
						/* hide the cursor on the fade */
						do_glow_sparkle(NULL);
						fade_name_to_pin(edit_name_data);
						do_glow_sparkle(edit_name_sprite_list[NAME_CURSOR].sprite);
						show_creatp_title(ENTER_PIN_MODE);
						edit_name_data->mode++;
					}
					return done;
				} else
					edit_name_data->mode++;
			}
		}
		/* enter letter sound call */
		snd_scall_bank(cursor_bnk_str, SELECT_SND1, VOLUME4, 127, PRIORITY_LVL4);
		do_top_sparkle(edit_name_sprite_list[3].sprite, 100, 100, TRUE, FALSE);
		do_oval_sparkle(edit_name_sprite_list[3].sprite, 31, 46, TRUE, FALSE, done);
	}
	return done;
}	/* edit_name_loop */

static void edit_name_wrapper(int startup_name)
{
	player_record_info record;
	int ok, save_mode, pending_guest, guest_team, guest_index;
	image_info_t *mugshot_image;
	texture_page_t guest_texture_list[2];
	
	save_mode = clock_mode;
	pending_guest = FALSE;
	guest_team = 0;
	guest_index = 0;
	if (startup_name)
		creatp_player_data.record_num = -1;
	do {
		/* show the title and instructions */
		show_creatp_title(ENTER_NAME_MODE);
		/* get the name and pin */
		mode_loop(edit_name_init, edit_name_loop, edit_name_exit);
		
		/* the timer expired */
		if (!creatp_active)
			break;
		
		/* find out the record we have */
		find_player_record(creatp_player_data.cmos_data.plyr_name, creatp_player_data.cmos_data.plyr_pin_nbr, 0, 0, 0, &record);
		
		/* hide instructions */
		hide_creatp_title();
		
		switch (record.record_type) {
		case NO_RECORD:
			ok = TRUE;
			if (startup_name) {
				creatp_player_data.record_num = get_free_record_nbr();
				player_data_init(TRUE);
				if (pending_guest) {
					creatp_player_data.cmos_data.team = guest_team;
					creatp_player_data.cmos_data.player_index = guest_index;
					pending_guest = FALSE;
				}
				edit_name_record_message(RECORD_NOT_FOUND);
				/* give them a full 2:30 */
				clock_mode = CLOCK_NORMAL;
			}
			break;
		case CREATP_RECORD:
		case PLAYER_RECORD:
			ok = TRUE;
			if (startup_name) {
				/* player found sound effect */
				snd_scall_bank(cursor_bnk_str, 53, VOLUME4, 127, PRIORITY_LVL3);
				creatp_player_data.record_num = record.record_num;
				memcpy(&creatp_player_data.cmos_data, &record.cmos_data, sizeof(plyr_record_t));
				if (pending_guest) {
					creatp_player_data.cmos_data.team = guest_team;
					creatp_player_data.cmos_data.player_index = guest_index;
					pending_guest = FALSE;
				} else {
					creatp_player_data.cmos_data.team = record.team;
					creatp_player_data.cmos_data.player_index = record.player_index;
				}
				if (record.record_type == CREATP_RECORD) {
					/* give them a fast 2:30 */
					clock_mode = CLOCK_FASTER;
				} else {
					player_data_init(FALSE);
					/* give them a full 2:30 */
					clock_mode = CLOCK_NORMAL;
				}
				edit_name_record_message(RECORD_FOUND);
			} else if (creatp_player_data.record_num != record.record_num) {
				/* we are about to save over an other record, keep our slot and blank the other record */
				/* do this to avoid having two records with the same name and pin in CMOS */
				memset(&record.cmos_data, 0, sizeof(plyr_record_t));
				set_generic_record(PLYR_RECS_ARRAY, record.record_num, &record.cmos_data);
			}
			break;
		default:
		case NBA_PLAYER_RECORD:
		case SECRET_PLAYER_RECORD:
			/* secret player found sound effect */
			snd_scall_bank(cursor_bnk_str, 57, VOLUME4, 127, PRIORITY_LVL4);
			ok = FALSE;
			creatp_player_data.cmos_data.team = record.team;
			creatp_player_data.cmos_data.player_index = record.player_index;
			if (startup_name) {
				pending_guest = TRUE;
				guest_team = creatp_player_data.cmos_data.team;
				guest_index = creatp_player_data.cmos_data.player_index;
			} else
				update_player_display();
			edit_name_record_message(RES_RECORD_FOUND);
			break;
		}
		load_guest_mug(&creatp_player_data.cmos_data);
	} while (!ok);
	clock_mode = save_mode;
	/* update the name on screen */
	if (!startup_name)
		mini_attribute_update(MINI_ATTR_NAME, -1);
	/* set which heads are unlocked for this player */
	unlock_head_list();
}	/* edit_name_wrapper */

static void load_guest_mug(plyr_record_t *player_info)
{
	image_info_t *mugshot_image;
	texture_page_t guest_texture_list[2];
	
#ifdef SHOW_ALL_HEADS
	if (TRUE) {
#else
	if (player_record_info_to_head_list_index(player_info) == -1) {
#endif
		/* we have a guest player, load their mugshot */
		if (guest_present) {
			/* dump the old mugshot texture page */
			unlock_multiple_textures(CREATP_GUEST_TID, 0xFFFFFFFF);
			delete_multiple_textures(CREATP_GUEST_TID, 0xFFFFFFFF);
		}
		guest_present = TRUE;
		/* find the guest mugshot */
		mugshot_image = teaminfo[player_info->team].p_spd[player_info->player_index].pii_mug;
		/* build the texture list to load */
		guest_texture_list[0].texture_name = mugshot_image->texture_name;
		guest_texture_list[0].texture_id = CREATP_GUEST_TID;
		guest_texture_list[1].texture_name = NULL;
		guest_texture_list[1].texture_id = 0;
		/* load it */
		load_texture_list(guest_texture_list);
		/* lock it into TMU ram */
		lock_multiple_textures(CREATP_GUEST_TID, 0xFFFFFFFF);
		creatp_head_list[0].team = player_info->team;
		creatp_head_list[0].player_index = player_info->player_index;
		creatp_head_list[0].mugshot.image = mugshot_image;
		creatp_head_list[0].availiable = TRUE;
	}
}	/* load_guest_mug */

static void edit_name_record_message(int which_message)
{
	static char *mode_instruction[] = {
		/* RECORD_FOUND */
		"RECORD FOUND", "PLAYER RESTORED",
		/* RECORD_NOT_FOUND */
		"RECORD NOT FOUND", "NEW PLAYER CREATED",
		/* RES_RECORD_FOUND */
		"RESERVED RECORD FOUND", "CHOOSE NEW NAME/PIN",
	};
	image_sprite title;
	int delay, font_id;
	float x, y, fh;
	
	title.image = &namepinm;
	title.sprite = NULL;
	
	image_to_sprite(&title, MODE_TITLE_Z, I2S_ORIG, I2S_ABS, 0, SPRITE_VRES - 126, FALSE);
	show_creatp_shadow(SPRITE_VRES - (126 + title.image->h + 2), 1, 2, 0, 1);
	
	font_id = is_low_res ? FONTID_NBA10 : FONTID_NBA12;
	set_text_font(font_id);
	set_string_id(BUTTON_INSTRUCTION_SID);
	set_text_justification_mode(HJUST_CENTER);
	set_text_color(OFF_YELLOW_COLOR);
	set_text_z_inc(-0.10F);
	
	fh = get_font_height(font_id);
	x = 246;
	y = SPRITE_VRES - 174;
	
	delay = (which_message == RES_RECORD_FOUND) ? 3 : 2;
	which_message *= 2;
	set_text_position(x, y + (fh * 0.5f), INSTRUCTION_Z);
	oprintf("%s", mode_instruction[which_message]);
	set_text_position(x, y - ((fh * 0.5f) + 5), INSTRUCTION_Z);
	flasher_str = oprintf("%s", mode_instruction[which_message + 1]);
	set_text_z_inc(0.0F);
	
	flasher_proc = qcreate("flashE", MESSAGE_PID, proc_flash_text, 0, 0, 0, 0);
	clock_mode = CLOCK_SLOWER;
	wait_for_button(20, delay * tsec);
	
	/* stop the text flashing */
	if (flasher_proc != NULL) {
		kill(flasher_proc, 0);
		flasher_proc = NULL;
	}
	delete_multiple_strings(BUTTON_INSTRUCTION_SID, 0xFFFFFFFF);
	hide_creatp_shadow();
	free_image_to_sprite(&title);
}	/* edit_name_record_message */

int creatp_check_bad_name(char *name, int count)
{
	static char *bad_name_list[] = { "FUCK", "FUC", "FUK", "FCK", "PHK", "FUX", "PHUCK", "PHUK", "VAGINA",
									 "ASS", "ASSWIP", "ASSWYP", "ASSHOL" "ASHOLE", "AHOLE", "ASSLIK",
									 "SUCK", "SUC", "SUK", "SUX", "CUM", "JIZ", "JISM", "JIZM", "CLIT",
									 "COCK", "CUNT", "PUSSY", "PUSSIE", "PISS", "PRICK", "SHIT", "NIGGER",
									 "PUTANG", "JAGOFF", "JRKOFF", "EATME", "GAYBOY", "JEWBOY", "FAGBOY",
									 "FAGGOT",  "FAGGIT", "FELCH", "PHELCH", "WHORE", "DICKHD"
	};
	char test_name1[7], test_name2[7];
	char test_name1r[7], test_name2r[7];
	int i, j, k;
	
	for (i = 0, j = 0, k = 0; i < count; i++) {
		/* delete any spaces and exclamation points */
		if ((name[i] != ' ') && (name[i] != '!'))
			test_name1[j++] = name[i];
		/* delete any spaces and view exclamation points as Is */
		if (name[i] != ' ') {
			if (name[i] == '!')
				test_name2[k++] = 'I';
			else
				test_name2[k++] = name[i];
		}
	}
	/* add the null */
	test_name1[j] = '\0';
	test_name2[k] = '\0';
	
	/* test the names reversed also */
	test_name1r[j] = '\0';
	test_name2r[k] = '\0';
	for (i = 0; i < j; i++)
		test_name1r[i] = test_name1[j - 1 - i];
	for (i = 0; i < k; i++)
		test_name2r[i] = test_name2[k - 1 - i];
	
	/* check for a bad name */
	for (i = 0; i < ARRAY_SIZE(bad_name_list); i++)
		if (strstr(test_name1, bad_name_list[i]) || strstr(test_name2, bad_name_list[i]) ||
			strstr(test_name1r, bad_name_list[i]) || strstr(test_name2r, bad_name_list[i]))
			return TRUE;
	return FALSE;
}	/* creatp_check_bad_name */

static void fade_name_to_pin(edit_name_data_t *edit_name_data)
{
	long name_alpha, pin_alpha;
	int i, j;
	
	/* hide the cursor */
	hide_sprite(edit_name_sprite_list[NAME_CURSOR].sprite);
	
	/* show all of the pin page items */
	for (i = PIN_0; i <= PIN_BACK; i++)
		unhide_sprite(edit_name_sprite_list[i].sprite);
	for (i = ENTER_DASH2; i <= ENTER_DASH5; i++)
		unhide_sprite(edit_name_sprite_list[i].sprite);
	
	/* fade the pin items through the name items */
	for (j = 1; j <= 16; j++) {
		pin_alpha = ((256 / 16) * j) - 1;
		name_alpha = 255 - pin_alpha;
		for (i = NAME_A; i <= NAME_END; i++) {
			edit_name_sprite_list[i].sprite->state.constant_color &= 0x00FFFFFF;
			edit_name_sprite_list[i].sprite->state.constant_color |= name_alpha << 24;
		}
		for (i = ENTER_LETTER1; i <= ENTER_LETTER6; i++) {
			edit_name_sprite_list[i].sprite->state.constant_color &= 0x00FFFFFF;
			edit_name_sprite_list[i].sprite->state.constant_color |= name_alpha << 24;
		}
		
		for (i = PIN_0; i <= PIN_BACK; i++) {
			edit_name_sprite_list[i].sprite->state.constant_color &= 0x00FFFFFF;
			edit_name_sprite_list[i].sprite->state.constant_color |= pin_alpha << 24;
		}
		for (i = ENTER_DASH2; i <= ENTER_DASH5; i++) {
			edit_name_sprite_list[i].sprite->state.constant_color &= 0x00FFFFFF;
			edit_name_sprite_list[i].sprite->state.constant_color |= pin_alpha << 24;
		}
		sleep(1);
	}
	
	/* hide all of the name items */
	for (i = NAME_A; i <= NAME_END; i++)
		hide_sprite(edit_name_sprite_list[i].sprite);
	for (i = ENTER_LETTER1; i <= ENTER_LETTER6; i++) {
		hide_sprite(edit_name_sprite_list[i].sprite);
		edit_name_sprite_list[i].sprite->state.constant_color |= 0xFF000000;
	}
	
	/* start the pin page with 0 selected */
	edit_name_data->cursor_pos = PIN_0;
	set_edit_name_cursor(edit_name_data->cursor_pos);
	/* show the cursor */
	unhide_sprite(edit_name_sprite_list[NAME_CURSOR].sprite);
	
	/* hide the first and last dash for pin mode */
	hide_sprite(edit_name_sprite_list[ENTER_DASH1].sprite);
	hide_sprite(edit_name_sprite_list[ENTER_DASH6].sprite);
}	/* fade_name_to_pin */

static void move_edit_name_cursor(int input, edit_name_data_t *edit_name_data)
{
	int base;
	
	/* calculate the new cursor position */
	if (edit_name_data->mode <= ENTERING_NAME6) {
		base = edit_name_data->cursor_pos - NAME_A;
		if ((input & P_UP) != 0)
			base = ((base >= 0) && (base <= 4)) ? base + 25 : base - 5;
		if ((input & P_DOWN) != 0)
			base = ((base >= 25) && (base <= 29)) ? base - 25 : base + 5;
		if ((input & P_LEFT) != 0)
			base = (base % 5) == 0 ? base + 4 : base - 1;
		if ((input & P_RIGHT) != 0)
			base = ((base - 4) % 5) == 0 ? base - 4 : base + 1;
		base += NAME_A;
	} else {
		base = edit_name_data->cursor_pos - PIN_0;
		if ((input & P_UP) != 0)
			base = ((base >= 0) && (base <= 2)) ? base + 9 : base - 3;
		if ((input & P_DOWN) != 0)
			base = ((base >= 9) && (base <= 11)) ? base - 9 : base + 3;
		if ((input & P_LEFT) != 0)
			base = (base % 3) == 0 ? base + 2 : base - 1;
		if ((input & P_RIGHT) != 0)
			base = ((base - 2) % 3) == 0 ? base - 2 : base + 1;
		base += PIN_0;
	}
	if (edit_name_data->cursor_pos != base) {
		/* move cursor sound call */
		snd_scall_bank(plyr_bnk_str, CURSOR_SND1, VOLUME4, 127, PRIORITY_LVL3);
		/* move the cursor sprite */
		edit_name_data->cursor_pos = base;
		set_edit_name_cursor(edit_name_data->cursor_pos);
	}
	/* update the dash position to the letter */
	update_dash_letter(edit_name_data);
}	/* move_edit_name_cursor */

static void set_edit_name_cursor(int cursor_pos)
{
	sprite_info_t *sprite;
	int cursor_delta_x, cursor_delta_y;
	
	find_sprite_center(edit_name_sprite_list[cursor_pos].sprite, &glow_cursor_x_pos, &glow_cursor_y_pos);
	glow_cursor_y_pos--;
	sprite = edit_name_sprite_list[NAME_CURSOR].sprite;
	if ((cursor_pos == PIN_1) || (cursor_pos == PIN_4))
		cursor_delta_x = (sprite->ii->w / 2) + 1;
	else
		cursor_delta_x = (sprite->ii->w / 2) + 2;
	cursor_delta_y = sprite->ii->h / 2;
	
	glow_cursor_x_pos = glow_cursor_x_pos - cursor_delta_x;
	glow_cursor_y_pos = SPRITE_VRES - (glow_cursor_y_pos - cursor_delta_y);
	set_sprite_pos(sprite, glow_cursor_x_pos, glow_cursor_y_pos);
}	/* set_edit_name_cursor */

static void update_dash_letter(edit_name_data_t *edit_name_data)
{
	sprite_info_t *dash_sprite, *letter_sprite;
	image_info_t *src_letter;
	int cursor_pos;
	int dash_sprite_index, letter_sprite_index;
	int x_pos, y_pos;
	int delta_x, delta_y;
	
	cursor_pos = edit_name_data->cursor_pos;
	if (edit_name_data->mode <= ENTERING_NAME6) {
		dash_sprite_index = edit_name_data->mode - ENTERING_NAME1 + ENTER_DASH1;
		letter_sprite_index = edit_name_data->mode - ENTERING_NAME1 + ENTER_LETTER1;
		src_letter = edit_name_sprite_list[cursor_pos].image;
	} else {
		dash_sprite_index = edit_name_data->mode - ENTERING_PIN1 + ENTER_DASH2;
		letter_sprite_index = edit_name_data->mode - ENTERING_PIN1 + ENTER_LETTER2;
		src_letter = edit_name_sprite_list[PIN_ASTRX].image;
	}
	dash_sprite = edit_name_sprite_list[dash_sprite_index].sprite;
	letter_sprite = edit_name_sprite_list[letter_sprite_index].sprite;
	if (cursor_pos == NAME_SPACE) {
		/* hide the dash character */
		hide_sprite(dash_sprite);
		/* hide the letter character */
		hide_sprite(letter_sprite);
	} else if ((cursor_pos == NAME_BACK) || (cursor_pos == NAME_END) || (cursor_pos == PIN_BACK)) {
		/* hide the letter character */
		hide_sprite(letter_sprite);
		/* show the dash character */
		unhide_sprite(dash_sprite);
	} else {
		/* hide the dash character */
		hide_sprite(dash_sprite);
		/* show the letter character and update to the correct sprite */
		letter_sprite->ii = src_letter;
		letter_sprite->tn = find_texture(src_letter->texture_name, CREATP_TID);
		find_sprite_center(dash_sprite, &x_pos, &y_pos);
		delta_x = src_letter->w / 2;
		delta_y = src_letter->h / 2;
		set_sprite_pos(letter_sprite, x_pos - delta_x, SPRITE_VRES - (y_pos - delta_y));
		unhide_sprite(letter_sprite);
	}
}	/* update_dash_letter */

/*
 *	exit save functions
 */

static void *yes_no_init(void)
{
	yes_no_data_t *data;
	int i, y_pos;
	
	/* allocate the private data */
	data = malloc(sizeof(yes_no_data_t));
	if (data != NULL) {
		data->current_button = YES_NO_YES_BUTTON;
		/* build the button sprites */
		y_pos = SPRITE_VRES - 114 + YES_NO_MODE_Y_OFFSET;
		for (i = YES_NO_YES_BUTTON; i < YES_NO_BUTTON_COUNT; i++) {
			image_to_sprite(&yes_no_button_list[i].unselected_button, BUTTON_MIDDLE_Z, I2S_ORIG, I2S_ABS, 0, y_pos, FALSE);
			image_to_sprite(&yes_no_button_list[i].selected_button, BUTTON_MIDDLE_Z, I2S_ORIG, I2S_ABS, 0, y_pos, TRUE);
			yes_no_button_list[i].selected_button.sprite->id = (i << 16) | GLOW_YES_NO;
			image_to_sprite(&yes_no_button_list[i].title, BUTTON_TITLE_Z, I2S_ORIG, I2S_ABS, 0, y_pos - 19, FALSE);
			y_pos += YES_NO_BUTTON_DELTA;
		}
		/* default the auto select timer */
		data->auto_select_timer = AUTO_SELECT_TIME_15;
		
		/* select the initial button */
		select_yes_no_button(data->current_button);
	}
	return data;
}	/* yes_no_init */

static void yes_no_exit(void *data)
{
	int i;
	
	/* delete the button sprites */
	for (i = YES_NO_YES_BUTTON; i < YES_NO_BUTTON_COUNT; i++) {
		free_image_to_sprite(&yes_no_button_list[i].unselected_button);
		free_image_to_sprite(&yes_no_button_list[i].selected_button);
		free_image_to_sprite(&yes_no_button_list[i].title);
	}
	/* free the private data */
	free(data);
}	/* yes_no_exit */

static int yes_no_loop(int input, void *data)
{
	yes_no_data_t *yes_no_data;
	int done, auto_select;
	
	yes_no_data = data;
	done = FALSE;
	if (next_button(YES_NO_YES_BUTTON, YES_NO_NO_BUTTON, &yes_no_data->current_button, input, FALSE)) {
		/* reset the auto select timer */
		yes_no_data->auto_select_timer = AUTO_SELECT_TIME_15;
		/* issue a sound call */
		snd_scall_bank(creatp_snd_bank, input == P_DOWN ? CREATP_DOWN : CREATP_UP, VOLUME4, 127, PRIORITY_LVL3);
		select_yes_no_button(yes_no_data->current_button);
	} else {
		auto_select = ((input & P_ABC_MASK) == 0) && (--yes_no_data->auto_select_timer == 0);
		if (auto_select) {
			yes_no_data->current_button = YES_NO_NO_BUTTON;
			select_yes_no_button(yes_no_data->current_button);
		}
		if (((input & P_ABC_MASK) != 0) || auto_select) {
			/* issue a sound call */
			snd_scall_bank(creatp_snd_bank, CREATP_SELECT, VOLUME4, 127, PRIORITY_LVL5);
			do_top_sparkle(yes_no_button_list[yes_no_data->current_button].selected_button.sprite, 300, 300, FALSE, FALSE);
			do_oval_sparkle(yes_no_button_list[yes_no_data->current_button].selected_button.sprite, 149, 117, FALSE, FALSE, TRUE);
			yes_no_result = yes_no_data->current_button == YES_NO_YES_BUTTON;
			done = TRUE;
		}
	}
	return done;
}	/* yes_no_loop */

static void select_yes_no_button(int button)
{
	int i;
	
	for (i = 0; i < YES_NO_BUTTON_COUNT; i++) {
		unhide_sprite(yes_no_button_list[i].unselected_button.sprite);
		hide_sprite(yes_no_button_list[i].selected_button.sprite);
		set_sprite_color(yes_no_button_list[i].title.sprite, GRAY_COLOR);
	}
	hide_sprite(yes_no_button_list[button].unselected_button.sprite);
	unhide_sprite(yes_no_button_list[button].selected_button.sprite);
	set_sprite_color(yes_no_button_list[button].title.sprite, OFF_YELLOW_COLOR);
	do_glow_sparkle(yes_no_button_list[button].selected_button.sprite);
}	/* select_yes_no_button */

/* no args used */
static void proc_timer(int *args)
{
	ostring_t *min_str, *colon_str, *sec_str;
	int last_min, last_sec;
	int time_left, min, sec, last_sparkle_time;
	int colon_width, two_width;
	int x_offset, min_offset[2];
	int timer_base;
	float timer_scale;
	
	set_text_font(is_low_res ? FONTID_NBA14 : FONTID_NBA20);
	colon_width = get_string_width(":") + (is_low_res ? 3 : 1);
	two_width = get_string_width("2");
	min_offset[0] = TIMER_X + (two_width - get_string_width("1"));
	min_offset[1] = TIMER_X;
	if (is_low_res)
		two_width += 4;
	last_sparkle_time = 0;
	last_min = last_sec = 0;
	timer_base = tsec;
	timer_scale = 1.0f;
	
	for (time_left = TIMER_INIT_VALUE; time_left >= 0; time_left -= tsec) {
		/* calculate the new time */
		min = time_left / (tsec * 60);
		sec = (time_left / tsec) % 60;
		
		switch (clock_mode) {
		case CLOCK_NORMAL:
			timer_scale = 1.0f;
			break;
		case CLOCK_FASTER:
			timer_scale = 0.85f;
			break;
		case CLOCK_SLOWER:
			timer_scale = 2.0f;
			break;
		}
		if ((min != last_min) || (sec != last_sec)) {
			last_min = min;
			last_sec = sec;
			
			/* remove old time text */
			delete_multiple_strings(TIMER_SID, 0xFFFFFFFF);
			
			/* draw new time text */
			set_text_font(FONTID_NBA20);
			set_string_id(TIMER_SID);
			set_text_justification_mode(HJUST_LEFT | VJUST_CENTER);
			set_text_color(((min == 0) && (sec <= 30)) ? LT_RED : WHITE);
			set_text_z_inc(-0.10F);
			
			/* draw the minutes */
			if (min != 0) {
				if (min > 2)
					set_text_position(min_offset[1], TIMER_Y, TIMER_MIN_Z);
				else
					set_text_position(min_offset[min - 1], TIMER_Y, TIMER_MIN_Z);
				min_str = oprintf("%d", min);
			}
			
			/* draw the colon */
			x_offset = TIMER_X + two_width;
			set_text_position(x_offset, TIMER_Y + 3, TIMER_COL_Z);
			colon_str = oprintf(":");
			
			/* draw the seconds */
			x_offset += colon_width;
			set_text_position(x_offset, TIMER_Y, TIMER_SEC_Z);
			sec_str = oprintf("%02d", sec);		
			set_text_z_inc(0.0F);
			
			/* check if time is running out */
			if ((min == 0) && (sec <= 20) && ((sec % 2) == 0) && (sec != last_sparkle_time)) {
				/* issue a sound effect */
				
				/* if getting really low on time scale the clock slower */
				if (sec == 20)
					timer_base = tsec * 1.50f;
				if (sec == 10)
					timer_base = tsec * 1.75f;
				else if (sec == 5)
					timer_base = tsec * 2.00f;
				
				if (sec == 0)
					snd_scall_bank(plyr_bnk_str, 13, VOLUME3, 127, PRIORITY_LVL4);
				else
					snd_scall_bank(cursor_bnk_str, 15, VOLUME3, 127, PRIORITY_LVL4);
				/* make a sparkle on the seconds */
				do_oval_sparkle(creatp_background_list2[0].sprite, 106, 66, FALSE, TRUE, FALSE);
				last_sparkle_time = sec;
				/* issue a gong sound */
			}
		}
		
		/* sleep for a second */
		sleep(timer_base * timer_scale);
#ifdef INFINITE_CLOCK
		if (time_left <= tsec) {
			time_left = TIMER_INIT_VALUE;
			timer_base = tsec;
			timer_scale = 1.0f;
		}
#endif
	}
	/* let the rest of the world know create player is over */
	creatp_active = FALSE;
	/* leave the clock displaying 0:00 */
#if 1
	for (;;)
		sleep(1);
#else
	/* when timer expires create player cleans up and exits back to attract mode */
	delete_multiple_strings(TIMER_SID, 0xFFFFFFFF);
	die(0);
#endif
}	/* proc_timer */

static void proc_credit_msg(int *args)
{
	sprite_info_t *credit_background;
	int is_free_play, first_time;
	int num_credit, num_new_credit;
	
	is_free_play = coin_check_freeplay();
	first_time = TRUE;
	credit_background = beginobj(&credtbar, 0.0f, SPRITE_VRES, CREDIT_BAR_Z, 0);
	
	if (is_free_play) {
		set_string_id(CREDIT_SID);
		set_text_font(FONTID_BAST7T),
			set_text_color(LT_YELLOW);
		set_text_justification_mode(HJUST_LEFT);
		set_text_position(220, SPRITE_VRES - 381, 4.0f);
		set_text_z_inc(-0.10F);
		oprintf("FREE PLAY");
		set_text_z_inc(0.0F);
		die(0);
	} else {
		num_credit = -1;
		for (;;) {
			num_new_credit = get_total_credits();
			if (num_credit != num_new_credit) {
				num_credit = num_new_credit;
				delete_multiple_strings(CREDIT_SID, -1);
				set_string_id(CREDIT_SID);
				set_text_font(FONTID_BAST7T),
				set_text_color(LT_YELLOW);
				set_text_justification_mode(HJUST_LEFT);
				set_text_position(214, SPRITE_VRES - 381, 4.0f);
				set_text_z_inc(-0.10F);
				oprintf("CREDITS:%d", num_credit);
				set_text_z_inc(0.0F);
				if (!first_time)
					do_top_sparkle(credit_background, 200, 150, FALSE, TRUE);
				first_time = FALSE;
			}
			sleep(3);
		}
	}
}	/* proc_credit_msg */

/* args[0] is the X position */
/* args[1] is the Y position */
/* args[2] is the w scale(mult by 100) */
/* args[3] is the h scale(mult by 100) */
static void proc_sparkle(int *args)
{
	static image_info_t *sparks[] = {&pflash01, &pflash02, &pflash03, &pflash04,
									 &pflash05, &pflash06, &pflash07, &pflash08,
									 &pflash09, &pflash10, &pflash11, &pflash12,
									 &pflash13, &pflash14, &pflash15, &pflash16
	};
	sprite_info_t *sparkle;
	float w_scale, h_scale;
	int i, tick;
	
	w_scale = (float)args[2] / 100.0f;
	h_scale = (float)args[3] / 100.0f;
	sparkle = beginobj(sparks[0], args[0], args[1], SPARKLE_Z, CREATP_TID);
	/* set to additive blending */
	sparkle->state.alpha_rgb_dst_function = GR_BLEND_ONE;
	sparkle->w_scale = w_scale;
	sparkle->h_scale = h_scale;
	generate_sprite_verts(sparkle);
	sleep(3);
	tick = 2;

	for (i = 1; i < ARRAY_SIZE(sparks); i++) {
		sparkle->ii = sparks[i];
		sparkle->tn = find_texture(sparks[i]->texture_name, CREATP_TID);
		generate_sprite_verts(sparkle);
		sleep(tick);
		tick = tick == 3 ? 2 : 3;
	}
	delobj(sparkle);
	die(0);
}	/* proc_sparkle */

/* args[0] is the X position */
/* args[1] is the Y position */
/* args[2] is the w scale(mult by 100) */
/* args[3] is the h scale(mult by 100) */
static void proc_oval_sparkle(int *args)
{
	static image_info_t *oval_sparks[] = {&cflash01, &cflash02, &cflash03, &cflash04,
										  &cflash05, &cflash06, &cflash07, &cflash08,
										  &cflash09, &cflash10, &cflash11, &cflash12,
										  &cflash13, &cflash14, &cflash15, &cflash16
	};
	float w_scale, h_scale;
	int i, short_sparkle, clock, tick;
	sprite_info_t *clock_sprite;
	sprite_info_t **the_sprite;
	
	short_sparkle = args[2] < 0;
	clock = args[3] < 0;
	w_scale = (float)abs(args[2]) / 100.0f;
	h_scale = (float)abs(args[3]) / 100.0f;
	the_sprite = clock ? &clock_sprite : &oval_sparkle;
	*the_sprite = beginobj(oval_sparks[0], args[0], args[1], OVAL_SPARKLE_Z, CREATP_TID);
	/* set to additive blending */
	(**the_sprite).state.alpha_rgb_dst_function = GR_BLEND_ONE;
	(**the_sprite).w_scale = w_scale;
	(**the_sprite).h_scale = h_scale;
	generate_sprite_verts(*the_sprite);
	sleep(2);
	tick = short_sparkle ? 1 : 2;
	for (i = 1; i < ARRAY_SIZE(oval_sparks); i++) {
		(**the_sprite).ii = oval_sparks[i];
		(**the_sprite).tn = find_texture(oval_sparks[i]->texture_name, CREATP_TID);
		generate_sprite_verts(*the_sprite);
		sleep(tick);
		if (short_sparkle)
			tick = tick == 1 ? 2 : 1;
	}
	delobj(*the_sprite);
}	/* proc_oval_sparkle */

static void proc_top_sparkle(int *args)
{
	static image_info_t *top_sparks[] = {&flarefl01, &flarefl02, &flarefl03, &flarefl04,
										 &flarefl05, &flarefl06, &flarefl07, &flarefl08,
										 &flarefl09, &flarefl10, &flarefl11, &flarefl12,
										 &flarefl13, &flarefl14, &flarefl15
	};
	float w_scale, h_scale;
	int i, short_sparkle, tick, credit_bar;
	sprite_info_t **the_sprite;
	
	short_sparkle = args[2] < 0;
	credit_bar = args[3] < 0;
	w_scale = (float)abs(args[2]) / 100.0f;
	h_scale = (float)abs(args[3]) / 100.0f;
	the_sprite = credit_bar ? &credit_sparkle : &top_sparkle;
	*the_sprite = beginobj(top_sparks[0], args[0], args[1], TOP_SPARKLE_Z, CREATP_TID);
	/* set to additive blending */
	(**the_sprite).state.alpha_rgb_dst_function = GR_BLEND_ONE;
	(**the_sprite).w_scale = w_scale;
	(**the_sprite).h_scale = h_scale;
	generate_sprite_verts(*the_sprite);
	sleep(2);
	tick = short_sparkle ? 1 : 2;
	for (i = 1; i < ARRAY_SIZE(top_sparks); i++) {
		(**the_sprite).ii = top_sparks[i];
		(**the_sprite).tn = find_texture(top_sparks[i]->texture_name, CREATP_TID);
		generate_sprite_verts(*the_sprite);
		sleep(tick);
		if (short_sparkle)
			tick = tick == 1 ? 2 : 1;
	}
	delobj(*the_sprite);
}	/* proc_top_sparkle */

static void proc_glow_sparkle(int *args)
{
	static image_info_t *glow_sprite_list[] = {&atrbdscs2, &privibrs2, &editdisc2, &exitdscs2,
											   &hedsplq3, &nknmdsc2, &hdscredl
	};
	sprite_info_t *glow_sprite;
	sprite_info_t *current_glow_sparkle_target;
	struct image_info *current_ii;
	long alpha_level, delta;
	int which_glow, which_item, x_pos, y_pos;
	int tx, ty;
	
	current_glow_sparkle_target = NULL;
	current_ii = NULL;
	alpha_level = 255;
	delta = -8;
	for (;;) {
		if (glow_sparkle_target == NULL) {
			current_glow_sparkle_target = NULL;
			current_ii = NULL;
			alpha_level = 255;
			delta = -8;
			while (glow_sparkle_target == NULL)
				sleep(1);
		}
		which_glow = glow_sparkle_target->id & 0x0000FFFF;
		which_item = glow_sparkle_target->id >> 16;
		glow_sprite = beginobj(glow_sprite_list[which_glow], 0, SPRITE_VRES, BUTTON_MIDDLE_Z - 5.0F, CREATP_TID);
		get_sprite_pos(glow_sprite, &x_pos, &y_pos);
		switch (which_glow) {
		case GLOW_ATTR:
			y_pos += (which_item * ATTR_BUTTON_DELTA);
			break;			
		case GLOW_PRIV:
			y_pos += PRIV_MODE_Y_OFFSET + (which_item * PRIV_BUTTON_DELTA) + 4;
			break;			
		case GLOW_YES_NO:
			y_pos += YES_NO_MODE_Y_OFFSET + (which_item * YES_NO_BUTTON_DELTA);
			break;			
		case GLOW_HEAD:
			y_pos += HEAD_MODE_Y_OFFSET;
			break;
		case GLOW_NAMELIST:
			y_pos += NAMELIST_MODE_Y_OFFSET;
			break;
		}
		set_sprite_pos(glow_sprite, x_pos, y_pos);
		current_glow_sparkle_target = glow_sparkle_target;
		current_ii = glow_sparkle_target->ii;
		while ((glow_sparkle_target != NULL) && (current_glow_sparkle_target == glow_sparkle_target) && (current_ii == glow_sparkle_target->ii)) {
			if (which_glow == GLOW_CURSOR) {
				tx = glow_sprite->ii->ax - uniform_number_sprite_list[UNIFORM_NUMBER_CURSOR].image->ax;
				ty = glow_sprite->ii->ay - uniform_number_sprite_list[UNIFORM_NUMBER_CURSOR].image->ay;
				set_sprite_pos(glow_sprite, glow_cursor_x_pos - tx, glow_cursor_y_pos - ty);
			} else if (which_glow == GLOW_HOTSPOT) {
				tx = glow_sprite->ii->ax - hotspot_list[1].image->ax;
				ty = glow_sprite->ii->ay - hotspot_list[1].image->ay - 4;
				set_sprite_pos(glow_sprite, glow_cursor_x_pos - tx, glow_cursor_y_pos - ty);
			}
			glow_sprite->state.constant_color &= 0x00FFFFFF;
			glow_sprite->state.constant_color |= alpha_level << 24;
			sleep(1);
			alpha_level += delta;
			if (alpha_level < 128) {
				alpha_level = 128;
				delta = +8;
			} else if (alpha_level > 255) {
				alpha_level = 255;
				delta = -8;
			}
		}
		delobj(glow_sprite);
	}
}	/* proc_glow_sparkle */

void do_oval_sparkle(sprite_info_t *sprite, int w_scale, int h_scale, int short_sparkle, int clock, int wait)
{
	static process_node_t *the_proc;
	int spark_x, spark_y;
	
	/* stop any existing sparkle effect */
	if (!clock) {
		the_proc = existp(OVAL_SPARKLE_PID, 0xFFFFFFFF);
		if (the_proc != NULL) {
			kill(the_proc, PD_DIE);
			delobj(oval_sparkle);
		}
	}
	
	find_sprite_center(sprite, &spark_x, &spark_y);
	
	if (short_sparkle)
		w_scale = -w_scale;
	if (clock)
		h_scale = -h_scale;
	the_proc = qcreate("CP_osprk", clock ? CLOCK_SPARKLE_PID : OVAL_SPARKLE_PID, proc_oval_sparkle, spark_x, SPRITE_VRES - spark_y, w_scale, h_scale);
	if (wait)
		while (existp(OVAL_SPARKLE_PID, 0xFFFFFFFF) != NULL)
			sleep(1);
}	/* do_oval_sparkle */

static void do_top_sparkle(sprite_info_t *sprite, int w_scale, int h_scale, int short_sparkle, int credit_bar)
{
	static process_node_t *the_proc;
	int spark_x, spark_y;
	
	/* stop any existing sparkle effect */
	the_proc = existp(credit_bar ? CREDIT_SPARKLE_PID : TOP_SPARKLE_PID, 0xFFFFFFFF);
	if (the_proc != NULL) {
		kill(the_proc, PD_DIE);
		delobj(credit_bar ? credit_sparkle : top_sparkle);
	}
	
	find_sprite_center(sprite, &spark_x, &spark_y);
	
	if (short_sparkle)
		w_scale = -w_scale;
	if (credit_bar)
		h_scale = -h_scale;
	the_proc = qcreate("CP_tsprk", credit_bar ? CREDIT_SPARKLE_PID : TOP_SPARKLE_PID, proc_top_sparkle, spark_x, SPRITE_VRES - spark_y, w_scale, h_scale);
}	/* do_top_sparkle */

static void do_glow_sparkle(sprite_info_t *sprite)
{
	glow_sparkle_target = sprite;
}	/* do_glow_sparkle */

static void proc_flash_text(int *args)
{ 
	for (;;) {
		sleep(15);
		flasher_str->state.color = LT_RED;
		change_alpha_cc(flasher_str, LT_RED);
		
		sleep(15);
		flasher_str->state.color = WHITE;
		change_alpha_cc(flasher_str, WHITE);
	}
}	/* proc_flash_text */

static void mode_loop(init_func initf, loop_func loopf, exit_func exitf)
{
	static int button_delay = 0;
	void *data;
	int switch_old, switch_current, switch_new, repeat;
	int done, do_fade_up;
	
	do_fade_up = (loopf != creatp_loop) && !kill_transit;
	/* reset the fade list */
	reset_fade_list();
	
	if (initf == NULL)
		data = NULL;
	else
		data = initf();
	
	/* present the mode */
	if (do_fade_up)
		fade_sprite_list(FADE_UP, FALSE);
	
	done = FALSE;
	switch_current = 0;
	repeat = SWITCH_DELAY;
	while (!done) {
		sleep(1);
		if (!creatp_active)
			break;
		if (button_delay > 0)
			button_delay--;
		switch_old = switch_current;
		switch_current = get_player_sw_current() >> player_mask;
		switch_new = get_player_sw_close() >> player_mask;
		
		if ((switch_new & P_ABC_MASK) == 0) {
			if (switch_current & P_DOWN) {
				button_delay = 0;
				if ((switch_old & P_DOWN) == 0)
					repeat = SWITCH_DELAY;
				else {
					if (--repeat == 0)
						repeat = REPEAT_DELAY;
				}
				if ((loopf == hotspot_loop) || (repeat == SWITCH_DELAY) || (repeat == REPEAT_DELAY))
					done = loopf(P_DOWN, data);
			} else if (switch_current & P_UP) {
				button_delay = 0;
				if ((switch_old & P_UP) == 0)
					repeat = SWITCH_DELAY;
				else {
					if (--repeat == 0)
						repeat = REPEAT_DELAY;
				}
				if ((loopf == hotspot_loop) || (repeat == SWITCH_DELAY) || (repeat == REPEAT_DELAY))
					done = loopf(P_UP, data);
			} else if (switch_current & P_LEFT) {
				button_delay = 0;
				if ((switch_old & P_LEFT) == 0)
					repeat = SWITCH_DELAY;
				else {
					if (--repeat == 0)
						repeat = REPEAT_DELAY;
				}
				if ((loopf == hotspot_loop) || (repeat == SWITCH_DELAY) || (repeat == REPEAT_DELAY))
					done = loopf(P_LEFT, data);
			} else if (switch_current & P_RIGHT) {
				button_delay = 0;
				if ((switch_old & P_RIGHT) == 0)
					repeat = SWITCH_DELAY;
				else {
					if (--repeat == 0)
						repeat = REPEAT_DELAY;
				}
				if ((loopf == hotspot_loop) || (repeat == SWITCH_DELAY) || (repeat == REPEAT_DELAY))
					done = loopf(P_RIGHT, data);
			} else
				/* allow idle actions */
				done = loopf(0, data);
		} else {
			if (button_delay == 0)
				done = loopf(switch_new & P_ABC_MASK, data);
			switch_current = 0;
			repeat = SWITCH_DELAY;
		}
	}
	/* remove the mode */
	if (loopf != creatp_loop)
		/* slow fade to give the cursor sparkle a chance to fade away */
		fade_sprite_list(FADE_DOWN, loopf == edit_name_loop);
	
	if (exitf != NULL)
		exitf(data);
	button_delay = 20;
}	/* mode_loop */

static void image_to_sprite(image_sprite *img, float z_level, char x_mode, char y_mode, int x_pos, int y_pos, int hide)
{
	int orig_x, orig_y;
	
	if (img->image == NULL)
		img->sprite = NULL;
	else {
		img->sprite = beginobj(img->image, 0, SPRITE_VRES, z_level, CREATP_TID);
		if (img->sprite == NULL) {
#ifdef DEBUG
			printf("build_image_sprite failed!\n");
			lockup();
#endif
			increment_audit(TEX_LOAD_FAIL_AUD);
		} else {
			if (fade_sprite_list_index < FADE_LIST_SIZE)
				fade_sprite_list_sprites[fade_sprite_list_index++] = img->sprite;
			get_sprite_pos(img->sprite, &orig_x, &orig_y);
			
			if (x_mode == I2S_ORIG)
				x_pos = orig_x;
			else if (x_mode == I2S_DELTA)
				x_pos += orig_x;
			
			if (y_mode == I2S_ORIG)
				y_pos = orig_y;
			else if (y_mode == I2S_DELTA)
				y_pos += orig_y;
			
			set_sprite_pos(img->sprite, x_pos, y_pos);
			
			if (hide)
				hide_sprite(img->sprite);
		}
	}
}	/* image_to_sprite */

static void free_image_to_sprite(image_sprite *img)
{
	if (img->sprite != NULL) {
		delobj(img->sprite);
		img->sprite = NULL;
	}
}	/* free_image_to_sprite */

static int load_texture_list(texture_page_t *texture_page_list)
{
	texture_node_t *texture_node;
	
	while (texture_page_list->texture_name != NULL) {
		texture_node = create_texture(texture_page_list->texture_name,
								  texture_page_list->texture_id,
								  0,
								  CREATE_NORMAL_TEXTURE,
								  GR_TEXTURECLAMP_CLAMP,
								  GR_TEXTURECLAMP_CLAMP,
								  GR_TEXTUREFILTER_BILINEAR,
								  GR_TEXTUREFILTER_BILINEAR);
		
		if (texture_node == NULL) {
			increment_audit(TEX_LOAD_FAIL_AUD);
#ifdef DEBUG
			printf("load_texture_list failed!\n");
			lockup();
#endif
			return FALSE;
		}
		texture_page_list++;
	}
	return TRUE;
}	/* load_texture_list */

static void find_sprite_center(sprite_info_t *sprite, int *x_pos, int *y_pos)
{
	*x_pos = sprite->x - sprite->ii->ax + sprite->ii->w / 2;
	*y_pos = SPRITE_VRES - (sprite->y + (sprite->ii->h / 2) + sprite->ii->ay);
}	/* find_sprite_center */

static void get_sprite_pos(sprite_info_t *sprite, int *x_pos, int *y_pos)
{
	*x_pos = sprite->x - sprite->ii->ax;
	*y_pos = sprite->y + sprite->ii->ay + sprite->ii->h;
}	/* get_sprite_pos */

static void set_sprite_pos(sprite_info_t *sprite, int x, int y)
{
	sprite->x = x + sprite->ii->ax;
	sprite->y = (y - sprite->ii->ay) - sprite->ii->h;
	generate_sprite_verts(sprite);
}	/* set_sprite_pos */

static void set_sprite_xyz(sprite_info_t *sprite, int x, int y, int z)
{
	sprite->x = x;
	sprite->y = y;
	sprite->z = z;
	/* force proper Z sort */
	change_sprite_alpha_state(sprite, 0);
	change_sprite_alpha_state(sprite, 1);
	generate_sprite_verts(sprite);
}	/* set_sprite_xyz */

static void set_sprite_color(sprite_info_t *sprite, long color)
{
	sprite->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
	sprite->state.constant_color = color;
}	/* set_sprite_color */

static void flash_sprite_proc(int *args)
{ 
	flash_sprite((sprite_info_t *)args[0], args[1], args[2], args[3]);
}	/* flash_sprite_proc */

static void flash_sprite(sprite_info_t *obj, int color, int cnt, int delay)
{
	int i, old_color, old_combine;
	
	old_combine = obj->state.color_combiner_function;
	old_color = obj->state.constant_color;
	for (i = 0; i < cnt; i++) {
		obj->state.color_combiner_function = GR_COLORCOMBINE_CCRGB;
		obj->state.constant_color = color;
		sleep(delay);
		obj->state.color_combiner_function = old_combine;
		obj->state.constant_color = old_color;
		sleep(delay);
	}
	obj->state.color_combiner_function = old_combine;
	obj->state.constant_color = old_color;
}	/* flash_sprite */

static void reset_fade_list(void)
{
	fade_sprite_list_index = 0;
}	/* reset_fade_list */

static void fade_sprite_list(int fade_dir, int slow_fade)
{
	long alpha;
	int i, j, step;
	ostring_t *str;
	sprite_node_t *slist;
	
	/* kill any glow sparkle before a fade */
	if (fade_dir == FADE_DOWN)
		do_glow_sparkle(NULL);
	
	/* when exiting do not do the fade effect */
	if (!creatp_active)
		return;
	
	/* fade the pin items through the name items */
	if ((fade_dir == FADE_UP_IMMED) || (fade_dir == FADE_DOWN_IMMED))
		step = 1;
	else
		step = slow_fade ? 16 : 8;
	for (j = 1; j <= step; j++) {
		alpha = ((256 / step) * j) - 1;
		if ((fade_dir == FADE_DOWN) || (fade_dir == FADE_DOWN_IMMED))
			alpha = 255 - alpha;
		/* the mode sprites */
		for (i = 0; i < fade_sprite_list_index; i++) {
			fade_sprite_list_sprites[i]->state.constant_color &= 0x00FFFFFF;
			fade_sprite_list_sprites[i]->state.constant_color |= alpha << 24;
		}
		/* the mode titles */
		for (i = 0; i < ARRAY_SIZE(creatp_title_list); i++) {
			creatp_title_list[i].sprite->state.constant_color &= 0x00FFFFFF;
			creatp_title_list[i].sprite->state.constant_color |= alpha << 24;
		}
		/* the instruction box */
		for (i = 0; i < ARRAY_SIZE(creatp_instruction_list); i++) {
			creatp_instruction_list[i].sprite->state.constant_color &= 0x00FFFFFF;
			creatp_instruction_list[i].sprite->state.constant_color |= alpha << 24;
		}
		/* the instruction strings */
		for (str = ostring_list; str != NULL; str = str->next)
			if ((str->id == BUTTON_INSTRUCTION_SID) || (str->id == ATTR_SID) || (str->id == PRIV_SID) || (str->id == STATS_SID))
				for (slist = alpha_sprite_node_list; slist != NULL; slist = slist->next)
					if (slist->si->parent == str) {
						slist->si->state.constant_color &= 0x00FFFFFF;
						slist->si->state.constant_color |= alpha << 24;
					}
		sleep(1);
	}
}	/* fade_sprite_list */

static void show_button_instruction(int button_number)
{
	static char *press_any_button = "PRESS ANY BUTTON";
	static char *button_instruction[CREATP_BUTTON_COUNT * 2] = {
		/* view stats */
		"TO SEE YOUR", "PLAYER\'S STATISTICS",
		/* head */
		"TO SELECT YOUR", "PLAYER\'S HEAD",
		/* number */
		"TO SELECT YOUR", "PLAYER\'S JERSEY NUMBER",
		/* attributes */
		"TO ADJUST YOUR", "PLAYER\'S ATTRIBUTES",
		/* privileges */
		"TO SELECT YOUR", "PLAYER\'S PRIVILEGES",
		/* nickname */
		"TO SELECT YOUR", "PLAYER\'S SPEECH CALL",
		/* edit name */
		"TO CHANGE YOUR", "PLAYER\'S NAME AND PIN",
		/* exit/save */
		"TO SAVE YOUR", "PLAYER AND EXIT"
	};
	int x_pos, y_pos, font_id;
	float x, y, h, fh;
	sprite_info_t *sprite;
	
	/* remove old button instruction */
	delete_multiple_strings(BUTTON_INSTRUCTION_SID, 0xFFFFFFFF);
	
	/* draw new button instruction */
	font_id = is_low_res ? 	FONTID_BAST8TA : FONTID_NBA10;
	set_text_font(font_id);
	set_string_id(BUTTON_INSTRUCTION_SID);
	set_text_justification_mode(HJUST_CENTER);
	set_text_color(OFF_YELLOW_COLOR);
	set_text_z_inc(-0.10F);
	
	fh = get_font_height(font_id) + (is_low_res ? 4 : 0);
	h = creatp_selected_button->selected_middle.image->h;
	
	sprite = creatp_selected_button->selected_middle.sprite;
	get_sprite_pos(sprite, &x_pos, &y_pos);
	
	x = x_pos + creatp_selected_button->selected_middle.image->w + 108.0f;
	y = y_pos - (h * 0.5F) - (fh * 0.5F);
	
	if (button_number == CREATP_VIEW_STAT_BUTTON)
		y -= fh * 0.5F;
	else if (button_number == CREATP_EXIT_SAVE_BUTTON)
		y += fh * 0.5F;
	
	/* line one */
	set_text_position(x, y + fh + (is_low_res ? 1 : 0), INSTRUCTION_Z);
	oprintf(press_any_button);
	
	/* line two */
	set_text_position(x, y, INSTRUCTION_Z);
	oprintf(button_instruction[button_number * 2]);
	
	/* line three */
	set_text_position(x, y - fh - (is_low_res ? 1 : 0), INSTRUCTION_Z);
	oprintf(button_instruction[(button_number * 2) + 1]);
	set_text_z_inc(0.0F);
	
	/* set up the background shadow plane */
	show_creatp_shadow(y + fh + fh + fh / 3, 0, 2, 0, 1);
}	/* show_button_instruction */

static void show_mode_instruction(int button_number)
{
	static char *mode_instruction[(CREATP_BUTTON_COUNT + MODE_COUNT) * 3] = {
		/* CREATP_VIEW_STAT_BUTTON */
		"BUTTONS: NEXT PAGE", NULL, NULL,
		/* CREATP_HEAD_BUTTON */
		"LEFT/RIGHT: SELECT HEAD", "BUTTONS: SAVE AND EXIT", NULL,
		/* CREATP_NUMBER_BUTTON */
		"JOYSTICK: MOVE CURSOR", "BUTTONS: SELECT NUMBER", NULL,
		/* CREATP_ATTRIBUTES_BUTTON */
		"UP/DOWN: SELECT ATTRIB", "LEFT/RIGHT: ADJUST ATTRIB", NULL,
		/* CREATP_PRIVILEGES_BUTTON */
		"UP/DOWN: SELECT PRIV", "LEFT/RIGHT: TOGGLE PRIV", "BUTTONS: SAVE AND EXIT",
		/* CREATP_NICKNAME_BUTTON */
		"UP/DOWN: SELECT NAME", "LEFT/RIGHT: PLAY SOUND", "BUTTONS: SAVE AND EXIT",
		/* CREATP_EDIT_NAME_BUTTON */
		"JOYSTICK: MOVE CURSOR", "BUTTONS: SELECT LETTER", NULL,
		/* CREATP_EXIT_SAVE_BUTTON */
		"DO YOU REALLY", "WANT TO EXIT", "CREATE PLAYER?",
		/* UNIFORM_JERSEY_MODE */
		"UP/DOWN: SELECT UNIFORM", "LEFT/RIGHT: SELECT STYLE", "BUTTONS: SAVE AND EXIT",
		/* EDIT_NAME_MODE */
		"DO YOU REALLY", "WANT TO CHANGE", "YOUR NAME AND PIN?",
		/* ENTER_NAME_MODE */
		"JOYSTICK: MOVE CURSOR", "BUTTONS: SELECT LETTER", NULL,
		/* ENTER_PIN_MODE */
		"JOYSTICK: MOVE CURSOR", "BUTTONS: SELECT NUMBER", NULL,
		/* HOTSPOT_MODE */
		"JOYSTICK: MOVE CURSOR", "BUTTONS: SELECT HOTSPOT", NULL,
	};
	float x, y, fh;
	int font_id;
	
	/* remove old button instruction */
	delete_multiple_strings(BUTTON_INSTRUCTION_SID, 0xFFFFFFFF);
	
	/* draw new button instruction */
	font_id = is_low_res ? 	FONTID_BAST8TA : FONTID_NBA10;
	set_text_font(font_id);
	set_string_id(BUTTON_INSTRUCTION_SID);
	set_text_justification_mode(HJUST_CENTER);
	set_text_color(OFF_YELLOW_COLOR);
	set_text_z_inc(-0.10F);
	
	fh = get_font_height(font_id) + (is_low_res ? 4 : 0);
	x = 246;
	
	if (button_number < 0)
		button_number = CREATP_EXIT_SAVE_BUTTON + -button_number;
	button_number *= 3;
	if (mode_instruction[button_number + 1] == NULL) {
		/* one line */
		y = SPRITE_VRES - 108;
		set_text_position(x, y + (is_low_res ? 1 : 0), INSTRUCTION_Z);
		oprintf("%s", mode_instruction[button_number]);
	} else if (mode_instruction[button_number + 2] == NULL) {
		/* two line */
		y = SPRITE_VRES - 109;
		set_text_position(x, y + (fh * 0.5f) + (is_low_res ? 4 : 0), INSTRUCTION_Z);
		oprintf("%s", mode_instruction[button_number]);
		set_text_position(x, y - (fh * 0.5f) + (is_low_res ? 2 : 0), INSTRUCTION_Z);
		oprintf("%s", mode_instruction[button_number + 1]);
	} else {
		/* three line */
		y = SPRITE_VRES - 111 - (fh * 0.5F);
		set_text_position(x, y + fh + 1 + (is_low_res ? 4 : 0), INSTRUCTION_Z);
		oprintf("%s", mode_instruction[button_number]);
		set_text_position(x, y + (is_low_res ? 3 : 0), INSTRUCTION_Z);
		oprintf("%s", mode_instruction[button_number + 1]);
		set_text_position(x, y - fh - (is_low_res ? 1 : 0), INSTRUCTION_Z);
		oprintf("%s", mode_instruction[button_number + 2]);
	}
}	/* show_mode_instruction */

static void hide_instruction(void)
{
	delete_multiple_strings(BUTTON_INSTRUCTION_SID, 0xFFFFFFFF);
}	/* hide_instruction */

static int next_button(int first_button, int last_button, int *current_button, int input, int wrap)
{
	int last, ret;
	
	last = *current_button;
	if (input == P_DOWN) {
		if (last == last_button) {
			if (wrap)
				last = first_button;
		} else
			last++;
	} if (input == P_UP) {
		if (last == first_button) {
			if (wrap)
				last = last_button;
		} else
			last--;
	}
	ret = last != *current_button;
	*current_button = last;
	return ret;
}	/* next_button */

static void wait_for_button(int min_time, int max_time)
{
	int sw;
	
	sleep(min_time);
	max_time -= min_time;
	while (max_time--) {
		sw = get_player_sw_current() >> player_mask;
		if ((sw & P_ABC_MASK) != 0) {
			snd_scall_bank(plyr_bnk_str, CURSOR_SND1, VOLUME4, 127, PRIORITY_LVL4);
			break;
		}
		sleep(1);
	}
}	/* wait_for_button */

static char *rank_suffix(int num)
{
	if (num == 1)
		return "ST";
	else if (num == 2)
		return "ND";
	else if (num == 3)
		return "RD";
	else
		return "TH";
}	/* rank_suffix */

#if defined(VEGAS)
static Texture_node_t	*player_model_textures[4];
#else
static GrMipMapId_t player_model_textures[4];
#endif

static void player_display_init(void)
{
	creatp_obj_data = &player_blocks[0];
	
	/* set the field of view value */
	save_fovval = fovval;
	fovval = 768.0f;
	
	/* allow custom uniforms */
	pup_team_jersey = 0;
	
	init_player_head_buffer(0, 600, 600, 1200, 400);
	
	*current_flesh_texture = '\0';
	*current_head_texture = '\0';
	*current_jersey_texture = '\0';
	*current_jnum_texture = '\0';
	
	update_player_display();
	qcreate("player", PLAYER_DISPLAY_PID, proc_player_display, 0, 0, 0, 0);
}	/* player_display_init */

static void player_display_exit(void)
{
	process_node_t *player_display_proc;
	
	player_display_proc = existp(PLAYER_DISPLAY_PID, 0xFFFFFFFF);
	if (player_display_proc != NULL) {
		kill(player_display_proc, PD_DIE);
		/* reset the field of view value */
		fovval = save_fovval;
	}
	
	unlock_multiple_textures(CREATP_CACHE_FLESH_TID, 0xFFFFFFFF);
	delete_multiple_textures(CREATP_CACHE_FLESH_TID, 0xFFFFFFFF);
	unlock_multiple_textures(CREATP_CACHE_HEAD_TID, 0xFFFFFFFF);
	delete_multiple_textures(CREATP_CACHE_HEAD_TID, 0xFFFFFFFF);
	unlock_multiple_textures(CREATP_PLAYER_FLESH_TID, 0xFFFFFFFF);
	delete_multiple_textures(CREATP_PLAYER_FLESH_TID, 0xFFFFFFFF);
	unlock_multiple_textures(CREATP_PLAYER_HEAD_TID, 0xFFFFFFFF);
	delete_multiple_textures(CREATP_PLAYER_HEAD_TID, 0xFFFFFFFF);
	unlock_multiple_textures(CREATP_PLAYER_JERSEY_TID, 0xFFFFFFFF);
	delete_multiple_textures(CREATP_PLAYER_JERSEY_TID, 0xFFFFFFFF);
	unlock_multiple_textures(CREATP_PLAYER_JNUM_TID, 0xFFFFFFFF);
	delete_multiple_textures(CREATP_PLAYER_JNUM_TID, 0xFFFFFFFF);	
}	/* player_display_exit */

#if defined(VEGAS)
static void load_a_player_texture(char *tex_name, int tex_id, Texture_node_t **tex_data, int wrap)
#else
static void load_a_player_texture(char *tex_name, int tex_id, GrMipMapId_t *tex_data, int wrap)
#endif
{
	struct texture_node *tex;
	int mode;
	
	
	mode = wrap ? GR_TEXTURECLAMP_WRAP : GR_TEXTURECLAMP_CLAMP;
	tex = create_texture(tex_name, tex_id, 0, CREATE_NORMAL_TEXTURE,
						 mode, mode,
						 GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
	if (tex == NULL) {
		increment_audit(TEX_LOAD_FAIL_AUD);
#ifdef DEBUG
		printf("load_a_player_texture failed!\n");
		lockup();
#endif
		*tex_data = NULL;
	} else {
		*tex_data = tex->texture_handle;
		guTexChangeLodBias(*tex_data, -1.25f);
	}
}	/* load_a_player_texture */

static void load_player_textures(void)
{
	player_textures tex;
	player_team team;
	struct texture_node *cache_tex;
	
	team.my_team = TEAM_ATL;
	team.opp_team = NO_OTHER_TEAM;
	if (creatp_player_data.cmos_data.privileges & PRIV_JERSEY) {
		team.jersey = creatp_player_data.cmos_data.jersey_city;
		team.jersey_style = creatp_player_data.cmos_data.jersey_style;
	} else {
		team.jersey = DEFAULT_UNIFORM_TEAM;
		team.jersey_style = AWAY_JERSEY;
	}
	get_player_textures(&team, creatp_player_data.cmos_data.team, creatp_player_data.cmos_data.player_index, &tex, TRUE);
	
	cache_tex = find_texture(tex.flesh_texture, CREATP_CACHE_FLESH_TID);
	if (cache_tex != NULL) {
		player_model_textures[PLAYER_TEXTURE_FLESH_INDEX] = cache_tex->texture_handle;
	} else {
		if (strcmp(current_flesh_texture, tex.flesh_texture) != 0) {
			unlock_multiple_textures(CREATP_PLAYER_FLESH_TID, 0xFFFFFFFF);
			delete_multiple_textures(CREATP_PLAYER_FLESH_TID, 0xFFFFFFFF);
			load_a_player_texture(tex.flesh_texture, CREATP_PLAYER_FLESH_TID, &player_model_textures[PLAYER_TEXTURE_FLESH_INDEX], TRUE);
			lock_multiple_textures(CREATP_PLAYER_FLESH_TID, 0xFFFFFFFF);
			strcpy(current_flesh_texture, tex.flesh_texture);
		} else {
			cache_tex = find_texture(tex.flesh_texture, CREATP_PLAYER_FLESH_TID);
			if (cache_tex != NULL)
				player_model_textures[PLAYER_TEXTURE_FLESH_INDEX] = cache_tex->texture_handle;
		}
	}
	
	cache_tex = find_texture(tex.head_texture, CREATP_CACHE_HEAD_TID);
	if (cache_tex != NULL) {
		player_model_textures[PLAYER_TEXTURE_HEAD_INDEX] = cache_tex->texture_handle;
	} else {
		if (strcmp(current_head_texture, tex.head_texture) != 0) {
			unlock_multiple_textures(CREATP_PLAYER_HEAD_TID, 0xFFFFFFFF);
			delete_multiple_textures(CREATP_PLAYER_HEAD_TID, 0xFFFFFFFF);
			load_a_player_texture(tex.head_texture, CREATP_PLAYER_HEAD_TID, &player_model_textures[PLAYER_TEXTURE_HEAD_INDEX], FALSE);
			lock_multiple_textures(CREATP_PLAYER_HEAD_TID, 0xFFFFFFFF);
			strcpy(current_head_texture, tex.head_texture);
		} else {
			cache_tex = find_texture(tex.head_texture, CREATP_PLAYER_HEAD_TID);
			if (cache_tex != NULL)
				player_model_textures[PLAYER_TEXTURE_HEAD_INDEX] = cache_tex->texture_handle;
		}
	}
	
	if (strcmp(current_jersey_texture, tex.jersey_texture) != 0) {
		unlock_multiple_textures(CREATP_PLAYER_JERSEY_TID, 0xFFFFFFFF);
		delete_multiple_textures(CREATP_PLAYER_JERSEY_TID, 0xFFFFFFFF);
		load_a_player_texture(tex.jersey_texture, CREATP_PLAYER_JERSEY_TID, &player_model_textures[PLAYER_TEXTURE_JERSEY_INDEX], TRUE);
		lock_multiple_textures(CREATP_PLAYER_JERSEY_TID, 0xFFFFFFFF);
		strcpy(current_jersey_texture, tex.jersey_texture);
	}
	
	if (strcmp(current_jnum_texture, tex.jnum_texture) != 0) {
		unlock_multiple_textures(CREATP_PLAYER_JNUM_TID, 0xFFFFFFFF);
		delete_multiple_textures(CREATP_PLAYER_JNUM_TID, 0xFFFFFFFF);
		load_a_player_texture(tex.jnum_texture, CREATP_PLAYER_JNUM_TID, &player_model_textures[PLAYER_TEXTURE_JERSEY_NUMBER_INDEX], TRUE);
		lock_multiple_textures(CREATP_PLAYER_JNUM_TID, 0xFFFFFFFF);
		strcpy(current_jnum_texture, tex.jnum_texture);
	}
}	/* load_player_textures */

static void init_player_pos(obj_3d *obj)
{
	creatp_obj_data->plyrnum = -1;
	obj->x = -21.0f;
	obj->y = -9.5f;
	obj->z = 90.0f;
	obj->fwd = 480;
	obj->tgt_fwd = 0;
	
	obj->ax = 0.0f;
	obj->ay = 0.0f;
	obj->az = 0.0f;
	obj->vx = 0.0f;
	obj->vy = 0.0f;
	obj->vz = 0.0f;
}	/* init_player_pos */

static void proc_player_display(int *args)
{
	char *buyin_stance_anims[] = {
		buyin_stance1_anim,
		buyin_stance2_anim,
		buyin_stance3_anim,
		buyin_stance4_anim
	};
	obj_3d *obj;
	
	/* get a pointer to the obj_3d for the player model */
	obj = (obj_3d *)creatp_obj_data;
	
	/* assign the player textures */
	obj->pdecal = player_model_textures;
	
	creatp_obj_data->dribble_mode      = DM_CAN_DRIBBLE;
	creatp_obj_data->no_ball_collide   = 0;
	creatp_obj_data->HangingOnRim      = NO;
	creatp_obj_data->auto_control      = NO;
	creatp_obj_data->defense_anim_wait = 0;
	creatp_obj_data->total_time        = 0;
	creatp_obj_data->flight_time       = 0;
	
	creatp_obj_data->stick_cur   = 24;
	creatp_obj_data->but_cur     = 0;
	obj->friction      = 0.0f;
	creatp_obj_data->attack_mode    = ATTMODE_NONE;
	creatp_obj_data->attack_time    = 0;
	creatp_obj_data->celebrate_time = 0;
	creatp_obj_data->misc_wait_time = -1;

	/* init the animation buffers */
	obj->adata[0].adata = creatp_obj_data->anidata[0];
	obj->adata[0].idata = creatp_obj_data->anidata[1];
	obj->adata[1].adata = creatp_obj_data->anidata[2];
	obj->adata[1].idata = creatp_obj_data->anidata[3];
	obj->adata[2].adata = creatp_obj_data->anidata[4];
	obj->adata[3].adata = creatp_obj_data->anidata[5];
	obj->adata[1].animode = MODE_SECONDARY;
	
	/* position the model in the world */
	init_player_pos(obj);
	
	/* set the animation */
	change_anim(obj, s_breath1_anim);
	animate_model(obj);
	change_anim(obj, buyin_stance_anims[randrng(ARRAY_SIZE(buyin_stance_anims))]);

	/* calculate the first frame */
	adjust_player(obj);
	animate_model(obj);
	obj->flags |= PF_ANITICK;
	for (;;) {
		/* adjust the player model */
		adjust_player(obj);
		
		/* animate the player */
		if ((obj->flags & PF_ANITICK) == 0)
			animate_model(obj);
		
		/* draw the player */
		player_pre_draw();
		draw_player(obj);
		sleep(1);
		obj->flags &= ~PF_ANITICK;
	}
}	/* proc_player_display */

static void update_player_display(void)
{
	int model;
	
	load_player_head(TRUE, 0, creatp_player_data.cmos_data.team,
					 creatp_player_data.cmos_data.player_index,
					 &creatp_obj_data->head_model, &creatp_obj_data->f_headsc);

	valid_head = (creatp_obj_data->f_headsc != 0.0f);

	if (creatp_player_data.cmos_data.team == TEAM_SECRET)
		model = creatp_player_data.cmos_data.player_index;
	else
		model = NBA_PLAYER;
	load_player_model(model, &creatp_obj_data->body_model, &creatp_obj_data->jnum_model, &creatp_obj_data->reflect_model);
	load_player_textures();
}	/* update_player_display */

static void player_pre_draw(void)
{
	/* init states for glide */
	guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);
	grTexCombineFunction(0, GR_TEXTURECOMBINE_DECAL);
	grTexClampMode(0, GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP);
	grDepthBufferMode(GR_DEPTHBUFFER_WBUFFER);
	grDepthMask(FXTRUE);
	grChromakeyMode(GR_CHROMAKEY_DISABLE);
	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
	grAlphaBlendFunction(GR_BLEND_SRC_ALPHA, GR_BLEND_ONE_MINUS_SRC_ALPHA,
						 GR_BLEND_SRC_ALPHA, GR_BLEND_ONE_MINUS_SRC_ALPHA);
	grDepthBufferFunction(GR_CMP_LESS);
	grDepthBiasLevel(0);
	grConstantColorValue(255 << 24);
}	/* player_pre_draw */

static void adjust_player(obj_3d *obj)
{
	char height;
	short weight;
	int i;
	
	creatp_calc_height_weight(creatp_player_data.cmos_data.height,
							  creatp_player_data.cmos_data.weight, &height, &weight);
	calc_player_scale(TRUE, creatp_player_data.cmos_data.height, height, creatp_player_data.cmos_data.weight, &obj->zscale, &obj->yscale);
	creatp_obj_data->f_headsc = calc_player_head_scale(TRUE, creatp_player_data.cmos_data.height, &creatp_player_data.cmos_data);
	obj->xscale = obj->zscale;
	obj->ascale = obj->yscale;
}	/* adjust_player */

static void draw_player(obj_3d *obj)
{
	jonhey_t jh_obj;
	int j1, j2;
	float xlate[12] = {1.0f, 0.0f, 0.0f, 0.0f,
					   0.0f, 1.0f, 0.0f, 0.0f,
					   0.0f, 0.0f, 1.0f, 0.0f};
	float rotate[12] = {1.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 1.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 1.0f, 0.0f};
	float cm[12];
	float head_scale;
	
	xlate[3] = obj->x;
	xlate[7] = obj->y + obj->ay;
	xlate[11] = obj->z;
	roty(obj->fwd, rotate);
#ifdef ROTATE_PLAYER
	obj->fwd += 2;
	if (obj->fwd > 1024)
		obj->fwd = 0;
#endif
	mxm(xlate, rotate, cm);
	cm[0] *= obj->xscale; cm[1] *= obj->yscale; cm[ 2] *= obj->zscale;
	cm[4] *= obj->xscale; cm[5] *= obj->yscale; cm[ 6] *= obj->zscale;
	cm[8] *= obj->xscale; cm[9] *= obj->yscale; cm[10] *= obj->zscale;
	
	head_scale = creatp_obj_data->f_headsc;
	if ((creatp_player_data.cmos_data.privileges & PRIV_BIG_HEAD) != 0)
		head_scale *= bighead_scale(creatp_player_data.cmos_data.team, creatp_player_data.cmos_data.player_index);

	/* insert the head limb into the model */
	if( valid_head )
		creatp_obj_data->body_model[JOINT_NECK] = creatp_obj_data->head_model;
	else
		creatp_obj_data->body_model[JOINT_NECK] = creatp_obj_data->body_model[JOINT_PELVIS];
	
	generate_matrices(bbplyr_skel, cm, obj->adata[0].pq, creatp_obj_data->matrices[0],
					  creatp_obj_data->jpos[0], head_scale, 0);
	
	/* render the player */
	render_node_mpc(bbplyr_skel, creatp_obj_data->body_model, creatp_obj_data->matrices[0], obj->pdecal, creatp_obj_data->jpos[0]);
	
	/* render the jersey numbers */
	grDepthBiasLevel(-8L);
	
	j1 = (unsigned char)creatp_player_data.cmos_data.jersey_num >> 4;
	j2 = creatp_player_data.cmos_data.jersey_num & 0x0F;
	grTexClampMode(0, GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP);
	if (creatp_obj_data->jnum_model != NULL) {
		if ((j1 != 0) || (j2 == 0)) {
			convert_2_jonhey (&jh_obj, creatp_obj_data->jnum_model[1], 
							  creatp_obj_data->matrices[JOINT_TORSO], 
							  &obj->pdecal[PLAYER_TEXTURE_JERSEY_NUMBER_INDEX]);
			render_limb_jers(&jh_obj, j2, j1);
		} else {
			convert_2_jonhey (&jh_obj, creatp_obj_data->jnum_model[0], 
							  creatp_obj_data->matrices[JOINT_TORSO], 
							  &obj->pdecal[PLAYER_TEXTURE_JERSEY_NUMBER_INDEX]);

			render_limb_jers(&jh_obj, j2, j2);
		}
	}
	grTexClampMode(0, GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP);
}	/* draw_player */
