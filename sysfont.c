/*
 *		$Archive: /video/Nba/sysfont.c $
 *		$Revision: 27 $
 *		$Date: 4/21/99 1:16p $
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
char *ss_sysfont_c = "$Workfile: sysfont.c $ $Revision: 27 $";
#endif

/*
 *		USER INCLUDES
 */

#include	<goose/goose.h>

#define	IMAGEDEFINE	1
#include	"include\nbafont.h"
#include	"include\bigfnt.h"

static image_info_t	*bast7t_font_table[] = {
&bast7texc,				/* ! - 0x21 */
(void *)0,				/* " - 0x22 */
&bast7tnum,				/* # - 0x23 */
&bast7tdol,				/* $ - 0x24 */
&bast7tpct,				/* % - 0x25 */
&bast7tand,				/* & - 0x26 */
&bast7tapo,				/* ' - 0x27 */
&bast7tlpr,				/* ( - 0x28 */
&bast7trpr,				/* ) - 0x29 */
(void *)0,				/* * - 0x2a */
&bast7tpls,				/* + - 0x2b */
&bast7tcom,				/* , - 0x2c */
&bast7tdsh,				/* - - 0x2d */
&bast7tper,				/* . - 0x2e */
&bast7tsls,				/* / - 0x2f */
&bast7t_0,				/* 0 - 0x30 */
&bast7t_1,				/* 1 - 0x31 */
&bast7t_2,				/* 2 - 0x32 */
&bast7t_3,				/* 3 - 0x33 */
&bast7t_4,				/* 4 - 0x34 */
&bast7t_5,				/* 5 - 0x35 */
&bast7t_6,				/* 6 - 0x36 */
&bast7t_7,				/* 7 - 0x37 */
&bast7t_8,				/* 8 - 0x38 */
&bast7t_9,				/* 9 - 0x39 */
&bast7tcol,				/* : - 0x3a */
&bast7tsem,				/* ; - 0x3b */
(void *)0,				/* < - 0x3c */
(void *)0,				/* = - 0x3d */
(void *)0,				/* > - 0x3e */
&bast7tque,				/* ? - 0x3f */
(void *)0,				/* @ - 0x40 */
&bast7t_a,				/* A - 0x41 */
&bast7t_b,				/* B - 0x42 */
&bast7t_c,				/* C - 0x43 */
&bast7t_d,				/* D - 0x44 */
&bast7t_e,				/* E - 0x45 */
&bast7t_f,				/* F - 0x46 */
&bast7t_g,				/* G - 0x47 */
&bast7t_h,				/* H - 0x48 */
&bast7t_i,				/* I - 0x49 */
&bast7t_j,				/* J - 0x4a */
&bast7t_k,				/* K - 0x4b */
&bast7t_l,				/* L - 0x4c */
&bast7t_m,				/* M - 0x4d */
&bast7t_n,				/* N - 0x4e */
&bast7t_o,				/* O - 0x4f */
&bast7t_p,				/* P - 0x50 */
&bast7t_q,				/* Q - 0x51 */
&bast7t_r,				/* R - 0x52 */
&bast7t_s,				/* S - 0x53 */
&bast7t_t,				/* T - 0x54 */
&bast7t_u,				/* U - 0x55 */
&bast7t_v,				/* V - 0x56 */
&bast7t_w,				/* W - 0x57 */
&bast7t_x,				/* X - 0x58 */
&bast7t_y,				/* Y - 0x59 */
&bast7t_z,				/* Z - 0x5a */
(void *)0,				/* [ - 0x5b */
(void *)0,				/* \ - 0x5c */
(void *)0,				/* ] - 0x5d */
(void *)0,				/* ^ - 0x5e */
(void *)0,				/*   - 0x5f */
(void *)0,				/*   - 0x60 */
&bast7t_a,				/* a - 0x61 */
&bast7t_b,				/* b - 0x62 */
&bast7t_c,				/* c - 0x63 */
&bast7t_d,				/* d - 0x64 */
&bast7t_e,				/* e - 0x65 */
&bast7t_f,				/* f - 0x66 */
&bast7t_g,				/* g - 0x67 */
&bast7t_h,				/* h - 0x68 */
&bast7t_i,				/* i - 0x69 */
&bast7t_j,				/* j - 0x6a */
&bast7t_k,				/* k - 0x6b */
&bast7t_l,				/* l - 0x6c */
&bast7t_m,				/* m - 0x6d */
&bast7t_n,				/* n - 0x6e */
&bast7t_o,				/* o - 0x6f */
&bast7t_p,				/* p - 0x70 */
&bast7t_q,				/* q - 0x71 */
&bast7t_r,				/* r - 0x72 */
&bast7t_s,				/* s - 0x73 */
&bast7t_t,				/* t - 0x74 */
&bast7t_u,				/* u - 0x75 */
&bast7t_v,				/* v - 0x76 */
&bast7t_w,				/* w - 0x77 */
&bast7t_x,				/* x - 0x78 */
&bast7t_y,				/* y - 0x79 */
&bast7t_z				/* z - 0x7a */
/* { - 0x7b */
/* | - 0x7c */
/* } - 0x7d */
/* ~ - 0x7e */
};
static image_info_t	*bast7w_font_table[] = {
&bast7wexc,				/* ! - 0x21 */
(void *)0,				/* " - 0x22 */
&bast7wnum,				/* # - 0x23 */
&bast7wdol,				/* $ - 0x24 */
&bast7wpct,				/* % - 0x25 */
&bast7wand,				/* & - 0x26 */
&bast7wapo,				/* ' - 0x27 */
&bast7wlpr,				/* ( - 0x28 */
&bast7wrpr,				/* ) - 0x29 */
(void *)0,				/* * - 0x2a */
&bast7wpls,				/* + - 0x2b */
&bast7wcom,				/* , - 0x2c */
&bast7wdsh,				/* - - 0x2d */
&bast7wper,				/* . - 0x2e */
&bast7wsls,				/* / - 0x2f */
&bast7w_0,				/* 0 - 0x30 */
&bast7w_1,				/* 1 - 0x31 */
&bast7w_2,				/* 2 - 0x32 */
&bast7w_3,				/* 3 - 0x33 */
&bast7w_4,				/* 4 - 0x34 */
&bast7w_5,				/* 5 - 0x35 */
&bast7w_6,				/* 6 - 0x36 */
&bast7w_7,				/* 7 - 0x37 */
&bast7w_8,				/* 8 - 0x38 */
&bast7w_9,				/* 9 - 0x39 */
&bast7wcol,				/* : - 0x3a */
&bast7wsem,				/* ; - 0x3b */
(void *)0,				/* < - 0x3c */
(void *)0,				/* = - 0x3d */
(void *)0,				/* > - 0x3e */
&bast7wque,				/* ? - 0x3f */
(void *)0,				/* @ - 0x40 */
&bast7w_a,				/* A - 0x41 */
&bast7w_b,				/* B - 0x42 */
&bast7w_c,				/* C - 0x43 */
&bast7w_d,				/* D - 0x44 */
&bast7w_e,				/* E - 0x45 */
&bast7w_f,				/* F - 0x46 */
&bast7w_g,				/* G - 0x47 */
&bast7w_h,				/* H - 0x48 */
&bast7w_i,				/* I - 0x49 */
&bast7w_j,				/* J - 0x4a */
&bast7w_k,				/* K - 0x4b */
&bast7w_l,				/* L - 0x4c */
&bast7w_m,				/* M - 0x4d */
&bast7w_n,				/* N - 0x4e */
&bast7w_o,				/* O - 0x4f */
&bast7w_p,				/* P - 0x50 */
&bast7w_q,				/* Q - 0x51 */
&bast7w_r,				/* R - 0x52 */
&bast7w_s,				/* S - 0x53 */
&bast7w_t,				/* T - 0x54 */
&bast7w_u,				/* U - 0x55 */
&bast7w_v,				/* V - 0x56 */
&bast7w_w,				/* W - 0x57 */
&bast7w_x,				/* X - 0x58 */
&bast7w_y,				/* Y - 0x59 */
&bast7w_z,				/* Z - 0x5a */
(void *)0,				/* [ - 0x5b */
(void *)0,				/* \ - 0x5c */
(void *)0,				/* ] - 0x5d */
(void *)0,				/* ^ - 0x5e */
(void *)0,				/*   - 0x5f */
(void *)0,				/*   - 0x60 */
&bast7w_a,				/* a - 0x61 */
&bast7w_b,				/* b - 0x62 */
&bast7w_c,				/* c - 0x63 */
&bast7w_d,				/* d - 0x64 */
&bast7w_e,				/* e - 0x65 */
&bast7w_f,				/* f - 0x66 */
&bast7w_g,				/* g - 0x67 */
&bast7w_h,				/* h - 0x68 */
&bast7w_i,				/* i - 0x69 */
&bast7w_j,				/* j - 0x6a */
&bast7w_k,				/* k - 0x6b */
&bast7w_l,				/* l - 0x6c */
&bast7w_m,				/* m - 0x6d */
&bast7w_n,				/* n - 0x6e */
&bast7w_o,				/* o - 0x6f */
&bast7w_p,				/* p - 0x70 */
&bast7w_q,				/* q - 0x71 */
&bast7w_r,				/* r - 0x72 */
&bast7w_s,				/* s - 0x73 */
&bast7w_t,				/* t - 0x74 */
&bast7w_u,				/* u - 0x75 */
&bast7w_v,				/* v - 0x76 */
&bast7w_w,				/* w - 0x77 */
&bast7w_x,				/* x - 0x78 */
&bast7w_y,				/* y - 0x79 */
&bast7w_z				/* z - 0x7a */
};

static image_info_t	*bast8t_font_table[] = {
&bast8texc,				/* ! - 0x21 */
(void *)0,				/* " - 0x22 */
&bast8tnum,				/* # - 0x23 */
&bast8tdol,				/* $ - 0x24 */
&bast8tpct,				/* % - 0x25 */
&bast8tand,				/* & - 0x26 */
&bast8tapo,				/* ' - 0x27 */
&bast8tprl,				/* ( - 0x28 */
&bast8tprr,				/* ) - 0x29 */
&bast8tast,				/* * - 0x2a */
&bast8tpls,				/* + - 0x2b */
&bast8tcom,				/* , - 0x2c */
&bast8tdas,				/* - - 0x2d */
&bast8tper,				/* . - 0x2e */
&bast8tsls,				/* / - 0x2f */
&bast8t_0,				/* 0 - 0x30 */
&bast8t_1,				/* 1 - 0x31 */
&bast8t_2,				/* 2 - 0x32 */
&bast8t_3,				/* 3 - 0x33 */
&bast8t_4,				/* 4 - 0x34 */
&bast8t_5,				/* 5 - 0x35 */
&bast8t_6,				/* 6 - 0x36 */
&bast8t_7,				/* 7 - 0x37 */
&bast8t_8,				/* 8 - 0x38 */
&bast8t_9,				/* 9 - 0x39 */
&bast8tcol,				/* : - 0x3a */
&bast8tsem,				/* ; - 0x3b */
(void *)0,				/* < - 0x3c */
(void *)0,				/* = - 0x3d */
(void *)0,				/* > - 0x3e */
&bast8tque,				/* ? - 0x3f */
(void *)0,				/* @ - 0x40 */
&bast8t_a,				/* A - 0x41 */
&bast8t_b,				/* B - 0x42 */
&bast8t_c,				/* C - 0x43 */
&bast8t_d,				/* D - 0x44 */
&bast8t_e,				/* E - 0x45 */
&bast8t_f,				/* F - 0x46 */
&bast8t_g,				/* G - 0x47 */
&bast8t_h,				/* H - 0x48 */
&bast8t_i,				/* I - 0x49 */
&bast8t_j,				/* J - 0x4a */
&bast8t_k,				/* K - 0x4b */
&bast8t_l,				/* L - 0x4c */
&bast8t_m,				/* M - 0x4d */
&bast8t_n,				/* N - 0x4e */
&bast8t_o,				/* O - 0x4f */
&bast8t_p,				/* P - 0x50 */
&bast8t_q,				/* Q - 0x51 */
&bast8t_r,				/* R - 0x52 */
&bast8t_s,				/* S - 0x53 */
&bast8t_t,				/* T - 0x54 */
&bast8t_u,				/* U - 0x55 */
&bast8t_v,				/* V - 0x56 */
&bast8t_w,				/* W - 0x57 */
&bast8t_x,				/* X - 0x58 */
&bast8t_y,				/* Y - 0x59 */
&bast8t_z,				/* Z - 0x5a */
(void *)0,				/* [ - 0x5b */
(void *)0,				/* \ - 0x5c */
(void *)0,				/* ] - 0x5d */
(void *)0,				/* ^ - 0x5e */
&bast8tdas,				/*   - 0x5f */
(void *)0,				/*   - 0x60 */
&bast8t_a,				/* a - 0x61 */
&bast8t_b,				/* b - 0x62 */
&bast8t_c,				/* c - 0x63 */
&bast8t_d,				/* d - 0x64 */
&bast8t_e,				/* e - 0x65 */
&bast8t_f,				/* f - 0x66 */
&bast8t_g,				/* g - 0x67 */
&bast8t_h,				/* h - 0x68 */
&bast8t_i,				/* i - 0x69 */
&bast8t_j,				/* j - 0x6a */
&bast8t_k,				/* k - 0x6b */
&bast8t_l,				/* l - 0x6c */
&bast8t_m,				/* m - 0x6d */
&bast8t_n,				/* n - 0x6e */
&bast8t_o,				/* o - 0x6f */
&bast8t_p,				/* p - 0x70 */
&bast8t_q,				/* q - 0x71 */
&bast8t_r,				/* r - 0x72 */
&bast8t_s,				/* s - 0x73 */
&bast8t_t,				/* t - 0x74 */
&bast8t_u,				/* u - 0x75 */
&bast8t_v,				/* v - 0x76 */
&bast8t_w,				/* w - 0x77 */
&bast8t_x,				/* x - 0x78 */
&bast8t_y,				/* y - 0x79 */
&bast8t_z				/* z - 0x7a */
/* { - 0x7b */
/* | - 0x7c */
/* } - 0x7d */
/* ~ - 0x7e */
};

static image_info_t	*bast75_font_table[] = {
 	&bas75exc,				/* ! - 0x21 */
 	(void *)0,				/* " - 0x22 */
 	&bas75num,				/* # - 0x23 */
 	&bas75dol,				/* $ - 0x24 */
 	&bas75pct,				/* % - 0x25 */
 	&bas75and,				/* & - 0x26 */
 	&bas75apo,				/* ' - 0x27 */
 	&bas75prl,				/* ( - 0x28 */
 	&bas75prr,				/* ) - 0x29 */
 	(void *)0,				/* * - 0x2a */
 	&bas75add,				/* + - 0x2b */
 	&bas75com,				/* , - 0x2c */
 	&bas75dsh,				/* - - 0x2d */
 	&bas75per,				/* . - 0x2e */
 	&bas75sls,				/* / - 0x2f */
 	&bast75_0,				/* 0 - 0x30 */
 	&bast75_1,				/* 1 - 0x31 */
 	&bast75_2,				/* 2 - 0x32 */
 	&bast75_3,				/* 3 - 0x33 */
 	&bast75_4,				/* 4 - 0x34 */
 	&bast75_5,				/* 5 - 0x35 */
 	&bast75_6,				/* 6 - 0x36 */
 	&bast75_7,				/* 7 - 0x37 */
 	&bast75_8,				/* 8 - 0x38 */
 	&bast75_9,				/* 9 - 0x39 */
 	&bas75col,				/* : - 0x3a */
 	&bas75com,				/* ; - 0x3b */
 	(void *)0,				/* < - 0x3c */
 	(void *)0,				/* = - 0x3d */
 	(void *)0,				/* > - 0x3e */
 	&bas75que,				/* ? - 0x3f */
 	(void *)0,				/* @ - 0x40 */
 	&bast75_a,				/* A - 0x41 */
 	&bast75_b,				/* B - 0x42 */
 	&bast75_c,				/* C - 0x43 */
 	&bast75_d,				/* D - 0x44 */
 	&bast75_e,				/* E - 0x45 */
 	&bast75_f,				/* F - 0x46 */
 	&bast75_g,				/* G - 0x47 */
 	&bast75_h,				/* H - 0x48 */
 	&bast75_i,				/* I - 0x49 */
 	&bast75_j,				/* J - 0x4a */
 	&bast75_k,				/* K - 0x4b */
 	&bast75_l,				/* L - 0x4c */
 	&bast75_m,				/* M - 0x4d */
 	&bast75_n,				/* N - 0x4e */
 	&bast75_o,				/* O - 0x4f */
 	&bast75_p,				/* P - 0x50 */
 	&bast75_q,				/* Q - 0x51 */
 	&bast75_r,				/* R - 0x52 */
 	&bast75_s,				/* S - 0x53 */
 	&bast75_t,				/* T - 0x54 */
 	&bast75_u,				/* U - 0x55 */
 	&bast75_v,				/* V - 0x56 */
 	&bast75_w,				/* W - 0x57 */
 	&bast75_x,				/* X - 0x58 */
 	&bast75_y,				/* Y - 0x59 */
 	&bast75_z,				/* Z - 0x5a */
 	(void *)0,				/* [ - 0x5b */
 	(void *)0,				/* \ - 0x5c */
 	(void *)0,				/* ] - 0x5d */
 	(void *)0,				/* ^ - 0x5e */
 	(void *)0,				/*   - 0x5f */
 	(void *)0,				/*   - 0x60 */
 	&bast75_a,				/* a - 0x61 */
 	&bast75_b,				/* b - 0x62 */
 	&bast75_c,				/* c - 0x63 */
 	&bast75_d,				/* d - 0x64 */
 	&bast75_e,				/* e - 0x65 */
 	&bast75_f,				/* f - 0x66 */
 	&bast75_g,				/* g - 0x67 */
 	&bast75_h,				/* h - 0x68 */
 	&bast75_i,				/* i - 0x69 */
 	&bast75_j,				/* j - 0x6a */
 	&bast75_k,				/* k - 0x6b */
 	&bast75_l,				/* l - 0x6c */
 	&bast75_m,				/* m - 0x6d */
 	&bast75_n,				/* n - 0x6e */
 	&bast75_o,				/* o - 0x6f */
 	&bast75_p,				/* p - 0x70 */
 	&bast75_q,				/* q - 0x71 */
 	&bast75_r,				/* r - 0x72 */
 	&bast75_s,				/* s - 0x73 */
 	&bast75_t,				/* t - 0x74 */
 	&bast75_u,				/* u - 0x75 */
 	&bast75_v,				/* v - 0x76 */
 	&bast75_w,				/* w - 0x77 */
 	&bast75_x,				/* x - 0x78 */
 	&bast75_y,				/* y - 0x79 */
 	&bast75_z					/* z - 0x7a */
 	/* { - 0x7b */
 	/* | - 0x7c */
 	/* } - 0x7d */
 	/* ~ - 0x7e */
};

/* bast7 thin font definition */
font_info_t	bast7t_font = {
sizeof(bast7t_font_table)/sizeof(void *),
0x21,
0x7a,
8,
0x5,
0x0,
nbafn4_tex_name,
bast7t_font_table
};

/* bast7 wide font definition */
font_info_t	bast7w_font = {
sizeof(bast7w_font_table)/sizeof(void *),
0x21,
0x7a,
8,
0x5,
0x0,
nbafn4_tex_name,
bast7w_font_table
};

/* bast8 thin font definition */
font_info_t	bast8t_font = {
sizeof(bast8t_font_table)/sizeof(void *),
0x21,
0x7a,
9,
0x5,
0x0,
nbafn4_tex_name,
bast8t_font_table
};


/*  bast75 (big) font definition */
font_info_t	bast75_font = {
sizeof(bast75_font_table)/sizeof(void *),
0x21,
0x7a,
75,				/* maximum character height */
24,				/* space in pixels */
-6,				/* inter character spacing */
bigfnt00_tex_name,
bast75_font_table
};

/* NBA style fonts */
static image_info_t	*nba10_font_table[] = {
	&int10_exc,				/* ! - 0x21 */
	&int10_qota,			/* " - 0x22 */
	&int10_pnd,				/* # - 0x23 */
	&int10_csh,				/* $ - 0x24 */
	&int10_prct,			/* % - 0x25 */
	&int10_nnd,				/* & - 0x26 */
	&int10_qotb,			/* ' - 0x27 */
	&int10_lpar,			/* ( - 0x28 */
	&int10_rpar,			/* ) - 0x29 */
	&int10_ast,				/* * - 0x2a */
	&int10_pls,				/* + - 0x2b */
	&int10_com,				/* , - 0x2c */
	&int10_dsh,				/* - - 0x2d */
	&int10_per,				/* . - 0x2e */
	&int10_sls,				/* / - 0x2f */
	&int10_0,				/* 0 - 0x30 */
	&int10_1,				/* 1 - 0x31 */
	&int10_2,				/* 2 - 0x32 */
	&int10_3,				/* 3 - 0x33 */
	&int10_4,				/* 4 - 0x34 */
	&int10_5,				/* 5 - 0x35 */
	&int10_6,				/* 6 - 0x36 */
	&int10_7,				/* 7 - 0x37 */
	&int10_8,				/* 8 - 0x38 */
	&int10_9,				/* 9 - 0x39 */
	&int10_col,				/* : - 0x3a */
	&int10_sem,				/* ; - 0x3b */
	&int10_arwl,			/* < - 0x3c */
	&int10_eql,				/* = - 0x3d */
	&int10_arwr,			/* > - 0x3e */
	&int10_que,				/* ? - 0x3f */
	&int10_ampr,			/* @ - 0x40 */
	&int10_a,				/* A - 0x41 */
	&int10_b,				/* B - 0x42 */
	&int10_c,				/* C - 0x43 */
	&int10_d,				/* D - 0x44 */
	&int10_e,				/* E - 0x45 */
	&int10_f,				/* F - 0x46 */
	&int10_g,				/* G - 0x47 */
	&int10_h,				/* H - 0x48 */
	&int10_i,				/* I - 0x49 */
	&int10_j,				/* J - 0x4a */
	&int10_k,				/* K - 0x4b */
	&int10_l,				/* L - 0x4c */
	&int10_m,				/* M - 0x4d */
	&int10_n,				/* N - 0x4e */
	&int10_o,				/* O - 0x4f */
	&int10_p,				/* P - 0x50 */
	&int10_q,				/* Q - 0x51 */
	&int10_r,				/* R - 0x52 */
	&int10_s,				/* S - 0x53 */
	&int10_t,				/* T - 0x54 */
	&int10_u,				/* U - 0x55 */
	&int10_v,				/* V - 0x56 */
	&int10_w,				/* W - 0x57 */
	&int10_x,				/* X - 0x58 */
	&int10_y,				/* Y - 0x59 */
	&int10_z,				/* Z - 0x5a */
	NULL,					/* [ - 0x5b */
	NULL,					/* \ - 0x5c */
	NULL,					/* ] - 0x5d */
	NULL,					/* ^ - 0x5e */
	&int10_dsh,				/*   - 0x5f */
	NULL,					/*   - 0x60 */
	&int10_a,				/* a - 0x61 */
	&int10_b,				/* b - 0x62 */
	&int10_c,				/* c - 0x63 */
	&int10_d,				/* d - 0x64 */
	&int10_e,				/* e - 0x65 */
	&int10_f,				/* f - 0x66 */
	&int10_g,				/* g - 0x67 */
	&int10_h,				/* h - 0x68 */
	&int10_i,				/* i - 0x69 */
	&int10_j,				/* j - 0x6a */
	&int10_k,				/* k - 0x6b */
	&int10_l,				/* l - 0x6c */
	&int10_m,				/* m - 0x6d */
	&int10_n,				/* n - 0x6e */
	&int10_o,				/* o - 0x6f */
	&int10_p,				/* p - 0x70 */
	&int10_q,				/* q - 0x71 */
	&int10_r,				/* r - 0x72 */
	&int10_s,				/* s - 0x73 */
	&int10_t,				/* t - 0x74 */
	&int10_u,				/* u - 0x75 */
	&int10_v,				/* v - 0x76 */
	&int10_w,				/* w - 0x77 */
	&int10_x,				/* x - 0x78 */
	&int10_y,				/* y - 0x79 */
	&int10_z,				/* z - 0x7a */
	NULL,					/* { - 0x7b */
	NULL,					/* | - 0x7c */
	NULL,					/* } - 0x7d */
	NULL					/* ~ - 0x7e */
};

static image_info_t	*nba12_font_table[] = {
	&int12_exc,				/* ! - 0x21 */
	&int12_qota,			/* " - 0x22 */
	&int12_pnd,				/* # - 0x23 */
	&int12_csh,				/* $ - 0x24 */
	&int12_prct,			/* % - 0x25 */
	&int12_nnd,				/* & - 0x26 */
	&int12_qotb,			/* ' - 0x27 */
	&int12_lpar,			/* ( - 0x28 */
	&int12_rpar,			/* ) - 0x29 */
	&int12_ast,				/* * - 0x2a */
	&int12_pls,				/* + - 0x2b */
	&int12_com,				/* , - 0x2c */
	&int12_dsh,				/* - - 0x2d */
	&int12_per,				/* . - 0x2e */
	&int12_sls,				/* / - 0x2f */
	&int12_0,				/* 0 - 0x30 */
	&int12_1,				/* 1 - 0x31 */
	&int12_2,				/* 2 - 0x32 */
	&int12_3,				/* 3 - 0x33 */
	&int12_4,				/* 4 - 0x34 */
	&int12_5,				/* 5 - 0x35 */
	&int12_6,				/* 6 - 0x36 */
	&int12_7,				/* 7 - 0x37 */
	&int12_8,				/* 8 - 0x38 */
	&int12_9,				/* 9 - 0x39 */
	&int12_coln,			/* : - 0x3a */
	&int12_sem,				/* ; - 0x3b */
	&int12_arwl,			/* < - 0x3c */
	&int12_eql,				/* = - 0x3d */
	&int12_arwr,			/* > - 0x3e */
	&int12_que,				/* ? - 0x3f */
	&int12_ampr,			/* @ - 0x40 */
	&int12_a,				/* A - 0x41 */
	&int12_b,				/* B - 0x42 */
	&int12_c,				/* C - 0x43 */
	&int12_d,				/* D - 0x44 */
	&int12_e,				/* E - 0x45 */
	&int12_f,				/* F - 0x46 */
	&int12_g,				/* G - 0x47 */
	&int12_h,				/* H - 0x48 */
	&int12_i,				/* I - 0x49 */
	&int12_j,				/* J - 0x4a */
	&int12_k,				/* K - 0x4b */
	&int12_l,				/* L - 0x4c */
	&int12_m,				/* M - 0x4d */
	&int12_n,				/* N - 0x4e */
	&int12_o,				/* O - 0x4f */
	&int12_p,				/* P - 0x50 */
	&int12_q,				/* Q - 0x51 */
	&int12_r,				/* R - 0x52 */
	&int12_s,				/* S - 0x53 */
	&int12_t,				/* T - 0x54 */
	&int12_u,				/* U - 0x55 */
	&int12_v,				/* V - 0x56 */
	&int12_w,				/* W - 0x57 */
	&int12_x,				/* X - 0x58 */
	&int12_y,				/* Y - 0x59 */
	&int12_z,				/* Z - 0x5a */
	NULL,					/* [ - 0x5b */
	NULL,					/* \ - 0x5c */
	NULL,					/* ] - 0x5d */
	NULL,					/* ^ - 0x5e */
	NULL,					/*   - 0x5f */
	NULL,					/*   - 0x60 */
	&int12_a,				/* a - 0x61 */
	&int12_b,				/* b - 0x62 */
	&int12_c,				/* c - 0x63 */
	&int12_d,				/* d - 0x64 */
	&int12_e,				/* e - 0x65 */
	&int12_f,				/* f - 0x66 */
	&int12_g,				/* g - 0x67 */
	&int12_h,				/* h - 0x68 */
	&int12_i,				/* i - 0x69 */
	&int12_j,				/* j - 0x6a */
	&int12_k,				/* k - 0x6b */
	&int12_l,				/* l - 0x6c */
	&int12_m,				/* m - 0x6d */
	&int12_n,				/* n - 0x6e */
	&int12_o,				/* o - 0x6f */
	&int12_p,				/* p - 0x70 */
	&int12_q,				/* q - 0x71 */
	&int12_r,				/* r - 0x72 */
	&int12_s,				/* s - 0x73 */
	&int12_t,				/* t - 0x74 */
	&int12_u,				/* u - 0x75 */
	&int12_v,				/* v - 0x76 */
	&int12_w,				/* w - 0x77 */
	&int12_x,				/* x - 0x78 */
	&int12_y,				/* y - 0x79 */
	&int12_z,				/* z - 0x7a */
	NULL,					/* { - 0x7b */
	NULL,					/* | - 0x7c */
	NULL,					/* } - 0x7d */
	NULL					/* ~ - 0x7e */
};

static image_info_t	*nba14_font_table[] = {
	&int14_exc,				/* ! - 0x21 */
	&int14_qota,			/* " - 0x22 */
	&int14_pnd,				/* # - 0x23 */
	&int14_csh,				/* $ - 0x24 */
	&int14_prct,			/* % - 0x25 */
	&int14_nnd,				/* & - 0x26 */
	&int14_qotb,			/* ' - 0x27 */
	&int14_lpar,			/* ( - 0x28 */
	&int14_rpar,			/* ) - 0x29 */
	&int14_ast,				/* * - 0x2a */
	&int14_pls,				/* + - 0x2b */
	&int14_com,				/* , - 0x2c */
	&int14_dsh,				/* - - 0x2d */
	&int14_per,				/* . - 0x2e */
	&int14_sls,				/* / - 0x2f */
	&int14_0,				/* 0 - 0x30 */
	&int14_1,				/* 1 - 0x31 */
	&int14_2,				/* 2 - 0x32 */
	&int14_3,				/* 3 - 0x33 */
	&int14_4,				/* 4 - 0x34 */
	&int14_5,				/* 5 - 0x35 */
	&int14_6,				/* 6 - 0x36 */
	&int14_7,				/* 7 - 0x37 */
	&int14_8,				/* 8 - 0x38 */
	&int14_9,				/* 9 - 0x39 */
	&int14_col,				/* : - 0x3a */
	&int14_sem,				/* ; - 0x3b */
	&int14_arwl,			/* < - 0x3c */
	&int14_eql,				/* = - 0x3d */
	&int14_arwr,			/* > - 0x3e */
	&int14_que,				/* ? - 0x3f */
	&int14_ampr,			/* @ - 0x40 */
	&int14_a,				/* A - 0x41 */
	&int14_b,				/* B - 0x42 */
	&int14_c,				/* C - 0x43 */
	&int14_d,				/* D - 0x44 */
	&int14_e,				/* E - 0x45 */
	&int14_f,				/* F - 0x46 */
	&int14_g,				/* G - 0x47 */
	&int14_h,				/* H - 0x48 */
	&int14_i,				/* I - 0x49 */
	&int14_j,				/* J - 0x4a */
	&int14_k,				/* K - 0x4b */
	&int14_l,				/* L - 0x4c */
	&int14_m,				/* M - 0x4d */
	&int14_n,				/* N - 0x4e */
	&int14_o,				/* O - 0x4f */
	&int14_p,				/* P - 0x50 */
	&int14_q,				/* Q - 0x51 */
	&int14_r,				/* R - 0x52 */
	&int14_s,				/* S - 0x53 */
	&int14_t,				/* T - 0x54 */
	&int14_u,				/* U - 0x55 */
	&int14_v,				/* V - 0x56 */
	&int14_w,				/* W - 0x57 */
	&int14_x,				/* X - 0x58 */
	&int14_y,				/* Y - 0x59 */
	&int14_z,				/* Z - 0x5a */
	NULL,					/* [ - 0x5b */
	NULL,					/* \ - 0x5c */
	NULL,					/* ] - 0x5d */
	NULL,					/* ^ - 0x5e */
	NULL,					/*   - 0x5f */
	NULL,					/*   - 0x60 */
	&int14_a,				/* a - 0x61 */
	&int14_b,				/* b - 0x62 */
	&int14_c,				/* c - 0x63 */
	&int14_d,				/* d - 0x64 */
	&int14_e,				/* e - 0x65 */
	&int14_f,				/* f - 0x66 */
	&int14_g,				/* g - 0x67 */
	&int14_h,				/* h - 0x68 */
	&int14_i,				/* i - 0x69 */
	&int14_j,				/* j - 0x6a */
	&int14_k,				/* k - 0x6b */
	&int14_l,				/* l - 0x6c */
	&int14_m,				/* m - 0x6d */
	&int14_n,				/* n - 0x6e */
	&int14_o,				/* o - 0x6f */
	&int14_p,				/* p - 0x70 */
	&int14_q,				/* q - 0x71 */
	&int14_r,				/* r - 0x72 */
	&int14_s,				/* s - 0x73 */
	&int14_t,				/* t - 0x74 */
	&int14_u,				/* u - 0x75 */
	&int14_v,				/* v - 0x76 */
	&int14_w,				/* w - 0x77 */
	&int14_x,				/* x - 0x78 */
	&int14_y,				/* y - 0x79 */
	&int14_z,				/* z - 0x7a */
	NULL,					/* { - 0x7b */
	NULL,					/* | - 0x7c */
	NULL,					/* } - 0x7d */
	NULL					/* ~ - 0x7e */
};

static image_info_t	*nba20_font_table[] = {
	&int20_exc,				/* ! - 0x21 */
	&int20_qota,			/* " - 0x22 */
	&int20_pnd,				/* # - 0x23 */
	&int20_csh,				/* $ - 0x24 */
	&int20_prct,			/* % - 0x25 */
	&int20_nnd,				/* & - 0x26 */
	&int20_qotb,			/* ' - 0x27 */
	&int20_lpar,			/* ( - 0x28 */
	&int20_rpar,			/* ) - 0x29 */
	&int20_ast,				/* * - 0x2a */
	&int20_pls,				/* + - 0x2b */
	&int20_com,				/* , - 0x2c */
	&int20_dsh,				/* - - 0x2d */
	&int20_per,				/* . - 0x2e */
	&int20_sls,				/* / - 0x2f */
	&int20_0,				/* 0 - 0x30 */
	&int20_1,				/* 1 - 0x31 */
	&int20_2,				/* 2 - 0x32 */
	&int20_3,				/* 3 - 0x33 */
	&int20_4,				/* 4 - 0x34 */
	&int20_5,				/* 5 - 0x35 */
	&int20_6,				/* 6 - 0x36 */
	&int20_7,				/* 7 - 0x37 */
	&int20_8,				/* 8 - 0x38 */
	&int20_9,				/* 9 - 0x39 */
	&int20_col,				/* : - 0x3a */
	&int20_sem,				/* ; - 0x3b */
	&int20_arwl,			/* < - 0x3c */
	&int20_eql,				/* = - 0x3d */
	&int20_arwr,			/* > - 0x3e */
	&int20_que,				/* ? - 0x3f */
	&int20_ampr,			/* @ - 0x40 */
	&int20_a,				/* A - 0x41 */
	&int20_b,				/* B - 0x42 */
	&int20_c,				/* C - 0x43 */
	&int20_d,				/* D - 0x44 */
	&int20_e,				/* E - 0x45 */
	&int20_f,				/* F - 0x46 */
	&int20_g,				/* G - 0x47 */
	&int20_h,				/* H - 0x48 */
	&int20_i,				/* I - 0x49 */
	&int20_j,				/* J - 0x4a */
	&int20_k,				/* K - 0x4b */
	&int20_l,				/* L - 0x4c */
	&int20_m,				/* M - 0x4d */
	&int20_n,				/* N - 0x4e */
	&int20_o,				/* O - 0x4f */
	&int20_p,				/* P - 0x50 */
	&int20_q,				/* Q - 0x51 */
	&int20_r,				/* R - 0x52 */
	&int20_s,				/* S - 0x53 */
	&int20_t,				/* T - 0x54 */
	&int20_u,				/* U - 0x55 */
	&int20_v,				/* V - 0x56 */
	&int20_w,				/* W - 0x57 */
	&int20_x,				/* X - 0x58 */
	&int20_y,				/* Y - 0x59 */
	&int20_z,				/* Z - 0x5a */
	NULL,					/* [ - 0x5b */
	NULL,					/* \ - 0x5c */
	NULL,					/* ] - 0x5d */
	NULL,					/* ^ - 0x5e */
	NULL,					/*   - 0x5f */
	NULL,					/*   - 0x60 */
	&int20_a,				/* a - 0x61 */
	&int20_b,				/* b - 0x62 */
	&int20_c,				/* c - 0x63 */
	&int20_d,				/* d - 0x64 */
	&int20_e,				/* e - 0x65 */
	&int20_f,				/* f - 0x66 */
	&int20_g,				/* g - 0x67 */
	&int20_h,				/* h - 0x68 */
	&int20_i,				/* i - 0x69 */
	&int20_j,				/* j - 0x6a */
	&int20_k,				/* k - 0x6b */
	&int20_l,				/* l - 0x6c */
	&int20_m,				/* m - 0x6d */
	&int20_n,				/* n - 0x6e */
	&int20_o,				/* o - 0x6f */
	&int20_p,				/* p - 0x70 */
	&int20_q,				/* q - 0x71 */
	&int20_r,				/* r - 0x72 */
	&int20_s,				/* s - 0x73 */
	&int20_t,				/* t - 0x74 */
	&int20_u,				/* u - 0x75 */
	&int20_v,				/* v - 0x76 */
	&int20_w,				/* w - 0x77 */
	&int20_x,				/* x - 0x78 */
	&int20_y,				/* y - 0x79 */
	&int20_z,				/* z - 0x7a */
	NULL,					/* { - 0x7b */
	NULL,					/* | - 0x7c */
	NULL,					/* } - 0x7d */
	NULL					/* ~ - 0x7e */
};

static image_info_t	*nba25_font_table[] = {
	&int25_exc,				/* ! - 0x21 */
	&int25_qota,			/* " - 0x22 */
	&int25_pnd,				/* # - 0x23 */
	&int25_csh,				/* $ - 0x24 */
	&int25_prct,			/* % - 0x25 */
	&int25_nnd,				/* & - 0x26 */
	&int25_qotb,			/* ' - 0x27 */
	&int25_lpar,			/* ( - 0x28 */
	&int25_rpar,			/* ) - 0x29 */
	&int25_ast,				/* * - 0x2a */
	&int25_pls,				/* + - 0x2b */
	&int25_com,				/* , - 0x2c */
	&int25_dsh,				/* - - 0x2d */
	&int25_per,				/* . - 0x2e */
	&int25_sls,				/* / - 0x2f */
	&int25_0,				/* 0 - 0x30 */
	&int25_1,				/* 1 - 0x31 */
	&int25_2,				/* 2 - 0x32 */
	&int25_3,				/* 3 - 0x33 */
	&int25_4,				/* 4 - 0x34 */
	&int25_5,				/* 5 - 0x35 */
	&int25_6,				/* 6 - 0x36 */
	&int25_7,				/* 7 - 0x37 */
	&int25_8,				/* 8 - 0x38 */
	&int25_9,				/* 9 - 0x39 */
	&int25_col,				/* : - 0x3a */
	&int25_sem,				/* ; - 0x3b */
	&int25_arwl,			/* < - 0x3c */
	&int25_eql,				/* = - 0x3d */
	&int25_arwr,			/* > - 0x3e */
	&int25_que,				/* ? - 0x3f */
	&int25_ampr,			/* @ - 0x40 */
	&int25_a,				/* A - 0x41 */
	&int25_b,				/* B - 0x42 */
	&int25_c,				/* C - 0x43 */
	&int25_d,				/* D - 0x44 */
	&int25_e,				/* E - 0x45 */
	&int25_f,				/* F - 0x46 */
	&int25_g,				/* G - 0x47 */
	&int25_h,				/* H - 0x48 */
	&int25_i,				/* I - 0x49 */
	&int25_j,				/* J - 0x4a */
	&int25_k,				/* K - 0x4b */
	&int25_l,				/* L - 0x4c */
	&int25_m,				/* M - 0x4d */
	&int25_n,				/* N - 0x4e */
	&int25_o,				/* O - 0x4f */
	&int25_p,				/* P - 0x50 */
	&int25_q,				/* Q - 0x51 */
	&int25_r,				/* R - 0x52 */
	&int25_s,				/* S - 0x53 */
	&int25_t,				/* T - 0x54 */
	&int25_u,				/* U - 0x55 */
	&int25_v,				/* V - 0x56 */
	&int25_w,				/* W - 0x57 */
	&int25_x,				/* X - 0x58 */
	&int25_y,				/* Y - 0x59 */
	&int25_z,				/* Z - 0x5a */
	NULL,					/* [ - 0x5b */
	NULL,					/* \ - 0x5c */
	NULL,					/* ] - 0x5d */
	NULL,					/* ^ - 0x5e */
	NULL,					/*   - 0x5f */
	NULL,					/*   - 0x60 */
	&int25_a,				/* a - 0x61 */
	&int25_b,				/* b - 0x62 */
	&int25_c,				/* c - 0x63 */
	&int25_d,				/* d - 0x64 */
	&int25_e,				/* e - 0x65 */
	&int25_f,				/* f - 0x66 */
	&int25_g,				/* g - 0x67 */
	&int25_h,				/* h - 0x68 */
	&int25_i,				/* i - 0x69 */
	&int25_j,				/* j - 0x6a */
	&int25_k,				/* k - 0x6b */
	&int25_l,				/* l - 0x6c */
	&int25_m,				/* m - 0x6d */
	&int25_n,				/* n - 0x6e */
	&int25_o,				/* o - 0x6f */
	&int25_p,				/* p - 0x70 */
	&int25_q,				/* q - 0x71 */
	&int25_r,				/* r - 0x72 */
	&int25_s,				/* s - 0x73 */
	&int25_t,				/* t - 0x74 */
	&int25_u,				/* u - 0x75 */
	&int25_v,				/* v - 0x76 */
	&int25_w,				/* w - 0x77 */
	&int25_x,				/* x - 0x78 */
	&int25_y,				/* y - 0x79 */
	&int25_z,				/* z - 0x7a */
	NULL,					/* { - 0x7b */
	NULL,					/* | - 0x7c */
	NULL,					/* } - 0x7d */
	NULL					/* ~ - 0x7e */
};

static image_info_t	*nba23_font_table[] = {
	&int23_exc,				/* ! - 0x21 */
	&int23_qota,			/* " - 0x22 */
	&int23_pnd,				/* # - 0x23 */
	&int23_csh,				/* $ - 0x24 */
	&int23_prct,			/* % - 0x25 */
	&int23_nnd,				/* & - 0x26 */
	&int23_qotb,			/* ' - 0x27 */
	&int23_lpar,			/* ( - 0x28 */
	&int23_rpar,			/* ) - 0x29 */
	&int23_ast,				/* * - 0x2a */
	&int23_pls,				/* + - 0x2b */
	&int23_com,				/* , - 0x2c */
	&int23_dsh,				/* - - 0x2d */
	&int23_per,				/* . - 0x2e */
	&int23_sls,				/* / - 0x2f */
	&int23_0,				/* 0 - 0x30 */
	&int23_1,				/* 1 - 0x31 */
	&int23_2,				/* 2 - 0x32 */
	&int23_3,				/* 3 - 0x33 */
	&int23_4,				/* 4 - 0x34 */
	&int23_5,				/* 5 - 0x35 */
	&int23_6,				/* 6 - 0x36 */
	&int23_7,				/* 7 - 0x37 */
	&int23_8,				/* 8 - 0x38 */
	&int23_9,				/* 9 - 0x39 */
	&int23_col,				/* : - 0x3a */
	&int23_sem,				/* ; - 0x3b */
	&int23_arwl,			/* < - 0x3c */
	&int23_eql,				/* = - 0x3d */
	&int23_arwr,			/* > - 0x3e */
	&int23_que,				/* ? - 0x3f */
	&int23_ampr,			/* @ - 0x40 */
	&int23_a,				/* A - 0x41 */
	&int23_b,				/* B - 0x42 */
	&int23_c,				/* C - 0x43 */
	&int23_d,				/* D - 0x44 */
	&int23_e,				/* E - 0x45 */
	&int23_f,				/* F - 0x46 */
	&int23_g,				/* G - 0x47 */
	&int23_h,				/* H - 0x48 */
	&int23_i,				/* I - 0x49 */
	&int23_j,				/* J - 0x4a */
	&int23_k,				/* K - 0x4b */
	&int23_l,				/* L - 0x4c */
	&int23_m,				/* M - 0x4d */
	&int23_n,				/* N - 0x4e */
	&int23_o,				/* O - 0x4f */
	&int23_p,				/* P - 0x50 */
	&int23_q,				/* Q - 0x51 */
	&int23_r,				/* R - 0x52 */
	&int23_s,				/* S - 0x53 */
	&int23_t,				/* T - 0x54 */
	&int23_u,				/* U - 0x55 */
	&int23_v,				/* V - 0x56 */
	&int23_w,				/* W - 0x57 */
	&int23_x,				/* X - 0x58 */
	&int23_y,				/* Y - 0x59 */
	&int23_z,				/* Z - 0x5a */
	NULL,					/* [ - 0x5b */
	NULL,					/* \ - 0x5c */
	NULL,					/* ] - 0x5d */
	NULL,					/* ^ - 0x5e */
	NULL,					/*   - 0x5f */
	NULL,					/*   - 0x60 */
	&int23_a,				/* a - 0x61 */
	&int23_b,				/* b - 0x62 */
	&int23_c,				/* c - 0x63 */
	&int23_d,				/* d - 0x64 */
	&int23_e,				/* e - 0x65 */
	&int23_f,				/* f - 0x66 */
	&int23_g,				/* g - 0x67 */
	&int23_h,				/* h - 0x68 */
	&int23_i,				/* i - 0x69 */
	&int23_j,				/* j - 0x6a */
	&int23_k,				/* k - 0x6b */
	&int23_l,				/* l - 0x6c */
	&int23_m,				/* m - 0x6d */
	&int23_n,				/* n - 0x6e */
	&int23_o,				/* o - 0x6f */
	&int23_p,				/* p - 0x70 */
	&int23_q,				/* q - 0x71 */
	&int23_r,				/* r - 0x72 */
	&int23_s,				/* s - 0x73 */
	&int23_t,				/* t - 0x74 */
	&int23_u,				/* u - 0x75 */
	&int23_v,				/* v - 0x76 */
	&int23_w,				/* w - 0x77 */
	&int23_x,				/* x - 0x78 */
	&int23_y,				/* y - 0x79 */
	&int23_z,				/* z - 0x7a */
	NULL,					/* { - 0x7b */
	NULL,					/* | - 0x7c */
	NULL,					/* } - 0x7d */
	NULL					/* ~ - 0x7e */
};

/* NBA 10 font */
font_info_t	nba10_font = {
	sizeof(nba10_font_table) / sizeof(nba10_font_table[0]),
	0x21,
	0x7e,
	14,
	4,
	-2,
	nbafn3_tex_name,
	nba10_font_table,
	0
};

/* NBA 12 font */
font_info_t	nba12_font = {
	sizeof(nba12_font_table) / sizeof(nba12_font_table[0]),
	0x21,
	0x7e,
	16,
	5,
	-3,
	nbafn2_tex_name,
	nba12_font_table,
	0
};

/* NBA 14 font */
font_info_t	nba14_font = {
	sizeof(nba14_font_table) / sizeof(nba14_font_table[0]),
	0x21,
	0x7e,
	18,
	6,
	-2,
	nbafn2_tex_name,
	nba14_font_table,
	0
};

/* NBA 20 font */
font_info_t	nba20_font = {
	sizeof(nba20_font_table) / sizeof(nba20_font_table[0]),
	0x21,
	0x7e,
	24,
	7,
	-2,
	nbafn0_tex_name,
	nba20_font_table,
	0
};

/* NBA 25 font */
font_info_t	nba25_font = {
	sizeof(nba25_font_table) / sizeof(nba25_font_table[0]),
	0x21,
	0x7e,
	28,
	11,
	-3,
	nbafn1_tex_name,
	nba25_font_table,
	0
};

/* NBA 23 font */
font_info_t	nba23_font = {
	sizeof(nba23_font_table) / sizeof(nba23_font_table[0]),
	0x21,
	0x7e,
	26,
	9,
	-2,
	nbafn0_tex_name,
	nba23_font_table,
	0
};
/* # chars in font */
/* start char value */
/*  end char value */
/*  pixel height */
/*  width of space */
/*  inter-character spacing (pixels) */
/*  pointer to texture name */
/*  pointer to array of char image info */
/*  Scale font size (for low res) */

static image_info_t	*ctn14_font_table[] = {
	&ctn14exc,				/* ! - 0x21 */
	&ctn14qot,				/* " - 0x22 */
	&ctn14pnd,				/* # - 0x23 */
	&ctn14csh,				/* $ - 0x24 */
	&ctn14pct,				/* % - 0x25 */
	&ctn14nnd,				/* & - 0x26 */
	&ctn14qt2,				/* ' - 0x27 */
	&ctn14lpr,				/* ( - 0x28 */
	&ctn14rpr,				/* ) - 0x29 */
	&ctn14ast,				/* * - 0x2a */
	&ctn14pls,				/* + - 0x2b */
	&ctn14com,				/* , - 0x2c */
	&ctn14mns,				/* - - 0x2d */
	&ctn14per,				/* . - 0x2e */
	&ctn14sls,				/* / - 0x2f */
	&ctn140,				/* 0 - 0x30 */
	&ctn141,				/* 1 - 0x31 */
	&ctn142,				/* 2 - 0x32 */
	&ctn143,				/* 3 - 0x33 */
	&ctn144,				/* 4 - 0x34 */
	&ctn145,				/* 5 - 0x35 */
	&ctn146,				/* 6 - 0x36 */
	&ctn147,				/* 7 - 0x37 */
	&ctn148,				/* 8 - 0x38 */
	&ctn149,				/* 9 - 0x39 */
	&ctn14col,				/* : - 0x3a */
	&ctn14sco,				/* ; - 0x3b */
	&ctn14lbk,				/* < - 0x3c */
	&ctn14eql,				/* = - 0x3d */
	&ctn14rbk,				/* > - 0x3e */
	&ctn14qst,				/* ? - 0x3f */
	&ctn14amp,				/* @ - 0x40 */
	&ctn14a,				/* A - 0x41 */
	&ctn14b,				/* B - 0x42 */
	&ctn14c,				/* C - 0x43 */
	&ctn14d,				/* D - 0x44 */
	&ctn14e,				/* E - 0x45 */
	&ctn14f,				/* F - 0x46 */
	&ctn14g,				/* G - 0x47 */
	&ctn14h,				/* H - 0x48 */
	&ctn14i,				/* I - 0x49 */
	&ctn14j,				/* J - 0x4a */
	&ctn14k,				/* K - 0x4b */
	&ctn14l,				/* L - 0x4c */
	&ctn14m,				/* M - 0x4d */
	&ctn14n,				/* N - 0x4e */
	&ctn14o,				/* O - 0x4f */
	&ctn14p,				/* P - 0x50 */
	&ctn14q,				/* Q - 0x51 */
	&ctn14r,				/* R - 0x52 */
	&ctn14s,				/* S - 0x53 */
	&ctn14t,				/* T - 0x54 */
	&ctn14u,				/* U - 0x55 */
	&ctn14v,				/* V - 0x56 */
	&ctn14w,				/* W - 0x57 */
	&ctn14x,				/* X - 0x58 */
	&ctn14y,				/* Y - 0x59 */
	&ctn14z,				/* Z - 0x5a */
	NULL,					/* [ - 0x5b */
	NULL,					/* \ - 0x5c */
	NULL,					/* ] - 0x5d */
	NULL,					/* ^ - 0x5e */
	NULL,					/*   - 0x5f */
	NULL,					/*   - 0x60 */
	&ctn14a,				/* a - 0x61 */
	&ctn14b,				/* b - 0x62 */
	&ctn14c,				/* c - 0x63 */
	&ctn14d,				/* d - 0x64 */
	&ctn14e,				/* e - 0x65 */
	&ctn14f,				/* f - 0x66 */
	&ctn14g,				/* g - 0x67 */
	&ctn14h,				/* h - 0x68 */
	&ctn14i,				/* i - 0x69 */
	&ctn14j,				/* j - 0x6a */
	&ctn14k,				/* k - 0x6b */
	&ctn14l,				/* l - 0x6c */
	&ctn14m,				/* m - 0x6d */
	&ctn14n,				/* n - 0x6e */
	&ctn14o,				/* o - 0x6f */
	&ctn14p,				/* p - 0x70 */
	&ctn14q,				/* q - 0x71 */
	&ctn14r,				/* r - 0x72 */
	&ctn14s,				/* s - 0x73 */
	&ctn14t,				/* t - 0x74 */
	&ctn14u,				/* u - 0x75 */
	&ctn14v,				/* v - 0x76 */
	&ctn14w,				/* w - 0x77 */
	&ctn14x,				/* x - 0x78 */
	&ctn14y,				/* y - 0x79 */
	&ctn14z,				/* z - 0x7a */
	NULL,					/* { - 0x7b */
	NULL,					/* | - 0x7c */
	NULL,					/* } - 0x7d */
	NULL					/* ~ - 0x7e */
};
static image_info_t	*ctn20_font_table[] = {
	&ctn20exc,				/* ! - 0x21 */
	&ctn20qot,				/* " - 0x22 */
	&ctn20pnd,				/* # - 0x23 */
	&ctn20csh,				/* $ - 0x24 */
	&ctn20pct,				/* % - 0x25 */
	&ctn20nnd,				/* & - 0x26 */
	&ctn20qt2,				/* ' - 0x27 */
	&ctn20lpr,				/* ( - 0x28 */
	&ctn20rpr,				/* ) - 0x29 */
	&ctn20ast,				/* * - 0x2a */
	&ctn20pls,				/* + - 0x2b */
	&ctn20com,				/* , - 0x2c */
	&ctn20mns,				/* - - 0x2d */
	&ctn20per,				/* . - 0x2e */
	&ctn20sls,				/* / - 0x2f */
	&ctn200,				/* 0 - 0x30 */
	&ctn201,				/* 1 - 0x31 */
	&ctn202,				/* 2 - 0x32 */
	&ctn203,				/* 3 - 0x33 */
	&ctn204,				/* 4 - 0x34 */
	&ctn205,				/* 5 - 0x35 */
	&ctn206,				/* 6 - 0x36 */
	&ctn207,				/* 7 - 0x37 */
	&ctn208,				/* 8 - 0x38 */
	&ctn209,				/* 9 - 0x39 */
	&ctn20col,				/* : - 0x3a */
	&ctn20sco,				/* ; - 0x3b */
	&ctn20lbk,				/* < - 0x3c */
	&ctn20eql,				/* = - 0x3d */
	&ctn20rbk,				/* > - 0x3e */
	&ctn20qst,				/* ? - 0x3f */
	&ctn20amp,				/* @ - 0x40 */
	&ctn20a,				/* A - 0x41 */
	&ctn20b,				/* B - 0x42 */
	&ctn20c,				/* C - 0x43 */
	&ctn20d,				/* D - 0x44 */
	&ctn20e,				/* E - 0x45 */
	&ctn20f,				/* F - 0x46 */
	&ctn20g,				/* G - 0x47 */
	&ctn20h,				/* H - 0x48 */
	&ctn20i,				/* I - 0x49 */
	&ctn20j,				/* J - 0x4a */
	&ctn20k,				/* K - 0x4b */
	&ctn20l,				/* L - 0x4c */
	&ctn20m,				/* M - 0x4d */
	&ctn20n,				/* N - 0x4e */
	&ctn20o,				/* O - 0x4f */
	&ctn20p,				/* P - 0x50 */
	&ctn20q,				/* Q - 0x51 */
	&ctn20r,				/* R - 0x52 */
	&ctn20s,				/* S - 0x53 */
	&ctn20t,				/* T - 0x54 */
	&ctn20u,				/* U - 0x55 */
	&ctn20v,				/* V - 0x56 */
	&ctn20w,				/* W - 0x57 */
	&ctn20x,				/* X - 0x58 */
	&ctn20y,				/* Y - 0x59 */
	&ctn20z,				/* Z - 0x5a */
	NULL,					/* [ - 0x5b */
	NULL,					/* \ - 0x5c */
	NULL,					/* ] - 0x5d */
	NULL,					/* ^ - 0x5e */
	NULL,					/*   - 0x5f */
	NULL,					/*   - 0x60 */
	&ctn20a,				/* a - 0x61 */
	&ctn20b,				/* b - 0x62 */
	&ctn20c,				/* c - 0x63 */
	&ctn20d,				/* d - 0x64 */
	&ctn20e,				/* e - 0x65 */
	&ctn20f,				/* f - 0x66 */
	&ctn20g,				/* g - 0x67 */
	&ctn20h,				/* h - 0x68 */
	&ctn20i,				/* i - 0x69 */
	&ctn20j,				/* j - 0x6a */
	&ctn20k,				/* k - 0x6b */
	&ctn20l,				/* l - 0x6c */
	&ctn20m,				/* m - 0x6d */
	&ctn20n,				/* n - 0x6e */
	&ctn20o,				/* o - 0x6f */
	&ctn20p,				/* p - 0x70 */
	&ctn20q,				/* q - 0x71 */
	&ctn20r,				/* r - 0x72 */
	&ctn20s,				/* s - 0x73 */
	&ctn20t,				/* t - 0x74 */
	&ctn20u,				/* u - 0x75 */
	&ctn20v,				/* v - 0x76 */
	&ctn20w,				/* w - 0x77 */
	&ctn20x,				/* x - 0x78 */
	&ctn20y,				/* y - 0x79 */
	&ctn20z,				/* z - 0x7a */
	NULL,					/* { - 0x7b */
	NULL,					/* | - 0x7c */
	NULL,					/* } - 0x7d */
	NULL					/* ~ - 0x7e */
};
static image_info_t	*ctn25_font_table[] = {
	&ctn25exc,				/* ! - 0x21 */
	&ctn25qot,				/* " - 0x22 */
	&ctn25pnd,				/* # - 0x23 */
	&ctn25csh,				/* $ - 0x24 */
	&ctn25pct,				/* % - 0x25 */
	&ctn25nnd,				/* & - 0x26 */
	&ctn25qt2,				/* ' - 0x27 */
	&ctn25lpr,				/* ( - 0x28 */
	&ctn25rpr,				/* ) - 0x29 */
	&ctn25ast,				/* * - 0x2a */
	&ctn25pls,				/* + - 0x2b */
	&ctn25com,				/* , - 0x2c */
	&ctn25mns,				/* - - 0x2d */
	&ctn25per,				/* . - 0x2e */
	&ctn25sls,				/* / - 0x2f */
	&ctn250,				/* 0 - 0x30 */
	&ctn251,				/* 1 - 0x31 */
	&ctn252,				/* 2 - 0x32 */
	&ctn253,				/* 3 - 0x33 */
	&ctn254,				/* 4 - 0x34 */
	&ctn255,				/* 5 - 0x35 */
	&ctn256,				/* 6 - 0x36 */
	&ctn257,				/* 7 - 0x37 */
	&ctn258,				/* 8 - 0x38 */
	&ctn259,				/* 9 - 0x39 */
	&ctn25col,				/* : - 0x3a */
	&ctn25sco,				/* ; - 0x3b */
	&ctn25lbk,				/* < - 0x3c */
	&ctn25eql,				/* = - 0x3d */
	&ctn25rbk,				/* > - 0x3e */
	&ctn25qst,				/* ? - 0x3f */
	&ctn25amp,				/* @ - 0x40 */
	&ctn25a,				/* A - 0x41 */
	&ctn25b,				/* B - 0x42 */
	&ctn25c,				/* C - 0x43 */
	&ctn25d,				/* D - 0x44 */
	&ctn25e,				/* E - 0x45 */
	&ctn25f,				/* F - 0x46 */
	&ctn25g,				/* G - 0x47 */
	&ctn25h,				/* H - 0x48 */
	&ctn25i,				/* I - 0x49 */
	&ctn25j,				/* J - 0x4a */
	&ctn25k,				/* K - 0x4b */
	&ctn25l,				/* L - 0x4c */
	&ctn25m,				/* M - 0x4d */
	&ctn25n,				/* N - 0x4e */
	&ctn25o,				/* O - 0x4f */
	&ctn25p,				/* P - 0x50 */
	&ctn25q,				/* Q - 0x51 */
	&ctn25r,				/* R - 0x52 */
	&ctn25s,				/* S - 0x53 */
	&ctn25t,				/* T - 0x54 */
	&ctn25u,				/* U - 0x55 */
	&ctn25v,				/* V - 0x56 */
	&ctn25w,				/* W - 0x57 */
	&ctn25x,				/* X - 0x58 */
	&ctn25y,				/* Y - 0x59 */
	&ctn25z,				/* Z - 0x5a */
	NULL,					/* [ - 0x5b */
	NULL,					/* \ - 0x5c */
	NULL,					/* ] - 0x5d */
	NULL,					/* ^ - 0x5e */
	NULL,					/*   - 0x5f */
	NULL,					/*   - 0x60 */
	&ctn25a,				/* a - 0x61 */
	&ctn25b,				/* b - 0x62 */
	&ctn25c,				/* c - 0x63 */
	&ctn25d,				/* d - 0x64 */
	&ctn25e,				/* e - 0x65 */
	&ctn25f,				/* f - 0x66 */
	&ctn25g,				/* g - 0x67 */
	&ctn25h,				/* h - 0x68 */
	&ctn25i,				/* i - 0x69 */
	&ctn25j,				/* j - 0x6a */
	&ctn25k,				/* k - 0x6b */
	&ctn25l,				/* l - 0x6c */
	&ctn25m,				/* m - 0x6d */
	&ctn25n,				/* n - 0x6e */
	&ctn25o,				/* o - 0x6f */
	&ctn25p,				/* p - 0x70 */
	&ctn25q,				/* q - 0x71 */
	&ctn25r,				/* r - 0x72 */
	&ctn25s,				/* s - 0x73 */
	&ctn25t,				/* t - 0x74 */
	&ctn25u,				/* u - 0x75 */
	&ctn25v,				/* v - 0x76 */
	&ctn25w,				/* w - 0x77 */
	&ctn25x,				/* x - 0x78 */
	&ctn25y,				/* y - 0x79 */
	&ctn25z,				/* z - 0x7a */
	NULL,					/* { - 0x7b */
	NULL,					/* | - 0x7c */
	NULL,					/* } - 0x7d */
	NULL					/* ~ - 0x7e */
};

static image_info_t	*bast8to_font_table[] = {
	&bast8lexc,				/* ! - 0x21 */
	NULL,					/* " - 0x22 */
	&bast8lnum,				/* # - 0x23 */
	&bast8ldol,				/* $ - 0x24 */
	&bast8lpct,				/* % - 0x25 */
	&bast8land,				/* & - 0x26 */
	&bast8lapo,				/* ' - 0x27 */
	&bast8lprl,				/* ( - 0x28 */
	&bast8lprr,				/* ) - 0x29 */
	&bast8last,				/* * - 0x2a */
	&bast8lpls,				/* + - 0x2b */
	&bast8lcom,				/* , - 0x2c */
	&bast8ldas,				/* - - 0x2d */
	&bast8lper,				/* . - 0x2e */
	&bast8lsls,				/* / - 0x2f */
	&bast8l_0,				/* 0 - 0x30 */
	&bast8l_1,				/* 1 - 0x31 */
	&bast8l_2,				/* 2 - 0x32 */
	&bast8l_3,				/* 3 - 0x33 */
	&bast8l_4,				/* 4 - 0x34 */
	&bast8l_5,				/* 5 - 0x35 */
	&bast8l_6,				/* 6 - 0x36 */
	&bast8l_7,				/* 7 - 0x37 */
	&bast8l_8,				/* 8 - 0x38 */
	&bast8l_9,				/* 9 - 0x39 */
	&bast8lcol,				/* : - 0x3a */
	&bast8lsem,				/* ; - 0x3b */
	NULL,					/* < - 0x3c */
	NULL,					/* = - 0x3d */
	NULL,					/* > - 0x3e */
	&bast8lque,				/* ? - 0x3f */
	NULL,					/* @ - 0x40 */
	&bast8l_a,				/* A - 0x41 */
	&bast8l_b,				/* B - 0x42 */
	&bast8l_c,				/* C - 0x43 */
	&bast8l_d,				/* D - 0x44 */
	&bast8l_e,				/* E - 0x45 */
	&bast8l_f,				/* F - 0x46 */
	&bast8l_g,				/* G - 0x47 */
	&bast8l_h,				/* H - 0x48 */
	&bast8l_i,				/* I - 0x49 */
	&bast8l_j,				/* J - 0x4a */
	&bast8l_k,				/* K - 0x4b */
	&bast8l_l,				/* L - 0x4c */
	&bast8l_m,				/* M - 0x4d */
	&bast8l_n,				/* N - 0x4e */
	&bast8l_o,				/* O - 0x4f */
	&bast8l_p,				/* P - 0x50 */
	&bast8l_q,				/* Q - 0x51 */
	&bast8l_r,				/* R - 0x52 */
	&bast8l_s,				/* S - 0x53 */
	&bast8l_t,				/* T - 0x54 */
	&bast8l_u,				/* U - 0x55 */
	&bast8l_v,				/* V - 0x56 */
	&bast8l_w,				/* W - 0x57 */
	&bast8l_x,				/* X - 0x58 */
	&bast8l_y,				/* Y - 0x59 */
	&bast8l_z,				/* Z - 0x5a */
	NULL,					/* [ - 0x5b */
	NULL,					/* \ - 0x5c */
	NULL,					/* ] - 0x5d */
	NULL,					/* ^ - 0x5e */
	NULL,					/*   - 0x5f */
	NULL,					/*   - 0x60 */
	&bast8l_a,				/* a - 0x61 */
	&bast8l_b,				/* b - 0x62 */
	&bast8l_c,				/* c - 0x63 */
	&bast8l_d,				/* d - 0x64 */
	&bast8l_e,				/* e - 0x65 */
	&bast8l_f,				/* f - 0x66 */
	&bast8l_g,				/* g - 0x67 */
	&bast8l_h,				/* h - 0x68 */
	&bast8l_i,				/* i - 0x69 */
	&bast8l_j,				/* j - 0x6a */
	&bast8l_k,				/* k - 0x6b */
	&bast8l_l,				/* l - 0x6c */
	&bast8l_m,				/* m - 0x6d */
	&bast8l_n,				/* n - 0x6e */
	&bast8l_o,				/* o - 0x6f */
	&bast8l_p,				/* p - 0x70 */
	&bast8l_q,				/* q - 0x71 */
	&bast8l_r,				/* r - 0x72 */
	&bast8l_s,				/* s - 0x73 */
	&bast8l_t,				/* t - 0x74 */
	&bast8l_u,				/* u - 0x75 */
	&bast8l_v,				/* v - 0x76 */
	&bast8l_w,				/* w - 0x77 */
	&bast8l_x,				/* x - 0x78 */
	&bast8l_y,				/* y - 0x79 */
	&bast8l_z,				/* z - 0x7a */
	NULL,					/* { - 0x7b */
	NULL,					/* | - 0x7c */
	NULL,					/* } - 0x7d */
	NULL					/* ~ - 0x7e */
};

static image_info_t	*bast8ta_font_table[] = {
	&bast8taexc,				/* ! - 0x21 */
	NULL,						/* " - 0x22 */
	&bast8tanum,				/* # - 0x23 */
	&bast8tadol,				/* $ - 0x24 */
	&bast8tapct,				/* % - 0x25 */
	&bast8taand,				/* & - 0x26 */
	&bast8taapo,				/* ' - 0x27 */
	&bast8taprl,				/* ( - 0x28 */
	&bast8taprr,				/* ) - 0x29 */
	&bast8taast,				/* * - 0x2a */
	&bast8tapls,				/* + - 0x2b */
	&bast8tacom,				/* , - 0x2c */
	&bast8tadas,				/* - - 0x2d */
	&bast8taper,				/* . - 0x2e */
	&bast8tasls,				/* / - 0x2f */
	&bast8ta_0,					/* 0 - 0x30 */
	&bast8ta_1,					/* 1 - 0x31 */
	&bast8ta_2,					/* 2 - 0x32 */
	&bast8ta_3,					/* 3 - 0x33 */
	&bast8ta_4,					/* 4 - 0x34 */
	&bast8ta_5,					/* 5 - 0x35 */
	&bast8ta_6,					/* 6 - 0x36 */
	&bast8ta_7,					/* 7 - 0x37 */
	&bast8ta_8,					/* 8 - 0x38 */
	&bast8ta_9,					/* 9  - 0x39 */
	&bast8tacol,				/* : - 0x3a */
	&bast8tasem,				/* ; - 0x3b */
	(void *)0,					/* < - 0x3c */
	(void *)0,					/* = - 0x3d */
	(void *)0,					/* > - 0x3e */
	&bast8taque,				/* ? - 0x3f */
	(void *)0,					/* @ - 0x40 */
	&bast8ta_a,					/* A - 0x41 */
	&bast8ta_b,					/* B - 0x42 */
	&bast8ta_c,					/* C - 0x43 */
	&bast8ta_d,					/* D - 0x44 */
	&bast8ta_e,					/* E - 0x45 */
	&bast8ta_f,					/* F - 0x46 */
	&bast8ta_g,					/* G - 0x47 */
	&bast8ta_h,					/* H - 0x48 */
	&bast8ta_i,					/* I - 0x49 */
	&bast8ta_j,					/* J - 0x4a */
	&bast8ta_k,					/* K - 0x4b */
	&bast8ta_l,					/* L - 0x4c */
	&bast8ta_m,					/* M - 0x4d */
	&bast8ta_n,					/* N - 0x4e */
	&bast8ta_o,					/* O - 0x4f */
	&bast8ta_p,					/* P - 0x50 */
	&bast8ta_q,					/* Q - 0x51 */
	&bast8ta_r,					/* R - 0x52 */
	&bast8ta_s,					/* S - 0x53 */
	&bast8ta_t,					/* T - 0x54 */
	&bast8ta_u,					/* U - 0x55 */
	&bast8ta_v,					/* V - 0x56 */
	&bast8ta_w,					/* W - 0x57 */
	&bast8ta_x,					/* X - 0x58 */
	&bast8ta_y,					/* Y - 0x59 */
	&bast8ta_z,					/* Z - 0x5a */
	(void *)0,					/* [ - 0x5b */
	(void *)0,					/* \ - 0x5c */
	(void *)0,					/* ] - 0x5d */
	(void *)0,					/* ^ - 0x5e */
	&bast8tadas,				/*   - 0x5f */
	(void *)0,					/*   - 0x60 */
	&bast8ta_a,					/* a - 0x61 */
	&bast8ta_b,					/* b - 0x62 */
	&bast8ta_c,					/* c - 0x63 */
	&bast8ta_d,					/* d - 0x64 */
	&bast8ta_e,					/* e - 0x65 */
	&bast8ta_f,					/* f - 0x66 */
	&bast8ta_g,					/* g - 0x67 */
	&bast8ta_h,					/* h - 0x68 */
	&bast8ta_i,					/* i - 0x69 */
	&bast8ta_j,					/* j - 0x6a */
	&bast8ta_k,					/* k - 0x6b */
	&bast8ta_l,					/* l - 0x6c */
	&bast8ta_m,					/* m - 0x6d */
	&bast8ta_n,					/* n - 0x6e */
	&bast8ta_o,					/* o - 0x6f */
	&bast8ta_p,					/* p - 0x70 */
	&bast8ta_q,					/* q - 0x71 */
	&bast8ta_r,					/* r - 0x72 */
	&bast8ta_s,					/* s - 0x73 */
	&bast8ta_t,					/* t - 0x74 */
	&bast8ta_u,					/* u - 0x75 */
	&bast8ta_v,					/* v - 0x76 */
	&bast8ta_w,					/* w - 0x77 */
	&bast8ta_x,					/* x - 0x78 */
	&bast8ta_y,					/* y - 0x79 */
	&bast8ta_z					/* z - 0x7a */
};

/* CTN 14 font */
font_info_t	ctn14_font = {
	sizeof(ctn14_font_table) / sizeof(ctn14_font_table[0]),
	0x21,
	0x7e,
	18,
	6,
	-2,
	nbafn0_tex_name,
	ctn14_font_table,
	0
};

/* CTN 20 font */
font_info_t	ctn20_font = {
	sizeof(ctn20_font_table) / sizeof(ctn20_font_table[0]),
	0x21,
	0x7e,
	24,
	7,
	-3,
	nbafn1_tex_name,
	ctn20_font_table,
	0
};

/* CTN 25 font */
font_info_t	ctn25_font = {
	sizeof(ctn25_font_table) / sizeof(ctn25_font_table[0]),
	0x21,
	0x7e,
	28,
	13,
	-4,
	nbafn2_tex_name,
	ctn25_font_table,
	0
};

/* BAST 8T outline font */
font_info_t	bast8to_font = {
	sizeof(bast8to_font_table) / sizeof(bast8to_font_table[0]),
	0x21,
	0x7e,
	12,
	5,
	-2,
	nbafn3_tex_name,
	bast8to_font_table,
	0
};

/* BAST 8T alpha font */
font_info_t	bast8ta_font = {
	sizeof(bast8ta_font_table) / sizeof(bast8ta_font_table[0]),
	0x21,
	0x7a,
	9,
	0x5,
	0x0,
	nbafn3_tex_name,
	bast8ta_font_table
};
