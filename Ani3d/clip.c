/*****************************************************************************/
/*                                                                           */
/* FILE: CLIP.C                                                              */
/*                                                                           */
/* NBA                                                                       */
/*                                                                           */
/* Triangle clipping routines.  Draws clipped triangle in terms of           */
/* sub-divided triangles.	                                                 */
/*                                                                           */
/* (c) 1999 Midway Games, Inc.                                               */
/*                                                                           */
/* 18 Jan 99 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

/***** INCLUDES **************************************************************/
#include <stdio.h>
	
/* GOOSE and GLIDE stuff */
#include <goose/goose.h>
#include <glide/glide.h>
#include <fxdll.h>
#include <glide.h>
#include	<glideutl.h>




#include <memory.h>
#include	<string.h>
#include	<stdio.h>
#include	<dir.h>
#include	<fcntl.h>
#include	<unistd.h>
#include	<sys/stat.h>

#include	<goose/goose.h>
#include	<glideutl.h>

#include <3dfx.h>

#define FX_DLL_DEFINITION
#include <fxdll.h>
#include <glide.h>

#include "fxglide.h"
#ifdef BANSHEE
#include	<fxcmd.h>
#endif

#if GLIDE_DISPATCH_SETUP
#include "fxinline.h"
#endif
#ifdef GLIDE_DEBUG
#include <math.h>
#endif /* GLIDE_DEBUG */


/* our stuff */
#include "clip.h"

extern int global_dog;

void grDrawFanReduced(ReducedVertex *a, int num_verts);
int trivial_test (ReducedVertex *v1, ReducedVertex *v2, ReducedVertex *v3);
void grDrawStripReduced(ReducedVertex **a, int num_verts);
static void memcpy_words(unsigned long *to, unsigned long **from, int num);



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: clip_triangle_t()                                               */
/*                                                                           */
/* Clipping Routine for SrVertexWithT Triangles.                             */
/*                                                                           */
/* Based on Sutherland-Hodgman Polygon-Clipping Algorithm,                   */
/* Computer Graphics: Principles and Pratices, Foley, Van Dam: pp. 124-129   */
/*                                                                           */
/* The algorithm clips against a single, infinite clip edge and outputs      */
/* another series of vertices defining the clipped polygon.  In a second     */
/* pass, the partially clipped polygon is then clipped against the second    */
/* clip edge, and so on.                                                     */
/*                                                                           */
/* Consider the polygon edge from vertex s to vertex p.  In CASE 1, when     */
/* the polygon edge is completely inside the clip boundary, vertex p is      */
/* added to the output list. In CASE 2, the intersection point i is output   */
/* as a vertex because the edge intersects the boundary.  In CASE 3, both    */
/* vertices are outside the boundary, so there is no output. In CASE 4, the  */
/* intersection point i and p are both added to the output list.             */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 18 Jan 99 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

ReducedVertex inVertexArrayDATA_T[8];
ReducedVertex outVertexArrayDATA_T[8];

/*****************************************************************************/

void clip_triangle_t (ReducedVertex *in1, ReducedVertex *in2, ReducedVertex *in3)
                        
{ /* clip_triangle_t() */

ReducedVertex inter_data;
ReducedVertex *s;
ReducedVertex *p;
ReducedVertex *intersect = (ReducedVertex *) &inter_data;
ReducedVertex *temp;
ReducedVertex *oo;
ReducedVertex *inVertexArray;
ReducedVertex *outVertexArray;
float t;
int j;
int inlength;
int outlength;

/****************************************************************************/


	// If the triangle is in front of the camera but too close - throw it out
	if(in1->oow > 1.0F &&	in2->oow > 1.0F &&	in3->oow > 1.0F)
	{
		return;
	}

	// If the triangle is behind the camera - throw it out
	if(in1->oow <= 0.0F && in2->oow <= 0.0f && in3->oow <= 0.0F)
	{
		return;
	}

	// If the triangle is off screen to the left - throw it out
	if((in1->x * in1->oow) <= 0.0F && (in2->x * in2->oow) <= 0.0F && (in3->x * in3->oow) <= 0.0F)
	{
		return;
	}

	// If the triangle if off the bottom of the screen - throw it out
	if((in1->y * in1->oow) <= 0.0F && (in2->y * in2->oow) <= 0.0F && (in3->y * in3->oow) <= 0.0F)
	{
		return;
	}

	// If the triangle is in front of the camera
	if(in1->oow > 0.0f && in2->oow >= 0.0f && in3->oow > 0.0f)
	{
		// If the triangle is above the top of the screen - throw it out
		if(in1->y >= 384.0F &&	in2->y >= 384.0F &&	in3->y >= 384.0F)
		{
			return;
		}

		// If the triangle is off the right side of the screen - throw it out
		if(in1->x >= 512.0F &&	in2->x >= 512.0F &&	in3->x >= 512.0F)
		{
			return;
		}
	}

	/*
	** perform trivial accept
	*/
	// If the triangle is within x pixels of the edges of screen - just draw it
	if (
		( in1->x >= CLIP_XMIN - GUTTER ) &&
		( in1->x <  CLIP_XMAX + GUTTER ) &&
		( in1->y >= CLIP_YMIN - GUTTER ) &&
		( in1->y <  CLIP_YMAX + GUTTER ) &&
		( in2->x >= CLIP_XMIN - GUTTER ) &&
		( in2->x <  CLIP_XMAX + GUTTER ) &&
		( in2->y >= CLIP_YMIN - GUTTER ) &&
		( in2->y <  CLIP_YMAX + GUTTER ) &&
		( in3->x >= CLIP_XMIN - GUTTER ) &&
		( in3->x <  CLIP_XMAX + GUTTER ) &&
		( in3->y >= CLIP_YMIN - GUTTER ) &&
		( in3->y <  CLIP_YMAX + GUTTER ) &&
		( in1-> oow > 0.0F ) && ( in1->oow <= 1.0F ) &&
		( in2-> oow > 0.0F ) && ( in2->oow <= 1.0F ) &&
		( in3-> oow > 0.0F ) && ( in3->oow <= 1.0F ))
		{

		grDrawSimpleTriangle(in1,in2,in3);

	    return;
	  }


 	inVertexArray = &inVertexArrayDATA_T[0];
	outVertexArray = &outVertexArrayDATA_T[0]; 

	/****************************************************************************/
	/* FRONT CLIPPING PLANE:                                                    */
	/****************************************************************************/

	outlength = 0;
	s = in3;		
	p = in1;

	for(j=0;j<3;j++)
		{ /* for(j) */
	    oo = &outVertexArray[outlength];
	    if( (p->oow > CLIP_WMAX) && (p->oow < CLIP_WMIN ))  /* CASES 1 and 4 */
	        { /* if(clip(p)) */
	        if ( (s->oow > CLIP_WMAX) && (s->oow < CLIP_WMIN ) ) /* CASE 1 */
	        	{  
				outlength++;
	            *oo = *p;
	            }
			else
	        	{ /* CASE 4 */
	            
	            /****************************************************************************/
				/* Calculates the intersection of the polygon edge from vertex s to vertex  */
				/* p with clip Boundary, which is definded by two vetices on the clip       */
				/* polygon's boundary.                                                      */
	            /****************************************************************************/
	            /* intersect->x = s->x + (CLIP_WMIN - s->oow)*(p->x-s->x)/(p->oow-s->oow);  */
	            /* intersect->y = s->y + (CLIP_WMIN - s->oow)*(p->y-s->y)/(p->oow-s->oow);  */
	            /****************************************************************************/
	            
				t = (CLIP_WMIN - s->oow) / (p->oow - s->oow);
				oo->x = s->x + t*(p->x - s->x);
				oo->y = s->y + t*(p->y - s->y);
	  			oo->oow = CLIP_WMIN;
				oo->sow = s->sow + t*(p->sow - s->sow);
				oo->tow = s->tow + t*(p->tow - s->tow);
	            
			    oo = &outVertexArray[ ++outlength ];
				outlength++;
	            *oo = *p;
	            
	        	} /* if(clip(s)) */
	        } /* if(clip(p)) */
		else
	    	{ /* if(clip(p)) */  
	        if ( (s->oow > CLIP_WMAX) && (s->oow < CLIP_WMIN ) )
	        	{ /* if(clip(s)) */  /* CASE 2 */
	            
				/***************************************************************************/
				/* Calculates the intersection of the polygon edge from vertex s to vertex */
				/* p with clip Boundary, which is definded by two vetices on the clip      */
				/* polygon's boundary.                                                     */
	            /***************************************************************************/
	 			/* intersect->x = s->x + (CLIP_WMIN - s->oow)*(p->x-s->x)/(p->oow-s->oow); */
	            /* intersect->y = s->y + (CLIP_WMIN - s->oow)*(p->y-s->y)/(p->oow-s->oow); */
	            /***************************************************************************/
	            
	 			t = (CLIP_WMIN - s->oow) / (p->oow - s->oow);
	 			outlength++;
				oo->x = s->x + t*(p->x - s->x);
				oo->y = s->y + t*(p->y - s->y);
	  			oo->oow = CLIP_WMIN;
				oo->sow = s->sow + t*(p->sow - s->sow);
				oo->tow = s->tow + t*(p->tow - s->tow);
	            
	            } /* if(clip(s)) */
	            /* CASE 3 no action */
	        } /* if(clip(p)) */
	        s = p;
	        if(j==0)
				p = in2;
		    else								 
	    		p = in3;
	        
	    } /* for(j) */


	/*************************************************************************/
	/* LEFT EDGE:                                                            */
	/*************************************************************************/

	temp = &inVertexArray[0];
	inVertexArray = &outVertexArray[0];
	inlength = outlength;
	outVertexArray = temp;

	outlength = 0;
	s = &inVertexArray[inlength-1];
	intersect->x = CLIP_XMIN;

	for(j=0;j<inlength;j++)
		{ /* for(j) */
		p =  &inVertexArray[j];
	    oo = &outVertexArray[outlength];

	    
	    if( (p->x >= CLIP_XMIN) )  /* CASES 1 and 4 */
	        { /* if(clip(p)) */
	        if ( (s->x >= CLIP_XMIN) ) /* CASE 1 */
	        	{ 
				outlength++;
	            *oo = *p;
	            }
			else
	        	{ /* CASE 4 */
	 
				/*****************************************************************************/
				/* Calculates the intersection of the polygon edge from vertex s to vertex   */
				/* p with clip Boundary, which is definded by two vetices on the clip        */
				/* polygon's boundary.                                                       */
	            /*****************************************************************************/
				/* intersect->y = s->y + (CLIP_XMIN - s->x) * (p->y - s->y) / (p->x - s->x); */
	            /*****************************************************************************/
	 
	 			t = (CLIP_XMIN - s->x) / (p->x - s->x);
				oo->x = intersect->x;
				oo->y = s->y + t*(p->y - s->y);
	   			oo->oow = s->oow + t*(p->oow - s->oow);
				oo->sow = s->sow + t*(p->sow - s->sow);
				oo->tow = s->tow + t*(p->tow - s->tow);
	            
			    oo = &outVertexArray[ ++outlength ];
				outlength++;
	            *oo = *p;
	            
	        	} /* if(clip(s)) */
	        } /* if(clip(p)) */
		else
	    	{ /* if(clip(p)) */
	        if ( (s->x >= CLIP_XMIN) )
	        	{ /* if(clip(s)) */ /* CASE 2 */

				/*****************************************************************************/
				/* Calculates the intersection of the polygon edge from vertex s to vertex   */
				/* p with clip Boundary, which is definded by two vetices on the clip        */
				/* polygon's boundary.                                                       */
	            /*****************************************************************************/
				/* intersect->y = s->y + (CLIP_XMIN - s->x) * (p->y - s->y) / (p->x - s->x); */
	            /*****************************************************************************/
	 
				t = (CLIP_XMIN - s->x) / (p->x - s->x);
	 			outlength++;
	       		oo->x = intersect->x;
				oo->y = s->y + t*(p->y - s->y);
	   			oo->oow = s->oow + t*(p->oow - s->oow);
				oo->sow = s->sow + t*(p->sow - s->sow);
				oo->tow = s->tow + t*(p->tow - s->tow);
	            
	            } /* if(clip(s)) */
	            /* CASE 3 no action */
	        } /* if(clip(p)) */
	        s = p;
	        
	    } /* for(j) */

	/****************************************************************************/
	/* BOTTOM EDGE:                                                             */
	/****************************************************************************/

	temp = &inVertexArray[0];
	inVertexArray = &outVertexArray[0];
	inlength = outlength;
	outVertexArray = temp;

	outlength = 0;
	s = &inVertexArray[inlength-1];
	intersect->y = CLIP_YMIN;

	for(j=0;j<inlength;j++)
		{ /* for(j) */
		p =  &inVertexArray[j];
	    oo = &outVertexArray[outlength];
	    if( p->y >= CLIP_YMIN )  /* CASES 1 and 4 */
	        { /* if(clip(p)) */
	        if ( s->y >= CLIP_YMIN )	  /* CASE 1 */
	        	{ 
				outlength++;
	            *oo = *p;
	            }
			else
	        	{ /* CASE 4 */
	            
				/*******************************************************************************/
				/* Calculates the intersection of the polygon edge from vertex s to vertex     */
				/* p with clip Boundary, which is definded by two vetices on the clip          */
				/* polygon's boundary.                                                         */
	            /*******************************************************************************/
		        /*   intersect->x = s->x + (CLIP_YMIN - s->y) * (p->x - s->x) / (p->y - s->y); */
	            /*******************************************************************************/
	            
				t = (CLIP_YMIN - s->y) / (p->y - s->y);
	         	oo->x = s->x + t * (p->x - s->x);
				oo->y = intersect->y;
	   			oo->oow = s->oow + t*(p->oow - s->oow);
				oo->sow = s->sow + t*(p->sow - s->sow);
				oo->tow = s->tow + t*(p->tow - s->tow);
	            
			    oo = &outVertexArray[ ++outlength ];
	 			outlength++;
	            *oo = *p;
	            
	        	} /* if(clip(s)) */
	        } /* if(clip(p)) */
		else
	    	{ /* if(clip(p)) */
	        if ( s->y >= CLIP_YMIN  )
	        	{ /* if(clip(s)) */	  /* CASE 2 */
	            
				/******************************************************************************/
				/* Calculates the intersection of the polygon edge from vertex s to vertex    */
				/* p with clip Boundary, which is definded by two vetices on the clip         */
				/* polygon's boundary.                                                        */
	            /******************************************************************************/
	            /*  intersect->x = s->x + (CLIP_YMIN - s->y) * (p->x - s->x) / (p->y - s->y); */
	            /******************************************************************************/
	            
				t = (CLIP_YMIN - s->y) / (p->y - s->y);
	 			outlength++;
	         	oo->x = s->x + t * (p->x - s->x);
				oo->y = intersect->y;
	   			oo->oow = s->oow + t*(p->oow - s->oow);
				oo->sow = s->sow + t*(p->sow - s->sow);
				oo->tow = s->tow + t*(p->tow - s->tow);
	            
	            } /* if(clip(s)) */
	            /* CASE 3 no action */
	        } /* if(clip(p)) */
	        s = p;
	    } /* for(j) */

	/****************************************************************************/
	/* RIGHT EDGE:                                                              */
	/****************************************************************************/

	temp = &inVertexArray[0];
	inVertexArray = &outVertexArray[0];
	inlength = outlength;
	outVertexArray = temp;

	outlength = 0;
	s = &inVertexArray[inlength-1];
	intersect->x = CLIP_XMAX;

	for(j=0;j<inlength;j++)
		{ /* for(j) */
		p =  &inVertexArray[j];
	    oo = &outVertexArray[outlength];
	    if( p->x <= CLIP_XMAX )  /* CASES 1 and 4 */
	        { /* if(clip(p)) */
	        if ( s->x <= CLIP_XMAX  )  /* CASE 1 */
	        	{ 
	 			outlength++;
	            *oo = *p;
	            }
			else
	        	{ /* CASE 4 */
	            
				/*****************************************************************************/
				/* Calculates the intersection of the polygon edge from vertex s to vertex   */
				/* p with clip Boundary, which is definded by two vetices on the clip        */
				/* polygon's boundary.                                                       */
	            /*****************************************************************************/
				/* intersect->y = s->y + (CLIP_XMAX - s->x) * (p->y - s->y) / (p->x - s->x); */
	            /*****************************************************************************/

				t = (CLIP_XMAX - s->x) / (p->x - s->x);
	       		oo->x = intersect->x;
				oo->y = s->y + t*(p->y - s->y);
	   			oo->oow = s->oow + t*(p->oow - s->oow);
				oo->sow = s->sow + t*(p->sow - s->sow);
				oo->tow = s->tow + t*(p->tow - s->tow);

			    oo = &outVertexArray[ ++outlength ];
	 			outlength++;
	            *oo = *p;
	            
	        	} /* if(clip(s)) */
	        
	        } /* if(clip(p)) */
		else
	    	{ /* if(clip(p)) */
	        if ( s->x <= CLIP_XMAX )
	        	{ /* if(clip(s)) */	  /* CASE 2 */
	            
				/*****************************************************************************/
				/* Calculates the intersection of the polygon edge from vertex s to vertex   */
				/* p with clip Boundary, which is definded by two vetices on the clip        */
				/* polygon's boundary.                                                       */
	            /*****************************************************************************/
				/* intersect->y = s->y + (CLIP_XMAX - s->x) * (p->y - s->y) / (p->x - s->x); */
	            /*****************************************************************************/

				t = (CLIP_XMAX - s->x) / (p->x - s->x);
	 			outlength++;
	       		oo->x = intersect->x;
				oo->y = s->y + t*(p->y - s->y);
	   			oo->oow = s->oow + t*(p->oow - s->oow);
				oo->sow = s->sow + t*(p->sow - s->sow);
				oo->tow = s->tow + t*(p->tow - s->tow);

	            } /* if(clip(s)) */
	            /* CASE 3 no action */
	        } /* if(clip(p)) */
	        s = p;
	    } /* for(j) */

	/****************************************************************************/
	/* TOP EDGE:                                                                */
	/****************************************************************************/

	temp = &inVertexArray[0];
	inVertexArray = &outVertexArray[0];
	inlength = outlength;
	outVertexArray = temp;

	outlength = 0;
	s = &inVertexArray[inlength-1];
	intersect->y = CLIP_YMAX;

	for(j=0;j<inlength;j++)
		{ /* for(j) */
		p =  &inVertexArray[j];
	    oo = &outVertexArray[outlength];
	    if( p->y <= CLIP_YMAX )  /* CASES 1 and 4 */
	        { /* if(clip(p)) */
	        if ( s->y <= CLIP_YMAX )	  /* CASE 1 */
	        	{ 
	 			outlength++;
	            *oo = *p;
	            }
			else
	        	{ /* CASE 4 */

				/*****************************************************************************/
				/* Calculates the intersection of the polygon edge from vertex s to vertex   */
				/* p with clip Boundary, which is definded by two vetices on the clip        */
				/* polygon's boundary.                                                       */
	            /*****************************************************************************/
	          	/* intersect->x = s->x + (CLIP_YMAX - s->y) * (p->x - s->x) / (p->y - s->y); */
	            /*****************************************************************************/
	 
				t = (CLIP_YMAX - s->y) / (p->y - s->y);
	         	oo->x = s->x + t * (p->x - s->x);
				oo->y = intersect->y;
	   			oo->oow = s->oow + t*(p->oow - s->oow);
				oo->sow = s->sow + t*(p->sow - s->sow);
				oo->tow = s->tow + t*(p->tow - s->tow);
	            
			    oo = &outVertexArray[ ++outlength ];
	 			outlength++;
	            *oo = *p;
	            
	         	} /* if(clip(s)) */
	        } /* if(clip(p)) */
		else
	    	{ /* if(clip(p)) */
	        if ( s->y <= CLIP_YMAX )
	        	{ /* if(clip(s)) */	  /* CASE 2 */
	          
				/*****************************************************************************/
				/* Calculates the intersection of the polygon edge from vertex s to vertex   */
				/* p with clip Boundary, which is definded by two vetices on the clip        */
				/* polygon's boundary.                                                       */
	            /*****************************************************************************/
	          	/* intersect->x = s->x + (CLIP_YMAX - s->y) * (p->x - s->x) / (p->y - s->y); */
	            /*****************************************************************************/
	            
				t = (CLIP_YMAX - s->y) / (p->y - s->y);
				outlength++;
	         	oo->x = s->x + t * (p->x - s->x);
				oo->y = intersect->y;
	   			oo->oow = s->oow + t*(p->oow - s->oow);
				oo->sow = s->sow + t*(p->sow - s->sow);
				oo->tow = s->tow + t*(p->tow - s->tow);
	            
	            } /* if(clip(s)) */
	            /* CASE 3 no action */
	        } /* if(clip(p)) */
	        s = p;
	    } /* for(j) */

	/****************************************************************************/
	/* Snap the vertices and draw all of the subtriangles in a "FAN" order.     */
	/****************************************************************************/

	 if(outlength >= 3)
	 	  {
		  grDrawFanReduced(outVertexArray,outlength);
	  	  }
	            
} /* clip_triangle_t() */

/***** END of clip_triangle_t() **********************************************/
			    

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: trivial_tesst()                                                 */
/*                                                                           */
/* (c) 1999 Midway Games, Inc.                                               */
/*                                                                           */
/* 18 Jan 99 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

int trivial_test (ReducedVertex *v1, ReducedVertex *v2, ReducedVertex *v3)

{

#if 1
short opcode1;
short opcode2;
short opcode3;
short opcodeA;
short opcodeB;

/*****************************************************************************/

/* Test all 3 points against the view planes */
	opcode1 = 0x0;

	if ( v1->y > CLIP_YMAX )  /* above the screen */
		opcode1 |= 0x1;

	if ( v1->y < CLIP_YMIN )  /* below the screen */
		opcode1 |= 0x2;

	if ( v1->x > CLIP_XMAX )  /* right edge of screen */
		opcode1 |= 0x4;

	if ( v1->x < CLIP_XMIN )  /* left edge of screen */
		opcode1 |= 0x8;

	if ( v1->oow <= CLIP_WMAX )  /* in front of the camera */
		opcode1 |= 0x10;

	opcode2 = 0x0;

	if ( v2->y > CLIP_YMAX )  /* above the screen */
		opcode2 |= 0x1;

	if ( v2->y < CLIP_YMIN )  /* below the screen */
		opcode2 |= 0x2;

	if ( v2->x > CLIP_XMAX )  /* right edge of screen */
		opcode2 |= 0x4;
			  
	if ( v2->x < CLIP_XMIN )  /* left edge of screen */
		opcode2 |= 0x8;

	if ( v2->oow <= CLIP_WMAX )  /* in front of the camera */
		opcode2 |= 0x10;

	opcode3 = 0x0;

	if ( v3->y > CLIP_YMAX )  /* above the screen */
		opcode3 |= 0x1;

	if ( v3->y < CLIP_YMIN )  /* below the screen */
		opcode3 |= 0x2;

	if ( v3->x > CLIP_XMAX )  /* right edge of screen */
		opcode3 |= 0x4;

	if ( v3->x < CLIP_XMIN )  /* left edge of screen */
		opcode3 |= 0x8;

	if ( v3->oow <= CLIP_WMAX )  /* in front of the camera */
		opcode3 |= 0x10;

	/* find the opcodes */
	opcodeA = opcode1 & opcode2 & opcode3;
	opcodeB = opcode1 | opcode2 | opcode3;

/* all 8 points are completely inside the view port */
if ( opcodeB == 0x0 )
	return ( TRIVIAL_ACCEPTED );

/* all 8 points are completely outside the view port */
if ( opcodeA != 0x0 )
	return ( TRIVIAL_REJECTED );

/* points are partially inside the view port */
else
	return ( CLIP_OBJECT );
}


#else

	// If the triangle is in front of the camera but too close - throw it out
	if(v1->oow > 1.0F &&	v2->oow > 1.0F &&	v3->oow > 1.0F)
	{
	return ( TRIVIAL_REJECTED );
	}

	// If the triangle is behind the camera - throw it out
	if(v1->oow <= 0.0F && v2->oow <= 0.0f && v3->oow <= 0.0F)
	{
	return ( TRIVIAL_REJECTED );
	}

	// If the triangle is off screen to the left - throw it out
	if((v1->x * v1->oow) <= 0.0F && (v2->x * v2->oow) <= 0.0F && (v3->x * v3->oow) <= 0.0F)
	{
	return ( TRIVIAL_REJECTED );
	}

	// If the triangle if off the bottom of the screen - throw it out
	if((v1->y * v1->oow) <= 0.0F && (v2->y * v2->oow) <= 0.0F && (v3->y * v3->oow) <= 0.0F)
	{
	return ( TRIVIAL_REJECTED );
	}

	// If the triangle is in front of the camera
	if(v1->oow > 0.0f && v2->oow >= 0.0f && v3->oow > 0.0f)
	{
		// If the triangle is above the top of the screen - throw it out
		if(v1->y >= 384.0F &&	v2->y >= 384.0F &&	v3->y >= 384.0F)
		{
	return ( TRIVIAL_REJECTED );
		}

		// If the triangle is off the right side of the screen - throw it out
		if(v1->x >= 512.0F &&	v2->x >= 512.0F &&	v3->x >= 512.0F)
		{
	return ( TRIVIAL_REJECTED );
		}
	}

	/*
	** perform trivial accept
	*/
	// If the triangle is within x pixels of the edges of screen - just draw it
	if (
		( v1->x >= (CLIP_XMIN)) &&
		( v1->x <  (CLIP_XMAX)) &&
		( v1->y >= (CLIP_YMIN)) &&
		( v1->y <  (CLIP_YMAX)) &&
		( v2->x >= (CLIP_XMIN)) &&
		( v2->x <  (CLIP_XMAX)) &&
		( v2->y >= (CLIP_YMIN)) &&
		( v2->y <  (CLIP_YMAX)) &&
		( v3->x >= (CLIP_XMIN)) &&
		( v3->x <  (CLIP_XMAX)) &&
		( v3->y >= (CLIP_YMIN)) &&
		( v3->y <  (CLIP_YMAX)) &&
		( v1-> oow > 0.0F ) && ( v1->oow <= 1.0F ) &&
		( v2-> oow > 0.0F ) && ( v2->oow <= 1.0F ) &&
		( v3-> oow > 0.0F ) && ( v3->oow <= 1.0F ))
		{
	return ( TRIVIAL_ACCEPTED );
	  }

return ( CLIP_OBJECT );



}
#endif

/***** End of trivial_test() ************************************************/



/*****************************************************************************/
/*                                                                           */
/* FUNCTION: grDrawFanReduced()                                              */
/*                                                                           */
/* (c) 1999 Midway Games, Inc.                                               */
/*                                                                           */
/* 18 Jan 99 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

		    
void grDrawFanReduced(ReducedVertex *a, int num_verts)
{
	register int	vsize;
	register float	*pptr;

	GR_DCL_GC;

	GR_FLUSH_STATE();

	vsize = gc->state.vData.vSize;

	if(vsize != 20)
		{
		printf("grDrawFanReduced: vsize: %d\n",vsize);
		lockup();
		}

	GR_SET_EXPECTED_SIZE(num_verts * vsize, 1);

	TRI_STRIP_BEGIN(kSetupFan, num_verts, vsize, SSTCP_PKT3_BDDDDD);

	pptr = (float *)tPackPtr;

	while(num_verts--)
	{
		*pptr++ = a->x;
		*pptr++ = a->y; 
		*pptr++ = a->oow; 
		*pptr++ = a->sow; 
		*pptr++ = a->tow; 
		a++;
	}

	tPackPtr = (void *)pptr;

	TRI_END;

}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: grDrawSrtipReduced()                                            */
/*                                                                           */
/* (c) 1999 Midway Games, Inc.                                               */
/*                                                                           */
/* 04 Mar 99 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void grDrawStripReduced(ReducedVertex **a, int num_verts)
{
	register int	vsize;
	int				num = num_verts > 14 ? 14 : num_verts;
	int packet_type;

	GR_DCL_GC;

	// Flush the state
	GR_FLUSH_STATE();

	// Get the size of vertex (in bytes)
	vsize = gc->state.vData.vSize;

	if(vsize != 20)
		{
		printf("grDrawStripReduced: vsize: %d\n",vsize);
		lockup();
		}

	 
	packet_type = SSTCP_PKT3_BDDDDD;

	// Loop and send verts
	while(num_verts)
	{
		// Set the expected size for the first part of the strip
		GR_SET_EXPECTED_SIZE(num * vsize, 1);

		// Set the strip header
		TRI_STRIP_BEGIN(kSetupStrip, num, vsize, packet_type);

		// Decrement number of verts left to send
		num_verts -= num;

		// Send the words
		memcpy_words((unsigned long *)tPackPtr, (unsigned long **)a, num);

		// Increment packet buffer pointer
		tPackPtr += num * (vsize>>2);

		// Done with this vertex set
		TRI_END;

		a += num;

		num = num_verts > 14 ? 14 : num_verts;

		packet_type = SSTCP_PKT3_DDDDDD;

	}
}

/***** END of grDrawStripReduced() *******************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: memcpy_words()                                                  */
/*                                                                           */
/* (c) 1999 Midway Games, Inc.                                               */
/*                                                                           */
/* 04 Mar 99 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

static void memcpy_words(unsigned long *to, unsigned long **from, int num)
{
unsigned long *ptr;
int i;

//	printf("num: %d \n",num);
	for (i=0; i<num; i++)
	{

//	printf("count: %d\n",num);
		ptr = from[i];
		*to++ = *ptr++;
		*to++ = *ptr++;
		*to++ = *ptr++;
		*to++ = *ptr++;
		*to++ = *ptr++;
	}
}
/***** END of memcpy_words() *************************************************/



/***** END OF CLIP.C *********************************************************/