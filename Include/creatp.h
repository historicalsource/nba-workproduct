/*
 *		$Archive: /video/Nba/Include/creatp.h $
 *		$Revision: 6 $
 *		$Date: 3/31/99 12:34p $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifndef __CREATP_H__
#define __CREATP_H__

#if defined(DECLARE_GLOBALS) && defined(INCLUDE_SSID)
char *ss_creatp_h = "$Workfile: creatp.h $ $Revision: 6 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

/*
 *		USER INCLUDES
 */

/*
 *		DEFINES
 */

/*
 *		TYPEDEFS
 */

/*
 *		GLOBAL PROTOTYPES
 */

void creatp_proc(int *args);
void do_oval_sparkle(sprite_info_t *sprite, int w_scale, int h_scale, int short_sparkle, int clock, int wait);
void creatp_calc_height_weight(char cp_height, char cp_weight, char *height, short *weight);
int creatp_check_bad_name(char *name, int count);
char *creatp_index_to_nickname(int nickname_num);
void creatp_decode_hotspot(unsigned int data, int use_custom, float *x, float *z);

/*
 *		GLOBAL VARIABLES
 */

#endif
