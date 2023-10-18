/****************************************************************************/
/*                                                                          */
/* pipe3d.c - 3D object pipeline & related functions                        */
/*                                                                          */
/* Written by:  Jason Skiles                                                */
/*                                                                          */
/* $Header: /video/Nba/Ani3d/pipe3D.c 40    4/13/99 3:01p Danielt $       */
/*                                                                          */
/* Copyright (c) 1996 by Williams Electronics Games Inc.                    */
/* All Right Reserved                                                       */
/*                                                                          */
/****************************************************************************/
#include <stdio.h>
#include	<math.h>
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif

#include "ani3D.h"
#include "ani3Di.h"
#include "trigs.h"
#include "game.h"
#include "player.h"
#ifdef JONHEY
#include "srmath.h"
#endif
#define PI_RAD			3.1415926f
#define PRINT_LOADS		FALSE

#ifdef JONHEY

void clip_triangle_t (ReducedVertex *in1, ReducedVertex *in2, ReducedVertex *in3);

void convert_2_jonhey (jonhey_t *jh_obj, LIMB *model, float *matrix, 
					   Texture_node_t **decals);

void draw_jh_object(jonhey_t *obj);


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

#endif
void render_node_mpc_reflect( SNODE *limb, LIMB **model, float *matrix,
							Texture_node_t **decal_array, float *jpos, float *inv_camera );



//
// local data
//
ReducedVertex		tmp_grvx[1024];
ReducedVertex		*tmp_grv = &tmp_grvx[1];

static void draw_jersey_tris( jonhey_t *, ReducedVertex *, int, int );

static void xform_plate( LIMB *, float *, float *, ReducedVertex * );
#if 0
static void xform_limb_c( LIMB *, float *, ReducedVertex * );
static void xform_limb_sls( LIMB *, float *, ReducedVertex * );
#if defined(VEGAS)
static void draw_tris_fbc( LIMB *, Texture_node_t **, ReducedVertex * );
static void draw_triangles( LIMB *, Texture_node_t **, ReducedVertex *, int * );

#else
static void draw_tris_fbc( LIMB *, GrMipMapId_t *, ReducedVertex * );
static void draw_triangles( LIMB *, GrMipMapId_t *, ReducedVertex *, int * );
#endif
#endif

//
// external data & functions
//
extern	float	hres, vres;
extern	int	is_low_res;
extern bbplyr_data player_blocks[];

//////////////////////////////////////////////////////////////////////////////
// load_textures() - Load all the textures from tex_list[].  Store their
// aspect ratios in ratios[] and all their handles in decals[].
//
#if defined(VEGAS)
void load_textures( char *tex_list[], int *ratios, Texture_node_t **decals,
		int max_decals, int id, int minfilt, int maxfilt )
#else
void load_textures( char *tex_list[], int *ratios, GrMipMapId_t *decals,
		int max_decals, int id, int minfilt, int maxfilt )
#endif
{
	int		i;
	struct texture_node *pnode;

	for( i = 0; tex_list[i] != NULL; i++ )
	{
		if( i >= max_decals )
		{
			lockup();
		}

#if PRINT_LOADS
		fprintf(stderr,  "libAni3D: Loading texture %s...", tex_list[i] );
#endif

  		pnode = create_texture( tex_list[i], id, 0, CREATE_NORMAL_TEXTURE,
  				GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP, minfilt, maxfilt );

		if( pnode != NULL )
		{
			ratios[i] = pnode->texture_info.header.aspect_ratio;
			decals[i] = pnode->texture_handle;
#if PRINT_LOADS
			fprintf(stderr,  "done, handle: %d\r\n", (int)pnode->texture_handle );
#endif
		}
		else
		{
			decals[i] = 0;
#if PRINT_LOADS
			fprintf(stderr,  "FAILED\r\n" );
			lockup();
#endif
		}
	}
}

void load_head_textures(char *tex_list[], int *ratios, Texture_node_t **decals,
						int max_decals, int id, int minfilt, int maxfilt, int index)
{
	int i, mode;
	struct texture_node *pnode;
	
	for (i = 0; tex_list[i] != NULL; i++) {
		if (i >= max_decals)
			lockup();
		
		mode = i == index ? GR_TEXTURECLAMP_CLAMP : GR_TEXTURECLAMP_WRAP;
  		pnode = create_texture(tex_list[i], id, 0, CREATE_NORMAL_TEXTURE, mode, mode, minfilt, maxfilt);
		
		if (pnode != NULL) {
			ratios[i] = pnode->texture_info.header.aspect_ratio;
			decals[i] = pnode->texture_handle;
		} else
			decals[i] = 0;
	}
}


//////////////////////////////////////////////////////////////////////////////
// render_node() - Draw limb and recursively draw any limbs beneath it
// in the object hierarchy.
//////////////////////////////////////////////////////////////////////////////
#define NORM	FALSE
#if defined(VEGAS)
void render_node( SNODE *limb, LIMB **model, float *parent_matrix, float *fp,
		Texture_node_t **decal_array, float *jpos )
#else
void render_node( SNODE *limb, LIMB **model, float *parent_matrix, float *fp,
		GrMipMapId_t *decal_array, float *jpos )
#endif
{
	float	lxform[12], matrix[12];
	float	tx,ty,tz;

#ifdef JONHEY
	jonhey_t jh_obj;
#endif

#if NORM
	float	n1,n2,n3;
#endif

	/* xlate +joint pos */
	tx = limb->x;
	ty = limb->y;
	tz = limb->z;

	lxform[3] = tx;
	lxform[7] = ty;
	lxform[11] = tz;

	jpos[0] = parent_matrix[0] * tx + parent_matrix[1] * ty + parent_matrix[2] * tz + parent_matrix[3];
	jpos[1] = parent_matrix[4] * tx + parent_matrix[5] * ty + parent_matrix[6] * tz + parent_matrix[7];
	jpos[2] = parent_matrix[8] * tx + parent_matrix[9] * ty + parent_matrix[10] * tz + parent_matrix[11];

	/* rotate about this joint */
	quat2mat( fp, lxform );

	mxm( parent_matrix, lxform, matrix );

	/* xlate -joint pos */
	matrix[3] -= matrix[0] * tx + matrix[1] * ty + matrix[2] * tz;
	matrix[7] -= matrix[4] * tx + matrix[5] * ty + matrix[6] * tz;
	matrix[11] -= matrix[8] * tx + matrix[9] * ty + matrix[10] * tz;

#if NORM
	// verify that the matrix is normalized!
	n1 = fsqrt(matrix[0] * matrix[0] + matrix[1] * matrix[1] + matrix[2] * matrix[2]);
	n2 = fsqrt(matrix[4] * matrix[4] + matrix[5] * matrix[5] + matrix[6] * matrix[6]);
	n3 = fsqrt(matrix[8] * matrix[8] + matrix[9] * matrix[9] + matrix[10] * matrix[10]);

	if(( n1 < 0.999f ) || ( n1 > 1.001 ) ||
	   ( n2 < 0.999f ) || ( n2 > 1.001 ) ||
	   ( n3 < 0.999f ) || ( n3 > 1.001 ))
	{
		fprintf(stderr,  "non-normalized quat %f %f %f %f\r\n", fp[0], fp[1], fp[2], fp[3] );
	}
#endif

#ifdef JONHEY
	convert_2_jonhey (&jh_obj, *model, matrix, decal_array);
#endif	


	/* render any objects at this joint */
	if( !(limb->flags & SN_NODRAW ))
	{
#ifndef JONHEY
#else
		if( limb->flags & SN_DBLSIDE )
			{
			jh_obj.mode |= JONHEY_MODE_DBL_SIDED;
			}

	draw_jh_object(&jh_obj);

#endif

	}

	/* draw any children */
	if( limb->children[0] )
	{
		render_node( limb + limb->children[0],
					 model + limb->children[0],
					 matrix,
					 fp + limb->children[0] * 4,
					 decal_array,
					 jpos + limb->children[0] * 3 );
		if( limb->children[1] )
		{
			render_node( limb + limb->children[1],
						 model + limb->children[1],
						 matrix,
						 fp + limb->children[1] * 4,
						 decal_array,
						 jpos + limb->children[1] * 3 );
			if( limb->children[2] )
			{
				render_node( limb + limb->children[2],
							 model + limb->children[2],
							 matrix,
							 fp + limb->children[2] * 4,
							 decal_array,
							 jpos + limb->children[2] * 3 );
			}
		}
	}
}


void fgenerate_matrices(short *script,  SNODE *toplimb, bbplyr_data *ppdata,
						float *parent_matrix, float *quats, float hs, float fs)
{
	float	*lxform;
	float	tx,ty,tz,sx,sy,sz;
	float	*matrix;
    float *fp, *jpos;
    SNODE *limb = toplimb;
    short cmd, skip=0;

	while (*script != DONE) 
	{
		if (*script & 0x80) 
		{
			cmd = *script & 0x7f;
			parent_matrix = ppdata->matrices[cmd];
			script++;
		}
		else 
		{
			skip = *script & NOMATRIX;
			cmd = *script & 0x3f;
			matrix = ppdata->matrices[cmd];
			lxform = ppdata->matrices_flipped[cmd];
			fp = quats + cmd*4;
			limb = toplimb + cmd;
			jpos = ppdata->jpos[cmd];

			/* xlate +joint pos */
			tx = limb->x;
			ty = limb->y;
			tz = limb->z;

			lxform[3] = tx;
			lxform[7] = ty;
			lxform[11] = tz;

			jpos[0] = parent_matrix[0] * tx + parent_matrix[1] * ty + parent_matrix[2] * tz + parent_matrix[3];
			jpos[1] = parent_matrix[4] * tx + parent_matrix[5] * ty + parent_matrix[6] * tz + parent_matrix[7];
			jpos[2] = parent_matrix[8] * tx + parent_matrix[9] * ty + parent_matrix[10] * tz + parent_matrix[11];

			/* rotate about this joint */
			quat2mat( fp, lxform );
			mxm( parent_matrix, lxform, matrix );

			if(limb->flags & SN_HEAD_SCALE)// || limb->flags & SN_FEET_SCALE)
			{
				sx = 1.0f / player_blocks[(int)fs].odata.xscale;
				sy = 1.0f / player_blocks[(int)fs].odata.yscale;
				sz = 1.0f / player_blocks[(int)fs].odata.zscale;
				matrix[0] *= sx; matrix[1] *= sy; matrix[ 2] *= sz;
				matrix[4] *= sx; matrix[5] *= sy; matrix[ 6] *= sz;
				matrix[8] *= sx; matrix[9] *= sy; matrix[10] *= sz;

				sx = hs;
				matrix[0] *= sx; matrix[1] *= sx; matrix[ 2] *= sx;
				matrix[4] *= sx; matrix[5] *= sx; matrix[ 6] *= sx;
				matrix[8] *= sx; matrix[9] *= sx; matrix[10] *= sx;
			}

			/* xlate -joint pos */
			matrix[3] -= matrix[0] * tx + matrix[1] * ty + matrix[2] * tz;
			matrix[7] -= matrix[4] * tx + matrix[5] * ty + matrix[6] * tz;
			matrix[11] -= matrix[8] * tx + matrix[9] * ty + matrix[10] * tz;
			parent_matrix = matrix;
		   	script++;	
		}
	}
}
			

//////////////////////////////////////////////////////////////////////////////
// generate_matrices() - Generate matrix for node & recurse through any sub-
// nodes.
//////////////////////////////////////////////////////////////////////////////
void generate_matrices( SNODE *limb, float *parent_matrix, float *fp, float *matrix, float *jpos , float hs, float fs)
{
	float	lxform[12];
	float	tx,ty,tz,sx,sy,sz;

	/* xlate +joint pos */
	tx = limb->x;
	ty = limb->y;
	tz = limb->z;

	lxform[3] = tx;
	lxform[7] = ty;
	lxform[11] = tz;

	jpos[0] = parent_matrix[0] * tx + parent_matrix[1] * ty + parent_matrix[2] * tz + parent_matrix[3];
	jpos[1] = parent_matrix[4] * tx + parent_matrix[5] * ty + parent_matrix[6] * tz + parent_matrix[7];
	jpos[2] = parent_matrix[8] * tx + parent_matrix[9] * ty + parent_matrix[10] * tz + parent_matrix[11];

	/* rotate about this joint */
	quat2mat( fp, lxform );
	mxm( parent_matrix, lxform, matrix );

#if 0
	if(limb->flags & SN_HEAD_SCALE || limb->flags & SN_FEET_SCALE)
	{
		s = limb->flags & SN_FEET_SCALE ? fs : hs;
		matrix[0] *= s; matrix[1] *= s; matrix[2] *= s;
		matrix[4] *= s; matrix[5] *= s; matrix[6] *= s;
		matrix[8] *= s; matrix[9] *= s; matrix[10] *= s;
	}
#else
	if(limb->flags & SN_HEAD_SCALE)// || limb->flags & SN_FEET_SCALE)
	{
		sx = 1.0f / player_blocks[(int)fs].odata.xscale;
		sy = 1.0f / player_blocks[(int)fs].odata.yscale;
		sz = 1.0f / player_blocks[(int)fs].odata.zscale;
		matrix[0] *= sx; matrix[1] *= sy; matrix[ 2] *= sz;
		matrix[4] *= sx; matrix[5] *= sy; matrix[ 6] *= sz;
		matrix[8] *= sx; matrix[9] *= sy; matrix[10] *= sz;

		sx = hs;
		matrix[0] *= sx; matrix[1] *= sx; matrix[ 2] *= sx;
		matrix[4] *= sx; matrix[5] *= sx; matrix[ 6] *= sx;
		matrix[8] *= sx; matrix[9] *= sx; matrix[10] *= sx;
	}
#endif

	/* xlate -joint pos */
	matrix[3] -= matrix[0] * tx + matrix[1] * ty + matrix[2] * tz;
	matrix[7] -= matrix[4] * tx + matrix[5] * ty + matrix[6] * tz;
	matrix[11] -= matrix[8] * tx + matrix[9] * ty + matrix[10] * tz;

	/* draw any children */
	if( limb->children[0] )
	{
		generate_matrices( limb + limb->children[0],
					 matrix,
					 fp + limb->children[0] * 4,
					 matrix + limb->children[0] * 12,
					 jpos + limb->children[0] * 3, hs, fs);
		if( limb->children[1] )
		{
			generate_matrices( limb + limb->children[1],
						 matrix,
						 fp + limb->children[1] * 4,
						 matrix + limb->children[1] * 12,
						 jpos + limb->children[1] * 3, hs, fs);
			if( limb->children[2] )
			{
				generate_matrices( limb + limb->children[2],
							 matrix,
							 fp + limb->children[2] * 4,
							 matrix + limb->children[2] * 12,
							 jpos + limb->children[2] * 3, hs, fs);
			}
		}
	}
}

void generate_matrices_flipy( SNODE *limb, float *parent_matrix, float *fp, float *matrix, float *jpos , float hs, float fs, float *inv_m, float *inv_camera)
{
	float	lxform[12];
	float	tx,ty,tz,sx,sy,sz;

#ifdef ITERATIVE_GEN_MATRICES
	int i;
	for (i=0; i<12; i++)
		lxform[i] = matrix[i];
#else

	/* xlate +joint pos */
	tx = limb->x;
	ty = limb->y;
	tz = limb->z;

	/* rotate about this joint */
	quat2mat( fp, lxform );
#endif // ITERATIVE_GEN_MATRICES
	lxform[3] = tx;
	lxform[7] = ty;
	lxform[11] = tz;

	mxm( parent_matrix, lxform, matrix );

	if(limb->flags & SN_HEAD_SCALE)// || limb->flags & SN_FEET_SCALE)
	{
		sx = 1.0f / player_blocks[(int)fs].odata.xscale;
		sy = 1.0f / player_blocks[(int)fs].odata.yscale;
		sz = 1.0f / player_blocks[(int)fs].odata.zscale;
		matrix[0] *= sx; matrix[1] *= sy; matrix[ 2] *= sz;
		matrix[4] *= sx; matrix[5] *= sy; matrix[ 6] *= sz;
		matrix[8] *= sx; matrix[9] *= sy; matrix[10] *= sz;

		sx = hs;
		matrix[0] *= sx; matrix[1] *= sx; matrix[ 2] *= sx;
		matrix[4] *= sx; matrix[5] *= sx; matrix[ 6] *= sx;
		matrix[8] *= sx; matrix[9] *= sx; matrix[10] *= sx;
	}

	/* xlate -joint pos */
	matrix[3] -= matrix[0] * tx + matrix[1] * ty + matrix[2] * tz;
	matrix[7] -= matrix[4] * tx + matrix[5] * ty + matrix[6] * tz;
	matrix[11] -= matrix[8] * tx + matrix[9] * ty + matrix[10] * tz;


	inv_camera[0] = inv_m[0];
	inv_camera[1] = inv_m[1];
	inv_camera[2] = inv_m[2];
	inv_camera[3] = inv_m[3];
	inv_camera[4] = inv_m[4];
	inv_camera[5] = inv_m[5];
	inv_camera[6] = inv_m[6];
	inv_camera[7] = inv_m[7];
	inv_camera[8] = inv_m[8];
	inv_camera[9] = inv_m[9];
	inv_camera[10] = inv_m[10];
	inv_camera[11] = inv_m[11];
	inv_camera[12] = inv_m[12];
	inv_camera[13] = inv_m[13];
	inv_camera[14] = inv_m[14];
	inv_camera[15] = inv_m[15];



	/* draw any children */
	if( limb->children[0] )
	{
		generate_matrices_flipy( limb + limb->children[0],
					 matrix,
					 fp + limb->children[0] * 4,
					 matrix + limb->children[0] * 12,
					 jpos + limb->children[0] * 3, hs, fs,inv_m,
					 inv_camera + limb->children[0]*16 );
		if( limb->children[1] )
		{
			generate_matrices_flipy( limb + limb->children[1],
						 matrix,
						 fp + limb->children[1] * 4,
						 matrix + limb->children[1] * 12,
						 jpos + limb->children[1] * 3, hs, fs,inv_m,
						 inv_camera + limb->children[1]*16);
			if( limb->children[2] )
			{
				generate_matrices_flipy( limb + limb->children[2],
							 matrix,
							 fp + limb->children[2] * 4,
							 matrix + limb->children[2] * 12,
							 jpos + limb->children[2] * 3, hs, fs,inv_m,
							 inv_camera + limb->children[2]*16);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// frender_node_mpc() - Draw limb and recursively draw any limbs beneath it
// in the object hierarchy. (Matrices precomputed.)
//////////////////////////////////////////////////////////////////////////////
#define NORM	FALSE
#if defined(VEGAS)
void frender_node_mpc(short *script, SNODE *toplimb, LIMB **topmodel, float *matrices,
		Texture_node_t **decal_array)
#else
void frender_node_mpc(short *script, SNODE *toplimb, LIMB **topmodel, float *matrices,
		GrMipMapId_t *decal_array)
#endif
{
	float	*matrix = NULL;
    SNODE *limb = toplimb;
    short cmd;
	LIMB **model;
	#ifdef JONHEY
	jonhey_t jh_obj;
	#endif

	while (*script != DONE) 
	{
		if (*script & 0x80) 
		{
			script++;
		}
		else 
		{
			cmd = *script & 0x3f;
			matrix = matrices + cmd*12;
			limb = toplimb + cmd;
			model = topmodel + cmd;

			#ifdef JONHEY
				convert_2_jonhey (&jh_obj, *model, matrix, decal_array);
			#endif	

			/* render any objects at this joint */
			if( !(limb->flags & SN_NODRAW ))
			{

				if( limb->flags & SN_DBLSIDE )
					{
						if(limb->flags & SN_TURBO_LIGHT)
						{
							if(limb->flags & SN_TURBO_ALLCOLORS)
							{
								if(limb->flags & SN_TURBO_COLOR_RED)
									grConstantColorValue( (0<<24) + (195<<16) + (0<<8) + 0);
								if(limb->flags & SN_TURBO_COLOR_YELLOW)
									grConstantColorValue( (0<<24) + (248<<16) + (225<<8) + 0);
								if(limb->flags & SN_TURBO_COLOR_BLUE)
									grConstantColorValue( (0<<24) + (0<<16) + (8<<8) + 200);
								if(limb->flags & SN_TURBO_COLOR_GREEN)
									grConstantColorValue( (0<<24) + (29<<16) + (195<<8) + 0);
							guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB);
							}		
						}

						#ifndef JONHEY
						#else
						jh_obj.mode = JONHEY_MODE_DBL_SIDED;
						draw_jh_object(&jh_obj);
						#endif

						if(limb->flags & SN_TURBO_LIGHT)
						{
							grConstantColorValue( (255 << 24) );
							guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
						}
					}
				else
					{

						#ifndef JONHEY
						#else
						draw_jh_object(&jh_obj);
						#endif

						if(limb->flags & SN_TURBO_LIGHT)
						{
					    /*grColorCombine( GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL,
															GR_COMBINE_FACTOR_ONE,
															GR_COMBINE_LOCAL_CONSTANT,
															GR_COMBINE_OTHER_TEXTURE,
															FXFALSE);*/
							grConstantColorValue( (255 << 24) );
							guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
						}
					}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// render_node_mpc() - Draw limb and recursively draw any limbs beneath it
// in the object hierarchy. (Matrices precomputed.)
//////////////////////////////////////////////////////////////////////////////
#define NORM	FALSE
#if defined(VEGAS)
void render_node_mpc( SNODE *limb, LIMB **model, float *matrix,
		Texture_node_t **decal_array, float *jpos )
#else
void render_node_mpc( SNODE *limb, LIMB **model, float *matrix,
		GrMipMapId_t *decal_array, float *jpos )
#endif
{
#ifdef JONHEY
jonhey_t jh_obj;
#endif

	/* render any objects at this joint */
	if( !(limb->flags & SN_NODRAW ))
	{

		#ifdef JONHEY
		convert_2_jonhey (&jh_obj, *model, matrix, decal_array);
		#endif	

		if( limb->flags & SN_DBLSIDE )
			{
				if(limb->flags & SN_TURBO_LIGHT)
				{
					if(limb->flags & SN_TURBO_ALLCOLORS)
					{
						if(limb->flags & SN_TURBO_COLOR_RED)
							grConstantColorValue( (0<<24) + (195<<16) + (0<<8) + 0);
						if(limb->flags & SN_TURBO_COLOR_YELLOW)
							grConstantColorValue( (0<<24) + (248<<16) + (225<<8) + 0);
						if(limb->flags & SN_TURBO_COLOR_BLUE)
							grConstantColorValue( (0<<24) + (0<<16) + (8<<8) + 200);
						if(limb->flags & SN_TURBO_COLOR_GREEN)
							grConstantColorValue( (0<<24) + (29<<16) + (195<<8) + 0);
					guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB);
					}
				}

				#ifndef JONHEY
				#else
				jh_obj.mode = JONHEY_MODE_DBL_SIDED;
				draw_jh_object(&jh_obj);
				#endif

				if(limb->flags & SN_TURBO_LIGHT)
				{
					grConstantColorValue( (255 << 24) );
					guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
				}
			}
		else
			{
				if(limb->flags & SN_TURBO_LIGHT)
				{
					if(limb->flags & SN_TURBO_ALLCOLORS)
					{
						if(limb->flags & SN_TURBO_COLOR_RED)
							grConstantColorValue( (0<<24) + (195<<16) + (0<<8) + 0);
						if(limb->flags & SN_TURBO_COLOR_YELLOW)
							grConstantColorValue( (0<<24) + (248<<16) + (225<<8) + 0);
						if(limb->flags & SN_TURBO_COLOR_BLUE)
							grConstantColorValue( (0<<24) + (0<<16) + (8<<8) + 200);
						if(limb->flags & SN_TURBO_COLOR_GREEN)
							grConstantColorValue( (0<<24) + (29<<16) + (195<<8) + 0);
					guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB);
					}
				}
	
				#ifndef JONHEY
				#else
				draw_jh_object(&jh_obj);
				#endif

				if(limb->flags & SN_TURBO_LIGHT)
				{
					grConstantColorValue( (255 << 24) );
					guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
				}
#if 0
				if(limb->flags & SN_TURBO_LIGHT)
				{
			    /*grColorCombine( GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL,
													GR_COMBINE_FACTOR_ONE,
													GR_COMBINE_LOCAL_CONSTANT,
													GR_COMBINE_OTHER_TEXTURE,
													FXFALSE);*/
					grConstantColorValue( (255 << 24) );
					guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
				}
#endif	
			}
	}

	/* draw any children */
	if( limb->children[0] )
	{
		render_node_mpc( limb + limb->children[0],
					 model + limb->children[0],
					 matrix + limb->children[0] * 12,
					 decal_array,
					 jpos + limb->children[0] * 3 );
		if( limb->children[1] )
		{
			render_node_mpc( limb + limb->children[1],
						 model + limb->children[1],
						 matrix + limb->children[1] * 12,
						 decal_array,
						 jpos + limb->children[1] * 3 );
			if( limb->children[2] )
			{
				render_node_mpc( limb + limb->children[2],
							 model + limb->children[2],
							 matrix + limb->children[2] * 12,
							 decal_array,
							 jpos + limb->children[2] * 3 );
			}
		}
	}
}








//////////////////////////////////////////////////////////////////////////////
// render_node_mpc_reflect() - Draw limb and recursively draw any limbs beneath it
// in the object hierarchy. (Matrices precomputed.)
//////////////////////////////////////////////////////////////////////////////

void render_node_mpc_reflect( SNODE *limb, LIMB **model, float *matrix,
							Texture_node_t **decal_array, float *jpos, float *inv_camera )

{
jonhey_t jh_obj;
int i;
VERTEX *vert;
float y;
float y_seed;
float alpha;
int alpha_int;
float result[16];

	/* render any objects at this joint */
	if( !(limb->flags & SN_NODRAW ))
	{
	if(absf(matrix[2]) > 100.0f)
		{
		//printf("%f abort\n",matrix[2]);
		goto rnmr_exit;
		}

		convert_2_jonhey (&jh_obj, *model, matrix, decal_array);

		if( limb->flags & SN_DBLSIDE )
			{
				if(limb->flags & SN_TURBO_LIGHT)
				{
					if(limb->flags & SN_TURBO_ALLCOLORS)
					{
						if(limb->flags & SN_TURBO_COLOR_RED)
							grConstantColorValue( (0<<24) + (195<<16) + (0<<8) + 0);
						if(limb->flags & SN_TURBO_COLOR_YELLOW)
							grConstantColorValue( (0<<24) + (248<<16) + (225<<8) + 0);
						if(limb->flags & SN_TURBO_COLOR_BLUE)
							grConstantColorValue( (0<<24) + (0<<16) + (8<<8) + 200);
						if(limb->flags & SN_TURBO_COLOR_GREEN)
							grConstantColorValue( (0<<24) + (29<<16) + (195<<8) + 0);
					guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB);
					}
				}


				jh_obj.mode = JONHEY_MODE_DBL_SIDED;
				draw_jh_object(&jh_obj);

				if(limb->flags & SN_TURBO_LIGHT)
				{
					grConstantColorValue( (255 << 24) );
					guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
				}
			}
		else
			{
				if(limb->flags & SN_TURBO_LIGHT)
				{
					if(limb->flags & SN_TURBO_ALLCOLORS)
					{
						if(limb->flags & SN_TURBO_COLOR_RED)
							grConstantColorValue( (0<<24) + (195<<16) + (0<<8) + 0);
						if(limb->flags & SN_TURBO_COLOR_YELLOW)
							grConstantColorValue( (0<<24) + (248<<16) + (225<<8) + 0);
						if(limb->flags & SN_TURBO_COLOR_BLUE)
							grConstantColorValue( (0<<24) + (0<<16) + (8<<8) + 200);
						if(limb->flags & SN_TURBO_COLOR_GREEN)
							grConstantColorValue( (0<<24) + (29<<16) + (195<<8) + 0);
					guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB);
					}
				}			    

				//	guAlphaSource(GR_ALPHASOURCE_CC_ALPHA);
				//	grAlphaBlendFunction( GR_BLEND_ONE, GR_BLEND_ZERO, GR_BLEND_ONE,	GR_BLEND_ZERO );
							    
				result[ 4] = inv_camera[ 4]*matrix[ 0] + inv_camera[ 5]*matrix[ 4] + inv_camera[ 6]*matrix[ 8];
				result[ 5] = inv_camera[ 4]*matrix[ 1] + inv_camera[ 5]*matrix[ 5] + inv_camera[ 6]*matrix[ 9];
				result[ 6] = inv_camera[ 4]*matrix[ 2] + inv_camera[ 5]*matrix[ 6] + inv_camera[ 6]*matrix[10];
				result[ 7] = inv_camera[ 4]*matrix[ 3] + inv_camera[ 5]*matrix[ 7] + inv_camera[ 6]*matrix[11] + inv_camera[ 7];


				y_seed = -100.0f;
				for(i=0;i<8;i++)
					{
   				    vert = &(jh_obj.limb->bbox[i]);
					y = result[4]*vert->x + result[5]*vert->y + result[6]*vert->z + result[7];
					if (y > y_seed)
						y_seed = y;
					}

				if(y_seed < 1.0f)
					y_seed = 1.0f;

				y_seed *= y_seed;
				y_seed *= 0.04f;


				alpha = 255.0f/y_seed;

				if(alpha > 255.0f)
					alpha = 255.0f;

				alpha_int = (int) alpha;
	
				grConstantColorValue(  (alpha_int << 24) );

				// grConstantColorValue(  (0xff << 24) );
				// guColorCombineFunction(GR_COLORCOMBINE_DECAL_TEXTURE);

				draw_jh_object(&jh_obj);

				if(limb->flags & SN_TURBO_LIGHT)
				{
					grConstantColorValue( (255 << 24) );
					guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
				}


			}

	}

rnmr_exit:


	/* draw any children */
	if( limb->children[0] )
	{
		render_node_mpc_reflect( limb + limb->children[0],
					 model + limb->children[0],
					 matrix + limb->children[0] * 12,
					 decal_array,
					 jpos + limb->children[0] * 3,
					 inv_camera + limb->children[0] * 16  );
		if( limb->children[1] )
		{
			render_node_mpc_reflect( limb + limb->children[1],
						 model + limb->children[1],
						 matrix + limb->children[1] * 12,
						 decal_array,
						 jpos + limb->children[1] * 3,
						 inv_camera + limb->children[1] * 16   );
			if( limb->children[2] )
			{
				render_node_mpc_reflect( limb + limb->children[2],
							 model + limb->children[2],
							 matrix + limb->children[2] * 12,
							 decal_array,
							 jpos + limb->children[2] * 3,
							 inv_camera + limb->children[2] * 16   );
			}
		}
	}





}


//////////////////////////////////////////////////////////////////////////////
// render_limb() - Draw limb.
//////////////////////////////////////////////////////////////////////////////
#if defined(VEGAS)
void render_limb( LIMB *limb, float *xform, Texture_node_t **decals,
				  int *ratios )
#else
void render_limb( LIMB *limb, float *xform, GrMipMapId_t *decals,
				  int *ratios )
#endif
{
	/* render any objects at this joint */
#ifndef JONHEY
#else
jonhey_t jh_obj;

	convert_2_jonhey (&jh_obj, limb, xform, decals);
	draw_jh_object(&jh_obj);
#endif


}

//////////////////////////////////////////////////////////////////////////////
// render_limb_jers() - Draw limb. (special for jerseys)
//////////////////////////////////////////////////////////////////////////////
void render_limb_jers( jonhey_t *obj, int d1, int d2)
{
	transform_verts(obj);
	draw_jersey_tris( obj, tmp_grv, d1, d2 );
}


//////////////////////////////////////////////////////////////////////////////
// transform vertices for a limb
//////////////////////////////////////////////////////////////////////////////
static void xform_plate( LIMB *limb, float *pos1, float *pos2, ReducedVertex *dest )
{
	int i;
	float tx, ty;

	for( i = 0; i < limb->nvtx; i++ )
	{
		/* transform (ha!) vertex into screen coordinates */
		tx = limb->pvtx[i].x + pos1[0] + pos2[0];
		ty = limb->pvtx[i].y + pos1[1] + pos2[1];

		/* store results */
		tmp_grv[i].x = tx;
		tmp_grv[i].y = ty;
		tmp_grv[i].oow = 0.5f;
	}
}


//////////////////////////////////////////////////////////////////////////////
#define SCROLL_WIDTH 0.097656f	//0.103239f
static void draw_jersey_tris( jonhey_t *obj, ReducedVertex *vsrc, int d1, int d2 )
{
register int	count;
register TRI	*ptri;
float			oow;
register ST		*pst;
float			sa;
ReducedVertex		*v1, *v2, *v3;
LIMB *limb;
/******************************************************************************/

	limb = obj->limb;
	count = limb->ntris;
	ptri = limb->ptri;
	pst = limb->pst;

	guTexSource( *(obj->text) );

	#ifdef GEOM_STATS
	stats.verts_processed += limb->nvtx;
	stats.verts_bbox_processed += limb->nvtx;
	stats.tris_processed  += count;
	stats.tris_bbox_processed  += count;
	#endif

	while(count--)
	{
		v1 = vsrc + (ptri->v1);
		v2 = vsrc + (ptri->v2);
		v3 = vsrc + (ptri->v3);

		if( ptri->texture )
			sa = SCROLL_WIDTH * d2 * 255.0f;
		else
			sa = SCROLL_WIDTH * d1 * 255.0f;

			oow = v1->oow;
			v1->sow = oow  * (sa  + pst[ptri->t1].s);
			v1->tow = oow * pst[ptri->t1].t;
			oow = v2->oow;
			v2->sow = oow  * (sa + pst[ptri->t2].s);
			v2->tow = oow * pst[ptri->t2].t;
			oow = v3->oow;
			v3->sow = oow  * (sa + pst[ptri->t3].s);
			v3->tow = oow * pst[ptri->t3].t;

		clip_triangle_t(v3,v2,v1);

		#ifdef GEOM_STATS
		stats.tris_drawn  += 1;
		stats.verts_drawn += 3;
		#endif


		ptri++;
	}
}


//////////////////////////////////////////////////////////////////////////////
// mxm() - multiply 4x3 matrix m1 by 4x3 matrix m2, store result in 4x3
// matrix m3.
//////////////////////////////////////////////////////////////////////////////
void mxm( float *m1, float *m2, float *m3 )
{
		m3[0] = m1[0] * m2[0] + m1[1] * m2[4] + m1[2] * m2[8];
		m3[1] = m1[0] * m2[1] + m1[1] * m2[5] + m1[2] * m2[9];
		m3[2] = m1[0] * m2[2] + m1[1] * m2[6] + m1[2] * m2[10];
		m3[3] = m1[0] * m2[3] + m1[1] * m2[7] + m1[2] * m2[11] + m1[3];
		m3[4] = m1[4] * m2[0] + m1[5] * m2[4] + m1[6] * m2[8];
		m3[5] = m1[4] * m2[1] + m1[5] * m2[5] + m1[6] * m2[9];
		m3[6] = m1[4] * m2[2] + m1[5] * m2[6] + m1[6] * m2[10];
		m3[7] = m1[4] * m2[3] + m1[5] * m2[7] + m1[6] * m2[11] + m1[7];
		m3[8] = m1[8] * m2[0] + m1[9] * m2[4] + m1[10] * m2[8];
		m3[9] = m1[8] * m2[1] + m1[9] * m2[5] + m1[10] * m2[9];
		m3[10] = m1[8] * m2[2] + m1[9] * m2[6] + m1[10] * m2[10];
		m3[11] = m1[8] * m2[3] + m1[9] * m2[7] + m1[10] * m2[11] + m1[11];
}


//////////////////////////////////////////////////////////////////////////////
// mat2quat() - convert top left 3x3 of 4x4 matrix to quaternion.
//////////////////////////////////////////////////////////////////////////////
#define X	0
#define Y	1
#define Z	2
#define W	3


//////////////////////////////////////////////////////////////////////////////
// quat2mat() - convert quaternion into top left 3x3 corner of a 4x4 matrix.
//////////////////////////////////////////////////////////////////////////////
void quat2mat( float *q, float *mat )
{
	float	s,xs,ys,zs,wx,wy,wz,xx,xy,xz,yy,yz,zz;

	s = 2.0f;

	xs = q[0]*s;	ys = q[1]*s;	zs = q[2]*s;
	wx = q[3]*xs;	wy = q[3]*ys;	wz = q[3]*zs;
	xx = q[0]*xs;	xy = q[0]*ys;	xz = q[0]*zs;
	yy = q[1]*ys;	yz = q[1]*zs;	zz = q[2]*zs;

	mat[0] = 1.0f-(yy+zz);	mat[1] = xy-wz;			mat[2] = xz+wy;
	mat[4] = xy+wz;			mat[5] = 1.0f-(xx+zz);	mat[6] = yz-wx;
	mat[8] = xz-wy;			mat[9] = yz+wx;			mat[10] = 1.0f-(xx+yy);
}


//////////////////////////////////////////////////////////////////////////////
// qxq() - multiply two quaternions together
//////////////////////////////////////////////////////////////////////////////
void qxq( float *q1, float *q2, float *q )
{
	q[W] = q1[W]*q2[W]-q1[X]*q2[X]-q1[Y]*q2[Y]-q1[Z]*q2[Z];

	q[X] = q1[Y]*q2[Z]-q1[Z]*q2[Y]+q1[W]*q2[X]+q2[W]*q1[X];
	q[Y] = q1[Z]*q2[X]-q1[X]*q2[Z]+q1[W]*q2[Y]+q2[W]*q1[Y];
	q[Z] = q1[X]*q2[Y]-q1[Y]*q2[X]+q1[W]*q2[Z]+q2[W]*q1[Z];
}

//////////////////////////////////////////////////////////////////////////////
// slerp() - spherical linear interpolation between two quats
//////////////////////////////////////////////////////////////////////////////
#define EPSILON		0.00001f
#define HALFPI		(PI_RAD*0.5f)
void slerp( float *p, float *q, float t, float *qt )
{
	int		i;
	float	omega, sinom, cosom, sclp, sclq;


	   
//fprintf(stderr,  "slerp: %f %9f %9f %9f %9f\ %9f %9f %9f %9f\r\n",
//			t, p[0], p[1], p[2], p[3], q[0], q[1], q[2], q[3] );

	if (t < 0.01f)
		t = 0.0f;
	if (t > 0.99f)
		t = 1.0f;
	
	if( t == 0.0f )
	{
		qt[0] = p[0];	qt[1] = p[1];	qt[2] = p[2];	qt[3] = p[3];
		return;
	}
	if( t == 1.0f )
	{
		qt[0] = q[0];	qt[1] = q[1];	qt[2] = q[2];	qt[3] = q[3];
		return;
	}

	cosom = p[0]*q[0] + p[1]*q[1] + p[2]*q[2] + p[3]*q[3];
	
	if((1.0f+cosom) > EPSILON )
	{
		if((1.0f-cosom) > EPSILON )
		{
			omega = acos_rad( cosom );
			sinom = 1.0f / sin_rad( omega );
			sclp = sin_rad((1.0f-t)*omega) * sinom;
			sclq = sin_rad(t*omega) * sinom;
		}
		else
		{
			sclp = 1.0f - t;
			sclq = t;
		}

		for( i = 0; i < 4; i++ )
			qt[i] = sclp*p[i] + sclq*q[i];
	}
	else
	{
		qt[0] = -p[1];	qt[1] = p[0];
		qt[2] = -p[3];	qt[3] = p[2];
		sclp = sin_rad((1.0f-t)*HALFPI);
		sclq = sin_rad(t*HALFPI);

		for( i = 0; i < 4; i++ )
			qt[i] = sclp*p[i] + sclq*qt[i];
	}
	
}

///////////////////////////////////////////////////////////////////////////////
// if -p is a better match to q, set p to -p.
void adjust_quat( float *p, float *q )
{
	float	pdq,npdq;

	pdq = (p[0]-q[0])*(p[0]-q[0]) +
			(p[1]-q[1])*(p[1]-q[1]) +
			(p[2]-q[2])*(p[2]-q[2]) +
			(p[3]-q[3])*(p[3]-q[3]);

	npdq = (p[0]+q[0])*(p[0]+q[0]) +
			(p[1]+q[1])*(p[1]+q[1]) +
			(p[2]+q[2])*(p[2]+q[2]) +
			(p[3]+q[3])*(p[3]+q[3]);

	if( npdq < pdq )
	{
		p[0] = -p[0];
		p[1] = -p[1];
		p[2] = -p[2];
		p[3] = -p[3];
	}
}

//////////////////////////////////////////////////////////////////////////////
// t is an int from 0 to 1023.  PI is 512
//////////////////////////////////////////////////////////////////////////////
float isin( int t )
{
	t &= 0x03ff;

	if( t < 256 )
		return( trigs[t] );
	if( t < 512 )
		return( trigs[511-t] );
	if( t < 768 )
		return( -trigs[t-512] );
	return( -trigs[1023-t] );
}

//////////////////////////////////////////////////////////////////////////////
float icos( int t )
{
	t = (t + 256) & 0x03ff;

	if( t < 256 )
		return( trigs[t] );
	if( t < 512 )
		return( trigs[511-t] );
	if( t < 768 )
		return( -trigs[t-512] );
	return( -trigs[1023-t] );
}

//////////////////////////////////////////////////////////////////////////////
// TODO: This will bomb on x=0.  Fix that.
int iasin( float x )
{
	int		low, high, mid, nflag = FALSE;
	float	ld,hd;

	if( x < 0 )
	{
		x = -x;
		nflag = TRUE;
	}

	// find the highest value in trigs[] that's lower than x
	low = 0;
	high = 255;
	while( low < high )
	{
		mid = (low + high)/2;
		if( trigs[mid] <= x )
		{
			if( trigs[mid+1] > x )
				break;
			low = mid + 1;
		}
		else
		{
			if( trigs[mid-1] <= x )
			{
				mid -= 1;
				break;
			}
			high = mid + 1;
		}
	}

	// now return that value, or the next higher, whichever is the
	// best fit.
	
	ld = trigs[mid] - x;
	hd = trigs[mid+1] - x;
	ld *= ld;	hd *= hd;

	if( ld < hd )
		return( nflag ? -mid : mid );
	else
		return( nflag ? -(mid+1) : mid+1 );

}

//////////////////////////////////////////////////////////////////////////////
float asin_rad( float x )
{
	int		low, high, mid, nflag = FALSE;
	float	d, val;

	if( x < 0 )
	{
		x = -x;
		nflag = TRUE;
	}

	// find the highest value in trigs[] that's lower than x
	low = 0;
	high = 255;
	while( low < high )
	{
		mid = (low + high)/2;
		if( trigs[mid] < x )
		{
			if( trigs[mid+1] > x )
				break;
			low = mid + 1;
		}
		else
		{
			if( trigs[mid-1] <= x )
			{
				mid -= 1;
				break;
			}
			high = mid + 1;
		}
	}

	// now do a linear interpolation between those mid and mid+1
	// now return that value, or the next higher, whichever is the
	// best fit.
	d = (x - trigs[mid]) / (trigs[mid+1] - trigs[mid]);
	val = ((float)mid + d) * PI_RAD / 512.0f;

	return( nflag ? -val : val );
	
}

//////////////////////////////////////////////////////////////////////////////
int iacos( float x )
{
	return( 256 - iasin( x ));
}

//////////////////////////////////////////////////////////////////////////////
// normalize vector v
//////////////////////////////////////////////////////////////////////////////
void norm( vec3d *v )
{
	float m;
	
	m = rsqrt(((v->x * v->x) + (v->y * v->y) + (v->z * v->z)) / v->w);
	v->x *= m;
	v->y *= m;
	v->z *= m;
}


//////////////////////////////////////////////////////////////////////////////
// compute cross product of vectors v1 and v2
//////////////////////////////////////////////////////////////////////////////
void vxv( vec3d *v1, vec3d *v2, vec3d *v3 )
{	
	v3->x = v1->y * v2->z - v1->z * v2->y;
	v3->y = v1->z * v2->x - v1->x * v2->z;
	v3->z = v1->x * v2->y - v1->y * v2->x;
	v3->w = v1->w * v2->w;
}


//////////////////////////////////////////////////////////////////////////////
// compute magnitude of vector v
//////////////////////////////////////////////////////////////////////////////
float magv( vec3d *v )
{
	float f;

	f = v->x * v->x;
	f += v->y * v->y;
	f += v->z * v->z;

	f /= v->w;

	f = fsqrt( f );

	return f;
}

//////////////////////////////////////////////////////////////////////////////
// compute determinant of top left 3x3 of a 4x4 matrix
//////////////////////////////////////////////////////////////////////////////
float det( float *m )
{
	float d;

	d = m[0]*(m[5]*m[10]-m[6]*m[9]) -
		m[1]*(m[4]*m[10]-m[6]*m[8]) +
		m[2]*(m[4]*m[9]-m[5]*m[8]);

	return d;
}


//////////////////////////////////////////////////////////////////////////////
float cos_rad(float x)
{
static float result;
static float x_2;
static float x_4;
static float x_6;

	if(x < 0.0001 && x >= 0.0f)
		x = 0.0001;
	if(x > -0.0001f && x < 0.0f )
		x = 0.0001f;
	
	
	if(x >= 0 && x < PI_RAD*0.50f)
		{
		x_2 = x*x;    
		x_4 = x_2*x_2;    
		x_6 = x_4*x_2;    
		result = 1.0f - x_2*0.50f+x_4*0.04167f-x_6*0.00139f;
		return(result);
		}
	
	if(x >= PI_RAD*0.50f && x <= PI_RAD)
		{
	    x = PI_RAD-x;
	   	x_2 = x*x;    
		x_4 = x_2*x_2;    
		x_6 = x_4*x_2;    
		result = 1.0f - x_2*0.50f+x_4*0.04167f-x_6*0.00139f;
		return(-result);
		}
	
	if(x < 0 && x >= -PI_RAD*0.50f)
		{
	    x = -x;
		x_2 = x*x;    
		x_4 = x_2*x_2;    
		x_6 = x_4*x_2;    
		result = 1.0f - x_2*0.50f+x_4*0.04167f-x_6*0.00139f;
		return(result);
		}
	 
		/* Else, (x < -PI_RAD*0.50f && x >= -PI_RAD) */
	    x = -x;
	    x = PI_RAD-x;
		x_2 = x*x;    
		x_4 = x_2*x_2;    
		x_6 = x_4*x_2;    
		result = 1.0f - x_2*0.50f+x_4*0.04167f-x_6*0.00139f;
		return(-result);
} /* cosine() */

//////////////////////////////////////////////////////////////////////////////
float sin_rad(float x)
{
static float result;
static float x_2;
static float x_3;
static float x_5;
static float x_7;

	if(x < 0.0001f && x >= 0.0f )
		x = 0.0001f;
	
	if(x > -0.0001f && x < 0.0f )
		x = -0.0001f;
	
	
	if(x >= 0 && x < PI_RAD/2.0f)
		{
	    x_2=x*x;
	    x_3=x*x_2;
	    x_5=x_3*x_2;
	    x_7=x_5*x_2;
		result = x - x_3*0.166666f + x_5*0.0083333f - x_7*0.00020f;
		return(result);
		}
	
	if(x >= PI_RAD/2.0f && x <= PI_RAD)
		{
	    x = PI_RAD-x;
		x_2=x*x;
		x_3=x*x_2;
		x_5=x_3*x_2;
		x_7=x_5*x_2;
		result = x - x_3*0.166666f + x_5*0.0083333f - x_7*0.00020f;
		return(result);
		}
	
	if(x < 0 && x >= -PI_RAD/2.0f)
		{
	    x = -x;
		x_2=x*x;
		x_3=x*x_2;
		x_5=x_3*x_2;
		x_7=x_5*x_2;
		result = x - x_3*0.166666f + x_5*0.0083333f - x_7*0.00020f;
		return(-result);
		}
	
		/* Else, (x < -PI_RAD*0.50f && x >= -PI_RAD) */
	    x = -x;
	    x = PI_RAD-x;
		x_2=x*x;
		x_3=x*x_2;
		x_5=x_3*x_2;
		x_7=x_5*x_2;
		result = x - x_3*0.166666f + x_5*0.0083333f - x_7*0.00020f;
		return(-result);
}

//////////////////////////////////////////////////////////////////////////////
float acos_rad(float x)
{
	return(asin_rad( fsqrt(1.0f-x*x)));
}

//////////////////////////////////////////////////////////////////////////////
void rotx( int t, float *m )
{
	float ic, is;
	
	ic = icos(t);
	is = isin(t);
	
	m[0] = 1.0f;
	m[1] = 0.0f;
	m[2] = 0.0f;

	m[4] = 0.0f;
	m[5] = ic;
	m[6] = -is;

	m[8] = 0.0f;
	m[9] = is;
	m[10] = ic;
}

//////////////////////////////////////////////////////////////////////////////
void roty( int t, float *m )
{
	float ic, is;
	
	ic = icos(t);
	is = isin(t);
	
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
void rotz( int t, float *m )
{
	float ic, is;
	
	ic = icos(t);
	is = isin(t);
	
	m[0] = ic;
	m[1] = -is;
	m[2] = 0.0f;

	m[4] = is;
	m[5] = ic;
	m[6] = 0.0f;

	m[8] = 0.0f;
	m[9] = 0.0f;
	m[10] = 1.0f;
}

//////////////////////////////////////////////////////////////////////////////
void rotxyz( int x, int y, int z, float *m )
{
	float	xm[12],ym[12],zm[12], cm[12];

	xm[3] = 0.0f;	xm[7] = 0.0f;	xm[11] = 0.0f;
	ym[3] = 0.0f;	ym[7] = 0.0f;	ym[11] = 0.0f;
	zm[3] = 0.0f;	zm[7] = 0.0f;	zm[11] = 0.0f;

	rotx( x, xm );
	roty( y, ym );
	rotz( z, zm );

	mxm( zm, ym, cm );
	mxm( cm, xm, m );
}

//////////////////////////////////////////////////////////////////////////////
