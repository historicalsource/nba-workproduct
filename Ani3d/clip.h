/*****************************************************************************/
/*                                                                           */
/* FILE: CLIP.H                                                              */
/*                                                                           */
/* NBA                                                                       */
/*                                                                           */
/* Defines and function prototypes for CLIP.C                                */
/*                                                                           */
/* (c) 1999 Midway Games, Inc.                                               */
/*                                                                           */
/* 18 Jan 99 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

#if 1
#define CLIP_XMIN2 156.0f
#define CLIP_XMAX2 356.0f
#define CLIP_YMIN2 92.0f
#define CLIP_YMAX2 292.0f
#define CLIP_WMIN2 1.0f  
#define CLIP_WMAX2 0.00f 
#endif

#if 1
#define CLIP_XMIN 0.0f
#define CLIP_XMAX 512.0f
#define CLIP_YMIN 0.0f
#define CLIP_YMAX 384.0f
#define CLIP_WMIN 1.0f  
#define CLIP_WMAX 0.00f 
#endif
#define GUTTER 0.0f

#define	CLIP_OBJECT  0x0
#define	TRIVIAL_REJECTED 0x1
#define TRIVIAL_ACCEPTED 0x2

#include "..\include\ani3d.h"
/* clip triangle routine for SrVertexWithT type triangles */

void clip_triangle_t (ReducedVertex *in1, ReducedVertex *in2, ReducedVertex *in3);

/***** END of CLIP.H *********************************************************/

























								    