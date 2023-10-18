/*****************************************************************************/
/*                                                                           */
/* FILE: geom.c()                                                            */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 29 Dec 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

/*** INCLUDES: ***************************************************************/

#include <stdio.h>

#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif

#include "ani3D.h"
#include "ani3Di.h"
#include "game.h"
#include "player.h"
#include "srmath.h"
#include "mainproc.h"
#include "clip.h"


/*** DEFINES: ****************************************************************/

//#define GEOM_STATS 1



#define MIN_AREA_CMP 0.00001f


#undef HRES
#undef VRES

#define HRES	512.0f
#define VRES	384.0f
#define VRESLOW	256.0f

#define HRES2		0.5f*HRES
#define VRES2		0.5f*VRES
#define VRES2LOW	0.5f*VRESLOW

extern	int	is_low_res;



int bbox_mode = TRUE;

extern int global_dog;

extern ReducedVertex		*tmp_grv;


float geom_identity_matrix[] = { 1.0f, 0.0f, 0.0f, 0.0f,
 								 0.0f, 1.0f, 0.0f, 0.0f,
 								 0.0f, 0.0f, 1.0f, 0.0f,
 								 0.0f, 0.0f, 0.0f, 1.0f};
 
ReducedVertex bbox[8];
unsigned short bbox_opcode[8];

int (* object_clip)(jonhey_t *obj);
void (* transform_verts)(jonhey_t *obj);

void grDrawStripReduced(ReducedVertex **a, int num_verts);

ReducedVertex *strip_verts_array[100];


#ifdef GEOM_STATS

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

stats_t		stats;

#endif



void *grPreflush(void);
void grDrawSimpleTrianglePreflushed(void *, const void *, const void *, const void *);



/*** PROTOS: *****************************************************************/
void draw_tris_strip_noclip( jonhey_t *obj, ReducedVertex *vsrc);
void draw_tris_strip_noclip2( jonhey_t *obj, ReducedVertex *vsrc);
void draw_tris_strip_noclip3( jonhey_t *obj, ReducedVertex *vsrc);
void draw_tris_strip_clip( jonhey_t *obj, ReducedVertex *vsrc);


																	 

void draw_tris_clip( jonhey_t *obj, ReducedVertex *vsrc);
void draw_tris_ds_clip( jonhey_t *obj, ReducedVertex *vsrc);


void draw_tris_noclip( jonhey_t *obj, ReducedVertex *vsrc);
void draw_tris_ds_noclip( jonhey_t *obj, ReducedVertex *vsrc);

void turn_off_bbox (void);
void turn_on_bbox (void);

//void guDrawTriangleWithClip(ReducedVertex *, ReducedVertex *, ReducedVertex *);
void check_vertex_indices(LIMB *limb);
void convert_2_jonhey (jonhey_t *jh_obj, LIMB *model, float *matrix, 
					   Texture_node_t **decals);
void transform_verts_med (jonhey_t *obj);
void transform_verts_low (jonhey_t *obj);
int object_clip_med (jonhey_t *obj);
int object_clip_low (jonhey_t *obj);
void recalculate_bbox(LIMB *limb);
void init_crowd_bbox_sub(crowd_bbox_t *crowd, LIMB **limbs);
int check_inside_screen (void);





#define	recip(x) \
({ float	__value, __arg = (x); \
	asm("recip.s	%0,%1": "=f" (__value): "f" (__arg)); \
	__value; })


/*** CODE: *******************************************************************/



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: draw_jh_object()                                                */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 8 Jan 99 EJK                                                              */
/*                                                                           */
/*****************************************************************************/

void draw_jh_object(jonhey_t *obj)
{
	
int status;

/*****************************************************************************/

	/* Do the bounding box test? */
	status = object_clip(obj); /* do the bbox test */


	#ifdef GEOM_STATS
	stats.tris_processed  += obj->limb->ntris;
	stats.verts_processed += obj->limb->nvtx;


	if (obj->limb->strip != NULL)
		{
		stats.strips_processed += obj->limb->strip->num_strip_info;
		}
	#endif

	/* 1) Bbox is completely outside the view port */
	/* Stop the train, go to next object.          */	
	if(status == TRIVIAL_REJECTED)
		{
		return;
		}


	#ifdef GEOM_STATS
	stats.tris_bbox_processed += obj->limb->ntris;
	stats.verts_bbox_processed += obj->limb->nvtx;

	if (obj->limb->strip != NULL)
		{
		stats.strips_bbox_processed += obj->limb->strip->num_strip_info;
		}
	#endif



	/* 2) Bbox is completely inside the view port  */	
	/* Transform the object and draw with NO CLIP. */
	//else if (status == TRIVIAL_ACCEPTED)
	if (status == TRIVIAL_ACCEPTED)
	{
	transform_verts(obj);

	if (obj->mode & JONHEY_MODE_DBL_SIDED)
		draw_tris_ds_noclip( obj,tmp_grv);
	else
		{
  		if (obj->limb->strip == NULL)
			{
			draw_tris_noclip( obj,tmp_grv);
			}
		else
			{
			draw_tris_strip_noclip3( obj, tmp_grv);
			}								 
		}											  
	}

	/* 3) Bbox is partially inside the view port  */	
	/* Transform the object and draw with CLIP.  */
	
	else
	{
	transform_verts(obj);

	#ifdef GEOM_STATS
	stats.tris_clip_processed += obj->limb->ntris;
	#endif


	if (obj->mode & JONHEY_MODE_DBL_SIDED)
		draw_tris_ds_clip( obj,tmp_grv);
	else
		{
  		if (obj->limb->strip == NULL)
			{
			draw_tris_clip( obj,tmp_grv);
			}
  		else
			{
  			draw_tris_strip_clip( obj, tmp_grv);
			}
		}

	}

}

/***** End of draw_jh_object() ***********************************************/



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: object_clip_med()                                               */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 8 Jan 99 EJK                                                              */
/*                                                                           */
/*****************************************************************************/

int object_clip_med (jonhey_t * obj)

{

ReducedVertex *scratch_ptr;
LIMB *limb;
VERTEX *vert;
float *m;

float x1;
float y1;
float z1;

unsigned short i;
unsigned short opcode;
unsigned short opcode1;
unsigned short opcode2;

/*****************************************************************************/  

	limb = obj->limb;
	m = obj->m;
    
    /*******************************************************************/
    /* Transform and Snap Verts to 0.25.                               */
    /*******************************************************************/
    
    /* Store these transform verts into scratch memory, this will      */
    /* allow the vertices to be processed on a per-polygon basis,      */
    /* without transforming shared vertices multiple times.            */
   

	/* Safety check */
	if (limb->bbox == NULL)
		{
	//	printf("NO BOUNDING BOX\n");
		return ( CLIP_OBJECT );
		}


	#ifdef GEOM_STATS
	stats.bbox_verts	  += 8;
	#endif

	/* Transform all 8 points in the bbox to view space */
    for (i=0; i < 8; i++)
        {

        vert = &limb->bbox[i];
        scratch_ptr = &bbox[i];

		x1 = vert->x;
        y1 = vert->y;
        z1 = vert->z;

        scratch_ptr->oow  =  1.0f / (m[8]*x1 + m[9]*y1 + m[10]*z1 + m[11]);
        scratch_ptr->x  = (m[0]*x1 + m[1]*y1 + m[2]*z1 + m[3]);
        scratch_ptr->y  = (m[4]*x1 + m[5]*y1 + m[6]*z1 + m[7]);
        scratch_ptr->x  *= scratch_ptr->oow;
        scratch_ptr->y  *= scratch_ptr->oow;
        scratch_ptr->x  += HRES2;
        scratch_ptr->y  += VRES2;

        }                


	#ifdef GEOM_STATS
	stats.bbox_verts_transformed  += 8;
	#endif 

	/* Test all 8 points against the view planes */
	for (i=0; i<8; i++)
		{
		opcode = 0x0;


		if ( bbox[i].oow <= CLIP_WMAX )  /* in front of the camera */
			{
			opcode |= 0x10;
			bbox[i].y = -bbox[i].y;
			bbox[i].x = -bbox[i].x;
			}

		if ( bbox[i].y > CLIP_YMAX + GUTTER )  /* above the screen */
			opcode |= 0x1;

		else if ( bbox[i].y < CLIP_YMIN - GUTTER)  /* below the screen */
			opcode |= 0x2;

		if ( bbox[i].x > CLIP_XMAX + GUTTER )  /* right edge of screen */
			opcode |= 0x4;

		else if ( bbox[i].x < CLIP_XMIN - GUTTER )  /* left edge of screen */
			opcode |= 0x8;

		bbox_opcode[i] = opcode;

		}

	/* set the opcode to the first element */
	opcode1 = bbox_opcode[0] & bbox_opcode[1] & bbox_opcode[2] & bbox_opcode[3] &
			  bbox_opcode[4] & bbox_opcode[5] & bbox_opcode[6] & bbox_opcode[7];

	opcode2 = bbox_opcode[0] | bbox_opcode[1] | bbox_opcode[2] | bbox_opcode[3] |
			  bbox_opcode[4] | bbox_opcode[5] | bbox_opcode[6] | bbox_opcode[7];


	/* all 8 points are completely inside the view port */
	if ( opcode2 == 0x0 )
		{
		return ( TRIVIAL_ACCEPTED );
		}

	/* all 8 points are completely outside the view port */
	if ( opcode1 != 0x0 )
		{
		return ( TRIVIAL_REJECTED );
		}

	/* points are partially inside the view port */
	else
		{
		return ( CLIP_OBJECT );
		}
}

/***** END of object_clip_med() **********************************************/




/*****************************************************************************/
/*                                                                           */
/* FUNCTION: object_clip_low()                                               */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 8 Jan 99 EJK                                                              */
/*                                                                           */
/*****************************************************************************/

int object_clip_low (jonhey_t * obj)

{

ReducedVertex *scratch_ptr;
LIMB *limb;
VERTEX *vert;
float *m;

float x1;
float y1;
float z1;

unsigned short i;
unsigned short opcode;
unsigned short opcode1;
unsigned short opcode2;

/*****************************************************************************/  

	limb = obj->limb;
	m = obj->m;
    
    /*******************************************************************/
    /* Transform and Snap Verts to 0.25.                               */
    /*******************************************************************/
    
    /* Store these transform verts into scratch memory, this will      */
    /* allow the vertices to be processed on a per-polygon basis,      */
    /* without transforming shared vertices multiple times.            */
   

	/* Safety check */
	if (limb->bbox == NULL)
		{
	//	printf("NO BOUNDING BOX\n");
		return ( CLIP_OBJECT );
		}


	#ifdef GEOM_STATS
	stats.bbox_verts	  += 8;
	#endif

	/* Transform all 8 points in the bbox to view space */
    for (i=0; i < 8; i++)
        {

        vert = &limb->bbox[i];
        scratch_ptr = &bbox[i];

		x1 = vert->x;
        y1 = vert->y;
        z1 = vert->z;

        scratch_ptr->oow  =  1.0f / (m[8]*x1 + m[9]*y1 + m[10]*z1 + m[11]);
        scratch_ptr->x  = (m[0]*x1 + m[1]*y1 + m[2]*z1 + m[3]);
        scratch_ptr->y  = (m[4]*x1 + m[5]*y1 + m[6]*z1 + m[7]);
        scratch_ptr->x  *= scratch_ptr->oow;
        scratch_ptr->y  *= scratch_ptr->oow;
        scratch_ptr->x  += HRES2;
        scratch_ptr->y  += VRES2LOW;

        }                


	#ifdef GEOM_STATS
	stats.bbox_verts_transformed  += 8;
	#endif 

	/* Test all 8 points against the view planes */
	for (i=0; i<8; i++)
		{
		opcode = 0x0;


		if ( bbox[i].oow <= CLIP_WMAX )  /* in front of the camera */
			{
			opcode |= 0x10;
			bbox[i].y = -bbox[i].y;
			bbox[i].x = -bbox[i].x;
			}

		if ( bbox[i].y > CLIP_YMAX + GUTTER )  /* above the screen */
			opcode |= 0x1;

		else if ( bbox[i].y < CLIP_YMIN - GUTTER)  /* below the screen */
			opcode |= 0x2;

		if ( bbox[i].x > CLIP_XMAX + GUTTER )  /* right edge of screen */
			opcode |= 0x4;

		else if ( bbox[i].x < CLIP_XMIN - GUTTER )  /* left edge of screen */
			opcode |= 0x8;

		bbox_opcode[i] = opcode;

		}

	/* set the opcode to the first element */
	opcode1 = bbox_opcode[0] & bbox_opcode[1] & bbox_opcode[2] & bbox_opcode[3] &
			  bbox_opcode[4] & bbox_opcode[5] & bbox_opcode[6] & bbox_opcode[7];

	opcode2 = bbox_opcode[0] | bbox_opcode[1] | bbox_opcode[2] | bbox_opcode[3] |
			  bbox_opcode[4] | bbox_opcode[5] | bbox_opcode[6] | bbox_opcode[7];


	/* all 8 points are completely inside the view port */
	if ( opcode2 == 0x0 )
		{
		return ( TRIVIAL_ACCEPTED );
		}

	/* all 8 points are completely outside the view port */
	if ( opcode1 != 0x0 )
		{
		return ( TRIVIAL_REJECTED );
		}

	/* points are partially inside the view port */
	else
		{
		return ( CLIP_OBJECT );
		}
}

/***** END of object_clip_low() **********************************************/




/*****************************************************************************/
/*                                                                           */
/* FUNCTION: transform_verts_med()                                           */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 06 Jan 99 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void transform_verts_med ( jonhey_t * obj ) 
{
register ReducedVertex *scratch_ptr __asm__("$8");
register float *m __asm__("$9");
register VERTEX *vert __asm__("$10");
register int i __asm__("$12");
register LIMB *limb __asm("$11");

register float x1;
register float y1;
register float z1;
register float x2;
register float y2;
/*****************************************************************************/


	limb = obj->limb;
	m = obj->m;

	/*******************************************************************/
	/* Transform and Snap Verts to 0.25.                               */
	/*******************************************************************/

	/* Store these transform verts into vert_scratch memory, this will */
	/* allow the vertices to be processed on a per-polygon basis,      */
	/* without transforming shared vertices multiple times.            */
   
	/* transform all of the verts in the object */
	/* and save them to scratch */

	vert = &limb->pvtx[0];
	scratch_ptr = &tmp_grv[0];
	i = limb->nvtx;

	#ifdef GEOM_STATS
	stats.verts_transformed += i;
	#endif

	while(i--)
	{
		x1 = vert->x;
		y1 = vert->y;
		z1 = vert->z;
		
		scratch_ptr->oow  =  recip(m[8]*x1 + m[9]*y1 + m[10]*z1 + m[11]);
		++vert;
		++scratch_ptr;
	}
    
	vert = &limb->pvtx[0];
	scratch_ptr = &tmp_grv[0];
	i = limb->nvtx;
	while(i--)
	{
		x1 = vert->x;
		y1 = vert->y;
		z1 = vert->z;

		x2  = (m[0]*x1 + m[1]*y1 + m[2]*z1 + m[3]);
		y2  = (m[4]*x1 + m[5]*y1 + m[6]*z1 + m[7]);
		x2  *= scratch_ptr->oow;
		y2  *= scratch_ptr->oow;
		scratch_ptr->x  = x2 + HRES2;
		scratch_ptr->y  = y2 + VRES2;
		++vert;
		++scratch_ptr;
	}                
} /* transform_verts_med() */
 
/***** END of transform_verts_med() *******************************************/




/*****************************************************************************/
/*                                                                           */
/* FUNCTION: transform_verts_low()                                           */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 06 Jan 99 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void transform_verts_low (jonhey_t * obj) 
{
register ReducedVertex *scratch_ptr __asm__("$8");
register float *m __asm__("$9");
register VERTEX *vert __asm__("$10");
register int i __asm__("$12");
register LIMB *limb __asm("$11");

register float x1;
register float y1;
register float z1;
register float x2;
register float y2;
/*****************************************************************************/


	limb = obj->limb;
	m = obj->m;

	/*******************************************************************/
	/* Transform and Snap Verts to 0.25.                               */
	/*******************************************************************/

	/* Store these transform verts into vert_scratch memory, this will */
	/* allow the vertices to be processed on a per-polygon basis,      */
	/* without transforming shared vertices multiple times.            */
   
	/* transform all of the verts in the object */
	/* and save them to scratch */

	vert = &limb->pvtx[0];
	scratch_ptr = &tmp_grv[0];
	i = limb->nvtx;

	#ifdef GEOM_STATS
	stats.verts_transformed += i;
	#endif

	while(i--)
	{
		x1 = vert->x;
		y1 = vert->y;
		z1 = vert->z;
		
		scratch_ptr->oow  =  recip(m[8]*x1 + m[9]*y1 + m[10]*z1 + m[11]);
		++vert;
		++scratch_ptr;
	}
    
	vert = &limb->pvtx[0];
	scratch_ptr = &tmp_grv[0];
	i = limb->nvtx;
	while(i--)
	{
		x1 = vert->x;
		y1 = vert->y;
		z1 = vert->z;

		x2  = (m[0]*x1 + m[1]*y1 + m[2]*z1 + m[3]);
		y2  = (m[4]*x1 + m[5]*y1 + m[6]*z1 + m[7]);
		x2  *= scratch_ptr->oow;
		y2  *= scratch_ptr->oow;
		scratch_ptr->x  = x2 + HRES2;
		scratch_ptr->y  = y2 + VRES2LOW;
		++vert;
		++scratch_ptr;
	}                
} /* transform_verts_low() */
 
/***** END of transform_verts_low() *******************************************/




/*****************************************************************************/
/*                                                                           */
/* FUNCTION: init_geom()                                                     */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 29 Dec 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void init_geom( int lo_res )
{
	if (lo_res)
	{
		object_clip = object_clip_low;
		transform_verts = transform_verts_low;
	}
	else
	{
		object_clip = object_clip_med;
		transform_verts = transform_verts_med;
	}
}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: draw_tris_clip()                                                */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 29 Dec 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void draw_tris_clip( jonhey_t *obj, ReducedVertex *vsrc)
{
register ReducedVertex	*v1;
register ReducedVertex  *v2;
register ReducedVertex  *v3;
register int   count;
register TRI   *ptri;
register ST	   *pst;
register int   t;
register float ooa;
register float oow;
register Texture_node_t **texture;
register LIMB *limb;

/*****************************************************************************/

	texture = obj->text;
	limb = obj->limb;
	count = limb->ntris;
	ptri = limb->ptri;
	pst = limb->pst;

	while(count--)
	{
		if(ptri->texture >= 0 )
		{
			guTexSource( texture[ptri->texture] );
		}

		v1 = &vsrc[ptri->v1];
		v2 = &vsrc[ptri->v2];
		v3 = &vsrc[ptri->v3];

		/* Only software cull if all points have positive oow's */
		if( v1->oow > 0.0f &&  v2->oow > 0.0f && v3->oow > 0.0f)
			{
			ooa = (v1->x - v2->x) * (v2->y - v3->y) - (v2->x - v3->x) * (v1->y - v2->y);

			if (ooa > 0.0f)
				{
				ptri++;
				continue;
				}
			}

		oow = v1->oow;
		t = ptri->t1;
		v1->sow = oow * pst[t].s;
		v1->tow = oow * pst[t].t;
		oow = v2->oow;
		t = ptri->t2;
		v2->sow = oow * pst[t].s;
		v2->tow = oow * pst[t].t;
		oow = v3->oow;
		t = ptri->t3;
		v3->sow = oow * pst[t].s;
		v3->tow = oow * pst[t].t;
		
		#ifdef GEOM_STATS
		stats.tris_drawn  += 1;
		stats.verts_drawn += 3;
		stats.tris_clipped += 1;
		#endif
		
		clip_triangle_t(v3, v2, v1);

		ptri++;

	}
}
/***** End of draw_tris_clip() ***********************************************/





/*****************************************************************************/
/*                                                                           */
/* FUNCTION: draw_tris_strip_clip()                                          */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 29 Dec 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void draw_tris_strip_clip( jonhey_t *obj, ReducedVertex *vsrc)
{
ReducedVertex	*va;
ReducedVertex	*vb;
ReducedVertex	*vc;
int   count;
ST	   *pst;
ST	   *tmp_pst;

float oow;
void  *gc;
LIMB           *limb;
Texture_node_t **texture;
STRIP_VERTS    *strip_v;
STRIP_INFO     *strip_i;
STRIP          *strip;
int i;
int j;
int num_strip_info;
float ooa;

/*****************************************************************************/

	texture = obj->text;
	limb = obj->limb;
	pst = limb->pst;
	strip = limb->strip;
	strip_v = strip->strip_verts;
	strip_i = strip->strip_info;
	num_strip_info = strip->num_strip_info -1;
	
	gc = grPreflush();

	for (i=0; i < num_strip_info; i++)
		{
		if(strip_i[i].text >= 0 )
			{
			guTexSource( texture[(strip_i[i].text)] );
			gc = grPreflush();
			}

		count = strip_i[i].num_verts-2;

		va = &vsrc[(strip_v->v)];
		tmp_pst = &pst[(strip_v->st)];
		oow = va->oow;
		va->sow = oow * tmp_pst->s;
		va->tow = oow * tmp_pst->t;

		strip_v++;

		vb = &vsrc[(strip_v->v)];
		tmp_pst = &pst[(strip_v->st)];
		oow = vb->oow;
		vb->sow = oow * tmp_pst->s;
		vb->tow = oow * tmp_pst->t;

		strip_v++;

		for (j=0; j < count; j++)
			{

			vc = &vsrc[(strip_v->v)];
			tmp_pst = &pst[(strip_v->st)];
			oow = vc->oow;
			vc->sow = oow * tmp_pst->s;
			vc->tow = oow * tmp_pst->t;

			strip_v++;
			
			if ((j & 0x1) != 0)
				{
				/* Only software cull if all points have positive oow's */
				if( va->oow > 0.0f &&  vb->oow > 0.0f && vc->oow > 0.0f)
					{
					ooa = (va->x - vb->x) * (vb->y - vc->y) - (vb->x - vc->x) * (va->y - vb->y);
					if(ooa < 0.0f)
						{
						clip_triangle_t(va,vc,vb);
						#ifdef GEOM_STATS
						stats.tris_drawn  += 1;
						stats.verts_drawn += 3;
						stats.tris_clipped += 1;
						#endif
						}
					}
				else
					{
					clip_triangle_t(va,vc,vb);
					#ifdef GEOM_STATS
					stats.tris_drawn  += 1;
					stats.verts_drawn += 3;
					stats.tris_clipped += 1;
					#endif
					}
				}
  			else
  				{

				/* Only software cull if all points have positive oow's */
				if( va->oow > 0.0f &&  vb->oow > 0.0f && vc->oow > 0.0f)
					{
					ooa = (va->x - vb->x) * (vb->y - vc->y) - (vb->x - vc->x) * (va->y - vb->y);
					if(ooa > 0.0f)
						{
		  				clip_triangle_t(va,vb,vc);
						#ifdef GEOM_STATS
						stats.tris_drawn  += 1;
						stats.verts_drawn += 3;
						stats.tris_clipped += 1;
						#endif
						}
					}
				else
					{
	  				clip_triangle_t(va,vb,vc);
					#ifdef GEOM_STATS
					stats.tris_drawn  += 1;
					stats.verts_drawn += 3;
					stats.tris_clipped += 1;
					#endif
					}
				}

			va = vb;
			vb = vc;

			} /* for(j) */

		} /* for(i) */

}
/***** End of draw_tris_strip_clip() *****************************************/







/*****************************************************************************/
/*                                                                           */
/* FUNCTION: draw_tris_strip_noclip2()                                       */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 29 Dec 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void draw_tris_strip_noclip2( jonhey_t *obj, ReducedVertex *vsrc)
{
ReducedVertex	*va;
ReducedVertex	*vb;
ReducedVertex	*vc;
int   count;
ST	   *pst;
ST	   *tmp_pst;

float oow;
void  *gc;
LIMB           *limb;
Texture_node_t **texture;
STRIP_VERTS    *strip_v;
STRIP_INFO     *strip_i;
STRIP          *strip;
int i;
int j;
int num_strip_info;
float ooa;

/*****************************************************************************/

	texture = obj->text;
	limb = obj->limb;
	pst = limb->pst;
	strip = limb->strip;
	strip_v = strip->strip_verts;
	strip_i = strip->strip_info;
	num_strip_info = strip->num_strip_info -1;
	
	gc = grPreflush();

	for (i=0; i < num_strip_info; i++)
		{
		if(strip_i[i].text >= 0 )
			{
			guTexSource( texture[(strip_i[i].text)] );
			gc = grPreflush();
			}

		count = strip_i[i].num_verts-2;

		va = &vsrc[(strip_v->v)];
		tmp_pst = &pst[(strip_v->st)];
		oow = va->oow;
		va->sow = oow * tmp_pst->s;
		va->tow = oow * tmp_pst->t;

		strip_v++;

		vb = &vsrc[(strip_v->v)];
		tmp_pst = &pst[(strip_v->st)];
		oow = vb->oow;
		vb->sow = oow * tmp_pst->s;
		vb->tow = oow * tmp_pst->t;

		strip_v++;

		for (j=0; j < count; j++)
			{

			vc = &vsrc[(strip_v->v)];
			tmp_pst = &pst[(strip_v->st)];
			oow = vc->oow;
			vc->sow = oow * tmp_pst->s;
			vc->tow = oow * tmp_pst->t;

			strip_v++;
			
			/* Its assumed that all points have positive oow's determined by */
			/* the bounding box check */
			ooa = (va->x - vb->x) * (vb->y - vc->y) - (vb->x - vc->x) * (va->y - vb->y);

			if ((j & 0x1) != 0)
				{
				if(ooa < 0.0f)
					{
				   	grDrawSimpleTrianglePreflushed(gc, va,vc,vb);

					#ifdef GEOM_STATS
					stats.tris_drawn  += 1;
					stats.verts_drawn += 3;
					#endif
					}

				}
  			else
  				{
				if(ooa > 0.0f)
					{
	  				grDrawSimpleTrianglePreflushed(gc, va,vb,vc);
					#ifdef GEOM_STATS
					stats.tris_drawn  += 1;
					stats.verts_drawn += 3;
					#endif
					}
  				}

			va = vb;
			vb = vc;
			}
		}

}
/***** End of draw_tris_strip_noclip2() *****************************************/



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: draw_tris_strip_noclip3()                                        */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 29 Dec 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void draw_tris_strip_noclip3( jonhey_t *obj, ReducedVertex *vsrc)
{
ReducedVertex	*va;
ReducedVertex	*vb;
ReducedVertex	*vc;
ReducedVertex	**strip_verts;


int   count;
ST	   *pst;
ST	   *tmp_pst;

float oow;
float ooa;
void  *gc;
LIMB           *limb;
Texture_node_t **texture;
STRIP_VERTS    *strip_v;
STRIP_INFO     *strip_i;
STRIP          *strip;
int i;										 
int j;
int num_strip_info;
int cull_passed;

/*****************************************************************************/

	texture = obj->text;
	limb = obj->limb;			    
	pst = limb->pst;
	strip = limb->strip;
	strip_v = strip->strip_verts;
	strip_i = strip->strip_info;
	num_strip_info = strip->num_strip_info -1;
	
	grCullMode(GR_CULL_NEGATIVE);

	gc = grPreflush();

	for (i=0; i < num_strip_info; i++)
		{
		if(strip_i[i].text >= 0 )
			{
			guTexSource( texture[(strip_i[i].text)] );
  			gc = grPreflush();
			}


		count = strip_i[i].num_verts-2;
		strip_verts = strip_verts_array;

		va = &vsrc[(strip_v->v)];
		tmp_pst = &pst[(strip_v->st)];
		oow = va->oow;
		va->sow = oow * tmp_pst->s;
		va->tow = oow * tmp_pst->t;

		*strip_verts++ = va;

		strip_v++;

		vb = &vsrc[(strip_v->v)];
		tmp_pst = &pst[(strip_v->st)];
		oow = vb->oow;
		vb->sow = oow * tmp_pst->s;
		vb->tow = oow * tmp_pst->t;

		*strip_verts++ = vb;

		strip_v++;

		cull_passed = 0;
		for (j=0; j < count; j++)
			{

			vc = &vsrc[(strip_v->v)];
			tmp_pst = &pst[(strip_v->st)];
			oow = vc->oow;
			vc->sow = oow * tmp_pst->s;
			vc->tow = oow * tmp_pst->t;
			
			if (!cull_passed)
				{
				/* Its assumed that all points have positive oow's determined by */
				/* the bounding box check */
				ooa = (va->x - vb->x) * (vb->y - vc->y) - (vb->x - vc->x) * (va->y - vb->y);

				if ((j & 0x1) != 0)
					{
					if(ooa < 0.0f)
						{
						cull_passed = 1;
						}
					}
  				else
  					{
					if(ooa > 0.0f)
						{
						cull_passed = 1;
						}
  					}
	
				va = vb;
				vb = vc;
				}

			*strip_verts++ = vc;
			strip_v++;

			}

		if (cull_passed)
			{
			grDrawStripReduced(strip_verts_array,strip_i[i].num_verts);

			#ifdef GEOM_STATS
			stats.tris_drawn   += (strip_i[i].num_verts-2);
			stats.verts_drawn  += strip_i[i].num_verts;
			stats.strips_drawn += 1;
			#endif


			}

		}

	grCullMode(GR_CULL_NEGATIVE);

}
/***** End of draw_tris_strip_noclip3() ***************************************/



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: draw_tris_strip_noclip()                                        */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 29 Dec 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void draw_tris_strip_noclip( jonhey_t *obj, ReducedVertex *vsrc)
{
ReducedVertex	*va;
ReducedVertex	*vb;
ReducedVertex	*vc;
ReducedVertex	**strip_verts;

int   count;
ST	   *pst;
ST	   *tmp_pst;

float oow;
void  *gc;
LIMB           *limb;
Texture_node_t **texture;
STRIP_VERTS    *strip_v;
STRIP_INFO     *strip_i;
STRIP          *strip;
int i;										 
int j;
int num_strip_info;

/*****************************************************************************/

	texture = obj->text;
	limb = obj->limb;
	pst = limb->pst;
	strip = limb->strip;
	strip_v = strip->strip_verts;
	strip_i = strip->strip_info;
	num_strip_info = strip->num_strip_info -1;
	

	grCullMode(GR_CULL_NEGATIVE);

	gc = grPreflush();

	for (i=0; i < num_strip_info; i++)
		{
		if(strip_i[i].text >= 0 )
			{
			guTexSource( texture[(strip_i[i].text)] );
  			gc = grPreflush();
			}

		count = strip_i[i].num_verts-2;
		strip_verts = strip_verts_array;

		va = &vsrc[(strip_v->v)];
		tmp_pst = &pst[(strip_v->st)];
		oow = va->oow;
		va->sow = oow * tmp_pst->s;
		va->tow = oow * tmp_pst->t;

		*strip_verts++ = va;

		strip_v++;

		vb = &vsrc[(strip_v->v)];
		tmp_pst = &pst[(strip_v->st)];
		oow = vb->oow;
		vb->sow = oow * tmp_pst->s;
		vb->tow = oow * tmp_pst->t;

		*strip_verts++ = vb;

		strip_v++;

		for (j=0; j < count; j++)
			{

			vc = &vsrc[(strip_v->v)];
			tmp_pst = &pst[(strip_v->st)];
			oow = vc->oow;
			vc->sow = oow * tmp_pst->s;
			vc->tow = oow * tmp_pst->t;

			*strip_verts++ = vc;
			strip_v++;
			}

		grDrawStripReduced(strip_verts_array,strip_i[i].num_verts);

		#ifdef GEOM_STATS
		stats.tris_drawn   += (strip_i[i].num_verts-2);
		stats.verts_drawn  += strip_i[i].num_verts;
		stats.strips_drawn += 1;
		#endif

		}

	grCullMode(GR_CULL_NEGATIVE);

}
/***** End of draw_tris_strip_noclip() ***************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: draw_tris_noclip()                                              */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 29 Dec 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void draw_tris_noclip( jonhey_t *obj, ReducedVertex *vsrc)
{
register ReducedVertex	*v1;
register ReducedVertex	*v2;
register ReducedVertex	*v3;
register int   count;
register TRI   *ptri;
register ST	   *pst;
register int   t;
register float ooa;
register float oow;
register void  *gc;
register LIMB *limb;
register Texture_node_t **texture;
/*****************************************************************************/

	texture = obj->text;
	limb = obj->limb;
	count = limb->ntris;
	ptri = limb->ptri;
	pst = limb->pst;

	gc = grPreflush();

	while(count--)
	{
		if(ptri->texture >= 0 )
		{
			guTexSource( texture[ptri->texture] );
			gc = grPreflush();
		}

		v1 = &vsrc[ptri->v1];						    
		v2 = &vsrc[ptri->v2];
		v3 = &vsrc[ptri->v3];

		/* Its assumed that all points have positive oow's determined by */
		/* the bounding box check */
		ooa = (v1->x - v2->x) * (v2->y - v3->y) - (v2->x - v3->x) * (v1->y - v2->y);

		if(ooa < 0.0f)
		{
			oow = v1->oow;
			t = ptri->t1;
			v1->sow = oow * pst[t].s;
			v1->tow = oow * pst[t].t;
			oow = v2->oow;
			t = ptri->t2;
			v2->sow = oow * pst[t].s;
			v2->tow = oow * pst[t].t;
			oow = v3->oow;
			t = ptri->t3;
			v3->sow = oow * pst[t].s;
			v3->tow = oow * pst[t].t;

			grDrawSimpleTrianglePreflushed(gc, v3,v2,v1);

			#ifdef GEOM_STATS
			stats.tris_drawn   += 1;
			stats.verts_drawn  += 3;
			#endif

		}
		ptri++;
	}
}
/***** End of draw_tris_noclip() *********************************************/



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: draw_tris_ds_noclip()                                           */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 29 Dec 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void draw_tris_ds_noclip( jonhey_t *obj, ReducedVertex *vsrc)
{
register ReducedVertex	*v1;
register ReducedVertex	*v2;
register ReducedVertex	*v3;
register int   count;
register TRI   *ptri;
register ST	   *pst;
register int   t;
register float oow;
register void  *gc;
register LIMB *limb;
register Texture_node_t **texture;
/*****************************************************************************/

	texture = obj->text;
	limb = obj->limb;
	count = limb->ntris;
	ptri = limb->ptri;
	pst = limb->pst;

	grCullMode(GR_CULL_DISABLE);
	gc = grPreflush();

	while(count--)
	{
		if(ptri->texture >= 0 )
		{
			guTexSource( texture[ptri->texture] );
			gc = grPreflush();
		}

		v1 = &vsrc[ptri->v1];
		v2 = &vsrc[ptri->v2];
		v3 = &vsrc[ptri->v3];

		oow = v1->oow;
		t = ptri->t1;
		v1->sow = oow * pst[t].s;
		v1->tow = oow * pst[t].t;
		oow = v2->oow;
		t = ptri->t2;
		v2->sow = oow * pst[t].s;
		v2->tow = oow * pst[t].t;
		oow = v3->oow;
		t = ptri->t3;
		v3->sow = oow * pst[t].s;
		v3->tow = oow * pst[t].t;

		grDrawSimpleTrianglePreflushed(gc, v3,v2,v1);

		#ifdef GEOM_STATS
		stats.tris_drawn   += 1;
		stats.verts_drawn  += 3;
		#endif

		ptri++;
	}

	grCullMode(GR_CULL_NEGATIVE);
	gc = grPreflush();

}
/***** End of draw_tris_ds_noclip() ******************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: draw_tris_ds_clip()                                             */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 29 Dec 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void draw_tris_ds_clip( jonhey_t *obj, ReducedVertex *vsrc)
{
register ReducedVertex	*v1;
register ReducedVertex  *v2;
register ReducedVertex  *v3;
register int   count;
register TRI   *ptri;
register ST	   *pst;
register int   t;
register float oow;
register Texture_node_t **texture;
register LIMB *limb;
register void  *gc;

/*****************************************************************************/

	texture = obj->text;
	limb = obj->limb;
	count = limb->ntris;
	ptri = limb->ptri;
	pst = limb->pst;

	grCullMode(GR_CULL_DISABLE);
	gc = grPreflush();


	while(count--)
	{
		if(ptri->texture >= 0 )
		{
			guTexSource( texture[ptri->texture] );
		}

		v1 = &vsrc[ptri->v1];
		v2 = &vsrc[ptri->v2];
		v3 = &vsrc[ptri->v3];

		oow = v1->oow;
		t = ptri->t1;
		v1->sow = oow * pst[t].s;
		v1->tow = oow * pst[t].t;
		oow = v2->oow;
		t = ptri->t2;
		v2->sow = oow * pst[t].s;
		v2->tow = oow * pst[t].t;
		oow = v3->oow;
		t = ptri->t3;
		v3->sow = oow * pst[t].s;
		v3->tow = oow * pst[t].t;

		clip_triangle_t(v3, v2, v1);

		#ifdef GEOM_STATS
		stats.tris_drawn   += 1;
		stats.tris_clipped   += 1;
		stats.verts_drawn  += 3;
		#endif

		ptri++;
	}

	grCullMode(GR_CULL_NEGATIVE);
	gc = grPreflush();


}
/***** End of draw_tris_ds_clip() ********************************************/





/*****************************************************************************/
/*                                                                           */
/* FUNCTION: check_vertex_indices()                                          */
/*                                                                           */
/* Check if the vertices are a multiple of 3. If they are fix it.  This      */
/* is only a safety net. It should be corrected in the art tools.            */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 29 Dec 98 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void check_vertex_indices(LIMB *limb)
{

TRI			*ptri;
int detect;
int count;

/*****************************************************************************/

	ptri = limb->ptri;
	detect=0;
	count = 5;

	while(count--)
	{
	if ( (ptri->v1 % 3)==0 && (ptri->v2 % 3)==0 && (ptri->v3 % 3)==0 )
		detect++;
	ptri++;
	}

	if(detect > 4)
		{
		ptri = limb->ptri;
		count = limb->ntris;
		printf("c:%03d v: %d %d %d  Verts mpy by 3\n",count,ptri->v1,ptri->v2,ptri->v3);
		while(count--)		
			{
			ptri->v1 = ptri->v1/3;
			ptri->v2 = ptri->v2/3;
			ptri->v3 = ptri->v3/3;
			ptri++;
			}
		}

}

/***** End of check_vertex_indices() *****************************************/



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: convert_2_jonhey()                                              */
/*                                                                           */
/* (c) 1999 Midway Games, Inc.                                               */
/*                                                                           */
/* 04 Jan 99 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void convert_2_jonhey (jonhey_t *jh_obj, LIMB *model, float *matrix, 
					   Texture_node_t **decals)
{
float *m;

/*****************************************************************************/

	jh_obj->limb = model;
	jh_obj->text = decals;
	jh_obj->mode = JONHEY_MODE_CLEAR;

	m = &jh_obj->m[0];

	m[0] = matrix[0] * -fovval;
	m[1] = matrix[1] * -fovval;
	m[2] = matrix[2] * -fovval;
	m[3] = matrix[3] * -fovval;
	if (is_low_res)					/* ty *= low_res ? (2.0f / 3.0f) : 1.0f; */
		{
		m[4] = matrix[4] * fovval * 0.6666f;
		m[5] = matrix[5] * fovval * 0.6666f;
		m[6] = matrix[6] * fovval * 0.6666f;
		m[7] = matrix[7] * fovval * 0.6666f;
		}
	else
		{
		m[4] = matrix[4] * fovval;
		m[5] = matrix[5] * fovval;
		m[6] = matrix[6] * fovval;
		m[7] = matrix[7] * fovval;
		}
	m[8] = matrix[8];
	m[9] = matrix[9];
	m[10] = matrix[10];
	m[11] = matrix[11];


//	m[0] = matrix[0] * -768.0f;
//	m[1] = matrix[1] * -768.0f;
//	m[2] = matrix[2] * -768.0f;
//	m[3] = matrix[3] * -768.0f;
//	m[4] = matrix[4] * 768.0f;
//	m[5] = matrix[5] * 768.0f;
//	m[6] = matrix[6] * 768.0f;
//	m[7] = matrix[7] * 768.0f;
//	m[8] = matrix[8];
//	m[9] = matrix[9];
//	m[10] = matrix[10];
//	m[11] = matrix[11];


}
/*****************************************************************************/






/*****************************************************************************/
/*                                                                           */
/* FUNCTION: recalculate_bbox()                                              */
/*                                                                           */
/* (c) 1999 Midway Games, Inc.                                               */
/*                                                                           */
/* 04 Jan 99 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void recalculate_bbox(LIMB *limb)
{

TRI	   *ptri;
VERTEX *bbox;
VERTEX *vert;

int nvtx;
int i;

float max_x;
float max_y;
float max_z;
float min_x;
float min_y;
float min_z;

float vx;
float vy;
float vz;


/*****************************************************************************/

	ptri = limb->ptri;
	nvtx = limb->ntris;
	bbox = limb->bbox;

    vert = &limb->pvtx[0];

	max_x = vert->x;
	max_y = vert->y;
	max_z = vert->z;
	min_x = vert->x;
	min_y = vert->y;
	min_z = vert->z;
		
	for (i = 0; i < nvtx; i++) 
		{

        vert = &limb->pvtx[i];
		vx = vert->x;
		vy = vert->y;
		vz = vert->z;

		if (vx < min_x)
			min_x = vx;

		if (vy < min_y)
			min_y = vy;

		if (vz < min_z)
			min_z = vz;

		if (vx > max_x)
			max_x = vx;

		if (vy > max_y)
			max_y = vy;

		if (vz > max_z)
			max_z = vz;
		}

		vert = &bbox[0];
		vert->x = min_x;
		vert->y = min_y;
		vert->z = min_z;

		vert = &bbox[1];
		vert->x = min_x;
		vert->y = min_y;
		vert->z = max_z;
		
		vert = &bbox[2];
		vert->x = min_x;
		vert->y = max_y;
		vert->z = min_z;
		
		vert = &bbox[3];
		vert->x = min_x;
		vert->y = max_y;
		vert->z = max_z;
		
		vert = &bbox[4];
		vert->x = max_x;
		vert->y = min_y;
		vert->z = min_z;
		
		vert = &bbox[5];
		vert->x = max_x;
		vert->y = min_y;
		vert->z = max_z;
		
		vert = &bbox[6];
		vert->x = max_x;
		vert->y = max_y;
		vert->z = min_z;
		
		vert = &bbox[7];
		vert->x = max_x;
		vert->y = max_y;
		vert->z = max_z;

}
/***** End of recalculate_bbox() *********************************************/







/*****************************************************************************/
/*                                                                           */
/* FUNCTION: init_crowd_bbox()                                               */
/*                                                                           */
/* (c) 1999 Midway Games, Inc.                                               */
/*                                                                           */
/* 04 Jan 99 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void init_crowd_bbox (crowd_bbox_t *crowd, LIMB **limbs)

{
int i;

/*	printf("init_crowd_bbox\n");*/
	i = 0;
	while (crowd[i].bbox != NULL)
		{
		init_crowd_bbox_sub(&crowd[i],limbs);
		i++;
		}


}

/***** End if init_crowd_bbox() **********************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: init_crowd_bbox_sub()                                           */
/*                                                                           */
/* (c) 1999 Midway Games, Inc.                                               */
/*                                                                           */
/* 04 Jan 99 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void init_crowd_bbox_sub(crowd_bbox_t *crowd, LIMB **limbs)

{
int size;
int offset;
LIMB *limb;
VERTEX *bbox;
VERTEX *new_bbox;
VERTEX *vert;

int i,j;
float vx,vy,vz;
float min_x;
float min_y;
float min_z;
float max_x;
float max_y;
float max_z;


	offset = crowd->offset;
	size = crowd->size;
	new_bbox = crowd->bbox;

	limb = limbs[offset];
	bbox = limb->bbox;

	max_x = bbox->x;
	max_y = bbox->y;
	max_z = bbox->z;
	min_x = bbox->x;
	min_y = bbox->y;
	min_z = bbox->z;


//	printf("sub %d %d\n",offset, size);
	for (i=offset;i<(offset+size);i++)
		{
		limb = limbs[i];
		
		for (j=0;j<8;j++)
			{
			bbox = &(limb->bbox[j]);

			vx = bbox->x;
			vy = bbox->y;
			vz = bbox->z;

			if (vx < min_x)
				min_x = vx;

			if (vy < min_y)
				min_y = vy;

			if (vz < min_z)
				min_z = vz;

			if (vx > max_x)
				max_x = vx;

			if (vy > max_y)
				max_y = vy;

			if (vz > max_z)
				max_z = vz;

			}

		}


		vert = &new_bbox[0];
		vert->x = min_x;
		vert->y = min_y;
		vert->z = min_z;

		vert = &new_bbox[1];
		vert->x = min_x;
		vert->y = min_y;
		vert->z = max_z;
		
		vert = &new_bbox[2];
		vert->x = min_x;
		vert->y = max_y;
		vert->z = min_z;
		
		vert = &new_bbox[3];
		vert->x = min_x;
		vert->y = max_y;
		vert->z = max_z;
		
		vert = &new_bbox[4];
		vert->x = max_x;
		vert->y = min_y;
		vert->z = min_z;
		
		vert = &new_bbox[5];
		vert->x = max_x;
		vert->y = min_y;
		vert->z = max_z;
		
		vert = &new_bbox[6];
		vert->x = max_x;
		vert->y = max_y;
		vert->z = min_z;
		
		vert = &new_bbox[7];
		vert->x = max_x;
		vert->y = max_y;
		vert->z = max_z;


}

/***** End if init_crowd_bbox_sub() ******************************************/



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: turn_on_bbox()                                                  */
/*                                                                           */
/* (c) 1999 Midway Games, Inc.                                               */
/*                                                                           */
/* 04 Jan 99 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void turn_on_bbox (void)
{
	bbox_mode = TRUE;
}
/***** End turn_on_bbox() ****************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: turn_off_bbox()                                                 */
/*                                                                           */
/* (c) 1999 Midway Games, Inc.                                               */
/*                                                                           */
/* 04 Jan 99 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
void turn_off_bbox (void)
{
	bbox_mode = FALSE;
}
/***** End turn_off_bbox() ***************************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: check_inside_screen()                                           */
/*                                                                           */
/* (c) 1999 Midway Games, Inc.                                               */
/*                                                                           */
/* 04 Jan 99 EJK                                                             */
/*                                                                           */
/*****************************************************************************/
int check_inside_screen (void)

{
float v_norm[3];  	// norm of the quad
float vcross[3];  	

float dot_result;

float v1[3];  		// temp vert1
float v2[3];  		// temp vert2

float va[3];  		// quad: vert a
float vb[3]; 		// quad: vert b
float vc[3];  		// quad: vert c
float vd[3];  		// quad: vert d

float test_pt[3];

/*****************************************************************************/
	test_pt[0] = 320.0f;
	test_pt[1] = 320.0f;
	test_pt[2] = 0.0f;

	/*****************************************************************/
	/* Setup the quad verts.                                         */
	/*****************************************************************/

	/* get a vert on the quad from the vert list */
	va[0] = CLIP_XMIN;
	va[1] = CLIP_YMIN;
	va[2] = 0.0f;

	vb[0] = CLIP_XMAX;
	vb[1] = CLIP_YMIN;
	vb[2] = 0.0f;

	vc[0] = CLIP_XMIN;
	vc[1] = CLIP_YMAX;
	vc[2] = 0.0f;

	vd[0] = CLIP_XMIN;
	vd[1] = CLIP_YMAX;
	vd[2] = 0.0f;
	 

	/*****************************************************************/
	/* Find the Norm of the quad.                                    */
	/*****************************************************************/

	/* V_NORM = (P2 - P1) x (P3 - P1) */
	
    /* Optimize:                        */
	/*		vect_subt (v1, p2, p1);	    */
	/*		vect_subt (v2, p3, p1);     */
	/*	    vect_cross (M, v1, v2);     */  
    
//   	v_norm[0] = (vb[1] - va[1]) * (vc[2] - va[2]) 
//    			- (vb[2] - va[2]) * (vc[1] - va[1]);  
//
//	v_norm[1] = (vb[2] - va[2]) * (vc[0] - va[0]) 
//    			- (vb[0] - va[0]) * (vc[2] - va[2]);
//
//	v_norm[2] = (vb[0] - va[0]) * (vc[1] - va[1]) 
//    			- (vb[1] - va[1]) * (vc[0] - va[0]);
    
	v_norm[0] = 0.0f;
	v_norm[1] = 0.0f;
	v_norm[2] = 1.0f;


//printf("v_norm %f %f %f\n",v_norm[0],v_norm[1],v_norm[2]);
//    if ( vect_dot(v_norm,v_norm) == 0.0f)
//    	{
//		fprintf (stderr,"zone_line_quad(): ERROR quad normal is zero\r\n");
//		lockup();
//        }

            
	/*************************************************************/
    /* Check Line 1: A to B                                      */
	/*************************************************************/
    
    vect_subt (v1, vb, va);
	vect_subt (v2, test_pt, va);

	vect_cross (vcross, v1, v2);
    dot_result = vect_dot ( v_norm, vcross);

	/* is it on the outside of the polygon */	    
	if (dot_result < 0.0f)  
    	{		   
        return (FALSE);
	    }
    
	/*************************************************************/
    /* Check Line 2: B to C                                      */
	/*************************************************************/

    vect_subt (v1, vc, vb);
	vect_subt (v2, test_pt, vb);
	
	vect_cross (vcross, v1, v2);
    dot_result = vect_dot ( v_norm, vcross);

	/* is it on the outside of the polygon */	    
    if (dot_result < 0.0f) 
	   	{		   
        return (FALSE);
    	}
	
	/*************************************************************/
    /* Check Line 3: C to D                                      */
	/*************************************************************/
    
 	vect_subt (v1, vd, vc);
    vect_subt (v2, test_pt, vc);
															  
	vect_cross (vcross, v1, v2);
    dot_result = vect_dot ( v_norm, vcross);

	/* is it on the outside of the polygon */
    if (dot_result < 0.0f) 
	   	{		   
        return (FALSE);
		}

	/*************************************************************/
	/* Check Line 4: D to A                                      */
	/*************************************************************/

    vect_subt (v1, va, vd);
	vect_subt (v2, test_pt, vd);

	vect_cross (vcross, v1, v2);
	dot_result = vect_dot ( v_norm, vcross);

	/* is it on the outside of the polygon */	
    if (dot_result < 0.0f) 
    	{		   
        return (FALSE);
    	}
	else
		{
        return (TRUE);
    	}           
        
} /* check_inside_screen() */
    
/***** END of check_inside_screen() ******************************************/






void geom_clear_stats(void)
{
	#ifdef GEOM_STATS
	stats.tris_processed          = 0;
	stats.tris_bbox_processed     = 0;
	stats.tris_drawn              = 0;
	stats.verts_processed         = 0;
	stats.verts_bbox_processed    = 0;
	stats.verts_transformed       = 0;
	stats.verts_drawn             = 0;
	stats.bbox_verts			  = 0;
	stats.bbox_verts_transformed  = 0;
	stats.tris_clip_processed     = 0;
	stats.tris_clipped            = 0;
	stats.strips_processed        = 0;
	stats.strips_bbox_processed   = 0;
	stats.strips_drawn            = 0;
	#endif

}



void geom_print_stats(void)
{

	#ifdef GEOM_STATS
	fprintf(stderr,"\n" );
	fprintf(stderr,"stats.tris_processed        : %d  \n",stats.tris_processed  );
	fprintf(stderr,"stats.tris_bbox_processed   : %d  \n",stats.tris_bbox_processed  );
	fprintf(stderr,"stats.tris_drawn            : %d  \n",stats.tris_drawn           );
	fprintf(stderr,"stats.verts_processed       : %d  \n",stats.verts_processed      );
	fprintf(stderr,"stats.verts_bbox_processed  : %d  \n",stats.verts_bbox_processed      );
	fprintf(stderr,"stats.verts_transformed     : %d  \n",stats.verts_transformed      );
	fprintf(stderr,"stats.verts_drawn           : %d  \n",stats.verts_drawn          );
	fprintf(stderr,"stats.bbox_verts            : %d  \n",stats.bbox_verts);
	fprintf(stderr,"stats.bbox_verts_transformed: %d  \n",stats.bbox_verts_transformed);
	fprintf(stderr,"stats.tris_clip_processed   : %d  \n",stats.tris_clip_processed  );
	fprintf(stderr,"stats.tris_clipped          : %d  \n",stats.tris_clipped         );
	fprintf(stderr,"stats.strips_processed      : %d  \n",stats.strips_processed     );
	fprintf(stderr,"stats.strips_bbox_processed : %d  \n",stats.strips_bbox_processed     );
	fprintf(stderr,"stats.strips_drawn          : %d  \n",stats.strips_drawn         );
	#endif

}






/***** END OF GEOM.C *********************************************************/
