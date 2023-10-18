//******************************************************************
//
// DESCRIPTION: Enter Initials and Select teams code for FOOTBALL
// AUTHOR: Jeff Johnson
// HISTORY: Started Sept 29. 1996
// $Revillon: 16
// $Archive: /video/Nba/select.c $
// $Modtime: 10/14/99 6:17p $
// *******************************************************************

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif
#include <assert.h>
#include <math.h>
#include <dir.h>

#include "fontid.h"
#include "id.h"
#include "game.h"
#include "ani3D.h"
#include "sndcalls.h"
#include "select.h"
#include "nbacmos.h"
#include "externs.h"
#include "coin.h"
#include "player.h"
#include "audits.h"
#include "gameadj.h"
#include "creatp.h"
#include "movie.h"
#if 0
#include "stream.h"
#endif
#include "transit.h"
#include "putil.h"

#include "game_sel.h"

#include "amode.h"

#define	IMAGEDEFINE	1
#include "optscrn.h"
#include "tmsel.h"
#include "inits.h"
#include "selplr.h"
#include "eoq.h"
#include "joinin.h"

void update_other_cmos_to_disk(int sleep_allowed);
int set_coin_hold(int flag);
int trivia_contest_init(void);
void trivia_contest(void);

void change_alpha_cc(void * parent, int cc);

//int	whois_teammate(int pnum);
int count_quarters_purchased(void);
int count_players_in_game(void);
//int read_plays_hd( int, int, cap_play_t * );
int	check_credits_to_start(void);
void do_team_percent_aud(int);
void disc_flash(sprite_info_t *obj, int tid, float xoff, float yoff, float scale,int val);
int print_player_record_stats(int,int,int);
void print_team_rosters(int, int);

extern char index_2p[NUM_PLAYERS];
extern char shift_2p[NUM_PLAYERS];

extern int which_court;
extern int game_just_finished;
extern int vs_page_showing;
extern int record_me;									// Flag bits of players to record to cmos
extern int	frame_factor;
extern int pup_no_fouls, pup_tourney;

extern int pintx_turbo[MAX_PLYRS][2];
extern int pinty_turbo[][2];
//extern int cap_pnum;

//extern int show_plysel_idiot_box;

extern int free_game_count[2];

extern void reinit_player_data(int, int);

extern int calc_tms_def(int tms_def);

extern int randrng(int);
extern int full_game_credits;
extern void spawn_sparkles_proc(int *args);
extern image_info_t playresult;
extern float idiot_box_xys[][2];
extern void eoq_music_handler(void);
extern void do_team_percent_aud(int);
extern int	is_low_res;

extern int reload;

extern int qtr_pstatus[];

extern char game_tune_bnk_str[];
extern char buyin_bnk_str[];
extern char eoq_tune_bnk_str[];
extern const int team_ladder[][5];
extern const int beat_all_teams[];
extern char eog_movie_snd_bnk_str[];
extern char nbc_logo_movie_snd_bnk_str[];
extern char midway_logo_movie_snd_bnk_str[];
extern char showtime_movie_snd_bnk_str[];
extern char trophy_snd_bnk_str[];

//extern char screen_trans_bnk_str[];

//#if defined(VEGAS)
//extern Texture_node_t	*all_teams_decals[NUM_TEAMS][4];
//extern Texture_node_t	*misc_decals[4];
//#else
//extern GrMipMapId_t	all_teams_decals[NUM_TEAMS][4];
//extern GrMipMapId_t	misc_decals[4];
//#endif

extern player_record_info selected_players[];


extern image_info_t pflasha01,pflasha02,pflasha03,pflasha04,pflasha05,pflasha06,pflasha07,pflasha08,pflasha09,pflasha10;

extern image_info_t nbcbk1,nbcbk1_c1,nbcbk1_c2,nbcbk1_c3;

extern int team_audits[];				// from audinfo.c
extern int showhide_flag;

extern void unload_player_speech_banks(void);
extern void load_player_speech_banks(void);

void flash_foul_meter_proc(int *args);

void show_final_player_stats(void);
void game_over_screen(void);
int pick_cpu_team(int, int);
static void pick_cpu_roster(int team, int base_index);
static void find_best_players(int team, int *p1_index, int *p2_index);
void find_all_star_team(int *p1_team, int *p1_index, int *p2_team, int *p2_index);
void launch_game_proc(void);
void load_ingame_textures(void);
void draw_backdrop(void);
void draw_backdrop_flip(void);
void flash_text_proc(int *args);
void flash_alpha_text_proc(int *args);
void flash_text_slow_proc(int *args);
void get_ready_for(void);
void wait_for_any_butn_press_active_plyr(int, int, int);
void wait_for_any_butn_press(int, int);
void plyr_stat_award_msgs(int *args);
void start_enable(int);
void show_player_stats(int);
//int teammate_in_tmsel(int p_num);
int find_best_total(int * num);
int return_best_total(int * num);
int return_best_percent(int * num, int * den);

void disc_flash_proc(int *args);
void disc_flash_3pt_proc(int *args);
void option_screen(int);
void plyr_select_court_proc(int *args);
void plyr_select_option_proc( int *args);
void plyr_joinin_proc(int *args);
void lighting_proc(int *args);
void update_roster_stats(float statbx_x, float statbx_y, struct stat_plyr_data *stat_ptr, int selected_player_array_index);
static void hidden_attrib_flasher(int *args);
static unsigned long xlate_color(unsigned char color);
static unsigned long cycle_color(unsigned char *color_table, int table_size, int *table_index);

static void whoosh_proc(int *args);

static void load_mugshot_tpage(void *pii_mug);
static void stick_court(void);
static void jordan_check(void);
static void bump_stat(char *stat);

int alt_ladder_active;
int ladder_level;
int cpu_team;
int	perspective_correct = 1;
int	reset_on_coin = 0;
//int	got_coin = 0;

// defines
#define MENU_LEVELS		4		// yes no, enter intials, enter pin number, team select
#define	YESNO			0
#define	INITS			1
#define	PIN_NBR			2
#define	TM_SELECT		3

#define	STATS_PER_PAGE			6
#define	NUM_STATS				12


#define	NUM_ON_ROSTER			9					// doesn't include linemen
#define	NUM_CITY_NAMES			9					// 4 on top, middle, 4 on bottom
#define	NUM_LIMBS				10

#define	LTR_COLS				5	//4
#define	LTR_ROWS				6	//8
#define	PIN_COLS				3
#define	PIN_ROWS				4

// offsets into PLAYER SELECT 'side' object ptrs. array
#define	BORDER_HDR_OBJ_PTR			0
#define	BORDER_HDR_GLOW_OBJ_PTR		1
#define	BORDER_HDR_NME_OBJ_PTR		2
#define	BORDER_OBJ_PTR				3
#define	BORDER_BKDRP_OBJ_PTR		4
#define	MUGSHOT_OBJ_PTR				5
#define	STATBOX_OBJ_PTR				6
#define	SWOOSH_OBJ_PTR				7
#define	NAME_BACKDROP_OBJ_PTR		8

// offsets into TEAM SELECT 'side' object ptrs. array
#define	TM_LOGO_OBJ_PTR				0
#define	TM_CITY1_OBJ_PTR			1
#define	TM_CITY2_OBJ_PTR			2
#define	TM_CITY3_OBJ_PTR			3
#define	TM_CITY4_OBJ_PTR			4	// center name
#define	TM_CITY5_OBJ_PTR			5
#define	TM_CITY6_OBJ_PTR			6
#define	TM_CITY7_OBJ_PTR			7
#define	TM_NME1_OBJ_PTR				8
#define	TM_NME2_OBJ_PTR				9
#define	TM_NME3_OBJ_PTR				10
#define	TM_NME4_OBJ_PTR				11
#define	TM_NME5_OBJ_PTR				12
#define TM_LOGO_PLATE_PTR			13
#define TM_LOGO_PLATE2_PTR			14
#define	TM_ROSTER_BAR_PTR			15
#define	TM_ROSTER_STR_PTR			16
#define	TM_ROSTER_BKDRP_PTR			17
#define	TM_CITY_PLAQ_PTR			18
#define	TM_NAME_HILITE_PTR			19
#define	TM_PLQ_WING_PTR				20
//#define	TM_NAME_HILITE2_PTR			20		// piece 2 of hilite graphic




// joystick repeat and delay counts
#define	TICKS_BEFORE_START_REPEAT		25		//35
#define	TICKS_BETWEEN_REPEATS				4			//5

// background scroll defines
#define	NUM_BKGRND_CHUNKS			4		//5
#define	NUM_PIECES_IN_CHUNK			2

#define	NUM_HLFTME_STATS			10

#define OVR_RNK		0						// indexes into the 'plr_ranks' table
#define	OFF_RNK		1
#define	STK_RNK		5
#define	DEF_RNK		7


// extern variables
extern int get_total_credits(void);
extern game_info_t game_info;
extern game_info_t game_info_last;
//extern struct process_node	*cur_proc;

extern int bought_in_first;
extern int tsec;
extern int p_status;
extern int p_status2;
extern pdata_t pdata_blocks[];
//extern tdata_t tdata_blocks[];				// team bookeeping structures (2)
//extern tdata_t tdata_blocks_last[];			// team bookeeping structures (2)
extern plyr_record_t	*rec_sort_ram;			// pointer to the array of PLAYER RECORDs from cmos into ram

extern char intro_bnk_str[];
extern char plyr_bnk_str[];
extern char cursor_bnk_str[];
extern char anc_make_bnk[];

//extern LIMB limb_num_3d_0,limb_num_3d_1,limb_num_3d_2,limb_num_3d_3,limb_num_3d_4;
//extern LIMB limb_num_3d_5,limb_num_3d_6,limb_num_3d_7,limb_num_3d_8,limb_num_3d_9;

extern int show_them_hiscore_tables_first;
int get_credits(void);

//extern image_info_t cheerpg1,cheerpg1_c1,cheerpg1_c2,cheerpg1_c3;
//extern image_info_t cheerpg2,cheerpg2_c1,cheerpg2_c2,cheerpg2_c3;
//extern image_info_t cheerpg3,cheerpg3_c1,cheerpg3_c2,cheerpg3_c3;
//extern image_info_t cheerpg4,cheerpg4_c1,cheerpg4_c2,cheerpg4_c3;



// variables
extern int four_plr_ver;					// 0 = 2 plyrs, 1 = 4 plyrs

int pushed_cnt[NUM_PLAYERS];

plyr_record_t		player1_data;
plyr_record_t		player2_data;
plyr_record_t		player3_data;
plyr_record_t		player4_data;

// for keeping game stats
plyr_record_t		player1_sdata;
plyr_record_t		player2_sdata;
plyr_record_t		player3_sdata;
plyr_record_t		player4_sdata;

int			option;
short		exit_status;
short		showing_awards;
short		force_selection;							// !=0 force cursor to current choice
short		load_failed;
short		loading_textures;
short		players;
//short		screen;
short		flash_status;
volatile int imgs_loaded;
int was_awarded_attr[4];

volatile int loading_mug_textures;

int is_guest[NUM_PLAYERS];
static int hidden_attrib;

char * game_tunes_tbl[]  =
{
	"tungangs", "tunextrm", "tundmjr4", "tuneurof",
	"tungrupn", "tunhappy", "tuncrea2"
};

char * game_outdoor_tunes_tbl[]  =
{
	"tungruv1", "tuncrea1", "tungrupn", "tunoutdr",
	"tundmjr4", "tunhappy", "tuncrea2"
};

//volatile int uniforms_loaded;
//volatile int uniform_loading;

//static obj3d_data_t nbr_3d_obj;

typedef struct coach_msg {
	char * msg;
	float  yoff;
} coach_msg_t;


//
// Tables
//
//static LIMB *nbr_3d_limbs[] = {&limb_num_3d_0,&limb_num_3d_1,&limb_num_3d_2,&limb_num_3d_3,&limb_num_3d_4,
//															 &limb_num_3d_5,&limb_num_3d_6,&limb_num_3d_7,&limb_num_3d_8,&limb_num_3d_9};

plyr_record_t  *plyr_data_ptrs[] = {&player1_data,&player2_data,&player3_data,&player4_data};
plyr_record_t  *plyr_stat_ptrs[] = {&player1_sdata,&player2_sdata,&player3_sdata,&player4_sdata};

//
// ** NOTE **  all points are based on MEDIUM RES.
//
//static float stat_pg_name_xys[MAX_PLYRS][2][2] = {		// [plyr_num][2plr/4plr][x,y]
//	{{ 66.0F,206.0F},{ 66.0F,206.0F}},					// player 1
//	{{449.0F,206.0F},{449.0F,206.0F}},					// player 2
//	{{  0.0F,0.0F},  {  0.0F, 0.0F}},					// player 3
//	{{  0.0F,0.0F},  {  0.0F, 0.0F}}					// player 4
//	};

float title_bar_xys[2][2][2] = {				// [res][objects][x,y]
	{{(SPRITE_HRES/2.0f)-21.0f, SPRITE_VRES-35.0f},{(SPRITE_HRES/2.0f)-21.0f, SPRITE_VRES-35.0f}},		// res..title..shadow xys
	{{(SPRITE_HRES/2.0f)-21.0f, SPRITE_VRES-35.0f},{(SPRITE_HRES/2.0f)-21.0f, SPRITE_VRES-35.0f}}};

float buyin_title_xys[2][2][2] = {				// [res][objects][x,y]
	// low res
	{{(SPRITE_HRES/2.0f)+0.0f, SPRITE_VRES-70.0f},			// title
	{(SPRITE_HRES/2.0f)-0.0f, (SPRITE_VRES/2.0f)-30.0f}},	 // box
	// mid res
	{{(SPRITE_HRES/2.0f)+0.0f, SPRITE_VRES-70.0f},			// title
	{(SPRITE_HRES/2.0f)-0.0f, (SPRITE_VRES/2.0f)-30.0f}},	 // box
	};

static float option_xys[2][3][2] = {				// [res][objects][x,y]
	// low res
	{{SPRITE_HRES/2.0f-168.0f,SPRITE_VRES/2.0f}, {SPRITE_HRES/2.0f, SPRITE_VRES/2.0f}, {SPRITE_HRES/2.0f+168.0f, SPRITE_VRES/2.0f}},
	// mid res
	{{SPRITE_HRES/2.0f-168.0f,SPRITE_VRES/2.0f}, {SPRITE_HRES/2.0f, SPRITE_VRES/2.0f}, {SPRITE_HRES/2.0f+168.0f, SPRITE_VRES/2.0f}}
	};


static float plaq_xs[MAX_PLYRS][2] = {			// [plyr_num][2p,4p]
	{128.0F, 64.0f},									// player 1
	{  0.0F,192.0f},									// player 2
	{384.0F,320.0F},									// player 3
	{  0.0F,448.0F},									// player 4
};

float japbx_cntr_xys[MAX_PLYRS][2][2] = {		// [plyr_num][2/4 plr][x,y]
	{{128.0F,48.0F},{ 68.0f,48.0F}},					// player 1
	{{  0.0F,48.0F},{192.0f,48.0F}},					// player 2
	{{384.0F,48.0F},{320.0F,48.0F}},					// player 3
	{{  0.0F,48.0F},{444.0F,48.0F}}						// player 4
	};

 float station_xys[MAX_PLYRS][2][2] = {			// [plyr_num][2plr/4plr][x,y]
 	{{128.0F,192.0F},{ 68.0f,192.0F}},					// player 1
 	{{  0.0F,192.0F},{192.0f,192.0F}},					// player 2
 	{{384.0F,192.0F},{320.0F,192.0F}},					// player 3
 	{{  0.0F,192.0F},{444.0F,192.0F}}					// player 4
 	};

static float credit_msg_xys[][2][2] = {			// [screen nbr][res][x,y]
	{{279.0F,7.0F},{279.0F,(5.0f *3.0f/2.0f)}},			// name entry screen
	{{279.0F,7.0F},{279.0F,(5.0f *3.0f/2.0f)}},			// team selection screen
	{{279.0F,7.0F},{279.0F,(5.0f *3.0f/2.0f)}},			// vs screen
	{{279.0F,7.0F},{279.0F,(5.0f *3.0f/2.0f)}},			// buy-in screen
	{{279.0F,7.0F},{279.0F,(5.0f *3.0f/2.0f)}},			// hhalftime screen
	{{279.0F,7.0F},{279.0F,(5.0f *3.0f/2.0f)}},			// play select screen
	};

float logo_xys[][2][2][2] = {				// [screen nbr][side0 or 1][res][x,y]
	{{{  0.0F,    0.0F},  {  0.0F,   0.0F}},   {{  0.0F,    0.0F},    {0.0F,    0.0F}}},	// name entry screen
	{{{128.0F,  115.0F},  {128.0F, 115.0F}},   {{384.0F,  115.0F},  {384.0F,  115.0F}}},	// team selection screen
	{{{128.0F,  225.0F},  {256.0F, 225.0F}},   {{  0.0F,    0.0F},  {  0.0F,    0.0F}}},	// vs screen
	{{{  0.0F,    0.0F},  {  0.0F,   0.0F}},   {{  0.0F,    0.0F},  {  0.0F,    0.0F}}},	// buy-in screen
	{{{ 64.0F,  272.0F},  { 64.0F,  272.0F}},  {{445.0F,  272.0F},  {445.0F,  272.0F}}},	// halftime screen
	{{{ 49.0F+6,340.0F-6},{ 49.0F+6,340.0F-6}},{{448.0F-6,340.0F-6},{448.0F-6,340.0F-6}}},	// play select screen
	{{{ 49.0F+6,293.0F-6},{ 49.0F+6,293.0F-6}},{{446.0F-6,293.0F-6},{446.0F-6,293.0F-6}}}	// Coaching tips screen
	};

static int table_sizes[MENU_LEVELS][2][2] = {			// [menu_level],[2plr/4plr],[width,height]
	{{0,0},					{0,0}},						//  yes no layout
	{{LTR_COLS,LTR_ROWS},	{LTR_COLS,LTR_ROWS}},		//  enter inits layout
	{{PIN_COLS,PIN_ROWS},	{PIN_COLS,PIN_ROWS}},		//  pin nbr. layout
	{{1,NUM_TEAMS}, 		{1,NUM_TEAMS}}				//  team select
	};

#define	Y 280.0F
static float city_names_xs[] = {189.0f,317.0f};

static float city_names_ys[][7] = {
	{285.0F, 269.0F, 253.0F, 233.0F, 212.0F, 196.0F, 180.0F}, 
	{193.0F, 181.0F, 169.0F, 155.0F, 140.0F, 128.0F, 116.0F}
};


int fullgame_bought_ok = 0;		// set only when its OK to show the plaqs



//static float stat_title_ys[MAX_PLYRS] = {360.0F, 360.0F, 360.0F, 360.0F};

static int plr_ranks[MAX_PLYRS][NUM_STATS];						// [plyr num][rank value for each stat]

#define	Y_SPC		37.0f //44.0F
#define	ST_Y1		318.0f //335.0F
#define	ST_Y2		ST_Y1-Y_SPC
#define	ST_Y3		ST_Y2-Y_SPC
#define	ST_Y4		ST_Y3-Y_SPC
#define	ST_Y5		ST_Y4-Y_SPC
#define	ST_Y6		ST_Y5-Y_SPC
//#define	ST_Y2		ST_Y1-(1*Y_SPC)
//#define	ST_Y3		ST_Y1-(2*Y_SPC)
//#define	ST_Y4		ST_Y1-(3*Y_SPC)
//#define	ST_Y5		ST_Y1-(4*Y_SPC)
//#define	ST_Y6		ST_Y1-(5*Y_SPC)

static float stat_plqs_xys[][STATS_PER_PAGE][MAX_PLYRS][2] = {	//[2p,4p][stat#][plyr#][x,y]
	{
	// 2 player ver.
		{ {128.0F,ST_Y1}, {  0.0F, 0.0F}, {384.0F,ST_Y1}, {  0.0F, 0.0F} },	//RANK      | PTS. SCORED
		{ {128.0F,ST_Y2}, {  0.0F, 0.0F}, {384.0F,ST_Y2}, {  0.0F, 0.0F} },	//RECORD    | PTS. ALLOWED
		{ {128.0F,ST_Y3}, {  0.0F, 0.0F}, {384.0F,ST_Y3}, {  0.0F, 0.0F} },	//OFF. RANK | 3 PT. SHOTS
		{ {128.0F,ST_Y4}, {  0.0F, 0.0F}, {384.0F,ST_Y4}, {  0.0F, 0.0F} },	//DEF. RANK | REBOUNDS
		{ {128.0F,ST_Y5}, {  0.0F, 0.0F}, {384.0F,ST_Y5}, {  0.0F, 0.0F} },	//STREAK    | STEALS
		{ {128.0F,ST_Y6}, {  0.0F, 0.0F}, {384.0F,ST_Y6}, {  0.0F, 0.0F} },	//DEFEATED  | ASSISTS
	},
	{
	// 4 player ver.
		{ { 64.0F,ST_Y1}, {192.0F,ST_Y1}, {320.0F,ST_Y1}, {448.0F,ST_Y1} },	//
		{ { 64.0F,ST_Y2}, {192.0F,ST_Y2}, {320.0F,ST_Y2}, {448.0F,ST_Y2} },	//
		{ { 64.0F,ST_Y3}, {192.0F,ST_Y3}, {320.0F,ST_Y3}, {448.0F,ST_Y3} },	//
		{ { 64.0F,ST_Y4}, {192.0F,ST_Y4}, {320.0F,ST_Y4}, {448.0F,ST_Y4} },	//
		{ { 64.0F,ST_Y5}, {192.0F,ST_Y5}, {320.0F,ST_Y5}, {448.0F,ST_Y5} },	//
		{ { 64.0F,ST_Y6}, {192.0F,ST_Y6}, {320.0F,ST_Y6}, {448.0F,ST_Y6} },	//
	}
};

static float stat_plqs_yoff1[][2][STATS_PER_PAGE] = {			//[2p,4p][med,low][stat#]
	{
	// 2 player ver.
		{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
		{ 2.0f, 5.0f, 2.0f, 2.0f, 1.0f, 2.0f },
	},
	{
	// 4 player ver.
		{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
		{ 2.0f, 3.0f, 2.0f, 2.0f, 3.0f, 2.0f },
	}
};


static float stat_plqs_yoff2[][2][STATS_PER_PAGE] = {			//[2p,4p][med,low][stat#]
	{
	// 2 player ver.
		{ 18.0f, 18.0f, 18.0f, 18.0f, 18.0f, 18.0f },
		{ 19.0f, 19.0f, 19.0f, 19.0f, 19.0f, 19.0f },
	},
	{
	// 4 player ver.
		{ 17.0f, 17.0f, 17.0f, 17.0f, 17.0f, 17.0f },
		{ 17.0f, 17.0f, 17.0f, 17.0f, 17.0f, 17.0f },
	}
};

#undef		ST_Y
#define		ST_Y		320.0F

static float stat_plq_msg_xys[][MAX_PLYRS][2] = {			//[2p,4p][plyr#][x,y]
	{ {65.0F,ST_Y}, {  0.0F,0.0F}, {325.0F,ST_Y}, {  0.0F,0.0F} },
	{ {65.0F,ST_Y}, {160.0F,ST_Y}, {280.0F,ST_Y}, {480.0F,ST_Y} },
};
				
#undef		ST_Y
#define		ST_Y 	147.0F  //315.0f
#define		Y_SPC2	14.0f	//20.0F
#define		ST_YL 	149.0F
#define		Y_SPCL2	15.0f

static float tm_stat_pg_name_xs[][MAX_PLYRS] = { {79.0f,182.0F,330.0f,431.0F}, {79.0f,182.0F,330.0f,431.0F} };

static float tm_stat_pg_stats_xs[][MAX_PLYRS] = { {79.0f,183.0F,323.0f,432.0F}, {79.0f,183.0F,323.0f,432.0F} };


static float tm_stat_pg_stats_ys[2][NUM_HLFTME_STATS] = {	//[med,low][stat#]
	{   ST_Y-Y_SPC2*0,   ST_Y-Y_SPC2*1,   ST_Y-Y_SPC2*2,   ST_Y-Y_SPC2*3,   ST_Y-Y_SPC2*4,
	    ST_Y-Y_SPC2*5,   ST_Y-Y_SPC2*6,   ST_Y-Y_SPC2*7,   ST_Y-Y_SPC2*8,   ST_Y-Y_SPC2*9 },
	{ ST_YL-Y_SPCL2*0, ST_YL-Y_SPCL2*1, ST_YL-Y_SPCL2*2, ST_YL-Y_SPCL2*3, ST_YL-Y_SPCL2*4,
	  ST_YL-Y_SPCL2*5, ST_YL-Y_SPCL2*6, ST_YL-Y_SPCL2*7, ST_YL-Y_SPCL2*8, ST_YL-Y_SPCL2*9 },
};

//static int tm_stat_pg_stats_js[] = {HJUST_RIGHT | VJUST_CENTER, HJUST_LEFT | VJUST_CENTER};
																				
//static float stat_pg_scrs_xys[2][2][2] = {
//	{{66.0F,168.0F}, {449.0F,168.0F}},
//	{{66.0F,186.0F}, {449.0F,186.0F}}};
			
//#define	XST		140.0F			
//#define	XSP		42.0F
//static float stat_pg_scr_sprd_xs[] = {30.0F,184.0F,227.0F,270.0F,313.0F,356.0F,399.0F,445.0F};
//static float stat_pg_scr_sprd_ys[][2] = {{58.0F,35.0F},{57.0F,35.0F}};
			

#define FB_Y	17.0f
#define FB_D	19.0f
static float credit_fb_flsh_xys[][MAX_PLYRS][5][2] = {		// [2p,4p][plyr#][qtr#][x,y]
	{
		{ {0.0F, 0.0F}, { 99.0F, FB_Y}, {118.0F, FB_Y}, {136.0F, FB_Y}, {154.0F, FB_Y} },	// plyr 1 (2 plyr ver)
		{ {0.0F, 0.0F}, {  0.0F, 0.0F}, {  0.0F, 0.0F}, {  0.0F, 0.0F}, {  0.0F, 0.0F} },	// plyr 2
		{ {0.0F, 0.0F}, {355.0F, FB_Y}, {374.0F, FB_Y}, {392.0F, FB_Y}, {410.0F, FB_Y} },	// plyr 3
		{ {0.0F, 0.0F}, {  0.0F, 0.0F}, {  0.0F, 0.0F}, {  0.0F, 0.0F}, {  0.0F, 0.0F} },	// plyr 4
	},
	{
		{ {0.0F, 0.0F}, { 35.0F, FB_Y}, { 54.0F, FB_Y}, { 73.0F, FB_Y}, { 92.0F, FB_Y} },	// plyr 1 (4 plyr ver)
		{ {0.0F, 0.0F}, {160.0F, FB_Y}, {179.0F, FB_Y}, {198.0F, FB_Y}, {217.0F, FB_Y} },	// plyr 2
		{ {0.0F, 0.0F}, {290.0F, FB_Y}, {309.0F, FB_Y}, {328.0F, FB_Y}, {347.0F, FB_Y} },	// plyr 3
		{ {0.0F, 0.0F}, {415.0F, FB_Y}, {434.0F, FB_Y}, {453.0F, FB_Y}, {472.0F, FB_Y} },	// plyr 4
	}
};

//static float stats_bckdrp_xys[][MAX_PLYRS][2][2] = {
//		 // two players
//	 {{{ 69.0F,330.0F},{123.0F,330.0F}},		// plyr 1
//	  {{328.0F,330.0F},{382.0F,330.0F}},		// plyr 2
//	  {{  0.0F,  0.0F},  {0.0F,  0.0F}},
//	  {{  0.0F,  0.0F},  {0.0F,  0.0F}}},
//		 // four players 
//	 {{{ 59.0F,330.0F},{0.0f,0.0F}},		// plyr 1
//	  {{187.0F,330.0F},{0.0f,0.0F}},		// plyr 2
//	  {{315.0F,330.0F},{0.0F,0.0F}},		// plyr 3
//	  {{443.0F,330.0F},{0.0F,0.0F}}} };		// plyr 4

//static float tm_city_plq_xys[2][2] = { {256.0f,192.0f}, {370.0F,192.0f} };

//static tmu_ram_texture_t court_select_textures_tbl[] = {
// {"optbk00.wms",BKGRND_TID},	// 256x256
// {"optbk01.wms",BKGRND_TID},
// {"optbk02.wms",BKGRND_TID},

// {"crtsel00.wms",OPT_SCR_TID},	// 256x256
// {"crtsel01.wms",OPT_SCR_TID},

// {"tmsel01.wms",OPT_SCR_TID},			// 128x256
// {"tmsel02.wms",OPT_SCR_TID},			// 128x256
// {"tmsel03.wms",OPT_SCR_TID},			// 128x256
// {"tmsel04.wms",OPT_SCR_TID},			// 128x256

// {"plqfsh00.wms",SPFX_TID},
// {"plqfsh01.wms",SPFX_TID},
// {"plqfsh02.wms",SPFX_TID},

// {"plqtop00.wms",SPFX_TID},
// {"plqtop01.wms",SPFX_TID},

// {"plrcrd00.wms",JAPPLE_BOX_TID},	// 256x256
// {"plrcrd01.wms",JAPPLE_BOX_TID},	// 256x256

// {"cntnbr.wms",CNTDWN_TID},
// {NULL,0} };

// for OPTION SCREEN (minimum textures needed to start game)
tmu_ram_texture_t opt_scrn_textures_tbl[] = {
 {"optbk00.wms",BKGRND_TID},	// 256x256
 {"optbk01.wms",BKGRND_TID},
 {"optbk02.wms",BKGRND_TID},

 {"optscr00.wms",OPT_SCR_TID},
 {"optscr01.wms",OPT_SCR_TID},
 {"optscr02.wms",OPT_SCR_TID},
 {"optscr03.wms",OPT_SCR_TID},
 {"optscr04.wms",OPT_SCR_TID},
 {"optscr05.wms",OPT_SCR_TID},

 {"plqfsh00.wms",SPFX_TID},
 {"plqfsh01.wms",SPFX_TID},
 {"plqfsh02.wms",SPFX_TID},

 {"plqtop00.wms",SPFX_TID},
 {"plqtop01.wms",SPFX_TID},

 {"plrcrd00.wms",JAPPLE_BOX_TID},	// 256x256
 {"plrcrd01.wms",JAPPLE_BOX_TID},	// 256x256

 {"cntnbr.wms",CNTDWN_TID},
 {NULL,0} };

// for enter initials (minimum textures needed)
static tmu_ram_texture_t name_entry_textures_tbl[] = {
// {"optbk00.wms",BKGRND_TID},			// 256x256
// {"optbk01.wms",BKGRND_TID},
// {"optbk02.wms",BKGRND_TID},

 {"inits00.wms", YES_NO_PLAQ_TID},
 {"inits01.wms", YES_NO_PLAQ_TID},
 {"inits02.wms", YES_NO_PLAQ_TID},

// {"plqfsh00.wms",SPFX_TID},
// {"plqfsh01.wms",SPFX_TID},
// {"plqfsh02.wms",SPFX_TID},
//
// {"plqtop00.wms",SPFX_TID},
// {"plqtop01.wms",SPFX_TID},

// {"plrcrd00.wms",JAPPLE_BOX_TID},	// 256x256
// {"plrcrd01.wms",JAPPLE_BOX_TID},	// 256x256
 {NULL,0} };


// for TEAM SELECT
static tmu_ram_texture_t team_select_textures_tbl[] = {
// {"optbk00.wms",BKGRND_TID},			// 256x256
// {"optbk01.wms",BKGRND_TID},
// {"optbk02.wms",BKGRND_TID},

 {"tmsel00.wms",TM_SELECT_TID},			// 128x256
 {"tmsel01.wms",TM_SELECT_TID},			// 128x256
 {"tmsel02.wms",TM_SELECT_TID},			// 128x256
 {"tmsel03.wms",TM_SELECT_TID},			// 128x256
 {"tmsel04.wms",TM_SELECT_TID},			// 128x256

 {"tmsl88.wms",TM_SELECT_TID},			// 256x128  (backdrops)

 {"tmlogo00.wms",TM_SEL_LOGOS_TID},
 {"tmlogo01.wms",TM_SEL_LOGOS_TID},
 {"tmlogo02.wms",TM_SEL_LOGOS_TID},
 {"tmlogo03.wms",TM_SEL_LOGOS_TID},
 {"tmlogo04.wms",TM_SEL_LOGOS_TID},
 {"tmlogo05.wms",TM_SEL_LOGOS_TID},
 {"tmlogo06.wms",TM_SEL_LOGOS_TID},
 {"tmlogo07.wms",TM_SEL_LOGOS_TID},
 {"tmlogo08.wms",TM_SEL_LOGOS_TID},
 {"tmlogo09.wms",TM_SEL_LOGOS_TID},
 {"tmlogo10.wms",TM_SEL_LOGOS_TID},

// {"plqfsh00.wms",SPFX_TID},
// {"plqfsh01.wms",SPFX_TID},
// {"plqfsh02.wms",SPFX_TID},
//
// {"plqtop00.wms",SPFX_TID},
// {"plqtop01.wms",SPFX_TID},

 {"selplr00.wms",TM_SELECT_TID},
 {"selplr01.wms",TM_SELECT_TID},

 {"hidatr.wms",TM_SELECT_TID},

 {"crtsel00.wms",TM_SELECT_TID},	// 256x256
 {"crtsel01.wms",TM_SELECT_TID},

// {"plrcrd00.wms",JAPPLE_BOX_TID},	// 256x256
// {"plrcrd01.wms",JAPPLE_BOX_TID},	// 256x256
 {NULL,0} };


static tmu_ram_texture_t halftime_substitution_textures_tbl[] = {
// {"optbk00.wms",BKGRND_TID},			// 256x256
// {"optbk01.wms",BKGRND_TID},
// {"optbk02.wms",BKGRND_TID},

// {"tmsel01.wms",TM_SELECT_TID},			// 128x256
// {"tmsel02.wms",TM_SELECT_TID},			// 128x256
// {"tmsel03.wms",TM_SELECT_TID},			// 128x256
// {"tmsel04.wms",TM_SELECT_TID},			// 128x256

// {"selplr00.wms",TM_SELECT_TID},
// {"selplr01.wms",TM_SELECT_TID},

// {"plrcrd00.wms",JAPPLE_BOX_TID},	// 256x256
// {"plrcrd01.wms",JAPPLE_BOX_TID},	// 256x256
 {NULL,0} };


// for enter initials (minimum textures needed)
static tmu_ram_texture_t eoq_textures_tbl[] = {		// indoor court
// {"optscr00.wms",EOQ_TID},

// {"ctipsr00.wms",EOQ_TID},
// {"ctipsr01.wms",EOQ_TID},
// {"ctipsr02.wms",EOQ_TID},

 {"buybg00.wms",EOQ_TID},
 {"buybg01.wms",EOQ_TID},
 {"buybg02.wms",EOQ_TID},

// {"plrcrd00.wms",JAPPLE_BOX_TID},	// 256x256
// {"plrcrd01.wms",JAPPLE_BOX_TID},	// 256x256
 {NULL,0} };

static tmu_ram_texture_t eoq_textures_tbl2[] = {	// gillmore court
// {"optscr00.wms",EOQ_TID},

// {"ctipsr00.wms",EOQ_TID},
// {"ctipsr01.wms",EOQ_TID},
// {"ctipsr02.wms",EOQ_TID},

 {"pbuybg00.wms",EOQ_TID},
 {"pbuybg01.wms",EOQ_TID},
 {"pbuybg02.wms",EOQ_TID},

// {"plrcrd00.wms",JAPPLE_BOX_TID},	// 256x256
// {"plrcrd01.wms",JAPPLE_BOX_TID},	// 256x256
 {NULL,0} };


static tmu_ram_texture_t eoq_textures_tbl3[] = {	// geer court
// {"optscr00.wms",EOQ_TID},

// {"ctipsr00.wms",EOQ_TID},
// {"ctipsr01.wms",EOQ_TID},
// {"ctipsr02.wms",EOQ_TID},

 {"ibuybg00.wms",EOQ_TID},
 {"ibuybg01.wms",EOQ_TID},
 {"ibuybg02.wms",EOQ_TID},

// {"plrcrd00.wms",JAPPLE_BOX_TID},	// 256x256
// {"plrcrd01.wms",JAPPLE_BOX_TID},	// 256x256
 {NULL,0} };


// for HALFTIME stats page
static tmu_ram_texture_t hlf_stats_pg_textures_tbl[] = {
 {"hstat00.wms",STATS_PG_TID},
 {"hstat01.wms",STATS_PG_TID},
 {"hstat02.wms",STATS_PG_TID},
// {"plrcrd00.wms",JAPPLE_BOX_TID},	// 256x256
// {"plrcrd01.wms",JAPPLE_BOX_TID},	// 256x256
 {NULL,0} };

static tmu_ram_texture_t hlf_stats_pg_textures_tbl_lr[] = {
 {"hstatk00.wms",STATS_PG_TID},
 {"hstatk01.wms",STATS_PG_TID},
// {"plrcrd00.wms",JAPPLE_BOX_TID},	// 256x256
// {"plrcrd01.wms",JAPPLE_BOX_TID},	// 256x256
 {NULL,0} };

//static tmu_ram_texture_t hlf_stats_pg_2p_textures_tbl[] = {
// {"hstat00.wms",STATS_PG_TID},
// {"hstat01.wms",STATS_PG_TID},
// {"hstat02.wms",STATS_PG_TID},
// {"plrcrd00.wms",JAPPLE_BOX_TID},	// 256x256
// {"plrcrd01.wms",JAPPLE_BOX_TID},	// 256x256
// {NULL,0} };


// for ENDGAME stats page
static tmu_ram_texture_t final_stats_pg_textures_tbl[] = {
 {"fstat00.wms",STATS_PG_TID},
 {"fstat01.wms",STATS_PG_TID},
 {"fstat02.wms",STATS_PG_TID},
// {"plrcrd00.wms",JAPPLE_BOX_TID},	// 256x256
// {"plrcrd01.wms",JAPPLE_BOX_TID},	// 256x256
 {NULL,0} };

static tmu_ram_texture_t final_stats_pg_textures_tbl_lr[] = {
 {"fstatk00.wms",STATS_PG_TID},
 {"fstatk01.wms",STATS_PG_TID},
// {"plrcrd00.wms",JAPPLE_BOX_TID},	// 256x256
// {"plrcrd01.wms",JAPPLE_BOX_TID},	// 256x256
 {NULL,0} };

//static tmu_ram_texture_t final_stats_pg_2p_textures_tbl[] = {
// {"fstat00.wms",STATS_PG_TID},
// {"fstat01.wms",STATS_PG_TID},
// {"fstat02.wms",STATS_PG_TID},
// {"plrcrd00.wms",JAPPLE_BOX_TID},	// 256x256
// {"plrcrd01.wms",JAPPLE_BOX_TID},	// 256x256
// {NULL,0} };



static char *pin_strs[] = {
									"----",
									"*---",
									"**--",
									"***-",
									"****" };

image_info_t *count_down_imgs[] =  {&countd_0,&countd_1,&countd_2,&countd_3,&countd_4,&countd_5,&countd_6,
											&countd_7,&countd_8,&countd_9,&countd_9};

static image_info_t *init_plaq_imgs[2][MAX_PLYRS] = {
	{&plaqb, &plaqb, &plaqr, &plaqr},
	{&plaqb, &plaqg, &plaqr, &plaqy}
};

static image_info_t *pflash_flsh_imgs[] = {
	&pflasha01,&pflasha02,&pflasha03,&pflasha04,&pflasha05,&pflasha06,&pflasha07,&pflasha08,&pflasha09,&pflasha10};

static image_info_t *optscr_flsh_imgs[] = {
	&cflash01,&cflash02,&cflash03,&cflash04,&cflash05,&cflash06,&cflash07,&cflash08,
	&cflash09,&cflash10,&cflash11,&cflash12,&cflash13,&cflash14,&cflash15,&cflash16 };

static image_info_t *optscr_flsh2_imgs[] = {
	&flarefl01,&flarefl02,&flarefl03,&flarefl04,&flarefl05,&flarefl06,&flarefl07,
	&flarefl08,&flarefl09,&flarefl10,&flarefl11,&flarefl12,&flarefl13,&flarefl14,&flarefl15};

static image_info_t *jbox_plaq_imgs[][MAX_PLYRS] = {
	{&sawrdbx,&sawrdbx,&sawrdbx,&sawrdbx},
	{&sawrdbx,&sawrdbx,&sawrdbx,&sawrdbx},
 };

/*

static image_info_t *no_flsh_imgs[2][MAX_PLYRS][11] = {
{{&nglwl01,&nglwl02,&nglwl03,&nglwl04,&nglwl05,&nglwl06,&nglwl07,&nglwl08,&nglwl09,&nglwl10,&nglwl11},			// plr 1
 {&nglwl01,&nglwl02,&nglwl03,&nglwl04,&nglwl05,&nglwl06,&nglwl07,&nglwl08,&nglwl09,&nglwl10,&nglwl11},			// plr 2
 {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},
 {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}},
	// 4 plyr ver
{{&nglwl01,&nglwl02,&nglwl03,&nglwl04,&nglwl05,&nglwl06,&nglwl07,&nglwl08,&nglwl09,&nglwl10,&nglwl11},			// plr 1
 {&nglwl01,&nglwl02,&nglwl03,&nglwl04,&nglwl05,&nglwl06,&nglwl07,&nglwl08,&nglwl09,&nglwl10,&nglwl11},			// plr 2
 {&nglwr01,&nglwr02,&nglwr03,&nglwr04,&nglwr05,&nglwr06,&nglwr07,&nglwr08,&nglwr09,&nglwr10,&nglwr11},			// plr 3
 {&nglwr01,&nglwr02,&nglwr03,&nglwr04,&nglwr05,&nglwr06,&nglwr07,&nglwr08,&nglwr09,&nglwr10,&nglwr11}} };			// plr 4


static image_info_t *yes_flsh_imgs[2][MAX_PLYRS][11] = {
{{&yglwl01,&yglwl02,&yglwl03,&yglwl04,&yglwl05,&yglwl06,&yglwl07,&yglwl08,&yglwl09,&yglwl10,&yglwl11},			// plr 1
 {&yglwl01,&yglwl02,&yglwl03,&yglwl04,&yglwl05,&yglwl06,&yglwl07,&yglwl08,&yglwl09,&yglwl10,&yglwl11},			// plr 2
 {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},
 {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}},
	// 4 plyr ver
{{&yglwl01,&yglwl02,&yglwl03,&yglwl04,&yglwl05,&yglwl06,&yglwl07,&yglwl08,&yglwl09,&yglwl10,&yglwl11},			// plr 1
 {&yglwl01,&yglwl02,&yglwl03,&yglwl04,&yglwl05,&yglwl06,&yglwl07,&yglwl08,&yglwl09,&yglwl10,&yglwl11},			// plr 2
 {&yglwr01,&yglwr02,&yglwr03,&yglwr04,&yglwr05,&yglwr06,&yglwr07,&yglwr08,&yglwr09,&yglwr10,&yglwr11},			// plr 3
 {&yglwr01,&yglwr02,&yglwr03,&yglwr04,&yglwr05,&yglwr06,&yglwr07,&yglwr08,&yglwr09,&yglwr10,&yglwr11}} };		// plr 4

static image_info_t *lighting_imgs[] = {&light1,&light2,&light3,&light4,&light5,&light6,
										 &light7,&light8,&light9,&light10,&light11};

image_info_t *stat_flsh_imgs[] = {&flshbx01,&flshbx01,&flshbx02,&flshbx03,&flshbx04,&flshbx05,
		  							 &flshbx06,&flshbx07,&flshbx08,&flshbx09};			// 226x76

*/

static image_info_t *burst_flsh_imgs[] = {&flasha01,&flasha02,&flasha03,&flasha04,&flasha05,
										   &flasha07,&flasha08,&flasha09,&flasha10};



static image_info_t *credit_box_imgs[][5] = {{&japlxxxx,&japltxxx,&japlttxx,&japltttx,&japltttt},	 // <=1st qtr
										  	  {&japl_xxx,&japl_txx,&japl_ttx,&japl_ttt,&japl____},	 // 2nd qtr
											  {&japl__xx,&japl__tx,&japl__tt,&japl____,&japl____},	 // 3rd qtr.
											  {&japl___x,&japl___t,&japl____,&japl____,&japl____}};	 // 4th qtr and OT's
																

static image_info_t *japple_box_imgs[][MAX_PLYRS] = {
	{&japlbox,NULL,&japlbox,NULL},
	{&japlbox,&japlbox,&japlbox,&japlbox}
};

static image_info_t *tm_city_plqs[2] = {&tsbx_b,&tsbx_r};


static sprite_info_t *name_entry_img_ptrs[MAX_PLYRS][LTR_COLS*LTR_ROWS];

static sprite_info_t *tm_selct_img_ptrs[][2] = {		// [num ptrs][side 0 or 1]
												{0,0},					// team logo ptrs
												{0,0},					// city name 1 img. ptrs
												{0,0},					// city name 2 img. ptrs
												{0,0},					// city name 3 img. ptrs
												{0,0},					// city name 4 img. ptrs
												{0,0},					// city name 5 img. ptrs
												{0,0},					// city name 6 img. ptrs
												{0,0},					// city name 7 img. ptrs
												{0,0},					// team roster name 1 img. ptr
												{0,0},					// team roster name 2 img. ptr
												{0,0},					// team roster name 3 img. ptr
												{0,0},					// team roster name 4 img. ptr
												{0,0},					// team roster name 5 img. ptr
												{0,0},					// logo backdrop img. ptr.
												{0,0},					// roster bar img. ptr.
												{0,0},					// roster string img. ptr.
												{0,0},					// roster backdrop img. ptr.
												{0,0},					// city plaque img. ptr.
												{0,0} };				// name hilite bar img. ptr

static sprite_info_t *plr_sel_img_ptrs[][4] = {		// [num ptrs][players]
												{0,0,0,0},					// border header img
												{0,0,0,0},					// border header name img
												{0,0,0,0},					// border 
												{0,0,0,0},					// border backdrop
												{0,0,0,0},					// mugshot
												{0,0,0,0},					// name bkdrop img
												{0,0,0,0},					// stat box
												{0,0,0,0},					// swoosh img
												{0,0,0,0} };				// player name backdrop img


	
static image_info_t *plr_wrd_imgs[2][MAX_PLYRS] = {		//[2p,4p][index#]
	{&dronewrd, &plyr1wrd, &plyr2wrd, &dronewrd},
	{&plyr1wrd, &plyr2wrd, &plyr3wrd, &plyr4wrd},
};
	
/*
														
static image_info_t * meter_imgs[2][2][6] = {
	{{&meterl_0,&meterl_1,&meterl_2,&meterl_3,&meterl_4,&meterl_5},
	 {&meterr_0,&meterr_1,&meterr_2,&meterr_3,&meterr_4,&meterr_5}},
	{{&meterl_0_lr,&meterl_1_lr,&meterl_2_lr,&meterl_3_lr,&meterl_4_lr,&meterl_5_lr},
	 {&meterr_0_lr,&meterr_1_lr,&meterr_2_lr,&meterr_3_lr,&meterr_4_lr,&meterr_5_lr}}
};


//static image_info_t *yes_no_cursor_imgs[] = {&hilite_n,&hilite_y};
static image_info_t *yes_no_cursor_imgs[] = {&no_l,&yes_l,		// plyr 1
											 &no_l,&yes_l,		// plyr 2
											 &no_r,&yes_r,		// plyr 3
											 &no_r,&yes_r};		// plyr 4


static image_info_t **cursor_tbl_ptrs[] = {yes_no_cursor_imgs,inits_cursor_imgs,pin_cursor_imgs};
			
			
static image_info_t *stats_title_imgs[2][MAX_PLYRS] = {
														{&statpl_r,&statpl_r,NULL,NULL},
														{&statpl_r,&statpl_r,&statpl_r,&statpl_r}};

static image_info_t *stats_plq_imgs[2][MAX_PLYRS] = {
														{&sstatplr,&sstatplr,NULL,NULL},
														{&sstatplr,&sstatplr,&sstatplr,&sstatplr}};

*/

static int    fullgame_bought_ids[] = {5555,5556,5557,5558};
static int    roster_name_ids[] = {ROSTER_NAME1_ID,ROSTER_NAME2_ID,ROSTER_NAME3_ID,ROSTER_NAME4_ID};
static int    roster_name2_ids[] = {6150,6151,6152,6153};
static int    roster_stat_ids[] = {ROSTER_STAT1_ID,ROSTER_STAT2_ID,ROSTER_STAT3_ID,ROSTER_STAT4_ID,ROSTER_STAT5_ID,ROSTER_STAT6_ID};
static int    plr_nme_str_ids[] = {P1_NAME_STR_ID,P2_NAME_STR_ID,P3_NAME_STR_ID,P4_NAME_STR_ID};
static int    plr_pin_str_ids[] = {P1_PIN_STR_ID,P2_PIN_STR_ID,P3_PIN_STR_ID,P4_PIN_STR_ID};
static int   plyr_gen_str_ids[] = {P1_GEN_STR_ID,P2_GEN_STR_ID,P3_GEN_STR_ID,P4_GEN_STR_ID};
static int      plyr_plaq_ids[] = {OID_P1PLAQ,OID_P2PLAQ,OID_P3PLAQ,OID_P4PLAQ};
static int plyr_award_plq_ids[] = {OID_P1AWARD,OID_P2AWARD,OID_P3AWARD,OID_P4AWARD};
static int     plyr_alpha_ids[] = {OID_P1ALPHA,OID_P2ALPHA,OID_P3ALPHA,OID_P4ALPHA};
static int      plyr_sel_pids[] = {P1_PLYR_SEL_PID,P2_PLYR_SEL_PID,P3_PLYR_SEL_PID,P4_PLYR_SEL_PID};

static int tmsel_ids[] = {OID_TEAM1,OID_TEAM2};

static char alpha_table[LTR_COLS*LTR_ROWS] = {
	CH_A,CH_B,CH_C,CH_D,CH_E,
	CH_F,CH_G,CH_H,CH_I,CH_J,
	CH_K,CH_L,CH_M,CH_N,CH_O,
	CH_P,CH_Q,CH_R,CH_S,CH_T,
	CH_U,CH_V,CH_W,CH_X,CH_Y,
	CH_Z,CH_EXC,CH_DEL,CH_SPC,CH_END};

static image_info_t *inits_letter_imgs[] = {
	&l_a,&l_b,&l_c,&l_d,&l_e,
	&l_f,&l_g,&l_h,&l_i,&l_j,
	&l_k,&l_l,&l_m,&l_n,&l_o,
	&l_p,&l_q,&l_r,&l_s,&l_t,
	&l_u,&l_v,&l_w,&l_x,&l_y,
	&l_z,&l_exc,&l_bck,&l_spc,&l_end };

static char pin_nbr_tbl[PIN_COLS*PIN_ROWS] = {
	CH_0,CH_1,CH_2,
	CH_3,CH_4,CH_5,
	CH_6,CH_7,CH_8,
	CH_9,CH_SPC,CH_DEL};
//	CH_1,CH_2,CH_3,
//	CH_4,CH_5,CH_6,
//	CH_7,CH_8,CH_9,
//	CH_SPC,CH_0,CH_DEL};

static image_info_t *pin_nbr_imgs[] = {
	&n_0,&n_1,&n_2,
	&n_3,&n_4,&n_5,
	&n_6,&n_7,&n_8,
	&n_9,&n_spc,&n_bck };


//char *down_str[] = { "1ST", "2ND", "3RD", "4TH" };
//char *qtr_str[] = { "1ST QUARTER", "2ND QUARTER", "3RD QUARTER", "4TH QUARTER", "OVERTIME 1", "OVERTIME 2", "OVERTIME 3" };

char *period_names[] =
{
	"2ND PERIOD",
	"3RD PERIOD",
	"4TH PERIOD",
	"OVERTIME",
	"OVERTIME 2",
	"OVERTIME 3",
	"OVERTIME 4"
};


extern char start_movie_snd_bnk_str[];

static sndinfo start_movie_snds[] = {
	{start_movie_snd_bnk_str, 0, VOLUME5, 127, PRIORITY_LVL5},
	{NULL}
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void show_movie(char *basename, int use_chroma_key, int ckey, sndinfo *sounds, void (*func)())
{
	static int	dbsw_cur, dbsw_old, dbsw_new;
	static char *movie_to_play[2];

	// Make sure the movie player is shut down if it is running
	if(movie_get_state() > 1)
	{
		// Enable drawing
		// draw_enable(1);

		// Abort the movie player
		movie_abort();

		// Fade the last displayed movie frame out
		//fade(0.0f, 5, NULL);
		//sleep(6);

		// Disable drawing
		//draw_enable(0);
	}

	normal_screen();

	movie_config_player(512.0f, 384.0f, 0.0f, 0.0f, 1.0f, 30);
	movie_to_play[0] =  basename;
	qcreate("mplay", 0, movie_proc, (int)movie_to_play, 0, 0, FALSE);
	/* Wait till movie is running */
	while (movie_get_state() != MOVIE_STATE_RUN)
	{

		if (movie_get_state() == MOVIE_STATE_DONE)
			break;

		//if (game_info.state != GS_ATTRACT) 
		//	break;
		sleep (1);

	}

	if ((movie_get_state() == MOVIE_STATE_RUN) &&
		(use_chroma_key))
	{
		movie_set_chromakey_mode(TRUE); 
		movie_set_chromakey(ckey);
	}

	// good place to start sound (maybe)
//	if (sounds && sounds->bank_name)
//	{
//		snd_scall_bank(sounds->bank_name,
//			sounds->scall,
//		    sounds->volume,
//		    sounds->pan,
//		    sounds->priority);
//	}

	if (func) 
		func();

	// this is put here... to avoid the black screen.. from wipeout..and show first frame of movie!
//	draw_enable(1);

	while (movie_get_state() != MOVIE_STATE_DONE)
	{
		dbsw_old = dbsw_cur;
		dbsw_cur = get_player_sw_current();
		dbsw_new = dbsw_cur & ~dbsw_old;

		sleep(1);
	}
	// Fade the movie
	//fade(0.0f, 5, NULL);

// !!!FIX (LET SOUND KEEP PLAYING)
//	if (sounds && sounds->bank_name)
//	{
//		snd_stop_all();
//		//snd_bank_delete(sounds->bank_name);
//	}

	// Make sure the movie player is shut down if it is running
	// This happens when the player whacks through the movie
	movie_abort();
}

							
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//void dim_text_start(int *args)
//{
//int	i,tm_num;
//
//	tm_num = args[0];
//
//	// change the text to a constant color ... so I can fade it down to a darker (dimmer) color.
//	for (i=TM_NME1_OBJ_PTR; i <= TM_NME7_OBJ_PTR; i++)
//	{
//		if (i != TM_NME4_OBJ_PTR)
//		{
//			tm_selct_img_ptrs[i][tm_num]->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
//			tm_selct_img_ptrs[i][tm_num]->state.constant_color = 0xffd0d0d0;
//		}
//	}
//
//	die(0);
//}

//-------------------------------------------------------------------------------------------------
// 						This process controls the flow of all the PRE-GAME screens
//
//  called by FIRST valid attract mode start button hit
//-------------------------------------------------------------------------------------------------

extern int texture_load_in_progress;

void pre_game_scrns(int *args)
{
	int no_name_entry, pnum, i;
	int sargs[4];
	sprite_info_t * psprite;

	// Make sure <game_proc> finishes loads &| dies
	if (existp(GAME_PROC_PID, 0xFFFFFFFF) && texture_load_in_progress)
		game_info.attract_coin_in = 1;

	// Don't kill any disk loads
	while (texture_load_in_progress) sleep(1);

	if (rec_sort_ram != NULL) {
		free(rec_sort_ram);
		rec_sort_ram = NULL;
	}
	// Disable drawing (re-enabled by name_entry or below)
	draw_enable(0);
	wipeout();
	clear_free_sprite_list();
	snd_stop_track(SND_TRACK_0);		// stop game over music

	/* set game mode master volume level */
	snd_master_volume(game_info.master_volume);
	/* enable sounds in case attract mode disabled them */
	snd_bank_lockout(0);

	no_name_entry = args[0];

	// player who started game!
	pnum = args[1];

	if ((plates_on == PLATES_ON) || (plates_on == PLATES_COMING_ON))
//		turn_transit_off(0);
		turn_transit_off(1);

	// Make sure the movie player is shut down if it is running
	if(movie_get_state() > 1)
	{
		// Enable drawing
		draw_enable(1);

		// Abort the movie player
		movie_abort();
#if 0
		stream_stop();
		do { sleep (1); } while (stream_check_active());
		stream_release_key();
#endif
		// Fade the last displayed movie frame out
		fade(0.0f, 5, NULL);
		sleep(6);

		// Disable drawing
		draw_enable(0);
	}

	if (!reload)
	{
		// Check game select & game lock DIPs
		if ((get_dip_coin_current() & DIP_BIT_6) == 0 ||	// 0 if no lock, 1 if lock
			(get_dip_coin_current() & DIP_BIT_7) == 0)		// 0 if NFL, 1 if NBA
		{
			turn_transit_on();
			if (game_select(NBA_RUNNING))		// for NBA change to NBA_RUNNING
			{
				// REBOOT PLEASE!!!!
				#ifdef DEBUG
					fprintf( stderr,"REBOOT!!!\n");
				//	while(1) sleep(1);
				#endif

				display_loading(NFL_RUNNING);

				draw_enable(1);
				turn_transit_off(1);

				sleep(120);
				fade(0.0f, 30, NULL);
				sleep(30);
				{
					sargs[0] = 1;
					sargs[1] = pnum;
					sargs[2] = 0;
					sargs[3] = 0;

					write_cmos_to_disk(FALSE);
					update_other_cmos_to_disk(FALSE);

					draw_enable(0);
					snd_master_volume(0);
					setdisk(1);		// Set NFL Partition
					write_game_info();		// Write out valuable info
					exec("GAME.EXE", 0xbebeefed, sargs);
				}
			}
		}
	}
	else
	{
		for (i = 0; i < MAX_PLYRS; i++)
		{
			if (pdata_blocks[i].quarters_purchased >= 4)
				qcreate("fgbought",0,fullgame_bought_msg,i,0,0,0);
		}
	}

	/* chalk 'game start' audit */
	increment_audit(GAME_START_AUD);
	update_gamestart_percents();

	set_coin_hold(FALSE);
	reload=0;

//fprintf(stderr, "1\n");
//sleep(1);

	// show first frame of movie... before anything
	psprite = beginobj(&startbg,  		  0.0F, SPRITE_VRES, 200.0F, BKGRND_TID);
	psprite->id = 1;
	psprite = beginobj(&startbg_c1,   	  0.0F, SPRITE_VRES, 200.0F, BKGRND_TID);
	psprite->id = 1;
	psprite = beginobj(&startbg_c2,   	  0.0F, SPRITE_VRES, 200.0F, BKGRND_TID);
	psprite->id = 1;
	psprite = beginobj(&startbg_c3,   	  0.0F, SPRITE_VRES, 200.0F, BKGRND_TID);
	psprite->id = 1;

	draw_enable(1);

//fprintf(stderr, "2\n");
//sleep(1);

	// Make sure the disk is in normal mode
	dio_mode(DIO_MODE_BLOCK);

	// Fix!!!  Maybe.  We need to clear up movie snd banks that play in amode when
	// we come to credit page - normally, the bank is cleared up when the movie finishes

	snd_bank_delete(showtime_movie_snd_bnk_str);
	snd_bank_delete(trophy_snd_bnk_str);
	snd_bank_delete(midway_logo_movie_snd_bnk_str);
	snd_bank_delete(nbc_logo_movie_snd_bnk_str);
	snd_bank_delete(eog_movie_snd_bnk_str);


	// In case we get here as a result of awarding a free game
	start_enable(TRUE);

	// Disable drawing (re-enabled by name_entry or below)
//	draw_enable(0);
//	wipeout();
//fprintf(stderr, "3\n");
//sleep(1);

	sleep(1);
	// In case we were in a fade
//	normal_screen();

	// start game sound
//	snd_scall_bank(cursor_bnk_str,35,VOLUME4,127,PRIORITY_LVL4);

	vs_page_showing = 0;

	// clear out record ram
	clear_cmos_record((char *)&player1_data,sizeof(plyr_record_t));
	clear_cmos_record((char *)&player2_data,sizeof(plyr_record_t));
	clear_cmos_record((char *)&player3_data,sizeof(plyr_record_t));
	clear_cmos_record((char *)&player4_data,sizeof(plyr_record_t));
	// clear out stat ram
	clear_cmos_record((char *)&player1_sdata,sizeof(plyr_record_t));
	clear_cmos_record((char *)&player2_sdata,sizeof(plyr_record_t));
	clear_cmos_record((char *)&player3_sdata,sizeof(plyr_record_t));
	clear_cmos_record((char *)&player4_sdata,sizeof(plyr_record_t));


	// have player choose between "create play", "enter name", "select team"
	option_screen(pnum);

	// was CREATE-A-PLAY selected ?
	if (option == 0) {
		int	args[4];
		
		/* chalk up a create a player audit */
		do_percent_aud(CREATE_PLAYER_AUD, TOTAL_PLAYS_AUD, CREATE_PLAYER_PERCENT_AUD);
		args[0] = four_plr_ver?pnum:index_2p[pnum];
		args[1] = 0;
		args[2] = 0;
		args[3] = 0;
		create_process("creatp", 0, args, creatp_proc, 2048);
		die(0);
	}

	for (i = 0; i < NUM_PLAYERS; i++) {
		is_guest[i] = FALSE;
		selected_players[i].record_num = -1;
	}

	if ((!no_name_entry) && (option == 1))
		name_entry(0);

	/* load the guest mugshots if any */
	for (i = 0; i < NUM_PLAYERS; i++)
		if (is_guest[i])
			load_mugshot_tpage(selected_players[i].player_data->pii_mug);
	for (i = 0; i < NUM_PLAYERS; i++)
		if (selected_players[i].record_type == CREATP_RECORD) {
			increment_audit(GAMES_PLAYED_WITH_CREATEP_PERCENT_AUD);
			break;
		}
	
	loading_mug_textures = 0;

	team_selection();

	fullgame_bought_ok = 0;				// set only when its OK to show the plaqs

//	sleep(50-PLATE_TRANSITION_TICKS);	//let slam sound finish

	vs_screen();
	jordan_check();
	launch_game_proc();
}


//-------------------------------------------------------------------------------------------------
//	This process flashes bonus price message on YES/NO screen
//
// 	INPUT:	Strings to flash (1 to 4) or sleep ticks if > 0
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
static void purchase_flasher(int *args)
{
	ostring_t	*pmsg[4] = {0, 0, 0, 0};
	int			pmsg_org_color[4];
	int			ticks, i;

	ticks = 15;

	for (i = 0; i < 4; i++)
	{
		if (args[i] > 0)
			ticks = args[i];
		if (args[i] < 0)
		{
			pmsg[i] = (ostring_t *)*(args+i);
			pmsg_org_color[i] = pmsg[i]->state.color;
		}
	}

	while(1)
	{
		sleep(ticks);
		for (i = 0; i < 4; i++)
		{
			if (pmsg[i])
			{
				pmsg[i]->state.color = WHITE;
				change_string_state(pmsg[i]);
			}
		}
		sleep(ticks);
		for (i = 0; i < 4; i++)
		{
			if (pmsg[i])
			{
				pmsg[i]->state.color = pmsg_org_color[i];
				change_string_state(pmsg[i]);
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------
//	This process flashes alpha string(s)
//
// 	INPUT:	Strings to flash (1 to 4) or sleep ticks if > 0
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
static void alpha_string_flasher(int *args)
{
	ostring_t	*pmsg[4] = {0, 0, 0, 0};
	int			pmsg_org_color[4];
	int			ticks, i;

	ticks = 15;

	for (i = 0; i < 4; i++)
	{
		if (args[i] > 0)
			ticks = args[i];
		if (args[i] < 0)
		{
			pmsg[i] = (ostring_t *)*(args+i);
			pmsg_org_color[i] = pmsg[i]->state.color;
		}
	}

	while(1)
	{
		sleep(ticks);
		for (i = 0; i < 4; i++)
			if (pmsg[i]) change_alpha_cc( pmsg[i], WHITE );

		sleep(ticks);
		for (i = 0; i < 4; i++)
			if (pmsg[i]) change_alpha_cc( pmsg[i], pmsg[i]->state.color );
	}
}

//-----------------------------------------------------------------------------------------------------------------------------
//	This process turns on a bonus pricing message on YES/NO screen if available
//
// 	INPUT:	Nothing
//	OUTPUT:	Nothing
//-----------------------------------------------------------------------------------------------------------------------------
static process_node_t	*pflasher = NULL;
static ostring_t			*pmsg[2];

void show_bonus_proc(int *args)
{
//	int i;

	if (!(coin_check_freeplay()))
	{
		// Allow each sequential letter to overlap the previous letter - turn off at end!
		set_text_z_inc(1.0F);
		set_text_id (1);
		set_string_id (1);

		if (full_game_credits < (get_credits_to_start() + (get_credits_to_continue() * 3)))
		{
			// If credits for full game is not zero - then it is valid
			if (full_game_credits)
			{
				// create the "N credits to purchase full game" string
			 	set_text_font (FONTID_NBA10);
				set_text_color(LT_RED);

//				set_text_position(SPRITE_HRES/2, 95.0F, 5.0F);
//				pmsg[0] = oprintf("%djBONUS PRICE:", (HJUST_CENTER|VJUST_CENTER));

				set_text_position(SPRITE_HRES/2, 80.0f, 5.0f);//79.0F, 5.0F);
				pmsg[0] = oprintf("%djPURCHASE FULL GAME UP FRONT FOR ONLY %d CREDITS", (HJUST_CENTER|VJUST_CENTER), get_full_game_credits());

				qcreate("pflashA",BONUS_PID, purchase_flasher, (int)pmsg[0], 0, 0, 0);
			}
		}
		set_text_z_inc(0.0F);
	}
}

//----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void display_option_screen(void)
{
	sprite_info_t * psprite;

	del1c(1,-1);		// delete first frame of 'start' movie

	draw_backdrop();

	psprite = beginobj(&chsmidbr, title_bar_xys[four_plr_ver][1][X_VAL], title_bar_xys[four_plr_ver][1][Y_VAL], 14.0F, OPT_SCR_TID);
	psprite->id = 1;
	psprite = beginobj(&chsmidbr_c1, title_bar_xys[four_plr_ver][1][X_VAL], title_bar_xys[four_plr_ver][1][Y_VAL], 14.0F, OPT_SCR_TID);
	psprite->id = 1;
	set_string_id(0x01);
	set_text_z_inc(0.01f);
	set_text_font(FONTID_NBA23);	/* FONTID_NBA25 */
	set_text_color(LT_YELLOW);
	set_text_position((SPRITE_HRES/2.0f)+25.0f,329.0f, 2.0F);
	oprintf("%djCHOOSE OPTION", (HJUST_CENTER|VJUST_CENTER));


	qcreate("cred_msg",CREDIT_PID,credit_msg,0,JAPPLE_BOX_TID,0,0);

	qcreate("credbx1",CREDIT_PID,plyr_credit_box,0,station_xys[0][four_plr_ver][X_VAL],station_xys[0][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
	qcreate("credbx3",CREDIT_PID,plyr_credit_box,2,station_xys[2][four_plr_ver][X_VAL],station_xys[2][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
	// messages for japple boxes (if needed)
	qcreate("p1_chal",P1_CHALNGR_PID,challenger_needed_msg,PLYR_1,JAPPLE_BOX_TID,0,0);
	qcreate("p3_chal",P3_CHALNGR_PID,challenger_needed_msg,PLYR_3,JAPPLE_BOX_TID,0,0);

	if (four_plr_ver)
	{
		qcreate("credbx2",CREDIT_PID,plyr_credit_box,1,station_xys[1][four_plr_ver][X_VAL],station_xys[1][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
		qcreate("credbx4",CREDIT_PID,plyr_credit_box,3,station_xys[3][four_plr_ver][X_VAL],station_xys[3][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
		qcreate("p2_chal",P2_CHALNGR_PID,challenger_needed_msg,PLYR_2,JAPPLE_BOX_TID,0,0);
		qcreate("p4_chal",P4_CHALNGR_PID,challenger_needed_msg,PLYR_4,JAPPLE_BOX_TID,0,0);
	}

	// Show bonus price message if available
	qcreate("bonus",CREDIT_PID,show_bonus_proc,0,0,0,0);		
}

//-----------------------------------------------------------------------------------------------------------------------------
//			 Have player choose between "create play", "enter name", "select team"
//
// returns: -1, 0, or 1 
//-----------------------------------------------------------------------------------------------------------------------------
void option_screen(int p_num)
{
	int	i;
	sprite_info_t * psprite;
	process_node_t * ptimer;

	draw_enable(1);

	// Disable the disk interrupt callback (IN CASE...started on some attract screens..ie team info)
//	dio_mode(DIO_MODE_BLOCK);

	// start movie tune
	snd_scall_bank(start_movie_snd_bnk_str, 0, VOLUME6, 127, SND_PRI_SET|SND_PRI_TRACK0);

	// show first frame of movie... before anything
//	psprite = beginobj(&startbg,  		  0.0F, SPRITE_VRES, 200.0F, BKGRND_TID);
//	psprite->id = 1;
//	psprite = beginobj(&startbg_c1,   	  0.0F, SPRITE_VRES, 200.0F, BKGRND_TID);
//	psprite->id = 1;
//	psprite = beginobj(&startbg_c2,   	  0.0F, SPRITE_VRES, 200.0F, BKGRND_TID);
//	psprite->id = 1;
//	psprite = beginobj(&startbg_c3,   	  0.0F, SPRITE_VRES, 200.0F, BKGRND_TID);
//	psprite->id = 1;

	normal_screen();
	sleep(5);
//	fade(1.0f, 20, 0);		/* 1=full on 20=ticks */

	// load nesessary textures
	if (load_textures_into_tmu(opt_scrn_textures_tbl))
	{
#ifdef DEBUG
		fprintf(stderr, "Couldn't load all option screen textures into tmu\r\n");
		lockup();
#else
		return;
#endif
	}

	// must LOCK these textures so that one player doesnt delete them for the other player
	lock_multiple_textures(BKGRND_TID,0xFFFFFFFF);
	lock_multiple_textures(OPT_SCR_TID,0xFFFFFFFF);
	lock_multiple_textures(SPFX_TID,0xFFFFFFFF);
	lock_multiple_textures(JAPPLE_BOX_TID,0xFFFFFFFF);
	lock_multiple_textures(CNTDWN_TID,0xFFFFFFFF);			// not unlocked until end of team select


	qcreate("whoosh",0,whoosh_proc,0,0,0,0);


	// start movie
   	show_movie("star", TRUE, 0xff00ff, start_movie_snds, display_option_screen);

	// set some exit variables
	force_selection = 0;
	exit_status = 0;
	option = 1;					// default to ENTER NAME

//	qcreate("cred_msg",CREDIT_PID,credit_msg,0,JAPPLE_BOX_TID,0,0);
  	ptimer = qcreate("timer",TIMER_PID,timedown,6,9,SPRITE_HRES/2.0f,99.0F);							//wait 1 second then cnt from 9

//	qcreate("credbx1",CREDIT_PID,plyr_credit_box,0,station_xys[0][four_plr_ver][X_VAL],station_xys[0][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
//	qcreate("credbx2",CREDIT_PID,plyr_credit_box,1,station_xys[1][four_plr_ver][X_VAL],station_xys[1][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
//
//	if (four_plr_ver)
//	{
//		qcreate("credbx3",CREDIT_PID,plyr_credit_box,2,station_xys[2][four_plr_ver][X_VAL],station_xys[2][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
//		qcreate("credbx4",CREDIT_PID,plyr_credit_box,3,station_xys[3][four_plr_ver][X_VAL],station_xys[3][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
//	}

	// messages for japple boxes (if needed)
//	qcreate("p1_chal",P1_CHALNGR_PID,challenger_needed_msg,PLYR_1,JAPPLE_BOX_TID,0,0);
//	qcreate("p2_chal",P2_CHALNGR_PID,challenger_needed_msg,PLYR_2,JAPPLE_BOX_TID,0,0);
//
//	if (four_plr_ver)
//	{
//		qcreate("p3_chal",P3_CHALNGR_PID,challenger_needed_msg,PLYR_3,JAPPLE_BOX_TID,0,0);
//		qcreate("p4_chal",P4_CHALNGR_PID,challenger_needed_msg,PLYR_4,JAPPLE_BOX_TID,0,0);
//	}


//	qcreate("welcome",0,welcome_proc,0,0,0,0);

	// Show bonus price message if available
//	qcreate("bonus",CREDIT_PID,show_bonus_proc,0,0,0,0);		

	draw_enable(1);
	normal_screen();
//	turn_transit_off(1);	//!!!

	fullgame_bought_ok = 1;		// set only when its OK to show the plaqs

	// Chk for & show 4 player free game msg
	if (!get_adjustment(H4_FREE_GAME_ADJ, &i))
	{
		if (four_plr_ver && i)
		{
			sprite_info_t * idiot_bx_obj;
			int ticks = 0;

			idiot_bx_obj = beginobj(&playresult, SPRITE_HRES/2.0f, SPRITE_VRES/2.0f, 20.0f, SCR_PLATE_TID);
			idiot_bx_obj->w_scale = 2.22f;
			idiot_bx_obj->h_scale = 1.9f;
			idiot_bx_obj->id = OID_MESSAGE;
			generate_sprite_verts(idiot_bx_obj);

			set_text_z_inc(1.0f);

			set_string_id(0x34);
			set_text_font(is_low_res ? FONTID_NBA20 : FONTID_NBA25);
			set_text_color(LT_RED);

			if (is_low_res)
				set_text_position(SPRITE_HRES/2.0f,(SPRITE_VRES/2.0f)+25.0f-1.0f, 1.0F);
			else
				set_text_position(SPRITE_HRES/2.0f,(SPRITE_VRES/2.0f)+25.0f, 1.0F);
			pmsg[0] = oprintf( "%djWINNERS OF A 4 PLAYER", (HJUST_CENTER|VJUST_CENTER));

			if (is_low_res)
				set_text_position(SPRITE_HRES/2.0f,(SPRITE_VRES/2.0f)-20.0f-1.0f, 1.0F);
			else
				set_text_position(SPRITE_HRES/2.0f,(SPRITE_VRES/2.0f)-20.0f, 1.0F);
			pmsg[1] = oprintf( "%djGAME STAY ON FREE!", (HJUST_CENTER|VJUST_CENTER));

			pflasher = qcreate("pflashB", MESSAGE_PID, flash_alpha_text_proc, (int)pmsg[0], (int)pmsg[1], 0, 0);
			suspend_process(ptimer);

// !!!FIX
//			snd_scall_bank(cursor_bnk_str,WARNING_SND,VOLUME4,127,PRIORITY_LVL5);
			// Tinkle snd
			snd_scall_bank(plyr_bnk_str, 57 , VOLUME7, 127, PRIORITY_LVL3);

			// Wait for timeout or button press
			do
			{
				sleep(1);
				ticks++;

				if (ticks >= 50)	// min ticks
				{
					i = get_player_sw_current();

					if ((p_status & 1) && (i & P1_ABCD_MASK)) break;
					if ((p_status & 2) && (i & P2_ABCD_MASK)) break;
					if ((p_status & 4) && (i & P3_ABCD_MASK)) break;
					if ((p_status & 8) && (i & P4_ABCD_MASK)) break;
				}
			} while (ticks < 240);	// max ticks

			snd_scall_bank(cursor_bnk_str,OPTION_CURSR_SND,VOLUME7,127,PRIORITY_LVL2);

			resume_process(ptimer);
			delete_multiple_strings(0x34, -1);
			delobj(idiot_bx_obj);
			kill(pflasher, 0);
		}
	}


	// start 'selection' process
	qcreate("slctopt",0,plyr_select_option_proc,p_num,0,0,0);

	// Take away the plates
//	draw_enable(1);
//	normal_screen();
//	turn_transit_off(1);

	do
	{
		sleep(1);

	} while (exit_status);

	// first wait for any FULLGAME BOUGHT msg...
	while (existp(FULLGAME_BOUGHT_PID, 0xFFFFFFFF))
		sleep(1);

	turn_transit_on();

	//
	// Now delete all objects and textures from this screen
	//
	delete_multiple_strings(CREDIT_MSG_ID, -1);
	delete_multiple_strings(0x01, -1);		// delete title bar msg.
	del1c(1, -1);
	del1c(OID_JAPPLE_BOX, -1);
	del1c(OID_MISC,-1);

	// BIG MAYBE
	killall(CREDIT_PID,-1);										//	kill_countdown_timer();
	killall(TIMER_PID,-1);										//	kill_countdown_timer();
	killall( P1_CHALNGR_PID, -1 );
	killall( P2_CHALNGR_PID, -1 );
	killall( P3_CHALNGR_PID, -1 );
	killall( P4_CHALNGR_PID, -1 );

//	unlock_multiple_textures(BKGRND_TID,0xffffffff);
//	delete_multiple_textures(BKGRND_TID,0xffffffff);
	unlock_multiple_textures(OPT_SCR_TID,0xffffffff);
	delete_multiple_textures(OPT_SCR_TID,0xffffffff);
//	unlock_multiple_textures(SPFX_TID,0xffffffff);
//	delete_multiple_textures(SPFX_TID,0xffffffff);
//	unlock_multiple_textures(JAPPLE_BOX_TID,0xffffffff);
//	delete_multiple_textures(JAPPLE_BOX_TID,0xffffffff);
	delete_multiple_strings(0x45, 0xffffffff);
	delete_multiple_strings(0x46, 0xffffffff);

	if (option == 0)
	{
		killall(CREDIT_PID,0xFFFFFFFF);										//	kill_countdown_timer();
		del1c(0, 0xFFFFFFFF);
	}
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void plyr_select_option_proc(int *args)
{
	int				pnum,plyrs;
	int				switches,buttons,btn_dly;
	int				cap_allowed;
	int				option_selected;
	int				asking_yesno;
	int				answer;
	int				ver;
	int				i;
	sprite_info_t 	*psprite;
	sprite_info_t 	*cap_plq_obj;
	sprite_info_t 	*init_plq_obj;
	sprite_info_t 	*sel_plq_obj;
	sprite_info_t 	*cap_txt_obj;
	sprite_info_t 	*init_txt_obj;
	sprite_info_t 	*sel_txt_obj;
	sprite_info_t 	*yes_plq_obj = NULL;
	sprite_info_t 	*no_plq_obj = NULL;
	sprite_info_t 	*yes_obj = NULL;
	sprite_info_t 	*no_obj = NULL;
	sprite_info_t 	*glow_obj = NULL;
	long			alpha_level, delta;


	// get player number of player making selection
	pnum = args[0];

	// for lack of wanting to type! :)
	ver = four_plr_ver;

	// create the THREE options plates
	cap_plq_obj = beginobj(&disc1, option_xys[ver][0][X_VAL], option_xys[ver][0][Y_VAL], 60.0F, OPT_SCR_TID);
	cap_plq_obj->id = 1;
	cap_txt_obj = beginobj(&cpwrds, option_xys[ver][0][X_VAL], option_xys[ver][0][Y_VAL], 50.0F, OPT_SCR_TID);
	cap_txt_obj->id = 1;

	init_plq_obj = beginobj(&disc1, option_xys[ver][1][X_VAL], option_xys[ver][1][Y_VAL], 61.0F, OPT_SCR_TID);
	init_plq_obj->id = 1;
	init_txt_obj = beginobj(&eiwrdy, option_xys[ver][1][X_VAL], option_xys[ver][1][Y_VAL], 51.0F, OPT_SCR_TID);
	init_txt_obj->id = 1;

	sel_plq_obj = beginobj(&disc1, option_xys[ver][2][X_VAL], option_xys[ver][2][Y_VAL], 62.0F, OPT_SCR_TID);
	sel_plq_obj->id = 1;
	sel_txt_obj = beginobj(&stwrds, option_xys[ver][2][X_VAL], option_xys[ver][2][Y_VAL], 52.0F, OPT_SCR_TID);
	sel_txt_obj->id = 1;

	glow_obj = beginobj(&disc2, option_xys[ver][1][X_VAL], option_xys[ver][1][Y_VAL], 58.0F, OPT_SCR_TID);
//	glow_obj->state.alpha_rgb_dst_function = GR_BLEND_ONE;			// set additive blending!!
	glow_obj->id = 1;

	// set variables
	exit_status |= 1<<pnum;												// SET players bit in variable

	cap_allowed = 1;				// yes...so far
	option_selected = 0;			// no
	asking_yesno = 0;
	answer = 0;						// default to NO (create a play -- yes/no)

	alpha_level = 255;
	delta = -8;

//	plyrs = (ver ? 4 : 2);

	btn_dly = 30;

	// control loop
	while (!option_selected)
	{
		// Read INPUT
		switches = 0;
		buttons = 0;
		// read joysticks from any active player
//		for (i=0; i < plyrs; i++)
		for (i=0; i < MAX_PLYRS; i++)
		{
			if ((p_status & (1<<i)) && (switches = get_joy_val_down(i)))
				break;
		}
		if (btn_dly > 0)
		{
			buttons = 0;
		}
		else
		{
			// read buttons from any active player
//			for (i=0; i < plyrs; i++)
			for (i=0; i < MAX_PLYRS; i++)
			{
				if ((p_status & (1<<i)) && (buttons = get_but_val_down(i)))
					break;
			}
		}
		// if another player buys in... dont allow them to select create-a-play
		if (((count_players_in_game() > 1) && (cap_allowed)) ||
			((count_quarters_purchased() > 1) && (cap_allowed)))
		{
			cap_allowed = 0;							// NOT anymore
			// put a big X over the create play option
			psprite = beginobj(&nonselct, option_xys[ver][0][X_VAL], option_xys[ver][0][Y_VAL], 5.0F, 0);
			psprite->id = 1;

			set_text_z_inc(0.01f);
			set_string_id(0x46);
			set_text_font(FONTID_NBA10);
			set_text_color(LT_CYAN);
			set_text_position(option_xys[ver][0][X_VAL]-4, option_xys[ver][0][Y_VAL]-80.0f, 1.1F);//-100.0f, 1.1F);
			oprintf("%dj%dcONE PLAYER ONLY", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);

			// if cursor on create-a-play...must move it off
			if (option == 0)
			{
				switches = JOY_RIGHT;				// move cursor
				if (asking_yesno)
				{
					// they decided NO for create a play
					answer = 0;			// default to NO then delete the YES/NO stuff
					asking_yesno = 0;
					buttons = 0;
					del1c(OID_MISC,-1);
					unhide_sprite(cap_txt_obj);
				}
			}
		}

		// if screen times out... default to SELECT TEAM
		if (force_selection)
		{
			// if asking YES/NO...must delete...that shit
			if (option == 0)
			{
				if (asking_yesno)
				{
					asking_yesno = 0;
					del1c(OID_MISC,-1);
					unhide_sprite(cap_txt_obj);
				}
				change_img_tmu(cap_txt_obj,&cpwrds,OPT_SCR_TID);
				change_img_tmu(init_txt_obj,&eiwrdy,OPT_SCR_TID);
				option = 1;
			}
			if (option == 1)
			{
				switches = JOY_RIGHT;				// move cursor
			}
			buttons = 1;							// and select it!
		}

		// Handle Joystick
		if ((switches == JOY_LEFT) && (option > 0) && (!asking_yesno))
		{
			if ((option == 2) || ((cap_allowed) && (option == 1)))
			{
				option--;

				// move glow obj.
				glow_obj->x = option_xys[ver][option][X_VAL];
				generate_sprite_verts(glow_obj);

				if (option == 1)
				{
					// hilite ENTER NAME
					change_img_tmu(cap_txt_obj,&cpwrds,OPT_SCR_TID);
					change_img_tmu(init_txt_obj,&eiwrdy,OPT_SCR_TID);
					change_img_tmu(sel_txt_obj,&stwrds,OPT_SCR_TID);
				}
				else if (option == 0)
				{
					// hilite CREATE PLAY
					change_img_tmu(cap_txt_obj,&cpwrdy,OPT_SCR_TID);
					change_img_tmu(init_txt_obj,&eiwrds,OPT_SCR_TID);
					change_img_tmu(sel_txt_obj,&stwrds,OPT_SCR_TID);

					set_text_z_inc(0.01f);

					set_string_id(0x45);
					set_text_font(FONTID_NBA10);
					set_text_color(LT_CYAN);
					set_text_position(option_xys[ver][0][X_VAL]-5, option_xys[ver][0][Y_VAL]-67.0f, 1.2F);
					oprintf("%dj%dcTHIS OPTION HAS", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);	  
					set_text_position(option_xys[ver][0][X_VAL]-5, option_xys[ver][0][Y_VAL]-82.0f-(is_low_res ? 2 : 0), 1.2F-(is_low_res ? .1f : 0.0f));
					oprintf("%dj%dcNO GAME PLAY", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
				}
				snd_scall_bank(cursor_bnk_str,OPTION_CURSR_SND,VOLUME7,127,PRIORITY_LVL2);
			}
		}
		else if ((switches == JOY_RIGHT) && (option != 2) && (!asking_yesno))
		{
			option++;

			// move glow obj.
			glow_obj->x = option_xys[ver][option][X_VAL];
			generate_sprite_verts(glow_obj);


			// delete text above CREATE PLAY option
			delete_multiple_strings(0x45, 0xffffffff);

			if (option == 1)
			{
				// hilite ENTER NAME
				change_img_tmu(cap_txt_obj,&cpwrds,OPT_SCR_TID);
				change_img_tmu(init_txt_obj,&eiwrdy,OPT_SCR_TID);
				change_img_tmu(sel_txt_obj,&stwrds,OPT_SCR_TID);
			}
			else
			{
				// hilite SELECT TEAM
				change_img_tmu(cap_txt_obj,&cpwrds,OPT_SCR_TID);
				change_img_tmu(init_txt_obj,&eiwrds,OPT_SCR_TID);
				change_img_tmu(sel_txt_obj,&stwrdy,OPT_SCR_TID);
			}
			snd_scall_bank(cursor_bnk_str,OPTION_CURSR_SND,VOLUME7,127,PRIORITY_LVL2);
		}
		else if ((switches == JOY_UP) && (asking_yesno) && (option == 0) && (answer != 1))
		{
			answer = 1;
			change_img_tmu(yes_plq_obj,&yn_blu,OPT_SCR_TID);
			change_img_tmu(no_plq_obj,&yn_blk,OPT_SCR_TID);
			// move glow obj.
			//glow_obj->x = option_xys[ver][0][X_VAL];
			glow_obj->y = option_xys[ver][0][Y_VAL]+30.0f;
			generate_sprite_verts(glow_obj);
			snd_scall_bank(plyr_bnk_str,TMSEL_CURSR_SND,VOLUME6,127,PRIORITY_LVL3);
		}
		else if ((switches == JOY_DOWN) && (asking_yesno) && (option == 0) && (answer != 0))
		{
			answer = 0;
			change_img_tmu(yes_plq_obj,&yn_blk,OPT_SCR_TID);
			change_img_tmu(no_plq_obj,&yn_blu,OPT_SCR_TID);
			// move glow obj.
			//glow_obj->x = option_xys[ver][0][X_VAL];
			glow_obj->y = option_xys[ver][0][Y_VAL]-30.0f;
			generate_sprite_verts(glow_obj);
			snd_scall_bank(plyr_bnk_str,TMSEL_CURSR_SND,VOLUME6,127,PRIORITY_LVL3);
		}
		// Handle BUTTONS
		if (buttons)
		{
			// if on CAP option... ask YES/NO
			if ((option == 0) && (!asking_yesno))
			{
				asking_yesno = 1;
				answer = 0;				// default to NO
				yes_plq_obj = beginobj(&yn_blk, option_xys[ver][0][X_VAL], option_xys[ver][0][Y_VAL]+30.0f, 45.0F, OPT_SCR_TID);
				yes_plq_obj->id = OID_MISC;
				no_plq_obj = beginobj(&yn_blu,  option_xys[ver][0][X_VAL], option_xys[ver][0][Y_VAL]-30.0f, 45.0F, OPT_SCR_TID);
				no_plq_obj->id = OID_MISC;
				no_obj = beginobj(&nowrd,   option_xys[ver][0][X_VAL]-2, option_xys[ver][0][Y_VAL]-26.0f, 15.0F, OPT_SCR_TID);
				no_obj->id = OID_MISC;
				yes_obj = beginobj(&yeswrd, option_xys[ver][0][X_VAL]-1, option_xys[ver][0][Y_VAL]+34.0f, 15.0F, OPT_SCR_TID);
				yes_obj->id = OID_MISC;
				hide_sprite(cap_txt_obj);

   				// Ask "Are you sure?"
   				set_text_z_inc(0.01f);

   				set_string_id(0x45);
   				set_text_font(FONTID_NBA10);
   				set_text_position(option_xys[ver][0][X_VAL]-5, option_xys[ver][0][Y_VAL]+83.0f, 1.2F);
   				oprintf("%dj%dcARE YOU SURE?", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);	  

				snd_scall_bank(cursor_bnk_str,SELECT_SND1,VOLUME7,127,PRIORITY_LVL3);
			}
			else if ((option == 0) && (answer == 0))
			{
				// they decided NO for create a play
				asking_yesno = 0;
				snd_scall_bank(cursor_bnk_str,SELECT_SND1,VOLUME7,127,PRIORITY_LVL3);
				disc_flash(no_plq_obj,SPFX_TID,0.0f,0.0f,0.94f,2);
				del1c(OID_MISC,-1);
				unhide_sprite(cap_txt_obj);
			}
			else
			{
				option_selected = 1;
				hide_sprite(glow_obj);
			}
		}

		// Update glow obj img
		if ((option == 0) && (asking_yesno))
		{
			glow_obj->y = option_xys[ver][0][Y_VAL]-30;
			change_img_tmu(glow_obj,&yn_blu2,OPT_SCR_TID);			// change to smaller glow
		}
		else
		{
			glow_obj->y = option_xys[ver][0][Y_VAL];
			change_img_tmu(glow_obj,&disc2,OPT_SCR_TID);			// change to larger glow
		}

		// update glow obj. aphla
		glow_obj->state.constant_color &= 0x00FFFFFF;
		glow_obj->state.constant_color |= alpha_level << 24;

		sleep(1);

		alpha_level += delta;
		if (alpha_level < 128)
		{
			alpha_level = 128;
			delta = +8;
		}
		else if (alpha_level > 255)
		{
			alpha_level = 255;
			delta = -8;
		}

		btn_dly--;

	}// while

	// OPTION was selected
	plyrs = count_players_in_game();

	snd_scall_bank(cursor_bnk_str,OPTION_SELECT_SND,VOLUME7,127,PRIORITY_LVL4);

	// flash option
	if ((option == 0) && (answer == 1))
		disc_flash(yes_plq_obj,SPFX_TID,0.0f,0.0f,0.95f,2);
//		disc_flash(cap_plq_obj,SPFX_TID,0.0f,0.0f,1.6f,2);
	else if (option == 1)
		disc_flash(init_plq_obj,SPFX_TID,0.0f,0.0f,1.55f,2);
	else
		disc_flash(sel_plq_obj,SPFX_TID,0.0f,0.0f,1.55f,2);


	// let flash finish
	sleep(25);

	if ((option == 0) && ((count_players_in_game() > plyrs) || (pdata_blocks[pnum].quarters_purchased > 1)))
	{
						// oh shit... someone bought in during flash and sleep... goto name entry!
		option = 1;		// goto NAME ENTRY
	}
	exit_status &= ~(1<<pnum);								// CLEAR players bit in variable
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void halftime_substitutions(void)
{
	sprite_info_t	*sprite_obj;
	int				i;
	int				ver;
	int				show_box;
	int				exit_box;
	int				old_pstatus;
	sprite_info_t	*idiot_bx_obj;
	const struct stat_plyr_data * p_spd;


	// Turn on start buttons
	start_enable(TRUE);

	// get rid of current head/ body models
	unload_player_heads();
	unload_player_models();
	// delete head textures
	// delete player textures
	unlock_multiple_textures(TXID_HEADS, -1);
	delete_multiple_textures(TXID_HEADS, -1);


	ver = four_plr_ver;
//	if (load_textures_into_tmu(halftime_substitution_textures_tbl))
//	{
//#ifdef DEBUG
//		fprintf(stderr, "Couldn't load all halftime substitution textures into tmu\r\n");
//		lockup();
//#else
//		return;
//#endif
//	}

	// load team 1's mugshots
	p_spd = teaminfo[game_info.team[0]].p_spd;
	i = teaminfo[game_info.team[0]].num_player;
	while (i--)
	{
		load_mugshot_tpage(p_spd->pii_mug);
		p_spd++;
		sleep(1);
	}
	// load team 2's mugshots
	p_spd = teaminfo[game_info.team[1]].p_spd;
	i = teaminfo[game_info.team[1]].num_player;
	while (i--)
	{
		load_mugshot_tpage(p_spd->pii_mug);
		p_spd++;
		sleep(1);
	}

	// load the guest mugshots if any
	for (i = 0; i < NUM_PLAYERS; i++)
	{
		if (is_guest[i])
		{
			load_mugshot_tpage(selected_players[i].player_data->pii_mug);
			#if DEBUG
			printf("loaded IS_GUEST mugshot for player %d\n",i);
			#endif
		}
	}


//for (i = 0; i < NUM_PLAYERS; i++)
//{
//	printf("b4 halftime subs^^^^^^^ selected_player = %d\n",selected_players[i].player_index);
//}

	// Disable the disk interrupt callback
//	dio_mode(DIO_MODE_BLOCK);

	// must LOCK these textures so that one player doesnt delete them for the other player
	lock_multiple_textures(MUG_TID, -1);
	lock_multiple_textures(TM_SELECT_TID, -1);

	exit_status = 0;
	force_selection = 0;
	
	// draw background
	sprite_obj = beginobj(&backgrnd,   		  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	sprite_obj->id = 1;
	sprite_obj = beginobj(&backgrnd_c1,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	sprite_obj->id = 1;
	sprite_obj = beginobj(&backgrnd_c2,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	sprite_obj->id = 1;
	sprite_obj = beginobj(&backgrnd_c3,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	sprite_obj->id = 1;

	// create the TITLE bar
	sprite_obj = beginobj(&slcmidbr, 	title_bar_xys[ver][0][X_VAL], title_bar_xys[ver][0][Y_VAL], 14.0F, TM_SELECT_TID);
	sprite_obj->id = 1;
	sprite_obj = beginobj(&slcmidbr_c1, title_bar_xys[ver][0][X_VAL], title_bar_xys[ver][0][Y_VAL], 14.0F, TM_SELECT_TID);
	sprite_obj->id = 1;
	sprite_obj = beginobj(&slcmidbr_c2, title_bar_xys[ver][0][X_VAL], title_bar_xys[ver][0][Y_VAL], 14.0F, TM_SELECT_TID);
	sprite_obj->id = 1;
	sprite_obj = beginobj(&slcmidbr_c3, title_bar_xys[ver][0][X_VAL], title_bar_xys[ver][0][Y_VAL], 14.0F, TM_SELECT_TID);
	sprite_obj->id = 1;

	// print title msg.
	set_string_id(0x01);
	set_text_z_inc(0.01f);
	set_text_font(FONTID_NBA23);		/* FONTID_NBA25 */
	set_text_color(LT_YELLOW);
	set_text_position((SPRITE_HRES/2.0f)+25.0f,333.0f, 2.0F);
	oprintf("%djSUBStITUTIONS", (HJUST_CENTER|VJUST_CENTER));
	
	// credit msg on bottom of screen
	qcreate("cred_msg",CREDIT_PID,credit_msg,0,JAPPLE_BOX_TID,0,0);

	qcreate("timer",TIMER_PID,timedown,15,9,SPRITE_HRES/2.0f,SPRITE_VRES/2.0f-85.0f);			//wait 25 seconds cnt from 9

	// japple boxes
	qcreate("credbx1",CREDIT_PID,plyr_credit_box,0,station_xys[0][four_plr_ver][X_VAL],station_xys[0][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
	qcreate("credbx3",CREDIT_PID,plyr_credit_box,2,station_xys[2][four_plr_ver][X_VAL],station_xys[2][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
	// "join in" msgs
	qcreate("p1_chal",P1_CHALNGR_PID,challenger_needed_msg,PLYR_1,JAPPLE_BOX_TID,0,0);
	qcreate("p3_chal",P3_CHALNGR_PID,challenger_needed_msg,PLYR_3,JAPPLE_BOX_TID,0,0);

	// roster selection procs
	if (four_plr_ver)
	{
		qcreate("credbx2",CREDIT_PID,plyr_credit_box,1,station_xys[1][four_plr_ver][X_VAL],station_xys[1][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
		qcreate("p2_chal",P2_CHALNGR_PID,challenger_needed_msg,PLYR_2,JAPPLE_BOX_TID,0,0);
		qcreate("credbx4",CREDIT_PID,plyr_credit_box,3,station_xys[3][four_plr_ver][X_VAL],station_xys[3][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
		qcreate("p4_chal",P4_CHALNGR_PID,challenger_needed_msg,PLYR_4,JAPPLE_BOX_TID,0,0);
	}

	qcreate("selplr",plyr_sel_pids[0],player_select_roster_cntrl,0,0,0,0);
	qcreate("selplr",plyr_sel_pids[2],player_select_roster_cntrl,1,2,0,0);
	if (four_plr_ver)
	{
		qcreate("selplr",plyr_sel_pids[1],player_select_roster_cntrl,0,1,0,0);
		qcreate("selplr",plyr_sel_pids[3],player_select_roster_cntrl,1,3,0,0);
	}

	hidden_attrib = 0;
	qcreate("hidatr", HIDDEN_ATTRIB_PID, hidden_attrib_flasher, 0, 0, 0, 0);

	// show screen
	turn_transit_off(1);

	do
	{
		sleep(1);
		if (force_selection)
			break;
	} while (((existp(plyr_sel_pids[0], -1) != NULL) && (p_status & (1<<0))) ||
			 ((existp(plyr_sel_pids[1], -1) != NULL) && (p_status & (1<<1))) ||
			 ((existp(plyr_sel_pids[2], -1) != NULL) && (p_status & (1<<2))) ||
			 ((existp(plyr_sel_pids[3], -1) != NULL) && (p_status & (1<<3))));

	// hide screen
	turn_transit_on();

	// Turn on start buttons
	start_enable(FALSE);

	// kill roster select procs
	killall(plyr_sel_pids[0],-1);										//	kill_countdown_timer();
	killall(plyr_sel_pids[1],-1);										//	kill_countdown_timer();
	killall(plyr_sel_pids[2],-1);										//	kill_countdown_timer();
	killall(plyr_sel_pids[3],-1);										//	kill_countdown_timer();
	killall(HIDDEN_ATTRIB_PID, -1);
	del1c(2084, -1);


	// at this point... reload player sound banks
#ifndef NOAUDIO
	sleep(1);
	snd_bank_delete(game_tune_bnk_str);

	coin_sound_holdoff(TRUE);

	// If game isn't over, load next quarter game tune
	if (game_info.game_quarter == 2)
	{
		// swap MAKE shot speech bank
		snd_bank_delete(anc_make_bnk);

		unload_player_speech_banks();
		load_player_speech_banks();
		

		strcpy(anc_make_bnk, "sca_mak2");
		if (snd_bank_load_playing(anc_make_bnk) == 0xeeee)
				increment_audit(SND_LOAD_FAIL_AUD);
	}
	// load next qtr. game tune

	if (COURT_IS_OUTDOORS)
		strcpy(game_tune_bnk_str, game_outdoor_tunes_tbl[game_info.game_quarter]);
	else
		strcpy(game_tune_bnk_str, game_tunes_tbl[game_info.game_quarter]);


	if (snd_bank_load_playing(game_tune_bnk_str) == 0xeeee)
		increment_audit(SND_LOAD_FAIL_AUD);

	coin_sound_holdoff(FALSE);
#endif

	del1c(0,-1);
	del1c(1,-1);
	del1c(OID_JAPPLE_BOX,-1);
	delete_multiple_strings(0x01, -1);

	for (i=0; i < NUM_PLAYERS; i++)
	{
		delete_multiple_strings(roster_name_ids[i],-1);
		delete_multiple_strings(roster_stat_ids[i],-1);
	}

	// kill "join in" msgs
	killall( P1_CHALNGR_PID, -1 );
	killall( P2_CHALNGR_PID, -1 );
	killall( P3_CHALNGR_PID, -1 );
	killall( P4_CHALNGR_PID, -1 );

	// stop countdown timer and delete it
	killall(TIMER_PID,-1);										//	kill_countdown_timer();
	del1c(OID_CNTDWN_DIGIT,-1);

	delete_multiple_strings(TIMER_MSG_ID, -1);
	// kill credit counter
	killall(CREDIT_PID,-1);
	delete_multiple_strings(CREDIT_MSG_ID, -1);
	delete_multiple_strings(CREDIT_MSG_ID+1, -1);

	// free textures
	unlock_multiple_textures(MUG_TID,-1);
	delete_multiple_textures(MUG_TID,-1);
	unlock_multiple_textures(TM_SELECT_TID,-1);
	delete_multiple_textures(TM_SELECT_TID,-1);
	delete_multiple_textures(BKGRND_TID,-1);
	delete_multiple_textures(JAPPLE_BOX_TID,-1);

	for (i = 0; i < NUM_PLAYERS; i++)
	{
		int which_model;
		
		if (!is_guest[i])
			set_player_record(selected_players[i].team, selected_players[i].player_index, FALSE, &selected_players[i]);
		if (selected_players[i].team == TEAM_SECRET)
			which_model = selected_players[i].player_index;
		else
			which_model = NBA_PLAYER;
		load_player_head(FALSE, i, selected_players[i].team, selected_players[i].player_index,
							&player_load_info[i].head_model, &player_load_info[i].head_scale);
		load_player_model(which_model, &player_load_info[i].body_model, &player_load_info[i].jnum_model, &player_load_info[i].reflect_model);
	}

	load_plyr_txtrs(game_info.team[0], game_info.team[1]);
	for (i=0; i < NUM_PLAYERS; i++)
		reinit_player_data(i,i >= NUM_PERTEAM);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
int court_selection(void)
{
	sprite_info_t	*sprite_obj;
	int				i,j;
	int				ver;


	// only do if player has the privilege
#if 1
	for(j=FALSE,i=0; i < MAX_PLYRS; i++)
	{
		if ((selected_players[i].record_type == CREATP_RECORD) && (selected_players[i].cmos_data.privileges & PRIV_COURTSEL))
		{
			j = TRUE;
			break;
		}
	}

	if (j == FALSE)
		return(0);
#endif
	if (alt_ladder_active)
		return 0;

	// delete team select and chose roster images
	killall(TM2_TM_SELECT_PID,-1);								//	team select procs
	killall(TM1_TM_SELECT_PID,-1);								//	team select procs
	killall(TIMER_PID,-1);										//	team select procs
//	killall(CREDIT_PID,-1);										//	kill_countdown_timer();
	delete_multiple_strings(0x01,-1);
	delete_multiple_strings(0x34,-1);
	for (i=0; i < 4; i++)
	{
		delete_multiple_strings(roster_name_ids[i],-1);
		delete_multiple_strings(roster_stat_ids[i],-1);
		delete_multiple_strings(roster_name2_ids[i],-1);
	}
	del1c(0,-1);
	del1c(10,-1);
	del1c(11,-1);
	del1c(tmsel_ids[0],-1);
	del1c(tmsel_ids[1],-1);
	del1c(OID_CNTDWN_DIGIT,-1);
	killall(HIDDEN_ATTRIB_PID, -1);
	del1c(2084, -1);
//	delete_all_strings();

	// print title msg.
	set_string_id(0x0885);
	set_text_z_inc(0.01f);
	set_text_font(FONTID_NBA23);
	set_text_color(LT_YELLOW);
	set_text_position((SPRITE_HRES/2.0f)+25.0f,335.0f, 2.0F);
	oprintf("%djCHOOSE COURT", (HJUST_CENTER|VJUST_CENTER));

//	qcreate("cred_msg",CREDIT_PID,credit_msg,0,JAPPLE_BOX_TID,0,0);
//
//	qcreate("credbx1",CREDIT_PID,plyr_credit_box,0,station_xys[0][four_plr_ver][X_VAL],station_xys[0][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
//	qcreate("credbx2",CREDIT_PID,plyr_credit_box,1,station_xys[1][four_plr_ver][X_VAL],station_xys[1][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
//	// messages for japple boxes (if needed)
//	qcreate("p1_chal",P1_CHALNGR_PID,challenger_needed_msg,PLYR_1,JAPPLE_BOX_TID,0,0);
//	qcreate("p2_chal",P2_CHALNGR_PID,challenger_needed_msg,PLYR_2,JAPPLE_BOX_TID,0,0);
//
//	if (four_plr_ver)
//	{
//		qcreate("credbx3",CREDIT_PID,plyr_credit_box,2,station_xys[2][four_plr_ver][X_VAL],station_xys[2][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
//		qcreate("credbx4",CREDIT_PID,plyr_credit_box,3,station_xys[3][four_plr_ver][X_VAL],station_xys[3][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
//		qcreate("p3_chal",P3_CHALNGR_PID,challenger_needed_msg,PLYR_3,JAPPLE_BOX_TID,0,0);
//		qcreate("p4_chal",P4_CHALNGR_PID,challenger_needed_msg,PLYR_4,JAPPLE_BOX_TID,0,0);
//	}

	exit_status = 0;
	force_selection = 0;

	qcreate("timer",TIMER_PID,timedown,5,9,(SPRITE_HRES/2.0f),(SPRITE_VRES/2.0f)-107.0f);							//wait 1 second then cnt from 9
	qcreate("slctcrt",0,plyr_select_court_proc,0,0,0,0);

	sleep(2);			// allow above processes to start
	
	sleep(20);			// Delay snd
	// Tinkle snd for special menu
	snd_scall_bank(cursor_bnk_str, 57, VOLUME7, 127, PRIORITY_LVL5);
//	draw_enable(1);
//	turn_transit_off(0);

	do
	{
		sleep(1);
	} while (exit_status);

	return(1);
//	turn_transit_on();

	//
	// Now delete all objects and textures from this screen
	//
//	killall(TIMER_PID,-1);										//	kill_countdown_timer();
//	del1c(OID_CNTDWN_DIGIT,-1);

//	delete_multiple_strings(CREDIT_MSG_ID, -1);
//	delete_multiple_strings(0x01, -1);		// delete title bar msg.
//	delete_multiple_strings(0x45, -1);		// court name strings
//	del1c(0, -1);
//	del1c(1, -1);
//	del1c(OID_JAPPLE_BOX, -1);

//	killall(CREDIT_PID,-1);										//	kill_countdown_timer();
//	killall(P1_CHALNGR_PID,-1);
//	killall(P2_CHALNGR_PID,-1);
//	if (four_plr_ver)
//	{
//		killall(P3_CHALNGR_PID,-1);
//		killall(P4_CHALNGR_PID,-1);
//	}

//	unlock_multiple_textures(SPFX_TID,-1);			// not unlocked until end of team select
//	delete_multiple_textures(SPFX_TID,-1);
//	unlock_multiple_textures(BKGRND_TID,-1);
//	delete_multiple_textures(BKGRND_TID,-1);
//	unlock_multiple_textures(OPT_SCR_TID,-1);
//	delete_multiple_textures(OPT_SCR_TID,-1);
//	unlock_multiple_textures(JAPPLE_BOX_TID,-1);
//	delete_multiple_textures(JAPPLE_BOX_TID,-1);
//	unlock_multiple_textures(CNTDWN_TID,-1);			// not unlocked until end of team select
//	delete_multiple_textures(CNTDWN_TID,-1);			// not unlocked until end of team select
}


char *city_name_strs[] =
{
"atlanta",
"boston",
"charlotte",
"chicago",
"cleveland",
"dallas",
"denver",
"detroit",
"golden state",
"houston",
"indiana",
"los angeles",
"los angeles",
"miami",
"milwaukee",
"minnesota",
"new jersey",
"new york",
"orlando",
"philadelphia",
"phoenix",
"portland",
"sacramento",
"san antonio",
"seattle",
"toronto",
"utah",
"vancouver",
"washington",
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void plyr_select_court_proc(int *args)
{
	int				pnum;//,plyrs;
	int				switches,buttons,btn_dly;
	int				option_selected;
	int				ver;
	int				i;
	int				alt_courts;
	ostring_t		*option1_str, *option2_str;
	sprite_info_t 	*psprite;
	sprite_info_t 	*cursor_obj = NULL;
	sprite_info_t 	*glow_obj = NULL;
	sprite_info_t 	*flash_obj = NULL;
	sprite_info_t 	*flash2_obj = NULL;
	long			alpha_level, delta;
	float			name_bx_ys[][4] = {{251.0f,211.0f,170.0f,130.0f},{249.0f,209.0f,169.0f,129.0f}};
	float			cursr_ys[] = {252.0f,212.0f,172.0f,132.0f};


	// for lack of wanting to type! :)
	ver = four_plr_ver;

	// create the option plates
	psprite = beginobj(&courtsel, SPRITE_HRES/2.0f, SPRITE_VRES/2.0f, 60.0F, OPT_SCR_TID);
	psprite->id = 1;

	// print court options
	set_text_z_inc(-0.01f);
	set_string_id(0x45);
	set_text_font(is_low_res ? FONTID_NBA10 : FONTID_NBA12);

	// team 1 court	
	set_text_position(SPRITE_HRES/2.0f, name_bx_ys[is_low_res][0], 40.0f);
	option1_str = oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, city_name_strs[game_info.team[0]]);
//	beginobj(teaminfo[game_info.team[0]].pii_cn,SPRITE_HRES/2.0f, name_bx_ys[0], 45.0F, OPT_SCR_TID);

	// team 2 court	
	set_text_position(SPRITE_HRES/2.0f, name_bx_ys[is_low_res][1], 40.0f);
	option2_str = oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, city_name_strs[game_info.team[1]]);
//	beginobj(teaminfo[game_info.team[1]].pii_cn,SPRITE_HRES/2.0f, name_bx_ys[1], 45.0F, OPT_SCR_TID);

	// street court
	set_text_position(SPRITE_HRES/2.0f, name_bx_ys[is_low_res][2], 40.0f);
	oprintf("%dj%dcSTREET", (HJUST_CENTER|VJUST_CENTER), WHITE);

	// jungle court
	set_text_position(SPRITE_HRES/2.0f, name_bx_ys[is_low_res][3], 40.0f);
	oprintf("%dj%dcISLAND", (HJUST_CENTER|VJUST_CENTER), WHITE);

	// set variables
	exit_status = 1;
	option_selected = 0;			// no
	option = 0;
	alpha_level = 255;
	delta = -8;
	alt_courts = 0;

//	plyrs = (ver ? 4 : 2);

	btn_dly = 30;

	cursor_obj = beginobj(&cs_curs, SPRITE_HRES/2.0f, cursr_ys[option], 50.0F, OPT_SCR_TID);
	cursor_obj->id = 1;
	glow_obj = beginobj(&cs_curs2, SPRITE_HRES/2.0f, cursr_ys[option], 45.0F, OPT_SCR_TID);
//	glow_obj->state.alpha_rgb_dst_function = GR_BLEND_ONE;			// set additive blending!!
	glow_obj->id = 1;

	// control loop
	while (!option_selected)
	{
		// Read INPUT
		switches = 0;
		buttons = 0;
		// read joysticks from any active player
//		for (i=0; i < plyrs; i++)
		for (i=0; i < MAX_PLYRS; i++)
		{
			if ((p_status & (1<<i)) && (switches = get_joy_val_down(i)))
				break;
		}
		if (btn_dly > 0)
			buttons = 0;
		else
		{
			// read buttons from any active player
//			for (i=0; i < plyrs; i++)
			for (i=0; i < MAX_PLYRS; i++)
			{
				if ((p_status & (1<<i)) && (buttons = get_but_val_down(i))) {
					if (buttons == TURBO_BUTTON) {			/* turbo toggles the alternate courts */
						char *str1, *str2;
						
						snd_scall_bank(cursor_bnk_str, 27, VOLUME4, 127, PRIORITY_LVL4);
						if (alt_courts) {
							str1 = city_name_strs[game_info.team[0]];
							str2 = city_name_strs[game_info.team[1]];
						} else {
							str1 = "MIDWAY";
							str2 = "NBC";
						}
						delete_string(option1_str);
						delete_string(option2_str);
						set_text_font(is_low_res ? FONTID_NBA10 : FONTID_NBA12);
						set_text_position(SPRITE_HRES/2.0f, name_bx_ys[is_low_res][0], 40.0f);
						option1_str = oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, str1);
						set_text_position(SPRITE_HRES/2.0f, name_bx_ys[is_low_res][1], 40.0f);
						option2_str = oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, str2);
						alt_courts ^= 1;
						buttons = 0;
					}
					break;
				}
			}
		}
		
		// if screen times out... default to SELECT TEAM
		if (force_selection)
			buttons = 1;							// and select it!

		// Handle Joystick
		if (((switches == JOY_UP) || (switches == JOY_LEFT)) && (option > 0))
		{
			option--;

			// move glow obj.
			cursor_obj->y = cursr_ys[option];
			glow_obj->y = cursr_ys[option];
			generate_sprite_verts(cursor_obj);
			generate_sprite_verts(glow_obj);

			snd_scall_bank(cursor_bnk_str,OPTION_CURSR_SND,VOLUME7,127,PRIORITY_LVL2);
		}
		else if (((switches == JOY_DOWN) || (switches == JOY_RIGHT)) && (option < 3))
		{
			option++;

			cursor_obj->y = cursr_ys[option];
			glow_obj->y = cursr_ys[option];
			generate_sprite_verts(cursor_obj);
			generate_sprite_verts(glow_obj);

			snd_scall_bank(cursor_bnk_str,OPTION_CURSR_SND,VOLUME7,127,PRIORITY_LVL2);
		}
		// Handle BUTTONS
		if (buttons)
		{
			snd_scall_bank(cursor_bnk_str,SELECT_SND1,VOLUME7,127,PRIORITY_LVL3);
			option_selected = 1;
		}

		// update glow obj. aphla
		glow_obj->state.constant_color &= 0x00FFFFFF;
		glow_obj->state.constant_color |= alpha_level << 24;

		sleep(1);

		alpha_level += delta;
		if (alpha_level < 128)
		{
			alpha_level = 128;
			delta = +8;
		}
		else if (alpha_level > 255)
		{
			alpha_level = 255;
			delta = -8;
		}

		btn_dly--;

	}// while

	snd_scall_bank(cursor_bnk_str,OPTION_SELECT_SND,VOLUME7,127,PRIORITY_LVL4);

	//
	// SET WHICH COURT!!!! for 'Arena proc'
	//
	if (option == 0)
		which_court = alt_courts ? COURT_MIDWAY : COURT_INDOOR;
	else if (option == 1)
		which_court = alt_courts ? COURT_NBC : COURT_INDOOR2;
	else if (option == 2)
		which_court = COURT_GILMORE;
	else
		which_court = COURT_GEER;


	// flash option
	hide_sprite(glow_obj);

	flash_obj = beginobj(optscr_flsh_imgs[0],
						 (cursor_obj->x-cursor_obj->ii->ax + (cursor_obj->ii->w / 2.0f))+0.0f,
						 (cursor_obj->y+cursor_obj->ii->ay + (cursor_obj->ii->h / 2.0f))+4.0f,
						 5.0f,
						 SPFX_TID);

	flash2_obj = beginobj(optscr_flsh2_imgs[0],
						 (cursor_obj->x-cursor_obj->ii->ax + (cursor_obj->ii->w / 2.0f))+0.0f,
						 (cursor_obj->y+cursor_obj->ii->ay + (cursor_obj->ii->h / 2.0f))+4.0f,
						 4.0f,
						 SPFX_TID);

	flash_obj->w_scale = 1.61f;
	flash_obj->h_scale = 0.51f;
	// set to additive blending
	flash_obj->state.alpha_rgb_dst_function = GR_BLEND_ONE;
	generate_sprite_verts(flash_obj);

	flash2_obj->w_scale = 2.50f;
	flash2_obj->h_scale = 1.85f;
	// set to additive blending
	flash2_obj->state.alpha_rgb_dst_function = GR_BLEND_ONE;
	generate_sprite_verts(flash2_obj);

	for (i=0; i < (sizeof(optscr_flsh_imgs)/sizeof(image_info_t *))-1; i++)
	{
		change_img_tmu(flash_obj,optscr_flsh_imgs[i],SPFX_TID);
		change_img_tmu(flash2_obj,optscr_flsh2_imgs[i],SPFX_TID);
		sleep(2);
	}

	hide_sprite(flash_obj);
	hide_sprite(flash2_obj);

	sleep(25);

	exit_status = 0;
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void team_selection(void)
{
	sprite_info_t	*sprite_obj;
	int				i;
	int				ver;
	int				show_box;
	int				exit_box;
	int				old_pstatus;
	int				court_selected = 0;
	sprite_info_t	*idiot_bx_obj;
	//int				was_low_res = is_low_res;

	// Clean up bonus flashing message
	delete_multiple_strings(1, 0xffffffff);
	killall(BONUS_PID,0xFFFFFFFF);

	// for lack of wanting to type! :)
	ver = four_plr_ver;


	// Enable the disk interrupt callback
//	dio_mode(DIO_MODE_BLOCK);

	if (load_textures_into_tmu(team_select_textures_tbl))
	{
#ifdef DEBUG
		fprintf(stderr, "Couldn't load all team-select textures into tmu\r\n");
		lockup();
#else
		return;
#endif
	}

	// Disable the disk interrupt callback
//	dio_mode(DIO_MODE_BLOCK);

	// must LOCK these textures so that one player doesnt delete them for the other player
	lock_multiple_textures(TM_SELECT_TID, -1);
	lock_multiple_textures(TM_SEL_LOGOS_TID, -1);
//	lock_multiple_textures(JAPPLE_BOX_TID, -1);
//	lock_multiple_textures(SPFX_TID, -1);

	exit_status = 0;
	force_selection = 0;

	// draw background
	sprite_obj = beginobj(&backgrnd,   		  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	sprite_obj->id = 1;
	sprite_obj = beginobj(&backgrnd_c1,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	sprite_obj->id = 1;
	sprite_obj = beginobj(&backgrnd_c2,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	sprite_obj->id = 1;
	sprite_obj = beginobj(&backgrnd_c3,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	sprite_obj->id = 1;
	//draw_backdrop();

	// credit msg on bottom of screen
	qcreate("cred_msg",CREDIT_PID,credit_msg,0,JAPPLE_BOX_TID,0,0);

	// timer for screen
	qcreate("timer",TIMER_PID,timedown,45,9,SPRITE_HRES/2.0f,SPRITE_VRES/2.0f-85.0f);			//wait 25 seconds cnt from 9
//	qcreate("timer",TIMER_PID,timedown,5,9,SPRITE_HRES/2.0f,SPRITE_VRES/2.0f);			//wait 25 seconds cnt from 9

	// create the TITLE bar
	sprite_obj = beginobj(&slcmidbr,    title_bar_xys[ver][1][X_VAL], title_bar_xys[ver][1][Y_VAL], 300.0F, TM_SELECT_TID);
	sprite_obj->id = 1;
	sprite_obj = beginobj(&slcmidbr_c1, title_bar_xys[ver][1][X_VAL], title_bar_xys[ver][1][Y_VAL], 300.0F, TM_SELECT_TID);
	sprite_obj->id = 1;
	sprite_obj = beginobj(&slcmidbr_c2, title_bar_xys[ver][1][X_VAL], title_bar_xys[ver][1][Y_VAL], 300.0F, TM_SELECT_TID);
	sprite_obj->id = 1;
	sprite_obj = beginobj(&slcmidbr_c3, title_bar_xys[ver][1][X_VAL], title_bar_xys[ver][1][Y_VAL], 300.0F, TM_SELECT_TID);
	sprite_obj->id = 1;
	// print title msg.
	set_string_id(0x01);
	set_text_z_inc(0.01f);
	set_text_font(FONTID_NBA23);		/* FONTID_NBA25 */
	set_text_color(LT_YELLOW);
	set_text_position((SPRITE_HRES/2.0f)+25.0f,335.0f, 200.0F);
	oprintf("%djCHOOSE TEAM", (HJUST_CENTER|VJUST_CENTER));

	// japple boxes
	qcreate("credbx1",CREDIT_PID,plyr_credit_box,0,station_xys[0][four_plr_ver][X_VAL],station_xys[0][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
	qcreate("credbx3",CREDIT_PID,plyr_credit_box,2,station_xys[2][four_plr_ver][X_VAL],station_xys[2][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);

	if (four_plr_ver)
	{
		qcreate("credbx2",CREDIT_PID,plyr_credit_box,1,station_xys[1][four_plr_ver][X_VAL],station_xys[1][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
		qcreate("credbx4",CREDIT_PID,plyr_credit_box,3,station_xys[3][four_plr_ver][X_VAL],station_xys[3][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
	}

	// messages for japple boxes (if needed)
	qcreate("p1_chal",P1_CHALNGR_PID,challenger_needed_msg,PLYR_1,JAPPLE_BOX_TID,0,0);
	qcreate("p3_chal",P3_CHALNGR_PID,challenger_needed_msg,PLYR_3,JAPPLE_BOX_TID,0,0);

	if (four_plr_ver)
	{
		qcreate("p2_chal",P2_CHALNGR_PID,challenger_needed_msg,PLYR_2,JAPPLE_BOX_TID,0,0);
		qcreate("p4_chal",P4_CHALNGR_PID,challenger_needed_msg,PLYR_4,JAPPLE_BOX_TID,0,0);
	}

	qcreate("tm1_sel",TM1_TM_SELECT_PID,team_select_cntrl,SIDE_0,PLYR_1,0,0);
	qcreate("tm2_sel",TM2_TM_SELECT_PID,team_select_cntrl,SIDE_1,PLYR_3,0,0);

	hidden_attrib = 0;
	qcreate("hidatr", HIDDEN_ATTRIB_PID, hidden_attrib_flasher, 0, 0, 0, 0);
		
	sleep(2);			// allow above processes to start

	turn_transit_off(0);

	// wait until player(s) are done selecting their teams
allow_new:
	do
	{
		sleep(1);
	} while (exit_status);

	// stop countdown timer
	killall(TIMER_PID,0xFFFFFFFF);										//	kill_countdown_timer();
	// delete timer image
	delete_multiple_strings(TIMER_MSG_ID, 0xffffffff);

	//
	// If credits remain, show a box and force humans to use them up - allow new human to join, etc.
	//
	show_box = 0;

	// Never show the box if we are in free play
	if (!coin_check_freeplay())
	{
		for (i = 0; i < MAX_PLYRS; i++)
		{
			if ((p_status & (1<<i)) &&							// player in game?
				pdata_blocks[i].quarters_purchased < 4 &&		// not bought full game?
				check_credits_to_continue())					// more credits to eat?
			{
				show_box |= 1<<i;								// YES - Show the box
			}
		}
	}

	old_pstatus = p_status;
	if (show_box)
	{
		set_text_z_inc(0.01f);

//		show_plysel_idiot_box = show_plysel_idiot_box + 10;
		idiot_bx_obj = beginobj(&playresult, SPRITE_HRES/2.0f, SPRITE_VRES/2.0f, 2.4f, SCR_PLATE_TID);
		idiot_bx_obj->w_scale = 1.52f;
		idiot_bx_obj->h_scale = 1.9f;
		idiot_bx_obj->id = OID_MESSAGE;
		generate_sprite_verts(idiot_bx_obj);

		set_string_id(0x34);
		if (is_low_res)
		{
			set_text_font(FONTID_NBA20);
			set_text_position(SPRITE_HRES/2.0f,(SPRITE_VRES/2.0f)+25.0f-2, 1.0F);
		}
		else
		{
			set_text_font(FONTID_NBA25);
			set_text_position(SPRITE_HRES/2.0f,(SPRITE_VRES/2.0f)+25.0f+2.0f, 1.0F);
		}
			
		set_text_color(LT_RED);
//		oprintf( "%djPLEASE USE UP", (HJUST_CENTER|VJUST_CENTER));
		pmsg[0] = oprintf( "%djPLEASE USE UP", (HJUST_CENTER|VJUST_CENTER));

		if (is_low_res)
			set_text_position(SPRITE_HRES/2.0f,(SPRITE_VRES/2.0f)-20.0f-7, 1.0F);
		else	
			set_text_position(SPRITE_HRES/2.0f,(SPRITE_VRES/2.0f)-20.0f, 1.0F);
			
		pmsg[1] = oprintf( "%djALL CREDITS", (HJUST_CENTER|VJUST_CENTER));
//		oprintf( "%djALL CREDITS", (HJUST_CENTER|VJUST_CENTER));
		pflasher = qcreate("pflashC", MESSAGE_PID, flash_alpha_text_proc, (int)pmsg[0], (int)pmsg[1], 0, 0);

		snd_scall_bank(cursor_bnk_str,WARNING_SND,VOLUME6,127,PRIORITY_LVL5);

		// Wait for all of the credits to get used up
		exit_box = 0;
		while (check_credits_to_continue())
		{
			sleep(1);

			if ((old_pstatus != p_status) && (!four_plr_ver))
			{
				delete_multiple_strings(0x34, -1);
				delobj(idiot_bx_obj);
				kill(pflasher, 0);
				killall(TIMER_PID,0xFFFFFFFF);										//	kill_countdown_timer();
				exit_status |= (old_pstatus^p_status);
				qcreate("timer",TIMER_PID,timedown,1,9,SPRITE_HRES/2.0f,SPRITE_VRES/2.0f);			//wait 25 seconds cnt from 9
				goto allow_new;
			}
			else
			{
				// only do this...if other team bought in.
				if ((old_pstatus != p_status) &&
					(four_plr_ver) &&
					((((old_pstatus & 12) == 0) && ((p_status & 12) != 0)) ||
					 (((old_pstatus & 3) == 0) && ((p_status & 3) != 0))) )
				{
					delete_multiple_strings(0x34, -1);
					delobj(idiot_bx_obj);
					kill(pflasher, 0);
					killall(TIMER_PID,0xFFFFFFFF);										//	kill_countdown_timer();
					exit_status |= (old_pstatus^p_status);
					qcreate("timer",TIMER_PID,timedown,1,9,SPRITE_HRES/2.0f,SPRITE_VRES/2.0f);			//wait 25 seconds cnt from 9
					goto allow_new;
				}
				old_pstatus = p_status;
			}


			if (p_status & (1<<0))
			{
				if(pdata_blocks[0].quarters_purchased >= 4)
					exit_box |= (1<<0);
			}
			if (p_status & (1<<1))
			{
				if(pdata_blocks[1].quarters_purchased >= 4)
					exit_box |= (1<<1);
			}
			if (p_status & (1<<2))
			{
				if(pdata_blocks[2].quarters_purchased >= 4)
					exit_box |= (1<<2);
			}
			if (p_status & (1<<3))
			{
				if(pdata_blocks[3].quarters_purchased >= 4)
					exit_box |= (1<<3);
			}

			// If all active plyrs have bought full game...
			if (!(p_status ^ exit_box))
				break;
		}
		delobj(idiot_bx_obj);
		delete_multiple_strings(0x34, -1);
		kill(pflasher, 0);
	}

	// If new guy bought in for first time with last remaining credits - go back to picking team for him

	// Once we get to this point, we must pick CPU team opponent (if necessary) before any SLEEP can happen!
	// otherwise, PSTATUS could get set!

	//
	// Pick opposing team if needed
	//

	/* assume that we are against an NBA team */
	alt_ladder_active = 0;
	ladder_level = 0;
	// check player 1/player 2 of team 1 (4 player version)
	if ( (!(p_status & 0x03)) && (four_plr_ver)) {
		// left team is drones
		game_info.home_team =1;
		if ((player3_data.plyr_name[0] != 0) || (player4_data.plyr_name[0] != 0)) {
			int alt_ladder;			
			alt_ladder = GREATER(player3_data.alt_ladder, player4_data.alt_ladder);
			game_info.team[0] = pick_cpu_team( player3_data.teams_defeated|player4_data.teams_defeated, alt_ladder );
			cpu_team = 0;
		} else {	// didn't enter inits, pick randomly
			game_info.team[0] = randrng(NUM_TEAMS);
			if (game_info.team[0] == game_info.team[1]) {
				if (game_info.team[0])
					game_info.team[0]--;
				else
					game_info.team[0]++;
			}
		}
		// select roster
		pick_cpu_roster(game_info.team[0], 0);
	}
	// check player 3/player 4 of team 2 (4 player version)
	if ( (!(p_status & 0x0c)) && (four_plr_ver)) {
		// right team is drones
		game_info.home_team = 0;
		if ((player1_data.plyr_name[0] != 0) || (player2_data.plyr_name[0] != 0)) {
			int alt_ladder;			
			alt_ladder = GREATER(player1_data.alt_ladder, player2_data.alt_ladder);
			game_info.team[1] = pick_cpu_team( player1_data.teams_defeated|player2_data.teams_defeated, alt_ladder );
			cpu_team = 1;
		} else {	// didn't enter inits, pick randomly
			game_info.team[1] = randrng(NUM_TEAMS);
			if( game_info.team[1] == game_info.team[0] ) {
				if (game_info.team[1])
					game_info.team[1]--;
				else
					game_info.team[1]++;
			}
		}
		// select roster
		pick_cpu_roster(game_info.team[1], NUM_PERTEAM);
	}

	// check player 1/team 1 (2 player version)
	if ( !(p_status & (1<<0)) && (!four_plr_ver)) {
		// left side is drones.  pick 'em, make right side home team
		game_info.home_team = 1;
		if (player3_data.plyr_name[0] != 0) {
			game_info.team[0] = pick_cpu_team( player3_data.teams_defeated, player3_data.alt_ladder );
			cpu_team = 0;
		} else {	// didn't enter inits, pick randomly
			game_info.team[0] = randrng(NUM_TEAMS);
			if( game_info.team[0] == game_info.team[1] ) {
				if (game_info.team[0])
					game_info.team[0]--;
				else
					game_info.team[0]++;
			}
		}
		// select roster
		pick_cpu_roster(game_info.team[0], 0);
	}
	else
	// check player 2/team 2 (2 player version)
	if ( !(p_status & (1<<2)) && (!four_plr_ver)) {
		// right side is drones.  pick 'em, make left side home team
		game_info.home_team = 0;
		if (player1_data.plyr_name[0] != 0) {
			game_info.team[1] = pick_cpu_team( player1_data.teams_defeated, player1_data.alt_ladder );
			cpu_team = 1;
		} else {	// didn't enter inits, pick randomly
			game_info.team[1] = randrng(NUM_TEAMS);
			if( game_info.team[1] == game_info.team[0] ) {
				if (game_info.team[1])
					game_info.team[1]--;
				else
					game_info.team[1]++;
			}
		}
		// select roster
		pick_cpu_roster(game_info.team[1], NUM_PERTEAM);
	}
	else
		// no drones.  random home team
		game_info.home_team = randrng(2);

	/* give them a chance to select the court */
	stick_court();

	// audit team picked
	if ( ((p_status & 1) && (!four_plr_ver)) || ((p_status & 0x3) && (four_plr_ver)) )
	{
		do_team_percent_aud(game_info.team[0]+ATLANTA_HAWKS_AUD);
	}
	if ( ((p_status & 4) && (!four_plr_ver)) || ((p_status & 0xc) && (four_plr_ver)) )
	{
		do_team_percent_aud(game_info.team[1]+ATLANTA_HAWKS_AUD);
	}

#ifdef DEBUG
	printf("team_selection\r\n");
	for (i = 0; i < MAX_PLYRS; i++)
	{
		if (selected_players[i].player_data)
		printf("  p%d: %s\r\n", i, selected_players[i].player_data->szlast);
	}
#endif

	// now chose a court!!
	court_selected = court_selection();

	// first wait for any FULLGAME BOUGHT msg...
	while (existp(FULLGAME_BOUGHT_PID, 0xFFFFFFFF))
		sleep(1);

//	if (!court_selected)
  	turn_transit_on();

	// kill processes (if not dead already)
	killall(TM1_TM_SELECT_PID,0xFFFFFFFF);
	killall(TM2_TM_SELECT_PID,0xFFFFFFFF);

	killall(P1_CHALNGR_PID,0xFFFFFFFF);
	killall(P2_CHALNGR_PID,0xFFFFFFFF);
	killall(P3_CHALNGR_PID,0xFFFFFFFF);
	killall(P4_CHALNGR_PID,0xFFFFFFFF);

	killall(CREDIT_PID,-1);
	killall(TIMER_PID,-1);										//	kill_countdown_timer();
							
	// delete all the objects with ID of 0 (thats any non screen to screen object) and 1
	del1c(OID_JAPPLE_BOX, -1);
	del1c(OID_CNTDWN_DIGIT,-1);
	del1c(0,-1);
	del1c(1,-1);
	killall(HIDDEN_ATTRIB_PID, -1);
	del1c(2084, -1);

	delete_all_strings();

	// delete textures
	unlock_multiple_textures(BKGRND_TID,-1);
	delete_multiple_textures(BKGRND_TID,-1);
	unlock_multiple_textures(CNTDWN_TID,-1);
	delete_multiple_textures(CNTDWN_TID,-1);
	unlock_multiple_textures(SPFX_TID, -1);
	delete_multiple_textures(SPFX_TID,-1);
	unlock_multiple_textures(JAPPLE_BOX_TID,-1);
	delete_multiple_textures(JAPPLE_BOX_TID,-1);
	unlock_multiple_textures(TM_SELECT_TID,-1);
	delete_multiple_textures(TM_SELECT_TID,-1);
	unlock_multiple_textures(TM_SEL_LOGOS_TID,-1);
	delete_multiple_textures(TM_SEL_LOGOS_TID,-1);
}

//-------------------------------------------------------------------------------------------------
//	   	This process handles everything to do with getting the TEAM selection from players
//-------------------------------------------------------------------------------------------------

#define	WIPE_TICKS	25

#define	MUG1_LX		128.0f - 64.0f - 15.0f
#define	MUG2_LX		128.0f - 32.0f - 15.0f
#define	MUG3_LX		128.0f
#define	MUG4_LX		128.0f + 32.0f + 15.0f
#define	MUG5_LX		128.0f + 64.0f + 15.0f

#define FRAMEY 	220.0f
#define	FRAMEZ	80.0f		// p1 on team X
#define	FRAME2Z	78.0f		// p2 on team X
#define	FRAME3Z	60.0f		// drone

#define	STATBOX_Y	FRAMEY-125.0f
#define	MUGY	FRAMEY+76.0f

#define	TM1_ROSTERX	64.0f
#define	TM2_ROSTERX	448.0f


void team_select_cntrl(int *args)
{
	short 		sw_select_delay_cnt = 40;				// # of ticks to wait before allowing team selection
	short 		sw_delay_cnt = 0;
	short	 	sw_repeat_cnt = 0;
	int 		tm_nbr;
	int 		tm_mask;
	int			butn_sws = 0;
	int			butn_sws_cur = 0;
	int			prev_joy_sws = 0;
	int			joy_sws = 0;
	int			p_num = 0;
	int			i;
	int			auto_select_tm;
	int 		rnd_team;
	int			mv_cnt;							// number of times to move once random select enabled
	int			cur_pos;						// index into alpha table and pin table
	int			tm_selected;
	int			was_low_res = is_low_res;
	long		alpha_level,delta;
	sprite_info_t *glow_obj;



	auto_select_tm = 0;						// no
	rnd_team = 0;
	mv_cnt = 0;
	tm_nbr = args[0];						// left side or right side
	cur_pos = 0;							// start on arizona
	game_info.team[tm_nbr] = -1;			// no team selected yet.

	tm_mask = TEAM_MASK(tm_nbr);

	// wait for one of the players from TEAM to join
	while (!(p_status & tm_mask))
		sleep(1);
//	exit_status |= 1<<tm_nbr;				// set TEAMS bit

	// determine which player picks team
	if (!four_plr_ver)
	{
		p_num = args[1];					// 2 player version (each player picks a team)
		exit_status |= 1<<p_num;			// set TEAMS bit
	}
	else
	{
		// 4 player version (one player from side picks a team)
		if (tm_nbr == 0)					// team 1 (left side)
		{
			if (bought_in_first & 1)
				p_num = 0;					// plyr 1 picks team (this makes player 1 the default for team 1)
			else
				p_num = 1;					// plyr 2 picks team
		}
		else
		{
			if (bought_in_first & 4)
				p_num = 2;					// plyr 3 picks team (this makes player 1 the default for team 1)
			else
				p_num = 3;					// plyr 4 picks team
		}
	}
	exit_status |= 1<<p_num;				// set TEAMS bit

	// create city names plaque
	tm_selct_img_ptrs[TM_CITY_PLAQ_PTR][tm_nbr] = beginobj(tm_city_plqs[tm_nbr],
		(tm_nbr ? 320.0f : 192.0f),
		(SPRITE_VRES/2.0f)+35.0f,
		(tm_nbr ? 48.0f : 50.0f),
		TM_SELECT_TID);
	tm_selct_img_ptrs[TM_CITY_PLAQ_PTR][tm_nbr]->id = tmsel_ids[tm_nbr];

	// create hi-lite city names bar
	tm_selct_img_ptrs[TM_NAME_HILITE_PTR][tm_nbr] = beginobj(&tsbar,
		(tm_nbr ? 320.0f-4.0f : 192.0f),
		(SPRITE_VRES/2.0f)+40.0f,
		(tm_nbr ? 39.0f : 40.0f),
		TM_SELECT_TID);
	tm_selct_img_ptrs[TM_NAME_HILITE_PTR][tm_nbr]->id = tmsel_ids[tm_nbr];

	// create roster disc
	tm_selct_img_ptrs[TM_ROSTER_BAR_PTR][tm_nbr] = beginobj(&ts_rdisc,
		(tm_nbr ? TM2_ROSTERX : TM1_ROSTERX),
		295.0f,
		40.0F,
		TM_SELECT_TID);
	tm_selct_img_ptrs[TM_ROSTER_BAR_PTR][tm_nbr]->id = tmsel_ids[tm_nbr];

	// create roster name image
	tm_selct_img_ptrs[TM_ROSTER_STR_PTR][tm_nbr] = beginobj(&rosterwrd, (tm_nbr ? TM2_ROSTERX : TM1_ROSTERX), 297.0f, 35.0F, TM_SELECT_TID);
	tm_selct_img_ptrs[TM_ROSTER_STR_PTR][tm_nbr]->id = tmsel_ids[tm_nbr];
	// create roster name backdrop
	tm_selct_img_ptrs[TM_ROSTER_BKDRP_PTR][tm_nbr] = beginobj(&rostrshdw, (tm_nbr ? TM2_ROSTERX : TM1_ROSTERX), 247.0f, 45.0F, TM_SELECT_TID);
	tm_selct_img_ptrs[TM_ROSTER_BKDRP_PTR][tm_nbr]->id = tmsel_ids[tm_nbr];

	// Make sure you put this back before sleeping!!!
	is_low_res = 0;

	// create city names (on plaque)
	// top 3
	tm_selct_img_ptrs[TM_CITY1_OBJ_PTR][tm_nbr] = beginobj(teaminfo[TEAM_UTA].pii_cn,
			city_names_xs[tm_nbr], city_names_ys[!!was_low_res][0], 25.7F, TM_SELECT_TID);
	tm_selct_img_ptrs[TM_CITY1_OBJ_PTR][tm_nbr]->id = tmsel_ids[tm_nbr];

	tm_selct_img_ptrs[TM_CITY2_OBJ_PTR][tm_nbr] = beginobj(teaminfo[TEAM_VAN].pii_cn,
			city_names_xs[tm_nbr], city_names_ys[!!was_low_res][1], 25.6F, TM_SELECT_TID);
	tm_selct_img_ptrs[TM_CITY2_OBJ_PTR][tm_nbr]->id = tmsel_ids[tm_nbr];

	tm_selct_img_ptrs[TM_CITY3_OBJ_PTR][tm_nbr] = beginobj(teaminfo[TEAM_WAS].pii_cn,
			city_names_xs[tm_nbr], city_names_ys[!!was_low_res][2], 25.5F, TM_SELECT_TID);
	tm_selct_img_ptrs[TM_CITY3_OBJ_PTR][tm_nbr]->id = tmsel_ids[tm_nbr];

	// middle city name (hilighted)
	tm_selct_img_ptrs[TM_CITY4_OBJ_PTR][tm_nbr] = beginobj(teaminfo[TEAM_ATL].pii_cn,
			city_names_xs[tm_nbr], city_names_ys[!!was_low_res][3], 25.4F, TM_SELECT_TID);
	tm_selct_img_ptrs[TM_CITY4_OBJ_PTR][tm_nbr]->id = tmsel_ids[tm_nbr];

	// bottom 3	
	tm_selct_img_ptrs[TM_CITY5_OBJ_PTR][tm_nbr] = beginobj(teaminfo[TEAM_BOS].pii_cn,
			city_names_xs[tm_nbr], city_names_ys[!!was_low_res][4], 25.3F, TM_SELECT_TID);
	tm_selct_img_ptrs[TM_CITY5_OBJ_PTR][tm_nbr]->id = tmsel_ids[tm_nbr];

	tm_selct_img_ptrs[TM_CITY6_OBJ_PTR][tm_nbr] = beginobj(teaminfo[TEAM_CHA].pii_cn,
			city_names_xs[tm_nbr], city_names_ys[!!was_low_res][5], 25.2F, TM_SELECT_TID);
	tm_selct_img_ptrs[TM_CITY6_OBJ_PTR][tm_nbr]->id = tmsel_ids[tm_nbr];

	tm_selct_img_ptrs[TM_CITY7_OBJ_PTR][tm_nbr] = beginobj(teaminfo[TEAM_CHI].pii_cn,
			city_names_xs[tm_nbr], city_names_ys[!!was_low_res][6], 25.1F, TM_SELECT_TID);
	tm_selct_img_ptrs[TM_CITY7_OBJ_PTR][tm_nbr]->id = tmsel_ids[tm_nbr];

	// Back it goes!!!
	is_low_res = was_low_res;

	glow_obj = beginobj(&teamwrd, SPRITE_HRES/2, SPRITE_VRES/2+41.0f-(is_low_res ? 0 : 0), 2.9F, TM_SELECT_TID);
	glow_obj->id = tmsel_ids[tm_nbr];
	glow_obj = beginobj(&ts_smdisc, SPRITE_HRES/2, SPRITE_VRES/2+40.0f-(is_low_res ? 0 : 0), 4.0F, TM_SELECT_TID);
	glow_obj->id = tmsel_ids[tm_nbr];												 
	glow_obj = beginobj(&ts_smdiscg, SPRITE_HRES/2, SPRITE_VRES/2+40.0f-(is_low_res ? 0 : 0), 3.0F, TM_SELECT_TID);
	glow_obj->id = 10+tm_nbr;

	// create team logo backdrop
	tm_selct_img_ptrs[TM_LOGO_PLATE_PTR][tm_nbr] = beginobj(&ts_disc,
			logo_xys[1][tm_nbr][is_low_res][X_VAL],
			logo_xys[1][tm_nbr][is_low_res][Y_VAL],
			40.0F,
			TM_SELECT_TID);
	tm_selct_img_ptrs[TM_LOGO_PLATE_PTR][tm_nbr]->id = tmsel_ids[tm_nbr];
	tm_selct_img_ptrs[TM_LOGO_PLATE_PTR][tm_nbr]->w_scale = 1.20f;
	tm_selct_img_ptrs[TM_LOGO_PLATE_PTR][tm_nbr]->h_scale = 1.20f;
	generate_sprite_verts(tm_selct_img_ptrs[TM_LOGO_PLATE_PTR][tm_nbr]);

	tm_selct_img_ptrs[TM_LOGO_PLATE2_PTR][tm_nbr] = beginobj(&ts_disc_c1,
			logo_xys[1][tm_nbr][is_low_res][X_VAL]-1.0f,
			logo_xys[1][tm_nbr][is_low_res][Y_VAL],
			40.0F,
			TM_SELECT_TID);
	tm_selct_img_ptrs[TM_LOGO_PLATE2_PTR][tm_nbr]->id = tmsel_ids[tm_nbr];
	tm_selct_img_ptrs[TM_LOGO_PLATE2_PTR][tm_nbr]->w_scale = 1.20f;
	tm_selct_img_ptrs[TM_LOGO_PLATE2_PTR][tm_nbr]->h_scale = 1.20f;
	generate_sprite_verts(tm_selct_img_ptrs[TM_LOGO_PLATE2_PTR][tm_nbr]);

	// create logo wing
//	tm_selct_img_ptrs[TM_PLQ_WING_PTR][tm_nbr] = beginobj(&dscwingl,
//			logo_xys[1][tm_nbr][is_low_res][X_VAL] + (tm_nbr ? 90.0f : -90.0f),
//			logo_xys[1][tm_nbr][is_low_res][Y_VAL],
//			45.0F,
//			TM_SEL_LOGOS_TID);
//	tm_selct_img_ptrs[TM_PLQ_WING_PTR][tm_nbr]->id = tmsel_ids[tm_nbr];
//	tm_selct_img_ptrs[TM_PLQ_WING_PTR][tm_nbr]->w_scale = 1.20f;
//	tm_selct_img_ptrs[TM_PLQ_WING_PTR][tm_nbr]->h_scale = 1.20f;
//	if (tm_nbr)
//		tm_selct_img_ptrs[TM_PLQ_WING_PTR][tm_nbr]->mode = FLIP_TEX_H;
//	generate_sprite_verts(tm_selct_img_ptrs[TM_PLQ_WING_PTR][tm_nbr]);

	// create team logo
	tm_selct_img_ptrs[TM_LOGO_OBJ_PTR][tm_nbr] = beginobj(teaminfo[cur_pos].pii_logo2,
			logo_xys[1][tm_nbr][is_low_res][X_VAL],
			logo_xys[1][tm_nbr][is_low_res][Y_VAL],
			20.0F,
			TM_SEL_LOGOS_TID);
	tm_selct_img_ptrs[TM_LOGO_OBJ_PTR][tm_nbr]->id = tmsel_ids[tm_nbr];
	tm_selct_img_ptrs[TM_LOGO_OBJ_PTR][tm_nbr]->w_scale = 1.20f;
	tm_selct_img_ptrs[TM_LOGO_OBJ_PTR][tm_nbr]->h_scale = 1.20f;
	generate_sprite_verts(tm_selct_img_ptrs[TM_LOGO_OBJ_PTR][tm_nbr]);

	// create roster names
	print_team_rosters(cur_pos, tm_nbr);

	tm_selected = NO;
	while ((force_selection != 1) && (tm_selected != YES))
	{
		if (!auto_select_tm)																	// basically... during random select just stick joy to UP
		{
			prev_joy_sws = joy_sws;
			joy_sws = get_joy_val_cur(p_num);
			butn_sws = get_but_val_down(p_num);
			butn_sws_cur = get_but_val_cur(p_num);
		}
		//
		// handle joystick
		//
		if (joy_sws)
		{
			if ((joy_sws != prev_joy_sws) || ((!sw_delay_cnt) && (!sw_repeat_cnt)))
			{
				if (!auto_select_tm)
				{
					if (joy_sws != prev_joy_sws)
						sw_delay_cnt = TICKS_BEFORE_START_REPEAT;
					sw_repeat_cnt = TICKS_BETWEEN_REPEATS;
				}
				switch(joy_sws)
				{
					case JOY_LEFT:
					case JOY_UP_LFT:
					case JOY_UP_RGT:
					case JOY_UP:																					// up
						if (!auto_select_tm)
							snd_scall_bank(plyr_bnk_str,TMSEL_CURSR_SND,VOLUME7,127,PRIORITY_LVL2);
						if (cur_pos > 0)
							cur_pos--;
						else
						 	cur_pos = NUM_TEAMS-1;
						update_tm_data(cur_pos,tm_nbr);
						break;
					case JOY_RIGHT:
					case JOY_DWN_LFT:
					case JOY_DWN_RGT:
					case JOY_DOWN:
						snd_scall_bank(plyr_bnk_str,TMSEL_CURSR_SND,VOLUME7,127,PRIORITY_LVL2);
						if (cur_pos < NUM_TEAMS-1)																			// on last row ?
							cur_pos++;																					// no... allow down
						else
						 	cur_pos = 0;
						update_tm_data(cur_pos,tm_nbr);
						break;
				}
			}
			else
			{
				if (sw_delay_cnt != 0)
					sw_delay_cnt--;
				 else if (sw_repeat_cnt != 0)
					sw_repeat_cnt--;													// timedown before allow auto-repeat
			}
		}
		//
		// handle button logic
		//
		if ((butn_sws) && (!auto_select_tm))
		{
			if ((butn_sws == TURBO_BUTTON) && (joy_sws == JOY_LEFT))
			{
				// start random team select; immediately available
				sw_delay_cnt = 0;
				sw_repeat_cnt = 0;
				prev_joy_sws = 0;				// in case player was already auto-repeating
				auto_select_tm = 1;
				rnd_team = randrng(NUM_TEAMS);
				mv_cnt = rnd_team + NUM_TEAMS*3;   								// loop at least twice
				snd_scall_bank(cursor_bnk_str,RND_SELCT_STRT_SND,VOLUME5,127,PRIORITY_LVL5);
			}
				// allow team selection after waiting <sw_select_delay_cnt> ticks
			else if (!sw_select_delay_cnt)
				tm_selected = YES;
		}

		// update random select stuff
		if (auto_select_tm)
		{
			if (mv_cnt == 0)
			{
				auto_select_tm = 0;
				tm_selected = YES;
			}
			else
				mv_cnt--;
		}

		glow_obj->state.constant_color &= 0x00FFFFFF;
		glow_obj->state.constant_color |= alpha_level << 24;

		sleep(1);

		if (sw_select_delay_cnt)
			sw_select_delay_cnt--;

		alpha_level += delta;
		if (alpha_level < 128)
		{
			alpha_level = 128;
			delta = +8;
		}
		else if (alpha_level > 255)
		{
			alpha_level = 255;
			delta = -8;
		}

	}

	// team picked or time ran out
	snd_scall_bank(cursor_bnk_str,TM_SELECTED_SND,VOLUME7,127,PRIORITY_LVL5);
//	snd_scall_bank(cursor_bnk_str,TM_SELECTED_SND,VOLUME4,127,PRIORITY_LVL4);

	game_info.team[tm_nbr] = cur_pos;


	// flash something
//	disc_flash(tm_selct_img_ptrs[TM_LOGO_PLATE_PTR][tm_nbr],SPFX_TID,20.0f,2.0f,1.7f,2);

//	flash_status |= (1<<p_num);														// set bit in variable
	disc_flash(tm_selct_img_ptrs[TM_LOGO_PLATE_PTR][tm_nbr],SPFX_TID,20.0f,2.0f,1.85f,p_num);

//	qcreate("cityflsh",FLASH_PID,stat_flash,city_names_xs[tm_nbr],city_names_ys[0][3]+1.0F,JAPPLE_BOX_TID,p_num);

	// wait for flash
//	while (flash_status & 1<<p_num)
//		sleep(2);

	sleep(60);		// let flash from team being selected... finish

	//
	//  Okay, now TEAM must select the teams roster...
	//

	// delete glow obj
	del1c(10+tm_nbr,-1);

	if (!force_selection)
	{
		const struct stat_plyr_data * p_spd = teaminfo[game_info.team[tm_nbr]].p_spd;
		i = teaminfo[game_info.team[tm_nbr]].num_player;

		// didnt time out...yet...
		// load mugshot textures
		while (loading_mug_textures)
			sleep(1);
		loading_mug_textures = 1;
//		dio_mode(DIO_MODE_NO_BLOCK);
		// Load all of the team mugshot textures
		while (i--)
		{
			load_mugshot_tpage(p_spd->pii_mug);
			p_spd++;
			sleep(1);
		}
		loading_mug_textures = 0;
//		dio_mode(DIO_MODE_BLOCK);

		// first trans all the teams stuff off...
//		for (i=0; i <= TM_NAME_HILITE2_PTR; i++)
//		{
//			tm_selct_img_ptrs[i][tm_nbr]->x_vel = (tm_nbr ? (SPRITE_HRES/WIPE_TICKS) : ((SPRITE_HRES/WIPE_TICKS) * -1));
//	//		tm_selct_img_ptrs[i][tm_nbr]->x_angle += (tm_nbr ? 4 : -4);
//			set_veladd_mode(tm_selct_img_ptrs[i][tm_nbr],1);
//		}
//
//		// let images slide off-screen
//		sleep(WIPE_TICKS+5);
//
//		// stop vel. adding
//		for (i=0; i <= TM_NAME_HILITE2_PTR; i++)
//			set_veladd_mode(tm_selct_img_ptrs[i][tm_nbr],0);
//
		// delete teams objects...so can show SELECT player stuff
		delete_multiple_strings(roster_name2_ids[tm_nbr], 0xffffffff);		// delete players name
		del1c(tmsel_ids[tm_nbr],-1);
		
		imgs_loaded = 0;
		if (four_plr_ver)
		{
			qcreate("selplr",plyr_sel_pids[(tm_nbr * 2) + 0],player_select_roster_cntrl,tm_nbr,((tm_nbr * 2) + 0),0,0);
			qcreate("selplr",plyr_sel_pids[(tm_nbr * 2) + 1],player_select_roster_cntrl,tm_nbr,((tm_nbr * 2) + 1),0,0);
		}
		else
			qcreate("selplr",plyr_sel_pids[p_num],player_select_roster_cntrl,tm_nbr,p_num,0,0);
			
		do
		{
			sleep(1);
		} while ( ((existp(plyr_sel_pids[tm_nbr], 0xFFFFFFFF) != NULL) && (!four_plr_ver)) ||
				  ((existp(plyr_sel_pids[(tm_nbr * 2)], 0xFFFFFFFF) != NULL) ||
				   (existp(plyr_sel_pids[(tm_nbr * 2) + 1], 0xFFFFFFFF) != NULL)) );

#if NUM_PERTEAM > 2
//		//
//		// Chose DRONE teammate
//		//
//		// update p1 & p2 mugshot/frame
//		// delete p1 and p2 statboxes and all text that goes with them
//		delobj(plr_sel_img_ptrs[STATBOX_OBJ_PTR][(tm_nbr*2)+0]);
//		delobj(plr_sel_img_ptrs[STATBOX_OBJ_PTR][(tm_nbr*2)+1]);
//		// delete roster stats
//		delete_multiple_strings(roster_stat_ids[tm_nbr*NUM_PERTEAM+0], 0xffffffff);
//		delete_multiple_strings(roster_stat_ids[tm_nbr*NUM_PERTEAM+1], 0xffffffff);
//
//		// create mugshot frame
//		beginobj((tm_nbr ? &plyrfrmr : &plyrfrmb), (tm_nbr ? 384.0f : 128.0f), FRAMEY, FRAME3Z, TM_SELECT_TID);
//		// create frame top
//		beginobj((tm_nbr ? &frmtopr : &frmtopb), (tm_nbr ? 384.0f : 128.0f), FRAMEY+80.0f, FRAME3Z-1.0f, TM_SELECT_TID);
//		// create frame name
//		beginobj(&dronewrd, (tm_nbr ? 384.0f : 128.0f), FRAMEY+84.0f, FRAME3Z-2.0f, TM_SELECT_TID);
//		// create stats box
//		beginobj(&statbx, (tm_nbr ? 384.0f : 128.0f), STATBOX_Y, FRAME3Z, TM_SELECT_TID);
//		// create mugshot
//		drone_mug_obj = beginobj(teaminfo[game_info.team[tm_nbr]].p_spd[0].pii_mug, (tm_nbr ? 384.0f : 128.0f)-58.0f, MUGY, FRAME3Z-2.0f, MUG_TID);
//
//		selected = 0;
//		move_stick = 1;
//		drone_index = (tm_nbr*NUM_PERTEAM)+NUM_PERTEAM-1;			// index in 'selected_players' array
//		cur_pos = 0;
////		nbr_plyrs = num_on_roster[game_info.team[tm_nbr]];
//		nbr_plyrs = teaminfo[game_info.team[tm_nbr]].num_player;
//
//		// select the drone teammate
//		while (!selected)
//		{
//			joy_sws = get_joy_val_down(p_num);
//			butn_sws = get_but_val_down(p_num);
//			if (move_stick)
//			{
//				joy_sws = JOY_LEFT;
//				move_stick = 0;
//			}
//			if (force_selection)
//				butn_sws = 1;			// force the selection of roster!!!
//
//			// handle joystick
//			if (joy_sws == JOY_LEFT)
//			{
//				// move cursor until valid selectable player
//				do
//				{
//					cur_pos--;
//					if (cur_pos < 0)
//						cur_pos = nbr_plyrs-1;
//					set_player_record(game_info.team[tm_nbr], cur_pos, FALSE, &selected_players[drone_index]);
//				} while ( (selected_players[drone_index].player_index == selected_players[drone_index-1].player_index) ||
//							(selected_players[drone_index].player_index == selected_players[drone_index-2].player_index) );
//
//				change_img_tmu(drone_mug_obj, teaminfo[game_info.team[tm_nbr]].p_spd[cur_pos].pii_mug, MUG_TID);
//				update_roster_stats((tm_nbr ? 384.0f : 128.0f), STATBOX_Y, tm_nbr, cur_pos, drone_index);
//				snd_scall_bank(cursor_bnk_str,MUGSHOT_MOVE_SND,VOLUME4,127,PRIORITY_LVL1);
//			}
//			else if (joy_sws == JOY_RIGHT)
//			{
//				do
//				{
//					cur_pos++;
//					if (cur_pos > nbr_plyrs-1)
//						cur_pos = 0;
//					set_player_record(game_info.team[tm_nbr], cur_pos, FALSE, &selected_players[drone_index]);
//
//				} while ( (selected_players[drone_index].player_index == selected_players[drone_index-1].player_index) ||
//							(selected_players[drone_index].player_index == selected_players[drone_index-2].player_index) );
//				change_img_tmu(drone_mug_obj, teaminfo[game_info.team[tm_nbr]].p_spd[cur_pos].pii_mug, MUG_TID);
//				update_roster_stats((tm_nbr ? 384.0f : 128.0f), STATBOX_Y, tm_nbr, cur_pos, drone_index);
//				snd_scall_bank(cursor_bnk_str,MUGSHOT_MOVE_SND,VOLUME4,127,PRIORITY_LVL1);
//			}
//
//			// handle buttons
//			if (butn_sws)
//			{
//				selected = 1;
//				set_player_record(game_info.team[tm_nbr], cur_pos, FALSE, &selected_players[drone_index]);
//				snd_scall_bank(cursor_bnk_str,MUGSHOT_SELECT_SND,VOLUME4,127,PRIORITY_LVL1);
//			}
//
//			sleep(1);
//		}
//
//		// let them see there roster selection...for a little bit.
//		sleep(60);
#endif
	} // if (!force_selection)
	else
	{	// timed out.. set default roster!!!
		for( i = 0; i < NUM_PERTEAM; i++)
			if (!is_guest[tm_nbr * NUM_PERTEAM + i])
				set_player_record(game_info.team[tm_nbr], i, FALSE, &selected_players[tm_nbr * NUM_PERTEAM + i]);
	}

	// this team is done with all selecting
	exit_status &= ~(1<<p_num);
}

//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
void player_select_roster_cntrl(int *args)
{
	int			butn_sws = 0;
	int			joy_sws = 0;
	int			cur_pos;						// index into mugshot table
	int			team;							// left or right
	int			selected;
	int			plyrs_in_roster;				// number of players on this teams roster
	int			move_stick;
	int			index1,index2;					// indexs into img arrays
	int			splyr1,splyr2;					// indexs into SELECTED_PLAYERS[] array
	char		rname1[40];
	char		rname2[40];
	long		alpha_level, delta;
	long		alpha_level2, delta2;
	int			was_low_res = is_low_res;
	

	team = args[0];

	splyr1 = index1 = args[1];
	splyr2 = index2 = args[1] ^ 1;

	plyrs_in_roster = teaminfo[game_info.team[team]].num_player;

	if (!four_plr_ver)
	{
		if (splyr1 == 0)
		{
			index1 = 1;
			index2 = 0;
		}
	}

	// wait for this player to join in.
	while (!(p_status & (1<<splyr1)))
		sleep(1);

	alpha_level = 255;
	delta = -8;
	alpha_level2 = 255;
	delta2 = -8;

	// create mugshot frame
	plr_sel_img_ptrs[BORDER_OBJ_PTR][index1] = beginobj((team ? &plyrfrmr : &plyrfrmb),
		station_xys[index1][1][X_VAL],    FRAMEY,       FRAMEZ,      TM_SELECT_TID);

	// create frame top
	plr_sel_img_ptrs[BORDER_HDR_OBJ_PTR][index1] = beginobj((team ? &frmtopr : &frmtopb),
		station_xys[index1][1][X_VAL],    FRAMEY+74,    FRAMEZ-1.0f, TM_SELECT_TID);

	// create frame top glow 
	plr_sel_img_ptrs[BORDER_HDR_GLOW_OBJ_PTR][index1] = beginobj((team ? &frmtoprg : &frmtopbg),
		station_xys[index1][1][X_VAL],    FRAMEY+74,    FRAMEZ-1.7f, TM_SELECT_TID);
	plr_sel_img_ptrs[BORDER_HDR_GLOW_OBJ_PTR][index1]->state.alpha_rgb_dst_function = GR_BLEND_ONE;	// set additive blending!!

	// create frame name
	plr_sel_img_ptrs[BORDER_HDR_NME_OBJ_PTR][index1] = beginobj(plr_wrd_imgs[four_plr_ver][index1],
		station_xys[index1][1][X_VAL],    FRAMEY+78,    FRAMEZ-1.8f, TM_SELECT_TID);

	// create stats box
	if (is_low_res)
		is_low_res = 3;
	plr_sel_img_ptrs[STATBOX_OBJ_PTR][index1] = beginobj((is_low_res ? &statbxl:&statbx),
		station_xys[index1][1][X_VAL],    STATBOX_Y,    FRAMEZ,      TM_SELECT_TID);
	is_low_res = was_low_res;

	// create mugshot
	plr_sel_img_ptrs[MUGSHOT_OBJ_PTR][index1] = beginobj(teaminfo[game_info.team[team]].p_spd[0].pii_mug,
		station_xys[index1][1][X_VAL]-58, MUGY,         FRAMEZ-1.6f, MUG_TID);

	// create swoosh img
	plr_sel_img_ptrs[SWOOSH_OBJ_PTR][index1] = beginobj(&frmcrest,
		station_xys[index1][1][X_VAL],    FRAMEY,       FRAMEZ-4.0f, TM_SELECT_TID);

	// create name dropshadow
	plr_sel_img_ptrs[NAME_BACKDROP_OBJ_PTR][index1] = beginobj(&plyrfrmshdw,
		station_xys[index1][1][X_VAL],    STATBOX_Y+45, FRAMEZ+1.0f, TM_SELECT_TID);

	if ((game_info.game_state == GS_EOQ) && !(is_guest[splyr1]))
		cur_pos = selected_players[splyr1].player_index - 1;			// set to chosen dude on roster
	else
		cur_pos = -1;													// 1st dude on roster

	move_stick = 1;														// dont have both players start on same roster number
	selected = 0;

	// select the player "I" want to "play as" during the game
	while (!selected)
	{
		joy_sws = get_joy_val_down(splyr1);
		butn_sws = get_but_val_down(splyr1);
		if (move_stick)
		{
			move_stick = 0;
			if (is_guest[splyr1])
				joy_sws = 1000;
			else
				joy_sws = JOY_RIGHT;
		}
		if (force_selection)
			butn_sws = 1;			// force the selection of roster!!!

		// handle joystick
		if (joy_sws)
		{
			if ((joy_sws == JOY_LEFT) || (joy_sws == JOY_UP))
			{
				// move cursor until valid selectable player
				do
				{
					if (--cur_pos < 0)
						cur_pos = plyrs_in_roster-1;
					set_player_record(game_info.team[team], cur_pos, FALSE, &selected_players[splyr1]);
				} while ((selected_players[splyr2].player_index == selected_players[splyr1].player_index) &&
						 ((four_plr_ver) && (p_status & (1<<splyr2))));
						
			}
			else if ((joy_sws == JOY_RIGHT) || (joy_sws == JOY_DOWN))
			{
				do
				{
					if (++cur_pos > plyrs_in_roster-1)
						cur_pos = 0;
					set_player_record(game_info.team[team], cur_pos, FALSE, &selected_players[splyr1]);
				} while ((selected_players[splyr2].player_index == selected_players[splyr1].player_index) &&
						 ((four_plr_ver) && (p_status & (1<<splyr2))));
			}

			// update mug and stats
			change_img_tmu(plr_sel_img_ptrs[MUGSHOT_OBJ_PTR][index1], selected_players[splyr1].player_data->pii_mug, MUG_TID);
			update_roster_stats(plr_sel_img_ptrs[STATBOX_OBJ_PTR][index1]->x, STATBOX_Y, selected_players[splyr1].player_data, splyr1);

			// delete old roster name
			delete_multiple_strings(roster_name_ids[index1], -1);		// delete players name
			sprintf(rname1, "%s %s", selected_players[splyr1].player_data->szfirst, selected_players[splyr1].player_data->szlast);
			set_text_font(FONTID_BAST8T);
			set_string_id(roster_name_ids[index1]);
			set_text_position(plr_sel_img_ptrs[STATBOX_OBJ_PTR][index1]->x-2.0f, MUGY-156.0f, FRAME2Z-20.0f);
			oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, rname1);

			snd_scall_bank(cursor_bnk_str,MUGSHOT_MOVE_SND,VOLUME6,127,PRIORITY_LVL2);
		}

		if (is_guest[splyr1])
		{
			butn_sws = 1;			// force the selection of roster!!!
			sleep(30);
		}
			
		// handle buttons
		if (butn_sws)
		{
			selected = 1;
			if (!is_guest[splyr1])
				set_player_record(game_info.team[team], cur_pos, FALSE, &selected_players[splyr1]);
			// flash mugshot!
			snd_scall_bank(cursor_bnk_str,MUGSHOT_SELECT_SND,VOLUME6,127,PRIORITY_LVL4);
			flash_obj_white(plr_sel_img_ptrs[MUGSHOT_OBJ_PTR][index1], 4, 2);
		}

		// update glow obj. aphla
		plr_sel_img_ptrs[BORDER_HDR_GLOW_OBJ_PTR][index1]->state.constant_color &= 0x00FFFFFF;
		plr_sel_img_ptrs[BORDER_HDR_GLOW_OBJ_PTR][index1]->state.constant_color |= alpha_level << 24;

		sleep(1);

		alpha_level += delta;
		if (alpha_level < 128)
		{
			alpha_level = 128;
			delta = +8;
		}
		else if (alpha_level > 255)
		{
			alpha_level = 255;
			delta = -8;
		}
	}
	
	hide_sprite(plr_sel_img_ptrs[BORDER_HDR_GLOW_OBJ_PTR][index1]);

	//
	//  OKay, now check to see if SAME player as above should choose his/her teammate... or if their teammate in game then teammate picks and this
	//			proc dies
	//

	if ( ((four_plr_ver) && (!(p_status & (1<<splyr2)))) || (!four_plr_ver) )
	{
		image_info_t * ii_wrd = (!(p_status & (1<<splyr2))) ? &dronewrd : plr_wrd_imgs[four_plr_ver][index2];

		// once the player gets to this point and his teammate isnt in game or he doesnt have one
		// he will pick the rest of the roster

		// kill his process.. and I'll select the rest of the players
		if (four_plr_ver)
			killall(plyr_sel_pids[splyr2],0xFFFFFFFF);

		// create mugshot frame
		plr_sel_img_ptrs[BORDER_OBJ_PTR][index2] = beginobj((team ? &plyrfrmr : &plyrfrmb),
			station_xys[index2][1][X_VAL],    FRAMEY,       FRAME2Z,      TM_SELECT_TID);

		// create frame top
		plr_sel_img_ptrs[BORDER_HDR_OBJ_PTR][index2] = beginobj((team ? &frmtopr : &frmtopb),
			station_xys[index2][1][X_VAL],    FRAMEY+74,    FRAME2Z-1.0f, TM_SELECT_TID);

		// create frame top glow 
		plr_sel_img_ptrs[BORDER_HDR_GLOW_OBJ_PTR][index2] = beginobj((team ? &frmtoprg : &frmtopbg),
			station_xys[index2][1][X_VAL],    FRAMEY+74,    FRAME2Z-1.7f, TM_SELECT_TID);
//		plr_sel_img_ptrs[BORDER_HDR_GLOW_OBJ_PTR][index2]->state.alpha_rgb_dst_function = GR_BLEND_ONE;	// set additive blending!!

		// create frame name
		plr_sel_img_ptrs[BORDER_HDR_NME_OBJ_PTR][index2] = beginobj(ii_wrd,
			station_xys[index2][1][X_VAL],    FRAMEY+78,    FRAME2Z-1.8f, TM_SELECT_TID);

		// create stats box
		if (is_low_res)
			is_low_res = 3;
		plr_sel_img_ptrs[STATBOX_OBJ_PTR][index2] = beginobj((is_low_res ? &statbxl:&statbx),
			station_xys[index2][1][X_VAL],    STATBOX_Y,    FRAME2Z,     TM_SELECT_TID);
		is_low_res = was_low_res;

		// create mugshot
		plr_sel_img_ptrs[MUGSHOT_OBJ_PTR][index2] = beginobj(teaminfo[game_info.team[team]].p_spd[0].pii_mug,
			station_xys[index2][1][X_VAL]-58, MUGY,         FRAME2Z-1.6f, MUG_TID);

		// create swoosh img
		plr_sel_img_ptrs[SWOOSH_OBJ_PTR][index2] = beginobj(&frmcrest,
			station_xys[index2][1][X_VAL],    FRAMEY,       FRAME2Z-4.0f, TM_SELECT_TID);

		// create name dropshadow
		plr_sel_img_ptrs[NAME_BACKDROP_OBJ_PTR][index2] = beginobj(&plyrfrmshdw,
			station_xys[index2][1][X_VAL],    STATBOX_Y+45, FRAME2Z+1.0f, TM_SELECT_TID);

		selected = 0;
		move_stick = 1;					// move to next available selectable player on roster

		if ((game_info.game_state == GS_EOQ) && !(is_guest[splyr2]))
			cur_pos = selected_players[splyr2].player_index - 1;					// set to chosen dude on roster
		else
			cur_pos = -1;					// try to show 1st dude on roster

		// select the player that my teammate will be if he joins in during the game
		while (!selected)
		{
			joy_sws = get_joy_val_down(splyr1);
			butn_sws = get_but_val_down(splyr1);
			if (move_stick)
			{
				joy_sws = JOY_RIGHT;
				move_stick = 0;
			}
			if (force_selection)
				butn_sws = 1;			// force the selection of roster!!!

			// handle joystick
			if (joy_sws)
			{
				if ((joy_sws == JOY_LEFT) || (joy_sws == JOY_UP))
				{
					// move cursor until valid selectable player
					do
					{
						if (--cur_pos < 0)
							cur_pos = plyrs_in_roster-1;
						set_player_record(game_info.team[team], cur_pos, FALSE, &selected_players[splyr2]);
					} while (selected_players[splyr2].player_index == selected_players[splyr1].player_index);
				}
				else if ((joy_sws == JOY_RIGHT) || (joy_sws == JOY_DOWN))
				{
					do
					{
						if (++cur_pos > plyrs_in_roster-1)
							cur_pos = 0;
						set_player_record(game_info.team[team], cur_pos, FALSE, &selected_players[splyr2]);
					} while (selected_players[splyr2].player_index == selected_players[splyr1].player_index);
				}

				// update mugshot and stats
				change_img_tmu(plr_sel_img_ptrs[MUGSHOT_OBJ_PTR][index2], selected_players[splyr2].player_data->pii_mug, MUG_TID);
				update_roster_stats(plr_sel_img_ptrs[STATBOX_OBJ_PTR][index2]->x, STATBOX_Y, selected_players[splyr2].player_data, splyr2);

				// delete old roster name
				delete_multiple_strings(roster_name_ids[index2], 0xffffffff);		// delete players name
				sprintf(rname2, "%s %s", selected_players[splyr2].player_data->szfirst, selected_players[splyr2].player_data->szlast);
				set_text_font(FONTID_BAST8T);
				set_string_id(roster_name_ids[index2]);
				set_text_position(plr_sel_img_ptrs[STATBOX_OBJ_PTR][index2]->x-2.0f, MUGY-156.0f, FRAME2Z-20.0f);
				oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, rname2);

				snd_scall_bank(cursor_bnk_str,MUGSHOT_MOVE_SND,VOLUME6,127,PRIORITY_LVL2);
			}

//			if (is_guest[splyr2])
//			{
//				butn_sws = 1;			// force the selection of roster!!!
//				update_roster_stats(plr_sel_img_ptrs[STATBOX_OBJ_PTR][index2]->x, STATBOX_Y, selected_players[splyr2].player_data, splyr2);
//				sprintf(rname2, "%s %s", selected_players[splyr2].player_data->szfirst, selected_players[splyr2].player_data->szlast);
//				sleep(30);
//			}
			
			// handle buttons
			if (butn_sws)
			{
				selected = 1;
				set_player_record(game_info.team[team], cur_pos, FALSE, &selected_players[splyr2]);
				// flash mugshot!
				snd_scall_bank(cursor_bnk_str,MUGSHOT_SELECT_SND,VOLUME6,127,PRIORITY_LVL4);
				flash_obj_white(plr_sel_img_ptrs[MUGSHOT_OBJ_PTR][index2], 4, 2);

				hide_sprite(plr_sel_img_ptrs[BORDER_HDR_GLOW_OBJ_PTR][index2]);
			}

			// update glow obj. aphla
			plr_sel_img_ptrs[BORDER_HDR_GLOW_OBJ_PTR][index2]->state.constant_color &= 0x00FFFFFF;
			plr_sel_img_ptrs[BORDER_HDR_GLOW_OBJ_PTR][index2]->state.constant_color |= alpha_level2 << 24;

			sleep(1);

			alpha_level2 += delta2;
			if (alpha_level2 < 128)
			{
				alpha_level2 = 128;
				delta2 = +8;
			}
			else if (alpha_level2 > 255)
			{
				alpha_level2 = 255;
				delta2 = -8;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------

void update_roster_stats(float statbx_x, float statbx_y, struct stat_plyr_data *stat_ptr, int selected_player_array_index)
{
	int i,color;
	char val;
	int	roster_stat_yoff[] = {23,11,-1,-13, 23,11,-1,-13};				// from center of stat box

	delete_multiple_strings(roster_stat_ids[selected_player_array_index], 0xffffffff);		// delete players name

	if (((selected_players[selected_player_array_index].record_type == CREATP_RECORD) &&
			(selected_players[selected_player_array_index].cmos_data.privileges & PRIV_HIDEATTR))
		|| (selected_players[selected_player_array_index].record_type == SECRET_PLAYER_RECORD))
	{
		hidden_attrib |= 1 << selected_player_array_index;
		return;
	} else
		hidden_attrib &= ~(1 << selected_player_array_index);

	if (is_low_res)
		set_text_font(FONTID_BAST7T);
	else
		set_text_font(FONTID_BAST8T);
	set_string_id(roster_stat_ids[selected_player_array_index]);

	for (i=0; i < 8; i++)	
	{
		/* the stats are stored in the 0-19 range, display them in the 1-20 range */
		switch(i)
		{
			case 0:
		    	val = stat_ptr->power + 1;
				break;
			case 1:
				val = stat_ptr->speed + 1;
				break;
			case 2:
				val = stat_ptr->shot + 1;
				break;
			case 3:
				val = stat_ptr->rebound + 1;
				break;
			case 4:
				val = stat_ptr->steal + 1;
				break;
			case 5:
				val = stat_ptr->block + 1;
				break;
			case 6:
				val = stat_ptr->dunk + 1;
				break;
			case 7:
				val = stat_ptr->dribble + 1;
				break;
		}

		//val = stat_ptr[stat_index[i]];

		if (val >= 13)
			color = LT_GREEN;
		else if (val >= 4)
			color = LT_YELLOW;
		else
			color = LT_RED;

		set_text_position( ((i < 4) ? statbx_x-10.0f : statbx_x+37.0f), statbx_y + roster_stat_yoff[i], FRAME3Z-.05f);
		oprintf("%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), color, val);
	}
}

//------------------------------------------------------------------------------------------------------------------------------

static unsigned long xlate_color(unsigned char color)
{
	unsigned long r, g, b, a;
	
	/* table format is :2 bit blue 3 bit green 3 bit red */
	a = 0xFF000000;
	r = color & 0x07;
	g = (color >> 3) & 0x07;
	b = (color >> 6) & 0x03;
	r = ((r << 5) | (r << 2) | (r >> 1)) << 16;
	g = ((g << 5) | (g << 2) | (g >> 1)) << 8;
	b = (b << 6) | (b << 4) | (b << 2) | b;
	return a | r | g | b;
}	/* xlate_color */


static unsigned long cycle_color(unsigned char *color_table, int table_size, int *table_index)
{
	(*table_index)++;
	if (*table_index == table_size)
		*table_index = 0;
	return xlate_color(color_table[*table_index]);
}	/* cycle_color */

unsigned char rgold[] = {
	0x07, 0x0F, 0x17, 0x1F, 0x27, 0x2F,
	0x27, 0x1F, 0x17, 0x0F
};
unsigned char rgb[] = {
	0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00,
	0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 
	0x30, 0x28, 0x20, 0x18, 0x10, 0x08, 0x00,
	0x40, 0x80, 0xC0, 0x80, 0x40, 0x00,
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06
};
unsigned char bpr[] = {
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0x87, 0x87, 0x47, 0x47,
	0x07, 0x07, 0x47, 0x47, 0x87, 0x87, 0xC7, 0xC7, 0xC6, 0xC5, 0xC4, 0xC3,
	0xC2, 0xC1
};
unsigned char decay[] = {
	0xC0, 0xC0, 0xD0, 0xE0, 0xF0, 0xF8, 0xFA, 0xBA, 0x7A, 0x3A,
	0x34, 0x2D, 0x1F, 0x17, 0x0F, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01
};
unsigned char laser[] = {
	0x38, 0x39, 0x3A, 0x3B, 0x3C,
	0x3D, 0x3E, 0x3F, 0x37, 0x2F, 0x27, 0x1F, 0x17,
	0x47, 0x47, 0x87, 0x87, 0xC7, 0xC7,
	0xC6, 0xC5, 0xCC, 0xCB, 0xCA, 0xDA, 0xE8, 0xF8,
	0xF9, 0xFA, 0xFB, 0xFD, 0xFF, 0xBF, 0x3F, 0x3E,
	0x3C
};

#define CNT_RGOLD	4
#define CNT_RGB		2
#define CNT_BPR		2
#define CNT_DECAY	2
#define CNT_LASER	2

static void hidden_attrib_flasher(int *args)
{
	image_info_t *quest_image[8] = {&quest1, &quest2, &quest3, &quest4,
									&quest5, &quest6, &quest7, &quest8
	};
	sprite_info_t *plate[NUM_PLAYERS], *text[NUM_PLAYERS], *quest[NUM_PLAYERS][8];
	long alpha_level, delta;
	int rgold_counter, rgb_counter, bpr_counter, decay_counter, laser_counter, lf_counter;
	unsigned long rgold_color, rgb_color, bpr_color, decay_color, laser_color, lf_color;
	int rgold_index, rgb_index, bpr_index, decay_index, laser_index, lf_mode, counter;
	int i, j;
	
	for (i = 0; i < NUM_PLAYERS; i++) {

		float x = station_xys[ (!four_plr_ver && i < NUM_PERTEAM) ? i^1 : i ][1][X_VAL];

		plate[i] = beginobj(&statbxhid, x, STATBOX_Y, 50.0F, 2084);
		plate[i]->id = 2084;
		hide_sprite(plate[i]);
		
		text[i] = beginobj(&hiddentxt, x, STATBOX_Y, 40.0F, 2084);
		text[i]->id = 2084;
		hide_sprite(text[i]);
		
		for (j = 0; j < 8; j++) {
			quest[i][j] = beginobj(quest_image[j], x, STATBOX_Y, 45.0F, 2084);
			quest[i][j]->id = 2084;
			quest[i][j]->state.color_combiner_function = GR_COLORCOMBINE_CCRGB;
			hide_sprite(quest[i][j]);
		}
	}
	
	alpha_level = 255;
	delta = -8;
	rgold_counter = rgb_counter = bpr_counter = decay_counter = laser_counter = lf_counter = counter = 0;
	rgold_index = rgb_index = bpr_index = decay_index = laser_index = 0;
	lf_mode = 0;
	lf_color = WHITE;
	for (;;) {
		if (rgold_counter-- == 0) {
			rgold_color = cycle_color(rgold, sizeof(rgold), &rgold_index);
			rgold_counter = CNT_RGOLD;
		}
		if (rgb_counter-- == 0) {
			rgb_color = cycle_color(rgb, sizeof(rgb), &rgb_index);
			rgb_counter = CNT_RGB;
		}
		if (bpr_counter-- == 0) {
			bpr_color = cycle_color(bpr, sizeof(bpr), &bpr_index);
			bpr_counter = CNT_BPR;
		}
		if (decay_counter-- == 0) {
			decay_color = cycle_color(decay, sizeof(decay), &decay_index);
			decay_counter = CNT_DECAY;
		}
		if (laser_counter-- == 0) {
			laser_color = cycle_color(laser, sizeof(laser), &laser_index);
			laser_counter = CNT_LASER;
		}
		if (lf_counter-- == 0) {
			if (lf_mode == 0) {
				lf_mode = 1;
				lf_color = xlate_color(laser[randrng(0x20)]);
				lf_counter = 2;
			} else {
				lf_mode = 0;
				lf_color = WHITE;
				lf_counter = 6;
			}
		}
		
		for (i = 0; i < NUM_PLAYERS; i++) {
			if (hidden_attrib & (1 << i)) {
				unhide_sprite(plate[i]);
				unhide_sprite(text[i]);
				for (j = 0; j < 8; j++)
					unhide_sprite(quest[i][j]);
			} else {
				hide_sprite(plate[i]);
				hide_sprite(text[i]);
				for (j = 0; j < 8; j++)
					hide_sprite(quest[i][j]);
			}
			text[i]->state.constant_color &= 0x00FFFFFF;
			text[i]->state.constant_color |= alpha_level << 24;
			
			quest[i][0]->state.constant_color = rgold_color;
			quest[i][1]->state.constant_color = rgb_color;
			quest[i][2]->state.constant_color = bpr_color;
			quest[i][3]->state.constant_color = decay_color;
			quest[i][4]->state.constant_color = laser_color;
			quest[i][5]->state.constant_color = rgold_color;
			quest[i][6]->state.constant_color = bpr_color;
			quest[i][7]->state.constant_color = laser_color;
		}
		sleep(1);
		if (counter == 0)
			alpha_level += delta;
		else {
			alpha_level = 255;
			counter--;
		}
		if (alpha_level < 128) {
			alpha_level = 128;
			delta = +8;
		} else if (alpha_level > 255) {
			alpha_level = 255;
			delta = -8;
			counter = 12;
		}
	}
}	/* hidden_attrib_flasher */

//
// PARMS: 0 = string id
//        1 = ptr to color table
//        2 = size of color table if not a default table
//        3 = tick delay count if not a default table
//
void cycle_ostring_color_proc(int * args)
{
	extern ostring_t * ostring_list;
	ostring_t * oslist;
	int color = 0;
	int index = 0;

	// Chk & set if default table
	if (args[1] == (int)(rgold))
	{
		args[2] = sizeof(rgold);
		args[3] = CNT_RGOLD;
	}
	else
	if (args[1] == (int)(rgb))
	{
		args[2] = sizeof(rgb);
		args[3] = CNT_RGB;
	}
	else
	if (args[1] == (int)(bpr))
	{
		args[2] = sizeof(bpr);
		args[3] = CNT_BPR;
	}
	else
	if (args[1] == (int)(decay))
	{
		args[2] = sizeof(decay);
		args[3] = CNT_DECAY;
	}
	else
	if (args[1] == (int)(laser))
	{
		args[2] = sizeof(laser);
		args[3] = CNT_LASER;
	}
	else
	// Not default; abort if any value is bogus
	if (args[1] == 0 || args[2] <= 0 || args[3] < 0)
		return;

	args[3]++;

	while (1)
	{
		int flag = FALSE;

		color  = cycle_color((char *)(args[1]), args[2], &index);
		oslist = ostring_list;

		while (oslist)
		{
			if (oslist->id == args[0])
			{
				change_string_cc(oslist, color);
				change_alpha_cc(oslist, color);

				flag = TRUE;
			}
			oslist = oslist->next;
		}

		// Exit if no ostrings were found
		if (flag == FALSE)
			return;

		sleep(args[3]);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
//int	whois_teammate(int pnum)
//{
//	return pnum^1;
//}

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
void update_tm_data(short c_pos, short tm_nbr)
{
	signed short	i,j;
	int				was_low_res = is_low_res;
	//int				num_plyrs;
	//char			rname[40];
	//float			y;


	// update team logo
	change_img_tmu(tm_selct_img_ptrs[TM_LOGO_OBJ_PTR][tm_nbr], teaminfo[c_pos].pii_logo2, TM_SEL_LOGOS_TID);

	// Make sure you put this back before sleeping!!!
	is_low_res = 0;

	// update top three city names
	for (i=TM_CITY3_OBJ_PTR,j=c_pos-1; i >= TM_CITY1_OBJ_PTR; i--,j--)
	{
		if (j < 0)
			j = NUM_TEAMS-1;
		change_img_tmu(tm_selct_img_ptrs[i][tm_nbr], teaminfo[j].pii_cn, TM_SELECT_TID);
	}

	// update hi-lited (middle) city name
	change_img_tmu(tm_selct_img_ptrs[TM_CITY4_OBJ_PTR][tm_nbr], teaminfo[c_pos].pii_cn, TM_SELECT_TID);

	// update bottom three city names
	for (i=TM_CITY5_OBJ_PTR,j=c_pos+1; i <= TM_CITY7_OBJ_PTR; i++,j++)
	{
		if (j > NUM_TEAMS-1)
			j = 0;
		change_img_tmu(tm_selct_img_ptrs[i][tm_nbr], teaminfo[j].pii_cn, TM_SELECT_TID);
	}

	// Back it goes!!!
	is_low_res = was_low_res;

	print_team_rosters(c_pos, tm_nbr);

}


//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
void print_team_rosters(int tm, int side)
{
	int		i,num_plyrs;
	float	y;
	int		was_low_res = is_low_res;

	is_low_res = 0;
	// update team roster strings
	delete_multiple_strings(roster_name2_ids[side], 0xffffffff);		// delete players name
	set_text_font(FONTID_BAST8T);
	set_string_id(roster_name2_ids[side]);
	y = 270.0f - (was_low_res ? 90.0f : 0.0f);
	// print new roster names (first and last name)
	num_plyrs = teaminfo[tm].num_player;
	if (num_plyrs == 6)
		y+=3.0f + (was_low_res ? 0.0f : 1.0f);
	if (num_plyrs > 6)
		y+=5.0f + (was_low_res ? 2.0f : 4.0f);
	for (i=0; i < num_plyrs; i++)
	{
		set_text_position((side ? TM2_ROSTERX : TM1_ROSTERX), y, 30.0f);
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, teaminfo[tm].p_spd[i].szlast);
		y-=(14.0f - (was_low_res ? 5.0f : 0.0f));
	}
	is_low_res = was_low_res;
}

//------------------------------------------------------------------------------------------------------------------------------
//	This process welcomes the player with some speech
//
// 	INPUT:	Nothing
//	OUTPUT:	Nothing
//------------------------------------------------------------------------------------------------------------------------------
//static void welcome_proc(int *args)
//{
////	sleep(90);
////	snd_scall_bank(intro_bnk_str,6,VOLUME3,127,SND_PRI_SET|SND_PRI_TRACK5);
//}

//------------------------------------------------------------------------------------------------------------------------------
//	This process welcomes the player with some speech
//
// 	INPUT:	Nothing
//	OUTPUT:	Nothing
//------------------------------------------------------------------------------------------------------------------------------
static void whoosh_proc(int *args)
{
	snd_scall_bank(plyr_bnk_str, WHOOSH_SND, VOLUME5, 127, PRIORITY_LVL3);
	sleep(90);
//	snd_scall_bank(plyr_bnk_str, 22, VOLUME4, 127, PRIORITY_LVL4);
//	sleep(2);
	snd_scall_bank(plyr_bnk_str,WHOOSH2_SND,VOLUME5,127, PRIORITY_LVL3);
}

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
void lighting_proc(int *args)
{
//	sprite_info_t 	*plighting;
//	float			x,y;
//	int				i;
//
//
//	x = args[0];
//	y = args[1];
//
//	snd_scall_bank(cursor_bnk_str,SEL_YESNO_SND,VOLUME2,127,PRIORITY_LVL3);
//
//	plighting = beginobj(lighting_imgs[0], x, y, 5.0F, JAPPLE_BOX_TID);
//
//	plighting->w_scale = 1.3f;
//	plighting->h_scale = 2.0f;
//	generate_sprite_verts(plighting);
//
//	for (i=0; i < (sizeof(lighting_imgs)/sizeof(image_info_t *))-1; i++)
////	for (i=0; i < 11; i++)
//	{
//		change_img_tmu(plighting,lighting_imgs[i],JAPPLE_BOX_TID);
//		sleep(2);
//	}
//	delobj(plighting);
//
}


//-------------------------------------------------------------------------------------------------
//  		This process gets the players NAME and PIN NUMBER.			
//-------------------------------------------------------------------------------------------------
void name_entry(int none)
{
	sprite_info_t		*psprite;

	// Disable the disk interrupt callback (IN CASE...started on some attract screens..ie team info)
//	dio_mode(DIO_MODE_BLOCK);

	// load the textures
	if (load_textures_into_tmu(name_entry_textures_tbl))
	{
#ifdef DEBUG
		fprintf(stderr, "Couldn't load all name-entry textures into tmu\r\n");
		lockup();
#else
		return;
#endif
	}

	// must LOCK these textures so that one player doesnt delete them for the other player
	lock_multiple_textures(YES_NO_PLAQ_TID,0xFFFFFFFF);
//	lock_multiple_textures(JAPPLE_BOX_TID,0xFFFFFFFF);
//	lock_multiple_textures(SPFX_TID,0xFFFFFFFF);

	get_all_records();


	// Re-create the wipeout'd plate objects
//	create_plates();

	// this will load a texture then sleep using the dma disk code
//	qcreate("ldtxturs",0,load_name_entry_textures,0,0,0,0);

	psprite = beginobj(&backgrnd,  		  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	psprite->id = 1;
	psprite = beginobj(&backgrnd_c1,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	psprite->id = 1;
	psprite = beginobj(&backgrnd_c2,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	psprite->id = 1;
	psprite = beginobj(&backgrnd_c3,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	psprite->id = 1;

	set_text_z_inc(0.0f);

  	qcreate("timer",TIMER_PID,timedown,40,9,SPRITE_HRES/2.0f,70.0F);							//wait 1 second then cnt from 9
//  	qcreate("timer",TIMER_PID,timedown,5,9,SPRITE_HRES/2.0f,50.0F);							//wait 1 second then cnt from 9
	qcreate("cred_msg",CREDIT_PID,credit_msg,0,JAPPLE_BOX_TID,0,0);

	qcreate("p1_cur",P1_CURSOR_PID,player_cursor,PLYR_1,0,0,0);
	qcreate("p1chlngr",P1_CHALNGR_PID,challenger_needed_msg,PLYR_1,JAPPLE_BOX_TID,0,0);
	qcreate("p3_cur",P3_CURSOR_PID,player_cursor,PLYR_3,0,0,0);
	qcreate("p3chlngr",P3_CHALNGR_PID,challenger_needed_msg,PLYR_3,JAPPLE_BOX_TID,0,0);

	if (four_plr_ver)
	{
		qcreate("p2_cur",P2_CURSOR_PID,player_cursor,PLYR_2,0,0,0);
		qcreate("p2chlngr",P2_CHALNGR_PID,challenger_needed_msg,PLYR_2,JAPPLE_BOX_TID,0,0);
		qcreate("p4_cur",P4_CURSOR_PID,player_cursor,PLYR_4,0,0,0);
		qcreate("p4chlngr",P4_CHALNGR_PID,challenger_needed_msg,PLYR_4,JAPPLE_BOX_TID,0,0);
	}

	force_selection = 0;
	exit_status = 0;
	showing_awards = 0;
	players = 0;												// min value

	// Take away the plates
	turn_transit_off(1);

	do
	{
		sleep(1);
		if ((exit_status == 0) && (players == 1))
		{
			// player selected NO and hes the only player (should we give extra time for other to buy-in or FUCK em ?)

		}
	} while (exit_status);

	// kill processes (if not dead already)
	killall(P1_CURSOR_PID,0xFFFFFFFF);
	killall(P2_CURSOR_PID,0xFFFFFFFF);
	killall(P3_CURSOR_PID,0xFFFFFFFF);
	killall(P4_CURSOR_PID,0xFFFFFFFF);

	// first wait for any FULLGAME BOUGHT msg...
	while (existp(FULLGAME_BOUGHT_PID, 0xFFFFFFFF))
		sleep(1);

	// Turn on the plates
	turn_transit_on();
	// freeze the screen

	// kill processes (if not dead already)
	killall(P1_CHALNGR_PID,0xFFFFFFFF);
	killall(P2_CHALNGR_PID,0xFFFFFFFF);
	killall(P3_CHALNGR_PID,0xFFFFFFFF);
	killall(P4_CHALNGR_PID,0xFFFFFFFF);

	// Do some clean-up
	// Clean up bonus flashing message
	killall(CREDIT_PID,0xFFFFFFFF);
	killall(TIMER_PID,0xFFFFFFFF);
	killall(BONUS_PID,0xFFFFFFFF);


	// delete all the objects with ID of 0 and 1
	del1c(0,0xFFFFFFFF);
	del1c(1,0xFFFFFFFF);
	del1c(OID_JAPPLE_BOX, 0xFFFFFFFF);

	delete_multiple_strings(1, 0xffffffff);

	unlock_multiple_textures(YES_NO_PLAQ_TID,0xFFFFFFFF);
	delete_multiple_textures(YES_NO_PLAQ_TID,0xFFFFFFFF);

//	unlock_multiple_textures(JAPPLE_BOX_TID,0xFFFFFFFF);
//	delete_multiple_textures(JAPPLE_BOX_TID,0xFFFFFFFF);
//	unlock_multiple_textures(SPFX_TID,0xFFFFFFFF);
//	delete_multiple_textures(SPFX_TID,0xFFFFFFFF);
//	unlock_multiple_textures(BKGRND_TID,0xFFFFFFFF);
//	delete_multiple_textures(BKGRND_TID,0xFFFFFFFF);

	if(rec_sort_ram)
	{
		JFREE(rec_sort_ram);
		rec_sort_ram = NULL;
	}
}

//-------------------------------------------------------------------------------------------------
// 				   This process controls the players cursor and data entry
//-------------------------------------------------------------------------------------------------

#define XP_SUB	26.0f
#define XI_SUB	41.0f

void player_cursor(int *args)
{
	float		japple_box_x;				// center X pos. for players tv box
	float		japple_box_y;				// center Y pos. for players tv box
	float		sectn_cntr_x;			// center X pos. for players section
	float		sectn_cntr_y;				// center Y pos. for players section
	float		yesno_x;
	int 		i;
	int			pnum;					// player number
	int			cur_pos;			// index into alpha table and pin table
	char		*cur_tbl;					// pointer to current alpha table
	int			entered_cnt;				// entered characters. (name and pin)
	int			menu_level;
	int			switches,buttons;
	int			entered_char;
	int			tbl_h;					// height of cursor movement
	int			tbl_w;					// width of cursor movement
	int			rnum,ver;						// cmos record number
	int			wait_time,answer;
	plyr_record_t	*prec;
	sprite_info_t	*cursor_obj;
	sprite_info_t 	*plaq_obj;
	sprite_info_t 	*plaq_obj2;
	sprite_info_t 	*glow_obj = NULL;
	sprite_info_t 	*yes_plq_obj = NULL;
	sprite_info_t 	*no_plq_obj = NULL;
	sprite_info_t 	*yes_obj = NULL;
	sprite_info_t 	*no_obj = NULL;
	long			alpha_level, delta;


	pnum = args[0];
	exit_status &= ~(1<<pnum);											// CLEAR players bit in variable (andn)
	
	// clear out stat keeping struct.
	prec = plyr_stat_ptrs[pnum];
	clear_cmos_record((char *)prec,sizeof(plyr_record_t));
	// clear out record keeping struct.
	prec = plyr_data_ptrs[pnum];
	clear_cmos_record((char *)prec,sizeof(plyr_record_t));

	// get player's section coordinates
	sectn_cntr_x = station_xys[pnum][four_plr_ver][X_VAL];
	sectn_cntr_y = station_xys[pnum][four_plr_ver][Y_VAL];
	japple_box_x = japbx_cntr_xys[pnum][four_plr_ver][X_VAL];
	japple_box_y = japbx_cntr_xys[pnum][four_plr_ver][Y_VAL];
	ver = four_plr_ver;
	
	// create the players credits box
	qcreate("credbox",CREDIT_PID,plyr_credit_box,pnum,sectn_cntr_x,sectn_cntr_y,JAPPLE_BOX_TID);
		
	// Wait for player to join
	while (!(p_status & (1<<pnum)))
		sleep(1);

	// Player is active
	if (force_selection == 1)
	{
		exit_status &= ~(1<<pnum);														// CLEAR players bit in variable (andn)
		die(0);
	}

	// keep count of active players
	players++;

	// SET players bit in variable
	exit_status |= 1<<pnum;

	alpha_level = 255;
	delta = -8;


	// create players entry plaque (top half)
	plaq_obj  = beginobj(init_plaq_imgs[four_plr_ver][pnum],
						 plaq_xs[pnum][four_plr_ver],
						 SPRITE_VRES-163.0f,
						 60.0F,
						 YES_NO_PLAQ_TID);
	plaq_obj->id = plyr_plaq_ids[pnum];


	yesno_x = sectn_cntr_x;
	if (pnum == 0)
		yesno_x -= 3;
	else if (pnum == 1)
		yesno_x += 1;
	else if (pnum == 2)
		yesno_x += 1;
	else
		yesno_x += 5;

	// Ask player (YES/NO) if he wants to enter name and pin for record keeping / retrival ?
	set_text_z_inc(-0.01f);
	set_string_id(0x344+pnum);
	set_text_font(is_low_res ? FONTID_BAST8TA : FONTID_NBA10);
	set_text_position(yesno_x-3.0f, 300.0f, 1.2F);
	oprintf("%dj%dcENTER NAME", (HJUST_CENTER|VJUST_CENTER), WHITE);
	set_text_position(yesno_x-4.0f, 285.0f, 1.2F);
	oprintf("%dj%dcFOR", (HJUST_CENTER|VJUST_CENTER), WHITE);
	set_text_position(yesno_x-3.0f, 270.0f, 1.2F);
	oprintf("%dj%dcRECORD", (HJUST_CENTER|VJUST_CENTER), WHITE);
	set_text_position(yesno_x-3.0f, 255.0f, 1.2F);
	oprintf("%dj%dcKEEPING ?", (HJUST_CENTER|VJUST_CENTER), WHITE);
	set_text_z_inc(0.0f);

	yes_plq_obj = beginobj(&yn_blk, yesno_x, option_xys[ver][0][Y_VAL]-20 + 30.0f, 45.0F, OPT_SCR_TID);
	yes_plq_obj->id = OID_MISC+pnum;
	no_plq_obj = beginobj(&yn_blu,  yesno_x, option_xys[ver][0][Y_VAL]-20 - 30.0f, 45.0F, OPT_SCR_TID);
	no_plq_obj->id = OID_MISC+pnum;
	no_obj = beginobj(&nowrd,   yesno_x, option_xys[ver][0][Y_VAL]-20 - 26.0f, 15.0F, OPT_SCR_TID);
	no_obj->id = OID_MISC+pnum;
	yes_obj = beginobj(&yeswrd, yesno_x, option_xys[ver][0][Y_VAL]-20 + 34.0f, 15.0F, OPT_SCR_TID);
	yes_obj->id = OID_MISC+pnum;
	glow_obj = beginobj(&yn_blu2, yesno_x, option_xys[ver][0][Y_VAL]-20 -30.0f, 58.0F, OPT_SCR_TID);
	glow_obj->id = OID_MISC+pnum;

	wait_time = tsec * 8;
	answer = NO;
	while (--wait_time)
	{
		switches = get_joy_val_down(pnum);
		if ((switches == JOY_UP) && (answer != YES))
		{
			answer = YES;
			change_img_tmu(yes_plq_obj,&yn_blu,OPT_SCR_TID);
			change_img_tmu(no_plq_obj,&yn_blk,OPT_SCR_TID);
			glow_obj->y = option_xys[ver][0][Y_VAL]-20 + 30;
			snd_scall_bank(plyr_bnk_str,TMSEL_CURSR_SND,VOLUME6,127,PRIORITY_LVL3);
		}
		else if ((switches == JOY_DOWN) && (answer != NO))
	 	{
			answer = NO;
			change_img_tmu(yes_plq_obj,&yn_blk,OPT_SCR_TID);
			change_img_tmu(no_plq_obj,&yn_blu,OPT_SCR_TID);
			glow_obj->y = option_xys[ver][0][Y_VAL]-20 - 30;
			snd_scall_bank(plyr_bnk_str,TMSEL_CURSR_SND,VOLUME6,127,PRIORITY_LVL3);
	 	}
		if (get_but_val_down(pnum))
	 		break;													// exit

		glow_obj->state.constant_color &= 0x00FFFFFF;
		glow_obj->state.constant_color |= alpha_level << 24;
		generate_sprite_verts(glow_obj);

		sleep(1);

		alpha_level += delta;
		if (alpha_level < 128)
		{
			alpha_level = 128;
			delta = +8;
		}
		else if (alpha_level > 255)
		{
			alpha_level = 255;
			delta = -8;
		}

	}

	// cleanup
	snd_scall_bank(cursor_bnk_str,SELECT_SND1,VOLUME7,127,PRIORITY_LVL3);
	if (answer == NO)
	{
		disc_flash(no_plq_obj,SPFX_TID,0.0f,0.0f,0.94f,pnum);
		del1c(plyr_plaq_ids[pnum],-1);
		delete_multiple_strings(0x344+pnum, -1);
		del1c(OID_MISC+pnum,-1);
		exit_status &= ~(1<<pnum);																				// CLEAR players bit in variable (andn)
		die(0);
	}
	else
		disc_flash(yes_plq_obj,SPFX_TID,0.0f,0.0f,0.94f,pnum);

//	del1c(555+pnum,-1);
	delete_multiple_strings(0x344+pnum, -1);
	del1c(OID_MISC+pnum,-1);

	// print header on plaq.
	set_text_z_inc(-0.01f);
	set_string_id(666 + pnum);
	set_text_font(FONTID_NBA10);
	set_text_position(plaq_xs[pnum][four_plr_ver]-4.0f, SPRITE_VRES-45.0f, 20.0F);
	oprintf("%dj%dcENTER NAME", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_z_inc(0.0f);

	// Init name and pin array
	for (i=0; i < LETTERS_IN_NAME; i++)
		prec->plyr_name[i] = '-';
	for (i=0; i < PIN_NUMBERS; i++)
		prec->plyr_pin_nbr[i] = '-';
	
	entered_cnt = 0;											// no entered data
	cur_pos = 0;
	menu_level = INITS;
	tbl_w = table_sizes[menu_level][four_plr_ver][0];
	tbl_h = table_sizes[menu_level][four_plr_ver][1];
	cur_tbl = alpha_table;

//	cur_img = ((pnum * LTR_COLS * LTR_ROWS) + (MAX_PLYRS * LTR_COLS * LTR_ROWS * four_plr_ver)) + cur_pos;

	// create the name-entry letters
	for (i=0; i < (LTR_COLS*LTR_ROWS); i++)
	{
		name_entry_img_ptrs[pnum][i] = beginobj(inits_letter_imgs[i],
						plaq_xs[pnum][four_plr_ver]-70.0f,
						SPRITE_VRES-8.0f,
						10.0F,
						YES_NO_PLAQ_TID);
		name_entry_img_ptrs[pnum][i]->id = plyr_alpha_ids[pnum];
	}

	cursor_obj = beginobj(&l_cursr,
			name_entry_img_ptrs[pnum][0]->x-name_entry_img_ptrs[pnum][0]->ii->ax + (name_entry_img_ptrs[pnum][0]->ii->w / 2.0f),
			name_entry_img_ptrs[pnum][0]->y+name_entry_img_ptrs[pnum][0]->ii->ay + (name_entry_img_ptrs[pnum][0]->ii->h / 2.0f),
			 25.0F,
			 YES_NO_PLAQ_TID);

	glow_obj = beginobj(&l_cursr2,cursor_obj->x,cursor_obj->y,20.0F,YES_NO_PLAQ_TID);
//	glow_obj->state.alpha_rgb_dst_function = GR_BLEND_ONE;			// set additive blending!!
	generate_sprite_verts(glow_obj);

	sleep(1);

	switches = 0;
	buttons = 0;

	// Get name and pin from player
	while ((entered_cnt < (LETTERS_IN_NAME+PIN_NUMBERS)) && (!force_selection))
	{
		prec->plyr_name[entered_cnt] = cur_tbl[cur_pos];						// save selection

		switches = get_joy_val_down(pnum);
		buttons = get_but_val_down(pnum);
		if (buttons)
		{
			snd_scall_bank(cursor_bnk_str,SELECT_SND1,VOLUME5,127,PRIORITY_LVL3);

			// kill previous copy on same letter
			if (!switches)
			{	// no joystick movement
				killall(SPFX_PID+pnum,-1);
				del1c(0x6543+pnum,-1);
			}
			qcreate("dsc_flsh",SPFX_PID+pnum,disc_flash_proc,cursor_obj->x,cursor_obj->y,pnum,0);
			//disc_flash(cursor_obj,SPFX_TID,0.0f,0.0f,0.40f,1);

			entered_char = prec->plyr_name[entered_cnt];
			switch(entered_cnt)
			{
				// name
				case 0:																			// first letter of name
					if (entered_char == CH_DEL)
						break;																	// do nothing
					if (entered_char == CH_END)
						break;																	// do nothing
					entered_cnt++;
					break;
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:																			// last letter in name
					if (entered_char == CH_DEL)
					{
						prec->plyr_name[entered_cnt] = '-';					// erase current letter
						entered_cnt--;
						break;
					}
					if (entered_char == CH_END)
					{
						// fill out name with spaces!
						while (entered_cnt < LETTERS_IN_NAME)
						{
							prec->plyr_name[entered_cnt] = CH_SPC;					// erase current letter
							entered_cnt++;											// start entering PIN NUMBER
						}
						entered_cnt = LETTERS_IN_NAME-1;				// start entering PIN NUMBER
					 }
		
					// dont allow bad words or abbrevs.
					if (creatp_check_bad_name(prec->plyr_name,entered_cnt))
						break;														// bad fucking name found
		
					// valid character
					entered_cnt++;
		
					if (entered_cnt == LETTERS_IN_NAME)
					{
						// switch to entering PIN NUMBER
						menu_level = PIN_NBR;
						tbl_w = table_sizes[menu_level][four_plr_ver][0];
						tbl_h = table_sizes[menu_level][four_plr_ver][1];
	 					cur_tbl = pin_nbr_tbl;
						cur_pos = 0;
						// delete 'enter inits' letters
						del1c(plyr_alpha_ids[pnum],-1);
						// delete players name string
						delete_multiple_strings(plr_nme_str_ids[pnum], 0xffffffff);		// delete players name
						delete_multiple_strings(666 + pnum, 0xffffffff);				// delete plaq title

						set_string_id(666 + pnum);
						set_text_font(FONTID_NBA10);
						set_text_position(plaq_xs[pnum][four_plr_ver]-4.0f, SPRITE_VRES-45.0f, 20.0F);
						set_text_z_inc(-0.01f);
						oprintf("%dj%dcENTER PIN", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
						set_text_z_inc(0.0f);

						// create the pin-numbers
						for (i=0; i < (PIN_COLS*PIN_ROWS); i++)
						{
							name_entry_img_ptrs[pnum][i] = beginobj(pin_nbr_imgs[i],
								plaq_xs[pnum][four_plr_ver]-70.0f,
								SPRITE_VRES-13.0f,
								10.0F,
								YES_NO_PLAQ_TID);
							name_entry_img_ptrs[pnum][i]->id = plyr_alpha_ids[pnum];
						}
						cursor_obj->x = name_entry_img_ptrs[pnum][cur_pos]->x-name_entry_img_ptrs[pnum][cur_pos]->ii->ax +
										 (name_entry_img_ptrs[pnum][cur_pos]->ii->w / 2.0f);
						cursor_obj->y = name_entry_img_ptrs[pnum][cur_pos]->y+name_entry_img_ptrs[pnum][cur_pos]->ii->ay +
										 (name_entry_img_ptrs[pnum][cur_pos]->ii->h / 2.0f);
						generate_sprite_verts(cursor_obj);
						glow_obj->x = cursor_obj->x;
						glow_obj->y = cursor_obj->y;
						//generate_sprite_verts(glow_obj);
					}
					break;
				// pin number
				case 6:																			// first nbr of pin nbr.
					if (entered_char == CH_DEL)
						break;																	// do nothing
					entered_cnt++;
					break;
				case 7:
				case 8:
				case 9:																			// last nbr of pin nbr.
					if (entered_char == CH_DEL)
					{
						prec->plyr_name[entered_cnt] = '-';					// erase current letter
						entered_cnt--;
						break;
					}
					entered_cnt++;
					if (entered_cnt == LETTERS_IN_NAME+PIN_NUMBERS)
						menu_level = TM_SELECT;														// player ready for team select
					break;
			}
		}
		//switches = get_joy_val_down(pnum);
		if ((switches)	&& (entered_cnt < LETTERS_IN_NAME+PIN_NUMBERS))
		{
			switch(switches)
			{
				case JOY_UP:																					// up
					if (cur_pos > (tbl_w-1))									// on top row ?
					{
						cur_pos-=tbl_w;	
						cursor_obj->x = name_entry_img_ptrs[pnum][cur_pos]->x - name_entry_img_ptrs[pnum][cur_pos]->ii->ax +
										 (name_entry_img_ptrs[pnum][cur_pos]->ii->w / 2.0f) - 3.0f;
						cursor_obj->y = name_entry_img_ptrs[pnum][cur_pos]->y+name_entry_img_ptrs[pnum][cur_pos]->ii->ay +
										 (name_entry_img_ptrs[pnum][cur_pos]->ii->h / 2.0f);
						generate_sprite_verts(cursor_obj);
						glow_obj->y = cursor_obj->y;
						//generate_sprite_verts(glow_obj);
						snd_scall_bank(plyr_bnk_str,CURSOR_SND1,VOLUME4,127,PRIORITY_LVL2);				// play sound
					}
					break;
				case JOY_DOWN:																					// down
					if (cur_pos < ((tbl_h*tbl_w)-tbl_w))			// on last row ?
					{
						cur_pos+=tbl_w;													// no... allow down
						cursor_obj->x = name_entry_img_ptrs[pnum][cur_pos]->x - name_entry_img_ptrs[pnum][cur_pos]->ii->ax +
										 (name_entry_img_ptrs[pnum][cur_pos]->ii->w / 2.0f) - 3.0f;
						cursor_obj->y = name_entry_img_ptrs[pnum][cur_pos]->y+name_entry_img_ptrs[pnum][cur_pos]->ii->ay +
										 (name_entry_img_ptrs[pnum][cur_pos]->ii->h / 2.0f);
						generate_sprite_verts(cursor_obj);
						glow_obj->y = cursor_obj->y;
						//generate_sprite_verts(glow_obj);
						snd_scall_bank(plyr_bnk_str,CURSOR_SND1,VOLUME4,127,PRIORITY_LVL2);				// play sound
					}
					break;
				case JOY_LEFT:																					// left
				case JOY_DWN_LFT:																					// left
				case JOY_UP_LFT:																					// left
					if (cur_pos > 0)															// on upper left most pos ?
					{
						cur_pos--;																	// no... allow left
						cursor_obj->x = name_entry_img_ptrs[pnum][cur_pos]->x - name_entry_img_ptrs[pnum][cur_pos]->ii->ax +
										 (name_entry_img_ptrs[pnum][cur_pos]->ii->w / 2.0f) - 3.0f;
						cursor_obj->y = name_entry_img_ptrs[pnum][cur_pos]->y+name_entry_img_ptrs[pnum][cur_pos]->ii->ay +
										 (name_entry_img_ptrs[pnum][cur_pos]->ii->h / 2.0f);
						generate_sprite_verts(cursor_obj);
						glow_obj->x = cursor_obj->x;
						glow_obj->y = cursor_obj->y;
						//generate_sprite_verts(glow_obj);
						snd_scall_bank(plyr_bnk_str,CURSOR_SND1,VOLUME4,127,PRIORITY_LVL2);				// play sound
					}
					break;
				case JOY_RIGHT:																					// right
				case JOY_DWN_RGT:																						// left
				case JOY_UP_RGT:																					// left
					if (cur_pos < ((tbl_w*tbl_h)-1))			// on lower right most pos ?
					{
						cur_pos++;																	// no... allow right
						cursor_obj->x = name_entry_img_ptrs[pnum][cur_pos]->x - name_entry_img_ptrs[pnum][cur_pos]->ii->ax +
										 (name_entry_img_ptrs[pnum][cur_pos]->ii->w / 2.0f) - 3.0f;
						cursor_obj->y = name_entry_img_ptrs[pnum][cur_pos]->y+name_entry_img_ptrs[pnum][cur_pos]->ii->ay +
										 (name_entry_img_ptrs[pnum][cur_pos]->ii->h / 2.0f);
						generate_sprite_verts(cursor_obj);
						glow_obj->x = cursor_obj->x;
						glow_obj->y = cursor_obj->y;
						//generate_sprite_verts(glow_obj);
						snd_scall_bank(plyr_bnk_str,CURSOR_SND1,VOLUME4,127,PRIORITY_LVL2);				// play sound
					}
					break;
			}
		}
		if (entered_cnt < LETTERS_IN_NAME)
			print_players_name(pnum, japple_box_x, japple_box_y, prec->plyr_name, 0);
		else
		{
			// Print pin number
			delete_multiple_strings(plr_pin_str_ids[pnum], 0xffffffff);

			set_string_id(plr_pin_str_ids[pnum]);
			set_text_font(FONTID_NBA20);

			set_text_position(japple_box_x, japple_box_y + (is_low_res ? 7.0f : 10.0f), 1.2F);
			set_text_z_inc(-0.01f);
			oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_TOP), LT_YELLOW, pin_strs[entered_cnt-LETTERS_IN_NAME]);
			set_text_z_inc(0.0f);
		}

		// update glow obj. aphla
		glow_obj->state.constant_color &= 0x00FFFFFF;
		glow_obj->state.constant_color |= alpha_level << 24;
		generate_sprite_verts(glow_obj);

		sleep (1);

		alpha_level += delta;
		if (alpha_level < 128)
		{
			alpha_level = 128;
			delta = +8;
		}
		else if (alpha_level > 255)
		{
			alpha_level = 255;
			delta = -8;
		}

	}

	// AT this point either the player entered NAME and PIN NUMBER or the screen timed out

	// kill disc flashes
	killall(SPFX_PID+pnum,-1);
	del1c(0x6543+pnum,-1);

	delete_multiple_strings(666 + pnum, 0xffffffff);				// delete plaq title
	delete_multiple_strings(plr_pin_str_ids[pnum], -1);					// delete pin number
	delobj(cursor_obj);
	delobj(glow_obj);

	// delete plaque text (pin numbers)
	del1c(plyr_alpha_ids[pnum],-1);
	
	// did player enter name and pin ?
	if ((entered_cnt >= LETTERS_IN_NAME+PIN_NUMBERS) && (prec->plyr_name[0] != NULL))
	{
		// check name and pin for SECRET heads
		find_player_record(prec->plyr_name, &prec->plyr_name[LETTERS_IN_NAME], 0, 0, pnum, &selected_players[pnum]);
		is_guest[pnum] = (selected_players[pnum].record_type == NBA_PLAYER_RECORD) ||
						(selected_players[pnum].record_type == SECRET_PLAYER_RECORD) ||
						(selected_players[pnum].record_type == CREATP_RECORD);
	   
		if (selected_players[pnum].record_type == SECRET_PLAYER_RECORD)
			snd_scall_bank(cursor_bnk_str, 57, VOLUME7, 127, PRIORITY_LVL5);
	
		rnum = selected_players[pnum].record_num;
		if (rnum != -1)
		{
			// Clean up bonus flashing message
			delete_multiple_strings(1, 0xffffffff);
			killall(BONUS_PID,0xFFFFFFFF);
//			if(pflasher)
//			{
//				kill(pflasher, 0);
//				pflasher = NULL;
//			}

			// copy cmos record into ram	
			get_generic_record(PLYR_RECS_ARRAY,rnum,plyr_data_ptrs[pnum]);			// get record from cmos into ram

			print_players_name(pnum, japple_box_x, japple_box_y, prec->plyr_name, 1);
			
			snd_scall_bank(cursor_bnk_str,RECORD_FOUND_SND,VOLUME6,127,PRIORITY_LVL3);				// play souund

			// now...print the stats
			print_player_record_stats(pnum,sectn_cntr_x,0);

			// dont let player exit..until all his plaques have been seen.
			while (showing_awards & (1<<pnum))
				sleep(1);

		}
		else
		{
			if (selected_players[pnum].record_type == NO_RECORD) {
				selected_players[pnum].record_num = get_free_record_nbr();
#ifdef DEBUG
fprintf(stderr, "in find record_num = %d\n", selected_players[pnum].record_num);
#endif
				// RECORD NOT FOUND..msg

				for(i = 0; i < LETTERS_IN_NAME; i++)
					selected_players[pnum].cmos_data.plyr_name[i] = prec->plyr_name[i];
				for(i = 0; i < PIN_NUMBERS; i++)
					selected_players[pnum].cmos_data.plyr_pin_nbr[i] = prec->plyr_pin_nbr[i];

				print_players_name(pnum, japple_box_x, japple_box_y, prec->plyr_name, 0);

				set_text_font(FONTID_NBA10);
				set_string_id(plyr_gen_str_ids[pnum]);
				set_text_position(sectn_cntr_x-2.0f, 270.0f, 15.0F);
				set_text_z_inc(-0.01f);
				oprintf("%dj%dcRECORD", (HJUST_CENTER|VJUST_CENTER), WHITE);
				set_text_position(sectn_cntr_x-2.0f, 250.0f, 15.0F);
				oprintf("%dj%dcNOT FOUND", (HJUST_CENTER|VJUST_CENTER), WHITE);

				set_text_position(sectn_cntr_x-2.0f, 195.0f, 15.0F);
				oprintf("%dj%dcNEW RECORD", (HJUST_CENTER|VJUST_CENTER), WHITE);
				set_text_position(sectn_cntr_x-2.0f, 175.0f, 15.0F);
				oprintf("%dj%dcCREATED", (HJUST_CENTER|VJUST_CENTER), WHITE);
				set_text_z_inc(0.0f);

				wait_for_any_butn_press_active_plyr(15,tsec*5,pnum);
				// !!!FIX
				//			beginobj(&mblank, sectn_cntr_x, SPRITE_VRES/2.0F, 16.0F, JAPPLE_BOX_TID);
				//			beginobj(&mrecnf, sectn_cntr_x, SPRITE_VRES/2.0F, 15.0F, JAPPLE_BOX_TID);
				//			sleep(120);					// show for a minium of 2 seconds
			} else {
				/* do a cool sounds for a NBA/secret player? */
			}
		}
	}
	else
	{
		// player didnt finish entering name and pin... make it an invalid record
		delete_multiple_strings(plr_nme_str_ids[pnum], -1);					// delete pin number
		delete_multiple_strings(plr_pin_str_ids[pnum], -1);					// delete pin number
		prec->plyr_name[0] = NULL;

		// say "record not created!!"
		set_text_font(FONTID_NBA10);
		set_string_id(plyr_gen_str_ids[pnum]);
		set_text_position(sectn_cntr_x-2.0f, 270.0f, 15.0F);
		set_text_z_inc(-0.01f);
		oprintf("%dj%dcRECORD", (HJUST_CENTER|VJUST_CENTER), WHITE);
		set_text_position(sectn_cntr_x-2.0f, 250.0f, 15.0F);
		oprintf("%dj%dcNOT CREATED", (HJUST_CENTER|VJUST_CENTER), WHITE);
		set_text_z_inc(0.0f);

		sleep(90);					// show for a minium of 2 seconds

	}

	// delete plaque
	del1c(plyr_plaq_ids[pnum],-1);

	// delete misc player text
	delete_multiple_strings(plyr_gen_str_ids[pnum], -1);

	// signal that player is done
	exit_status &= ~(1<<pnum);																				// CLEAR players bit in variable (andn)
}


//-------------------------------------------------------------------------------------------------------------------------
//							This process displays some effect when the player buys a full game.
//
//   INPUT: player number
// RETURNS: Nothing
//-------------------------------------------------------------------------------------------------------------------------
void plyr_stat_award_msgs(int *args)
{
	sprite_info_t	*plq_obj;
	sprite_info_t	*msg_obj;
	image_info_t	*img;
//	sprite_info_t	*flsh_obj;
	float			angle;
	int				p_num;
	float			dest_y;
	signed int		delta_y;
	int				time;						// in ticks for plaq to land in japple box

	p_num = args[0];
	angle = 0.0f;
	time = 20;
	dest_y = station_xys[p_num][four_plr_ver][Y_VAL]-140.0F;
//	snd_scall_bank(cursor_bnk_str,FULLGM_APPEAR_SND,VOLUME4,127,PRIORITY_LVL3);

//&bdefwrd
//&boffwrd
//&cplyrwrd

	//
	// players rank plaque
	//




	//FIX!!!  Add create a player plaque that falls down - art is done
	img = NULL;
	if (calc_tms_def(plyr_data_ptrs[p_num]->teams_defeated) >= NUM_TEAMS)
		img = &gchmpwrd;
	else if (plr_ranks[p_num][OVR_RNK] == 1)
		img = &top5wrd;
	else if (plr_ranks[p_num][OVR_RNK] <= 5)
		img = &top5wrd;
	else if (plr_ranks[p_num][OVR_RNK] <= 10)
		img = &top10wrd;
	if (img != NULL)
	{
		plq_obj = beginobj(jbox_plaq_imgs[four_plr_ver][p_num],
			station_xys[p_num][four_plr_ver][X_VAL]-4.0f,
			SPRITE_VRES,
			1.18f,
			JAPPLE_BOX_TID);

		delta_y = dest_y - plq_obj->y;
		plq_obj->y_vel = (float)delta_y/time;
//		plq_obj->x_ang_vel = (float)delta_y/(time*8);
		plq_obj->id = plyr_award_plq_ids[p_num];

		msg_obj = beginobj(img,
			station_xys[p_num][four_plr_ver][X_VAL]-4.0f,
			SPRITE_VRES,
			1.17f,
			JAPPLE_BOX_TID);
		msg_obj->y_vel = (float)delta_y/time;
//		msg_obj->x_ang_vel = (float)delta_y/(time*8);
		msg_obj->id = plyr_award_plq_ids[p_num];

		set_veladd_mode(plq_obj,1);
		set_veladd_mode(msg_obj,1);

		do
		{
			sleep(1);
		} while (--time);

		set_veladd_mode(plq_obj,0);
		set_veladd_mode(msg_obj,0);
//		plq_obj->x_angle = 0.0f;
//		msg_obj->x_angle = 0.0f;
		generate_sprite_verts(plq_obj);
		generate_sprite_verts(msg_obj);

		snd_scall_bank(cursor_bnk_str,FULLGM_HIT_SND,VOLUME7,127,PRIORITY_LVL3);
		sleep(60);
		del1c(plyr_award_plq_ids[p_num],0xffffffff);
	}

	//
	// Best offense
	//
	time = 20;
	if (plr_ranks[p_num][OFF_RNK] == 1)
	{
		plq_obj = beginobj(jbox_plaq_imgs[four_plr_ver][p_num],
			station_xys[p_num][four_plr_ver][X_VAL]-4.0f,
			SPRITE_VRES,
			1.18f,
			JAPPLE_BOX_TID);

		delta_y = dest_y - plq_obj->y;
		plq_obj->y_vel = (float)delta_y/time;
//		plq_obj->x_ang_vel = (float)delta_y/(time*8);
		plq_obj->id = plyr_award_plq_ids[p_num];

		msg_obj = beginobj(&boffwrd,
			station_xys[p_num][four_plr_ver][X_VAL]-4.0f,
			SPRITE_VRES,
			1.17f,
			JAPPLE_BOX_TID);
		msg_obj->y_vel = (float)delta_y/time;
//		msg_obj->x_ang_vel = (float)delta_y/(time*8);
		msg_obj->id = plyr_award_plq_ids[p_num];

		set_veladd_mode(plq_obj,1);
		set_veladd_mode(msg_obj,1);

		do
		{
			sleep(1);
		} while (--time);

		set_veladd_mode(plq_obj,0);
		set_veladd_mode(msg_obj,0);
//		plq_obj->x_angle = 0.0f;
//		msg_obj->x_angle = 0.0f;
		generate_sprite_verts(plq_obj);
		generate_sprite_verts(msg_obj);

		snd_scall_bank(cursor_bnk_str,FULLGM_HIT_SND,VOLUME7,127,PRIORITY_LVL3);
		sleep(60);
		del1c(plyr_award_plq_ids[p_num],0xffffffff);
	}

	//
	// Best defense
	//
	time = 20;
	if (plr_ranks[p_num][DEF_RNK] == 1)
	{
		plq_obj = beginobj(jbox_plaq_imgs[four_plr_ver][p_num],
			station_xys[p_num][four_plr_ver][X_VAL]-4.0f,
			SPRITE_VRES,
			1.18f,
			JAPPLE_BOX_TID);

		delta_y = dest_y - plq_obj->y;
		plq_obj->y_vel = (float)delta_y/time;
//		plq_obj->x_ang_vel = (float)delta_y/(time*8);
		plq_obj->id = plyr_award_plq_ids[p_num];

		msg_obj = beginobj(&bdefwrd,
			station_xys[p_num][four_plr_ver][X_VAL]-4.0f,
			SPRITE_VRES,
			1.17f,
			JAPPLE_BOX_TID);
		msg_obj->y_vel = (float)delta_y/time;
//		msg_obj->x_ang_vel = (float)delta_y/(time*8);
		msg_obj->id = plyr_award_plq_ids[p_num];

		set_veladd_mode(plq_obj,1);
		set_veladd_mode(msg_obj,1);

		do
		{
			sleep(1);
		} while (--time);

		set_veladd_mode(plq_obj,0);
		set_veladd_mode(msg_obj,0);
//		plq_obj->x_angle = 0.0f;
//		msg_obj->x_angle = 0.0f;
		generate_sprite_verts(plq_obj);
		generate_sprite_verts(msg_obj);

		snd_scall_bank(cursor_bnk_str,FULLGM_HIT_SND,VOLUME7,127,PRIORITY_LVL3);
		sleep(60);
		del1c(plyr_award_plq_ids[p_num],0xffffffff);
	}


//	flsh_obj = beginobj(stat_flsh_imgs[0],
//		japbx_cntr_xys[p_num][four_plr_ver][X_VAL],
//		japbx_cntr_xys[p_num][four_plr_ver][Y_VAL]+2.0f,
//		1.15f - 0.005f * (float)p_num,
//		JAPPLE_BOX_TID);
//	flsh_obj->w_scale = 2.4f;
//	flsh_obj->h_scale = 4.2f;
//	flsh_obj->id = OID_JAPPLE_BOX;
//	generate_sprite_verts(flsh_obj);

//	for (i=0; i < (sizeof(stat_flsh_imgs)/sizeof(image_info_t *))-1; i++)
//	{
//		change_img_tmu(flsh_obj,stat_flsh_imgs[i],JAPPLE_BOX_TID);
//		sleep(3);
//	}
//	delobj(flsh_obj);

	// no longer showing this dudes awards
	showing_awards &= ~(1<<p_num);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
float print_players_name(int pnum, float x, float y, char *name, int showpts)
{
	char pname[LETTERS_IN_NAME];
	int  h;

//	if (name[0] == NULL)
//		return 0;

	x -= 3.0f;

	if (showpts >= 0)
	{
		int i = LETTERS_IN_NAME - 1;
		strncpy(pname, name, LETTERS_IN_NAME);
		while(i >= 0)
		{
			if(pname[i] != ' ')
			{
				break;
			}
			i--;
		}
		pname[i+1] = 0;
		delete_multiple_strings(plr_nme_str_ids[pnum], 0xffffffff);
		set_string_id(plr_nme_str_ids[pnum]);
	}

	set_text_z_inc(-0.01f);
	set_text_justification_mode(HJUST_CENTER|VJUST_TOP);

	if (showpts)
	{
		h  = (is_low_res ? FONTID_BAST8T : FONTID_NBA12);
		y += (is_low_res ? 6 : 6);
	}
	else
	{
		h  = (is_low_res ? FONTID_NBA10 : FONTID_NBA14);
		if (is_low_res) y -= 2.0f;
	}
	set_text_font(h);
	h  = get_font_height(h);
	y += (float)(h/2);

	set_text_position(x, y, 4.5f);/*1.2f*/
	if (showpts < 0)
		oprintf("%dcFREE", LT_YELLOW);
	else
		oprintf("%dc%s", LT_YELLOW, pname);

	if (showpts)
	{
		y -= (float)(h + (is_low_res ? 5 : 0));

		set_text_position(x, y, 4.5f);/*1.19f*/
		if (showpts < 0)
			oprintf("%dcGAME", LT_YELLOW);
		else
		if (plyr_data_ptrs[pnum]->trivia_pts)
			oprintf("%dc%d%dc POINT%c",
				LT_RED2,//0xffff5050,//LT_CYAN,
				plyr_data_ptrs[pnum]->trivia_pts,
				WHITE,
				plyr_data_ptrs[pnum]->trivia_pts != 1 ? 'S' : '\0');
		else
			oprintf("%dcNO POINTS",
				WHITE);
	}
	set_text_z_inc(0.0f);
	return x;
}

//-------------------------------------------------------------------------------------------------
//								This routine prints the players STATS in the players section
//-------------------------------------------------------------------------------------------------
//static plyr_record_t	plrs_rec;

static void copy_record(plyr_record_t *to, plyr_record_t *from)
{
	int	i;

	for(i = 0; i < LETTERS_IN_NAME; i++)
	{
		to->plyr_name[i] = from->plyr_name[i];
	}
	for(i = 0; i < PIN_NUMBERS; i++)
	{
		to->plyr_pin_nbr[i] = from->plyr_pin_nbr[i];
	}
	to->last_used = from->last_used;
	to->games_played = from->games_played;
	to->games_won = from->games_won;
	to->winstreak = from->winstreak;
	to->points_scored = from->points_scored;
	to->points_allowed = from->points_allowed;
	to->teams_defeated = from->teams_defeated;


	to->three_ptrs_attempted = from->three_ptrs_attempted;
	to->three_ptrs_made = from->three_ptrs_made;
	to->dunks_attempted = from->dunks_attempted;
	to->dunks_made = from->dunks_made;
	to->alt_ladder = from->alt_ladder;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
int print_player_record_stats(int p_num, int cntr_x, int only_1_page)
{
	plyr_record_t	*plr_rec;
//	plyr_record_t	*plr_rec = &plrs_rec;
	float			msg_spc;
	//sprite_info_t 	*stat_obj;
	//sprite_info_t 	*dshdw_obj;
	int				i,j;
	int				ver;

	// make sure records are in ram!!
	if (!rec_sort_ram)
		return 0;

	ver = four_plr_ver;

	msg_spc = 14.0F;

	//_plr_rec = plyr_data_ptrs[p_num];

	plr_rec = plyr_data_ptrs[p_num];
	// point to the players record in ram
//	_plr_rec = &rec_sort_ram[find_record_in_ram(_plr_rec->plyr_name)-1];
//	copy_record(&plrs_rec, _plr_rec);

	//
	// Print Stats:
	//
	set_text_z_inc(0.1f);
	set_string_id(666 + p_num);
	set_text_font(FONTID_NBA10);
	set_text_position(plaq_xs[p_num][four_plr_ver]-4.0f, SPRITE_VRES-45.0f, 20.0F);
	oprintf("%dj%dcSTATS", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);

	if (is_low_res)
		set_text_font(FONTID_BAST8T);

//	set_text_font(FONTID_NBA10);
	set_string_id(STAT_PG_TXT_ID+p_num);

	// AWARD ATTRIBUTE points msg!!!
	if ((only_1_page == 1) &&
		(selected_players[p_num].record_type == CREATP_RECORD) &&
		was_awarded_attr[p_num])
	{
		int x_pos, y_pos;
		
		x_pos = stat_plqs_xys[ver][0][p_num][X_VAL]-4.0f;
		y_pos = stat_plqs_xys[ver][1][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][1];
		
		set_text_justification_mode(HJUST_CENTER|VJUST_CENTER);
		set_text_color(WHITE);
		set_text_position(x_pos, y_pos, 30.0F);
		y_pos -= 18;
		oprintf("2 ATTRIBUTE");
		
		set_text_position(x_pos, y_pos, 30.0F);
		y_pos -= 18;
		oprintf("POINTS");
		
		set_text_position(x_pos, y_pos, 30.0F);
		y_pos -= 26;
		oprintf("AWARDED!");
		
		set_text_position(x_pos, y_pos, 30.0F);
		y_pos -= 18;
		oprintf("ENTER CREATE");

		set_text_position(x_pos, y_pos, 30.0F);
		y_pos -= 18;
		oprintf("PLAYER TO");

		set_text_position(x_pos, y_pos, 30.0F);
		y_pos -= 18;
		oprintf("DISTRIBUTE");

		set_text_position(x_pos, y_pos, 30.0F);
		y_pos -= 18;
		oprintf("POINTS");
		snd_scall_bank(cursor_bnk_str, 57, VOLUME7, 127, PRIORITY_LVL5);
		return 1;
	}

	//
	// *** PAGE 1 ***
	//

	// print OVERALL rank
	qsort(rec_sort_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_wins);
	plr_ranks[p_num][OVR_RNK] = find_record_in_ram(plr_rec->plyr_name);

	set_text_position(stat_plqs_xys[ver][0][p_num][X_VAL]-4.0f, stat_plqs_xys[ver][0][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][0], 30.0F);
	oprintf("%dj%dc RANK", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][0][p_num][X_VAL]-4.0f, stat_plqs_xys[ver][0][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][0], 30.0F);
	oprintf("%dj%dc%d / %d", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_ranks[p_num][OVR_RNK], NUM_PLYR_RECORDS);

	// print WIN-LOSS record
	set_text_position(stat_plqs_xys[ver][1][p_num][X_VAL]-4.0f, stat_plqs_xys[ver][1][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][1], 30.0F);
	oprintf("%dj%dc RECORD", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][1][p_num][X_VAL]-4.0f, stat_plqs_xys[ver][1][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][1], 30.0F);
	oprintf("%dj%dc %d W/ %d L", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_rec->games_won, (plr_rec->games_played-plr_rec->games_won));

	// print OFFFENSIVE RANK
	qsort(rec_sort_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_points_scored);
	plr_ranks[p_num][OFF_RNK] = find_record_in_ram(plr_rec->plyr_name);

	set_text_position(stat_plqs_xys[ver][2][p_num][X_VAL]-4.0f, stat_plqs_xys[ver][2][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][2], 30.0F);
	oprintf("%dj%dc OFF. RANK", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][2][p_num][X_VAL]-4.0f, stat_plqs_xys[ver][2][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][2], 30.0F);
	oprintf("%dj%dc%d / %d", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_ranks[p_num][OFF_RNK], NUM_PLYR_RECORDS);

	// print DEFENSIVE RANK
	qsort(rec_sort_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_points_allowed);
	plr_ranks[p_num][DEF_RNK] = find_record_in_ram(plr_rec->plyr_name);

	set_text_position(stat_plqs_xys[ver][3][p_num][X_VAL]-4.0f, stat_plqs_xys[ver][3][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][3], 30.0F);
	oprintf("%dj%dc DEF. RANK", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][3][p_num][X_VAL]-4.0f, stat_plqs_xys[ver][3][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][3], 30.0F);
	oprintf("%dj%dc%d / %d", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_ranks[p_num][DEF_RNK], NUM_PLYR_RECORDS);

	// print WINSTREAK
	qsort(rec_sort_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_win_streaks);
	plr_ranks[p_num][STK_RNK] = find_record_in_ram(plr_rec->plyr_name);

	set_text_position(stat_plqs_xys[ver][4][p_num][X_VAL]-4.0f, stat_plqs_xys[ver][4][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][4], 30.0F);
	oprintf("%dj%dc STREAK", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][4][p_num][X_VAL]-4.0f, stat_plqs_xys[ver][4][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][4], 30.0F);

	{
		int tmp = plr_rec->winstreak;
		if (tmp < 0) {
			tmp = -tmp;
			oprintf("%dj%dc %d LOSS%s", (HJUST_CENTER|VJUST_CENTER), WHITE, tmp, tmp != 1 ? "ES" : "");
		} else
			oprintf("%dj%dc %d WIN%s", (HJUST_CENTER|VJUST_CENTER), WHITE, tmp, tmp != 1 ? "S" : "");
	}

	// print TEAMS DEFEATED
	set_text_position(stat_plqs_xys[ver][5][p_num][X_VAL]-4.0f, stat_plqs_xys[ver][5][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][5], 30.0F);
	oprintf("%dj%dc DEFEATED", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][5][p_num][X_VAL]-4.0f, stat_plqs_xys[ver][5][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][5], 30.0F);
	{
		int tmp;
		tmp = calc_tms_def(plr_rec->teams_defeated);
		if (tmp <= NUM_TEAMS)
			oprintf("%dj%dc %d / 29 TEAMS", (HJUST_CENTER|VJUST_CENTER), WHITE, tmp);
		else
			oprintf("%dj%dc ALL TEAMS", (HJUST_CENTER|VJUST_CENTER), WHITE);
	}

	if (only_1_page != 0)
		return 0;

	// Show player rank awards (ego boost)
	showing_awards |= (1<<p_num);
	qcreate("plrawrds",PLAQ_AWARD_PID+p_num,plyr_stat_award_msgs,p_num,0,0,0);

	// wait for player to change page
	wait_for_any_butn_press_active_plyr(25,tsec*10,p_num);
	snd_scall_bank(cursor_bnk_str,NEXT_PAGE_SND,VOLUME4,127,PRIORITY_LVL2);				// play sound

	delete_multiple_strings(STAT_PG_TXT_ID+p_num, 0xffffffff);							// delete the stats
//	sleep(1);
	
	if (plr_rec->games_won > 0)
	{
		//
		// *** PAGE 2 ***
		//
		if (is_low_res)
			set_text_font(FONTID_BAST8T);
		else
			set_text_font(FONTID_NBA10);
			
		set_string_id(STAT_PG_TXT_ID+p_num);

		// print AVG. PTS. SCORED per game
		set_text_position(stat_plqs_xys[ver][0][p_num][X_VAL]-4.0f, stat_plqs_xys[ver][0][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][0], 30.0F);
		oprintf("%dj%dcPTS. SCORED", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
		set_text_position(stat_plqs_xys[ver][0][p_num][X_VAL]-4.0f, stat_plqs_xys[ver][0][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][0], 30.0F);
		oprintf("%dj%dc %hd PTS/GM", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_rec->points_scored/plr_rec->games_played);

		// print AVG. PTS. ALLOWED per game
		set_text_position(stat_plqs_xys[ver][1][p_num][X_VAL]-4.0f, stat_plqs_xys[ver][1][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][1], 30.0F);
		oprintf("%dj%dcPTS. ALLOWED", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
		set_text_position(stat_plqs_xys[ver][1][p_num][X_VAL]-4.0f, stat_plqs_xys[ver][1][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][1], 30.0F);
		oprintf("%dj%dc %u PTS/GM", (HJUST_CENTER|VJUST_CENTER), WHITE, (int)(plr_rec->points_allowed/plr_rec->games_played));

		// print THREE POINTERS
		set_text_position(stat_plqs_xys[ver][2][p_num][X_VAL]-4.0f, stat_plqs_xys[ver][2][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][2], 30.0F);
		oprintf("%dj%dc3 PT. SHOTS", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
		set_text_position(stat_plqs_xys[ver][2][p_num][X_VAL]-4.0f, stat_plqs_xys[ver][2][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][2], 30.0F);
		oprintf("%dj%dc %d", (HJUST_CENTER|VJUST_CENTER), WHITE, (int)plr_rec->three_ptrs_made);

		// print REBOUNDS
		set_text_position(stat_plqs_xys[ver][3][p_num][X_VAL]-4.0f, stat_plqs_xys[ver][3][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][3], 30.0F);
		oprintf("%dj%dcREBOUNDS", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
		set_text_position(stat_plqs_xys[ver][3][p_num][X_VAL]-4.0f, stat_plqs_xys[ver][3][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][3], 30.0F);
		oprintf("%dj%dc %d", (HJUST_CENTER|VJUST_CENTER), WHITE, (int)plr_rec->rebounds);

		// print STEALS
		set_text_position(stat_plqs_xys[ver][4][p_num][X_VAL]-4.0f, stat_plqs_xys[ver][4][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][4], 30.0F);
		oprintf("%dj%dcSTEALS", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
		set_text_position(stat_plqs_xys[ver][4][p_num][X_VAL]-4.0f, stat_plqs_xys[ver][4][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][4], 30.0F);
		oprintf("%dj%dc %d", (HJUST_CENTER|VJUST_CENTER), WHITE, (int)plr_rec->steals);

		// print ASSISTS
		set_text_position(stat_plqs_xys[ver][5][p_num][X_VAL]-4.0f, stat_plqs_xys[ver][5][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][5], 30.0F);
		oprintf("%dj%dcASSISTS", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
		set_text_position(stat_plqs_xys[ver][5][p_num][X_VAL]-4.0f, stat_plqs_xys[ver][5][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][5], 30.0F);
		oprintf("%dj%dc %d", (HJUST_CENTER|VJUST_CENTER), WHITE, (int)plr_rec->assists);

		// Show player rank awards
		// !!!FIX
//		qcreate("plrawrds",PLAQ_AWARD_PID+p_num,plyr_plaques_awards,p_num,0,0,0);

		// wait for player to change page
		wait_for_any_butn_press_active_plyr(15,tsec*10,p_num);
		snd_scall_bank(cursor_bnk_str,NEXT_PAGE_SND,VOLUME4,127,PRIORITY_LVL2);				// play sound

		del1c(OID_STAT2_PG+p_num, 0xffffffff);
//		killall(PLAQ_AWARD_PID+p_num, 0xffffffff);					// kill the plyr_plaques_awards process
		del1c(OID_STAT3_PG+p_num, 0xffffffff);
		delete_multiple_strings(STAT_PG_TXT_ID+p_num, 0xffffffff);							// delete the stats

		//
		// *** PAGE 3 *** (teams defeated)
		//
		plr_rec = plyr_data_ptrs[p_num];
	
		set_text_z_inc(0.1f);
		if (is_low_res)
			set_text_font(FONTID_BAST8T);
		else
			set_text_font(FONTID_NBA10);
		set_string_id(STAT_PG_TXT_ID+p_num);
	
		// print 'DEFEATED' header
//		stat_obj = beginobj(stats_plq_imgs[ver][p_num], stat_plqs_xys[ver][0][p_num][X_VAL], stat_plqs_xys[ver][0][p_num][Y_VAL], 52.0F, YES_NO_PLAQ_TID);
//		stat_obj->id = OID_STAT2_PG+p_num;
		if (calc_tms_def(plr_rec->teams_defeated) <= 14)
		{
			set_text_position(stat_plqs_xys[ver][0][p_num][X_VAL]-4.0f, stat_plqs_xys[ver][0][p_num][Y_VAL], 30.0F);
			oprintf("%dj%dcDEFEATED", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
			// print TEAMS DEFEATED
			for (i=0,j=0; i < NUM_TEAMS; i++)
			{
				if (plr_rec->teams_defeated & (1<<i))			// bit set ?
				{
					j++;
					set_text_position(stat_plqs_xys[ver][0][p_num][X_VAL]-4.0f, stat_plq_msg_xys[ver][p_num][Y_VAL]-(14.0F * j)-5, 30.0F);
					oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, teaminfo[i].szname);
				}
			}
		}
		else
		{
			set_text_position(stat_plqs_xys[ver][0][p_num][X_VAL]-4.0f, stat_plqs_xys[ver][0][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][0], 30.0F);
			oprintf("%dj%dcREMAINING", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
			// print REMAINING teams
			set_text_font(FONTID_BAST8T);
			for (i=0,j=0; i < NUM_TEAMS + 1; i++)
			{
				if (!(plr_rec->teams_defeated & (1<<i)))			// bit clear ?
				{
					/* only print the secret team title after we beaten all others! */
					if ((i == NUM_TEAMS) && (j > 0))
						break;
					j++;
					set_text_position(stat_plqs_xys[ver][0][p_num][X_VAL]-4.0f, stat_plq_msg_xys[ver][p_num][Y_VAL]-(14.0F * j)-5, 30.0F);
					oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, teaminfo[i].szname);
				}
			}
		}	
		// wait for player to change page
		wait_for_any_butn_press_active_plyr(15,tsec*10,p_num);	//50
		snd_scall_bank(cursor_bnk_str,NEXT_PAGE_SND,VOLUME4,127,PRIORITY_LVL2);				// play sound
	
	}
	delete_multiple_strings(666+p_num, 0xFFFFFFFF);							// delete the stats
	delete_multiple_strings(STAT_PG_TXT_ID+p_num, 0xFFFFFFFF);							// delete the stats
	del1c(OID_STAT_PG+p_num, 0xffffffff);
	del1c(OID_STAT2_PG+p_num, 0xffffffff);

	return 0;
}

//-------------------------------------------------------------------------------------------------
//				This process flashes the stat "area" of any stat inwhich the player in is the top 10
//
//  INPUT: X, Y, Z
//-------------------------------------------------------------------------------------------------
void stat_flash(int *args)
{
//
//	sprite_info_t 	*flsh_obj;
//	int	i;
//
//
//	snd_scall_bank(cursor_bnk_str,TM_SELECTED_SND,VOLUME2,127,PRIORITY_LVL3);
//
//	flsh_obj = beginobj(stat_flsh_imgs[0], args[0], args[1], 10.0F+args[3], JAPPLE_BOX_TID);
//	flsh_obj->w_scale = 2.0f;
//	flsh_obj->h_scale = 2.4f;
//	generate_sprite_verts(flsh_obj);
//
//	for (i=0; i < (sizeof(stat_flsh_imgs)/sizeof(image_info_t *))-1; i++)
//	{
//		change_img_tmu(flsh_obj,stat_flsh_imgs[i],JAPPLE_BOX_TID);
//		sleep(3);
//	}
//	delobj(flsh_obj);
//	flash_status &= ~(1<<(args[3]));														// CLEAR teams (1 or 2) bit in variable
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void disc_flash(sprite_info_t *obj, int tid, float xoff, float yoff, float scale, int val)
{
	sprite_info_t 	*flash_obj;
	sprite_info_t 	*flash2_obj;
	int				i;

	flash_obj = beginobj(optscr_flsh_imgs[0],
						 (obj->x-obj->ii->ax + (obj->ii->w / 2.0f))+xoff,
						 (obj->y+obj->ii->ay + (obj->ii->h / 2.0f))+yoff,
						 obj->z - .05f,
						 tid);
	flash_obj->id = 555+val;

	flash2_obj = beginobj(optscr_flsh2_imgs[0],
						 (obj->x-obj->ii->ax + (obj->ii->w / 2.0f))+xoff,
						 (obj->y+obj->ii->ay + (obj->ii->h / 2.0f))+yoff,
						 2.0f,
						 tid);
	flash2_obj->id = 555+val;

	flash_obj->w_scale = scale;
	flash_obj->h_scale = scale;
	// set to additive blending
	flash_obj->state.alpha_rgb_dst_function = GR_BLEND_ONE;
	generate_sprite_verts(flash_obj);

	flash2_obj->w_scale = scale * 1.8f;
	flash2_obj->h_scale = scale * 1.85f;
	// set to additive blending
	flash2_obj->state.alpha_rgb_dst_function = GR_BLEND_ONE;
	generate_sprite_verts(flash2_obj);

	for (i=0; i < (sizeof(optscr_flsh_imgs)/sizeof(image_info_t *))-1; i++)
	{
		change_img_tmu(flash_obj,optscr_flsh_imgs[i],tid);
		change_img_tmu(flash2_obj,optscr_flsh2_imgs[i],tid);
		sleep(2);
	}
	del1c(555+val,-1);
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void disc_flash_proc(int *args)
{
	sprite_info_t 	*flash_obj;
	sprite_info_t 	*flash2_obj;
	int				i;
	float			ax,ay,x,y;
	int				pnum;

	x = (float)args[0];
	y = (float)args[1];
	pnum = args[2];
	ax = 16.0f;
	ay = -16.0f;

	flash_obj = beginobj(optscr_flsh_imgs[0],
						 (x-ax + (33 / 2.0f)),
						 (y+ay + (34 / 2.0f)),
						 5.0f,
						 SPFX_TID);

	flash2_obj = beginobj(optscr_flsh2_imgs[0],
						 (x-ax + (33 / 2.0f)),
						 (y+ay + (34 / 2.0f)),
						 4.0f,
						 SPFX_TID);

	flash_obj->id = 0x6543+pnum;
	flash2_obj->id = 0x6543+pnum;
	flash_obj->w_scale = 0.32f;
	flash_obj->h_scale = 0.42f;
	// set to additive blending
	flash_obj->state.alpha_rgb_dst_function = GR_BLEND_ONE;
	generate_sprite_verts(flash_obj);

	flash2_obj->w_scale = 0.42f * 1.9f;
	flash2_obj->h_scale = 0.42f * 1.95f;
	// set to additive blending
	flash2_obj->state.alpha_rgb_dst_function = GR_BLEND_ONE;
	generate_sprite_verts(flash2_obj);

	for (i=0; i < (sizeof(optscr_flsh_imgs)/sizeof(image_info_t *))-1; i++)
	{
		change_img_tmu(flash_obj,optscr_flsh_imgs[i],SPFX_TID);
		change_img_tmu(flash2_obj,optscr_flsh2_imgs[i],SPFX_TID);
		sleep(1);
	}

	del1c(0x6543+pnum,-1);
}

//-------------------------------------------------------------------------------------------------
// Used for in game 3pt bubble flash
//-------------------------------------------------------------------------------------------------
void disc_flash_3pt_proc(int *args)
{
	sprite_info_t 	*flash_obj;
	sprite_info_t 	*flash2_obj;
	int				i;
	float			ax,ay,x,y;
	int				pnum;

	x = (float)args[0];
	y = (float)args[1];
	pnum = args[2];
	ax = 16.0f;
	ay = -16.0f;

	flash_obj = beginobj(optscr_flsh_imgs[0],
						 (x-ax + (33 / 2.0f)),
						 (y+ay + (34 / 2.0f)),
						 1.3f,
						 SPFX_TID);

//	flash2_obj = beginobj(optscr_flsh2_imgs[0],
//						 (x-ax + (33 / 2.0f)),
//						 (y+ay + (34 / 2.0f)),
//						 4.0f,
//						 SPFX_TID);

	flash_obj->id = 0x6543+pnum;
//	flash2_obj->id = 0x6543+pnum;
	flash_obj->w_scale = 1.08f;
	flash_obj->h_scale = 1.08f;
	// set to additive blending
	flash_obj->state.alpha_rgb_dst_function = GR_BLEND_ONE;
	generate_sprite_verts(flash_obj);

//	flash2_obj->w_scale = 0.42f * 1.9f;
//	flash2_obj->h_scale = 0.42f * 1.95f;
	// set to additive blending
//	flash2_obj->state.alpha_rgb_dst_function = GR_BLEND_ONE;
//	generate_sprite_verts(flash2_obj);

	for (i=0; i < (sizeof(optscr_flsh_imgs)/sizeof(image_info_t *))-1; i++)
	{
		change_img_tmu(flash_obj,optscr_flsh_imgs[i],SPFX_TID);
//		change_img_tmu(flash2_obj,optscr_flsh2_imgs[i],SPFX_TID);
		sleep(2);
	}

	del1c(0x6543+pnum,-1);
}

//-------------------------------------------------------------------------------------------------
//				This process flashes the stat "area" of any stat inwhich the player in is the top 10
//
//  INPUT: X, Y, Z, texture id
//-------------------------------------------------------------------------------------------------
void flash_cursor(sprite_info_t *obj, int tid)
{
//	sprite_info_t 	*burst_obj;
//	int		i;
//
//	burst_obj = beginobj(burst_flsh_imgs[0],
//						 obj->x-obj->ii->ax + (obj->ii->w / 2.0f),
//						 obj->y+obj->ii->ay + (obj->ii->h / 2.0f),
//						 5.0F,
//						 JAPPLE_BOX_TID);
//
//	burst_obj->w_scale = 2.4f;
//	burst_obj->h_scale = 2.4f;
//	burst_obj->state.alpha_rgb_dst_function = GR_BLEND_ONE;
////	burst_obj->state.chroma_key_mode = GR_CHROMAKEY_ENABLE;
//	generate_sprite_verts(burst_obj);
//
//	for (i=0; i < (sizeof(burst_flsh_imgs)/sizeof(image_info_t *))-1; i++)
//	{
//		change_img_tmu(burst_obj,burst_flsh_imgs[i],tid);
//		sleep(1);
//	}
//	delobj(burst_obj);
}

//-------------------------------------------------------------------------------------------------
//	This process flashes the object white
//
// 	INPUT:	Pointer to flash, cnt, delay
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void flash_obj_proc(int *args)
{ 
	flash_obj_white((sprite_info_t *)args[0], args[1], args[2]);
}

//-------------------------------------------------------------------------------------------------
//	Flash sprite white - 
//
//	INPUT:  Sprite *, # of times to flash, # of ticks between flashes
//	OUTPUT: Nothing
//-------------------------------------------------------------------------------------------------
void flash_obj_white(sprite_info_t *obj, int cnt, int delay)
{
	int i,cnst_clr,com_func;

	#ifdef DEBUG
	void	*parent;
	int		id;

	parent = obj->parent;
	id = obj->id;
	#endif

	#ifdef DEBUG
	assert( sprite_exists(obj));
	#else
	if (!sprite_exists(obj))
		return;
	#endif

	// save obj. state
	cnst_clr = obj->state.constant_color;
	com_func = obj->state.color_combiner_function;

//	obj->state.constant_color = 0xffFFFFFF;

	for (i=0; i < cnt; i++)
	{
		// turn obj. white
		obj->state.color_combiner_function = GR_COLORCOMBINE_ONE;
		obj->state.chroma_key_mode = GR_CHROMAKEY_ENABLE;

		sleep(delay);
		#ifdef DEBUG
		assert( sprite_exists(obj));
		#else
		if (!sprite_exists(obj))
			return;
		#endif

		// obj. back to color
		obj->state.color_combiner_function = com_func;
		obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
		sleep(delay);
		#ifdef DEBUG
		assert( sprite_exists(obj));
		#else
		if (!sprite_exists(obj))
			return;
		#endif
	}

	// restore obj. back to original state
	obj->state.constant_color = cnst_clr;
	obj->state.color_combiner_function = com_func;
	obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;

	#ifdef DEBUG
	obj->parent = parent;
	obj->id = id;							
	#endif
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//void trans_to_yn_plaq(sprite_info_t *p_plaq_obj, sprite_info_t *p_plaq_obj2, int pnum, float y_dest)
//{
//	signed int delta_y;
//	int time, i;
//
//	time = 15;			// in ticks
//	delta_y = y_dest - p_plaq_obj->y;
//	p_plaq_obj->y_vel = (float)delta_y/time;
//	p_plaq_obj2->y_vel = (float)delta_y/time;
////	p_shdw_obj->y_vel = (float)delta_y/time;
////	p_shdw_obj2->y_vel = (float)delta_y/time;
//
//	p_plaq_obj->y_end = y_dest;
//	p_plaq_obj2->y_end = y_dest;
////	p_shdw_obj->y_end = y_dest;
////	p_shdw_obj2->y_end = y_dest;
//
//	// Wait for plates; set time-out so we don't hang
//	i = PLATE_TRANSITION_TICKS * 2;
//	// Do it this way since we can't know how many ticks have lapsed since transition began
//	while (plates_on != PLATES_OFF && --i) sleep(1);
//	// Make sure plates are gone
//	plates_on = PLATES_OFF;
//
//	set_veladd_mode(p_plaq_obj,1);
//	set_veladd_mode(p_plaq_obj2,1);
////	set_veladd_mode(p_shdw_obj,1);
////	set_veladd_mode(p_shdw_obj2,1);
//
//	snd_scall_bank(cursor_bnk_str,PLAQ_SLIDE_SND,VOLUME4,127,PRIORITY_LVL1);
//
//	do
//	{
//		sleep(1);
//	} while (--time);
//
//	set_veladd_mode(p_plaq_obj,0);
//	set_veladd_mode(p_plaq_obj2,0);
////	set_veladd_mode(p_shdw_obj,0);
////	set_veladd_mode(p_shdw_obj2,0);
//
//	snd_scall_bank(cursor_bnk_str,PLAQ_STOP_SND,VOLUME2,127,PRIORITY_LVL2);
//}


//-------------------------------------------------------------------------------------------------
//											This routine changes the image and/or texture on the players cursor
//
//	NOTE: THE CURSOR TEXTURES are ASSUMED TO BE LOADED INTO THE TMU
//
//   INPUT: cursor obj, menu level and cursor position
// RETURNS: nothing
//-------------------------------------------------------------------------------------------------
//void update_cursor_imgs(sprite_info_t *cur_obj, int level, int c_pos, int tid)
//{
//	image_info_t	**tbl_ptr;
//
//	// get ptr. to current cursor image table
//	tbl_ptr = cursor_tbl_ptrs[level];
//
//	change_img_tmu(cur_obj, tbl_ptr[c_pos], tid);
//}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//void heeflash_yes_no(sprite_info_t *cur_obj, float flsh_x, float flsh_y, int pnum, short answer)
//{
//	int						i;
//
//	if (answer == YES)
//	{
//		snd_scall_bank(cursor_bnk_str,SEL_YESNO_SND,VOLUME4,127,PRIORITY_LVL2);
//		for (i=0; i < 11; i++)
//		{
//			change_img_tmu(cur_obj, yes_flsh_imgs[four_plr_ver][pnum][i], YES_NO_PLAQ_TID);
//			sleep(3);
//		}
//	}
//	else
//	{
//		snd_scall_bank(cursor_bnk_str,SEL_YESNO_SND,VOLUME4,127,PRIORITY_LVL2);
//		for (i=0; i < 11; i++)
//		{
//			change_img_tmu(cur_obj, no_flsh_imgs[four_plr_ver][pnum][i], YES_NO_PLAQ_TID);
//			sleep(3);
//		}
//	}
//
//}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//void dim_text(int *args)
//{
//int	i,j,tm_num;
//
//	tm_num = args[0];
//
//	// Make rosters more yellow	in color
////	for (i=TM_QB_NME_OBJ_PTR,j=0; i <= TM_RNME9_OBJ_PTR; i++,j++)
////		tm_selct_img_ptrs[i][tm_num]->state.constant_color = 0xffFFFF00;
//
//	// change the text to a constant color ... so I can fade it down to a darker (dimmer) color.
//	for (i=TM_NME1_OBJ_PTR; i <= TM_NME3_OBJ_PTR; i++)
//	{
//		tm_selct_img_ptrs[i][tm_num]->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
//		tm_selct_img_ptrs[i][tm_num]->state.constant_color = 0xffB0B0B0;
//	}
//	for (i=TM_NME5_OBJ_PTR; i <= TM_NME7_OBJ_PTR; i++)
//	{
//		tm_selct_img_ptrs[i][tm_num]->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
//		tm_selct_img_ptrs[i][tm_num]->state.constant_color = 0xffB0B0B0;
//	}
//
//	for (j=0; j < 20; j++)
//	{
//		// dim top four city names
//		for (i=TM_NME1_OBJ_PTR; i <= TM_NME3_OBJ_PTR; i++)
//			tm_selct_img_ptrs[i][tm_num]->state.constant_color -= 0x00020202;
//
//		// dim bottom four city names
//		for (i=TM_NME5_OBJ_PTR; i <= TM_NME7_OBJ_PTR; i++)
//			tm_selct_img_ptrs[i][tm_num]->state.constant_color -= 0x00020202;
//
//		sleep (2);
//	}
//}

//-------------------------------------------------------------------------------------------------
//						This routine controls the in-between period buying and removing of credits
//
//
// RETURNS: 1 = game over, else 0
//-------------------------------------------------------------------------------------------------
int buyin_screen(void)
{
	register int	i, mode;
	int				status;
	int				score[2];
	int				free_quarters = 0;
	sprite_info_t 	*psprite;

	// Turn on start buttons
	start_enable(TRUE);

	// If any player was in the game and is now out and is ahead (score)
	// then award that player another quarter and put him back in the game.
	// This code handles quarters 1 through 3.  If the score is tied at the
	// end of the 4th quarter or overtime quarters 1 and/or 2 we end up here,
	// but because the score is tied, the player MUST buy the next quarter.

	// Are we awarding free quarters to the player that is ahead ?
	while (1)
	{
		// Chk HH vs HH - game must be humans-only to qualify
		if (!get_adjustment(H4_FREE_QUARTER_ADJ, &status))
			if (qtr_pstatus[0] == qtr_pstatus[2] &&
				qtr_pstatus[0] == qtr_pstatus[1] &&
				qtr_pstatus[2] == qtr_pstatus[3])
				break;

		// Chk H vs H - game must be same H vs H to qualify
		if (!get_adjustment(HH_FREE_QUARTER_ADJ, &status))
		{
			if (four_plr_ver)
			{
				if (((qtr_pstatus[0] + qtr_pstatus[1]) ==
					 (qtr_pstatus[2] + qtr_pstatus[3])) &&
					(!qtr_pstatus[0] || !qtr_pstatus[1]) &&
					(!qtr_pstatus[2] || !qtr_pstatus[3]))
					break;
			}
			else if (qtr_pstatus[0] == qtr_pstatus[2])
				break;
		}

		// Chk H vs CPU - game must be one human team to qualify
		if (!get_adjustment(HC_FREE_QUARTER_ADJ, &status))
		{
			if (four_plr_ver)
			{
				if ((qtr_pstatus[0] == qtr_pstatus[1] &&
						!(qtr_pstatus[2] + qtr_pstatus[3])) ||
					(qtr_pstatus[2] == qtr_pstatus[3] &&
						!(qtr_pstatus[0] + qtr_pstatus[1])))
					break;

				// The Europe Rules
				if (((!qtr_pstatus[0] || !qtr_pstatus[1]) &&
						!(qtr_pstatus[2] + qtr_pstatus[3])) ||
					((!qtr_pstatus[2] || !qtr_pstatus[3]) &&
						!(qtr_pstatus[0] + qtr_pstatus[1])))
					break;
			}
			else if ((p_status2 == 1 && !qtr_pstatus[2]) ||
					(p_status2 == 4 && !qtr_pstatus[0]))
				break;
		}

		// Nothing to chk
		status = 0;
		break;
	}

	// Do the chk if set
	if (status)
	{
		score[0] = compute_score(0);
		score[1] = compute_score(1);

//		for (mode = 0, i = 0; i < ((four_plr_ver) ? MAX_PLYRS : 2); i++)
		for (mode = 0, i = 0; i < MAX_PLYRS; i++)
		{
//			if (i == ((four_plr_ver) ? (MAX_PLYRS/2) : (2/2)))
			if (i == (MAX_PLYRS/2))
				mode = 1;

			// Was this player in and now out ?
			if ((p_status ^ p_status2) & (1 << i))
			{
				// YES - Is this player ahead ?
				if (score[mode] > score[mode ^ 1])
				{
					// YES - Give him another quarter and put him back in
					// the game
					pdata_blocks[i].quarters_purchased++;
					if(pdata_blocks[i].quarters_purchased > 4)
					{
						pdata_blocks[i].quarters_purchased = 1;
					}

					// Put player in game
					p_status |= (1 << i);

					// Chalk a free quarter audit
					increment_audit(FREE_QUARTERS_AUD);

					// Mark player that got free quarter
					free_quarters |= (1 << i);
				}
			}
		}
	}


	snd_stop_all();
	snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);
	snd_scall_bank(buyin_bnk_str, 0, VOLUME6, 127, SND_PRI_SET|SND_PRI_TRACK0);


	// If either player/team is out - give 'em a chance to buy in
	if (((p_status != 5) && !(four_plr_ver)) ||
		((p_status != 15) && (four_plr_ver)))
	{
		// load end-of-quarter images
//		if (load_textures_into_tmu(eoq_textures_tbl))
//		{
//			fprintf(stderr, "Couldn't load all EOQ textures into tmu\r\n");
//#ifdef DEBUG
//			lockup();
//#else
//			return(0);
//#endif
//		}
//
//		lock_multiple_textures(JAPPLE_BOX_TID,0xFFFFFFFF);

//		// If halftime, we let halftime music from stats page continue over buyin page
//		if (game_info.game_quarter != 2)
//		{
//			// Stop coaching tips/halftime music
//			snd_stop_track(SND_TRACK_0);
//			// reset track allocations
//			snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);
//			// Play buyin groove
//			snd_scall_bank(eoq_tune_bnk_str,0,VOLUME4,127,SND_PRI_SET|SND_PRI_TRACK1);
//		}


		if (which_court == COURT_GILMORE)
		{
			psprite = beginobj(&prkbuyin,			0.0F, SPRITE_VRES, 170.0F, EOQ_TID);
			psprite->id = 1;
			psprite = beginobj(&prkbuyin_c1,  0.0F, SPRITE_VRES, 170.0F, EOQ_TID);
			psprite->id = 1;
			psprite = beginobj(&prkbuyin_c2,  0.0F, SPRITE_VRES, 170.0F, EOQ_TID);
			psprite->id = 1;
			psprite = beginobj(&prkbuyin_c3,  0.0F, SPRITE_VRES, 170.0F, EOQ_TID);
			psprite->id = 1;
		} else if (which_court == COURT_GEER) {
			psprite = beginobj(&islbuyin,			0.0F, SPRITE_VRES, 170.0F, EOQ_TID);
			psprite->id = 1;
			psprite->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
			psprite = beginobj(&islbuyin_c1,  0.0F, SPRITE_VRES, 170.0F, EOQ_TID);
			psprite->id = 1;
			psprite->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
			psprite = beginobj(&islbuyin_c2,  0.0F, SPRITE_VRES, 170.0F, EOQ_TID);
			psprite->id = 1;
			psprite->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
			psprite = beginobj(&islbuyin_c3,  0.0F, SPRITE_VRES, 170.0F, EOQ_TID);
			psprite->id = 1;
			psprite->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
		} else {	
			psprite = beginobj(&buyinbg,  	0.0F, SPRITE_VRES, 170.0F, EOQ_TID);
			psprite->id = 1;
			psprite = beginobj(&buyinbg_c1,   0.0F, SPRITE_VRES, 170.0F, EOQ_TID);
			psprite->id = 1;
			psprite = beginobj(&buyinbg_c2,   0.0F, SPRITE_VRES, 170.0F, EOQ_TID);
			psprite->id = 1;
			psprite = beginobj(&buyinbg_c3,   0.0F, SPRITE_VRES, 170.0F, EOQ_TID);
			psprite->id = 1;
		}

		// credit string on bottom of screen
		qcreate("cred_msg",CREDIT_PID,credit_msg,0,JAPPLE_BOX_TID,0,0);

		// japple boxes and msg...instead them
		qcreate("crdbox1",CREDIT_PID,plyr_credit_box,0,station_xys[0][four_plr_ver][X_VAL],station_xys[0][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
		qcreate("crdbox3",CREDIT_PID,plyr_credit_box,2,station_xys[2][four_plr_ver][X_VAL],station_xys[2][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
		qcreate("p1chlngr",CREDIT_PID,challenger_needed_msg,PLYR_1,JAPPLE_BOX_TID,free_quarters,0);
		qcreate("p3chlngr",CREDIT_PID,challenger_needed_msg,PLYR_3,JAPPLE_BOX_TID,free_quarters,0);

		if (four_plr_ver)
		{
			qcreate("crdbox2",CREDIT_PID,plyr_credit_box,1,station_xys[1][four_plr_ver][X_VAL],station_xys[1][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
			qcreate("crdbox4",CREDIT_PID,plyr_credit_box,3,station_xys[3][four_plr_ver][X_VAL],station_xys[3][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
			qcreate("p2chlngr",CREDIT_PID,challenger_needed_msg,PLYR_2,JAPPLE_BOX_TID,free_quarters,0);
			qcreate("p4chlngr",CREDIT_PID,challenger_needed_msg,PLYR_4,JAPPLE_BOX_TID,free_quarters,0);
		}

		// Announce "get ready for x period at top of buy in screen" - unless we need to show free quarter message
//		if (free_quarters == 0)
//			get_ready_for();		

		reset_on_coin = 1;
		qcreate("timer",TIMER_PID,timedown_buyin,0,9,SPRITE_HRES/2.0f,90.0F);	//wait 1 second then cnt from 9

		// print names in japple boxes
//		if (player1_data.plyr_name[0] != NULL)
//			print_players_name(0, japbx_cntr_xys[0][four_plr_ver][X_VAL], japbx_cntr_xys[0][four_plr_ver][Y_VAL], player1_data.plyr_name, 0);
//		if (player2_data.plyr_name[0] != NULL)
//			print_players_name(1, japbx_cntr_xys[1][four_plr_ver][X_VAL], japbx_cntr_xys[1][four_plr_ver][Y_VAL], player2_data.plyr_name, 0);

//		if (four_plr_ver)
//		{
//			if (player3_data.plyr_name[0] != NULL)
//				print_players_name(2, japbx_cntr_xys[2][four_plr_ver][X_VAL], japbx_cntr_xys[2][four_plr_ver][Y_VAL], player3_data.plyr_name, 0);
//			if (player4_data.plyr_name[0] != NULL)
//				print_players_name(3, japbx_cntr_xys[3][four_plr_ver][X_VAL], japbx_cntr_xys[3][four_plr_ver][Y_VAL], player4_data.plyr_name, 0);
//		}

		// create the player models here...all 4 of them
		qcreate("plr1", BUYIN_PLAYER_PID, simplified_player_proc, 0, 0, 0, 0);
		qcreate("plr3", BUYIN_PLAYER_PID, simplified_player_proc, 2, 1, 0, 0);
		if (four_plr_ver)
		{
			qcreate("plr2", BUYIN_PLAYER_PID, simplified_player_proc, 1, 0, 0, 0);
			qcreate("plr4", BUYIN_PLAYER_PID, simplified_player_proc, 3, 1, 0, 0);
		}

		turn_transit_off(1);

		// Make em see it for at least 2 seconds
//		sleep(120);

		do
		{
			sleep(1);
		}	while(!force_selection);

		// kill processes
		killall(BONUS_PID, -1);
		killall(TIMER_PID,-1);										//	kill_countdown_timer();
		killall(CREDIT_PID,-1);										//	kill_countdown_timer();
		killall(BUYIN_PLAYER_PID,-1);										//	kill_countdown_timer();
		
		turn_transit_on();

		// Stop buyin music
//		snd_stop_track(SND_TRACK_1);
//		snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);

		// Play drum loop under transition plates
//		snd_scall_bank(generic_bnk_str, 16, VOLUME2, 127, SND_PRI_SET|SND_PRI_TRACK0);

		// Do sound only if the transition finished, not the time-out,
		//  and the game didn't end
//		if (i) snd_scall_bank(plyr_bnk_str, LOCK_SND, VOLUME4, 127, PRIORITY_LVL5);

		// delete objects and strings
		delete_multiple_strings(CREDIT_MSG_ID, -1);
		delete_multiple_strings(CREDIT_MSG_ID + 1, -1);
		delete_multiple_strings(TIMER_MSG_ID, -1);
		// delete the models
		delete_multiple_objects(OID_BUYIN_PLYR, -1);

		del1c(OID_MESSAGE,-1);	 				// Result box underneath "get ready for" message
		del1c(1,-1);			 				// delete background
		del1c(OID_JAPPLE_BOX,-1);	   			// all japple box stuff

		// if all players dropped out...then end game
//printf("p_status %d\n",p_status);
		if (!p_status)
		{
			start_enable(FALSE);
			reset_on_coin = 0;
			return(1);
		}

//		load_ingame_textures();

		start_enable(FALSE);
		reset_on_coin = 0;
		return(0);
	}

//#if DEBUG
//	printf("Restoring ingame textures after coaching tips screen only\n");
//#endif

	// Delete japple box stuff.. and backgrounds
  	unlock_multiple_textures(JAPPLE_BOX_TID,-1);
	delete_multiple_textures(JAPPLE_BOX_TID,-1);
	delete_multiple_textures(EOQ_TID,-1);

	start_enable(FALSE);
	reset_on_coin = 0;
	return(0);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void coaching_tip_scrn(void)
{
	coach_msg_t * phint;
	process_node_t	*flash = NULL;
	sprite_info_t *obj;
	ostring_t * f_msg;
	float ypos = 0;
	int i, x, delta;
	int	was_low_res = is_low_res;



	static coach_msg_t hint1_txt[] =
	{
		{"hook shots",-50},
		{"when you're running near and",-20},
		{"perpendicular to the hoop,",-20},
		{"press the shoot button.",-20},
		{" ",-20},
		{"pressing turbo and shoot will give",-20}, 
		{"the ball a higher arc!",0}
	};

	static coach_msg_t hint2_txt[] =
	{
		{"super dunks",-50}, 
		{"press the turbo and shoot buttons",-20}, 
		{"while running toward the basket.",-20},
		{" ",-20}, 
		{"remember, some players have more",-20}, 
		{"spectacular dunks than others!",0}  
	};

	static coach_msg_t hint3_txt[] =
	{
		{"head fakes",-50}, 
		{"one quick tap to the shoot button",-20},
		{"makes your player do a head fake.",-20},
		{"remember, this also causes you to",-20},
		{"pick up your dribble.",-20}, 
		{" ",-20}, 
		{"try faking out cpu drones!",0}  
	};

	static coach_msg_t hint4_txt[] =
	{
		{"jump shot pass",-50}, 
		{"press and hold the shoot button,",-20}, 
		{"then press the pass button.  this",-20},
		{"allows you to dish off to your",-20},
		{"teammate.",-20}, 
		{" ",-20}, 
		{"this is most effective when you are",-20}, 
		{"about to be rejected!",0}  
	};

	static coach_msg_t hint5_txt[] =
	{
		{"accurate jump shots",-50}, 
		{"increase the chances of a jump",-20}, 
		{"shot going in by releasing the shoot",-20}, 
		{"button at the peak of your jump.",-20}, 
		{" ",-20}, 
		{"avoid being rejected by holding on",-20}, 
		{"to the ball until the defender is",-20}, 
		{"out of position.",0}  
	};

	static coach_msg_t hint6_txt[] =
	{
		{"juke move",-50}, 
		{"when running, quickly tap the",-20}, 
		{"turbo button twice. this will cause",-20}, 
		{"you to juke around your opponent.",-20}, 
		{" ",-20}, 
		{"while spinning you're less likely to",-20}, 
		{"get cleared out by an opponent!",0}  
	};

	static coach_msg_t hint7_txt[] =
	{
		{"clearing out",-50}, 
		{"when facing your opponent, press",-20}, 
		{"the turbo and pass buttons at the",-20}, 
		{"same time. this will clear him out!",-20}, 
		{" ",-20}, 
		{"remember, small players can't",-20}, 
		{"clear out big players!",0}  
	};

	static coach_msg_t hint8_txt[] =
	{
		{"protect the ball",-50}, 
		{"when standing in position with",-20}, 
		{"the ball, quickly tap the turbo",-20},
		{"button. this will cause the player",-20}, 
		{"to protect the ball!",0}  
	};

	static coach_msg_t hint9_txt[] =
	{
		{"turbo pass",-50}, 
		{"press the turbo and pass buttons at",-20}, 
		{"the same time for a fast pass.",-20}, 
		{" ",-20}, 
		{"turbo passes are less likely to be",-20}, 
		{"intercepted by the defense!",0}  
	};

	static coach_msg_t hint10_txt[] =
	{
		{"super jumps",-50}, 
		{"jump extra high when shooting,",-20}, 
		{"blocking, or rebounding by pressing",-20}, 
		{"the turbo and shoot buttons at the",-20}, 
		{"same time!",0}  
	};

	static coach_msg_t hint11_txt[] =
	{
		{"dunk pass",-50}, 
		{"to avoid having your dunk blocked,",-20}, 
		{"pass off to your teammate.",-20}, 
		{" ",-20}, 
		{"this is most effective when your",-20}, 
		{"teammate is open for a three point",-20}, 
		{"shot!",0}  
	};

	static coach_msg_t hint12_txt[] =
	{
		{"layups",-50}, 
		{"when running at the hoop, quickly",-20}, 
		{"press the turbo and shoot buttons!",-20}, 
		{" ",-20}, 
		{"holding the shoot button delays",-20}, 
		{"the release of the ball. use this",-20}, 
		{"to fake out your opponent!",0}  
	};

	static coach_msg_t hint13_txt[] =
	{
		{"hotspots",-50},
		{"each player has a secret hotspot!",-20}, 
		{" ",-20}, 
		{"the second successful shot from",-20}, 
		{"a player's hotspot will activate",-20}, 
		{"the hotspot for the remainder of",-20}, 
		{"the game.",-20}, 
		{" ",-20}, 
		{"shooting from a hotspot greatly",-20}, 
		{"increases the shot percentage!",0} 
	};

	static coach_msg_t hint14_txt[] =
	{
		{"lean-out jumper",-50}, 
		{"if you are facing straight up or",-20}, 
		{"down, pull the joystick to the left",-20}, 
		{"or right!",-20}, 
		{" ",-20}, 
		{"holding the shoot button delays the",-20}, 
		{"release of the ball!",0}  
	};

	static coach_msg_t hint15_txt[] =
	{
		{"hot streak",-50}, 
		{"when a player scores 3 consecutive",-20}, 
		{"baskets, he is on fire!",-20}, 
		{" ",-20}, 
		{"when a player goes on a hot streak,",-20}, 
		{"give him the ball!  he will remain",-20}, 
		{"hot for several shots or until the",-20},
		{"other team scores.",0} 
	};

	static coach_msg_t hint16_txt[] =
	{
		{"double-dunks",-50}, 
		{"when a player is in a dunk, his",-20}, 
		{"teammate can also start a dunk.",-20}, 
		{"the player with the ball can then",-20}, 
		{"press the pass button to lob the",-20}, 
		{"ball to his teammate.",-20}, 
		{"",-20}, 
		{"try for 3 in a row!",0}  
	};

	static coach_msg_t hint17_txt[] =
	{
		{"drone control",-50}, 
		{"cause a teammate controlled by the",-20}, 
		{"cpu to shoot or pass by pressing",-20}, 
		{"your own shoot or pass buttons.",-20}, 
		{" ",-20}, 
		{"pass to your drone when he is under",-20}, 
		{"the hoop for a quick dunk.",0} 
	};  

	static coach_msg_t hint18_txt[] =
	{
		{"fade-away jumper",-50}, 
		{"just as you jump up for a shot, pull",-20}, 
		{"the joystick in the opposite direction",-20}, 
		{"your player is facing.",-20}, 
		{" ",-20}, 
		{"holding the shoot button delays the",-20}, 
		{"release of the ball!",0}  
	};  

	static coach_msg_t hint19_txt[] =
	{
		{"player attributes",-50}, 
		{"for maximum success, pay attention",-20}, 
		{"to your player's strengths and",-20}, 
		{"weaknesses.  play big guys near the",-20}, 
		{"hoop, and small guys outside.",-20}, 
		{" ",-20}, 
		{"big guys are poor 3 point shooters!",0}  
	};  

	static coach_msg_t hint20_txt[] =
	{
		{"power attribute",-50}, 
		{"the greater a player's power rating,",-20}, 
		{"the harder it is to clear him out",-20}, 
		{"or to block his dunks.  try to avoid",-20}, 
		{"size mismatches. ",-20}, 
		{" ",-20}, 
		{"powerful guys tend to hold on to",-20}, 
		{"the ball when cleared out!",0}  
	};

	static coach_msg_t hint21_txt[] =
	{
		{"alley-oop play",-50}, 
		{"when your teammate has the ball,",-20}, 
		{"drive the open lane toward the hoop",-20}, 
		{"and press turbo and shoot.",-20}, 
		{"once airborne, your teammate must",-20}, 
		{"quickly pass you the ball!",-20}, 
		{"",-20}, 
		{"try for 3 in a row!",0}  
	};

	static coach_msg_t hint22_txt[] =
	{
		{"teamwork",-50}, 
		{"a combination of 3 consecutive",-20}, 
		{"alley-oops or double-dunks will",-20}, 
		{"put your team in the zone!",-20}, 
		{" ",-20}, 
		{"don't be a ball hog!",0} 
	};
  
	static coach_msg_t hint23_txt[] =
	{
		{"steal attribute",-50}, 
		{"the greater a player's steal rating,",-20}, 
		{"the more effective his swiping",-20}, 
		{"becomes. he will also intercept",-20}, 
		{"more passes!",-20}, 
		{" ",-20}, 
		{"smaller, quicker players are very",-20}, 
		{"skilled at stealing the ball.",0}  
	};

	static coach_msg_t hint24_txt[] =
	{
		{"dunk attribute",-50}, 
		{"the greater a player's dunk rating,",-20}, 
		{"the more spectacular his dunks. a",-20}, 
		{"player with a one dunk rating will",-20}, 
		{"perform just layups.",-20}, 
		{" ",-20}, 
		{"layups are generally less",-20},
		{"successful than dunks.",0}  
	};

	static coach_msg_t hint25_txt[] =
	{
		{"speed attribute",-50}, 
		{"the greater a player's speed rating,",-20}, 
		{"the faster he moves.  usually, the",-20}, 
		{"faster players are less powerful",-20}, 
		{"and therefor get cleared out more.",-20}, 
		{" ",-20}, 
		{"match speed against speed.",0} 
	};

	static coach_msg_t hint26_txt[] =
	{
		{"create player",-50}, 
		{"at the start of a game, choose",-20}, 
		{"the 'create player' option and",-20}, 
		{"customize a player to fit your",-20}, 
		{"individual playing style.",-20}, 
		{" ",-20}, 
		{"game wins can earn a created",-20}, 
		{"player extra attribute points!",0}  
	};

	static coach_msg_t hint27_txt[] =
	{
		{"power-ups",-50}, 
		{"on the versus screen, press turbo,",-20}, 
		{"shoot, and pass to select an icon",-20}, 
		{"combination. to enter the combo,",-20}, 
		{"move the stick up, down, left, or",-20}, 
		{"right. some power-ups require both",-20}, 
		{"teams to enter the same combination!",-20}, 
		{"",-20},
		{"enter '111 down' to play in",-20},
		{"tournament mode!",0}
	};

	static coach_msg_t hint28_txt[] =
	{
		{"lean-in jumper",-50}, 
		{"just as you jump up for a shot, push",-20}, 
		{"the joystick in the direction your",-20}, 
		{"player is facing!",-20}, 
		{" ",-20}, 
		{"holding the shoot button delays the",-20}, 
		{"release of the ball!",0} 
	};    

	static coach_msg_t hint29_txt[] =
	{
		{"smart turbo useage",-50}, 
		{"the amount of turbo remaining is",-20}, 
		{"displayed at the top of the screen.",-20}, 
		{" ",-20}, 
		{"all turbo actions deplete this",-20}, 
		{"amount, so conserve just enough for",-20}, 
		{"that all important next move!",0}  
	};  

	static coach_msg_t hint30_txt[] =
	{
		{"substitutions",-50}, 
		{"some players may become fatigued",-20}, 
		{"during the second half!  try",-20}, 
		{"substituting those players with",-20}, 
		{"fresh players at halftime.",-20}, 
		{" ",0}  
	};      
  
	static coach_msg_t hint31_txt[] =
	{
		{"burst of speed",-50}, 
		{"avoid pesky defenders and increase",-20}, 
		{"the speed of your player by holding",-20}, 
		{"down the turbo button.",0}  
	}; 

	static coach_msg_t hint32_txt[] =
    {
		{"court awareness",-50}, 
		{"locate a player's position when he",-20}, 
		{"is off screen by watching for his",-20}, 
		{"arrow.  try to anticipate his moves.",-20}, 
		{" ",-20}, 
		{"even when off screen, try to stay",-20}, 
		{"away from opposing players!",0} 
	};  

	static coach_msg_t hint33_txt[] =
	{
		{"smart play",-50}, 
		{"an open player is more likely to",-20}, 
		{"make a basket, so pass to your open",-20}, 
		{"teammate whenever possible!",-20}, 
		{" ",-20}, 
		{"try to anticipate your opponent's",-20}, 
		{"next move before he makes it!",0} 
	};  

	static coach_msg_t hint34_txt[] =
	{
		{"secret power-ups",-50}, 
		{"the players can be powered-up with",-20}, 
		{"super human abilities.  try special",-20}, 
		{"joystick and button combinations",-20}, 
		{"during the versus screen!",0}
	};

	static coach_msg_t hint35_txt[] =
	{
		{"strong defense",-50}, 
		{"you can decrease the chances of",-20}, 
		{"an opponent's shot going in by",-20}, 
		{"getting real close to him!",-20}, 
		{" ",-20}, 
		{"clear him out, steal the ball!",0} 
	};      
  
	static coach_msg_t hint36_txt[] =
	{
		{"hidden passing attribute",-50}, 
		{"the greater a player's hidden pass",-20}, 
		{ "rating, the faster he passes.",-20}, 
		{" ",-20}, 
		{"it is more difficult to intercept a",-20}, 
		{"quick pass than a slow pass.  you",-20}, 
		{"should always try a turbo pass.",0} 
	};  

	static coach_msg_t hint37_txt[] =
	{
		{"blocking attribute",-50}, 
		{"the greater a player's block",-20}, 
		{"rating, the better he is at blocking",-20}, 
		{"dunks and regular jump shots.",-20}, 
		{" ",-20}, 
		{"it is also harder for someone to",-20}, 
		{"drive by him for a dunk.",0} 
	};    

	static coach_msg_t hint38_txt[] =
	{
		{"hidden clutch attribute",-50}, 
		{"the greater a player's hidden",-20}, 
		{"clutch rating, the more chance",-20}, 
		{"he will make a great play in the",-20},
		{"final seconds of any period!",0}, 
	}; 

	static coach_msg_t hint39_txt[] =   
	{
		{"stats pages",-50}, 
		{"on the stats page, the steals",-20}, 
		{"category includes slap-aways and",-20}, 
		{"clear-outs of ball handlers.",-20}, 
		{" ",-20}, 
		{"the injured category represents how",-20}, 
		{"many times you were cleared out!",0} 
	};    

	static coach_msg_t hint40_txt[] =
	{
		{"layup out of dunks",-50}, 
		{"when a player is in a dunk, he can",-20}, 
		{"press the shoot button to lob the",-20}, 
		{"the ball toward the hoop.",-20}, 
		{"",-20}, 
		{"this is most effective if your dunk",-20}, 
		{"is about to get blocked.",0} 
	};  

	static coach_msg_t hint41_txt[] =
	{
		{"on demand layups",-50}, 
		{"run toward the basket without",-20}, 
		{"holding down the turbo button.",-20}, 
		{"then press the turbo and shoot",-20}, 
		{"buttons.  release shoot button",-20}, 
		{"to layup the ball.",0} 
	};      
  
	static coach_msg_t hint42_txt[] =
	{
		{"stronger created player",-50}, 
		{"as a created player wins more",-20}, 
		{"games, he is awarded additional",-20}, 
		{"attribute points.  as you increase",-20}, 
		{"your players strength, hidden",-20}, 
		{"features will become accessible",-20}, 
		{"for that player!",0}  
	};  

	static coach_msg_t hint43_txt[] =
	{
		{"shot swatting",-50}, 
		{"when swatting at a jump shot, hold",-20}, 
		{"the block button down until your",-20}, 
		{"opponent releases the ball, then",-20}, 
		{"release the block button to follow",-20}, 
		{"through with the swat.",0}  
	};      
   
	static coach_msg_t hint44_txt[] =
	{
		{"hidden rebound attribute",-50}, 
		{"the greater a player's hidden",-20}, 
		{"rebound rating, the more chance",-20}, 
		{"that he will snag the rebound",-20}, 
		{"from a crowd of players. he",-20}, 
		{"will also jump higher when",-20},
		{"trying to rebound.",0} 
	}; 
  
	static coach_msg_t hint45_txt[] =
	{
		{"successful rebounding",-50}, 
		{"wait until the ball has hit the rim",-20}, 
		{"before pressing the block button.",-20}, 
		{"",-20}, 
		{"auto rebounding may also occur",-20}, 
		{"if you are near the basket for a",-20},
		{"missed shot.",0},
	};     
  
	static coach_msg_t hint46_txt[] =
	{
		{"speeding up play",-50}, 
		{"players can usually advance through",-20}, 
		{"non-game screens or sequences by",-20}, 
		{"pushing one or more of their action",-20}, 
		{"buttons.",-20}, 
		{"",-20}, 
		{"be sure to try various button and",-20},
		{"stick combos on the versus screen",-20},
		{"to find cool power ups!",0},
	};     
  


	static coach_msg_t *hint_list[] =
	{
		hint1_txt,
		hint2_txt,
		hint3_txt,
		hint4_txt,
		hint5_txt,
		hint6_txt,
		hint7_txt,
		hint8_txt,
		hint9_txt,
		hint10_txt,
		hint11_txt,
		hint12_txt,
		hint13_txt,
		hint14_txt,
		hint15_txt,
		hint16_txt,
		hint17_txt,
		hint18_txt,
		hint19_txt,
		hint20_txt,
		hint21_txt,
		hint22_txt,
		hint23_txt,
		hint24_txt,
		hint25_txt,
		hint26_txt,
		hint27_txt,
		hint28_txt,
		hint29_txt,
		hint30_txt,
		hint31_txt,
		hint32_txt,
		hint33_txt,
		hint34_txt,
		hint35_txt,
		hint36_txt,
		hint37_txt,
		hint38_txt,
		hint39_txt,
		hint40_txt,
		hint41_txt,
		hint42_txt,
		hint43_txt,
		hint44_txt,
		hint45_txt,
		hint46_txt
	};

	normal_screen();


	// load end-of-quarter images

//	TEXTURE_DEBUG_LEVEL(DEBUG_RETURNS);

//	if (which_court == COURT_GILMORE) {
//		if (load_textures_into_tmu(eoq_textures_tbl2)) {
//	#ifdef DEBUG
//			fprintf(stderr, "Couldn't load all EOQ textures 2 into tmu\r\n");
//			lockup();
//	#else
//			return;
//	#endif
//		}
//	} else if (which_court == COURT_GEER) {
//		 if (load_textures_into_tmu(eoq_textures_tbl3)) {
//	#ifdef DEBUG
//			fprintf(stderr, "Couldn't load all EOQ textures into tmu\r\n");
//			lockup();
//	#else
//			return;
//	#endif
//		} 
//	} else {
//		 if (load_textures_into_tmu(eoq_textures_tbl)) {
//	#ifdef DEBUG
//			fprintf(stderr, "Couldn't load all EOQ textures into tmu\r\n");
//			lockup();
//	#else
//			return;
//	#endif
//		} 
//	}	

//	lock_multiple_textures(JAPPLE_BOX_TID,0xFFFFFFFF);

	// draw backdround
	obj = beginobj(&chtpsscr,		0.0F, SPRITE_VRES, 100.0F, EOQ_TID);
	obj->id = 50;
	obj = beginobj(&chtpsscr_c1,	0.0F, SPRITE_VRES, 100.0F, EOQ_TID);
	obj->id = 50;
	obj = beginobj(&chtpsscr_c2,	0.0F, SPRITE_VRES, 100.0F, EOQ_TID);
	obj->id = 50;
	obj = beginobj(&chtpsscr_c3,	0.0F, SPRITE_VRES, 100.0F, EOQ_TID);
	obj->id = 50;


	// For testing all of the hints
#if 0
	for (x=0; x < (sizeof(hint_list)/sizeof(void *)); x++)
	{
		phint = hint_list[x];
#else
		phint = hint_list[randrng(sizeof(hint_list)/sizeof(void *))];		// want to be rndper
#endif
		ypos = SPRITE_VRES - 150 + (is_low_res ? 20.0f : 20.0f);

		set_string_id(5);
		set_text_z_inc(-0.01f);

		i = 0;
		do {
			//set_text_position(SPRITE_HRES/2.0f, ypos, 1.1F);
			set_text_position(SPRITE_HRES/2.0f, ypos + (is_low_res ? 2 : 0), 40.1F);
			if (!i)
			{
				set_text_color(LT_RED);
				if (is_low_res)
					set_text_font(FONTID_NBA14);
				else	
#ifdef USE_CHATTEN_FONT
					set_text_font(FONTID_CHATTEN20);
#else
					set_text_font(FONTID_NBA20);
#endif
				oprintf("%dj%s", (HJUST_CENTER|VJUST_BOTTOM), phint->msg);

//				f_msg = oprintf("%dj%s", (HJUST_CENTER|VJUST_BOTTOM), phint->msg);
//				flash = qcreate("flash", MESSAGE_PID, flash_text_slow_proc, (int)f_msg, 0, 0, 0);

				set_text_color(WHITE);
				if (is_low_res)
					set_text_font(FONTID_NBA12);
				else	
#ifdef USE_CHATTEN_FONT
					set_text_font(FONTID_CHATTEN14);
#else
					set_text_font(FONTID_NBA14);
#endif
				ypos += 16;
			}
			else
			{
				is_low_res = 0;
				oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_TOP), WHITE , phint[i].msg);
				is_low_res = was_low_res;
			}
			if (is_low_res) {
				if ((i == 7) && ((phint == hint13_txt) || (phint == hint27_txt)))
					delta = phint[i].yoff;
				else {
					if (i == 0) {
						if ((phint == hint13_txt) || (phint == hint27_txt) || (phint == hint46_txt))
							delta = 67;
						else
							delta = 68;
					} else {
						delta = -3;
					}
				}
			} else
				delta = 0;
			ypos += phint[i].yoff - delta;
		}	while (phint[i++].yoff);

		set_text_z_inc( 0.00f);

	// Take away the plates
	turn_transit_off(1);


#ifndef NOAUDIO
	coin_sound_holdoff(TRUE);

	snd_bank_delete(game_tune_bnk_str);

	// If game isn't over, load next quarter game tune
	if ((unsigned int)game_info.game_quarter < (unsigned int)MAX_QUARTERS)
	{


		if (COURT_IS_OUTDOORS)
			strcpy(game_tune_bnk_str, game_outdoor_tunes_tbl[game_info.game_quarter]);
		else
			strcpy(game_tune_bnk_str, game_tunes_tbl[game_info.game_quarter]);


		if (snd_bank_load_playing(game_tune_bnk_str) == 0xeeee)
			increment_audit(SND_LOAD_FAIL_AUD);
	}

	coin_sound_holdoff(FALSE);
#endif


	wait_for_any_butn_press(10,tsec*7);

	// Turn on the plates
	turn_transit_on();

	// do clean-up
	kill(flash, 0);
	delete_multiple_strings(5, -1);
#if 0
	}
#endif
	del1c(50,-1);							// delete background and header
	delete_multiple_textures(EOQ_TID,-1);
}

//-------------------------------------------------------------------------------------------------------------------------
//							This process displays some effect when the player buys a full game.
//
//   INPUT: player number
// RETURNS: Nothing
//-------------------------------------------------------------------------------------------------------------------------
void fullgame_bought_msg(int *args)
{
	sprite_info_t	*fg_plq_obj;
	sprite_info_t	*fg_msg_obj;
	sprite_info_t	*flsh_obj;
	float			angle;
	int				i,p_num;

	while (!fullgame_bought_ok)
		sleep(1);

	p_num = args[0];
	angle = 60.0f;
//	snd_scall_bank(cursor_bnk_str,FULLGM_APPEAR_SND,VOLUME3,127,PRIORITY_LVL3);

	// fullgame plaque
	fg_plq_obj = beginobj(jbox_plaq_imgs[four_plr_ver][p_num],
		station_xys[p_num][four_plr_ver][X_VAL]-4.0f,
		station_xys[p_num][four_plr_ver][Y_VAL]-140.0F,
		1.35f - 0.005f * (float)p_num,
		JAPPLE_BOX_TID);
	fg_plq_obj->w_scale = 2.2f;
	fg_plq_obj->h_scale = 2.2f;
//	fg_plq_obj->y_angle = DG2RD(angle);
	fg_plq_obj->id = fullgame_bought_ids[p_num];

	// fullgame message
	fg_msg_obj = beginobj(&fgamwrd,
		station_xys[p_num][four_plr_ver][X_VAL]-4.0f,
		station_xys[p_num][four_plr_ver][Y_VAL]-142.0F,
		1.18f - 0.009f * (float)p_num,
		JAPPLE_BOX_TID);
	fg_msg_obj->w_scale = 2.2f;		// 12 steps back to original size 1.0f
	fg_msg_obj->h_scale = 2.2f;
//	fg_msg_obj->y_angle = DG2RD(angle);
	fg_msg_obj->id = fullgame_bought_ids[p_num];

	while (fg_plq_obj->h_scale > 1.0F)
	{
		fg_plq_obj->w_scale -= 0.05f;
		fg_plq_obj->h_scale -= 0.05f;
//		fg_plq_obj->z_angle = DG2RD(angle);
		generate_sprite_verts(fg_plq_obj);

		fg_msg_obj->w_scale -= 0.05f;
		fg_msg_obj->h_scale -= 0.05f;
//		fg_msg_obj->z_angle = DG2RD(angle);
		generate_sprite_verts(fg_msg_obj);
		
//		angle-=5.0f;

		sleep(1);
	}

	snd_scall_bank(cursor_bnk_str,FULLGM_HIT_SND,VOLUME7,127,PRIORITY_LVL3);

	flsh_obj = beginobj(optscr_flsh2_imgs[0],
		japbx_cntr_xys[p_num][four_plr_ver][X_VAL],
		japbx_cntr_xys[p_num][four_plr_ver][Y_VAL]+2.0f,
		1.10f - 0.005f * (float)p_num,
		SPFX_TID);
	flsh_obj->w_scale = 3.0f;
	flsh_obj->h_scale = 2.7f;
	flsh_obj->state.alpha_rgb_dst_function = GR_BLEND_ONE;		// set additive blend mode
	flsh_obj->id = fullgame_bought_ids[p_num]+10;
	generate_sprite_verts(flsh_obj);

	for (i=0; i < (sizeof(optscr_flsh2_imgs)/sizeof(image_info_t *))-1; i++)
	{
		change_img_tmu(flsh_obj,optscr_flsh2_imgs[i],SPFX_TID);
		sleep(2);
	}


//	delobj(flsh_obj);
//	flash_obj_white(fg_plq_obj, 5, 3);
	del1c(fullgame_bought_ids[p_num]+10,-1);
	sleep(60);
	// delete by ID
	del1c(fullgame_bought_ids[p_num],-1);
//	delobj(fg_plq_obj);
//	delobj(fg_msg_obj);
}

//-------------------------------------------------------------------------------------------------------------------------
//    				This process wait X ticks then counts down from 9 to 0
//
//  INPUT: ticks b4 cnt dwn from SECONDS, seconds, x, y
//-------------------------------------------------------------------------------------------------------------------------
void timedown(int *args)
{
	sprite_info_t * digit_obj;
	int		wait_secs,old_credits;
	int		seconds;
	char	old_pstatus;
	float	x,y;

	force_selection = 0;

	x = args[2];
	y = args[3] - (is_low_res ? 0.0f : 0.0f);

plyr_in:

	old_credits = get_credits();

	seconds = args[1];
	if(seconds > 9)
		seconds = 9;
	wait_secs = args[0];
	old_pstatus = p_status;

	// delete any prevoius timer
	del1c(OID_CNTDWN_DIGIT,-1);

	digit_obj = beginobj(count_down_imgs[seconds],x,y,3.5f,CNTDWN_TID);
	digit_obj->id = OID_CNTDWN_DIGIT;
	hide_sprite(digit_obj);

	while (wait_secs)
	{
		wait_secs--;
		sleep(tsec);
	}

	while (seconds >= 0)
	{
		// show timer number
		unhide_sprite(digit_obj);
		change_img_tmu(digit_obj,count_down_imgs[seconds],CNTDWN_TID);

		// play time down sound
		snd_scall_bank(cursor_bnk_str,TIMEDOWN_SND,VOLUME5,127,PRIORITY_LVL2);

		// if PSTATUS changes...
		if (old_pstatus != p_status)
			goto plyr_in;

		// show number
		sleep(55);

		// delete number
		hide_sprite(digit_obj);
		snd_scall_bank(cursor_bnk_str,TIMEDOWN_SND,VOLUME5,127,PRIORITY_LVL2);

		// if PSTATUUS changes...
		if (old_pstatus != p_status)
			goto plyr_in;

		// show no number for a bit
		sleep(25);

		// 1 less second
		seconds--;

		// number went negative
		if (seconds > args[1])
			break;
	}

	// a little grace period
  	sleep(25);

	// delete number
	del1c(OID_CNTDWN_DIGIT,-1);

	// mark that the time has expired
	force_selection = 1;											// forces a button press
}

//-------------------------------------------------------------------------------------------------------------------------
//    				This process wait X ticks then counts down from 9 to 0
//
//  INPUT: ticks b4 cnt dwn from SECONDS, seconds, x, y
//-------------------------------------------------------------------------------------------------------------------------
void timedown_buyin(int *args)
{
	sprite_info_t * digit_obj;
	int		wait_secs,old_credits;
	int		seconds,i;
	char	old_pstatus;
	float	x,y;

	force_selection = 0;

	x = args[2];
	y = args[3];

plyr_joined:

	old_credits = get_credits();

	seconds = args[1];
	wait_secs = args[0];
	old_pstatus = p_status;

	// delete any prevoius timer
	del1c(OID_CNTDWN_DIGIT,-1);

	digit_obj = beginobj(count_down_imgs[seconds],x,y,3.5f,CNTDWN_TID);
	digit_obj->id = OID_CNTDWN_DIGIT;
	hide_sprite(digit_obj);

	while (wait_secs)
	{
		wait_secs--;
		sleep(tsec);
	}

	while (seconds >= 0)
	{
		// show timer number
		unhide_sprite(digit_obj);
		change_img_tmu(digit_obj,count_down_imgs[seconds],CNTDWN_TID);

		// play time down sound
		snd_scall_bank(cursor_bnk_str,TIMEDOWN_SND,VOLUME5,127,PRIORITY_LVL4);

		// !!!FIX   if PSTATUS changes..goto plyr_joined:
		if (old_pstatus != p_status)
			goto plyr_joined;

		// show number
//		if ((game_info.game_state == GS_EOQ) && (get_player_sw_current() & 0xF))
		if (get_player_sw_current() & ALL_ABCD_MASK)
		{
			// If game is in overtime, don't allow quick whack out for first few seconds
			if ((game_info.game_quarter >= 4) && (seconds >= 9))
				sleep(60);
			else
				sleep(3);
		}
		else
		{
			for(i = 0; i < tsec; i++)
			{
				// restart timer...if start button pressed
				if ((get_dip_coin_current() & (four_plr_ver?
						(P1_START_SW|P2_START_SW|P3_START_SW|P4_START_SW):
						(P1_START_SW|P2_START_SW))
					) && !check_credits_to_continue())
				{
					seconds = args[1];
					snd_scall_bank(cursor_bnk_str,TIMEDOWN_SND,VOLUME5,127,PRIORITY_LVL4);
					// delete number and show new number
					change_img_tmu(digit_obj,count_down_imgs[seconds],CNTDWN_TID);
					
					sleep(30);
					break;
				}
				else
				{
					if((game_info.game_state == GS_EOQ) && (get_player_sw_current() & ALL_ABCD_MASK))
					{
						if (!(game_info.game_quarter >= 4 && !p_status))
							i = tsec;
					}	
				}
				sleep(1);
			}
		}

		// delete number
		hide_sprite(digit_obj);
		snd_scall_bank(cursor_bnk_str,TIMEDOWN_SND,VOLUME5,127,PRIORITY_LVL4);

		// !!!FIX   if PSTATUUS changes..goto plyr_joined:
		if (old_pstatus != p_status)
			goto plyr_joined;

		// show no number for a bit
		if (get_player_sw_current() & ALL_ABCD_MASK)
		{
			sleep(3);
		}
		else
		{
			for(i = 0; i < 25; i++)
			{
				if ((get_dip_coin_current() & (four_plr_ver?
						(P1_START_SW|P2_START_SW|P3_START_SW|P4_START_SW):
						(P1_START_SW|P2_START_SW))
					) && !check_credits_to_continue())
				{
					seconds = args[1] + 1;
					break;
				}
				sleep(1);
			}
		}
		if (old_credits != get_credits())
		{
			old_credits = get_credits();
			goto plyr_joined;
		}

		// 1 less second
		seconds--;

		// number went negative
		if (seconds > args[1])
			break;
	}

	// delete number
	del1c(OID_CNTDWN_DIGIT,-1);

	// mark that the time has expired
	force_selection = 1;											// forces a button press
}

//-------------------------------------------------------------------------------------------------------------------------
//				This process updpdates the credits message on the botton of the screen
//-------------------------------------------------------------------------------------------------------------------------
void credit_msg(int *args)
{
	sprite_info_t * credit_box_obj;
	float cntr_x;
	float cntr_y;
	int was_low_res = is_low_res;
	int old_creds = -1;
	int cred = coin_check_freeplay();

	// Make sure you put this back before sleeping!!!
	is_low_res = 0;

	// credit backdrop
	credit_box_obj = beginobj(&crdbar,
		(SPRITE_HRES/2.0f),
		(was_low_res ? (SPRITE_VRES/2.0f-2.0f) : (SPRITE_VRES/2.0f)),
		4.5f,	//15.5
		args[1]);
	credit_box_obj->id = OID_JAPPLE_BOX;

	// credit msg
	credit_box_obj = beginobj((cred ? &fplay : &crdwrd),
		(SPRITE_HRES/2.0f)-5.0f,
		(was_low_res ? (SPRITE_VRES/2.0f-6.0f) : (SPRITE_VRES/2.0f)-3.0f),
		3.5f,
		args[1]);
	credit_box_obj->id = OID_JAPPLE_BOX;

	// Back it goes!!!
	is_low_res = was_low_res;

	if (cred)
		die(0);

	cntr_x = credit_msg_xys[0][!!was_low_res][X_VAL];
	cntr_y = credit_msg_xys[0][!!was_low_res][Y_VAL];

	while (1)
	{
		if (old_creds != (cred = get_total_credits()))
		{
			old_creds = cred;
			delete_multiple_strings(CREDIT_MSG_ID, -1);
			set_string_id(CREDIT_MSG_ID);
			set_text_font(FONTID_BAST7T);
			set_text_color(LT_YELLOW);
			set_text_justification_mode(HJUST_CENTER|VJUST_BOTTOM);
			set_text_position(cntr_x+2.0f, cntr_y-4.0f - (is_low_res ? 2.0f : 0.0f), 1.1f);
			oprintf("%d", old_creds);
		}
		sleep(3);
	}
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void stats_page(void)
{
	tmu_ram_texture_t * ptmu_tbl;
	sprite_info_t * bkgrnd_obj;
	game_info_t	* pgi = &game_info;
	float		result, stat_z;
	int			num[MAX_PLYRS], den[MAX_PLYRS], ret, tci, i, c;
	int			was_low_res = is_low_res;

	if (pgi->game_state == GS_ATTRACT_MODE)
		pgi = &game_info_last;
	else if (pgi->game_quarter > 3) {
		unlock_multiple_textures(TXID_HEADS, -1);
		delete_multiple_textures(TXID_HEADS, -1);
		unlock_multiple_textures(TXID_REFEREE, -1);
		delete_multiple_textures(TXID_REFEREE, -1);
	}

	ptmu_tbl = was_low_res ?
		(pgi->game_quarter < 3 ? hlf_stats_pg_textures_tbl_lr : final_stats_pg_textures_tbl_lr ):
		(pgi->game_quarter < 3 ? hlf_stats_pg_textures_tbl    : final_stats_pg_textures_tbl    );

	if (load_textures_into_tmu(ptmu_tbl))
	{
#ifdef DEBUG
		fprintf(stderr, "Couldn't load all stat page textures into tmu\r\n");
		lockup();
#else
		return;
#endif
	}

	stat_z = 5.0F;

	if (was_low_res)
	{
		// Make sure you put this back before sleeping!!!
		is_low_res = 0;

		if(pgi->game_quarter < 3)
		{
			bkgrnd_obj = beginobj(&lstatbk1,    0.0f, 256.0f, stat_z+1, STATS_PG_TID);
			bkgrnd_obj->id = 1;
			bkgrnd_obj->state.texture_filter_mode = GR_TEXTUREFILTER_POINT_SAMPLED;
			bkgrnd_obj = beginobj(&lstatbk1_c1, 0.0f, 256.0f, stat_z+1, STATS_PG_TID);
			bkgrnd_obj->id = 1;																
			bkgrnd_obj->state.texture_filter_mode = GR_TEXTUREFILTER_POINT_SAMPLED;
		}
		else
		{
			bkgrnd_obj = beginobj(&lstatbk2,    0.0f, 256.0f, stat_z+1, STATS_PG_TID);
			bkgrnd_obj->id = 1;
			bkgrnd_obj->state.texture_filter_mode = GR_TEXTUREFILTER_POINT_SAMPLED;
			bkgrnd_obj = beginobj(&lstatbk2_c1, 0.0f, 256.0f, stat_z+1, STATS_PG_TID);
			bkgrnd_obj->id = 1;																
			bkgrnd_obj->state.texture_filter_mode = GR_TEXTUREFILTER_POINT_SAMPLED;
		}

		// Back it goes!!!
		is_low_res = was_low_res;
	}
	else if (pgi->game_quarter < 3)
	{
		bkgrnd_obj = beginobj(&statbk1,    0.0f, SPRITE_VRES, stat_z+1, STATS_PG_TID);
		bkgrnd_obj->id = 1;
		bkgrnd_obj = beginobj(&statbk1_c1, 0.0f, SPRITE_VRES, stat_z+1, STATS_PG_TID);
		bkgrnd_obj->id = 1;
		bkgrnd_obj = beginobj(&statbk1_c2, 0.0f, SPRITE_VRES, stat_z+1, STATS_PG_TID);
		bkgrnd_obj->id = 1;
		bkgrnd_obj = beginobj(&statbk1_c3, 0.0f, SPRITE_VRES, stat_z+1, STATS_PG_TID);
		bkgrnd_obj->id = 1;
	}
	else
	{
		bkgrnd_obj = beginobj(&statbk2,    0.0f, SPRITE_VRES, stat_z+1, STATS_PG_TID);
		bkgrnd_obj->id = 1;
		bkgrnd_obj = beginobj(&statbk2_c1, 0.0f, SPRITE_VRES, stat_z+1, STATS_PG_TID);
		bkgrnd_obj->id = 1;
		bkgrnd_obj = beginobj(&statbk2_c2, 0.0f, SPRITE_VRES, stat_z+1, STATS_PG_TID);
		bkgrnd_obj->id = 1;
		bkgrnd_obj = beginobj(&statbk2_c3, 0.0f, SPRITE_VRES, stat_z+1, STATS_PG_TID);
		bkgrnd_obj->id = 1;
	}
//	}
//	else
//	{
//		if (pgi->game_quarter < 3)
//		{
//			bkgrnd_obj = beginobj(&statbk3,    0.0f, SPRITE_VRES, stat_z+1, STATS_PG_TID);
//			bkgrnd_obj->id = 1;
//			bkgrnd_obj = beginobj(&statbk3_c1, 0.0f, SPRITE_VRES, stat_z+1, STATS_PG_TID);
//			bkgrnd_obj->id = 1;																
//			bkgrnd_obj = beginobj(&statbk3_c2, 0.0f, SPRITE_VRES, stat_z+1, STATS_PG_TID);
//			bkgrnd_obj->id = 1;
//			bkgrnd_obj = beginobj(&statbk3_c3, 0.0f, SPRITE_VRES, stat_z+1, STATS_PG_TID);
//			bkgrnd_obj->id = 1;
//		}
//		else
//		{
//			bkgrnd_obj = beginobj(&statbk4,    0.0f, SPRITE_VRES, stat_z+1, STATS_PG_TID);
//			bkgrnd_obj->id = 1;
//			bkgrnd_obj = beginobj(&statbk4_c1, 0.0f, SPRITE_VRES, stat_z+1, STATS_PG_TID);
//			bkgrnd_obj->id = 1;																
//			bkgrnd_obj = beginobj(&statbk4_c2, 0.0f, SPRITE_VRES, stat_z+1, STATS_PG_TID);
//			bkgrnd_obj->id = 1;
//			bkgrnd_obj = beginobj(&statbk4_c3, 0.0f, SPRITE_VRES, stat_z+1, STATS_PG_TID);
//			bkgrnd_obj->id = 1;
//		}
//	}


	// display credits
	qcreate("cred_msg",CREDIT_PID,credit_msg,0,JAPPLE_BOX_TID,0,0);

//
// ** NOTE ** This code only works if NUM_PLAYERS = 4 (num on each team being 2 not 3)
//
	// create TEAM 1 mugshots
	bkgrnd_obj = beginobj(selected_players[!four_plr_ver].player_data->pii_mug, 29.0f,  321.0f, 1.5f, STATS_PG_TID);
	bkgrnd_obj->id = 1;
	bkgrnd_obj = beginobj(selected_players[ four_plr_ver].player_data->pii_mug, 128.0f, 321.0f, 1.5f, STATS_PG_TID);
	bkgrnd_obj->id = 1;

	// create TEAM 2 mugshots
	bkgrnd_obj = beginobj(selected_players[2].player_data->pii_mug, 274.0f, 321.0f, 1.5f, STATS_PG_TID);
	bkgrnd_obj->id = 1;
	bkgrnd_obj = beginobj(selected_players[3].player_data->pii_mug, 373.0f, 321.0f, 1.5f, STATS_PG_TID);
	bkgrnd_obj->id = 1;

	// print scores
	set_text_z_inc(0.1f);

	set_text_font(FONTID_NBA20);
	set_string_id(STAT_PG_TXT_ID);
	set_text_position(50.0f, SPRITE_VRES-28.0f, 2.0f);
	oprintf("%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, compute_score(0));
	set_text_position(457.0f, SPRITE_VRES-28.0f, 2.0f);
	oprintf("%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, compute_score(1));


	set_text_font(FONTID_BAST8T);
	set_string_id(STAT_PG_TXT_ID);
	// print player names
	for (i=0; i < MAX_PLYRS; i++)
	{
		int j = (four_plr_ver || i >= NUM_PERTEAM) ? i : i^1;
		set_text_position(tm_stat_pg_name_xs[is_low_res][i], 166.0f, 2.0f);
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, selected_players[j].player_data->szlast);

		//set_text_position(station_xys[i][1][X_VAL], 332.0f, 2.0f);
		//if (p_status2 & (1<<i))
		// 	oprintf("%dj%dcPLAYER %d", (HJUST_CENTER|VJUST_CENTER), WHITE, i+1);
		//else
		//	oprintf("%dj%dcCPU", (HJUST_CENTER|VJUST_CENTER), WHITE);
	}

	// print team stats
	if (was_low_res)
		set_text_font(FONTID_BAST7T);
	else
		set_text_font(FONTID_BAST8T);
		
	set_string_id(STAT_PG_TXT_ID);


	// fg's (2 pointers)
	for (i=0; i < MAX_PLYRS; i++)
	{
		num[i] = plyr_stat_ptrs[i]->two_ptrs_made;
		den[i] = plyr_stat_ptrs[i]->two_ptrs_attempted;
	}
	ret = return_best_percent(num, den);

	for (i=0; i < MAX_PLYRS; i++)
	{
		int j = (four_plr_ver || i >= NUM_PERTEAM) ? i : i^1;
		c = (ret & (1<<j)) ? LT_GREEN : WHITE;

		set_text_position(tm_stat_pg_stats_xs[is_low_res][i], tm_stat_pg_stats_ys[is_low_res][0], stat_z);
		oprintf("%dj%dc%d/%d", (HJUST_CENTER|VJUST_CENTER), c, num[j], den[j]);

		set_text_position(tm_stat_pg_stats_xs[is_low_res][i]+((i==0 || i==2) ? -35.0f : 35.0f), tm_stat_pg_stats_ys[is_low_res][0], stat_z);

		if (den[j] <= 0)
			oprintf("%dj%dc 0%%", (HJUST_CENTER|VJUST_CENTER), c);
		else
		{
			result = (float)num[j] / (float)den[j] * 100.0f;
			if (result >= 100.0f)
				oprintf("%dj%dc100%%", (HJUST_CENTER|VJUST_CENTER), c);
			else
				oprintf("%dj%dc%3.1f%%", (HJUST_CENTER|VJUST_CENTER), c, result);
		}
	}


	// 3 pointers
	for (i=0; i < MAX_PLYRS; i++)
	{
		num[i] = plyr_stat_ptrs[i]->three_ptrs_made;
		den[i] = plyr_stat_ptrs[i]->three_ptrs_attempted;
	}
	ret = return_best_percent(num, den);

	for (i=0; i < MAX_PLYRS; i++)
	{
		int j = (four_plr_ver || i >= NUM_PERTEAM) ? i : i^1;
		c = (ret & (1<<j)) ? LT_GREEN : WHITE;

		set_text_position(tm_stat_pg_stats_xs[is_low_res][i], tm_stat_pg_stats_ys[is_low_res][1], stat_z);
		oprintf("%dj%dc%d/%d", (HJUST_CENTER|VJUST_CENTER), c, num[j], den[j]);

		set_text_position(tm_stat_pg_stats_xs[is_low_res][i]+((i==0 || i==2) ? -35.0f : 35.0f), tm_stat_pg_stats_ys[is_low_res][1], stat_z);

		if (den[j] <= 0)
			oprintf("%dj%dc 0%%", (HJUST_CENTER|VJUST_CENTER), c);
		else
		{
			result = (float)num[j] / (float)den[j] * 100.0f;
			if (result >= 100.0f)
				oprintf("%dj%dc100%%", (HJUST_CENTER|VJUST_CENTER), c);
			else
				oprintf("%dj%dc%3.1f%%", (HJUST_CENTER|VJUST_CENTER), c, result);
		}

	}


	// total points
	for (i=0; i < MAX_PLYRS; i++)
	{
		num[i] = plyr_stat_ptrs[i]->three_ptrs_made*3 + plyr_stat_ptrs[i]->two_ptrs_made*2 + plyr_stat_ptrs[i]->dunks_made*2;
	}
	ret = return_best_total(num);

	for (i=0; i < MAX_PLYRS; i++)
	{
		int j = (four_plr_ver || i >= NUM_PERTEAM) ? i : i^1;
		c = (ret & (1<<j)) ? LT_GREEN : WHITE;

		set_text_position(tm_stat_pg_stats_xs[is_low_res][i], tm_stat_pg_stats_ys[is_low_res][2], stat_z);
		oprintf("%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), c, num[j]);
	}


	// dunks
	for (i=0; i < MAX_PLYRS; i++)
	{
		num[i] = plyr_stat_ptrs[i]->dunks_made;
		den[i] = plyr_stat_ptrs[i]->dunks_attempted;
	}
	ret = return_best_percent(num, den);

	for (i=0; i < MAX_PLYRS; i++)
	{
		int j = (four_plr_ver || i >= NUM_PERTEAM) ? i : i^1;
		c = (ret & (1<<j)) ? LT_GREEN : WHITE;

		set_text_position(tm_stat_pg_stats_xs[is_low_res][i], tm_stat_pg_stats_ys[is_low_res][3], stat_z);
		oprintf("%dj%dc%d/%d", (HJUST_CENTER|VJUST_CENTER), c, num[j], den[j]);

		set_text_position(tm_stat_pg_stats_xs[is_low_res][i]+((i==0 || i==2) ? -35.0f : 35.0f), tm_stat_pg_stats_ys[is_low_res][3], stat_z);

		if (den[j] <= 0)
			oprintf("%dj%dc 0%%", (HJUST_CENTER|VJUST_CENTER), c);
		else
		{
			result = (float)num[j] / (float)den[j] * 100.0f;
			if (result >= 100.0f)
				oprintf("%dj%dc100%%", (HJUST_CENTER|VJUST_CENTER), c);
			else
				oprintf("%dj%dc%3.1f%%", (HJUST_CENTER|VJUST_CENTER), c, result);
		}
	}


	// assists
	for (i=0; i < MAX_PLYRS; i++)
	{
		num[i] = plyr_stat_ptrs[i]->assists;
	}
	ret = return_best_total(num);

	for (i=0; i < MAX_PLYRS; i++)
	{
		int j = (four_plr_ver || i >= NUM_PERTEAM) ? i : i^1;
		c = (ret & (1<<j)) ? LT_GREEN : WHITE;

		set_text_position(tm_stat_pg_stats_xs[is_low_res][i], tm_stat_pg_stats_ys[is_low_res][4], stat_z);
		oprintf("%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), c, num[j]);
	}


	// steals
	for (i=0; i < MAX_PLYRS; i++)
	{
		num[i] = plyr_stat_ptrs[i]->steals;
	}
	ret = return_best_total(num);

	for (i=0; i < MAX_PLYRS; i++)
	{
		int j = (four_plr_ver || i >= NUM_PERTEAM) ? i : i^1;
		c = (ret & (1<<j)) ? LT_GREEN : WHITE;

		set_text_position(tm_stat_pg_stats_xs[is_low_res][i], tm_stat_pg_stats_ys[is_low_res][5], stat_z);
		oprintf("%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), c, num[j]);
	}


	// blocks
	for (i=0; i < MAX_PLYRS; i++)
	{
		num[i] = plyr_stat_ptrs[i]->blocks;
	}
	ret = return_best_total(num);

	for (i=0; i < MAX_PLYRS; i++)
	{
		int j = (four_plr_ver || i >= NUM_PERTEAM) ? i : i^1;
		c = (ret & (1<<j)) ? LT_GREEN : WHITE;

		set_text_position(tm_stat_pg_stats_xs[is_low_res][i], tm_stat_pg_stats_ys[is_low_res][6], stat_z);
		oprintf("%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), c, num[j]);
	}


	// rebounds
	for (i=0; i < MAX_PLYRS; i++)
	{
		num[i] = plyr_stat_ptrs[i]->rebounds;
	}
	ret = return_best_total(num);

	for (i=0; i < MAX_PLYRS; i++)
	{
		int j = (four_plr_ver || i >= NUM_PERTEAM) ? i : i^1;
		c = (ret & (1<<j)) ? LT_GREEN : WHITE;

		set_text_position(tm_stat_pg_stats_xs[is_low_res][i], tm_stat_pg_stats_ys[is_low_res][7], stat_z);
		oprintf("%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), c, num[j]);
	}


	// injured
	for (i=0; i < MAX_PLYRS; i++)
	{
		num[i] = pushed_cnt[i];
	}
	ret = return_best_total(num);

	for (i=0; i < MAX_PLYRS; i++)
	{
		int j = (four_plr_ver || i >= NUM_PERTEAM) ? i : i^1;
		c = (ret & (1<<j)) ? LT_RED : WHITE;

		set_text_position(tm_stat_pg_stats_xs[is_low_res][i], tm_stat_pg_stats_ys[is_low_res][8], stat_z);
		oprintf("%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), c, num[j]);
	}


	// turnovers
	for (i=0; i < MAX_PLYRS; i++)
	{
		num[i] = plyr_stat_ptrs[i]->turnovers;
	}
	ret = return_best_total(num);

	for (i=0; i < MAX_PLYRS; i++)
	{
		int j = (four_plr_ver || i >= NUM_PERTEAM) ? i : i^1;
		c = (ret & (1<<j)) ? LT_RED : WHITE;

		set_text_position(tm_stat_pg_stats_xs[is_low_res][i], tm_stat_pg_stats_ys[is_low_res][9], stat_z);
		oprintf("%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), c, num[j]);
	}


	// Take away the plates
	turn_transit_off(0);

	// wait for X ticks or for a button press after N ticks
	wait_for_any_butn_press(tsec*1,tsec*22);

//	tci = 0;
	tci = (game_info.game_quarter < 3) ? 0 : trivia_contest_init();

	if (!tci && (game_info.game_quarter < 3 || !record_me))
	{
		if ((game_info.game_quarter < 3) || (record_me))
		  	turn_transit_on();
		else
			game_just_finished = 1;
			
		// Stop ambient noise
	  	snd_stop_track(SND_TRACK_1);
	  	snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);
	}

	// do clean up
	killall(CREDIT_PID, -1);					// kill the plyr_plaques_awards process
	del1c(1, -1);
	del1c(OID_JAPPLE_BOX, -1);
	delete_multiple_strings(CREDIT_MSG_ID, -1);
	delete_multiple_strings(STAT_PG_TXT_ID, -1);
	delete_multiple_textures(STATS_PG_TID,-1);

	if (tci)
	{
		trivia_contest();

		game_just_finished = 1;
	}
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
int find_best_total(int * num)
{
	int i, max, ret;

	for (max = num[0], i = 1; i < MAX_PLYRS; i++)
	{
		if (num[i] > max)
			max = num[i];
	}
	for (ret = 0, i = MAX_PLYRS-1; i >= 0; i--)
	{
		ret <<= 1;
		if (num[i] == max)
			ret++;
	}
	return ret;
}

int return_best_total(int * num)
{
	int ret = find_best_total(num);
	return (ret == 0xf ? 0 : ret);
}

int return_best_percent(int * num, int * den)
{
	int i, j, max, ret;

	ret = find_best_total(num);

	// If all numerators are zero, return no best %
	if ((ret == 0xf) && (num[0] == 0))
		ret = 0;

	if (ret != (ret & -ret))
	{
		for (max = 0, j = 0, i = 0; i < MAX_PLYRS; i++)
		{
			if ((ret & (1<<i)) && ((!j++) || (den[i] < max)))
				max = den[i];
		}
		for (j = 0, i = MAX_PLYRS-1; i >= 0; i--)
		{
			j <<= 1;
			if (den[i] == max)
				j++;
		}
		ret &= j;
	}
	return (ret == 0xf ? 0 : ret);
}

//-------------------------------------------------------------------------------------------------
//    This process shows the "get ready for x quarter" message at the top of the buy in page
//
//
//		INPUT: Nothing
//-------------------------------------------------------------------------------------------------
//void get_ready_for(void)
//{
//	float	ypos;
//	ostring_t *pmsg[4];
//	sprite_info_t *obj;
////	static sprite_info_t *idiot_bx_obj;
//
//	obj = beginobj(&gtrdyfr, 0.0f, SPRITE_VRES-10.0f, 300.0f, EOQ_TID);
//	obj->id = 1;
//	if (game_info.game_quarter == 1)
//		obj = beginobj(&scndprd, 0.0f, SPRITE_VRES-10.0f, 300.0f, EOQ_TID);
//	else if (game_info.game_quarter == 2)
//		obj = beginobj(&thrdprd, 0.0f, SPRITE_VRES-10.0f, 300.0f, EOQ_TID);
//	else if (game_info.game_quarter == 3)
//		obj = beginobj(&frthprd, 0.0f, SPRITE_VRES-10.0f, 300.0f, EOQ_TID);
//	else
//		obj = beginobj(&ovrtm, 0.0f, SPRITE_VRES-10.0f, 300.0f, EOQ_TID);
//	obj->id = 1;
//}

//-------------------------------------------------------------------------------------------------
//	  This process show the "join in" and/or "continue" msg
//		for players who aren't in the game.
//
//  	INPUT: player number,texture id,0,0
//-------------------------------------------------------------------------------------------------
void challenger_needed_msg(int *args)
{
	ostring_t * pmsg[] = {0, 0, 0};
	int pnum,cntr_x,cntr_y,i,tid;
	sprite_info_t		*box_obj = NULL;
	sprite_info_t		*msg_obj;
	int					free_quarter;
	float				xpos, ypos;
	int					was_low_res = is_low_res;

	pnum = args[0];
	tid = args[1];
	free_quarter = args[2];

	// player isn't in game
	cntr_x = japbx_cntr_xys[pnum][four_plr_ver][X_VAL];
	cntr_y = japbx_cntr_xys[pnum][four_plr_ver][Y_VAL];

	
//	is_low_res = 0;
	msg_obj = beginobj(&lpresstr, cntr_x, cntr_y-1.0f, 1.2F, tid);
	msg_obj->id = 1;
//	is_low_res = was_low_res;

	while ( !(p_status & (1<<pnum)) || (game_info.game_state == GS_EOQ))
	{
		// player has already bought this qtr. just tell him to get ready!
		if (p_status & (1<<pnum))
		{
			change_img_tmu(msg_obj,&lgetread,tid);

			// Also tell him if he won a free quarter
			if(free_quarter & (1<<pnum))
			{
				int fid = is_low_res ? FONTID_NBA14 : FONTID_NBA20;
				int yinc = get_font_height(fid) + 3;

				i = (pnum<2?1:3);
			//	i = !four_plr_ver ? (!pnum?1:3) : (pnum<2?1:3);

				xpos = (SPRITE_HRES / 4.0f) * (float)i;
				ypos = (SPRITE_VRES * 0.50f);

				set_string_id(CREDIT_MSG_ID + 1);
				set_text_justification_mode(HJUST_CENTER|VJUST_TOP);
				set_text_color(LT_GREEN);
				set_text_font(fid);

				set_text_z_inc(-0.1);

				set_text_position(xpos, ypos, 3.5f);

				i = 1;
				if (four_plr_ver && (
						((pnum == 1) && (free_quarter & (1<<0))) ||
						((pnum == 3) && (free_quarter & (1<<2))) ))
					i = 2;
				ypos -= (float)(yinc * i--);
				if (is_low_res) ypos -= !i?5.0f:8.0f;

				if (!i)
				{
					pmsg[0] = oprintf("CONGRATULATIONS");
					box_obj = beginobj(&playresult, xpos + 2, ypos - yinc, 3.51f, JAPPLE_BOX_TID);
					box_obj->id = 1;
					box_obj->w_scale = 1.10f;
					box_obj->h_scale = 1.55f;
					generate_sprite_verts(box_obj);
				}

				set_text_position(xpos, ypos, 3.5f);
				pmsg[1] = oprintf("PLAYER %d", pnum + ((four_plr_ver)||(pnum==0)));

				if (!i)
				{
					ypos -= (float)yinc;
					ypos -= (float)yinc;
					if (is_low_res) ypos -= 12.0f;

					set_text_font(is_low_res ? FONTID_BAST8TA : FONTID_NBA10);
					set_text_position(xpos, ypos, 3.5f);
					if (!i) pmsg[2] = oprintf("YOUR NEXT PERIOD IS FREE");
				}

				set_text_z_inc( 0.0f);

				qcreate("pflash",BONUS_PID, alpha_string_flasher, (int)pmsg[0], (int)pmsg[1], (int)pmsg[2], 5 );

				// don't wanna do this again!
				free_quarter = 0;
			}
		}
		else																								// player hasn't bought any more quarters.
		{
			if (coin_check_freeplay())
				change_img_tmu(msg_obj,&lfreeply,tid);						// game in FREE PLAY -- just show freeplay msg
	
			else if (p_status2 & (1<<pnum))										// was player in last qtr. ?
			{																									// yes (continue state)
				if (check_credits_to_continue())
					change_img_tmu(msg_obj,&lpresstr,tid);
				else
					change_img_tmu(msg_obj,&linsertc,tid);
			}
			else																							// player wasn't in last qtr. (new start state)
			{
				if (check_credits_to_start())
					change_img_tmu(msg_obj,&lpresstr,tid);
				else
					change_img_tmu(msg_obj,&linsertc,tid);
			}
		}

		// show first msg for a bit
		for (i=0; i<25; i++)
		{
//			if (teammate_in_tmsel(pnum))				// used only if 4 player version
//				break;
			if ((p_status & (1<<pnum)) && (game_info.game_state != GS_EOQ))
				break;
			sleep(2);
		}

		//
		// toggle message
		//
		if (!(p_status & (1<<pnum)))
		{																										// player hasn't bought this qtr.
			if (coin_check_freeplay())
				change_img_tmu(msg_obj,&lpresstr,tid);						// game in FREE PLAY -- show 'press start' msg

			else if (p_status2 & (1<<pnum))										// was player in last qtr. ?
				change_img_tmu(msg_obj,&lcontinu,tid);
	
			else																							// player wasn't in last qtr. (new start state)
				change_img_tmu(msg_obj,&joinin,tid);
		}

		// show second msg for a bit
		for (i=0; i<25; i++)
		{
//			if (teammate_in_tmsel(pnum))				// used only if 4 player version
//				break;
			if ((p_status & (1<<pnum)) && (game_info.game_state != GS_EOQ))
				break;
			sleep(2);
		}
	}
	killall(BONUS_PID, -1);
	if (box_obj) delobj(box_obj);
	delobj(msg_obj);
	delete_multiple_strings(CREDIT_MSG_ID + 1, -1);
}

//-------------------------------------------------------------------------------------------------------------------------
//    This process updates/shows the PLAYERS credits (quarters) bought
//
//	This ASSUMES CREDIT TEXTURES are in TMU
//
//  INPUT: player number,cntr X,cntr Y,0
//-------------------------------------------------------------------------------------------------------------------------
void plyr_credit_box(int *args)
{
	int    				pnum,tid;
	int	   				gq,qp,oqp;
	pdata_t				*pdata;
	sprite_info_t		*credit_mrkr_obj;
	sprite_info_t		*plr_namebx_obj;
	//sprite_info_t		*plr_bx_shdw_obj;
	image_info_t		*img;
	float				x,y;

	pnum = args[0];
	if (pnum < 0) pnum = -1 - pnum;
	x = args[1];
	y = args[2];
	tid = args[3];
	pdata = pdata_blocks + pnum;

	// drop shadow for box
//	plr_bx_shdw_obj = beginobj(&japlsh_4, args[1], args[2], 1.4F, tid);
//	plr_bx_shdw_obj->id = OID_JAPPLE_BOX;

//!!!	// name entry box
//!!!	if ((pnum == 0) && (four_plr_ver))
//!!!		x += 4.0f;
//!!!	if ((pnum == 3) && (four_plr_ver))
//!!!		x -= 4.0f;

//	plr_namebx_obj = beginobj(&japlbox, args[1], args[2], 1.3F, tid);
	plr_namebx_obj = beginobj(japple_box_imgs[four_plr_ver][pnum], x, y, (7.0F + (pnum * 0.1f)), tid);
	plr_namebx_obj->id = OID_JAPPLE_BOX;

	gq = game_info.game_quarter;
	if (gq >= NUM_QUARTERS)
		gq = NUM_QUARTERS-1;

	// little orange basketballs
	if (game_info.game_state == GS_GAME_OVER || (game_info.game_state == GS_EOQ && game_info.game_mode == GM_GAME_OVER))
	{
		credit_mrkr_obj = beginobj(&japlxxxx, x-3, y, 1.25F, tid);
		//hide_sprite(credit_mrkr_obj);
	}
	else	
		credit_mrkr_obj = beginobj(credit_box_imgs[gq][pdata->quarters_purchased], x-3, y, 1.25F, tid);

	credit_mrkr_obj->id = OID_JAPPLE_BOX;

	if (args[0] < 0)
		hide_sprite(credit_mrkr_obj);

	oqp = pdata->quarters_purchased;

	while (1)
	{
		qp = pdata->quarters_purchased;
		if (oqp != qp)
		{
			oqp = qp;
//			gq = game_info.game_quarter;
//			if (gq >= NUM_QUARTERS)
//				gq = NUM_QUARTERS-1;

			// change image for credit box if needed
			img = credit_box_imgs[gq][qp];

			if (credit_mrkr_obj->ii != img)
			{
				change_img_tmu(credit_mrkr_obj,img,tid);
//				credit_mrkr_obj->ii = img;
//				generate_sprite_verts(credit_mrkr_obj);
			}
	
			qcreate("flshcrdt",CREDIT_PID,flash_credit_balls,pnum,tid,0,0);
	
		}
		sleep(1);
	}
	delobj(credit_mrkr_obj);
	delobj(plr_namebx_obj);
}

//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
void flash_credit_balls(int *args)
{
	sprite_info_t 		*burst_obj;
	pdata_t				*pdata;
	int					i, qp, gq, tid, pnum;
	float				zs[] = {1.200f, 1.201f, 1.202f, 1.203f, 1.204f, 1.205f, 1.206f};

	pnum = args[0];
	tid = args[1];
	pdata = pdata_blocks + pnum;

	gq = game_info.game_quarter;
	if (gq >= NUM_QUARTERS)
		gq = NUM_QUARTERS-1;

	qp = pdata->quarters_purchased + gq;

	burst_obj = beginobj(burst_flsh_imgs[0],
		credit_fb_flsh_xys[four_plr_ver][pnum][qp][X_VAL],
		credit_fb_flsh_xys[four_plr_ver][pnum][qp][Y_VAL],
		zs[qp],
		tid);
	burst_obj->w_scale = 2.5f;
	burst_obj->h_scale = 2.5f;
	burst_obj->id = OID_JAPPLE_BOX;		// Survive del1c
//	burst_obj->state.chroma_key_mode = GR_CHROMAKEY_ENABLE;
	burst_obj->state.alpha_rgb_dst_function = GR_BLEND_ONE;
	generate_sprite_verts(burst_obj);

	for (i=0; i < (sizeof(burst_flsh_imgs)/sizeof(image_info_t *))-1; i++)
	{
		change_img_tmu(burst_obj,burst_flsh_imgs[i],tid);
		sleep(2);
	}
//	del1c(OID_JAPPLE_BOX+pnum,-1);
	delobj(burst_obj);
}

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
void flash_foul_meter_proc(int *args)
{
	sprite_info_t 		*burst_obj;
	pdata_t				*pdata;
	int					i, qp, gq, tid, pnum;
	sprite_info_t	 	*flash_obj;
	sprite_info_t 		*flash2_obj;
	int					was_low_res = is_low_res;


	if ((showhide_flag != SHOW) || (pup_no_fouls))
		die(0);


	i = pnum = args[0];

	flash_obj = beginobj(pflash_flsh_imgs[0],
						pintx_turbo[i][four_plr_ver]+((i > 1) ? -39.0f : 43.0f), pinty_turbo[2][0]-5.0f, 4.5f, SPFX_TID);

//	flash2_obj = beginobj(optscr_flsh2_imgs[0],
//						pintx_turbo[i][four_plr_ver]+((i > 1) ? -39.0f : 43.0f), pinty_turbo[2][!!was_low_res]-5.0f, 2.0f + i, SPFX_TID);

	flash_obj->id = 0x6666+pnum;
//	flash2_obj->id = 0x6666+pnum;
	flash_obj->w_scale = 2.15f;
	flash_obj->h_scale = 2.10f;
	// set to additive blending
	flash_obj->state.alpha_rgb_dst_function = GR_BLEND_ONE;
	generate_sprite_verts(flash_obj);

//	flash2_obj->w_scale = 0.21f * 1.9f;
//	flash2_obj->h_scale = 0.21f * 1.95f;
	// set to additive blending
//	flash2_obj->state.alpha_rgb_dst_function = GR_BLEND_ONE;
//	generate_sprite_verts(flash2_obj);

	for (i=0; i < (sizeof(pflash_flsh_imgs)/sizeof(image_info_t *))-1; i++)
	{
		change_img_tmu(flash_obj,pflash_flsh_imgs[i],SPFX_TID);
//		change_img_tmu(flash2_obj,optscr_flsh2_imgs[i],SPFX_TID);
		sleep(2);
	}

	del1c(0x6666+pnum,-1);
}

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
void show_final_player_stats(void)
{
	int		pnum, i;
	float	cntr_x;
	int displayed_one;
//	int		show_me = 0;

	// Are any players valid to show?
//	for(pnum = 0; pnum < MAX_PLYRS; pnum++)
//	{
//		if ((plyr_data_ptrs[pnum]->plyr_name[0] != '\0') &&
//			(plyr_data_ptrs[pnum]->plyr_name[0] != '-') &&
//			(qtr_pstatus[pnum] == ((1 << game_info.game_quarter) - 1)))
//		{
//			show_me |= 1 << pnum;
//		}
//	}
	if (!record_me)
		return;

	//rec_sort_ram = NULL;
	get_all_records();

	draw_backdrop();

	qcreate("cred_msg",CREDIT_PID,credit_msg,0,JAPPLE_BOX_TID,0,0);
	
	// Show stats for each player
	displayed_one = 0;
	for(pnum = 0; pnum < MAX_PLYRS; pnum++)
	{
		if (record_me & (1<<pnum))
		{
			// create players entry plaque (top half)
			beginobj(init_plaq_imgs[four_plr_ver][pnum],
					 plaq_xs[pnum][four_plr_ver],
					 SPRITE_VRES-163.0f,
					 60.0F,
					 YES_NO_PLAQ_TID);

			// show japple boxes
			qcreate("credbox", CREDIT_PID, plyr_credit_box,
				pnum,
				station_xys[pnum][four_plr_ver][X_VAL],
				station_xys[pnum][four_plr_ver][Y_VAL],
				JAPPLE_BOX_TID);
			print_players_name(
				pnum,
				japbx_cntr_xys[pnum][four_plr_ver][X_VAL],
				japbx_cntr_xys[pnum][four_plr_ver][Y_VAL],
				plyr_data_ptrs[pnum]->plyr_name,
				1);

			get_generic_record(PLYR_RECS_ARRAY,selected_players[pnum].record_num,plyr_data_ptrs[pnum]);			// get record from cmos into ram
			//show_player_stats(pnum);
			displayed_one |= 
				print_player_record_stats(pnum,station_xys[pnum][four_plr_ver][X_VAL],1);
		}
	}

	draw_enable(1);

	turn_transit_off(1);

	wait_for_any_butn_press(tsec,tsec*20);

	if (displayed_one)
	{
		for(pnum = 0; pnum < MAX_PLYRS; pnum++)
		{
			if (record_me & (1<<pnum)) {
				delete_multiple_strings(STAT_PG_TXT_ID+pnum, 0xffffffff);
				get_generic_record(PLYR_RECS_ARRAY,selected_players[pnum].record_num,plyr_data_ptrs[pnum]);
				print_player_record_stats(pnum,station_xys[pnum][four_plr_ver][X_VAL],2);
			}
		}
		wait_for_any_butn_press(tsec,tsec*20);
	}

	delete_multiple_strings(STAT_PG_TXT_ID+0, -1);							// delete the stats
	delete_multiple_strings(STAT_PG_TXT_ID+1, -1);							// delete the stats
	delete_multiple_strings(0, 0);
	del1c(0, -1);
	del1c(OID_STAT_PG, -1);
	killall(CREDIT_PID, -1);					// kill the plyr_plaques_awards process
	if(rec_sort_ram)
	{
		JFREE(rec_sort_ram);
		rec_sort_ram = NULL;
	}

	game_just_finished = 1;			// show attract (game-ended) movie
	show_them_hiscore_tables_first = 1;
}

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
void game_over_screen(void)
{
	// show game over screen
	beginobj(&gameover,    0.0F, SPRITE_VRES, 250.0f, 0xbeef);
	beginobj(&gameover_c1, 0.0F, SPRITE_VRES, 250.0f, 0xbeef);
	beginobj(&gameover_c2, 0.0F, SPRITE_VRES, 250.0f, 0xbeef);
	beginobj(&gameover_c3, 0.0F, SPRITE_VRES, 250.0f, 0xbeef);
}

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
void show_player_stats(int p_num)
{
	float			msg_spc;
	sprite_info_t 	*stat_obj;
	sprite_info_t 	*dshdw_obj;
	int				i;
	plyr_record_t	*_plr_rec = plyr_data_ptrs[p_num];
	plyr_record_t	*plr_rec;
	int				ver;
	float			cntr_x;			// center X pos. for players section


	// make sure records are in ram!!
	if (!rec_sort_ram)
		return;

	ver = four_plr_ver;

	msg_spc = 14.0F;

	_plr_rec = plyr_data_ptrs[p_num];
//	_plr_rec = &rec_sort_ram[find_record_in_ram(_plr_rec->plyr_name)-1];
//	copy_record(&plrs_rec, _plr_rec);

	//
	// Print Stats:
	//
	set_text_z_inc(0.1f);
	set_string_id(666 + p_num);
	set_text_font(FONTID_NBA10);
	set_text_position(plaq_xs[p_num][ver], SPRITE_VRES-45.0f, 20.0F);
	oprintf("%dj%dcSTATS", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);

	set_string_id(STAT_PG_TXT_ID+p_num);

	// print OVERALL rank
	qsort(rec_sort_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_wins);
	plr_ranks[p_num][OVR_RNK] = find_record_in_ram(plr_rec->plyr_name);

	set_text_position(stat_plqs_xys[ver][0][p_num][X_VAL], stat_plqs_xys[ver][0][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][0], 30.0F);
	oprintf("%dj%dc RANK", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][0][p_num][X_VAL], stat_plqs_xys[ver][0][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][0], 30.0F);
	oprintf("%dj%dc%d / %d", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_ranks[p_num][OVR_RNK], NUM_PLYR_RECORDS);

	// print WIN-LOSS record
	set_text_position(stat_plqs_xys[ver][1][p_num][X_VAL], stat_plqs_xys[ver][1][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][1], 30.0F);
	oprintf("%dj%dc RECORD", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][1][p_num][X_VAL], stat_plqs_xys[ver][1][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][1], 30.0F);
	oprintf("%dj%dc %d W/ %d L", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_rec->games_won, (plr_rec->games_played-plr_rec->games_won));

	// print OFFFENSIVE RANK
	qsort(rec_sort_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_points_scored);
	plr_ranks[p_num][OFF_RNK] = find_record_in_ram(plr_rec->plyr_name);

	set_text_position(stat_plqs_xys[ver][2][p_num][X_VAL], stat_plqs_xys[ver][2][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][2], 30.0F);
	oprintf("%dj%dc OFF. RANK", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][2][p_num][X_VAL], stat_plqs_xys[ver][2][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][2], 30.0F);
	oprintf("%dj%dc%d / %d", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_ranks[p_num][OFF_RNK], NUM_PLYR_RECORDS);

	// print DEFENSIVE RANK
	qsort(rec_sort_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_points_allowed);
	plr_ranks[p_num][DEF_RNK] = find_record_in_ram(plr_rec->plyr_name);

	set_text_position(stat_plqs_xys[ver][3][p_num][X_VAL], stat_plqs_xys[ver][3][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][3], 30.0F);
	oprintf("%dj%dc DEF. RANK", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][3][p_num][X_VAL], stat_plqs_xys[ver][3][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][3], 30.0F);
	oprintf("%dj%dc%d / %d", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_ranks[p_num][DEF_RNK], NUM_PLYR_RECORDS);

	// print WINSTREAK
	qsort(rec_sort_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_win_streaks);
	plr_ranks[p_num][STK_RNK] = find_record_in_ram(plr_rec->plyr_name);

	set_text_position(stat_plqs_xys[ver][4][p_num][X_VAL], stat_plqs_xys[ver][4][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][4], 30.0F);
	oprintf("%dj%dc STREAK", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][4][p_num][X_VAL], stat_plqs_xys[ver][4][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][4], 30.0F);
	{
		int tmp = plr_rec->winstreak;
		if (tmp < 0) {
			tmp = -tmp;
			oprintf("%dj%dc %d LOSS%s", (HJUST_CENTER|VJUST_CENTER), WHITE, tmp, tmp != 1 ? "ES" : "");
		} else
			oprintf("%dj%dc %d WIN%s", (HJUST_CENTER|VJUST_CENTER), WHITE, tmp, tmp != 1 ? "S" : "");
	}

	// print TEAMS DEFEATED
	set_text_position(stat_plqs_xys[ver][5][p_num][X_VAL], stat_plqs_xys[ver][5][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][5], 30.0F);
	oprintf("%dj%dc DEFEATED", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][5][p_num][X_VAL], stat_plqs_xys[ver][5][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][5], 30.0F);
	{
		int tmp;
		tmp = calc_tms_def(plr_rec->teams_defeated);
		if (tmp <= NUM_TEAMS)
			oprintf("%dj%dc %d / 29 TEAMS", (HJUST_CENTER|VJUST_CENTER), WHITE, tmp);
		else
			oprintf("%dj%dc ALL TEAMS", (HJUST_CENTER|VJUST_CENTER), WHITE);
	}
}

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
int pick_cpu_team(int defeated, int alt_ladder)
{
	int			tier,i,r;
	int			unbeaten[4],nu=0;
	
	//FIX!!!??? tier # limit may not be correct !
	for (tier=0; (tier < 9) && !nu; tier++)
	{
		// has the guy defeated every team on this tier
		nu = 0;
		for (i=0; team_ladder[tier][i] != -1; i++ )
		{
			if (!(defeated & (1<<team_ladder[tier][i])))
				unbeaten[nu++] = team_ladder[tier][i];
		}
	}

	if (nu) {
		// pick from the unbeaten teams on this tier
		r = unbeaten[randrng(nu)];
		alt_ladder_active = 0;
		ladder_level = tier;
	} else {
		// beat all - pick from the list of super tough teams
		r = beat_all_teams[randrng(4)];
		alt_ladder_active = 1;
		ladder_level = alt_ladder;
		r = TEAM_SECRET;
	}
	return r;
}


//-------------------------------------------------------------------------------------------------
//									This routine returns the player specific button bits
//
// INPUT : player number
// RETURN: players buttons bits
//-------------------------------------------------------------------------------------------------
int	get_but_val_down(int pnum)
{
	int sw = get_player_sw_close();
	if (!four_plr_ver) pnum = index_2p[pnum];
	sw >>=((pnum<<3)+4);
	return(sw & 0xF);		/* A, B, C, D buttons */
}

//-------------------------------------------------------------------------------------------------
//									This routine returns the player specific button bits
//
// INPUT : player number
// RETURN: players buttons bits
//-------------------------------------------------------------------------------------------------
int	get_but_val_cur(int pnum)
{
	int sw = get_player_sw_current();
	if (!four_plr_ver) pnum = index_2p[pnum];
	sw >>=((pnum<<3)+4);
	return(sw & 0xF);		/* A, B, C, D buttons */
}

//-------------------------------------------------------------------------------------------------
//									This routine returns the player specific joystick bits
//
// INPUT : player number
// RETURN: players joystick bits
//-------------------------------------------------------------------------------------------------
int	get_joy_val_down(int pnum)
{
	int sw = get_player_sw_close();
	if (!four_plr_ver) pnum = index_2p[pnum];
	sw >>=(pnum<<3);
	return(sw & 0xF);		/* R, L, D, U buttons */
}

//-------------------------------------------------------------------------------------------------
//									This routine returns the player specific joystick bits
//
// INPUT : player number
// RETURN: players joystick bits
//-------------------------------------------------------------------------------------------------
int	get_joy_val_cur(int pnum)
{
	int sw = get_player_sw_current();
	if (!four_plr_ver) pnum = index_2p[pnum];
	sw >>=(pnum<<3);
	return(sw & 0xF);		/* R, L, D, U buttons */
}

//-------------------------------------------------------------------------------------------------
//									This routine returns the player specific switch bits
//
// INPUT : player number
// RETURN: players switch bits
//-------------------------------------------------------------------------------------------------
int	get_psw_val_down(int pnum)
{
	int sw = get_player_sw_close();
	if (!four_plr_ver) pnum = index_2p[pnum];
	sw >>=(pnum<<3);
	return(sw & 0xFF);		/* switch bits */
}

//-------------------------------------------------------------------------------------------------
//									This routine returns the player specific switch bits
//
// INPUT : player number
// RETURN: players switch bits
//-------------------------------------------------------------------------------------------------
int	get_psw_val_cur(int pnum)
{
	int sw = get_player_sw_current();
	if (!four_plr_ver) pnum = index_2p[pnum];
	sw >>=(pnum<<3);
	return(sw & 0xFF);		/* switch bits */
}

//-------------------------------------------------------------------------------------------------
//
// This function waits for a button press from player
//
// INPUT:  player #, Time to sleep, minimum time to sleep before returning
// OUTPUT: Nothing
//
//-------------------------------------------------------------------------------------------------
void wait_for_button_press(int plyr_num, int min_ticks, int wait_ticks)

{
	while (--min_ticks)
		sleep (1);

	while (--wait_ticks)
	{
		sleep (1);
		if (get_but_val_down(plyr_num))
			break;
	}
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
int count_quarters_purchased(void)
{
	int i;
	int total = 0;

	for (i=0; i < MAX_PLYRS; i++)
		total += pdata_blocks[i].quarters_purchased;
	return(total);
}

//------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
int count_players_in_game(void)
{
	int i;
	int plrs = 0;

	for (i=0; i < MAX_PLYRS; i++)
	{
		if (p_status & (1<<i))
			plrs++;
	}
	return(plrs);
}

//-------------------------------------------------------------------------------------------------
//												This routine changes the IMAGE for a given sprite
//
//   INPUT: ptr. to sprite and image ptr.
// RETURNS: nothing
//-------------------------------------------------------------------------------------------------
void change_img_tmu(sprite_info_t *obj, image_info_t *ii, int tid)
{
	struct texture_node	*tn;

	// Ensure no NULL ptrs were passed
	if (obj && obj->tn && ii && ii->texture_name)
	{
		// Return if same image_info
		if (obj->ii == ii)
			return;

		while (1)
		{
			// Is img in current texture ?
			if (strncmp(obj->tn->texture_name,ii->texture_name,8))
			{
				// No; get new texture ptr
				tn = find_texture(ii->texture_name,tid);

				// Find it ?
				if (!tn)
					// No; break to error
					break;

				obj->tn = tn;
			}
			// Success; do setup & exit
			obj->ii = ii;
			generate_sprite_verts(obj);
			return;
		}
		// Error; tried but texture wasn't there
		fprintf(stderr, "change_img_tmu() couldn't find `%s' in TMU\r\n", ii->texture_name);
#ifdef DEBUG
		lockup();
#endif
		return;
	}
	// Error; bad object and/or image_info ptr
	fprintf(stderr, "change_img_tmu() was passed bogus ptr:\r\n");
	fprintf(stderr, "  obj=%08X  obj->tn=%08X  ii:%08X  ii->texture_name=%08X\r\n",
		(int)(obj), (int)(obj->tn), (int)(ii), (int)(ii->texture_name));
#ifdef DEBUG
	lockup();
#endif
	return;
}

//-------------------------------------------------------------------------------------------------
//															This routines loads all textures in table into TMU ram
//
//   INPUT: ptr. to list of texture names
// RETURNS: 1 if load failed else 0
//-------------------------------------------------------------------------------------------------
int load_textures_into_tmu(tmu_ram_texture_t *tex_tbl)
{
	int error = 0;

	texture_node_t	*tex_node;

	while (tex_tbl->t_name != NULL)
	{
		tex_node = create_texture(tex_tbl->t_name,
									tex_tbl->tex_id,
									0,
									CREATE_NORMAL_TEXTURE,
									GR_TEXTURECLAMP_CLAMP,
									GR_TEXTURECLAMP_CLAMP,
									GR_TEXTUREFILTER_BILINEAR,
									GR_TEXTUREFILTER_BILINEAR);
		// Success?
		if (!tex_node)
		{
			// Tag & audit error, then continue with the list
			increment_audit(TEX_LOAD_FAIL_AUD);
			error = 1;
		}
		tex_tbl++;
	}
	return error;
}

//-------------------------------------------------------------------------------------------------
//	This routines loads the first frame for a movie
//
//   INPUT: ptr. to list of texture names
// RETURNS: 1 if load failed else 0
//-------------------------------------------------------------------------------------------------
//int pre_load_movie_tex(char *movie)
//{
//	texture_node_t	*tex_node1, *tex_node2;
//	char texture_name[16];
//	sprintf(texture_name, "%s1.wms", movie);
//
//	tex_node1 = create_texture(texture_name,
//								2,
//								0,
//								CREATE_NORMAL_TEXTURE,
//								GR_TEXTURECLAMP_CLAMP,
//								GR_TEXTURECLAMP_CLAMP,
//								GR_TEXTUREFILTER_BILINEAR,
//								GR_TEXTUREFILTER_BILINEAR);
//	sprintf(texture_name, "%s2.wms", movie);
//	tex_node2 = create_texture(texture_name,
//									2,
//								0,
//								CREATE_NORMAL_TEXTURE,
//								GR_TEXTURECLAMP_CLAMP,
//								GR_TEXTURECLAMP_CLAMP,
//								GR_TEXTUREFILTER_BILINEAR,
//								GR_TEXTUREFILTER_BILINEAR);
//	if (!tex_node1 || !tex_node2)
//	{
//		increment_audit(TEX_LOAD_FAIL_AUD);
//		return(1);									// no
//	}
//	return(0);
//}  

//---------------------------------------------------------------------------------------------------------------------------
// 	THIS creates the background as a TRUE BACKGROUND OBJECT where erase is turned off, thus the image erases the screen
//---------------------------------------------------------------------------------------------------------------------------
void draw_backdrop(void)
{
	sprite_info_t		*psprite;

	psprite = beginobj(&backgrnd,  		  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	psprite->id = 1;
	psprite = beginobj(&backgrnd_c1,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	psprite->id = 1;
	psprite = beginobj(&backgrnd_c2,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	psprite->id = 1;
	psprite = beginobj(&backgrnd_c3,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	psprite->id = 1;
}

static void load_mugshot_tpage(void *pii_mug)
{
	char *tex_name;
	
	if (pii_mug != NULL) {
		tex_name = ((image_info_t *)pii_mug)->texture_name;
		if (!create_texture(tex_name,
							MUG_TID,
							0,
							CREATE_NORMAL_TEXTURE,
							GR_TEXTURECLAMP_CLAMP,
							GR_TEXTURECLAMP_CLAMP,
						GR_TEXTUREFILTER_BILINEAR,
							GR_TEXTUREFILTER_BILINEAR)) {
			increment_audit(TEX_LOAD_FAIL_AUD);
#ifdef DEBUG
			fprintf(stderr, "could not load mugshot page %s\n", tex_name);
			lockup();
#endif
		}
	}
}	/* load_mugshot_tpage */

//---------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
int joinin_selection(int);
static int joinin_result;

// single argument is player number of buying-in guy
int joinin_selection(int pnum)
{
	sprite_info_t	*sprite_obj;
	int				i,j;
	int				ver;

	ver = four_plr_ver;

	exit_status = 0;
	force_selection = 0;

	// convert pnum to team index of selecting team
	pnum = pnum / NUM_PERTEAM;
	pnum = !pnum;

//	qcreate("timer",TIMER_PID,timedown,5,9,(SPRITE_HRES/2.0f),(SPRITE_VRES/2.0f)-107.0f);
	qcreate("joinin",0,plyr_joinin_proc,pnum,0,0,0);

	sleep(2);			// allow above processes to start

	do
	{
		sleep(1);
	} while (exit_status);

	return (!joinin_result);
}

//-------------------------------------------------------------------------------------------------
#define JI_TITLE_Z		1.0f
#define JI_DIMMER_Z		1.4f
#define JI_BOX_Z		1.3f
#define JI_BUTTON_Z		1.2f
#define JI_HILITE_Z		1.15f
#define JI_BTEXT_Z		1.10f
#define JI_BFLASH2_Z	1.05f
#define JI_BFLASH1_Z	1.0f

void plyr_joinin_proc(int *args)
{
	int				pnum;//,plyrs;
	int				switches,buttons,btn_dly;
	int				option_selected;
	int				ver;
	int				i;
	sprite_info_t 	*psprite,*sprite_obj;
	sprite_info_t 	*cursor_obj = NULL;
	sprite_info_t 	*glow_obj = NULL;
	sprite_info_t 	*glow2_obj = NULL;
	sprite_info_t 	*flash_obj = NULL;
	sprite_info_t 	*flash2_obj = NULL;
	sprite_info_t	*button1_obj = NULL;
	sprite_info_t	*button2_obj = NULL;
	long			alpha_level, delta;
	float			cursr_ys[] = {194.0f,129.0f};
	int				sel_team;

	#ifdef DEBUG
	printf( "---> plyr_joinin_proc starting\n" );
	#endif

	sel_team = args[0];

	// for lack of wanting to type! :)
	ver = four_plr_ver;

	// set variables
	exit_status = 1;
	option_selected = 0;			// no
	option = 0;
	alpha_level = 255;
	delta = -8;

//	plyrs = (ver ? 4 : 2);

	btn_dly = 30;

	// dimmer
	sprite_obj = beginobj(&playresult, SPRITE_HRES/2.0f, SPRITE_VRES/2.0f, JI_DIMMER_Z, JAPPLE_BOX_TID);
	sprite_obj->id = OID_JOININ_BK;
	sprite_obj->w_scale = 3.0f;
	sprite_obj->h_scale = 6.0f;
	generate_sprite_verts(sprite_obj);

	// print title msg.
	sprite_obj = beginobj(&ji_prepar, buyin_title_xys[ver][0][X_VAL], buyin_title_xys[ver][0][Y_VAL], JI_TITLE_Z, JOININ_TID);
	sprite_obj->id = OID_JOININ_BK;
	sprite_obj = beginobj(&ji_prepar_c1, buyin_title_xys[ver][0][X_VAL], buyin_title_xys[ver][0][Y_VAL], JI_TITLE_Z, JOININ_TID);
	sprite_obj->id = OID_JOININ_BK;

	// create the box
	sprite_obj = beginobj(&ji_barblk, buyin_title_xys[ver][1][X_VAL], buyin_title_xys[ver][1][Y_VAL], JI_BOX_Z, JOININ_TID);
	sprite_obj->id = OID_JOININ_BK;
	sprite_obj = beginobj(&ji_barblk_c1, buyin_title_xys[ver][1][X_VAL], buyin_title_xys[ver][1][Y_VAL], JI_BOX_Z, JOININ_TID);
	sprite_obj->id = OID_JOININ_BK;

	// create button texts
	button1_obj = beginobj(&ji_newgam, SPRITE_HRES/2.0f, cursr_ys[0], JI_BTEXT_Z, JOININ_TID);
	button1_obj->id = OID_JOININ_BK;
	button2_obj = beginobj(&ji_join, SPRITE_HRES/2.0f, cursr_ys[1], JI_BTEXT_Z, JOININ_TID);
	button2_obj->id = OID_JOININ_BK;

	// create button hilite & glow
	cursor_obj = beginobj(&ji_barblu, SPRITE_HRES/2.0f, cursr_ys[option], JI_BUTTON_Z, JOININ_TID);
	cursor_obj->id = OID_JOININ_BTN;
	glow_obj = beginobj(&ji_barglo, SPRITE_HRES/2.0f, cursr_ys[option], JI_HILITE_Z, JOININ_TID);
	glow_obj->id = OID_JOININ_BFSH;
	glow2_obj = beginobj(&ji_barglo_c1, SPRITE_HRES/2.0f, cursr_ys[option], JI_HILITE_Z, JOININ_TID);
	glow2_obj->id = OID_JOININ_BFSH;

	// if all drones, let buying-in side choose
	if ((p_status & TEAM_MASK(sel_team)) == 0)
		sel_team = !sel_team;

	// control loop
	while (!option_selected)
	{
		// Read INPUT
		switches = 0;
		buttons = 0;

		// read joysticks from any active player
//		for (i=0; i < plyrs; i++)
		for (i=0; i < MAX_PLYRS; i++)
		{
			if ((p_status & (1<<i) & TEAM_MASK(sel_team)) &&
				(switches = get_joy_val_down(i)))
				break;
		}

		if (btn_dly > 0)
			buttons = 0;
		else
		{
			// read buttons from any active player
//			for (i=0; i < plyrs; i++)
			for (i=0; i < MAX_PLYRS; i++)
			{
				if ((p_status & (1<<i) & TEAM_MASK(sel_team)) &&
					(buttons = get_but_val_down(i)))
					break;
			}
		}

		// if screen times out... default to whatever it's on
		if (force_selection)
			buttons = 1;							// and select it!

		// Handle Joystick
		if (((switches == JOY_UP) || (switches == JOY_LEFT)) && (option > 0))
		{
			option--;

			// move glow obj.
			cursor_obj->y = cursr_ys[option];
			glow_obj->y = cursr_ys[option];
			glow2_obj->y = cursr_ys[option];
			generate_sprite_verts(cursor_obj);
			generate_sprite_verts(glow_obj);
			generate_sprite_verts(glow2_obj);

			snd_scall_bank(cursor_bnk_str,OPTION_CURSR_SND,VOLUME7,127,PRIORITY_LVL2);
		}
		else if (((switches == JOY_DOWN) || (switches == JOY_RIGHT)) && (option < 1))
		{
			option++;

			cursor_obj->y = cursr_ys[option];
			glow_obj->y = cursr_ys[option];
			glow2_obj->y = cursr_ys[option];
			generate_sprite_verts(cursor_obj);
			generate_sprite_verts(glow_obj);
			generate_sprite_verts(glow2_obj);

			snd_scall_bank(cursor_bnk_str,OPTION_CURSR_SND,VOLUME7,127,PRIORITY_LVL2);
		}
		// Handle BUTTONS
		if (buttons)
		{
			snd_scall_bank(cursor_bnk_str,SELECT_SND1,VOLUME7,127,PRIORITY_LVL3);
			option_selected = 1;
		}

		// update glow obj. aphla
		glow_obj->state.constant_color &= 0x00FFFFFF;
		glow_obj->state.constant_color |= alpha_level << 24;
		glow2_obj->state.constant_color &= 0x00FFFFFF;
		glow2_obj->state.constant_color |= alpha_level << 24;

		sleep(1);

		alpha_level += delta;
		if (alpha_level < 128)
		{
			alpha_level = 128;
			delta = +8;
		}
		else if (alpha_level > 255)
		{
			alpha_level = 255;
			delta = -8;
		}

		btn_dly--;

	}// while

	snd_scall_bank(cursor_bnk_str,OPTION_SELECT_SND,VOLUME7,127,PRIORITY_LVL4);

	// flash option
	hide_sprite(glow_obj);
	hide_sprite(glow2_obj);

	flash_obj = beginobj(optscr_flsh_imgs[0],
						 (cursor_obj->x-cursor_obj->ii->ax + (cursor_obj->ii->w / 2.0f))+0.0f,
						 (cursor_obj->y+cursor_obj->ii->ay + (cursor_obj->ii->h / 2.0f))+4.0f,
						 JI_BFLASH2_Z,
						 SPFX_TID);
	flash_obj->id = OID_JOININ;

	flash2_obj = beginobj(optscr_flsh2_imgs[0],
						 (cursor_obj->x-cursor_obj->ii->ax + (cursor_obj->ii->w / 2.0f))+0.0f,
						 (cursor_obj->y+cursor_obj->ii->ay + (cursor_obj->ii->h / 2.0f))+4.0f,
						 JI_BFLASH1_Z,
						 SPFX_TID);
	flash2_obj->id = OID_JOININ;

	flash_obj->w_scale = 2.55f;
	flash_obj->h_scale = 0.70f;

	// set to additive blending
	flash_obj->state.alpha_rgb_dst_function = GR_BLEND_ONE;
	generate_sprite_verts(flash_obj);

	flash2_obj->w_scale = 5.50f;
	flash2_obj->h_scale = 2.00f;

	// set to additive blending
	flash2_obj->state.alpha_rgb_dst_function = GR_BLEND_ONE;
	generate_sprite_verts(flash2_obj);

	for (i=0; i < (sizeof(optscr_flsh_imgs)/sizeof(image_info_t *))-1; i++)
	{
		change_img_tmu(flash_obj,optscr_flsh_imgs[i],SPFX_TID);
		change_img_tmu(flash2_obj,optscr_flsh2_imgs[i],SPFX_TID);
		sleep(2);
	}

	hide_sprite(flash_obj);
	hide_sprite(flash2_obj);

	sleep(25);

	exit_status = 0;
	joinin_result = option;

	del1c(OID_JOININ,0xfffffff0);
}


//-------------------------------------------------------------------------------------------------

static void stick_court(void)
{
	int i, courts[4], court, sw, count;

	if (alt_ladder_active)
		return;
	sleep(15);

	count = 0;
	for (i = 0; i < MAX_PLYRS; i++) {
		court = -1;
		if ((p_status & (1<<i)) && (sw = get_psw_val_cur(i)))
		{
			if (sw & P1_C)
			{
				if (sw & P1_UP)
					court = COURT_INDOOR;
				else if (sw & P1_DOWN)
					court = COURT_INDOOR2;
				else if (sw & P1_LEFT)
					court = COURT_GILMORE;
				else if (sw & P1_RIGHT)
					court = COURT_GEER;
			}
			else
			if ((sw & P1_A) && (sw & P1_B))
			{
				if (sw & P1_UP)
					court = COURT_MIDWAY;
				else if (sw & P1_DOWN)
					court = COURT_NBC;
			}
			if (court != -1)
				courts[count++] = court;
		}
	}
	if (count != 0) {
		which_court = courts[randrng(count)];
		snd_scall_bank(cursor_bnk_str, 57, VOLUME7, 127, PRIORITY_LVL5);
		sleep(30);
	}
}	/* stick_court */

static void jordan_check(void)
{
	player_record_info *player;
	struct stat_plyr_data *data;
	int i, jordans[4], count;
	
	if (pup_tourney)
		return;
	count = 0;
	for (i = 0; i < 4; i++) {
		player = &selected_players[i];
		data = player->player_data;
		if ((player->record_type == CREATP_RECORD) &&
			(player->team == TEAM_SECRET) &&
			(player->player_index == WILLY_M_CHARACTER) &&
			(data->number == 0x23) &&
			(strcmp(data->szlast, "JORDAN")) == 0) {
			jordans[count++] = i;
		}
	}
	if (count != 0) {
		i = jordans[randrng(count)];
		player = &selected_players[i];
		data = player->player_data;
		bump_stat(&data->speed);
		bump_stat(&data->shot);
		bump_stat(&data->dunk);
		bump_stat(&data->power);
	}
}	/* jordan_check */

static void bump_stat(char *stat)
{
	if (*stat < 18) {
		*stat += 5;
		if (*stat > 18)
			*stat = 18;
	}
}	/* bump_stat */

static void pick_cpu_roster(int team, int base_index)
{
	int i, p1_team, p2_team, p1_index, p2_index;
	
	if (alt_ladder_active == 0) {		/* normal NBA team */
		p1_team = p2_team = team;
		if (ladder_level < 6) {			/* select random players */
			do {
				p1_index = randrng(teaminfo[team].num_player);
				p2_index = randrng(teaminfo[team].num_player);
			} while (p1_index == p2_index);
		} else						/* select the best two players */
			find_best_players(team, &p1_index, &p2_index);
	} else							/* special teams */
		find_all_star_team(&p1_team, &p1_index, &p2_team, &p2_index);
	set_player_record(p1_team, p1_index, FALSE, &selected_players[base_index + 0]);
	set_player_record(p2_team, p2_index, FALSE, &selected_players[base_index + 1]);
}	/* pick_cpu_roster */
	
static void find_best_players(int team, int *p1_index, int *p2_index)
{
	struct stat_plyr_data *player;
	int i, rating, p1_rating, p2_rating;
	
	*p1_index = 0;
	*p2_index = 1;
	p1_rating = p2_rating = 0;
	for (i = 0 ; i < teaminfo[team].num_player; i++) {
		player = (struct stat_plyr_data *)&teaminfo[team].p_spd[i];
		rating = (player->speed + player->power + player->rebound + player->shot) * 3;
		rating += player->dunk + player->pass + player->dribble + player->block + player->steal;
		if (rating > p1_rating) {
			*p2_index = *p1_index;
			p2_rating = p1_rating;
			*p1_index = i;
			p1_rating = rating;
		} else if (rating > p2_rating) {
			*p2_index = i;
			p2_rating = rating;
		}
	}
}	/* find_best_players */

typedef struct {
	int p1;
	int p2;
	int court;
	int weather;
} allstar_team;

typedef struct {
	int team;
	int index;
} allstar_info;

static int special_weather = 0;
extern void *spipp, *sonea, *thard, *kbrya, *ghill, *rmill, *kmalo, *grice, *gpayt, *cmull;
extern int pup_thickfog, pup_fog, pup_sfog, pup_nfog, pup_blizzard,	pup_snow, pup_rain;
#define RAIN		1
#define SNOW		2
#define BLIZZARD	4
#define FOG			8
#define THICK_FOG	16
#define NIGHT_FOG	32
#define SWAMP_FOG	64

static allstar_team special_team_info[] = {
	{LIA_CHARACTER, KERRI_CHARACTER, COURT_GILMORE, NIGHT_FOG},
	{HORSE_CHARACTER, PINTO_CHARACTER, COURT_GEER, RAIN},
	{ALIEN_CHARACTER, JAM_CHARACTER, COURT_GILMORE, NIGHT_FOG},
	{OLDMAN_CHARACTER, RETRO_CHARACTER, COURT_GEER, THICK_FOG | RAIN},
	{CLOWN_CHARACTER, PUMPKIN_CHARACTER, COURT_GILMORE, FOG | SNOW},
	{FRANK_CHARACTER, BRIDE_CHARACTER, COURT_GILMORE, NIGHT_FOG | RAIN},
	{CHEERLEADER_LM_CHARACTER, CHEERLEADER_KH_CHARACTER, COURT_GEER, RAIN},
	{NIKKO_CHARACTER, WOLFMAN_CHARACTER, COURT_GILMORE, THICK_FOG},
	{WIZARD_CHARACTER, WIZARD_CHARACTER, COURT_GILMORE, FOG | SNOW},
	{MUMMY_CHARACTER, CREATURE_CHARACTER, COURT_GEER, SWAMP_FOG | RAIN},
	{REF_CHARACTER, ISAIAH_T_CHARACTER, COURT_NBC, 0},
	{NIKKO_CHARACTER, JENNIFER_H_CHARACTER, COURT_GILMORE, NIGHT_FOG | BLIZZARD},
	{SHAWN_L_CHARACTER, TIM_K_CHARACTER, COURT_GEER, NIGHT_FOG},
	{WILLY_M_CHARACTER, CHAD_CHARACTER, COURT_GILMORE, NIGHT_FOG | RAIN},
	{PAULO_G_CHARACTER, BRIAN_L_CHARACTER, COURT_MIDWAY, 0},
	{TIM_M_CHARACTER, DAVE_G_CHARACTER, COURT_MIDWAY, 0},
	{JON_H_CHARACTER, ANDY_E_CHARACTER, COURT_MIDWAY, 0},
	{JASON_S_CHARACTER, JOHN_R_CHARACTER, COURT_MIDWAY, 0},
	{MATT_G_CHARACTER, TIM_B_CHARACTER, COURT_MIDWAY, 0},
	{JEFF_J_CHARACTER, EUGENE_G_CHARACTER, COURT_MIDWAY, 0},
	{SAL_D_CHARACTER, JENNIFER_H_CHARACTER, COURT_MIDWAY, 0},
	{ROB_G_CHARACTER, JIM_G_CHARACTER, COURT_MIDWAY, 0},
	{MARK_G_CHARACTER, DAN_T_CHARACTER, COURT_MIDWAY, 0},
	{MARK_T_CHARACTER, SAL_D_CHARACTER, COURT_MIDWAY, 0}
};

int get_alt_ladder_size(void)
{
	return (sizeof(special_team_info) / sizeof(special_team_info[0])) + 4;
}	/* get_alt_ladder_size */

void find_all_star_team(int *p1_team, int *p1_index, int *p2_team, int *p2_index)
{
	static int mascot1[] = {CHA_MASCOT, CHI_MASCOT, IND_MASCOT, MIN_MASCOT, TOR_MASCOT, HOU_MASCOT};
	static int mascot2[] = {ATL_MASCOT, PHO_MASCOT, NJN_MASCOT, DEN_MASCOT, SEA_MASCOT, UTA_MASCOT};
	static void *nba_player_list[10] = {
		/* Scottie Pippen, Saquel O Neal, Tim Hardaway, Kobie Bryant, Grant Hill */
		/* Reggie Miller, Karl Malone, Glen Rice, Gary Payton, Chris Mullin */
		&spipp, &sonea, &thard, &kbrya, &ghill,
		&rmill, &kmalo, &grice, &gpayt, &cmull
	};
	int i, j, k, index1, index2;
	allstar_info nba_player[10];
	
	special_weather = 0;
	which_court = (cpu_team == 0) ? COURT_INDOOR2 : COURT_INDOOR;
	if (ladder_level < 2) {				/* NBA all stars */
		for (i = 0; i < NUM_TEAMS; i++)
			for (j = 0; j < teaminfo[i].num_player; j++)
				for (k = 0; k < 10; k++) {
					if (teaminfo[i].p_spd[j].pii_mug == nba_player_list[k]) {
						nba_player[k].team = i;
						nba_player[k].index = j;
						break;
					}
				}
		do {
			index1 = randrng(5);
			index2 = randrng(5);
		} while (index1 == index2);
		if (ladder_level == 1) {
			index1 += 5;
			index2 += 5;
		}
		*p1_team = nba_player[index1].team;
		*p1_index = nba_player[index1].index;
		*p2_team = nba_player[index2].team;
		*p2_index = nba_player[index2].index;
	} else {
		*p1_team = TEAM_SECRET;
		*p2_team = TEAM_SECRET;
		if (ladder_level < 4) {			/* all mascot */
			int mascot_count, mascot[6], *ptr;
			int op1, op2;
			
			ptr = (ladder_level == 2) ? mascot1 : mascot2;
			mascot_count = 0;
			if (cpu_team == 0) {
				op1 = 2;
				op2 = 3;
			} else {
				op1 = 0;
				op2 = 1;
			}
			/* do not pick mascots that are already on the other team */
			for (i = 0; i < 6; i++)
				if (((selected_players[op1].team != TEAM_SECRET) || (selected_players[op1].player_index != ptr[i])) &&
					((selected_players[op2].team != TEAM_SECRET) || (selected_players[op2].player_index != ptr[i])))
					mascot[mascot_count++] = ptr[i];
			do {
				index1 = randrng(mascot_count);
				index2 = randrng(mascot_count);
			} while (index1 == index2);
			*p1_index = mascot[index1];
			*p2_index = mascot[index2];
		} else if (ladder_level < get_alt_ladder_size()) {
										/* special */
			i = ladder_level - 4;
			*p1_index = special_team_info[i].p1;
			*p2_index = special_team_info[i].p2;
			which_court = special_team_info[i].court;
			special_weather = special_team_info[i].weather;
		} else {						/* two random great players */
			int tmp, outside;
			
			/* select two teams */
			if (randrng(100) < 50) {
				*p1_team = TEAM_SECRET;
				*p1_index = randrng(TOTAL_MASCOTS);
				*p2_team = randrng(NUM_TEAMS);
				find_best_players(*p2_team, p2_index, &tmp);
			} else {
				*p1_team = randrng(NUM_TEAMS);
				find_best_players(*p1_team, p1_index, &tmp);
				*p2_team = TEAM_SECRET;
				*p2_index = randrng(TOTAL_MASCOTS);
			}
			/* select a court */
			outside = FALSE;
			tmp = randrng(100);
			if (tmp < 33) {
				which_court = COURT_GILMORE;
				outside = TRUE;
			} else if (tmp > 66) {
				which_court = COURT_GEER;
				outside = TRUE;
			}
			/* if outside pick some weather */
			if (outside) {
				if (randrng(100) < 50) {
					tmp = randrng(100);
					if (tmp < 33)
						special_weather |= SNOW;
					else if (tmp > 66)
						special_weather |= RAIN;
					else
						special_weather |= BLIZZARD;
				}
				if (randrng(100) < 50) {
					tmp = randrng(100);
					if (tmp < 33)
						special_weather |= THICK_FOG;
					else if (tmp > 66)
						special_weather |= FOG;
					else
						special_weather |= NIGHT_FOG;
				}
			}
		}
	}
}	/* find_all_star_team */

void force_special_weather(void)
{
	if (alt_ladder_active) {
		pup_rain = !!(special_weather & RAIN);
		pup_snow = !!(special_weather & SNOW);
		pup_blizzard = !!(special_weather & BLIZZARD);
		pup_fog = !!(special_weather & FOG);
		pup_thickfog = !!(special_weather & THICK_FOG);
		pup_nfog = !!(special_weather & NIGHT_FOG);
		pup_sfog = !!(special_weather & SWAMP_FOG);
	}
}	/* force_special_weather */
