/****************************************************************
*		$Archive: /video/Nba/Include/particle.h $
*		$Revision: 9 $
*		$Date: 3/22/99 4:39p $
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

#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_particle_h = "$Workfile: particle.h $ $Revision: 9 $";
#endif

#include "system.h"

//컴컴컴컴컴컴컴� Image definitions

#define	PTLI_X1		0		// Index
#define	PTLIF_X1		21	// # frames
#define	PTLI_XA		1
#define	PTLIF_XA		10
#define	PTLI_XB		2
#define	PTLIF_XB		10
#define	PTLI_XC		3
#define	PTLIF_XC		10
#define	PTLI_FLASHA	4
#define	PTLIF_FLASHA	10
#define	PTLI_SPRKA	5
#define	PTLIF_SPRKA	10
#define	PTLI_BALLBLUR	6
#define	PTLI_RAIN	7
#define	PTLI_SNOW	8

typedef	struct	_PTLIMG {	// Particle
	int				txnum;		// Texture index
	texture_node_t *txn_p;		// * texture
	image_info_t	**imgi_p;	// * img info or 0 to use whole texture
	int				pad;
} PTLIMG;

#define	PTLIMG(i)	(&ptlimg_t[i])
extern PTLIMG	ptlimg_t[];

//컴컴컴컴컴컴컴�

typedef	void	PTLFRET;
typedef	PTLFRET	(PTLFUNC)(void *);

#define	PTLMAX		1200

#define M_PTLF_BLENDA	1		// Additive blending mode
#define M_PTLF_TEX		2		// Tex * constant mode
#define M_PTLF_TRI		4		// Triangular particle else quad
#define M_PTLF_CLIPQ		8		// Quick presort clip. Use on small particles
#define M_PTLF_PLAYBACK	0x10	// Particle created in replay flag

typedef	struct	_PTL {			// Particle
 	struct _PTL *nxt_p;		// Link *
	float			x;				// Position
	float			y;
	float			z;
	float			vx;			// Velocity
	float			vy;
	float			vz;
	float			ay;			// Y acceleration (gravity)
	float			drag;			// Velocity drag
	float			rad;			// Radius
	float			radv;			// Radius velocity
	float			alpha;		// 0-255
	float			alphav;		// Alpha velocity
	uint			color;		// 0RGB
	uint			flgs;			// Flags
	int			fuel;			// >0 = Has fuel
	int			frm;			// Image frame number (0 to Max-1)
	texture_node_t *txn_p;	// * texture
 	PTLIMG		*img_p;		// * img or 0 to use whole texture
	PTLFUNC		*func_p;		// * callback function or 0
	union {						// For use by functions
		void		*v_p;
		int		i;
		float		f;
	} data1;
	union {
		void		*v_p;
		int		i;
		float		f;
	} data2;
	union {
		void		*v_p;
		int		i;
		float		f;
	} data3;
	float			tx;			// Temp position (tranformed)
	float			ty;
} PTL;

void	ptl_init();
void	ptl_shutdown();
PTL	*ptl_get (PTLFUNC *);
void	ptl_free (PTL *p_p);
void	ptl_draw();

/****************************************************************/
#endif
