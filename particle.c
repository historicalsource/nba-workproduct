/****************************************************************
*		$Archive: /video/Nba/particle.c $
*		$Revision: 44 $
*		$Date: 4/20/99 11:01p $
*
*		Copyright (c) 1998 Midway Games Inc.
*		All Rights Reserved
*
*		This file is confidential and a trade secret of Midway Games Inc.
*		Use, reproduction, adaptation, distribution, performance or
*		display of this computer program or the associated audiovisual work
*		is strictly forbidden unless approved in writing by Midway Games Inc.
*
*		Started 10/25/98 by Shawn Liptak
*
*.Last mod - 3/22/99 17:00
****************************************************************/

#ifdef INCLUDE_SSID
char *ss_particle_c = "$Workfile: particle.c $ $Revision: 44 $";
#endif


#ifdef	DEBUG
#define	DEBUG_PTL
//#define	DEBUG_PTLINFO	// Needs DEBUG_PTL
#endif

#ifdef	DEBUG_PTL
#define	PRINTFDB	printf
#define	HALT	halt
#define	PHALT	phalt
#else
#define	PRINTFDB	//
#define	HALT	0
#define	PHALT	0
#endif

/****************************************************************/
// SYSTEM INCLUDES

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <goose/goose.h>
#if defined(VEGAS)
#include <glide.h>
#else
#include <glide/glide.h>
#endif

/****************************************************************/
// USER INCLUDES

#include "ani3d/clip.h"

#include "include/game.h"
#include "include/mainproc.h"
#include "include/ani3d.h"
#include "include/id.h"
#include "include/externs.h"
#include "include/fontid.h"
#include "include/particle.h"

#define IMAGEDEFINE
#include "include/ptlimg.h"

/****************************************************************/
// Defines

typedef	void	*PV;
typedef	struct object_node	OBJN;

#define	PTLDB_STRID	0x7010



//#define GEOM_STATS 1
typedef	struct stats
{
	int		tris_processed;
	int		tris_bbox_processed;
	int		tris_drawn;
	int		verts_processed;
	int		verts_bbox_processed;
	int		verts_transformed;
	int		verts_drawn;
	int		bbox_verts;
	int		bbox_verts_transformed;
	int		tris_clip_processed;
	int		tris_clipped;
	int		strips_processed;
	int		strips_bbox_processed;
	int		strips_drawn;
} stats_t;

extern stats_t		stats;



//****************************************************************
// Global data & function prototypes


#if	0
__asm__("
	.set	noreorder
	.globl	___clr_count
___clr_count:
	jr	$31
	mtc0	$0,$9

	.globl	___get_count
___get_count:
	mfc0	$2,$9
	nop
	nop
	jr	$31
	nop
	.set	reorder
");
#endif

#ifdef	DEBUG_PTLINFO
int	___get_count();
#define	GET_TIME(A)	(A) = (___get_count() * 13)
#endif

int	rndrng0 (int max);

extern float	trigs[];
extern VIEWCOORD cambot;
extern ReducedVertex	tmp_grvx[];

extern texture_node_t	*smkpuf_decals[];


//***************************************************************
// Local data & function prototypes

typedef	struct	_VEC {
	float		x;
	float		y;
	float		z;
} VEC;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

static
void	ptl_xdraw (PTL *pt_p, float oow);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

static	PTL	*ptlmem_p;		// * mem for particles
static	PTL	*ptllst_p;			// * 1st active particle or 0
static	PTL	*ptlfree_p;			// * 1st free particle or 0
static int vres_var = 384;
static float vresf_var = 384.0F;
static float y_scale_factor = 1.0f;

#ifdef	DEBUG_PTL
PTL	*dbpt_p;				// * debug particle
#endif

typedef	struct	_PTLTEX {	// Particle texture
	char		*fn_p;			// * filename or 0 for end
	texture_node_t *txn_p;	// * texture or 0 if not loaded
} PTLTEX;

PTLTEX	ptltex_t[] = {
	{ "ptl.wms", 0 },
	{ 0 }
};

image_info_t	*ptlx1_t[] = {
	&xpld1_01,
	&xpld1_02,
	&xpld1_03,
	&xpld1_04,
	&xpld1_05,
	&xpld1_06,
	&xpld1_07,
	&xpld1_08,
	&xpld1_09,
	&xpld1_10,
	&xpld1_11,
	&xpld1_12,
	&xpld1_13,
	&xpld1_14,
	&xpld1_15,
	&xpld1_16,
	&xpld1_17,
	&xpld1_18,
	&xpld1_19,
	&xpld1_20,
	&xpld1_21,
	0
};
image_info_t	*ptlxa_t[] = {
	&xplda_01,
	&xplda_02,
	&xplda_03,
	&xplda_04,
	&xplda_05,
	&xplda_06,
	&xplda_07,
	&xplda_08,
	&xplda_09,
	&xplda_10,
	0
};
image_info_t	*ptlxb_t[] = {
	&xpldb_01,
	&xpldb_02,
	&xpldb_03,
	&xpldb_04,
	&xpldb_05,
	&xpldb_06,
	&xpldb_07,
	&xpldb_08,
	&xpldb_09,
	&xpldb_10,
	0
};
image_info_t	*ptlxc_t[] = {
	&xpldc_01,
	&xpldc_02,
	&xpldc_03,
	&xpldc_04,
	&xpldc_05,
	&xpldc_06,
	&xpldc_07,
	&xpldc_08,
	&xpldc_09,
	&xpldc_10,
	0
};
image_info_t	*ptlflsha_t[] = {
	&pflasha01,
	&pflasha02,
	&pflasha03,
	&pflasha04,
	&pflasha05,
	&pflasha06,
	&pflasha07,
	&pflasha08,
	&pflasha09,
	&pflasha10,
	0
};
image_info_t	*ptlsprka_t[] = {
	&sparka01,
	&sparka02,
	&sparka03,
	&sparka04,
	&sparka05,
	&sparka06,
	&sparka07,
	&sparka08,
	&sparka09,
	&sparka10,
	0
};
image_info_t	*ptlballblur_t[] = {
	&ballblur,
	0
};
image_info_t	*ptlraindrop_t[] = {
	&raindrop,
	0
};
image_info_t	*ptlsnow_t[] = {
	&snow,
	0
};

PTLIMG	ptlimg_t[] = {
	{ 0, 0, ptlx1_t },
	{ 0, 0, ptlxa_t },
	{ 0, 0, ptlxb_t },
	{ 0, 0, ptlxc_t },
	{ 0, 0, ptlflsha_t },
	{ 0, 0, ptlsprka_t },
	{ 0, 0, ptlballblur_t },
	{ 0, 0, ptlraindrop_t },
	{ 0, 0, ptlsnow_t },
	{ -1 }
};

//LIMB		ptllimb;
//TRI		ptlt_t[3];
//VERTEX	ptlv_t[4];
ST			ptlst_t[3];

float		rat_t[] = {
	256,	32,
	256,	64,
	256,	128,
	256,	256,
	128,	256,
	64,	256,
	32,	256
};

/****************************************************************/

/********************************
 Init particle system
********************************/

void	ptl_init()
{
	PTL		*pt_p;
	texture_node_t	*txn_p;

	int		n, i;
	char		*c_p;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	ptllst_p = 0;
	ptlfree_p = 0;

	if (is_low_res) {
		vres_var = 256;
		vresf_var = 256.0F;
		y_scale_factor = 0.6666f;
	} else {
		vres_var = 384;
		vresf_var = 384.0F;
		y_scale_factor = 1.0f;
	}
	
	if (ptlmem_p == 0) {
		ptlmem_p = malloc (sizeof(PTL) * PTLMAX);
#ifdef	DEBUG
		printf ("ptl_init: mallocing space\n");
#endif

		if (ptlmem_p == 0) {	// Error?
#ifdef	DEBUG
			printf ("ptl_init: no mem\n");
#endif
			return;
		}
	}

	pt_p = ptlmem_p;
	ptlfree_p = pt_p;		// 1st on free list

	for (i = 0; i < PTLMAX - 1; i++) {	// Link mem
		pt_p->nxt_p = pt_p + 1;
		pt_p++;
	}
	pt_p->nxt_p = 0;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Load textures

	n = 0;
	while ((c_p = ptltex_t[n].fn_p)) {

		txn_p = create_texture ("ptl.wms", SPFX_TID, 0, CREATE_NORMAL_TEXTURE,
				GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
				GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);

		ptltex_t[n].txn_p = txn_p;

		if (!txn_p) {
			ptlfree_p = 0;	// Prevent particle usage
#ifdef DEBUG
			PRINTFDB ("ptl_init: create tex failed\n");
#endif
			return;
		}

#ifdef DEBUG
		printf ("ptl_init: %s load OK (%x)\n", c_p, (uint) txn_p);
#endif

		n++;
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Assign each image a texture node

	n = 0;
	while ((i = ptlimg_t[n].txnum) >= 0) {

		ptlimg_t[n].txn_p = ptltex_t[i].txn_p;
		n++;
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	create_object ("ptl", OID_MISC, OF_NONE, DRAWORDER_PTL, NULL, ptl_draw);
//	printf("created ptl object\n");

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

//	lm_p = &ptllimb;
//
//	lm_p->nvtx = 4;
//	lm_p->ntris = 2;
//	lm_p->pvn = 0;
//	lm_p->pvtx = ptlv_t;
//	lm_p->pst = ptlst_t;
//	lm_p->ptri = ptlt_t;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Test

#ifdef	DEBUG_PTL

	i = 0;
	while (i-- > 0) {

		if ((pt_p = ptl_get(NULL))) {

			dbpt_p = pt_p;		// Save
			pt_p->flgs = M_PTLF_TEX;
			pt_p->x = 0;
			pt_p->y = 0;
			pt_p->z = i * -.001f;
			pt_p->vx = 0;
			pt_p->vy = 0;
			pt_p->vz = 0;
			pt_p->ay = 0;
			pt_p->drag = 0;
			pt_p->fuel = 0x7fffffff;
			pt_p->rad = 15;
			pt_p->radv = 0;
			pt_p->txn_p = 0;
			pt_p->alpha = 240;
			pt_p->alphav = 0;
			pt_p->color = 0xffffff;

 			pt_p->img_p = PTLIMG(PTLI_BALLBLUR);

//			if (i & 1) {
//				pt_p->x = 0;
//	 			pt_p->img_p = PTLIMG(PTLI_X1);
//				pt_p->frm = 0;
//				pt_p->color = 0xff;
//			}
		}
	}
#endif

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

}

/********************************
 Shutdown particle system. Free mem
********************************/

void	ptl_shutdown()
{
	if (ptlmem_p) {
		free (ptlmem_p);
		ptlmem_p = 0;
	}
}


/********************************
 Get a free particle
> * PTL or 0 if none
********************************/

PTL	*ptl_get (PTLFUNC *func_p)
{
	PTL		*pt_p;

	pt_p = ptlfree_p;

	if (pt_p) {

		ptlfree_p = pt_p->nxt_p;	// Unlink 1st free

		pt_p->nxt_p = ptllst_p;		// Link to active
		ptllst_p = pt_p;

		pt_p->ay = BALL_GRAVITY;
		pt_p->func_p = func_p;
		pt_p->txn_p = 0;
 		pt_p->img_p = 0;
 		pt_p->frm = 0;

 		pt_p->flgs = (PLAYBACK) ? M_PTLF_PLAYBACK : 0;
	}

	return (pt_p);
}


/********************************
 Free an active particle
 (* PTL)
********************************/

void	ptl_free (PTL *p_p)
{
	PTL		*pt_p;
	PTL		*ptl_p;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	ptl_p = (PV) &ptllst_p;
	pt_p = ptl_p->nxt_p;
	if (!pt_p) return;

	do {
		if (p_p == pt_p) {

			ptl_p->nxt_p = p_p->nxt_p;	// Unlink

			p_p->nxt_p = ptlfree_p;		// Link to free
			ptlfree_p = p_p;
		}
		ptl_p = pt_p;

	} while ((pt_p = pt_p->nxt_p));

}

/****************************************************************/

typedef	struct	_PTLSRT {	// Particle
	PTL		*pt_p;
	float		z;
} PTLSRT;

PTLSRT	ptlsrt_t[PTLMAX + 1];

/********************************
 Update and draw particles
********************************/

#if	1

// 500 ptls
// 3690 On screen no sort
// 3940 On screen 0 swap sort
// 4450 On screen 2196 swap sort 5> 1lp
// 4280 On screen 2300 swap sort 5> 3lp
// 4320 On screen 2292 swap sort 6> 2lp
// 4335 On screen 2300 swap sort 5> 3lp img

// 1399 Off scrn 0 swap
// 1720 Off scrn 2316 swap
// 1833 Off scrn 2316 swap

#define	_HR	fovval
#define	_H	50.0f		// Off screen range to use clipper
#define	_V	50.0f
#ifdef	DEBUG_PTL
static int SRTSHFT = 5;
static int SRTMINLP = 3;
#else
#define	SRTSHFT	5
#define	SRTMINLP	3
#endif

void	ptl_draw()
{
	PTL		*pt_p;
	PTL		*pt2_p;
	PTLSRT	*psrt_p;
	PTLSRT	*psrt2_p;
	PTLSRT	*psrtend_p;

	VERTEX	vt;
	float		*m_p;
	ReducedVertex	*mv_p;
	float		oow;					// 1/W
//	register float oow asm("$f4");

	int		clmode;				// Color combine mode
	int		blenda;				// !0 = Additive blend

	texture_node_t	*txn_p;
	PTLIMG	*img_p;
#if defined(VEGAS)
	Texture_node_t	*decal;
#else
	GrMipMapId_t decal;
#endif
	int				ratio;
	image_info_t	*imgi_p;

	ST			*st_p;
	float		sf,tf;
	ReducedVertex	*v1_p, *v2_p, *v3_p, *v4_p;

	void		*v_p;
	int		n, i, i2, i3, i4;
	float		f, f2, fx, fy, fz, fx0, fy0, fx1, fy1, fx2, fy2;
	float		fw, fh;

#ifdef	DEBUG_PTL
	int		tm;
	int		phalt;			// Local halt
	static int testv = 255;
	static int testv2 = 0x80;
//	GrState	grst;
#endif
	int		srtloops = 0;
	int		srtswaps = 0;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	pt_p = ptllst_p;
	if (!pt_p) return;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#if 0//def	DEBUG_PTL
{
	static int	dbinit;

	if (!dbinit) {

		v_p = smkpuf_decals[0];
		if (v_p) {
			dbinit = 1;
			do {
				if (!pt_p->img_p)
					pt_p->txn_p = v_p;
			} while ((pt_p = pt_p->nxt_p));
		}
		else return;
	}
}
	pt_p = ptllst_p;
#endif

#ifdef	DEBUG_PTL
	i = get_p5_sw_current();
	if (!(i & P_A)) {

#if	0
		i = get_p5_sw_close();
		if (i & P_LEFT) {
			SRTSHFT--;
			if (SRTSHFT < 1) SRTSHFT = 9;
		}
		if (i & P_RIGHT) {
			SRTSHFT++;
			if (SRTSHFT > 9) SRTSHFT = 1;
		}
		if (i & P_UP) {
			SRTMINLP++;
			if (SRTMINLP > 9) SRTMINLP = 1;
		}
		if (i & P_DOWN) {
			SRTMINLP--;
			if (SRTMINLP < 1) SRTMINLP = 9;
		}
#else
		i = get_p5_sw_current();
		if (i & P_LEFT) {
			testv--;
			if (testv < 0) testv = 0;
		}
		if (i & P_RIGHT) {
			testv++;
			if (testv > 255) testv = 255;
		}
		if (i & P_UP) {
			testv2++;
			if (testv2 > 255) testv2 = 255;
		}
		if (i & P_DOWN) {
			testv2--;
			if (testv2 < 0) testv2 = 0;
		}
//		if (i &	(P_LEFT|P_RIGHT|P_UP|P_DOWN))
//			printf ("PTL test %x %x\n", testv, testv2);

		if (dbpt_p) {
			i = testv << 16 | testv << 8 | testv;
			dbpt_p->color = i;
			dbpt_p->alpha = testv2;
		}
#endif
	}
#endif

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

//	grGlideGetState (&grst);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#if	0
//GrFog_t fog_t[GR_FOG_TABLE_SIZE];
GrFog_t fog_t[256];	//FIX!!!

	guFogGenerateLinear (fog_t, 1, 300);
//	printf ("Fog\n");
//	for (i = 0; i < 64; i+=4)
//		printf ("%d %d %d %d\n", fog_t[i], fog_t[i+1], fog_t[i+1], fog_t[i+2]);

	grFogTable (fog_t);
	grFogColorValue (0x404040);
	grFogMode (GR_FOG_WITH_TABLE);
//	grFogMode (GR_FOG_DISABLE);
#endif

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	// RGB = Constant color
	grColorCombine (GR_COMBINE_FUNCTION_LOCAL, GR_COMBINE_FACTOR_LOCAL,
			GR_COMBINE_LOCAL_CONSTANT, GR_COMBINE_OTHER_TEXTURE, 0);
	clmode = 0;

	grChromakeyMode (GR_CHROMAKEY_DISABLE);

	guAlphaSource (GR_ALPHASOURCE_TEXTURE_ALPHA_TIMES_CONSTANT_ALPHA);
//	grAlphaCombine (GR_COMBINE_FUNCTION_BLEND_OTHER, GR_COMBINE_FACTOR_LOCAL_ALPHA,
//		GR_COMBINE_LOCAL_CONSTANT, GR_COMBINE_OTHER_TEXTURE, FXFALSE);

	grAlphaBlendFunction (GR_BLEND_SRC_ALPHA, GR_BLEND_ONE_MINUS_SRC_ALPHA,
			GR_BLEND_ONE, GR_BLEND_ZERO);
	blenda = 0;

	grAlphaTestFunction (GR_CMP_GEQUAL);
	grAlphaTestReferenceValue (0x10);

	grTexCombineFunction (0, GR_TEXTURECOMBINE_DECAL);
	grTexClampMode (0, GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP);

	grDepthBufferMode (GR_DEPTHBUFFER_WBUFFER);
	grDepthBufferFunction (GR_CMP_LEQUAL);
	grDepthMask (FXTRUE);
//	grDepthMask (FXFALSE);

//	grCullMode (GR_CULL_NEGATIVE);
	grCullMode (GR_CULL_DISABLE);

	ptlst_t[0].s = 0;
	ptlst_t[0].t = 0;
	ptlst_t[2].s = 1;
	ptlst_t[2].t = 1;

#ifdef	DEBUG_PTL
	if (get_p5_sw_current() & P_B)
		guAlphaSource (GR_ALPHASOURCE_TEXTURE_ALPHA);
	if (get_p5_sw_current() & P_C)
		guAlphaSource (GR_ALPHASOURCE_CC_ALPHA);
#endif

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Update

	psrt_p = ptlsrt_t;

#ifdef	DEBUG_PTLINFO
	GET_TIME(tm);
#endif
#ifdef	DEBUG_PTL
	if ((phalt = halt)) {
		i = get_player_sw_close();
		if (i & P4_C) phalt = 0;
	}
#endif

	do {
		// Use gas if replay state matches replay flag
		//  (and for debug, not halted while not in a replay)
		if ((!PLAYBACK && !(pt_p->flgs & M_PTLF_PLAYBACK) && (!PHALT)) ||
			( PLAYBACK &&  (pt_p->flgs & M_PTLF_PLAYBACK)))
			pt_p->fuel--;

		// Free ptl if no more gas or its not a replay & its a replay ptl
		if ((pt_p->fuel < 0) || (!PLAYBACK && (pt_p->flgs & M_PTLF_PLAYBACK))) {
			v_p = pt_p->nxt_p;
			ptl_free (pt_p);
			pt_p = v_p;
			continue;
		}

		// Skip ptl if replay state doesn't match replay flag
		if (PLAYBACK != (pt_p->flgs & M_PTLF_PLAYBACK))
			goto nxts;

		// At least position ptl in current camera space
		fx = pt_p->x;
		fy = pt_p->y;
		fz = pt_p->z;

		// Don't mess with ptl if halted
		if (!PHALT) {
			f = pt_p->drag;
			pt_p->vx *= f;
			pt_p->vy = pt_p->vy * f + pt_p->ay;
			pt_p->vz *= f;
			pt_p->x += pt_p->vx;
			pt_p->y += pt_p->vy;
			pt_p->z += pt_p->vz;

			if (pt_p->y < 0) {
				pt_p->y = 0;
				pt_p->vy *= -.8f;
			}
			f = pt_p->rad + pt_p->radv;
			if (f < 0.01f) f = .01f;
			pt_p->rad = f;

			f = pt_p->alpha + pt_p->alphav;
			if (f < 1) f = 1;
			pt_p->alpha = f;

			if (pt_p->func_p) (pt_p->func_p)(pt_p);
		}

	//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Transform center

		// Position ptl in current camera space
		m_p = cambot.xform;

		vt.x = fx - cambot.x;
		vt.y = fy - cambot.y;
		vt.z = fz - cambot.z;

		// Transform vertex into camera coordinates
		fx = vt.x * m_p[0] + vt.y * m_p[1] + vt.z * m_p[2];
		fy = (vt.x * m_p[4] + vt.y * m_p[5] + vt.z * m_p[6]) * y_scale_factor;
		fz = vt.x * m_p[8] + vt.y * m_p[9] + vt.z * m_p[10];

		fz -= pt_p->rad * .5f;	// Offset to give depth
		if (fz < 1) goto nxts;	// Right in front of or behind camera?

		oow = 1.0f / fz;
		f2 = oow * _HR;
		fx = -fx * f2;
		fy = fy * f2;

		if ((pt_p->flgs & M_PTLF_CLIPQ)) {
			if (fx < -HRES/2 - 2) goto nxts;
			if (fx > HRES/2 + 2) goto nxts;
			if (fy < -vres_var/2 - 2) goto nxts;
			if (fy > vres_var/2 + 2) goto nxts;
		}
		pt_p->tx = fx + (HRES/2);				// Save working values
		pt_p->ty = fy + (vres_var/2);
		psrt_p->pt_p = pt_p;
		psrt_p->z = fz;
		psrt_p++;

nxts:
		pt_p = pt_p->nxt_p;

	} while (pt_p);


	psrt_p->pt_p = 0;
	psrtend_p = psrt_p;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Sort

	n = psrtend_p - ptlsrt_t;

#ifdef	DEBUG_PTL
	if (get_p5_sw_current() & P_C) n = 0;
#endif

	if (n > 1) {

		i2 = n >> SRTSHFT;		// Pointer restart offset (5 shift is best)
		if (i2 < 1) i2 = 1;
		i4 = 0;

		do {

			i3 = 0;

			if (--i4 <= 0) {
				i4 = 1;
				n = n >> 1;				// Scale down offset
				if (n == 1) i4 = SRTMINLP;
				if (n < 1) n = i2;
			}
			psrt_p = ptlsrt_t;
			psrt2_p = ptlsrt_t + n;	// Add offset

			srtloops++;

			do {

				if (psrt_p->z < psrt2_p->z) {	// Swap? Hi Z goes 1st

					srtswaps++;

					i3 = 1;
					pt_p = psrt_p->pt_p;
					fz = psrt_p->z;
					psrt_p->pt_p = psrt2_p->pt_p;
					psrt_p->z = psrt2_p->z;
					psrt2_p->pt_p = pt_p;
					psrt2_p->z = fz;
				}
				psrt_p++;
				psrt2_p++;

			} while (psrt2_p < psrtend_p);

		} while (i3 || n > 1);
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Draw sorted

	psrt_p = ptlsrt_t;

	while ((pt_p = psrt_p->pt_p)) {

		// Skip ptl if replay state doesn't match replay flag
		if (PLAYBACK != (pt_p->flgs & M_PTLF_PLAYBACK))
			goto nxt;

		fx = pt_p->tx;
		fy = pt_p->ty;
		fz = psrt_p->z;

	//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Project

		oow = 1.0f / fz;
//		asm ("recip.s	%0,%1" : "=f" (oow) : "f" (fz) );

		f2 = oow * _HR;
//		fx = -fx * f2 + (HRES/2);	// clamp to mult of 0.25
//		fy = fy * f2 + (vres_var/2);


	//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Size & clip

		fw = 1;
		fh = 1;
		fx1 = .5f;
		fy1 = -.5f;

		imgi_p = 0;

		img_p = pt_p->img_p;
		if (img_p) {

			i = pt_p->frm;
			imgi_p = (img_p->imgi_p)[i];

			if (imgi_p) {
				fw = imgi_p->w * (1.0f / 32.0f);
				fh = imgi_p->h * (1.0f / 32.0f);
				fx1 = imgi_p->ax * (1.0f / 32.0f);
				fy1 = imgi_p->ay * (1.0f / 32.0f);
			}
#ifdef	DEBUG
//			else {
//				printf ("ptl_draw: NULL imgi_p");
//				lockup();
//			}
#endif
		}

		f = pt_p->rad * 2;
//		f = 1.0f;
		f *= f2;
		fw *= f;
		fh *= f * y_scale_factor;
		fx -= fx1 * f;
		fy += fy1 * f;
		fx0 = fx + fw;
		fy0 = fy;
		fy  = fy + fh;

		mv_p = &tmp_grvx[1];

		mv_p[0].x = fx;
		mv_p[0].y = fy;
		mv_p[0].oow = oow;

		if (fx >= HRESf) goto nxt;		// Off right of screen?

		mv_p[1].x = fx0;
		mv_p[1].y = fy;
		mv_p[1].oow = oow;

		if (fx0 <= 0.0f) goto nxt;		// Off left of screen?

		mv_p[2].x = fx0;
		mv_p[2].y = fy0;
		mv_p[2].oow = oow;

		if (fy <= 0.0f) goto nxt;		// Off bottom of screen?

		mv_p[3].x = fx;
		mv_p[3].y = fy0;
		mv_p[3].oow = oow;

		if (fy0 >= vresf_var) goto nxt;	// Off top of screen?

	//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Setup texture & draw

#if	0
		ptl_xdraw (pt_p, oow);
#else
		v1_p = mv_p;
		v2_p = mv_p + 1;
		v3_p = mv_p + 2;
		v4_p = mv_p + 3;
#if	1

		i = pt_p->flgs & M_PTLF_TEX;
		if (clmode != i) {
			clmode = i;

			if (i)	// RGB = Constant color * texture
				grColorCombine (GR_COMBINE_FUNCTION_SCALE_OTHER, GR_COMBINE_FACTOR_LOCAL,
						GR_COMBINE_LOCAL_CONSTANT, GR_COMBINE_OTHER_TEXTURE, 0);
			else		// RGB = Constant color
				grColorCombine (GR_COMBINE_FUNCTION_LOCAL, GR_COMBINE_FACTOR_LOCAL,
						GR_COMBINE_LOCAL_CONSTANT, GR_COMBINE_OTHER_TEXTURE, 0);
		}

		i = pt_p->flgs & M_PTLF_BLENDA;
		if (blenda != i) {
			blenda = i;

			if (i)	// Additive
				grAlphaBlendFunction (GR_BLEND_SRC_ALPHA, GR_BLEND_ONE,
						GR_BLEND_ONE, GR_BLEND_ZERO);
			else		// Normal
				grAlphaBlendFunction (GR_BLEND_SRC_ALPHA, GR_BLEND_ONE_MINUS_SRC_ALPHA,
						GR_BLEND_ONE, GR_BLEND_ZERO);
		}
#endif
		i = pt_p->color | (int) pt_p->alpha << 24;
		grConstantColorValue (i);

//		i = testv << 16 | testv << 8 | testv;
//		grConstantColorValue (testv2 << 24 | i);

		txn_p = pt_p->txn_p;
		if (!txn_p)
			txn_p = pt_p->img_p->txn_p;
		if (txn_p) {

			decal = txn_p->texture_handle;
			guTexSource (decal);
//			grTexLodBiasValue (GR_TMU0, -1.0f);

			i = txn_p->texture_info.header.aspect_ratio * 2;
			sf = rat_t[i];
			tf = rat_t[i + 1];

//			printf ("PTL %d %d\n", (int)decal, i);

			sf *= oow;
			tf *= oow;
		}
		else
		{
			sf = oow;
			tf = oow;
		}

		if (imgi_p) {
			fx1 = imgi_p->s_start * sf;
			fy1 = imgi_p->t_start * tf;
			fx2 = imgi_p->s_end * sf;
			fy2 = imgi_p->t_end * tf;
		}
		else {
			st_p = ptlst_t;
			fx1 = st_p[0].s * sf;		// Assumes rectangular ST
			fy1 = st_p[0].t * tf;
			fx2 = st_p[2].s * sf;
			fy2 = st_p[2].t * tf;
		}
		v1_p->sow = fx1;
		v1_p->tow = fy1;

		v2_p->sow = fx2;
		v2_p->tow = fy1;

		v3_p->sow = fx2;
		v3_p->tow = fy2;

		v4_p->sow = fx1;
		v4_p->tow = fy2;


	#ifdef GEOM_STATS
	stats.verts_processed += 4;
	stats.verts_bbox_processed += 4;
	stats.verts_transformed += 4;
	stats.tris_processed  += 2;
	stats.tris_bbox_processed  += 2;
	stats.strips_processed += 1;
	stats.strips_bbox_processed += 1;
	#endif



		if (fx > -_H &&				// Left edge mostly on screen?
			 fx0 < HRESf + _H &&	// Right edge mostly on screen?
			 fy0 > -_V &&			// Bottom edge mostly on screen?
			 fy < vresf_var + _V) {		// Top edge mostly on screen?

			grDrawSimpleTriangle (v1_p,v2_p,v4_p);

			#ifdef GEOM_STATS
			stats.tris_drawn  += 1;
			stats.verts_drawn += 3;
			#endif

			if (!(pt_p->flgs & M_PTLF_TRI))	{
				grDrawSimpleTriangle (v2_p,v3_p,v4_p);

				#ifdef GEOM_STATS
				stats.tris_drawn  += 1;
				stats.verts_drawn += 3;
				#endif
			}


		}
		else {

			clip_triangle_t (v1_p, v2_p, v4_p);

			#ifdef GEOM_STATS
			stats.tris_drawn  += 1;
			stats.verts_drawn += 3;
			stats.tris_clipped += 1;
			stats.tris_clip_processed += 1;
			#endif


			if (!(pt_p->flgs & M_PTLF_TRI)) {
				clip_triangle_t (v2_p, v3_p, v4_p);

				#ifdef GEOM_STATS
				stats.tris_drawn  += 1;
				stats.verts_drawn += 3;
				stats.tris_clipped += 1;
				stats.tris_clip_processed += 1;
				#endif
				}

		}
#endif

nxt:
		psrt_p++;
	}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#ifdef	DEBUG_PTLINFO
	{
	static int	ttm;

	GET_TIME(i);
	tm = i - tm;
	ttm = (ttm * 29 + tm) / 30;

	i = 0;	// Count total ptls
	if ((pt_p = ptllst_p)) {
		do {
			i++;
		} while ((pt_p = pt_p->nxt_p));
	}

	i2 = 0;	// Count sorted ptls
	psrt_p = ptlsrt_t;
	if ((pt_p = psrt_p->pt_p)) {
		do {
			i2++;
			psrt_p++;
		} while ((pt_p = psrt_p->pt_p));
	}
	delete_multiple_strings (PTLDB_STRID, 0xffffffff);
//	delete_string_id (PTLDB_STRID);
	set_string_id (PTLDB_STRID);
	set_text_font (FONTID_BAST8T);
	set_text_position (20, vresf_var-9, 1);

	tm >>= 10;
	//printf ("%4d\n", tm);
	oprintf ("%dj%dcPTL #%d srt #%d  %d %d", HJUST_LEFT, LT_YELLOW, i, i2, ttm >> 10, tm);
	set_text_position (20, vresf_var-18, 1);
	oprintf ("%dj%dcSorts %d Swaps %d shift %d mlp %d", HJUST_LEFT, LT_YELLOW,
			srtloops, srtswaps, SRTSHFT, SRTMINLP);
	}
#endif

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Reset modes

	grColorCombine (GR_COMBINE_FUNCTION_SCALE_OTHER, GR_COMBINE_FACTOR_ONE,
			GR_COMBINE_LOCAL_ITERATED, GR_COMBINE_OTHER_TEXTURE, 0);

	grConstantColorValue (0xffffffff);

}


#endif


/****************************************************************/
// OLD

/********************************
 Transform particle, clip & draw if neccessary
********************************/

#if	0

static
void	ptl_xdraw (PTL *pt_p, float oow)
{
	ReducedVertex	*mv_p;

	texture_node_t	*txn_p;
	GrMipMapId_t	decal;
	int		ratio;

	ST			*st_p;
	float		sf,tf;
	ReducedVertex	*v1_p, *v2_p, *v3_p, *v4_p;

	int		i, i2;
	float		f, fx, fy, fz, fx0, fy0, fx1, fy1, fx2, fy2;


//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	mv_p = &tmp_grvx[1];

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	v1_p = mv_p;
	v2_p = mv_p + 1;
	v3_p = mv_p + 2;
	v4_p = mv_p + 3;

	i = pt_p->color | (int) pt_p->alpha << 24;
	grConstantColorValue (i);

	i = 0;
	txn_p = smkpuf_decals[i];

	decal = txn_p->texture_handle;
	guTexSource (decal);
//	grTexLodBiasValue (GR_TMU0, -1.0f);

	i = txn_p->texture_info.header.aspect_ratio * 2;
	sf = rat_t[i];
	tf = rat_t[i + 1];

//	printf ("PTL %d %d\n", (int)decal, i);

	sf *= oow;
	tf *= oow;

	st_p = ptlst_t;

	fx1 = st_p[0].s * sf;		// Assumes rectangular ST
	fy1 = st_p[0].t * tf;
	fx2 = st_p[2].s * sf;
	fy2 = st_p[2].t * tf;

	v1_p->sow = fx1;
	v1_p->tow = fy1;

	v2_p->sow = fx2;
	v2_p->tow = fy1;

	v3_p->sow = fx2;
	v3_p->tow = fy2;

	v4_p->sow = fx1;
	v4_p->tow = fy2;


	if (fx > -_H &&				// Left edge mostly on screen?
		 fx0 < HRESf + _H &&		// Right edge mostly on screen?
		 fy0 > -_V &&				// Bottom edge mostly on screen?
		 fy < vresf_var + _V) {		// Top edge mostly on screen?



		grDrawSimpleTriangle(v4_p,v2_p,v1_p);
		grDrawSimpleTriangle(v4_p,v3_p,v2_p);


	}
	else {
			clip_triangle_t(v4_p, v2_p, v1_p);
			clip_triangle_t(v4_p, v3_p, v2_p);
	}
}

#endif


/********************************
 Transform
********************************/

#if	0

// 184xxxx C (orig 263xxxx)
// 135xxxx asm

#define	_HR	fovval

static
void	ptl_xform (LIMB *limb, float *m_p)
{
	VERTEX	*vt_p;
	ReducedVertex	*mv_p;
	float		oow;				// 1/W
	float		oowhr;			// 1/W * _HR

	int		i;
	float		vx, vy, vz;
	float		fx, fy, fz;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	vt_p = limb->pvtx;
	mv_p = tmpgrv_p;

	i = 0;
	do {

		vx = vt_p->x;
		vy = vt_p->y;
		vz = vt_p->z;
		vt_p++;

		// Transform vertex into camera coordinates

		fx = vx * m_p[0] + vy * m_p[1] + vz * m_p[2] + m_p[3];
		fy = vx * m_p[4] + vy * m_p[5] + vz * m_p[6] + m_p[7];
		fz = vx * m_p[8] + vy * m_p[9] + vz * m_p[10] + m_p[11];

//		if (fz == 0.0f)
//			fz = 0.0001;

		oow = 1.0f / fz;
		mv_p->oow = oow;

		oowhr = oow * _HR;
		fx = -fx * oowhr + (HRES/2);	// clamp to mult of 0.25
		fy = fy * oowhr + (vres_var/2);

		mv_p->x = fx;
		mv_p->y = fy;
		mv_p++;

	} while (++i < limb->nvtx);
}

#endif

/****************************************************************/

/********************************
 Test
********************************/

#if	0

typedef struct {
  float x, y, z;                /* object space coordinates */
  float nx, ny, nz;             /* object space vertex normal for lighting */
  float s, t;                   /* pre-glide-ified texture coordinates */
} Vert;

float		brgt = 1;
float		norm[3] = { 0, 0, 1 };
float		light[3] = { .57f, .57f, .57f };

void	shad_test()
{
	GrVertex	gvert[3];
	float		int[3];

	int		i;
	float		vt_t[3];
	float		f;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

	i = 0;

	gvert[i].x = vt_t[0];
	gvert[i].y = vt_t[1];
	gvert[i].oow = 1.0f / vt_t[2];
	gvert[i].tmuvtx[0].oow = gvert[i].oow;
	gvert[i].tmuvtx[0].sow = v->s * gvert[i].oow;
	gvert[i].tmuvtx[0].tow = v->t * gvert[i].oow;

	f = ((light[0] * norm[0] + 
			light[1] * norm[1] + 
			light[2] * norm[2]) + 1.0f) * 127.5f;

	gvert[i].r = f * brgt;
	gvert[i].g = f * brgt;
	gvert[i].b = f * brgt;
	gvert[i].a = 255.0f;

	grDrawTriangle(&gvert[0], &gvert[1], &gvert[2]);

}

#endif

/****************************************************************/
// EOF

