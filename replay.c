/*
 *		$Archive: /video/Nba/replay.c $
 *		$Revision: 59 $
 *		$Date: 9/29/99 5:16p $
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
char *ss_replay_c = "$Workfile: replay.c $ $Revision: 59 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#if defined(VEGAS)
#include <glide.h>
#else
#include <glide/glide.h>
#endif
#include <goose/goose.h>

/*
 *		USER INCLUDES
 */

#include "game.h"
#include "player.h"
#include "externs.h"
#include "id.h"
#include "fontid.h"
#include "select.h"

/*
 *		DEFINES
 */

#ifndef RELEASE_VER
 #ifndef TEST_VER
  #ifdef DEBUG
//	#define DBG_REPLAY
  #endif //DEBUG
 #endif //TEST_VER
#endif //RELEASE_VER

/*
 *		STRUCTURES/TYPEDEFS
 */

/*
 *		PROTOTYPES
 */

static void inc_and_write( void );
static void record_cur_frame( void );
static void reset_replay( void );

static float slomo_fraction(void);
static void prep_frame_quats( int );
static void replay_draw_prep( void );

static void draw_replay_shadows( void * );
static void draw_replay( void * );

static void suspend_players( void );
static void resume_players( void );

#ifdef DEBUG
static void dump_frame( FILE *, int );
static void dump_player( FILE *, int );
#endif

/*
 *		GLOBAL REFERNCES
 */

void draw_ball_shadow(void *);
void draw_ball(void *);
void draw_goals(void *);

void update_ostring_value(ostring_t *, int, int);

void replay_speech(void);

extern char *		limb_names[];
extern VIEWCOORD	cambot;
extern LIMB			**models[];
extern LIMB *		plyr_limbs_rm[];
extern LIMB *		hand_sets[];
extern LIMB			limb_Net;
extern LIMB			limb_Net2;
extern BBDATA		bbdata[];
extern SNODE		bbplyr_skel[];
extern SNODE		tophalf_skel[];
extern SNODE		bothalf_skel[];
extern char			*anim_blocks[];
extern float		lookup[];
extern int			hide_status_box;
extern int			showhide_flag;
extern char			*seq_names[];
extern MAT4			ballmat;			// Current rot matrix
extern int			balldbias;
extern float		hres, vres;

void rot_mat(float *, int);

//void Prep_Shadows( void );
//void Rend_Shadows( obj_3d *, int );
//void Draw_Shadows( void * );

/*
 *		GLOBAL VARIABLES
 */

int recorder = FALSE;							// recorder on? set with RR_??? <set_recorder_mode>
int playback = FALSE;							// playback on? set with RP_??? <set_playback_mode>

unsigned int rsnd[RSND_COUNT];					// replay sound data indexed with RSND_???

#ifdef DEBUG
int dbg_pnum;
int dbg_frame;
int dbg_play_head;
int dbg_playback = FALSE;
int dbg_playlast = FALSE;
int dbg_textflag = FALSE;
int dbg_dispflag = TRUE;
#endif //DEBUG

/*
 *		STATIC VARIABLES
 */

static replay_t		replay_data[REPLAY_FRAMES][NUM_ROBJS];
static replay_obj_t	replay_objs[NUM_ROBJS];

static int	record_head;						// index of most recently written frame, -1 == none
static int	record_tail;						// index of oldest valid frame, -1 == none
static int	tape_counter;						// non-wrapping frame count

static int	play_head;							// index of play head
static int	play_tail;							// index of last frame to play
static int	tick_dly;							// tick delay between frames in playback
static int	tick_cnt;							// tick delay counter

static void (* rsnd_func[RSND_COUNT])(unsigned int);

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
void show_replay_state_proc( int *args )
{
	int	last_recorder = FALSE;
	int	last_dbg_playback = FALSE;
	int last_dbg_textflag = FALSE;
	int last_dbg_dispflag = TRUE;
	int time = 0;

	while (1)
	{
		if (last_recorder != recorder ||
			last_dbg_playback != dbg_playback ||
			last_dbg_textflag != dbg_textflag ||
			last_dbg_dispflag != dbg_dispflag)
		{
			float y = 4.0f;
			float d = 14.0f;

			delete_multiple_strings(0xcdce, 0xffffffff);

			set_string_id(0xcdce);
			set_text_font(FONTID_NBA10);
			set_text_transparency_mode(TRANSPARENCY_ENABLE);
			set_text_z_inc(0.01f);

			set_text_justification_mode(HJUST_CENTER|VJUST_BOTTOM);

			if (!dbg_dispflag && last_dbg_dispflag)
			{
				set_text_position(256, y, 1.2f);
				set_text_color( LT_CYAN );
				oprintf("DISPLAY OFF");
				y += d;
			}
			else
			if (dbg_dispflag && (dbg_playback||playback))
			{
				y += d + d + d;
			}

			if (last_recorder != recorder ||
				last_dbg_playback != dbg_playback)
			{
				set_text_position(256, y, 1.2f);
				set_text_color((recorder ? LT_GREEN:LT_RED));
				oprintf("RECORD %s", (recorder) ? "ON":"OFF");
				y += d;
			}
			if (last_dbg_textflag != dbg_textflag)
			{
				set_text_position(256, y, 1.2f);
				set_text_color((dbg_textflag ? LT_GREEN:LT_RED));
				oprintf("TEXT %s", (dbg_textflag) ? "ON":"OFF");
				y += d;
			}
			set_text_z_inc(0.0f);

			time = 57 * 2;

			last_recorder = recorder;
			last_dbg_playback = dbg_playback;
			last_dbg_textflag = dbg_textflag;
			last_dbg_dispflag = dbg_dispflag;
		}

		if (!time)
		{
			delete_multiple_strings(0xcdce, 0xffffffff);
		}

		sleep(1);

		if (time > -1) time--;
	}
}
#endif //DEBUG

//////////////////////////////////////////////////////////////////////////////
#define PLAYBACK_BTN	P_B
#define RECORDER_BTN	P_C
#define HOLD	30
#define REPEAT	4

void replay_proc( int *args )
{
#ifdef DEBUG
	int dbsw_old, dbsw_new;
	int dbsw_cur = 0, cnt = 0, rpt = 0, acc = 0;
	int hold = 0;
	ostring_t * po_cur = NULL, * po_cnt = NULL, * po_n1 = NULL, * po_n2 = NULL;

	dbg_playback = FALSE;
	dbg_playlast = FALSE;
	dbg_textflag = FALSE;
	dbg_dispflag = TRUE;

#ifndef RELEASE_VER
#ifndef TEST_VER
	set_process_run_level( qcreate( "repstate", REPLAY_PID, show_replay_state_proc, 0, 0, 0, 0), RL_REPLAY+1 );
#endif //TEST_VER
#endif //RELEASE_VER
#endif //DEBUG

	// Initialize
	reset_replay();
	playback = FALSE;

	// Make replay objects
	create_object( "r_bshad", OID_REPLAY, OF_HIDDEN, DRAWORDER_REPLAY+0, (void *)&replay_objs[BALL_INDEX], draw_ball_shadow );
	create_object( "r_pshad", OID_REPLAY, OF_HIDDEN, DRAWORDER_REPLAY+1, NULL, draw_replay_shadows );
	create_object( "r_plyrs", OID_REPLAY, OF_HIDDEN, DRAWORDER_REPLAY+2, NULL, draw_replay );
	create_object( "r_bball", OID_REPLAY, OF_HIDDEN, DRAWORDER_REPLAY+3, (void *)&replay_objs[BALL_INDEX], draw_ball);
	create_object( "r_goals", OID_REPLAY, OF_HIDDEN, DRAWORDER_REPLAY+4, (void *)&replay_objs[GOAL_INDEX], draw_goals);

	while( 1 )
	{
		sleep( 1 );

#ifdef DBG_REPLAY
		delete_multiple_strings(0xaced0000,-1);
		del1c(0xaced0000,-1);
#endif

#ifdef DEBUG
		{
			dbsw_old = dbsw_cur;
			dbsw_cur = get_p5_sw_current() & 255;
			dbsw_new = get_p5_sw_close() & 255;

			if( dbsw_new == PLAYBACK_BTN || dbsw_new == RECORDER_BTN )
			{
				if( !playback )
				{
					int rec = recorder;

					if( dbg_playback )
					{
						dbg_playback = FALSE;
						recorder = (dbsw_new == PLAYBACK_BTN);
						if ( dbg_textflag )
						{
							fprintf( stderr, "Stop playback.\n" );
						}
						resume_players();
					}
					else if( dbsw_new == PLAYBACK_BTN && !halt )
					{
						if( record_head == -1 )
						{
							if ( dbg_textflag )
							{
								fprintf( stderr, "Can't playback--no frames.\n" );
							}
						}
						else
						{
							dbg_playback = TRUE;
							recorder = FALSE;
							play_head = record_head;
							dbsw_old = 0;
							if ( dbg_textflag )
							{
								fprintf( stderr, "Start playback at frame %d.\n",
									((record_head - play_head + REPLAY_FRAMES) % REPLAY_FRAMES));
							}
							suspend_players();
						}
					}
					else if( dbsw_new == RECORDER_BTN && !halt )
					{
						// Start/Stop recorder
						recorder = !recorder;
					}
					if (rec != recorder)
					{
						if ( dbg_textflag )
						{
							fprintf( stderr, "%s recorder.\n", recorder ? "Start" : "Stop");
						}
					}
				}
				// btn hit while in playback
				// dump to disk
				//write_replay_data();
			}
			else if( (dbg_playback || playback) && !(dbsw_cur & (P_A|P_B|P_C|P_D)) )
			{
				if( dbsw_new == P_UP )
				{
					dbg_dispflag = !dbg_dispflag;
				}
				else if( dbsw_new == P_DOWN )
				{
					dbg_textflag = !dbg_textflag;
					fprintf( stderr, "Replay Text %s.\r\n", dbg_textflag ? "ON" : "OFF" );
				}
				else if( dbg_playback && dbsw_cur == P_RIGHT )
				{
					if (!(dbsw_old == P_RIGHT)) cnt = HOLD;
					else
					if (!(--cnt)) cnt = rpt;

					if ( cnt == HOLD || cnt == rpt)
					{
						// frame advance
						if( play_head == record_head )
						{
							if ( dbg_textflag )
							{
								fprintf( stderr, "Can't advance--at end of replay.\n" );
							}
						}
						else
						{
							play_head = ++play_head % REPLAY_FRAMES;
							prep_frame_quats( play_head );
							if ( dbg_textflag )
							{
								fprintf( stderr, "Advancing to replay frame %d\n",
									((record_head - play_head + REPLAY_FRAMES) % REPLAY_FRAMES) );
								dump_frame( stderr, play_head );
							}
						}
					}
				}
				else if( dbg_playback && dbsw_cur == P_LEFT )
				{
					if (!(dbsw_old == P_LEFT)) cnt = HOLD;
					else
					if (!(--cnt)) cnt = rpt;

					if ( cnt == HOLD || cnt == rpt)
					{
						// frame backup
						if( play_head == record_tail )
						{
							if ( dbg_textflag )
							{
								fprintf( stderr, "Can't back up--already at start of replay.\n" );
							}
						}
						else
						{
							play_head = (--play_head + REPLAY_FRAMES) % REPLAY_FRAMES;
							prep_frame_quats( play_head );
							if ( dbg_textflag )
							{
								fprintf( stderr, "Backing up to frame %d.\n", 
									((record_head - play_head + REPLAY_FRAMES) % REPLAY_FRAMES) );
								dump_frame( stderr, play_head );
							}
						}
					}
				}
				else if (!hold)
				{
					cnt = 0;
					rpt = REPEAT;
					acc = 0;
				}
				if (!hold && cnt == rpt && ++acc == 10 && rpt > 1)
				{
					rpt--;
					acc = 0;
				}
			}
		}

		if( (!playback && !dbg_playback) || !dbg_dispflag )
		{
			if ( dbg_playlast)
				delete_multiple_strings(0xcccc, 0xffffffff);
			dbg_playlast = FALSE;
		}
		if( playback || dbg_playback )
		{

#ifndef RELEASE_VER
#ifndef TEST_VER
			int frame_cnt = (record_head - record_tail + REPLAY_FRAMES) % REPLAY_FRAMES;
			int frame_cur = (record_head - play_head   + REPLAY_FRAMES) % REPLAY_FRAMES;

			if ( !dbg_playlast && dbg_dispflag )
			{
				set_string_id(0xcccc);
				set_text_font(FONTID_NBA10);
				set_text_transparency_mode(TRANSPARENCY_ENABLE);
				set_text_color(LT_CYAN);
				set_text_z_inc(0.01f);

				set_text_justification_mode(HJUST_CENTER|VJUST_BOTTOM);
				set_text_position(256, 32, 1.2f);
				oprintf("REPLAY");
				set_text_justification_mode(HJUST_CENTER|VJUST_BOTTOM);
				set_text_position(256, 17, 1.2f);
				oprintf("/");
				set_text_justification_mode(HJUST_CENTER|VJUST_BOTTOM);
				set_text_position(256, 3, 1.2f);
				oprintf("/");

				set_text_justification_mode(HJUST_RIGHT|VJUST_BOTTOM);
				set_text_position(250, 18, 1.2f);
				po_cur = oprintf("%04d", 0);

				set_text_justification_mode(HJUST_LEFT|VJUST_BOTTOM);
				set_text_position(262, 18, 1.2f);
				po_cnt = oprintf("%04d", 0);

				set_text_justification_mode(HJUST_RIGHT|VJUST_BOTTOM);
				set_text_position(250, 4, 1.2f);
				po_n1 = oprintf("%04d", 0);

				set_text_justification_mode(HJUST_LEFT|VJUST_BOTTOM);
				set_text_position(262, 4, 1.2f);
				po_n2 = oprintf("%04d", 0);

				set_text_z_inc(0.0f);

				dbg_playlast = TRUE;
			}
			if ( dbg_playlast )
			{
				update_ostring_value(po_cur, frame_cur, 1);
				update_ostring_value(po_cnt, frame_cnt, 1);

				update_ostring_value(po_n1, (dbg_playback ? -1 : tick_cnt), 1);
				update_ostring_value(po_n2, (dbg_playback ? -1 : tick_dly), 1);
			}
#endif //TEST_VER
#endif //RELEASE_VER

			if( !dbg_playback )
			{
#endif //DEBUG

		////////////////
		// Chk if doing a playback
		if( playback )
		{

#ifdef DEBUG
			extern int pop_cnt;
			if( dbsw_new == PLAYBACK_BTN )
				hold = !hold;

			if (hold)
			{
				int bt = dbsw_cur & (P_A|P_B|P_C|P_D);
				int sw = dbsw_cur & (P_UP|P_DOWN|P_LEFT|P_RIGHT);
				int so = dbsw_old & (P_UP|P_DOWN|P_LEFT|P_RIGHT);
				if (cambot.tick_cnt > 1) cambot.tick_cnt--;
				pop_cnt++;
				if(sw == P_LEFT && !bt)
				{
					if (!(so == P_LEFT)) cnt = HOLD;
					else
					if (!(--cnt)) cnt = rpt;

					if ( cnt == HOLD || cnt == rpt)
					{
						if (tick_cnt == tick_dly && play_head != record_tail)
						{
							play_head = (--play_head + REPLAY_FRAMES) % REPLAY_FRAMES;
							tick_cnt = 0;
							if (cambot.tick_cnt > 1) cambot.tick_cnt--;
							pop_cnt++;
						}
						else
						if (play_head != record_tail)
						{
							tick_cnt++;
							if (cambot.tick_cnt > 1) cambot.tick_cnt--;
							pop_cnt++;
						}
					}
				}
				else
				if(sw & P_RIGHT && !bt)
				{
					if (!(so == P_RIGHT)) cnt = HOLD;
					else
					if (!(--cnt)) cnt = rpt;

					if ( cnt == HOLD || cnt == rpt)
					{
						if (tick_cnt == 0 && play_head != play_tail)
						{
							play_head = ++play_head % REPLAY_FRAMES;
							tick_cnt = tick_dly;
							if (cambot.tick_cnt < (cambot.csi.mode_cnt-1)) cambot.tick_cnt++;
							pop_cnt--;
						}
						else
						if (play_head != play_tail)
						{
							tick_cnt--;
							if (cambot.tick_cnt < (cambot.csi.mode_cnt-1)) cambot.tick_cnt++;
							pop_cnt--;
						}
					}
				}
				else
				{
					cnt = 0;
					rpt = REPEAT;
					acc = 0;
				}

				if (cnt == rpt && ++acc == 10 && rpt > 1)
				{
					rpt--;
					acc = 0;
				}

				if ( dbg_playlast )
				{
					if (hold < 10)
					{
						update_ostring_value(po_n1, tick_cnt, 1);
						update_ostring_value(po_n2, tick_dly, 1);
					}
					else
					{
						update_ostring_value(po_n1, 0, 0);
						update_ostring_value(po_n2, 0, 0);
					}
				}
				if (++hold > 15)
					hold = 1;
			}
			else
#endif //DEBUG

			// Chk if time for next frame
			if (--tick_cnt < 0)
			{
				// Chk if any frames left
				if (play_head != play_tail)
				{
					// Yes, continue
					play_head = ++play_head % REPLAY_FRAMES;
					tick_cnt = tick_dly;
				}
				else
					// No, stop here till someone turns us off
					tick_cnt == 0;
			}
		}
		if( playback )
			prep_frame_quats( play_head );

#ifdef DEBUG
			}
			else
				prep_frame_quats( play_head );
		}
#endif

		if( recorder && !halt )
			inc_and_write();
	}
}

//////////////////////////////////////////////////////////////////////////////
static void inc_and_write( void )
{
	// update head and tail
	tape_counter += 1;

	record_head = tape_counter % REPLAY_FRAMES;

	if( record_tail == record_head )
		record_tail = (tape_counter + 1) % REPLAY_FRAMES;

	if( record_tail == -1 )
		record_tail = 0;

	// record current frame
	record_cur_frame();
}

//////////////////////////////////////////////////////////////////////////////
static void record_cur_frame( void )
{
	bbplyr_data	*	ppdata  = player_blocks;
	replay_t *		pframe1 = &replay_data[record_head][0];
	replay_t *		pframe2 = &replay_data[record_head][TWOP_INDEX];
	int				index, block;
	int				i;

	// Record camera data
	memcpy(pframe1->rm.xform, cambot.xform, sizeof(cambot.xform));
	pframe1->rm.xform[ 3] = cambot.x;
	pframe1->rm.xform[ 7] = cambot.y;
	pframe1->rm.xform[11] = cambot.z;

	// Advance ptr to 1st plyr record
	pframe1 += PLYR_INDEX - CAMX_INDEX;

	// Record player data
	for( i = 0; i < NUM_PLAYERS; ppdata++, pframe1++, pframe2++, i++ )
	{
		anihdr  * panihdr;
		anidata	* pani1 = ppdata->odata.adata;
		anidata * pani2 = ppdata->odata.adata + 2;

		pframe1->rp.x        = ppdata->odata.x;
		pframe1->rp.y        = ppdata->odata.y + ppdata->odata.ay;
		pframe1->rp.z        = ppdata->odata.z;
		pframe1->rp.fwd      = ppdata->odata.fwd;
		pframe1->rp.hands    = ppdata->hands;
		pframe1->rp.head_dir = ppdata->head_dir;
		pframe1->rp.hackval  = 0;

		if( pani1->animode & MODE_INTSTREAM )
		{
			pframe1->rp.seq1   = pani1->seq_index;
			pframe1->rp.frame1 = pani1->iframe;
			pframe1->rp.t      = pani1->fframe - (float)pani1->iframe;

			block = pframe1->rp.seq1 >> 16;
			index = pframe1->rp.seq1 & 0xffff;

			panihdr = (struct anim_header *)anim_blocks[block];

			// Chk if at the end of the INTSTREAM, e.g. ANI_STREAMTOEND;
			// don't flag it as INTSTREAM if so
			if( pframe1->rp.frame1 >= panihdr[index].nframes - 1 )
			{
#ifdef DEBUG
				if( pframe1->rp.frame1 >= panihdr[index].nframes )
				{
					fprintf( stderr, "ERROR: record_cur_frame INTSTREAM "
						"out of range, seq %s frame %d (limit %d)\n",
						seq_names[index],
						pframe1->rp.frame1,
						panihdr[index].nframes );
				}
				if(pframe1->rp.t > 0.000001 )
				{
					fprintf( stderr, "ERROR: record_cur_frame INTSTREAM "
						"interpolate overflow, seq %s fframe %f (limit %d)\n",
						seq_names[index],
						pani1->fframe,
						panihdr[index].nframes );
				}
#endif //DEBUG
				pframe1->rp.flags = 0;
			}
			else
			{
				pframe1->rp.flags = R_INTSTREAM;
			}
		}
		else if( pani1->animode & MODE_INTERP )
		{
			pframe1->rp.seq1   = pani1->seq_index2;
			pframe1->rp.seq2   = pani1->seq_index;
			pframe1->rp.frame1 = pani1->iframe;
			pframe1->rp.frame2 = pani2->iframe;
			pframe1->rp.t      = (float)(pani1->icnt) / (float)(pani2->icnt);
			pframe1->rp.flags  = R_INTTOSEQ;

			// Chk if there was a MODE_TURNHACK
			if ((pani1->hackval & 0xffff0000) == RF_TURNHACK)
			{
				// Clr MODE_TURNHACK condition & record <adata.hackval>
				pani1->hackval = (int)((short)pani1->hackval);
				pframe1->rp.hackval = pani1->hackval;
			}
		}
		else
		{
			pframe1->rp.seq1   = pani1->seq_index;
			pframe1->rp.frame1 = pani1->iframe;
			pframe1->rp.flags  = 0;
		}

		if( game_info.ball_handler == i )
			pframe1->rp.flags |= R_RBALL;

		pani1 = ppdata->odata.adata + 1;

		if( pani1->animode & MODE_TWOPART )
		{
			pani2 = ppdata->odata.adata + 3;

			pframe1->rp.flags  |= R_TWOPART;
			pframe2->rp.fwd     = ppdata->odata.fwd2;
//			pframe2->rp.hackval = 0;					//!!! Need this!?!

			if( pani1->animode & MODE_INTSTREAM )
			{
				pframe2->rp.seq1   = pani1->seq_index;
				pframe2->rp.frame1 = pani1->iframe;
				pframe2->rp.t      = pani1->fframe - (float)pani1->iframe;

				block = pframe2->rp.seq1 >> 16;
				index = pframe2->rp.seq1 & 0xffff;

				panihdr = (struct anim_header *)anim_blocks[block];

				// Chk if at the end of the INTSTREAM, e.g. ANI_STREAMTOEND;
				// don't flag it as INTSTREAM if so
				if( pframe2->rp.frame1 >= panihdr[index].nframes - 1 )
				{
#ifdef DEBUG
					if( pframe2->rp.frame1 >= panihdr[index].nframes )
					{
						fprintf( stderr, "ERROR: record_cur_frame INTSTREAM "
							"out of range, seq %s frame %d (limit %d)\n",
							seq_names[index],
							pframe2->rp.frame1,
							panihdr[index].nframes );
					}
					if(pframe2->rp.t > 0.000001 )
					{
						fprintf( stderr, "ERROR: record_cur_frame INTSTREAM "
							"interpolate overflow, seq %s fframe %f (limit %d)\n",
							seq_names[index],
							pani1->fframe,
							panihdr[index].nframes );
					}
#endif //DEBUG
					pframe2->rp.flags = 0;
				}
				else
				{
					pframe2->rp.flags = R_INTSTREAM;
				}
			}
			else if( pani1->animode & MODE_INTERP )
			{
				pframe2->rp.seq1   = pani1->seq_index2;
				pframe2->rp.seq2   = pani1->seq_index;
				pframe2->rp.frame1 = pani1->iframe;
				pframe2->rp.frame2 = pani2->iframe;
				pframe2->rp.t      = (float)(pani1->icnt) / (float)(pani2->icnt);
				pframe2->rp.flags  = R_INTTOSEQ;

				// Chk if there was a MODE_TURNHACK
				if ((pani1->hackval & 0xffff0000) == RF_TURNHACK)
				{
					// Clr MODE_TURNHACK condition & record <adata.hackval>
					pani1->hackval = (int)((short)pani1->hackval);
					pframe2->rp.hackval = pani1->hackval;
//					fprintf(stderr, "===> 2PRT+INTRP:  J#:%02X,  %s -> %s,  hackval:%d,  fwd2:%d\r\n",
//						ppdata->p_spd->number,
//						seq_names[pframe2->rp.seq1 & 0xffff],
//						seq_names[pframe2->rp.seq2 & 0xffff],
//						pframe2->rp.hackval,
//						pframe2->rp.fwd);
				}
			}
			else
			{
				pframe2->rp.seq1   = pani1->seq_index;
				pframe2->rp.frame1 = pani1->iframe;
				pframe2->rp.flags  = 0;
			}
		}
		else {
			pframe2->rp.flags = R_UNUSED;
		}
	}

	// Advance ptr to ball record
	pframe2 += BALL_INDEX - TWOP_INDEX - NUM_PLAYERS;

	// Record ball data
	pframe2->rb.x     = ball_obj.odata.x;
	pframe2->rb.y     = ball_obj.odata.y;
	pframe2->rb.z     = ball_obj.odata.z;
	pframe2->rb.color = ball_obj.color;
	pframe2->rb.dbias = balldbias;
	pframe2->rb.scale = ball_obj.scale;
//	pframe2->rb.flags = R_DRAWBALL;

	// Advance ptr to ball rotation record
	pframe2 += BROT_INDEX - BALL_INDEX;

	// Record ball rotation data
	memcpy(pframe2->rm.xform, &ballmat, sizeof(MAT4));

	// Advance ptr to goal record
	pframe2 += GOAL_INDEX - BROT_INDEX;

	// Record goals data
	for (i = 0; i < 2; pframe2++, i++)
	{
		pframe2->rg.bend     = bbdata[i].bend;
		pframe2->rg.rimbend  = bbdata[i].rimbend;
		pframe2->rg.rimcolor = bbdata[i].rimcolor;
		pframe2->rg.netcolor = bbdata[i].netcolor;
		pframe2->rg.netya    = bbdata[i].netya;

		pframe2->rg.pvtx = !i ? limb_Net.pvtx : limb_Net2.pvtx;
#ifdef JONHEY
		pframe2->rg.bbox = !i ? limb_Net.bbox : limb_Net2.bbox;
#endif
	}

	// Advance ptr to replay sounds data
	pframe2 += RSND_INDEX - (GOA2_INDEX + 1);

	// Record replay sounds data & clr for next frame
	memcpy(pframe2->rs.rsnd, &rsnd, sizeof(rsnd));
	i = RSND_COUNT;
	while (i--) rsnd[i] = 0;
}

//////////////////////////////////////////////////////////////////////////////
static void reset_replay( void )
{
	int i = RSND_COUNT;

	// Init replay sounds data
	while (i--) rsnd[i] = 0;

	rsnd_func[RSND_DRIBBLE1]       = make_dribble1_snd;
	rsnd_func[RSND_DRIBBLE2]       = make_dribble2_snd;
	rsnd_func[RSND_NET]            = make_net_snd;
	rsnd_func[RSND_HIT_BACKBOARD1] = make_hit_backboard1_snd;
	rsnd_func[RSND_HIT_BACKBOARD3] = make_hit_backboard3_snd;
	rsnd_func[RSND_HIT_RIM]        = make_hit_rim_snd;
	rsnd_func[RSND_DUNK]           = make_dunk_snd;
	rsnd_func[RSND_NET_ON_FIRE]    = make_net_on_fire_snd;

	record_head  = -1;
	record_tail  = -1;
	tape_counter = -1;

	recorder = FALSE;
}

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
static float qmag( float *pq );
static float qmag( float *pq )
{
	return fsqrt( pq[0] * pq[0] + pq[1] * pq[1] + pq[2] * pq[2] + pq[3] * pq[3] );
}

int dbg_chk_plyr_quats(int frame, int pnum, float * src, float * dst);
int dbg_chk_plyr_quats(int frame, int pnum, float * src, float * dst)
{
	replay_obj_t * probj = replay_objs + pnum + PLYR_INDEX;
	replay_plyr_t * prp = &replay_data[frame][pnum + PLYR_INDEX].rp;
	float * fp = probj->pq;
	float mag;
	int limb = 0;
	int e = 0;
	int f = (record_head - frame + REPLAY_FRAMES) % REPLAY_FRAMES;
 
 	// verify that all quats are normalized
	do
	{
		mag = qmag(fp);

		if ( fabs(mag - 1.0f) > 0.05f )
		{
			if (!e)
			{
				fprintf( stderr, "\r\n" );
				fprintf( stderr, "Error: non-normalized quat, Player: %d (%02x), Replay frame: %d\r\n",
					pnum, player_blocks[pnum].p_spd->number, f );
				fprintf( stderr, "Seqs %s & %s, Frames %d & %d, t %f, flags %08x\r\n",
					seq_names[prp->seq1], seq_names[prp->seq2],
					prp->frame1, prp->frame2,
					prp->t,
					prp->flags );
				e = 1;
			}
			fprintf( stderr, "Limb: %2d = %s\r\n", limb, limb_names[limb] );
			if (src)
			fprintf( stderr, " Src: %10f, %10f, %10f, %10f - mag %10f\r\n", src[0], src[1], src[2], src[3], qmag(src) );
			if (dst)
			fprintf( stderr, " Dst: %10f, %10f, %10f, %10f - mag %10f\r\n", dst[0], dst[1], dst[2], dst[3], qmag(dst) );
			fprintf( stderr, "Quat: %10f, %10f, %10f, %10f - mag %10f\r\n", fp[0], fp[1], fp[2], fp[3], mag );
		}

		if (src) src += 4;
		if (dst) dst += 4;
		fp += 4;
	} while (++limb < 15);

	return e;
}

int dbg_chk_plyr_matrices(int frame, int pnum);
int dbg_chk_plyr_matrices(int frame, int pnum)
{
	replay_obj_t * probj = replay_objs + pnum + PLYR_INDEX;
	replay_plyr_t * prp = &replay_data[frame][pnum + PLYR_INDEX].rp;
	float * fp = probj->matrices[0];
	float mag;
	int limb;
	int j = 0;
	int e = 0;
	int f = (record_head - frame + REPLAY_FRAMES) % REPLAY_FRAMES;

	// make sure all the matrices are normalized
	do
	{
		limb = j / 3;

		// ignore the head
		if ( limb != JOINT_NECK )
		{
			mag = fp[0]*fp[0] + fp[1]*fp[1] + fp[2]*fp[2];

			if ( fabs(mag - 1.0f) > ((1.200f * 1.200f) - 1.0f) )
			{
				if (!e)
				{
					fprintf( stderr, "\r\n" );
					fprintf( stderr, "Error: non-normalized matrix line, Player: %d (%02x), Replay frame: %d\r\n",
						pnum, player_blocks[pnum].p_spd->number, f );
					fprintf( stderr, "Seqs %s & %s, Frames %d & %d, t %f, flags %08x\r\n",
						seq_names[prp->seq1], seq_names[prp->seq2],
						prp->frame1, prp->frame2,
						prp->t,
						prp->flags );
					e = 1;
				}
				fprintf( stderr, "Limb: %2d = %s\r\n", limb, limb_names[limb] );
				fprintf( stderr, "Mat line: %10f, %10f, %10f - mag %10f\r\n", fp[0], fp[1], fp[2], sqrt(mag) );
				limb *= 4;
				mag = qmag(probj->pq + limb);
				fprintf( stderr, "Src quat: %10f, %10f, %10f, %10f - mag %10f\r\n",
					probj->pq[limb+0], probj->pq[limb+1], probj->pq[limb+2], probj->pq[limb+3], mag);
			}
		}
		fp += 4;
	} while (++j < 45);

	return e;
}
#endif //DEBUG

//////////////////////////////////////////////////////////////////////////////
static void build_plyr_quats(replay_plyr_t * prp, float * pq, int hackval)
{
	float	iblock1[60], iblock2[60];
	int		j;

	if ( prp->flags & R_INTSTREAM )
	{
		j = get_frame( prp->seq1, prp->frame1, iblock1, iblock2 );
#ifdef DEBUG
		if (j)
		{
			int f = (record_head - dbg_frame + REPLAY_FRAMES) % REPLAY_FRAMES;

			if (dbg_pnum < NUM_PLAYERS)
				fprintf(stderr,"Replay %d INTSTREAM -> f:%d  t:%f  P#:%d  J#:%02X\r\n",
					f,prp->frame1,prp->t,dbg_pnum,player_blocks[dbg_pnum].p_spd->number);
			else
				fprintf(stderr,"Replay %d INTSTREAM -> f:%d  t:%f  P#:%d  J#:%02X  (2nd Part)\r\n",
					f,prp->frame1,prp->t,(dbg_pnum-NUM_PLAYERS),player_blocks[(dbg_pnum-NUM_PLAYERS)].p_spd->number);
		}
#endif
		for( j = 0; j < 15; j++ )
			slerp( iblock1 + 4*j, iblock2 + 4*j, prp->t, pq + 4*j );
	}
	else
	if ( prp->flags & R_INTTOSEQ )
	{
		float	fq[4], pp[4];

		get_frame( prp->seq1, prp->frame1, iblock1, NULL );
		get_frame( prp->seq2, prp->frame2, iblock2, NULL );

		fq[0] = 0.0f;
		fq[2] = 0.0f;

		if ( hackval )
		{
			fq[1] = isin( hackval/2 );
			fq[3] = icos( hackval/2 );
		}
		else
		{
			fq[1] = 0.0f;
			fq[3] = 1.0f;
		}
	
		qxq( fq, iblock1, pp );
	
		iblock1[0] = pp[0];
		iblock1[1] = pp[1];
		iblock1[2] = pp[2];
		iblock1[3] = pp[3];

		for( j = 0; j < 15; j++ )
			adjust_quat( iblock1 + 4*j, iblock2 + 4*j );

		for( j = 0; j < 15; j++ )
			slerp( iblock1 + 4*j, iblock2 + 4*j, prp->t, pq + 4*j );
	}
	else
	{
		get_frame( prp->seq1, prp->frame1, pq, NULL );
	}
}

//////////////////////////////////////////////////////////////////////////////
// figure slow motion fraction per <tick_dly>; returns:
//   a) 0 to use current <play_head> data;
//   b) fractional amount to use of deltas between current & next frame
//      of <play_head> data
static float slomo_fraction(void)
{
	int cnt = tick_dly - tick_cnt;
	if (cnt <= 0
#ifdef DEBUG
		|| dbg_playback
#endif //DEBUG
		)
	{
		// Use current frame if tick_cnt >= tick_dly
		return 0.0f;
	}
	return (float)cnt / (float)(tick_dly + 1);
}

//////////////////////////////////////////////////////////////////////////////
// set replay_block quats to match recorded frame N
static void prep_frame_quats( int frame )
{
#ifdef DEBUG
	static int last_play_head;
	static int last_tick_cnt;
#endif
	replay_obj_t *	probj;
	replay_t *		pframe1;
	replay_t *		pframe2;
	float			iblock1[60], iblock2[60];
	float			fq[4], pp[4];
	float			frac = slomo_fraction();
	float			t;
	int				i,j;

#ifdef DBG_REPLAY
	set_string_id(0xaced0000);
	set_text_font(FONTID_BAST8T);//NBA10);
	set_text_transparency_mode(TRANSPARENCY_ENABLE);
	set_text_justification_mode(HJUST_RIGHT|VJUST_TOP);
	set_text_z_inc(0.01f);
#endif

	// barf if frame isn't valid
	if ( frame < 0 || frame >= REPLAY_FRAMES ||
			( record_tail == 0 && record_head < frame ))
	{
#ifdef DEBUG
		fprintf( stderr, "Error: Replay of invalid frame.\n" );
		lockup();
#else
		while(1);
#endif
	}

	probj   = replay_objs;
	pframe1 = replay_data[frame];

	// Playback camera data
	if (cambot.csi.mode < 0
#ifdef DEBUG
		|| dbg_playback
#endif
		)
	{
		memcpy(cambot.xform, pframe1->rm.xform, sizeof(cambot.xform));
		cambot.x = pframe1->rm.xform[ 3];
		cambot.y = pframe1->rm.xform[ 7];
		cambot.z = pframe1->rm.xform[11];
		cambot.xform[ 3] = 0.0f; 
		cambot.xform[ 7] = 0.0f;
		cambot.xform[11] = 0.0f;
	}

	// Advance ptrs to 1st plyr record
	probj   += PLYR_INDEX - CAMX_INDEX;
	pframe1 += PLYR_INDEX - CAMX_INDEX;

	// Set ptr to next frame, if available, for any interpolation
	if (frame != play_tail
#ifdef DEBUG
		|| (dbg_playback && play_head != record_head)
#endif
		)
		pframe2 = &replay_data[(frame + 1) % REPLAY_FRAMES][PLYR_INDEX];
	else
		pframe2 = pframe1;


	// Playback player data
	for ( i = 0; i < (NUM_PLAYERS*2); probj++, pframe1++, pframe2++, i++ )
	{
		if ( pframe1->rp.flags & R_UNUSED )
			continue;

		if (( pframe1->rp.flags & R_INTTOSEQ ) &&
				(probj->last_seq1   != pframe1->rp.seq1   ||
				 probj->last_frame1 != pframe1->rp.frame1 ||
				 probj->last_seq2   != pframe1->rp.seq2   ||
				 probj->last_frame2 != pframe1->rp.frame2))
		{
			probj->hackval = pframe1->rp.hackval;

//	fprintf(stderr,"replay hackval:%d  P#:%d  J#:%02X  fwd:%d  nxtfwd:%d  %s\r\n",
//		probj->hackval,(i%NUM_PLAYERS),player_blocks[(i%NUM_PLAYERS)].p_spd->number,
//		pframe1->rp.fwd,pframe2->rp.fwd,(i < NUM_PLAYERS ? "":"(2ND PART)"));
//sleep(20);

		}
		else
		{
			probj->hackval = 0;
		}

#ifdef DEBUG
		dbg_pnum  = i;
		dbg_frame = frame;
#endif

		build_plyr_quats( &pframe1->rp, iblock1, probj->hackval );
		build_plyr_quats( &pframe2->rp, iblock2, probj->hackval );

		for( j = 0; j < 15; j++ )
			adjust_quat( iblock1 + 4*j, iblock2 + 4*j );

		for( j = 0; j < 15; j++ )
			slerp( iblock1 + 4*j, iblock2 + 4*j, frac, probj->pq + 4*j );

#ifdef DEBUG
		// verify that all quats are normalized
		j = dbg_chk_plyr_quats( frame, i, iblock1, iblock2 );
#ifdef PAUSE
		if (j)
		{
			fprintf(stderr,"Any button continues\r\n");
			do scan_switches(); while(!(get_player_sw_current()&ALL_ABCD_MASK));
		}
#endif
#endif

#ifdef DBG_REPLAY
		if ( i < NUM_PLAYERS && dbg_dispflag )
		{
			static image_info_t ii = {NULL, 1, 1, 0, 0, 0, 1, 0, 1};
			sprite_info_t * psi;
			int d = 10;
			int y = 4 + (4+d+d+d) * (NUM_PLAYERS-i);

			if (i == 0)
			{
				psi = beginobj(&ii, 390, 0, 2, 0);
				psi->id = 0xaced0000;
				psi->w_scale = 512 - 390;
				psi->h_scale = y+4;
				psi->state.constant_color = 0x303030;
				psi->state.alpha_rgb_dst_function = GR_BLEND_DST_COLOR;
				generate_sprite_verts(psi);
			}

			set_text_position(500, y, 1.2f);
			oprintf("%dc%x %dc%s%dc%s%dc%s%dc",
				pframe1->rp.flags & R_RBALL ?
				MED_ORANGE : MED_GREEN,
					player_blocks[(i%NUM_PLAYERS)].p_spd->number,
				MED_CYAN,
					pframe1->rp.flags & R_INTSTREAM ? "INTSTREAM" : ( pframe1->rp.flags & R_INTTOSEQ ? "INTTOSEQ" : "NORMAL" ),
				pframe1->rp.flags & R_TWOPART ?
				MED_YELLOW : MED_GREEN,
					pframe1->rp.flags & R_TWOPART ? " 2P" : " 1P",
				MED_RED,
					probj->hackval ? " H" : "",
				MED_CYAN);

			y -= d;

			set_text_position(470, y, 1.2f);
			oprintf("%s", seq_names[pframe1->rp.seq1] );
			set_text_position(500, y, 1.2f);
			oprintf("%d", pframe1->rp.frame1 );

			y -= d;

			set_text_position(470, y, 1.2f);
			oprintf("%s", seq_names[pframe2->rp.seq1] );
			set_text_position(500, y, 1.2f);
			oprintf("%d", pframe2->rp.frame1 );
		}
#endif

		probj->last_seq1   = pframe1->rp.seq1;
		probj->last_frame1 = pframe1->rp.frame1;
		probj->last_seq2   = pframe1->rp.seq2;
		probj->last_frame2 = pframe1->rp.frame2;

		probj->x        = pframe1->rp.x;
		probj->y        = pframe1->rp.y;
		probj->z        = pframe1->rp.z;
		probj->fwd      = pframe1->rp.fwd;
		probj->hands    = pframe1->rp.hands;
		probj->head_dir = pframe1->rp.head_dir;
		probj->flags    = pframe1->rp.flags;

		if (frac && !(pframe2->rp.flags & R_UNUSED))
		{
			// Interpolate position with portion of now-to-next deltas
			probj->x += frac * (pframe2->rp.x - probj->x);
			probj->y += frac * (pframe2->rp.y - probj->y);
			probj->z += frac * (pframe2->rp.z - probj->z);

			j = pframe2->rp.fwd - probj->fwd;
			// Remove any <hackval> from the yet-to-be-active <fwd>
			if (pframe1 != pframe2) j += pframe2->rp.hackval;
			while (j >= 512) j -= 1024;
			while (j < -512) j += 1024;
			probj->fwd += (int)(frac * (float)j);

			probj->head_dir += frac * (pframe2->rp.head_dir - probj->head_dir);
		}
	}

	// Advance ptrs to ball record
	probj   += BALL_INDEX - TWOP_INDEX - NUM_PLAYERS;
	pframe1 += BALL_INDEX - TWOP_INDEX - NUM_PLAYERS;
	pframe2 += BALL_INDEX - TWOP_INDEX - NUM_PLAYERS;

	// Playback ball data
	probj->x     = pframe1->rb.x;
	probj->y     = pframe1->rb.y;
	probj->z     = pframe1->rb.z;
	probj->color = pframe1->rb.color;
	probj->dbias = pframe1->rb.dbias;
	probj->scale = pframe1->rb.scale;
//	probj->flags = pframe1->rb.flags;

	if (frac)
	{
		// Interpolate position with portion of now-to-next deltas
		probj->x += frac * (pframe2->rb.x - probj->x);
		probj->y += frac * (pframe2->rb.y - probj->y);
		probj->z += frac * (pframe2->rb.z - probj->z);
	}

	// Advance ptrs to ball rotation record
	pframe1 += BROT_INDEX - BALL_INDEX;

	// Playback ball rotation data to ball <replay_objs>
	memcpy(probj->pq, pframe1->rm.xform, sizeof(MAT4));

	// Advance ptrs to goal record
	probj   += GOAL_INDEX - BALL_INDEX;
	pframe1 += GOAL_INDEX - BROT_INDEX;
	pframe2 += GOAL_INDEX - BALL_INDEX;

	// Playback goals data
	for (i = 0; i < 2; probj++, pframe1++, pframe2++, i++)
	{
		((replay_goal_t *)probj)->bend     = pframe1->rg.bend    + frac * (pframe2->rg.bend - pframe1->rg.bend);
		((replay_goal_t *)probj)->rimbend  = pframe1->rg.rimbend + frac * (pframe2->rg.rimbend - pframe1->rg.rimbend);
		((replay_goal_t *)probj)->rimcolor = pframe1->rg.rimcolor;
		((replay_goal_t *)probj)->netcolor = pframe1->rg.netcolor;
		((replay_goal_t *)probj)->netya    = pframe1->rg.netya   + frac * (pframe2->rg.netya - pframe1->rg.netya);

		((replay_goal_t *)probj)->pvtx = pframe1->rg.pvtx;
#ifdef JONHEY
		((replay_goal_t *)probj)->bbox = pframe1->rg.bbox;
#endif
	}

	// Advance ptrs to replay sounds data
	pframe1 += RSND_INDEX - (GOA2_INDEX + 1);

	// Playback replay sounds data
	if (tick_cnt == tick_dly
#ifdef DEBUG
		&& (last_play_head != play_head || last_tick_cnt != tick_cnt)
#endif
		)
	{
		i = RSND_COUNT;
		while (i--)
		{
			if (pframe1->rs.rsnd[i])
			{
				rsnd_func[i](pframe1->rs.rsnd[i]);
			}
		}
	}

#ifdef DBG_REPLAY
	set_text_z_inc(0.00f);
#endif

#ifdef DEBUG
	last_play_head = play_head;
	last_tick_cnt = tick_cnt;
#endif
}

//////////////////////////////////////////////////////////////////////////////
// final pre-draw prep.  This data is invalidated by any call to
// prep_frame_quats() or any change to the cambot matrix.
static void replay_draw_prep( void )
{
	replay_obj_t *	probj = replay_objs + PLYR_INDEX;
	bbplyr_data *	ppdata = player_blocks;
	float			mat[12],cm[12],cm2[12];
	float			rx,ry,rz,x,y,z,xf,yf;
	int				i;

	// flag onscreen players
	for( i = 0; i < NUM_PLAYERS; probj++, ppdata++, i++ )
	{
		rx = probj->x - cambot.x;
		ry = probj->y - cambot.y;
		rz = probj->z - cambot.z;

		// compute screen coords of player center
		x = cambot.xform[0]*rx+cambot.xform[1]*ry+cambot.xform[2]*rz+cambot.xform[3];
		y = cambot.xform[4]*rx+cambot.xform[5]*ry+cambot.xform[6]*rz+cambot.xform[7];
		z = cambot.xform[8]*rx+cambot.xform[9]*ry+cambot.xform[10]*rz+cambot.xform[11];

		xf = z * (50.0f/hres + 0.5f);
		yf = xf * (vres/hres);

		// set or clear onscreen bit
		if( z < 10.0f || fabs(x) > xf || fabs(y) > yf )
			probj->flags &= ~R_ONSCREEN;
		else
			probj->flags |= R_ONSCREEN;

#ifdef DEBUG
		// verify that all quats are normalized
		if (dbg_chk_plyr_quats( play_head, i, NULL, NULL ))
		{
			fprintf( stderr, "Players:\r\n" );
			dump_frame( stderr, play_head );
#ifdef PAUSE
			fprintf(stderr,"Any button continues\r\n");
			do scan_switches(); while(!(get_player_sw_current()&ALL_ABCD_MASK));
#endif
		}
#endif
		// onscreen or not, we need the matrices for collision checking
		mat[3]  = rx;
		mat[7]  = ry;
		mat[11] = rz;

		roty( probj->fwd, mat );
		mxm( cambot.xform, mat, cm );
		cm[0] *= ppdata->odata.xscale; cm[1] *= ppdata->odata.yscale; cm[2]  *= ppdata->odata.zscale;
		cm[4] *= ppdata->odata.xscale; cm[5] *= ppdata->odata.yscale; cm[6]  *= ppdata->odata.zscale;
		cm[8] *= ppdata->odata.xscale; cm[9] *= ppdata->odata.yscale; cm[10] *= ppdata->odata.zscale;

		if( probj->flags & R_TWOPART )
		{
			// do top half first so bottom overwrites pelvis matrix (top and bottom
			// each have a pelvis, but only the bottom's pelvis is displayed.)
			roty( probj[NUM_PLAYERS].fwd, mat );
			mxm( cambot.xform, mat, cm2 );
			cm2[0] *= ppdata->odata.xscale; cm2[1] *= ppdata->odata.yscale; cm2[2]  *= ppdata->odata.zscale;
			cm2[4] *= ppdata->odata.xscale; cm2[5] *= ppdata->odata.yscale; cm2[6]  *= ppdata->odata.zscale;
			cm2[8] *= ppdata->odata.xscale; cm2[9] *= ppdata->odata.yscale; cm2[10] *= ppdata->odata.zscale;

			generate_matrices( tophalf_skel, cm2, probj[NUM_PLAYERS].pq, probj->matrices[0], probj->jpos[0], ppdata->f_headsc, ppdata->plyrnum );
			generate_matrices( bothalf_skel, cm,  probj->pq,             probj->matrices[0], probj->jpos[0], ppdata->f_headsc, ppdata->plyrnum );
		}
		else
		{
			generate_matrices( bbplyr_skel,  cm,  probj->pq,             probj->matrices[0], probj->jpos[0], ppdata->f_headsc, ppdata->plyrnum );
		}

		rot_mat( probj->matrices[JOINT_NECK], probj->head_dir );

#ifdef DEBUG
		// make sure all the matrices are normalized
		if (dbg_chk_plyr_matrices( play_head, i ))
		{
			fprintf( stderr, "Players:\r\n" );
			dump_frame( stderr, play_head );
			write_replay_data();
#ifdef PAUSE
			fprintf(stderr,"Any button continues\r\n");
			do scan_switches(); while(!(get_player_sw_current()&ALL_ABCD_MASK));
#endif
		}
#endif
	}
}

//////////////////////////////////////////////////////////////////////////////
// Set recorder mode
// Returns:
//    TRUE if successful, FALSE otherwise
//
int set_recorder_mode( int mode )
{
	switch( mode )
	{
		case RR_RESTART:
			reset_replay();
		case RR_START:
			recorder = TRUE;
			break;

		case RR_REWIND:
			reset_replay();
		case RR_STOP:
			recorder = FALSE;
			break;

		default:
#ifdef DEBUG
			fprintf( stderr, "Error: bad replay recorder mode: %d\n", mode );
			lockup();
#else
//fix!!!	while(1) ;
#endif
			return FALSE;
	}
	// Valid mode - stop playback
	playback = FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// Set playback mode - wants to be called before <player_procs> execute (?)
// Returns:
//    playback tick count if successful, 0 otherwise
//
int set_playback_mode( int mode, int sframe, int eframe, int ticks )
{
	int i;

	if ((mode != RP_STOP && ticks < 0) ||
		(mode == RP_START &&
		(sframe < 0 || eframe < 0 || sframe >= REPLAY_FRAMES || sframe < eframe)))
	{
#ifdef DEBUG
		fprintf( stderr, "Error: bad replay playback parameter(s): "
			"mode=%d  sframe=%d  eframe=%d  ticks=%d\n",
			mode, sframe, eframe, ticks );
#endif
		return 0;
	}
	if (mode != RP_STOP && (record_head < 0 || record_tail < 0))
	{
		i = recorder;
#ifdef DEBUG
		fprintf( stderr, "Error: bad or no tape for replay playback - reseting\n");
#endif
		// reset recorder but keep current mode
		reset_replay();
		recorder = i;

		return 0;
	}
	if (game_info.game_state == GS_EOQ)
	{
		return 0;
	}

	switch( mode )
	{
		case RP_STARTALL:
			sframe = REPLAY_FRAMES - 1;
			eframe = 0;
		case RP_START:
			i = (record_head - record_tail + REPLAY_FRAMES) % REPLAY_FRAMES;

			if (sframe > i) sframe = i;
			if (eframe > i) eframe = i;

#ifdef DEBUG
			dbg_play_head =
#endif
			play_head = (record_head - sframe + REPLAY_FRAMES) % REPLAY_FRAMES;
			play_tail = (record_head - eframe + REPLAY_FRAMES) % REPLAY_FRAMES;
			tick_dly = ticks;
			tick_cnt = ticks + 1;	// +1 to show first frame

			replay_speech();

			suspend_players();

			playback = TRUE;
			recorder = FALSE;

			// Prep first frame so replay_blocks will be initialized for caller
			prep_frame_quats( play_head );

			return (sframe - eframe + 1) * (ticks + 1);

		case RP_STOP:
			if (playback)
				resume_players();

			playback = FALSE;

			break;

		default:
#ifdef DEBUG
			fprintf( stderr, "Error: bad replay playback mode: %d\n", mode );
			lockup();
#else
//fix!!!	while(1) ;
#endif
			break;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
void * get_replay_obj_ptr(int objnum)
{
	if (objnum < 0 || objnum >= NUM_ROBJS)
		return NULL;

	return &replay_objs[objnum];
}

//////////////////////////////////////////////////////////////////////////////
static void draw_replay_shadows( void *oi )
{
	replay_obj_t *	probj;
	bbplyr_data		temp_player;
	float *			fp_s;
	float *			fp_d;
	int				i, j;

	temp_player.odata.flags = PF_ONSCREEN;

	replay_draw_prep();

//	// player circle shadows
//	Prep_Shadows();
//
//	probj = replay_objs + PLYR_INDEX;
//
//	for( i = 0; i < NUM_PLAYERS; probj++, i++ )
//	{
//		if(!( probj->flags & R_ONSCREEN ))
//			continue;
//
//		temp_player.odata.x   = probj->x;
//		temp_player.odata.y   = probj->y;
//		temp_player.odata.z   = probj->z;
//		temp_player.odata.fwd = probj->fwd;
//
//		Rend_Shadows( (obj_3d *)&temp_player.odata, FALSE );
//	}
//
//	// ball shadow
//	if( replay_objs[BALL_INDEX].flags & R_DRAWBALL )
//	{
//		temp_player.odata.x   = replay_objs[BALL_INDEX].x;
//		temp_player.odata.y   = replay_objs[BALL_INDEX].y;
//		temp_player.odata.z   = replay_objs[BALL_INDEX].z;
//		temp_player.odata.fwd = replay_objs[BALL_INDEX].fwd;
//
//		Rend_Shadows( (obj_3d *)&temp_player.odata, TRUE );
//	}

	// player limb shadows
	player_shadow_predraw( NULL );

	probj = replay_objs + PLYR_INDEX;

	for( i = 0; i < NUM_PLAYERS; probj++, i++ )
	{
		if(!( probj->flags & R_ONSCREEN ))
			continue;

		for( j = 0; j < 12; j++)
		{
			temp_player.matrices[JOINT_RANKLE][j] = probj->matrices[JOINT_RANKLE][j];
			temp_player.matrices[JOINT_LANKLE][j] = probj->matrices[JOINT_LANKLE][j];
		}

		fp_s = probj->jpos[0];
		fp_d = temp_player.jpos[0];
		for( j = 0; j < (15 * 3); j++ )
			*fp_d++ = *fp_s++;

		draw_player_shadow( &temp_player );
	}
}

//////////////////////////////////////////////////////////////////////////////

static void draw_replay( void *oi )
{
	replay_obj_t *	probj  = replay_objs + PLYR_INDEX;
	bbplyr_data *	ppdata = player_blocks;
	LIMB **			body;
	LIMB *			tmp_hand_r;
	LIMB *			tmp_hand_l;
	int				i;

	// init glide states for players
	guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);
	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
	grTexClampMode(0,GR_TEXTURECLAMP_WRAP,GR_TEXTURECLAMP_WRAP);

	grDepthBufferMode( GR_DEPTHBUFFER_WBUFFER );
	grDepthMask( FXTRUE );

	grChromakeyMode( GR_CHROMAKEY_DISABLE );

	grConstantColorValue( (255 << 24) );
	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
	grAlphaBlendFunction( GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA,
			GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA );

	grDepthBufferFunction( GR_CMP_LESS );

	for( i = 0; i < NUM_PLAYERS; probj++, ppdata++, i++ )
	{
		// on screen?
		if(!(probj->flags & R_ONSCREEN ))
			continue;

		grDepthBiasLevel( 0 );

		// select proper model
		body = ppdata->body_model;

		// Save default hands for later so we don't crash in the unload-reload!
		tmp_hand_r = body[JOINT_RWRIST];
		tmp_hand_l = body[JOINT_LWRIST];

		// Give this player a head on his shoulders
		if( ppdata->f_headsc != 0.0f )
			body[JOINT_NECK] = ppdata->head_model;
		else
			body[JOINT_NECK] = body[JOINT_PELVIS];

		if (body == plyr_limbs_rm)
		{
			body[JOINT_RWRIST] = hand_sets[(probj->hands & MASK_RIGHT_HAND)];
			body[JOINT_LWRIST] = hand_sets[(probj->hands & MASK_LEFT_HAND)>>BITS_HAND];
		}

		render_node_mpc( bbplyr_skel, body, probj->matrices[0], ppdata->odata.pdecal, probj->jpos[0] );

		// Put back the default hands
		body[JOINT_RWRIST] = tmp_hand_r;
		body[JOINT_LWRIST] = tmp_hand_l;

		// Draw jersey #s
		draw_jersey_numbers( ppdata->plyrnum, ppdata->p_spd, ppdata->odata.pdecal, probj->matrices[1] );
	}
}

//////////////////////////////////////////////////////////////////////////////
static void suspend_players( void )
{
	halt = TRUE;
	suspend_multiple_processes( PLAYER_PID, -2 );	// (-2) includes pre_ & last_ player_proc
	suspend_multiple_processes( BALL_PID, -1 );
	suspend_multiple_processes( REFEREE_PID, -1 );

	hide_multiple_objects( OID_GOALS, -1 );
	hide_multiple_objects( OID_PLAYER, -1 );
	hide_multiple_objects( OID_CURSOR, -1 );
	hide_multiple_objects( OID_BALL, -1 );
	hide_multiple_objects( OID_SHADOW, -1 );

	unhide_multiple_objects( OID_REPLAY, -8 );		// (-8) for all replay objs

	hide_status_box = HIDE;
	showhide_flag   = HIDE;
}

//////////////////////////////////////////////////////////////////////////////
static void resume_players( void )
{
	halt = FALSE;
	resume_multiple_processes( PLAYER_PID, -2 );	// (-2) includes pre_ & last_ player_proc
	resume_multiple_processes( BALL_PID, -1 );
	resume_multiple_processes( REFEREE_PID, -1 );

	unhide_multiple_objects( OID_GOALS, -1 );
	unhide_multiple_objects( OID_PLAYER, -1 );
	unhide_multiple_objects( OID_CURSOR, -1 );
	unhide_multiple_objects( OID_BALL, -1 );
	unhide_multiple_objects( OID_SHADOW, -1 );

	hide_multiple_objects( OID_REPLAY, -8 );		// (-8) for all replay objs

	hide_status_box = SHOW;
	showhide_flag   = SHOW;
}

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
void write_replay_data( void )
{
	char	filename[13];
	FILE *	fp = NULL;
	int		num_frames = ((record_head - record_tail + REPLAY_FRAMES) % REPLAY_FRAMES) + 1;
	int		cur_frame = record_head;
	int		i = 0;

	if (record_head < 0 || record_tail < 0)
	{
		fprintf( stderr, "No valid replay data to dump\r\n" );
		return;
	}

	while( fp == NULL )
	{
		sprintf( filename, "RDATA.%03d", i );
		fp = fopen( filename, "rb" );
		if( fp == NULL )
		{
			if((fp = fopen( filename, "wb" )) == NULL )
			{
				fprintf( stderr, "Error opening file %s for writing\r\n", filename );
				return;
			}
		}
		else
		{
			fclose( fp );
			fp = NULL;
			i++;
		}
	}

	fprintf( stderr, "Dumping replay data to file %s\r\n", filename );

//	if( num_frames > 50 )
//		num_frames = 50;

	fprintf( fp, "Replay file %03d, # of frames: %d\r\n", i, num_frames );

	for( i = 0; i < num_frames; i++ )
	{
		fprintf( fp, "\r\nFrame %d\r\n", ((record_head - cur_frame + REPLAY_FRAMES) % REPLAY_FRAMES));
		dump_frame( fp, cur_frame );
		cur_frame = (cur_frame + REPLAY_FRAMES - 1) % REPLAY_FRAMES;
	}

	for( i = 0; i < NUM_PLAYERS; i++ )
	{
		fprintf( fp, "\r\nPlayer %d jersey %02x\r\n", i, player_blocks[i].p_spd->number );
		dump_player( fp, i );
	}

	fflush( fp );

	fclose( fp );
}

//////////////////////////////////////////////////////////////////////////////
static void dump_frame( FILE *fp, int frame )
{
	replay_t *	pframe = &replay_data[frame][PLYR_INDEX];
	int			i, c;

	for( i = 0; i < NUM_PLAYERS; pframe++, i++ )
	{
		fprintf( fp, "%02x: ", player_blocks[i].p_spd->number );

		if( pframe->rp.flags & R_INTSTREAM )
		{
			c = fprintf( fp, "%s:%f\r\n", seq_names[pframe->rp.seq1],
					(float)pframe->rp.frame1 + pframe->rp.t );
		}
		else if( pframe->rp.flags & R_INTTOSEQ )
		{
			c = fprintf( fp, "%s:%d->%s:%d (%f)\r\n",
					seq_names[pframe->rp.seq1],
					pframe->rp.frame1,
					seq_names[pframe->rp.seq2],
					pframe->rp.frame2,
					pframe->rp.t );
		}
		else
		{
			c = fprintf( fp, "%s:%d\r\n", seq_names[pframe->rp.seq1],
					pframe->rp.frame1 );
		}
		if( c < 1 )
		{
			fprintf( stderr, "Error %i writing to debug file\r\n", c );
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
static void dump_player( FILE *fp, int player )
{
	replay_t *	pframe;
	int			num_frames = ((record_head - record_tail + REPLAY_FRAMES) % REPLAY_FRAMES) + 1;
	int			cur_frame = record_head;
	int			i, c;

//	if( num_frames > 50 )
//		num_frames = 50;

	for( i = 0; i < num_frames; i++ )
	{
		pframe = &replay_data[cur_frame][PLYR_INDEX+player];

		fprintf( fp, "%5d: ", ((record_head - cur_frame + REPLAY_FRAMES) % REPLAY_FRAMES));

		if( pframe->rp.flags & R_INTSTREAM )
		{
			c = fprintf( fp, "%s:%f\r\n", seq_names[pframe->rp.seq1],
					(float)pframe->rp.frame1 + pframe->rp.t );
		}
		else if( pframe->rp.flags & R_INTTOSEQ )
		{
			c = fprintf( fp, "%s:%d -> %s:%d (%f)\r\n",
					seq_names[pframe->rp.seq1],
					pframe->rp.frame1,
					seq_names[pframe->rp.seq2],
					pframe->rp.frame2,
					pframe->rp.t );
		}
		else
		{
			c = fprintf( fp, "%s:%d\r\n", seq_names[pframe->rp.seq1],
					pframe->rp.frame1 );
		}

		if( c < 1 )
		{
			fprintf( stderr, "Error %i writing to debug file\r\n", c );
			return;
		}
		cur_frame = (cur_frame + REPLAY_FRAMES - 1) % REPLAY_FRAMES;
	}
}
#endif

//////////////////////////////////////////////////////////////////////////////
