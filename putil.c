/*
 *		$Archive: /video/Nba/putil.c $
 *		$Revision: 110 $
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
char *ss_putil_c = "$Workfile: putil.c $ $Revision: 110 $";
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <goose\goose.h>

#include "putil.h"

#include "creatp.h"
#include "player.h"
#include "game.h"
#include "nbacmos.h"

#define ARRAY_SIZE(a)				(sizeof(a) / sizeof(a[0]))
#define MIDJ						(char *)-1
#define DEFJ						NULL
#define WJNUM						"SANJNUMA"
#define BJNUM						"SANJNUMH"
#define FIRE						"CUSJRSYK"
#define TIEDYE						"CUSJRSYX"
#define UM							"JRSYMUMY"
		
extern player_record_info selected_players[];

extern int pup_team_jersey;
extern int alt_ladder_active;
extern int texture_load_in_progress;
extern LIMB limb_jn_1, limb_jn_2;
extern LIMB limb_jnah_1, limb_jnah_2;
extern LIMB limb_jnb_1, limb_jnb_2;
extern LIMB limb_jnc_1, limb_jnc_2;
extern LIMB limb_jncb_1, limb_jncb_2;
extern LIMB limb_jncc_1, limb_jncc_2;
extern LIMB limb_jnch_1, limb_jnch_2;
extern LIMB limb_jndr_1, limb_jndr_2;
extern LIMB limb_jnf_1, limb_jnf_2;
extern LIMB limb_jnhb_1, limb_jnhb_2;
extern LIMB limb_jnho_1, limb_jnho_2;
extern LIMB limb_jnj_1, limb_jnj_2;
extern LIMB limb_jnom_1, limb_jnom_2;
extern LIMB limb_jnp_1, limb_jnp_2;
extern LIMB limb_jnpg_1, limb_jnpg_2;
extern LIMB limb_jns_1, limb_jns_2;
extern LIMB limb_jnss_1, limb_jnss_2;
extern LIMB limb_jntr_1, limb_jntr_2;
extern LIMB limb_jnub_1, limb_jnub_2;
extern LIMB limb_jnuf_1, limb_jnuf_2;
extern LIMB limb_jnw_1, limb_jnw_2;
extern LIMB limb_jncl_1, limb_jncl_2;
extern LIMB limb_jnrf_1, limb_jnrf_2;
extern LIMB *hornet_limbs[], *bulls_limbs[], *pacers_limbs[], *timberwolves_limbs[], *raptors_limbs[];
extern LIMB *rockets_limbs[], *hawks_limbs[], *suns_limbs[], *nets_limbs[], *rocky_limbs[], *sonics_limbs[];
extern LIMB *ubear_limbs[], *alien_limbs[], *biggy_smalls_limbs[], *nikko_limbs[], *nana_limbs[];
extern LIMB *retro_limbs[], *oldman_limbs[], *clown_limbs[], *pumpkin_limbs[], *horse_limbs[];
extern LIMB *wizard_limbs[], *frank_limbs[], *mummy_limbs[], *bride_limbs[], *wolfm_limbs[];
extern LIMB *creff_limbs[], *cheerleader_limbs[], *plyr_limbs_rm[], *creature_limbs[];
extern LIMB *hornet_reflect_limbs[], *alien_reflect_limbs[], *pacers_reflect_limbs[];
extern LIMB *timberwolves_reflect_limbs[], *raptors_reflect_limbs[];
extern LIMB *bulls_reflect_limbs[], *biggy_smalls_reflect_limbs[];
extern LIMB *nikko_reflect_limbs[], *rockets_reflect_limbs[];
extern LIMB *hawks_reflect_limbs[], *suns_reflect_limbs[];
extern LIMB *nets_reflect_limbs[], *nana_reflect_limbs[];
extern LIMB *wizard_reflect_limbs[], *oldman_reflect_limbs[];
extern LIMB *sonics_reflect_limbs[], *clown_reflect_limbs[];
extern LIMB *pumpkin_reflect_limbs[], *rocky_reflect_limbs[];
extern LIMB *frank_reflect_limbs[], *mummy_reflect_limbs[];
extern LIMB *bride_reflect_limbs[], *wolfm_reflect_limbs[], *creature_reflect_limbs[];
extern LIMB *horse_reflect_limbs[], *ubear_reflect_limbs[], *plyr_reflect_limbs_rm[];
extern float hotspot_xzs[NUM_HOTSPOTS + 4][2];

typedef struct {
	int away_major_color;
	int away_minor_color;
	int home_major_color;
	int home_minor_color;
	int alt_major_color;
	int alt_minor_color;
} uniform_color;
	
typedef struct {
	void *pvtx;
	void *pst;
	void *strip_info;
	void *strip_verts;
	void *bbox;
} model_data;

typedef struct {
	char name[LETTERS_IN_NAME];
	char pin[PIN_NUMBERS];
	player_textures tex;
} name_pin;

typedef struct {
	LIMB **body_model;
	LIMB **jnum_model;
	LIMB **reflect_model;
	char *reflect_fname;
	char *bod_fname;
} model_info;

typedef struct {
	int percent;
	char *jersey_texture;
	char *jnum_texture;
} special_jersey;

static int str_similiar(char *a, char *b, int len);
static char find_jersey_style(int my_team, int opp_team, int prefered_style);
static char find_jersey_color(player_team *team, int has_custom_color, char **team_name, char *team_style);
static int compare_ucolor(int ucolor1, int ucolor2);
static char ucolor_to_char(int ucolor);
static char ucolor_to_jnum_style(int ucolor);
static void find_nba_jersey(player_textures *tex, player_team *team, int female);
static void get_nba_player_textures(player_team *team, int which_team, int which_player, player_textures *tex);
static void get_hidden_character_textures(player_team *team, int which_character, player_textures *tex, int is_creatp);
static void choose_special_jersey(int count, special_jersey *table, player_textures *tex, player_team *team, player_textures *tdata, int is_female, char team_style);
static void trim_str(char *dest, char *src);
static int limit_weight(int height, int weight);
static struct stat_plyr_data *convert_creatp_to_player(int index, plyr_record_t *cmos_data);

static uniform_color uniform_list[NUM_TEAMS] = {
	/* ATL */
	{RED_UCOLOR, YELLOW_UCOLOR, WHITE_UCOLOR, RED_UCOLOR, NO_UCOLOR, NO_UCOLOR},
	/* BOS */
	{GREEN_UCOLOR, WHITE_UCOLOR, WHITE_UCOLOR, GREEN_UCOLOR, NO_UCOLOR, NO_UCOLOR},
	/* CHA */
	{TEAL_UCOLOR, BLUE_UCOLOR, WHITE_UCOLOR, TEAL_UCOLOR, NO_UCOLOR, NO_UCOLOR},
	/* CHI */
	{RED_UCOLOR, WHITE_UCOLOR, WHITE_UCOLOR, RED_UCOLOR, BLACK_UCOLOR, RED_UCOLOR},
	/* CLE */
	{BLACK_UCOLOR, TEAL_UCOLOR, WHITE_UCOLOR, TEAL_UCOLOR, NO_UCOLOR, NO_UCOLOR},
	/* DAL */
	{BLUE_UCOLOR, WHITE_UCOLOR, WHITE_UCOLOR, BLUE_UCOLOR, NO_UCOLOR, NO_UCOLOR},
	/* DEN */
	{BLUE_UCOLOR, RED_UCOLOR, WHITE_UCOLOR, RED_UCOLOR, NO_UCOLOR, NO_UCOLOR},
	/* DET */
	{TEAL_UCOLOR, BLACK_UCOLOR, WHITE_UCOLOR, TEAL_UCOLOR, NO_UCOLOR, NO_UCOLOR},
	/* GOL */
	{BLUE_UCOLOR, YELLOW_UCOLOR, WHITE_UCOLOR, YELLOW_UCOLOR, NO_UCOLOR, NO_UCOLOR},
	/* HOU */
	{BLUE_UCOLOR, RED_UCOLOR, WHITE_UCOLOR, RED_UCOLOR, NO_UCOLOR, NO_UCOLOR},
	/* IND */
	{BLUE_UCOLOR, YELLOW_UCOLOR, WHITE_UCOLOR, YELLOW_UCOLOR, NO_UCOLOR, NO_UCOLOR},
	/* LAC */
	{RED_UCOLOR, BLUE_UCOLOR, WHITE_UCOLOR, RED_UCOLOR, NO_UCOLOR, NO_UCOLOR},
	/* LAL */
	{PURPLE_UCOLOR, YELLOW_UCOLOR, YELLOW_UCOLOR, PURPLE_UCOLOR, NO_UCOLOR, NO_UCOLOR},
	/* MIA */
	{BLACK_UCOLOR, RED_UCOLOR, WHITE_UCOLOR, RED_UCOLOR, RED_UCOLOR, BLACK_UCOLOR},
	/* MIL */
	{PURPLE_UCOLOR, GREEN_UCOLOR, WHITE_UCOLOR, PURPLE_UCOLOR, GREEN_UCOLOR, PURPLE_UCOLOR},
	/* MIN */
	{BLUE_UCOLOR, GREEN_UCOLOR, WHITE_UCOLOR, GREEN_UCOLOR, BLACK_UCOLOR, GREEN_UCOLOR},
	/* NJN */
	{BLUE_UCOLOR, WHITE_UCOLOR, WHITE_UCOLOR, BLUE_UCOLOR, NO_UCOLOR, NO_UCOLOR},
	/* NYK */
	{TEAL_UCOLOR, BLACK_UCOLOR, WHITE_UCOLOR, TEAL_UCOLOR, NO_UCOLOR, NO_UCOLOR},
	/* ORL */
	{BLUE_UCOLOR, BLACK_UCOLOR, WHITE_UCOLOR, BLACK_UCOLOR, BLACK_UCOLOR, BLUE_UCOLOR},
	/* PHI */
	{BLACK_UCOLOR, RED_UCOLOR, WHITE_UCOLOR, BLACK_UCOLOR, NO_UCOLOR, NO_UCOLOR},
	/* PHO */
	{PURPLE_UCOLOR, YELLOW_UCOLOR, WHITE_UCOLOR, PURPLE_UCOLOR, BLACK_UCOLOR, PURPLE_UCOLOR},
	/* POR */
	{BLACK_UCOLOR, RED_UCOLOR, WHITE_UCOLOR, RED_UCOLOR, NO_UCOLOR, NO_UCOLOR},
	/* SAC */
	{BLACK_UCOLOR, BLUE_UCOLOR, WHITE_UCOLOR, BLUE_UCOLOR, BLUE_UCOLOR, BLACK_UCOLOR},
	/* SAN */
	{BLACK_UCOLOR, WHITE_UCOLOR, WHITE_UCOLOR, BLACK_UCOLOR, NO_UCOLOR, NO_UCOLOR},
	/* SEA */
	{GREEN_UCOLOR, RED_UCOLOR, WHITE_UCOLOR, GREEN_UCOLOR, NO_UCOLOR, NO_UCOLOR},
	/* TOR */
	{PURPLE_UCOLOR, BLACK_UCOLOR, WHITE_UCOLOR, PURPLE_UCOLOR, NO_UCOLOR, NO_UCOLOR},
	/* UTA */
	{PURPLE_UCOLOR, WHITE_UCOLOR, WHITE_UCOLOR, PURPLE_UCOLOR, NO_UCOLOR, NO_UCOLOR},
	/* VAN */
	{GREEN_UCOLOR, RED_UCOLOR, WHITE_UCOLOR, GREEN_UCOLOR, BLACK_UCOLOR, GREEN_UCOLOR},
	/* WAS */
	{BLUE_UCOLOR, BLACK_UCOLOR, WHITE_UCOLOR, BLUE_UCOLOR, NO_UCOLOR, NO_UCOLOR}
};

static name_pin hidden_info[TOTAL_SECRET] = {
	{"HORNET", "1105", {"CHA_HORN", "CHA_HORN", "CHA_HORN", "CHAJNUMA"}},		/* CHA_MASCOT */
	{"BENNY ", "0503", {"FL_BBULL", "CHIJRSYM", "CHIJRSYM", "CHIJNUMH"}},		/* CHI_MASCOT */
	{"BOOMER", "0604", {"FL_BOOME", "HD_BOOME", "INDJRSYM", "INDJNUMH"}},		/* IND_MASCOT */
	{"CRUNCH", "0503", {"FL_CRUNC", "HD_CRUNC", "MINJRSYM", "MINJNUMH"}},		/* MIN_MASCOT */
	{"RAPTOR", "1020", {"FL_RAPTR", "HD_RAPTR", "TORJRSYM", "TORJNUMH"}},		/* TOR_MASCOT */
	{"TURBO ", "1111", {"FL_HBEAR", "HD_HBEAR", "HOUJRSYM", "HOUJNUMA"}},		/* HOU_MASCOT */
	{"HAWK  ", "0322", {"FL_HAWK", "HD_HAWK", "ATLJRSYM", "ATLJNUMH"}},			/* ATL_MASCOT */
	{"GORILA", "0314", {"FL_GORIL", "HD_GORIL", "PHOJRSYM", "PHOJNUMH"}},		/* PHO_MASCOT */
	{"SLY   ", "6765", {"FL_SLY", "HD_SLY", "NJNJRSYM", "NJNJNUMH"}},			/* NJN_MASCOT */
	{"ROCKY ", "0201", {"FL_ROCKY", "HD_ROCKY", "DENJRSYM", "DENJNUMA"}},		/* DEN_MASCOT */
	{"SASQUA", "7785", {"FL_SASQU", "HD_SASQU", "SEAJRSYM", "SEAJNUMA"}},		/* SEA_MASCOT */
	{"BEAR  ", "1228", {"FL_UBEAR", "HD_UBEAR", "UTAJRSYM", "UTAJNUMH"}},		/* UTA_MASCOT */
	{"SMALLS", "0856", {"FL_ALIAN", "FL_ALIAN", "JRSYALI%c", "ALIANJRN"}},		/* ALIEN_CHARACTER */
	{"BIGGY ", "0958", {"FL_JAM", "HD_JAM", "%sJRSY%c", "%sJNUM%c"}},			/* JAM_CHARACTER */
	{"NIKKO ", "6666", {"FL_NIKK%c", "FL_NIKK%c", "FL_NIKK%c", "%sJNUM%c"}},	/* NIKKO_CHARACTER */
	{"LIA   ", "0712", {"FL_LIA", "HD_LIA", "%sJRS%cF", "%sJNUM%c"}},			/* LIA_CHARACTER */
	{"KERRI ", "0220", {"FL_KHOSK", "HD_KHOSK", "%sJRS%cF", "%sJNUM%c"}},		/* KERRI_CHARACTER */
	{"RETRO ", "1970", {"FL_RETRO", "HD_RGATS", "%sJRSY%c", "%sJNUM%c"}},		/* RETRO_CHARACTER */
	{"OLDMAN", "2001", {"FL_OMAN", "HD_OMAN", "%sJRSY%c", "%sJNUM%c"}},			/* OLDMAN_CHARACTER */
	{"CRISPY", "2084", {"FL_CLOWN", "HD_KLOWN", "JRSYCLO%c", "%sJNUM%c"}},		/* CLOWN_CHARACTER */
	{"JACKO ", "1031", {"FL_PUMP", "FL_PUMP", "JRSYPUM%c", "PUMJNUMH"}},		/* PUMPKIN_CHARACTER */
	{"HORSE ", "1966", {"FL_HORSE", "HD_HORSE", "%sJRSY%c", "%sJNUM%c"}},		/* HORSE_CHARACTER */
	{"THEWIZ", "1136", {"FL_WZRD%c", "HD_WZRD%c", "JRSYWIZ%c", "WIZJNUMH"}},	/* WIZARD_CHARACTER */
	{"FRANK ", "1931", {"FL_FRANK", "HD_FRANK", "JRSYFRNK", "SANJNUMA"}},		/* FRANK_CHARACTER */
	{"MUMMY ", "1932", {"FL_MUMMY", "HD_MUMMY", "JRSYMUMY", "SANJNUMA"}},		/* MUMMY_CHARACTER */
	{"BRIDE ", "1935", {"FL_BRIDE", "HD_BRIDE", "JRSBRIDE", "SANJNUMA"}},		/* BRIDE_CHARACTER */
	{"WOLFMN", "1941", {"FL_WOLFM", "HD_WOLFM", "JRSWOLFM", "SANJNUMA"}},		/* WOLFMAN_CHARACTER */
	{"THEREF", "7777", {"FL_CREFF", "HD_CREFF", "JRSYREFF", "%sJNUM%c"}},		/* REF_CHARACTER */
	{"LIA   ", "1111", {"FL_CHLM", "HD_CHLM%c", "JRSYCL_%c", "%sJNUM%c"}},		/* CHEERLEADER_LM_CHARACTER */
	{"KERRI ", "1111", {"FL_CHKH", "HD_CHKH%c", "JRSYCL_%c", "%sJNUM%c"}},		/* CHEERLEADER_KH_CHARACTER */
	{"JENIFR", "1111", {"FL_CHJH", "HD_JHEDR", "JRSYCL_%c", "%sJNUM%c"}},		/* CHEERLEADER_JH_CHARACTER */
	{"PINTO ", "1966", {"FL_HORSP", "HD_HORSP", "%sJRSY%c", "%sJNUM%c"}},		/* PINTO_CHARACTER */
	{"CREATR", "1954", {"FL_CREAT", "HD_CREAT", "JRSYCRET", "SANJNUMA"}},		/* CREATURE_CHARACTER */
	{"******", "****", {"FL_MTURM", "HD_MTURM", "DETJRSYA", "DETJNUMA"}},		/* NBA_PLAYER */
	{"TURMEL", "0322", {"FL_MTURM", "HD_MTURM", "%sJRSY%c", "%sJNUM%c"}},		/* MARK_T_CHARACTER */
	{"GATSON", "1111", {"FL_RGATS", "HD_RGATS", "%sJRSY%c", "%sJNUM%c"}},		/* ROB_G_CHARACTER */
	{"GUIDO ", "6765", {"FL_MGUID", "HD_MGUID", "%sJRSY%c", "%sJNUM%c"}},		/* MARK_G_CHARACTER */
	{"DANIEL", "0604", {"FL_DTHOM", "HD_DTHOM", "%sJRSY%c", "%sJNUM%c"}},		/* DAN_T_CHARACTER */
	{"JAPPLE", "6660", {"FL_JEFFJ", "HD_JEFFJ", "%sJRSY%c", "%sJNUM%c"}},		/* JEFF_J_CHARACTER */
	{"SAL   ", "0201", {"FL_SDIVI", "HD_SDIVI", "%sJRSY%c", "%sJNUM%c"}},		/* SAL_D_CHARACTER */
	{"JENIFR", "3333", {"FL_JHEDR", "HD_JHEDR", "%sJRS%cF", "%sJNUM%c"}},		/* JENNIFER_H_CHARACTER */
	{"E GEER", "1105", {"FL_EGEER", "HD_EGEER", "%sJRSY%c", "%sJNUM%c"}},		/* EUGENE_G_CHARACTER */
	{"MATT G", "1006", {"FL_MGILM", "HD_MGILM", "%sJRSY%c", "%sJNUM%c"}},		/* MATT_G_CHARACTER */
	{"TIMMYB", "3314", {"FL_TBRYA", "HD_TBRYA", "%sJRSY%c", "%sJNUM%c"}},		/* TIM_B_CHARACTER */
	{"GENTIL", "1228", {"FL_JGENT", "HD_JGENT", "%sJRSY%c", "%sJNUM%c"}},		/* JIM_G_CHARACTER */
	{"JONHEY", "8823", {"FL_JHEY", "HD_JHEY", "%sJRSY%c", "%sJNUM%c"}},			/* JON_H_CHARACTER */
	{"LIPTAK", "0114", {"FL_SLIPT", "HD_SLIPT", "%sJRSY%c", "%sJNUM%c"}},		/* SHAWN_L_CHARACTER */
	{"THOMAS", "1111", {"FL_ITHOM", "HD_ITHOM", "DETJRSY%c", "DETJNUM%c"}},		/* ISAIAH_T_CHARACTER */
	{"TIMK  ", "7785", {"FL_TKIZR", "HD_TKIZR", "%sJRSY%c", "%sJNUM%c"}},		/* TIM_K_CHARACTER */
	{"TIMCRP", "6666", {"FL_TMORN", "HD_TMORN", "%sJRSY%c", "%sJNUM%c"}},		/* TIM_M_CHARACTER */
	{"WIL   ", "0101", {"FL_WMORR", "HD_WMORR", "%sJRSY%c", "%sJNUM%c"}},		/* WILLY_M_CHARACTER */
	{"CHAD  ", "0628", {"FL_CHAD", "HD_CHAD", "%sJRSY%c", "%sJNUM%c"}},			/* CHAD_CHARACTER */
	{"CMSVID", "0000", {"FL_CSKRU", "HD_CSKRU", "%sJRSY%c", "%sJNUM%c"}},		/* CHRIS_S_CHARACTER */
	{"BETHAN", "1111", {"FL_BSMUK", "HD_BSMUK", "%sJRS%cF", "%sJNUM%c"}},		/* BETH_S_CHARACTER */
	{"DAVE  ", "1104", {"FL_DGROS", "HD_DGROS", "%sJRSY%c", "%sJNUM%c"}},		/* DAVE_G_CHARACTER */
	{"DIMI  ", "0619", {"FL_JTIAN", "HD_JTIAN", "%sJRSY%c", "%sJNUM%c"}},		/* JIM_T_CHARACTER */
	{"STRAT ", "2112", {"FL_LWOTM", "HD_LWOTM", "%sJRSY%c", "%sJNUM%c"}},		/* LARRY_W_CHARACTER */
	{"LYNCH ", "3333", {"FL_MLYNC", "HD_MLYNC", "%sJRSY%c", "%sJNUM%c"}},		/* MIKE_L_CHARACTER */
	{"JASON ", "3141", {"FL_JSKIL", "HD_JSKIL", "%sJRSY%c", "%sJNUM%c"}},		/* JASON_S_CHARACTER */
	{"PAULO ", "0517", {"FL_PGARC", "HD_PGARC", "%sJRSY%c", "%sJNUM%c"}},		/* PAULO_G_CHARACTER */
	{"GRINCH", "0222", {"FL_BLEBA", "HD_BLEBA", "%sJRSY%c", "%sJNUM%c"}},		/* BRIAN_L_CHARACTER */
	{"LEX   ", "0014", {"FL_AGILL", "HD_AGILL", "%sJRSY%c", "%sJNUM%c"}},		/* LEX_CHARACTER */
	{"CUTLER", "1111", {"FL_GCUTL", "HD_GCUTL", "%sJRSY%c", "%sJNUM%c"}},		/* GREG_C_CHARACTER */
	{"ROOT  ", "6000", {"FL_JROOT", "HD_JROOT", "%sJRSY%c", "%sJNUM%c"}},		/* JOHN_R_CHARACTER */
	{"ELOFF ", "2181", {"FL_AELOF", "HD_AELOF", "%sJRSY%c", "%sJNUM%c"}},		/* ANDY_E_CHARACTER */
	{"STENTR", "0269", {"FL_PMART", "HD_PMART", "%sJRSY%c", "%sJNUM%c"}},		/* PAUL_M_CHARACTER */
	{"RODMAN", "9676", {"FL_DRODM", "HD_DROD%c","%sJRSY%c", "%sJNUM%c"}}		/* RODMAN_CHARACTER */
};

static LIMB *jnums[2] = {&limb_jn_1, &limb_jn_2};
static LIMB *jnums_ah[2] = {&limb_jnah_1, &limb_jnah_2};
static LIMB *jnums_b[2] = {&limb_jnb_1, &limb_jnb_2};
static LIMB *jnums_c[2] = {&limb_jnc_1, &limb_jnc_2};
static LIMB *jnums_cb[2] = {&limb_jncb_1, &limb_jncb_2};
static LIMB *jnums_cc[2] = {&limb_jncc_1, &limb_jncc_2};
static LIMB *jnums_ch[2] = {&limb_jnch_1, &limb_jnch_2};
static LIMB *jnums_dr[2] = {&limb_jndr_1, &limb_jndr_2};
static LIMB *jnums_f[2] = {&limb_jnf_1, &limb_jnf_2};
static LIMB *jnums_hb[2] = {&limb_jnhb_1, &limb_jnhb_2};
static LIMB *jnums_ho[2] = {&limb_jnho_1, &limb_jnho_2};
static LIMB *jnums_j[2] = {&limb_jnj_1, &limb_jnj_2};
static LIMB *jnums_om[2] = {&limb_jnom_1, &limb_jnom_2};
static LIMB *jnums_p[2] = {&limb_jnp_1, &limb_jnp_2};
static LIMB *jnums_pg[2] = {&limb_jnpg_1, &limb_jnpg_2};
static LIMB *jnums_s[2] = {&limb_jns_1, &limb_jns_2};
static LIMB *jnums_ss[2] = {&limb_jnss_1, &limb_jnss_2};
static LIMB *jnums_tr[2] = {&limb_jntr_1, &limb_jntr_2};
static LIMB *jnums_ub[2] = {&limb_jnub_1, &limb_jnub_2};
static LIMB *jnums_uf[2] = {&limb_jnuf_1, &limb_jnuf_2};
static LIMB *jnums_w[2] = {&limb_jnw_1, &limb_jnw_2};
static LIMB *jnums_cl[2] = {&limb_jncl_1, &limb_jncl_2};
static LIMB *jnums_rf[2] = {&limb_jnrf_1, &limb_jnrf_2};

static model_info model_list[TOTAL_MASCOTS + 1] = {
	{hornet_limbs, jnums_ch, hornet_reflect_limbs, "RLG_LLCH", "HORN"},				/* CHA_MASCOT */
	{bulls_limbs, jnums_cb, bulls_reflect_limbs, "RLG_LLCB", "BENNY"},				/* CHI_MASCOT */
	{pacers_limbs, jnums_b, pacers_reflect_limbs, "RLG_LLB", "BOOME"},				/* IND_MASCOT */
	{timberwolves_limbs, jnums_c, timberwolves_reflect_limbs, "RLG_LLC", "CRUNC"},	/* MIN_MASCOT */
	{raptors_limbs, jnums_tr, raptors_reflect_limbs, "RLG_LLTR", "RAPTR"},			/* TOR_MASCOT */
	{rockets_limbs, jnums_hb, rockets_reflect_limbs, "RLG_LLHB", "HBEAR"},			/* HOU_MASCOT */
	{hawks_limbs, jnums_ah, hawks_reflect_limbs, "RLG_LLAH", "HAWK"},				/* ATL_MASCOT */
	{suns_limbs, jnums_pg, suns_reflect_limbs, "RLG_LLPG", "GORIL"},				/* PHO_MASCOT */
	{nets_limbs, jnums_s, nets_reflect_limbs, "RLG_LLS", "SLY"},					/* NJN_MASCOT */
	{rocky_limbs, jnums_dr, rocky_reflect_limbs, "RLG_LLDR", "ROCKY"},				/* DEN_MASCOT */
	{sonics_limbs, jnums_ss, sonics_reflect_limbs, "RLG_LLSS", "SASQU"},			/* SEA_MASCOT */
	{ubear_limbs, jnums_ub, ubear_reflect_limbs, "RLG_LLUB", "UBEAR"},				/* UTA_MASCOT */
	{alien_limbs, jnums, alien_reflect_limbs, "RLG_LLA", "ALIAN"},					/* ALIEN_CHARACTER */
	{biggy_smalls_limbs, jnums_j, biggy_smalls_reflect_limbs, "RLG_LLJ", "JAM"},	/* JAM_CHARACTER */
	{nikko_limbs, NULL, nikko_reflect_limbs, "RLG_LLN", "NIKKO"},					/* NIKKO_CHARACTER */
	{nana_limbs, jnums_f, nana_reflect_limbs, "RLG_LLF", "FEMAL"},					/* LIA_CHARACTER */
	{nana_limbs, jnums_f, nana_reflect_limbs, "RLG_LLF", "FEMAL"},					/* KERRI_CHARACTER */
	{retro_limbs, jnums, plyr_reflect_limbs_rm, "RLG_LLRM", "RETRO"},				/* RETRO_CHARACTER */
	{oldman_limbs, jnums_om, oldman_reflect_limbs, "RLG_LLOM", "OMAN"},				/* OLDMAN_CHARACTER */
	{clown_limbs, jnums_cc, clown_reflect_limbs, "RLG_LLCC", "CLOWN"},				/* CLOWN_CHARACTER */
	{pumpkin_limbs, jnums_p, pumpkin_reflect_limbs, "RLG_LLP", "PUMPK"},			/* PUMPKIN_CHARACTER */
	{horse_limbs, jnums_ho, horse_reflect_limbs, "RLG_LLHO", "HORSE"},				/* HORSE_CHARACTER */
	{wizard_limbs, jnums_w, wizard_reflect_limbs, "RLG_LLW", "WIZRD"},				/* WIZARD_CHARACTER */
	{frank_limbs, jnums_uf, frank_reflect_limbs, "RLG_LLUF", "FRANK"},				/* FRANK_CHARACTER */
	{mummy_limbs, jnums_uf, mummy_reflect_limbs, "RLG_LLUM", "MUMMY"},				/* MUMMY_CHARACTER */
	{bride_limbs, jnums_f, bride_reflect_limbs, "RLG_LLTB", "BRIDE"},				/* BRIDE_CHARACTER */
	{wolfm_limbs, jnums_ub, wolfm_reflect_limbs, "RLG_LLWM", "WOLFM"},				/* WOLFMAN_CHARACTER */
	{creff_limbs, jnums_rf, plyr_reflect_limbs_rm, "RLG_LLRM", "CREFF"},			/* REF_CHARACTER */
	{cheerleader_limbs, jnums_cl, nana_reflect_limbs, "RLG_LLF", "CHEER"},			/* CHEERLEADER_LM_CHARACTER */
	{cheerleader_limbs, jnums_cl, nana_reflect_limbs, "RLG_LLF", "CHEER"},			/* CHEERLEADER_KH_CHARACTER */
	{cheerleader_limbs, jnums_cl, nana_reflect_limbs, "RLG_LLF", "CHEER"},			/* CHEERLEADER_JH_CHARACTER */
	{horse_limbs, jnums_ho, horse_reflect_limbs, "RLG_LLHO", "HORSE"},				/* PINTO_CHARACTER */
	{creature_limbs, jnums_uf, creature_reflect_limbs, "RLG_LLUC", "CFTBL"},		/* CREATURE_CHARACTER */
	{plyr_limbs_rm, jnums, plyr_reflect_limbs_rm, "RLG_LLRM", "RB_PLYR"}			/* NBA_PLAYER */
};

#ifndef DEBUG_PLAYERS
static model_data head_cache[NUM_PLAYERS + 1] = {{NULL, NULL, NULL, NULL, NULL},
												 {NULL, NULL, NULL, NULL, NULL},
												 {NULL, NULL, NULL, NULL, NULL},
												 {NULL, NULL, NULL, NULL, NULL},
												 {NULL, NULL, NULL, NULL, NULL}
};
#else
static model_data head_cache[] = {{NULL, NULL, NULL, NULL, NULL},
								  {NULL, NULL, NULL, NULL, NULL},
								  {NULL, NULL, NULL, NULL, NULL},
								  {NULL, NULL, NULL, NULL, NULL},
								  {NULL, NULL, NULL, NULL, NULL},
								  {NULL, NULL, NULL, NULL, NULL},
								  {NULL, NULL, NULL, NULL, NULL},
								  {NULL, NULL, NULL, NULL, NULL},
								  {NULL, NULL, NULL, NULL, NULL}
};
#endif

static struct stat_plyr_data player_data_cache[NUM_PLAYERS];

static int str_similiar(char *a, char *b, int len)
{
	while (len-- > 0) {
		if (toupper(*a) != toupper(*b))
			return FALSE;
		a++;
		b++;
	}
	return TRUE;
}	/* str_similiar */

static char find_jersey_style(int my_team, int opp_team, int prefered_style)
{
	static char style_list[3] = {'A', 'H', 'X'};
	uniform_color *mt, *ot;
	int m_has_alt, o_has_alt;

	/* requesting a specific style for a team */
	if (opp_team != NO_OTHER_TEAM) {
		mt = &uniform_list[my_team];
		ot = &uniform_list[opp_team];
		m_has_alt = mt->alt_major_color != NO_UCOLOR;
		o_has_alt = ot->alt_major_color != NO_UCOLOR;
		
#if 1
		if ((prefered_style == ALTERNATE_JERSEY) && !m_has_alt)
			prefered_style = AWAY_JERSEY;
#else
		/* if the away jerseys do not clash use them */
		if (compare_ucolor(mt->away_major_color, ot->away_major_color))
			prefered_style = AWAY_JERSEY;
		else {
			if (prefered_style == AWAY_JERSEY) {
				/* switch from away to alternate if possible to allow other team to be away jersey instead of home */
				if (m_has_alt && (compare_ucolor(mt->alt_major_color, ot->away_major_color)))
					prefered_style = ALTERNATE_JERSEY;
			} else if (prefered_style == HOME_JERSEY) {
				if (o_has_alt && (compare_ucolor(ot->alt_major_color, mt->away_major_color)))
					prefered_style = AWAY_JERSEY;
				else if (m_has_alt && (compare_ucolor(mt->alt_major_color, ot->away_major_color)))
					prefered_style = ALTERNATE_JERSEY;
			}
		}
#endif
	}
	return style_list[prefered_style];
}	/* find_jersey_style */

static char find_jersey_color(player_team *team, int has_custom_color, char **team_name, char *team_style)
{
	uniform_color *uniform_data;
	int force_default;
	char color;
		
	force_default = !has_custom_color && (team->jersey == CUSTOM_UNIFORM_TEAM) && (team->jersey_style == CUSTOM_UCOLOR);
	if (force_default || (team->jersey == DEFAULT_UNIFORM_TEAM)) {
		/* this should only happen in creatp mode */
		if (team->opp_team == NO_OTHER_TEAM) {
			*team_name = "SAN";
			*team_style = 'A';
			return 'K';
		}
		uniform_data = &uniform_list[team->my_team];
		*team_name = (char *)teaminfo[team->my_team].szprfx;
		*team_style = find_jersey_style(team->my_team, team->opp_team, force_default ? AWAY_JERSEY : team->jersey_style);
	} else if (team->jersey == MIDWAY_UNIFORM_TEAM) {
		uniform_data = NULL;
		*team_name = "SAN";
		if (team->jersey_style == HOME_JERSEY) {
			*team_style = 'H';
			return 'W';
		} else {
			*team_style = 'A';
			return 'K';
		}
	} else if (team->jersey == CUSTOM_UNIFORM_TEAM) {
		uniform_data = NULL;
		/* this should only happen in creatp mode */
		if (team->opp_team == NO_OTHER_TEAM)
			*team_name = "SAN";
		else
			*team_name = (char *)teaminfo[team->my_team].szprfx;
		*team_style = ucolor_to_jnum_style(team->jersey_style);
	} else {
		uniform_data = &uniform_list[team->jersey];
		*team_name = (char *)teaminfo[team->jersey].szprfx;
		*team_style = find_jersey_style(team->jersey, team->opp_team, team->jersey_style);
	}
	
	if (uniform_data != NULL) 
		switch (*team_style) {
		default:
		case 'A':
			color = ucolor_to_char(uniform_data->away_major_color);
			break;
		case 'H':
			color = ucolor_to_char(uniform_data->home_major_color);
			break;
		case 'X':
			color = ucolor_to_char(uniform_data->alt_major_color);
			break;
		}
	else
		color = ucolor_to_char(team->jersey_style);
	return color;
}	/* find_jersey_color */

static int compare_ucolor(int ucolor1, int ucolor2)
{
	int other_ucolor;
	
	if (ucolor1 == ucolor2)
		return FALSE;
	if ((ucolor1 == BLUE_UCOLOR) || (ucolor2 == BLUE_UCOLOR)) {
		other_ucolor = (ucolor1 == BLUE_UCOLOR) ? ucolor2 : ucolor1;
		if ((other_ucolor == TEAL_UCOLOR) || (other_ucolor == BLACK_UCOLOR) || (other_ucolor == PURPLE_UCOLOR))
			return FALSE;
	}
	return TRUE;
}	/* compare_ucolor */

static char ucolor_to_char(int ucolor)
{
	static char jersey_color[NUM_UCOLOR] = {'B', 'G', 'K', 'P', 'R', 'T', 'W', 'Y', 'X'};	
	
	return jersey_color[ucolor];
}	/* ucolor_to_char */

static char ucolor_to_jnum_style(int ucolor)
{
	if ((ucolor == WHITE_UCOLOR) || (ucolor == YELLOW_UCOLOR))
		return 'H';
	else
		return 'A';
}	/* ucolor_to_jnum_style */

static void find_nba_jersey(player_textures *tex, player_team *team, int female)
{
	char *team_str, *jnum_str;
	char team_style, jnum_style;
	
	if (team->jersey == DEFAULT_UNIFORM_TEAM) {
		/* this should only happen in creatp mode */
		if (team->opp_team == NO_OTHER_TEAM) {
			team_str = "CUS";
			team_style = 'J';
			jnum_str = "SAN";
			jnum_style = 'A';
		} else {
			team_str = (char *)teaminfo[team->my_team].szprfx;
			team_style = find_jersey_style(team->my_team, team->opp_team, team->jersey_style);
			jnum_str = team_str;
			jnum_style = team_style;
		}
	} else if (team->jersey == MIDWAY_UNIFORM_TEAM) {
		jnum_str = "SAN";
		if (female) {
			if (team->jersey_style == HOME_JERSEY) {
				team_str = "CUS";
				jnum_style = 'H';
				team_style = 'Z';
			} else {
				team_str = "MID";
				jnum_style = 'A';
				team_style = 'Y';
			}
		} else {
			if (team->jersey_style == HOME_JERSEY) {
				team_str = "CUS";
				jnum_style = 'H';
				team_style = 'Z';
			} else {
				team_str = "MID";
				jnum_style = 'A';
				team_style = 'M';
			}
		}
	} else if (team->jersey == CUSTOM_UNIFORM_TEAM) {
		team_str = "CUS";
		team_style = ucolor_to_char(team->jersey_style);
		jnum_style = ucolor_to_jnum_style(team->jersey_style);
		/* this should only happen in creatp mode */
		if (team->opp_team == NO_OTHER_TEAM)
			jnum_str = "SAN";
		else
			jnum_str = (char *)teaminfo[team->my_team].szprfx;
	} else {		
		team_str = (char *)teaminfo[team->jersey].szprfx;
		team_style = find_jersey_style(team->jersey, team->opp_team, team->jersey_style);
		jnum_str = team_str;
		jnum_style = team_style;
	}
	if (female)
		sprintf(tex->jersey_texture, "%sJRS%cF", team_str, team_style);
	else
		sprintf(tex->jersey_texture, "%sJRSY%c", team_str, team_style);
	sprintf(tex->jnum_texture, "%sJNUM%c", jnum_str, jnum_style);
}	/* find_nba_jersey */

static void get_nba_player_textures(player_team *team, int which_team, int which_player, player_textures *tex)
{
	int which_model;
	char *player;
	
	player = (char *)teaminfo[which_team].p_spd[which_player].sztext;
	which_model = ((teaminfo[which_team].p_spd[which_player].flags & FEMALE) != 0) ? LIA_CHARACTER : NBA_PLAYER;
	sprintf(tex->flesh_texture, "FL_%s.WMS", player);
	sprintf(tex->head_texture, "HD_%s.WMS", player);
	find_nba_jersey(tex, team, FALSE);
	strcat(tex->jersey_texture, ".WMS");
	strcat(tex->jnum_texture, ".WMS");
	strcpy(tex->reflect_texture, model_list[which_model].reflect_fname);
	strcat(tex->reflect_texture, ".WMS");
}	/* get_nba_player_textures */

static void get_hidden_character_textures(player_team *team, int which_character, player_textures *tex, int is_creatp)
{
	static char rod_table[] = {'B', 'G', 'K', 'L', 'O', 'P', 'R', 'V', 'W', 'Y', 'Z'};
	static int rod_index = 0;
	player_textures *tdata;
	int is_female, has_custom_color, which_model;
	char *team_name;
	char team_style;
	char color;
	
	tdata = &hidden_info[which_character].tex;
	is_female = (which_character == LIA_CHARACTER) || (which_character == KERRI_CHARACTER) || (which_character == JENNIFER_H_CHARACTER) || (which_character == BETH_S_CHARACTER);
	has_custom_color = (which_character == NIKKO_CHARACTER) || (which_character == WIZARD_CHARACTER);
	
	strcpy(tex->flesh_texture, tdata->flesh_texture);
	if (which_character == RODMAN_CHARACTER) {
		sprintf(tex->head_texture, tdata->head_texture, rod_table[rod_index]);
		if (++rod_index == sizeof(rod_table))
			rod_index = 0;
	} else
		strcpy(tex->head_texture, tdata->head_texture);
	strcpy(tex->jersey_texture, tdata->jersey_texture);
	strcpy(tex->jnum_texture, tdata->jnum_texture);
	if (which_character > NBA_PLAYER)
		which_model = ((teaminfo[TEAM_SECRET].p_spd[which_character].flags & FEMALE) != 0) ? LIA_CHARACTER : NBA_PLAYER;
	else
		which_model = which_character;
	strcpy(tex->reflect_texture, model_list[which_model].reflect_fname);
	
	color = find_jersey_color(team, has_custom_color, &team_name, &team_style);
	
	switch (which_character) {
	case ALIEN_CHARACTER:
	case PUMPKIN_CHARACTER:
	case CLOWN_CHARACTER:
		if ((team->jersey == CUSTOM_UNIFORM_TEAM) && (team->jersey_style != BLACK_UCOLOR) &&
			(team->jersey_style != CUSTOM_UCOLOR)) {
			sprintf(tex->jersey_texture, tdata->jersey_texture, color);
			if (which_character == CLOWN_CHARACTER)
				sprintf(tex->jnum_texture, tdata->jnum_texture, team_name, team_style);
		} else
			find_nba_jersey(tex, team, is_female);
		break;
	case NIKKO_CHARACTER:
		/* if not a created player 25% no armband */
		if (!alt_ladder_active && !is_creatp && (randrng(100) <= 25))
			color = 'O';
		sprintf(tex->flesh_texture, tdata->flesh_texture, color);
		sprintf(tex->head_texture, tdata->head_texture, color);
		sprintf(tex->jersey_texture, tdata->jersey_texture, color);
		sprintf(tex->jnum_texture, tdata->jnum_texture, team_name, team_style);
		break;
	case WIZARD_CHARACTER:
		sprintf(tex->flesh_texture, tdata->flesh_texture, color);
		sprintf(tex->head_texture, tdata->head_texture, color);
		sprintf(tex->jersey_texture, tdata->jersey_texture, color);
		break;
	case JAM_CHARACTER:
	case LIA_CHARACTER:
	case KERRI_CHARACTER:
	case RETRO_CHARACTER:
	case OLDMAN_CHARACTER:
	case HORSE_CHARACTER:
	case PINTO_CHARACTER:
	case TIM_B_CHARACTER:
	case MIKE_L_CHARACTER:
	case MARK_T_CHARACTER:
	case SAL_D_CHARACTER:
	case GREG_C_CHARACTER:
	case RODMAN_CHARACTER:
		find_nba_jersey(tex, team, is_female);
		break;
	case CHEERLEADER_LM_CHARACTER:
	case CHEERLEADER_KH_CHARACTER:
	case CHEERLEADER_JH_CHARACTER: {
		char actual_color;
		
		if ((team->jersey == DEFAULT_UNIFORM_TEAM) && (team->opp_team == NO_OTHER_TEAM)) {
			color = 'J';
			actual_color = 'K';
		} else if (!pup_team_jersey && ((which_character == CHEERLEADER_JH_CHARACTER) || (team->jersey == MIDWAY_UNIFORM_TEAM))) {
			if ((which_character != CHEERLEADER_JH_CHARACTER) && (team->jersey_style == HOME_JERSEY)) {
				color = 'N';
				actual_color = 'W';
			} else {
				color = 'M';
				actual_color = 'K';
			}
		} else
			actual_color = color;
		if (which_character != CHEERLEADER_JH_CHARACTER)
			/* only one head texture for CHEERLEADER_JH_CHARACTER */
			sprintf(tex->head_texture, tdata->head_texture, actual_color);
		sprintf(tex->jersey_texture, tdata->jersey_texture, color);
		sprintf(tex->jnum_texture, tdata->jnum_texture, team_name, team_style);
		break;
	}
	case REF_CHARACTER:
		sprintf(tex->jnum_texture, tdata->jnum_texture, team_name, team_style);
		break;
	case ISAIAH_T_CHARACTER:
		if (team_style == 'X')
			team_style = 'A';
		sprintf(tex->jersey_texture, tdata->jersey_texture, team_style);
		sprintf(tex->jnum_texture, tdata->jnum_texture, team_style);
		break;
	case WILLY_M_CHARACTER:
	case PAUL_M_CHARACTER:
	case LEX_CHARACTER:
	case PAULO_G_CHARACTER:
	case BRIAN_L_CHARACTER:
	case JASON_S_CHARACTER:
	case TIM_K_CHARACTER:
	case CHAD_CHARACTER:
	case MATT_G_CHARACTER:
	case JEFF_J_CHARACTER:
	case JON_H_CHARACTER:
	case MARK_G_CHARACTER:
	case ROB_G_CHARACTER:
	case DAN_T_CHARACTER:
	case JIM_G_CHARACTER:
	case CHRIS_S_CHARACTER:
	case EUGENE_G_CHARACTER:
	case JENNIFER_H_CHARACTER:
	case JIM_T_CHARACTER:
	case LARRY_W_CHARACTER:
	case TIM_M_CHARACTER:
	case SHAWN_L_CHARACTER:
	case JOHN_R_CHARACTER:
	case ANDY_E_CHARACTER:		
	case BETH_S_CHARACTER:
	case DAVE_G_CHARACTER: {		
		if (alt_ladder_active || pup_team_jersey || is_creatp)
			find_nba_jersey(tex, team, is_female);
		else {
			static special_jersey andy_e_table[] = {
				{50, "CUSJRSYZ", BJNUM},
				{50, "CLEJRSYA", "CLEJNUMA"}
			};
			static special_jersey brian_l_table[] = {
				{75, "MIDJRSYM", WJNUM},
				{25, UM, WJNUM}
			};
			static special_jersey lex_table[] = {
				{100, FIRE, WJNUM}
			};
			static special_jersey paulo_g_table[] = {
				{100, "MIDJRSYM", WJNUM}
			};
			static special_jersey chad_table[] = {
				{95, DEFJ, DEFJ},
				{5, MIDJ, MIDJ}
			};
			static special_jersey matt_g_table[] = {
				{94, DEFJ, DEFJ},
				{6, MIDJ, MIDJ}
			};
			static special_jersey jeff_j_table[] = {
				{15, FIRE, WJNUM},
				{35, MIDJ, MIDJ},
				{50, UM, WJNUM}
			};
			static special_jersey jon_h_table[] = {
				{33, MIDJ, MIDJ},
				{33, DEFJ, DEFJ},
				{34, FIRE, WJNUM}
			};
			static special_jersey mark_g_table[] = {
				{25, TIEDYE, BJNUM},
				{15, UM, WJNUM},
				{10, DEFJ, DEFJ},
				{25, FIRE, WJNUM},
				{25, MIDJ, MIDJ}
			};
			static special_jersey rob_g_table[] = {
				{4, MIDJ, MIDJ},
				{95, DEFJ, DEFJ},
				{1, FIRE, WJNUM}
			};
			static special_jersey dan_t_table[] = {
				{75, FIRE, WJNUM},
				{25, MIDJ, MIDJ}
			};
			static special_jersey jim_g_table[] = {
				{25, FIRE, WJNUM},
				{50, TIEDYE, BJNUM},
				{25, MIDJ, MIDJ}
			};
			static special_jersey eugene_g_table[] = {
				{50, DEFJ, DEFJ},
				{50, UM, WJNUM}
			};
			static special_jersey jennifer_h_table[] = {
				{100, "MIDJRSYF", WJNUM}
			};
			static special_jersey tim_k_table[] = {
				{100, "CHIJRSYX", "CHIJNUMX"}
			};
			static special_jersey chris_s_table[] = {
				{20, MIDJ, MIDJ},
				{80, "CHIJRSYA", "CHIJNUMA"}
			};
			static special_jersey tim_m_table[] = {
				{100, "CHIJRSYA", "CHIJNUMA"}
			};
			static special_jersey beth_s_table[] = {
				{50, "LALJRS%cF", "LALJNUM%c"},
				{50, "INDJRS%cF", "INDJNUM%c"}
			};
			static special_jersey dave_g_table[] = {
				{100, "NYKJRSY%c", "NYKJNUM%c"}
			};
			static special_jersey larry_w_table[] = {
				{50, "CHIJRSYA", "CHIJNUMA"},
				{50, TIEDYE, BJNUM}
			};
			int table_size;
			special_jersey *table;
			
			switch (which_character) {
			case BRIAN_L_CHARACTER:
				table = brian_l_table;
				table_size = ARRAY_SIZE(brian_l_table);
				break;
			case LEX_CHARACTER:
				table = lex_table;
				table_size = ARRAY_SIZE(lex_table);
				break;
			case PAULO_G_CHARACTER:
				table = paulo_g_table;
				table_size = ARRAY_SIZE(paulo_g_table);
				break;
			case WILLY_M_CHARACTER:
			case PAUL_M_CHARACTER:
			case JASON_S_CHARACTER:
			case TIM_K_CHARACTER:
				table = tim_k_table;
				table_size = ARRAY_SIZE(tim_k_table);
				break;
			case CHAD_CHARACTER:
				table = chad_table;
				table_size = ARRAY_SIZE(chad_table);
				break;
			case MATT_G_CHARACTER:
				table = matt_g_table;
				table_size = ARRAY_SIZE(matt_g_table);
				break;
			case JEFF_J_CHARACTER:
				table = jeff_j_table;
				table_size = ARRAY_SIZE(jeff_j_table);
				break;
			case JON_H_CHARACTER:
				table = jon_h_table;
				table_size = ARRAY_SIZE(jon_h_table);
				break;
			case MARK_G_CHARACTER:
				table = mark_g_table;
				table_size = ARRAY_SIZE(mark_g_table);
				break;
			case ROB_G_CHARACTER:
				table = rob_g_table;
				table_size = ARRAY_SIZE(rob_g_table);
				break;
			case DAN_T_CHARACTER:
				table = dan_t_table;
				table_size = ARRAY_SIZE(dan_t_table);
				break;
			case JIM_G_CHARACTER:
				table = jim_g_table;
				table_size = ARRAY_SIZE(jim_g_table);
				break;
			case CHRIS_S_CHARACTER:
				table = chris_s_table;
				table_size = ARRAY_SIZE(chris_s_table);
				break;
			case EUGENE_G_CHARACTER:
				table = eugene_g_table;
				table_size = ARRAY_SIZE(eugene_g_table);
				break;
			case JENNIFER_H_CHARACTER:
			case JOHN_R_CHARACTER:
				table = jennifer_h_table;
				table_size = ARRAY_SIZE(jennifer_h_table);
				break;
			case ANDY_E_CHARACTER:		
				table = andy_e_table;
				table_size = ARRAY_SIZE(andy_e_table);
				break;
			default:
			case JIM_T_CHARACTER:
			case TIM_M_CHARACTER:
			case SHAWN_L_CHARACTER:
				table = tim_m_table;
				table_size = ARRAY_SIZE(tim_m_table);
				break;
			case BETH_S_CHARACTER:
				table = beth_s_table;
				table_size = ARRAY_SIZE(beth_s_table);
				break;
			case DAVE_G_CHARACTER:
				table = dave_g_table;
				table_size = ARRAY_SIZE(dave_g_table);
				break;
			case LARRY_W_CHARACTER:
				table = larry_w_table;
				table_size = ARRAY_SIZE(larry_w_table);
				break;
			}
			choose_special_jersey(table_size, table, tex, team, tdata, is_female, team_style);
		}
		break;
	}
	default:
		break;
	}
	strcat(tex->flesh_texture, ".WMS");
	strcat(tex->head_texture, ".WMS");
	strcat(tex->jersey_texture, ".WMS");
	strcat(tex->jnum_texture, ".WMS");
	strcat(tex->reflect_texture, ".WMS");
}	/* get_hidden_character_textures */

static void choose_special_jersey(int count, special_jersey *table, player_textures *tex, player_team *team, player_textures *tdata, int is_female, char team_style)
{
	int rnd, i, limit;
	
	for (i = 0, limit = 0; i < count; i++)
		limit += table[i].percent;
	rnd = randrng(limit);
	for (i = 0, limit = 0; i < count; i++) {
		limit += table[i].percent;
		if (rnd <= limit) {
			if (table[i].jersey_texture == DEFJ)
				i = count;
			break;
		}
	}
	
	if (i == count)
		/* use the default jersey */
		find_nba_jersey(tex, team, is_female);
	else {
		if (team_style == 'X')
			team_style = 'A';
		if (table[i].jersey_texture == MIDJ) {
			/* the MIDWAY jersey */
			sprintf(tex->jnum_texture, "SANJNUM%c", team_style);
			if (is_female) {
				if (team_style == 'A')
					strcpy(tex->jersey_texture, "MIDJRSYF");
				else
					strcpy(tex->jersey_texture, "CUSJRSZF");
			} else {
				if (team_style == 'A')
					strcpy(tex->jersey_texture, "MIDJRSYM");
				else
					strcpy(tex->jersey_texture, "CUSJRSYZ");
			}
		} else {
			/* use a special jersey */
			sprintf(tex->jersey_texture, table[i].jersey_texture, team_style);
			sprintf(tex->jnum_texture, table[i].jnum_texture, team_style);
		}
	}
}	/* choose_special_jersey */

static void trim_str(char *dest, char *src)
{
	int i;
	
	memcpy(dest, src, LETTERS_IN_NAME);
	for (i = LETTERS_IN_NAME; i > 0; i--)
		if (src[i - 1] != ' ')
			break;
	dest[i] = '\0';
}	/* trim_str */

static int limit_weight(int height, int weight)
{
	int min_w, max_w;
	
	min_w = height - 7;
	max_w = height + 7;
	if (weight > max_w)
		weight = max_w;
	else if (weight < min_w)
		weight = min_w;
	return weight;
}	/* limit_weight */

static struct stat_plyr_data *convert_creatp_to_player(int index, plyr_record_t *cmos_data)
{
	struct stat_plyr_data *player_data, *parent_data;
	float hs_x, hs_z;
	char height;
	short weight;
	
	player_data = &player_data_cache[index];
	parent_data = (struct stat_plyr_data *)&teaminfo[cmos_data->team].p_spd[cmos_data->player_index];
	creatp_calc_height_weight(cmos_data->height, cmos_data->weight, &height, &weight);
	
	player_data->speed = cmos_data->speed - 1;
	player_data->shot = cmos_data->twop - 1;
	player_data->dunk = cmos_data->dunks - 1;
	player_data->pass = parent_data->pass;
	player_data->dribble = cmos_data->dribble - 1;
	player_data->block = cmos_data->block - 1;
	player_data->steal = cmos_data->steal - 1;
	player_data->power = cmos_data->power - 1;
	player_data->clutch = parent_data->clutch;
	player_data->drone = parent_data->drone;
	creatp_decode_hotspot(cmos_data->hotspot, (cmos_data->privileges & PRIV_HOTSPOT) != 0, &hs_x, &hs_z);
	hotspot_xzs[NUM_HOTSPOTS + index][0] = hs_x;
	hotspot_xzs[NUM_HOTSPOTS + index][1] = hs_z;
	player_data->hotspot = NUM_HOTSPOTS + index;
	player_data->tall = height;
	player_data->number = cmos_data->jersey_num;
	player_data->position = parent_data->position;
	player_data->flags = parent_data->flags;
	player_data->sound = parent_data->sound;
	player_data->f_headsc = calc_player_head_scale(TRUE, cmos_data->height, cmos_data);
	player_data->p_headlm = parent_data->p_headlm;
	player_data->pii_mug = parent_data->pii_mug;
	player_data->pii_mini = parent_data->pii_mini;
	player_data->pii_name = parent_data->pii_name;		/* translate this to player name */
	memcpy(player_data->sztext, parent_data->sztext, sizeof(parent_data->sztext));
	trim_str(player_data->szlast, cmos_data->plyr_name);
	player_data->szfirst[0] = '\0';
	player_data->weight = limit_weight(cmos_data->height, cmos_data->weight) - 1;
	player_data->rebound = cmos_data->threep - 1;
	return player_data;
}	/* convert_cmos_to_data */

void get_player_textures(player_team *team, int which_team, int which_player, player_textures *tex, int is_creatp)
{
	if (which_team == TEAM_SECRET)
		get_hidden_character_textures(team, which_player, tex, is_creatp);
	else
		get_nba_player_textures(team, which_team, which_player, tex);
}	/* get_player_textures */

void load_player_model(int which_model, LIMB ***body_model, LIMB ***jnum_model, LIMB ***reflect_model)
{
	FILE *dat;
	void *ptr;
	int i, nvtx, ntri, nst;
	int num_strip_info;
	int num_strip_verts;
	int num_bbox;
	char datfile[13];
	
	if (which_model > NBA_PLAYER)
		which_model = ((teaminfo[TEAM_SECRET].p_spd[which_model].flags & FEMALE) != 0) ? LIA_CHARACTER : NBA_PLAYER;
	if (model_list[which_model].body_model[0]->pvtx == NULL) {
		/* do not step on other disk loads */
		while (texture_load_in_progress)
			sleep(1);
		texture_load_in_progress = TRUE;
		
		sprintf(datfile, "%s.BOD", model_list[which_model].bod_fname);
		dat = fopen(datfile, "rb");
		if (dat == NULL) {
#ifdef DEBUG
	   		fprintf(stderr,"get_player_model:error opening %s\n", datfile);
			lockup();
#else
			while(1);
#endif
		}
		/* skip file checksum */
		fseek(dat, 4L, SEEK_SET);

		
		for (i = JOINT_PELVIS; i <= JOINT_LANKLE; i++) {
			if (i != JOINT_NECK) {


				/* load the Vertices */
				fread(&nvtx, sizeof(int), 1, dat);
				ptr = malloc(sizeof(VERTEX) * nvtx);
#ifdef DEBUG
				if (ptr == NULL) {
					fprintf(stderr, "could not malloc model vertex list\n");
					lockup();
				}
#endif
				fread(ptr, sizeof(VERTEX), nvtx, dat);
				model_list[which_model].body_model[i]->pvtx = (VERTEX *)ptr;
				
				
				/* load the ST List */
				fread(&nst, sizeof(int), 1, dat);
				ptr = malloc(sizeof(ST) * nst);
#ifdef DEBUG
				if (ptr == NULL) {
					fprintf(stderr, "could not malloc model st list\n");
					lockup();
				}
#endif
				fread(ptr, sizeof(ST), nst, dat);
				model_list[which_model].body_model[i]->pst = (ST *)ptr;
				

				/* load the Strip Info List */
				fread(&num_strip_info, sizeof(int), 1, dat);
				ptr = malloc(sizeof(STRIP_INFO) * num_strip_info);
#ifdef DEBUG
				if (ptr == NULL) {
					fprintf(stderr, "could not malloc Strip Info list\n");
					lockup();
				}
#endif
				fread(ptr, sizeof(STRIP_INFO), num_strip_info, dat);
				model_list[which_model].body_model[i]->strip->strip_info = (STRIP_INFO *)ptr;
				

				/* load the Strip Vertex List */
				fread(&num_strip_verts, sizeof(int), 1, dat);
				ptr = malloc(sizeof(STRIP_VERTS) * num_strip_verts);
#ifdef DEBUG
				if (ptr == NULL) {
					fprintf(stderr, "could not malloc Strip Vertex list\n");
					lockup();
				}
#endif
				fread(ptr, sizeof(STRIP_VERTS), num_strip_verts, dat);
				model_list[which_model].body_model[i]->strip->strip_verts = (STRIP_VERTS *)ptr;


				/* load the Bounding Box List */
				fread(&num_bbox, sizeof(int), 1, dat);
				if (num_bbox > 0) {
					ptr = malloc(sizeof(VERTEX) * num_bbox);
#ifdef DEBUG
					if (ptr == NULL) {
						fprintf(stderr, "could not malloc BBOX list\n");
						lockup();
					}
#endif
					fread(ptr, sizeof(VERTEX), num_bbox, dat);
					model_list[which_model].body_model[i]->bbox = (VERTEX *)ptr;
				}
			}
		}
		fclose(dat);
		texture_load_in_progress = FALSE;
	}
	*body_model = model_list[which_model].body_model;
	*jnum_model = model_list[which_model].jnum_model;
	*reflect_model = model_list[which_model].reflect_model;
}	/* load_player_model */

void unload_player_models(void)
{
	int i, j;
	
	for (i = 0; i <= NBA_PLAYER; i++) {
		for (j = JOINT_PELVIS; j <= JOINT_LANKLE; j++) {
			if (j == JOINT_NECK) {
				model_list[i].body_model[j] = NULL;
			} else {
				/* vertex list */
				if (model_list[i].body_model[j]->pvtx != NULL) {
					free(model_list[i].body_model[j]->pvtx);
					model_list[i].body_model[j]->pvtx = NULL;
				}
				
				/* ST list */
				if (model_list[i].body_model[j]->pst != NULL) {
					free(model_list[i].body_model[j]->pst);
					model_list[i].body_model[j]->pst = NULL;
				}
				
				/* Strip Info list */
				if (model_list[i].body_model[j]->strip->strip_info != NULL) {
					free(model_list[i].body_model[j]->strip->strip_info);
					model_list[i].body_model[j]->strip->strip_info = NULL;
				}
				
				/* Strip Vertex list */
				if (model_list[i].body_model[j]->strip->strip_verts != NULL) {
					free(model_list[i].body_model[j]->strip->strip_verts);
					model_list[i].body_model[j]->strip->strip_verts = NULL;
				}

				/* BBOX list */
				if (model_list[i].body_model[j]->bbox != NULL) {
					free(model_list[i].body_model[j]->bbox);
					model_list[i].body_model[j]->bbox = NULL;
				}
			}
		}
	}
}	/* unload_player_models */

void init_player_head_buffer(int index, int num_vertex, int num_st, int num_svert, int num_sinfo)
{
	int num_bbox;
	
	if (head_cache[index].pvtx != NULL)
		free(head_cache[index].pvtx);
	head_cache[index].pvtx = malloc(sizeof(VERTEX) * num_vertex);
	
	if (head_cache[index].pst != NULL)
		free(head_cache[index].pst);
	head_cache[index].pst = malloc(sizeof(ST) * num_st);

	if (head_cache[index].strip_info != NULL)
		free(head_cache[index].strip_info);
	head_cache[index].strip_info = malloc(sizeof(STRIP_INFO) * num_sinfo);

	if (head_cache[index].strip_verts != NULL)
		free(head_cache[index].strip_verts);
	head_cache[index].strip_verts = malloc(sizeof(STRIP_VERTS) * num_svert);
	
	num_bbox = 8;	
 	if (head_cache[index].bbox != NULL)
 		free(head_cache[index].bbox);
 	head_cache[index].bbox = malloc(sizeof(VERTEX) * num_bbox);
}	/* init_player_head_buffer */

void load_player_head(int reload, int index, int which_team, int which_player, LIMB **head_model, float *head_scale)
{
//	void *ptr;
	LIMB *p_headlm;
	FILE *dat;
//	void *junk_ptr;
//	long position;
	const struct stat_plyr_data *p_spd;
	int nst;
//	int num_strip_info;
//	int num_strip_verts;
	int num_bbox;
	int retry;
	char datfile[13];

	p_spd = &teaminfo[which_team].p_spd[which_player];

	p_headlm    = p_spd->p_headlm;
	*head_scale = p_spd->f_headsc;

	if ((p_headlm->pvtx == NULL) || reload)
	{
		/* do not step on other disk loads */
		while (texture_load_in_progress)
			sleep(1);
		texture_load_in_progress = TRUE;

		sprintf(datfile, "%s.DAT", p_spd->sztext);
		retry = 0;
		while (retry < 3)
		{
			dat = fopen(datfile, "rb");
//!!!
//fclose(dat);
//dat = NULL;
//!!!
			if (dat == NULL)
			{
				if (retry == 0)
				{
					// Increment head load failure
					get_audit(TOTAL_TEAM_FIRE_AUD, &nst);
					nst += 10000;
					set_audit(TOTAL_TEAM_FIRE_AUD,  nst);
					printf("ERROR loading %s\n", datfile);
				}
				// <retry> != 0 will record audits later
				retry++;
				continue;
			}
			break;
		}

		if (dat != NULL)
		{
			/* Skip file checksum */
			fseek(dat, 4L, SEEK_SET);


			/* STRIP_INFO list */
			if (head_cache[index].strip_info == NULL) {
				head_cache[index].strip_info = malloc(sizeof(STRIP_INFO) * p_headlm->strip->num_strip_info);
#ifdef DEBUG
				if (head_cache[index].strip_info == NULL) {
					fprintf(stderr, "could not malloc strip_info list\n");
					lockup();
				}
#endif
			}
			p_headlm->strip->strip_info = head_cache[index].strip_info;
			fread(p_headlm->strip->strip_info, sizeof(STRIP_INFO), p_headlm->strip->num_strip_info, dat);
			p_headlm->strip->strip_info->text = 1;


			/* STRIP_VERTS list */
			if (head_cache[index].strip_verts == NULL) {
				head_cache[index].strip_verts = malloc(sizeof(STRIP_VERTS) * p_headlm->strip->num_strip_verts);
#ifdef DEBUG
				if (head_cache[index].strip_verts == NULL) {
					fprintf(stderr, "could not malloc strip_verts list\n");
					lockup();
				}
#endif
			}
			p_headlm->strip->strip_verts = head_cache[index].strip_verts;
			fread(p_headlm->strip->strip_verts, sizeof(STRIP_VERTS), p_headlm->strip->num_strip_verts, dat);


			/* VERTEX list */
			if (head_cache[index].pvtx == NULL) {
				head_cache[index].pvtx = malloc(sizeof(VERTEX) * p_headlm->nvtx);
#ifdef DEBUG
				if (head_cache[index].pvtx == NULL) {
					fprintf(stderr, "could not malloc head vertex list\n");
					lockup();
				}
#endif
			}
			p_headlm->pvtx = head_cache[index].pvtx;
			fread(p_headlm->pvtx, sizeof(VERTEX), p_headlm->nvtx, dat);


			/* ST texture list */
			fread(&nst, sizeof(int), 1, dat);
			if (head_cache[index].pst == NULL) {
				head_cache[index].pst = malloc(sizeof(ST) * nst);
#ifdef DEBUG
				if (head_cache[index].pst == NULL) {
					fprintf(stderr, "could not malloc head st list\n");
					lockup();
				}
#endif
			}
			p_headlm->pst = head_cache[index].pst;
			fread(p_headlm->pst, sizeof(ST), nst, dat);


			/* BBOX list */
			fread(&num_bbox, sizeof(int), 1, dat);
			if (num_bbox > 0) {
				if (head_cache[index].bbox == NULL) {
					head_cache[index].bbox = malloc(sizeof(VERTEX) * num_bbox);
#ifdef DEBUG
					if (head_cache[index].bbox == NULL) {
						fprintf(stderr, "could not malloc BBOX list\n");
						lockup();
					}
#endif
				}
				p_headlm->bbox = head_cache[index].bbox;
				fread(p_headlm->bbox, sizeof(VERTEX), num_bbox, dat);
			}

			fclose(dat);
		}
		texture_load_in_progress = FALSE;

		// Record any failures
		if (retry != 0)
		{
			// Complete failure?
			if (dat == NULL)
			{
				// Yes; record team & player #'s
				nst = 1000000 * (selected_players[0].team+1) +
					    10000 * (selected_players[0].player_index+1) +
					      100 * (selected_players[1].team+1) +
					        1 * (selected_players[1].player_index+1);
				set_audit(TOTAL_PLAYER_FIRE_AUD, nst);
				nst = 1000000 * (selected_players[2].team+1) +
					    10000 * (selected_players[2].player_index+1) +
					      100 * (selected_players[3].team+1) +
					        1 * (selected_players[3].player_index+1);
				set_audit(AVG_PTS_PLAYER_FIRE_AUD, nst);

				// Flag invalid head
				*head_scale = 0.0f;
			}
			else
			{
				// Increment head load failure recovery
				increment_audit(TOTAL_TEAM_FIRE_AUD);
			}
			write_cmos_to_disk(FALSE);
#ifdef DEBUG
			lockup();
#endif
		}
	}
	*head_model = p_headlm;
}	/* load_player_head */

void unload_player_heads(void)
{
	int i, j;
	
	for (i = 0; i < NUM_PLAYERS + 1; i++) {
		if (head_cache[i].pvtx != NULL) {
			free(head_cache[i].pvtx);
			head_cache[i].pvtx = NULL;
		}
		if (head_cache[i].pst != NULL) {
			free(head_cache[i].pst);
			head_cache[i].pst = NULL;
		}
		if (head_cache[i].strip_info != NULL) {
			free(head_cache[i].strip_info);
			head_cache[i].strip_info = NULL;
		}
		if (head_cache[i].strip_verts != NULL) {
			free(head_cache[i].strip_verts);
			head_cache[i].strip_verts = NULL;
		}
		if (head_cache[i].bbox != NULL) {
			free(head_cache[i].bbox);
			head_cache[i].bbox = NULL;
		}
	}
	
	for (i = 0; i <= TEAM_SECRET; i++) {
		for (j = 0; j < teaminfo[i].num_player; j++) {
			teaminfo[i].p_spd[j].p_headlm->pvtx = NULL;
			teaminfo[i].p_spd[j].p_headlm->ptri = NULL;
			teaminfo[i].p_spd[j].p_headlm->pst = NULL;
			teaminfo[i].p_spd[j].p_headlm->strip->strip_verts = NULL;
			teaminfo[i].p_spd[j].p_headlm->strip->strip_info = NULL;
			teaminfo[i].p_spd[j].p_headlm->bbox = NULL;
		}
	}
}	/* unload_player_heads */

void find_player_record(char *name, char *pin, int team, int which_player, int index, player_record_info *record)
{
	int i, j, pin_num;
	char pin_str[PIN_NUMBERS + 1];
	char name_pin_str[LETTERS_IN_NAME + PIN_NUMBERS + 1];
	char *bank_name;
	
	/* check for an NBA player record */
	for (i = 0, j = 0; i < PIN_NUMBERS; i++)
		if (pin[i] != ' ')
			pin_str[j++] = pin[i];
	pin_str[j] = '\0';
	sscanf(pin_str, "%x", &pin_num);
	
	if ((name[3] == ' ') && (name[4] == ' ') && (name[5] == ' '))
		for (i = 0; i < NUM_TEAMS; i++) {
			if (str_similiar((char *)teaminfo[i].szprfx, name, 3)) {
				for (j = 0; j < teaminfo[i].num_player; j++) {
					if (teaminfo[i].p_spd[j].number == pin_num) {
						record->record_type = NBA_PLAYER_RECORD;
						record->team = i;
						record->player_index = j;
						record->player_data = (struct stat_plyr_data *)&teaminfo[i].p_spd[j];
						record->record_num = -1;
						memset(&record->cmos_data, 0, sizeof(plyr_record_t));
						strcpy(record->sound_bank, record->player_data->sztext);
						return;
					}
				}
				break;
			}
		}
	/* check for a secret player record */
	for (i = 0; i < TOTAL_SECRET; i++) {
		if ((i != NBA_PLAYER) && str_similiar(name, hidden_info[i].name, LETTERS_IN_NAME) && str_similiar(pin, hidden_info[i].pin, PIN_NUMBERS)) {
			record->record_type = SECRET_PLAYER_RECORD;
			record->team = TEAM_SECRET;
			record->player_index = i;
			record->player_data = (struct stat_plyr_data *)&teaminfo[TEAM_SECRET].p_spd[i];
			record->record_num = -1;
			memset(&record->cmos_data, 0, sizeof(plyr_record_t));
			if (record->player_index == PINTO_CHARACTER)
				strcpy(record->sound_bank, "pinto");
			else
				strcpy(record->sound_bank, record->player_data->sztext);
			return;
		}
	}
	for (i = 0; i < LETTERS_IN_NAME; i++)
		name_pin_str[i] = name[i];
	for (j = 0; j < PIN_NUMBERS; j++)
		name_pin_str[i++] = pin[j];
	name_pin_str[i] = '\0';
	i = find_record_in_cmos(name_pin_str);
	if (i != -1) {
		get_generic_record(PLYR_RECS_ARRAY, i, &record->cmos_data);
		record->record_num = i;
		if (record->cmos_data.is_creatp_record) {

			record->record_type = CREATP_RECORD;
			record->team = record->cmos_data.team;
			record->player_index = record->cmos_data.player_index;
			record->player_data = convert_creatp_to_player(index, &record->cmos_data);
			bank_name = creatp_index_to_nickname(record->cmos_data.nickname);
			if (bank_name == NULL) {				
				if (record->player_index == PINTO_CHARACTER)
					bank_name = "pinto";
				else
					bank_name = record->player_data->sztext;
			}			
			strcpy(record->sound_bank, bank_name);
			return;
		} else {
			record->record_type = PLAYER_RECORD;
			record->team = team;
			record->player_index = which_player;
			record->player_data = (struct stat_plyr_data *)&teaminfo[team].p_spd[which_player];
			strcpy(record->sound_bank, record->player_data->sztext);
			return;
		}
	}
	/* no record */
	record->record_type = NO_RECORD;
	record->team = team;
	record->player_index = which_player;
	record->player_data = (struct stat_plyr_data *)&teaminfo[team].p_spd[which_player];
	record->record_num = -1;
	memset(&record->cmos_data, 0, sizeof(plyr_record_t));
	strcpy(record->sound_bank, record->player_data->sztext);
}	/* find_player_record */

void set_player_record(int team, int which_player, int override_cmos, player_record_info *record)
{
	if (team == TEAM_SECRET)
		record->record_type = SECRET_PLAYER_RECORD;
	else
		record->record_type = NBA_PLAYER_RECORD;
	record->team = team;
	record->player_index = which_player;
	record->player_data = (struct stat_plyr_data *)&teaminfo[team].p_spd[which_player];
	if ((team == TEAM_SECRET) && (which_player == PINTO_CHARACTER))
		strcpy(record->sound_bank, "pinto");
	else
		strcpy(record->sound_bank, record->player_data->sztext);
	if (override_cmos) {
		record->record_num = -1;
		memset(&record->cmos_data, 0, sizeof(plyr_record_t));
	}
}	/* set_player_record */

/* height is in INCHES, weight is in the range of 1 to 20 */
void calc_player_scale(int is_creatp, int h, int height, int weight, float *xz_scale, float *ay_scale)
{
	float delta;
	
	if (is_creatp)
		weight = limit_weight(h, weight);
	delta = ((18 - 12) / 15.0f) / 20.0f;
	*xz_scale = (delta * weight) + (12.0f / 15.0f);
	*ay_scale = height / 78.0f;
}	/* calc_player_scale */

float calc_player_head_scale(int is_creatp, int h, plyr_record_t *cmos_data)
{
	struct stat_plyr_data *parent_data;
	float orig_xz_scale, orig_ay_scale,	xz_scale, ay_scale, delta;
	
	parent_data = (struct stat_plyr_data *)&teaminfo[cmos_data->team].p_spd[cmos_data->player_index];
	calc_player_scale(FALSE, 0, parent_data->tall, parent_data->weight, &orig_xz_scale, &orig_ay_scale);
	calc_player_scale(is_creatp, h, cmos_data->height, cmos_data->weight, &xz_scale, &ay_scale);
	delta = (((xz_scale - orig_xz_scale) / orig_xz_scale) * parent_data->f_headsc);
	if (delta < 0.0F)
		delta = 0.0F;
	return parent_data->f_headsc + delta;
}	/* calc_player_head_scale */

float bighead_scale(int team, int player_index)
{
	float scale;
	
	scale = 1.5f;
	if (team == TEAM_SECRET) {
		switch (player_index) {
		case CHA_MASCOT:
		case CHI_MASCOT:
		case IND_MASCOT:
		case MIN_MASCOT:
		case NJN_MASCOT:
		case DEN_MASCOT:
		case TOR_MASCOT:
		case HOU_MASCOT:
		case ATL_MASCOT:
		case SEA_MASCOT:
		case UTA_MASCOT:
		case ALIEN_CHARACTER:
		case NIKKO_CHARACTER:
		case PUMPKIN_CHARACTER:
		case HORSE_CHARACTER:
		case PINTO_CHARACTER:
			scale = 1.25f;
			break;
		}
	}
	return scale;
}	/* bighead_scale */
