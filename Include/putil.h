/*
 *		$Archive: /video/Nba/Include/putil.h $
 *		$Revision: 18 $
 *		$Date: 4/05/99 3:27p $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifndef __PUTIL_H__
#define __PUTIL_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_putil_h = "$Workfile: putil.h $ $Revision: 18 $";
#endif

#include "ani3d.h"
#include "nbacmos.h"
#include "player.h"

enum {
	NO_OTHER_TEAM = -1,
	AWAY_JERSEY,
	HOME_JERSEY,
	ALTERNATE_JERSEY
};

enum {
	NO_UCOLOR = -1,
	BLUE_UCOLOR,
	GREEN_UCOLOR,
	BLACK_UCOLOR,
	PURPLE_UCOLOR,
	RED_UCOLOR,
	TEAL_UCOLOR,
	WHITE_UCOLOR,
	YELLOW_UCOLOR,
	CUSTOM_UCOLOR,
	NUM_UCOLOR
};

#define MIDWAY_UNIFORM_TEAM			(NUM_TEAMS + 1)
#define CUSTOM_UNIFORM_TEAM			NUM_TEAMS
#define DEFAULT_UNIFORM_TEAM		(NUM_TEAMS + 100)

typedef struct {
	char flesh_texture[13];
	char head_texture[13];
	char jersey_texture[13];
	char jnum_texture[13];
	char reflect_texture[13];
} player_textures;

typedef struct {
	int my_team;		/* one of the 29 teams */
	int opp_team;		/* one of the 29 teams, or NO_OTHER_TEAM */
	int jersey;			/* one of the 29 teams, DEFAULT_UNIFORM_TEAM, or CUSTOM_UNIFORM_TEAM */
	int jersey_style;	/* for all but CUSTOM_UNIFORM_TEAM; HOME_JERSEY AWAY_JERSEY, or ALTERNATE_JERSEY */
						/* for CUSTOM_UNIFORM_TEAM: BLUE_UCOLOR, GREEN_UCOLOR, BLACK_UCOLOR, PURPLE_UCOLOR */
						/* RED_UCOLOR, TEAL_UCOLOR, WHITE_UCOLOR, YELLOW_UCOLOR, CUSTOM_UCOLOR */
} player_team;

enum {
	NO_RECORD,					/* No record in CMOS */
	CREATP_RECORD,				/* CMOS record with creatp flag set */
	PLAYER_RECORD,				/* CMOS record without creatp flag set */
	NBA_PLAYER_RECORD,			/* Psuedo record for an individual NBA player, name is city abbr, pin is jnum */
	SECRET_PLAYER_RECORD		/* Psuedo record for a secret character */
};

typedef struct {
	int record_type;						/* which record type this is */
	int team;								/* NBA team/SECRET team */
	int player_index;						/* NBA player index into team roster */
	struct stat_plyr_data *player_data;		/* static data block for NBA_PLAYER_RECORD or SECRET_PLAYER_RECORD */
	int record_num;							/* record number in cmos for CREATP_RECORD or PLAYER_RECORD */
	plyr_record_t cmos_data;				/* cmos record for CREATP_RECORD or PLAYER_RECORD */
	char sound_bank[6];						/* sound bank to load for player */
} player_record_info;

void get_player_textures(player_team *team, int which_team, int which_player, player_textures *tex, int is_creatp);

void load_player_model(int which_model, LIMB ***body_model, LIMB ***jnum_model, LIMB ***reflect_model);
void unload_player_models(void);

void init_player_head_buffer(int index, int num_vertex, int num_st, int num_svert, int num_sinfo);
void load_player_head(int reload, int index, int which_team, int which_player, LIMB **head_model, float *head_scale);
void unload_player_heads(void);

void find_player_record(char *name, char *pin, int team, int which_player, int index, player_record_info *record);
void set_player_record(int team, int which_player, int override_cmos, player_record_info *record);

void calc_player_scale(int is_creatp, int h, int height, int weight, float *xz_scale, float *ay_scale);
float calc_player_head_scale(int is_creatp, int h, plyr_record_t *cmos_data);
float bighead_scale(int team, int player_index);

#endif
