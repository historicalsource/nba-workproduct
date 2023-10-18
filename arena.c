/*
 *		$Archive: /video/Nba/arena.c $
 *		$Revision: 211 $
 *		$Date: 4/22/99 3:02p $
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
char *ss_arena_c = "$Workfile: arena.c $ $Revision: 211 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif

/*
 *		USER INCLUDES
 */

#include "include/fontid.h"
#include "include/ani3D.h"
#include "include/player.h"
#include "include/externs.h"
#include "include/game.h"
#include "include/id.h"


#include "models/railing.h"
#include "models/stadium.h"
//#include "models/Goalshi4.h"
#include "models/Goallow.h"
#include "models/goalref.h"
#include "models/goalshad.h"
//#include "models/oncrt3.h"
//#include "models/oncrtref.h"
#include "models/oncrt4.h"
#include "models/onctref4.h"
#include "models/crowdhi.h"
#include "models/backcrwd.h"
#include "models/jumbo.h"
//--- COURTS ----

#include "include/courts.h"
#include "models/parkgoal.h"
#include "models/prkgshad.h"
#include "models/specular.h"
#include "models/beachcrt.h"
#include "models/sea.h"
#include "models/trees.h"
#include "models/b_stand1.h"
#include "models/b_stand2.h"
#include "models/b_rim1.h"
#include "models/b_rim2.h"
#include "models/b_bkwod1.h"
#include "models/b_bkwod2.h"
#include "models/b_bkbmb1.h"
#include "models/b_bkbmb2.h"
#include "models/b_shadw1.h"
#include "models/b_shadw2.h"
#include "models/b_sky.h"


#define ARENA_BUFFER_SIZE		200


#if defined(VEGAS)
Texture_node_t *arena_decal_buffer[ARENA_BUFFER_SIZE];
#else
GrMipMapId_t arena_decal_buffer[ARENA_BUFFER_SIZE];
#endif
int arena_ratio_buffer[ARENA_BUFFER_SIZE];
int arena_texture_buffer_index;
int which_court = COURT_INDOOR;
static int geer_goals,geer_rims;

int reset_limb_mode = TRUE;

#ifdef JONHEY										    
#include "bbcrowd.h"
void draw_jh_object(jonhey_t *obj);

void init_crowd_bbox (crowd_bbox_t *crowd, LIMB **limbs);
void convert_2_jonhey (jonhey_t *jh_obj, LIMB *model, float *matrix, Texture_node_t **decals);
void turn_off_bbox (void);
void turn_on_bbox (void);
int global_dog;
void recalculate_bbox(LIMB *limb);
static void roty1616( int t, float *m );

#define	CLIP_OBJECT  0x0
#define	TRIVIAL_REJECTED 0x1
#define TRIVIAL_ACCEPTED 0x2
#endif

void	crowd_camflash();

//___________________________

#define STADIUM_SCALE 17.0f
#define RIM_ROTATE_X	(119.7f * 1.1f)	//119.6851f
#define RIM_ROTATE_Y	RIM_Y

#ifndef RELEASE_VER
 #ifndef TEST_VER
//	#define NOSHOW_COURT
//	#define NOSHOW_GOALS
//	#define NOSHOW_STADIUM
//	#define NOSHOW_JUMBOTRON
//	#define NOSHOW_CROWD
//	#define NOSHOW_CROWD_ANIM
//	#define NOSHOW_SCORETABLE
//	#define NOSCROLL_ADVERTISEMENTS
 #endif //TEST_VER
#endif //RELEASE_VER

// FUNCTION PROTO'S
void
	scroll_advert_1(void), scroll_advert_2(void),
	scroll_advert_3(void), scroll_advert_4(void),
	scroll_advert_5(void), scroll_advert_6(void),
	scroll_advert_7(void)
	;

void draw_goals( void * );

// These bits must coorespond to fields in the structure below
#define	LDI_MCF_DMODE	1
#define	LDI_MCF_DFUNC	2
#define	LDI_MCF_DMASK	4
#define	LDI_MCF_DBIAS	8
#define	LDI_MCF_CFUNC	16
#define	LDI_MCF_ASRC	32
#define	LDI_MCF_TFUNC	64
#define	LDI_MCF_CMODE	128
#define	LDI_MCF_RGBS	256
#define	LDI_MCF_RGBD	512
#define	LDI_MCF_AS		1024
#define	LDI_MCF_AD		2048
#define	LDI_MCF_CKMODE	4096
#define	LDI_MCF_CVAL	8192
#define	LDI_MCF_AFUNC	16384
#define	LDI_MCF_AVAL	32768

#define	LDI_MCF_ALL	(LDI_MCF_DMODE|LDI_MCF_DFUNC|LDI_MCF_DMASK|LDI_MCF_DBIAS| \
							LDI_MCF_CFUNC|LDI_MCF_ASRC|LDI_MCF_TFUNC|LDI_MCF_CMODE| \
							LDI_MCF_RGBS|LDI_MCF_RGBD|LDI_MCF_AS|LDI_MCF_AD|LDI_MCF_CKMODE| \
							LDI_MCF_CVAL|LDI_MCF_AFUNC|LDI_MCF_AVAL)

typedef struct limb_draw_info
{
	LIMB	*limb;					// Pointer to the LIMB to draw
	int	mode_change_flags;	// Mode change flags
	int	depth_mode;				// Depth mode
	int	depth_func;				// Depth buffer function
	int	depth_mask;				// Depth mask
	int	depth_bias;				// Depth bias
	int	cc_function;			// Color Combiner function
	int	alpha_source;			// Alpha source
	int	tc_function;			// Texture combiner function
	int	clamp_mode;				// S and T clamp modes
	int	rgb_src;					// Source RGB Blend function
	int	rgb_dst;					// Destination RGB Blend function
	int	a_src;					// Source Alpha Blend function
	int	a_dst;					// Destination Alpha Blend function
	int	chroma_mode;			// Chroma key mode
	int	chroma_key_value;		// Chroma key value
	int	alpha_test_function;	// Alpha test mode
	int	alpha_test_value;		// Alpha test value
	int	cull_mode;				// Culling mode
} limb_draw_info_t;


//
// local data
//

static int
	*court_ratios, *specular_ratios, *Goals_Ratios, *jumbo_ratios,
	*oncrt_ratios, *adverts_ratios, *crowda_ratios, *crowdb_ratios, *crowdc_ratios, *crowd_ratios,
	*stadium_ratios, *goalref_ratios, *oncrtref_ratios, *goalshad_ratios, *railing_ratios,
	*parkflr_ratios, *parkgoal_ratios, *parkonct_ratios, *parkenv_ratios, *parknet_ratios,
	*prkgshad_ratios, *beachcrt_ratios, *beachsea_ratios, *beachtrees_ratios, *water_ratios,
	*bamboo_ratios, *backcrwd_ratios, *bchshad_ratios, *beachsky_ratios
	;
#if defined(VEGAS)
static Texture_node_t
	**court_decals, **specular_decals, **Goals_Decals, **jumbo_decals,
	**oncrt_decals, **adverts_decals, **crowda_decals, **crowdb_decals, **crowdc_decals, **crowd_decals,
	**stadium_decals,	**goalref_decals, **oncrtref_decals, **goalshad_decals,	**railing_decals,
	**parkflr_decals, **parkgoal_decals, **parkonct_decals, **parkenv_decals, **parknet_decals,
	**prkgshad_decals, **beachcrt_decals, **beachsea_decals, **beachtrees_decals, **water_decals,
	**bamboo_decals, **backcrwd_decals,**bchshad_decals, **beachsky_decals
	;
#else
static GrMipMapId_t
	*court_decals, *specular_decals, *Goals_Decals, *jumbo_decals,
	*oncrt_decals, *adverts_decals, *crowda_decals, *crowdb_decals, *crowdc_decals, *crowd_decals,
	*stadium_decals, *goalref_decals, *oncrtref_decals, *goalshad_decals, *railing_decals
	;
#endif

//컴컴컴컴컴컴컴�
BBDATA	bbdata[2];

#ifdef SHOW_TEST_HEAD
	static int 	*head_ratios;
#if defined(VEGAS)
	static Texture_node_t	**head_decals;
#else
	static GrMipMapId_t *head_decals;
#endif
	extern LIMB limb_kbrya;
	extern char *kbrya_textures[];
#endif

void draw_court( void * );
static void draw_gilmore_court( void * );
static void draw_gilmore_goals( void * );
static void gilmore_arena( void );
static void draw_geer_court( void * );
static void draw_geer_goals( void * );
static void geer_arena( void );

void	mat_setrotz_prec (int ang, float *m_p);

char *crowda_textures[] =
{
	"crda0001.wms",	"crda0002.wms",	"crda0003.wms",	"crda0004.wms",
	"crda0005.wms",	"crda0006.wms",	"crda0007.wms",	"crda0008.wms",
	"crda0009.wms",	"crda0010.wms",	"crda0011.wms",	"crda0012.wms",
	"crda0013.wms",	"crda0014.wms",	"crda0015.wms",	"crda0016.wms",	NULL
};

char *crowdb_textures[] =
{
	"crdb0001.wms",	"crdb0002.wms",	"crdb0003.wms",	"crdb0004.wms",
	"crdb0005.wms",	"crdb0006.wms",	"crdb0007.wms",	"crdb0008.wms",
	"crdb0009.wms",	"crdb0010.wms",	"crdb0011.wms",	"crdb0012.wms",
	"crdb0013.wms",	"crdb0014.wms",	"crdb0015.wms",	"crdb0016.wms",	NULL
};

char *crowdc_textures[] =
{
	"crdc0001.wms",	"crdc0002.wms",	"crdc0003.wms",	"crdc0004.wms",
	"crdc0005.wms",	"crdc0006.wms",	"crdc0007.wms",	"crdc0008.wms",
	"crdc0009.wms",	"crdc0010.wms",	"crdc0011.wms",	"crdc0012.wms",
	"crdc0013.wms",	"crdc0014.wms",	"crdc0015.wms",	"crdc0016.wms",	NULL
};

//
// external data
//
extern VIEWCOORD cambot;
extern float hres, vres;
extern int crowd_cheer_time;

extern VERTEX
	*nt_swish_verts[],
	*nt_swshb_verts[],
	*nt_dunk_verts[],
	*nt_hard_verts[],
	*nt_rim_verts[],
	*nt_air_verts[]
	;


#ifdef JONHEY
extern VERTEX
	*nt_swish_bbox[],
	*nt_swshb_bbox[],
	*nt_dunk_bbox[],
	*nt_hard_bbox[],
	*nt_rim_bbox[],
	*nt_air_bbox[]
	;
#endif

VERTEX **net_anims[] =
{
	nt_swish_verts,
	nt_swshb_verts,
	nt_dunk_verts,
	nt_hard_verts,
	nt_rim_verts,
	nt_air_verts
};

// NOTE - This control the draw order for limbs that make up the arena

static limb_draw_info_t goalshad_limb_draw_info[] =
{
	{
	&limb_goalshd1,
	0,
	GR_DEPTHBUFFER_DISABLE,	//WBUFFER,
	GR_CMP_LESS,
	FXFALSE,
	0,
	GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
	GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_ALWAYS,
	0,
	GR_CULL_NEGATIVE
	},
	{
	&limb_goalshd2,
	0,
	GR_DEPTHBUFFER_DISABLE,	//WBUFFER,
	GR_CMP_LESS,
	FXFALSE,
	0,
	GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
	GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_ALWAYS,
	0,
	GR_CULL_NEGATIVE
	},
};

static limb_draw_info_t oncrtref_limb_draw_info[] =
{
	{
	NULL,
	0,
	GR_DEPTHBUFFER_DISABLE,	//WBUFFER,
	GR_CMP_LESS,
	FXFALSE,
	0,
	GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
	GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE,	//_MINUS_SRC_ALPHA,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_ALWAYS,
	0,
	GR_CULL_NEGATIVE
	},
#if 0
	{
	&limb_advertref1,
	0,
	GR_DEPTHBUFFER_DISABLE,	//WBUFFER,
	GR_CMP_LESS,
	FXFALSE,
	0,
	GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
	GR_ALPHASOURCE_CC_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE,	//_MINUS_SRC_ALPHA,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_ALWAYS,
	0,
	GR_CULL_NEGATIVE
	},
	{
	&limb_advertref2,
	0,
	GR_DEPTHBUFFER_DISABLE,	//WBUFFER,
	GR_CMP_LESS,
	FXFALSE,
	0,
	GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
	GR_ALPHASOURCE_CC_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE,	//_MINUS_SRC_ALPHA,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_ALWAYS,
	0,
	GR_CULL_NEGATIVE
	},
	{
	&limb_advertref3,
	0,
	GR_DEPTHBUFFER_DISABLE,	//WBUFFER,
	GR_CMP_LESS,
	FXFALSE,
	0,
	GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
	GR_ALPHASOURCE_CC_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE,	//_MINUS_SRC_ALPHA,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_ALWAYS,
	0,
	GR_CULL_NEGATIVE
	}
#endif	
};

#ifdef SHOW_TEST_HEAD
static limb_draw_info_t head_limb_draw_info[] = {
{
&limb_kbrya,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_LESS,
FXTRUE,
0,
GR_COLORCOMBINE_DECAL_TEXTURE,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_ALWAYS,
0,
GR_CULL_NEGATIVE
}
};
#endif

static limb_draw_info_t jumbo_limb_draw_info[] =
{ 
	{
	&limb_jumbotron,
	0,
	GR_DEPTHBUFFER_WBUFFER,
	GR_CMP_LESS,
	FXTRUE,
	0,
	GR_COLORCOMBINE_DECAL_TEXTURE,
	GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_ONE,
	GR_BLEND_ZERO,
	GR_BLEND_ONE,
	GR_BLEND_ZERO,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_ALWAYS,
	0,
	GR_CULL_NEGATIVE
	}
};

static limb_draw_info_t adverts_limb_draw_info[] =
{
	{
	NULL,
	0,
	GR_DEPTHBUFFER_WBUFFER,	//DISABLE,
	GR_CMP_LESS,
	FXTRUE,	//FXFALSE,
	0,
	GR_COLORCOMBINE_DECAL_TEXTURE,
	GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_ONE,
	GR_BLEND_ZERO,
	GR_BLEND_ONE,
	GR_BLEND_ZERO,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_ALWAYS,
	0,
	GR_CULL_NEGATIVE
	},
};

static limb_draw_info_t scoretable_limb_draw_info[] =
{
	{
	//&limb_scoretable,
	&limb_oncourt,
	0,
	//GR_DEPTHBUFFER_DISABLE,
	//GR_CMP_LESS,
	//FXFALSE,
	GR_DEPTHBUFFER_WBUFFER,
	GR_CMP_LESS,
	FXTRUE,
	0,
	GR_COLORCOMBINE_DECAL_TEXTURE,
	GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_ONE,
	GR_BLEND_ZERO,
	GR_BLEND_ONE,
	GR_BLEND_ZERO,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_ALWAYS,
	0,
	GR_CULL_NEGATIVE
	}
};

static limb_draw_info_t Harness_reflection_limb_draw_info[] =
{
	{
	&limb_goalrefl1,
	0,
	GR_DEPTHBUFFER_DISABLE,
	GR_CMP_LESS,
	FXFALSE,
	0,
	GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
	GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_SRC_ALPHA,						//GR_BLEND_ONE,
	GR_BLEND_ONE,		//_MINUS_SRC_ALPHA,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_ALWAYS,
	0,
	GR_CULL_NEGATIVE
	},
	{
	&limb_goalrefl2,
	0,
	GR_DEPTHBUFFER_DISABLE,//	GR_DEPTHBUFFER_WBUFFER,
	GR_CMP_LESS,
	FXFALSE,
	0,
	GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
	GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE,	//_MINUS_SRC_ALPHA,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_ALWAYS,
	0,
	GR_CULL_NEGATIVE
	},
};

static limb_draw_info_t court_limb_draw_info[] =
{
#if 0
	{
	&limb_Specular,
	0,
	GR_DEPTHBUFFER_WBUFFER,
	GR_CMP_LESS,
	FXTRUE,
	0,
	GR_COLORCOMBINE_DECAL_TEXTURE,
	GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_ONE,
	GR_BLEND_ZERO,
	GR_BLEND_ONE,
	GR_BLEND_ZERO,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_ALWAYS,
	0,
	GR_CULL_NEGATIVE
	}
#else
	{
	&limb_Specular,
	0,
	GR_DEPTHBUFFER_WBUFFER,//WBUFFER,
	GR_CMP_LESS,
	FXTRUE,
	0,
	GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
	//GR_COLORCOMBINE_DECAL_TEXTURE,
	GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_GREATER,	//ALWAYS,
	2,	//129 0,
	GR_CULL_NEGATIVE
	}
#endif
};

// This is closest to camera in ENDLINE_CAM, or the left Goal in the SIDELINE_CAM.
static limb_draw_info_t goalbase_limb_draw_info[] = {
	{
	&limb_goal_base,
	0,
	GR_DEPTHBUFFER_WBUFFER,
	GR_CMP_LESS,
	FXTRUE,
	0,
	GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,	//GR_COLORCOMBINE_DECAL_TEXTURE,
	GR_ALPHASOURCE_CC_ALPHA,				//GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_SRC_ALPHA,						//GR_BLEND_ONE,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,			//GR_BLEND_ZERO,
	GR_BLEND_ONE,
	GR_BLEND_ZERO,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_ALWAYS,
	0,
	GR_CULL_NEGATIVE
	},
};

static limb_draw_info_t goalstnd_limb_draw_info[] = {
	{
	&limb_GoalStand,
	0,
	GR_DEPTHBUFFER_WBUFFER,
	GR_CMP_LESS,
	FXTRUE,
	0,
	GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,	//GR_COLORCOMBINE_DECAL_TEXTURE,
	GR_ALPHASOURCE_CC_ALPHA,				//GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_SRC_ALPHA,						//GR_BLEND_ONE,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,			//GR_BLEND_ZERO,
	GR_BLEND_ONE,
	GR_BLEND_ZERO,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_ALWAYS,
	0,
	GR_CULL_NEGATIVE
	},
};

static limb_draw_info_t backboard_limb_draw_info[] = {
	{
	&limb_BackBoard,
	0,
	GR_DEPTHBUFFER_WBUFFER,
	GR_CMP_LESS,
	FXTRUE,
	0,
#if 0
	GR_COLORCOMBINE_DECAL_TEXTURE,
	GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_ONE,
	GR_BLEND_ZERO,
#endif
	GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,	//GR_COLORCOMBINE_DECAL_TEXTURE,
	GR_ALPHASOURCE_CC_ALPHA,				//GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_SRC_ALPHA,						//GR_BLEND_ONE,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,			//GR_BLEND_ZERO,
	GR_BLEND_ONE,
	GR_BLEND_ZERO,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_ALWAYS,
	0,
	GR_CULL_NEGATIVE
	},
	{
	&limb_decals,
	0,
	GR_DEPTHBUFFER_WBUFFER,//DISABLE,
	GR_CMP_LESS,
	FXFALSE,
	0,
	GR_COLORCOMBINE_DECAL_TEXTURE,
	GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_ONE,
	GR_BLEND_ZERO,
	GR_BLEND_ONE,
	GR_BLEND_ZERO,
#if 0
	GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
	GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_SRC_ALPHA,						//GR_BLEND_ONE,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,			//GR_BLEND_ZERO,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
#endif
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_ALWAYS,
	0,
	GR_CULL_NEGATIVE
	},
};


static limb_draw_info_t glass_limb_draw_info[] = {
	{
	&limb_glass,
	0,
	GR_DEPTHBUFFER_WBUFFER,
	GR_CMP_LESS,
	FXTRUE,
	0,
	GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
	GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_SRC_ALPHA,						//GR_BLEND_ONE,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,			//GR_BLEND_ZERO,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_GREATER,	//ALWAYS,
	0,	//129 0,
	GR_CULL_NEGATIVE
	},
};


static limb_draw_info_t Rim_limb_draw_info[] =
{
	{
	&limb_Rim,
	0,
	GR_DEPTHBUFFER_WBUFFER,
	GR_CMP_LESS,
	FXTRUE,
	0,
	GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,	//GR_COLORCOMBINE_DECAL_TEXTURE,
	GR_ALPHASOURCE_CC_ALPHA,				//GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_SRC_ALPHA,						//GR_BLEND_ONE,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,			//GR_BLEND_ZERO,
	GR_BLEND_ONE,
	GR_BLEND_ZERO,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_ALWAYS,
	0,
	GR_CULL_NEGATIVE
	},
};

static limb_draw_info_t Net_limb_draw_info[] =
{
	{
	&limb_Net,
	0,
	GR_DEPTHBUFFER_WBUFFER,
	GR_CMP_LESS,
	FXTRUE,
	0,
	GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
	GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_GREATER,	//ALWAYS,
	64,	//129 0,
	GR_CULL_NEGATIVE
	}
};

// This Goal is farthest away from camera in ENDLINE_CAM, or the right Goal in the SIDELINE_CAM.
static limb_draw_info_t goalbase2_limb_draw_info[] = {
	{
	&limb_goal_base2,
	0,
	GR_DEPTHBUFFER_WBUFFER,
	GR_CMP_LESS,
	FXTRUE,
	0,
	GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,	//GR_COLORCOMBINE_DECAL_TEXTURE,
	GR_ALPHASOURCE_CC_ALPHA,				//GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_SRC_ALPHA,						//GR_BLEND_ONE,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,			//GR_BLEND_ZERO,
	GR_BLEND_ONE,
	GR_BLEND_ZERO,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_ALWAYS,
	0,
	GR_CULL_NEGATIVE
	},
};
static limb_draw_info_t goalstnd2_limb_draw_info[] = {
	{
	&limb_GoalStand2,
	0,
	GR_DEPTHBUFFER_WBUFFER,
	GR_CMP_LESS,
	FXTRUE,
	0,
	GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,	//GR_COLORCOMBINE_DECAL_TEXTURE,
	GR_ALPHASOURCE_CC_ALPHA,				//GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_SRC_ALPHA,						//GR_BLEND_ONE,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,			//GR_BLEND_ZERO,
	GR_BLEND_ONE,
	GR_BLEND_ZERO,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_ALWAYS,
	0,
	GR_CULL_NEGATIVE
	},
};

static limb_draw_info_t backboard2_limb_draw_info[] = {
	{
	&limb_BackBoard2,
	0,
	GR_DEPTHBUFFER_WBUFFER,
	GR_CMP_LESS,
	FXTRUE,
	0,
//#if 0
	GR_COLORCOMBINE_DECAL_TEXTURE,
	GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_ONE,
	GR_BLEND_ZERO,
//#endif
#if 0
	GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,	//GR_COLORCOMBINE_DECAL_TEXTURE,
	GR_ALPHASOURCE_CC_ALPHA,				//GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_SRC_ALPHA,						//GR_BLEND_ONE,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,			//GR_BLEND_ZERO,
#endif
	GR_BLEND_ONE,
	GR_BLEND_ZERO,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_ALWAYS,
	0,
	GR_CULL_NEGATIVE
	},
	{
	&limb_decals2,
	0,
	GR_DEPTHBUFFER_WBUFFER,//DISABLE,
	GR_CMP_LESS,
	FXTRUE,
	//FXFALSE,
	0,
	GR_COLORCOMBINE_DECAL_TEXTURE,
	GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_ONE,
	GR_BLEND_ZERO,
	GR_BLEND_ONE,
	GR_BLEND_ZERO,
#if 0
	GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
	GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_SRC_ALPHA,						//GR_BLEND_ONE,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,			//GR_BLEND_ZERO,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
#endif
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_ALWAYS,
	0,
	GR_CULL_NEGATIVE
	},
};




static limb_draw_info_t glass2_limb_draw_info[] = {
	{
	&limb_glass2,
	0,
	GR_DEPTHBUFFER_WBUFFER,
	GR_CMP_LESS,
	FXTRUE,
	0,
	GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
	GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_SRC_ALPHA,						//GR_BLEND_ONE,
	GR_BLEND_ONE,	//_MINUS_SRC_ALPHA,			//GR_BLEND_ZERO,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
	GR_CHROMAKEY_DISABLE,
	0,

	GR_CMP_GREATER,	//ALWAYS,
	0,	//129 0,
	GR_CULL_NEGATIVE
	},
};


static limb_draw_info_t Rim_2_limb_draw_info[] =
{
	{
	&limb_Rim2,
	0,
	GR_DEPTHBUFFER_WBUFFER,
	GR_CMP_LESS,
	FXTRUE,
	0,
#if 0
	GR_COLORCOMBINE_DECAL_TEXTURE,
	GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_ONE,
	GR_BLEND_ZERO,
#endif
	GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,	//GR_COLORCOMBINE_DECAL_TEXTURE,
	GR_ALPHASOURCE_CC_ALPHA,				//GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_SRC_ALPHA,						//GR_BLEND_ONE,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,			//GR_BLEND_ZERO,
	GR_BLEND_ONE,
	GR_BLEND_ZERO,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_ALWAYS,
	0,
	GR_CULL_NEGATIVE
	},
};

static limb_draw_info_t Net_2_limb_draw_info[] =
{
	{
	&limb_Net2,
	0,
	GR_DEPTHBUFFER_WBUFFER,
	GR_CMP_LESS,
	FXTRUE,
	0,
	GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
	GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_GREATER,	//ALWAYS,
	64,	//129 0,
	GR_CULL_NEGATIVE
	}
};

static limb_draw_info_t specular_limb_draw_info[] =
{
	{
	&limb_Specular,
	0,
	GR_DEPTHBUFFER_DISABLE,//GR_DEPTHBUFFER_WBUFFER,
	GR_CMP_LESS,
	FXFALSE,
	0,
	GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB,//ALPHA,
	GR_ALPHASOURCE_CC_ALPHA,	//	GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE,	//_MINUS_SRC_ALPHA,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_GREATER,	//ALWAYS,
	0,	//129 0,
	GR_CULL_NEGATIVE
	},
};

static limb_draw_info_t stadium_limb_draw_info[] =
{
	{
	&limb_stadium_section_1,
	0,
	GR_DEPTHBUFFER_WBUFFER,
	GR_CMP_LESS,
	FXTRUE,
	0,
	GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
	GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_GREATER,
	2,	//129 0,
	GR_CULL_NEGATIVE
	},
};

static limb_draw_info_t crowd_limb_draw_info[] =
{
	{
	NULL,//&limb_crowd_section_1,
	0,
	GR_DEPTHBUFFER_WBUFFER,
	GR_CMP_LESS,
	FXTRUE,
	0,
	GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB,
	GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_CLAMP,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_GREATER,	//ALWAYS,
	2,	//129 0,
	GR_CULL_NEGATIVE
	},
};

static limb_draw_info_t railing_limb_draw_info[] =
{
	{
	NULL,
	0,
	GR_DEPTHBUFFER_WBUFFER,
	GR_CMP_LESS,
	FXTRUE,
	0,
	GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB,//ALPHA,
	GR_ALPHASOURCE_TEXTURE_ALPHA,
	GR_TEXTURECOMBINE_DECAL,
	GR_TEXTURECLAMP_WRAP,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
	GR_BLEND_SRC_ALPHA,
	GR_BLEND_ONE_MINUS_SRC_ALPHA,
	GR_CHROMAKEY_DISABLE,
	0,
	GR_CMP_GREATER,	//ALWAYS,
	2,	//129 0,
	GR_CULL_NEGATIVE
	},
};

void init_limb_draw_info(limb_draw_info_t *di, int num)
{
	int	i;
	int	j;
	int	*t1;
	int	*t2;

	di->mode_change_flags = LDI_MCF_ALL;
	di++;
	for(i = 1; i < num; i++)
	{
		t1 = &(di-1)->depth_mode;
		t2 = &di->depth_mode;
		di->mode_change_flags = 0;
		for(j = 0; j < (sizeof(limb_draw_info_t)/sizeof(int)) - 2; j++)
		{
			if(*t1 != *t2)
			{
				di->mode_change_flags |= (1<<j);
			}
			++t1;
			++t2;
		}
		++di;
	}
}

void set_limb_mode(limb_draw_info_t *di)
{

	if(reset_limb_mode == FALSE)
		return;


	// Depth Buffer Mode
	if(di->mode_change_flags & LDI_MCF_DMODE)
	{
		grDepthBufferMode(di->depth_mode);
	}

	// Depth Buffer Function
	if(di->mode_change_flags & LDI_MCF_DFUNC)
	{
		grDepthBufferFunction(di->depth_func);
	}

	// Depth Buffer Mask
	if(di->mode_change_flags & LDI_MCF_DMASK)
	{
		grDepthMask(di->depth_mask);
	}

	// Depth Bias
	if(di->mode_change_flags & LDI_MCF_DBIAS)
	{
		grDepthBiasLevel(di->depth_bias);
	}

	// Color Combiner Function
	if(di->mode_change_flags & LDI_MCF_CFUNC)
	{
		guColorCombineFunction(di->cc_function);
	}

	// Alpha Source
	if(di->mode_change_flags & LDI_MCF_ASRC)
	{
		guAlphaSource(di->alpha_source);
	}

	// Texture Combiner Function
	if(di->mode_change_flags & LDI_MCF_TFUNC)
	{
		grTexCombineFunction(0, di->tc_function);
	}

	// Clamp Mode
	if(di->mode_change_flags & LDI_MCF_CMODE)
	{
		grTexClampMode(0, di->clamp_mode, di->clamp_mode);
	}

	// Source RGB Blend Function
	// Destination RGB Blend Function
	// Source Alpha Blend Function
	// Destination Alpha Blend Function
	if(di->mode_change_flags & (LDI_MCF_RGBS|LDI_MCF_RGBD|LDI_MCF_AS|LDI_MCF_AD))
	{
		grAlphaBlendFunction(di->rgb_src, di->rgb_dst, di->a_src, di->a_dst);
	}

	// Chromakey Mode
	if(di->mode_change_flags & LDI_MCF_CKMODE)
	{
		grChromakeyMode(di->chroma_mode);
	}

	// Chromakey Value
	if(di->mode_change_flags & LDI_MCF_CVAL)
	{
		grChromakeyValue(di->chroma_key_value);
	}

	// Alpha Test Function
	if(di->mode_change_flags & LDI_MCF_AFUNC)
	{
		grAlphaTestFunction(di->alpha_test_function);
	}

	// Alpha test value
	if(di->mode_change_flags & LDI_MCF_AVAL)
	{
		grAlphaTestReferenceValue(di->alpha_test_value);
	}
}

int tex_count( char **tex )
{
	int i = 0;
	
	while(tex[i]) ++i;
	return i;
}

#if defined(VEGAS)
static void get_buffer(Texture_node_t ***decal_buffer, int **ratio_buffer, int amount)
#else
static void get_buffer(GrMipMapId_t **decal_buffer, int **ratio_buffer, int amount)
#endif
{
	if (arena_texture_buffer_index + amount >= ARENA_BUFFER_SIZE) {
#ifdef DEBUG
		fprintf(stderr, "arena_buffer FULL!, increase ARENA_BUFFER_SIZE in arena.c\n");
#endif
		lockup();
	}
	
	*decal_buffer = &arena_decal_buffer[arena_texture_buffer_index];
	*ratio_buffer = &arena_ratio_buffer[arena_texture_buffer_index];
	arena_texture_buffer_index += amount;
}	/* get_buffer */

/****************************************************************************/
// Arena_proc - main Arena process.
void Arena_proc( int *parg )
{
	int	i, west = NO,
	eastconf[15] = {
		TEAM_BOS, TEAM_MIA, TEAM_NJN, TEAM_NYK, TEAM_ORL, TEAM_PHI, TEAM_WAS,
		TEAM_ATL, TEAM_CHA, TEAM_CHI, TEAM_CLE, TEAM_DET, TEAM_IND, TEAM_MIL, TEAM_TOR
		};
	char step[12],cush[12],gref[12],tabl[12],topc[12],banr[12],conf[12],advt[12];
	int team=0;
	int alt_arena;
	const char *court_prefix;
	const char *banner_prefix;
	char **court_texture;

	/* Set the background color */
	set_bgnd_color(0xFF000000);

#ifdef JASON	
	which_court = COURT_GEER;
#endif

	switch (which_court)
	{
		case COURT_INDOOR:
			court_prefix = teaminfo[game_info.team[0]].szprfx;
			banner_prefix = court_prefix;
			court_texture = teaminfo[game_info.team[0]].court_tex;
			team = 0;
			alt_arena = 0;
			break;
		case COURT_INDOOR2:
			court_prefix = teaminfo[game_info.team[1]].szprfx;
			banner_prefix = court_prefix;
			court_texture = teaminfo[game_info.team[1]].court_tex;
			team = 1;
			alt_arena = 0;
			break;
		case COURT_MIDWAY:
			court_prefix = "MID";
			banner_prefix = teaminfo[game_info.team[0]].szprfx;
			court_texture = (char **)&midcourt_textures;
			team = 0;
			alt_arena = 1;
			break;
		case COURT_NBC:
			court_prefix = "NBC";
			banner_prefix = teaminfo[game_info.team[0]].szprfx;
			court_texture = (char **)&nbccourt_textures;
			team = 0;
			alt_arena = 2;
			break;
		case COURT_GILMORE:
			gilmore_arena();
			return;
		case COURT_GEER:
			geer_arena();
			return;
	}

#ifdef JONHEY
//	init_crowd_bbox (crowdbbox, crowdhi_limbs);
#endif

	// Initialize the drawing state info for the basketball court limbs
	init_limb_draw_info(stadium_limb_draw_info, sizeof(stadium_limb_draw_info)/sizeof(limb_draw_info_t));

	init_limb_draw_info(specular_limb_draw_info, sizeof(specular_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(court_limb_draw_info, sizeof(court_limb_draw_info)/sizeof(limb_draw_info_t));

	init_limb_draw_info(goalshad_limb_draw_info, sizeof(goalshad_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(oncrtref_limb_draw_info, sizeof(oncrtref_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(scoretable_limb_draw_info, sizeof(scoretable_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(adverts_limb_draw_info, sizeof(adverts_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(jumbo_limb_draw_info, sizeof(jumbo_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(crowd_limb_draw_info, sizeof(crowd_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(railing_limb_draw_info, sizeof(railing_limb_draw_info)/sizeof(limb_draw_info_t));

	init_limb_draw_info(Harness_reflection_limb_draw_info, sizeof(Harness_reflection_limb_draw_info)/sizeof(limb_draw_info_t));

	init_limb_draw_info(goalbase_limb_draw_info, sizeof(goalbase_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(goalstnd_limb_draw_info, sizeof(goalstnd_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(backboard_limb_draw_info, sizeof(backboard_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(glass_limb_draw_info, sizeof(glass_limb_draw_info)/sizeof(limb_draw_info_t));

	init_limb_draw_info(Rim_limb_draw_info, sizeof(Rim_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(Net_limb_draw_info, sizeof(Net_limb_draw_info)/sizeof(limb_draw_info_t));

	init_limb_draw_info(goalbase2_limb_draw_info, sizeof(goalbase2_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(goalstnd2_limb_draw_info, sizeof(goalstnd2_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(backboard2_limb_draw_info, sizeof(backboard2_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(glass2_limb_draw_info, sizeof(glass2_limb_draw_info)/sizeof(limb_draw_info_t));

	init_limb_draw_info(Rim_2_limb_draw_info, sizeof(Rim_2_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(Net_2_limb_draw_info, sizeof(Net_2_limb_draw_info)/sizeof(limb_draw_info_t));

#ifdef SHOW_TEST_HEAD
	init_limb_draw_info(head_limb_draw_info, sizeof(head_limb_draw_info)/sizeof(limb_draw_info_t));
#endif

//----------------------------------------------------------------------------
	arena_texture_buffer_index = 0;

	get_buffer(&goalshad_decals, &goalshad_ratios, tex_count(goalshad_textures));
	get_buffer(&oncrtref_decals, &oncrtref_ratios, tex_count(onctref4_textures));
	get_buffer(&stadium_decals, &stadium_ratios, tex_count(stadium_textures));
	get_buffer(&court_decals, &court_ratios, tex_count(court_texture));
	get_buffer(&Goals_Decals, &Goals_Ratios, tex_count(goallow_textures));
	get_buffer(&goalref_decals, &goalref_ratios, tex_count(goalref_textures));
	get_buffer(&specular_decals, &specular_ratios, tex_count(specular_textures));
	get_buffer(&oncrt_decals, &oncrt_ratios, tex_count(oncrt4_textures));
	get_buffer(&crowd_decals, &crowd_ratios, tex_count(crowdhi_textures));
	get_buffer(&crowda_decals, &crowda_ratios, tex_count(crowda_textures));
	get_buffer(&crowdb_decals, &crowdb_ratios, tex_count(crowdb_textures));
	get_buffer(&crowdc_decals, &crowdc_ratios, tex_count(crowdc_textures));
	get_buffer(&railing_decals, &railing_ratios, tex_count(railing_textures));
	get_buffer(&jumbo_decals, &jumbo_ratios, tex_count(jumbo_textures));
	get_buffer(&adverts_decals, &adverts_ratios, 1);
	get_buffer(&backcrwd_decals, &backcrwd_ratios, 1);

	west = YES;
	for(i = 0; i < 15; i++)
		if(game_info.team[team] == eastconf[i])
			west = NO;
	sprintf(conf, "%sconf.wms", west == YES ? "west" : "east");
	sprintf(advt, "%sadvt.wms", west == YES ? "west" : "east");
	sprintf(cush, "%scush.wms", court_prefix);
	sprintf(gref, "%sgref.wms", court_prefix);
	sprintf(tabl, "%stabl.wms", court_prefix);
	sprintf(topc, "%stopc.wms", court_prefix);
	sprintf(banr, "%sbannr.wms",banner_prefix);

	if(randrng(100) < 50)
		sprintf(step, "step%s.wms", "purp");
	else if(randrng(100) > 30)
		sprintf(step, "step%s.wms", "blue");
	else
		sprintf(step, "step%s.wms", "red");

	onctref4_textures[0] = banr;
	onctref4_textures[1] = tabl;
	onctref4_textures[3] = advt;
	onctref4_textures[11] = conf;
	goallow_textures[1] = cush;
	oncrt4_textures[15] = conf;
	oncrt4_textures[0] = banr;
	oncrt4_textures[1] = tabl;
	oncrt4_textures[2] = topc;
	oncrt4_textures[4] = advt;
	goalref_textures[0] = gref;

	stadium_textures[14] = step;

	load_textures(goalshad_textures, goalshad_ratios, goalshad_decals, tex_count(goalshad_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);

	load_textures(onctref4_textures, oncrtref_ratios, oncrtref_decals, tex_count(onctref4_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);

	load_textures(stadium_textures, stadium_ratios, stadium_decals, tex_count(stadium_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);

	load_textures(specular_textures, specular_ratios, specular_decals, tex_count(specular_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);

#ifndef NOSHOW_COURT
	load_textures(court_texture, court_ratios, court_decals, tex_count(court_texture),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
#endif

	load_textures(oncrt4_textures, oncrt_ratios, oncrt_decals, tex_count(oncrt4_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);

#ifndef NOSHOW_JUMBOTRON
	load_textures(jumbo_textures, jumbo_ratios, jumbo_decals, tex_count(jumbo_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
#endif

#ifndef NOSHOW_CROWD
	load_textures(crowdhi_textures, crowd_ratios, crowd_decals, tex_count(crowdhi_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
	load_textures(backcrwd_textures, backcrwd_ratios, backcrwd_decals, tex_count(backcrwd_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
 #ifndef NOSHOW_CROWD_ANIM
	load_textures(crowda_textures, crowda_ratios, crowda_decals, tex_count(crowda_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
	load_textures(crowdb_textures, crowdb_ratios, crowdb_decals, tex_count(crowdb_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
	load_textures(crowdc_textures, crowdc_ratios, crowdc_decals, tex_count(crowdc_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
 #endif
	load_textures(railing_textures, railing_ratios, railing_decals, tex_count(railing_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
#endif

	load_textures(goallow_textures, Goals_Ratios, Goals_Decals, tex_count(goallow_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);

	load_textures(goalref_textures, goalref_ratios, goalref_decals, tex_count(goalref_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);

#if 0
	load_textures(adverts_textures, adverts_ratios, adverts_decals, 1,
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
#endif

#ifndef NOSHOW_COURT
// make sure all court textures are CLAMPED
	for(i = 0; i < tex_count(court_texture); i++)
	{
		guTexChangeAttributes( court_decals[i],
												-1, -1,
												-1,
												-1,//GR_MIPMAP_NEAREST,
												-1, -1,
												-1,
												GR_TEXTURECLAMP_CLAMP,
												GR_TEXTURECLAMP_CLAMP,
												-1,	//GR_TEXTUREFILTER_BILINEAR,
												-1	//GR_TEXTUREFILTER_BILINEAR
                    	  );

// Took this out to get rid of moire pattern on court
//	guTexChangeLodBias( court_decals[i], -2.0f);

	if(!alt_arena && (game_info.team[team] == TEAM_CHA)) // Charlotte Hornets
		guTexChangeLodBias( court_decals[i], -0.5f);
	}
#endif
#ifndef NOSHOW_CROWD
//	for(i = 0; i < tex_count(crowdhi_textures); i++)
//	{
//		guTexChangeAttributes( crowd_decals[i],
//												-1, -1,
//												-1,
//												GR_MIPMAP_DISABLE,
//												-1, -1,
//												-1,
//												-1,	//GR_TEXTURECLAMP_CLAMP,
//												-1,	//GR_TEXTURECLAMP_CLAMP,
//												-1,	//GR_TEXTUREFILTER_BILINEAR,
//												-1	//GR_TEXTUREFILTER_BILINEAR
//                    	  );
//	}
	guTexChangeAttributes( railing_decals[0],
											-1, -1,
											-1,
											-1,
											-1, -1,
											-1,
											GR_TEXTURECLAMP_WRAP,
											GR_TEXTURECLAMP_WRAP,
											-1,	//GR_TEXTUREFILTER_BILINEAR,
											-1	//GR_TEXTUREFILTER_BILINEAR
                  	  );
#endif
#ifndef NOSHOW_COURT
// Some courts can't have CLAMPED textures
	if(!alt_arena && (game_info.team[team] == TEAM_MIN))	// Minnesota Timberwolves
		guTexChangeAttributes( court_decals[2],
												-1, -1,
												-1,
												-1,	//GR_MIPMAP_DISABLE,
												-1, -1,
												-1,
												GR_TEXTURECLAMP_WRAP,
												GR_TEXTURECLAMP_WRAP,
												-1,	//GR_TEXTUREFILTER_BILINEAR,
												-1	//GR_TEXTUREFILTER_BILINEAR
                    	  );
	if((alt_arena == 1) || (game_info.team[team] == TEAM_HOU))
		guTexChangeAttributes( court_decals[8],
												-1, -1,
												-1,
												-1,	//GR_MIPMAP_DISABLE,
												-1, -1,
												-1,
												GR_TEXTURECLAMP_WRAP,
												GR_TEXTURECLAMP_WRAP,
												-1,	//GR_TEXTUREFILTER_BILINEAR,
												-1	//GR_TEXTUREFILTER_BILINEAR
                    	  );
#endif
	guTexChangeAttributes( Goals_Decals[1],
											-1, -1,
											-1,
											-1,
											-1, -1,
											-1,
											GR_TEXTURECLAMP_CLAMP,
											GR_TEXTURECLAMP_CLAMP,
											GR_TEXTUREFILTER_BILINEAR,
											GR_TEXTUREFILTER_BILINEAR
                   	  );
#if 0
	guTexChangeAttributes( Goals_Decals[11],
											-1, -1,
											-1,
											-1,
											-1, -1,
											-1,
											GR_TEXTURECLAMP_CLAMP,
											GR_TEXTURECLAMP_CLAMP,
											GR_TEXTUREFILTER_BILINEAR,
											GR_TEXTUREFILTER_BILINEAR
                   	  );
	guTexChangeAttributes( Goals_Decals[0],
                      -1, -1,
                      -1,
                      GR_MIPMAP_DISABLE,
                      -1, -1,
                      -1,
											-1,
                      -1,
											GR_TEXTUREFILTER_BILINEAR,//POINT_SAMPLED,
											GR_TEXTUREFILTER_BILINEAR
                      );
	guTexChangeAttributes( Goals_Decals[1],
                      -1, -1,
                      -1,
                      GR_MIPMAP_DISABLE,
                      -1, -1,
                      -1,
											-1,
                      -1,
											GR_TEXTUREFILTER_POINT_SAMPLED,
											GR_TEXTUREFILTER_BILINEAR
                      );
#endif

	// Create the drawing object to draw the arena
//	create_object("court", OID_FIELD, OF_NONE, DRAWORDER_FIELD, NULL,   draw_court);
//	create_object("goals", OID_GOALS, OF_NONE, DRAWORDER_GOALS, bbdata, draw_goals);

	// Let the process die cause it's not needed anymore
#if 0
	{
	float a,b,c,b2m4ac,root1,root2;

		a = GRAVITY / 2.0f;	//-0.08f / 2.0f;	//GRAVITY / 2.0f;
		b = 1.9f;	//pobj->vy;
		c = 0.0f;	//pobj->y;

		b2m4ac = b * b - 4.0f * a * c;
		root1 = (-1.0f * b + fsqrt( b2m4ac )) / (2.0f * a);
		root2 = (-1.0f * b - fsqrt( b2m4ac )) / (2.0f * a);

		printf("b2m4ac = %f, root1 = %f, root2 = %f\n",b2m4ac, root1, root2);
	}
#endif
}

//-------------------------------------------------------------------------------
// These variables control the amount of backboard shaking and rim rattling done.
//extern float bb_shake_l, bb_shake_r;
//extern int rim_rattle_l, rim_rattle_r, shakecnt; 
static float xlate[12] =
{
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f
};

static float cm[12], cm2[12];

#if defined(VEGAS)
static void draw_world_object(register limb_draw_info_t *di, register int num_limbs, register Texture_node_t **decals, register int *ratios)
#else
static void draw_world_object(register limb_draw_info_t *di, register int num_limbs, register GrMipMapId_t *decals, register int *ratios)
#endif
{
	while(num_limbs--)
	{
		if(di->mode_change_flags)
		{
			set_limb_mode(di);
		}
		do
		{
			if (di->limb == &limb_railing_section_1 ||
				di->limb == &limb_railing_section_5
			/*|| di->limb == &limb_stadium_section_5*/)
			{
				//FIX!!!???
				// Though effective, this chk is just a hack that could
				//  produce undesirable results with a trickier camera or
				//  certainly with any other mode besides CAM_HTIME
				if ((cambot.z > 0 && di->limb->pvtx->z > 0) ||
					(cambot.z < 0 && di->limb->pvtx->z < 0))
				{
					break;
				}
			}
			if(((cambot.csi.mode == CAM_END && cambot.x < 0.0f)
			&& (di->limb == &limb_advert06
			|| di->limb == &limb_advert07
			|| di->limb == &limb_advertref6
			|| di->limb == &limb_advertref7
			|| di->limb == &limb_billboard_3
			|| di->limb == &limb_billboard4
			|| di->limb == &limb_billboard_3_ref
			|| di->limb == &limb_billboard_4_ref)
			) ||
				((cambot.csi.mode == CAM_END && cambot.x > 0.0f)
			&& (di->limb == &limb_advert04
			|| di->limb == &limb_advert05
			|| di->limb == &limb_advertref4
			|| di->limb == &limb_advertref5
			|| di->limb == &limb_billboard_1
			|| di->limb == &limb_billboard_2
			|| di->limb == &limb_billboard_1_ref
			|| di->limb == &limb_billboard_2_ref)) )
			{
				break;
			}
			render_limb(di->limb, cm, decals, ratios);
		}
		while (0);
		++di;
	}
}

extern LIMB	limb_ballshadow_obj, limb_ball;
extern ball_data	ball_obj;			// ball
#if defined(VEGAS)
extern Texture_node_t	*ball_decal;
#else
extern GrMipMapId_t	ball_decal;
#endif
extern int ball_ratio;
extern int players_ready;

int
	sect_1 = YES, sect_2 = YES, sect_3 = YES,
	sect_1_cont = YES, sect_2_cont = YES, sect_3_cont = YES;
void draw_court( void *oi )
{

	LIMB *court_limb;
#ifdef DEBUG
	static int dc_cnt1 = 0;
	static int dc_flg1 = 0;		// -1 for entire crowd, 1 for first 5 rows
	static int dc_tog1 = 1;
#endif

#ifdef JONHEY
int k;
jonhey_t jh_obj;
LIMB tmp_limb;
int kkk;
#endif

int i, crowd_brightness = 255;
static float lod_bias = 0.0f, tx,ty,tz, spec[16][2] =
{
	{-7.25f, 2.0f},
	{-6.75f, 2.0f},
	{-6.25f, 2.0f},
	{-5.75f, 2.0f},
	{-2.75f, 2.0f},
	{-2.25f, 2.0f},
	{-1.75f, 2.0f},
	{-1.25f, 2.0f},

	{1.25f,	-2.0f},
	{1.75f,	-2.0f},
	{2.25f,	-2.0f},
	{2.75f,	-2.0f},
	{5.75f,	-2.0f},
	{6.25f,	-2.0f},
	{6.75f,	-2.0f},
	{7.25f,	-2.0f}
};

static int
	crd_frm[20 * 18] = {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0}, crd_loop_wait[20 * 18] = {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
	crd_hold[20 * 18] =
		{
			13,7,5,1,0,5,4,2,8,3,		5,7,1,10,2,4,15,2,6,3,
			5,7,1,10,2,4,15,2,6,3,	13,7,5,1,0,5,4,2,8,3,
			13,7,5,1,0,5,4,2,8,3,		5,7,1,10,2,4,15,2,6,3,
			5,7,1,10,2,4,15,2,6,3,	13,7,5,1,0,5,4,2,8,3,
			13,7,5,1,0,5,4,2,8,3,		5,7,1,10,2,4,15,2,6,3,
			5,7,1,10,2,4,15,2,6,3,	13,7,5,1,0,5,4,2,8,3,
			13,7,5,1,0,5,4,2,8,3,		5,7,1,10,2,4,15,2,6,3,
			5,7,1,10,2,4,15,2,6,3,	13,7,5,1,0,5,4,2,8,3,
			13,7,5,1,0,5,4,2,8,3,		5,7,1,10,2,4,15,2,6,3,
			5,7,1,10,2,4,15,2,6,3,	13,7,5,1,0,5,4,2,8,3,
			13,7,5,1,0,5,4,2,8,3,		5,7,1,10,2,4,15,2,6,3,
			5,7,1,10,2,4,15,2,6,3,	13,7,5,1,0,5,4,2,8,3,
			13,7,5,1,0,5,4,2,8,3,		5,7,1,10,2,4,15,2,6,3,
			5,7,1,10,2,4,15,2,6,3,	13,7,5,1,0,5,4,2,8,3,
			13,7,5,1,0,5,4,2,8,3,		5,7,1,10,2,4,15,2,6,3,
			5,7,1,10,2,4,15,2,6,3,	13,7,5,1,0,5,4,2,8,3,
			13,7,5,1,0,5,4,2,8,3,		10,7,1,20,2,4,15,2,6,3,
			5,7,1,10,2,4,15,2,6,3,	13,7,5,1,0,5,4,2,8,3
		},
	crd_hold_reset = 3;

	switch (which_court)
	{
		case COURT_INDOOR:
			court_limb = teaminfo[game_info.team[0]].court;
			break;
		case COURT_INDOOR2:
			court_limb = teaminfo[game_info.team[1]].court;
			break;
		case COURT_MIDWAY:
			court_limb = &limb_midcourt;
			break;
		case COURT_NBC:
			court_limb = &limb_NBCcourt;
			break;
		case COURT_GILMORE:
			draw_gilmore_court( oi );
			return;
		case COURT_GEER:
			draw_geer_court( oi );
			return;
	}


	xlate[0] = xlate[5] = xlate[10] = 1.0f;
	xlate[1] = xlate[2] = xlate[3] = xlate[4] =
	xlate[6] = xlate[7] = xlate[8] = xlate[9] = xlate[11] = 0.0f;

	xlate[3] = -cambot.x;
	xlate[7] = -cambot.y;
	xlate[11] = -cambot.z;

	mxm(cambot.xform, xlate, cm);

#ifndef NOSHOW_COURT
	reset_limb_mode = TRUE;
	grConstantColorValue(255<<24);
	court_limb_draw_info->limb = court_limb;
	//grTexLodBiasValue(GR_TMU0,-8.0f);
	set_limb_mode(court_limb_draw_info);
	convert_2_jonhey(&jh_obj, court_limb_draw_info->limb, cm, court_decals);
	jh_obj.limb->bbox = NULL;
	jh_obj.mode = JONHEY_MODE_DBL_SIDED;
	draw_jh_object(&jh_obj);
	//grTexLodBiasValue(GR_TMU0,0.0f);

	draw_world_object(goalshad_limb_draw_info,	sizeof(goalshad_limb_draw_info)/sizeof(limb_draw_info_t), goalshad_decals, goalshad_ratios);

 #if 1
	reset_limb_mode = TRUE;
	for(i = 0; i < 16; i++)
	{
		xlate[0] = xlate[5] = xlate[10] = 1.0f;	xlate[1] = xlate[2] = xlate[3] = xlate[4] =	xlate[6] = xlate[7] = xlate[8] = xlate[9] = xlate[11] = 0.0f;
		xlate[3] = (spec[i][0] * (STADIUM_SCALE * 1.1f)) - cambot.x;
		xlate[7] = (-7.3053f * 1.0f) - cambot.y;
		xlate[11] = (spec[i][1] * (STADIUM_SCALE * 1.05f)) - cambot.z;
		roty( ( (int)(RD2GR(cambot.theta) + 512) & 1023), xlate );
		mxm(cambot.xform, xlate, cm);

		grConstantColorValue( (255<<24) + (177<<16) + (177<<8) + 177);
		specular_limb_draw_info->limb = specular_limbs[0];

		draw_world_object(specular_limb_draw_info, sizeof(specular_limb_draw_info)/sizeof(limb_draw_info_t), specular_decals, specular_ratios);
	reset_limb_mode = FALSE;
	}
	for(i = 0; i < 16; i++)
	{
		xlate[0] = xlate[5] = xlate[10] = 1.0f;	xlate[1] = xlate[2] = xlate[3] = xlate[4] =	xlate[6] = xlate[7] = xlate[8] = xlate[9] = xlate[11] = 0.0f;
		xlate[3] = (spec[i][0] * (STADIUM_SCALE * 1.1f)) - cambot.x;
		xlate[7] = (-7.3053f * 1.0f) - cambot.y;
		xlate[11] = (-1.0f * spec[i][1] * (STADIUM_SCALE * 1.05f)) - cambot.z;
		roty( ( (int)(RD2GR(cambot.theta) + 512) & 1023), xlate );
		mxm(cambot.xform, xlate, cm);

		grConstantColorValue( (255<<24) + (177<<16) + (177<<8) + 177);
		specular_limb_draw_info->limb = specular_limbs[0];

		draw_world_object(specular_limb_draw_info, sizeof(specular_limb_draw_info)/sizeof(limb_draw_info_t), specular_decals, specular_ratios);
	reset_limb_mode = FALSE;
	}

	reset_limb_mode = TRUE;

 #endif
#endif
		xlate[0] = xlate[5] = xlate[10] = 1.0f;	xlate[1] = xlate[2] = xlate[3] = xlate[4] =	xlate[6] = xlate[7] = xlate[8] = xlate[9] = xlate[11] = 0.0f;
		xlate[3] = -cambot.x;
		xlate[7] = -cambot.y;
		xlate[11] = -cambot.z;
		mxm(cambot.xform, xlate, cm);

#ifndef NOSHOW_JUMBOTRON
	draw_world_object(jumbo_limb_draw_info,	sizeof(jumbo_limb_draw_info)/sizeof(limb_draw_info_t),	jumbo_decals,	jumbo_ratios);
#endif

#ifndef NOSHOW_STADIUM
	reset_limb_mode = TRUE;

	for(i = 0; stadium_limbs[i] != NULL; i++)
	{
		stadium_limb_draw_info->limb = stadium_limbs[i];
		draw_world_object(stadium_limb_draw_info,	sizeof(stadium_limb_draw_info)/sizeof(limb_draw_info_t), stadium_decals, stadium_ratios);
	reset_limb_mode = FALSE;
	}
	reset_limb_mode = TRUE;

#ifndef NOSHOW_CROWD
	reset_limb_mode = TRUE;
	for(i = 0; railing_limbs[i] != NULL; i++)
	{
		railing_limb_draw_info->limb = railing_limbs[i];
		draw_world_object(railing_limb_draw_info,	sizeof(railing_limb_draw_info)/sizeof(limb_draw_info_t), railing_decals, railing_ratios);
		reset_limb_mode = FALSE;
	}
	reset_limb_mode = TRUE;
#endif

#endif

#ifndef NOSHOW_CROWD

	crowd_camflash();


	if (!PLAYBACK)
	{
		crowd_cheer_time = GREATER(crowd_cheer_time - 1, 0);
		if(crowd_cheer_time == 1)
		{
			sect_1 = randrng(100) <= 30 ? NO : YES;
			sect_2 = randrng(100) >= 30 ? NO : YES;
			sect_3 = randrng(100) <= 30 ? NO : YES;
		}
	}

#ifdef JONHEY
	k=0;
//	turn_off_bbox ();
	reset_limb_mode = TRUE;
	kkk = 0;

	tmp_limb.nvtx = 0;
	tmp_limb.ntris = 0;
	tmp_limb.pvn = NULL;
	tmp_limb.pvtx = NULL;
	tmp_limb.pst = NULL;
	tmp_limb.ptri = NULL;
#endif

#ifdef DEBUG
	{
		if (dc_cnt1) dc_cnt1--;
		if (get_player_sw_close() & P2_D)
		{
			if (dc_flg1) {dc_flg1 = 0;}
			else
			if (dc_cnt1) {dc_flg1 = dc_tog1; dc_tog1 = -dc_tog1; }//fprintf(stderr,"got one: f%d c%d\r\n",dc_flg1,dc_cnt1);}
			else
			{dc_cnt1 = 10; }//fprintf(stderr,"new press: f%d c%d\r\n",dc_flg1,dc_cnt1);}
		}
	}
#endif //DEBUG

	reset_limb_mode = TRUE;
//	for(i = 0, crowd_brightness = 65; crowdhi_limbs[i] != NULL; i++, crowd_brightness += 5)  // Original
	for(i = 0, crowd_brightness = 0; crowdhi_limbs[i] != NULL; i++, crowd_brightness += 20)
	{

#ifdef JONHEY
		if (crowdbbox[k].offset == i)
			{
//			printf("inside k:%d i:%d offset:%d\n",k,i,crowdbbox[k].offset);
			convert_2_jonhey (&jh_obj, &tmp_limb, cm, crowd_decals);
			tmp_limb.bbox = crowdbbox[k].bbox;
//
			if ( object_clip (&jh_obj) == TRIVIAL_REJECTED )
				{
				i += crowdbbox[k].size-1;
//				printf("REJECTED: k:%d new i:%d\n",k,i);
				k++;
				kkk++;
				continue;
				}
//
			k++;
			}

#endif
//crowd_brightness = 200;
//		if(i % 20 ==  0) crowd_brightness = 65;  // Original
		if(i % 10 <= 1) crowd_brightness = 10;
//		if(i % 20 == 15) crowd_brightness += 40; // Original
//		if(i % 20 == 15) crowd_brightness = 0;

		grConstantColorValue( (255<<24) + (crowd_brightness<<16) + (crowd_brightness<<8) + crowd_brightness);
		crowd_limb_draw_info->limb = crowdhi_limbs[i];
#ifndef NOSHOW_CROWD_ANIM

		//if(sect_1 == YES)// || (sect_1 == NO && crd_frm[i] != 0))
		//	crowd_decals[0] = crowda_decals[crd_frm[i] & 15], crowd_ratios[0] = crowda_ratios[crd_frm[i] & 15];
		//else
		//	crowd_decals[0] = crowda_decals[0], crowd_ratios[0] = crowda_ratios[0];
		//
		//if(sect_2 == YES)// || (sect_2 == NO && crd_frm[i] != 0))
		//	crowd_decals[1] = crowdb_decals[crd_frm[i] & 15], crowd_ratios[1] = crowdb_ratios[crd_frm[i] & 15];
		//else
		//	crowd_decals[1] = crowdb_decals[0], crowd_ratios[0] = crowdb_ratios[0];

		if(PLAYBACK)
			crowd_decals[0] = crowda_decals[0], crowd_ratios[0] = crowda_ratios[0];
		else
		if(sect_1 == YES)
		{
			crowd_decals[0] = crowda_decals[crd_frm[i] & 15], crowd_ratios[2] = crowda_ratios[crd_frm[i] & 15];
		 	sect_1_cont = YES;
		}
		else
		{
			if(crd_frm[i] == 0)
				sect_1_cont = NO;
			
			if(sect_1_cont == YES)
				crowd_decals[0] = crowda_decals[crd_frm[i] & 15], crowd_ratios[2] = crowda_ratios[crd_frm[i] & 15];
			else
				crowd_decals[0] = crowda_decals[0], crowd_ratios[0] = crowda_ratios[0];
		}

		if(PLAYBACK)
			crowd_decals[1] = crowdb_decals[0], crowd_ratios[0] = crowdb_ratios[0];
		else
		if(sect_2 == YES)
		{
			crowd_decals[1] = crowdb_decals[crd_frm[i] & 15], crowd_ratios[2] = crowdb_ratios[crd_frm[i] & 15];
		 	sect_2_cont = YES;
		}
		else
		{
			if(crd_frm[i] == 0)
				sect_2_cont = NO;
			
			if(sect_2_cont == YES)
				crowd_decals[1] = crowdb_decals[crd_frm[i] & 15], crowd_ratios[2] = crowdb_ratios[crd_frm[i] & 15];
			else
				crowd_decals[1] = crowdb_decals[0], crowd_ratios[0] = crowdb_ratios[0];
		}

		if(PLAYBACK)
			crowd_decals[2] = crowdc_decals[0], crowd_ratios[0] = crowdc_ratios[0];
		else
		if(sect_3 == YES)
		{
			crowd_decals[2] = crowdc_decals[crd_frm[i] & 15], crowd_ratios[2] = crowdc_ratios[crd_frm[i] & 15];
		 	sect_3_cont = YES;
		}
		else
		{
			if(crd_frm[i] == 0)
				sect_3_cont = NO;
			
			if(sect_3_cont == YES)
				crowd_decals[2] = crowdc_decals[crd_frm[i] & 15], crowd_ratios[2] = crowdc_ratios[crd_frm[i] & 15];
			else
				crowd_decals[2] = crowdc_decals[0], crowd_ratios[0] = crowdc_ratios[0];
		}
#endif

#ifdef DEBUG
//		if ((i % 20 >= 15 && dc_flg1 > 0) ||
//			(dc_flg1 < 0))
#endif
		//if(i % 20 >= 15)
		//if((i % 20 >= 12) || (i % 20 <= 1))
		draw_world_object(crowd_limb_draw_info,	sizeof(crowd_limb_draw_info)/sizeof(limb_draw_info_t), crowd_decals, crowd_ratios);
		//printf("crd_frm[%d] = %d crowda_ratios = %d\n", i, crd_frm[i], crowda_ratios[crd_frm[i] & 15]);

	reset_limb_mode = FALSE;

 #ifndef NOSHOW_CROWD_ANIM
		if((crowd_cheer_time > 0 || crd_frm[i] != 0) && !PLAYBACK)
		{
			if(crd_loop_wait[i] <= 0)
			{
				if(crd_hold[i] == 0)
				{
					crd_hold[i] = 4;	// display this frame 4 ticks
					crd_frm[i]++;
					if(crd_frm[i] == 16)
					{	// after one complete cycle..this row waits to animate again
						crd_frm[i] &= 15;
						crd_loop_wait[i] = rand() & 15;
					}
				}
				crd_hold[i]--;
			}
			else
			{
				crd_loop_wait[i]--;
			}
		}
 #endif
	}
#endif

	reset_limb_mode = TRUE;
	for(i = 0, crowd_brightness = 255; backcrwd_limbs[i] != NULL; i++)
	{
		grConstantColorValue( (255<<24) + (crowd_brightness<<16) + (crowd_brightness<<8) + crowd_brightness);
		crowd_limb_draw_info->limb = backcrwd_limbs[i];
		draw_world_object(crowd_limb_draw_info,	sizeof(crowd_limb_draw_info)/sizeof(limb_draw_info_t), backcrwd_decals, backcrwd_ratios);
		reset_limb_mode = FALSE;
	}

#ifdef JONHEY
//		turn_on_bbox ();
	reset_limb_mode = TRUE;
//			printf("k:%d \n",kkk);
#endif

#ifndef NOSHOW_SCORETABLE
	reset_limb_mode = TRUE;
	grConstantColorValue( (50<<24) + (255<<16) + (255<<8) + 255);
	for(i = 0; onctref4_limbs[i] != NULL; i++)
	{
		oncrtref_limb_draw_info->limb = onctref4_limbs[i];
		draw_world_object(oncrtref_limb_draw_info, sizeof(oncrtref_limb_draw_info)/sizeof(limb_draw_info_t), oncrtref_decals, oncrtref_ratios);
	reset_limb_mode = FALSE;
	}
	reset_limb_mode = TRUE;

	draw_world_object(scoretable_limb_draw_info, sizeof(scoretable_limb_draw_info)/sizeof(limb_draw_info_t), oncrt_decals, oncrt_ratios);
#endif

	reset_limb_mode = TRUE;
	for(i = 0; oncrt4_limbs[i] != NULL; i++)
	{
		if(i == 7) continue;	// don't draw the score table yet
		adverts_limb_draw_info->limb = oncrt4_limbs[i];
		draw_world_object(adverts_limb_draw_info,	sizeof(adverts_limb_draw_info)/sizeof(limb_draw_info_t), oncrt_decals, oncrt_ratios);
	reset_limb_mode = FALSE;
	}
	reset_limb_mode = TRUE;

#ifndef NOSCROLL_ADVERTISEMENTS
	scroll_advert_1();
	scroll_advert_2();
	scroll_advert_3();

 #if 1
	scroll_advert_4();
	scroll_advert_5();
	scroll_advert_6();
	scroll_advert_7();
 #endif
#endif

	grConstantColorValue( (110<<24) + (255<<16) + (255<<8) + 255);
	draw_world_object(Harness_reflection_limb_draw_info, sizeof(Harness_reflection_limb_draw_info)/sizeof(limb_draw_info_t), goalref_decals, goalref_ratios);
}

#define SCROLL_TIME	120
void scroll_advert_1(void)
{
float scroll_rate = 0.2f / (float)SCROLL_TIME;
static int cur_img = 4, next_scroll_wait = (10 * 60), scroll_cnt = 0;

	next_scroll_wait = GREATER(-1, next_scroll_wait - 1);

	if(next_scroll_wait > 0)
	{
	// make sure poly is centered on current img
		advert1_st[1].t = advert1_st[3].t = (0.2f * (float)cur_img) * 256.0f;
		advert1_st[0].t = advert1_st[2].t = ((0.2f * (float)cur_img) + 0.19f) * 256.0f;

		advertref1_st[1].t = advertref1_st[3].t = (0.2f * (float)cur_img) * 256.0f;
		advertref1_st[0].t = advertref1_st[2].t = ((0.2f * (float)cur_img) + 0.19f) * 256.0f;
		return;
	}
	else
	{
		scroll_cnt = LESSER(SCROLL_TIME, scroll_cnt + 1);
		advert1_st[1].t = advert1_st[3].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate))) * 256.0f;
		advert1_st[0].t = advert1_st[2].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate)) + 0.19f) * 256.0f;

		advertref1_st[1].t = advertref1_st[3].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate))) * 256.0f;
		advertref1_st[0].t = advertref1_st[2].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate)) + 0.19f) * 256.0f;
		if(scroll_cnt > 119)
		{
			cur_img++;
			cur_img = cur_img > 4 ? 0 : cur_img;
			// Scrolling is distracting - only scroll at end of periods, etc.
			next_scroll_wait = 120 * 60 + randrng(45);
			scroll_cnt = 0;
		}
	}
}

void scroll_advert_2(void)
{
float scroll_rate = 0.2f / (float)SCROLL_TIME;
static int cur_img = 4, next_scroll_wait = (10 * 60), scroll_cnt = 0;

	next_scroll_wait = GREATER(-1, next_scroll_wait - 1);

	if(next_scroll_wait > 0)
	{
	// make sure poly is centered on current img
		advert2_st[1].t = advert2_st[3].t = (0.2f * (float)cur_img) * 256.0f;
		advert2_st[0].t = advert2_st[2].t = ((0.2f * (float)cur_img) + 0.19f) * 256.0f;

		advertref2_st[1].t = advertref2_st[3].t = (0.2f * (float)cur_img) * 256.0f;
		advertref2_st[0].t = advertref2_st[2].t = ((0.2f * (float)cur_img) + 0.19f) * 256.0f;
		return;
	}
	else
	{
		scroll_cnt = LESSER(SCROLL_TIME, scroll_cnt + 1);
		advert2_st[1].t = advert2_st[3].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate))) * 256.0f;
		advert2_st[0].t = advert2_st[2].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate)) + 0.19f) * 256.0f;

		advertref2_st[1].t = advertref2_st[3].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate))) * 256.0f;
		advertref2_st[0].t = advertref2_st[2].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate)) + 0.19f) * 256.0f;
		if(scroll_cnt > 119)
		{
			cur_img++;
			cur_img = cur_img > 4 ? 0 : cur_img;
			// Scrolling is distracting - only scroll at end of periods, etc.
			next_scroll_wait = 120 * 60 + randrng(45);
			scroll_cnt = 0;
		}
	}
}

void scroll_advert_3(void)
{
float scroll_rate = 0.2f / (float)SCROLL_TIME;
static int cur_img = 4, next_scroll_wait = (10 * 60), scroll_cnt = 0;

	next_scroll_wait = GREATER(-1, next_scroll_wait - 1);

	if(next_scroll_wait > 0)
	{
	// make sure poly is centered on current img
		advert3_st[1].t = advert3_st[3].t = (0.2f * (float)cur_img) * 256.0f;
		advert3_st[0].t = advert3_st[2].t = ((0.2f * (float)cur_img) + 0.19f) * 256.0f;

		advertref3_st[1].t = advertref3_st[3].t = (0.2f * (float)cur_img) * 256.0f;
		advertref3_st[0].t = advertref3_st[2].t = ((0.2f * (float)cur_img) + 0.19f) * 256.0f;
		return;
	}
	else
	{
		scroll_cnt = LESSER(SCROLL_TIME, scroll_cnt + 1);
		advert3_st[1].t = advert3_st[3].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate))) * 256.0f;
		advert3_st[0].t = advert3_st[2].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate)) + 0.19f) * 256.0f;

		advertref3_st[1].t = advertref3_st[3].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate))) * 256.0f;
		advertref3_st[0].t = advertref3_st[2].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate)) + 0.19f) * 256.0f;
		if(scroll_cnt > 119)
		{
			cur_img++;
			cur_img = cur_img > 4 ? 0 : cur_img;
			// Scrolling is distracting - only scroll at end of periods, etc.
			next_scroll_wait = 120 * 60 + randrng(45);
			scroll_cnt = 0;
		}
	}
}

void scroll_advert_4(void)
{
float scroll_rate = 0.2f / (float)SCROLL_TIME;
static int cur_img = 4, next_scroll_wait = (10 * 60), scroll_cnt = 0;

	next_scroll_wait = GREATER(-1, next_scroll_wait - 1);

	if(next_scroll_wait > 0)
	{
	// make sure poly is centered on current img
		advert04_st[1].t = advert04_st[3].t = (0.2f * (float)cur_img) * 256.0f;
		advert04_st[0].t = advert04_st[2].t = ((0.2f * (float)cur_img) + 0.19f) * 256.0f;

		advertref4_st[1].t = advertref4_st[3].t = (0.2f * (float)cur_img) * 256.0f;
		advertref4_st[0].t = advertref4_st[2].t = ((0.2f * (float)cur_img) + 0.19f) * 256.0f;
		return;
	}
	else
	{
		scroll_cnt = LESSER(SCROLL_TIME, scroll_cnt + 1);
		advert04_st[1].t = advert04_st[3].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate))) * 256.0f;
		advert04_st[0].t = advert04_st[2].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate)) + 0.19f) * 256.0f;

		advertref4_st[1].t = advertref4_st[3].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate))) * 256.0f;
		advertref4_st[0].t = advertref4_st[2].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate)) + 0.19f) * 256.0f;
		if(scroll_cnt > 119)
		{
			cur_img++;
			cur_img = cur_img > 4 ? 0 : cur_img;
			// Scrolling is distracting - only scroll at end of periods, etc.
			next_scroll_wait = 80 * 60 + randrng(60);
			scroll_cnt = 0;
		}
	}
}

void scroll_advert_5(void)
{
float scroll_rate = 0.2f / (float)SCROLL_TIME;
static int cur_img = 4, next_scroll_wait = (10 * 60), scroll_cnt = 0;

	next_scroll_wait = GREATER(-1, next_scroll_wait - 1);

	if(next_scroll_wait > 0)
	{
	// make sure poly is centered on current img
		advert05_st[1].t = advert05_st[3].t = (0.2f * (float)cur_img) * 256.0f;
		advert05_st[0].t = advert05_st[2].t = ((0.2f * (float)cur_img) + 0.19f) * 256.0f;

		advertref5_st[1].t = advertref5_st[3].t = (0.2f * (float)cur_img) * 256.0f;
		advertref5_st[0].t = advertref5_st[2].t = ((0.2f * (float)cur_img) + 0.19f) * 256.0f;
		return;
	}
	else
	{
		scroll_cnt = LESSER(SCROLL_TIME, scroll_cnt + 1);
		advert05_st[1].t = advert05_st[3].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate))) * 256.0f;
		advert05_st[0].t = advert05_st[2].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate)) + 0.19f) * 256.0f;

		advertref5_st[1].t = advertref5_st[3].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate))) * 256.0f;
		advertref5_st[0].t = advertref5_st[2].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate)) + 0.19f) * 256.0f;
		if(scroll_cnt > 119)
		{
			cur_img++;
			cur_img = cur_img > 4 ? 0 : cur_img;
			// Scrolling is distracting - only scroll at end of periods, etc.
			next_scroll_wait = 80 * 60 + randrng(60);
			scroll_cnt = 0;
		}
	}
}

void scroll_advert_6(void)
{
float scroll_rate = 0.2f / (float)SCROLL_TIME;
static int cur_img = 4, next_scroll_wait = (10 * 60), scroll_cnt = 0;

	next_scroll_wait = GREATER(-1, next_scroll_wait - 1);

	if(next_scroll_wait > 0)
	{
	// make sure poly is centered on current img
		advert06_st[1].t = advert06_st[3].t = (0.2f * (float)cur_img) * 256.0f;
		advert06_st[0].t = advert06_st[2].t = ((0.2f * (float)cur_img) + 0.19f) * 256.0f;

		advertref6_st[1].t = advertref6_st[3].t = (0.2f * (float)cur_img) * 256.0f;
		advertref6_st[0].t = advertref6_st[2].t = ((0.2f * (float)cur_img) + 0.19f) * 256.0f;
		return;
	}
	else
	{
		scroll_cnt = LESSER(SCROLL_TIME, scroll_cnt + 1);
		advert06_st[1].t = advert06_st[3].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate))) * 256.0f;
		advert06_st[0].t = advert06_st[2].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate)) + 0.19f) * 256.0f;

		advertref6_st[1].t = advertref6_st[3].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate))) * 256.0f;
		advertref6_st[0].t = advertref6_st[2].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate)) + 0.19f) * 256.0f;
		if(scroll_cnt > 119)
		{
			cur_img++;
			cur_img = cur_img > 4 ? 0 : cur_img;
			// Scrolling is distracting - only scroll at end of periods, etc.
			next_scroll_wait = 80 * 60 + randrng(60);
			scroll_cnt = 0;
		}
	}
}

void scroll_advert_7(void)
{
float scroll_rate = 0.2f / (float)SCROLL_TIME;
static int cur_img = 4, next_scroll_wait = (10 * 60), scroll_cnt = 0;

	next_scroll_wait = GREATER(-1, next_scroll_wait - 1);

	if(next_scroll_wait > 0)
	{
	// make sure poly is centered on current img
		advert07_st[1].t = advert07_st[3].t = (0.2f * (float)cur_img) * 256.0f;
		advert07_st[0].t = advert07_st[2].t = ((0.2f * (float)cur_img) + 0.19f) * 256.0f;

		advertref7_st[1].t = advertref7_st[3].t = (0.2f * (float)cur_img) * 256.0f;
		advertref7_st[0].t = advertref7_st[2].t = ((0.2f * (float)cur_img) + 0.19f) * 256.0f;
		return;
	}
	else
	{
		scroll_cnt = LESSER(SCROLL_TIME, scroll_cnt + 1);
		advert07_st[1].t = advert07_st[3].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate))) * 256.0f;
		advert07_st[0].t = advert07_st[2].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate)) + 0.19f) * 256.0f;

		advertref7_st[1].t = advertref7_st[3].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate))) * 256.0f;
		advertref7_st[0].t = advertref7_st[2].t = ((((float)cur_img * 0.2f)+((float)scroll_cnt * scroll_rate)) + 0.19f) * 256.0f;
		if(scroll_cnt > 119)
		{
			cur_img++;
			cur_img = cur_img > 4 ? 0 : cur_img;
			// Scrolling is distracting - only scroll at end of periods, etc.
			next_scroll_wait = 80 * 60 + randrng(60);
			scroll_cnt = 0;
		}
	}
}


/********************************
 Draw backboards, supports, rims and nets
   Note that replays use this object code also
********************************/
void	draw_goals (void *oi)
{
	BBDATA * bbd_p;
	VERTEX * pvtx_tmp;

#ifdef JONHEY
	VERTEX * bbox_tmp;
	jonhey_t jh_obj;
#endif

	void * v_p;
	float	bendm[12], m[12], m2[12], m3[12], f;
	int		i, end_side = ((game_info.game_quarter & 2) >> 1) ^ game_info.home_team;

//컴컴컴컴컴컴컴�	Debug stuff

#ifdef	DEBUG
	static int	_rotx;
	static int	_roty;

	i = get_p5_sw_current();
	if (i & P_C) {
		if (i & P_LEFT) _rotx -= 16;
		if (i & P_RIGHT) _rotx += 16;
		if (i & P_UP) _roty += 16;
		if (i & P_DOWN) _roty -= 16;
	}
//	bbd_p = &bbdata[1];
//	bbd_p->bend = _rotx;
//	bbd_p->rimbend = _roty;
#endif

	switch (which_court)
	{
		case COURT_GILMORE:
			draw_gilmore_goals( oi );
			return;
		case COURT_GEER:
			draw_geer_goals( oi );
			return;
	}

#ifdef	SHOW_TEST_HEAD
	head_showtest();
#endif

#ifdef	NOSHOW_GOALS
	return;
#endif

	grColorCombine (
			GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL,
			GR_COMBINE_FACTOR_ONE,
			GR_COMBINE_LOCAL_CONSTANT,
			GR_COMBINE_OTHER_TEXTURE,
			0);

//컴컴컴컴컴컴컴�	Draw left rim

	bbd_p = (BBDATA *)oi;

	mat_setrotz_prec (bbd_p->bend, bendm);

	m[0]  = m[5] = m[10] = 1.0f;
	m[1]  = m[2] = m[4] = m[6] = m[8] = m[9] = 0.0f;
	m[3]  =  RIM_ROTATE_X;
	m[7]  = -RIM_ROTATE_Y;
	m[11] = -RIM_Z;

	m2[3]  = -RIM_ROTATE_X + RIM_CX + 45;
	m2[7]  =  RIM_ROTATE_Y - RIM_Y * 0.3f;
	m2[11] =         RIM_Z - RIM_Z;
	rotz ((int) bbd_p->rimbend >> 6, m2);
	mxm (m2, m, m3);

	bendm[3]  = -cambot.x - RIM_CX - 45;
	bendm[7]  = -cambot.y + RIM_Y * 0.3f;
	bendm[11] = -cambot.z + RIM_Z;
	mxm (bendm, m3, m);
	mxm (cambot.xform, m, cm);

	grConstantColorValue (bbd_p->rimcolor);

	draw_world_object (Rim_limb_draw_info,
			sizeof(Rim_limb_draw_info)/sizeof(limb_draw_info_t),
			Goals_Decals,
			Goals_Ratios);

//컴컴컴컴컴컴컴� Animate & draw left net

	if (PLAYBACK)
	{
		// In a replay; save the current net & set the recorded one
		pvtx_tmp = limb_Net.pvtx;
		limb_Net.pvtx = ((replay_goal_t *)bbd_p)->pvtx;
#ifdef JONHEY
		bbox_tmp = limb_Net.bbox;
		limb_Net.bbox = ((replay_goal_t *)bbd_p)->bbox;
#endif
	}
	else
	if (bbd_p->netv_p)
	{
		if ((v_p = *bbd_p->netv_p))
		{
			limb_Net.pvtx = v_p;
#ifdef JONHEY
			limb_Net.bbox = *bbd_p->netv_bbox;
#endif
#ifdef	DEBUG
			if (!halt)
			{
#endif
			f  = bbd_p->netvposf;
			f += bbd_p->netvstep;
			while (f >= 1)					// Frac >= 1? Step ahead X frames
			{
				f -= 1;
				bbd_p->netv_p++;
#ifdef JONHEY
				bbd_p->netv_bbox++;
#endif
				bbd_p->netvpritm--;
				if (*bbd_p->netv_p == 0)	// End?
					f = 0;
			}
			bbd_p->netvposf = f;
#ifdef	DEBUG
	 		}
#endif
		}
		else
		{
			bbd_p->netv_p = 0;
#ifdef JONHEY
			bbd_p->netv_bbox = 0;
#endif
		}
	}

	m[3]  = 4.3425f;
	m[7]  = 0.0f;
	m[11] = 0.0f;
	roty (bbd_p->netya, m);

	m2[3]  = -4.3425f - RIM_CX + RIM_CX + 45;
	m2[7]  =     0.0f + RIM_CY - RIM_Y * 0.3f;
	m2[11] =     0.0f + RIM_CZ - RIM_Z;
	rotz ((int) bbd_p->rimbend >> 6, m2);
	mxm (m2, m, m3);

	bendm[3]  = -cambot.x - RIM_CX - 45;
	bendm[7]  = -cambot.y + RIM_Y * 0.3f;
	bendm[11] = -cambot.z + RIM_Z;
	mxm (bendm, m3, m);
	mxm (cambot.xform, m, cm);

	grConstantColorValue (bbd_p->netcolor);

	set_limb_mode(Net_limb_draw_info);
	convert_2_jonhey (&jh_obj, Net_limb_draw_info->limb, cm, Goals_Decals);
	jh_obj.mode = JONHEY_MODE_DBL_SIDED;
	draw_jh_object(&jh_obj);

	if (PLAYBACK)
	{
		// In a replay; restore the current net
		limb_Net.pvtx = pvtx_tmp;
#ifdef JONHEY
		limb_Net.bbox = bbox_tmp;
#endif
	}

//컴컴컴컴컴컴컴� Draw left backboard & stand

	if (cambot.csi.mode != CAM_END || end_side)
	{
		m[0]  = m[5] = m[10] = 1.0f;
		m[1]  = m[2] = m[4] = m[6] = m[8] = m[9] = 0.0f;
		m[3]  = -cambot.x;
		m[7]  = -cambot.y;
		m[11] = -cambot.z;
		mxm (cambot.xform, m, cm);

		grConstantColorValue (255 << 24);

		draw_world_object (goalbase_limb_draw_info,
				sizeof(goalbase_limb_draw_info)/sizeof(limb_draw_info_t),
				Goals_Decals,
				Goals_Ratios);
	}
	m[0]  = m[5] = m[10] = 1.0f;
	m[1]  = m[2] = m[4] = m[6] = m[8] = m[9] = 0.0f;
	m[3]  =  RIM_CX + 45;
	m[7]  = -RIM_Y * 0.3f;
	m[11] = -RIM_Z;

	bendm[3]  = -cambot.x - RIM_CX - 45;
	bendm[7]  = -cambot.y + RIM_Y * 0.3f;
	bendm[11] = -cambot.z + RIM_Z;
	mxm (bendm, m, m2);
	mxm (cambot.xform, m2, cm);

	i = 185 << 24;
	if (cambot.csi.mode != CAM_END || end_side)
	{
		i = 255 << 24;

		draw_world_object (goalstnd_limb_draw_info,
				sizeof(goalstnd_limb_draw_info)/sizeof(limb_draw_info_t),
				Goals_Decals,
				Goals_Ratios);
	}

	grConstantColorValue (i);

	set_limb_mode(glass_limb_draw_info);
	convert_2_jonhey (&jh_obj, glass_limb_draw_info->limb, cm, Goals_Decals);
	jh_obj.mode = JONHEY_MODE_DBL_SIDED;
	draw_jh_object(&jh_obj);

	draw_world_object (backboard_limb_draw_info,
			sizeof(backboard_limb_draw_info)/sizeof(limb_draw_info_t),
			Goals_Decals,
			Goals_Ratios);



//컴컴컴컴컴컴컴�	Draw right rim

	if (PLAYBACK)
		((replay_obj_t *)bbd_p)++;
	else
		((BBDATA *)bbd_p)++;

	mat_setrotz_prec (-bbd_p->bend, bendm);

	m[0]  = m[5] = m[10] = 1.0f;
	m[1]  = m[2] = m[4] = m[6] = m[8] = m[9] = 0.0f;
	m[3]  = -RIM_ROTATE_X;
	m[7]  = -RIM_ROTATE_Y;
	m[11] = -RIM_Z;

	m2[3]  = RIM_ROTATE_X - RIM_CX - 45;
	m2[7]  = RIM_ROTATE_Y - RIM_Y * 0.3f;
	m2[11] =        RIM_Z - RIM_Z;
	rotz (-(int) bbd_p->rimbend >> 6, m2);
	mxm (m2, m, m3);

	bendm[3]  = -cambot.x + RIM_CX + 45;
	bendm[7]  = -cambot.y + RIM_Y * 0.3f;
	bendm[11] = -cambot.z + RIM_Z;
	mxm (bendm, m3, m);
	mxm (cambot.xform, m, cm);

	grConstantColorValue (bbd_p->rimcolor);

	draw_world_object (Rim_2_limb_draw_info,
			sizeof(Rim_2_limb_draw_info)/sizeof(limb_draw_info_t),
			Goals_Decals,
			Goals_Ratios);

//컴컴컴컴컴컴컴� Animate & draw right net

	if (PLAYBACK)
	{
		// In a replay; save the current net & set the recorded one
		pvtx_tmp = limb_Net2.pvtx;
		limb_Net2.pvtx = ((replay_goal_t *)bbd_p)->pvtx;
#ifdef JONHEY
		bbox_tmp = limb_Net2.bbox;
		limb_Net2.bbox = ((replay_goal_t *)bbd_p)->bbox;
#endif
	}
	else
	if (bbd_p->netv_p)
	{
		if ((v_p = *bbd_p->netv_p))
		{
			limb_Net2.pvtx = v_p;
#ifdef JONHEY
			limb_Net2.bbox = *bbd_p->netv_bbox;
#endif
#ifdef	DEBUG
			if (!halt)
			{
#endif
			f  = bbd_p->netvposf;
			f += bbd_p->netvstep;
			while (f >= 1)					// Frac >= 1? Step ahead X frames
			{
				f -= 1;
				bbd_p->netv_p++;
#ifdef JONHEY
				bbd_p->netv_bbox++;
#endif
				bbd_p->netvpritm--;
				if (*bbd_p->netv_p == 0)	// End?
					f = 0;
			}
			bbd_p->netvposf = f;
#ifdef	DEBUG
	 		}
#endif
		}
		else 
		{
			bbd_p->netv_p = 0;
#ifdef JONHEY
			bbd_p->netv_bbox = 0;
#endif
		}
	}

	m[3]  = -4.3425f;
	m[7]  = 0.0f;
	m[11] = 0.0f;
	roty (bbd_p->netya, m);

	m2[3]  = 4.3425f + RIM_CX - RIM_CX - 45;
	m2[7]  =    0.0f + RIM_CY - RIM_Y * 0.3f;
	m2[11] =    0.0f + RIM_CZ - RIM_Z;
	rotz (-(int) bbd_p->rimbend >> 6, m2);
	mxm (m2, m, m3);

	bendm[3]  = -cambot.x + RIM_CX + 45;
	bendm[7]  = -cambot.y + RIM_Y * 0.3f;
	bendm[11] = -cambot.z + RIM_Z;
	mxm (bendm, m3, m);
	mxm (cambot.xform, m, cm);

	grConstantColorValue (bbd_p->netcolor);

	set_limb_mode(Net_2_limb_draw_info);
	convert_2_jonhey (&jh_obj, Net_2_limb_draw_info->limb, cm, Goals_Decals);
	jh_obj.mode = JONHEY_MODE_DBL_SIDED;
	draw_jh_object(&jh_obj);

	if (PLAYBACK)
	{
		// In a replay; restore the current net
		limb_Net2.pvtx = pvtx_tmp;
#ifdef JONHEY
		limb_Net2.bbox = bbox_tmp;
#endif
	}

//컴컴컴컴컴컴컴� Draw right backboard & stand

	if (cambot.csi.mode != CAM_END || !end_side)
	{
		m[0]  = m[5] = m[10] = 1.0f;
		m[1]  = m[2] = m[4] = m[6] = m[8] = m[9] = 0.0f;
		m[3]  = -cambot.x;
		m[7]  = -cambot.y;
		m[11] = -cambot.z;
		mxm (cambot.xform, m, cm);

		grConstantColorValue (255 << 24);

		draw_world_object (goalbase2_limb_draw_info,
				sizeof(goalbase2_limb_draw_info)/sizeof(limb_draw_info_t),
				Goals_Decals,
				Goals_Ratios);
	}
	m[0]  = m[5] = m[10] = 1.0f;
	m[1]  = m[2] = m[4] = m[6] = m[8] = m[9] = 0.0f;
	m[3]  = -RIM_CX - 45;
	m[7]  = -RIM_Y * 0.3f;
	m[11] = -RIM_Z;

	bendm[3]  = -cambot.x + RIM_CX + 45;
	bendm[7]  = -cambot.y + RIM_Y * 0.3f;
	bendm[11] = -cambot.z + RIM_Z;
	mxm (bendm, m, m2);
	mxm (cambot.xform, m2, cm);

	i = 185 << 24;
	if (cambot.csi.mode != CAM_END || !end_side)
	{
		i = 255 << 24;

		draw_world_object (goalstnd2_limb_draw_info,
				sizeof(goalstnd2_limb_draw_info)/sizeof(limb_draw_info_t),
				Goals_Decals,
				Goals_Ratios);
	}

	grConstantColorValue (i);

	set_limb_mode(glass2_limb_draw_info);
	convert_2_jonhey (&jh_obj, glass2_limb_draw_info->limb, cm, Goals_Decals);
	jh_obj.mode = JONHEY_MODE_DBL_SIDED;
	draw_jh_object(&jh_obj);

	draw_world_object (backboard2_limb_draw_info,
			sizeof(backboard2_limb_draw_info)/sizeof(limb_draw_info_t),
			Goals_Decals,
			Goals_Ratios);
}

/********************************
 Set a matrix for Z rotation. Use high precision math.
 (Angle 0-0xffff, * to 4x3 or 4x4 matrix)
********************************/

void	mat_setrotz_prec (int ang, float *m_p)
{
	float		f;

	f = ang * (M_PI / 65536.0f);						    	

	m_p[0] = cos (f);
	m_p[5] = m_p[0];
	m_p[4] = sin (f);
	m_p[1] = -m_p[4];
	m_p[2] = 0;
	m_p[6] = 0;
	m_p[8] = 0;
	m_p[9] = 0;
	m_p[10] = 1;
}

/********************************
 Rob's head test
********************************/

#ifdef SHOW_TEST_HEAD

#define	JOY_UP		1
#define	JOY_DOWN	2
#define	JOY_LEFT	4
#define	JOY_RIGHT	8

void	head_showtest()
{
	int		joy, but;
	float		sf = 2.10f;
	static int rob = 0, rob2 = 0;

//컴컴컴컴컴컴컴�

	joy = get_p5_sw_current();

	m[0] = m[5] = m[10] = 1.0f;
	m[1] = m[2] = m[3] = m[4] =
	m[6] = m[7] = m[8] = m[9] = m[11] = 0.0f;

	m[3] = -cambot.x;
	m[7] = 20.0f - cambot.y;

	if(joy & JOY_UP)
	{
		rob += 2;
		rob %= 1024;
	}
	if(joy & JOY_DOWN)
	{
		rob -= 2;
		rob %= 1024;
	}
	if(joy & JOY_LEFT)
	{
		rob2 += 2;
		rob2 %= 1024;
	}
	if(joy & JOY_RIGHT)
	{
		rob2 -= 2;
		rob2 %= 1024;
	}

	m[11] = -cambot.z;

//	rotxyz(rob2, rob, 0, m );
	rotx(rob, m );

//	rob += 2;
//	rob = rob > 1023 ? 1023 - rob : rob;

	mxm(cambot.xform, m, cm);

	cm[0] *= sf; cm[1] *= sf; cm[2] *= sf;
	cm[4] *= sf; cm[5] *= sf; cm[6] *= sf;
	cm[8] *= sf; cm[9] *= sf; cm[10] *= sf;

	draw_world_object(head_limb_draw_info,
		sizeof(head_limb_draw_info)/sizeof(limb_draw_info_t),
		head_decals,
		head_ratios);
}
#endif

/******************************************************************************
* gilmore arena
******************************************************************************/
#define BASE_NONALPHA_LIMB(x,y) {(x),0,GR_DEPTHBUFFER_WBUFFER,GR_CMP_LESS,FXTRUE,0, \
		GR_COLORCOMBINE_DECAL_TEXTURE,GR_ALPHASOURCE_CC_ALPHA,GR_TEXTURECOMBINE_DECAL, \
		GR_TEXTURECLAMP_CLAMP,GR_BLEND_ONE,GR_BLEND_ZERO,GR_BLEND_ONE, \
		GR_BLEND_ZERO,GR_CHROMAKEY_DISABLE,0,GR_CMP_ALWAYS,2,(y)}

#define NONALPHA_LIMB(x)	BASE_NONALPHA_LIMB((x),GR_CULL_NEGATIVE)
#define NONALPHA_LIMB_DS(x)	BASE_NONALPHA_LIMB((x),GR_CULL_DISABLE)

#define BASE_ALPHA_LIMB(x,y,z,s) {(x),0,GR_DEPTHBUFFER_WBUFFER,GR_CMP_LESS,FXTRUE,0, \
		GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,(s), \
		GR_TEXTURECOMBINE_DECAL,GR_TEXTURECLAMP_CLAMP,GR_BLEND_SRC_ALPHA, \
		GR_BLEND_ONE_MINUS_SRC_ALPHA,GR_BLEND_SRC_ALPHA,GR_BLEND_ONE_MINUS_SRC_ALPHA, \
		GR_CHROMAKEY_DISABLE,0,(y),(z),GR_CULL_NEGATIVE}

#define BASE_ALPHAADD_LIMB(x,y,z,s) {(x),0,GR_DEPTHBUFFER_WBUFFER,GR_CMP_LESS,FXTRUE,0, \
		GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,(s), \
		GR_TEXTURECOMBINE_DECAL,GR_TEXTURECLAMP_CLAMP,GR_BLEND_SRC_ALPHA, \
		GR_BLEND_ONE,GR_BLEND_SRC_ALPHA,GR_BLEND_ONE_MINUS_SRC_ALPHA, \
		GR_CHROMAKEY_DISABLE,0,(y),(z),GR_CULL_NEGATIVE}

#define BASE_ALPHA_LIMB_DS(x,y,z) {(x),0,GR_DEPTHBUFFER_WBUFFER,GR_CMP_LESS,FXTRUE,0, \
		GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,GR_ALPHASOURCE_TEXTURE_ALPHA, \
		GR_TEXTURECOMBINE_DECAL,GR_TEXTURECLAMP_CLAMP,GR_BLEND_SRC_ALPHA, \
		GR_BLEND_ONE_MINUS_SRC_ALPHA,GR_BLEND_SRC_ALPHA,GR_BLEND_ONE_MINUS_SRC_ALPHA, \
		GR_CHROMAKEY_DISABLE,0,(y),(z),GR_CULL_DISABLE}

#define ALPHA_LIMB(x)		BASE_ALPHA_LIMB((x),GR_CMP_ALWAYS,2,GR_ALPHASOURCE_TEXTURE_ALPHA)
#define ALPHAADD_LIMB(x)	BASE_ALPHAADD_LIMB((x),GR_CMP_ALWAYS,2,GR_ALPHASOURCE_TEXTURE_ALPHA)
#define ALPHA_LIMB_CCA(x)	BASE_ALPHA_LIMB((x),GR_CMP_ALWAYS,2,GR_ALPHASOURCE_CC_ALPHA)
#define ALPHA_LIMB_GR(x,y)	BASE_ALPHA_LIMB((x),GR_CMP_GREATER,(y),GR_ALPHASOURCE_TEXTURE_ALPHA)
#define ALPHA_LIMB_GR_DS(x,y)	BASE_ALPHA_LIMB_DS((x),GR_CMP_GREATER,(y))

char *parknet_textures[] =
{
	"rwbnet.wms",
	"rwbnet.wms",
	"rwbnet.wms",
	"rwbnet.wms",
	"rwbnet.wms",
	"rwbnet.wms",
	"rwbnet.wms",
	"rwbnet.wms",
	"rwbnet.wms",
	NULL
};

static limb_draw_info_t parkflr_limb_draw_info[] =
{
	ALPHA_LIMB_GR(&limb_park_fence,64),
	NONALPHA_LIMB(&limb_parcourt),
};

static limb_draw_info_t parkgoal_limb_draw_info[] =
{
	ALPHA_LIMB_GR(&limb_park_goal_grass2,64),
	ALPHA_LIMB_GR(&limb_park_goal_grass1,64),
	NONALPHA_LIMB_DS(&limb_park_goal_target_2),
	NONALPHA_LIMB_DS(&limb_park_goal_target_1),

//	NONALPHA_LIMB(&limb_park_goal_pole_1),
//	NONALPHA_LIMB(&limb_park_goal_pole_2),
};

//static limb_draw_info_t parkbbd_limb_draw_info[] =
//{
//	NONALPHA_LIMB(&limb_park_backboard_2),
//	NONALPHA_LIMB(&limb_park_backboard_1),
//};

static limb_draw_info_t parkbbd_limb_draw_info[] =
{
	ALPHA_LIMB_CCA(&limb_park_backboard_2),
	ALPHA_LIMB_CCA(&limb_park_backboard_1),
};

static limb_draw_info_t parkpole_limb_draw_info[] =
{
	ALPHA_LIMB_CCA(&limb_park_goal_pole_2),
	ALPHA_LIMB_CCA(&limb_park_goal_pole_1),
};

static limb_draw_info_t prkgshad_limb_draw_info[] =
{
	ALPHA_LIMB_GR(&limb_pkgoalshd2,0),
	ALPHA_LIMB_GR(&limb_pkgoalshd1,0),
};

static limb_draw_info_t parknet_limb_draw_info[] =
{
	ALPHA_LIMB_GR_DS(&limb_Net,64),
	ALPHA_LIMB_GR_DS(&limb_Net2,64),
};

static limb_draw_info_t parkrim_limb_draw_info[] =
{
	NONALPHA_LIMB(&limb_park_Rim2),
	NONALPHA_LIMB(&limb_park_Rim1),
};

static limb_draw_info_t parkonct_limb_draw_info[] =
{
	NONALPHA_LIMB(&limb_trashcan01),
	NONALPHA_LIMB(&limb_trashcan02),
	NONALPHA_LIMB(&limb_bench01),
	NONALPHA_LIMB(&limb_bench02),
	NONALPHA_LIMB(&limb_bench03),
	NONALPHA_LIMB(&limb_bench04),
	NONALPHA_LIMB(&limb_bench05),
	NONALPHA_LIMB(&limb_bench06),
	NONALPHA_LIMB(&limb_bench07),
	NONALPHA_LIMB(&limb_bench08),
};

static limb_draw_info_t parkenv_limb_draw_info[] =
{
//	NONALPHA_LIMB(&limb_park_sky),
	NONALPHA_LIMB(&limb_scenery),
	NONALPHA_LIMB(&limb_continent),
	ALPHA_LIMB_GR(&limb_house01,64),
	ALPHA_LIMB_GR(&limb_house02,64),
	ALPHA_LIMB_GR(&limb_house03,64),
	ALPHA_LIMB_GR(&limb_house04,64),
	ALPHA_LIMB_GR(&limb_house05,64),
	ALPHA_LIMB_GR(&limb_house06,64),
	ALPHA_LIMB_GR(&limb_house07,64),
	ALPHA_LIMB_GR(&limb_house08,64),
	ALPHA_LIMB_GR(&limb_house09,64),
	ALPHA_LIMB_GR(&limb_house10,64),
	ALPHA_LIMB_GR(&limb_house11,64),
	ALPHA_LIMB_GR(&limb_house12,64),
	NONALPHA_LIMB(&limb_alley_trashcans),
	NONALPHA_LIMB(&limb_sidewalk_west),
	NONALPHA_LIMB(&limb_sidewalk_southwest),
	NONALPHA_LIMB(&limb_sidewalk_northwest),
	NONALPHA_LIMB(&limb_sidewalk_northeast),
	NONALPHA_LIMB(&limb_sidewalk_east),
	NONALPHA_LIMB(&limb_sidewalk_southeast),
	NONALPHA_LIMB(&limb_chimney01),
	NONALPHA_LIMB(&limb_chimney02),
	NONALPHA_LIMB(&limb_chimney03),
	NONALPHA_LIMB(&limb_chimney04),
	NONALPHA_LIMB(&limb_chimney05),
	NONALPHA_LIMB(&limb_chimney06),
	ALPHA_LIMB_GR(&limb_BG_Northwest,64),
	ALPHA_LIMB_GR(&limb_BG_NorthEast,64),
	ALPHA_LIMB_GR(&limb_BG_southwest,64),
	ALPHA_LIMB_GR(&limb_BG_southeast,64),
	NONALPHA_LIMB(&limb_street_west),
	NONALPHA_LIMB(&limb_street_southwest),
	NONALPHA_LIMB(&limb_street_northwest),
	NONALPHA_LIMB(&limb_street_east),
	NONALPHA_LIMB(&limb_street_southeast),
	NONALPHA_LIMB(&limb_street_northeast),
	NONALPHA_LIMB(&limb_park_sidewalk),
	NONALPHA_LIMB(&limb_grass_patches),
	NONALPHA_LIMB(&limb_stormfan05),
	NONALPHA_LIMB(&limb_stormfan06),
	NONALPHA_LIMB(&limb_stormfan07),
	NONALPHA_LIMB(&limb_stormfan08),
	NONALPHA_LIMB(&limb_stormfan04),
	NONALPHA_LIMB(&limb_stormfan03),
	NONALPHA_LIMB(&limb_stormfan02),
	NONALPHA_LIMB(&limb_stormfan01),
	NONALPHA_LIMB(&limb_building_southwest),
	NONALPHA_LIMB(&limb_building_northeast),
	NONALPHA_LIMB(&limb_building_northwest),
	NONALPHA_LIMB(&limb_building_southeast),
	ALPHA_LIMB(&limb_watertower_tank),
	ALPHA_LIMB(&limb_watertower_base),
};

/*****************************************************************************/
static void gilmore_arena( void )
{
	int			i;

	init_limb_draw_info(parkflr_limb_draw_info, sizeof(parkflr_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(parkgoal_limb_draw_info, sizeof(parkgoal_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(parknet_limb_draw_info, sizeof(parknet_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(parkonct_limb_draw_info, sizeof(parkonct_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(parkenv_limb_draw_info, sizeof(parkenv_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(prkgshad_limb_draw_info, sizeof(prkgshad_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(parkpole_limb_draw_info, sizeof(parkpole_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(parkbbd_limb_draw_info, sizeof(parkbbd_limb_draw_info)/sizeof(limb_draw_info_t));

	arena_texture_buffer_index = 0;

	get_buffer(&parkflr_decals, &parkflr_ratios, tex_count(parkflr_textures));
	get_buffer(&parkgoal_decals, &parkgoal_ratios, tex_count(parkgoal_textures));
	get_buffer(&parknet_decals, &parknet_ratios, tex_count(parknet_textures));
	get_buffer(&parkonct_decals, &parkonct_ratios, tex_count(parkonct_textures));
	get_buffer(&parkenv_decals, &parkenv_ratios, tex_count(parkenv_textures));
	get_buffer(&prkgshad_decals, &prkgshad_ratios, tex_count(prkgshad_textures));

	// use chain net
	parkgoal_textures[3] = "rwbnet.wms";

	load_textures(parkflr_textures, parkflr_ratios, parkflr_decals, tex_count(parkflr_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
	load_textures(parkgoal_textures, parkgoal_ratios, parkgoal_decals, tex_count(parkgoal_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
	load_textures(parknet_textures, parknet_ratios, parknet_decals, tex_count(parknet_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
	load_textures(parkonct_textures, parkonct_ratios, parkonct_decals, tex_count(parkonct_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
	load_textures(parkenv_textures, parkenv_ratios, parkenv_decals, tex_count(parkenv_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
	load_textures(prkgshad_textures, prkgshad_ratios, prkgshad_decals, tex_count(prkgshad_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);

	// clamp floor textures
	for(i = 0; parkflr_textures[i]; i++)
	{
		if (!strcmp(parkflr_textures[i],"parkgraf.wms" ))
			continue;
		if (!strcmp(parkflr_textures[i],"parkfnce.wms" ))
			continue;

		guTexChangeAttributes( parkflr_decals[i], -1, -1, -1, -1, -1, -1,
				-1, GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP, -1, -1);
	}

	// clamp goal textures
	for(i = 0; parkgoal_textures[i]; i++)
	{
		if (!strcmp(parkgoal_textures[i],"net.wms" ))
			continue;
		if (!strcmp(parkgoal_textures[i],"hoop.wms" ))
			continue;
		if (!strcmp(parkgoal_textures[i],"pole1.wms" ))
			continue;

		guTexChangeAttributes( parkgoal_decals[i], -1, -1, -1, -1, -1, -1,
				-1, GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP, -1, -1);
	}

	// clamp onct textures
	for(i = 0; parkonct_textures[i]; i++)
	{
		guTexChangeAttributes( parkonct_decals[i], -1, -1, -1, -1, -1, -1,
				-1, GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP, -1, -1);
	}

	// clamp env textures
	for(i = 0; parkenv_textures[i]; i++)
	{
		if (!strcmp(parkenv_textures[i],"stormfan.wms" ))
			continue;
		if (!strcmp(parkenv_textures[i],"street.wms" ))
			continue;
		if ((i>=22) && (i<=28))		// sidewlk?.wms
			continue;

		guTexChangeAttributes( parkenv_decals[i], -1, -1, -1, -1, -1, -1,
				-1, GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP, -1, -1);
	}

	// clamp gshad textures
	for(i = 0; prkgshad_textures[i]; i++)
	{
		guTexChangeAttributes( prkgshad_decals[i], -1, -1, -1, -1, -1, -1,
				-1, GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP, -1, -1);
	}

}

/*****************************************************************************/
#define DR_INC		0x00000800
static void draw_gilmore_court( void *oi )
{
	static int		dome_angle = 0;
	float			m1[12];

	xlate[0] = xlate[5] = xlate[10] = 1.0f;
	xlate[1] = xlate[2] = xlate[3] = xlate[4] =
	xlate[6] = xlate[7] = xlate[8] = xlate[9] = xlate[11] = 0.0f;

	xlate[3] = -cambot.x;
	xlate[7] = -cambot.y;
	xlate[11] = -cambot.z;

	mxm(cambot.xform, xlate, cm);

	roty1616(dome_angle, xlate);
	dome_angle += DR_INC;

	dome_angle = dome_angle % 0x04000000;

	mxm( cambot.xform, xlate, m1 );

	reset_limb_mode = TRUE;

	set_limb_mode(parkenv_limb_draw_info);
	grConstantColorValue (255 << 24);

	render_limb(&limb_park_sky, m1, parkenv_decals, parkenv_ratios );

	draw_world_object(parkenv_limb_draw_info,
			sizeof(parkenv_limb_draw_info)/sizeof(limb_draw_info_t),
			parkenv_decals, parkenv_ratios);

	set_limb_mode(parkflr_limb_draw_info);

	draw_world_object(parkflr_limb_draw_info,
			sizeof(parkflr_limb_draw_info)/sizeof(limb_draw_info_t),
			parkflr_decals, parkflr_ratios);

	return;
}

/*****************************************************************************/
static void gilmore_left_pole( void );
static void gilmore_right_pole( void );
static void gilmore_left_bbd( void );
static void gilmore_right_bbd( void );

static void draw_gilmore_goals( void *oi )
{
	BBDATA	*bbd_p;
	VERTEX	*pvtx_tmp;
	VERTEX	*bbox_tmp;
	void	*v_p;
	float	bendm[12], m[12], m2[12], m3[12], f;
	jonhey_t	jh_obj;
	int		end_side = ((game_info.game_quarter & 2) >> 1) ^ game_info.home_team;

	xlate[0] = xlate[5] = xlate[10] = 1.0f;
	xlate[1] = xlate[2] = xlate[3] = xlate[4] =
	xlate[6] = xlate[7] = xlate[8] = xlate[9] = xlate[11] = 0.0f;

	xlate[3] = -cambot.x;
	xlate[7] = -cambot.y;
	xlate[11] = -cambot.z;

	mxm(cambot.xform, xlate, cm);

	// trashcans and benches (here because bottom benches can be over players
	draw_world_object(parkonct_limb_draw_info,
			sizeof(parkonct_limb_draw_info)/sizeof(limb_draw_info_t),
			parkonct_decals, parkonct_ratios);

	set_limb_mode( prkgshad_limb_draw_info + 0 );

	// goal shadows
	draw_world_object(prkgshad_limb_draw_info,
			sizeof(prkgshad_limb_draw_info)/sizeof(limb_draw_info_t),
			prkgshad_decals, prkgshad_ratios);

	// grass and targets
	draw_world_object(parkgoal_limb_draw_info,
			sizeof(parkgoal_limb_draw_info)/sizeof(limb_draw_info_t),
			parkgoal_decals, parkgoal_ratios);

	// left rim

	bbd_p = (BBDATA *)oi;
	mat_setrotz_prec (0.0f, bendm);

	m[0]  = m[5] = m[10] = 1.0f;
	m[1]  = m[2] = m[4] = m[6] = m[8] = m[9] = 0.0f;
	m[3]  =  RIM_ROTATE_X;
	m[7]  = -RIM_ROTATE_Y;
	m[11] = -RIM_Z;

	m2[3]  = -RIM_ROTATE_X + RIM_CX + 45;
	m2[7]  =  RIM_ROTATE_Y - RIM_Y * 0.3f;
	m2[11] =         RIM_Z - RIM_Z;
	rotz ((int) bbd_p->rimbend >> 6, m2);
	mxm (m2, m, m3);

	bendm[3]  = -cambot.x - RIM_CX - 45;
	bendm[7]  = -cambot.y + RIM_Y * 0.3f;
	bendm[11] = -cambot.z + RIM_Z;
	mxm (bendm, m3, m);
	mxm (cambot.xform, m, cm);

	set_limb_mode(parkrim_limb_draw_info);
	draw_world_object (parkrim_limb_draw_info, 1,
			parkgoal_decals, parkgoal_ratios );

	// Animate & draw left net
	if (PLAYBACK)
	{
		// In a replay; save the current net & set the recorded one
		pvtx_tmp = limb_Net.pvtx;
		limb_Net.pvtx = ((replay_goal_t *)bbd_p)->pvtx;
#ifdef JONHEY
		bbox_tmp = limb_Net.bbox;
		limb_Net.bbox = ((replay_goal_t *)bbd_p)->bbox;
#endif
	}
	else if (bbd_p->netv_p)
	{
		if ((v_p = *bbd_p->netv_p))
		{
			limb_Net.pvtx = v_p;
#ifdef JONHEY
			limb_Net.bbox = *bbd_p->netv_bbox;
#endif
#ifdef	DEBUG
			if (!halt)
			{
#endif
			f  = bbd_p->netvposf;
			f += bbd_p->netvstep;
			while (f >= 1)					// Frac >= 1? Step ahead X frames
			{
				f -= 1;
				bbd_p->netv_p++;
#ifdef JONHEY
				bbd_p->netv_bbox++;
#endif
				bbd_p->netvpritm--;
				if (*bbd_p->netv_p == 0)	// End?
					f = 0;
			}
			bbd_p->netvposf = f;
#ifdef	DEBUG
	 		}
#endif
		}
		else
		{
			bbd_p->netv_p = 0;
#ifdef JONHEY
			bbd_p->netv_bbox = 0;
#endif
		}
	}

	m[3]  = 4.3425f;
	m[7]  = 0.0f;
	m[11] = 0.0f;
	roty (bbd_p->netya, m);

	m2[3]  = -4.3425f - RIM_CX + RIM_CX + 45;
	m2[7]  =     0.0f + RIM_CY - RIM_Y * 0.3f;
	m2[11] =     0.0f + RIM_CZ - RIM_Z;
	rotz ((int) bbd_p->rimbend >> 6, m2);
	mxm (m2, m, m3);

	bendm[3]  = -cambot.x - RIM_CX - 45;
	bendm[7]  = -cambot.y + RIM_Y * 0.3f;
	bendm[11] = -cambot.z + RIM_Z;
	mxm (bendm, m3, m);
	mxm (cambot.xform, m, cm);

	set_limb_mode(parknet_limb_draw_info);
	convert_2_jonhey (&jh_obj, &limb_Net, cm, parknet_decals);
	jh_obj.mode = JONHEY_MODE_DBL_SIDED;
	draw_jh_object(&jh_obj);

	if (PLAYBACK)
	{
		// In a replay; restore the current net
		limb_Net.pvtx = pvtx_tmp;
#ifdef JONHEY
		limb_Net.bbox = bbox_tmp;
#endif
	}

	// right rim

	if (PLAYBACK)
		((replay_obj_t *)bbd_p)++;
	else
		((BBDATA *)bbd_p)++;

	mat_setrotz_prec (0.0f, bendm);

	m[0]  = m[5] = m[10] = 1.0f;
	m[1]  = m[2] = m[4] = m[6] = m[8] = m[9] = 0.0f;
	m[3]  = -RIM_ROTATE_X;
	m[7]  = -RIM_ROTATE_Y;
	m[11] = -RIM_Z;

	m2[3]  = RIM_ROTATE_X - RIM_CX - 45;
	m2[7]  = RIM_ROTATE_Y - RIM_Y * 0.3f;
	m2[11] =        RIM_Z - RIM_Z;
	rotz (-(int) bbd_p->rimbend >> 6, m2);
	mxm (m2, m, m3);

	bendm[3]  = -cambot.x + RIM_CX + 45;
	bendm[7]  = -cambot.y + RIM_Y * 0.3f;
	bendm[11] = -cambot.z + RIM_Z;
	mxm (bendm, m3, m);
	mxm (cambot.xform, m, cm);

	set_limb_mode(parkrim_limb_draw_info+1);
	draw_world_object (parkrim_limb_draw_info+1, 1,
			parkgoal_decals, parkgoal_ratios );

	// Animate & draw right net
	if (PLAYBACK)
	{
		// In a replay; save the current net & set the recorded one
		pvtx_tmp = limb_Net2.pvtx;
		limb_Net2.pvtx = ((replay_goal_t *)bbd_p)->pvtx;
#ifdef JONHEY
		bbox_tmp = limb_Net2.bbox;
		limb_Net2.bbox = ((replay_goal_t *)bbd_p)->bbox;
#endif
	}
	else if (bbd_p->netv_p)
	{
		if ((v_p = *bbd_p->netv_p))
		{
			limb_Net2.pvtx = v_p;
#ifdef JONHEY
			limb_Net2.bbox = *bbd_p->netv_bbox;
#endif
#ifdef	DEBUG
			if (!halt)
			{
#endif
			f  = bbd_p->netvposf;
			f += bbd_p->netvstep;
			while (f >= 1)					// Frac >= 1? Step ahead X frames
			{
				f -= 1;
				bbd_p->netv_p++;
#ifdef JONHEY
				bbd_p->netv_bbox++;
#endif
				bbd_p->netvpritm--;
				if (*bbd_p->netv_p == 0)	// End?
					f = 0;
			}
			bbd_p->netvposf = f;
#ifdef	DEBUG
	 		}
#endif
		}
		else 
		{
			bbd_p->netv_p = 0;
#ifdef JONHEY
			bbd_p->netv_bbox = 0;
#endif
		}
	}

	m[3]  = -4.3425f;
	m[7]  = 0.0f;
	m[11] = 0.0f;
	roty (bbd_p->netya, m);

	m2[3]  = 4.3425f + RIM_CX - RIM_CX - 45;
	m2[7]  =    0.0f + RIM_CY - RIM_Y * 0.3f;
	m2[11] =    0.0f + RIM_CZ - RIM_Z;
	rotz (-(int) bbd_p->rimbend >> 6, m2);
	mxm (m2, m, m3);

	bendm[3]  = -cambot.x + RIM_CX + 45;
	bendm[7]  = -cambot.y + RIM_Y * 0.3f;
	bendm[11] = -cambot.z + RIM_Z;
	mxm (bendm, m3, m);
	mxm (cambot.xform, m, cm);

	set_limb_mode(parknet_limb_draw_info+1);
	convert_2_jonhey (&jh_obj, &limb_Net2, cm, parknet_decals);
	jh_obj.mode = JONHEY_MODE_DBL_SIDED;
	draw_jh_object(&jh_obj);

	if (PLAYBACK)
	{
		// In a replay; restore the current net
		limb_Net2.pvtx = pvtx_tmp;
#ifdef JONHEY
		limb_Net2.bbox = bbox_tmp;
#endif
	}

	xlate[0] = xlate[5] = xlate[10] = 1.0f;
	xlate[1] = xlate[2] = xlate[3] = xlate[4] =
	xlate[6] = xlate[7] = xlate[8] = xlate[9] = xlate[11] = 0.0f;

	xlate[3] = -cambot.x;
	xlate[7] = -cambot.y;
	xlate[11] = -cambot.z;

	mxm(cambot.xform, xlate, cm);

	// three possibilities for draw order
	// side, left to right, right to left
	if (cambot.csi.mode != CAM_END)
	{	// side
		grConstantColorValue (255 << 24);
		gilmore_left_pole();
		gilmore_right_pole();
		gilmore_left_bbd();
		gilmore_right_bbd();
	}
	else if (end_side)
	{	// left side is distant
		grConstantColorValue (255 << 24);
		gilmore_left_pole();
		gilmore_left_bbd();
		grConstantColorValue (65 << 24);
		gilmore_right_bbd();
		gilmore_right_pole();
	}
	else
	{	// right side is distant
		grConstantColorValue (255 << 24);
		gilmore_right_pole();
		gilmore_right_bbd();
		grConstantColorValue (65 << 24);
		gilmore_left_bbd();
		gilmore_left_pole();
	}

	grConstantColorValue (255 << 24);

	// voodoo
	set_limb_mode( parkflr_limb_draw_info + 0 );
	return;
}

/*************************************/
static void gilmore_left_pole( void )
{
	// left pole
	draw_world_object(parkpole_limb_draw_info+0, 1,
		parkgoal_decals, parkgoal_ratios);
}

/*************************************/
static void gilmore_right_pole( void )
{
	// right pole
	draw_world_object(parkpole_limb_draw_info+1, 1,
		parkgoal_decals, parkgoal_ratios);
}

/*************************************/
static void gilmore_left_bbd( void )
{
	// left backboard
	draw_world_object(parkbbd_limb_draw_info+0, 1,
			parkgoal_decals, parkgoal_ratios);
}

/*************************************/
static void gilmore_right_bbd( void )
{
	// right backboard
	draw_world_object(parkbbd_limb_draw_info+1, 1,
			parkgoal_decals, parkgoal_ratios);
}

/*****************************************************************************/

 
/******************************************************************************
* geer arena
******************************************************************************/
static limb_draw_info_t beachcrt_limb_draw_info[] =
{
	NONALPHA_LIMB(&limb_beachcrt),
};

static limb_draw_info_t beachsea_limb_draw_info[] =
{
	ALPHAADD_LIMB(&limb_sea),
};

static limb_draw_info_t beachtrees_limb_draw_info[] =
{
	ALPHA_LIMB_GR(&limb_trees,64),
};

static limb_draw_info_t bch_stand_limb_draw_info[] =
{
	ALPHA_LIMB_CCA(&limb_b_stand1),
	ALPHA_LIMB_CCA(&limb_b_stand2),
};

static limb_draw_info_t bch_bkwod_limb_draw_info[] =
{
	ALPHA_LIMB_CCA(&limb_b_bkwod1),
	ALPHA_LIMB_CCA(&limb_b_bkwod2),
};

static limb_draw_info_t bch_bkbmb_limb_draw_info[] =
{
	ALPHA_LIMB_CCA(&limb_b_bkbmb1),
	ALPHA_LIMB_CCA(&limb_b_bkbmb2),
};

static limb_draw_info_t bch_rim_limb_draw_info[] =
{
	NONALPHA_LIMB(&limb_b_rim1),
	NONALPHA_LIMB(&limb_b_rim2),
};

static limb_draw_info_t bch_gshad_limb_draw_info[] =
{
	ALPHA_LIMB_GR(&limb_b_shadw1,0),
	ALPHA_LIMB_GR(&limb_b_shadw2,0),
};

static limb_draw_info_t beachsky_limb_draw_info[] =
{
	NONALPHA_LIMB(&limb_b_sky),
};

static char *water_textures[] =
{
	"wave01.wms",
	"wave02.wms",
	"wave03.wms",
	"wave04.wms",
	"wave05.wms",
	"wave06.wms",
	"wave07.wms",
	"wave08.wms",
	"wave09.wms",
	"wave10.wms",
	"wave11.wms",
	"wave12.wms",
	"wave13.wms",
	"wave14.wms",
	"wave15.wms",
	"wave16.wms",
	"wave17.wms",
	"wave18.wms",
	NULL
};

static char *bamboo2_textures[] =
{
	"bamboo2.wms",
	NULL
};

/*****************************************************************************/
#define GEER_GOAL_STANDARD	0
#define GEER_GOAL_BAMBOO	1
#define GEER_GOAL_PEACOCK	2

#define GEER_RIM_RED		0
#define GEER_RIM_BAMBOO		1

static void geer_arena( void )
{
	int			i;

	// pick the backboards to use
	switch (randrng(20))
	{
		case 0:
			geer_goals = GEER_GOAL_BAMBOO;
			break;
		case 1:
			geer_goals = GEER_GOAL_PEACOCK;
			break;
		default:
			geer_goals = GEER_GOAL_STANDARD;
			break;
	}

	if (randrng(10))
		geer_rims = GEER_RIM_RED;
	else
		geer_rims = GEER_RIM_BAMBOO;

	set_bgnd_color(0xFF311A81);

	init_limb_draw_info(beachcrt_limb_draw_info, sizeof(beachcrt_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(beachsea_limb_draw_info, sizeof(beachsea_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(beachtrees_limb_draw_info, sizeof(beachtrees_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(parknet_limb_draw_info, sizeof(parknet_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(bch_stand_limb_draw_info, sizeof(bch_stand_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(bch_bkwod_limb_draw_info, sizeof(bch_bkwod_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(bch_bkbmb_limb_draw_info, sizeof(bch_bkbmb_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(bch_rim_limb_draw_info, sizeof(bch_rim_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(bch_gshad_limb_draw_info, sizeof(bch_gshad_limb_draw_info)/sizeof(limb_draw_info_t));
	init_limb_draw_info(beachsky_limb_draw_info, sizeof(beachsky_limb_draw_info)/sizeof(limb_draw_info_t));

	arena_texture_buffer_index = 0;

	get_buffer(&beachcrt_decals, &beachcrt_ratios, tex_count(beachcrt_textures));
	get_buffer(&beachsea_decals, &beachsea_ratios, tex_count(sea_textures));
	get_buffer(&beachtrees_decals, &beachtrees_ratios, tex_count(trees_textures));
	get_buffer(&water_decals, &water_ratios, tex_count(water_textures));
	get_buffer(&parknet_decals, &parknet_ratios, tex_count(parknet_textures));
	get_buffer(&bamboo_decals, &bamboo_ratios, tex_count(b_stand1_textures));
	get_buffer(&bchshad_decals, &bchshad_ratios, tex_count(b_shadw1_textures));
	get_buffer(&parkgoal_decals, &parkgoal_ratios, tex_count(parkgoal_textures));
	get_buffer(&beachsky_decals, &beachsky_ratios, tex_count(b_sky_textures));

	load_textures(beachcrt_textures, beachcrt_ratios, beachcrt_decals, tex_count(beachcrt_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
	load_textures(sea_textures, beachsea_ratios, beachsea_decals, tex_count(sea_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
	load_textures(trees_textures, beachtrees_ratios, beachtrees_decals, tex_count(trees_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
	load_textures(water_textures, water_ratios, water_decals, tex_count(water_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
	load_textures(parknet_textures, parknet_ratios, parknet_decals, tex_count(parknet_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
	load_textures(b_shadw1_textures, bchshad_ratios, bchshad_decals, tex_count(b_shadw1_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
	load_textures(parkgoal_textures, parkgoal_ratios, parkgoal_decals, tex_count(parkgoal_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
	load_textures(b_sky_textures, beachsky_ratios, beachsky_decals, tex_count(b_sky_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);

	// if peacock, get bamboo2.wms instead of bamboo1.wms
	if (geer_goals == GEER_GOAL_PEACOCK)
	{
		load_textures(bamboo2_textures, bamboo_ratios, bamboo_decals, tex_count(bamboo2_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
	}
	else
	{
		load_textures(b_stand1_textures, bamboo_ratios, bamboo_decals, tex_count(b_stand1_textures),
			TXID_COURT,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
	}

	// clamp beachcrt textures
	for(i = 0; beachcrt_textures[i]; i++)
	{
		guTexChangeAttributes( beachcrt_decals[i], -1, -1, -1, -1, -1, -1,
				-1, GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP, -1, -1);
	}
	
	// clamp water textures
	for(i = 0; water_textures[i]; i++)
	{
		guTexChangeAttributes( water_decals[i], -1, -1, -1, -1, -1, -1,
				-1, GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP, -1, -1);
	}
	
}

/*****************************************************************************/
static void draw_geer_court( void *oi )
{
	static int		dome_angle = 0;
	float			m1[12];

	xlate[0] = xlate[5] = xlate[10] = 1.0f;
	xlate[1] = xlate[2] = xlate[3] = xlate[4] =
	xlate[6] = xlate[7] = xlate[8] = xlate[9] = xlate[11] = 0.0f;

	xlate[3] = -cambot.x;
	xlate[7] = -cambot.y;
	xlate[11] = -cambot.z;

	mxm(cambot.xform, xlate, cm);

	roty1616(dome_angle, xlate);
	dome_angle += DR_INC;

	dome_angle = dome_angle % 0x04000000;

	mxm( cambot.xform, xlate, m1 );

	reset_limb_mode = TRUE;

	set_limb_mode(beachsky_limb_draw_info);
	grConstantColorValue (255 << 24);

	render_limb(&limb_b_sky, m1, beachsky_decals, beachsky_ratios );

	draw_world_object(beachcrt_limb_draw_info,
			sizeof(beachcrt_limb_draw_info)/sizeof(limb_draw_info_t),
			beachcrt_decals, beachcrt_ratios);
	set_limb_mode( bch_gshad_limb_draw_info + 0 );
	draw_world_object(bch_gshad_limb_draw_info,
			sizeof(bch_gshad_limb_draw_info)/sizeof(limb_draw_info_t),
			bchshad_decals, bchshad_ratios);
	set_limb_mode( beachtrees_limb_draw_info + 0 );
	draw_world_object(beachtrees_limb_draw_info,
			sizeof(beachtrees_limb_draw_info)/sizeof(limb_draw_info_t),
			beachtrees_decals, beachtrees_ratios);
	draw_world_object(beachsea_limb_draw_info,
			sizeof(beachsea_limb_draw_info)/sizeof(limb_draw_info_t),
			beachsea_decals, beachsea_ratios);

	return;
}

/*****************************************************************************/
static void geer_left_pole( void );
static void geer_right_pole( void );
static void geer_left_bbd( void );
static void geer_right_bbd( void );

static void draw_geer_goals( void *oi )
{
	BBDATA	*bbd_p;
	VERTEX	*pvtx_tmp;
	VERTEX	*bbox_tmp;
	void	*v_p;
	float	bendm[12], m[12], m2[12], m3[12], f;
	jonhey_t	jh_obj;
	int		end_side = ((game_info.game_quarter & 2) >> 1) ^ game_info.home_team;
	static	int		water_anict = 0;

	// animate water textures
	beachsea_decals[0] = water_decals[(water_anict%(18*6))/6];

	water_anict++;

	// voodoo
	set_limb_mode( beachcrt_limb_draw_info + 0 );

	// left rim

	bbd_p = (BBDATA *)oi;
	mat_setrotz_prec (0.0f, bendm);

	m[0]  = m[5] = m[10] = 1.0f;
	m[1]  = m[2] = m[4] = m[6] = m[8] = m[9] = 0.0f;
	m[3]  =  RIM_ROTATE_X;
	m[7]  = -RIM_ROTATE_Y;
	m[11] = -RIM_Z;

	m2[3]  = -RIM_ROTATE_X + RIM_CX + 45;
	m2[7]  =  RIM_ROTATE_Y - RIM_Y * 0.3f;
	m2[11] =         RIM_Z - RIM_Z;
	rotz ((int) bbd_p->rimbend >> 6, m2);
	mxm (m2, m, m3);

	bendm[3]  = -cambot.x - RIM_CX - 45;
	bendm[7]  = -cambot.y + RIM_Y * 0.3f;
	bendm[11] = -cambot.z + RIM_Z;
	mxm (bendm, m3, m);
	mxm (cambot.xform, m, cm);

	if (geer_rims == GEER_RIM_RED)
	{
		set_limb_mode(parkrim_limb_draw_info+0);
		draw_world_object (parkrim_limb_draw_info, 1,
				parkgoal_decals, parkgoal_ratios );
	}
	else
	{
		set_limb_mode(bch_rim_limb_draw_info+0);
		draw_world_object (bch_rim_limb_draw_info, 1,
				bamboo_decals, bamboo_ratios );
	}

	// Animate & draw left net
	if (PLAYBACK)
	{
		// In a replay; save the current net & set the recorded one
		pvtx_tmp = limb_Net.pvtx;
		limb_Net.pvtx = ((replay_goal_t *)bbd_p)->pvtx;
#ifdef JONHEY
		bbox_tmp = limb_Net.bbox;
		limb_Net.bbox = ((replay_goal_t *)bbd_p)->bbox;
#endif
	}
	else if (bbd_p->netv_p)
	{
		if ((v_p = *bbd_p->netv_p))
		{
			limb_Net.pvtx = v_p;
#ifdef JONHEY
			limb_Net.bbox = *bbd_p->netv_bbox;
#endif
#ifdef	DEBUG
			if (!halt)
			{
#endif
			f  = bbd_p->netvposf;
			f += bbd_p->netvstep;
			while (f >= 1)					// Frac >= 1? Step ahead X frames
			{
				f -= 1;
				bbd_p->netv_p++;
#ifdef JONHEY
				bbd_p->netv_bbox++;
#endif
				bbd_p->netvpritm--;
				if (*bbd_p->netv_p == 0)	// End?
					f = 0;
			}
			bbd_p->netvposf = f;
#ifdef	DEBUG
	 		}
#endif
		}
		else
		{
			bbd_p->netv_p = 0;
#ifdef JONHEY
			bbd_p->netv_bbox = 0;
#endif
		}
	}

	m[3]  = 4.3425f;
	m[7]  = 0.0f;
	m[11] = 0.0f;
	roty (bbd_p->netya, m);

	m2[3]  = -4.3425f - RIM_CX + RIM_CX + 45;
	m2[7]  =     0.0f + RIM_CY - RIM_Y * 0.3f;
	m2[11] =     0.0f + RIM_CZ - RIM_Z;
	rotz ((int) bbd_p->rimbend >> 6, m2);
	mxm (m2, m, m3);

	bendm[3]  = -cambot.x - RIM_CX - 45;
	bendm[7]  = -cambot.y + RIM_Y * 0.3f;
	bendm[11] = -cambot.z + RIM_Z;
	mxm (bendm, m3, m);
	mxm (cambot.xform, m, cm);

	set_limb_mode(parknet_limb_draw_info);
	convert_2_jonhey (&jh_obj, &limb_Net, cm, parknet_decals);
	jh_obj.mode = JONHEY_MODE_DBL_SIDED;
	draw_jh_object(&jh_obj);

	if (PLAYBACK)
	{
		// In a replay; restore the current net
		limb_Net.pvtx = pvtx_tmp;
#ifdef JONHEY
		limb_Net.bbox = bbox_tmp;
#endif
	}

	// right rim

	if (PLAYBACK)
		((replay_obj_t *)bbd_p)++;
	else
		((BBDATA *)bbd_p)++;

	mat_setrotz_prec (0.0f, bendm);

	m[0]  = m[5] = m[10] = 1.0f;
	m[1]  = m[2] = m[4] = m[6] = m[8] = m[9] = 0.0f;
	m[3]  = -RIM_ROTATE_X;
	m[7]  = -RIM_ROTATE_Y;
	m[11] = -RIM_Z;

	m2[3]  = RIM_ROTATE_X - RIM_CX - 45;
	m2[7]  = RIM_ROTATE_Y - RIM_Y * 0.3f;
	m2[11] =        RIM_Z - RIM_Z;
	rotz (-(int) bbd_p->rimbend >> 6, m2);
	mxm (m2, m, m3);

	bendm[3]  = -cambot.x + RIM_CX + 45;
	bendm[7]  = -cambot.y + RIM_Y * 0.3f;
	bendm[11] = -cambot.z + RIM_Z;
	mxm (bendm, m3, m);
	mxm (cambot.xform, m, cm);

	if (geer_rims == GEER_RIM_RED)
	{
		set_limb_mode(parkrim_limb_draw_info+1);
		draw_world_object (parkrim_limb_draw_info+1, 1,
				parkgoal_decals, parkgoal_ratios );
	}
	else
	{
		set_limb_mode(bch_rim_limb_draw_info+1);
		draw_world_object (bch_rim_limb_draw_info+1, 1,
				bamboo_decals, bamboo_ratios );
	}

	// Animate & draw right net
	if (PLAYBACK)
	{
		// In a replay; save the current net & set the recorded one
		pvtx_tmp = limb_Net2.pvtx;
		limb_Net2.pvtx = ((replay_goal_t *)bbd_p)->pvtx;
#ifdef JONHEY
		bbox_tmp = limb_Net2.bbox;
		limb_Net2.bbox = ((replay_goal_t *)bbd_p)->bbox;
#endif
	}
	else if (bbd_p->netv_p)
	{
		if ((v_p = *bbd_p->netv_p))
		{
			limb_Net2.pvtx = v_p;
#ifdef JONHEY
			limb_Net2.bbox = *bbd_p->netv_bbox;
#endif
#ifdef	DEBUG
			if (!halt)
			{
#endif
			f  = bbd_p->netvposf;
			f += bbd_p->netvstep;
			while (f >= 1)					// Frac >= 1? Step ahead X frames
			{
				f -= 1;
				bbd_p->netv_p++;
#ifdef JONHEY
				bbd_p->netv_bbox++;
#endif
				bbd_p->netvpritm--;
				if (*bbd_p->netv_p == 0)	// End?
					f = 0;
			}
			bbd_p->netvposf = f;
#ifdef	DEBUG
	 		}
#endif
		}
		else 
		{
			bbd_p->netv_p = 0;
#ifdef JONHEY
			bbd_p->netv_bbox = 0;
#endif
		}
	}

	m[3]  = -4.3425f;
	m[7]  = 0.0f;
	m[11] = 0.0f;
	roty (bbd_p->netya, m);

	m2[3]  = 4.3425f + RIM_CX - RIM_CX - 45;
	m2[7]  =    0.0f + RIM_CY - RIM_Y * 0.3f;
	m2[11] =    0.0f + RIM_CZ - RIM_Z;
	rotz (-(int) bbd_p->rimbend >> 6, m2);
	mxm (m2, m, m3);

	bendm[3]  = -cambot.x + RIM_CX + 45;
	bendm[7]  = -cambot.y + RIM_Y * 0.3f;
	bendm[11] = -cambot.z + RIM_Z;
	mxm (bendm, m3, m);
	mxm (cambot.xform, m, cm);

	set_limb_mode(parknet_limb_draw_info+1);
	convert_2_jonhey (&jh_obj, &limb_Net2, cm, parknet_decals);
	jh_obj.mode = JONHEY_MODE_DBL_SIDED;
	draw_jh_object(&jh_obj);

	if (PLAYBACK)
	{
		// In a replay; restore the current net
		limb_Net2.pvtx = pvtx_tmp;
#ifdef JONHEY
		limb_Net2.bbox = bbox_tmp;
#endif
	}

	xlate[0] = xlate[5] = xlate[10] = 1.0f;
	xlate[1] = xlate[2] = xlate[3] = xlate[4] =
	xlate[6] = xlate[7] = xlate[8] = xlate[9] = xlate[11] = 0.0f;

	xlate[3] = -cambot.x;
	xlate[7] = -cambot.y;
	xlate[11] = -cambot.z;

	mxm(cambot.xform, xlate, cm);

	// three possibilities for draw order
	// side, left to right, right to left
	if (cambot.csi.mode != CAM_END)
	{	// side
		grConstantColorValue (255 << 24);
		geer_left_pole();
		geer_right_pole();
		geer_left_bbd();
		geer_right_bbd();
	}
	else if (end_side)
	{	// left side is distant
		grConstantColorValue (255 << 24);
		geer_left_pole();
		geer_left_bbd();
		grConstantColorValue (65 << 24);
		geer_right_bbd();
		geer_right_pole();
	}
	else
	{	// right side is distant
		grConstantColorValue (255 << 24);
		geer_right_pole();
		geer_right_bbd();
		grConstantColorValue (65 << 24);
		geer_left_bbd();
		geer_left_pole();
	}

	grConstantColorValue (255 << 24);
	// voodoo
	set_limb_mode( beachcrt_limb_draw_info + 0 );
	return;
}

/*************************************/
static void geer_left_pole( void )
{
	// left pole
	draw_world_object(bch_stand_limb_draw_info+0, 1,
		bamboo_decals, bamboo_ratios);
}

/*************************************/
static void geer_right_pole( void )
{
	// right pole
	draw_world_object(bch_stand_limb_draw_info+1, 1,
		bamboo_decals, bamboo_ratios);
}

/*************************************/
static void geer_left_bbd( void )
{
	// left backboard
	if (geer_goals == GEER_GOAL_BAMBOO)
	{
		draw_world_object(bch_bkbmb_limb_draw_info+0, 1,
			bamboo_decals, bamboo_ratios);
	}
	else
	{
		draw_world_object(bch_bkwod_limb_draw_info+0, 1,
			bamboo_decals, bamboo_ratios);
	}
}

/*************************************/
static void geer_right_bbd( void )
{
	// right backboard
	if (geer_goals == GEER_GOAL_BAMBOO)
	{
		draw_world_object(bch_bkbmb_limb_draw_info+1, 1,
			bamboo_decals, bamboo_ratios);
	}
	else
	{
		draw_world_object(bch_bkwod_limb_draw_info+1, 1,
			bamboo_decals, bamboo_ratios);
	}
}

/*****************************************************************************/

void arena_fog_adjustment(int fog_on)
{
	int i;
	
	if (fog_on) {
		if (which_court == COURT_GILMORE) {
			for (i = 0; i < sizeof(parkenv_limb_draw_info) / sizeof(parkenv_limb_draw_info[0]); i++) {
				if (parkenv_limb_draw_info[i].alpha_test_function == GR_CMP_GREATER)
					parkenv_limb_draw_info[i].alpha_test_value = 225;
			}
		} else if (which_court == COURT_GEER) {
		}
	} else {
		if (which_court == COURT_GILMORE) {
			for (i = 0; i < sizeof(parkenv_limb_draw_info) / sizeof(parkenv_limb_draw_info[0]); i++) {
				if (parkenv_limb_draw_info[i].alpha_test_function == GR_CMP_GREATER)
					parkenv_limb_draw_info[i].alpha_test_value = 64;
			}
		} else if (which_court == COURT_GEER) {
		}
	}
}	/* arena_fog_adjustment */

//////////////////////////////////////////////////////////////////////////////
static void roty1616( int t, float *m )
{
	float ic, is, ft;

	// sin_rad and cos_rad are valid on the range -PI to +PI.
	// The t we get is from 0 to 2PI.  Get it in range.
	if (t > 0x02000000)
		t -= 0x04000000;

	ft = (float)t / (float)(0x04000000);
	ft *= (2.0f * 3.14159f);
	
	ic = cos_rad(ft);
	is = sin_rad(ft);
	
	m[0] = ic;
	m[1] = 0.0f;
	m[2] = is;

	m[4] = 0.0f;
	m[5] = 1.0f;
	m[6] = 0.0f;

	m[8] = -is;
	m[9] = 0.0f;
	m[10] = ic;
}
//////////////////////////////////////////////////////////////////////////////

