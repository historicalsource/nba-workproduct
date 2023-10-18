/*
 *		$Archive: /video/Nba/pmisc.c $
 *		$Revision: 39 $
 *		$Date: 4/18/99 4:16p $
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
char *ss_pmisc_c = "$Workfile: pmisc.c $ $Revision: 39 $";
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
//#include "include/handicap.h"
//#include "anim/gen_seq.dbg"

#define DEBUG_SOLVEQUAD

extern int p_status;
extern int four_plr_ver;
extern int juke_flail_allow, ignore_pass, ignore_steal, ignore_shoot, what_type_shot;

extern char *seq_names[];
extern int OOBPasser;

extern int
	p2p_angles[NUM_PLAYERS][NUM_PLAYERS], show_score_plate,
			ally_player, ally_jump_wait,
	IsShotGood,
	tsec, nearest_to_ball(int ),
	nearest_to_me(int , int ),
	nearest_drone_to_ball(int )
	;
extern float odistxz( obj_3d *o1, obj_3d *o2 );

#ifdef DEBUG
///////////////////////////////////////////////////////////////////////////////
char *limb_names[] = {
		"PELVIS",
		"TORSO",
		"RSHOULDER",
		"RELBOW",
		"RWRIST",
		"LSHOULDER",
		"LELBOW",
		"LWRIST",
		"NECK",
		"RHIP",
		"RKNEE",
		"RANKLE",
		"LHIP",
		"LKNEE",
		"LANKLE"
};

///////////////////////////////////////////////////////////////////////////////
char *animode_names[] = { "MODE_END", "MODE_UNINT", "MODE_TWOPART",
		"MODE_INTSTREAM", "MODE_INTXITION", "MODE_ROTONLY", "MODE_ZERO",
		"MODE_REACTING", "MODE_CANBLOCK", "MODE_NOCYL", "MODE_STIFF_ARM",
		"MODE_NOCATCH", "UNDEFINED", "UNDEFINED", "UNDEFINED", "UNDEFINED",
		"MODE_ANCHORY", "MODE_ANCHORXZ", "MODE_SECONDARY", "MODE_INTERP",
		"MODE_ANIDEBUG", "MODE_TURNHACK", "MODE_PUPPETEER", "MODE_PUPPET",
		"UNDEFINED", "UNDEFINED", "UNDEFINED", "UNDEFINED", "UNDEFINED",
		"UNDEFINED", "UNDEFINED", "UNDEFINED", NULL };

///////////////////////////////////////////////////////////////////////////////
char *game_state_names[] = {
		"GS-DIAGNOSTICS",
		"GS-ATTRACT-MODE",
		"GS-PRE-GAME",
		"GS-GAME-MODE",
		"GS-EOQ",
		"GS-GAME-OVER",
		"GS-FREE-PRICE",
		"GS-HALF-PRICE",

		"GS-EXTRA1","GS-EXTRA2","GS-EXTRA3",NULL
};

///////////////////////////////////////////////////////////////////////////////
char *game_mode_names[] = {
		"GM-PRE-TIPOFF,",
		"GM-TIPOFF,",
		"GM-IN-PLAY,",
		"GM-INBOUNDING,",
		"GM-BALL-OUT,",
		"GM-GAME-OVER,",
		"GM-INITIALIZING",
		"GM-FREE-THROW",

		"GM-EXTRA1","GM-EXTRA2","GM-EXTRA3",NULL
};

///////////////////////////////////////////////////////////////////////////////
char *ball_mode_names[] = {
		"BM-OFF-INTO-PLAY",
		"BM-FREE-ON-COURT",
		"BM-DRIBBLING",
		"BM-PASSING",
		"BM-SHOT-RELEASE",
		"BM-SHOOTING",
		"BM-BOUNCE-ON-RIM",
		"BM-TIPOFF",

		"BM-EXTRA1","BM-EXTRA2","BM-EXTRA3",NULL
};

///////////////////////////////////////////////////////////////////////////////
char *ball_flag_names[] = {
		"BF-WHITE",
		"BF-BOUNCEPASS",
		"BF-NOSTEAL",
		"BF-NOINTERCEPT",

		"BF-EXTRA1","BF-EXTRA2","BF-EXTRA3",NULL
};

///////////////////////////////////////////////////////////////////////////////
char *player_mode_names[] = {
		"MODE-CHILL",
		"MODE-OFFENSE",
		"MODE-DEFENSE",
		"MODE-OUT-OF-BOUNDS",
		"MODE-TIPOFF",
		"MODE-FREETHROW",
		"MODE-END-OF-GAME",
		"MODE-END-OF-QTR",

		"MODE-EXTRA1","MODE-EXTRA2","MODE-EXTRA3",NULL
};

///////////////////////////////////////////////////////////////////////////////
char *player_flag_names[] = {
		"PF-REBOUND",
		"PF-ONSCREEN",
		"PF-FLASHRED",
		"PF-TURBO",
		"PF-OLD_TURBO",
		"PF-NOCOLL",
		"PF-ANITICK",
		"PF-INBND",
		"PF-CELEBRATE",
		"PF-SHOOTING",
		"PF-PASSING",
		"PF-FLASHWHITE",
		"PF-DUNKER",
		"PF-ALLYDUNKER",
		"PF-SWATING",
		"PF-LAYUP",
		"PF-FUNKY_DRIBBLE",
		"PF-STEALING",
		"PF-CATCHING",
		"PF-BOTH_HANDS",
		"PF-NO_LOOK",
		"PF-IN_AIR_BLOCK",
		"PF-QUICK_REBOUND",

		"PF-EXTRA1","PF-EXTRA2","PF-EXTRA3",NULL
};

///////////////////////////////////////////////////////////////////////////////
//char *game_flag_names[] = { "GF_REC_IN_EZ", "GF_LEFT_EZ", "GF_THROUGH_UPRIGHTS",
//		"GF_POSS_CHANGE", "GF_BOBBLE", "GF_FD_LINE_CROSSED", "GF_FWD_PASS",
//		"GF_PASS_THROWN", "GF_BALL_PUNTED", "GF_BALL_KICKED", "GF_PLAY_ENDING",
//		"GF_FUMBLE", "GF_QUICK_KICK", "GF_AIR_CATCH", "GF_BALL_CROSS_LOS",
//		"GF_HIKING_BALL", "GF_ONSIDE_KICK", "GF_OLS_SPEEDS_SET",
//		"GF_OPENING_KICK", NULL };
//
///////////////////////////////////////////////////////////////////////////////
//char *play_type_names[] = { "PT_NORMAL", "PT_KICKOFF", "PT_FIELD_GOAL", "PT_PUNT",
//		"PT_EXTRA_POINT" };
//
/////////////////////////////////////////////////////////////////////////////////
//char *play_end_cause_names[] = { "PEC_CAR_OOB", "PEC_CAR_TACKLED_IB",
//		"PEC_CAR_TACKLED_OWNEZ", "PEC_CAR_TOUCHDOWN", "PEC_DEAD_BALL_IB",
//		"PEC_BALL_OOB" };
//
/////////////////////////////////////////////////////////////////////////////////
//char *down_names[] = { "-1", "0", "1st", "2nd", "3rd", "4th" };

//////////////////////////////////////////////////////////////////////////////
// Shot types:  Declared in game.h
char *w_t_s[] = {"JUMPER_2", "JUMPER_3", "DUNK", "LAYUP" };
//////////////////////////////////////////////////////////////////////////////
void dump_player_state(int player)
{
	bbplyr_data	*ppdata;
	obj_3d		*pobj;
	char *mode_names[] = {"CHILL", "OFFENSE", "DEFENSE","OUT OF BOUNDS","TIP OFF"};

	ppdata = player_blocks + player;
	pobj = (obj_3d *)ppdata;

}

void dump_game_state( int mode )
{
	int i;

	fprintf(stderr, "\ngame_info:\n" );
	fprintf(stderr, " game_mode = %s\n", game_mode_names[game_info.game_mode] );
	fprintf(stderr, " game_off_side = %d !game_off_side %d\n", game_info.off_side, !game_info.off_side);
	fprintf(stderr, " game_quarter = %d\n", game_info.game_quarter);
	fprintf(stderr, " active goal = %d\n", game_info.active_goal );
	fprintf(stderr, " ball_handler = %d(%2x)\n", game_info.ball_handler, (game_info.ball_handler == -1 ) ? 0xFF : JERSEYNUM(game_info.ball_handler) );
	fprintf(stderr, " fire_player = %d, fire_cnt = %d\n",game_info.fire_player, game_info.fire_cnt);
	fprintf(stderr, " team_fire = %d, tm1_fire_cnt = %d  tm2_fire_cnt = %d ",game_info.team_fire, game_info.tm_fire_cnt[0], game_info.tm_fire_cnt[1]);
	fprintf(stderr, " IsShotGood = %d\n",IsShotGood);
	fprintf(stderr, " passer = %d, sc_scored = %d\n", game_info.passer, game_info.sc_scored);
	fprintf(stderr, " shooter = %d, ball_obj.who_shot = %d\n", game_info.shooter, ball_obj.who_shot);
	fprintf(stderr, 
#if NUM_PERTEAM > 2
		" receiver_a = %d, receiver_b = %d,"
#endif
		" ball.int_receiver %d\n",
#if NUM_PERTEAM > 2
		game_info.receiver_a, game_info.receiver_b,
#endif
		ball_obj.int_receiver );

	fprintf(stderr, " game_quarter: %d  game_time: %d:%d:%d\n", game_info.game_quarter,
		((game_info.game_time>>16) & 0xff),
		((game_info.game_time>>8) & 0xff),
		(game_info.game_time & 0xff));
	fprintf(stderr, " shot clock: %d:%d\n",
		((game_info.shotclock>>8) & 0xff), (game_info.shotclock & 0xff));
	fprintf(stderr, " ball mode = %s\n", ball_mode_names[game_info.ball_mode - BM_OFF_INTO_PLAY] );
	if (((TEAM_MASK(game_info.off_side) & p_status) == TEAM_MASK(game_info.off_side)) && (NUM_PERTEAM == 2))
		fprintf(stderr, " offense: all humans\n" );
	else
		fprintf(stderr, " offense: nearest drone to ball = %d\n", nearest_drone_to_ball(game_info.off_side));
	if (((TEAM_MASK(1 - game_info.off_side) & p_status) == TEAM_MASK(1 - game_info.off_side)) && (NUM_PERTEAM == 2))
		fprintf(stderr, " defense: all humans\n" );
	else
		fprintf(stderr, " defense: nearest drone to ball = %d\n", nearest_drone_to_ball(1 - game_info.off_side));
	fprintf(stderr, " plyr_control: 0:%d 1:%d 2:%d 3:%d 4:%d 5:%d \n", game_info.plyr_control[0], game_info.plyr_control[1], game_info.plyr_control[2], game_info.plyr_control[3], game_info.plyr_control[4], game_info.plyr_control[5]);
	fprintf(stderr, " show_score_plate %d, tsec %d\n", show_score_plate, tsec);
	fprintf(stderr, " ally_player %d, ally_jump_wait %d\n", ally_player, ally_jump_wait);

	if( 1 )
	{
		fprintf( stderr, "\n ball position: %f %f %f\n",
			ball_obj.odata.x, ball_obj.odata.y, ball_obj.odata.z );
		fprintf( stderr, " ball velocity: %f %f %f\n",
			ball_obj.odata.vx, ball_obj.odata.vy, ball_obj.odata.vz );
		fprintf( stderr, " ball target: %f %f\n",
			ball_obj.tx, ball_obj.tz );
		fprintf( stderr, " ball_obj.flight_time  total_time: %d %d\n",
			ball_obj.flight_time, ball_obj.total_time );
		if(ball_obj.flags & BF_NOINTERCEPT)
			fprintf(stderr, " ball_obj.flags has BF_NOINTERCEPT\n");
		if(ball_obj.flags & BF_WHITE)
			fprintf(stderr, " ball_obj.flags has BF_WHITE\n");
	}
	fprintf(stderr, " OOBPasser = %d, juke_flail_allow = %d\n ignore_steal = %d, ignore_pass = %d, ignore_shoot = %d\n", OOBPasser, juke_flail_allow, ignore_steal, ignore_pass, ignore_shoot );
	fprintf(stderr, " what_type_shot = %s\n", w_t_s[what_type_shot]);

	fprintf( stderr, "\n" );
}

//////////////////////////////////////////////////////////////////////////////
void print_buttons( int but );
void dump_anim_data2( int player )
{
	int			i,j;
	bbplyr_data	*ppdata;
	obj_3d		*pobj;
	char *att[] = {"NONE","SWAT","DUNK","RUN","ELBOWS","PUSH"};

	fprintf( stderr, "\n" );
//	for( j = 0; j < 6; j++ )
//	{
		ppdata = player_blocks + player;
		pobj = (obj_3d *)ppdata;
		if( pobj->adata[0].animode & MODE_INTSTREAM )
		{
			fprintf( stderr, "\n%2x: %s %s - %6s %9s:%5.2f",
					ppdata->p_spd->number,
					ppdata->p_spd->szfirst,
					ppdata->p_spd->szlast,
					player_mode_names[pobj->plyrmode]+5,
					seq_names[pobj->adata[0].seq_index], pobj->adata[0].fframe );
		}
		else if( pobj->adata[0].animode & MODE_INTERP )
		{
			fprintf( stderr, "\n%2x: %s %s - %6s %9s:%2d to %10s:%2d (%f)",
					ppdata->p_spd->number,
					ppdata->p_spd->szfirst,
					ppdata->p_spd->szlast,
					player_mode_names[pobj->plyrmode]+5,
					seq_names[pobj->adata[0].seq_index2], pobj->adata[0].iframe,
					seq_names[pobj->adata[2].seq_index], pobj->adata[2].iframe,
					(float)(pobj->adata[0].icnt) / (float)(pobj->adata[2].icnt) );
		}
		else
		{
			fprintf( stderr, "\n%2x: %s %s - %6s %9s:%-5d",
					ppdata->p_spd->number,
					ppdata->p_spd->szfirst,
					ppdata->p_spd->szlast,
					player_mode_names[pobj->plyrmode]+5,
					seq_names[pobj->adata[0].seq_index], pobj->adata[0].iframe );
		}

		if( pobj->adata[1].animode & MODE_TWOPART )
		{
			fprintf( stderr, "           %9s:%2d\n",
				seq_names[pobj->adata[1].seq_index], pobj->adata[1].iframe );
		}

		if( pobj->adata[0].animode )
		{
			fprintf( stderr, "  animode: " );
			for( i = 0; animode_names[i]; i++ )
			{
				if( pobj->adata[0].animode & (1<<i))
					fprintf( stderr, " %s", animode_names[i]+5 );
			}
			fprintf( stderr, "\n" );
		}

		if( pobj->adata[1].animode & MODE_TWOPART )
		{
			if( pobj->adata[1].animode )
			{
				fprintf( stderr, "  animode: " );
				for( i = 0; animode_names[i]; i++ )
				{
					if( pobj->adata[1].animode & (1<<i))
						fprintf( stderr, " %s", animode_names[i]+5 );
				}
				fprintf( stderr, "\n" );
			}
		}

		if( pobj->flags )
		{
			fprintf( stderr, "  plrflgs: " );
			for( i = 0; player_flag_names[i]; i++ )
			{
				if( pobj->flags & (1<<i))
				{
					fprintf( stderr, " %s", player_flag_names[i]+3 );
					//if( (1<<i) == PF_SPEED_OVERRIDE )
					//	fprintf( stderr, "(%4.2f)", player_blocks[j].speed );
				}
			}
		}
		fprintf( stderr, "\n x,y,z: %3.3f %3.3f %3.3f  vx,vy,vz: %3.3f %3.3f %3.3f xzvel %3.3f\n", pobj->x, pobj->y, pobj->z, pobj->vx, pobj->vy, pobj->vz, fsqrt((pobj->vx * pobj->vx) + (pobj->vz * pobj->vz)) );

		fprintf( stderr, " fwd %d, tgt_fwd %d, fwd2 %d, tgt_fwd2 %d\n",
			pobj->fwd, pobj->tgt_fwd, pobj->fwd2, pobj->tgt_fwd2);
		fprintf( stderr, " flight_time %d, total_time %d, dribble_mode %d\n",
			ppdata->flight_time, ppdata->total_time, ppdata->dribble_mode);
		fprintf( stderr, " ball in %s hand, %s handed",// receiver of pass %d\n",
			ppdata->ball_in_hand == 0 ? "RIGHT" : "LEFT", ppdata->handedness == 0 ? "RIGHT" : "LEFT");//, ppdata->receiver_of_pass );
		fprintf( stderr, " auto control %s\n",
			ppdata->auto_control == YES ? "YES" : "NO");
		fprintf( stderr, " flail_time %d misc_wait_time %d, no ball collide %d\n",
			ppdata->flail_time, ppdata->misc_wait_time, ppdata->no_ball_collide);
		fprintf( stderr, " hanging on rim %s, defense anim wait %d, team %d, player num %d\n",
			ppdata->HangingOnRim == YES ? "YES" : "NO", ppdata->defense_anim_wait, ppdata->team, ppdata->plyrnum);
		fprintf(stderr, " distance to ball = %f, attack_mode = %s, attack_time = %d\n", odistxz(pobj, (obj_3d *)&ball_obj), att[ppdata->attack_mode], ppdata->attack_time);
		//fprintf(stderr, " stick_cur,old %d,%d button_cur,old,new %d,%d,%d stick_time %d\n", ppdata->stick_cur, ppdata->stick_old, ppdata->but_cur, ppdata->but_old, ppdata->but_new, ppdata->stick_time);
		fprintf(stderr, " stick_cur,old %d,%d button_cur,old,new ", ppdata->stick_cur, ppdata->stick_old);
		print_buttons(ppdata->but_cur);
		print_buttons(ppdata->but_old);
		print_buttons(ppdata->but_new);
		fprintf(stderr, "stick time %d\n",ppdata->stick_time);
		fprintf(stderr, " icnt = %d scnt = %d sinc = %f iframe %d fframe %f\n",
			pobj->adata[0].icnt, pobj->adata[0].scnt, pobj->adata[0].sinc, pobj->adata[0].iframe, pobj->adata[0].fframe);

		//fprintf( stderr, "\n" );
	//}
}

///////////////////////////////////////////////////////////////////////////////
void print_buttons( int but )
{
	if( but & P_A )
		fprintf( stderr, "A");
	else	
		fprintf( stderr, "-");

	if( but & P_B )
		fprintf( stderr, "B");
	else	
		fprintf( stderr, "-");

	if( but & P_C )
		fprintf( stderr, "C ");
	else	
		fprintf( stderr, "-");
	fprintf( stderr, " ");
}

///////////////////////////////////////////////////////////////////////////////
void print_ball_flags( void )
{
	int i;
	
	for( i = 0; ball_flag_names[i]; i++ )
		if( (1<<i) & ball_obj.flags )
			fprintf( stderr, "%s  ", ball_flag_names[i] );
			
	fprintf( stderr, "\n" );
}

///////////////////////////////////////////////////////////////////////////////
static void show_commands( void );
void do_jason_stuff( void );

static char	*ld_commands[] = {
"F - fumble on next hit",
"B - bobble on next hit",
"G - dump game state",
"D - set down",
"C - continue",
"Anything else - Show This Help"
};

static void show_commands(void)
{
	unsigned	i;

	for(i = 0; i < sizeof(ld_commands)/sizeof(void *); i++)
	{
		fprintf(stderr, "%s\r\n", ld_commands[i]);
	}
	fprintf(stderr, "Command:  ");
}

void do_jason_stuff( void )
{
	int	done = 0;
	int	cont = 1;
	int	c,d;

	while(!done)
	{
		show_commands();
		c = getchar();
		fprintf(stderr, "%c\r\n", c);
		switch(toupper(c))
		{
			case 'F':				// Force fumble
			{
				fprintf( stderr, "next hit will fumble\n" );
				//force_fumble = TRUE;
				//force_bobble = FALSE;
				break;
			}
			case 'B':				// Force bobble
			{
				fprintf( stderr, "next hit will bobble\n" );
				//force_bobble = TRUE;
				//force_fumble = FALSE;
				break;
			}
			case 'D':				// set down
			{
				fprintf( stderr, "set down to (1-4):" );
				d = getchar() - '0';
				if ((d<1) || (d>4))
					fprintf( stderr, "invalid entry\n" );
				//else
				//	game_info.down = d;
				break;
			}
			case 'G':
				dump_game_state(TRUE);
				break;
			case 'C':				// Continue execution
			{
				cont = 1;
				done = 1;
				break;
			}
			default:
			{
				break;
			}
		}
	}
}
#endif //DEBUG

///////////////////////////////////////////////////////////////////////////////
float solve_quad( float a, float b, float c )
{
	float	b2m4ac, r1, r2, r;

	if( fabs(c) < 0.001f )
		return 0.0f;

	if(( fabs( b ) < 0.001f ) && ( fabs( a ) < 0.001f ))
	{
#ifdef DEBUG_SOLVEQUAD
		fprintf(stderr,"SOLVE QUAD: returning 200000\n");
#endif
		return 200000.0f;
	}
	if( fabs( a ) < 0.001f )
		return( -1.0f * c / b );

	if( fabs( b ) < 0.001f )
		if(( -c / a ) < 0.0f )
		{
#ifdef DEBUG_SOLVEQUAD
		fprintf(stderr,"SOLVE QUAD: returning 300000\n");
#endif
			return 300000.0f;
		}
		else
			return fsqrt( -c / a );

	b2m4ac = (b*b) - (4.0f*a*c);

	if( b2m4ac < 0.0f )
	{
#ifdef DEBUG_SOLVEQUAD
		fprintf(stderr,"SOLVE QUAD: returning 100000\n");
#endif
		return 100000.0f;			// no real solution
	}

	r1 = (-1.0f * b + fsqrt(b2m4ac)) / (2.0f * a);
	r2 = (-1.0f * b - fsqrt(b2m4ac)) / (2.0f * a);

	if(( r1 < 0.0f ) && ( r2 < 0.0f ))
		return GREATER( r1, r2 );	// both are negative

	if(( r1 >= 0.0f ) && ( r2 >= 0.0f ))
		return LESSER( r1, r2 );	// both are nonnegative
	else
		return GREATER( r1, r2 );	// ne is nonnegative
}

//////////////////////////////////////////////////////////////////////////////
void norm2( float *v )
{
	float	mag;

	mag = fsqrt( v[0]*v[0] + v[1]*v[1] );

	if( mag <= 0.00001f )
	{
		v[0] = 0.0f;
		v[1] = 0.0f;
		return;
	}

	v[0] /= mag;
	v[1] /= mag;
}

///////////////////////////////////////////////////////////////////////////////

extern sprite_node_t *sprite_node_list;
extern sprite_node_t *alpha_sprite_node_list;

//////////////////////////////////////////////////////////////////////////////
// hide all the sprites in a string
void hide_string( void *parent )
{
	register sprite_node_t	*snode = sprite_node_list;

	// Walk a sprite list
	while(snode)
	{
		// Node we are looking for ?
		if(snode->si->parent == parent)
			hide_sprite( snode->si );

		snode = snode->next;
	}

	snode = alpha_sprite_node_list;

	// Walk a sprite list
	while(snode)
	{
		// Node we are looking for ?
		if(snode->si->parent == parent)
			hide_sprite( snode->si );

		snode = snode->next;
	}
}

//////////////////////////////////////////////////////////////////////////////
// unhide all the sprites in a string
void unhide_string( void *parent )
{
	register sprite_node_t	*snode = sprite_node_list;

	// Walk a sprite list
	while(snode)
	{
		// Node we are looking for ?
		if(snode->si->parent == parent)
			unhide_sprite( snode->si );

		snode = snode->next;
	}

	snode = alpha_sprite_node_list;

	// Walk a sprite list
	while(snode)
	{
		// Node we are looking for ?
		if(snode->si->parent == parent)
			unhide_sprite( snode->si );

		snode = snode->next;
	}
}
///////////////////////////////////////////////////////////////////////////////
void update_gamestart_percents( void )
{
	recalc_percent_aud(TWO_QUARTER_AUD, GAME_START_AUD, TWO_QUARTER_PERCENT_AUD);
	recalc_percent_aud(THREE_QUARTER_AUD, GAME_START_AUD, THREE_QUARTER_PERCENT_AUD);
	recalc_percent_aud(FOUR_QUARTER_AUD, GAME_START_AUD, FOUR_QUARTER_PERCENT_AUD);
	recalc_percent_aud(ONE_OVERTIME_AUD, GAME_START_AUD, ONE_OVERTIME_PERCENT_AUD);
	recalc_percent_aud(TWO_OVERTIME_AUD, GAME_START_AUD, TWO_OVERTIME_PERCENT_AUD);
	recalc_percent_aud(THREE_OVERTIME_AUD, GAME_START_AUD, THREE_OVERTIME_PERCENT_AUD);
	recalc_percent_aud(FULL_GAME_PURCHASE_AUD, GAME_START_AUD, FULL_GAME_PURCHASE_PCT_AUD);
	recalc_percent_aud(INITIALS_ENTERED_AUD, GAME_START_AUD, INITIALS_ENTERED_PERCENT_AUD);
}

///////////////////////////////////////////////////////////////////////////////
void update_quarters_percents( void )
{
	recalc_percent_aud( ONE_PLAYER_AUD,QUARTERS_PLAYED_AUD,ONE_PLAYER_PERCENT_AUD );
	recalc_percent_aud( TWO_PLAYER_AUD,QUARTERS_PLAYED_AUD,TWO_PLAYER_PERCENT_AUD );
	recalc_percent_aud( THREE_PLAYER_AUD,QUARTERS_PLAYED_AUD,THREE_PLAYER_PERCENT_AUD );
	recalc_percent_aud( FOUR_PLAYER_AUD,QUARTERS_PLAYED_AUD,FOUR_PLAYER_PERCENT_AUD );
	recalc_percent_aud( TWOVCPU_PLAYER_AUD,QUARTERS_PLAYED_AUD,TWOVCPU_PLAYER_PERCENT_AUD );
}

/////////////////////////////////////////////////////////////////////////////////////////////
int intercept_time( int attacker, int target, float vmax )
{
	register bbplyr_data * pa = player_blocks + attacker;
	register bbplyr_data * pt = player_blocks + target;
	register float a_x = pa->odata.x - pt->odata.x;
	register float b_z = pa->odata.z - pt->odata.z;
	register float c_d = fsqrt( a_x * a_x + b_z * b_z );
	register float b2m4ac;
	register int root1, root2;

	// looking for cylinder collision, not point collision.
	// pretend we're 8 units closer than we are
	if( c_d < 0.01f )
		return 1;

	b2m4ac = c_d - 8.0f;		//FIX!!!
	if( b2m4ac < 0.0f )
		b2m4ac = 0.0f;

	a_x *= (b2m4ac /= c_d);
	b_z *=  b2m4ac;

	// now solve the quad equation
	c_d = a_x * a_x + b_z * b_z;
	b_z = -2.0f * (pt->odata.vx * a_x + pt->odata.vz * b_z);
	a_x = pt->odata.vx * pt->odata.vx + pt->odata.vz * pt->odata.vz - vmax * vmax;

	if( fabs(a_x) < 0.01f )
	{
		// speeds are the same.  if b >= 0, target is moving away
		// and we can't catch him.
		if( b_z < -0.0001f )
			if( (root1 = (int)(-c_d / b_z)) <= 1000 )
				return root1;

		return 1000;
	}

	if(( b2m4ac = b_z * b_z - 4.0f * a_x * c_d ) < 0.0f )
		// target is too fast.  Can't catch him.
		return 1000;

	root1 = (int)(( -b_z + ( b2m4ac = fsqrt( b2m4ac ))) / ( a_x += a_x ));
	root2 = (int)(( -b_z - b2m4ac ) / a_x );

	if( root1 < 0 && root2 < 0 )
		return 1000;

	if( root1 < 0 )
		return root2;

	if( root2 < 0 )
		return root1;

	return ( root1 < root2 ) ? root1 : root2;
}

////////////////////////////////////////////////////////////////
/*
#define CAP(a,b,c)		LESSER(GREATER((a),(b)),(c))
float season_rating( int, int, int, int, float );
float season_rating( int wins, int losses, int scored, int allowed, float qb )
{
	float	w,s,q, rating;

	// guard against divide by zero
	wins = GREATER(wins,30);
	losses = GREATER(losses,0);
	allowed = GREATER(allowed,1);

	w = (float)wins / (float)(wins + losses);
	w = CAP(w,0.5f,1.0f);

	s = (float)scored / (float)allowed;
	s = CAP(s,1.0f,1.5f);

	q = CAP(qb,125.0f,175.0f);

	rating = 50.0f + (w-0.5f) * 50.0f + (s-1.0f) * 25.0f + (q-125.0f) / 2.0f;

	return rating;
}
*/
///////////////////////////////////////////////////////////////////////////////

