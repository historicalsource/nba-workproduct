/*
 *		$Archive: /video/Nba/base.c $
 *		$Revision: 25 $
 *		$Date: 4/19/99 1:11a $
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
char *ss_pmisc_c = "$Workfile: base.c $ $Revision: 25 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include <stdio.h>
#include <stdlib.h>
#include <goose/goose.h>
#include <math.h>
#include <ctype.h>

/*
 *		USER INCLUDES
 */

#include "include/game.h"
#include "include/player.h"
#include "include/externs.h"
#include "include/audits.h"
#include "include/id.h"
#include "include/putil.h"

extern int p_status;
extern int four_plr_ver, pup_tourney, what_type_shot;
extern int
	p2p_angles[NUM_PLAYERS][NUM_PLAYERS], show_score_plate,
	ally_player, ally_jump_wait,
	tsec, nearest_to_ball(int ),
	nearest_to_me(int , int ),
	nearest_drone_to_ball(int )
	;
extern float odistxz( obj_3d *o1, obj_3d *o2 );
extern float get_dist_to_rim(bbplyr_data *ppd);


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

#define TWO_BASE	64	//58 //63
#define THREE_BASE	48	//42	//37 //48
#define INC_2PT	2
#define INC_3PT	2
int base_2pt[] = {TWO_BASE, TWO_BASE+INC_2PT, TWO_BASE+(INC_2PT*2), TWO_BASE+(INC_2PT*3), TWO_BASE+(INC_2PT*4),
			TWO_BASE+(INC_2PT*5),  TWO_BASE+(INC_2PT*6),  TWO_BASE+(INC_2PT*7),  TWO_BASE+(INC_2PT*8),
			TWO_BASE+(INC_2PT*9),  TWO_BASE+(INC_2PT*10), TWO_BASE+(INC_2PT*11), TWO_BASE+(INC_2PT*12),
			TWO_BASE+(INC_2PT*13), TWO_BASE+(INC_2PT*14), TWO_BASE+(INC_2PT*15), TWO_BASE+(INC_2PT*16),
			TWO_BASE+(INC_2PT*17), TWO_BASE+(INC_2PT*18), TWO_BASE+(INC_2PT*19)};

int base_3pt[] = {THREE_BASE, THREE_BASE+INC_3PT, THREE_BASE+(INC_3PT*2), THREE_BASE+(INC_3PT*3), THREE_BASE+(INC_3PT*4),
			THREE_BASE+(INC_3PT*5),  THREE_BASE+(INC_3PT*6),  THREE_BASE+(INC_3PT*7),  THREE_BASE+(INC_3PT*8),
			THREE_BASE+(INC_3PT*9),  THREE_BASE+(INC_3PT*10), THREE_BASE+(INC_3PT*11), THREE_BASE+(INC_3PT*12),
			THREE_BASE+(INC_3PT*13), THREE_BASE+(INC_3PT*14), THREE_BASE+(INC_3PT*15), THREE_BASE+(INC_3PT*16),
			THREE_BASE+(INC_3PT*17), THREE_BASE+(INC_3PT*18), THREE_BASE+(INC_3PT*19)};

int pt_diff[] =
{
	30, 25, 22, 21, 20, 17, 15, 14, 12, 10, 9, 7, 6, 4, 3,
	0,
	-3, -4, -6, -7, -9, -10, -10, -12, -14, -16, -18, -20, -22, -25, -25,
};

int get_shot_base(bbplyr_data *ppdata)
{
int diff, base, score0, score1, plyr;

	plyr = ppdata->plyrnum;

	score0 = compute_score(game_info.off_side);
	score1 = compute_score(1 - game_info.off_side);

	diff = score0 - score1;
	diff = GREATER(diff, -15);
	diff = LESSER(diff, 15);

	if(pup_tourney)
		diff = 0;

	if(what_type_shot == JUMPER_3)
	{
		base = base_3pt[player_blocks[plyr]._threeshot];
	}
	else
	{
		base = base_2pt[player_blocks[plyr]._shooting];
	}

	base += get_dist_to_rim(ppdata) <= 25.0f ? 15 : 0;

	base += pt_diff[15 + diff];

	return base;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
int stl_base[] = 
#define	ADJ	2
{
	35-ADJ, 37-ADJ, 39-ADJ, 40-ADJ, 41-ADJ, 42-ADJ, 43-ADJ, 44-ADJ, 45-ADJ, 46-ADJ,
	47-ADJ, 48-ADJ, 49-ADJ, 50-ADJ, 51-ADJ, 54-ADJ, 55-ADJ, 56-ADJ, 57-ADJ, 58-ADJ
};

int stl_diff[] =
{
	30, 25, 20, 19, 18, 16, 15, 14, 11, 10, 9, 8, 7, 6, 5,
	0,
	-5, -6, -7, -8, -9, -10, -11, -14, -15, -16, -18, -19, -20, -25, -30,
};

#undef	ADJ

int get_steal_base(bbplyr_data *ppdata)
{
int diff, base, score0, score1, plyr;

	plyr = ppdata->plyrnum;

	score0 = compute_score(1 - game_info.off_side);
	score1 = compute_score(game_info.off_side);

	diff = score0 - score1;
	diff = GREATER(diff, -15);
	diff = LESSER(diff, 15);

	if(pup_tourney)
		diff = 0;

	base = stl_base[ppdata->_stealing];

	base += stl_diff[15 + diff];

	return base;
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
// This is used by the cylinder collision with the ball - passes sometimes bounce off of the defense
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
int keepball_base[] = 
{
	5, 7, 9, 10, 11, 12, 13, 14, 15, 16,
	17, 18, 19, 20, 21, 22, 23, 24, 25, 26
};

int keepball_diff[] =
{
	15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,
	0,
	-1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11, -12, -13, -14, -15,
};

int get_keepball_base(bbplyr_data *ppdata)
{
int diff, base, score0, score1, plyr;

	plyr = ppdata->plyrnum;

	score0 = compute_score(1 - game_info.off_side);
	score1 = compute_score(game_info.off_side);

	diff = score0 - score1;
	diff = GREATER(diff, -15);
	diff = LESSER(diff, 15);

	if(pup_tourney)
		diff = 0;

	base = keepball_base[ppdata->_stealing];

	base += keepball_diff[15 + diff];

	return base;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
// When a ball owner gets knocked down by a pusher
// Called by guy who owns ball
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
int loseball_base[] = 
{
#define	ADJ	5	//10
	// % Chance we lose ball, based on power differential of both players
	85-ADJ, 84-ADJ, 83-ADJ, 82-ADJ, 81-ADJ, 80-ADJ, 79-ADJ, 78-ADJ, 77-ADJ, 76-ADJ,
	75-ADJ, 74-ADJ, 73-ADJ, 72-ADJ, 71-ADJ, 70-ADJ, 69-ADJ, 68-ADJ, 67-ADJ,
	// If power is same, use this middle element for loss %
	65-ADJ,
	// If ball owner is more powerful than pusher, use these elements
	    63-ADJ, 62-ADJ, 57-ADJ, 56-ADJ, 55-ADJ, 53-ADJ, 51-ADJ, 48-ADJ, 45-ADJ,
	44-ADJ, 41-ADJ, 39-ADJ, 36-ADJ, 33-ADJ, 30-ADJ, 25-ADJ, 22-ADJ, 19-ADJ, 15-ADJ
};

int loseball_diff[] =
{
	20, 18, 17, 15, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 2,
	0,
	-2, -4, -5, -6, -7, -8, -9, -10, -12, -13, -14, -15, -17, -18, -20,
};

#undef	ADJ

int get_loseball_base(bbplyr_data *ppdata, bbplyr_data *att)
{
int base_diff, score_diff, base, score0, score1, plyr;

	plyr = ppdata->plyrnum;

	score1 = compute_score(game_info.off_side);
	score0 = compute_score(1 - game_info.off_side);

	score_diff = score0 - score1;	// pusher minus ball owner -5-5
	score_diff = GREATER(score_diff, -15);
	score_diff = LESSER(score_diff, 15);

	base_diff = ppdata->_power - att->_power;
	base_diff = GREATER(base_diff, -19);
	base_diff = LESSER(base_diff, 19);

	if(pup_tourney)
		base_diff = score_diff = 0;

	base = loseball_base[19 + base_diff];

	base += loseball_diff[15 + score_diff];

	return base;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
int swat_base[] = 
{
#define	ADJ	5
	15-ADJ, 17-ADJ, 19-ADJ, 20-ADJ, 21-ADJ, 22-ADJ, 23-ADJ, 24-ADJ, 25-ADJ, 26-ADJ,
	27-ADJ, 28-ADJ, 29-ADJ, 30-ADJ, 33-ADJ, 35-ADJ, 40-ADJ, 41-ADJ, 42-ADJ, 45-ADJ
};

int swat_diff[] =
{
	20, 18, 17, 15, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 2,
	0,
	-2, -4, -5, -6, -7, -8, -9, -10, -12, -13, -14, -15, -17, -18, -20,
};

#undef	ADJ

int get_swat_base(bbplyr_data *ppdata)
{
int diff, base, score0, score1, plyr;

	plyr = ppdata->plyrnum;

	score0 = compute_score(1 - game_info.off_side);
	score1 = compute_score(game_info.off_side);

	diff = score0 - score1;
	diff = GREATER(diff, -15);
	diff = LESSER(diff, 15);

	if(pup_tourney)
		diff = 0;

	base = swat_base[ppdata->_blocking];

	base += swat_diff[15 + diff];

	return base;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
int knockdown_base[] = 
{
	25, 27, 29, 30, 31, 32, 33, 34, 35, 36,
	47, 48, 49, 50, 51, 52, 53, 54, 55, 56
};

int knockdown_diff[] =
{
	20, 18, 17, 15, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 2,
	0,
	-2, -4, -5, -6, -7, -8, -9, -10, -12, -13, -14, -15, -17, -18, -20,
};

int get_knockdown_base(bbplyr_data *ppdata)
{
int diff, base, score0, score1, plyr;

	plyr = ppdata->plyrnum;

	score0 = compute_score(1 - game_info.off_side);
	score1 = compute_score(game_info.off_side);

	diff = score0 - score1;
	diff = GREATER(diff, -15);
	diff = LESSER(diff, 15);

	if(pup_tourney)
		diff = 0;

	base = knockdown_base[ppdata->_power];

	base += knockdown_diff[15 + diff];

	return base;
}
