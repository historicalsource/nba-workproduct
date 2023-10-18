/*
 *		$Archive: /video/Nba/detect.c $
 *		$Revision: 138 $
 *		$Date: 4/13/99 4:19p $
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
char *ss_detect_c = "$Workfile: detect.c $ $Revision: 138 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include <stdio.h>
#include <math.h>
#include <goose/goose.h>

/*
 *		USER INCLUDES
 */

#include "include/game.h"
#include "include/player.h"
#include "include/externs.h"

#include "include/plyrseq.h"				// anim sequences
#include "anim/gen_seq.h"					// anim sequences #defines
#include "include/sndcalls.h"
#include "nbacmos.h"

/*
** Global data/prototypes
*/
extern int p_status, four_plr_ver;
extern int pup_no_fouls, pup_no_pushes;
extern int pushed_cnt[NUM_PLAYERS];

extern char gameplay_bnk_str[], plyr_bnk_str[];
extern int in_between_ball_basket( bbplyr_data *);
extern int
	randrng(int),	plyr_bearing( int plyr1, int plyr2 );
extern int no_pump_fake;
extern int juke_flail_allow;

extern int OOBPasser;
extern int plyr_in_front_of_me( obj_3d *pobj );
extern void	loose_ball_speech(obj_3d *);
extern void throw_elbows_speech(void);
extern void push_speech(bbplyr_data *pusher,int hit_ball_handler);
extern void flash_ball( int );
extern float get_dist_to_rim(bbplyr_data *ppd);

extern int get_knockdown_base(bbplyr_data *ppdata);
extern int get_loseball_base(bbplyr_data *def, bbplyr_data *att);

extern void do_freethrow(void);

extern plyr_record_t  * plyr_stat_ptrs[];

/*
** Local data/prototypes
*/
static float line_int_check( float *, float *, float *, float *, float );
static float point2line_d( float *, float *, float  * );
static void do_collision( bbplyr_data *, bbplyr_data *, int );
static void extend_line( float *, float *, float, float, float *, float * , bbplyr_data *);

void flash_foul_meter_proc(int *args);

// player to player distance/angle table
float	distxz[NUM_PLAYERS][NUM_PLAYERS];
int		p2p_angles[NUM_PLAYERS][NUM_PLAYERS];

/*
** Definitions
*/

#define ALLOW_FREETHROWS


#define PASS1_DIST		24.0f				// do no checking unless closer than this
#define BODY_RADIUS		10.0f //5.0f				// body cylinder radius
#define Q8_BEHIND				0
#define Q8_LEFT_BEHIND	1
#define Q8_LEFT					2
#define Q8_LEFT_FRONT		3
#define Q8_FRONT				4
#define Q8_RIGHT_FRONT	5
#define Q8_RIGHT				6
#define Q8_RIGHT_BEHIND	7

#define Q4_BEHIND				0
#define Q4_LEFT					1
#define Q4_FRONT				2
#define Q4_RIGHT				3



//.......0.......
//.......|.......
//...1...|...7...	inside circle is dir1
//....\..0../....	outside circle is dir2
//......\|/......
//2----1-*-3----6
//....../|\......
//..../..2..\....
//...3...|...5...
//.......|.......
//.......4.......
typedef void (*cfunc)( bbplyr_data *, bbplyr_data *, int, int );

static void att_none( bbplyr_data *att, bbplyr_data *def, int dir1, int dir2 );
static void att_swat( bbplyr_data *att, bbplyr_data *def, int dir1, int dir2 );
static void att_dunk( bbplyr_data *att, bbplyr_data *def, int dir1, int dir2 );
static void att_run( bbplyr_data *att, bbplyr_data *def, int dir1, int dir2 );
static void att_elbows( bbplyr_data *att, bbplyr_data *def, int dir1, int dir2 );
static void att_push( bbplyr_data *att, bbplyr_data *def, int dir1, int dir2 );

cfunc cfuncs[] = { att_none, att_swat, att_dunk, att_run, att_elbows, att_push };

int juker_pnum;
int jukee_pnum;

static char *hit_front_tbl[] = {
	h_pushed_front_anim,
	NULL };

static char *flail_front_tbl[] = {
	h_front_flail_anim,
	NULL };

static char *hit_left_side_tbl[] = {
	h_pushed_side_l_anim,
	NULL };

static char *flail_left_side_tbl[] = {
	h_l_side_flail_anim,	
	NULL };

static char *hit_back_tbl[] = {
	h_pushed_back_anim,
	NULL };

static char *flail_back_tbl[] = {
	h_back_flail_anim,
	NULL };

static char *hit_right_side_tbl[] = {
	h_pushed_side_r_anim,
	NULL };

static char *flail_right_side_tbl[] = {
	h_r_side_flail_anim,
	NULL };


static char **std_hit_tbls[] = {
	hit_front_tbl,
	hit_right_side_tbl,
	hit_back_tbl,
	hit_left_side_tbl };

static char **std_flail_tbls[] = {
	flail_front_tbl,
	flail_right_side_tbl,
	flail_back_tbl,
	flail_left_side_tbl };


static char *hit_stumble_anim_tbl[] = {
	h_back_stumble_anim,	// front stumble...shouldnt happen...but just in case
	h_side_l_stumble_anim,
	h_back_stumble_anim,
	h_side_r_stumble_anim,
	NULL };


static char *fouled_push_tbl[] = {
	h_pushed_front3_anim,
	h_pushed_side2_r_anim,
	h_pushed_back2_anim,
	h_pushed_side2_l_anim };


///////////////////////////////////////////////////////////////////////////////
//
// player_detect_proc() - does player-to-player collision detection
//
void player_detect_proc(int *parg)
{
	int		i, j;
	float	ext1[3], ext2[3], dist;
	float	low_body_focus[NUM_PLAYERS][3];
	bbplyr_data	* ppdatai;
	bbplyr_data	* ppdataj;

	while( TRUE )
	{
		sleep( 1 );

		// Reset juke pnums
		juker_pnum = NO_JUKER;
		jukee_pnum = NO_JUKEE;

		ppdatai = player_blocks;

		// compute all player distances & angles
		for( i = 0; i < NUM_PLAYERS; ppdatai++, i++ )
		{
			distxz[i][i] = 0.0f;
			p2p_angles[i][i] = 0;

			ppdataj = ppdatai + 1;

			for( j = i + 1; j < NUM_PLAYERS; ppdataj++, j++ )
			{
				dist = fsqrt(
					(ppdatai->odata.x - ppdataj->odata.x) * (ppdatai->odata.x - ppdataj->odata.x) +
					(ppdatai->odata.y - ppdataj->odata.y) * (ppdatai->odata.y - ppdataj->odata.y) + 
					(ppdatai->odata.z - ppdataj->odata.z) * (ppdatai->odata.z - ppdataj->odata.z));
				distxz[i][j] = dist;
				distxz[j][i] = dist;
				p2p_angles[i][j] = ptangle( &(ppdatai->odata.x), &(ppdataj->odata.x) );
				p2p_angles[j][i] = (p2p_angles[i][j] + 512) & 1023;
			}

//MJT
//			ppdatai->odata.pobj_hit = 0;	// Clr before collision chk

		}

		// body cylinder connects neck and midpoint of knees & pelvis.  compute these in advance
		for( i = 0; i < NUM_PLAYERS; i++ )
		{
			low_body_focus[i][0] = (player_blocks[i].jpos[JOINT_NECK][0] + 20.0f +
									player_blocks[i].jpos[JOINT_RANKLE][0] +
									player_blocks[i].jpos[JOINT_LANKLE][0]) / 3.0f;

			low_body_focus[i][1] = (player_blocks[i].jpos[JOINT_NECK][1] + 
									player_blocks[i].jpos[JOINT_RANKLE][1] +
									player_blocks[i].jpos[JOINT_LANKLE][1]) / 3.0f;

			low_body_focus[i][2] = (player_blocks[i].jpos[JOINT_NECK][2] +
									player_blocks[i].jpos[JOINT_RANKLE][2] +
									player_blocks[i].jpos[JOINT_LANKLE][2]) / 3.0f;
		}

		ppdatai = player_blocks;

		for( i = 0; i < NUM_PLAYERS; ppdatai++, i++ )
		{


//MJT
			ppdatai->odata.pobj_hit = 0;	// Clr before collision chk



			if( ppdatai->odata.flags & PF_NOCOLL )
				continue;

			ppdataj = ppdatai + 1;

			for( j = i + 1; j < NUM_PLAYERS; ppdataj++, j++ )
			{
				ppdataj = player_blocks + j;

// ? about this hack
//				if(!((ppdataj->odata.flags | ppdatai->odata.flags) & (PF_SWATING)) && distxz[i][j] > PASS1_DIST )
//					continue;

				if( ppdataj->odata.flags & PF_NOCOLL )
					continue;

				// close enough -- perform more detailed check

				// first check for attack cylinders vs body cylinders.  i attacking j
				// team 0 attacking team 1
				if( ppdatai->attack_mode )
				{
					if( ppdatai->acj1 == ppdatai->acj2 )
					{
						// spherical attack zone -- not yet supported
#ifdef DEBUG
						fprintf( stderr, "Error: spherical attack zones not supported.\n" );
						lockup();
#endif
					}
					else
					{
						extend_line(
							ppdatai->jpos[ppdatai->acj1],
							ppdatai->jpos[ppdatai->acj2],
							ppdatai->acx1, ppdatai->acx2,
							ext1, ext2, ppdatai);
						if( line_int_check( ext1, ext2, ppdataj->jpos[0],
							low_body_focus[j], 5.0f + ppdatai->acr ))
						{
							// collision
							do_collision( ppdatai, ppdataj, ppdatai->attack_mode );
						}
					}
				}

				// j attacking i, team 1 attacking team 0
				if( ppdataj->attack_mode )
				{
					if( ppdataj->acj1 == ppdataj->acj2 )
					{
						// spherical attack zone -- not yet supported
#ifdef DEBUG
						fprintf( stderr, "Error: spherical attack zones not supported.\n" );
						lockup();
#endif
					}
					else
					{
						extend_line(
							ppdataj->jpos[ppdataj->acj1],
							ppdataj->jpos[ppdataj->acj2],
							ppdataj->acx1, ppdataj->acx2,
							ext1, ext2, ppdataj);
						if( line_int_check( ext1, ext2,	ppdatai->jpos[0],
							low_body_focus[i], 5.0f + ppdataj->acr ))
						{
							// collision
							do_collision( ppdataj, ppdatai, ppdataj->attack_mode );
							continue;
						}
					}
				}

//				if((ppdataj->odata.flags | ppdatai->odata.flags) & (PF_SWATING))
//				{
//				// I want a 2d check first
//					float d = fsqrt(
//						(ppdatai->odata.x - ppdataj->odata.x) * (ppdatai->odata.x - ppdataj->odata.x) +
//						(ppdatai->odata.z - ppdataj->odata.z) * (ppdatai->odata.z - ppdataj->odata.z));
//				
//				// no body collisions if either guy has the MODE_NOCYL bit set
//					if(( ppdatai->odata.adata[0].animode | ppdataj->odata.adata[0].animode ) & MODE_NOCYL )
//						continue;
//
//				// now check body vs body
//					if( /*distxz[i][j]*/d < ( 2.0f * BODY_RADIUS ))
//					{
///						do_collision( ppdatai, ppdataj, ATTMODE_SWAT );
//					}
//				}	
//				else if( !((ppdataj->odata.flags | ppdatai->odata.flags) & (PF_SWATING)) )
//				{	
//					// no body collisions if either guy has the MODE_NOCYL bit set
//					if(( ppdatai->odata.adata[0].animode | ppdataj->odata.adata[0].animode ) & MODE_NOCYL )
//						continue;
//
				// now check body vs body
//				if (i==2 && j==1)
//					printf("distxz %2.2f\n",distxz[i][j]);
				else if( distxz[i][j] <= BODY_RADIUS )
//				else if( distxz[i][j] < ( 2.0f * BODY_RADIUS ))
				{
					do_collision( ppdatai, ppdataj, ATTMODE_NONE );
				}
//				}	
			}//END.FOR
		}//END.FOR
	}//END.WHILE
}

//////////////////////////////////////////////////////////////////////////////
// line_int_check() - returns TRUE if the distance between lines a and b,
// defined by endpoints a1,a2 and b1,b2, at their point of closest approach,
// is less than or equal to d.
//////////////////////////////////////////////////////////////////////////////
#define CLAMP(x,lo,hi) ((x<lo) ? lo : ((x>hi) ? hi : x))

static float line_int_check( float *a1, float *a2, float *b1, float *b2, float d )
{
	vec3d	v1,v2;
	float	m[12];
	float	t,s,mag,tt,ss;
	float	dx,dy,dz,dist;
	int		bad_t = FALSE, bad_s = FALSE;
	float	*pp1,*pp2, p1[3], p2[3];

	v1.x = a2[0] - a1[0];
	v1.y = a2[1] - a1[1];
	v1.z = a2[2] - a1[2];
	v1.w = 1.0f;

	v2.x = b2[0] - b1[0];
	v2.y = b2[1] - b1[1];
	v2.z = b2[2] - b1[2];
	v2.w = 1.0f;

	m[0] = b1[0] - a1[0];
	m[1] = b1[1] - a1[1];
	m[2] = b1[2] - a1[2];

	vxv( &v1, &v2, (vec3d *)(m+8) );

	mag = magv( (vec3d *)(m+8) );

	if( mag == 0.0f )
	{
		// lines are parallel
		s = point2line_d( b1, b2, a1 );

		dx = b1[0] + s*(b2[0]-b1[0]) - a1[0];
		dy = b1[1] + s*(b2[1]-b1[1]) - a1[1];
		dz = b1[2] + s*(b2[2]-b1[2]) - a1[2];

		if((dx*dx + dy*dy + dz*dz) <= d*d )
			return TRUE;
		else
			return FALSE;
	}

	m[4] = v2.x;
	m[5] = v2.y;
	m[6] = v2.z;

	t = det( m ) / (mag*mag);

	m[4] = v1.x;
	m[5] = v1.y;
	m[6] = v1.z;

	s = det( m ) / (mag*mag);

	// useless.  this init just prevents a warning
	pp1 = a1; pp2 = b1;

	if( t > 1.0f )
	{
		t = 1.0f;
		pp1 = a2;
		bad_t = TRUE;
	}
	if( t < 0.0f )
	{
		t = 0.0f;
		pp1 = a1;
		bad_t = TRUE;
	}
	if( s > 1.0f )
	{
		s = 1.0f;
		pp2 = b2;
		bad_s = TRUE;
	}
	if( s < 0.0f )
	{
		s = 0.0f;
		pp2 = b1;
		bad_s = TRUE;
	}

	if( bad_t && bad_s )
	{
		// both t and s were out of range, and both have been capped.
		// the shortest distance between the two segments ends with one
		// of these endpoints, but we have to check both to figure out
		// which.
		tt = point2line_d( a1, a2, pp2 );
		tt = CLAMP( t, 0.0f, 1.0f );
		p1[0] = a1[0] + tt*(a2[0]-a1[0]);
		p1[1] = a1[1] + tt*(a2[1]-a1[1]);
		p1[2] = a1[2] + tt*(a2[2]-a1[2]);

		dist = (pp2[0]-p1[0])*(pp2[0]-p1[0]) +
				(pp2[1]-p1[1])*(pp2[1]-p1[1]) +
				(pp2[2]-p1[2])*(pp2[2]-p1[2]);

		if( dist <= d*d )
			return TRUE;

		ss = point2line_d( b1, b2, pp1 );
		ss = CLAMP( ss, 0.0f, 1.0f );
		p2[0] = b1[0] + ss*(b2[0]-b1[0]);
		p2[1] = b1[1] + ss*(b2[1]-b1[1]);
		p2[2] = b1[2] + ss*(b2[2]-b1[2]);

		dist = (pp1[0]-p2[0])*(pp1[0]-p2[0]) +
				(pp1[1]-p2[1])*(pp1[1]-p2[1]) +
				(pp1[2]-p2[2])*(pp1[2]-p2[2]);

		if( dist <= d*d )
			return TRUE;

		return FALSE;
	}
	else if( bad_t )
	{
		// t was out of range, s was in.  t has been capped and is
		// definitely the point on line1 closest to line2.  Recompute
		// s based on new capped t.
		s = point2line_d( b1, b2, pp1 );
		s = CLAMP( s, 0.0f, 1.0f );

		p2[0] = b1[0] + s*(b2[0]-b1[0]);
		p2[1] = b1[1] + s*(b2[1]-b1[1]);
		p2[2] = b1[2] + s*(b2[2]-b1[2]);
		pp2 = p2;
	}
	else if( bad_s )
	{
		t = point2line_d( a1, a2, pp2 );
		t = CLAMP( t, 0.0f, 1.0f );

		p1[0] = a1[0] + t*(a2[0]-a1[0]);
		p1[1] = a1[1] + t*(a2[1]-a1[1]);
		p1[2] = a1[2] + t*(a2[2]-a1[2]);

		pp1 = p1;
	}
	else
	{
		p1[0] = a1[0] + t*(a2[0]-a1[0]);
		p1[1] = a1[1] + t*(a2[1]-a1[1]);
		p1[2] = a1[2] + t*(a2[2]-a1[2]);

		p2[0] = b1[0] + s*(b2[0]-b1[0]);
		p2[1] = b1[1] + s*(b2[1]-b1[1]);
		p2[2] = b1[2] + s*(b2[2]-b1[2]);

		pp1 = p1;
		pp2 = p2;
	}

	dist = (pp2[0]-pp1[0])*(pp2[0]-pp1[0]) +
			(pp2[1]-pp1[1])*(pp2[1]-pp1[1]) +
			(pp2[2]-pp1[2])*(pp2[2]-pp1[2]);

	if( dist <= d*d )
		return TRUE;

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// point2line_d() - returns t for the point on a line L1L2 closest to point P.
//////////////////////////////////////////////////////////////////////////////
static float point2line_d( float *l1, float *l2, float *p )
{
	float v[3],rmp[3];
	float t;

	v[0] = l2[0] - l1[0];
	v[1] = l2[1] - l1[1];
	v[2] = l2[2] - l1[2];

	rmp[0] = p[0] - l1[0];
	rmp[1] = p[1] - l1[1];
	rmp[2] = p[2] - l1[2];

	t = (rmp[0]*v[0] + rmp[1]*v[1] + rmp[2]*v[2]) / (v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );

	return t;
}

//////////////////////////////////////////////////////////////////////////////
// extend line() - Given a line defined by endpoints p1 and p2, extend the
// endpoints by distances x1 and x2, respectively, and store the extended
// endpoints in xp1 and xp2.
static void extend_line( float *p1, float *p2, float x1, float x2, float *xp1, float *xp2, bbplyr_data *ppdata )
{
	float	v[3];
	float	mag;


	if (ppdata->acj1 == JOINT_BWRIST)
	{
		p1[0] = (ppdata->jpos[JOINT_LWRIST][0] + ppdata->jpos[JOINT_RWRIST][0]) / 2.0f;
		p1[1] = (ppdata->jpos[JOINT_LWRIST][1] + ppdata->jpos[JOINT_RWRIST][1]) / 2.0f;
		p1[2] = (ppdata->jpos[JOINT_LWRIST][2] + ppdata->jpos[JOINT_RWRIST][2]) / 2.0f;
	}
	if (ppdata->acj2 == JOINT_BWRIST)
	{
		p2[0] = (ppdata->jpos[JOINT_LWRIST][0] + ppdata->jpos[JOINT_RWRIST][0]) / 2.0f;
		p2[1] = (ppdata->jpos[JOINT_LWRIST][1] + ppdata->jpos[JOINT_RWRIST][1]) / 2.0f;
		p2[2] = (ppdata->jpos[JOINT_LWRIST][2] + ppdata->jpos[JOINT_RWRIST][2]) / 2.0f;
	}
	
	v[0] = p2[0] - p1[0];
	v[1] = p2[1] - p1[1];
	v[2] = p2[2] - p1[2];

	mag = 1.0f / fsqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
	v[0] *= mag;	v[1] *= mag;	v[2] *= mag;

	xp1[0] = p1[0] - v[0] * x1;
	xp1[1] = p1[1] - v[1] * x1;
	xp1[2] = p1[2] - v[2] * x1;

	xp2[0] = p2[0] + v[0] * x2;
	xp2[1] = p2[1] + v[1] * x2;
	xp2[2] = p2[2] + v[2] * x2;
}

//////////////////////////////////////////////////////////////////////////////
// do_collision() - All collisions come through here.
static void do_collision( bbplyr_data *att, bbplyr_data *def, int mode )
{
	int dir1, dir2;
	// TODO: have this use actual angle of impact, not just attacker facing
	dir2 = att->odata.fwd - def->odata.fwd;

	// No need for while loops!
	dir2 &= 1023;
	//while (dir2 < 0) dir2 += 1024;					// fix if it needs to be
	//while (dir2 >= 2147481600) dir2 -= 1024;		// fix if it needs to be (k = 2^31 - 2048)

	dir1 = ((dir2 + 512 + 128) & 1023) / 256;		// four directions
	dir2 = ((dir2 + 512 + 64)  & 1023) / 128;		// eight directions

	def->odata.pobj_hit = (obj_3d *)att;
	att->odata.pobj_hit = (obj_3d *)def;

	cfuncs[mode]( att, def, dir1, dir2 );
}

//////////////////////////////////////////////////////////////////////////////
static void att_none( bbplyr_data *att, bbplyr_data *def, int dir1, int dir2 )
{
	// push both away along the line connecting them.  Each moves a distance
	// proportional to the dot product of their velocity with the connecting
	// line.
	vec3d	c;
	float	da,dd,dy,m=0.0f,mag;
	bbplyr_data	*tmp;

//	if((att->odata.flags | def->odata.flags) & (PF_SWATING))
//	{
//		printf("somebody is swating in att_none\n");
//	}
	// swap such that def is team with ball
	if((att->team == game_info.off_side) && att->team != def->team)
	{
		tmp = att;
		att = def;
		def = tmp;
	}

//	dy = fabs( att->odata.y - def->odata.y );
//
//	// too much y difference?
//	if( dy > 30.0f )
//		return;

	// if a player is in "OUT_OF_BOUNDS" mode ignore collisions
	//if(att->odata.plyrmode == MODE_OUT_OF_BOUNDS || def->odata.plyrmode == MODE_OUT_OF_BOUNDS)
	//	return;

	c.x = def->odata.x - att->odata.x;
	c.z = def->odata.z - att->odata.z;
	mag = c.x*c.x + c.z*c.z;

	if( mag < 0.001f )
	{
		c.x  = 1.0f;
		c.z = 0.0f;
	}
	else
	{
		m = 1.0f / fsqrt( mag );
		c.x *= m;
		c.z *= m;
	}

	da = fabs( att->odata.vx * c.x + att->odata.vz * c.z );
	dd = fabs( def->odata.vx * c.x + def->odata.vz * c.z );

	// if neither has any velocity along the line connecting them, set them equal
//	if(( fabs(da) < 0.001 ) && ( fabs(dd) < 0.001 ))
//	{
//		da = 1.0f;
//		dd = 1.0f;
//	}

	if(def->odata.flags & (PF_SHOOTING|PF_LAYUP|PF_DUNKER|PF_ALLYDUNKER))
		dd = 0.0f;			// offense wins
	else if(att->odata.flags & (PF_SWATING|PF_IN_AIR_BLOCK))
		dd = 0.0f;			// offense wins
//	else if((def->odata.flags & (PF_CATCHING|PF_PASSING)) || (def->plyrnum == ball_obj.int_receiver && game_info.ball_mode == BM_PASSING))
//		dd = 0.0f;			// offense wins

//	else if((def->plyrnum != game_info.ball_handler) && (def->team == game_info.off_side))
//		da = 0.0f;			// defense wins
//	else if ((game_info.ball_handler != NO_BALLHANDLER) && (att->team != def->team))
//		da = 0.0f;			// defense wins

	if(def->odata.flags & (PF_FUNKY_DRIBBLE))
		da += 0.50f;

	if( att->team != def->team )
	{
		if(def->odata.flags & (PF_FUNKY_DRIBBLE))
		{
			//da += 0.50f;	// 1.0f;
			// Note: plyr_bearing() returns the ABS() of the facing-angles diff
			//if(pobj->adata[0].seq_index == (SEQ_D_BKN2_R)

			if(!(att->odata.adata[0].animode & (MODE_REACTING)) &&
			   (in_between_ball_basket(att) == YES) &&
			   (juke_flail_allow != 0) &&
				 (def->action <= 2)
				 )			// Only flail opponents in first x (30) ticks of any juke move
			{
				if(randrng(100) <= 50)
				{
					if (plyr_in_front_of_me((obj_3d *)def))
					{
	//					change_anim((obj_3d *)att, h_front_flail_anim);
						change_anim((obj_3d *)att, randrng(100) < 75 ? h_front_flail_anim : dfake1_butt_anim);
						def->action++;	// inc. players juke cnt
					}
					else
					{
						//if(plyr_bearing(def->plyrnum, att->plyrnum) < 250)
							change_anim((obj_3d *)att, randrng(100) < 50 ? h_r_side_flail_anim : h_l_side_flail_anim);
							def->action++;	// inc. players juke cnt
					}	

					juker_pnum = def->plyrnum;
					jukee_pnum = att->plyrnum;
				}
			}
				//snd_scall_bank(gameplay_bnk_str,FLAIL1_SND, 70/*VOLUME1*/,127,PRIORITY_LVL1);
		}
	}
//			else
//			{
//				da += 0.15f;	// opposing team moves
//				if(game_info.ball_mode == BM_PASSING
//					&& def->plyrnum == ball_obj.int_receiver)
//				{
//					dd = 0.0f;
//					da = 1.0f;
//				}
//			}

	if(game_info.game_mode == GM_INBOUNDING)
	{
		dd = 1.0f, da = 0.0f;
		if(att->team == def->team)
			dd = da = 1.0f;
	}

	m = 1.0f / (da + dd);
	m *= ((BODY_RADIUS) - distxz[att->plyrnum][def->plyrnum]);
	da *= m;
	dd *= m;

//printf("att b4 add x  = %2.2f  z = %2.2f\n",att->odata.x,att->odata.z);

	att->odata.x -= da * c.x;
	att->odata.z -= da * c.z;

//printf("att after add x  = %2.2f  z = %2.2f\n",att->odata.x,att->odata.y);
	def->odata.x += dd * c.x;
	def->odata.z += dd * c.z;

//printf("da = %2.2f           dd = %2.2f\n",da,dd);

//	if ((att->odata.flags & PF_IN_AIR_BLOCK) || (att->odata.flags & PF_SWATING))
//		printf("da = %2.2f   dd = %2.2f   m = %2.2f    c.x = %d    c.y = %d\n",da,dd,m,c.x,c.y);
}

//////////////////////////////////////////////////////////////////////////////
static void att_swat( bbplyr_data *att, bbplyr_data *def, int dir1, int dir2 )
{
	// push both away along the line connecting them.  Each moves a distance
	// proportional to the dot product of their velocity with the connecting
	// line.
	vec3d	c;
	float	da,dd,m,dist;
	bbplyr_data	*tmp;

//	if( att->odata.y != def->odata.y )
//		return;

	// swap such that def is team with ball
	if((att->team == game_info.off_side))
	{
		tmp = att;
		att = def;
		def = tmp;
	}

// no collisions with players hanging on the rim or coming off of the rim
// in this case att is the team that just scored
	if((/*def->flight_time >= (def->total_time - 20)
		||*/ def->HangingOnRim == YES)
		//|| (def->odata.flags & (PF_DUNKER) && game_info.ball_handler == NO_BALLHANDLER))
		|| 
		(/*att->flight_time >= (att->total_time - 20)
		||*/ att->HangingOnRim == YES)
		//|| (att->odata.flags & (PF_DUNKER) && game_info.ball_handler == NO_BALLHANDLER))
		)
	{
#ifdef DEBUG
		fprintf(stderr, "plyr %d is swating in att_swat\n",att->plyrnum);
#endif
		return;
	}

	if(att->odata.flags & (PF_SWATING))
	{
		;//printf("plyr %d is swating in att_swat\n",att->plyrnum);
	}

	if( ((att->odata.flags | def->odata.flags) & (PF_DUNKER|PF_ALLYDUNKER)) )
	{
	int my_chance = 50 + (40.0f * ((float)att->_power / 20.0f)),
			his_chance = 50 + (40.0f * ((float)def->_power / 20.0f));
		
		if(randrng(my_chance) < randrng(his_chance))	//70)
		{	// knock over the dunker
			// blocker is busy...ignore
			if( ((obj_3d *)def)->adata[0].animode & (MODE_REACTING))
			{
				return;
			}
			//printf("ATT_SWAT: knocking down dunker\n");
			//def->odata.flags &= ~(PF_REBOUND|PF_DUNKER|PF_ALLYDUNKER|PF_SWATING);
			snd_scall_bank(gameplay_bnk_str,FLAIL1_SND,VOLUME4,127,PRIORITY_LVL2);
		}
		else
		{	// knock over the blocker
			// blocker is busy...ignore
			if( att->odata.adata[0].animode & (MODE_REACTING))
			{
				return;
			}
		 	change_anim((obj_3d *)att, h_front_flail_anim);
			att->odata.flags &= ~(PF_REBOUND|PF_DUNKER|PF_ALLYDUNKER|PF_SWATING);
			snd_scall_bank(gameplay_bnk_str,FLAIL1_SND,VOLUME4,127,PRIORITY_LVL2);
		}
	}
	else
	{
		//printf("COLLISIONS in ATT_SWAT: att->plyr %d, def->plyr %d\n",att->plyrnum, def->plyrnum);
		c.x = def->odata.x - att->odata.x;
		c.z = def->odata.z - att->odata.z;
		dist = fsqrt( c.x * c.x + c.z * c.z );
		m = 1.0f / dist;
		c.x *= m;
		c.z *= m;

		da = fabs( att->odata.vx * c.x + att->odata.vz * c.z );
		dd = fabs( def->odata.vx * c.x + def->odata.vz * c.z );
	
		// if neither has any velocity along the line connecting them, set them equal
		if(( fabs(da) < 0.001 ) && ( fabs(dd) < 0.001 ))
		{
			da = 1.0f;
			dd = 1.0f;
		}

		// if on opposing teams, make the defender do most of the moving.
		if( att->team != def->team )
		{

			if(def->odata.plyrmode == MODE_OFFENSE)
			{
				da += 0.5f;
			}
			else if(def->odata.plyrmode == MODE_DEFENSE)
			{
				da += 0.0f;
			}	
		}
	
		m = 1.0f / (da + dd);
		m *= ((2.0f * BODY_RADIUS) - dist);
		da *= m;
		dd *= m;

		att->odata.x -= da * c.x;
		att->odata.z -= da * c.z;
	
		def->odata.x += dd * c.x;
		def->odata.z += dd * c.z;
	}
}

//////////////////////////////////////////////////////////////////////////////
static void att_dunk( bbplyr_data *att, bbplyr_data *def, int dir1, int dir2 )
{

	bbplyr_data	*tmp;
	int bearing,att_pow,def_pow;
	vec3d	c;
	float	da,dd,m,dist;
	static char * ground_reactions_tbl[] = {
		dfake1_butt_anim,//h_dunker_hit_me_r_anim,
		h_dunker_hit_me_r_anim,
		h_back_flail_anim,
		h_dunker_hit_me_l_anim
	};
	static char * air_reactions_tbl[] = {
		h_dunker_hit_me_r_anim,
		h_dunker_hit_me_r_anim,
		h_back_flail_anim,
		h_dunker_hit_me_l_anim
	};


	if((att->team == game_info.off_side))
	{
		tmp = att;
		att = def;
		def = tmp;
	}

	// ignore this if we're on the same team
	if(att->team == def->team)
		return;

	// dont hit more than once
	if(att->odata.adata[0].animode & MODE_REACTING)
		return;

	// dont flail, someone thats behind me
	bearing = (def->odata.fwd - ptangle( &(def->odata.x), &(att->odata.x))) & 1023;
	if(bearing >= 300 && bearing <= 724)
	{
		return;
	}

	// clear defender's attack mode
	def->attack_mode = 0;
	def->attack_time = 0;

	// dont collide if close to rim
	if( (.75f * (float)def->total_time) < def->flight_time)
		return;

	// is player jumping or standing on ground ?
	def_pow = player_blocks[def->plyrnum]._power;
	att_pow = player_blocks[att->plyrnum]._power;
	if (randrng(100) < 50)
	{
		if (def->odata.y < 2.0f)
			change_anim((obj_3d *)att, ground_reactions_tbl[dir1]);
		else
			change_anim((obj_3d *)att, air_reactions_tbl[dir1]);
	}
}

//////////////////////////////////////////////////////////////////////////////
static void att_push( bbplyr_data *att, bbplyr_data *def, int dir1, int dir2 )
{
	char	**anim_tbl;
	int		rnd;
	int		angle;
	float	dist, tx, tz, time;


	if (pup_no_pushes)
		return;

	// ignore this if we're on the same team
	if((att->team == def->team) && (randrng(100) > 4))
		return;

	// dont hit more than once
	if(def->odata.adata[0].animode & MODE_REACTING)
		return;

	// to far to hit ?
	if (distxz[att->plyrnum][def->plyrnum] > 20.0f)		//23
		return;

	// !!!FIX
	// need rule here for defender doing a one-on-one move....
	// also if dude is big...increase chance of flail.
	if(def->odata.flags & PF_FUNKY_DRIBBLE)
	{
	int chance = 80, jchance = 100, att_score = compute_score(att->team), def_score = compute_score(def->team);

		if(att_score > def_score)
			chance = 90;

		if(def->action == 2)
			jchance = 50;

		if(def->action > 2)
			jchance = 5;

		if(randrng(100) < chance && randrng(100) <= jchance)
		{
			// pusher...lost against the one-on-one move.... shake em'
			// clear attacker's attack mode and dont allow defender to be knocked down
			att->attack_mode = 0;
			att->attack_time = 0;

			if(plyr_in_front_of_me((obj_3d *)def))
			{
				change_anim((obj_3d *)att, dfake1_butt_anim);
				def->action++;	// inc. players juke cnt
			}
			else
			{
				// if pusher is less than 90 units away from active rim allow him to be flailed...

				if(get_dist_to_rim(att) <= 90.0f)
				{
					if(plyr_bearing(def->plyrnum, att->plyrnum) < 250)
					{
						change_anim((obj_3d *)att, randrng(100) < 50 ? h_dunker_hit_me_r_anim : h_dunker_hit_me_l_anim);
						def->action++;	// inc. players juke cnt
					}
				}
				else if(get_dist_to_rim(att) <= get_dist_to_rim(def))
				{
				// if pusher is far away from active rim don't allow him to be flailed...
					if(plyr_bearing(def->plyrnum, att->plyrnum) < 250)
					{
						change_anim((obj_3d *)att, randrng(100) < 50 ? h_dunker_hit_me_r_anim : h_dunker_hit_me_l_anim);
						def->action++;	// inc. players juke cnt
					}
				}
#ifdef DEBUG
				else
					printf("was gonna get flailed\n");
#endif	
			}

			juker_pnum = def->plyrnum;
			jukee_pnum = att->plyrnum;

			flash_ball(4);

			return;
		}
	
		if(def->action > 2 && randrng(100) < 80)
			return;
	}

	// clear all necessary player flags...
	att->odata.flags &= ~(PF_PLYRFLAGS);
	def->odata.flags &= ~(PF_PLYRFLAGS);

	// clear defender's attack mode
	def->attack_mode = 0;
	def->attack_time = 0;
	att->attack_mode = 0;
	att->attack_time = 0;

	// inc. game stat
	pushed_cnt[def->plyrnum] += 1;

	// this is here to make sure that the player is able to dribble...if he kept ball after pushed and was passing
	if(def->plyrnum == game_info.ball_handler)
	{
		game_info.passer = NO_PASSER;
		//ball_obj.who_shot   = NO_SHOOTER;
		game_info.ball_mode = BM_DRIBBLING;
		def->dribble_mode = DM_CAN_DRIBBLE;
		no_pump_fake = YES;
		flash_ball(4);
	}

	// with this set his pass button will be ignored if he has ball
	if(game_info.ball_handler == def->plyrnum)
		def->flail_time = 25;

	// on push that will cause FOUL... always do falldown and keepball

#ifdef ALLOW_FREETHROWS
	if ((def->team != att->team) &&
		(def->plyrnum == game_info.ball_handler) &&
		(ISHUMAN(att)) &&
		(game_info.game_time > 0x00000200) &&
		(!pup_no_fouls) &&
		!((game_info.fire_player == att->plyrnum && game_info.fire_cnt >= FIRECNT) || (game_info.team_fire == att->team)) &&
		(game_info.game_state != GS_ATTRACT_MODE))
	{
		game_info.foul_cnt[att->plyrnum]++;

		snd_scall_bank(gameplay_bnk_str,133,VOLUME7,127,PRIORITY_LVL4);
		qcreate("fl_foul", 0, flash_foul_meter_proc, att->plyrnum, 0, 0, 0);
		
		push_speech(att,1);
	}
	else
		push_speech(att,0);
		
#endif

	// in a one player game, allow human to go to line when a drone pushes him down late in the game - and drone is winning
	if (game_info.foul_cnt[att->plyrnum] >= FOUL_LIMIT
		|| ((ISDRONETEAM(att) == YES)
		&& ISHUMAN(def)
		&& (def->plyrnum == game_info.ball_handler)
		&& (def->odata.flags & PF_ONSCREEN)
		&& (game_info.game_quarter == 1 || game_info.game_quarter >= 3)
		&& compute_score(att->team) > compute_score(def->team)
		&& (((game_info.game_time & 0x00FF0000)>> 16) == 0 && ((game_info.game_time & 0x0000FF00) >> 8) <= 50)
		&& randrng(100) < 10))		//10
	{
		game_info.foul_cnt[att->plyrnum] = 0;			// reset his foul count
		game_info.freethrow_shooter = def->plyrnum;
		change_anim((obj_3d *)def, fouled_push_tbl[dir1]);
		do_freethrow();
	}
	else
	{
		// push the dude...but sometimes do running stumbles
		if((dir1 != 0) &&
			(def->odata.y == 0.0f) &&
			(def->plyrnum != game_info.ball_handler) &&
			(randrng(100) < 40) &&
			(fsqrt(def->odata.vx*def->odata.vx + def->odata.vz*def->odata.vz) > 1.00f))
		{	// DO a stumble run!
			change_anim((obj_3d *)def, hit_stumble_anim_tbl[dir1]);
		}
		else if ((randrng(100) > get_knockdown_base(att)) &&
				!((game_info.fire_player == def->plyrnum && game_info.fire_cnt >= FIRECNT) || (game_info.team_fire == def->team)))
//		else if(randrng(1000) <= (500 + (att->_power - def->_power) * 25))
		{	// do knockdown

			anim_tbl = std_hit_tbls[dir1];
			for(rnd=0; anim_tbl[rnd]; rnd++);
			rnd = randrng(rnd);
			change_anim( (obj_3d *)def, anim_tbl[rnd]);
		}
		else
		{	// do a flail reaction

			anim_tbl = std_flail_tbls[dir1];
			for(rnd = 0; anim_tbl[rnd]; rnd++);
			rnd = randrng(rnd);
			change_anim( (obj_3d *)def, anim_tbl[rnd]);
		}

		//
		// sometimes...lose ball
		//
		if((randrng(100) <= get_loseball_base(def, att))
		&& (game_info.ball_handler != NO_BALLHANDLER)
		&& (OOBPasser == NO_PASSER)
		// If on fire, lose ball only 25% of time when flailed by a pusher
		&& !(((game_info.fire_player == def->plyrnum && game_info.fire_cnt >= FIRECNT) || (game_info.team_fire == def->team)) && randrng(100) < 25 ) 
		&& (game_info.ball_handler == def->plyrnum))
		{
			// inc. game stat
			plyr_stat_ptrs[game_info.ball_handler]->turnovers++;

			def->flail_time = 0;
			def->no_ball_collide = 15;
			def->dribble_mode = DM_CAN_DRIBBLE;	

			dist = 3.0f;		//4
			time = 4.0f;
			angle = def->odata.fwd - 64;
			angle -= randrng(256);
			tx = ball_obj.odata.x + (dist * isin(angle));
			tz = ball_obj.odata.z + (dist * icos(angle));

			ball_obj.odata.vx = (tx - ball_obj.odata.x) / (float)time;
			ball_obj.odata.vz = (tz - ball_obj.odata.z) / (float)time;
			ball_obj.odata.vy = ((float)randrng(1600)) / 1000.0f;
			//ball_obj.flight_time = 0;
			//ball_obj.total_time = 0;	//time;

			game_info.ball_handler = NO_BALLHANDLER;	// clear so ball can go flying
			game_info.ball_mode = BM_FREE_ON_COURT;
			//ball_obj.who_shot   = NO_SHOOTER;
			game_info.passer    = NO_PASSER;
			no_pump_fake = YES;

			loose_ball_speech((obj_3d *)def);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
static void att_run( bbplyr_data *att, bbplyr_data *def, int dir1, int dir2 )
{
	bbplyr_data	*tmp;
	int rnd = 0, quad8, quad4;
	char **pup_tbl;


	// clear attacker's attack mode
//	att->attack_mode = 0;
//	att->attack_time = 0;
//
//	att_none(att,def,dir1,dir2);

	// ignore this if we're on the same team
//	if( att->team == def->team )
//		return;

	// swap such that def is team with ball
	if((att->team == game_info.off_side))
	{
		tmp = att;
		att = def;
		def = tmp;
	}

	//if(def->odata.vx != 0.0f || def->odata.vz != 0.0f)
	//	return;

	if( def->odata.adata[0].animode & (MODE_REACTING))
		return;

	quad8 = def->odata.fwd - p2p_angles[def->plyrnum][att->plyrnum];
	quad4 = ((quad8 + 512 + 128) & 1023) / 256;		// four directions
	quad8 = ((quad8 + 512 + 64)  & 1023) / 128;		// eight directions

#if 0
	if(quad4 == Q4_BEHIND)
		change_anim( (obj_3d *)def, hit_in_back_1);

	if(quad4 == Q4_FRONT)
		change_anim( (obj_3d *)def, hit_in_front_1);

	if(quad4 == Q4_RIGHT)
		change_anim( (obj_3d *)def, hit_on_left_1);

	if(quad4 == Q4_LEFT)
		change_anim( (obj_3d *)def, hit_on_right_1);
#endif

	// clear attacker's attack mode
	att->attack_mode = 0;
	att->attack_time = 0;

	// clear defender's attack mode
	def->attack_mode = 0;
	def->attack_time = 0;

#if 0
	if(def->action & PA_SET_PICK && att->plyrnum == def->enemy_player) 
	{
	 	//change_anim((obj_3d *)att, flail_1_anim);	// a pick was set
		def->enemy_player = -1;
	}
#endif
	// ignore this if def isn't the ball carrier
	if( def->plyrnum != game_info.ball_handler )
	{
//		change_anim( (obj_3d *)att, r_flaild_anim );
		return;
	}

}

//////////////////////////////////////////////////////////////////////////////
static void att_elbows( bbplyr_data *att, bbplyr_data *def, int dir1, int dir2 )
{
	char	**anim_tbl;
	int		rnd;

	// ignore this if we're on the same team
	if (att->team == def->team)
		return;

	// clear attacker's attack mode
	att->attack_mode = 0;
	att->attack_time = 0;

	if (def->odata.adata[0].animode & MODE_REACTING)
		return;

	// clear defender's attack mode
	def->attack_mode = 0;
	def->attack_time = 0;

	// clear all necessary player flags...
	att->odata.flags &= ~(PF_PLYRFLAGS);
	def->odata.flags &= ~(PF_PLYRFLAGS);

	// 60% of time do flail ?
	anim_tbl = std_hit_tbls[dir1];
	// get number of animations
	for (rnd=0; anim_tbl[rnd]; rnd++);
	// get a random reaction seq.
	rnd = randrng(rnd);

	throw_elbows_speech();

	change_anim( (obj_3d *)def, anim_tbl[rnd]);
//	snd_scall_bank(plyr_bnk_str,P_HARD_HIT2_SND,VOLUME4,127,PRIORITY_LVL4);		// explosion
}



//////////////////////////////////////////////////////////////////////////////
