/*
 *		$Archive: /video/Nba/Include/ani3d.h $
 *		$Revision: 49 $
 *		$Date: 4/21/99 2:44p $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifndef __ANI3D_H__
#define __ANI3D_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_ani3d_h = "$Workfile: ani3d.h $ $Revision: 49 $";
#endif

#if defined(VEGAS)
#include	<glide.h>
#ifndef __TEXTURE_H__
#include	<goose/texture.h>
#endif
#else
#include <glide/glide.h>
#endif


// Field-Of-View correction factor from <Geom.S: xform_limb_fs>
//#define fovfac		(768.0f / 512.0f)
//#define fovfac		(1536.0f / 512.0f)

//#define GRAVITY	-0.110f
//#define GRAVITY	-0.08f
//#define GRAVITY	-0.07f
#define GRAVITY	-0.055f //.065f

/* 2d point */
typedef struct tagPOINT
{
	float	x,y;
} POINT;

/* animation sequence header */
typedef struct anim_header
{
	int		nframes;	/* frames in anim seq */
	int		nlimbs;		/* limbs in anim seq */
	int		offset;		/* file position of sequence */
	int		flags;		// AH_ flags defined below
} anihdr;

/* simple 4-vector */
typedef struct _vec3d
{
	float	x,y,z,w;
} vec3d;

typedef	struct	_MAT4 {
	float		xx;
	float		xy;
	float		xz;
	float		xw;
	float		yx;
	float		yy;
	float		yz;
	float		yw;
	float		zx;
	float		zy;
	float		zz;
	float		zw;
} MAT4;

/* 4-point spline struct */
typedef struct _SPLINE
{
	float	x1,y1,z1;		// starting pt (typically 0,0,0)
	float	x2,y2,z2;		// control pt 1
	float	x3,y3,z3;		// control pt 2
	float	x4,y4,z4;		// ending pt
} SPLINE;

/* cambot info & sequence-control struct */
typedef struct _CAMSEQINFO
{
	int		mode;			// camera mode
	int		mode_cnt;		// camera mode active tick count
	int		trav_cnt;		// travel time tick count
	float	theta;			// XZ-plane target angle
	float	phi;			// Y-elevation target angle
	float 	d;				// distance target from cambot to focus
	void	*pseqinfo;		// generic ptr for info spec data
	float 	trgy_adj;		// distance target from cambot to focus
} CAMSEQINFO;

/* camera struct */
typedef struct _VIEWCOORD
{
	CAMSEQINFO	csi;			// cambot info & sequence-control struct
	SPLINE		s;				// current spline track
	void		*pmodeobj;		// generic ptr for mode spec data
	int			mode_last;		// camera mode - last itteration
	int			mode_prev;		// camera mode - previous for resume mode
	int			tick_cnt;		// mode elapsed-time tick count
	int			pop_flg;		// pop-replay camera mode
	int			pop_flg_last;	// pop-replay camera mode - last itteration
	int			was_turnover;	// set if doing a turnover, clr if not
	int			new_quarter;	// set if a new quarter, clr if not
//	int			kickoff_flag;	// set if doing a full or quick kickoff
	int			off_prev;		// which side had the ball last time (0 or 1)
	float		theta_trg;		// XZ-plane target angle
	float		phi_trg;		// Y-elevation target angle
	float 		a,b,c;			// focus x,y,z position
	float 		e,f,g;			// point above focus in y on viewplane
	float 		x,y,z,d;		// camera x,y,z,distance position
	float		theta,phi;		// camera theta (XZ),phi (Y) position
	float 		xv,yv,zv,dv;	// camera x,y,z,distance movement velocities
	float 		tv,pv;			// camera theta (XZ),phi (Y) movement velocities
	float 		xa,ya,za,da;	// camera distance,x,y,z velocity accelerations
	float 		ta,pa;			// camera theta (XZ),phi (Y) velocity accelerations
	float		xform[12];		// matrix
	float		sin_theta;		// Sine of XZ-plane view angle
	float		cos_theta;		// Cosine of XZ-plane view angle
	float		sin_phi;		// Sine of Y-elevation view angle
	float		cos_phi;		// Cosine of Y-elevation view angle
//	float 		length;			// distance from cambot to viewplane
} VIEWCOORD;

typedef struct _ARROWINFO
{
	void *	arr_obj;
	void *	num_obj;
	void *	ppdata;
	float	plyr_sx;
	float	plyr_sy;
	float	plyr_sz;
//	float	arr_ax;
//	float	arr_ay;
//	float	num_ax;
// 	float	num_ay;
	int		ai_flags;
	int		timer;
	int		show;
#ifdef SHOW3DARROWS
	int		show3d;
#endif
} ARROWINFO;

// flags for ARROWINFO.flags
#define AF_INACTIVE			0x01	// arrow is not active
#define AF_ONSCREEN			0x02	// player is on-screen this tick
#define AF_OFFSCREENARROW	0x04	// player off-screen arrow was active

// cambot mode values
enum
{
// n-modes
	NM_RSNAP_NR = -3,
	NM_RSNAP_NFR,
	NM_DUNK,
// primary modes
	CAM_SIDE,
	CAM_HTIME,
	CAM_END,
	CAM_SHAWN,
	CAM_ATTRACT,
// transient modes
	CAM_ZOOM,
	CAM_ZO_NR,			// NR   - no ranging
	CAM_ZO_NFR,			// NFR  - no facing, no ranging
	CAM_SPLINE,
	CAM_SP_NR,			// NR   - no ranging
	CAM_SP_NFR,			// NFR  - no facing, no ranging
	CAM_SP_CNFR,		// CNFR - use last cambot, no facing, no ranging
	CAM_RAIL,
	CAM_RA_NF,			// NF   - no facing
	CAM_RA_NFT,			// NFT  - no facing, no tracking
	CAM_PATH,
	CAM_PA_NF,			// NF   - no facing
	CAM_PA_NFT,			// NFT  - no facing, no tracking
	CAM_COURSE,
	CAM_CO_C,			// C    - use last cambot
	CAM_CO_CNT,			// CNT  - use last cambot, no tracking
	CAM_P2P,
	CAM_P2P_X,			// X    - negate X data
	CAM_GOPHER,
	CAM_RESUME,
// total modes
	CAM_MODECNT,
// no-mode special cases
	NMODE_NOCSI,		// no CamSeqInfo
#ifdef SPECIAL_MODES
	CAM_HELMET
#endif //SPECIAL_MODES
};

typedef struct _STRIP_VERTS {
	short v;
	short st;
} STRIP_VERTS;


typedef struct _STRIP_INFO {
	short num_verts;
	short text;
} STRIP_INFO;


typedef struct _STRIP {
	int		num_strip_verts;
	int		num_strip_info;
	STRIP_VERTS *strip_verts;
	STRIP_INFO  *strip_info;
} STRIP;


/* model source data structures */
typedef struct _VERTEX
{
	float	x,y,z;
} VERTEX;

typedef struct _ST
{
	float	s,t;
} ST;

typedef struct _TRI
{
	unsigned short	v1,v2,v3;
	unsigned short	t1,t2,t3;
	short			texture;
} TRI;

typedef struct _LIMB
{
	int		nvtx;
	int		ntris;
    VERTEX	*pvn;
	VERTEX	*pvtx;
	ST		*pst;
	TRI		*ptri;
#ifdef JONHEY
	VERTEX  *bbox;
	STRIP	*strip;
#endif
} LIMB;

/* skeleton node */
typedef struct _SNODE
{
	float		x,y,z;			/* joint position (relative to parent) */
	int			flags;			/* flags */
	int			children[3];	/* child node indices (self-relative, 0 if none) */
} SNODE;


typedef struct
{
	float	x;
	float	y;
	float	oow;
	float	sow;
	float	tow;
} ReducedVertex;


#define JONHEY_MODE_CLEAR      0x0
#define JONHEY_MODE_PLAYER     0x1
#define JONHEY_MODE_DBL_SIDED  0x2
#define JONHEY_MODE_NO_CULL	   0x4

#ifdef JONHEY
typedef	struct jonhey
{
	LIMB			*limb;
	float 			m[12];
	Texture_node_t 	**text;
	int 			mode;
} jonhey_t;



typedef struct crowd_bbox
{
	int offset;
	int size;
	VERTEX  *bbox;
}crowd_bbox_t;

#endif


// There can be lots of 3D object type structs, but they all should have one
// of these structs as their first elements.  Data that's particular to the
// type of object goes after the obj_3d.  Data that's universal to all 3D
// objects goes inside the 3d object struct.  All calls to/from the anim
// system use type *obj_3d.  Calling/called functions can cast the pointer
// back to their more specific type if'n they need access to the extra data.
// IF THE OBJECT ANIMATION DATA MAY BE COMPRESSED OR INTERPOLATED, you have to
// include space for the uncompressed quats inside the enclosing data structure
// immediately after the obj_3d struct.
// NOTE ON INTERPOLATION:  The target frame must be uncompressed.  Therefore,
// if the target sequence is compressed, only the first frame is a legal dest.
// Source frame may be compressed.
// FOR STREAM INTERPOLATION: The sequence must be uncompressed.

typedef struct _anidata
{
	int			animode;		// animation mode
	int			hackval;		// MODE_TURNHACK arg
	char		*pscrhdr;		// animation script header (source code)
	char		*pscript;		// animation script pointer
	int			seq_index;		// sequence index (from xxxxxSEQ.H)
	int			seq_index2;		// interpolation source frame seq index
	int			anicnt;			// ticks to hold this frame
	float		*pframe0;		// frame 0 (float) pointer
	char		*pframe;		// head of frame (int) data
	float		*pxlate;		// translation (animpt) data
	int			iframe;			// frame number
	float		fframe;			// frame number (float format, iframe is rounded down)
								// fframe is only valid when animode & MODE_INTSTREAM
	anihdr		*pahdr;			// ani sequence header ptr (data)
	float		*pq;			// ptr to current frame data (script or object struct ram)
	float		*adata;			// ptr to anim data block (decompression scratch pad)
	float		*idata;			// ptr to interpolation base data
	int			icnt;			// interpolation counter
	int			scnt;			// stream interpolation counter
	float		sinc;			// stream interpolation frame increment
	int			vframe;			// frame index for ANI_INTTOSEQ.  Used with AA_VAR_FRAME flag.
	int			vcount;			// tick count for ANI_INTTOSEQ.  Used with AA_VAR_COUNT flag.
} anidata;

// Event position data for replay; top of <obj_3d> must match!
typedef struct _pos_3d
{
	float		x,y,z;			// world position
	int			fwd;			// facing direction (0-1023)
} pos_3d;

typedef struct _obj_3d
{
	float		x,y,z;			// world position
	int			fwd;			// facing direction (0-1023)
	float		vx,vy,vz;		// velocity
	float		friction;		// applied to X & Z velocities every tick.
	float		vxf,vzf; 		// facing-relative velocities (+z is forward)
	float		ax,ay,az;		// prev frame ani x & z
	float		ascale;			// x & z animpt scale factor
	float		xscale;			// x & z animpt scale factor
	float		yscale;			// x & z animpt scale factor
	float		zscale;			// x & z animpt scale factor
	int			tgt_fwd;		// intended facing direction (0-1023)
	int			turn_rate;		// turn rate per tick (reset any time you change tgt_fwd)
	int			fwd2;			// second facing direction (0-1023) (mostly unused)
	int			tgt_fwd2;		// second intended facing direction (0-1023)
	int			turn_rate2;		// second turn rate per tick
	anidata		adata[4];		// animation data (two parts, +2 for interpolation)
	int			plyrmode;		// player mode
#if defined(VEGAS)
	Texture_node_t	**pdecal;		// array of decal indices
#else
	GrMipMapId_t *pdecal;		// array of decal indices
#endif
	int			flags;			// general object flags (defined in player.h)
	struct _obj_3d *pobj_hit;	// this is set to point to the last fbplyr to collide with me
} obj_3d;

//
// script interpreter function prototypes
//
void anim_sys_init( void );
int load_anim_block( int, char *, int );
void change_anim( obj_3d *, char * );
void change_anim1( obj_3d *, char * );
void change_anim2( obj_3d *, char * );
void animate_model( obj_3d * );

//
// 3D pipeline & associated function prototypes
//
#if defined(VEGAS)
void load_textures( char *[], int *, Texture_node_t **, int, int, int, int );
#else
void load_textures( char *[], int *, GrMipMapId_t *, int, int, int, int );
#endif
void load_head_textures(char *tex_list[], int *ratios, Texture_node_t **decals,
						int max_decals, int id, int minfilt, int maxfilt, int index);
void render_node( SNODE *, LIMB **, float *, float *, Texture_node_t **, float * );
void generate_matrices( SNODE *, float *, float *, float *, float * , float , float );
void generate_matrices_flipy( SNODE *, float *, float *, float *, float * , float , float, float *,float * );

void render_node_mpc( SNODE *, LIMB **, float *, Texture_node_t **, float * );
void render_limb( LIMB *, float *, Texture_node_t **, int * );
void render_limb_noclip( LIMB *, float *, Texture_node_t **, int * );

void render_limb_jers( jonhey_t *obj, int d1, int d2);

void render_plate( LIMB *, float *, float *, Texture_node_t **, int * );

void mxm( float *m1, float *m2, float *m3 );
void qxq( float *, float *, float * );
void mat2quat( float *mat, float *q );
void slerp( float *, float *, float, float * );
void adjust_quat( float *, float * );
//float fsqrt( float );
//float rsqrt( float f );
void quat2mat( float *q, float *mat );
float isin( int );
float icos( int );
int iasin( float );
int iacos( float );
float sin_rad( float );
float cos_rad( float );
float asin_rad( float );
float acos_rad( float );
void rotx( int, float * );
void roty( int, float * );
void rotz( int, float * );
void rotxyz( int, int, int, float * );
void vxv( vec3d *, vec3d *, vec3d * );
void norm( vec3d * );
float magv( vec3d * );
float det( float * );

void init_geom( int lo_res );
extern int (* object_clip)(jonhey_t *obj);
extern void (* transform_verts)(jonhey_t *obj);

#ifndef NULL
#define NULL 0
#endif

//
// skeleton node flags
//

#define SN_DBLSIDE_BIT			0					// limb is double-sided
#define SN_DBLSIDE				(1<<SN_DBLSIDE_BIT)

#define SN_NODRAW_BIT			1					// don't draw limb
#define SN_NODRAW				(1<<SN_NODRAW_BIT)

#define SN_HEAD_SCALE_BIT		2					// dynamic scale factor
#define SN_HEAD_SCALE			(1<<SN_HEAD_SCALE_BIT)
#define SN_FEET_SCALE_BIT		3					// dynamic scale factor
#define SN_FEET_SCALE			(1<<SN_FEET_SCALE_BIT)

#define SN_TURBO_LIGHT_BIT		4					// dynamic HIGHLIGHTING
#define SN_TURBO_LIGHT			(1<<SN_TURBO_LIGHT_BIT)

#define SN_TURBO_COLOR_RED		(1<<5)	// dynamic HIGHLIGHT COLORS
#define SN_TURBO_COLOR_YELLOW	(1<<6)
#define SN_TURBO_COLOR_BLUE		(1<<7)
#define SN_TURBO_COLOR_GREEN	(1<<8)
#define SN_TURBO_ALLCOLORS		(SN_TURBO_COLOR_RED|SN_TURBO_COLOR_YELLOW|SN_TURBO_COLOR_BLUE|SN_TURBO_COLOR_GREEN)

// ani header flags
#define AH_COMPRESSED		0x1

//
// animation modes -- duplicated in ani3d.equ
//
#define MODE_END			0x0001

// don't let player interrupt sequence
#define MODE_UNINT			0x0002

// in two-part anim mode (set this on the first part)
#define MODE_TWOPART		0x0004

// in streaming interpolation mode
#define MODE_INTSTREAM		0x0008

// interpolate at next sequence transition (if supported by new sequence)
// use ANI_PREP_XITION to set this flag, target frame, and tick count.
#define MODE_INTXITION		0x0010

// Allow limited stick control.  Player can control direction, but
// run functions won't change the anim sequence.
#define MODE_ROTONLY        0x0020

// Zero bit.  Set whenever an ANI_CODE returns 0, cleared when one returns
// nonzero.
#define MODE_ZERO			0x0040

// Reacting to a no-mode attack.  Immune to further such attacks.
#define MODE_REACTING		0x0080

// Can pick up a block even if UNINT
#define MODE_CANBLOCK		0x0100

// No cylinder interaction with other players
#define MODE_NOCYL			0x0200

// Player is stiff arming someone (this is an ani-code cause it deals with two part...and it get knocked outta seq...)
#define MODE_STIFF_ARM		0x0400

// top 16 bits of mode flags must be individually set and cleared.
// ANI_SETMODE(mask) will result in two operations.  The low halfword
// of the mask will be assigned to the low halfword of the mode word.
// The high halfword will be ORed in.  The idea is that if you've got
// a mode like MODE_ANCHOR that you don't want gettting wiped out on
// every ANI_SETMODE command, put it in the high halfword.

// hold ref point (instead of anim pt) steady at next frame change
#define MODE_ANCHORY		0x00010000
#define MODE_ANCHORXZ		0x00020000
#define MODE_ANCHOR			(MODE_ANCHORY|MODE_ANCHORXZ)

// not the primary animation -- ignore translation
#define MODE_SECONDARY		0x00040000

// interpolating
#define MODE_INTERP			0x00080000

// noisy debugging
#define MODE_ANIDEBUG		0x00100000

// next time you interpolate, rotate the src root quat by hackval
#define MODE_TURNHACK		0x00200000

// puppet move.  various effects
#define MODE_PUPPETEER		0x00400000
#define MODE_PUPPET			0x00800000

//
// anim block indices -- duplicated in ani3d.equ
//
#define ASB_GEN_SEQ		0
#define	ASB_LAST		ASB_GEN_SEQ

// anim script tag
#define	SCRIPT			0x80

//
// special arguments to ANI commands -- duplicated & documented in ani3d.equ
#define	AA_PROPFRAME		-1
#define AA_VAR_FRAME		-2
#define AA_VAR_COUNT		-1
#define AA_TOGROUND			-4
#define AA_TOEND			-2
#define AA_AIRTIME			-3
#define AA_BALL_AIRTIME		-5
#define AA_RAND				-1

#define AA_TGT_CARRIER		-1
#define AA_TGT_TARGET		-2

// commands are defined in ani3d.equ

#endif
